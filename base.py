#!/usr/bin/python
#coding:utf-8

import tornado.web
import pymongo
import logging

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

class PlainDict(object):
  """ 书写方便性，如： d['a']，写成 d.a
  构造函数中的 name_table 作为对简写的描述(特别是mongodb中对字段的描述)
  from:
  http://stackoverflow.com/questions/1305532/convert-python-dict-to-object
  >>> d = {'a': 1, 'b': {'c': 2}, 'd': ["hi", {'foo': "bar"}]}
  >>> x = PlainDict(d)
  >>> x.a
  1
  >>> x.b.c
  2
  >>> x.d[1].foo
  'bar'
  >>> x = PlainDict(d, {'afoo': 'a'})
  >>> x.afoo
  1
  >>> x.a
  1
  """
  def __init__(self, d, name_table={}):
    assert(isinstance(d,  dict))
    # self.__dict__.update(d)
    for a, b in d.items():
      if isinstance(b, (list, tuple)):
        setattr(self, a, [PlainDict(x) if isinstance(x, dict) else x for x in b])
      else:
        setattr(self, a, PlainDict(b) if isinstance(b, dict) else b)

    self.name_table_ = name_table
    
    # 针对mongodb特殊处理，省去 _id/id 不分之苦
    if '_id' in d and 'id' not in d:
      self.name_table_['id'] = '_id'
    
  def __getattr__(self, name):
    if name in self.name_table_:
      return self.__dict__[ self.name_table_[name] ]
    return object.__getattr__(self, name)

  def __str__(self):
    return str(self.__dict__)

class User(PlainDict):
  """
  >>> d={'name':'ken', 'passwd':'k', 'nick':'old dog', 'head':'/s/am.png'}
  >>> u = User(d)
  >>> u.name
  'ken'
  >>> db = pymongo.Connection('localhost', 27017).square
  >>> db.user.remove({'name':'ken'})
  >>> v = User.New(db, **d)
  >>> v.name
  u'ken'
  >>> type(v.id)
  <class 'pymongo.objectid.ObjectId'>
  >>> len(str(v.id))
  24
  >>> v.AddFocus(db, [1,2], 3)
  >>> focus = v.GetFocus(db)
  >>> type(focus)
  'dict'
  
  """
  def __init__(self, d):
    PlainDict.__init__(self, d)

  def AddFocus(self, db, center, radius):
    db.focus.save({'uid':self.id, 'center':center, 'radius':radius})
    # update self.focus? No
    
  def GetFocus(self, db):
    return db.focus.find({'uid':self.id})

  @staticmethod
  def New(db, **kwargs):
    """
    name, passwd, nick
    """
    try:
      name = kwargs['name']
      passwd = kwargs['passwd']
      nick = kwargs['nick']
      head = kwargs['head']
    except KeyError:
      logging.error('User.New parameter error, with %r',  kwargs)
      
    email = 'email' in kwargs and kwargs['email'] or ''
    
    uid = db.user.save({'name':name, 
                         'passwd':passwd, 
                         'nick':nick, 
                         'email':email, 
                         'email_verified':False, 
                         'head':head},  safe=True)
    u = db.user.find_one({'_id':uid})
    return User(u)
  
  @staticmethod
  def CheckName(self, db, name):
    d = db.user.find_one({'name':name})
    return d is None

  @staticmethod
  def Remove(self, db, id=None, name=None, with_data=True):
    if id is not None:
      q = {'_id':id}
    elif name is not None:
      q = {'name':name}
    else:
      return False

    db.user.remove(q, safe=True)

    if with_data and id is not None:
      db.feedlist.remove({'_id':id})
      db.feed.remove({'owner':id})

    return True

class BaseHandler(tornado.web.RequestHandler):
  def initialize(self):
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
    if uid and len(uid) == len('4c6fc6bc3758980f08000000'):
      # TODO: class User, now it's a dict
      # {u'h': u'', u'_id': ObjectId('4c6fc6bc3758980f08000000'), u'l': [1, 2], u'n': u'ken', u'f': [{u'c': [1, 2], u'r': 3}]}
      d = self.db.user.find_one({'_id': pymongo.objectid.ObjectId(uid)})
      # print 'find_one result:', d, 'uid:', uid
      if d:
        self.user_ = User(d)
    return self.user_

if __name__ == "__main__":
  import doctest
  doctest.testmod()
