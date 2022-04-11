
#ifndef _CJEAGLADAPTER_H_
#define _CJEAGLADAPTER_H_

#include "engine/cjGLCommon.h"
#include "engine/apple/cjEAGLDef.h"


class cjEAGLAdapter // do not use : public cjMem
{
public:
    cjEAGLAdapter();
    ~cjEAGLAdapter();

    cjBool Create(cjEAGL_VER ver, cjEAGL_COLOR_FMT colorFmt,
        cjEAGL_DEPTH depth, cjBool preserveBackBuffer=cjFalse, void* eaglContextShareGrp=NULL,
        cjBool doMultiSampling=cjFalse, cjINT32 numMultiSample=0);

    void* GetEAGLContext();

    // Default frame buffer accessor
    GLuint GetDefaultFrameBuffer();
    GLuint GetDefaultFrameRenderBufferColor();
    GLuint GetDefaultFrameRenderBufferDepth();

    void SwapBuffers();
    void SetCurrentContext();
    
    void GetDefaultFrameRenderBufferSize(cjINT32* width, cjINT32* height);
    cjRSurfaceFmt GetDefaultFrameRenBufferColorFmt();
    GLenum GetDefaultFrameRenBufferColorGLFmt();
    cjRSurfaceFmt GetDefaultFrameDepthStencilFmt();
    GLenum GetDefaultFrameDepthStencilGLFmt();

// GLuint GetMSAAFrameBuffer();
// GLuint GetMSAAColorBuffer();
// cjBool IsMultiSampling();
// cjUINT32 GetNumMultiSample();
    
    // EAGL Control
// cjBool GetRetainedBacking() { return cjFalse; }
    void* GetColorFormatStr(); // ret: (NSString*)
    
    void OnResizeWithLayer(void* inCAEAGLLayer);
    void OnResizeWithLayerInGLKView(void* inGLKView);
    
private:
    void*                       m_eaglProxy; // cjEAGLAdapterImpl
};

#endif
