
#ifndef _CJSHADERMTLDESC_H_
#define _CJSHADERMTLDESC_H_


#include "cjShaderDef.h"


//
// cjShaderMtlDefaultDesc
//

class cjShaderMtlDefaultDesc : public cjShaderMtlDesc
{
	// do not declare any class member variable more

public:
	cjShaderMtlDefaultDesc() 
	{ 
		SetShaderMtlClsName(cjName(cjName_cjShaderMtlDefault));
	}

	//
	// Collect from scene mesh
	//
		// from mesh vertex stream
	cjBool CollectTransform(const cjFVertexStream** streams, cjINT32 streamCount, cjBool& rstHWSkinned);
	cjBool CollectNBT(const cjFVertexStream** streams, cjINT32 streamCount, cjBool hwSkinned, cjBool& rstVtxNormalExist, cjBool& rstVtxNBTExist);
	cjBool CollectVertexColor(const cjFVertexStream** streams, cjINT32 streamCount, cjBool& rstUseVtxColorExist);

		// from mesh SceneRenPropList
		// streams, streamCount can be null
	cjBool CollectSceneRenProp(const cjFVertexStream** streams, cjINT32 streamCount, const cjScenePropRPtrList& scenePropList);
	
	cjUINT32 GetTextureCount() const;

	void SetTextureUVUsage(cjUINT32 texIndex, cjUINT32 texUVIndex, cjShaderMtlDefault::TEXUVOUT_TYPE texUVOutType);
	void GetTextureUVUsage(cjUINT32 texIndex, cjUINT32& texUVIndex, cjShaderMtlDefault::TEXUVOUT_TYPE& texUVOutType) const;

protected:
		// texture support
	void HandleSceneRenPropTex(const cjFVertexStream** streams, cjINT32 streamCount, cjSceneRenPropTextures* texProp);

	cjBool HandleTexDiffuseDesc(const cjSceneRenPropTextures* texProp, cjSceneRenPropTextures::Tex** collectedTexAry, cjUINT32& curCollectedTexCount);
	cjBool HandleTexDetailDesc(const cjSceneRenPropTextures* texProp, cjSceneRenPropTextures::Tex** collectedTexAry, cjUINT32& curCollectedTexCount);
	cjBool HandleTexEnvDesc(const cjSceneRenPropTextures* texProp, cjSceneRenPropTextures::Tex** collectedTexAry, cjUINT32& curCollectedTexCount);
	cjBool HandleTexNormalDesc(const cjSceneRenPropTextures* texProp, cjSceneRenPropTextures::Tex** collectedTexAry, cjUINT32& curCollectedTexCount);
	cjBool HandleTexSpecularDesc(const cjSceneRenPropTextures* texProp, cjSceneRenPropTextures::Tex** collectedTexAry, cjUINT32& curCollectedTexCount, cjBool useEnv, cjBool useSpecular);
	cjBool HandleTexGlowDesc(const cjSceneRenPropTextures* texProp, cjSceneRenPropTextures::Tex** collectedTexAry, cjUINT32& curCollectedTexCount);
	cjBool HandleTexExt0Desc(const cjSceneRenPropTextures* texProp, cjSceneRenPropTextures::Tex** collectedTexAry, cjUINT32& curCollectedTexCount);
	cjBool HandleTexExt1Desc(const cjSceneRenPropTextures* texProp, cjSceneRenPropTextures::Tex** collectedTexAry, cjUINT32& curCollectedTexCount);
	
	void HandleTexUVDesc(const cjSceneRenPropTextures* texProp, cjSceneRenPropTextures::Tex** collectedTexAry, cjUINT32 maxTexCount, cjUINT32 vtxUVSetCount, cjUINT32 collectedTexCount);

//	cjINT32 ResolveTexIndex(cjShaderMtlDefaultDesc::TEX_TYPE texType, void* pkPixDesc);
		
public:
	static const cjUINT32 _AST_DNBPOS = 1;
	
	CJSHDESC_DEF_BEGIN(TM)
		TM_DEFAULT, 
		TM_SKINNED, 
		TM_INSTANCED,
	CJSHDESC_DEF_END(TM, ST) // need to update maxsize of CJSHDESC_DEF(name, maxsize, prev) 
	
	CJSHDESC_DEF_BEGIN(NORMAL_FROM)
		NRM_FROM_NONE,
		NRM_FROM_VTX,
		NRM_FROM_NBT,
	CJSHDESC_DEF_END(NORMAL_FROM, TM)

	CJSHDESC_DEF_BEGIN(VTXCOLOR_FROM)
		VTXCOLOR_FROM_NONE,
		VTXCOLOR_FROM_VTX,
	CJSHDESC_DEF_END(VTXCOLOR_FROM, NORMAL_FROM)

	CJSHDESC_DEF_BEGIN(ALPHATEST)
		ALPHATEST_NONE,
		ALPHATEST_USE,
	CJSHDESC_DEF_END(ALPHATEST, VTXCOLOR_FROM)

	CJSHDESC_DEF_BEGIN(SPECULAR)
		SPECULAR_NONE,
		SPECULAR_USE,
	CJSHDESC_DEF_END(SPECULAR, ALPHATEST)
	
	CJSHDESC_DEF_BEGIN(LIGHT_TYPE)
		LIGHT_TYPE_DIRECTIONAL,
		LIGHT_TYPE_POINT,
		LIGHT_TYPE_SPOT,
	CJSHDESC_DEF_END(LIGHT_TYPE, SPECULAR)
	
	CJSHDESC_DEF_BEGIN(LIGHTING)
		LIGHTING_EMISSIVE_ONLY,			// only emissive portion of lighting is used
		LIGHTING_EAD,					// use emissive, ambient, diffuse portion
	CJSHDESC_DEF_END(LIGHTING, LIGHT_TYPE)

	CJSHDESC_DEF_BEGIN(PER_VTX_LIGHTING)
		PER_VTX_LIGHTING_NONE,
		PER_VTX_LIGHTING_USE,
	CJSHDESC_DEF_END(PER_VTX_LIGHTING, LIGHTING)

	CJSHDESC_DEF_BEGIN(ADE_FROM)
		ADE_FROM_MTL,					// Ambient,Diffuse,Emissive = cjSceneRenPropMaterial
		ADE_FROM_AD_MTL_E_VC,			// Emissive = vertex color, Diffuse/Ambient = cjSceneRenPropMaterial
		ADE_FROM_AD_VC_E_MTL,			// Ambient/Diffuse=vertex color, Emissive = cjSceneRenPropMaterial
	CJSHDESC_DEF_END(ADE_FROM, PER_VTX_LIGHTING)

	CJSHDESC_DEF_BEGIN(NRMMAP_TYPE)		
		NRMMAP_TYPE_RGB,
		NRMMAP_TYPE_DXT5,
		NRMMAP_TYPE_RESERVED0,
		NRMMAP_TYPE_RESERVED1,
	CJSHDESC_DEF_END(NRMMAP_TYPE, ADE_FROM)

	CJSHDESC_DEF_BEGIN(ENVMAP_TYPE)
		ENVMAP_TYPE_WORLD_PARALLEL,
		ENVMAP_TYPE_WORLD_PERSEPCTIVE,
		ENVMAP_TYPE_SEPHERE,
		ENVMAP_TYPE_SPECULAR_CUBE,
		ENVMAP_TYPE_DIFFUSE_CUBE,
	CJSHDESC_DEF_END(ENVMAP_TYPE, NRMMAP_TYPE)

	CJSHDESC_DEF_BEGIN(FOG_TYPE)
		FOG_TYPE_NONE,
		FOG_TYPE_LINEAR,
		FOG_TYPE_SQUARE,
	CJSHDESC_DEF_END(FOG_TYPE, ENVMAP_TYPE)

	CJSHDESC_DEF_BEGIN(TEX_LIGHT_OPER)
		TEX_LIGHT_OPER_TEX_ONLY,			// use texture only, lighting is ignored
		TEX_LIGHT_OPER_MUL,					// texture x light
	CJSHDESC_DEF_END(TEX_LIGHT_OPER, FOG_TYPE)

	CJSHDESC_DEF_BEGIN(VTX_UV_COUNT)
		_VTX_UV_COUNT=3,					// uv0~uv2
	CJSHDESC_DEF_END(VTX_UV_COUNT, TEX_LIGHT_OPER)

	CJSHDESC_DEF_BEGIN(TEXSAMP_OPER)
		TEXSAMP_RGBA,
		TEXSAMP_RGB,
		TEXSAMP_A,
		TEXSAMP_CUBE_RGB,
	CJSHDESC_DEF_END(TEXSAMP_OPER, VTX_UV_COUNT)
	
	CJSHDESC_DEF_BEGIN(TEXUV_SRC)
		TEXUV_SRC_IN,					// uv from vtx source
		TEXUV_SRC_WPOS,					// uv from world pos
		TEXUV_SRC_WNRM,					// uv from world normal
		TEXUV_SRC_WREFLECT,				// uv from view reflection vector in world space
	CJSHDESC_DEF_END(TEXUV_SRC, TEXSAMP_OPER)
	
	CJSHDESC_DEF_BEGIN(TEXMAP_OPER)
		TEXMAP_OPER_MUL,
		TEXMAP_OPER_ADD,
		TEXMAP_OPER_LERP,
		TEXMAP_OPER_PASSTHRU,
	CJSHDESC_DEF_END(TEXMAP_OPER, TEXUV_SRC)

	CJSHDESC_DEF_BEGIN(TEXTM_OPER)
		TEXTM_PASS,						// pass thru uv 
		TEXTM_MATRIX,					// apply tex matrix with uv
		TEXTM_PARALLEL_PROJ,			// apply parallel projection
		TEXTM_PERSPECTIVE_PROJECT,		// apply perspective projection
	CJSHDESC_DEF_END(TEXTM_OPER, TEXMAP_OPER)

	CJSHDESC_DEF_BEGIN(TEX_DIFFUSE_CNT)
		_TEX_DIFFUSE_CNT=1,
	CJSHDESC_DEF_END(TEX_DIFFUSE_CNT, TEXTM_OPER)

	CJSHDESC_DEF_BEGIN(TEX_ENV_CNT)
		_TEX_ENV_CNT=1,
	CJSHDESC_DEF_END(TEX_ENV_CNT, TEX_DIFFUSE_CNT)

	CJSHDESC_DEF_BEGIN(TEX_DETAIL_CNT)
		_TEX_DETAIL_CNT=1,
	CJSHDESC_DEF_END(TEX_DETAIL_CNT, TEX_ENV_CNT)

	CJSHDESC_DEF_BEGIN(TEX_NORMAL_CNT)
		_TEX_NORMAL_CNT=1,
	CJSHDESC_DEF_END(TEX_NORMAL_CNT, TEX_DETAIL_CNT)

	CJSHDESC_DEF_BEGIN(TEX_SPECULAR_CNT)
		_TEX_SPECULAR_CNT=1,
	CJSHDESC_DEF_END(TEX_SPECULAR_CNT, TEX_NORMAL_CNT)

	CJSHDESC_DEF_BEGIN(TEX_GLOW_CNT)
		_TEX_GLOW_CNT=1,
	CJSHDESC_DEF_END(TEX_GLOW_CNT, TEX_SPECULAR_CNT)

	CJSHDESC_DEF_BEGIN(TEX_EXT0_CNT)
		_TEX_ETX0_CNT=1,
	CJSHDESC_DEF_END(TEX_EXT0_CNT, TEX_GLOW_CNT)

	CJSHDESC_DEF_BEGIN(TEX_EXT1_CNT)
		_TEX_EXT1_CNT=1,
	CJSHDESC_DEF_END(TEX_EXT1_CNT, TEX_EXT0_CNT)

	// 0~ (TEX_MAX-1)
	CJSHDESC_DEF_BEGIN(TEX_UVSRC_COUNT)
		_TEX_UVSRC_COUNT=7,
	CJSHDESC_DEF_END(TEX_UVSRC_COUNT, TEX_EXT1_CNT)
	

	//
	// Shader TextureStage 0~7
	//

	// cjoptim , shrink to maximum 3 texture uv coordinate set

	CJSHDESC_DEF_BEGIN(TEX0_UVSRC_INDEX)
		_TEX0_UVSRC_INDEX=7,
	CJSHDESC_DEF_END(TEX0_UVSRC_INDEX, TEX_UVSRC_COUNT)

	CJSHDESC_DEF_BEGIN(TEX1_UVSRC_INDEX)
		_TEX1_UVSRC_INDEX=7,
	CJSHDESC_DEF_END(TEX1_UVSRC_INDEX, TEX0_UVSRC_INDEX)

	CJSHDESC_DEF_BEGIN(TEX2_UVSRC_INDEX)
		_TEX2_UVSRC_INDEX=7,
	CJSHDESC_DEF_END(TEX2_UVSRC_INDEX, TEX1_UVSRC_INDEX)

	CJSHDESC_DEF_BEGIN(TEX3_UVSRC_INDEX)
		_TEX3_UVSRC_INDEX=7,
	CJSHDESC_DEF_END(TEX3_UVSRC_INDEX, TEX2_UVSRC_INDEX)

	CJSHDESC_DEF_BEGIN(TEX4_UVSRC_INDEX)
		_TEX4_UVSRC_INDEX=7,
	CJSHDESC_DEF_END(TEX4_UVSRC_INDEX, TEX3_UVSRC_INDEX)

	CJSHDESC_DEF_BEGIN(TEX5_UVSRC_INDEX)
		_TEX5_UVSRC_INDEX=7,
	CJSHDESC_DEF_END(TEX5_UVSRC_INDEX, TEX4_UVSRC_INDEX)

	CJSHDESC_DEF_BEGIN(TEX6_UVSRC_INDEX)
		_TEX6_UVSRC_INDEX=7,
	CJSHDESC_DEF_END(TEX6_UVSRC_INDEX, TEX5_UVSRC_INDEX)

	CJSHDESC_DEF_BEGIN(TEX7_UVSRC_INDEX)
		_TEX7_UVSRC_INDEX=7,
	CJSHDESC_DEF_END(TEX7_UVSRC_INDEX, TEX6_UVSRC_INDEX)


	//
	// TEXUV_OUT_NONE, TEXUV_OUT_PASSTHRU, TRANSFORM, PERSPECTIVE_PROJ
	//

	CJSHDESC_DEF_BEGIN(TEX0_UVOUT_TYPE)
		_TEX0_UVOUT_TYPE=3,
	CJSHDESC_DEF_END(TEX0_UVOUT_TYPE, TEX7_UVSRC_INDEX)

	CJSHDESC_DEF_BEGIN(TEX1_UVOUT_TYPE)
		_TEX1_UVOUT_TYPE=3,
	CJSHDESC_DEF_END(TEX1_UVOUT_TYPE, TEX0_UVOUT_TYPE)

	CJSHDESC_DEF_BEGIN(TEX2_UVOUT_TYPE)
		_TEX2_UVOUT_TYPE=3,
	CJSHDESC_DEF_END(TEX2_UVOUT_TYPE, TEX1_UVOUT_TYPE)

	CJSHDESC_DEF_BEGIN(TEX3_UVOUT_TYPE)
		_TEX3_UVOUT_TYPE=3,
	CJSHDESC_DEF_END(TEX3_UVOUT_TYPE, TEX2_UVOUT_TYPE)

	CJSHDESC_DEF_BEGIN(TEX4_UVOUT_TYPE)
		_TEX4_UVOUT_TYPE=3,
	CJSHDESC_DEF_END(TEX4_UVOUT_TYPE, TEX3_UVOUT_TYPE)

	CJSHDESC_DEF_BEGIN(TEX5_UVOUT_TYPE)
		_TEX5_UVOUT_TYPE=3,
	CJSHDESC_DEF_END(TEX5_UVOUT_TYPE, TEX4_UVOUT_TYPE)

	CJSHDESC_DEF_BEGIN(TEX6_UVOUT_TYPE)
		_TEX6_UVOUT_TYPE=3,
	CJSHDESC_DEF_END(TEX6_UVOUT_TYPE, TEX5_UVOUT_TYPE)

	CJSHDESC_DEF_BEGIN(TEX7_UVOUT_TYPE)
		_TEX7_UVOUT_TYPE=3,
	CJSHDESC_DEF_END(TEX7_UVOUT_TYPE, TEX6_UVOUT_TYPE)

};


#endif


