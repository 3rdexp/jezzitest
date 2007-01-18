#ifndef __CACHEDC_H__
#define __CACHEDC_H__

#include "CDibData.h"

namespace Skin{

// 选入了一个位图的dc,方便进行其他操作
// TODO: 检测是否存在 leak
class CCacheDC
{
public:
	CCacheDC(HDC dcSource = 0)
	{
		_hdc = CreateCompatibleDC(dcSource);
		_bmpSelected = 0;

		_orghdc = 0;
	}
	~CCacheDC()
	{
		Release();

		if( _hdc )
			DeleteDC( _hdc );
	}

	void SetBitmap(HBITMAP bmp, COLORREF trans)
	{
		Release();
		if( 0 == bmp )
			return;

        _trans_color = trans;

		GetObject( bmp, sizeof BITMAP, &_bm );

		_bmpSelected = (HBITMAP)SelectObject(_hdc, bmp);
		ASSERT( _bmpSelected );
        _bmp = bmp;

		_orghdc = CreateCompatibleDC( _hdc );
		_orgBmp.CreateCompatibleBitmap(_hdc, _bm.bmWidth, _bm.bmHeight );
		_hBmpOld = SelectObject(_orghdc, _orgBmp);
		BitBlt( _orghdc, 0, 0, _bm.bmWidth, _bm.bmHeight, _hdc, 0, 0, SRCCOPY);
	}
	operator HDC()
	{
		return _hdc;
	}
    HDC GetDC() { return _hdc; }
    HBITMAP GetBitmap() { return _bmp; }
	void GetBitmap(BITMAP *lpbm)
	{
		ASSERT( lpbm );
		*lpbm = _bm;
	}
    COLORREF TranslateColor() const
    {
        return _trans_color;
    }

	void ClearColor ()
	{
		BitBlt( _hdc, 0, 0, _bm.bmWidth, _bm.bmHeight, _orghdc, 0, 0, SRCCOPY);
	}

	void ChangeColor( COLORREF clr )
	{
		//int nPrecentRed = GetRValue(clr) - 100;
		//int nPrecentGreen = GetGValue(clr) - 100;
		//int nPrecentBlue = GetBValue(clr) - 100;

		//SetBitmap ( _bmp, _trans_color );
		WTL::CRect rc;
		rc.left = 0;
		rc.right = _bm.bmWidth;
		rc.top = 0;
		rc.bottom = _bm.bmHeight;

		for ( int i = 0; i < _bm.bmWidth; i++ )
		{
			for ( int j = 0; j < _bm.bmHeight; j++ )
			{
				COLORREF clrGet = GetPixel( _orghdc, i, j );
				if ( clrGet != TranslateColor() )
				{
					/*
					int lR = GetRValue(clr) * ( 1 + nPrecentRed / 100 );
					if ( lR < 0 )
						lR = 0;
					else if ( lR > 255 )
						lR = 255;
					
					int lG = GetGValue(clr) * ( 1 + nPrecentGreen / 100 );
					if ( lG < 0 )
						lG = 0;
					else if ( lG > 255 )
						lG = 255;


					int lB = GetBValue(clr) * ( 1 + nPrecentBlue / 100 );
					if ( lB < 0 )
						lB = 0;
					else if ( lB > 255 )
						lB = 255;
					
					*/
					BYTE lR = 255 - (255 - GetRValue(clrGet)) * (255 - GetRValue(clr))  /  255;
					BYTE lG = 255 - (255 - GetGValue(clrGet)) * (255 - GetGValue(clr))  /  255;
					BYTE lB = 255 - (255 - GetBValue(clrGet)) * (255 - GetBValue(clr))  /  255;

					SetPixel( _hdc, i, j, RGB( lR, lG, lB) );
				}
			}
		}

		//SetBitmap( GetBitmap(), TranslateColor() );
	}

#if 0
    int width() const { return _bm.bmWidth; }
    int height() const { return _bm.bmHeight; }
    bool ready() const { return( _bmpSelected != 0 ); }
#endif 
protected:
	HDC _hdc;
	HGDIOBJ _bmpSelected;
    HBITMAP _bmp;
	BITMAP _bm;
    COLORREF _trans_color;

	WTL::CBitmap _orgBmp;
	HDC		_orghdc;
	HGDIOBJ	_hBmpOld;

	void Release()
	{
		if( _bmpSelected && _hdc )
		{
			DeleteObject( ::SelectObject(_hdc, _bmpSelected) );
			_bmpSelected = 0;
		}

		if ( _orghdc )
		{
			DeleteObject( ::SelectObject(_orghdc, _hBmpOld) );
		}
	}
};

} // Skin


#endif //__CACHEDC_H__