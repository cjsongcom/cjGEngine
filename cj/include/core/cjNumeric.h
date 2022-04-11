
#ifndef _CJNUMERIC_H_
#define _CJNUMERIC_H_

#include <math.h>

//typedef cjSQWORD                          cjStreamBigOffset;
#define CJ_USE_64BIT_STREAM

#ifdef CJ_USE_64BIT_STREAM

//int64 = ~ +9,223,372,036,854,775,807(hex 0x7FFFFFFFFFFFFFFF) = ..
typedef cjINT64                             cjStreamOffset;

#else

//int32 = -2,147,483,647 ~ +2,147,483,647(hex 0x7FFFFFFF) = 2.1gb
typedef cjINT32                             cjStreamOffset;

#endif

#define cjOffset64To(b,lo,hi)			{lo=(cjOffset)b; hi=(cjOffset)(b >>32);}
#define cjToOffset64(lo,hi)				(((cjBigOffset)hi << 32) | (cjBigOffset)lo)


typedef double								cjDouble;

#define CJ_PI	3.14159265358979323846f
#define CJ_HALF_PI (CJ_PI * 0.5f)
#define CJ_2XPI (3.14159265358979323846 * 2.0)

#define CJ_UCHAR_MAX				0xff
#define CJ_CHAR_MIN					(-128)
#define CJ_CHAR_MAX					127

const cjUINT32  CJ_UINT_MAX			= UINT_MAX;

const cjINT32   CJ_INT_MAX			= INT_MAX;
const cjINT32   CJ_INT_INVALID		= -INT_MAX;

const cjSWORD	CJ_SHORT_MIN		= (-32768);
const cjSWORD	CJ_SHORT_MAX		= 32767;

#ifdef CJ_PLATFORM_WIN32

const cjFLOAT	CJ_FLT_MAX			= FLT_MAX;
const cjFLOAT	CJ_FLT_INFINITY		= FLT_MAX;
const cjFLOAT	CJ_FLT_INVALID		= -FLT_MAX;

const cjFLOAT	CJ_FTIME_INFINITY	= FLT_MAX;
const cjFLOAT	CJ_FTIME_INVALID	= -FLT_MAX;
const cjFLOAT	CJ_FLOAT_INVALID	= -FLT_MAX;

#else

const cjFLOAT	CJ_FLT_MAX			= __FLT_MAX__;
const cjFLOAT	CJ_FLT_INFINITY		= __FLT_MAX__;
const cjFLOAT	CJ_FLT_INVALID		= -__FLT_MAX__;

const cjFLOAT	CJ_FTIME_INFINITY	= __FLT_MAX__;
const cjFLOAT	CJ_FTIME_INVALID	= -__FLT_MAX__;
const cjFLOAT	CJ_FLOAT_INVALID	= -__FLT_MAX__;

#endif

enum
{
    cjMAXBYTE		= 0xff,
    cjMAXWORD		= 0xffff,
//  cjMAXDWORD		= 0xffffffff,
	cjMAXUINT32		= 0xffffffff,
    cjMAXSBYTE		= 0x7f,
    cjMAXSWORD		= 0x7fff,
    cjMAXINT		= 0x7fffffff
};

#define cj_FLOAT_MAX FLT_MAX

//#define cjBigOffsetTo(b,lo,hi)   {lo=(cjOffset)b; hi=(cjOffset)(b >>32);}
//#define cjToBigOffset(lo,hi)     (((cjBigOffset)hi << 32) | (cjBigOffset)lo)

#define cjDegree2Rad(degree)     ((CJ_PI/180.0f)*degree)
#define cjRad2Degree(rad)        ((180.0f/CJ_PI)*rad)


template<class T, class TCh >
struct cjRectParser
{
	void operator ()(TCh* inTxt, T& l, T& t, T& r, T& b) const { }
};

template<>
struct cjRectParser<cjINT32, const char >
{
	void operator ()(const char* inTxt, cjINT32& l, cjINT32& t, cjINT32& r, cjINT32& b) const
	{
#ifdef _MSC_VER
        sscanf(inTxt, "%d,%d,%d,%d", &l,&t,&r,&b);
#else
        sscanf(inTxt, "%d,%d,%d,%d", &l,&t,&r,&b);
#endif
	}
};


template <class T>
inline T cjWrap2XPI(T& val)
{
	if(val > (T)CJ_2XPI) while(val > (T)CJ_2XPI) val -= (T)CJ_2XPI;
	else if(val < (T)0) while(val < (T)0) val += (T)CJ_2XPI;

	return val;
}

template <class T> 
struct cjRect
{
    inline cjRect (T left = T(0), T right = T(0), T top = T(0), T bottom = T(0))
    {
        m_left = left;
        m_right = right;
        m_top = top;
        m_bottom = bottom;
    }

    T m_left, m_right, m_top, m_bottom;

	inline T GetWidth() const { return m_right > m_left ? (m_right - m_left) : (m_left - m_right); }
	inline T GetHeight() const { return m_top > m_bottom ? (m_top - m_bottom) : (m_bottom - m_top); }

	bool operator==(const cjRect<T>& rhs) const
	{
		return m_left == rhs.m_left && m_right == rhs.m_right 
			&& m_top == rhs.m_top && m_bottom == rhs.m_bottom;
	}

	bool operator!=(const cjRect<T>& rhs) const
	{
		return !operator==(rhs);
	}
};

template<class T>
struct cjTSiz
{
	cjTSiz() {}
	cjTSiz(const T& w, const T& h) : m_w(w), m_h(h) {}
	T m_w, m_h;
};

typedef cjTSiz<cjFLOAT> cjFSiz;
typedef cjTSiz<cjINT32> cjSiz;

template<class T>
struct cjPoint
{
	cjPoint() {}
	cjPoint(const T& x, const T& y) : m_x(x), m_y(y) {}
	T m_x, m_y;
};


template<class T>
inline T cjAbs(const T a)
{
	return a >= (T)0 ? a : -a;
}

template<class T>
inline T cjMin(const T a, const T b)
{
	return a <= b ? a : b;
}

template<class T>
inline T cjMax(const T a, const T b)
{
	return a >= b ? a : b;
}

template<class T>
inline T cjClamp(const T val, const T minV, const T maxV)
{
	if(val < minV) return minV;
	if(val > maxV) return maxV;

	return val;
}

const cjFLOAT cjFloatCloseTestEpsilon = 0.0001f; // 1e-04f
inline cjBool cjFloatCloseTest(cjFLOAT a, cjFLOAT b, cjFLOAT ep=cjFloatCloseTestEpsilon)
{
	return ((cjFLOAT) fabs(a - b) <= ep) ? cjTrue : cjFalse;
}


//
// inline functions
//
inline cjFLOAT cjSqrt(cjFLOAT v) { return cjFLOAT(sqrt(v)); }
inline cjFLOAT cjTan(cjFLOAT v)	 { return cjFLOAT(tan(v));  }

inline void cjSinCosf(cjFLOAT rad, cjFLOAT& rstSin, cjFLOAT& rstCos)
{
    
#if defined(CJ_PLATFORM_WIN32) && defined(CJ_P32)
	cjFLOAT s=0.0, c=0.0;

	__asm fld rad
    __asm fsincos
    __asm fstp c
    __asm fstp s
	rstSin = s;
	rstCos = c;
#else
	//cjopti - need to 64bit inline assembly
	rstSin = sin(rad);
	rstCos = cos(rad);
#endif
}


#define cjExp(x) exp(x)
#define cjLoge(x) log(x)

// fmod (5.3,    2) = 1.300000
// fmod (18.5, 4.2) = 1.700000
#define cjFMod(x,y) fmod(x,y)	// fmod

#define cjSin(x) sin(x)
#define cjASin(x) asin(x)
#define cjCos(x) cos(x)
#define cjACos(x) acos(x)
#define cjTan(x) tan(x)
#define cjATan(x) atan(x)
#define cjATan2(x,y) atan2(x,y)
#define cjPow(x,y) pow(x,y)
#define cjRand() rand()
#define cjRandInit(x) srand(x)
#define cjFrand() ((cjFLOAT) (rand() / (cjFLOAT) RAND_MAX))
#define cjSqrt(x) sqrt(x)
#define cjFloor(x) ((cjINT32)floor(x))
#define cjFloorF(x) (floor(x))
#define cjCeil(x) ((cjINT32)ceil(x))
#define cjCeilF(x) (ceil(x))
#define cjRound(x) ((cjINT32)floor(x + 0.5f))
#define cjFractional(x) (x - floor(x))


inline cjFLOAT cjGetFMODTime(cjFLOAT etime, cjFLOAT duration)
{
	cjFLOAT f = floorf(etime / duration);
	cjFLOAT l = f * duration;

	return etime - l;
}

inline cjFLOAT cjFastATan2(cjFLOAT y, cjFLOAT x)
{
    if(x == 0.0f && y == 0.0f)
        return 0.0f;

    cjFLOAT off = 0.0f;
    cjFLOAT z;

    if(cjAbs(y) > cjAbs(x))
    {        
        z = x / y;  //  |y/x| > 1 then, use 1/z

        if(z > 0.0f)
            off = CJ_HALF_PI;
        else if(z < 0.0f)
            off = -CJ_HALF_PI;
        else // z == 0.0f
            return (y > 0.0f) ? CJ_HALF_PI : -CJ_HALF_PI; // 90 and -90 degree
    }
    else
    {
        z = y / x;

		if(z == 0.0f)  // 0 or 180 degree
            return (x > 0.0f) ? 0.0f : CJ_PI;
    }

    cjFLOAT z2 = z * z;
    cjFLOAT r;

    r = 0.0208351f;
    r *= z2;
    r -= 0.0851330f;
    r *= z2;
    r += 0.1801410f;
    r *= z2;
    r -= 0.3302995f;
    r *= z2;
    r += 0.9998660f;
    r *= z;

    if(off)
        r = off - r;

    if(y < 0.0f && x < 0.0f)
        r -= CJ_PI;

    if(y > 0.0f && x < 0.0f)
        r += CJ_PI;

    return r;
}


#if defined(CJ_PLATFORM_WIN32)

    #define cjIsNan(x) (_isnan(x))

#else

    #define cjIsNan(x) (isnan(x))

#endif


inline void cjRadToSinCos(const cjFLOAT rad, cjFLOAT& rstSin, cjFLOAT& rstCos)
{
#if defined(CJ_PLATFORM_WIN32) && defined(CJ_P32)
    cjFLOAT s, c;
    __asm fld rad
    __asm fsincos
    __asm fstp c
    __asm fstp s

    rstSin = s;
    rstCos = c;
#else
	//cjopti - need to 64bit inline assembly
	rstSin = sin(rad);
	rstCos = cos(rad);
#endif
}


template<class T> inline T cjAlign(const T ptr, cjINT32 alignment)
{
	return (T)(((cjPTR_NUMERIC)ptr + alignment-1) & ~(alignment-1));
}


/*
 Get nearest POT Bit
 
 127 = 2^7 bit (7bit)
 128 = 2^7 bit (7bit)
-----------------------
 129 = 2^8 bit (8bit)
 ...
 255 = 2^8 bit (8bit)
 256 = 2^8 bit (8bit)
 -----------------------
 257 = 2^9 bit (9bit)
*/

CJCORE_API cjBYTE cjGetNearestPOTBit(cjTFLAG val);

#define cjLerp(ratio, st, en)  ((st) + ((ratio)*((en)-(st))))


//
// HalfFloat 
//

struct CJCORE_API cjHalfFloat : public cjMem
{
public:
	cjHalfFloat();
	cjHalfFloat(cjHalfFloat& rhs);
	~cjHalfFloat();

	cjHalfFloat& operator =(const cjHalfFloat& rhs);

	void Build(const cjFLOAT* src, cjUINT32 count);
	static void GetMinMaxFloat(const cjFLOAT* srcAry, cjUINT32 count, cjFLOAT& minV, cjFLOAT& maxV);
	
	cjUINT32 SaveAsFloatAry(cjFLOAT* dstFloatAry, cjUINT32 dstCount=(cjUINT32)-1);

	// HalfFloat to cjFLOAT
	static cjUINT32 ConvertToFloat(cjFLOAT* dstFloatAry, cjUINT32 dstFloatAryCount, 
		const cjSWORD* srcHalfFloatAry, cjUINT32 srcHalfFloatCount, cjFLOAT halfRangeOffset, 
		cjFLOAT halfRangeCenter);

public:
	cjSWORD*				m_halfAry;			// cjFLOAT value as half
	cjUINT32				m_count;

	cjFLOAT					m_offset;			
	cjFLOAT					m_rangeCenter;	
};


//
// cjPrimeNumberTable
//

class CJCORE_API cjPrimeNumberTable
{
public:
	static cjUINT32 GetPrimeCount();	// number of primes in table
	static cjUINT32 GetPrime(cjUINT32 index);
	static cjUINT32 GetMaxPrime();
	static cjBool GetNearestPrime(cjUINT32& rstPrime, const cjUINT32 number);
};


//
// Interpolation
//

class CJCORE_API cjScalarInterpolate
{
public:
    static cjFLOAT DoLinear(cjFLOAT rtime, cjFLOAT p, cjFLOAT q);
    static cjFLOAT DoBezier(cjFLOAT rtime, cjFLOAT p, cjFLOAT dp, cjFLOAT q, cjFLOAT dq);
    static cjFLOAT DoTCB(cjFLOAT rtime, cjFLOAT fP, cjFLOAT fPDD, cjFLOAT fQ, cjFLOAT fQDS);

    static void DoAdjustBezier(cjFLOAT fLastValue, cjFLOAT fLastTime, 
        cjFLOAT& fLastOut, cjFLOAT fNextValue, cjFLOAT fNextTime, cjFLOAT& fNextIn, 
        cjFLOAT fNewTime, cjFLOAT fNewValue, cjFLOAT& fNewIn, cjFLOAT& fNewOut);
};


#endif
