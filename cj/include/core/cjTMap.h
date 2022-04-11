
#ifndef _CJTMAP_H_
#define _CJTMAP_H_

#include "cjTAry.h"
//
//
//template< class TKey, class TVal, class THashFunc=cjTHashFunc<TKey> > 
//class cjTMapBase
//{
//protected:
//	class cjTPair
//	{
//	public:
//		cjTPair(const TKey& key, const TVal& val) : m_key(key), m_value(val) {}
//		cjTPair() {}
//
//		fnBool operator==(const cjTPair& rhs) const
//			{ return m_key == rhs.m_key && m_value == rhs.m_value; }
//		fnBool operator!=(const cjTPair& rhs) const
//			{ return m_key != rhs.m_key || m_value != rhs.m_value; }
//
//		INT					m_hashNext;
//
//		TKey				m_key;
//		TVal				m_value;
//	};
//
//	void Rehash()
//	{
//		INT* newHash = new(fnFMemAlloc) INT[m_hashCount];
//
//		for(INT i=0; i<m_hashCount; i++)
//			newHash[i] = cjTIndex_None;
//
//		for(INT i=0; i<m_pairs.GetSiz(); i++)
//		{
//			cjTPair& pair = m_pairs(i);
//			INT hash = THashFunc::Hash(pair.m_key) & (m_hashCount-1);
//
//			pair.m_hashNext = newHash[hash];
//			newHash[hash] = i;
//		}
//
//		if(m_hash)
//			operator delete [](m_hash, fnFMemAlloc);
//
//		m_hash = newHash;
//	}
//
//	void Relax()
//	{
//		while(m_hashCount > m_pairs.GetSiz() * 2 + 8)
//			m_hashCount /= 2;
//
//		Rehash();
//	}
//
//	TVal& Add(const TKey& key, const TVal& val)
//	{
//		cjTPair& pair = *new(m_pairs) cjTPair(key, val);
//		INT hash = THashFunc::Hash(pair.m_key) & (m_hashCount-1);
//
//		pair.m_hashNext = m_hash[hash];
//		m_hash[hash] = m_pairs.GetSiz()-1;
//		
//		if(m_hashCount*2+8 < m_pairs.GetSiz())
//		{
//			m_hashCount *= 2;
//			Rehash();
//		}
//
//		return pair.m_value;
//	}
//
//
//public:
//	const static INT					DEFAULT_HASH_CNT = 8;
//
//	cjTMapBase() 
//		: m_hash(NULL), m_hashCount(DEFAULT_HASH_CNT)
//	{ 
//		Rehash(); 
//	}
//
//	cjTMapBase(const cjTMapBase& rhs) 
//		: m_pairs(rhs.m_pairs), m_hash(NULL), m_hashCount(rhs.m_hashCount)
//	{
//		Rehash();
//	}
//
//	~cjTMapBase()
//	{
//		if(m_hash)
//		{
//			operator delete [](m_hash, fnFMemAlloc);
//			m_hash = NULL;
//		}
//		m_hashCount = 0;
//	}
//
//	cjTMapBase& operator=(const cjTMapBase& rhs)
//	{
//		m_pairs = rhs.m_pairs;
//		m_hashCount = rhs.m_hashCount;
//
//		Rehash();
//
//		return *this;
//	}
//
//	fnBool operator==(const cjTMapBase<TKey, TVal>& rhs) const
//	{
//		if(m_pairs.GetSiz() != rhs.m_pairs.GetSiz())
//			return 0;
//
//		for(INT i=0; i<m_pairs.GetSiz(); i++)
//			if(m_pairs(i) != rhs.m_pairs(i))
//				return 0;
//
//		return 1;
//	}
//
//	fnBool operator!=(const cjTMapBase<TKey, TVal>& rhs) const
//	{
//		return !(operator==(rhs));
//	}
//
//	void Clear()
//	{
//		m_pairs.RemoveAll();
//		m_hashCount = DEFAULT_HASH_CNT;
//
//		Rehash();
//	}
//
//	// management
//	TVal& Set(const TKey& key, const TVal& val)
//	{
//		for(INT i=m_hash[(THashFunc::Hash(key) & (m_hashCount-1))]; i!=cjTIndex_None; i=m_pairs(i).m_hashNext)
//		{
//			if(m_pairs(i).m_key == key)
//				return m_pairs(i).m_value = val;
//		}
//
//		return Add(key, val);
//	}
//
//	INT Remove(const TKey& key) 
//	{
//		INT count=0;
//
//		for(INT i=m_pairs.GetSiz()-1; i>=0; i--)
//		{
//			if(m_pairs(i).m_key == key)
//			{
//				m_pairs.Remove(i); 
//				count++; 
//			}
//		}
//
//		if(count)
//			Relax();
//
//		return count;
//	}
//
//	TVal* Find(const TKey& key)
//	{
//		for(INT i=m_hash[(THashFunc::Hash(key) & (m_hashCount-1))]; 
//			i!=cjTIndex_None; i=m_pairs(i).m_hashNext)
//		{
//			if(m_pairs(i).m_key == key)
//				return &m_pairs(i).m_value;
//		}
//		return NULL;
//	}
//
//	const TVal* Find(const TKey& key) const
//	{
//		for(INT i=m_hash[(THashFunc::Hash(key) & (m_hashCount-1))]; 
//			i!=cjTIndex_None; i=m_pairs(i).m_hashNext)
//		{
//			if(m_pairs(i).m_key == key)
//				return &m_pairs(i).m_value;
//		}
//		return NULL;
//	}
//
//
//	// for sort
//		// 1,2,3,4,5
//    static int CDECL SortCompareAscending(const void *inA, const void *inB)
//    {
//        const cjTPair* a = (const cjTPair*)inA;
//        const cjTPair* b = (const cjTPair*)inB;
//
//        if(a->m_key == b->m_key)
//            return 0;
//        else if(a->m_key > b->m_key)
//            return 1;
//        else
//            return -1;
//    }
//		// 5,4,3,2,1
//    static int CDECL SortCompareDescending(const void *inA, const void *inB)
//    {
//        const cjTPair* a = (const cjTPair*)inA;
//        const cjTPair* b = (const cjTPair*)inB;
//
//        if(a->m_key == b->m_key)
//            return 0;
//        else if(a->m_key > b->m_key)
//            return -1;
//        else
//            return 1;
//    }
//
//	void Sort(fnBool reverse=fnFalse)
//	{
//        if(m_pairs.GetSiz())
//        {
//            if(reverse)
//                fnQSort(&m_pairs(0), m_pairs.GetSiz(), sizeof(cjTPair), SortCompareDescending);
//            else
//                fnQSort(&m_pairs(0), m_pairs.GetSiz(), sizeof(cjTPair), SortCompareAscending);
//
//            Rehash();
//        }
//	}
//
//	class cjTIterator
//	{
//	public:
//		cjTIterator(cjTMapBase& map) : m_map(map), m_pairs(map.m_pairs), m_index(0), m_removed(0) {}
//		~cjTIterator()					{ if(m_removed) m_map.Relax(); }
//		void operator++()				{ ++m_index; }
//		void erase()					{ m_pairs.Remove(m_index--); m_removed++; }
//
//		operator fnBool() const			{ return m_index < m_pairs.GetSiz(); }
//		TKey& Key() const				{ return m_pairs(m_index).m_key; }
//		TVal& Val() const				{ return m_pairs(m_index).m_value; }
//
//	private:
//		cjTMapBase&						m_map;
//		cjTArray<cjTPair>&				m_pairs;
//		INT								m_index;
//		INT								m_removed;
//	};
//
//	class cjTConstIterator 
//	{
//	public:
//		cjTConstIterator(const cjTMapBase& map) : m_map(map), m_pairs(map.m_pairs), m_index(0) {}
//		~cjTConstIterator() {}
//
//		void operator++()				{ ++m_index; }
//		operator fnBool() const			{ return m_index < m_pairs.GetSiz(); }
//		const TKey& Key() const			{ return m_pairs(m_index).m_key; }
//		const TVal& Val() const			{ return m_pairs(m_index).m_val; }		
//
//	private:
//		const cjTMapBase&				m_map;
//		const cjTArray<cjTPair>&		m_pairs;
//		INT								m_index;
//	};
//
//protected:
//	cjTArray<cjTPair>					m_pairs;
//	INT*								m_hash;
//	INT									m_hashCount;
//
//	friend class cjTIterator;
//	friend class cjTConstIterator;
//};
//
//
//template<class TKey, class TVal >
//class cjTMap : public cjTMapBase<TKey, TVal>
//{
//public:
//	cjTMap& operator=(const cjTMap& rhs)
//	{
//		cjTMapBase<TKey, TVal>::operator=(rhs);
//		return *this;
//	}
//
//	INT GetSiz() { return this->m_pairs.GetSiz(); }
//};


#endif

