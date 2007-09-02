////////////////////////////////////////////////////////////////
// MSDN Magazine -- October 2001
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual C++ 6.0 for Windows 98 and probably Windows 2000 too.
// Set tabsize = 3 in your editor.
//
#include "StdAfx.h"


#ifndef ASSERT
    #define ASSERT ATLASSERT
#endif
#include "Picture.h"

////////////////////////////////////////////////////////////////
// CPicture implementation
//

CPicture::CPicture()
{
}

CPicture::~CPicture()
{
}

//////////////////
// Load from resource. Looks for "IMAGE" type.
//
#if 0
BOOL CPicture::Load(UINT nIDRes)
{
	// find resource in resource file
	HINSTANCE hInst = _Module.GetResourceInstance();
	HRSRC hRsrc = ::FindResource(hInst,
		MAKEINTRESOURCE(nIDRes),
		"IMAGE"); // type
	if (!hRsrc)
		return FALSE;

	// load resource into memory
	DWORD len = SizeofResource(hInst, hRsrc);
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);
	if (!lpRsrc)
		return FALSE;

	// create memory file and load it
	CMemFile file(lpRsrc, len);
	BOOL bRet = Load(file);
	FreeResource(hRsrc);

	return bRet;
}
#endif

//////////////////
// Load from path name.
//
BOOL CPicture::Load(LPCTSTR pszPathName)
{
    HRESULT hr = OleLoadPicturePath(
        CT2OLE(pszPathName), 0, 0, 0,
        IID_IPicture, (void**)&m_spIPicture);

    return SUCCEEDED(hr);
}

//////////////////
// Load from stream (IStream). This is the one that really does it: call
// OleLoadPicture to do the work.
//
BOOL CPicture::Load(IStream* pstm)
{
	Free();
	HRESULT hr = OleLoadPicture(pstm, 0, FALSE,
		IID_IPicture, (void**)&m_spIPicture);
	ASSERT(SUCCEEDED(hr) && m_spIPicture);	
	return TRUE;
}

//////////////////
// Render to device context. Covert to HIMETRIC for IPicture.
//
BOOL CPicture::Render(HDC hDC, LPRECT prc, LPCRECT prcMFBounds) const
{
	ASSERT(hDC);

    RECT rc;
    if (prc) {
        rc = *prc;
    } else {
        rc.left = rc.top = 0;
        SIZE sz = GetImageSize(hDC);
        rc.right = sz.cx;
        rc.bottom = sz.cy;
    }

	long hmWidth,hmHeight; // HIMETRIC units
	GetHIMETRICSize(hmWidth, hmHeight);
	m_spIPicture->Render(hDC, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
		0, hmHeight, hmWidth, -hmHeight, prcMFBounds);

	return TRUE;
}

//////////////////
// Get image size in pixels. Converts from HIMETRIC to device coords.
//
SIZE CPicture::GetImageSize(HDC hDC) const
{
    SIZE sz;
    sz.cx = 0, sz.cy = 0;
	if (!m_spIPicture)
		return sz;
	
	LONG hmWidth, hmHeight; // HIMETRIC units
	m_spIPicture->get_Width(&hmWidth);
	m_spIPicture->get_Height(&hmHeight);

	sz.cx = hmWidth;
    sz.cy = hmHeight;
	if (hDC==NULL) {
		CWindowDC dc(NULL);
		dc.HIMETRICtoDP(&sz); // convert to pixels
	} else {
        CDCHandle dc(hDC);
		dc.HIMETRICtoDP(&sz);
	}
	return sz;
}

