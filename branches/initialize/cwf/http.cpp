#include <time.h> // mktime, timezone
#include <stdarg.h> // va_end
#include <functional>
#include <algorithm>
#include "cwf/http.hpp"

#ifndef ASSERT
  #define ASSERT(x) if (!(x)) __asm int 3;
#endif

#ifndef ARRAY_SIZE
  #define ARRAY_SIZE(x) (static_cast<int>((sizeof(x)/sizeof(x[0]))))
#endif

namespace cwf { namespace http {

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


static const char* kHttpVersions[HVER_LAST+1] = {
  "1.0", "1.1"
};
ENUM(HttpVersion, kHttpVersions);

static const char* kHttpVerbs[HV_LAST+1] = {
  "GET", "POST", "PUT", "DELETE", "CONNECT", "HEAD"
};
ENUM(HttpVerb, kHttpVerbs);

static const char* kHttpHeaders[HH_LAST+1] = {
  "Accept",
  "Accept-Charset",
  "Accept-Encoding",
  "Accept-Language",
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
  "Server",
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
  case HC_UNAUTHORIZED:
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

bool HttpShouldKeepAlive(const HttpData& data) {
//   std::string connection;
//   if ((data.hasHeader(HH_PROXY_CONNECTION, &connection)
//     || data.hasHeader(HH_CONNECTION, &connection))) {
//       return (_stricmp(connection.c_str(), "Keep-Alive") == 0);
//   }
  return (data.version >= HVER_1_1);
}

namespace {

inline bool IsEndOfAttributeName(size_t pos, size_t len, const char * data) {
  if (pos >= len)
    return true;
  if (isspace(static_cast<unsigned char>(data[pos])))
    return true;
  // The reason for this complexity is that some attributes may contain trailing
  // equal signs (like base64 tokens in Negotiate auth headers)
  if ((pos+1 < len) && (data[pos] == '=') &&
    !isspace(static_cast<unsigned char>(data[pos+1])) &&
    (data[pos+1] != '=')) {
      return true;
  }
  return false;
}

}  // anonymous namespace

void HttpParseAttributes(const char * data, size_t len, 
                         HttpAttributeList& attributes) {
   size_t pos = 0;
   while (true) {
     // Skip leading whitespace
     while ((pos < len) && isspace(static_cast<unsigned char>(data[pos]))) {
       ++pos;
     }

     // End of attributes?
     if (pos >= len)
       return;

     // Find end of attribute name
     size_t start = pos;
     while (!IsEndOfAttributeName(pos, len, data)) {
       ++pos;
     }

     HttpAttribute attribute;
     attribute.first.assign(data + start, data + pos);

     // Attribute has value?
     if ((pos < len) && (data[pos] == '=')) {
       ++pos; // Skip '='
       // Check if quoted value
       if ((pos < len) && (data[pos] == '"')) {
         while (++pos < len) {
           if (data[pos] == '"') {
             ++pos;
             break;
           }
           if ((data[pos] == '\\') && (pos + 1 < len))
             ++pos;
           attribute.second.append(1, data[pos]);
         }
       } else {
         while ((pos < len) &&
           !isspace(static_cast<unsigned char>(data[pos])) &&
           (data[pos] != ',')) {
             attribute.second.append(1, data[pos++]);
         }
       }
     }

     attributes.push_back(attribute);
     if ((pos < len) && (data[pos] == ',')) ++pos; // Skip ','
   }
}

bool HttpHasAttribute(const HttpAttributeList& attributes,
                      const std::string& name,
                      std::string* value) {
  for (HttpAttributeList::const_iterator it = attributes.begin();
    it != attributes.end(); ++it) {
      if (it->first == name) {
        if (value) {
          *value = it->second;
        }
        return true;
      }
  }
  return false;
}

bool HttpHasNthAttribute(HttpAttributeList& attributes,
                         size_t index, 
                         std::string* name,
                         std::string* value) {
  if (index >= attributes.size())
    return false;

  if (name)
    *name = attributes[index].first;
  if (value)
    *value = attributes[index].second;
  return true;
}

bool HttpDateToSeconds(const std::string& date, unsigned long* seconds) {
  const char* const kTimeZones[] = {
    "UT", "GMT", "EST", "EDT", "CST", "CDT", "MST", "MDT", "PST", "PDT",
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "K", "L", "M",
    "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y"
  };
  const int kTimeZoneOffsets[] = {
    0,  0, -5, -4, -6, -5, -7, -6, -8, -7,
    -1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -11, -12,
    1,  2,  3,  4,  5,  6,  7,  8,  9,  10,  11,  12
  };

  ASSERT(NULL != seconds);
  struct tm tval;
  memset(&tval, 0, sizeof(tval));
  char month[4], zone[6];
  memset(month, 0, sizeof(month));
  memset(zone, 0, sizeof(zone));

  if (7 != sscanf(date.c_str(), "%*3s, %d %3s %d %d:%d:%d %5c",
    &tval.tm_mday, month, &tval.tm_year,
    &tval.tm_hour, &tval.tm_min, &tval.tm_sec, &zone)) {
      return false;
  }
  switch (toupper(month[2])) {
  case 'N': tval.tm_mon = (month[1] == 'A') ? 0 : 5; break;
  case 'B': tval.tm_mon = 1; break;
  case 'R': tval.tm_mon = (month[0] == 'M') ? 2 : 3; break;
  case 'Y': tval.tm_mon = 4; break;
  case 'L': tval.tm_mon = 6; break;
  case 'G': tval.tm_mon = 7; break;
  case 'P': tval.tm_mon = 8; break;
  case 'T': tval.tm_mon = 9; break;
  case 'V': tval.tm_mon = 10; break;
  case 'C': tval.tm_mon = 11; break;
  }
  tval.tm_year -= 1900;
  unsigned long gmt, non_gmt = mktime(&tval);
  if ((zone[0] == '+') || (zone[0] == '-')) {
    if (!isdigit(zone[1]) || !isdigit(zone[2])
      || !isdigit(zone[3]) || !isdigit(zone[4])) {
        return false;
    }
    int hours = (zone[1] - '0') * 10 + (zone[2] - '0');
    int minutes = (zone[3] - '0') * 10 + (zone[4] - '0');
    int offset = (hours * 60 + minutes) * 60;
    gmt = non_gmt + (zone[0] == '+') ? offset : -offset;
  } else {
    size_t zindex;
    if (!find_string(zindex, zone, kTimeZones, ARRAY_SIZE(kTimeZones))) {
      return false;
    }
    gmt = non_gmt + kTimeZoneOffsets[zindex] * 60 * 60;
  }
#ifdef OSX
  tm *tm_for_timezone = localtime((time_t *)&gmt);
  *seconds = gmt + tm_for_timezone->tm_gmtoff;
#else
  *seconds = gmt - timezone;
#endif
  return true;
}

//////////////////////////////////////////////////////////////////////
// HttpData
//////////////////////////////////////////////////////////////////////

#if 0
void
HttpData::clear() {
  m_headers.clear();
}

void
HttpData::changeHeader(const std::string& name, const std::string& value,
                       HeaderCombine combine) {
  if (combine == HC_AUTO) {
    HttpHeader header;
    // Unrecognized headers are collapsible
    combine = !FromString(header, name) || HttpHeaderIsCollapsible(header)
              ? HC_YES : HC_NO;
  } else if (combine == HC_REPLACE) {
    m_headers.erase(name);
    combine = HC_NO;
  }
  // At this point, combine is one of (YES, NO, NEW)
  if (combine != HC_NO) {
    HeaderMap::iterator it = m_headers.find(name);
    if (it != m_headers.end()) {
      if (combine == HC_YES) {
        it->second.append(",");
        it->second.append(value);
	  }
      return;
	}
  }
  m_headers.insert(HeaderMap::value_type(name, value));
}

void
HttpData::clearHeader(const std::string& name) {
  m_headers.erase(name);
}

bool
HttpData::hasHeader(const std::string& name, std::string* value) const {
  HeaderMap::const_iterator it = m_headers.find(name);
  if (it == m_headers.end()) {
    return false;
  } else if (value) {
    *value = it->second;
  }
  return true;
}

template<class CTYPE>
size_t vsprintfn(CTYPE* buffer, size_t buflen, const CTYPE* format,
                 va_list args) {
  int len = vsnprintf(buffer, buflen, format, args);
  if ((len < 0) || (static_cast<size_t>(len) >= buflen)) {
    len = static_cast<int>(buflen - 1);
    buffer[len] = 0;
  }
  return len;
}

template<class CTYPE>
size_t sprintfn(CTYPE* buffer, size_t buflen, const CTYPE* format, ...) {
  va_list args;
  va_start(args, format);
  size_t len = vsprintfn(buffer, buflen, format, args);
  va_end(args);
  return len;
}
#endif


//
// HttpRequest
//
const std::string & HttpRequest::hasHeader(HttpHeader hh) const {
  for(const_iterator i = headers_.begin(); i != headers_.end(); i++)
      if (i->first == hh)
        return i->second;
  return "";
}

//
// HttpResponse
//

void
HttpResponse::set_success(uint32 scode, const std::string &message) {
  this->scode = scode;
  this->message = message;
  addHeader(HH_CONTENT_LENGTH, "0");
}

// void
// HttpResponse::set_success(const std::string& content_type,
//                               StreamInterface* document,
//                               uint32 scode) {
//   this->scode = scode;
//   message.erase(message.begin(), message.end());
//   setContent(content_type, document);
// }

void
HttpResponse::set_redirect(const std::string& location, uint32 scode) {
  this->scode = scode;
  message.clear();
  addHeader(HH_LOCATION, location);
  addHeader(HH_CONTENT_LENGTH, "0");
}

// OK, Continue
void HttpResponse::set_error(uint32 scode) {
  this->scode = scode;
  message = ""; // TODO:
}

const std::string kCRLF("\r\n");

std::ostream & operator<<(std::ostream &out, const HttpResponse &rep) {
  // HTTP/1.1 200 OK
  out << "HTTP/" << ToString(rep.version) 
    <<  " " << rep.scode 
    << " " << rep.message << kCRLF;

  for(HttpData::const_iterator i = rep.begin();
    i != rep.end(); ++i) {
      out << ToString(i->first) << ": " << i->second << kCRLF;
  }
  out << kCRLF;
  return out;
}

} } // cwf::http
