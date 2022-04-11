
#include "cjEnginePCH.h"
#include "cjShader.h"


//
// cjShader
//

CJSER_IMPL_CLASS(cjShader, cjRef)

cjShader::cjShader()
{
}

cjShader::~cjShader()
{
}


//
// cjShaderMtlInstance
//

cjShaderMtlInst::cjShaderMtlInst()
{
	m_dirtyState = EMDS_DIRTY;
}

cjShaderMtlInst::cjShaderMtlInst(const cjShaderMtlInst& rhs)
{
	m_shaderMtl = rhs.m_shaderMtl;
	m_cachedShader = rhs.m_cachedShader;
	m_dirtyState = EMDS_DIRTY;
}

cjShaderMtlInst::cjShaderMtlInst(cjShaderMtl* shaderMtl)
{
	m_shaderMtl = shaderMtl;
	m_dirtyState = EMDS_DIRTY;
}

cjShaderMtlInst::~cjShaderMtlInst()
{
//cjtemp
//	ClearCachedShader();
}

cjShaderMtlInst& cjShaderMtlInst::operator=(const cjShaderMtlInst& shMtlInst)
{
	m_shaderMtl = shMtlInst.m_shaderMtl;
	m_cachedShader = shMtlInst.m_cachedShader;
	m_dirtyState = shMtlInst.m_dirtyState;

	return *this;
}

bool cjShaderMtlInst::operator==(const cjShaderMtlInst& shMtlInst) const
{
	if(m_shaderMtl != shMtlInst.m_shaderMtl) 
		return false;

	if(m_cachedShader != shMtlInst.m_cachedShader)
		return false;

	if(m_dirtyState != shMtlInst.m_dirtyState)
		return false;

	return true;
}

//cjtemp
//
//cjShader* cjShaderMtlInst::EvaluateCachedShader(const cjSceneMesh* mesh, const cjScenePropRPtrList* props) const
//{
//	if(m_cachedShader)
//	{
//		if(  m_dirtyState == EMDS_DIRTY || 
//			(m_dirtyState == EMDS_QUERY && mesh->IsAlwaysUpdateShaderMtl()) )
//		{
//			return cjNULL;
//		}
//
//		if(!m_shaderMtl || m_shaderMtl->EvaluateSameShader(m_cachedShader, mesh, props))
//			return m_cachedShader;
//	}    
//
//	return cjNULL;
//}
//
//cjShader* cjShaderMtlInst::CacheShader(cjSceneMesh* mesh, const cjScenePropRPtrList* props)
//{
//	if(m_shaderMtl)
//	{
//		cjBool buildShader = cjFalse;
//
//		if(m_dirtyState == EMDS_DIRTY) 
//			buildShader = cjTrue;
//
//		if(m_dirtyState == EMDS_QUERY) 
//			buildShader = mesh->IsAlwaysUpdateShaderMtl();
//
//		if(buildShader && m_cachedShader) 
//			buildShader = !m_shaderMtl->EvaluateSameShader(m_cachedShader, mesh, props);
//
//		if(buildShader)
//		{
//			cjShader* newShader = m_shaderMtl->ResolveShader(mesh, props);
//
//			if(newShader)
//			{ 
//				cjAssert(m_cachedShader != newShader);
//
//				ClearCachedShader();
//
//				m_cachedShader = newShader;
//
//				if(!newShader->SetupSceneMesh(mesh, this))
//					ClearCachedShader();
//			} 
//			else
//			{  
//				ClearCachedShader();
//			}
//		}
//
//		m_dirtyState = EMDS_QUERY;
//
//	}  // if(m_spMaterial)
//
//	return m_cachedShader;
//}
//
//void cjShaderMtlInst::ClearCachedShader()
//{
//	m_cachedShader = cjNULL;    
//}

