
#include "cjCorePCH.h"
#include "cjCoreAutoRegClasses.h"


CJOBJ_IMPL_PKG_MODULE(cjCore)

static cjInitParam gs_InitParam;
void* g_mainExecutableHInstance = NULL;

cjImplementLib(cjCore, CJCORE_API, "")



cjImplementLib_Init(cjCore)
{
	cjLOG("[cjCore] lib init..");

	CJCORE_AUTOREG_CLASSES

	cjFile::SetBasePath(cjFilePathHint_Base, gs_InitParam.m_basePath[cjFilePathHint_Base]);    
    cjFileSystem::StaticInit_cjFileSystem();
	cjExtraDat::StaticInit();
    
	// 	cjMem::_StaticInit_(); called form cjLibManager::Init

	// generate built in package	
	    
    return 1;
}

cjImplementLib_Shutdown(cjCore)
{
	cjFileSystem::StaticShutdown_cjFileSystem();
	cjLOG("[cjCore] lib shutdown..");
}

cjImplementLib_MakeModulePkgLoad(cjCore)
{
	cjObjPkgLoad* pkgLoad = (cjObjPkgLoad*)_pkgLoad;
	cjFObjImpDesc* imp = NULL;
	
	//                    ClsPkg    ClsName     ObjName
	// [0] - cjCore, cjObjPackage, "cjCore"
	imp = new(pkgLoad->m_importDescAry) cjFObjImpDesc();
	imp->m_classPkgName = cjName_cjCore;
	imp->m_className    = cjName_cjObjPackage;
	imp->m_objName		= cjName_cjCore;

	imp->m_pkgIndex		= 0;
	imp->m_srcPkgLoad	= pkgLoad; // cjCore pkg load
	imp->m_srcPkgLoadExpIndex = cjObjIndex_None;  // cjObjPackage has no expIndex
	imp->m_impObj		= NULL;
	
	// [1] - cjCore, cjObjCls, "cjObjCls"
	imp = new(pkgLoad->m_importDescAry) cjFObjImpDesc();
	imp->m_classPkgName = cjName_cjCore;
	imp->m_className    = cjName_cjObjCls;
	imp->m_objName		= cjName_cjObjCls;

	imp->m_pkgIndex		= -1;
	imp->m_srcPkgLoad	= pkgLoad; // cjCore pkg load
	imp->m_srcPkgLoadExpIndex = cjObjIndex_None;  // for inherited cjObjCls
	imp->m_impObj		= NULL;
	
}


//
// Global variables
// 

CJCORE_API cjBool								cjGIsOpenGL = cjFalse;




//
//  cjTAbstractPtrPoolAlloc_XXX
//

void* cjDeclareForwardTPtrPoolAlloc::ms_shutdownList = cjNULL;

CJTABSTRACT_PTRPOOL_ALLOC_IMPL(size_t, 2048)				// mostly pointer list,hash
CJTABSTRACT_PTRPOOL_ALLOC_IMPL(unsigned char, 256)
CJTABSTRACT_PTRPOOL_ALLOC_IMPL(cjAnimatorTimeline, 256)
CJTABSTRACT_PTRPOOL_ALLOC_IMPL(cjSerializable*, 256)		// cjTPtrHash<cjSerializable*, cjSerializable*>


void cjDeclareForwardTPtrPoolAlloc::Init()
{
	typedef cjTSingleListItem<cjTAbstractPtrPoolAlloc_PFShutDown> Item;
    cjTAbstractPtrPoolAlloc_PFShutDown pfShutDown;
    
    #define CJTABSTRACT_PTRPOOL_REG_SHUTDOWN(type) \
        pfShutDown = &cjTAbstractPtrPoolAlloc< type >::ShutDown; \
        ms_shutdownList = (void*)new Item( pfShutDown, (Item*)ms_shutdownList);

	CJTABSTRACT_PTRPOOL_REG_SHUTDOWN(size_t)
	CJTABSTRACT_PTRPOOL_REG_SHUTDOWN(unsigned char)
	CJTABSTRACT_PTRPOOL_REG_SHUTDOWN(cjSerializable*)
	CJTABSTRACT_PTRPOOL_REG_SHUTDOWN(cjAnimatorTimeline)
}

void cjDeclareForwardTPtrPoolAlloc::ShutDown()
{
	typedef cjTSingleListItem<cjTAbstractPtrPoolAlloc_PFShutDown> Item;
	Item* next = (Item*)ms_shutdownList;
	Item* toDel;

	while(next)
	{
		(next->m_item)();

		toDel = next;
		next = next->m_next;
		delete toDel;
	}
    
    ms_shutdownList = cjNULL;
}


//
// cjCore APIs
//

CJCORE_API void cjGetInitParam(cjInitParam& out)
{
    out = gs_InitParam;
}


CJCORE_API cjBool cjInit(const cjInitParam& param)
{
    gs_InitParam = param;
    
	g_mainExecutableHInstance = param.m_mainExecutableHInstance;

	cjLibManager::Init();

	cjProperty::CheckPropertyNames();

    return cjTrue;
}

CJCORE_API void cjShutdown()
{
	cjLibManager::Shutdown();

	cjObjCls::ms_onShutdown = cjTrue;

#ifndef CJDLL
	// clean up cjClsCls
	cjINT32 numDelete = 0;
	if(cjObjCls::ms_next)
	{
		cjObjCls** cur = cjObjCls::ms_next;

		while(cur)
		{
			cjObjCls* del = cur[1];
			cur= (cjObjCls**)cur[0];

			::delete del;
			numDelete++;
		}
	}

	cjAssert(cjObjCls::ms_count == numDelete);
	cjObjCls::ms_next = NULL;

#endif
}


CJCORE_API cjHMODULE cjGetDllLibHandle(const cjChar* dllPath)
{
	cjAssert(dllPath);

	if(!dllPath)
		return NULL;

#ifdef CJ_PLATFORM_WIN32

	cjHMODULE handle = (cjHMODULE)::LoadLibraryA(dllPath);
	return handle;

#else

//    #pragma message("ERR!- cjGetDllHandle is not implemented")

    return NULL;
#endif
}

CJCORE_API cjHMODULE cjGetDllLibHandle(const cjUni* dllPath)
{
	cjAssert(dllPath);

	if(!dllPath)
		return NULL;

#ifdef CJ_PLATFORM_WIN32

	cjHMODULE handle = (cjHMODULE)::LoadLibraryW(dllPath);
	return handle;

#else

//    #pragma message("ERR!- cjGetDllHandle is not implemented")

    return NULL;
#endif
}


CJCORE_API void* cjGetDllProcAddr(cjHMODULE module, const char* procName)
{
#ifdef CJ_PLATFORM_WIN32
	return (void*)GetProcAddress((HMODULE)module, procName);
#else
	return NULL;
#endif
}

//
// Time
//
#define SEC2NANO 1000000000

#define BILLONSEC 1000000000
#define MICROSEC 1000000		// 1s = 1000000.0
#define MILLONSEC 1000			// 1s = 1000.0


static cjBool gs_engineTimeReset = false;

#if defined(CJ_PLATFORM_WIN32)

static cjEngineTime_t gs_engineTime;
static cjEngineTime_t gs_engineTimeLastElapsed;

static LARGE_INTEGER gs_engineTimeLastQuad;

static cjDOUBLE gs_engineTimeFreqPerSec; 
static LARGE_INTEGER gs_engineTimeFreq;

#elif defined(CJ_PLATFORM_APPLE)

#include <mach/mach.h>
#include <mach/mach_time.h>

static uint64_t gs_engineTime = 0;
static cjEngineTime_t gs_engineTimeLastElapsed = 0;
static uint32_t gs_timebaseFreq = 0;
static uint64_t gs_engineTimeLast = 0;

static mach_timebase_info_data_t gs_engineTimebaseInfo;

#else

//static struct timespec gs_engineTimeClockResolution;
static struct timespec gs_engineTimeClockLast;

/*
 CLOCK_REALTIME, a system-wide realtime clock.
 CLOCK_PROCESS_CPUTIME_ID, high-resolution timer provided by the CPU for each process.
 CLOCK_THREAD_CPUTIME_ID, high-resolution timer provided by the CPU for each of the threads.
 */
// CLOCK_REALTIME, get elapsed time from january 1, 1970 00:00 ~
static clockid_t gs_engineTimeClockID = CLOCK_REALTIME;

static unsigned long CalcClockElapsedFromTimeSpec(struct timespec* prev, struct timespec* cur)
{
//    long nano_time;
    long diff_sec;
    long diff_nano;
    
    if((cur->tv_nsec - prev->tv_nsec) < 0)
    {
        diff_sec = cur->tv_sec - prev->tv_sec - 1;
        diff_nano = SEC2NANO + cur->tv_nsec - prev->tv_nsec;
    }
    else
    {
        diff_sec  = cur->tv_sec - prev->tv_sec;
        diff_nano = cur->tv_nsec - prev->tv_nsec;
    }
    
//    nano_time = SEC2NANO * diff_sec + diff_nano;
    return diff_sec + (diff_nano/SEC2NANO);
}

#endif


CJCORE_API void cjResetEngineTime()
{
    
#if defined(CJ_PLATFORM_WIN32)
    gs_engineTime = 0;
	gs_engineTimeLastElapsed = 0;

    cjMemZero(&gs_engineTimeLastQuad, sizeof(gs_engineTimeLastQuad));	
    QueryPerformanceFrequency(&gs_engineTimeFreq);
	gs_engineTimeFreqPerSec = cjDOUBLE(gs_engineTimeFreq.QuadPart);
    
#elif defined(CJ_PLATFORM_APPLE)
    gs_engineTime = 0;
    gs_engineTimeLastElapsed = 0;

    if(gs_engineTimebaseInfo.denom == 0)
    {
        mach_timebase_info(&gs_engineTimebaseInfo);
        gs_timebaseFreq = gs_engineTimebaseInfo.numer / gs_engineTimebaseInfo.denom;
    }
    
    gs_engineTimeLast = mach_absolute_time();
    
#else
    #error "error not implemented!!!"
    clock_gettime(gs_engineTimeClockID, &gs_engineTimeClockLast);
#endif

	gs_engineTimeReset = true;
}

CJCORE_API void cjUpdateEngineTime()
{
#if defined(CJ_PLATFORM_WIN32)

//    static cjBool init = cjFalse;
//    if(!init)
//    {
//        cjResetEngineTime();
//        init = cjTrue;
//    }
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);

	// cjDOUBLE
	if(gs_engineTimeReset)
	{
		gs_engineTimeLastElapsed = cjET(0);
		gs_engineTimeReset = false;
	}
	else
	{
		// generate ms
		gs_engineTimeLastElapsed = cjDOUBLE(counter.QuadPart - gs_engineTimeLastQuad.QuadPart) 
			/ gs_engineTimeFreqPerSec;
		gs_engineTime += gs_engineTimeLastElapsed;
	}

	gs_engineTimeLastQuad = counter;
    
#elif defined(CJ_PLATFORM_APPLE)

    //cjopti
#pragma message("cj Need to optimize..")

	uint64_t cur = mach_absolute_time();
	gs_engineTimeLastElapsed = (cjEngineTime_t)(cur - gs_engineTimeLast);
    gs_engineTime += gs_engineTimeLastElapsed;
    gs_engineTimeLast = cur;
    
#else
    struct timespec cur;
    clock_gettime(gs_engineTimeClockID, &cur);
    gs_engineTimeLastElapsed = CalcClockElapsedFromTimeSpec(&gs_engineTimeClockLast, &cur);
    gs_engineTime += gs_engineTimeLastElapsed;
    gs_engineTimeClockLast = cur;
#endif
}

CJCORE_API cjEngineTime_t cjGetEngineTimeSec()
{
#if defined(CJ_PLATFORM_WIN32)
    return gs_engineTime;

#elif defined(CJ_PLATFORM_APPLE)
	return (cjDOUBLE)gs_engineTime * gs_timebaseFreq * 1000.0f;
#else
    #error "not impl"
    return gs_engineTime;
#endif
}

//CJCORE_API cjEngineTime_ft cjGetEngineTimeSec()
//{
//#if defined(CJ_PLATFORM_WIN32)
//    #error "not impl"
//    //return ((cjEngineTime_ft)gs_engineTimeQuadpart / gs_engineTimeFreq);
//#elif defined(CJ_PLATFORM_APPLE)
//	return (cjDOUBLE)gs_engineTime * gs_timebaseFreq / BILLONSEC;
//#else
//    #error "not impl"
//    //return (cjEngineTime_ft)gs_engineTime/1000.0f;
//#endif
//}

CJCORE_API cjEngineTime_t cjGetEngineTimeElapsedSec()
{
#if defined(CJ_PLATFORM_WIN32)
	return gs_engineTimeLastElapsed;

#elif defined(CJ_PLATFORM_APPLE)
	return (cjEngineTime_t)((cjDOUBLE)gs_engineTimeLastElapsed * gs_timebaseFreq / BILLONSEC);

#else
    #error "not impl"
//  return (cjEngineTime_ft)gs_engineTime/1000.0f;
#endif
}

//CJCORE_API cjEngineTime_ft cjGetEngineTimeElapsedSec()
//{
//#if defined(CJ_PLATFORM_WIN32)
//    #error "not impl"
//#elif defined(CJ_PLATFORM_APPLE)
//	return (cjDOUBLE)gs_engineTimeLastElapsed * gs_timebaseFreq / BILLONSEC;
//#else
//    #error "not impl"
//#endif
//}


//
// cjLoadFileToArray
//

CJCORE_API cjBool cjLoadFileToByteArray(cjTAry<cjBYTE>& buf, const cjChar* path,
	cjFilePathHint hint, cjTFLAG readFlag, cjBool /*loggingErr*/)
{
    cjFStream* stream = cjFileSystem::Get()->CreateFileReadStream(path, hint, readFlag);
    
	if(!stream)
		return cjFalse;
    
	cjAryOff siz = (cjAryOff)stream->GetSiz();

	buf.RemoveAll(siz);
	buf.SetSiz(siz);
    
    cjStreamOffset sizToRead = buf.Num();
	cjBool rst = sizToRead == stream->SerializeStream(&buf(0), sizToRead) ? cjTrue : cjFalse;

	delete stream;
    
	return rst;
}

CJCORE_API cjBool cjLoadFileToByteArray(cjTAry<cjBYTE>& buf, const cjUni* path,
	cjFilePathHint hint, cjTFLAG readFlag, cjBool /*loggingErr*/)
{
    cjFStream* stream = cjFileSystem::Get()->CreateFileReadStream(path, hint, readFlag);
    
	if(!stream)
		return cjFalse;

	cjAryOff siz = stream->GetSiz();
    
	buf.RemoveAll(siz);
	buf.SetSiz(siz);
    
    cjStreamOffset sizToRead = buf.Num();
	cjBool rst = sizToRead == stream->SerializeStream(&buf(0), sizToRead) ? cjTrue : cjFalse;

	delete stream;
    
	return rst;
}

CJCORE_API const cjChar* cjGetFileExtStr(const cjChar* path)
{
    const cjChar* p = cjStrrchr(path, '.');
    return p ? p+1 : NULL;
}

CJCORE_API const cjUni* cjGetFileExtStr(const cjUni* path)
{
    const cjUni* p = cjStrrchr(path, '.');
    return p ? p+1 : NULL;
}

CJCORE_API void cjSplitPathA(const char* path, cjStringA& pathOnly, cjStringA& filenameOnly, cjBool addSlash)
{
	if(!path)
		return;

	const char* c = path;

	while(*c)
		c++;

	cjBool foundFilename = cjFalse;

	for(;c>path; c--)
	{
		if(*c == '/')
		{
			filenameOnly = (c+1);
			foundFilename = cjTrue;
			break;
		}
	}

	if(foundFilename)
		pathOnly.Import(path, c + (addSlash ? 1:0));
	else
		pathOnly = path;
}


//
// cjPackage Functions
//

CJCORE_API cjBool cjPkgPath_CheckAcceptable(const cjNameChar* pkgPath, 
	cjINT32 pkgAndNameOnly)
{
	if(!pkgPath || !(*pkgPath))
		return cjFalse;

	const cjNameChar* cur = pkgPath;
	cjINT32 dotCount = 0;
	const cjNameChar* lastChar = cjNULL;
	
	while(*cur)
	{
		cjNameChar c = *cur;
		
		if(!cjIsAlphaNumeric(c))
		{
			if(c != _CT('.') &&
				c != _CT('_') 				
			)
			{
				return cjFalse;
			}
		}

		if(c == _CT('.'))
			dotCount++;

		lastChar = cur;
		cur++;
	}

	if(dotCount == 0)
		return cjFalse;

	if(pkgAndNameOnly && dotCount > 1)
		return cjFalse; // only allowed 'pkgName.objName'

	if(*lastChar == cjNULL || *lastChar == '.')
		return cjFalse;

	return cjTrue;
}

