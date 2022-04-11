
#ifndef _CJOBJPROP_H_
#define _CJOBJPROP_H_


class cjObjCls;
class cjPropStruct;

class cjProperty;
class cjBoolProperty;
class cjIntProperty;
class cjStrProperty;
class cjObjProperty;
class cjObjClsProperty; 
class cjAryProperty;
class cjSerObjRefProperty;


#ifdef CJ_PLATFORM_WIN32
  #define CJ_PROP_NATIVE_OFF(_prop_name_) cjObjEProperty, (cjBYTE*)&((cjThisClass*)0)->_prop_name_ - (cjBYTE*)0
#else
  #define CJ_PROP_NATIVE_OFF(_prop_name_) cjObjEProperty, (((cjBYTE*)(&((cjThisClass*)1)->_prop_name_)) - ((cjBYTE*)1))
#endif


//
// cjProperty
//

class CJCORE_API cjPropertySizCounter : public cjFStream
{
public:
	cjPropertySizCounter(cjFStream& stream) 
		: m_propSiz(0), m_stream(stream)
	{
		SetObjOnly(stream.IsObjOnly());
		//SetLoading(stream.IsLoading());
		SetSaving(stream.IsSaving());
		SetPersist(stream.IsPersist());
	}

	// for avoid warning C4512 in ms-vs , caused from cjFStream& m_stream;
	cjPropertySizCounter& operator=(const cjPropertySizCounter& ) { return *this; }

	cjINT32 GetPropSiz() { return m_propSiz; }

private:
	cjFStream& operator<<(cjObj*& obj)
	{
		cjObjIndex index = m_stream.GetObjToMappingIndex(obj);
		return *this << cjFStreamCmprI32(index);
	}

	cjFStream& operator<<(cjName& name)
	{
		cjINT32 index = m_stream.GetNameToMappingIndex(&name);
		return *this << cjFStreamCmprI32(index);
	}

	void Serialize(void* v, cjINT32 len)
	{
		m_propSiz += len;
	}

	cjFStream&						m_stream;
	cjINT32							m_propSiz;
};


class CJCORE_API cjPropHdrSerialzer : public cjMem
{
public:
	cjPropHdrSerialzer();
	cjPropHdrSerialzer(cjFStream& stream, cjProperty* prop, cjINT32 aryIndex, cjBYTE* v);

	friend cjFStream& operator<<(cjFStream& stream, cjPropHdrSerialzer& propHdrSerializer);
	void Serialize(cjFStream& stream, cjProperty* prop, cjBYTE* v, cjINT32 maxReadBytes);
	
public:
	cjBYTE							m_propType;			// 0 for end, 1=cjByteProperty, 2=cjIntProperty..
	cjBYTE							m_propSerFlag;
	cjName							m_propName;
	cjINT32							m_propSiz;			// property size.
	cjINT32							m_aryIndex;			// if not array then 0
};


//
// cjProperty
//

CJOBJ_DECL_ABSTRACT_BEGIN(CJCORE_API, cjProperty, cjElement, cjClsFL_Property, cjCore)

	friend class cjPropStruct;
	CJOBJ_DECL_SERIALIZE

public:
	virtual cjProperty* GetAsProperty() { return this; }

public:
    cjProperty();
	cjProperty(cjObjEPropertyTag, cjINT32 nativeOffset, const cjNameChar* cateName, cjTFLAG propFlag);

	cjPropStruct* GetPropStruct() const { return (cjPropStruct*)GetOuter(); }
    cjPropStruct* GetPropStruct() { return (cjPropStruct*)GetOuter(); }
	
	cjINT32 GetSiz() const { return m_aryNum * m_elemSiz; }

		// caster
	virtual cjBoolProperty* GetBoolProperty() { return cjNULL; }
	virtual cjIntProperty* GetIntProperty() { return cjNULL; }
	virtual cjStrProperty* GetStrProperty() { return cjNULL; }
	virtual cjObjProperty* GetObjProperty() { return cjNULL; }
	virtual cjObjClsProperty* GetObjClsProperty() { return cjNULL; }
	virtual cjAryProperty* GetAryProperty() { return cjNULL; }
	virtual cjSerObjRefProperty* GetSerObjRefProperty() { return cjNULL; }
	
	virtual cjBYTE GetPropertyType() const;

	virtual void Link(cjFStream& stream, cjProperty* prev) {}
	virtual void SerializeItem(cjFStream& stream, void* v, cjINT32 maxReadBytes=0) const = 0;	

	virtual cjBool IsIdenticalValue(const void* src, const void* dst) const = 0;
	virtual void ClearValue(cjBYTE* dst) const = 0;
	virtual cjBool HasValue(const cjBYTE* dst) const = 0;
	virtual void DestroyValue(void* dst) const {}
    virtual void CopyValue(void* dst, void* src, cjObj* /*superObj*/, cjBool isInstanceSubObj=cjTrue) const { cjMemCpy(dst, src, m_elemSiz); }
    virtual void CopyAllValue(void* dst, void* src, cjObj* superObj, cjBool isInstanceSubObj=cjTrue) const
	{
		for(cjINT32 i=0; i<m_aryNum; i++)
			CopyValue((cjBYTE*)dst + i*m_elemSiz, (cjBYTE*)src + i*m_elemSiz, superObj);
	}

    virtual void RemoveRefForPendingKillObj(cjBYTE* /*dst*/) const {} // dst is property address in instance
	
	cjBool IsRawValueCompare(const void* src, const void* dst, cjINT32 aryIndex) const
	{
		cjINT32 off = m_nativeOffset + aryIndex * m_elemSiz;
		return IsIdenticalValue((cjBYTE*)src + off, dst ? (cjBYTE*)dst + off : cjNULL);
	}

	cjBool CheckSerializeable(cjFStream& stream) const
	{
		if( (m_propFlag & cjPropF_Native) ||
			((m_propFlag & cjPropF_Transient) && stream.IsPersist()))
			return cjFalse;

		return cjTrue;
	}

	static cjBool CheckPropertyNames();

public:
	cjProperty*						m_nextProp;
		// sub collection of property
    cjProperty*                     m_nextConstructorProp;
	cjProperty*                     m_nextConfigProp;
	cjProperty*						m_nextRefProp;

	cjINT32                         m_aryNum;
    cjINT32                         m_elemSiz;
	cjINT32                         m_nativeOffset; // in class
    cjTFLAG                         m_propFlag;
    cjName                          m_cateName;    

CJOBJ_DECL_ABSTRACT_END(cjProperty)


//
// cjBoolProperty
//

CJOBJ_DECL_CLASS_BEGIN(CJCORE_API, cjBoolProperty, cjProperty, cjClsFL_Property | cjClsFL_BoolProperty, cjCore)

	CJOBJ_DECL_SERIALIZE

public:
	virtual void Link(cjFStream& stream, cjProperty* prev);
	virtual void SerializeItem(cjFStream& stream, void* v, cjINT32 maxReadBytes=0) const;

	virtual cjBoolProperty* GetBoolProperty() { return this; }

	virtual cjBool IsIdenticalValue(const void* src, const void* dst) const;
	virtual void CopyValue(void* dst, void* src, cjObj* superObj, cjBool isInstanceSubObj=cjTrue) const;
	virtual void ClearValue(cjBYTE* dst) const;
	virtual cjBool HasValue(const cjBYTE* dst) const;
	
public:
	cjBoolProperty() {}
	cjBoolProperty(cjObjEPropertyTag, cjINT32 nativeOffset, const cjNameChar* cateName, cjTFLAG propFlag)
		: cjProperty(cjObjEProperty, nativeOffset, cateName, propFlag),	m_bitMask(1) {}

	cjBITFIELD						m_bitMask;

CJOBJ_DECL_CLASS_END(cjBoolProperty)


//
// cjIntProperty
//

CJOBJ_DECL_CLASS_BEGIN(CJCORE_API, cjIntProperty, cjProperty, cjClsFL_Property, cjCore)
    
	//no need CJOBJ_DECL_SERIALIZE

public:
	virtual void Link(cjFStream& stream, cjProperty* prev);
	virtual void SerializeItem(cjFStream& stream, void* v, cjINT32 maxReadBytes=0) const;

	virtual cjIntProperty* GetIntProperty() { return this; }

	virtual cjBool IsIdenticalValue(const void* src, const void* dst) const;
	virtual void CopyValue(void* dst, void* src, cjObj* superObj, cjBool isInstanceSubObj=cjTrue) const;
    virtual void CopyAllValue(void* dst, void* src, cjObj* superObj, cjBool isInstanceSubObj=cjTrue) const;
	virtual void ClearValue(cjBYTE* dst) const;
	virtual cjBool HasValue(const cjBYTE* dst) const;

public:
    cjIntProperty()	{}
	cjIntProperty(cjObjEPropertyTag , cjINT32 nativeOffset, const cjNameChar* cateName, cjTFLAG propFlag)
        : cjProperty(cjObjEProperty, nativeOffset, cateName, propFlag) {}

CJOBJ_DECL_CLASS_END(cjIntProperty)


//
// cjStrProperty
//

CJOBJ_DECL_CLASS_BEGIN(CJCORE_API, cjStrProperty, cjProperty, cjClsFL_Property, cjCore)

	//no need CJOBJ_DECL_SERIALIZE

public:
	virtual void Link(cjFStream& stream, cjProperty* prev);
	virtual void SerializeItem(cjFStream& stream, void* v, cjINT32 maxReadBytes=0) const;

	virtual cjStrProperty* GetStrProperty() { return this; }

	virtual cjBool IsIdenticalValue(const void* src, const void* dst) const;
	virtual void ClearValue(cjBYTE* dst) const;
	virtual cjBool HasValue(const cjBYTE* dst) const;	
	virtual void DestroyValue(void* dst) const;
	virtual void CopyValue(void* dst, void* src, cjObj* superObj, cjBool isInstanceSubObj=cjTrue) const;
	
public:
	cjStrProperty()	{}
	cjStrProperty(cjObjEPropertyTag , cjINT32 nativeOffset, const cjNameChar* cateName, cjTFLAG propFlag)
        : cjProperty(cjObjEProperty, nativeOffset, cateName, propFlag) {}

CJOBJ_DECL_CLASS_END(cjStrProperty)


//
// cjObjProperty
//

CJOBJ_DECL_CLASS_BEGIN(CJCORE_API, cjObjProperty, cjProperty, cjClsFL_Property | cjClsFL_ObjProperty, cjCore)

	CJOBJ_DECL_SERIALIZE

public:
	// cjProperty
	virtual void Link(cjFStream& stream, cjProperty* prev);
	virtual void SerializeItem(cjFStream& stream, void* v, cjINT32 maxReadBytes=0) const;

	virtual cjObjProperty* GetObjProperty() { return this; }
    
	virtual cjBool IsIdenticalValue(const void* src, const void* dst) const;
    virtual void CopyValue(void* dst, void* src, cjObj* superObj, cjBool isInstanceSubObj=cjTrue) const;
    virtual void CopyAllValue(void* dst, void* src, cjObj* superObj, cjBool isInstanceSubObj=cjTrue) const;
	virtual void ClearValue(cjBYTE* dst) const;
	virtual cjBool HasValue(const cjBYTE* dst) const;

	virtual void RemoveRefForPendingKillObj(cjBYTE* dst) const;

public:
	cjObjProperty()	{} // for m_aryItem of cjAryProeprty
	cjObjProperty(cjObjEPropertyTag , cjINT32 nativeOffset, const cjNameChar* cateName, cjTFLAG propFlag, cjObjCls* cls)
        : cjProperty(cjObjEProperty, nativeOffset, cateName, propFlag), m_propCls(cls) {}
    
    cjObjCls*                       m_propCls;

CJOBJ_DECL_CLASS_END(cjObjProperty)


//
// cjObjClsProperty
//

CJOBJ_DECL_CLASS_BEGIN(CJCORE_API, cjObjClsProperty, cjObjProperty, cjClsFL_ObjProperty | cjClsFL_ClsProperty, cjCore)

	CJOBJ_DECL_SERIALIZE

public:
	virtual cjObjClsProperty* GetObjClsProperty() { return this; }

	virtual void ClearValue(cjBYTE* dst) const;
	virtual cjBool HasValue(const cjBYTE* dst) const;
    
public:
	cjObjClsProperty() {}
	cjObjClsProperty(cjObjEPropertyTag, cjINT32 nativeOffset, const cjNameChar* cateName, cjTFLAG propFlag, cjObjCls* cls);

	cjBYTE GetPropertyType() const { return cjName_cjObjProperty; }
	
	cjObjCls*						m_metaCls;

CJOBJ_DECL_CLASS_END(cjObjClsProperty)


//
// cjNameProperty
//

CJOBJ_DECL_CLASS_BEGIN(CJCORE_API, cjNameProperty, cjProperty, cjClsFL_Property, cjCore)

	//no need CJOBJ_DECL_SERIALIZE

public:
	virtual void Link(cjFStream& stream, cjProperty* prev);
	virtual void SerializeItem(cjFStream& stream, void* v, cjINT32 maxReadBytes=0) const;

	virtual cjNameProperty* GetNameProperty() { return this; }

	virtual cjBool IsIdenticalValue(const void* src, const void* dst) const;
	virtual void ClearValue(cjBYTE* dst) const;
	virtual cjBool HasValue(const cjBYTE* dst) const;
	virtual void CopyValue(void* dst, void* src, cjObj* superObj, cjBool isInstanceSubObj=cjTrue) const;
	virtual void CopyAllValue(void* dst, void* src, cjObj* superObj, cjBool isInstanceSubObj=cjTrue) const;

public:
	cjNameProperty()	{}
	cjNameProperty(cjObjEPropertyTag , cjINT32 nativeOffset, const cjNameChar* cateName, cjTFLAG propFlag)
        : cjProperty(cjObjEProperty, nativeOffset, cateName, propFlag) {}

CJOBJ_DECL_CLASS_END(cjNameProperty)


//
// cjAryProperty
//

CJOBJ_DECL_CLASS_BEGIN(CJCORE_API, cjAryProperty, cjProperty, cjClsFL_Property, cjCore)
    
	CJOBJ_DECL_SERIALIZE

public:
	virtual void Link(cjFStream& stream, cjProperty* prev);
	virtual void SerializeItem(cjFStream& stream, void* v, cjINT32 maxReadBytes=0) const;

	virtual cjAryProperty* GetAryProperty() { return this; }
    
	virtual cjBool IsIdenticalValue(const void* src, const void* dst) const;
    virtual void ClearValue(cjBYTE* dst) const;
    virtual cjBool HasValue(const cjBYTE* dst) const;
    virtual void DestroyValue(void* dst) const;

    virtual void CopyAllValue(void* dst, void* src, cjObj* superObj, cjBool isInstanceSubObj=cjTrue) const;
    virtual void RemoveRefForPendingKillObj(cjBYTE* dst) const;
    
public:
    cjAryProperty() { }
	cjAryProperty(cjObjEPropertyTag , cjINT32 nativeOffset, const cjNameChar* cateName, cjTFLAG propFlag)
        : cjProperty(cjObjEProperty, nativeOffset, cateName, propFlag) {}

    void AddNativeProperty(cjProperty* prop);

    cjProperty*                     m_aryItem;
};


//
// cjSerObjProperty
//

CJOBJ_DECL_CLASS_BEGIN(CJCORE_API, cjSerObjRefProperty, cjProperty, cjClsFL_Property | cjClsFL_SerObjRefProperty, cjCore)

	CJOBJ_DECL_SERIALIZE

public:
	// cjProperty
	virtual void Link(cjFStream& stream, cjProperty* prev);
	virtual void SerializeItem(cjFStream& stream, void* v, cjINT32 maxReadBytes=0) const;

	virtual cjSerObjRefProperty* GetSerObjRefProperty() { return this; }
    
	virtual cjBool IsIdenticalValue(const void* src, const void* dst) const;
    virtual void CopyValue(void* dst, void* src, cjObj* superObj, cjBool isInstanceSubObj=cjTrue) const;
    virtual void CopyAllValue(void* dst, void* src, cjObj* superObj, cjBool isInstanceSubObj=cjTrue) const;
	virtual void ClearValue(cjBYTE* dst) const;
	virtual cjBool HasValue(const cjBYTE* dst) const;
	virtual void DestroyValue(void* dst) const;

	virtual void RemoveRefForPendingKillObj(cjBYTE* dst) const;

public:
	cjSerObjRefProperty()	
	{
	}

	cjSerObjRefProperty(cjObjEPropertyTag , cjINT32 nativeOffset, const cjNameChar* cateName, cjTFLAG propFlag, 
		const cjRTTI* propRtti)
        : cjProperty(cjObjEProperty, nativeOffset, cateName, propFlag), m_propRTTI(propRtti) 
	{
	}
    
	// cjTRefPtr<XXX>
	const cjRTTI*                   m_propRTTI;

CJOBJ_DECL_CLASS_END(cjSerObjRefProperty)



#endif
