
#include "cjCorePCH.h"
#include "cj3DMath.h"


//
// cjColor / cjColorF
//

const cjColor cjColor::BLACK = cjColor(0,0,0,0);
const cjColor cjColor::WHITE = cjColor(0xff, 0xff, 0xff, 0xff);

const cjColorF cjColorF::BLACK = cjColorF(0.0f,0.0f,0.0f,0.0f);
const cjColorF cjColorF::WHITE = cjColorF(1.0f,1.0f,1.0f,1.0f);
const cjColorF cjColorF::INVALID = cjColorF(CJ_FLOAT_INVALID,CJ_FLOAT_INVALID,CJ_FLOAT_INVALID,CJ_FLOAT_INVALID);


cjColor::cjColor(const cjColorF* color)
{
	if(color)
		this->operator =(color);
}

cjColor& cjColor::operator =(const cjColorF& rhs)
{
	b = (cjBYTE)(rhs.b * 255.0f);
	g = (cjBYTE)(rhs.g * 255.0f);
	r = (cjBYTE)(rhs.r * 255.0f);
	a = (cjBYTE)(rhs.a * 255.0f);

	return *this;
}


CJCORE_API void cjColorFSerHelper(cjSerializer& ser, cjColorF& clr, int type)
{
	if(type == 0) // 32F Raw
	{
		ser << clr; 
	}
	else if(type == 1) // 8Byte Unit Float
	{
		cjColor bc;

		if(ser.IsSaving())
			bc = clr;

		ser << bc;

		if(ser.IsLoading())
			clr = bc;
	}
	else if(type == 2) // 16F
	{
		cjAssert(!"not implement");
	}
}


//
// cjVec2,3,4
//
const cjVec2 cjVec2::ZERO(0,0);
const cjVec2 cjVec2::UNIT(1,1);
const cjVec2 cjVec2::INVALID(CJ_FLOAT_INVALID, CJ_FLOAT_INVALID);

const cjVec3 cjVec3::ZERO(0,0,0);
const cjVec3 cjVec3::UNIT(1,1,1);
const cjVec3 cjVec3::INVALID(CJ_FLOAT_INVALID, CJ_FLOAT_INVALID, CJ_FLOAT_INVALID);
const cjVec3 cjVec3::UNIT_X(1,0,0);
const cjVec3 cjVec3::UNIT_Y(0,1,0);
const cjVec3 cjVec3::UNIT_Z(0,0,1);

const cjVec4 cjVec4::ZERO(0,0,0,0);
const cjVec4 cjVec4::UNIT(1,1,1,1);
const cjVec4 cjVec4::INVALID(CJ_FLOAT_INVALID, CJ_FLOAT_INVALID, CJ_FLOAT_INVALID, CJ_FLOAT_INVALID);


//
// cjVec3
//

const cjVec3 cjGBaseCoordX(1, 0, 0);
const cjVec3 cjGBaseCoordY(0, 1, 0);
const cjVec3 cjGBaseCoordZ(0, 0, 1);

cjVec3* cjVec3::RotateByAxis(cjVec3& out, const cjVec3& in, const cjVec3& axis, cjFLOAT deltaRad)
{
	cjMatrix3 axisRot;

	axisRot.MakeRotationFromAxis(deltaRad, axis);
				
	out = axisRot * in; // row-major matrix x column vec3

	return &out;
}


//
// cjMatrix3, cjMatrix
//

const cjMatrix3 cjMatrix3::Identity = cjMatrix3(cjVec3(1,0,0), cjVec3(0,1,0), cjVec3(0,0,1));
const cjMatrix cjMatrix::Identity = cjMatrix(cjVec4(1,0,0,0), cjVec4(0,1,0,0), cjVec4(0,0,1,0), cjVec4(0,0,0,1));


void cjMatrix3::ToAngleAndAxis(cjFLOAT& angle, cjFLOAT& x, cjFLOAT& y, cjFLOAT& z) const
{
    // Let (x,y,z) be the unit-length axis and let A be an angle of rotation.
    // The rotation matrix is R = I + sin(A)*P + (1-cos(A))*P^2 where
    // I is the identity and
    //
    //       +-        -+
    //   P = |  0 +z -y |
    //       | -z  0 +x |
    //       | +y -x  0 |
    //       +-        -+
    //
    // Some algebra will show that
    //
    //   cos(A) = (trace(R)-1)/2  and  R - R^t = 2*sin(A)*P

    cjFLOAT tr = m[0][0]+m[1][1]+m[2][2];
    angle = cjACos(0.5f*(tr-1.0f));

    x = m[1][2]-m[2][1];
    y = m[2][0]-m[0][2];
    z = m[0][1]-m[1][0];

    cjFLOAT len = cjSqrt(x*x+y*y+z*z);

    const cjFLOAT epsilon = 1e-06f; // 10^-6 = 0.000001
    if(len > epsilon)
    {
        cjFLOAT fInvLength = 1.0f/len;
        x *= fInvLength;
        y *= fInvLength;
        z *= fInvLength;
    }
    else  
    {
		// angle is 0 or pi
        if(angle > 1.0f)  // any number strictly between 0 and pi works
        {
            // angle must be pi
            x = cjSqrt(0.5f*(1.0f+m[0][0]));
            y = cjSqrt(0.5f*(1.0f+m[1][1]));
            z = cjSqrt(0.5f*(1.0f+m[2][2]));

            // determine signs of axis components
            cjFLOAT tx, ty, tz;
            tx = m[0][0]*x+m[0][1]*y+m[0][2]*z-x;
            ty = m[1][0]*x+m[1][1]*y+m[1][2]*z-y;
            tz = m[2][0]*x+m[2][1]*y+m[2][2]*z-z;
            len = tx*tx+ty*ty+tz*tz;
            if(len < epsilon)
                return;

            z = -z;
            tx = m[0][0]*x+m[0][1]*y+m[0][2]*z-x;
            ty = m[1][0]*x+m[1][1]*y+m[1][2]*z-y;
            tz = m[2][0]*x+m[2][1]*y+m[2][2]*z-z;
            len = tx*tx+ty*ty+tz*tz;
            if(len < epsilon)
                return;

            y = -y;
            tx = m[0][0]*x+m[0][1]*y+m[0][2]*z-x;
            ty = m[1][0]*x+m[1][1]*y+m[1][2]*z-y;
            tz = m[2][0]*x+m[2][1]*y+m[2][2]*z-z;
            len = tx*tx+ty*ty+tz*tz;
            if(len < epsilon)
                return;
        }
        else
        {
            // Angle is zero, matrix is the identity, no unique axis
            x = 1.0f; y = 0.0f; z = 0.0f;
        }
    }
}

cjBool cjMatrix3::IsRotationMatrix(const cjMatrix3& matrix)
{
    // Rotation Matrix Requirement
	// 1. all columns must be unit vector(length=1.0) 
	// 2. perpendicular to each other

    const cjFLOAT rotEpsilon = 0.01f;

    for(cjUINT32 colSrc=0; colSrc<3; colSrc++)
    {
        cjVec3 vecColSrc;
		matrix.GetCol(colSrc, vecColSrc);

		// vecColSrc length must be nearest to 1.0
        if(cjAbs(vecColSrc.Dot(vecColSrc) - 1.0f) > rotEpsilon)
            return cjFalse;

        for(cjUINT32 colDst=0; colDst<3; colDst++)
        {			
            if(cjIsNan(matrix.GetEntry(colSrc, colDst))) // is valid cjFLOAT values
                return cjFalse;

			// compare each rotation axis
            if(colDst > colSrc)
            {
				cjVec3 vecColDst;
                matrix.GetCol(colDst, vecColDst);

                if(cjAbs(vecColSrc.Dot(vecColDst)) > rotEpsilon)
                    return cjFalse;
            }
        }
    }

    return cjTrue;	
}

cjBool cjMatrix3::GetEulerAngleXYZ(cjFLOAT& X, cjFLOAT& Y, cjFLOAT& Z) const
{
    Y = -cjASin(m[0][2]);

    if( Y < CJ_HALF_PI )
    {
        if( Y > -CJ_HALF_PI )
        {
            X = -cjFastATan2(-m[1][2],m[2][2]);
            Z = -cjFastATan2(-m[0][1],m[0][0]);

            return cjTrue;
        }
        else
        {            
            cjFLOAT ty = cjFastATan2(m[1][0],m[1][1]);
            Z = 0.0f;  
            X = ty - Z;

            return cjFalse;
        }
    }
    else
    {        
        cjFLOAT ty = cjFastATan2(m[1][0],m[1][1]);
        Z = 0.0f;  
        X = Z - ty;

        return cjFalse;
    }
}

cjBool cjMatrix3::GetEulerAngleXZY(cjFLOAT& X, cjFLOAT& Z, cjFLOAT& Y) const
{    
	Z = cjASin(m[0][1]);

    if( Z < CJ_HALF_PI )
    {
        if( Z > -CJ_HALF_PI )
        {
            X = -cjFastATan2(m[2][1],m[1][1]);
            Y = -cjFastATan2(m[0][2],m[0][0]);

            return cjTrue;
        }
        else
        {            
            cjFLOAT ty = cjFastATan2(-m[2][0],m[2][2]);
            Y = 0.0f;  
            X = ty - Y;

            return cjFalse;
        }
    }
    else
    {        
        cjFLOAT ty = cjFastATan2(-m[2][0],m[2][2]);
        Y = 0.0f;  
        X = Y - ty;

        return cjFalse;
    }
}

cjBool cjMatrix3::GetEulerAngleYXZ(cjFLOAT& Y, cjFLOAT& X, cjFLOAT& Z) const
{
    X = cjASin(m[1][2]);

    if( X < CJ_HALF_PI )
    {
        if( X > -CJ_HALF_PI )
        {
            Y = -cjFastATan2(m[0][2],m[2][2]);
            Z = -cjFastATan2(m[1][0],m[1][1]);

            return cjTrue;
        }
        else
        {            
            cjFLOAT ty = cjFastATan2(-m[0][1],m[0][0]);
            Z = 0.0f;  
            Y = ty - Z;

            return cjFalse;
        }
    }
    else
    {        
        cjFLOAT ty = cjFastATan2(-m[0][1],m[0][0]);
        Z = 0.0f;  
        Y = Z - ty;

        return cjFalse;
    }
}

cjBool cjMatrix3::GetEulerAngleYZX(cjFLOAT& Y, cjFLOAT& Z, cjFLOAT& X) const
{
    Z = -cjASin(m[1][0]);

    if( Z < CJ_HALF_PI )
    {
        if( Z > -CJ_HALF_PI )
        {
            Y = -cjFastATan2(-m[2][0],m[0][0]);
            X = -cjFastATan2(-m[1][2],m[1][1]);

            return cjTrue;
        }
        else
        {            
            cjFLOAT ty = cjFastATan2(m[2][1],m[2][2]);
            X = 0.0f;  
            Y = ty - X;

            return cjFalse;
        }
    }
    else
    {        
        cjFLOAT ty = cjFastATan2(m[2][1],m[2][2]);
        X = 0.0f;  
        Y = X - ty;

        return cjFalse;
    }
}

cjBool cjMatrix3::GetEulerAngleZXY(cjFLOAT& Z, cjFLOAT& X, cjFLOAT& Y) const
{
    X = -cjASin(m[2][1]);

    if( X < CJ_HALF_PI )
    {
        if( X > -CJ_HALF_PI )
        {
            Z = -cjFastATan2(-m[0][1],m[1][1]);
            Y = -cjFastATan2(-m[2][0],m[2][2]);

            return cjTrue;
        }
        else
        {
            cjFLOAT ty = cjFastATan2(m[0][2],m[0][0]);
            Y = 0.0f;
            Z = ty - Y;

            return cjFalse;
        }
    }
    else
    {
        cjFLOAT ty = cjFastATan2(m[0][2],m[0][0]);
        Y = 0.0f;
        Z = Y - ty;

        return cjFalse;
    }
}

cjBool cjMatrix3::GetEulerAngleZYX(cjFLOAT& Z, cjFLOAT& Y, cjFLOAT& X) const
{
    Y = cjASin(m[2][0]);

    if( Y < CJ_HALF_PI )
    {
        if( Y > -CJ_HALF_PI )
        {
            Z = -cjFastATan2(m[1][0],m[0][0]);
            X = -cjFastATan2(m[2][1],m[2][2]);

            return cjTrue;
        }
        else
        {
            cjFLOAT ty = cjFastATan2(-m[0][1],m[0][2]);
            X = 0.0f;  
            Z = ty - X;

            return cjFalse;
        }
    }
    else
    {
        cjFLOAT ty = cjFastATan2(-m[0][1],m[0][2]);
        X = 0.0f;  
        Z = X - ty;

        return cjFalse;
    }
}

void cjMatrix3::MakeFromEulerAngleXYZ(cjFLOAT X, cjFLOAT Y, cjFLOAT Z)
{
    cjMatrix3 rotX, rotY, rotZ;
    
	rotX.MakeXRotation(X);
    rotY.MakeYRotation(Y);
    rotZ.MakeZRotation(Z);

    *this = rotX*(rotY*rotZ);
}

void cjMatrix3::MakeFromEulerAngleXZY(cjFLOAT X, cjFLOAT Z, cjFLOAT Y)
{
    cjMatrix3 rotX, rotY, rotZ;
    
	rotX.MakeXRotation(X);
    rotY.MakeYRotation(Y);
    rotZ.MakeZRotation(Z);

    *this = rotX*(rotZ*rotY);
}

void cjMatrix3::MakeFromEulerAngleYXZ(cjFLOAT Y, cjFLOAT X, cjFLOAT Z)
{
    cjMatrix3 rotX, rotY, rotZ;
    
	rotX.MakeXRotation(X);
    rotY.MakeYRotation(Y);
    rotZ.MakeZRotation(Z);

    *this = rotY*(rotX*rotZ);
}

void cjMatrix3::MakeFromEulerAngleYZX(cjFLOAT Y, cjFLOAT Z, cjFLOAT X)
{
    cjMatrix3 rotX, rotY, rotZ;
    
	rotX.MakeXRotation(X);
    rotY.MakeYRotation(Y);
    rotZ.MakeZRotation(Z);

    *this = rotY*(rotZ*rotX);
}

void cjMatrix3::MakeFromEulerAngleZXY(cjFLOAT Z, cjFLOAT X, cjFLOAT Y)
{
    cjMatrix3 rotX, rotY, rotZ;

    rotX.MakeXRotation(X);
    rotY.MakeYRotation(Y);
    rotZ.MakeZRotation(Z);

    *this = rotZ*(rotX*rotY);
}

void cjMatrix3::MakeFromEulerAngleZYX(cjFLOAT Z, cjFLOAT Y, cjFLOAT X)
{
    cjMatrix3 rotX, rotY, rotZ;

    rotX.MakeXRotation(X);
    rotY.MakeYRotation(Y);
    rotZ.MakeZRotation(Z);

    *this = rotZ*(rotY*rotX);
}


//
// cjQuat
//

const cjQuat cjQuat::INVALID = cjQuat(CJ_FLOAT_INVALID,CJ_FLOAT_INVALID,CJ_FLOAT_INVALID,CJ_FLOAT_INVALID);
const cjQuat cjQuat::IDENTITY = cjQuat(0, 0, 0, 1.0f);
const cjFLOAT cjQuat::QUAT_EPSILON = 1e-03f;

cjQuat& cjQuat::MakeFromRotation(const cjMatrix3& rotMat)
{  
	// from Ken Shoemake's article "Quaternion Calculus and Fast Animation"
	cjFLOAT tr = rotMat.m[0][0] + rotMat.m[1][1] + rotMat.m[2][2];
    cjFLOAT fRoot;

    if(tr > 0.0f)
    {
        fRoot = cjSqrt(tr+1.0f);

		w = 0.5f * fRoot;
        fRoot = 0.5f / fRoot;

        x = (rotMat.m[2][1] - rotMat.m[1][2]) * fRoot;
        y = (rotMat.m[0][2] - rotMat.m[2][0]) * fRoot;
        z = (rotMat.m[1][0] - rotMat.m[0][1]) * fRoot;
    }
    else
    {
        // |w| <= 1/2
        static cjINT32 next[3] = { 1, 2, 0 };
        cjINT32 i = 0;
        
		if(rotMat.m[1][1] > rotMat.m[0][0])
            i = 1;

        if(rotMat.m[2][2] > rotMat.m[i][i]) 
            i = 2;

        int j = next[i]; int k = next[j];

        fRoot = cjSqrt(rotMat.m[i][i] - rotMat.m[j][j] - rotMat.m[k][k] + 1.0f);

        cjFLOAT* quat[3] = { &x, &y, &z };
        *quat[i] = 0.5f * fRoot;
        fRoot = 0.5f / fRoot;

		w = (rotMat.m[k][j] - rotMat.m[j][k]) * fRoot;
        *quat[j] = (rotMat.m[j][i] + rotMat.m[i][j]) * fRoot;
        *quat[k] = (rotMat.m[k][i] + rotMat.m[i][k]) * fRoot;
    }

	return *this;
}

cjFLOAT cjQuat::SlerpCounterWrap(cjFLOAT t, cjFLOAT fCos) 
{
    const cjFLOAT ATTENUATION = 0.82279687f;
    const cjFLOAT WORST_CASE_SLOPE = 0.58549219f;

    cjFLOAT factor = 1.0f - ATTENUATION * fCos;
    factor *= factor;
    cjFLOAT k = WORST_CASE_SLOPE * factor;

    return t*(k*t*(2.0f*t - 3.0f) + 1.0f + k);
}

cjQuat cjQuat::Slerp(cjFLOAT ratio, const cjQuat& st, const cjQuat& en)
{
    // from "Jonathan Blow, from his article - Hacking Quaternions"
    cjFLOAT fCos = Dot(st, en);

    cjFLOAT prime;

    if(ratio <= 0.5f)
        prime = SlerpCounterWrap(ratio, fCos);
    else
        prime = 1.0f - SlerpCounterWrap(1.0f - ratio, fCos);

	cjQuat rst;

	rst.x = Lerp(st.x, en.x, prime);
	rst.y = Lerp(st.y, en.y, prime);
	rst.z = Lerp(st.z, en.z, prime);
	rst.w = Lerp(st.w, en.w, prime);

	rst.Normalize();
	
	return rst;
}

void cjQuat::Slerp(cjFLOAT ratio, const cjQuat& st, const cjQuat& en, cjQuat* rst)
{
	cjAssert(rst);
    
    cjFLOAT fCos = Dot(st, en);

    cjFLOAT prime;

    if(ratio <= 0.5f) 
        prime = SlerpCounterWrap(ratio, fCos);
    else
        prime = 1.0f - SlerpCounterWrap(1.0f - ratio, fCos);

	rst->x = Lerp(st.x, en.x, prime);
	rst->y = Lerp(st.y, en.y, prime);
	rst->z = Lerp(st.z, en.z, prime);
    rst->w = Lerp(st.w, en.w, prime);

    rst->Normalize();
}


//
// cjTransform
//

bool cjTransform::operator!=(const cjTransform &dstTM) const
{
    return m_rotate != dstTM.m_rotate || m_pos != dstTM.m_pos || m_scale != dstTM.m_scale;
}

void cjTransform::SerializeRAry(cjSerializer& ser, cjTransform** d, cjINT32* count)
{
	if(ser.IsQuerySerObjAndStr())
		return;

	cjINT32 serCount;

	if(ser.IsSaving())
		serCount = *count;

	ser << cjFStreamCmprI32(serCount);

	if(ser.IsLoading())
	{
		cjAssert(*d == 0);

		if(serCount > 0)
		{
			*d = new cjTransform[serCount];
			for(cjINT32 i=0; i<serCount; i++)
				ser << (*d)[i];
		}

		if(count)
			*count = serCount;
	}
	else if(ser.IsSaving())
	{
		for(cjINT32 i=0; i<serCount; i++)
			ser << (*d)[i];
	}	
}

void cjTransform::MakeIdentity()
{
	m_rotate	= cjMatrix3::Identity;
    m_pos		= cjVec3::ZERO;
    m_scale		= cjVec3::UNIT;
}

void cjTransform::Invert(cjTransform& rstTM) const
{
	cjAssert(&rstTM != this);

	m_rotate.Transpose(rstTM.m_rotate);
    rstTM.m_scale	= cjVec3::UNIT / m_scale;  // 1.0f/scale
    rstTM.m_pos		= (rstTM.m_rotate * (-m_pos)) * rstTM.m_scale;
}

void cjTransform::Invert()
{
	cjTransform invTM;

	m_rotate.Transpose(invTM.m_rotate);
    invTM.m_scale	= cjVec3::UNIT / m_scale; // 1.0f/scale
    invTM.m_pos		= (invTM.m_rotate * (-m_pos)) * invTM.m_scale;

	*this = invTM;
}

void cjTransform::Interpolate(cjTransform& rstTM, cjFLOAT ratio, 
	const cjTransform& srcTM, const cjTransform& dstTM)
{
    if(ratio <= 0.f)
        rstTM = srcTM;
    else if(ratio >= 1.f)
        rstTM = dstTM;
    else
    {
        rstTM.m_scale = cjLerp(ratio, srcTM.m_scale, dstTM.m_scale);
        rstTM.m_pos = cjLerp(ratio, srcTM.m_pos, dstTM.m_pos);

        cjQuat srcQuat; srcQuat.MakeFromRotation(srcTM.m_rotate);
        cjQuat dstQuat; dstQuat.MakeFromRotation(dstTM.m_rotate);

        cjQuat slerpQuat; slerpQuat.Slerp(ratio, srcQuat, dstQuat);
		slerpQuat.ToRotateMatrix(rstTM.m_rotate);
    }
}


//
// cjFrustum
//

void cjFrustum::SetFov(cjFLOAT fov, cjFLOAT aspect)
{
	m_right		= cjTan(fov) / 2.0f;
	m_left		= -m_right;
	m_top		= m_right / aspect;
	m_bottom	= m_left / aspect;
}

void cjFrustum::SetFovY(cjFLOAT fovY, cjFLOAT aspect)
{
	m_top		= cjTan(fovY) / 2.0f;
	m_bottom	= -m_top;
	m_left		= m_bottom * aspect;
	m_right		= m_top * aspect;
}

CJCORE_API void cjCalcViewportArea(cjRect<cjFLOAT>& rstPort, cjINT32 areaX, cjINT32 areaY,
	cjINT32 areaW, cjINT32 areaH, cjINT32 viewportW, cjINT32 viewportH)
{
	rstPort.m_left	 = (cjFLOAT)areaX/viewportW;
	rstPort.m_top	 = 1.0f - ((cjFLOAT)areaY/viewportH);
	rstPort.m_right	 = rstPort.m_left + (cjFLOAT)areaW/viewportW;
	rstPort.m_bottom = rstPort.m_top  - (cjFLOAT)areaH/viewportH;
}


//
// cjFUniBoundDat
//

CJCORE_API cjFStream& operator<<(cjFStream& stream, cjFUniBoundDat& d)
{
	cjBYTE type = d.m_desc & 0xff;
	stream << type;	// low(4):type, high(4): rev

	switch(type & 0xf)
	{
	case cjUniBoundType_Box:
		cjPODSerialize(stream, d._uniDat.m_box);
		break;

	case cjUniBoundType_Sphere:
		cjPODSerialize(stream, d._uniDat.m_sphere);
		break;

	default:
		break;
	}

	return stream;
}

CJCORE_API const cjVec3& cjGetCenter(const cjFUniBoundDat& boundDat)
{
	switch(boundDat.m_desc & 0xf)
	{
	case cjUniBoundType_Box:
		return cjVec3::ZERO;

	case cjUniBoundType_Sphere:
		return *((const cjVec3*)(cjFLOAT*)boundDat._uniDat.m_sphere._center);

	default:
		break;
	}

	return cjVec3::ZERO;
}

CJCORE_API void cjSetCenter(cjFUniBoundDat& boundDat, cjFLOAT x, cjFLOAT y, cjFLOAT z)
{
	switch(boundDat.m_desc & 0xf)
	{
	case cjUniBoundType_Box:
		break;

	case cjUniBoundType_Sphere:
		{
			cjFLOAT* centerAry = (cjFLOAT*)boundDat._uniDat.m_sphere._center;
			*centerAry++ = x;
			*centerAry++ = y;
			*centerAry = z;
		};
		break;

	default:
		break;
	}
}

CJCORE_API cjFLOAT cjGetRadius(const cjFUniBoundDat& boundDat)
{
	switch(boundDat.m_desc & 0xf)
	{
	case cjUniBoundType_Box:
		{
			const cjFLOAT* mi = boundDat._uniDat.m_box._min;
			const cjFLOAT* mx = boundDat._uniDat.m_box._max;

			cjFLOAT halfX = (*mx++ - *mi++) * 0.5f;
			cjFLOAT halfY = (*mx++ - *mi++) * 0.5f;
			cjFLOAT halfZ = (*mx - *mi) * 0.5f;

			return cjGetLengthVec3(halfX, halfY, halfZ);
		}

	case cjUniBoundType_Sphere:
		return boundDat._uniDat.m_sphere._radius;
		
	default: 
		break;
	}

	return 0.0f;
}

CJCORE_API void cjSetRadius(cjFUniBoundDat& boundDat, cjFLOAT radius)
{
	radius = cjAbs(radius);

	switch(boundDat.m_desc & 0xf)
	{
	case cjUniBoundType_Sphere:
		{
			boundDat._uniDat.m_sphere._radius = radius;
			return;
		}

	case cjUniBoundType_Box:
		{
			cjFLOAT* mi = boundDat._uniDat.m_box._min;
			cjFLOAT* mx = boundDat._uniDat.m_box._max;

			if(radius == 0.0f)
			{
				*mi++ = 0.0f; *mi++ = 0.0f; *mi = 0.0f;
				*mx++ = 0.0f; *mx++ = 0.0f; *mx = 0.0f;
				return;
			}

			cjFLOAT oldRadius = cjGetRadius(boundDat);
			cjFLOAT factor = oldRadius == 0.0f ? 0.0f : oldRadius / radius;

			*mi++ *= factor; *mi++ *= factor; *mi *= factor;
			*mx++ *= factor; *mx++ *= factor; *mx *= factor;
		}
		break;
		
	default: 
		break;
	}
}


//
// cjFBound
//


//
// cjPlane
//

//
// cjFBox
//

//
// cjBSphere
//
//
//const cjFLOAT cjBSphere::BOUND_EPLISION				= 1e-04f; // 1*10^-4. same as .0001
//const cjFLOAT cjBSphere::BOUND_MERGE_EPLISION		= 1e-06f; // 1*10^-6. same as .000001
//
//
//cjBool cjBSphere::operator==(const cjBSphere& bound)
//{
//    return (m_radius == bound->m_radius && m_center == bound->m_center);
//}
//
//cjBool cjBSphere::operator<=(const cjBSphere& bound)
//{
//    cjVec3 diff = bound->m_center - m_center;
//    return diff.Length() + m_radius <= bound->m_radius + BOUND_EPLISION;
//}
//
//cjBool cjBSphere::operator>=(const cjBSphere& bound)
//{
//    cjVec3 diff = bound->m_center - m_center;
//    return diff.Length() + bound->m_radius <= m_radius + BOUND_EPLISION;
//}
//
//cjPlane::ESIDE cjBSphere::TestSide(const cjPlane& plane) const
//{
//    cjFLOAT dist= plane.CalcDistance(m_center);
//
//    if(dist <= -m_radius)
//        return cjPlane::NEGATIVE_SIDE;
//    else if(dist >= m_radius)
//        return cjPlane::POSITIVE_SIDE;
//    else
//        return cjPlane::NO_SIDE;
//}
//
//void cjBSphere::Merge(const cjBSphere* dstBound)
//{
//    cjVec3 diff = m_center - dstBound->m_center;
//    
//    cjFLOAT lenSqrt = diff.Dot(diff);
//    cjFLOAT deltaRadius = dstBound->m_radius - m_radius;
//    cjFLOAT deltaRadiusSqrt = deltaRadius * deltaRadius;
//    cjFLOAT lenSqrt2, ratio;
//    
//    if(deltaRadius >= 0.0f)
//    {
//        if(deltaRadiusSqrt >= lenSqrt)
//        {
//            m_center = dstBound->m_center;
//            m_radius = dstBound->m_radius;
//        }
//        else
//        {
//            lenSqrt2 = cjSqrt(lenSqrt);
//            if(lenSqrt2 > BOUND_MERGE_EPLISION)
//            {
//                ratio = (lenSqrt2 - deltaRadius)/(2.0f*lenSqrt2);
//                m_center = dstBound->m_center + ratio*diff;
//            }
//            m_radius = 0.5f*(dstBound->m_radius+lenSqrt2+m_radius);
//        }
//    }
//    else if(deltaRadiusSqrt < lenSqrt)
//    {
//        lenSqrt2 = cjSqrt(lenSqrt);
//        if(lenSqrt2 > BOUND_MERGE_EPLISION)
//        {
//            ratio = (lenSqrt2 - deltaRadius)/(2.0f*lenSqrt2);
//            m_center = dstBound->m_center + ratio*diff;
//        }
//        m_radius = 0.5f*(dstBound->m_radius+lenSqrt2+m_radius);
//    }
//	else
//	{
//		// 
//	}
//}
//
//



//
// cjQuatTM
//

const cjVec3 cjQuatTM::INVALID_SCL	= cjVec3(CJ_FLT_INVALID, CJ_FLT_INVALID, CJ_FLT_INVALID);
const cjQuat  cjQuatTM::INVALID_ROT = cjQuat(CJ_FLT_INVALID, CJ_FLT_INVALID, CJ_FLT_INVALID, CJ_FLT_INVALID);
const cjVec3  cjQuatTM::INVALID_POS = cjVec3(CJ_FLT_INVALID, CJ_FLT_INVALID, CJ_FLT_INVALID);


void cjQuatTM::InvertEachSRT(cjQuatTM& dstTM) const
{
    if(!IsScaleValid())
        dstTM.SetScaleInvalid();
    else		
        dstTM.SetScale(m_scale.Get1DivMe()); // SetScale(1.0 / m_scale)

    if(!IsRotateValid())
        dstTM.SetRotateInvalid();
    else
        dstTM.SetRotate(cjQuat::InverseAxis(m_rotate));

    if(!IsPosValid())
        dstTM.SetPosInvalid();
    else
        dstTM.SetPos(-m_pos);
}

void cjQuatTM::InvertHierSRT(cjQuatTM& dstTM) const
{
    cjVec3 invScale = cjVec3::UNIT;

    if(!IsScaleValid())
	{
		dstTM.SetScaleInvalid();
	}
    else
    {
        dstTM.SetScale(m_scale.Get1DivMe());
        invScale = dstTM.GetScale();
    }

    cjMatrix3 invRot = cjMatrix3::Identity;
    if(!IsRotateValid())
	{
        dstTM.SetRotateInvalid();
	}
    else
    {
        dstTM.SetRotate(cjQuat::InverseAxis(m_rotate));
        dstTM.GetRotate().ToRotateMatrix(invRot);
    }

    if(!IsPosValid())
        dstTM.SetPosInvalid();
    else
        dstTM.SetPos(invScale * (invRot * -m_pos));
}

cjQuatTM cjQuatTM::Multiply(const cjQuatTM& src, const cjQuatTM& dst)
{
    cjQuatTM rstTM;

    cjBool isScaleValid = src.IsScaleValid();
    if(!isScaleValid || !dst.IsScaleValid())
        rstTM.SetScaleInvalid();
    else
        rstTM.SetScale(src.GetScale() * dst.GetScale());

    cjBool isRotateValid = src.IsRotateValid();
    if(!isRotateValid || !dst.IsRotateValid())
	{
        rstTM.SetRotateInvalid();
	}
    else
    {
		cjQuat rot = src.GetRotate() * dst.GetRotate();
        rot.Normalize();
        rstTM.SetRotate(rot);
    }

    if(!src.IsPosValid() || !dst.IsPosValid())
    {
        rstTM.SetPosInvalid();
    }
    else
    {
        cjVec3 scale = cjVec3::UNIT;

        if(isScaleValid)
            scale = src.GetScale();

        cjMatrix3 rot = cjMatrix3::Identity;
        if(isRotateValid)
            src.GetRotate().ToRotateMatrix(rot);

        rstTM.SetPos(src.GetPos() + scale * (rot * dst.GetPos()));
    }

    return rstTM;
}

cjQuatTM cjQuatTM::operator*(const cjQuatTM& tm) const
{
    cjQuatTM rst;
    cjVec3 scale = cjVec3::UNIT;

    if(!IsScaleValid() || !tm.IsScaleValid())
    {
        rst.SetScaleInvalid();
    }
    else
    {
        rst.SetScale(m_scale * tm.GetScale());
        scale = rst.GetScale();
    }

    if(!IsRotateValid() || !tm.IsRotateValid())
    {
        rst.SetRotateInvalid();
    }
    else
    {
        cjQuat rot = m_rotate * tm.GetRotate();
        rot.Normalize();
        rst.SetRotate(rot);
    }

    if(!IsPosValid() || !tm.IsPosValid())
        rst.SetPosInvalid();
    else
        rst.SetPos(m_pos + tm.GetPos());

    return rst;
}


//
// 
//































//
////
//// cjMatrix3
////
//
//const cjMatrix3 cjMatrix3::identity(1.0f,0.0f,0.0f, 0.0f,1.0f,0.0f, 0.0f,0.0f,1.0f);
//
//void cjMatrix3::MakeAxisRot(cjFLOAT angle, const cjVec3& axis)
//{
//	cjFLOAT s, c;
//    cjSinCos(angle, s, c);
//
//    cjFLOAT omcs = 1.0f-c;
//    cjFLOAT x2 = axis.x * axis.x;
//    cjFLOAT y2 = axis.y * axis.y;
//    cjFLOAT z2 = axis.z * axis.z;
//    cjFLOAT xym = axis.x * axis.y*omcs;
//    cjFLOAT xzm = axis.x * axis.z*omcs;
//    cjFLOAT yzm = axis.y * axis.z*omcs;
//    cjFLOAT xsin = axis.x * s;
//    cjFLOAT ysin = axis.y * s;
//    cjFLOAT zsin = axis.z * s;
//    
//    m[0][0] = x2*omcs+c;
//    m[0][1] = xym+zsin;
//    m[0][2] = xzm-ysin;
//    m[1][0] = xym-zsin;
//    m[1][1] = y2*omcs+c;
//    m[1][2] = yzm+xsin;
//    m[2][0] = xzm+ysin;
//    m[2][1] = yzm-xsin;
//    m[2][2] = z2*omcs+c;
//}
//
//

//
// cjMatrix
//



//static const cjFLOAT _cjMatrix4_IDENTITY[16] = 
//{
//   1.0f, 0.0f, 0.0f, 0.0f,
//   0.0f,1.0f,0.0f,0.0f,
//   0.0f,0.0f,1.0f,0.0f,
//   0.0f,0.0f,0.0f,1.0f   
//};
//
//const cjMatrix& _cjMatrix::IDENTITY = *((const cjMatrix* const)_cjMatrix4_IDENTITY);
//
//
//CJCORE_API cjMatrix* cjMatrix4Inverse(cjMatrix* out, const cjMatrix* in)
//{
//    cjAssert(out != in);
//
//    //ftp://download.intel.com/design/PentiumIII/sml/24504301.pdf
//    
//    // by Cramer's rule
//   cjFLOAT tmp[12]; cjFLOAT src[16]; cjFLOAT det;
//   const cjFLOAT* mat = in->m;
//   cjFLOAT* dst = out->m;
//    
//   // transpose matrix
//    for(int i=0; i<4; i++)
//    {
//        src[i]       = mat[i*4];
//        src[i + 4]   = mat[i*4 + 1];
//        src[i + 8]   = mat[i*4 + 2];
//        src[i + 12]  = mat[i*4 + 3];
//    }
//   
//    /* calculate pairs for first 8 elements (cofactors) */
//    tmp[0] = src[10] * src[15];
//    tmp[1] = src[11] * src[14];
//    tmp[2] = src[9] * src[15];
//    tmp[3] = src[11] * src[13];
//    tmp[4] = src[9] * src[14];
//    tmp[5] = src[10] * src[13];
//    tmp[6] = src[8] * src[15];
//    tmp[7] = src[11] * src[12];
//    tmp[8] = src[8] * src[14];
//    tmp[9] = src[10] * src[12];
//    tmp[10] = src[8] * src[13];
//    tmp[11] = src[9] * src[12];
//    
//    /* calculate first 8 elements (cofactors) */
//    dst[0] = tmp[0]*src[5] + tmp[3]*src[6] + tmp[4]*src[7];
//    dst[0] -= tmp[1]*src[5] + tmp[2]*src[6] + tmp[5]*src[7];
//    dst[1] = tmp[1]*src[4] + tmp[6]*src[6] + tmp[9]*src[7];
//    dst[1] -= tmp[0]*src[4] + tmp[7]*src[6] + tmp[8]*src[7];
//    dst[2] = tmp[2]*src[4] + tmp[7]*src[5] + tmp[10]*src[7];
//    dst[2] -= tmp[3]*src[4] + tmp[6]*src[5] + tmp[11]*src[7];
//    dst[3] = tmp[5]*src[4] + tmp[8]*src[5] + tmp[11]*src[6];
//    dst[3] -= tmp[4]*src[4] + tmp[9]*src[5] + tmp[10]*src[6];
//    dst[4] = tmp[1]*src[1] + tmp[2]*src[2] + tmp[5]*src[3];
//    dst[4] -= tmp[0]*src[1] + tmp[3]*src[2] + tmp[4]*src[3];
//    dst[5] = tmp[0]*src[0] + tmp[7]*src[2] + tmp[8]*src[3];
//    dst[5] -= tmp[1]*src[0] + tmp[6]*src[2] + tmp[9]*src[3];
//    dst[6] = tmp[3]*src[0] + tmp[6]*src[1] + tmp[11]*src[3];
//    dst[6] -= tmp[2]*src[0] + tmp[7]*src[1] + tmp[10]*src[3];
//    dst[7] = tmp[4]*src[0] + tmp[9]*src[1] + tmp[10]*src[2];
//    dst[7] -= tmp[5]*src[0] + tmp[8]*src[1] + tmp[11]*src[2];
//    
//    /* calculate pairs for second 8 elements (cofactors) */
//    tmp[0] = src[2]*src[7];
//    tmp[1] = src[3]*src[6];
//    tmp[2] = src[1]*src[7];
//    tmp[3] = src[3]*src[5];
//    tmp[4] = src[1]*src[6];
//    tmp[5] = src[2]*src[5];
//    tmp[6] = src[0]*src[7];
//    tmp[7] = src[3]*src[4];
//    tmp[8] = src[0]*src[6];
//    tmp[9] = src[2]*src[4];
//    tmp[10] = src[0]*src[5];
//    tmp[11] = src[1]*src[4];
//    
//    /* calculate second 8 elements (cofactors) */
//    dst[8] = tmp[0]*src[13] + tmp[3]*src[14] + tmp[4]*src[15];
//    dst[8] -= tmp[1]*src[13] + tmp[2]*src[14] + tmp[5]*src[15];
//    dst[9] = tmp[1]*src[12] + tmp[6]*src[14] + tmp[9]*src[15];
//    dst[9] -= tmp[0]*src[12] + tmp[7]*src[14] + tmp[8]*src[15];
//    dst[10] = tmp[2]*src[12] + tmp[7]*src[13] + tmp[10]*src[15];
//    dst[10]-= tmp[3]*src[12] + tmp[6]*src[13] + tmp[11]*src[15];
//    dst[11] = tmp[5]*src[12] + tmp[8]*src[13] + tmp[11]*src[14];
//    dst[11]-= tmp[4]*src[12] + tmp[9]*src[13] + tmp[10]*src[14];
//    dst[12] = tmp[2]*src[10] + tmp[5]*src[11] + tmp[1]*src[9];
//    dst[12]-= tmp[4]*src[11] + tmp[0]*src[9] + tmp[3]*src[10];
//    dst[13] = tmp[8]*src[11] + tmp[0]*src[8] + tmp[7]*src[10];
//    dst[13]-= tmp[6]*src[10] + tmp[9]*src[11] + tmp[1]*src[8];
//    dst[14] = tmp[6]*src[9] + tmp[11]*src[11] + tmp[3]*src[8];
//    dst[14]-= tmp[10]*src[11] + tmp[2]*src[8] + tmp[7]*src[9];
//    dst[15] = tmp[10]*src[10] + tmp[4]*src[8] + tmp[9]*src[9];
//    dst[15]-= tmp[8]*src[9] + tmp[11]*src[10] + tmp[5]*src[8];
//    
//    /* calculate determinant */
//    det=src[0]*dst[0]+src[1]*dst[1]+src[2]*dst[2]+src[3]*dst[3];
//
//    /* calculate matrix inverse */
//    det = 1/det;
//    for(int j = 0; j < 16; j++)
//        dst[j] *= det;
//    
//    return out;
//}

