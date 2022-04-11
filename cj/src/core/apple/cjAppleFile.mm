
#include "cjCorePCH.h"
#include "apple/cjAppleFile.h"
#include "fcntl.h"

static NSFileManager* gs_nsFileManager = nil;
static NSBundle* gs_nsMainBundle = nil;
static NSString* gs_nsMainBundleResPath = nil;

static const cjINT32 CJ_MAX_BUNDLE_RESPATH_SIZ = 512;
static cjChar gs_bundleResPath[CJ_MAX_BUNDLE_RESPATH_SIZ];


//
// cjIOSFileSystem
//

cjIOSFileSystem::cjIOSFileSystem()
{
    if(gs_nsFileManager == nil)
    {
        gs_nsFileManager = [NSFileManager defaultManager];
        [gs_nsFileManager retain];
        
        gs_nsMainBundle = [NSBundle mainBundle];
        [gs_nsMainBundle retain];
        
        gs_nsMainBundleResPath = gs_nsMainBundle.resourcePath;
        cjStrcpy(gs_bundleResPath, [gs_nsMainBundleResPath UTF8String]);
    }
}

cjIOSFileSystem::~cjIOSFileSystem()
{
    if(gs_nsMainBundle != nil)
    {
        [gs_nsMainBundle release];
        gs_nsMainBundle = nil;
    }
    
    gs_nsMainBundleResPath = nil;

    if(gs_nsFileManager != nil)
    {
        [gs_nsFileManager release];
        gs_nsFileManager = nil;
    }
}

const cjChar* cjIOSFileSystem::GetWriteablePath()
{
    return 0;
}

cjBool cjIOSFileSystem::IsFileExist(const cjChar* path)
{
    if(!path || !*path)
        return cjFalse;
    
    if(path[0] != '/')
    {
        path = ResolveFullPath(path);
        
        if(!path)
            return cjFalse;
    }

    if([gs_nsFileManager fileExistsAtPath:[NSString stringWithUTF8String:path]])
       return cjTrue;
    else
       return cjFalse;
}

cjBool cjIOSFileSystem::IsFileExist(const cjUni* path)
{
    if(!path || !*path)
        return cjFalse;
    
    cjUni2Char u2c(path);
    
    return IsFileExist(*u2c);
}

const cjChar* cjIOSFileSystem::ResolveFullPath(const cjChar* path)
{
    if(path == 0)
        return 0;
    
    if(path[0] == '/') // absolute path
    {
        if([gs_nsFileManager fileExistsAtPath:[NSString stringWithUTF8String:path]])
            return path;
        
        return 0;
    }

    AppFullPathMap::Pair* findPair = m_hmapFullPath.Find(path);
    
    if(findPair)
        return findPair->m_val.GetStr();
    
    cjStringA pathOnly, filenameOnly;
    cjSplitPathA(path, pathOnly, filenameOnly, cjTrue); // add '/'
    
    if(!filenameOnly.Len())
        return 0;
    
    NSString* fullpath = [gs_nsMainBundle pathForResource:[NSString stringWithUTF8String:(*filenameOnly)] ofType:nil inDirectory:[NSString stringWithUTF8String:(*pathOnly)]];
    
    const char* resolvedFullPath = NULL;
    if(fullpath != nil)
    {
       resolvedFullPath = [fullpath UTF8String];
    }
    else
    {
        cjLOG("WARN! failed to resolve app path for '%s'.. ", path);
    }
    
    AppFullPathMap::Pair* insPair = m_hmapFullPath.Insert(path, resolvedFullPath);
    
    return insPair->m_val;
}

const cjChar* cjIOSFileSystem::ResolveFullPath(const cjUni* path)
{
    if(path == 0)
        return 0;
    
    cjUni2Char u2c(path);
    
    return ResolveFullPath(*u2c);
}

cjFStream* cjIOSFileSystem::CreateFileReadStream(const cjChar* path, cjFilePathHint hint, cjTFLAG readFlag)
{
    cjFStream* readStream = new cjFileStream(path, hint, cjFStream::ESF_READ);
    
    if(!readStream->Open())
    {
        delete readStream;
        return NULL;
    }
    
    return readStream;
}

cjFStream* cjIOSFileSystem::CreateFileReadStream(const cjUni* path, cjFilePathHint hint, cjTFLAG readFlag)
{
    cjFStream* readStream = new cjFileStream(path, hint, cjFStream::ESF_READ);
    
    if(!readStream->Open())
    {
        delete readStream;
        return NULL;
    }
    
    return readStream;
}

cjFStream* cjIOSFileSystem::CreateFileWriteStream(const cjChar* path, cjFilePathHint hint, cjTFLAG writeFlag)
{
    return 0;
}

cjFStream* cjIOSFileSystem::CreateFileWriteStream(const cjUni* path, cjFilePathHint hint, cjTFLAG writeFlag)
{
    return 0;
}

cjUINT64 cjIOSFileSystem::GetFileSiz(const cjChar* path)
{
    if(!path)
        return 0;

    NSString* nsPath = [NSString stringWithUTF8String:path];
    unsigned long long siz = [gs_nsFileManager attributesOfItemAtPath:nsPath error:nil].fileSize;
    
    return siz;
}

cjUINT64 cjIOSFileSystem::GetFileSiz(const cjUni* path)
{
    if(!path)
        return 0;
    
    cjUni2Char u2c(path);
    
    return GetFileSiz(*u2c);
}

cjBool cjIOSFileSystem::DeleteFile(const cjChar* path)
{
    if(!path)
        return cjFalse;
    
    NSString* nsPath = [NSString stringWithUTF8String:path];
    return [gs_nsFileManager removeItemAtPath:nsPath error:nil];
}

cjBool cjIOSFileSystem::DeleteFile(const cjUni* path)
{
    if(!path)
        return cjFalse;

    cjUni2Char u2c(path);
    return DeleteFile(*u2c);
}

cjBool cjIOSFileSystem::Rename(const cjChar* renameTo, const cjChar* renameFrom)
{
    if(!renameTo || !renameFrom)
        return cjFalse;
    
    NSString* nsRenameTo = [NSString stringWithUTF8String:renameTo];
    NSString* nsRenameFrom = [NSString stringWithUTF8String:renameFrom];
    
    return [gs_nsFileManager moveItemAtPath:nsRenameTo toPath:nsRenameFrom error:nil];
}

cjBool cjIOSFileSystem::Rename(const cjUni* renameTo, const cjUni* renameFrom)
{
    if(!renameTo || !renameFrom)
        return cjFalse;

    cjUni2Char u2cTo(renameTo);
    cjUni2Char u2cFrom(renameFrom);

    return Rename(*u2cTo, *u2cFrom);
}


//cjTAry<cjString> cjIOSFileSystem::FindFiles(const cjChar* path, cjBool incFile, cjBool incDirectory)
//{
//    cjTAry<cjString> empty;
//    
//    return empty;
//}


//
// cjIOSFile
//

cjFile* cjIOSFile::CreateInstanceM(const cjChar* path, cjTFLAG openFlag, cjFilePathHint hint, cjTFLAG shareFlag, void* param)
{
    return new cjIOSFile(path, openFlag, hint, shareFlag, param);
}

cjFile* cjIOSFile::CreateInstanceU(const cjUni* path, cjTFLAG openFlag, cjFilePathHint hint, cjTFLAG shareFlag, void* param)
{
    return new cjIOSFile(path, openFlag, hint, shareFlag, param);
}

cjIOSFile::cjIOSFile()
{
    m_handle = INVALID_FILE_HANDLE_VAL;
}

cjIOSFile::cjIOSFile(const cjChar* path, cjTFLAG openFlag, cjFilePathHint , cjTFLAG shareFlag, void* param)
{
    m_handle = INVALID_FILE_HANDLE_VAL;
    m_openFlag = openFlag;
    m_shareFlag = shareFlag;
    m_filePos = 0;
    m_name = NULL;
    m_param = param;

	if(path)
    {
        const char* resolvedPath = ms_fileSys->ResolveFullPath(path);
        if((resolvedPath) && (*resolvedPath != 0))
        {
            cjStrcpy(m_path, cjMAX_PATH, resolvedPath);
        }
        else
        {
            m_path[0] = 0;
            SetErr(ERR_APP_RESOLVE_PATH);
        }
    }
	else
		SetErr(ERR_INVALID_PARAM);
}

cjIOSFile::cjIOSFile(const cjUni* path, cjTFLAG openFlag, cjFilePathHint , cjTFLAG shareFlag, void* param)
{
    m_handle = INVALID_FILE_HANDLE_VAL;
    m_openFlag = openFlag;
    m_shareFlag = shareFlag;
    m_filePos = 0;
    m_name = NULL;
    m_param = param;

	if(path)
    {
        const char* resolvedPath = ms_fileSys->ResolveFullPath(path);
        if((resolvedPath) && (*resolvedPath != 0))
        {
            cjStrcpy(m_path, cjMAX_PATH, resolvedPath);
        }
        else
        {
            m_path[0] = 0;
            SetErr(ERR_APP_RESOLVE_PATH);
        }
    }
	else
		SetErr(ERR_INVALID_PARAM);
}

cjIOSFile::~cjIOSFile()
{
    Close();
}

cjBool cjIOSFile::Open()
{
	if(m_err)
		return cjFalse;
    
    if(IsValid())
        return cjTrue;

	m_name = GetNameExtFromPath(m_path);
    
    /*
     www.gnu.org/software/libc/manual/html_node/Opening-and-Closing-Files.html
     www.gnu.org/software/libc/manual/html_node/Access-Modes.html
     open (filename, O_WRONLY | O_CREAT | O_TRUNC, mode)
    */
    
    cjINT32 operMode;
    
    if(!TranslateOpenFlag(m_openFlag, operMode))
        return cjFalse;

    m_handle = ::open(m_path, operMode);
    
    if(m_handle == INVALID_FILE_HANDLE_VAL)
        return cjFalse;
	
	return cjTrue;
}

cjBool cjIOSFile::TranslateOpenFlag(cjTFLAG openFlag, cjINT32& transOpenFlag)
{
    transOpenFlag = 0;
    
    if(openFlag & EFF_WRITE)
    {
        if(openFlag & EFF_READ)
            transOpenFlag |= O_RDWR;
    
        if(openFlag & EFF_CREATE)
            transOpenFlag |= O_CREAT;
        
        if(openFlag & EFF_CREATE_ONLY_NOT_EXIST)
            transOpenFlag |= O_EXCL;
        
        if(openFlag & EFF_TRUNCATE)
            transOpenFlag |= O_TRUNC;
    }
    else if(openFlag & EFF_READ)
    {
        // read only
        transOpenFlag |= O_RDONLY;
    }
    else
    {
        // bad parameter
        return cjFalse;
    }
    
    /*
     O_RDONLY : read access
     O_WRONLY : write access
     O_RDWR   : read & write
    
     O_CREAT : If set, the file will be created if it doesn't already exist.
     O_EXCL  : If both O_CREAT and O_EXCL are set, then open fails if the specified file already exists. This is guaranteed to never clobber an existing file.
     O_TRUNC : Truncate the file to zero length. This option is only useful for regular files, not special files such as directories or FIFOs. POSIX.1 requires that you open the file for writing to use O_TRUNC. In BSD and GNU you must have permission to write the file to truncate it, but you need not open for write access.
                This is the only open-time action flag specified by POSIX.1. There is no good reason for 
                truncation to be done by open, instead of by calling ftruncate afterwards. The O_TRUNC flag 
                existed in Unix before ftruncate was invented, and is retained for backward compatibility.
    
     O_SHLOCK : Acquire a shared lock on the file, as with flock
                If O_CREAT is specified, the locking is done atomically when creating the file. You are guaranteed that no other process will get the lock on the new file first.
     O_EXLOCK : Acquire an exclusive lock on the file, as with flock
     
     O_SYNC   : no needs flush io buffer
     O_APPEND : If you want to append to the file, setting the file position to the current end of file with SEEK_END is not sufficient. Another process may write more data after you seek but before you write, extending the file so the position you write onto clobbers their data. Instead, use the O_APPEND operating mode
    */
    
    return cjTrue;
}

void cjIOSFile::Close()
{
    if(IsValid())
	{
		::close(m_handle);
		m_handle = INVALID_FILE_HANDLE_VAL;
	}

	cjSuper::Close();
}

void cjIOSFile::FlushBuffer()
{
    if(!IsValid())
        return;
    
    // POSIX doesn't define a behavior for fsync() on a file descriptor that was opened O_RDONLY
    if(m_openFlag & EFF_WRITE)
        ::fsync(m_handle);
    
/*
       fsync() transfers ("flushes") all modified in-core data of (i.e.,
       modified buffer cache pages for) the file referred to by the file
       descriptor fd to the disk device (or other permanent storage device)
       so that all changed information can be retrieved even after the
       system crashed or was rebooted.  This includes writing through or
       flushing a disk cache if present.  The call blocks until the device
       reports that the transfer has completed.  It also flushes metadata
       information associated with the file (see stat(2)).

       Calling fsync() does not necessarily ensure that the entry in the
       directory containing the file has also reached disk.  For that an
       explicit fsync() on a file descriptor for the directory is also
       needed.

       fdatasync() is similar to fsync(), but does not flush modified
       metadata unless that metadata is needed in order to allow a
       subsequent data retrieval to be correctly handled.  For example,
       changes to st_atime or st_mtime (respectively, time of last access
       and time of last modification; see stat(2)) do not require flushing
       because they are not necessary for a subsequent data read to be
       handled correctly.  On the other hand, a change to the file size
       (st_size, as made by say ftruncate(2)), would require a metadata
       flush.

       The aim of fdatasync() is to reduce disk activity for applications
       that do not require all metadata to be synchronized with the disk.
*/

}

cjBool cjIOSFile::ReWind()
{
    if(!IsValid())
        return cjFalse;

	return SetPos(0);
}

cjStreamOffset cjIOSFile::GetPos()
{
    if(!IsValid())
        return (cjStreamOffset)-1;

    if(m_openFlag & EFF_WRITE)
        FlushBuffer();
    
    return (cjStreamOffset)::lseek(m_handle, 0, SEEK_CUR);
}

cjBool cjIOSFile::SetPos(cjStreamOffset pos)
{
    if(!IsValid())
        return cjFalse;

	if(pos == m_filePos)
		return cjTrue;
    
    if(-1 == ::lseek(m_handle, pos, SEEK_SET))
        return cjFalse;
    
	m_filePos = pos;

	return cjTrue;
}

cjStreamOffset cjIOSFile::GetSiz()
{
	if(!IsValid())
		return -1;
    
    if(m_openFlag & EFF_WRITE)
        FlushBuffer();
    
    cjStreamOffset cur = GetPos();
    
    ::lseek(m_handle, 0, SEEK_END);
    cjStreamOffset endPos = (cjStreamOffset)::lseek(m_handle, 0, SEEK_CUR);
    ::lseek(m_handle, cur, SEEK_SET);
    
    return endPos;
}

cjBool cjIOSFile::SetSiz(cjStreamOffset newSiz)
{
    if(!IsValid())
        return cjFalse;
    
    if(!(m_openFlag & EFF_WRITE))
        return cjFalse;
    
	cjStreamOffset oldPos = GetPos();
    
    if(oldPos > newSiz)
        oldPos = newSiz;

	if(!SetPos(newSiz))
		return cjFalse;
    
	if(-1 == ::ftruncate(m_handle, newSiz))
		return cjFalse;

	if(!SetPos(oldPos))
		return cjFalse;
	
	return cjTrue;
}

cjBool cjIOSFile::Read(void* readBuf, cjStreamOffset bytesToRead, cjStreamOffset* bytesReaded, 
	cjStreamOffset* readPos)
{
	if(!IsValid())
	{
		SetErr(ERR_INVALID_HANDLE);
		return cjFalse;
	}

	cjStreamOffset rstReadBytes = 0;
    
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

		rstReadBytes = ::read(m_handle, readBuf, bytesToRead);
        if(rstReadBytes == -1)
		{
			SetErr(ERR_INVALID_PARAM);
			return cjFalse;
		}
	}

	m_filePos += rstReadBytes;

	if(bytesReaded != cjNULL_PTR)
		*bytesReaded = rstReadBytes;

	if(rstReadBytes != bytesToRead)
	{
		// reached End of File
		SetErr(ERR_REACHED_EOF);
		return cjFalse;
	}

	return cjTrue;
}

cjBool cjIOSFile::Write(const void* writeBuf, cjStreamOffset bytesToWrite, cjStreamOffset* bytesWrited, 
	cjStreamOffset* writePos, cjBool doFlush)
{
	if(!IsValid())
	{
		SetErr(ERR_INVALID_HANDLE);
		return cjFalse;
	}

	cjStreamOffset rstWriteBytes = 0;

	if(bytesToWrite != 0)
	{
		if(writePos != 0)
		{
			cjStreamOffset cur = GetPos();

			if(*writePos != cur)
			{
				if(!SetPos(cur))
					return cjFalse;

				m_filePos = *writePos;
			}
		}
        
        rstWriteBytes = ::write(m_handle, writeBuf, bytesToWrite);

		if(rstWriteBytes == -1)
		{
			SetErr(ERR_INVALID_PARAM);
			return cjFalse;
		}
	}

	m_filePos += rstWriteBytes;

	if(bytesWrited != 0)
		*bytesWrited = rstWriteBytes;

	if(bytesToWrite != rstWriteBytes)
	{
		SetErr(ERR_PARTIAL_WRITE); // disk full ?
		return cjFalse;
	}

	return cjTrue;
}

