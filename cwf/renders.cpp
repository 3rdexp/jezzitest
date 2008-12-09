#include <google/template_dictionary.h>
#include "cwf/render.hpp"

// google::TemplateDictionary * dict);
namespace cwf {

RenderResult NullRender(const HostInfo *, const RequestInfo *
    , const http::HttpRequest *, http::HttpResponse *response, ResponsePipe *) {
  response->set_success();
  return RR_SUCCEEDED;
}

RenderResult ServerErrorRender(const HostInfo *, const RequestInfo *
    , const http::HttpRequest *, http::HttpResponse *response, ResponsePipe *) {
  response->set_error(http::HC_SERVICE_UNAVAILABLE);
  return RR_SUCCEEDED;
}

RenderResult StaticRender(const HostInfo *, const RequestInfo *
    , const http::HttpRequest *request, http::HttpResponse *response, ResponsePipe *) {
  // load file, doc_root?
  // gzip
  std::string accepted = request->hasHeader(http::HH_ACCEPT_ENCODING);
  return RR_SUCCEEDED;
}

}
