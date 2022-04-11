
#ifndef _CJPLATFORM_APPLE_H_
#define _CJPLATFORM_APPLE_H_

#pragma warning(disable : 4291 )

/*
    xcode project setting defines
    
    
    -[0] HelloCJApp.ios
      xcode.proj.xcodeproj

      - Build
        - Intermediate
          - HelloCJApp.build
            - Debug-iphonesimulator
              - core.build
              - engine.build
          - Precompiled header
 
        - Products
            - Debug-iphonesimulator
               libcore.a
 
 
 
 $(BUILD_DIR) : HelloCJApp.ios/Build/Products
 $(EFFECTIVE_PLATFORM_NAME) = -iphoneos
 
 
 $> xcodebuild -target HelloCJApp -showBuildSettings
 find LIBRARY_SEARCH_PATHS value, then verify value is valid
 * HelloCJAPP - LIBRARY_SEARCH_PATHS
 
 https://developer.apple.com/library/mac/documentation/DeveloperTools/Reference/XcodeBuildSettingRef/1-Build_Setting_Reference/build_setting_ref.html#//apple_ref/doc/uid/TP40003931-CH3-SW48

 

*/


// disable "deprecated conversion from string constant to char*"
#pragma GCC diagnostic ignored "-Wwrite-strings"

#define CJLIB


#define _CJ_ENABLE_LOGGING_
#define _CJFUNCTION_			__FUNCTION__
#define cjAssert(x)				{ if(!(x)) __cjAssert_Impl_Apple__(#x, __FILE__, __LINE__); }
#define cjFatal(x)              { if(!(x)) __cjAssert_Impl_Apple__(#x, __FILE__, __LINE__); }

#define CJ_MAXPATH				260

#if (DEBUG == 1)
#ifndef _DEBUG
#define _DEBUG
#endif
#endif

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <stdarg.h>
#include <time.h>

#include <assert.h>

#include <sys/sysctl.h>    // kinfo_proc
#include <pthread.h>        // built-in library
#include <unistd.h>


bool IsDebuggerPresent();
void OutputDebugString(const char* fmt, ...);

void __cjAssert_Impl_Apple__(const char* desc, const char* file, unsigned int line);
void __cjLog_Impl_Apple__(const char* log);
void __cjLog_Impl_Apple__(const wchar_t* log);


#define CJ_MAXPATH				260
#define cjOutputDebugString(x)

#define cjDLL_EXPORT			
#define cjSTDCALL	
#define cjHMODULE				void*
#define cjHANDLE				void*
#define cjHINSTANCE				void*
#define cjHWND					void*

typedef signed char					cjSBYTE;
typedef unsigned char				cjBYTE;
typedef unsigned short				cjWORD;
typedef signed short				cjSWORD;
typedef float						cjFLOAT;
typedef double						cjDOUBLE;
typedef unsigned short              cjUINT16;
typedef signed short                cjINT16;
typedef unsigned int				cjUINT32;
typedef signed int                  cjINT32;
typedef unsigned long long          cjUINT64;
typedef long long                   cjINT64;   // same to long(64bit)

typedef unsigned int                cjTFLAG;
typedef size_t                      cjSiz_t;


#define CJPRIu64					i64d

#ifdef CJ_P64
#define cjPtrDiff					cjINT64
#else
#define cjPtrDiff					cjINT32
#endif

#define cjNULL_PTR					NULL

#define CJ_SLOW_CHECK

typedef unsigned char				cjBool;
enum { cjTrue = 1, cjFalse = 0 };
typedef unsigned int				cjBitField;

typedef char						cjChar;
typedef wchar_t 					cjUni;		// 4byte on POSIX(mac,unix)


typedef cjINT64                     cjBigOffset;
typedef signed int					cjOffset;
typedef signed int					cjOffset32;

#ifdef CJ_P64
typedef unsigned long long			cjPTR_NUMERIC; //64
#define cjPtrToNumeric(x)			((unsigned long long)(x))
#else
typedef unsigned int                cjPTR_NUMERIC; //32
#define cjPtrToNumeric(x)			((unsigned int)(x))
#endif


//
// all stuff that is for apple not for linux
//

#include <mach/mach_time.h>
#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 0

typedef int clockid_t;

int clock_gettime(clockid_t clk_id, struct timespec *t);


//
// Atomic functions
//

#include <libkern/OSAtomic.h>

/*
 * These are the preferred atomic and synchronization operations.
 *
 * Their implementation is customized at boot time for the platform, including
 * late-breaking errata fixes as necessary. They are thread safe.
 *
 * WARNING: all addresses passed to these functions must be "naturally aligned",
 * i.e.  * <code>int32_t</code> pointers must be 32-bit aligned (low 2 bits of
 * address are zeroes), and <code>int64_t</code> pointers must be 64-bit aligned
 * (low 3 bits of address are zeroes.)
 *
 * Note that some versions of the atomic functions incorporate memory barriers
 * and some do not.  Barriers strictly order memory access on weakly-ordered
 * architectures such as ARM.  All loads and stores that appear (in sequential
 * program order) before the barrier are guaranteed to complete before any
 * load or store that appears after the barrier.
 *
 * The barrier operation is typically a no-op on uniprocessor systems and
 * fully enabled on multiprocessor systems. On some platforms, such as ARM,
 * the barrier can be quite expensive.
 *
 * Most code should use the barrier functions to ensure that memory shared
 * between threads is properly synchronized.  For example, if you want to
 * initialize a shared data structure and then atomically increment a variable
 * to indicate that the initialization is complete, you must use
 * {@link OSAtomicIncrement32Barrier} to ensure that the stores to your data
 * structure complete before the atomic increment.
 *
 * Likewise, the consumer of that data structure must use
 * {@link OSAtomicDecrement32Barrier},
 * in order to ensure that their loads of the structure are not executed before
 * the atomic decrement.  On the other hand, if you are simply incrementing a
 * global counter, then it is safe and potentially faster to use
 * {@link OSAtomicIncrement32}.
 *
 * If you are unsure which version to use, prefer the barrier variants as they
 * are safer.
 *
 * The spinlock and queue operations always incorporate a barrier.
 *
 * For the kernel-space version of this header, see
 * {@link //apple_ref/doc/header/OSAtomic.h OSAtomic.h (Kernel Framework)}
 *
 * @apiuid //apple_ref/doc/header/user_space_OSAtomic.h

 // use barrier function to prevent memory sync problem
*/


inline cjINT32 cjAtomicInc(cjINT32& valSigned)
{
    return OSAtomicIncrement32Barrier((int32_t*)&valSigned);
}

inline cjINT32 cjAtomicDec(cjINT32& valSigned)
{
    return OSAtomicDecrement32Barrier((int32_t*)&valSigned);
}

inline cjUINT32 cjAtomicInc(cjUINT32& valUSigned)
{
    return OSAtomicIncrement32Barrier((int32_t*)&valUSigned)+1;
}

inline cjUINT32 cjAtomicDec(cjUINT32& valUSigned)
{
//    cjAssert(valUSigned > 0);
    return OSAtomicDecrement32Barrier((int32_t*)&valUSigned)-1;
}


#import "apple/cjCoreApple.h"


#endif
