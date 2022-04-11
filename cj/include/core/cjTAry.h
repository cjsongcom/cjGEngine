
#ifndef _CJTARY_H_
#define _CJTARY_H_

#if defined(CJ_PLATFORM_WIN32)
#pragma warning(disable : 4251)
#endif

#if defined(CJ_PLATFORM_WIN32)
#pragma warning(push)
#pragma warning(disable : 4127)
#endif


class cjFStream;


class CJCORE_API cjAry : public cjMem  // if needs to remove 'cjMem', solve _MRealloc first
{
public:
	cjAry() : m_numElement(0), m_maxElement(0), m_data(NULL) 
	{
	}

	cjAry(cjAryOff initialNum, cjAryOff elementSiz, cjBool initWithEmpty=cjFalse)
		: m_numElement(initialNum), m_maxElement(initialNum), m_data(NULL)
	{
		ReAlloc(elementSiz);

		if(initWithEmpty)
			PushEmpty(initialNum, elementSiz);
	}

	~cjAry() 
	{ 
		if(m_data)
			cjMemDeAlloc(m_data);

		m_data = NULL;
		m_numElement = m_maxElement = 0;
	}
	
	cjAryOff Num() const { return m_numElement; }
	cjAryOff Count() const { return m_numElement; }
	cjAryOff GetMaxCount() const { return m_maxElement; }
	cjAryOff GetAvailCount() const { return m_maxElement - m_numElement; }

	void* GetData() { return m_data; }
	const void* GetData() const { return m_data; }

	cjBool IsValidIndex(cjAryOff index) const
	{
		return index >= 0 && index < m_numElement;
	}

	// ret : start position to add
	cjAryOff PushEmpty(cjAryOff count, cjAryOff elementSiz)
	{
		cjAryOff index = m_numElement;

		m_numElement += count;
		if(m_numElement > m_maxElement)
		{
			ExtendMaxElement();
			ReAlloc(elementSiz);
		}

		return index;
	}

	void ExtendMaxElement()
		{ m_maxElement = m_numElement + (3 * m_numElement / 8) + 16; }

	// insert blink space at index
	void Insert(cjAryOff index, cjAryOff count, cjAryOff elementSiz)
	{
		cjAryOff numOld = m_numElement;

		m_numElement += count;
		if( m_numElement > m_maxElement)
		{
			ExtendMaxElement();
			ReAlloc(elementSiz);
		}

		cjMemMove((cjBYTE*)m_data + (index + count) * elementSiz,
			(cjBYTE*)m_data + (index * elementSiz), (numOld - index) * elementSiz);
	}
   
	cjINT32 AddZeroed(cjINT32 elementSiz, cjINT32 n=1)
	{
		cjINT32 idx = PushEmpty(n, elementSiz);
		cjMemZero((cjBYTE*)m_data+idx*elementSiz, n*elementSiz);
		return idx;
	}

	void Shrink(cjAryOff elementSiz)
	{
		if(m_maxElement != m_numElement)
		{
			m_maxElement = m_numElement;
			ReAlloc(elementSiz);
		}
	}

	void SetSiz(cjAryOff count, cjAryOff elementSiz)
	{
		m_numElement = m_maxElement = count;
		ReAlloc(elementSiz);
	}

	void Remove(cjAryOff index, cjAryOff count, cjAryOff elementSiz)
	{
		if(count)
		{
			cjMemMove(	(cjBYTE*)m_data + index * elementSiz,
						(cjBYTE*)m_data + (index+count) * elementSiz,
						(m_numElement - index - count) * elementSiz );

			m_numElement -= count;

			// shrink size to zero when m_numElement is zero
			if(	(3*m_numElement < 2*m_maxElement || (m_maxElement-m_numElement)*elementSiz >= 16384)
			    && (m_maxElement-m_numElement > 64 || m_numElement == 0) )
			{
				m_maxElement = m_numElement;
				ReAlloc(elementSiz);
			}
		}
		cjAssert(m_maxElement >= m_numElement);
	}

	void RemoveAll(cjAryOff elementSiz, cjAryOff maxNumElement=0)
	{
		m_numElement = 0;
		m_maxElement = maxNumElement;
		ReAlloc(elementSiz);
	}

	void ReAlloc(cjAryOff elementSiz)
	{
		// _MRealloc will preserve content of memory is being relocated
		m_data = _MReAlloc(m_data, m_maxElement * elementSiz, NULL /* _CT("cjTAry") */ );

		// when reallocing from any valid size to zero
		// ex) char* v = (char*)malloc(100);
		//           v = (char*)realloc(0);
		//     'realloc(0)' will returns null pointer
		//     original buf(v) is freed, no need to call free(v);
	}
    
protected:
	void*							m_data;

	cjAryOff						m_numElement;
	cjAryOff						m_maxElement;
};


template<class T> 
class cjTAry : public cjAry
{
public:
	typedef T ElementType;
  
/*
    The 2003 C++ standard only allows member template specialization outside of 
    the enclosing class definition. Also, the out-of-definition specialization 
    must be an explicit full specialization of the enclosing template.
    Microsoft C++ is non-standard in this regard
*/
	void Cpy(const cjTAry<T>& rhs, cjUINT32 /*siz=1*/)
	{
		// primitive type
		if(!cjTypeInfo<T>::TypeHasDestructor)
		{
			cjMemCpy(&(*this)(0), &rhs(0), m_numElement * sizeof(T));
		}
		else
		{
			for(cjAryOff i=0; i<m_numElement; i++)
				(*this)(i) = rhs(i);
		}
	}

	cjTAry() : cjAry() {}
	cjTAry(cjAryOff initialNum, cjBool initWithEmpty=cjFalse) 
		: cjAry(initialNum, sizeof(T), initWithEmpty) 
	{
		if(!initWithEmpty && initialNum > 0)
		{
			if(cjTypeInfo<T>::TypeHasDestructor)
			{
				for(cjAryOff index=0; index<initialNum; index++)
					::new(*this, index) T; // do not use new(*this) T();
			}
			else
			{
				T f = T();
				Fill(f);
			}
		}
	}

	cjTAry(const cjTAry<T>& rhs) 
		: cjAry(rhs.m_numElement, sizeof(T))
	{	
		// primitive type
		if(cjTypeInfo<T>::TypeHasDestructor)
		{
			m_numElement = 0;
			for(cjAryOff i=0; i<rhs.m_numElement; i++)
				::new(*this) T(rhs(i)); // do not use new(*this) T(rhs(i) !!
		}
		else
		{
			Cpy(rhs, sizeof(T));
		}
	}

	~cjTAry()
	{
		Remove( 0, m_numElement); // with calling destructor ?
	}

    T& operator()(cjAryOff i) { return ((T*)m_data)[i]; }
	const T& operator()(cjAryOff i) const { return ((T*)m_data)[i]; }

    // yield for cjTString
	//T& operator[](cjAryOff i) { return ((T*)m_data)[i]; }
	//const T& operator[](cjAryOff i) const { return ((T*)m_data)[i]; }

	// warn! - pop item has called destructor
	T Pop()
	{
		T Result = ((T*)m_data)[m_numElement-1];
		Remove(m_numElement-1);
		return Result;
	}

	T& Last(cjAryOff c=0)
	{
		return ((T*)m_data)[m_numElement-c-1];
	}

	const T& Last(cjAryOff c=0) const
	{
		return ((T*)m_data)[m_numElement-c-1];
	}

	void Shrink()
	{
		cjAry::Shrink(sizeof(T));
	}

	cjBool FindItem(const T& item, cjAryOff& index) const
	{
		for(index=0; index<m_numElement; index++)
			if((*this)(index)==item)
				return cjTrue;

		return cjFalse;
	}

	cjAryOff FindItemIndex(const T& item) const
	{
		for(cjAryOff index=0; index<m_numElement; index++)
			if( (*this)(index)==item)
				return index;

		return cjIndex_None;
	}

	cjAryOff PushEmpty(cjAryOff n=1) // not to set element to zero
	{		
		if(cjTypeInfo<T>::TypeHasDestructor)
		{
			for(cjAryOff index=0; index<n; index++)
				::new(*this) T; // do not use new(*this) T();

			return m_numElement-1;
		}
		else
		{
			//cjtodo : not implemented for replicating class instance when reallocation is occur
			return cjAry::PushEmpty(n, sizeof(T));
		}
	}

	cjAryOff PushZero(cjAryOff n=1) // set element to zero
	{
		return cjAry::AddZeroed(sizeof(T), n);
	}

	// this function does not call destructor
	// use RemoveAll(newSiz)
	void SetSiz(cjAryOff n=1) 
	{
		if(cjTypeInfo<T>::TypeHasDestructor)
		{
			cjAssert(!"do not call non POD type.. use RemoveAll for setting initial size..");
			return;
		}

		cjAry::SetSiz(n, sizeof(T));
	}

	void Insert(cjAryOff index, cjAryOff count=1)
	{
		cjAry::Insert(index, count, sizeof(T));
	}

	void Remove(cjAryOff index, cjAryOff count=1)
	{
		if(cjTypeInfo<T>::TypeHasDestructor)
			for(cjAryOff i=index; i<index+count; i++)
				(&(*this)(i))->~T();

		cjAry::Remove(index, count, sizeof(T));
	}

	void RemoveAll(cjAryOff maxCount=0, cjBool addEmpty=false)
	{
		if(cjTypeInfo<T>::TypeHasDestructor)
			for(cjAryOff i=0; i<m_numElement; i++)
				(&(*this)(i))->~T();

		cjAry::RemoveAll(sizeof(T), maxCount);

		if(addEmpty)
		{
			PushEmpty(maxCount);
		}
	}

	cjAryOff Push(const T& Item)
	{
		cjAryOff index;

//		if(cjTypeInfo<T>::HasDestructor())
//		{
			//cjTAry<T>& dst = *((cjTAry<T>*)&(*this)(index));
			//cjtodo: can be change to ::new(*this) T(item) ?, call copy-constructor direct
			//cjtodo: can be not use new for primitive type ex:int, cjFLOAT
			//::new(*this) T() cause warning to POD type
			::new(*this) T;  // do not use new(*this) T;
			index = m_numElement-1;
//		}
//		else
//		{
//			index = Add();
//		}

		(*this)(index) = Item;

		return index;
	}

	cjAryOff PushUnique(const T& item)
	{
		for(cjAryOff i=0; i<m_numElement; i++)
		{
			if((*this)(i) == item)
				return i;
		}

		::new(*this) T;
		cjAryOff index = m_numElement-1;
		(*this)(index) = item;

		return index;
	}

	cjAryOff SetWithGrow(cjAryOff index, const T& item)
	{
		cjAryOff count = Count();

		if(count < index)
			PushEmpty(index - count);

		(*this)(index) = item;

		return index;
	}


	// return counts how many items is removed
	cjAryOff RemoveItem(const T& item)
	{
		cjAryOff oldNum=m_numElement;
		for(cjAryOff index=0; index<m_numElement; index++)
			if((*this)(index)==item)
				Remove(index--);
		return oldNum-m_numElement;
	}

	cjBool IsEmpty() const
	{
		return m_numElement == 0 ? cjTrue : cjFalse;
	}

	void Fill(const T& f)
	{
		if(cjTypeInfo<T>::TypeHasDestructor)
		{
			for(cjAryOff i=0; i<m_numElement; i++)
				::new(*this, i) T;
		}
		else
		{
			if(sizeof(T) == 1)
			{
				cjMemSet(&(*this)(0), *((unsigned char*)&f), sizeof(T) * m_numElement);
			}
			else
			{
				for(cjAryOff i=0; i<m_numElement; i++)
					cjMemCpy(&(*this)(i), (unsigned char*)&f, sizeof(T));
			}
		}
	}

	cjAryOff cjSlow Find(const T& findItem, cjAryOff startOff=0)
	{
		for(cjAryOff i=startOff; i<m_numElement; i++)
		{
			if((((T*)m_data)[i]) == findItem)
				return i;
		}

		return cjAryOff_Invalid;
	}
		
	void* Begin() { return GetData(); }
	void* End() { return (cjBYTE*)GetData() + Num() * sizeof(T); }

	//
	// stream support
	//

	friend cjFStream& operator<< (cjFStream& stream, cjTAry& ary)
	{
		if(sizeof(T)==1) 
		{
			// byte type, no requires construction or destruction
			stream << cjFStreamCmprI32(ary.m_numElement);

			if(stream.IsLoading())
			{
				ary.m_maxElement = ary.m_numElement;
				ary.ReAlloc(sizeof(T));
			}

			if(ary.Num() > 0)
				stream.SerializeStream(&ary(0), ary.Num());
			else
				stream.SerializeStream(NULL, 0);
		}
		else if(stream.IsLoading())
		{
			// loading
			cjINT32 loadedNum = 0;

			stream << cjFStreamCmprI32(loadedNum);
			ary.RemoveAll(loadedNum);

			for(cjINT32 i=0; i<loadedNum; i++)
				stream << *::new(ary) T;
		}
		else
		{
			// saving
			stream << cjFStreamCmprI32(ary.m_numElement);

			for(cjINT32 i=0; i<ary.m_numElement; i++)
				stream << ary(i);
		}

		return stream;
	}


	//
	// iterator support
	//

	class Iterator
	{
	public:
		Iterator(cjTAry<T>& ary) :  m_ary(ary), m_index(-1) { ++*this; }
		void operator++() { ++m_index; }

		void Erase() { m_ary.Remove(m_index--); }
		cjAryOff GetIndex() const { return m_index; }
		
		operator cjBool() const { return m_index < m_ary.GetSiz(); }
		
		T& operator*() const { return m_ary(m_index); }
		T* operator->() const { return &m_ary(m_index); }

		T& GetCur() const { return m_ary(m_index); }
		T& GetPrev() const { return m_ary(m_index ? m_index-1 : m_ary.GetSiz()-1 ); }
		T& GetNext() const { return m_ary(m_index<m_ary.GetSiz()-1 ? m_index+1 : 0 ); }

	private:
		cjTAry<T>& m_ary;
		cjAryOff m_index;
	};

};

struct _cjNameCharPath
{
	cjNameChar			m_path[cjMAX_PATH];
};

typedef cjTAry<_cjNameCharPath> cjNameCharPathAry;

//template <class T>
// template <cjUINT32 TSiz>
//inline void cjTAry<T>::Cpy<TSiz>(const cjTAry<T>& rhs) 
//{ 
//	for(cjAryOff i=0; i<m_numElement; i++)
//		(*this)(i) = rhs(i);	
//}
//
//template <class T>
//  template <cjUINT32 TSiz>
//inline void cjTAry<T>::Cpy<1>(const cjTAry<T>& rhs) 
//{ 
//	cjMemCpy(&(*this)(0), &rhs(0), m_numElement * sizeof(T));
//}
//

template <class T> 
inline void* operator new(size_t , cjTAry<T>& ary)
{
	cjAryOff index = ary.cjAry::PushEmpty(1, sizeof(T));
	return &ary(index);
}

template <class T> 
inline void* operator new(size_t , cjTAry<T>& ary, cjAryOff index)
{
	return &ary(index);
}


//
// cjTNullableAry
//

//template <class T, T TNUL=T(0)>
template <class T, class TNUL=T >
class cjTNullableAry : public cjAry
{
public:

	cjTNullableAry() : cjAry(), m_validCount(m_maxElement) { }
	cjTNullableAry(cjAryOff initialNum, cjBool initWithEmpty=cjFalse) : cjAry(initialNum, sizeof(T), initWithEmpty), m_validCount(m_maxElement) {}

	cjTNullableAry(const cjTNullableAry<T, TNUL>& rhs) : cjAry(rhs.m_numElement, sizeof(T)), m_validCount(rhs.m_validCount)
	{	
		*this = rhs;
	}

	cjTNullableAry& operator =(const cjTNullableAry<T, TNUL>& rhs)
	{
		if(this == &rhs)
			return *this;

		if(cjTypeInfo<T>::HasDestructor())
		{
			for(cjAryOff i=0; i<m_numElement; i++)
			{
				T& cur = (*this)(i);
				if(cur != TNUL())
					(&(*this)(i))->~T();
			}
		}

		cjAry::RemoveAll(sizeof(T), rhs.m_numElement);

		if(cjTypeInfo<T>::HasDestructor())
		{								
			for(cjAryOff i=0; i<rhs.m_numElement; i++)
			{
				cjAryOff index = 1;

				if(rhs(i) != TNUL())
					::new(*this, index) T(rhs(i));
				else
					::new(*this, index) T(TNUL());
			}
		}
		else
		{
			// primitive type
			cjMemCpy(&(*this)(0), &rhs(0), sizeof(T) * rhs.m_numElement);
		}

		m_numElement = rhs.m_numElement;
		m_validCount = rhs.m_validCount;

		return *this;
	}

	~cjTNullableAry() 
	{ 
		RemoveAll();
	}
	
	T& operator()(cjAryOff i)		 { return ((T*)m_data)[i]; }
	const T& operator()(cjAryOff i) const { return ((T*)m_data)[i]; }

	T Pop() 
	{
		cjAssert(m_validCount != 0);
		cjAryOff notNullCount = m_validCount;

		for(cjAryOff i=m_numElement-1; i>0 && notNullCount>0; i--, notNullCount--)
		{
			T& cur = (*this)(i);

			if(cur == TNUL())
			{
				continue;
			}
			else
			{
				T r = cur;
				Remove(i);

				return r;
			}
		}

		return TNUL();
	}

	cjAryOff Last(cjAryOff c=0) const 
	{
		cjAryOff notNullCount = m_validCount;

		if(m_validCount == 0)
			return cjAryOff_Invalid;									

		for(cjAryOff i=m_numElement-1; i>0 && notNullCount>0; i--, notNullCount--)
		{
			if((*this)(i) == TNUL())
				continue;

			return i;
		}

		return cjAryOff_Invalid;
	}

	cjAryOff FindFirstNullElement()
	{
		if(m_validCount == 0)
			return cjAryOff_Invalid;

		for(cjAryOff i=0; i<m_numElement; i++)
		{
			if((*this)(i) == TNUL())
				return i;
		}

		return cjAryOff_Invalid;
	}

	cjBool FindItem(const T& item, cjAryOff& index) const 
	{
		for(index=0; index<m_numElement; index++)
			if((*this)(index)==item)
				return cjTrue;

		return cjFalse;
	}

	cjAryOff FindItemIndex(const T& item) const
	{
		for(cjAryOff index=0; index<m_numElement; index++)
			if( (*this)(index)==item)
				return index;

		return cjAryOff_Invalid;
	}

	void RemoveAll(cjAryOff maxCount=0)
	{
		if(cjTypeInfo<T>::TypeHasDestructor)
		{
			for(cjAryOff i=0; i<m_numElement; i++)
			{
				T& cur = (*this)(i);
				if(cur != TNUL())
					(&(*this)(i))->~T();
			}
		}

		cjAry::RemoveAll(sizeof(T), maxCount);
		m_validCount = 0;

		if(cjTypeInfo<T>::TypeHasDestructor)
			for(cjAryOff i=0; i<m_numElement; i++)
				(*this)(i) = TNUL();
	}

	void Remove(cjAryOff index, cjAryOff count=1) 
	{
		cjAssert(index < m_numElement);
		cjAssert(index+count <= m_numElement);

		for(cjAryOff i=index; i<index+count; i++)
		{
			T& cur = (*this)(i);
			if(cur != TNUL())
			{
				/*if(cjTypeInfo<T>::HasDestructor())
					(&(*this)(i))->~T();*/

				cur = TNUL();
				m_validCount--;
			}
		}

		if(m_numElement == index+count)
			cjAry::Remove(index, count, sizeof(T));

		cjAssert(m_validCount <= m_numElement);
	}

	cjAryOff Remove(const T& item)
	{
		cjAryOff oldNotNullCount = m_validCount;
		cjAryOff noHitCount=0;

		for(cjAryOff index=0; 
			index<m_numElement && noHitCount != m_validCount; 
			index++)
		{
			T& cur = (*this)(index);

			if(cur == TNUL())
				continue;

			if(cur == item)
			{
				/*if(cjTypeInfo<T>::HasDestructor())
					(&(*this)(index))->~T();*/

				cur = TNUL();
				m_validCount--;
			}
			else
			{
				noHitCount++;
			}								
		}

		cjAssert(m_validCount <= m_numElement);

		return oldNotNullCount - m_validCount;
	}

	cjAryOff Push(const T& item, cjBool insertAtFirstNull=cjTrue)
	{		
		cjAryOff index = insertAtFirstNull ? 1 : 0;

		if(item != TNUL())
		{
			::new(*this, index) T;  // do not use new(*this) T;							
			(*this)(index) = item;

			if(item != TNUL())
				m_validCount++;

			cjAssert(m_validCount <= m_numElement);
		}
		else
		{
			// insert null item
			index = 0; // don't insert item at first null item
			::new(*this, index) T(TNUL());
		}

		return index;
	}

	cjAryOff PushUnique(const T& item, cjBool insertAtFirstNull=cjTrue)
	{
		cjAssert(item != TNUL());

		for(cjAryOff i=0; i<m_numElement; i++)
		{
			if((*this)(i) == item)
				return i;
		}

		cjAryOff index = insertAtFirstNull ? 1 : 0;

		::new(*this, index) T;
		(*this)(index) = item;

		if(item != TNUL())
			m_validCount++;

		cjAssert(m_validCount <= m_numElement);

		return index;
	}

	cjBool IsEmpty() { return m_validCount == 0 ? cjTrue : cjFalse; }

	void Shrink() 
	{
		if(m_validCount == m_numElement)
			return;

		if(m_validCount)
		{
			for(cjAryOff i=0, k=0; i<m_numElement; i++)
			{
				T& cur = (*this)(i);

				if(cur != TNUL())
				{
					T& dst = (*this)(k);

					if(dst != cur)
						dst = cur;

					k++;
				}
			}
		}

		// adjust storage
		cjAry::m_maxElement = cjAry::m_numElement = m_validCount;
		cjAry::ReAlloc(sizeof(T));					
	}

	void SetSiz(cjAryOff siz)
	{
		if(m_numElement == siz)
			return;

		cjAryOff diff = siz - m_numElement;

		if(m_numElement < siz)
		{
			PushEmpty(diff);
			return;
		}
		else
		{
			Shrink();
			Remove(siz, diff);
		}
	}

	cjAryOff			GetValidCount() const { return m_validCount; }

	void Copy(cjTNullableAry<T, TNUL>& dst) const
	{
		dst.RemoveAll(m_numElement);
		
		if(m_validCount == 0)
			return;

		for(cjAryOff i=0; i<m_numElement; i++)
			dst(i) = (*this)(i);
	}

	void CloneCopy(cjTNullableAry<T, TNUL>& dst) const
	{
		dst.RemoveAll(m_numElement);

		if(m_validCount == 0)
			return;

		for(cjAryOff i=0; i<m_numElement; i++)
		{
			const T& cur = (*this)(i);

			if(cur != TNUL())
				dst(i) = cur->Clone();
			else
				dst(i) = TNUL();
		}
	}

	
public:
	typedef T							ElementType;
	cjAryOff							m_validCount; // not null item

private:
	// disable functions	
	cjAryOff		PushEmpty(cjAryOff n=1) { return cjAryOff_Invalid; }
	cjAryOff		PushZero(cjAryOff n=1)  { return cjAryOff_Invalid; }
	void			Insert(cjAryOff index, cjAryOff count=1) { }

};


template <class T, class TNUL> 
inline void* operator new(size_t , cjTNullableAry<T, TNUL>& ary)
{
	index = ary.cjAry::PushEmpty(1, sizeof(T));
	return &ary(index);
}

template <class T, class TNUL> 
inline void* operator new(size_t , cjTNullableAry<T, TNUL>& ary, cjAryOff& index)
{
	if(index)
	{
		index = ary.FindFirstNullElement();

		if(index == cjAryOff_Invalid)
			index = ary.cjAry::PushEmpty(1, sizeof(T));
	}
	else
	{
		index = ary.cjAry::PushEmpty(1, sizeof(T));
	}

	return &ary(index);
}


//
// cjTStrideArray
//

template <class T, cjUINT32 TStride> 
class cjTStrideArray : public cjAry
{
public:
	typedef T ElementType;
	enum { ElementStride = TStride };
	enum { ElementSiz = sizeof(T) * TStride };
    
	cjAryOff GetNum() const { return m_numElement; }
	cjAryOff GetStride() const { return ElementStride; }
	cjAryOff GetElementSiz() const { return ElementSiz; } 

	void Cpy(const cjTStrideArray<T, TStride>& rhs, cjBool rawCpy=!cjTypeInfo<T>::HasDestructor())
	{
		if(rawCpy)
			cjMemCpy((*this)(0), rhs(0), m_numElement * ElementSiz);
		else
		{
			for(cjAryOff i=0; i<m_numElement; i++)
			{
				T* dst = (*this)(i);
				T* src = rhs(i);

				for(cjAryOff j=0; j<ElementStride; j++)
					dst[j] = src[j];
			}
		}
	}

	cjTStrideArray() : cjAry() {}
	cjTStrideArray(cjAryOff initialNum) : cjAry(initialNum, ElementSiz) {}
	cjTStrideArray(const cjTStrideArray<T, TStride>& rhs) : cjAry(rhs.m_numElement, ElementSiz)
	{	
		if(cjTypeInfo<T>::HasDestructor())
		{
			m_numElement = 0;
			for(cjAryOff i=0; i<rhs.m_numElement; i++)
				::new(*this) T(rhs(i)); // do not use new(*this) T(rhs(i) !!
		}
		else
		{
			Cpy(rhs);
		}
	}

	~cjTStrideArray()
	{
		Remove(0, m_numElement); // with calling destructor ?
	}

    T* operator()(cjAryOff i) { return &(((T*)m_data)[i*ElementStride]); }
	const T* operator()(cjAryOff i) const { return &((T*)m_data)[i*ElementStride]; }

	// warn! - pop item has called destructor
	T* Pop()
	{
		T* rst = (*this)(m_numElement-1);
		Remove(m_numElement-1);
		return rst;
	}

	T* Last(cjAryOff c=0)
	{
		return (*this)(m_numElement-c-1);
	}

	const T* Last(cjAryOff c=0) const
	{
		return (*this)(m_numElement-c-1);
	}

	void Shrink()
	{
		cjAry::Shrink(ElementSiz);
	}

	cjAryOff PushEmpty(cjAryOff n=1)
	{
		return cjAry::PushEmpty(n, ElementSiz);
	}

	// this function does not call destructor
	void SetSiz(cjAryOff n=1) // use RemoveAll(newSiz)
	{
		cjAry::SetSiz(n, ElementSiz);
	}

	void Insert(cjAryOff index, cjAryOff count=1)
	{
		cjAry::Insert(index, count, ElementSiz);
	}

	void Remove(cjAryOff index, cjAryOff count=1)
	{
		if(cjTypeInfo<T>::HasDestructor())
		{
			T* cur = (*this)(index);
			for(cjAryOff i=index; i< (index+count) * ElementStride; i++)
			{
				T* next = cur += sizeof(ElementType);
				cur->~T();
				cur = next;
			}
		}

		cjAry::Remove(index, count, ElementSiz);
	}

	void RemoveAll(cjAryOff maxCount=0)
	{
		if(cjTypeInfo<T>::HasDestructor())
		{
			T* cur = (*this)(0);

			for(cjAryOff i=0; i<m_numElement * ElementStride; i++)
			{
				T* next = cur += sizeof(ElementType);
				cur->~T();
				cur = next;
			}
		}

		cjAry::RemoveAll(ElementSiz, maxCount);
	}

	cjAryOff Push(const T* item)
	{
		cjAryOff index;

//		if(cjTypeInfo<T>::HasDestructor())
//		{
			//cjTStrideArray<T>& dst = *((cjTStrideArray<T>*)&(*this)(index));
			::new(*this) T;  // do not use new(*this) T;
			index = m_numElement-1;
//		}
//		else
//		{
//			index = Add();
//		}

		T* cur = (*this)(index);

		if(cjTypeInfo<T>::HasDestructor())
		{
			for(cjAryOff i=0; i<ElementStride; i++)
				cur[i] = *(item+i);
		}
		else
		{
			cjMemCpy(cur, item, ElementSiz);
		}

		return index;
	}

	cjBool IsEmpty()
	{
		return m_numElement == 0 ? cjTrue : cjFalse;
	}

	void Fill(const T& f)
	{
		if(cjTypeInfo<T>::HasDestructor())
		{
			for(cjAryOff i=0; i<m_numElement; i++)
				::new(*this, i) T;
		}
		else
		{
//			if(sizeof(T) == 1)
//			{
				cjMemSet((*this)(0), f, ElementSiz * m_numElement);
			//}
			//else
			//{
			//	for(cjAryOff i=0; i<m_numElement; i++)
			//		cjMemCpy(&(*this)(i), &f, sizeof(T));
			//}
		}
	}

	class Iterator
	{
	public:
		Iterator(cjTStrideArray<T, TStride>& ary, cjAryOff baseIndex=0) 
			: m_ary(ary), m_baseIndex(baseIndex), m_index(-1), m_cur(NULL) { ++(*this); }

		void operator++() { ++m_index; m_cur=m_ary(m_baseIndex+m_index); }

		void Erase() { m_ary.Remove(m_baseIndex+m_index--); m_cur=m_ary(m_baseIndex+m_index); }
		cjAryOff GetIndex() const { return m_baseIndex+m_index; }
		
		operator cjBool() const { return m_baseIndex+m_index < m_ary.GetNum(); }

		// ary(0)[0]  ary(0)[1]  ary(0)[2]
		const T& operator[](cjAryOff indexInStrideBlock) const 
		{ 
			#ifdef CJDEBUG
				cjAssert(indexInStrideBlock < TStride);
			#endif

			return m_cur[indexInStrideBlock]; 
		}

		T& operator[](cjAryOff indexInStrideBlock)
		{ 
			#ifdef CJDEBUG
				cjAssert(indexInStrideBlock < TStride);
			#endif

			return m_cur[indexInStrideBlock]; 
		}

		cjBool isEnd() const { return m_index < m_ary.GetNum() ? cjTrue : cjFalse; }		
		cjBool isBegin() const { return m_index == 0 ? cjTrue : cjFalse; }

	private:
		cjTStrideArray<T, TStride>&			m_ary;
		cjAryOff							m_baseIndex;
		cjAryOff							m_index;
		T*									m_cur;
	};

}; 


template <class T, cjUINT32 TStride> 
inline void* operator new(size_t , cjTStrideArray<T, TStride>& ary)
{
	cjAryOff index = ary.cjAry::PushEmpty(1, cjTStrideArray<T, TStride>::ElementSiz);
	return ary(index);
}

template <class T, cjUINT32 TStride> 
inline void* operator new(size_t , cjTStrideArray<T, TStride>& ary, cjINT32 index)
{
	return ary(index);
}



//
// cjTUniqueAry
//

template <class T>
class cjTUniqueAry : public cjTAry<T>
{
public:
	cjAryOff Push(const T& item)
	{		
		return PushUnique(item);
	}
};



// end of cjTStrideArray

// sum and subtract in 64bit
//int A = -2;
//unsigned B = 1;
//int array[5] = { 1, 2, 3, 4, 5 };
//int *ptr = array + 3;
//ptr = ptr + (A + B); //Error
//printf("%i\n", *ptr);
//
// This code will be correctly performed in the 32-bit version and print number "3". 
// After compilation in 64-bit mode there will be a fail when executing the code.
// Let's examine the sequence of code execution and the cause of the error:
// - A variable of int type is cast into unsigned type;
// - A and B are summed. As a result, we get 0xFFFFFFFF value of unsigned type;
// - ptr + 0xFFFFFFFFu" expression is calculated. 
// The result depends on the pointer's size on the current platform. 
// In the 32-bit program, the expression will be equal to "ptr - 1" and we will successfully print number 3. 
// In the 64-bit program, 0xFFFFFFFFu value will be added to the pointer and as a result, 
// the pointer will be far beyond the array's limits.

template<typename T, cjUINT32 numElement=1>
class cjTStrideRandomAccessIterator
{
public:
	cjTStrideRandomAccessIterator() : m_src(NULL), m_cur(NULL), m_stride(sizeof(T)), m_numElement(numElement), 
		m_accessStride(m_stride * m_numElement) {}

	cjTStrideRandomAccessIterator(void* src) : m_src((cjBYTE*)src), m_cur((cjBYTE*)src), m_stride(sizeof(T)),
		m_numElement(numElement), m_accessStride(m_stride * m_numElement) {}

	cjBool IsValid() const { return m_src ? cjTrue : cjFalse; }

	// accessor
	T& operator()(cjAryOff offset) { cjAssert(offset <= numElement); return *reinterpret_cast<T*>(m_cur + m_stride * offset); }
	T& operator[](cjAryOff offset) { return *reinterpret_cast<T*>(m_src + m_accessStride * offset); }

	T& operator*() { return *reinterpret_cast<T*>(m_cur); }
//	T* operator->() { return reinterpret_cast<T*>(m_src); }

	// prefix operator 
	cjTStrideRandomAccessIterator& operator++() 
	{ 
		m_cur += m_accessStride;
		return *this; 
	}

	cjTStrideRandomAccessIterator& operator--() 
	{ 
		m_cur -= m_accessStride; 
		cjAssert(m_cur >= m_src);
		return *this; 
	}

	// cjSlow
	// postfix operator
	//cjTStrideRandomAccessIterator operator++(int) 
	//{ 
	//	cjTStrideRandomAccessIterator rst = *this;
	//	m_cur += m_accessStride; 
	//	return rst;
	//}

	//cjTStrideRandomAccessIterator operator--(int)
	//{ 
	//	cjTStrideRandomAccessIterator rst = *this;
	//	m_cur -= m_accessStride;
	//	cjAssert(m_cur >= m_src);
	//	return rst;
	//}
	
	cjTStrideRandomAccessIterator& operator+=(ptrdiff_t num) 
	{ 
		m_cur += m_accessStride * num;
		return *this; 
	}

	cjTStrideRandomAccessIterator& operator-=(ptrdiff_t num) 
	{ 
		m_cur -= m_accessStride * num; 
		cjAssert(m_cur >= m_src);
		return *this; 
	}

	//cjTStrideRandomAccessIterator operator+(ptrdiff_t num) const
	//{ 
	//	return cjTStrideRandomAccessIterator(reinterpret_cast<T*>(m_cur + m_accessStride * num), m_numElement); 
	//}

	//cjTStrideRandomAccessIterator operator-(ptrdiff_t num) const
	//{ 
	//	return cjTStrideRandomAccessIterator(reinterpret_cast<T*>(m_cur - m_accessStride * num), m_numElement); 
	//}

	ptrdiff_t operator-(const cjTStrideRandomAccessIterator& dst) const	
	{ 
		return (m_cur - dst.m_cur) / m_accessStride; 
	}

	bool operator == (const cjTStrideRandomAccessIterator& dst) const
	{ 
		return (m_src == dst.m_src) && (m_cur == dst.m_cur) && (m_stride == dst.m_stride) 
			&& (m_numElement == dst.m_numElement); 
	}

	bool operator != (const cjTStrideRandomAccessIterator& dst) const
	{ 
		return !operator==(dst); 
	}

	bool operator<(const cjTStrideRandomAccessIterator& dst) const { return m_cur < dst.m_cur; }
	bool operator>(const cjTStrideRandomAccessIterator& dst) const { return m_cur > dst.m_cur; }
	bool operator<=(const cjTStrideRandomAccessIterator& dst) const { return m_cur <= dst.m_cur; }
	bool operator>=(const cjTStrideRandomAccessIterator& dst) const { return m_cur >= dst.m_cur; }

	void Reset() { m_cur = m_src; }
	void SetPos(ptrdiff_t index) { m_cur = m_src + m_accessStride * index; }

	cjBYTE* GetSrc() { return m_src; }
	cjSiz_t GetElemSiz() { return m_stride * m_numElement; }

private:
	cjBYTE*									m_src;
	cjBYTE*									m_cur;
	ptrdiff_t								m_stride;		// ptrdiff_t to calculate in pointer	
	ptrdiff_t								m_numElement;

	ptrdiff_t								m_accessStride;
};


//template<class T, cjUINT32 numElement>
//class cjTStrideIterator
//{
//public:
//	typedef cjUINT32			OffsetType;
//
//	enum 
//	{ 
//		ElementSiz = sizeof(T),
//		ElementNum = numElement
//	};
//
//	cjTStrideIterator(void* src, OffsetType numItem=(OffsetType)-1)
//	{
//		m_src = src;
//		m_endPos = numItem;
//		m_pos = 0;
//	}
//
//	cjTStrideIterator(const void* src, OffsetType numItem=(OffsetType)-1)
//	{
//		m_src = const_cast<void*>(src);
//		m_endPos = numItem;
//		m_pos = 0;
//	}
//
//	T& operator[](cjAryOff index) const 
//	{ 
//		return *reinterpret_cast<T*>(m_src + m_stride * index); 
//	}
//
//	T& operator*() const 
//	{ 
//		return *reinterpret_cast<T*>(m_src); 
//	}
//
//	T* operator->() const 
//	{ 
//		return reinterpret_cast<T*>(m_src); 
//	}
//
//
//private:
//	void*						m_src;
//	OffsetType					m_endPos;
//
//	OffsetType					m_pos;
//};


//
// cjTDeferLoadAry
//

template <class T> 
class cjTAryDeferLoad : public cjTAry<T>, public cjFDeferLoad
{
public:
	cjTAryDeferLoad(cjINT32 num=0) : cjTAry<T>(num), cjFDeferLoad() {}
	~cjTAryDeferLoad()
	{
		if(m_deferStream)
			m_deferStream->RemoveDeferLoad(this);
	}

	//T& operator()(cjAryOff i)
	//{
	//	if(m_deferLoadPos > 0)
	//		Load();

	//	return ((T*)Data)[i];
	//}
	//const T& operator()(cjAryOff i) const
	//{
	//	if(m_deferLoadPos > 0)
	//		Load();

	//	return ((T*)Data)[i];
	//}

	void ValidateLoad()
	{		
		if(m_deferLoadPos > 0)
		{
			cjStreamOffset oldPos = m_deferStream->GetPos();

			m_deferStream->SetPos(m_deferLoadPos);
			*m_deferStream << (cjTAry<T>&)*this;
			m_deferLoadPos *= -1; // make state that UnLoad() is callable

				// restore
			m_deferStream->SetPos(oldPos);
		}
	}

	void Unload()
	{
		if(m_deferLoadPos < 0)
		{
			cjTAry<T>::RemoveAll(); // cjTAry's
			m_deferLoadPos *= -1; // make state that LoadDeferred() is callable
		}
	}

	friend cjFStream& operator<<(cjFStream& stream, cjTAryDeferLoad& defer)
	{
		if(stream.IsLoading())
		{
			cjStreamOffset nextLoadPos=0;

			stream << nextLoadPos;
			stream.AddDeferLoad(&defer);

			if(!ms_enableDeferLoading)
				defer.ValidateLoad();

			stream.SetPos(nextLoadPos);
		}
		else if(stream.IsSaving())
		{
			cjStreamOffset saveStPos = stream.GetPos();

			stream << saveStPos << (cjTAry<T>&)defer;			
			cjStreamOffset saveEnPos = stream.GetPos();
			
			// overwrite saveEnPos at original saveStPos
			stream.SetPos(saveStPos);
			stream << saveEnPos;

			stream.SetPos(saveEnPos);
		}
		else
		{
			// not loading and not saving
			stream << (cjTAry<T>&)defer;
		}

		return stream;
	}

};


#if defined(CJ_PLATFORM_WIN32)
#pragma warning(pop)
#endif


#endif
