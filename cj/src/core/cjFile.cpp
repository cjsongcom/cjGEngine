
#include "cjCorePCH.h"
#include "cjFile.h"


#if defined(CJ_PLATFORM_WIN32)
	#include "w32/cjWinFile.h"
#elif defined(CJ_PLATFORM_APPLE)
    #include "apple/cjAppleFile.h"
#elif defined(CJ_PLATFORM_LINUX)
    #include "linux/cjLinuxFile.h"
#else
	#error "ERR! - No Platform Identifier is defined.."
#endif


//
// cjFileSystem
//

static cjFileSystem* gs_fileSystem = NULL;


cjFileSystem::cjFileSystem()
{
    gs_fileSystem = this;
}

cjFileSystem::~cjFileSystem()
{
    gs_fileSystem = NULL;
}

cjFileSystem* cjFileSystem::Get()
{
    return gs_fileSystem;
}

void cjFileSystem::StaticInit_cjFileSystem()
{
    if(gs_fileSystem)
        return;

#if defined(CJ_PLATFORM_WIN32)

	cjFile::ms_pfnInstanceCreatorM = &cjWinFile::CreateInstanceM;
	cjFile::ms_pfnInstanceCreatorU = &cjWinFile::CreateInstanceU;
    gs_fileSystem = new cjW32FileSystem;
    
#elif defined(CJ_PLATFORM_APPLE)
	
	cjFile::ms_pfnInstanceCreatorM = &cjIOSFile::CreateInstanceM;
	cjFile::ms_pfnInstanceCreatorU = &cjIOSFile::CreateInstanceU;
    gs_fileSystem = new cjIOSFileSystem;
    
#elif defined(CJ_PLATFORM_LINUX)
	
	cjFile::ms_pfnInstanceCreatorM = &cjLinuxFile::CreateInstanceM;
	cjFile::ms_pfnInstanceCreatorU = &cjLinuxFile::CreateInstanceU;

	gs_fileSystem = NULL;
    
#else

#endif

    cjFile::ms_fileSys = gs_fileSystem;
}

void cjFileSystem::StaticShutdown_cjFileSystem()
{
    if(!gs_fileSystem)
        return;
    
    cjFile::ms_fileSys = NULL;
    
    delete gs_fileSystem;
    gs_fileSystem = NULL;
}


//
// cjFile
//

cjFileSystem* cjFile::ms_fileSys;

cjFile::FileInstanceCreatorM cjFile::ms_pfnInstanceCreatorM;
cjFile::FileInstanceCreatorU cjFile::ms_pfnInstanceCreatorU;

cjChar cjFile::ms_basePath[cjFilePathHint_Max][cjMAX_PATH];
cjUni cjFile::ms_basePathU[cjFilePathHint_Max][cjMAX_PATH];


cjFile::cjFile() 
	: m_err(ERR_OK)
{
}

cjFile::~cjFile()
{
}

cjFile* cjFile::CreateDefer(const cjChar* path, cjTFLAG openFlag, cjFilePathHint hint, cjTFLAG shareFlag, void* param)
{
	return ms_pfnInstanceCreatorM(path, openFlag, hint, shareFlag, param);
}

cjFile* cjFile::Create(const cjChar* path, cjTFLAG openFlag, cjFilePathHint hint, cjTFLAG shareFlag, void* param)
{
	cjFile* file = ms_pfnInstanceCreatorM(path, openFlag, hint, shareFlag, param);

	if(file)
	{
		if(!file->Open())
		{
			delete file;
			file = NULL;
		}
	}

	return file;
}

void cjFile::Close()
{
	SetErr(ERR_CLOSED);
}

cjChar* cjFile::GetNameExtFromPath(cjChar* path)
{
	cjChar* c = NULL;
	c = cjStrrchr(path, '/');

	if(!c)
		c = cjStrrchr(path, '\\');

	if(c)
		return c+1;
	else
		return path;
}

cjBool cjFile::IsFileExist(const cjChar* path)
{
    return ms_fileSys->IsFileExist(path);
}

cjBool cjFile::IsFileExist(const cjUni* path)
{
	return ms_fileSys->IsFileExist(path);
}

cjBool cjFile::IsAbsolutePath(const cjChar* path)
{
#ifdef CJ_PLATFORM_WIN32

	cjChar c = *(path+1);

	if(!c || c != ':')
		return cjFalse;

	return cjTrue;

#else
    return path[0] == '/' ? cjTrue : cjFalse;
#endif
}

cjBool cjFile::IsAbsolutePath(const cjUni* path)
{
#ifdef CJ_PLATFORM_WIN32

	cjUni c = *(path+1);

	if(!c || c != L':')
		return cjFalse;

	return cjTrue;

#else
	return path[0] == L'/' ? cjTrue : cjFalse;
#endif
}

// \\, .. 
void cjFile::NormalizePath(cjChar* out, cjINT32 bufSiz, const cjChar* path, cjChar d)
{
	if(!out || !path)
		return;

	out[0] = NULL;

	const char* c = path;
	cjINT32 curPos = 0;

	while(*c && bufSiz > 0)
	{
		if(d == '/')
		{
			if(*c == '\\')
				out[curPos] = '/';
			else
				out[curPos] = *c;
		}
		else
		{
			if(*c == '/')
				out[curPos] = '\\';
			else
				out[curPos] = *c;
		}

		bufSiz--;
		curPos++;

		c++;	
	}

	if(bufSiz <= 0)
	{
		cjAssert(0 && "out index in buf..");
	}

	out[curPos] = NULL;
}

void cjFile::NormalizePath(cjUni* out, cjINT32 bufSiz, const cjUni* path, cjUni d)
{
	if(!out || !path)
		return;

	out[0] = NULL;

	const cjUni* c = path;
	cjINT32 curPos = 0;

	while(*c && bufSiz > 0)
	{
		if(d == L'/')
		{
			if(*c == L'\\')
				out[curPos] = L'/';
			else
				out[curPos] = *c;
		}
		else
		{
			if(*c == L'/')
				out[curPos] = L'\\';
			else
				out[curPos] = *c;
		}

		bufSiz--;
		curPos++;

		c++;	
	}

	if(bufSiz <= 0)
	{
		cjAssert(0 && "out index in buf..");
	}

	out[curPos] = NULL;
}

void cjFile::SetBasePath(cjFilePathHint hint, const cjChar* path)
{
	if(!path || !*path)
	{
		ms_basePath[hint][0] = NULL;
		ms_basePathU[hint][0] = NULL;
		return;
	}
	
	cjStrcpy(ms_basePath[hint], cjMAX_PATH, path);
	
	cjChar2Uni char2uni(path);
	cjStrcpy(ms_basePathU[hint], cjMAX_PATH, char2uni);
}

void cjFile::SetBasePath(cjFilePathHint hint, const cjUni* path)
{
	if(!path)
	{
		ms_basePath[hint][0] = NULL;
		ms_basePathU[hint][0] = NULL;
		return;
	}

	cjStrcpy(ms_basePathU[hint], cjMAX_PATH, path);

	cjUni2Char uni2char(path);
	cjStrcpy(ms_basePath[hint], cjMAX_PATH, uni2char);
}

cjChar* cjFile::ResolvePath(cjChar* out, cjINT32 sizBuf, const cjChar* path, cjFilePathHint hint)
{
	out[0] = NULL;

	if(!path || *path==0)
		return out;

	if(hint == cjFilePathHint_PassThru || IsAbsolutePath(path))
	{
		cjStrcpy(out, sizBuf, path);
	}
	else
	{
		const char* basePath = GetBasePath(hint);

		//cjopti
		if(basePath && *basePath)
		{
			const char* fmt = NULL;

			if(*path == '\\' || *path == '/')
				fmt = "%s%s";
			else
				fmt = "%s/%s";

			cjSprintf(out, sizBuf, fmt, basePath, path);
		}
		else
		{
			cjStrcpy(out, sizBuf, path);
		}
	}

	return out;
}

cjUni* cjFile::ResolvePath(cjUni* out, cjINT32 sizBuf, const cjUni* path, cjFilePathHint hint)
{
	out[0] = NULL;

	if(!path || *path==0)
		return out;

	if(hint == cjFilePathHint_PassThru || IsAbsolutePath(path))
	{
		cjStrcpy(out, sizBuf, path);
	}
	else
	{
		const cjUni* basePath = GetBasePathU(hint);

		//cjopti
		if(basePath && *basePath)
		{
			const cjUni* fmt = NULL;

			if(*path == L'\\' || *path == L'/')
				fmt = L"%s%s";
			else
				fmt = L"%s/%s";

			cjSprintf(out, sizBuf, fmt, basePath, path);
		}
		else
		{
			cjStrcpy(out, sizBuf, path);
		}
	}

	return out;
}


//
// cjAppFile
//

cjAppFile::cjAppFile()
{

}

cjAppFile::~cjAppFile()
{

}

cjBool cjAppFile::Open()
{

    return cjTrue;
}

void cjAppFile::Close()
{

}

const cjChar* cjAppFile::GetWriteablePath()
{
    return NULL;
}



