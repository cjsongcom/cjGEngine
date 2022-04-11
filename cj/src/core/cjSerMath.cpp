
#include "cjCorePCH.h"
#include "cjSerMath.h"


//
// cjBSplineBasisF3Dat
//

CJSER_IMPL_CLASS(cjBSplineBasisF3Dat, cjRef)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjBSplineBasisF3Dat)
	d->m_basisF3 = m_basisF3;
CJSER_IMPL_CLONE_PROPCOPY_END(cjBSplineBasisF3Dat)

CJSER_IMPL_CLONE(cjBSplineBasisF3Dat)


cjBSplineBasisF3Dat::~cjBSplineBasisF3Dat()
{
}

//	
// cjBSplineDat
//

CJSER_IMPL_CLASS(cjBSplineDat, cjRef)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjBSplineDat)

	d->m_cpCount = m_cpCount;
	d->m_cps = (cjFLOAT*)cjMemAlloc(sizeof(cjFLOAT) * m_cpCount);
	cjMemCpy(d->m_cps, m_cps, sizeof(cjFLOAT) * m_cpCount);

	d->m_compactCPCount = m_compactCPCount;	
	d->m_halfFloat = (cjSWORD*)cjMemAlloc(sizeof(cjSWORD) * m_compactCPCount);
	cjMemCpy(d->m_halfFloat, m_halfFloat, sizeof(cjSWORD) * m_compactCPCount);
	
CJSER_IMPL_CLONE_PROPCOPY_END(cjBSplineDat)

CJSER_IMPL_CLONE(cjBSplineDat)


cjBSplineDat::cjBSplineDat()
{
	m_cps = cjNULL;
    m_halfFloat = cjNULL;
    m_cpCount = 0;
	m_compactCPCount = 0;
}

cjBSplineDat::~cjBSplineDat ()
{
    cjMemDeAlloc(m_cps);
    cjMemDeAlloc(m_halfFloat);
}

cjBSplineDat::BSOffset cjBSplineDat::AddCP(const cjFLOAT* srcCPAry, cjUINT32 cpCount, cjUINT32 dimension)
{
    cjUINT32 newCPCount = m_cpCount + cpCount * dimension;
    cjFLOAT* newCPAry = (cjFLOAT*)cjMemAlloc(sizeof(cjFLOAT) * newCPCount);

    cjUINT32 newSiz = newCPCount * sizeof(cjFLOAT);
    cjUINT32 oldSiz = m_cpCount * sizeof(cjFLOAT);
	
	// copy old data
    cjMemCpy(newCPAry, newSiz, m_cps, oldSiz);
    
    cjFLOAT* newCPInsert = &newCPAry[m_cpCount];

    cjUINT32 remainSiz = newSiz - oldSiz;
    cjAssert(remainSiz == cpCount * dimension * sizeof(cjFLOAT));

	// append new cp
    cjMemCpy(newCPInsert, remainSiz, srcCPAry, remainSiz);
    m_cpCount = newSiz;
    
	cjMemDeAlloc(m_cps);
    m_cps = newCPAry;

    // [ret] start offset of newly inserted cp
    return (cjBSplineDat::BSOffset)(newCPInsert - newCPAry); 
}

cjBSplineDat::BSOffset cjBSplineDat::AddHalfFloatCP(const cjFLOAT* srcCPAry, cjUINT32 cpCount,
	cjUINT32 dimension, cjFLOAT& halfRangeOffset, cjFLOAT& halfRangeCenter)
{
    cjUINT32 newCount = m_compactCPCount + cpCount * dimension;
    cjSWORD* newCPAry = (cjSWORD*)cjMemAlloc(sizeof(cjSWORD) * newCount);

    cjHalfFloat halfFloat;
	halfFloat.Build(srcCPAry, cpCount*dimension);

    cjSWORD* tmpCompactCPAry = halfFloat.m_halfAry;
    halfRangeCenter = halfFloat.m_rangeCenter;
    halfRangeOffset = halfFloat.m_offset;

    cjUINT32 newSiz = newCount * sizeof(cjSWORD);
    cjUINT32 oldSiz = m_compactCPCount * sizeof(cjSWORD);

    // copy old cp
    cjMemCpy(newCPAry, newSiz, m_halfFloat, oldSiz);
    
    cjSWORD* newCPInsert = &newCPAry[m_compactCPCount];
    cjUINT32 remainSiz = newSiz - oldSiz;
    cjAssert(remainSiz == cpCount * dimension * sizeof(cjSWORD));

	// append new cp
    cjMemCpy(newCPInsert, remainSiz, tmpCompactCPAry, remainSiz);
    m_compactCPCount = newCount;
    
    cjMemDeAlloc(m_halfFloat);
    m_halfFloat = newCPAry;

    return (cjBSplineDat::BSOffset)(newCPInsert - newCPAry); 
}

void cjBSplineDat::GetValueF3(cjFLOAT etime, cjFLOAT* posAry, cjUINT32 dimension, 
	cjBSplineBasisF3Dat* basisDat, BSOffset cpStOffset) const
{
    cjAssert(cpStOffset != INVALID_BSOFFSET);
    cjINT32 bsMin, bsMax;

    cjBSplineBasisF3& bsBasisF3 = basisDat->GetBasis();
    bsBasisF3.Compute(etime, bsMin, bsMax);

    const cjFLOAT* src = GetCP(cpStOffset, bsMin, dimension);
    cjFLOAT basisVal = bsBasisF3.Get(0);

	for(cjUINT32 k=0; k<dimension; k++)
        posAry[k] = basisVal * (*src++);

    for(cjINT32 i=bsMin+1, iIndex=1; i<=bsMax; i++, iIndex++)
    {
        basisVal = bsBasisF3.Get(iIndex);

        for(cjUINT32 k=0; k<dimension; k++)
            posAry[k] += basisVal * (*src++);
    }
}

void cjBSplineDat::GetHalfFloatValueF3(cjFLOAT etime, cjFLOAT* posAry, cjUINT32 dimension, 
	cjBSplineBasisF3Dat* basisDat, BSOffset cpStOffset, cjFLOAT halfRangeOffset, 
	cjFLOAT halfRangeCenter) const
{
    cjAssert(cpStOffset != INVALID_BSOFFSET);
    cjINT32 bsMin, bsMax;

    cjBSplineBasisF3& bsBasisF3 = basisDat->GetBasis();
    bsBasisF3.Compute(etime, bsMin, bsMax);

    const cjSWORD* srcHalfFloatCP = GetHalfFloatCP(cpStOffset, bsMin, dimension);

    cjUINT32 itemCount = dimension*4;
    cjFLOAT srcFloatCP[16];

	// half cjFLOAT -> cjFLOAT
	cjHalfFloat::ConvertToFloat(srcFloatCP, itemCount, srcHalfFloatCP, itemCount, 
		halfRangeOffset, halfRangeCenter);

	cjFLOAT basisVal = bsBasisF3.Get(0);
    cjUINT32 srcIdx = 0;
    
	for(cjUINT32 k = 0; k < dimension; k++)
        posAry[k] = basisVal * srcFloatCP[srcIdx++];

    for(cjINT32 i=bsMin+1, iIndex=1; i<=bsMax; i++, iIndex++)
    {
        basisVal = bsBasisF3.Get(iIndex);

        for(cjUINT32 k=0; k<dimension; k++)
			posAry[k] += basisVal * srcFloatCP[srcIdx++];
    }
}

