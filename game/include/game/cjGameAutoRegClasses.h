
/*
	This file only be located in cjGame.cpp!
	Do not include this in anywhere except cjGame.cpp!
*/

#ifndef _CJGAMEAUTOREGCLASSES_H_
#define _CJGAMEAUTOREGCLASSES_H_


//////////////////////////////////////////////////////////////////////////
#ifdef CJ_DLL
	#define CJGAME_AUTOREG_CLASSES

#else
//////////////////////////////////////////////////////////////////////////

    #include "cjGame.h"
    #include "cjAppGame.h"

    #ifdef CJ_PLATFORM_WIN32
        // #include "...."  // win32 only header

        #define CJGAME_AUTOREG_WIN32_ONLY \
           cjGame::cjObjGetStaticCls(); \

    #endif


    #ifdef CJ_PLATFORM_APPLE
        // #include "...."  // apple only header

        #define CJGAME_AUTOREG_APPLE_ONLY \
           cjGame::cjObjGetStaticCls(); \

    #endif

    //
    // Common
    //

	#define CJGAME_AUTOREG_CLASSES_COMMON \
        cjAppGame::cjObjGetStaticCls();

    #define CJGAME_AUTOREG_CLASSES_MERGE(x,y) x y
        

    #ifdef CJ_PLATFORM_WIN32

        #define CJGAME_AUTOREG_CLASSES \
            CJGAME_AUTOREG_CLASSES_MERGE(CJGAME_AUTOREG_CLASSES_COMMON, CJGAME_AUTOREG_WIN32_ONLY)

    #elif defined(CJ_PLATFORM_APPLE)

        #define CJGAME_AUTOREG_CLASSES \
            CJGAME_AUTOREG_CLASSES_MERGE(CJGAME_AUTOREG_CLASSES_COMMON, CJGAME_AUTOREG_APPLE_ONLY)

    #else
        #error "not impl.."

    #endif


//////////////////////////////////////////////////////////////////////////
#endif // end of #ifdef CJ_DLL
//////////////////////////////////////////////////////////////////////////


#endif

