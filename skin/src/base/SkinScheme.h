#ifndef __SKINSCHEME_H__
#define __SKINSCHEME_H__

#include <boost/shared_ptr.hpp>
#include <tmschema.h>

#include "../skinitf.h"
#include "skiniid.h"
#include "scheme.h"
#include "CacheDC.h"

//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +-----------+-----------+-------+-------------------------------+
//  |   class   |  part     | state |               prop            |
//  +-----------+-----------|-------+-------------------------------+
namespace Skin{


class ATL_NO_VTABLE SkinScheme : public CComObjectRoot
	, public ISkinScheme
{
public:
	typedef SkinScheme thisType;
	BEGIN_COM_MAP(thisType)
		COM_INTERFACE_ENTRY_IID(IID_ISkinScheme, ISkinScheme)
	END_COM_MAP()

	SkinScheme() {}

	DECLARE_PROTECT_FINAL_CONSTRUCT()
	HRESULT FinalConstruct()
	{
		// Do Nothing
		return S_OK;
	}

	void FinalRelease()
	{
	}

	// ISkinScheme
	STDMETHODIMP_(BOOL) Draw(HDC hdc, int iClassId, int iPartId, int iStateId, 
        long dx, long dy, long dcx = 0, long dcy = 0, DWORD dwRop = SRCCOPY)
	{
		ASSERT( hdc );

		if (!_spCache)
			return FALSE;

		area_t area;
		BOOL f = _psd->get(make_key(iClassId, iPartId, iStateId), area);
		ASSERT(f);
		if (f)
		{
			if( 0 == dcx ) dcx = area.right - area.left;
			if( 0 == dcy ) dcy = area.bottom - area.top;

			
			// TransparentBlt2(hdc, dx, dy, dcx, dcy, _spCache->GetDC(), area.left, 
            // area.top, scx, scy, _spCache->TranslateColor());

			if (area.hstep && !area.vstep)
			{
				// horz and !vert
				// ___________________
				// ||               || 
				// ||               ||
				// ||               ||
				// -------------------
				// ��
				TransparentBlt2(hdc, dx, dy, area.hstep, dcy, 
					_spCache->GetDC(), area.left, area.top, area.hstep, area.Height(), 
                    _spCache->TranslateColor());
				// ��
				StretchBlt(hdc, dx + area.hstep, dy, dcx - 2 * area.hstep, dcy, 
					_spCache->GetDC(), area.left + area.hstep, area.top, area.Width() 
                    - 2 * area.hstep, area.Height(), SRCCOPY);
				// ��
				TransparentBlt2(hdc, dx + dcx - area.hstep, dy, area.hstep, dcy, 
					_spCache->GetDC(), area.right - area.hstep, area.top, area.hstep, area.Height(), 
                    _spCache->TranslateColor());
			}
			else if (!area.hstep && area.vstep)
			{
				// !horz and vert
				// ___________________
				// |                 | 
				// -------------------
				// |                 |
				// |                 |
				// -------------------
				// |                 |
				// -------------------
				// ��
				TransparentBlt2(hdc, dx, dy, dcx, area.vstep, 
					_spCache->GetDC(), area.left, area.top, area.Width(), area.vstep, 
                    _spCache->TranslateColor());
				// ��
				StretchBlt(hdc, dx, dy + area.vstep, dcx, dy - 2 * area.vstep, 
					_spCache->GetDC(), area.left, area.top + area.vstep, area.Width(), 
                    area.Height() - 2 * area.vstep, SRCCOPY);
				// ��
				TransparentBlt2(hdc, dx, dy + dcy - area.vstep, dcx, area.vstep, 
					_spCache->GetDC(), area.left, area.bottom - area.vstep, area.Width(), 
                    area.vstep, _spCache->TranslateColor());
			}
			else if (area.hstep && area.vstep)
			{
				// horz and vert
				// ___________________
				// ||               || 
				// -------------------
				// ||               ||
				// ||               ||
				// -------------------
				// ||               ||
				// -------------------
				// ���Ͻ�
				TransparentBlt2(hdc, dx, dy, area.hstep, area.vstep, 
					_spCache->GetDC(), area.left, area.top, area.hstep, area.vstep, 
                    _spCache->TranslateColor());
				// �ϲ��м�
				StretchBlt(hdc, dx + area.hstep, dy, dcx - 2 * area.hstep, area.vstep, 
					_spCache->GetDC(), area.left + area.hstep, area.top, area.Width() 
                    - 2 * area.hstep, area.vstep, SRCCOPY);
				// �ϲ��ұ�
				TransparentBlt2(hdc, dx + dcx - area.hstep, dy, area.hstep, area.vstep, 
					_spCache->GetDC(), area.right - area.hstep, area.top, area.hstep, 
                    area.vstep, _spCache->TranslateColor());

				// �м䲿�� ���
				StretchBlt(hdc, dx, dy + area.vstep, area.hstep, dcy - 2 * area.vstep, 
					_spCache->GetDC(), area.left, area.top + area.vstep, area.hstep, 
                    area.Height() - 2 * area.vstep, SRCCOPY);

				// �м䲿�� �в�
				StretchBlt(hdc, dx + area.hstep, dy + area.vstep, dcx - 2 * area.hstep, 
                    dcy - 2 * area.vstep, 
					_spCache->GetDC(), area.left + area.hstep, area.top + area.vstep, 
                    area.Width() - 2 * area.hstep, area.Height() - 2 * area.vstep, SRCCOPY);

				// �м䲿�� �ұ�
				StretchBlt(hdc, dx + dcx - area.hstep, dy + area.vstep, area.hstep, 
                    dcy - 2 * area.vstep, 
					_spCache->GetDC(), area.right - area.hstep, area.top + area.vstep, 
                    area.hstep, area.Height() - 2 * area.vstep, SRCCOPY);

				// ���沿�� ���
				TransparentBlt2(hdc, dx, dy + dcy - area.vstep, area.hstep, area.vstep, 
					_spCache->GetDC(), area.left, area.top + area.Height() - area.vstep, 
                    area.hstep, area.vstep, _spCache->TranslateColor());

				// ���沿�� �м�
				StretchBlt(hdc, dx + area.hstep, dy + dcy - area.vstep, dcx - 2 * area.hstep, 
                    area.vstep,
					_spCache->GetDC(), area.left + area.hstep, area.bottom - area.vstep, 
                    area.Width() - 2 * area.hstep, area.vstep, SRCCOPY);

				// ���沿�� �ұ�
				TransparentBlt2(hdc, dx + dcx - area.hstep, dy + dcy - area.vstep,  
                   area.hstep, area.vstep, 
					_spCache->GetDC(), area.right - area.hstep, area.bottom - area.vstep, 
                    area.hstep, area.vstep, _spCache->TranslateColor());
			}
			else if (!area.hstep && !area.vstep)
			{
				f = ::StretchBlt(hdc, dx, dy, dcx, dcy, _spCache->GetDC(), area.left, 
                    area.top, area.right-area.left, area.bottom-area.top, dwRop);
				ASSERT(f);
			}
#if 0
			HDC d = ::GetDC(0);
			BitBlt(d, dx, dy, dcx, dcy, _spCache->GetDC(), area.left, area.top, SRCCOPY);
			::ReleaseDC(0, d);
#endif
		}
		return f;
	}

	// source ���ֲ���: sx/sy ����ڱ�ͼƬ�����Ͻǵ����꣬scx/scy �ߴ�
	// d: dest s: source
	STDMETHODIMP_(BOOL) Draw(HDC hdc, int iClassId, int iPartId, int iStateId, long dx, 
        long dy, long dcx, long dcy, 
		long sx, long sy, long scx, long scy, DWORD dwRop = SRCCOPY)
	{
		ASSERT( hdc );

		if (!_spCache)
			return FALSE;

		area_t area;
		BOOL f = _psd->get(make_key(iClassId, iPartId, iStateId), area);
		ASSERT(f);
		if (f)
		{
			ASSERT( sx + scx <= area.right - area.left );
			ASSERT( sy + scy <= area.bottom - area.top );

			area.left += sx;
			area.top += sy;

			f = ::StretchBlt(hdc, dx, dy, dcx, dcy, _spCache->GetDC(), area.left, 
                area.top, area.right-area.left, area.bottom-area.top, dwRop);
			ASSERT(f);
		}
		return f;
	}

	STDMETHODIMP_(BOOL) TransparentDraw(HDC hdc, int iClassId, int iPartId, int iStateId, 
        long dx, long dy, long dcx, long dcy,
		long sx, long sy, long scx, long scy)
	{
		ASSERT( hdc );

		if (!_spCache)
			return FALSE;

		area_t area;
		BOOL f = _psd->get(make_key(iClassId, iPartId, iStateId), area);
		ASSERT(f);
		if (f)
		{
			ASSERT( sx + scx <= area.right - area.left );
			ASSERT( sy + scy <= area.bottom - area.top );

			area.left += sx;
			area.top += sy;

			f = TransparentBlt2(hdc, dx, dy, dcx, dcy, _spCache->GetDC(), area.left, 
                area.top, scx, scy, _spCache->TranslateColor());
			ASSERT(f);
		}
		return f;
	}
	STDMETHODIMP_(BOOL) TransparentDraw(HDC hdc, int iClassId, int iPartId, int iStateId, 
        const RECT* lprc)
	{
		ASSERT( lprc );
		return lprc && TransparentDraw(hdc, iPartId, iStateId, lprc->left, lprc->top, 
            lprc->right-lprc->left, lprc->bottom-lprc->top );
	}
	STDMETHODIMP_(BOOL) TransparentDraw(HDC hdc, int iClassId, int iPartId, int iStateId,
        long dx, long dy, long dcx = 0, long dcy = 0)
	{
		ASSERT( hdc );

		if (!_spCache)
			return FALSE;

		area_t area;
		BOOL f = _psd->get(make_key(iClassId, iPartId, iStateId), area);
		ASSERT(f);
		if (f)
		{
			if( 0 == dcx ) dcx = area.right - area.left;
			if( 0 == dcy ) dcy = area.bottom - area.top;

			f = TransparentBlt2(hdc, dx, dy, dcx, dcy, _spCache->GetDC(), area.left, 
                area.top, area.right-area.left, area.bottom-area.top, _spCache->TranslateColor());
			ASSERT(f);
		}
		return f;
	}


	// ������ ExtractIcon
	STDMETHODIMP_(BOOL) GetBitmap(int iClassId, int iPartId, int iStateId, int iPropId, 
        HBITMAP *pBitmap)
	{
		if (!_spCache)
			return FALSE;

		area_t area;
		BOOL f = _psd->get(make_key(iClassId, iPartId, iStateId), area);
		ASSERT(f);
		if (!f)
			return FALSE;

		// 1 target bitmap and target dc
		HDC dcTarget = ::CreateCompatibleDC( 0 ); //(HDC)_cache );
		ATLASSERT( dcTarget );
		if( !dcTarget )
			return FALSE;

		int bitsPixel = ::GetDeviceCaps( dcTarget, BITSPIXEL );
		int planes = ::GetDeviceCaps( dcTarget, PLANES );
		HBITMAP bmpTarget = ::CreateBitmap( area.right - area.left, area.bottom-area.top, 
            planes, bitsPixel, 0 );
		ATLASSERT( bmpTarget );
		if( !bmpTarget )
		{
			::DeleteDC( dcTarget );
			return FALSE;
		}

		HGDIOBJ pOldTargetBmp = ::SelectObject( dcTarget, bmpTarget );
		ATLASSERT( pOldTargetBmp );
		if( !pOldTargetBmp )
		{
			::DeleteObject( bmpTarget );
			::DeleteDC( dcTarget );
			return FALSE;
		}

		// 2 just bitblt on target bitmap
		int nRet = 0;
		nRet = ::BitBlt(dcTarget, 0, 0, area.right-area.left, area.bottom-area.top, 
            _spCache->GetDC(), area.left, area.top, SRCCOPY);
		ATLASSERT( nRet );

		// 3 release
		::SelectObject( dcTarget, pOldTargetBmp );
		::DeleteDC( dcTarget );

		*pBitmap = bmpTarget;
		return TRUE;
	}

    STDMETHOD(DrawParentBackground)(HWND hwnd, HDC hdc, RECT *prc)
    {
        CWindow wndparent( GetParent(hwnd) );
        CWindow wndchild(hwnd);

        // ֱ��ʹ�ô�ɫ���
        HBRUSH br = (HBRUSH)wndparent.SendMessage(WM_CTLCOLORDLG, (WPARAM)hdc, 
            (LPARAM)hwnd);
        if (br)
            FillRect(hdc, prc, br);

        // TODO: ��� WM_PRINTCLIENT ����ֵ��ȷ�� Dialog �Ƿ��е�ͼ
#if 0
        CRect rcparent;
        wndparent.GetClientRect(&rcparent);

        // memory dc
        HDC dcMem = ::CreateCompatibleDC(hdc);
        ASSERT( dcMem );
        HBITMAP bmpMemBg = ::CreateCompatibleBitmap(hdc, rcparent.Width(), 
            rcparent.Height());
        ASSERT( bmpMemBg );
        HGDIOBJ pOldBmp = ::SelectObject(dcMem, bmpMemBg);
        ASSERT( pOldBmp );

        // 1������memdc ����
        LRESULT lRes = wndparent.SendMessage(WM_PRINTCLIENT, (WPARAM)dcMem, 
            PRF_CLIENT | PRF_ERASEBKGND | PRF_CHECKVISIBLE);

        // memory dc
        //::BitBlt(hdc, prc->left, prc->top, prc->right - prc->left, prc->bottom - prc->top, dcMem, pt.x, pt.y, SRCCOPY);
        ::SelectObject(dcMem, pOldBmp);
        ::DeleteObject(bmpMemBg);
        ::DeleteDC(dcMem);
#endif
        return S_OK;
    }

    STDMETHOD_(BOOL, IsThemeBackgroundPartiallyTransparent)(int iClassId, int iPartId, 
        int iStateId)
    {
        // TODO: �������ж�ȡһЩ��Ϣ��ȷ���Ƿ�Ϊ͸���ؼ�

        if (iClassId == BUTTON )
            return TRUE;

		if (iClassId == PROGRESS )
			return TRUE;

		if (iClassId == TRACKBAR)
			return  TRUE;

		if (iClassId == TAB)
			return  TRUE;
        return FALSE;
    }

    // �ڷ�MemoryDC�У�����pClipRect�ǳ������壬�ڱ�Skin��ʵ���п��Բ�ʹ��
    STDMETHOD(DrawBackground)(HDC hdc, int iClassId, int iPartId, int iStateId, 
        const RECT *pRect, const RECT *pClipRect)
    {
        BOOL f = Draw( hdc, iClassId, iPartId, iStateId, pRect->left, pRect->top,
            pRect->right - pRect->left, pRect->bottom - pRect->top );
        return f;
    }

    // dwTextFlags, dwTextFlags2 �Ķ���� MSDN: DrawThemeText
    STDMETHOD(DrawText)(HDC hdc, int iClassId, int iPartId, int iStateId, LPCSTR szText, 
        DWORD dwTextFlags,DWORD dwTextFlags2, const RECT *pRect)
    {
        // TODO: use logfont

        COLORREF cr;
        if (pRect && GetColor(iClassId, iPartId, iStateId, TMT_TEXTCOLOR, &cr))
        {
            RECT rc = *pRect;
			
            int mode = SetBkMode(hdc, TRANSPARENT);
			COLORREF clrOldText = ::SetTextColor( hdc, cr );
            ::DrawText(hdc, szText, -1, &rc, dwTextFlags);
            SetBkMode(hdc, mode);
			::SetTextColor(hdc,clrOldText);
        }
        return S_OK;
    }

    STDMETHOD(DrawIcon)(HDC hdc, int iClassId, int iPartId, int iStateId, 
        const RECT *pRect, HIMAGELIST himl, int iImageIndex)
    {
        return S_OK;
    }

    // uEdge, uFlags, pContentRect ���ڿ��Բ�ʹ��
    STDMETHOD(DrawEdge)(HDC hdc, int iClassId, int iPartId, int iStateId, 
        const RECT *pDestRect, UINT uEdge, UINT uFlags, OPTIONAL OUT RECT *pContentRect)
    {
        return S_OK;
    }

	STDMETHODIMP_(BOOL) GetColor(int iClassId, int iPartId, int iStateId, int iPropId, 
        COLORREF *pColor)
	{
		return _psd->get(make_key(iClassId, iPartId, iStateId, iPropId), *pColor);
	}
	STDMETHOD_(BOOL, GetRect)(int iClassId, int iPartId, int iStateId, RECT *pRect)
	{
		area_t area;
		BOOL f = _psd->get(make_key(iClassId, iPartId, iStateId), area);
        if (!f && iStateId)
        {
            ATLTRACE("iStateId change to zero\n");
            f = _psd->get(make_key(iClassId, iPartId, 0), area);
        }
		ASSERT(f);
		if (f && pRect)
			*pRect = area;
		return f;
	}
	STDMETHOD_(BOOL, GetFont)  (int iClassId, int iPartId, int iStateId, int iPropId, 
        LOGFONT *pFont)
	{
		return FALSE;
	}
	STDMETHOD_(BOOL, GetRegion)(int iClassId, int iPartId, int iStateId, int iPropId, 
        HRGN *pRegion)
	{
		area_t area;
		BOOL f = _psd->get(make_key(iClassId, iPartId, iStateId), area);
		if (f)
		{
			HBITMAP bmp = _spCache->GetBitmap();
			if( bmp )
			{
				HRGN rgn = BitmapToRegion( bmp, _spCache->TranslateColor(), 
                    _spCache->TranslateColor(), &area);
				DeleteObject( bmp );
				f = TRUE;
				*pRegion = rgn;
			}
		}
		return f;
	}
public:
    void SetSchemeData(const scheme_data * psd, boost::shared_ptr<CCacheDC> cdc)
	{
        _psd = psd;
		_spCache = cdc;
	}
private:
    boost::shared_ptr<CCacheDC> _spCache; // todo: use shared_ptr<CCacheDC>
    const scheme_data * _psd;
};

} // Skin
#endif //__SKINSCHEME_H__