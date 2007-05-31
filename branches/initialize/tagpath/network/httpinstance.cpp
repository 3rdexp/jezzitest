#include "HttpSession.h"
#include "HttpInstance.h"

namespace tting {

HttpSession & HttpInstance::Session()
{
    // TODO: Lock, only single thread call
    if (inst_ == 0)
    {
        inst_ = new HttpSession();
    }
    return *inst_;
}

void HttpInstance::ReleaseSession()
{
    delete inst_;
    inst_ = 0;
}

HttpSession * HttpInstance::inst_ = 0;

} // namespace tting

