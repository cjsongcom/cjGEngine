
#include "cjCorePCH.h"
#include "cjRef.h"


CJSER_IMPL_CLASS(cjRef, cjSerializable)

cjINT32 cjRef::ms_totalCount;


cjINT32 cjRef::ReleaseRef(void)
{
	cjAtomicDec(m_refCount);

#if defined(CJ_PLATFORM_WIN32)

    // sizeof(LONG) in win32/win64 is always 32
	if(::InterlockedCompareExchange((LONG*)&m_refCount, (LONG)0, 0) == 0)
	{
		cjSerSetFlag(CJSER_FL_REF_DELETEABLE);
		DelInst();

		return 0;
	}
	else
	{
		if(::InterlockedCompareExchange((LONG*)&m_refCount, (LONG) -1, -1) == -1)
		{
			// if refCount < 0 , something wrong state is going..
			cjAssert(!"ERR! - RefCount is under zero..");
		}

		return m_refCount;
	}

#elif defined(CJ_PLATFORM_APPLE)

    // sizeof(LONG) in ios is 32 for armv7 , 64 for arm64
    // so using int instead for
	if(::OSAtomicCompareAndSwap32Barrier(0, 0, (cjINT32*)&m_refCount))
	{
		DelInst();

		return 0;
	}
	else
	{
		if(::OSAtomicCompareAndSwap32Barrier(-1, -1, (cjINT32*)&m_refCount))
		{
			// if refCount < 0 , something wrong state is going..
			cjAssert(!"ERR! - RefCount is under zero..");
		}

		return m_refCount;
	}
#else
	#error "not implemented"
#endif

	// no more code is here
}


cjSerializable* cjRef::Clone() 
{
	cjSerObjCloner cloner;
		
	// pre-pass, create instance, copy class property 
	cjSerializable* clone = _Clone(&cloner); 

	// post pass
	_PostClone(&cloner);

	return clone;
}

cjSerializable* cjRef::Clone(cjSerObjCloner* cloner) 
{	
	if(!cloner)
		return Clone();

	cjSerializable* clone = _Clone(cloner);
	_PostClone(cloner);

	return clone;
}

cjSerializable* cjRef::_CloneShared(cjSerObjCloner* cloner)
{
	cjRef* cloned = (cjRef*)cloner->GetCloned(this);

    if(cloned)
        return cloned;
    else
        return _Clone(cloner);
}

void cjRef::_PostClone(cjSerObjCloner* cloner)
{
	cloner->SetPostCloned(this);
}


//
// cjTRefPtr Defer Operation
//

cjBool cjTRefPtr_cjSerObjBroker_QuerySerObjAndStr(cjSerObjStreamBroker* serBroker)
{
    return serBroker->IsQuerySerObjAndStr();
}

cjBool cjTRefPtr_cjSerObjBroker_SavingOrLoading(cjSerObjStreamBroker* serBroker)
{
    return serBroker->IsSaving() || serBroker->IsLoading();
}

