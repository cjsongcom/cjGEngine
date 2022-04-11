
#ifndef _CJMEM_H_
#define _CJMEM_H_

#include "cjMem_Comm.h"

#ifndef CJ_PLATFORM_WIN32
#include <new>    // std::bad_alloc
#endif


#ifdef _CJMEM_DEBUG_

cjMemSizT cjMemGetSiz(void* m);

#define CJMEM_GETSIZ(x)			cjMemGetSiz(x)

#define cjMemAlloc(siz)			cjMem::_MAlloc(siz, 0, 0, __FILE__, __LINE__, __FUNCTION__)
#define cjMemAllocTag(siz, tag)	cjMem::_MAlloc(siz, 0, tag, __FILE__, __LINE__, __FUNCTION__)
#define cjMemReAlloc(mem, siz, tag)	cjMem::_MReAlloc(mem, siz, tag, __FILE__, __LINE__, __FUNCTION__)

#define cjMemDeAlloc(x)			cjMem::_MDeAlloc(x, 0)
#define cjMemDeAllocPrm(x, p)	cjMem::_MDeAlloc(x, p)

//#define cjMemFastAlloc(siz, count)	cjMem::_MAlloc(siz, 0, 0, __FILE__, __LINE__, __FUNCTION__)
//#define cjMemFastDeAlloc(p)			cjMem::_MDeAlloc(p)

#define cjMemDelete				::delete(cjEMemTag)
#define cjMemNew				::new(cjMemTag, __FILE__, __LINE__, __FUNCTION__)


//
// this will always overwrites global operator new,delete,new[],delete[]
//
// this means new allocation can't used on global variable initialization

CJCORE_API extern unsigned int		cjGIsMemBaseInit;


#ifdef CJ_PLATFORM_APPLE
void* operator new(size_t siz) throw(std::bad_alloc)
#else
inline void* operator new(size_t siz)
#endif
{
	if(!cjGIsMemBaseInit)
		cjMem::_StaticMemBaseInit_();

	return cjMem::_MAlloc(siz);
}

#ifdef CJ_PLATFORM_APPLE
void operator delete(void* m) throw()
#else
inline void operator delete(void* m)
#endif
{
	cjMem::_MDeAlloc(m, 0);
}

#ifdef CJ_PLATFORM_APPLE
void* operator new[](size_t siz) throw(std::bad_alloc)
#else
inline void* operator new[](size_t siz)
#endif
{
	if(!cjGIsMemBaseInit)
		cjMem::_StaticMemBaseInit_();

	return cjMem::_MAlloc(siz);
}

#ifdef CJ_PLATFORM_APPLE
void operator delete[](void* m) throw()
#else
inline void operator delete[](void* m)
#endif
{
	return cjMem::_MDeAlloc(m, 0);
}


CJCORE_API void* operator new(cjMemSizT siz, cjEMemTag, const char* dbgFile=0, unsigned int dbgLine=0, const char* dbgFunc=0);
CJCORE_API void* operator new[](cjMemSizT siz, cjEMemTag, const char* dbgFile=0, unsigned int dbgLine=0, const char* dbgFunc=0);
CJCORE_API void operator delete(void* mem, cjEMemTag);
CJCORE_API void operator delete[](void* mem, cjEMemTag);


///////////////////////////////////////////////////////////////////////////////
#else // _CJMEM_DEBUG_
///////////////////////////////////////////////////////////////////////////////


#define CJMEM_GETSIZ(x)			(*(cjMemSizT*)((cjBYTE*)x - sizeof(cjMemSizT)))

//#define cjNew					new
#define cjMemAlloc(x)			cjMem::_Malloc(x)
#define cjMemReAlloc(m,x)		cjMem::_MRealloc(m,x)

#define cjMemDeAlloc(x)			cjMem::_MDeAlloc(x, 0)
#define cjMemDeAllocPrm(x, p)	cjMem::_MDeAlloc(x, p)
#define cjMemDelete				::delete(cjEMemTag)

//inline void* operator new(size_t siz)
//{
//	return cjMem::_MAlloc(siz);
//}
//
//inline void operator delete(void* m)
//{
//	cjMem::_MDeAlloc(m, 0);
//}
//
//inline void* operator new[](size_t siz)
//{
//	return cjMem::_MAlloc(siz);
//}
//
//inline void operator delete[](void* m)
//{
//	return cjMem::_MDeAlloc(m, 0);
//}

CJCORE_API void* operator new(cjMemSizT siz, cjEMemTag);
CJCORE_API void* operator new[](cjMemSizT siz, cjEMemTag);
CJCORE_API void operator delete(void* mem, cjEMemTag);
CJCORE_API void operator delete[](void* mem, cjEMemTag);


#endif // end of _CJMEM_DEBUG_


typedef void* (*cjMemAllocPF)(cjSiz_t siz, void* owner);
typedef void (*cjMemDeallocPF)(void* mem, void* owner);

void* cjMemAllocDefault(cjSiz_t siz, void* owner);
void cjMemDeallocDefault(void* mem, void* owner);

// Memory functions
CJCORE_API void cjMemSet(void* dst, unsigned char c, cjMemSizT count);
CJCORE_API void cjMemZero(void* dst, cjMemSizT count);
CJCORE_API cjMemSizT cjMemCpy(void* dst, cjMemSizT dstSiz, const void* src, cjMemSizT count); // copy src memory as dstSiz
CJCORE_API void cjMemCpy(void* dst, const void* src, cjMemSizT count);
CJCORE_API void* cjMemMove(void* dst, const void* src, cjMemSizT count);

// 0:equal cmp1 , <0: cmp1 < cmp2 ,  >0: cmp1 > cmp2
CJCORE_API cjINT32 cjMemCmp(const void* cmp1, const void* cmp2, cjMemSizT count);

CJCORE_API void* cjFastMemAlloc(cjSiz_t siz);
CJCORE_API void cjFastMemDeAlloc(void* mem);


#define CJ_SAFE_DEALLOC(x)			{ if(x) cjMemDeAlloc(x); x = 0; }

#define CJ_SAFE_DELETE(x)			{ if(x) delete x; x = NULL; }
#define CJ_SAFE_DELETE_ARY(x)		{ if(x) delete [] x; x = NULL; }




#endif
