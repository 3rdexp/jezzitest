#!/usr/bin/python
#coding:utf-8

import os
import StringIO
import datetime

import tornado.web
import pymongo

from tornado.options import define, options

import base
import upload
import ugc
import antispam

class UserModule(tornado.web.UIModule):
  def render(self, user):
    return self.render_string('module_user.html', user=user)

class AuthLogoutHandler(base.BaseHandler):
  # @tornado.web.addslash
  def get(self):
    self.clear_cookie('u')
    self.redirect(self.get_argument("next", "/"))

class AuthLoginHandler(base.BaseHandler):
  # @tornado.web.addslash
  def get(self):
    # TODO: 用户已经登录呢？ 提醒 + 给用户切换用户的机会
    self.render('login.html', captcha=None)
    
  def post(self):
    name = self.get_argument("name", None)
    # email = self.get_argument("email", None)
    passwd = self.get_argument("passwd", None)
    
    if name and passwd:
      d = self.db.user.find_one({'n': name, 'p': passwd})
      if d:
        # TODO: setting['expires_days']
        self.set_secure_cookie('u', str(d['_id']), expires_days=30)
        self.redirect(self.get_argument("next", "/"))
      return

    # TODO: 在 Application 中记录该用户的所有请求都必须要进行验证
    c = antispam.NewCaptcha()
    self.render('login.html', captcha=c)


class NameCheckHandler(base.BaseHandler):
  def get(self,  name):
    d = self.db.user.find_one({'n':name})
    if d:
      self.write('{"":""}')

class SignHandler(base.BaseHandler):
  @tornado.web.addslash
  def get(self):
    self._render_get()

  def post(self):
    if self.settings.get('sign_captcha'):
      id = self.get_argument("captcha", None)
      check = self.get_argument("check", None)
      
      if not id or not check or not antispam.Check(id, check):
        self._render_get()
        return
      
    goto = self.get_argument("goto", '/')
    
    name = self.get_argument("name", None)
    email = self.get_argument("email", None)
    passwd = self.get_argument("passwd", None)
    
    # 扩大 1000 倍，提高mongodb里比较的精度
    center = [float(self.get_argument("lat", None)) * 1000
      , float(self.get_argument("lng", None)) * 1000]
    radius = 3000 # TODO: 合适的单位是什么?
    
    # 1 判断各种输入合法性
    # 2 检查名字, email 是否重复
    # 3 生成头像，保存
    # 4 写入数据库, 返回
    
    # head to /s/h/%U/%f.jpg # /s/h/35/185312.jpg
    # %W Week number of the year
    # %f Microsecond as a decimal number [0,999999], zero-padded on the left
    
    # 3
    head_filepath,  head_url = upload.GenFileName()
    if not GenHead(head_filepath, self.request.files['head'][0]):
      self.write('TODO')
      return

    # TODO: 使用索引检查重复
    # TODO: email 不是必须的，对于中国用户来说
    # new user
    uid=self.db.user.save({
        'n':name
        , 'l':[1,2], 'h':head_url
        , 'e': email
        , 'p' : passwd     # TODO: hash it
        # TODO: save ticket
        #, 't':{'k':'alsdfjlsejrwae', 'e':'2010/10/10 12:23:33'}
        #, 'f' : {'c' : center, 'r':radius}
        , 'c' : center, 'r' : radius
      }, safe=True)
    print 'new uid:', uid
    
    if uid:
      # TODO: User.xxx
      self.set_secure_cookie('u', str(uid))
      self.redirect(goto)
      return
    
    self._render_get()

  def _render_get(self):
    c = None
    if self.settings.get('sign_captcha'):
      c = antispam.NewCaptcha()
    self.render('sign.html', captcha=c)

# TODO: move in upload.py
def GenHead(filepath, file):
  dir = os.path.dirname(filepath)
  if not os.path.exists(dir):
    os.makedirs(dir)

  f = open(filepath, 'wb')
  f.write(file['body'])
  f.close()
  return True

if __name__ == "__main__":    
  pass
  # TODO: how  to test a Handler
