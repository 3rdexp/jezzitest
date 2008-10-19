

namespace cwf {

class Gateway {
    Run() {
        // Wait fastcgi connection
        // Generate Request
        // url map ===>  Handle
        // Handle.Render(Request)
    }
};

class Connection {};

class Request {
public:    
    // header, body, address
};

class Handle {
    virtual bool Render(Request &);
};

class Data {};

class TemplateHandle {
};

}
