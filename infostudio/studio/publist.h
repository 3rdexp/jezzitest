
#ifndef __PUBLIST_H__
#define __PUBLIST_H__

typedef CWinTraitsOR<WS_CLIPCHILDREN | WS_CLIPSIBLINGS> PublishListTraits;

class PublishList : public CWindowImpl<PublishList, CListViewCtrl, PublishListTraits>
{
public:
    BEGIN_MSG_MAP(PublishList)
    END_MSG_MAP()
};



#endif // __PUBLIST_H__
