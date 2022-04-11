
#ifndef _CJGLSHADER_MANAGER_H_
#define _CJGLSHADER_MANAGER_H_

class cjShaderMtlDesc;
class cjShaderMtlDefaultDesc;
class cjShaderProgramDesc;

#include "cjGLShader.h"


//
// cjGLShaderProgSnippets
//

class CJENGINE_API cjGLShaderProgSnippet : public cjMem
{
public:
	class CJENGINE_API VertexProg : public cjMem
	{
	public:
		VertexProg();
		~VertexProg();

		cjShaderProgramDesc					m_progDesc;
		cjFVertexShader*					m_vertexShader;
	};
		
	typedef cjTHash<cjShaderProgramDesc::HashKey, VertexProg*, cjTHashRemoveByDel<VertexProg*>, 
		cjShaderProgramDesc::HashKeyCmp, cjShaderProgramDesc::KeyHasher > VtxDesc2Prog;


	class CJENGINE_API PixelProg : public cjMem
	{
	public:
		PixelProg();
		~PixelProg();

		cjShaderProgramDesc					m_progDesc;
		cjFPixelShader*						m_pixelShader;		
	};
		
	typedef cjTHash<cjShaderProgramDesc::HashKey, PixelProg*, cjTHashRemoveByDel<PixelProg*>, 
		cjShaderProgramDesc::HashKeyCmp, cjShaderProgramDesc::KeyHasher > PxlDesc2Prog;
	
public:
	cjGLShaderProgSnippet();
	virtual ~cjGLShaderProgSnippet();

	static cjGLShaderProgSnippet* Create(const cjName& mtlName);

	// resolve and clone new instance
	virtual cjFVertexShader* ResolveVertexShader(const cjShaderProgramDesc* vtxDesc) = 0;
	virtual cjFPixelShader* ResolvePixelShader(const cjShaderProgramDesc* pxlDesc) = 0;
		
protected:
	VtxDesc2Prog					m_vtxDesc2Prog;
	PxlDesc2Prog					m_pxlDesc2Prog;
};


//
// cjGLShaderManager
//

class CJENGINE_API cjGLShaderManager : public cjShaderManager
{
	CJSER_DECL_CLASS(cjGLShaderManager, cjShaderManager, 0, CJENGINE_API)

	// cjShaderManager
public:
	virtual cjShader* ResolveShader(const cjShaderMtlDesc& shMtlDesc);

protected:
//	virtual cjBool BuildShaderMtlDesc(cjShaderMtlDesc* shMtlDesc, const cjScenePropRPtrList* props) const;
	
public:	
		// cjShaderManager
	virtual cjShader* CreateShaderInstance(const cjShaderMtlDesc* shMtlDesc=cjNULL);
	virtual cjBool ResolveShaderProgramForRenderPass(cjShader* shader);

	virtual void PurgeCache();

public:

	class ShaderProgKey 
	{ 
	public:
		typedef ShaderProgKey HashKey;

		class HashKeyCmp
		{ 
		public:
			cjBool operator()(const HashKey& lhs, const HashKey& rhs) const 
			{	
				if( (cjMemCmp(lhs.m_hash0.v, rhs.m_hash0.v, sizeof(lhs.m_hash0.v)) == 0) &&
					(cjMemCmp(lhs.m_hash1.v, rhs.m_hash1.v, sizeof(lhs.m_hash1.v)) == 0) )
					return cjTrue;

				return cjFalse;
			}
		};

		class KeyHasher
		{
		public:
			cjTHasherValType operator()(const HashKey& key) const
			{
				cjUINT32 hash = key.m_hash0.v[0];

				for(cjUINT32 i=1; i<cjShaderProgramDesc::EARY_COUNT; i++)
					hash = (hash << 5) + hash + key.m_hash0.v[i];

				for(cjUINT32 i=0; i<cjShaderProgramDesc::EARY_COUNT; i++)
					hash = (hash << 5) + hash + key.m_hash1.v[i];
			
				return hash;
			}
		};

	public:
		ShaderProgKey(const cjShaderProgramDesc* vtxProgDesc, const cjShaderProgramDesc* pxlProgDesc);

	public:
		cjShaderProgramDesc::HashKey			m_hash0; // vertex hash
		cjShaderProgramDesc::HashKey			m_hash1; // pixel hash
	};
	
	class ProgCache
	{
	public:
		ProgCache();
		~ProgCache();

	public:
		cjName							m_shaderMtlClsName;
		cjGLShaderProgSnippet*			m_snippet;

		typedef cjTHash<ShaderProgKey, cjFShaderProgram*, cjTHashRemoveByDel<cjFShaderProgram*>,
			ShaderProgKey::HashKeyCmp, ShaderProgKey::KeyHasher > ShaderProgHash;
		ShaderProgHash					m_shaderProgHash;
	};

public:
	cjGLShaderManager();
	
protected:

	cjFShaderProgram* GetShaderProgram(cjGLShaderManager::ProgCache* cache,
		const cjShaderProgramDesc* vtxProgDesc, const cjShaderProgramDesc* pxlProgDesc);

	cjGLShaderManager::ProgCache* GetProgCache(const cjName& mtlName);

	
	//virtual cjShader* GenerateShaderProg(const cjShaderMtlDefaultDesc* shMtlDefaultDesc);
	//cjGLShader* GenerateShader(const cjShaderMtlDefaultDesc* shMtlDefaultDec);
	//
	//cjUINT32 ResolveShaderProgramForRenderPass(cjShader* shader);
	//cjGLShaderManager::ProgCache* GetProgCache(const cjName& mtlName);

protected:
	typedef cjTHash<cjName, ProgCache*, cjTHashRemoveByDel<ProgCache*> > ProgCacheDatHash;

	ProgCacheDatHash				m_progCaches;
};


#endif
