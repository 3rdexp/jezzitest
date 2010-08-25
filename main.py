#!/usr/bin/python
#coding:utf-8

import os

import tornado.web
import tornado.options
import tornado.ioloop
import tornado.httpserver

import pymongo

from tornado.options import define, options

import base
import antispam
import auth
import ugc

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

    # print user.name, user.id
    
    d = self.db.feedlist.find_one(dict(owner=user.id))
    feeds = []
    if d:
      fids = d['fid']
      # print 'fids', fids
      if fids:
        ds = self.db.feed.find({'_id' : {'$in': fids}}).limit(40).sort(u'time', pymongo.DESCENDING)
        for d in ds:
          feeds.append(base.PlainDict(d))
      
    self.render('home.html', feeds=feeds, user=user)

class Square(tornado.web.Application):
  def __init__(self):
    handlers = [
      (r'/', HomeHandler),
      
      (r'/feed/comment', ugc.FeedHandler),
      
      (r'/auth/login',    auth.AuthLoginHandler),
      (r'/auth/logout',   auth.AuthLogoutHandler),
      (r'/sign/?',        auth.SignHandler),
      
      (r'/captcha/([^/]+)', antispam.CaptchaHandler),
      
      (r'/publish/',  ugc.PublishHandler),
      (r'/upload/',   ugc.UploadHandler),
    ]
    settings = dict(
      debug = True,
      cookie_secret='daydayup',
      static_url_prefix = '/s/',
      
      template_path=os.path.join(os.path.dirname(__file__), "templates"),
      static_path=os.path.join(os.path.dirname(__file__), "static"),
      
      sign_captcha = False,
      
      ui_modules = {'UserModule' : auth.UserModule,
          'FeedModule' : ugc.FeedModule
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
      self.db_ = pymongo.Connection('localhost', 27017).square

tornado.options.define("port", default=5222, help="run on the given port", type=int)

def main():
  tornado.options.parse_command_line()
  ws = tornado.httpserver.HTTPServer(Square())
  ws.listen(options.port)
  tornado.ioloop.IOLoop.instance().start()

if __name__ == "__main__":
  main()
