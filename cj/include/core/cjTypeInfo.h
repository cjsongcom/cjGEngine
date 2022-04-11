
#ifndef _CJTYPEINFO_H_
#define _CJTYPEINFO_H_


class cjNullType {}; 


enum cjTypeInfoActMemTag
{
	cjTypeInfoActMemTag_CallConstructor
};

// cjTAry Support

template <class T> 
struct cjTypeInfoBase
{
public:
	typedef const T& ConstType;
	typedef T& Type;
	enum { TypeHasDestructor = 0 };

	// for ptr
	inline static void CallConstructor(void* ) {}
	inline static void CallConstructor(const void* ) {}

	inline static void CallDestructor(const T& m)
	{
#ifdef CJDEBUG
		cjMemSet((void*)m, 0xfe, sizeof(void*));
#endif
	}

	inline static void CallDestructor(T& m) 
	{
#ifdef CJDEBUG
		cjMemSet((void*)m, 0xfe, sizeof(void*));
#endif
	}
};

template <class T> struct cjTypeInfo : public cjTypeInfoBase<T>
{
};


// String concatenation using preprocessor
// The '##' preprocessing operator performs token pasting. When a macro is expanded,
// the two tokens on either side of each '##' operator are combined into a single token,
// which then replaces the '##' and the two original tokens in the macro expansion.
/*
 Consider a C program that interprets named commands. There probably needs to be a table of commands, perhaps an array of structures declared as follows:
 
 struct command
 {
 char *name;
 void (*function) (void);
 };
 
 struct command commands[] =
 {
 { "quit", quit_command },
 { "help", help_command },
 ...
 };
 It would be cleaner not to have to give each command name twice, 
 once in the string constant and once in the function name. 
 A macro which takes the name of a command as an argument can make this unnecessary. 
 
 The string constant can be created with stringification, 
 and the function name by concatenating the argument with _command. 
 Here is how it is done:
 
 #define COMMAND(NAME)  { #NAME, NAME ## _command }
 
 struct command commands[] =
 {
 COMMAND (quit),
 COMMAND (help),
 ...
 };
 */


//
//  for support cjTAry
//  to support calling constructor/destructor when new/delete operation in cjTAry
//

#define CJTYPEINFO_SUPPORT_DECL(clsName) \
	friend struct cjTypeInfo<clsName>;

// implementation of cjDeclTypeInfoCallConstructor
#define CJTYPEINFO_SUPPORT_IMPL(clsName) \
inline void* operator new(size_t, cjTypeInfoActMemTag, clsName& mem) \
		{ return (void*)&mem; } \
inline void operator delete(void* , cjTypeInfoActMemTag, clsName& ) \
		{ } \
template<> struct cjTypeInfo<clsName > : public cjTypeInfoBase<clsName > { \
	enum { TypeHasDestructor = 1 }; \
	inline static void CallConstructor(clsName& mem) { ::new(cjTypeInfoActMemTag_CallConstructor, mem) clsName(); } \
	inline static void CallDestructor(clsName& mem) { mem.~clsName(); } \
};



template<class T>
struct cjTHashFunc
{
	static cjUINT32 Hash(const T& src) { return T::GetHash(src); }
};



//
// Determine type is pointer
//

template<typename T> 
class cjPtrTypeTraits
{ 
private: 
	template<class U> struct PtrTraits
	{ 
		enum { result = cjFalse };
		typedef cjNullType Pointee; 
	}; 

	template<class U> struct PtrTraits<U*>
	{ 
		enum { result = cjTrue }; 
		typedef U Pointee; 
	}; 

	//
	// determine class member pointer type
	//
	template<class U> struct ClsMPtrTraits 
	{ 
		enum { result = false }; 
	}; 

	template<class U, class V> struct ClsMPtrTraits<U V::*> 
	{ 
		enum { result = true }; 
	}; 

public: 
	enum { isPointer = PtrTraits<T>::result };
	enum { isClassMemberPointer = ClsMPtrTraits<T>::result }; 

	typedef typename PtrTraits<T>::Pointee Pointee;
}; 

// use> const bool isPtrType = cjPtrTypeTraits<TestClass*>::isPointer; 
// pointee type = cjPtrTypeTraits<TestClass*>::Pointee


#endif

