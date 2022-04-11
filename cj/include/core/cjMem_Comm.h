
#ifndef _CJMEM_COMM_H_
#define _CJMEM_COMM_H_

#include "cjCoreLibType.h"


#ifdef _DEBUG
#define _CJMEM_DEBUG_
#endif

enum cjEMemTag { cjMemTag };

#define cjMemSizT			size_t


struct CJCORE_API cjMem
{
#ifdef _CJMEM_DEBUG_
    static void* operator new(cjMemSizT stSizeInBytes, const char* tag=NULL, const char* dbgFile=NULL, unsigned int dbgLine=0, const char* dbgFunc=NULL);
    static void* operator new[](cjMemSizT stSizeInBytes, const char* tag=NULL, const char* dbgFile=NULL, unsigned int dbgLine=0, const char* dbgFunc=NULL);
#else
    static void* operator new(cjMemSizT stSize, const char* tag=NULL);
    static void* operator new[](cjMemSizT stSize, const char* tag=NULL);
#endif

#ifdef _CJMEM_DEBUG_
	static void operator delete(void *mem, const char* tag, const char* dbgFile, unsigned int dbgLine, const char* dbgFunc);
	static void operator delete[](void *mem, const char* tag, const char* dbgFile, unsigned int dbgLine, const char* dbgFunc);
#else
	static void operator delete(void *mem, const char* tag);
	static void operator delete[](void *mem, const char* tag);
#endif

	static void operator delete(void *mem);
	static void operator delete[](void *mem);

	// placement new/delete
    inline static void* operator new( cjMemSizT, void* p ) { return p; }
	inline static void* operator new[]( cjMemSizT, void* p ) { return p; }
    inline static void operator delete( void *, void* ) {}
    inline static void operator delete[]( void *, void* ) {}

	// Malloc/Free
#ifdef _CJMEM_DEBUG_
	static void* _MAlloc(cjMemSizT siz, void* p=0, const char* tag=NULL, const char* dbgFile=0, unsigned int dbgLine=0, const char* dbgFunc=0);
	static void* _MReAlloc(void* mem, cjMemSizT siz, const char* tag=NULL, const char* dbgFile=0, unsigned int dbgLine=0, const char* dbgFunc=0);
#else
	static void* _MAlloc(cjMemSizT siz, void* p=0, const char* tag=NULL);
	static void* _MReAlloc(void* mem, cjMemSizT siz, const char* tag=NULL);
#endif
	static void  _MDeAlloc(void* mem, void* p=0);

	// Static Initialization
	static cjBool _StaticMemBaseInit_(); // can be called on initializing global allocated variable
	static cjBool _StaticMemInit_();

	static void _StaticMemShutdown_();
	static void _ShrinkMem();
};


#endif // end of _CJMEM_COMM_H_

