
#ifndef _CJGLSHADER_H_
#define _CJGLSHADER_H_


//
// cjGLRenPass
//

class CJENGINE_API cjGLRenPass : public cjRefObj
{
	CJSER_DECL_CLASS(cjGLRenPass, cjRefObj,0 , CJENGINE_API) 

public:
    cjGLRenPass();
	
public:
	cjShaderProgramDesc				m_vtxProgDesc;
	cjShaderProgramDesc				m_pxlProgDesc;

	cjFShaderProgram*				m_shaderProgram;

	cjBool							m_alphaBlend;

	cjBool							m_usePrevSrcBlendMode;
	cjTENUM							m_srcBlendMode;
	cjBool							m_usePrevDstBlendMode;
	cjTENUM							m_dstBlendMode;

protected:
	CJTYPEINFO_SUPPORT_DECL(cjGLRenPass)	    
};

cjRefPtrDefine(cjGLRenPass)


//
// cjGLShader
//

class CJENGINE_API cjGLShader : public cjShader
{
	CJSER_DECL_CLASS(cjGLShader, cjShader, 0, CJENGINE_API)

public:
	// cjShader
	virtual cjBool SetupSceneMesh(cjSceneMesh* mesh, cjShaderMtlInst* shMtlInst);

	virtual cjAryOff GetRenPassCount() const { return m_totalRenPass; }
	virtual cjBool AppendRenderPass(cjINT32& renPassIndex);

	virtual cjBool SetOverrideAlpha(cjINT32 renPassIndex, cjBool doAlphaBlend, 
									cjBool usePrevSrcBlendMode, cjTENUM srcBlendMode,
									cjBool usePrevDstBlendMode, cjTENUM dstBlendMode);

	virtual const cjShaderProgramDesc* GetVertexShaderProgDesc(cjINT32 renPassIndex) const;
	virtual const cjShaderProgramDesc* GetPixelShaderProgDesc(cjINT32 renPassIndex) const;

	virtual cjBool SetShaderProgramDesc(cjINT32 renPassIndex, const cjShaderProgramDesc* vtxProgDesc,
						const cjShaderProgramDesc* pxlProgDesc);
	
	virtual cjFShaderProgram* GetShaderProgram(cjINT32 renPassIndex=0);
	virtual cjBool SetShaderProgram(cjINT32 renPassIndex, cjFShaderProgram* shaderProg);

	virtual cjBool ResolveRenderPassShaderProgram();

	
public:
	enum { INIT_REN_PASS_COUNT = 2 };
	cjGLShader();	


protected:
	cjGLRenPassRPtr					m_curPass;

	cjINT32							m_totalRenPass;
	cjTAry<cjGLRenPassRPtr>			m_renPasses;
};

cjRefPtrDefine(cjGLShader)



#endif

