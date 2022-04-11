
#ifndef _CJSHADER_MANAGER_H_
#define _CJSHADER_MANAGER_H_


class cjShader;
class cjShaderMtlDesc;


//
// cjShaderFactory
//

cjRefPtrForwardDefine(cjShaderManager)


class CJENGINE_API cjShaderManager : public cjRef
{
	CJSER_DECL_CLASS_ABSTRACT(cjShaderManager, cjRef, 0, CJENGINE_API)

public:
	enum { MAX_RETRY_GENERATE_SHADER = 3 };

protected:
	class ShaderCache
	{
	public:
		ShaderCache(cjShader* shader=cjNULL)
		{ 
			m_generateShaderFailCount = 0; 
			m_cachedShader = shader;
		}

		cjUINT32					m_generateShaderFailCount;
		cjShaderRPtr				m_cachedShader;
	};

	typedef cjTHash<cjShaderMtlDesc::HashKey, ShaderCache, cjTHashDefaultNoneRemove<ShaderCache>,
		cjShaderMtlDesc::HashKeyCmp, cjShaderMtlDesc::KeyHasher > MtlDesc2Shader;
	
	class ShaderContainer
	{
	public:
		ShaderContainer() {}

		MtlDesc2Shader					m_mtlDesc2Shader;
		MtlDesc2Shader					m_mtlDesc2FailedShader;
	};

	typedef cjTHash<cjName, ShaderContainer> ShaderContainerHash;
	

public:	
	virtual cjShader* ResolveShader(const cjShaderMtlDesc& shMtlDesc) = 0;
		
	virtual cjShader* FindShader(const cjName& mtlClsName, const cjShaderMtlDesc& desc, cjBool* isFailedShader=cjNULL);

	virtual void AddShaderCache(const cjName& mtlClsName, cjShader* shader);
	virtual void SetFailedShader(const cjName& mtlClsName, const cjShaderMtlDesc& desc);

	virtual cjShader* CreateShaderInstance(const cjShaderMtlDesc* shMtlDesc=cjNULL) = 0;
	virtual cjBool ResolveShaderProgramForRenderPass(cjShader* shader) = 0;

	virtual void PurgeCache() {}

	ShaderContainerHash::Pair* FindShaderContainer(const cjName& mtlClsName, cjBool createAtNone=cjFalse);
	
	static cjShaderManager* Get() { return ms_shaderManager; }	

	static cjBool StaticInit();
	static void StaticShutdown();
	
protected:	
	ShaderContainerHash				m_shaders;

	static cjShaderManagerRPtr		ms_shaderManager;

	//	virtual cjBool BuildShaderMtlDesc(cjShaderMtlDesc* shMtlDesc, const cjScenePropRPtrList* props) const = 0;
	
};

cjRefPtrDefine(cjShaderManager)


#endif

