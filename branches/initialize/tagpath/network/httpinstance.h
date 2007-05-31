
#ifndef __HTTP_INSTANCE_H__
#define __HTTP_INSTANCE_H__

namespace tting {

class HttpSession;

class HttpInstance {
public:
    static HttpSession & Session();
    static void ReleaseSession();
private:
    static HttpSession * inst_;
};

} // namespace tting

#endif // __HTTP_INSTANCE_H__
