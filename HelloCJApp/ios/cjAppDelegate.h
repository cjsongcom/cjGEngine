//
//  cjAppDelegate.h
//  HelloCJApp
//
//  Created by 송천종 on 14. 2. 1..
//  Copyright (c) 2014년 송천종. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <core/apple/cjCoreApple.h>

@class cjAppViewCntr;
@class cjIOSEAGLView;

//NSObject <UIAccelerometerDelegate, UIAlertViewDelegate, UITextFieldDelegate,UIApplicationDelegate> {

@interface cjAppDelegate : UIResponder <UIApplicationDelegate, cjIOSEAGLViewCallback, cjAppViewCntrCallback >
//@interface cjAppDelegate : NSObject <UIAccelerometerDelegate, UIAlertViewDelegate, UITextFieldDelegate,UIApplicationDelegate, cjIOSEAGLViewCallback, cjAppViewCntrCallback >
{
    UIWindow*                                   m_window;
    cjAppViewCntr*                              m_viewController;
    
    cjIOSEAGLView*                              m_eaglView;
    void*                                       m_eaglAdapter; // cjEAGLAdapter;
}

- (void) cjAppDelegate;

// cjIOSEAGLViewCallback
- (void) onIOSEAGLView_OnDrawRect:(CGRect)rect;

// cjAppViewCntrCallback
- (void) cjAppViewCntrCallback_OnDraw;

// strong = retain
@property (strong, nonatomic) UIWindow* window;
@property (strong, nonatomic) UIViewController* viewController;

@end
