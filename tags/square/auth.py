#!/usr/bin/python
#coding:utf-8

import os
import StringIO
import datetime

import tornado.web
import pymongo

import base
import upload
import feed
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
    email = self.get_argument("email", None)
    passwd = self.get_argument("passwd", None)
    
    if email and passwd:
      u = base.User.CheckLogin(self.db, email,  passwd)
      if u is not None:
        # TODO: setting['expires_days']
        self.set_secure_cookie('u', str(u.id), expires_days=30)
        self.redirect(self.get_argument("next", "/"))
      return

    # TODO: 在 Application 中记录该用户的所有请求都必须要进行验证
    c = antispam.NewCaptcha()
    self.render('login.html', captcha=c)


class NameCheckHandler(base.BaseHandler):
  def get(self,  nick):
    d = self.db.user.find_one({'nick':nick})
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
    
    email = self.get_argument("email", None)
    nick = self.get_argument("nick", None)
    passwd = self.get_argument("passwd", None)
    
    # http://en.wikipedia.org/wiki/Latitude
    # 地球半径为 6356752.3142 m
    # Latitude	        Town	              Degree	    Minute	    Second	±0.0001°
    # 60°	            Saint Petersburg	55.65 km	0.927 km	15.42m	5.56m
    # 51° 28' 38" N	Greenwich	        69.29 km	1.155 km	19.24m	6.93m
    # 45°	            Bordeaux	          78.7 km	  1.31 km	  21.86m	7.87m
    # 30°	            New Orleans	      96.39 km	1.61 km	  26.77m	9.63m
    # 0°	              Quito	                111.3 km	1.855 km	30.92m	11.13m
    
    # Latitude	| N-S radius of curvature | Surface distance per 1° change in latitude
    #         | E-W radius of curvature | Surface distance per 1° change  in longitude
    # 0°	  6335.44 km	110.574 km		6378.14 km	111.320 km
    # 15°	6339.70 km	110.649 km		6379.57 km	107.551 km
    # 30°	6351.38 km	110.852 km		6383.48 km	96.486 km
    # 45°	6367.38 km	111.132 km		6388.84 km	78.847 km
    # 60°	6383.45 km	111.412 km		6394.21 km	55.800 km
    # 75°	6395.26 km	111.618 km		6398.15 km	28.902 km
    # 90°	6399.59 km	111.694 km		6399.59 km	0.000 km
    
    # 对于中国，经度1度大致为 111 km，纬度1度大致为 78 km
    
    # 经纬度要乘以 60 * 1000，然后直接运算相当于单位为 m
    # 
    
    # google map
    # (建議放大級數: 大城市15, 小型城市或鄉鎮12 , 郊區8, 大陸或海洋6)
    
    center = [float(self.get_argument("lat", None)) * 60000
      , float(self.get_argument("lng", None)) * 60000]
    radius = 3000 # 单位是 m
    
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
    u=base.User.New(email=email,  head=head_url, passwd=passwd)
    if u is not None:
      u.AddFocus(self.db, center, radius)
      
      self.set_secure_cookie('u', str(u.id))
      self.redirect(goto)
      return
    
    self._render_get()

  def _render_get(self):
    c = None
    if self.settings.get('sign_captcha'):
      c = antispam.NewCaptcha()
    self.render('sign.html', captcha=c)
    
    
class SettingHandler(base.BaseHandler):
  @tornado.web.removeslash
  def get(self):
    self.current_user.GetFocus(self.db)
    self.render('setting.html', user=self.current_user)
    
  def post(self):
    pass

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
