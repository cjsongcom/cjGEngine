
#ifndef _CJSHADER_H_
#define _CJSHADER_H_


class cjSceneMesh;
class cjShaderMtlDesc;
class cjShaderMtlInst;
class cjFShaderProgram;


//
// cjShader
//

class CJENGINE_API cjShader : public cjRef
{
	CJSER_DECL_CLASS_ABSTRACT(cjShader, cjRef, 0, CJENGINE_API)

public:	
	const cjShaderMtlDesc* GetShaderMtlDesc() { return &m_shaderMtlDesc; }
	void SetMtlDesc(const cjShaderMtlDesc* mtlDesc) { m_shaderMtlDesc = *mtlDesc; }

	virtual cjBool SetupSceneMesh(cjSceneMesh* mesh, cjShaderMtlInst* shMtlInst) = 0;

	virtual cjAryOff GetRenPassCount() const = 0;	
	virtual cjBool AppendRenderPass(cjINT32& renPassIndex) = 0; // ret: pass index, -1 for error

	virtual cjBool SetOverrideAlpha(cjINT32 renPassIndex, cjBool doAlphaBlend, 
						cjBool usePrevSrcBlendMode, cjTENUM m_srcBlendMode,
						cjBool m_usePrevDstBlendMode, cjTENUM m_dstBlendMode) = 0;

		// shader program
	virtual const cjShaderProgramDesc* GetVertexShaderProgDesc(cjINT32 renPassIndex) const = 0;
	virtual const cjShaderProgramDesc* GetPixelShaderProgDesc(cjINT32 renPassIndex) const = 0;

	virtual cjBool SetShaderProgramDesc(cjINT32 renPassIndex, const cjShaderProgramDesc* vtxProgDesc,
						const cjShaderProgramDesc* pxlProgDesc) = 0;
	
	virtual cjFShaderProgram* GetShaderProgram(cjINT32 renPassIndex=0) = 0;
	virtual cjBool SetShaderProgram(cjINT32 renPassIndex, cjFShaderProgram* shaderProg) = 0;

	virtual cjBool ResolveRenderPassShaderProgram() = 0;
	
private:
	cjShaderMtlDesc					m_shaderMtlDesc;

};

cjRefPtrDefine(cjShader)


//
// cjShaderMtlIns
//

class CJENGINE_API cjShaderMtlInst : public cjMem
{
	CJTYPEINFO_SUPPORT_DECL(cjShaderMtlInst)

public:
	enum EMtlDirtyState { EMDS_CLEAR, EMDS_DIRTY, EMDS_QUERY };

	cjShaderMtlInst();
    cjShaderMtlInst(const cjShaderMtlInst& rhs);
    cjShaderMtlInst(cjShaderMtl* shaderMtl);
	~cjShaderMtlInst();

    cjShaderMtlInst& operator=(const cjShaderMtlInst& shMtlInst);
	
	bool operator==(const cjShaderMtlInst& shMtlInst) const ;
	bool operator!=(const cjShaderMtlInst& shMtlInst) const { return !operator==(shMtlInst); }

	// 1. evaluate cached shader with mesh, sceneprop, ..
	// 2. if evalutation is succed return cached shader, otherwise return null
//	cjShader* EvaluateCachedShader(const cjSceneMesh* mesh, const cjScenePropRPtrList* props) const;

	// 1. if shaderMaterial doesn't exist, return current cached shader
	// 2. cache shader matching with shaderMaterial
//	cjShader* CacheShader(cjSceneMesh* mesh, const cjScenePropRPtrList* props);

//	void ClearCachedShader();

	cjShaderMtl* GetShaderMtl() const { return m_shaderMtl; }
	cjObjRef<cjShaderMtl>& GetShaderMtlObjRef() { return m_shaderMtl; }

	void SetMtlDirtyState(EMtlDirtyState state) { m_dirtyState = state; }

public:
	cjObjRef<cjShaderMtl>			m_shaderMtl;
	cjShaderRPtr					m_cachedShader;

	EMtlDirtyState					m_dirtyState;
};

CJTYPEINFO_SUPPORT_IMPL(cjShaderMtlInst);


#endif
