
#ifndef _CJMATERIAL_H_
#define _CJMATERIAL_H_


class cjDefaultShaderVtxProgDesc;
class cjDefaultShaderPxlProgDesc;


enum cjMaterialType
{
        // shader stuff
   	cjMT_Bitmap				= 0x00000001,
	cjMT_Texture			= 0x00000002,
	cjMT_Shader				= 0x00000004,

        // modifiers
    cjMT_Modifier           = 0x00000100,
	cjMT_TexModifier		= 0x00000200,
	cjMT_FinalBlend			= 0x00000400,
    cjMT_ColorModifier      = 0x00000800,
};

enum cjShaderMtlType
{
    cjShaderMtl_Diffuse,
    cjShaderMtl_Specuar,
    cjShaderMtl_Reflect,
//    cjShaderMat_MAX
};


enum cjShaderOutBlendType
{
    cjShaderOBT_Add      = 0,
    cjShaderOBT_Masked   = 1,
    cjShaderOBT_Blend    = 2,
    cjShaderOBT_MAX      = 3
};


CJOBJ_DECL_ABSTRACT_BEGIN(CJENGINE_API, cjMaterial, cjObj, 0, cjEngine)

	CJOBJ_DECL_SERIALIZE

protected:
	virtual void OnDestroy();

public:
    cjMaterial();

	virtual cjBool CheckCircularRef(cjTAry<cjMaterial*>& materials);
	virtual void PreSetupMaterial(cjEngineTime_ft timeSec) {};
    
	virtual cjINT32 GetMaterialUSiz()   { return 0; }
	virtual cjINT32 GetMaterialVSiz()   { return 0; }
	virtual cjBool CheckNeedSorting()   { return cjFalse; }
	virtual cjBool IsTransparent()      { return cjFalse; }
	virtual cjBool CheckNeedUVComp()    { return cjTrue;  }
	virtual cjBool CheckNeedNormalComp(){ return cjFalse; }
    
 	static void ClearFallbacks();
    
	virtual cjMaterial* CheckFallback();
	virtual cjBool HasFallback() { return m_fallbackMaterial ? cjTrue : cjFalse; }
    
    static cjMaterial* GetDefaultMaterial() { return cjNULL; }
	virtual cjBYTE GetNumOfUVStreams() { return 1; }
    
public:
    cjMaterialType                          m_typeMaterial;
    cjMaterial*                             m_fallbackMaterial;
    cjMaterial*                             m_defaultMaterial;
    
    cjBool                                  m_useFallbackMaterial;

CJOBJ_DECL_ABSTRACT_END(cjMaterial)


//
// Material Modifiers
//

CJOBJ_DECL_CLASS_BEGIN(CJENGINE_API, cjMtlModifier, cjMaterial, 0, cjEngine)
    
public:
	virtual cjBool CheckCircularRef(cjTAry<cjMaterial*>& materials);
	virtual void PreSetupMaterial(cjEngineTime_ft timeSec)
        { if(m_material) m_material->CheckFallback()->PreSetupMaterial(timeSec); }
    
	virtual cjINT32 GetMaterialUSiz()   { return m_material ? m_material->GetMaterialUSiz() : 0; }
	virtual cjINT32 GetMaterialVSiz()   { return m_material ? m_material->GetMaterialVSiz() : 0; }
	virtual cjBool CheckNeedSorting()   { return m_material ? m_material->CheckNeedSorting() : cjFalse; }
	virtual cjBool IsTransparent()      { return m_material ? m_material->IsTransparent() : cjFalse; }
	virtual cjBool CheckNeedUVComp()    { return m_material ? m_material->CheckNeedUVComp() : cjTrue; }
	virtual cjBool CheckNeedNormalComp(){ return m_material ? m_material->CheckNeedNormalComp() : cjFalse; }
   
public:
    cjMaterial*                             m_material;

CJOBJ_DECL_CLASS_END(cjMtlModifier)


CJOBJ_DECL_CLASS_BEGIN(CJENGINE_API, cjColorMtlModifier, cjMtlModifier, 0, cjEngine)

public:
    cjColor                             m_color;
    cjBool                              m_renderTwoSided;
    cjBool                              m_alphaBlend;

CJOBJ_DECL_CLASS_END(cjColorMtlModifier)


CJOBJ_DECL_CLASS_BEGIN(CJENGINE_API, cjFinalBlendMtlModifier, cjMtlModifier, 0, cjEngine)

public:
        // cjMaterial
    virtual cjBool CheckNeedSorting();
    virtual cjBool IsTransparent();

public:
    cjBYTE                              m_frameBufferBlend; // cjRenFrameBufferBlending
    cjBool                              m_zWrite;
    cjBool                              m_zTest;
    cjBool                              m_alphaTest;
    cjBool                              m_twoSided;
    cjBYTE                              m_alphaRef;

CJOBJ_DECL_CLASS_END(cjFinalBlendMtlModifier)


//
// texture modifiers
//
    // base class of texture modifiers
CJOBJ_DECL_CLASS_BEGIN(CJENGINE_API, cjTexMtlModifier, cjMtlModifier, 0, cjEngine)

public:
	// Material interface.
	virtual cjINT32 GetMaterialUSiz();
	virtual cjINT32 GetMaterialVSiz();
	virtual cjBool CheckNeedUVComp();
	virtual cjBool CheckNeedNormalComp();
    
public:
	virtual cjMatrix* GetMatrix(cjEngineTime_ft timeSec) { return NULL; }

public:
    cjBYTE                              m_texCoordSource; // cjTexCoordSrc
    cjBYTE                              m_texCoordCount;
    cjBool                              m_texCoordProjected;

CJOBJ_DECL_CLASS_END(cjTexMtlModifier)


CJOBJ_DECL_CLASS_BEGIN(CJENGINE_API, cjTexCoordSrcStreamMtlModifer, cjTexMtlModifier, 0, cjEngine)
    
public:
    virtual void cjObjPostEditChange();
    
public:
    cjBYTE                             m_uvStreamIndex; // cjTexCoordSrc0 ~ 7

CJOBJ_DECL_CLASS_END(cjTexCoordSrcStreamMtlModifer)


CJOBJ_DECL_CLASS_BEGIN(CJENGINE_API, cjTexMatrixMtlModifier, cjTexMtlModifier, 0, cjEngine)
    
public:
    // cjTexMtlModifier
	virtual cjMatrix* GetMatrix(cjEngineTime_ft timeSec) { return &m_matrix; }

public:
    cjMatrix                           m_matrix;

CJOBJ_DECL_CLASS_END(cjTexMatrixMtlModifier)


CJOBJ_DECL_CLASS_BEGIN(CJENGINE_API, cjTexEnvMapMtlModifier, cjTexMtlModifier, 0, cjEngine)

public:
    // cjTexMtlModifier
    // always return null and m_texCoordSource at cjTexMtlModifier will be changed
	virtual cjMatrix* GetMatrix(cjEngineTime_ft timeSec);

public:
    cjBYTE                                  m_envMapType;

CJOBJ_DECL_CLASS_END(cjTexEnvMapMtlModifier)


//
// cjRenderMaterial
//

CJOBJ_DECL_ABSTRACT_BEGIN(CJENGINE_API, cjRenderedMaterial, cjMaterial, 0, cjEngine)

CJOBJ_DECL_ABSTRACT_END(cjRenderedMaterial)


CJOBJ_DECL_ABSTRACT_BEGIN(CJENGINE_API, cjBitmapMaterial, cjRenderedMaterial, 0, cjEngine)

	CJOBJ_DECL_SERIALIZE
    
public:
        // cjMaterial
	virtual cjINT32 GetMaterialUSiz() { return m_sizU; }
	virtual cjINT32 GetMaterialVSiz() { return m_sizV; }

public:
    virtual cjFBaseTexture* GetTexInterface() = 0;
	virtual cjBitmapMaterial* Get(cjEngineTime_t curTime, cjViewport* viewport) { return this; }
    
    cjRSurfaceFmt GetFormat() { return m_format; }
    cjTexClampMode GetTexClampModeU() { return m_texUClamp; }
    cjTexClampMode GetTexClampModeV() { return m_texVClamp; }
    
public:
    cjBitmapMaterial();

protected:
	cjRSurfaceFmt                       m_format;
	cjTexClampMode                      m_texUClamp;
	cjTexClampMode                      m_texVClamp;

	cjBYTE                              m_potbitU; // POT=Power of two , 2^n, size=128 => 2^7 = 7
    cjBYTE                              m_potbitV;
    
	cjINT32                             m_sizU;
    cjINT32                             m_sizV;
    
CJOBJ_DECL_ABSTRACT_END(cjBitmapMaterial)


//
//class cjTestMaterial : public cjBitmapMaterial
//{
//    CJOBJ_DECL_CLASS(cjTestMaterial, cjMaterial, 0, cjEngine)
//   
//public:
//    cjTestMaterial();
//    
//protected:
//    virtual void OnDestroy();
//    
//};


#endif
