#ifndef _CJCOREAPPLE_H_
#define _CJCOREAPPLE_H_

#ifdef __OBJC__
#import <Availability.h>
#endif

#ifndef __IPHONE_5_0
#warning "This project uses features only available in iOS SDK 5.0 and later."
#endif

#ifdef __OBJC__

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <Foundation/NSException.h>

#define CJIOS_MAIN_ENTRY \
    int main(int argc, char *argv[]) { \
        NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init]; \
        int retVal = UIApplicationMain(argc, argv, nil, @"AppController"); \
        [pool release]; \
        return retVal; \
    }


@protocol cjIOSEAGLViewCallback

- (void) onIOSEAGLView_OnDrawRect:(CGRect)rect;

@end


@protocol cjAppViewCntrCallback

- (void) cjAppViewCntrCallback_OnDraw;

@end





//#include <mach-o/dyld.h>
//
//inline void * cjAppleGetProcAddress (const char *name)
//{
//    if(!name)
//        return NULL;
//    
//    NSSymbol symbol;
//    
//    char *symbolName;    symbolName = malloc (strlen (name) + 2);
//    strcpy(symbolName + 1, name);
//    symbolName[0] = '_';
//    symbol = NULL;
//    
//    if (NSIsSymbolNameDefined (symbolName))
//        symbol = NSLookupAndBindSymbol (symbolName);
//    
//    free (symbolName);
//    
//    return symbol ? NSAddressOfSymbol (symbol) : NULL;
//}

#endif

#endif