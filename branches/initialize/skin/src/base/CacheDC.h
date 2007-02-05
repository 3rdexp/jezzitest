#ifndef __CACHEDC_H__
#define __CACHEDC_H__

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

		_bmpSelected = (HBITMAP)::SelectObject(_hdc, bmp);
		ASSERT( _bmpSelected );
        _bmp = bmp;

		_orghdc = ::CreateCompatibleDC( _hdc );
		_orgBmp.CreateCompatibleBitmap(_hdc, _bm.bmWidth, _bm.bmHeight );
		_hBmpOld = ::SelectObject(_orghdc, _orgBmp);
		BitBlt( _orghdc, 0, 0, _bm.bmWidth, _bm.bmHeight, _hdc, 0, 0, SRCCOPY);
		
		/*
		BITMAPINFO  bmi = { 0 };
		//////////////////////////////////////////
		//构建DIB图片结构
		bmi.bmiHeader.biSize            = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth           = _bm.bmWidth;
		bmi.bmiHeader.biHeight          = _bm.bmHeight;
		bmi.bmiHeader.biPlanes          = 1;
		bmi.bmiHeader.biBitCount        = 24;
		bmi.bmiHeader.biCompression     = BI_RGB;
		bmi.bmiHeader.biSizeImage       = 0;
		bmi.bmiHeader.biXPelsPerMeter   = 0;
		bmi.bmiHeader.biYPelsPerMeter   = 0;
		bmi.bmiHeader.biClrUsed         = 0;
		bmi.bmiHeader.biClrImportant    = 0;

		//计算
		//	int nFileHeaderSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO);
		//	int nBits = 4 * ((bmi.bmiHeader.biBitCount * bmi.bmiHeader.biWidth + 31) / 32) * bmi.bmiHeader.biHeight;
		//	int nFileSize = nFileHeaderSize + nBits;
		//	int nStep = 4 * ((bmi.bmiHeader.biBitCount * bmi.bmiHeader.biWidth + 31) / 32);

		m_hScrDIB = ::CreateDIBSection(0,
			&bmi,
			DIB_RGB_COLORS,
			(VOID **)&m_pScrBit,
			NULL,
			0
			);

		HDC					hScreenDC = ::GetDC(NULL);

		HDC MemDC = ::CreateCompatibleDC( hScreenDC );
		HGDIOBJ hTemp = ::SelectObject(MemDC, m_hScrDIB);

		BitBlt( MemDC, 0, 0, _bm.bmWidth, _bm.bmHeight, _hdc, 0, 0, SRCCOPY);

		::SelectObject(MemDC, hTemp);
		DeleteDC( MemDC );
		::ReleaseDC(NULL,hScreenDC);
		*/

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

	void BitmapChangeColor( COLORREF cColor )
	{
		//if ( !m_pScrBit )
		//	return ;

		/*
		HDC	hScreenDC = ::GetDC(NULL);
		HDC MemDC	= ::CreateCompatibleDC( hScreenDC );
		HGDIOBJ hTemp = ::SelectObject(MemDC, m_hScrDIB);

		BitBlt( MemDC, 0, 0, _bm.bmWidth, _bm.bmHeight, _orghdc, 0, 0, SRCCOPY);

		AlphaColor( m_pScrBit, _bm.bmWidth, _bm.bmHeight, cColor );

		BitBlt( _hdc, 0, 0, _bm.bmWidth, _bm.bmHeight, MemDC, 0, 0, SRCCOPY);

		::SelectObject(MemDC, hTemp);
		DeleteDC( MemDC );
		::ReleaseDC(NULL,hScreenDC);

		*/
		BITMAPINFO  bmi = { 0 };
		//////////////////////////////////////////
		//构建DIB图片结构
		bmi.bmiHeader.biSize            = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth           = _bm.bmWidth;
		bmi.bmiHeader.biHeight          = _bm.bmHeight;
		bmi.bmiHeader.biPlanes          = 1;
		bmi.bmiHeader.biBitCount        = 24;
		bmi.bmiHeader.biCompression     = BI_RGB;
		bmi.bmiHeader.biSizeImage       = 0;
		bmi.bmiHeader.biXPelsPerMeter   = 0;
		bmi.bmiHeader.biYPelsPerMeter   = 0;
		bmi.bmiHeader.biClrUsed         = 0;
		bmi.bmiHeader.biClrImportant    = 0;

		//计算
	//	int nFileHeaderSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO);
	//	int nBits = 4 * ((bmi.bmiHeader.biBitCount * bmi.bmiHeader.biWidth + 31) / 32) * bmi.bmiHeader.biHeight;
	//	int nFileSize = nFileHeaderSize + nBits;
	//	int nStep = 4 * ((bmi.bmiHeader.biBitCount * bmi.bmiHeader.biWidth + 31) / 32);



		LPBYTE				pBits = NULL;
		HDC					hScreenDC = ::GetDC(NULL);

		HBITMAP hScrDIB = ::CreateDIBSection(0,
			&bmi,
			DIB_RGB_COLORS,
			(VOID **)&pBits,
			NULL,
			0
			);

		HDC MemDC = ::CreateCompatibleDC( hScreenDC );
		HGDIOBJ hTemp = ::SelectObject(MemDC, hScrDIB);

		::BitBlt( MemDC, 0, 0, _bm.bmWidth, _bm.bmHeight, _orghdc, 0, 0, SRCCOPY);

		AlphaColor( pBits, _bm.bmWidth, _bm.bmHeight, cColor );

		::BitBlt( _hdc, 0, 0, _bm.bmWidth, _bm.bmHeight, MemDC, 0, 0, SRCCOPY);

		::SelectObject(MemDC, hTemp);
		::DeleteDC( MemDC );
		::ReleaseDC(NULL,hScreenDC);

	}

	void AlphaColor( LPBYTE lpScr,int nWidth,int nHeight,COLORREF clr)
	{
		//COLORREF	*pAlphaBmpColor = NULL;
		COLORREF	clrTemp = 0;
		RGBTRIPLE	*pScrColor = NULL;

		//pAlphaBmpColor = (COLORREF*)(lpScr + sizeof(BITMAPINFOHEADER));//图片缓冲

		RGBTRIPLE rtBK = {0};
		RGBTRIPLE rtClr = {0};
		
		RGBTRIPLE rtTrans = {0};

		rtTrans.rgbtRed = GetRValue(TranslateColor());
		rtTrans.rgbtGreen = GetGValue(TranslateColor());
		rtTrans.rgbtBlue  = GetBValue(TranslateColor());

		rtClr.rgbtRed = GetRValue( clr );
		rtClr.rgbtGreen = GetGValue( clr );
		rtClr.rgbtBlue  = GetBValue( clr );

		int nStep = 4 * ((24 * nWidth + 31) / 32);

		for(int i = 0; i < nHeight; i++)//行
		{
			for(int j = 0; j < nWidth;j++)//像素
			{
				
				pScrColor = (RGBTRIPLE*)(lpScr + (i  * nStep));
				
				rtBK = pScrColor[j];//屏幕像素,3字节对齐需要调整

				if ( rtTrans.rgbtBlue != rtBK.rgbtBlue || rtTrans.rgbtGreen != rtBK.rgbtGreen || rtTrans.rgbtRed != rtBK.rgbtRed)
				{
					rtBK.rgbtBlue = 255 - ( 255 - rtBK.rgbtBlue ) * ( 255 - rtClr.rgbtBlue )  /  255;
					rtBK.rgbtGreen = 255 - ( 255 - rtBK.rgbtGreen ) * ( 255 - rtClr.rgbtGreen )  /  255;
					rtBK.rgbtRed = 255 - ( 255 - rtBK.rgbtRed ) * ( 255 - rtClr.rgbtRed )  /  255;

					pScrColor[j] = rtBK;	
				}				
			}
		}
	}

	void ChangeColor( COLORREF clr )
	{
		//int nPrecentRed = GetRValue(clr) - 100;
		//int nPrecentGreen = GetGValue(clr) - 100;
		//int nPrecentBlue = GetBValue(clr) - 100;
		DWORD dwStart = GetTickCount();
		TRACE("time start is %d \r\n", dwStart );
		BitmapChangeColor( clr );
		TRACE("time end is %d and run %d \r\n", GetTickCount(), GetTickCount() - dwStart );
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
			::DeleteObject( ::SelectObject(_hdc, _bmpSelected) );
			_bmpSelected = 0;
		}

		if ( _orghdc )
		{
			::DeleteObject( ::SelectObject(_orghdc, _hBmpOld) );
		}
	}
};

} // Skin


#endif //__CACHEDC_H__