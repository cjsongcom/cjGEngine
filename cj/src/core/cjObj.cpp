
#include "cjCorePCH.h"
#include "cjObj.h"


//
// cjObj
//

std::vector<void*>   gs_cjObjRegistered;

CJOBJ_IMPL_CLASS(cjObj);


static cjBool gs_cjObjInit = cjFalse;

cjObj*							cjObj::_ms_lastMarkDestroy = NULL;
cjBool							cjObj::_ms_cjObjInitialized = cjFalse;

cjTAry<cjObj*>					cjObj::_ms_objects;
cjTAry<cjINT32>					cjObj::_ms_objAvail;
cjObj*							cjObj::_ms_objHash[cjObj_MAX_HASH] = { NULL };
cjTAry<cjObj*>					cjObj::_ms_objRoot;

cjUINT32						cjObj::_ms_objAutoRegisterCount = 0;
cjTAry<cjObj*>					cjObj::_ms_objAutoRegisters;
cjObj*							cjObj::_ms_objNextAutoRegister = NULL;

cjBool							cjObj::_ms_cjObjNoRegistering = cjFalse;


CJCORE_API void cjObjAddModulePkgName(const cjNameChar* pkgName, cjMakeModulePkgLoadPFN pfn)
{
	cjObjPackage::AddModulePkgName(pkgName, pfn);
}

void cjObj::Serialize(cjFStream& stream)
{
	cjObjSetFlag(cjOF_TagSerialze);
	cjObjCls* cls = GetCls();

		// make sure class of object is loaded
	if(cls != cjObjCls::cjObjGetStaticCls())
		stream.Preload(cls);

	if(!stream.IsLoading() && !stream.IsSaving())
	{
		// just serialize
		stream << _m_name;		// not to use GetName()
		stream << _m_outer;		// not to use GetOuter()
		stream << _m_cls;		// not to use GetCls()
		stream << _m_pkgload;
	}

	// serialize properties in class
	if(cls != cjObjCls::cjObjGetStaticCls())
	{
		if( stream.IsLoading() || stream.IsSaving() )
		{	// on save or load
			cls->SerializeModifiedDefaultProperty(stream, (cjBYTE*)this, cls);
		}
		else
		{
			cls->SerializePropertyInRange(stream, (cjBYTE*)this, 0);
		}
	}

	//cjINT32 siz = cls->GetPropertySiz();
}


//
// object load/create
//

cjObj* cjObj::__cjObjAlloc(cjObjCls* cls, cjObj* outer, cjName name, cjTFLAG objFlag, void* prm,
	cjObj* defaultValObj, void* memToReuse, cjObj* propSuperObj)
{	
	if(!cls)
		return NULL;

	if(cls->cjObjGetIndex() == cjIndex_None && _ms_objAutoRegisterCount == 0)
	{
		cjError("Fatal Err, cjObjCls is not loaded ..");
		return NULL;
	}
	
	// only cjObjCls_cjObjPackage can be pass with outer is null
	if(!outer && cls != cjObjPackage::cjObjGetStaticCls())
	{
		cjError("Fatal Err, outer is null, but given cjObjCls is not cjObjPackage..");
		return NULL;
	}

	cjTFLAG prevObjStFlag = 0;
			
	if(	name == cjName_None 
		&& ( outer==cjObj::cjObjGetTempPkg() 
		     || (!outer || outer->cjObjIsInOuterChain(cjObj::cjObjGetTempPkg()))
			 || (objFlag & cjOF_Transient) )
	)
	{
		name = cjName_Transient;		
	}

	if(name == cjName_None)
		name = cjObjGenerateUniqueName(outer, cls);
	
	cjObj* obj = NULL;
	if(name != cjName_Transient) // find already exists
		obj = cjObjFind(cls, outer, *name);
		
	cjObjCls* objIsClsInst = obj ? cjObjCast<cjObjCls>(obj) : NULL;
	void (*prevClsConstructor)(void*) = NULL;
	cjTFLAG prevClsFlag = 0;
			
	cjObjIndex desiredObjIndex = (cjObjIndex)cjObjIndex_None;

	if(!obj)
	{	// create a new object
		obj = memToReuse ? (cjObj*)memToReuse : (cjObj*)_cjObjMalloc(cls->GetPropertySiz(), *name);
	}
	else
	{	
		// object already exist.. rebuild
		if(obj->GetCls() != cls)
		{
			cjError("Fatal Err, cjObjCls is not match..");
		}

		prevObjStFlag = obj->_m_objStFlag;
		objFlag |= obj->_m_objFlag;
		desiredObjIndex = obj->_m_index;

		if(objIsClsInst)
		{
			prevClsConstructor = objIsClsInst->m_clsInternalConstructor;
			prevClsFlag = objIsClsInst->m_clsFlag;
		}

		obj->~cjObj(); // add object index into _ms_objAvail
		cjAssert(_ms_objAvail.Num() && _ms_objAvail.Last() == desiredObjIndex);
		_ms_objAvail.Pop(); // last pop index must be _m_index;
	}

	if( cls->cjObjGetClassFlag() & cjClsFL_Transient)
		objFlag |= cjOF_Transient; // cls is transient, so obj must be transient

	obj->_m_index			= cjObjIndex_None;
	obj->_m_nextHash		= NULL;
	obj->_m_objStFlag		= prevObjStFlag;
	obj->_m_pkgload			= NULL;
	obj->_m_pkgLoadExpIndex	= cjIndex_None;
	obj->_m_outer			= outer;
	obj->_m_prm				= prm;
	obj->_m_objFlag			= objFlag;
	obj->_m_name			= name == cjName_Transient ? cls->GetName() : name;
	obj->_m_cls				= cls;

	cjObjCls* clsDefault = cls;
	cjBYTE* defaultSrcDat = (cjBYTE*)defaultValObj;
	cjINT32 defaultSrcSiz = cls->GetPropertySiz();
	cjObj* dstObj = obj;

	obj->cjObjInitProperties((cjBYTE*)obj, cls->GetPropertySiz(),
		clsDefault, defaultSrcDat, defaultSrcSiz, dstObj, propSuperObj);

	obj->_cjObjAdd(desiredObjIndex);

	//if(cls->m_clsFlag & cjObjCls_Flag_PerObjectConfig)
	//{
	//	obj->LoadConfig();
	//}

	if(objIsClsInst)
	{
		objIsClsInst->m_clsFlag = prevClsFlag;
		objIsClsInst->m_clsInternalConstructor = prevClsConstructor;
	}
	
	return obj;
}

cjObj* cjObj::_cjObjNew(cjObjCls* cls, cjObj* outer, cjName name, cjTFLAG objFlag,
	void* prm, cjObj* defaultValObj, cjObj* propSuperObj)
{
	cjObj* obj = __cjObjAlloc(cls, outer, name, objFlag, prm, defaultValObj, 
		propSuperObj);

	if(obj)
		cls->m_clsInternalConstructor((void*)obj); // call constructor

	return obj;
}

cjObjCls* cjObj::_cjObjLoadCls(cjObjCls* baseCls, const cjNameChar* clsName,
	cjObj* outer, cjTFLAG loadFlags)
{
	cjAssert(baseCls);

	cjObjCls* cls = cjObjLoadPkgObj<cjObjCls>(outer, clsName, NULL, loadFlags);

	if(cls && cls->IsChildOf(baseCls))
		return cls;

	return NULL;
}

cjObj* cjObj::_cjObjLoadPkgObj(cjObjCls* objCls, cjObj* outer, const cjNameChar* name, 
	const cjNameChar* fileName, cjTFLAG loadFlags)
{
	cjAssert(objCls);
	cjAssert(name);

	cjObj* rst = NULL;

	cjObjBeginLoadPkgObj();

	cjObjPkgLoad* pkgLoad = NULL;
	cjObjResolveNameAndPkg(outer, name, cjTrue);

	cjObj* topOuter = outer;
	while(topOuter && topOuter->GetOuter())
		topOuter = topOuter->GetOuter();

	// if cjOPLF_NoLoadingFromFile is not present,
	// look object in currently loaded object in memory (same to just calling cjObjFind)
	if(!(loadFlags & cjOPLF_NoLoadingFromFile))
		cjObjGetPkgLoad(&pkgLoad, topOuter, fileName, loadFlags);

	if(pkgLoad)
		rst = pkgLoad->Create(objCls, name, loadFlags);

	if(!rst)
		rst = cjObjFind(objCls, outer, name);

	cjObjEndLoadPkgObj();

	return rst;
}


//
// cjObj Entry
//

cjObj::cjObj()
// do not use => : _m_outer(NULL), _m_objFlag(0) <= this will initialized in __cjObjAlloc
{
}

cjObj::cjObj(const cjObj& cjUNUSED_PARAM(rhs))
{	
	cjError("ERR! - cjObj does not support copy-constructor..");
	assert(0);
}

// called form CJOBJCLS_IMPLEMENT, static global object in dll
cjObj::cjObj(cjObjENativeConstructTag , cjObjCls* cls, const cjNameChar* name, 
	cjTFLAG flag, void* prm, const cjNameChar* pkgName)
: _m_index(cjObjIndex_None),
  _m_nextHash(NULL),
  _m_objStFlag(cjOSF_cjStaticObj),
  _m_pkgload(NULL),
  _m_pkgLoadExpIndex(cjObjIndex_None),
  _m_prm(prm),
  _m_objFlag(flag),  
  _m_cls(cls),
  _m_outer(NULL),
  _m_name(cjName_None)
{
	cjAssert(!_ms_cjObjNoRegistering);

	// temporary
		// this will changed suitable cjObjPkg object after initialization
	*(const cjNameChar **)&_m_outer = pkgName;

	// temporary
	// for post changing to cjName, because cjName is fully initialized
	// after initialization of global variable
	*(const cjNameChar **)&_m_prm	= name; // not _m_name, because index is not 64bit
	_m_nextObjAutoRegister			= _ms_objNextAutoRegister;
	_ms_objNextAutoRegister			= this;

	// Call native registration from terminal constructor.
	if( cjObjGetObjInitialized() && cjObjGetStaticCls() == cls)
		_cjObjRegister();
}

// called form CJOBJCLS_IMPLEMENT, static global object
cjObj::cjObj(cjObjEStaticConstructTag, const cjNameChar* name, cjTFLAG flag, void* prm,
	const cjNameChar* pkgName)
: _m_index(cjObjIndex_None),
  _m_nextHash(NULL),
  _m_objStFlag(cjOSF_cjStaticObj),
  _m_pkgload(NULL),
  _m_pkgLoadExpIndex(cjObjIndex_None),
  _m_prm(prm),
  _m_objFlag(flag),
  _m_outer(NULL),
  _m_name(cjName_None)
{
	// temporary
	// this will changed suitable cjObjPkg object after initialization
	*(const cjNameChar **)&_m_outer		= pkgName;

	// temporary
	*(const cjNameChar **)&_m_prm		= name;

	if(!cjObjGetObjInitialized())
	{
		_m_nextObjAutoRegister			= _ms_objNextAutoRegister;
		_ms_objNextAutoRegister			= this;
	}
	else
	{
		// this will be called from XXX::_cjObjInitStaticCls_XXX
		//_cjObjRegister();
	}
}

cjObj& cjObj::operator=(const cjObj& /*rhs*/)
{
	cjError("ERR! - cjobj does not assign operator");	
	assert(0);

	return *this;
}

cjObj::~cjObj()
{
	// cjObjGetObjInitialized is false after calling cjObj::StaticShutdown()
	if(_m_index != cjObjIndex_None && cjObjGetObjInitialized())
	{		
		// destroy if necessary
		cjObjConditionalDestroy();

		// _m_pkgLoadExpIndex is setted 0 when cjObj::cjObjConditionalDestroy() called -(cjObj::OnDestroy)-
		// 
		cjAssert(_m_pkgLoadExpIndex != cjIndex_None);
		_cjObjUnHash(_m_pkgLoadExpIndex);

		_ms_objects(_m_index) = NULL;
		_ms_objAvail.Push(_m_index);
	}
}

void* cjObj::_cjObjMalloc(size_t siz, const cjNameChar* tag)
{
	return cjLibManager::Malloc(siz, tag);
}

void cjObj::_cjObjFree(void* mem)
{
	cjLibManager::Free(mem);
}

cjBool cjObj::StaticInit()
{
	if(gs_cjObjInit)
		return cjTrue;

	gs_cjObjInit = cjTrue;

	memset(_ms_objHash, 0, sizeof(_ms_objHash));

	_ms_cjObjInitialized = cjTrue;

	// collect and register for all auto registered classes
	_cjObjProcessAutoRegister();

	// create temporary package
					// do not use cjObjNew, cjObjNew. 
					// because they will check cjObjCls is child
	_ms_objTempPkg = new (NULL, _CU("cjTempPackage") ) cjObjPackage();
	_ms_objTempPkg->cjObjSetStFlag(cjOSF_NonFilePkg);
	_ms_objTempPkg->cjObjSetStFlag(cjOSF_TempPkg);

    _ms_objTempPkg->cjObjAddToRoot();
	
	return cjTrue;
}

void cjObj::StaticTick()
{
	if(!gs_cjObjInit)
		return;

	cjLibManager::Tick();
}

void cjObj::StaticShutdown()
{
	if(!gs_cjObjInit)
		return;

	cjAssert(_ms_objOnLoading.IsEmpty());
	cjAssert(_ms_objAutoRegisters.IsEmpty());
	cjAssert(_ms_objNextAutoRegister == NULL);

	_ms_objTempPkg->cjObjRemoveFromRoot();
	
	for(cjFObjIterator i; i; ++i)
		i->cjObjSetFlag(cjOF_Unreachable | cjOF_TagGarbage);

	for(cjINT32 i=0; i< cjName::GetCount(); i++)
	{
		if(cjName::GetEntry(i))
			cjName::GetEntry(i)->m_flags |= cjOF_Unreachable;
	}

	// destroy all object
	for(cjAryOff i=0; i<_ms_objects.Num(); i++)
	{
		cjObj* obj = _ms_objects(i);

		if(!obj)
			continue;

		// with global static allocated object
		// do not call delete obj;
		obj->cjObjConditionalDestroy();
	}

	// delete instance memory except global static allocated object
	for(cjAryOff i=0; i<_ms_objects.Num(); i++)
	{
		cjObj* obj = _ms_objects(i);

		if(!obj)
			continue;

		// skip static allocated object (cjObjCls), this will be deallocated by CRT
		if(obj->cjObjGetStFlag() & cjOSF_cjStaticObj)
			continue;

		delete obj;
	}

	_ms_objects.RemoveAll();
	_ms_objAvail.RemoveAll();

	_ms_objAvail.RemoveAll();
	_ms_objAutoRegisters.RemoveAll();

	// for bypass cjObj destructor
	_ms_cjObjInitialized = cjFalse;

	_ms_cjObjPreLookupPkgFilePaths.RemoveAll();

	gs_cjObjInit = cjFalse;
}

cjBool cjObj::cjObjConditionalDestroy()
{
	if(!(cjObjGetFlag() & cjOF_Destroyed))
	{
		cjObjSetFlag((cjTFLAG)cjOF_Destroyed);
		cjObjClearFlag(cjOF_TraceRouteDestroy);

		OnDestroy();

		if(!(cjObjGetFlag() & cjOF_TraceRouteDestroy))
		{
			cjAssert("failed to route calling destroy..");
		}

		return cjTrue;
	}

	return cjFalse;
}

void cjObj::OnDestroy()
{
	cjObjSetFlag(cjOF_TraceRouteDestroy);
	cjObjExitProperties((cjBYTE*)this, GetCls());

	cjObjSetPkgLoad(NULL, cjIndex_None);

	// 0 for cjIndex_None to 0
	_m_pkgLoadExpIndex = _m_outer ? _m_outer->cjObjGetIndex() :  0; 
}


/*
	0. pkg=CJPKG_TEMP	name="obj0"
	1. pkg=NULL,		name="apkg.obj0"
	2. pkg=apkg			name="obj0"
	3. pkg=CJPKG_TEMP	name="bpkg.obj1"
*/
cjObj* cjObj::cjObjFind(cjObjCls* inCls, cjObj* pkg, const cjNameChar* inName, 
	cjBool exactClass)
{
	// lookup pkg
	cjObj* objPkg = pkg != CJPKG_TEMP ? pkg : NULL;

	// objPkg, name can be modified
	// 1. pkg=NULL,           name=apkg.obj0 => pkg=apkg's instance, name=obj0
	// 2. pkg=appkg,          name=obj0
	// 3. pkg=CJPKG_TEMP   name=apkg.obj0
	if(!cjObjResolveNameAndPkg(objPkg, inName, cjFalse))
		return NULL;

	cjName findName(inName, cjNameFind_Find);
	if(findName == cjName_None)
		return NULL;

	cjUINT32 calcHash = _cjObjCalcHash(findName);
	for(cjObj* hash=_ms_objHash[calcHash]; hash; hash=hash->_m_nextHash)
	{
		if(hash->GetName() == findName)
		{
			if(    (!objPkg && pkg == CJPKG_TEMP)
				|| (hash->_m_outer == objPkg)
				|| (pkg == CJPKG_TEMP && objPkg && hash->cjObjIsInOuterChain(objPkg)) )
			{
				if(!inCls || (exactClass ? hash->GetCls() == inCls : hash->cjObjHasKindOfCls(inCls)) )
				{
					return hash;
				}
			}
		}
		
		//if(    (hash->GetName() == findName) // 1. hash obj's name is equal
		//	&& ( 
		//		 (		!objPkg && pkg== CJPKG_TEMP)							// CJPKG_TEMP
		//			||	(hash->_m_outer == objPkg)								// or hash obj is in pkg
		//			||	(pkg== CJPKG_TEMP && objPkg && hash->cjObjIsInOuterChain(objPkg)) ) // or ANY PACKAGE and given named Pkg and hashObj is in named Pkg
		//	&& ( !inCls || (exactClass ? hash->GetCls() == inCls : hash->cjObjHasKindOfCls(inCls)) )  // cls is NULL or cls present and same or within
		//)
		//return hash;
	}

	return NULL;
}

cjName cjObj::cjObjGenerateUniqueName(cjObj* outer, cjObjCls* cls)
{
	cjNameChar baseName[cjNameSiz];
	cjNameChar rst[cjNameSiz];
	cjNameChar temp[cjNameSiz];

	cjStrcpy(baseName, cjNameSiz, *cls->GetName());
	cjNameChar* append = baseName + cjStrlen(baseName);
	while(append > baseName && cjIsDigit(append[-1]))
		append--;

	*append = 0;

	// append number after base name
	do 
	{
		cjSprintf(temp, cjNameSiz, _CU("%i"), cls->m_lastUniqueNameGenIdx++);
		cjStrncpy(rst, baseName, cjNameSiz - cjStrlen(temp)-1);
		cjStrcat(rst, cjNameSiz, temp);

	} while(cjObjFind( NULL, outer, rst));

	return rst;
}

void cjObj::_cjObjAdd(cjObjIndex index)
{
	if(index == cjObjIndex_None)
	{
		if(_ms_objAvail.Num())
		{
			index = _ms_objAvail.Pop();
			cjAssert(_ms_objects(index) == NULL);
		}
		else
		{
			index = _ms_objects.PushEmpty();
		}
	}

	_ms_objects(index) = this;
	_m_index = index;

	_cjObjHash();
}

void cjObj::_cjObjHash()
{
	cjUINT32 hash = _cjObjCalcHash(_m_name);

	_m_nextHash			= _ms_objHash[hash];
	_ms_objHash[hash]	= this;	
}

void cjObj::_cjObjUnHash(cjObjIndex /*outerIndex*/)
{
	cjUINT32 hash = _cjObjCalcHash(_m_name /* ,outerIndex*/);
	cjObj** curHashObj = &_ms_objHash[hash];

	cjINT32 numUnHash = 0;
	while(*curHashObj != NULL)
	{
		if( *curHashObj != this)
		{
			curHashObj = &(*curHashObj)->_m_nextHash;
 		}
		else
		{
			*curHashObj = (*curHashObj)->_m_nextHash;
			numUnHash++;
		}
	}

	cjAssert(numUnHash != 0);
	cjAssert(numUnHash == 1);
}

void cjObj::_cjObjProcessAutoRegister()
{
	if(++_ms_objAutoRegisterCount == 1)
	{
		// _m_nextHash is used temporary to find next autoregister class
		cjObj* nextReg = NULL;

		for( ;_ms_objNextAutoRegister; )
		{
			cjAssert(_ms_objNextAutoRegister->_m_index == cjObjIndex_None);
			_ms_objAutoRegisters.Push(_ms_objNextAutoRegister);

			nextReg = _ms_objNextAutoRegister->_m_nextObjAutoRegister;
			_ms_objNextAutoRegister->_m_nextObjAutoRegister = NULL;
			_ms_objNextAutoRegister= nextReg;			
		}

		for(cjAryOff i=0; i< _ms_objAutoRegisters.Num(); i++)
			_ms_objAutoRegisters(i)->_cjObjConditonalRegister();

		_ms_objAutoRegisters.RemoveAll();
		cjAssert(!_ms_objNextAutoRegister);
	}

	_ms_objAutoRegisterCount--;
}

void cjObj::_cjObjConditonalRegister()
{
	if(cjObjGetIndex() == cjObjIndex_None)
	{
		cjAryOff i;

		// check dupplicated
		for(i=0; i<_ms_objAutoRegisters.Num(); i++)
		{
			if(_ms_objAutoRegisters(i) == this)
				break;
		}

        if(!_ms_objAutoRegisters.IsEmpty())
        {
            cjAssert(i != _ms_objAutoRegisters.Num());
        }
        
		_cjObjRegister();
	}
}


//
//  index in _ms_objects
//

cjObj* cjObj::cjObjGetByIndex(cjObjIndex index)
{
	if(index != cjObjIndex_None && index < (cjObjIndex)_ms_objects.Num() )
		return _ms_objects(index);

	return NULL;
}


//
//
//

void cjObj::_cjObjRegister()
{
	//cjslow
    for(cjAryOff i=0; i<(cjAryOff)gs_cjObjRegistered.size(); i++)
    {
        if(gs_cjObjRegistered[i] == this)
        {
            cjAssert(0 && "ERR! - Dupplicated registering cjObj..");
            return;
        }
    }
    
    gs_cjObjRegistered.push_back((void*)this);

	// name literial is given at static global allocation
	// for avoid memory allocation conflict

	// resolve package
	const cjNameChar* pkgName = *(const cjNameChar **)&_m_outer;
	_m_outer = cjObjCreatePkg(NULL, pkgName);

	cjAssert(_m_index == cjObjIndex_None);
	const cjNameChar* nameParsed = *(const cjNameChar**)&_m_prm;
	_m_name = nameParsed;
	_m_prm = NULL; // clear
	_m_pkgLoadExpIndex = cjIndex_None;
		
	_cjObjAdd(cjObjIndex_None);
}

cjBool cjObj::cjObjIsInOuterChain(cjObj* inOuter)
{
	for(const cjObj* outer=GetOuter(); outer; outer=outer->GetOuter())
	{
		if(outer == inOuter)
			return cjTrue;
	}

	return inOuter == NULL ? cjTrue : cjFalse;
}

cjBool cjObj::cjObjHasKindOfCls(cjObjCls* cls) const
{
	//_m_objFlag & cjOF_cjObjCls
	
	// if this instance is cjObjCls, GetCls() always returns 'cjObjCls::GetStaticCls()'
	for(const cjObjCls* cur=GetCls(); cur; cur=(const cjObjCls*)cur->m_superElement)
	{	
		if(cur == cls)
			return cjTrue;
	}

	// different
	// if cls == NULL return cjTrue
	return cls ? cjFalse : cjTrue;
}

void cjObj::cjObjParseParam(const cjNameChar* /*param*/)
{

}

void cjObj::_cjObjImportClsDefaultDat(cjObjCls* cls, cjBool setOuter)
{
	// this = &cjTArray<cjBYTE> cjObjCls::m_defaults(0)

	cjMemZero(this, sizeof(cjObj));	
	*(void**)this = *(void**)cls; // copy virtual table only, copy sizeof(void*) bytes
    
    _m_index = cjObjIndex_None;
    _m_cls   = cls;

	if(setOuter)
        _m_outer = cls->GetOuter();
    
    // just clear property area -> fill m_defaults
	cjObj::cjObjInitProperties((cjBYTE*)this, cls->GetPropertySiz(), NULL, NULL, 0, setOuter ? this : NULL, NULL);
}

void cjObj::cjObjInitProperties(cjBYTE* writeTo, cjINT32 propertySiz, cjObjCls* clsDefault,
    cjBYTE* defaultSrcData, cjINT32 defaultSrcSiz, cjObj* dstObj, cjObj* superObj)
{
	cjAssert(propertySiz >= sizeof(cjObj));
	cjINT32 minInitSiz = sizeof(cjObj);

    // called from cjObj::_cjObjAlloc
	if(!defaultSrcData && clsDefault && clsDefault->m_default.Num())
	{
		defaultSrcData  = &clsDefault->m_default(0);
		defaultSrcSiz   = clsDefault->m_default.Num();
	}

	if(defaultSrcData)
	{
		cjMemCpy(writeTo+minInitSiz, defaultSrcData+minInitSiz, defaultSrcSiz-minInitSiz);
		minInitSiz = defaultSrcSiz;
	}

	if(minInitSiz < propertySiz)
		cjMemZero(writeTo+minInitSiz, propertySiz-minInitSiz);
    
   if(superObj)
   {
//		cjBYTE* clsDefaultDat = &clsDefault->m_default(0);
//       
//		for(cjTPropertyFlagIterator<cjProperty, cjObjClsF_Property> It(dstObj->GetCls()); it; ++it)
//        {
//			if(it->m_propertyFlag & cjPropF_Transient)
//				cjMemCpy(writeTo+it->m_nativeOffset, clsDefaultDat+it->m_nativeOffset, it->m_aryNum*it->m_elemSiz);
//        }
	}

	if(clsDefault)
	{
		for(cjProperty* curProp = clsDefault->m_constructorProps; curProp; curProp=curProp->m_nextConstructorProp)
		{
            // cjArrayProperty
			if(curProp->m_nativeOffset < defaultSrcSiz)
			{
				cjMemZero(writeTo + curProp->m_nativeOffset, curProp->GetSiz());
                
                cjObj* desiredSuperObj = dstObj;
                
                if(superObj && (!curProp->cjObjHasKindOfCls(cjObjProperty::cjObjGetStaticCls()) || !dstObj) )
                    desiredSuperObj = superObj;
                
                curProp->CopyAllValue(writeTo + curProp->m_nativeOffset, defaultSrcData + curProp->m_nativeOffset, desiredSuperObj);
			}
		}
	}
}

void cjObj::cjObjExitProperties(cjBYTE* dst, cjObjCls* cls)
{
    cjProperty* curProp=NULL;
    
    // for cjObjProperty
	for(curProp=cls->m_constructorProps; curProp; curProp=curProp->m_nextConstructorProp)
		curProp->DestroyValue(dst + curProp->m_nativeOffset);
}

void cjObj::cjObjConditionalPostLoad()
{
	if(cjObjGetFlag() & cjOF_NeedPostLoad)
	{
		cjObjClearFlag( cjOF_NeedPostLoad | cjOF_PostLoadInProgress);
        
		cjObjPostLoad();
        
		if(!(cjObjGetFlag() & cjOF_PostLoadInProgress))
        {
 			cjAssert(!"ERR! - cjObjPostLoad was called abnormally..");
        }
	}
}

void cjObj::cjObjPostLoad()
{
	cjObjSetFlag(cjOF_PostLoadInProgress);

    cjObj* outer = GetOuter();
    
    // if this instance is derived from cjObjProperty
	if(outer && outer->cjObjHasKindOfCls(cjObjCls::cjObjGetStaticCls()))
	{
        cjObjCls* objCls = GetCls();
    
		cjBYTE* defaultData	= &objCls->m_default(0);
		cjINT32 defaultDataSiz = objCls->m_default.Num();
        
		for(cjProperty* curProp=objCls->m_constructorProps; curProp; curProp=curProp->m_nextConstructorProp)
		{        
			// current property is in native class
			if(curProp->m_nativeOffset < defaultDataSiz)
			{
				// ObjProperty
				if(curProp->GetPropertyType() == cjName_cjObjProperty)
				{
					if(!cjMemCmp((cjBYTE*)this + curProp->m_nativeOffset,
                        defaultData + curProp->m_nativeOffset, curProp->GetSiz()))
					{
						cjINT32 i;
                        
						for(i=0;i<curProp->m_aryNum;i++)
                        {
							if(( (cjObj**)((cjBYTE*)this + curProp->m_nativeOffset) )[i])
							    break;
                        }

						if(i < curProp->m_aryNum)
						{
							curProp->CopyAllValue((cjBYTE*)this + curProp->m_nativeOffset,
                                defaultData + curProp->m_nativeOffset, this);
						}
					}
				}
				else if(curProp->GetPropertyType()==cjName_cjAryProperty)
				{
					cjAryProperty* aryProp = (cjAryProperty*)curProp;
                    
					if(aryProp->m_aryItem->GetPropertyType() == cjName_cjObjProperty &&
                        (aryProp->m_aryItem->m_propFlag & cjPropF_NeedConstructorLink) )
					{
						cjAry* ary = (cjAry*)((cjBYTE*)this + curProp->m_nativeOffset);
						cjAry* defaultAry = (cjAry*)(defaultData + curProp->m_nativeOffset);
                        
						for(cjAryOff i=0; i<ary->Num() && i<defaultAry->Num(); i++)
						{
							cjObj** obj = &((cjObj**)ary->GetData())[i];
							cjObj** defaultObj = &((cjObj**)defaultAry->GetData())[i];

							if(*obj && *obj==*defaultObj)
								aryProp->m_aryItem->CopyAllValue(obj, defaultObj, this);
						}
					}
				} // end of if(curProp->GetID() == cjNameAryProperty)
			}
            
		} // end of for
	} // end of if
    
}

void cjObj::cjObjPostEditChange()
{
}

cjBool cjObj::cjObjIsValid() const
{
	cjIndexType objIndex = cjObjGetIndex();
	const cjObjAry& objs = cjObjGetAllObjAry();
	cjNameChar buf[cjMAX_PATH];

	if(!objs.IsValidIndex(objIndex))
	{
		cjLOG(_CU("ERR! - Invalid cjObj index=%d / this=%s "), cjObjGetIndex(), 
			cjObjGetClassNameAndPathName(buf, cjMAX_PATH));
		
		return cjFalse;
	}
	else if(!objs(objIndex))
	{
		cjLOG(_CU("ERR! - cjObjIndex=%d , obj in that index is null, this=%s"), 
			objIndex, cjObjGetClassNameAndPathName(buf, cjMAX_PATH));

		return cjFalse;
	}
	else if(objs(objIndex) != this)
	{
		cjLOG(_CU("ERR! - cjObjIndex=%d ,[index]=%s and this=%s is different "),
			objIndex, objs(objIndex)->cjObjGetClassNameAndPathName(buf, cjMAX_PATH), 
			cjObjGetClassNameAndPathName(buf, cjMAX_PATH));

		return cjFalse;
	}

	return cjTrue;
}

void cjObj::cjObjAddToRoot()
{
	if(_ms_objRoot.FindItemIndex(this) == cjAryOff_Invalid)
		_ms_objRoot.Push(this);
}

void cjObj::cjObjRemoveFromRoot()
{
	_ms_objRoot.RemoveItem(this);
}

void cjObj::SerializeRootSet(cjFStream& stream, cjTFLAG keepFlags, cjTFLAG requiredFlags)
{
	stream << _ms_objRoot;

	for(cjFObjIterator i; i; ++i)
	{
		if(	   (i->cjObjGetFlag() & keepFlags)
			&& (i->cjObjGetFlag() & requiredFlags) == requiredFlags)
		{
			cjObj* obj = *i;
			stream << obj;
		}
	}
}


//
// cjElement
//

CJOBJ_IMPL_CLASS(cjElement)

CJOBJ_IMPL_SERIALIZE_BEGIN(cjElement)
	stream << m_superElement << m_nextElement;
CJOBJ_IMPL_SERIALIZE_END(cjElement)

cjElement::cjElement(cjObjENativeConstructTag tag, cjObjCls* cls, const cjNameChar* name, 
	const cjNameChar* pkgName, cjTFLAG flag, cjElement* superElement, void* prm)
	: cjObj(tag, cls, name, flag, prm, pkgName), m_superElement(superElement), m_nextElement(cjNULL)
{
}

cjElement::cjElement(cjObjEStaticConstructTag tag, const cjNameChar* name, const cjNameChar* pkgName, 
	cjTFLAG flag, void* prm)
	: cjObj(tag, name, flag, prm, pkgName), m_nextElement(cjNULL)
{
}

cjElement::cjElement(cjElement* superElement) : m_superElement(superElement)
{
}

void cjElement::_cjObjRegister()
{
	cjSuper::_cjObjRegister();

	if(m_superElement)
		m_superElement->_cjObjConditonalRegister();
}

cjObjCls* cjElement::GetOwnerCls()
{
	cjObj* obj;

	for(obj=this; obj->GetCls() != cjObjCls::cjObjGetStaticCls(); obj=obj->GetOuter())
		;

	return (cjObjCls*)obj;
}


//
// cjPropStruct
//

CJOBJ_IMPL_CLASS(cjPropStruct)

CJOBJ_IMPL_SERIALIZE_BEGIN(cjPropStruct)

	stream << m_childElement;
	
	if(stream.IsLoading())
		LinkProperty(stream, cjTrue); // load property from package

CJOBJ_IMPL_SERIALIZE_END(cjPropStruct)

cjPropStruct::cjPropStruct(cjPropStruct* superStruct)
: cjElement(superStruct),
	m_propertySiz(superStruct ? superStruct->GetPropertySiz() : 0),
	m_refProps(cjNULL)
{
}

cjPropStruct::cjPropStruct(cjObjENativeConstructTag , cjINT32 propSiz, const cjNameChar* name, 
	const cjNameChar* pkgName, cjTFLAG flag, cjPropStruct* superPropStruct, void* prm)
  : cjElement(cjObjENativeConstruct, cjObjCls::cjObjGetStaticCls(), name, pkgName, flag, superPropStruct, prm),
	m_childElement(cjNULL),
	m_propertySiz(propSiz),
	m_props(cjNULL),
	m_refProps(cjNULL),
	m_constructorProps(cjNULL),
	m_configProps(cjNULL)
{
}

cjPropStruct::cjPropStruct(cjObjEStaticConstructTag , cjINT32 propSiz, const cjNameChar* name, 
	const cjNameChar* pkgName, cjTFLAG flag, void* prm)
  : cjElement(cjObjEStaticClassConstruct, name, pkgName, flag, prm),
	m_childElement(cjNULL),
	m_propertySiz(propSiz),
	m_props(cjNULL),
	m_refProps(cjNULL),
	m_constructorProps(cjNULL),
	m_configProps(cjNULL)
{
}

void cjPropStruct::AddNativeProperty(cjProperty* prop)
{
	if(!prop)
	{
		cjAssert(!"ERR! - null property..");
		return;
	}

	prop->m_nextElement = m_childElement;
	m_childElement = prop;
}

cjBool cjPropStruct::IsChildOf(cjPropStruct* dst) const
{	
	for(const cjPropStruct* cur=this; cur; cur=cur->GetSuperStruct())
	{
		if(cur==dst) 
			return cjTrue;
	}

	return cjFalse;
}

void cjPropStruct::LinkProperty(cjFStream& stream, cjBool loadProperty)
{
	if(loadProperty)
	{
		// called from cjObjCls::Serialize

		m_propertySiz = 0;

		if(GetSuperStruct())
		{
			// load super class
			stream.Preload(GetSuperStruct());

#ifdef CJ_USE_ADJUST_CJOBJCLS_PROPSIZ_ALIGN
			//cjdefer
			m_propertySiz = cjAlignAddr(GetInheritanceSuper()->GetPropertySiz(), 
				cjObj_ClsPropertySizAlignByte);
#else
			//cjdefer
			m_propertySiz = GetSuperStruct()->GetPropertySiz();
#endif
		}

		cjProperty* prevProp = cjNULL;
		for(cjElement* cur=m_childElement; cur; cur=cur->m_nextElement)
		{
			// load next element
			stream.Preload(cur); 

			if(cur->GetOuter() != this)
				break;

			cjProperty* curProp = cur->GetAsProperty();
			if(curProp)
			{
				curProp->Link(stream, prevProp);
				m_propertySiz = curProp->m_nativeOffset + curProp->GetSiz();
				prevProp = curProp;
			}
		}

#ifdef CJ_USE_ADJUST_CJOBJCLS_PROPSIZ_ALIGN
		//cjdefer
		m_propertySiz = cjAlignAddr(m_propertySiz, cjObj_ClsPropertySizAlignByte);
#endif
	}
	else
	{
		// called from cjObjCls::Register()
		cjProperty* prevProp = cjNULL;
		for(cjElement* cur=m_childElement; cur && cur->GetOuter()==this; cur=cur->m_nextElement)
		{
			cjProperty* curProp = cur->GetAsProperty();

			if(curProp)
			{
				cjBoolProperty* boolProp = curProp->GetBoolProperty();
				cjINT32			prevNativeOffset = curProp->m_nativeOffset;
				cjBITFIELD		savedBitMask = boolProp ? boolProp->m_bitMask : 0;

				curProp->Link(stream, prevProp);
				curProp->m_nativeOffset = prevNativeOffset;
				prevProp = curProp;

				if(curProp->GetBoolProperty())
					curProp->GetBoolProperty()->m_bitMask = savedBitMask;
			}
		}
	}


	//
	// Make Reference Property Linked-list
	//
	cjProperty** refProps = (cjProperty**)&m_refProps;

	for(cjTObjElementClsFlagIter<cjProperty, cjClsFL_Property> it(this); it; ++it)
	{
		cjProperty* curProp = *it;

		if(curProp->GetObjProperty() || curProp->GetAryProperty())
		{
			*refProps = curProp;
			refProps = &(*refProps)->m_nextRefProp;
		}
	}
		
	*refProps = cjNULL; // marking end

	//
	// Make Property, Config, Constructor
	//
	cjProperty** props = &m_props;
	cjProperty** configProps = &m_configProps;
	cjProperty** constructorProps = &m_constructorProps;
	
	for(cjTObjElementClsFlagIter<cjProperty, cjClsFL_Property> it(this); it; ++it)
	{
		cjProperty* curProp = *it;

		if(curProp->m_propFlag & cjPropF_NeedConstructorLink)
		{
			*constructorProps = curProp;
			constructorProps  = &(*constructorProps)->m_nextConstructorProp;
		}

		if(curProp->m_propFlag & cjPropF_Config)
		{
			*configProps = curProp;
			configProps  = &(*configProps)->m_nextConfigProp;
		}

		*props = curProp;
		 props = &(*props)->m_nextProp;
	}

	// marking end
	*props				= cjNULL;
	*configProps		= cjNULL;
	*constructorProps	= cjNULL;
}

void cjPropStruct::SerializePropertyInRange(cjFStream& stream, cjBYTE* dat, cjINT32 maxReadBytes)
{
	cjName propName(cjName_None);

	cjStreamOffset maxReadPos = stream.GetPos() + maxReadBytes;
	cjINT32 index=0;

	for(cjTObjElementClsFlagIter<cjProperty, cjClsFL_Property> it(this); it; ++it)
	{
		cjProperty* curProp = *it;
		propName = curProp->GetName();
		
		if(curProp->CheckSerializeable(stream))
		{
			for(index=0; index<curProp->m_aryNum; index++)
			{				
				if( !maxReadBytes || 
					!stream.IsLoading() || // saving
					stream.GetPos() < maxReadPos)
				{
					curProp->SerializeItem(stream, dat + curProp->m_nativeOffset + (index * curProp->m_elemSiz), 0);
				}
				else
				{
					cjLOGS(11, CJLOG_WARN, _CT("invalid property, %s-%s skip."), *(curProp->GetName()), *GetName());
				}
			}
		}
	}
}

// defaultCls = mostly superClass 
void cjPropStruct::SerializeModifiedDefaultProperty(cjFStream& stream, cjBYTE* dat, cjObjCls* defaultCls)
{
	cjAssert(stream.IsLoading() || stream.IsSaving());

	cjName propName(cjName_None);
	cjINT32	aryIndex = -1;
	cjBYTE* defaultValues = cjNULL;
	cjINT32	defaultValsSiz = 0;

	if(defaultCls)
	{
		defaultValues = &defaultCls->m_default(0);
		defaultValsSiz = defaultCls->m_default.Count();
	}

	if(stream.IsLoading() )
	{		
		cjINT32 count = 0;

		for(;;)
		{
			cjPropHdrSerialzer propHdr;
			stream << propHdr;

			// end loading property
			if(propHdr.m_propName == cjName_None)
				break;

			propName = propHdr.m_propName;
			cjProperty* curProp = cjNULL;

			for(curProp=m_props; curProp; curProp=curProp->m_nextProp)
			{
				if(curProp->GetName() == propHdr.m_propName)
					break;
			}

			if(!curProp)
			{
				;
			}
			else if(propHdr.m_propType != curProp->GetPropertyType())
			{
				cjLOG(11, 2, _CT("invalid property type.. propName=%s , name=%s, propType0=%d propType=1=%d"),
					*propHdr.m_propName, *GetName(), propHdr.m_propType, curProp->GetPropertyType());
			}
			else if(propHdr.m_aryIndex >= curProp->m_aryNum)
			{
				cjLOG(11, 2, _CT("invalid array index found %s of %s: %d/%d"), 
					*propHdr.m_propName, *GetName(), propHdr.m_aryIndex, curProp->m_aryNum);
			}
			else if(!curProp->CheckSerializeable(stream))
			{				
				cjLOG(11, 5, _CT("this property does not serializeable %s of %s"), 
					*propHdr.m_propName, *GetName());
			}
			else
			{
				// loading property from file

				// load property and overwrite default of new instance
				propHdr.Serialize(stream, 
					curProp, 
					dat + curProp->m_nativeOffset + propHdr.m_aryIndex * curProp->m_elemSiz, 
					propHdr.m_propSiz);

				continue;
			}
			
			// unknown property, skip
			cjBYTE c;

			for(cjINT32 i=0; i<propHdr.m_propSiz; i++)
				stream << c;
		}

		count = 0;
	}
	else
	{
		// save property that is different with default property
		for(cjTObjElementClsFlagIter<cjProperty, cjClsFL_Property> it(this); it; ++it)
		{
			cjProperty* curProp = *it;

			// 0. check property flag - 
			if(curProp->CheckSerializeable(stream))
			{
				propName = curProp->GetName();

				for(aryIndex=0; aryIndex<curProp->m_aryNum; aryIndex++)
				{
					cjINT32 off = curProp->m_nativeOffset + aryIndex * curProp->m_elemSiz;

					// 1. first compare default property at objcls
					// 2. detect property value either value is changed (Matches(dat, ..))
					if(	(!cjObjHasKindOfCls(cjObjCls::cjObjGetStaticCls()) && !defaultValues) || 
						(!curProp->IsRawValueCompare(dat, (off + curProp->m_elemSiz <= defaultValsSiz) ? defaultValues : cjNULL,
											aryIndex))
					  )
					{
						// 3. if changed save that property only
 						cjPropHdrSerialzer modifiedDefalutProp(stream, curProp, aryIndex, dat + off);
						stream << modifiedDefalutProp;

						modifiedDefalutProp.Serialize(stream, curProp, dat + off, 0);
					}
				}
			}
		}

		// save end marker
		cjName tmpName(cjName_None);
		stream << tmpName;  
	}
}

void cjPropStruct::CleanupDestroyed(cjBYTE* dat)
{
	for(cjProperty* ref=m_refProps; ref; ref=ref->m_nextRefProp)
		ref->RemoveRefForPendingKillObj(dat + ref->m_nativeOffset);
}


//
// cjTObjElementClsFlagIter dummy operation
//

cjBool cjElement_TestClsFlag(cjElement* element, cjObjClassStaticFlag clsStaticFlag)
{
    return (element->GetCls()->m_clsFlag & clsStaticFlag) ? cjTrue : cjFalse;
}
