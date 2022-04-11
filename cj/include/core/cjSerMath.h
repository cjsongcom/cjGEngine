
#ifndef _CJSERMATH_H_
#define _CJSERMATH_H_


// Bezier Spline (B-Spline)

//
// cjBSplineBasisF3
//

class cjBSplineBasisF3 : public cjMem
{
public:
    cjBSplineBasisF3();
	cjBSplineBasisF3(cjINT32 quantity);

    cjINT32 GetControlPointCount() const { return m_controlPointCount; }

	// 0.0 <= stime >= 1.0
    void Compute(cjFLOAT stime, cjINT32& minVal, cjINT32& maxVal);

    cjFLOAT Get(cjINT32 i) const 
	{
		cjAssert(0 <= i && i <= 3);
		return m_value[i];
	}


    cjBool IsEqual(const cjBSplineBasisF3* rhs) const 
		{ return m_controlPointCount == rhs->m_controlPointCount; }

	cjBSplineBasisF3& operator =(const cjBSplineBasisF3& rhs)
	{
		if(this == &rhs)
			return *this;

		m_controlPointCount = rhs.m_controlPointCount;
		cjMemCpy(m_value, rhs.m_value, sizeof(cjFLOAT)*4);
		m_lastCalcTime = rhs.m_lastCalcTime;
		m_min = rhs.m_min;
		m_max = rhs.m_max;		

		return *this;
	}
	
private:
    cjINT32								m_controlPointCount;
    cjFLOAT								m_value[3+1];
    cjFLOAT								m_lastCalcTime;		// prevent recomputing
    cjINT32								m_min, m_max;
};


inline cjBSplineBasisF3::cjBSplineBasisF3(cjINT32 controlPointCount)
{
    cjAssert(1 <= 3 && 3 < controlPointCount);

    m_controlPointCount = controlPointCount;
    m_lastCalcTime = (cjFLOAT)-1.0;
    m_min = 0;
    m_max = 3;
}

inline cjBSplineBasisF3::cjBSplineBasisF3()
{
    m_controlPointCount = 0;
    m_lastCalcTime = (cjFLOAT)-1.0;
    m_min = 0;
    m_max = 3;
}

inline void cjBSplineBasisF3::Compute(cjFLOAT stime, cjINT32& minVal, cjINT32& maxVal)
{
    cjAssert(0.0f <= stime && stime <= 1.0f);

    cjINT32 iQm3 = m_controlPointCount - 3;
    cjINT32 i;

    if(stime == m_lastCalcTime)
    {
        minVal = m_min; 
		maxVal = m_max; 

		return;    
	}

    m_lastCalcTime = stime;
    
    if(stime < 1.0f)
        i = 3 + (cjINT32)(stime * (cjFLOAT)iQm3);
    else // stime == 1.0f
        i = m_controlPointCount - 1;

    m_min = minVal = i - 3;
    m_max = maxVal = i;

    cjFLOAT fQm3 = (cjFLOAT)iQm3;
    cjFLOAT fT = fQm3 * stime;

    const cjFLOAT fOneThird = 1.0f / 3.0f;

    if(m_controlPointCount >= 7)
    {
        cjINT32 iQm2 = iQm3 + 1, iQm1 = iQm2 + 1;

        cjFLOAT fG0 = (i > 5 ? (cjFLOAT)(i-5) : 0.0f);
        cjFLOAT fG1 = (i > 4 ? (cjFLOAT)(i-4) : 0.0f);
        cjFLOAT fG2 = (cjFLOAT)(i-3);
        cjFLOAT fG3 = (cjFLOAT)(i-2);
        cjFLOAT fG4 = (i < iQm2 ? (cjFLOAT)(i-1) : (cjFLOAT)iQm3);
        cjFLOAT fG5 = (i < iQm3 ? (cjFLOAT)i : (cjFLOAT)iQm3);

        cjFLOAT fInvG3mG1 = (i == 3 ? 1.0f : 0.5f);
        cjFLOAT fInvG4mG2 = (i == iQm1 ? 1.0f : 0.5f);
        cjFLOAT fInvG3mG0 = (i == 3 ? 1.0f : (i == 4 ? 0.5f : fOneThird));
        cjFLOAT fInvG4mG1 = (i == 3 || i == iQm1 ? 0.5f : fOneThird);
        cjFLOAT fInvG5mG2 = (i == iQm1 ? 1.0f : (i == iQm2 ? 0.5f : fOneThird));

        cjFLOAT fTmG0 = fT - fG0;
        cjFLOAT fTmG1 = fT - fG1;
        cjFLOAT fTmG2 = fT - fG2;
        cjFLOAT fG3mT = fG3 - fT;
        cjFLOAT fG4mT = fG4 - fT;
        cjFLOAT fG5mT = fG5 - fT;

        cjFLOAT fExpr0 = fG3mT * fInvG3mG1;
        cjFLOAT fExpr1 = fTmG2 * fInvG4mG2;
        cjFLOAT fExpr2 = fInvG3mG0 * fG3mT * fExpr0;
        cjFLOAT fExpr3 = fInvG4mG1 * (fTmG1 * fExpr0 + fG4mT * fExpr1);
        cjFLOAT fExpr4 = fInvG5mG2 * fTmG2 * fExpr1;

        m_value[0] = fG3mT * fExpr2;
        m_value[1] = fTmG0 * fExpr2 + fG4mT * fExpr3;
        m_value[2] = fTmG1 * fExpr3 + fG5mT * fExpr4;
        m_value[3] = fTmG2 * fExpr4;

		return;
    }

    if(m_controlPointCount == 6)
    {
        if(i == 3)
        {
            cjFLOAT f1mT = 1.0f - fT;
            cjFLOAT f2mT = 2.0f - fT;
            cjFLOAT f3mT = 3.0f - fT;
            cjFLOAT fHalfT = 0.5f * fT;
            cjFLOAT f1mTSqr = f1mT * f1mT;
            cjFLOAT fExpr0 = 0.5f * (fT * f1mT + f2mT * fHalfT);
            cjFLOAT fExpr1 = fOneThird * fT * fHalfT;

            m_value[0] = f1mT * f1mTSqr;
            m_value[1] = fT * f1mTSqr + f2mT * fExpr0;
            m_value[2] = fT * fExpr0 + f3mT * fExpr1;
            m_value[3] = fT * fExpr1;
        }
        else if(i == 4)
        {
            cjFLOAT fTm1 = fT - 1.0f;
            cjFLOAT f2mT = 2.0f - fT;
            cjFLOAT f3mT = 3.0f - fT;
            cjFLOAT fHalfT = 0.5f * fT;
            cjFLOAT f1mHalfT = 1.0f - fHalfT;
            cjFLOAT fHalfTm1 = 0.5f * fTm1;
            cjFLOAT f1mHalfTSqr = f1mHalfT * f1mHalfT;
            cjFLOAT fExpr = fOneThird * (fT * f1mHalfT + f3mT * fHalfTm1);
            cjFLOAT fHalfTm1Sqr = fHalfTm1 * fHalfTm1;

            m_value[0] = f2mT * f1mHalfTSqr;
            m_value[1] = fT * f1mHalfTSqr + f3mT * fExpr;
            m_value[2] = fT * fExpr + f3mT * fHalfTm1Sqr;
            m_value[3] = fTm1 * fHalfTm1Sqr;
        }
        else  // i == 5
        {
            cjFLOAT fTm1 = fT - 1.0f;
            cjFLOAT fTm2 = fT - 2.0f;
            cjFLOAT f3mT = 3.0f - fT;
            cjFLOAT fHalf3mT = 0.5f * f3mT;
            cjFLOAT fTm2Sqr = fTm2 * fTm2;
            cjFLOAT fExpr0 = fOneThird * f3mT * fHalf3mT;
            cjFLOAT fExpr1 = 0.5f * (fTm1 * fHalf3mT + f3mT * fTm2);

            m_value[0] = f3mT * fExpr0;
            m_value[1] = fT * fExpr0 + f3mT * fExpr1;
            m_value[2] = fTm1 * fExpr1 + f3mT * fTm2Sqr;
            m_value[3] = fTm2 * fTm2Sqr;
        }
        return;
    }

    if(m_controlPointCount == 5)
    {
        if(i == 3)
        {
            cjFLOAT f1mT = 1.0f - fT;
            cjFLOAT f2mT = 2.0f - fT;
            cjFLOAT fHalfT = 0.5f * fT;
            cjFLOAT f1mTSqr = f1mT * f1mT;
            cjFLOAT fExpr = 0.5f * (fT * f1mT + f2mT * fHalfT);
            cjFLOAT fHalfTSqr = fHalfT * fHalfT;

            m_value[0] = f1mT * f1mTSqr;
            m_value[1] = fT * f1mTSqr + f2mT * fExpr;
            m_value[2] = fT * fExpr + f2mT * fHalfTSqr;
            m_value[3] = fT * fHalfTSqr;
        }
        else  // i == 4
        {
            cjFLOAT fTm1 = fT - 1.0f;
            cjFLOAT f2mT = 2.0f - fT;
            cjFLOAT fHalfT = 0.5f * fT;
            cjFLOAT fTm1Sqr = fTm1 * fTm1;
            cjFLOAT f1mHalfT = 1.0f - fHalfT;
            cjFLOAT f1mHalfTSqr = f1mHalfT * f1mHalfT;
            cjFLOAT fExpr = f1mHalfT * (fHalfT + fTm1);

            m_value[0] = f2mT * f1mHalfTSqr;
            m_value[1] = fT * f1mHalfTSqr + f2mT * fExpr;
            m_value[2] = fT * fExpr + f2mT * fTm1Sqr;
            m_value[3] = fTm1 * fTm1Sqr;
        }
        return;
    }

    if(m_controlPointCount == 4)
    {
        // i == 3
        cjFLOAT f1mT = 1.0f - fT;
        cjFLOAT fTSqr = fT * fT;
        cjFLOAT f1mTSqr = f1mT * f1mT;

        m_value[0] = f1mT * f1mTSqr;
        m_value[1] = 3.0f * fT * f1mTSqr;
        m_value[2] = 3.0f * fTSqr * f1mT;
        m_value[3] = fT * fTSqr;
        return;
    }

    cjAssert(cjFalse);
}


//
// cjBSplineBasisF3Dat
//

class CJCORE_API cjBSplineBasisF3Dat : public cjRef
{
	CJSER_DECL_CLASS(cjBSplineBasisF3Dat, cjRef, 0, CJCORE_API)
	CJSER_DECL_CLONE(cjBSplineBasisF3Dat)

public:
    void SetBasis(const cjBSplineBasisF3* basisF3) { m_basisF3 = *basisF3; }
    cjBSplineBasisF3& GetBasis() { return m_basisF3; }

protected:
	// keep 4byte alignment
    cjBSplineBasisF3					m_basisF3;
};

cjRefPtrDefine(cjBSplineBasisF3Dat)


//
// cjBSplineDat
//

class CJCORE_API cjBSplineDat : public cjRef
{
	CJSER_DECL_CLASS(cjBSplineDat, cjRef, 0, CJCORE_API)
	CJSER_DECL_CLONE(cjBSplineDat)

public:
    cjBSplineDat();

    typedef cjUINT32 BSOffset; // offset
    enum { INVALID_BSOFFSET = 0xFFFF };

	// ret: start offset(zero based) of newly added cp
    BSOffset AddCP(const cjFLOAT* srcCPAry, cjUINT32 count, cjUINT32 dimension);
    BSOffset AddHalfFloatCP(const cjFLOAT* srcCPAry, cjUINT32 count, cjUINT32 dimension, 
		cjFLOAT& halfRangeOffset, cjFLOAT& halfRangeCenter); // cjSWORD size of cjFLOAT
    
    const cjFLOAT* GetCP(BSOffset bsStOffset, cjUINT32 index, cjUINT32 dimension) const
		{ return &((m_cps + bsStOffset)[index * dimension]); }
	cjUINT32 GetCPCount() const { return m_cpCount; }

    const cjSWORD* GetHalfFloatCP(BSOffset bsStOffset, cjUINT32 index, cjUINT32 dimension) const
		{ return &((m_halfFloat + bsStOffset)[index * dimension]); }
	cjUINT32 GetHalfFloatCPCount() const { return m_compactCPCount; }	

	// floatx3
    void GetValueF3(cjFLOAT etime, cjFLOAT* posAry, cjUINT32 dimension, cjBSplineBasisF3Dat* basisDat, 
		BSOffset cpStOffset) const;
    void GetHalfFloatValueF3(cjFLOAT etime, cjFLOAT* posAry, cjUINT32 dimension, 
		cjBSplineBasisF3Dat* basisDat, BSOffset cpStOffset, cjFLOAT halfRangeOffset, 
		cjFLOAT halfRangeCenter) const;
    
protected:
    cjFLOAT*							m_cps;			// control points
    cjSWORD*							m_halfFloat;
    
    cjUINT32							m_cpCount;
    cjUINT32							m_compactCPCount;
};

cjRefPtrDefine(cjBSplineDat)


#endif
