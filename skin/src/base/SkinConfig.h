// SkinConfig.h: Implement interface ISkinConfig
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "skinitf.h"
#include "skiniid.h"
#include <map>

namespace SkinDefault
{

// @ 能够增长的ISkinConfig的实现
class GrowConfig : public CComObjectRoot
	, public ISkinConfig  
{
public:
	typedef GrowConfig thisType;
	BEGIN_COM_MAP(thisType)
		COM_INTERFACE_ENTRY_IID(IID_ISkinConfig, ISkinConfig)
	END_COM_MAP()


	STDMETHOD(SetCustomConfig)(ISkinConfig* pConfig)
	{
		_spCustomConfig = pConfig;
		return S_OK;
	}

	STDMETHOD_(LPCSTR, GetName)(int iPartId, int iStateId, BOOL* fUseDefault)
	{
		//1 find in _nameMap
		DWORD nID = (DWORD)MAKELONG(iPartId, iStateId);
		NAME_CONST_ITERATOR it = _nameMap.find(nID);
		if( it != _nameMap.end() )
		{
			return it->second.c_str();
		}
		//2 find in customConfig;
		if (_spCustomConfig)
		{
			return _spCustomConfig->GetName(iPartId,iStateId,fUseDefault);
		}
		
		SKINCONFIG_USE_DEFAULT(fUseDefault);
		return 0;
	}

	STDMETHOD(AddItem)(int iPartId, int iStateId, LPCSTR szItemName)
	{
		DWORD nID = (DWORD)MAKELONG(iPartId, iStateId);
		NAME_ITERATOR it = _nameMap.find(nID);
		if( it != _nameMap.end() )
		{
			_nameMap.erase(it);
		}
		_nameMap.insert(std::make_pair(nID, szItemName));
		return S_OK;
	}


	STDMETHOD_(LPCSTR, GetColor)(int iPartId, int iStateId, BOOL* fUseDefault)
	{
		//1 find in _nameMap
		DWORD nID = (DWORD)MAKELONG(iPartId, iStateId);
		NAME_CONST_ITERATOR it = _colorMap.find(nID);
		if( it != _colorMap.end() )
		{
			return it->second.c_str();
		}
		//2 find in customConfig;
		if (_spCustomConfig)
		{
			return _spCustomConfig->GetColor(iPartId,iStateId,fUseDefault);
		}

		SKINCONFIG_USE_DEFAULT(fUseDefault);
		return 0;		
	}

	STDMETHOD(AddColor)(int iPartId, int iStateId, LPCSTR szItemName)
	{
		DWORD nID = (DWORD)MAKELONG(iPartId, iStateId);
		NAME_ITERATOR it = _colorMap.find(nID);
		if( it != _colorMap.end() )
		{
			_colorMap.erase(it);
		}
		_colorMap.insert(std::make_pair(nID, szItemName));
		return S_OK;
	}
private:
	CComPtr<ISkinConfig> _spCustomConfig;
	typedef std::map<DWORD, std::string>::const_iterator NAME_CONST_ITERATOR;
	typedef std::map<DWORD, std::string>::iterator		 NAME_ITERATOR;
	std::map<DWORD, std::string> _nameMap;	
	std::map<DWORD, std::string> _colorMap;

};

}; // namespace SkinDefault