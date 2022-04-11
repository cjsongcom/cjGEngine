
#ifndef _CJRESCACHEID_H_
#define _CJRESCACHEID_H_

#include "cjEngineLibType.h"

// 64bit Quad Word ID
/*
  63						 16   15     0
	[ccccccc] [bbbbbb] [aaaaaaa]  [xxxxxx]

	x(0~15) = cjResCacheIDType 
	c,b,a   = obj pointer << 8 
	        = global increment id
*/

enum cjResCacheIDType
{
	cjRSCID_None				= 0,
	cjRSCID_RenderTarget		= 0x10,
	cjRSCID_Texture				= 0x11,
	cjRSCID_Shader				= 0x12,
    cjRSCID_VertexStreamAttrib  = 0x13,
    cjRSCID_VertexStream        = 0x14,
	cjRSCID_IndexStream			= 0x15,
    cjRSCID_Num,

	cjRSCID_MAX					= 0xff
};

//enum
//{
//	cjResCacheHashMax = 4096
//};

typedef cjUINT64	cjResCacheID;


CJENGINE_API extern cjResCacheID cjGResMakeCacheIDIndex;

inline cjResCacheID cjResMakeCacheID(cjResCacheIDType type)
{
	return type + ((cjGResMakeCacheIDIndex++) << 8);
}

//inline cjResCacheID cjResMakeCacheID(cjResCacheIDType type, cjPTR_NUMERIC ptr)
//{
//#ifdef CJ_P64
//	return (type) + ( (cjQWORD)ptr << 8 );
//#elif
//	return (type) + ( (cjQWORD)ptr << 8 );
//#endif
//}


inline cjUINT32 cjResGetHashIndex(cjResCacheID cacheID)
{
	// max 0~4095
	return ( (cjUINT32)cacheID & 0xfff  )        ^ 
		   (((cjUINT32)cacheID & 0xff00 ) >> 4 ) + 
		   (((cjUINT32)cacheID & 0xf0000) >> 16);
}

inline cjResCacheIDType cjGetResCacheType(cjResCacheID cacheID)
{
    return (cjResCacheIDType)(cacheID & 0xff);
}


template<cjResCacheIDType TResCID>
inline cjBool cjResCheckCacheIDType(cjResCacheID cacheID)
{
	return (cjResCacheIDType)(cacheID & 0xff) == TResCID ? cjTrue : cjFalse;
}

#endif

