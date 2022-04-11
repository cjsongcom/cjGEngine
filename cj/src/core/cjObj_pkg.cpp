
#include "cjCorePCH.h"
#include "cjObj.h"


// pkgload
cjTAry<cjObjPkgLoad*>			cjObj::_ms_objPkgLoads;
cjINT32							cjObj::_ms_objPkgLinkerImpCount;
cjNameCharPathAry				cjObj::_ms_cjObjPreLookupPkgFilePaths;
cjObjPackage*					cjObj::_ms_objTempPkg;

// loading
cjINT32							cjObj::_ms_objBeginLoadCount;
cjTAry<cjObj*>					cjObj::_ms_objOnLoading;


// serialize/load/save

void cjObj::cjObjBeginLoadPkgObj()
{
	// validate loading state
	if(++_ms_objBeginLoadCount == 1)
	{
		cjAssert(_ms_objAutoRegisterCount == 0);
		for(cjINT32 i=0; i<_ms_objPkgLoads.Num(); i++)
		{
			cjAssert(cjObjPkgLinkerGetByIndex(i)->IsValid());
		}
	}
}

static inline cjINT32 cjObjLinkerCompareLess(const cjObj*& a, const cjObj*& b)
{
	cjObjPkgLinker* lnkA = a->cjObjGetPkgLoad();
	cjObjPkgLinker* lnkB = b->cjObjGetPkgLoad();

	if(!lnkA || !lnkB || lnkA != lnkB )
		return lnkA < lnkB ? 1 : 0;

	// both of linker is same, compare serialize offset
	cjFObjExpDesc& expDescA = lnkA->m_exportDescAry(a->cjObjGetPkgLoadExpIndex());
	cjFObjExpDesc& expDescB = lnkB->m_exportDescAry(b->cjObjGetPkgLoadExpIndex());

	return expDescA.m_offset < expDescB.m_offset ? 1 : 0;
}


#ifdef _USE_CJTQSORT_DBG_

class cjTSortDBGViewEx : public cjTSortDBGView
{
public:
	cjTSortDBGViewEx()
	{
		m_seq = 0;
	}

	virtual void DBGViewQ(void* ary, cjAryOff first, cjAryOff last, cjAryOff cen) 
	{
		cjObj** a = (cjObj**)ary;

		cjMLOG(L"**** begin[%03d] %d,%d,c=%d ", m_seq, first, last, cen);

		for(cjAryOff i=0; i<m_count; i++)
		{
			cjObjPkgLinker* lnkA = a[i]->cjObjGetPkgLoad();
			cjFObjExpDesc& expDescA = lnkA->m_exportDescAry(a[i]->cjObjGetPkgLoadExpIndex());

			cjUINT32 x = expDescA.m_offset;
			cjMLOG(L"[%03d] %u ", i, x);
		}

		cjMLOG(L"**** end[%03d] %d,%d,c=%d ", m_seq, first, last, cen);

		m_seq++;
	}

	cjUINT32		m_seq;
	cjAryOff		m_count;
};

#endif

void cjObj::cjObjEndLoadPkgObj()
{
	cjAssert(_ms_objBeginLoadCount>0);

	if(--_ms_objBeginLoadCount == 0)
	{
		cjTAry<cjObj*>	loadingObjs;

		while(cjObjGetLoadingAry().Num())
		{			
			cjAryOff insStOff = loadingObjs.PushEmpty(cjObjGetLoadingAry().Num());
			
			cjMemCpy(&loadingObjs(insStOff), &cjObjGetLoadingAry()(0),
				cjObjGetLoadingAry().Num() * sizeof(cjObj*));

			cjObjGetLoadingAry().RemoveAll();

#ifdef _USE_CJTQSORT_DBG_
			cjTSortDBGViewEx sortDbgViewEx;
			sortDbgViewEx.m_count = loadingObjs.Num();
#endif

			// sort by object serialize offset, for maximize loading cache hit rate
			cjTSort(&loadingObjs(0), loadingObjs.Num(), cjObjLinkerCompareLess				
#ifdef _USE_CJTQSORT_DBG_
				, &sortDbgViewEx
#endif
					);
			

			for(cjINT32 i=0; i<loadingObjs.Num(); i++)
			{
				cjObj* obj = loadingObjs(i);
				{
					if(obj->cjObjGetFlag() & cjOF_NeedLoad)
					{
						cjAssert(obj->cjObjGetPkgLoad());
						obj->cjObjGetPkgLoad()->Preload(obj);
					}
				}
			}

			// _ms_objOnLoading may be expanded on previous iteration
			if(cjObjGetLoadingAry().Num())
				continue;

			// serializing all of the cjObj which is in loadingObjs is done at this
			// start postloading
			for(cjINT32 i=0; i<loadingObjs.Num(); i++)
				loadingObjs(i)->cjObjConditionalPostLoad();
		}

		if(cjObjPkgLinkerImpCount())
		{
			for(cjINT32 i=0; i<cjObjPkgLoadGetCount(); i++)
			{
				for(cjINT32 k=0; k<cjObjPkgLinkerGetByIndex(i)->m_importDescAry.Num(); k++)
				{
					cjFObjImpDesc& impDesc = cjObjPkgLinkerGetByIndex(i)->m_importDescAry(k);

					if(impDesc.m_impObj && !(impDesc.m_impObj->cjObjGetStFlag() & cjOSF_DestroyAtExit))
						impDesc.m_impObj = NULL;
				}
			}
		}

		cjObjPkgLinkerImpCount()=0;
	}
}

//
// linker functions
//

void cjObj::cjObjSetPkgLoad(cjObjPkgLoad* pkgLoad, cjObjIndex index)
{
	// detach from current linker
	if(_m_pkgload)
	{
		// check status in previous linker
		cjAssert(_m_pkgload->m_exportDescAry(_m_pkgLoadExpIndex).m_expObj);
		cjAssert(_m_pkgload->m_exportDescAry(_m_pkgLoadExpIndex).m_expObj == this);

		_m_pkgload->m_exportDescAry(_m_pkgLoadExpIndex).m_expObj = NULL;
	}

	_m_pkgload = pkgLoad;
	_m_pkgLoadExpIndex = index;
}

void cjObj::cjObjPkgLoadRemove(cjObjPkgLoad* linker)
{
	if(!linker)
		return;

	_ms_objPkgLoads.RemoveItem(linker);
}


//
// package functions
//

const cjNameChar* cjObj::cjObjGetClassNameAndPathName(cjNameChar* buf, cjINT32 bufSiz) const
{
//	if(this)
//	{
		const cjObjCls* cls = GetCls();
		cjSprintf(buf, bufSiz, _CU("%s "), *cls->GetName());

		cjINT32 len = cjStrlen(buf);
		cjObjGetPathName(buf+len, bufSiz-len, NULL, buf+len);
//	}
//	else
//	{
//		cjStrcpy(buf, bufSiz, *cjName::NullName);
//	}

	return buf;
}

const cjNameChar* cjObj::cjObjGetPathName(cjNameChar* buf, cjINT32 bufSiz, cjObj* stopOuter, 
	cjNameChar* str) const
{
	cjNameChar* rst = str ? str : buf;

	if(this != stopOuter)
	{
		*rst = 0;

		cjObj* outer = GetOuter();
		if(outer && outer != stopOuter)
		{
			outer->cjObjGetPathName(cjNULL, bufSiz, stopOuter, rst);
			cjStrcat(rst, bufSiz, _CU("."));
		}

		cjStrcat(rst, bufSiz, *GetName());
	}
	else 
	{
		cjSprintf(rst, bufSiz, *cjName::NullName);
	}

	return rst;
}

cjBool cjObj::cjObjResolveNameAndPkg(cjObj*& pkg, const cjNameChar*& name,
	cjBool createPkg)
{
	while(cjStrstr(name, _CU(".")))
	{
		// '.' or 'pkg.'
		if(name[0] == '.' || name[cjStrlen(name)-1] == '.')
			return cjFalse;

		cjNameChar partName[cjMAX_PATH];
		cjStrncpy(partName, name, cjARRAYCNT(partName)-1);

		cjNameChar* token = cjStrstr(partName, _CU("."));

		if(!token)
			return cjFalse;
		else
			*token = NULL;

		if(createPkg)
		{
			pkg = cjObjCreatePkg(pkg, partName); // find instance or create
		}
		else
		{
			// lookup pkg
			cjObj* newPkg = cjObjFind(cjObjPackage::cjObjGetStaticCls(), pkg, partName);

			if(!newPkg)
			{
				newPkg = cjObjFind(cjObj::cjObjGetStaticCls(), pkg, partName);

				if(!newPkg)
				{   
					// finding is fail
					if(pkg)
						return cjFalse;

					// pkg is null, find in any package
					newPkg = cjObjFind(cjObj::cjObjGetStaticCls(), CJPKG_TEMP, partName);

					if(!newPkg)
						return cjFalse;
				}
			}

			pkg = newPkg;
		}

		name = cjStrstr(name, _CU(".")) +1;
	}

	return cjTrue;
}

cjObjPackage* cjObj::cjObjCreatePkg(cjObj* outer, const cjNameChar* pkgName, cjBool delPkgFile)
{
	// test package existence
	cjObjResolveNameAndPkg(outer, pkgName, cjTrue);
	cjObjPackage* pkg = (cjObjPackage*)cjObjFind(cjObjPackage::cjObjGetStaticCls(), outer, pkgName);

	if(!pkg)
	{
		cjAssert(outer == NULL);
		pkg = new (outer, pkgName, cjOF_Importable) cjObjPackage;

		if(cjObjPackage::CheckModulePkgName(pkgName) != -1)
		{
			pkg->cjObjSetStFlag(cjOSF_NonFilePkg);
		}

		pkg->cjObjSetStFlag(cjOSF_Package);
	}

	return pkg;
}

void cjObj::cjObjPkgLoadReset(cjObj* pkg, cjBool removeLinkerPkgLoad, cjBool removeDeferLoads)
{
	// pkg is not null
		// 1. detach all export objects in linker of >> given pkg <<
		// 2. set linker and linker-Index of import objects to zero 
	    // in pkg linkers >> except linker of given pkg. <<
		
	// pkg is null (reset all linker)
		// 1. detach all export objects in all the linkers

	// reverse iterator linkers
	for(cjINT32 i=cjObjGetPkgLoads().Num()-1; i>=0; i--)
	{
		cjObjPkgLoad* cur = cjObjPkgLinkerGetByIndex(i);

		if(!pkg || cur->m_pkg == pkg)
		{	
			if(!removeLinkerPkgLoad) 
			{				
				for(cjINT32 i=0; i<cur->m_exportDescAry.Num(); i++)
				{
					cjObj*& expObj = cur->m_exportDescAry(i).m_expObj;
					// detach object in export section in linker if obj is not static created object(ex: cjObjCls)
					if(expObj && !(expObj->GetCls()->cjObjGetStFlag() & cjOSF_DestroyAtExit))
						cur->DetachExport(i);
				}
			}
			else
			{
				// pkg && cur->m_pkg != pkg
				if(removeDeferLoads)
					cur->RemoveAllDeferLoads(cjTrue);
			}
		}
		else
		{
			// process import section in linker
			// set linker, linker index to null of import object which is matched given pkg
			for(cjINT32 i=0; i<cur->m_importDescAry.Num(); i++)
			{
				if(cur->m_importDescAry(i).m_srcPkgLoad 
					&& cur->m_importDescAry(i).m_srcPkgLoad->m_pkg == pkg)
				{
					cur->m_importDescAry(i).m_srcPkgLoad = NULL;
					cur->m_importDescAry(i).m_srcPkgLoadExpIndex = cjIndex_None;
				}
			}
		}
	}

	// 1. delete all the LinkerLoad(pkg is null) 
	// 2. delete  matched linker of given pkg(pkg is not null)
	if(removeLinkerPkgLoad)
	{
		for(cjINT32 i=cjObjGetPkgLoads().Num()-1; i>=0; i--)
		{
			cjObjPkgLoad* cur = cjObjPkgLinkerGetByIndex(i);
			if(!pkg || cur->m_pkg == pkg)
					delete cur; // delete loadLinker
		}
	}
}

cjBool cjObj::cjObjSavePackage(cjObj* pkg, cjObj* topObj, cjTFLAG flags, const cjNameChar* filePath)
{
	return cjObjPkgSave::SavePackage(pkg, topObj, flags, filePath);
}

cjBool cjObj::cjObjGetPkgLoad(cjObjPkgLoad** rstPkgLoad, cjObj* pkg, 
	const cjNameChar* lookupPkgFilePath, cjTFLAG loadFlags)
{
	*rstPkgLoad = NULL;
	cjAssert(_ms_objBeginLoadCount != 0);

	//
	// 1st - lookup linker from given pkg object and if linker is founded then just return it.
	//
	if(pkg) // find previous loaded linker of given pkg object
	{
		if(cjObjIsModulePkg(pkg))
			return cjObjPkgLoad::_GetModulePkgLoad(rstPkgLoad, pkg, loadFlags, false);

		for(cjINT32 i=0; i<cjObjGetPkgLoads().Num(); i++)
		{
			if(cjObjPkgLinkerGetByIndex(i)->m_pkg == pkg)
			{
				*rstPkgLoad = cjObjPkgLinkerGetByIndex(i);
				return cjTrue;
			}
		}
	}
	
	//
	// 2nd
	//

	// 1. linker is not founded , pkg obj may be null or valid
	// 2. generate package file path(resolvedPkgFilePath) to make linker

	// 3. lookupPkgFilePath == null
	//   3.1 using name of given pkg object to resolve package file path
	//   3.2 so given pkg object must be valid
	
	// 4. lookupPkgFilePath != null
	//   4.1 strip filename(filename+ext only) : (../Map/TestPkg.000 -> TestPkg)
	//	 4.2 
	//   4.3 create package with name that is striped from resolvedPkgFilePath

	cjObjPkgLoad* pkgLoad = NULL;
	cjNameChar resolvedPkgFilePath[cjMAX_PATH]; // resolvedPkgFilePath: ../Map/TestPkg.000
	resolvedPkgFilePath[0] = NULL;
	
	if(!lookupPkgFilePath)
	{	
		if(!pkg) // must be valid package object
		{
			cjLOG(L"ERR! - failed to resolve pkg name.. pkg and filename is null..");
			return cjFalse;
		}

		// resolve package filepath from name of given pkg object
		// ex: pkg->GetName()=testmap, resolvedPkgFilePath=res/map/testmap.cjp

		if(    !(pkg->cjObjGetStFlag() & cjOSF_NonFilePkg) 
			&& (!cjObjResolvePkgFilePathByName(resolvedPkgFilePath, cjMAX_PATH, *pkg->GetName())) )
		{
			cjLOG(L"ERR! - failed to find pkg file - pkg name=%s", *pkg->GetName());
			return cjFalse;
		}
	}
	else // if(lookupPkgFilePath)
	{
		// given pkg param may be null or not.		
		// linker of given pkg is not present.
		
		// 1. lookupPkgFilePath is present.
		// 2. check pkg file on lookupPkgFilePath is exist.
		// resolvedPkgFilePath is not used
		if(    !(pkg->cjObjGetStFlag() & cjOSF_NonFilePkg) 
			&& !cjObjResolvePkgFilePathByName(resolvedPkgFilePath, cjMAX_PATH, lookupPkgFilePath))
		{
			cjLOG(L"ERR! - failed to file pkg file..  filename=%s", lookupPkgFilePath);
			return cjFalse;
		}

		// 2. strip filename only from lookupPkgFilePath to use it as package name
		// strip path: res/map/testmap.cjp -> testmap
		cjNameChar pkgNameOnly[cjMAX_PATH];
		cjObjPackage::StripPkgNameOnlyFromPath(pkgNameOnly, lookupPkgFilePath);

		// 3. find pkg with resolved package name, if not exist then create new package instance.
		cjObjPackage* genPkgFromResolvedFileName = cjObjCreatePkg(NULL, pkgNameOnly);

		if(!pkg)
		{
			if(!genPkgFromResolvedFileName)
			{
				cjAssert(0);
				cjLOG(L"ERR! - failed to load pkg file ");
				return cjFalse;
			}

			// change package instance
			pkg = genPkgFromResolvedFileName;

			// lookup linker of resolved pkg object
			for(cjINT32 i=0; i<cjObjGetPkgLoads().Num(); i++)
			{
				if(cjObjPkgLinkerGetByIndex(i)->m_pkg == pkg)
				{
					*rstPkgLoad = cjObjPkgLinkerGetByIndex(i);
					return cjTrue;
				}
			}
		}
		else if(pkg != genPkgFromResolvedFileName)
		{
			// check difference given pkg object with newly created pkg object from lookupPkgFilePath			
			cjNameChar fullPathOri[cjMAX_PATH], fullPathNew[cjMAX_PATH];

			cjLOG(L"Warn! - previous existing linker is reseted to replace to new linker (%s)->(%s)", 
				pkg->cjObjGetClassNameAndPathName(fullPathOri), 
				genPkgFromResolvedFileName->cjObjGetClassNameAndPathName(fullPathNew));

			cjObjPkgLoadReset(pkg, cjFalse, cjTrue); // do not delete pkgload
		}
	}

	cjAssert(pkg);

	if(!pkgLoad)
		pkgLoad = new cjObjPkgLoad(pkg, resolvedPkgFilePath, loadFlags);

	*rstPkgLoad = pkgLoad;

	return cjTrue;
}

cjBool cjObj::cjObjResolvePkgFilePathByName(cjNameChar* outPkgName,
	cjINT32 outPkgNameBufSiz, const cjNameChar* inName, 
	const cjNameCharPathAry* lookupPkgFilePaths)
{	
	// case s0 : inName = package Name (ex: "IceCrownMap")
	
	const cjNameCharPathAry& preLookupPaths = lookupPkgFilePaths 
		? *lookupPkgFilePaths : cjObjGetPreLookupPkgFilePathAry();

	cjNameChar ImmName[cjMAX_PATH];
	cjStrncpy(ImmName, inName, cjARRAYCNT(ImmName));
	cjStrcpy(outPkgName, ImmName); // case s0 : ImmName="IceCrownMap"
		
	cjNameChar temp[cjMAX_PATH];
	
	for(cjINT32 i=0; i<preLookupPaths.Count(); i++)
	{
		for(cjINT32 k=0; k<1; k++)
		{
			const cjNameChar* ext=0;
			*temp = 0;

			if(i<preLookupPaths.Count())
			{
				// preLookupPaths
				// [0] Res/Map/*.cjp
				// [1] Res/Mdl/*.cjp
				// [2] Res/Snd/*.cjp
				const cjNameChar* curLookupPath = (const cjNameChar*)&preLookupPaths(i);

				cjStrcat(temp, cjMAX_PATH, curLookupPath);		  // case s0 : preLookupPaths[0] Res/Map/*.cjp							
				cjNameChar* ext2 = cjStrstr(temp, _CU("*"));	  // case s0 : temp = "Res/Map/*.cjp"
				
				if(ext2)
					*ext2++ = 0;								  // case s0 : temp="Res/Map/"
				ext = ext2;										  // case s0: ext="*.cjp"

				cjStrcpy(outPkgName, temp);						  // case s0: outPkgName="Res/Map/"
				cjStrcat(outPkgName, outPkgNameBufSiz, ImmName);  // case s0: outPkgName="Res/Map/IceCrownMap"
			}

			cjBool found=cjFalse;

			// case s0: outPkgName="Res/Map/IceCrownMap", ext:*.cjp (from preLookupPaths[0])			
			cjINT32 curLen = cjStrlen(outPkgName);
			cjINT32 curExtLen = 0;
            
            if(ext && *ext)
                curExtLen = cjStrlen(ext);

			// check existance
			// case s0: check existance of "Res/Map/IceCrownMap"
			found = (    (!ext || curLen > curExtLen)
				      && (!ext || !cjStricmp(ext, &outPkgName[curLen-curExtLen]))
				      && cjFileSystem::Get()->IsFileExist(outPkgName));
						
			if(!found && ext)
			{				
				cjStrcat(outPkgName, outPkgNameBufSiz, _CU("."));	
				cjStrcat(outPkgName, outPkgNameBufSiz, ext+1); // case s0: outPkgName="Res/Map/IceCrownMap.cjp"
				
				// case s0: check existance of pkgfile "Res/Map/IceCrownMap.cjp"
				if(cjFileSystem::Get()->IsFileExist(outPkgName))
					return cjTrue;
			}
		}
	}
	
	return cjFalse;
}


//
// cjObjPackage
//

CJOBJ_IMPL_CLASS(cjObjPackage)


cjNameChar cjObjPackage::_ms_modulePkgNames[CJPKG_MAX_MODULE][cjNameSiz];
cjMakeModulePkgLoadPFN cjObjPackage::_ms_modulePkgMakePFN[CJPKG_MAX_MODULE];

cjObjPackage::cjObjPackage()
{
	m_pkgFlags = 0;
	m_dirty = cjFalse;
}

void cjObjPackage::AddModulePkgName(const cjNameChar* pkgName, cjMakeModulePkgLoadPFN pfn)
{
	if(!pkgName)
		return;

	const cjNameChar (*cur)[cjNameSiz] = _ms_modulePkgNames;
	cjINT32 i = 0;

	while(i < (CJPKG_MAX_MODULE-1))
	{
		if(!**cur)
		{
			cjStrncpy((cjNameChar*)*cur, pkgName, cjNameSiz);
			_ms_modulePkgMakePFN[i] = pfn;
			return;
		}

		i++;
        cur++;
	}

	cjAssert(i<(CJPKG_MAX_MODULE-1));
}

cjINT32 cjObjPackage::CheckModulePkgName(const cjNameChar* pkgName)
{
	if(!pkgName)
		return -1;

	cjINT32 i=0;
	const cjNameChar (*cur)[cjNameSiz] = _ms_modulePkgNames;

	while(**cur)
	{
		if(**cur)
		{
			if(!cjStrcmp((cjNameChar*)*cur, pkgName))
				return i;
		}

		i++;
		cur++;
	}

	return -1;
}

void cjObjPackage::OnDestroy()
{
	cjSuper::OnDestroy();
}

// strip path - res/map/testmap.cjp -> testmap
cjNameChar* cjObjPackage::StripPkgNameOnlyFromPath(cjNameChar* outPkgNameOnly, 
	const cjNameChar* inPkgPath, cjINT32 outSiz)
{
	if(outPkgNameOnly) *outPkgNameOnly = NULL;

	if(!inPkgPath)
		return outPkgNameOnly;

	cjStrncpy(outPkgNameOnly, inPkgPath, outSiz);

	// remove '/', ':' in given inPkgPath
	static const cjNameChar* stripStrs[] = { _CU("/"), _CU(":") };
	static const cjINT32  sizStripStrs[] = { 1, 1 };
	static const cjINT32  numStripStrs = cjARRAYCNT(stripStrs);

	cjNameChar* cur = outPkgNameOnly;
	cjNameChar* c = cur;
	cjBool cont = cjTrue;

	while(cont)
	{
		for(cjINT32 i=0; i<numStripStrs; i++)
		{
			c = cjStrstr(cur, stripStrs[i]);

			if(c)
				cur = c + sizStripStrs[i];
			else
			{
				cont = false;
				break;
			}
		}
	}

	// (test.cjp => test)
	cjNameChar* ext = cjStrstr(outPkgNameOnly, _CU("."));
	if(ext) *ext = NULL;

	return outPkgNameOnly;
}

cjBool cjObjPackage::_MakeModulePkgLoad(cjObjPkgLoad* pkgLoad)
{
	if(!cjObjIsModulePkg(this))
		return cjFalse;

	cjINT32 idx = CheckModulePkgName(*GetName());
	if(idx == -1)
		return cjFalse;

	(_ms_modulePkgMakePFN[idx])(pkgLoad);

	return cjTrue;
}
