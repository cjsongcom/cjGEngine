
#include "cjCorePCH.h"
#include "cjLibManager.h"


//
// cjObjMem
//

class cjObjMem // do not specify any memory allocator
{
public:
	cjObjMem();
	~cjObjMem();

	void Tick();

	void* Malloc(size_t siz);
	void Free(void* mem);

	void AddMarkDestroy(cjObj* obj);
	void ClearMarkDestroy();

	cjUINT32				m_numAlloc;
	cjUINT32				m_numTotalAllocating;
	cjObj*					m_nextMarkDestroy;

};

static cjObjMem* gs_memObj = NULL;


cjObjMem::cjObjMem() 
{
	m_numAlloc = 0;
	m_numTotalAllocating = 0;
	m_nextMarkDestroy = NULL;	
}

cjObjMem::~cjObjMem()
{
	ClearMarkDestroy();

//	cjLOG("[INFO!][%s] total allocation(%u) / leaked(%d) \n", __CJFUNCTION__, m_numTotalAllocating, m_numAlloc);
//	cjLOG("[cjObjMem] INFO! total allocation(%u) / leaked(%d) \n");

	if(m_numAlloc != 0)
	{
		cjLOG(_CT("[WARN!][%s] leaked cjObj = %d \n"), __CJFUNCTION__, m_numAlloc);
	}
	else
	{
		cjLOG(_CT("[INFO!] no leaked cjObj \n"), __CJFUNCTION__);
	}
}

void cjObjMem::Tick()
{
	ClearMarkDestroy();
}

void* cjObjMem::Malloc(size_t siz)
{
	m_numTotalAllocating++;
	m_numAlloc++;

	return cjMemAllocTag(siz, "cjObj");
}

void cjObjMem::Free(void *mem)
{
	if(!mem)
		return;

#ifdef CJDEBUG
	if( m_numAlloc == 0 )
	{
		cjAssert(!"Error.. heap is corrupted..");
	}
#endif
	
	m_numAlloc--;

	return cjMemDeAlloc(mem);
}

void cjObjMem::AddMarkDestroy(cjObj* obj)
{
	if(!obj)
		return;

	cjAssert(!obj->_m_nextMarkDestroy);

	obj->_m_nextMarkDestroy = m_nextMarkDestroy;
	m_nextMarkDestroy = obj;
}

void cjObjMem::ClearMarkDestroy()
{
	cjObj* curDel = NULL;

	while(m_nextMarkDestroy)
	{
		curDel = m_nextMarkDestroy;
		m_nextMarkDestroy = m_nextMarkDestroy->_m_nextMarkDestroy;

		delete curDel;
	}
}




//
// cjLibManager
//

cjLibManager::LibInfo cjLibManager::ms_lib[CJ_MAX_LIB];
unsigned int cjLibManager::ms_numLib = 0;
unsigned int cjLibManager::ms_numInitializedLib = 0;
unsigned int cjLibManager::ms_procOrder[CJ_MAX_LIB];
cjBool cjLibManager::ms_init = cjFalse;


void cjLibManager::Init()
{
	if(ms_init)
		return;

	cjMem::_StaticMemInit_();
	gs_memObj = ::new cjObjMem;

	cjDeclareForwardTPtrPoolAlloc::Init();

	cjInitParam initPrm;
	cjGetInitParam(initPrm);

	cjINT32 curPkgPathIndex = 0;

	for(;;)
	{
		const cjChar* curPkgPath = initPrm.m_pkgPath[curPkgPathIndex];
		if(!curPkgPath || !*curPkgPath)
			break;

#if cjNameCharIsUnicode
		const cjChar2Uni convPkgPath(curPkgPath);
		cjObj::_ms_cjObjPreLookupPkgFilePaths.PushEmpty(1);	

		const cjUni* u = *convPkgPath;
		cjStrcpy((cjUni*)&cjObj::_ms_cjObjPreLookupPkgFilePaths.Last(), u);

#else
	#error "not impl"
#endif

		curPkgPathIndex++;
		
	}

	ProcessAccumLib();
	
	cjName::StaticInit();	

	cjObj::StaticInit();

	ms_init = true;
		
}

void cjLibManager::Shutdown()
{
	if(!ms_init)
		return;

	cjDeclareForwardTPtrPoolAlloc::ShutDown();

	FinalizeLib();
	
	gs_memObj->ClearMarkDestroy();

	cjObj::StaticShutdown();
	cjName::StaticShutdown();

	delete gs_memObj;
	gs_memObj = NULL;

	cjMem::_StaticMemShutdown_();

	ms_init = false;
}

void cjLibManager::Tick()
{
	if(gs_memObj)
		gs_memObj->Tick();
}

void* cjLibManager::Malloc(size_t siz, const  wchar_t* /*tag*/)
{
#ifdef CJDEBUG
	if(!siz)
	{
		cjAssert(0);
	}
#endif
	return gs_memObj->Malloc(siz);
}

void* cjLibManager::Malloc(size_t siz, const  char* /*tag*/)
{
	return gs_memObj->Malloc(siz);
}

void cjLibManager::Free(void* mem)
{
	return gs_memObj->Free(mem);
}

void cjLibManager::AddLib(const char* libName, InitFn initFn, ShutdownFn shutdownFn, const char* dep)
{
    cjBool alreadyRegistered = cjFalse;
    
    for (cjUINT32 i=0; i<ms_numLib; i++)
    {
        if(!cjStricmp(ms_lib[i].m_name, libName))
        {
            alreadyRegistered = cjTrue;
            break;
        }
    }
    
    if(alreadyRegistered)
    {
        return;
    }
    
	LibInfo& info = ms_lib[ms_numLib];

    cjStrcpy(info.m_name, libName);
//	info.m_name = libName;

    cjStrcpy(info.m_dep, dep);
//info.m_dep = dep;
    
	info.m_initFn = initFn;
	info.m_shutdownFn = shutdownFn;	
	info.m_index = ms_numLib;

	ms_procOrder[ms_numLib] = ms_numLib;

	ms_numLib++;
}

void cjLibManager::RemoveLib(const char* /*libName*/)
{
	
}

void cjLibManager::ProcessAccumLib()
{
	//if(!ms_init)
	//	return;

	if(!ms_numLib)
		return;

	if(ms_numInitializedLib) // found initialized library
	{
        for(unsigned int i=ms_numInitializedLib; i<ms_numLib; i++)
            (*ms_lib[i].m_initFn)();

		ms_numInitializedLib = ms_numLib;
    }
    else
    {
        ComputeProcessOrder();
        
        for(unsigned int i=0; i<ms_numLib; i++)
            (*ms_lib[ms_procOrder[i]].m_initFn)();

		ms_numInitializedLib = ms_numLib;
	}
}

cjBool cjLibManager::ComputeProcessOrder()
{
    if(ms_numLib == 0)
        return cjTrue;
    
    std::vector<LibInfo> originalLibInfoVec;
    
    for(cjUINT32 i=0; i<ms_numLib;i++)
        originalLibInfoVec.push_back(ms_lib[i]);
    
    cjMemSet(ms_lib, 0, sizeof(LibInfo)* CJ_MAX_LIB);
    
    //static const cjChar* libnameOrders[] = { "cjCore", "cjEngine", "cjGame" };
    //static const cjINT32 libnameOrdersNum = sizeof(libnameOrders) / sizeof(libnameOrders[0]);
    
    for(cjINT32 i=0; i<(cjINT32)originalLibInfoVec.size(); i++)
    {
        if(cjStrlen(originalLibInfoVec[i].m_dep) == 0)
        {
            ms_lib[0] = originalLibInfoVec[i];
            break;
        }
    }
    
    for(cjINT32 i=0; i<(cjINT32)originalLibInfoVec.size(); i++)
    {
        if(!cjStricmp("cjCore", originalLibInfoVec[i].m_dep))
        {
            ms_lib[1] = originalLibInfoVec[i];
            break;
        }
    }

    for(cjINT32 i=0; i<(cjINT32)originalLibInfoVec.size(); i++)
    {
        if(!cjStricmp("cjEngine", originalLibInfoVec[i].m_dep))
        {
            ms_lib[2] = originalLibInfoVec[i];
            break;
        }
    }

	for(cjINT32 i=0; i<(cjINT32)originalLibInfoVec.size(); i++)
	{
		if(!cjStricmp("cjGame", originalLibInfoVec[i].m_dep))
		{
			ms_lib[3] = originalLibInfoVec[i];
			break;
		}
	}

    return true;
}

void cjLibManager::FinalizeLib()
{
	if(!ms_init)
		return;

	if(!ms_numLib)
		return;

	if(ms_numInitializedLib)
	{
        for(unsigned int i=0; i<ms_numLib; i++)
            (*ms_lib[i].m_shutdownFn)();

		ms_numInitializedLib = ms_numLib;
    }

	ms_numLib = 0;
	ms_numInitializedLib = 0;
}