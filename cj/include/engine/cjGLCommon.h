
#ifndef _CJGLCOMMON_H_
#define _CJGLCOMMON_H_

#if defined(CJ_PLATFORM_WIN32)

#define CJ_GL_MODULENAME	_CT("OpenGL32.dll")

// windows gl stuff
#include <gl/gl.h> // windows opengl 1.1
#include "glext.h"
//#include "win32/wglext.h"

#define GLhandle								GLuint
#define GL_FRAMEBUFFER							GL_FRAMEBUFFER_EXT
#define GL_FRAMEBUFFER_COMPLETE					GL_FRAMEBUFFER_COMPLETE_EXT
#define GL_COLOR_ATTACHMENT0					GL_COLOR_ATTACHMENT0_EXT
#define GL_DEPTH_ATTACHMENT						GL_DEPTH_ATTACHMENT_EXT
#define GL_RENDERBUFFER							GL_RENDERBUFFER_EXT
#define GL_FRAMEBUFFER_COMPLETE					GL_FRAMEBUFFER_COMPLETE_EXT
#define cjGL_WRITE_ONLY							GL_WRITE_ONLY

/*
    the transpose parameter in the uniformMatrix API call can only be FALSE in opengl es 2.0
	transpose filed was added to UniformMatrix{234}fv(..) as OpenGL 2.0 supports both column major and row major matrices.
	but OpenGL ES 1.0 and 1.1 do not support both column major and row major matrices because there was no real demand for it.
	there is no reason to support both column major and row major matrices in OpenGL ES 2.0.
	an INVALID_VALUE error will be generated if transpose is not FALSE in OpenGL ES 2.0
*/
#define cjGLES_USE_COLUMN_MAJOR_MAT_ONLY	

// directx-hlsl matrix : column major matrix , default


#elif defined(CJ_PLATFORM_APPLE)


/////////////////////////////////////////////////////////////////////////
// EAGL(Embeded Apple Graphic Library)
/////////////////////////////////////////////////////////////////////////

#ifdef _OBJC_
#import <OpenGLES/EAGL.h>
#import <QuartzCore/QuartzCore.h>
#import <GLKit/GLKit.h>
#endif

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

// there is no double version stuff of GL in EAGL
#define GLclampd            float
#define GLdouble            float

#define GLhandle            GLuint

// there is no PolygonMode in opengl es, also no glPolygonMode function
#define GL_POINT            0x1B00
#define GL_LINE             0x1B01
#define GL_FILL             0x1B02

// dummy for glPolygonMode
void _nullfunc_glPolygonMode(GLenum face, GLenum mode);



#define GL_TEXTURE_MAX_LEVEL        GL_TEXTURE_MAX_LEVEL_APPLE
#define GL_DEPTH_COMPONENT24        GL_DEPTH_COMPONENT24_OES
#define GL_DEPTH_COMPONENT32        0

// GL_MAX_TEXTURE_UNITS is not defined on gl.h iOS
//   GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS  : texture units in vertex shader
//   GL_MAX_TEXTURE_IMAGE_UNITS         : texture units in pixel shader
//   GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS : both shader combined
// OpenGL ES 2.0 on the PowerVR SGX
// You can use up to 8 textures in a fragment shader. You cannot use texture lookups in a vertex shader.

#define GL_MAX_TEXTURE_UNITS GL_MAX_TEXTURE_IMAGE_UNITS

#else

/////////////////////////////////////////////////////////////////////////
// Linux
/////////////////////////////////////////////////////////////////////////

#define CJ_GL_MODULENAME	_CT("libGL.so.1")

#endif

#endif
