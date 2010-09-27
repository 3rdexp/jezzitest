#!/usr/bin/python
#coding:utf-8

import sys, base64

def process(fn):
  f = open(fn,  'rb')
  d = f.read()
  f.close()
  
  print base64.b64encode(d)

if __name__ == "__main__":
  process(sys.argv[1])
