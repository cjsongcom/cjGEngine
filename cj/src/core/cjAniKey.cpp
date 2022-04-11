
#include "cjCorePCH.h"
#include "cjAniKey.h"


//
// cjAniKey - Operators
//
																   
cjAniKey::PFIsPosed			cjAniKey::ms_isposedPF		[cjAniKey::KEYOPER_SIZ];
cjAniKey::PFEqual			cjAniKey::ms_equalPF		[cjAniKey::KEYOPER_SIZ];
																   
cjAniKey::PFSave			cjAniKey::ms_savePF			[cjAniKey::KEYOPER_SIZ];
cjAniKey::PFLoad			cjAniKey::ms_loadPF			[cjAniKey::KEYOPER_SIZ];
cjAniKey::PFCreate			cjAniKey::ms_createPF		[cjAniKey::KEYOPER_SIZ];
cjAniKey::PFDelete			cjAniKey::ms_deletePF		[cjAniKey::KEYOPER_SIZ];
cjAniKey::PFCopy			cjAniKey::ms_copyPF			[cjAniKey::KEYOPER_SIZ];
cjAniKey::PFInsert			cjAniKey::ms_insertPF		[cjAniKey::KEYOPER_SIZ];

cjAniKey::PFCurvedRatio		cjAniKey::ms_curvePF		[cjAniKey::KEYOPER_SIZ];
cjAniKey::PFFillDerivedVals	cjAniKey::ms_fillderivedPF	[cjAniKey::KEYOPER_SIZ];
cjAniKey::PFCubicCoefs		cjAniKey::ms_cubiccoefsPF	[cjAniKey::KEYOPER_SIZ];
cjAniKey::PFInterpolation	cjAniKey::ms_interpPF		[cjAniKey::KEYOPER_SIZ];
cjAniKey::PFInterpolation	cjAniKey::ms_interpd1PF		[cjAniKey::KEYOPER_SIZ];
cjAniKey::PFInterpolation	cjAniKey::ms_interpd2PF		[cjAniKey::KEYOPER_SIZ];

	// keysize
cjBYTE						cjAniKey::ms_keysiz			[cjAniKey::KEYOPER_SIZ];


//
// cjAniKey - Get PF
//

	// isPosed, equal
cjAniKey::PFIsPosed cjAniKey::GetIsPosedKeyPF(EKEY_VALTYPE valType, EKEY_TYPE type)
{
    return ms_isposedPF[valType * EKT_MAX + type];
}

cjAniKey::PFEqual cjAniKey::GetEqualKeyPF(EKEY_VALTYPE valType, EKEY_TYPE type)
{
    return ms_equalPF[valType * EKT_MAX + type];
}

	// save/load/create/delete/copy/insert
cjAniKey::PFSave cjAniKey::GetSaveKeyPF(EKEY_VALTYPE valType, EKEY_TYPE type)
{
    return ms_savePF[valType * EKT_MAX + type];
}

cjAniKey::PFLoad cjAniKey::GetLoadKeyPF(EKEY_VALTYPE valType, EKEY_TYPE type)
{
    return ms_loadPF[valType * EKT_MAX + type];
}

cjAniKey::PFCreate cjAniKey::GetCreateKeyPF(EKEY_VALTYPE valType, EKEY_TYPE type)
{
    return ms_createPF[valType * EKT_MAX + type];
}

cjAniKey::PFDelete cjAniKey::GetDeleteKeyPF(EKEY_VALTYPE valType, EKEY_TYPE type)
{
    return ms_deletePF[valType * EKT_MAX + type];
}

cjAniKey::PFCopy cjAniKey::GetCopyKeyPF(EKEY_VALTYPE valType, EKEY_TYPE type)
{
    return ms_copyPF[valType * EKT_MAX + type];
}

cjAniKey::PFInsert cjAniKey::GetInsertKeyPF(EKEY_VALTYPE valType, EKEY_TYPE type)
{
    return ms_insertPF[valType * EKT_MAX + type];
}

	// interpolation
cjAniKey::PFCurvedRatio cjAniKey::GetCurvedRatioKeyPF(EKEY_VALTYPE valType, EKEY_TYPE type)
{
    return ms_curvePF[valType * EKT_MAX + type];
}

cjAniKey::PFFillDerivedVals cjAniKey::GetFillDerivedValsKeyPF(EKEY_VALTYPE valType, EKEY_TYPE type)
{
    return ms_fillderivedPF[valType * EKT_MAX + type];
}

cjAniKey::PFCubicCoefs cjAniKey::GetCubicCoefsKeyPF(EKEY_VALTYPE valType, EKEY_TYPE type)
{
    return ms_cubiccoefsPF[valType * EKT_MAX + type];
}

cjAniKey::PFInterpolation cjAniKey::GetInterpKeyPF(EKEY_VALTYPE valType, EKEY_TYPE type)
{
    return ms_interpPF[valType * EKT_MAX + type];
}

cjAniKey::PFInterpolation cjAniKey::GetInterpD1KeyPF(EKEY_VALTYPE valType, EKEY_TYPE type)
{
    return ms_interpd1PF[valType * EKT_MAX + type];
}

cjAniKey::PFInterpolation cjAniKey::GetInterpD2KeyPF(EKEY_VALTYPE valType, EKEY_TYPE type)
{
    return ms_interpd2PF[valType * EKT_MAX + type];
}


//
// cjAniKey - Set PF
//

	// posed/equal
void cjAniKey::SetIsPosedKeyPF(EKEY_VALTYPE valType, EKEY_TYPE type, PFIsPosed pfIsPosed)
{
    ms_isposedPF[valType * EKT_MAX + type] = pfIsPosed;
}

void cjAniKey::SetEqualKeyPF(EKEY_VALTYPE valType, EKEY_TYPE type, PFEqual pfEqual)
{
    ms_equalPF[valType * EKT_MAX + type] = pfEqual;
}

	// save/load/create/delete/copy/insert
void cjAniKey::SetSaveKeyPF(EKEY_VALTYPE valType, EKEY_TYPE type, PFSave pfSave)
{
    ms_savePF[valType * EKT_MAX + type] = pfSave;
}

void cjAniKey::SetLoadKeyPF(EKEY_VALTYPE valType, EKEY_TYPE type, PFLoad pfLoad)
{
    ms_loadPF[valType * EKT_MAX + type] = pfLoad;
}

void cjAniKey::SetCreateKeyPF(EKEY_VALTYPE valType, EKEY_TYPE type, PFCreate pfCreate)
{
    ms_createPF[valType * EKT_MAX + type] = pfCreate;
}

void cjAniKey::SetDeleteKeyPF(EKEY_VALTYPE valType, EKEY_TYPE type, PFDelete pfDelete)
{
    ms_deletePF[valType * EKT_MAX + type] = pfDelete;
}

void cjAniKey::SetCopyKeyPF(EKEY_VALTYPE valType, EKEY_TYPE type, PFCopy pfCopy)
{
    ms_copyPF[valType * EKT_MAX + type] = pfCopy;
}

void cjAniKey::SetInsertKeyPF(EKEY_VALTYPE valType, EKEY_TYPE type, PFInsert pfInsert)
{
    ms_insertPF[valType * EKT_MAX + type] = pfInsert;
}

	// interpolation
void cjAniKey::SetCurvedRatioKeyPF(EKEY_VALTYPE valType, EKEY_TYPE type, PFCurvedRatio pfCurvedRatio)
{
    ms_curvePF[valType * EKT_MAX + type] = pfCurvedRatio;
}

void cjAniKey::SetFillDerivedValsKeyPF(EKEY_VALTYPE valType, EKEY_TYPE type, PFFillDerivedVals pfFillDerivedVals)
{
    ms_fillderivedPF[valType * EKT_MAX + type] = pfFillDerivedVals;
}

void cjAniKey::SetCubicCoefsKeyPF(EKEY_VALTYPE valType, EKEY_TYPE type, PFCubicCoefs pfCubicCoefs)
{
    ms_cubiccoefsPF[valType * EKT_MAX + type] = pfCubicCoefs;
}

void cjAniKey::SetInterpKeyPF(EKEY_VALTYPE valType, EKEY_TYPE type, PFInterpolation pfInterp)
{
    ms_interpPF[valType * EKT_MAX + type] = pfInterp;
}

void cjAniKey::SetInterpD1KeyPF(EKEY_VALTYPE valType, EKEY_TYPE type, PFInterpolation pfInterp)
{
    ms_interpd1PF[valType * EKT_MAX + type] = pfInterp;
}

void cjAniKey::SetInterpD2KeyPF(EKEY_VALTYPE valType, EKEY_TYPE type, PFInterpolation pfInterp)
{
    ms_interpd2PF[valType * EKT_MAX + type] = pfInterp;
}


//
// cjAniKey - default operator
//

//// [ret] cjFalse: already exist
//cjBool cjAniKey::FindInsKeyPos(cjFLOAT etime, const cjAniKey* keys, cjUINT32 keyCount, 
//	cjBYTE keySiz, cjUINT32* rstInsPos)
//{
//	cjBool alreadyExist = cjFalse;
//	cjUINT32 insPos = 0;
//
//    for(; insPos<keyCount; insPos++)
//    {
//        const cjAniKey* curKey = keys->GetKey(insPos, keySiz);
//
//        if(curKey->GetTime() < etime) // go next if under
//            continue;
//
//        if(curKey->GetTime() > etime) // exit if over
//            break;
//
//		cjAssert(curKey->GetTime() == etime);
//
//		alreadyExist = cjTrue;
//
//        break;
//    }
//
//	if(rstInsPos)
//		*rstInsPos = insPos;
//
//    return !alreadyExist;
//}

cjBool cjAniKey::TestKeyInsertable(cjFLOAT etime, const cjAniKey* keys, cjUINT32 keyCount, 
	cjBYTE keySiz, cjUINT32& insertIndex)
{
    cjBool alreadyExist = cjFalse;

    for(insertIndex=0; insertIndex<keyCount; insertIndex++)
    {
        const cjAniKey* key = keys->GetKey(insertIndex, keySiz);

        if(key->m_time < etime)
            continue;

        if(key->m_time > etime)
            break;

        cjAssert(key->m_time == etime);
        
		alreadyExist = cjTrue;
        break;
    }

    return !alreadyExist;
}

void cjAniKey::CloneKeysByRange(EKEY_VALTYPE valType, EKEY_TYPE keyType, const cjAniKey* srcKeys, 
	cjUINT32 srcKeyCount, cjFLOAT srcStTime, cjFLOAT srcEnTime, cjAniKey*& rstDstKeys, cjUINT32& rstDstKeyCount)
{
    if(keyType == EKT_EULERKEY)
    {
        cjAniKeyEulerRot* eulerKey = (cjAniKeyEulerRot*)srcKeys->GetKey(0, sizeof(cjAniKeyEulerRot));

        cjAniKeyFloat*			newFloatKeys[3];	// XYZ
        cjUINT32				newFloatKeyCount[3];
        cjAniKey::EKEY_TYPE		newFloatType[3];

        for(cjUINT32 i=0; i<3; i++)
        {
            cjAniKeyFloat* floatKeys = eulerKey->GetKeys((cjBYTE)i);
            cjUINT32 floatKeyCount	 = eulerKey->GetKeyCount((cjBYTE)i);
            newFloatType[i]			 = eulerKey->GetType((cjBYTE)i);

			cjAniKey* curNewKey = (cjAniKey*)newFloatKeys[i];

			// recursive call
            CloneKeysByRange(EKVAL_FLOAT, newFloatType[i], floatKeys, floatKeyCount, 
				srcStTime, srcEnTime, curNewKey, newFloatKeyCount[i]);

            newFloatKeys[i] = (cjAniKeyFloat*)curNewKey;
        }

        // Create the new Euler key with the copied keys
        rstDstKeys = new cjAniKeyEulerRot[1];
        rstDstKeyCount = 1;

        cjAniKeyEulerRot* newEulerKey = (cjAniKeyEulerRot*)rstDstKeys->GetKey(0, sizeof(cjAniKeyEulerRot));

		newEulerKey->Initialize(
			newFloatKeys[0], newFloatKeyCount[0], newFloatType[0],
            newFloatKeys[1], newFloatKeyCount[1], newFloatType[1],
            newFloatKeys[2], newFloatKeyCount[2], newFloatType[2]);
    }
    else
    {
        rstDstKeyCount = 0;
        cjBYTE siz = cjAniKey::GetKeySiz(valType, keyType);

        for(cjUINT32 srcKeyIndex=0; srcKeyIndex<srcKeyCount; srcKeyIndex++)
        {
            const cjAniKey* srcKey = srcKeys->GetKey(srcKeyIndex, siz);

            if(srcKey->GetTime() < srcStTime)
                continue;

            if(srcKey->GetTime() > srcEnTime)
                break;
            
            rstDstKeyCount++;
        }   

        if(rstDstKeyCount)
        {
            PFCreate pfnCreateKey = GetCreateKeyPF(valType, keyType);
            cjAssert(pfnCreateKey);
            rstDstKeys = pfnCreateKey(rstDstKeyCount);
    
			// copy keys in range
            PFCopy pfnCopyKey = GetCopyKeyPF(valType, keyType);
            cjAssert(pfnCopyKey);

            for(cjUINT32 srcKeyIndex=0, rstDstKeyCount=0; srcKeyIndex<srcKeyCount; srcKeyIndex++)
            {
                const cjAniKey* srcKey = srcKeys->GetKey(srcKeyIndex, siz);

                if(srcKey->GetTime() < srcStTime)
                    continue;

				if(srcKey->GetTime() > srcEnTime)
                    break;
            
                cjAniKey* dstKeys = rstDstKeys->GetKey(rstDstKeyCount, siz);
                pfnCopyKey(dstKeys, srcKey);
                dstKeys->SetTime(srcKey->GetTime() - srcStTime);

                rstDstKeyCount++;
            }   
        }
        else
        {
            rstDstKeys = 0;
        }        
    }
}

void cjAniKey::RecursiveGetKeysByRange(EKEY_VALTYPE valType, EKEY_TYPE keyType, cjAniKey*& rstKeys, 
	cjUINT32& rstKeyCount, cjFLOAT stTime, cjFLOAT enTime)
{
    cjAssert(stTime != enTime); // check for time range is zero
    cjAssert(rstKeyCount != 0);

    if(keyType == EKT_EULERKEY)
    {
        cjAssert(rstKeyCount == 1); // euler key always have one key
        cjAniKeyEulerRot* eulerRotKeys = (cjAniKeyEulerRot*)rstKeys;

        for(cjUINT32 i=0; i<3; i++)
        {
            cjAniKey* eulerKeys		= (cjAniKey*)eulerRotKeys->GetKeys((cjBYTE)i);
            cjUINT32  eulerkeyCount	= eulerRotKeys->GetKeyCount((cjBYTE)i);
            EKEY_TYPE eulerKeyType	= eulerRotKeys->GetType((cjBYTE)i);

            if(eulerkeyCount)
                RecursiveGetKeysByRange(EKVAL_FLOAT, eulerKeyType, eulerKeys, eulerkeyCount, stTime, enTime);

            eulerRotKeys->SetKeys		((cjBYTE)i, (cjAniKeyFloat*)eulerKeys);
            eulerRotKeys->SetKeyCount	((cjBYTE)i, eulerkeyCount);
            eulerRotKeys->SetKeyType	((cjBYTE)i, eulerKeyType);
        }

        return;
    }

    PFInsert pfnInsKey = GetInsertKeyPF(valType, keyType);
    cjAssert(pfnInsKey);

    if(rstKeyCount == 1)
    {        
        if(rstKeys->GetTime() == stTime) // If only one key at initial time, then return
            return;
    }

	pfnInsKey(stTime, rstKeys, rstKeyCount);
    pfnInsKey(enTime, rstKeys, rstKeyCount);
}


//
// cjAniKeyFloat
//

CJ_ANIKEYOPER_IMPL(cjAniKeyFloat, EKVAL_FLOAT, EKT_NOINTERP);
cjFLOAT cjAniKeyFloat::ms_default = 0.0f;


void cjAniKeyFloat::RegisterCustomAniKeyOperation(EKEY_VALTYPE keyValType, EKEY_TYPE keyType)
{
	SetIsPosedKeyPF		(keyValType, keyType, cjNULL);
	SetEqualKeyPF		(keyValType, keyType, EqualKey);

    SetInsertKeyPF		(keyValType, keyType, cjNULL);

    SetCurvedRatioKeyPF	(keyValType, keyType, cjNULL);
    SetFillDerivedValsKeyPF(keyValType, keyType, FillDerivedValsKey);
    SetCubicCoefsKeyPF	(keyValType, keyType, cjNULL);
	SetInterpKeyPF		(keyValType, keyType, InterpolateKey);
    SetInterpD1KeyPF	(keyValType, keyType, cjNULL);
    SetInterpD2KeyPF	(keyValType, keyType, cjNULL);    
}

cjFLOAT cjAniKeyFloat::GenInterp(cjFLOAT etime, cjAniKeyFloat* keys, cjAniKeyFloat::EKEY_TYPE keyType,
	cjUINT32 keyCount, cjUINT32& lastIdx, cjBYTE siz)
{
    cjAssert(keyCount != 0);

    if(keyCount == 1)
        return keys->GetKey(0, siz)->GetValue();

    cjUINT32 enKeyIndex = keyCount - 1;
    cjUINT32 stkLastIdx = lastIdx;

    cjFLOAT lastTime = keys->GetKey(stkLastIdx, siz)->GetTime();
    if(etime < lastTime)
    {
        stkLastIdx = 0;
        lastTime = keys->GetKey(0, siz)->GetTime();
    }

    cjUINT32 curKeyIdx;
    cjFLOAT nextTime = 0.0f;
    for(curKeyIdx = stkLastIdx + 1; curKeyIdx <= enKeyIndex; curKeyIdx++)
    {
        nextTime = keys->GetKey(curKeyIdx, siz)->GetTime();

        if(etime <= nextTime)
            break;

        stkLastIdx++;
        lastTime = nextTime;
    }
    
    cjAssert(curKeyIdx < keyCount);

    cjFLOAT ratio = (etime - lastTime) / (nextTime - lastTime);
    cjAniKey::PFInterpolation interp = cjAniKeyFloat::GetInterpKeyPF(keyType);
    cjAssert(interp);

    cjFLOAT rst;
    interp(ratio, keys->GetKey(stkLastIdx, siz), keys->GetKey(curKeyIdx, siz), &rst);
    lastIdx = stkLastIdx;

    return rst;
}

void cjAniKeyFloat::GenCubicCoefs(cjFLOAT etime, cjAniKeyFloat* keys, EKEY_TYPE keyType, 
	cjUINT32 keyCount, cjUINT32& lastIdx, cjBYTE siz, cjFLOAT& valTime0, cjFLOAT& valTime1, 
    cjFLOAT& val0, cjFLOAT& outTan, cjFLOAT& valA, cjFLOAT& valB)
{
    cjAssert(keyCount != 0);

    if(keyCount == 1)
    {
        val0 = keys->GetKey(0, siz)->GetValue();
        outTan = 0.0f;
        valA = 0.0f;
        valB = 0.0f;
        valTime0 = 0.0f;
        valTime1 = CJ_FLT_MAX;
        lastIdx = 0;

        return;
    }

    cjUINT32 enKeyIndex = keyCount - 1;
    cjUINT32 stkLastIdx = lastIdx;

    cjFLOAT lastTime = keys->GetKey(stkLastIdx, siz)->GetTime();
    if(etime < lastTime)
    {
        stkLastIdx = 0;
        lastTime = keys->GetKey(0, siz)->GetTime();
    }

    cjUINT32 curKeyIdx;
    cjFLOAT nextTime = 0.0f;
    for(curKeyIdx = stkLastIdx + 1; curKeyIdx <= enKeyIndex; curKeyIdx++)
    {
        nextTime = keys->GetKey(curKeyIdx, siz)->GetTime();

        if((etime < nextTime) || (etime == nextTime && keyType != EKT_STEPKEY))
            break;

        stkLastIdx++;
        lastTime = nextTime;
    }
    
    if(keyType == EKT_STEPKEY && etime >= nextTime && curKeyIdx == keyCount)
    {
        val0		= keys->GetKey(enKeyIndex, siz)->GetValue();
        outTan		= 0.0f;
        valA		= 0.0f;
        valB		= 0.0f;
        valTime0	= nextTime;
        valTime1	= nextTime;
        lastIdx		= enKeyIndex;
    }
    else
    {
        cjAssert(curKeyIdx < keyCount);
        cjAniKey::PFCubicCoefs cubiccoefs = cjAniKeyFloat::GetCubicCoefsKeyPF(keyType);
        cjAssert(cubiccoefs);
        cubiccoefs(keys, keyCount, stkLastIdx, &val0, &outTan, &valA, &valB);
        valTime0 = lastTime;
        valTime1 = nextTime;
        lastIdx = stkLastIdx;
    }
}

cjBool cjAniKeyFloat::EqualKey(const cjAniKey& k0, const cjAniKey& k1)
{
    if(!cjAniKey::EqualKey(k0, k1))
        return cjFalse;

    const cjAniKeyFloat& v0 = (const cjAniKeyFloat&)k0;
    const cjAniKeyFloat& v1 = (const cjAniKeyFloat&)k1;

    return v0.m_value == v1.m_value;
}

cjFLOAT cjAniKeyFloat::GenInterpDefault(cjFLOAT etime, cjAniKeyFloat* keys, EKEY_TYPE keyType, 
	cjUINT32 keyCount, cjBYTE siz)
{
    if(keyCount)
    {
        if(etime < keys->GetKey(0, siz)->GetTime()) // first key
		{
            return keys->GetKey(0, siz)->GetValue();
		}
        else if(etime > keys->GetKey(keyCount-1, siz)->GetTime()) // last key
		{
            return keys->GetKey(keyCount-1, siz)->GetValue();
		}
        else
        {
            cjUINT32 lastIdx = 0;
            return cjAniKeyFloat::GenInterp(etime, keys, keyType, keyCount, lastIdx, siz);
        }
    }

	return ms_default;
}

//
// cjAniKeyFloat
//  - cjAniKeyLinFloat
//

CJ_ANIKEYOPER_IMPL(cjAniKeyLinFloat, EKVAL_FLOAT, EKT_LINKEY);


void cjAniKeyLinFloat::RegisterCustomAniKeyOperation(EKEY_VALTYPE valType, EKEY_TYPE keyType)
{
	// Save/Load/Create/Delete/Copy is setted at CJ_ANIKEYOPER_IMPL

	SetIsPosedKeyPF				(valType, keyType, IsPosedKey);
	SetEqualKeyPF				(valType, keyType, EqualKey);

	SetInsertKeyPF				(valType, keyType, InsertKey);

    SetCurvedRatioKeyPF			(valType, keyType, cjNULL);
	SetFillDerivedValsKeyPF		(valType, keyType, FillDerivedValsKey);
	SetCubicCoefsKeyPF			(valType, keyType, CubicCoefsKey);
    SetInterpKeyPF				(valType, keyType, InterpolateKey);
    SetInterpD1KeyPF			(valType, keyType, cjNULL);
    SetInterpD2KeyPF			(valType, keyType, cjNULL);    
}

void cjAniKeyLinFloat::SaveKey(cjFStream& stream, cjAniKey* keys, cjUINT32 keyCount)
{
    cjAniKeyLinFloat* linearFloatKeys = (cjAniKeyLinFloat*)keys;

    for(cjUINT32 i=0; i<keyCount; i++)
		stream << linearFloatKeys[i];
}

cjAniKey* cjAniKeyLinFloat::LoadKey(cjFStream& stream, cjUINT32 keyCount)
{
    cjAniKeyLinFloat* linearFloatKeys = new cjAniKeyLinFloat[keyCount];

	cjAssert(stream.IsLoading());

    for(cjUINT32 i=0; i<keyCount; i++)
        stream << linearFloatKeys[i];

	return linearFloatKeys;
}

cjAniKey* cjAniKeyLinFloat::CreateKey(cjUINT32 keyCount)
{
    return new cjAniKeyLinFloat[keyCount];
}

void cjAniKeyLinFloat::DeleteKey(cjAniKey* keys)
{
    cjAniKeyLinFloat* linearFloatKeys = (cjAniKeyLinFloat*)keys;
    delete [] linearFloatKeys;
}

void cjAniKeyLinFloat::CopyKey(cjAniKey* dstKey, const cjAniKey* srcKey)
{
    cjAniKeyLinFloat* dstFloat = (cjAniKeyLinFloat*)dstKey;
    cjAniKeyLinFloat* srcFloat = (cjAniKeyLinFloat*)srcKey;

    dstFloat->m_time = srcFloat->m_time;
    dstFloat->m_value = srcFloat->m_value;
}

cjBool cjAniKeyLinFloat::IsPosedKey(const cjAniKey* keys, cjUINT32 keyCount)
{
    if(keyCount > 1)
    {
        const cjAniKeyLinFloat* linearFloatKeys = (const cjAniKeyLinFloat*)keys;
        cjFLOAT v = linearFloatKeys[0].m_value;

        for(cjUINT32 i=1; i<keyCount; i++)
        {
            if(linearFloatKeys[i].m_value != v)
                return cjFalse;
        }
    }

    return cjTrue;
}

cjBool cjAniKeyLinFloat::InsertKey(cjFLOAT etime, cjAniKey*& keys, cjUINT32& keyCount)
{
    cjAniKeyLinFloat* linearFloatKeys = (cjAniKeyLinFloat*)keys;

    cjUINT32 dstSiz;
    cjUINT32 insIndex;

    if(TestKeyInsertable(etime, linearFloatKeys, keyCount, sizeof(cjAniKeyLinFloat), insIndex))
    {
        cjAniKeyLinFloat* newKeys = new cjAniKeyLinFloat[keyCount + 1];
        dstSiz = (keyCount + 1) * sizeof(cjAniKeyLinFloat);

        cjMemCpy(newKeys, dstSiz, linearFloatKeys, insIndex * sizeof(cjAniKeyLinFloat));
        if(keyCount > insIndex)
        {
            dstSiz = (keyCount - insIndex) * sizeof(cjAniKeyLinFloat);
            cjMemCpy(&newKeys[insIndex + 1], &linearFloatKeys[insIndex], dstSiz);
        }

        cjFLOAT v = cjAniKeyFloat::GenInterpDefault(etime, linearFloatKeys, EKT_LINKEY, keyCount, sizeof(cjAniKeyLinFloat));
        cjAniKeyLinFloat* newKey = &newKeys[insIndex];

        newKey->m_time	= etime;
        newKey->m_value	= v;

        keyCount++;
        delete[] linearFloatKeys;

        keys = newKeys;
        FillDerivedValsKey(keys, keyCount, sizeof(cjAniKeyLinFloat));

        return cjTrue;
    }
    else
	{
        return cjFalse;
	}
}

void cjAniKeyLinFloat::CubicCoefsKey(const cjAniKey* keys, cjUINT32 keyCount, 
	cjUINT32 index, void* v0, void* vOutTangent, void* va0, void* vb0)
{
    cjAssert(index < keyCount - 1);
    const cjAniKeyLinFloat* linearKeyFloat = (const cjAniKeyLinFloat*)keys;

    *(cjFLOAT*)v0			= linearKeyFloat[index].m_value;
    *(cjFLOAT*)vOutTangent	= linearKeyFloat[index + 1].m_value - linearKeyFloat[index].m_value;
    *(cjFLOAT*)va0			= 0.0f;
    *(cjFLOAT*)vb0			= 0.0f;
}

void cjAniKeyLinFloat::InterpolateKey(cjFLOAT rtime, const cjAniKey* key0, const cjAniKey* key1, void* rst)
{
    const cjAniKeyLinFloat* lin0 = (const cjAniKeyLinFloat*)key0;
    const cjAniKeyLinFloat* lin1 = (const cjAniKeyLinFloat*)key1;

    *(cjFLOAT*)rst = cjScalarInterpolate::DoLinear(rtime, lin0->m_value, lin1->m_value);
}

//
// cjAniKeyFloat
//   - cjAniKeyBezFloat
//

CJ_ANIKEYOPER_IMPL(cjAniKeyBezFloat, EKVAL_FLOAT, EKT_BEZKEY);


void cjAniKeyBezFloat::RegisterCustomAniKeyOperation(EKEY_VALTYPE valType, EKEY_TYPE keyType)
{
	SetIsPosedKeyPF				(valType, keyType, IsPosedKey);
	SetEqualKeyPF				(valType, keyType, EqualKey);

	SetInsertKeyPF				(valType, keyType, InsertKey);

    SetCurvedRatioKeyPF			(valType, keyType, cjNULL);
	SetFillDerivedValsKeyPF		(valType, keyType, FillDerivedValsKey);
	SetCubicCoefsKeyPF			(valType, keyType, CubicCoefsKey);
    SetInterpKeyPF				(valType, keyType, InterpolateKey);
    SetInterpD1KeyPF			(valType, keyType, cjNULL);
    SetInterpD2KeyPF			(valType, keyType, cjNULL);    
}

void cjAniKeyBezFloat::SaveKey(cjFStream& stream, cjAniKey* keys, cjUINT32 keyCount)
{
    cjAniKeyBezFloat* k = (cjAniKeyBezFloat*)keys;

    for(cjUINT32 i=0; i<keyCount; i++)
		stream << k[i];
}

cjAniKey* cjAniKeyBezFloat::LoadKey(cjFStream& stream, cjUINT32 keyCount)
{
    cjAniKeyBezFloat* k = new cjAniKeyBezFloat[keyCount];

	cjAssert(stream.IsLoading());

    for(cjUINT32 i=0; i<keyCount; i++)
        stream << k[i];

	return k;
}

cjAniKey* cjAniKeyBezFloat::CreateKey(cjUINT32 keyCount)
{
    return new cjAniKeyBezFloat[keyCount];
}

void cjAniKeyBezFloat::DeleteKey(cjAniKey* keys)
{
    cjAniKeyBezFloat* k = (cjAniKeyBezFloat*)keys;
    delete [] k;
}

void cjAniKeyBezFloat::CopyKey(cjAniKey* dstKey, const cjAniKey* srcKey)
{
    cjAniKeyBezFloat* d = (cjAniKeyBezFloat*)dstKey;
    cjAniKeyBezFloat* s = (cjAniKeyBezFloat*)srcKey;

    d->m_time	= s->m_time;
    d->m_value	= s->m_value;
    d->m_inTan	= s->m_inTan;
    d->m_outTan = s->m_outTan;
}

cjBool cjAniKeyBezFloat::IsPosedKey(const cjAniKey* keys, cjUINT32 keyCount)
{
    if(keyCount > 1)
    {
        const cjAniKeyBezFloat* bezKeys = (const cjAniKeyBezFloat*)keys;

        if(bezKeys[0].m_inTan != 0.0f || bezKeys[0].m_outTan != 0.0f)
            return cjFalse;

        cjFLOAT value = bezKeys[0].m_value;

        for(cjUINT32 i=1; i<keyCount; i++)
        {
            if(bezKeys[i].m_value != value || bezKeys[i].m_inTan != 0.0f || bezKeys[i].m_outTan != 0.0f)
                return cjFalse;
        }
    }

    return cjTrue;
}

cjBool cjAniKeyBezFloat::EqualKey(const cjAniKey& key0, const cjAniKey& key1)
{
    if(!cjAniKeyFloat::EqualKey(key0,key1) )
        return cjFalse;

    const cjAniKeyBezFloat& bez0 = (const cjAniKeyBezFloat&)key0;
    const cjAniKeyBezFloat& bez1 = (const cjAniKeyBezFloat&)key1;

    return (bez0.m_inTan == bez1.m_inTan && bez0.m_outTan == bez1.m_outTan);
}

cjBool cjAniKeyBezFloat::InsertKey(cjFLOAT etime, cjAniKey*& keys, cjUINT32& keyCount)
{
    cjAniKeyBezFloat* bezFloatKeys = (cjAniKeyBezFloat*)keys;

    cjUINT32 dstSiz;
    cjUINT32 insIndex;

    if(TestKeyInsertable(etime, bezFloatKeys, keyCount, sizeof(cjAniKeyBezFloat), insIndex))
    {
        cjAniKeyBezFloat* newKeys = new cjAniKeyBezFloat[keyCount + 1];
        dstSiz = (keyCount + 1) * sizeof(cjAniKeyBezFloat);

        cjMemCpy(newKeys, dstSiz, bezFloatKeys, insIndex * sizeof(cjAniKeyBezFloat));

        if(keyCount > insIndex)
        {
            dstSiz = (keyCount - insIndex) * sizeof(cjAniKeyBezFloat);
            cjMemCpy(&newKeys[insIndex + 1], &bezFloatKeys[insIndex], dstSiz);
        }

        cjFLOAT f = cjAniKeyFloat::GenInterpDefault(etime, bezFloatKeys, EKT_BEZKEY,
            keyCount, sizeof(cjAniKeyBezFloat));

        cjAniKeyBezFloat* newKey = &newKeys[insIndex];

        newKey->m_time = etime;
        newKey->m_value = f;
        newKey->m_inTan = 0.0f;
        newKey->m_outTan = 0.0f;

        if(insIndex != 0 && insIndex != keyCount)
        {
            cjAniKeyBezFloat* lastKey = &newKeys[insIndex - 1];

            cjFLOAT lastVal = lastKey->GetValue();
            cjFLOAT lastTime = lastKey->GetTime();
            cjFLOAT lastOut = lastKey->GetOutTan();
    
            cjFLOAT curVal = newKey->GetValue();
            cjFLOAT curTime = newKey->GetTime();    
            cjFLOAT curIn, curOut;
    
            cjAniKeyBezFloat* nextKey = &newKeys[insIndex + 1];

            cjFLOAT nextVal = nextKey->GetValue();
            cjFLOAT nextTime = nextKey->GetTime();
            cjFLOAT nextIn = nextKey->GetInTan();
    
            cjScalarInterpolate::DoAdjustBezier(lastVal, lastTime, lastOut,
                nextVal, nextTime, nextIn, curTime, curVal, curIn, curOut);
        
            lastKey->SetOutTan(lastOut);

            newKey->SetInTan(curIn);
            newKey->SetOutTan(curOut);

            nextKey->SetInTan(nextIn);
		}

        keyCount++;

        delete [] bezFloatKeys;
        keys = newKeys;

        FillDerivedValsKey(keys, keyCount, sizeof(cjAniKeyBezFloat));

        return cjTrue;
    }
	
	return cjFalse;
}

void cjAniKeyBezFloat::InterpolateKey(cjFLOAT etime, const cjAniKey* key0, const cjAniKey* key1, void* rst)
{
    const cjAniKeyBezFloat* k0 = (const cjAniKeyBezFloat*)key0;
    const cjAniKeyBezFloat* k1 = (const cjAniKeyBezFloat*)key1;

    *(cjFLOAT*)rst = cjScalarInterpolate::DoBezier(etime, k0->GetValue(), k0->GetOutTan(), 
		k1->GetValue(), k1->GetInTan());
}

void cjAniKeyBezFloat::CubicCoefsKey(const cjAniKey* keys, cjUINT32 keyCount, cjUINT32 index, 
	void* v0, void* vOutTan0, void* va0, void* vb0)
{
    cjAssert(index < keyCount-1);
    const cjAniKeyBezFloat* bezKeys = (const cjAniKeyBezFloat*)keys;

    cjFLOAT kv0 = bezKeys[index].m_value;
    cjFLOAT kOutTan0 = bezKeys[index].m_outTan;
    cjFLOAT kv1 = bezKeys[index + 1].m_value;
    cjFLOAT kInTan1 = bezKeys[index + 1].m_inTan;

    *(cjFLOAT*)v0 = kv0;
    *(cjFLOAT*)vOutTan0 = kOutTan0;
    *(cjFLOAT*)va0 = 3 * (kv1 - kv0) - (kInTan1 + 2 * kOutTan0);
    *(cjFLOAT*)vb0 = kOutTan0 + kInTan1 - 2 * (kv1 - kv0);
}


//
// cjAniKeyFloat
//   - cjAniKeyTCBFloat
//

CJ_ANIKEYOPER_IMPL(cjAniKeyTCBFloat, EKVAL_FLOAT, EKT_TCBKEY);

void cjAniKeyTCBFloat::RegisterCustomAniKeyOperation(EKEY_VALTYPE valType, EKEY_TYPE keyType)
{
	SetIsPosedKeyPF				(valType, keyType, IsPosedKey);
	SetEqualKeyPF				(valType, keyType, EqualKey);

	SetInsertKeyPF				(valType, keyType, InsertKey);

    SetCurvedRatioKeyPF			(valType, keyType, cjNULL);
	SetFillDerivedValsKeyPF		(valType, keyType, FillDerivedValsKey);
	SetCubicCoefsKeyPF			(valType, keyType, CubicCoefsKey);
    SetInterpKeyPF				(valType, keyType, InterpolateKey);
    SetInterpD1KeyPF			(valType, keyType, cjNULL);
    SetInterpD2KeyPF			(valType, keyType, cjNULL);    
}

void cjAniKeyTCBFloat::SaveKey(cjFStream& stream, cjAniKey* keys, cjUINT32 keyCount)
{
    cjAniKeyTCBFloat* k = (cjAniKeyTCBFloat*)keys;

    for(cjUINT32 i=0; i<keyCount; i++)
		stream << k[i];
}

cjAniKey* cjAniKeyTCBFloat::LoadKey(cjFStream& stream, cjUINT32 keyCount)
{
    cjAniKeyTCBFloat* k = new cjAniKeyTCBFloat[keyCount];

	cjAssert(stream.IsLoading());

    for(cjUINT32 i=0; i<keyCount; i++)
        stream << k[i];

	return k;
}

cjAniKey* cjAniKeyTCBFloat::CreateKey(cjUINT32 keyCount)
{
    return new cjAniKeyTCBFloat[keyCount];
}

void cjAniKeyTCBFloat::DeleteKey(cjAniKey* keys)
{
    cjAniKeyTCBFloat* k = (cjAniKeyTCBFloat*)keys;
    delete [] k;
}

void cjAniKeyTCBFloat::CopyKey(cjAniKey* dstKey, const cjAniKey* srcKey)
{
    cjAniKeyTCBFloat* d = (cjAniKeyTCBFloat*)dstKey;
    cjAniKeyTCBFloat* s = (cjAniKeyTCBFloat*)srcKey;

    d->m_time		= s->m_time;
    d->m_value		= s->m_value;

    d->m_tension	= s->m_tension;
    d->m_continuity = s->m_continuity;
    d->m_bias		= s->m_bias;

    d->m_diffSrc	= s->m_diffSrc;
    d->m_diffDst	= s->m_diffDst;
}

cjBool cjAniKeyTCBFloat::IsPosedKey(const cjAniKey* keys, cjUINT32 keyCount)
{
    cjAssert(keys);
    cjAssert(keyCount >= 2);

    if(keyCount > 1)
    {
        const cjAniKeyTCBFloat* tcbKeys = (const cjAniKeyTCBFloat*)keys;

        cjFLOAT value = tcbKeys[0].m_value;
        for(cjUINT32 i=0; i<keyCount; i++)
        {
            if(tcbKeys[i].m_value != value || tcbKeys[i].m_diffSrc != 0.0f ||  tcbKeys[i].m_diffDst != 0.0f)
                return cjFalse;
        }
    }

    return cjTrue;
}

cjBool cjAniKeyTCBFloat::EqualKey(const cjAniKey& key0, const cjAniKey& key1)
{
    if(!cjAniKeyFloat::EqualKey(key0, key1))
        return cjFalse;

    const cjAniKeyTCBFloat& k0 = (const cjAniKeyTCBFloat&)key0;
    const cjAniKeyTCBFloat& k1 = (const cjAniKeyTCBFloat&)key1;

    return k0.m_tension == k1.m_tension && k0.m_continuity == k1.m_continuity && k0.m_bias == k1.m_bias;
}

cjBool cjAniKeyTCBFloat::InsertKey(cjFLOAT etime, cjAniKey*& keys, cjUINT32& keyCount)
{
    cjAniKeyTCBFloat* tcbFloatKeys = (cjAniKeyTCBFloat*)keys;

    cjUINT32 dstSiz;
    cjUINT32 insIndex;

    if(TestKeyInsertable(etime, tcbFloatKeys, keyCount, sizeof(cjAniKeyTCBFloat), insIndex))
    {
        cjAniKeyTCBFloat* newKeys = new cjAniKeyTCBFloat[keyCount + 1];

        dstSiz = (keyCount+1) * sizeof(cjAniKeyTCBFloat);
        cjMemCpy(newKeys, dstSiz, tcbFloatKeys, insIndex * sizeof(cjAniKeyTCBFloat));

        if(keyCount > insIndex)
        {
            dstSiz = (keyCount - insIndex) * sizeof(cjAniKeyTCBFloat);
            cjMemCpy(&newKeys[insIndex + 1], &tcbFloatKeys[insIndex], dstSiz);
        }

        cjFLOAT f = cjAniKeyFloat::GenInterpDefault(etime, tcbFloatKeys, EKT_TCBKEY, 
			keyCount, sizeof(cjAniKeyTCBFloat));

        cjAniKeyTCBFloat* newKey = &newKeys[insIndex];
        newKey->m_time = etime;
        newKey->m_value = f;
        newKey->m_tension = 0.0f;
        newKey->m_continuity = 0.0f;
        newKey->m_bias = 0.0f;

        keyCount++;

        delete [] tcbFloatKeys;
        keys = newKeys;

        FillDerivedValsKey(keys, keyCount, sizeof(cjAniKeyTCBFloat));

        return cjTrue;
    }

	return cjFalse;
}

void cjAniKeyTCBFloat::FillDerivedValsKey(cjAniKey* keys, cjUINT32 keyCount, cjBYTE )
{
    cjAniKeyTCBFloat* tcbFloatKeys = (cjAniKeyTCBFloat*)keys;

    if(keyCount >= 2)
    {
        cjAniKeyTCBFloat::CalculateDiffVals(tcbFloatKeys[0], (tcbFloatKeys[0].m_value * 2.0f) - tcbFloatKeys[1].m_value,
            tcbFloatKeys[1].m_value, 1, 1);

        cjUINT32 keyCount1 = keyCount-1;
        for(cjUINT32 i=1; i<keyCount1; i++)
        {
            cjAniKeyTCBFloat::CalculateDiffVals(tcbFloatKeys[i], tcbFloatKeys[i-1].m_value, 
                tcbFloatKeys[i+1].m_value, tcbFloatKeys[i].m_time - tcbFloatKeys[i-1].m_time,
                tcbFloatKeys[i+1].m_time - tcbFloatKeys[i].m_time);
        }

        cjUINT32 keyCount2 = keyCount - 2;
        cjAniKeyTCBFloat::CalculateDiffVals(tcbFloatKeys[keyCount1], tcbFloatKeys[keyCount2].m_value,
            2.0f * tcbFloatKeys[keyCount1].m_value - tcbFloatKeys[keyCount2].m_value, 1, 1);
    }
}

void cjAniKeyTCBFloat::InterpolateKey(cjFLOAT etime, const cjAniKey* key0, const cjAniKey* key1, void* rst)
{
    const cjAniKeyTCBFloat* k0 = (const cjAniKeyTCBFloat*)key0;
    const cjAniKeyTCBFloat* k1 = (const cjAniKeyTCBFloat*)key1;

    *(cjFLOAT*)rst = cjScalarInterpolate::DoTCB(etime, k0->GetValue(), k0->m_diffDst, k1->GetValue(), k1->m_diffSrc);
}

void cjAniKeyTCBFloat::CubicCoefsKey(const cjAniKey* keys, cjUINT32 keyCount, cjUINT32 index, 
	void* v0, void* vOutTan0, void* va0, void* vb0)
{
    cjAssert(keyCount >= 2);
    cjAssert(index < keyCount - 1);

    const cjAniKeyTCBFloat* tcbKeys = (const cjAniKeyTCBFloat*)keys;

    cjFLOAT kv0 = tcbKeys[index].m_value;
    cjFLOAT dd0 = tcbKeys[index].m_diffDst;
    cjFLOAT kv1 = tcbKeys[index + 1].m_value;
    cjFLOAT ds1 = tcbKeys[index + 1].m_diffSrc;

    *(cjFLOAT*)v0 = kv0;
    *(cjFLOAT*)vOutTan0 = dd0;

    *(cjFLOAT*)va0 = 3 * (kv1 - kv0) - (ds1 + 2 * dd0);
    *(cjFLOAT*)vb0 = dd0 + ds1 - 2 * (kv1 - kv0);
}

void cjAniKeyTCBFloat::CalculateDiffVals(cjAniKeyTCBFloat& rst, cjFLOAT valSub1, cjFLOAT valPlus1, 
	cjFLOAT preLen, cjFLOAT nextLen)
{
    cjFLOAT diff1 = rst.GetValue() - valSub1;
    cjFLOAT diff2 = valPlus1 - rst.GetValue();

    cjFLOAT opc = 1.0f + rst.GetContinuity();
    cjFLOAT omc = 1.0f - rst.GetContinuity();
    cjFLOAT opb = 1.0f + rst.GetBias();
    cjFLOAT omb = 1.0f - rst.GetBias();

    cjFLOAT homt = 0.5f*(1.0f-rst.GetTension());
    cjFLOAT homtopc = homt*opc;
    cjFLOAT homtomc = homt*omc;

    rst.m_diffSrc = (homtomc*opb)*diff1 + (homtopc*omb)*diff2;
    rst.m_diffDst = (homtopc*opb)*diff1 + (homtomc*omb)*diff2;

    cjFLOAT fInv = 2.0f/(preLen + nextLen);

    rst.m_diffSrc *= preLen*fInv;
    rst.m_diffDst *= nextLen*fInv;
}


//
// cjAniKeyVec3
//

CJ_ANIKEYOPER_IMPL(cjAniKeyVec3, EKVAL_VEC3, EKT_NOINTERP);

cjINT32 cjAniKeyVec3::ms_sampleSiz		= 20;
cjFLOAT cjAniKeyVec3::ms_sampleDelta	= 1.0f / ms_sampleSiz;
cjVec3 cjAniKeyVec3::ms_default			= cjVec3::ZERO;

void cjAniKeyVec3::RegisterCustomAniKeyOperation(EKEY_VALTYPE valType, EKEY_TYPE keyType)
{
    SetIsPosedKeyPF			(valType, keyType, cjNULL);
	SetEqualKeyPF			(valType, keyType, EqualKey);

    SetInsertKeyPF			(valType, keyType, cjNULL);

	SetCurvedRatioKeyPF		(valType, keyType, CaclCurveRatioKey);
    SetFillDerivedValsKeyPF	(valType, keyType, FillDerivedValsKey);
    SetCubicCoefsKeyPF		(valType, keyType, cjNULL);
	SetInterpKeyPF			(valType, keyType, InterpolateKey);
    SetInterpD1KeyPF		(valType, keyType, InterpolateD1Key);
    SetInterpD2KeyPF		(valType, keyType, InterpolateD2Key);
}

cjVec3 cjAniKeyVec3::GenInterp(cjFLOAT etime, cjAniKeyVec3* keys,
    cjAniKeyVec3::EKEY_TYPE keyType, cjUINT32 keyCount, cjUINT32& lastIdx, cjBYTE siz)
{
    cjAssert(keyCount != 0);

    if(keyCount == 1)
        return keys->GetKey(0, siz)->GetValue();

    cjUINT32 enKeyIndex = keyCount - 1;
    cjFLOAT lastTime = keys->GetKey(lastIdx, siz)->GetTime();

    if(etime < lastTime)
    {
        lastIdx = 0;
        lastTime = keys->GetKey(0, siz)->GetTime();
    }
    
    cjUINT32 curKeyIdx;
    cjFLOAT nextTime = 0.0f;
    for(curKeyIdx=lastIdx+1; curKeyIdx<=enKeyIndex; curKeyIdx++)
    {
        nextTime = keys->GetKey(curKeyIdx, siz)->GetTime();

        if(etime <= nextTime)
            break;

        lastIdx++;
        lastTime = nextTime;
    }

    cjAssert(curKeyIdx < keyCount);

    cjFLOAT ratio = (etime - lastTime)/(nextTime - lastTime);

    cjAniKeyVec3::PFInterpolation interp = cjAniKeyVec3::GetInterpKeyPF(keyType);
    cjAssert(interp);

    cjVec3 rst;
    interp(ratio, keys->GetKey(lastIdx, siz), keys->GetKey(curKeyIdx, siz), &rst);

    return rst;
}

void cjAniKeyVec3::GenCubicCoefs(cjFLOAT etime, cjAniKeyVec3* keys, EKEY_TYPE keyType, 
	cjUINT32 keyCount, cjUINT32& lastIdx, cjBYTE siz, cjFLOAT& valTime0, cjFLOAT& valTime1, 
    cjVec3& val0, cjVec3& outTan, cjVec3& valA, cjVec3& valB)
{
    cjAssert(keyCount != 0);

    if(keyCount == 1)
    {
        val0		= keys->GetKey(0, siz)->GetValue();
        outTan		= cjVec3::ZERO;
        valA		= cjVec3::ZERO;
        valB		= cjVec3::ZERO;
        valTime0	= 0.0f;
        valTime1	= CJ_FLT_MAX;
        lastIdx		= 0;
        return;
    }

    cjUINT32 enKeyIndex = keyCount - 1;
    cjUINT32 stkLastIdx = lastIdx;

    cjFLOAT lastTime = keys->GetKey(stkLastIdx, siz)->GetTime();
    if(etime < lastTime)
    {
        stkLastIdx = 0;
        lastTime = keys->GetKey(0, siz)->GetTime();
    }

    cjUINT32 curKeyIdx;
    cjFLOAT nextTime = 0.0f;
    for(curKeyIdx=stkLastIdx + 1; curKeyIdx<=enKeyIndex; curKeyIdx++)
    {
        nextTime = keys->GetKey(curKeyIdx, siz)->GetTime();

        if((etime < nextTime) || (etime == nextTime && keyType != EKT_STEPKEY))
            break;

        stkLastIdx++;
        lastTime = nextTime;
    }
    
    if(keyType == EKT_STEPKEY && etime >= nextTime && curKeyIdx == keyCount)
    {
        val0 = keys->GetKey(enKeyIndex, siz)->GetValue();
        outTan = cjVec3::ZERO;
        valA = cjVec3::ZERO;
        valB = cjVec3::ZERO;
        valTime0 = nextTime;
        valTime1 = nextTime;
        lastIdx = enKeyIndex;
    }
    else
    {
        cjAssert(curKeyIdx < keyCount);

        cjAniKey::PFCubicCoefs cubiccoefs = cjAniKeyVec3::GetCubicCoefsKeyPF(keyType);
        cjAssert(cubiccoefs);
        cubiccoefs(keys, keyCount, stkLastIdx, &val0, &outTan, &valA, &valB);
        valTime0 = lastTime;
        valTime1 = nextTime;
        lastIdx = stkLastIdx;
    }
}

cjVec3 cjAniKeyVec3::GenInterpDefault(cjFLOAT etime, cjAniKeyVec3* keys, EKEY_TYPE keyType, 
	cjUINT32 keyCount, cjBYTE siz)
{
    if(keyCount)
    {
        if(etime < keys->GetKey(0, siz)->GetTime())
            return keys->GetKey(0, siz)->GetValue();
        else if(etime > keys->GetKey(keyCount-1, siz)->GetTime())
            return keys->GetKey(keyCount-1, siz)->GetValue();
        else
        {
            cjUINT32 lastIdx = 0;
            return cjAniKeyVec3::GenInterp(etime, keys, keyType, keyCount, lastIdx, siz);
        }
    }

	return ms_default;
}

cjBool cjAniKeyVec3::EqualKey(const cjAniKey& k0, const cjAniKey& k1)
{
    if(!cjAniKey::EqualKey(k0, k1))
        return cjFalse;

    const cjAniKeyVec3& v0 = (const cjAniKeyVec3&)k0;
    const cjAniKeyVec3& v1 = (const cjAniKeyVec3&)k1;

    return v0.m_value == v1.m_value;
}

void cjAniKeyVec3::CoordinateFrame(cjFLOAT etime, const cjAniKeyVec3* key0,
    const cjAniKeyVec3* key1, cjAniKeyVec3::EKEY_TYPE keyType, cjVec3& tangent,
    cjVec3& normal, cjVec3& binormal, cjFLOAT& curvature)
{
    PFInterpolation d1Func = cjAniKeyVec3::GetInterpD1KeyPF(keyType);
    PFInterpolation d2Func = cjAniKeyVec3::GetInterpD2KeyPF(keyType);
    
    cjVec3 d1, d2;
    d1Func(etime,key0,key1,&d1);
    d2Func(etime,key0,key1,&d2);

    cjFLOAT lenSquare = d1.SquareXYZ();
    cjFLOAT length = cjSqrt(lenSquare);
    cjFLOAT invLength = 1.0f / length;

    tangent = invLength*d1;

    cjVec3 cross = cjVec3::Cross(d1, d2);
    curvature = cross.Length() * invLength * invLength;

    const cjFLOAT epsilon = 1e-06f;
    if(cjAbs(curvature) > epsilon)
    {
        normal = (d2 * lenSquare) - (d1 * cjVec3::Dot(d1, d2));
    }
    else
    {
        curvature = 0.0f;

        if(cjAbs(tangent.x) > epsilon || cjAbs(tangent.y) > epsilon)
        {
            normal.x = tangent.y;
            normal.y = -tangent.x;
            normal.z = 0;
        }
        else
        {
            normal.z = tangent.y;
            normal.y = -tangent.z;
            normal.x = 0;
        }

    }

    normal.Normalize();
    binormal = cjVec3::Cross(tangent, normal);
}


//
// cjAniKeyVec3 - cjAniKeyLinVec3
//

CJ_ANIKEYOPER_IMPL(cjAniKeyLinVec3, EKVAL_VEC3, EKT_LINKEY);


void cjAniKeyLinVec3::RegisterCustomAniKeyOperation(EKEY_VALTYPE valType, EKEY_TYPE keyType)
{
	// Save/Load/Create/Delete/Copy is setted at CJ_ANIKEYOPER_IMPL

	SetIsPosedKeyPF			(valType, keyType, IsPosedKey);
	SetEqualKeyPF			(valType, keyType, EqualKey);
	
	SetInsertKeyPF			(valType, keyType, InsertKey);

    SetCurvedRatioKeyPF		(valType, keyType, CaclCurveRatioKey);
	SetFillDerivedValsKeyPF	(valType, keyType, FillDerivedValsKey);
	SetCubicCoefsKeyPF		(valType, keyType, CubicCoefsKey);

    SetInterpKeyPF			(valType, keyType, InterpolateKey);
    SetInterpD1KeyPF		(valType, keyType, InterpolateD1Key);
    SetInterpD2KeyPF		(valType, keyType, InterpolateD2Key);
}

void cjAniKeyLinVec3::SaveKey(cjFStream& stream, cjAniKey* keys, cjUINT32 keyCount)
{
    cjAniKeyLinVec3* linearVec3Keys = (cjAniKeyLinVec3*)keys;

    for(cjUINT32 i=0; i<keyCount; i++)
		stream << linearVec3Keys[i];
}

cjAniKey* cjAniKeyLinVec3::LoadKey(cjFStream& stream, cjUINT32 keyCount)
{
    cjAniKeyLinVec3* linearVec3Keys = new cjAniKeyLinVec3[keyCount];

	cjAssert(stream.IsLoading());

    for(cjUINT32 i=0; i<keyCount; i++)
		stream << linearVec3Keys[i];

	return linearVec3Keys;
}

cjAniKey* cjAniKeyLinVec3::CreateKey(cjUINT32 keyCount)
{
    return new cjAniKeyLinVec3[keyCount];
}

void cjAniKeyLinVec3::DeleteKey(cjAniKey* keys)
{
    cjAniKeyLinVec3* linearVec3Keys = (cjAniKeyLinVec3*)keys;
	delete [] linearVec3Keys;
}

void cjAniKeyLinVec3::CopyKey(cjAniKey* dstKey, const cjAniKey* srcKey)
{
    cjAniKeyLinVec3* dst = (cjAniKeyLinVec3*)dstKey;
    cjAniKeyLinVec3* src = (cjAniKeyLinVec3*)srcKey;

    dst->m_time	 = src->m_time;
    dst->m_value = src->m_value;
}

cjBool cjAniKeyLinVec3::IsPosedKey(const cjAniKey* keys, cjUINT32 keyCount)
{
    if(keyCount > 1)
    {
        const cjAniKeyLinVec3* linearVec3Keys = (const cjAniKeyLinVec3*)keys;
        cjVec3 v = linearVec3Keys[0].m_value;

        for(cjUINT32 i=1; i<keyCount; i++)
        {
            if(linearVec3Keys[i].m_value != v)
                return cjFalse;
        }
    }

    return cjTrue;
}

cjBool cjAniKeyLinVec3::InsertKey(cjFLOAT etime, cjAniKey*& keys, cjUINT32& keyCount)
{
    cjAniKeyLinVec3* linearVec3Keys = (cjAniKeyLinVec3*)keys;

    cjUINT32 dstSiz;
    cjUINT32 insIndex;

	if(TestKeyInsertable(etime, linearVec3Keys, keyCount, sizeof(cjAniKeyLinVec3), insIndex))
    {
        cjAniKeyLinVec3* newKeys = new cjAniKeyLinVec3[keyCount + 1];
        dstSiz = (keyCount + 1) * sizeof(cjAniKeyLinVec3);

        cjMemCpy(newKeys, dstSiz, linearVec3Keys, insIndex * sizeof(cjAniKeyLinVec3));

        if(keyCount > insIndex)
        {
            dstSiz = (keyCount - insIndex) * sizeof(cjAniKeyLinVec3);
            cjMemCpy(&newKeys[insIndex + 1], &linearVec3Keys[insIndex], dstSiz);
        }

        cjVec3 v = cjAniKeyVec3::GenInterpDefault(etime, linearVec3Keys, EKT_LINKEY, keyCount, sizeof(cjAniKeyLinVec3));
        cjAniKeyLinVec3* newKey = &newKeys[insIndex];

        newKey->m_time	= etime;
        newKey->m_value = v;

        keyCount++;
		delete [] linearVec3Keys;

        keys = newKeys;
        FillDerivedValsKey(keys, keyCount, sizeof(cjAniKeyLinVec3));

        return cjTrue;
    }

	return cjFalse;
}

void cjAniKeyLinVec3::CubicCoefsKey(const cjAniKey* keys, cjUINT32 keyCount, cjUINT32 index, 
	void* v0, void* vOutTangent, void* va0, void* vb0)
{
    cjAssert(index < keyCount - 1);

    const cjAniKeyLinVec3* linearVec3Keys = (const cjAniKeyLinVec3*)keys;

    *(cjVec3*)v0			= linearVec3Keys[index].m_value;
    *(cjVec3*)vOutTangent	= linearVec3Keys[index+1].m_value - linearVec3Keys[index].m_value;

    *(cjVec3*)va0			= cjVec3::ZERO;
    *(cjVec3*)vb0			= cjVec3::ZERO;
}

void cjAniKeyLinVec3::InterpolateKey(cjFLOAT rtime, const cjAniKey* key0, const cjAniKey* key1, void* rst)
{
    cjAniKeyVec3* posKey0 = (cjAniKeyVec3*)key0;
    cjAniKeyVec3* posKey1 = (cjAniKeyVec3*)key1;

    *(cjVec3*)rst = rtime * posKey1->m_value + (1.0f-rtime) * posKey0->m_value;
}

void cjAniKeyLinVec3::InterpolateD1Key(cjFLOAT, const cjAniKey* key0, const cjAniKey* key1, void* rst)
{
    cjAniKeyVec3* posKey0 = (cjAniKeyVec3*)key0;
    cjAniKeyVec3* posKey1 = (cjAniKeyVec3*)key1;

    *(cjVec3*)rst = posKey1->m_value - posKey0->m_value;
}

void cjAniKeyLinVec3::InterpolateD2Key(cjFLOAT, const cjAniKey*, const cjAniKey*, void* rst)
{
    *(cjVec3*)rst = cjVec3::ZERO;
}


//
// cjAniKeyVec3 
//    - cjAniKeyBezVec3
//

CJ_ANIKEYOPER_IMPL(cjAniKeyBezVec3, EKVAL_VEC3, EKT_BEZKEY);

void cjAniKeyBezVec3::RegisterCustomAniKeyOperation(EKEY_VALTYPE valType, EKEY_TYPE keyType)
{
	SetIsPosedKeyPF			(valType, keyType, IsPosedKey);
	SetEqualKeyPF			(valType, keyType, EqualKey);
	
	SetInsertKeyPF			(valType, keyType, InsertKey);

    SetCurvedRatioKeyPF		(valType, keyType, CaclCurveRatioKey);
	SetFillDerivedValsKeyPF	(valType, keyType, FillDerivedValsKey);
	SetCubicCoefsKeyPF		(valType, keyType, CubicCoefsKey);

    SetInterpKeyPF			(valType, keyType, InterpolateKey);
    SetInterpD1KeyPF		(valType, keyType, InterpolateD1Key);
    SetInterpD2KeyPF		(valType, keyType, InterpolateD2Key);
}

void cjAniKeyBezVec3::SaveKey(cjFStream& stream, cjAniKey* keys, cjUINT32 keyCount)
{
    cjAniKeyBezVec3* k = (cjAniKeyBezVec3*)keys;

    for(cjUINT32 i=0; i<keyCount; i++)
		stream << k[i];
}

cjAniKey* cjAniKeyBezVec3::LoadKey(cjFStream& stream, cjUINT32 keyCount)
{
    cjAniKeyBezVec3* k = new cjAniKeyBezVec3[keyCount];

	cjAssert(stream.IsLoading());

    for(cjUINT32 i=0; i<keyCount; i++)
		stream << k[i];

	return k;
}

cjAniKey* cjAniKeyBezVec3::CreateKey(cjUINT32 keyCount)
{
    return new cjAniKeyBezVec3[keyCount];
}

void cjAniKeyBezVec3::DeleteKey(cjAniKey* keys)
{
    cjAniKeyBezVec3* k = (cjAniKeyBezVec3*)keys;
	delete [] k;
}

void cjAniKeyBezVec3::CopyKey(cjAniKey* dstKey, const cjAniKey* srcKey)
{
    cjAniKeyBezVec3* dst = (cjAniKeyBezVec3*)dstKey;
    cjAniKeyBezVec3* src = (cjAniKeyBezVec3*)srcKey;

    dst->m_time		= src->m_time;
    dst->m_value	= src->m_value;

    dst->m_inTan	= src->m_inTan;
    dst->m_outTan	= src->m_outTan;
    dst->m_a		= src->m_a;
    dst->m_b		= src->m_b;
}

cjBool cjAniKeyBezVec3::IsPosedKey(const cjAniKey* keys, cjUINT32 keyCount)
{
    if(keyCount > 1)
    {
        const cjAniKeyBezVec3* bezVec3Keys = (const cjAniKeyBezVec3*)keys;
        cjVec3 value = bezVec3Keys[0].m_value;

        for(cjUINT32 i=0; i<keyCount; i++)
        {
            if(bezVec3Keys[i].m_value != value ||
                bezVec3Keys[i].m_inTan != cjVec3::ZERO ||
                bezVec3Keys[i].m_outTan != cjVec3::ZERO)
            {
                return cjFalse;
            }
        }
    }

    return cjTrue;
}

cjBool cjAniKeyBezVec3::EqualKey(const cjAniKey& key0, const cjAniKey& key1)
{
    if(!cjAniKeyVec3::EqualKey(key0, key1))
        return cjFalse;

    const cjAniKeyBezVec3& bez0 = (const cjAniKeyBezVec3&)key0;
    const cjAniKeyBezVec3& bez1 = (const cjAniKeyBezVec3&)key1;

    return bez0.m_inTan == bez1.m_inTan && bez0.m_outTan == bez1.m_outTan;
}

cjBool cjAniKeyBezVec3::InsertKey(cjFLOAT etime, cjAniKey*& keys, cjUINT32& keyCount)
{
    cjAniKeyBezVec3* bezVec3Keys = (cjAniKeyBezVec3*)keys;

    cjUINT32 dstSiz;
    cjUINT32 insIndex;

    if(TestKeyInsertable(etime, bezVec3Keys, keyCount, sizeof(cjAniKeyBezVec3), insIndex))
    {
        cjAniKeyBezVec3* newKeys = new cjAniKeyBezVec3[keyCount + 1];
        dstSiz = (keyCount + 1) * sizeof(cjAniKeyBezVec3);

        cjMemCpy(newKeys, dstSiz, bezVec3Keys, insIndex * sizeof(cjAniKeyBezVec3));

        if(keyCount > insIndex)
        {
            dstSiz = (keyCount - insIndex) * sizeof(cjAniKeyBezVec3);
            cjMemCpy(&newKeys[insIndex + 1], &bezVec3Keys[insIndex], dstSiz);
        }

        cjVec3 value = cjAniKeyVec3::GenInterpDefault(etime, bezVec3Keys, EKT_BEZKEY, keyCount, sizeof(cjAniKeyBezVec3));

        cjAniKeyBezVec3* newKey = &newKeys[insIndex];
        newKey->m_time = etime;
        newKey->m_value = value;
        newKey->m_inTan = cjVec3::ZERO;
        newKey->m_outTan = cjVec3::ZERO;

        if(insIndex && (insIndex != keyCount))
        {
            cjAniKeyBezVec3* lastKey = &newKeys[insIndex - 1];
            cjVec3 lastValue = lastKey->GetValue();
            cjFLOAT lastTime = lastKey->GetTime();
            cjVec3 lastOut = lastKey->GetOutTan();
    
            cjVec3 curValue = newKey->GetValue();
            cjFLOAT curTime = newKey->GetTime();    
            cjVec3 curIn, curOut;
    
            cjAniKeyBezVec3* nextKey = &newKeys[insIndex + 1];
            cjVec3 nextValue = nextKey->GetValue();
            cjFLOAT nextTime = nextKey->GetTime();
            cjVec3 nextIn = nextKey->GetInTan();
    
            cjScalarInterpolate::DoAdjustBezier(lastValue.x, lastTime, lastOut.x, nextValue.x, nextTime, nextIn.x,
                curTime, curValue.x, curIn.x, curOut.x);
            cjScalarInterpolate::DoAdjustBezier(lastValue.y, lastTime, lastOut.y, nextValue.y, nextTime, nextIn.y,
                curTime, curValue.y, curIn.y, curOut.y);
            cjScalarInterpolate::DoAdjustBezier(lastValue.z, lastTime, lastOut.z, nextValue.z, nextTime, nextIn.z,
                curTime, curValue.z, curIn.z, curOut.z);
        
            lastKey->SetOutTan(lastOut);
    
            newKey->SetInTan(curIn);
            newKey->SetOutTan(curOut);
    
            nextKey->SetInTan(nextIn);
        }

        keyCount++;

        delete [] bezVec3Keys;
        keys = newKeys;

        FillDerivedValsKey(keys, keyCount, sizeof(cjAniKeyBezVec3));

        return cjTrue;
    }

	return cjFalse;
}

void cjAniKeyBezVec3::FillDerivedValsKey(cjAniKey* keys, cjUINT32 keyCount, cjBYTE )
{
    cjAniKeyBezVec3* bezVec3Keys = (cjAniKeyBezVec3*)keys;

    for(cjUINT32 i=0; i<keyCount-1; i++)
    {
        cjAniKeyBezVec3* bez0 = &bezVec3Keys[i];
        cjAniKeyBezVec3* bez1 = &bezVec3Keys[i+1];
		        
        bez0->m_a = 3*(bez1->m_value - bez0->m_value) - (bez1->m_inTan + 2*bez0->m_outTan); // A = 3(X1 - X0) - (T1 + 2T0)        
        bez0->m_b = bez0->m_outTan + bez1->m_inTan - 2*(bez1->m_value - bez0->m_value); // B = (T0 + T1) - 2(X1 - X0)
    }
}

void cjAniKeyBezVec3::CubicCoefsKey(const cjAniKey* keys, cjUINT32 keyCount, cjUINT32 index, void* v0, 
    void* vOutTan0, void* va0, void* vb0)
{
    cjAssert(index < keyCount - 1);

    const cjAniKeyBezVec3* bezKeys = (const cjAniKeyBezVec3*)keys;

    *(cjVec3*)v0 = bezKeys[index].m_value;
    *(cjVec3*)vOutTan0 = bezKeys[index].m_outTan;
    *(cjVec3*)va0 = bezKeys[index].m_a;
    *(cjVec3*)vb0 = bezKeys[index].m_b;
}

void cjAniKeyBezVec3::InterpolateKey(cjFLOAT rtime, const cjAniKey* key0, const cjAniKey*, void* rst)
{
    cjAniKeyBezVec3* bezVec3Key0 = (cjAniKeyBezVec3*)key0;
    *(cjVec3*)rst = bezVec3Key0->GetValue() + (bezVec3Key0->m_outTan + (bezVec3Key0->m_a + bezVec3Key0->m_b*rtime)*rtime)*rtime;
}

void cjAniKeyBezVec3::InterpolateD1Key(cjFLOAT etime, const cjAniKey* key0, const cjAniKey*, void* rst)
{
    cjAniKeyBezVec3* bezVec3Key0 = (cjAniKeyBezVec3*)key0;
    *(cjVec3*)rst = bezVec3Key0->m_outTan + (2.0f*bezVec3Key0->m_a + bezVec3Key0->m_b*(3.0f*etime))*etime;
}

void cjAniKeyBezVec3::InterpolateD2Key(cjFLOAT etime, const cjAniKey* key0, const cjAniKey*, void* rst)
{
    cjAniKeyBezVec3* bezVec3Key0 = (cjAniKeyBezVec3*)key0;
    *(cjVec3*)rst = 2.0f*bezVec3Key0->m_a + bezVec3Key0->m_b*(6.0f*etime);
}

cjFLOAT cjAniKeyBezVec3::CaclCurveRatioKey(cjAniKey* keys, cjUINT32 keyCount)
{
    cjAniKeyBezVec3* bezVec3Keys = (cjAniKeyBezVec3*)keys;

    cjFLOAT maxCurvature = 0.0f;

    for(cjUINT32 i=0; i<keyCount-1; i++)
    {
        cjAniKeyBezVec3* bez0 = &bezVec3Keys[i];
        cjAniKeyBezVec3* bez1 = &bezVec3Keys[i+1];

        for(cjINT32 m=0; m<=ms_sampleSiz; m++)
        {
            cjFLOAT etime = m * ms_sampleDelta;

            cjVec3 d1,d2;            
			InterpolateD1Key(etime, bez0, bez1, &d1);
            InterpolateD2Key(etime, bez0, bez1, &d2);

			cjFLOAT curvature = d1.Cross(d2).Length() / d1.SquareXYZ();
            
			if(curvature > maxCurvature)
                maxCurvature = curvature;
        }
    }

    return maxCurvature;
}


//
// cjAniKeyVec3 
//    - cjAniKeyTCBVec3
//

CJ_ANIKEYOPER_IMPL(cjAniKeyTCBVec3, EKVAL_VEC3, EKT_TCBKEY);

void cjAniKeyTCBVec3::RegisterCustomAniKeyOperation(EKEY_VALTYPE valType, EKEY_TYPE keyType)
{
	SetIsPosedKeyPF			(valType, keyType, IsPosedKey);
	SetEqualKeyPF			(valType, keyType, EqualKey);
	
	SetInsertKeyPF			(valType, keyType, InsertKey);

    SetCurvedRatioKeyPF		(valType, keyType, CaclCurveRatioKey);
	SetFillDerivedValsKeyPF	(valType, keyType, FillDerivedValsKey);
	SetCubicCoefsKeyPF		(valType, keyType, CubicCoefsKey);

    SetInterpKeyPF			(valType, keyType, InterpolateKey);
    SetInterpD1KeyPF		(valType, keyType, InterpolateD1Key);
    SetInterpD2KeyPF		(valType, keyType, InterpolateD2Key);
}

void cjAniKeyTCBVec3::SaveKey(cjFStream& stream, cjAniKey* keys, cjUINT32 keyCount)
{
    cjAniKeyTCBVec3* k = (cjAniKeyTCBVec3*)keys;

    for(cjUINT32 i=0; i<keyCount; i++)
		stream << k[i];
}

cjAniKey* cjAniKeyTCBVec3::LoadKey(cjFStream& stream, cjUINT32 keyCount)
{
    cjAniKeyTCBVec3* k = new cjAniKeyTCBVec3[keyCount];

	cjAssert(stream.IsLoading());

    for(cjUINT32 i=0; i<keyCount; i++)
		stream << k[i];

	return k;
}

cjAniKey* cjAniKeyTCBVec3::CreateKey(cjUINT32 keyCount)
{
    return new cjAniKeyTCBVec3[keyCount];
}

void cjAniKeyTCBVec3::DeleteKey(cjAniKey* keys)
{
    cjAniKeyTCBVec3* k = (cjAniKeyTCBVec3*)keys;
	delete [] k;
}

void cjAniKeyTCBVec3::CopyKey(cjAniKey* dstKey, const cjAniKey* srcKey)
{
    cjAniKeyTCBVec3* dst = (cjAniKeyTCBVec3*)dstKey;
    cjAniKeyTCBVec3* src = (cjAniKeyTCBVec3*)srcKey;

    dst->m_time = src->m_time;
    dst->m_value = src->m_value;

    dst->m_tension = src->m_tension;
    dst->m_continuity = src->m_continuity;
    dst->m_bias = src->m_bias;

    dst->m_DS = src->m_DS;
    dst->m_DD = src->m_DD;

    dst->m_a = src->m_a;
    dst->m_b = src->m_b;
}

cjBool cjAniKeyTCBVec3::IsPosedKey(const cjAniKey* keys, cjUINT32 keyCount)
{
    cjAssert(keys);
    cjAssert(keyCount >= 2);

    if(keyCount > 1)
    {
        const cjAniKeyTCBVec3* tcbKeys = (const cjAniKeyTCBVec3*)keys;
        cjVec3 value = tcbKeys[0].m_value;

		for(cjUINT32 i=0; i<keyCount; i++)
        {
            if(tcbKeys[i].m_value != value || tcbKeys[i].m_DS != cjVec3::ZERO ||  tcbKeys[i].m_DD != cjVec3::ZERO)
                return cjFalse;
        }
    }

    return cjTrue;
}

cjBool cjAniKeyTCBVec3::EqualKey(const cjAniKey& key0, const cjAniKey& key1)
{
    if(!cjAniKeyVec3::EqualKey(key0, key1))
        return cjFalse;

    const cjAniKeyTCBVec3& tcb0 = (const cjAniKeyTCBVec3&)key0;
    const cjAniKeyTCBVec3& tcb1 = (const cjAniKeyTCBVec3&)key1;

    return tcb0.m_tension == tcb1.m_tension && tcb0.m_continuity == tcb1.m_continuity && tcb0.m_bias == tcb1.m_bias;
}

cjBool cjAniKeyTCBVec3::InsertKey(cjFLOAT etime, cjAniKey*& keys, cjUINT32& keyCount)
{
    cjAniKeyTCBVec3* tcbVec3Keys = (cjAniKeyTCBVec3*)keys;

    cjUINT32 dstSiz;
    cjUINT32 insIndex;

    if(TestKeyInsertable(etime, tcbVec3Keys, keyCount, sizeof(cjAniKeyTCBVec3), insIndex))
    {
        cjAniKeyTCBVec3* newKeys = new cjAniKeyTCBVec3[keyCount + 1];
        dstSiz = (keyCount + 1) * sizeof(cjAniKeyTCBVec3);

        cjMemCpy(newKeys, dstSiz, tcbVec3Keys, insIndex * sizeof(cjAniKeyTCBVec3));

        if(keyCount > insIndex)
        {
            dstSiz = (keyCount - insIndex) * sizeof(cjAniKeyTCBVec3);
            cjMemCpy(&newKeys[insIndex + 1], &tcbVec3Keys[insIndex], dstSiz);
        }

        cjVec3 value = cjAniKeyVec3::GenInterpDefault(etime, tcbVec3Keys, EKT_TCBKEY, keyCount, sizeof(cjAniKeyTCBVec3));

        cjAniKeyTCBVec3* newKey = &newKeys[insIndex];
        newKey->m_time = etime;
        newKey->m_value = value;
        newKey->m_tension = 0.0f;
        newKey->m_continuity = 0.0f;
        newKey->m_bias = 0.0f;

        keyCount++;

        delete [] tcbVec3Keys;
        keys = newKeys;

        FillDerivedValsKey(keys, keyCount, sizeof(cjAniKeyTCBVec3));

		return cjTrue;
    }

	return cjFalse;
}

void cjAniKeyTCBVec3::FillDerivedValsKey(cjAniKey* keys, cjUINT32 keyCount, cjBYTE )
{
    cjAniKeyTCBVec3* tcbVec3Keys = (cjAniKeyTCBVec3*)keys;
    cjUINT32 keyCount1 = keyCount - 1;
    
    if(keyCount >= 2)
    {
        tcbVec3Keys[0].CalculateDiffVals(2 * tcbVec3Keys[0].m_value - tcbVec3Keys[1].m_value, tcbVec3Keys[1].m_value, 1, 1);
       
        for(cjUINT32 i=1; i<keyCount1; i++)
        {
            cjAniKeyTCBVec3* tcbM = &tcbVec3Keys[i-1];
            cjAniKeyTCBVec3* tcbZ = &tcbVec3Keys[i];
            cjAniKeyTCBVec3* tcbP = &tcbVec3Keys[i+1];

            tcbZ->CalculateDiffVals(tcbM->m_value, tcbP->m_value, tcbZ->m_time - tcbM->m_time, tcbP->m_time - tcbZ->m_time);
        }

        cjUINT32 keyCount2 = keyCount - 2;
        tcbVec3Keys[keyCount1].CalculateDiffVals(tcbVec3Keys[keyCount2].m_value, 
			2.0f * tcbVec3Keys[keyCount1].m_value - tcbVec3Keys[keyCount2].m_value, 1, 1);
    }

    for(cjUINT32 i=0; i<keyCount1; i++)
    {
        cjAniKeyTCBVec3* tcbA = &tcbVec3Keys[i];
        cjAniKeyTCBVec3* tcbB = &tcbVec3Keys[i+1];
		        
        tcbA->m_a = 3.0f*(tcbB->m_value - tcbA->m_value) - (tcbB->m_DS + 2.0f*tcbA->m_DD); // A = 3(P1 - P0) - (D1 + 2D0)        
        tcbA->m_b = tcbA->m_DD + tcbB->m_DS - 2.0f*(tcbB->m_value - tcbA->m_value); // B = (D0 + D1) - 2(P1 - P0)
    }
}

void cjAniKeyTCBVec3::CubicCoefsKey(const cjAniKey* keys, cjUINT32 keyCount, cjUINT32 index, 
	void* v0, void* vOutTan0, void* va0, void* vb0)
{
    cjAssert(keyCount >= 2);
    cjAssert(index < keyCount - 1);

    const cjAniKeyTCBVec3* tcbVec3Keys = (const cjAniKeyTCBVec3*)keys;

    *(cjVec3*)v0 = tcbVec3Keys[index].m_value;
    *(cjVec3*)vOutTan0 = tcbVec3Keys[index].m_DD;
    *(cjVec3*)va0 = tcbVec3Keys[index].m_a;
    *(cjVec3*)vb0 = tcbVec3Keys[index].m_b;
}

cjFLOAT cjAniKeyTCBVec3::CaclCurveRatioKey(cjAniKey* keys, cjUINT32 keyCount)
{
    cjAniKeyTCBVec3* tcbVec3Keys = (cjAniKeyTCBVec3*) keys;
    cjFLOAT maxCurvature = 0.0f;

    for(cjUINT32 i=0; i<keyCount-1; i++)
    {
        cjAniKeyTCBVec3* tcbA = &tcbVec3Keys[i];
        cjAniKeyTCBVec3* tcbB = &tcbVec3Keys[i+1];

        for(cjINT32 m=0; m<= ms_sampleSiz; m++)
        {
            cjFLOAT etime = m * ms_sampleDelta;

            cjVec3 d1, d2;
            InterpolateD1Key(etime,tcbA,tcbB,&d1);
            InterpolateD2Key(etime,tcbA,tcbB,&d2);

            cjFLOAT curvature = d1.Cross(d2).Length()/d1.SquareXYZ();

            if(curvature > maxCurvature)
                maxCurvature = curvature;
        }
    }

    return maxCurvature;
}

void cjAniKeyTCBVec3::InterpolateKey(cjFLOAT etime, const cjAniKey* key0, const cjAniKey*, void* rst)
{
    cjAniKeyTCBVec3* k = (cjAniKeyTCBVec3*)key0;
    *(cjVec3*)rst = k->m_value + (k->m_DD + (k->m_a + k->m_b*etime)*etime)*etime;
}

void cjAniKeyTCBVec3::InterpolateD1Key(cjFLOAT etime, const cjAniKey* key0, const cjAniKey*, void* rst)
{
    cjAniKeyTCBVec3* k = (cjAniKeyTCBVec3*)key0;
    *(cjVec3*)rst = k->m_DD + (2.0f*k->m_a + k->m_b*(3.0f*etime))*etime;
}

void cjAniKeyTCBVec3::InterpolateD2Key(cjFLOAT etime, const cjAniKey* key0, const cjAniKey*, void* rst)
{
    cjAniKeyTCBVec3* k0 = (cjAniKeyTCBVec3*) key0;
    *(cjVec3*)rst = 2.0f*k0->m_a + k0->m_b*(6.0f*etime);
}

void cjAniKeyTCBVec3::CalculateDiffVals(const cjVec3& sub1, const cjVec3& plus1, cjFLOAT prevLen, cjFLOAT nextLen)
{
    cjVec3 diff1 = m_value - sub1;
    cjVec3 diff2 = plus1 - m_value;

    cjFLOAT opc = 1.0f + m_continuity;
    cjFLOAT omc = 1.0f - m_continuity;
    cjFLOAT opb = 1.0f + m_bias;
    cjFLOAT omb = 1.0f - m_bias;
    cjFLOAT homt = 0.5f*(1.0f-m_tension);
    cjFLOAT homtopc = homt*opc;
    cjFLOAT homtomc = homt*omc;

    m_DS = (homtomc*opb)*diff1 + (homtopc*omb)*diff2;
    m_DD = (homtopc*opb)*diff1 + (homtomc*omb)*diff2;

    cjFLOAT inv = 2.0f/(prevLen + nextLen);

    m_DS *= prevLen*inv;
    m_DD *= nextLen*inv;
}


//
// cjAniKeyRot
//

CJ_ANIKEYOPER_IMPL(cjAniKeyRot, EKVAL_ROT, EKT_NOINTERP);
cjQuat cjAniKeyRot::ms_default = cjQuat::IDENTITY;


void cjAniKeyRot::RegisterCustomAniKeyOperation(EKEY_VALTYPE valType, EKEY_TYPE keyType)
{
    SetIsPosedKeyPF			(valType, keyType, cjNULL);
	SetEqualKeyPF			(valType, keyType, EqualKey);

    SetInsertKeyPF			(valType, keyType, cjNULL);

    SetCurvedRatioKeyPF		(valType, keyType, cjNULL);
    SetFillDerivedValsKeyPF	(valType, keyType, FillDerivedValsKey);
    SetCubicCoefsKeyPF		(valType, keyType, cjNULL);
	SetInterpKeyPF			(valType, keyType, InterpolateKey);
    SetInterpD1KeyPF		(valType, keyType, cjNULL);
    SetInterpD2KeyPF		(valType, keyType, cjNULL);
}

cjQuat cjAniKeyRot::GenInterp(cjFLOAT etime, cjAniKeyRot* keys, EKEY_TYPE keyType, 
	cjUINT32 keyCount, cjUINT32& lastIdx, cjBYTE siz)
{
    cjAssert(keys);
    cjAssert(keyCount != 0);

    if(keyCount == 1)
    {
        if(keyType != EKT_EULERKEY)
            return keys->GetKey(0, siz)->GetValue();
    }

	cjQuat rst;

    if(keyType == EKT_EULERKEY)
    {
        cjAniKeyRot::PFInterpolation interp = cjAniKeyRot::GetInterpKeyPF(keyType);
        cjAssert( interp );

        interp(etime, keys->GetKey(0, siz), 0, &rst);
        return rst;
    }

    cjUINT32 enKeyIndex = keyCount - 1;
    cjFLOAT lastTime = keys->GetKey(lastIdx, siz)->GetTime();

    if(etime < lastTime)
    {
        lastIdx = 0;
        lastTime = keys->GetKey(0, siz)->GetTime();
    }
    
    cjUINT32 curKeyIdx;
    cjFLOAT nextTime = 0.0f;
    for(curKeyIdx=lastIdx+1; curKeyIdx<=enKeyIndex; curKeyIdx++)
    {
        nextTime = keys->GetKey(curKeyIdx, siz)->GetTime();

        if(etime <= nextTime)
            break;

        lastIdx++;
        lastTime = nextTime;
    }

    cjAssert(curKeyIdx < keyCount);

    // interpolate the keys, requires that the time is normalized to [0,1]
    cjFLOAT ratio = (etime - lastTime) / (nextTime - lastTime);

    cjAniKeyRot::PFInterpolation interp = cjAniKeyRot::GetInterpKeyPF(keyType);
    cjAssert( interp );

    interp(ratio, keys->GetKey(lastIdx, siz), keys->GetKey(curKeyIdx, siz), &rst);
	
    return rst;
}

void cjAniKeyRot::GenInterp(cjFLOAT etime, cjAniKeyRot* keys, EKEY_TYPE keyType, cjUINT32 keyCount, 
	cjUINT32& lastIdx, cjBYTE siz, cjQuat* rst)
{
    cjAssert(keys);
    cjAssert(keyCount != 0);
    cjAssert(rst);

    if(keyCount > 1)
    {        
        cjAssert(keyType != EKT_EULERKEY); // only one key is allowed for Euler key.

        cjAniKeyRot* lastKey = keys->GetKey(lastIdx, siz);
        cjFLOAT lastTime = lastKey->GetTime();

        if(etime < lastTime)
        {
            lastIdx = 0;
            lastKey = keys->GetKey(0, siz);
            lastTime = lastKey->GetTime();
        }

        // The following loop always iterates at least once since 
        // lastIdx never reaches enKeyIndex.

        cjAniKeyRot* nextKey = cjNULL;
        cjFLOAT nextTime = 0.0f;
        cjUINT32 enKeyIndex = keyCount - 1;

        while(lastIdx < enKeyIndex)
        {
            nextKey = keys->GetKey(lastIdx+1, siz);
            nextTime = nextKey->GetTime();

            if(etime <= nextTime)
                break;

            lastKey = nextKey;
            lastTime = nextTime;
            lastIdx++;
        }

        cjAssert(lastIdx < enKeyIndex);

        cjFLOAT ratio = (etime - lastTime)/(nextTime - lastTime);

        cjAniKeyRot::PFInterpolation interp = cjAniKeyRot::GetInterpKeyPF(keyType);
        cjAssert(interp);

        interp(ratio, lastKey, nextKey, rst);
    }
    else if(keyType == EKT_EULERKEY)
    {
        cjAssert(keyCount == 1);

        cjAniKeyRot::PFInterpolation interp = cjAniKeyRot::GetInterpKeyPF(keyType);
        cjAssert(interp);

        interp(etime, keys->GetKey(0, siz), 0, rst);
    }
    else
    {
        *rst = keys->GetKey(0, siz)->GetValue();
    }
}

cjQuat cjAniKeyRot::GenInterpDefault(cjFLOAT rtime, cjAniKeyRot* keys, EKEY_TYPE keyType, 
	cjUINT32 keyCount, cjBYTE siz)
{
    if(keyCount)
    {
        if(rtime < keys->GetKey(0, siz)->GetTime())
		{
            return keys->GetKey(0, siz)->GetValue();
		}
        else if(rtime > keys->GetKey(keyCount-1, siz)->GetTime())
		{
            return keys->GetKey(keyCount-1, siz)->GetValue();
		}
        else
        {
            cjUINT32 lastIdx = 0;
            return cjAniKeyRot::GenInterp(rtime, keys, keyType, keyCount, lastIdx, siz);
        }
    }
    else
    {
        return ms_default;
    }
}

void cjAniKeyRot::InterpolateKey(cjFLOAT, const cjAniKey*, const cjAniKey*, void* rst)
{
    cjAssert(cjFalse);
	*(cjQuat*)rst = cjQuat(0, 0, 0, 1.0f);
}

cjBool cjAniKeyRot::EqualKey(const cjAniKey& k0, const cjAniKey& k1)
{
    if(!cjAniKey::EqualKey(k0, k1))
        return cjFalse;

    const cjAniKeyRot& v0 = (const cjAniKeyRot&) k0;
    const cjAniKeyRot& v1 = (const cjAniKeyRot&) k1;

    return v0.m_value == v1.m_value;
}

void cjAniKeyRot::FillDerivedValsKey(cjAniKey* keys, cjUINT32 keyCount, cjBYTE siz)
{
    cjUINT32 i;

    for(i=0; i<keyCount-1; i++)
    {
        cjAniKeyRot* k0 = (cjAniKeyRot*)keys->GetKey(i, siz);
        cjAniKeyRot* k1 = (cjAniKeyRot*)keys->GetKey(i+1, siz);

        if(cjQuat::Dot(k0->m_value, k1->m_value) < 0.0f)
            k1->m_value = -k1->m_value;
    }

    for(i=0; i<keyCount; i++)
    {
        cjAniKeyRot* cur = (cjAniKeyRot*)keys->GetKey(i, siz);
        cjFLOAT w = cur->m_value.w;

        if(w < -1.0f)
            cur->m_value.w = -1.0f;
        else if(w > 1.0f)
            cur->m_value.w = 1.0f;
    }
}

void cjAniKeyRot::GenCubicCoefs(cjFLOAT rtime, cjAniKeyRot* keys, EKEY_TYPE keyType, 
	cjUINT32 keyCount, cjUINT32& lastIdx, cjBYTE siz, cjFLOAT& valTime0, cjFLOAT& valTime1, 
    cjQuat& quat0, cjQuat& quat1, cjQuat& outQuat0, cjQuat& inQuat1)
{
    cjAssert(keyCount != 0);
    cjAssert(keyType != EKT_EULERKEY);

    if(keyCount == 1)
    {
        quat0		= keys->GetKey(0, siz)->GetValue();
        quat1		= quat0;
        outQuat0	= quat0;
        inQuat1		= quat0;

        valTime0	= 0.0f;
        valTime1	= CJ_FLT_MAX;
        lastIdx		= 0;

        return;
    }

    cjUINT32 enKeyIndex = keyCount - 1;
    cjUINT32 stkLastIdx = lastIdx;

    cjFLOAT lastTime = keys->GetKey(stkLastIdx, siz)->GetTime();
    if(rtime < lastTime)
    {
        stkLastIdx = 0;
        lastTime = keys->GetKey(0, siz)->GetTime();
    }

    cjUINT32 curKeyIdx;
    cjFLOAT nextTime = 0.0f;
    for(curKeyIdx=stkLastIdx+1; curKeyIdx<=enKeyIndex; curKeyIdx++)
    {
        nextTime = keys->GetKey(curKeyIdx, siz)->GetTime();

        if((rtime < nextTime) || (rtime == nextTime && keyType != EKT_STEPKEY))
            break;

        stkLastIdx++;
        lastTime = nextTime;
    }
    
    if(keyType == EKT_STEPKEY && rtime >= nextTime && curKeyIdx == keyCount)
    {
        quat0		= keys->GetKey(enKeyIndex, siz)->GetValue();
        quat1		= quat0;
        outQuat0	= quat0;
        inQuat1		= quat0;
        valTime0	= nextTime;
        valTime1	= nextTime;
        lastIdx		= enKeyIndex;
    }
    else
    {
        cjAssert(curKeyIdx < keyCount);
        cjAniKey::PFCubicCoefs cubiccoefs = cjAniKeyRot::GetCubicCoefsKeyPF(keyType);
        cjAssert(cubiccoefs);

        cubiccoefs(keys, keyCount, stkLastIdx, &quat0, &quat1, &outQuat0, &inQuat1);

        valTime0 = lastTime;
        valTime1 = nextTime;
        lastIdx = stkLastIdx;
    }
}


//
// cjAniKeyRot
//   - cjAniKeyLinRot
//

CJ_ANIKEYOPER_IMPL(cjAniKeyLinRot, EKVAL_ROT, EKT_LINKEY);


void cjAniKeyLinRot::RegisterCustomAniKeyOperation(EKEY_VALTYPE valType, EKEY_TYPE keyType)
{
	// Save/Load/Create/Delete/Copy is setted at CJ_ANIKEYOPER_IMPL

	SetIsPosedKeyPF			(valType, keyType, IsPosedKey);
	SetEqualKeyPF			(valType, keyType, EqualKey);

	SetInsertKeyPF			(valType, keyType, InsertKey);

    SetCurvedRatioKeyPF		(valType, keyType, cjNULL);
	SetFillDerivedValsKeyPF	(valType, keyType, FillDerivedValsKey);
	SetCubicCoefsKeyPF		(valType, keyType, CubicCoefsKey);

    SetInterpKeyPF			(valType, keyType, InterpolateKey);
    SetInterpD1KeyPF		(valType, keyType, cjNULL);
    SetInterpD2KeyPF		(valType, keyType, cjNULL);
}

void cjAniKeyLinRot::SaveKey(cjFStream& stream, cjAniKey* keys, cjUINT32 keyCount)
{
    cjAniKeyLinRot* linearRotKeys = (cjAniKeyLinRot*)keys;

    for(cjUINT32 i=0; i<keyCount; i++)
		stream << linearRotKeys[i];
}

cjAniKey* cjAniKeyLinRot::LoadKey(cjFStream& stream, cjUINT32 keyCount)
{
    cjAniKeyLinRot* linearRotKeys = new cjAniKeyLinRot[keyCount];
	
	cjAssert(stream.IsLoading());

    for(cjUINT32 i=0; i<keyCount; i++)
		stream << linearRotKeys[i];

	return linearRotKeys;
}

cjAniKey* cjAniKeyLinRot::CreateKey(cjUINT32 keyCount)
{
    return new cjAniKeyLinRot[keyCount];
}

void cjAniKeyLinRot::DeleteKey(cjAniKey* keys)
{
    cjAniKeyLinRot* linearRotKeys = (cjAniKeyLinRot*)keys;
    delete [] linearRotKeys;
}

void cjAniKeyLinRot::CopyKey(cjAniKey* dstKey, const cjAniKey* srcKey)
{
    cjAniKeyLinRot* dstRot = (cjAniKeyLinRot*)dstKey;
    cjAniKeyLinRot* srcRot = (cjAniKeyLinRot*)srcKey;

    dstRot->m_time = srcRot->m_time;
    dstRot->m_value = srcRot->m_value;
}

cjBool cjAniKeyLinRot::IsPosedKey(const cjAniKey* keys, cjUINT32 keyCount)
{
    cjAssert(keys);
    cjAssert(keyCount > 0);

    if(keyCount > 1)
    {
        const cjAniKeyLinRot* linearRotKeys = (const cjAniKeyLinRot*)keys;
        cjQuat v = linearRotKeys[0].m_value;

        for(cjUINT32 i=1; i<keyCount; i++)
        {
            if(linearRotKeys[i].m_value != v)
                return cjFalse;
        }
    }

    return cjTrue;
}

cjBool cjAniKeyLinRot::InsertKey(cjFLOAT etime, cjAniKey*& keys, cjUINT32& keyCount)
{
    cjAniKeyLinRot* linearRotKeys = (cjAniKeyLinRot*)keys;

    cjUINT32 dstSiz;
    cjUINT32 insIndex;

    if(TestKeyInsertable(etime, linearRotKeys, keyCount, sizeof(cjAniKeyLinRot), insIndex))
    {
        cjAniKeyLinRot* newKeys = new cjAniKeyLinRot[keyCount + 1];
        dstSiz = (keyCount + 1) * sizeof(cjAniKeyLinRot);

        cjMemCpy(newKeys, dstSiz, linearRotKeys, insIndex * sizeof(cjAniKeyLinRot));

        if(keyCount > insIndex)
        {
            dstSiz = (keyCount - insIndex) * sizeof(cjAniKeyLinRot);
            cjMemCpy(&newKeys[insIndex + 1], &linearRotKeys[insIndex], dstSiz);
        }

        cjQuat v = cjAniKeyRot::GenInterpDefault(etime, linearRotKeys, EKT_LINKEY, keyCount, sizeof(cjAniKeyLinRot));
        cjAniKeyLinRot* newKey = &newKeys[insIndex];

        newKey->m_time = etime;
        newKey->m_value = v;

        keyCount++;
        delete [] linearRotKeys;

        keys = newKeys;
        FillDerivedValsKey(keys, keyCount, sizeof(cjAniKeyLinRot));

        return cjTrue;
    }

	return cjFalse;
}

void cjAniKeyLinRot::CubicCoefsKey(const cjAniKey* keys, cjUINT32 keyCount, cjUINT32 index, 
	void* quat0, void* quat1, void* outQuat0, void* inQuat1)
{
    cjAssert(index < keyCount - 1);
    const cjAniKeyLinRot* linearRotKeys = (const cjAniKeyLinRot*)keys;

    *(cjQuat*)quat0		= linearRotKeys[index]		.m_value;
    *(cjQuat*)quat1		= linearRotKeys[index + 1]	.m_value;
    *(cjQuat*)outQuat0	= linearRotKeys[index]		.m_value;
    *(cjQuat*)inQuat1	= linearRotKeys[index + 1]	.m_value;
}

void cjAniKeyLinRot::InterpolateKey(cjFLOAT rtime, const cjAniKey* key0, const cjAniKey* key1, void* rst)
{
    cjAniKeyRot* rotKey0 = (cjAniKeyRot*)key0;
    cjAniKeyRot* rotKey1 = (cjAniKeyRot*)key1;

    cjQuat::Slerp(rtime, rotKey0->m_value, rotKey1->m_value, (cjQuat*)rst);
}


//
// cjAniKeyRot
//   - cjAniKeyBezRot
//

CJ_ANIKEYOPER_IMPL(cjAniKeyBezRot, EKVAL_ROT, EKT_BEZKEY);


void cjAniKeyBezRot::RegisterCustomAniKeyOperation(EKEY_VALTYPE valType, EKEY_TYPE keyType)
{
	SetIsPosedKeyPF			(valType, keyType, IsPosedKey);
	SetEqualKeyPF			(valType, keyType, EqualKey);

	SetInsertKeyPF			(valType, keyType, InsertKey);

    SetCurvedRatioKeyPF		(valType, keyType, cjNULL);
	SetFillDerivedValsKeyPF	(valType, keyType, FillDerivedValsKey);
	SetCubicCoefsKeyPF		(valType, keyType, CubicCoefsKey);

    SetInterpKeyPF			(valType, keyType, InterpolateKey);
    SetInterpD1KeyPF		(valType, keyType, cjNULL);
    SetInterpD2KeyPF		(valType, keyType, cjNULL);
}

void cjAniKeyBezRot::SaveKey(cjFStream& stream, cjAniKey* keys, cjUINT32 keyCount)
{
    cjAniKeyBezRot* k = (cjAniKeyBezRot*)keys;

    for(cjUINT32 i=0; i<keyCount; i++)
		stream << k[i];
}

cjAniKey* cjAniKeyBezRot::LoadKey(cjFStream& stream, cjUINT32 keyCount)
{
    cjAniKeyBezRot* k = new cjAniKeyBezRot[keyCount];
	
	cjAssert(stream.IsLoading());

    for(cjUINT32 i=0; i<keyCount; i++)
		stream << k[i];

	return k;
}

cjAniKey* cjAniKeyBezRot::CreateKey(cjUINT32 keyCount)
{
    return new cjAniKeyBezRot[keyCount];
}

void cjAniKeyBezRot::DeleteKey(cjAniKey* keys)
{
    cjAniKeyBezRot* k = (cjAniKeyBezRot*)keys;
    delete [] k;
}

void cjAniKeyBezRot::CopyKey(cjAniKey* dstKey, const cjAniKey* srcKey)
{
    cjAniKeyBezRot* dst = (cjAniKeyBezRot*)dstKey;
    cjAniKeyBezRot* src = (cjAniKeyBezRot*)srcKey;

    dst->m_time = src->m_time;
	dst->m_value = src->m_value;

	dst->m_intermediateQuat = src->m_intermediateQuat;

}

cjBool cjAniKeyBezRot::IsPosedKey(const cjAniKey* keys, cjUINT32 keyCount)
{
    cjAssert(keys);
    cjAssert(keyCount > 0);

    if(keyCount > 1)
    {
        const cjAniKeyBezRot* bezKeys = (const cjAniKeyBezRot*)keys;
        cjQuat value = bezKeys[0].m_value;

        for(cjUINT32 i=1; i<keyCount; i++)
        {
            if(bezKeys[i].m_value != value)
                return cjFalse;
        }
    }

    return cjTrue;
}

cjBool cjAniKeyBezRot::EqualKey(const cjAniKey& key0, const cjAniKey& key1)
{
    return cjAniKeyRot::EqualKey(key0, key1);
}

cjBool cjAniKeyBezRot::InsertKey(cjFLOAT etime, cjAniKey*& keys, cjUINT32& keyCount)
{
    cjAniKeyBezRot* bezRotKeys = (cjAniKeyBezRot*) keys;

    cjUINT32 dstSiz;
    cjUINT32 insIndex;

    if(TestKeyInsertable(etime, bezRotKeys, keyCount, sizeof(cjAniKeyBezRot), insIndex))
    {
        cjAniKeyBezRot* newKeys = new cjAniKeyBezRot[keyCount + 1];
        dstSiz = (keyCount + 1) * sizeof(cjAniKeyBezRot);

        cjMemCpy(newKeys, dstSiz, bezRotKeys, insIndex * sizeof(cjAniKeyBezRot));

        if(keyCount > insIndex)
        {
            dstSiz = (keyCount - insIndex) * sizeof(cjAniKeyBezRot);
            cjMemCpy(&newKeys[insIndex + 1], &bezRotKeys[insIndex], dstSiz);
        }

        cjQuat value = cjAniKeyRot::GenInterpDefault(etime, bezRotKeys, EKT_BEZKEY, keyCount, sizeof(cjAniKeyBezRot));

        cjAniKeyBezRot* newKey = &newKeys[insIndex];
        newKey->m_time = etime;
        newKey->m_value = value;

        keyCount++;

        delete [] bezRotKeys;
        keys = newKeys;

        FillDerivedValsKey(keys, keyCount, sizeof(cjAniKeyBezRot));
        
		return cjTrue;
    }

	return cjFalse;
}

void cjAniKeyBezRot::CubicCoefsKey(const cjAniKey* keys, cjUINT32 keyCount, cjUINT32 index, 
	void* quat0, void* quat1, void* outQuat0, void* inQuat1)
{
    cjAssert(keyCount >= 2);
    cjAssert(index < keyCount - 1);

    const cjAniKeyBezRot* bezKeys = (const cjAniKeyBezRot*)keys;

	*(cjQuat*)quat0 = bezKeys[index].m_value;
    *(cjQuat*)outQuat0 = bezKeys[index].m_intermediateQuat;
    *(cjQuat*)quat1 = bezKeys[index + 1].m_value;
    *(cjQuat*)inQuat1 = bezKeys[index + 1].m_intermediateQuat;
}

void cjAniKeyBezRot::InterpolateKey(cjFLOAT etime, const cjAniKey* key0, const cjAniKey* key1, void* rst)
{
    cjAniKeyBezRot* bez0 = (cjAniKeyBezRot*)key0;
    cjAniKeyBezRot* bez1 = (cjAniKeyBezRot*)key1;

    *(cjQuat*)rst = cjQuat::Squad(etime, bez0->m_value, bez0->m_intermediateQuat, bez1->m_intermediateQuat, bez1->m_value);
}

void cjAniKeyBezRot::FillDerivedValsKey(cjAniKey* keys, cjUINT32 keyCount, cjBYTE keySiz)
{
    if(keyCount >= 2)
    {
        cjAniKeyRot::FillDerivedValsKey(keys, keyCount, keySiz);
        cjAniKeyBezRot* bezRotKeys = (cjAniKeyBezRot*)keys;

        bezRotKeys[0].m_intermediateQuat = cjQuat::Intermediate(bezRotKeys[0].m_value, bezRotKeys[0].m_value, bezRotKeys[1].m_value);

        cjUINT32 keyCount1 = keyCount - 1;

        for(cjUINT32 i=1; i<keyCount1; i++)
            bezRotKeys[i].m_intermediateQuat = cjQuat::Intermediate(bezRotKeys[i-1].m_value, bezRotKeys[i].m_value, bezRotKeys[i+1].m_value);

        bezRotKeys[keyCount1].m_intermediateQuat = cjQuat::Intermediate(bezRotKeys[keyCount-2].m_value, bezRotKeys[keyCount1].m_value, bezRotKeys[keyCount1].m_value);
    }
}


//
// cjAniKeyRot
//   - cjAniKeyTCBRot
//

CJ_ANIKEYOPER_IMPL(cjAniKeyTCBRot, EKVAL_ROT, EKT_TCBKEY);


void cjAniKeyTCBRot::RegisterCustomAniKeyOperation(EKEY_VALTYPE valType, EKEY_TYPE keyType)
{
	SetIsPosedKeyPF			(valType, keyType, IsPosedKey);
	SetEqualKeyPF			(valType, keyType, EqualKey);

	SetInsertKeyPF			(valType, keyType, InsertKey);

    SetCurvedRatioKeyPF		(valType, keyType, cjNULL);
	SetFillDerivedValsKeyPF	(valType, keyType, FillDerivedValsKey);
	SetCubicCoefsKeyPF		(valType, keyType, CubicCoefsKey);

    SetInterpKeyPF			(valType, keyType, InterpolateKey);
    SetInterpD1KeyPF		(valType, keyType, cjNULL);
    SetInterpD2KeyPF		(valType, keyType, cjNULL);
}

void cjAniKeyTCBRot::SaveKey(cjFStream& stream, cjAniKey* keys, cjUINT32 keyCount)
{
    cjAniKeyTCBRot* k = (cjAniKeyTCBRot*)keys;

    for(cjUINT32 i=0; i<keyCount; i++)
		stream << k[i];
}

cjAniKey* cjAniKeyTCBRot::LoadKey(cjFStream& stream, cjUINT32 keyCount)
{
    cjAniKeyTCBRot* k = new cjAniKeyTCBRot[keyCount];
	
	cjAssert(stream.IsLoading());

    for(cjUINT32 i=0; i<keyCount; i++)
		stream << k[i];

	return k;
}

cjAniKey* cjAniKeyTCBRot::CreateKey(cjUINT32 keyCount)
{
    return new cjAniKeyTCBRot[keyCount];
}

void cjAniKeyTCBRot::DeleteKey(cjAniKey* keys)
{
    cjAniKeyTCBRot* k = (cjAniKeyTCBRot*)keys;
    delete [] k;
}

void cjAniKeyTCBRot::CopyKey(cjAniKey* dstKey, const cjAniKey* srcKey)
{
    cjAniKeyTCBRot* dst = (cjAniKeyTCBRot*)dstKey;
    cjAniKeyTCBRot* src = (cjAniKeyTCBRot*)srcKey;

    dst->m_time = src->m_time;
    dst->m_value = src->m_value;

    dst->m_tension = src->m_tension;
    dst->m_continuity = src->m_continuity;
    dst->m_bias = src->m_bias;

    dst->m_a = src->m_a;
    dst->m_b = src->m_b;
}

cjBool cjAniKeyTCBRot::IsPosedKey(const cjAniKey* keys, cjUINT32 keyCount)
{
    cjAssert(keys);
    cjAssert(keyCount >= 2);

    if(keyCount > 1)
    {
        const cjAniKeyTCBRot* tcbKeys = (const cjAniKeyTCBRot*)keys;

		cjQuat value = tcbKeys[0].m_value;

        for(cjUINT32 i = 0; i < keyCount; i++)
        {
            // Check for identical values and intermediate values.
            if(tcbKeys[i].m_value != value || tcbKeys[i].m_a != value ||  tcbKeys[i].m_b != value)
                return cjFalse;
        }
    }

    return cjTrue;
}

cjBool cjAniKeyTCBRot::EqualKey(const cjAniKey& key0, const cjAniKey& key1)
{
    if(!cjAniKeyRot::EqualKey(key0, key1))
        return cjFalse;

    const cjAniKeyTCBRot& tcb0 = (const cjAniKeyTCBRot&)key0;
    const cjAniKeyTCBRot& tcb1 = (const cjAniKeyTCBRot&)key1;

    return tcb0.m_tension == tcb1.m_tension && tcb0.m_continuity == tcb1.m_continuity && tcb0.m_bias == tcb1.m_bias;
}

cjBool cjAniKeyTCBRot::InsertKey(cjFLOAT etime, cjAniKey*& keys, cjUINT32& keyCount)
{
    cjAniKeyTCBRot* tcbRotKeys = (cjAniKeyTCBRot*)keys;

    cjUINT32 dstSiz;
    cjUINT32 insIndex;

    if(TestKeyInsertable(etime, tcbRotKeys, keyCount, sizeof(cjAniKeyTCBRot), insIndex))
    {
        cjAniKeyTCBRot* newKeys = new cjAniKeyTCBRot[keyCount + 1];

        dstSiz = (keyCount + 1) * sizeof(cjAniKeyTCBRot);
        cjMemCpy(newKeys, dstSiz, tcbRotKeys, insIndex * sizeof(cjAniKeyTCBRot));

        if(keyCount > insIndex)
        {
            dstSiz = (keyCount - insIndex) * sizeof(cjAniKeyTCBRot);
            cjMemCpy(&newKeys[insIndex + 1], &tcbRotKeys[insIndex], dstSiz);
        }

        cjQuat value = cjAniKeyRot::GenInterpDefault(etime, tcbRotKeys,
            EKT_TCBKEY, keyCount, sizeof(cjAniKeyTCBRot));

        cjAniKeyTCBRot* newKey = &newKeys[insIndex];
        newKey->m_time = etime;
        newKey->m_value = value;

        newKey->m_tension = 0.0f;
        newKey->m_continuity = 0.0f;
        newKey->m_bias = 0.0f;

        keyCount++;
        
		delete [] tcbRotKeys;
        keys = newKeys;

        FillDerivedValsKey(keys, keyCount, sizeof(cjAniKeyTCBRot));

        return cjTrue;
    }

	return cjFalse;
}

void cjAniKeyTCBRot::FillDerivedValsKey(cjAniKey* keys, cjUINT32 keyCount, cjBYTE ucSize)
{
    if( keyCount >= 2 )
    {
        cjAniKeyRot::FillDerivedValsKey(keys, keyCount, ucSize);
        cjAniKeyTCBRot* tcbRotKeys = (cjAniKeyTCBRot*)keys;

        tcbRotKeys[0].CalculateDiffVals(&tcbRotKeys[0], &tcbRotKeys[1]);

        cjUINT32 keyCount1 = keyCount-1;

        for(cjUINT32 i=1; i<keyCount1; i++)
            tcbRotKeys[i].CalculateDiffVals(&tcbRotKeys[i-1], &tcbRotKeys[i+1]);

        tcbRotKeys[keyCount1].CalculateDiffVals(&tcbRotKeys[keyCount-2], &tcbRotKeys[keyCount1]);
    }
}

void cjAniKeyTCBRot::CubicCoefsKey(const cjAniKey* keys, cjUINT32 keyCount, cjUINT32 index, 
	void* quat0, void* quat1, void* outQuat0, void* inQuat1)
{
    cjAssert(keyCount >= 2);
    cjAssert(index < keyCount - 1);

    const cjAniKeyTCBRot* tcbKeys = (const cjAniKeyTCBRot*)keys;

    *(cjQuat*)quat0 = tcbKeys[index].m_value;
    *(cjQuat*)quat1 = tcbKeys[index + 1].m_value;
    *(cjQuat*)outQuat0 = tcbKeys[index].m_a;
    *(cjQuat*)inQuat1 = tcbKeys[index + 1].m_b;
}

void cjAniKeyTCBRot::InterpolateKey(cjFLOAT etime, const cjAniKey* key0, const cjAniKey* key1, void* rst)
{
    cjAniKeyTCBRot* tcbA = (cjAniKeyTCBRot*)key0;
    cjAniKeyTCBRot* tcbB = (cjAniKeyTCBRot*)key1;

    *(cjQuat*)rst = cjQuat::Squad(etime, tcbA->m_value, tcbA->m_a, tcbB->m_b, tcbB->m_value);
}

void cjAniKeyTCBRot::CalculateDiffVals(const cjAniKeyTCBRot* prev, const cjAniKeyTCBRot* next)
{
    cjQuat logPrev = cjQuat::Log(cjQuat::InverseAxis(prev->m_value) * m_value);
    cjQuat logNext = cjQuat::Log(cjQuat::InverseAxis(m_value) * next->m_value);

    cjFLOAT inv = 1.0f/(next->m_time - prev->m_time);
    cjFLOAT mt = 1.0f - m_tension;
    cjFLOAT mc = 1.0f - m_continuity;
    cjFLOAT pc = 1.0f + m_continuity;
    cjFLOAT mb = 1.0f - m_bias;
    cjFLOAT pb = 1.0f + m_bias;

    cjFLOAT adjVal = (m_time - prev->m_time)* inv;

    cjFLOAT d0 = adjVal * mt * pc * pb;
    cjFLOAT d1 = adjVal * mt * mc * mb;

    cjQuat outTan = logNext * d1 + logPrev * d0;
    m_a = m_value * cjQuat::Exp((outTan - logNext) * 0.5f);
    adjVal = (next->m_time - m_time) * inv;

    d0 = adjVal * mt * mc * pb;
    d1 = adjVal * mt * pc * mb;

    cjQuat inTan = logNext * d1 + logPrev * d0;
    m_b = m_value * cjQuat::Exp((logPrev - inTan) * 0.5f);
}


//
// cjAniKeyRot
//   - cjAniKeyEulerRot
//

CJ_ANIKEYOPER_IMPL(cjAniKeyEulerRot, EKVAL_ROT, EKT_EULERKEY);


void cjAniKeyEulerRot::RegisterCustomAniKeyOperation(EKEY_VALTYPE keyValType, EKEY_TYPE keyType)
{
	SetIsPosedKeyPF		(keyValType, keyType,	IsPosedKey);
    SetEqualKeyPF		(keyValType, keyType,	EqualKey);

    SetInsertKeyPF		(keyValType, keyType,	InsertKey);   
	
    SetCurvedRatioKeyPF	(keyValType, keyType,	cjNULL);
	SetFillDerivedValsKeyPF(keyValType, keyType, FillDerivedValsKey);
	SetCubicCoefsKeyPF	(keyValType, keyType,	cjNULL);
	SetInterpKeyPF		(keyValType, keyType,	InterpolateKey);
    SetInterpD1KeyPF	(keyValType, keyType,	cjNULL);
    SetInterpD2KeyPF	(keyValType, keyType,	cjNULL);
}

cjAniKeyEulerRot::cjAniKeyEulerRot()
{
    m_keyCount[0] = 0; m_keyCount[1] = 0; m_keyCount[2] = 0;
    m_keyType[0] = cjAniKey::EKT_NOINTERP; 
	m_keyType[1] = cjAniKey::EKT_NOINTERP; 
	m_keyType[2] = cjAniKey::EKT_NOINTERP;
    m_keySiz[0] = 0; m_keySiz[1] = 0; m_keySiz[2] = 0;

	m_keys[0] = 0; m_keys[1] = 0; m_keys[2] = 0;
    m_lastIdx[0] = 0; m_lastIdx[1] = 0; m_lastIdx[2] = 0;
}

cjAniKeyEulerRot::cjAniKeyEulerRot(
	cjAniKeyFloat* keysX, cjUINT32 keyCountX, cjAniKeyFloat::EKEY_TYPE keyTypeX,
	cjAniKeyFloat* keysY, cjUINT32 keyCountY, cjAniKeyFloat::EKEY_TYPE keyTypeY, 
	cjAniKeyFloat* keysZ, cjUINT32 keyCountZ, cjAniKeyFloat::EKEY_TYPE keyTypeZ)
{
    m_keys[0] = keysX;	m_keyCount[0] = keyCountX;	m_keyType[0] = keyTypeX;
    m_keys[1] = keysY;	m_keyCount[1] = keyCountY;	m_keyType[1] = keyTypeY;
    m_keys[2] = keysZ;	m_keyCount[2] = keyCountZ;	m_keyType[2] = keyTypeZ;
    m_lastIdx[0] = 0; m_lastIdx[1] = 0; m_lastIdx[2] = 0;

    for(cjUINT32 i=0; i<3; i++)
    {
        if(m_keyCount[i])
            m_keySiz[i] = cjAniKeyFloat::GetKeySiz(m_keyType[i]);
        else
            m_keySiz[i] = 0;
    }
}

void cjAniKeyEulerRot::Initialize(
	cjAniKeyFloat* keysX, cjUINT32 keyCountX, cjAniKeyFloat::EKEY_TYPE keyTypeX,
	cjAniKeyFloat* keysY, cjUINT32 keyCountY, cjAniKeyFloat::EKEY_TYPE keyTypeY, 
	cjAniKeyFloat* keysZ, cjUINT32 keyCountZ, cjAniKeyFloat::EKEY_TYPE keyTypeZ)
{
    CleanUp();

    m_keys[0] = keysX;	m_keyCount[0] = keyCountX;	m_keyType[0] = keyTypeX;
    m_keys[1] = keysY;	m_keyCount[1] = keyCountY;	m_keyType[1] = keyTypeY;
    m_keys[2] = keysZ;	m_keyCount[2] = keyCountZ;	m_keyType[2] = keyTypeZ;
    m_lastIdx[0] = 0;	m_lastIdx[1] = 0;	m_lastIdx[2] = 0;
    
    for(cjUINT32 i=0; i<3; i++)
    {
        if(m_keyCount[i])
            m_keySiz[i] = cjAniKeyFloat::GetKeySiz(m_keyType[i]);
        else
            m_keySiz[i] = 0;
    }
}

void cjAniKeyEulerRot::CleanUp()
{
    for(cjINT32 i=0; i<3; i++)
    {
        if(m_keys[i])
        {
            cjAniKeyFloat::PFDelete pfnDeleteKey = cjAniKeyFloat::GetDeleteKeyPF(m_keyType[i]);
            cjAssert(pfnDeleteKey);
            pfnDeleteKey(m_keys[i]);
        }

        m_keys[i] = 0;
        m_keyCount[i] = 0;
        m_keyType[i] = cjAniKey::EKT_NOINTERP;
    }
}

void cjAniKeyEulerRot::ReplaceKeys(cjBYTE index, cjUINT32 keyCount, cjAniKeyFloat* keys)
{
    cjAssert(index <= 2);

    if(m_keys[index]) // delete previous
    {
        cjAniKeyFloat::PFDelete pfnDeleteKey = cjAniKeyFloat::GetDeleteKeyPF(m_keyType[index]);
        cjAssert(pfnDeleteKey);
        pfnDeleteKey(m_keys[index]);
    }

    SetKeys(index, keys);
    SetKeyCount(index, keyCount);
}

void cjAniKeyEulerRot::SaveKey(cjFStream& stream, cjAniKey* keys, cjUINT32 keyCount)
{
    cjAniKeyEulerRot* k = (cjAniKeyEulerRot*)keys;

    for(cjUINT32 i=0; i<keyCount; i++)
		stream << k[i];
}

cjAniKey* cjAniKeyEulerRot::LoadKey(cjFStream& stream, cjUINT32 keyCount)
{
    cjAniKeyEulerRot* k = new cjAniKeyEulerRot[keyCount];
	
	cjAssert(stream.IsLoading());

    for(cjUINT32 i=0; i<keyCount; i++)
		stream << k[i];

	return k;
}

cjAniKey* cjAniKeyEulerRot::CreateKey(cjUINT32 keyCount)
{
    return new cjAniKeyEulerRot[keyCount];
}

void cjAniKeyEulerRot::DeleteKey(cjAniKey* keys)
{
    cjAniKeyEulerRot* k = (cjAniKeyEulerRot*)keys;
    delete [] k;
}

void cjAniKeyEulerRot::CopyKey(cjAniKey* newKey, const cjAniKey* oriKey)
{
    cjAniKeyEulerRot* eulerKeyNew = (cjAniKeyEulerRot*)newKey;
    cjAniKeyEulerRot* eulerKeyOrig = (cjAniKeyEulerRot*)oriKey;
	    
    for(cjINT32 i=0; i<3; i++)
    {
        eulerKeyNew->m_keyCount[i]	= eulerKeyOrig->m_keyCount[i];
        eulerKeyNew->m_keyType[i]	= eulerKeyOrig->m_keyType[i];
        eulerKeyNew->m_keySiz[i]	= eulerKeyOrig->m_keySiz[i];

        cjUINT32 keyCount = eulerKeyOrig->m_keyCount[i];

        if(keyCount)
        {
            cjAniKeyFloat::PFCreate pfnCreateKey = cjAniKeyFloat::GetCreateKeyPF(eulerKeyOrig->m_keyType[i]);
            cjAssert(pfnCreateKey);

            eulerKeyNew->m_keys[i] = (cjAniKeyFloat*)pfnCreateKey(keyCount);
            cjAssert(eulerKeyNew->m_keys[i]);

            cjAniKeyFloat::PFCopy pfnCopyKey = cjAniKeyFloat::GetCopyKeyPF(eulerKeyOrig->m_keyType[i]);
            cjAssert(pfnCopyKey);

            for(cjUINT32 k=0; k<keyCount; k++)
            {
                pfnCopyKey(eulerKeyNew->m_keys[i]->GetKey(k, eulerKeyNew->m_keySiz[i]),
						eulerKeyOrig->m_keys[i]->GetKey(k, eulerKeyOrig->m_keySiz[i]));
            }
        }
    }
}

cjBool cjAniKeyEulerRot::IsPosedKey(const cjAniKey* keys, cjUINT32 keyCount)
{
    cjAssert(keyCount == 1);

    cjAniKeyEulerRot* eulerKey = (cjAniKeyEulerRot*)keys->GetKey(0, sizeof(cjAniKeyEulerRot));
    cjAssert(eulerKey);

    for(cjBYTE uc=0; uc < 3; uc++)
    {
        cjUINT32 floatKeyCount = eulerKey->GetKeyCount(uc);

        if(floatKeyCount > 0)
        {
            cjAniKeyFloat::EKEY_TYPE keyType = eulerKey->GetType(uc);

            PFIsPosed pfnIsPosedKey = cjAniKeyFloat::GetIsPosedKeyPF(keyType);
            cjAssert(pfnIsPosedKey);

            cjAniKeyFloat* floatKeys = eulerKey->GetKeys(uc);

            if(!pfnIsPosedKey(floatKeys, floatKeyCount))
                return cjFalse;
        }
    }

    return cjTrue;
}

cjBool cjAniKeyEulerRot::EqualKey(const cjAniKey& key0, const cjAniKey& key1)
{
    const cjAniKeyEulerRot& euler0 = (const cjAniKeyEulerRot&)key0;
    const cjAniKeyEulerRot& euler1 = (const cjAniKeyEulerRot&)key1;

    for(cjUINT32 i=0; i<3; i++)
    {
        if( euler0.m_keyCount[i] != euler1.m_keyCount[i] ||
			euler0.m_keyType[i] != euler1.m_keyType[i] ||
            euler0.m_keySiz[i] != euler1.m_keySiz[i])
        {
            return cjFalse;
        }

        if(euler0.m_keyCount[i] > 0)
        {
            cjAniKeyFloat::PFEqual pfnEqualKey = cjAniKeyFloat::GetEqualKeyPF(euler0.m_keyType[i]);
            cjAssert(pfnEqualKey);

            for(cjUINT32 k=0; k<euler0.m_keyCount[i]; k++)
            {
                if(!pfnEqualKey(*euler0.m_keys[i]->GetKey(k, euler0.m_keySiz[i]), 
								*euler1.m_keys[i]->GetKey(k, euler1.m_keySiz[i])))
                    return cjFalse;
            }
        }
    }

    return cjTrue;
}

cjBool cjAniKeyEulerRot::InsertKey(cjFLOAT etime, cjAniKey*& keys, cjUINT32& keyCount)
{
    if(keyCount == 0)
    {
        keys = new cjAniKeyEulerRot[1];
        keyCount = 1;
    }

    cjAniKeyEulerRot* eulerKey = (cjAniKeyEulerRot*)keys->GetKey(0, sizeof(cjAniKeyEulerRot));
    cjBool insertedKey = cjFalse;

    for(cjUINT32 i=0; i<3; i++)
    {
        cjAniKeyFloat*			floatKeys;
        cjUINT32				floatKeyCount;
        cjAniKey::EKEY_TYPE		floatKeyType;

        floatKeys = eulerKey->GetKeys((cjBYTE)i);
        floatKeyCount = eulerKey->GetKeyCount((cjBYTE)i);

        if(!floatKeyCount)
            continue;

        floatKeyType = eulerKey->GetType((cjBYTE)i);

        PFInsert pfnInsertKey = cjAniKey::GetInsertKeyPF(EKVAL_FLOAT, floatKeyType);
        cjAssert(pfnInsertKey);

        cjAniKey* aniKey = (cjAniKey*)floatKeys;

        if(pfnInsertKey(etime, aniKey, floatKeyCount))
            insertedKey = cjTrue;

        floatKeys = (cjAniKeyFloat*)aniKey;

        eulerKey->SetKeyCount((cjBYTE)i, floatKeyCount);
        eulerKey->SetKeys((cjBYTE)i, floatKeys);
    }

    if(insertedKey)
        return cjTrue;

	return cjFalse;
}

void cjAniKeyEulerRot::FillDerivedValsKey(cjAniKey* keys, cjUINT32, cjBYTE keySiz)
{
    cjAniKeyEulerRot* eulerKey = (cjAniKeyEulerRot*)keys->GetKey(0, keySiz);

    for(cjUINT32 i=0; i<3; i++)
    {
        if(eulerKey->m_keys[i])
        {
            cjAniKeyFloat::PFFillDerivedVals pfnDerivedKey
				= cjAniKeyFloat::GetFillDerivedValsKeyPF(eulerKey->m_keyType[i]);
            
			cjAssert(pfnDerivedKey);
            pfnDerivedKey(eulerKey->m_keys[i], eulerKey->m_keyCount[i], eulerKey->m_keySiz[i]);
        }
    }
}

void cjAniKeyEulerRot::InterpolateKey(cjFLOAT rtime, const cjAniKey* keys0, const cjAniKey*, void* rst)
{
    cjAniKeyEulerRot* euler = (cjAniKeyEulerRot*)keys0;
    cjFLOAT angles[3];

	for(cjINT32 i=0; i<3; i++)
    {
        if(euler->m_keyCount[i])
        {
            angles[i] = cjAniKeyFloat::GenInterp(rtime, 
				euler->m_keys[i],
				euler->m_keyType[i], 
				euler->m_keyCount[i], 
				euler->m_lastIdx[i], 
				euler->m_keySiz[i]);
        }
        else 
			angles[i] = 0.0f;
    }

    ((cjQuat*)rst)->MakeFrom3AxisAngle(angles[0], angles[1], angles[2]);
}


//
// cjAniKeyColor
//

CJ_ANIKEYOPER_IMPL(cjAniKeyColor, EKVAL_COLOR, EKT_NOINTERP);

cjColorF cjAniKeyColor::ms_default = cjColorF::BLACK;

void cjAniKeyColor::RegisterCustomAniKeyOperation(EKEY_VALTYPE valType, EKEY_TYPE keyType)
{
	SetIsPosedKeyPF		(valType, keyType, cjNULL);
	SetEqualKeyPF		(valType, keyType, EqualKey);

	SetInsertKeyPF		(valType, keyType, cjNULL);

	SetCurvedRatioKeyPF	(valType, keyType, cjNULL);
    SetFillDerivedValsKeyPF(valType, keyType, cjNULL);
	SetCubicCoefsKeyPF	(valType, keyType, cjNULL);
	SetInterpKeyPF		(valType, keyType, InterpolateKey);
    SetInterpD1KeyPF	(valType, keyType, cjNULL);
    SetInterpD2KeyPF	(valType, keyType, cjNULL);
}

cjAniKeyColor::cjAniKeyColor()
{
}

cjAniKeyColor::cjAniKeyColor(cjFLOAT etime, const cjColorF& col) : cjAniKey(etime) 
{ 
	m_color = col; 
}

cjAniKeyColor::~cjAniKeyColor()
{
}

cjColorF cjAniKeyColor::GenInterp(cjFLOAT rtime, cjAniKeyColor* keys,
    cjAniKeyColor::EKEY_TYPE keyType, cjUINT32 keyCount, cjUINT32& lastIdx, cjBYTE siz)
{
    cjAssert(keyCount != 0);

    if(keyCount == 1)
        return keys->GetKey(0, siz)->GetValue();

    cjUINT32 enKeyIndex = keyCount - 1;
	cjFLOAT lastTime = keys->GetKey(lastIdx, siz)->GetTime();

    if(rtime < lastTime)
    {
        lastIdx = 0;
        lastTime = keys->GetKey(0, siz)->GetTime();
    }

    cjUINT32 curKeyIdx;
    cjFLOAT nextTime = 0.0f;

    for(curKeyIdx=lastIdx+1; curKeyIdx<=enKeyIndex; curKeyIdx++)
    {
        nextTime = keys->GetKey(curKeyIdx, siz)->GetTime();

        if(rtime <= nextTime)
            break;

        lastIdx++;
        lastTime = nextTime;
    }

    cjAssert(curKeyIdx < keyCount);
    cjFLOAT stimeCur = (rtime - lastTime) / (nextTime - lastTime);

    cjAniKeyColor::PFInterpolation interp = cjAniKeyColor::GetInterpKeyPF(keyType);
    cjAssert(interp);

    cjColorF rst;
    interp(stimeCur, keys->GetKey(lastIdx, siz), keys->GetKey(curKeyIdx, siz), &rst);

    return rst;
}

void cjAniKeyColor::GenCubicCoefs(cjFLOAT etime, cjAniKeyColor* keys, EKEY_TYPE keyType, 
	cjUINT32 keyCount, cjUINT32& lastIdx, cjBYTE siz, cjFLOAT& valTime0, cjFLOAT& valTime1, 
    cjColorF& val0, cjColorF& outTan, cjColorF& valA, cjColorF& valB)
{
    cjAssert(keyCount != 0);

    if(keyCount == 1)
    {
        // Hold this constant value forever.
        val0 = keys->GetKey(0, siz)->GetValue();
        outTan = cjColorF(0, 0, 0, 0);
        valA = cjColorF(0, 0, 0, 0);
        valB = cjColorF(0, 0, 0, 0);

        valTime0 = 0.0f;
        valTime1 = CJ_FLT_MAX;		

        lastIdx = 0;
        return;
    }

    cjUINT32 enKeyIndex = keyCount - 1;
	cjUINT32 stkLastIdx = lastIdx;

    cjFLOAT lastTime = keys->GetKey(stkLastIdx, siz)->GetTime();
    if(etime < lastTime)
    {
        stkLastIdx = 0;
        lastTime = keys->GetKey(0, siz)->GetTime();
    }

    cjUINT32 curKeyIdx;
    cjFLOAT nextTime = 0.0f;
    for(curKeyIdx=stkLastIdx+1; curKeyIdx<=enKeyIndex; curKeyIdx++)
    {
        nextTime = keys->GetKey(curKeyIdx, siz)->GetTime();
        if((etime < nextTime) || (etime == nextTime && keyType != EKT_STEPKEY))
            break;

        stkLastIdx++;
        lastTime = nextTime;
    }
    
    if(keyType == EKT_STEPKEY && etime >= nextTime && curKeyIdx == keyCount)
    {
        val0			= keys->GetKey(enKeyIndex, siz)->GetValue();
        outTan			= cjColorF(0, 0, 0, 0);
        valA				= cjColorF(0, 0, 0, 0);
        valB				= cjColorF(0, 0, 0, 0);
        valTime0		= nextTime;
        valTime1		= nextTime;
        lastIdx			= enKeyIndex;
    }
    else
    {
        cjAssert(curKeyIdx < keyCount);
		cjAniKey::PFCubicCoefs cubiccoefs = cjAniKeyColor::GetCubicCoefsKeyPF(keyType);

        cjAssert(cubiccoefs);
        cubiccoefs(keys, keyCount, stkLastIdx, &val0, &outTan, &valA, &valB);

        valTime0 = lastTime;
        valTime1 = nextTime;
        lastIdx = stkLastIdx;
    }
}

cjColorF cjAniKeyColor::GenInterpDefault(cjFLOAT etime, cjAniKeyColor* keys,
    EKEY_TYPE keyType, cjUINT32 keyCount, cjBYTE siz)
{
    if(keyCount)
    {
        // GenInterp no longer handles out-of-range times
        if(etime < keys->GetKey(0, siz)->GetTime())
            return keys->GetKey(0, siz)->GetValue();
        else if(etime > keys->GetKey(keyCount-1, siz)->GetTime())
            return keys->GetKey(keyCount-1, siz)->GetValue();
        else
        {
            cjUINT32 lastIdx = 0;
            return cjAniKeyColor::GenInterp(etime, keys, keyType, keyCount,
                lastIdx, siz);
        }
    }
    else
    {
        return ms_default;
    }
}

cjBool cjAniKeyColor::EqualKey(const cjAniKey& key0, const cjAniKey& key1)
{
    if(!cjAniKey::EqualKey(key0, key1))
        return cjFalse;

    const cjAniKeyColor& c0 = (const cjAniKeyColor&)key0;
    const cjAniKeyColor& c1 = (const cjAniKeyColor&)key1;

    return c0.m_color == c1.m_color;
}


//
// cjAniKeyLinColor
//

CJ_ANIKEYOPER_IMPL(cjAniKeyLinColor, EKVAL_COLOR, EKT_LINKEY);

void cjAniKeyLinColor::RegisterCustomAniKeyOperation(EKEY_VALTYPE valType, EKEY_TYPE keyType)
{
	SetIsPosedKeyPF		(valType, keyType, IsPosedKey);
	SetEqualKeyPF		(valType, keyType, EqualKey);

	SetInsertKeyPF		(valType, keyType, InsertKey);

	SetCurvedRatioKeyPF	(valType, keyType, cjNULL);
    SetFillDerivedValsKeyPF(valType, keyType, cjNULL);
	SetCubicCoefsKeyPF	(valType, keyType,  CubicCoefsKey);

	SetInterpKeyPF		(valType, keyType, InterpolateKey);
    SetInterpD1KeyPF	(valType, keyType, cjNULL);
    SetInterpD2KeyPF	(valType, keyType, cjNULL);
}

cjAniKeyLinColor::cjAniKeyLinColor()
{
}

cjAniKeyLinColor::cjAniKeyLinColor(cjFLOAT etime, const cjColorF& color)
	: cjAniKeyColor(etime,color)
{
}

void cjAniKeyLinColor::SaveKey(cjFStream& stream, cjAniKey* keys, cjUINT32 keyCount)
{
    cjAniKeyLinColor* k = (cjAniKeyLinColor*)keys;

    for(cjUINT32 i=0; i<keyCount; i++)
		stream << k[i];
}

cjAniKey* cjAniKeyLinColor::LoadKey(cjFStream& stream, cjUINT32 keyCount)
{
    cjAniKeyLinColor* k = new cjAniKeyLinColor[keyCount];
	
	cjAssert(stream.IsLoading());

    for(cjUINT32 i=0; i<keyCount; i++)
		stream << k[i];

	return k;
}

cjAniKey* cjAniKeyLinColor::CreateKey(cjUINT32 keyCount)
{
    return new cjAniKeyLinColor[keyCount];
}

void cjAniKeyLinColor::DeleteKey(cjAniKey* keys)
{
    cjAniKeyLinColor* k = (cjAniKeyLinColor*)keys;
    delete [] k;
}

void cjAniKeyLinColor::CopyKey(cjAniKey* dstKey, const cjAniKey* srcKey)
{
    cjAniKeyLinColor* dst = (cjAniKeyLinColor*)dstKey;
    cjAniKeyLinColor* src = (cjAniKeyLinColor*)srcKey;

    dst->m_time = src->m_time;
    dst->m_color = src->m_color;
}

cjBool cjAniKeyLinColor::IsPosedKey(const cjAniKey* keys, cjUINT32 keyCount)
{
    cjAssert(keys);
    cjAssert(keyCount > 0);

    if(keyCount > 1)
    {
        const cjAniKeyLinColor* linColorKeys = (const cjAniKeyLinColor*)keys;

        cjColorF value = linColorKeys[0].m_color;

        for(cjUINT32 i = 1; i < keyCount; i++)
        {
            if(linColorKeys[i].m_color != value)
                return cjFalse;
        }
    }

    return cjTrue;
}

cjBool cjAniKeyLinColor::EqualKey(const cjAniKey& key0, const cjAniKey& key1)
{
    return cjAniKeyColor::EqualKey(key0, key1);
}

cjBool cjAniKeyLinColor::InsertKey(cjFLOAT etime, cjAniKey*& keys, cjUINT32& keyCount)
{
    cjAniKeyLinColor* linColorKeys = (cjAniKeyLinColor*) keys;

    cjUINT32 dstSiz;
    cjUINT32 insIndex;

    if(TestKeyInsertable(etime, linColorKeys, keyCount, sizeof(cjAniKeyLinColor), insIndex))
    {
        cjAniKeyLinColor* newKeys = new cjAniKeyLinColor[keyCount + 1];

        dstSiz = (keyCount + 1) * sizeof(cjAniKeyLinColor);
        cjMemCpy(newKeys, dstSiz, linColorKeys, insIndex * sizeof(cjAniKeyLinColor));

        if(keyCount > insIndex)
        {
            dstSiz = (keyCount - insIndex) * sizeof(cjAniKeyLinColor);
            cjMemCpy(&newKeys[insIndex + 1], &linColorKeys[insIndex], dstSiz);
        }

        cjColorF value = cjAniKeyColor::GenInterpDefault(etime, linColorKeys,
			EKT_LINKEY, keyCount, sizeof(cjAniKeyLinColor));

        cjAniKeyLinColor* newKey = &newKeys[insIndex];
        newKey->m_time = etime;
        newKey->m_color = value;

        keyCount++;

        delete[] linColorKeys;
        keys = newKeys;

        return cjTrue;
    }
    else
    {
        return cjFalse;
    }
}

void cjAniKeyLinColor::CubicCoefsKey(const cjAniKey* keys, cjUINT32 keyCount, cjUINT32 index, 
	void* val0, void* outTangent0, void* a0, void* b0)
{
    cjAssert(index < keyCount - 1);

    const cjAniKeyLinColor* linColorKeys = (const cjAniKeyLinColor*)keys;

    *(cjColorF*)val0 = linColorKeys[index].m_color;
    *(cjColorF*)outTangent0 = linColorKeys[index + 1].m_color - linColorKeys[index].m_color;
    *(cjColorF*)a0 = cjColorF(0, 0, 0, 0);
    *(cjColorF*)b0 = cjColorF(0, 0, 0, 0);
}

void cjAniKeyLinColor::InterpolateKey(cjFLOAT etime, const cjAniKey* key0, const cjAniKey* key1, void* rst)
{
    cjAniKeyColor* colorKey0 = (cjAniKeyColor*)key0;
    cjAniKeyColor* colorKey1 = (cjAniKeyColor*)key1;

    *(cjColorF*)rst = (colorKey1->GetValue() * etime) + (colorKey0->GetValue() * (1.0f - etime));
}


//
// cjAniKeyBool
//

CJ_ANIKEYOPER_IMPL(cjAniKeyBool, EKVAL_BOOL, EKT_NOINTERP);

cjBool cjAniKeyBool::ms_default = cjFalse;

void cjAniKeyBool::RegisterCustomAniKeyOperation(EKEY_VALTYPE valType, EKEY_TYPE keyType)
{
    SetIsPosedKeyPF			(valType, keyType,	cjNULL);
	SetEqualKeyPF			(valType, keyType,	EqualKey);

    SetInsertKeyPF			(valType, keyType,	cjNULL);

	SetCurvedRatioKeyPF		(valType, keyType,	CaclCurveRatioKey);
    SetFillDerivedValsKeyPF	(valType, keyType,	FillDerivedValsKey);
    SetCubicCoefsKeyPF		(valType, keyType,	cjNULL);
	SetInterpKeyPF			(valType, keyType,	InterpolateKey);
    SetInterpD1KeyPF		(valType, keyType,	InterpolateD1Key);
    SetInterpD2KeyPF		(valType, keyType,	InterpolateD2Key);    
}

cjBool cjAniKeyBool::GenInterp(cjFLOAT etime, cjAniKeyBool* keys, cjAniKeyBool::EKEY_TYPE keyType,
	cjUINT32 keyCount, cjUINT32& lastIdx, cjBYTE siz)
{
    cjAssert(keyCount != 0);

    if(keyCount == 1)
        return keys->GetKey(0, siz)->GetValue();

    cjUINT32 enKeyIndex = keyCount - 1;
    cjFLOAT lastTime = keys->GetKey(lastIdx, siz)->GetTime();

    if(etime < lastTime)
    {
        lastIdx = 0;
        lastTime = keys->GetKey(0, siz)->GetTime();
    }
    
    cjUINT32 curKeyIdx;
    cjFLOAT nextTime = 0.0f;
    for(curKeyIdx=lastIdx+1; curKeyIdx <= enKeyIndex; curKeyIdx++)
    {
        nextTime = keys->GetKey(curKeyIdx, siz)->GetTime();

        if(etime <= nextTime)
            break;

        lastIdx++;
        lastTime = nextTime;
    }

    cjAssert(curKeyIdx < keyCount);
    cjAssert(nextTime > lastTime);

    cjAniKeyBool::PFInterpolation interp = cjAniKeyBool::GetInterpKeyPF(keyType);
	cjAssert(interp);

    cjFLOAT ratio = (etime - lastTime) / (nextTime - lastTime);	    
    cjBool rst;

    interp(ratio, keys->GetKey(lastIdx, siz), keys->GetKey(curKeyIdx, siz), &rst);

    return rst;
}

void cjAniKeyBool::GenCubicCoefs(cjFLOAT etime, cjAniKeyBool* keys, EKEY_TYPE keyType, 
	cjUINT32 keyCount, cjUINT32& lastIdx, cjBYTE siz, cjFLOAT& valTime0, cjFLOAT& valTime1, 
    cjFLOAT& val0, cjFLOAT& outTan, cjFLOAT& valA, cjFLOAT& valB)
{
    cjAssert(keyCount != 0);

    if(keyCount == 1)
    {
        val0			= keys->GetKey(0, siz)->GetValue() ? 1.0f : 0.0f;
        outTan			= 0.0f;
        valA			= 0.0f;
        valB			= 0.0f;
        valTime0		= 0.0f;
        valTime1		= CJ_FLT_MAX;
        lastIdx			= 0;

        return;
    }

    cjUINT32 enKeyIndex = keyCount - 1;
    cjUINT32 stkLastIdx = lastIdx;

    cjFLOAT lastTime = keys->GetKey(stkLastIdx, siz)->GetTime();
    if(etime < lastTime)
    {
        stkLastIdx = 0;
        lastTime = keys->GetKey(0, siz)->GetTime();
    }

    cjUINT32 curKeyIdx;
    cjFLOAT nextTime = 0.0f;

    for(curKeyIdx = stkLastIdx+1; curKeyIdx <= enKeyIndex; curKeyIdx++)
    {
        nextTime = keys->GetKey(curKeyIdx, siz)->GetTime();

		if((etime < nextTime) || (etime == nextTime && keyType != EKT_STEPKEY))
            break;

        stkLastIdx++;
        lastTime = nextTime;
    }
    
	// handle step key
    if(keyType == EKT_STEPKEY && etime >= nextTime && curKeyIdx == keyCount)
    {
        val0 = keys->GetKey(enKeyIndex, siz)->GetValue() ? 1.0f : 0.0f;
        outTan = 0.0f;
        valA = 0.0f;
        valB = 0.0f;
        valTime0 = nextTime;
        valTime1 = nextTime;
        lastIdx = enKeyIndex;
    }
    else
    {
        cjAssert(curKeyIdx < keyCount);
        cjAniKey::PFCubicCoefs cubiccoefs = cjAniKeyBool::GetCubicCoefsKeyPF(keyType);
        cjAssert(cubiccoefs);

        cubiccoefs(keys, keyCount, stkLastIdx, &val0, &outTan, &valA, &valB);
        valTime0 = lastTime;
        valTime1 = nextTime;

        lastIdx = stkLastIdx;
    }
}

cjBool cjAniKeyBool::GenInterpDefault(cjFLOAT etime, cjAniKeyBool* keys, EKEY_TYPE keyType, 
	cjUINT32 keyCount, cjBYTE siz)
{
    if(keyCount)
    {
        if(etime < keys->GetKey(0, siz)->GetTime()) // first key
		{
            return keys->GetKey(0, siz)->GetValue();
		}
        else if(etime > keys->GetKey(keyCount-1, siz)->GetTime()) // lastkey
		{ 
            return keys->GetKey(keyCount-1, siz)->GetValue();
		}
        else
        {
            cjUINT32 lastIdx = 0;
            return cjAniKeyBool::GenInterp(etime, keys, keyType, keyCount, lastIdx, siz);
        }
    }

	return ms_default;
}

cjBool cjAniKeyBool::EqualKey(const cjAniKey& k0, const cjAniKey& k1)
{
    if(!cjAniKey::EqualKey(k0,k1))
        return cjFalse;

    const cjAniKeyBool& b0 = (const cjAniKeyBool&)k0;
    const cjAniKeyBool& b1 = (const cjAniKeyBool&)k1;

    return b0.m_value == b1.m_value;
}

//
// cjAniKeyBool
//  - cjAniKeyStepBool
//

CJ_ANIKEYOPER_IMPL(cjAniKeyStepBool, EKVAL_BOOL, EKT_STEPKEY);

void cjAniKeyStepBool::RegisterCustomAniKeyOperation(EKEY_VALTYPE valType, EKEY_TYPE keyType)
{
    SetIsPosedKeyPF			(valType, keyType,	IsPosedKey);
	SetEqualKeyPF			(valType, keyType,	EqualKey);

    SetInsertKeyPF			(valType, keyType,	InsertKey);

	SetCurvedRatioKeyPF		(valType, keyType,	cjNULL);
    SetFillDerivedValsKeyPF	(valType, keyType,	FillDerivedValsKey);
    SetCubicCoefsKeyPF		(valType, keyType,	CubicCoefsKey);
	SetInterpKeyPF			(valType, keyType,	InterpolateKey);
    SetInterpD1KeyPF		(valType, keyType,	cjNULL);
    SetInterpD2KeyPF		(valType, keyType,	cjNULL);   
}

void cjAniKeyStepBool::SaveKey(cjFStream& stream, cjAniKey* keys, cjUINT32 keyCount)
{
    cjAniKeyStepBool* stepBoolKeys = (cjAniKeyStepBool*)keys;

    for(cjUINT32 i=0; i<keyCount; i++)
		stream << stepBoolKeys[i];
}

cjAniKey* cjAniKeyStepBool::LoadKey(cjFStream& stream, cjUINT32 keyCount)
{
    cjAniKeyStepBool* stepBoolKeys = new cjAniKeyStepBool[keyCount];

	cjAssert(stream.IsLoading());

    for(cjUINT32 i=0; i<keyCount; i++)
        stream << stepBoolKeys[i];

	return stepBoolKeys;
}

cjAniKey* cjAniKeyStepBool::CreateKey(cjUINT32 keyCount)
{
    return new cjAniKeyStepBool[keyCount];
}

void cjAniKeyStepBool::DeleteKey(cjAniKey* keys)
{
    cjAniKeyStepBool* stepBoolKeys = (cjAniKeyStepBool*)keys;
    delete [] stepBoolKeys;
}

void cjAniKeyStepBool::CopyKey(cjAniKey* dstKey, const cjAniKey* srcKey)
{
    cjAniKeyStepBool* dstStep = (cjAniKeyStepBool*)dstKey;
    cjAniKeyStepBool* srcStep = (cjAniKeyStepBool*)srcKey;

    dstStep->m_time = srcStep->m_time;
    dstStep->m_value = srcStep->m_value;
}

cjBool cjAniKeyStepBool::IsPosedKey(const cjAniKey* keys, cjUINT32 keyCount)
{
    if(keyCount > 1)
    {
        const cjAniKeyStepBool* stepKeys = (const cjAniKeyStepBool*)keys;

        cjBool v = stepKeys[0].m_value;

        for(cjUINT32 i=1; i<keyCount; i++)
        {
            if(stepKeys[i].m_value != v)
                return cjFalse;
        }
    }

    return cjTrue;
}

cjBool cjAniKeyStepBool::InsertKey(cjFLOAT etime, cjAniKey*& keys, cjUINT32& keyCount)
{
    cjAniKeyStepBool* stepBoolKeys = (cjAniKeyStepBool*)keys;

    cjUINT32 dstSiz;
    cjUINT32 insIndex;

    if(TestKeyInsertable(etime, stepBoolKeys, keyCount, sizeof(cjAniKeyStepBool), insIndex))
    {
        cjAniKeyStepBool* newKeys = new cjAniKeyStepBool[keyCount + 1];
        dstSiz = (keyCount + 1) * sizeof(cjAniKeyStepBool);

        cjMemCpy(newKeys, dstSiz, stepBoolKeys, insIndex * sizeof(cjAniKeyStepBool));
        if(keyCount > insIndex)
        {
            dstSiz = (keyCount - insIndex) * sizeof(cjAniKeyStepBool);
            cjMemCpy(&newKeys[insIndex + 1], &stepBoolKeys[insIndex], dstSiz);
        }

        cjBool v = cjAniKeyBool::GenInterpDefault(etime, stepBoolKeys, EKT_STEPKEY, keyCount, sizeof(cjAniKeyStepBool));
        cjAniKeyStepBool* newKey = &newKeys[insIndex];

        newKey->m_time = etime;
        newKey->m_value = v;

        keyCount++;
        delete[] stepBoolKeys;

        keys = newKeys;
        FillDerivedValsKey(keys, keyCount, sizeof(cjAniKeyStepBool));

        return cjTrue;
    }
    else
    {
        return cjFalse;
    }
}

void cjAniKeyStepBool::CubicCoefsKey(const cjAniKey* keys, cjUINT32 keyCount, cjUINT32 index, 
	void* v0, void* vOutTangent, void* va0, void* vb0)
{
    cjAssert(index < keyCount);
    const cjAniKeyStepBool* stepKeys = (const cjAniKeyStepBool*)keys;

    *(cjFLOAT*)v0 = stepKeys[index].m_value ? 1.0f : 0.0f;
    *(cjFLOAT*)vOutTangent = 0.0f;
    *(cjFLOAT*)va0 = 0.0f;
    *(cjFLOAT*)vb0 = 0.0f;
}

void cjAniKeyStepBool::InterpolateKey(cjFLOAT rtime, const cjAniKey* key0, const cjAniKey* key1, void* rst)
{
    const cjAniKeyStepBool* step0 = (const cjAniKeyStepBool*)key0;
    const cjAniKeyStepBool* step1 = (const cjAniKeyStepBool*)key1;

    if(rtime < 1.0f)
        *(cjBool*)rst = step0->GetValue();
    else
        *(cjBool*)rst = step1->GetValue();
}


//
// cjFloatDat
//

CJSER_IMPL_CLASS(cjAniFloatDat, cjRefObj)

CJSER_IMPL_SERIALIZE_BEGIN(cjAniFloatDat)

	if(ser.IsQuerySerObjAndStr())
		return;

	if(ser.IsLoading())
	{
		cjUINT32 keyCount;
		ser << keyCount;

		if(keyCount > 0)
		{			
			cjFStreamEnum(ser, m_keyType);

			m_keySiz = cjAniKeyFloat::GetKeySiz(m_keyType);
			cjAniKeyFloat::PFLoad pfLoad = cjAniKeyFloat::GetLoadKeyPF(m_keyType);
			cjAniKeyFloat* keys = (cjAniKeyFloat*)pfLoad(ser, keyCount);

			cjAniKeyFloat::PFFillDerivedVals pfFillDerived = cjAniKeyFloat::GetFillDerivedValsKeyPF(m_keyType);
			pfFillDerived(keys, keyCount, m_keySiz);

			ReplaceKeys(keys, keyCount, m_keyType);
		}
	}
	else if(ser.IsSaving())
	{
		ser << m_keyCount;

		if(m_keyCount > 0)
		{
			cjFStreamEnum(ser, m_keyType);

			cjAniKeyFloat::PFSave pfSave = cjAniKeyFloat::GetSaveKeyPF(m_keyType);
			pfSave(ser, m_keys, m_keyCount);
		}
	}

CJSER_IMPL_SERIALIZE_END(cjAniFloatDat)

cjSerializable* cjAniFloatDat::_CloneAsCopy()
{
	cjAniFloatDat* d = new cjAniFloatDat;

	d->m_keyCount = m_keyCount;
	d->m_keyType = m_keyType;
	d->m_keySiz = m_keySiz;
	d->m_keys = cjNULL;

	if(m_keyCount)
	{
		cjAniKeyFloat::PFCreate pfCreate = cjAniKeyFloat::GetCreateKeyPF(m_keyType);

		cjAniKeyFloat* dstKeys = (cjAniKeyFloat*)pfCreate(m_keyCount);
		
		cjAniKeyFloat::PFCopy pfCopy = cjAniKeyFloat::GetCopyKeyPF(m_keyType);
		for(cjUINT32 i=0; i<m_keyCount; i++)
			pfCopy(dstKeys->GetKey(i, d->m_keySiz), m_keys->GetKey(i, m_keySiz));
				
		d->ReplaceKeys(dstKeys, m_keyCount, m_keyType);
	}	

	return d;
}


cjAniFloatDat::~cjAniFloatDat()
{
	if(m_keys)
	{
		cjAniKeyFloat::PFDelete deleteKey = cjAniKeyFloat::GetDeleteKeyPF(m_keyType);
		deleteKey(m_keys);
	}
}

cjAniKeyFloat* cjAniFloatDat::GetKeys(cjUINT32& keyCount, cjAniKeyFloat::EKEY_TYPE& keyType, cjBYTE& keySiz) const
{
    keyCount = m_keyCount; 
	keyType = m_keyType; 
	keySiz = m_keySiz;

    return m_keys;
}

void cjAniFloatDat::ReplaceKeys(cjAniKeyFloat* keys, cjUINT32 keyCount, cjAniKeyFloat::EKEY_TYPE keyType)
{
	if(m_keys)
	{
		cjAniKeyFloat::PFDelete deleteKey = cjAniKeyFloat::GetDeleteKeyPF(m_keyType);
		deleteKey(m_keys);
	}

	SetKeys(keys, keyCount, keyType);
}

void cjAniFloatDat::SetKeys(cjAniKeyFloat* keys, cjUINT32 keyCount, cjAniKeyFloat::EKEY_TYPE keyType)
{
	if(!keys || !keyCount || keyType == cjAniKey::EKT_NOINTERP)
	{
		m_keyCount = 0;
		m_keys = 0;
		m_keySiz = 0;
		m_keyType = cjAniKey::EKT_NOINTERP;

		return;
	}

	m_keySiz = cjAniKeyFloat::GetKeySiz(keyType);

	m_keyCount = keyCount;
	m_keys = keys;
	m_keyType = keyType;
}

void cjAniFloatDat::CacheKeys(cjFLOAT stTime, cjFLOAT enTime)
{
	cjAssert(m_keyCount != 0);

	cjAniKey* keys = (cjAniKey*)m_keys;
	cjAniKey::RecursiveGetKeysByRange(cjAniKey::EKVAL_FLOAT, m_keyType, keys, m_keyCount, stTime, enTime);
	m_keys = (cjAniKeyFloat*)keys;
}

cjAniFloatDatRPtr cjAniFloatDat::GetDat(cjFLOAT stTime, cjFLOAT enTime)
{
	cjAniFloatDatRPtr spNewDatFloat = (cjAniFloatDat*)_CloneAsCopy();

	if(m_keyCount > 0)
	{
		cjAniKey* newKeys = cjNULL;
		cjUINT32 newKeyCount = 0;

		cjAniKey::CloneKeysByRange(cjAniKey::EKVAL_FLOAT, m_keyType, m_keys, m_keyCount, stTime, enTime, newKeys, newKeyCount);
		spNewDatFloat->ReplaceKeys((cjAniKeyFloat*)newKeys, newKeyCount, m_keyType);
	}

	return spNewDatFloat;
}


//
// cjAniBoolDat
//

CJSER_IMPL_CLASS(cjAniBoolDat, cjRefObj)

CJSER_IMPL_SERIALIZE_BEGIN(cjAniBoolDat)

	if(ser.IsQuerySerObjAndStr())
		return;

	if(ser.IsLoading())
	{
		cjUINT32 keyCount;
		ser << keyCount;

		if(keyCount > 0)
		{			
			cjFStreamEnum(ser, m_keyType);

			m_keySiz = cjAniKeyBool::GetKeySiz(m_keyType);
			cjAniKeyBool::PFLoad pfLoad = cjAniKeyBool::GetLoadKeyPF(m_keyType);
			cjAniKeyBool* keys = (cjAniKeyBool*)pfLoad(ser, keyCount);

			cjAniKeyBool::PFFillDerivedVals pfFillDerived = cjAniKeyBool::GetFillDerivedValsKeyPF(m_keyType);
			pfFillDerived(keys, keyCount, m_keySiz);

			ReplaceKeys(keys, keyCount, m_keyType);
		}
	}
	else if(ser.IsSaving())
	{
		ser << m_keyCount;

		if(m_keyCount > 0)
		{
			cjFStreamEnum(ser, m_keyType);

			cjAniKeyBool::PFSave pfSave = cjAniKeyBool::GetSaveKeyPF(m_keyType);
			pfSave(ser, m_keys, m_keyCount);
		}
	}

CJSER_IMPL_SERIALIZE_END(cjAniBoolDat)

cjSerializable* cjAniBoolDat::_CloneAsCopy()
{
	cjAniBoolDat* d = new cjAniBoolDat;

	d->m_keyCount = m_keyCount;
	d->m_keyType = m_keyType;
	d->m_keySiz = m_keySiz;
	d->m_keys = cjNULL;

	if(m_keyCount)
	{
		cjAniKeyBool::PFCreate pfCreate = cjAniKeyBool::GetCreateKeyPF(m_keyType);
		cjAniKeyBool* dstKeys = (cjAniKeyBool*)pfCreate(m_keyCount);
		
		cjAniKeyBool::PFCopy pfCopy = cjAniKeyBool::GetCopyKeyPF(m_keyType);
		for(cjUINT32 i=0; i<m_keyCount; i++)
			pfCopy(dstKeys->GetKey(i, d->m_keySiz), m_keys->GetKey(i, m_keySiz));

		d->ReplaceKeys(dstKeys, m_keyCount, m_keyType);
	}	

	return d;
}

cjAniBoolDat::~cjAniBoolDat()
{
	if(m_keys)
	{
		cjAniKeyBool::PFDelete pfDeleteKey = cjAniKeyBool::GetDeleteKeyPF(m_keyType);
		pfDeleteKey(m_keys);
	}
}

cjAniKeyBool* cjAniBoolDat::GetKeys(cjUINT32& keyCount, cjAniKeyBool::EKEY_TYPE& keyType, cjBYTE& keySiz) const
{
    keyCount = m_keyCount; 
	keyType = m_keyType; 
	keySiz = m_keySiz;

    return m_keys;
}

void cjAniBoolDat::ReplaceKeys(cjAniKeyBool* keys, cjUINT32 keyCount, cjAniKeyBool::EKEY_TYPE keyType)
{
	if(m_keys)
	{
		cjAniKeyBool::PFDelete pfDeleteKey = cjAniKeyBool::GetDeleteKeyPF(m_keyType);
		pfDeleteKey(m_keys);
	}

	SetKeys(keys, keyCount, keyType);
}

void cjAniBoolDat::SetKeys(cjAniKeyBool* keys, cjUINT32 keyCount, cjAniKeyBool::EKEY_TYPE keyType)
{
	if(!keys || !keyCount)
	{
		m_keyCount = 0;
		m_keys = 0;
		m_keySiz = 0;
		m_keyType = cjAniKey::EKT_NOINTERP;

		return;
	}

	m_keySiz = cjAniKeyBool::GetKeySiz(keyType);
	m_keyCount = keyCount;
	m_keys = keys;
	m_keyType = keyType;
}

void cjAniBoolDat::CacheKeys(cjFLOAT stTime, cjFLOAT enTime)
{
	cjAssert(m_keyCount != 0);

	cjAniKey* keys = (cjAniKey*)m_keys;
	cjAniKey::RecursiveGetKeysByRange(cjAniKey::EKVAL_BOOL, m_keyType, keys, m_keyCount, stTime, enTime);
	m_keys = (cjAniKeyBool*)keys;
}

cjAniBoolDatRPtr cjAniBoolDat::GetDat(cjFLOAT stTime, cjFLOAT enTime)
{
	cjAniBoolDatRPtr spNewData = (cjAniBoolDat*)_CloneAsCopy();

	if(m_keyCount > 0)
	{
		cjAniKey* newKeys = cjNULL;
		cjUINT32 newKeyCount = 0;

		cjAniKey::CloneKeysByRange(cjAniKey::EKVAL_BOOL, m_keyType, m_keys, m_keyCount, stTime, enTime, newKeys, newKeyCount);
		spNewData->ReplaceKeys((cjAniKeyBool*)newKeys, newKeyCount, m_keyType);
	}

	return spNewData;
}


//
// cjAniRotDat
//

CJSER_IMPL_CLASS(cjAniRotDat, cjRefObj)

CJSER_IMPL_SERIALIZE_BEGIN(cjAniRotDat)

	if(ser.IsQuerySerObjAndStr())
		return;

	if(ser.IsLoading())
	{
		cjUINT32 keyCount;
		ser << keyCount;

		if(keyCount > 0)
		{			
			cjFStreamEnum(ser, m_keyType);

			m_keySiz = cjAniKeyRot::GetKeySiz(m_keyType);
			cjAniKeyRot::PFLoad pfLoad = cjAniKeyRot::GetLoadKeyPF(m_keyType);
			cjAniKeyRot* keys = (cjAniKeyRot*)pfLoad(ser, keyCount);

			cjAniKeyRot::PFFillDerivedVals pfFillDerived = cjAniKeyRot::GetFillDerivedValsKeyPF(m_keyType);
			pfFillDerived(keys, keyCount, m_keySiz);

			ReplaceKeys(keys, keyCount, m_keyType);
		}
	}
	else if(ser.IsSaving())
	{
		ser << m_keyCount;

		if(m_keyCount > 0)
		{
			cjFStreamEnum(ser, m_keyType);

			cjAniKeyRot::PFSave pfSave = cjAniKeyRot::GetSaveKeyPF(m_keyType);
			pfSave(ser, m_keys, m_keyCount);
		}
	}

CJSER_IMPL_SERIALIZE_END(cjAniRotDat)

cjSerializable* cjAniRotDat::_CloneAsCopy()
{
	cjAniRotDat* d = new cjAniRotDat;

	d->m_keyCount = m_keyCount;
	d->m_keyType = m_keyType;
	d->m_keySiz = m_keySiz;
	d->m_keys = cjNULL;

	if(m_keyCount)
	{
		cjAniKeyRot::PFCreate pfCreate = cjAniKeyRot::GetCreateKeyPF(m_keyType);
		cjAniKeyRot* dstKeys = (cjAniKeyRot*)pfCreate(m_keyCount);		
		cjAniKeyRot::PFCopy pfCopy = cjAniKeyRot::GetCopyKeyPF(m_keyType);

		for(cjUINT32 i=0; i<m_keyCount; i++)
			pfCopy(dstKeys->GetKey(i, d->m_keySiz), m_keys->GetKey(i, m_keySiz));

		d->ReplaceKeys(dstKeys, m_keyCount, m_keyType);
	}	

	return d;
}


cjAniRotDat::cjAniRotDat() 
	: m_keyCount(0), m_keys(cjNULL), m_keyType(cjAniKeyRot::EKT_NOINTERP), m_keySiz(0)
{
}

cjAniRotDat::~cjAniRotDat()
{
    if(m_keys)
    {
        cjAniKeyRot::PFDelete pfDelete = cjAniKeyRot::GetDeleteKeyPF(m_keyType);
        pfDelete(m_keys);
    }
}

cjAniKeyRot* cjAniRotDat::GetKeys(cjUINT32& keyCount, cjAniKeyRot::EKEY_TYPE& keyType, cjBYTE& keySiz) const
{
	keyCount = m_keyCount;
	keyType = m_keyType;
	keySiz = m_keySiz;

	return m_keys;
}

void cjAniRotDat::ReplaceKeys(cjAniKeyRot* keys, cjUINT32 keyCount, cjAniKeyRot::EKEY_TYPE keyType)
{
    if(m_keys)
    {
        cjAniKeyRot::PFDelete pfDelete = cjAniKeyRot::GetDeleteKeyPF(m_keyType);
        pfDelete(m_keys);
    }

    SetKeys(keys, keyCount, keyType);
}

void cjAniRotDat::SetKeys(cjAniKeyRot* keys, cjUINT32 keyCount, cjAniKeyRot::EKEY_TYPE keyType)
{
    if(!keys || keyCount == 0)
    {
        m_keyCount = 0;
        m_keys = cjNULL;
        m_keyType = cjAniKeyRot::EKT_NOINTERP;
        m_keySiz = 0;

        return;
    }

    m_keySiz = cjAniKeyRot::GetKeySiz(keyType);
    m_keyCount = keyCount;
    m_keys = keys;
    m_keyType = keyType;
}

void cjAniRotDat::CacheKeys(cjFLOAT stTime, cjFLOAT enTime)
{
    cjAssert(m_keyCount != 0);

    cjAniKey* keys = (cjAniKey*)m_keys;
    cjAniKey::RecursiveGetKeysByRange(cjAniKey::EKVAL_ROT, m_keyType, keys, m_keyCount, stTime, enTime);
    m_keys = (cjAniKeyRot*)keys;
}

cjAniRotDatRPtr cjAniRotDat::GetDat(cjFLOAT stTime, cjFLOAT enTime)
{
    cjAniRotDatRPtr spNewDatRot = (cjAniRotDat*)_CloneAsCopy();

    if(m_keyCount)
    {
        cjAniKey* newKeys = cjNULL;
        cjUINT32 newKeyCount = 0;
        cjAniKey::CloneKeysByRange(cjAniKey::EKVAL_ROT, m_keyType, m_keys, m_keyCount, stTime, enTime, newKeys, newKeyCount);
        spNewDatRot->ReplaceKeys((cjAniKeyRot*)newKeys, newKeyCount, m_keyType);
    }

    return spNewDatRot;
}


//
// cjAniVec3Dat
//

CJSER_IMPL_CLASS(cjAniVec3Dat, cjRefObj)

CJSER_IMPL_SERIALIZE_BEGIN(cjAniVec3Dat)

	if(ser.IsQuerySerObjAndStr())
		return;

	if(ser.IsLoading())
	{
		cjUINT32 keyCount;
		ser << keyCount;

		if(keyCount > 0)
		{			
			cjFStreamEnum(ser, m_keyType);

			m_keySiz = cjAniKeyVec3::GetKeySiz(m_keyType);
			cjAniKeyVec3::PFLoad pfLoad = cjAniKeyVec3::GetLoadKeyPF(m_keyType);
			cjAniKeyVec3* keys = (cjAniKeyVec3*)pfLoad(ser, keyCount);

			cjAniKeyVec3::PFFillDerivedVals pfFillDerived = cjAniKeyVec3::GetFillDerivedValsKeyPF(m_keyType);
			pfFillDerived(keys, keyCount, m_keySiz);

			ReplaceKeys(keys, keyCount, m_keyType);
		}
	}
	else if(ser.IsSaving())
	{
		ser << m_keyCount;

		if(m_keyCount > 0)
		{
			cjFStreamEnum(ser, m_keyType);

			cjAniKeyVec3::PFSave pfSave = cjAniKeyVec3::GetSaveKeyPF(m_keyType);
			pfSave(ser, m_keys, m_keyCount);
		}
	}

CJSER_IMPL_SERIALIZE_END(cjAniVec3Dat)

cjSerializable* cjAniVec3Dat::_CloneAsCopy()
{
	cjAniVec3Dat* d = new cjAniVec3Dat;

	d->m_keyCount = m_keyCount;
	d->m_keyType = m_keyType;
	d->m_keySiz = m_keySiz;
	d->m_keys = cjNULL;

	if(m_keyCount)
	{
		cjAniKeyVec3::PFCreate pfCreate = cjAniKeyVec3::GetCreateKeyPF(m_keyType);
		cjAniKeyVec3* dstKeys = (cjAniKeyVec3*)pfCreate(m_keyCount);
		cjAniKeyVec3::PFCopy pfCopy = cjAniKeyVec3::GetCopyKeyPF(m_keyType);

		for(cjUINT32 i=0; i<m_keyCount; i++)
			pfCopy(dstKeys->GetKey(i, d->m_keySiz), m_keys->GetKey(i, m_keySiz));

		d->ReplaceKeys(dstKeys, m_keyCount, m_keyType);
	}

	return d;
}


cjAniVec3Dat::cjAniVec3Dat()
{
    m_keyCount = 0;
    m_keys = 0;
    m_keyType = cjAniKey::EKT_NOINTERP;
    m_keySiz = 0;
}

cjAniVec3Dat::~cjAniVec3Dat()
{
    if(m_keys)
    {
        cjAniKeyVec3::PFDelete pfDelete = cjAniKeyVec3::GetDeleteKeyPF(m_keyType);
        pfDelete(m_keys);
    }
}

cjAniKeyVec3* cjAniVec3Dat::GetKeys(cjUINT32& keyCount, cjAniKeyVec3::EKEY_TYPE& keyType, cjBYTE& keySiz) const
{
    keyCount = m_keyCount;
    keyType = m_keyType;
    keySiz = m_keySiz;

    return m_keys;
}

void cjAniVec3Dat::ReplaceKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType)
{
    if(m_keys)
    {
        cjAniKeyVec3::PFDelete pfDelete = cjAniKeyVec3::GetDeleteKeyPF(m_keyType);
        pfDelete(m_keys);
    }

    SetKeys(keys, keyCount, keyType);
}

void cjAniVec3Dat::SetKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType)
{
    if(!keys || keyCount == 0)
    {
        m_keyCount = 0;
        m_keys = 0;
        m_keyType = cjAniKey::EKT_NOINTERP;
        m_keySiz = 0;

		return;
    }

    m_keySiz = cjAniKeyVec3::GetKeySiz(keyType);
    m_keyCount = keyCount;
    m_keys = keys;
    m_keyType = keyType;
}

void cjAniVec3Dat::CacheKeys(cjFLOAT stTime, cjFLOAT enTime)
{
    cjAssert(m_keyCount != 0);

	cjAniKey* keys = (cjAniKey*)m_keys;
    cjAniKey::RecursiveGetKeysByRange(cjAniKey::EKVAL_VEC3, m_keyType, keys, m_keyCount, stTime, enTime);
    m_keys = (cjAniKeyVec3*)keys;
}

cjAniVec3DatRPtr cjAniVec3Dat::GetDat(cjFLOAT stTime, cjFLOAT enTime)
{
    cjAniVec3DatRPtr spNewDatVec3 = (cjAniVec3Dat*)_CloneAsCopy();

    if(m_keyCount > 0)
    {
        cjAniKey* newKeys = cjNULL;
        cjUINT32 newKeyCount = 0;
        cjAniKey::CloneKeysByRange(cjAniKey::EKVAL_VEC3, m_keyType, m_keys, m_keyCount, stTime, enTime, newKeys, newKeyCount);
        spNewDatVec3->ReplaceKeys((cjAniKeyVec3*)newKeys, newKeyCount, m_keyType);
    }

    return spNewDatVec3;
}


//
// cjAniTMDat
//

CJSER_IMPL_CLASS(cjAniTMDat, cjRef)

CJSER_IMPL_SERIALIZE_BEGIN(cjAniTMDat)

	if(ser.IsQuerySerObjAndStr())
		return;

	if(ser.IsLoading())
	{
		cjUINT32 keyCount;

		// scale
		{
			ser << keyCount;
			if(keyCount > 0)
			{			
				cjFStreamEnum(ser, m_scaleKeyType);

				m_scaleKeySiz = cjAniKeyVec3::GetKeySiz(m_scaleKeyType);
				cjAniKeyVec3::PFLoad pfLoad = cjAniKeyVec3::GetLoadKeyPF(m_scaleKeyType);
				cjAniKeyVec3* keys = (cjAniKeyVec3*)pfLoad(ser, keyCount);
				cjAniKeyVec3::PFFillDerivedVals pfFillDerived = cjAniKeyVec3::GetFillDerivedValsKeyPF(m_scaleKeyType);

				pfFillDerived(keys, keyCount, m_scaleKeySiz); 
				ReplaceScaleKeys(keys, keyCount, m_scaleKeyType);
			}
		}

		// rot
		{
			ser << keyCount;
			if(keyCount > 0)
			{			
				cjFStreamEnum(ser, m_rotKeyType);

				m_rotKeySiz = cjAniKeyRot::GetKeySiz(m_rotKeyType);
				cjAniKeyRot::PFLoad pfLoad = cjAniKeyRot::GetLoadKeyPF(m_rotKeyType);
				cjAniKeyRot* keys = (cjAniKeyRot*)pfLoad(ser, keyCount);
				cjAniKeyRot::PFFillDerivedVals pfFillDerived = cjAniKeyRot::GetFillDerivedValsKeyPF(m_rotKeyType);
				pfFillDerived(keys, keyCount, m_rotKeySiz);
				ReplaceRotKeys(keys, keyCount, m_rotKeyType);
			}
		}

		// pos
		{
			ser << keyCount;
			if(keyCount > 0)
			{			
				cjFStreamEnum(ser, m_posKeyType);

				m_posKeySiz = cjAniKeyVec3::GetKeySiz(m_posKeyType);
				cjAniKeyVec3::PFLoad pfLoad = cjAniKeyVec3::GetLoadKeyPF(m_posKeyType);
				cjAniKeyVec3* keys = (cjAniKeyVec3*)pfLoad(ser, keyCount);
				cjAniKeyVec3::PFFillDerivedVals pfFillDerived = cjAniKeyVec3::GetFillDerivedValsKeyPF(m_posKeyType);

				pfFillDerived(keys, keyCount, m_posKeySiz);
				ReplacePosKeys(keys, keyCount, m_posKeyType);
			}
		}

	}
	else if(ser.IsSaving())
	{
		// scale
		{
			ser << m_scaleKeyCount;
			if(m_scaleKeyCount > 0)
			{
				cjFStreamEnum(ser, m_scaleKeyType);
				cjAniKeyVec3::PFSave pfSave = cjAniKeyVec3::GetSaveKeyPF(m_scaleKeyType);
				pfSave(ser, m_scaleKeys, m_scaleKeyCount);
			}
		}

		// rot
		{
			ser << m_rotKeyCount;
			if(m_rotKeyCount > 0)
			{
				cjFStreamEnum(ser, m_rotKeyType);
				cjAniKeyRot::PFSave pfSave = cjAniKeyRot::GetSaveKeyPF(m_rotKeyType);
				pfSave(ser, m_rotKeys, m_rotKeyCount);
			}
		}

		// pos
		{
			ser << m_posKeyCount;
			if(m_posKeyCount > 0)
			{
				cjFStreamEnum(ser, m_posKeyType);
				cjAniKeyVec3::PFSave pfSave = cjAniKeyVec3::GetSaveKeyPF(m_posKeyType);
				pfSave(ser, m_posKeys, m_posKeyCount);
			}
		}
	}

CJSER_IMPL_SERIALIZE_END(cjAniTMDat)

cjSerializable* cjAniTMDat::_CloneAsCopy()
{
	cjAniTMDat* d = new cjAniTMDat;

	// scale
	d->m_scaleKeyCount	= m_scaleKeyCount;
	d->m_scaleKeyType	= m_scaleKeyType;
	d->m_scaleKeySiz	= m_scaleKeySiz;
	d->m_scaleKeys		= cjNULL;

	if(m_scaleKeyCount)
	{
		cjAniKeyVec3::PFCreate createFn = cjAniKeyVec3::GetCreateKeyPF(m_scaleKeyType);
		cjAniKeyVec3* dstScaleKeys = (cjAniKeyVec3*)createFn(m_scaleKeyCount);
		cjAniKeyVec3::PFCopy copyFn = cjAniKeyVec3::GetCopyKeyPF(m_scaleKeyType);

		for(cjUINT32 i=0; i<m_scaleKeyCount; i++)
			copyFn(dstScaleKeys->GetKey(i, d->m_scaleKeySiz), m_scaleKeys->GetKey(i, m_scaleKeySiz));

		d->ReplaceScaleKeys(dstScaleKeys, m_scaleKeyCount, m_scaleKeyType);
	}

	// rotation
	d->m_rotKeyCount	= m_rotKeyCount;
	d->m_rotKeyType		= m_rotKeyType;
	d->m_rotKeySiz		= m_rotKeySiz;
	d->m_rotKeys		= cjNULL;

	if(m_rotKeyCount)
	{
		cjAniKeyRot::PFCreate createFn = cjAniKeyRot::GetCreateKeyPF(m_rotKeyType);
		cjAniKeyRot* dstRotKeys = (cjAniKeyRot*)createFn(m_rotKeyCount);
		cjAniKeyRot::PFCopy copyFn = cjAniKeyRot::GetCopyKeyPF(m_rotKeyType);

		for(cjUINT32 i=0; i<m_rotKeyCount; i++)
			copyFn(dstRotKeys->GetKey(i, d->m_rotKeySiz), m_rotKeys->GetKey(i, m_rotKeySiz));

		d->ReplaceRotKeys(dstRotKeys, m_rotKeyCount, m_rotKeyType);
	}
	
	// pos
	d->m_posKeyCount	= m_posKeyCount;
	d->m_posKeyType		= m_posKeyType;
	d->m_posKeySiz		= m_posKeySiz;
	d->m_posKeys		= cjNULL;

	if(m_posKeyCount)
	{
		cjAniKeyVec3::PFCreate createFn = cjAniKeyVec3::GetCreateKeyPF(m_posKeyType);
		cjAniKeyVec3* dstPosKeys = (cjAniKeyVec3*)createFn(m_posKeyCount);
		cjAniKeyVec3::PFCopy copyFn = cjAniKeyVec3::GetCopyKeyPF(m_posKeyType);

		for(cjUINT32 i=0; i<m_posKeyCount; i++)
			copyFn(dstPosKeys->GetKey(i, d->m_posKeySiz), m_posKeys->GetKey(i, m_posKeySiz));

		d->ReplacePosKeys(dstPosKeys, m_posKeyCount, m_posKeyType);
	}

	return d;
}


cjAniTMDat::cjAniTMDat()
{
	m_scaleKeyCount = 0;
	m_scaleKeys	= 0;
	m_scaleKeyType	= cjAniKey::EKT_NOINTERP;
	m_scaleKeySiz	= 0;

	m_rotKeyCount	= 0;
	m_rotKeys		= 0;
	m_rotKeyType	= cjAniKey::EKT_NOINTERP;
	m_rotKeySiz		= 0;

	m_posKeyCount	= 0;
	m_posKeys		= 0;
	m_posKeyType	= cjAniKey::EKT_NOINTERP;
	m_posKeySiz		= 0;
}

cjAniTMDat::~cjAniTMDat()
{
	if(m_scaleKeys)
	{
		cjAniKeyVec3::PFDelete pfnScaleKeyDelete = cjAniKeyVec3::GetDeleteKeyPF(m_scaleKeyType);
		pfnScaleKeyDelete(m_scaleKeys);
	}

	CleanRotKeys(m_rotKeys, m_rotKeyCount, m_rotKeyType);

	if(m_posKeys)
	{
		cjAniKeyVec3::PFDelete pfPosKeyDelete = cjAniKeyVec3::GetDeleteKeyPF(m_posKeyType);
		pfPosKeyDelete(m_posKeys);
	}
}

cjAniKeyVec3* cjAniTMDat::GetScaleKeys(cjUINT32& rstKeyCount, cjAniKeyVec3::EKEY_TYPE& rstKeyType, 
	cjBYTE& rstKeySiz) const
{
	rstKeyCount = m_scaleKeyCount;
	rstKeyType	= m_scaleKeyType;
	rstKeySiz	= m_scaleKeySiz;

	return m_scaleKeys;
}

void cjAniTMDat::ReplaceScaleKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType)
{
	if(m_scaleKeys)
    {
        cjAniKeyVec3::PFDelete pfnDeleteFloatKey = cjAniKeyVec3::GetDeleteKeyPF(m_scaleKeyType);
        pfnDeleteFloatKey(m_scaleKeys);
    }

    SetScaleKeys(keys, keyCount, keyType);
}

void cjAniTMDat::SetScaleKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType)
{
    if(!keys || keyCount == 0)
    {
        m_scaleKeys	= 0;
		m_scaleKeyCount = 0;
        m_scaleKeySiz	= 0;
        m_scaleKeyType	= cjAniKey::EKT_NOINTERP;

        return;
    }

	m_scaleKeys		= keys;
    m_scaleKeyCount	= keyCount;
	m_scaleKeyType	= keyType;
    m_scaleKeySiz	= cjAniKeyVec3::GetKeySiz(keyType);
}

cjAniKeyRot* cjAniTMDat::GetRotKeys(cjUINT32& rstKeyCount, cjAniKeyRot::EKEY_TYPE& rstKeyType, cjBYTE& rstKeySiz) const
{
	rstKeyCount	= m_rotKeyCount;
	rstKeyType	= m_rotKeyType;
	rstKeySiz	= m_rotKeySiz;

	return m_rotKeys;
}

void cjAniTMDat::ReplaceRotKeys(cjAniKeyRot* keys, cjUINT32 keyCount, cjAniKeyRot::EKEY_TYPE keyType)
{	
	CleanRotKeys(m_rotKeys, m_rotKeyCount, m_rotKeyType); // clear old keys	
	SetRotKeys(keys, keyCount, keyType); // set new keys
}

void cjAniTMDat::SetRotKeys(cjAniKeyRot* keys, cjUINT32 keyCount, cjAniKeyRot::EKEY_TYPE keyType)
{
	if(!keys || keyCount == 0)
	{
		m_rotKeys		= 0;
		m_rotKeyCount	= 0;
		m_rotKeyType	= cjAniKey::EKT_NOINTERP;
		m_rotKeySiz		= 0;

		return;
	}

	m_rotKeys		= keys;
	m_rotKeyCount	= keyCount;	
	m_rotKeyType	= keyType;

	m_rotKeySiz = cjAniKeyRot::GetKeySiz(keyType);
}

cjAniKeyVec3* cjAniTMDat::GetPosKeys(cjUINT32& rstKeyCount, cjAniKeyVec3::EKEY_TYPE& rstKeyType, 
	cjBYTE& rstKeySiz) const
{
	rstKeyCount	= m_posKeyCount;
	rstKeyType	= m_posKeyType;
	rstKeySiz	= m_posKeySiz;

	return m_posKeys;
}

void cjAniTMDat::ReplacePosKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType)
{
    if(m_posKeys)
    {
        cjAniKeyVec3::PFDelete pfVec3Delete = cjAniKeyVec3::GetDeleteKeyPF(m_posKeyType);
        pfVec3Delete(m_posKeys);
    }

    SetPosKeys(keys, keyCount, keyType);
}

void cjAniTMDat::SetPosKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType)
{
	if(!keys || keyCount == 0)
    {
		m_posKeyCount	= 0;
		m_posKeys		= 0;
		m_posKeyType	= cjAniKey::EKT_NOINTERP;
		m_posKeySiz		= 0;

		return;
    }

	m_posKeyCount	= keyCount;
	m_posKeys		= keys;
	m_posKeyType	= keyType;
	m_posKeySiz		= cjAniKeyVec3::GetKeySiz(keyType);
}

void cjAniTMDat::CacheKeys(cjFLOAT stTime, cjFLOAT enTime)
{
	cjAniKey* keys;

    // scale keys
    if(m_scaleKeyCount != 0)
    {
        keys = (cjAniKey*)m_scaleKeys;
        cjAniKey::RecursiveGetKeysByRange(cjAniKey::EKVAL_VEC3, m_scaleKeyType, keys, m_scaleKeyCount, stTime, enTime);
        m_scaleKeys = (cjAniKeyVec3*)keys;
    }

	// rotation keys 
    if(m_rotKeyCount != 0)
    {
        keys = (cjAniKey*)m_rotKeys;
        cjAniKey::RecursiveGetKeysByRange(cjAniKey::EKVAL_ROT, m_rotKeyType, keys, m_rotKeyCount, stTime, enTime);
        m_rotKeys = (cjAniKeyRot*)keys;
    }

    // position keys
    if(m_posKeyCount != 0)
    {
        keys = (cjAniKey*)m_posKeys;
        cjAniKey::RecursiveGetKeysByRange(cjAniKey::EKVAL_VEC3, m_posKeyType, keys, m_posKeyCount, stTime, enTime);
        m_posKeys = (cjAniKeyVec3*)keys;
    }
}

cjAniTMDatRPtr cjAniTMDat::GetDat(cjFLOAT stTime, cjFLOAT enTime)
{
	cjAniTMDatRPtr spNewDatTM = new cjAniTMDat;

	cjAniKey* newKeyAry = cjNULL;
	cjUINT32 newKeyCount = 0;

	if(m_scaleKeyCount > 0)
	{
		cjAniKey::CloneKeysByRange(cjAniKey::EKVAL_VEC3, m_scaleKeyType, m_scaleKeys, m_scaleKeyCount, 
			stTime, enTime, newKeyAry, newKeyCount);
		spNewDatTM->ReplaceScaleKeys((cjAniKeyVec3*)newKeyAry, newKeyCount, m_scaleKeyType);
	}

	if(m_rotKeyCount > 0)
	{
		cjAniKey::CloneKeysByRange(cjAniKey::EKVAL_ROT, m_rotKeyType, m_rotKeys, m_rotKeyCount, 
			stTime, enTime, newKeyAry, newKeyCount);
		spNewDatTM->ReplaceRotKeys((cjAniKeyRot*)newKeyAry, newKeyCount, m_rotKeyType);
	}

	if(m_posKeyCount > 0)
	{
		cjAniKey::CloneKeysByRange(cjAniKey::EKVAL_VEC3, m_posKeyType, m_posKeys, m_posKeyCount,
			stTime, enTime, newKeyAry, newKeyCount);
		spNewDatTM->ReplacePosKeys((cjAniKeyVec3*)newKeyAry, newKeyCount, m_posKeyType);
	}

	return spNewDatTM;
}

void cjAniTMDat::CleanRotKeys(cjAniKeyRot* keys, cjUINT32 rotKeyCount, cjAniKeyRot::EKEY_TYPE keyType)
{
	if(keys && keyType == cjAniKeyRot::EKT_EULERKEY)
	{
        ((cjAniKeyEulerRot*)keys->GetKey(0, m_rotKeySiz))->CleanUp();
	}

    if(keys)
    {
        cjAniKeyRot::PFDelete pfnRotKeyDelete = cjAniKeyRot::GetDeleteKeyPF(m_rotKeyType);
        pfnRotKeyDelete(keys);
    }
}


//
// cjAniColorDat
//

CJSER_IMPL_CLASS(cjAniColorDat, cjRefObj)

CJSER_IMPL_SERIALIZE_BEGIN(cjAniColorDat)

	if(ser.IsQuerySerObjAndStr())
		return;

	if(ser.IsLoading())
	{
		cjUINT32 keyCount;
		ser << keyCount;

		if(keyCount > 0)
		{			
			cjFStreamEnum(ser, m_keyType);

			m_keySiz = cjAniKeyColor::GetKeySiz(m_keyType);
			cjAniKeyColor::PFLoad pfLoad = cjAniKeyColor::GetLoadKeyPF(m_keyType);
			cjAniKeyColor* keys = (cjAniKeyColor*)pfLoad(ser, keyCount);

			ReplaceKeys(keys, keyCount, m_keyType);
		}
	}
	else if(ser.IsSaving())
	{
		ser << m_keyCount;

		if(m_keyCount > 0)
		{
			cjFStreamEnum(ser, m_keyType);

			cjAniKeyColor::PFSave pfSave = cjAniKeyColor::GetSaveKeyPF(m_keyType);
			pfSave(ser, m_keys, m_keyCount);
		}
	}

CJSER_IMPL_SERIALIZE_END(cjAniColorDat)

cjSerializable* cjAniColorDat::_CloneAsCopy()
{
	cjAniColorDat* d = new cjAniColorDat;

	d->m_keyCount = m_keyCount;
	d->m_keyType = m_keyType;
	d->m_keySiz = m_keySiz;
	d->m_keys = cjNULL;

	if(m_keyCount)
	{
		cjAniKeyColor::PFCreate pfCreate = cjAniKeyColor::GetCreateKeyPF(m_keyType);
		cjAniKeyColor* dstKeys = (cjAniKeyColor*)pfCreate(m_keyCount);
		cjAniKeyColor::PFCopy pfCopy = cjAniKeyColor::GetCopyKeyPF(m_keyType);

		for(cjUINT32 i=0; i<m_keyCount; i++)
			pfCopy(dstKeys->GetKey(i, d->m_keySiz), m_keys->GetKey(i, m_keySiz));

		d->ReplaceKeys(dstKeys, m_keyCount, m_keyType);
	}

	return d;
}

cjAniColorDat::cjAniColorDat()
{
    m_keyCount = 0;
    m_keys = 0;
    m_keyType = cjAniKey::EKT_NOINTERP;
    m_keySiz = 0;
}

cjAniColorDat::~cjAniColorDat()
{
    if(m_keys)
    {
        cjAniKeyColor::PFDelete pfDelete = cjAniKeyColor::GetDeleteKeyPF(m_keyType);
        pfDelete(m_keys);
    }
}

cjAniKeyColor* cjAniColorDat::GetKeys(cjUINT32& keyCount, cjAniKeyColor::EKEY_TYPE& keyType, cjBYTE& keySiz) const
{
    keyCount = m_keyCount;
    keyType = m_keyType;
    keySiz = m_keySiz;

    return m_keys;
}

void cjAniColorDat::ReplaceKeys(cjAniKeyColor* keys, cjUINT32 keyCount, cjAniKeyColor::EKEY_TYPE keyType)
{
    if(m_keys)
    {
        cjAniKeyColor::PFDelete pfDelete = cjAniKeyColor::GetDeleteKeyPF(m_keyType);
        pfDelete(m_keys);
    }

    SetKeys(keys, keyCount, keyType);
}

void cjAniColorDat::SetKeys(cjAniKeyColor* keys, cjUINT32 keyCount, cjAniKeyColor::EKEY_TYPE keyType)
{
    if(!keys || keyCount == 0)
    {
        m_keyCount = 0;
        m_keys = 0;
        m_keyType = cjAniKey::EKT_NOINTERP;
        m_keySiz = 0;

		return;
    }

    m_keySiz = cjAniKeyColor::GetKeySiz(keyType);
    m_keyCount = keyCount;
    m_keys = keys;
    m_keyType = keyType;
}

void cjAniColorDat::CacheKeys(cjFLOAT stTime, cjFLOAT enTime)
{
    cjAssert(m_keyCount != 0);

	cjAniKey* keys = (cjAniKey*)m_keys;
    cjAniKey::RecursiveGetKeysByRange(cjAniKey::EKVAL_COLOR, m_keyType, keys, m_keyCount, stTime, enTime);
    m_keys = (cjAniKeyColor*)keys;
}

cjAniColorDatRPtr cjAniColorDat::GetDat(cjFLOAT stTime, cjFLOAT enTime)
{
    cjAniColorDatRPtr spNewDatColor = (cjAniColorDat*)_CloneAsCopy();

    if(m_keyCount > 0)
    {
        cjAniKey* newKeys = cjNULL;
        cjUINT32 newKeyCount = 0;
        cjAniKey::CloneKeysByRange(cjAniKey::EKVAL_COLOR, m_keyType, m_keys, m_keyCount, stTime, enTime, newKeys, newKeyCount);
        spNewDatColor->ReplaceKeys((cjAniKeyColor*)newKeys, newKeyCount, m_keyType);
    }

    return spNewDatColor;
}




