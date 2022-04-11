
#include "cjEnginePCH.h"

#include "cjGLRenRes.h"
#include "cjGLRenDev.h"


//
// Declare Caps, function pointer for OpenGL
//

#if defined(CJ_PLATFORM_WIN32)

#define CJGLCAPS(name) cjBool cjGLRenDev::ms_caps_##name = cjFalse;
#define CJGLPROC(funccate,ret,glfunc,glfuncreal,prm) ret (cjSTDCALL *cjGLRenDev::glfunc)prm;
#define CJGLEXTPROC(funccate,ret,glfunc,glfuncreal,prm) ret (cjSTDCALL *cjGLRenDev::glfunc)prm;
#include "cjGLFuncs.h"

#elif defined(CJ_PLATFORM_APPLE)

#define CJGLCAPS(name) cjBool cjGLRenDev::ms_caps_##name = cjFalse;
#define CJGLPROC(funccate,ret,glfunc,glfuncreal,prm) ret (*cjGLRenDev::glfunc)prm;
#define CJGLEXTPROC(funccate,ret,glfunc,glfuncreal,prm) ret (*cjGLRenDev::glfunc)prm;

#include "engine/apple/cjIOSGLFuncs.h"


void _nullfunc_glPolygonMode(GLenum face, GLenum mode)
{
    face = mode; // just to avoid compiler warning
}

#endif

#undef CJGLPROC
#undef CJGLCAPS
#undef CJGLEXTPROC

cjBool cjGLRenDev::ms_wasGLProclookup = cjFalse;

cjBool cjGLRenDev::_FindGLExtension(const cjChar* name)
{
	cjBool found = cjStrstr((cjChar*)cjGLRenDev::glGetString(GL_EXTENSIONS), name) != NULL;
	if(!found)
		cjTrace(_CT("[GL][ERR!] GL Extension - does not support: %s"), name);

	return found;
}

cjBool cjGLRenDev::LookupGLProcedure()
{
    if(ms_wasGLProclookup)
        return cjTrue;

	ms_caps_cjGL = cjTrue;

#if defined(CJ_PLATFORM_WIN32)
    
#define CJGLCAPS(name) ms_caps_##name = _FindGLExtension((#name)+2);
#define CJGLPROC(funccate,ret,glfunc,glfuncreal,prm) _BindGLProcedure(*(void**)&glfunc, #glfuncreal, #funccate);
#define CJGLEXTPROC(funccate,ret,glfunc,glfuncreal,prm) \
	//if(lookupExtension) _BindGLProcedure(*(void**)&glfunc, #glfuncreal, #funccate, ms_caps_##funccate);
#include "cjGLFuncs.h"

#elif defined(CJ_PLATFORM_APPLE)
    
#define CJGLCAPS(name)
#define CJGLPROC(funccate,ret,glfunc,glfuncreal,prm) glfunc=::glfuncreal;
#include "engine/apple/cjIOSGLFuncs.h"

#endif
    
#undef CJGLPROC
#undef CJGLCAPS
#undef CJGLEXTPROC

    ms_wasGLProclookup = cjTrue;

	return cjTrue;
}

#if defined(CJ_PLATFORM_WIN32)

cjBool cjGLRenDev::_BindGLProcedure(void*& procAddr, const char* name, const char* funcCate)
{
	// Do not change calling function orders..
	// or it causes message "Debug String: Detected error: 
	// The debugged process asked for an extension function pointer (glGenFramebuffersEXT) 
	// from one render context, but called this function pointer in another render context (context #1) " 
	// at gDebugger

	// 2016.7.13 order #1 cause that glBindTexture which is taken from "::wglGetProcAddress" was not working
	// may be caused from opengl of win sdk 7.0a ?
	// 1st - ::wglGetProcAddress
	// 2nd - ::cjGetDllProcAddr
	
	// so i changed order 1. cjGetDllProcAddr , 2. wglGetProcAddr


	if(!procAddr)
		procAddr = cjGetDllProcAddr(ms_winModuleGL, name); // find in opengl32.dll

	if(!procAddr)
		procAddr = ::wglGetProcAddress(name);


	//// 3rd find in gdi32.dll
	//if(!procAddr)
	//	procAddr = cjGetDllProcAddr(ms_winModuleGDI, name);

	if(!procAddr)
	{
//		if(isSupport)
			cjLOG("[GL][WARN!] - unsupported  gl function (name=%s, cate=%s) ", name, funcCate);

//		isSupport = cjFalse;
		return cjFalse;
	}

	return cjTrue;
}

#endif


#if defined(CJ_PLATFORM_APPLE)

cjBool cjGLRenDev::_BindGLProcedureEAGL(void*& procAddr, const char* name, const char* funcCate)
{
//    procAddr = cjAppleGetProcAddress(name);
//    
//   	if(!procAddr)
//	{
//        cjTrace(_CT("[EAGL][WARN!] - unsupported  gl function (name=%s, cate=%s) "), name, funcCate);
//		return cjFalse;
//	}
 
    return cjTrue;
}

#endif


const char* cjGLRenDev::GetGLFrameBufferStatusText(GLenum frameBufferStatus)
{
	switch(frameBufferStatus)
	{
	case	0x0506	: return 		"	GL_INVALID_FRAMEBUFFER_OPERATION_EXT	";
	case	0x84E8	: return 		"	GL_MAX_RENDERBUFFER_SIZE_EXT	";
	case	0x8CA6	: return 		"	GL_FRAMEBUFFER_BINDING_EXT	";
	case	0x8CA7	: return 		"	GL_RENDERBUFFER_BINDING_EXT	";
	case	0x8CD0	: return 		"	GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT	";
	case	0x8CD1	: return 		"	GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT	";
	case	0x8CD2	: return 		"	GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_EXT	";
	case	0x8CD3	: return 		"	GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_EXT	";
	case	0x8CD4	: return 		"	GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT	";
	case	0x8CD5	: return 		"	GL_FRAMEBUFFER_COMPLETE_EXT	";
	case	0x8CD6	: return 		"	GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT	";
	case	0x8CD7	: return 		"	GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT	";
	case	0x8CD8	: return 		"	GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT	";
	case	0x8CD9	: return 		"	GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT	";
	case	0x8CDA	: return 		"	GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT	";
	case	0x8CDB	: return 		"	GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT	";
	case	0x8CDC	: return 		"	GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT	";
	case	0x8CDD	: return 		"	GL_FRAMEBUFFER_UNSUPPORTED_EXT	";
	default:
		break;
	}

	return "GL_UNKNOWN_ERROR";
}