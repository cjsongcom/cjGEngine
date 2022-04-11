
#ifndef _CJREF_H_
#define _CJREF_H_


class CJCORE_API cjRef : public cjSerializable
{
	CJSER_DECL_CLASS_ABSTRACT(cjRef, cjSerializable, CJRTTI_CF_NO_CLONE, CJCORE_API)
		
public:
	cjINT32	AddRef(void);
	cjINT32 ReleaseRef(void);

	cjINT32 GetRef(void) const { return m_refCount; }

public:
	virtual cjRef* GetInstAScjRef() { return this; }

	virtual cjSerializable* Clone();
	virtual cjSerializable* Clone(cjSerObjCloner* cloner);

	// cloning in instance which is derived cjRef is just increase reference count
	cjSerializable* _Clone(cjSerObjCloner* /*cloner*/) { return this; }
	cjSerializable* _CloneShared(cjSerObjCloner* cloner);
	void _PostClone(cjSerObjCloner* cloner);

	virtual cjSerializable* _CloneAsCopy() { return cjNULL; }	

protected:
	void _CloneProperty(cjRef* d, cjSerObjCloner* cloner) 
	{ 
		cjSerializable::_CloneProperty(d, cloner);

		if(cloner)
		{
			cjSerializable* s = this;
			cloner->m_clones->Set(s, d);
		}
	}
	 
private:
	// keep 4byte alignment
	cjINT32							m_refCount;
	cjUINT32						m_dbgID;
	static cjINT32					ms_totalCount;
};


inline cjRef::cjRef()
{
	m_refCount = 0;
	m_dbgID = 0;
	cjSerSetFlag(CJSER_FL_REF);
	cjAtomicInc(ms_totalCount);
}

inline cjRef::~cjRef() 
{ 
	cjAssert(m_refCount == 0);
	cjAtomicDec(ms_totalCount);	
}

inline cjINT32 cjRef::AddRef(void)
{
	cjAssert(m_refCount > -1);
	cjAtomicInc(m_refCount);

	return m_refCount;
}


#define CJ_REF_SAFE_ADD(p)		{ if(p) (p)->AddRef();				 }
#define CJ_REF_ADD(p)			(p)->AddRef();

#define CJ_REF_SAFE_RELEASE(p)	{ if(p) { (p)->ReleaseRef(); (p)=0; } } 
#define CJ_REF_RELEASE(p)		{ (p)->ReleaseRef(); (p)=0; }
#define CJ_REF_MUST_RELEASE(p)	{ (p)->ReleaseRef(); } 

#define CJ_REF_CHANGE_TO(dst, src) \
	{ \
		if( src != 0 ) (src)->AddRef(); \
		CJ_REF_SAFE_RELEASE(dst); \
		dst = src; \
	}


//
// cjRef Smart Pointer
//

//cjopti
cjBool cjTRefPtr_cjSerObjBroker_QuerySerObjAndStr(cjSerObjStreamBroker* serBroker);
cjBool cjTRefPtr_cjSerObjBroker_SavingOrLoading(cjSerObjStreamBroker* serBroker);


template <class T> 
class cjTRefPtr : public cjMem
{
public:
	typedef T	Type;

	friend cjFStream& operator<<(cjFStream& stream, cjTRefPtr& d)
	{
		if(stream.IsObjOnly())
			return stream;

		cjSerObjStreamBroker* serBroker = stream.GetSerializerBroker();
		
		if(!serBroker)
		{
			cjAssert(!"Failed to serialize.. need cjSerObjStreamBroker derived stream..");
			return stream;
		}

		if(cjTRefPtr_cjSerObjBroker_QuerySerObjAndStr(serBroker))
		{
			if(d)
			{
				(*d) >> *serBroker;
			}
		}
		else if(cjTRefPtr_cjSerObjBroker_SavingOrLoading(serBroker))
		{
			cjAssert(!"this must be not called.. use stream.SerializeSerObjPtr(&m_ptr) .. ");

			/*cjBYTE isValidRPtr = (d != cjNULL) ? 1: 0;
			(*serBroker) << isValidPtr;

			if(isValidPtr)
			{
				T* tmp = d;

				if(serBroker->IsLoading())
				{
					tmp = T::NewInst();
					*tmp >> *serBroker;

					d = tmp;
				}
				else
				{
					*tmp >> *serBroker;
				}
			}
			else
			{
				if(serBroker->IsLoading())
					d = cjNULL;
			}*/
		}

		return stream;
	}

public:
	inline cjTRefPtr() { _m_refPtr = cjNULL; }
    inline cjTRefPtr(T* inRef);
    inline cjTRefPtr(const cjTRefPtr& refPtr);
    
	inline ~cjTRefPtr()
	{
		if(_m_refPtr)
			_m_refPtr->ReleaseRef();
	}
	
    inline operator T*() const;
    inline T& operator*() const;
    inline T* operator->() const;
	inline T* _Ptr() const;

    inline cjTRefPtr& operator=(const cjTRefPtr& refPtr);
    inline cjTRefPtr& operator=(T* refPtr);

    inline cjBool operator==(T* refPtr) const;
    inline cjBool operator!=(T* refPtr) const;
    inline cjBool operator==(const cjTRefPtr& refPtr) const;
    inline cjBool operator!=(const cjTRefPtr& refPtr) const;

private:
    T* _m_refPtr;
};

#define cjRefPtrDefine(CLS_NAME) \
    class CLS_NAME; \
    typedef cjTRefPtr<CLS_NAME> CLS_NAME##RPtr; \
	CJTYPEINFO_SUPPORT_IMPL(cjTRefPtr<CLS_NAME>) 

#define cjRefPtrForwardDefine(CLS_NAME) \
    class CLS_NAME; \
    typedef cjTRefPtr<CLS_NAME> CLS_NAME##RPtr;


template <class T>
inline cjTRefPtr<T>::cjTRefPtr(T* refPtr)
{
	_m_refPtr = refPtr;
	if(_m_refPtr)
		_m_refPtr->AddRef();
}

template <class T>
inline cjTRefPtr<T>::cjTRefPtr(const cjTRefPtr& refPtr)
{
    _m_refPtr = refPtr._m_refPtr;
    if(_m_refPtr)
        _m_refPtr->AddRef();
}



template <class T>
inline cjTRefPtr<T>::operator T*() const
{
    return _m_refPtr;
}

template <class T>
inline T& cjTRefPtr<T>::operator*() const
{
    return *_m_refPtr;
}

template <class T>
inline T* cjTRefPtr<T>::operator->() const
{
    return _m_refPtr;
}

template <class T>
inline T* cjTRefPtr<T>::_Ptr() const
{
    return _m_refPtr;
}

template <class T>
inline cjTRefPtr<T>& cjTRefPtr<T>::operator=(const cjTRefPtr& refPtr)
{
	if(_m_refPtr != refPtr._m_refPtr)
	{
		if(_m_refPtr)
			_m_refPtr->ReleaseRef();

		_m_refPtr = refPtr._m_refPtr;

		if(_m_refPtr)
			_m_refPtr->AddRef();
	}

	return *this;
}

template <class T>
inline cjTRefPtr<T>& cjTRefPtr<T>::operator=(T* refPtr)
{
    if(_m_refPtr != refPtr)
    {
        if(_m_refPtr)
            _m_refPtr->ReleaseRef();

        _m_refPtr = refPtr;

        if(_m_refPtr)
            _m_refPtr->AddRef();
    }

    return *this;
}

template <class T>
inline cjBool cjTRefPtr<T>::operator==(T* refPtr) const
{
    return (_m_refPtr == refPtr);
}

template <class T>
inline cjBool cjTRefPtr<T>::operator!=(T* refPtr) const
{
    return (_m_refPtr != refPtr);
}

template <class T>
inline cjBool cjTRefPtr<T>::operator==(const cjTRefPtr& refPtr) const
{
    return (_m_refPtr == refPtr._m_refPtr);
}

template <class T>
inline cjBool cjTRefPtr<T>::operator!=(const cjTRefPtr& refPtr) const
{
    return (_m_refPtr != refPtr._m_refPtr);
}


//
// cjSerObjStreamBroker
//

class CJCORE_API cjSerObjStreamBroker : public cjSerializer
{
protected:
	virtual cjFStream& operator<< (cjName& name);
	virtual cjStreamOffset SerializeStream(void* v, cjStreamOffset siz);

	virtual cjBool Flush();
	virtual cjSerObjStreamBroker* GetSerializerBroker() { return this; }

public:
	// debug support
	virtual cjStreamOffset GetDebugCurContentPos(cjWORD serial=0) 
	{ 
		return m_dstStream ? m_dstStream->GetDebugCurContentPos(serial) : -1; 
	}

	virtual cjStreamOffset GetDebugLogicalPos() { return  m_dstStream ? m_dstStream->GetDebugLogicalPos() : 0; }
	
	virtual void ValidateStreamDBGInfo()
	{
		if(m_dstStream)
			m_dstStream->ValidateStreamDBGInfo();
	}

	virtual cjWORD BeginDBGSerInfo(cjWORD* rstDbgSerial, const cjNameChar* rttiName, 
		const cjNameChar* name, const cjChar* func, const cjUINT32 line, const cjChar* fname)
	{
		if(m_dstStream)
			return m_dstStream->BeginDBGSerInfo(rstDbgSerial, rttiName, name, func, line, fname);
		else
			return 0;
	}

	virtual void EndDBGSerInfo(const cjChar* func, const cjUINT32 line, const cjChar* fname, cjWORD lastSerial)
	{
		if(m_dstStream)
			m_dstStream->EndDBGSerInfo(func, line, fname, lastSerial);
	}

	virtual void Report(const cjNameChar* filepath)
	{
		if(m_dstStream)
			m_dstStream->Report(filepath);
	}

public:
	cjSerObjStreamBroker(cjFStream* dstSteram);
	virtual ~cjSerObjStreamBroker();

	template<class T>
	cjBool Serialize(cjTRefPtr<T>& serObj)
	{
		T* serialized = serObj;

		if(IsLoading())
		{
			cjBool r;			
			r = Serialize((cjSerializable**)&serialized);

			if(serialized)
			{
				if(!cjrIsKindOf<T>(serialized))
				{
					cjAssert(!"[ERR!] Failed to serialze (loading)..");
					return cjFalse;
				}
			}
			
			cjAssert(serialized->GetRef() == 1);
			serObj = serialized; // ref=2						
			serialized->ReleaseRef(); // ref=1

			return r;
		}

		return Serialize((cjSerializable**)&serialized);
	}
	
	template<class T>
	cjBool Serialize(T** serObj)
	{
		return Serialize((cjSerializable**)serObj);
	}

	cjBool Serialize(cjSerializable** serObj);

private:
	cjSerObjStreamBroker() {}

private:
	cjFStream*						m_dstStream;
};


typedef cjTRefPtr<cjRef>			cjRefPtr;


#endif

