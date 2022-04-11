
#include "cjCorePCH.h"
#include "cjString.h"

#ifdef _MSC_VER
#pragma warning(disable:4996)
// C4996 This function or variable may be unsafe. Consider using strcpy_s instead.
//       To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details
#endif


#if defined(CJ_PLATFORM_WIN32)

//
// win32
//

#define _cjstrcmp(a,b) strcmp(a,b)
#define _cjwcscmp(a,b) wcscmp(a,b)
#define _cjstricmp(a,b) _stricmp(a,b)
#define _cjwcsicmp(a,b) _wcsicmp(a,b)
#define _cjstrncmp(a,b,n) strncmp(a,b,n)
#define _cjwcsncmp(a,b,n) wcsncmp(a,b,n)
#define _cjstrnicmp(a,b,n) strnicmp(a,b,n)
#define _cjwcsnicmp(a,b,n) _wcsnicmp(a,b,n)

#define _cjstrcpy(d,s) strcpy(d,s)
#define __cjstrcpy(d,siz,s) strcpy_s(d,siz,s)
#define _cjwcscpy(d,s) wcscpy(d,s)
#define __cjwcscpy(d,siz,s) wcscpy_s(d,siz,s)
#define _cjstrncpy(d,s,siz) strncpy(d,s,siz)
#define _cjwcsncpy(d,s,siz) wcsncpy(d,s,siz)

#define _cjstrlwr(d) strlwr(d)
#define _cjwcslwr(d) wcslwr(d)
#define _cjstrupr(d) strupr(d)
#define _cjwcsupr(d) wcsupr(d)

#define _cjstrcat(d,s) strcat(d,s)
#define __cjstrcat(d,siz,s) strcat_s(d,siz,s)
#define _cjwcscat(d,s) wcscat(d,s)
#define __cjwcscat(d,siz,s) wcscat_s(d,siz,s)

#define _cjstrlen(s) strlen(s)
#define _cjwcslen(s) wcslen(s)
#define _cjstrchr(s,c) strchr(s,c)
#define _cjwcschr(s,c) wcschr(s,c)

#define _cjstrrchr(s,c) strrchr(s,c)
#define _cjwcsrchr(s,c) wcsrchr(s,c)
#define _cjstrstr(s,f) strstr(s,f)
#define _cjwcsstr(s,f) wcsstr(s,f)

// str to unsigned long
#define _cjstrtoul(s,e,radix) strtoul(s,e,radix)
#define _cjwcstoul(s,e,radix) wcstoul(s,e,radix)

#define _cjatoi(s) atoi(s)
#define _cjwtoi(s) _wtoi(s)
#define _cjatof(s) atof(s)
#define _cjwtof(s) _wtof(s)

#define _cjWCS2MBS(d,s,bufsiz)  ::WideCharToMultiByte(CP_ACP, 0, s, -1, d, bufsiz, NULL, FALSE)
#define _cjMBS2WCS(d,s,bufsiz)  ::MultiByteToWideChar(CP_ACP, 0, s, -1, d, bufsiz)


////////////////////////////////////////////////////////////////
#else
////////////////////////////////////////////////////////////////

//
// apple
//

//
// listing of non ansi functions
//
static cjChar* _cj_strlower(cjChar *str)
{
    for(cjChar *p = str; *p; p++)
        *p = (cjChar)tolower((cjINT32)*p);
    
    return(str);
}
static cjUni* _cj_wcslower(cjUni *str)
{
    for(cjUni *p = str; *p; p++)
        *p = (cjUni)tolower((cjINT32)*p);
    
    return(str);
}
static cjChar* _cj_strupper(cjChar *str)
{
    for(cjChar *p = str; *p; p++)
        *p = (cjChar)toupper((cjINT32)*p);
    
    return(str);
}
static cjUni* _cj_wcsupper(cjUni *str)
{
    for(cjUni *p = str; *p; p++)
        *p = (cjUni)toupper((cjINT32)*p);
    
    return(str);
}

#define _cjstrcmp(a,b) strcmp(a,b)
#define _cjwcscmp(a,b) wcscmp(a,b)

// strcasecmp, strncasecmp - compare two strings ignoring case
#define _cjstricmp(a,b) strcasecmp(a,b)
#define _cjwcsicmp(a,b) wcscasecmp(a,b)

#define _cjstrncmp(a,b,n) strncmp(a,b,n)
#define _cjwcsncmp(a,b,n) wcsncmp(a,b,n)

#define _cjstrnicmp(a,b,n) strncasecmp(a,b,n)
#define _cjwcsnicmp(a,b,n) wcsncasecmp(a,b,n)

#define _cjstrcpy(d,s) strcpy(d,s)
#define __cjstrcpy(d,siz,s) strcpy(d,s)
#define _cjwcscpy(d,s) wcscpy(d,s)
#define __cjwcscpy(d,siz,s) wcscpy(d,s)

#define _cjstrncpy(d,s,siz) strncpy(d,s,siz)
#define _cjwcsncpy(d,s,siz) wcsncpy(d,s,siz)

// strlwr , strupr is not ansi functions
#define _cjstrlwr(d) _cj_strlower(d)
#define __cjstrlwr(d,siz) _cj_strlower(d)
#define _cjwcslwr(d) _cj_wcslower(d)
#define __cjwcslwr(d,siz) _cj_wcslower(d)

#define _cjstrupr(d) _cj_strupper(d)
#define __cjstrupr(d,siz) _cj_strupper(d)
#define _cjwcsupr(d) _cj_wcsupper(d)
#define __cjwcsupr(d,siz) _cj_wcsupper(d)

#define _cjstrcat(d,s) strcat(d,s)
#define __cjstrcat(d,siz,s) strcat(d,s)
#define _cjwcscat(d,s) wcscat(d,s)
#define __cjwcscat(d,siz,s) wcscat(d,s)

#define _cjstrlen(s) strlen(s)
#define _cjwcslen(s) wcslen(s)
#define _cjstrchr(s,c) strchr(s,c)
#define _cjwcschr(s,c) wcschr(s,c)

#define _cjstrrchr(s,c) strrchr(s,c)
#define _cjwcsrchr(s,c) wcsrchr(s,c)
#define _cjstrstr(s,f) strstr(s,f)
#define _cjwcsstr(s,f) wcsstr(s,f)

// str to unsigned long
#define _cjstrtoul(s,e,radix) strtoul(s,e,radix)
#define _cjwcstoul(s,e,radix) wcstoul(s,e,radix)

#define _cjatoi(s) atoi(s)
#define _cjwtoi(s) _cjwcstoul(s,0,10)
#define _cjatof(s) atof(s)
#define _cjwtof(s) wcstof(s,0)

/*
 (void)setlocale(LC_ALL, "");
 * Calculate the maximum number of bytes needed to
 * store the wide character buffer in multibyte form in the
 * current code page and malloc() the appropriate storage,
 * including the terminating null.

  s = (char *) malloc( wcslen(pwcs) * MB_CUR_MAX + 1 );
  retval= wcstombs( s, pwcs, n);
  if( retval == -1) {
    * Error handle
    * s points to the multibyte character string.
*/

// The behavior of this function depends on the LC_CTYPE category of the selected C locale.

// return size of converted from wcstombs in mac is only for string, not added for null-terminator
// so adding +1 for null-terminator
#define _cjWCS2MBS(d,s,bufsiz)  wcstombs(d,s,bufsiz)+1
#define _cjMBS2WCS(d,s,bufsiz)  mbstowcs(d,s,bufsiz)+1

#endif



CJCORE_API cjINT32 cjStrcmp(const cjChar* a, const cjChar* b)
{
	return _cjstrcmp(a,b);
}
CJCORE_API cjINT32 cjStrcmp(const cjUni* a, const cjUni* b)
{
	return _cjwcscmp(a,b);
}

CJCORE_API cjINT32 cjStricmp(const cjChar* a, const cjChar* b)
{
	return _cjstricmp(a,b);
}
CJCORE_API cjINT32 cjStricmp(const cjUni* a, const cjUni* b)
{
	return _cjwcsicmp(a,b);
}

CJCORE_API cjINT32 cjStrncmp(const cjChar* a, const cjChar* b, cjINT32 len)
{
	return _cjstrncmp(a, b, len);
}
CJCORE_API cjINT32 cjStrncmp(const cjUni* a, const cjUni* b, cjINT32 len)
{
	return _cjwcsncmp(a, b, len);
}

CJCORE_API cjINT32 cjStrnicmp(const cjChar* a, const cjChar* b, cjINT32 len)
{
	return _cjstrnicmp(a, b, len);
}
CJCORE_API cjINT32 cjStrnicmp(const cjUni* a, const cjUni* b, cjINT32 len)
{
	return _cjwcsnicmp(a, b, len);
}

CJCORE_API void cjStrcpy(cjChar* dst, cjUINT32 siz, const cjChar* src)
{
	__cjstrcpy(dst, siz, src);
}
CJCORE_API void cjStrcpy(cjChar* dst, const cjChar* src)
{
	_cjstrcpy(dst, src);
}
CJCORE_API void cjStrcpy(cjUni* dst, cjUINT32 siz, const cjUni* src)
{
	__cjwcscpy(dst, siz, src);
}
CJCORE_API void cjStrcpy(cjUni* dst, const cjUni* src)
{
	_cjwcscpy(dst, src);
}
CJCORE_API cjChar* cjStrncpy(cjChar* dst, const cjChar* src, cjINT32 sizCpy)
{
	// fill remaining buff space to zero after end of string if string size is less than sizCpy
	_cjstrncpy(dst, src, sizCpy);
	dst[sizCpy-1] = 0;
	return dst;
}
CJCORE_API cjUni* cjStrncpy(cjUni* dst, const cjUni* src, cjINT32 sizCpy)
{
	_cjwcsncpy(dst, src, sizCpy);
	dst[sizCpy-1] = 0;
	return dst;
}
CJCORE_API void cjStricpy(cjChar* dst, cjUINT32 siz, const cjChar* src, cjINT32 lwr)
{
	__cjstrcpy(dst, siz, src);

	if(lwr)
		_cjstrlwr(dst);
	else
		_cjstrupr(dst);
}
CJCORE_API void cjStricpy(cjUni* dst, cjUINT32 bufSiz, const cjUni* src, cjINT32 lwr)
{
	//cjugly
	__cjwcscpy(dst, bufSiz, src);

	if(lwr)
		_cjwcslwr(dst);
	else
		_cjwcsupr(dst);
}

CJCORE_API void cjStrcat(cjChar* dst, cjUINT32 siz, const char* src)
{
	__cjstrcat(dst, siz, src);
}

CJCORE_API void cjStrcat(cjUni* dst, cjUINT32 siz, const cjUni* src)
{
	__cjwcscat(dst, siz, src);
}

CJCORE_API cjINT32 cjStrlen(const cjChar* src)
{
	return (cjINT32)_cjstrlen(src);
}
CJCORE_API cjINT32 cjStrlen(const cjUni* src)
{
	return (cjINT32)_cjwcslen(src);
}
CJCORE_API cjChar* cjStrchr(cjChar* src, cjChar c)
{
	return _cjstrchr(src, c);
}
CJCORE_API cjUni* cjStrchr(cjUni* src, cjUni c)
{
	return _cjwcschr(src, c);
}
CJCORE_API cjChar* cjStrrchr(cjChar* src, cjChar c)
{
	return _cjstrrchr(src, c);
}
CJCORE_API cjUni* cjStrrchr(cjUni* src, cjUni c)
{
	return _cjwcsrchr(src, c);
}
CJCORE_API cjChar* cjStrstr(const cjChar* src, const cjChar* find)
{
	return (cjChar*)_cjstrstr(src, find);
}
CJCORE_API cjUni* cjStrstr(const cjUni* src, const cjUni* find)
{
	return (cjUni*)_cjwcsstr(src, find);
}
CJCORE_API const cjChar* cjStrrchr(const cjChar* src, cjChar c)
{
	return _cjstrrchr(src, c);
}
CJCORE_API const cjUni* cjStrrchr(const cjUni* src, cjUni c)
{
	return _cjwcsrchr(src, c);
}

/*
    ascii
	0-9  : 48(0x30) ~ 57(0x39)
	A-Z  : 65(0x41) ~ 90(0x5a)   uppercase alphabet
	     : [(91) \\(92) ](93) ^(94) _(95) '(96)
	a-z  : 97(0x61) ~ 122(0x7a)  lowercase alphabet
*/

CJCORE_API const cjChar* cjStrifind(const cjChar* src, const cjChar* find)
{
	if(!src || find)
		return NULL;

	// a~z -> (a~z) + (-32) -> A-Z
	cjChar upperCased = (*find<'a' || *find>'z') 
		? (*find) : (*find+'A'-'a'); // 'A'-'a' = -32

	cjINT32 len = cjStrlen(find++)-1;

	cjBool isAlphabetNum = cjFalse;
	cjChar c = *src++;
	while(c)
	{
		if(c>='a' && c<='z') c += 'A'-'a'; // a-z => A-Z

		// found starting of word
		// skip until find starting non-alphanumeric char
		if(!isAlphabetNum && c==upperCased && !cjStrnicmp(src, find, len))
			return src-1;
		
		isAlphabetNum = (c>='A' && c<='Z') || (c>='0' && c<='9') ? cjTrue : cjFalse;
		c = *src++;
	}
	return NULL;	
}

CJCORE_API const cjUni* cjStrifind(const cjUni* src, const cjUni* find)
{
	if(!src || !find)
		return NULL;

	// a~z -> (a~z) + (-32) -> A-Z
	cjUni upperCased = (*find<L'a' || *find>L'z') 
		? (*find) : (*find+L'A'-L'a'); // 'A'-'a' = -32

	cjINT32 len = cjStrlen(find++)-1;
	cjBool isAlphabetNum = cjFalse;
	cjUni c = *src++;
	while(c)
	{
		if(c>=L'a' && c<=L'z') c += (cjUni)(L'A'-L'a'); // a-z => A-Z

		// found starting of word
		// skip until find starting non-alphanumeric char
		if(!isAlphabetNum && c==upperCased && !cjStrnicmp(src, find, len))
			return src-1;
		
		isAlphabetNum = (c>=L'A' && c<=L'Z') || (c>=L'0' && c<=L'9') ? cjTrue : cjFalse;
		c = *src++;
	}
	return NULL;	
}

CJCORE_API cjUINT32 cjStrtoui(const cjChar* st, cjChar** en, cjINT32 radix)
{
	return (cjUINT32)_cjstrtoul( st, en ? en : 0, radix);
}

CJCORE_API cjUINT32 cjStrtoui(const cjUni* st, cjUni** en, cjINT32 radix)
{
	return (cjUINT32)_cjwcstoul( st, en ? en : 0, radix);
}

CJCORE_API cjINT32 cjAtoi(const cjChar* src)
{
	return (cjINT32)_cjatoi(src);
}

CJCORE_API cjINT32 cjAtoi(const cjUni* src)
{
	return (cjINT32)_cjwtoi(src);
}

CJCORE_API cjFLOAT cjAtof(const cjChar* src)
{
	return (cjFLOAT)_cjatof(src);
}

CJCORE_API cjFLOAT cjAtof(const cjUni* src)
{
	return (cjFLOAT)_cjwtof(src);
}


// mbcs <-> unicode

CJCORE_API cjSiz_t cjGetSizUniToMbcs(const cjUni* uni)
{
	if(!uni) 
		return 0;

	return _cjWCS2MBS(NULL, uni, 0);
}

CJCORE_API cjSiz_t cjGetSizMbcsToUni(const cjChar* ch)
{
	if(!ch)
		return 0;

	return _cjMBS2WCS(NULL, ch, 0);
}

CJCORE_API cjChar* cjUniToMbcs(cjChar* mbcs, cjUINT32 siz, const cjUni* uni)
{
	_cjWCS2MBS(mbcs, uni, siz);
	return mbcs;
}

CJCORE_API cjUni* cjMbcsToUni(cjUni* uni, cjUINT32 siz, const cjChar* mbcs)
{
	_cjMBS2WCS(uni, mbcs, siz);
	return uni;
}

CJCORE_API void cjSprintf(cjChar* dst, cjUINT32 bufSiz, const cjChar* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	cjvsnprintf(dst, bufSiz, fmt, args);

	va_end(args);
}

CJCORE_API void cjSprintf(cjUni* dst, cjUINT32 bufSiz, const cjUni* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	cjvsnwprintf(dst, bufSiz, fmt, args);

	va_end(args);
}

CJCORE_API cjBool cjTestAllCharIsAnsi(const cjUni* str)
{
	for( ; *str; str++)
	{
		if(*str < 0 || *str > 0xff)
			return cjFalse;
	}

	return cjTrue;
}


//
// cjChar <-> cjUni convert utility
//

//cjtodo
// make memorypool for cjChar2Uni, cjCUni2Char , to reduce allocating/deallocating memory
// make calling once from calling twice

// cjChar -> cjUni
cjChar2Uni::cjChar2Uni() : m_original(NULL), m_temp(NULL) {}
cjChar2Uni::cjChar2Uni(const cjChar* ch) : m_original(ch), m_temp(NULL)
{
	if(!m_original || !*m_original)
		return;

	if(!m_temp)
	{	
		cjSiz_t sizLen = GetConvLen(m_original);
		m_temp = cjMemAlloc(sizeof(cjUni)*sizLen);
		Conv((cjUni*)m_temp, m_original, (cjUINT32)sizLen);
		// ((cjUni*)m_temp)[sizLen-1] = NULL;
	}
}

cjChar2Uni::~cjChar2Uni() {  if(m_temp) { cjMemDeAlloc(m_temp); m_temp = NULL; } }
cjChar2Uni::operator const cjUni*() const { return (cjUni*)m_temp; }
const cjUni* cjChar2Uni::operator *() const { return (cjUni*)m_temp; }
cjUINT32 cjChar2Uni::GetConvLen(const cjChar* ch)
{
	if(!ch)
		return 0;

	return (cjUINT32)cjGetSizMbcsToUni(ch);
}
cjUni* cjChar2Uni::Conv(cjUni* out, const cjChar* in, cjUINT32 outBufLen)
{
	return cjMbcsToUni(out, (cjUINT32)outBufLen, in);
}


// cjUni -> cjChar
cjUni2Char::cjUni2Char() : m_original(NULL), m_temp(NULL) {}
cjUni2Char::cjUni2Char(const cjUni* uni) : m_original(uni), m_temp(NULL) 
{
	if(!m_original || !*m_original)	return;
	if(!m_temp)
	{
		cjSiz_t sizLen = cjGetSizUniToMbcs(m_original);
		m_temp = cjMemAlloc(sizeof(cjChar)*sizLen);
		cjUniToMbcs((cjChar*)m_temp, (cjUINT32)sizLen, m_original);
		//((cjChar*)m_temp)[sizLen-1] = NULL;
	}
}
cjUni2Char::~cjUni2Char() { if(m_temp) { cjMemDeAlloc(m_temp); m_temp = NULL; } }
cjUni2Char::operator const cjChar*() const { return (cjChar*)m_temp; }
const cjChar* cjUni2Char::operator *() const { return (cjChar*)m_temp; }


//
// cjString
//



CJCORE_API cjFStream& operator<<(cjFStream& stream, cjString& CJSTR)
{
	cjINT32 serSiz = 0; 
	cjBool allAnsiChar = cjTrue;

	if(stream.IsLoading())
	{
		stream << cjFStreamCmprI32(serSiz);

		if(stream.m_maxSerializeSiz && cjAbs(serSiz) > stream.m_maxSerializeSiz)
		{
			stream.m_err = cjFStream::ERR_READ_OUT_OF_SIZ;
			return stream;
		}

		// (+1 for null string)
		CJSTR.ReSize(cjAbs(serSiz)+1); // +1 for null
		
		if(serSiz > 0)
		{
			// read as compressed unicode
			cjINT32 i=0;
			for(; i<CJSTR.Len(); i++)
			{			
				cjINT32 uch;
				stream << cjFStreamCmprI32(uch);
				CJSTR[i] = (cjUni)uch;
			}
			CJSTR[i] = (cjUni)0; // add null
		}
		else if(serSiz < 0)
		{
			// read as ansi
			cjINT32 i=0;
			for(; i<CJSTR.Len(); i++)
			{			
				cjBYTE ch;
				stream << *((cjBYTE*)&ch);
				CJSTR[i] = ch;
			}

			CJSTR[i] = 0;  // add null
		}
		else
		{
			CJSTR.Clear();
		}
	}
	else
	{	
		// save
		serSiz = CJSTR.Len(); // except null

		if(serSiz > 0)
		{
			allAnsiChar = CJSTR.TestAllCharIsAnsi();

			if(allAnsiChar)
				serSiz = -serSiz;
		}

		stream << cjFStreamCmprI32(serSiz);

		if(serSiz > 0)
		{
			// write as compressed unicode
			for(cjINT32 i=0; i<CJSTR.Len(); i++)
			{			
				cjINT32 uch = (cjINT32)CJSTR[i];
				stream << cjFStreamCmprI32(uch);
			}
		}
		else
		{
			// write as ansi
			for(cjINT32 i=0; i<CJSTR.Len(); i++)
			{			
				cjUni uni = CJSTR[i];
				cjAssert(uni < 0x100);
				stream << *((cjBYTE*)&uni);
			}
		}
	}

	return stream;
}


//
//CJCORE_API cjFStream& operator<<(cjFStream& stream, cjBaseString<cjUni>& USTR)
//{
////	// 
////	cjINT32 serSiz = 0;	//  pure ansi(0~255), 
////	
////	// if string is pure alphabeted , save ansi character only (1byte)
////	// else string is mixed, save uni character - (compressed char-4byte)
////
////	if(stream.IsLoading())
////	{
////		serSiz = cjStr.Count();
////	}
////	else
////	{
////		cjIsPureAnsiStr()
////		serSiz = cjStr().Count();
////
////
////	}
////	
////	stream << cjFStreamCmprI32(serializeStrSiz);
////
////	if(stream.IsLoading())
////	{
////		if(stream.m_maxSerializeSiz && (serializeStrSiz > stream.m_maxSerializeSiz))
////		{
////			stream.m_err = cjFStream::ERR_READ_OUT_OF_SIZ;
////			return stream;
////		}
////
////		cjStr.m_maxElement = cjStr.m_numElement = serializeStrSiz;
////
////#ifdef CJUNICODE
////		cjStr.ReAlloc(sizeof(cjUni));
////
////		for(cjINT32 i=0; i<cjStr.Count(); i++)
////		{
////			cjChar ch;
////			stream << *(cjBYTE*)&ch;
////			cjStr(i) = FromAnsi(ch);
////		}
////
////#else 
////		cjStr.ReAlloc(sizeof(cjChar));
////
////		cjUni uch; 
////		stream << uch; 
////		cjStr(i) = FromUnicode(uch);
////
////#endif
////		
////		if(serializeStrSiz>=0)
////		{
////		}
////		else
////		{
////			if(stream.IsLoading() && cjStr.Count() == 1)
////				cjStr.RemoveAll();
////		}
////	}
////	else
////	{
////		// saving
////#ifdef CJUNICODE
//////		for(cjINT32 i=0; i<cjStr.Count(); i++)
////			stream << uch;
////#else
////		for(cjINT32 i=0; i<cjStr.Count(); i++)
////		{ 
////			cjChar ch=ToAnsi(cjStr(i)); 
////			stream << *(cjBYTE*)&ch;
////		}
////#endif
////	}
////
//	return stream;
//}

