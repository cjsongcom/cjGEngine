
#ifndef _CJNAME_H_
#define _CJNAME_H_

#include "cjMem.h"
#include "cjTypeInfo.h"

enum cjNameFind
{
	cjNameFind_Find,	// just find, not adding
	cjNameFind_Add		// adding if not exist
};

enum 
{ 
	cjNameSiz = 160,	
	// cjFMaterialDesc = 128 byte

	cjNamePredefineCountMax				= 1000,
};

// do not use bellow order as absolute index
enum cjNamePredefined
{
	cjName_None							= 0,

	// don't modify order
    cjName_cjByteProperty				=1,
    cjName_cjIntProperty				=2,
    cjName_cjBoolProperty				=3,
    cjName_cjFloatProperty				=4,
    cjName_cjObjProperty				=5,
    cjName_cjNameProperty				=6,
	cjName_cjStrProperty				=7,
    cjName_cjObjClsProperty				=8,
    cjName_cjAryProperty				=9,
	cjName_cjHashProperty				=10,    
	cjName_cjVecProperty				=11,
	cjName_cjRotProperty				=12,
	cjName_cjSerObjRefProperty			=13,
	cjName_ReservedProperty1			=14,
	cjName_ReservedProperty2			=15,

	cjName_Transient					,

	cjName_cjCore						,
	cjName_cjEngine						,
	cjName_cjGame						,
	cjName_cjObj						,
	cjName_cjObjCls						,
	cjName_cjObjPackage					,

	cjName_cjViewport					,
	cjName_cjClient						,
	cjName_cjRenDev						,
	cjName_cjGeometry					,
	cjName_cjMesh						,

	cjName_cjAppGame					,
	cjName_cjAppEngine					,


	cjName_cjAniTMCntrID				,
	cjName_cjAniBoneRoot				,
	cjName_cjAniNonAccumSuffix			,
	cjName_cjAniStTxtKey				,
	cjName_cjAniEnTxtKey				,
	cjName_cjAniMorphTxtKey				,

	cjName_cjFlipAniCntr				,

	cjName_cjShaderMtlDefault			,
};


//
// cjNameEntry
//

class CJCORE_API cjNameEntry  // do not specify ': public cjMem'
{
public:
	cjNameEntry();

	cjNameIndex							m_index;
	
	cjTFLAG								m_flags; // cjOF_TagImport/cjOF_TagExport/cjOF_Unreachable

	cjNameEntry*						m_next;
	cjUINT32							m_preHash;

	cjNameChar							m_name[cjNameSiz];
	
	CJCORE_API friend cjFStream& operator<< (cjFStream& stream, cjNameEntry& nameEntry);
	CJCORE_API friend cjNameEntry* cjAllocNameEntry(const cjNameChar* name, 
		cjNameIndex index, cjNameEntry* next);
};

typedef cjTAry<cjNameEntry*> cjNameEntryAry;
typedef cjTAry<cjINT32> cjNameIndexAvailable;


//
// cjName
//

// do not use rtti
// do not use virtual function
class CJCORE_API cjName : public cjMem
{
	CJTYPEINFO_SUPPORT_DECL(cjName)

public:
	const static cjUINT32 NUM_HASH = 4096;

	cjName();
	cjName(cjNamePredefined nameIndex);
	cjName(cjNameIndex nameIndex);
	cjName(const cjNameChar* name, cjNameFind find=cjNameFind_Add);

	cjFStream& operator>>(cjFStream& stream);

	const cjNameChar* operator*() const
	{
        cjNameChar* dst = ms_names(m_nameIndex)->m_name;
        return dst;
	}

	operator const cjNameChar* () const
	{
		cjNameChar* dst = ms_names(m_nameIndex)->m_name;
		return dst;		
	}

	inline cjNameIndex GetIndex() const
	{
		return m_nameIndex;
	}

	static cjINT32 GetCount()
	{
		return ms_names.Count();
	}

	static cjNameEntry* GetEntry(cjINT32 i)
	{ 
		return ms_names(i); 
	}

	cjTFLAG GetFlags() const
	{
		return ms_names(m_nameIndex)->m_flags;
	}

	void SetFlags(cjTFLAG flags) const
	{
		ms_names(m_nameIndex)->m_flags |= flags;
	}

	void ClearFlags(cjTFLAG flags) const
	{
		ms_names(m_nameIndex)->m_flags &= ~flags;
	}

	cjBool operator==(cjNamePredefined predefinedName) const
	{
		return m_nameIndex == predefinedName ? cjTrue : cjFalse;
	}

	cjBool operator!=(cjNamePredefined predefinedName) const
	{
		return m_nameIndex != predefinedName ? cjTrue : cjFalse;
	}

	cjBool operator==(const cjName& rhs) const
	{
		return m_nameIndex == rhs.m_nameIndex ? cjTrue : cjFalse;
	}

	cjBool operator!=( const cjName& rhs ) const
	{
		return m_nameIndex != rhs.m_nameIndex ? cjTrue : cjFalse;
	}

	cjBool IsValid() const
	{
		return m_nameIndex>0 && m_nameIndex < (cjNameIndex)ms_names.Num() && 
			ms_names(m_nameIndex) != NULL ? cjTrue : cjFalse;
	}

	void SetNone() { m_nameIndex = cjName_None;	}
	void SetName(cjNamePredefined predefinedName);
	void SetName(const cjNameChar* name);

	cjBool Cmp(const cjNameChar* name) const;

	cjBool Equal(const cjNameChar* name) const;
	cjBool Equal(cjNamePredefined predefinedName) const;

	cjBool ICmp(const cjNameChar* name) const;
	cjBool IEqual(const cjNameChar* name) const;

	cjName& operator=(const cjNameChar* name);

	cjSlow cjINT32 GetLen() const; //cjopti
	cjUINT32 GetHash() const;

	static void StaticInit();
	static void StaticShutdown();

	static const cjName					NullName;
		
	static cjUINT32 GenHash(const cjNameChar* name);
	static cjUINT32 GenHash(const cjName& a, const cjName& b, const cjName& c);

private:
	cjNameIndex FindAdd(const cjNameChar* name, cjNameFind findType);

	cjNameIndex							m_nameIndex;
	const cjNameChar*					m_dbgName;

	static cjNameEntryAry				ms_names;
	static cjNameIndexAvailable			ms_indexAvailable;

	static cjNameEntry*					ms_hash[NUM_HASH];
	static cjBool						ms_Init;

private:
	static void _RegPredefine(cjNamePredefined preDefine, const cjNameChar* name);

};


struct cjTHashCompareName
{
	cjBool operator()(const cjName& lhs, const cjName& rhs) const
	{
		return lhs == rhs;
	}
};

struct cjTHasherName
{
	cjUINT32 operator()(const cjName& name) const
	{
		return name.GetIndex();
	}
};


template<>
struct cjTHasher<const cjName>
{
	cjUINT32 operator()(const cjName& key) const
	{		
		return key.GetHash();
	}
};

template<>
struct cjTHasher<cjName>
{
	cjUINT32 operator()(const cjName& key) const
	{		
		return key.GetHash();
	}
};


//template<class TKey, class TVal>
//void cjTNameHashCloner(const cjName&* srcKey, const void&* srcVal, void&* cloneKey, void&* cloneVal)
//{
//	cloneKey = *key;
//}

//
//template<>
//cjTRemoveConst<const cjName>& cjTCloner<const cjName>(const cjName& in, cjTRemoveConst<const cjName>& clone, cjTFLAG flag)
//{
//	clone = in;
//	return clone;
//}

//template<class TVal, cjUINT32 TBucketSiz=40 >
//class cjTNameHash : public cjTHash<const cjName, TVal, cjTHashDefaultNoneRemove<TVal>, cjTHashCompareName, cjTHasherName >
//{
//public:
//    cjTNameHash(cjUINT32 bucketSiz=TBucketSiz) 
//		: cjTHash<const cjName, TVal, cjTHashDefaultNoneRemove<TVal>, cjTHashCompareName, cjTHasherName >(bucketSiz) 
//	{
//	}
//};


CJTYPEINFO_SUPPORT_IMPL(cjName);





#endif
