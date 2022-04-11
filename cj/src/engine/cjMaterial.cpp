
#include "cjEnginePCH.h"
#include "cjMaterial.h"


//
// cjMaterial
//

CJOBJ_IMPL_CLASS(cjMaterial)
CJOBJ_IMPL_SERIALIZE_BEGIN(cjMaterial)

	cjPODSerializeWithType<cjTFLAG>(stream, m_typeMaterial);
	cjObjSerialize(stream, m_fallbackMaterial);
	cjObjSerialize(stream, m_defaultMaterial);
	stream << m_useFallbackMaterial;

CJOBJ_IMPL_SERIALIZE_END(cjMaterial)


cjMaterial::cjMaterial()
{
}

cjBool cjMaterial::CheckCircularRef(cjTAry<cjMaterial*>& materials)
{
    if(materials.FindItemIndex(this) != cjIndex_None)
		return cjFalse;
    
	if(m_fallbackMaterial)
	{
		cjINT32 newIndex = materials.PushEmpty();
		materials(newIndex) = this;
        
		if(!m_fallbackMaterial->CheckCircularRef(materials))
			return cjFalse;
        
		materials.Remove(newIndex);
	}
    
	return cjTrue;
}

cjMaterial* cjMaterial::CheckFallback()
{
	if(m_fallbackMaterial && m_useFallbackMaterial)
		return m_fallbackMaterial->CheckFallback();
    
	return this;
}

void cjMaterial::ClearFallbacks()
{
	for(cjTObjIterator<cjMaterial> it; it; ++it)
	{
		it->m_useFallbackMaterial = 0;
	}
}

void cjMaterial::OnDestroy()
{
    cjSuper::OnDestroy();
}


//
// cjMtlModifier
//

CJOBJ_IMPL_CLASS(cjMtlModifier);

cjBool cjMtlModifier::CheckCircularRef(cjTAry<cjMaterial*>& materials)
{
	if(!cjSuper::CheckCircularRef(materials))
		return cjFalse;
	
	cjINT32 newIndex = materials.PushEmpty();
	materials(newIndex) = this;

	if(m_material && !m_material->CheckCircularRef(materials))
		return cjFalse;

	materials.Remove(newIndex);

	return cjTrue;
}


//
// cjColorMtlModifier
//

CJOBJ_IMPL_CLASS(cjColorMtlModifier);




//
// cjFinalBlendMtlModifier
//

CJOBJ_IMPL_CLASS(cjFinalBlendMtlModifier);

cjBool cjFinalBlendMtlModifier::CheckNeedSorting()
{
    if(m_frameBufferBlend == cjRFB_AlphaBlend || m_frameBufferBlend == cjRFB_Modulate)
       return cjTrue;

	return cjFalse;
}

cjBool cjFinalBlendMtlModifier::IsTransparent()
{
	return CheckNeedSorting();
}


//
// cjTexMtlModifier
//

CJOBJ_IMPL_CLASS(cjTexMtlModifier);

cjINT32 cjTexMtlModifier::GetMaterialUSiz()
{
	cjINT32 sizU = 0;

    if(m_material == this)
    {
        cjLOG("WARN! - GetMaterialUSiz Recursion..!");
        return sizU;
    }

	if(m_material)
		sizU = m_material->GetMaterialUSiz();
    
	return sizU;
}

cjINT32 cjTexMtlModifier::GetMaterialVSiz()
{
	cjINT32 sizV = 0;

    if(m_material == this)
    {
        cjLOG("WARN! - GetMaterialVSiz Recursion..!");
        return sizV;
    }
    
	if(m_material)
		sizV = m_material->GetMaterialVSiz();
    
	return sizV;
}

cjBool cjTexMtlModifier::CheckNeedUVComp()
{
    cjBool rst = cjTrue;

	if(m_texCoordSource >= cjTexCoordSrc0 && m_texCoordSource <= cjTexCoordSrc7)
    {
        if(m_material)
            rst = m_material->CheckNeedUVComp();
   
        // 1 << 0 = 1
        if(!rst && (1 << (m_texCoordSource - cjTexCoordSrc0)))
            rst = true;
    }
	else
		rst = m_material ? m_material->CheckNeedUVComp() : cjTrue;
    
    return rst;
}

cjBool cjTexMtlModifier::CheckNeedNormalComp()
{
	if(m_texCoordSource == cjTexCoordSrcEnvMapWorld || m_texCoordSource == cjTexCoordSrcEnvMapCamera)
		return cjTrue;

	return cjFalse;
}


//
// cjTexCoordSrcStreamMtlModifer
//

CJOBJ_IMPL_CLASS(cjTexCoordSrcStreamMtlModifer);


void cjTexCoordSrcStreamMtlModifer::cjObjPostEditChange()
{
	cjSuper::cjObjPostEditChange();

	m_uvStreamIndex = cjClamp(m_uvStreamIndex, (cjBYTE)0, (cjBYTE)7);
    
    // update cjTexMtlModifer
	m_texCoordSource = (cjBYTE)(cjTexCoordSrc0 + m_uvStreamIndex);
}

//
// cjTexMatrixMtlModifier
//

CJOBJ_IMPL_CLASS(cjTexMatrixMtlModifier);


//
// cjTexEnvMapMtlModifier
//
CJOBJ_IMPL_CLASS(cjTexEnvMapMtlModifier);

cjMatrix* cjTexEnvMapMtlModifier::GetMatrix(cjEngineTime_ft timeSec)
{
	switch(m_envMapType)
	{
	case cjTexEnv_WorldSpace:  m_texCoordSource = cjTexCoordSrcEnvMapWorld; break;
	case cjTexEnv_CameraSpace: m_texCoordSource = cjTexCoordSrcEnvMapCamera; break;
    default: break;
	}

	return NULL;
}


//
// cjRenderedMaterial
//

CJOBJ_IMPL_CLASS(cjRenderedMaterial)


//
// cjBitmapMaterial
//

CJOBJ_IMPL_CLASS(cjBitmapMaterial)
CJOBJ_IMPL_SERIALIZE_BEGIN(cjBitmapMaterial)
	cjPODSerializeWithType<cjBYTE>(stream, m_format);
	cjPODSerializeWithType<cjBYTE>(stream, m_texUClamp);
	cjPODSerializeWithType<cjBYTE>(stream, m_texVClamp);
	
	stream << m_potbitU;
	stream << m_potbitV;

	stream << cjFStreamCmprI32(m_sizU);
	stream << cjFStreamCmprI32(m_sizV);

CJOBJ_IMPL_SERIALIZE_END(cjBitmapMaterial)


cjBitmapMaterial::cjBitmapMaterial()
{

}

