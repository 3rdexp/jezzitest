#!/usr/bin/env python
#
# Copyright 2007 Google Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
import cgi
import datetime
import wsgiref.handlers
import logging

from google.appengine.ext import db
from google.appengine.api import users
from google.appengine.ext import webapp
from google.appengine.api import urlfetch

class Greeting(db.Model):
  author = db.UserProperty()
  content = db.StringProperty(multiline=True)
  date = db.DateTimeProperty(auto_now_add=True)



"""
google.appengine.ext.webapp.Request
  Properties:
    uri: the complete URI requested by the user
    scheme: 'http' or 'https'
    host: the host, including the port
    path: the path up to the ';' or '?' in the URL
    parameters: the part of the URL between the ';' and the '?', if any
    query: the part of the URL after the '?'
"""



class MainPage(webapp.RequestHandler):
  def get(self):
    url = "http://www.google.com"
    
    
    logging.info(self.request)
    
    if self.request.path.startswith('/hot'):
      url += self.request.path[len('/hot'):]
      
    if self.request.query:
      url += "?" + self.request.query
      
    # self.response.out.write(url)
    # logging.info('url: %s', url)
    # logging.info('q: %s', type(self.request))
    
    
    
    # result = urlfetch.fetch(url, headers=self.request.parameters)
    #if result.status_code == 200:
    #  self.response.out.write(result.content)
    return
    
    # logging.info('host_url %s', self.request.path)    
    # logging.info('application_url %s', self.request.application_url())
    
    result = urlfetch.fetch(url)
    if result.status_code == 200:
      self.response.out.write(result.content)
    


class Guestbook(webapp.RequestHandler):
  def post(self):
    greeting = Greeting()

    if users.get_current_user():
      greeting.author = users.get_current_user()

    greeting.content = self.request.get('content')
    greeting.put()
    self.redirect('/')


application = webapp.WSGIApplication([
  ('/hot/.*', MainPage),
  ('/sign', Guestbook)
], debug=True)


def main():
  wsgiref.handlers.CGIHandler().run(application)


if __name__ == '__main__':
  main()
