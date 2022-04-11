
#ifndef _CJENGINELIBTYPE_H_
#define _CJENGINELIBTYPE_H_

#include "core/cjPlatform.h"


#ifdef CJENGINE_EXPORT

	#if defined(CJ_PLATFORM_WIN32)
        #define CJENGINE_API __declspec(dllexport)
    #else
        #define CJENGINE_API
    #endif

#else

    #ifdef CJENGINE_IMPORT
        #if defined(CJ_PLATFORM_WIN32)
            #define CJENGINE_API __declspec(dllimport)
        #else
            #define CJENGINE_API
        #endif
    #else
        #define CJENGINE_API	// as static library
    #endif

#endif


#endif
