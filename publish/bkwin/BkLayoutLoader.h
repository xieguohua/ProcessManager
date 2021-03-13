/*************************************************
//  File name:       BkLayoutLoader.h
//  Date:              2020/12/04
//  <author>      hexujun@cmcm.com
//  <time>          2020/12/04
//   <version>     1.0    
//  布局资源加载器: 
*************************************************/
#ifndef _BkWin_BkLayoutLoader_h_
#define _BkWin_BkLayoutLoader_h_

#include "tinyxml\tinyxml.h"
#include "bkres\bkresutil.h"

namespace BkWin {\

class BkLayoutLoader 
{
public:
	BkLayoutLoader(UINT uResLayoutID)
	{
		m_pXmlDoc = NULL;
		Load(uResLayoutID);
	}

	BkLayoutLoader()
	{
		m_pXmlDoc = NULL;
	}

	~BkLayoutLoader() {
		Free();
	}

	void Load(UINT uResLayoutID)
	{
		Free();
		if(!uResLayoutID) {
			return;
		}

		CStringA strXml;
		BkResManager::LoadResource(uResLayoutID, strXml);
		if (strXml.IsEmpty()) {
			return;
		}

		m_pXmlDoc = new TiXmlDocument();
		m_pXmlDoc->Parse(strXml, NULL, TIXML_ENCODING_UTF8);
		if (m_pXmlDoc->Error())
		{
			Free();
		}
	}

	TiXmlElement* GetRootTiXmlElement() {
		return m_pXmlDoc ? m_pXmlDoc->RootElement() : NULL;
	}

	bool Validate() {
		return GetRootTiXmlElement() != NULL;
	}

	operator TiXmlElement*() {
		return GetRootTiXmlElement();
	}

	operator bool () {
		return Validate();
	}

private:
	void Free() {
		if (m_pXmlDoc) {
			delete m_pXmlDoc;
			m_pXmlDoc = NULL;
		}
	}
private:
	TiXmlDocument* m_pXmlDoc;

};

}

#endif