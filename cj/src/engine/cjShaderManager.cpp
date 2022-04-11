
#include "cjEnginePCH.h"
#include "cjShaderManager.h"

#include "cjGLShaderManager.h"

//
// cjShaderManager
//

CJSER_IMPL_CLASS(cjShaderManager, cjRef)

cjShaderManagerRPtr cjShaderManager::ms_shaderManager;


cjShaderManager::cjShaderManager()
{
}

cjShaderManager::~cjShaderManager()
{
	PurgeCache();
}

cjBool cjShaderManager::StaticInit()
{
	cjAssert(!ms_shaderManager);

	if(!ms_shaderManager)
		ms_shaderManager = new cjGLShaderManager;

	return cjTrue;
}

void cjShaderManager::StaticShutdown()
{
	if(!ms_shaderManager)
		return;

	cjAssert(ms_shaderManager->GetRef() == 1);
	ms_shaderManager = cjNULL;	
}

cjShaderManager::ShaderContainerHash::Pair* cjShaderManager::FindShaderContainer(
	const cjName& mtlClsName, cjBool createAtNone)
{
	ShaderContainerHash::Pair* containerPair = m_shaders.Find(mtlClsName);

	if(!containerPair && createAtNone)
	{
		ShaderContainerHash::Val v;
		containerPair = m_shaders.Insert(mtlClsName, v);
	}

	return containerPair;
}

cjShader* cjShaderManager::FindShader(const cjName& mtlClsName, const cjShaderMtlDesc& desc,
	cjBool* isFailedShader)
{
	if(isFailedShader)
		*isFailedShader = cjFalse;

	ShaderContainerHash::Pair* containerPair = FindShaderContainer(mtlClsName, cjFalse);
	
	if(!containerPair)
		return cjNULL;

	ShaderContainer& container = containerPair->m_val;
	MtlDesc2Shader::Pair* shaderCachePair = container.m_mtlDesc2Shader.Find(desc.GetHashKey());
	cjShader* shader = cjNULL;
		
	if(shaderCachePair)
	{
		shader = shaderCachePair->m_val.m_cachedShader;

		if(!shader && isFailedShader)
		{
			// check failed shader
			if(container.m_mtlDesc2FailedShader.Find(desc.GetHashKey()))
				*isFailedShader = cjTrue;
		}
	}

	return shader;
}

void cjShaderManager::AddShaderCache(const cjName& mtlClsName, cjShader* shader)
{
	ShaderContainerHash::Pair* containerPair = FindShaderContainer(mtlClsName, cjTrue);

	MtlDesc2Shader& desc2Shader = containerPair->m_val.m_mtlDesc2Shader;
	const cjShaderMtlDesc* mtlDesc = shader->GetShaderMtlDesc();

	MtlDesc2Shader::Pair* pair = desc2Shader.Find(mtlDesc->GetHashKey());

	if(!pair)
	{
		MtlDesc2Shader::Val v;
		pair = desc2Shader.Insert(mtlDesc->GetHashKey(), v);
	}

	pair->m_val.m_cachedShader = shader;	
}

void cjShaderManager::SetFailedShader(const cjName& mtlClsName, const cjShaderMtlDesc& desc)
{
	ShaderContainerHash::Pair* containerPair = FindShaderContainer(mtlClsName, cjTrue);

	if(!containerPair)
	{
		cjAssert(!"falied to create ShaderContainer..");
		return;
	}
	
	ShaderContainer& container = containerPair->m_val;

	if(container.m_mtlDesc2FailedShader.Find(desc.GetHashKey()))
	{
		cjAssert(!"failed shader container is already inserted..");
	}
	else
	{
		MtlDesc2Shader::Val v;

		//cjopti v is not needed as hash value
		container.m_mtlDesc2FailedShader.Insert(desc.GetHashKey(), v);
	}
}




//cjtemp
//cjBool cjShaderMtlDefault::EvaluateSameShader(cjShader* shader, const cjSceneMesh* mesh, 
//	const cjScenePropRPtrList* props) const
//{
//	if(!shader)
//		return cjFalse;
//
//	cjShaderMtlDesc shMtlDesc;
//
//	BuildShaderMtlDesc(&shMtlDesc, mesh, props);
//
//	if(*(shader->GetShaderMtlDesc()) == shMtlDesc)
//		return cjTrue;
//
//	return cjFalse;
//}
//

//
//
//



