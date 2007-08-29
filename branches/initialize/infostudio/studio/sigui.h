
#ifndef __SIG_UI_H__
#define __SIG_UI_H__

#include <boost/signal.hpp>


class SigStudio
{
public:
    boost::signal<void (std::wstring *, std::wstring *)> SigUserInfo;
};

#endif // __SIG_UI_H__

