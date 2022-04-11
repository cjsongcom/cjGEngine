
#include "tinyxml/tinyxml2.h"
#include "tinyxml/cjtTinyXML.h"


class cjtTinyXMLWrap
{
public:
	cjtTinyXMLWrap(const char* path)
	{
		m_doc.LoadFile(path);
	}

	tinyxml2::XMLDocument					m_doc;
};


cjtTinyXML cjtCreateTinyXML()
{
	cjtTinyXMLWrap* wrapper = new cjtTinyXMLWrap("resources/dream.xml");

	return (cjtTinyXML)wrapper;
}

void cjtDestroyTinyXML(cjtTinyXML xml)
{
	if(xml)
		delete (cjtTinyXMLWrap*)xml;
}
