
#ifndef _CJSHADERRES_H_
#define _CJSHADERRES_H_


//
// cjShaderRes
//

CJOBJ_DECL_CLASS_BEGIN(CJENGINE_API, cjShaderRes, cjObj, 0, cjEngine)

	CJOBJ_DECL_SERIALIZE
	CJOBJ_DECL_STATIC_REG

public:
	cjShaderRes();

	cjFShaderProgram* Get(const cjFVertexDecl& vtxDecl);
	//cjFShaderProgram* Get(const cjFMaterialDesc& mtlDesc);
	//cjFShaderProgram* Get(const cjFVertexDecl& vtxDecl);


protected:
	virtual void OnDestroy();

public:
	//cjTAry<cjName>					m_shaderNames;
	//cjTAry<cjString>				m_name;

//	cjTAry<cjString>				m_vtxShaders;
//	cjTAry<cjString>				m_pxlShaders;	

CJOBJ_DECL_CLASS_END(cjShaderRes)



//
////
//// cjShaderResFactory
////
//
//class CJENGINE_API cjShaderResFactory : public cjMem
//{
//public:
//	cjShaderResFactory();
//	virtual ~cjShaderResFactory();
//
//public:
//	static cjFVertexShader* GetVertexShader()
//
//
//};
//
//
//


#endif
