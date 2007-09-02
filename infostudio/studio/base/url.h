#ifndef __URL_H__
#define __URL_H__

#include <string>

template<class CTYPE>
struct Traits {
    // STL string type
    //typedef XXX string;
    // Null-terminated string
    //inline static const CTYPE* empty_str();
};

template<>
struct Traits<char> {
    typedef std::string string;
    inline static const char* Traits<char>::empty_str() { return ""; }
};

template<>
struct Traits<wchar_t> {
    typedef std::wstring string;
    inline static const wchar_t* Traits<wchar_t>::empty_str() { return L""; }
};

typedef unsigned short uint16;
const uint16 HTTP_DEFAULT_PORT = 80;
const uint16 HTTP_SECURE_PORT = 443;


template<class CTYPE>
class Url {
public:
    typedef typename Traits<CTYPE>::string string;

    // TODO: Implement Encode/Decode
    static int Encode(const CTYPE* source, CTYPE* destination, size_t len);
    static int Encode(const string& source, string& destination);
    static int Decode(const CTYPE* source, CTYPE* destination, size_t len);
    static int Decode(const string& source, string& destination);

    Url(const string& url);
    Url(const string& path, const string& server, uint16 port = HTTP_DEFAULT_PORT)
        : m_server(server), m_path(path), m_port(port),
        m_secure(HTTP_SECURE_PORT == port)
    {
        ASSERT(m_path.empty() || (m_path[0] == static_cast<CTYPE>('/')));
    }

    bool valid() const { return !m_server.empty(); }
    const string& server() const { return m_server; }
    // Note: path() was renamed to path_, because it now uses the stricter sense
    // of not including a query string.  I'm trying to think of a clearer name.
    const string& path_() const { return m_path; }
    const string& query() const { return m_query; }
    string full_path();
    string url();
    uint16 port() const { return m_port; }
    bool secure() const { return m_secure; }

    void set_server(const string& val) { m_server = val; }
    void set_path(const string& val) {
        ASSERT(val.empty() || (val[0] == static_cast<CTYPE>('/')));
        m_path = val;
    }
    void set_query(const string& val) {
        ASSERT(val.empty() || (val[0] == static_cast<CTYPE>('?')));
        m_query = val;
    }
    void set_port(uint16 val) { m_port = val; }
    void set_secure(bool val) { m_secure = val; }

private:
    string m_server, m_path, m_query;
    uint16 m_port;
    bool m_secure;
};



template<class CTYPE>
Url<CTYPE>::Url(const string& url) {
    const CTYPE* raw_url = url.c_str();
    if (ascnicmp(raw_url, "http://", 7) == 0) {
        raw_url += 7;
        m_secure = false;
    } else if (ascnicmp(raw_url, "https://", 8) == 0) {
        raw_url += 8;
        m_secure = true;
    } else {
        return;
    }
    m_port = UrlDefaultPort(m_secure);
    const CTYPE* colon = ::strchr(raw_url, static_cast<CTYPE>(':'));
    const CTYPE* slash = ::strchr(raw_url, static_cast<CTYPE>('/'));
    if (!colon && !slash) {
        m_server = url;
        // TODO: rethink this slash
        m_path.append(1, static_cast<CTYPE>('/'));
    } else {
        const CTYPE* ptr;
        if (colon == 0) {
            ptr = slash;
        } else if (slash == 0) {
            ptr = colon;
        } else {
            ptr = _min(colon, slash);
        }
        m_server.assign(raw_url, ptr - raw_url);
        if (ptr == colon) {
            CTYPE* tmp = 0;
            m_port = static_cast<uint16>(::strtoul(ptr + 1, &tmp, 10));
            ptr = tmp;
        }
        const CTYPE* query = ::strchr(ptr, static_cast<CTYPE>('?'));
        if (!query) {
            m_path.assign(ptr);
        } else {
            m_path.assign(ptr, query - ptr);
            m_query.assign(query);
        }
    }
    ASSERT(m_path.empty() || (m_path[0] == static_cast<CTYPE>('/')));
    ASSERT(m_query.empty() || (m_query[0] == static_cast<CTYPE>('?')));
}

template<class CTYPE>
typename Traits<CTYPE>::string Url<CTYPE>::full_path() {
    string full_path(m_path);
    full_path.append(m_query);
    return full_path;
}

template<class CTYPE>
typename Traits<CTYPE>::string Url<CTYPE>::url() {
    CTYPE protocol[9];
    strcpyn(protocol, ARRAY_SIZE(protocol), m_secure ? "https://" : "http://");
    string url(protocol);
    url.append(m_server);
    if (m_port != UrlDefaultPort(m_secure)) {
        CTYPE format[5], port[32];
        strcpyn(format, ARRAY_SIZE(format), ":%hu");
        sprintfn(port, ARRAY_SIZE(port), format, m_port);
        url.append(port);
    }
    url.append(m_path);
    url.append(m_query);
    return url;
}

#endif // __URL_H__
