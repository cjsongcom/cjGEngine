
#ifndef _CJTRESCACHE_H_
#define _CJTRESCACHE_H_


template<class TResCacheBase>
class cjTResCacheContainer
{
public:
	cjTResCacheContainer(cjUINT32 bucketSiz=2048);
	~cjTResCacheContainer();

	void Add(TResCacheBase* resCache); // return next resCache
	void Remove(TResCacheBase* resCache);
	void RemoveAll();
    cjUINT32 GetBucketSiz() { return m_bucketSiz; }

	TResCacheBase* GetFirst();

	TResCacheBase* Find(cjResCacheID cacheID);
	TResCacheBase* FindPrevBySequence(TResCacheBase* resCache);

protected:
    cjUINT32                            m_bucketSiz;
	TResCacheBase*						m_resCacheList;
	TResCacheBase**						m_resCacheHash;
};


//
// for prevent inlining class member function
//

template<class TResCacheBase>
cjTResCacheContainer<TResCacheBase>::cjTResCacheContainer(cjUINT32 bucketSiz)
{
	m_resCacheList = cjNULL;
    m_bucketSiz = bucketSiz;
    m_resCacheHash = (TResCacheBase**)cjMemAlloc(sizeof(TResCacheBase*) * m_bucketSiz);
	cjMemZero(m_resCacheHash, sizeof(TResCacheBase*) * m_bucketSiz);
}

template<class TResCacheBase>
cjTResCacheContainer<TResCacheBase>::~cjTResCacheContainer()
{
	RemoveAll();
    
    if(m_resCacheHash)
    {
		cjMemDeAlloc(m_resCacheHash);
        m_resCacheHash = cjNULL;
    }
}

template<class TResCacheBase>
void cjTResCacheContainer<TResCacheBase>::Add(TResCacheBase* resCache)
{
	if(!resCache)
		return;
    
    cjFResCacheLinkInfo* cli = resCache->GetResCacheLinkInfo();

	cli->m_nextRes = m_resCacheList;
	m_resCacheList = resCache;

	cjUINT32 hashIndex = cjResGetHashIndex(resCache->m_cacheID) % m_bucketSiz;

	cli->m_container = this;
    
	cli->m_hashIndex = hashIndex;
	cli->m_hashNext = m_resCacheHash[hashIndex];

	m_resCacheHash[hashIndex] = resCache;
}

template<class TResCacheBase>
void cjTResCacheContainer<TResCacheBase>::Remove(TResCacheBase* resCache)
{
	if(!resCache)
		return;
    
	TResCacheBase* prev = FindPrevBySequence(resCache);
    struct cjFResCacheLinkInfo* curCLI = resCache->GetResCacheLinkInfo();
	
	if(prev)
		prev->GetResCacheLinkInfo()->m_nextRes = curCLI->m_nextRes;
	else
	{
		cjAssert(m_resCacheList==resCache);
		m_resCacheList = (TResCacheBase*)curCLI->m_nextRes;
	}

	// remove from hash list
	cjUINT32 hashIndex = curCLI->m_hashIndex;
	TResCacheBase* hashNext = (TResCacheBase*)curCLI->m_hashNext;
	
	if(m_resCacheHash[hashIndex] != resCache)
	{
		TResCacheBase* curHash = m_resCacheHash[hashIndex];
		prev = NULL;

		while(curHash && curHash->GetResCacheLinkInfo()->m_hashNext != resCache)
		{
			prev = curHash;
			curHash =(TResCacheBase*)curHash->GetResCacheLinkInfo()->m_hashNext;
        }

		if(curHash == resCache)
		{
			if(prev)
            {
				prev->GetResCacheLinkInfo()->m_hashNext = hashNext;
            }
			else
            {
				m_resCacheHash[hashIndex] = hashNext;
            }
		}
	}
	else
	{
		m_resCacheHash[hashIndex] = hashNext;
	}
}

template<class TResCacheBase>
void cjTResCacheContainer<TResCacheBase>::RemoveAll()
{
	while(m_resCacheList)
		m_resCacheList->DelInst();

	// this will update m_resCacheList by m_renDev->RemoveResCache() at cjXXResCache::~cjXXResCache()
	// so no need to call m_resCacehList->Next

	cjAssert(m_resCacheList == cjNULL);
}

template<class TResCacheBase>
TResCacheBase* cjTResCacheContainer<TResCacheBase>::GetFirst()
{
	return m_resCacheList;
}

template<class TResCacheBase>
TResCacheBase* cjTResCacheContainer<TResCacheBase>::Find(cjResCacheID cacheID)
{
	if(!cacheID)
		return NULL;

	cjUINT32 hashIndex = cjResGetHashIndex(cacheID) % m_bucketSiz;
	TResCacheBase* cur = m_resCacheHash[hashIndex];

	while(cur)
	{
		if(cur->GetCacheID() == cacheID)
			return cur;

		cur = (TResCacheBase*)(cur->GetResCacheLinkInfo()->m_hashNext);
	}

	return NULL;
}

template<class TResCacheBase>
TResCacheBase* cjTResCacheContainer<TResCacheBase>::FindPrevBySequence(
	TResCacheBase* resCache)
{
	if(!resCache || !m_resCacheList)
		return NULL;

	TResCacheBase* cur = m_resCacheList;
	TResCacheBase* prev = NULL;

	if(m_resCacheList != resCache)
	{
        cjFResCacheLinkInfo* curCli = cur ? cur->GetResCacheLinkInfo() : NULL;
    
		while(curCli->m_nextRes != resCache)
		{
			prev = cur;
            
			cur = (TResCacheBase*)curCli->m_nextRes;
            curCli = cur->GetResCacheLinkInfo();
		}

		if(cur != resCache)
			return NULL;
	}

	return prev;
}

#endif
