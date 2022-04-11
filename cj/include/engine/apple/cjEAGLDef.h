
#ifndef _CJEAGLDEF_H_
#define _CJEAGLDEF_H_


#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>


#define cjGL_WRITE_ONLY         GL_WRITE_ONLY_OES


enum cjEAGL_VER
{
   cjEAGL_VER_NONE,
   cjEAGL_VER_10,               // kEAGLRenderingAPIOpenGLES1
   cjEAGL_VER_20,               // kEAGLRenderingAPIOpenGLES2
   cjEAGL_VER_30                // kEAGLRenderingAPIOpenGLES3
};
    
enum cjEAGL_COLOR_FMT
{
    cjEAGL_COLOR_FMT_NONE,
    cjEAGL_COLOR_FMT_RGB565,    // kEAGLColorFormatRGB565
    cjEAGL_COLOR_FMT_RGBA8,     // kEAGLColorFormatRGBA8
    cjEAGL_COLOR_FMT_SRGBA8,    // kEAGLColorFormatSRGBA8
};
    
enum cjEAGL_DEPTH
{
    cjEAGL_DEPTH_NONE,
    cjEAGL_DEPTH_16,            // GL_DEPTH_COMPONENT16
    cjEAGL_DEPTH_24,            // GL_DEPTH_COMPONENT24_OES
    cjEAGL_DEPTH_24_S8          // GL_DEPTH24_STENCIL8_OES
};


inline cjRSurfaceFmt cjConvertEAGLColorFmtToRSurfaceFmt(cjEAGL_COLOR_FMT fmtColor)
{
    switch(fmtColor)
    {
        case cjEAGL_COLOR_FMT_RGB565: return cjRSurf_RGB565;
        case cjEAGL_COLOR_FMT_RGBA8:  return cjRSurf_RGBA8;
        case cjEAGL_COLOR_FMT_SRGBA8: return cjRSurf_RGBA8;
        default:
            break;
    }
    
    return cjRSurf_UNKNOWN;
}

inline cjRSurfaceFmt cjConvertEAGLDepthStencilFmtToRSurfaceFmt(cjEAGL_DEPTH fmtDetphStencil)
{
    switch(fmtDetphStencil)
    {
        case cjEAGL_DEPTH_16       : return cjRSurf_D16;
        case cjEAGL_DEPTH_24       : return cjRSurf_D24X8;
        case cjEAGL_DEPTH_24_S8    : return cjRSurf_D24S8;
        default:
            break;
    }
    
    return cjRSurf_UNKNOWN;
}

inline cjRSurfaceFmt cjConvertEAGLFmtToRSurfaceFmt(GLenum fmtColor)
{
    switch(fmtColor)
    {
    case GL_RGB8_OES   				: return cjRSurf_RGB8; 	  // 0x8051
    case GL_RGBA8_OES  				: return cjRSurf_RGBA8;   // 0x8058
    case GL_RGB565      			: return cjRSurf_RGB565;  // 0x8D62
	case GL_DEPTH_COMPONENT16     	: return cjRSurf_D16;     // 0x81A5
    case GL_DEPTH_COMPONENT24_OES 	: return cjRSurf_D24X8;   // 0x81A6
	case GL_DEPTH24_STENCIL8_OES  	: return cjRSurf_D24S8;   // 0x88F0
	default : break;
	}

    return cjRSurf_UNKNOWN;
}

inline unsigned int cjConvertEAGLStreamAttr(cjVtxStreamAttr attr)
{
    switch(attr)
    {
	case cjStreamAttr_Usage_StreamDraw : return GL_STREAM_DRAW;
	case cjStreamAttr_Usage_StreamRead : break;
    case cjStreamAttr_Usage_StreamCopy : break;

	case cjStreamAttr_Usage_StaticDraw : return GL_STATIC_DRAW;
	case cjStreamAttr_Usage_StaticRead : break;
	case cjStreamAttr_Usage_StaticCopy : break;

	case cjStreamAttr_Usage_DynamicDraw : return GL_DYNAMIC_DRAW;
	case cjStreamAttr_Usage_DynamicRead : break;
	case cjStreamAttr_Usage_DynamicCopy : break;

	case cjStreamAttr_Access_ReadOnly  : return 0; //return GL_READ_ONLY_OES;
	case cjStreamAttr_Access_WriteOnly : return GL_WRITE_ONLY_OES;
	case cjStreamAttr_Access_ReadWrite : return 0; // return GL_READ_WRITE_OES;
    }
    
    return 0;
}


#endif
