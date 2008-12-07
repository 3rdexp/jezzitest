#ifndef CWF_RENDER_HPP_
#define CWF_RENDER_HPP_

#include "cwf/http.hpp"

namespace cwf {

class HostInfo {
public:
  // server name
  // address
  // docucment root
};
class RequestInfo {
public:
  // ip, ...
};

typedef bool(*RenderFunction)(const http::HttpRequest *request, http::HttpResponse *response);

bool NullRender(const http::HttpRequest *request, http::HttpResponse *response);
bool ServerErrorRender(const http::HttpRequest *request, http::HttpResponse *response);
bool StaticRender(const http::HttpRequest *request, http::HttpResponse *response);

}
#endif // CWF_RENDER_HPP_
