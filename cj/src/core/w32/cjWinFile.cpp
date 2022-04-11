
#include "cjCorePCH.h"
#include "cjWinFile.h"


//
// cjW32FileSystem
//

cjW32FileSystem::cjW32FileSystem()
{

}

cjW32FileSystem::~cjW32FileSystem()
{
}

const cjChar* cjW32FileSystem::GetWriteablePath()
{
    return cjNULL;
}

cjBool cjW32FileSystem::IsFileExist(const cjChar* path)
{
	cjChar pathResolve[cjMAX_PATH];
	cjFile::ResolvePath(pathResolve, cjMAX_PATH, path, cjFilePathHint_Base);

	DWORD flag = GetFileAttributesA(pathResolve);
	if(flag == INVALID_FILE_ATTRIBUTES || (flag & FILE_ATTRIBUTE_DIRECTORY))
		return cjFalse;

	return cjTrue;
}

cjBool cjW32FileSystem::IsFileExist(const cjUni* path)
{
	cjUni pathResolve[cjMAX_PATH];
	cjFile::ResolvePath(pathResolve, cjMAX_PATH, path, cjFilePathHint_Base);
	
	DWORD flag = GetFileAttributesW(pathResolve);
	if(flag == INVALID_FILE_ATTRIBUTES || (flag & FILE_ATTRIBUTE_DIRECTORY))
		return cjFalse;

	return cjTrue;
}

const cjChar* cjW32FileSystem::ResolveFullPath(const cjChar* path)
{
	return path;
}

const cjChar* cjW32FileSystem::ResolveFullPath(const cjUni* path)
{
	return 0;
}

cjFStream* cjW32FileSystem::CreateFileReadStream(const cjChar* path, cjFilePathHint hint, 
	cjTFLAG /*readFlag*/)
{
    cjFStream* readStream = new cjFileStream(path, hint, cjFStream::ESF_READ);
    
    if(!readStream->Open())
    {
        delete readStream;
        return cjNULL;
    }
    
    return readStream;
}

cjFStream* cjW32FileSystem::CreateFileReadStream(const cjUni* path, cjFilePathHint hint,
	cjTFLAG /*readFlag*/)
{
	cjFStream* readStream = new cjFileStream(path, hint, cjFStream::ESF_READ);

	if(!readStream->Open())
	{
		delete readStream;
		return cjNULL;
	}

	return readStream;
}

cjFStream* cjW32FileSystem::CreateFileWriteStream(const cjChar* path, cjFilePathHint hint, 
	cjTFLAG /*writeFlag*/)
{
	cjFStream* writeStream = new cjFileStream(path, hint, cjFStream::ESF_WRITE);

	if(!writeStream->Open())
	{
		delete writeStream;
		return cjNULL;
	}

	return writeStream;
}

cjFStream* cjW32FileSystem::CreateFileWriteStream(const cjUni* path, cjFilePathHint hint, 
	cjTFLAG /*writeFlag*/)
{
	cjFStream* writeStream = new cjFileStream(path, hint, cjFStream::ESF_WRITE);

	if(!writeStream->Open())
	{
		delete writeStream;
		return cjNULL;
	}

	return writeStream;
}

cjUINT64 cjW32FileSystem::GetFileSiz(const cjChar* path)
{
	cjChar pathResolve[cjMAX_PATH];
	cjFile::ResolvePath(pathResolve, cjMAX_PATH, path, cjFilePathHint_Base);

	HANDLE handle = CreateFileA(pathResolve, GENERIC_READ, FILE_SHARE_READ, cjNULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, cjNULL);

	if(handle==INVALID_HANDLE_VALUE)
		return (cjUINT64)-1;

	DWORD siz = GetFileSize(handle, cjNULL);
	CloseHandle(handle);

	return siz;
}

cjUINT64 cjW32FileSystem::GetFileSiz(const cjUni* path)
{
	cjUni pathResolve[cjMAX_PATH];
	cjFile::ResolvePath(pathResolve, cjMAX_PATH, path, cjFilePathHint_Base);

	HANDLE handle = CreateFileW(pathResolve, GENERIC_READ, FILE_SHARE_READ, cjNULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, cjNULL);

	if(handle==INVALID_HANDLE_VALUE)
		return (cjUINT64)-1;

	DWORD siz = GetFileSize(handle, cjNULL);
	CloseHandle(handle);

	return siz;
}

cjBool cjW32FileSystem::DeleteFile(const cjChar* path)
{
	cjChar pathResolve[cjMAX_PATH];
	cjFile::ResolvePath(pathResolve, cjMAX_PATH, path, cjFilePathHint_Base);

	return ::DeleteFileA(path) ? cjTrue : cjFalse;
}

cjBool cjW32FileSystem::DeleteFile(const cjUni* path)
{
	cjUni pathResolve[cjMAX_PATH];
	cjFile::ResolvePath(pathResolve, cjMAX_PATH, path, cjFilePathHint_Base);

	return ::DeleteFileW(pathResolve) ? cjTrue : cjFalse;
}

cjBool cjW32FileSystem::Rename(const cjChar* renameTo, const cjChar* renameFrom)
{
	cjChar renameToResolve[cjMAX_PATH];
	cjChar renameFromResolve[cjMAX_PATH];

	cjFile::ResolvePath(renameToResolve, cjMAX_PATH, renameTo, cjFilePathHint_Base);
	cjFile::ResolvePath(renameFromResolve, cjMAX_PATH, renameFrom, cjFilePathHint_Base);

	return MoveFileExA(renameFromResolve, renameToResolve, MOVEFILE_COPY_ALLOWED) ? cjTrue : cjFalse;
}

cjBool cjW32FileSystem::Rename(const cjUni* renameTo, const cjUni* renameFrom)
{
	cjUni renameToResolve[cjMAX_PATH];
	cjUni renameFromResolve[cjMAX_PATH];

	cjFile::ResolvePath(renameToResolve, cjMAX_PATH, renameTo, cjFilePathHint_Base);
	cjFile::ResolvePath(renameFromResolve, cjMAX_PATH, renameFrom, cjFilePathHint_Base);

	return MoveFileExW(renameFromResolve, renameToResolve, MOVEFILE_COPY_ALLOWED) ? cjTrue : cjFalse;
}

cjBool cjW32FileSystem::GetCurDirectory(cjChar* path, cjUINT32 siz)
{
	return ::GetCurrentDirectoryA(siz,  path) == 0 ? cjFalse : cjTrue;
}

cjBool cjW32FileSystem::GetCurDirectory(cjUni* path, cjUINT32 siz)
{
	// ret : length of path
	return ::GetCurrentDirectoryW(siz,  path) == 0 ? cjFalse : cjTrue;
}


//cjTAry<cjString> cjW32FileSystem::FindFiles(const cjChar* path, cjBool incFile, cjBool incDirectory)
//{
//    cjTAry<cjString> empty;
//    
//    return empty;
//}


//
// cjWinFile
//

cjWinFile::cjWinFile()
	: m_handle(INVALID_HANDLE_VALUE) 
{
}

cjWinFile::cjWinFile(const cjChar* path, cjTFLAG openFlag, cjFilePathHint hint, cjTFLAG shareFlag, void* param)
	: m_handle(INVALID_HANDLE_VALUE), m_openFlag(openFlag), 
	m_shareFlag(shareFlag), m_filePos(0), m_name(cjNULL), m_param(param)
{
	if(path)
	{
		ResolvePath(m_path, cjMAX_PATH, path, hint);
	}
	else
		SetErr(ERR_INVALID_PARAM);
}

cjWinFile::cjWinFile(const cjUni* path, cjTFLAG openFlag, cjFilePathHint hint, cjTFLAG shareFlag, void* param)
	: m_handle(INVALID_HANDLE_VALUE), m_openFlag(openFlag), 
	m_shareFlag(shareFlag), m_filePos(0), m_name(cjNULL), m_param(param)
{
	if(path)
	{
		cjUni tmpPath[cjMAX_PATH];
		ResolvePath(tmpPath, cjMAX_PATH, path, hint);

		//cjslow
		cjUni2Char uni2ch(tmpPath);
		cjStrcpy(m_path, cjMAX_PATH, uni2ch);
	}
	else
		SetErr(ERR_INVALID_PARAM);
}


cjWinFile::~cjWinFile()
{
	Close();
}

cjBool cjWinFile::Open()
{
	if(m_err)
		return cjFalse;

	m_name = GetNameExtFromPath(m_path);

	cjTFLAG desiredAccess, shareMode, creationDisposition;
	cjTFLAG flagAndAttrib;

	TranslateToCreateFlag(m_openFlag, &desiredAccess, &shareMode,
		&creationDisposition, &flagAndAttrib);

//	cjChar winPath[cjMAX_PATH];
//	NormalizePath(winPath, cjMAX_PATH, GetPath(), '\\');

	// flag ÀÌ»ף
//#ifdef CJUNICODE
//		::CreateFileW
//#else
		m_handle = ::CreateFileA
//#endif
		(GetPath(), 
		desiredAccess,			// W:GENERIC_WRITE
		shareMode,				// 0 for do not share [FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SAHRE_WRITE]
		cjNULL,					// default security
		creationDisposition,	// W:CREATE_NEW         
		flagAndAttrib,			// FILE_ATTRIBUTE_NORMAL
		cjNULL);

//	::CreateFileA(cjTempPath,  GENERIC_READ, FILE_SHARE_READ,
//		cjNULL, OPEN_EXISTING, flagAndAttrib, cjNULL);

	if(m_handle == INVALID_HANDLE_VALUE)
	{
		DebugBreak();
		return cjFalse;
	}
	
	return cjTrue;
}

void cjWinFile::Close()
{
	if(m_handle != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(m_handle);
		m_handle = INVALID_HANDLE_VALUE;
	}

	cjSuper::Close();
}

void cjWinFile::FlushBuffer()
{
	if(m_handle != INVALID_HANDLE_VALUE)
		::FlushFileBuffers(m_handle);
}

cjBool cjWinFile::ReWind()
{
	if(m_handle == INVALID_HANDLE_VALUE)
		return cjFalse;

	return SetPos(0);
}

cjStreamOffset cjWinFile::GetPos()
{
	if(m_handle == INVALID_HANDLE_VALUE)
		return (cjStreamOffset)-1;

	LARGE_INTEGER pos;
	pos.QuadPart = 0;

//	if(!FlushFileBuffers(m_handle))
//		return (cjStreamOffset)-1;

	pos.LowPart = ::SetFilePointer(m_handle, pos.LowPart, &pos.HighPart, FILE_CURRENT);

	if(pos.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != ERROR_SUCCESS)
		return (cjStreamOffset)-1;

#ifdef CJ_USE_64BIT_STREAM
	return pos.QuadPart;
#else
	return pos.LowPart;
#endif
}

cjBool cjWinFile::SetPos(cjStreamOffset pos)
{
	if(m_handle == INVALID_HANDLE_VALUE)
		return cjFalse;

	if(pos == m_filePos)
		return cjTrue;

	LARGE_INTEGER		dstPos;

#ifdef CJ_USE_64BIT_STREAM
	dstPos.QuadPart = pos;
#else
	dstPos.QuadPart = 0;
	dstPos.LowPart = pos;
#endif

	dstPos.LowPart = SetFilePointer(m_handle, dstPos.LowPart, &dstPos.HighPart, FILE_BEGIN);

	if(dstPos.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != ERROR_SUCCESS)
	{
	//	dstPos.QuadPart = -1;
		return cjFalse;
	}
	
#ifdef CJ_USE_64BIT_STREAM
	m_filePos = dstPos.QuadPart;
#else
	m_filePos = dstPos.LowPart;
#endif

	return cjTrue;
}

cjStreamOffset cjWinFile::GetSiz()
{
	if(m_handle == INVALID_HANDLE_VALUE)
		return 0;

	LARGE_INTEGER siz;
		
	// function needs xp or higer, same for GetFileSize
	if(!GetFileSizeEx(m_handle, &siz))
		return 0;

#ifdef CJ_USE_64BIT_STREAM
	return siz.QuadPart;
#else
	return siz.LowPart;
#endif

}

cjBool cjWinFile::SetSiz(cjStreamOffset newSiz)
{
	if(m_handle == INVALID_HANDLE_VALUE)
		return cjFalse;

	cjStreamOffset oldPos = m_filePos;

	if(!SetPos(newSiz))
		return cjFalse;

	if(!::SetEndOfFile(m_handle))
		return cjFalse;

	if(!SetPos(oldPos))
		return cjFalse;
	
	return cjTrue;
}

cjBool cjWinFile::Read(void* readBuf, cjStreamOffset bytesToRead, cjStreamOffset* bytesReaded, 
	cjStreamOffset* readPos)
{
	if(m_handle == INVALID_HANDLE_VALUE)
	{
		SetErr(ERR_INVALID_HANDLE);
		return cjFalse;
	}

	cjWinDWORD rstReadBytes = 0;	
	if(bytesToRead)
	{
		if(readPos)
		{
			cjStreamOffset cur = GetPos();

			if(*readPos != cur)
			{
				if(!SetPos(*readPos))
					return cjFalse;

				m_filePos = *readPos;
			}
		}


#ifdef CJ_P64

		cjWinDWORD _rstReadBytes = 0;
		if(!::ReadFile(m_handle, readBuf, (cjWinDWORD)bytesToRead, &_rstReadBytes, cjNULL))
		{			
			SetErr(ERR_INVALID_PARAM);
			return cjFalse;
		}
		rstReadBytes = _rstReadBytes;

#else

		if(!::ReadFile(m_handle, readBuf, (cjWinDWORD)bytesToRead, &rstReadBytes, cjNULL))
		{
			SetErr(ERR_INVALID_PARAM);
			return cjFalse;
		}

#endif

	}

	m_filePos += rstReadBytes;

	if(bytesReaded)
		*bytesReaded = rstReadBytes;

	if(rstReadBytes != bytesToRead)
	{
		// reached End of File
		SetErr(ERR_REACHED_EOF);
		return cjFalse;
	}

	return cjTrue;
}

cjBool cjWinFile::Write(const void* writeBuf, cjStreamOffset bytesToWrite, cjStreamOffset* bytesWrited, 
	cjStreamOffset* writePos, cjBool doFlush)
{
	if(m_handle == INVALID_HANDLE_VALUE)
	{
		SetErr(ERR_INVALID_HANDLE);
		return cjFalse;
	}

	cjWinDWORD rstWriteBytes = 0;

	if(bytesToWrite)
	{
		if(writePos)
		{
			cjStreamOffset cur = GetPos();

			if(*writePos != cur)
			{
				if(!SetPos(cur))
					return cjFalse;

				m_filePos = *writePos;
			}
		}

#ifdef CJ_P64
		cjWinDWORD _rstWriteBytes = 0;
		if(!::WriteFile(m_handle, writeBuf, (cjWinDWORD)bytesToWrite, &_rstWriteBytes, cjNULL))
		{
			SetErr(ERR_INVALID_PARAM);
			return cjFalse;
		}
		rstWriteBytes = _rstWriteBytes;
#else
		if(!::WriteFile(m_handle, writeBuf, (cjWinDWORD)bytesToWrite, &rstWriteBytes, cjNULL))
		{
			SetErr(ERR_INVALID_PARAM);
			return cjFalse;
		}
#endif
	}

	m_filePos += rstWriteBytes;

	if(bytesWrited)
		*bytesWrited = rstWriteBytes;

	if(bytesToWrite != rstWriteBytes)
	{
		SetErr(ERR_PARTIAL_WRITE);
		return cjFalse;
	}

	if(doFlush)
		::FlushFileBuffers(m_handle);

	return cjTrue;
}

void cjWinFile::TranslateToCreateFlag(cjTFLAG winFlag, cjTFLAG* desiredAccess, 
	cjTFLAG* shareMode, cjTFLAG* creationDisposition, cjTFLAG* flagAndAttrib)
{
	if(desiredAccess) *desiredAccess = 0;
	if(shareMode) *shareMode = 0;
	if(creationDisposition) *creationDisposition = 0;
	if(flagAndAttrib) *flagAndAttrib = 0;

	if(winFlag & EFF_READ)
	{
		*desiredAccess |= GENERIC_READ;
		*shareMode |= FILE_SHARE_READ;
		*creationDisposition |= OPEN_EXISTING;
		*flagAndAttrib = FILE_ATTRIBUTE_NORMAL; // | FILE_FLAG_OVERLAPPED
	}
	else if(winFlag & EFF_WRITE)
	{
		*desiredAccess |= GENERIC_WRITE;

#ifdef CJDEBUG
		*shareMode |= FILE_SHARE_READ;
#endif

		if(winFlag & EFF_CREATE)
		{
			(*creationDisposition) |= CREATE_ALWAYS;
		}
		else
		{	// EXISTING ONLY
			(*creationDisposition) |= OPEN_EXISTING;
		}

		if(winFlag & EFF_TRUNCATE)
			*creationDisposition |= TRUNCATE_EXISTING;
	}
}

cjFile* cjWinFile::CreateInstanceM(const cjChar* path, cjTFLAG openFlag, cjFilePathHint hint, 
	cjTFLAG shareFlag, void* param)
{
    return new cjWinFile(path, openFlag, hint, shareFlag, param);
}

cjFile* cjWinFile::CreateInstanceU(const cjUni* path, cjTFLAG openFlag, cjFilePathHint hint,
	cjTFLAG shareFlag, void* param)
{
	return new cjWinFile(path, openFlag, hint, shareFlag, param);
}
