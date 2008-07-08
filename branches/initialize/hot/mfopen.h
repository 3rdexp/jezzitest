// mfopen.h: interface for the mfopen class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MFOPEN_H__0395BC5D_C869_4105_ADE9_841B081639BB__INCLUDED_)
#define AFX_MFOPEN_H__0395BC5D_C869_4105_ADE9_841B081639BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <string>

// 打开多个文件
std::vector<std::string> MultiOpenFile(bool filter = true);

std::vector<std::string> OpenMediaDirImpl(bool filter = true);

#endif // !defined(AFX_MFOPEN_H__0395BC5D_C869_4105_ADE9_841B081639BB__INCLUDED_)
