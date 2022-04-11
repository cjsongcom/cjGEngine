
#ifndef _CJIOSEAGLVIEW_H_
#define _CJIOSEAGLVIEW_H_

//
// cjIOSEAGLView
//

#import <QuartzCore/QuartzCore.h>
#import <GLKit/GLKit.h>

//#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>  // defined EAGL constants
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
//#import <CoreFoundation/CoreFoundation.h>

class cjEAGLAdapter;
class cjIOSViewport;

@interface cjIOSEAGLView : UIView <UIKeyInput, UITextInput>
{
    cjEAGLAdapter*                          m_eaglAdapter;
    cjIOSViewport*                          m_viewport;
    CGRect                                  m_viewSiz;

    id<cjIOSEAGLViewCallback>               m_callback;
    
    NSString *                              m_markedText;
    CGRect                                  m_caretRect;
    
    // eliminated 'm_' prefix for accessing by 'xxx.fmtColor=y;"
    // with auto-synthesizing properties this is no longer necessary.
    // Xcode synthesizes a @property xxxx with an ivar named _xxxx behind the scene
    NSString*                               _fmtColor;
    GLuint                                  _fmtDepth;
    BOOL                                    _doMultiSampling;
    unsigned int                            _numMultiSamples;
    EAGLContext*                            _context;
    BOOL                                    _preserveBackbuffer;
    CGSize                                  _sizView;
    NSNotification*                         _keyboardShowNotification;
    BOOL                                    _isKeyboardShown;
    CGFloat                                 _initialScale;
}

// in protocols
@property(nonatomic, readonly) UITextPosition *beginningOfDocument;
@property(nonatomic, readonly) UITextPosition *endOfDocument;
@property(nonatomic, assign) id<UITextInputDelegate> inputDelegate;
@property(nonatomic, readonly) UITextRange *markedTextRange;
@property (nonatomic, copy) NSDictionary *markedTextStyle;
@property(readwrite, copy) UITextRange *selectedTextRange;
@property(nonatomic, readonly) id<UITextInputTokenizer> tokenizer;

//@property(nonatomic, readonly) GLint defaultFrameBuffer;
//@property(nonatomic, readonly) GLint defaultRenderBufferColor;

// for protocol cjIOSEAGLViewListener
@property(nonatomic, strong) id <cjIOSEAGLViewCallback> callback;

// in cjIOSEAGLView
@property(nonatomic, readonly) cjEAGLAdapter* eaglAdapter;
@property(nonatomic, readonly) CGRect viewSiz;

@property(nonatomic, readonly) NSString* fmtColor;          // associated with _fmtColor
@property(nonatomic, readonly) GLuint fmtDepth;
@property(nonatomic, readonly) BOOL doMultiSampling;
@property(nonatomic, readonly) unsigned int numMultiSamples;
@property(nonatomic, readonly) EAGLContext* context;
@property(nonatomic, readonly) BOOL preserveBackbuffer;
@property(nonatomic, readonly) CGSize sizView;
@property(nonatomic, copy) NSNotification* keyboardShowNotification;
@property(nonatomic, readonly, getter = isKeyboardShown) BOOL isKeyboardShown;
@property(nonatomic, readonly) CGFloat initialScale;


// functions
- (id) initWithEAGLAdapter:(CGRect)viewSiz eaglAdapter:(cjEAGLAdapter*)eaglAdapter callback:(id)callback;
- (void) layoutSubviews;

@end


#endif
