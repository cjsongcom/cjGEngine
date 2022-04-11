
#ifndef _CJGLRENDEV_H_
#define _CJGLRENDEV_H_

#include "cjGLCommon.h"
#include "cjGLRenRes.h"


class cjGLRenInterface;
class cjGLResCache;
class cjGLShaderCache;
class cjGLShaderProgram;
class cjGLRenTargetGrp;
class cjGLRenderPass;

#if defined(CJ_PLATFORM_APPLE)
class cjEAGLAdpater;
#endif


struct CJENGINE_API cjGLRenDevParam : public cjRenDevParam
{
	cjGLRenDevParam();
	cjGLRenDevParam& operator =(const cjRenDevParam& rhs);

	GLfloat GetGLAnisoLevel() { return m_glAnisotropyLevel; }

	// gl specific
	cjINT32							m_glMultisampleHint;
	cjINT32							m_glMultisampleBuffers;
	cjINT32							m_glMultisampleSamples;

	cjBool							m_glWasFullScreen;
	
	cjFLOAT							m_glAnisotropyLevel;
	cjBool							m_glPendingGLCmdFlush;
};


CJOBJ_DECL_CLASS_BEGIN(CJENGINE_API, cjGLRenDev, cjRenDev, 0, cjEngine)
    
    friend class cjGLResCache;

public:

#if defined(CJ_PLATFORM_WIN32)
	// GL function start
    #define CJGLCAPS(name) static cjBool ms_caps_##name;
    #define CJGLPROC(funccate,ret,glfunc,glfuncreal,prm) static ret (cjSTDCALL *glfunc)prm;
    #define CJGLEXTPROC(funccate,ret,glfunc,glfuncreal,prm) static ret (cjSTDCALL *glfunc)prm;
    #include "cjGLFuncs.h"
    
#elif defined(CJ_PLATFORM_APPLE)

    #define CJGLCAPS(name) static cjBool ms_caps_##name;
    #define CJGLPROC(funccate,ret,glfunc,glfuncreal,prm) static ret (*glfunc)prm;
    #define CJGLEXTPROC(funccate,ret,glfunc,glfuncreal,prm) static ret (*glfunc)prm;
    #include "engine/apple/cjIOSGLFuncs.h"
    
#endif

    #undef CJGLCAPS
	#undef CJGLPROC
	#undef CJGLEXTPROC
	// GL function end

	typedef cjRect<cjUINT32>				DisplayModeData;

public:
	virtual cjBool Init(const cjRenDevParam* renDevPrm);
	virtual void Shutdown();

	virtual cjBool Exec(const cjNameChar* cmd);

	virtual cjBool SetResolution(cjViewport* viewport, cjINT32 width, cjINT32 height,
		cjBool fullScreen, cjINT32 colorBPP=32, cjUINT32 refreshRate=0);
	virtual void UnSetResolution(cjViewport* viewport);
	virtual cjBool IsFullScreen() { return m_devParam.IsFullScreen(); }

	virtual cjBool TestDeviceStatus();
	virtual cjRenInterface* BeginRender(cjViewport* viewPort);
	virtual void EndRender(cjRenInterface* ri);
	virtual void Present(cjViewport* viewport);

	// render resource
	virtual void DisposeResCache(cjResCacheID cacheID);
	virtual void FlushResCache();

	// rendering
	virtual cjRenDevParam* GetRenDevParam() { return &m_devParam; }
	virtual cjGLRenDevParam* GetGLRenDevParam() { return &m_devParam; }
		// render target operation
	virtual cjFRenTargetGrp* GetCurRenTargetGrp();

	virtual cjFRenTargetGrp* GetDefaultRenTargetGrp();

	virtual cjBool BeginUsingDefaultRenTargetGrp(cjTFLAG clearFlag);
	virtual cjBool BeginUsingRenTargetGrp(cjFRenTargetGrp* renTargetGrp, cjTFLAG clearFlag);
	virtual void EndUsingRenTargetGrp();
		//
	virtual void SetBackgroundColor(cjColor& color);
	virtual void SetDepthClear(cjFLOAT depth);
	virtual void SetStencilClear(cjTFLAG stencil);
	virtual void ClearBuffer(cjTFLAG clearFlag, const cjRect<cjFLOAT>* rect=NULL);

public:
	cjGLRenDev();

	// ret: 0 for OK or Error
	static cjUINT32 GetLastGLErr(cjBool* rstOk=NULL, const char* file=NULL, cjINT32 line=0);

	void BindGLContextToThisThread();
	cjBool GetClosestDisplayMode(cjINT32 width, cjINT32 height, cjINT32 colorBPP);

	// cjGLRenderTargetGrp
//	cjGLRenTargetGrp* GetCurGLRenTargetGrp(); // pair with GetCurRenTargetGrp();

	cjGLResCache* FindResCache(cjResCacheID cacheID);

	// gl error to text
	static const char* GetGLFrameBufferStatusText(GLenum frameBufferStatus);
	static cjBool CheckGLFrameBufferStatus();

	static cjBool CheckGLErrStatus(GLhandle handle, GLenum statusType,
		const cjChar* func=NULL, cjUINT32 line=0);
	static cjBool CheckGLErrStatus(GLhandle handle, GLenum statusType,
		const cjUni* func=NULL, cjUINT32 line=0);

	void TestDraw();
    
    cjINT32 GetFrameCounter() { return m_frameCounter; }
    
//    static cjGLVertexStream* GenerateVertexStream(cjGLRenDev* renDev, cjResCacheID cacheID);

protected:
	virtual void OnDestroy();

	virtual void _DoSetCamData(const cjVec3& eye, const cjVec3& dir, const cjVec3& up, 
		const cjVec3& right, const cjFrustum& frustum, const cjRect<cjFLOAT>& port);

	// GL function management
	static cjBool LookupGLProcedure();
	static cjBool _FindGLExtension(const char* name);
   
#if defined(CJ_PLATFORM_WIN32)
	static cjBool _BindGLProcedure(void*& procAddr, const char* name, const char* funcCate);
#elif defined(CJ_PLATFORM_APPLE)
	static cjBool _BindGLProcedureEAGL(void*& procAddr, const char* name, const char* funcCate);
#endif
    
	void CleanGLContext();

	void CreateDefaultRenderTargetGrp();
	void DestroyDefaultRenderTargetGrp();
    
    void CreateDefaultShader();
    void DestroyDefaultShader();
    
private:
//	// cjGLRenResCache
//    template<class TResCacheType>
//    void AddResCache(TResCacheType* resCache)
//    {
//        template<
//        m_resCache.Add(resCache);
//    }
//    
//    
//    
//    template<>
//    void AddResCache<cjGLShaderCache>(cjGLShaderCache* resCache) { m_shaderCache.Add(resCache); }
//
//    template<class TResCacheType>
//	void RemoveResCache(TResCacheType* resCache) { m_resCache.Remove(resCache); }
//    template<>
//	void RemoveResCache(cjGLShaderCache* resCache) { m_shaderCache.Remove(resCache); }
//    
//    template<class TResCacheType>
//    TResCacheType* FindResCache(cjResCacheID cacheID) { return m_resCache.Find(cacheID); }
//    template<>
//    cjGLShaderCache* FindResCache<cjGLShaderCache>(cjResCacheID cacheID)
//        { return m_shaderCache.Find(cacheID); }
    
		// returns next resCache
	void AddResCache(cjGLResCache* resCache);
    void RemoveResCache(cjGLResCache* resCache);
    
    void CreateResCacheContainers();
    void DestroyResCacheContainers();
    
//	// cjGLShaderCache
//		// returns next cjGLShaderCache
//	void AddShaderCache(cjGLShaderCache* shaderCache) { m_shaderCache.Add(shaderCache); }
//	void RemoveShaderCache(cjGLShaderCache* shaderCache) { m_shaderCache.Remove(shaderCache); }
//	cjGLShaderCache* FindShaderCache(cjResCacheID cacheID) { return m_shaderCache.Find(cacheID); }
    
private:
	cjGLRenInterface*							m_renInterface;
	cjGLRenDevParam								m_devParam;

    typedef cjTResCacheContainer<cjGLResCache> GLResCacheContainer;
	GLResCacheContainer*                        m_resCache[cjRSCID_Num];

//	cjGLRenderPass								m_defaultRenderPass;
//	cjGLRenderPassPool							m_renderPassPool;

	// GL caps
//	static cjBool								ms_supportRenderTarget;
	static cjBool								ms_supportNPOTexture;
	static cjBool								ms_supportStencil;
//	static cjBool								ms_supportVertexShader;
//	static cjBool								ms_supportPixelShader;
	static cjUINT32								ms_supportAnisotropyLevel;
	static cjBool								ms_supportZBias;
//	static cjBool								ms_supportCubemaps;
	static cjINT32								ms_supportMaxTextureUnits;
    
    static cjBool                               ms_wasGLProclookup;
	
	// camera stuff
	cjMatrix3									m_glView;
	cjMatrix3									m_glProj;
	cjMatrix3									m_glInvView;

	cjFrustum									m_glFrustum;
	cjRect<cjFLOAT>								m_glViewport;

	static cjBool								ms_initGL;

	// gl device context
	cjBool										m_glContextValid;

#if defined(CJ_PLATFORM_WIN32)
	static HMODULE								ms_winModuleGL;
	static HMODULE								ms_winModuleGDI;
	static HGLRC								ms_winCurGLCtx;

	HWND										m_wglWnd;
	HGLRC										m_wglRenCtx;
	HDC											m_wglDC;
#elif defined(CJ_PLATFORM_APPLE)
    cjEAGLAdapter*                              m_eaglAdapter;
#endif

	cjTAry<DisplayModeData>						m_displayModeDatas;

	// render stuff
	cjViewport*									m_usingRenViewport;

	cjFRenTargetGrp*							m_curRenTargetGrp;
	cjFRenTargetGrp*							m_defaultRenTargetGrp;

	cjColorF									m_colorBackground;
	cjFLOAT										m_depthClear;
	cjINT32										m_stencilClear;
    
    cjINT32                                     m_frameCounter;
   
CJOBJ_DECL_CLASS_END(cjGLRenDev)


CJENGINE_API cjGLResCache* cjResolveGLRenRes(const cjRTTI* rtti, cjGLRenDev* renDev, 
	cjFRenResSrc* renRes, cjBool doNotCreateCacheWhenNotExist=cjFalse);

template<class TGLRenResType>
inline TGLRenResType* cjTResolveGLRenRes(cjGLRenDev* renDev, cjFRenResSrc* renRes,
    cjBool doNotCreateCacheWhenNotExist=cjFalse)
{
	if(!renRes)
		return cjNULL;

	return (TGLRenResType*)cjResolveGLRenRes(TGLRenResType::cjrStaticRTTI(), renDev, 
		renRes, doNotCreateCacheWhenNotExist);
}


#endif
