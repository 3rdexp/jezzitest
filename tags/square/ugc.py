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

# TODO: rename to feed.py

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
    
      fid = Feed.New(self.db, user, p.html, user.center)
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
      html = fc.render(c)
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
  def New(db, user, text, title='', where=None):
    now = datetime.datetime.now()
    fid = db.feed.save(dict(
        time= now,
        last_modify=now,
        owner= user.id,
        name= user.name,
        head = user.head,
        body= text,
        title= title, # two title is correct
        comments= [],
        where= user.center,
      ))
      
    # publish
    # 更新感兴趣的人的 feedlist
    # TODO: 确定 maxDistance=5 的精确度
    if not isinstance(where, list):
      return fid      
    where.append(5)
    users = db.user.find({'c' : {'$near' : where}}, {'_id':1, 'c':1, 'r':1})

    uids = [u['_id'] for u in users]
    logging.debug('Feed.New publish feed fid: %s to users:' % (fid, uids))

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
        logging.warning('publish feed list slow, fid:%s to user:%s' % (fid,  uid))
        ret = db.feedlist.update({'_id' : uid},
            {'$push': {'fid': fid}},
            safe=True, upsert=False, multi=False
          )
    return fid

  # index: index of parent comment
  @staticmethod
  def Comment(db, fid, owner, body, index=None):
    d = {'$push': 
        {'comments':{'owner': owner.id, 'name': owner.name, 'body': body}}
      }
    # key 不存在 和 =None 是有些不同的，使用不存在来表示第一级
    if index:
      d['$push']['p'] = index
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
    self.user = base.User(self.db.user.find_one())
    self.text = 'test text'
    self.remove_feed = []
    # new user A B C

  def tearDown(self):
    # remove the users, feedlist
    pass
    #for fid in self.remove_feed:
    #  self.db.feed.remove({'_id':fid})

  def testNew(self):
    # new feed
    fid = Feed.New(self.db, self.user, self.text, self.user.center)

    fd = self.db.feed.find_one(dict(_id=fid))
    print 'after create, last_modify:', fd['last_modify']

    # add a group of comments
    Feed.Comment(self.db, fid, self.user, 'first comment')
    Feed.Comment(self.db, fid, self.user, 'comment to 0', 0)
    Feed.Comment(self.db, fid, self.user, '2nd comment')
    Feed.Comment(self.db, fid, self.user, 'comment to 2', 2)
    Feed.Comment(self.db, fid, self.user, 'comment to 2 again', 2)
    Feed.Comment(self.db, fid, self.user, '3rd comment')
    Feed.Comment(self.db, fid, self.user, '4th comment')

    fd = self.db.feed.find_one(dict(_id=fid))
    print 'after 6 comment, last_modify:', fd['last_modify']

    allfeed = Feed.Read(self.db, self.user)

    ret = []
    for a in allfeed:
      if not a['p']:
        pass

if __name__ == "__main__":
  unittest.main()
