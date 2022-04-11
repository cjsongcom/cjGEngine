
#include "cjEnginePCH.h"
#include "cjShaderMaterial.h"

#include "cjShaderMtlDesc.h"
#include "cjShaderProgDesc.h"


//
// cjShaderMtl
//

CJOBJ_IMPL_CLASS(cjShaderMtl)

CJOBJ_IMPL_SERIALIZE_BEGIN(cjShaderMtl)
	
	if(stream.IsObjOnly())
		return;

	if(stream.IsLoading() || stream.IsSaving())
	{
	}

	// serialize for m_shaderCateName is performed by cjSerObjRefProperty
		
CJOBJ_IMPL_SERIALIZE_END(cjShaderMtl)

CJOBJ_IMPL_STATIC_REG_BEGIN(cjShaderMtl)

//	cjNameProperty* shaderCateName = 
		new(GetCls(), _CT("shaderMtlName"), 0) cjNameProperty(CJ_PROP_NATIVE_OFF(m_shaderMtlClsName), 0, 0);

CJOBJ_IMPL_STATIC_REG_END(cjShaderMtl)


cjShaderMtl::cjShaderMtl()
{
//	m_maxPassCount = DEFAULT_MAX_PASS_COUNT;
}

//void cjShaderMtl::OnDestroy()
//{
//    cjSuper::OnDestroy();
//}

cjBool cjShaderMtl::CheckCircularRef(cjTAry<cjMaterial*>& materials)
{
	if(!cjSuper::CheckCircularRef(materials))
		return cjFalse;

    return cjTrue;
}

void cjShaderMtl::PreSetupMaterial(cjEngineTime_ft timeSec)
{
	cjSuper::PreSetupMaterial(timeSec);	
}

void cjShaderMtl::AddShaderToShaderManager(cjShader* shader) const
{
	cjShaderManager* shaderManager = cjShaderManager::Get();

	shaderManager->AddShaderCache(GetShaderMtlClsName(), shader);
}


//
// cjShaderMtlDefault
//

CJOBJ_IMPL_CLASS(cjShaderMtlDefault)

CJOBJ_IMPL_SERIALIZE_BEGIN(cjShaderMtlDefault)
	
	if(stream.IsObjOnly())
		return;

	if(stream.IsLoading() || stream.IsSaving())
	{
	}
		
CJOBJ_IMPL_SERIALIZE_END(cjShaderMtlDefault)

cjShaderMtlDefault::cjShaderMtlDefault()
{
	m_shaderMtlClsName.SetName(cjName_cjShaderMtlDefault);
	m_mtlDesc.SetShaderMtlClsName(m_shaderMtlClsName);
}

void cjShaderMtlDefault::OnDestroy()
{
    cjSuper::OnDestroy();
}

cjShader* cjShaderMtlDefault::ResolveShader(cjShaderMtlDesc& rstMtlDesc, const cjFVertexStream** streamAry,
	cjINT32 streamCount, const cjScenePropRPtrList* overrideSceneProps) const
{
	ResolveShaderMtlDesc(rstMtlDesc, streamAry, streamCount, overrideSceneProps);

	cjShader* shader = GenerateShader(rstMtlDesc);

	return shader;
}

void cjShaderMtlDefault::ResolveShaderMtlDesc(cjShaderMtlDesc& rstMtlDesc, const cjFVertexStream** streamAry, 
	cjINT32 streamCount, const cjScenePropRPtrList* overrideSceneProps) const
{	
	rstMtlDesc = m_mtlDesc;
	
	cjShaderMtlDefaultDesc& mtlDesc = (cjShaderMtlDefaultDesc&)rstMtlDesc;
	
	for(cjINT32 i=0; i<streamCount; i++)
	{
		const cjFVertexStream* stream = streamAry[i];

		if(i==0)
		{
			cjBool hwSkinned;
			mtlDesc.CollectTransform(streamAry, streamCount, hwSkinned);

			cjBool vtxNrmExist, vtxNBTExist;
			mtlDesc.CollectNBT(streamAry, streamCount, hwSkinned, vtxNrmExist, vtxNBTExist);

			cjBool vtxColorExist;
			mtlDesc.CollectVertexColor(streamAry, streamCount, vtxColorExist);
		}
	}

//	mtlDesc.CollectSceneRenProp(streamAry, streamCount, GetScenePropList());

	if(overrideSceneProps)
		mtlDesc.CollectSceneRenProp(streamAry, streamCount, *overrideSceneProps);

	//if(overrideSceneProps)
	//{
	//	//cjdefer
	//}
		
}

cjShader* cjShaderMtlDefault::GenerateShader(const cjShaderMtlDesc& mtlDesc) const
{
	cjShaderManager* shaderManager = cjShaderManager::Get();
	const cjName& mtlClsName = mtlDesc.GetShaderMtlClsName();
	cjAssert(mtlClsName.Equal(cjName_cjShaderMtlDefault));

	cjBool isFailedShader = cjFalse;
	
	cjShader* shader = shaderManager->FindShader(mtlClsName, mtlDesc, &isFailedShader);

	if(shader)
		return shader;

	if(isFailedShader)
		return cjNULL;

	RenderPassDesc* renPassDescAry = new RenderPassDesc[MAX_RENDER_PASS];

	shader = _GenerateShader(renPassDescAry, mtlDesc);

	if(!shader)
		shaderManager->SetFailedShader(mtlClsName, mtlDesc);

	if(renPassDescAry)
		delete [] renPassDescAry;

	return shader;
}

cjShader* cjShaderMtlDefault::_GenerateShader(cjShaderMtlDefault::RenderPassDesc* renPassDescAry, 
	const cjShaderMtlDesc& mtlDesc) const
{
	cjShaderMtlDefaultDesc& mtlDefDesc = (cjShaderMtlDefaultDesc&)mtlDesc;

	cjUINT32 passCount = 0;
	cjBool doCont = cjTrue;
	cjUINT32 totalRenPassCount = 0;

	cjShader* shader = cjNULL;
		
	while(doCont)
	{
		for(cjUINT32 i=0; i<MAX_RENDER_PASS; i++)
			renPassDescAry[i].Initialize();

		if(!SetupShaderProgDesc(mtlDefDesc, renPassDescAry, MAX_RENDER_PASS, totalRenPassCount))
			return cjNULL;

		if(!totalRenPassCount)
			return cjNULL;

		shader = CreateShaderForMtlDefDesc(mtlDefDesc, renPassDescAry, totalRenPassCount);

		if(!shader)
			return cjNULL;

		if(shader)
			break;
	}

//	if(!shader->IsInitialized())
//		shader->Initialize();

	AddShaderToShaderManager(shader);

    return shader;
}

cjBool cjShaderMtlDefault::SetupShaderProgDesc(const cjShaderMtlDesc& desc, cjShaderMtlDefault::RenderPassDesc* pass,
	cjUINT32 maxPassCount, cjUINT32& curPassCount, cjUINT32* errNo) const
{
	if(errNo)
		*errNo = 0;

	cjAssert(maxPassCount != 0);
	curPassCount = 0;

    if(desc.GetShaderMtlClsName() != cjName_cjShaderMtlDefault)
	{
		if(errNo)
			*errNo = 1;

		return cjFalse;
	}

	const cjShaderMtlDefaultDesc& shMtlDefDesc = (cjShaderMtlDefaultDesc&)desc;

	cjDefaultShaderVtxProgDesc* vtxProgDesc = (cjDefaultShaderVtxProgDesc*)&pass[0].m_vtxProgDesc;
	cjDefaultShaderPxlProgDesc* pxlProgDesc = (cjDefaultShaderPxlProgDesc*)&pass[0].m_pxlProgDesc;

	vtxProgDesc->SetShaderMtlClsName(shMtlDefDesc.GetShaderMtlClsName());
	pxlProgDesc->SetShaderMtlClsName(shMtlDefDesc.GetShaderMtlClsName());


	cjUINT32 texMapCount[TEX_MAX] = {0};

	texMapCount[TEX_DIFFUSE]		= shMtlDefDesc.GetTEX_DIFFUSE_CNT();
	texMapCount[TEX_ENV]			= shMtlDefDesc.GetTEX_ENV_CNT();
	texMapCount[TEX_DETAIL]			= shMtlDefDesc.GetTEX_DETAIL_CNT();
	texMapCount[TEX_NORMAL]			= shMtlDefDesc.GetTEX_NORMAL_CNT();
	texMapCount[TEX_SPECULAR]		= shMtlDefDesc.GetTEX_SPECULAR_CNT();
	texMapCount[TEX_GLOW]			= shMtlDefDesc.GetTEX_GLOW_CNT();
	texMapCount[TEX_EXT0]			= shMtlDefDesc.GetTEX_EXT0_CNT();
	texMapCount[TEX_EXT1]			= shMtlDefDesc.GetTEX_EXT1_CNT();	

    cjUINT32 texCount				= shMtlDefDesc.GetTextureCount();
    cjAssert(texCount <= TEX_MAX);
	
	cjUINT32 envMapType				= shMtlDefDesc.GetENVMAP_TYPE();

	cjUINT32 normalFrom				= shMtlDefDesc.GetNORMAL_FROM();
	cjUINT32 nrmMapType				= shMtlDefDesc.GetNRMMAP_TYPE();
	
	if(normalFrom == cjShaderMtlDefaultDesc::NRM_FROM_NBT && !shMtlDefDesc.GetTEX_NORMAL_CNT())
        normalFrom = cjShaderMtlDefaultDesc::NRM_FROM_VTX;

	cjUINT32 vtxColorFrom			= shMtlDefDesc.GetVTXCOLOR_FROM();

	if(vtxColorFrom == cjShaderMtlDefaultDesc::VTXCOLOR_FROM_VTX)
		pxlProgDesc->SetIN_VTX_COLOR(1);

	// Texture only(ignore light) or 
	cjUINT32 texLightOper			= shMtlDefDesc.GetTEX_LIGHT_OPER(); 

    cjUINT32 ambDiffEmiFrom			= shMtlDefDesc.GetADE_FROM();
	cjUINT32 lightingMode			= shMtlDefDesc.GetLIGHTING();
    cjUINT32 usePerVtxLighting		= shMtlDefDesc.GetPER_VTX_LIGHTING();
    cjUINT32 fogType				= shMtlDefDesc.GetFOG_TYPE();	
	cjUINT32 useAlphaTest			= shMtlDefDesc.GetALPHATEST();

    cjUINT32 dirLightCount			= 0; // shMtlDefDesc.GetDIRLIGHTCOUNT();
    cjUINT32 spotLightCount			= 0; // shMtlDefDesc.GetSPOTLIGHTCOUNT();
    cjUINT32 pointLightCount		= 0; // shMtlDefDesc.GetPOINTLIGHTCOUNT();
    cjUINT32 shadowMapForLight		= 0; // shMtlDefDesc.GetSHADOWMAPFORLIGHT();
    cjUINT32 useSpecular			= shMtlDefDesc.GetSPECULAR();
    cjUINT32 totalLightCount		= dirLightCount + spotLightCount + pointLightCount;
	    
    if(texLightOper == cjShaderMtlDefaultDesc::TEX_LIGHT_OPER_TEX_ONLY)
    {
		// No Lighting, Use texture color only
        totalLightCount = 0;
        dirLightCount = 0;
        spotLightCount = 0;
        pointLightCount = 0;
        useSpecular = 0;
        usePerVtxLighting = 1;
    }
	
	//
	// Setup ShaderProgram Description
	//
			
		// texture
	pxlProgDesc->SetTEX_DIFFUSE_CNT(texMapCount[TEX_DIFFUSE]);
	pxlProgDesc->SetTEX_ENV_CNT(texMapCount[TEX_ENV]);
	pxlProgDesc->SetTEX_DETAIL_CNT(texMapCount[TEX_DETAIL]);
	pxlProgDesc->SetTEX_NORMAL_CNT(texMapCount[TEX_NORMAL]);
	pxlProgDesc->SetTEX_SPECULAR_CNT(texMapCount[TEX_SPECULAR]);
	pxlProgDesc->SetTEX_GLOW_CNT(texMapCount[TEX_GLOW]);
	pxlProgDesc->SetTEX_EXT0_CNT(texMapCount[TEX_EXT0]);
	pxlProgDesc->SetTEX_EXT1_CNT(texMapCount[TEX_EXT1]);

	pxlProgDesc->SetNRMMAP_TYPE(nrmMapType);
    pxlProgDesc->SetTEX_LIGHT_OPER(texLightOper);

	// projected light map count, type, clipped
	// projected shadow map count, type, clipped

	vtxProgDesc->SetTM(shMtlDefDesc.GetTM()); // Transform Type,  non-skin/skinning/instancing
	vtxProgDesc->SetNRMMAP_TYPE(nrmMapType);
	vtxProgDesc->SetOUT_NORMAL((normalFrom == cjShaderMtlDefaultDesc::NRM_FROM_VTX) ? 1 : 0);
    vtxProgDesc->SetVTXCOLOR_FROM(vtxColorFrom);        
    vtxProgDesc->SetTEX_LIGHT_OPER(texLightOper);

	pxlProgDesc->SetSPECULAR_USE(useSpecular);

	vtxProgDesc->SetADE_FROM(ambDiffEmiFrom);
	pxlProgDesc->SetADE_FROM(ambDiffEmiFrom);    

	vtxProgDesc->SetLIGHTING(lightingMode);
    pxlProgDesc->SetLIGHTING(lightingMode);

    pxlProgDesc->SetPER_VTX_LIGHTING(usePerVtxLighting);

	vtxProgDesc->SetFOG_TYPE(fogType);
    pxlProgDesc->SetFOG_USE(fogType != cjShaderMtlDefaultDesc::FOG_TYPE_NONE);    

    pxlProgDesc->SetALPHATEST_USE(useAlphaTest);
	

	//
	// Handle Per-Vertex/Pixel Lighting
	//

	if(usePerVtxLighting)
	{
		// per-vertex lighting
		vtxProgDesc->SetPOINT_LCOUNT(pointLightCount);	// set to zero
		vtxProgDesc->SetSPOT_LCOUNT(spotLightCount);	// set to zero
		vtxProgDesc->SetDIR_LCOUNT(dirLightCount);		// set to zero
		vtxProgDesc->SetSPECULAR_USE(useSpecular);		// set to zero
		vtxProgDesc->SetVTX_LIGHTING_ONLY(1);

		pxlProgDesc->SetAMBIENT_USE(0);
		pxlProgDesc->SetEMISSIVE_USE(0);		
	}
	else
	{	
		// per-pixel lighting
		pxlProgDesc->SetPOINT_LCOUNT(pointLightCount);
		pxlProgDesc->SetSPOT_LCOUNT(spotLightCount);
		pxlProgDesc->SetDIR_LCOUNT(dirLightCount);

		pxlProgDesc->SetSHADOWMAP_FOR_LIGHT(shadowMapForLight);
		pxlProgDesc->SetAMBIENT_USE(1);
		pxlProgDesc->SetEMISSIVE_USE(1);

		if(useSpecular)
		{
			vtxProgDesc->SetOUT_WORLD_VIEW(useSpecular);
			pxlProgDesc->SetIN_WORLD_VIEW(1);
		}

		if(totalLightCount != 0)
		{
			vtxProgDesc->SetOUT_WORLD_POS(1);
			vtxProgDesc->SetOUT_WORLD_NBT(1);
			vtxProgDesc->SetVTX_LIGHTING_ONLY(0);

			pxlProgDesc->SetIN_WORLD_NORMAL(1);
			pxlProgDesc->SetIN_WORLD_POS(1);			
		}
		else
		{
			vtxProgDesc->SetVTX_LIGHTING_ONLY(1);
			pxlProgDesc->SetPER_VTX_LIGHTING(1);
		}

	}
		
		// Handle LightMap
		// Handle ShadowMap

		// Handle EnvMap
    if(texMapCount[TEX_NORMAL]) 
    {
		// if normal map is exist, per-pixel env map processing is required
        vtxProgDesc->SetOUT_WORLD_NBT(1);
		vtxProgDesc->SetENVMAP_TYPE(envMapType);

        pxlProgDesc->SetIN_WORLD_NBT(1);
        pxlProgDesc->SetENVMAP_TYPE(envMapType);

        if(envMapType == cjShaderMtlDefaultDesc::ENVMAP_TYPE_SPECULAR_CUBE ||
			envMapType == cjShaderMtlDefaultDesc::ENVMAP_TYPE_SEPHERE)
        {
            vtxProgDesc->SetOUT_WORLD_VIEW(1);
            pxlProgDesc->SetIN_WORLD_VIEW(1);
        }
    }
    else
    {
		vtxProgDesc->SetENVMAP_TYPE(envMapType);
        pxlProgDesc->SetENVMAP_TYPE(envMapType);
    }
	  

	//
	// texture uvSet 
	//
		
	cjUINT32 uvSetIndexs[TEX_MAX];
	cjMemSet(uvSetIndexs, 0xff, sizeof(uvSetIndexs));

	TEXUVOUT_TYPE uvOutTypes[TEX_MAX];
	cjMemSet(uvOutTypes, (cjBYTE)TEXUVOUT_NONE, sizeof(uvOutTypes));

    for(cjUINT32 i=0; i<texCount; i++)
        shMtlDefDesc.GetTextureUVUsage(i, uvSetIndexs[i], uvOutTypes[i]);

    SetTextureUVSetInfoToShaderProgDesc(vtxProgDesc, pxlProgDesc, texCount, uvSetIndexs, uvOutTypes);
		
    curPassCount++;

    return cjTrue;
}

cjBool cjShaderMtlDefault::SetTextureUVSetInfoToShaderProgDesc(cjDefaultShaderVtxProgDesc* vtxProgDesc, 
	cjDefaultShaderPxlProgDesc* pxlProgDesc, const cjUINT32 texCount, const cjUINT32* uvSetIndexs, 
	const cjShaderMtlDefault::TEXUVOUT_TYPE* uvOutTypes) const
{
	cjAssert(texCount <= TEX_MAX);
		
	cjUINT32 assignedUVSetIndex[TEX_MAX]; // initialized with 0xff
	cjMemSet(assignedUVSetIndex, 0xff, sizeof(assignedUVSetIndex));

	cjUINT32 useUVSetIndex[TEX_MAX] = {0};
	cjINT32 lastUVSetIndex = -1;

	for(cjUINT32 i=0; i<texCount; i++)
	{
		cjUINT32 curUVSetIndex = uvSetIndexs[i];

		if(uvOutTypes[i] == TEXUVOUT_PASSTHRU)
		{
			assignedUVSetIndex[i] = curUVSetIndex;

			if ((cjINT32)curUVSetIndex > lastUVSetIndex)
				lastUVSetIndex = (cjINT32)curUVSetIndex;

			useUVSetIndex[curUVSetIndex] = 1;
		}
	}

	cjINT32 additionalUVSetIndexMax = -1;
	for(cjUINT32 i=0; lastUVSetIndex != -1 && i<=(cjUINT32)lastUVSetIndex; i++)
	{
		if(!useUVSetIndex[i])
			continue;

		additionalUVSetIndexMax++;

		for(cjUINT32 k=0; k<texCount; k++)
		{
			if(assignedUVSetIndex[k] == i)
				assignedUVSetIndex[k] = (cjUINT32)additionalUVSetIndexMax;
		}
	}

	for(cjUINT32 i=0; i<texCount; i++)
	{
		cjUINT32 curUVSetIndex = uvSetIndexs[i];
		TEXUVOUT_TYPE curUVOutType = uvOutTypes[i];

		if(curUVOutType == TEXUVOUT_NONE)
		{
			;
		}
		else if(curUVOutType == TEXUVOUT_PASSTHRU)
		{
			vtxProgDesc->SetTexStageUVSetIndexAndOutType(assignedUVSetIndex[i], curUVSetIndex, curUVOutType);
			pxlProgDesc->SetSamplerTexcoord(i, assignedUVSetIndex[i]+1); // 0:none, 1:texcoord0			
		}
		else
		{
			additionalUVSetIndexMax++;

			vtxProgDesc->SetTexStageUVSetIndexAndOutType(additionalUVSetIndexMax, curUVSetIndex, curUVOutType);
			pxlProgDesc->SetSamplerTexcoord(i, additionalUVSetIndexMax+1);  // 0:none, 1:texcoord0
		}
	}

	if(additionalUVSetIndexMax != -1)
		vtxProgDesc->SetOUT_UV_COUNT(1 + (cjUINT32)additionalUVSetIndexMax);
	else
		vtxProgDesc->SetOUT_UV_COUNT(0);

	return true;
}

cjShader* cjShaderMtlDefault::CreateShaderForMtlDefDesc(const cjShaderMtlDesc& mtlDesc,
	cjShaderMtlDefault::RenderPassDesc* renPassDescAry, cjINT32 totalRenPassCount) const
{
	cjShaderManager* shaderMan = cjShaderManager::Get();
	cjShader* shader = shaderMan->CreateShaderInstance(&mtlDesc);

	cjINT32 appendRenPass = 0;

	// init shader render pass
	for(cjINT32 i=0; i<totalRenPassCount; i++)
	{
		RenderPassDesc& cur = renPassDescAry[i];

		shader->AppendRenderPass(appendRenPass);

		shader->SetShaderProgramDesc(appendRenPass, &cur.m_vtxProgDesc, &cur.m_pxlProgDesc);

		if(cur.m_overrideAlpha)
		{
			shader->SetOverrideAlpha(appendRenPass, cur.m_doAlphaBlend, 
				cur.m_usePrevSrcBlendMode, cur.m_srcBlendMode,
				cur.m_usePrevDstBlendMode, cur.m_dstBlendMode);
		}		
	}

	if(!shader->ResolveRenderPassShaderProgram())
	{
		//cjdefer
	}

	return shader;
}

const cjSceneRenPropTextures* cjShaderMtlDefault::GetTextures() const
{
	return (const cjSceneRenPropTextures*)cjScenePropUtil_Find<cjSceneRenPropTextures>(GetScenePropList());
}

cjSceneRenPropTextures::Tex* cjShaderMtlDefault::SetTexture(cjSceneRenPropTextures::ETexType texType, 
	cjTexture* texObj)
{
	cjSceneRenPropTextures* propTex
		= cjScenePropUtil_Find<cjSceneRenPropTextures>(GetScenePropList(), cjTrue);

	cjSceneRenPropTextures::Tex* tex = new cjSceneRenPropTextures::Tex();
		tex->SetTexObj(texObj);

	propTex->SetTex(texType, tex);

	return tex;
}


//
// cjShaderMtlDefault::RenderPassDesc
//

cjShaderMtlDefault::RenderPassDesc::RenderPassDesc()
{
	//cjtemp
}

void cjShaderMtlDefault::RenderPassDesc::Initialize()
{
}


