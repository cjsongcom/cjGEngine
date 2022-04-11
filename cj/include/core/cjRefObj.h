
#ifndef _CJREFOBJ_H_
#define _CJREFOBJ_H_

class cjExtraDat;
class cjAniCntr;
class cjSceneNode;

cjRefPtrForwardDefine(cjAniCntr);

//	cjTRefPtr<CLS_NAME>


class CJCORE_API cjRefObj : public cjRef
{
	CJSER_DECL_CLASS(cjRefObj, cjRef, CJRTTI_CF_ABSTRACT, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_SERIALIZE_POST
	CJSER_DECL_CLONE(cjRefObj)
	CJSER_DECL_CLONE_POST

public:
	virtual cjRefObj* GetInstAsRefObj() { return this; }

public:
	cjRefObj();
	cjRefObj(const cjName& name);

	// manage extra data
    cjWORD GetExtDatCount() const { return m_extDatCount; }
    cjBool AddExtDat(const cjName& key, cjExtraDat* dat);
    cjBool AddExtDat(cjExtraDat* dat);
    cjExtraDat* GetExtDat(const cjName& key) const;
    cjExtraDat* GetExtDatByIndex(cjWORD idx) const
	{
		cjAssert(idx < m_extDatCount);
		return m_extDatAry[idx];
	}

	// assume all extra data is sorted by their keyname
    cjBool RemoveExtDat(const cjName& key);
    cjBool RemoveExtDatByIndex(cjWORD idx);
    void RemoveAllExtDat();
    cjBool SetExtDatCount(cjWORD extDatCount); 

	const cjName& GetName() const { return m_name; }
	void SetName(const cjName& name) { m_name = name; }
#ifdef CJUNICODE
	void SetName(const cjChar* name);
#endif

	// animate controller
	cjAniCntr* GetFirstAniCntr() { return m_spAniCntr; }
	void ReplaceAniCntrList(cjAniCntr* cntr);    

	void AddAniCntr(cjAniCntr* cntr);
	void DelAniCntr(cjAniCntr* cntr);
	void DelAllAniCntrs();
	
	cjAniCntr* FindAniCntr(const cjRTTI* rtti) const;
	cjBool HasTMAniCntr() const;

	virtual cjSceneNode* GetSceneNode() { return cjNULL; }

protected:
	//virtual void OnClone() {}
	// perform sorting by key name, every inserting
    cjBool InsertExtDat(cjExtraDat* dat); 
    void DeleteExtDat(cjWORD idx);

private:

	enum REFOBJ_FLMASK
	{
		MASK_CULLED				= 0x00000001,
		BPOS_CULLED				= 0,
	};

    void _SetFlag(REFOBJ_FLMASK mask, cjWORD bpos, cjWORD valMask, cjUINT32 val) { _flag = (_flag & ~mask) | ((cjUINT32)((val & valMask) << bpos)); }
	void _SetFlag(REFOBJ_FLMASK mask, cjWORD bpos, cjBool val) { cjUINT32 _val = val ? 1 : 0; _flag = (_flag & ~mask) | (_val << bpos); }

	cjUINT32 _GetFlag(REFOBJ_FLMASK mask, cjWORD bpos) const { return (_flag & mask) >> bpos; }
	cjBool _GetFlagAsBool(REFOBJ_FLMASK mask, cjWORD bpos) const { return (_flag & mask) >> bpos ? cjTrue : cjFalse; }

	cjTFLAG							_flag;

public:
	cjBool							IsCulled() { return _GetFlagAsBool(MASK_CULLED, BPOS_CULLED); }
	void							SetCulled(cjBool culled) { _SetFlag(MASK_CULLED, BPOS_CULLED, culled); }

protected:
    cjExtraDat**					m_extDatAry;

	cjWORD							m_extDatCount;		// 2byte
    cjWORD							m_extDatMaxCount;	// 2byte - 4byte align
	
	cjAniCntrRPtr					m_spAniCntr;		// head of aniCntr list

private:
	// disable default copy constructor and assignment operator
	// use Clone() instead
	cjRefObj(const cjRefObj& ) {}
	cjRefObj& operator=(const cjRefObj& ) { return *this; }

	cjName							m_name;
};

cjRefPtrDefine(cjRefObj)

/*	template<>
cjRefObjPtr& cjTCloner<cjRefObjPtr>(const cjRefObjPtr& in, cjRefObjPtr& clone, cjTFLAG flag)
{
	return clone;
}*/

/* template<>
cjRefObj& cjTCloner<cjRefObj>(const cjRefObj& in, cjRefObj& clone, cjTFLAG flag)
{
	return clone;
}*/

//CJCORE_API void* cjName2RefObjHashCloner(void* src);
//typedef cjTHash<const cjNameChar*, cjRefObj*, cjName2RefObjHashCloner, cjTHashSerRemover<cjRefObj> > cjName2RefObjHash;

typedef cjTSerHash<const cjNameChar*, cjRefObj*, 
	cjTPairCloner<const cjNameChar*, cjRefObj*, cjTCloneByCopy, cjTCloneByCopy>,
	cjTHashDefaultNoneRemove<cjRefObj*>, 
	cjTHashCompare<const cjNameChar*> > cjName2RefObjHash;


#endif

