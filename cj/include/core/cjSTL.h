
#ifndef _CJSTL_H_
#define _CJSTL_H_

#include <string>
#include <vector>
#include <set>
#include <map>


#if defined(__GNUC__)

//// to resolve compiling error at ndk add this below, 
//// LOCAL_CFLAGS += -D_GLIBCXX_PERMIT_BACKWARD_HASH 
//#include <ext/hash_map>   // __gnu_cxx
//
//namespace __gnu_cxx
//{  
//   // to avoid 
//   // error: specialization of 'template<class _Key> struct __gnu_cxx::hash' 
//   // in different namespace [-fpermissive]
//   // in gcc compiling at ndk
//
//template<class Key>
//struct cjHashFn : public __gnu_cxx::hash<Key> { };
//
//template<>
//struct cjHashFn<std::string> : public __gnu_cxx::hash<std::string>
//{
//    size_t
//    operator()(const std::string& s) const
//    {
//        std::hash<std::string> hash_fn;
//        return hash_fn(s);
//    }
//};
//
//template<class Key>
//struct cjHashNoCaseFn
//{
//   size_t
//    operator()(const char* s) const
//    {
//        return (size_t)cjStriHash(s);
//    }
//};
//
//} // end of namespace __gnu_cxx
//
//
//template<class Key>
//struct cjHashEqualTo : public std::equal_to<Key>
//{
//	bool operator()(const Key& _Left, const Key& _Right) const
//	{
//		return _Left == _Right;
//	}
//};
//
//template<>
//struct cjHashEqualTo<const char* > : public std::equal_to<char* >
//{
//	bool operator()(const char* _Left, const char* _Right) const
//	{
//		return strcmp(_Left, _Right) == 0;
//	}
//    
//    //    bool
//    //  operator()(const _Tp& __x, const _Tp& __y) const
//    //{ return __x == __y; }
//};
//
//template<class Key>
//struct cjHashNoCaseEqualTo
//{
//};
//
//template<>
//struct cjHashNoCaseEqualTo<const char* >
//{
//	bool operator()(const char* _Left, const char* _Right) const
//	{
//		return cjStricmp(_Left, _Right) == 0;
//	}
//};
//
//
//template<class _Key,
//         class _Tp,
//         class _HashFn = __gnu_cxx::cjHashFn<_Key>,
//         class _EqualKey = cjHashEqualTo<_Key>, // std::equal_to<_Key>,
//         class _Alloc = __gnu_cxx::allocator<_Tp> >
//class cjHashMap : public __gnu_cxx::hash_map< _Key, _Tp, _HashFn, _EqualKey, _Alloc >
//{
//public:
//};
//
//
//template<class _Key,
//         class _Tp,
//         class _HashFn = __gnu_cxx::cjHashNoCaseFn<_Key>,
//         class _EqualKey = cjHashNoCaseEqualTo<_Key>,
//         class _Alloc = __gnu_cxx::allocator<_Tp> >
//class cjNoCaseHashMap : public __gnu_cxx::hash_map< _Key, _Tp, _HashFn, _EqualKey, _Alloc >
//{
//public:
//};


//////////////////////////////////////////////////////////////////////////
#elif defined(_WIN32)
//////////////////////////////////////////////////////////////////////////

#include <hash_map> // stdext::hash_map

template<class Key>
struct cjSTLHashStrLess : public std::less<Key>
{
	bool operator()(const Key& _Left, const Key& _Right) const
	{
		return strcmp(_Left, _Right) < 0;
	}
};

template<>
struct cjSTLHashStrLess<std::string> : public std::less<std::string>
{
	bool operator()(const std::string& _Left, const std::string& _Right) const
	{
		return _Left <  _Right ? true : false;
	}
};

template<class _Kty, class _Ty, 
         class _Tr = stdext::hash_compare<_Kty, cjSTLHashStrLess<_Kty> >,
		 class _Alloc = std::allocator<std::pair<const _Kty, _Ty> > >
class cjSTLStrHashMap : public stdext::hash_map< _Kty, _Ty, _Tr, _Alloc >
{
public:
};

#endif

//#if defined(_WIN32)
//#include <hash_map>
//#elif defined(__APPLE__)
//#include <ext/hash_map>
//#else
//#include <ext/hash_map>
//#endif


//////////////////////////////////////////////////////////////////////////
#endif // end of _CJSTL_
//////////////////////////////////////////////////////////////////////////