
#include "cjCorePCH.h"
#include "cjNumeric.h"


//
// Returns smallest N such that (1<<N)>=Arg.
// Note: appCeilLogTwo(0)=0 because (1<<0)=1 >= 0.
//

static cjBool gs_initNearestPOTLogTable = cjFalse;
static cjBYTE gs_nearestPOTLogTable[257];

static void InitNearestPOTLogTable()
{
	for(cjINT32 i=0,e=-1,c=0; i<=256; i++)
	{
        gs_nearestPOTLogTable[i] = (cjBYTE)(e+1);
        
		if(!i || ++c >= (1<<e))
        {
			c=0;
            e++;
        }
	}
}

cjBYTE cjGetNearestPOTBit(cjTFLAG val)
{
    if(!gs_initNearestPOTLogTable)
    {
        InitNearestPOTLogTable();
        gs_initNearestPOTLogTable = cjTrue;
    }

	if(--val == cjMAXUINT32)
		return 0;
    
    cjBYTE shift;
    
    if(val <= 0x10000)
        shift = val <= 0x100 ? 0:8;
    else
        shift = val <= 0x1000000 ? 16:24;
    
	return shift + gs_nearestPOTLogTable[val >> shift];
}


//
// cjHalfFloat
//

cjHalfFloat::cjHalfFloat()	
{ 
	cjMemSet(this, 0, sizeof(cjHalfFloat)); 
}

cjHalfFloat::cjHalfFloat(cjHalfFloat& rhs) 
{ 
	this->operator=(rhs); 
}

cjHalfFloat::~cjHalfFloat()	
{ 
	if(m_halfAry) 
		cjMemDeAlloc(m_halfAry); 
}

cjHalfFloat& cjHalfFloat::operator =(const cjHalfFloat& rhs)
{
	if(this == &rhs)
		return *this;

	cjUINT32 rhsValueCount = rhs.m_count;

	if(rhsValueCount == 0)
	{
		cjMemDeAlloc(m_halfAry);
		m_halfAry = 0;
	}
	else if(m_count >= rhsValueCount)
	{
		cjMemCpy(m_halfAry, rhs.m_halfAry, sizeof(cjSWORD)*rhsValueCount);
	}
	else
	{
		cjMemDeAlloc(m_halfAry);
		m_halfAry = (cjSWORD*)cjMemAlloc(sizeof(cjSWORD)*rhsValueCount);
		cjMemCpy(m_halfAry, rhs.m_halfAry, sizeof(cjSWORD)*rhsValueCount);
	}

	m_count = rhsValueCount;
	m_offset = rhs.m_offset;
	m_rangeCenter = rhs.m_rangeCenter;

	return *this;
}

void cjHalfFloat::Build(const cjFLOAT* src, cjUINT32 count)
{
	if(m_count < count)
	{
		cjMemDeAlloc(m_halfAry);
		m_halfAry = cjNULL;

		if(count > 0)
			m_halfAry = (cjSWORD*)cjMemAlloc(sizeof(cjSWORD) * count);
	}

	m_count = count;
	m_offset = 0;
	m_rangeCenter = 0;

	if(m_count == 0)
		return;
		
    cjFLOAT minV, maxV;
    GetMinMaxFloat(src, count, minV, maxV);

    m_offset		= (minV + maxV) * 0.5f;
    m_rangeCenter	= (maxV - minV) * 0.5f;

    cjAssert(maxV > minV);

    cjFLOAT fltShortMax = (cjFLOAT)CJ_SHORT_MAX;

	// convert cjFLOAT to short
    for(cjUINT32 i=0; i<count; i++)
    {
		// 0.0~1.0 * fltShortMax
        cjFLOAT flt = ( ((src[i]-m_offset) / (m_rangeCenter)) * (fltShortMax) );

		// clip value range 32767 ~ -32768
        if(flt < -fltShortMax)
            flt = -fltShortMax;

        if(flt > fltShortMax)
            flt = fltShortMax;

        m_halfAry[i] = (cjSWORD)flt;
    }
}

void cjHalfFloat::GetMinMaxFloat(const cjFLOAT* srcAry, cjUINT32 count, cjFLOAT& minV, cjFLOAT& maxV)
{    
	minV = CJ_FLT_MAX;
    maxV = CJ_FLT_INVALID;

    for(cjUINT32 i=0; i<count; i++)
    {
        if(srcAry[i] < minV)
            minV = srcAry[i];

        if(srcAry[i] > maxV)
            maxV = srcAry[i];
    }

    cjAssert(minV < maxV);
}

cjUINT32 cjHalfFloat::SaveAsFloatAry(cjFLOAT* dstFloatAry, cjUINT32 dstCount)
{
	cjUINT32 saveCount;
	
	if(dstCount == ((cjUINT32)-1))
		saveCount = m_count;
	else
		saveCount = m_count < dstCount ? m_count : dstCount;

    for(cjUINT32 i=0; i<saveCount; i++)
    {
		// 1. convert short value to cjFLOAT (cjFLOAT)(short value) and div with CJ_SHORT_MAX
		// 2. multiply halfRange
		// 3. add offset
		dstFloatAry[i] = ( ( ( (cjFLOAT)m_halfAry[i]) / ((cjFLOAT)CJ_SHORT_MAX) ) * m_rangeCenter) + m_offset;
    }

	return saveCount;
}

cjUINT32 cjHalfFloat::ConvertToFloat(cjFLOAT* dstFloatAry, cjUINT32 dstFloatAryCount, 
	const cjSWORD* srcHalfFloatAry, cjUINT32 srcHalfFloatCount, cjFLOAT halfRangeOffset, cjFLOAT halfRangeCenter)
{
	cjAssert(srcHalfFloatCount >= dstFloatAryCount);

    for(cjUINT32 i=0; i<dstFloatAryCount; i++)
	{
		dstFloatAry[i] = ((((cjFLOAT)srcHalfFloatAry[i]) 
			/ ((cjFLOAT)CJ_SHORT_MAX)) * halfRangeCenter) + halfRangeOffset;
	}

	return dstFloatAryCount;
}


//
// PrimeNumberTable
//

// Prime numbers list, 1024
static const cjUINT32 gs_primeTbl[] = 
{
	2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 
	43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 
	101, 103, 107, 109, 113, 
    127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 
    179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 
    233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 
    283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 
    353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 
    419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 
    467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 
    547, 557, 563, 569, 571, 577, 587, 593, 599, 601, 
    607, 613, 617, 619, 631, 641, 643, 647, 653, 659, 
    661, 673, 677, 683, 691, 701, 709, 719, 727, 733, 
    739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 
    811, 821, 823, 827, 829, 839, 853, 857, 859, 863, 
    877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 
    947, 953, 967, 971, 977, 983, 991, 997, 1009, 1013, 
    1019, 1021, 1031
};

const cjUINT32 gs_primeTblCount = sizeof(gs_primeTbl) / sizeof(cjUINT32);


cjUINT32 cjPrimeNumberTable::GetPrimeCount()
{
    return gs_primeTblCount;
}

cjUINT32 cjPrimeNumberTable::GetPrime(cjUINT32 index)
{
    cjAssert(index < gs_primeTblCount);
    return gs_primeTbl[index];
}

cjUINT32 cjPrimeNumberTable::GetMaxPrime()
{
    cjAssert(gs_primeTblCount >= 1);
    return gs_primeTbl[gs_primeTblCount - 1];
}

cjBool cjPrimeNumberTable::GetNearestPrime(cjUINT32& rstPrime, const cjUINT32 number)
{
    cjAssert(gs_primeTblCount >= 2);

    if(number > gs_primeTbl[gs_primeTblCount - 2])
    {
        rstPrime = gs_primeTbl[gs_primeTblCount - 1];
        return (number <= rstPrime) ? cjTrue : cjFalse;
    }

    if(number < gs_primeTbl[0])
    {
        rstPrime = gs_primeTbl[0];
        return cjTrue;
    }

    cjINT32 bot = 0;
    cjINT32 top = (cjINT32)gs_primeTblCount - 1;
    cjINT32 mid = 0;

    while (bot <= top)
    {
        mid = (top + bot) >> 1;

		if(number == gs_primeTbl[mid])
        {
            rstPrime = gs_primeTbl[mid];
            return cjTrue;
        }
        else if(number > gs_primeTbl[mid])
        {
            bot = mid + 1;
        }
        else
        {
            top = mid - 1;
        }
    }

    rstPrime = gs_primeTbl[bot];

    return cjTrue;
}


//
// Interpolation
//

cjFLOAT cjScalarInterpolate::DoLinear(cjFLOAT rtime, cjFLOAT p, cjFLOAT q)
{
    return p*(1-rtime) + q*rtime;
}

cjFLOAT cjScalarInterpolate::DoBezier(cjFLOAT rtime, cjFLOAT p, cjFLOAT dp, cjFLOAT q, cjFLOAT dq)
{
    cjFLOAT a = 3*(q-p)-(dq+2*dp);
    cjFLOAT b = dp+dq-2*(q-p);

    return p+(dp+(a+b*rtime)*rtime)*rtime;
}

cjFLOAT cjScalarInterpolate::DoTCB(cjFLOAT rtime, cjFLOAT p, cjFLOAT pdd, cjFLOAT q, cjFLOAT qds)
{
    cjFLOAT a = 3*(q-p)-(qds+2*pdd);
    cjFLOAT b = pdd+qds-2*(q-p);

    return p+(pdd+(a+b*rtime)*rtime)*rtime;
}

void cjScalarInterpolate::DoAdjustBezier(cjFLOAT lastVal, cjFLOAT lastTime, cjFLOAT& lastOut, 
	cjFLOAT nextVal, cjFLOAT nextTime, cjFLOAT& nextIn, cjFLOAT newTime, cjFLOAT, 
	cjFLOAT& newIn, cjFLOAT& newOut)
{
    cjFLOAT oldDeltaX	= nextVal - lastVal;
    cjFLOAT oldDeltaT	= nextTime - lastTime;
    cjFLOAT newDeltaTA	= newTime - lastTime;
    cjFLOAT newDeltaTB	= nextTime - newTime;

    cjFLOAT t = newDeltaTA / oldDeltaT;
    
    cjFLOAT a = -2.0f * oldDeltaX + lastOut + nextIn;
    cjFLOAT b = 3.0f * oldDeltaX - 2.0f * lastOut - nextIn;

    newIn = (((3.0f * a * t + 2.0f * b) * t + lastOut) / oldDeltaT);
    newOut = newIn * newDeltaTB;
    newIn *= newDeltaTA;

    lastOut *= newDeltaTA / oldDeltaT;
    nextIn *= newDeltaTB / oldDeltaT;
}

