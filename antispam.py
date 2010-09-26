#!/usr/bin/python
#coding:utf-8

import os
import StringIO

import tornado.web
import tornado.options
import tornado.ioloop
import tornado.httpserver

import Captcha
import Captcha.Visual.Tests

import base

_DefaultSize = (240, 48)
# _cf = Captcha.PersistentFactory('captcha.db')
_cf = Captcha.Factory()


def NewCaptcha():
  return _cf.new(Captcha.Visual.Tests.PseudoGimpy)
  
def Check(id, check):
  return _cf.test(id, [check])

# /captcha/([^/]+) 括号部分为 id
class CaptchaHandler(base.BaseHandler):
  def get(self, id):
    c = _cf.get(id)
    if not c.valid:
      raise tornado.web.HTTPError(404)

    self.set_header("Content-Type", "image/jpeg")
    # http://htaccess.wordpress.com/2009/09/22/x-content-type-options-nosniff-header/
    self.set_header("X-Content-Type-Options", "nosniff")
    
    # TODO: Expires 立即过期

    img = StringIO.StringIO()
    c.render(_DefaultSize).save(img, "JPEG")

    self.write(img.getvalue())

if __name__ == "__main__":
  # for i in range(1,10): Captcha.Visual.Tests.PseudoGimpy().render((240,48)).save('%d.jpg' % i)
  
  # cf = Captcha.PersistentFactory('captcha.db')
  # cf = Captcha.Factory()
  # c = cf.new(Captcha.Visual.Tests.PseudoGimpy)
  # print c.__class__.__name__, c.id
  print NewCaptcha().id
