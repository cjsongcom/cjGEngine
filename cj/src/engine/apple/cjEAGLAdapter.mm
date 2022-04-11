
#include "cjEnginePCH.h"
#include "cjEAGLAdapter.h"
#include "cjGLCommon.h"

#include "cjIOSEAGLView.h"
#include "cjEAGLRenRes.h"

//
// cjEAGLAdapterImpl
//

@interface cjEAGLAdapterImpl : NSObject
{
    GLenum                              m_depthFormat;
    GLenum                              m_colorFormat;
    NSString*                           m_colorFormatStr;

    GLuint                              m_frameBuffer;
    GLuint                              m_frameRenderBufferColor;
    GLuint                              m_frameRenderBufferDepth;

    GLint                               m_frameBufferWidth;
    GLint                               m_frameBufferHeight;
 
//    BOOL                m_doMultiSampling;
//    unsigned int        m_numMultiSamples;
//    GLint               m_maxSamplesAllowed;
    
    //buffers for MSAA
    GLint                               m_msaaFrameBuffer;
    GLint                               m_msaaColorBuffer;
    
    EAGLContext*                        m_context;
}

@property (nonatomic,readonly) GLenum depthFormat;
@property (nonatomic,readonly) GLenum colorFormat;
@property (nonatomic,readonly) NSString* colorFormatStr;
@property (nonatomic,readonly) GLuint frameBuffer;
@property (nonatomic,readonly) GLuint frameRenderBufferColor;
@property (nonatomic,readonly) GLuint frameRenderBufferDepth;
@property (nonatomic,readonly) GLuint frameRenderBufferStencil;
@property (nonatomic,readonly) GLint frameBufferWidth;
@property (nonatomic,readonly) GLint frameBufferHeight;
@property (nonatomic,readonly) EAGLContext* context;


//- (BOOL)resizeFromLayer:(CAEAGLLayer *)layer;
- (id) initWithDepthFormat:(GLenum)depthFormat
           withColorFormat:(GLenum)colorFormat
            withSharegroup:(EAGLSharegroup*)sharegroup
         withMultiSampling:(BOOL) doMultiSampling
       withNumberOfSamples:(unsigned int) numMultiSamples;

- (BOOL) onResizeWithLayer:(CAEAGLLayer*)inCAEAGLLayer;
- (BOOL) onResizeWithLayerInGLKView:(GLKView*)inGLKView;
- (void) swapBuffers;
- (void) setupContext;

@end


//
// cjEAGLAdapterImpl implementation
//

@implementation cjEAGLAdapterImpl

@synthesize depthFormat=m_depthFormat;
@synthesize colorFormat=m_colorFormat;
@synthesize colorFormatStr=m_colorFormatStr;
@synthesize frameBuffer=m_frameBuffer;
@synthesize frameRenderBufferColor=m_frameRenderBufferColor;
@synthesize frameRenderBufferDepth=m_frameRenderBufferDepth;
@synthesize frameRenderBufferStencil=m_frameRenderBufferStencil;
@synthesize frameBufferWidth=m_frameBufferWidth;
@synthesize frameBufferHeight=m_frameBufferHeight;
@synthesize context=m_context;


#define CJGL_RST_CHECK_I \
    { GLuint _glerrval=glGetError(); NSAssert3(!_glerrval, @"[GLERR] err=%d , %s,%d", _glerrval, __FILE__, __LINE__); }


- (BOOL) createDefaultOnScreenFrameBuffer:(CAEAGLLayer*)inCAEAGLLayer
{
//    GLuint err;

    [self setupContext];

    // create frame buffer
    glGenFramebuffers(1, &m_frameBuffer);
    CJGL_RST_CHECK_I;
    NSAssert(m_frameBuffer, @"[cjIOEAGLProxyImpl::createDefaultOnScreenFrameBuffer] ERR! - failed to generate framebuffer!");
    
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    CJGL_RST_CHECK_I;

        // create frame color buffer
    glGenRenderbuffers(1, &m_frameRenderBufferColor);
    CJGL_RST_CHECK_I;
    NSAssert(m_frameRenderBufferColor, @"[cjIOEAGLProxyImpl::createDefaultOnScreenFrameBuffer] ERR! - failed to create renderbufferColor!");
    
    glBindRenderbuffer(GL_RENDERBUFFER, m_frameRenderBufferColor);
    CJGL_RST_CHECK_I;

    //glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height);
    // To create a renderbuffer that can be presented to the screen,
    // you bind the renderbuffer and then allocate shared storage for it by calling this method
    // This method call replaces the call normally made to glRenderbufferStorage.
    // A renderbuffer whose storage has been allocated with this method can later be displayed
    // with a call to presentRenderbuffer
    
    // [m_context renderbufferStorage:GL_RENDERBUFFER fromDrawable: message to your EAGLContext binds a storage to the currently bound renderbuffer(glBindRenderbuffer(GL_RENDERBUFFER, (color, depth)),
    
    // to create presentable color buffer
    // allocate shared storage for currently binded render buffer(m_frameRenderBufferColor)
    if(![m_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:inCAEAGLLayer])
    {   // this replaces glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height);
        NSLog(@"[cjIOEAGLProxyImpl::createDefaultOnScreenFrameBuffer] ERR! - Failed to call renderbufferStorage.. ");
    }

    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &m_frameBufferWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &m_frameBufferHeight);

    // attach color buffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_frameRenderBufferColor);
    
    
    NSLog(@"[cjIOEAGLProxyImpl::createDefaultOnScreenFrameBuffer] INFO! - onResizeWithLayer - render buffer size: %d x %d",
        (int)m_frameBufferWidth, (int)m_frameBufferHeight);
    
    
    //
    // Attach DepthBuffer
    //
    
    if(m_depthFormat)
    {
        if(!m_frameRenderBufferDepth)
        {
            glGenRenderbuffers(1, &m_frameRenderBufferDepth);
            NSAssert(m_frameRenderBufferDepth, @"[cjIOEAGLProxyImpl::createDefaultFrameBuffer] ERR! - failed to create depth buffer");
        }
        
        glBindRenderbuffer(GL_RENDERBUFFER, m_frameRenderBufferDepth);
       
//        if(m_doMultiSampling && 0)
//        {
//            glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER, m_numMultiSamples,
//               m_depthFormat, m_width, m_height);
//        }
//        else
//        {

        glRenderbufferStorage(GL_RENDERBUFFER, m_depthFormat, m_frameBufferWidth, m_frameBufferHeight);
        CJGL_RST_CHECK_I;

//        }
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_frameRenderBufferDepth);
        CJGL_RST_CHECK_I;

        
        if(m_depthFormat == GL_DEPTH24_STENCIL8_OES)
        {
          glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_frameRenderBufferDepth);
          CJGL_RST_CHECK_I;

        }
	}
    
    glBindRenderbuffer(GL_RENDERBUFFER, m_frameRenderBufferColor);
    CJGL_RST_CHECK_I;

    
    GLenum status;
    if((status=glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
    {
        NSLog(@"[cjEAGLAdpaterImpl::createDefaultOnScreenFrameBuffer] ERR! - error in operation to create framebuffer status=%X", status);
        return NO;
    }
   
    return YES;
}

- (void) destroyDefaultFrameBuffer
{
    NSLog(@"[cjIOEAGLProxyImpl:destroyDefaultFrameBuffer] INFO! ");
    
    GLuint err;
    
    if(m_frameRenderBufferDepth)
    {
        glDeleteRenderbuffers(1, &m_frameRenderBufferDepth);
        m_frameRenderBufferDepth = 0;
    }

    if(m_frameRenderBufferColor)
    {
        glDeleteRenderbuffers(1, &m_frameRenderBufferColor);
        m_frameRenderBufferColor = 0;
    }

    if(m_frameBuffer)
    {
        glDeleteFramebuffers(1, &m_frameBuffer);
        m_frameBuffer = 0;
    }
}


- (id) initWithDepthFormat:(GLenum)depthFormat
           withColorFormat:(GLenum)colorFormat
            withSharegroup:(EAGLSharegroup*)sharegroup
         withMultiSampling:(BOOL) doMultiSampling
       withNumberOfSamples:(unsigned int) numMultiSamples
{
    self = [super init];
    
    m_depthFormat = 0;
    m_colorFormat = 0 ;
    m_colorFormatStr = nil;
    
    m_frameBuffer = 0;
    m_frameRenderBufferColor = 0;
    m_frameRenderBufferDepth = 0;
    m_frameRenderBufferStencil = 0;
    m_frameBufferWidth = 0;
    m_frameBufferHeight = 0;
    
    m_msaaFrameBuffer = 0;
    m_msaaColorBuffer = 0;
    
    m_context = NULL;
    
    NSLog(@"[cjIOEAGLProxyImpl:initWith~] start %@", self);
 
    if(self)
    {
        if(!sharegroup)
            m_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        else
            m_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2 sharegroup:sharegroup];
        
        if(!m_context || ![EAGLContext setCurrentContext:m_context])
        {
            [self release];
            return nil;
        }
        
        m_depthFormat = depthFormat;
        m_colorFormat = colorFormat;

        switch(m_colorFormat)
        {
       // case :        m_colorFormatStr = kEAGLColorFormatSRGBA8; break;
        case GL_RGB565:     m_colorFormatStr = kEAGLColorFormatRGB565; break;
        case GL_RGBA8_OES:
        default:            m_colorFormatStr = kEAGLColorFormatRGBA8;  break;
        }
        
        /* -http://stackoverflow.com/questions/20769620/srgb-on-ios-opengl-es-2-0
            iOS 7.0 adds the new color format kEAGLColorFormatSRGBA8, which you can set instead of
         kEAGLColorFormatRGBA8 (the default value) for the kEAGLDrawablePropertyColorFormat key 
         in the drawableProperties dictionary of a CAEAGLLayer. If you’re using GLKit to manage your main
         framebuffer for you, you can get GLKView to create a sRGB renderbuffer by setting its
         drawableColorFormat property to GLKViewDrawableColorFormatSRGBA8888.
         
         Note that the OpenGL ES version of EXT_sRGB behaves as if GL_FRAMEBUFFER_SRGB is always enabled. 
         If you want to render without sRGB conversion to/from the destination framebuffer, you’ll need 
         to use a different attachment with a non-sRGB internal format.
        */
        
//        m_doMultiSampling = doMultiSampling;
//        m_numMultiSamples = numMultiSamples;

//        [self createDefaultFrameBuffer];
      
//        if(m_doMultiSampling)
//        {
//            glGetIntegerv(GL_MAX_SAMPLES_APPLE, &m_maxSamplesAllowed);
//            m_numMultiSamples = MIN(m_maxSamplesAllowed, m_numMultiSamples);
//            
//            // Create the MSAA framebuffer (offscreen)
//            glGenFramebuffers(1, &m_msaaFrameBuffer);
//            NSAssert(m_msaaFrameBuffer, @"[cjIOEAGLProxyImpl] ERR! - failed to create MSAA frame buffer!");
//            glBindFramebuffer(GL_FRAMEBUFFER, m_msaaFrameBuffer);
//        }
        
 //     CHECK_GL_ERROR();
    }
    
    return self;
}

//- (id) initWithDepthFormat:(GLenum)depthFormat
//           withColorFormat:(GLenum)colorFormat
//            withSharegroup:(EAGLSharegroup*)sharegroup
//         withMultiSampling:(BOOL) doMultiSampling
//       withNumberOfSamples:(unsigned int) numMultiSamples
//{
//    self = [super init];
//
//    NSLog(@"[cjIOEAGLProxyImpl:initWith~] start %@", self);
// 
//    if(self)
//    {
//        if(!sharegroup)
//            m_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
//        else
//            m_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2 sharegroup:sharegroup];
//        
//        if(!m_context || ![EAGLContext setCurrentContext:m_context])
//        {
//            [self release];
//            return nil;
//        }
//        
//        m_depthFormat = depthFormat;
//        m_colorFormat = colorFormat;
//
//        switch(m_colorFormat)
//        {
//       // case :        m_colorFormatStr = kEAGLColorFormatSRGBA8; break;
//        case GL_RGB565:     m_colorFormatStr = kEAGLColorFormatRGB565; break;
//        case GL_RGBA8_OES:
//        default:            m_colorFormatStr = kEAGLColorFormatRGBA8;  break;
//        }
//    }
//    
//    return self;
//}

- (void) swapBuffers
{
//    if(m_depthFormat)
//    {
//        GLenum attachments[] = { GL_DEPTH_ATTACHMENT };
//        glDiscardFramebufferEXT(GL_FRAMEBUFFER, 1, attachments);
//    }

    if(![ m_context presentRenderbuffer:GL_RENDERBUFFER])
    {
        NSAssert(0,@"[cjIOEAGLProxyImpl] ERR! - failed to call presentRenderbuffer..");
    }
}

- (void) setupContext
{
    if(![EAGLContext setCurrentContext:m_context])
    {
        NSAssert(0,@"[cjIOEAGLProxyImpl:setupContext] ERR! - failed to call setCurrentContext..");
    }
}

- (BOOL) onResizeWithLayerInGLKView:(GLKView*)inGLKView
{
//    // Setup Default Renderbuffer to find out Default Frame Renderbuffeer info
//    [inGLKView bindDrawable ];
//    
//    glGetIntegerv(GL_FRAMEBUFFER_BINDING_OES, &m_frameBuffer);
//    glGetIntegerv(GL_RENDERBUFFER_BINDING_OES, &m_frameRenderBufferColor);
//
//    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &m_frameBufferWidth);
//    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &m_frameBUfferHeight);
//    
//    NSLog(@"[cjEAGLAdpaterImpl] INFO! - created view with size: %d x %d",
//        (int)m_frameBufferWidth, (int)m_frameBUfferHeight);
//   
//    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_DEPTH_ATTACHMENT,  &m_frameRenderBufferDepth);
//    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_STENCIL_ATTACHMENT,  &m_frameRenderBufferStencil);
//    
//    NSLog(@"[cjEAGLAdpaterImpl] INFO! - created view id=(frame/render/depth/stencil): %d,%d,%d,%d",
//        m_frameBuffer, m_frameRenderBufferColor, m_frameRenderBufferDepth, m_frameRenderBufferStencil);

    return TRUE;
}

- (BOOL) onResizeWithLayer:(CAEAGLLayer*)inCAEAGLLayer
{
    [self setupContext];
    
    [self destroyDefaultFrameBuffer];
    return [self createDefaultOnScreenFrameBuffer:inCAEAGLLayer ];

////    [self setupContext];
//    glBindRenderbuffer(GL_RENDERBUFFER, m_frameRenderBufferColor);
//    
//    // To create a renderbuffer that can be presented to the screen,
//    // you bind the renderbuffer and then allocate shared storage for it by calling this method
//    // This method call replaces the call normally made to glRenderbufferStorage.
//    // A renderbuffer whose storage has been allocated with this method can later be displayed
//    // with a call to presentRenderbuffer
//    if(![m_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:inCAEAGLLayer])
//    {
//        NSLog(@"[cjEAGLAdapterImpl] ERR! - Failed to call renderbufferStorage.. ");
//    }
//    
//    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &m_frameBufferWidth);
//    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &m_frameBufferHeight);
//    
//    NSLog(@"[cjEAGLAdpaterImpl] INFO! - onResizeWithLayer - render buffer size: %d x %d",
//        (int)m_frameBufferWidth, (int)m_frameBufferHeight);
//    
//    // glerror
//    if(m_depthFormat)
//    {
//        if(!m_frameRenderBufferDepth)
//        {
//            glGenRenderbuffers(1, &m_frameRenderBufferDepth);
//            NSAssert(m_frameRenderBufferDepth, @"[cjEAGLAdpaterImpl] ERR! - failed to create depth buffer");
//        }
//        
//        glBindRenderbuffer(GL_RENDERBUFFER, m_frameRenderBufferDepth);
//        
////        if(m_doMultiSampling && 0)
////        {
////            glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER, m_numMultiSamples,
////               m_depthFormat, m_width, m_height);
////        }
////        else
////        {
//            glRenderbufferStorage(GL_RENDERBUFFER, m_depthFormat, m_frameBufferWidth, m_frameBufferHeight);
////        }
//        
//        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,m_frameRenderBufferDepth);
//        
//        if(m_depthFormat == GL_DEPTH24_STENCIL8_OES)
//        {
//          glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER,          m_frameRenderBufferDepth);
//        }
//		}
//        
//    glBindRenderbuffer(GL_RENDERBUFFER, m_frameRenderBufferColor);
//    
//    GLenum status;
//    if((status=glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
//    {
//        NSLog(@"[cjEAGLAdpaterImpl] ERR! - error in operation to create framebuffer status=%X", status);
//        return NO;
//    }
//   
//    return YES;
}

- (void)dealloc
{
    NSLog(@"[cjIOEAGLProxyImpl:Dealloc] dealloc %@", self);

    [self destroyDefaultFrameBuffer];
    
//    if(m_msaaColorBuffer)
//    {
//        glDeleteRenderbuffers(1, &m_msaaColorBuffer);
//        m_msaaColorBuffer = 0;
//    }
//    
//    if(m_msaaFrameBuffer)
//    {
//        glDeleteRenderbuffers(1, &m_msaaFrameBuffer);
//        m_msaaFrameBuffer = 0;
//    }
    
    if([EAGLContext currentContext] == m_context)
        [EAGLContext setCurrentContext:nil];
    
    [m_context release];
    m_context = nil;
    
    [super dealloc];
}

@end


//
// cjEAGLAdapter
//

cjEAGLAdapter::cjEAGLAdapter()
{
    m_eaglProxy = NULL;
}

cjEAGLAdapter::~cjEAGLAdapter()
{
    if(m_eaglProxy)
    {
        [((cjEAGLAdapterImpl*)m_eaglProxy) release];
        m_eaglProxy = NULL;
    }
}

cjBool cjEAGLAdapter::Create(cjEAGL_VER ver, cjEAGL_COLOR_FMT colorFmt, cjEAGL_DEPTH depthFmt,
    cjBool preserveBackbuffer, void* eaglContextShareGrp, cjBool doMultiSampling, cjINT32 numMultiSample)
{
    GLenum eaglColorFmt = 0;
    switch(colorFmt)
    {
    case cjEAGL_COLOR_FMT_RGB565: eaglColorFmt = GL_RGB565;    break; 
    case cjEAGL_COLOR_FMT_RGBA8:  eaglColorFmt = GL_RGBA8_OES; break; 
    case cjEAGL_COLOR_FMT_SRGBA8: eaglColorFmt = GL_RGBA8_OES; break;
    default: return cjFalse;
    }
    
    GLint eaglDepthFmt = 0;
    switch(depthFmt)
    {
    case cjEAGL_DEPTH_16:     eaglDepthFmt = GL_DEPTH_COMPONENT16; break;
    case cjEAGL_DEPTH_24:     eaglDepthFmt = GL_DEPTH_COMPONENT24_OES; break;
    case cjEAGL_DEPTH_24_S8:  eaglDepthFmt = GL_DEPTH24_STENCIL8_OES; break;
    default: return cjFalse;
    }
    
    EAGLSharegroup* eaglSharegroup = nil;
    if(eaglContextShareGrp)
        eaglSharegroup = (EAGLSharegroup*)eaglContextShareGrp;
    
    BOOL eaglDoMultiSampling = doMultiSampling ? YES : NO;
    unsigned int eaglNumMultiSample = numMultiSample;
    
    id impl = [ [cjEAGLAdapterImpl alloc] initWithDepthFormat:eaglDepthFmt
                    withColorFormat:eaglColorFmt
                    withSharegroup:eaglSharegroup
                    withMultiSampling:eaglDoMultiSampling
                    withNumberOfSamples:eaglNumMultiSample ];
    
    if(impl == nil) m_eaglProxy = NULL;
    else
    {
        [impl retain];
        m_eaglProxy = impl;
    }

    return m_eaglProxy ? cjTrue : cjFalse;
}
                    
void* cjEAGLAdapter::GetEAGLContext()
{
    if(!m_eaglProxy)
        return NULL;

    return (void*)([((cjEAGLAdapterImpl*)m_eaglProxy) context]);
}

GLuint cjEAGLAdapter::GetDefaultFrameBuffer()
{
    return [((cjEAGLAdapterImpl*)m_eaglProxy) frameBuffer];
}

GLuint cjEAGLAdapter::GetDefaultFrameRenderBufferColor()
{
    return [((cjEAGLAdapterImpl*)m_eaglProxy) frameRenderBufferColor];
}

GLuint cjEAGLAdapter::GetDefaultFrameRenderBufferDepth()
{
    return [((cjEAGLAdapterImpl*)m_eaglProxy) frameRenderBufferDepth];
}

void cjEAGLAdapter::SwapBuffers()
{
    return [((cjEAGLAdapterImpl*)m_eaglProxy) swapBuffers];
}


//GLuint cjEAGLAdapter::GetMSAAFrameBuffer()
//{
//    return [((cjEAGLAdapterImpl*)m_eaglProxy) msaaFrameBuffer];
//}
//
//GLuint cjEAGLAdapter::GetMSAAColorBuffer()
//{
//    return [((cjEAGLAdapterImpl*)m_eaglProxy) msaaColorBuffer];
//}

void* cjEAGLAdapter::GetColorFormatStr()
{
    return (void*)[((cjEAGLAdapterImpl*)m_eaglProxy) colorFormatStr];
}

void cjEAGLAdapter::SetCurrentContext()
{
    cjEAGLAdapterImpl* impl = ((cjEAGLAdapterImpl*)m_eaglProxy);
    [impl setupContext];
}

void cjEAGLAdapter::GetDefaultFrameRenderBufferSize(cjINT32* width, cjINT32* height)
{
    cjEAGLAdapterImpl* impl = ((cjEAGLAdapterImpl*)m_eaglProxy);
    
    *width = [impl frameBufferWidth];
    *height = [impl frameBufferHeight];
}

cjRSurfaceFmt cjEAGLAdapter::GetDefaultFrameRenBufferColorFmt()
{
    cjEAGLAdapterImpl* impl = ((cjEAGLAdapterImpl*)m_eaglProxy);
    return cjConvertEAGLFmtToRSurfaceFmt([impl colorFormat]);
}

GLenum cjEAGLAdapter::GetDefaultFrameRenBufferColorGLFmt()
{
    cjEAGLAdapterImpl* impl = ((cjEAGLAdapterImpl*)m_eaglProxy);
    return [impl colorFormat];
}

cjRSurfaceFmt cjEAGLAdapter::GetDefaultFrameDepthStencilFmt()
{
    cjEAGLAdapterImpl* impl = ((cjEAGLAdapterImpl*)m_eaglProxy);
    return cjConvertEAGLFmtToRSurfaceFmt([impl depthFormat]);
}

GLenum cjEAGLAdapter::GetDefaultFrameDepthStencilGLFmt()
{
    cjEAGLAdapterImpl* impl = ((cjEAGLAdapterImpl*)m_eaglProxy);
    return [impl depthFormat];
}


//
// Calling from cjIOSEAGLView(GLKView)
//

void cjEAGLAdapter::OnResizeWithLayer(void* inCAEAGLLayer)
{
    cjEAGLAdapterImpl* impl = ((cjEAGLAdapterImpl*)m_eaglProxy);
    [impl onResizeWithLayer:(CAEAGLLayer*)inCAEAGLLayer ];
}

//void cjEAGLAdapter::OnResizeWithLayerInGLKView(void* inGLKView)
//{
//    cjEAGLAdapterImpl* impl = ((cjEAGLAdapterImpl*)m_eaglProxy);
//    [impl onResizeWithLayerInGLKView:(GLKView*)inGLKView ];
//}


