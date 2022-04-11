
#ifndef _CJPLATFORMDEF_
#define _CJPLATFORMDEF_

#if defined(_WIN64)
    #define CJ_PLATFORM_WIN32
    #define CJ_P64

#elif defined(_WIN32)
    #define CJ_PLATFORM_WIN32
    #define CJ_P32
    //#define CJ_LIB, CJ_DLL -> defined in cjlib.props, cjdll.props

#elif defined(__APPLE__)
    #define CJ_PLATFORM_APPLE

#ifdef __LP64__
    #define CJ_P64
#else
	#define CJ_P32
#endif

    #define CJ_LIB   // static library mode only

#else
    #define CJ_PLATFORM_LINUX

#ifdef __LP64__
    #define CJ_P64
#else
	#define CJ_P32
#endif

    #define CJ_LIB   // static library mode only


#endif


#define CJ_UNREF_VALUE(x)			x = x;

#endif
