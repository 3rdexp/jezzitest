#!/usr/bin/python
#coding:utf-8

import datetime
import simplejson
import logging

import tornado.web
import tornado.escape

import pymongo

import base
import antispam
import unihtml

# for ajax response json
# /j/publish?content=
# /j/action?fid=&action=&c=&p=
class JsonHandler(base.BaseHandler):
  def post(self, cate):
    logging.debug('JsonHandler cate:%s' % cate)
    user = self.current_user

    if 'publish' == cate:
      content = self.get_argument("content", None)
      if not content:
        raise tornado.web.HTTPError(409) # Conflict
      
      p = unihtml.UniParser()
      p.Parse(content)
      logging.debug('JsonHandler publish content:%s parsed:%s' %(content, p.html))
    
      focus = base.PlainDict(user.GetFocus(self.db)[0])
    
      fid = Feed.New(self.db, user, p.html, where=focus.center, radius=focus.radius)
      d = self.db.feed.find_one(dict(_id=fid))
      feed = base.PlainDict(d)

      fm = FeedModule(self)
      html = fm.render(feed)
      self.write(simplejson.dumps(dict(error=0, payload=html)))
    elif 'action' == cate:
      # action: comment, like
      # p for parent, c for content
      fid = self.get_argument("fid", None)
      action = self.get_argument("action", None)
      body = self.get_argument("c", None)
      p = self.get_argument("p", None)
      logging.debug('action: %s, fid: %s' % (action, fid))
      
      # TODO: 强检测参数
      if -1 == p: p = None
      fid = pymongo.objectid.ObjectId(fid)
      
      if 'comment' == action:
        Feed.Comment(self.db, fid, user, body, index=p)
      
      d = self.db.feed.find_one(dict(_id=fid))
      c = d['comments'].pop()
      
      # TODO: 如何找到刚更新的 comment 呢？
      fc = FeedCommentModule(self)
      html = fc.render(base.PlainDict(c))
      self.write(simplejson.dumps(dict(error=0, fid=str(fid), payload=html)))

class PublishHandler(base.BaseHandler):
  # @tornado.web.authenticated
  def post(self):
    user = self.current_user
    if not user:
      raise tornado.web.HTTPError(403)

    text = self.get_argument('content', None)
    
    # 1 html parse
    # 2 save to db
    
    # 1 
    p = unihtml.UniParser()
    p.Parse(text)
    text = p.html
    logging.debug('PublishHandler parsed:%s' % text)
    
    # 2
    Feed.New(self.db, user, text, user.center)
    
    #~ self.redirect(self.get_argument("next", "/"))
    ds = Feed.Read(self.db, user)
    feeds = []
    for d in ds:
      feeds.append(base.PlainDict(d))
    self.render('home.html', feeds=feeds, user=user)

class UploadHandler(base.BaseHandler):
  # @tornado.web.addslash
  def get(self):
    raise tornado.web.HTTPError(404)

  def post(self):
    pass


class FeedModule(tornado.web.UIModule):
  def render(self, feed):
    return self.render_string('module_feed.html', feed=feed
      , FormatTime=FormatTime)

class FeedCommentModule(tornado.web.UIModule):
  def render(self, comment):
    return self.render_string('module_feedcomment.html', comment=comment
      , FormatTime=FormatTime)


class FeedHandler(base.BaseHandler):
  # TODO: use @tornado.web.authenticated
  def post(self):
    user = self.current_user
    if not user:
      raise tornado.web.HTTPError(403)

    comment_text = self.get_argument('c', None)
    fid = self.get_argument('fid', None)
    p = self.get_argument('p', None) # parent comment

    # 1 html parse
    # 2 save to db
    # 3 publish it again

    fid = Feed.Comment(self.db, pymongo.objectid.ObjectId(fid)
        , user, comment_text, index=p)

    self.redirect(self.get_argument("next", "/"))



def FormatTime(when):
  assert isinstance(when, datetime.datetime)
  diff = datetime.datetime.now() - when

  if diff.days:
    return "%d天前" % diff.days

  diff_minutes = diff.seconds / 60
  if diff_minutes <= 0:
    return "刚刚更新"
  elif diff_minutes < 60:
    return "%d分钟前" % diff_minutes
  elif diff_minutes < 24 * 60:
    return "%d小时前" % (diff_minutes / 60)

  return when.strftime(u"%m-%d %H:%M")
  

class Feed(object):
  def __init__(self, d = None):
    if not d:
      self.__dict__.update(d)
  
  def ShowComment(self):
    # 时间较近，数目不是太多的，显示全部Comments，还有输入框
    # 当天，< 100
    return True

  @staticmethod
  def New(db, user, text, title='', where=None,  radius=None):
    center = where or user.center
    now = datetime.datetime.now()
    fid = db.feed.insert(dict(
        time= now,
        last_modify=now,
        owner= user.id,
        name= user.nick,
        head = user.head,
        body= text,
        title= title, # two title is correct
        comments= [],
        where= center,
      ),  safe=True)

    # publish
    # 更新感兴趣的人的 feedlist
    # TODO: 确定 maxDistance=1000 的精确度
    cond = [center,  radius or 1000]
    logging.debug('condition: %r', cond)

    c = db.focus.find({'center' : {'$within' : {'$center':cond}}}, {'uid':1, 'center':1, 'radius':1})
    if not c.alive:
      logging.debug('nobody got the feed? condition: %r', cond)
      return
    
    uids = [u['uid'] for u in c]
    logging.debug('Feed.New publish feed fid: %s to users:%s' % (str(fid), str(uids)))

    ret = db.feedlist.update({'_id' : {'$in' : uids}},
          {'$push': {'fid': fid}},
          safe=True, upsert=False, multi=True
        )
        

    # 如果更新失败or部分失败，则低效的挨个更新
    if not ret['updatedExisting'] or ret['n'] != len(uids):
      for uid in uids:
        try:
          r = db.feedlist.insert({'_id': uid, 'fid':[]}, safe=True)
        except pymongo.errors.DuplicateKeyError:
          continue
        logging.warning('feed slow publish, fid:%s to user:%s' % (fid,  uid))
        ret = db.feedlist.update({'_id' : uid},
            {'$push': {'fid': fid}},
            safe=True, upsert=False, multi=False
          )
    return fid

  # index: index of parent comment
  @staticmethod
  def Comment(db, fid, owner, body, index=None):
    d = {'$push': 
        {'comments':{'owner': owner.id, 'name': owner.nick, 'body': body}}
      }
    # key 不存在 和 =None 是有些不同的，使用不存在来表示第一级
    if index is not None:
      d['$push']['comments']['p'] = index
    # print 'push', index, d
    db.feed.update({'_id': fid}, d, safe=True)
    # 貌似一次不能同时执行 $push 和 $set
    db.feed.update({'_id': fid}, {'$set': {'last_modify': datetime.datetime.now()}})

  @staticmethod
  def Read(db, owner, skip=0, limit=40):
    d = db.feedlist.find_one(dict(_id=owner.id))
    if not d:
      return None

    fids = d['fid']
    # 按时间倒序
    return db.feed.find({'_id' : {'$in': fids}}).limit(limit).sort(u'time', pymongo.DESCENDING)

  @staticmethod
  def Around(db, center, radius):
    pass

import unittest
class FeedTestCase(unittest.TestCase):
  def setUp(self):
    self.db = pymongo.Connection('localhost', 27017).square
    
    # base.User.Remove(self.db, email='testa')
    # base.User.Remove(self.db, email='testb')
    # base.User.Remove(self.db, email='testc')
    # base.User.Remove(self.db, email='testd')
    
    # a --1000-- b
    # 
    # c
    
    factor = 60000
    
    self.usera = self.TryUser('testa',  (126*factor, 50*factor),  3000)
    self.userb = self.TryUser('testb',  (126*factor, 50*factor+1000),  1000) # near a
    self.userc = self.TryUser('testc',  (126*factor - 3000, 50*factor),  1000) # faraway a
    
    self.userd = self.TryUser('testd',  (126*factor, 50*factor+1200),  1000) # near a
    # self.db.feedlist.insert({'_id':self.userd.id,  fid:[]})
    
    self.feeds_ = []

  def _tearDown(self):
    # remove the users, feeds
    base.User.Remove(self.db, id=self.usera.id, with_data=True)
    base.User.Remove(self.db, id=self.userb.id, with_data=True)
    base.User.Remove(self.db, id=self.userc.id, with_data=True)
    base.User.Remove(self.db, id=self.userd.id, with_data=True)

    for fid in self.feeds_:
      self.db.feed.remove({'_id':fid})
      
  def CheckPublish(self, u, center, radius, got=[], notgot=[]):
    fid = Feed.New(self.db,  u,  'demo feed',  where=center, radius=radius)
    self.assertTrue(fid != None)
    if fid:
      self.feeds_.append(fid)
      
    for i in got:
      fs = Feed.Read(self.db, i)
      self.assertNotEqual(None,  fs)
      ids = [f['_id'] for f in fs]
      self.assertTrue(len(ids) > 0)
      self.assertTrue(fid in ids)
      
    for i in notgot:
      fs = Feed.Read(self.db, i)
      if fs is not None:
        ids = [f['_id'] for f in fs]
        self.assertTrue(fid not in ids)

  def _testPublish(self):
    logging.debug('usera focus: %s', self.usera.focus)
    focus = base.PlainDict(self.usera.focus[0])
    logging.debug('first focus: %s, %s', focus.center,  focus.radius)
    self.CheckPublish(self.usera, focus.center, focus.radius, got=[self.usera, self.userb, self.userc, self.userd])
    
    self.CheckPublish(self.usera, focus.center, 1000, got=[self.usera, self.userb], notgot=[self.userc, self.userd])
    
    self.CheckPublish(self.usera, focus.center, 1200, got=[self.usera, self.userb, self.userd], notgot=[self.userc])

  def NewUser(self,  email,  focus,  radius):
    u = base.User.New(self.db, email=email, passwd='p', nick=email, head='/s/am.png')
    u.AddFocus(self.db, focus, radius)
    u.GetFocus(self.db)
    return u
    
  def TryUser(self,  email,  focus,  radius):
    try:
      u = self.NewUser(email,  focus,  radius)
    except:
      u = base.User.CheckLogin(self.db, email, 'p')
      u.GetFocus(self.db)
    return u

  def testNew(self):
    # new feed
    fid = Feed.New(self.db, self.usera, 'fake body', where=[100, 200], radius=100)
    print 'check comment fid:', fid

    fd = self.db.feed.find_one(dict(_id=fid))
    logging.debug('after create, last_modify: %s', fd['last_modify'])

    # add a group of comments
    Feed.Comment(self.db, fid, self.usera, 'first comment')
    Feed.Comment(self.db, fid, self.usera, 'comment to 0', 0)
    Feed.Comment(self.db, fid, self.usera, '2nd comment')
    Feed.Comment(self.db, fid, self.usera, 'comment to 2', 2)
    Feed.Comment(self.db, fid, self.usera, 'comment to 2 again', 2)
    Feed.Comment(self.db, fid, self.usera, '3rd comment')
    Feed.Comment(self.db, fid, self.usera, '4th comment')

    fd = self.db.feed.find_one(dict(_id=fid))
    logging.debug('after 6 comment, last_modify: %s', fd['last_modify'])

    allfeed = Feed.Read(self.db, self.usera, limit=1000)

    for c in fd['comments']:
      #print c
      if 'p' in c:
        print c['p'], 
      print c['body']

if __name__ == "__main__":
  logging.getLogger('').setLevel(logging.INFO)
  unittest.main()
