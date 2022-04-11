
#ifndef _CJPLATFORM_WIN_H_
#define _CJPLATFORM_WIN_H_

#pragma warning(disable : 4291)

#include "w32/cjWinVer.h"

#if !defined(CJ_NOT_USE_W32_CRTMEMDBG) && defined(_DEBUG)

// this declaration can cause conflict with 3dsmax header files..

#define _CRTDBG_MAP_ALLOC	// include Microsoft memory leak detection procedures
#define _INC_MALLOC			// exclude standard memory alloc procedures
#include <crtdbg.h>

#define CJ_WIN32_SET_CRTDBGFLAG  _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#define cjAssert(x)             assert(x)
#define cjRAssert(x)            assert(x)

#else

#define CJ_WIN32_SET_CRTDBGFLAG 
#define cjAssert(x) 
#define cjRAssert(x)

#endif

#define  cjFatal(x)				assert(x)


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define PTW32_STATIC_LIB  // link pthread32 library as static
#include "pthread.h"   // pthread for win32


#ifndef _DEBUG
#define cjAssert(x)
#endif // end of _DEBUG

#define _CJ_ENABLE_LOGGING_
#define _CJFUNCTION_			__FUNCTION__

#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers


#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#define CJW32CRTDBG_INIT			_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#define CJW32CRTDBG_DUMPLEAK		_CrtDumpMemoryLeaks();
#define CJW32CRTDBG_SETBREAK(x)		_CrtSetBreakAlloc(x);	// {,,msvcr100d.dll}_crtBreakAlloc

#else

#define CJW32CRTDBG_INIT			
#define CJW32CRTDBG_DUMPLEAK		
#define CJW32CRTDBG_SETBREAK(x)		

#endif

#include <windows.h>
#include <tchar.h>
#include <float.h>

#define CJ_MAXPATH				260

#define cjOutputDebugString(x)	OutputDebugString(x)


#define cjDLL_EXPORT			__declspec(dllexport)
#define cjSTDCALL				__stdcall

#define cjHMODULE				HMODULE
#define cjHANDLE				HANDLE
#define cjHINSTANCE				void*			// to share name to other platform
#define cjHWND					void*			// to share name to other platform

/*

Datatype	LP64	LLP64
char		8		8
short		16		16
_int32		32		32    
int			32		32
long		64		32   <<   unix64:64, win64:32 
long long	64		64   
pointer		64		64

LLP64 : microsoft windows
LP64  : most unix 

do not use 'long' type!!
but long long type is safe.
'DWORD = unsigned long'

sizeof(DWORD) = sizeof(unsigned long)
=> unix32,win32 = 32
=> unix64 = 64, win64=32

sizeof(DWORD64) = 64 for win64

*/

typedef signed char					cjSBYTE;
typedef unsigned char				cjBYTE;
typedef unsigned short				cjWORD;
typedef signed short				cjSWORD;
typedef float						cjFLOAT;
typedef double						cjDOUBLE;
typedef unsigned char				cjUINT8;
typedef signed char					cjINT8;
typedef unsigned __int16			cjUINT16;
typedef signed __int16              cjINT16;
typedef unsigned __int32			cjUINT32;
typedef signed __int32              cjINT32;
typedef unsigned __int64			cjUINT64;
typedef __int64						cjINT64;

//typedef unsigned __int64			cjQWORD;
//typedef signed __int64			cjSQWORD;
//typedef unsigned long long		cjLONGLONG;

typedef unsigned long				cjWinDWORD;  // win32=win64=32bit
typedef long						cjWinLong;   // win32=win64=32bit
typedef unsigned __int32			cjTFLAG;
typedef unsigned __int32			cjTENUM;
typedef size_t                      cjSiz_t;

#define CJPRIu64					llu

#ifdef CJ_P64
#define cjPtrDiff					cjINT64
#else
#define cjPtrDiff					cjINT32
#endif

#define cjNULL_PTR					NULL

#define CJ_SLOW_CHECK


/*	
	enable C4244 / C4800 need
	cjBool = unsigned char
	32bitFlag & XXX_FLAG(0x01000000) is wrong result, and always 0, unsigned char just takes low 1bit
	
	cjBool = unsigned int
	32bitFlag & XXX_FLAG(0x01000000) is correct result, unsigned int takes all bit

	bool
	32bitFlag & XXX_FLAG(0x01000000) is correct result, compiler will implicit convert bool type
*/

typedef unsigned char				cjBool;
const cjBool						cjTrue = 1;
const cjBool						cjFalse = 0;
const cjBool						CJ_BOOL_INVALID = (cjBool)-1;

//typedef bool						cjBool;
//enum { cjTrue = true, cjFalse = false };


typedef unsigned int				cjBitField;

typedef char						cjChar;
typedef wchar_t 					cjUni;		// 2byte on win, 4byte on POSIX(mac,unix)

typedef cjINT64                     cjBigOffset;
typedef __int32						cjOffset;
typedef __int32						cjOffset32;

#ifdef CJ_P64
typedef unsigned __int64			cjPTR_NUMERIC;
#define cjPtrToNumeric(x)			((unsigned __int64)(x))
#else
typedef unsigned __int32			cjPTR_NUMERIC;
#define cjPtrToNumeric(x)			((unsigned __int32)(x))
#endif


//
// Atomic functions
//

inline cjINT32 cjAtomicInc(cjINT32& valSigned)
{
    return InterlockedIncrement((LONG*)&valSigned);
}

inline cjINT32 cjAtomicDec(cjINT32& valSigned)
{
    return InterlockedDecrement((LONG*)&valSigned);
}

inline cjUINT32 cjAtomicInc(cjUINT32& valUSigned)
{
    return InterlockedIncrement((LONG*)&valUSigned);
}

inline cjUINT32 cjAtomicDec(cjUINT32& valUSigned)
{
    cjAssert(valUSigned > 0);
    return InterlockedDecrement((LONG*)&valUSigned);
}




#include <MMSystem.h>

#endif
