#include "stdafx.h"

namespace Skin{
// BitmapToRegion() is a pure Win32 function. It doesn't make any use of the MFC. Its code is standalone 
	// and only need to be compiled in a C++ file (a C compiler implies some minor modifications in the code). 
	// The function has been tested successfully on both Window 95, Windows 98 and Windows NT4. On the two 
	// formers systems, we only had have to modify the function slightly, because ExtCreateRgn() failed to 
	// create regions containing more than 4000 rectangles. The performances are quite good : on a PII-400 under NT4, 
	// the function spend only 70 ms to scan a 575x846x8 drawing (from the great artist Luis Royo) and create a 
	// corresponding region of more than 21000 rectangles !

	
//	BitmapToRegion :	Create a region from the "non-transparent" pixels of a bitmap
//	Author :			Jean-Edouard Lachand-Robert (http://www.geocities.com/Paris/LeftBank/1160/resume.htm), June 1998.
//
//	hBmp :				Source bitmap
//	cTransparentColor :	Color base for the "transparent" pixels (default is black)
//	cTolerance :		Color tolerance for the "transparent" pixels.
//
//	A pixel is assumed to be transparent if the value of each of its 3 components (blue, green and red) is 
//	greater or equal to the corresponding value in cTransparentColor and is lower or equal to the 
//	corresponding value in cTransparentColor + cTolerance.
//
// 声明: HRGN BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor = 0x00ff00ff, COLORREF cTolerance = 0x00ff00ff, const LPRECT prcSrounce = 0)
HRGN WINAPI BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance, const LPRECT prcSrounce)
{
	HRGN hRgn = NULL;

	if (hBmp)
	{
		// Create a memory DC inside which we will scan the bitmap content
		HDC hMemDC = CreateCompatibleDC(NULL);
		if (hMemDC)
		{
			// Get bitmap size
			BITMAP bm;
			GetObject(hBmp, sizeof(bm), &bm);

			// Create a 32 bits depth bitmap and select it into the memory DC 
			BITMAPINFOHEADER RGB32BITSBITMAPINFO = {	
					sizeof(BITMAPINFOHEADER),	// biSize 
					bm.bmWidth,					// biWidth; 
					bm.bmHeight,				// biHeight; 
					1,							// biPlanes; 
					32,							// biBitCount 
					BI_RGB,						// biCompression; 
					0,							// biSizeImage; 
					0,							// biXPelsPerMeter; 
					0,							// biYPelsPerMeter; 
					0,							// biClrUsed; 
					0							// biClrImportant; 
			};
			VOID * pbits32; 
			HBITMAP hbm32 = CreateDIBSection(hMemDC, (BITMAPINFO *)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pbits32, NULL, 0);
			if (hbm32)
			{
				HBITMAP holdBmp = (HBITMAP)SelectObject(hMemDC, hbm32);

				// Create a DC just to copy the bitmap into the memory DC
				HDC hDC = CreateCompatibleDC(hMemDC);
				if (hDC)
				{
					// Get how many bytes per row we have for the bitmap bits (rounded up to 32 bits)
					BITMAP bm32;
					GetObject(hbm32, sizeof(bm32), &bm32);
					while (bm32.bmWidthBytes % 4)
						bm32.bmWidthBytes++;

					// Copy the bitmap into the memory DC
					HBITMAP holdBmp = (HBITMAP)SelectObject(hDC, hBmp);

					RECT rcSource; // 需要分析的 bitmap 的区域
					if(prcSrounce) 
					{
						rcSource = *prcSrounce;
						// TODO: 这是比较恶劣的做法
						bm.bmWidth = prcSrounce->right - prcSrounce->left;
						bm.bmHeight = prcSrounce->bottom - prcSrounce->top;
					}
					else
						SetRect(&rcSource, 0, 0, bm.bmWidth, bm.bmHeight);

					ASSERT( (rcSource.right-rcSource.left) <= bm.bmWidth );
					ASSERT( (rcSource.bottom-rcSource.top) <= bm.bmHeight );

					BitBlt(hMemDC, 0, 0, rcSource.right-rcSource.left, rcSource.bottom-rcSource.top, hDC, rcSource.left, rcSource.top, SRCCOPY);
#if 0
					HDC hdc = ::GetDC(0);
					BitBlt(hdc, 0, 0, rcSource.right-rcSource.left, rcSource.bottom-rcSource.top, hMemDC, 0, 0, SRCCOPY);
					ReleaseDC(0, hdc);
#endif

					// For better performances, we will use the ExtCreateRegion() function to create the
					// region. This function take a RGNDATA structure on entry. We will add rectangles by
					// amount of ALLOC_UNIT number in this structure.
					#define ALLOC_UNIT	100
					DWORD maxRects = ALLOC_UNIT;
					HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects));
					RGNDATA *pData = (RGNDATA *)GlobalLock(hData);
					pData->rdh.dwSize = sizeof(RGNDATAHEADER);
					pData->rdh.iType = RDH_RECTANGLES;
					pData->rdh.nCount = pData->rdh.nRgnSize = 0;
					SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);

					// Keep on hand highest and lowest values for the "transparent" pixels
					BYTE lr = GetRValue(cTransparentColor);
					BYTE lg = GetGValue(cTransparentColor);
					BYTE lb = GetBValue(cTransparentColor);
					BYTE hr = min(0xff, lr + GetRValue(cTolerance));
					BYTE hg = min(0xff, lg + GetGValue(cTolerance));
					BYTE hb = min(0xff, lb + GetBValue(cTolerance));

					// Scan each bitmap row from bottom to top (the bitmap is inverted vertically)
					BYTE *p32 = (BYTE *)bm32.bmBits + (bm32.bmHeight - 1) * bm32.bmWidthBytes;
					for (int y = 0; y < bm.bmHeight; y++)
					{
						// Scan each bitmap pixel from left to right
						for (int x = 0; x < bm.bmWidth; x++)
						{
							// Search for a continuous range of "non transparent pixels"
							int x0 = x;
							LONG *p = (LONG *)p32 + x;
							while (x < bm.bmWidth)
							{
								BYTE b = GetRValue(*p);
								if (b >= lr && b <= hr)
								{
									b = GetGValue(*p);
									if (b >= lg && b <= hg)
									{
										b = GetBValue(*p);
										if (b >= lb && b <= hb)
											// This pixel is "transparent"
											break;
									}
								}
								p++;
								x++;
							}

							if (x > x0)
							{
								// Add the pixels (x0, y) to (x, y+1) as a new rectangle in the region
								if (pData->rdh.nCount >= maxRects)
								{
									GlobalUnlock(hData);
									maxRects += ALLOC_UNIT;
									hData = GlobalReAlloc(hData, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), GMEM_MOVEABLE);
									pData = (RGNDATA *)GlobalLock(hData);
								}
								RECT *pr = (RECT *)&pData->Buffer;
								SetRect(&pr[pData->rdh.nCount], x0, y, x, y+1);
								if (x0 < pData->rdh.rcBound.left)
									pData->rdh.rcBound.left = x0;
								if (y < pData->rdh.rcBound.top)
									pData->rdh.rcBound.top = y;
								if (x > pData->rdh.rcBound.right)
									pData->rdh.rcBound.right = x;
								if (y+1 > pData->rdh.rcBound.bottom)
									pData->rdh.rcBound.bottom = y+1;
								pData->rdh.nCount++;

								// On Windows98, ExtCreateRegion() may fail if the number of rectangles is too
								// large (ie: > 4000). Therefore, we have to create the region by multiple steps.
								if (pData->rdh.nCount == 2000)
								{
									HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
									if (hRgn)
									{
										CombineRgn(hRgn, hRgn, h, RGN_OR);
										DeleteObject(h);
									}
									else
										hRgn = h;
									pData->rdh.nCount = 0;
									SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
								}
							}
						}

						// Go to next row (remember, the bitmap is inverted vertically)
						p32 -= bm32.bmWidthBytes;
					}

					// Create or extend the region with the remaining rectangles
					HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
					if (hRgn)
					{
						CombineRgn(hRgn, hRgn, h, RGN_OR);
						DeleteObject(h);
					}
					else
						hRgn = h;

					// Clean up
					SelectObject(hDC, holdBmp);
					DeleteDC(hDC);
				}

				DeleteObject(SelectObject(hMemDC, holdBmp));
			}

			DeleteDC(hMemDC);
		}
	}

	return hRgn;
}

#if 0
void TRACERECT(const char* sz, LPRECT lprc)
{
	TRACE( "%s: %d.%d %d.%d\n", sz, lprc->left, lprc->top, lprc->right, lprc->bottom );
}
#endif

// 缩放 Region, SIZE 是缩放后的尺寸, 
// TODO: BUG 如果距离边缘少于5个像素才正确,应该智能的判断是靠哪个边
HRGN WINAPI RegulateRegion(HRGN hrgn, const SIZE* pszDest)
{
	ASSERT( hrgn && pszDest );

	RECT rcSrc;
	int nRet = ::GetRgnBox( hrgn, &rcSrc );
	ASSERT( nRet );

	const SIZE szSrc = {rcSrc.right-rcSrc.left, rcSrc.bottom-rcSrc.top};

	// 快速返回, 要返回复制的 Region, 不能直接返回，BugFix
	if( szSrc.cx == pszDest->cx && szSrc.cy == pszDest->cy )
	{
		HRGN hrgnDiff = CreateRectRgn(0, 0, 0, 0);
		ASSERT( hrgnDiff );
		nRet = ::CombineRgn(hrgnDiff, hrgn, NULL, RGN_COPY);
		ASSERT( ERROR != nRet );
		return hrgnDiff;
	}

	// 1 对 hrgn 取反 hrgnDiff = box - hrgn
	HRGN hrgnBox = CreateRectRgn(0, 0, szSrc.cx, szSrc.cy);
	ASSERT( hrgnBox );

	HRGN hrgnDiff = CreateRectRgn(0, 0, 0, 0);
	ASSERT( hrgnDiff );

	nRet = CombineRgn(hrgnDiff, hrgnBox, hrgn, RGN_DIFF);
	ASSERT( ERROR != nRet );
	
	// 2 对 hrgnDiff 进行缩放
	DWORD dwCount = GetRegionData(hrgnDiff, 0, 0);
	LPRGNDATA prd = (LPRGNDATA)new BYTE[dwCount];
	GetRegionData(hrgnDiff, dwCount, prd);
	RECT* prc = (RECT*)prd->Buffer;

	int y = pszDest->cy - szSrc.cy;
	int x = pszDest->cx - szSrc.cx;

#if 0
	for(int j=0; j<prd->rdh.nCount; j++)
	{
		if( szSrc.cx == prc[j].right && 0 == prc[j].top )
		{
			TRACERECT( "right, top", &prc[j] );
		}
	}
#endif
	for(unsigned int i=0; i<prd->rdh.nCount; i++)
	{
		// 所有右边的
		if( prc[i].right == szSrc.cx )
		{
			prc[i].left += x;
			prc[i].right += x;

			// 右下角
			if( szSrc.cy - prc[i].bottom < 10 )
			{
				prc[i].top += y;
				prc[i].bottom += y;
			}
		}
		// 左下角, BUGFIX: prc[i].top > 5 这个条件
		else if( prc[i].left == 0 && prc[i].top > 10  ) //szSrc.cy - prc[i].bottom < 5 && prc[i].top > 5 )
		{
			prc[i].top += y;
			prc[i].bottom += y;
		}
	}
#if 0
	for(i=0; i<prd->rdh.nCount; i++)
	{
		if( 0 == prc[i].left && 0 == prc[i].top )
		{
			// TRACERECT( "after: left, top", &prc[i] );
		}
		else if( pszDest->cx == prc[i].right && 0 == prc[i].top )
		{
			TRACERECT( "after: right, top", &prc[i] );
		}
	}
#endif

	// TODO: hrgn 是否产生泄露呢？

	// 3 生成放大后的 Region (排除的部分)
	hrgn = ExtCreateRegion(NULL, dwCount, prd);
	ASSERT( hrgn );

	delete [] prd;

	// 4 再排除 hrgnDiff = box - hrgn
	SetRectRgn(hrgnBox, 0, 0, pszDest->cx, pszDest->cy);
	SetRectRgn(hrgnDiff, 0, 0, 0, 0);
	nRet = CombineRgn(hrgnDiff, hrgnBox, hrgn, RGN_DIFF);
	ASSERT( ERROR != nRet );

	nRet = DeleteObject( hrgnBox );
	ASSERT( nRet );

	// 不应该释放哟
	nRet = DeleteObject( hrgn );
	ASSERT( nRet );

	return hrgnDiff;
}

BOOL WINAPI TransparentBlt2(HDC hdcDest,int nXOriginDest,int nYOriginDest,int nWidthDest,int nHeightDest,
					   HDC hdcSrc,int nXOriginSrc,int nYOriginSrc,int nWidthSrc,int nHeightSrc,COLORREF clrTransparent)
{
	BOOL bRet = FALSE;
	HDC hdcTemp = NULL,hdcMask = NULL;
	HBITMAP hbmpTemp = NULL,hbmpMask = NULL;

	if (!hdcDest || !hdcSrc)
		return FALSE;
	if (nWidthDest <= 0 || nHeightDest <= 0 || nWidthSrc <= 0 || nHeightSrc <= 0)
		return FALSE;

	//make a copy for the transparent region;
	hdcTemp = ::CreateCompatibleDC(hdcDest);
	if (!hdcTemp)
		return FALSE;

	hbmpTemp = ::CreateCompatibleBitmap(hdcDest,nWidthDest,nHeightDest);
	if (!hbmpTemp)
		goto ResourceClean;

	::SelectObject(hdcTemp,hbmpTemp);

	if (nWidthDest != nWidthSrc || nHeightDest != nHeightSrc)
	{
		::SetStretchBltMode(hdcTemp,COLORONCOLOR);
		::StretchBlt(hdcTemp,0,0,nWidthDest,nHeightDest,
			hdcSrc,nXOriginSrc,nYOriginSrc,nWidthSrc,nHeightSrc,SRCCOPY);
	}
	else
	{
		::BitBlt(hdcTemp,0,0,nWidthDest,nHeightDest,
			hdcSrc,nXOriginSrc,nYOriginSrc,SRCCOPY);
	}

	//create a mask bitmap;
	hdcMask = ::CreateCompatibleDC(hdcDest);
	if (!hdcMask)
		goto ResourceClean;

	hbmpMask = ::CreateBitmap(nWidthDest,nHeightDest,1,1,NULL);
	if (!hbmpMask)
		goto ResourceClean;

	::SelectObject(hdcMask,hbmpMask);

	::SetBkColor(hdcTemp,clrTransparent);
	::BitBlt(hdcMask,0,0,nWidthDest,nHeightDest,hdcTemp,0,0,SRCCOPY);

	//Transparent bitblt;
	::SetBkColor(hdcTemp,RGB(0,0,0));
	::SetTextColor(hdcTemp,RGB(255,255,255));
	::BitBlt(hdcTemp,0, 0, nWidthDest, nHeightDest, hdcMask, 0, 0, SRCAND);

	COLORREF clrOldBk,clrOldText;

	clrOldBk = ::SetBkColor(hdcDest,RGB(255,255,255));
	clrOldText = ::SetTextColor(hdcDest,RGB(0,0,0));
	::BitBlt(hdcDest,nXOriginDest,nYOriginDest, nWidthDest, nHeightDest, hdcMask, 0, 0, SRCAND);
	::SetTextColor(hdcDest,clrOldText);
	::SetBkColor(hdcDest,clrOldBk);
	
	::BitBlt(hdcDest,nXOriginDest,nYOriginDest, nWidthDest, nHeightDest, 
		hdcTemp, 0, 0, SRCPAINT);

	bRet = TRUE;

ResourceClean:

	::DeleteDC(hdcTemp);
	::DeleteObject(hbmpTemp);

	::DeleteDC(hdcMask);
	::DeleteObject(hbmpMask);

	return bRet;
}

} // namespace Skin