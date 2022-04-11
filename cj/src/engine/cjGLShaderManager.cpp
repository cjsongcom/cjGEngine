
#include "cjEnginePCH.h"

#include "cjGLShader.h"
#include "cjShaderMtlDesc.h"

#include "cjGLShaderProgSnippet_ShaderMtlDef.h"
#include "cjGLShaderManager.h"



//
// cjGLShaderProgSnippet
//

cjGLShaderProgSnippet::cjGLShaderProgSnippet()
{
}

cjGLShaderProgSnippet::~cjGLShaderProgSnippet()
{
}

cjGLShaderProgSnippet* cjGLShaderProgSnippet::Create(const cjName& mtlName)
{
	if(mtlName == cjName_cjShaderMtlDefault)
		return new cjGLShaderProgSnippet_ShaderMtlDefault;

	return cjNULL;
}

cjGLShaderProgSnippet::VertexProg::VertexProg()
{
	m_vertexShader = cjNULL;
}

cjGLShaderProgSnippet::VertexProg::~VertexProg()
{
	if(m_vertexShader)
		delete m_vertexShader;
}

cjGLShaderProgSnippet::PixelProg::PixelProg()
{
	m_pixelShader = cjNULL;
}

cjGLShaderProgSnippet::PixelProg::~PixelProg()
{
	if(m_pixelShader)
		delete m_pixelShader;
}


//
// cjGLShaderManager
//

CJSER_IMPL_CLASS(cjGLShaderManager, cjShaderManager)


cjGLShaderManager::cjGLShaderManager()
{
}

cjGLShaderManager::~cjGLShaderManager()
{
}

cjShader* cjGLShaderManager::ResolveShader(const cjShaderMtlDesc& shMtlDesc)
{
//cjtemp
	return cjNULL;
}

cjShader* cjGLShaderManager::CreateShaderInstance(const cjShaderMtlDesc* shMtlDesc)
{
	cjGLShader* shader = new cjGLShader;

	if(shMtlDesc)
		shader->SetMtlDesc(shMtlDesc);

	return shader;
}

cjBool cjGLShaderManager::ResolveShaderProgramForRenderPass(cjShader* shader)
{
	if(!shader)
		return cjFalse;

	const cjShaderMtlDesc* shMtlDesc = shader->GetShaderMtlDesc();
	const cjName& mtlName = shMtlDesc->GetShaderMtlClsName();
	
	ProgCache* cache = GetProgCache(mtlName);

	if(!cache)
		return cjFalse;

	cjAryOff renPassCount = shader->GetRenPassCount();
	cjUINT32 validShaderProgCount = 0;

	for(cjAryOff i=0; i<renPassCount; i++)
	{
		const cjShaderProgramDesc* vtxProgDesc = shader->GetVertexShaderProgDesc(i);
		const cjShaderProgramDesc* pxlProgDesc = shader->GetPixelShaderProgDesc(i);

		cjFShaderProgram* shaderProg = GetShaderProgram(cache, vtxProgDesc, pxlProgDesc);
		shader->SetShaderProgram(i, shaderProg);

		if(shaderProg)
			validShaderProgCount++;
		else
			return cjFalse;
	}
	
	return cjTrue;
}

void cjGLShaderManager::PurgeCache()
{
}

cjFShaderProgram* cjGLShaderManager::GetShaderProgram(cjGLShaderManager::ProgCache* cache,
	const cjShaderProgramDesc* vtxProgDesc, const cjShaderProgramDesc* pxlProgDesc)
{
	if(!cache->m_snippet)
		return cjNULL;

	ShaderProgKey key(vtxProgDesc, pxlProgDesc);

	ProgCache::ShaderProgHash::Pair* pair = cache->m_shaderProgHash.Find(key);

	if(pair)
		return pair->m_val;

	cjFVertexShader* vtxShader = cache->m_snippet->ResolveVertexShader(vtxProgDesc);
	cjFPixelShader* pxlShader = cache->m_snippet->ResolvePixelShader(pxlProgDesc);

	if(!vtxShader || !pxlShader)
		return cjNULL;
	
	// last parameter (cjFalse): vtxShader, pxlShader will deleted cache->m_snippet
	cjFShaderProgram* shaderProg = new cjFShaderProgram(vtxShader, pxlShader, cjFalse);
	cache->m_shaderProgHash.Insert(key, shaderProg);
	
	return shaderProg;
}

cjGLShaderManager::ProgCache* cjGLShaderManager::GetProgCache(const cjName& mtlName)
{
	if(!mtlName.IsValid())
		return cjNULL;

	ProgCacheDatHash::Pair* pair = m_progCaches.Find(mtlName);
	
	if(!pair)
	{
		cjGLShaderProgSnippet* snippet = cjGLShaderProgSnippet::Create(mtlName);

		if(!snippet)
			return cjNULL;
		
		ProgCache* v = new ProgCache();

		v->m_shaderMtlClsName = mtlName;
		v->m_snippet = snippet;

		pair = m_progCaches.Insert(mtlName, v);
	}

	return pair->m_val;
}


//
// cjGLShaderManager::ShaderProgKey
//

cjGLShaderManager::ShaderProgKey::ShaderProgKey(const cjShaderProgramDesc* vtxProgDesc, 
	const cjShaderProgramDesc* pxlProgDesc)
{
	vtxProgDesc->Fill(m_hash0);
	pxlProgDesc->Fill(m_hash1);
}


//
// cjGLShaderManager::ProgCache
//

cjGLShaderManager::ProgCache::ProgCache()
{
	m_shaderMtlClsName.SetNone();
	m_snippet = cjNULL;
}

cjGLShaderManager::ProgCache::~ProgCache()
{
	if(m_snippet)
	{
		delete m_snippet;
		m_snippet = cjNULL;
	}
}


//
//cjFShaderProgram* cjGLShaderManager::ResolveShaderProgram(const cjShaderProgramDesc* vtxProgDesc, 
//	const cjShaderProgramDesc* pxlProgDesc)
//{
//	cjFVertexShader* vtxShader = cjNULL;
//	cjFPixelShader* pxlShader = cjNULL;
//
//	vtxShader = m_shaderMtlDefProgSnippets.ResolveVertexShader(vtxProgDesc);
//	pxlShader = m_shaderMtlDefProgSnippets.ResolvePixelShader(pxlProgDesc);
//
//	if(!vtxShader || !pxlShader)
//	{
//		SetFailed
//	}
//
//	return cjNULL;
//}
//
//void cjGLShaderManager::PurgeCache()
//{
//}
//
//cjShader* cjGLShaderManager::GenerateShaderProgram(const cjShaderMtlDefaultDesc* shMtlDefaultDesc)
//{
//	const cjShaderMtlDesc::HashKey& hash = shMtlDefaultDesc->GetHashKey();
//
//	ShMtlDesc2Shader::Pair* pair = m_shMtlDesc2Shader.Find(hash);
//	CacheDat* cacheDat = cjNULL;
//	
//	if(!pair)
//	{
//		ShMtlDesc2Shader::Val v;
//		pair = m_shMtlDesc2Shader.Insert(hash, v);		
//	}
//
//	cacheDat = &pair->m_val;
//
//	if(cacheDat->m_cachedShader)
//		return cacheDat->m_cachedShader;
//	
//	if(cacheDat->m_generateShaderFailCount)
//	{
//		if(cacheDat->m_generateShaderFailCount > MAX_RETRY_GENERATE_SHADER)
//			return cjNULL;
//	}
//	
//	cjGLShader* shader = GenerateShader(shMtlDefaultDesc);
//
//	if(!shader)
//	{
//		cacheDat->m_generateShaderFailCount++;
//		return cjNULL;
//	}
//
//	cacheDat->m_generateShaderFailCount = 0;
//	cacheDat->m_cachedShader = shader;
//	
//	return shader;
//}





//
//public:	
//	virtual cjShader* CreateShaderInstance(const cjShaderMtlDesc* shMtlDesc) = 0;
//
//		// [r] resolved shader count(=renPassCount)
//	virtual cjUINT32 ResolveShaderProgramForRenderPass(cjShader* shader) = 0; 
//	
//	virtual void PurgeCache() {}
//
//	static cjShaderManager* Get() { return ms_shaderManager; }	
//
//	static cjBool StaticInit();
//	static void StaticShutdown();	
//		void SetFailedShader(const cjShaderMtlDesc* mtlDesc);
//
//
//protected:
//	cjShader* FindShader(const cjShaderMtlDesc* mtlDesc);	
//	cjShader* GenerateShader(const cjShaderMtlDesc* mtlDesc);
//	
//	cjBool SetupShaderProgDesc(const cjShaderMtlDesc* desc, cjShaderMtlDefault::RenderPassDesc* pass, 
//		cjUINT32 maxPassCount, cjUINT32& curPassCount, cjUINT32* errNo=cjNULL);
//
//	//cjShaderMtl
//	virtual cjBool BuildShaderMtlDesc(cjShaderMtlDesc* shMtlDesc, const cjSceneMesh* mesh, const cjScenePropRPtrList* props) const;
//
//
//	cjBool SetTextureUVSetInfoToShaderProgDesc(cjDefaultShaderVtxProgDesc* vtxProgDesc, 
//		cjDefaultShaderPxlProgDesc* pxlProgDesc, const cjUINT32 texCount, const cjUINT32* uvSetIndexs, 
//		const cjShaderMtlDefault::TEXUVOUT_TYPE* uvOutTypes);
//	
//	virtual cjShader* CreateShaderForMtlDefDesc(const cjShaderMtlDesc* mtlDesc,
//		cjShaderMtlDefault::RenderPassDesc* renPassDescAry, cjINT32 totalRenPassCount);
//	
//private:
//	cjShader* _GenerateShader(cjShaderMtlDefault::RenderPassDesc* renPassDescAry, const cjShaderMtlDesc* mtlDesc);
//
//
//
//
//
//
//
//protected:
//	static cjShaderManagerRPtr			ms_shaderManager;
//
//
//private:
//	class ShaderCache
//	{
//	public:
//		ShaderCache() { m_generateShaderFailCount = 0; }
//
//		cjUINT32					m_generateShaderFailCount;
//		cjShaderRPtr				m_cachedShader;
//	};
//
//	typedef cjTHash<cjShaderMtlDesc::HashKey, ShaderCache, 
//		cjTHashDefaultNoneRemove<ShaderCache>,
//		cjShaderMtlDesc::HashKeyCmp, 
//		cjShaderMtlDesc::KeyHasher > MtlDesc2Shader;
//
//	MtlDesc2Shader					m_mtlDesc2Shader;
//	MtlDesc2Shader					m_mtlDesc2FailedShader;

