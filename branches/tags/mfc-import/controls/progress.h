#pragma once

#include "../base/skinctrl.h"


namespace Skin {

	template<class BaseT = WTL::CProgressBarCtrl>
	struct SkinProgressCtrl : public SkinControlImpl<SkinProgressCtrl, BaseT>
	{
		enum { class_id = PROGRESS };

		typedef SkinProgressCtrl<BaseT> this_type;
		typedef SkinControlImpl<SkinProgressCtrl, BaseT> base_type;

		BEGIN_MSG_MAP(this_type)
			MESSAGE_HANDLER(WM_PAINT, OnPaint)
		END_MSG_MAP()

		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			WTL::CPaintDC dc(m_hWnd);
			DoPaint(dc);
			return 0;
		}

		LRESULT DoPaint(HDC dc)
		{
			WTL::CRect rc;
			GetClientRect(&rc);

			WTL::CMemoryDC memdc(dc, rc);

			BOOL bSmooth = (GetStyle() & PBS_SMOOTH);
			BOOL bHorz = !(GetStyle() & PBS_VERTICAL);

			int nBarPart = bHorz ? PP_BAR : PP_BARVERT ;
			int nChunkPart = bHorz ? PP_CHUNK : PP_CHUNKVERT ;

			int nState = 1; //没有状态的变化,都为1即可 ,tmschema.h里面没有progress的state的定义
			if(_scheme && _scheme->IsThemeBackgroundPartiallyTransparent(class_id, nBarPart, nState))
				_scheme->DrawParentBackground(m_hWnd, memdc, &rc);


			// 绘制背景
			if (_scheme)
				_scheme->DrawBackground(memdc, class_id, nBarPart, nState, &rc, NULL );

			// 绘制进度条
			// 1.计算进度条的RECT
			int nPos= GetPos();

			int nLower, nUpper;
			GetRange(nLower, nUpper);

			WTL::CRect rClient = rc;
			if (bHorz)
				rClient.right = rClient.left + (rClient.Width() * nPos) / (nUpper - nLower);
			else
				rClient.top = rClient.bottom - (rClient.Height() * nPos) / (nUpper - nLower);
			
			// 2.根据进度条和背景图的大小关系,修正进度条的RECT
			int nBarWidth =  GetSchemeWidth( nBarPart, nState );
			int nBarHeight =  GetSchemeHeight( nBarPart, nState );
			int nChunkWidth =  GetSchemeWidth( nChunkPart, nState );
			int nChunkHeight =  GetSchemeHeight( nChunkPart, nState );
			
			int nSpaceWidth = (nBarWidth - nChunkWidth) / 2;
			int nSpaceHeight = (nBarHeight - nChunkHeight) / 2;
			
			rClient.left += nSpaceWidth;
			rClient.right -= nSpaceWidth;
			rClient.top += nSpaceHeight;
			rClient.bottom -= nSpaceHeight;
			
			// 绘制进度条
			if (_scheme && rClient.Width() > 0 && rClient.Height() > 0)
				_scheme->DrawBackground(memdc, class_id, nChunkPart, nState, &rClient, NULL );

			return 0;
		}
	private:
		int m_nPart;
	};

}; // namespace 