
#include "cjEnginePCH.h"
#include "cjShaderMtlDesc.h"


//
// cjShaderMtlDefaultDesc
//

cjBool cjShaderMtlDefaultDesc::CollectTransform(const cjFVertexStream** streams, cjINT32 streamCount, 
	cjBool& rstHWSkinned)
{
	rstHWSkinned = cjFalse;

	cjINT32 type = 0;

	if(streams[0])
	{
		const cjFVertexDecl* vtxDecl = streams[0]->GetVertexDecl();

		if(!vtxDecl)
			return cjFalse;
	
		cjBool hasBlendInfo = vtxDecl->HasComp(cjVtxComp_BL4Index) && vtxDecl->HasComp(cjVtxComp_BL4Weight);

		if(hasBlendInfo)
			type = 1;
		else
			type = 0;
	}
	
	switch(type)
	{
	default:
	case 0:	
		SetTM(TM_DEFAULT);	
		break;

	case 1:
		{
			SetTM(TM_SKINNED);
			rstHWSkinned = cjTrue;
		}
		break;

	case 2: SetTM(TM_INSTANCED); 
		break;	
	}

	return cjTrue;
}

cjBool cjShaderMtlDefaultDesc::CollectNBT(const cjFVertexStream** streams, cjINT32 streamCount, 
	cjBool hwSkinned, cjBool& rstVtxNormalExist, cjBool& rstVtxNBTExist)
{
	cjBool vtxNormal = cjFalse;
	cjBool binormal = cjFalse;
	cjBool tangent = cjFalse;

	const cjFVertexDecl* vtxDecl = cjNULL;
	
	if(streams[0])
		vtxDecl = streams[0]->GetVertexDecl();

	if(vtxDecl)
	{
		vtxNormal = vtxDecl->HasComp(cjVtxComp_Normal);
	
		if(vtxNormal)
		{
			tangent = vtxDecl->HasComp(cjVtxComp_Tangent);
			binormal = vtxDecl->HasComp(cjVtxComp_BiNormal);
		}

		rstVtxNormalExist = vtxNormal;
		rstVtxNBTExist = vtxNormal && binormal && tangent;

		if(rstVtxNBTExist)
			SetNORMAL_FROM(NRM_FROM_NBT);
		else if(rstVtxNormalExist)
			SetNORMAL_FROM(NRM_FROM_VTX);
		else
			SetNORMAL_FROM(NRM_FROM_NONE);
	}
		
	return cjTrue;
}

cjBool cjShaderMtlDefaultDesc::CollectVertexColor(const cjFVertexStream** streams, cjINT32 streamCount, 
	cjBool& rstUseVtxColorExist)
{
	rstUseVtxColorExist = cjFalse;

	if(streams[0])
		rstUseVtxColorExist = streams[0]->GetVertexDecl()->HasComp(cjVtxComp_Diffuse);

	SetVTXCOLOR_FROM(rstUseVtxColorExist);

    return cjTrue;
}

cjBool cjShaderMtlDefaultDesc::CollectSceneRenProp(const cjFVertexStream** streams, cjINT32 streamCount,
	const cjScenePropRPtrList& scenePropList)
{
	cjScenePropRPtrList::Iter iter = scenePropList.Begin();
	cjScenePropRPtr v;

	while(scenePropList.GetNextV(iter, v))
	{
		cjSceneRenProp* sceneRenProp = (cjSceneRenProp*)v->GetRenProp();

		if(sceneRenProp)
		{
			cjSceneRenProp::ERenProp renPropType = sceneRenProp->GetRenPropType();

			switch(renPropType)
			{
			case cjSceneRenProp::ERenProp_Alpha:
				{
					cjSceneRenPropAlpha* alpha = (cjSceneRenPropAlpha*)sceneRenProp;

					if(alpha->GetAlphaTestUse() && 
						(alpha->GetAlphaTestFunc() != cjSceneRenPropAlpha::EALPHATESTFN_ALWAYS))
					{
						SetALPHATEST(ALPHATEST_USE);
					}
					else
					{
						SetALPHATEST(ALPHATEST_NONE);
					}
				}
				break;

			case cjSceneRenProp::ERenProp_Fog:
				{
					cjSceneRenPropFog* fog = (cjSceneRenPropFog*)sceneRenProp;

					if(fog->GetUseFog())
					{
						switch(fog->GetFogAttenuType())
						{
						case cjSceneRenPropFog::FOG_ATTENU_ZLINEAR:
							SetFOG_TYPE(FOG_TYPE_LINEAR);
							break;

						case cjSceneRenPropFog::FOG_ATTENU_RANGE_SQ:
							SetFOG_TYPE(FOG_TYPE_SQUARE);
							break;

						default:
							SetFOG_TYPE(FOG_TYPE_NONE);
							break;
						}
					}
					else
					{
						SetFOG_TYPE(FOG_TYPE_NONE);
					}
				}
				break;

			case cjSceneRenProp::ERenProp_Material:
				{
					cjSceneRenPropMaterial* mtl = (cjSceneRenPropMaterial*)sceneRenProp;
					// cjSceneRenPropMaterial is handled with cjSceneRenPropVtxColor
					// no need to do any
				}
				break;

			case cjSceneRenProp::ERenProp_Specular:
				{
					cjSceneRenPropSpecular* specular = (cjSceneRenPropSpecular*)sceneRenProp;

					if(specular->GetSpecular())
						SetSPECULAR(SPECULAR_USE);
					else
						SetSPECULAR(SPECULAR_NONE);
				}
				break;

			case cjSceneRenProp::ERenProp_Textures:
				{
					HandleSceneRenPropTex(streams, streamCount, (cjSceneRenPropTextures*)sceneRenProp);
				}
				break;

			case cjSceneRenProp::ERenProp_VtxColor:
				{					
					if(streamCount != 0)
					{
						cjSceneRenPropVtxColor* renPropVtxColor = (cjSceneRenPropVtxColor*)sceneRenProp;

						cjBool vtxColorComp = streams[0]->GetVertexDecl()->HasComp(cjVtxComp_Diffuse);

						if(vtxColorComp)
						{
							cjSceneRenPropVtxColor::ESourceVertexMode srcVtxMode = renPropVtxColor->GetSourceVertexMode();
						
							if(srcVtxMode == cjSceneRenPropVtxColor::ESRC_AD_VC_E_MTL)
							{
								SetADE_FROM(ADE_FROM_AD_VC_E_MTL);
							}
							else if(srcVtxMode == cjSceneRenPropVtxColor::ESRC_AD_MTL_E_VC)
							{
								SetADE_FROM(ADE_FROM_AD_MTL_E_VC);
							}
							else if(srcVtxMode == cjSceneRenPropVtxColor::ESRC_FROM_MTL)
							{
								SetADE_FROM(ADE_FROM_MTL);
							}
						}
						else
						{
							// assume srcVtxMode as ESRC_IGNORE
							SetADE_FROM(ADE_FROM_MTL);
						}

						SetLIGHTING(renPropVtxColor->GetLightingMode());
					}
				}
				break;

			default:
				break;
			}
		}
	}

    return cjTrue;
}

cjUINT32 cjShaderMtlDefaultDesc::GetTextureCount() const
{
	cjUINT32 count = GetTEX_DIFFUSE_CNT() 
		+ GetTEX_ENV_CNT()
		+ GetTEX_DETAIL_CNT() 
		+ GetTEX_NORMAL_CNT()
		+ GetTEX_SPECULAR_CNT() 
		+ GetTEX_GLOW_CNT() 
		+ GetTEX_EXT0_CNT() 
		+ GetTEX_EXT1_CNT();

    return count;	
}

void cjShaderMtlDefaultDesc::HandleSceneRenPropTex(const cjFVertexStream** streams, cjINT32 streamCount, 
	cjSceneRenPropTextures* texProp)
{	
	cjSceneRenPropTextures::Tex* collectedTexAry[cjShaderMtlDefault::TEX_MAX] = {0};
	cjSceneRenPropTextures::ETexColorAlphaOper texCAOper = texProp->GetTexColorAlphaOper();

	switch(texCAOper)
	{
	case cjSceneRenPropTextures::TEXCA_OPER_TEX:
		SetTEX_LIGHT_OPER(TEX_LIGHT_OPER_TEX_ONLY);
		break;

	default: // fall-through
	case cjSceneRenPropTextures::TEXCA_OPER_MUL_DECAL: // fall-through
	case cjSceneRenPropTextures::TEXCA_OPER_MUL_VTX:
		SetTEX_LIGHT_OPER(TEX_LIGHT_OPER_MUL);
		break;
	}

	cjUINT32 uvSetCount = 0;
	
	if(streams[0])
		uvSetCount = streams[0]->GetVertexDecl()->GetUVCompCount();

	cjBool useSpecular = GetSPECULAR() ? cjTrue : cjFalse;
	cjBool useEnv = cjFalse;
	cjUINT32 collectedTexCount = 0;
			
	if(uvSetCount != 0)
	{
		HandleTexDiffuseDesc(texProp, collectedTexAry, collectedTexCount);
		HandleTexDetailDesc(texProp, collectedTexAry, collectedTexCount);		
		HandleTexNormalDesc(texProp, collectedTexAry, collectedTexCount);
		HandleTexSpecularDesc(texProp, collectedTexAry, collectedTexCount, useEnv, useSpecular);
		HandleTexGlowDesc(texProp, collectedTexAry, collectedTexCount);
		HandleTexExt0Desc(texProp, collectedTexAry, collectedTexCount);
		HandleTexExt1Desc(texProp, collectedTexAry, collectedTexCount);
		HandleTexEnvDesc(texProp, collectedTexAry, collectedTexCount);

		HandleTexUVDesc(texProp, collectedTexAry, cjShaderMtlDefault::TEX_MAX, uvSetCount, collectedTexCount);
	}

}

cjBool cjShaderMtlDefaultDesc::HandleTexDiffuseDesc(const cjSceneRenPropTextures* texProp,
    cjSceneRenPropTextures::Tex** collectedTexAry, cjUINT32& curCollectedTexCount)
{
	cjSceneRenPropTextures::Tex* diffuse = texProp->GetTex(cjSceneRenPropTextures::TEX_DIFFUSE);

    if(diffuse && diffuse->GetTexObj())
    {		
        SetTEX_DIFFUSE_CNT(1);
		collectedTexAry[cjShaderMtlDefault::TEX_DIFFUSE] = diffuse;

		curCollectedTexCount++;
    }

    return cjTrue;
}

cjBool cjShaderMtlDefaultDesc::HandleTexDetailDesc(const cjSceneRenPropTextures* texProp,
    cjSceneRenPropTextures::Tex** collectedTexAry, cjUINT32& curCollectedTexCount)
{
	cjSceneRenPropTextures::Tex* detail = texProp->GetTex(cjSceneRenPropTextures::TEX_DETAIL);

    if(detail && detail->GetTexObj())
    {		
        SetTEX_DETAIL_CNT(1);
		collectedTexAry[cjShaderMtlDefault::TEX_DETAIL] = detail;

		curCollectedTexCount++;
    }

    return cjTrue;
}

cjBool cjShaderMtlDefaultDesc::HandleTexEnvDesc(const cjSceneRenPropTextures* texProp,
    cjSceneRenPropTextures::Tex** collectedTexAry, cjUINT32& curCollectedTexCount)
{
	//cjdefer	
    return cjTrue;
}

cjBool cjShaderMtlDefaultDesc::HandleTexNormalDesc(const cjSceneRenPropTextures* texProp,
    cjSceneRenPropTextures::Tex** collectedTexAry, cjUINT32& curCollectedTexCount)
{
	cjSceneRenPropTextures::Tex* nrm = texProp->GetTex(cjSceneRenPropTextures::TEX_NORMAL);

    if(nrm && nrm->GetTexObj())
    {		
        SetTEX_NORMAL_CNT(1);
		SetNRMMAP_TYPE(NRMMAP_TYPE_RGB);
		
		collectedTexAry[cjShaderMtlDefault::TEX_NORMAL] = nrm;

		curCollectedTexCount++;
    }

    return cjTrue;
}

cjBool cjShaderMtlDefaultDesc::HandleTexSpecularDesc(const cjSceneRenPropTextures* texProp,
    cjSceneRenPropTextures::Tex** collectedTexAry, cjUINT32& curCollectedTexCount,
	cjBool useEnv, cjBool useSpecular)
{
	cjSceneRenPropTextures::Tex* specular = texProp->GetTex(cjSceneRenPropTextures::TEX_SPECULAR);

    if( (specular && specular->GetTexObj()) && (useSpecular || useEnv) )
    {		
        SetTEX_SPECULAR_CNT(1);
		collectedTexAry[cjShaderMtlDefault::TEX_SPECULAR] = specular;

		curCollectedTexCount++;
    }

    return cjTrue;
}

cjBool cjShaderMtlDefaultDesc::HandleTexGlowDesc(const cjSceneRenPropTextures* texProp,
    cjSceneRenPropTextures::Tex** collectedTexAry, cjUINT32& curCollectedTexCount)
{
	cjSceneRenPropTextures::Tex* glow = texProp->GetTex(cjSceneRenPropTextures::TEX_GLOW);

    if(glow && glow->GetTexObj())
    {		
        SetTEX_GLOW_CNT(1);
		collectedTexAry[cjShaderMtlDefault::TEX_GLOW] = glow;

		curCollectedTexCount++;
    }

    return cjTrue;
}

cjBool cjShaderMtlDefaultDesc::HandleTexExt0Desc(const cjSceneRenPropTextures* texProp,
    cjSceneRenPropTextures::Tex** collectedTexAry, cjUINT32& curCollectedTexCount)
{
	cjSceneRenPropTextures::Tex* ext0 = texProp->GetTex(cjSceneRenPropTextures::TEX_EXT0);

    if(ext0 && ext0->GetTexObj())
    {		
        SetTEX_EXT0_CNT(1);
		collectedTexAry[cjShaderMtlDefault::TEX_EXT0] = ext0;

		curCollectedTexCount++;
    }

    return cjTrue;
}

cjBool cjShaderMtlDefaultDesc::HandleTexExt1Desc(const cjSceneRenPropTextures* texProp,
    cjSceneRenPropTextures::Tex** collectedTexAry, cjUINT32& curCollectedTexCount)
{
	cjSceneRenPropTextures::Tex* ext1 = texProp->GetTex(cjSceneRenPropTextures::TEX_EXT1);

    if(ext1 && ext1->GetTexObj())
    {		
        SetTEX_EXT1_CNT(1);
		collectedTexAry[cjShaderMtlDefault::TEX_EXT1] = ext1;

		curCollectedTexCount++;
    }

    return cjTrue;
}

// maxTexCount = _TEXX_UV_SRC_INDEX = _TEX_UVSRC_COUNT
void cjShaderMtlDefaultDesc::HandleTexUVDesc(const cjSceneRenPropTextures* texProp,
    cjSceneRenPropTextures::Tex** collectedTexAry, const cjUINT32 maxTexCount, 
	cjUINT32 vtxUVSetCount, cjUINT32 collectedTexCount)
{
	cjUINT32* uvSetAsPassThruCount = (cjUINT32*)cjFastMemAlloc(sizeof(cjUINT32) * maxTexCount);
	cjMemSet(uvSetAsPassThruCount, 0, maxTexCount*sizeof(cjUINT32));

    cjBool* useUVSet = (cjBool*)cjFastMemAlloc(sizeof(cjBool) * maxTexCount);
    memset(useUVSet, 0, maxTexCount * sizeof(cjBool));
	cjUINT32 useUVSetCount = 0;

    cjUINT32 texTMCount = 0;
    cjUINT32 curUsedTexCount = 0;

    for(cjUINT32 i=0; i<maxTexCount; i++)
    {
        const cjSceneRenPropTextures::Tex* curTex = collectedTexAry[i];

        if(!curTex || vtxUVSetCount == 0)
            continue;

        cjShaderMtlDefault::TEXUVOUT_TYPE curTexOut = cjShaderMtlDefault::TEXUVOUT_PASSTHRU;
        cjUINT32 curTexUVSrcIndex = curTex->GetTexUVSrcIndex();

        if(curTex->GetTexTM())
        {
			curTexOut = cjShaderMtlDefault::TEXUVOUT_WITH_TRANSFORM;
            texTMCount++;
        }
        else
        {
            uvSetAsPassThruCount[curTexUVSrcIndex]++;
        }

        if(!useUVSet[curTexUVSrcIndex])
        { 
            useUVSetCount++;
            useUVSet[curTexUVSrcIndex] = cjTrue;
        }

		cjAssert(curTexUVSrcIndex < maxTexCount);

        SetTextureUVUsage(curUsedTexCount, curTexUVSrcIndex, curTexOut);

        curUsedTexCount++;
    }

	cjAssert(curUsedTexCount == collectedTexCount);

    SetTEX_UVSRC_COUNT(useUVSetCount);
	
	cjFastMemDeAlloc(useUVSet);
	cjFastMemDeAlloc(uvSetAsPassThruCount);
}

//cjINT32 cjShaderMtlDefaultDesc::ResolveTexIndex(cjShaderMtlDefaultDesc::TEX_TYPE texType, 
//	void* pkPixDesc)
//{
//	cjINT32 texIdx = -1;
//
//	// must keep order with cjShaderMtlDefaultDesc::TEX_TYPE
//	switch(texType)
//	{
//	case TEX_MAX: default: cjAssert(!"[TEX_MAP] Invalid parameter."); break;
//	case TEX_ENV:		texIdx += pkPixDesc->GetTEX_ENV_CNT();	// fall-through
//	case TEX_EXT1: 		texIdx += pkPixDesc->GetTEX_EXT1_CNT();	// fall-through
//	case TEX_EXT0: 		texIdx += pkPixDesc->GetTEX_EXT0_CNT();	// fall-through
//	case TEX_GLOW: 		texIdx += pkPixDesc->GetTEX_GLOW_CNT();	// fall-through
//	case TEX_SPECULAR:	texIdx += pkPixDesc->GetTEX_SPECULAR_CNT();	// fall-through
//	case TEX_NORMAL:	texIdx += pkPixDesc->GetTEX_NORMAL_CNT();	// fall-through
//	case TEX_DETAIL:	texIdx += pkPixDesc->GetTEX_DETAIL_CNT();	// fall-through
//	case TEX_DIFFUSE:	texIdx += pkPixDesc->GetTEX_DIFFUSE_CNT();// fall-through
//	}
//
//    return texIdx;
//}

void cjShaderMtlDefaultDesc::SetTextureUVUsage(cjUINT32 texIndex, cjUINT32 texUVIndex,
    cjShaderMtlDefault::TEXUVOUT_TYPE texUVOutType)
{
	switch(texIndex)
	{
	default: cjAssert(!"Invalid parameter!"); break;
	case 0:	SetTEX0_UVSRC_INDEX(texUVIndex); SetTEX0_UVOUT_TYPE(texUVOutType); break;
	case 1:	SetTEX1_UVSRC_INDEX(texUVIndex); SetTEX1_UVOUT_TYPE(texUVOutType); break;
	case 2:	SetTEX2_UVSRC_INDEX(texUVIndex); SetTEX2_UVOUT_TYPE(texUVOutType); break;
	case 3:	SetTEX3_UVSRC_INDEX(texUVIndex); SetTEX3_UVOUT_TYPE(texUVOutType); break;
	case 4:	SetTEX4_UVSRC_INDEX(texUVIndex); SetTEX4_UVOUT_TYPE(texUVOutType); break;
	case 5:	SetTEX5_UVSRC_INDEX(texUVIndex); SetTEX5_UVOUT_TYPE(texUVOutType); break;
	case 6:	SetTEX6_UVSRC_INDEX(texUVIndex); SetTEX6_UVOUT_TYPE(texUVOutType); break;
	case 7:	SetTEX7_UVSRC_INDEX(texUVIndex); SetTEX7_UVOUT_TYPE(texUVOutType); break;
	}
}

void cjShaderMtlDefaultDesc::GetTextureUVUsage(cjUINT32 texIndex, cjUINT32& texUVIndex,
    cjShaderMtlDefault::TEXUVOUT_TYPE& texUVOut) const
{
	switch(texIndex)
	{
	default: cjAssert(!"Invalid parameter!"); break;
	case 0:	texUVIndex = GetTEX0_UVSRC_INDEX(); texUVOut = (cjShaderMtlDefault::TEXUVOUT_TYPE)GetTEX0_UVOUT_TYPE(); break;
	case 1:	texUVIndex = GetTEX1_UVSRC_INDEX(); texUVOut = (cjShaderMtlDefault::TEXUVOUT_TYPE)GetTEX1_UVOUT_TYPE(); break;
	case 2:	texUVIndex = GetTEX2_UVSRC_INDEX(); texUVOut = (cjShaderMtlDefault::TEXUVOUT_TYPE)GetTEX2_UVOUT_TYPE(); break;
	case 3:	texUVIndex = GetTEX3_UVSRC_INDEX(); texUVOut = (cjShaderMtlDefault::TEXUVOUT_TYPE)GetTEX3_UVOUT_TYPE(); break;
	case 4:	texUVIndex = GetTEX4_UVSRC_INDEX(); texUVOut = (cjShaderMtlDefault::TEXUVOUT_TYPE)GetTEX4_UVOUT_TYPE(); break;
	case 5:	texUVIndex = GetTEX5_UVSRC_INDEX(); texUVOut = (cjShaderMtlDefault::TEXUVOUT_TYPE)GetTEX5_UVOUT_TYPE(); break;
	case 6:	texUVIndex = GetTEX6_UVSRC_INDEX(); texUVOut = (cjShaderMtlDefault::TEXUVOUT_TYPE)GetTEX6_UVOUT_TYPE(); break;
	case 7:	texUVIndex = GetTEX7_UVSRC_INDEX(); texUVOut = (cjShaderMtlDefault::TEXUVOUT_TYPE)GetTEX7_UVOUT_TYPE(); break;
	}
}

