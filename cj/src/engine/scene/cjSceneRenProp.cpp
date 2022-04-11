
#include "cjEnginePCH.h"
#include "cjSceneRenProp.h"


//
// cjSceneRenProp
//

CJSER_IMPL_CLASS(cjSceneRenProp, cjSceneProp)
	
CJSER_IMPL_SERIALIZE_BEGIN(cjSceneRenProp)
CJSER_IMPL_SERIALIZE_END(cjSceneRenProp)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjSceneRenProp)
CJSER_IMPL_CLONE_PROPCOPY_END(cjSceneRenProp)


cjSceneRenProp::~cjSceneRenProp()
{
}


//
// cjSceneRenPropAlpha
//

CJSER_IMPL_CLASS(cjSceneRenPropAlpha, cjSceneRenProp)

CJSER_IMPL_SERIALIZE_BEGIN(cjSceneRenPropAlpha)

	if(ser.IsLoading() || ser.IsSaving())
	{
		cjPODSerializeWithType<cjBYTE>(ser, m_refAlphaTest);

		cjBYTE v0;
		cjBYTE v1;

		if(ser.IsSaving())
		{
			v0 = (m_srcBlend & 0xf) | (m_dstBlend & 0xf) << 4;
			v1 = (m_funcAlphaTest & 0xf) << 4;
			v1 |= m_useAlphaTest ? 0x01 : 0;
			v1 |= m_useAlphaBlend ? 0x10 : 0;
		}		
		
		ser << v0;
		ser << v1;
		
		if(ser.IsLoading())
		{
			m_srcBlend = (EALPHABL)(v0 & 0xf);
			m_dstBlend = (EALPHABL)((v0 & 0xf0) >> 4);

			m_funcAlphaTest	= (EALPHATESTFN)((v1 & 0xf0) >> 4);
			m_useAlphaTest	= v1 & 0x01 ? cjTrue : cjFalse;
			m_useAlphaBlend = v1 & 0x10 ? cjTrue : cjFalse;
		}
	}

CJSER_IMPL_SERIALIZE_END(cjSceneRenPropAlpha)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjSceneRenPropAlpha)

	d->m_refAlphaTest	= m_refAlphaTest;

	d->m_srcBlend		= m_srcBlend;
	d->m_dstBlend		= m_dstBlend;

	d->m_funcAlphaTest	= m_funcAlphaTest;
	d->m_useAlphaTest	= m_useAlphaTest;
	d->m_useAlphaBlend	= m_useAlphaBlend;

CJSER_IMPL_CLONE_PROPCOPY_END(cjSceneRenPropAlpha)

CJSER_IMPL_CLONE(cjSceneRenPropAlpha)

cjSceneRenPropAlpha::cjSceneRenPropAlpha()
{
	m_useAlphaBlend = cjFalse;
	m_srcBlend = EALPHABL_SRCALPHA;
	m_dstBlend = EALPHABL_INVSRCALPHA;

	m_refAlphaTest = 0;
	m_funcAlphaTest = EALPHATESTFN_ALWAYS;
	m_useAlphaTest = cjFalse;
}

cjSceneRenPropAlpha::~cjSceneRenPropAlpha()
{
}

void cjSceneRenPropAlpha::ApplyRenderInterface(cjRenInterface* ri)
{

}


//
// cjSceneRenPropFog
//

CJSER_IMPL_CLASS(cjSceneRenPropFog, cjSceneRenProp)

CJSER_IMPL_SERIALIZE_BEGIN(cjSceneRenPropFog)

	if(ser.IsLoadOrSaving())
	{
		CJFLAG_SERIALIZE

		ser << m_depth;
		ser << m_color;
	}

CJSER_IMPL_SERIALIZE_END(cjSceneRenPropFog)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjSceneRenPropFog)

	CJFLAG_CLONE

	d->m_depth = m_depth;
	d->m_color = m_color;

CJSER_IMPL_CLONE_PROPCOPY_END(cjSceneRenPropFog)

CJSER_IMPL_CLONE(cjSceneRenPropFog)

cjSceneRenPropFog::cjSceneRenPropFog()
{
	CJFLAG_IMPL_INIT

	SetUseFog(cjFalse);
	SetFogAttenuType(FOG_ATTENU_ZLINEAR);

	m_depth = 1.0f;
	m_color = cjColorF::BLACK;
}

cjSceneRenPropFog::~cjSceneRenPropFog()
{
}

void cjSceneRenPropFog::ApplyRenderInterface(cjRenInterface* ri)
{
}


//
// cjSceneRenPropMaterial
//

CJSER_IMPL_CLASS(cjSceneRenPropMaterial, cjSceneRenProp)
	
CJSER_IMPL_SERIALIZE_BEGIN(cjSceneRenPropMaterial)

	if(ser.IsLoading() || ser.IsSaving())
	{
		cjColor bc;
		ser << m_serialFlag;

		cjColorFSerHelper(ser, m_ambient,	m_serialFlag & EAMB);
		cjColorFSerHelper(ser, m_diffuse,	m_serialFlag & EDIF);
		cjColorFSerHelper(ser, m_specular,	m_serialFlag & ESPEC);
		cjColorFSerHelper(ser, m_emissive,	m_serialFlag & EEMIS);

		cjFloatSerHelper(ser, m_shineness,	m_serialFlag & ESHI);
		cjFloatSerHelper(ser, m_alpha	 ,	m_serialFlag & EALPH);
	}
		
CJSER_IMPL_SERIALIZE_END(cjSceneRenPropMaterial)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjSceneRenPropMaterial)

	d->m_serialFlag = m_serialFlag;
					
	d->m_ambient	= m_ambient;
	d->m_diffuse	= m_diffuse;
	d->m_specular	= m_specular;
	d->m_emissive	= m_emissive;
	d->m_shineness	= m_shineness;
	d->m_alpha		= m_alpha;	

CJSER_IMPL_CLONE_PROPCOPY_END(cjSceneRenPropMaterial)

CJSER_IMPL_CLONE(cjSceneRenPropMaterial)


cjSceneRenPropMaterial::cjSceneRenPropMaterial()
{
	m_serialFlag = 0;
}

cjSceneRenPropMaterial::~cjSceneRenPropMaterial()
{
}

void cjSceneRenPropMaterial::ApplyRenderInterface(cjRenInterface* ri)
{

}


//
// cjSceneRenPropTextures
//

CJSER_IMPL_CLASS(cjSceneRenPropTextures, cjSceneRenProp)

CJSER_IMPL_SERIALIZE_BEGIN(cjSceneRenPropTextures)

	if(ser.IsQuerySerObjAndStr())
	{
		Tex::SerializeAry(ser, (Tex***)&m_texs, 0, 0x1);
	}
	else if(ser.IsLoadOrSaving())
	{
		CJFLAG_SERIALIZE
		Tex::SerializeAry(ser, (Tex***)&m_texs, 0, 0x1);
	}

CJSER_IMPL_SERIALIZE_END(cjSceneRenPropTextures)
	
CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjSceneRenPropTextures)
	
	CJFLAG_CLONE

	d->m_texs.PushZero(cjSceneRenPropTextures::TEX_MAX);

	for(cjAryOff i=0; i<m_texs.Count(); i++)
	{
		cjSceneRenPropTextures::Tex* texMap = m_texs(i);

		if(texMap)
			d->m_texs(i) = (cjSceneRenPropTextures::Tex*)texMap->Clone();
		else
			d->m_texs(i) = cjNULL;
	}

CJSER_IMPL_CLONE_PROPCOPY_END(cjSceneRenPropTextures)

CJSER_IMPL_CLONE(cjSceneRenPropTextures)


cjSceneRenPropTextures::cjSceneRenPropTextures()
{
	CJFLAG_IMPL_INIT

	m_texs.PushZero(cjSceneRenPropTextures::TEX_MAX);	
	SetTexColorAlphaOper(TEXCA_OPER_MUL_VTX);
}

cjSceneRenPropTextures::~cjSceneRenPropTextures()
{
	for(cjAryOff i=0; i<m_texs.Num(); i++)
		CJSER_SAFE_RELEASE(m_texs(i));
}

cjSceneRenPropTextures::cjSceneRenPropTextures(cjTexture* baseTexture)
{
	CJFLAG_IMPL_INIT

	if(baseTexture)
	{
		Tex* newMap = new Tex();
		newMap->AddRef();

		newMap->Init(baseTexture, 0);
		m_texs(TEX_DIFFUSE) = newMap;
	}

	SetTexColorAlphaOper(TEXCA_OPER_MUL_VTX);
}

void cjSceneRenPropTextures::ApplyRenderInterface(cjRenInterface* ri)
{
	for(cjAryOff i=0; i<m_texs.Count(); i++)
	{
		Tex* tex = m_texs(i);

		if(tex)
		{
			if(i == TEX_DIFFUSE)
			{
				cjTexture* texObj = tex->GetTexObj();
				ri->SetMaterial(texObj);
			}
		}
	}
}

cjSceneRenPropTextures::Tex* cjSceneRenPropTextures::GetTex(cjSceneRenPropTextures::ETexType texType) const
{
	return m_texs(texType);	
}

cjTexture* cjSceneRenPropTextures::GetTexObj(cjSceneRenPropTextures::ETexType texType) const
{
	Tex* tex = m_texs(texType);

	if(!tex)
		return cjNULL;

	return tex->GetTexObj();
}

void cjSceneRenPropTextures::SetTex(cjSceneRenPropTextures::ETexType texType, cjSceneRenPropTextures::Tex* tex)
{
	CJSER_SAFE_RELEASE(m_texs(texType));
	m_texs(texType) = tex;
	tex->AddRef();
}



//
//  cjSceneRenPropTextures::Tex
//

CJSER_IMPL_CLASS(cjSceneRenPropTextures::Tex, cjSerializable)

CJSER_IMPL_SERIALIZE_BEGIN(cjSceneRenPropTextures::Tex)

	if(ser.IsQuerySerObjAndStr())
	{
		ser.AddCJObj(m_texObj);

		if(m_texTM)
			(*m_texTM) >> ser;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		// cjdefer 
		// serialize other texmap stuff
		cjPODSerializeWithType<cjBYTE>(ser, m_maxAnisotropy);		
		
		ser.SerializeCJObjRef(m_texObj);
		ser.SerializeObjNoPost(m_texTM);

		cjPODSerializeWithType<cjBYTE>(ser, m_uClampMode);
		cjPODSerializeWithType<cjBYTE>(ser, m_vClampMode);
		cjPODSerializeWithType<cjBYTE>(ser, m_filterMode);
		cjPODSerializeWithType<cjBYTE>(ser, m_texUVSrcIndex);
	}

CJSER_IMPL_SERIALIZE_END(cjSceneRenPropTextures::Tex)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjSceneRenPropTextures::Tex)

	d->m_maxAnisotropy		= m_maxAnisotropy;
	d->m_texObj				= m_texObj;
	d->m_texTM				= cjNULL;

	if(m_texTM)
		d->m_texTM = (cjTextureTM*)m_texTM->Clone();
							   
	d->m_uClampMode			= m_uClampMode;
	d->m_vClampMode			= m_vClampMode;
	d->m_filterMode			= m_filterMode;
	d->m_texUVSrcIndex		= m_texUVSrcIndex;

CJSER_IMPL_CLONE_PROPCOPY_END(cjSceneRenPropTextures::Tex)

CJSER_IMPL_CLONE(cjSceneRenPropTextures::Tex)

//void cjSceneRenPropTextures::Tex::_InitCommon()
//{
//	m_maxAnisotropy = 1;
//	m_texObj = 0;
//	m_texTM = 0;
//
//	m_uClampMode = cjTexClamp_Wrap;
//	m_vClampMode = cjTexClamp_Wrap;
//	m_filterMode = cjTexFilter_Bilerp;
//	m_texUVSetIndex = 0;
//}

cjSceneRenPropTextures::Tex::Tex()
{
	Init(cjNULL, 0);
//	m_texTM = 0;
}

cjSceneRenPropTextures::Tex::~Tex()
{
	CJSER_SAFE_DELETE(m_texTM);
}

cjSerializer& cjSceneRenPropTextures::Tex::SerializeAry(cjSerializer& ser, 
	Tex*** d, cjINT32* count, cjTFLAG flag)
{
	cjTAry<Tex*>& tmAry = *((cjTAry<Tex*>*)d);
	cjINT32 datCount = count ? *count : 0;

	if(flag & 0x1)
		datCount = tmAry.Num();

	if(ser.IsQuerySerObjAndStr())
	{
		for(cjINT32 i=0; i<datCount; i++)
		{
			Tex* cur;

			if(flag & 0x1) 
				cur = tmAry(i);
			else 
				cur = ((*d)[i]);

			if(cur)
				*cur >> ser;
		}
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		ser << cjFStreamCmprI32(datCount);

		if(ser.IsLoading())
		{
			if(flag & 0x1)
			{				
				tmAry.RemoveAll(datCount, cjTrue);
				
				cjSerID serID;
				for(cjINT32 i=0; i<datCount; i++)
				{
					cjSerID_Serialize(ser, serID);

					Tex* cur = ser.GetSerObj<Tex>(serID);
					tmAry(i) = cur;
					if(cur)
						cur->AddRef();
				}
			}
			else
			{
				cjAssert(*d == 0);
				cjAssert(!"not impl!");
			}

			if(count)
				*count = datCount;
		}
		else if(ser.IsSaving())
		{
			for(cjINT32 i=0; i<datCount; i++)
			{
				Tex* cur; 
			
				if(flag & 0x1)
					cur = tmAry(i);
				else
					cur = ((*d)[i]);

				cjSerID serID = ser.GetSerObjID(cur);
				cjSerID_Serialize(ser, serID);
			}
		}
	}

	return ser;		
}

void cjSceneRenPropTextures::Tex::Init(cjTexture* tex, cjUINT32 index, 
 cjTexClampMode uClampMode, cjTexClampMode vClampMode, cjTexFilterMode filterMode,
 cjTextureTM* texTM)
{
	m_maxAnisotropy = 1;
	m_texObj = cjNULL;
	m_texTM = cjNULL;

	m_uClampMode = uClampMode;
	m_vClampMode = vClampMode;
	m_filterMode = filterMode;
	m_texUVSrcIndex = 0;
}
 
cjSceneRenPropTextures::Tex::Tex(const Tex& src)
{
//	_InitCommon();
	const cjTextureTM* srcTexTM = src.GetTexTM();

	if(srcTexTM)
	{
		m_texTM = cjTextureTM::NewInst();
		m_texTM->Init(srcTexTM->GetUVOffset(), srcTexTM->GetUVAxisWRot(),
			srcTexTM->GetUVScale(), srcTexTM->GetUVRotCenter(), srcTexTM->GetTextureTMFlag());
	}
}

bool cjSceneRenPropTextures::Tex::operator==(const cjSceneRenPropTextures::Tex& dst) const
{
	if(m_maxAnisotropy	!= dst.m_maxAnisotropy || 
		m_texObj			!= dst.m_texObj ||
		m_uClampMode	!= dst.m_uClampMode ||
		m_vClampMode	!= dst.m_vClampMode ||
		m_filterMode	!= dst.m_filterMode ||
		m_texUVSrcIndex	!= dst.m_texUVSrcIndex
		)
		return false;

	if(m_texTM && dst.m_texTM)
	{
		if(*m_texTM != *dst.m_texTM)
			return false;
	}
	else if(!m_texTM || !dst.m_texTM)
		return false;

	return true;
}

bool cjSceneRenPropTextures::Tex::operator!=(const cjSceneRenPropTextures::Tex& dst) const
{
	return !operator==(dst);
}

cjBYTE cjSceneRenPropTextures::Tex::ConvertToShaderTexType(cjINT32 typeIndex)
{
	return (cjBYTE)typeIndex;
}

//
// cjSceneRenPropVtxColor
//

CJSER_IMPL_CLASS(cjSceneRenPropVtxColor, cjSceneRenProp)

CJSER_IMPL_SERIALIZE_BEGIN(cjSceneRenPropVtxColor)

	if(ser.IsLoadOrSaving())
	{
		CJFLAG_SERIALIZE
	}

CJSER_IMPL_SERIALIZE_END(cjSceneRenPropVtxColor)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjSceneRenPropVtxColor)

	CJFLAG_CLONE

CJSER_IMPL_CLONE_PROPCOPY_END(cjSceneRenPropVtxColor)

CJSER_IMPL_CLONE(cjSceneRenPropVtxColor)

cjSceneRenPropVtxColor::cjSceneRenPropVtxColor()
{
	CJFLAG_IMPL_INIT

	_SetFlag(MASK_SRC_VTX_MODE, BPOS_SRC_VTX_MODE, ESRC_FROM_MTL);
	_SetFlag(MASK_LIGHTING_MODE, BPOS_LIGHTING_MODE, ELIGHTING_EMISSIVE_AMBIENT_DIFFUSE);
}

cjSceneRenPropVtxColor::~cjSceneRenPropVtxColor()
{
}

void cjSceneRenPropVtxColor::ApplyRenderInterface(cjRenInterface* ri)
{
}


//
// cjSceneRenPropShade
//

CJSER_IMPL_CLASS(cjSceneRenPropShade, cjSceneRenProp)

CJSER_IMPL_SERIALIZE_BEGIN(cjSceneRenPropShade)

	if(ser.IsLoading() || ser.IsSaving())
	{
		ser << m_flag;
	}

CJSER_IMPL_SERIALIZE_END(cjSceneRenPropShade)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjSceneRenPropShade)
	d->m_flag = m_flag;
CJSER_IMPL_CLONE_PROPCOPY_END(cjSceneRenPropShade)

CJSER_IMPL_CLONE(cjSceneRenPropShade)


cjSceneRenPropShade::~cjSceneRenPropShade()
{
}

void cjSceneRenPropShade::ApplyRenderInterface(cjRenInterface* ri)
{
}

//
// cjSceneRenPropSpecular
//

CJSER_IMPL_CLASS(cjSceneRenPropSpecular, cjSceneRenProp)

CJSER_IMPL_SERIALIZE_BEGIN(cjSceneRenPropSpecular)

	if(ser.IsLoading() || ser.IsSaving())
	{
		ser << m_useSpecular;
	}

CJSER_IMPL_SERIALIZE_END(cjSceneRenPropSpecular)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjSceneRenPropSpecular)
	d->m_useSpecular = m_useSpecular;
CJSER_IMPL_CLONE_PROPCOPY_END(cjSceneRenPropSpecular)

CJSER_IMPL_CLONE(cjSceneRenPropSpecular)


cjSceneRenPropSpecular::~cjSceneRenPropSpecular()
{
}

void cjSceneRenPropSpecular::ApplyRenderInterface(cjRenInterface* ri)
{
}


//
// cjSceneRenPropWireframe
//

CJSER_IMPL_CLASS(cjSceneRenPropWireframe, cjSceneRenProp)

CJSER_IMPL_SERIALIZE_BEGIN(cjSceneRenPropWireframe)

if(ser.IsLoading() || ser.IsSaving())
	{
		ser << m_useWireframe;
	}

CJSER_IMPL_SERIALIZE_END(cjSceneRenPropWireframe)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjSceneRenPropWireframe)

	d->m_useWireframe = m_useWireframe;

CJSER_IMPL_CLONE_PROPCOPY_END(cjSceneRenPropWireframe)

CJSER_IMPL_CLONE(cjSceneRenPropWireframe)


cjSceneRenPropWireframe::~cjSceneRenPropWireframe()
{
}

void cjSceneRenPropWireframe::ApplyRenderInterface(cjRenInterface* ri)
{
}


//
// cjSceneRenPropTwoSide
//

CJSER_IMPL_CLASS(cjSceneRenPropTwoSide, cjSceneRenProp)

CJSER_IMPL_SERIALIZE_BEGIN(cjSceneRenPropTwoSide)

	if(ser.IsLoading() || ser.IsSaving())
	{
		ser << m_disableTwoSide;
	}

CJSER_IMPL_SERIALIZE_END(cjSceneRenPropTwoSide)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjSceneRenPropTwoSide)

	d->m_disableTwoSide = m_disableTwoSide;

CJSER_IMPL_CLONE_PROPCOPY_END(cjSceneRenPropTwoSide)

CJSER_IMPL_CLONE(cjSceneRenPropTwoSide)

cjSceneRenPropTwoSide::~cjSceneRenPropTwoSide()
{
}

void cjSceneRenPropTwoSide::ApplyRenderInterface(cjRenInterface* ri)
{
}

