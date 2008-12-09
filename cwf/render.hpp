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

  // const std::string & config(const std::string &) const;

  const std::string & name() const;
  const std::string & document_root() const;
};

class RequestInfo {
public:
  // ip, ...
};

enum RenderResult {
  RR_FAILED, 
  RR_SUCCEEDED,
  RR_ASYNC, 
};

class ResponsePipe {
public:
  virtual bool Write(const char *buf, std::size_t len) = 0; // error detail
  virtual bool Close() = 0; // need ?
};

typedef bool(*RenderFunction)(const HostInfo *, const RequestInfo *
    , const http::HttpRequest *, http::HttpResponse *, ResponsePipe *);

RenderResult NullRender(const HostInfo *, const RequestInfo *
    , const http::HttpRequest *, http::HttpResponse *, ResponsePipe *);
RenderResult ServerErrorRender(const HostInfo *, const RequestInfo *
    , const http::HttpRequest *, http::HttpResponse *, ResponsePipe *);
RenderResult StaticRender(const HostInfo *, const RequestInfo *
    , const http::HttpRequest *, http::HttpResponse *, ResponsePipe *);

RenderResult TemplateRender(const HostInfo *, const RequestInfo *
    , const http::HttpRequest *, http::HttpResponse *, ResponsePipe *);

}
#endif // CWF_RENDER_HPP_
