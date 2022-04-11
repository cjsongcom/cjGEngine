
#include "cjCorePCH.h"
#include "cjResAni.h"


CJOBJ_IMPL_CLASS(cjResAniSeq)

CJOBJ_IMPL_SERIALIZE_BEGIN(cjResAniSeq)
	
	if(stream.IsObjOnly())
		return;
	
// this will be performed by cjSerObjRefProperty
//	cjSerObjStreamBroker ser(&stream);
//	ser.Serialize(m_seq);

CJOBJ_IMPL_SERIALIZE_END(cjResAniSeq)

CJOBJ_IMPL_STATIC_REG_BEGIN(cjResAniSeq)

	// create property in cjResAniSeq class (outer class)
	cjSerObjRefProperty* ser = new(GetCls(), _CT("seq"), (cjTFLAG)0) cjSerObjRefProperty(CJ_PROP_NATIVE_OFF(m_seq), 0, 0, CJRTTI(cjAniSeq));

CJOBJ_IMPL_STATIC_REG_END(cjResAniSeq)


cjResAniSeq::cjResAniSeq()
{

}

void cjResAniSeq::OnDestroy()
{
	cjSuper::OnDestroy();
}








//cjResAniSeq::cjResAniSeq()
//{
//	// skip zero
//	m_frequency	= 1.0f;
//}
//
//void cjResAniSeq::OnDestroy()
//{
//	m_aniNote = cjNULL;
//
//	if(m_evaluators)
//	{
//		delete [] m_evaluators;
//		m_evaluators = cjNULL;
//	}
//
//	cjSuper::OnDestroy();
//}
//
//cjFLOAT cjResAniSeq::CalcTime(cjFLOAT etime) const
//{
//	cjFLOAT scaledTime = etime * m_frequency;
//
//	switch(m_loopType)
//	{
//	case cjAniCntr::ANILOOP_LOOP:
//		{
//			if(m_duration != 0.0f)
//			{
//				if(scaledTime != m_duration)
//					scaledTime = cjFMod(scaledTime, m_duration);
//
//				if(scaledTime < 0.0f)
//					scaledTime += m_duration;
//			}
//			else
//			{
//				scaledTime = 0.0f;
//			}			
//		}
//		break;
//
//	case cjAniCntr::ANILOOP_ONE:
//		break;
//
//	default:
//		break;
//	}
//
//	if(scaledTime > m_duration)
//		scaledTime = m_duration;
//	else if(scaledTime < 0.0f)
//		scaledTime = 0.0f;
//
//	return scaledTime;
//}
//
//cjFLOAT cjResAniSeq::GetAniNoteTime(const cjNameChar* noteName) const
//{
//	if(!noteName)
//		return CJ_FTIME_INVALID;
//
//	cjINT32 noteCount;
//	cjAniNote* notes = m_aniNote->GetNoteAry(&noteCount);
//
//	for(cjINT32 i=0; i<noteCount; i++)
//	{
//		cjAniNote& cur = notes[i];
//
//		if(!cjStrcmp(*cur.GetNote(), noteName))
//			return cur.GetTime();
//	}
//
//	return CJ_FTIME_INVALID;
//}
//
//cjFLOAT cjResAniSeq::GetAniNoteTime(cjNameMatch* match) const
//{
//	if(!match || !m_aniNote)
//		return CJ_FTIME_INVALID;
//
//	cjINT32 noteCount;
//	cjAniNote* notes = m_aniNote->GetNoteAry(&noteCount);
//
//	for(cjINT32 i=0; i<noteCount; i++)
//	{
//		cjAniNote& cur = notes[i];
//		if(match->IsMatch(cur.GetNote()))
//			return cur.GetTime();
//	}
//
//	return CJ_FTIME_INVALID;
//}
//
//cjUINT32 cjResAniSeq::AddEvaluator(cjAniEvaluator* evaluator)
//{
//	cjAssert(evaluator);
//
//	m_evaluatorCount++;
//
//	if(m_evaluatorCount >= m_maxEvaluatorCount)
//        SetMaxEvaluatorCount(m_evaluatorCount);
//
//	m_evaluators[m_evaluatorCount-1] = evaluator;
//
//	return m_evaluatorCount - 1;
//}
//
//cjAniEvaluatorPtr cjResAniSeq::RemoveEvaluator(cjUINT32 index)
//{
//	cjAniEvaluatorPtr evaluator = m_evaluators[index];
//
//	// move elements of after index 
//    for(cjUINT32 i=index+1; i<m_evaluatorCount; i++)
//        m_evaluators[i-1] = m_evaluators[i];
//
//	m_evaluatorCount--;
//	m_evaluators[m_evaluatorCount] = cjNULL;
//
//    return evaluator;
//}
//
//void cjResAniSeq::SortEvaluatorsByPBChannelType()
//{
//	if(m_evaluatorCount < 2)
//        return;
//
//	cjAniEvaluator** tmpEvaluators	 = (cjAniEvaluator**)cjMemAlloc(sizeof(cjAniEvaluator*) * m_evaluatorCount);
//	cjBYTE*			 tmpEvaluatorGrp = (cjBYTE*)cjMemAlloc(sizeof(cjBYTE) * m_evaluatorCount);
//		
//	cjUINT32 tmpEvaluatorCount = 0; // temporary copy
//	
//	for(cjUINT32 i=0; i<m_evaluatorCount; i++)
//    {
//        cjAniEvaluator* evaluator = m_evaluators[i];
//
//        if(!evaluator)
//            continue;
//
//        // default
//        cjAniEvalGrp resolvedEvaluatorGrp = cjAniEvalGrp_Referenced;
//
//        if(evaluator->IsTransformEvaluator())
//        {
//			// must have 3 evaluator channel(pos,rot,scale)
//            cjAssert(evaluator->GetMaxChannels() == 3);
//
//            if( evaluator->IsChannelItemInvalid(0) && 
//				evaluator->IsChannelItemInvalid(1) && 
//				evaluator->IsChannelItemInvalid(2))
//			{
//                resolvedEvaluatorGrp = cjAniEvalGrp_Invalid;
//			}
//            else if(!evaluator->IsReferenced())
//			{
//                resolvedEvaluatorGrp = cjAniEvalGrp_Transform;
//			}
//        }
//        else
//        {
//			// 1:normal evaluator
//            cjAssert(evaluator->GetMaxChannels() == 1); 
//
//            if(evaluator->IsChannelItemInvalid(0))
//			{
//				resolvedEvaluatorGrp = cjAniEvalGrp_Referenced;
//			}
//            else if(!evaluator->IsReferenced())
//            {
//				cjAniPoseBufChanItem pbChanItem = evaluator->GetPBChannelItemFromChanItemWithNoCareFlag(0);
//
//                if	   (pbChanItem == cjAniPoseBufChanItem_Color)	resolvedEvaluatorGrp = cjAniEvalGrp_Color;
//                else if(pbChanItem == cjAniPoseBufChanItem_Bool)	resolvedEvaluatorGrp = cjAniEvalGrp_Bool;
//                else if(pbChanItem == cjAniPoseBufChanItem_Float)	resolvedEvaluatorGrp = cjAniEvalGrp_Float;
//                else if(pbChanItem == cjAniPoseBufChanItem_Vec3S)	resolvedEvaluatorGrp = cjAniEvalGrp_Vec3S;
//				else if(pbChanItem == cjAniPoseBufChanItem_Vec3P)	resolvedEvaluatorGrp = cjAniEvalGrp_Vec3P;
//                else { 
//						cjAssert(pbChanItem == cjAniPoseBufChanItem_Rot); // must be cjAniPoseBufChanItem_Rot, by order
//						resolvedEvaluatorGrp = cjAniEvalGrp_Rot;	
//				}
//            }
//        }
//
//        cjAssert(resolvedEvaluatorGrp != cjAniEvalGrp_Invalid);
//
//        tmpEvaluators[tmpEvaluatorCount]	= evaluator;
//        tmpEvaluatorGrp[tmpEvaluatorCount]	= (cjBYTE)resolvedEvaluatorGrp;
//        tmpEvaluatorCount++;
//
//        evaluator->AddRef();
//        m_evaluators[i] = cjNULL;
//    }
//	
//	//
//	// rebuild evaluator order
//	//
//
//    m_evaluatorCount = 0;
//
//    for(cjUINT32 resolvedEvaluatorGrp=cjAniEvalGrp_Color; resolvedEvaluatorGrp <= 8; resolvedEvaluatorGrp++)
//    {
//        for(cjUINT32 i=0; i<tmpEvaluatorCount; i++)
//        {
//            if(tmpEvaluatorGrp[i] == resolvedEvaluatorGrp)
//            {
//                m_evaluators[m_evaluatorCount] = tmpEvaluators[i];
//                m_evaluatorCount++;
//
//				tmpEvaluators[i]->ReleaseRef();
//                tmpEvaluators[i] = cjNULL;
//            }
//        }
//    }
//
//	cjMemDeAlloc(tmpEvaluators);
//	cjMemDeAlloc(tmpEvaluatorGrp);
//}
//
//cjUINT32 cjResAniSeq::GetScratchPadBufSiz(cjBool isCumulativeAnimations, cjRefObjNameHash* sceneNodeHash, 
//	cjUINT32& uniqueBasisDatCount) const
//{
//    cjBSplineBasisF3Dat* uniqueBasisDat[MAX_UNIQUE_BASIS_DAT];
//    uniqueBasisDatCount = 0;
//
//    cjUINT32 spadBlockItemsCount[cjAniSPADB_Max];
//
//    for(cjUINT32 i=0; i<cjAniSPADB_Max; i++)
//        spadBlockItemsCount[i] = 0;
//
//    cjUINT32 scratchPadSiz = 0;
//
//    for(cjUINT32 i=0; i<m_evaluatorCount; i++)
//    {
//        cjAniEvaluator* evaluator = m_evaluators[i];
//
//        cjUINT32 maxEvalChannels = evaluator->GetMaxChannels();
//        cjBool forceAlwaysUpdate = cjFalse;
//
//        if(isCumulativeAnimations)
//		{
//			if(maxEvalChannels == 3 &&	// is transform evaluator
//				m_accumRootName.IsValid() && evaluator->GetID() == m_accumRootName)
//			{
//				// transform evaluator and accumRoot is exist
//				forceAlwaysUpdate = cjTrue;
//			}
//		}
//
//        cjBool alreadyShared = cjFalse;
//        for(cjUINT32 k=0; k<maxEvalChannels; k++)
//        {
//            cjUINT32 fillSiz;
//            cjBool isSharedFillDat;
//
//            cjAniSPadBlock spadBlkSegDat;
//            cjBSplineBasisF3Dat* basisDat;
//
//            if(evaluator->GetChannelScratchPadInfo(k, forceAlwaysUpdate, sceneNodeHash,
//				fillSiz, isSharedFillDat, spadBlkSegDat, basisDat))
//            {
//                if(!alreadyShared)
//                {
//                    scratchPadSiz += fillSiz;
//
//                    if(isSharedFillDat)
//                        alreadyShared = cjTrue;
//                }
//
//                spadBlockItemsCount[cjAniSPADB_EvaluatorDat]++;
//                spadBlockItemsCount[spadBlkSegDat]++;
//
//                if(basisDat)
//                {
//                    cjBool found = cjFalse;
//
//                    for(cjUINT32 m=0; m<uniqueBasisDatCount; m++)
//                    {
//                        if(basisDat == uniqueBasisDat[m])
//                        {
//                            found = cjTrue;
//                            break; // Found a match.
//                        }
//                    }
//
//                    // Add the new basis data to the array, if there's room.
//                    if(!found)
//                    {
//                        if(uniqueBasisDatCount < MAX_UNIQUE_BASIS_DAT)
//                            uniqueBasisDat[uniqueBasisDatCount] = basisDat;
//
//						uniqueBasisDatCount++;
//                    }
//                }
//            }
//        }
//    }
//
//    scratchPadSiz += spadBlockItemsCount[cjAniSPADB_EvaluatorDat]		* sizeof(cjAniEvalSPDat);
//
//    scratchPadSiz += uniqueBasisDatCount								* sizeof(cjBSplineBasisF3Dat);
//
//		// Linear/BSpline Color
//	scratchPadSiz += spadBlockItemsCount[cjAniSPADB_LinearColorSeg]		* sizeof(cjAniSPad::LColorSeg);
//    scratchPadSiz += spadBlockItemsCount[cjAniSPADB_BSColorSeg]			* sizeof(cjAniSPad::BSColorSeg);
//    																			 
//	scratchPadSiz += spadBlockItemsCount[cjAniSPADB_CBoolSeg]			* sizeof(cjAniSPad::CBoolSeg);
//    																			 
//		// Cubic/BSpline Float													 
//	scratchPadSiz += spadBlockItemsCount[cjAniSPADB_CubicFloatSeg]		* sizeof(cjAniSPad::CubicFloatSeg);
//    scratchPadSiz += spadBlockItemsCount[cjAniSPADB_BSFloatSeg]			* sizeof(cjAniSPad::BSFloatSeg);
//    																			 
//		// Cubic/BSpline Vec3S (scale)
//	scratchPadSiz += spadBlockItemsCount[cjAniSPADB_CubicVec3SSeg]		* sizeof(cjAniSPad::CubicVec3Seg);
//    scratchPadSiz += spadBlockItemsCount[cjAniSPADB_BSVec3SSeg]			* sizeof(cjAniSPad::BSVec3Seg);
//
//		// Cubic/BSpline Vec3S (pos)
//	scratchPadSiz += spadBlockItemsCount[cjAniSPADB_CubicVec3PSeg]		* sizeof(cjAniSPad::CubicVec3Seg);
//    scratchPadSiz += spadBlockItemsCount[cjAniSPADB_BSVec3PSeg]			* sizeof(cjAniSPad::BSVec3Seg);
//
//		// Slerp/Squad/BSpline  Rot												 
//	scratchPadSiz += spadBlockItemsCount[cjAniSPADB_SlerpRotSeg]		* sizeof(cjAniSPad::SlerpRotSeg);
//    scratchPadSiz += spadBlockItemsCount[cjAniSPADB_SquadRotSeg]		* sizeof(cjAniSPad::SquadRotSeg);
//    scratchPadSiz += spadBlockItemsCount[cjAniSPADB_BSRotSeg]			* sizeof(cjAniSPad::BSRotSeg);
//
//    cjAssert(cjAniSPADB_Max == 13);
//
//    return scratchPadSiz;
//}
//
//cjBool cjResAniSeq::CheckMatchMorphAniNote(cjAniCntrSeq* timeSyncCntrSeq)
//{
//	cjAssert(timeSyncCntrSeq);
//	return CheckMatchMorphAniNote(timeSyncCntrSeq->GetAniNote());
//}
//
//cjBool cjResAniSeq::CheckMatchMorphAniNote(cjExtDatAniNote* timeSyncAniNoteExtDat)
//{
//    if(!m_aniNote || !timeSyncAniNoteExtDat)
//        return cjFalse;
//
//    cjINT32 noteCountA, noteCountB;
//	cjAniNote* noteAryA = timeSyncAniNoteExtDat->GetNoteAry(&noteCountA);
//    cjAniNote* noteAryB = m_aniNote->GetNoteAry(&noteCountB);
//
//    const cjNameChar* morphTextKey	= cjAniConstants::GetMorphTextKey();
//    cjINT32 morphTextKeyLen			= cjAniConstants::GetMorphTextKeyLen();
//    cjBool morphKeysExistA			= cjFalse;
//
//    for(cjINT32 i=0; i<noteCountA; i++)
//    {
//        const cjNameChar* noteA = *(noteAryA[i].GetNote());        
//		cjAssert(noteA);
//
//        if(!cjStrncmp(noteA, morphTextKey, morphTextKeyLen))
//        {
//            morphKeysExistA = cjTrue;
//            cjBool found = cjFalse;
//
//            for(cjINT32 k=0; k<noteCountB; k++)
//            {
//                const cjNameChar* noteB = *(noteAryB[k].GetNote());
//                cjAssert(noteB);
//
//                if(!cjStrcmp(noteA, noteB))
//                {
//                    found = cjTrue;
//                    break;
//                }
//            }
//
//            if(!found)
//                return cjFalse;
//        }
//    }
//
//    if(!morphKeysExistA)
//        return cjFalse;
//
//    cjBool morphKeysExistB = cjFalse;
//
//    for(cjINT32 i=0; i<noteCountB; i++)
//    {
//        const cjNameChar* noteB = *(noteAryB[i].GetNote());
//        cjAssert(noteB);
//
//        if(cjStrncmp(noteB, morphTextKey, morphTextKeyLen))
//        {
//            morphKeysExistB = cjTrue;
//            cjBool found = cjFalse;
//
//            for(cjINT32 k=0; k<noteCountA; k++)
//            {
//                const cjNameChar* noteA = *(noteAryA[k].GetNote());
//                cjAssert(noteA);
//
//                if(!cjStrcmp(noteB, noteA))
//                {
//                    found = cjTrue;
//                    break;
//                }
//            }
//
//            if(!found)
//                return cjFalse;
//        }
//    }
//
//    if(!morphKeysExistB)
//        return cjFalse;
//
//    return cjTrue;
//}

