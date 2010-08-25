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

    fid = self.db.feed.save(dict(
        time= datetime.datetime.now(),
        owner= user.id,
        name= user.name,
        body= text,
        title= '',
        comments= [],
        where= user.center,
      ))
    PostPublish(self.db, user, fid, user.center)

    self.redirect(self.get_argument("next", "/"))

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

@base.async
def PostPublish(db, who, fid, where):
  # db.user.find $near where
  # db.user.update owner, $push fid
  
  assert isinstance(where, list)
  
  # 给自己发新鲜事挺好的
  """db.feedlist.update({'owner': who.id},
      {'$push': {'fid': fid}},
      upsert = True
    )"""

  # 更新感兴趣的人的 feedlist
  # TODO: 确定 maxDistance 的精确度
  where.append(5)
  users = db.user.find({'c' : {'$near' : where}}, {'_id':1, 'c':1, 'r':1})
  uids = [u['_id'] for u in users]
  # print 'publish to', uids
  db.feedlist.update({'owner' : {'$in' : uids}},
      {'$push': {'fid': fid}},
      upsert = True, multi = True
    )



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
  

if __name__ == "__main__":
  pass
  # TODO: how  to test a Handler
