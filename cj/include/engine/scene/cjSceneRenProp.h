
#ifndef _CJSCENERENPROP_H_
#define _CJSCENERENPROP_H_


//
// Scene Render Property
//

class CJENGINE_API cjSceneRenProp : public cjSceneProp
{
	CJSER_DECL_CLASS(cjSceneRenProp, cjSceneProp, CJRTTI_CF_ABSTRACT, CJENGINE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE_ABSTRACT(cjSceneRenProp)

public:
	virtual cjSceneProp* GetRenProp() { return this; }

public:
	enum ERenProp
	{
		ERenProp_None,
		ERenProp_Alpha,
		ERenProp_Dither,
		ERenProp_Fog,
		ERenProp_Material,
		ERenProp_Specular,
		ERenProp_Textures,
		ERenProp_VtxColor,
		ERenProp_WireFrame,
		ERenProp_TwoSide,
		ERenProp_ZBuffer,
		ERenProp_Stencil,
		ERenProp_Shade,
		ERenProp_Max
	};

	cjSceneRenProp() { m_cate = EPropCate_Render; }

	virtual ERenProp GetRenPropType() { return ERenProp_None; }
	virtual void ApplyRenderInterface(cjRenInterface* /*ri*/) { }

};


//
// cjSceneRenPropAlpha
//

class CJENGINE_API cjSceneRenPropAlpha : public cjSceneRenProp
{
	CJSER_DECL_CLASS(cjSceneRenPropAlpha, cjSceneRenProp, 0, CJENGINE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjSceneRenPropAlpha)

public:
	enum EALPHABL
	{
		EALPHABL_ONE,
		EALPHABL_ZERO,
		EALPHABL_SRCCOLOR,
		EALPHABL_INVSRCCOLOR,
		EALPHABL_DSTCOLOR,
		EALPHABL_INVDSTCOLOR,
		EALPHABL_SRCALPHA,
		EALPHABL_INVSRCALPHA,
		EALPHABL_DSTALPHA,
		EALPHABL_INVDSTALPHA,
		EALPHABL_MAX
	};

	enum EALPHATESTFN
	{
		EALPHATESTFN_ALWAYS,
		EALPHATESTFN_LESS,
		EALPHATESTFN_EQUAL,
		EALPHATESTFN_LESSEQUAL,
		EALPHATESTFN_GREATER,
		EALPHATESTFN_NOTEQUAL,
		EALPHATESTFN_GREATEREQUAL,
		EALPHATESTFN_NEVER,
		EALPHATESTFN_MAX
	};

public:
	virtual ERenProp GetRenPropType() { return ERenProp_Alpha; }
	virtual void ApplyRenderInterface(cjRenInterface* ri);

public:
	cjSceneRenPropAlpha();

	void SetAlphaBlending(cjBool useBlend) { m_useAlphaBlend = useBlend; }
	cjBool GetAlphaBlending() const { return m_useAlphaBlend; }

	void SetSrcBlend(EALPHABL srcBlend) { m_srcBlend = srcBlend; }
	EALPHABL GetSrcBlend() const { return m_srcBlend; }

	void SetDstBlend(EALPHABL dstBlend) { m_dstBlend = dstBlend; }
	EALPHABL GetDstBlend() const { return m_dstBlend; }

	void SetAlphaTestUse(cjBool useAlphaTest) { m_useAlphaTest = useAlphaTest; }
	cjBool GetAlphaTestUse() const { return m_useAlphaTest; }

	void SetAlphaTestFunc(EALPHATESTFN testFunc) { m_funcAlphaTest = testFunc; }
	EALPHATESTFN GetAlphaTestFunc() const { return m_funcAlphaTest; }

	void SetAlphaTestRef(cjBYTE refAlpha) { m_refAlphaTest = refAlpha; }
	cjBYTE GetAlphaTestRef() const { return m_refAlphaTest; }

private:
	cjBYTE											m_refAlphaTest;		// 8
		
	EALPHABL										m_srcBlend;			// 4
	EALPHABL										m_dstBlend;			// 4

	EALPHATESTFN									m_funcAlphaTest;	// 4	
	cjBool											m_useAlphaTest;		// 1
	cjBool											m_useAlphaBlend;	// 1	
};


//
// cjSceneRenPropFog
//

class CJENGINE_API cjSceneRenPropFog : public cjSceneRenProp
{
	CJSER_DECL_CLASS(cjSceneRenPropFog, cjSceneRenProp, 0, CJENGINE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjSceneRenPropFog)

public:
	// cjSceneRenProp
	virtual ERenProp GetRenPropType() { return ERenProp_Fog; }
	virtual void ApplyRenderInterface(cjRenInterface* ri);

public:
	// fog attenuation type
    enum FOG_ATTENU_TYPE  
    {
        FOG_ATTENU_ZLINEAR,
        FOG_ATTENU_RANGE_SQ,
        FOG_ATTENU_VTX_ALPHA
		// Reserved 0
    };

CJFLAG_DECL_BEGIN
		MASK_USE_FOG	= 0x00000001,	// 1bit
		BPOS_USE_FOG	= 0,

		MASK_ATTENU		= 0x00000006,	// 2bit
		BPOS_ATTENU		= 1,
CJFLAG_DECL_END
	
public:
	cjSceneRenPropFog();

	void SetUseFog(cjBool useFog) { _SetFlagAsBool(MASK_USE_FOG, BPOS_USE_FOG, useFog); }
	cjBool GetUseFog() const { return _GetFlagAsBool(MASK_USE_FOG, BPOS_USE_FOG); }

	void SetFogAttenuType(FOG_ATTENU_TYPE type) { _SetFlag(MASK_ATTENU, BPOS_ATTENU, type); }
	FOG_ATTENU_TYPE GetFogAttenuType() const { return (FOG_ATTENU_TYPE)_GetFlag(MASK_ATTENU, BPOS_ATTENU); }

	void SetFogColor(const cjColorF& color) { m_color = color; }
	const cjColorF& GetFogColor() const { return m_color; }

	// 0: no fogging, 1: fog starts at near plane, >1 : fog starts at front of near plane
	void SetDepth(cjFLOAT depth) { m_depth = (depth >= 0.0f ? depth : 0.0f); }
	cjFLOAT GetDepth() const { return m_depth; }

protected:
	cjFLOAT							m_depth;
	cjColorF						m_color;
};


//
// cjSceneRenPropMaterial
//

class CJENGINE_API cjSceneRenPropMaterial : public cjSceneRenProp
{
	CJSER_DECL_CLASS(cjSceneRenPropMaterial, cjSceneRenProp, 0, CJENGINE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjSceneRenPropMaterial)

public:
	virtual ERenProp GetRenPropType() { return ERenProp_Material; }
	virtual void ApplyRenderInterface(cjRenInterface* ri);

public:

/*
			amb		dif		spec	emis	shi		alph
			[0]		[1]		[2]		[3]		[4]		[5]		[6]		[7]
32F(RAW)	0		0		0	    0       0        0
8B (UNIT)   0x1		0x2		0x4		0x8		0x10	0x20   0x40  0x80
*/
	enum 
	{
		EAMB		= 0x1,
		EDIF		= 0x2,
		ESPEC		= 0x4,
		EEMIS		= 0x8,
		ESHI		= 0x10,
		EALPH		= 0x20,
	};

	cjSceneRenPropMaterial();

	void SetAmbientColor(const cjColorF& color)		{ m_ambient = color;	}
	void SetDiffuseColor(const cjColorF& color)		{ m_diffuse = color;	}
	void SetSpecularColor(const cjColorF& color)	{ m_specular = color;	}
	void SetEmissiveColor(const cjColorF& color)	{ m_emissive = color;	}

	void SetShineness(cjFLOAT shineness=0.0f)		{ m_shineness = shineness; }
	void SetAlpha(cjFLOAT alpha=1.0f)				{ m_alpha = alpha; }

	void SetSerialFlag(cjBYTE flag) { m_serialFlag = flag; }
	
protected:
	cjBYTE						m_serialFlag;

	cjColorF					m_ambient;
	cjColorF					m_diffuse;
	cjColorF					m_specular;
	cjColorF					m_emissive;
	cjFLOAT						m_shineness;
	cjFLOAT						m_alpha;	
};



//
// cjSceneRenPropTexture
//

class CJENGINE_API cjSceneRenPropTextures : public cjSceneRenProp
{
	CJSER_DECL_CLASS(cjSceneRenPropTextures, cjSceneRenProp, 0, CJENGINE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjSceneRenPropTextures)

public:
	CJFLAG_DECL_BEGIN				
		MASK_TEX_CAOPER				= 0x00000003,		// 0~3
		BPOS_TEX_CAOPER				= 0,
	CJFLAG_DECL_END

public:
	virtual ERenProp GetRenPropType() { return ERenProp_Textures; }
	virtual void ApplyRenderInterface(cjRenInterface* ri);

public:
	enum ETexType
	{
		TEX_DIFFUSE,			// base texture, diffuse texture
		TEX_ENV,
		TEX_DETAIL,
		TEX_NORMAL,
		TEX_SPECULAR,
		TEX_GLOW,
		TEX_EXT0,
		TEX_EXT1,		
		TEX_MAX,
	};

	enum ETexColorAlphaOper			// color, alpha operation
	{
		TEXCA_OPER_TEX,				// (Color,Alpha) = (Ctex,                         Atex)		
		TEXCA_OPER_MUL_VTX,			// (Color,Alpha) = (Ctex*CVtx,                    Atex * AVtx)
		TEXCA_OPER_MUL_DECAL,		// (Color,Alpha) = (Ctex*Atex + CVtx*(1-ATex),    AVtx)
	};

public:

	ETexColorAlphaOper GetTexColorAlphaOper() const { return (ETexColorAlphaOper)_GetFlag(MASK_TEX_CAOPER, BPOS_TEX_CAOPER); }
	void SetTexColorAlphaOper(ETexColorAlphaOper caOper) { _SetFlag(MASK_TEX_CAOPER, BPOS_TEX_CAOPER, caOper); }

	//
	// Tex
	//
	class CJENGINE_API Tex : public cjRef
	{
		CJSER_DECL_CLASS(Tex, cjSerializable, 0, CJENGINE_API)
		CJSER_DECL_SERIALIZE
		CJSER_DECL_CLONE(Tex)

    public:
		// flag : [0x1] d == cjTAry
		static cjSerializer& SerializeAry(cjSerializer& ser, Tex*** d, cjINT32* count=0,
			cjTFLAG flag=0);

        Tex();
        Tex(const Tex& src);

		void Init(cjTexture* texture, cjUINT32 index, 
			cjTexClampMode uClampMode = cjTexClamp_Wrap,
			cjTexClampMode vClampMode = cjTexClamp_Wrap,
			cjTexFilterMode filterMode = cjTexFilter_Bilerp,
			cjTextureTM* texTM = 0);

        bool operator==(const Tex& dst) const;
        bool operator!=(const Tex& dst) const;

		cjTexture* GetTexObj() const { return m_texObj; }
		void SetTexObj(cjTexture* texObj) { m_texObj = texObj; }

		cjTexClampMode GetClampModeU() const { return m_uClampMode; }
		cjTexClampMode GetClampModeV() const { return m_vClampMode; }
        void SetClampModeU(cjTexClampMode uClampMode) { m_uClampMode = uClampMode; }
		void SetClampModeV(cjTexClampMode vClampMode) { m_vClampMode = vClampMode; }

		cjTexFilterMode GetFilterMode() const { return m_filterMode; }
		void SetFilterMode(cjTexFilterMode filterMode) { m_filterMode = filterMode; }

		cjWORD GetMaxAnisotropy() const { return m_maxAnisotropy; }
		void SetMaxAnisotropy(cjWORD maxAnisotropy) { m_maxAnisotropy = maxAnisotropy; }

		cjUINT32 GetTexUVSrcIndex() const { return m_texUVSrcIndex; }
		void SetTexUVSrcIndex(cjUINT32 index) { m_texUVSrcIndex = index; }

		cjTextureTM* GetTexTM() { return m_texTM; }
		const cjTextureTM* GetTexTM() const { return m_texTM; }
		void SetTexTM(cjTextureTM* texTM) { m_texTM = texTM; }

		// [RET] cjShaderMtl::TEX_TYPE
		static cjBYTE ConvertToShaderTexType(cjINT32 typeIndex);

	protected:
//		void _InitCommon();

    protected:
		cjWORD								m_maxAnisotropy;
        cjObjRef<cjTexture>					m_texObj;
        cjTextureTM*						m_texTM;

		cjTexClampMode						m_uClampMode;
		cjTexClampMode						m_vClampMode;
		cjTexFilterMode						m_filterMode;
		cjUINT32							m_texUVSrcIndex;
	};


public:
    cjSceneRenPropTextures();
	cjSceneRenPropTextures(cjTexture* baseTexture);

	Tex* GetTex(cjSceneRenPropTextures::ETexType texType) const;
	cjTexture* GetTexObj(cjSceneRenPropTextures::ETexType texType) const;
	void SetTex(cjSceneRenPropTextures::ETexType texType, cjSceneRenPropTextures::Tex* texMap);

	typedef cjTAry<cjSceneRenPropTextures::Tex*>		TexAry;

	TexAry& GetTexAry() { return m_texs; }
	const TexAry& GetTexAry() const { return m_texs; }

private:
	TexAry									m_texs;

};


//
// cjSceneRenPropVtxColor
//

class CJENGINE_API cjSceneRenPropVtxColor : public cjSceneRenProp
{
	CJSER_DECL_CLASS(cjSceneRenPropVtxColor, cjSceneRenProp, 0, CJENGINE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjSceneRenPropVtxColor)

public:
	virtual ERenProp GetRenPropType() { return ERenProp_VtxColor; }
	virtual void ApplyRenderInterface(cjRenInterface* ri);

public:
	enum ESourceVertexMode
	{
		ESRC_FROM_MTL,
		ESRC_AD_MTL_E_VC,
		ESRC_AD_VC_E_MTL,
		ESRC_RESERVED_0,
		ESRC_MAX
	};

	enum ELightingMode
	{
		ELIGHTING_EMISSIVE_ONLY,
		ELIGHTING_EMISSIVE_AMBIENT_DIFFUSE,
		ELIGHTING_RESERVED_0,
		ELIGHTING_RESERVED_1,
		ELIGHTING_MDOE_MAX
	};

CJFLAG_DECL_BEGIN
		MASK_SRC_VTX_MODE	= 0x00000003,	// 2bit
		BPOS_SRC_VTX_MODE	= 0,

		MASK_LIGHTING_MODE	= 0x0000000C,	// 2bit
		BPOS_LIGHTING_MODE	= 2,
CJFLAG_DECL_END

public:
	cjSceneRenPropVtxColor();

	void SetSourceVertexMode(ESourceVertexMode src) { _SetFlag(MASK_SRC_VTX_MODE, BPOS_SRC_VTX_MODE, src); }
	ESourceVertexMode GetSourceVertexMode() const { return (ESourceVertexMode)_GetFlag(MASK_SRC_VTX_MODE, BPOS_SRC_VTX_MODE); }

    void SetLightingMode(ELightingMode lightingMode) { _SetFlag(MASK_LIGHTING_MODE, BPOS_LIGHTING_MODE, lightingMode); }
    ELightingMode GetLightingMode() const { return (ELightingMode)_GetFlag(MASK_LIGHTING_MODE, BPOS_LIGHTING_MODE); }
};


//
// cjSceneRenPropShade
//

class CJENGINE_API cjSceneRenPropShade : public cjSceneRenProp
{
	CJSER_DECL_CLASS(cjSceneRenPropShade, cjSceneRenProp, 0, CJENGINE_API)	
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjSceneRenPropShade)

public:
	virtual ERenProp GetRenPropType() { return ERenProp_Shade; }
	virtual void ApplyRenderInterface(cjRenInterface* ri);

public:
	enum ESHADE_FLAG
	{
		// 1,2,4,8
		ESHADE_FLAT			= 0x01,  // not set for Gouraud shading
		ESHADE_WIRE			= 0x02,  //
	};

	cjSceneRenPropShade() {	m_flag = 0;	}

	void SetGouraudShade(cjBool gouraud=cjTrue) { m_flag = (m_flag & (~ESHADE_FLAT)) | (gouraud ? 0 : 0x1); }
	cjBool GetGouraudShade() const { return m_flag & ESHADE_FLAT ? cjFalse : cjTrue; }
	
protected:
	cjBYTE								m_flag;
};


//
// cjSceneRenPropSpecular
//

class CJENGINE_API cjSceneRenPropSpecular : public cjSceneRenProp
{
	CJSER_DECL_CLASS(cjSceneRenPropSpecular, cjSceneRenProp, 0, CJENGINE_API)	
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjSceneRenPropSpecular)

public:
	virtual ERenProp GetRenPropType() { return ERenProp_Specular; }
	virtual void ApplyRenderInterface(cjRenInterface* ri);

public:
	cjSceneRenPropSpecular() { m_useSpecular = cjFalse; }

	void SetSpecular(cjBool useSpecular) { m_useSpecular = useSpecular; }
	cjBool GetSpecular() const { return m_useSpecular; }

protected:
	cjBool								m_useSpecular;
};


//
// cjSceneRenPropWireframe
//

class CJENGINE_API cjSceneRenPropWireframe : public cjSceneRenProp
{
	CJSER_DECL_CLASS(cjSceneRenPropWireframe, cjSceneRenProp, 0, CJENGINE_API)	
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjSceneRenPropWireframe)

public:
	virtual ERenProp GetRenPropType() { return ERenProp_WireFrame; }
	virtual void ApplyRenderInterface(cjRenInterface* ri);

public:
	cjSceneRenPropWireframe() { m_useWireframe = cjFalse; }

	void SetUseWireframe(cjBool useWireframe) { m_useWireframe = useWireframe; }
	cjBool GetUseWireframe() const { return m_useWireframe; }

protected:
	cjBool								m_useWireframe;
};


//
// cjSceneRenPropTwoSide
//

class CJENGINE_API cjSceneRenPropTwoSide : public cjSceneRenProp
{
	CJSER_DECL_CLASS(cjSceneRenPropTwoSide, cjSceneRenProp, 0, CJENGINE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjSceneRenPropTwoSide)

public:
	virtual ERenProp GetRenPropType() { return ERenProp_TwoSide; }
	virtual void ApplyRenderInterface(cjRenInterface* ri);

public:
	cjSceneRenPropTwoSide() { m_disableTwoSide = cjFalse; }

	void SetUseTwoSide(cjBool use) { m_disableTwoSide = !use; }
	cjBool GetUseTwoSide() const { return !m_disableTwoSide; }

protected:
	cjBool								m_disableTwoSide;
};


#endif

