
#ifndef _CJFILE_H_
#define _CJFILE_H_


//
// cjFileSystem
//

class CJCORE_API cjFileSystem : public cjMem
{
public:
    virtual const cjChar* GetWriteablePath() = 0;

    virtual cjBool IsFileExist(const cjChar* path) = 0;
	virtual cjBool IsFileExist(const cjUni* path) = 0;
    
    virtual const cjChar* ResolveFullPath(const cjChar* relPath) = 0;
    virtual const cjChar* ResolveFullPath(const cjUni* path) = 0;
   
	virtual cjFStream* CreateFileReadStream(const cjChar* path, cjFilePathHint hint=cjFilePathHint_Default, cjTFLAG readFlag=0) = 0;
	virtual cjFStream* CreateFileReadStream(const cjUni* path, cjFilePathHint hint=cjFilePathHint_Default, cjTFLAG readFlag=0) = 0;

	virtual cjFStream* CreateFileWriteStream(const cjChar* path, cjFilePathHint hint=cjFilePathHint_Default, cjTFLAG writeFlag=0) = 0;
	virtual cjFStream* CreateFileWriteStream(const cjUni* path, cjFilePathHint hint=cjFilePathHint_Default, cjTFLAG writeFlag=0) = 0;
    
	virtual cjUINT64 GetFileSiz(const cjChar* path) = 0;
	virtual cjUINT64 GetFileSiz(const cjUni* path) = 0;

	virtual cjBool DeleteFile(const cjChar* path) = 0;
	virtual cjBool DeleteFile(const cjUni* path) = 0;

	virtual cjBool Rename(const cjChar* /*renameTo*/, const cjChar* /*renameFrom*/) = 0;
	virtual cjBool Rename(const cjUni* /*renameTo*/, const cjUni* /*renameFrom*/) = 0;

	virtual cjBool GetCurDirectory(cjChar* path, cjUINT32 siz=cjMAX_PATH) = 0;
	virtual cjBool GetCurDirectory(cjUni* path, cjUINT32 siz=cjMAX_PATH) = 0;
    
//	virtual cjTAry<cjString> FindFiles(const cjChar* path, cjBool incFile=cjTrue,
//        cjBool incDirectory=cjFalse)=0;
    
    static void StaticInit_cjFileSystem();
    static void StaticShutdown_cjFileSystem();
    
    static cjFileSystem* Get();

protected:
    cjFileSystem();
    virtual ~cjFileSystem();
};


//
// cjFile
//

class CJCORE_API cjFile : public cjMem
{
    friend class cjFileSystem;
    
public:
	enum EFileFlag
	{
		// open existing file
		EFF_READ                        = 0x00000001,
		EFF_WRITE                       = 0x00000002,
		EFF_READ_WRITE                  = 0x00000003,

        // create (write mode only)
		EFF_CREATE                      = 0x00000010,  // if file is not exist, it create new file or just ok
		EFF_CREATE_ONLY_NOT_EXIST       = 0x00000020,  // create file only when that is not exist
        
		EFF_TRUNCATE                    = 0x00000100,  // drop data
	};

	enum ERR_STATUS
	{
		ERR_OK							= 0,
		ERR_INVALID_HANDLE				= -1,
		ERR_INVALID_PARAM				= -2,
		ERR_REACHED_EOF					= -3,	// file: EOF
		ERR_PARTIAL_WRITE				= -4,   // file: Disk full ?
		ERR_OPEN_FAIL					= -5,
		ERR_CLOSED						= -6,
        
        // In App(IOS,Android)
        ERR_APP_RESOLVE_PATH            = -1001,
	};

	enum ESeek
	{
		ESeek_Begin,
		ESeek_Cur,
		ESeek_End
	};

	cjFile();
	virtual ~cjFile();

	static cjFile* CreateDefer(const cjChar* path, cjTFLAG openFlag=EFF_READ, cjFilePathHint hint=cjFilePathHint_Default,
		cjTFLAG shareFlag=0, void* param=NULL);

	static cjFile* Create(const cjChar* path, cjTFLAG openFlag=EFF_READ, cjFilePathHint hint=cjFilePathHint_Default,
		cjTFLAG shareFlag=0, void* param=NULL);

	virtual cjBool Open() = 0;
	virtual void Close();
	virtual void FlushBuffer() = 0;
    virtual cjBool IsValid() = 0;

	virtual cjBool ReWind() = 0;
	
	virtual cjStreamOffset GetPos() = 0;
	virtual cjBool SetPos(cjStreamOffset pos) = 0;
	
	virtual cjStreamOffset GetSiz() = 0;
	virtual cjBool SetSiz(cjStreamOffset newSiz) = 0;

	virtual cjBool Read(void* readBuf, cjStreamOffset bytesToRead, cjStreamOffset* bytesReaded=NULL, cjStreamOffset* readPos=NULL) = 0;
	virtual cjBool Write(const void* writeBuf, cjStreamOffset bytesToWrite, cjStreamOffset* bytesWrited=NULL, cjStreamOffset* writePos=NULL, cjBool doFlush=cjFalse) = 0;

	virtual void SetErr(ERR_STATUS ERR_STATUS) { m_err = ERR_STATUS; }
	inline ERR_STATUS GetLastErr() { return m_err; }

	virtual const cjChar* GetPath() = 0;
	virtual const cjChar* GetName() = 0;	// return name only on path

	static cjChar* GetNameExtFromPath(cjChar* path);
	inline static const cjChar* GetNameExtFromPath(const cjChar* path)
		{ return GetNameExtFromPath((cjChar*)path); }
    
    //
    // static utility through cjFileSystem
    //
    static cjBool IsFileExist(const cjChar* path);
	static cjBool IsFileExist(const cjUni* path);

	static cjBool IsAbsolutePath(const cjChar* path);
	static cjBool IsAbsolutePath(const cjUni* path);

	static void NormalizePath(cjChar* out, cjINT32 bufSiz, const cjChar* path, cjChar d='/');
	static void NormalizePath(cjUni* out, cjINT32 bufSiz, const cjUni* path, cjUni d='/');

	static cjChar* ResolvePath(cjChar* out, cjINT32 sizBuf, const cjChar* path, cjFilePathHint hint);
	static cjUni* ResolvePath(cjUni* out, cjINT32 sizBuf, const cjUni* path, cjFilePathHint hint);
	
	static const cjChar* GetBasePath(cjFilePathHint hint=cjFilePathHint_Base) { return ms_basePath[hint]; }
	static const cjUni* GetBasePathU(cjFilePathHint hint=cjFilePathHint_Base) { return ms_basePathU[hint]; }

	static void SetBasePath(cjFilePathHint hint, const cjChar* path);
	static void SetBasePath(cjFilePathHint hint, const cjUni* path);

protected:
	ERR_STATUS                              m_err;
    static cjFileSystem*                    ms_fileSys;

private:
	// unicode in ios 4byte,utf32  /  char 1byte, utf8
	// unicode in w32 2byte utf16  /  char 1byte, utf8
	static cjChar							ms_basePath[cjFilePathHint_Max][cjMAX_PATH];
	static cjUni							ms_basePathU[cjFilePathHint_Max][cjMAX_PATH];
	
private:
	typedef cjFile* (*FileInstanceCreatorM)(const cjChar* , cjTFLAG, cjFilePathHint hint, cjTFLAG, void*);
	typedef cjFile* (*FileInstanceCreatorU)(const cjUni* , cjTFLAG, cjFilePathHint hint, cjTFLAG, void*);
	
	static FileInstanceCreatorM             ms_pfnInstanceCreatorM;
	static FileInstanceCreatorU             ms_pfnInstanceCreatorU;
    
};


//
// cjAppFile
//

class CJCORE_API cjAppFile : public cjFile
{
    typedef cjFile cjSuper;
    
public:
    virtual cjBool Open() = 0;
    virtual void Close() = 0;

public:
    cjAppFile();
    virtual ~cjAppFile();
    
    static const cjChar* GetWriteablePath();

};



#endif
