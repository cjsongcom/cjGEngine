
#ifndef _CJSHADERMATERIAL_H_
#define _CJSHADERMATERIAL_H_

#include "cjMaterial.h"


//
// cjShaderMtl
//

CJOBJ_DECL_ABSTRACT_BEGIN(CJENGINE_API, cjShaderMtl, cjMaterial, 0, cjEngine)

	CJOBJ_DECL_SERIALIZE
	CJOBJ_DECL_STATIC_REG

public:
	/* 
		cjShaderMtlDefaultDesc::GetTextureCount();
		cjShaderMtlDefaultDesc::HandleXXXDesc
		cjShaderMtlDefault::SetupShaderProgDesc
		CJSHDESC_DEF_BEGIN(TEX_XXX_CNT)		
	*/
	enum TEX_TYPE
	{
		TEX_DIFFUSE,
		TEX_ENV,
		TEX_DETAIL,		
		TEX_NORMAL,
		TEX_SPECULAR,
		TEX_GLOW,
		TEX_EXT0,
		TEX_EXT1,		
		TEX_MAX
	};

    enum TEXUVOUT_TYPE
    {        
		TEXUVOUT_NONE						= 0,
        TEXUVOUT_PASSTHRU					= 1,
        TEXUVOUT_WITH_TRANSFORM				= 2,
		TEXUVOUT_WITH_PERSPECTIVE_PROJ		= 3,
		
        TEXUV_OUT_MAX
    };

public:
        // cjMaterial
	virtual cjBool CheckCircularRef(cjTAry<cjMaterial*>& materials);
	virtual void PreSetupMaterial(cjEngineTime_ft timeSec);
       
public:
	enum { NO_SHMTL_INDEX = -1 };
	enum { DEFAULT_MAX_PASS_COUNT = 5 };

    cjShaderMtl();

	const cjShaderMtlDesc& GetMtlDesc() { return m_mtlDesc; }
	const cjName& GetShaderMtlClsName() const { return m_shaderMtlClsName; }

	const cjScenePropRPtrList& GetScenePropList() const { return m_scenePropList; }
	cjScenePropRPtrList& GetScenePropList() { return m_scenePropList; }

	virtual const cjSceneRenPropTextures* GetTextures() const = 0;

	//virtual cjBool BuildShaderMtlDesc(cjShaderMtlDesc& outMtlDesc, const cjFVertexStream** stream, 
	//	cjINT32 streamCount) = 0;

	//virtual void ResolveMtlDesc(cjShaderMtlDesc& rstMtlDesc, const cjFVertexStream** streamAry, 
	//	cjINT32 streamCount, const cjScenePropRPtrList* scenePropList=cjNULL) const = 0;

//	static cjINT32 ConvToTexStage(TEX_TYPE type) { return (cjINT32)type; }
//	static TEX_TYPE ConvToTexType(cjINT32 texStageIndex) { return (TEX_TYPE)texStageIndex; }

	virtual cjShader* ResolveShader(cjShaderMtlDesc& rstMtlDesc, const cjFVertexStream** streamAry, 
		cjINT32 streamCount, const cjScenePropRPtrList* scenePropList=cjNULL) const = 0;

protected:
	void AddShaderToShaderManager(cjShader* shader) const;
	
protected:
	cjName							m_shaderMtlClsName; // cjShaderMtlDefault, ShaderShadow	

	cjShaderMtlDesc					m_mtlDesc;
	cjScenePropRPtrList				m_scenePropList;


CJOBJ_DECL_ABSTRACT_END(cjShaderMtl)


//
// cjShaderMtlDefault
//

CJOBJ_DECL_CLASS_BEGIN(CJENGINE_API, cjShaderMtlDefault, cjShaderMtl, 0, cjEngine)

	CJOBJ_DECL_SERIALIZE

public:
	// cjShaderMtl
	cjShader* ResolveShader(cjShaderMtlDesc& rstMtlDesc, const cjFVertexStream** streamAry, 
		cjINT32 streamCount, const cjScenePropRPtrList* scenePropList=cjNULL) const;
		
public:
	enum { MAX_RENDER_PASS = 4 };

	class CJENGINE_API RenderPassDesc : public cjMem
    {
    public:
        RenderPassDesc();
//      RenderPassDesc(RenderPassDesc&);
//      RenderPassDesc& operator=(RenderPassDesc& rhs);

        void Initialize();
		
	public:
        cjShaderProgramDesc			m_vtxProgDesc;
        cjShaderProgramDesc			m_pxlProgDesc;

        cjBool						m_overrideAlpha;

        cjBool						m_doAlphaBlend;	// valid when m_alphaOverride=cjTrue

        cjBool						m_usePrevSrcBlendMode;
		cjTENUM						m_srcBlendMode;		// cjSceneRenPropAlpha::EALPHABL, m_usePrevSrcBlendMode=cjFalse

        cjBool						m_usePrevDstBlendMode;
		cjTENUM						m_dstBlendMode;		// cjSceneRenPropAlpha::EALPHABL, m_usePrevDstBlendMode=cjFalse        
    };
		
public:
    cjShaderMtlDefault();
	
	virtual const cjSceneRenPropTextures* GetTextures() const;
	cjSceneRenPropTextures::Tex* SetTexture(cjSceneRenPropTextures::ETexType texType, cjTexture* texObj);
	
protected:
	// cjObj
    virtual void OnDestroy();

	virtual void ResolveShaderMtlDesc(cjShaderMtlDesc& rstMtlDesc, const cjFVertexStream** streamAry, 
		cjINT32 streamCount, const cjScenePropRPtrList* overrideSceneProps=cjNULL) const;

	cjShader* GenerateShader(const cjShaderMtlDesc& mtlDesc) const;
	cjShader* _GenerateShader(cjShaderMtlDefault::RenderPassDesc* renPassDescAry, 
		const cjShaderMtlDesc& mtlDesc) const;

	cjBool SetupShaderProgDesc(const cjShaderMtlDesc& desc, cjShaderMtlDefault::RenderPassDesc* pass,
		cjUINT32 maxPassCount, cjUINT32& curPassCount, cjUINT32* errNo=cjNULL) const;

	cjBool SetTextureUVSetInfoToShaderProgDesc(cjDefaultShaderVtxProgDesc* vtxProgDesc, 
		cjDefaultShaderPxlProgDesc* pxlProgDesc, const cjUINT32 texCount, const cjUINT32* uvSetIndexs, 
		const cjShaderMtlDefault::TEXUVOUT_TYPE* uvOutTypes) const;

	cjShader* CreateShaderForMtlDefDesc(const cjShaderMtlDesc& mtlDesc, cjShaderMtlDefault::RenderPassDesc* renPassDescAry, 
		cjINT32 totalRenPassCount) const;
			
protected:


CJOBJ_DECL_CLASS_END(cjShaderMtlDefault)



#endif
