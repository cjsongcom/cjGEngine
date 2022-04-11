
#include "cjEnginePCH.h"

#include "cjGLRenRes.h"
#include "cjGLRenInterface.h"

#if defined(CJ_PLATFORM_APPLE)
#include "engine/apple/cjEAGLAdapter.h"
#endif

#include "cjGLRenDev.h"

CJOBJ_IMPL_CLASS(cjGLRenDev)

cjBool cjGLRenDev::ms_initGL = cjFalse;

#if defined(CJ_PLATFORM_WIN32)
HMODULE	cjGLRenDev::ms_winModuleGL = NULL;
HMODULE cjGLRenDev::ms_winModuleGDI = NULL;
HGLRC cjGLRenDev::ms_winCurGLCtx = NULL;
#elif defined(CJ_PLATFORM_APPLE)

#else
#error "ERR!-cjGLRenDev.cpp-Not Implemented"
#endif


// GL caps
//cjBool cjGLRenDev::ms_supportRenderTarget				= cjFalse;
cjBool cjGLRenDev::ms_supportNPOTexture					= cjFalse;
cjBool cjGLRenDev::ms_supportStencil					= cjFalse;
//cjBool cjGLRenDev::ms_supportVertexShader				= cjTrue;
//cjBool cjGLRenDev::ms_supportPixelShader				= cjFalse;
cjUINT32 cjGLRenDev::ms_supportAnisotropyLevel			= 0;
cjBool cjGLRenDev::ms_supportZBias						= cjFalse;
//cjBool cjGLRenDev::ms_supportCubemaps					= cjFalse;
cjINT32 cjGLRenDev::ms_supportMaxTextureUnits			= 0;


//
// cjGLRenDevParam
//

cjGLRenDevParam::cjGLRenDevParam()
{
	m_glMultisampleHint		= 0;
	m_glMultisampleBuffers	= 0;
	m_glMultisampleSamples	= 0;

	m_glWasFullScreen		= cjFalse;

	m_glAnisotropyLevel		= 0;
	m_glPendingGLCmdFlush	= cjFalse;
}

cjGLRenDevParam& cjGLRenDevParam::operator =(const cjRenDevParam& rhs)
{
	cjMemCpy(((cjRenDevParam*)this), &rhs, sizeof(cjRenDevParam));
	return *this;
}


//
// cjGLRenDev
//

cjGLRenDev::cjGLRenDev()
{
	m_renInterface = NULL;
	m_glContextValid = cjFalse;
    cjMemZero(m_resCache, sizeof(m_resCache));
    
#if defined(CJ_PLATFORM_WIN32)
	m_wglWnd = NULL;
	m_wglRenCtx = NULL;
	m_wglDC = NULL;

#elif defined(CJ_PLATFORM_APPLE)
    m_eaglAdapter = NULL;
#endif

	m_displayModeDatas.RemoveAll();

	m_usingRenViewport = NULL;
	m_curRenTargetGrp = NULL;
	m_defaultRenTargetGrp = NULL;
    
    CreateResCacheContainers();

	cjShaderManager::StaticInit();
}

void cjGLRenDev::OnDestroy()
{
	if(m_renInterface)
	{
		delete m_renInterface;
		m_renInterface = NULL;
	}

    FlushResCache();

	DestroyDefaultRenderTargetGrp();
    
    DestroyDefaultShader();
    
    DestroyResCacheContainers();

	cjShaderManager::StaticShutdown();

	CleanGLContext();

	cjSuper::OnDestroy();
}

cjBool cjGLRenDev::Init(const cjRenDevParam* renDevPrm)
{
	cjSuper::Init(renDevPrm);

	cjGIsOpenGL = cjTrue;
    m_frameCounter = 0;

	m_devParam = *renDevPrm;

#if defined(CJ_PLATFORM_WIN32)

	for(INT32 i=0;;i++)
	{
		DEVMODE displayMode;
		cjMemZero(&displayMode, sizeof(displayMode));
		displayMode.dmSize = sizeof(displayMode);

		if(!EnumDisplaySettings(NULL, i, &displayMode))
			break;

		DisplayModeData modeData;
		modeData.m_left		= displayMode.dmPelsWidth;
		modeData.m_top		= displayMode.dmPelsHeight;
		modeData.m_right	= displayMode.dmBitsPerPel;
		modeData.m_bottom	= displayMode.dmDisplayFrequency;

		m_displayModeDatas.PushUnique(modeData);
	}

	if(!ms_initGL)
	{
		ms_winModuleGL = (HMODULE)cjGetDllLibHandle(CJ_GL_MODULENAME);
		ms_winModuleGDI = (HMODULE)cjGetDllLibHandle(_CM("GDI32.dll"));

		if(!ms_winModuleGL)
		{
			cjLOG("ERR! - Failed to load opengl module..!");
			return cjFalse;
		}

//		cjGLRenDev::wglGetProcAddress = (PROC (cjSTDCALL *)(LPCSTR))cjGetDllProcAddr(ms_winModuleGL, "wglGetProcAddress");
//		if(!cjGLRenDev::wglGetProcAddress)
//			return cjFalse;
		
		ms_initGL = cjTrue;
	}

#elif defined(CJ_PLATFORM_APPLE)
    m_eaglAdapter = m_devParam.m_eaglAdapter;
#else
  #pragma message("ERR! - Needs implementation of CollectDisplayMode")
#endif

	m_renInterface = new cjGLRenInterface(this);
    
	return cjTrue;
}

void cjGLRenDev::Shutdown()
{ 
	cjSuper::Shutdown();

	delete this;
}

cjBool cjGLRenDev::Exec(const cjNameChar* cmd)
{
	return cjFalse;
}

cjBool cjGLRenDev::SetResolution(cjViewport* viewport, cjINT32 width, cjINT32 height, 
	cjBool fullScreen, cjINT32 colorBPP, cjUINT32 refreshRate)
{
	if(!viewport)
		return cjFalse;
	
	if(colorBPP != 16)
		colorBPP = 32;
	
	if(width == -1 || height == -1)
	{
		// using current viewport size
		width = viewport->GetWidth();
		height = viewport->GetHeight();		
	}
	
	m_devParam.m_renderWndWidth = width;
	m_devParam.m_renderWndHeight = height;
	m_devParam.m_fullScreen = fullScreen ? cjTrue : cjFalse;

	if(m_usingRenViewport)
    {
    	cjError(_CT("ERR! - this device is locked !!!"));
    }

	FlushResCache();

    UnSetResolution(viewport);

	// 0:GL_DONT_CARE, 1:GL_FASTEST, 2:GL_NICEST
	m_devParam.m_glMultisampleHint    = cjMax(m_devParam.m_glMultisampleHint, (cjINT32)0);
	m_devParam.m_glMultisampleHint    = cjMin(m_devParam.m_glMultisampleHint, (cjINT32)2);
	m_devParam.m_glMultisampleBuffers = cjMax(m_devParam.m_glMultisampleBuffers, (cjINT32)0);
	m_devParam.m_glMultisampleSamples = cjMax(m_devParam.m_glMultisampleSamples, (cjINT32)0);

#if defined(CJ_PLATFORM_WIN32)
	m_wglWnd = (HWND)viewport->GetWnd();
	m_wglDC	= GetDC(m_wglWnd);

	if(m_devParam.m_fullScreen)
	{
		if(!GetClosestDisplayMode(width, height, colorBPP))
		{
			cjLOG(_CT("Can't find closest modes.. width=%d, height=%d, colorBPP=%d"), 
				width, height, colorBPP);

			return cjFalse;
		}

		cjUINT32 refreshRate = m_devParam.m_refreshRate ? 
			cjClamp<cjUINT32>(m_devParam.m_refreshRate, 60, 100) : 0;

		DEVMODEA displayMode;
		cjMemZero(&displayMode, sizeof(displayMode));
		displayMode.dmSize = sizeof(displayMode);
		displayMode.dmPelsWidth  = width;
		displayMode.dmPelsHeight = height;
		displayMode.dmBitsPerPel = colorBPP * 8;

		if(refreshRate)
		{
			displayMode.dmDisplayFrequency = refreshRate;
			displayMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY | DM_BITSPERPEL;
		}
		else
		{
			displayMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
		}

		if(ChangeDisplaySettingsA(&displayMode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			cjLOG(_CT("ChangeDisplaySettingsA failed, w=%d h=%d, colorbpp=%d"), width, height, colorBPP);
			return cjFalse;
		}
	}

	cjINT32 stencilBits = 0;
	cjINT32 depthBits = colorBPP == 16 ? 16 : 24;

	PIXELFORMATDESCRIPTOR pxlFmtDesc =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		colorBPP,
		0,0,0,0,0,0,
		0,0,
		0,0,0,0,0,
		depthBits,
		stencilBits,
		0,
		PFD_MAIN_PLANE,
		0,
		0,0,0
	};
	
	cjINT32 nPixelFormat = ::ChoosePixelFormat(m_wglDC, &pxlFmtDesc);
	::SetPixelFormat(m_wglDC, nPixelFormat, &pxlFmtDesc);
 

	m_wglRenCtx = ::wglCreateContext(m_wglDC);
	cjAssert(m_wglRenCtx);


	cjBool rst = viewport->ResizeViewport(m_devParam.m_fullScreen, width, height);

	if(!rst)
	{
		if(m_devParam.m_fullScreen)
			ChangeDisplaySettings(NULL,0);

		return cjFalse;
	}

#else

    ///////////////////////////////////////////////////
    // EAGL
    ///////////////////////////////////////////////////


#endif

	m_glContextValid = cjTrue;

	BindGLContextToThisThread(); // nvoglv32.dll on win32 being loaded in here

//	wglGetExtensionsString

	// lookup gl extension functions - must be called after BindGLContextToThisThread()
	LookupGLProcedure();
     
	cjLOG(_CT("[GL] GL_VENDOR   : %s"), (char*)glGetString(GL_VENDOR));
	cjLOG(_CT("[GL] GL_RENDERER : %s"), (char*)glGetString(GL_RENDERER));
	cjLOG(_CT("[GL] GL_VERSION  : %s"), (char*)glGetString(GL_VERSION));


//	cjLOG(_CT("[GL] GL_VERSION  : %s"), (char*)glGetString(GL_EXTENSIONS));
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &m_numMaxTextureStages);


	if(ms_caps_cjGL_EXT_texture_filter_anisotropic)
	{
		cjLOG(_CT("[GL] using GL_EXT_texture_filter_anisotropic.."));

		GLfloat maxAnisotropy;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);

		m_devParam.m_glAnisotropyLevel = cjMax<cjFLOAT>(1.0f, 
			cjMin<cjFLOAT>(maxAnisotropy, m_devParam.m_glAnisotropyLevel));
	}


	if(!ms_caps_cjGL_multisample)
	{
		cjLOG(_CT("[GL] using GL_multisample is disabled.."));
	}
	else
	{
        GLint samplingBuffers, numSamples;
		glGetIntegerv( GL_SAMPLE_BUFFERS, &samplingBuffers);
		glGetIntegerv( GL_SAMPLES, &numSamples);

		cjLOG(_CT("[GL] Multisampling info buffers=%d , samples=%d"), 
			samplingBuffers, numSamples);

		if(!samplingBuffers || !numSamples)
		{
			cjLOG(_CT("[GL] Multisampling is disabled.."));
			ms_caps_cjGL_multisample = cjFalse;
		} 
		else
        {
			cjLOG(_CT("[GL] using GL_ARB_multisample.."));
//			glEnable(GL_MULTISAMPLE);
		}
	}


//	// using pixel shader program
//    if(ms_caps_cjGL_ARB_fragment_program)
//    {
//		cjLOG(_CT("[GL] using GL_ARB_fragment_program.. and using pixel shaders"));
//
//		ms_supportVertexShader = cjTrue;
//		ms_supportPixelShader = cjTrue;		
//    }
//	else
//	{
//        ms_caps_cjGL_ARB_vertex_program = cjFalse;
//		ms_supportVertexShader = cjFalse;
//		
//		cjLOG(_CT("[GL] Warn! - using vertex and pixel shader is disabled.."));
//	}

//	if(ms_caps_cjGL_ARB_vertex_buffer_object)
//	{
//		cjLOG(_CT("[GL] using GL_ARB_fragment_program.."));
//	}
//	else
//	{
//		cjLOG(_CT("[GL] Warn! - using GL_ARB_fragment_program is disabled.."));
//		ms_caps_cjGL_ARB_vertex_buffer_object = cjFalse;
//	}
//
//	// Check Texture compression
//	if(!ms_caps_cjGL_ARB_texture_compression)
//	{
//		cjLOG(_CT("[GL] Warn! - using GL_ARB_texture_compression is disabled.."));
//		ms_caps_cjGL_EXT_texture_compression_s3tc = cjFalse;
//	}
//
//	if(!ms_caps_cjGL_EXT_texture_compression_s3tc)
//	{
//		cjLOG(_CT("[GL] Warn! - using GL_EXT_texture_compression_s3tc is disabled.."));
//	}

//	if(!ms_caps_cjGL_EXT_bgra)
//	{
//		cjError(_CT("[GL] Err! - GL_EXT_bgra is not supported.."));
//		return cjFalse;
//	}
//
//    if(ms_caps_cjGL_EXT_framebuffer_object)
//	{
//		cjLOG(_CT("[GL] using GL_EXT_framebuffer_object(RenderTarget) "));
//		ms_supportRenderTarget = cjTrue;
//	}
//	else
//	{
//		cjLOG(_CT("[GL] Warn! - using GL_EXT_framebuffer_object(RenderTarget) is disabled.."));
//	}

//	ms_supportNPOTexture = ms_caps_cjGL_ARB_texture_non_power_of_two;

#ifdef CJ_PLATFORM_WIN32
	//if(ms_caps_cjWGL_EXT_swap_control)
		wglSwapIntervalEXT(m_devParam.IsUseVSync() ? TRUE : FALSE);
#endif

	// z
	glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);		// enable or disable writing into the depth buffer
	glDepthFunc(GL_LEQUAL);
	// glDepthRange

	// blend
	glBlendFunc(GL_ONE, GL_ZERO);
	glEnable(GL_BLEND);

//	glFogi(GL_FOG_MODE, GL_LINEAR);
//	glEnable(GL_NORMALIZE);
//	glShadeModel(GL_SMOOTH);

	glEnable(GL_POLYGON_OFFSET_FILL);
//	glDisable(GL_DITHER);

	//
	// backface culling
	//

	// set front face direction
	glFrontFace(GL_CCW); // GL_CCW, GL_CW (default GL_CCW)

	// set cull face
	glCullFace(GL_BACK); // GL_FRONT, GL_BACK  (default GL_BACK)

	// do culling ?
	glEnable(GL_CULL_FACE); // glDisable(GL_CULL_FACE);

//	glDisable(GL_LIGHTING);
//
////	glEnableClientState(GL_VERTEX_ARRAY);
//	glGetIntegerv(GL_MAX_TEXTURE_UNITS, (GLint*)&ms_supportMaxTextureUnits);
//
//	for(cjINT32 i=0; i<ms_supportMaxTextureUnits; i++)
//	{
//		glActiveTexture(GL_TEXTURE0 + i);
//		glDisable(GL_TEXTURE_2D);
////		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
//	}
//
//	glActiveTexture(GL_TEXTURE0);
//	glEnable(GL_TEXTURE_2D);

//	cjFLOAT colorOne[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
//	cjFLOAT colorZero[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
//
//	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE , (GLfloat*)colorOne);
//	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT , (GLfloat*)colorOne);
//	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (GLfloat*)colorZero);
//	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, (GLfloat*)colorZero);
//
//	glColorMaterial(GL_FRONT_AND_BACK, GL_EMISSION);
//	glEnable(GL_COLOR_MATERIAL);
//	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	
//	for(cjINT32 i=0; i<8; i++)
//		glLightfv(GL_LIGHT0+i, GL_SPECULAR, colorZero);

	glViewport(0, 0, width, height);

	glEnable(GL_SCISSOR_TEST);
	glScissor(0, 0, width, height);

//	cjMemZero(HardwareState, sizeof(HardwareState));	
//	ms_supportCubemaps = ms_caps_cjGL_ARB_texture_cube_map;
	ms_supportZBias = cjTrue;
	m_devParam.m_glWasFullScreen = fullScreen;

	viewport->SetPendingFrame(cjFalse);

    CreateDefaultShader();

	CreateDefaultRenderTargetGrp();

	// init stuff of renderinterface
	cjAssert(GetDefaultRenTargetGrp());
	cjAssert(!GetCurRenTargetGrp());

	m_renInterface->SetRenTargetGrp(GetDefaultRenTargetGrp());

//    extern void cj_gltestdraw_init(cjGLRenDev* renDev);
//    cj_gltestdraw_init(this);
    
	return cjTrue;
}

void cjGLRenDev::UnSetResolution(cjViewport* viewport)
{
//    extern void cj_gltestdraw_end(cjGLRenDev* renDev);
//    cj_gltestdraw_end(this);
    
	cjAssert(m_renInterface->GetRenTargetGrp() == GetDefaultRenTargetGrp());
	cjAssert(m_renInterface->IsFinished());

	m_renInterface->SetRenTargetGrp(NULL);

    FlushResCache();
    DestroyDefaultShader();
	DestroyDefaultRenderTargetGrp();
}

cjBool cjGLRenDev::TestDeviceStatus()
{
	return cjTrue;
}

cjRenInterface* cjGLRenDev::BeginRender(cjViewport* viewPort)
{
	BindGLContextToThisThread();
    
    m_frameCounter++;
	m_usingRenViewport = viewPort;

	glViewport(0, 0, viewPort->GetWidth(), viewPort->GetHeight());
	glScissor(0, 0, viewPort->GetWidth(), viewPort->GetHeight());

	if(!m_renInterface->Begin(viewPort))
		return NULL;

	return m_renInterface;
}

void cjGLRenDev::EndRender(cjRenInterface* ri)
{
	if(ri)
		ri->End();

	if(m_usingRenViewport)
	{
		m_usingRenViewport = NULL;
	}
}

void cjGLRenDev::Present(cjViewport* viewport)
{
	cjAssert(GetCurRenTargetGrp() == GetDefaultRenTargetGrp());

	//Bind 0, which means render to back buffer, as a result, fb is unbound

#if defined(WIN32)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

//  if(m_devParam.m_glPendingGLCmdFlush)
	glFinish(); // pending gl command buffer is flushed
#endif

#if defined(CJ_PLATFORM_WIN32)

	glFinish(); // pending gl command buffer is flushed
    SwapBuffers(m_wglDC);
#elif defined(CJ_PLATFORM_APPLE)
    m_eaglAdapter->SwapBuffers();
#endif
}


//
// render resouce
//

void cjGLRenDev::DisposeResCache(cjResCacheID cacheID)
{
    cjGLResCache* resCache = FindResCache(cacheID);
    
    if(resCache)
        delete resCache;
}

void cjGLRenDev::FlushResCache()
{
    typedef std::set<GLResCacheContainer*> ContainersSet;
    ContainersSet containers;
    
    for(cjINT32 i=0; i<cjRSCID_Num; i++)
    {
        if(m_resCache[i])
            containers.insert(m_resCache[i]);
    }
    
    ContainersSet::iterator x = containers.begin();
    while(x != containers.end())
    {
        (*x)->RemoveAll();
        x++;
    }
}


//
// Render target operation
//
cjFRenTargetGrp* cjGLRenDev::GetCurRenTargetGrp()
{ 
	return m_renInterface->GetRenTargetGrp();
}

cjFRenTargetGrp* cjGLRenDev::GetDefaultRenTargetGrp()
{
	return m_defaultRenTargetGrp;
}

void cjGLRenDev::SetBackgroundColor(cjColor& color) 
{
    m_colorBackground = color;
}

void cjGLRenDev::SetDepthClear(cjFLOAT depth) 
{
	m_depthClear = depth;
}

void cjGLRenDev::SetStencilClear(cjTFLAG stencil) 
{
	m_stencilClear = stencil;
}

void cjGLRenDev::ClearBuffer(cjTFLAG clearFlag, const cjRect<cjFLOAT>* rect) 
{
	if(clearFlag == cjRenInterface::ECLEAR_NONE)
        return;

//	cjFRenTargetGrp* grp = m_renInterface->GetRenTargetGrp();

	if(!m_renInterface->GetRenTargetGrp())
		return;

	GLbitfield glOperFlag = 0;

	if(clearFlag & cjRenInterface::ECLEAR_COLOR)
	{
		glClearColor(m_colorBackground.r, m_colorBackground.g, m_colorBackground.b, 
			m_colorBackground.a);

		glOperFlag |= GL_COLOR_BUFFER_BIT;
	}

	if(clearFlag & cjRenInterface::ECLEAR_DEPTH)
	{
		glClearDepthf(m_depthClear);
		glOperFlag |= GL_DEPTH_BUFFER_BIT;
	}

	if( (clearFlag & cjRenInterface::ECLEAR_STENCIL) && ms_supportStencil && m_devParam.m_useStencil)
	{
		glClearStencil(m_stencilClear);
		glOperFlag |= GL_STENCIL_BUFFER_BIT;
	}

	glClear(glOperFlag);

}


//
// camera
//

void cjGLRenDev::_DoSetCamData(const cjVec3& eye, const cjVec3& dir, const cjVec3& up, 
	const cjVec3& right, const cjFrustum& frustum, const cjRect<cjFLOAT>& port)
{
}


//
// local
//

void cjGLRenDev::BindGLContextToThisThread()
{
#ifdef CJ_PLATFORM_WIN32
	if(ms_winCurGLCtx != m_wglRenCtx)
	{
		wglMakeCurrent(m_wglDC, m_wglRenCtx);
		ms_winCurGLCtx = m_wglRenCtx;
	}
#else
    if(m_devParam.m_eaglAdapter)
        m_devParam.m_eaglAdapter->SetCurrentContext();
#endif
}

void cjGLRenDev::CleanGLContext()
{
#ifdef CJ_PLATFORM_WIN32

	if(m_wglDC)
	{
		ReleaseDC(m_wglWnd, m_wglDC);
		m_wglDC = NULL;
	}

	if(m_wglRenCtx)
	{
		wglDeleteContext(m_wglRenCtx);
		m_wglRenCtx = NULL;
	}

	if(m_wglWnd)
		m_wglWnd = NULL;

	ms_winCurGLCtx = NULL;

	if(ms_winModuleGL)
	{
		::FreeLibrary(ms_winModuleGL);
		ms_winModuleGL = NULL;
	}

	if(ms_winModuleGDI)
	{
		::FreeLibrary(ms_winModuleGDI);
		ms_winModuleGDI = NULL;
	}

#endif
}

cjBool cjGLRenDev::GetClosestDisplayMode(cjINT32 width, cjINT32 height, cjINT32 colorBPP) 
{ 
	return cjFalse; 
}

void cjGLRenDev::CreateDefaultShader()
{
    // Varying can be used only with the data types cjFLOAT, vec2, vec3, vec4, mat2, mat3, mat4
    /*
        gl_Vertex	Position (vec4)
        gl_Normal	Normal (vec4)
        gl_Color	Primary color of vertex (vec4)
        gl_MultiTexCoord0	Texture coordinate of texture unit 0 (vec4)
        gl_MultiTexCoord1	Texture coordinate of texture unit 1 (vec4)
        gl_MultiTexCoord2	Texture coordinate of texture unit 2 (vec4)
        gl_MultiTexCoord3	Texture coordinate of texture unit 3 (vec4)
        gl_MultiTexCoord4	Texture coordinate of texture unit 4 (vec4)
        gl_MultiTexCoord5	Texture coordinate of texture unit 5 (vec4)
        gl_MultiTexCoord6	
        Texture coordinate of texture unit 6 (vec4)
        gl_MultiTexCoord7	Texture coordinate of texture unit 7 (vec4)
        gl_FogCoord	Fog Coord (cjFLOAT)
    */
    
//    const static char vtxShader[] =
//	{
//        "attribute vec4 position; "
//        "attribute vec3 normal; "
//        "  "
//        "varying lowp vec4 colorVarying; "
//        "  "
//        "uniform mat4 modelViewProjectionMatrix; "
//        "uniform mat3 normalMatrix; "
//        "  "
//        "void main() "
//        "{ "
//        "    vec3 eyeNormal = normalize(normalMatrix * normal); "
//        "    vec3 lightPosition = vec3(0.0, 0.0, 1.0); "
//        "    vec4 diffuseColor = vec4(0.4, 0.4, 1.0, 1.0); "
//        "  "
//        "    cjFLOAT nDotVP = max(0.0, dot(eyeNormal, normalize(lightPosition))); "
//        "  "
//        "    colorVarying = diffuseColor * nDotVP; "
//        "  "
//        "    gl_Position = modelViewProjectionMatrix * position; "
//        "} "
//	};
//
//	const static cjINT32 lenVtxShader = sizeof(vtxShader);
//
//	const static char pxlShader[] = 
//	{
//        "varying lowp vec4 colorVarying;"
//        "void main() "
//        "{ "
//        " gl_FragColor = colorVarying; "
//        "} "
//	};
//    
//	const static cjINT32 lenPxlShader = sizeof(pxlShader);

/*
    variable prefix 'a_'  : attribute in vs (vs shader input soruce)
    variable prefix 'u_'  : uniforms in vs, ps
    variable prefix 'v_'  : varying (interpolated on transfer from vs to ps)
*/
    

//////////////////////////////////////////////////////////////////////////////////////
#ifdef CJ_PLATFORM_WIN32
//////////////////////////////////////////////////////////////////////////////////////

/*
	When an array element i is transferred to the GL by the DrawArrays or 
	DrawElements commands, each generic attribute is expanded to four components. 
	If size is one then the x component of the attribute is specified by the array; 
	the y, z, and w components are implicitly set to zero, zero, and one, respectively. 
	If size is two then the x and y components of the attribute are specified by the array; 
	the z, and w components are implicitly set to zero, and one, respectively. 
	If size is three then x, y, and z are specified, and w is implicitly set to one. 
	If size is four then all components are specified 
*/

	//cjneed  bonematrix4x4 -> 4x3
	//cjneed  indexBuffer optimization
	//cjneed  truncate blend weigt, 2~3 weight blending
	//cjneed  4-bone/blendweight -> 2~3 bone/weight


// stable skinning
//	const static char vtxShader[] =
//	{
//        "attribute vec4 a_position;			\n" // connected from vao
//        "attribute vec3 a_normal;			\n"
//		"attribute vec4 a_color;			\n"
//        "attribute vec2 a_texcoord0;		\n"
//		"attribute vec4 a_blendweight;		\n"
//		"attribute vec4 a_blendindices;		\n"
//
//		"varying vec4 v_color;              \n" // interpolated between vtx,frag shader
//        "varying vec2 v_texcoord;           \n"
//
//		"const int blendMatAryCount = 35;					\n"
//		"uniform vec4 blendMatrixAry[blendMatAryCount*4];	\n"
//        "uniform mat4 modelViewProjectionMatrix;			\n"
//        "uniform mat3 normalMatrix;							\n"
//
//        "void main()									\n"
//        "{ \n"
//        "   vec3 eyeNormal = normalize(normalMatrix * a_normal); \n"
//        "   vec3 lightPosition = vec3(0.0, 0.0, 1.0); \n"
//        "   vec4 diffuseColor = vec4(0.4, 0.4, 1.0, 1.0); \n"
////        "   cjFLOAT nDotVP = max(0.0, dot(eyeNormal, normalize(lightPosition))); \n"
////		"   v_color = diffuseColor * nDotVP + vec4(1,1,1,1);	\n"
//		"   v_color = diffuseColor;	\n"
//  //      "   v_color = a_color;							\n"
//        "   v_texcoord = a_texcoord0;					\n"
//		"   vec3 p = vec4(0,0,0,0);						\n"
//		"   vec3 xp = a_position.xyz;				\n"
//		"   mat3 m; \n"
//		"   vec3 mx; \n"
//		"for(int i=0; i<4; i++)	\n"
//		"{	\n"
//		"  int index = int(a_blendindices[i]);	\n"
//		"  index = index * 4; \n"
//		"  m = mat3(blendMatrixAry[index].xyz, blendMatrixAry[index+1].xyz, blendMatrixAry[index+2].xyz);	\n"
//		"  mx= blendMatrixAry[index+3].xyz; \n"
//		"  p = p + (((xp * m) + mx) * a_blendweight[i]);	\n"
//		"}	\n"	
////		"   gl_Position = vec4(0,0,0,0);				\n"
//		"   gl_Position = modelViewProjectionMatrix * vec4(p, 1.0f); \n"
//        "} \n"
//	};


	/*
	 GLSL

	mat3 m = mat3(
		1.1, 2.1, 3.1, // first column (not row!)
		1.2, 2.2, 3.2, // second column
		1.3, 2.3, 3.3  // third column
	);
	mat3 id = mat3(1.0); // puts 1.0 on the diagonal, all other components are 0.0
	vec3 column0 = vec3(0.0, 1.0, 0.0);
	vec3 column1 = vec3(1.0, 0.0, 0.0);
	vec3 column2 = vec3(0.0, 0.0, 1.0);

	mat3 n = mat3(column0, column1, column2); // sets columns of matrix n
	*/
//	const static char vtxShader[] =
//	{
//        "attribute vec4 a_position;			\n" // connected from vao
//        "attribute vec3 a_normal;			\n"
//		"attribute vec4 a_color;			\n"
//        "attribute vec2 a_texcoord0;		\n"
//		"attribute vec4 a_blendweight;		\n"
//		"attribute vec4 a_blendindices;		\n"
//
//		"varying vec4 v_color;              \n" // interpolated between vtx,frag shader
//        "varying vec2 v_texcoord;           \n"
//
//		"const int blendMatAryCount = 35;					\n"
//		"uniform vec4 blendMatrixAry[blendMatAryCount*4];	\n"
//        "uniform mat4 modelViewProjectionMatrix;			\n"
//        "uniform mat3 normalMatrix;							\n"
//
//        "void main()									\n"
//        "{ \n"
//        "   vec3 eyeNormal = normalize(normalMatrix * a_normal); \n"
//        "   vec3 lightPosition = vec3(0.0, 0.0, 1.0); \n"
//        "   vec4 diffuseColor = vec4(0.4, 0.4, 1.0, 1.0); \n"
////        "   cjFLOAT nDotVP = max(0.0, dot(eyeNormal, normalize(lightPosition))); \n"
////		"   v_color = diffuseColor * nDotVP + vec4(1,1,1,1);	\n"
//		"   v_color = diffuseColor;	\n"
//  //      "   v_color = a_color;							\n"
//        "   v_texcoord = a_texcoord0;					\n"
//		"   vec3 p = vec3(0,0,0);						\n"
//		"   vec3 xp = a_position.xyz;				\n"
//		"   mat3 m; \n"
//		"   vec3 mx; \n"
//		" for(int i=0; i<4; i++)	\n"
//		" {	\n"
//		"  int index = int(a_blendindices[i]);	\n"
//		"  index = index * 4; \n"
//		"  m = mat3(blendMatrixAry[index].xyz, blendMatrixAry[index+1].xyz, blendMatrixAry[index+2].xyz);	\n"
//		"  mx= blendMatrixAry[index+3].xyz; \n"
//		// xp*m => because mat3 is column matrix
//		"  p = p + (((xp*m) + mx) * a_blendweight[i]);	\n"
//		" }	\n"
////		"   gl_Position = vec4(0,0,0,0);				\n"
//		"   gl_Position = modelViewProjectionMatrix * vec4(p, 1.0); \n"
//        "} \n"
//	};


//
//	const static char vtxShader[] =
//	{
//        "attribute vec4 a_position;			\n" // connected from vao
//        "attribute vec3 a_normal;			\n"
//		"attribute vec4 a_color;			\n"
//        "attribute vec2 a_texcoord0;		\n"
//		"attribute vec4 a_blendweight;		\n"
//		"attribute vec4 a_blendindices;		\n"
//
//		"varying vec4 v_color;              \n" // interpolated between vtx,frag shader
//        "varying vec2 v_texcoord;           \n"
//
//		"const int blendMatAryCount = 35;					\n"
//		"uniform vec4 blendMatrixAry[blendMatAryCount*4];	\n"
//        "uniform mat4 modelViewProjectionMatrix;			\n"
//        "uniform mat3 normalMatrix;							\n"
//" \n"
//"vec4 cjSkin4W(in vec3 lpos, in vec4 _blendMatrixAry[], in vec4 blendIndex, in vec4 blendweight) \n"
//"{ \n"
//"	mat3 sr; \n"
//"	vec3 t;  \n"
//"	vec3 p = vec3(0,0,0); \n"
//" \n"
//"	for(int i=0; i<4; i++) \n"
//"	{ \n"
//"		int index = int(blendIndex[i]) * 4; \n"
//" \n"
//"		sr = mat3(_blendMatrixAry[index].xyz, \n"
//"			_blendMatrixAry[index+1].xyz,  \n"
//"			_blendMatrixAry[index+2].xyz ); \n"
//" \n"
//"		t = _blendMatrixAry[index+3].xyz; \n"
//" \n"
//"		p = p + ( ((lpos * sr) + t) * blendweight[i] ); \n"
//"	} \n"
//" \n"
//"	return vec4(p, 1.0); \n"
//"}\n"
//		" \n"
//        "void main() \n"
//		"{ \n"
//		"   v_color = a_color;							\n"
//		"   v_texcoord = a_texcoord0;					\n"
//		"   vec4 p = cjSkin4W(a_position.xyz, blendMatrixAry, a_blendindices, a_blendweight); \n"
//		"	gl_Position = modelViewProjectionMatrix * p; \n"
//		" } \n"
//	};


//	// raw vtx
//	const static char vtxShader[] =
//	{
//        "attribute vec4 a_position;			\n" // connected from vao
//        "attribute vec3 a_normal;			\n"
//		"attribute vec4 a_color;			\n"
//        "attribute vec2 a_texcoord0;		\n"
//		"attribute vec4 a_blendweight;		\n"
//		"attribute vec4 a_blendindices;		\n"
//
//		"varying vec4 v_color;              \n" // interpolated between vtx,frag shader
//        "varying vec2 v_texcoord;           \n"
//
//		"const int blendMatAryCount = 35;					\n"
//		"uniform vec4 blendMatrixAry[blendMatAryCount*4];	\n"
//        "uniform mat4 modelViewProjectionMatrix;			\n"
//        "uniform mat3 normalMatrix;							\n"
//
//        "void main()									\n"
//        "{ \n"
//        "   vec3 eyeNormal = normalize(normalMatrix * a_normal); \n"
//        "   vec3 lightPosition = vec3(0.0, 0.0, 1.0); \n"
//        "   vec4 diffuseColor = vec4(0.4, 0.4, 1.0, 1.0); \n"
////        "   cjFLOAT nDotVP = max(0.0, dot(eyeNormal, normalize(lightPosition))); \n"
////		"   v_color = diffuseColor * nDotVP + vec4(1,1,1,1);	\n"
//		"   v_color = diffuseColor;	\n"
//  //      "   v_color = a_color;							\n"
//        "   v_texcoord = a_texcoord0;					\n"
//		"   vec4 p = a_position;						\n"
////		"   gl_Position = vec4(0,0,0,0);				\n"
//		"   gl_Position = modelViewProjectionMatrix * p; \n"
//        "} \n"
//	};




//	const static char vtxShader[] =
//	{
//        "attribute vec4 a_position;			\n" // connected from vao
//        "attribute vec3 a_normal;			\n"
//		"attribute vec4 a_color;			\n"
//        "attribute vec2 a_texcoord0;		\n"
//		"attribute vec4 a_blendweight;		\n"
//		"attribute vec4 a_blendindices;		\n"
//
//		"varying vec4 v_color;              \n"      // interpolated between vtx,frag shader
//        "varying vec2 v_texcoord;           \n"
//
//		"const int blendMatAryCount = 35;					\n"
//		"uniform vec4 blendMatrixAry[blendMatAryCount*4];	\n"
//        "uniform mat4 modelViewProjectionMatrix;			\n"
//        "uniform mat3 normalMatrix;							\n"
//
//        "void main()									\n"
//        "{ \n"
//        "   vec3 eyeNormal = normalize(normalMatrix * a_normal); \n"
//        "   vec3 lightPosition = vec3(0.0, 0.0, 1.0); \n"
//        "   vec4 diffuseColor = vec4(0.4, 0.4, 1.0, 1.0); \n"
//        "   cjFLOAT nDotVP = max(0.0, dot(eyeNormal, normalize(lightPosition))); \n"
//
////		"   v_color = diffuseColor * nDotVP + vec4(1,1,1,1);	\n"
//		"   v_color = diffuseColor;	\n"
//  //      "   v_color = a_color;							\n"
//        "   v_texcoord = a_texcoord0;					\n"
//		
//		"	\
//		   vec4 p = vec4(0,0,0,0);	\
//		   mat4 m; \
//			\
//		   for(int i=0; i<2; i++)	\
//		   {	\
//			  int index = int(a_blendindices[i]);	\
//			  m = mat4(blendMatrixAry[index], blendMatrixAry[index+1], blendMatrixAry[index+2], blendMatrixAry[index+3]);	\
//			  p = p + ((m * a_position) * a_blendweight[i]);	\
//		   }	\
//		   \
//		   gl_Position = modelViewProjectionMatrix * p;		"		
//
//		//"   mat4 m0 (blendMatrixAry[int(a_blendindices.x)]; "
//
//		//"   vec4 p0 = blendMatrixAry[int(a_blendindices.x)] * a_position;		\n"
//		//"   vec4 p1 = blendMatrixAry[int(a_blendindices.y)] * a_position;		\n"
//		//"   vec4 p2 = blendMatrixAry[int(a_blendindices.z)] * a_position;		\n"
//		//"   vec4 p3 = blendMatrixAry[int(a_blendindices.w)] * a_position;		\n"
//		//"   vec4 p  = p0 * a_blendweight.x + p1 * a_blendweight.y + p2 * a_blendweight.z + p3 * a_blendweight.w;  \n"
//  //      "   gl_Position = modelViewProjectionMatrix * p; \n"
//        "} \n"
//	};

	
//	const static cjINT32 lenVtxShader = sizeof(vtxShader);

	const static char pxlShader[] = 
	{
        "varying vec4 v_color;              \n"
        "varying vec2 v_texcoord;           \n"
        "uniform sampler2D texSamp0;        \n"
        "void main()                        \n"
        "{ \n"
		"	vec4 texColor = texture2D(texSamp0, v_texcoord); \n"
        "   gl_FragColor.rgb = texColor.rgb; \n"
        "   gl_FragColor.a = texColor.a; \n"
//		"   gl_FragColor.rgb = v_color.rgb * texColor.rgb; \n"
//        "   gl_FragColor.a = texColor.a; \n"
        "} \n"
	};
    
	const static cjINT32 lenPxlShader = sizeof(pxlShader);


//////////////////////////////////////////////////////////////////////////////////////
#else
//////////////////////////////////////////////////////////////////////////////////////

 //   const static char vtxShader[] =
	//{
 //       "#ifdef GL_ES                       \n"
 //       "attribute mediump vec4 a_position; \n" // connected from vao
 //       "attribute mediump vec3 a_normal;   \n"
 //       "attribute mediump vec2 a_texcoord0; \n"
	//	"varying lowp vec4 v_color;         \n"      // interpolated between vtx,frag shader
 //       "varying mediump vec2 v_texcoord;   \n"
 //       "#else                              \n"
 //       "attribute vec4 a_position; \n" // connected from vao
 //       "attribute vec3 a_normal;   \n"
 //       "attribute vec2 a_texcoord0; \n"
	//	"varying vec4 v_color;              \n"      // interpolated between vtx,frag shader
 //       "varying vec2 v_texcoord;           \n"
 //       "#endif                             \n"
 //       "uniform mat4 modelViewProjectionMatrix; \n"
 //       "uniform mat3 normalMatrix;              \n"
 //       "void main()                             \n"
 //       "{ \n"
 //       "   vec3 eyeNormal = normalize(normalMatrix * a_normal); \n"
 //       "   vec3 lightPosition = vec3(0.0, 0.0, 1.0); \n"
 //       "   vec4 diffuseColor = vec4(0.4, 0.4, 1.0, 1.0); \n"
 //       "   cjFLOAT nDotVP = max(0.0, dot(eyeNormal, normalize(lightPosition))); \n"
 //       "   v_color = diffuseColor * nDotVP; \n"
 //       "   v_texcoord = a_texcoord0;    \n"
 //       "   gl_Position = modelViewProjectionMatrix * a_position; \n"
 //       "} \n"
	//};

	//const static cjINT32 lenVtxShader = sizeof(vtxShader);

	//const static char pxlShader[] = 
	//{
 //       "#ifdef GL_ES                       \n"
 //       "varying lowp vec4 v_color;         \n"
 //       "varying mediump vec2 v_texcoord;   \n"
 //       "#else                              \n"
 //       "varying vec4 v_color;              \n"
 //       "varying vec2 v_texcoord;           \n"
 //       "#endif                             \n"
 //       "uniform sampler2D texSamp0;        \n"
 //       "void main()                        \n"
 //       "{ \n"
	//	"#ifdef GL_ES \n"
 //       "   mediump vec4 texColor = texture2D(texSamp0, v_texcoord);  \n"
	//	"#else \n"
	//	"	vec4 texColor = texture2D(texSamp0, v_texcoord); \n"
	//	"#endif \n"
 //       "   gl_FragColor.rgb = v_color.rgb * texColor.rgb; \n"
 //       "   gl_FragColor.a = texColor.a; \n"
 //       "} \n"
	//};
 //   
	//const static cjINT32 lenPxlShader = sizeof(pxlShader);

//////////////////////////////////////////////////////////////////////////////////////
//
//
//	const static char vtxShader[] =
//	{
//        "precision mediump float;           \n"
//        "attribute vec4 a_position;			\n" // connected from vao
//        "attribute vec3 a_normal;			\n"
//		"attribute vec4 a_color;			\n"
//        "attribute vec2 a_texcoord0;		\n"
//		"attribute vec4 a_blendweight;		\n"
//		"attribute vec4 a_blendindices;		\n"
//
//		"varying vec4 v_color;              \n" // interpolated between vtx,frag shader
//        "varying vec2 v_texcoord;           \n"
//
//		"const int blendMatAryCount = 35;					\n"
//		"uniform vec4 blendMatrixAry[blendMatAryCount*4];	\n"
//        "uniform mat4 modelViewProjectionMatrix;			\n"
//        "uniform mat3 normalMatrix;							\n"
//
//        "void main()									\n"
//        "{ \n"
//        "   vec3 eyeNormal = normalize(normalMatrix * a_normal); \n"
//        "   vec3 lightPosition = vec3(0.0, 0.0, 1.0); \n"
//        "   vec4 diffuseColor = vec4(0.4, 0.4, 1.0, 1.0); \n"
//		"   v_color = diffuseColor;	\n"
//        "   v_texcoord = a_texcoord0;					\n"
//		"   vec3 p = vec3(0,0,0);						\n"
//		"   vec3 xp = a_position.xyz;				\n"
//		"   mat3 m; \n"
//		"   vec3 mx; \n"
//		"   for(int i=0; i<4; i++)	\n"
//		"   {	\n"
//		"  int index = int(a_blendindices[i]);	\n"
//		"  index = index * 4; \n"
//		"  m = mat3(blendMatrixAry[index].xyz, blendMatrixAry[index+1].xyz, blendMatrixAry[index+2].xyz);	\n"
//		"      mx= blendMatrixAry[index+3].xyz; \n"
//		"      p = p + (((xp*m) + mx) * a_blendweight[i]);	\n"
//		"    }	\n"
//		"   gl_Position = modelViewProjectionMatrix * vec4(p, 1.0); \n"
//        " } \n"
//	};
//
//	const static cjINT32 lenVtxShader = sizeof(vtxShader);
//
//	const static char pxlShader[] = 
//	{
//        "precision mediump float;           \n"
//        "varying vec4 v_color;              \n"
//        "varying vec2 v_texcoord;           \n"
//        "uniform sampler2D texSamp0;        \n"
//        "void main()                        \n"
//        "{ \n"
//		"	vec4 texColor = texture2D(texSamp0, v_texcoord); \n"
//        "   gl_FragColor.rgb = v_color.rgb; \n"
//        "   gl_FragColor.a = v_color.a; \n"
////		"   gl_FragColor.rgb = v_color.rgb * texColor.rgb; \n"
////        "   gl_FragColor.a = texColor.a; \n"
//        "} \n"
//	};
//    
//	const static cjINT32 lenPxlShader = sizeof(pxlShader);

#endif
    
}

void cjGLRenDev::DestroyDefaultShader()
{
}


//
// cjGLResCache
//
//cjGLResCache* cjGLRenDev::AddResCache(cjGLResCache* resCache)
//{
//	if(!resCache)
//		return NULL;
//
//	resCache->m_nextRes = m_resCacheList;
//	m_resCacheList = resCache;
//
//	cjUINT32 hashIndex = cjResGetHashIndex(resCache->m_cacheID);
//
//	resCache->m_hashIndex = hashIndex;
//	resCache->m_hashNext = m_resCacheHash[hashIndex];
//	m_resCacheHash[hashIndex] = this;
//}
//
//cjGLResCache* cjGLRenDev::SequenceFindPrevResCacheBy(cjGLResCache* resCache)
//{
//	if(!resCache || !m_resCacheList)
//		return NULL;
//
//	cjGLResCache* cur = m_resCacheList;
//	cjGLResCache* prev = NULL;
//
//	if(m_resCacheList != resCache)
//	{
//		while(cur && cur->m_nextRes != resCache)
//		{
//			prev = cur;
//			cur = cur->m_nextRes;
//		}
//
//		if(cur != resCache)
//			return NULL;		
//	}
//
//	return prev;
//}
//
//void cjGLRenDev::RemoveResCache(cjGLResCache* resCache)
//{
//	if(!resCache)
//		return;
//
//	cjGLResCache* prev = SequenceFindPrevResCacheBy(resCache);
//	
//	if(prev)
//		prev->m_nextRes = resCache->m_nextRes;
//	else
//	{
//		cjAssert(!m_resCacheList);
//		m_resCacheList = resCache->m_nextRes;
//	}
//
//	// remove from hash list
//	cjUINT32 hashIndex = resCache->m_hashIndex;
//	cjGLResCache* hashNext = resCache->m_hashNext;
//	
//	if(m_resCacheHash[hashIndex] != resCache)
//	{
//		cjGLResCache* curHash = m_resCacheHash[hashIndex];
//		cjGLResCache* prev = NULL;
//
//		while(curHash && curHash->m_hashNext != this)
//		{
//			prev = curHash;
//			curHash = curHash->m_hashNext;			
//		}
//
//		if(curHash == resCache)
//		{
//			if(prev)
//				prev->m_hashNext = hashNext;
//			else
//				m_resCacheHash[hashIndex] = hashNext;
//		}
//	}
//	else
//	{
//		m_resCacheHash[hashIndex] = hashNext;
//	}
//}
//
//cjGLResCache* cjGLRenDev::GetFirstResCache()
//{
//	return m_resCacheList;
//}
//
//cjGLResCache* cjGLRenDev::FindResCache(cjResCacheID cacheID)
//{
//	if(!cacheID)
//		return NULL;
//
//	cjUINT32 hashIndex = cjResGetHashIndex(cacheID);
//	cjGLResCache* cur = m_resCacheHash[hashIndex];
//
//	while(cur)
//	{
//		if(cur->m_cacheID == cacheID)
//			return cur;
//
//		cur = cur->m_hashNext;
//	}
//
//	return NULL;
//}

cjUINT32 cjGLRenDev::GetLastGLErr(cjBool* rstOk, const char* file, cjINT32 line)
{
	if(!cjGLRenDev::glGetError)
		return cjTrue;

	GLenum glErr = cjGLRenDev::glGetError();
	cjBool ret = cjTrue;
	GLenum glLastErrNo = 0;

	while(glErr != GL_NO_ERROR)
	{
#ifdef CJDEBUG
		char buf[cjMAX_PATH] = {0};
		cjSprintf(buf, cjMAX_PATH, "[GLERR] %d (%s-%d) \n",
			glErr, file ? file : "*NoFileInfo*", line);

        cjLOG((cjChar*)buf);
		cjAssert(0);
#endif
		ret = cjFalse;
		glLastErrNo = glErr;

		// next
		glErr = glGetError();
	}

	if(rstOk)
		*rstOk = ret;

	return ret ? 0 : glLastErrNo;
}

void cjGLRenDev::CreateDefaultRenderTargetGrp()
{
	if(m_defaultRenTargetGrp)
	{
		delete m_defaultRenTargetGrp;
		m_defaultRenTargetGrp = NULL;
	}

	cjFRenTargetGrp* defaultRTG = NULL;
	cjFRenTarget* priRenTarget = NULL;

#if defined(CJ_PLATFORM_WIN32)
	cjINT32 width = GetRenDevParam()->GetWidth();
	cjINT32 height = GetRenDevParam()->GetHeight();
	cjRSurfaceFmt colorSurFmt = GetRenDevParam()->GetFormat();
	cjRSurfaceFmt depthStencilFmt = GetRenDevParam()->GetDSFormat();

	defaultRTG = new cjFRenTargetGrp;
    priRenTarget = new cjFRenTarget(width, height, colorSurFmt, depthStencilFmt);
    
#elif defined (CJ_PLATFORM_APPLE)
    defaultRTG = new cjFAppleDefFrmRenTargetGrp();
	priRenTarget = new cjFAppleDefFrmRenTarget();
#else
    #error "needs to implement"
#endif
	   
    defaultRTG->AttachRenTarget(0, priRenTarget);

	m_defaultRenTargetGrp = defaultRTG;
   
    // cache Default render target group
#if defined(CJ_PLATFORM_WIN32)
	{
		cjGLRenTargetGrp* rtgInst = cjGLRenTargetGrp::NewInst();
		rtgInst->Init(this, defaultRTG->GetCacheID());

		rtgInst->Cache(defaultRTG);
	}    
#elif defined (CJ_PLATFORM_APPLE)
	{
		cjEAGLDefFrmRenTargetGrp* rtgInst = cjEAGLDefFrmRenTargetGrp::NewInst();
		rtgInst->Init(this, defaultRTG->GetCacheID());

		rtgInst->Cache(defaultRTG);
	}
#else
	#error "not impl"
#endif
}

void cjGLRenDev::DestroyDefaultRenderTargetGrp()
{
	if(m_defaultRenTargetGrp)
	{
		delete m_defaultRenTargetGrp;
		m_defaultRenTargetGrp = NULL;
	}
}

cjBool cjGLRenDev::BeginUsingDefaultRenTargetGrp(cjTFLAG clearFlag) 
{ 
	return BeginUsingRenTargetGrp(GetDefaultRenTargetGrp(), clearFlag);
}

cjBool cjGLRenDev::BeginUsingRenTargetGrp(cjFRenTargetGrp* renTargetGrp, cjTFLAG clearFlag)
{ 
	if(!renTargetGrp)
	{
//		cjAssert(0 && "[ERR!] bad parameter for renTargetGrp..");
		return cjFalse;
	}

	cjGLRenTargetGrp* rtgGL = (cjGLRenTargetGrp*)FindResCache(renTargetGrp->GetCacheID());

	if(!rtgGL)
	{
		rtgGL = cjGLRenTargetGrp::NewInst();
		rtgGL->Init(this, renTargetGrp->GetCacheID());
	}

	if(renTargetGrp->GetCacheID() != rtgGL->GetCacheID())
		rtgGL->Cache(renTargetGrp);

	if(!rtgGL->BeginRenTargetGrp())
	{
		cjLOG("[ERR!] Failed to begin render target group!..");
		return cjFalse;
	}

	m_curRenTargetGrp = renTargetGrp;

	return cjTrue;
}

void cjGLRenDev::EndUsingRenTargetGrp() 
{
	if(!m_curRenTargetGrp)
		return;

	cjGLRenTargetGrp* renderTaretGrpGL 
		= (cjGLRenTargetGrp*)FindResCache(m_curRenTargetGrp->GetCacheID());

	if(renderTaretGrpGL)
		renderTaretGrpGL->EndRenTargetGrp();

	m_curRenTargetGrp = NULL;
}

cjBool cjGLRenDev::CheckGLErrStatus(GLhandle handle, GLenum statusType, const cjUni* func, cjUINT32 line)
{	
	cjUni2Char u2c(func);
	return CheckGLErrStatus(handle, statusType, (const cjChar*)u2c, line);
}

cjBool cjGLRenDev::CheckGLErrStatus(GLhandle handle, GLenum statusType, const cjChar* func, cjUINT32 line)
{
	GLint rst = GL_FALSE;
    
    typedef void (cjSTDCALL *StatusGetterPFN)(GLuint, GLenum, GLint*);
    typedef void (cjSTDCALL *StatusInfoLogPFN)(GLuint, GLsizei, GLsizei*, GLchar*);
    
    StatusGetterPFN statusGetter = NULL;
    StatusInfoLogPFN statusInfoLog = NULL;
    
    switch(statusType)
    {
    case GL_COMPILE_STATUS: statusGetter = (StatusGetterPFN)glGetShaderiv;  statusInfoLog = (StatusInfoLogPFN)glGetShaderInfoLog; break;
    case GL_LINK_STATUS:    statusGetter = (StatusGetterPFN)glGetProgramiv; statusInfoLog = (StatusInfoLogPFN)glGetProgramInfoLog; break;
    default:
        cjAssert(!"Bad status type!");
        return cjFalse;
    }
    
    statusGetter(handle, statusType, &rst);

	if(!rst)
	{
		GLint infoLogLen = 0;
        
		statusGetter(handle, GL_INFO_LOG_LENGTH, &infoLogLen);

		if(infoLogLen)
		{
			cjStringA errMsg;
			errMsg.SetStrSiz(infoLogLen+1);
            
			statusInfoLog(handle, infoLogLen, NULL, &errMsg[0]);
			cjLOG("[ERR!] GL Err! desc=%s , file=%s,line=%d", *errMsg, func ? func : "none", line);
		}
	}
	
	return rst ? cjTrue : cjFalse;
}

cjBool cjGLRenDev::CheckGLFrameBufferStatus()
{
	GLint frameBufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if(frameBufferStatus == GL_FRAMEBUFFER_COMPLETE)
		return cjTrue;

#ifdef CJDEBUG

	switch(frameBufferStatus)
	{
#ifdef CJ_PLATFORM_WIN32
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT           : cjLOG("ERR! -GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT"); break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT	: cjLOG("ERR! -GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT"); break;
	case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT : cjLOG("ERR! -GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT"); break;
	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT			: cjLOG("ERR! -GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT"); break;
	case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT				: cjLOG("ERR! -GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT"); break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT			: cjLOG("ERR! -GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT"); break;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT			: cjLOG("ERR! -GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT"); break;
	case GL_FRAMEBUFFER_UNSUPPORTED_EXT						: cjLOG("ERR! -GL_FRAMEBUFFER_UNSUPPORTED_EXT"); break;
#elif defined CJ_PLATFORM_APPLE
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT               : cjLOG("ERR! -GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT"); break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT       : cjLOG("ERR! -GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT"); break;
	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS               : cjLOG("ERR! -GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT"); break;
	case GL_FRAMEBUFFER_UNSUPPORTED                         : cjLOG("ERR! -GL_FRAMEBUFFER_UNSUPPORTED_EXT"); break;
#else
    #error "not impl!"
#endif
    
	default:  cjLOG("ERR! - Unknown framebuffer error status.."); break;
		break;
	}

#endif

	cjAssert(0);

	return cjFalse;
}


void cjGLRenDev::TestDraw()
{
//    cjFRenTargetGrp* grp = GetDefaultRenTargetGrp();
//    
//    if(!grp)
//        return;
//    
//    cjFRenTarget* rtg = grp->GetRenTarget(0);
//    
//    if(!rtg)
//        return;
//
//    extern void cj_gltestdraw_draw(cjGLRenDev* renDev, cjFLOAT width, cjFLOAT height);
//    cj_gltestdraw_draw(this, rtg->GetWidth(), rtg->GetHeight());
}

//cjGLVertexStream* cjGLRenDev::GenerateVertexStream(cjGLRenDev* renDev, cjResCacheID cacheID)
//{
//#if defined(CJ_PLATFORM_APPLE)
//      return new cjEAGLVertexStream(renDev, cacheID);
//#else
//      return new cjGLVertexStream(renDev, cacheID);
//#endif
//}

void cjGLRenDev::AddResCache(cjGLResCache* resCache)
{
    cjResCacheIDType type = cjGetResCacheType(resCache->GetCacheID());
    GLResCacheContainer* cntr = m_resCache[type];
    
    cntr->Add(resCache);
}

void cjGLRenDev::RemoveResCache(cjGLResCache* resCache)
{
    cjResCacheIDType type = cjGetResCacheType(resCache->GetCacheID());
    GLResCacheContainer* cntr = m_resCache[type];

    if(cntr)
        cntr->Remove(resCache);
}

cjGLResCache* cjGLRenDev::FindResCache(cjResCacheID cacheID)
{
    cjResCacheIDType type = cjGetResCacheType(cacheID);
    GLResCacheContainer* cntr = m_resCache[type];
 
    return cntr ? cntr->Find(cacheID) : NULL;
}

void cjGLRenDev::CreateResCacheContainers()
{
    for(cjINT32 i=0; i<cjRSCID_Num; i++)
    {
        cjAssert(!m_resCache[i]);
    }

    m_resCache[cjRSCID_RenderTarget             ] = new GLResCacheContainer(1024);
    m_resCache[cjRSCID_Texture                  ] = new GLResCacheContainer(2048);
    m_resCache[cjRSCID_Shader                   ] = new GLResCacheContainer(512);
    m_resCache[cjRSCID_VertexStreamAttrib       ] = new GLResCacheContainer(1024);
    m_resCache[cjRSCID_VertexStream             ] = new GLResCacheContainer(1024);
}

void cjGLRenDev::DestroyResCacheContainers()
{
    typedef std::set<GLResCacheContainer*> ContainersSet;
    ContainersSet containers;
    
    for(cjINT32 i=0; i<cjRSCID_Num; i++)
    {
        if(m_resCache[i])
            containers.insert(m_resCache[i]);
    }
    
    ContainersSet::iterator x = containers.begin();
    while(x != containers.end())
    {
        (*x)->RemoveAll();
        delete (*x);
        x++;
    }

	cjMemZero(m_resCache, sizeof(m_resCache));
}


CJENGINE_API cjGLResCache* cjResolveGLRenRes(const cjRTTI* rtti, cjGLRenDev* renDev, 
	cjFRenResSrc* renRes, cjBool doNotCreateCacheWhenNotExist)
{
    cjResCacheID cacheID = renRes->GetCacheID();
    cjGLResCache* renResGL = renDev->FindResCache(cacheID);

#if defined(CJDEBUG)
	if(renResGL)
	{
		cjAssert(renResGL->cjsIsKindOf(rtti));
	}
#endif
    
    if(!renResGL && !doNotCreateCacheWhenNotExist)
        renResGL = cjGLResCache::CreateInstance(rtti, renDev, renRes);
    
    if(renResGL && renResGL->GetCacheRev() != renRes->GetCacheRev())
        renResGL->Cache(renRes);
    
    return renResGL;
}



////	const static char vtxShader[] =
////	{
////        "attribute vec4 a_position;			\n" // connected from vao
////        "attribute vec3 a_normal;			\n"
////		"attribute vec4 a_color;			\n"
////        "attribute vec2 a_texcoord0;		\n"
////		"attribute vec4 a_blendweight;		\n"
////		"attribute vec4 a_blendindices;		\n"
////
////		"varying vec4 v_color;              \n"      // interpolated between vtx,frag shader
////        "varying vec2 v_texcoord;           \n"
////
////		"const int blendMatAryCount = 35;					\n"
////		"uniform vec4 blendMatrixAry[blendMatAryCount*4];	\n"
////        "uniform mat4 modelViewProjectionMatrix;			\n"
////        "uniform mat3 normalMatrix;							\n"
////
////        "void main()									\n"
////        "{ \n"
////        "   vec3 eyeNormal = normalize(normalMatrix * a_normal); \n"
////        "   vec3 lightPosition = vec3(0.0, 0.0, 1.0); \n"
////        "   vec4 diffuseColor = vec4(0.4, 0.4, 1.0, 1.0); \n"
////        "   cjFLOAT nDotVP = max(0.0, dot(eyeNormal, normalize(lightPosition))); \n"
////
//////		"   v_color = diffuseColor * nDotVP + vec4(1,1,1,1);	\n"
////		"   v_color = diffuseColor;	\n"
////  //      "   v_color = a_color;							\n"
////        "   v_texcoord = a_texcoord0;					\n"
////		
////		"	\
////		   vec4 p = vec4(0,0,0,0);	\
////		   mat4 m; \
////			\
////		   for(int i=0; i<4; i++)	\
////		   {	\
////			  int index = int(a_blendindices[i]);	\
////			  m = mat4(blendMatrixAry[index], blendMatrixAry[index+1], blendMatrixAry[index+2], blendMatrixAry[index+3]);	\
////			  p = p + ((m * a_position) * a_blendweight[i]);	\
////		   }	\
////		   \
////		   gl_Position = modelViewProjectionMatrix * p;		"		
////
////		//"   mat4 m0 (blendMatrixAry[int(a_blendindices.x)]; "
////
////		//"   vec4 p0 = blendMatrixAry[int(a_blendindices.x)] * a_position;		\n"
////		//"   vec4 p1 = blendMatrixAry[int(a_blendindices.y)] * a_position;		\n"
////		//"   vec4 p2 = blendMatrixAry[int(a_blendindices.z)] * a_position;		\n"
////		//"   vec4 p3 = blendMatrixAry[int(a_blendindices.w)] * a_position;		\n"
////		//"   vec4 p  = p0 * a_blendweight.x + p1 * a_blendweight.y + p2 * a_blendweight.z + p3 * a_blendweight.w;  \n"
////  //      "   gl_Position = modelViewProjectionMatrix * p; \n"
////        "} \n"
////	};
//
//
//		//"uniform mat4 blendMat[4] = mat4[] ( \
//		//	mat4(0.011362, -0.999936, -0.000003, 0.000000,						\
//		//		 -0.000000, 0.000003, -1.000000, 0.000000,						\
//		//		  0.999936, 0.011362, -0.000000, 0.000000,				\
//		//		  -0.087007, 0.000000, -4.031322, 1.000000		),		\
//		//		 \
//		//	mat4(0.999883, 0.015297, 0.000000, 0.000000,		\
//		//		 -0.015297, 0.999883, -0.000003, 0.000000,		\
//		//		 -0.000000, 0.000003, 1.000000, 0.000000, 		\
//		//		  5.104737, -0.000001, 0.000000, 1.000000       ),		\
//		//		 \
//		//	mat4(1.000000, -0.000000, 0.000000, 0.000000,		\
//		//		 0.000000,  1.000000, 0.000000,	0.000000,		\
//		//		 0.000000,	0.000000, 1.000000,	0.000000,		\
//		//		 4.351894,  0.000002, 0.000000, 1.000000 ),		\
//		//		 \
//		//	mat4(1.000000, 0.000000, 0.000000, 0.000000,		\
//		//		 0.000000, 1.000000, 0.000000, 0.000000,		\
//		//		 0.000000, 0.000000, 1.000000, 0.000000,		\
//		//		 0.000000, 0.000000, 0.000000, 1.000000		));	"
//
//	
//
//
//		//"uniform mat4 blendMat[4] = mat4[] ( \
//		//	mat4(0.011362, -0.999936, -0.000003, 0.000000,						\
//		//		 -0.000000, 0.000003, -1.000000, 0.000000,						\
//		//		  0.999936, 0.011362, -0.000000, 0.000000,				\
//		//		  -0.087007, 0.000000, -4.031322, 1.000000		),		\
//		//		 \
//		//	mat4(0.026656937	, -0.99964470	 ,  1.6914100e-008, 0,	\
//		//		 -6.1300248e-008, -7.7341383e-008, -1.0000001, 		0,	\
//		//		 0.99964470     ,  0.026656967   , -1.2430199e-007,	0,	\
//		//		 5.0177407      , -0.0013435383  , -4.0313220     ,	1 ),\
//		//		 \
//		//	mat4(0.026656937     ,  -0.99964470    ,  1.6914100e-008,     0,	\
//		//		 -6.1300248e-008 , -7.7341383e-008 ,  -1.0000001    ,     0,	\
//		//		 0.99964470      , 0.026656967     ,  -1.2430199e-007 ,   0,	\
//		//		 9.3696346       , -0.0013413106   ,  -4.0313220      ,   1 ),	\
//		//		 \
//		//	mat4(0.026656937     ,  -0.99964470     , 1.6914100e-008 ,     0,	\
//		//		 -6.1300248e-008 ,  -7.7341383e-008 , -1.0000001     ,     0,	\
//		//		 0.99964470      ,  0.026656967     , -1.2430199e-007,     0,	\
//		//		 9.3696346       ,  -0.0013413106   , -4.0313220     ,     1 )); "
//
//
//
//
//
//
//	// vec4 v_t = vector * matrix_row_major
//	// vec4 v_t = matrix_column_major * vector
//
///*
//	When an array element i is transferred to the GL by the DrawArrays or 
//	DrawElements commands, each generic attribute is expanded to four components. 
//	If size is one then the x component of the attribute is specified by the array; 
//	the y, z, and w components are implicitly set to zero, zero, and one, respectively. 
//	If size is two then the x and y components of the attribute are specified by the array; 
//	the z, and w components are implicitly set to zero, and one, respectively. 
//	If size is three then x, y, and z are specified, and w is implicitly set to one. 
//	If size is four then all components are specified 
//*/
//
//	const static char vtxShader[] =
//	{
//        "attribute vec4 a_position;			\n" // connected from vao
//        "attribute vec3 a_normal;			\n"
//		"attribute vec4 a_color;			\n"
//        "attribute vec2 a_texcoord0;		\n"
//		"attribute vec4 a_blendweight;		\n"
//		"attribute vec4 a_blendindices;		\n"
//
//		"varying vec4 v_color;              \n"      // interpolated between vtx,frag shader
//        "varying vec2 v_texcoord;           \n"
//
//		"const int blendMatAryCount = 35;					\n"
//		"uniform vec4 blendMatrixAry[blendMatAryCount*4];	\n"
//		"uniform vec4 blendWeightAry[4] = vec4[] ( \
//			vec4(0.93227315, 0.067726813, 5.9604645e-008, 0),	\
//			vec4(0.62057322, 0.37942678,  0, 0),	\
//			vec4(0.97658783, 0.023412118, 0, 0),	\
//			vec4(0.55556208, 0.44443795,  0, 0) );  "
//
//		//"uniform mat4 blendMat[4] = mat4[] ( \
//		//	mat4(1, 0,  0,  0,						\
//		//		 0, 1,  0,  0,						\
//		//		 0, 0,  1,  0,				\
//		//		 0, -5.118, -4.944,  1),		\
//		//		 \
//		//	mat4(1, 0,  0, 0,		\
//		//		 0, 1,  0, 0, 		\
//		//		 0, 0,  1, 0, 		\
//		//		 5.1025, -0.0919, -5.0023, 1),		\
//		//		 \
//		//	mat4(1.000000, -0.000000, 0.000000, 0.000000,		\
//		//		 0.000000,  1.000000, 0.000000, 0.000000,		\
//		//		 0.000000,	0.000000, 1.000000,	0.000000,		\
//		//		 9.4513,    4.2583,   -5.1183,        1 ),		\
//		//		 \
//		//	mat4(1.000000, 0.000000, 0.000000, 0.000000,		\
//		//		 0.000000, 0.000000, 1.000000, 0.000000,		\
//		//		 0.000000, -1.000000, 0.000000, 0.000000,		\
//		//		 0.000000, 1.000000, 0.000000, 1.000000		));	"
//		
//		//  y,z axis의  y,z값이 바뀜 ,포지션은 유지
//		"uniform mat4 blendMat[4] = mat4[] ( \
//			mat4(1, 0,   0,  0,				\
//				 0, -1,  0,  0,				\
//				 0, 0,   1,  0,				\
//				 0, 0,   1,  1),			\
//				 \
//			mat4(1, 0,   0, 0,		\
//				 0, -1,  0, 0, 		\
//				 0, 0,   1, 0, 		\
//				 0, 0,   1, 1),		\
//				 \
//			mat4(1, 0,   0,  0,		\
//				 0, -1,  0,  0,		\
//				 0,	0,   1,	 0,		\
//				 0, 0,   1,  1),	\
//				 \
//			mat4(1.000000, 0.000000, 0.000000, 0.000000,		\
//				 0.000000, 0.000000, 1.000000, 0.000000,		\
//				 0.000000, -1.000000, 0.000000, 0.000000,		\
//				 0.000000, 1.000000, 0.000000, 1.000000		));	"
//
//		//"uniform mat4 blendMat[4] = mat4[] ( \
//		//	mat4(1, 0,  0,  0,						\
//		//		 0, 1,  0,  0,						\
//		//		 0, 0,  1,  0,				\
//		//		 0, -5.118, -4.944,  1),		\
//		//		 \
//		//	mat4(1, 0,  0, 0,		\
//		//		 0, 1,  0, 0, 		\
//		//		 0, 0,  1, 0, 		\
//		//		 5.1025, -5.210299, -9.946663, 1),		\
//		//		 \
//		//	mat4(1.000000, -0.000000, 0.000000, 0.000000,		\
//		//		 0.000000,  1.000000, 0.000000, 0.000000,		\
//		//		 0.000000,	0.000000, 1.000000,	0.000000,		\
//		//		 14.553989 , -0.951929, -15.064974,        1 ),		\
//		//		 \
//		//	mat4(1.000000, 0.000000, 0.000000, 0.000000,		\
//		//		 0.000000, 0.000000, 1.000000, 0.000000,		\
//		//		 0.000000, -1.000000, 0.000000, 0.000000,		\
//		//		 14.553989, 16.064972 , -0.951930 , 1.000000		));	"
//
//
//
//        "uniform mat4 modelViewProjectionMatrix;			\n"
//        "uniform mat3 normalMatrix;							\n"
//		"uniform mat4 mx = mat4( 1,0,0,0, 0,1,0,0, 0,0,1,0, 10,0,0,1 );"
//
//        "void main()									\n"
//        "{ \n"
//        "   vec3 eyeNormal = normalize(normalMatrix * a_normal); \n"
//        "   vec3 lightPosition = vec3(0.0, 0.0, 1.0); \n"
//        "   vec4 diffuseColor = vec4(0.4, 0.4, 1.0, 1.0); \n"
//        "   cjFLOAT nDotVP = max(0.0, dot(eyeNormal, normalize(lightPosition))); \n"
//
////		"   v_color = diffuseColor * nDotVP + vec4(1,1,1,1);	\n"
//		"   v_color = diffuseColor;	\n"
//  //      "   v_color = a_color;							\n"
//        "   v_texcoord = a_texcoord0;					\n"
//		
//		"   vec4 p = vec4(0,0,0,0);	"
//		"   mat4 m; "
//
//		"for(int i=0; i<2; i++)	"
//		"{	"
//		"  int index = int(a_blendindices[i]);	"
//		"  index = index * 4; "
//		"  m = mat4(blendMatrixAry[index], blendMatrixAry[index+1], blendMatrixAry[index+2], blendMatrixAry[index+3]);	"
////		"	  m = blendMat[index]; "
////		"		m = mat4(vec4(1,0,0,0), vec4(0,1,0,0), vec4(0,0,1,0) , vec4(0,0,0,1));	"
//		"	  p = p + ((m * a_position) * a_blendweight[i]);	"
////		"	  p = ((m * a_position));	"
//		"  }	"	
////		"  p = a_position; "
//		"   gl_Position = modelViewProjectionMatrix * p;"
//
//		//"   mat4 m0 (blendMatrixAry[int(a_blendindices.x)]; "
//
//		//"   vec4 p0 = blendMatrixAry[int(a_blendindices.x)] * a_position;		\n"
//		//"   vec4 p1 = blendMatrixAry[int(a_blendindices.y)] * a_position;		\n"
//		//"   vec4 p2 = blendMatrixAry[int(a_blendindices.z)] * a_position;		\n"
//		//"   vec4 p3 = blendMatrixAry[int(a_blendindices.w)] * a_position;		\n"
//		//"   vec4 p  = p0 * a_blendweight.x + p1 * a_blendweight.y + p2 * a_blendweight.z + p3 * a_blendweight.w;  \n"
//  //      "   gl_Position = modelViewProjectionMatrix * p; \n"
//        "} \n"
//	};
//
//
//
////	const static char vtxShader[] =
////	{
////        "attribute vec4 a_position;			\n" // connected from vao
////        "attribute vec3 a_normal;			\n"
////		"attribute vec4 a_color;			\n"
////        "attribute vec2 a_texcoord0;		\n"
////		"attribute vec4 a_blendweight;		\n"
////		"attribute vec4 a_blendindices;		\n"
////
////		"varying vec4 v_color;              \n"      // interpolated between vtx,frag shader
////        "varying vec2 v_texcoord;           \n"
////
////		"const int blendMatAryCount = 35;					\n"
////		"uniform vec4 blendMatrixAry[blendMatAryCount*4];	\n"
////        "uniform mat4 modelViewProjectionMatrix;			\n"
////        "uniform mat3 normalMatrix;							\n"
////
////        "void main()									\n"
////        "{ \n"
////        "   vec3 eyeNormal = normalize(normalMatrix * a_normal); \n"
////        "   vec3 lightPosition = vec3(0.0, 0.0, 1.0); \n"
////        "   vec4 diffuseColor = vec4(0.4, 0.4, 1.0, 1.0); \n"
////        "   cjFLOAT nDotVP = max(0.0, dot(eyeNormal, normalize(lightPosition))); \n"
////
//////		"   v_color = diffuseColor * nDotVP + vec4(1,1,1,1);	\n"
////		"   v_color = diffuseColor;	\n"
////  //      "   v_color = a_color;							\n"
////        "   v_texcoord = a_texcoord0;					\n"
////		
////		"	\
////		   vec4 p = vec4(0,0,0,0);	\
////		   mat4 m; \
////			\
////		   for(int i=0; i<2; i++)	\
////		   {	\
////			  int index = int(a_blendindices[i]);	\
////			  m = mat4(blendMatrixAry[index], blendMatrixAry[index+1], blendMatrixAry[index+2], blendMatrixAry[index+3]);	\
////			  p = p + ((m * a_position) * a_blendweight[i]);	\
////		   }	\
////		   \
////		   gl_Position = modelViewProjectionMatrix * p;		"		
////
////		//"   mat4 m0 (blendMatrixAry[int(a_blendindices.x)]; "
////
////		//"   vec4 p0 = blendMatrixAry[int(a_blendindices.x)] * a_position;		\n"
////		//"   vec4 p1 = blendMatrixAry[int(a_blendindices.y)] * a_position;		\n"
////		//"   vec4 p2 = blendMatrixAry[int(a_blendindices.z)] * a_position;		\n"
////		//"   vec4 p3 = blendMatrixAry[int(a_blendindices.w)] * a_position;		\n"
////		//"   vec4 p  = p0 * a_blendweight.x + p1 * a_blendweight.y + p2 * a_blendweight.z + p3 * a_blendweight.w;  \n"
////  //      "   gl_Position = modelViewProjectionMatrix * p; \n"
////        "} \n"
////	};