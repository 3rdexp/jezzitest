// Copyright (C) 2006 and onwards Google, Inc.

const Cc = Components.classes;
const Ci = Components.interfaces;

const GNOTES_COMPILED_JS = "gnotes.js";

try {
  loadScript(GNOTES_COMPILED_JS);
  compiledJsLoaded = true;
} catch (e) {
  throw new Error("Google Notebook install failed: " + e.message);
}

var GNOTES_module = new G_JSModule();

const GNOTES_MODULE_ID = "{f9705dcb-9a6d-4de8-b4f6-01d74c20e838}";
const GNOTES_MODULE_NAME = "@google.com/gnotes/app-context;1";
const GNOTES_MODULE_OBJECT = "GNOTES_AppContext";

GNOTES_module.registerObject(GNOTES_MODULE_ID,
                             GNOTES_MODULE_NAME, 
                             GNOTES_MODULE_OBJECT,
                             {wrappedJSObject:this});

function NSGetModule() {
  return GNOTES_module;
}

function loadScript(libPath) {
   Cc["@mozilla.org/moz/jssubscript-loader;1"]
     .getService(Ci.mozIJSSubScriptLoader)
     .loadSubScript(getLibUrl(libPath));
}

function getLibUrl(path) {
  var file = __LOCATION__.clone().parent.parent;
  var parts = path.split("/");

  file.append("lib");

  for (var i = 0, part; part = parts[i]; i++) {
    file.append(part);
  }

  if (!file.exists()) {
    throw new Error("Specified library {" + file.path + "} does not exist");
  }

  return Cc["@mozilla.org/network/protocol;1?name=file"]
           .getService(Ci.nsIFileProtocolHandler)
           .getURLSpecFromFile(file);
}
