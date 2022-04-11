
/*
	this code list is body of class CJCORE_API cjStringU : protected cjTAry<cjUni>
	and class CJCORE_API cjStringA : protected cjTAry<cjChar>
*/

#pragma warning(push)
#pragma warning(disable : 4127)

private:
	_CLS_NAME(cjINT32 srcSiz, const T* src) : cjTAry<T>(srcSiz ? srcSiz+1 : 0)
	{	
		if(srcSiz) 
			cjStrncpy((T*)(&(*this)(0)), src, srcSiz+1);	
	}

public:
	_CLS_NAME() : cjTAry<T>() {}
	_CLS_NAME(const _CLS_NAME& rhs) : cjTAry<T>(rhs.m_numElement)
	{
		if(cjTAry<T>::m_numElement)
			cjMemCpy(&(*this)(0), &rhs(0), rhs.m_numElement * sizeof(T));
	}
	_CLS_NAME(const T* str) : cjTAry<T>(str ? cjStrlen(str)+1 : 0)
	{
		if(cjTAry<T>::m_numElement)
			cjMemCpy(&(*this)(0), str, cjTAry<T>::m_numElement * sizeof(T));
	}
	~_CLS_NAME()
	{
		Clear();		
	}

	cjAryOff Count() const { return cjTAry<T>::Count(); }

	void Clear()
	{
		// ensure to calling destructor when element has constructor
		cjTAry<T>::RemoveAll();
	}

	void ReSize(cjINT32 siz)
	{
		m_maxElement = m_numElement = siz;
		ReAlloc(sizeof(T));
	}

	// operators

	_CLS_NAME& operator=(const cjChar* str)
	{
		if(!str)
		{
			Clear();
			return *this;
		}

		if(sizeof(cjChar) == sizeof(T))
		{
			_SubOperAssign((void*)str);
		}
		else
		{
			cjChar2Uni c2u(str);
			_SubOperAssign((void*)(*c2u));
		}

		return *this;
	}

	_CLS_NAME& operator=(const cjUni* str)
	{
		if(!str)
		{
			Clear();
			return *this;
		}

		if(sizeof(cjUni) == sizeof(T))
		{
			_SubOperAssign((void*)str);
		}
		else
		{
			cjUni2Char u2c(str);
			_SubOperAssign((void*)(*u2c));
		}

		return *this;
	}

	void _SubOperAssign(void* _str)
	{
		const T* str = (const T*)_str;

		if(!cjTAry<T>::m_data || (&(*this)(0) != str))
		{
			cjTAry<T>::m_numElement = cjTAry<T>::m_maxElement = *str ? cjStrlen(str) + 1 : 0;

			cjTAry<T>::ReAlloc(sizeof(T));

			if(cjTAry<T>::m_numElement)
				cjMemCpy(&(*this)(0), str, cjTAry<T>::m_numElement * sizeof(T));
		}
	}


	_CLS_NAME& operator=(const _CLS_NAME& rhs)
	{
		if(this != &rhs)
		{
			cjTAry<T>::m_numElement = cjTAry<T>::m_maxElement = rhs.Num();

			cjTAry<T>::ReAlloc(sizeof(T));

			if(cjTAry<T>::m_numElement)
				cjMemCpy(&(*this)(0), *rhs, cjTAry<T>::m_numElement * sizeof(T));
		}

		return *this;
	}

	T& operator[](cjINT32 i)
	{
		return ((T*)cjTAry<T>::m_data)[i];
	}

	const T& operator[](cjINT32 i) const
	{
		return ((T*)cjTAry<T>::m_data)[i];
	}

	const T* operator*() const
	{
		return cjTAry<T>::m_numElement ? &(*this)(0) : (T*)"";
	}

	const T* GetStr() const
	{
		return cjTAry<T>::m_numElement ? &(*this)(0) : (T*)"";
	}

	operator cjBool() const
	{
		return cjTAry<T>::m_numElement ? cjTrue : cjFalse;
	}

	operator const T*() const {	return GetStr(); }

	_CLS_NAME& operator+=(const T* str)
	{
		if(str && *str != (T)(0))
		{
			if(cjTAry<T>::m_numElement)
			{
				cjINT32 index = cjTAry<T>::m_numElement-1; // without null terminating
				cjTAry<T>::PushEmpty(cjStrlen(str));
				cjStrcpy(&(*this)(index), str);
			}
			else
			{
				cjTAry<T>::PushEmpty(cjStrlen(str)+1); // with null
				cjStrcpy(&(*this)(0), str);
			}
		}

		return *this;
	}

	_CLS_NAME& operator+=(const _CLS_NAME& rhs)
	{
		return operator+=(*rhs);
	}

	_CLS_NAME operator+(const T* str)
	{
		return _CLS_NAME(*this) += str;
	}

	_CLS_NAME operator+(const _CLS_NAME& rhs)
	{
		return operator+(*rhs);
	}

	void SetStrSiz(cjINT32 len, cjBool fillZero=cjFalse)
	{
		cjAry::SetSiz(len+1, sizeof(T));// with null

		if(fillZero)
			cjTAry<T>::Fill(0);

		(*this)[len] = 0;
	}


	//
	// less, greater, equal
	//
	// <=
	bool operator<=(const T* str) const
	{
		return !(cjStringUtil::Cmp(**this, str) > 0);
	}
	bool operator<=(const _CLS_NAME& rhs) const
	{
		return !(cjStringUtil::Cmp(**this, *rhs) > 0);
	}
	// <
	bool operator<(const T* str) const
	{
		return cjStringUtil::Cmp(**this, str) < 0;
	}

	bool operator<(const _CLS_NAME& rhs) const
	{
		return cjStringUtil::Cmp(**this, *rhs) < 0;
	}
	// >=
	bool operator>=(const T* str) const
	{
		return !(cjStringUtil::Cmp(**this, str) < 0);
	}
	bool operator>=(const _CLS_NAME& rhs) const
	{
		return !(cjStringUtil::Cmp(**this, *rhs) < 0);
	}
	// >
	bool operator>(const T* str) const
	{
		return cjStringUtil::Cmp(**this, str) > 0;
	}
	bool operator>(const _CLS_NAME& rhs) const
	{
		return cjStringUtil::Cmp(**this, *rhs) > 0;
	}
	// ==
	bool operator==(const T* str) const
	{
		return cjStringUtil::Cmp(**this, str) == 0;
	}
	bool operator==(const _CLS_NAME& rhs) const
	{
		return cjStringUtil::Cmp(**this, *rhs) == 0;
	}
	// !=
	bool operator!=(const T* str) const
	{
		return cjStringUtil::Cmp(**this, str) != 0;
	}

	bool operator!=(const _CLS_NAME& rhs) const
	{
		return cjStringUtil::Cmp(**this, *rhs) != 0;
	}

	// case-insensitive, no case, stricmp
	cjBool CmpNoCase(const T* str) const
	{
		return cjStringIUtil::Cmp(**this, str) == 0 ? cjTrue : cjFalse;
	}

	//cjINT32 Len()
	//{
	//	return cjTAry<T>::m_numElement;
	//}

	cjINT32 Len() const
	{
		return Count() ? Count() -1 : 0;
	}

	// Lower, Upper
	void ToLower()
	{
		for(cjINT32 i=0; i<cjTAry<T>::m_numElement; i++)
			operator[](i) = cjCharLower(operator[](i));
	}

	void ToUpper()
	{
		for(cjINT32 i=0; i<cjTAry<T>::m_numElement; i++)
			operator[](i) = cjCharUpper(operator[](i));
	}

	inline cjUINT32 GetHash() const
	{
		return cjStringUtil::GetHash(**this);
	}

	inline static cjUINT32 GetHash(const _CLS_NAME& src)
	{
		return src.GetHash();
	}

	cjINT32 Import(const T* st, const T* en=NULL)
	{
		cjINT32 importLen = en ? ((cjINT32)((en - st) / sizeof(T)))+1 : cjStrlen(st);

		SetStrSiz(importLen);
		cjMemCpy(&(*this)(0), st, importLen * (cjINT32)sizeof(T));

		if(en)
			operator[](importLen-1) = NULL;

		return importLen;
	}

	// Format
	static _CLS_NAME Format(const T* fmt, ...)
	{
		va_list args;        
		va_start(args, fmt);

		const static cjUINT32 bufSiz = 2048;
		T buf[bufSiz];

		if(sizeof(T) == 1)		
			cjvsnprintf((char*)buf, bufSiz, (const char*)fmt, args);
		else
			cjvsnwprintf((wchar_t*)buf, bufSiz, (const wchar_t*)fmt, args);

		va_end(args);

		if(buf[0]!=0)
			return _CLS_NAME((const T*)buf);
		else
			return _CLS_NAME();
	}

	//   	inline static void* operator new(size_t, _CLS_NAME* mem, cjTypeInfoActMemTag)
	//  {
	//    return (void*)mem;
	//}

	//inline static void operator delete(void* dst, _CLS_NAME* mem, cjTypeInfoActMemTag)
	// {
	// }

	cjBool TestAllCharIsAnsi() const
	{ 
		if(cjTAry<T>::m_numElement == 0) 
			return cjTrue;

#ifdef CJUNICODE
		const T* str = &(*this)(0);

		for( ; *str; str++)
		{
			if(*str < 0 || 0xff < *str)
				return cjFalse;
		}
#endif
		return cjTrue;
	}

	_CLS_NAME Left(cjINT32 siz) const
	{
		return _CLS_NAME(cjClamp(siz, (cjINT32)0, (cjINT32)Len()), **this);
	}

	void AddSuffix(const T* suffix)
	{
		operator+=(suffix);
	}

	cjBool IsEmpty() const
	{
		return cjTAry<T>::IsEmpty();
	}


#pragma warning(pop) // pair of #pragma warning(push)
