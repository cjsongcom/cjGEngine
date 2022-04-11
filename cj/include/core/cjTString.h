
#ifndef _CJTSTRING_H_
#define _CJTSTRING_H_

#include "cjTAry.h"

#include "cjString.h"


	// case sensitive
struct CJCORE_API cjStringUtil
{
	template <class T>
	inline static cjINT32 Cmp(T a, T b) { return cjStrcmp(a, b); }

	template <class T>
	inline static cjUINT32 GetHash(T a) { return cjStrHash(a); }
};

	// case insensitive
struct CJCORE_API cjStringIUtil
{
	template <class T>
	inline static cjINT32 Cmp(T a, T b) { return cjStricmp(a, b); }

	template <class T>
	inline static cjUINT32 GetHash(T a) { return cjStriHash(a); }
};


//template<class T, class TSensitive=cjStringUtil >
//class cjBaseString : protected cjTAry<T>
//{
//	CJTYPEINFO_SUPPORT_DECL(cjBaseString<T>)
//
//private:
//	cjBaseString(cjINT32 srcSiz, const T* src) : cjTAry<T>(srcSiz ? srcSiz+1 : 0)
//	{
//		if(srcSiz)
//			cjStrncpy(&(*this)(0), src, srcSiz+1);
//	}
//
//public:
//	cjBaseString() : cjTAry<T>() {}
//
//	cjBaseString(const cjBaseString<T>& rhs) : cjTAry<T>(rhs.m_numElement)
//	{
//		if(cjTAry<T>::m_numElement)
//			cjMemCpy(&(*this)(0), &rhs(0), rhs.m_numElement * sizeof(T));
//	}
//	cjBaseString(const T* str) : cjTAry<T>(str ? cjStrlen(str)+1 : 0)
//	{
//		if(cjTAry<T>::m_numElement)
//			cjMemCpy(&(*this)(0), str, cjTAry<T>::m_numElement * sizeof(T));
//	}
//
//	~cjBaseString()
//	{
//		Clear();		
//	}
//
//	void Clear()
//	{
//		// ensure to calling destructor when element has constructor
//		cjTAry<T>::RemoveAll();
//	}
//
//	// operators
//	cjBaseString<T>& operator=(const T* str)
//	{
//		if(!cjTAry<T>::m_data || (&(*this)(0) != str))
//		{
//			cjTAry<T>::m_numElement = cjTAry<T>::m_maxElement = *str ? cjStrlen(str) + 1 : 0;
//
//			cjTAry<T>::ReAlloc(sizeof(T));
//
//			if(cjTAry<T>::m_numElement)
//				cjMemCpy(&(*this)(0), str, cjTAry<T>::m_numElement * sizeof(T));
//		}
//
//		return *this;
//	}
//
//	cjBaseString<T>& operator=(const cjBaseString<T>& rhs)
//	{
//		if(this != &rhs)
//		{
//			cjTAry<T>::m_numElement = cjTAry<T>::m_maxElement = rhs.Num();
//
//			cjTAry<T>::ReAlloc(sizeof(T));
//
//			if(cjTAry<T>::m_numElement)
//				cjMemCpy(&(*this)(0), *rhs, cjTAry<T>::m_numElement * sizeof(T));
//		}
//
//		return *this;
//	}
//
//	T& operator[](cjINT32 i)
//	{
//		return ((T*)cjTAry<T>::m_data)[i];
//	}
//
//	const T& operator[](cjINT32 i) const
//	{
//		return ((T*)cjTAry<T>::m_data)[i];
//	}
//
//	const T* operator*() const
//	{
//		return cjTAry<T>::m_numElement ? &(*this)(0) : (T*)"";
//	}
//
//	const T* GetStr() const
//	{
//		return cjTAry<T>::m_numElement ? &(*this)(0) : (T*)"";
//	}
//
//	operator cjBool() const
//	{
//		return cjTAry<T>::m_numElement;
//	}
//
//#ifdef CJUNICODE
//	operator const cjUni*() const {	return GetStr(); }
//#else
//	operator const cjChar*() const { return GetStr(); }
//#endif
//
//	cjBaseString<T>& operator+=(const T* str)
//	{
//		if(str && *str != (T)(0))
//		{
//			if(cjTAry<T>::m_numElement)
//			{
//				cjINT32 index = cjTAry<T>::m_numElement-1; // without null terminating
//				cjTAry<T>::PushEmpty(cjStrlen(str));
//				cjStrcpy(&(*this)(index), str);
//			}
//			else
//			{
//				cjTAry<T>::PushEmpty(cjStrlen(str)+1); // with null
//				cjStrcpy(&(*this)(0), str);
//			}
//		}
//
//		return *this;
//	}
//
//	cjBaseString<T>& operator+=(const cjBaseString<T>& rhs)
//	{
//		return operator+=(*rhs);
//	}
//
//	cjBaseString<T> operator+(const T* str)
//	{
//		return cjBaseString<T>(*this) += str;
//	}
//
//	cjBaseString<T> operator+(const cjBaseString<T>& rhs)
//	{
//		return operator+(*rhs);
//	}
//
//	void SetStrSiz(cjINT32 len, cjBool fillZero=cjFalse)
//	{
//		cjAry::SetSiz(len+1, sizeof(T));// with null
//
//		if(fillZero)
//			cjTAry<T>::Fill(0);
//
//		(*this)[len] = 0;
//	}
//
//
//	//
//	// less, greater, equal
//	//
//		// <=
//	bool operator<=(const T* str) const
//	{
//		return !(TSensitive::Cmp(**this, str) > 0);
//	}
//	bool operator<=(const cjBaseString<T>& rhs) const
//	{
//		return !(TSensitive::Cmp(**this, *rhs) > 0);
//	}
//		// <
//	bool operator<(const T* str) const
//	{
//		return TSensitive::Cmp(**this, str) < 0;
//	}
//
//	bool operator<(const cjBaseString<T>& rhs) const
//	{
//		return TSensitive::Cmp(**this, *rhs) < 0;
//	}
//		// >=
//	bool operator>=(const T* str) const
//	{
//		return !(TSensitive::Cmp(**this, str) < 0);
//	}
//	bool operator>=(const cjBaseString<T>& rhs) const
//	{
//		return !(TSensitive::Cmp(**this, *rhs) < 0);
//	}
//		// >
//	bool operator>(const T* str) const
//	{
//		return TSensitive::Cmp(**this, str) > 0;
//	}
//	bool operator>(const cjBaseString<T>& rhs) const
//	{
//		return TSensitive::Cmp(**this, *rhs) > 0;
//	}
//		// ==
//	bool operator==(const T* str) const
//	{
//		return TSensitive::Cmp(**this, str) == 0;
//	}
//	bool operator==(const cjBaseString<T>& rhs) const
//	{
//		return TSensitive::Cmp(**this, *rhs) == 0;
//	}
//		// !=
//	bool operator!=(const T* str) const
//	{
//		return TSensitive::Cmp(**this, str) != 0;
//	}
//
//	bool operator!=(const cjBaseString<T>& rhs) const
//	{
//		return TSensitive::Cmp(**this, *rhs) != 0;
//	}
//
//	// case-insensitive, no case, stricmp
//	cjBool CmpNoCase(const T* str) const
//	{
//		return cjStringIUtil::Cmp(**this, str) == 0 ? cjTrue : cjFalse;
//	}
//
//	//cjINT32 Len()
//	//{
//	//	return cjTAry<T>::m_numElement;
//	//}
//
//	cjINT32 Len() const
//	{
//		return Count() ? Count() -1 : 0;
//	}
//
//
//	// Lower, Upper
//	void ToLower()
//	{
//		for(cjINT32 i=0; i<cjTAry<T>::m_numElement; i++)
//			operator[](i) = cjCharLower(operator[](i));
//	}
//
//	void ToUpper()
//	{
//		for(cjINT32 i=0; i<cjTAry<T>::m_numElement; i++)
//			operator[](i) = cjCharUpper(operator[](i));
//	}
//
//	inline cjUINT32 GetHash() const
//	{
//		return TSensitive::GetHash(**this);
//	}
//
//	inline static cjUINT32 GetHash(const cjBaseString<T>& src)
//	{
//		return src.GetHash();
//	}
//    
//    cjINT32 Import(const T* st, const T* en=NULL)
//    {
//        cjINT32 importLen = en ? ((cjINT32)((en - st) / sizeof(T)))+1 : cjStrlen(st);
//    
//        SetStrSiz(importLen);
//        cjMemCpy(&(*this)(0), st, importLen * (cjINT32)sizeof(T));
//        
//        if(en)
//            operator[](importLen-1) = NULL;
//        
//        return importLen;
//    }
//    
//	// Format
//	static cjBaseString<T> Format(const T* fmt, ...)
//	{
//		va_list args;
//		va_start(args, fmt);
//
//		const static cjUINT32 bufSiz = 2048;
//		T buf[bufSiz]; buf[0] = NULL;
//
//#ifdef CJUNICODE
//		cjvsnwprintf(buf, bufSiz, fmt,args);
//#else
//		cjvsnprintf(buf, bufSiz, fmt,args);
//#endif
//		va_end(args);
//
//		if(buf[0]!=NULL)
//			return cjBaseString<T>(buf);
//		else
//			return cjBaseString();
//	}
//    
////   	inline static void* operator new(size_t, cjBaseString<T>* mem, cjTypeInfoActMemTag)
//  //  {
//    //    return (void*)mem;
//    //}
//    
//	//inline static void operator delete(void* dst, cjBaseString<T>* mem, cjTypeInfoActMemTag)
//   // {
//   // }
//
//	cjBool TestAllCharIsAnsi() const
//	{ 
//		if(cjTAry<T>::m_numElement == 0) 
//			return cjTrue;
//
//#ifdef CJUNICODE
//		cjUni* str = &(*this)(0);
//
//		for( ; *str; str++)
//		{
//			if(*str < 0 || *str > 0xff)
//				return cjFalse;
//		}
//#endif
//		return cjTrue;
//	}
//
//	cjBaseString<T> Left(cjINT32 siz) const
//	{
//		return cjBaseString<T>(cjClamp(siz, (cjINT32)0, (cjINT32)Len()), **this);
//	}
// 
//};


/*
   cjString

   ""	 => NULL , num = max = 0
   "abc" => 0x61 0x00 | 0x62 00 | 0x63 00 | 00 00 , num=max=4
*/

class CJCORE_API cjStringW : public cjTAry<cjUni>
{
	CJTYPEINFO_SUPPORT_DECL(cjStringW)

public:
	typedef cjUni T;

#ifdef _CLS_NAME
#error "_CLS_NAME is alreay defined.."
#endif
		
	#define _CLS_NAME	cjStringW
#include "cjTString_StringBody.h"
	#undef _CLS_NAME	
};

typedef cjStringW cjString;

class CJCORE_API cjStringA : public cjTAry<cjChar>
{
	CJTYPEINFO_SUPPORT_DECL(cjStringA)

public:
	typedef cjChar T;

#ifdef _CLS_NAME
#error "_CLS_NAME is alreay defined.."
#endif
	
#define _CLS_NAME	cjStringA
#include "cjTString_StringBody.h"
#undef _CLS_NAME

};


//typedef cjBaseString<char>				cjStringA;
//typedef cjBaseString<wchar_t>			cjStringW;

//#ifdef CJUNICODE
//typedef cjBaseString<wchar_t>			cjString;
//#else
//typedef cjBaseString<char>				cjString;
//#endif

//typedef cjBaseString<cjNameChar>		cjNameString;

//CJTYPEINFO_SUPPORT_IMPL(cjStringA);
//CJTYPEINFO_SUPPORT_IMPL(cjStringW);

//typedef cjBaseString		cjNameString;
//typedef cjBaseString		cjString;

typedef cjString			cjNameString;

CJTYPEINFO_SUPPORT_IMPL(cjString);
CJTYPEINFO_SUPPORT_IMPL(cjStringA);

#endif
