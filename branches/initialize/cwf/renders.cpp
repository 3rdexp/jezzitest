#include <google/template_dictionary.h>
#include "cwf/render.hpp"

// google::TemplateDictionary * dict);
namespace cwf {

bool NullRender(const HostInfo *, const RequestInfo *
    , const http::HttpRequest *, http::HttpResponse *response) {
  response->set_success();
  return true;
}

bool ServerErrorRender(const HostInfo *, const RequestInfo *
    , const http::HttpRequest *, http::HttpResponse *response) {
  response->set_error(http::HC_SERVICE_UNAVAILABLE);
  return true;
}

bool StaticRender(const HostInfo *, const RequestInfo *
    , const http::HttpRequest *, http::HttpResponse *response) {
  // load file, doc_root?
  // gzip
  return true;
}

}
