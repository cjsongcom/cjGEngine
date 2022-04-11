
#include "cjEnginePCH.h"
#include "cjShaderRes.h"


//
// cjShaderRes
//

CJOBJ_IMPL_CLASS(cjShaderRes)

CJOBJ_IMPL_SERIALIZE_BEGIN(cjShaderRes)
	
	if(stream.IsObjOnly())
		return;

	if(stream.IsLoading() || stream.IsSaving())
	{
	}	

	// m_name, m_vtxShader, m_pxlShader is handled by objproperty

CJOBJ_IMPL_SERIALIZE_END(cjShaderRes)

CJOBJ_IMPL_STATIC_REG_BEGIN(cjShaderRes)

	//cjAryProperty*  m_shaderNamesnameAry = new(GetCls(), _CT("names"), 0) cjAryProperty(CJ_PROP_NATIVE_OFF(m_name), 0, 0);

	//cjNameProperty* beforeProp = new(GetCls(), _CT("before"), 0) cjNameProperty(CJ_PROP_NATIVE_OFF(m_before), 0, 0);

	//cjAryProperty* nameAry = new(GetCls(), _CT("names"), 0) cjAryProperty(CJ_PROP_NATIVE_OFF(m_name), 0, 0);
	//	nameAry->m_aryItem = new(nameAry, _CT("nameProp0"), 0) cjStrProperty();

	//cjNameProperty* afterProp = new(GetCls(), _CT("after"), 0) cjNameProperty(CJ_PROP_NATIVE_OFF(m_after), 0, 0);

	//cjAryProperty* testNameAry = new(GetCls(), _CT("testnames"), 0) cjAryProperty(CJ_PROP_NATIVE_OFF(m_testAry), 0, 0);
	//	testNameAry->m_aryItem = new(testNameAry, _CT("testnameProp0"), 0) cjNameProperty();


CJOBJ_IMPL_STATIC_REG_END(cjShaderRes)


cjShaderRes::cjShaderRes()
{
}

void cjShaderRes::OnDestroy()
{
	cjSuper::OnDestroy();
}

