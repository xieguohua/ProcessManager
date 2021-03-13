#include "stdafx.h"
#include "BkLayoutUtil.h"
#include "tinyxml\tinyxml.h"
#include "bkres\bkresutil.h"

BkLayoutUtil::BkLayoutUtil() : m_pXmlDoc(NULL)
{

}

BkLayoutUtil::BkLayoutUtil(UINT uResLayoutID)  : m_pXmlDoc(NULL)
{
	Load(uResLayoutID);
}

BkLayoutUtil::~BkLayoutUtil()
{
	Free();
}

void BkLayoutUtil::Load(UINT uResLayoutID)
{
	Free();

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
		return;
	}
}

TiXmlElement* BkLayoutUtil::GetRootTiXmlElement()
{
	if(m_pXmlDoc != NULL) {
		return m_pXmlDoc->RootElement();
	}

	return NULL;
}

bool BkLayoutUtil::Validate()
{
	return GetRootTiXmlElement() != NULL;
}

BkLayoutUtil::operator TiXmlElement*()
{
	return GetRootTiXmlElement();
}

void BkLayoutUtil::Free()
{
	if (m_pXmlDoc) {
		delete m_pXmlDoc;
		m_pXmlDoc = NULL;
	}
}
