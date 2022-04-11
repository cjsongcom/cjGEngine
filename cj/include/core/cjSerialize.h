
#ifndef _CJSERIALIZE_H_
#define _CJSERIALIZE_H_


#include "cjRTTI.h"

class cjSerializable;
class cjNode;
class cjFStream;
class cjObjCls;
class cjRef;


enum CJSER_FLAG
{
	// 0x0000ffff = serialize
	CJSER_FL_EXTERN					= 0x00000001,

	// non serialize
	CJSER_FL_REF					= 0x01000000, // is cjRef driven class ?
	CJSER_FL_REF_DELETEABLE			= 0x02000000, // setted at cjRef::ReleaseRef()

	CJSER_FL_DESTROYED				= 0x10000000,
	CJSER_FL_TRACE_DESTROY			= 0x20000000,
};

typedef cjUINT32					cjSerID;
typedef cjINT32						cjSerOrdinal;

enum { cjSerID_Invalid = 1 }; // 0x00000001, for compress

/*
	cjSerID

	 [12bit]     [16bit]     [4bit=0xf]
	 reserved    ordinal      flag 
	                          0x1 = cjSerID_Invalid
							  0x2 = Extern
							  0x4 = convert to count value
							  , 0x8 = reserved
*/

#define cjSerID_Serialize(ser, serID)    ser << cjFStreamCmprI32(serID)

#define cjSerID_MakeCount(_CountToSerID)	(((cjSerID)_CountToSerID << 4) | 0x4)
#define cjSerID_GetCount(_SerIDToCount)		(((cjSerID)_SerIDToCount >> 4))

#define cjSerID_IsCount(serID)	(((cjSerID)serID & 0x4) ? 1 : 0)


inline cjBool cjSerID_IsExtern(cjSerID serID)
{ 
	return serID & 0x00000002 ? cjTrue : cjFalse; 
}

inline void cjSerID_SetExtern(cjSerID& serID, cjBool ext)
{ 
	if(ext)
		serID |= 0x00000002;
	else
		serID &= ~0x00000002;
}

inline cjSerOrdinal cjSerID_GetOrdinal(cjSerID serID)
{
	return (cjSerOrdinal)((serID >> 4) & 0xffff);
}

inline cjSerOrdinal cjSerID_MakeOrdinal(cjSerOrdinal ord)
{
	return (cjSerOrdinal)((ord & 0xffff) << 4);
}


//
// cjSerializer
//

#define CJSERIALIZER_REV_MAJOR				1
#define CJSERIALIZER_REV_MINOR				1
#define CJSERIALIZER_REV_MAKE(ma,mi)		(((ma & 0xff) << 8) | (mi & 0xff))


class CJCORE_API cjSerializer : public cjFStream
{
private:
	virtual cjFStream& operator<< (cjObj*& /*obj*/) { return *this; };

public:
	virtual cjFStream& operator<< (cjName& /*name*/) { return *this; }
	virtual cjStreamOffset SerializeStream(void* /*v*/, cjStreamOffset /*siz*/) { return 0; }
	virtual cjBool IsPureFStream() { return cjFalse; }

public:
	struct DeferObjSetDat
	{
		cjSerID						m_serID;		
		cjSerializable*				m_dstObj;		
	};


	cjSerializer();
	virtual ~cjSerializer();

	virtual void Reset();

public:	
	cjBool IsQuerySerObjAndStr()	{ return m_streamFlag & ESF_QUERY_CJSEROBJANDSTR ? cjTrue : cjFalse; }
	void SetQuerySerObjAndStr(cjBool queryObjAndStr) { SetStreamFlag(ESF_QUERY_CJSEROBJANDSTR, queryObjAndStr); }
	
		// collecting
	cjAryOff AddString(const cjUni* str);
	cjAryOff AddString(const cjChar* str);

	virtual cjAryOff AddCJObj(cjObj* obj);
	virtual cjAryOff AddSerObj(cjSerializable* serObj);
	cjAryOff FindQuerySerObj(cjSerializable* serObj);

	void PostQuery();

		//
		// can use after calling PostQuery()
		//

	//// Save & Load Ser Obj
	////cjdrprecate
	//void SaveSerObj(cjSerializable* serObj);

	////cjdrprecate
	//template<class T>
	//T* LoadSerObj()
	//{
	//	cjSerID serID;
	//	cjSerID_Serialize(*this, serID);

	//	if(serID == cjSerID_Invalid)
	//		return 0;

	//	cjSerializable* obj = GetSerObj(serID);

	//	if(!obj)
	//		return 0;

	//	if(obj->cjsIsKindOf<T>())
	//		return (T*)obj;

	//	return 0;
	//}

	template<class T>
	cjSerID SerializeObjNoPost(T*& d)
	{
		cjSerID serID;

		if(IsSaving())
			serID = GetSerObjID(d);

		(*this) << serID;

		if(IsLoading())
			d = GetSerObj<T>(serID);

		return serID;
	}
	
	template<class T>
	cjSerID SerializeObj(T*& serObj)
	{
		cjSerID serID;

		if(IsLoading())
		{
			cjSerID_Serialize(*this, serID);
			
			if(!EnqueuePostSerializeSerID(serID))
				serID = cjSerID_Invalid;

			return serID;
		}
		else if(IsSaving())
		{
			serID = GetSerObjID(serObj);
			cjSerID_Serialize(*this, serID);

			return serID;
		}

		return cjSerID_Invalid;
	}

	cjSerID SerializeObjForPaddingOnLoad()
	{
		cjSerID serID;

		if(IsLoading())
		{
			cjSerID_Serialize(*this, serID);
			
			if(!EnqueuePostSerializeSerID(serID))
				serID = cjSerID_Invalid;

			return serID;
		}

		return cjSerID_Invalid;
	}
	
	template<class TPtr>
	cjSerID SerializeSerObjPtr(TPtr* serObjPtr=cjNULL)
	{
		cjSerID serID;

		if(IsLoading())
		{
			cjSerID_Serialize(*this, serID);
			
			if(!EnqueuePostSerializeSerID(serID))
				serID = cjSerID_Invalid;

			return serID;
		}
		else if(IsSaving())
		{
			serID = GetSerObjID((*serObjPtr));
			cjSerID_Serialize(*this, serID);

			return serID;
		}

		return cjSerID_Invalid;
	}
	
	template<class T>
	cjSerID PostSerializeSerObj(T*& serObj)
	{
		if(IsLoading())
		{
			cjSerID serID;			
			PopPostSerializeSerID(serID);

			cjAssert(!cjSerID_IsCount(serID));

			serObj = GetSerObj<T>(serID);

			return serID;
		}

		return cjSerID_Invalid;
	}
	
	template<class TPtr>
	cjSerID PostSerializeSerObjPtr(TPtr& serObjPtr)
	{
		if(IsLoading())
		{
			cjSerID serID;
			PopPostSerializeSerID(serID);

			cjAssert(!cjSerID_IsCount(serID));
			
			serObjPtr = GetSerObj<typename TPtr::Type>(serID);

			return serID;
		}

		return cjSerID_Invalid;
	}

	cjSerID GetCJObjID(cjObj* obj);
	cjSerID GetSerObjID(cjSerializable* serObj);

	// if verifyCls==0, just pass instance in m_serCJObj;
	cjObj* GetCJObj(cjSerID objID, cjObjCls* verifyCls);

	template<class T>
	T* GetCJObj(cjSerID serID)
	{
		if(serID == cjSerID_Invalid)
			return 0;

		cjObj* obj = GetCJObj(serID, T::cjObjGetStaticCls());

		if(!obj)
			return 0;

		return (T*)obj;
	}

	cjSerializable* GetSerObj(cjSerID serObjID);

	template<class T>
	T* GetSerObj(cjSerID serID)
	{
		if(serID == cjSerID_Invalid)
			return 0;

		cjSerializable* obj = GetSerObj(serID);

        //cjdefer
		//if(obj->cjsIsKindOf<T>())
		//	return (T*)obj;

		// return 0;

		return (T*)obj;
	}

	template< class T >
	cjSerID SerializeCJObjRef(T& dst)
	{
		cjSerID id;

		if(IsSaving())
		{
			id = GetCJObjID(dst);			
		}

		(*this) << id;

		if(IsLoading())
			dst = GetCJObj<typename T::Type >(id);

		return id;
	}

	cjNode* GetSerObjNode(cjSerID serID);

	static cjBool CheckExternSerID(cjSerID id) { return id & 0x80000000 ? cjTrue : cjFalse; }
	static void* RecycleMAlloc(cjSiz_t siz, void* owner);
	static void RecycleMFree(void* m, void* owner);
	void FlushFreeMem();

	// caution! - this obj is not resetted.. need to call ResetPreloadedCJObj() manually..
	void AddPreLoadedCJObj(cjObj* obj);
	void ResetPreloadedCJObj();


	cjBool EnqueuePostSerializeObjCount(cjAryOff count)
	{
		//cjtemp
		//cjMLOG("[ENCOUNT] ");

		return EnqueuePostSerializeSerID(cjSerID_MakeCount(count));
	}

	cjBool PopPostSerializeObjCount(cjAryOff& count)
	{
		cjSerID serID;

		//cjtemp
		//cjMLOG("[POCOUNT] ");

		if(!PopPostSerializeSerID(serID))
			return cjFalse;

		if(!cjSerID_IsCount(serID))
		{
			cjAssert(!"invalid serID, not a enqueued count..");
			return cjFalse;
		}

		count = cjSerID_GetCount(serID);

		return cjTrue;
	}

protected:
	void WriteHeader();

	void ReadHeader();
	void PostReadHeader();

	cjSerID PopPreloadSerObj();
	cjBool EnqueuePostSerializeSerID(cjSerID serID);
	cjBool PopPostSerializeSerID(cjSerID& serID);		
					
protected:
	cjTFLAG							m_flag;	

	//
	// On Querying (PreSaving)
	//
		// collect by SetQueryObj(cjTrue);
	cjTAry<cjObj*>					m_collectedCJObj;
	cjTAry<cjSerializable*>			m_collectedSerObj;
	cjTAry<const cjRTTI*>			m_collectedSerObjRtti;
	cjTAry<cjString>				m_collectedStr;

		// generated by calling PostQuery() on PreSaving / ReadHeader()
	cjTAry<cjObj*>					m_serCJObj;
	
	cjTAry<cjSerializable*>			m_serSerObj;
	cjTAry<cjINT32>					m_serSerObjHitNum;
	cjTAry<const cjChar*>			m_serSerObjRttiStr;
	cjTAry<cjAryOff>				m_serSerObjClsStrIdx;
	cjTAry<const cjRTTI*>			m_serRtti;

	cjTAry<cjString>				m_serStr;
	cjTAry<cjAryOff>				m_serStrIndex;		// index for m_serStr
	
	//
	// On Loading
	//
	cjTAry<DeferObjSetDat>			m_deferSetObjs;
	cjAryOff						m_deferSetObjIndex;
	cjAryOff						m_deferSetObjPopIndex;

		// load
	// objName size is cjNameSiz = 64, so cjString is need to store more long size name
	cjTHash<cjString, cjObj*>		m_cjObjPreLoaded;
	
	const static cjUINT32		NUM_MAX_RECYLE_MEM = 100;
	void*							m_freeMem[NUM_MAX_RECYLE_MEM];
	cjSiz_t							m_freeMemSiz[NUM_MAX_RECYLE_MEM];
	cjINT32							m_numFreeMem;	
	cjUINT32						m_lastFreeMem;
};


#define CJ_DBG_SERIALIZE_BEGIN(_stream, _func, _line, _fname) \
		cjWORD __lastDBGSerial = 0; \
		cjWORD __curDBGSerial = 0; \
		cjStreamOffset __curDBGOffset = 0; \
		cjStreamOffset __lastDBGEndOffset = 0; \
		if(_stream.IsLoading()) { \
			_stream << __curDBGSerial; \
			__curDBGOffset = _stream.GetDebugCurContentPos(__curDBGSerial); \
			if(__curDBGOffset) __curDBGOffset -= sizeof(__curDBGSerial); \
		} \
		if(_stream.IsSaving()) { \
			_stream.ValidateStreamDBGInfo(); \
			const cjNameChar* __dbgSerName = 0; \
			if(GetInstAsRefObj()) __dbgSerName = ((cjRefObj*)this)->GetName();\
			__lastDBGSerial = _stream.BeginDBGSerInfo(&__curDBGSerial, cjrStaticRTTI()->cjrGetNameW(), __dbgSerName, _func, _line, _fname); \
			__curDBGOffset = _stream.GetDebugCurContentPos(__curDBGSerial); \
			_stream << __curDBGSerial; \
		} \

#define CJ_DBG_SERIALIZE_END(_stream, _func, _line, _fname) \
		if(_stream.IsLoading()) { \
			_stream << __lastDBGSerial; \
			__lastDBGEndOffset = _stream.GetDebugCurContentPos(__lastDBGSerial); \
			if(__lastDBGEndOffset) __lastDBGEndOffset -= sizeof(__lastDBGSerial); \
			_stream.m_dbgLastDBGSerial = __lastDBGSerial; \
			_stream.m_dbgLastBeginOffset = __curDBGOffset; \
			_stream.m_dbgLastEndOffset = __lastDBGEndOffset; \
		} \
		if(_stream.IsSaving()) { \
			_stream.EndDBGSerInfo(_func, _line, _fname, __lastDBGSerial); \
			_stream.Flush(); \
		} \


//
// cjSerializable
//


enum cjSerCallConstructorTag { cjSerEInternalConstructor };

/*
	CLS_NAME* cjrCast(const cjRTTI* rtti) const \
		{ return (cjrIsKindOf(rtti) ? (CLS_NAME*)this : 0); } \
    static CLS_NAME* cjrCast(const cjRTTI* rtti, const CLS_NAME* dst) \
		{ return dst ? dst->cjrCast(rtti) : 0; } \
*/

	//
	// Base Declaration
	//

//#define CJSER_DECL_CLONE \
//	virtual cjSerializable* Clone(void* c=0) const { \
//		if(!IsCloneable()) return cjNULL; \
//		CLS_NAME* cloneInst = (CLS_NAME*)cjrRTTI()->NewInst(); \
//		cjSerObjCloner cloner(this); \
//		cloner << cloneInst; \
//		return cloneInst; } \
//	friend API_PKG cjSerObjCloner& operator <<(cjSerObjCloner& cloner, CLS_NAME* d); 


#define CJSER_DECL_BASE(CLS_NAME, CLS_FLAG) \
public: \
	enum { CJSER_CLS_FLAG = CLS_FLAG }; \
	static const cjRTTI* cjrStaticRTTI() {return &_ms_cjrRTTI;} \
    virtual const cjRTTI* cjrRTTI() const {return &_ms_cjrRTTI;} \
	static void* cjGetStaticCls(cjUINT32* flag=cjNULL) { if(flag) { *flag=1; } return (void*)&_ms_cjrRTTI; }


#define CJSER_DECL_CLASS(CLS_NAME, CLSBASE_NAME, CLS_FLAG, API_PKG) \
	CJSER_DECL_BASE(CLS_NAME, CLS_FLAG) \
	static cjRTTI								_ms_cjrRTTI; \
	static CLS_NAME* NewInst() { return (CLS_NAME*)CLS_NAME::_ms_cjrRTTI.NewInst(); } \
	virtual void DelInst() { CLS_NAME::_ms_cjrRTTI.DelInst(this); } \
	static void* NewInst_() { return CLS_NAME::NewInst(); } \
	static void DelInst_(void* m) { ((CLS_NAME*)m)->DelInst(); } \
	static void* PNew(void* m) { return new((cjSerCallConstructorTag*)m) CLS_NAME(); } \
	static void _PDel(void* m) { if(m) ((cjSerializable*)m)->PDel(); } \
	virtual void PDel() { CLS_NAME::~CLS_NAME(); } \
	typedef	CLSBASE_NAME cjSuper; \
protected: \
	virtual ~CLS_NAME(); \
public: \
	static void* operator new(size_t , cjSerCallConstructorTag* m) throw() \
		{ return (void*)m; } \
	static void* operator new(size_t ) \
		{ return CLS_NAME::_ms_cjrRTTI.Alloc(); }


#define CJSER_DECL_CLASS_X(CLS_NAME, CLSBASE_NAME, CLS_FLAG, API_PKG) \
	CJSER_DECL_BASE(CLS_NAME, CLS_FLAG) \
	static cjRTTI								_ms_cjrRTTI; \
	static CLS_NAME* NewInst() { return (CLS_NAME*)CLS_NAME::_ms_cjrRTTI.NewInst(); } \
	virtual void DelInst() { CLS_NAME::_ms_cjrRTTI.DelInst(this); } \
	static void* NewInst_() { return CLS_NAME::NewInst(); } \
	static void DelInst_(void* m) { ((CLS_NAME*)m)->DelInst(); } \
	static void* PNew(void* m) { return new((cjSerCallConstructorTag*)m) CLS_NAME(); } \
	static void _PDel(void* m) { if(m) ((cjSerializable*)m)->PDel(); } \
	virtual void PDel() { CLS_NAME::~CLS_NAME(); } \
	typedef	CLSBASE_NAME cjSuper; \
	virtual ~CLS_NAME(); \
public: \
	static void* operator new(size_t , cjSerCallConstructorTag* m) throw() \
		{ return (void*)m; } \
	static void* operator new(size_t ) \
		{ return CLS_NAME::_ms_cjrRTTI.Alloc(); }


#define CJSER_DECL_CLASS_ABSTRACT(CLS_NAME, CLSBASE_NAME, CLS_FLAG, API_PKG) \
	CJSER_DECL_BASE(CLS_NAME, CLS_FLAG | CJRTTI_CF_ABSTRACT) \
	static cjRTTI								_ms_cjrRTTI; \
	static CLS_NAME* NewInst() { return 0; } \
	virtual void DelInst() { CLS_NAME::_ms_cjrRTTI.DelInst(this); } \
	static void* NewInst_() { return 0; } \
	static void DelInst_(void* m) { ((CLS_NAME*)m)->DelInst(); } \
	static void* PNew(void* ) { return 0; } \
	static void _PDel(void* m) { if(m) ((cjSerializable*)m)->PDel(); } \
	virtual void PDel() { CLS_NAME::~CLS_NAME(); } \
	typedef	CLSBASE_NAME cjSuper; \
protected: \
	CLS_NAME(); \
	virtual ~CLS_NAME(); \
private: \
	static void* operator new(size_t ) throw() { return 0; } \
	static void* operator new(size_t , cjSerCallConstructorTag* ) throw() { return 0; }
	
// operator delete is not allowed 
//   - calling virtual function on upper destructor
// operator delete [] is not allowed 
//   - (ref count object, calling each destructor, calling virtual function on upper destructor)

#define CJSER_IMPL_CLASS_ROOT(CLS_NAME) \
	cjRTTI CLS_NAME::_ms_cjrRTTI(sizeof(CLS_NAME), #CLS_NAME, _CU(#CLS_NAME), \
		cjNULL, CLS_NAME::CJSER_CLS_FLAG, \
		(cjRTTI::PFNew)&CLS_NAME::NewInst, (cjRTTI::PFDel)&CLS_NAME::DelInst_, \
		(cjRTTI::PFPNew)&CLS_NAME::PNew, (cjRTTI::PFPDel)&CLS_NAME::_PDel);

#define CJSER_IMPL_CLASS(CLS_NAME, CLSBASE_NAME) \
	cjRTTI CLS_NAME::_ms_cjrRTTI(sizeof(CLS_NAME), #CLS_NAME, _CU(#CLS_NAME), \
		CLSBASE_NAME::cjrStaticRTTI(), CLS_NAME::CJSER_CLS_FLAG, \
		(cjRTTI::PFNew)&CLS_NAME::NewInst, (cjRTTI::PFDel)&CLS_NAME::DelInst_, \
		(cjRTTI::PFPNew)&CLS_NAME::PNew, (cjRTTI::PFPDel)&CLS_NAME::_PDel);


	//
	// Serialize
	//

#define CJSER_DECL_SERIALIZE \
	public: \
		virtual void operator >> (cjSerializer& ser);

#define CJSER_IMPL_SERIALIZE_BEGIN(CLSNAME) \
	void CLSNAME::operator >> (cjSerializer& ser) \
	{ \
		CJ_DBG_SERIALIZE_BEGIN(ser, __FUNCTION__, __LINE__, __FILE__) \
		cjSuper::operator >> (ser);

#define CJSER_IMPL_SERIALIZE_NOSUPER_BEGIN(CLSNAME) \
	void CLSNAME::operator >> (cjSerializer& ser) \
	{ \
		CJ_DBG_SERIALIZE_BEGIN(ser, __FUNCTION__, __LINE__, __FILE__) \


#define CJSER_IMPL_SERIALIZE_END(CLSNAME) \
		CJ_DBG_SERIALIZE_END(ser, __FUNCTION__, __LINE__, __FILE__) \
	}


#define CJSER_DECL_SERIALIZE_POST \
	public: \
		virtual void PostSerialize(cjSerializer& ser);

#define CJSER_IMPL_SERIALIZE_POST_BEGIN(CLSNAME) \
	void CLSNAME::PostSerialize(cjSerializer& ser) \
	{ \
		cjSuper::PostSerialize(ser); \
	

#define CJSER_IMPL_SERIALIZE_POST_END(CLSNAME) \
	} 


//#define CJSER_DECL_SERIALIZE_REV(rev) \
//	public: static const cjBYTE SER_REV=rev; \
//		virtual void operator >> (cjSerializer& ser);
//
//#define CJSER_IMPL_SERIALIZE_REV_BEGIN(CLSNAME) \
//	void CLSNAME::operator >> (cjSerializer& ser) \
//	{ \
//		CJ_DBG_SERIALIZE_BEGIN(ser, __FUNCTION__, __LINE__, __FILE__) \
//		cjBYTE rev = SER_REV; \
//		ser << rev; \
//		cjSuper::operator >> (ser);
//
//#define CJSER_IMPL_SERIALIZE_REV_NOSUPER_BEGIN(CLSNAME) \
//	void CLSNAME::operator >> (cjSerializer& ser) \
//	{ \
//		CJ_DBG_SERIALIZE_BEGIN(ser, __FUNCTION__, __LINE__, __FILE__) \
//		cjBYTE rev = SER_REV; \
//		ser << rev;
//
//#define CJSER_IMPL_SERIALIZE_REV_END(CLSNAME) \
//		CJ_DBG_SERIALIZE_END(ser, __FUNCTION__, __LINE__, __FILE__) \
//	}

#define CJSER_IMPL_CALL_SERIALIZE_SUPER			cjSuper::operator >> (ser);

#define CJSER_DECL_SERIALIZE_ARY(CLSNAME) static void SerializeAry(cjSerializer& ser, CLSNAME*** d, cjINT32* count);
#define CJSER_IMPL_SERIALIZE_ARY(CLSNAME) void CLSNAME::SerializeAry(cjSerializer& ser, CLSNAME*** d, cjINT32* count) \
{ \
	if(ser.IsQuerySerObjAndStr()) \
	{ \
		for(cjINT32 i=0; i< *count; i++) \
		{ \
			CLSNAME* cur = (*d)[i]; \
			*cur >> ser; \
		} \
	} \
	else if(ser.IsLoading() || ser.IsSaving()) \
	{ \
		cjSerID serID; \
		cjINT32 datCount = count ? *count : 0; \
		\
		ser << cjFStreamCmprI32(datCount); \
		\
		if(ser.IsLoading()) \
		{ \
			cjAssert(*d == 0); \
			\
			if(datCount) \
			{ \
				*d = new CLSNAME*[datCount]; \
				for(cjINT32 i=0; i<datCount; i++) \
				{ \
					cjSerID_Serialize(ser, serID); \
					(*d)[i] = ser.GetSerObj<CLSNAME>(serID); \
				} \
			} \
			\
			if(count) \
				*count = datCount; \
		} \
		else if(ser.IsSaving()) \
		{ \
			for(cjINT32 i=0; i<datCount; i++) \
			{ \
				serID = ser.GetSerObjID((*d)[i]); \
				cjSerID_Serialize(ser, serID); \
			} \
		} \
	} \
}


#define CJSER_DECL_SERIALIZE_PTR_ARY(CLSNAME) static void SerializePtrAry(cjSerializer& ser, cjTRefPtr<CLSNAME>** d, cjINT32* count);
#define CJSER_IMPL_SERIALIZE_PTR_ARY(CLSNAME) void CLSNAME::SerializePtrAry(cjSerializer& ser, cjTRefPtr<CLSNAME>** d, cjINT32* count) \
{ \
	if(ser.IsQuerySerObjAndStr()) \
	{ \
		for(cjINT32 i=0; i< *count; i++) \
		{ \
			CLSNAME* cur = (*d)[i]; \
			if(cur) \
				*cur >> ser; \
		} \
	} \
	else if(ser.IsLoading() || ser.IsSaving()) \
	{ \
		cjSerID serID; \
		cjINT32 datCount = count ? *count : 0; \
		\
		ser << cjFStreamCmprI32(datCount); \
		\
		if(ser.IsLoading()) \
		{ \
			cjAssert(*d == 0); \
			\
			if(datCount) \
			{ \
				*d = new cjTRefPtr<CLSNAME>[datCount]; \
				for(cjINT32 i=0; i<datCount; i++) \
				{ \
					cjSerID_Serialize(ser, serID); \
					(*d)[i] = ser.GetSerObj<CLSNAME>(serID); \
				} \
			} \
			\
			if(count) \
				*count = datCount; \
		} \
		else if(ser.IsSaving()) \
		{ \
			for(cjINT32 i=0; i<datCount; i++) \
			{ \
				serID = ser.GetSerObjID((*d)[i]); \
				cjSerID_Serialize(ser, serID); \
			} \
		} \
	} \
}


class cjSerObjCloner;
class cjSerCloneParam;
class cjRefObj;

class CJCORE_API cjSerializable // do not use : public cjMem
{
	friend class cjSerializer;
	CJSER_DECL_CLASS_ABSTRACT(cjSerializable, cjSerializable, 0, CJCORE_API)
	
public:
	static void operator delete(void* ) { cjFatal(0); }
	static void operator delete[](void* ) { cjFatal(0); }

public: // rtti support
	enum 
	{
		CLONEPROC_CLONE  = 0,
		CLONEPROC_POST_CLONE = 1,
	};
	
	template<class T>
	CJ_FORCE_INLINE cjBool cjsIsKindOf() const 
	{ 
		const cjRTTI* curRTTI = cjrRTTI();     
		while(curRTTI) {
			if(curRTTI == T::cjrStaticRTTI())
				return cjTrue;
			curRTTI = curRTTI->cjrGetBase();
		}
		return cjFalse;
	}

    CJ_FORCE_INLINE cjBool cjsIsKindOf(const cjRTTI* rtti) const 
	{ 
		const cjRTTI* curRTTI = cjrRTTI();
		while(curRTTI) {
            if(curRTTI == rtti) 
				return cjTrue;
            curRTTI = curRTTI->cjrGetBase();
        }
        return cjFalse;
    }

	template<class T>
	CJ_FORCE_INLINE T* cjsCast() const
	{
		if(cjsIsKindOf<T>())
			return (T*)this;

		return cjNULL;
	}

	cjBool cjsIsSameOf(const cjRTTI* rtti) const 
		{ return (cjrRTTI() == rtti); }
			
	//static cjBool cjsIsSameOf(const cjRTTI* rtti, const cjSerializable* dst)
	//	{ return dst ? dst->cjsIsSameOf(rtti) : cjFalse; }
 //   static cjBool cjsIsKindOf(const cjRTTI* rtti, const cjSerializable* dst)
	//	{ return dst ? dst->cjsIsKindOf(rtti) : cjFalse; }

	virtual cjBool IsCloneable() const { return cjTrue; }

	virtual cjSerializable* Clone() { return cjNULL; }
	virtual cjSerializable* Clone(cjSerObjCloner* /*cloner*/) { return cjNULL; }

	virtual cjSerializable* _Clone(cjSerObjCloner* ) { return cjNULL; }
	virtual void _PostClone(cjSerObjCloner* ) {}
	void _CloneProperty(cjSerializable* d, cjSerObjCloner* /*cloner*/) { d->_m_serFlag = _m_serFlag; }
		
public:
	static cjSerID GenerateSerObjID(cjSerializable* obj, cjSerOrdinal ord);
	static cjSerID GenerateCJObjID(cjObj* obj, cjSerOrdinal ord);

	CJ_FORCE_INLINE cjTFLAG cjSerGetFlag() const { return _m_serFlag; }
	CJ_FORCE_INLINE void cjSerSetFlag(cjTFLAG flag) { _m_serFlag |= flag; }
	CJ_FORCE_INLINE void cjSerClearFlag(cjTFLAG flag) { _m_serFlag &= ~flag; }

	//cjBool cjSerFlag_IsRef() { return _m_serFlag & CJSER_FL_REF ? cjTrue : cjFalse; }
	//cjRef* cjGetAsRefInst() { return _m_serFlag & CJSER_FL_REF ? (cjRef*)this : 0; }

	virtual cjRef* GetInstAScjRef() { return cjNULL; }
	virtual cjRefObj* GetInstAsRefObj() { return cjNULL; }

	virtual void operator >>(cjSerializer& ser);

protected:
	virtual void PostSerialize(cjSerializer& /*ser*/) {}

private:
	cjSerializable(const cjSerializable& /*rhs*/) { cjAssert(0); }
	cjSerializable& operator=(const cjSerializable& /*rhs*/) { cjAssert(0); return *this; }
		
public:
	cjTFLAG									_m_serFlag; // keep 4byte alignment
};

CJ_FORCE_INLINE cjSerializable::cjSerializable() : _m_serFlag(0) {}	



template<class TSerCls >
struct cjTHashSerRemover
{
	void operator()(TSerCls* removeItem) 
	{ 
		if(removeItem)
		{
			removeItem->DelInst();
			removeItem = 0;
		}
	}

	void operator()(TSerCls& removeItem) 
	{ 
		if(removeItem)
		{
			removeItem->DelInst();
			removeItem = 0;
		}
	}
};


//
// cjNode Template utility class
//

template<class T>
inline cjBool cjrIsSameOf(const cjSerializable* ser)
{
	return T::cjsIsSameOf(ser);
}

template<class T>
inline cjBool cjrIsKindOf(const cjSerializable* ser)
{
	if(!ser) 
		return cjFalse;

	return ser->cjsIsKindOf(T::cjrStaticRTTI());
}

template<class T>
inline T* cjrCast(const cjSerializable* ser)
{
	return ser ? ser->cjsCast<T>() : cjNULL;
}

#define CJSER_SAFE_DELETE(x) { if(x) x->DelInst(); x = cjNULL; }
#define CJSER_SAFE_RELEASE(x) { if(x) x->ReleaseRef(); x = cjNULL; }


class cjRefObj;


//
// cjSerCloner
//

//class CJCORE_API cjSerCloner : public cjFStream
//{
//private:
//	virtual cjFStream& operator<< (cjObj*& obj) { return *this; };
//
//public:
//	virtual cjFStream& operator<< (cjName& name) { return *this; }
//	virtual cjStreamOffset SerializeStream(void* v, cjStreamOffset siz) { return 0; }
//	virtual cjBool IsPureFStream() { return cjFalse; }
//
//public:
//	cjSerCloner(const cjSerializable* src);
//};


//
// cjSerObjCloner
//

class cjSerCloneParam
{
public:
	cjSerCloneParam(const cjSerializable* caller=cjNULL) 
	{
		m_caller = caller;
		m_prm0 = 0;
	}

	const cjSerializable*		m_caller;
	void*						m_prm0;
};

class CJCORE_API cjSerObjCloner
{
public:
	enum { DefaultHashBucketSiz = 30 };

	cjSerObjCloner(cjPtrHasherValType bucketSiz=DefaultHashBucketSiz);
	~cjSerObjCloner();

	cjSerializable*					GetCloned(cjSerializable* serObj);
	cjRef*							GetClonedAsRef(cjSerializable* serObj);
	cjRefObj*						GetClonedAsRefObj(cjSerializable* serObj);
	void							SetCloned(cjSerializable* src, cjSerializable* dst);

	cjBool							IsPostCloned(cjSerializable* serObj);
	void							SetPostCloned(cjSerializable* serObj);

	// cjRefObj::Clone() -> cjRefObj::_Clone() -> _CloneProperty() -> cjRefObj::_CloneProperty -> m_clones->Set
	cjTPtrHash<cjSerializable*, cjSerializable*>*		m_clones;

	// cjRefObj::Clone() -> cjRefObj::_PostClone()-> cjRefObj::m_postCloned->Set
	cjTPtrHash<cjSerializable*, cjBool>*				m_postCloned;

	const static cjBool				Cloned;
};


#define CJSER_NO_CLONEABLE \
protected: \
	virtual cjBool IsCloneable() const \
	{ \
		cjAssert(!"not cloneable!"); \
		return cjFalse; \
	}


#define CJSER_DECL_CLONE(CLS_NAME) \
public: \
	virtual cjSerializable* _Clone(cjSerObjCloner* cloner); \
	void _CloneProperty(CLS_NAME* d, cjSerObjCloner* cloner);

#define CJSER_DECL_CLONE_ABSTRACT(CLS_NAME) \
public: \
	void _CloneProperty(CLS_NAME* d, cjSerObjCloner* cloner); 

#define CJSER_DECL_CLONE_POST \
public: \
	void _PostClone(cjSerObjCloner* cloner); 


#define CJSER_DECL_CLONE_MANUAL(CLS_NAME) \
	virtual cjSerializable* Clone(cjSerObjCloner* cloner=0);


//#define CJSER_DECL_CLONE_WITH_POST(CLS_NAME) \
//public: \
//	virtual cjRefObj* _Clone(cjSerObjCloner* cloner); \
//	void _CloneProperty(CLS_NAME* d, cjSerObjCloner* cloner); \
//	void _PostClone(cjSerObjCloner* cloner); 
//
//#define CJSER_DECL_CLONE_WITH_POST_ROUTE(CLS_NAME) \
//	virtual cjRefObj* _Clone(cjSerObjCloner* cloner); \
//	void _CloneProperty(CLS_NAME* d, cjSerObjCloner* cloner); \
//	void _PostClone(cjSerObjCloner* cloner) { cjSuper::_PostClone(cloner); } 
//
//
//#define CJSER_DECL_CLONE_ABSTRACT_WITH_POST(CLS_NAME) \
//public: \
//	void _CloneProperty(CLS_NAME* d, cjSerObjCloner* cloner); \
//	void _PostClone(cjSerObjCloner* cloner); 

// #define CJSER_IMPL_CLONE_ABSTRACT(CLS_NAME) 

#define CJSER_IMPL_CLONE_PROPCOPY_BEGIN(CLS_NAME) \
void CLS_NAME::_CloneProperty(CLS_NAME* d, cjSerObjCloner* cloner) \
{ \
	cjSuper::_CloneProperty(d, cloner); 

#define CJSER_IMPL_CLONE_PROPCOPY_END(CLS_NAME) \
} 

#define CJSER_IMPL_CLONE(CLS_NAME) \
cjSerializable* CLS_NAME::_Clone(cjSerObjCloner* cloner) \
{ \
	CLS_NAME* _inst = new CLS_NAME; \
	_CloneProperty(_inst, cloner); \
	return _inst; \
}

#define CJSER_IMPL_CLONE_POST_BEGIN(CLS_NAME) \
void CLS_NAME::_PostClone(cjSerObjCloner* cloner) \
{ \
	cjSuper::_PostClone(cloner);

#define CJSER_IMPL_CLONE_POST_BEGIN_EX(CLS_NAME) \
void CLS_NAME::_PostClone(cjSerObjCloner* cloner) \
{ \
	cjSuper::_PostClone(cloner); \
	CLS_NAME* d = (CLS_NAME*)cloner->GetClonedInstance(this); 

#define CJSER_IMPL_CLONE_POST_END(CLS_NAME) \
}

//#define CJSER_IMPL_CLONE_MANUAL_BEGIN(CLS_NAME) \
//cjSerializable* CLS_NAME::Clone(cjSerObjCloner* cloner) \
//{ \
//	CLS_NAME* d = new CLS_NAME; \
//	CLS_NAME& s = *(this); 
//
//#define CJSER_IMPL_CLONE_MANUAL_END(CLS_NAME) \
//	return d; \
//}


#endif
