#include "stdafx.h"

#include <stdlib.h> // strtol
#define file_parse_test
#ifdef file_parse_test
  #include <iostream> // for test
  #include <iomanip> // for test
#endif // file_parse_test

//


#include "SkinScheme.h"

using namespace ATL; // 解决CComPtr c2501的问题


//#include <atlctrls.h>
//#include "skinhook.h"

#ifdef _DEBUG
	// #define PERF_DEBUG
	#ifdef PERF_DEBUG
		#include "PreciseCounter.h"
	#endif
#endif

namespace Skin
{
#if 0
//////////////////////////////////////////////////////////////////////////////////
// helper function: read xml node attribute value
static std::string _getXMLAttribute(IXMLDOMNode* p, BSTR bsAttributeName)
{
	HRESULT hr = E_FAIL;
	CComPtr<IXMLDOMNode> spSrcNode(p);
	CComPtr<IXMLDOMNamedNodeMap> spAttr;
	hr = spSrcNode->get_attributes(&spAttr);
	ATLASSERT( SUCCEEDED(hr) && spAttr && "get_attributes" );

	CComPtr<IXMLDOMNode> spAttrNode;
	hr = spAttr->getNamedItem(bsAttributeName, &spAttrNode);
	// ATLASSERT( SUCCEEDED(hr) && spAttrNode && "getNamedItem" );

#ifdef _DEBUG
	if( !spAttrNode)
	{
		long count = 0;
		hr = spAttr->get_length(&count);
		for(long i=0; i<count; i++)
		{
			spAttr->get_item(i, &spAttrNode);
			CComBSTR bsXML;
			spAttrNode->get_xml(&bsXML);
			spAttrNode.Release();
		}
	}
#endif

	if( spAttrNode )
	{
		CComVariant var;
		hr = spAttrNode->get_nodeValue(&var);
		ATLASSERT( SUCCEEDED(hr) && "get_nodeValue" );

		USES_CONVERSION;
		if( VT_BSTR == var.vt )
			return std::string(W2A(var.bstrVal));
	}

	return std::string("");
}

unsigned long _getXMLAttributeULong(IXMLDOMNode* p, BSTR bsAttributeName, int base = 10)
{
	std::string strTmp = _getXMLAttribute(p, bsAttributeName);
	if( 0 == strTmp.length() ) return 0;

	unsigned long ulRet = strtoul(strTmp.c_str(), 0, base);
	return ulRet;
}

long _getXMLAttributeLong(IXMLDOMNode* p, BSTR bsAttributeName)
{
	return (long)_getXMLAttributeULong(p, bsAttributeName);
}

std::string _getXMLNodeName(IXMLDOMNode* p)
{
	USES_CONVERSION;

	CComBSTR bs;
	p->get_nodeName(&bs);
	return std::string(W2A(bs));
}

// try to release msxml?.dll, decrease process used memory
static void _FreeXmlDll()
{
}


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


BOOL SchemeData::LoadConfig(const char* pszConfigFile)
{
	// 1 Scheme 基本信息
	CComPtr<IXMLDOMDocument> spDom;
	HRESULT hr = spDom.CoCreateInstance(CLSID_DOMDocument);
	ASSERT( SUCCEEDED(hr) && "CoCreateInstacen IXMLDOMDocument" );
	if( FAILED(hr) ) return FALSE;

	USES_CONVERSION;
	VARIANT_BOOL vbool = VARIANT_FALSE;
	hr = spDom->load( CComVariant(A2W(pszConfigFile)), &vbool );
	ASSERT( SUCCEEDED(hr) && "IXMLDOMDocument::load" );
	if( FAILED(hr) || (VARIANT_FALSE == vbool) ) return FALSE;

	CComPtr<IXMLDOMElement> spElement;
	CComBSTR bsTemp;
	hr = spDom->get_documentElement(&spElement);
	ATLASSERT( SUCCEEDED(hr) && "get_documentElement" );
	
	// IXMLDOMElement inherits from IXMLDOMNode
	_name = _getXMLAttribute(spElement, L"name");
	_author = _getXMLAttribute(spElement, L"author");
	_copyright = _getXMLAttribute(spElement, L"copyright");
	_bgFile = _getXMLAttribute(spElement, L"image_file");
	COLORREF _transcolor = _getXMLAttributeULong(spElement, L"transcolor", 16);

	CComObject<DCCache> * pCache = 0;
	CComObject<DCCache>::CreateInstance(&pCache);
	if (pCache)
	{
		// pCache->QueryInterface(IID_IDCCache, (void**)&_spCache);
		HBITMAP h = LoadImage(_bgFile.c_str());
		pCache->SetBitmap(h, _transcolor);

#if 0
		HDC d = ::GetDC(0);
		BitBlt(d, 0, 0, 100, 100, pCache->DC(), 0, 0, SRCCOPY);
		::ReleaseDC(0, d);
#endif
	}

	// TODO: pCache AddRef

	// @@@@@@@@ style
	CComPtr<IXMLDOMNodeList> spStyleList;
	hr = spElement->selectNodes(L"//theme/style", &spStyleList);
	ATLASSERT( SUCCEEDED(hr) && "get style nodes" );

	CComPtr<IXMLDOMNode> spndstyle;	
    while( SUCCEEDED(spStyleList->nextNode(&spndstyle)) && spndstyle )
	{
		std::string style_name = _getXMLAttribute(spndstyle, L"name");

		CComObject<SkinScheme> * pss = 0;
		CComObject<SkinScheme>::CreateInstance(&pss);
		ASSERT(pss);

		// @@@@@@@@ class
		CComPtr<IXMLDOMNodeList> spControlList;
		hr = spndstyle->selectNodes(L"class", &spControlList);
		
		CComPtr<IXMLDOMNode> spndcontrol;
		while( SUCCEEDED(spControlList->nextNode(&spndcontrol)) && spndcontrol)
		{
            int ti = _getXMLAttributeLong(spndcontrol, L"id");
            const int class_id = 1 << ti;

            // @@@@@@@@ part
            CComPtr<IXMLDOMNodeList> spPartList;
            hr = spndcontrol->selectNodes(L"part", &spPartList);

            CComPtr<IXMLDOMNode> spndpart;
            while( SUCCEEDED(spPartList->nextNode(&spndpart)) && spndpart)
            {
    			const int part_id = _getXMLAttributeLong(spndpart, L"id");

		        // @@@@@@@@ area / color / font ...
		        CComPtr<IXMLDOMNodeList> spValueList;
		        hr = spndpart->get_childNodes(&spValueList);
		        ATLASSERT( SUCCEEDED(hr) && "get area or color or ... list" );

			    CComPtr<IXMLDOMNode> spndvalue;

			    while( SUCCEEDED(spValueList->nextNode(&spndvalue)) && spndvalue)
			    {
				    DOMNodeType type;
				    hr = spndvalue->get_nodeType(&type); // 不对注释等处理
				    ASSERT( SUCCEEDED(hr) );
				    if( NODE_ELEMENT == type )
				    {
					    const int state_id = _getXMLAttributeLong(spndvalue, L"state");

					    std::string name = _getXMLNodeName(spndvalue);
					    if( name == "area" )
					    {
						    std::string strrc = _getXMLAttribute(spndvalue, L"rc");
						    RECT rc;
						    if( strrc.length() )
						    {
							    typedef std::vector< std::string > split_vector_type;

							    split_vector_type SplitVec;
							    boost::split( SplitVec, strrc, boost::is_any_of(", ") );

							    if(SplitVec.size() == 4)
							    {
								    rc.left = boost::lexical_cast<int>(SplitVec[0].c_str());
								    rc.top = boost::lexical_cast<int>(SplitVec[1].c_str());
								    rc.right = boost::lexical_cast<int>(SplitVec[2].c_str());
								    rc.bottom = boost::lexical_cast<int>(SplitVec[3].c_str());
							    }
						    }
						    else
						    {
							    rc.left = _getXMLAttributeLong(spndvalue, L"left");
							    rc.top = _getXMLAttributeLong(spndvalue, L"top");
							    rc.right = _getXMLAttributeLong(spndvalue, L"right");
							    rc.bottom = _getXMLAttributeLong(spndvalue, L"bottom");
						    }

						    //config_item._areas.insert( 
						    //	std::make_pair( make_key(part_id, state_id), rc )
						    //	);
						    SkinArea area;
						    area.rc = rc;
						    area.step_horz = _getXMLAttributeLong(spndvalue, L"step_horz");
						    area.step_vert = _getXMLAttributeLong(spndvalue, L"step_vert");
						    pss->add(make_config_key(class_id, part_id, state_id, 0), area); // TODO:
					    }
					    else if( name == "color" )
					    {
						    const int prop_id = _getXMLAttributeLong(spndvalue, L"prop");

						    std::string str = _getXMLAttribute(spndvalue, L"value");
						    COLORREF v = 0xff000000;
						    char * pend;
						    if(!str.empty())
						    {
							    if( str[0] == '#' )
								    v = strtol(str.c_str() + 1, &pend, 16);
							    else if( str.length() >= 2 && str[0] == '0' && (str[1] == 'x') ||(str[1] == 'X') )
							        v = strtol(str.c_str() + 2, &pend, 16);
							    else
								    v = strtol(str.c_str(), &pend, 10);
						    }
						    if (v != 0xff000000)
							    pss->add(make_config_key(class_id, part_id, state_id, prop_id), v);
					    }
				    }

    				spndvalue.Release();
                }

                spndpart.Release();
			}

			spndcontrol.Release();
		}

		if (pss && pss->Size())
		{
			pss->SetDCCache(pCache);

			CComPtr<ISkinScheme> spss;
			pss->QueryInterface(IID_ISkinScheme, (void**)&spss);
			if(spss)
				Add(style_name, pss);
		}
		
		spndstyle.Release();
	}

#ifdef file_parse_test
	for(scheme_map::iterator j = _schemes.begin(); j!=_schemes.end(); j++)
	{
		std::cout << j->first << " \t" << std::endl;
		j->second.m_T->Dump();
		std::cout << std::endl;
	}
#endif
	
	// 3 最后必须释放 XML DOM 指针,更新成员
	_configFile = pszConfigFile;

	scheme_map::iterator i = _schemes.lower_bound(std::string("default"));
	if (i!=_schemes.end())
	{
		_spDefaultScheme = i->second.m_T;
	}
		
	return TRUE;
}
#endif

//



} // namespace Skin




