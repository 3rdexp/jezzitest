#!/usr/bin/python
#coding:utf-8

import datetime

import tornado.web

import pymongo

import base
import antispam

class PublishHandler(base.BaseHandler):
  # @tornado.web.addslash
  def get(self):
    raise tornado.web.HTTPError(405)

  # @tornado.web.authenticated
  def post(self):
    user = self.current_user
    if not user:
      raise tornado.web.HTTPError(403)

    text = self.get_argument('content', None)
    
    # 1 html parse
    # 2 save to db
    # 3 publish it
    
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

@base.async
def CreateFeedList(db, uid):
  """对于 $push，如果数据不存在是不会成功的，所以建立一条空数据
  db.feedlist.save({owner:ObjectId('4c7343ec58fb5e4024000001'), fid:[]})
  """
  print 'CreateFeedList', uid
  db.feedlist.save({'owner' : uid, 'fid' : []})


class FeedModule(tornado.web.UIModule):
  def render(self, feed):
    return self.render_string('module_feed.html', feed=feed, FormatTime=FormatTime)



class FeedHandler(base.BaseHandler):
  # TODO: use @tornado.web.authenticated
  def post(self):
    user = self.current_user
    if not user:
      raise tornado.web.HTTPError(403)

    comment_text = self.get_argument('c', None)
    fid = self.get_argument('fid', None)
    # TODO: name, head?
    
    # 1 html parse
    # 2 save to db
    # 3 publish it again

    print 'comment fid', fid
    fid = self.db.feed.update({'_id' : pymongo.objectid.ObjectId(fid)}
        , {'$push':{'comments':{'owner':user.id, 'name': user.name, 'body':comment_text}}}
      )

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

  @staticmethod
  def New(db, user, text, where=None):
    now = datetime.datetime.now()
    fid = db.feed.save(dict(
        time= now,
        last_modify=now,
        owner= user.id,
        name= user.name,
        head = user.head,
        body= text,
        title= '',
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
    print 'publish', fid, 'to', uids

    ret = db.feedlist.update({'_id' : {'$in' : uids}},
          {'$push': {'fid': fid}},
          safe =True, upsert = False, multi = True
        )
        
    # print 'update ret:', ret
    # 如果更新失败or部分失败，则低效的挨个更新
    if not ret['updatedExisting'] or ret['n'] != len(uids):
      for uid in uids:
        try:
          r = db.feedlist.insert({'_id': uid, 'fid':[]}, safe=True)
        except pymongo.errors.DuplicateKeyError:
          continue
        ret = db.feedlist.update({'_id' : uid},
            {'$push': {'fid': fid}},
            safe=True, upsert=False, multi=False
          )
        # print 'update 2nd ret:', ret
    return fid

  # index: index of parent comment
  @staticmethod
  def Comment(db, fid, owner, body, index=None):
    # 貌似一次不能同时执行 $push 和 $set
    db.feed.update({'_id': fid}
        , {'$push': {'comments':{'owner': owner.id, 'name': owner.name, 'body': body, 'p' : index}}
#            , '$set': {'last_modify': datetime.datetime.now()}
          }
      )
    db.feed.update({'_id': fid}, {'$set': {'last_modify': datetime.datetime.now()}})

  @staticmethod
  def Read(db, owner, skip=0, limit=40):
    # print 'read:', owner.id
    d = db.feedlist.find_one(dict(_id=owner.id))
    if not d:
      return None

    fids = d['fid']
    # 按时间倒序
    # p=None
    return db.feed.find({'_id' : {'$in': fids}}).limit(limit).sort(u'time', pymongo.DESCENDING)


import unittest
class FeedTestCase(unittest.TestCase):
  def setUp(self):
    self.db = pymongo.Connection('localhost', 27017).square
    self.user = base.User(self.db.user.find_one())
    self.text = 'test text'
    self.remove_feed = []

  def tearDown(self):
    pass
    #for fid in self.remove_feed:
    #  self.db.feed.remove({'_id':fid})

  def testNew(self):
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
    # for af in allfeed: print af
    
    ret = []
    for a in allfeed:
      if not a['p']:
        

if __name__ == "__main__":
  import unittest
  unittest.main()
  
  
