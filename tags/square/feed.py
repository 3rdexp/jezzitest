>>> import pymongo
>>> c=pymongo.Connection('localhost', 27017)
>>> c
Connection('localhost', 27017)

>>> db = c.sq
>>> db.collection_names()
[u'user', u'system.indexes', u'q', u'feed', u'feedlist']
>>> dir(db)
['_Database__connection', '_Database__incoming_copying_manipulators', '_Database__incoming_manipulators', '_Database__name', '_Database__outgoing_copying_manipulators', '_Database__outgoing_manipulators', '_Database__system_js', '__call__', '__class__', '__cmp__', '__delattr__', '__dict__', '__doc__', '__getattr__', '__getattribute__', '__getitem__', '__hash__', '__init__', '__iter__', '__module__', '__new__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__str__', '__weakref__', '_fix_incoming', '_fix_outgoing', 'add_son_manipulator', 'add_user', 'authenticate', 'collection_names', 'command', 'connection', 'create_collection', 'dereference', 'drop_collection', 'error', 'eval', 'last_status', 'logout', 'name', 'next', 'previous_error', 'profiling_info', 'profiling_level', 'remove_user', 'reset_error_history', 'set_profiling_level', 'system_js', 'validate_collection']
>>> db.user.find_one()
{u'loc': 2.0, u'head': u'/p/head.jpg', u'uid': 1.0, u'token': {u'k': u'alsdfjlsejrwae', u'expire': u'2010/10/10 12:23:33'}, u'interest': [{u'radius': 200.0, u'center': [100.0, 200.0]}], u'_id': ObjectId('4c6d00818467aec5395abe3c'), u'name': u'\u5f20\u5c71'}
>>> db.user.find()    
<pymongo.cursor.Cursor object at 0x2b5825af1350>
>>> for i in db.user.find():
...   print i
... 
{u'loc': 2.0, u'head': u'/p/head.jpg', u'uid': 1.0, u'token': {u'k': u'alsdfjlsejrwae', u'expire': u'2010/10/10 12:23:33'}, u'interest': [{u'radius': 200.0, u'center': [100.0, 200.0]}], u'_id': ObjectId('4c6d00818467aec5395abe3c'), u'name': u'\u5f20\u5c71'}
{u'loc': 2.0, u'head': u'/p/head.jpg', u'uid': 2.0, u'token': {u'k': u'alsdfjlsejrwae', u'expire': u'2010/10/10 12:23:33'}, u'interest': [{u'radius': 200.0, u'center': [100.0, 201.0]}], u'_id': ObjectId('4c6d02ea8467aec5395abe40'), u'name': u'\u90bb\u5c45'}


>>> dir(pymongo)
['ALL', 'ASCENDING', 'Connection', 'DESCENDING', 'GEO2D', 'OFF', 'PyMongo_Connection', 'SLOW_ONLY', '__builtins__', '__doc__', '__file__', '__name__', '__path__', 'binary', 'bson', 'code', 'collection', 'connection', 'cursor', 'cursor_manager', 'database', 'dbref', 'errors', 'has_c', 'helpers', 'max_key', 'message', 'min_key', 'objectid', 'son', 'son_manipulator', 'timestamp', 'tz_util', 'version']
>>> dir(pymongo.objectid)
['InvalidId', 'ObjectId', '__builtins__', '__doc__', '__file__', '__name__', '_machine_bytes', '_md5func', 'calendar', 'datetime', 'md5', 'os', 'socket', 'struct', 'threading', 'time', 'utc']




def Create(feed):
  # init
  db.feed.save(feed)