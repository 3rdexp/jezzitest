#!/usr/bin/python
#coding:utf-8

__pychecker__ = 'no-callinit no-classattr'

import os
import logging

import tornado.web
import tornado.options
import tornado.ioloop
import tornado.httpserver

import pymongo

from tornado.options import define, options

import base
import antispam
import auth
import feed
import list2tree
import testtmpl

"""
第一批实现的 url
/auth/login/
/auth/logout/
/sign/

/

/publish/
/upload/

/guest/

/inbox/
/album/


/captcha/([^/]+)

静态
/s/
/s/h/ 头像
/s/p/ 相册
"""

class HomeHandler(base.BaseHandler):
  @tornado.web.addslash
  def get(self):
    user = self.current_user
    if not user:
      self.render('home.html', feeds=[], user=user)
      return

    ds = feed.Feed.Read(self.db, user)
    feeds = []
    if ds:
      for d in ds:
        t = list2tree.BuildTree(d['comments'])
        # print 'comments dict:', d['comments']
        d['comments'] = t
        feeds.append(base.PlainDict(d))
        #print 'comments tree:', t
        #print d

    # self.set_header("Expires", "-1")
    self.render('home.html', feeds=feeds, user=user)


class Square(tornado.web.Application):
  def __init__(self):
    handlers = [
      (r'/', HomeHandler),
      
      (r'/feed/comment', feed.FeedHandler),
      
      (r'/auth/login',    auth.AuthLoginHandler),
      (r'/auth/logout',   auth.AuthLogoutHandler),
      (r'/auth/name/([^/]+)',   auth.NameCheckHandler),
      (r'/sign/?',        auth.SignHandler),
      (r'/auth/setting/?', auth.SettingHandler), 
      
      (r'/captcha/([^/]+)', antispam.CaptchaHandler),
      
      (r'/publish/?',  feed.PublishHandler),
      (r'/upload/?',   feed.UploadHandler),
      
      (r'/j/([^/]+)',  feed.JsonHandler),
      (r'/test/([^/]+)', testtmpl.TestMustacheHandler),
    ]
    settings = dict(
      debug = True,
      cookie_secret='daydayup',
      static_url_prefix = '/s/',
      xsrf_cookies=True,
      
      template_path=os.path.join(os.path.dirname(__file__), "templates"),
      static_path=os.path.join(os.path.dirname(__file__), "static"),
      
      sign_captcha = False,
      
      ui_modules = {'UserModule' : auth.UserModule,
          'FeedModule' : feed.FeedModule,
          'FeedCommentModule' : feed.FeedCommentModule
        },
    )
    tornado.web.Application.__init__(self, handlers, **settings)

    self.db_ = None

  @property
  def db(self):
    if not self.db_:
      self.InitDatabase()
    return self.db_
    
  def InitDatabase(self):
    if not self.db_:
      # self.db_ = pymongo.Connection('localhost', 27017).square
      # 内部有连接池，数目为 10
      self.db_ = pymongo.Connection(options.dbhost,  options.dbport).square


tornado.options.define("port", default=1026, help="run on the given port", type=int)
tornado.options.define("dbhost", default='localhost', help="mongodb host", type=str)
tornado.options.define("dbport", default=27017, help="mongodb port", type=int)


def init():
  db = pymongo.Connection(options.dbhost,  options.dbport).square
  # db.user.ensureIndex({c : '2d', n : 1}, {min:-180000, max:180000})

  db.user.ensure_index('email', unique=True)
  db.user.ensure_index('nick', unique=True)
  
  db.focus.ensure_index([('center', pymongo.GEO2D)], unique=False, ttl=300,  min=-180*60000, max=180*60000)
  db.focus.ensure_index('uid', unique=False)
  
  db.feed.ensure_index([('where', pymongo.GEO2D)], unique=False, ttl=300,  min=-180*60000, max=180*60000)
  db.feed.ensure_index('time', unique=False)
  

def main():
  tornado.options.parse_command_line()
  
  init()
  logging.debug('hello')
  
  ws = tornado.httpserver.HTTPServer(Square())
  ws.listen(options.port)
  tornado.ioloop.IOLoop.instance().start()


if __name__ == "__main__":
  main()
