
#ifndef _CJSTRING_H_
#define _CJSTRING_H_

inline cjChar cjCharUpper(cjChar c)   { return (c<'a' || c>'z') ? (c) : (c+'A'-'a'); }
inline cjUni cjCharUpper(cjUni c) { return (c<'a' || c>'z') ? (c) : (c+'A'-'a'); }
inline cjChar cjCharLower(cjChar c)	{ return (c<'A' || c>'Z') ? (c) : (c-'A'-'a'); }
inline cjUni cjCharLower(cjUni c) { return (c<'A' || c>'Z') ? (c) : (c-'A'-'a'); }

CJCORE_API cjINT32 cjStrcmp(const cjChar* a, const cjChar* b);
CJCORE_API cjINT32 cjStrcmp(const cjUni* a, const cjUni* b);

CJCORE_API cjINT32 cjStricmp(const cjChar* a, const cjChar* b);
CJCORE_API cjINT32 cjStricmp(const cjUni* a, const cjUni* b);

CJCORE_API cjINT32 cjStrncmp(const cjChar* a, const cjChar* b, cjINT32 len);
CJCORE_API cjINT32 cjStrncmp(const cjUni* a, const cjUni* b, cjINT32 len);

CJCORE_API cjINT32 cjStrnicmp(const cjChar* a, const cjChar* b, cjINT32 len);
CJCORE_API cjINT32 cjStrnicmp(const cjUni* a, const cjUni* b, cjINT32 len);

CJCORE_API void cjStrcpy(cjChar* dst, cjUINT32 siz, const cjChar* src);
CJCORE_API void cjStrcpy(cjChar* dst, const cjChar* src);
CJCORE_API void cjStrcpy(cjUni* dst, cjUINT32 siz, const cjUni* src);
CJCORE_API void cjStrcpy(cjUni* dst, const cjUni* src);

CJCORE_API cjChar* cjStrncpy(cjChar* dst, const cjChar* src, cjINT32 sizCpy);
CJCORE_API cjUni* cjStrncpy(cjUni* dst, const cjUni* src, cjINT32 sizCpy);

CJCORE_API void cjStricpy(cjChar* dst, cjUINT32 siz, const cjChar* src, cjINT32 lwr=0);
CJCORE_API void cjStricpy(cjUni* dst, cjUINT32 siz, const cjUni* src, cjINT32 lwr=0);

CJCORE_API void cjStrcat(cjChar* dst, cjUINT32 siz, const char* src);
CJCORE_API void cjStrcat(cjUni* dst, cjUINT32 siz, const cjUni* src);

CJCORE_API cjINT32 cjStrlen(const cjChar* src);
CJCORE_API cjINT32 cjStrlen(const cjUni* src);

CJCORE_API cjChar* cjStrchr(cjChar* src, cjChar c);
CJCORE_API cjUni* cjStrchr(cjUni* src, cjUni c);

CJCORE_API cjChar* cjStrrchr(cjChar* src, cjChar c);
CJCORE_API cjUni* cjStrrchr(cjUni* src, cjUni c);

CJCORE_API cjChar* cjStrstr(const cjChar* src, const cjChar* find);
CJCORE_API cjUni* cjStrstr(const cjUni* src, const cjUni* find);

CJCORE_API const cjChar* cjStrrchr(const cjChar* src, cjChar c);
CJCORE_API const cjUni* cjStrrchr(const cjUni* src, cjUni c);

// case insensitive find
CJCORE_API const cjChar* cjStrifind(const cjChar* src, const cjChar* find);
CJCORE_API const cjUni* cjStrifind(const cjUni* src, const cjUni* find);

// to unsigned integer, radix=10 for decimal
CJCORE_API cjUINT32 cjStrtoui(const cjChar* st, cjChar** en, cjINT32 radix=10);
CJCORE_API cjUINT32 cjStrtoui(const cjUni* st, cjUni** en, cjINT32 radix=10);

CJCORE_API cjINT32 cjAtoi(const cjChar* src);
CJCORE_API cjINT32 cjAtoi(const cjUni* src);

CJCORE_API cjFLOAT cjAtof(const cjChar* src);
CJCORE_API cjFLOAT cjAtof(const cjUni* src);

// mbcs <-> unicode
CJCORE_API cjSiz_t cjGetSizUniToMbcs(const cjUni* uni);
CJCORE_API cjSiz_t cjGetSizMbcsToUni(const cjChar* ch);

CJCORE_API cjChar* cjUniToMbcs(char* mbcs, cjUINT32 siz, const cjUni* uni);
CJCORE_API cjUni* cjMbcsToUni(cjUni* uni, cjUINT32 siz, const cjChar* mbcs);

CJCORE_API void cjSprintf(cjChar* dst, cjUINT32 bufSiz, const cjChar* fmt, ...);
CJCORE_API void cjSprintf(cjUni* dst, cjUINT32 bufSiz, const cjUni* fmt, ...);

inline cjBool cjIsAlpha(cjChar c)
{
	return (c>='a' && c<='z') || (c>='A' && c<='Z') ? cjTrue : cjFalse;
}

inline cjBool cjIsAlpha(cjUni c)
{
	return (c>=L'a' && c<=L'z') || (c>=L'A' && c<=L'Z') ? cjTrue : cjFalse;
}

inline cjBool cjIsDigit(cjChar c)
{
	return c>='0' && c<='9' ? cjTrue : cjFalse;
}

inline cjBool cjIsDigit(cjUni c)
{
	return c>=L'0' && c<=L'9' ? cjTrue : cjFalse;
}

inline cjBool cjIsAlphaNumeric(cjChar c)
{
	return (c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') ? cjTrue : cjFalse;
}

inline cjBool cjIsAlphaNumeric(cjUni c)
{
	return (c>=L'a' && c<=L'z') || (c>=L'A' && c<=L'Z') || (c>=L'0' && c<=L'9') ? cjTrue : cjFalse;
}

#if defined(CJ_PLATFORM_WIN32)

#define cjvsnprintf(dst,bufsiz,fmt,args) vsnprintf_s(dst,bufsiz,_TRUNCATE,fmt,args)
#define cjvsnwprintf(dst,bufsiz,fmt,args) _vsnwprintf_s((wchar_t*)dst,bufsiz,_TRUNCATE,(const wchar_t*)fmt,args)

#else

#define cjvsnprintf(dst,bufsiz,fmt,args) vsnprintf(dst,bufsiz,fmt,args)
#define cjvsnwprintf(dst,bufsiz,fmt,args) vswprintf(dst,bufsiz,fmt,args)

/*
    
    http://www.phonesdevelopers.com/1770033/
 
    In windows vswprintf on the return value is equal to len, but on iOS it returns -1, then finally solved, I len +1 as the second parameter on the right.  view plain
 
 void vout(wchar_t *wcs, wchar_t *fmt, ...)
 {
 va_list arg_ptr;
 
 va_start(arg_ptr, fmt);
 
 size_t len = vwprintf(fmt, arg_ptr);
 
 vswprintf(wcs, len+1, fmt, arg_ptr);  <<
 va_end(arg_ptr);
 }
*/

#endif

inline  cjBool cjTestAllCharIsAnsi(const cjChar* ) { return cjTrue; }
CJCORE_API cjBool cjTestAllCharIsAnsi(const cjUni* str); // true:0~255 , false : 256~


//
// Hash
//

// case insensitive   'A' == 'a'
// case sensitive     'A' != 'a'

// x65599 hash
// case sensitive
inline cjUINT32 cjStrHash(const cjChar* str)
{
	cjUINT32 hash = 0; if(!str) return 0;
	cjChar c;

	while( (c=*str++) != (cjChar)NULL )
		{ hash = 65599 * hash + c; }

	return hash ^ (hash >> 16);
}

inline cjUINT32 cjStrHash(const cjUni* str)
{
	cjUINT32 hash = 0; if(!str) return 0;
	cjUni c;

	while( (c=*str++) != (cjUni)NULL )
	{
		hash = 65599 * hash + c;

		// win 2byte, mac/unix 4byte
		for(cjUINT32 ui=1; ui<sizeof(cjUni); ui++)
		{
			c = c >> 8;
			hash = 65599 * hash + c;
		}
	}

	return hash ^ (hash >> 16);
}

template<class T, cjINT32 paramType=0>
struct cjStriHashCharConvert
{ 
	inline static T Convert(T value) { return value; } 
};

template<class T>
struct cjStriHashCharConvert<T, (cjINT32)1>
{ 
	inline static T Convert(T value) { return cjCharUpper(value); } 
};

template<class T>
struct cjStriHashCharConvert<T, (cjINT32)-1>
{ 
	inline static T Convert(T value) { return cjCharLower(value); } 
};


// case insensitive
inline cjUINT32 cjStriHash(const cjChar* str)
{
	cjUINT32 hash = 0; if(!str) return 0;
	cjChar c;

	while( (c=*str++) != (cjChar)NULL )
	{ 
		hash = 65599 * hash + cjCharUpper(c);
	}

	return hash ^ (hash >> 16);
}

inline cjUINT32 cjStriHash(const cjUni* str)
{
	cjUINT32 hash = 0; if(!str) return 0;
	cjUni c;

	while( (c=*str++) != (cjUni)NULL )
	{
		c = cjCharUpper(c);
		hash = 65599 * hash + c;

		// win 2byte, mac/unix 4byte
		for(cjUINT32 ui=1; ui<sizeof(cjUni); ui++)
		{
			c = c >> 8;
			hash = 65599 * hash + c;
		}
	}

	return hash ^ (hash >> 16);
}


template<>
struct cjTHashFunc<cjChar>
{
	static cjUINT32 Hash(const cjChar& src) { return cjStrHash(&src); }
};

template<>
struct cjTHashFunc<cjUni>
{
	static cjUINT32 Hash(const cjUni& src) { return cjStrHash(&src); }
};


//
// cjChar <-> cjUni convert utility
//

//cjSlow
class CJCORE_API cjChar2Uni
{
private:
	cjChar2Uni();

public:
	explicit cjChar2Uni(const cjChar* ch);
	~cjChar2Uni();

	operator const cjChar*() const { return m_original; }
	operator const cjUni*() const;

	const cjUni* operator *() const;

	static cjUINT32 GetConvLen(const cjChar* ch);
	static cjUni* Conv(cjUni* out, const cjChar* in, cjUINT32 outBufLen);

private:
	const cjChar*			m_original;
	void*					m_temp;
};

//cjSlow
class CJCORE_API cjUni2Char
{
private:
	cjUni2Char();

public:
	explicit cjUni2Char(const cjUni* ch);
	~cjUni2Char();

	operator const cjUni*() const { return m_original; }
	operator const cjChar*() const;
	const cjChar* operator *() const;

private:
	const cjUni*			m_original;
	void*					m_temp;
};


#endif
