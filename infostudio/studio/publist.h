
#ifndef __PUBLISH_RESULT_LIST_H__
#define __PUBLISH_RESULT_LIST_H__

typedef CWinTraitsOR<WS_CLIPCHILDREN | WS_CLIPSIBLINGS> PublishResultListTraits;

class PublishResultList : public CWindowImpl<PublishResultList
        , CListViewCtrl, PublishResultListTraits>
{
public:
    BEGIN_MSG_MAP(PublishResultList)
    END_MSG_MAP()
};



#endif // __PUBLISH_RESULT_LIST_H__
