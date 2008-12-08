#ifndef CWF_RENDER_HPP_
#define CWF_RENDER_HPP_

#include "cwf/http.hpp"

namespace cwf {

class HostInfo {
public:
  // server name
  // address
  // document root
  // config
};

class RequestInfo {
public:
  // ip, ...
};

typedef bool(*RenderFunction)(const HostInfo *, const RequestInfo *
    , const http::HttpRequest *, http::HttpResponse *);

bool NullRender(const HostInfo *, const RequestInfo *
    , const http::HttpRequest *, http::HttpResponse *);
bool ServerErrorRender(const HostInfo *, const RequestInfo *
    , const http::HttpRequest *, http::HttpResponse *);
bool StaticRender(const HostInfo *, const RequestInfo *
    , const http::HttpRequest *, http::HttpResponse *);

}
#endif // CWF_RENDER_HPP_
