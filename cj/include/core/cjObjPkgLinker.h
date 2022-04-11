
#ifndef _CJOBJPKGLINKER_H_
#define _CJOBJPKGLINKER_H_


//
// cjFPkgFileHeader
//

struct CJCORE_API cjFPkgHeaderV0
{
	friend CJCORE_API cjFStream& operator<< (cjFStream& stream, cjFPkgHeaderV0& headerV0);
	const static cjUINT32					REV = 0x00010000 | 0x00000000; // version 1.0

	cjINT32									m_exportCount;
	cjStreamOffset							m_exportOffset;

	cjINT32									m_importCount;
	cjStreamOffset							m_importOffset;

	cjINT32									m_nameCount;
	cjStreamOffset							m_nameOffset;
};


#define CJPKGFILE_HEADER_MAGIC				{ 0x63, 0x6a, 0x50, 0x6b, 0x67, 0x1a, 0x00, 0x00 }
#define CJPKGFILE_REV_MAJOR					1
#define CJPKGFILE_REV_MINOR					20
#define CJPKGFILE_REV(major, minor)			(cjUINT32)(((cjWORD)(major & 0xffff) << 16) | (cjWORD)(minor & 0xffff))


struct CJCORE_API cjFPkgFileHeader
{
public:
	cjFPkgFileHeader();

	cjINT32 GetExportCount() const { return m_exportCount; }
	cjStreamOffset GetExportOffset() const { return m_exportOffset; }

	cjINT32 GetImportCount() const { return m_importCount; }
	cjStreamOffset GetImportOffset() const { return m_importOffset; }
	
	cjINT32 GetNameCount() const { return m_nameCount; }
	cjStreamOffset GetNameOffset() const { return m_nameOffset; }

	friend CJCORE_API cjFStream& operator << (cjFStream& stream, cjFPkgFileHeader& fileHeader);

public:
	cjBYTE										m_magic[8];
	cjUINT32									m_rev;			// high=major, low=minor

	cjBYTE										m_header[255];

	// internal
	cjINT32										m_exportCount;
	cjStreamOffset								m_exportOffset;
	cjINT32										m_importCount;
	cjStreamOffset								m_importOffset;
	cjINT32										m_nameCount;
	cjStreamOffset								m_nameOffset;

	cjTFLAG										m_pkgFlags;
};


//
// cjFObjImpDesc
//

struct CJCORE_API cjFObjImpDesc
{	
	cjFObjImpDesc();
	cjFObjImpDesc(cjObj* obj);

	friend CJCORE_API cjFStream& operator<<(cjFStream& stream, cjFObjImpDesc& impDesc);

	// expDescIndex in m_srcPkgLoad = cjName::GenHash(m_objName, m_className, m_classPkgName)
	// m_srcPkgLoad->m_exportDescAry[expDescIndex]
	cjName										m_classPkgName; 
	cjName										m_className;
	cjName										m_objName;
	//   0 = cjObjPackage, no need to find upper object
	// < 0 = need to find upper object in m_importDescAry(-m_pkgIndex-1) of owner linker 
	//       which has this impDesc (not a m_srcPkgLoad)
	// > 0 = find in exportDesc
	cjIndexType									m_pkgIndex;

		// internal	
	// below stuff can be null by cjObjPkgLoadReset()
	// set by cjObjPkgLoad::ValidateImport 
	cjObjPkgLoad*								m_srcPkgLoad;
	cjIndexType									m_srcPkgLoadExpIndex; 
	cjObj*										m_impObj;
};


//
// cjFObjExpDesc
//

struct CJCORE_API cjFObjExpDesc
{
	cjFObjExpDesc();
	cjFObjExpDesc(cjObj* obj);

	friend CJCORE_API cjFStream& operator<<(cjFStream& stream, cjFObjExpDesc& expDesc);
		
		// stream
	cjUINT32						m_siz;				// serialized
	cjStreamOffset					m_offset;			// serialized offset

	// pkgIndex == 0 && pkg is core : cjObjPackage in Core

		// 0: no outer, -1: cjProperties, 0~: outer obj index in expTbl
	cjIndexType						m_pkgIndex;	 // upper index going to the pkg		

		//  >0 : lookup obj in expTbl, <0 : Create obj in ImpTbl
	cjIndexType						m_classIndex;		
	cjIndexType						m_superIndex;	

	cjName							m_objName;			// from name table
	cjTFLAG							m_objFlags;

		// internal
	cjObj*							m_expObj;
	cjIndexType						m_hashNext;
};


//
// cjObjPkgLinker
//

CJOBJ_DECL_CLASS_BEGIN(CJCORE_API, cjObjPkgLinker, cjObj, cjClsFL_Transient, cjCore)

	CJOBJ_DECL_SERIALIZE

protected:
	cjObjPkgLinker() {} // disable default constructor

public:
	cjObjPkgLinker(cjObj* root, const cjNameChar* filename);

	// for debugging
	cjString GetImpObjFullName(cjIndexType i); // pkgName.aaa.bbb00
	cjString GetExpObjFullName(cjIndexType i, const cjNameChar* tmpPkgPathName=NULL);

	cjBool IsValid() const { return m_valid; }
		
public:
	cjObj*										m_pkg;		
	cjFPkgFileHeader							m_header;
	cjBool										m_valid;

	/*
		index is zero	, object has null reference
		index < 0		, object is in importDesc with position(-index-1)
		index > 0       , object is in exportDesc with position(index-1)
	*/
	
	// cjName is generated through cjName("name", cjName_Add)
	cjTAry<cjName>								m_nameAry;

	cjTAry<cjFObjImpDesc>						m_importDescAry;
	cjTAry<cjFObjExpDesc>						m_exportDescAry;

	cjString									m_filename;

	cjTFLAG										m_loadflags; // load flag mask

CJOBJ_DECL_CLASS_END(cjObjPkgLinker)


//
// cjObjPkgLoad
//

class cjFDeferLoad;

CJOBJ_DECL_CLASS_BEGIN_EX2(CJCORE_API, cjObjPkgLoad, cjObjPkgLinker, cjFStream, cjClsFL_Transient, cjCore)

	friend class cjObj;
	friend class cjPkgMap;

	CJOBJ_DECL_SERIALIZE

protected:
	// cjObj interface.
	virtual void OnDestroy();

	// cjFStream interface.
	virtual cjBool IsPureFStream() { return cjFalse; }
	virtual cjStreamOffset SerializeStream(void* v, cjStreamOffset siz);
	virtual cjBool SetPos(cjStreamOffset pos);
	virtual void Preload(cjObj* obj);

	virtual cjFStream& operator<<(cjName& name);
	virtual cjFStream& operator<<(cjObj*& obj);

		// debug support
	virtual cjStreamOffset GetDebugCurContentPos(cjWORD serial=0) 
	{ 
		return m_loadStream ? m_loadStream->GetDebugCurContentPos(serial) : -1; 
	}
	virtual cjStreamOffset GetDebugLogicalPos() { return  m_loadStream ? m_loadStream->GetDebugLogicalPos() : 0; }
	
public:
	cjObjPkgLoad(cjObj* pkg, const cjNameChar* filename, cjTFLAG loadFlags);
	cjObj* Create(cjObjCls* objCls, const cjName& objName, cjTFLAG loadFlags);
	
	void Validate();
	cjBool ValidateImport(cjIndexType i);

	void LoadAll();

	cjName GetExpClsPkg(cjIndexType i);
	cjName GetExpClsName(cjIndexType i);

	cjINT32 FindExpIndex(cjName className, cjName classPackage, cjName objName,
		cjINT32 pkgIndex);

	//cjtemp
	//static cjObjPkgLoad* _BuildModulePkg(const cjNameChar* moduleName);
	//cjIndexType _IndirectAddImpObj(cjObj* obj);

	static cjBool _GetModulePkgLoad(cjObjPkgLoad** rstPkgLoad, cjObj* pkg, 
		cjTFLAG loadFlags=0, cjBool skipFindLoad=false);

	cjAryOff _AddIndirectExpHash(cjFObjExpDesc* expDesc);

protected:
	cjObjPkgLoad() {}
	
		
private:	
	cjStreamOffset GetPos();
	cjStreamOffset GetSiz();

	cjObj* CreateExport(cjIndexType Index);
	void DetachExport(cjIndexType index);

	cjObj* CreateImport(cjIndexType Index);
	cjObj* IndexToObj(cjIndexType Index);

		// DeferLoad Service
	void AddDeferLoad(cjFDeferLoad* deferLoad);
	void RemoveDeferLoad(cjFDeferLoad* deferLoad);
	void RemoveAllDeferLoads(cjBool forceLoadAllDeferred);

public:
	cjFStream*							m_loadStream;
	cjIndexType							m_exportHash[256];
	cjUINT32							m_loadFlags;
	cjBool								m_validateImport;
	cjTAry<cjFDeferLoad*>				m_deferLoads;	

CJOBJ_DECL_CLASS_END(cjObjPkgLoad)


//
// cjObjPkgSave
//

CJOBJ_DECL_CLASS_BEGIN_EX2(CJCORE_API, cjObjPkgSave, cjObjPkgLinker, cjFStream, cjClsFL_Transient, cjCore)

	friend class cjFStreamMarkImportTag;

public:
		// debug support
	virtual cjStreamOffset GetDebugCurContentPos(cjWORD serial=0) 
	{ 
		return m_saveStream ? m_saveStream->GetDebugCurContentPos(serial) : -1; 
	}
	virtual cjStreamOffset GetDebugLogicalPos() { return  m_saveStream ? m_saveStream->GetDebugLogicalPos() : 0; }

	virtual cjINT32 GetNameToMappingIndex(cjName* name);
	virtual cjObjIndex GetObjToMappingIndex(cjObj* obj);

protected:
	// cjObj
	virtual void OnDestroy();

	// cjFStream
	virtual cjFStream& operator<< (cjName& name);
	virtual cjFStream& operator<< (cjObj*& obj);

	virtual cjBool SetPos(cjStreamOffset pos);
	virtual cjStreamOffset GetPos();
	virtual cjBool Flush() { return m_saveStream->Flush(); } // for writing
	virtual cjStreamOffset GetSiz() { return m_saveStream->GetSiz(); }

	virtual cjBool IsPureFStream() { return cjFalse; }
	
	// cjObjPkgLinker
	virtual cjStreamOffset SerializeStream(void* v, cjStreamOffset siz);

public:
	cjObjPkgSave(cjObj* parent, const cjNameChar* filename);

	static cjBool SavePackage(cjObj* pkg, cjObj* keyObj, 
		cjTFLAG flags, const cjNameChar* filePath);

protected:
	cjObjPkgSave() {}

protected:
	cjFStream*							m_saveStream;

	cjTAry<cjIndexType>					m_objToIndex;	  // index to cjObj (cjObj::cjObjGetIndex())
	cjTAry<cjIndexType>					m_nameToIndex;    // index to cjName (cjName::GetIndex())

	cjINT32								m_err;				// 0 for ok

CJOBJ_DECL_CLASS_END(cjObjPkgSave)


#endif
