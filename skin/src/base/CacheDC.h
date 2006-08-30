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
	}
	~CCacheDC()
	{
		Release();

		if( _hdc )
			DeleteDC( _hdc );
	}
	void SetBitmap(HBITMAP bmp, COLORREF trans = 0xff000000)
	{
		Release();
		if( 0 == bmp )
			return;

        _trans_color = trans;

		GetObject( bmp, sizeof BITMAP, &_bm );

		_bmpSelected = (HBITMAP)SelectObject(_hdc, bmp);
		ASSERT( _bmpSelected );
        _bmp = bmp;
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

	void Release()
	{
		if( _bmpSelected && _hdc )
		{
			DeleteObject( ::SelectObject(_hdc, _bmpSelected) );
			_bmpSelected = 0;
		}
	}
};

} // Skin


#endif //__CACHEDC_H__