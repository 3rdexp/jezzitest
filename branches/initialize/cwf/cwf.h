#ifndef __CWF_CWF_H__
#define __CWF_CWF_H__

// C++ Web (Devlopment) Framework
//////////////////////////////////////////////////////////////////////////
// 1 nginx + fastcgi devkit => samples
// 2 fcgi host, test => fcgi client
// 3 
namespace cwf {

class Server {
public:
    virtual ~Server() {}

    /// Wait fastcgi connection
    /// Generate Request
    /// url map ===>  Handle
    /// Handle.Render(Request)
    void Run();

    /// Templates
};

class Connection {
public:
};

class Request {
public:    
    // header, body, address
};

class Handle {
public:
    virtual ~Handle() {}
    virtual bool Render(Request &);
};

class Data {};

class TemplateHandle {
};

}

#endif __CWF_CWF_H__
