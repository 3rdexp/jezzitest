#pragma once

#include <string>
#include <map>
#include "base.h"


//////////////////////////////////////////////////////////////////////
// Constants
//////////////////////////////////////////////////////////////////////

enum HttpCode { 
    HC_OK = 200,
    HC_NON_AUTHORITATIVE = 203,
    HC_NO_CONTENT = 204,
    HC_PARTIAL_CONTENT = 206,

    HC_MULTIPLE_CHOICES = 300,
    HC_MOVED_PERMANENTLY = 301,
    HC_FOUND = 302,
    HC_SEE_OTHER = 303,
    HC_NOT_MODIFIED = 304,
    HC_MOVED_TEMPORARILY = 307,

    HC_BAD_REQUEST = 400,
    HC_UNAUTHORIZED = 401,
    HC_FORBIDDEN = 403,
    HC_NOT_FOUND = 404,
    HC_PROXY_AUTHENTICATION_REQUIRED = 407,
    HC_GONE = 410,

    HC_INTERNAL_SERVER_ERROR = 500 
};

enum HttpVersion {
    HVER_1_0, HVER_1_1,
    HVER_LAST = HVER_1_1
};

enum HttpVerb {
    HV_GET, HV_POST, HV_PUT, HV_DELETE, HV_CONNECT, HV_HEAD,
    HV_LAST = HV_HEAD
};

enum HttpError {
    HE_NONE,
    HE_PROTOCOL, HE_DISCONNECTED, HE_OVERFLOW,
    HE_SOCKET, HE_SHUTDOWN, HE_OPERATION_CANCELLED,
    HE_AUTH,                // Proxy Authentication Required
    HE_CERTIFICATE_EXPIRED, // During SSL negotiation
    HE_STREAM,              // Problem reading or writing to the document
    HE_CACHE,               // Problem reading from cache
    HE_DEFAULT
};

enum HttpHeader {
    HH_AGE,
    HH_CACHE_CONTROL,
    HH_CONNECTION,
    HH_CONTENT_LENGTH,
    HH_CONTENT_RANGE,
    HH_CONTENT_TYPE,
    HH_COOKIE,
    HH_DATE,
    HH_ETAG,
    HH_EXPIRES,
    HH_HOST,
    HH_IF_MODIFIED_SINCE,
    HH_IF_NONE_MATCH,
    HH_KEEP_ALIVE,
    HH_LAST_MODIFIED,
    HH_LOCATION,
    HH_PROXY_AUTHENTICATE,
    HH_PROXY_AUTHORIZATION,
    HH_PROXY_CONNECTION,
    HH_RANGE,
    HH_SET_COOKIE,
    HH_TE,
    HH_TRAILERS,
    HH_TRANSFER_ENCODING,
    HH_UPGRADE,
    HH_USER_AGENT,
    HH_WWW_AUTHENTICATE,
    HH_LAST = HH_WWW_AUTHENTICATE
};

const unsigned short HTTP_DEFAULT_PORT = 80;
const unsigned short HTTP_SECURE_PORT = 443;

//////////////////////////////////////////////////////////////////////
// Utility Functions
//////////////////////////////////////////////////////////////////////

inline HttpError mkerr(HttpError err, HttpError def_err = HE_DEFAULT) {
    return (err != HE_NONE) ? err : def_err;
}

const char* ToString(HttpVersion version);
bool FromString(HttpVersion& version, const std::string& str);

const char* ToString(HttpVerb verb);
bool FromString(HttpVerb& verb, const std::string& str);

const char* ToString(HttpHeader header);
bool FromString(HttpHeader& header, const std::string& str);

inline bool HttpCodeIsInformational(uint32 code) { return ((code / 100) == 1); }
inline bool HttpCodeIsSuccessful(uint32 code)    { return ((code / 100) == 2); }
inline bool HttpCodeIsRedirection(uint32 code)   { return ((code / 100) == 3); }
inline bool HttpCodeIsClientError(uint32 code)   { return ((code / 100) == 4); }
inline bool HttpCodeIsServerError(uint32 code)   { return ((code / 100) == 5); }

bool HttpCodeHasBody(uint32 code);
bool HttpCodeIsCacheable(uint32 code);
bool HttpHeaderIsEndToEnd(HttpHeader header);
bool HttpHeaderIsCollapsible(HttpHeader header);

// functional for insensitive std::string compare
struct iless {
    bool operator()(const std::string& lhs, const std::string& rhs) const {
        return (::_stricmp(lhs.c_str(), rhs.c_str()) < 0);
    }
};

class RequestMap : private std::multimap<std::string, std::string, iless>
{
public:
    void addParam(const std::string& name, const std::string& value
        , bool overwrite = false);
    void clear(const std::string& name);
    bool hasParam(const std::string& name, std::string* value) const;
    bool parse(const std::string & s);

    enum CHARSET { CHARSET_UTF8, CHARSET_ANSI };

    std::string build(CHARSET ch = CHARSET_ANSI) const;
};


