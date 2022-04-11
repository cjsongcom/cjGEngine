
#ifndef _CJWINFILE_H_
#define _CJWINFILE_H_


class cjFStream;

class CJCORE_API cjW32FileSystem : public cjFileSystem
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

	virtual cjBool GetCurDirectory(cjChar* path, cjUINT32 siz=cjMAX_PATH);
	virtual cjBool GetCurDirectory(cjUni* path, cjUINT32 siz=cjMAX_PATH);

//	virtual cjTAry<cjString> FindFiles(const cjChar* path, cjBool incFile=cjTrue, cjBool incDirectory=cjFalse);
    
public:
    cjW32FileSystem();
    virtual ~cjW32FileSystem();

};


class CJCORE_API cjWinFile : public cjFile
{
	typedef cjFile cjSuper;

public:
	cjWinFile();
	cjWinFile(const cjChar* path, cjTFLAG openFlag, cjFilePathHint hint=cjFilePathHint_Default,
		cjTFLAG shareFlag=0, void* param=NULL);
	cjWinFile(const cjUni* path, cjTFLAG openFlag, cjFilePathHint hint=cjFilePathHint_Default,
		cjTFLAG shareFlag=0, void* param=NULL);

	virtual ~cjWinFile();

	virtual cjBool Open();
	virtual void Close();
	virtual void FlushBuffer();
	virtual cjBool IsValid() { return m_handle != INVALID_HANDLE_VALUE ? cjTrue : cjFalse; }

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
	virtual const cjChar* GetName() { return m_name; }

	void TranslateToCreateFlag(cjTFLAG winFlag, cjTFLAG* desiredAccess,
		cjTFLAG* shareMode, cjTFLAG* creationDisposition, cjTFLAG* flagAndAttrib);

	static cjFile* CreateInstanceM(const cjChar* path, cjTFLAG openFlag, cjFilePathHint hint, 
		cjTFLAG shareFlag, void* param);
	static cjFile* CreateInstanceU(const cjUni* path, cjTFLAG openFlag, cjFilePathHint hint, 
		cjTFLAG shareFlag, void* param);

private:
	HANDLE								m_handle;
	cjTFLAG								m_openFlag;
	cjTFLAG								m_shareFlag;
	void*								m_param;

	cjStreamOffset						m_filePos;

	cjChar								m_path[cjMAX_PATH];
	const cjChar*						m_name; //ref
};


#endif
