
#include "cjCorePCH.h"
#include "cjRTTI.h"


//
// cjRTTI
//

cjBool cjRTTI::CmpName(const cjChar* name) 
{ 
	return cjStrcmp(m_cjrNameA, name) == 0 ? cjTrue : cjFalse; 
}

cjBool cjRTTI::CmpName(const cjChar* name) const
{ 
	return cjStrcmp(m_cjrNameA, name) == 0 ? cjTrue : cjFalse; 
}

cjBool cjRTTI::CmpName(const cjUni* name) 
{ 
	return cjStrcmp(m_cjrNameW, name) == 0 ? cjTrue : cjFalse; 
}

cjBool cjRTTI::CmpName(const cjUni* name) const
{ 
	return cjStrcmp(m_cjrNameW, name) == 0 ? cjTrue : cjFalse; 
}

void* cjRTTI::NewInst(cjSiz_t siz) 
{
	if(siz !=0)
	{
		cjAssert(m_siz == siz);
	}

	void* m = Alloc();
	return m_pfPNew(m);
}

void cjRTTI::DelInst(void* m)
{
	cjSerializable* serObj = (cjSerializable*)m;

	if(serObj->cjSerGetFlag() == CJSER_FL_REF)
	{
		if(!(serObj->cjSerGetFlag() == CJSER_FL_REF_DELETEABLE))
		{
			cjAssert(!"ERR! - Called from non cjRef derived instance.. use ReleaseRef() !!..");
			return;
		}
	}

	m_pfPDel(m);
	DeAlloc(m);
}

//
// RTTI Hash
//

class cjRTTIHash
{
public:
	enum { EBUCKET = 1024 };

	inline cjUINT32 doHash(const cjChar* str) const
	{
		cjUINT32 hash = 0; if(!str) return 0;
		cjChar c;

		while( (c=*str++) != (cjChar)NULL )
			{ hash = 65599 * hash + c; }

		return (hash ^ (hash >> 16)) % (EBUCKET-1);
	}
	
	const cjRTTI* FindA(const cjChar* name, cjRTTI* insRtti=0, cjBool forCreating=false);

private:
	static cjRTTI*								m_bucket[EBUCKET];
};

cjRTTI* cjRTTIHash::m_bucket[cjRTTIHash::EBUCKET];


const cjRTTI* cjRTTIHash::FindA(const cjChar* name, cjRTTI* insRtti, cjBool forCreating)
{
	if(!name)
		return 0;

	cjUINT32 hash = doHash(name);

	cjRTTI* cur = m_bucket[hash];
	cjRTTI* prev = 0;

	while(cur)
	{
		if(!strcmp(cur->cjrGetNameA(), name))
		{
			if(forCreating)
			{
				cjAssert(!"[cjRTTIHash] ERR! - Dupplicated rtti name");
			}

			return cur;
		}

		prev = cur;
		cur = cur->m_nextHash;
	}

	if(!forCreating)
		return 0;

	if(prev)
		prev->m_nextHash = insRtti;
	else
		m_bucket[hash] = insRtti;

	return insRtti;
}


cjRTTIHash gs_cjRTTIHash;


//
// cjRTTI
//

cjRTTI::cjRTTI(cjSiz_t siz, const cjChar* nameA, const cjUni* nameW, 
	const cjRTTI* base, cjTFLAG flag,
	PFNew pfNew, PFDel pfDel, PFPNew pfPNew, PFPDel pfPDel)
{
	m_siz = siz;

	m_flag = flag;
	m_nextHash = 0;

	m_cjrBase  = const_cast<cjRTTI*>(base);
	m_cjrNameA = const_cast<cjChar*>(nameA);
	m_cjrNameW = const_cast<cjUni*>(nameW);

	m_pfNew = pfNew;
	m_pfDel = pfDel;
	m_pfPNew = pfPNew;
	m_pfPDel = pfPDel;

	// adding cjRTTI
	gs_cjRTTIHash.FindA(m_cjrNameA, this, cjTrue);
}

const cjRTTI* cjRTTI::FindByNameA(const cjChar* name)
{
	return gs_cjRTTIHash.FindA(name);
}

void* cjRTTI::Alloc()
{
	return cjMem::_MAlloc(m_siz);
}

void cjRTTI::DeAlloc(void* m)
{
	cjMem::_MDeAlloc(m);
}

