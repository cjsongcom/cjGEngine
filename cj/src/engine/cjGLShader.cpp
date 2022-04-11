
#include "cjEnginePCH.h"
#include "cjGLShader.h"


//
// cjGLRenPass
//

CJSER_IMPL_CLASS(cjGLRenPass, cjRefObj)

cjGLRenPass::cjGLRenPass()
{
	m_shaderProgram = cjNULL;

	m_alphaBlend = cjFalse;
	
	m_usePrevSrcBlendMode = cjTrue;
	m_srcBlendMode = 0;

	m_usePrevDstBlendMode = cjTrue;
	m_dstBlendMode = 0;
}

cjGLRenPass::~cjGLRenPass()
{
}



//
// cjGLShader
//

CJSER_IMPL_CLASS(cjGLShader, cjShader)


cjGLShader::cjGLShader() 
	: m_renPasses(cjGLShader::INIT_REN_PASS_COUNT)
{
	m_totalRenPass = 0;
}

cjGLShader::~cjGLShader()
{
}

cjBool cjGLShader::SetupSceneMesh(cjSceneMesh* mesh, cjShaderMtlInst* shMtlInst)
{
	return cjFalse;
}

cjBool cjGLShader::AppendRenderPass(cjINT32& renPassIndex)
{
	cjGLRenPassRPtr pass = new cjGLRenPass();

	m_renPasses.SetWithGrow(m_totalRenPass, pass);

	renPassIndex = m_totalRenPass;
	m_totalRenPass++;

	return cjTrue;
}

cjBool cjGLShader::SetOverrideAlpha(cjINT32 renPassIndex, cjBool doAlphaBlend, 
			cjBool usePrevSrcBlendMode, cjTENUM srcBlendMode,
			cjBool usePrevDstBlendMode, cjTENUM dstBlendMode)
{
	if(m_totalRenPass <= renPassIndex)
		return cjFalse;

	cjGLRenPass* renPass = m_renPasses(renPassIndex);
	
    if(doAlphaBlend)
    {
		renPass->m_alphaBlend = cjTrue;

		renPass->m_usePrevSrcBlendMode = usePrevSrcBlendMode;
		renPass->m_srcBlendMode = srcBlendMode;

		renPass->m_usePrevDstBlendMode = usePrevDstBlendMode;
		renPass->m_dstBlendMode = dstBlendMode;
    }
    else
    {
		renPass->m_alphaBlend = cjFalse;
    }

	return cjTrue;
}

const cjShaderProgramDesc* cjGLShader::GetVertexShaderProgDesc(cjINT32 renPassIndex) const
{
	if(m_totalRenPass <= renPassIndex)
		return cjFalse;

	return &m_renPasses(renPassIndex)->m_vtxProgDesc;
}

const cjShaderProgramDesc* cjGLShader::GetPixelShaderProgDesc(cjINT32 renPassIndex) const
{
	if(m_totalRenPass <= renPassIndex)
		return cjFalse;

	return &m_renPasses(renPassIndex)->m_pxlProgDesc;
}

cjBool cjGLShader::SetShaderProgramDesc(cjINT32 renPassIndex, const cjShaderProgramDesc* vtxProgDesc,
			const cjShaderProgramDesc* pxlProgDesc)
{
	if(m_totalRenPass <= renPassIndex)
		return cjFalse;

	cjGLRenPass* renPass = m_renPasses(renPassIndex);

	renPass->m_vtxProgDesc = *vtxProgDesc;
	renPass->m_pxlProgDesc = *pxlProgDesc;
	
	return cjTrue;
}

cjFShaderProgram* cjGLShader::GetShaderProgram(cjINT32 renPassIndex)
{
	if(m_totalRenPass <= renPassIndex)
		return cjNULL;

	return m_renPasses(renPassIndex)->m_shaderProgram;
}

cjBool cjGLShader::SetShaderProgram(cjINT32 renPassIndex, cjFShaderProgram* shaderProg)
{
	if(m_totalRenPass <= renPassIndex)
		return cjFalse;

	m_renPasses(renPassIndex)->m_shaderProgram = shaderProg;

	return cjTrue;
}

cjBool cjGLShader::ResolveRenderPassShaderProgram()
{
	if(!cjShaderManager::Get()->ResolveShaderProgramForRenderPass(this))
		return cjFalse;	

	return cjTrue;
}

