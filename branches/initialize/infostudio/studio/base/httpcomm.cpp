
#include <sstream>
#include <vector>

#include "httpcomm.h"

BEGIN_ENGINE_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Enum - TODO: expose globally later?
//////////////////////////////////////////////////////////////////////

bool find_string(size_t& index, const std::string& needle,
                 const char* const haystack[], size_t max_index) {
  for (index=0; index<max_index; ++index) {
	if (_stricmp(needle.c_str(), haystack[index]) == 0) {
	  return true;
	}
  }
  return false;
}

template<class E>
struct Enum {
  static const char** Names;
  static size_t Size;

  static inline const char* Name(E val) { return Names[val]; }
  static inline bool Parse(E& val, const std::string& name) { 
	size_t index;
	if (!find_string(index, name, Names, Size))
	  return false;
	val = static_cast<E>(index);
	return true;
  }

  E val;

  inline operator E&() { return val; }
  inline Enum& operator=(E rhs) { val = rhs; return *this; }

  inline const char* name() const { return Name(val); }
  inline bool assign(const std::string& name) { return Parse(val, name); }
  inline Enum& operator=(const std::string& rhs) { assign(rhs); return *this; }
};

#define ENUM(e,n) \
  template<> const char** Enum<e>::Names = n; \
  template<> size_t Enum<e>::Size = sizeof(n)/sizeof(n[0])


//////////////////////////////////////////////////////////////////////
// HttpCommon
//////////////////////////////////////////////////////////////////////

static const char* kHttpVersions[HVER_LAST+1] = {
    "1.0", "1.1"
};
ENUM(HttpVersion, kHttpVersions);

static const char* kHttpVerbs[HV_LAST+1] = {
    "GET", "POST", "PUT", "DELETE", "CONNECT", "HEAD"
};
ENUM(HttpVerb, kHttpVerbs);

static const char* kHttpHeaders[HH_LAST+1] = {
    "Age",
        "Cache-Control",
        "Connection",
        "Content-Length",
        "Content-Range",
        "Content-Type",
        "Cookie",
        "Date",
        "ETag",
        "Expires",
        "Host",
        "If-Modified-Since",
        "If-None-Match",
        "Keep-Alive",
        "Last-Modified",
        "Location",
        "Proxy-Authenticate",
        "Proxy-Authorization",
        "Proxy-Connection",
        "Range",
        "Set-Cookie",
        "TE",
        "Trailers",
        "Transfer-Encoding",
        "Upgrade",
        "User-Agent",
        "WWW-Authenticate",
};
ENUM(HttpHeader, kHttpHeaders);

const char* ToString(HttpVersion version) {
    return Enum<HttpVersion>::Name(version);
}

bool FromString(HttpVersion& version, const std::string& str) {
    return Enum<HttpVersion>::Parse(version, str);
}

const char* ToString(HttpVerb verb) {
    return Enum<HttpVerb>::Name(verb);
}

bool FromString(HttpVerb& verb, const std::string& str) {
    return Enum<HttpVerb>::Parse(verb, str);
}

const char* ToString(HttpHeader header) {
    return Enum<HttpHeader>::Name(header);
}

bool FromString(HttpHeader& header, const std::string& str) {
    return Enum<HttpHeader>::Parse(header, str);
}



bool HttpCodeHasBody(uint32 code) {
    return !HttpCodeIsInformational(code)
        && (code != HC_NO_CONTENT) || (code != HC_NOT_MODIFIED);
}

bool HttpCodeIsCacheable(uint32 code) {
    switch (code) {
  case HC_OK:
  case HC_NON_AUTHORITATIVE:
  case HC_PARTIAL_CONTENT:
  case HC_MULTIPLE_CHOICES:
  case HC_MOVED_PERMANENTLY:
  case HC_GONE:
      return true;
  default:
      return false;
    }
}

bool HttpHeaderIsEndToEnd(HttpHeader header) {
    switch (header) {
  case HH_CONNECTION:
  case HH_KEEP_ALIVE:
  case HH_PROXY_AUTHENTICATE:
  case HH_PROXY_AUTHORIZATION:
      //case HH_PROXY_CONNECTION:??
  case HH_TE:
  case HH_TRAILERS:
  case HH_TRANSFER_ENCODING:
  case HH_UPGRADE:
      return false;
  default:
      return true;
    }
}

bool HttpHeaderIsCollapsible(HttpHeader header) {
    switch (header) {
  case HH_SET_COOKIE:
  case HH_PROXY_AUTHENTICATE:
  case HH_WWW_AUTHENTICATE:
      return false;
  default:
      return true;
    }
}

//bool HttpShouldKeepAlive(const HttpData& data) {
//    std::string connection;
//    if ((data.hasHeader(HH_PROXY_CONNECTION, &connection)
//        || data.hasHeader(HH_CONNECTION, &connection))) {
//            return (_stricmp(connection.c_str(), "Keep-Alive") == 0);
//        }
//        return (data.version >= HVER_1_1);
//}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void RequestMap::addParam(const std::string& name, const std::string& value
              , bool overwrite)
{
    if (overwrite)
        erase(name);
    insert(value_type(name, value));
}

void RequestMap::clear(const std::string& name)
{
    if (name.empty())
        std::multimap<std::string, std::string, iless>::clear();
    else
        erase(name);
}

bool RequestMap::hasParam(const std::string& name, std::string* value) const
{
    const_iterator it = find(name);
    if (it == end())
        return false;
    else if (value)
        *value = it->second;
    return true;
}

#include "escape.h"
#include "unicode.h"

std::string RequestMap::build(CHARSET charset) const
{
    UrlQueryEscape uqe;

    std::ostringstream hs;
    for (const_iterator i = begin();
        i != end(); ++i)
    {
        if (charset == CHARSET_UTF8)
        {   
            hs << uqe(string2utf8(i->first)) << "=" << uqe(string2utf8(i->second))
                << "&";
        }
        else
            hs << uqe(i->first) << "=" << uqe(i->second)
                << "&";
    }

    // remove last &
    std::string ret = hs.str();
    if (!ret.empty())
        ret.resize(ret.size() - 1);

    return ret;
}

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "unescape.h"

bool RequestMap::parse(const std::string & s)
{
    using namespace std;

    UrlQueryUnEscape une;

    vector<string> vec;
    boost::split(vec, s, boost::is_any_of("&"));
    if (!vec.empty())
    {
        for (vector<string>::const_iterator i = vec.begin(); i != vec.end(); ++i)
        {
            vector<string> v2;
            boost::split(v2, *i, boost::is_any_of("="));
            if (v2.size() == 2)
            {
                addParam(une(v2[0]), une(v2[1]));
            }
        }
    }
    return true;
}

END_ENGINE_NAMESPACE
