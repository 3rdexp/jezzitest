#!/usr/bin/python
#coding:utf-8

import os
import tornado.web

def GenFileName(prefix = 'h',  ext = 'jpg'):
  """按日期生成文件名，早期可以用星期，后期量大时改为按天
  path: .../static/prefix/Week/Microsecond.ext
  url: /s/prefix/Week/Microsecond.ext
  TODO: Week 能按年增长
  """
  part = '%s/%s.%s' % (prefix, (datetime.datetime.now().strftime('%W/%f')),  ext)
  url = "/s/%s" % part
  # 前面加 / 会导致 os.path.join 错误
  # 实际路径为 .../static/h/...
  filepath = os.path.join(os.path.dirname(__file__), "static", part)
  return (filepath,  url)

# TODO: 保存在独立的文件 upload.py 中
# TODO: 图像处理
# TODO: 不引入 web.RequestHandler
# save files => urls
def Upload(request,  field='file'):
  # TODO: assert(request, tornado.web.RequestHandler);
  urls = []
  for file in request.files[field]:
    filepath,  url = GenFileName()
    
    dir = os.path.dirname(filepath)
    if not os.path.exists(dir):
      os.makedirs(dir)
    f = open(filepath, 'wb')
    f.write(file['body'])
    f.close()
    
    urls.push(url)
  return urls

def Dump(fs):
  """针对web.RequestHandler中的files分析"""
  s = StringIO.StringIO()
  print >>s, "root %s len:%d" % (type(fs), len(fs))
  for af in fs:
      print >>s, "%s -> %s, len:%d" % (af, type(fs[af]),len(fs[af]))
      for d in fs[af]:
          print >>s, "   %s" % (type(d))
          for i in d:
              print >>s, "    %s, len:%d" %(i, len(d[i]))
  print >>s, fs
  print s.getvalue()

if __name__ == "__main__":
  import doctest
  doctest.testmod()