
#include "core/cjCore.h"
#import "cjAppViewCntr.h"


@implementation cjAppViewCntr
@synthesize callback = m_callback;
@synthesize numNeedFrameToStartDraw = m_displayInterval;


- (id)init
{
    self = [super init];

    m_eaglContext = NULL;
    m_displayLink = nil;
    m_displayInterval = 1;
    m_callback = nil;

    [self createDrawNotifier];
      
    return self;
}

- (void)dealloc
{
    [m_displayLink release];
    
    if([EAGLContext currentContext] == m_eaglContext)
    {
        [EAGLContext setCurrentContext:nil];
    }
    
    m_eaglContext = nil;
	
    [super dealloc];
}

-(void) viewDidLoad
{
	[super viewDidLoad];
}

//-(void) update
//{
//     if(m_callback != nil)
//     {
//        [ m_callback cjAppGLKViewCntrCallback_OnUpdate:self ];
//     }
//}

-(void) doNotifyDraw:(id)sender
{
    if(m_callback)
        [m_callback cjAppViewCntrCallback_OnDraw];
}

- (void) createDrawNotifier
{
    if(m_displayLink != nil)
    {
        [m_displayLink invalidate];
        m_displayLink = nil;
    }
    
    m_displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(doNotifyDraw:) ];
    
    [m_displayLink setFrameInterval:m_displayInterval];
    [m_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}


- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    return UIInterfaceOrientationIsLandscape( interfaceOrientation );
}

- (BOOL) shouldAutorotate
{
    return YES;
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

@end
