////////////////////////////////////////////////////////////////
// MSDN Magazine -- October 2001
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual C++ 6.0 for Windows 98 and probably Windows 2000 too.
// Set tabsize = 3 in your editor.
//
#pragma once
#include <atlbase.h>

//////////////////
// Picture object--encapsulates IPicture
//
class CPicture {
public:
	CPicture();
	~CPicture();

	// Load frm various sosurces
	// BOOL Load(UINT nIDRes);
	BOOL Load(LPCTSTR pszPathName);
	BOOL Load(IStream* pstm);

	// render to device context
	BOOL Render(HDC hDC, LPRECT prc = 0, LPCRECT prcMFBounds=NULL) const;

	SIZE GetImageSize(HDC hDC=NULL) const;

	operator IPicture*() {
		return m_spIPicture;
	}

	void GetHIMETRICSize(OLE_XSIZE_HIMETRIC& cx, OLE_YSIZE_HIMETRIC& cy) const {
        HRESULT hr = S_OK;
		cx = cy = 0;
		hr = m_spIPicture->get_Width(&cx);
		ASSERT(SUCCEEDED(hr));
		hr = m_spIPicture->get_Height(&cy);
		ASSERT(SUCCEEDED(hr));
	}

	void Free() {
		if (m_spIPicture) {
			m_spIPicture.Release();
		}
	}

protected:
	CComQIPtr<IPicture> m_spIPicture;		 // ATL smart pointer to IPicture
};
