
#include "cjCorePCH.h"
#include "cjName.h"


//
// cjNameEntry
//

cjNameEntry::cjNameEntry()
{
	m_index = cjIndex_None;
	m_flags = 0;
	m_next = NULL;
	m_name[0] = NULL;
	m_preHash = 0;
}


cjNameEntry* cjAllocateNameEntry(const cjNameChar* name, cjUINT32 nameIndex, 
	cjTFLAG flag, cjNameEntry* nextHash)
{
	// cjNameEntry is not derived cjMem

	// calculate actual string siz,
	cjINT32 len = cjStrlen(name);
	cjAssert(len < (cjNameSiz-1));

	cjUINT32 actualSiz = sizeof(cjNameEntry) - (sizeof(cjUINT32) + (cjNameSiz - len -1) * sizeof(cjNameChar));
	cjNameEntry* nameEntry = (cjNameEntry*)cjMemAllocTag(actualSiz, "cjNameEntry");

	nameEntry->m_index = nameIndex;
	nameEntry->m_next = nextHash;
	nameEntry->m_flags = flag;

	cjStrcpy(nameEntry->m_name, len+1, name); // +1 for null

	nameEntry->m_preHash = cjName::GenHash(nameEntry->m_name);

	return nameEntry;	
}

cjFStream& operator << (cjFStream& stream, cjNameEntry& nameEntry)
{
	cjString str(nameEntry.m_name);
	stream << str;

	cjStrcpy(nameEntry.m_name, *str.Left(cjNameSiz-1));
	stream << nameEntry.m_flags;

	//cjBYTE strSiz = 0;
	//stream << strSiz;		// get string siz
	//cjAssert(strSiz > 0 && strSiz < cjNameSiz);
	//cjFStreamNByteSerializer nameRead(nameEntry.m_name, sizeof(cjNameChar) * strSiz);
	//stream << nameRead;

	return stream;
}


//
// cjName
//

cjNameEntryAry					cjName::ms_names;
cjNameIndexAvailable			cjName::ms_indexAvailable;

cjNameEntry*					cjName::ms_hash[cjName::NUM_HASH] = {0};
cjBool							cjName::ms_Init = 0;
const cjName					cjName::NullName = cjName(cjName_None);

cjName::cjName() //Do not use : m_nameIndex(cjName_None), it is initialized in __cjObjAlloc
{
// to avoid crash from unordered global variable initialization
//    cjNameEntry* entry = ms_names(m_nameIndex);
//    m_dbgName = entry ? entry->m_name : NULL;
	m_dbgName = 0;
}

cjName::cjName(cjNamePredefined nameIndex) : m_nameIndex(nameIndex)
{
	// to avoid crash from unordered global variable initialization
	if(ms_Init)
	{
		cjNameEntry* entry = ms_names(nameIndex);
		m_dbgName = entry ? entry->m_name : NULL;
	}
	else
		m_dbgName = 0;
}

cjName::cjName(cjNameIndex nameIndex) : m_nameIndex(nameIndex) 
{
// to avoid crash from unordered global variable initialization
	if(ms_Init)
	{
		cjNameEntry* entry = ms_names(nameIndex);
		m_dbgName = entry ? entry->m_name : NULL;
	}
	else
		m_dbgName = 0;
}

cjName::cjName(const cjNameChar* name, cjNameFind find)
{
	m_nameIndex = FindAdd(name, find);

// to avoid crash from unordered global variable initialization
	if(ms_Init)
	{
		cjNameEntry* entry = ms_names(m_nameIndex);
		m_dbgName = entry ? entry->m_name : NULL;
	}
	else
		m_dbgName = 0;
}


//
// static init/exit functions
//

void cjName::StaticInit()
{
	ms_Init = 1;

	ms_names.RemoveAll();
	ms_names.PushZero(cjNamePredefineCountMax*2);
	
	// init predefined names
	cjName::_RegPredefine(cjName_None					, _CU("(null)"			));

	// do not modify order
	cjName::_RegPredefine(cjName_cjByteProperty			, _CU("cjByteProperty"	));
	cjName::_RegPredefine(cjName_cjIntProperty			, _CU("cjIntProperty"	));
	cjName::_RegPredefine(cjName_cjBoolProperty			, _CU("cjBoolProperty"	));
	cjName::_RegPredefine(cjName_cjFloatProperty		, _CU("cjFloatProperty"	));
	cjName::_RegPredefine(cjName_cjObjProperty			, _CU("cjObjProperty"	));
	cjName::_RegPredefine(cjName_cjNameProperty			, _CU("cjNameProperty"	));
	cjName::_RegPredefine(cjName_cjStrProperty			, _CU("cjStrProperty"	));
	cjName::_RegPredefine(cjName_cjObjClsProperty		, _CU("cjObjClsProperty"));
	cjName::_RegPredefine(cjName_cjAryProperty			, _CU("cjAryProperty"	));
	cjName::_RegPredefine(cjName_cjHashProperty			, _CU("cjHashProperty"	));
	cjName::_RegPredefine(cjName_cjVecProperty			, _CU("cjVecProperty"	));
	cjName::_RegPredefine(cjName_cjRotProperty			, _CU("cjRotProperty"	));
	cjName::_RegPredefine(cjName_cjSerObjRefProperty	, _CU("cjSerObjRefProperty"));
	cjName::_RegPredefine(cjName_ReservedProperty1		, _CU("cjReservedProperty1"));
	cjName::_RegPredefine(cjName_ReservedProperty2		, _CU("cjReservedProperty2"));
	
	cjName::_RegPredefine(cjName_Transient				, _CU("(transient)"		));

	cjName::_RegPredefine(cjName_cjCore					, _CU("cjCore"			));
	cjName::_RegPredefine(cjName_cjEngine				, _CU("cjEngine"		));
	cjName::_RegPredefine(cjName_cjGame					, _CU("cjGame"			));
	cjName::_RegPredefine(cjName_cjObj					, _CU("cjObj"			));
	cjName::_RegPredefine(cjName_cjObjCls				, _CU("cjObjCls"		));
	cjName::_RegPredefine(cjName_cjObjPackage			, _CU("cjObjPackage"	));

	cjName::_RegPredefine(cjName_cjViewport				, _CU("cjViewport"	));
	cjName::_RegPredefine(cjName_cjClient				, _CU("cjClient"	));
	cjName::_RegPredefine(cjName_cjRenDev				, _CU("cjRenDev"	));
	cjName::_RegPredefine(cjName_cjGeometry				, _CU("cjGeometry"	));
	cjName::_RegPredefine(cjName_cjMesh					, _CU("cjMesh"	));

	cjName::_RegPredefine(cjName_cjAppGame				, _CU("cjAppGame"	));
	cjName::_RegPredefine(cjName_cjAppEngine			, _CU("cjName_cjAppEngine"	));	


	cjName::_RegPredefine(cjName_cjAniTMCntrID			, _CU("cjTransformAniCntr"));
	cjName::_RegPredefine(cjName_cjAniBoneRoot			, _CU("@BONEROOT"));
	cjName::_RegPredefine(cjName_cjAniNonAccumSuffix	, _CU("_NonAccum"));
	cjName::_RegPredefine(cjName_cjAniStTxtKey			, _CU("start"));
	cjName::_RegPredefine(cjName_cjAniEnTxtKey			, _CU("end"));
	cjName::_RegPredefine(cjName_cjAniMorphTxtKey		, _CU("morph:"));

	cjName::_RegPredefine(cjName_cjFlipAniCntr			, _CU("cjFlipAniCntr"));

	cjName::_RegPredefine(cjName_cjShaderMtlDefault		, _CU("cjShaderMtlDefault"));
	
}

void cjName::StaticShutdown()
{
	assert(ms_Init);

	for(cjAryOff i=0; i< ms_names.Num(); i++)
	{
		cjNameEntry* entry = ms_names(i);
		if(entry)
			cjMemDeAlloc(entry);
	}

	ms_names.RemoveAll();
	ms_Init = 0;
}

cjName& cjName::operator =(const cjNameChar* name)
{
	m_nameIndex = FindAdd(name, cjNameFind_Add);
	return *this;
}

cjNameIndex cjName::FindAdd(const cjNameChar* name, cjNameFind findType)
{
	if(!name || !ms_Init)
		return cjName_None;

	// case sensitive
	cjINT32 hash = cjStrHash(name) & (cjName::NUM_HASH-1);

	for(cjNameEntry* curNameEntry=ms_hash[hash]; curNameEntry; curNameEntry = curNameEntry->m_next)
	{
		// case sensitive
		if(!cjStrcmp(name, curNameEntry->m_name))
			return curNameEntry->m_index;
	}

	// if finding only
	if(findType == cjNameFind_Find)
		return cjName_None;

	cjNameIndex nameIndex;

	if(ms_indexAvailable.Num())
	{
		nameIndex = ms_indexAvailable(ms_indexAvailable.Num()-1);
		ms_indexAvailable.Remove(ms_indexAvailable.Num()-1);
	}
	else
	{
		nameIndex = ms_names.PushEmpty();
	}

	cjNameEntry* nameEntry = cjAllocateNameEntry(name, nameIndex, 0, ms_hash[hash]);

	cjAssert(nameEntry);
	ms_names(nameIndex) = ms_hash[hash] = nameEntry;

	return nameIndex;
}

void cjName::SetName(cjNamePredefined predefinedName)
{
	m_nameIndex = predefinedName;

	if(ms_Init)
	{
		cjNameEntry* entry = ms_names(m_nameIndex);
		m_dbgName = entry ? entry->m_name : NULL;
	}
	else
		m_dbgName = 0;

}

void cjName::SetName(const cjNameChar* name)
{
	m_nameIndex = FindAdd(name, cjNameFind_Add);

	if(ms_Init)
	{
		cjNameEntry* entry = ms_names(m_nameIndex);
		m_dbgName = entry ? entry->m_name : NULL;
	}
	else
		m_dbgName = 0;

}

cjBool cjName::Cmp(const cjNameChar* name) const
{
	if( (!name || *name==0))
		return m_nameIndex == cjName_None ? cjTrue : cjFalse;

	return cjStrcmp(ms_names(m_nameIndex)->m_name, name) ? cjFalse : cjTrue;
}

cjBool cjName::Equal(const cjNameChar* name) const
{
	if( (!name || *name==0))
		return m_nameIndex == cjName_None ? cjTrue : cjFalse;

	return cjStrcmp(ms_names(m_nameIndex)->m_name, name) ? cjFalse : cjTrue;
}

cjBool cjName::Equal(cjNamePredefined predefinedName) const
{
	return m_nameIndex == predefinedName ? cjTrue : cjFalse;
}

cjBool cjName::ICmp(const cjNameChar* name) const
{
	if( (!name || *name==0))
		return m_nameIndex == cjName_None ? cjTrue : cjFalse;

	return cjStricmp(ms_names(m_nameIndex)->m_name, name) ? cjFalse : cjTrue;
}

cjBool cjName::IEqual(const cjNameChar* name) const
{
	if( (!name || *name==0))
		return m_nameIndex == cjName_None ? cjTrue : cjFalse;

	return cjStricmp(ms_names(m_nameIndex)->m_name, name) ? cjFalse : cjTrue;
}

cjINT32 cjName::GetLen() const
{
	if(m_nameIndex == cjName_None)
		return 0;

	//cjopti need to precalculate size
	return cjStrlen(ms_names(m_nameIndex)->m_name);
}

cjUINT32 cjName::GetHash() const
{
	return ms_names(m_nameIndex)->m_preHash;
	//return cjStrHash(ms_names(m_nameIndex)->m_name);
}

cjUINT32 cjName::GenHash(const cjNameChar* name)
{
	return cjStrHash(name) & (cjName::NUM_HASH-1);
}

cjUINT32 cjName::GenHash(const cjName& a, const cjName& b, const cjName& c)
{
	return a.GetIndex() + 7 * b.GetIndex() + 31 * c.GetIndex();
}

void cjName::_RegPredefine(cjNamePredefined preDefine, const cjNameChar* name)
{
	if(!name || !ms_Init)
		return;

	// case sensitive
	cjINT32 hash = cjStrHash(name) & (cjName::NUM_HASH-1);

//	cjNameEntry* curNameEntry = ms_hash[hash];
	cjNameIndex nameIndex = (cjNameIndex)preDefine;
	cjNameEntry* nameEntry = cjAllocateNameEntry(name, nameIndex, 0, ms_hash[hash]);
	cjAssert(nameEntry);

	ms_names(nameIndex) = ms_hash[hash] = nameEntry;
}

cjFStream& cjName::operator>>(cjFStream& stream)
{
	if(stream.IsLoading())
	{
		cjNameChar* buf = 0;
		cjSerializeNameCharStr(stream, (const cjNameChar**)&buf, 0, &cjMemAllocDefault);

		if(buf)
		{
			cjName loadName(buf, cjNameFind_Add);
			cjMemDeAlloc(buf);

			*this = loadName;
		}
	}
	else if(stream.IsSaving())
	{	
		const cjNameChar* buf = 0;

		if(m_nameIndex == cjIndex_None)
		{
			;
		}
		else
		{
			cjNameEntry* entry = cjName::GetEntry(GetIndex());
			buf = entry->m_name;
		}

		cjSerializeNameCharStr(stream, (const cjNameChar**)&buf, 0);
	}

	return stream;
}

CJCORE_API cjUINT32 cjName_GenHash(const cjNameChar* name)
{
    return cjName::GenHash(name);
}

