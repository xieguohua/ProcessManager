/*************************************************
//  File name:       BkLayoutUtil.h
//  Date:              2020/11/10
//  <author>      hexujun@cmcm.com
//  <time>          2020/11/10
//   <version>     1.0    
*************************************************/
#ifndef _BkLayoutUtil_h_
#define _BkLayoutUtil_h_

class TiXmlElement;
class TiXmlDocument;

class BkLayoutUtil 
{
public:
	BkLayoutUtil(UINT uResLayoutID);
	BkLayoutUtil();
	~BkLayoutUtil();

	void Load(UINT uResLayoutID);
	TiXmlElement* GetRootTiXmlElement();
	bool Validate();
	operator TiXmlElement*();

private:
	void Free();
private:
	TiXmlDocument* m_pXmlDoc;

};

#endif