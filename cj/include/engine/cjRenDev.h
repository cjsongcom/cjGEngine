
#ifndef _CJRENDEV_H_
#define _CJRENDEV_H_

class cjEAGLAdapter;
class cjMaterial;
class cjShaderMtl;
//class cjScenePropRPtrList;



// must keep POD state, no virtual function is allowed
struct CJENGINE_API cjRenDevParam : public cjMem
{
	cjRenDevParam();
	cjRenDevParam(const cjRenDevParam& rhs);
	cjRenDevParam& operator= (const cjRenDevParam& rhs);

	void SetDefault();

	cjRSurfaceFmt					m_format; // framebuffer surface format
	cjRSurfaceFmt					m_depthStencil;
	cjRFBSwapEffect					m_swapEffect;	

	cjRPresentInterval				m_interval;
	cjUINT32						m_refreshRate;

	// can be null or same with m_renderWnd if m_renderWnd is valid
	void*							m_focusWnd;
	void*							m_renderWnd;

	// can be null if m_renderWnd is NULL
	cjUINT32						m_renderWndWidth;
	cjUINT32						m_renderWndHeight;

	cjBool							m_fullScreen;
	cjBool							m_useStencil;
    
#if defined(CJ_PLATFORM_APPLE)
    cjEAGLAdapter*                  m_eaglAdapter;
#endif
   
	inline cjUINT32 GetWidth() { return m_renderWndWidth; }
	inline cjUINT32 GetHeight() { return m_renderWndHeight; }

	inline cjRSurfaceFmt GetFormat() { return m_format; } // framebuffer surface format
	inline cjRSurfaceFmt GetDSFormat() { return m_depthStencil; } // depth-stencil

	cjBool IsUseVSync()	{ return m_interval == cjRPRESENT_INTERVAL_ONE ? true : false; }
	cjBool IsFullScreen() { return m_fullScreen; }
};


//
// cjRenInterface
//

class CJENGINE_API cjRenInterface : public cjMem
{
public:
	enum ECLEAR_FLAG
	{
		ECLEAR_NONE			= 0,
		ECLEAR_COLOR		= 0x00000001,
		ECLEAR_DEPTH		= 0x00000010,
		ECLEAR_STENCIL		= 0x00000100,
		ECLEAR_ALL			= ECLEAR_COLOR | ECLEAR_DEPTH | ECLEAR_STENCIL
	};
    
    class cjVertexStreamAttribLayoutResolver : public cjMem
    {
    public:
        virtual cjFVertexStreamAttribLayout* OnResolveVertexStreamAttribLayout(
			const cjFVertexStream::StreamAry streams, cjINT32 streamCount, 
			cjRenDev* renDev=cjNULL) const = 0;
    };

	cjRenInterface() {}    

	virtual cjBool Begin(cjViewport* viewport) = 0;
	virtual void End() = 0;

	virtual void PushState() = 0;
	virtual void PopState() = 0;

	virtual cjFRenTargetGrp* GetRenTargetGrp() = 0;
	virtual cjBool SetRenTargetGrp(cjFRenTargetGrp* renderTargetGrp, cjTFLAG opt=0) = 0;
	virtual void SetViewport(cjINT32 x, cjINT32 y, cjINT32 width, cjINT32 height) = 0;
    virtual cjViewport* GetViewport() { return NULL; }

	virtual cjBool Clear(cjTFLAG clearFlag=ECLEAR_ALL, cjColor color=cjColor::BLACK, 
		cjFLOAT depth=1.0f, cjTFLAG stencil=0) = 0;

	virtual cjBool IsFinished() = 0;

	virtual void DrawTest() = 0;

	virtual cjINT32 SetMaterial(cjMaterial* material) = 0;

	// World, View, Projection
		// matrix=cjNULL : set worldmatrix to identity
	virtual void SetWorld(const cjMatrix* world=cjNULL) = 0;
	virtual void SetView(const cjMatrix* view=cjNULL) = 0; 
	virtual void SetProj(const cjMatrix* proj=cjNULL) = 0;
	    
    // Vertex/Index Stream
	virtual cjINT32 SetVertexStreams(cjFVertexStream** streams, cjINT32 numStream) = 0;	
	virtual void SetIndexBuffer(cjFIndexBuffer* indexBuf, cjINT32 baseVertexIndex) = 0;
	virtual void SetSkinBoneMatrixs(const cjVec4* boneMatrixs, cjINT32 boneMatrixCount) = 0;
	
	virtual const cjScenePropRPtrList* GetScenePropList() = 0;
	virtual void SetScenePropList(const cjScenePropRPtrList* scenePropList) = 0;

	virtual void ResolveShader(const cjVertexStreamAttribLayoutResolver* resolver) = 0;
		
	virtual void DrawPrimitive(cjPrimitiveType primitiveType, cjINT32 firstIndex,
		cjINT32 numPrimitive, cjINT32 minIndex, cjINT32 maxIndex) = 0;
};


//
// cjRenDev
//

CJOBJ_DECL_ABSTRACT_BEGIN(CJENGINE_API, cjRenDev, cjObj, 0, cjEngine)

public:
	enum ERENCAPS
	{
		ERENCAPS_CUBEMAP,
		ERENCAPS_ZBIAS,
		ERENCAPS_RGBA8888,
		ERENCAPS_RGB565,
		ERENCAPS_RENDERTOTEX_RGBA8888,
		ERENCAPS_RENDERTOTEX_RGB565,
		ERENCAPS_RENDERTOTEX_NPOT,
		ERENCAPS_MAX
	};

	cjRenDev();

	virtual cjBool Init(const cjRenDevParam* renDevPrm) = 0;
	virtual void Shutdown() = 0;

	virtual cjBool Exec(const cjNameChar* cmd) = 0;

	virtual cjBool SetResolution(cjViewport* viewport, cjINT32 width, cjINT32 height, 
		cjBool fullScreen, cjINT32 colorBPP=32, cjUINT32 refreshRate=0) = 0;
	virtual void UnSetResolution(cjViewport* viewport) = 0;
	virtual cjBool IsFullScreen() = 0;

	virtual cjBool TestDeviceStatus() = 0;
	virtual cjRenInterface* BeginRender(cjViewport* viewPort) = 0;
	virtual void EndRender(cjRenInterface* ri) = 0;
	virtual void Present(cjViewport* viewport) = 0;

	// render resource
	virtual void DisposeResCache(cjResCacheID cacheID) = 0;
	virtual void FlushResCache() = 0;

	// rendering
	virtual cjRenDevParam* GetRenDevParam() = 0;
		// render target operation
	virtual cjFRenTargetGrp* GetCurRenTargetGrp() = 0;
	//virtual cjBool BeginUsingDefaultRenTargetGrp(cjTFLAG clearFlag) = 0;
	//virtual cjBool BeginUsingRenTargetGrp(cjFRenTargetGrp* renTargetGrp, 
	//	cjTFLAG clearFlag) = 0;
	//virtual void EndUsingRenTargetGrp() = 0;
		// 
	virtual void SetBackgroundColor(cjColor& color) = 0;
	virtual void SetDepthClear(cjFLOAT depth) = 0;
	virtual void SetStencilClear(cjTFLAG stencil) = 0;
	virtual void ClearBuffer(cjTFLAG clearFlag, const cjRect<cjFLOAT>* rect=NULL) = 0;
    
    // info
    cjINT32 GetNumMaxTextureStages() { return m_numMaxTextureStages; }

	// camera
    void SetCameraData(const cjCamera* camera);
    void SetCameraData(const cjVec3& eye, const cjVec3& dir, const cjVec3& up, 
		const cjVec3& right, const cjFrustum& frustum, const cjRect<cjFLOAT>& port);
	void SetScreenSpaceCameraData(const cjRect<cjFLOAT>* port= NULL);
    void GetCameraData(cjCamera& camera);
    void GetCameraData(cjVec3& eye, cjVec3& dir, cjVec3& up, cjVec3& right, 
		cjFrustum& frustum, cjRect<cjFLOAT>& port);
	
	// shader
//	virtual fnShader* GetShader(const fnTChar* shaderName) = 0;
//	virtual fnBool AddShader(fnShader* newShader) = 0;
//	virtual void DisposeShader(fnShader* shader) = 0;
//	virtual fnMaterial* GetDefaultMaterial() = 0;
    
#if defined(CJ_PLATFORM_APPLE)
    inline cjEAGLAdapter* GetEAGLAdapter() { return GetRenDevParam()->m_eaglAdapter; }
#endif

    virtual cjBool IsSupportHWCompressSurfaceFmt() { return cjFalse; }
    virtual cjBool IsHWCompressSurfaceFmt(cjRSurfaceFmt fmt) { return cjFalse; }

	static const cjNameChar* GetDefaultShaderMtlName(cjBool nameOnly=cjFalse);
	virtual cjShaderMtl* GetDefaultShaderMtl();	

	static const cjNameChar* GetDefaultMtlName(cjBool nameOnly=cjFalse);
	static cjMaterial* GetDefaultMtl();

protected:
	virtual void OnDestroy();

	virtual void _DoSetCamData(const cjVec3& eye, const cjVec3& dir, const cjVec3& up, 
		const cjVec3& right, const cjFrustum& frustum, const cjRect<cjFLOAT>& port) = 0;
    
	cjShaderMtl* GenerateDefaultShaderMtl();
	cjMaterial* GenerateDefaultMtl();

protected:
    cjINT32                                 m_numMaxTextureStages;
    
public:
    cjBool                                  m_useTrilinearFilter;
    cjBool                                  m_supportAnisotropicFilter;
    cjINT32                                 m_levelAnisotropicFilter;

	cjObjRef<cjShaderMtl>					m_defaultShaderMtl;
	cjObjRef<cjMaterial>					m_defaultMtl;

CJOBJ_DECL_ABSTRACT_END(cjRenDev)


#endif

