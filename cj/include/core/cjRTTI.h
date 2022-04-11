
#ifndef _CJRTTI_H_
#define _CJRTTI_H_


class cjFStream;
class cjSerializable;


enum CJRTTI_CLS_FLAG
{
	CJRTTI_CF_NO_CLONE					= 0x00000001,
	CJRTTI_CF_ABSTRACT					= 0x00000002, 
	CJRTTI_CF_NO_INSTANCE				= 0x00000004,
	
	CJRTTI_CF_SCENE						= 0x00000010,

	CJRTTI_CF_MASK_SERIALIZE			= 0xffff0000,
};

// do not add virtual function
class CJCORE_API cjRTTI // do not specify cjMem
{
	friend class cjRTTIHash;

public:
	typedef void* (*PFNew)();
	typedef void  (*PFDel)(void* m);
	typedef void* (*PFPNew)(void* m);
	typedef void  (*PFPDel)(void* m);

	cjRTTI(cjSiz_t siz, const cjChar* name, const cjUni* nameW, 
		const cjRTTI* base, cjTFLAG flag, 
		PFNew pfNew, PFDel pfDel, PFPNew pfInNew, PFPDel pfInDel);

    inline const cjChar* cjrGetNameA() const { return m_cjrNameA; }
	inline const cjUni* cjrGetNameW() const { return m_cjrNameW; }
    inline const cjRTTI* cjrGetBase() const {return m_cjrBase; }
	
	cjBool CmpName(const cjChar* name);
	cjBool CmpName(const cjChar* name) const;

	cjBool CmpName(const cjUni* name);
	cjBool CmpName(const cjUni* name) const;

	inline void* NewInst(cjSiz_t siz=0) const 
	{ 
		if(siz != 0)  
		{
			cjAssert(m_siz == siz);
		}
			
		return m_pfNew();		
	}
	inline void DelInst(void* m) const	{ return m_pfDel(m);	}
	inline void* InNewInst(void* m)		{ return m_pfPNew(m);	}
	inline void InDelInst(void* m)		{ return m_pfPDel(m);	}

	void* NewInst(cjSiz_t siz=0);
	void DelInst(void* m);
			
	// only alloc/dealloc memory
	void* Alloc();
	void DeAlloc(void* m);
	
	static const cjRTTI* FindByNameA(const cjChar* name);
	
protected:
    cjSiz_t								m_siz;

	cjChar*								m_cjrNameA; 
	cjUni*								m_cjrNameW; //cjopti change to cjUni* -> cjName
    cjRTTI*								m_cjrBase;
	cjTFLAG								m_flag;

	PFNew								m_pfNew;
	PFDel								m_pfDel;
	PFPNew								m_pfPNew;
	PFPDel								m_pfPDel;

private:
	cjRTTI*								m_nextHash;
};

#define CJRTTI(cls)					&cls::_ms_cjrRTTI

#endif
