
#ifndef _CJ3DMATH_H_
#define _CJ3DMATH_H_

#include "cjNumeric.h"


#define cjFLOAT_VAL_01x4F	1e-05	// 0.00001f
#define cjFLOAT_VAL_01x5f   1e-06	// 0.00000f
#define cjQUAT_EPSILON		(0.00001f)

class cjMatrix3;
class cjTransform;


enum cjAxis
{
	cjAxisX,
	cjAxisY,
	cjAxisZ
};


//
// cjColor / cjColorF
//

class CJCORE_API cjColor : public cjMem
{
	CJ_DECL_SERIALIZE_INLINE_BEGIN(cjColor)
		stream << d.b << d.g << d.r << d.a;
	CJ_DECL_SERIALIZE_INLINE_END(cjColor)

public:
	cjColor() {}
	explicit cjColor(cjBYTE _r, cjBYTE _g, cjBYTE _b, cjBYTE _a=255) : r(_r), g(_g), b(_b), a(_a) {}
	cjColor(const class cjColorF* color);
	cjColor& operator =(const cjColorF& rhs);

	const static cjColor BLACK;
	const static cjColor WHITE;

	cjBYTE b, g, r, a;
};


class CJCORE_API cjColorF : public cjMem
{
	CJ_DECL_SERIALIZE_INLINE_BEGIN(cjColorF)
		stream << d.b << d.g << d.r << d.a;
	CJ_DECL_SERIALIZE_INLINE_END(cjColorF)

public:
	cjColorF() {}
	explicit cjColorF(cjFLOAT _r, cjFLOAT _g, cjFLOAT _b, cjFLOAT _a=1.0f) : r(_r), g(_g), b(_b), a(_a) {}
	cjColorF(cjColor& rhs) { operator=(rhs); }

	cjColorF operator+ (const cjColorF& c) const
	{
		cjColorF rst = *this;
		rst.r += c.r; rst.g += c.g; rst.b += c.b; rst.a += c.a;
		return rst;
	}

	cjColorF operator- (const cjColorF& c) const
	{
		cjColorF rst = *this;
		rst.r -= c.r; rst.g -= c.g; rst.b -= c.b; rst.a -= c.a;
		return rst;
	}

	cjColorF& operator+= (const cjColorF& c)
	{
		r += c.r; g += c.g; b += c.b; a += c.a;
		return *this;
	}

	cjColorF& operator-= (const cjColorF& c)
	{
		r -= c.r; g -= c.g; b -= c.b; a -= c.a;
		return *this;
	}

	cjColorF operator* (cjFLOAT scalar) const
	{
		cjColorF rst = *this;
		rst.r *= scalar; rst.g *= scalar; rst.b *= scalar; rst.a *= scalar;
		return rst;
	}

	cjColorF operator* (const cjColorF &c) const
	{
		cjColorF rst = *this;
		rst.r *= c.r; rst.g *= c.g; rst.b *= c.b; rst.a *= c.a;
		return rst;
	}

	cjColorF operator/ (cjFLOAT scalar) const
	{
		cjColorF rst = *this;
		rst.r /= scalar; rst.g /= scalar; rst.b /= scalar; rst.a /= scalar;
		return rst;
	}

	cjColorF operator/ (const cjColorF &c) const
	{
		cjColorF rst = *this;
		rst.r /= c.r; rst.g /= c.g; rst.b /= c.b; rst.a /= c.a;
		return rst;
	}

	cjColorF& operator =(const cjColor& rhs)
	{
		b = (cjFLOAT)(rhs.b / 255.0f);
		g = (cjFLOAT)(rhs.g / 255.0f);
		r = (cjFLOAT)(rhs.r / 255.0f);
		a = (cjFLOAT)(rhs.a / 255.0f);		

		return *this;
	}

	bool operator ==(const cjColorF& rhs) const
		{ return b == rhs.b && g == rhs.g && r == rhs.r && a == rhs.a; }
	bool operator !=(const cjColorF& rhs) const
		{ return !operator==(rhs); }

	const static cjColorF BLACK;
	const static cjColorF WHITE;
	const static cjColorF INVALID;

	cjFLOAT b, g, r, a;
};

CJCORE_API void cjColorFSerHelper(cjSerializer& ser, cjColorF& clr, int type);


//
// Vector
//

class CJCORE_API cjVec2 : public cjMem
{
	CJ_DECL_SERIALIZE_INLINE_BEGIN(cjVec2)
		stream << d.x << d.y;
	CJ_DECL_SERIALIZE_INLINE_END(cjVec2)

public:
	cjVec2() {}
	cjVec2(cjFLOAT _x, cjFLOAT _y) : x(_x), y(_y) {}

	// a *= b + c is equivalent to a = a * (b + c)
	cjVec2& operator +=(const cjVec2& rhs) { x+= rhs.x, y+=rhs.y; return *this; }
	cjVec2& operator +=(cjFLOAT f) { x+=f, y+=f; return *this; }
	cjVec2  operator + (const cjVec2& rhs) const { return cjVec2(x+rhs.x, y+rhs.y); }
	cjVec2  operator + (cjFLOAT f) const { return cjVec2(x+f, y+f); }

	cjVec2& operator -=(const cjVec2& rhs) { x-= rhs.x, y-=rhs.y; return *this; }
	cjVec2& operator -=(cjFLOAT f) { x-=f, y-=f; return *this; }
	cjVec2  operator - (const cjVec2& rhs) const { return cjVec2(x-rhs.x, y-rhs.y); }
	cjVec2  operator - (cjFLOAT f) const { return cjVec2(x-f, y-f); }

	cjVec2& operator *=(const cjVec2& rhs) { x*=rhs.x, y*=rhs.y; return *this; }	
	cjVec2& operator *=(cjFLOAT f) { x*=f, y*=f; return *this; }
	cjVec2  operator * (const cjVec2& rhs) const { return cjVec2(x*rhs.x, y*rhs.y); }	
	cjVec2  operator * (cjFLOAT f) const { return cjVec2(x*f, y*f); }

	// cjFLOAT * vec2
	CJCORE_API friend cjVec2 operator * (cjFLOAT f, const cjVec2& rhs) { return cjVec2(rhs.x*f, rhs.y*f); }

	cjVec2& operator /=(const cjVec2& rhs) { x/=rhs.x, y/=rhs.y; return *this; }
	cjVec2& operator /=(cjFLOAT f) { x/=f, y/=f; return *this; }
	cjVec2  operator / (const cjVec2& rhs) const { return cjVec2(x/rhs.x, y/rhs.y); }
	cjVec2  operator / (cjFLOAT f) const { return cjVec2(x/f, y/f); }

	cjVec2 operator-() const { return cjVec2(-x,-y); }

	bool operator ==(const cjVec2& rhs) const
		{ return x==rhs.x && y==rhs.y ? true : false; }

	bool operator !=(const cjVec2& rhs) const
		{ return *this == rhs ? false : true; }

	cjFLOAT Dot(const cjVec2& rhs) const
		{ return x*rhs.x + y*rhs.y; }

	// cross is only valid in vector3
	//cjVec2 Cross(const cjVec2& rhs) const{}
	cjFLOAT Length() const { return cjSqrt(x*x + y*y); }

	// 1e-06(0.000001) 1e-05(0.00001). 0.0001
	void Normalize()
	{ 
		cjFLOAT len = Length();

		if(len > cjFLOAT_VAL_01x5f) 
			{ cjFLOAT f=1.0f/len; x*=f; y*=f; }
		else 
			{ x=0.0f; y=0.0f; } // len=0;
	}

public:
	cjFLOAT x, y;
	const static cjVec2 ZERO;
	const static cjVec2 UNIT;
	const static cjVec2 INVALID;
};

inline cjFLOAT cjGetLengthVec2(cjFLOAT x, cjFLOAT y)
{
	return cjSqrt(x*x+y*y); 
}


class CJCORE_API cjVec3 : public cjMem
{
	CJ_DECL_SERIALIZE_INLINE_BEGIN(cjVec3)
		stream << d.x << d.y << d.z;
	CJ_DECL_SERIALIZE_INLINE_END(cjVec3)

public:
	cjVec3() {}
	cjVec3(cjFLOAT _x, cjFLOAT _y, cjFLOAT _z) : x(_x), y(_y), z(_z) {}

	// a *= b + c is equivalent to a = a * (b + c)
	cjVec3& operator +=(const cjVec3& rhs) { x+= rhs.x, y+=rhs.y, z+=rhs.z; return *this; }
	cjVec3& operator +=(cjFLOAT f) { x+=f, y+=f, z+=f; return *this; }
	cjVec3  operator + (const cjVec3& rhs) const { return cjVec3(x+rhs.x, y+rhs.y, z+rhs.z); }
	cjVec3  operator + (cjFLOAT f) const { return cjVec3(x+f, y+f, z+f); }

	cjVec3& operator -=(const cjVec3& rhs) { x-= rhs.x, y-=rhs.y, z-=rhs.z; return *this; }
	cjVec3& operator -=(cjFLOAT f) { x-=f, y-=f, z-=f; return *this; }
	cjVec3  operator - (const cjVec3& rhs) const { return cjVec3(x-rhs.x, y-rhs.y, z-rhs.z); }
	cjVec3  operator - (cjFLOAT f) const { return cjVec3(x-f, y-f, z-f); }

	cjVec3& operator *=(const cjVec3& rhs) { x*=rhs.x, y*=rhs.y, z*=rhs.z; return *this; }	
	cjVec3& operator *=(cjFLOAT f) { x*=f, y*=f, z*=f; return *this; }
	cjVec3  operator * (const cjVec3& rhs) const { return cjVec3(x*rhs.x, y*rhs.y, z*rhs.z); }	
	cjVec3  operator * (cjFLOAT f) const { return cjVec3(x*f, y*f, z*f); }

	// cjFLOAT * vec3
	CJCORE_API friend cjVec3 operator * (cjFLOAT f, const cjVec3& rhs) { return cjVec3(rhs.x*f, rhs.y*f, rhs.z*f); }


	cjVec3& operator /=(const cjVec3& rhs) { x/=rhs.x, y/=rhs.y, z/=rhs.z; return *this; }
	cjVec3& operator /=(cjFLOAT f) { x/=f, y/=f, z/=f; return *this; }
	cjVec3  operator / (const cjVec3& rhs) const { return cjVec3(x/rhs.x, y/rhs.y, z/rhs.z); }
	cjVec3  operator / (cjFLOAT f) const { return cjVec3(x/f, y/f, z/f); }

	cjVec3 GetDiv(cjFLOAT f) const { return cjVec3(f/x, f/y, f/z); }
	
	// 1.0f / *this
	cjVec3 Get1DivMe() const { return cjVec3(1.0f/x, 1.0f/y, 1.0f/z); }

	cjVec3 operator-() const { return cjVec3(-x,-y,-z); }

	bool operator ==(const cjVec3& rhs) const
		{ return x==rhs.x && y==rhs.y && z == rhs.z ? true : false; }

	bool operator !=(const cjVec3& rhs) const
		{ return *this == rhs ? false : true; }

	cjFLOAT Dot(const cjVec3& rhs) const
		{ return x*rhs.x + y*rhs.y + z*rhs.z; }

	static cjFLOAT Dot(const cjVec3& lhs, const cjVec3& rhs)
		{ return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z; }

	cjVec3 Cross(const cjVec3& rhs) const
	{
		cjVec3 rst; 
		rst.x = y*rhs.z - z*rhs.y; rst.y = z*rhs.x - x*rhs.z; rst.z = x*rhs.y - y*rhs.x;
		return rst;
	}

	static cjVec3 Cross(const cjVec3& lhs, const cjVec3& rhs) 
	{
		cjVec3 rst; 
		rst.x = lhs.y*rhs.z - lhs.z*rhs.y; 
		rst.y = lhs.z*rhs.x - lhs.x*rhs.z; 
		rst.z = lhs.x*rhs.y - lhs.y*rhs.x;
		return rst;
	}

	cjVec3 UnitCross(const cjVec3& rhs) const
	{
		cjVec3 cross(y*rhs.z-z*rhs.y, z*rhs.x-x*rhs.z, x*rhs.y-y*rhs.x);
		cjFLOAT len = cross.Length();

		if(len > 1e-06f)
			return cross / len;
		else
			return ZERO;
	}

	cjFLOAT Length() const { return cjSqrt(x*x + y*y + z*z); }
	cjFLOAT SquareXYZ() const { return x*x + y*y + z*z; }

	// 1e-06(0.000001) 1e-05(0.00001). 0.0001
	void Normalize()
	{ 
		cjFLOAT len = Length();

		if(len > cjFLOAT_VAL_01x5f) 
		{   cjFLOAT f=1.0f/len; x *= f; y *= f; z *= f; }
		else 
			{ x=0.0f; y=0.0f; z=0.0f; } // len=0;
	}

	cjVec3& NormalizeFrom(const cjVec3& rhs)
	{
		cjFLOAT len = rhs.Length();

		if(len > cjFLOAT_VAL_01x5f) 
			{ cjFLOAT f=1.0f/len; x=rhs.x*f; y=rhs.y*f; z=rhs.z*f; }
		else 
			{ x=0.0f; y=0.0f; z=0.0f; } // len=0;

		return *this;
	}

	static cjVec3* RotateByAxis(cjVec3& out, const cjVec3& in, const cjVec3& axis, cjFLOAT deltaRad);
		
public:
	cjFLOAT x, y, z;

	static const cjVec3 ZERO;
	static const cjVec3 UNIT;
	static const cjVec3 INVALID;

	static const cjVec3 UNIT_X;
	static const cjVec3 UNIT_Y;
	static const cjVec3 UNIT_Z;
};

inline cjFLOAT cjGetLengthVec3(cjFLOAT x, cjFLOAT y, cjFLOAT z)
{
	return cjSqrt(x*x+y*y+z*z); 
}


//
// cjVector4
//

class CJCORE_API cjVec4 : public cjMem
{
	CJ_DECL_SERIALIZE_INLINE_BEGIN(cjVec4)
		stream << d.x << d.y << d.z << d.w;
	CJ_DECL_SERIALIZE_INLINE_END(cjVec4)

public:
	cjVec4() {}
	cjVec4(cjFLOAT inX, cjFLOAT inY, cjFLOAT inZ, cjFLOAT inW)
		: x(inX), y(inY), z(inZ), w(inW) { }
	cjVec4(const cjVec3& v3, cjFLOAT inW)
		: x(v3.x), y(v3.y), z(v3.z), w(inW) { }

	// a *= b + c is equivalent to a = a * (b + c)
	inline cjVec4& operator +=(const cjVec4& rhs) { x+= rhs.x, y+=rhs.y, z+=rhs.z, w+=rhs.w; return *this; }
	inline cjVec4& operator +=(cjFLOAT f) { x+=f, y+=f, z+=f, w+=f; return *this; }
	inline cjVec4  operator + (const cjVec4& rhs) const { return cjVec4(x+rhs.x, y+rhs.y, z+rhs.z, w+rhs.w); }
	inline cjVec4  operator + (cjFLOAT f) const { return cjVec4(x+f, y+f, z+f, w+f); }

	inline cjVec4& operator -=(const cjVec4& rhs) { x-=rhs.x, y-=rhs.y, z-=rhs.z, w-=rhs.w; return *this; }
	inline cjVec4& operator -=(cjFLOAT f) { x-=f, y-=f, z-=f, w-=f; return *this; }
	inline cjVec4  operator - (const cjVec4& rhs) const { return cjVec4(x-rhs.x, y-rhs.y, z-rhs.z, w-rhs.w); }
	inline cjVec4  operator - (cjFLOAT f) const { return cjVec4(x-f, y-f, z-f, w-f); }

	inline cjVec4& operator *=(const cjVec4& rhs) { x*=rhs.x, y*=rhs.y, z*=rhs.z; w*=rhs.w; return *this; }
	inline cjVec4& operator *=(cjFLOAT f) { x*=f, y*=f, z*=f; return *this; }
	inline cjVec4  operator * (const cjVec4& rhs) const { return cjVec4(x*rhs.x, y*rhs.y, z*rhs.z, w*rhs.w); }
	inline cjVec4  operator * (cjFLOAT f) const { return cjVec4(x*f, y*f, z*f, w*f); }

	inline cjVec4& operator /=(const cjVec4& rhs) { x/=rhs.x, y/=rhs.y, z/=rhs.z; w/=rhs.w; return *this; }
	inline cjVec4& operator /=(cjFLOAT f) { x/=f, y/=f, z/=f; return *this; }
	inline cjVec4  operator / (const cjVec4& rhs) const { return cjVec4(x/rhs.x, y/rhs.y, z/rhs.z, w/rhs.w); }
	inline cjVec4  operator / (cjFLOAT f) const { return cjVec4(x/f, y/f, z/f, w/f); }

	inline bool operator ==(const cjVec4& rhs) const
		{ return x==rhs.x && y==rhs.y && z == rhs.z && w == rhs.w ? true : false; }

	inline bool operator !=(const cjVec4& rhs) const
		{ return *this == rhs ? false : true;  }

	// 1e-06(0.000001) 1e-05(0.00001). 0.0001
	inline cjFLOAT Normalize() 
	{ 
		cjFLOAT len=Length();
		if (len > cjFLOAT_VAL_01x5f) { cjFLOAT f=1.0f/len;  x*=f; y*=f; z*=f; w*=f; }
		else { x=0.0f; y=0.0f; z=0.0f; w=0.0f; len=0.0f; }
        
		return len;
	}

	inline cjFLOAT Dot(const cjVec4& rhs) const
		{ return x*rhs.x + y*rhs.y + z*rhs.z + w*rhs.w; }

    // no crossproduct in 4d vector
//	inline cjVec4 Cross(const cjVec4& rhs) const { }

	inline cjFLOAT Length() const 
		{ return cjSqrt(x*x+y*y+z*z+w*w); }
	
public:
	cjFLOAT x, y, z, w;

	static const cjVec4 ZERO;
	static const cjVec4 UNIT;
	static const cjVec4 INVALID;
};

extern CJCORE_API const cjVec3 cjGBaseCoordX;
extern CJCORE_API const cjVec3 cjGBaseCoordY;
extern CJCORE_API const cjVec3 cjGBaseCoordZ;


//
// cjMatrix3, cjMatrix
//

#define cjMatrix2Determinant(a,b,c,d) ((a)*(d) - (c)*(b))

#define cjMatrix3Determinant(a,b,c,d,e,f,g,h,i)  \
    ((a)*cjMatrix2Determinant((e),(f),(h),(i)) - \
     (b)*cjMatrix2Determinant((d),(f),(g),(i)) + \
     (c)*cjMatrix2Determinant((d),(e),(g),(h)))


// Matrix RotX - right handed, row major
//  1        0        0
//  0        cos(t)   sin(t)
//  0       -sin(t)   cos(t)
//  t>0 : clockwise rotation in the yz-plane,

// Matrix RotY - right handed, row major
//  cos(t)    0      -sin(t)
//  0         1       0
//  sin(t)    0       cos(t)
//  t>0 : clockwise rotation in the zx-plane

// Matrix RotZ - right handed, row major
//  cos(t)    sin(t)  0
// -sin(t)    cos(t)  0
//  0         0       1
// t>0 : clockwise rotation in the xy-plane
// positive angles : clockwise rotation.


class CJCORE_API cjMatrix3 : public cjMem
{
	CJ_DECL_SERIALIZE_INLINE_BEGIN(cjMatrix3)
		cjPODSerialize(stream, d);
	CJ_DECL_SERIALIZE_INLINE_END(cjMatrix3)

public:
	cjMatrix3() {}
	cjMatrix3(const cjVec3& r0, const cjVec3& r1, const cjVec3& r2)
	{
		m[0][0] = r0.x, m[0][1] = r0.y, m[0][2] = r0.z;
		m[1][0] = r1.x, m[1][1] = r1.y, m[1][2] = r1.z;
		m[2][0] = r2.x, m[2][1] = r2.y, m[2][2] = r2.z;
	}

	void SetZero()
	{
		m[0][0] = 0; m[0][1] = 0;  m[0][2] = 0;
		m[1][0] = 0; m[1][1] = 0;  m[1][2] = 0;
		m[2][0] = 0; m[2][1] = 0;  m[2][2] = 0;
	}

	void SetIdentity()
	{
		m[0][0] = 1; m[0][1] = 0;  m[0][2] = 0;
		m[1][0] = 0; m[1][1] = 1;  m[1][2] = 0;
		m[2][0] = 0; m[2][1] = 0;  m[2][2] = 1;
	}

	// row major matrix x row vec3
	cjVec3 operator*(const cjVec3& dst) const
	{
		return cjVec3(
			m[0][0]*dst.x + m[0][1]*dst.y + m[0][2]*dst.z,
			m[1][0]*dst.x + m[1][1]*dst.y + m[1][2]*dst.z,
			m[2][0]*dst.x + m[2][1]*dst.y + m[2][2]*dst.z );
	}
	
	cjMatrix3 DoMatRowXVec3Col(const cjVec3& dst) const
	{
		cjMatrix3 rst;

		rst.m[0][0] = m[0][0] * dst.x; rst.m[0][1] = m[0][1] * dst.x; rst.m[0][2] = m[0][2] * dst.x;
		rst.m[1][0] = m[1][0] * dst.y; rst.m[1][1] = m[1][1] * dst.y; rst.m[1][2] = m[1][2] * dst.y;
		rst.m[2][0] = m[2][0] * dst.z; rst.m[2][1] = m[2][1] * dst.z; rst.m[2][2] = m[2][2] * dst.z;

		return rst;
	}

	cjMatrix3 operator*(const cjMatrix3& dst) const
	{
		cjMatrix3 rst;

		rst.m[0][0] = m[0][0] * dst.m[0][0] + m[0][1] * dst.m[1][0] + m[0][2] * dst.m[2][0] ;
		rst.m[0][1] = m[0][0] * dst.m[0][1] + m[0][1] * dst.m[1][1] + m[0][2] * dst.m[2][1] ;
		rst.m[0][2] = m[0][0] * dst.m[0][2] + m[0][1] * dst.m[1][2] + m[0][2] * dst.m[2][2] ;

		rst.m[1][0] = m[1][0] * dst.m[0][0] + m[1][1] * dst.m[1][0] + m[1][2] * dst.m[2][0] ;
		rst.m[1][1] = m[1][0] * dst.m[0][1] + m[1][1] * dst.m[1][1] + m[1][2] * dst.m[2][1] ;
		rst.m[1][2] = m[1][0] * dst.m[0][2] + m[1][1] * dst.m[1][2] + m[1][2] * dst.m[2][2] ;

		rst.m[2][0] = m[2][0] * dst.m[0][0] + m[2][1] * dst.m[1][0] + m[2][2] * dst.m[2][0] ;
		rst.m[2][1] = m[2][0] * dst.m[0][1] + m[2][1] * dst.m[1][1] + m[2][2] * dst.m[2][1] ;
		rst.m[2][2] = m[2][0] * dst.m[0][2] + m[2][1] * dst.m[1][2] + m[2][2] * dst.m[2][2] ;

		return rst;
	}

	void operator*=(const cjMatrix3& dst)
	{
		cjMatrix3 rst;

		rst.m[0][0] = m[0][0] * dst.m[0][0] + m[0][1] * dst.m[1][0] + m[0][2] * dst.m[2][0];
		rst.m[0][1] = m[0][0] * dst.m[0][1] + m[0][1] * dst.m[1][1] + m[0][2] * dst.m[2][1];
		rst.m[0][2] = m[0][0] * dst.m[0][2] + m[0][1] * dst.m[1][2] + m[0][2] * dst.m[2][2];

		rst.m[1][0] = m[1][0] * dst.m[0][0] + m[1][1] * dst.m[1][0] + m[1][2] * dst.m[2][0];
		rst.m[1][1] = m[1][0] * dst.m[0][1] + m[1][1] * dst.m[1][1] + m[1][2] * dst.m[2][1];
		rst.m[1][2] = m[1][0] * dst.m[0][2] + m[1][1] * dst.m[1][2] + m[1][2] * dst.m[2][2];

		rst.m[2][0] = m[2][0] * dst.m[0][0] + m[2][1] * dst.m[1][0] + m[2][2] * dst.m[2][0];
		rst.m[2][1] = m[2][0] * dst.m[0][1] + m[2][1] * dst.m[1][1] + m[2][2] * dst.m[2][1];
		rst.m[2][2] = m[2][0] * dst.m[0][2] + m[2][1] * dst.m[1][2] + m[2][2] * dst.m[2][2];

		*this = rst;
	}

	bool operator==(const cjMatrix3& dst) const
	{
		for(cjINT32 r=0; r<3; r++)
			for(cjINT32 c=0; c<3; c++)
				if(m[r][c] != dst.m[r][c])
					return false;

		return true;
	}

	cjBool operator!=(const cjMatrix3& dst) const { return !(*this == dst); }
	
	//cjVec3 operator*(const cjVec3& rhs) const
	//{
	//	return TransformVector(rhs);
	//}

	//// row x col
	//cjVec3 TransformVector(const cjVec3& v) const
	//{
	//	cjVec3 rst;

	//	rst.x = m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z;
	//	rst.y = m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z;
	//	rst.z = m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z;

	//	return rst;
	//}

	cjMatrix3& Transpose(cjMatrix3& rst) const
	{
		rst.m[0][0] = m[0][0]; rst.m[0][1] = m[1][0]; rst.m[0][2] = m[2][0];
		rst.m[1][0] = m[0][1]; rst.m[1][1] = m[1][1]; rst.m[1][2] = m[2][1];
		rst.m[2][0] = m[0][2]; rst.m[2][1] = m[1][2]; rst.m[2][2] = m[2][2];

		return rst;
	}

	cjMatrix3 Transpose()
	{
		cjMatrix3 tp;
		
		tp.m[0][0] = m[0][0]; tp.m[0][1] = m[1][0]; tp.m[0][2] = m[2][0];
		tp.m[1][0] = m[0][1]; tp.m[1][1] = m[1][1]; tp.m[1][2] = m[2][1];
		tp.m[2][0] = m[0][2]; tp.m[2][1] = m[1][2]; tp.m[2][2] = m[2][2];		

		return tp;
	}

    cjMatrix3 Inverse() const
	{
		cjMatrix3 inv;    
		
		if(Inverse(inv) == cjFalse)
			inv.SetZero();

		return inv;
	}

    cjBool Inverse(cjMatrix3& inv) const
	{
		inv.m[0][0] = m[1][1]*m[2][2]- m[1][2]*m[2][1];
		inv.m[0][1] = m[0][2]*m[2][1]- m[0][1]*m[2][2];
		inv.m[0][2] = m[0][1]*m[1][2]- m[0][2]*m[1][1];
		inv.m[1][0] = m[1][2]*m[2][0]- m[1][0]*m[2][2];
		inv.m[1][1] = m[0][0]*m[2][2]- m[0][2]*m[2][0];
		inv.m[1][2] = m[0][2]*m[1][0]- m[0][0]*m[1][2];
		inv.m[2][0] = m[1][0]*m[2][1]- m[1][1]*m[2][0];
		inv.m[2][1] = m[0][1]*m[2][0]- m[0][0]*m[2][1];
		inv.m[2][2] = m[0][0]*m[1][1]- m[0][1]*m[1][0];

		cjFLOAT d = m[0][0]*inv.m[0][0]+ m[0][1]*inv.m[1][0]+ m[0][2]*inv.m[2][0];

		if(cjAbs(d) <= 1e-06f)
			return cjFalse;

		cjFLOAT invD = 1.0f / d;

		for(cjINT32 row=0; row<3; row++)
		{
			for(cjINT32 col=0; col<3; col++)
				inv.m[row][col] *= invD;
		}

		return cjTrue;
	}
	
	// normally row x column
	cjMatrix3 MultiplyColXCol(const cjMatrix3& mat) const
	{
		cjMatrix3 rst;

		/*
		      [00][][] x [00][][]   [][01][] x [00][][]		[][][01] x [00][][] 
			  [10][][] x [10][][]	[][11][] x [10][][]		[][][11] x [10][][]
			  [20][][] x [20][][]	[][21][] x [20][][]		[][][21] x [20][][]

		      [00][][] x [][00][]   [][01][] x [][00][]		[][][01] x [][00][] 
			  [10][][] x [][10][]	[][11][] x [][10][]		[][][11] x [][10][]
			  [20][][] x [][20][]	[][21][] x [][20][]		[][][21] x [][20][]

		      [00][][] x [][][00]   [][01][] x [][][00]		[][][01] x [][][00] 
			  [10][][] x [][][10]	[][11][] x [][][10]		[][][11] x [][][10]
			  [20][][] x [][][20]	[][21][] x [][][20]		[][][21] x [][][20]
		*/

		rst.m[0][0] = m[0][0] * mat.m[0][0] + m[1][0] * mat.m[1][0] + m[2][0] * mat.m[2][0];
		rst.m[1][0] = m[0][1] * mat.m[0][0] + m[1][1] * mat.m[1][0] + m[2][1] * mat.m[2][0];
		rst.m[2][0] = m[0][2] * mat.m[0][0] + m[1][2] * mat.m[1][0] + m[2][2] * mat.m[2][0];
							 	 					 	    				 	 				   
		rst.m[0][1] = m[0][0] * mat.m[0][1] + m[1][0] * mat.m[1][1] + m[2][0] * mat.m[2][1];
		rst.m[1][1] = m[0][1] * mat.m[0][1] + m[1][1] * mat.m[1][1] + m[2][1] * mat.m[2][1];
		rst.m[2][1] = m[0][2] * mat.m[0][1] + m[1][2] * mat.m[1][1] + m[2][2] * mat.m[2][1];
							    					    					    
		rst.m[0][2] = m[0][0] * mat.m[0][2] + m[1][0] * mat.m[1][2] + m[2][0] * mat.m[2][2];
		rst.m[1][2] = m[0][1] * mat.m[0][2] + m[1][1] * mat.m[1][2] + m[2][1] * mat.m[2][2];
		rst.m[2][2] = m[0][2] * mat.m[0][2] + m[1][2] * mat.m[1][2] + m[2][2] * mat.m[2][2];

		return rst;
	}

	void MakeScale(cjFLOAT x, cjFLOAT y, cjFLOAT z)
	{
		m[0][0] = x;    m[0][1] = 0.0f; m[0][2] = 0.0f;
		m[1][0] = 0.0f; m[1][1] = y;    m[1][2] = 0.0f;
		m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = z;   
	}

	// angle axis
	void MakeRotationFromAxis(cjFLOAT rad, cjFLOAT x, cjFLOAT y, cjFLOAT z)
	{
		cjFLOAT sn, cs;
		cjSinCosf(rad, sn, cs);

		cjFLOAT omcs = 1.0f-cs;
		cjFLOAT x2 = x*x;
		cjFLOAT y2 = y*y;
		cjFLOAT z2 = z*z;
		cjFLOAT xym = x*y*omcs;
		cjFLOAT xzm = x*z*omcs;
		cjFLOAT yzm = y*z*omcs;
		cjFLOAT xsin = x*sn;
		cjFLOAT ysin = y*sn;
		cjFLOAT zsin = z*sn;
	    
		m[0][0] = x2*omcs+cs; m[0][1] = xym+zsin;		m[0][2] = xzm-ysin;
		m[1][0] = xym-zsin;   m[1][1] = y2*omcs+cs;		m[1][2] = yzm+xsin;
		m[2][0] = xzm+ysin;   m[2][1] = yzm-xsin;		m[2][2] = z2*omcs+cs;
	}

	void MakeRotationFromAxis(cjFLOAT rad, const cjVec3& axis)
	{
		MakeRotationFromAxis(rad, axis.x, axis.y, axis.z);
	}

	void MakeXRotation(cjFLOAT angle)
	{
		cjFLOAT sn, cs;
		cjSinCosf(angle, sn, cs);
	    
		m[0][0] = 1.0f;	m[0][1] = 0.0f;	m[0][2] = 0.0f;
		m[1][0] = 0.0f;	m[1][1] = cs;	m[1][2] = sn;
		m[2][0] = 0.0f;	m[2][1] = -sn;	m[2][2] = cs;
	}

	void MakeYRotation(cjFLOAT angle)
	{
		cjFLOAT sn, cs;
		cjSinCosf(angle, sn, cs);
	    
		m[0][0] = cs;	m[0][1] = 0.0f; m[0][2] = -sn;
		m[1][0] = 0.0f;	m[1][1] = 1.0f;	m[1][2] = 0.0f;
		m[2][0] = sn;	m[2][1] = 0.0f;	m[2][2] = cs;
	}

	void MakeZRotation(cjFLOAT angle)
	{
		cjFLOAT sn, cs;
		cjSinCosf(angle, sn, cs);

		m[0][0] = cs;	m[0][1] = sn;	m[0][2] = 0.0f;
		m[1][0] = -sn;	m[1][1] = cs;	m[1][2] = 0.0f;
		m[2][0] = 0.0f;	m[2][1] = 0.0f;	m[2][2] = 1.0f;
	}

	void ToAngleAndAxis(cjFLOAT& angle, cjFLOAT& x, cjFLOAT& y, cjFLOAT& z) const;

	void SetRow(cjINT32 row, cjFLOAT x, cjFLOAT y, cjFLOAT z) 
	{ 
		m[row][0] = x, m[row][1] = y, m[row][2] = z; 
	}
	
	void SetRow(cjINT32 row, const cjVec3& v)  
	{ 
		m[row][0] = v.x, m[row][1] = v.y, m[row][2] = v.z; 
	}
	
	void SetCol(cjINT32 col, cjFLOAT x, cjFLOAT y, cjFLOAT z) 
	{ 
		m[0][col] = x, m[1][col] = y, m[2][col] = z; 
	}
	
	void SetCol(cjINT32 col, const cjVec3& v) 
	{ 
		m[0][col] = v.x, m[1][col] = v.y, m[2][col] = v.z; 
	}

	void GetCol(cjINT32 col, cjVec3& rstCol) const
	{
		rstCol.x = m[0][col]; rstCol.y = m[1][col]; rstCol.z = m[2][col];
	}

	void GetCol(cjINT32 col, cjFLOAT& x, cjFLOAT& y, cjFLOAT& z) const
	{
		x = m[0][col]; y = m[1][col]; z = m[2][col];
	}

	void GetRow(cjINT32 row, cjVec3& rstRow) const
	{
		rstRow.x = m[row][0]; rstRow.y = m[row][1]; rstRow.z = m[row][2];
	}

	void GetRow(cjINT32 row, cjFLOAT& x, cjFLOAT& y, cjFLOAT& z) const
	{
		x = m[row][0]; y = m[row][1]; z = m[row][2];
	}

	cjFLOAT GetEntry(cjUINT32 row, cjUINT32 col) const 
	{ 
		return m[row][col]; 
	}
	
	// Euler Utility
	// matrix must be orthonormal
	cjBool GetEulerAngleXYZ(cjFLOAT& X, cjFLOAT& Y, cjFLOAT& Z) const;
    cjBool GetEulerAngleXZY(cjFLOAT& X, cjFLOAT& Z, cjFLOAT& Y) const;
    cjBool GetEulerAngleYXZ(cjFLOAT& Y, cjFLOAT& X, cjFLOAT& Z) const;
    cjBool GetEulerAngleYZX(cjFLOAT& Y, cjFLOAT& Z, cjFLOAT& X) const;
    cjBool GetEulerAngleZXY(cjFLOAT& Z, cjFLOAT& X, cjFLOAT& Y) const;
    cjBool GetEulerAngleZYX(cjFLOAT& Z, cjFLOAT& Y, cjFLOAT& X) const;

    void MakeFromEulerAngleXYZ(cjFLOAT X, cjFLOAT Y, cjFLOAT Z);
    void MakeFromEulerAngleXZY(cjFLOAT X, cjFLOAT Z, cjFLOAT Y);
    void MakeFromEulerAngleYXZ(cjFLOAT Y, cjFLOAT X, cjFLOAT Z);
    void MakeFromEulerAngleYZX(cjFLOAT Y, cjFLOAT Z, cjFLOAT X);
    void MakeFromEulerAngleZXY(cjFLOAT Z, cjFLOAT X, cjFLOAT Y);
    void MakeFromEulerAngleZYX(cjFLOAT Z, cjFLOAT Y, cjFLOAT X);

	// assume column vector is rotation axis
	static cjBool IsRotationMatrix(const cjMatrix3& matrix);

public:
	cjFLOAT m[3][3];
    static const cjMatrix3 Identity;
};


/*
	[rx] [ux] [dx] [0]
	[ry] [uy] [dy] [0]
	[rz] [uz] [dz] [0]
	[px] [py] [pz] [1]
*/

// row major matrix
class CJCORE_API cjMatrix : public cjMem
{
	CJ_DECL_SERIALIZE_INLINE_BEGIN(cjMatrix)
		cjPODSerialize(stream, d);
	CJ_DECL_SERIALIZE_INLINE_END(cjMatrix)

public:
	cjMatrix() {}

	cjMatrix(const cjTransform& rhs) { *this = rhs; }

	cjMatrix(const cjVec4& r0, const cjVec4& r1, const cjVec4& r2, const cjVec4& r3)
	{
		m[0][0] = r0.x, m[0][1] = r0.y, m[0][2] = r0.z, m[0][3] = r0.w;
		m[1][0] = r1.x, m[1][1] = r1.y, m[1][2] = r1.z, m[1][3] = r1.w;
		m[2][0] = r2.x, m[2][1] = r2.y, m[2][2] = r2.z, m[2][3] = r2.w;
		m[3][0] = r3.x, m[3][1] = r3.y, m[3][2] = r3.z, m[3][3] = r3.w;
	}

	void SetIdentity()
	{
		m[0][0] = 1; m[0][1] = 0;  m[0][2] = 0;  m[0][3] = 0;
		m[1][0] = 0; m[1][1] = 1;  m[1][2] = 0;  m[1][3] = 0;
		m[2][0] = 0; m[2][1] = 0;  m[2][2] = 1;  m[2][3] = 0;
		m[3][0] = 0; m[3][1] = 0;  m[3][2] = 0;  m[3][3] = 1;
	}

	cjMatrix operator*(cjFLOAT f) const
	{
		cjMatrix rst;
				
		rst.m[0][0] = m[0][0] * f; rst.m[0][1] = m[0][1] * f; rst.m[0][2] = m[0][2]* f; rst.m[0][3] = m[0][3] * f;
		rst.m[1][0] = m[1][0] * f; rst.m[1][1] = m[1][1] * f; rst.m[1][2] = m[1][2]* f; rst.m[1][3] = m[1][3] * f;
		rst.m[2][0] = m[2][0] * f; rst.m[2][1] = m[2][1] * f; rst.m[2][2] = m[2][2]* f; rst.m[2][3] = m[2][3] * f;
		rst.m[3][0] = m[3][0] * f; rst.m[3][1] = m[3][1] * f; rst.m[3][2] = m[3][2]* f; rst.m[3][3] = m[3][3] * f;

		return rst;
	}

	// row x col = row
	cjMatrix operator*(const cjMatrix& dst) const
	{
		cjMatrix rst;

		rst.m[0][0] = m[0][0] * dst.m[0][0] + m[0][1] * dst.m[1][0] + m[0][2] * dst.m[2][0] + m[0][3] * dst.m[3][0];
		rst.m[0][1] = m[0][0] * dst.m[0][1] + m[0][1] * dst.m[1][1] + m[0][2] * dst.m[2][1] + m[0][3] * dst.m[3][1];
		rst.m[0][2] = m[0][0] * dst.m[0][2] + m[0][1] * dst.m[1][2] + m[0][2] * dst.m[2][2] + m[0][3] * dst.m[3][2];
		rst.m[0][3] = m[0][0] * dst.m[0][3] + m[0][1] * dst.m[1][3] + m[0][2] * dst.m[2][3] + m[0][3] * dst.m[3][3];

		rst.m[1][0] = m[1][0] * dst.m[0][0] + m[1][1] * dst.m[1][0] + m[1][2] * dst.m[2][0] + m[1][3] * dst.m[3][0];
		rst.m[1][1] = m[1][0] * dst.m[0][1] + m[1][1] * dst.m[1][1] + m[1][2] * dst.m[2][1] + m[1][3] * dst.m[3][1];
		rst.m[1][2] = m[1][0] * dst.m[0][2] + m[1][1] * dst.m[1][2] + m[1][2] * dst.m[2][2] + m[1][3] * dst.m[3][2];
		rst.m[1][3] = m[1][0] * dst.m[0][3] + m[1][1] * dst.m[1][3] + m[1][2] * dst.m[2][3] + m[1][3] * dst.m[3][3];

		rst.m[2][0] = m[2][0] * dst.m[0][0] + m[2][1] * dst.m[1][0] + m[2][2] * dst.m[2][0] + m[2][3] * dst.m[3][0];
		rst.m[2][1] = m[2][0] * dst.m[0][1] + m[2][1] * dst.m[1][1] + m[2][2] * dst.m[2][1] + m[2][3] * dst.m[3][1];
		rst.m[2][2] = m[2][0] * dst.m[0][2] + m[2][1] * dst.m[1][2] + m[2][2] * dst.m[2][2] + m[2][3] * dst.m[3][2];
		rst.m[2][3] = m[2][0] * dst.m[0][3] + m[2][1] * dst.m[1][3] + m[2][2] * dst.m[2][3] + m[2][3] * dst.m[3][3];

		rst.m[3][0] = m[3][0] * dst.m[0][0] + m[3][1] * dst.m[1][0] + m[3][2] * dst.m[2][0] + m[3][3] * dst.m[3][0];
		rst.m[3][1] = m[3][0] * dst.m[0][1] + m[3][1] * dst.m[1][1] + m[3][2] * dst.m[2][1] + m[3][3] * dst.m[3][1];
		rst.m[3][2] = m[3][0] * dst.m[0][2] + m[3][1] * dst.m[1][2] + m[3][2] * dst.m[2][2] + m[3][3] * dst.m[3][2];
		rst.m[3][3] = m[3][0] * dst.m[0][3] + m[3][1] * dst.m[1][3] + m[3][2] * dst.m[2][3] + m[3][3] * dst.m[3][3];

		return rst;
	}

	void operator*=(const cjMatrix dst)
	{
		cjMatrix rst;

		rst.m[0][0] = m[0][0] * dst.m[0][0] + m[0][1] * dst.m[1][0] + m[0][2] * dst.m[2][0] + m[0][3] * dst.m[3][0];
		rst.m[0][1] = m[0][0] * dst.m[0][1] + m[0][1] * dst.m[1][1] + m[0][2] * dst.m[2][1] + m[0][3] * dst.m[3][1];
		rst.m[0][2] = m[0][0] * dst.m[0][2] + m[0][1] * dst.m[1][2] + m[0][2] * dst.m[2][2] + m[0][3] * dst.m[3][2];
		rst.m[0][3] = m[0][0] * dst.m[0][3] + m[0][1] * dst.m[1][3] + m[0][2] * dst.m[2][3] + m[0][3] * dst.m[3][3];

		rst.m[1][0] = m[1][0] * dst.m[0][0] + m[1][1] * dst.m[1][0] + m[1][2] * dst.m[2][0] + m[1][3] * dst.m[3][0];
		rst.m[1][1] = m[1][0] * dst.m[0][1] + m[1][1] * dst.m[1][1] + m[1][2] * dst.m[2][1] + m[1][3] * dst.m[3][1];
		rst.m[1][2] = m[1][0] * dst.m[0][2] + m[1][1] * dst.m[1][2] + m[1][2] * dst.m[2][2] + m[1][3] * dst.m[3][2];
		rst.m[1][3] = m[1][0] * dst.m[0][3] + m[1][1] * dst.m[1][3] + m[1][2] * dst.m[2][3] + m[1][3] * dst.m[3][3];

		rst.m[2][0] = m[2][0] * dst.m[0][0] + m[2][1] * dst.m[1][0] + m[2][2] * dst.m[2][0] + m[2][3] * dst.m[3][0];
		rst.m[2][1] = m[2][0] * dst.m[0][1] + m[2][1] * dst.m[1][1] + m[2][2] * dst.m[2][1] + m[2][3] * dst.m[3][1];
		rst.m[2][2] = m[2][0] * dst.m[0][2] + m[2][1] * dst.m[1][2] + m[2][2] * dst.m[2][2] + m[2][3] * dst.m[3][2];
		rst.m[2][3] = m[2][0] * dst.m[0][3] + m[2][1] * dst.m[1][3] + m[2][2] * dst.m[2][3] + m[2][3] * dst.m[3][3];

		rst.m[3][0] = m[3][0] * dst.m[0][0] + m[3][1] * dst.m[1][0] + m[3][2] * dst.m[2][0] + m[3][3] * dst.m[3][0];
		rst.m[3][1] = m[3][0] * dst.m[0][1] + m[3][1] * dst.m[1][1] + m[3][2] * dst.m[2][1] + m[3][3] * dst.m[3][1];
		rst.m[3][2] = m[3][0] * dst.m[0][2] + m[3][1] * dst.m[1][2] + m[3][2] * dst.m[2][2] + m[3][3] * dst.m[3][2];
		rst.m[3][3] = m[3][0] * dst.m[0][3] + m[3][1] * dst.m[1][3] + m[3][2] * dst.m[2][3] + m[3][3] * dst.m[3][3];

		*this = rst;
	}

	cjMatrix& operator +=(const cjMatrix& dst)
	{
		m[0][0] += dst.m[0][0];	m[0][1] += dst.m[0][1];	m[0][2] += dst.m[0][2];	m[0][3] += dst.m[0][3];	
		m[1][0] += dst.m[1][0];	m[1][1] += dst.m[1][1];	m[1][2] += dst.m[1][2];	m[1][3] += dst.m[1][3];	
		m[2][0] += dst.m[2][0];	m[2][1] += dst.m[2][1];	m[2][2] += dst.m[2][2];	m[2][3] += dst.m[2][3];	
		m[3][0] += dst.m[3][0];	m[3][1] += dst.m[3][1];	m[3][2] += dst.m[3][2];	m[3][3] += dst.m[3][3];	

		return *this;
	}

	bool operator==(const cjMatrix& dst) const
	{
		for(cjINT32 r=0; r<4; r++)
			for(cjINT32 c=0; c<4; c++)
				if(m[r][c] != dst.m[r][c])
					return false;

		return true;
	}

	bool operator!=(cjMatrix& dst) const { return !(*this == dst); }

	inline void operator= (const cjTransform& tm); // defer implement to after cjTransform
	

	// row-major matrix x column vector4
	cjVec4 operator*(const cjVec4& v) const
	{
		cjVec4 rst;

		rst.x = m[0][0] * v.x  + m[0][1] * v.y  + m[0][2] * v.z  + m[0][3] * v.w;
		rst.y = m[1][0] * v.x  + m[1][1] * v.y  + m[1][2] * v.z  + m[1][3] * v.w;
		rst.z = m[2][0] * v.x  + m[2][1] * v.y  + m[2][2] * v.z  + m[2][3] * v.w;
		rst.w = m[3][0] * v.x  + m[3][1] * v.y  + m[3][2] * v.z  + m[3][3] * v.w;

		return rst;
	}
	
	// row-major matrix x colum vector4
	cjVec3 operator*(const cjVec3& v) const
	{
		cjVec3 rst;

		rst.x = m[0][0] * v.x  + m[0][1] * v.y  + m[0][2] * v.z; 
		rst.y = m[1][0] * v.x  + m[1][1] * v.y  + m[1][2] * v.z;
		rst.z = m[2][0] * v.x  + m[2][1] * v.y  + m[2][2] * v.z;

		rst.x += m[3][0];
		rst.y += m[3][1];
		rst.z += m[3][2];

		return rst;
	}

	// normal, so pos row( [3][0], [3][1], [3][2] ) is omitted
	cjVec4 TransformNormalRC(const cjVec4& v) const
	{
		cjVec4 rst;

		rst.x = m[0][0] * v.x  + m[0][1] * v.y  + m[0][2] * v.z; 
		rst.y = m[1][0] * v.x  + m[1][1] * v.y  + m[1][2] * v.z;
		rst.z = m[2][0] * v.x  + m[2][1] * v.y  + m[2][2] * v.z;
		rst.w = 0.0f;

		return rst;
	}

	cjMatrix Transpose()
	{
		cjMatrix tp;
		
		tp.m[0][0] = m[0][0]; tp.m[0][1] = m[1][0]; tp.m[0][2] = m[2][0]; tp.m[0][3] = m[3][0];
		tp.m[1][0] = m[0][1]; tp.m[1][1] = m[1][1]; tp.m[1][2] = m[2][1]; tp.m[1][3] = m[3][1];
		tp.m[2][0] = m[0][2]; tp.m[2][1] = m[1][2]; tp.m[2][2] = m[2][2]; tp.m[2][3] = m[3][2];
		tp.m[3][0] = m[0][3]; tp.m[3][1] = m[1][3]; tp.m[3][2] = m[2][3]; tp.m[3][3] = m[3][3];

		return tp;
	}

	void Transpose(cjMatrix& rst) const
	{
		rst.m[0][0] = m[0][0]; rst.m[0][1] = m[1][0]; rst.m[0][2] = m[2][0]; rst.m[0][3] = m[3][0];
		rst.m[1][0] = m[0][1]; rst.m[1][1] = m[1][1]; rst.m[1][2] = m[2][1]; rst.m[1][3] = m[3][1];
		rst.m[2][0] = m[0][2]; rst.m[2][1] = m[1][2]; rst.m[2][2] = m[2][2]; rst.m[2][3] = m[3][2];
		rst.m[3][0] = m[0][3]; rst.m[3][1] = m[1][3]; rst.m[3][2] = m[2][3]; rst.m[3][3] = m[3][3];
	}

	cjFLOAT Determinant() const
	{
		return	m[0][0] * (m[1][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) - m[2][1] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) + m[3][1] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]))
		      - m[1][0] * (m[0][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) - m[2][1] * (m[0][2] * m[3][3] - m[0][3] * m[3][2]) + m[3][1] * (m[0][2] * m[2][3] - m[0][3] * m[2][2])) 
		 	  + m[2][0] * (m[0][1] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) - m[1][1] * (m[0][2] * m[3][3] - m[0][3] * m[3][2]) + m[3][1] * (m[0][2] * m[1][3] - m[0][3] * m[1][2])) 
			  - m[3][0] * (m[0][1] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]) - m[1][1] * (m[0][2] * m[2][3] - m[0][3] * m[2][2]) + m[2][1] * (m[0][2] * m[1][3] - m[0][3] * m[1][2]));
	}

	cjMatrix& Inverse()
	{
		cjMatrix rst; 
		
		Inverse(rst); 
		*this = rst;

		return *this;
	}

	void Inverse(cjMatrix& rst) const
	{
		cjFLOAT	det = Determinant();

		if(det == 0.0f)
		{
			rst = cjMatrix::Identity;
			return;
		}

		cjFLOAT	revDet = 1.0f / det;

		rst.m[0][0] = revDet * (m[1][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2])  - m[2][1] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) +	m[3][1] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]));
		rst.m[0][1] = -revDet * (m[0][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) - m[2][1] * (m[0][2] * m[3][3] - m[0][3] * m[3][2]) +	m[3][1] * (m[0][2] * m[2][3] - m[0][3] * m[2][2]));
		rst.m[0][2] = revDet * (m[0][1] * (m[1][2] * m[3][3] - m[1][3] * m[3][2])  - m[1][1] * (m[0][2] * m[3][3] - m[0][3] * m[3][2]) +	m[3][1] * (m[0][2] * m[1][3] - m[0][3] * m[1][2]));
		rst.m[0][3] = -revDet * (m[0][1] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]) - m[1][1] * (m[0][2] * m[2][3] - m[0][3] * m[2][2]) +	m[2][1] * (m[0][2] * m[1][3] - m[0][3] * m[1][2]));

		rst.m[1][0] = -revDet * (m[1][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) - m[2][0] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) +	m[3][0] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]));
		rst.m[1][1] = revDet * (m[0][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2])  - m[2][0] * (m[0][2] * m[3][3] - m[0][3] * m[3][2]) +	m[3][0] * (m[0][2] * m[2][3] - m[0][3] * m[2][2]));
		rst.m[1][2] = -revDet * (m[0][0] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) - m[1][0] * (m[0][2] * m[3][3] - m[0][3] * m[3][2]) +	m[3][0] * (m[0][2] * m[1][3] - m[0][3] * m[1][2]));
		rst.m[1][3] = revDet * (m[0][0] * (m[1][2] * m[2][3] - m[1][3] * m[2][2])  - m[1][0] * (m[0][2] * m[2][3] - m[0][3] * m[2][2]) +	m[2][0] * (m[0][2] * m[1][3] - m[0][3] * m[1][2]));

		rst.m[2][0] = revDet * (m[1][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1])  -m[2][0] * (m[1][1] * m[3][3] - m[1][3] * m[3][1]) +	m[3][0] * (m[1][1] * m[2][3] - m[1][3] * m[2][1]));
		rst.m[2][1] = -revDet * (m[0][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) -m[2][0] * (m[0][1] * m[3][3] - m[0][3] * m[3][1]) +	m[3][0] * (m[0][1] * m[2][3] - m[0][3] * m[2][1]));
		rst.m[2][2] = revDet * (m[0][0] * (m[1][1] * m[3][3] - m[1][3] * m[3][1])  -m[1][0] * (m[0][1] * m[3][3] - m[0][3] * m[3][1]) +	m[3][0] * (m[0][1] * m[1][3] - m[0][3] * m[1][1]));
		rst.m[2][3] = -revDet * (m[0][0] * (m[1][1] * m[2][3] - m[1][3] * m[2][1]) -m[1][0] * (m[0][1] * m[2][3] - m[0][3] * m[2][1]) +	m[2][0] * (m[0][1] * m[1][3] - m[0][3] * m[1][1]));

		rst.m[3][0] = -revDet * (m[1][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) -m[2][0] * (m[1][1] * m[3][2] - m[1][2] * m[3][1]) +	m[3][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]));
		rst.m[3][1] = revDet * (m[0][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1])  -m[2][0] * (m[0][1] * m[3][2] - m[0][2] * m[3][1]) +	m[3][0] * (m[0][1] * m[2][2] - m[0][2] * m[2][1]));
		rst.m[3][2] = -revDet * (m[0][0] * (m[1][1] * m[3][2] - m[1][2] * m[3][1]) -m[1][0] * (m[0][1] * m[3][2] - m[0][2] * m[3][1]) +	m[3][0] * (m[0][1] * m[1][2] - m[0][2] * m[1][1]));
		rst.m[3][3] = revDet * (m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -	m[1][0] * (m[0][1] * m[2][2] - m[0][2] * m[2][1]) +	m[2][0] * (m[0][1] * m[1][2] - m[0][2] * m[1][1]));
	}

	void MakeScale(cjFLOAT x, cjFLOAT y, cjFLOAT z)
	{
		m[0][0] = x;    m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
		m[1][0] = 0.0f; m[1][1] = y;    m[1][2] = 0.0f; m[1][3] = 0.0f;
		m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = z;    m[2][3] = 0.0f;
		m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
	}

	void MakeTranslation(cjFLOAT x, cjFLOAT y, cjFLOAT z)
	{
		m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0;
		m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0;
		m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0;
		m[3][0] = x;	m[3][1] = y;	m[3][2] = z;	m[3][3] = 1.0f;
	}

	// from angle, axis
	void MakeRotation(cjFLOAT rad, cjFLOAT x, cjFLOAT y, cjFLOAT z)
	{
		cjFLOAT sn, cs;
		cjSinCosf(rad, sn, cs);

		cjFLOAT omcs = 1.0f-cs;
		cjFLOAT x2 = x*x;
		cjFLOAT y2 = y*y;
		cjFLOAT z2 = z*z;
		cjFLOAT xym = x*y*omcs;
		cjFLOAT xzm = x*z*omcs;
		cjFLOAT yzm = y*z*omcs;
		cjFLOAT xsin = x*sn;
		cjFLOAT ysin = y*sn;
		cjFLOAT zsin = z*sn;
	    
		m[0][0] = x2*omcs+cs; m[0][1] = xym+zsin;		m[0][2] = xzm-ysin;		m[0][3] = 0.0f;
		m[1][0] = xym-zsin;   m[1][1] = y2*omcs+cs;		m[1][2] = yzm+xsin;		m[1][3] = 0.0f;
		m[2][0] = xzm+ysin;   m[2][1] = yzm-xsin;		m[2][2] = z2*omcs+cs;   m[2][3] = 0.0f;
		m[3][0] = 0.0f;		  m[3][1] = 0.0f;			m[3][2] = 0.0f;			m[3][3] = 1.0f;
	}

	void CopyMatrix3(cjMatrix3& rst) const
	{
		rst.m[0][0] = m[0][0]; rst.m[0][1] = m[0][1]; rst.m[0][2] = m[0][2];
		rst.m[1][0] = m[1][0]; rst.m[1][1] = m[1][1]; rst.m[1][2] = m[1][2];
		rst.m[2][0] = m[2][0]; rst.m[2][1] = m[2][1]; rst.m[2][2] = m[2][2];
	}

	void SetRow(cjINT32 row, cjFLOAT x, cjFLOAT y, cjFLOAT z, cjFLOAT w) { m[row][0] = x, m[row][1] = y, m[row][2] = z, m[row][3] = w; }
	void SetRow(cjINT32 row, const cjVec4& v)  { m[row][0] = v.x, m[row][1] = v.y, m[row][2] = v.z; m[row][3] = v.w; }
	void SetCol(cjINT32 col, cjFLOAT x, cjFLOAT y, cjFLOAT z, cjFLOAT w) { m[0][col] = x, m[1][col] = y, m[2][col] = z, m[3][col] = w; }
	void SetCol(cjINT32 col, const cjVec4& v) { m[0][col] = v.x, m[1][col] = v.y, m[2][col] = v.z, m[3][col] = v.w; }

public:
	cjFLOAT m[4][4];
	static const cjMatrix Identity;
};


//
// cjMatrix34
//

/*

   right up dir
   [00] [01] [02] [03]   rx  ux  dx  0
   [10] [11] [12] [13]   ry  uy  dy  0
   [20] [21] [22] [23]   rz  uz  dz  0
   [30] [31] [32] [33]   p.x p.y p.z 1   
               
			   convert to

	vec r0	[00]rx  [01]ry  [02]rz  [03]p.x
	vec r1  [10]ux  [11]uy  [12]uz  [13]p.y
	vec r2	[20]dx  [21]dy  [22]dz  [23]p.x
*/

class CJCORE_API cjMatrix34 : public cjMem
{
	CJ_DECL_SERIALIZE_INLINE_BEGIN(cjMatrix34)
		cjPODSerialize(stream, d);
	CJ_DECL_SERIALIZE_INLINE_END(cjMatrix34)

public:
	cjMatrix34() {}
	cjMatrix34(const cjVec4& r0, const cjVec4& r1, const cjVec4& r2)
	{
		// pos x,y,z = (ro.w, r1.w, r2.w)
		m[0][0] = r0.x, m[0][1] = r0.y, m[0][2] = r0.z, m[0][3] = r0.w; // rx,ry,rz,p.x
		m[1][0] = r1.x, m[1][1] = r1.y, m[1][2] = r1.z, m[1][3] = r1.w; // ux,uy,uz,p.y
		m[2][0] = r2.x, m[2][1] = r2.y, m[2][2] = r2.z, m[2][3] = r2.w; // dx,dy,dz,p.z
	}

	void SetIdentity()
	{
		m[0][0] = 1; m[0][1] = 0;  m[0][2] = 0;  m[0][3] = 0;
		m[1][0] = 0; m[1][1] = 1;  m[1][2] = 0;  m[1][3] = 0;
		m[2][0] = 0; m[2][1] = 0;  m[2][2] = 1;  m[2][3] = 0;
	}

	cjMatrix34& operator *(cjFLOAT scale)
	{
		m[0][0] *= scale, 
		m[0][1] *= scale,
		m[0][2] *= scale,
		
		m[1][0] *= scale, 
		m[1][1] *= scale,
		m[1][2] *= scale,
		
		m[2][0] *= scale, 
		m[2][1] *= scale,
		m[2][2] *= scale;

		return *this;
	}

	cjMatrix34& operator =(const cjMatrix& mat)
	{
		const cjFLOAT** s = (const cjFLOAT**)mat.m;

		m[0][0] = s[0][0], m[0][1] = s[1][0], m[0][2] = s[2][0], m[0][3] = s[3][0];
		m[1][0] = s[0][1], m[1][1] = s[1][1], m[1][2] = s[2][1], m[1][3] = s[3][1];
		m[2][0] = s[0][2], m[2][1] = s[1][2], m[2][2] = s[2][2], m[2][3] = s[3][2];

		return *this;
	}

	inline cjMatrix34& operator =(const cjTransform& tm);
	
	bool operator==(const cjMatrix34& dst) const
	{
		for(cjINT32 r=0; r<3; r++)
			for(cjINT32 c=0; c<4; c++)
				if(m[r][c] != dst.m[r][c])
					return false;

		return true;
	}

	bool operator!=(cjMatrix34& dst) const { return !(*this == dst); }
		
	// row x col
	cjVec4 TransformVector(const cjVec4& v) const
	{
		cjVec4 rst;

		rst.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w; // r + p.x
		rst.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w; // u + p.y
		rst.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w; // d + p.z
		rst.w = v.w;

		return rst;
	}

	cjVec4 TransformNormal(const cjVec4& v) const
	{
		cjVec4 rst;

		rst.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z; // r
		rst.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z; // u
		rst.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z; // d
		rst.w = 0;

		return rst;
	}

	void MakeScale(cjFLOAT x, cjFLOAT y, cjFLOAT z)
	{
		m[0][0] = x;    m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
		m[1][0] = 0.0f; m[1][1] = y;    m[1][2] = 0.0f; m[1][3] = 0.0f;
		m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = z;    m[2][3] = 0.0f;
	}

	void MakeTranslation(cjFLOAT x, cjFLOAT y, cjFLOAT z)
	{
		m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = x;
		m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = y;
		m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = z;
	}

	// from angle, axis
	void MakeRotation(cjFLOAT rad, cjFLOAT x, cjFLOAT y, cjFLOAT z)
	{
		cjFLOAT sn, cs;
		cjSinCosf(rad, sn, cs);

		cjFLOAT omcs = 1.0f-cs;
		cjFLOAT x2 = x*x;
		cjFLOAT y2 = y*y;
		cjFLOAT z2 = z*z;
		cjFLOAT xym = x*y*omcs;
		cjFLOAT xzm = x*z*omcs;
		cjFLOAT yzm = y*z*omcs;
		cjFLOAT xsin = x*sn;
		cjFLOAT ysin = y*sn;
		cjFLOAT zsin = z*sn;
	    
		m[0][0] = x2*omcs+cs; 	m[0][1] = xym-zsin;		m[0][2] = xzm+ysin;     m[0][3] = 0.0f;
		m[1][0] = xym+zsin;		m[1][1] = y2*omcs+cs;	m[1][2] = yzm-xsin;		m[1][3] = 0.0f;
		m[2][0] = xzm-ysin;		m[2][1] = yzm+xsin;		m[2][2] = z2*omcs+cs;   m[2][3] = 0.0f;
	}

	void CopyMatrix3(cjMatrix3& rst) const
	{
		rst.m[0][0] = m[0][0];	rst.m[0][1] = m[1][0]; 	rst.m[0][2] = m[2][0]; 
		rst.m[1][0] = m[0][1]; 	rst.m[1][1] = m[1][1]; 	rst.m[1][2] = m[2][1]; 
		rst.m[2][0] = m[0][2];	rst.m[2][1] = m[1][2];	rst.m[2][2] = m[2][2];
	}

public:
	cjFLOAT m[3][4];
	static const cjMatrix Identity;
};


//
// cjFPlane
//

class CJCORE_API cjFPlane : public cjMem
{
	CJ_DECL_SERIALIZE_INLINE_BEGIN(cjFPlane)
		cjPODSerialize(stream, *((cjVec4*)&d));
	CJ_DECL_SERIALIZE_INLINE_END(cjFPlane)

public:
	enum ESIDE 
	{ 
		NO_SIDE, 
		POSITIVE_SIDE,
		NEGATIVE_SIDE,
		SIDE_MAX
	};

	cjFPlane() {}
	cjFPlane(cjFLOAT _x, cjFLOAT _y, cjFLOAT _z, cjFLOAT _w)
		: x(_x), y(_y), z(_z), w(_w) {}
	cjFPlane(const cjVec3& _v, cjFLOAT _w) : x(_v.x), y(_v.y), z(_v.z), w(_w) {}

    bool operator==(const cjFPlane& plane) const
		{ return x==plane.x && y==plane.y && z==plane.z && w==plane.w; }
    bool operator!=(const cjFPlane& plane) const
		{ return !(*this == plane); }

	void SetNormal(const cjVec3& vecNormal) { x=vecNormal.x, y=vecNormal.y, z=vecNormal.z; }
	const cjVec3 GetNormal() const { return cjVec3(x, y, z); }

	void SetDistance(cjFLOAT dist) { w = dist; }
	cjFLOAT GetDistance() const { return w; }
    cjFLOAT CalcDistance(const cjVec3& pt) const { return (x*pt.x + y*pt.y + z*pt.z) - w; }

    ESIDE TestSide(const cjVec3& pt) const
	{
		cjFLOAT dist = x * pt.x + y * pt.y + z * pt.z - w;

		if(dist < 0.0f)
			return NEGATIVE_SIDE;
		else if(dist > 0.0f)
			return POSITIVE_SIDE;
		else
			return NO_SIDE;
	}

public:
	cjFLOAT x, y, z, w;
};


class CJCORE_API cjFSphere : public cjFPlane
{
	CJ_DECL_SERIALIZE_INLINE_BEGIN(cjFSphere)
		cjPODSerialize(stream, (cjFPlane&)d);
	CJ_DECL_SERIALIZE_INLINE_END(cjFSphere)

public:
	cjFSphere() {}
	cjFSphere(cjINT32) : cjFPlane(0,0,0,0) {}
	cjFSphere(cjVec3 v, cjFLOAT radius) : cjFPlane(v, radius) {}

//	cjSphere TransformBy(const FMatrix& M) const;
};


//
// cjQuat
//

class CJCORE_API cjQuat : public cjMem
{
	CJ_DECL_SERIALIZE_INLINE_BEGIN(cjQuat)
		operator << (stream, *((cjVec4*)&d));
	CJ_DECL_SERIALIZE_INLINE_END(cjQuat)

public:
	cjQuat() {}
	cjQuat(cjFLOAT _x, cjFLOAT _y, cjFLOAT _z, cjFLOAT _w) :
		x(_x), y(_y), z(_z), w(_w) {}
	cjQuat(cjFLOAT angle, const cjVec3& axis) { MakeFromAngleAxis(angle, axis); }

	cjQuat operator+(const cjQuat& q) const	{ return cjQuat(x+q.x, y+q.y, z+q.z, w+q.w); }
	cjQuat operator-(const cjQuat& q) const	{ return cjQuat(x-q.x, y-q.y, z-q.z, w-q.w); }
	cjQuat operator*(const cjQuat& q) const	
		{ 
			return cjQuat(w*q.x + x*q.w + y*q.z - z*q.y,
				w*q.y - x*q.z + y*q.w + z*q.x,
				w*q.z + x*q.y - y*q.x + z*q.w,
				w*q.w - x*q.x - y*q.y - z*q.z );
		}
	cjQuat operator*(const cjFLOAT& s) const { return cjQuat(s*x, s*y, s*z, s*w); }
	cjQuat operator-() const { return cjQuat(x, y, z, -w); } // -cjQuat

	bool operator==(const cjQuat& q) const { return (x == q.x && y == q.y && z == q.z && w == q.w) ? true : false;	}
	bool operator!=(const cjQuat& q) const { return !operator==(q);	}
		
	static cjFLOAT Dot(const cjQuat& src, const cjQuat& dst) { return src.w * dst.w + src.x * dst.x + src.y * dst.y + src.z * dst.z; }
	cjFLOAT Dot(const cjQuat& dst) { return w * dst.w + x * dst.x + y * dst.y + z * dst.z; }
	    
	static cjQuat Exp(const cjQuat& q)
	{
		cjFLOAT sn, cs;
		cjFLOAT angle = cjSqrt(q.x*q.x + q.y*q.y + q.z*q.z);
		
		cjRadToSinCos(angle, sn, cs);

		// cjAbs(sn) is near zero, coff is 1
		cjFLOAT coeff = (cjAbs(sn) < QUAT_EPSILON ? 1.0f : sn/angle );
		cjQuat rst(cs, coeff * q.x, coeff * q.y, coeff * q.z);

		return rst;
	}

    static cjQuat Log(const cjQuat& q)
	{
		cjFLOAT angle = cjACos(q.w);
		cjFLOAT sn = cjSin(angle);

		cjFLOAT coeff = (cjAbs(sn) < QUAT_EPSILON) ? 1.0f : angle/sn;

		return cjQuat(coeff * q.x, coeff * q.y, coeff * q.z, 0.0f);
	}

	cjBool Normalize()
	{
		cjFLOAT squareSum = (cjFLOAT)(x*x+y*y+z*z+w*w);

		if(squareSum >= cjQUAT_EPSILON)
		{
			cjFLOAT s = 1.0f / (cjFLOAT)cjSqrt(squareSum);
			x *= s; y *= s; z *= s; w *= s;
			return cjTrue;
		}
		else 
		{	
			x = 0.0f; y = 0.0f; z = 0.1f; w = 0.0f;
			return cjFalse;
		}
	}

	cjQuat ToAngleAxis() const
	{
		cjFLOAT s = (cjFLOAT)cjSqrt(1.0f-w*w); // scale
		cjQuat angleAxis;

		if(s >= cjQUAT_EPSILON)
		{
			angleAxis.x = x / s;
			angleAxis.y = y / s;
			angleAxis.z = z / s;
			angleAxis.w = 2.0f * (cjFLOAT)cjACos(w);
		}
		else 
		{
			angleAxis.x = 0.0f;
			angleAxis.y = 0.0f;
			angleAxis.z = 1.0f;
			angleAxis.w = 0.0f; 
		}

		return angleAxis;
	};

	cjQuat AngleAxisToQuat() const
	{
		cjFLOAT s = (cjFLOAT)cjSin(w * 0.5f); // scale
		cjQuat q;

		q.x = x * s;
		q.y = y * s;
		q.z = z * s;
		q.w = (cjFLOAT)cjCos(w * 0.5f);

		return q;
	}

	cjVec3 RotateVector(const cjVec3& v) const
	{	
		// (q.w*q.w-qv.qv)v + 2(qv.v)qv + 2 q.w (qv x v)		
		cjVec3 qv(x, y, z);
		cjVec3 rst = qv.Cross(v) * w * 2.0f;

		rst += v * (w*w - qv.Dot(qv));
		rst += qv * (2.0f * qv.Dot(v));

		return rst;
	}

	static cjFLOAT SlerpCounterWrap(cjFLOAT t, cjFLOAT fCos);

	static cjQuat Slerp(cjFLOAT ratio, const cjQuat& st, const cjQuat& en);
    static void Slerp (cjFLOAT t, const cjQuat& p, const cjQuat& q, cjQuat* rst);
    static cjQuat Squad(cjFLOAT t, const cjQuat& p, const cjQuat& a, const cjQuat& b, const cjQuat& q)
	{
		return Slerp(2.0f*t*(1.0f-t), Slerp(t,p,q), Slerp(t,a,b));
	}

	static cjFLOAT Lerp(cjFLOAT v0, cjFLOAT v1, cjFLOAT fPerc) { return v0 + fPerc * (v1 - v0); }
		
    static cjQuat Intermediate(const cjQuat& q0, const cjQuat& q1, const cjQuat& q2)
	{
		// q0, q1, q2 must be unit quaternions
		cjQuat inv = InverseAxis(q1);
		cjQuat exp = Exp((Log(inv * q0) + Log(inv * q2)) * -0.25f);

		return q1 * exp;
	}
	
	cjFLOAT Length() const { return cjSqrt(x*x + y*y + z*z + w*w); }

	cjQuat& Conjugate(cjQuat& rst) const 
		{ rst.x = -x; rst.y = -y; rst.z = -z;  rst.w = w; return rst; }

	void Invert()
	{
		cjFLOAT scale = 1.0f / (x*x + y*y + z*z + w*w);		
		x=-x*scale; y=-y*scale; z=-z*scale; w=w*scale;
	}

	static cjQuat Invert(const cjQuat& in)
	{
		cjQuat inv = in;
		inv.Invert();

		return inv;
	}

	// quaternion can not represent 180 higher, so inverting axis is need
	// q must be unit length
	static cjQuat InverseAxis(const cjQuat& q)
	{
		return cjQuat(-q.x, -q.y, -q.z, q.w);
	}

	cjMatrix3& ToRotateMatrix(cjMatrix3& rst) const
	{
		cjFLOAT tx  = 2.0f * x;
		cjFLOAT ty  = 2.0f * y;
		cjFLOAT tz  = 2.0f * z;
		cjFLOAT twx = tx * w;
		cjFLOAT twy = ty * w;
		cjFLOAT twz = tz * w;
		cjFLOAT txx = tx * x;
		cjFLOAT txy = ty * x;
		cjFLOAT txz = tz * x;
		cjFLOAT tyy = ty * y;
		cjFLOAT tyz = tz * y;
		cjFLOAT tzz = tz * z;

		rst.m[0][0] = 1.0f-(tyy+tzz);	rst.m[1][0] = txy+twz;			rst.m[2][0] = txz-twy;
		rst.m[0][1] = txy-twz;			rst.m[1][1] = 1.0f-(txx+tzz);	rst.m[2][1] = tyz+twx;
		rst.m[0][2] = txz+twy;			rst.m[1][2] = tyz-twx;			rst.m[2][2] = 1.0f-(txx+tyy);

		return rst;
	}

	cjQuat& MakeFromAngleAxis(cjFLOAT rad, cjFLOAT _x, cjFLOAT _y, cjFLOAT _z)
	{
		cjFLOAT halfAngle = rad * 0.5f;
		cjFLOAT scale = cjSin(halfAngle);
    
		x = scale * _x;
		y = scale * _y;
		z = scale * _z;
		w = cjCos(halfAngle);
    
		return *this;
	}

	cjQuat& MakeFromAngleAxis(cjFLOAT rad, const cjVec3& axis)
	{
		cjFLOAT halfAngle = rad * 0.5f;
		cjFLOAT scale = cjSin(halfAngle);
    
		x = scale * axis.x;
		y = scale * axis.y;
		z = scale * axis.z;
		w = cjCos(halfAngle);
    
		return *this;
	}

	void MakeFrom3AxisAngle(cjFLOAT axisXAngle, cjFLOAT axisYAngle, cjFLOAT axisZAngle)
	{
		cjFLOAT fsin0, fcos0, fsin1, fcos1, fsin2, fcos2;

		cjRadToSinCos(axisXAngle * 0.5f, fsin0, fcos0);
		cjRadToSinCos(axisYAngle * 0.5f, fsin1, fcos1);
		cjRadToSinCos(axisZAngle * 0.5f, fsin2, fcos2);
				
		x = fcos2 * fcos1 * fsin0 - fsin2 * fsin1 * fcos0;
		y = fcos2 * fsin1 * fcos0 + fsin2 * fcos1 * fsin0;
		z = -fcos2 * fsin1 * fsin0 + fsin2 * fcos1 * fcos0;
		w = fcos2 * fcos1 * fcos0 + fsin2 * fsin1 * fsin0;
	}

	cjQuat& MakeFromRotation(const cjMatrix3& rotMat);

public:
	cjFLOAT x,y,z,w;		// axis, angle

	const static cjQuat INVALID;
	const static cjQuat IDENTITY;
	const static cjFLOAT QUAT_EPSILON;
};


//
// rotate 
//

class CJCORE_API cjRot : public cjMem
{
	CJ_DECL_SERIALIZE_INLINE_BEGIN(cjRot)
		stream << d.m_pitch << d.m_yaw << d.m_roll;
	CJ_DECL_SERIALIZE_INLINE_END(cjRot)

public:
	cjRot() {}
	cjRot(cjSWORD pitch, cjSWORD yaw, cjSWORD roll)
		: m_pitch(pitch), m_yaw(yaw), m_roll(roll) {}

	cjSWORD m_pitch, m_yaw, m_roll;		// 32765 ~ -32765 <-> 180 ~ -180 degree
};



//
// cjTransform
//

/*
	cjTransform tmA, tmB, tmC;
	cjMatrix matA = tmA, matB=tmB , matC

    (matC = matA * matB)  !=  (tmC = tmA * tmB)
*/

class CJCORE_API cjTransform : public cjMem
{
	CJ_DECL_SERIALIZE_INLINE_BEGIN(cjTransform)
		stream << d.m_rotate << d.m_pos << d.m_scale;
	CJ_DECL_SERIALIZE_INLINE_END(cjTransform)

public:
	cjTransform() {}

	static void SerializeRAry(cjSerializer& ser, cjTransform** d, cjINT32* count);

    bool operator!=(const cjTransform& dstTm) const;
    cjTransform operator*(const cjTransform& dstTm) const;
    inline cjVec3 operator*(const cjVec3& pt) const;

	void MakeIdentity();
    void Invert(cjTransform& rstTM) const;
	void Invert();

    static void Interpolate(cjTransform& rstTM, cjFLOAT ratio, const cjTransform& srcTM,
        const cjTransform& dstTM);

public:
	// keep member variable declaration order(cjMatrix3, cjVec3, cjFLOAT)
	// to minimize side effect in assembly
    cjMatrix3					m_rotate;
    cjVec3						m_pos;
    cjVec3						m_scale;
};

inline cjTransform cjTransform::operator*(const cjTransform& dstTM) const
{
    cjTransform rstTM;

    rstTM.m_scale	= m_scale * dstTM.m_scale;
	rstTM.m_rotate	= m_rotate * dstTM.m_rotate;
	rstTM.m_pos		= ((m_rotate * dstTM.m_pos) * m_scale) + m_pos;

	return rstTM;
}

inline cjVec3 cjTransform::operator*(const cjVec3& pt) const
{
    return (((m_rotate * pt) * m_scale) + m_pos);
}

inline cjMatrix34& cjMatrix34::operator =(const cjTransform& /*tm*/)
{
	cjAssert(0);
	//tm.m_rotate.GetRow(0, m[0][0], m[1][0], m[2][0]);
	//tm.m_rotate.GetRow(1, m[0][1], m[1][1], m[2][1]);
	//tm.m_rotate.GetRow(2, m[0][2], m[1][2], m[2][2]);
	//		
	//m[0][3] = tm.m_pos.x; m[1][3] = tm.m_pos.y;	m[2][3] = tm.m_pos.z;

	return *this;
}

void cjMatrix::operator= (const cjTransform& tm)
{
	const cjVec3& scale = tm.m_scale;	
	const cjFLOAT (*rm)[3] = tm.m_rotate.m;
		
	m[0][0] = rm[0][0] * scale.x;
	m[0][1] = rm[0][1] * scale.y;
	m[0][2] = rm[0][2] * scale.z;
	m[0][3] = 0.0f;

	m[1][0] = rm[1][0] * scale.x; 
	m[1][1] = rm[1][1] * scale.y; 
	m[1][2] = rm[1][2] * scale.z; 
	m[1][3] = 0.0f;

	m[2][0] = rm[2][0] * scale.x;
	m[2][1] = rm[2][1] * scale.y;
	m[2][2] = rm[2][2] * scale.z;
	m[2][3] = 0.0f;
	
	m[3][0] = tm.m_pos.x; 
	m[3][1] = tm.m_pos.y; 
	m[3][2] = tm.m_pos.z;
	m[3][3] = 1.0f;
}


//
// frustum
//

class CJCORE_API cjFrustum : public cjMem
{
	CJ_DECL_SERIALIZE_INLINE_BEGIN(cjFrustum)
		stream << d.m_left << d.m_right << d.m_top << d.m_bottom << d.m_near << d.m_far << d.m_ortho;
	CJ_DECL_SERIALIZE_INLINE_END(cjFrustum)

public:
    cjFLOAT	m_left;
    cjFLOAT	m_right;
    cjFLOAT	m_top;
    cjFLOAT	m_bottom;

    cjFLOAT	m_near;
    cjFLOAT	m_far;

	cjBYTE	m_ortho;

	void SetFov(cjFLOAT fov, cjFLOAT aspect=1.0f /*vert/hor*/);
	void SetFovY(cjFLOAT fovY, cjFLOAT aspect=1.0f /*hor/vert*/);	
};

CJCORE_API void cjCalcViewportArea(cjRect<cjFLOAT>& rstPort, cjINT32 areaX, 
	cjINT32 areaY, cjINT32 areaW, cjINT32 areaH, cjINT32 viewportW, 
	cjINT32 viewportH);


//
// box
//

class CJCORE_API cjFBox : public cjMem
{
	CJ_DECL_SERIALIZE_INLINE_BEGIN(cjFBox)
		cjPODSerialize(stream, d.m_min);
		cjPODSerialize(stream, d.m_max);
		cjPODSerialize(stream, d.m_isValid);
	CJ_DECL_SERIALIZE_INLINE_END(cjFBox)

public:
	cjFBox() { }
	cjFBox(const cjVec3& mi, const cjVec3& mx) : m_min(mi), m_max(mx), m_isValid(1) {}
	cjFBox(const cjVec3* points, cjINT32 count);

	void Init() { m_min = m_max = cjVec3(0,0,0); m_isValid = 0; }
	
	bool operator==(const cjFBox& rhs) const 
		{ return (m_min == rhs.m_min && m_max == rhs.m_max && m_isValid == rhs.m_isValid) ? cjTrue : cjFalse; }
	bool operator!=(const cjFBox& rhs) const { return !(*this == rhs); }

		// + cjVec3
	cjFBox& operator += (const cjVec3& rhs)
	{
		if(m_isValid)
		{
			m_min.x = ::cjMin(m_min.x, rhs.x); m_min.y = ::cjMin(m_min.y, rhs.y); m_min.z = ::cjMin(m_min.z, rhs.z);
			m_max.x = ::cjMin(m_max.x, rhs.x); m_max.y = ::cjMin(m_max.y, rhs.y); m_max.z = ::cjMin(m_max.z, rhs.z);
		}
		else
		{
			m_min = m_max = rhs;
			m_isValid = 1;
		}

		return *this;
	}

	cjFBox& operator +(const cjVec3& rhs) const
		{ return cjFBox(*this) += rhs; }
	
		// + cjFBox
	cjFBox& operator += (const cjFBox& rhs)
	{
		if(m_isValid && rhs.m_isValid)
		{
			m_min.x = ::cjMin(m_min.x, rhs.m_min.x); m_min.y = ::cjMin(m_min.y, rhs.m_min.y); m_min.z = ::cjMin(m_min.z, rhs.m_min.z);
			m_max.x = ::cjMin(m_max.x, rhs.m_max.x); m_max.y = ::cjMin(m_max.y, rhs.m_max.y); m_max.z = ::cjMin(m_max.z, rhs.m_max.z);
		}
		else
			*this = rhs;

		return *this;
	}
	cjFBox& operator +(const cjFBox& rhs) const
		{ return cjFBox(*this) += rhs; }

		// access
	const cjVec3& GetMin() const { return m_min; }
	const cjVec3& GetMax() const { return m_max; }
	const cjVec3 GetCenter() const { return cjVec3(m_min + m_max) * 0.5f; }

		// intersect
	cjBool Intersect(const cjFBox& rhs) const
	{
		if(m_min.x > rhs.m_max.x || rhs.m_min.x > m_max.x)
			return cjFalse;
		if(m_min.y > rhs.m_max.y || rhs.m_min.y > m_max.y)
			return cjFalse;
		if(m_min.z > rhs.m_max.z || rhs.m_min.z > m_max.z)
			return cjFalse;

		return cjTrue;
	}

public:
	cjVec3									m_min;
	cjVec3									m_max;

	cjBYTE									m_isValid;
};


//
// cjFBound
//

enum cjUniBoundDesc 
{
	cjUniBoundType_None			= 0,
	cjUniBoundType_Box			= 1,
	cjUniBoundType_Sphere		= 2,
	cjUniBoundType_Cylinder		= 3,
	cjUniBoundType_OBB			= 4,
	cjUniBoundType_Max
};


//
// cjFUniBoundDat
//

struct CJCORE_API cjFUniBoundDat : public cjMem
{
	// 32bit
	// reserved 24bit | serialize 8bit ([4] rev, [4bit] type)
	cjTFLAG							m_desc; // 0=none, 1=Box, 2=Sphere, 3=Cylinder, 4=obb
	
	union _boundUni_t
	{
							// none

		struct _box_t		// cjFBox
		{
			cjFLOAT					_min[3];	// center x,y,z
			cjFLOAT					_max[3];
		} m_box;

		struct _sphere_t	// cjFPlane, cjFSphere
		{
			cjFLOAT					_center[3];	// center x,y,z
			cjFLOAT					_radius;	// w
		} m_sphere;

		struct _obb_t
		{
			cjFLOAT					_center[3];	// center x,y,z
			cjFLOAT					_axis[3];
			cjFLOAT					_axisExtent[3];
		} m_obb;

		// struct _cylinder { };
	}								_uniDat;
};

inline void MakeBoundDatIdentity(cjFUniBoundDat& boundDat, cjUniBoundDesc type=cjUniBoundType_None)
{
	boundDat.m_desc = type;

	switch(type)
	{
	case cjUniBoundType_None	: break;
	case cjUniBoundType_Box		: cjMemZero(&boundDat._uniDat.m_box,	sizeof(boundDat._uniDat.m_box));	return;
	case cjUniBoundType_Sphere	: cjMemZero(&boundDat._uniDat.m_sphere, sizeof(boundDat._uniDat.m_sphere)); return;
	case cjUniBoundType_Cylinder: break;
	case cjUniBoundType_OBB		: cjMemZero(&boundDat._uniDat.m_obb,	sizeof(boundDat._uniDat.m_obb));	return;
	default:
		break;
	};
}

inline cjUniBoundDesc cjGetBoundType(const cjFUniBoundDat& boundDat)
{
	return (cjUniBoundDesc)(boundDat.m_desc & 0x0f);
}

CJCORE_API const cjVec3& cjGetCenter(const cjFUniBoundDat& boundDat);
CJCORE_API void cjSetCenter(cjFUniBoundDat& boundDat, cjFLOAT x, cjFLOAT y, cjFLOAT z);
CJCORE_API cjFLOAT cjGetRadius(const cjFUniBoundDat& boundDat);
CJCORE_API void cjSetRadius(cjFUniBoundDat& boundDat, cjFLOAT radius);

CJCORE_API cjFStream& operator<<(cjFStream& stream, cjFUniBoundDat& d);


//
// cjFBound
//
	
class CJCORE_API cjFBound : public cjMem
{
	CJ_DECL_SERIALIZE_INLINE_BEGIN(cjFBound)
		::operator<<(stream, d.m_boundDat);
	CJ_DECL_SERIALIZE_INLINE_END(cjFBound)

public:
	const cjVec3& GetCenter() const { return cjGetCenter(m_boundDat); }
	void SetCenter(const cjVec3& center) { cjSetCenter(m_boundDat, center.x, center.y, center.z); }
	void SetCenter(cjFLOAT x, cjFLOAT y, cjFLOAT z) { cjSetCenter(m_boundDat, x, y, z); }

	cjFLOAT GetRadius() const { return cjGetRadius(m_boundDat); }
	void SetRadius(cjFLOAT radius) { cjSetRadius(m_boundDat, radius); }
	void SetCenterAndRadius(const cjVec3& center, cjFLOAT radius)
	{
		cjSetCenter(m_boundDat, center.x, center.y, center.z);
		cjSetRadius(m_boundDat, radius);
	}

	void MakeIdentity(cjUniBoundDesc type=cjUniBoundType_None)
		{ MakeBoundDatIdentity(m_boundDat, type); }

public:
	cjFBound() {}
	cjFUniBoundDat					m_boundDat;
};


inline cjMatrix& cjMatPerspectiveFovRH(cjMatrix& rst, cjFLOAT fovY, cjFLOAT aspect, cjFLOAT nearP, cjFLOAT farP)
{
    cjFLOAT cotan = 1.0f / cjTan(fovY/2.0f);
    
    rst.m[0][0] = 1.0f / (aspect * cjTan(fovY/2.0f));
	rst.m[0][1] = 0.0f;
	rst.m[0][2] = 0.0f;
	rst.m[0][3] = 0.0f;

    rst.m[1][0] = 0.0f;
	rst.m[1][1] = 1.0f / cjTan(fovY/2.0f);
	rst.m[1][2] = 0.0f;
	rst.m[1][3] = 0.0f;

    rst.m[2][0] = 0.0f;
	rst.m[2][1] = 0.0f;
	rst.m[2][2] = farP / (nearP - farP);
	rst.m[2][3] = -1.0f;

    rst.m[3][0] = 0.0f;
	rst.m[3][1] = 0.0f;
	rst.m[3][2] = (farP * nearP) / (nearP - farP);
	rst.m[3][3] = 0.0f;


 //   rst.m[0][0] = cotan/aspect;
	//rst.m[0][1] = 0.0f;
	//rst.m[0][2] = 0.0f;
	//rst.m[0][3] = 0.0f;

 //   rst.m[1][0] = 0.0f;
	//rst.m[1][1] = cotan;
	//rst.m[1][2] = 0.0f;
	//rst.m[1][3] = 0.0f;

 //   rst.m[2][0] = 0.0f;
	//rst.m[2][1] = 0.0f;
	//rst.m[2][2] = -(farP+nearP)/(farP-nearP);
	//rst.m[2][3] = -(2.0f*farP*nearP)/(farP-nearP); 

 //   rst.m[3][0] = 0.0f;
	//rst.m[3][1] = 0.0f;
	//rst.m[3][2] = -1.0f;
	//rst.m[3][3] = 0.0f;  // in dx : m[3][2] = 1
    
    return rst;
}

inline cjMatrix& cjMatPerspectiveFovLH(cjMatrix& rst, cjFLOAT fovY, cjFLOAT aspect, cjFLOAT nearP, cjFLOAT farP)
{
    rst.m[0][0] = 1.0f / (aspect * cjTan(fovY/2.0f));
	rst.m[0][1] = 0.0f;
	rst.m[0][2] = 0.0f;
	rst.m[0][3] = 0.0f;

    rst.m[1][0] = 0.0f;
	rst.m[1][1] = 1.0f / cjTan(fovY/2.0f);
	rst.m[1][2] = 0.0f;                            
	rst.m[1][3] = 0.0f;

    rst.m[2][0] = 0.0f;
	rst.m[2][1] = 0.0f;
	rst.m[2][2] = farP / (farP - nearP);
	rst.m[2][3] = 1.0f;

    rst.m[3][0] = 0.0f;
	rst.m[3][1] = 0.0f;
	rst.m[3][2] = (farP * nearP) / (nearP - farP);
	rst.m[3][3] = 0.0f;
	
    //cjFLOAT cotan = 1.0f / cjTan(fovY/2.0f);    
    //rst.m[0][0] = cotan/aspect;   rst.m[0][1] = 0.0f;   rst.m[0][2] = 0.0f;                            rst.m[0][3] = 0.0f;
    //rst.m[1][0] = 0.0f;           rst.m[1][1] = cotan;  rst.m[1][2] = 0.0f;                            rst.m[1][3] = 0.0f;
    //rst.m[2][0] = 0.0f;           rst.m[2][1] = 0.0f;   rst.m[2][2] = -(farP+nearP)/(farP-nearP);      rst.m[2][3] = -1.0f; // in dx : 1
    //rst.m[3][0] = 0.0f;           rst.m[3][1] = 0.0f;   rst.m[3][2] = -(2.0f*farP*nearP)/(farP-nearP); rst.m[3][3] = 0.0f;
    
    return rst;
}

/*
	right handed coordinate view matrix
	
	default:
		left vector : (1,0,0)
		up   vector : (0,1,0)
		dir  vector : (0,0,1)
		
	dir = normal(LookAt - Eye)
	right = normal(cross(Up, dir))
	up = cross(dir, right)

	-right.x           up.x           -dir.x          0
	-right.y           up.y           -dir.y          0
	-right.z           up.z           -dir.z          0
	dot(right, eye)   -dot(up, eye)   dot(dir, eye)   1

*/
inline cjMatrix* cjMatViewLookAtRH(cjMatrix& matView, const cjVec3& camPos, const cjVec3& camLookAt, 
	const cjVec3& camUp, cjVec3* outR=cjNULL, cjVec3* outU=cjNULL, cjVec3* outD=cjNULL)
{
	cjVec3 dir = camLookAt - camPos;    // zaxis
	dir.Normalize();
		
	cjVec3 right = cjVec3::Cross(camUp, dir); 	
	cjVec3 upn = cjVec3::Cross(dir, right); // yaxis

	right.Normalize();
	upn.Normalize();

	if(outR)
		*outR = right;

	if(outU)
		*outU = upn;

	if(outD)
		*outD = dir;
				
	matView.m[0][0] = -right.x;
	matView.m[1][0] = -right.y;
	matView.m[2][0] = -right.z;
	matView.m[3][0] = cjVec3::Dot(right, camPos);
	
	matView.m[0][1] = upn.x;
	matView.m[1][1] = upn.y;
	matView.m[2][1] = upn.z;
	matView.m[3][1] = -cjVec3::Dot(upn,    camPos);

	matView.m[0][2] = -dir.x;
	matView.m[1][2] = -dir.y;
	matView.m[2][2] = -dir.z;	
	matView.m[3][2] = cjVec3::Dot(dir,   camPos);

	matView.m[0][3] = 0.0f;
	matView.m[1][3] = 0.0f;
	matView.m[2][3] = 0.0f;
	matView.m[3][3] = 1.0f;

	return &matView;
};


/*
	left handed coordinate view matrix (dx style)

	dir = normal(LokAt - Eye)
	right = normal(cross(Up, dir))
	up = cross(dir, right)
    
	 right.x           up.x           dir.x          0
	 right.y           up.y           dir.y          0
	 right.z           up.z           dir.z          0
	-dot(right, eye)  -dot(up, eye)  -dot(dir, eye)  1
*/

inline cjMatrix* cjMatViewLookAtLH(cjMatrix& matView, const cjVec3& camPos, const cjVec3& camLookAt, const cjVec3& camUp)
{
	cjVec3 dir = camLookAt - camPos;    // zaxis
	dir.Normalize();
		
	cjVec3 right = cjVec3::Cross(camUp, dir); 	
	cjVec3 upn = cjVec3::Cross(dir, right); // up

	right.Normalize();
	upn.Normalize();
			
	matView.m[0][0] = right.x;
	matView.m[1][0] = right.y;
	matView.m[2][0] = right.z;
	matView.m[3][0] = -cjVec3::Dot(right, camPos);
	
	matView.m[0][1] = upn.x;
	matView.m[1][1] = upn.y;
	matView.m[2][1] = upn.z;
	matView.m[3][1] = -cjVec3::Dot(upn,    camPos);

	matView.m[0][2] = dir.x;
	matView.m[1][2] = dir.y;
	matView.m[2][2] = dir.z;	
	matView.m[3][2] = -cjVec3::Dot(dir,   camPos);

	matView.m[0][3] = 0.0f;
	matView.m[1][3] = 0.0f;
	matView.m[2][3] = 0.0f;
	matView.m[3][3] = 1.0f;

	return &matView;
};


/*
	Float Compressor

	http://stackoverflow.com/questions/1659440/32-bit-to-16-bit-floating-point-conversion

	Complete conversion from single precision to half precision. 
	This is a direct copy from my SSE version, so it's branch-less. 
	It makes use of the fact that in GCC (-true == ~0), may be true for VisualStudio too but, I don't have a copy.

	So that's a lot to take in but, it handles all subnormal values, both infinities, quiet NaNs, signaling NaNs, 
	and negative zero. Of course full IEEE support isn't always needed. So compressing generic floats: 
*/

class cjFloat16Compressor : public cjMem
{
    union Bits
    {
        cjFLOAT f;
        cjINT32 si;
        cjUINT32 ui;
    };

    static cjINT32 const shift = 13;
    static cjINT32 const shiftSign = 16;

    static cjINT32 const infN = 0x7F800000; // flt32 infinity
    static cjINT32 const maxN = 0x477FE000; // max flt16 normal as a flt32
    static cjINT32 const minN = 0x38800000; // min flt16 normal as a flt32
    static cjINT32 const signN = 0x80000000; // flt32 sign bit

    static cjINT32 const infC = infN >> shift;
    static cjINT32 const nanN = (infC + 1) << shift; // minimum flt16 nan as a flt32
    static cjINT32 const maxC = maxN >> shift;
    static cjINT32 const minC = minN >> shift;
    static cjINT32 const signC = signN >> shiftSign; // flt16 sign bit

    static cjINT32 const mulN = 0x52000000; // (1 << 23) / minN
    static cjINT32 const mulC = 0x33800000; // minN / (1 << (23 - shift))

    static cjINT32 const subC = 0x003FF; // max flt32 subnormal down shifted
    static cjINT32 const norC = 0x00400; // min flt32 normal down shifted

    static cjINT32 const maxD = infC - maxC - 1;
    static cjINT32 const minD = minC - subC - 1;

public:

    static cjUINT16 comp(cjFLOAT value)
    {
        Bits v, s;
        v.f = value;
        cjUINT32 sign = v.si & signN;
        v.si ^= sign;
        sign >>= shiftSign; // logical shift
        s.si = mulN;
        s.si = (cjINT32)(s.f * v.f); // correct subnormals
        v.si ^= (s.si ^ v.si) & -(minN > v.si);
        v.si ^= (infN ^ v.si) & -((infN > v.si) & (v.si > maxN));
        v.si ^= (nanN ^ v.si) & -((nanN > v.si) & (v.si > infN));
        v.ui >>= shift; // logical shift
        v.si ^= ((v.si - maxD) ^ v.si) & -(v.si > maxC);
        v.si ^= ((v.si - minD) ^ v.si) & -(v.si > subC);
        return (cjUINT16)(v.ui | sign);
    }

    static cjFLOAT decomp(cjUINT16 value)
    {
        Bits v;
        v.ui = value;
        cjINT32 sign = v.si & signC;
        v.si ^= sign;
        sign <<= shiftSign;
        v.si ^= ((v.si + minD) ^ v.si) & -(v.si > subC);
        v.si ^= ((v.si + maxD) ^ v.si) & -(v.si > maxC);
        Bits s;
        s.si = mulC;
        s.f *= v.si;
        cjINT32 mask = -(norC > v.si);
        v.si <<= shift;
        v.si ^= (s.si ^ v.si) & mask;
        v.si |= sign;
        return v.f;
    }
};

class cjFloatCompressor : public cjMem
{
    union Bits
    {
        cjFLOAT f;
        cjINT32 si;
        cjUINT32 ui;
    };

    bool hasNegatives;
    bool noLoss;
    cjINT32 _maxF;
    cjINT32 _minF;
    cjINT32 _epsF;
    cjINT32 _maxC;
    cjINT32 _zeroC;
    cjINT32 _pDelta;
    cjINT32 _nDelta;
    int _shift;

    static cjINT32 const signF = 0x80000000;
    static cjINT32 const absF = ~signF;

public:


/*
	This forces all values into the accepted range, no support for NaNs, infinities or negative zero. 
	Epsilon is the smallest allowable value in the range. Precision is how many bits of precision to retain from the cjFLOAT. 
	While there are a lot of branches above, they are all static and will be cached by the branch predictor in the CPU.

	Of course if your values don't require logarithmic resolution approaching zero. 
	Then linearizing them to a fixed point format is much faster, as was already mentioned.
	I use the FloatCompressor (SSE version) in graphics library for reducing the size of linear cjFLOAT color values in memory. 
	Compressed floats have the advantage of creating small look-up tables for time consuming functions, 
	like gamma correction or transcendentals. Compressing linear sRGB values reduces to a max of 12 bits or a max value of 3011, 
	which is great for a look-up table size for to/from sRGB.
*/
    cjFloatCompressor(cjFLOAT min, cjFLOAT epsilon, cjFLOAT max, int precision)
    {
        // legal values
        // min <= 0 < epsilon < max
        // 0 <= precision <= 23
        _shift = 23 - precision;
        Bits v;
        v.f = min;
        _minF = v.si;
        v.f = epsilon;
        _epsF = v.si;
        v.f = max;
        _maxF = v.si;
        hasNegatives = _minF < 0;
        noLoss = _shift == 0;
        cjINT32 pepsU, nepsU;
        if(noLoss) {
            nepsU = _epsF;
            pepsU = _epsF ^ signF;
            _maxC = _maxF ^ signF;
            _zeroC = signF;
        } else {
            nepsU = cjUINT32(_epsF ^ signF) >> _shift;
            pepsU = cjUINT32(_epsF) >> _shift;
            _maxC = cjUINT32(_maxF) >> _shift;
            _zeroC = 0;
        }
        _pDelta = pepsU - _zeroC - 1;
        _nDelta = nepsU - _maxC - 1;
    }

    cjFLOAT clamp(cjFLOAT value)
    {
        Bits v;
        v.f = value;
        cjINT32 max = _maxF;
        if(hasNegatives)
            max ^= (_minF ^ _maxF) & -(0 > v.si);
        v.si ^= (max ^ v.si) & -(v.si > max);
        v.si &= -(_epsF <= (v.si & absF));
        return v.f;
    }

    cjUINT32 compress(cjFLOAT value)
    {
        Bits v;
        v.f = clamp(value);
        if(noLoss)
            v.si ^= signF;
        else
            v.ui >>= _shift;
        if(hasNegatives)
            v.si ^= ((v.si - _nDelta) ^ v.si) & -(v.si > _maxC);
        v.si ^= ((v.si - _pDelta) ^ v.si) & -(v.si > _zeroC);
        if(noLoss)
            v.si ^= signF;
        return v.ui;
    }

    cjFLOAT decompress(cjUINT32 value)
    {
        Bits v;
        v.ui = value;
        if(noLoss)
            v.si ^= signF;
        v.si ^= ((v.si + _pDelta) ^ v.si) & -(v.si > _zeroC);
        if(hasNegatives)
            v.si ^= ((v.si + _nDelta) ^ v.si) & -(v.si > _maxC);
        if(noLoss)
            v.si ^= signF;
        else
            v.si <<= _shift;
        return v.f;
    }

};


//
// cjQuatTM
//

class CJCORE_API cjQuatTM : public cjMem
{
	CJ_DECL_SERIALIZE_INLINE_BEGIN(cjQuatTM)
		stream << d.m_scale << d.m_rotate << d.m_pos;
	CJ_DECL_SERIALIZE_INLINE_END(cjQuatTM)

public:
	cjQuatTM() : m_scale(INVALID_SCL), m_rotate(INVALID_ROT), m_pos(INVALID_POS) {}
    cjQuatTM(const cjVec3& pos, const cjQuat& rot, const cjVec3& scale) : m_scale(scale), m_rotate(rot), m_pos(pos) {}
	
    const cjVec3& GetScale() const { return m_scale; }
    void SetScale(const cjVec3& scale) { m_scale = scale; }

    const cjQuat& GetRotate() const { return m_rotate; }
	void SetRotate(const cjQuat& rot) { m_rotate = rot; }
	void SetRotate(const cjMatrix3& rot) // convert matrix rotation to quaternion 
	{ 
		cjQuat quat;
		quat.MakeFromRotation(rot);
		SetRotate(m_rotate);
	}

    const cjVec3& GetPos() const { return m_pos; }
    void SetPos(const cjVec3& pos)	{ m_pos = pos; }

	cjBool IsScaleValid() const		{ return m_scale	!= INVALID_SCL ? cjTrue : cjFalse; }
    cjBool IsRotateValid() const	{ return m_rotate	!= INVALID_ROT ? cjTrue : cjFalse; }
	cjBool IsPosValid() const		{ return m_pos		!= INVALID_POS ? cjTrue : cjFalse; }
        
    cjBool IsTMInvalid() const		{ return !(IsScaleValid() || IsRotateValid() || IsPosValid()) ? cjTrue : cjFalse; }

    cjBool operator==(const cjQuatTM& tm) const
	{
		return (m_pos == tm.m_pos && m_rotate == tm.m_rotate &&	m_scale == tm.m_scale);
	}
    cjBool operator!=(const cjQuatTM& tm) const { return !(*this == tm); }
    cjQuatTM operator*(const cjQuatTM& tm) const;

	// invScale=1.0f/m_scale, invRot=InvAxis, pos = -pos;
    void InvertEachSRT(cjQuatTM& dstTM) const;	

	// invScale=1.0f/m_scale, invRot=InvAxis, pos = invScale * (invRot * -pos)
	void InvertHierSRT(cjQuatTM& dstTM) const;	

	static cjQuatTM Multiply(const cjQuatTM& src, const cjQuatTM& dst);

    void MakeInvalid()
	{
		SetScaleInvalid();
		SetRotateInvalid();
		SetPosInvalid();
	}

	cjVec3* GetScalePtr()		{ return &m_scale;	}
    cjQuat* GetRotatePtr()		{ return &m_rotate; }
	cjVec3* GetPosPtr()			{ return &m_pos;	}    

protected:
    cjVec3									m_scale;
    cjQuat									m_rotate;
	cjVec3									m_pos;

	void SetScaleInvalid()		{ m_scale	= INVALID_SCL; }
	void SetRotateInvalid()		{ m_rotate	= INVALID_ROT; }
	void SetPosInvalid()		{ m_pos		= INVALID_POS; }

    static const cjVec3			INVALID_POS;
    static const cjQuat			INVALID_ROT;
    static const cjVec3			INVALID_SCL;
};

//// in : 0.0~1.0
//inline cjBYTE cjConvUnitFloat2Byte(const cjFLOAT in)
//{
//	cjAssert( (in >= 0) && (in <= 1.0) );
//	return 255 * in;
//};


#endif


//
//
//inline cjQuat* cjQuatAdd(cjQuat* out, const cjQuat* lhs, const cjQuat* rhs)
//{
//    out->x = lhs->x + rhs->x;
//    out->y = lhs->y + rhs->y;
//    out->z = lhs->z + rhs->z;
//    out->w = lhs->w + rhs->w;
//   
//    return out;
//}
//
//inline cjQuat* cjQuatSub(cjQuat* out, const cjQuat* lhs, const cjQuat* rhs)
//{
//    out->x = lhs->x - rhs->x;
//    out->y = lhs->y - rhs->y;
//    out->z = lhs->z - rhs->z;
//    out->w = lhs->w - rhs->w;
//   
//    return out;
//}
//
//inline cjQuat* cjQuatMultiply(cjQuat* out, const cjQuat* lhs, const cjQuat* rhs)
//{
//    cjFLOAT x,y,z,w;
//
//    x = lhs->q[3] * rhs->q[0] + lhs->q[0] * rhs->q[3] + lhs->q[1] * rhs->q[2] - lhs->q[2] * rhs->q[1];
//    y = lhs->q[3] * rhs->q[1] + lhs->q[1] * rhs->q[3] + lhs->q[2] * rhs->q[0] - lhs->q[0] * rhs->q[2];
//    z = lhs->q[3] * rhs->q[2] + lhs->q[2] * rhs->q[3] + lhs->q[0] * rhs->q[1] - lhs->q[1] * rhs->q[0];
//    w = lhs->q[3] * rhs->q[3] - lhs->q[0] * rhs->q[0] - lhs->q[1] * rhs->q[1] - lhs->q[2] * rhs->q[2];
//    
//    out->x = x; out->y = y; out->z = z; out->w = w;
//    return out;
//}
// 

//inline cjQuat* cjQuatMakeFromAngleAndAxis(cjQuat* out, cjFLOAT rad, const cjVec3* axis)
//{
//    return cjQuatMakeFromAngleAndAxis(out, rad, axis->x, axis->y, axis->z);
//}
    
//inline cjQuat* cjQuatNormalize(cjQuat* out, const cjQuat* in)
//{
//    cjFLOAT scale = 1.0f / cjQuatLength(in);
//    out->x = in->q[0] * scale; out->y = in->q[1] * scale; out->z = in->q[2] * scale; out->w = in->q[3] * scale;
//    return out;
//}
    /*
inline cjVec3* cjQuatRotateVec3(cjVec3* out, const cjQuat* inQuat, const cjVec3* inVec)
{
	cjQuat rotatedQuat = {inVec->x, inVec->y, inVec->z, 0.0f };
    cjQuat invQuat;
    
    cjQuatMultiply(&rotatedQuat, inQuat, &rotatedQuat);
    cjQuatInvert(&invQuat, inQuat);
    cjQuatMultiply(&rotatedQuat, &rotatedQuat, &invQuat);
    
    out->x = rotatedQuat.x; out->y = rotatedQuat.y; out->z = rotatedQuat.z;
    return out;
}

inline cjVec4* cjQuatRotateVec4(cjVec4* out, const cjQuat* inQuat, const cjVec4* inVec)
{
	cjQuat rotatedQuat = { inVec->x, inVec->y, inVec->z, 0.0f };
    cjQuat invQuat;
    
    cjQuatMultiply(&rotatedQuat, inQuat, &rotatedQuat);
    cjQuatInvert(&invQuat, inQuat);
    cjQuatMultiply(&rotatedQuat, &rotatedQuat, &invQuat);
    
    out->x = rotatedQuat.x; out->y = rotatedQuat.y; out->z = rotatedQuat.z; out->w = inVec->w;
    return out;
}*/

// CJCORE_API cjQuat* cjQuatSlerp(cjQuat* rst, cjFLOAT ratio, const cjQuat& st, const cjQuat& en);

//
//inline cjMatrix3* cjMatrix3FromQuat(cjMatrix3* rst, const cjQuat* src)
//{
//    cjFLOAT tx  = 2.0f * src->x;
//    cjFLOAT ty  = 2.0f * src->y;
//    cjFLOAT tz  = 2.0f * src->z;
//    cjFLOAT twx = tx * src->w;
//    cjFLOAT twy = ty * src->w;
//    cjFLOAT twz = tz * src->w;
//    cjFLOAT txx = tx * src->x;
//    cjFLOAT txy = ty * src->x;
//    cjFLOAT txz = tz * src->x;
//    cjFLOAT tyy = ty * src->y;
//    cjFLOAT tyz = tz * src->y;
//    cjFLOAT tzz = tz * src->z;
//
//	rst->mrc[0][0] = 1.0f-(tyy+tzz);	rst->mrc[1][0] = txy+twz;			rst->mrc[2][0] = txz-twy;
//	rst->mrc[0][1] = txy-twz;			rst->mrc[1][1] = 1.0f-(txx+tzz);	rst->mrc[2][1] = tyz+twx;
//	rst->mrc[0][2] = txz+twy;			rst->mrc[1][2] = tyz-twx;			rst->mrc[2][2] = 1.0f-(txx+tyy);
//
//	return rst;
//}


//CJCORE_API cjQuat* cjQuatFromMatrixRotation(cjQuat* rst, const cjMatrix3* rotMat);

//inline cjQuat* cjQuatMakeFromAngleAndAxis(cjQuat* out, cjFLOAT rad, cjFLOAT x, cjFLOAT y, cjFLOAT z)
//{
//    cjFLOAT halfAngle = rad * 0.5f;
//    cjFLOAT scale = cjSin(halfAngle);
//    
//    out->x = scale * x;    out->y = scale * y;    out->z = scale * z;
//    out->w = cjCos(halfAngle);
//    
//    return out;
//}



//class CJCORE_API cjMatrix3 : public cjMem
//{
//public:
//	cjMatrix3() { }
//
//	cjMatrix3(cjFLOAT r00, cjFLOAT r01, cjFLOAT r02,
//			  cjFLOAT r10, cjFLOAT r11, cjFLOAT r12,
//			  cjFLOAT r20, cjFLOAT r21, cjFLOAT r22)
//	{
//		m[0][0] = r00; m[0][1] = r01;  m[0][2] = r02;
//		m[1][0] = r10; m[1][1] = r11;  m[1][2] = r12;
//		m[2][0] = r20; m[2][1] = r21;  m[2][2] = r22;
//	}
//
//	void SetIdentity()
//	{
//		m[0][0] = 1; m[0][1] = 0;  m[0][2] = 0;
//		m[1][0] = 0; m[1][1] = 1;  m[1][2] = 0;
//		m[2][0] = 0; m[2][1] = 0;  m[2][2] = 1;
//	}
//
//	cjMatrix3 operator*(cjMatrix3 rhs) const
//	{
//		cjMatrix3 rst;
//
//		rst.m[0][0] = m[0][0] * rhs.m[0][0] + m[0][1] * rhs.m[1][0] + m[0][2] * rhs.m[2][0];
//		rst.m[0][1] = m[0][0] * rhs.m[0][1] + m[0][1] * rhs.m[1][1] + m[0][2] * rhs.m[2][1];
//		rst.m[0][2] = m[0][0] * rhs.m[0][2] + m[0][1] * rhs.m[1][2] + m[0][2] * rhs.m[2][2];
//
//		rst.m[1][0] = m[1][0] * rhs.m[0][0] + m[1][1] * rhs.m[1][0] + m[1][2] * rhs.m[2][0];
//		rst.m[1][1] = m[1][0] * rhs.m[0][1] + m[1][1] * rhs.m[1][1] + m[1][2] * rhs.m[2][1];
//		rst.m[1][2] = m[1][0] * rhs.m[0][2] + m[1][1] * rhs.m[1][2] + m[1][2] * rhs.m[2][2];
//
//		rst.m[2][0] = m[2][0] * rhs.m[0][0] + m[2][1] * rhs.m[1][0] + m[2][2] * rhs.m[2][0];
//		rst.m[2][1] = m[2][0] * rhs.m[0][1] + m[2][1] * rhs.m[1][1] + m[2][2] * rhs.m[2][1];
//		rst.m[2][2] = m[2][0] * rhs.m[0][2] + m[2][1] * rhs.m[1][2] + m[2][2] * rhs.m[2][2];
//
//		return rst;
//	}
//
//	cjVec3 operator *(const cjVec3& rhs) const
//	{
//		return cjVec3(	m[0][0]*rhs.x + m[0][1]*rhs.y + m[0][2]*rhs.z,
//						m[1][0]*rhs.x + m[1][1]*rhs.y + m[1][2]*rhs.z,
//						m[2][0]*rhs.x + m[2][1]*rhs.y + m[2][2]*rhs.z );
//	}
//
//	void MakeAxisRot(cjFLOAT angle, const cjVec3& axis);
//
//
//	cjFLOAT						m[3][3];
//	const static cjMatrix3		identity;
//};



//
// cjMatrix3, cjMatrix
//

//
//
//
//// column x row (GL style)
//
//inline cjMatrix3* cjGLMatrix3Multiply(cjMatrix3* out, cjMatrix3* a, cjMatrix3* b)
//{
//    cjMatrix3 o;
//   
//    // column 0 x row 0~2
//    //       00 * 00           10 * 01           20*02
//    o.m[0] = a->m[0]*b->m[0] + a->m[3]*b->m[1] + a->m[6]*b->m[2];
//    o.m[3] = a->m[0]*b->m[3] + a->m[3]*b->m[4] + a->m[6]*b->m[5];
//    o.m[6] = a->m[0]*b->m[6] + a->m[3]*b->m[7] + a->m[6]*b->m[8];
//
//    // column 1 x row 0~2
//    //        01 * 00           11*01             21*02
//    o.m[1] = a->m[1]*b->m[0] + a->m[4]*b->m[1] + a->m[7]*b->m[2];
//    o.m[4] = a->m[1]*b->m[3] + a->m[4]*b->m[4] + a->m[7]*b->m[5];
//    o.m[7] = a->m[1]*b->m[6] + a->m[4]*b->m[7] + a->m[7]*b->m[8];
//
//    // column 2
//    o.m[2] = a->m[2]*b->m[0] + a->m[5]*b->m[1] + a->m[8]*b->m[2];
//    o.m[5] = a->m[2]*b->m[3] + a->m[5]*b->m[4] + a->m[8]*b->m[5];
//    o.m[8] = a->m[2]*b->m[6] + a->m[5]*b->m[7] + a->m[8]*b->m[8];
//    
//    *out = o;
//    return out;
//}
//
//// column x row
//inline cjMatrix* cjGLMatixMultiply(cjMatrix* out, cjMatrix* a, cjMatrix* b)
//{
//    cjMatrix o;
//   
//    // column 0 x row 0~3
//    //       00 * 00           10 * 01           20*02             30*03
//    o.m[0]  = a->m[0]*b->m[0] + a->m[4]*b->m[1] + a->m[8]*b->m[2] + a->m[12]*b->m[3];
//    o.m[4]  = a->m[0]*b->m[4] + a->m[4]*b->m[5] + a->m[8]*b->m[6] + a->m[12]*b->m[7];
//    o.m[8]  = a->m[0]*b->m[8] + a->m[4]*b->m[9] + a->m[8]*b->m[10] + a->m[12]*b->m[11];
//    o.m[12] = a->m[0]*b->m[12] + a->m[4]*b->m[13] + a->m[8]*b->m[14] + a->m[12]*b->m[15];
//
//    // column 1 x row 0~3
//    //        01 * 00           11*01             21*02             31*03
//    o.m[1]  = a->m[1]*b->m[0] + a->m[5]*b->m[1] + a->m[9]*b->m[2] + a->m[13]*b->m[3];
//    o.m[5]  = a->m[1]*b->m[4] + a->m[5]*b->m[5] + a->m[9]*b->m[6] + a->m[13]*b->m[7];
//    o.m[9]  = a->m[1]*b->m[8] + a->m[5]*b->m[9] + a->m[9]*b->m[10] + a->m[13]*b->m[11];
//    o.m[13] = a->m[1]*b->m[12] + a->m[5]*b->m[13] + a->m[9]*b->m[14] + a->m[13]*b->m[15];
//
//    // column 2
//    o.m[2]  = a->m[2]*b->m[0] + a->m[6]*b->m[1] + a->m[10]*b->m[2] + a->m[14]*b->m[3];
//    o.m[6]  = a->m[2]*b->m[4] + a->m[6]*b->m[5] + a->m[10]*b->m[6] + a->m[14]*b->m[7];
//    o.m[10] = a->m[2]*b->m[8] + a->m[6]*b->m[9] + a->m[10]*b->m[10] + a->m[14]*b->m[11];
//    o.m[14] = a->m[2]*b->m[12] + a->m[6]*b->m[13] + a->m[10]*b->m[14] + a->m[14]*b->m[15];
//    
//    // column 3
//    o.m[3]  = a->m[3]*b->m[0] + a->m[7]*b->m[1] + a->m[11]*b->m[2] + a->m[15]*b->m[3];
//    o.m[7]  = a->m[3]*b->m[4] + a->m[7]*b->m[5] + a->m[11]*b->m[6] + a->m[15]*b->m[7];
//    o.m[11] = a->m[3]*b->m[8] + a->m[7]*b->m[9] + a->m[11]*b->m[10] + a->m[15]*b->m[11];
//    o.m[15] = a->m[3]*b->m[12] + a->m[7]*b->m[13] + a->m[11]*b->m[14] + a->m[15]*b->m[15];
//
//    *out = o;
//    return out;
//}
//
//inline cjMatrix* cjGLMatrixPerspective(cjMatrix* out, cjFLOAT fovY, cjFLOAT aspect, cjFLOAT nearP, cjFLOAT farP)
//{
//    cjFLOAT cotan = 1.0f / cjTanf(fovY/2.0f);
//    
//    out->m[0] = cotan/aspect;   out->m[1] = 0.0f;   out->m[2] = 0.0f;                               out->m[3] = 0.0f;
//    out->m[4] = 0.0f;           out->m[5] = cotan;  out->m[6] = 0.0f;                               out->m[7] = 0.0f;
//    out->m[8] = 0.0f;           out->m[9] = 0.0f;   out->m[10] = (nearP+farP)/(nearP-farP);         out->m[11] = -1.0f; // in dx : 1
//    out->m[12] = 0.0f;          out->m[13] = 0.0f;  out->m[14] = (2.0f*nearP*farP)/(nearP-farP);    out->m[15] = 0.0f;
//    
//    return out;
//}
//
//CJCORE_API cjMatrix* cjMatrix4Inverse(cjMatrix* out, const cjMatrix* in);
//
//inline cjMatrix* cjMatrixScale(cjMatrix* out, cjFLOAT x, cjFLOAT y, cjFLOAT z)
//{
//    out->m[0] = x;      out->m[1] = 0.0f;   out->m[2] = 0.0f;   out->m[3] = 0.0f;
//    out->m[4] = 0.0f;   out->m[5] = y;      out->m[6] = 0.0f;   out->m[7] = 0.0f;
//    out->m[8] = 0.0f;   out->m[9] = 0.0f;   out->m[10] = z;     out->m[11] = 0.0f;
//    out->m[12] = 0.0f;  out->m[13] = 0.0f;  out->m[14] = 0.0f;  out->m[15] = 1.0f;
//    
//    return out;
//}
//
//inline cjMatrix* cjMatrixTranslation(cjMatrix* out, cjFLOAT x, cjFLOAT y, cjFLOAT z)
//{
//    out->m[0] = 1.0f;   out->m[1] = 0.0f;   out->m[2] = 0.0f;   out->m[3] = 0.0f;
//    out->m[4] = 0.0f;   out->m[5] = 1.0f;   out->m[6] = 0.0f;   out->m[7] = 0.0f;
//    out->m[8] = 0.0f;   out->m[9] = 0.0f;   out->m[10] = 1.0f;  out->m[11] = 0.0f;
//    out->m[12] = x;     out->m[13] = y;     out->m[14] = z;     out->m[15] = 1.0f;
//    
//    return out;
//}
//
//inline cjMatrix3* cjMatrix3Transpose(cjMatrix3* out, const cjMatrix3* in)
//{
//    cjFLOAT tmp;
//
//    out->m[0] = in->m[0];
//
//    tmp = in->m[1], out->m[1] = in->m[3], out->m[3] = tmp;
//    tmp = in->m[2], out->m[2] = in->m[6], out->m[6] = tmp;
//    tmp = in->m[3], out->m[3] = in->m[12], out->m[12] = tmp;
//
//    out->m[4] = in->m[4];
//    tmp = in->m[5], out->m[5] = in->m[7], out->m[7] = tmp;
//
//    out->m[8] = in->m[8];
//    
//    return out;
//}
//
//inline cjMatrix* cjMatrixTranspose(cjMatrix* out, const cjMatrix* in)
//{
//    cjFLOAT tmp;
//    out->m[0] = in->m[0];
//    tmp = in->m[1], out->m[1] = in->m[4], out->m[4] = tmp;
//    tmp = in->m[2], out->m[2] = in->m[8], out->m[8] = tmp;
//    tmp = in->m[3], out->m[3] = in->m[12], out->m[12] = tmp;
//
//    out->m[5] = in->m[5];
//    tmp = in->m[6], out->m[6] = in->m[9], out->m[9] = tmp;
//    tmp = in->m[7], out->m[7] = in->m[13], out->m[13] = tmp;
//
//    out->m[10] = in->m[10];
//    tmp = in->m[11], out->m[11] = in->m[14], out->m[14] = tmp;
//    
//    return out;
//}
//
//inline cjMatrix3* cjMatrix4TransposeToMatrix3(cjMatrix3* out, const cjMatrix* in)
//{
//    cjFLOAT tmp;
//    
//    out->m[0] = in->m[0];
//    tmp = in->m[1], out->m[1] = in->m[4], out->m[4] = tmp;
//    tmp = in->m[2], out->m[2] = in->m[8], out->m[6] = tmp;
//
//    out->m[4] = in->m[5];
//    tmp = in->m[9], out->m[7] = in->m[6], out->m[5] = tmp;
//
//    out->m[8] = in->m[10];
//    
//    return out;
//
//}
//
//inline cjBool cjMatrixCompare(const cjMatrix* left, const cjMatrix* right)
//{
//	for(cjINT32 x=0; x<16; x++)
//    {
//		if(left->m[x] != right->m[x])
//			return cjFalse;
//    }
//    
//    return cjTrue;
//}
//
//inline cjMatrix* cjMatrixAccumRotByAxis(cjMatrix* out, cjFLOAT rad, cjFLOAT x, cjFLOAT y, cjFLOAT z)
//{
//    cjFLOAT angleCos    = cjCosf(rad);
//    cjFLOAT invAngleCos = 1.0f - angleCos;
//    cjFLOAT angleSin    = cjSinf(rad);
//    
//    out->m[0] = angleCos + invAngleCos * x * x;
//    out->m[1] = invAngleCos * x * y + z * angleSin;
//    out->m[2] = invAngleCos * x * z - y * angleSin;
//    out->m[3] = 0.0f;
//    
//    out->m[4] = invAngleCos * x * y - z * angleSin;
//    out->m[5] = angleCos + invAngleCos * y * y;
//    out->m[6] = invAngleCos * y * z + x * angleSin;
//    out->m[7] = 0.0f;
//    
//    out->m[8] = invAngleCos * x * z + y * angleSin;
//    out->m[9] = invAngleCos * y * z - x * angleSin;
//    out->m[10] = angleCos + invAngleCos * z * z;
//    out->m[11] = 0.0f;
//    
//    out->m[12] = 0.0f; out->m[13] = 0.0f; out->m[14] = 0.0f; out->m[15] = 1.0f;
//
//    return out;
//}

//
//
//
//
//
//
//
//
//
//
//
//class CJCORE_API cjMatrix : public cjMem
//{
//public:
//	cjMatrix() {}
//	cjMatrix(cjFLOAT r00, cjFLOAT r01, cjFLOAT r02, cjFLOAT r03,
//			  cjFLOAT r10, cjFLOAT r11, cjFLOAT r12, cjFLOAT r13,
//			  cjFLOAT r20, cjFLOAT r21, cjFLOAT r22, cjFLOAT r23,
//			  cjFLOAT r30, cjFLOAT r31, cjFLOAT r32, cjFLOAT r33)
//	{
//		m[0][0] = r00; m[0][1] = r01;  m[0][2] = r02;  m[0][3] = r03;
//		m[1][0] = r10; m[1][1] = r11;  m[1][2] = r12;  m[1][3] = r13;
//		m[2][0] = r20; m[2][1] = r21;  m[2][2] = r22;  m[2][3] = r23;
//		m[3][0] = r30; m[3][1] = r31;  m[3][2] = r32;  m[3][3] = r33;
//	}
//
//	void SetIdentity()
//	{
//		m[0][0] = 1; m[0][1] = 0;  m[0][2] = 0;  m[0][3] = 0;
//		m[1][0] = 0; m[1][1] = 1;  m[1][2] = 0;  m[1][3] = 0;
//		m[2][0] = 0; m[2][1] = 0;  m[2][2] = 1;  m[2][3] = 0;
//		m[3][0] = 0; m[3][1] = 0;  m[3][2] = 0;  m[3][3] = 1;
//	}
//
//	cjMatrix operator*(cjMatrix rhs) const
//	{
//		cjMatrix rst;
//
//		rst.m[0][0] = m[0][0] * rhs.m[0][0] + m[0][1] * rhs.m[1][0] + m[0][2] * rhs.m[2][0] + m[0][3] * rhs.m[3][0];
//		rst.m[0][1] = m[0][0] * rhs.m[0][1] + m[0][1] * rhs.m[1][1] + m[0][2] * rhs.m[2][1] + m[0][3] * rhs.m[3][1];
//		rst.m[0][2] = m[0][0] * rhs.m[0][2] + m[0][1] * rhs.m[1][2] + m[0][2] * rhs.m[2][2] + m[0][3] * rhs.m[3][2];
//		rst.m[0][3] = m[0][0] * rhs.m[0][3] + m[0][1] * rhs.m[1][3] + m[0][2] * rhs.m[2][3] + m[0][3] * rhs.m[3][3];
//
//		rst.m[1][0] = m[1][0] * rhs.m[0][0] + m[1][1] * rhs.m[1][0] + m[1][2] * rhs.m[2][0] + m[1][3] * rhs.m[3][0];
//		rst.m[1][1] = m[1][0] * rhs.m[0][1] + m[1][1] * rhs.m[1][1] + m[1][2] * rhs.m[2][1] + m[1][3] * rhs.m[3][1];
//		rst.m[1][2] = m[1][0] * rhs.m[0][2] + m[1][1] * rhs.m[1][2] + m[1][2] * rhs.m[2][2] + m[1][3] * rhs.m[3][2];
//		rst.m[1][3] = m[1][0] * rhs.m[0][3] + m[1][1] * rhs.m[1][3] + m[1][2] * rhs.m[2][3] + m[1][3] * rhs.m[3][3];
//
//		rst.m[2][0] = m[2][0] * rhs.m[0][0] + m[2][1] * rhs.m[1][0] + m[2][2] * rhs.m[2][0] + m[2][3] * rhs.m[3][0];
//		rst.m[2][1] = m[2][0] * rhs.m[0][1] + m[2][1] * rhs.m[1][1] + m[2][2] * rhs.m[2][1] + m[2][3] * rhs.m[3][1];
//		rst.m[2][2] = m[2][0] * rhs.m[0][2] + m[2][1] * rhs.m[1][2] + m[2][2] * rhs.m[2][2] + m[2][3] * rhs.m[3][2];
//		rst.m[2][3] = m[2][0] * rhs.m[0][3] + m[2][1] * rhs.m[1][3] + m[2][2] * rhs.m[2][3] + m[2][3] * rhs.m[3][3];
//
//		rst.m[3][0] = m[3][0] * rhs.m[0][0] + m[3][1] * rhs.m[1][0] + m[3][2] * rhs.m[2][0] + m[3][3] * rhs.m[3][0];
//		rst.m[3][1] = m[3][0] * rhs.m[0][1] + m[3][1] * rhs.m[1][1] + m[3][2] * rhs.m[2][1] + m[3][3] * rhs.m[3][1];
//		rst.m[3][2] = m[3][0] * rhs.m[0][2] + m[3][1] * rhs.m[1][2] + m[3][2] * rhs.m[2][2] + m[3][3] * rhs.m[3][2];
//		rst.m[3][3] = m[3][0] * rhs.m[0][3] + m[3][1] * rhs.m[1][3] + m[3][2] * rhs.m[2][3] + m[3][3] * rhs.m[3][3];
//
//		return rst;
//	}
//
//	// assumes vector is cjVec3 as fnVec4(x,y,z, 1.0f)
//	cjVec3 operator *(const cjVec3& rhs) const
//	{
//		return cjVec3(	m[0][0]*rhs.x + m[0][1]*rhs.y + m[0][2]*rhs.z + m[0][3],
//						m[1][0]*rhs.x + m[1][1]*rhs.y + m[1][2]*rhs.z + m[1][3],
//						m[2][0]*rhs.x + m[2][1]*rhs.y + m[2][2]*rhs.z + m[2][3] );
//	}
//
//	cjFLOAT						m[4][4];
//	const static cjMatrix		identity;
//};
//

