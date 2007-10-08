#pragma once

class CTransTab : public CWindowImpl<CTransTab, CTabCtrl>
{
public:
	BEGIN_MSG_MAP(CTransTab)
        FORWARD_NOTIFICATIONS()
	END_MSG_MAP()
};
