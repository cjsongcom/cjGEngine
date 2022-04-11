
#include "cjEnginePCH.h"
#include "cjShaderDef.h"


//
// cjShaderMtlDesc
//

cjShaderMtlDesc::cjShaderMtlDesc()
{
	cjMemSet(m_descFlag, 0, sizeof(m_descFlag));
}

cjShaderMtlDesc::cjShaderMtlDesc(const cjShaderMtlDesc& rhs)
{
	*this = rhs;
}

cjBool cjShaderMtlDesc::GetFlagString(cjNameChar* buf, cjUINT32 bufSiz) const
{
	cjSprintf(buf, bufSiz, _CT("%.8X%.8X%.8X"), m_descFlag[0], m_descFlag[1], m_descFlag[2]);

	return cjTrue;
}

cjShaderMtlDesc& cjShaderMtlDesc::operator= (const cjShaderMtlDesc& rhs)
{
	if(this == &rhs)
		return *this;

	m_shMtlClsName = rhs.m_shMtlClsName;
	cjMemCpy(m_descFlag, rhs.m_descFlag, sizeof(m_descFlag));

    return *this;
}

bool cjShaderMtlDesc::operator== (const cjShaderMtlDesc& rhs) const
{
	if(this == &rhs)
		return cjTrue;
		
	if(m_shMtlClsName != rhs.m_shMtlClsName)
		return cjFalse;

	if(cjMemCmp(m_descFlag, rhs.m_descFlag, sizeof(m_descFlag)))
		return cjFalse;

    return cjTrue;
}


//
// cjShaderProgramDesc
//

cjShaderProgramDesc::cjShaderProgramDesc()
{
	m_shMtlClsName.SetNone();
	cjMemSet(m_descFlag, 0, sizeof(m_descFlag));
}

cjShaderProgramDesc::cjShaderProgramDesc(const cjShaderProgramDesc& rhs)
{
	*this = rhs;
}

const cjStringA::T* cjShaderProgramDesc::GetFlagStringA(cjStringA& outFlagStr) const
{
	outFlagStr.ReSize(36);
	cjSprintf(&outFlagStr(0), 36, "%0.8X-%0.8X-%0.8X", m_descFlag[0], m_descFlag[1], m_descFlag[2]);

	return *outFlagStr;
}

cjShaderProgramDesc& cjShaderProgramDesc::operator=(const cjShaderProgramDesc& rhs)
{
	if(this == &rhs)
		return *this;

	m_shMtlClsName = rhs.m_shMtlClsName;
	cjMemCpy(m_descFlag, rhs.m_descFlag, sizeof(m_descFlag));

    return *this;
}

bool cjShaderProgramDesc::operator== (const cjShaderProgramDesc& rhs) const
{
	if(this == &rhs)
		return cjTrue;
		
	if(m_shMtlClsName != rhs.m_shMtlClsName)
		return cjFalse;

	if(cjMemCmp(m_descFlag, rhs.m_descFlag, sizeof(m_descFlag)))
		return cjFalse;

    return cjTrue;
}

