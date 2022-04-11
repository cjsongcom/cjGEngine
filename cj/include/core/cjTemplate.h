
#ifndef _CJTEMPLATE_H_
#define _CJTEMPLATE_H_

#include "cjMem.h"
#include "cjTypeInfo.h"


enum cjTIndex 
{
	cjTIndex_None = -1
};

typedef cjINT32	cjAryOff;
enum { cjAryOff_Invalid = cjIndex_None };


template<class T>
class cjTSingleListItem
{
public:
	cjTSingleListItem(T& item, cjTSingleListItem* next)
	{
		m_item = item;
		m_next = next;
	}

	cjTSingleListItem*		m_next;
	T						m_item;
};


//
// cjTCloner
//

enum cjCloneMethod
{
	cjTCloneByCopy		= 0,		// a=b
	cjTCloneByRef		= 1,		// cjRefObj->AddRef()
	cjTCloneByClone		= 2,		// cjSerializeable->Clone()
};


//template<class T>
//T& cjTCloner(const T& src, cjTFLAG flag)
//{
//	if(flag & cjTCloneByClone)
//		return src.Clone();
//
//	return src;
//}

template<typename T> struct cjTRemoveConst { typedef T type; };

template<typename T> struct cjTRemoveConst<T const volatile> 
{ 
	typedef typename cjTRemoveConst<T>::type volatile type; 
};

template<typename T> struct cjTRemoveConst<T volatile> 
{ 
	typedef typename cjTRemoveConst<T>::type volatile type; 
};

template<typename T> struct cjTRemoveConst<T const> 
{ 
	typedef typename cjTRemoveConst<T>::type type; 
};

template<typename T> struct cjTRemoveConst<T&> 
{ 
	typedef typename cjTRemoveConst<T>::type& type; 
};

template<typename T> struct cjTRemoveConst<const T&> 
{ 
	typedef typename cjTRemoveConst<T>::type& type; 
};

template<typename T> struct cjTRemoveConst<T*> 
{ 
	typedef typename cjTRemoveConst<T>::type* type; 
};

template<typename T> struct cjTRemoveConst<const T*> 
{ 
	typedef typename cjTRemoveConst<T>::type* type; 
};

template<typename T> struct cjTRemoveConst<T*&> 
{ 
	typedef typename cjTRemoveConst<T>::type*& type; 
};

template<typename T> struct cjTRemoveConst<const T*&> 
{ 
	typedef typename cjTRemoveConst<T>::type*& type; 
};


//
// cjTypeTrait
//


// cjName
template<typename T> 
struct cjTypeTrait						
{ 
	typedef typename T				_Type;		// cjName
	typedef typename const T		_CType;		// const cjName
	typedef typename const T&		_CRType;	// const cjName&
	typedef typename const T*		_CPType;	// const cjName*
	typedef typename const T*&		_CPRType;	// const cjName*&
	
	typedef _CRType					HPrm; // const cjName&
};

//template<typename T> struct cjTypeTrait<T const volatile>	{ typedef typename cjTRemoveConst<T>::type volatile type; };
//template<typename T> struct cjTypeTrait<T volatile>			{ typedef typename cjTRemoveConst<T>::type volatile type; };


// const cjName
template<typename T> 
struct cjTypeTrait<const T >
{ 
	typedef typename cjTypeTrait<T>::_Type				_Type;		// cjName
	typedef typename const cjTypeTrait<T>::_Type		_CType;		// const cjName
	typedef typename const cjTypeTrait<T>::_Type&		_CRType;	// const cjName&
	typedef typename const cjTypeTrait<T>::_Type*		_CPType;	// const cjName*
	typedef typename const cjTypeTrait<T>::_Type*&		_CPRType;	// const cjName*&

	typedef _CRType					HPrm; // const cjName&			
};

// cjName&
template<typename T> struct cjTypeTrait<T&>					
{ 
	typedef typename cjTypeTrait<T>::_Type				_Type;		// cjName
	typedef typename const cjTypeTrait<T>::_Type		_CType;		// const cjName
	typedef typename const cjTypeTrait<T>::_Type&		_CRType;	// const cjName&
	typedef typename const cjTypeTrait<T>::_Type*		_CPType;	// const cjName*
	typedef typename const cjTypeTrait<T>::_Type*&		_CPRType;	// const cjName*&

	typedef _CRType					HPrm; // const cjName&
};

// const cjName&
template<typename T> struct cjTypeTrait<const T& >			
{ 
	typedef typename cjTypeTrait<T>::_Type				_Type;		// cjName
	typedef typename const cjTypeTrait<T>::_Type		_CType;		// const cjName
	typedef typename const cjTypeTrait<T>::_Type&		_CRType;	// const cjName&
	typedef typename const cjTypeTrait<T>::_Type*		_CPType;	// const cjName*
	typedef typename const cjTypeTrait<T>::_Type*&		_CPRType;	// const cjName*&

	typedef _CRType					HPrm; // const cjName&
};

// cjNameChar*
template<typename T> struct cjTypeTrait<T* >					
{ 
	typedef typename cjTypeTrait<T>::_Type				_Type;		// cjNameChar
	typedef typename const cjTypeTrait<T>::_Type		_CType;		// const cjNameChar
	typedef typename const cjTypeTrait<T>::_Type&		_CRType;	// const cjNameChar&
	typedef typename const cjTypeTrait<T>::_Type*		_CPType;	// const cjNameChar*
	typedef typename const cjTypeTrait<T>::_Type*&		_CPRType;	// const cjNameChar*&

	typedef _CPType					HPrm; // const cjNameChar*
};
 
// const cjNameChar*
template<typename T> struct cjTypeTrait<const T* >			
{ 	
	typedef typename cjTypeTrait<T>::_Type				_Type;		// cjNameChar
	typedef typename const cjTypeTrait<T>::_Type		_CType;		// const cjNameChar
	typedef typename const cjTypeTrait<T>::_Type&		_CRType;	// const cjNameChar&
	typedef typename const cjTypeTrait<T>::_Type*		_CPType;	// const cjNameChar*
	typedef typename const cjTypeTrait<T>::_Type*&		_CPRType;	// const cjNameChar*&

	typedef _CPType					HPrm; // const cjNameChar*
};

template<typename T> struct cjTypeTrait<const T* const>		
{ 
	typedef typename cjTypeTrait<T>::_Type _Type; 
};


template<class TKey, class TVal>
class cjTPair : public cjMem
{
public:
	typedef typename cjTypeTrait<TKey>::HPrm	_CRKey;
	typedef typename cjTypeTrait<TKey>::_Type&	_RKey;

	cjTPair() : m_next(cjNULL), m_seqPrev(cjNULL), m_seqNext(cjNULL) {}
//	cjTPair() : m_key(TKey()), m_val(TVal()), m_next(cjNULL), m_seqPrev(cjNULL), m_seqNext(cjNULL) {}

//	cjTPair(_CRKey& key, const TVal& val) : m_key(key), m_val(val), m_next(cjNULL), m_seqPrev(cjNULL), m_seqNext(cjNULL) {}
	cjTPair(TKey& key, const TVal& val) : m_key(key), m_val(val), m_next(cjNULL), m_seqPrev(cjNULL), m_seqNext(cjNULL) {}
	//cjTPair(_CRKey& key, const TVal& val) : m_key(const_cast<_TKey&>(key)), m_val(val), m_next(cjNULL), m_seqPrev(cjNULL), m_seqNext(cjNULL) {}

	template<class TRHSKey, class TRHSVal>
	cjTPair(const cjTPair<TRHSKey, TRHSVal>& rhs) : m_key(rhs.m_key), m_val(rhs.m_val), m_next(cjNULL) {}
	~cjTPair() {}

	TKey					m_key;
	TVal					m_val;

	cjTPair*				m_next; // at same hash key(hash bucket)

	// for linear operate(delete, sequence find)
	cjTPair*				m_seqPrev;
	cjTPair*				m_seqNext;
};


template<class TKey, class TVal, cjUINT32 TKeyCloneMethod, cjUINT32 TValCloneMethod>
struct cjTPairCloner
{
	void operator()(cjTPair<TKey, TVal>* dst, const cjTPair<TKey, TVal>* src) const
	{
	}
};


template<class TKey, class TVal>
struct cjTPairCloner<TKey, TVal, cjTCloneByCopy, cjTCloneByCopy>
{
	void operator()(cjTPair<TKey, TVal>* dst, const cjTPair<TKey, TVal>* src) const
	{
		dst->m_key = src->m_key;
		dst->m_val = src->m_val;
	}
};


template<class TKey, class TVal>
struct cjTPairCloner<TKey, TVal, cjTCloneByCopy, cjTCloneByRef>
{
	void operator()(cjTPair<TKey, TVal>* dst, const cjTPair<TKey, TVal>* src) const
	{
		dst->m_key = src->m_key;
		dst->m_val = src->m_val;
		
		if(dst->m_val)
			dst->m_val->AddRef();
	}
};


template<class TKey, class TVal>
struct cjTPairCloner<TKey, TVal, cjTCloneByCopy, cjTCloneByClone>
{
	void operator()(cjTPair<TKey, TVal>* dst, const cjTPair<TKey, TVal>* src) const
	{
		dst->m_key = src->m_key;
		dst->m_val = src->m_val;
		
		if(dst->m_val)
			dst->m_val->AddRef();
	}
};





//template<class T_>
//struct cjTCloner
//{	
//	typename typedef cjTypeOnly<T_>::type	T;
//
//	T_& operator()(const T& src, const T_& clone, cjTFLAG flag) const
//	{
//		clone = src;
//		return clone;
//	}
//
//
//	//T& operator()(const T& src, T& clone, cjTFLAG flag) const
//	//{
//	////	T& clone_ = const_cast<T>(clone);
//	//	if(flag & cjTCloneByClone)
//	//		return src.Clone(&clone);
//	//
//	//	return clone;
//	//}
//
//	//const T*& operator()(const T*& src, const T*& clone, cjTFLAG flag) const
//	//{
//	//	clone = src;
//	//	return clone;
//	//}
//
//	//T* operator()(const T* src, T* clone, cjTFLAG flag) const
//	//{
//	////	T& clone_ = const_cast<T>(clone);
//	//	if(flag & cjTCloneByClone)
//	//		return src->Clone(&clone);
//	//
//	//	return clone;
//	//}
//};


//template<class T>
//const T& cjTCloner(const T& src, const T& clone, cjTFLAG flag)
//{
//	src = clone;
//	return clone;
//}

// ex: src/dst: const cjName* src  -> const cjName* dst
//const wchar_t* cjTCloner(const wchar_t* src , const wchar_t*& clone, cjTFLAG /*flag*/)
//{
//	src = clone;
//	return clone;
//}

//template<class T>
//const T& cjTCloner(const T& src, const T& clone, cjTFLAG flag)
//{
//	src = clone;
//	return clone;
//}


//template<class T>
//T& cjTCloner(T& src, T& clone, cjTFLAG flag)
//{
//	src = clone;
//	return clone;
//}




//template<class T>
//cjTRemoveConst<T>* cjTCloner(T* src, cjTRemoveConst<T>* clone, cjTFLAG flag)
//{
////	T& clone_ = const_cast<T>(clone);
//
//	if(flag & cjTCloneByClone)
//		return src->Clone(clone);
//
//	return clone;
//}


//
// cjTSelect
//

template<cjBool flags, typename T, typename U> 
struct cjTSelect 
{
	typedef T SelectType;
}; 
 
template<typename T, typename U> 
struct cjTSelect<false, T, U> 
{ 
	typedef U SelectType; 
}; 


// simple linked-list
// use as 'new (xxx) cjTSList(element, lstXXX)

template <class TElem> 
class cjTSList // no specify ': public cjMem', for global static allocation
{
public:
	TElem								m_element;
	cjTSList<TElem>*					m_next;

	cjTSList(TElem elem, cjTSList<TElem>* next=cjNULL)
	{
		m_element = elem;
		m_next = next;
	}
};


template< class T> 
inline void cjTExchange(T& a, T& b)
{
	const T t = a;

	a = b;
	b = t;
}


// compare = int cmp(const T& a, const T& b) { if(a > b) return 1; else return 0 }
template<class T, class TCF >
void cjTSimpleBubbleSort(T* ary, cjAryOff elemCount, TCF compare)
{
	typedef cjINT32 (*PFNComp)(const T&, const T&);

	for(cjAryOff i=0; i<elemCount-1; i++)
	{
		for(cjAryOff k=0; k<elemCount-1-i; k++)
		{
			if( ((PFNComp)compare)(ary[k], ary[k+1]) > 0)
				cjTExchange(ary[k], ary[k+1]);
		}
	}
}

#ifdef _USE_CJTQSORT_DBG_
class cjTSortDBGView
{
public:
	virtual void DBGViewQ(void* ary, cjAryOff first, cjAryOff last, cjAryOff cen) {}
};
#endif


//cjSlow
template<class T, class TCF>
void cjTQSort(T* ary, cjAryOff first, cjAryOff last, TCF compare, void* dbgView=0)
{  
	typedef cjINT32 (*PFNComp)(const T&, const T&);
	cjAryOff left, right, cen;
	
	/*
	cjAryOff diff = last - first;

	if(diff == 0)
	{
		return;
	}
	else if(diff == 1)
	{
		if(((PFNComp)compare)(ary[last], ary[first]))
		{
			if(first == cen)
				cen = first;
			else if(last == cen)
				cen = last;
			cjTExchange(ary[first], ary[last]);

		return;
	}*/

	left=first;
	right=last;
		
	// caution !!!
	// center value may be changed on sorting, but once a center value is selected,
	// the value is not changed until end of this sort phase
	// save center value
	cen = (first + last) / 2; // center value for efficient sorting presorted elements

	while(left <= right) 
	{  
		while( ((PFNComp)compare)(ary[left], ary[cen]) )
		{
			left++;
			cjAssert(left <= last);
		}

		while( ((PFNComp)compare)(ary[cen], ary[right]) ) 
		{
			right--;
			cjAssert(right >= 0);  // right<0 , something wrong is happend
		}

		if(left <= right) // left and right does not cross
		{
			if(left != right)
			{
				// center value position is changed
				if(left == cen)
					cen = right;
				else if(right == cen)
					cen = left;

				cjTExchange(ary[left], ary[right]);
			}

			left++;  
			right--;  
		}

#ifdef _USE_CJTQSORT_DBG_
		if(dbgView)
			((cjTSortDBGView*)dbgView)->DBGViewQ(ary, first, last, cen);
#endif
	}

	if(first < right)
		cjTQSort(ary, first, right, compare, dbgView);

	if(left < last)
		cjTQSort(ary, left, last, compare, dbgView);	
}  


template<class T, class TCF> 
void cjTSort(T* ary, cjAryOff elemCount, TCF compare, void* dbgViewer=0)
{
	if(elemCount < 2)
		return; // needs 2 and more items to do sort.

	if(elemCount <= 8) // use bubble sort if element count is less than 9
		return cjTSimpleBubbleSort(ary, elemCount, compare);

	// do quick-sort
	cjTQSort(ary, 0, elemCount-1, compare, dbgViewer);
}


//
// Pointer Pool Alloc
//

typedef void (*cjTAbstractPtrPoolAlloc_PFShutDown)();
CJCORE_API void cjTAbstractPtrPoolAlloc_RegisterShutDown(cjTAbstractPtrPoolAlloc_PFShutDown pfShutDown);
CJCORE_API void cjTAbstractPtrPoolAlloc_ShutDown();


template <class T> 
class CJCORE_API cjTAbstractPtrPoolAlloc : public cjMem
{
public:
	class AllocNode : public cjMem
	{
	public:
		static void CallDestructor(void* m) 
		{
			((AllocNode*)m)->~AllocNode();
		}

	public:
		AllocNode*					m_next;
		void*						m_reserved;  // key in MapItem, prev in ListItem
		T							m_value;
		AllocNode*					m_pool;
	};

	static void Init()
	{
		cjAssert(!ms_freeNodes);
		cjAssert(ms_freeNodePoolSiz > 1);

		ms_freeNodes = new AllocNode[ms_freeNodePoolSiz];

		AllocNode* cur = ms_freeNodes + ms_freeNodePoolSiz - 1;
		cur->m_next = 0;

		cjUINT32 i=1;

		// pre build nodes (ms_freeNodePoolSiz-2 times)
		do
		{
			cur = ms_freeNodes + i;
			cur->m_next = cur + 1;
			cur->m_pool = ms_freeNodes;

		} while(i++ < ms_freeNodePoolSiz-2);

		if(ms_head)
		{
			ms_freeNodes->m_next = ms_head;
			ms_head = ms_freeNodes;
		}
		else
		{
			ms_head = ms_freeNodes;
			ms_head->m_next = cjNULL;
		}

		ms_freeNodes = ms_freeNodes + 1;		
	}

	static void ShutDown()
	{
		AllocNode* cur = ms_head;

		while(cur)
		{
			AllocNode* next = cur->m_next;
			delete [] cur;
			cur = next;
		}

		ms_head = cjNULL;
		ms_freeNodes = cjNULL;
	}

	static void SetPoolSiz(cjUINT32 pollSiz)
	{
		cjAssert(pollSiz > 1);
		ms_freeNodePoolSiz = pollSiz;
	}

protected:
	static AllocNode*				ms_head;
	static AllocNode*				ms_freeNodes;
	static cjUINT32					ms_freeNodePoolSiz;

	static cjMutex					ms_cs;
};


#define CJTABSTRACT_PTRPOOL_ALLOC_IMPL(type, poolSiz) \
	template<> typename cjTAbstractPtrPoolAlloc< type >::AllocNode* cjTAbstractPtrPoolAlloc< type >::ms_head = cjNULL; \
	template<> typename cjTAbstractPtrPoolAlloc< type >::AllocNode* cjTAbstractPtrPoolAlloc< type >::ms_freeNodes = cjNULL; \
	template<> cjUINT32 cjTAbstractPtrPoolAlloc< type >::ms_freeNodePoolSiz = poolSiz; \
	template<> cjMutex cjTAbstractPtrPoolAlloc< type >::ms_cs = cjMutex();

// in ios
// 'template<> cjMutex cjTAbstractPtrPoolAlloc< type >::ms_cs;' causes
// 'Undefined symbols for architecture xxx : cjTAbstractPtrPoolAlloc<xxx>::ms_cs'
// fix) 'template<> cjMutex* cjTAbstractPtrPoolAlloc< type >::ms_cs = cjMutex();'


template<class T> 
class cjTPtrPoolAlloc : public cjTAbstractPtrPoolAlloc<T>
{
public:
	typedef cjTAbstractPtrPoolAlloc<T>		Pool;
	typedef typename Pool::AllocNode		Node;
	
	static void* Alloc()
	{
		Pool::ms_cs.DoLock();

		if(!Pool::ms_freeNodes)
			Pool::Init();

		Node* p = Pool::ms_freeNodes;

		if(p)
		{
			// no need to call constructor, this is pointer pool
			//Node::CallConstructor(node);

			Pool::ms_freeNodes = Pool::ms_freeNodes->m_next;				
		}
		else
		{
			// no more freeNodes
			p = new Node;
			p->m_pool = cjNULL;
		}

		p->m_next = cjNULL;
		p->m_reserved = cjNULL;
		p->m_value = cjNULL;
		
		Pool::ms_cs.DoUnLock();

		return p;
	}

	static void DeAlloc(void* p)
	{
		Pool::ms_cs.DoLock();

		Node* node = (Node*)p;

		if(node->m_pool)
		{
			typename Pool::AllocNode* oldPool = node->m_pool;

			// no need to call destructor for node->m_value, this is pointer pool
			// Node::CallDestructor(node);

			node->m_pool = oldPool;
			node->m_reserved = cjNULL;
			node->m_next = Pool::ms_freeNodes;

			Pool::ms_freeNodes = node;
		}
		else
		{
			delete node;
		}

		Pool::ms_cs.DoUnLock();
	}
};


template<class T>
class cjTDefaultAlloc : public cjMem
{
public:
	class AllocNode : public cjMem
	{
	public:
		AllocNode*					m_next;
		void*						m_reserved;  // key in MapItem, prev in ListItem
		T							m_value;
	};

	static void* Alloc()
	{
		return new AllocNode;
	}

	static void DeAlloc(void* p)
	{
		delete ((AllocNode*)p);
	}
};

template<class T, cjBYTE TFill>
class cjTPODTypeFill
{
public:
	template<class M>
	class FillPtr
	{
	public:
		static void Fill(M& asPtr)
		{
			cjMemSet((void*)asPtr, TFill, sizeof(M));
			//asPtr = (M)TFill;
		}
	};

	template<class M>
	class FillPOD
	{
	public:
		static void Fill(M& asPOD)
		{
			cjMemSet(&asPOD, TFill, sizeof(M));
		}
	};

	typedef typename cjTSelect< cjPtrTypeTraits<T>::isPointer, FillPtr<T>, FillPOD<T> >::SelectType Filler;
};


#endif
