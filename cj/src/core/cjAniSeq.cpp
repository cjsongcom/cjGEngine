
#include "cjCorePCH.h"
#include "cjAniSeq.h"


//
// cjAniSeq
//

CJSER_IMPL_CLASS(cjAniSeq, cjRef)

CJSER_IMPL_SERIALIZE_BEGIN(cjAniSeq)

	if(ser.IsQuerySerObjAndStr())
	{
		ser << m_aniNote;
		cjAniEvaluator::SerializePtrAry(ser, &m_evaluatorPtrAry, &m_maxEvaluatorCount);
	}
	else if(ser.IsSaving() || ser.IsLoading())
	{
		ser << m_name;
		ser << m_accumRootName;

		ser << m_duration;
		ser << m_loopType;
		ser << m_frequency;

		ser.SerializeSerObjPtr(&m_aniNote);

		ser << m_maxEvaluatorCount;
		cjAniEvaluator::SerializePtrAry(ser, &m_evaluatorPtrAry, &m_evaluatorCount);

		ser << m_flag;	
	}

CJSER_IMPL_SERIALIZE_END(cjAniSeq)

CJSER_IMPL_SERIALIZE_POST_BEGIN(cjAniSeq)

	if(ser.IsLoading())
	{
		ser.PostSerializeSerObjPtr(m_aniNote); 
	}

CJSER_IMPL_SERIALIZE_POST_END(cjAniSeq)


CJSER_IMPL_SERIALIZE_ARY(cjAniSeq)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjAniSeq)

	d->m_name = m_name;
	d->m_accumRootName = m_accumRootName;
	d->m_duration = m_duration;
	d->m_loopType = m_loopType;
	d->m_frequency = m_frequency;

	d->m_aniNote = m_aniNote;

	d->m_maxEvaluatorCount = m_maxEvaluatorCount;
	d->m_evaluatorCount = m_evaluatorCount;
	d->m_evaluatorPtrAry = cjNULL;
	
	if(m_evaluatorCount > 0)
	{
		cjINT32 copyEvalCount = m_evaluatorCount;
		d->m_evaluatorPtrAry = new cjAniEvaluatorRPtr[copyEvalCount];

		for(cjINT32 i=0; i<copyEvalCount; i++)
			d->m_evaluatorPtrAry[i] = m_evaluatorPtrAry[i];
	}
	
	d->m_activationCBArySiz = m_activationCBArySiz;
	d->m_activationCBCount = m_activationCBCount;
	d->m_activationCBs = cjNULL;

	cjUINT32 copyCBCount = m_activationCBCount;
	if(copyCBCount)
	{
		d->m_activationCBs = new cjAniSeqCBItem[copyCBCount];

		for(cjUINT32 i=0; i<copyCBCount; i++)
			d->m_activationCBs[i] = m_activationCBs[i];
	}

	d->m_flag = m_flag;

CJSER_IMPL_CLONE_PROPCOPY_END(cjAniSeq)

CJSER_IMPL_CLONE(cjAniSeq)


void cjAniSeq::_InitCommon()
{
	m_duration						= 0; 
    m_loopType						= cjAniCntr::ANILOOP_LOOP;
	m_frequency						= 1.0f; 

	m_aniNote						= cjNULL; 

    m_maxEvaluatorCount				= 0;
	m_evaluatorCount				= 0;
	m_evaluatorPtrAry				= cjNULL; 

    m_activationCBArySiz			= 0;
	m_activationCBCount				= 0;
    m_activationCBs					= cjNULL;

	m_flag							= 0; 
}

cjAniSeq::cjAniSeq(const cjName& name, cjUINT32 maxEvaluatorCount)
{
	_InitCommon();

    SetName(name);
	m_accumRootName = cjName::NullName;

    if(maxEvaluatorCount > 0)
		SetMaxEvaluatorCount(maxEvaluatorCount);
}

cjAniSeq::cjAniSeq()
{
	_InitCommon();
}

cjAniSeq::~cjAniSeq()
{
	if(m_activationCBs)
	{
		for(cjUINT32 i=0; i<m_activationCBCount; i++)
			m_activationCBs[i].ClearValues();

		delete [] m_activationCBs;
		m_activationCBs = cjNULL;
	}
   	
    m_activationCBArySiz = 0;
    m_activationCBCount = 0;

	m_aniNote = cjNULL;

	if(m_evaluatorPtrAry)
	{
		delete [] m_evaluatorPtrAry;
		m_evaluatorPtrAry = cjNULL;
	}

	m_evaluatorCount = 0;
	m_maxEvaluatorCount = 0;
}

// activation callback
cjFLOAT cjAniSeq::CalcTime(cjFLOAT etime)
{
	cjFLOAT scaledTime = etime * m_frequency;

	switch(m_loopType)
	{
	case cjAniCntr::ANILOOP_LOOP:
		{
			if(m_duration != 0.0f)
			{
				if(scaledTime != m_duration)
					scaledTime = cjFMod(scaledTime, m_duration);

				if(scaledTime < 0.0f)
					scaledTime += m_duration;
			}
			else
			{
				scaledTime = 0.0f;
			}			
		}
		break;

	case cjAniCntr::ANILOOP_ONE:
		break;

	default:
		break;
	}

	if(scaledTime > m_duration)
		scaledTime = m_duration;
	else if(scaledTime < 0.0f)
		scaledTime = 0.0f;

	return scaledTime;
}

cjFLOAT cjAniSeq::GetAniNoteTime(const cjNameChar* noteName) const
{
	if(!noteName)
		return CJ_FTIME_INVALID;

	cjINT32 noteCount;
	cjAniNote* notes = m_aniNote->GetNoteAry(&noteCount);

	for(cjINT32 i=0; i<noteCount; i++)
	{
		cjAniNote& cur = notes[i];

		if(!cjStrcmp(cur.GetNote(), noteName))
			return cur.GetTime();
	}

	return CJ_FTIME_INVALID;
}

cjFLOAT cjAniSeq::GetAniNoteTime(cjNameMatch* match) const
{
	if(!match || !m_aniNote)
		return CJ_FTIME_INVALID;

	cjINT32 noteCount;
	cjAniNote* notes = m_aniNote->GetNoteAry(&noteCount);

	for(cjINT32 i=0; i<noteCount; i++)
	{
		cjAniNote& cur = notes[i];
		if(match->IsMatch(cur.GetNote()))
			return cur.GetTime();
	}

	return CJ_FTIME_INVALID;
}

void cjAniSeq::AddActivationCB(cjAniSeqCB* cb, cjAniCntrManager* man)
{
    if(m_activationCBCount >= m_activationCBArySiz)
        AdjustActvationCBArySiz(m_activationCBCount + 3); // x3

    cjAniSeqCBItem& cbItem	= m_activationCBs[m_activationCBCount];

    cbItem.m_cb				= cb;
    cbItem.m_man			= man;

    m_activationCBCount++;
}

void cjAniSeq::RemoveActivationCB(cjAniSeqCB* cb, cjAniCntrManager* man)
{
	cjUINT32 i=0;

	while(i < m_activationCBCount)
    {
		cjAniSeqCBItem& curItem = m_activationCBs[i];

        if(curItem.m_man == man && (!cb || curItem.m_cb == cb))
        {
            curItem = m_activationCBs[m_activationCBCount-1];
            m_activationCBs[m_activationCBCount-1].ClearValues();
            m_activationCBCount--;
        }
        else
            i++;
    }
}

void cjAniSeq::GetActivationCB(cjUINT32 index, cjAniSeqCB*& cb, cjAniCntrManager*& man)
{
	cjAniSeqCBItem& cbItem = m_activationCBs[index];

	cb	= cbItem.m_cb;
	man	= cbItem.m_man;
}


//
// Evaluators
//

cjUINT32 cjAniSeq::AddEvaluator(cjAniEvaluator* evaluator)
{
	cjAssert(evaluator);

	m_evaluatorCount++;

	if(m_evaluatorCount >= m_maxEvaluatorCount)
        SetMaxEvaluatorCount(m_evaluatorCount);

	m_evaluatorPtrAry[m_evaluatorCount-1] = evaluator;

	return m_evaluatorCount - 1;
}

cjAniEvaluatorRPtr cjAniSeq::RemoveEvaluator(cjUINT32 index)
{
	cjAniEvaluatorRPtr evaluator = m_evaluatorPtrAry[index];

	// move elements of after index 
    for(cjINT32 i=index+1; i<m_evaluatorCount; i++)
        m_evaluatorPtrAry[i-1] = m_evaluatorPtrAry[i];

	m_evaluatorCount--;
	m_evaluatorPtrAry[m_evaluatorCount] = cjNULL;

    return evaluator;
}

void cjAniSeq::SetMaxEvaluatorCount(cjINT32 maxCount)
{
	cjAssert(maxCount > 0);

	cjAniEvaluatorRPtr* newEvaluatorRPtrAry = new cjAniEvaluatorRPtr[maxCount];

	cjUINT32 copyCount = (m_maxEvaluatorCount < maxCount) ? m_maxEvaluatorCount : maxCount;

    for(cjUINT32 i=0; i<copyCount; i++)
		newEvaluatorRPtrAry[i] = m_evaluatorPtrAry[i];

	delete [] m_evaluatorPtrAry;

    m_maxEvaluatorCount = maxCount;
    m_evaluatorPtrAry = newEvaluatorRPtrAry;
}

void cjAniSeq::SortEvaluatorsByPBChannelType()
{
	if(m_evaluatorCount < 2)
        return;

	cjAniEvaluator** tmpEvaluators	 = (cjAniEvaluator**)cjMemAlloc(sizeof(cjAniEvaluator*) * m_evaluatorCount);
	cjBYTE*			 tmpEvaluatorGrp = (cjBYTE*)cjMemAlloc(sizeof(cjBYTE) * m_evaluatorCount);
		
	cjINT32 tmpEvaluatorCount = 0; // temporary copy
	
	for(cjINT32 i=0; i<m_evaluatorCount; i++)
    {
        cjAniEvaluator* evaluator = m_evaluatorPtrAry[i];

        if(!evaluator)
            continue;

        // default
        cjAniEvalGrp resolvedEvaluatorGrp = cjAniEvalGrp_Referenced;

        if(evaluator->IsTransformEvaluator())
        {
			// must have 3 evaluator channel(pos,rot,scale)
            cjAssert(evaluator->GetMaxChannels() == 3);

            if( evaluator->IsChannelItemInvalid(0) && 
				evaluator->IsChannelItemInvalid(1) && 
				evaluator->IsChannelItemInvalid(2))
			{
                resolvedEvaluatorGrp = cjAniEvalGrp_Invalid;
			}
            else if(!evaluator->IsReferenced())
			{
                resolvedEvaluatorGrp = cjAniEvalGrp_Transform;
			}
        }
        else
        {
			// 1:normal evaluator
            cjAssert(evaluator->GetMaxChannels() == 1); 

            if(evaluator->IsChannelItemInvalid(0))
			{
				resolvedEvaluatorGrp = cjAniEvalGrp_Referenced;
			}
            else if(!evaluator->IsReferenced())
            {
				cjAniPoseBufChanItem pbChanItem = evaluator->GetPBChannelItemFromChanItemWithNoCareFlag(0);

                if	   (pbChanItem == cjAniPoseBufChanItem_Color)	resolvedEvaluatorGrp = cjAniEvalGrp_Color;
                else if(pbChanItem == cjAniPoseBufChanItem_Bool)	resolvedEvaluatorGrp = cjAniEvalGrp_Bool;
                else if(pbChanItem == cjAniPoseBufChanItem_Float)	resolvedEvaluatorGrp = cjAniEvalGrp_Float;
                else if(pbChanItem == cjAniPoseBufChanItem_Vec3S)	resolvedEvaluatorGrp = cjAniEvalGrp_Vec3S;
				else if(pbChanItem == cjAniPoseBufChanItem_Vec3P)	resolvedEvaluatorGrp = cjAniEvalGrp_Vec3P;
                else { 
						cjAssert(pbChanItem == cjAniPoseBufChanItem_Rot); // must be cjAniPoseBufChanItem_Rot, by order
						resolvedEvaluatorGrp = cjAniEvalGrp_Rot;	
				}
            }
        }

        cjAssert(resolvedEvaluatorGrp != cjAniEvalGrp_Invalid);

        tmpEvaluators[tmpEvaluatorCount]	= evaluator;
        tmpEvaluatorGrp[tmpEvaluatorCount]	= (cjBYTE)resolvedEvaluatorGrp;
        tmpEvaluatorCount++;

        evaluator->AddRef();
        m_evaluatorPtrAry[i] = cjNULL;
    }
	
	//
	// rebuild evaluator order
	//

    m_evaluatorCount = 0;

    for(cjUINT32 resolvedEvaluatorGrp=cjAniEvalGrp_Color; resolvedEvaluatorGrp <= 8; resolvedEvaluatorGrp++)
    {
        for(cjINT32 i=0; i<tmpEvaluatorCount; i++)
        {
            if(tmpEvaluatorGrp[i] == resolvedEvaluatorGrp)
            {
                m_evaluatorPtrAry[m_evaluatorCount] = tmpEvaluators[i];
                m_evaluatorCount++;

				tmpEvaluators[i]->ReleaseRef();
                tmpEvaluators[i] = cjNULL;
            }
        }
    }

	cjMemDeAlloc(tmpEvaluators);
	cjMemDeAlloc(tmpEvaluatorGrp);
}

cjUINT32 cjAniSeq::GetScratchPadBufSiz(cjBool isCumulativeAnimations, cjRefObjNameHash* sceneNodeHash, 
	cjUINT32& uniqueBasisDatCount) const
{
    cjBSplineBasisF3Dat* uniqueBasisDat[MAX_UNIQUE_BASIS_DAT];
    uniqueBasisDatCount = 0;

    cjUINT32 spadBlockItemsCount[cjAniSPADB_Max];

    for(cjUINT32 i=0; i<cjAniSPADB_Max; i++)
        spadBlockItemsCount[i] = 0;

    cjUINT32 scratchPadSiz = 0;

    for(cjINT32 i=0; i<m_evaluatorCount; i++)
    {
        cjAniEvaluator* evaluator = m_evaluatorPtrAry[i];

        cjUINT32 maxEvalChannels = evaluator->GetMaxChannels();
        cjBool forceAlwaysUpdate = cjFalse;

        if(isCumulativeAnimations)
		{
			if(maxEvalChannels == 3 &&	// is transform evaluator
				m_accumRootName.IsValid() && evaluator->GetID() == m_accumRootName)
			{
				// transform evaluator and accumRoot is exist
				forceAlwaysUpdate = cjTrue;
			}
		}

        cjBool alreadyShared = cjFalse;
        for(cjUINT32 k=0; k<maxEvalChannels; k++)
        {
            cjUINT32 fillSiz;
            cjBool isSharedFillDat;

            cjAniSPadBlock spadBlkSegDat;
            cjBSplineBasisF3Dat* basisDat;

            if(evaluator->GetSPadInfo(k, forceAlwaysUpdate, sceneNodeHash,
				fillSiz, isSharedFillDat, spadBlkSegDat, basisDat))
            {
                if(!alreadyShared)
                {
                    scratchPadSiz += fillSiz;

                    if(isSharedFillDat)
                        alreadyShared = cjTrue;
                }

                spadBlockItemsCount[cjAniSPADB_EvaluatorDat]++;
                spadBlockItemsCount[spadBlkSegDat]++;

                if(basisDat)
                {
                    cjBool found = cjFalse;

                    for(cjUINT32 m=0; m<uniqueBasisDatCount; m++)
                    {
                        if(basisDat == uniqueBasisDat[m])
                        {
                            found = cjTrue;
                            break; // Found a match.
                        }
                    }

                    // Add the new basis data to the array, if there's room.
                    if(!found)
                    {
                        if(uniqueBasisDatCount < MAX_UNIQUE_BASIS_DAT)
                            uniqueBasisDat[uniqueBasisDatCount] = basisDat;

						uniqueBasisDatCount++;
                    }
                }
            }
        }
    }

    scratchPadSiz += spadBlockItemsCount[cjAniSPADB_EvaluatorDat]		* sizeof(cjAniEvalSPDat);

    scratchPadSiz += uniqueBasisDatCount								* sizeof(cjBSplineBasisF3Dat);

		// Linear/BSpline Color
	scratchPadSiz += spadBlockItemsCount[cjAniSPADB_LinearColorSeg]		* sizeof(cjAniSPad::LColorSeg);
    scratchPadSiz += spadBlockItemsCount[cjAniSPADB_BSColorSeg]			* sizeof(cjAniSPad::BSColorSeg);
    																			 
	scratchPadSiz += spadBlockItemsCount[cjAniSPADB_CBoolSeg]			* sizeof(cjAniSPad::CBoolSeg);
    																			 
		// Cubic/BSpline Float													 
	scratchPadSiz += spadBlockItemsCount[cjAniSPADB_CubicFloatSeg]		* sizeof(cjAniSPad::CubicFloatSeg);
    scratchPadSiz += spadBlockItemsCount[cjAniSPADB_BSFloatSeg]			* sizeof(cjAniSPad::BSFloatSeg);
    																			 
		// Cubic/BSpline Vec3S (scale)
	scratchPadSiz += spadBlockItemsCount[cjAniSPADB_CubicVec3SSeg]		* sizeof(cjAniSPad::CubicVec3Seg);
    scratchPadSiz += spadBlockItemsCount[cjAniSPADB_BSVec3SSeg]			* sizeof(cjAniSPad::BSVec3Seg);

		// Cubic/BSpline Vec3S (pos)
	scratchPadSiz += spadBlockItemsCount[cjAniSPADB_CubicVec3PSeg]		* sizeof(cjAniSPad::CubicVec3Seg);
    scratchPadSiz += spadBlockItemsCount[cjAniSPADB_BSVec3PSeg]			* sizeof(cjAniSPad::BSVec3Seg);

		// Slerp/Squad/BSpline  Rot												 
	scratchPadSiz += spadBlockItemsCount[cjAniSPADB_SlerpRotSeg]		* sizeof(cjAniSPad::SlerpRotSeg);
    scratchPadSiz += spadBlockItemsCount[cjAniSPADB_SquadRotSeg]		* sizeof(cjAniSPad::SquadRotSeg);
    scratchPadSiz += spadBlockItemsCount[cjAniSPADB_BSRotSeg]			* sizeof(cjAniSPad::BSRotSeg);

    cjAssert(cjAniSPADB_Max == 15);

    return scratchPadSiz;
}

void cjAniSeq::AdjustActvationCBArySiz(cjUINT32 cbArySiz)
{
	cjAssert(cbArySiz > 0 && cbArySiz >= m_activationCBCount);

    cjAniSeqCBItem* newAry = new cjAniSeqCBItem[cbArySiz];

    for(cjUINT32 i=0; i<m_activationCBCount; i++)
        newAry[i] = m_activationCBs[i];

    delete [] m_activationCBs;

    m_activationCBArySiz = cbArySiz;
    m_activationCBs = newAry;
}

cjBool cjAniSeq::CheckMatchMorphAniNote(cjAniCntrSeq* timeSyncCntrSeq)
{
	cjAssert(timeSyncCntrSeq);
	return CheckMatchMorphAniNote(timeSyncCntrSeq->GetAniNote());
}

cjBool cjAniSeq::CheckMatchMorphAniNote(cjExtDatAniNote* timeSyncAniNoteExtDat)
{
    if(!m_aniNote || !timeSyncAniNoteExtDat)
        return cjFalse;

    cjINT32 noteCountA, noteCountB;
	cjAniNote* noteAryA = timeSyncAniNoteExtDat->GetNoteAry(&noteCountA);
    cjAniNote* noteAryB = m_aniNote->GetNoteAry(&noteCountB);

    const cjNameChar* morphTextKey	= cjAniConstants::GetMorphTextKey();
    cjINT32 morphTextKeyLen			= cjAniConstants::GetMorphTextKeyLen();
    cjBool morphKeysExistA			= cjFalse;

    for(cjINT32 i=0; i<noteCountA; i++)
    {
        const cjNameChar* noteA = noteAryA[i].GetNote(); 
		cjAssert(noteA);

        if(!cjStrncmp(noteA, morphTextKey, morphTextKeyLen))
        {
            morphKeysExistA = cjTrue;
            cjBool found = cjFalse;

            for(cjINT32 k=0; k<noteCountB; k++)
            {
                const cjNameChar* noteB = noteAryB[k].GetNote();
                cjAssert(noteB);

                if(!cjStrcmp(noteA, noteB))
                {
                    found = cjTrue;
                    break;
                }
            }

            if(!found)
                return cjFalse;
        }
    }

    if(!morphKeysExistA)
        return cjFalse;

    cjBool morphKeysExistB = cjFalse;

    for(cjINT32 i=0; i<noteCountB; i++)
    {
        const cjNameChar* noteB = noteAryB[i].GetNote();
        cjAssert(noteB);

        if(cjStrncmp(noteB, morphTextKey, morphTextKeyLen))
        {
            morphKeysExistB = cjTrue;
            cjBool found = cjFalse;

            for(cjINT32 k=0; k<noteCountA; k++)
            {
                const cjNameChar* noteA = noteAryA[k].GetNote();
                cjAssert(noteA);

                if(!cjStrcmp(noteB, noteA))
                {
                    found = cjTrue;
                    break;
                }
            }

            if(!found)
                return cjFalse;
        }
    }

    if(!morphKeysExistB)
        return cjFalse;

    return cjTrue;
}

void cjAniSeq::GetAccumPos(cjBool& accumX, cjBool& accumY, cjBool& accumZ) const
{	
    accumX = m_flag & FL_ACCUM_XPOS ? cjTrue : cjFalse;
	accumY = m_flag & FL_ACCUM_YPOS ? cjTrue : cjFalse;
	accumZ = m_flag & FL_ACCUM_ZPOS ? cjTrue : cjFalse;
}

void cjAniSeq::SetAccumPos(cjBool accumX, cjBool accumY, cjBool accumZ)
{
	cjUINT32 flag = 0;

	if(accumX) flag |= FL_ACCUM_XPOS;
	if(accumY) flag |= FL_ACCUM_YPOS;
	if(accumZ) flag |= FL_ACCUM_ZPOS;

	m_flag &= ~(FL_ACCUM_XPOS | FL_ACCUM_YPOS | FL_ACCUM_ZPOS);
	m_flag |= flag;
}

void cjAniSeq::GetAccumRot(cjBool& accumX, cjBool& accumY, cjBool& accumZ) const
{
    accumX = m_flag & FL_ACCUM_XROT ? cjTrue : cjFalse;
	accumY = m_flag & FL_ACCUM_YROT ? cjTrue : cjFalse;
	accumZ = m_flag & FL_ACCUM_ZROT ? cjTrue : cjFalse;
}

void cjAniSeq::SetAccumRot(cjBool accumX, cjBool accumY, cjBool accumZ)
{
	cjUINT32 flag = 0;

	if(accumX) flag |= FL_ACCUM_XROT;
	if(accumY) flag |= FL_ACCUM_YROT;
	if(accumZ) flag |= FL_ACCUM_ZROT;

	m_flag &= ~(FL_ACCUM_XROT | FL_ACCUM_YROT | FL_ACCUM_ZROT);
	m_flag |= flag;
}

void cjAniSeq::GetAccumFrontFacing(cjBYTE& axis, cjBool& isNeg) const
{
    if(m_flag & FL_ACCUM_XFACING)		{ axis = 0; }
    else if(m_flag & FL_ACCUM_YFACING)	{ axis = 1; }
    else if(m_flag & FL_ACCUM_ZFACING)	{ axis = 2; }

    isNeg = (m_flag & FL_ACCUM_NEG_FACING) ? cjTrue : cjFalse;
}

void cjAniSeq::SetAccumFrontFacing(const cjBYTE axis, const cjBool isNeg)
{
	cjUINT32 mask = FL_ACCUM_XFACING | FL_ACCUM_YFACING | FL_ACCUM_ZFACING | FL_ACCUM_NEG_FACING;
	cjUINT32 flag = 0;
	
    switch(axis)
    {
        case 0: flag |= FL_ACCUM_XFACING; break;
        case 1: flag |= FL_ACCUM_YFACING; break;
        case 2: flag |= FL_ACCUM_ZFACING; break;
		default: break;

	}

	if(isNeg)
		flag |= FL_ACCUM_NEG_FACING;

	m_flag &= mask;
	m_flag |= flag;
}