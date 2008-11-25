#include <iostream>
#include <cassert>
#include "cwf/http.hpp"


#define ASSERT(cond)  do {                                      \
  if (!(cond)) {                                                \
    printf("%s: %d: ASSERT FAILED: %s\n", __FILE__, __LINE__,   \
           #cond);                                              \
    assert(cond);                                               \
    exit(1);                                                    \
  }                                                             \
} while (0)

#define ASSERT_STRING_EQ(a, b)   do {                   \
    assert(StringEq(a, b, __FILE__, __LINE__, #a, #b)); \
} while (0)

bool StringEq(const std::string& a, const std::string& b,
                    const char* filename, int lineno,
                    const char* namea, const char* nameb) {
  if (a != b) {
    printf("%s: %d: ASSERT FAILED: %s == %s:\n", filename, lineno,
           namea, nameb);
    printf("EXPECTED:\n%s\n", a.c_str());
    printf("ACTUAL:\n%s\n", b.c_str());
    return false;
  }
  return true;
}

template<typename T>
bool FromToCheck(T input) {
  T out;
  bool f = FromString(out, ToString(input));
  return f && out == input;
}


int main() {
  using namespace cwf::http;
  

  ASSERT_STRING_EQ(ToString(HVER_1_0), "1.0");
  ASSERT(FromToCheck(HVER_1_0));

  ASSERT_STRING_EQ(ToString(HH_CONTENT_TYPE), "Content-Type");
  ASSERT(FromToCheck(HH_CONTENT_TYPE));
  
  HttpRequestData request;
  request.path = "/search?features=WH&sourceid=navclient-ff&client=navclient-auto-ff&googleip=O;null;189&ch=8cc70af15&q=info:file%3A%2F%2F%2FC%3A%2Fboost_1_36_0%2Flibs%2Ftypeof%2Findex.html";

  request.setHeader(HH_HOST, "toolbarqueries.google.com");
  request.setHeader(HH_USER_AGENT, "Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.9.0.4) Gecko/2008102920 Firefox/3.0.4 GoogleToolbarFF 5.0.20080919");
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,* / *;q=0.8
// Accept-Language: en-us,en;q=0.5
// Accept-Encoding: gzip,deflate
// Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7
//  request.setHeader(HH_KEEP_ALIVE, 300);
  request.setHeader(HH_CONNECTION, "keep-alive");
  request.setHeader(HH_COOKIE, "rememberme=true; PREF=ID=f1d4f9a634068eb2:TB=2:TM=1221895237:LM=1221956840:DV=AA:GM=1:S=NZy4WXaoJCPlbBGp; NID=17=kmfnhBr0V0q_qbnbi20vtUDJQCV7IVYqpnyongrSePYpnFAF2ImFuXvUSb_kLi49NHnbPGA-MFctkugCw_s1_D70CmFLpa1jGjG0B3O8alO7RIoRCc_SxpOiE3n4nqHP; SID=DQAAAIQAAAAReXbBVU69lY1N_sipdxcdE8uHqbVVCGfcZqVmetWHYV0Khl3a0caSDYn8lSJRuySeA_RR8MC1Kmj1z8qkMu1nZ2jBQPLZo4GYzy8lERtbnEFMAn5WSSB8K5HOeBZkHyaMLPGDgI8fL4ps5rYWROFPjVWVjMC6d_7yaHQDvKVTADHx41-m83u9SS82V5WSGA0; SS=DQAAAIYAAAAB5tX1QLvZ_CfcKOuWN4DxzPmvxSojmkznfxCE9ec7wkaNxawau8B_TZsZvxu34SYr8Yq8ITJsh-0gF0TD6esznpQr9hDqMx0GJNxuB4Wi2DSYwMCq3NqfEsC3mSMnKZ8WWsTQ5tRdcns3OKU7dpPLUXo-NEAJhYIYpM2BT6yeEvmv1_eZNOLPeiYy6hyf4aw; TZ=-480");


#if 0
GET /search?features=WH&sourceid=navclient-ff&client=navclient-auto-ff&googleip=O;null;189&ch=8cc70af15&q=info:file%3A%2F%2F%2FC%3A%2Fboost_1_36_0%2Flibs%2Ftypeof%2Findex.html HTTP/1.1
Host: toolbarqueries.google.com
User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.9.0.4) Gecko/2008102920 Firefox/3.0.4 GoogleToolbarFF 5.0.20080919
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,* / *;q=0.8
Accept-Language: en-us,en;q=0.5
Accept-Encoding: gzip,deflate
Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7
Keep-Alive: 300
Connection: keep-alive
Cookie: rememberme=true; PREF=ID=f1d4f9a634068eb2:TB=2:TM=1221895237:LM=1221956840:DV=AA:GM=1:S=NZy4WXaoJCPlbBGp; NID=17=kmfnhBr0V0q_qbnbi20vtUDJQCV7IVYqpnyongrSePYpnFAF2ImFuXvUSb_kLi49NHnbPGA-MFctkugCw_s1_D70CmFLpa1jGjG0B3O8alO7RIoRCc_SxpOiE3n4nqHP; SID=DQAAAIQAAAAReXbBVU69lY1N_sipdxcdE8uHqbVVCGfcZqVmetWHYV0Khl3a0caSDYn8lSJRuySeA_RR8MC1Kmj1z8qkMu1nZ2jBQPLZo4GYzy8lERtbnEFMAn5WSSB8K5HOeBZkHyaMLPGDgI8fL4ps5rYWROFPjVWVjMC6d_7yaHQDvKVTADHx41-m83u9SS82V5WSGA0; SS=DQAAAIYAAAAB5tX1QLvZ_CfcKOuWN4DxzPmvxSojmkznfxCE9ec7wkaNxawau8B_TZsZvxu34SYr8Yq8ITJsh-0gF0TD6esznpQr9hDqMx0GJNxuB4Wi2DSYwMCq3NqfEsC3mSMnKZ8WWsTQ5tRdcns3OKU7dpPLUXo-NEAJhYIYpM2BT6yeEvmv1_eZNOLPeiYy6hyf4aw; TZ=-480
#endif

  HttpRequestData response;

#if 0
HTTP/1.x 200 OK
Cache-Control: private, max-age=0
Date: Tue, 25 Nov 2008 14:54:00 GMT
Expires: -1
Content-Type: text/html; charset=ISO-8859-1
Server: gws
Content-Length: 0
#endif

  printf("Done\n");
  return 0;
}
