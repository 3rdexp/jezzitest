#!/usr/bin/python
#coding:utf-8

import tornado.web
import pymongo

class BaseHandler(tornado.web.RequestHandler):
  def __init__(self,application, request, transforms=None):
    tornado.web.RequestHandler.__init__(self,application, request, transforms)

    self.user_ = None
    self.user_checked_ = False # 是否从数据库中读取记录，验证过 cookie 是否合法

  @property
  def db(self):
    return self.application.db

  @property
  def current_user(self):
    if self.user_checked_:
      return self.user_

    self.user_checked_ = True
    uid = self.get_secure_cookie('u')
    if uid:
      # TODO: class User, now it's a dict
      # {u'h': u'', u'_id': ObjectId('4c6fc6bc3758980f08000000'), u'l': [1, 2], u'n': u'ken', u'f': [{u'c': [1, 2], u'r': 3}]}
      d = self.db.user.find_one({'_id': pymongo.objectid.ObjectId(uid)})
      # print 'find_one result:', d, 'uid:', uid
      if d:      
        self.user_ = User(d)
    return self.user_


import threading

class async(object):
  """Decorators for 异步执行
  http://www.artima.com/weblogs/viewpost.jsp?thread=240845
  http://www.ibm.com/developerworks/cn/linux/l-cpdecor.html
  """
  def __init__(self, method):
    self.m_ = method

  def __call__(self, *args, **kwargs):
    t = threading.Thread(target=self.m_, args=args, kwargs=kwargs)
    t.start()


class User(object):
  """改变dict访问方式的不便;mongodb缩写带来的理解问题，可以直接写 User.name，而不用写 User['n']
  >>> u = User(dict(c=1, n='foo'))
  >>> u.name
  'foo'
  >>> u.n
  'foo'
  """
  def __init__(self, d):
    self.__dict__.update(d)
    self.imp_dict = dict(
        center = 'c',
        name = 'n',
        id = '_id',
        email = 'e',
        passwd = 'p',
        head = 'h',
      )

  def __getattr__(self, name):
    if name in self.imp_dict:
      return self.__dict__[ self.imp_dict[name] ]
    return object.__getattr__(self, name)

# TODO: 对dict中的值也做类似处理
class PlainDict(object):
  def __init__(self, d):
    assert(isinstance(d,  dict))
    self.__dict__.update(d)

  def __str__(self):
    return str(self.__dict__)

if __name__ == "__main__":
  import doctest
  doctest.testmod()
