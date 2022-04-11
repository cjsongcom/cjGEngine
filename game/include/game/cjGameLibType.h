
#ifndef _CJGAMELIBTYPE_H_
#define _CJGAMELIBTYPE_H_

#include "core/cjPlatform.h"

#ifdef CJGAME_EXPORT

	#if defined(CJ_PLATFORM_WIN32)
        #define CJGAME_API __declspec(dllexport)
    #else
        #define CJGAME_API
    #endif

#else

    #ifdef CJGAME_IMPORT
        #if defined(CJ_PLATFORM_WIN32)
            #define CJGAME_API __declspec(dllimport)
        #else
            #define CJGAME_API
        #endif
    #else
        #define CJGAME_API	// as static library
    #endif

#endif


#endif
