# 
tornado 不支持指出正确的 .ico Content-Type
修改 pyhton/Lib/mimetypes.py，加入

'.ico'    : 'image/x-icon',

# 验证码
## PIL
[windows]
执行
http://effbot.org/zone/python-register.htm
http://effbot.org/media/downloads/PIL-1.1.6.win32-py2.6.exe

* 使用1.1.7会有问题
--- PIL CORE support ok
--- TKINTER support ok
--- JPEG support ok
--- ZLIB (PNG/ZIP) support ok
*** FREETYPE2 support not installed
--- LITTLECMS support ok

Lib\site-packages\PIL\_imagingft.pyd 需要 msvcr90.dll

执行 selftest.py 检测

[linux]
TODO:

## captcha
http://svn.navi.cx/misc/trunk/pycaptcha/

测试
simple_example.py

另一个选择是 http://code.activestate.com/recipes/440588-human-verification-test-captcha/
还未进行测试

* Captcha.Factory() 不会定期删除过期数据，而是在new/test等地方清理，不是一个好的策略
* 生成图时，会读入底图文件，消耗太大?


# simplejson系统安装的版本可能有 c module

PYTHONPATH=pymongo:pycaptcha:simplejson
set PYTHONPATH=pymongo;pycaptcha;simplejson


# 位置信息设置
可以随时选择改变关注位置
得到上网位置(现在的技术貌似达不到实用水平)
1 navigator.geolocation.getCurrentPosition | mapabc.com | map.google.com

mapabc 示例
http://code.mapabc.com/class.html

2 从地图上选择一个位置
先使用 mapabc 的 API

# 
发布信息时，找到感兴趣的人，给每个人的 feedlist push 一条记录

publish:
[content         ] [submit]
[位置: 缺省位置][改变]

改变位置后，可以添加对新位置的关注

# latitude 度能到110 km
# longtitude 能到0-111 km

建立索引
db.user.ensureIndex({c : '2d', n : 1}, {min:-180000, max:180000})

头像可以使用
http://brandonmathis.com/projects/fancy-avatars/demo/

upload with ajax
http://valums.com/ajax-upload/

压缩js
http://code.google.com/closure/compiler/docs/compilation_levels.html
http://code.google.com/closure/compiler/docs/api-ref.html

jquery default text
http://www.dailycoding.com/Posts/default_text_fields_using_simple_jquery_trick.aspx

json decode
http://code.google.com/p/jspickle/
http://www.undefined.org/python/


# email
一直摇摆不定，是使用 email 作为用户名呢，还是不用

== user table
email: 登录名称，不可更改
nick: 显示名字，可以更改的
email_verified:
passwd: 

== focus table
uid, [x, y], radius

== feed
== feedlist


TODO:
拆分 focus 表
  class User
    staticmethod New/Remove
能添加感兴趣的地点，多个


完成注册 
google oauth
renren oauth
