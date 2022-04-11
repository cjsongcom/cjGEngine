
#ifndef _CJGLSHADER_PROG_SNIPPET_SHADER_MTL_DEF_
#define _CJGLSHADER_PROG_SNIPPET_SHADER_MTL_DEF_


#include "cjGLShaderManager.h"


//
// cjGLShaderProgSnippet_ShaderMtlDefault
//

class CJENGINE_API cjGLShaderProgSnippet_ShaderMtlDefault : public cjGLShaderProgSnippet
{
public:
		// cjGLShaderManager
	virtual cjFVertexShader* ResolveVertexShader(const cjShaderProgramDesc* vtxDesc);
	virtual cjFPixelShader* ResolvePixelShader(const cjShaderProgramDesc* pxlDesc);

public:
	cjGLShaderProgSnippet_ShaderMtlDefault();
	virtual ~cjGLShaderProgSnippet_ShaderMtlDefault();

private:
	cjFVertexShader* GenerateVtxProgram(const cjDefaultShaderVtxProgDesc* vtxDesc);
	cjFVertexShader* _GenerateVtxProgram(const cjDefaultShaderVtxProgDesc* vtxDesc);

	cjFPixelShader* GeneratePxlProgram(const cjDefaultShaderPxlProgDesc* pxlDesc);
	cjFPixelShader* _GeneratePxlProgram(const cjDefaultShaderPxlProgDesc* pxlDesc);
};


#endif

