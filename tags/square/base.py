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
    self.name_table_ = name_table
    
    if not isinstance(d,  dict):
      return

    # self.__dict__.update(d)
    for a, b in d.items():
      if isinstance(b, (list, tuple)):
        setattr(self, a, [PlainDict(x) if isinstance(x, dict) else x for x in b])
      else:
        setattr(self, a, PlainDict(b) if isinstance(b, dict) else b)

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
  >>> d={'email':'ken', 'passwd':'k', 'nick':'old dog', 'head':'/s/am.png'}
  >>> u = User(d)
  >>> u.email
  'ken'
  
  >>> db = pymongo.Connection('localhost', 27017).square
  >>> db.user.remove({'email':'ken'})
  >>> v = User.New(db, **d)
  >>> v.email
  u'ken'
  >>> type(v.id)
  <class 'pymongo.objectid.ObjectId'>
  >>> len(str(v.id))
  24
  
  >>> f0 = v.GetFocus(db)
  >>> a0 = len(f0)
  >>>
  >>> v.AddFocus(db, [1,2], 3)
  >>> f1 = v.GetFocus(db)
  >>> a1 = len(f1)
  >>> a1 == a0 + 1
  True
  
  >>> User.CheckAvailable(db, email='ken')
  False
  >>> User.CheckAvailable(db, nick='old dog')
  False
  >>> User.CheckAvailable(db, email='ken3')
  True
  
  >>> u = User.CheckLogin(db, email='ken', passwd='k')
  >>> len(str(u.id))
  24
  
  ### return None 居然在 doctest 中是 got nothing
  >>> User.CheckLogin(db, email='ken', passwd='k2')
  >>> User.CheckLogin(db, email='ken1000', passwd='k')
  """
  def __init__(self, d):
    PlainDict.__init__(self, d)

  def AddFocus(self, db, center, radius):
    db.focus.save({'uid':self.id, 'center':center, 'radius':radius})
    # update self.focus? No
    
  def GetFocus(self, db):
    d = db.focus.find({'uid':self.id})
    self.focus = d is None and [] or [i for i in d]
    return self.focus

  @staticmethod
  def New(db, **kwargs):
    """
    email, passwd, nick
    """
    try:
      email = kwargs['email']
      passwd = kwargs['passwd']
      nick = kwargs['nick']
      head = kwargs['head']
    except KeyError:
      logging.error('User.New parameter error, with %r',  kwargs)

    uid = db.user.save({'email':email, 
                         'passwd':passwd,
                         'nick':nick,
                         'email_verified':False,
                         'head':head},  safe=True)
    u = db.user.find_one({'_id':uid})
    return User(u)

  @staticmethod
  def Remove(db, id=None, email=None, with_data=True):
    if id is not None:
      q = {'_id':id}
    elif email is not None:
      q = {'email':email}
    else:
      return False
      
    logging.info("user Remove by:%r",  q)

    db.user.remove(q, safe=True)

    if with_data and id is not None:
      db.feedlist.remove({'_id':id})
      db.feed.remove({'owner':id})

    return True
    
  @staticmethod
  def CheckLogin(db, email, passwd):
    d = db.user.find_one({'email': email, 'passwd': passwd})
    if d:
      return User(d)
    return None
    
  @staticmethod
  def CheckAvailable(db, nick=None,  email=None):
    if nick is not None:
      q = {'nick':nick}
    elif email is not None:
      q = {'email':email}
    d = db.user.find_one(q)
    return d is None

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
      d = self.db.user.find_one({'_id': pymongo.objectid.ObjectId(uid)})
      if d:
        self.user_ = User(d)
    return self.user_

if __name__ == "__main__":
  import doctest
  doctest.testmod()
