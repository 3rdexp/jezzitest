var GNOTES_SERVER="http://www.google.com/notebook/",
GNOTES_DOM_ID="gnotes-",
GNOTES_RDF_ID="notebook@google.com",
GNOTES_PREFS_PREFIX="extensions.google.gnotes.",
GNOTES_CHROME="chrome://google-gnotes/";

var global=this;
function isDef(val){ return typeof val!="undefined"}
function isString(val){ return typeof val=="string" }
String.prototype.startsWith=function(prefix){return this.indexOf(prefix)==0};
String.prototype.endsWith=function(suffix){
 var l=this.length-suffix.length;
 return l>=0&&this.lastIndexOf(suffix,l)==l
};
String.prototype.trim=function(){return this.replace(/^\s+|\s+$/g,"")};
String.prototype.subs=function(){
  var ret=this;
  for(var i=0;i<arguments.length;i++)
    ret=ret.replace(/\%s/,String(arguments[i]));
  return ret
};

if(!Function.prototype.apply)
Function.prototype.apply=function(oScope,opt_args){
  var sarg=[],rtrn,call;
  if(!oScope)oScope=global;
  var args=opt_args||[];
  for(var i=0;i<args.length;i++)
    sarg[i]="args["+i+"]";
  call="oScope.__applyTemp__.peek()("+sarg.join(",")+");";
  if(!oScope.__applyTemp__)oScope.__applyTemp__=[];
  oScope.__applyTemp__.push(this);
  rtrn=eval(call);
  oScope.__applyTemp__.pop();
  return rtrn
};

if(!Array.prototype.push)Array.prototype.push=function(){for(var i=0;i<arguments.length;i++)this[this.length]=
arguments[i];return this.length};if(!Array.prototype.pop)Array.prototype.pop=function(){if(!this.length)return;var val=this[this.length-1];this.length--;return val};Array.prototype.peek=function(){return this[this.length-1]};if(!Array.prototype.shift)Array.prototype.shift=function(){if(this.length==0)return;var val=this[0];for(var i=0;i<this.length-1;i++)this[i]=this[i+1];this.length--;return val};if(!Array.prototype.unshift)Array.prototype.unshift=function(){var numArgs=arguments.length;for(var i=
this.length-1;i>=0;i--)this[i+numArgs]=this[i];for(var j=0;j<numArgs;j++)this[j]=arguments[j];return this.length};if(!Array.prototype.forEach)Array.prototype.forEach=function(callback,opt_scope){for(var i=0;i<this.length;i++)callback.call(opt_scope,this[i],i,this)};function bind(fn,self){var boundargs=fn.boundArgs_||[];boundargs=boundargs.concat(Array.prototype.slice.call(arguments,2));if(typeof fn.boundSelf_!="undefined")self=fn.boundSelf_;if(typeof fn.boundFn_!="undefined")fn=fn.boundFn_;var newfn=
function(){var args=boundargs.concat(Array.prototype.slice.call(arguments));return fn.apply(self,args)};newfn.boundArgs_=boundargs;newfn.boundSelf_=self;newfn.boundFn_=fn;return newfn}Function.prototype.bind=function(self){return bind.apply(null,[this,self].concat(Array.prototype.slice.call(arguments,1)))};Function.prototype.partial=function(){return bind.apply(null,[this,null].concat(Array.prototype.slice.call(arguments)))};Function.prototype.inherits=function(parentCtor){var tempCtor=function(){};
tempCtor.prototype=parentCtor.prototype;this.superClass_=parentCtor.prototype;this.prototype=new tempCtor};Function.prototype.mixin=function(props){for(var x in props)this.prototype[x]=props[x];if(typeof props.toString=="function"&&props.toString!=this.prototype.toString)this.prototype.toString=props.toString};function ListDictionary(name){this.name_=name;this.members_=[]}ListDictionary.prototype.isMember=function(item){for(var i=0;i<this.members_.length;i++)if(this.members_[i]==item)return true;return false};ListDictionary.prototype.addMember=function(item){this.members_.push(item)};ListDictionary.prototype.removeMember=function(item){for(var i=0;i<this.members_.length;i++)if(this.members_[i]==item){for(var j=i;j<this.members_.length;j++)this.members_[j]=this.members_[j+1];this.members_.length--;return true}return false};
ListDictionary.prototype.forEach=function(func){if(typeof func!="function")throw new Error("argument to forEach is not a function, it's a(n) "+typeof func);for(var i=0;i<this.members_.length;i++)func(this.members_[i])};function EventRegistrar(eventTypes){this.eventTypes=[];this.listeners_={};if(eventTypes instanceof Array)var events=eventTypes;else if(typeof eventTypes=="object"){var events=[];for(var e in eventTypes)events.push(eventTypes[e])}else throw new Error("Unrecognized init parameter to EventRegistrar");for(var i=0;i<events.length;i++){this.eventTypes.push(events[i]);this.listeners_[events[i]]=new ListDictionary(events[i]+"Listeners")}}EventRegistrar.prototype.isKnownEventType=function(eventType){for(var i=
0;i<this.eventTypes.length;i++)if(eventType==this.eventTypes[i])return true;return false};EventRegistrar.prototype.addEventType=function(eventType){if(this.isKnownEventType(eventType))throw new Error("Event type already known: "+eventType);this.eventTypes.push(eventType);this.listeners_[eventType]=new ListDictionary(eventType+"Listeners")};EventRegistrar.prototype.registerListener=function(eventType,listener){if(!this.isKnownEventType(eventType))throw new Error("Unknown event type: "+eventType);this.listeners_[eventType].addMember(listener)};
EventRegistrar.prototype.removeListener=function(eventType,listener){if(!this.isKnownEventType(eventType))throw new Error("Unknown event type: "+eventType);this.listeners_[eventType].removeMember(listener)};EventRegistrar.prototype.fire=function(eventType,e){if(!this.isKnownEventType(eventType))throw new Error("Unknown event type: "+eventType);var invoke=function(listener){listener(e)};this.listeners_[eventType].forEach(invoke)};

function G_Preferences(opt_startPoint,opt_getDefaultBranch,opt_noUnicode){this.debugZone="prefs";this.observers_={};var startPoint=opt_startPoint||null,prefSvc=Cc["@mozilla.org/preferences-service;1"].getService(Ci.nsIPrefService);this.prefs_=opt_getDefaultBranch?prefSvc.getDefaultBranch(startPoint):prefSvc.getBranch(startPoint);this.prefs_.QueryInterface(Ci.nsIPrefBranchInternal);this.noUnicode_=!(!opt_noUnicode)}
G_Preferences.setterMap_={string:"setCharPref","boolean":"setBoolPref",number:"setIntPref"};
G_Preferences.getterMap_={};G_Preferences.getterMap_[Ci.nsIPrefBranch.PREF_STRING]="getCharPref";G_Preferences.getterMap_[Ci.nsIPrefBranch.PREF_BOOL]="getBoolPref";G_Preferences.getterMap_[Ci.nsIPrefBranch.PREF_INT]="getIntPref";G_Preferences.prototype.setPref=function(key,val){var datatype=typeof val;if(datatype=="number"&&val%1!=0)throw new Error("Cannot store non-integer numbers in preferences.");if(datatype=="string"&&!this.noUnicode_)return this.setUnicodePref(key,val);var meth=G_Preferences.setterMap_[datatype];
if(!meth)throw new Error("Pref datatype {"+datatype+"} not supported.");return this.prefs_[meth](key,val)};G_Preferences.prototype.getPref=function(key,opt_default){var type=this.prefs_.getPrefType(key);if(type==Ci.nsIPrefBranch.PREF_INVALID)return opt_default;if(type==Ci.nsIPrefBranch.PREF_STRING&&!this.noUnicode_)return this.getUnicodePref(key,opt_default);var meth=G_Preferences.getterMap_[type];if(!meth)throw new Error("Pref datatype {"+type+"} not supported.");try{return this.prefs_[meth](key)}catch(e){return opt_default}};
G_Preferences.prototype.setUnicodePref=function(key,value){var s=Cc["@mozilla.org/supports-string;1"].createInstance(Ci.nsISupportsString);s.data=value;return this.prefs_.setComplexValue(key,Ci.nsISupportsString,s)};G_Preferences.prototype.getUnicodePref=function(key,opt_default){try{return this.prefs_.getComplexValue(key,Ci.nsISupportsString).data}catch(e){return opt_default}};G_Preferences.prototype.setBoolPref=function(which,value){return this.setPref(which,value)};G_Preferences.prototype.getBoolPref=
function(which){return this.prefs_.getBoolPref(which)};G_Preferences.prototype.getBoolPrefOrDefault=function(which,def){return this.getPref(which,def)};G_Preferences.prototype.getBoolPrefOrDefaultAndSet=function(which,def){try{return this.prefs_.getBoolPref(which)}catch(e){this.prefs_.setBoolPref(which,!(!def));return def}};G_Preferences.prototype.clearPref=function(which){try{this.prefs_.clearUserPref(which)}catch(e){}};G_Preferences.prototype.addObserver=function(which,callback){var observer=new G_PreferenceObserver(callback);
if(!this.observers_[which])this.observers_[which]=new G_ObjectSafeMap;this.observers_[which].insert(callback,observer);this.prefs_.addObserver(which,observer,false)};G_Preferences.prototype.removeObserver=function(which,callback){var observer=this.observers_[which].find(callback);this.prefs_.removeObserver(which,observer);this.observers_[which].erase(callback)};G_Preferences.prototype.removeAllObservers=function(){for(var which in this.observers_){var observersMap=this.observers_[which],observers=
observersMap.getAllValues();for(var i=0;i<observers.length;i++){var observer=observers[i];this.prefs_.removeObserver(which,observer)}}this.observers_={}};G_Preferences.prototype.getChildNames=function(opt_startingPoint){if(!opt_startingPoint)opt_startingPoint="";return this.prefs_.getChildList(opt_startingPoint,{})};G_Preferences.savePrefFile=function(){var prefService=Cc["@mozilla.org/preferences;1"].getService(Ci.nsIPrefService);try{prefService.savePrefFile(null)}catch(e){}};

function G_PreferenceObserver(callback){this.debugZone=
"prefobserver";this.callback_=callback}G_PreferenceObserver.prototype.observe=function(subject,topic,data){this.callback_(data)};G_PreferenceObserver.prototype.QueryInterface=function(iid){var Ci=Ci;if(iid.equals(Ci.nsISupports)||iid.equals(Ci.nsIObserver)||iid.equals(Ci.nsISupportsWeakReference))return this;throw Components.results.NS_ERROR_NO_INTERFACE;};if(!isDef(false))throw new Error("G_GDEBUG constant must be set before loading debug.js");

function G_Debug(){}
function G_DebugL(){}
function G_Assert(){}
function G_DebugZone(){}
G_DebugZone.prototype.zoneIsEnabled=function(){};
G_DebugZone.prototype.enableZone=function(){};
G_DebugZone.prototype.disableZone=function(){};
G_DebugZone.prototype.debug=function(){};
G_DebugZone.prototype.error=function(){};
G_DebugZone.prototype.assert=function(){};

function G_DebugService(){}
G_DebugService.ERROR_LEVEL_INFO="INFO";
G_DebugService.ERROR_LEVEL_WARNING="WARNING";
G_DebugService.ERROR_LEVEL_EXCEPTION="EXCEPTION";
G_DebugService.TIMESTAMP_INTERVAL=300000;
G_DebugService.prototype.QueryInterface=function(iid){if(iid.equals(Ci.nsISupports)||iid.equals(Ci.nsITimerCallback))return this;throw Components.results.NS_ERROR_NO_INTERFACE;};
G_DebugService.prototype.alsoDumpToShell=function(){};
G_DebugService.prototype.alsoDumpToConsole=function(){};
G_DebugService.prototype.logFileIsEnabled=function(){};
G_DebugService.prototype.enableLogFile=
function(){};
G_DebugService.prototype.disableLogFile=function(){};
G_DebugService.prototype.getLogFile=function(){};
G_DebugService.prototype.setLogFile=function(){};
G_DebugService.prototype.closeLogFile=function(){this.logWriter_.close();this.logWriter_=null};
G_DebugService.prototype.enableDumpToShell=function(){};
G_DebugService.prototype.disableDumpToShell=function(){};
G_DebugService.prototype.enableDumpToConsole=function(){};
G_DebugService.prototype.disableDumpToConsole=function(){};
G_DebugService.prototype.getZone=
function(){};
G_DebugService.prototype.enableZone=function(){};
G_DebugService.prototype.disableZone=function(){};
G_DebugService.prototype.allZonesEnabled=function(){};
G_DebugService.prototype.enableAllZones=function(){};
G_DebugService.prototype.disableAllZones=function(){};
G_DebugService.prototype.callTracingEnabled=function(){};
G_DebugService.prototype.enableCallTracing=function(){};
G_DebugService.prototype.disableCallTracing=function(){};
G_DebugService.prototype.getLogFileErrorLevel=function(){};
G_DebugService.prototype.setLogFileErrorLevel=function(){};
G_DebugService.prototype.notify=function(){if(!this.activeSinceLastTimestamp_)return;this.dump(G_File.LINE_END_CHAR+"=========== Date: "+(new Date).toLocaleString()+" ============"+G_File.LINE_END_CHAR+G_File.LINE_END_CHAR);this.activeSinceLastTimestamp_=false};
G_DebugService.prototype.dump=function(){};
G_DebugService.prototype.maybeDumpToFile=function(msg){if(this.logFileIsEnabled()&&this.logFile_){if(!this.logWriter_)this.logWriter_=new G_FileWriter(this.logFile_,
true);this.logWriter_.write(msg)}};
G_DebugService.prototype.observe=function(){};
G_DebugService.prototype.reportScriptError_=function(message,sourceName,lineNumber,label){if(sourceName.startsWith("http"))return;var message=["","------------------------------------------------------------",label+": "+message,"location: "+sourceName+", line: "+lineNumber,"------------------------------------------------------------","",""].join(G_File.LINE_END_CHAR);if(this.alsoDumpToShell())dump(message);this.maybeDumpToFile(message)};

function G_Loggifier(){}
G_Loggifier.prototype.mark_=function(){};
G_Loggifier.prototype.isLoggified=function(){};
G_Loggifier.prototype.getFunctionName_=function(){};
G_Loggifier.prototype.loggify=function(){};

function G_DebugSettings(){this.defaults_={};this.prefs_=new G_Preferences}
G_DebugSettings.prototype.getSetting=function(name,opt_default){var override=this.prefs_.getPref(name,null);return override!==null?override:(name in this.defaults_?this.defaults_[name]:opt_default)};
G_DebugSettings.prototype.setDefault=function(name,val){this.defaults_[name]=val};
new G_DebugService;
var G_File={};
G_File.getHomeFile=function(opt_file){return this.getSpecialFile("Home",opt_file)};
G_File.getProfileFile=function(opt_file){return this.getSpecialFile("ProfD",opt_file)};
G_File.getTempFile=function(opt_file){return this.getSpecialFile("TmpD",opt_file)};
G_File.getSpecialFile=function(loc,opt_file){var file=Cc["@mozilla.org/file/directory_service;1"].getService(Ci.nsIProperties).get(loc,Ci.nsILocalFile);if(opt_file)file.append(opt_file);return file};
G_File.createUniqueTempFile=function(opt_baseName){var baseName=
(opt_baseName||(new Date).getTime())+".tmp",file=this.getSpecialFile("TmpD",baseName);file.createUnique(file.NORMAL_FILE_TYPE,420);return file};
G_File.createUniqueTempDir=function(opt_baseName){var baseName=(opt_baseName||(new Date).getTime())+".tmp",dir=this.getSpecialFile("TmpD",baseName);dir.createUnique(dir.DIRECTORY_TYPE,484);return dir};
G_File.fromFileURI=function(uri){if(uri.indexOf("file://")!=0)throw new Error("File path must be a file:// URL");
var fileHandler=Cc["@mozilla.org/network/protocol;1?name=file"].getService(Ci.nsIFileProtocolHandler);
return fileHandler.getFileFromURLSpec(uri)};
G_File.PR_RDONLY=1;
G_File.PR_WRONLY=2;
G_File.PR_RDWR=4;
G_File.PR_CREATE_FILE=8;
G_File.PR_APPEND=16;
G_File.PR_TRUNCATE=32;
G_File.PR_SYNC=64;
G_File.PR_EXCL=128;
G_File.__defineGetter__("LINE_END_CHAR",function(){
  var end_char;
  end_char="@mozilla.org/xre/app-info;1"in Cc?(Cc["@mozilla.org/xre/app-info;1"].getService(Ci.nsIXULRuntime).OS=="WINNT"?"\r\n":"\n"):(Cc["@mozilla.org/network/protocol;1?name=http"].getService(Ci.nsIHttpProtocolHandler).platform.toLowerCase().indexOf("win")==
0?"\r\n":"\n");
G_File.__defineGetter__("LINE_END_CHAR",function(){return end_char});return end_char});

function G_FileReader(file){this.file_=isString(file)?G_File.fromFileURI(file):file}
G_FileReader.readAll=function(file){var reader=new G_FileReader(file);try{return reader.read()}finally{reader.close()}};
G_FileReader.prototype.read=function(opt_maxBytes){if(!this.stream_){var fs=Cc["@mozilla.org/network/file-input-stream;1"].createInstance(Ci.nsIFileInputStream);fs.init(this.file_,G_File.PR_RDONLY,
292,0);this.stream_=Cc["@mozilla.org/scriptableinputstream;1"].createInstance(Ci.nsIScriptableInputStream);this.stream_.init(fs)}if(!isDef(opt_maxBytes))opt_maxBytes=this.stream_.available();return this.stream_.read(opt_maxBytes)};
G_FileReader.prototype.close=function(){if(this.stream_){this.stream_.close();this.stream_=null}};

function G_FileWriter(file,opt_append){this.file_=typeof file=="string"?G_File.fromFileURI(file):file;this.append_=!(!opt_append)}
G_FileWriter.writeAll=function(file,data,opt_append){var writer=
new G_FileWriter(file,opt_append);try{return writer.write(data)}finally{writer.close();return 0}};
G_FileWriter.prototype.write=function(data){if(!this.stream_){this.stream_=Cc["@mozilla.org/network/file-output-stream;1"].createInstance(Ci.nsIFileOutputStream);var flags=G_File.PR_WRONLY|G_File.PR_CREATE_FILE|(this.append_?G_File.PR_APPEND:G_File.PR_TRUNCATE);this.stream_.init(this.file_,flags,-1,0)}return this.stream_.write(data,data.length)};
G_FileWriter.prototype.writeLine=function(data){this.write(data+
G_File.LINE_END_CHAR)};
G_FileWriter.prototype.close=function(){if(this.stream_){this.stream_.close();this.stream_=null}};

function G_ObjectSafeMap(opt_name){this.debugZone="objectsafemap";this.name_=opt_name?opt_name:"noname";this.keys_=[];this.values_=[]}
G_ObjectSafeMap.prototype.indexOfKey_=function(key){for(var i=0;i<this.keys_.length;i++)if(this.keys_[i]===key)return i;return-1};
G_ObjectSafeMap.prototype.insert=function(key,value){if(key===null)throw new Error("Can't use null as a key");if(value===undefined)throw new Error("Can't store undefined values in this map");var i=this.indexOfKey_(key);if(i==-1){this.keys_.push(key);
this.values_.push(value)}else{this.keys_[i]=key;this.values_[i]=value}};
G_ObjectSafeMap.prototype.erase=function(key){var keyLocation=this.indexOfKey_(key),keyFound=keyLocation!=-1;if(keyFound){this.keys_.splice(keyLocation,1);this.values_.splice(keyLocation,1)}return keyFound};
G_ObjectSafeMap.prototype.find=function(key){var keyLocation=this.indexOfKey_(key);return keyLocation==-1?undefined:this.values_[keyLocation]};
G_ObjectSafeMap.prototype.replace=function(other){this.keys_=[];this.values_=[];
for(var i=0;i<other.keys_.length;i++){this.keys_.push(other.keys_[i]);this.values_.push(other.values_[i])}};
G_ObjectSafeMap.prototype.forEach=function(func){if(typeof func!="function")throw new Error("argument to forEach is not a function, it's a(n) "+typeof func);for(var i=0;i<this.keys_.length;i++)func(this.keys_[i],this.values_[i])};
G_ObjectSafeMap.prototype.getAllKeys=function(){return this.keys_};
G_ObjectSafeMap.prototype.getAllValues=function(){return this.values_};
G_ObjectSafeMap.prototype.size=function(){return this.keys_.length};
function G_Alarm(callback,delayMS,opt_repeating,opt_maxTimes){this.debugZone="alarm";this.callback_=callback;this.repeating_=!(!opt_repeating);var Cc=Components.classes,Ci=Components.interfaces;this.timer_=Cc["@mozilla.org/timer;1"].createInstance(Ci.nsITimer);var type=opt_repeating?this.timer_.TYPE_REPEATING_SLACK:this.timer_.TYPE_ONE_SHOT;this.maxTimes_=opt_maxTimes?opt_maxTimes:null;this.nTimes_=0;this.timer_.initWithCallback(this,delayMS,type)}
G_Alarm.prototype.cancel=function(){if(this.timer_){this.timer_.cancel();
this.timer_=null;this.callback_=null}};
G_Alarm.prototype.notify=function(){var ret=this.callback_();this.nTimes_++;if(this.repeating_&&typeof this.maxTimes_=="number"&&this.nTimes_>=this.maxTimes_)this.cancel();else if(!this.repeating_)this.cancel();return ret};
G_Alarm.prototype.QueryInterface=function(iid){if(iid.equals(Components.interfaces.nsISupports)||iid.equals(Components.interfaces.nsIObserver)||iid.equals(Components.interfaces.nsITimerCallback))return this;throw Components.results.NS_ERROR_NO_INTERFACE;
};

function G_ConditionalAlarm(callback,delayMS,opt_repeating,opt_maxTimes){
  G_Alarm.call(this,callback,delayMS,opt_repeating,opt_maxTimes);this.debugZone="conditionalalarm"}
G_ConditionalAlarm.inherits(G_Alarm);
G_ConditionalAlarm.prototype.notify=function(timer){
  var rv=G_Alarm.prototype.notify.call(this,timer);if(this.repeating_&&rv)this.cancel()};
  
var goog=goog||{};
goog.global=this;goog.evalWorksForGlobals_=null;
goog.provide=function(name){goog.exportPath_(name)};
goog.exportPath_=function(name,opt_object){
  var parts=name.split("."),cur=goog.global,part;if(!(parts[0]in cur)&&cur.execScript)cur.execScript("var "+parts[0]);while(parts.length&&(part=parts.shift()))if(!parts.length&&goog.isDef(opt_object))cur[part]=opt_object;else cur=cur[part]?cur[part]:(cur[part]={})};
  goog.getObjectByName=function(name){
    var parts=name.split("."),cur=goog.global;
    for(var part;part=parts.shift();)
      if(cur[part])
        cur=cur[part];
      else 
        return null;
      return cur
  };
goog.globalize=function(obj,opt_global){var global=opt_global||goog.global;for(var x in obj)global[x]=obj[x]};
goog.addDependency=function(){};
goog.require=function(){};
goog.useStrictRequires=false;
goog.basePath="";
goog.nullFunction=function(){};
goog.identityFunction=function(){return arguments[0]};
goog.abstractMethod=function(){throw Error("unimplemented abstract method");};
goog.typeOf=function(value){
  var s=typeof value;
  if(s=="object")
    if(value){
      if(typeof value.length=="number"&&typeof value.splice!="undefined"&&!goog.propertyIsEnumerable_(value,"length"))
        return"array";
      if(typeof value.call!="undefined")
        return"function"
      }
      else 
      return"null";
     else if(s=="function"&&typeof value.call=="undefined")
       return"object";
     return s
  };
if(Object.prototype.propertyIsEnumerable)
  goog.propertyIsEnumerable_=function(object,propName){
    return Object.prototype.propertyIsEnumerable.call(object,propName)
  };
  else goog.propertyIsEnumerable_=function(object,propName){
    if(propName in object)
      for(var key in object)
        if(key==propName&&Object.prototype.hasOwnProperty.call(object,propName))
          return true;
      return false
  };
goog.isDef=function(val){return typeof val!="undefined"};
goog.isNull=function(val){return val===null};
goog.isDefAndNotNull=function(val){
  return goog.isDef(val)&&!goog.isNull(val)
  };
goog.isArray=function(val){return goog.typeOf(val)=="array"};
goog.isArrayLike=function(val){
  var type=goog.typeOf(val);return type=="array"||type=="object"&&typeof val.length=="number"
  };
goog.isDateLike=function(val){return goog.isObject(val)&&typeof val.getFullYear=="function"};
goog.isString=function(val){return typeof val=="string"};
goog.isBoolean=function(val){return typeof val=="boolean"};
goog.isNumber=function(val){return typeof val=="number"};
goog.isFunction=function(val){return goog.typeOf(val)=="function"};
goog.isObject=function(val){var type=goog.typeOf(val);return type=="object"||type=="array"||type=="function"};
goog.getHashCode=function(obj){
  if(obj.hasOwnProperty&&obj.hasOwnProperty(goog.HASH_CODE_PROPERTY_))
    return obj[goog.HASH_CODE_PROPERTY_];
  if(!obj[goog.HASH_CODE_PROPERTY_])
    obj[goog.HASH_CODE_PROPERTY_]=++goog.hashCodeCounter_;
  return obj[goog.HASH_CODE_PROPERTY_]
  };
goog.removeHashCode=function(obj){if("removeAttribute"in obj)obj.removeAttribute(goog.HASH_CODE_PROPERTY_);try{delete obj[goog.HASH_CODE_PROPERTY_]}catch(ex){}};goog.HASH_CODE_PROPERTY_="closure_hashCode_";
goog.hashCodeCounter_=0;
goog.cloneObject=function(proto){var type=goog.typeOf(proto);if(type==
"object"||type=="array"){if(proto.clone)return proto.clone();var clone=type=="array"?[]:{};for(var key in proto)clone[key]=goog.cloneObject(proto[key]);return clone}return proto};
goog.bind=function(fn,self){var boundArgs=fn.boundArgs_;if(arguments.length>2){var args=Array.prototype.slice.call(arguments,2);if(boundArgs)args.unshift.apply(args,boundArgs);boundArgs=args}self=fn.boundSelf_||self;fn=fn.boundFn_||fn;var newfn,context=self||goog.global;newfn=boundArgs?function(){var args=Array.prototype.slice.call(arguments);
args.unshift.apply(args,boundArgs);return fn.apply(context,args)}:function(){return fn.apply(context,arguments)};newfn.boundArgs_=boundArgs;newfn.boundSelf_=self;newfn.boundFn_=fn;return newfn};
goog.partial=function(fn){var args=Array.prototype.slice.call(arguments,1);args.unshift(fn,null);return goog.bind.apply(null,args)};
goog.mixin=function(target,source){for(var x in source)target[x]=source[x]};
goog.now=Date.now||function(){return(new Date).getTime()};
goog.globalEval=function(script){if(goog.global.execScript)goog.global.execScript(script,
"JavaScript");else if(goog.global.eval){if(goog.evalWorksForGlobals_==null){goog.global.eval("var _et_ = 1;");if(typeof goog.global._et_!="undefined"){delete goog.global._et_;goog.evalWorksForGlobals_=true}else goog.evalWorksForGlobals_=false}if(goog.evalWorksForGlobals_)goog.global.eval(script);else{var doc=goog.global.document,scriptElt=doc.createElement("script");scriptElt.type="text/javascript";scriptElt.defer=false;scriptElt.appendChild(doc.createTextNode(script));doc.body.appendChild(scriptElt);
doc.body.removeChild(scriptElt)}}else throw Error("goog.globalEval not available");};goog.getMsg=function(str,opt_values){var values=opt_values||{};for(var key in values)str=str.replace(new RegExp("\\{\\$"+key+"\\}","gi"),values[key]);return str};goog.exportSymbol=function(publicPath,object){goog.exportPath_(publicPath,object)};goog.exportProperty=function(object,publicName,symbol){object[publicName]=symbol};if(!Function.prototype.apply)Function.prototype.apply=function(oScope,args){var sarg=[],rtrn,
call;if(!oScope)oScope=goog.global;if(!args)args=[];for(var i=0;i<args.length;i++)sarg[i]="args["+i+"]";call="oScope.__applyTemp__.peek().("+sarg.join(",")+");";if(!oScope.__applyTemp__)oScope.__applyTemp__=[];oScope.__applyTemp__.push(this);rtrn=eval(call);oScope.__applyTemp__.pop();return rtrn};Function.prototype.bind=function(self){if(arguments.length>1){var args=Array.prototype.slice.call(arguments,1);args.unshift(this,self);return goog.bind.apply(null,args)}else return goog.bind(this,self)};
Function.prototype.partial=function(){var args=Array.prototype.slice.call(arguments);args.unshift(this,null);return goog.bind.apply(null,args)};Function.prototype.inherits=function(parentCtor){goog.inherits(this,parentCtor)};goog.inherits=function(childCtor,parentCtor){function tempCtor(){}tempCtor.prototype=parentCtor.prototype;childCtor.superClass_=parentCtor.prototype;childCtor.prototype=new tempCtor;childCtor.prototype.constructor=childCtor};Function.prototype.mixin=function(source){goog.mixin(this.prototype,
source)};

function G_JSModule(){this.factoryLookup_={};this.categoriesLookup_={}}goog.exportSymbol("G_JSModule",G_JSModule);G_JSModule.prototype.registerObject=function(classID,contractID,className,instance,opt_categories){this.factoryLookup_[classID]=new G_JSFactory(Components.ID(classID),contractID,className,instance,opt_categories);this.categoriesLookup_[classID]=opt_categories};goog.exportProperty(G_JSModule.prototype,"registerObject",G_JSModule.prototype.registerObject);G_JSModule.prototype.registerSelf=
function(compMgr,fileSpec,location,type){compMgr=compMgr.QueryInterface(Ci.nsIComponentRegistrar);for(var factory in this.factoryLookup_){compMgr.registerFactoryLocation(this.factoryLookup_[factory].classID,this.factoryLookup_[factory].className,this.factoryLookup_[factory].contractID,fileSpec,location,type);this.factoryLookup_[factory].registerCategories()}};goog.exportProperty(G_JSModule.prototype,"registerSelf",G_JSModule.prototype.registerSelf);G_JSModule.prototype.getClassObject=function(compMgr,
classID){var factory=this.factoryLookup_[classID.toString()];if(!factory)throw new Error("Invalid classID {%s}".subs(classID));return factory};goog.exportProperty(G_JSModule.prototype,"getClassObject",G_JSModule.prototype.getClassObject);G_JSModule.prototype.canUnload=function(){return true};goog.exportProperty(G_JSModule.prototype,"canUnload",G_JSModule.prototype.canUnload);

function G_JSFactory(classID,contractID,className,instance,opt_categories){this.classID=classID;this.contractID=contractID;
this.className=className;this.instance_=instance;this.categories_=opt_categories}G_JSFactory.prototype.registerCategories=function(){if(this.categories_){var catMgr=Cc["@mozilla.org/categorymanager;1"].getService(Ci.nsICategoryManager);for(var i=0,cat;cat=this.categories_[i];i++)catMgr.addCategoryEntry(cat,this.className,this.contractID,true,true)}};G_JSFactory.prototype.createInstance=function(){return this.instance_};goog.exportProperty(G_JSFactory.prototype,"createInstance",G_JSFactory.prototype.createInstance);

function BindToObject(func){var args=Array.prototype.splice.call(arguments,1,arguments.length);return Function.prototype.bind.apply(func,args)};

function G_TabbedBrowserWatcher(tabBrowser,name,opt_filterAboutBlank){this.debugZone="tabbedbrowserwatcher";this.tabBrowser_=tabBrowser;this.filterAboutBlank_=!(!opt_filterAboutBlank);this.events=G_TabbedBrowserWatcher.events;this.name_=name;this.mark_=G_TabbedBrowserWatcher.mark_+"-"+this.name_;this.registrar_=new EventRegistrar(G_TabbedBrowserWatcher.events);this.tabbox_=this.getTabBrowser().mTabBox;this.onDOMContentLoadedClosure_=BindToObject(this.onDOMContentLoaded,this);this.tabbox_.addEventListener("DOMContentLoaded",
this.onDOMContentLoadedClosure_,true);this.onDOMNodeInsertedClosure_=BindToObject(this.onDOMNodeInserted,this);this.tabbox_.addEventListener("DOMNodeInserted",this.onDOMNodeInsertedClosure_,true);this.onTabSwitchClosure_=BindToObject(this.onTabSwitch,this);this.tabbox_.addEventListener("select",this.onTabSwitchClosure_,true);var win=this.tabbox_.ownerDocument.defaultView;this.onChromeUnloadClosure_=BindToObject(this.onChromeUnload_,this);win.addEventListener("unload",this.onChromeUnloadClosure_,false);
this.lastTab_=this.getCurrentBrowser();this.detectNewTabs_()}G_TabbedBrowserWatcher.events={DOMCONTENTLOADED:"domcontentloaded",LOCATIONCHANGE:"locationchange",PAGESHOW:"pageshow",PAGEHIDE:"pagehide",LOAD:"load",UNLOAD:"unload",TABLOAD:"tabload",TABUNLOAD:"tabunload",TABSWITCH:"tabswitch",TABMOVE:"tabmove"};G_TabbedBrowserWatcher.mark_="watcher-marked";G_TabbedBrowserWatcher.prototype.onChromeUnload_=function(){if(this.tabbox_){this.tabbox_.removeEventListener("DOMContentLoaded",this.onDOMContentLoadedClosure_,
true);this.tabbox_.removeEventListener("DOMNodeInserted",this.onDOMNodeInsertedClosure_,true);this.tabbox_.removeEventListener("select",this.onTabSwitchClosure_,true);var win=this.tabbox_.ownerDocument.defaultView;win.removeEventListener("unload",this.onChromeUnloadClosure_,false);this.tabbox_=null}if(this.lastTab_)this.lastTab_=null;if(this.tabBrowser_)this.tabBrowser_=null};G_TabbedBrowserWatcher.prototype.isInstrumented_=function(browser){return!(!browser[this.mark_])};G_TabbedBrowserWatcher.prototype.instrumentBrowser_=
function(browser){G_Assert(this,!this.isInstrumented_(browser),"Browser already instrumented!");new G_BrowserWatcher(this,browser);browser[this.mark_]=true};G_TabbedBrowserWatcher.prototype.detectNewTabs_=function(){var tb=this.getTabBrowser();for(var i=0;i<tb.browsers.length;++i)this.maybeFireTabLoad(tb.browsers[i])};G_TabbedBrowserWatcher.prototype.registerListener=function(eventType,listener){this.registrar_.registerListener(eventType,listener)};G_TabbedBrowserWatcher.prototype.removeListener=
function(eventType,listener){this.registrar_.removeListener(eventType,listener)};G_TabbedBrowserWatcher.prototype.fire=function(eventType,e){this.registrar_.fire(eventType,e)};G_TabbedBrowserWatcher.prototype.fireDocEvent_=function(eventType,doc,browser){if(!this.tabBrowser_)return;try{var isTop=doc==browser.contentDocument}catch(e){var isTop=undefined}var inSelected=browser==this.getCurrentBrowser(),location=doc?doc.location.href:undefined;if(!this.filterAboutBlank_||location!="about:blank")this.fire(eventType,
{doc:doc,isTop:isTop,inSelected:inSelected,browser:browser})};G_TabbedBrowserWatcher.prototype.maybeFireTabLoad=function(browser){if(!this.isInstrumented_(browser)){this.instrumentBrowser_(browser);this.fire(this.events.TABLOAD,{browser:browser})}};G_TabbedBrowserWatcher.prototype.onDOMContentLoaded=function(e){var doc=e.target,browser=this.getBrowserFromDocument(doc);if(!browser)return;this.maybeFireTabLoad(browser);this.fireDocEvent_(this.events.DOMCONTENTLOADED,doc,browser)};G_TabbedBrowserWatcher.prototype.onDOMNodeInserted=
function(e){if(e.target.localName!="tab")return;if(!isDef(e.target._tPos))return;var fromPos=e.target._tPos,toPos;for(var i=0;i<e.relatedNode.childNodes.length;i++){var child=e.relatedNode.childNodes[i];if(child==e.target){toPos=i;break}}this.fire(this.events.TABMOVE,{tab:e.target,fromIndex:fromPos,toIndex:toPos})};G_TabbedBrowserWatcher.prototype.onTabSwitch=function(e){if(e.target==null||e.target.localName!="tabs"&&e.target.localName!="tabpanels")return;var fromBrowser=this.lastTab_,toBrowser=this.getCurrentBrowser();
if(fromBrowser!=toBrowser){this.lastTab_=toBrowser;this.fire(this.events.TABSWITCH,{fromBrowser:fromBrowser,toBrowser:toBrowser})}};G_TabbedBrowserWatcher.prototype.getTabBrowser=function(){return this.tabBrowser_};G_TabbedBrowserWatcher.prototype.getCurrentBrowser=function(){return this.getTabBrowser().selectedBrowser};G_TabbedBrowserWatcher.prototype.getCurrentWindow=function(){return this.getCurrentBrowser().contentWindow};G_TabbedBrowserWatcher.prototype.getBrowserFromDocument=function(doc){

function docInWindow(doc,
win){if(win.document==doc)return true;if(win.frames)for(var i=0;i<win.frames.length;i++)if(docInWindow(doc,win.frames[i]))return true;return false}var browsers=this.getTabBrowser().browsers;for(var i=0;i<browsers.length;i++)if(docInWindow(doc,browsers[i].contentWindow))return browsers[i];return null};G_TabbedBrowserWatcher.prototype.getDocumentFromURL=function(url,opt_browser){

function docWithURL(win,url){if(win.document.location.href==url)return win.document;if(win.frames)for(var i=0;i<win.frames.length;i++){var rv=
docWithURL(win.frames[i],url);if(rv)return rv}return null}if(opt_browser)return docWithURL(opt_browser.contentWindow,url);var browsers=this.getTabBrowser().browsers;for(var i=0;i<browsers.length;i++){var rv=docWithURL(browsers[i].contentWindow,url);if(rv)return rv}return null};G_TabbedBrowserWatcher.prototype.getDocumentsFromURL=function(url,opt_browser){var docs=[];

function getDocsWithURL(win,url){if(win.document.location.href==url)docs.push(win.document);if(win.frames)for(var i=0;i<win.frames.length;i++)getDocsWithURL(win.frames[i],
url)}if(opt_browser)return getDocsWithURL(opt_browser.contentWindow,url);var browsers=this.getTabBrowser().browsers;for(var i=0;i<browsers.length;i++)getDocsWithURL(browsers[i].contentWindow,url);return docs};G_TabbedBrowserWatcher.prototype.getBrowserFromWindow=function(sub){
function containsSubWindow(sub,win){if(win==sub)return true;if(win.frames)for(var i=0;i<win.frames.length;i++)if(containsSubWindow(sub,win.frames[i]))return true;return false}var browsers=this.getTabBrowser().browsers;for(var i=
0;i<browsers.length;i++)if(containsSubWindow(sub,browsers[i].contentWindow))return browsers[i];return null};G_TabbedBrowserWatcher.getTabElementFromBrowser=function(tabBrowser,browser){for(var i=0;i<tabBrowser.browsers.length;i++)if(tabBrowser.browsers[i]==browser)return tabBrowser.mTabContainer.childNodes[i];return null};

function G_BrowserWatcher(tabbedBrowserWatcher,browser){this.debugZone="browserwatcher";this.parent_=tabbedBrowserWatcher;this.browser_=browser;this.onPageShowClosure_=BindToObject(this.onPageShow,
this);this.browser_.addEventListener("pageshow",this.onPageShowClosure_,true);this.onPageHideClosure_=BindToObject(this.onPageHide,this);this.browser_.addEventListener("pagehide",this.onPageHideClosure_,true);this.onLoadClosure_=BindToObject(this.onLoad,this);this.browser_.addEventListener("load",this.onLoadClosure_,true);this.onUnloadClosure_=BindToObject(this.onUnload,this);this.browser_.addEventListener("unload",this.onUnloadClosure_,true);this.webProgressListener_={QueryInterface:function(){return this},
onLocationChange:bind(this.onLocationChange,this),onStateChange:function(){},onStatusChange:function(){},onSecurityChange:function(){},onProgressChange:function(){}};this.browser_.addProgressListener(this.webProgressListener_,Ci.nsIWebProgress.NOTIFY_LOCATION)}G_BrowserWatcher.prototype.onLocationChange=function(webProgress,request,location){G_Debug(this,"onLocationChange for {%s}".subs(location.spec));if(!this.parent_)return;var e={browser:this.browser_,isTop:true};this.parent_.fire(this.parent_.events.LOCATIONCHANGE,
e)};G_BrowserWatcher.prototype.onPageShow=function(e){if(e.target&&e.target.nodeName=="#document"){var doc=e.target;this.parent_.fireDocEvent_(this.parent_.events.PAGESHOW,doc,this.browser_)}};G_BrowserWatcher.prototype.onLoad=function(e){if(!e.target)return;if(e.target.nodeName!="#document")return;var doc=e.target;this.parent_.fireDocEvent_(this.parent_.events.LOAD,doc,this.browser_)};G_BrowserWatcher.prototype.onUnload=function(e){var doc=e.target;if(doc&&doc.nodeName=="#document")this.parent_.fireDocEvent_("unload",
doc,this.browser_);if(this.browser_.docShell)return;if(!doc){this.parent_.fire(this.parent_.events.TABUNLOAD,{browser:this.browser_});this.browser_.removeEventListener("pageshow",this.onPageShowClosure_,true);this.browser_.removeEventListener("pagehide",this.onPageHideClosure_,true);this.browser_.removeEventListener("load",this.onLoadClosure_,true);this.browser_.removeEventListener("unload",this.onUnloadClosure_,true);this.parent_=null;this.browser_=null}};G_BrowserWatcher.prototype.onPageHide=function(e){if(e.target.nodeName!=
"#document")return;var doc=e.target;this.parent_.fireDocEvent_(this.parent_.events.PAGEHIDE,doc,this.browser_)};

function G_Base64(){this.byteToCharMap_={};this.charToByteMap_={};this.byteToCharMapWebSafe_={};this.charToByteMapWebSafe_={};this.init_()}G_Base64.ENCODED_VALS="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";G_Base64.ENCODED_VALS_WEBSAFE="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_=";G_Base64.prototype.init_=function(){for(var i=0;i<G_Base64.ENCODED_VALS.length;i++){this.byteToCharMap_[i]=G_Base64.ENCODED_VALS.charAt(i);this.charToByteMap_[this.byteToCharMap_[i]]=
i;this.byteToCharMapWebSafe_[i]=G_Base64.ENCODED_VALS_WEBSAFE.charAt(i);this.charToByteMapWebSafe_[this.byteToCharMapWebSafe_[i]]=i}};G_Base64.prototype.encodeByteArray=function(input,opt_webSafe){if(!(input instanceof Array))throw new Error("encodeByteArray takes an array as a parameter");var byteToCharMap=opt_webSafe?this.byteToCharMapWebSafe_:this.byteToCharMap_,output=[],i=0;while(i<input.length){var byte1=input[i],haveByte2=i+1<input.length,byte2=haveByte2?input[i+1]:0,haveByte3=i+2<input.length,
byte3=haveByte3?input[i+2]:0,outByte1=byte1>>2,outByte2=(byte1&3)<<4|byte2>>4,outByte3=(byte2&15)<<2|byte3>>6,outByte4=byte3&63;if(!haveByte3){outByte4=64;if(!haveByte2)outByte3=64}output.push(byteToCharMap[outByte1]);output.push(byteToCharMap[outByte2]);output.push(byteToCharMap[outByte3]);output.push(byteToCharMap[outByte4]);i+=3}return output.join("")};G_Base64.prototype.decodeString=function(input,opt_webSafe){if(input.length%4)throw new Error("Length of b64-encoded data must be zero mod four");
var charToByteMap=opt_webSafe?this.charToByteMapWebSafe_:this.charToByteMap_,output=[],i=0;while(i<input.length){var byte1=charToByteMap[input.charAt(i)],byte2=charToByteMap[input.charAt(i+1)],byte3=charToByteMap[input.charAt(i+2)],byte4=charToByteMap[input.charAt(i+3)];if(byte1===undefined||byte2===undefined||byte3===undefined||byte4===undefined)throw new Error("String contains characters not in our alphabet: "+input);var outByte1=byte1<<2|byte2>>4;output.push(outByte1);if(byte3!=64){var outByte2=
byte2<<4&240|byte3>>2;output.push(outByte2);if(byte4!=64){var outByte3=byte3<<6&192|byte4;output.push(outByte3)}}i+=4}return output};G_Base64.prototype.arrayifyString=function(str){var output=[];for(var i=0;i<str.length;i++){var c=str.charCodeAt(i);while(c>255){output.push(c&255);c>>=8}output.push(c)}return output};G_Base64.prototype.stringifyArray=function(array){var output=[];for(var i=0;i<array.length;i++)output[i]=String.fromCharCode(array[i]);return output.join("")};

function G_Requester(opt_timeoutMillis){this.status="";this.responseText="";this.timeout_=opt_timeoutMillis?opt_timeoutMillis:10000;this.timeoutExpired_=false}G_Requester.prototype.Open=function(url,options){G_Debug(this,"Sending Async Req To: %s".subs(url));var channel=Cc["@mozilla.org/network/io-service;1"].getService(Ci.nsIIOService).newChannel(url,null,null);channel.QueryInterface(Ci.nsIHttpChannel);if(options.postData){var newData=[];for(var i=0;i<options.postData.length;i++)if(options.postData.charCodeAt(i)>
128){G_Debug(this,"Fixing %s".subs(options.postData.charCodeAt(i)));newData.push("&#"+options.postData.charCodeAt(i)+";")}else newData.push(options.postData.charAt(i));newData=newData.join("");var uploadCh=channel.QueryInterface(Ci.nsIUploadChannel),sis=Cc["@mozilla.org/io/string-input-stream;1"].createInstance(Ci.nsIStringInputStream);sis.setData(newData,-1);uploadCh.setUploadStream(sis,"application/xml",-1);channel.requestMethod="POST"}if(isDef(options.username)||isDef(options.password)){var encoder=
new G_Base64,as=encoder.arrayifyString(options.username+":"+options.password),enc=encoder.encodeByteArray(as);channel.setRequestHeader("Authorization","BASIC "+enc,false)}channel.setRequestHeader("Content-Type","application/xml",false);channel.setRequestHeader("Accept-Encoding","compress, gzip",false);channel.setRequestHeader("Connection","close",false);if(options.headers)for(var headerName in options.headers)channel.setRequestHeader(headerName,options.headers[headerName],false);channel.asyncOpen(this,
null);this.timer_=new G_Alarm(this.Timeout.bind(this),this.timeout_)};G_Requester.prototype.Timeout=function(){this.timeoutExpired_=true;this.OnFailure()};G_Requester.prototype.onStartRequest=function(request){if(this.timeoutExpired_)return false;this.status=request.QueryInterface(Ci.nsIHttpChannel).responseStatus};G_Requester.prototype.onDataAvailable=function(request,context,inputStream,offset,count){if(this.timeoutExpired_)return false;var sis=Cc["@mozilla.org/scriptableinputstream;1"].createInstance(Ci.nsIScriptableInputStream);
sis.init(inputStream);var text=sis.read(count);this.responseText+=text;G_Debug(this,"Streaming: %s".subs(text))};G_Requester.prototype.onStopRequest=function(){if(this.timeoutExpired_)return false;this.timer_.cancel();this.OnSuccess()};G_Requester.prototype.OnSuccess=function(){};G_Requester.prototype.OnFailure=function(){};G_Requester.prototype.debugZone="G_Requester";

function G_ExtensionUpdater(id,opt_requestOptions){if(!id);else{this.RDFSvc_=Cc["@mozilla.org/rdf/rdf-service;1"].getService(Ci.nsIRDFService);this.requestOptions_=opt_requestOptions?opt_requestOptions:{};this.id=id;this.skipUpdating_=false;this.updating=false;this.preferences=new G_Preferences("extensions.google.updater.")}}G_ExtensionUpdater.prototype.CheckRegistryHive_=function(key,hive,extensionId){try{key.open(hive,"SOFTWARE\\Mozilla\\Firefox\\Extensions",Ci.nsIWindowsRegKey.ACCESS_READ|Ci.nsIWindowsRegKey.ACCESS_WRITE)}catch(e){return false}return key.hasValue(extensionId)};
G_ExtensionUpdater.prototype.IsInstalledByWindowsRegistry_=function(extensionId){var cc=Cc["@mozilla.org/windows-registry-key;1"];if(!cc)return false;var key=cc.createInstance(Ci.nsIWindowsRegKey);if(!key)return false;try{if(this.CheckRegistryHive_(key,Ci.nsIWindowsRegKey.ROOT_KEY_LOCAL_MACHINE,extensionId))return true;else if(this.CheckRegistryHive_(key,Ci.nsIWindowsRegKey.ROOT_KEY_CURRENT_USER,extensionId))return true}finally{key.close()}return false};G_ExtensionUpdater.prototype.RecognizeInstallationsByWindowsRegistry=
function(){this.skipUpdating_=this.IsInstalledByWindowsRegistry_(this.id)};G_ExtensionUpdater.prototype.Update=function(){if(this.updating||this.skipUpdating_)return false;this.updating=true;this.appVersion="";this.appID="";this.currentVersion="";this.updateURL="";this.updateLink="";this.updateVersion="";if(!this.GetCurrent()){this.Failure("'%s' not found in extension manager.".subs(this.name));return false}this.AttemptUpdate()};G_ExtensionUpdater.prototype.UpdatePeriodically=function(opt_makeTimer){if(this.skipUpdating_)return;
var lastUpdate=this.preferences.getPref(this.id,0),timeBetweenUpdates=86400000,nextUpdate=Number(lastUpdate)+timeBetweenUpdates,now=(new Date).getTime();if(now>nextUpdate){this.preferences.setPref(this.id,String(now));this.Update();nextUpdate=now+timeBetweenUpdates}if(opt_makeTimer)this.loop_=new G_Alarm(this.UpdatePeriodically.bind(this,true),nextUpdate-now,false)};G_ExtensionUpdater.prototype.CompareVersions=function(aV1,aV2){var v1=aV1.split("."),v2=aV2.split("."),numSubversions=v1.length>v2.length?
v1.length:v2.length;for(var i=0;i<numSubversions;i++){if(typeof v2[i]=="undefined")return 1;if(typeof v1[i]=="undefined")return-1;if(parseInt(v2[i],10)>parseInt(v1[i],10))return-1;else if(parseInt(v2[i],10)<parseInt(v1[i],10))return 1}return 0};G_ExtensionUpdater.prototype.GetCurrent=function(){var updItem=Cc["@mozilla.org/extensions/manager;1"].getService(Ci.nsIExtensionManager).getItemForID(this.id);if(updItem){var appInfo=Cc["@mozilla.org/xre/app-info;1"].getService(Ci.nsIXULAppInfo);this.name=
updItem.name;this.currentVersion=updItem.version;this.updateURL=updItem.updateRDF;this.updateURL=this.updateURL.replace(/%ITEM_VERSION%/gi,this.currentVersion).replace(/%ITEM_ID%/gi,this.id).replace(/%APP_VERSION%/gi,appInfo.version).replace(/%APP_ID%/gi,appInfo.ID);return true}return false};G_ExtensionUpdater.prototype.AttemptUpdate=function(){if(!this.updateURL)return false;this.req_=new G_Requester;this.req_.OnSuccess=this.OnReqSuccess.bind(this);this.req_.OnFailure=this.OnReqFailure.bind(this);
this.req_.Open(this.updateURL,this.requestOptions_)};G_ExtensionUpdater.prototype.OnReqFailure=function(){this.Failure("OnReqFailure")};G_ExtensionUpdater.prototype.OnReqSuccess=function(){if(this.req_.status!=200||!this.req_.responseText.match(/<rdf/gi)){this.Failure("Error: Invalid Update RDF contents. HTTP Status '%s'".subs(this.req_.status));return false}var uri=Cc["@mozilla.org/network/io-service;1"].getService(Ci.nsIIOService).newURI(this.updateURL,null,null),parser=Cc["@mozilla.org/rdf/xml-parser;1"].createInstance(Ci.nsIRDFXMLParser),
memoryDS=Cc["@mozilla.org/rdf/datasource;1?name=in-memory-datasource"].createInstance(Ci.nsIRDFDataSource);parser.parseString(memoryDS,uri,this.req_.responseText);var moz="http://www.mozilla.org/2004/em-rdf#",versionArc=this.RDFSvc_.GetResource(moz+"version"),updateLinkArc=this.RDFSvc_.GetResource(moz+"updateLink"),thisResource=null,dsResources=memoryDS.GetAllResources();while(dsResources.hasMoreElements()){thisResource=dsResources.getNext().QueryInterface(Ci.nsIRDFResource);var versionRes=memoryDS.GetTarget(thisResource,
versionArc,true);if(versionRes)this.updateVersion=versionRes.QueryInterface(Ci.nsIRDFLiteral).Value;var updateLinkRes=memoryDS.GetTarget(thisResource,updateLinkArc,true);if(updateLinkRes)this.updateLink=updateLinkRes.QueryInterface(Ci.nsIRDFLiteral).Value}if(this.updateVersion&&this.updateLink){G_Debug(this,"currentVersion:%s\nupdateVersion: %s\nupdateLink: %s".subs(this.currentVersion,this.updateVersion,this.updateLink));if(this.CompareVersions(this.updateVersion,this.currentVersion)==1)this.InstallUpdate();
else this.Failure("No need to update")}else this.Failure("No update info in rdf")};G_ExtensionUpdater.prototype.InstallUpdate=function(){if(!this.updateLink){this.Failure("Failure");return false}var manager=Cc["@mozilla.org/xpinstall/install-manager;1"].createInstance(Ci.nsIXPInstallManager);if(manager!=null){G_Debug(this,"UpdateLink: %s".subs(this.updateLink));this.OnUpdateAvailable();var items=[this.updateLink],p=new G_Preferences,autoupdate=p.getPref("extensions.google.%s.autoupdate".subs(this.id.split("@")[0]),
true);if(autoupdate==false){G_DebugL(this,"Extension '%s' would've been updated".subs(this.name));this.Success()}else{G_DebugL(this,"Extension '%s' updating...".subs(this.name));manager.initManagerFromChrome(items,items.length,this)}}else this.Failure("Error creating manager")};G_ExtensionUpdater.prototype.onStateChange=function(index,state,value){if(state==Ci.nsIXPIProgressDialog.INSTALL_DONE)if(value!=0)this.Failure("Download Error");else this.Success()};G_ExtensionUpdater.prototype.onProgress=
function(){};G_ExtensionUpdater.prototype.Failure=function(aMessage){G_Debug(this,"Failure: %s".subs(aMessage));this.updating=false;this.OnFail(aMessage)};G_ExtensionUpdater.prototype.Success=function(){this.updating=false;this.OnSuccess()};G_ExtensionUpdater.prototype.OnFail=function(){};G_ExtensionUpdater.prototype.OnSuccess=function(){};G_ExtensionUpdater.prototype.OnUpdateAvailable=function(){};G_ExtensionUpdater.prototype.debugZone="G_ExtensionUpdater";

function G_MozVersionNumber(version){this.debugZone="mozversion";this.version_=version;this.components_=this.version_.split(".");this.comparator_=Cc["@mozilla.org/xpcom/version-comparator;1"].getService(Ci.nsIVersionComparator)}G_MozVersionNumber.prototype.compareToString=function(v){return this.comparator_.compare(this.version_,v)};G_MozVersionNumber.prototype.isVersionOf=function(v){if(this.version_.indexOf("+")!=-1||v.indexOf("+")!=-1)return this.compareToString(v)==0;if(this.compareToString(v)==
0)return true;var vComponents=v.split(".");if(vComponents.length>this.components_)return false;for(var i=0;i<vComponents.length;i++)if(vComponents[i]!=this.components_[i])return false;return true};G_MozVersionNumber.prototype.getVersion=function(){return this.version_};

function G_ThisFirefoxVersion(){var version;try{var appInfo=Cc["@mozilla.org/xre/app-info;1"].getService(Ci.nsIXULAppInfo);version=appInfo.version}catch(e){version=(new G_Preferences).getPref("app.version")}if(!version)throw new Error("Couldn't get application version!");
G_MozVersionNumber.call(this,version);this.debugZone="firefoxversion"}G_ThisFirefoxVersion.inherits(G_MozVersionNumber);var G_FirefoxPlatform={UNKNOWN:0,WIN:1,MAC:2,LINUX:3};

function G_getFirefoxPlatform(){var os=Cc["@mozilla.org/xre/app-info;1"].getService(Ci.nsIXULRuntime).OS.toUpperCase(),platform=G_FirefoxPlatform.UNKNOWN;switch(os){case "WINNT":case "WINME":case "WIN98":platform=G_FirefoxPlatform.WIN;break;case "DARWIN":platform=G_FirefoxPlatform.MAC;break;case "LINUX":case "LINUX-GNU":case "FREEBSD":case "SUNOS":platform=
G_FirefoxPlatform.LINUX;break}return platform};

//
// GNOTES_AddButton
function GNOTES_AddButton(overlay){
  this.overlay_=overlay;
  this.manager_=GNOTES_Manager.GetInstance(overlay);
  this.showButton_=false;
  this.debugZone="gnotes-add-button"
}

GNOTES_AddButton.prototype.Init_=function(win,selr){
  var iDoc=this.iframe_.contentDocument,iBody=iDoc.body;
  while(iBody.firstChild)
    iBody.removeChild(iBody.firstChild);
  this.button_=this.MakeButton_(iDoc);
  this.AddButtonListeners_(this.iframe_.ownerDocument,win,selr);
  iBody.style.margin="0";
  iBody.style.padding="0";
  iBody.appendChild(this.button_);
  this.SizeIframe_(this.button_,this.iframe_)
};

GNOTES_AddButton.prototype.IsBlacklisted_=function(url){return this.manager_.IsInterestingSite(url)};
GNOTES_AddButton.prototype.OnAddClick_=function(win,text){
  var bod=text.replace(/^\s+|\s+$/g,""),permalink=this.manager_.GetPermalink(this.overlay_.GetCurrentBrowser().contentWindow),url=permalink[0],title=permalink[1];
  this.iframe_.parentNode.removeChild(this.iframe_);
  var sel=win.getSelection();
  sel.collapseToEnd();
  this.overlay_.MiniNotebook().AddEntry(bod,title,url,"nthis");
  return false
};
GNOTES_AddButton.prototype.GetX_=function(elt){if(elt!=null&&elt.tagName!="BODY")return elt.offsetLeft+this.GetX_(elt.offsetParent);return 0};
GNOTES_AddButton.prototype.GetY_=function(elt){if(elt!=null&&elt.tagName!="BODY")return elt.offsetTop+this.GetY_(elt.offsetParent);return 0};
GNOTES_AddButton.prototype.SizeIframe_=function(button,iframe){iframe.style.width=button.clientWidth+4+"px";iframe.style.height=button.clientHeight+4+"px"};
GNOTES_AddButton.prototype.SetShowButton=
function(newVal){this.showButton_=newVal};
GNOTES_AddButton.prototype.OnButtonMouseOver_=function(button){var iframe=this.iframe_;button.value=this.overlay_.GetMessage("noteThis")+" +";iframe.style.MozOpacity=1;this.SizeIframe_(button,iframe)};
GNOTES_AddButton.prototype.OnButtonMouseOut_=function(button){var iframe=this.iframe_;button.value="+";iframe.style.MozOpacity=0.7;this.SizeIframe_(button,iframe)};
GNOTES_AddButton.prototype.MakeIframe_=function(doc){var iframe=doc.createElement("iframe");this.overlay_.SetGNotesElementId(iframe,
"notemagic");iframe.setAttribute("frameborder","0");iframe.setAttribute("src",GNOTES_SERVER+"static_files/blank.html");var iframeStyle=iframe.style;iframeStyle.position="absolute";iframeStyle.display="block";iframeStyle.MozOpacity="0.7";iframeStyle.zIndex=500;iframeStyle.width="1px";iframeStyle.height="1px";return iframe};
GNOTES_AddButton.prototype.MakeButton_=function(doc){var button=doc.createElement("input");button.setAttribute("type","button");button.value="+";this.overlay_.SetGNotesElementId(button,
"add-button");var buttonStyle=button.style;buttonStyle.fontSize="83%";buttonStyle.fontFamily="sans-serif";buttonStyle.padding="0px 0px 0px 0px";buttonStyle.backgroundColor="#BADCFF";return button};
GNOTES_AddButton.prototype.AddButtonListeners_=function(doc,win,selr){var iframe=this.iframe_,button=this.button_;iframe.addEventListener("mouseover",this.OnButtonMouseOver_.bind(this,button),false);iframe.addEventListener("mouseout",this.OnButtonMouseOut_.bind(this,button),false);var text=GNOTES_Manager.SerializeRange(doc,
selr);button.addEventListener("mouseup",this.OnAddClick_.bind(this,win,text),true)};
GNOTES_AddButton.prototype.OnMouseClick=function(event){var win=event.view,selr=GNOTES_Manager.GetValidSelectionRange(win);if(selr==null){this.overlay_.NotifySelection(false);if(this.iframe_&&this.iframe_.parentNode)this.iframe_.parentNode.removeChild(this.iframe_)}};
GNOTES_AddButton.prototype.OnMouseDown=function(event){this.downX_=event.pageX;this.downY_=event.pageY;return true};
GNOTES_AddButton.prototype.OnMouseUp=
function(event){var win=event.view,doc=event.originalTarget.ownerDocument;this.ConfigureAddToNoteButton(win,doc)};
GNOTES_AddButton.prototype.ConfigureAddToNoteButton=function(win,doc){if(this.iframe_&&this.iframe_.parentNode)this.iframe_.parentNode.removeChild(this.iframe_);if(this.IsBlacklisted_(win.location.href))return true;if(doc.contentType!="text/html")return true;var selr=GNOTES_Manager.GetValidSelectionRange(win);if(selr==null){this.overlay_.NotifySelection(false);return true}this.overlay_.NotifySelection(true);
if(!this.showButton_)return true;this.iframe_=this.MakeIframe_(doc);this.iframe_.addEventListener("load",this.Init_.bind(this,win,selr.cloneRange()),false);var body=doc.body;body.appendChild(this.iframe_);var positionFinder=doc.createElement("a");selr.collapse(false);selr.insertNode(positionFinder);var desiredX,desiredY;desiredX=body.clientWidth-this.GetX_(positionFinder);desiredY=this.GetY_(positionFinder)+4;if(desiredX>body.clientWidth-200)desiredX=body.clientWidth-200;if(desiredX<100)desiredX=
100;if(desiredY<0)desiredY=0;var iframeStyle=this.iframe_.style;iframeStyle.top=desiredY+"px";iframeStyle.right=desiredX+"px";positionFinder.parentNode.removeChild(positionFinder);return true};

function GNOTES_ContextMenu(window,overlay,mini){this.win_=window;this.doc_=window.document;this.mini_=mini;this.overlay_=overlay;this.manager_=GNOTES_Manager.GetInstance(overlay);this.debugZone="gnotes-context-menu";this.menu_=this.mini_.GetMiniElement("context-menu");this.separator_=this.mini_.GetMiniElement("context-sep-open");this.pasteIntoItem_=this.mini_.GetMiniElement("context-paste");this.pasteNewItem_=this.mini_.GetMiniElement("context-paste-new");this.mini_.AttachListener(this.menu_,"popupshowing",
this.UpdateMenu.bind(this),true);this.openWindowItem_=this.mini_.GetMiniElement("context-openlink");this.openTabItem_=this.mini_.GetMiniElement("context-openlinkintab");this.openWindowItem_.addEventListener("command",this.OpenLinkInNewWindow_.bind(this),false);this.openTabItem_.addEventListener("command",this.OpenLinkInNewTab_.bind(this),false);var pasteCommand=this.mini_.PasteNewNote.bind(this.mini_);this.pasteNewItem_.addEventListener("command",pasteCommand,false)}GNOTES_ContextMenu.prototype.Destroy=
function(){this.menu_.parentNode.appendChild(this.menu_)};GNOTES_ContextMenu.prototype.UpdateMenu=function(event){this.UpdatePasteMenuItems_(event);this.UpdateLinkMenuItems_(event)};GNOTES_ContextMenu.prototype.UpdateLinkMenuItems_=function(){this.link_=this.GetLink_(this.doc_.popupNode);var isLink=this.link_!=null;this.separator_.hidden=this.openWindowItem_.hidden=this.openTabItem_.hidden=!isLink};GNOTES_ContextMenu.prototype.UpdatePasteMenuItems_=function(){this.pasteIntoItem_.hidden=!this.mini_.InEditingMode();
this.pasteNewItem_.hidden=!this.mini_.InNotebookMode();this.pasteNewItem_.setAttribute("disabled",!this.manager_.CanPasteNewNote())};GNOTES_ContextMenu.prototype.GetLink_=function(node){while(node&&node.getAttribute){var link=node.getAttribute("href");if(link)return link;node=node.parentNode}return null};GNOTES_ContextMenu.prototype.OpenLinkInNewTab_=function(){this.overlay_.NavigateUrl(this.link_,2)};GNOTES_ContextMenu.prototype.OpenLinkInNewWindow_=function(){this.overlay_.NavigateUrl(this.link_,
1)};

function GNOTES_Manager(overlay){
  this.debugZone="gnotesManager";
  var xhr=overlay.Window().XMLHttpRequest;
  this.xhr_=new xhr;
  this.domainUtil_=new G_Domains(new xhr);
  this.windowWatcher_=Cc["@mozilla.org/appshell/window-mediator;1"].getService(Ci.nsIWindowMediator);
  var win=overlay.Window();
  this.browserUri_=win.document.location.href;
  this.windowType_=win.document.getElementById("main-window").getAttribute("windowtype");
  this.ResetIframeCache();
  this.prefs_=new G_Preferences(GNOTES_PREFS_PREFIX,false,true);
  this.RefreshOnPageUrl();
  this.prefs_.addObserver("onpageurl",this.RefreshOnPageUrl.bind(this));
  this.clipboard_=Cc["@mozilla.org/widget/clipboard;1"].getService(Ci.nsIClipboard);
  this.clipboardFlavorArray_=Cc["@mozilla.org/supports-array;1"].createInstance(Ci.nsICollection);
  this.clipboardFlavorArrayJs_=[];this.flavors_=new win.FlavourSet;
  this.transferHelper_=Cc["@mozilla.org/widget/transferable;1"].createInstance(Ci.nsITransferable);
  this.useNewClipboardApis_=false;
  for(var i=0;i<this.CLIPBOARD_FLAVORS.length;i++){
    var flavor=this.CLIPBOARD_FLAVORS[i];
    this.transferHelper_.addDataFlavor(flavor);
    var string=Cc["@mozilla.org/supports-cstring;1"].createInstance(Ci.nsISupportsCString);
    string.data=flavor;
    this.clipboardFlavorArray_.AppendElement(string);
    this.clipboardFlavorArrayJs_.push(flavor);this.flavors_.appendFlavour(flavor)}this.overlays_={};this.overlayCount_=0;this.toggleTearawayHelper_=this.ToggleTearawayHelper_.bind(this)
  }
GNOTES_Manager.singleton_=null;
GNOTES_Manager.GetInstance=function(overlay){if(GNOTES_Manager.singleton_==
null)GNOTES_Manager.singleton_=GNOTES_Manager.CreateInstance_(overlay);GNOTES_Manager.singleton_.TrackOverlay_(overlay);return GNOTES_Manager.singleton_};GNOTES_Manager.CreateInstance_=function(){throw new Error("Override this method to create suitable instance of GNOTES_Manager");};
GNOTES_Manager.GWS_SEARCH_PROPERTIES_={search:1,images:1,groups:1,news:1,products:1,maps:1,"reader/view/":1,books:1};
GNOTES_Manager.prototype.CLIPBOARD_FLAVORS=["text/html","text/unicode"];
GNOTES_Manager.prototype.windowWatcher_=
null;GNOTES_Manager.prototype.windowType_=null;GNOTES_Manager.prototype.miniMode_=null;GNOTES_Manager.prototype.GetCurrentOverlay=function(){var type=this.windowType_,watcher=this.windowWatcher_,win=watcher.getMostRecentWindow(type),overlays=this.GetAllOverlays();for(var i in overlays)if(overlays[i].Window()==win)return overlays[i];return null};GNOTES_Manager.prototype.TrackOverlay_=function(overlay){var id=overlay.Id();if(!this.overlays_[id]&&id!=-1){this.overlays_[id]=overlay;this.overlayCount_++;
var win=overlay.Window();win.addEventListener("close",this.RemoveOverlay_.bind(this,id),true)}};GNOTES_Manager.prototype.RemoveOverlay_=function(id){if(this.overlays_[id]){delete this.overlays_[id];this.overlayCount_--}};GNOTES_Manager.prototype.GetAllOverlays=function(){return this.overlays_};GNOTES_Manager.prototype.NumberOfOverlays=function(){return this.overlayCount_};GNOTES_Manager.prototype.OpenBrowserWindow=function(){var win=this.windowWatcher_.getMostRecentWindow(null);win.open()};GNOTES_Manager.prototype.GetOnChangeHandler=
function(){return null};
GNOTES_Manager.prototype.GetPrefs=function(){return this.prefs_};
GNOTES_Manager.prototype.RefreshOnPageUrl=function(){this.onpageUrl_=this.prefs_.getPref("onpageurl",this.GetDefaultOnPageUrl_());this.loggingUrl_=this.onpageUrl_.replace("onpage","z");this.frontendUrl_=this.onpageUrl_.replace("onpage","")};
GNOTES_Manager.prototype.GetDefaultOnPageUrl_=function(){return GNOTES_SERVER+"onpage"};
GNOTES_Manager.prototype.GetFlavors=function(){return this.flavors_};
GNOTES_Manager.prototype.IsInterestingSite=
function(url){var stripped=this.domainUtil_.StripDomain(url);if(url.startsWith(this.frontendUrl_))return true;{var isDomain=this.domainUtil_.IsDomain(url);return isDomain&&(stripped.startsWith("notebook")||stripped.startsWith("accounts/ServiceLogin"))}};
GNOTES_Manager.prototype.IsGWSResults=function(url){if(!this.domainUtil_.GetGoogleSubDomain(url))return false;var stripped=this.domainUtil_.StripDomain(url),startOfParams=stripped.indexOf("?");if(startOfParams!=-1)stripped=stripped.substring(0,startOfParams);
return GNOTES_Manager.GWS_SEARCH_PROPERTIES_[stripped]};
GNOTES_Manager.prototype.GetInterfaceObj=function(ifr){var win=ifr&&ifr.contentWindow;if(win)if(win.wrappedJSObject)return win.wrappedJSObject._GN_interface;else if(win._GN_interface)return win._GN_interface;return null};
GNOTES_Manager.prototype.GetMiniMode=function(){return this.GetPrefs().getPref("miniMode","yj")};
GNOTES_Manager.prototype.SetMiniMode=function(value){this.GetPrefs().setPref("miniMode",value)};
GNOTES_Manager.prototype.IsStatusTextHidden=
function(){return this.GetPrefs().getPref("hideStatusText",false)};
GNOTES_Manager.prototype.SetStatusTextHidden=function(value){this.GetPrefs().setPref("hideStatusText",value);var allOverlays=this.GetAllOverlays();for(var id in allOverlays)allOverlays[id].SetStatusTextHidden(value)};
GNOTES_Manager.prototype.IsAddButtonEnabled=function(){return this.GetPrefs().getPref("addButton",false)};
GNOTES_Manager.prototype.SetAddButtonEnabled=function(value){this.GetPrefs().setPref("addButton",value);var allOverlays=
this.GetAllOverlays();for(var id in allOverlays)allOverlays[id].SetShowAddButton(value)};
GNOTES_Manager.prototype.SetSoftState=function(softState){this.softState_=softState};
GNOTES_Manager.prototype.GetSoftState=function(){return this.softState_};
GNOTES_Manager.prototype.GetCloseMessageId=function(){return this.GetMiniMode()=="yj"?"closeNotebook":"popinNotebook"};
GNOTES_Manager.prototype.ToggleTearawayMode=function(){if(!this.overlayCount_)this.OpenBrowserWindow();this.SetMiniMode(this.GetMiniMode()==
"yj"?"tearaway":"yj");var currentOverlay=this.GetCurrentOverlay();if(currentOverlay){var currentNotebook=currentOverlay.MiniNotebook();currentNotebook.TryClose(undefined,this.toggleTearawayHelper_);var allOverlays=this.GetAllOverlays();for(var i in allOverlays){var overlay=allOverlays[i];if(overlay!=currentOverlay)if(overlay.MiniNotebook()!=currentNotebook)overlay.MiniNotebook().TryClose(undefined,overlay.CreateMiniNotebook.bind(overlay));else overlay.CreateMiniNotebook()}}};
GNOTES_Manager.prototype.ToggleTearawayHelper_=
function(){var currentOverlay=this.GetCurrentOverlay();if(currentOverlay){currentOverlay.CreateMiniNotebook();currentOverlay.OpenMiniNotebook()}this.LogAction(this.GetMiniMode()=="yj"?"popin":"popout")};
GNOTES_Manager.prototype.GetOnpageUrl=function(){return this.onpageUrl_};
GNOTES_Manager.prototype.SetIframeUrl=function(){throw new Error("You must override this method with extension-specific code.");};
GNOTES_Manager.prototype.ResetIframeCache=function(){this.cacheBreaker_=(new Date).getTime()};
GNOTES_Manager.prototype.SetHashParam=function(hash){this.hashParam_=hash};
GNOTES_Manager.prototype.LogAction=function(type){var data="atyp="+type;this.xhr_.open("POST",this.loggingUrl_,true);this.xhr_.setRequestHeader("Content-Type","application/x-www-form-urlencoded");this.xhr_.setRequestHeader("Content-Length",data.length);this.xhr_.send(data)};GNOTES_Manager.GetSelectionRange_=function(win){var sel=win.getSelection();return sel!=null&&sel.rangeCount>0?sel.getRangeAt(sel.rangeCount-1):null};GNOTES_Manager.GetValidSelectionRange=function(win){win=
win.wrappedJSObject||win;var sel=win.getSelection(),range=GNOTES_Manager.GetSelectionRange_(win),result=null;if(range){if(!/^(\s*)$/.test(sel.toString())&&sel.toString().trim().length>1)return range.cloneRange();var ca=range.commonAncestorContainer;if(ca!=null&&isDef(ca.getElementsByTagName)){var imgs=ca.getElementsByTagName("IMG");for(var i=0;i<imgs.length;++i){var compRange=win.document.createRange();compRange.selectNode(imgs[i]);if(range.compareBoundaryPoints(range.START_TO_START,compRange)==-1&&
range.compareBoundaryPoints(range.END_TO_END,compRange)==1)result=range.cloneRange();compRange.detach();if(result)return result}}}return result};GNOTES_Manager.SerializeRange=function(doc,range){if(range==null||doc==null)return null;var renderingDiv=doc.createElement("div");renderingDiv.appendChild(range.cloneContents());return renderingDiv.innerHTML.replace(/^\s+|\s+$/g,"")};
GNOTES_Manager.prototype.SelectionContainsNode=function(win,node){win=win.wrappedJSObject||win;var range=GNOTES_Manager.GetSelectionRange_(win);
if(range){var nodeRange=win.document.createRange();nodeRange.selectNode(node);return range.compareBoundaryPoints(range.START_TO_START,nodeRange)<=0&&range.compareBoundaryPoints(range.END_TO_END,nodeRange)>=0}return false};
GNOTES_Manager.prototype.GetPermalink=function(win){win=win.wrappedJSObject||win;var doc=win.document,url=doc.location.href+"",title=isString(doc.title)?doc.title:"",subdomain=this.domainUtil_.GetGoogleSubDomain(url);if(subdomain){var strippedDomain=this.domainUtil_.StripDomain(url);
if(subdomain=="www"&&strippedDomain.startsWith("map")||subdomain=="maps")try{var pageUrl=win.gApplication.getPageUrl();if(pageUrl)url=pageUrl}catch(e){}else if(subdomain=="mail")try{var jsFrame=doc.getElementsByName("js")[0].contentWindow,pageUrl=isDef(jsFrame._CV_CurrConvExtUrl)?jsFrame._CV_CurrConvExtUrl():null;if(pageUrl)url=pageUrl}catch(e){}else if(strippedDomain.startsWith("reader"))try{var permalink=win.getPermalink();if(permalink){url=permalink.url||url;title=permalink.title||title}}catch(e){}}if(!title){title=
url;title=title.substring(title.indexOf("//")+2)}return[url,title]};
GNOTES_Manager.prototype.CanPasteNewNote=function(){if(!this.useNewClipboardApi_)try{return this.clipboard_.hasDataMatchingFlavors(this.clipboardFlavorArray_,Ci.nsIClipboard.kGlobalClipboard)}catch(e){this.useNewClipboardApi_=true}return this.clipboard_.hasDataMatchingFlavors(this.clipboardFlavorArrayJs_,this.clipboardFlavorArrayJs_.length,Ci.nsIClipboard.kGlobalClipboard)};
GNOTES_Manager.prototype.GetAddableDataFromClipboard=function(){if(!this.clipboard_)return null;
if(!this.transferHelper_)return null;if(!this.CanPasteNewNote())return null;try{this.clipboard_.getData(this.transferHelper_,Ci.nsIClipboard.kGlobalClipboard);var string={},length={},type={};this.transferHelper_.getAnyTransferData(type,string,length)}catch(e){return null}if(!string.value||!length.value)return null;if(!string.value.QueryInterface(Ci.nsISupportsString))return null;return string.value.data};

function GNOTES_Mini(win,overlay){this.overlay_=overlay;this.win_=win;this.doc_=win.document;this.listeners_=[];this.manager_=GNOTES_Manager.GetInstance(overlay);this.dragDropCallback_={onDrop:this.DoDrop_.bind(this),onDragOver:this.EnableDrop_.bind(this),getSupportedFlavours:this.manager_.GetFlavors.bind(this.manager_)};this.debugZone="mini";this.callAfterNotebookLoads_=null;this.lastClose_=0;this.keyCaptureListener_=this.CatchKeys_.bind(this);this.IframeLoadCallback_=this.IframeLoadCallback_.bind(this)}
var FAILURE_PAGE=GNOTES_CHROME+"content/gnotes-offline.xul",KEYCODE_B="B".charCodeAt(0),KEYCODE_I="I".charCodeAt(0),KEYCODE_Z="Z".charCodeAt(0),KEYCODE_V="V".charCodeAt(0);GNOTES_Mini.prototype.CAPTURED_KEYS={};GNOTES_Mini.prototype.CAPTURED_KEYS[KEYCODE_B]=true;GNOTES_Mini.prototype.CAPTURED_KEYS[KEYCODE_I]=true;GNOTES_Mini.prototype.CAPTURED_KEYS[KEYCODE_Z]=true;GNOTES_Mini.prototype.CAPTURED_KEYS[KEYCODE_V]=true;GNOTES_Mini.VirtualFunction=function(){};GNOTES_Mini.prototype.Window_=function(){return this.Overlay_().Window()};
GNOTES_Mini.prototype.Overlay_=function(){return this.overlay_};GNOTES_Mini.prototype.InEditingMode=function(){var gnInterface=this.manager_.GetInterfaceObj(this.GetIframeElt());return gnInterface&&gnInterface._inEditingMode&&gnInterface._inEditingMode()};GNOTES_Mini.prototype.InNotebookMode=function(){var gnInterface=this.manager_.GetInterfaceObj(this.GetIframeElt());return gnInterface&&gnInterface._inEditingMode&&!gnInterface._inEditingMode()};GNOTES_Mini.prototype.Destroy=function(){this.RemoveListeners_();
this.lastClose_=0;this.callAfterNotebookLoads_=null};GNOTES_Mini.prototype.GetMiniElement=GNOTES_Mini.VirtualFunction;GNOTES_Mini.prototype.GetIframeElt=function(){if(!this.IsVisible())return null;return this.GetMiniElement("ifr")};GNOTES_Mini.prototype.IsVisible=GNOTES_Mini.VirtualFunction;GNOTES_Mini.prototype.ShowBox=GNOTES_Mini.VirtualFunction;GNOTES_Mini.prototype.TryClose=function(opt_event,opt_doAfter){this.doAfterClose_=opt_doAfter;var gnInterface=this.manager_.GetInterfaceObj(this.GetIframeElt());
if(gnInterface){if(opt_event)opt_event.preventDefault();var ifr=this.GetIframeElt(),win=ifr.contentWindow,hash=win&&win.location.hash;if(hash)this.manager_.SetHashParam(hash);if(this.lastClose_>=Date.now()-5000){var chromeWin=ifr.ownerDocument.defaultView;if(chromeWin.confirm(this.Overlay_().GetMessage("forceClose")))this.HideBox_();else{this.lastClose_=0;this.doAfterClose_=null}return}this.lastClose_=Date.now();gnInterface._tryClose()}else this.HideBox_()};GNOTES_Mini.prototype.HideBox_=function(opt_softState){this.Overlay_().SetStatusBarText("openNotebook");
this.callAfterNotebookLoads_=null;this.manager_.SetSoftState(opt_softState);this.lastClose_=0;if(this.doAfterClose_){this.doAfterClose_();this.doAfterClose_=null}};GNOTES_Mini.prototype.ReattachBox=GNOTES_Mini.VirtualFunction;GNOTES_Mini.prototype.RedrawBox=function(){};GNOTES_Mini.prototype.PositionBox=GNOTES_Mini.VirtualFunction;GNOTES_Mini.prototype.ResizeBox=GNOTES_Mini.VirtualFunction;GNOTES_Mini.prototype.ResizeBoxForFailurePage=function(){this.ResizeBox(300,150)};GNOTES_Mini.prototype.IframeContentLoadCallback_=
function(){var ifr=this.GetIframeElt();if(ifr){var url=ifr.contentDocument.URL;if(url&&url.startsWith("about:"))ifr.setAttribute("src",FAILURE_PAGE)}};GNOTES_Mini.prototype.IframeLoadCallback_=function(){var ifr=this.GetIframeElt();if(ifr==null)return;var win=ifr.contentWindow,doc=win.document,gnInterface=this.manager_.GetInterfaceObj(ifr);if(gnInterface==null){var url=doc.location.href+"";if(url==FAILURE_PAGE){this.PositionBox();this.ResizeBoxForFailurePage();this.Overlay_().SetStatusBarText(this.manager_.GetCloseMessageId());
var tryAgainLink=ifr.contentDocument.getElementById(GNOTES_DOM_ID+"tryAgain");tryAgainLink.addEventListener("click",this.manager_.SetIframeUrl.bind(this.manager_,ifr),true)}else{this.HideBox_();this.Overlay_().NavigateUrl(url,1)}return true}this.manager_.SetHashParam(null);this.PositionBox();this.Overlay_().SetStatusBarText(this.manager_.GetCloseMessageId());var softState=this.manager_.GetSoftState();if(gnInterface._setSoftState&&softState){gnInterface._setSoftState(softState);this.manager_.SetSoftState(null)}this.initialSizeSet_=
false;gnInterface._setResizeHandler(this.IframeResizeHandler.bind(this));gnInterface._setCloseHandler(this.HideBox_.bind(this));gnInterface._setGetCurrentPageHandler(this.IframeGetCurrentPageHandler.bind(this));gnInterface._setMaximizeHandler(this.IframeMaximizeHandler.bind(this));gnInterface._setOpenHandler(this.IframeOpenHandler.bind(this));gnInterface._selectionChanged(GNOTES_Manager.GetValidSelectionRange(this.Window_())!=null);if(gnInterface._setOnChangeHandler&&this.manager_.GetOnChangeHandler())gnInterface._setOnChangeHandler(this.manager_.GetOnChangeHandler());
if(gnInterface._setTearawayHandler){var manager=this.manager_,poppedOut=manager.GetMiniMode()=="tearaway";gnInterface._setTearawayHandler(function(){win.setTimeout(manager.ToggleTearawayMode.bind(manager),0)},poppedOut)}if(this.callAfterNotebookLoads_&&doc.location.href.startsWith(this.manager_.GetOnpageUrl())){this.callAfterNotebookLoads_();this.callAfterNotebookLoads_=null}return true};GNOTES_Mini.prototype.Resize_=function(width,height){var ifr=this.GetIframeElt(),gnInterface=this.manager_.GetInterfaceObj(ifr);
if(gnInterface&&gnInterface._resize)gnInterface._resize(width,height)};GNOTES_Mini.prototype.IframeResizeHandler=function(width,height){this.ResizeBox(width,height)};GNOTES_Mini.prototype.IframeGetCurrentPageHandler=function(){return this.MakeAddableDict_(this.Overlay_().GetAddableContent())};GNOTES_Mini.prototype.IframeMaximizeHandler=function(url){var ifr=this.GetIframeElt();if(ifr==null)return;this.Overlay_().NavigateUrl(url);var gnInterface=this.manager_.GetInterfaceObj(ifr);if(gnInterface)gnInterface._tryClose()};
GNOTES_Mini.prototype.IframeOpenHandler=function(url,way){this.Overlay_().NavigateUrl(url,way)};GNOTES_Mini.prototype.SetUpIframeListeners=function(ifr){ifr.addEventListener("DOMContentLoaded",this.IframeContentLoadCallback_.bind(this),true);ifr.addEventListener("load",this.IframeLoadCallback_,true);ifr.addEventListener("keydown",this.keyCaptureListener_,false)};GNOTES_Mini.prototype.AddEntry=function(body,title,url,opt_method,opt_query){opt_method=isDef(opt_method)?opt_method:"y";this.ShowBox();
var ifr=this.GetIframeElt(),w=ifr?ifr.contentWindow:null;if(body!=null&&body.indexOf("<br>")==0)body=body.substring(4);var gnInterface=this.manager_.GetInterfaceObj(ifr);if(gnInterface!=null&&w.document.location.href.startsWith(this.manager_.GetOnpageUrl()))w.setTimeout(function(){gnInterface._addNote(body,url,title,opt_query,opt_method)},1);else this.callAfterNotebookLoads_=this.AddEntry.apply.bind(this.AddEntry,this,arguments)};GNOTES_Mini.prototype.MakeAddableDict_=function(content){var browser=
this.Overlay_().GetCurrentBrowser();if(!browser)return{_content:content};var win=browser.contentWindow,permalink=this.manager_.GetPermalink(win),url=permalink[0],title=permalink[1];return{_title:title,_url:url,_content:content}};GNOTES_Mini.prototype.PasteNewNote=function(){var dict=this.MakeAddableDict_(this.manager_.GetAddableDataFromClipboard());if(dict!=null)this.AddEntry(dict._content,dict._title,dict._url,"paste")};GNOTES_Mini.prototype.CreateNewNote=function(content,opt_method){var dict=this.MakeAddableDict_(content);
if(dict!=null)this.AddEntry(dict._content,dict._title,dict._url,opt_method)};GNOTES_Mini.prototype.CanDrop=GNOTES_Mini.VirtualFunction;GNOTES_Mini.prototype.AttachListener=function(target,type,callback,capturing){this.listeners_.push(new GNOTES_Listener(target,type,callback,capturing))};GNOTES_Mini.prototype.RemoveListeners_=function(){var listener=null;while(listener=this.listeners_.pop())listener.Remove()};GNOTES_Mini.prototype.CatchKeys_=function(event){if(Ci.nsIDOMKeyEvent.DOM_VK_ESCAPE==event.keyCode){if(this.IsVisible())this.TryClose(event)}else if(this.CAPTURED_KEYS[event.keyCode]&&
(event.metaKey||event.ctrlKey&&!event.altKey))switch(event.keyCode){case KEYCODE_V:{if(this.InNotebookMode()){this.PasteNewNote();event.preventDefault()}break}default:event.preventDefault()}};GNOTES_Mini.prototype.CanDrop=GNOTES_Mini.VirtualFunction;GNOTES_Mini.prototype.OnDragOver_=function(event){var dragDropManager=this.win_.nsDragAndDrop;if(this.CanDrop(event))dragDropManager.dragOver(event,this.dragDropCallback_)};GNOTES_Mini.prototype.OnDrop_=function(event){var dragDropManager=this.win_.nsDragAndDrop;
if(this.CanDrop(event))dragDropManager.drop(event,this.dragDropCallback_)};GNOTES_Mini.prototype.EnableDrop_=function(event,flavor,session){session.canDrop=true};GNOTES_Mini.prototype.DoDrop_=function(event,dropData){if(typeof dropData.data=="string")this.CreateNewNote(dropData.data,"dnd")};

function GNOTES_Listener(target,type,callback,capturing){this.target_=target;this.type_=type;this.callback_=callback;this.capturing_=capturing;target.addEventListener(type,callback,capturing)}GNOTES_Listener.prototype.Remove=
function(){this.target_.removeEventListener(this.type_,this.callback_,this.capturing_)};

function GNOTES_Lock(){this.locked_=false}GNOTES_Lock.prototype.TryToCall=function(f,context){if(!this.locked_){this.locked_=true;try{f.call(context)}finally{this.locked_=false}}};GNOTES_MiniYJMacAndLinux.zIndex_=20;

function GNOTES_MiniYJMacAndLinux(win,overlay){GNOTES_Mini.call(this,win,overlay);this.debugZone="miniYJMacAndLinux";this.overlayElt_=this.overlay_.GetGNotesElementById("overlay");this.contextMenu_=new GNOTES_ContextMenu(this.win_,this.overlay_,this);this.AttachListener(this.win_,"resize",this.PositionBox.bind(this),true);var contentArea=this.doc_.getElementById("content");this.AttachListener(contentArea,"dragdrop",this.OnDrop_.bind(this),true);this.AttachListener(contentArea,
"dragover",this.OnDragOver_.bind(this),true);this.AttachListener(this.win_,"unload",this.HideBox_.bind(this),false)}GNOTES_MiniYJMacAndLinux.inherits(GNOTES_Mini);var GNOTES_MiniYJ={};GNOTES_MiniYJ.GetInstance=function(win,overlay){return G_getFirefoxPlatform()==G_FirefoxPlatform.WIN?new GNOTES_MiniYJWindows(win,overlay):new GNOTES_MiniYJMacAndLinux(win,overlay)};GNOTES_MiniYJMacAndLinux.prototype.GetOverlayElt_=function(){return this.overlayElt_};GNOTES_MiniYJMacAndLinux.prototype.Destroy=function(){GNOTES_MiniYJMacAndLinux.superClass_.Destroy.call(this);
this.contextMenu_.Destroy()};GNOTES_MiniYJMacAndLinux.prototype.GetMiniElement=function(id){return this.overlay_.GetGNotesElementById(id)};GNOTES_MiniYJMacAndLinux.prototype.IsVisible=function(){return!this.GetOverlayElt_().hidden};GNOTES_MiniYJMacAndLinux.prototype.ShowBox=function(){if(!this.IsVisible()){var t=this.GetOverlayElt_();t.style.display="block";t.hidden=false;this.ReattachBox();this.PositionBox();this.overlay_.SetStatusBarText("loadingNotebook");if(!this.iframeListenersSet_){this.iframeListenersSet_=
true;this.SetUpIframeListeners(this.GetMiniElement("ifr"))}this.manager_.SetIframeUrl(this.GetIframeElt())}};GNOTES_MiniYJMacAndLinux.prototype.HideBox_=function(opt_softState){var t=this.GetOverlayElt_();t.hidden=true;t.style.display="none";this.GetMiniElement("ifr").setAttribute("src","");t.parentNode.appendChild(t);this.iframeListenersSet_=false;GNOTES_MiniYJMacAndLinux.superClass_.HideBox_.call(this,opt_softState)};GNOTES_MiniYJMacAndLinux.prototype.ReattachBox=function(){var node=this.GetOverlayElt_(),
doc=this.doc_,dummyNode=doc.createElementNS(null,"vbox");node.parentNode.insertBefore(dummyNode,node);node.parentNode.removeChild(dummyNode);this.RedrawBox()};GNOTES_MiniYJMacAndLinux.prototype.RedrawBox=function(){var node=this.GetOverlayElt_();node.style.zIndex=""+GNOTES_MiniYJMacAndLinux.zIndex_++};GNOTES_MiniYJMacAndLinux.prototype.PositionBox=function(){if(this.IsVisible()){var t=this.GetOverlayElt_(),currentBrowser=this.overlay_.GetCurrentBrowser(),doc=currentBrowser.contentDocument,bottomHeight=
this.doc_.getElementById("status-bar").boxObject.height;t.style.bottom=bottomHeight+"px";if(isDef(doc.body)){t.style.left="";t.style.right="20px"}}};GNOTES_MiniYJMacAndLinux.prototype.ResizeBox=function(width,height){var overlayStyle=this.GetOverlayElt_().style;overlayStyle.width=width+"px";overlayStyle.height=height+"px"};GNOTES_MiniYJMacAndLinux.prototype.CanDrop=function(event){var x=event.screenX,y=event.screenY,overlayElt=this.GetOverlayElt_(),box=overlayElt.boxObject;return!overlayElt.hidden&&
x>=box.screenX&&x<box.screenX+box.width&&y>=box.screenY&&y<box.screenY+box.height};var GNOTES_SIZE_MAP={innerWidth:"tearawayWidth",innerHeight:"tearawayHeight"};


// 
function GNOTES_MiniTearaway(win,overlay){
  var overlayProxy=new GNOTES_OverlayProxy(this,GNOTES_Manager.GetInstance(overlay));
  GNOTES_Mini.call(this,win,overlayProxy);
  this.debugZone="miniTearaway";
  this.notebookId_=GNOTES_DOM_ID+"mini-notebook";
  this.isMac_=G_getFirefoxPlatform()==G_FirefoxPlatform.MAC;
  this.xulFile_=GNOTES_CHROME+"content/"+(this.isMac_?"gnotes-tearaway-osx.xul":"gnotes-tearaway.xul");
  this.mini_=null;
  this.miniDoc_=null;
  this.miniWindowElt_=null;
  this.ifr_=null;
  this.resizeLock_=new GNOTES_Lock;
  this.prefs_=this.manager_.GetPrefs();
  this.horizontalPadding_=2;
  this.verticalPadding_=2;
  this.useDefaultSize_=this.prefs_.getPref(GNOTES_SIZE_MAP.innerWidth,null)==null;
  this.miniResizeListener_=this.MiniWindowResize.bind(this)
  }

GNOTES_MiniTearaway.inherits(GNOTES_Mini);
GNOTES_MiniTearaway.singleton_=null;
GNOTES_MiniTearaway.GetInstance=function(win,overlay){
  if(GNOTES_MiniTearaway.singleton_==null)
    GNOTES_MiniTearaway.singleton_=new GNOTES_MiniTearaway(win,overlay);
  return GNOTES_MiniTearaway.singleton_
  };
GNOTES_MiniTearaway.prototype.SavePosition=function(){
  if(!this.useDefaultSize_)
    for(var member in GNOTES_SIZE_MAP)
      this.prefs_.setPref(GNOTES_SIZE_MAP[member],this.mini_[member])
  };
GNOTES_MiniTearaway.prototype.MiniWindowResize=function(){
  if(this.initialSizeSet_)
    this.ResizeIframe_()
  };
GNOTES_MiniTearaway.prototype.ResizeIframe_=function(){
  this.resizeLock_.TryToCall(this.ResizeIframeHelper_,this)
  };
GNOTES_MiniTearaway.prototype.ResizeIframeHelper_=function(){
  this.Resize_(this.mini_.innerWidth-this.horizontalPadding_,this.mini_.innerHeight-this.verticalPadding_);
  this.useDefaultSize_=false;
  this.width_=this.mini_.innerWidth;
  this.height_=this.mini_.innerHeight
};
GNOTES_MiniTearaway.prototype.GetMiniElement=function(id){
  var result=null;
  if(this.miniDoc_)result=this.miniDoc_.getElementById(GNOTES_DOM_ID+id);
  return result};
GNOTES_MiniTearaway.prototype.IsVisible=function(){return this.mini_!=null};
GNOTES_MiniTearaway.prototype.ShowBox=function(){if(!this.IsVisible()){var win=
this.Window_();if(win){this.mini_=win.open(this.xulFile_,this.notebookId_,"chrome,resizable");this.Overlay_().SetStatusBarText("loadingNotebook");this.mini_.addEventListener("load",this.BoxLoadCallback_.bind(this),false)}}};GNOTES_MiniTearaway.prototype.HideBox_=function(opt_softState){if(this.IsVisible()){this.SavePosition();this.mini_.close();this.mini_=null;this.miniDoc_=null;this.miniWindowElt_=null;this.ifr_=null}GNOTES_MiniTearaway.superClass_.HideBox_.call(this,opt_softState)};GNOTES_MiniTearaway.prototype.ReattachBox=
function(){};GNOTES_MiniTearaway.prototype.PositionBox=function(){};GNOTES_MiniTearaway.prototype.ResizeBox=function(width,height){if(!this.initialSizeSet_&&!this.useDefaultSize_)this.ResizeIframe_();else if(this.mini_.windowState==this.mini_.STATE_MAXIMIZED)this.ResizeIframe_();else this.resizeLock_.TryToCall(function(){this.mini_.innerWidth=width+this.horizontalPadding_;this.mini_.innerHeight=height+this.verticalPadding_;this.PositionBox()},this);this.initialSizeSet_=true};GNOTES_MiniTearaway.prototype.ResizeBoxForFailurePage=
function(){};GNOTES_MiniTearaway.prototype.BoxLoadCallback_=function(){this.miniDoc_=this.mini_.document;this.miniWindowElt_=this.GetMiniElement("window");this.ifr_=this.GetMiniElement("ifr");if(this.isMac_&&!this.miniDoc_.getElementById("statusbar")){var bar=this.miniDoc_.createElement("statusbar");bar.id="statusbar";this.miniWindowElt_.appendChild(bar);this.verticalPadding_=2+bar.boxObject.height}this.width_=-1;this.height_=-1;this.SetUpIframeListeners(this.ifr_);this.manager_.SetIframeUrl(this.ifr_);
new GNOTES_ContextMenu(this.mini_,this.Overlay_(),this);this.miniWindowElt_.addEventListener("dragdrop",this.OnDrop_.bind(this),true);this.miniWindowElt_.addEventListener("dragover",this.OnDragOver_.bind(this),true);this.mini_.addEventListener("resize",this.miniResizeListener_,true);for(var member in GNOTES_SIZE_MAP){var value=this.prefs_.getPref(GNOTES_SIZE_MAP[member],null);if(value!=null)this.mini_[member]=value}this.mini_.addEventListener("close",this.TryClose.bind(this),true)};GNOTES_MiniTearaway.prototype.CanDrop=
function(){return true};

// 
function GNOTES_OverlayProxy(mini,manager){
  this.mini_=mini;
  this.manager_=manager;
  this.debugZone="GNOTES_OverlayProxy"
}

GNOTES_OverlayProxy.prototype.Id=function(){return-1};
GNOTES_OverlayProxy.prototype.MiniNotebook=function(){return this.mini_};
GNOTES_OverlayProxy.prototype.OpenMiniNotebook=function(){};
GNOTES_OverlayProxy.prototype.SetStatusBarText=function(msgId){var overlays=this.manager_.GetAllOverlays();for(var i in overlays)overlays[i].SetStatusBarText(msgId)};

GNOTES_OverlayProxy.prototype.GetMessage=
function(messageID){var bundle=this.mini_.GetMiniElement("locale");return bundle?bundle.getString("gnotes."+messageID):"Error"};
GNOTES_OverlayProxy.prototype.Window=function(){var overlay=this.manager_.GetCurrentOverlay();return overlay?overlay.Window():null};
GNOTES_OverlayProxy.prototype.NavigateUrl=function(url,opt_way){var overlay=this.manager_.GetCurrentOverlay();if(!overlay){this.manager_.OpenBrowserWindow();overlay=this.manager_.GetCurrentOverlay();opt_way=0}overlay.NavigateUrl(url,opt_way)};
GNOTES_OverlayProxy.prototype.GetAddableContent=function(){var overlay=this.manager_.GetCurrentOverlay();return overlay?overlay.GetAddableContent():null};
GNOTES_OverlayProxy.prototype.GetCurrentBrowser=function(){var overlay=this.manager_.GetCurrentOverlay();return overlay?overlay.GetCurrentBrowser():null};

function GNOTES_MiniYJWindows(win,overlay){
  GNOTES_Mini.call(this,win,overlay);
  this.debugZone="miniYJWindows";
  this.notebookId_=GNOTES_DOM_ID+"mini-notebook-yj"+GNOTES_MiniYJWindows.uniqueId_++;
  this.mini_=null;
  this.miniDoc_=null;
  this.miniWindowElt_=null;
  this.ifr_=null;
  this.resizeLock_=new GNOTES_Lock;
  this.prefs_=this.manager_.GetPrefs();
  this.repositionHandler_=this.PositionBox.bind(this)
  }
GNOTES_MiniYJWindows.inherits(GNOTES_Mini);
GNOTES_MiniYJWindows.uniqueId_=1;
GNOTES_MiniYJWindows.prototype.GetMiniElement=function(id){
  var result=null;
  if(this.miniDoc_)result=this.miniDoc_.getElementById(GNOTES_DOM_ID+id);
  return result
  };
GNOTES_MiniYJWindows.prototype.IsVisible=function(){return this.mini_!=null};
GNOTES_MiniYJWindows.prototype.ShowBox=function(){
  if(!this.IsVisible()){
    var win=this.Window_();
    this.mini_=win.open(GNOTES_CHROME+"content/gnotes-yj-win.xul",
        this.notebookId_,
        ["chrome","resizable=no","dependent","alwaysRaised","titlebar=no"].join(",")
        );
    this.Overlay_().SetStatusBarText("loadingNotebook");
    this.mini_.addEventListener("load",this.BoxLoadCallback_.bind(this),false)
  }
};
GNOTES_MiniYJWindows.prototype.HideBox_=function(opt_softState){
  if(this.IsVisible()){
    this.mini_.close();
    this.mini_=null;
    this.miniDoc_=null;
    this.miniWindowElt_=null;
    this.ifr_=null;
    var win=this.Window_(),topWinElement=win.document.getElementById("main-window");
    win.removeEventListener("resize",this.repositionHandler_,true);
    topWinElement.removeEventListener("DOMAttrModified",this.repositionHandler_,true)
}
GNOTES_MiniYJWindows.superClass_.HideBox_.call(this,opt_softState)};
GNOTES_MiniYJWindows.prototype.ReattachBox=function(){};
GNOTES_MiniYJWindows.prototype.PositionBox=function(){
  var win=this.Window_(),extraY=win.document.getElementById("status-bar").boxObject.height+win.outerWidth-win.innerWidth,extraX=20,candidateX=win.screenX+win.outerWidth-this.mini_.outerWidth-extraX,candidateY=win.screenY+win.outerHeight-this.mini_.outerHeight-extraY;
  if(candidateX>0&&candidateY>0){
    this.mini_.screenX=candidateX;
    this.mini_.screenY=candidateY
    }
  };
GNOTES_MiniYJWindows.prototype.ResizeBox=function(width,height){
  this.resizeLock_.TryToCall(function(){
    this.mini_.innerWidth=width;
    this.mini_.innerHeight=height;
    this.PositionBox()},this)
  };
GNOTES_MiniYJWindows.prototype.BoxLoadCallback_=function(){
  this.miniDoc_=this.mini_.document;
  this.miniWindowElt_=this.GetMiniElement("yj-window");
  this.ifr_=this.GetMiniElement("ifr");
  this.SetUpIframeListeners(this.ifr_);
  this.manager_.SetIframeUrl(this.ifr_);
  new GNOTES_ContextMenu(this.mini_,this.Overlay_(),this);
  var win=this.Window_(),topWinElement=win.document.getElementById("main-window");
  win.addEventListener("resize",this.repositionHandler_,true);
  topWinElement.addEventListener("DOMAttrModified",this.repositionHandler_,true);
  this.miniWindowElt_.addEventListener("dragdrop",this.OnDrop_.bind(this),true);
  this.miniWindowElt_.addEventListener("dragover",this.OnDragOver_.bind(this),true)
  };
GNOTES_MiniYJWindows.prototype.CanDrop=function(){return true};

function GNOTES_ResultsPage(overlay){
  this.debugZone="ResultsPage";
  this.overlay_=overlay;
  this.manager_=GNOTES_Manager.GetInstance(overlay)}GNOTES_ResultsPage.prototype.ShouldAnnotate_=function(url){if(this.manager_.IsGWSResults(url))return true;
  return false};

GNOTES_ResultsPage.prototype.AddCallback_=function(content,title,url,searchurl){this.overlay_.MiniNotebook().AddEntry(content,title,url,"gws",searchurl)};
GNOTES_ResultsPage.prototype.AnnotateSearch=function(event){
  var doc=event.browser.contentDocument,
  win=event.browser.contentWindow,context=win.wrappedJSObject||win,loc=win.document.location.href;
  if(!this.ShouldAnnotate_(loc))return;
  if(context&&isDef(context.gnb)){
    if(isDef(context.gnb._added))
      return;
    context.gnb._added=true;
    context.gnb._show=function(){};
    context.gnb._doAdd=this.AddCallback_.bind(this);
    context.gnb._open=this.overlay_.OpenMiniNotebook.bind(this.overlay_);
    if(isDef(context.gnb._hidden)){
      var spans=doc.getElementsByTagName("SPAN");
      for(var i=0;i<spans.length;++i){
        var span=spans[i];
        if(span.className=="bl")
          span.style.display="inline"
        }
      }
    }
  };
  
var G_DOMAINS_PREFS=GNOTES_PREFS_PREFIX;

function G_Domains(xhr){
  this.xhr_=xhr;
  this.debugZone="G_Domains";
  this.prefs_=new G_Preferences(G_DOMAINS_PREFS);
  this.lastFetch_=Number(this.prefs_.getPref("lastFetch",0));
  var domains=this.prefs_.getPref("domains",""),domainList=isString(domains)?domains.split(","):[];
  this.suffixes_=G_Domains.DEFAULT_SUFFIXES_;
  if(domainList.length>0)
    for(var i=0;i<domainList.length;i++)
      this.suffixes_[domainList[i]]=true;
  else this.Refresh()
  }

G_Domains.DEFAULT_SUFFIXES_={com:true};

G_Domains.RESPONSE_START_=".google.";
G_Domains.DOMAIN_REGEXP_=new RegExp(/^https?:\/\/(www\.)?google\.([^\/]+)\//);
G_Domains.SUBDOMAIN_REGEXP_=new RegExp(/^https?:\/\/([a-zA-Z\.]+)\.google\.([^\/]+)\//);
G_Domains.SUFFIX_PART_=2;
G_Domains.PREFIX_PART_=1;
G_Domains.DOMAIN_LIST_="http://www.google.com/supported_domains";
G_Domains.prototype.RequestDomains_=function(){
  if(!this.hasOutstandingRequest_){
    this.hasOutstandingRequest_=true;
    this.lastFetch_=Date.now();
    this.xhr_.onreadystatechange=this.ReceiveDomains_.bind(this);
    this.xhr_.open("GET",G_Domains.DOMAIN_LIST_,true);
    this.xhr_.send(null)
  }
};

G_Domains.prototype.ReceiveDomains_=function(){
  if(this.xhr_.readyState==4){
    this.hasOutstandingRequest_=false;
    var status=this.xhr_.status;
    if(status==200){
      var response=this.xhr_.responseText;
      if(isString(response)&&response.length){
        var list=response.split("\n");
        this.suffixes_=G_Domains.DEFAULT_SUFFIXES_;
        var suffixList=[],length=G_Domains.RESPONSE_START_.length;
        for(var i=0;i<list.length;i++){
          var domain=list[i];
          if(domain.startsWith(G_Domains.RESPONSE_START_)&&length<domain.length){
            suffixList.push(domain.substring(length));
            this.suffixes_[suffixList.peek()]=true
          }
        }
        this.lastFetch_=Date.now();
        this.prefs_.setPref("lastFetch",String(this.lastFetch_));
        this.prefs_.setPref("domains",suffixList.join(","))
        }
      }
    }
  };

G_Domains.prototype.IsDomain=function(url){
  this.RefreshIfNeeded();
  var matches=G_Domains.DOMAIN_REGEXP_.exec(url);
  if(!matches)
    return false;
  else{
    var suffix=matches[G_Domains.SUFFIX_PART_];
    return!(!this.suffixes_[suffix])
    }
  };
G_Domains.prototype.IsSubDomain=function(url,subdomain){
  var match=this.GetGoogleSubDomain(url);
  return match?match==subdomain:false
  };
G_Domains.prototype.GetGoogleSubDomain=function(url){this.RefreshIfNeeded();var matches=G_Domains.SUBDOMAIN_REGEXP_.exec(url);if(!matches)return null;else{var suffix=matches[G_Domains.SUFFIX_PART_];return this.suffixes_[suffix]?matches[G_Domains.PREFIX_PART_]||null:null}};
G_Domains.prototype.StripDomain=function(url){var startIndex=8,index=url.indexOf("/",startIndex);return index==-1||index+1>=url.length?"":url.substring(index+
1)};
G_Domains.prototype.Refresh=function(){this.RequestDomains_()};
G_Domains.prototype.RefreshIfNeeded=function(){
  if(this.lastFetch_+604800000<Date.now())
    this.Refresh()
  };

function GNOTES_BrowserOverlay(win){
  this.win_=win;
  this.doc_=this.win_.document;
  this.debugZone="gnotes";
  this.mini_=null;
  this.id_=GNOTES_BrowserOverlay.id_++;
  this.needsContinuousRedraws_=G_getFirefoxPlatform()==G_FirefoxPlatform.MAC&&(new G_ThisFirefoxVersion).compareToString("3.0b1")
  }
  
GNOTES_BrowserOverlay.id_=1;

GNOTES_BrowserOverlay.prototype.Window=function(){return this.win_};

GNOTES_BrowserOverlay.prototype.Id=function(){return this.id_};
GNOTES_BrowserOverlay.prototype.GetGNotesElementById=function(id){return this.doc_.getElementById(GNOTES_DOM_ID+id)};
GNOTES_BrowserOverlay.prototype.SetGNotesElementId=function(element,id){element.id=GNOTES_DOM_ID+id};
GNOTES_BrowserOverlay.prototype.MiniNotebook=function(){return this.mini_};
GNOTES_BrowserOverlay.prototype.GetCurrentBrowser=function(){return this.watcher_.getCurrentBrowser()};
GNOTES_BrowserOverlay.prototype.GetMessage=function(messageID){return this.messages_.getString("gnotes."+messageID)};
GNOTES_BrowserOverlay.prototype.SetStatusBarText=function(messageID){this.statusBarManager_.SetStatusBarText(messageID)};
GNOTES_BrowserOverlay.prototype.MakeImageTag_=function(img){return"<img src='"+img.src+"'/>"};
GNOTES_BrowserOverlay.prototype.NavigateUrl=function(url,opt_way){
  if(!isDef(opt_way)||opt_way==null)
    opt_way=this.openNewWindow_==3?3:1;
    if(opt_way==0){
      var currentWindow=this.watcher_.getCurrentBrowser().contentWindow;
      currentWindow.document.location.href=url
    }
    else if(opt_way==1)
      this.win_.open(url);
    else if(opt_way==2)
      this.watcher_.getTabBrowser().addTab(url);
    else if(opt_way==3){
      var tabBrowser=this.watcher_.getTabBrowser();
      tabBrowser.selectedTab=tabBrowser.addTab(url)
    }
  return false
  };
GNOTES_BrowserOverlay.prototype.StatusCallback=function(event){
  if(event.button!=0)return true;var mini=this.MiniNotebook();
  if(!mini.IsVisible())this.OpenMiniNotebook();
  else if(this.manager_.GetMiniMode()=="yj")mini.TryClose(event);
  else this.manager_.ToggleTearawayMode()};
GNOTES_BrowserOverlay.prototype.NotifySelection=function(newState){
  if(this.MiniNotebook().IsVisible()){
    var ifr=this.MiniNotebook().GetIframeElt(),gnInterface=this.manager_.GetInterfaceObj(ifr);
    if(gnInterface!=null)gnInterface._selectionChanged(newState)
    }
  };
GNOTES_BrowserOverlay.prototype.GetAddableContent=function(){
  var util=this.manager_,selection=null,win=this.doc_.commandDispatcher.focusedWindow;
  if(win)selection=GNOTES_Manager.SerializeRange(win.document,GNOTES_Manager.GetValidSelectionRange(win));
  var contextMenu=this.win_.gContextMenu;
  if(contextMenu!=null){
    var target=contextMenu.target;
    if(target&&(!selection||!util.SelectionContainsNode(win,target)))
      if(contextMenu.onImage)
        selection=this.MakeImageTag_(contextMenu.target);
      else if(contextMenu.onLink)
        selection="<a href="+contextMenu.linkURL+">"+contextMenu.linkText()+"</a>"
  }
  if(selection==null){
    var frames=[this.GetCurrentBrowser()];
    for(var current=0;current<frames.length;current++){
      var win=frames[current].contentWindow,doc=frames[current].contentDocument;
      if(win&&doc){
        selection=GNOTES_Manager.SerializeRange(doc,GNOTES_Manager.GetValidSelectionRange(win));
        if(selection!=null)break;
        var moreFrames=doc.getElementsByTagName("frame");
        for(var i=0;i<moreFrames.length;i++)
          frames.push(moreFrames[i]);
          var moreIFrames=doc.getElementsByTagName("iframe");
          for(var i=0;i<moreIFrames.length;i++)
            frames.push(moreIFrames[i])
      }
    }
  }
  return selection
};
GNOTES_BrowserOverlay.prototype.NotifyGoogle_=function(win){var url=win.location.href;if(this.manager_.IsInterestingSite(url)){win=win.wrappedJSObject||win;win.hasGnExt=true}};

GNOTES_BrowserOverlay.prototype.ContextCallback=function(){
  this.MiniNotebook().CreateNewNote(this.GetAddableContent(),"ctxt")
};

GNOTES_BrowserOverlay.prototype.TogglePlus=
function(){
  var newBool=this.statusBarManager_.IsEnablePlusItemChecked();
  this.manager_.SetAddButtonEnabled(newBool)
};

GNOTES_BrowserOverlay.prototype.SetShowAddButton=function(value){this.addButton_.SetShowButton(value)};
GNOTES_BrowserOverlay.prototype.ToggleStatusText=function(){var newBool=this.statusBarManager_.IsHideTextItemChecked();this.manager_.SetStatusTextHidden(newBool)};
GNOTES_BrowserOverlay.prototype.SetStatusTextHidden=function(value){this.statusBarManager_.SetStatusTextHidden(value)};
GNOTES_BrowserOverlay.prototype.CreateMiniNotebook=function(){if(this.mini_)this.mini_.Destroy();this.mini_=this.manager_.GetMiniMode()=="yj"?GNOTES_MiniYJ.GetInstance(this.win_,this):GNOTES_MiniTearaway.GetInstance(this.win_,this)};
GNOTES_BrowserOverlay.prototype.OpenMiniNotebook=function(){if(!this.MiniNotebook().IsVisible())this.manager_.ResetIframeCache();this.MiniNotebook().ShowBox()};
GNOTES_BrowserOverlay.prototype.OnPageLoad_=function(event){var win=event.browser.contentWindow;this.resultsPage_.AnnotateSearch(event);
this.NotifyGoogle_(win);var mini=this.MiniNotebook(),isVisible=mini.IsVisible();if(isVisible){if(event.isTop&&event.inSelected)this.NotifySelection(GNOTES_Manager.GetValidSelectionRange(win)!=null);if(this.needsContinuousRedraws_)mini.ReattachBox()}};
GNOTES_BrowserOverlay.prototype.OnTabSwitch_=function(){var mini=this.MiniNotebook();if(mini.IsVisible())mini.ReattachBox()};
GNOTES_BrowserOverlay.prototype.OnPageScroll_=function(){var mini=this.MiniNotebook();if(mini.IsVisible())mini.RedrawBox()};
GNOTES_BrowserOverlay.prototype.Init=
function(){this.messages_=this.GetGNotesElementById("locale");this.manager_=GNOTES_Manager.GetInstance(this);this.statusBarManager_=this.CreateStatusbarManager_();this.contentArea_=this.doc_.getElementById("content");this.watcher_=new G_TabbedBrowserWatcher(this.contentArea_,GNOTES_DOM_ID);this.CreateMiniNotebook();this.watcher_.registerListener("tabswitch",this.OnTabSwitch_.bind(this));this.watcher_.registerListener("load",this.OnPageLoad_.bind(this));if(this.needsContinuousRedraws_)this.contentArea_.addEventListener("scroll",
this.OnPageScroll_.bind(this),false);this.addButton_=new GNOTES_AddButton(this,"fullpage");this.contentArea_.addEventListener("mousedown",this.addButton_.OnMouseDown.bind(this.addButton_),true);this.contentArea_.addEventListener("mouseup",this.addButton_.OnMouseUp.bind(this.addButton_),false);this.contentArea_.addEventListener("click",this.addButton_.OnMouseClick.bind(this.addButton_),false);this.resultsPage_=new GNOTES_ResultsPage(this);var addButtonEnabled=this.manager_.IsAddButtonEnabled(),statusTextHidden=
this.manager_.IsStatusTextHidden();this.SetShowAddButton(addButtonEnabled);this.SetStatusTextHidden(statusTextHidden);this.statusBarManager_.SetEnablePlusItemChecked(addButtonEnabled);this.statusBarManager_.SetHideTextItemChecked(statusTextHidden);this.tabPrefs_=new G_Preferences("browser.link.",false,true);this.openNewWindow_=this.tabPrefs_.getPref("open_newwindow");var mini=this.MiniNotebook();if(!mini.IsVisible())this.SetStatusBarText("openNotebook");else this.SetStatusBarText(this.manager_.GetCloseMessageId())};
GNOTES_BrowserOverlay.prototype.CreateStatusbarManager_=function(){throw new Error("This is a virtual function that must be overriden by extension specific code.");};

function GNOTES_ManagerCustom(overlay){GNOTES_Manager.call(this,overlay);this.clientParam_="client=gnotesff&v=1.0.0.20"}
GNOTES_ManagerCustom.inherits(GNOTES_Manager);GNOTES_ManagerCustom.prototype.SetIframeUrl=function(ifr){ifr.setAttribute("src",this.GetOnpageUrl()+"?"+this.clientParam_+(this.cacheBreaker_?"&zx="+this.cacheBreaker_:"")+(this.hashParam_||""))};GNOTES_Manager.CreateInstance_=function(overlay){if(!GNOTES_Manager.updater_){GNOTES_Manager.updater_=new G_ExtensionUpdater(GNOTES_RDF_ID);
GNOTES_Manager.updater_.UpdatePeriodically(false)}var rdf_prefs=new G_Preferences("extensions."+GNOTES_RDF_ID+".",false,false);rdf_prefs.setPref("name",overlay.GetMessage("name"));rdf_prefs.setPref("description",overlay.GetMessage("description"));return new GNOTES_ManagerCustom(overlay)};

function GNOTES_StatusBar(overlay){this.debugZone="GNOTES_StatusBar";this.overlay_=overlay;this.statusElt_=overlay.GetGNotesElementById("statusbar-lbl");this.statusMessageId_=null;this.hideStatusBarText_=false}
GNOTES_StatusBar.hidableMessages_={openNotebook:true,closeNotebook:true,popinNotebook:true};
GNOTES_StatusBar.prototype.SetStatusBarText=function(messageID){this.statusMessageId_=messageID;this.statusElt_.label=this.hideStatusBarText_&&GNOTES_StatusBar.hidableMessages_[messageID]?"":this.overlay_.GetMessage(messageID)};
GNOTES_StatusBar.prototype.SetStatusTextHidden=function(value){
  this.hideStatusBarText_=value;
  this.RefreshStatusBarText_()};
GNOTES_StatusBar.prototype.SetEnablePlusItemChecked=function(value){this.overlay_.GetGNotesElementById("enable-plus").setAttribute("checked",value?"true":"false")};GNOTES_StatusBar.prototype.IsEnablePlusItemChecked=function(){var attr=this.overlay_.GetGNotesElementById("enable-plus").getAttribute("checked");return attr=="true"};GNOTES_StatusBar.prototype.SetHideTextItemChecked=
function(value){this.overlay_.GetGNotesElementById("hide-text").setAttribute("checked",value?"true":"false")};GNOTES_StatusBar.prototype.IsHideTextItemChecked=function(){var attr=this.overlay_.GetGNotesElementById("hide-text").getAttribute("checked");return attr=="true"};GNOTES_StatusBar.prototype.RefreshStatusBarText_=function(){if(this.statusMessageId_)this.SetStatusBarText(this.statusMessageId_)};function GNOTES_BrowserOverlayCustom(win){GNOTES_BrowserOverlay.call(this,win)}GNOTES_BrowserOverlayCustom.inherits(GNOTES_BrowserOverlay);GNOTES_BrowserOverlayCustom.prototype.Init=function(){GNOTES_BrowserOverlayCustom.superClass_.Init.call(this);var prefs=this.manager_.GetPrefs(),introVersion=prefs.getPref("introversion",null);if(introVersion==null||introVersion==""||introVersion<1){var watcher=this.watcher_,self=this,onshow=function(){watcher.removeListener(G_TabbedBrowserWatcher.events.PAGESHOW,
onshow);prefs.setPref("introversion","1");self.NavigateUrl(GNOTES_SERVER+"welcome/ff",3)};watcher.registerListener(G_TabbedBrowserWatcher.events.PAGESHOW,onshow)}};GNOTES_BrowserOverlayCustom.prototype.CreateStatusbarManager_=function(){return new GNOTES_StatusBar(this)};
