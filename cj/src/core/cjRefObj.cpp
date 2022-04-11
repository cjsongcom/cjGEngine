
#include "cjCorePCH.h"
#include "cjRefObj.h"


CJSER_IMPL_CLASS(cjRefObj, cjRef)

// serialize
CJSER_IMPL_SERIALIZE_BEGIN(cjRefObj)

	if(ser.IsQuerySerObjAndStr())
	{
		cjAniCntr::SerializeList(ser, m_spAniCntr);
		return;
	}

	if(ser.IsSaving() || ser.IsLoading())
	{
		cjBool isLoading = ser.IsLoading();
		cjINT32 count = isLoading ? 0 : m_extDatCount;

		if(isLoading) { cjAssert(m_extDatAry == 0); }

		ser << _flag;
		ser << m_name;
		ser << count;

		cjSLOG(L"Name=%s , extDatNum count=%d \n", *m_name, count);

		if(isLoading) 
			m_extDatMaxCount = m_extDatCount = (cjWORD)count;

		if(m_extDatCount > 0)
			cjExtraDat::SerializeAry(ser, &m_extDatAry, isLoading ? 0 : &count);

		cjAniCntr::SerializeList(ser, m_spAniCntr);
	}

CJSER_IMPL_SERIALIZE_END(cjRefObj)

CJSER_IMPL_SERIALIZE_POST_BEGIN(cjRefObj)

	if(ser.IsLoading())
		cjAniCntr::PostSerializeList(ser, m_spAniCntr);

CJSER_IMPL_SERIALIZE_POST_END(cjRefObj)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjRefObj)

	cloner->SetCloned(this, d);

	d->_flag			= _flag;
	d->m_name			= m_name;

	// this will performed by below ,  d->AddExtDat(cloneExtDat);
	//d->m_extDatCount	= m_extDatCount; 
	//d->m_extDatMaxCount	= m_extDatMaxCount;
	for(cjWORD i=0; i<m_extDatCount; i++)
    {
		cjExtraDat* curExtDat = m_extDatAry[i];

		if(curExtDat->IsCloneable())
		{
			cjExtraDat* cloneExtDat = (cjExtraDat*)(curExtDat->_Clone(cloner));
			d->AddExtDat(cloneExtDat);
		}
    }

    if(m_spAniCntr) // recurive cloning will be occured in cjAniCntr
		d->ReplaceAniCntrList((cjAniCntr*)m_spAniCntr->_Clone(cloner));

CJSER_IMPL_CLONE_PROPCOPY_END(cjRefObj)

CJSER_IMPL_CLONE(cjRefObj)

CJSER_IMPL_CLONE_POST_BEGIN(cjRefObj)

    for(cjWORD i=0; i<m_extDatCount; i++)
    {
        cjExtraDat* curExtDat = m_extDatAry[i];
        curExtDat->_PostClone(cloner);
    }

    if(m_spAniCntr)
        m_spAniCntr->_PostClone(cloner);

CJSER_IMPL_CLONE_POST_END(cjRefObj)


cjRefObj::cjRefObj() : m_name(cjName_None)
{
	_flag = 0;
	m_extDatAry = NULL;
	m_extDatCount = 0;
	m_extDatMaxCount = 0;
}

cjRefObj::cjRefObj(const cjName& name)
{
	_flag = 0;
	m_name = name;
}

cjRefObj::~cjRefObj()
{
	RemoveAllExtDat();
}

cjBool cjRefObj::AddExtDat(const cjName& key, cjExtraDat* dat)
{
    if(!key.IsValid())
        return cjFalse;

    if(!dat)
       return cjFalse;
    
    if(!dat->GetName().IsValid())
    {
        dat->SetName(key);
    }
    else if(key != dat->GetName())
    {
        return cjFalse;
    }

    return InsertExtDat(dat);
}

cjBool cjRefObj::AddExtDat(cjExtraDat* dat)
{
    if(!dat)
		return cjFalse;
    
	// if extra data is nonamed
    if(!dat->GetName().IsValid())
    {
		const cjUni* rttiName = dat->cjrRTTI()->cjrGetNameW();
		const static cjUINT32 PREFIX_SIZ = 8; // strlen("cjExtDat")

        if(rttiName && (cjStrlen(rttiName) > 0))
        {
            cjUni suffix[6];
            cjSprintf(suffix, 6, _CU("ED%03d"), m_extDatCount);

			// cjExtDatStr -> String
			if(cjStrncmp(rttiName, _CU("cjExtDat"), PREFIX_SIZ))
			{
				cjAssert(0 && "bad extra data founded");
				return cjFalse;
			}

			cjUINT32 genKeyNameLen = cjStrlen(suffix) + cjStrlen(rttiName+PREFIX_SIZ)+1;
			cjUni* genKeyName = (cjUni*)cjMemAlloc(sizeof(cjUni)*genKeyNameLen);

			cjStrcpy(genKeyName, rttiName+PREFIX_SIZ);
			cjStrcat(genKeyName, genKeyNameLen, suffix);
			
            dat->SetName(genKeyName);
			cjMemDeAlloc(genKeyName);
        }
        else
        {
            cjAssert(0 && "ERR! - bad rttiname");
        }
    }

    return InsertExtDat(dat);
}

cjExtraDat* cjRefObj::GetExtDat(const cjName& key) const
{
    if(!key.IsValid())
        return NULL;

    cjSWORD bot = 0, mid = 0;
    cjSWORD top = (cjSWORD)m_extDatCount - 1;

	// using binary search
    while(bot <= top)
    {
        mid = (top + bot) >> 1;
        cjINT32 r = cjStricmp(*key, *(m_extDatAry[mid]->GetName()));

        if(r==0)
            return m_extDatAry[mid];
        else if(r > 0)
            bot = mid + 1; // key > mid key
        else 
            top = mid - 1; // key < mid key
    }

    return 0; // can't find
}

cjBool cjRefObj::RemoveExtDat(const cjName& key)
{
    if(!m_extDatCount) return 0;
    if(!key.IsValid()) return cjFalse;

    cjSWORD bot=0, mid = 0;
    cjSWORD top = (cjSWORD)m_extDatCount - 1;

	// using binary search
    while(bot <= top)
    {
        mid = (top + bot) >> 1;

		cjINT32 r = cjStrcmp(*key, *m_extDatAry[mid]->GetName());
		if(r == 0)
        {
            DeleteExtDat(mid);
            return cjTrue;
        }
        else if(r > 0)
            bot = mid + 1;
        else
            top = mid - 1;
    }

    return cjFalse;
}

cjBool cjRefObj::RemoveExtDatByIndex(cjWORD idx)
{
    if(idx < m_extDatCount)
    {
        DeleteExtDat(idx);
        return cjTrue;
    }

    return cjFalse;
}

void cjRefObj::RemoveAllExtDat()
{
/*    
	for(cjSWORD i=(cjSWORD)m_extDatCount-1; i>=0; i--)
        DeleteExtDat((cjWORD)i);

	if(m_extDatAry)
	{
		cjMemDeAlloc(m_extDatAry);
		m_extDatAry = 0;
	}
*/	
//	_DeallocExtDatAry();

	if(m_extDatAry)
	{
		for(cjWORD i=0; i<m_extDatCount; i++)
		{
			cjExtraDat* cur = m_extDatAry[i];

			if(cur)
				cur->ReleaseRef();
		}

		cjMemDeAlloc(m_extDatAry);
		m_extDatAry = 0;
	}

	m_extDatCount = m_extDatMaxCount = 0;
}

cjBool cjRefObj::SetExtDatCount(cjWORD extDatCount)
{
    if(extDatCount == 0)
        return cjFalse;

    if(m_extDatMaxCount == 0)
    {
        m_extDatMaxCount = extDatCount;
		m_extDatAry = (cjExtraDat**)cjMemAlloc(sizeof(cjExtraDat*) * m_extDatMaxCount);
		m_extDatCount = 0;
    }
    else
    {
		m_extDatMaxCount = extDatCount;
		cjExtraDat** newExtDatAry = (cjExtraDat**)cjMemAlloc(sizeof(cjExtraDat*) * m_extDatMaxCount);

		cjUINT32 copySiz = m_extDatMaxCount * sizeof(cjExtraDat*);
		cjMemZero(newExtDatAry, copySiz);

		cjMemCpy(newExtDatAry, m_extDatAry, m_extDatCount * sizeof(cjExtraDat*));
		
		// just remove ary
		cjMemDeAlloc(m_extDatAry);
		m_extDatAry = newExtDatAry;
    }

    return cjTrue;
}

void cjRefObj::DeleteExtDat(cjWORD idx)
{
    if(idx >= m_extDatCount)
        return;

    if(m_extDatAry[idx])
        m_extDatAry[idx]->ReleaseRef();

	// copy next items from given idx
    for(cjWORD i=idx; i<(m_extDatCount-1); i++)
		m_extDatAry[i] = m_extDatAry[i+1];

    m_extDatCount--;
    m_extDatAry[m_extDatCount] = NULL;
}

cjBool cjRefObj::InsertExtDat(cjExtraDat* dat)
{
    if(!dat)
        return cjFalse;

    dat->AddRef(); // ref+1

    if(m_extDatMaxCount < 1)
    {
        m_extDatCount = m_extDatMaxCount = 1;
		m_extDatAry = (cjExtraDat**)cjMemAlloc(sizeof(cjExtraDat) * m_extDatMaxCount);
		m_extDatAry[0] = dat;

		return cjTrue;
    }
    else if(m_extDatCount == m_extDatMaxCount) // grow array
    {
        m_extDatMaxCount = (m_extDatMaxCount * 2) + 1;
        cjExtraDat** newExtDatAry = (cjExtraDat**)cjMemAlloc(sizeof(cjExtraDat) * m_extDatMaxCount);

        cjUINT32 newExtDatArySiz = m_extDatMaxCount * sizeof(cjExtraDat*);
		cjMemZero(newExtDatAry, newExtDatArySiz);

		// copy from previous array
        cjMemCpy(newExtDatAry, m_extDatAry, m_extDatCount * sizeof(cjExtraDat*));

		// just delete ary
		cjMemDeAlloc(m_extDatAry);

		m_extDatAry = newExtDatAry;
    }

	cjWORD i;
    m_extDatAry[m_extDatCount] = dat;
    m_extDatCount++;

    for(i=m_extDatCount; i<m_extDatMaxCount; i++)
        m_extDatAry[i] = 0;
  
	// sorting for binary search (back to front)
    for(i=m_extDatCount-1; i>0; i--)
    {
        cjINT32 r = cjStrcmp(*m_extDatAry[i-1]->GetName(), *m_extDatAry[i]->GetName());

        if(r == 0)
        {	
			// found same named key, delete
            DeleteExtDat(i);
            return cjFalse;
        }
        else if(r > 0)
        {	
			// swap previous key is greater.
            cjExtraDat* tmp		= m_extDatAry[i-1];
            m_extDatAry[i-1]	= m_extDatAry[i];
            m_extDatAry[i]		= tmp;
        }
        else
			break; // finish sorting
    }

    return cjTrue;
}

#ifdef CJUNICODE
void cjRefObj::SetName(const cjChar* name)
{
	if(!name)
		m_name = cjName::NullName;

	cjChar2Uni uni(name);
	m_name.SetName(*uni);		
}
#endif


//
// animate controller
//

void cjRefObj::ReplaceAniCntrList(cjAniCntr* cntr)
{
	m_spAniCntr = cntr;
}

void cjRefObj::AddAniCntr(cjAniCntr* cntr)
{
	if(cntr)
	{
		cntr->SetNextCntr(m_spAniCntr);
		m_spAniCntr = cntr;
	}
}

void cjRefObj::DelAniCntr(cjAniCntr* cntr)
{
	if(!cntr || !m_spAniCntr)
		return;

	if(m_spAniCntr == cntr)
	{
		cjAniCntrRPtr preserve = cntr; // ref+1
		m_spAniCntr = cntr->GetNextCntr();
		cntr->SetNextCntr(0);

		// cjdefer - need to warning
		// cntr ref count must be greater than 1 or cntr list is destroyed. except on exporting scene at 3dsmax		
		// cjAssert(cntr->GetRef() > 1); 
	}
	else
	{
		// find cntr in list
		cjAniCntr* prev = m_spAniCntr;
		cjAniCntr* cur = prev->GetNextCntr();

		while(cur && cur != cntr)
		{
			prev = cur;
			cur = cur->GetNextCntr();
		}

		if(cur)
		{
			cjAniCntrRPtr preserve = cntr;
			prev->SetNextCntr(cntr->GetNextCntr());
			cntr->SetNextCntr(0);

			// cntr ref count must be greater than 1 or cntr list is destroyed.
			cjAssert(cntr->GetRef() > 1);
		}
	}
}

void cjRefObj::DelAllAniCntrs()
{
    m_spAniCntr = 0;
}

cjAniCntr* cjRefObj::FindAniCntr(const cjRTTI* rtti) const
{
	cjAniCntr* curCntr = m_spAniCntr;
	while(curCntr)
	{
		if(curCntr->cjsIsKindOf(rtti))
			return curCntr;

		curCntr = curCntr->GetNextCntr();
	}

	return cjNULL;
}

cjBool cjRefObj::HasTMAniCntr() const
{
	cjAniCntr* curCntr = m_spAniCntr;
	while(curCntr)
	{
		if(curCntr->IsTransformController())
			return cjTrue;

		curCntr = curCntr->GetNextCntr();
	}

	return cjFalse;
}
