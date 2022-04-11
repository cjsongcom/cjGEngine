//
//  cjAppDelegate.m
//  HelloCJApp
//
//  Created by 송천종 on 14. 2. 1..
//  Copyright (c) 2014년 송천종. All rights reserved.
//

#include "core/cjCore.h"
#include "engine/cjEngine.h"
#include "game/cjGame.h"

#import "cjAppDelegate.h"
#import "cjAppViewCntr.h"

#include "engine/apple/cjEAGLAdapter.h"
#include "engine/apple/cjIOSEAGLView.h"


static cjObjCls* gs_clsGame = NULL;
static cjGame*   gs_game = NULL;


cjBool cjGameInit(void* egalView);
cjBool cjGameUpdate();
void cjGameDraw();
void cjGameShutDown();

@implementation cjAppDelegate

@synthesize window = m_window;
@synthesize viewController = m_viewController;

- (void) cjAppDelegate
{
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    [[UIApplication sharedApplication] setStatusBarHidden: YES];

    m_window = [[UIWindow alloc] initWithFrame: [[UIScreen mainScreen] bounds]];
    CGRect viewSiz = [m_window bounds];

    cjEAGLAdapter* eaglAdapter = NULL;
    eaglAdapter = new cjEAGLAdapter;
    eaglAdapter->Create(cjEAGL_VER_20, cjEAGL_COLOR_FMT_RGBA8, cjEAGL_DEPTH_24);
    eaglAdapter->SetCurrentContext();
    m_eaglAdapter = (void*)eaglAdapter;
    
    // create egalView
    m_eaglView = [[cjIOSEAGLView alloc] initWithEAGLAdapter:viewSiz eaglAdapter:eaglAdapter callback:(id)self ];

    m_viewController = [[cjAppViewCntr alloc] init];
    m_viewController.view = m_eaglView;
    
// Use RootViewController manage EAGLView
//    m_viewController = [[cjAppGLKViewCntr alloc] initWithNibName:nil bundle:nil];
//   m_viewController.wantsFullScreenLayout = YES;
//   m_viewController.view = m_eaglkView;

    [m_window setRootViewController:m_viewController];
     m_viewController.callback=self;

    [m_window makeKeyAndVisible];
        // EAGLView::didMoveToWindow()
        // EAGLView::layoutSubviews
        //

    // if not , it will cause gl error 36054
    eaglAdapter->SetCurrentContext();
    cjGameInit((__bridge void*)m_eaglView);

    return YES;
}
							
- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
    // pause();
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
    // appEnterBackground();
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
    
    // appEnterForeground();
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
    // resume();
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application
{
    /*
     Free up as much memory as possible by purging cached data objects that can be recreated (or reloaded from disk) later.
     */
    // shrinkMemory();
}

// cjIOSEAGLViewCallback
- (void) onIOSEAGLView_OnDrawRect:(CGRect)rect
{
}

// cjAppViewCntrCallback
- (void) cjAppViewCntrCallback_OnDraw
{
    cjGameUpdate();

    ((cjEAGLAdapter*)m_eaglAdapter)->SetCurrentContext();
    cjGameDraw();
}

@end


cjBool cjGameInit(void* eaglView)
{
	cjInitParam param;
    cjMemZero(&param, sizeof(param));
    
    param.m_eaglView = eaglView;
    param.m_eaglAdapter = [((id)eaglView) eaglAdapter];





    
    
    
    //DWORD flag = GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;
    //::GetModuleHandleEx(flag, (LPCTSTR)&main, (HMODULE*)&param.m_mainExecutableHInstance);
    
//  char startupPath[cjMAX_PATH] = {0};
//  cjFile::NormalizePath(param.m_basePath[cjFilePathHint_Base], cjMAX_PATH, startupPath);
    
    
//    const char* tx = "cjp/mdl/*.cjp";
//    int si = mbstowcs(0, tx, 0);
//    wchar_t xx[100];
//    si = mbstowcs(xx, tx, 100);
////    xx[si] = 0;


//    NSFileManager* nsFileManager = [NSFileManager defaultManager];
//    [nsFileManager retain];
    
//    NSString* nsStr = [NSString stringWithUTF8String:"cjp/mdl/*.cjp"];
//    
//    cjBool b = [nsFileManager fileExistsAtPath:nsStr];
//    
//    
//NSString* bundlePath        = [NSBundle mainBundle].bundlePath;
//NSString* resourcePath      = [NSBundle mainBundle].resourcePath;
//NSString* executablePath    = [NSBundle mainBundle].executablePath;

   
    NSString* fullpath = [ [NSBundle mainBundle] pathForResource:[NSString stringWithUTF8String:("TestMeshPak.cjp")] ofType:nil inDirectory:[NSString stringWithUTF8String:("cjp/mdl/")] ];
   
   
//   const char* px = [fullpath UTF8String];
//   
//    if([nsFileManager fileExistsAtPath:[NSString stringWithUTF8String:px]])
////    if([nsFileManager fileExistsAtPath:fullpath])
//    {
//        cjINT32 x=0;
//        x=1;
//    }
//    else
//    {
//        cjINT32 y=1;
//        y=2;
//    }
   
   
    cjInitParam_AddPkgPath(param, "cjp/mdl/*.cjp");
    
	cjInit(param);
   
	cjObjCls* clsGame = cjObjLoadCls<cjGame>(_CT("cjGame.cjAppGame"));
    gs_game = cjObjNew<cjGame>(clsGame);
    
    cjResChr* chr = cjObjLoadPkgObj<cjResChr>(cjNULL, _CT("TestResChar.Cat"));
    
    gs_game->Init();
    
    return cjTrue;
}

cjBool cjGameUpdate()
{
    if(gs_game)
        return gs_game->Tick() == cjGame::ERUN_OK ? cjTrue : cjFalse;
    
    return cjFalse;
}

void cjGameDraw()
{
    if(gs_game)
    {
        gs_game->Draw();
    }
}

void cjGameShutDown()
{
    cjShutdown();
}
