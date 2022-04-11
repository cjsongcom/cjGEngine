
#include "cjCorePCH.h"
#include "cjObjPkgLinker.h"


//
// cjFPkgFileHeader
//

cjFStream& operator << (cjFStream& stream, cjFPkgHeaderV0& headerV0)
{ 
	stream << headerV0.m_nameCount	 << headerV0.m_nameOffset;
	stream << headerV0.m_exportCount << headerV0.m_exportOffset;
	stream << headerV0.m_importCount << headerV0.m_importOffset;

	return stream;
}

cjFPkgFileHeader::cjFPkgFileHeader()
{
	cjMemZero(this, sizeof(*this));
}

cjFStream& operator <<(cjFStream& stream, cjFPkgFileHeader& fileHeader)
{
	if(stream.IsSaving())
	{
		const cjBYTE magic[] = CJPKGFILE_HEADER_MAGIC;
		cjMemCpy(fileHeader.m_magic, magic, sizeof(fileHeader.m_magic));
		fileHeader.m_rev = CJPKGFILE_REV(CJPKGFILE_REV_MAJOR, CJPKGFILE_REV_MINOR);
	}

	stream << *((cjUINT32*)&fileHeader.m_magic[0]);
	stream << *((cjUINT32*)&fileHeader.m_magic[4]);
	stream << fileHeader.m_rev;

	// on save, prewriting for deferred writing
	stream << fileHeader.m_nameCount;
	stream << fileHeader.m_nameOffset;
	stream << fileHeader.m_exportCount;
	stream << fileHeader.m_exportOffset;
	stream << fileHeader.m_importCount;
	stream << fileHeader.m_importOffset;

	if(stream.IsLoading())
	{
		if(fileHeader.m_rev == cjFPkgHeaderV0::REV)
		{
			stream << *((cjFPkgHeaderV0*)fileHeader.m_header);
		}
		else
			return stream;
	}
	else
	{
		
	}

	return stream;
}


//
// cjFObjImpDesc
//

cjFObjImpDesc::cjFObjImpDesc()
{
	m_srcPkgLoad = NULL;
}

cjFObjImpDesc::cjFObjImpDesc(cjObj* obj)
{
	m_classPkgName = obj->GetCls()->GetOuter()->GetName();
	m_className = obj->GetCls()->GetName();
	m_pkgIndex = 0;
	m_objName = obj->GetName();

	m_impObj = obj;
	m_srcPkgLoad = NULL;
	m_srcPkgLoadExpIndex = cjIndex_None;

	if(m_impObj)
		cjObj::cjObjPkgLinkerImpCount()++;
}

cjFStream& operator<<(cjFStream& stream, cjFObjImpDesc& impDesc)
{
	stream << impDesc.m_classPkgName;
	stream << impDesc.m_className;
	stream << impDesc.m_pkgIndex;
	stream << impDesc.m_objName;

	if(stream.IsLoading())
	{
		impDesc.m_srcPkgLoadExpIndex = cjIndex_None;
		impDesc.m_impObj = NULL;
	}

	return stream;
}


//
// cjFObjExpDesc
//

cjFObjExpDesc::cjFObjExpDesc()
{
	m_siz = 0;
	m_offset = 0;

	m_classIndex = 0;
	m_superIndex = 0;
	m_pkgIndex = 0;
	m_objName = cjName::NullName;
	m_objFlags = 0;
	
	m_expObj = NULL;
	m_hashNext = cjIndex_None;
}

cjFObjExpDesc::cjFObjExpDesc(cjObj* obj)
{
	m_siz = 0;
	m_offset = 0;

	m_classIndex = 0;
	m_superIndex = 0;
	m_pkgIndex = 0;
	
	m_objName = obj ? obj->GetName() : cjName::NullName;
	m_objFlags = obj ? obj->cjObjGetFlag() & cjOF_ObjLoadFlags : 0;

	m_expObj = obj;
	m_hashNext = cjIndex_None;
}

cjFStream& operator<<(cjFStream& stream, cjFObjExpDesc& expObjDesc)
{
	stream << cjFStreamCmprI32(expObjDesc.m_classIndex);
	stream << cjFStreamCmprI32(expObjDesc.m_superIndex);
	stream << expObjDesc.m_pkgIndex;
	stream << expObjDesc.m_objName;
	stream << expObjDesc.m_objFlags;
	stream << cjFStreamCmprI32(expObjDesc.m_siz);

	if(expObjDesc.m_siz)
		stream << cjFStreamCmprI64(expObjDesc.m_offset);

	return stream;
}


//
// cjObjPkgLinker
//

CJOBJ_IMPL_CLASS(cjObjPkgLinker)

CJOBJ_IMPL_SERIALIZE_BEGIN(cjObjPkgLinker)

	stream << m_nameAry << m_pkg;

	for(cjINT32 i=0; i<m_exportDescAry.Num(); i++)
	{
		cjFObjExpDesc& expObjDesc = m_exportDescAry(i);
		stream << expObjDesc.m_objName;
	}

	for(cjINT32 i=0; i<m_importDescAry.Num(); i++)
	{
		cjFObjImpDesc& impDesc = m_importDescAry(i);
		
		stream << *(cjObj**)&impDesc.m_srcPkgLoad;
		stream << impDesc.m_classPkgName << impDesc.m_className;
	}

CJOBJ_IMPL_SERIALIZE_END(cjObjPkgLinker)

cjObjPkgLinker::cjObjPkgLinker(cjObj* pkg, const cjNameChar* filename)
{
	cjAssert(pkg);
//	cjAssert(filename);
	
	m_pkg = pkg;
	m_valid = cjFalse;

	if(filename)
		m_filename = filename;

	m_loadflags = 0;

	m_header.m_pkgFlags = 0;
}

cjString cjObjPkgLinker::GetImpObjFullName(cjIndexType i)
{
	cjString tmp;

	for(cjIndexType k=-i-1; k!=0; k=m_importDescAry(-k-1).m_pkgIndex)
	{
		if(k != -i-1)
			tmp = cjString(_CU("")) + _CU(".") + tmp;

		tmp = cjString(*m_importDescAry(-k-1).m_objName) + tmp;
	}

	return cjString(*m_importDescAry(i).m_className) + _CU(" ") + tmp;
}

cjString cjObjPkgLinker::GetExpObjFullName(cjIndexType i, const cjNameChar* tmpPkgPathName)
{
	cjString tmp;

	for(cjIndexType k=i+1; k!=0; k=m_exportDescAry(k-1).m_pkgIndex)
	{
		if(k != i+1)
			tmp = cjString(_CU("")) + _CU(".") + tmp;

		tmp = cjString(*m_exportDescAry(k-1).m_objName) + tmp;
	}

	cjIndexType classIndex = m_exportDescAry(i).m_classIndex;
	cjName className;

	if(classIndex > 0)
		className = m_exportDescAry(classIndex-1).m_objName;
	else if(classIndex < 0)
		className = m_importDescAry(-classIndex-1).m_objName;
	else // classIndex == 0
		className = cjName(cjName_cjObjCls);

	cjNameChar pkgName[cjMAX_PATH];
		
	return cjString(*className) + _CU(" ") 
		+ (tmpPkgPathName ? tmpPkgPathName : m_pkg->cjObjGetPathName(pkgName, cjMAX_PATH)) 
		+ _CU(".") + tmp;

}


//
// cjObjPkgLoad
//

CJOBJ_IMPL_CLASS(cjObjPkgLoad)

CJOBJ_IMPL_SERIALIZE_BEGIN(cjObjPkgLoad)
CJOBJ_IMPL_SERIALIZE_END(cjObjPkgLoad)


cjBool cjObjPkgLoad::_GetModulePkgLoad(cjObjPkgLoad** rstPkgLoad, cjObj* pkg, 
	cjTFLAG loadFlags, cjBool skipFindLoad)
{
	if(!cjObjIsModulePkg(pkg))
		return cjFalse;

	if(!skipFindLoad && pkg)
	{
		for(cjINT32 i=0; i<cjObjGetPkgLoads().Num(); i++)
		{
			if(cjObjPkgLinkerGetByIndex(i)->m_pkg == pkg)
			{
				*rstPkgLoad = cjObjPkgLinkerGetByIndex(i);
				return cjTrue;
			}
		}
	}

	cjObjPkgLoad* pkgLoad = new cjObjPkgLoad(pkg, NULL, loadFlags);

	if(!pkgLoad->IsValid())
	{
		delete pkgLoad;
		return cjFalse;
	}

	*rstPkgLoad = pkgLoad;

	return cjTrue;	
}

cjObjPkgLoad::cjObjPkgLoad(cjObj* pkg, const cjNameChar* filename, cjTFLAG loadFlags)
	: cjObjPkgLinker(pkg, filename), m_loadFlags(loadFlags) 
{	
	if(cjObjIsModulePkg(pkg))
	{
#ifdef CJDEBUG
		{
			cjTAry<cjObjPkgLoad*>& pkgLoads = cjObjGetPkgLoads();
			for(cjAryOff i=0; i<pkgLoads.Count(); i++)
			{
				if(pkgLoads(i)->m_pkg == pkg)
				{
					m_valid = cjFalse;
					cjFatal(0);
					return;
				}
			}
		}
#endif

		m_loadStream = NULL;

		// stream setup
		SetSaving(cjFalse);
		SetLoading(cjTrue);

		for(int i=0; i<cjARRAYCNT(m_exportHash); i++)
			m_exportHash[i] = cjIndex_None;

		m_loadflags = loadFlags;
		m_validateImport = cjFalse;

		((cjObjPackage*)pkg)->_MakeModulePkgLoad(this);

		// add linker
		cjObjGetPkgLoads().Push(this);
	}
	else
	{
		m_loadStream = cjFileSystem::Get()->CreateFileReadStream(filename);

		if(!m_loadStream)
		{
			cjLOG(L"ERR! - error in opening pkg file [%s]", filename);
			cjFatal(0);

			return;
		}

		// check linker is already created
		for(cjINT32 i=0; i<cjObjPkgLoadGetCount(); i++)
		{
			if(cjObjPkgLinkerGetByIndex(i)->m_pkg == pkg)
			{
				cjLOG(L"ERR! - dupplicated loader is founded [%s]", *pkg->GetName());
				cjFatal(0);
			}
		}

		// set linker stream
		SetLoading(cjTrue);
		SetSaving(cjFalse);

		// load header
		*this << m_header;

		//cj checkcode
		{
			cjAssert(m_header.GetExportCount() != 0);
			cjAssert(m_header.GetImportCount() != 0);
			cjAssert(m_header.GetNameCount() != 0);
		}


		if(cjObjCast<cjObjPackage>(m_pkg))
			cjObjCast<cjObjPackage>(m_pkg)->m_pkgFlags = m_header.m_pkgFlags;

		// resize
		m_importDescAry.RemoveAll(m_header.GetImportCount());
		m_exportDescAry.RemoveAll(m_header.GetExportCount());
		m_nameAry.RemoveAll(m_header.GetNameCount());
		
		//
		// load & build name table
		//

		if(m_header.GetNameCount() > 0)
		{
			SetPos(m_header.GetNameOffset());

			for(cjINT32 i=0; i<m_header.GetNameCount(); i++)
			{
				cjNameEntry nameEntry;
				*this << nameEntry;

				if(nameEntry.m_flags) // & m_loadflags)
					m_nameAry.Push(cjName(nameEntry.m_name, cjNameFind_Add));
				else
					m_nameAry.Push(cjName::NullName);
			}
		}

		if(m_header.GetImportCount() > 0)
		{
			SetPos(m_header.GetImportOffset());
			for(cjINT32 i=0; i<m_header.GetImportCount(); i++)
			{
				cjFObjImpDesc* impDesc = new (m_importDescAry) cjFObjImpDesc;
				*this << *impDesc;
			}
		}

		if(m_header.GetExportCount() > 0)
		{
			SetPos(m_header.GetExportOffset());
			for(cjINT32 i=0; i<m_header.GetExportCount(); i++)
			{
				cjFObjExpDesc* expObjDesc = new (m_exportDescAry) cjFObjExpDesc;
				*this << *expObjDesc;
			}
		}

		for(cjINT32 i=0; i<cjARRAYCNT(m_exportHash); i++)
			m_exportHash[i] = cjIndex_None;

		for(cjINT32 i=0; i<m_exportDescAry.Num(); i++)
		{
			cjINT32 hash = cjName::GenHash(m_exportDescAry(i).m_objName, GetExpClsName(i), 
				GetExpClsPkg(i)) & (cjARRAYCNT(m_exportHash)-1);

			m_exportDescAry(i).m_hashNext = m_exportHash[hash];
			m_exportHash[hash] = i;
		}

		// add linker
		cjObjGetPkgLoads().Push(this);
	}

	if(!(loadFlags & cjOPLF_NoVerify))
		Validate();

	m_valid = cjTrue;
}

void cjObjPkgLoad::Validate()
{
	if(!m_validateImport)
	{
		if(cjObjCast<cjObjPackage>(m_pkg))
		{
			// remove missing import flag
			cjObjCast<cjObjPackage>(m_pkg)->m_pkgFlags &= ~cjPkgFlag_MissingImportLinks;
		}

		for(cjINT32 i=0; i<m_importDescAry.Count(); i++)
		{
			if(!ValidateImport(i))
			{
				cjLOG(L"ERR! - failed to import - imp=%d", i);
				cjObjGetPkgLoads().RemoveItem(this);
			}
		}
	}

	m_validateImport = cjTrue;
}

cjBool cjObjPkgLoad::ValidateImport(cjIndexType impDescIndex)
{
	cjFObjImpDesc& impDesc = m_importDescAry(impDescIndex);

	if( (impDesc.m_srcPkgLoad && impDesc.m_srcPkgLoadExpIndex != cjIndex_None)
		|| impDesc.m_classPkgName == cjName_None 
		|| impDesc.m_className == cjName_None
		|| impDesc.m_objName == cjName_None)
	{
		return cjTrue; // just skip if it is loaded or none
	}
	

	//
	// phase #1 resolve package linker of importing object
	//

	cjINT32 depth=0;
	cjObj* importObjPkg=NULL;

	if(impDesc.m_pkgIndex == 0) 
	{	
		// m_pkgIndex == 0, it is cjObjPackage object in Core Package

		cjAssert(impDesc.m_className == cjName_cjObjPackage);
		cjAssert(impDesc.m_classPkgName == cjName_cjCore);

			// create package or find exist
		cjObjPackage* tmpPkg = cjObjCreatePkg(NULL, *impDesc.m_objName);
		
			// load pkg file
		if(!cjObjGetPkgLoad(&impDesc.m_srcPkgLoad, tmpPkg, NULL, 0))
			return cjFalse; // fatal error
	}
	else
	{
		cjAssert(impDesc.m_pkgIndex < 0); // must be import desc index (<0)
				
		cjIndexType parentImpDesc = -impDesc.m_pkgIndex-1;
		ValidateImport(parentImpDesc); // recursive calling until m_pkgIndex is zero

		//impDesc.m_srcPkgLoad = m_importDescAry(-impDesc.m_pkgIndex-1).m_srcPkgLoad;

		// apply result of recursive calling VeridyImport()
		impDesc.m_srcPkgLoad = m_importDescAry(parentImpDesc).m_srcPkgLoad; 

		// lookup package for this import object
		if(impDesc.m_srcPkgLoad)
		{
			cjFObjImpDesc* impDescRoot = &impDesc;
			for(; impDescRoot->m_pkgIndex < 0; 
				impDescRoot = &m_importDescAry(-impDescRoot->m_pkgIndex-1), depth++)
			{
				;
			}

			cjAssert(impDescRoot->m_pkgIndex == 0);
			importObjPkg = cjObjCreatePkg(NULL, *impDescRoot->m_objName);
		}
	}


	//
	// Phase #2 find import object in linker(loaded from pkg file or already exist)
	//

	cjBool safeReplace = 0;
	cjINT32 hash = cjName::GenHash(impDesc.m_objName, impDesc.m_className,
		impDesc.m_classPkgName) & (cjARRAYCNT(m_exportHash)-1);

	//
	// resolve impDesc.m_srcPkgLoadExpIndex
	//
	if(impDesc.m_srcPkgLoad)
	{		
		// iterate export hash of package load (impDesc) 
		for(cjINT32 expDescIdx = impDesc.m_srcPkgLoad->m_exportHash[hash];
			expDescIdx != cjIndex_None;
			expDescIdx = impDesc.m_srcPkgLoad->m_exportDescAry(expDescIdx).m_hashNext)
		{			
			cjFObjExpDesc& curExpDesc = impDesc.m_srcPkgLoad->m_exportDescAry(expDescIdx);

			// check objName, className, pkgClassName
			if(    (curExpDesc.m_objName == impDesc.m_objName)
				&& (impDesc.m_srcPkgLoad->GetExpClsName(expDescIdx) == impDesc.m_className)
				&& (impDesc.m_srcPkgLoad->GetExpClsPkg(expDescIdx) == impDesc.m_classPkgName))
			{
				if(impDesc.m_pkgIndex < 0) 
				{
					// lookup upper object
					cjIndexType upperObjImpDescIndex = -impDesc.m_pkgIndex-1;
					cjFObjImpDesc& upperObjImpDesc = m_importDescAry(upperObjImpDescIndex);

					if(upperObjImpDesc.m_srcPkgLoad)
					{
						if(upperObjImpDesc.m_srcPkgLoadExpIndex == cjIndex_None)
						{
							//cj resolve:
							if(curExpDesc.m_pkgIndex != 0)
								continue; // continue iterate export hash
						}
						else if(upperObjImpDesc.m_srcPkgLoadExpIndex+1 != curExpDesc.m_pkgIndex)
						{
							//cj resolve:
							if(curExpDesc.m_pkgIndex != 0)
								continue; // continue iterate export hash
						}
					}
				}
				
				if(!(curExpDesc.m_objFlags & cjOF_Importable))
				{
					if(m_loadFlags & cjOPLF_AllowMissingImport)
					{
						if(cjObjCast<cjObjPackage>(m_pkg))
							cjObjCast<cjObjPackage>(m_pkg)->m_pkgFlags |= cjPkgFlag_MissingImportLinks;

						cjLOG(L"ERR! - failed to import %s,%s,%s", *impDesc.m_className,
							*GetImpObjFullName(impDescIndex), *impDesc.m_srcPkgLoad->m_filename);

						return cjTrue; // just pass
					}

					return cjFalse;
				}

				impDesc.m_srcPkgLoadExpIndex = expDescIdx;
				break;
			}

		} // end of for
	}
	
	
	//
	// Phase #3 - handle native class 
	//

	if(impDesc.m_srcPkgLoadExpIndex == cjIndex_None && importObjPkg != NULL)
	{
		// 1. package is founded, but m_srcPkgLoadExpIndex is invalid
		// 2. this may be native class in temp package
		
		// find class package
		cjObj* clsPkg = cjObjFindObject<cjObjPackage>(NULL, *impDesc.m_classPkgName);
		if(clsPkg)
		{	// find className obj from given class package
			cjObjCls* foundObjCls = cjObjFindObject<cjObjCls>(clsPkg, *impDesc.m_className);

			if(foundObjCls)
			{
				// find object
				cjObj* foundObj = cjObjFind(foundObjCls, importObjPkg, *impDesc.m_objName);

				if(    (foundObj) 					
					&& (foundObj->cjObjGetStFlag() & cjOSF_cjStaticObj) 
					&& (foundObj->cjObjGetFlag() & cjOF_Importable) 
					&& (foundObj->cjObjGetFlag() & cjOF_Transient) )   // just for cjObjCls ?
				{
					impDesc.m_impObj = foundObj;
					cjObjPkgLinkerImpCount()++;
				}
				else
				{
					cjLOG(L"Warn! -  Missing object %s  %s", *foundObjCls->GetName(),
						*GetImpObjFullName(impDescIndex));

					safeReplace = 1;
				}
			}
		}

		if(!impDesc.m_impObj && !safeReplace)
		{
			cjLOG(L"ERR! - failed to import  %s,%s,%s ", *impDesc.m_className, 
				*GetImpObjFullName(impDescIndex), *impDesc.m_srcPkgLoad->m_filename);

			if(m_loadFlags & cjOPLF_AllowMissingImport)
			{
				if(cjObjCast<cjObjPackage>(m_pkg))
					cjObjCast<cjObjPackage>(m_pkg)->m_pkgFlags |= cjPkgFlag_MissingImportLinks;

				return cjTrue; // just pass
			}

			cjLOG(L"ERR! - Failed to load class - %s , %s", *impDesc.m_className, 
				*GetImpObjFullName(impDescIndex));

			return cjFalse;
		}
	}

	return cjTrue;
}

cjName cjObjPkgLoad::GetExpClsPkg(cjIndexType i)
{
	cjFObjExpDesc& expObjDesc = m_exportDescAry(i);

	if(expObjDesc.m_classIndex < 0)
	{
		cjFObjImpDesc& impDesc = m_importDescAry(-expObjDesc.m_classIndex-1);
		cjAssert(impDesc.m_pkgIndex < 0);

		return m_importDescAry(-impDesc.m_pkgIndex-1).m_objName;
	}
	else if(expObjDesc.m_classIndex > 0)
	{
		return m_pkg->GetName();
	}

	return cjName_cjCore;
}

cjName cjObjPkgLoad::GetExpClsName(cjIndexType i)
{
	cjFObjExpDesc& expObjDesc = m_exportDescAry(i);

	if(expObjDesc.m_classIndex < 0)
		return m_importDescAry(-expObjDesc.m_classIndex-1).m_objName;
	else if(expObjDesc.m_classIndex > 0)
		return m_exportDescAry(expObjDesc.m_classIndex-1).m_objName;

	return cjName_cjObjCls;
}

void cjObjPkgLoad::LoadAll()
{
	for(cjINT32 i=0; i<m_header.GetExportCount(); i++)
		CreateExport(i);
}

cjINT32 cjObjPkgLoad::FindExpIndex(cjName className, cjName classPkgName, cjName objName,
	cjINT32 pkgIndex)
{
	cjINT32 hash = cjName::GenHash(objName, className, classPkgName) & (cjARRAYCNT(m_exportHash)-1);

	for(cjINT32 i=m_exportHash[hash]; i!=cjIndex_None; i=m_exportDescAry(i).m_hashNext)
	{
		if(	   (m_exportDescAry(i).m_objName	== objName )
			&& (m_exportDescAry(i).m_pkgIndex	== pkgIndex || pkgIndex == cjIndex_None)
			&& (GetExpClsPkg(i)	== classPkgName)
			&& (GetExpClsName(i)		== className) 
			)
		{
			return i;
		}
	}

	for(cjINT32 expIndex=0; expIndex<m_exportDescAry.Num(); expIndex++)
	{
		cjFObjExpDesc& expObjDesc = m_exportDescAry(expIndex);

		if(expObjDesc.m_objName == objName 
			&& (pkgIndex == cjIndex_None || expObjDesc.m_pkgIndex == pkgIndex) )
		{
			cjObjCls* expClass = cjObjCast<cjObjCls>(IndexToObj(expObjDesc.m_classIndex));

			for(cjObjCls* parentCls = expClass; 
				parentCls; 
				parentCls = parentCls->GetSuperCls())
			{
				if(parentCls->GetName() == className)
					return expIndex;
			}
		}
	}

	return cjIndex_None;
}

cjObj* cjObjPkgLoad::Create(cjObjCls* objCls, const cjName& objName, cjTFLAG loadFlags)
{
	cjINT32 index = FindExpIndex(objCls->GetName(), objCls->GetOuter()->GetName(),
		objName, cjIndex_None);

	if(index != cjIndex_None)
	{
		if((loadFlags & cjOPLF_CheckExistingInExpDescOnly))
			return (cjObj*)-1;
		else
			return CreateExport(index);
	}

	return NULL;
}

void cjObjPkgLoad::Preload(cjObj* obj)
{
	cjAssert(IsValid());
	cjAssert(obj);

	if(obj->cjObjGetPkgLoad() == this)
	{
		if(obj->cjObjGetFlag() & cjOF_NeedLoad)
		{
			// if cjObjCls, load base cjObjCls
			if(obj->cjObjHasKindOfCls(cjObjCls::cjObjGetStaticCls()))
			{
				if(((cjPropStruct*)obj)->GetSuperStruct())
					Preload(((cjPropStruct*)obj)->GetSuperStruct());
			}

			// Load the local object now.
			cjFObjExpDesc& expObjDesc = m_exportDescAry(obj->cjObjGetPkgLoadExpIndex());
			cjAssert(expObjDesc.m_expObj==obj);

			if(cjObj::cjObjIsModulePkg(m_pkg))
			{
				obj->cjObjClearFlag(cjOF_NeedLoad);
				obj->cjObjSetFlag(cjOF_Preloading);
				obj->Serialize(*this);
				obj->cjObjClearFlag(cjOF_Preloading);
			}
			else
			{
				cjStreamOffset oldStreamPos = m_loadStream->GetPos();

				m_loadStream->SetPos(expObjDesc.m_offset);
				m_loadStream->PreCache(expObjDesc.m_siz);

				// remove cjOF_NeedLoad -> add cjOF_Loading -> Serialize -> remove cjOF_Loading
				obj->cjObjClearFlag(cjOF_NeedLoad);
				obj->cjObjSetFlag(cjOF_Preloading);
				obj->Serialize(*this);
				obj->cjObjClearFlag(cjOF_Preloading);

				// check serialized siz
				cjStreamOffset deltaSiz = GetPos() - expObjDesc.m_offset;
				if(deltaSiz != expObjDesc.m_siz)
				{
					cjNameChar buf[cjMAX_PATH];
					cjUINT32 delta = (cjUINT32)(GetPos() - expObjDesc.m_offset);
					cjLOG(_CT("ERR! - serialize, %s, %u, %u"), 
						obj->cjObjGetClassNameAndPathName(buf, cjMAX_PATH), 
						delta, expObjDesc.m_siz);

					cjFatal(0);
				}

				m_loadStream->SetPos(oldStreamPos);
			}
		}
	}
	else if(obj->cjObjGetPkgLoad())
	{ 	
		// route preload call if obj is in another pkgload
		obj->cjObjGetPkgLoad()->Preload(obj);
	}
}

cjObj* cjObjPkgLoad::CreateExport(cjIndexType expIndex)
{
	cjFObjExpDesc& expObjDesc = m_exportDescAry(expIndex);

	if(!expObjDesc.m_expObj) // && (expObjDesc.m_flags & m_loadflags))
	{
		cjAssert(expObjDesc.m_objName != cjName_None);
		cjAssert(!(expObjDesc.m_objFlags & cjOF_Importable));

		cjObjCls* loadCls = (cjObjCls*)IndexToObj(expObjDesc.m_classIndex);

		if(!loadCls)
			loadCls = cjObjCls::cjObjGetStaticCls();

		cjAssert(loadCls);
		cjAssert(loadCls->GetCls() == cjObjCls::cjObjGetStaticCls());

		Preload(loadCls);

		cjObj* outer = expObjDesc.m_pkgIndex ? IndexToObj(expObjDesc.m_pkgIndex) : m_pkg;
		// outer can be package or upper object
		// core.vector.nameobj
		// core = package
		// vector = upper obj of nameobj

		if(expObjDesc.m_expObj)
			return expObjDesc.m_expObj;

		cjTFLAG flag = (expObjDesc.m_objFlags & cjOF_ObjLoadFlags) | cjOF_NeedLoad | cjOF_NeedPostLoad;

		expObjDesc.m_expObj = _cjObjNew(loadCls, outer, expObjDesc.m_objName, flag);
		expObjDesc.m_expObj->cjObjSetPkgLoad(this, expIndex);

		cjObjGetLoadingAry().Push(expObjDesc.m_expObj);

		// if propstruct
		if(    expObjDesc.m_expObj->cjObjHasKindOfCls(cjObjCls::cjObjGetStaticCls()) 
			&& expObjDesc.m_superIndex)
		{
			((cjPropStruct*)expObjDesc.m_expObj)->m_superElement 
				= (cjPropStruct*)IndexToObj(expObjDesc.m_superIndex);
		}

		// If class, validate parent.
		//if(expObjDesc.m_expObj->cjObjHasKindOfCls(cjObjCls::cjObjGetStaticCls()) && expObjDesc.m_superIndex)
		//{
		//	((cjObjCls*)expObjDesc.m_expObj)->m_base 
		//		= (cjObjCls*)IndexToObj(expObjDesc.m_superIndex);
		//}
	}
	
	return expObjDesc.m_expObj;
}

cjObj* cjObjPkgLoad::CreateImport(cjIndexType index)
{
	cjFObjImpDesc& impDesc = m_importDescAry(index);

	if(!impDesc.m_impObj)
	{
		if(!impDesc.m_srcPkgLoad)
		{
			cjObjBeginLoadPkgObj();

			ValidateImport(index);

			cjObjEndLoadPkgObj();
		}

		if(impDesc.m_srcPkgLoadExpIndex != cjIndex_None)
		{
			impDesc.m_impObj = impDesc.m_srcPkgLoad->CreateExport(impDesc.m_srcPkgLoadExpIndex);
			cjObjPkgLinkerImpCount()++;
		}
	}

	return impDesc.m_impObj;
}

cjObj* cjObjPkgLoad::IndexToObj(cjIndexType index)
{
	if(index > 0)
	{
		if(!m_exportDescAry.IsValidIndex(index-1))
		{
			cjLOG(L"ERR! - index=%d , exportNum=%d ", index-1, m_exportDescAry.Num());
			cjFatal(0);
		}

		return CreateExport(index-1);
	}
	else if(index < 0)
	{
		if(!m_importDescAry.IsValidIndex(-index-1))
		{
			// in static library context
			// #0. unregisterd class,    fix) add target class in cjCoreAutoRegClasses.h or cjEngineAutoRegClasses.h ..


			cjLOG(L"ERR! - index=%d , importNum=%d ", -index-1, m_importDescAry.Num());
			cjFatal(0);
		}

		return CreateImport(-index-1);
	} 

	return NULL;
}

void cjObjPkgLoad::DetachExport(cjIndexType index)
{
	cjFObjExpDesc& expObjDesc = m_exportDescAry(index);
	cjAssert(expObjDesc.m_expObj);

	if(!expObjDesc.m_expObj->cjObjIsValid())
	{
		cjLOG(L"ERR! - invalid obj - expClsName=%s , pkg=%s , objName=%s ", 
			*GetExpClsName(index), *m_pkg->GetName(), *expObjDesc.m_objName);
		cjFatal(0);
	}

	if(expObjDesc.m_expObj->cjObjGetPkgLoad() != this)
	{
		cjLOG(L"ERR! - pkgload is miss linked - expClsName=%s , pkg=%s , objName=%s ", 
			*GetExpClsName(index), *m_pkg->GetName(), *expObjDesc.m_objName);
		cjFatal(0);
	}

	if(expObjDesc.m_expObj->cjObjGetPkgLoadExpIndex() != index)
	{
		cjLOG(L"ERR! - invalid pkgLinkerIndex - expClsName=%s , pkg=%s , objName=%s ", 
			*GetExpClsName(index), *m_pkg->GetName(), *expObjDesc.m_objName);
	}

	m_exportDescAry(index).m_expObj->cjObjSetPkgLoad(NULL, cjIndex_None);
}

void cjObjPkgLoad::OnDestroy()
{
	RemoveAllDeferLoads(cjFalse);

	// delete export object
	for(cjINT32 i=0; i<m_exportDescAry.Num(); i++)
	{
		if(m_exportDescAry(i).m_expObj)
			DetachExport(i);
	}

	cjObjPkgLoadRemove(this); // if linker has created

	if(m_loadStream)
	{
		delete m_loadStream;
		m_loadStream = NULL;
	}

	cjSuper::OnDestroy();
}

cjBool cjObjPkgLoad::SetPos(cjStreamOffset pos)
{
	return m_loadStream->SetPos(pos);
}

cjStreamOffset cjObjPkgLoad::GetPos()
{
	return m_loadStream->GetPos();
}

cjStreamOffset cjObjPkgLoad::GetSiz()
{
	return m_loadStream->GetSiz();
}

cjStreamOffset cjObjPkgLoad::SerializeStream(void* v, cjStreamOffset siz)
{
	return m_loadStream->SerializeStream(v, siz);
}

cjFStream& cjObjPkgLoad::operator<<(cjObj*& obj)
{
	cjINT32 index;
	*m_loadStream << cjFStreamCmprI32(index);

	cjObj* src = IndexToObj(index);

	// obj is now pointing package object..
	cjMemCpy(&obj, &src, sizeof(cjObj*));

	return *this;
}

cjFStream& cjObjPkgLoad::operator<<(cjName& name)
{
	cjNameIndex index;
	*m_loadStream << cjFStreamCmprI32(index);

	if(!m_nameAry.IsValidIndex(index))
	{
		cjLOG(L"ERR! - Name index is invalid.. nameIndex=%d , num=%d", index, m_nameAry.Num());
		cjFatal(0);
	}

	cjName& tmpName = m_nameAry(index);
	cjMemCpy(&name, &tmpName, sizeof(cjName));

	return *this;
}

void cjObjPkgLoad::AddDeferLoad(cjFDeferLoad* deferLoad)
{
	cjAssert(!deferLoad->m_deferStream);
	cjAssert(m_deferLoads.FindItemIndex(deferLoad) == cjIndex_None);

	m_deferLoads.Push(deferLoad);

	deferLoad->m_deferStream = this;
	deferLoad->m_deferLoadPos = GetPos();
}

void cjObjPkgLoad::RemoveDeferLoad(cjFDeferLoad* deferLoad)
{
	cjAssert(deferLoad->m_deferStream == this);

	cjINT32 removedCount = m_deferLoads.RemoveItem(deferLoad);
	if(removedCount != 1)
	{
		cjLOG(_CU("ERR! - deferload instances management is corrupted.. removeCount(valid==1) =%d, pkgfile=%s"), 
			removedCount, *m_filename);
		cjFatal(0);
	}

	deferLoad->m_deferStream = NULL;
	deferLoad->m_deferLoadPos = 0;
}

void cjObjPkgLoad::RemoveAllDeferLoads(cjBool forceLoadAllDeferred)
{
	for(cjINT32 i=0; i<m_deferLoads.Num(); i++)
	{
		cjFDeferLoad* deferLoad = m_deferLoads(i);

		if(forceLoadAllDeferred)
			deferLoad->ValidateLoad();

		deferLoad->m_deferStream = NULL;
		deferLoad->m_deferLoadPos = 0;
	}

	m_deferLoads.RemoveAll();
}

cjAryOff cjObjPkgLoad::_AddIndirectExpHash(cjFObjExpDesc* dstExpDesc)
{
	cjAryOff dstIndex = cjAryOff_Invalid;

	for(cjAryOff i=0; i<m_exportDescAry.Count(); i++)
	{
		if(&m_exportDescAry(i) == dstExpDesc)
		{
			dstIndex = i;
			break;
		}
	}

	if(dstIndex == cjAryOff_Invalid)
		return dstIndex;

	cjFObjExpDesc& expDesc = m_exportDescAry(dstIndex);
	cjINT32 hash = cjName::GenHash(expDesc.m_objName, GetExpClsName(dstIndex), 
		GetExpClsPkg(dstIndex)) & (cjARRAYCNT(m_exportHash)-1);

	expDesc.m_hashNext = m_exportHash[hash];
	m_exportHash[hash] = dstIndex;

	return dstIndex;
}


//cjObjPkgLoad* cjObjPkgLoad::_BuildModulePkg(cjObj* pkg)
//{
//	cjObj* corePkg = NULL;
//	cjName corePkgName(cjName_cjCore);
//
//	if(pkg->GetName() == corePkgName)
//		corePkg = pkg;
//	else
//		corePkg = cjObj::cjObjCreatePkg(NULL, moduleName);
//
//	cjAssert(corePkg);
//	
//	cjNameChar pkgLoadName[cjMAX_PATH];
//	cjSprintf(pkgLoadName, cjMAX_PATH, _CU("%s.%s"), *corePkgName, CJPKG_DEFAULT_EXT);
//	cjTFLAG loadFlag = 0;
//
//	cjObjPkgLoad* pkgLoad = new cjObjPkgLoad(pkg, pkgLoadName, loadFlag);
//	
//	// import index 0 is always cjPackage in cjCore package
//	cjIndexType impIndex = pkgLoad->_IndirectAddImpObj(corePkg);
//	cjAssert(impIndex == 0);
//}

//cjtemp
//cjIndexType cjObjPkgLoad::_IndirectAddImpObj(cjObj* obj)
//{
//	cjFObjImpDesc* desc = new(m_importDescAry) cjFObjImpDesc;
//
///*
//      Core Pkg
//
//// import
//impAryIdx	 outerIdx	name		class			clsPkg
//	  0			0		cjCore		cjPackage		cjCore		// class cjPackage "cjCore" in "cjCore" package
//	  1			0		cjCls		cjCls			cjCore
//	  2			0		cjObj		cjCls			cjCore
//
//*/
//
//
//
//	cjObj* pkg = cjObj::LookupPkgObj(obj);
//
//	cjObjCls* cls = obj->GetCls();
//		
//	desc->m_classPkgName		= cjObj::LookupPkgObj(cls)->GetName();
//	desc->m_className			= cls->GetName();
//	desc->m_objName				= obj->GetName();
//
//	desc->m_pkgIndex			= 
//
//	desc->m_srcPkgLoad			= this;	
//	desc->m_srcPkgLoadExpIndex	=  
//	desc->m_impObj				= obj;
//
//	return -1;
//}


//
// cjObjPkgSave
//

CJOBJ_IMPL_CLASS(cjObjPkgSave)


cjObjPkgSave::cjObjPkgSave(cjObj* pkg, const cjNameChar* filename)
	: cjObjPkgLinker(pkg, filename), m_saveStream(NULL)
{
	m_err = 0;
	m_saveStream = cjFileSystem::Get()->CreateFileWriteStream(filename);

	if(!m_saveStream)
	{
#ifdef CJ_PLATFORM_WIN32
		cjUni curDirPath[cjMAX_PATH];
		cjFileSystem::Get()->GetCurDirectory(curDirPath, cjMAX_PATH);
		cjLOG(_CT("ERR! - failed to create stream %s , curdir=%s "), filename, curDirPath);
#else
		cjLOG(_CT("ERR! - failed to create stream %s "), filename);
#endif
		cjAssert(0);
		m_err = -1;
	}

	// current linker stream setup
	SetLoading(cjFalse);
	SetSaving(cjTrue);
	SetPersist(cjTrue);

	// remove and set to zero
	m_objToIndex.PushZero(cjObj::cjObjGetAllObjAry().Count());
	m_nameToIndex.PushZero(cjName::GetCount());
}

cjStreamOffset cjObjPkgSave::SerializeStream(void* v, cjStreamOffset siz)
{
	// *Linker << Linker->Summary
	// *Linker << *cjName::GetEntry

	return m_saveStream->SerializeStream(v, siz);
}

void cjObjPkgSave::OnDestroy()
{
	if(m_saveStream)
	{
		delete m_saveStream;
		m_saveStream = NULL;
	}

	cjSuper::OnDestroy();
}

cjINT32 cjObjPkgSave::GetNameToMappingIndex(cjName* name)
{
	cjINT32 r = m_nameToIndex(name->GetIndex());
	return r;
}

cjObjIndex cjObjPkgSave::GetObjToMappingIndex(cjObj* obj)
{
	return obj ? m_objToIndex(obj->cjObjGetIndex()) : 0;
}

cjBool cjObjPkgSave::SetPos(cjStreamOffset pos)
{
	return m_saveStream->SetPos(pos);
}

cjStreamOffset cjObjPkgSave::GetPos()
{
	return m_saveStream->GetPos();
}

cjFStream& cjObjPkgSave::operator<<(cjName& name)
{
	cjINT32 saveIndex = m_nameToIndex(name.GetIndex());
	return *this << cjFStreamCmprI32(saveIndex);
}

cjFStream& cjObjPkgSave::operator<<(cjObj*& obj)
{
	cjINT32 saveIndex = obj ? m_objToIndex(obj->cjObjGetIndex()) : 0;
	return *this << cjFStreamCmprI32(saveIndex);
}

//
// Saving Package
//

static cjObjPkgSave* gs_tempPkgSave = NULL;

static cjINT32 cjPkgSaveNameSort(const void* a, const void* b)
{
	int cntB = gs_tempPkgSave->GetNameToMappingIndex((cjName*)b);
	int cntA = gs_tempPkgSave->GetNameToMappingIndex((cjName*)a);

	return cntB - cntA;
}

static cjINT32 cjPkgSaveImportDescSort(const void* a, const void* b)
{
	cjObj* ia = ((cjFObjImpDesc*)a)->m_impObj;
	cjObj* ib = ((cjFObjImpDesc*)b)->m_impObj;

	return gs_tempPkgSave->GetObjToMappingIndex(ib) - gs_tempPkgSave->GetObjToMappingIndex(ia);
}

static cjINT32 cjPkgSaveExportDescSort(const void* a, const void* b)
{
	cjObj* ea = ((cjFObjExpDesc*)a)->m_expObj;
	cjObj* eb = ((cjFObjExpDesc*)b)->m_expObj;

	return gs_tempPkgSave->GetObjToMappingIndex(eb) - gs_tempPkgSave->GetObjToMappingIndex(ea);
}


class cjFStreamMarkExportTag : public cjFStream
{
public:
	virtual cjBool IsPureFStream() { return cjFalse; }

	cjFStreamMarkExportTag(cjObj* outer) : m_outer(outer)
	{
		SetObjOnly(cjTrue);

		//cjtemp is Need Saving flag?
		SetSaving(cjTrue);
		
		SetPersist(cjTrue);
	}

	cjFStream& operator<<(cjObj*& obj)
	{
		//if(!obj)
		//	return *this;

		//cjBool b = obj->cjObjIsInOuterChain(m_outer);
		//cjBool c = (obj->cjObjGetFlag() & cjOF_Transient) ? cjTrue : cjFalse;
		//cjBool d = (obj->cjObjGetFlag() & cjOF_TagExport) ? cjTrue : cjFalse;

		// skip m_outer
		// skip all of the clsObj(cjOF_Transient)
		// skip previous tagged export
		// skip transient object
		if(    obj 
			&& obj->cjObjIsInOuterChain(m_outer) 
			&& !(obj->cjObjGetFlag() & (cjOF_Transient | cjOF_TagExport))
		  )
		{
			obj->cjObjSetFlag(cjOF_TagExport);

			//cjLOG(L"[cjFStreamMarkExportTag-tagexport] %s ", *obj->GetName());

			// recursive mark cjOF_TagExport for class and outer
			cjObj* cls   = obj->GetCls();
			cjObj* outer = obj->GetOuter();

			*this << cls << outer;

			// recursive mark for child object
			obj->Serialize(*this);
		}

		return *this;
	}

	cjObj*								m_outer;
};

class cjFStreamMarkImportTag : public cjFStream
{
public:	
	virtual cjBool IsPureFStream() { return cjFalse; }

	cjFStreamMarkImportTag(cjObjPkgSave* pkgSave, cjTFLAG contextFlag)
		: m_pkgSave(pkgSave), m_contextFlag(contextFlag)
	{
		SetObjOnly(cjTrue);
		SetSaving(cjTrue);
		SetPersist(cjTrue);
	}

	cjFStream& operator<<(cjObj*& obj)
	{
		if(obj)
			cjLOG(L"[MarkImp-Exam] %s ", *obj->GetName());

		if(obj && !obj->cjObjIsPendingDelete())
		{
			// 1. skip transient obj except importable object(cls)
			if(!(obj->cjObjGetFlag() & cjOF_Transient) || (obj->cjObjGetFlag() & cjOF_Importable))
			{
				cjLOG(L"passing [MarkImp-Exam2] %s ", *obj->GetName());

				m_pkgSave->m_objToIndex(obj->cjObjGetIndex())++;
				cjLOG(L"m_objToIndex update [objIndex= %d ], m_objToIndex(%d) = %d ", 
					obj->cjObjGetIndex(), obj->cjObjGetIndex(),
					m_pkgSave->m_objToIndex(obj->cjObjGetIndex()));

				if(!(obj->cjObjGetFlag() & cjOF_TagExport))
				{
					cjLOG(L"   selected [MarkImp-Exam2] %s ", *obj->GetName());

					obj->cjObjSetFlag(cjOF_TagImport);
					
					cjObj* outer = obj->GetOuter();					
					if(outer)
						*this << outer;
				}
			}
		}

		return *this;
	}


	//   cjObj::Serialize
	//   [ if(cls != cjObjCls::StaticClass()) {
	//      ... cls->SerilizeTaggedProp(stream, (cjBYTE*)this, cls);
	cjFStream& operator<<(cjName& name)
	{
		name.SetFlags(cjOF_TagExport | m_contextFlag);
		m_pkgSave->m_nameToIndex(name.GetIndex())++;

		return *this;
	}

	cjObjPkgSave*						m_pkgSave;
	cjTFLAG								m_contextFlag;
};


// filePath = "c:\\test\\maps\\testmap.cjp"
cjBool cjObjPkgSave::SavePackage(cjObj* pkg, cjObj* keyObj, cjTFLAG flag, 
	const cjNameChar* filePath)
{
	if(cjObjIsModulePkg(pkg))
		return cjFalse;

	cjINT32 er = 0;

	cjNameChar tempFilename[cjMAX_PATH];
	cjStrncpy(tempFilename, filePath, cjMAX_PATH);

	// find path separator '/', '\', ':'
	cjINT32 sepPos = cjStrlen(tempFilename);
	while( sepPos > 0 
#ifdef CJ_PLATFORM_WIN32
		&& tempFilename[sepPos-1] != '\\'
#endif
		&& tempFilename[sepPos-1] != '/' 
		&& tempFilename[sepPos-1] !=':' )
	{
		sepPos--;
	}

	tempFilename[sepPos] = 0;
	cjStrcat(tempFilename, cjMAX_PATH, _CU("_tmpSavePkg_.tmp"));  // c:\\test\\maps\\_tempSavePkg_.tmp


	//
	// phase #0  
	//

	// 1. detach all export objects in all pkgloads
	// 2. commit all deferLoads
	cjObjPkgLoadReset(pkg, cjFalse, cjTrue);

	// 3. clear import/export tag of all object and names

	// all cjobj
	for(cjFObjIterator i; i; ++i)
		i->cjObjClearFlag(cjOF_TagImport | cjOF_TagExport);

	// all names
	for(cjINT32 i=0; i<cjName::GetCount(); i++)
	{
		if(cjName::GetEntry(i))
			cjName::GetEntry(i)->m_flags &= ~(cjOF_TagImport | cjOF_TagExport);
	}


	//
	// phase #1
	//

	// Set cjOF_TagExport on object to export	
	{
		// traverse :  obj, outer obj, child object and their class
		// skipped  : outer pkg, all of the cls(cjOF_Transient), transient obj
		cjFStreamMarkExportTag markExportTagStream(pkg);

		if(keyObj)
			markExportTagStream << keyObj;

		int tmp = 0;
		for(cjFObjIterator i; i; ++i)
		{
//			cjObj* o = *i;
			{
				cjNameChar __f[cjMAX_PATH];

				i->cjObjGetPathName(__f);

				cjLOG(L"[SavePackage-markPrev] %s  - [%d]", __f, tmp++);
			}
			
			if((flag == 0 || (i->cjObjGetFlag() & flag)) && i->cjObjIsInOuterChain(pkg))
			{
				cjLOG(L">> selected [SavePackage-markPrev] %s ", *(i->GetName()));

				cjObj* obj = *i;
				markExportTagStream << obj;
			}
		}
	}


	//
	// phase #2
	//

	cjObjPkgSave* pkgSave = new cjObjPkgSave(pkg, tempFilename);

	// #2-1 update obj ref and mark import tag and recursive to outer.
	
	// Target : cjOF_TagExported	
	// Action : Update obj[not Transient] RefNum in m_pkgSave->m_objToIndex
	//          Mark cjOF_TagImport
	//          Recursive call to outer
	//          Update m_pkgSave->m_nameToIndex

	for(cjFObjIterator i; i; ++i)
	{
		cjLOG(_CU("[TagExp] %s-outer:%s"), *(i->GetName()), i->GetOuter() ? *(i->GetOuter()->GetName()) : L" "); 

		if(i->cjObjGetFlag() & cjOF_TagExport)
		{			
			cjFStreamMarkImportTag markImportTagStream(pkgSave, 0);
			i->Serialize(markImportTagStream);

			// mark import to obj's class
			cjObjCls* cls = i->GetCls();
			markImportTagStream << cls;

			if(i->cjObjIsInOuterChain(cjObjGetTempPkg()))
			{
				cjNameChar buf[cjMAX_PATH];
				cjLOG(_CU("ERR! - error in marking importTag -(obj(%s) in tempPkg founded)."), 
					i->cjObjGetClassNameAndPathName(buf, cjMAX_PATH));

				er = -1;
				break;
			}
		}
	}
	
	// #2-2 mark tagExport to Name (obj, cls)
	// mark tagExport to Name for obj and cls is tagged export and import
	// propagate tagging export through outer
	if(!er)
	{
		for(cjFObjIterator i; i; ++i)
		{
			if(i->cjObjGetFlag() & (cjOF_TagExport | cjOF_TagImport))
			{
				i->GetName().SetFlags(cjOF_TagExport);

				cjLOG(L"[SavePackage-TagExp] %s ", *i->GetName());

				if(i->GetOuter())
				{
					i->GetOuter()->GetName().SetFlags(cjOF_TagExport);
					cjLOG(L"[SavePackage-TagExp,Outer] %s ", *(i->GetOuter()->GetName()));
				}

				if(i->cjObjGetFlag() & cjOF_TagImport)
				{
					i->GetCls()->GetName().SetFlags(cjOF_TagExport);

					cjLOG(L"[SavePackage-TagImp, Cls] %s ", *(i->GetCls()->GetName()));

					cjAssert(i->GetCls()->GetOuter());

					i->GetCls()->GetOuter()->GetName().SetFlags(cjOF_TagExport);

					cjLOG(L"[SavePackage-TagImp, Cls->Outer] %s ", *(i->GetCls()->GetOuter()->GetName()));
					
					if(!(i->cjObjGetFlag() & cjOF_Importable))
					{
						cjNameChar buf[cjMAX_PATH];
						cjLOG("Err! - private object founded on saving package - %s, %s", 
							filePath, i->cjObjGetClassNameAndPathName(buf, cjMAX_PATH));
						er = -2;
						break;
					}
				}
				else
				{					
				}
			}
		} // end of for(cjFObjIterator i; i; ++i)
	}


	// #2-2 write header, build NameMap
	// 1. push all names that are tagged exporting to m_nameAry in pkgSave

	if(!er)
	{	
		// write empty header
		*pkgSave << pkgSave->m_header;

		// build name index map
		pkgSave->m_header.m_nameOffset = pkgSave->GetPos();

		for(cjINT32 i=0; i<cjName::GetCount(); i++)
		{
			if(cjName::GetEntry(i))
			{
				cjName name((cjNameIndex)i);

				if(name.GetFlags() & cjOF_TagExport)
					pkgSave->m_nameAry.Push(name);
			}
		}

		// sort names by usage count
		gs_tempPkgSave = pkgSave;

		{
			cjAryOff cnt = pkgSave->m_nameAry.Count();
			for(cjAryOff t=0; t<cnt; t++)
			{
				cjName& na = pkgSave->m_nameAry(t);
				cjIndexType tp = pkgSave->m_nameToIndex(na.GetIndex());
				cjLOG(L"[%d] - [%05d][%s] -[%05d] ", t, na.GetIndex(), *na, tp);
			}
		}

		// at this point, pkgSave->m_nameToIndex is name reference count 
		// do decrease sorting by name reference count at pkgSave->m_nameToIndex
		cjTSort(&pkgSave->m_nameAry(0), pkgSave->m_nameAry.Num(), cjPkgSaveNameSort);
		
		{
			cjLOG(L"After Sort ");

			cjAryOff cnt = pkgSave->m_nameAry.Count();
			for(cjAryOff t=0; t<cnt; t++)
			{
				cjName& na = pkgSave->m_nameAry(t);				
				cjIndexType tp = pkgSave->m_nameToIndex(na.GetIndex());
				cjLOG(L"[%d] - [%05d][%s] - [%05d] ", t, na.GetIndex(), *na, tp);
			}
		}

		pkgSave->m_header.m_nameCount = pkgSave->m_nameAry.Num();

		//
		// replace m_nameToIndex to sequnece index
		//
		for(cjINT32 i=0; i<pkgSave->m_nameAry.Num(); i++)
		{
			// *pkgSave << cjNameEntry&
			*pkgSave << *cjName::GetEntry(pkgSave->m_nameAry(i).GetIndex());

			cjLOG(L"prev [%d] - nameIndex=%d, nameToIndex=%d ", 
				i,
				pkgSave->m_nameAry(i).GetIndex(), 
				pkgSave->m_nameToIndex(pkgSave->m_nameAry(i).GetIndex()) );

			pkgSave->m_nameToIndex(pkgSave->m_nameAry(i).GetIndex()) = i;
		}

		// generate importDescAry
		for(cjFObjIterator i; i; ++i)
		{
			if(i->cjObjGetFlag() & cjOF_TagImport)
				new(pkgSave->m_importDescAry) cjFObjImpDesc(*i);
		}
		pkgSave->m_header.m_importCount = pkgSave->m_importDescAry.Num();

		// sort by usage count.
		gs_tempPkgSave = pkgSave;
		cjTSort(&pkgSave->m_importDescAry(0), pkgSave->m_importDescAry.Num(), cjPkgSaveImportDescSort);
		
		// Build ExportMap.
		for(cjFObjIterator i; i; ++i)
		{
			if(i->cjObjGetFlag() & cjOF_TagExport)
				new(pkgSave->m_exportDescAry)cjFObjExpDesc(*i);
		}
	}

	if(!er)
	{
		for(cjINT32 i=0; i<pkgSave->m_objToIndex.Count(); i++)
		{
			cjLOG("[m_objToIndex] PrevSort, [%03d] %d ", i, pkgSave->m_objToIndex(i));
		}

		cjTSort(&pkgSave->m_exportDescAry(0), pkgSave->m_exportDescAry.Num(),
			cjPkgSaveExportDescSort);


		for(cjINT32 i=0; i<pkgSave->m_objToIndex.Count(); i++)
		{
			cjLOG("[m_objToIndex] AfterSort, [%03d] %d ", i, pkgSave->m_objToIndex(i));
		}

		pkgSave->m_header.m_exportCount = pkgSave->m_exportDescAry.Num();


		//
		// assign sequnce index
		//
		// 0 for package (end marker)
		// exportObj :  1,  2..
		// importObj : -1, -2..

			// arrange exportDesc index
		for(cjINT32 i=0; i<pkgSave->m_exportDescAry.Num(); i++)
		{
			if(pkgSave->m_exportDescAry(i).m_expObj)
			{
				cjINT32 eIdx = pkgSave->m_exportDescAry(i).m_expObj->cjObjGetIndex();
				cjLOG("Reordering[Exp]-m_objToIndex([exp objIdx] %d) = %d (i+1) , i=%d", 
					eIdx, 
					pkgSave->m_objToIndex(eIdx),
					i+1,
					i);
				pkgSave->m_objToIndex(pkgSave->m_exportDescAry(i).m_expObj->cjObjGetIndex()) = i+1;
			}
		}

			// arrange importDesc index
		for(cjINT32 i=0; i<pkgSave->m_importDescAry.Num(); i++)
		{
			cjINT32 eIdx = pkgSave->m_importDescAry(i).m_impObj->cjObjGetIndex();
			cjLOG("Reordering[Imp]-m_objToIndex([imp objIdx] %d) = %d (-i-1) , i=%d", 
				eIdx, 
				pkgSave->m_objToIndex(eIdx),
				-i-1,
				i);

			pkgSave->m_objToIndex(pkgSave->m_importDescAry(i).m_impObj->cjObjGetIndex()) = -i-1;
		}

		for(cjINT32 i=0; i<pkgSave->m_objToIndex.Count(); i++)
		{
			cjLOG("[m_objToIndex] AfterReorder, [%03d] %d ", i, pkgSave->m_objToIndex(i));
		}

	} // end of if(!er)


	if(!er)
	{
		// serialize(write) export object
		for(cjINT32 i=0; i<pkgSave->m_exportDescAry.Num(); i++)
		{
			cjFObjExpDesc& expDesc = pkgSave->m_exportDescAry(i);
			if(expDesc.m_expObj)
			{
					// cjObjCls
				if(!expDesc.m_expObj->cjObjHasKindOfCls(cjObjCls::cjObjGetStaticCls()))
				{
					expDesc.m_classIndex = pkgSave->m_objToIndex(
						expDesc.m_expObj->GetCls()->cjObjGetIndex());

					cjAssert(expDesc.m_classIndex != 0);
				}
					// cjSuperPropStruct
				if(expDesc.m_expObj->cjObjHasKindOfCls(cjPropStruct::cjObjGetStaticCls()))
				{
					cjPropStruct* propStruct = (cjPropStruct*)expDesc.m_expObj;
					if(propStruct->m_superElement)
					{
						expDesc.m_superIndex = pkgSave->m_objToIndex(
							propStruct->m_superElement->cjObjGetIndex());
						cjAssert(expDesc.m_superIndex != 0);
					}
				}

				if(expDesc.m_expObj->GetOuter() != pkg)
				{
					cjAssert(expDesc.m_expObj->GetOuter()->cjObjIsInOuterChain(pkg));

					expDesc.m_pkgIndex = pkgSave->m_objToIndex(
						expDesc.m_expObj->GetOuter()->cjObjGetIndex());

					cjAssert(expDesc.m_pkgIndex > 0);
				}

				// serialize
				expDesc.m_offset = pkgSave->GetPos();
				expDesc.m_expObj->Serialize(*pkgSave);
				expDesc.m_siz = (cjUINT32)(pkgSave->GetPos() - pkgSave->m_exportDescAry(i).m_offset);
			}
		}

		// write importDesc
		pkgSave->m_header.m_importOffset = pkgSave->GetPos();

		for(cjINT32 i=0; i<pkgSave->m_importDescAry.Num(); i++)
		{
			cjFObjImpDesc& impDesc = pkgSave->m_importDescAry(i);
			if(impDesc.m_impObj->GetOuter())
			{
				cjAssert(!impDesc.m_impObj->GetOuter()->cjObjIsInOuterChain(pkg));
				impDesc.m_pkgIndex = pkgSave->m_objToIndex(
					impDesc.m_impObj->GetOuter()->cjObjGetIndex());
				cjAssert(impDesc.m_pkgIndex < 0);
			}

			*pkgSave << impDesc;
		}

		// write exportDesc
		pkgSave->m_header.m_exportOffset = pkgSave->GetPos();

		for(cjINT32 i=0; i<pkgSave->m_exportDescAry.Num(); i++)
			*pkgSave << pkgSave->m_exportDescAry(i);

		// rewrite updated file header
		pkgSave->SetPos(0);

		//cj checkcode
		{
			cjAssert(pkgSave->m_header.GetExportCount() != 0);
			cjAssert(pkgSave->m_header.GetImportCount() != 0);
			cjAssert(pkgSave->m_header.GetNameCount() != 0);
		}

		*pkgSave << pkgSave->m_header;
		
		if(pkgSave)
		{
			//cjtemp
			{
				cjNameChar dbgSerialziePath[cjMAX_PATH];

				cjFile::ResolvePath(dbgSerialziePath, cjMAX_PATH, filePath, cjFilePathHint_Base);
				cjStrcat(dbgSerialziePath, cjMAX_PATH, _CT(".xml"));

				pkgSave->Report(dbgSerialziePath);
			}		

			delete pkgSave;
			pkgSave = NULL;
		}

	} // end of if(!er)
	

	if(!er) // all is ok
	{
		if(cjFileSystem::Get()->IsFileExist(filePath))
		{
			// delete previous package file
			cjFileSystem::Get()->DeleteFile(filePath);

			// check again
			if(cjFileSystem::Get()->IsFileExist(filePath))
			{
				cjLOG("ERR!- failed to save package (%s) - deleting previous file is fail..", filePath);
				cjAssert(!"ERR! - error on saving package file..");
				er = -10;
			}
		}

		if(!er)
		{
			// rename temporary file to save file.
			if(!cjFileSystem::Get()->Rename(filePath, tempFilename))
			{	// if fail, then delete it
				er = -11;
			}
		}
	}

	if(er)
	{	
		// failed!
		cjFileSystem::Get()->DeleteFile(tempFilename); // delete temporary file
		cjLOG(_CU("ERR! - failed to save package file (%s) "), filePath);
	}

	return er ? cjFalse : cjTrue;
}
