
#ifndef _CJCORELIBTYPE_H_
#define _CJCORELIBTYPE_H_

#include "cjPlatformDef.h"

#ifdef CJCORE_EXPORT

    #if defined(CJ_PLATFORM_WIN32)
        #define CJCORE_API __declspec(dllexport)
    #else
        #define CJCORE_API
    #endif

#else

    #ifdef CJCORE_IMPORT
        #if defined(CJ_PLATFORM_WIN32)
            #define CJCORE_API __declspec(dllimport)
        #else
            #define CJCORE_API
        #endif
    #else
        #define CJCORE_API		// as static library
    #endif

#endif


#endif