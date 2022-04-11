
#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>


@interface cjAppViewCntr : UIViewController
{
    EAGLContext*                                m_eaglContext;
 
    id                                          m_displayLink;
    int                                         m_displayInterval;

    id <cjAppViewCntrCallback>                  m_callback;
}

// strong = retain
@property (strong, nonatomic) EAGLContext* eaglContext;

// The default value is 1, which results in your application being notified at the refresh rate of the display.
// If the value is set to a value larger than 1, the display link notifies your application at a fraction
// of the native refresh rate. For example, setting the interval to 2 causes the display link to fire
// every other frame, providing half the frame rate.
@property (nonatomic) int numNeedFrameToStartDraw;
@property (strong, nonatomic) id callback;

@end
