
#include "cjCorePCH.h"

#import "foundation/NSException.h"

void __cjAssert_Impl_Apple__(const char* desc, const char* file, unsigned int line)
{
// http://stackoverflow.com/questions/9739460/weird-error-nsassert
// inside every Objective-c method there are two hidden variables id self and SEL _cmd
// so
// - (void)foo:(id)bar;
// is really
// void foo(id self, SEL _cmd, id bar) { ... }
// and when you call
// [someObject foo:@"hello world"]
// it is actually
// foo( someObject, @selector(foo), @"hello world")
// If you cmd-click on NSAssert to jump to it's definition you will see that it is a macro that uses the hidden _cmd variable of the method you are calling it from. This means that if you are not inside an Objective-c method (perhaps you are in 'main'), therefore you don't have a _cmd argument, you cannot use NSAssert.
// Instead you can use the alternative NSCAssert.
    NSString* str = [ [ NSString alloc ] initWithFormat:@"%s:%d %s", file, line, desc ];
   NSCAssert(NO, str);
}

void __cjLog_Impl_Apple__(const char* log)
{
    NSLog(@"%s", log);
}

void __cjLog_Impl_Apple__(const wchar_t* log)
{
    NSLog([[NSString alloc] initWithBytes:log length:wcslen(log)*sizeof(wchar_t) encoding:NSUTF32LittleEndianStringEncoding]);
}
