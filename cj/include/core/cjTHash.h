
#ifndef _CJTHASH_H_
#define _CJTHASH_H_


template<class TKey>
struct cjTHashCompare
{
	typedef typename cjTypeTrait<TKey>::HPrm	_Key;

	cjBool operator()(_Key lhs, _Key rhs) const
	{
		return lhs == rhs;
	}
};

template<class TKey>
struct cjTHashCompareNCase
{
	typedef typename cjTypeTrait<TKey>::HPrm	_Key;

	cjBool operator()(_Key& lhs, _Key& rhs) const
	{
		return lhs == rhs;
	}
};

template<>
struct cjTHashCompareNCase<cjChar>
{
	cjBool operator()(const cjChar& lhs, const cjChar& rhs) const
	{
		return cjCharUpper(lhs) == cjCharUpper(rhs);
	}
};

template<>
struct cjTHashCompareNCase<cjUni>
{
	cjBool operator()(const cjUni& lhs, const cjUni& rhs) const
	{
		return cjCharUpper(lhs) == cjCharUpper(rhs);
	}
};

typedef cjUINT32 cjTHasherValType;


template<class TKey>
struct cjTHasherPtr
{
	typedef typename cjTypeTrait<TKey>::HPrm			_CPType;

	cjTHasherValType operator()(_CPType key) const
	{
		// http://stackoverflow.com/questions/20953390/what-is-the-fastest-hash-function-for-pointers
        return (cjTHasherValType)(((size_t)key) >> 1); // 3 on 64 bit, 1 on 32 bit
	}
};

template<class TKey>
struct cjTHasher
{
	//cjUINT32 operator()(const TKey* key) const
	//{
	//	// http://stackoverflow.com/questions/20953390/what-is-the-fastest-hash-function-for-pointers
 //       return (cjUINT32)(((size_t)key) >> 1); // 3 on 64 bit, 1 on 32 bit
	//}

	//cjUINT32 operator()(const TKey& key) const
	//{		
	//	return (cjUINT32)key;
	//}

	cjTHasherValType operator()(const cjINT32 key) const
	{		
		return (cjTHasherValType)key;
	}

	cjTHasherValType operator()(const cjTHasherValType key) const
	{		
		return key;
	}

	cjTHasherValType operator()(const cjWORD key) const
	{		
		return (cjTHasherValType)key;
	}
};

template<> struct cjTHasher<cjName>;

template<>
struct cjTHasher<const cjNameChar*>
{
	// http://www.cse.yorku.ca/~oz/hash.html
	// hash(i) = hash(i - 1) * 65599 + str[i]; 
	cjTHasherValType operator()(const cjUni* uni) const
	{
		unsigned int hash = 0; int c;	  

        while((c = *uni++))
            hash = c + (hash << 6) + (hash << 16) - hash;

        return hash;
	}

	cjTHasherValType operator()(const cjChar* mbcs) const
	{
		unsigned int hash = 0; int c;
        while((c = *mbcs++))
            hash = c + (hash << 6) + (hash << 16) - hash;

        return hash;
	}
};

template<>
struct cjTHasher<cjString>
{
	// http://www.cse.yorku.ca/~oz/hash.html
	// hash(i) = hash(i - 1) * 65599 + str[i]; 
	cjTHasherValType operator()(const cjUni* uni) const
	{
		unsigned int hash = 0; int c;	  

        while((c = *uni++))
            hash = c + (hash << 6) + (hash << 16) - hash;

        return hash;
	}

	cjTHasherValType operator()(const cjChar* mbcs) const
	{
		unsigned int hash = 0; int c;
        while((c = *mbcs++))
            hash = c + (hash << 6) + (hash << 16) - hash;

        return hash;
	}
};


template<class TKey>
struct cjTHasherNCase
{
	cjTHasherValType operator()(const TKey& key) const
	{
		cjAssert(!"notimpl");
		return 0;
	}

	cjTHasherValType operator()(const TKey* key) const
	{
        return (cjTHasherValType)(((size_t)key) >> 1); // 3 on 64 bit, 1 on 32 bit
	}

};

template<>
struct cjTHasherNCase<const cjChar*>
{
	cjTHasherValType operator()(const cjChar* mbcs) const
	{
		unsigned int hash = 0; cjChar c;
        while((c = *mbcs++))
            hash = (hash << 6) + (hash << 16) - hash + cjCharUpper(c);

        return hash;
	}
};

template<>
struct cjTHasherNCase<cjChar*>
{
	cjTHasherValType operator()(const cjChar* mbcs) const
	{
		unsigned int hash = 0; cjChar c;
        while((c = *mbcs++))
            hash = (hash << 6) + (hash << 16) - hash + cjCharUpper(c);

        return hash;
	}
};


template<>
struct cjTHasherNCase<const cjUni*>
{
	cjTHasherValType operator()(const cjUni* uni) const
	{
		unsigned int hash = 0; cjUni c;
        while((c = *uni++))
            hash = (hash << 6) + (hash << 16) - hash + cjCharUpper(c);

        return hash;
	}
};

template<>
struct cjTHasherNCase<cjUni*>
{
	cjTHasherValType operator()(const cjUni* uni) const
	{
		unsigned int hash = 0; cjUni c;
        while((c = *uni++))
            hash = (hash << 6) + (hash << 16) - hash + cjCharUpper(c);

        return hash;
	}
};

template<>
struct cjTHasherNCase<cjString>
{
	// http://www.cse.yorku.ca/~oz/hash.html
	// hash(i) = hash(i - 1) * 65599 + str[i]; 
	cjTHasherValType operator()(const cjUni* uni) const
	{
		unsigned int hash = 0; cjUni c;

        while((c = *uni++))
            hash = (hash << 6) + (hash << 16) - hash + cjCharUpper(c);

        return hash;
	}

	cjTHasherValType operator()(const cjChar* mbcs) const
	{
		unsigned int hash = 0; cjChar c;
        while((c = *mbcs++))
            hash = (hash << 6) + (hash << 16) - hash + cjCharUpper(c);

        return hash;
	}
};


template<class TVal>
struct cjTHashRemoveByDel
{
	template<class T> struct Remover { void operator()(const TVal& val) const { delete val; } };
	template<class T> struct RemoverNone { void operator()(const TVal& val) const {} };

	typedef typename cjTSelect< 
		cjPtrTypeTraits<TVal>::isPointer, Remover<TVal>, RemoverNone<TVal> 
	>::SelectType RemoveOper;

	void operator()(const TVal& removeItem) const { m_oper(removeItem); }

	RemoveOper						m_oper;
};

template<class TVal>
struct cjTHashDefaultNoneRemove
{
	void operator()(const TVal& /*removeItem*/) const {}
};



//	const cjTPair<TKey, TVal>* srcPair = (const cjTPair<TKey, TVal>*)src;
//	cjTPair<TKey, TVal>* dstPair = (cjTPair<TKey, TVal>*)dst;

//	dstPair->m_key = srcPair->m_key;
//	dstPair->m_val = srcPair->m_val;		
//}

//template<class TKey, class TVal>
//void cjTHashDefaultCloner(void* dst, const void* src)
//{
//	const cjTPair<TKey, TVal>* srcPair = (const cjTPair<TKey, TVal>*)src;
//	cjTPair<TKey, TVal>* dstPair = (cjTPair<TKey, TVal>*)dst;
//
//	dstPair->m_key = srcPair->m_key;
//	dstPair->m_val = srcPair->m_val;		
//}

//const cjTPair<TKey, TVal>* src

//
// cjTHash & cjTHashNCase
//

template<class TKey, class TVal,
	class THashRemover=cjTHashDefaultNoneRemove<TVal>, 
	class THashCmp=cjTHashCompare<TKey>, 
	class THasher=typename cjTSelect< cjPtrTypeTraits<TKey>::isPointer, cjTHasherPtr<TKey>, cjTHasher<TKey> >::SelectType ,
	class TCloner=cjTPairCloner<TKey, TVal, cjTCloneByCopy, cjTCloneByCopy> >
class cjTHash : public cjMem
{
public:
	typedef cjTHash<TKey, TVal, THashRemover, THashCmp, THasher, TCloner> Type;
	typedef cjTPair<TKey, TVal>* Iter;
	typedef cjTPair<TKey, TVal> Pair;

	typedef	TKey				Key;
	typedef TVal				Val;

	typedef typename cjTypeTrait<TKey>::HPrm			_CRType;
	typedef typename cjTypeTrait<TKey>::_Type&			_RType;


	cjTHash(cjUINT32 numHashBucket=24) 
	{
		cjAssert(numHashBucket);
		m_numHashBucket = numHashBucket;

		// deferred until first key-val pair is inserting
		m_hashBucket = 0; 

		m_numPair = 0;
		m_lastInsPair = NULL;
	}
	
	void InitHashBucket()
	{
		if(m_hashBucket)
			return;

		if(m_numHashBucket > 0)
		{
			m_hashBucket = new Pair*[m_numHashBucket]; // Pair : cjMem
			cjMemZero(m_hashBucket, sizeof(Pair*)*m_numHashBucket);
		}
	}

    ~cjTHash()
	{
		RemoveAll();

		if(m_hashBucket)
		{
			delete [] m_hashBucket;
			m_hashBucket = 0;
		}
	}

	//template<class DKey, class DVal, void* DCloner, class DHashRemover, class DHashCmp, class DHasher>
	void CloneTo(cjTHash& dst) const
	{
//		typedef void* (*clonerPFN)(void* );
//		clonerPFN cloner = DCloner;

		//cjAssert((*this).CheckPairList());
		
		dst.RemoveAll();

		if(m_numHashBucket != dst.m_numHashBucket)
		{
			if(dst.m_hashBucket)
			{
				delete [] dst.m_hashBucket;
				dst.m_hashBucket = 0;
			}
		}

		dst.m_numHashBucket = m_numHashBucket;
		dst.InitHashBucket();
				
		cjINT32 clonePairCount = 0;
		cjTPair<TKey, TVal>* prevClonePair = cjNULL;
		TCloner cloner;

		for(cjUINT32 i=0; i<m_numHashBucket; i++)
		{
			const Pair* srcPair = m_hashBucket[i];

			cjTPair<TKey, TVal>* bucketCurPair = dst.m_hashBucket[i];
			cjAssert(!bucketCurPair);

			cjTPair<TKey, TVal>* clonePair = cjNULL;									

			while(srcPair)
			{
				//clonePair = new cjTPair<TKey, TVal>((TKey&)srcPair->m_key, srcPair->m_val); // (cjTCloner(srcPair->m_key), cjTCloner(srcPair->m_val));
				clonePair = new cjTPair<TKey, TVal>();
				cloner(clonePair, srcPair);
				clonePairCount++;
								
				if(prevClonePair)
				{
					prevClonePair->m_seqPrev = clonePair;
					clonePair->m_seqNext = prevClonePair;
				}
						
				if(bucketCurPair)
				{
					bucketCurPair->m_next = clonePair;
				}
				else
				{
					dst.m_hashBucket[i] = clonePair;
				}

				bucketCurPair = clonePair;
				prevClonePair = clonePair;

				srcPair = srcPair->m_next;				
			}
		}

		dst.m_lastInsPair = prevClonePair;

		cjAssert(clonePairCount == m_numPair);
		dst.m_numPair = clonePairCount;				
	}

	cjBool IsEmpty()
	{
		return m_numPair == 0 ? cjTrue : cjFalse;
	}

	cjTHasherValType CalcHash(_CRType key) const
	{
		cjTHasherValType hash = m_hasher(key);
		return hash % (m_numHashBucket-1);
	}

	Pair* Find(_CRType key, TVal* rstVal=NULL) const
	{
		if(m_numPair == 0)
			return cjNULL;

		cjTHasherValType hash = CalcHash(key);
		Pair* rst = m_hashBucket[hash];

		if(!LookupPairByKey(rst, key)) 
			return cjNULL; // rst is last pair at linked list in bucket

		if(rstVal)
			*rstVal = rst->m_val;

		return rst;
	}

	Pair* FindByKeyName(const cjNameChar* name, TVal* rstVal=NULL) const
	{
		if(m_numPair == 0)
			return cjNULL;

		cjTHasherValType hash = cjName_GenHash(name);
		Pair* rst = m_hashBucket[hash];
		
		if(!LookupPairByKeyName(rst, name)) 
			return cjNULL; // rst is last pair at linked list in bucket

		if(rstVal)
			*rstVal = rst->m_val;

		return rst;
	}

	Pair* FindByVal(TVal& val) const cjVerySlow
	{
		for(cjUINT32 hash=0; hash<m_numHashBucket; hash++)
		{
			Pair* cur = m_hashBucket[hash];

			while(cur)
			{
				if(cur->m_val == val)
					return cur;

				cur = cur->m_next;
			}

			hash++;
		}

		return cjNULL;
	}

	void Set(TKey key, TVal& val)
	{
		Pair* pair = Find(key, cjNULL);
		
		if(!pair)
			pair = Insert(key, val);

		pair->m_val = val;
	}

	cjBool LookupPairByKeyName(Pair*& rstPair, const cjNameChar* keyName) const
	{
		while(rstPair)
		{
			if(!cjStrcmp(*(rstPair->m_key), keyName))
				return cjTrue;

			// stop at last valid pair
			if(!rstPair->m_next)
				return cjFalse;

			rstPair = rstPair->m_next;			
		}

		return cjFalse;
	}

	cjBool LookupPairByKey(Pair*& rstPair, _CRType key) const
	{
		while(rstPair)
		{
			if(m_hashComp(rstPair->m_key, key))
				return cjTrue;

			// stop at last valid pair
			if(!rstPair->m_next)
				return cjFalse;

			rstPair = rstPair->m_next;			
		}

		return cjFalse;
	}

	cjBool Remove(_CRType key)
	{
		if(m_numPair == 0)
			return cjFalse;

		cjTHasherValType hash = CalcHash(key);
		Pair* cur = m_hashBucket[hash];
		Pair* prev = cur;

		if(!cur)
			return cjFalse;

		// first hit
		if(m_hashComp(cur->m_key, key))
		{
			m_hashBucket[hash] = cur->m_next;
			_InnerDelete(NULL, cur);

			return cjTrue;			
		}

		prev = cur;	cur = cur->m_next;

		while(cur)
		{
			if(m_hashComp(cur->m_key, key))
			{				
				if(prev)
					prev->m_next = cur->m_next;

				_InnerDelete(prev, cur);

				return cjTrue;
			}

			prev = cur;	cur = cur->m_next;
		}

		return cjFalse;
	}

	static void DefaultRemove(Pair* pair) { delete pair; }

	void RemoveAll()
	{
		if(m_numPair == 0)
			return;

		Pair* cur = m_lastInsPair;
		while(cur)
		{
			Pair* del = cur;
			cur = cur->m_seqNext;

			m_hashRemover(del->m_val);
			delete del;

			m_numPair--;
		}

		cjAssert(m_numPair == 0);
		m_numPair = 0;
		m_lastInsPair = NULL;

		cjMemZero(m_hashBucket, sizeof(Pair*)*m_numHashBucket);
	}

    Pair* Insert(TKey key, const TVal& val)	
	{
		if(!m_hashBucket)
			InitHashBucket();

		cjTHasherValType hash = CalcHash(key);
		Pair* pair = m_hashBucket[hash];
		Pair* newPair = NULL;

		if(pair)
		{
			if(LookupPairByKey(pair, key))
			{
				pair->m_val = val;  // change only value
			}
			else
			{
				// this pair is in end at same hash bucket
				newPair = new Pair(key, val); // Pair : cjMem
				/*newPair = new Pair();

				newPair->m_key = const_cast<wchar_t*>(key);
				newPair->m_val = val;*/

				cjAssert(!pair->m_next);
				pair->m_next = newPair;
			}
		}
		else
		{
			newPair = new Pair(key, val);
			m_hashBucket[hash] = newPair;
		}

		if(newPair)
		{
			UpdateSeqLink(newPair);
			m_numPair++;
		}
			
		return newPair ? newPair : pair;
	}

	cjINT32 GetNum() const { return m_numPair; }
	cjINT32 Count() const { return m_numPair; }

	// for sequence iterate
//	Pair* GetHeadPair() const { return m_}

	Iter GetLast() const { return m_lastInsPair; }
	cjBool GetNext(Iter& pair, Pair*& cur) const 
	{
		if(pair)
		{
			if(cur)
				cur = pair;

			pair = pair->m_seqNext;

			return cjTrue;
		}
		else
			return cjFalse;
	}

	//Pair* GetPrevPair(const Pair* pair) const { return pair ? pair->m_seqPrev : cjNULL; }

	typedef void (*PFTraverseAllPair)(Pair* curPair);
	static void PFDefaultRemovePairValue(Pair* curPair)
	{
		delete curPair->m_val;
		curPair->m_val = cjNULL;
	}

	void TraverseAllPair(PFTraverseAllPair pfTraverser)
	{
		Pair* cur = m_lastInsPair;

		while(cur)
		{
			pfTraverser(cur);
			cur = cur->m_seqNext;
		}
	}

	//cjBool CheckPairList() const
	//{
	//	cjINT32 count = 0;

	//	Pair* cur = m_lastInsPair;
	//	while(cur)
	//	{
	//		count++;
	//		cur = cur->m_seqNext;
	//	}

	//	return count == m_numPair; 
	//}

private:

	void UpdateSeqLink(Pair* pair)
	{
		if(m_lastInsPair)
		{
			m_lastInsPair->m_seqPrev = pair;
			pair->m_seqNext = m_lastInsPair;
		}

		m_lastInsPair = pair;
	}

	void _InnerDelete(Pair* prev, Pair* del)
	{
		if(prev)
			prev->m_next = del->m_next;

		if(del->m_seqPrev)
			del->m_seqPrev->m_seqNext = del->m_seqNext;

		if(del->m_seqNext)
			del->m_seqNext->m_seqPrev = del->m_seqPrev;

		if(m_lastInsPair == del)
		{
			cjAssert(del->m_seqPrev == NULL);
			m_lastInsPair = del->m_seqNext;
		}
		
		m_hashRemover(del->m_val);
		delete del;
		m_numPair--;
		cjAssert(m_numPair > 0);		
	}

private:	
	THasher								m_hasher;
	THashRemover						m_hashRemover;
	THashCmp							m_hashComp;
	cjTHasherValType					m_numHashBucket;
	Pair**								m_hashBucket;		// m_hash[m_sizHash];

	Pair*								m_lastInsPair;
	cjINT32								m_numPair;
};


template<class TKey, class TVal, cjUINT32 TBucketSiz=128>
class cjTHashNCase : public cjTHash< TKey, TVal, cjTHashDefaultNoneRemove<TVal>,
    cjTHashCompareNCase<TKey>, cjTHasherNCase<TKey> >
{
public:
    cjTHashNCase(cjUINT32 bucketSiz=TBucketSiz) : cjTHash<TKey, TVal, cjTHashDefaultNoneRemove<TVal>,
        cjTHashCompareNCase<TKey>, cjTHasherNCase<TKey> >(bucketSiz) {}
};


template<class TKey, class TVal, 	 
	class TCloner=cjTPairCloner<TKey, TVal, cjTCloneByCopy, cjTCloneByCopy>,
	class THashRemover=cjTHashDefaultNoneRemove<TVal>, 
	class THashCmp=cjTHashCompare<TKey>, 
	class THasher= typename cjTSelect<
                      cjPtrTypeTraits<TKey>::isPointer,
                      cjTHasherPtr<TKey>,
                      cjTHasher<TKey>
                      >::SelectType
        >
class cjTSerHash : public cjTHash<TKey, TVal, THashRemover, THashCmp, THasher, TCloner>
{
public:
    typedef cjTHash<TKey, TVal, THashRemover, THashCmp, THasher, TCloner> BaseType;
	typedef cjTSerHash<TKey, TVal, TCloner, THashRemover, THashCmp, THasher> Type;
	typedef cjTPair<TKey, TVal> Pair;
	typedef	TKey				Key;
	typedef TVal				Val;

	cjTSerHash(cjUINT32 numHashBucket) : BaseType(numHashBucket) {}
};


//
// cjTPtrHash
//

template <class TPtrKey, class TVal, class TAllocator=cjTDefaultAlloc<TVal> >
class cjTPtrHashPair : public cjMem
{
public:
	typedef TAllocator Allocator;

	TVal& operator *() { return m_val; }
	const TVal& operator *() const { return m_val; }

	// keep order and type  ptr,ptr,itemType
    cjTPtrHashPair*					m_next;
    TPtrKey							m_key;
    TVal							m_val;
};

typedef cjUINT32					cjPtrHasherValType;

template<class T>
inline cjPtrHasherValType cjGetPtrHash(T*& ptrKey)
{
	// http://stackoverflow.com/questions/20953390/what-is-the-fastest-hash-function-for-pointers
	return (cjPtrHasherValType)(((size_t)ptrKey) >> 1); // 3 on 64 bit, 1 on 32 bit
}

template<class TKey, class TVal, class TAllocator=cjTPtrPoolAlloc<TVal> >
class cjTPtrHash : public cjMem
{
public:
	typedef cjTPtrHash<TKey, TVal, TAllocator>		Type;
	typedef cjTPtrHashPair<TKey, TVal, TAllocator>* Iter;
	typedef cjTPtrHashPair<TKey, TVal, TAllocator>	Pair;
	typedef	TKey									Key;
	typedef TVal									Val;

public:

	cjTPtrHash(cjPtrHasherValType hashBucketSiz=24, cjBool deferCreateHashBucket=cjFalse)
	{
		cjAssert(hashBucketSiz);

		m_hashBucketSiz = hashBucketSiz;
		m_hashBucket = cjNULL;
		m_pairCount = 0;

		if(!deferCreateHashBucket)
			InitHashBucket();
	}

	~cjTPtrHash()
	{
		RemoveAll();

		if(m_hashBucket)
		{
			delete [] m_hashBucket;
			m_hashBucket = cjNULL;
		}
	}

	void InitHashBucket()
	{
		if(m_hashBucket)
			return;

		if(m_hashBucketSiz > 0)
		{
			m_hashBucket = new Pair*[m_hashBucketSiz];
			cjMemZero(m_hashBucket, sizeof(Pair*)*m_hashBucketSiz);
		}
	}

	cjBool IsEmpty() { return m_pairCount == 0 ? cjTrue : cjFalse; }
	cjINT32 Count() const { return m_pairCount; }
	
	void Set(TKey& key, const TVal& val)
	{
		cjPtrHasherValType hash = cjGetPtrHash(key) % m_hashBucketSiz;
		Pair* pair = m_hashBucket[hash];

		while(pair)
		{
			if(key == pair->m_key)
			{
				pair->m_val = val;
				return;
			}

			pair = pair->m_next;
		}

		pair = (Pair*)AllocPair();

		pair->m_key = key;
		pair->m_val = val;
		pair->m_next = m_hashBucket[hash];
		m_hashBucket[hash] = pair;

		m_pairCount++;
	}

	Pair* Find(TKey& key) 
	{
		if(m_pairCount == 0)
			return cjNULL;

		cjPtrHasherValType hash = cjGetPtrHash(key) % m_hashBucketSiz;
		Pair* pair = m_hashBucket[hash];

		while(pair)
		{
			if(key == pair->m_key)
				return pair;

			pair = pair->m_next;
		}

		return pair;
	}

	cjBool GetValue(TKey& key, TVal& val) 
	{
		Pair* pair = Find(key);

		if(!pair)
			return cjFalse;

		val = pair->m_val;

		return cjTrue;
	}

	void _DeletePair(Pair* prev, Pair* del)
	{
		if(prev)
			prev->m_next = del->m_next;

		DeAllocPair(del);
		m_pairCount--;

		cjAssert(m_pairCount > 0);
	}

	void Remove(TKey& key)
	{
		if(m_pairCount == 0)
			return;

		cjPtrHasherValType hash = cjGetPtrHash(key) % m_hashBucketSiz;
		
		Pair* pair = m_hashBucket[hash];
		Pair* prev = cjNULL;

		while(pair)
		{
			if(key == pair->m_key)
			{
				_DeletePair(prev, pair);
				return;
			}

			prev = pair;
			pair = pair->m_next;
		}
	}

	void RemoveAll()
	{
		if(m_pairCount == 0)
			return;

		for(cjPtrHasherValType hash=0; hash<m_hashBucketSiz; hash++)
		{
			Pair* pair = m_hashBucket[hash];

			if(!pair)
				continue;

			while(pair)
			{
				Pair* nextPair = pair->m_next;
				DeAllocPair(pair);

			#ifdef CJDEBUG
				m_pairCount--;
			#endif

				pair = nextPair;
			}

			m_hashBucket[hash] = cjNULL;
		}

		cjAssert(m_pairCount == 0);
		m_pairCount = 0;
	}

	void* AllocPair()
	{
		return Pair::Allocator::Alloc();
	}

	void DeAllocPair(Pair* pair)
	{

//if(cjTypeInfo<TVAL>::HasDestructor())
//		{
//			(pair->m_val).~T();
//		}
//		else
//		{
//#ifdef CJDEBUG
//			cjMemSet((cjBYTE*)pair->m_val, 0xfe, sizeof(TVAL));
//#endif
//		}	

		pair->m_val = cjNULL;
		Pair::Allocator::DeAlloc(pair);
	}

private:
	cjPtrHasherValType						m_hashBucketSiz;
	Pair**								m_hashBucket;

	cjPtrHasherValType						m_pairCount;
};


#endif
