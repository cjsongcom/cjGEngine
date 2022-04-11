
#ifndef _CJGLRENINTERFACE_H_
#define _CJGLRENINTERFACE_H_

#include "cjGLRenRes.h"

class cjGLRenDev;
class cjGLVertexStream;
class cjGLIndexBuffer;
class cjMaterial;


struct cjGLMtlModifyAccum : public cjMem
{
    // cjCheckMaterial, cjTexMtlModifier
	cjBool                  m_commitTexTransforms;
    
    // cjCheckMaterial, cjFinalBlendMtlModifier
    // SetSimpleMaterial, for texture material(mask, alpha texture)
	cjBool                  m_commitFramebufferBlending;
	
    // cjCheckMaterial, cjColorMtlModifier
    cjBool                  m_commitColor;

    // Ref By CommitTexStageModify, applied to cjGLTextureStage
	cjMatrix				m_texMatrix;
	cjBYTE                  m_texCoordSource;     // cjTexCoordSrc
	cjBYTE                  m_texCoordCount;      // cjTexCoordCount
	cjBool                  m_texCoordProjected;
    
    // Ref By CommitFinalBlendModify, applied to curState->cjGLRenderPass[0]
    cjBYTE                  m_frameBufferBlend;
    cjBool                  m_zWrite;
    cjBool                  m_zTest;
    cjBool                  m_alphaTest;
    cjBYTE                  m_alphaRef;
    cjBool                  m_twoSided;


	cjBool                  m_alphaBlend;
	cjColor                 m_texFactorColor;

	cjMaterial*             m_fallback;

	cjGLMtlModifyAccum();
};


class CJENGINE_API cjGLTextureStage : public cjMem
{
public:
	cjGLTexture*                            m_texture;
	
	// applied by CommitTexStageModify start
	cjBYTE                                  m_texCoordCount; // cjTexCoordCount
	cjBYTE                                  m_texCoordSource; // cjTexCoordSrc
	cjBYTE									m_texTypeForShader;
	cjBYTE									m_pad0;

	cjMatrix                                m_texTransformMatrix;
	cjBool                                  m_texTransformEnable; // modifyAccum->m_texGenProjected || modifyAccum->m_texMatrix	
	cjBool                                  m_texCoordGenProjected;

	// cjTexCoordSrcWorld ~ SphereMap
	cjBool                                  m_useTexMatrixAsTexCoordGen;
	cjBYTE                                  m_texCoordGenMode; // cjTexCoordSrc
	
	// applied by CommitTexStageModify end

	cjTFLAG                                 m_texAddrU, m_texAddrV, m_texAddrW;
	cjFLOAT                                 m_texMipLODBias;

	cjGLTextureStage();
};


class CJENGINE_API cjGLRenderPass : public cjMem
{
public:
	enum { MAX_STAGE = 8 };
	cjGLRenderPass();

	cjINT32                                 m_numRef;
    cjFShaderProgram*                       m_shaderProg;

        // Setup By CommitFinalBlendModify
	GLenum                                  m_srcBlend;
	GLenum                                  m_dstBlend;
	cjBool                                  m_alphaBlend;
	
	cjBool                                  m_zWrite;
	cjBool                                  m_zTest;
    
	cjBool                                  m_alphaTest;
	cjBYTE                                  m_alphaRef;

	// default : front face direction is ccw
	// default : cull back face
	cjCullMode                              m_cullMode;  
	cjFillMode                              m_fillMode;

	cjINT32                                 m_numTextureStage;
	cjGLTextureStage                        m_textureStage[MAX_STAGE];
    
    cjGLVertexStreamAttribLayout*           m_vertexStreamAttribLayout;
};


class cjGLRenderPassPool : public cjMem
{
public:
	~cjGLRenderPassPool()
	{
		for(cjAryOff stateIndex=0; 
			stateIndex < m_pool.Num(); stateIndex++)
		{
			delete m_pool(stateIndex);
		}
	}

	cjGLRenderPass* Alloc(cjGLRenderPass* cloneSource=NULL)
	{
		cjGLRenderPass* rst;

		if(!m_pool.IsEmpty())
			rst = m_pool.Pop();
		else
			rst = new cjGLRenderPass();

		if(cloneSource)
			cjMemCpy(rst, cloneSource, sizeof(cjGLRenderPass));

		rst->m_numRef = 1;

		return rst;
	}

	void Free(cjGLRenderPass* pass)
	{
		if(pass)
			m_pool.Push(pass);
	}

	cjTAry<cjGLRenderPass*>					m_pool;
};


class CJENGINE_API cjGLRenInterface : public cjRenInterface
{
public:
	enum 
	{
		STATE_STACK_MAX = 128,
	};

	class CJENGINE_API cjGLSavedState : public cjMem
	{
	public:
		// use POD or pointer type Only in here, initialized with cjMemSet

		cjFRenTargetGrp*				m_renderTargetGrp;

		// viewport
		cjINT32							m_vpX;
		cjINT32							m_vpY;
		cjINT32							m_vpW;
		cjINT32							m_vpH;
        
		// cam
		//cjVec3							m_camR;
		//cjVec3							m_camU;
		//cjVec3							m_camD;
  //      
		//cjVec3							m_camEye;
  //      
		//cjFrustum						m_camFrustum;
		//cjRect<cjFLOAT>                 m_camPort;
        
		cjBool                          m_useDetailTexturing;
		cjBool                          m_useDynamicLighting;
		cjBool                          m_useStaticLighting;
		cjBool                          m_lightingModulate2X;
		cjBool                          m_lightingOnly;
		cjGLTexture*                    m_lightmap;
		//cjFSphere                     m_lightSphere;
		cjColor                         m_ambientLightColor;
        
		// zbuffer/stencil
		cjFLOAT							m_zbias;
        
		cjPxlCmpFunc					m_stencilTest;
		cjStencilOper					m_stencilFailOper, m_stencilZFailOper, m_stencilPassOper;
		cjTFLAG							m_stencilRef, m_stencilMask, m_stencilWriteMask;
        
        cjBool                          m_alphaBlending;
        cjBYTE                          m_alphaRef;
        cjBool                          m_alphaTest;
        cjBool                          m_zTest;
        cjBool                          m_zWrite;
        
		cjCullMode						m_cullMode;
        
        
		// world,view,proj
		cjMatrix						m_world;
		cjMatrix						m_view;
		cjMatrix						m_proj;


		// Vertex stream, Index Buffer
//        cjGLVertexStreamAttribLayout    m_vsaLayout;
        
		cjFVertexStream::StreamAry		m_fstream;
		cjGLVertexStream*				m_stream[cjFVertexStream::MAX_STREAMS];
		cjINT32							m_streamStride[cjFVertexStream::MAX_STREAMS];
		cjINT32							m_streamOffset[cjFVertexStream::MAX_STREAMS];
		cjINT32							m_numStream;
				
		cjGLIndexBuffer*				m_indexBuf;
		cjINT32							m_indexBufBase;

		// skinning matrix palette
		cjINT32							m_skinBoneMatrixCount;	// 4*(n matrix)
		const cjVec4*					m_skinBoneMatrixs;		// cjopti 4x4 -> 4x3

		const cjScenePropRPtrList*		m_scenePropList;
		
		// Light
		cjBool							m_useDynamicLight;
		
//		D3DLIGHT9						m_lights[8];
//		cjBool							m_lightEnabled[8];

		// fog
		cjBool							m_distanceFogEnabled;
		cjFLOAT							m_distanceFogStart;
		cjFLOAT							m_distanceFogEnd;
		cjColor							m_distanceFogColor;

		cjGLRenderPass*                 m_renderPass[8];
		cjINT32							m_numRenderPass;
		cjGLRenderPass*                 m_curRenderPass;
   
		cjGLSavedState();

		void SetDefault();
	};

	cjGLRenInterface(cjGLRenDev* renDev);
	virtual ~cjGLRenInterface() {}

		// overrides cjRenInterface
	virtual cjBool Begin(cjViewport* viewport);
	virtual void End();

	virtual void PushState();
	virtual void PopState();

	virtual cjFRenTargetGrp* GetRenTargetGrp() 
	{ 
		return m_curState->m_renderTargetGrp; 
	}
    
	virtual cjBool SetRenTargetGrp(cjFRenTargetGrp* renderTargetGrp, cjTFLAG opt=0);
	virtual void SetViewport(cjINT32 x, cjINT32 y, cjINT32 width, cjINT32 height);
    virtual cjViewport* GetViewport() { return m_viewport; }

	virtual cjBool Clear(cjTFLAG clearFlag=ECLEAR_ALL, cjColor color=cjColor::BLACK, 
		cjFLOAT depth=1.0f, cjTFLAG stencil=0);

	virtual cjBool IsFinished() { return m_savedStateIndex == 0 ?  cjTrue : cjFalse; }
	virtual void DrawTest();

    virtual void SetCullMode(cjCullMode cullMode);

	virtual void SetWorld(const cjMatrix* world);
	virtual void SetView(const cjMatrix* view);
	virtual void SetProj(const cjMatrix* proj);
    
	// must be setted shaderProg
	virtual cjINT32 SetVertexStreams(cjFVertexStream** stream, cjINT32 numStream);
	virtual void SetIndexBuffer(cjFIndexBuffer* indexBuf, cjINT32 baseVertexIndex);
	virtual void SetSkinBoneMatrixs(const cjVec4* boneMatrixs, cjINT32 boneMatrixCount);

	virtual const cjScenePropRPtrList* GetScenePropList();
	virtual void SetScenePropList(const cjScenePropRPtrList* scenePropList);

	virtual void ResolveShader(const cjVertexStreamAttribLayoutResolver* resolver=cjNULL);
    
	virtual cjINT32 SetMaterial(cjMaterial* material);
	
    virtual void DrawPrimitive(cjPrimitiveType primitiveType, cjINT32 firstIndex,
		cjINT32 numPrimitive, cjINT32 minIndex, cjINT32 maxIndex);
    
protected:
    virtual cjBool SetupRenderPass(cjGLRenderPass* renderPass);

public:

	// cjtemp
	// Camera
	// virtual void SetCamera(cjVec3& camEye, cjVec3& camLookAt, cjVec3* camUp=NULL);
	//virtual void SetCameraRot(const cjVec3& right, const cjVec3& up, const cjVec3& dir);
	//virtual void SetCameraRot(cjFLOAT pitch, cjFLOAT yaw, cjFLOAT roll, cjBool isDelta);	
	//virtual void SetCameraMove(const cjVec3& newPos, cjBool isDelta, cjBool isLocal=cjFalse);
	//virtual void GetCameraDataToState(cjGLSavedState* state);
	//void CommitCameraData(cjGLSavedState* state=NULL);
    
    enum EDirtyFlag
    {
        EDF_STREAM_CHANGED                  = 0x00000001,
        EDF_MATRIX                          = 0x00000010,
        EDF_RENPASS                         = 0x00000100,
    };
    
    void SetDirty(cjTFLAG flag);
    void RemoveDirtyFlag(cjTFLAG flag);
    void ClearDirty();

	static void SetDefaultProj(cjFLOAT fovYRad=cjDegree2Rad(45.0f), cjFLOAT aspect=4.0f/3.0f, 
		cjFLOAT projNear=0.1f, cjFLOAT projFar=1000.0f);

	static void GetDefaultProj(cjMatrix& matProj) { matProj = ms_defaultProj; }


protected:
	void SetupShaderUniforms(cjGLShaderProgram* shaderProg);
    
    void _CommitStreams(cjINT32 firstIndex, cjINT32 pass);
    void _CommitMatrices();
    
private:
    cjBool SetBitmapMaterial(cjBitmapMaterial* bitmapMtl, cjGLMtlModifyAccum modifyAccum);
	cjBool SetShaderMaterial(const cjShaderMtl* shaderMtl, cjGLMtlModifyAccum modifyAccum);

	cjBool SetSimpleMaterial(cjMaterial* material, cjGLMtlModifyAccum modifyAccum, cjString* errStr,
        cjMaterial** errMaterial);
    
   	cjBool HandleCombinedMaterial(cjMaterial* material, cjINT32& numUsedPasses, cjINT32& numUsedStages, cjGLMtlModifyAccum modifyAccum, cjBool invertOutputAlpha=0, cjString* errStr=NULL, cjMaterial** errMaterial=NULL);
    
    void SetTextureStageBitmap(cjGLTextureStage& stage, cjBitmapMaterial* bitmapMtl);
    void CommitTexStageModify(cjGLTextureStage& stage, cjGLMtlModifyAccum* modifyAccum);
    
//	void SetShaderBitmap(cjGLTextureStage* stage, cjBitmapMaterial* bitmapMaterial);
    cjGLTexture* CacheTexture(cjFBaseTexture* texture);

	cjINT32 GetFVertexStream(cjFVertexStream*** vtxStreamAry);
    
    void CommitFinalBlendModify(cjGLMtlModifyAccum* modifyAccum);
    void CommitTexStageModify(cjGLTextureStage* stage, cjGLMtlModifyAccum* modifyAccum);
    
protected:
	cjGLRenDev*                     m_renDev;
	cjViewport*                     m_viewport;
    
    cjTFLAG                         m_dirtyFlag;

	// state stacks
	cjGLSavedState                  m_savedState[STATE_STACK_MAX];
	cjGLSavedState*                 m_curState;
	cjINT32                         m_savedStateIndex;
    
    cjGLRenderPass                  m_defRenderPass;
    cjGLRenderPassPool              m_renderPassPool;

	
	//
	// defaults
	//
	
	static cjMatrix					ms_defaultProj;

	static cjFLOAT					ms_projDefFovY;   // rad
	static cjFLOAT					ms_projDefAspect;
	static cjFLOAT					ms_projDefNear;
	static cjFLOAT					ms_projDefFar;
};


// TMaterialClass = cjShaderMtl..
// TMaterialType = cjMaterialType
template<class TMaterialClass, cjTFLAG TMaterialType>
inline TMaterialClass* cjMaterialCast(cjMaterial* material)
{
	if(!material)
        return NULL;
    
    if(material->m_typeMaterial & TMaterialType)
        return (TMaterialClass*)material;
    else if(material->m_typeMaterial == 0)
        return cjObjCast<TMaterialClass>(material);
    
    return NULL;
}

#define CJMATERIAL_CHK_FALLBACK(material)	\
	(useFallbacks && material ? material->CheckFallback() : material)

template<class TMaterialClass, cjTFLAG MaterialType >
inline TMaterialClass* cjGLCheckMaterial(cjGLRenInterface* ri,
 cjMaterial* material, cjGLMtlModifyAccum* modifyAccum=NULL, cjBool useFallbacks=0)
{
	TMaterialClass* rstMaterial = cjMaterialCast<TMaterialClass, MaterialType>(CJMATERIAL_CHK_FALLBACK(material));
    
	if(rstMaterial)
		return rstMaterial;
    
	cjMtlModifier* modifier = cjMaterialCast<cjMtlModifier, cjMT_Modifier>(CJMATERIAL_CHK_FALLBACK(material));
    
	while(modifier)
	{
		rstMaterial = cjMaterialCast<TMaterialClass, MaterialType>(CJMATERIAL_CHK_FALLBACK(modifier->m_material));
		modifier = cjMaterialCast<cjMtlModifier,cjMT_Modifier>(CJMATERIAL_CHK_FALLBACK(modifier->m_material));
	}
    
	if(rstMaterial && modifyAccum)
	{
		cjBool needSource = 1;
		modifier = cjMaterialCast<cjMtlModifier,cjMT_Modifier>(CJMATERIAL_CHK_FALLBACK(material));
        
		while(modifier)
		{
			if(modifier->HasFallback() && useFallbacks)
				modifyAccum->m_fallback = modifier;
            
			cjTexMtlModifier* texModifier = cjMaterialCast<cjTexMtlModifier,cjMT_TexModifier>(modifier);
			if(texModifier)
			{
				cjMatrix* matTexture = texModifier->GetMatrix(ri->GetViewport()->GetCurTime());
                
				if(matTexture)
				{
					modifyAccum->m_commitTexTransforms = cjTrue;
					modifyAccum->m_texMatrix = modifyAccum->m_texMatrix * (*matTexture);
				}
                
				if( needSource && texModifier->m_texCoordSource != cjTexCoordSrcSameToPrevious)
				{
					modifyAccum->m_commitTexTransforms  = cjTrue;
					modifyAccum->m_texCoordSource       = texModifier->m_texCoordSource;
					modifyAccum->m_texCoordCount        = texModifier->m_texCoordCount;
					modifyAccum->m_texCoordProjected    = texModifier->m_texCoordProjected;
					
                    needSource = 0;
				}
			}
            
			cjFinalBlendMtlModifier* finalBlend
            = cjMaterialCast<cjFinalBlendMtlModifier,cjMT_FinalBlend>(modifier);
			
            if(finalBlend)
			{
				modifyAccum->m_commitFramebufferBlending = cjTrue;
                
				modifyAccum->m_frameBufferBlend    = finalBlend->m_frameBufferBlend;
				modifyAccum->m_zWrite               = finalBlend->m_zWrite;
				modifyAccum->m_zTest                = finalBlend->m_zTest;
				modifyAccum->m_alphaTest            = finalBlend->m_alphaTest;
				modifyAccum->m_alphaRef             = finalBlend->m_alphaRef;
				modifyAccum->m_twoSided             = finalBlend->m_twoSided;
			}
            
			cjColorMtlModifier* colorModifier = cjMaterialCast<cjColorMtlModifier,cjMT_ColorModifier>(modifier);
			if(colorModifier)
			{
				modifyAccum->m_commitColor = cjTrue;
                
				modifyAccum->m_alphaBlend           |= colorModifier->m_alphaBlend;
				modifyAccum->m_twoSided             |= colorModifier->m_renderTwoSided;
				modifyAccum->m_texFactorColor        = colorModifier->m_color;
			}
            
			modifier = cjMaterialCast<cjMtlModifier,cjMT_Modifier>(CJMATERIAL_CHK_FALLBACK(modifier->m_material));
		}
	}

	return rstMaterial;
}

#endif
