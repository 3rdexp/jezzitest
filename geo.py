#!/usr/bin/python
#coding:utf-8

import math

"""经纬度和度分秒(dms)转换"""

def todms(afloat):
  """
  >>> print '%d %d %.2f' % todms(116.34412714235)
  116 20 38.86
  >>> print '%d %d %.2f' % todms(40.083411296333)
  40 5 0.28
  >>> print '%d %d %.2f' % todms(117.21951)
  117 13 10.24
  """
  deg = math.trunc(afloat)
  min = math.trunc((afloat - deg)*60)
  sec = round((afloat - deg - float(min)/60) * 60 * 60,  2)
  return (deg,  min,  sec)
  
def fromdms(*args):
  """
  >>> print '%.10f' % fromdms(57,55, 56.6)
  57.9323888889
  """
  if len(args) == 3:
    d, m, s = args
  else:
    d, m, s = args[0]
  return d + float(m)/60 + float(s)/3600

# Decimal Degress
def todms2(*args):
  # 纬度 lat -90 90
  # 经度 lng -180 180
  lng = len(args) == 1 and args[0][0] or args[0]
  lat = len(args) == 1 and args[0][1] or args[1]

  assert(lng >=-180 and lng <= 180)
  assert(lat >=-90 and lat <= 90)
  
  return todms(lng), todms(lat)
  
if __name__ == "__main__":
  print todms2((116.34412714235, 40.083411296333))
  print todms2(116.34412714235, 40.083411296333)
  # use python -m doctest -v geo.py

