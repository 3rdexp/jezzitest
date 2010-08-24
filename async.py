#!/usr/bin/python
#coding:utf-8

import threading

class async(object):
  def __init__(self, method):
    self.m_ = method
    
  def __call__(self, *args, **kwargs):
    t = threading.Thread(target = self.m_, args=args, kwargs=kwargs)
    t.start()

@async
def foo():
  print 'foo thread: %d' % threading.current_thread().ident

@async
def bar(v):
  print 'bar thread: %d' % threading.current_thread().ident
  print v

if __name__ == "__main__":
  print 'main thread: %d' % threading.current_thread().ident
  foo()
  bar(1)