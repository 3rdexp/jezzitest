#!/usr/bin/python
#coding:utf-8

from HTMLParser import HTMLParser
import StringIO

class UniParser(HTMLParser):
  def __init__(self):
    HTMLParser.__init__(self)
    self.io_ = StringIO.StringIO()
    self.stack_ = list()
    if 0 == len(self.SafeTagTable):
      InitSafeTag(self.SafeTagTable)
      InitSafeAttr(self.SafeAttrTable)

  def handle_starttag(self, tag, attrs):
    if not self.SafeTag(tag):
      return

    self.stack_.append(tag)

    self.Emit('<%s' % tag)
    for p in attrs:
      if self.SafeAttr(p[0]):
        self.Emit(' %s=\"%s\"' % (p[0],  p[1]))
    self.Emit('>')

  def handle_endtag(self, tag):
    if len(self.stack_):
      self.Emit('</%s>' % self.stack_.pop())

  def handle_data(self, data):
    self.Emit(data)
  
  @staticmethod
  def SafeAttr(attr):
    return attr in UniParser.SafeAttrTable
    
  @staticmethod
  def SafeTag(tag):
    return tag in UniParser.SafeTagTable

  SafeTagTable= set()
  SafeAttrTable = set()
  
  def Emit(self, text):
    self.io_.write(text)

  @property
  def html(self):
    if 0 == len(self.stack_):
      return self.io_.getvalue()
    return ''

  def Parse(self, text):
    self.feed(text)
    self.close()

def InitSafeTag(aset):
  for i in ('a',  'span',  'div',  'font',  'br'
            ,  'h1',  'h2',  'h3', 'h4', 'h5',  'h6'
            ,  'table',  'tr',  'td', 'thead', 'tbody'
            ,  'p',  'dl', 'td', 'dd', 'ol', 'ul', 'li'
            ,  'blockquote',  'abbr', 'em', 'strong', 'b', 'i', 'u'
            ,  'big', 'small', 'font', 'q', 'sub', 'img'):
    aset.add(i)

def InitSafeAttr(aset):
  for i in ('href',  'border', 'bgcolor', 'color', 'cols',  'colspan'
            , 'name', 'nohref', 'rows', 'rowspan', 'src', 'title'):
    aset.add(i)

import unittest
class FeedTestCase(unittest.TestCase):
  def setUp(self):
    pass
    
  def tearDown(self):
    pass

if __name__ == "__main__":
  arr = ['<a b=c>', 
    '<a href=c>d</a>', 
    '<a href=c>d</e>', 
    '<a b>', 
    '<a href=c d=e>', 
    '<a>', 
    # '<a ', 
    '', 
    '<a script=\'alert()\'>link</a>', 
    '<a><span><div>'
    ]
  for i in arr:
    p = UniParser()
    p.Parse(i)
    print p.html
    
