
#ifndef _CJTLIST_H_
#define _CJTLIST_H_


template <class T, class TAllocator=cjTDefaultAlloc<T> > 
class cjTListElem : public cjMem
{
public:
	typedef TAllocator Allocator;

	T& operator *() { return m_item; }
	const T& operator *() const { return m_item; }

    cjTListElem*						m_next;
    cjTListElem*						m_prev;
    T									m_item;
};

//template <class T, class TAllocator=cjTDefaultAlloc<T> > class cjTListElem;

//typedef void* cjTListIter;


template <class T, class TAllocator=cjTDefaultAlloc<T> > 
class cjTList : public cjMem
{
public:
	typedef cjTListElem<T, TAllocator>	Element;
	typedef Element*					Iter;

	cjTList()
	{
		m_head = 0;
		m_tail = 0;
		m_count = 0;
	}

	~cjTList() { RemoveAll(); }

	cjUINT32 Count() const { return m_count; }
	cjBool IsEmpty() const { return m_count == 0 ? cjTrue : cjFalse; }

	void RemoveAll()
	{
		Element* cur = m_head;

		while(cur)
		{
			Element* toDel = cur;

			cur = cur->m_next;
			_DelElement(toDel);
		}

		m_head = cjNULL;
		m_tail = cjNULL;
	}

	// push/pop/remove
	void PushFront(const T& item)
	{
		Element* elem = _NewElement();
		elem->m_item = item;

		_PushFront(elem);
	}

	void PushBack(const T& item)
	{
		Element* elem = (Element*)_NewElement();
		elem->m_item = item;

		_PushBack(elem);
	}

	Iter PushBefore(const Iter iter, const T& item)
	{
		if(!iter)
			return cjNULL;

		Element* elem = _NewElement();
		elem->m_item = item;

		return _PushBefore(iter, elem);
	}

	Iter PushAfter(const Iter iter, const T& item)
	{
		if(!iter)
			return cjNULL;

		Element* elem = _NewElement();
		elem->m_item = item;

		return _PushAfter(iter, elem);
	}

	Iter RemoveByIter(Iter iter) 
	{
		if(!iter)
			return cjNULL;

		Element* curItem = (Element*)iter;
		Element* nextItem = curItem->m_next;
								
		if(curItem->m_prev)
		{
			curItem->m_prev->m_next = nextItem;
		}
		else
		{
			cjAssert(curItem == m_head);
		}

		if(m_head == curItem)
			m_head = nextItem;

		if(nextItem)
		{
			nextItem->m_prev = curItem->m_prev;
		}
		else
		{
			cjAssert(curItem == m_tail);
		}

		if(m_tail == curItem)
			m_tail = nextItem;

		_DelElement(curItem);

		return (Iter)nextItem;
	}

	T& GetHeadV()
	{
		return m_head->m_item;
	}

	T& GetBackV()
	{
		return m_tail->m_item;
	}

	Iter RemoveHeadV(T& item)
	{
		if(!m_head)
			return m_head;

		Element* curItem = m_head;
		m_head = m_head->m_next;

		if(m_head)
		{
			m_head->m_prev = cjNULL;
		}
		else
		{
			cjAssert(curItem == m_tail);
		}

		if(m_tail == curItem)
			m_tail = m_head;

		_DelElement(curItem);

		return (Iter)(m_head->m_next);
	}
	
	Iter Remove(const T& item, Iter stIter=cjNULL) 
	{
		Iter curIter = stIter;

		if(!curIter)
			curIter = m_head;

		if(!curIter)
			return cjNULL;

		do 
		{
			Element* curItem = (Element*)curIter;

			if(curItem->m_item == item)
			{
				Element* nextItem = curItem->m_next;
								
				if(curItem->m_prev)
				{
					curItem->m_prev->m_next = nextItem;
				}
				else
				{
					cjAssert(curItem == m_head);
				}
				
				if(m_head == curItem)
					m_head = nextItem;

				if(nextItem)
				{
					nextItem->m_prev = curItem->m_prev;
				}
				else
				{
					cjAssert(curItem == m_tail);
				}

				if(m_tail == curItem)
					m_tail = nextItem;

				_DelElement(curItem);

				return (Iter)nextItem;
			}

			curIter = curItem->m_next;

		} while (curIter);

		return cjNULL;
	}

	static T& GetValue(const Iter& iter) { return ((Element*)iter)->m_item; }
	//static const T& GetValue(const Iter& iter) { return ((Element*)iter)->m_item; }
		
	// iterator
	Iter Begin() const { return m_head; }
	Iter End() const { return m_tail; }  

	cjBool GetNext(Iter& iter) const 
	{ 
		if(iter)
		{
			iter = ((Element*)iter)->m_next;
			return cjTrue;
		}
		else
			return cjFalse;
	}

	cjBool GetNextV(Iter& iter, T& v) const 
	{ 
		if(iter)
		{
			v = ((Element*)iter)->m_item;
			iter = ((Element*)iter)->m_next;

			return cjTrue;
		}
		else
			return cjFalse;
	}

	cjBool GetPrev(Iter& iter) const 
	{
		if(iter)
		{
			iter = ((Element*)iter)->m_prev;

			return cjTrue;
		}
		else
			return cjFalse;
	}

	cjBool GetPrevV(Iter& iter, T& v) const 
	{
		if(iter)
		{
			v = ((Element*)iter)->m_item;
			iter = ((Element*)iter)->m_prev;

			return cjTrue;
		}
		else
			return cjFalse;
	}

	Iter Find(const T& item, Iter stIter=0) const
	{
		Element* curElem = stIter ? (Element*)stIter : (Element*)m_head; 

		while(curElem)
		{
			if(curElem->m_item == item)
				return (Iter)curElem;

			curElem = curElem->m_next;
		}

		return (Iter)cjNULL;
	}

protected:

	Element* _NewElement()
	{
		// this will be processed Element::Allocator::Alloc()
//		if(cjTypeInfo<T>::HasDestructor())
//			cjTypeInfo<T>::CallConstructor(elem->m_item);

		Element* elem = (Element*)Element::Allocator::Alloc();


		return elem;
	}

	void _DelElement(Element* elem)
	{
		cjAssert(m_count != 0);
		m_count--;

		// this will be processed Element::Allocator::DeAlloc(elem);
		// if(cjTypeInfo<T>::HasDestructor())
		//   cjTypeInfo<T>::CallDestructor(elem->m_item);

		Element::Allocator::DeAlloc(elem);
	}
	
	void _PushBack(Element* elem)
	{
		elem->m_next = cjNULL;
		elem->m_prev = m_tail;
    
		if(m_tail)
			m_tail->m_next = elem;
		else
			m_head = elem;
    
		m_tail = elem;

		m_count++;
	}

	void _PushFront(Element* elem)
	{
		elem->m_prev = cjNULL;
		elem->m_next = m_head;
    
		if(m_head)
			m_head->m_prev = elem;
		else
			m_tail = elem;
    
		m_head = elem;

		m_count++;
	}

	Iter _PushAfter(Iter pos, Element* elem)
	{		
		Element* cur = (Element*)pos;

		elem->m_prev = cur;
		elem->m_next = cur->m_next;

		Element* curNext = cur->m_next;

		if(curNext)
		{
			curNext->m_prev = elem;
		}
		else
		{
			cjAssert(cur == m_tail);
			m_tail = elem;
		}
		
		cur->m_next = elem;

		m_count++;

		return (Iter)elem;
	}

	Iter _PushBefore(Iter pos, Element* elem)
	{
		Element* cur = (Element*)pos;

		elem->m_next = cur;
		elem->m_prev = cur->m_prev;
    
		if(cur->m_prev)
		{
			cur->m_prev->m_next = elem;
		}
		else
		{
			cjAssert(cur == m_head);
			m_head = elem;
		}

		cur->m_prev = elem;

		m_count++;

		return (Iter)elem;
	}

	Element*								m_head;
	Element*								m_tail;

	cjUINT32								m_count;

private:
	cjTList(const cjTList&) {}
	cjTList& operator=(const cjTList&) {}
};


template <class T> 
class cjTPtrList : public cjTList<T, cjTPtrPoolAlloc<size_t> >
{
};


#endif

