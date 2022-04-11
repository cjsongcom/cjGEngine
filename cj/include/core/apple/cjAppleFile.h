
#ifndef _CJAPPLEFILE_H_
#define _CJAPPLEFILE_H_


class cjIOSFileSystem : public cjFileSystem
{
public:
    typedef cjFileSystem cjSuper;

    virtual const cjChar* GetWriteablePath();
    
    virtual cjBool IsFileExist(const cjChar* path);
    virtual cjBool IsFileExist(const cjUni* path);
    
    virtual const cjChar* ResolveFullPath(const cjChar* path);
    virtual const cjChar* ResolveFullPath(const cjUni* path);
    
	virtual cjFStream* CreateFileReadStream(const cjChar* path, cjFilePathHint hint=cjFilePathHint_Default, cjTFLAG readFlag=0);
	virtual cjFStream* CreateFileReadStream(const cjUni* path, cjFilePathHint hint=cjFilePathHint_Default, cjTFLAG readFlag=0);

	virtual cjFStream* CreateFileWriteStream(const cjChar* path, cjFilePathHint hint=cjFilePathHint_Default, cjTFLAG writeFlag=0);
	virtual cjFStream* CreateFileWriteStream(const cjUni* path, cjFilePathHint hint=cjFilePathHint_Default, cjTFLAG writeFlag=0);
    
	virtual cjUINT64 GetFileSiz(const cjChar* path);
	virtual cjUINT64 GetFileSiz(const cjUni* path);

	virtual cjBool DeleteFile(const cjChar* path);
	virtual cjBool DeleteFile(const cjUni* path);

	virtual cjBool Rename(const cjChar* renameTo, const cjChar* renameFrom);
	virtual cjBool Rename(const cjUni* renameTo, const cjUni* renameFrom);
    
//	virtual cjTAry<cjString> FindFiles(const cjChar* path, cjBool incFile=cjTrue, cjBool incDirectory=cjFalse);
    
public:
    cjIOSFileSystem();
    virtual ~cjIOSFileSystem();

private:
    typedef cjTHashNCase<const char*, cjStringA>        AppFullPathMap;
    
    AppFullPathMap                                      m_hmapFullPath;
    void*                                               m_nsFilieManager; // NSFileManager
};


class cjIOSFile : public cjAppFile
{
public:
    typedef cjAppFile       cjSuper;
    typedef cjINT32         FileHandleType;
    const static cjINT32    INVALID_FILE_HANDLE_VAL = -1;
    
   	virtual cjBool Open();
	virtual void Close();
	virtual void FlushBuffer();
    virtual cjBool IsValid() { return m_handle != INVALID_FILE_HANDLE_VAL ? cjTrue : cjFalse; }

	virtual cjBool ReWind();
	
	virtual cjStreamOffset GetPos();
	virtual cjBool SetPos(cjStreamOffset pos);
	
	virtual cjStreamOffset GetSiz();
	virtual cjBool SetSiz(cjStreamOffset newSiz);

    virtual cjBool Read(void* readBuf, cjStreamOffset bytesToRead, cjStreamOffset* bytesReaded=NULL,
        cjStreamOffset* readPos=NULL);
	virtual cjBool Write(const void* writeBuf, cjStreamOffset bytesToWrite, cjStreamOffset* bytesWrited=NULL,
        cjStreamOffset* writePos=NULL, cjBool doFlush=cjFalse);

	virtual const cjChar* GetPath() { return m_path; }
	virtual const cjChar* GetName() { return m_name; }	// return name only on path

public:
    cjIOSFile();
    cjIOSFile(const cjChar* path, cjTFLAG openFlag, cjFilePathHint hint=cjFilePathHint_Default,
        cjTFLAG shareFlag=0, void* param=0);
    cjIOSFile(const cjUni* path, cjTFLAG openFlag, cjFilePathHint hint=cjFilePathHint_Default,
        cjTFLAG shareFlag=0, void* param=0);

    virtual ~cjIOSFile();

    cjBool TranslateOpenFlag(cjTFLAG openFlag, cjINT32& transOpenFlag);
    
    static cjFile* CreateInstanceM(const cjChar* path, cjTFLAG openFlag, cjFilePathHint hint,
        cjTFLAG shareFlag, void* param);
    static cjFile* CreateInstanceU(const cjUni* path, cjTFLAG openFlag, cjFilePathHint hint,
        cjTFLAG shareFlag, void* param);
    
protected:
	cjINT32                                 m_handle;       // handle for open (CRT)
	cjTFLAG                                 m_openFlag;
	cjTFLAG                                 m_shareFlag;
    void*                                   m_param;

	cjStreamOffset                          m_filePos;

	cjChar                                  m_path[cjMAX_PATH];
	const cjChar*                           m_name; //ref
};


#endif

