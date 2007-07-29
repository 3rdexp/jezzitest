#include "stdafx.h"

#include <atlctrls.h>
#include "ado/AdoRecordSet.h"

#include "webinfoDlg.h"
#include "weblistDlg.h"
#include "backbaseDlg.h"

LRESULT CBackBaseDlg::OnAdd (WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	_pWebInfo = _pInfoManage->createNewWebInfo();

	SaveInfo( _pWebInfo, TRUE );

	if ( _pWebInfoDlg )
	{
		_pWebInfoDlg->_pWebListDlg->InsertWebInfo( _pWebInfo );
		_pWebInfoDlg->_pWebListDlg->ShowWebInfo( _pWebInfo );
	}
	return 0;
}