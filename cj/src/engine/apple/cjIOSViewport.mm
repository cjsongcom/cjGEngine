
#include "cjEnginePCH.h"
#include "engine/apple/cjIOSViewport.h"

#include "cjEAGLAdapter.h"
#include "engine/apple/cjIOSEAGLView.h"


@implementation cjIOSEAGLView

@synthesize eaglAdapter = m_eaglAdapter;
@synthesize viewSiz = m_viewSiz;
@synthesize callback = m_callback;

//@synthesize defaultFrameBuffer = m_defaultFrameBuffer;
//@synthesize defaultRenderBufferColor = m_defaultRenderBufferColor;
//

// if this method is not declared, you will receive error
// 'CALayer setDrawableProperties:]: unrecognized selector sent to instance'
+ (Class) layerClass
{
    return [CAEAGLLayer class];
}

- (GLKViewDrawableDepthFormat) resolveGLKViewDrawableDepthFormat:(cjRSurfaceFmt)depthFmt
{
    switch(depthFmt)
    {
    case cjRSurf_D16: return GLKViewDrawableDepthFormat16;
    
    case cjRSurf_D24X8:
    case cjRSurf_D24S8:
        return GLKViewDrawableDepthFormat24;
        
    default :
        break;
    }
    
    return GLKViewDrawableDepthFormatNone;
}

- (void)setViewport:(cjIOSViewport*)viewport
{
    m_viewport = viewport;
}

- (id) initWithEAGLAdapter:(CGRect)viewSiz eaglAdapter:(cjEAGLAdapter*)eaglAdapter callback:(id)callback
{
    m_viewport = NULL;
    m_eaglAdapter = eaglAdapter;
    m_viewSiz = viewSiz;
    m_callback = callback;
    
    //BOOL doRetainedBacking = m_eaglAdapter->GetRetainedBacking() ? YES : NO;
    BOOL doRetainedBacking = NO;
    NSString* colorFormatStr = (NSString*)m_eaglAdapter->GetColorFormatStr();
//    EAGLContext* eaglContext = (EAGLContext*)eaglAdapter->GetEAGLContext();
    
    if((self = [super initWithFrame:viewSiz]))
    {
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        eaglLayer.opaque = YES;
        
    // NSDictionary dictionaryWithObjectsAndKeys: value1, key1, value2, key2, nil
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool: doRetainedBacking], kEAGLDrawablePropertyRetainedBacking, colorFormatStr, kEAGLDrawablePropertyColorFormat, nil];

//   eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:FALSE],
//     kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
        
        m_viewSiz = viewSiz;
		
        // check function "setContentScaleFactor" is exist in this class
        if([self respondsToSelector:@selector(setContentScaleFactor:)])
        {
            self.contentScaleFactor = [[UIScreen mainScreen] scale];
        }
    }
    
//    // Adjust Property In GLKView
//	self.context = eaglContext;
//	self.drawableDepthFormat = [ self resolveGLKViewDrawableDepthFormat:eaglAdapter->GetRenBufferDepthStencilFmt() ];  // ex) GLKViewDrawableDepthFormat24;

    // Collect Default FrameBufferInfo
 //   [self bindDrawable ];
//
//    glGetIntegerv(GL_FRAMEBUFFER_BINDING_OES, &m_defaultFrameBuffer);
//    glGetIntegerv(GL_RENDERBUFFER_BINDING_OES, &m_defaultRenderBufferColor);
    return self;
}

- (void) layoutSubviews
{
    cjEAGLAdapter* eaglAdapter = (cjEAGLAdapter*)m_eaglAdapter;
    eaglAdapter->SetCurrentContext();
    
//    eaglAdapter->OnResizeWithLayerInGLKView((void*)(__bridge self));
    CAEAGLLayer* layer = (CAEAGLLayer*)self.layer;
    eaglAdapter->OnResizeWithLayer((void*)(__bridge layer));
    
    cjINT32 width, height;
    eaglAdapter->GetDefaultFrameRenderBufferSize(&width, &height);
    NSLog(@"[cjIOSViewport::layoutSubvies] size width=%d height=%d ", width, height);

    cjIOSViewport* viewport = (cjIOSViewport*)m_viewport;
    if(viewport)
        viewport->ResizeViewport(cjTrue, width, height);
}

/*
About update() The view controller calls its delegate’s glkViewControllerUpdate: method. Your delegate should update frame data that does not involve rendering the results to the screen.

About drawInRect: the GLKView object makes its OpenGL ES context the current context and binds its framebuffer as the target for OpenGL ES rendering commands. Your delegate method should then draw the view’s contents.
*/

- (void)drawRect:(CGRect)rect
{
    if(m_callback != nil)
        [m_callback onIOSEAGLView_OnDrawRect:rect ];
}

//
// implement protocol functions
//

//
// UIKeyInput
//
- (BOOL)hasText
{
    return NO;
}

- (void)insertText:(NSString *)text
{
}

- (void)deleteBackward
{
}


//
// Position
//
- (UITextRange *)textRangeFromPosition:(UITextPosition *)fromPosition toPosition:(UITextPosition *)toPosition;
{
    return nil;
}

- (UITextPosition *)positionFromPosition:(UITextPosition *)position offset:(NSInteger)offset;
{
    return nil;
}

- (UITextPosition *)positionFromPosition:(UITextPosition *)position inDirection:(UITextLayoutDirection)direction offset:(NSInteger)offset;
{
    return nil;
}

//
// Writing Direction
//
- (UITextWritingDirection)baseWritingDirectionForPosition:(UITextPosition *)position inDirection:(UITextStorageDirection)direction;
{
    return UITextWritingDirectionNatural;
}
- (void)setBaseWritingDirection:(UITextWritingDirection)writingDirection forRange:(UITextRange *)range;
{
}

//
// UITextInput - Replacing and Returning Text
//
- (NSString *)textInRange:(UITextRange *)range;
{
    return @"";
}
- (void)replaceRange:(UITextRange *)range withText:(NSString *)theText;
{
}

/* If text can be selected, it can be marked. Marked text represents provisionally
 * inserted text that has yet to be confirmed by the user.  It requires unique visual
 * treatment in its display.  If there is any marked text, the selection, whether a
 * caret or an extended range, always resides within.
 *
 * Setting marked text either replaces the existing marked text or, if none is present,
 * inserts it from the current selection. */
- (void)setMarkedTextRange:(UITextRange *)markedTextRange;
{
}

- (UITextRange *)markedTextRange;
{
    return nil; // Nil if no marked text.
}

- (void)setMarkedTextStyle:(NSDictionary *)markedTextStyle;
{
}

- (NSDictionary *)markedTextStyle;
{
    return nil;
}

- (void)setMarkedText:(NSString *)markedText selectedRange:(NSRange)selectedRange;
{
    if(markedText == m_markedText)
    {
        return;
    }
    
    if(nil != m_markedText)
    {
        [m_markedText release];
    }
    
    m_markedText = markedText;
    [m_markedText retain];
}

- (void)unmarkText;
{
    if(nil == m_markedText)
    {
        return;
    }

 //   const char * pszText = [m_markedText cStringUsingEncoding:NSUTF8StringEncoding];
    
    [m_markedText release];
    m_markedText = nil;
}

//
// Find the closest position to a given point
//
- (UITextPosition *)closestPositionToPoint:(CGPoint)point;
{
    return nil;
}

- (UITextPosition *)closestPositionToPoint:(CGPoint)point withinRange:(UITextRange *)range;
{
    return nil;
}

- (UITextRange *)characterRangeAtPoint:(CGPoint)point;
{
    return nil;
}

//
// evaluation of positions
//
- (NSComparisonResult)comparePosition:(UITextPosition *)position toPosition:(UITextPosition *)other;
{
    return (NSComparisonResult)0;
}

- (NSInteger)offsetFromPosition:(UITextPosition *)from toPosition:(UITextPosition *)toPosition;
{
    return 0;
}

- (UITextPosition *)positionWithinRange:(UITextRange *)range farthestInDirection:(UITextLayoutDirection)direction;
{
    return nil;
}

- (UITextRange *)characterRangeByExtendingPosition:(UITextPosition *)position inDirection:(UITextLayoutDirection)direction;
{
    return nil;
}

//
// Geometry
//
- (CGRect)firstRectForRange:(UITextRange *)range;
{
    return CGRectNull;
}
- (CGRect)caretRectForPosition:(UITextPosition *)position;
{
    return m_caretRect;
}


@end



//
//  cjIOSViewport
//

CJOBJ_IMPL_CLASS(cjIOSViewport)

cjIOSViewport::cjIOSViewport()
{
    m_eaglAdapter = NULL;
    m_iosEAGLView = NULL;
}

cjBool cjIOSViewport::Init(const cjNameChar* title, cjINT32 left,
   cjINT32 top, cjINT32 width, cjINT32 height, void* prm)
{
    if(!prm)
        return cjFalse;

    m_iosEAGLView = prm;
    m_eaglAdapter = [ ((cjIOSEAGLView*)m_iosEAGLView) eaglAdapter ];
    [((cjIOSEAGLView*)m_iosEAGLView) setViewport:((cjIOSViewport*)(__bridge this)) ];
    
    left = 0;
    top = 0;
    
    m_eaglAdapter->GetDefaultFrameRenderBufferSize(&width, &height);
    
    if(!cjSuper::Init(title, left, top, width, height, prm))
      return cjFalse;

    SetNameStr(title);
    
    if(m_viewportFlag & EVF_PrimaryView)
		SetPrimaryViewport(this);
   
    return cjTrue;
}

cjBool cjIOSViewport::Init(void* givenWnd, const cjNameChar* title, void* prm)
{
    return cjFalse;
}

void cjIOSViewport::Tick()
{

}

void cjIOSViewport::ReadInput(cjFLOAT elapsedTime)
{
    
}

void cjIOSViewport::UpdateInputData(cjBool reset, cjFLOAT elapsedTime)
{
    
}

void cjIOSViewport::SetPendingFrame(cjBool pending)
{

}

void cjIOSViewport::Paint()
{
    GetClient()->GetEngine()->Draw(this);
}

cjBool cjIOSViewport::RenderLock()
{
  	cjBool rst = cjTrue;
    
	rst = cjSuper::RenderLock();
    
	if(!rst)
		return cjFalse;
    
	return cjTrue;
}

void cjIOSViewport::RenderUnLock()
{
    	cjSuper::RenderUnLock();
}

void cjIOSViewport::Present()
{
    	cjSuper::Present();
}

cjHWND cjIOSViewport::GetWnd()
{
    return NULL;
}

cjBool cjIOSViewport::AttachRenderDevice(cjRenDev* renDev)
{
  	cjBool rst = cjSuper::AttachRenderDevice(renDev);
    
	if(!rst)
		return cjFalse;
    
	cjBool fullScreen = cjFalse;
	//cjINT32 colorBPP = 32;
    
	cjINT32 width = GetWidth();
	cjINT32 height = GetHeight();
    
	if(!fullScreen)
		GetClientSiz(width, height);
    
	if(!renDev->SetResolution(this, width, height, fullScreen, 32))
	{
		cjLOG(_CT("Failed on SetResolution.."));
		return cjFalse;
	}
    
    m_renDev = renDev;
    
	return cjTrue;
}

void cjIOSViewport::DetachRenderDevice()
{
	if(!m_renDev)
		return;
    
	m_renDev->UnSetResolution(this);
	m_renDev = NULL;
}

void cjIOSViewport::GetClientSiz(cjINT32& width, cjINT32& height)
{
//   CGRect viewSiz = [ ((cjIOSEAGLView*)m_iosEAGLView) viewSiz ];
//   width = viewSiz.size.width;
//   height = viewSiz.size.height;
    width = GetWidth();
    height = GetHeight();
}

void cjIOSViewport::OnDestroy()
{
	if(!GetObjDestroyed())
		--ms_numViewports;
    
//	if(m_input)
//	{
//		delete m_input;
//		m_input = NULL;
//	}
    
	cjSuper::OnDestroy();
    
	if(m_viewportFlag & EVF_PrimaryView)
		SetPrimaryViewport(NULL);
}

cjBool cjIOSViewport::ResizeViewport(cjBool /*fullScreen*/ , cjINT32 width, cjINT32 height)
{
    m_left = 0;
    m_top = 0;
    m_width = width;
    m_height = height;
    
    if(m_renDev && !m_renDev->SetResolution(this, width, height, cjFalse, 32))
    {
        cjLOG(_CT("[cjIOSViewport::ResizeViewport] Failed on SetResolution.."));
        return cjFalse;
    }
    
    return cjTrue;
}

