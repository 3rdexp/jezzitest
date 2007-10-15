
#ifndef __PUBTREE_H__
#define __PUBTREE_H__

typedef CWinTraitsOR<WS_CLIPCHILDREN | WS_CLIPSIBLINGS
    | TVS_LINESATROOT | TVS_HASLINES | TVS_HASBUTTONS | TVS_DISABLEDRAGDROP 
    | TVS_SHOWSELALWAYS | TVS_TRACKSELECT>
    PublishTreeTraits;


class PublishTree : public CWindowImpl<PublishTree, CTreeViewCtrl, PublishTreeTraits>
{
public:
    BEGIN_MSG_MAP(PublishTree)
    END_MSG_MAP()
};

#endif // __PUBTREE_H__
