
/*
        cjMem.cpp

	1. no use cjCorePCH.h
*/


#include "cjPlatform.h"
#include "cjMem_Comm.h"

//#include <exception> // for std::bad_alloc
//#include <new>


CJCORE_API unsigned int		cjGIsMemBaseInit = 0;
//CJCORE_API 
unsigned int				cjGIsMemStaticInit = 0;


#ifdef _CJMEM_DEBUG_

#define CJMEM_PREFIX		0xefeffefe
#define CJMEM_SUFFIX		0xfefeefef

static pthread_mutex_t		gs_cjMemDbgChunkStoreMutex;

//
// cjMemDbgChunk
//

#pragma pack(push, 1)

struct cjMemDbgChunk
{
	void*			m_start;

	unsigned int	m_time;
	const char*		m_tag;
	const char*		m_name;
	const char*		m_func;
	unsigned short	m_line;	
	unsigned short  m_flag;
	unsigned int	m_index;
	cjMemSizT		m_siz;	
	
	static cjMemDbgChunk* GetChunkFromRawMem(void* mem);
	inline static cjMemSizT CalcChunkedSiz(cjMemSizT siz);
	static cjMemDbgChunk* Generate(void* givenMem, unsigned int ary, cjMemSizT siz, const char* tag,
	const char* name, const unsigned int line, const char* func);
	
	static cjUINT32			ms_allocIndex;
};

#pragma pack(push, pop)


cjUINT32 cjMemDbgChunk::ms_allocIndex;

cjMemDbgChunk* cjMemDbgChunk::GetChunkFromRawMem(void* mem)
{
	unsigned char* cur = (unsigned char*)mem;

	cur -= sizeof(CJMEM_PREFIX);
		
	if(*((unsigned int*)cur) != CJMEM_PREFIX)
	{
		assert(0);
		return NULL;
	}

	cur -= sizeof(cjMemDbgChunk);

	return (cjMemDbgChunk*)cur;
}
	
inline cjMemSizT cjMemDbgChunk::CalcChunkedSiz(cjMemSizT siz)
{
	return siz ? sizeof(cjMemDbgChunk) + sizeof(CJMEM_PREFIX) + siz + sizeof(CJMEM_SUFFIX) : 0;
}

cjMemDbgChunk* cjMemDbgChunk::Generate(void* givenMem, unsigned int ary, cjMemSizT siz, const char* tag,
	const char* name, const unsigned int line, const char* func)
{
	cjMemSizT siz_tot = CalcChunkedSiz(siz);

	if(!siz_tot)
		return NULL;

	void* mem = givenMem ? givenMem : malloc(siz_tot);
	//char* ret = NULL;
		
	cjMemDbgChunk* chunk = (cjMemDbgChunk*)mem;
	chunk->m_time = (unsigned int)siz; // (unsigned int)-1;
	chunk->m_tag = tag;
	chunk->m_name = name;
	chunk->m_func = func;
	chunk->m_line = (unsigned short)line;

	unsigned short fl = ary ? 1:0;
				   fl |= cjGIsMemStaticInit ? 0x0 : 0x10;
	chunk->m_flag = fl;
	chunk->m_index = ++ms_allocIndex;
	chunk->m_siz = siz;		
		
	char* c = ((char*)mem) + sizeof(cjMemDbgChunk);
	*(unsigned int*)c = CJMEM_PREFIX;
	c += sizeof(CJMEM_PREFIX);

	chunk->m_start = c;
	c += siz;

	*((unsigned int*)c) = CJMEM_SUFFIX;

	return chunk;
}

cjMemSizT cjMemGetSiz(void* m)
{
	if(!m) return 0;

//	cjBYTE* s = ((cjBYTE*)m - (sizeof(CJMEM_PREFIX) + sizeof(cjMemSizT)));
//	cjMemSizT t = *((cjMemSizT*)s);
	return *((cjMemSizT*)((cjBYTE*)m - (sizeof(CJMEM_PREFIX) + sizeof(cjMemSizT))));
}


void cjMemDbgChunkStore_DestructorOnAtExit();


struct cjMemDbgChunkStore
{	
	const static unsigned int HASH_BUCKET = 8096;

	struct Chunk
	{	
		void* operator new(cjMemSizT siz, cjMemDbgChunkStore* p)
		{
			Chunk* inst = NULL;

			if(p->m_free)
			{
				inst = p->m_free;
				p->m_free = p->m_free->m_nextFree;				
			}
			else
			{
				inst = (Chunk*)malloc(siz);
			}

			return inst;
		}

		void operator delete(void* mem, cjMemDbgChunkStore* p)
		{
			Chunk* del = (Chunk*)mem;

			if(p->m_free)
				del->m_nextFree = p->m_free;
			else
				del->m_nextFree = NULL;				

			p->m_free = del;
		}

		Chunk()
		{
			m_memDbg = NULL;
			m_nextHash = NULL;
			m_nextFree = NULL;
		}

		// ~Chunk(){} // not called, ever

		cjMemDbgChunk*			m_memDbg;

		Chunk*					m_nextHash;
		Chunk*					m_nextFree;
	};
 
	cjMemDbgChunkStore()
	{
		pthread_mutex_init(&gs_cjMemDbgChunkStoreMutex, NULL);
		memset(m_hash, 0, sizeof(m_hash));
		m_free = NULL;
		m_numRemoveFailed = 0;

		cjOutputDebugString("[cjMemDbgChunkStore] init \n");
	}

	void Finalize()
	{
		Shrink();

		// force delete leaked memory block

		// pthread_mutex_lock(&gs_cjMemDbgChunkStoreMutex);
		//for(unsigned int i=0; i<HASH_BUCKET; i++)
		//{
		//	Chunk* st = m_hash[i];

		//	Chunk* prev = 0;

		//	while(st)
		//	{
		//		Chunk* del = st;
		//						
		//		if(del->m_memDbg->m_flag & 0x10)
		//		{
		//			// do not free for global allocation instance that is called new
		//			prev = del;
		//			del = 0;					
		//		}
		//		else
		//		{
		//			// delete
		//			if(prev)
		//			{
		//				prev->m_nextHash = del->m_nextHash;
		//			}
		//			else
		//			{
		//				m_hash[i] = del->m_nextHash;
		//				prev = m_hash[i];
		//			}
		//		}

		//		st = st->m_nextHash;
		//						
		//		if(del)
		//		{
		//			// del->~Chunk();
		//			free((void*)del);
		//		}
		//	}
		//}
		//
		// pthread_mutex_unlock(&gs_cjMemDbgChunkStoreMutex);

		ShowLeak();
	}

	~cjMemDbgChunkStore()
	{	
		Shrink();
		pthread_mutex_destroy(&gs_cjMemDbgChunkStoreMutex);
		cjOutputDebugString("[~cjMemDbgChunkStore] shutdown \n");
	}
	
	void Add(cjMemDbgChunk* in)
	{
		if(!in)
			return;

//		in->m_index = m_index++;

		unsigned int hash_num = calcHash(in);
		Chunk* inst = new(this) Chunk();

		inst->m_memDbg = in;
		inst->m_nextHash = m_hash[hash_num];

		m_hash[hash_num] = inst;	
	}

	Chunk* Find(cjMemDbgChunk* in, Chunk** rstPrev, unsigned int* rstHash)
	{
		if(!in)
			return NULL;

		unsigned int hash_num = calcHash(in);
		if(rstHash)
			*rstHash = hash_num;
		
		Chunk* st = m_hash[hash_num];

		Chunk* prev = NULL;

		while(st)
		{
			if(st->m_memDbg == in)
			{
				if(rstPrev)
					*rstPrev = prev;

				return st;
			}

			prev = st;
			st = st->m_nextHash;
		}

		return NULL;
	}

	void Remove(cjMemDbgChunk* in)
	{
		if(!in)
			return;

		Chunk* prev;
		unsigned int hash_num;

		Chunk* rst = Find(in, &prev, &hash_num);

		if(rst)
		{
			if(prev)
				prev->m_nextHash = rst->m_nextHash;
			else
				m_hash[hash_num] = rst->m_nextHash;

			// delete(this) rst;
			rst->operator delete(rst, this);
		}
		else
		{
			/*
				case #0 
				 - new allocate from global allocation instance
				 - _StaticMemBaseInit_ called, _StaticMemInit_ is not called
				 - cjGIsMemBaseInit		=1 / cjGIsMemStaticInit	=0
				 - global allocated unit : std::vector<void*> gs_xxx created
				 - memory block flag of gs_xxx is set to (0x10, global allocated)
				 - _StaticMemInit_() is called
				 - gs_xxx.push_back(y)
				 - memory block flag of newly inserted item ('y') is normal
				 - starting application exit
				 - cjMem::_StaticMemShutdown_() called
				 - gs_cjMemDbgChunkStore->Finalize() will destroy all the memory block except flaged(0x10)
				 - item of gs_vector<> gs_xxx is destroyed by finalize
				 - starting atexit for global allocation 
				 - gs_xxx is trying to delete its item ('y')
				 - but that item is already destroyed..
				 - and called Remove(), hitted here..
			*/
			m_numRemoveFailed++;
		}
	}

	inline unsigned int calcHash(cjMemDbgChunk* in)
    {
#ifdef CJ_P32
		return ((unsigned int)in) % HASH_BUCKET;
#elif defined(CJ_P64)
		return (unsigned int)((((size_t)in) >> 3) % HASH_BUCKET);
#else
	#error "unknown CJ_PXX"
#endif
	}

	void Shrink()
	{
		pthread_mutex_lock(&gs_cjMemDbgChunkStoreMutex);
		Chunk* cur = m_free;

		while(cur)
		{
			Chunk* del = cur;
			cur = cur->m_nextFree;

			// del->~Chunk();
			free((void*)del);
		}

		m_free = NULL;
		pthread_mutex_unlock(&gs_cjMemDbgChunkStoreMutex);
	}

	void ShowLeak()
	{
        pthread_mutex_lock(&gs_cjMemDbgChunkStoreMutex);
            unsigned int numLeak = 0;
			unsigned int numGlobalAllocated = 0;

            char buf[256] = {0};
			for(unsigned int i=0; i<HASH_BUCKET; i++)
			{
				Chunk* st = m_hash[i];
				while(st)
				{
					if(!(st->m_memDbg->m_flag & 0x10)) // called new form global allocated
					{
						numLeak++;
						cjMemDbgChunk* chunk = st->m_memDbg;
						sprintf(buf, "%s(%d) : fn=%s , %d byte !! leak(%d)!!  ..\n", chunk->m_name ? chunk->m_name : "none",
							chunk->m_line, chunk->m_func ? chunk->m_func : "none", (unsigned int)chunk->m_siz, chunk->m_index);
	#if defined(CJ_PLATFORM_WIN32)
						OutputDebugString(buf);
	#else
						printf("%s",buf);
	#endif
					}
					else
					{
						numGlobalAllocated++;
					}

					st = st->m_nextHash;
				}
			}
        
	#if defined(CJ_PLATFORM_WIN32)
			if(numLeak)
			{
				sprintf(buf,"[MemoryReport]  GlobalAllocated UnFree(%d), Leak(%d),  Total UnFreed(%d).. \n", 
					numGlobalAllocated, numLeak - numGlobalAllocated, numLeak);
				OutputDebugStringA(buf);
			}

			if(m_numRemoveFailed)
			{
				sprintf(buf,"[MemoryReport] RemoveFailed Count(%d).. \n", m_numRemoveFailed);
				OutputDebugStringA(buf);
			}
	#else
			if(numLeak)
			{
				printf(buf,"[MemoryReport]  GlobalAllocated UnFree(%d) , Leak(%d), Total UnFreed(%d).. \n", 
					numGlobalAllocated, numLeak - numGlobalAllocated, numLeak);
			}
			
			if(m_numRemoveFailed)
			{
				printf("[MemoryReport] RemoveFailed Count(%d).. \n", m_numRemoveFailed);
			}
	#endif
                
		pthread_mutex_unlock(&gs_cjMemDbgChunkStoreMutex);
	}

	Chunk*					m_hash[HASH_BUCKET];
	Chunk*					m_free;
	cjUINT32				m_numRemoveFailed;

public:
//	unsigned int			m_index;
};

static cjMemDbgChunkStore*	gs_cjMemDbgChunkStore = NULL;


void cjMemDbgChunkStore_DestructorOnAtExit()
{
	if(gs_cjMemDbgChunkStore)
	{
		gs_cjMemDbgChunkStore->~cjMemDbgChunkStore();
		gs_cjMemDbgChunkStore = 0;
	}		
	// no need calling free or delete
	// because gs_cjMemDbgChunkStore is allocated global allocation area
}

void* operator new (size_t siz, cjMemDbgChunkStore* m)
{
	return m;
}

// just calling destructor
void operator delete (void* , cjMemDbgChunkStore* )
{
}

#endif


//
// cjMemAllocate/cjMemReAllocate/cjMemDeAllocate
//

// Allocate

#ifdef _CJMEM_DEBUG_

static void* cjMemAllocate(unsigned int ary, cjMemSizT siz, const char* tag=NULL,
	const char* name=NULL, unsigned int line=0, const char* func=NULL)
{
	if(!siz)
	{
		cjAssert(0);
		return NULL;
	}

	pthread_mutex_lock(&gs_cjMemDbgChunkStoreMutex);

	cjMemDbgChunk* chunk = cjMemDbgChunk::Generate(NULL, ary, siz, tag, name, line, func);
	gs_cjMemDbgChunkStore->Add(chunk);

	pthread_mutex_unlock(&gs_cjMemDbgChunkStoreMutex);

	return (void*)chunk->m_start;
}

#else

static void* cjMemAllocate(unsigned int ary, cjMemSizT siz, const char* tag=NULL)
{
	if(!siz)
		return NULL;

	return malloc(siz);
}

#endif


// Deallocate
static void cjMemDeAllocate(unsigned int /*ary*/, void* mem)
{
	if(!mem)
		return;

#ifdef _CJMEM_DEBUG_
	void* del = NULL;

    cjMemDbgChunk* chunkHdr = (cjMemDbgChunk*)((char*)mem-(sizeof(cjMemDbgChunk)+sizeof(CJMEM_PREFIX)));
    
//    unsigned int pre = *((unsigned int*)((char*)mem - sizeof(CJMEM_PREFIX)));
//    unsigned int suf = *((unsigned int*)((char*)mem + chunkHdr->m_siz));
    
	if( (*((unsigned int*)((char*)mem - sizeof(CJMEM_PREFIX))) == CJMEM_PREFIX) &&
		(*((unsigned int*)((char*)mem + chunkHdr->m_siz)) == CJMEM_SUFFIX))
	{
		del = (char*)mem - (sizeof(cjMemDbgChunk) + sizeof(CJMEM_PREFIX));
		pthread_mutex_lock(&gs_cjMemDbgChunkStoreMutex);
			gs_cjMemDbgChunkStore->Remove((cjMemDbgChunk*)del);
		pthread_mutex_unlock(&gs_cjMemDbgChunkStoreMutex);
	}
	else
	{
		// corrupted
		cjAssert(!"memory corruption is occured..");
//		assert(0);
		return;
	}

	free(del);
#else
	free(mem);
#endif
}


// ReAllocate
#ifdef _CJMEM_DEBUG_
static void* cjMemReAllocate(unsigned int ary, void* mem, cjMemSizT siz, const char* tag=NULL,
	const char* name=NULL, unsigned int line=0, const char* func=NULL)
{
	if(!siz)
	{
		cjMemDeAllocate(ary, mem);
		return NULL;
	}

	if(!mem)
		return cjMemAllocate(ary, siz, tag, name, line, func);
	
	pthread_mutex_lock(&gs_cjMemDbgChunkStoreMutex);

		void* rawMem = cjMemDbgChunk::GetChunkFromRawMem(mem);
		gs_cjMemDbgChunkStore->Remove((cjMemDbgChunk*)rawMem);

		cjMemSizT reallocSiz = cjMemDbgChunk::CalcChunkedSiz(siz);
		mem = realloc(rawMem, reallocSiz);

		cjMemDbgChunk* chunk = cjMemDbgChunk::Generate(mem, ary, siz, tag, name, line, func);
		gs_cjMemDbgChunkStore->Add(chunk);

	pthread_mutex_unlock(&gs_cjMemDbgChunkStoreMutex);
		
	return chunk->m_start;	
}
#else
static void* cjMemReAllocate(unsigned int ary, void* mem, cjMemSizT siz, const char* tag)
{
	if(!siz)
	{
		cjMemDeAllocate(ary, mem);
		return NULL;
	}

	if(!mem)
		return cjMemAllocate(ary, siz, tag);

	return realloc(mem, siz);
}
#endif


#ifdef _CJMEM_DEBUG_

void* cjMem::operator new(cjMemSizT siz, const char* tag, const char* dbgFile, unsigned int dbgLine, const char* dbgFunc)
{
	return cjMemAllocate(0, siz, tag, dbgFile, dbgLine, dbgFunc);
}

void* cjMem::operator new[](cjMemSizT siz, const char* tag, const char* dbgFile, unsigned int dbgLine, const char* dbgFunc)
{
	return cjMemAllocate(1, siz, tag, dbgFile, dbgLine, dbgFunc);
}

void cjMem::operator delete(void *mem, const char* /*tag*/, const char* /*dbgFile*/, unsigned int /*dbgLine*/, const char* /*dbgFunc*/)
{
	if(!mem)
		return;

	cjMemDeAllocate(0, mem);
}

void cjMem::operator delete[](void *mem, const char* /*tag*/, const char* /*dbgFile*/, unsigned int /*dbgLine*/, const char* /*dbgFunc*/)
{
    // On GCC the allocated size is passed into operator delete[] 
    // so there is no need for the allocator to save the size of
    // the allocation.
	if(!mem)
		return;

	cjMemDeAllocate(1, mem);
}

#else

void* cjMem::operator new(cjMemSizT siz, const char* tag)
{
	return cjMemAllocate(0, siz);
}

void* cjMem::operator new[](cjMemSizT siz, const char* tag)
{
	return cjMemAllocate(1, siz);
}

void cjMem::operator delete(void *mem, cjMemSizT siz, const char* tag)
{
	if(!mem)
		return;

	cjMemDeAllocate(0, mem);
}

void cjMem::operator delete[](void *mem, cjMemSizT /*stElementSize*/, const char* tag)
{
    // On GCC the allocated size is passed into operator delete[] 
    // so there is no need for the allocator to save the size of
    // the allocation.
	if(!mem)
		return;

	cjMemDeAllocate(1, mem);
}

#endif


void cjMem::operator delete(void *mem)
{
	cjMemDeAllocate(0, mem);
}

void cjMem::operator delete[](void *mem)
{
	cjMemDeAllocate(1, mem);
}


const int cjMemDbgChunkStoreSiz = sizeof(cjMemDbgChunkStore);  // 64776(fd08) byte
unsigned char cjMemDbgChunkStoreInstBuf[cjMemDbgChunkStoreSiz + 100];


cjBool cjMem::_StaticMemBaseInit_()
{
	if(cjGIsMemBaseInit)
		return cjTrue;

#ifdef _CJMEM_DEBUG_
	if(gs_cjMemDbgChunkStore)
		return cjFalse;

	::atexit(cjMemDbgChunkStore_DestructorOnAtExit);
	cjOutputDebugString("[cjMemDbgChunkStore] registered destructor.. \n");

	//placement new
	gs_cjMemDbgChunkStore = ::new ((cjMemDbgChunkStore*)cjMemDbgChunkStoreInstBuf) cjMemDbgChunkStore;

#endif

//	cjAtomicInc(cjGIsMemBaseInit);  // cjGIsMemBaseInit = 1;
	cjGIsMemBaseInit = 1;

	return cjTrue;
}


cjBool cjMem::_StaticMemInit_()
{
	if(cjGIsMemStaticInit)
		return cjFalse;

	_StaticMemBaseInit_();

//	cjAtomicInc(cjGIsMemStaticInit);
	cjGIsMemStaticInit = 1;

	return cjTrue;
}


void cjMem::_StaticMemShutdown_()
{
	if(cjGIsMemBaseInit == 0)
		return;

#ifdef _CJMEM_DEBUG_
	gs_cjMemDbgChunkStore->Finalize();

// do not call destructor and destory, allocated in global allocation area
//	gs_cjMemDbgChunkStore->~cjMemDbgChunkStore();
//	gs_cjMemDbgChunkStore = 0;
#endif

	//cjAtomicDec(cjGIsMemBaseInit);
	//cjAtomicDec(cjGIsMemStaticInit);

	//cjAssert(cjGIsMemBaseInit == 0);
	//cjAssert(cjGIsMemStaticInit == 0);

//	cjGIsMemBaseInit = 0;
	cjGIsMemStaticInit = 0;
}

void cjMem::_ShrinkMem()
{
#ifdef _CJMEM_DEBUG_
	if(gs_cjMemDbgChunkStore)
		gs_cjMemDbgChunkStore->Shrink();
#endif
}


#ifdef _CJMEM_DEBUG_
void* cjMem::_MAlloc(cjMemSizT siz, void* , const char* tag, const char* dbgFile, unsigned int dbgLine, const char* dbgFunc)
{
	return cjMemAllocate(0, siz, tag, dbgFile, dbgLine, dbgFunc);
}
#else
void* cjMem::MAlloc(cjMemSizT siz)
{
	return cjMemAllocate(0, siz);
}
#endif


#ifdef _CJMEM_DEBUG_
void* cjMem::_MReAlloc(void* mem, cjMemSizT siz, const char* tag, const char* dbgFile, unsigned int dbgLine, const char* dbgFunc)
{
	return cjMemReAllocate(0, mem, siz, tag, dbgFile, dbgLine, dbgFunc);
}
#else
void* cjMem::ReAlloc(void* mem, cjMemSizT siz)
{
	return cjMemReAllocate(0, mem, siz);
}
#endif


void cjMem::_MDeAlloc(void* ptr, void*)
{
	if(!ptr)
		return;

	cjMemDeAllocate(0, ptr);
}


//
// memory functions
//

CJCORE_API void cjMemSet(void* dst, unsigned char c, cjMemSizT count)
{
	memset(dst, c, count);
}

CJCORE_API void cjMemZero(void* dst, cjMemSizT count)
{
	memset(dst, 0, count);
}

CJCORE_API void cjMemCpy(void* dst, const void* src, cjMemSizT count)
{
	memcpy(dst, src, count);
}

CJCORE_API cjMemSizT cjMemCpy(void* dst, cjMemSizT dstSiz, const void* src, cjMemSizT count)
{
    if(dstSiz < count)
        count = dstSiz;

	memcpy(dst, src, count);
	return count;
}

CJCORE_API void* cjMemMove(void* dst, const void* src, cjMemSizT count)
{
	return memmove(dst, src, count);
}

CJCORE_API cjINT32 cjMemCmp(const void* cmp1, const void* cmp2, cjMemSizT count)
{
	return memcmp(cmp1, cmp2, count);
}

void* cjMemAllocDefault(cjSiz_t siz, void* owner=0)
{
	return cjMem::_MAlloc(siz, owner);
}

void cjMemDeallocDefault(void* mem, void* owner=0)
{
	cjMem::_MDeAlloc(mem, owner);
}


CJCORE_API void* cjFastMemAlloc(cjSiz_t siz)
{
	return cjMem::_MAlloc(siz);
}

CJCORE_API void cjFastMemDeAlloc(void* mem)
{
	cjMem::_MDeAlloc(mem);
}

