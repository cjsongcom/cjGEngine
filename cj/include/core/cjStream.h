
#ifndef _CJSTREAM_H_
#define _CJSTREAM_H_


//
// File IO
//
enum cjFilePathHint
{
	cjFilePathHint_Base				= 0,	// path in gamebase
	cjFilePathHint_Pak				= 1,	// path in resource pak

	cjFilePathHint_Max,

	cjFilePathHint_Default			= cjFilePathHint_Base,
	cjFilePathHint_Absolute			= -1,
	cjFilePathHint_PassThru			= -2,
};


class cjFile;
class cjFStream;
class cjFDeferLoad;
class cjSerObjStreamBroker;
class cjStringW;

//
// Compressed Numeric Value Serializer
//

struct CJCORE_API _cjFStreamCompressI32
{
public:
	cjINT32						m_v;
	CJCORE_API friend cjFStream& operator <<(cjFStream& stream, _cjFStreamCompressI32& i32);
};

#define cjFStreamCmprI32(x) (*(_cjFStreamCompressI32*)&(x))




//struct CJCORE_API _cjFStreamCompressUI32
//{
//public:
//	cjUINT32						m_v;
//	CJCORE_API friend cjFStream& operator <<(cjFStream& stream, _cjFStreamCompressUI32& ui32);
//};
//
//#define cjFStreamCmprUI32(x) (*(_cjFStreamCompressUI32*)&(x))



struct CJCORE_API _cjFStreamCompressI64
{
public:
	cjINT64						m_v;
	CJCORE_API friend cjFStream& operator <<(cjFStream& stream, _cjFStreamCompressI64& i64);
};

#define cjFStreamCmprI64(x) (*(_cjFStreamCompressI64*)&(x))


//
// cjFDeferLoad
//

class cjFDeferLoad // do not use ': public cjMem', cjTAry is using cjMem
{
	friend class cjObjPkgLoad;

public:
	cjFDeferLoad()
	{
		m_deferStream = NULL;
		m_deferLoadPos = 0;
	}
    virtual ~cjFDeferLoad() { }

	virtual void ValidateLoad() = 0;
	virtual void Unload() = 0;

protected:
	cjFStream*								m_deferStream;

	// ==0 : nothing to load deferred 
	// >0  : deferred data is loaded
	// <0  : not loaded deferred data, call ValidateLoad() to change load state
	cjStreamOffset							m_deferLoadPos;	

	static cjBool							ms_enableDeferLoading;

public:
	static cjBool GetDeferLoadingState();
	static void SetDeferLoadState(cjBool enableDeferLoading);

};


//
// cjFStream
//

class cjFStreamDBGInfo;

class CJCORE_API cjFStream // do not use ': public cjMem', cjLinker is using cjFStream
{
public:
	const static cjStreamOffset			DEFAULT_CSH = INT_MAX;

	enum ERR_STATUS
	{
		ERR_OK							= 0,
		ERR_CLOSED						= -1,
		ERR_INVALID_INIT_PARAM			= -2,
		ERR_REACHED_EOF					= -3,	// file: EOF
		ERR_PARTIAL_WRITE				= -4,   // file: Disk full ?
		ERR_READ						= -5,
		ERR_CACHING						= -6,
		ERR_FLUSH						= -7,
		ERR_OPEN_FAIL					= -8,
		ERR_SIZ_EXCEED_4G				= -9,
		ERR_WRITE						= -10,
		ERR_INVALID_CALL				= -11,
		ERR_READ_OUT_OF_SIZ				= -12
	};

	enum ESTREAM_FLAG
	{
		ESF_READ						= 0x00000001,
		ESF_WRITE						= 0x00000002,

		ESF_OPENED						= 0x00000010,
		ESF_OPEN_FAIL					= 0x00000020,

		ESF_PERSIST						= 0x00000010, // keep stream handle after Close() called
			
			// cjSerializable inherited
		ESF_CJOBJONLY					= 0x10000000,
		ESF_CJSERONLY					= 0x20000000,

			// query serialize obj and string
		ESF_QUERY_CJSEROBJANDSTR		= 0x02000000,
	};

	cjFStream(cjTFLAG flags = 0);
	virtual ~cjFStream();

	virtual	cjBool Open();
	virtual void Close();

	virtual cjStreamOffset SerializeStream(void*, cjStreamOffset) { return 0; } // read/write
	virtual cjBool SetPos(cjStreamOffset ) { return cjFalse; }

	virtual cjBool PreCache(cjStreamOffset cacheSizHint = DEFAULT_CSH)
		{ cacheSizHint = cacheSizHint; return cjTrue; } // for reading
	virtual cjBool Flush() { return cjTrue; } // for writing

	virtual cjStreamOffset GetSiz() { return 0; }
	virtual cjStreamOffset GetPos() { return 0; }

	virtual cjBool IsReadonly() { return m_streamFlag & ESF_WRITE ? cjFalse : cjTrue; }
	virtual cjBool IsWriteable() { return m_streamFlag & ESF_WRITE ? cjTrue : cjFalse; }

	virtual void SetErr(ERR_STATUS errVal) 
	{ 
		m_err = errVal; 

		if(m_err != ERR_OK) 
		{
			if(m_err != ERR_CLOSED)
				cjLOG("ERR! error on streaming.. = %u ", errVal); 
		}
	}
	virtual ERR_STATUS GetErr() { return m_err; }

	virtual const cjChar* GetPath() { return NULL; }
	virtual const cjChar* GetName() { return NULL; }

	inline cjTFLAG GetStreamFlag() { return m_streamFlag; }
	inline void SetStreamFlag(cjTFLAG flag) { m_streamFlag |= flag; }
	inline void RemoveStreamFlag(cjTFLAG flag) { m_streamFlag &= ~flag; }
	inline void SetStreamFlag(cjTFLAG flag, cjBool set) 
	{ 
		if(set) { m_streamFlag |= flag; }
		else { m_streamFlag &= ~flag; }
	}

	// loading, saving, garbage collect
	cjBool IsLoading()				{ return m_streamFlag & ESF_READ ? cjTrue: cjFalse; }
	void SetLoading(cjBool loading) { SetStreamFlag(ESF_READ, loading);			}

	cjBool IsSaving()				{ return m_streamFlag & ESF_WRITE ? cjTrue : cjFalse; }
	void SetSaving(cjBool saving)	{ SetStreamFlag(ESF_WRITE, saving);			}

	cjBool IsLoadOrSaving()			{ return m_streamFlag & (ESF_READ | ESF_WRITE) ? cjTrue : cjFalse; }

	// Persist : traverse all object for GarbageCollecting
	//           mark export tag for save object to package
	//           collect object name and class info to save object to package
	virtual cjBool IsPersist()		{ return m_streamFlag & ESF_PERSIST;		}
	void SetPersist(cjBool persist) { SetStreamFlag(ESF_PERSIST, persist);		}

	cjBool IsObjOnly()				{ return m_streamFlag & ESF_CJOBJONLY ? cjTrue: cjFalse; }
	void SetObjOnly(cjBool objOnly)	{ SetStreamFlag(ESF_CJOBJONLY, objOnly);	}

	bool IsTestObjOnly()			{ return m_streamFlag & ESF_CJOBJONLY ? cjTrue: cjFalse; }

		// cjSerializable
	cjBool IsSerOnly()				{ return m_streamFlag & ESF_CJSERONLY ? cjTrue: cjFalse; }
	void SetSerOnly(cjBool serOnly)	{ SetStreamFlag(ESF_CJSERONLY, serOnly);	}

	
	// Friend operators (Primitive Types)
	CJCORE_API friend cjFStream& operator<<(cjFStream& stream, cjBYTE&						B );
	CJCORE_API friend cjFStream& operator<<(cjFStream& stream, cjChar&						B );
	CJCORE_API friend cjFStream& operator<<(cjFStream& stream, char&						B );
	CJCORE_API friend cjFStream& operator<<(cjFStream& stream, cjSBYTE&						SB);
	CJCORE_API friend cjFStream& operator<<(cjFStream& stream, short&						SW);
	CJCORE_API friend cjFStream& operator<<(cjFStream& stream, unsigned short&				W );
	CJCORE_API friend cjFStream& operator<<(cjFStream& stream, unsigned int& 				UI);
	CJCORE_API friend cjFStream& operator<<(cjFStream& stream, int&							I );
	CJCORE_API friend cjFStream& operator<<(cjFStream& stream, cjFLOAT&						F );
	CJCORE_API friend cjFStream& operator<<(cjFStream& stream, cjINT64&						SI64);
	CJCORE_API friend cjFStream& operator<<(cjFStream& stream, cjUINT64&                    UI64);
	CJCORE_API friend cjFStream& operator<<(cjFStream& stream, cjStringW&                   CJSTR);
#ifdef CJ_PLATFORM_WIN32
	CJCORE_API friend cjFStream& operator<<(cjFStream& stream, wchar_t& 					W);
#endif
	
	CJCORE_API friend cjFStream& operator<<(cjFStream& stream, void*&  						W);

	// cjName, cjObj
	virtual cjFStream& operator<<(cjName& )	{ return *this;	}
	virtual cjFStream& operator<<(cjObj*& ) { return *this; }
	virtual cjFStream& operator<<(const cjRTTI*& );
	
		// cjObj
	virtual void Preload(cjObj* ) {}

		// support cjFDeferLoad
	virtual void AddDeferLoad(cjFDeferLoad* ) {}
	virtual void RemoveDeferLoad(cjFDeferLoad* ) {}

	virtual cjBool IsPureFStream() { return cjTrue; }
	virtual cjSerObjStreamBroker* GetSerializerBroker() { return cjNULL; }

	virtual cjINT32 GetNameToMappingIndex(cjName* name) { return 0; }
	virtual cjObjIndex GetObjToMappingIndex(cjObj* obj) { return cjObjIndex_None; }

public:
	// debug stuff
	virtual cjStreamOffset GetDebugCurContentPos(cjWORD serial=0) 
	{ 
		serial=serial; 
		return -1; 
	}
	virtual cjStreamOffset GetDebugLogicalPos() { return 0; }
		
	// ret: last serial
	virtual void ValidateStreamDBGInfo();
	virtual cjWORD BeginDBGSerInfo(cjWORD* rstDbgSerial, const cjNameChar* rttiName, const cjNameChar* name, const cjChar* func, const cjUINT32 line, const cjChar* fname);
	virtual void EndDBGSerInfo(const cjChar* func, const cjUINT32 line, const cjChar* fname, cjWORD lastSerial);

	virtual void Report(const cjNameChar* filepath);


protected:
	ERR_STATUS								m_err;
	
	//cjBool								m_loading;
	//cjBool								m_saving;
	//cjBool								m_persistent;
	//cjBool								m_objOnly; // skip serialize for non cjObj
	//cjBool								m_isTransaction;
	cjINT32									m_maxSerializeSiz;

	cjTFLAG									m_streamFlag;	// streamflag

public:
	cjFStreamDBGInfo*						m_dbgInfo;
	cjWORD									m_dbgCurSerial;

	cjWORD									m_dbgLastDBGSerial;
	cjStreamOffset							m_dbgLastBeginOffset;
	cjStreamOffset							m_dbgLastEndOffset;
};


template<class T>
inline void cjFStreamEnum(cjFStream& s, T& e)
{
	cjINT32 _e = (cjINT32)e; 

	if(s.IsSaving())
		_e = (cjINT32)e;

	s << (*(_cjFStreamCompressI32*)&(_e));

	if(s.IsLoading())
		e = (T)_e;
}


void cjSerializeNameCharStr(cjFStream& stream, const cjNameChar** src, cjINT32* sizStr=0,
	cjMemAllocPF _ma=0, cjMemDeallocPF _mf=0);

void cjSerializeStrChar(cjFStream& stream, const cjChar** src, cjINT32* sizStr=0,
	cjMemAllocPF _ma=0, cjMemDeallocPF _mf=0);


template<class T>
struct cjPODSerializeProxy
{
	cjPODSerializeProxy(const T& s) : src(s), siz(sizeof(T)) {}

	friend cjFStream& operator<< (cjFStream& stream, cjPODSerializeProxy& proxy)
	{
		stream.SerializeStream((void*)&proxy.src, proxy.siz);
		return stream;
	}

	const T&		src;
	size_t			siz;
};

template<class S, class T>
inline S& cjPODSerialize(S& stream, T& d)
{
	stream.SerializeStream((void*)&d, sizeof(T));
	return stream;
}

template<class C, class S, class T>
inline S& cjPODSerializeWithType(S& stream, T& d)
{
	C v;

	if(stream.IsSaving())
		v = (C)d;

	stream.SerializeStream((void*)&v, sizeof(C));

	if(stream.IsLoading())
		d = (T)v;

	return stream;
}

template<class T>
struct cjFPODSerialProxy
{
	cjFPODSerialProxy(const T& s) : src(s), siz(sizeof(T)) {}

	friend cjFStream& operator<< (cjFStream& stream, cjFPODSerialProxy& proxy)
	{
		stream.SerializeStream((void*)&proxy.src, proxy.siz);
		return stream;
	}

	const T&		src;
	size_t			siz;
};

template<class T>
inline cjFStream& cjObjSerialize(cjFStream& stream, T*& obj)
{
	stream << ((cjObj*&)obj);
	return stream;
}

// type : [0] 32bit cjFLOAT  [1] 8bit(0~255),unit cjFLOAT(0.0~1.0),  [2] 16bit cjFLOAT
CJCORE_API void cjFloatSerHelper(cjFStream& stream, cjFLOAT& v, cjINT32 type=0);


#include "cjTString.h"

//
// cjFCacheBaseStream
//

// abstract class for cache based stream
class CJCORE_API cjFCacheBaseStream : public cjFStream
{
	typedef cjFStream cjSuper;
public:
	virtual cjBool Open();
	virtual void Close();

	virtual cjStreamOffset SerializeStream(void* v, cjStreamOffset siz);
	virtual cjBool SetPos(cjStreamOffset pos);

	virtual cjBool PreCache(cjStreamOffset cacheSizHint = DEFAULT_CSH);
	virtual cjBool Flush();

	virtual cjStreamOffset GetSiz() 
		{ return IsLoading() ? m_siz : m_cacheBasePos+m_cachedCnt; }
	virtual cjStreamOffset GetPos() { return m_curPos; }

	virtual const cjChar* GetPath() = 0;
	virtual const cjChar* GetName() = 0;

	virtual cjBool IsPureFStream() { return cjFalse; }

	// debug stuff
	virtual cjStreamOffset GetDebugLogicalPos() { return m_curPos; }
	
public:
	inline cjStreamOffset GetCacheBufSiz() { return m_cacheBufSiz; }
	inline cjStreamOffset GetCachedCnt() { return m_cachedCnt; }
	inline cjStreamOffset GetCacheBasePos() { return m_cacheBasePos; }
	inline const cjBYTE* GetCacheBuf() { return m_cacheBuf; }

protected:
	cjFCacheBaseStream() {}
	cjFCacheBaseStream(cjTFLAG fnStreamFlag);
	virtual ~cjFCacheBaseStream();

	cjBool ResetCache(cjStreamOffset readerSiz=0/*ignored when write stream*/);
	
	// derived class implementation
	virtual cjBool OnCacheSetSrcPos(cjStreamOffset cachePos) = 0;
	virtual cjBool OnCacheFlush(const void* srcBuf, cjStreamOffset cacheWritePos, 
		cjStreamOffset numWrite, cjStreamOffset* numRstWrited) = 0;
	virtual cjBool OnCacheRead(cjStreamOffset cacheReadPos, void* dstBuf, 
		cjStreamOffset numRead,	cjStreamOffset* numRstReaded) = 0;

private:
	cjStreamOffset								m_siz;
	cjStreamOffset								m_curPos;

	// current cache position at file (relative with m_basePos)
	cjBYTE*										m_cacheBuf;
	cjStreamOffset								m_cacheBasePos;		// relative with m_basePos 
	cjStreamOffset								m_cachedCnt;		// Currently cached data offset on buffer
	cjStreamOffset								m_cacheBufSiz;

	static const cjINT32						CACHESIZ_READ	= 1024;
	static const cjINT32						CACHESIZ_WRITE	= 2048;
};


//
// cjFileStream
//

class CJCORE_API cjFileStream : public cjFCacheBaseStream
{
	typedef cjFCacheBaseStream cjSuper;
public:
	virtual cjBool Open();
	virtual void Close();

	virtual cjStreamOffset SerializeStream(void* v, cjStreamOffset siz);
	virtual cjBool SetPos(cjStreamOffset pos);

	virtual const cjChar* GetPath();
	virtual const cjChar* GetName();

	virtual cjStreamOffset GetDebugCurContentPos(cjWORD serial=0);

protected:
	virtual void InitStuff();

	// int32 = ~ +2,147,483,647(hex 0x7FFFFFFF) = 2.1gb

	// implements cjCachebaseStream's
	virtual cjBool OnCacheSetSrcPos(cjStreamOffset cachePos);
	virtual cjBool OnCacheFlush(const void* srcBuf, cjStreamOffset cacheWritePos,
		cjStreamOffset numWrite, cjStreamOffset* numRstWrited);
	virtual cjBool OnCacheRead(cjStreamOffset cacheReadPos, void* dstBuf, cjStreamOffset numRead,
		cjStreamOffset* numRstReaded);


public:
	cjFileStream();
	cjFileStream(cjFile* file, cjTFLAG baseFlag/*cjFStream::ESTREAM_FLAG*/, cjStreamOffset baseFilePos=0,
		cjStreamOffset givenSiz=0);

	cjFileStream(const cjChar* path, cjFilePathHint hint, cjTFLAG baseFlag/*cjFStream::ESTREAM_FLAG*/, 
		cjStreamOffset baseFilePos=0, cjStreamOffset givenSiz=0);
	cjFileStream(const cjUni* path, cjFilePathHint hint, cjTFLAG baseFlag/*cjFStream::ESTREAM_FLAG*/, 
		cjStreamOffset baseFilePos=0, cjStreamOffset givenSiz=0);

	virtual ~cjFileStream();

	inline cjFile* GetFile() { return m_file; }

protected:
	enum EFILE_STREAM_LOCAL_FLAG
	{
		EFSLF_GIVEN_FILE			= 0x00000001,
		EFSLF_GIVEN_SIZ				= 0x00000010,
		EFSLF_PERSIST				= 0x10000000,
		EFSLF_ALWAYS_FLUSH			= 0x20000000,
	};

private:
	cjFile*									m_file;

	// File size (only valid when reading file) or given size
	cjStreamOffset							m_fileSiz;
	
	// base position (Absolute file position)
	cjStreamOffset							m_basePos;	

	cjTFLAG									m_localFlag;

	cjChar*									m_name;
	cjChar									m_path[cjMAX_PATH];
};



#endif

