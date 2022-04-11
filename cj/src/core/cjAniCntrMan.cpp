
#include "cjCorePCH.h"
#include "cjAniCntrMan.h"


//
// cjAniCntrManager
//

CJSER_IMPL_CLASS(cjAniCntrManager, cjAniCntr)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjAniCntrManager)

    if(m_nodesHash)
		d->m_nodesHash = (cjRefObjNameHash*)m_nodesHash->_Clone(cloner);

	// skip cloning 
	// - m_cntrSeqs
	// - m_freeCntrSeqs
	// - m_poseBind, m_poseBlend, m_hdrMTPose

CJSER_IMPL_CLONE_PROPCOPY_END(cjAniCntrManager)

CJSER_IMPL_CLONE(cjAniCntrManager)

CJSER_IMPL_CLONE_POST_BEGIN(cjAniCntrManager)

	cjSerializable* clone = cjNULL;
	
	clone = cloner->GetCloned(this);

	if(clone)
	{
		cjAniCntrManager* clonedAniCntrMan = (cjAniCntrManager*)clone;

		if(m_nodesHash)
			m_nodesHash->_PostClone(cloner);

		clone = cloner->GetCloned(GetTarget());
		cjAssert(clone);

		clonedAniCntrMan->Init((cjRefObj*)clone, m_cumulative, m_poseBind, cjFalse);
		clonedAniCntrMan->SetMaxFreeCntrSeqCount(m_maxFreeCntrSeqCount);

		cjUINT32 seqCount = m_seqs.Count();
		for(cjUINT32 i=0; i<seqCount; i++)
		{			
			cjAniSeq* seq = m_seqs(i);

			if(seq && !seq->IsTempPose()) // skip temp poses
				clonedAniCntrMan->AddSeq(seq);
		}
	}

CJSER_IMPL_CLONE_POST_END(cjAniCntrManager)


void cjAniCntrManager::Update(cjFLOAT etime)
{
    if(!GetActive())
        return;

    cjINT32 lod = 0; // m_lodController ? m_lodController->GetBoneLOD() : 0;

    for(cjAryOff i=0; i<m_cntrSeqs.Count(); i++)
    {
        cjAniCntrSeq* cntrSeq = m_cntrSeqs(i);

        if(cntrSeq)
            cntrSeq->Update(etime, lod, cjTrue);
    }

    cjUINT32 cntrSeqCount = m_cntrSeqs.GetValidCount();

    if(cntrSeqCount == 0)
        return;

    cjUINT32 cntrSeqArySiz = m_cntrSeqs.Count();
    cjUINT32 index = 0;

	// remove null item gab
	for(cjUINT32 i=0; i<cntrSeqArySiz; i++)
    {
        cjAniCntrSeq* cntrSeq = m_cntrSeqs(i);

		if(cntrSeq)
        {
            if(index != i)
            {
                m_cntrSeqs(index) = cntrSeq;
                m_cntrSeqs(i) = cjNULL;
            }

            index++;
        }
    }

    cjAssert(index == cntrSeqCount);

	// sort sequences
	// active/inactive, priority, spinner and weight
	cjBool isSwapped;

    do
    {
        isSwapped = cjFalse;
        cjAniCntrSeq* cntrSeqA = m_cntrSeqs(0);

        for(cjUINT32 i=1; i<cntrSeqCount; i++)
        {
            cjAniCntrSeq* cntrSeqB = m_cntrSeqs(i);

            if(cntrSeqB->IsMoreImportantThan(cntrSeqA))
            {
                cntrSeqA->AddRef();

				m_cntrSeqs(i - 1) = cntrSeqB;
                m_cntrSeqs(i) = cntrSeqA;

                cntrSeqA->ReleaseRef();

                isSwapped = cjTrue;
            }
            else
            {
                cntrSeqA = cntrSeqB;
            }
        }

    } while(isSwapped);
	

	//
	// collect sequences to blend
	//

    cjFLOAT maxEaseSpinner = 0.0f;
    cjINT32 lastFullBodyPriority = CJ_INT_INVALID;
    cjUINT32 fullBodyPriorities = 0;
    cjUINT32 blendSeqCount = 0;

    while(blendSeqCount < cntrSeqCount)
    {
        cjAniCntrSeq* cntrSeq = m_cntrSeqs(blendSeqCount);

        if(cntrSeq->GetSeqState() == cjAniSeqState_InActive)
            break;

        cjINT32 priority = cntrSeq->GetPriority();

        if(priority < lastFullBodyPriority)
        {
            if(maxEaseSpinner >= 1.0f || fullBodyPriorities >= 2)
                break;
        }

        blendSeqCount++;

        cjAssert(cntrSeq->GetPoseBuf());
        cjAssert(cntrSeq->GetPoseBuf()->GetPoseBind());
        cjAssert(cntrSeq->GetPoseBuf()->GetPoseBind() == m_poseBind);

        if(cntrSeq->GetPoseBuf()->IsFullBodyPose())
        {
            if(priority != lastFullBodyPriority)
            {
                if(lastFullBodyPriority == CJ_INT_INVALID)
                    maxEaseSpinner = cntrSeq->GetEaseSpinner();

				lastFullBodyPriority = cntrSeq->GetPriority();
                fullBodyPriorities++;
            }
        }
    }

    if(blendSeqCount > 1)
    {
        cjAniCntrSeq* cntrSeq;
        cjAniPoseBuf* poseBuf;

        if(m_poseBlend->Update(blendSeqCount, lod, poseBuf, cntrSeq))
            m_hdrMTPose->Update(etime, lod, poseBuf, cntrSeq);
    }
    else if(blendSeqCount == 1)
    {
        cjAniCntrSeq* cntrSeq = m_cntrSeqs(0);
        cjAssert(cntrSeq->GetPoseBuf());

        m_hdrMTPose->Update(etime, lod, cntrSeq->GetPoseBuf(), cntrSeq);
    }

    RemoveAllDeActiveCntrSeq(cjFalse);
}

cjBool cjAniCntrManager::_CheckAcceptableTarget()  
{ 
	if(m_target)
		return m_target->GetInstAsRefObj() ? cjTrue : cjFalse;
	else
		return cjFalse;
}


//
// static functions
//

cjAniCntrManager* cjAniCntrManager::FindCntrManager(cjRefObj* obj)
{
    cjAssert(obj);
    cjAniCntrManager* cntrManager = cjGetAniCntr(cjAniCntrManager, obj);

    if(cntrManager)
        return cntrManager;

	cjSceneNode* node = 0;

	if(obj && cjrIsKindOf<cjSceneNode>(obj))
		node = (cjSceneNode*)obj;
	else
		return cjNULL;

	cjSceneNode* child = node->GetFirstChildSceneNode();

	while(child)
	{
		cntrManager = FindCntrManager(child);

		if(cntrManager)
			return cntrManager;

		child = child->GetSibilSceneNode();
	}

    return cjNULL;
}

cjRefObj* cjAniCntrManager::FindAccumRoot(cjRefObj* obj)
{
	if(!obj)
		return cjNULL;

	if(!cjrIsKindOf<cjSceneNode>(obj))
		return cjNULL;

	cjSceneNode* node = (cjSceneNode*)obj;
    const cjName& name = obj->GetName();

	if(name.IsValid())
    {
        cjINT32 nonAccumLen = cjAniConstants::GetNonAccumSuffixNameLen();
        const cjNameChar* nonAccumSuffix = cjAniConstants::GetNonAccumSuffixName();

		cjSceneNode* child = node->GetFirstChildSceneNode();
		while(child)
		{
			const cjName& childName = child->GetName();
			cjINT32 len = childName.GetLen() - nonAccumLen;

			if(len >= 0)
			{
				const cjNameChar* childNameSuffix = *childName;
				childNameSuffix += len;

				if(!cjStrcmp(childNameSuffix, nonAccumSuffix)) // found "NonAccum" suffix
                {							
                    if(!cjStrncmp(*childName, name, len))
                        return node; // child name and parent is match
                }
			}

			child = child->GetSibilSceneNode();
		}
	}

    {
        cjSceneNode* child = node->GetFirstChildSceneNode();
		cjRefObj* accumRoot = 0;

        while(child)
        {
            accumRoot = FindAccumRoot(child);

            if(accumRoot)
                return accumRoot;

			child = child->GetSibilSceneNode();
        }
    }

    return cjNULL;
}

cjAniCntrManager::cjAniCntrManager() : m_seqNameHash(40) 
{
	m_activationID				= 0;
	m_maxSPadBufSiz				= 4;
	m_maxEvaluatorCount			= 1;
	m_maxUniqueBasisDatCount	= 0;
	m_maxFreeCntrSeqCount		= CJ_SHORT_MAX;
	m_cumulative				= cjFalse;
}

cjAniCntrManager::cjAniCntrManager(cjRefObj* target, cjBool cumulativeAnimations,
	cjAniPoseBind* poseBind, cjBool moveAccumTMToChild) : m_seqNameHash(40)
{
	m_activationID				= 0;
	m_maxSPadBufSiz				= 4;
	m_maxEvaluatorCount			= 1;
	m_maxUniqueBasisDatCount	= 0;
	m_maxFreeCntrSeqCount		= CJ_SHORT_MAX;
	m_cumulative				= cjFalse;
	
    Init(target, cumulativeAnimations, poseBind, moveAccumTMToChild);
}

cjAniCntrManager::~cjAniCntrManager()
{
	Shutdown();
}



void cjAniCntrManager::ClearCumulativeAni()
{
	cjAssert(m_hdrMTPose);

	if(m_cumulative)
		m_hdrMTPose->ClearAccumTM();
}

cjBool cjAniCntrManager::GetAccumulatedTM(cjQuatTM& tm) const
{
	cjAssert(m_hdrMTPose);

	if(m_cumulative)
		tm = m_hdrMTPose->GetAccumTM();

	return m_cumulative;
}

void cjAniCntrManager::SetAccumulatedTM(const cjQuatTM& tm)
{
	cjAssert(m_hdrMTPose);

	if(m_cumulative)
		m_hdrMTPose->SetAccumTM(tm);
}

cjBool cjAniCntrManager::IsKnownSeq(const cjAniSeq* seq) const 
{
	cjAssert(seq);

	if(GetSeqByName(seq->GetName()) == seq)
		return cjTrue;

	//cjtemp very slow
	if(m_seqNameHash.FindByVal(const_cast<cjAniSeq*&>(seq)))
		return cjTrue;

	return cjFalse;
}


//
// cjAniSeq Management
//
//
//cjAniSeq* cjAniCntrManager::AddSeq(const cjNameChar* filename, cjUINT32 index)
//{
//    cjAniSeqRPtr spSeqData = cjAniSeq::CreateSeq(filename, index);
//
//    if(!spSeqData)
//        return cjNULL;
//
//    if(!AddSeq(spSeqData))
//        return cjNULL;
//
//    return spSeqData;
//}
//
//cjAniSeq* cjAniCntrManager::AddSeq(const cjFStream& stream, cjUINT32 index)
//{
//    cjAniSeqRPtr spSeqData = cjAniSeq::CreateSeq(stream, index);
//
//	if(!spSeqData)
//        return cjNULL;
//
//    if(!AddSeq(spSeqData))
//        return cjNULL;
//
//    return spSeqData;
//}

cjBool cjAniCntrManager::AddSeq(cjAniSeq* seq)
{
    cjAssert(seq);

    m_seqs.Push(seq);
    m_seqNameHash.Insert(seq->GetName(), seq);

    cjAssert(m_poseBind);
    m_poseBind->SyncBindInfoToSeqEvaluator(seq, this);

    if(!seq->GetAccumRootName().IsValid() && m_poseBind->GetAccumRootName().IsValid())
        seq->SetAccumRootName(m_poseBind->GetAccumRootName());

    cjAssert(m_hdrMTPose);
    m_hdrMTPose->AddNewTargetsFromPoseBind();

    cjWORD evaluatorCount = (cjWORD)seq->GetEvaluatorCount();
    if(evaluatorCount > m_maxEvaluatorCount)
        m_maxEvaluatorCount = evaluatorCount;

    cjUINT32 uniqueBasisDatCount;
    cjUINT32 spadSiz = seq->GetScratchPadBufSiz(m_cumulative, m_nodesHash, uniqueBasisDatCount);

    if(spadSiz > m_maxSPadBufSiz)
        m_maxSPadBufSiz = spadSiz;

    if((cjSWORD)uniqueBasisDatCount > m_maxUniqueBasisDatCount)
        m_maxUniqueBasisDatCount = (cjSWORD)uniqueBasisDatCount;

    return cjTrue;
}

//cjBool cjAniCntrManager::AddMultipleSeq(const cjNameChar* filename)
//{
//	cjAniSeqRPtrAry seqs;
//
//    if(!cjAniSeq::CreateMultipleSeq(filename, seqs))
//        return cjFalse;
//
//    cjBool success = cjTrue;
//    m_cntrSeqs.SetSiz(m_cntrSeqs.Count() + seqs.Count());
//
//    for(cjAryOff i=0; i<seqs.Count(); i++)
//    {
//        cjAniSeq* seq = seqs(i);
//
//        if(seq)
//			success = success && AddSeq(seq);
//    }
//
//    seqs.RemoveAll();
//
//    return success;
//}

cjAniSeq* cjAniCntrManager::AddPoseSeq(const cjAniSeq* seqToSync, cjBool tmpPose, 
	const cjName& name)
{
    cjAssert(seqToSync);
    cjAssert(m_hdrMTPose);

    cjAniSeq* poseSeq;
    cjUINT32 evaluatorCount = seqToSync->GetEvaluatorCount();

    if(name.IsValid())
    {
		poseSeq = new cjAniSeq(name, evaluatorCount);
    }
    else
    {
		// cjopti
		cjString poseName(_CT("__PoseSync"));

        poseName.AddSuffix(seqToSync->GetName());
        poseName.AddSuffix(_CT("__"));

        cjAniSeq* existingSeq;

        while(m_seqNameHash.FindByKeyName(poseName.GetStr(), &existingSeq))
            poseName.AddSuffix(_CT("_"));

        poseSeq = new cjAniSeq(poseName.GetStr(), evaluatorCount);
    }

    poseSeq->SetDuration(0.0f);

    if(tmpPose)
        poseSeq->SetTempPose();

    m_seqs.Push(poseSeq);
    m_seqNameHash.Insert(poseSeq->GetName(), poseSeq);

    m_hdrMTPose->FillPoseSeqDat(seqToSync, poseSeq);
    evaluatorCount = poseSeq->GetEvaluatorCount();

    if((cjSWORD)evaluatorCount > m_maxEvaluatorCount)
        m_maxEvaluatorCount = (cjSWORD)evaluatorCount;

    cjUINT32 uniqueBasisDatCount;
    cjUINT32 scratchPadSize = poseSeq->GetScratchPadBufSiz(m_cumulative, m_nodesHash, uniqueBasisDatCount);

	if(scratchPadSize > m_maxSPadBufSiz)
        m_maxSPadBufSiz = scratchPadSize;

    cjAssert((cjSWORD)uniqueBasisDatCount <= m_maxUniqueBasisDatCount);

    return poseSeq;
}

cjAniSeqRPtr cjAniCntrManager::RemoveSeq(const cjName& name)
{
    cjAssert(name.IsValid());
    cjAniSeq* seq;

    if(m_seqNameHash.Find(name, &seq))
        return RemoveSeq(seq);

    return cjNULL;
}

cjAniSeqRPtr cjAniCntrManager::RemoveSeq(cjAniSeq* seq)
{
    cjAssert(seq);
    cjAssert(GetCntrSeqBySeq(seq) == cjNULL);

    seq->RemoveActivationCB(cjNULL, this);
    cjAniSeqRPtr spSeqData = seq;

    if(m_seqs.Remove(seq) != cjAryOff_Invalid)
    {
        const cjName& name = spSeqData->GetName();

        if(name.IsValid())
            m_seqNameHash.Remove(name);

		return spSeqData;
    }

    return cjNULL;
}

void cjAniCntrManager::RemoveAllSeq()
{
    cjUINT32 seqCount = m_seqs.Count();

    for(cjUINT32 i=0; i<seqCount; i++)
    {
        cjAniSeq* seq = m_seqs(i);

        if(seq)
            seq->RemoveActivationCB(cjNULL, this);
    }

    m_seqs.RemoveAll();
    m_seqNameHash.RemoveAll();
}

void cjAniCntrManager::RemoveAllDeActiveCntrSeq(cjBool ignoreRefCount)
{
    for(cjUINT32 i=m_cntrSeqs.Count(); i>0; i--)
    {
        cjAniCntrSeq* cntrSeq = m_cntrSeqs(i-1);

		if(cntrSeq && cntrSeq->GetSeqState() == cjAniSeqState_InActive && 
			(ignoreRefCount || cntrSeq->GetRef() == 1))
		{
            RemoveCntrSeq(cntrSeq);
		}
    }
}

cjAniSeq* cjAniCntrManager::GetSeqByName(const cjName& name) const
{
	cjAssert(name.IsValid());
	cjAniSeq* seq;

	if(m_seqNameHash.Find(name, &seq))
		return seq;

	return cjNULL;
}

//
// cjAniCntrSeq Management
//

void cjAniCntrManager::AddCntrSeq(cjAniCntrSeq* cntrSeq) 
{
	cjAssert(cntrSeq && !cntrSeq->GetOwner());

	m_cntrSeqs.Push(cntrSeq);

	cntrSeq->SetOwner(this);	
}

void cjAniCntrManager::RemoveCntrSeq(cjAniCntrSeq* cntrSeq)
{
	cjAssert(cntrSeq && cntrSeq->GetOwner() == this);
	cjAssert(m_hdrMTPose);

	m_hdrMTPose->OnRemoveSequence(cntrSeq);

	if(cntrSeq->GetRef() == 1)
	{
		cntrSeq->PrepareForRecycling();
		PushFreeCntrSeq(cntrSeq);
	}

	cjAniSeqRPtr spSeq = cntrSeq->GetSeq();

	cntrSeq->SetSeq(cjNULL);
	cntrSeq->SetOwner(cjNULL);
	m_cntrSeqs.Remove(cntrSeq);

	if(spSeq && spSeq->IsTempPose())
		RemoveSeq(spSeq);
}

void cjAniCntrManager::RemoveAllCntrSeq()
{
	for(cjUINT32 i=m_cntrSeqs.Count(); i>0; i--)
	{
		cjAniCntrSeq* cntrSeq = m_cntrSeqs(i - 1);

		if(cntrSeq)
			RemoveCntrSeq(cntrSeq);
	}
}

cjAniCntrSeq* cjAniCntrManager::GetCntrSeqByName(const cjName& seqName) const
{
	cjAssert(seqName.IsValid());

	cjAniCntrSeq* cntrSeq;
	cjUINT32 cntrSeqCount = m_cntrSeqs.Count();

	for(cjUINT32 i=0; i<cntrSeqCount; i++)
	{
		cntrSeq = m_cntrSeqs(i);

		if(cntrSeq && cntrSeq->GetSeqName() == seqName)
			return cntrSeq;
	}

	return cjNULL;
}

cjAniCntrSeq* cjAniCntrManager::GetCntrSeqBySeq(const cjAniSeq* seq) const
{
	cjAssert(seq);

	cjAniCntrSeq* cntrSeq;
	cjUINT32 cntrSeqCount = m_cntrSeqs.Count();

	for(cjUINT32 i=0; i<cntrSeqCount; i++)
	{
		cntrSeq = m_cntrSeqs(i);

		if(cntrSeq && cntrSeq->GetSeq() == seq)
			return cntrSeq;
	}

	return cjNULL;
}

cjBool cjAniCntrManager::SetCntrSeqWeight(const cjName& cntrSeqName, cjFLOAT weight)
{
    cjAssert(cntrSeqName.IsValid());
    cjAniCntrSeq* cntrSeq = GetCntrSeqByName(cntrSeqName);

    if(cntrSeq)
        return SetCntrSeqWeight(cntrSeq, weight);

    return cjFalse;
}

cjBool cjAniCntrManager::SetCntrSeqWeight(cjAniCntrSeq* cntrSeq, cjFLOAT weight)
{
    cjAssert(cntrSeq && cntrSeq->GetOwner() == this);
    cntrSeq->SetWeight(weight);

	return cjTrue;
}

//
// Activate/Deactivate
//

cjAniCntrSeq* cjAniCntrManager::ActivateSeq(const cjName& seqName, cjINT32 priority, 
	cjFLOAT weight, cjFLOAT easeInTime, cjAniCntrSeq* timeSyncCntrSeq, cjFLOAT frequency, cjFLOAT stTime)
{
    cjAssert(seqName.IsValid());

    cjAniSeq* seq;

	if(m_seqNameHash.Find(seqName, &seq))
        return ActivateSeq(seq, priority, weight, easeInTime, timeSyncCntrSeq, frequency, stTime, cjFalse, cjFalse);

    return cjNULL;
}

cjAniCntrSeq* cjAniCntrManager::ActivateSeq(cjAniSeq* seq, cjINT32 priority, cjFLOAT weight, 
    cjFLOAT easeInTime, cjAniCntrSeq* timeSyncCntrSeq, cjFLOAT frequency, cjFLOAT stTime, 
	cjBool isTransition, cjBool isHoldTransitionTime)
{
    cjAssert(seq);

    return cjAniCntrSeq::Activate(this, seq, priority, weight, easeInTime, timeSyncCntrSeq, 
		frequency, stTime, isTransition, isHoldTransitionTime);
}

cjAniCntrSeq* cjAniCntrManager::ActivateSeq(cjAniSeq* seq, cjINT32 priority, cjFLOAT weight, 
	cjFLOAT easeInTime, cjAniCntrSeq* timeSyncCntrSeq, cjFLOAT frequency, cjFLOAT stTime)
{
    return ActivateSeq(seq, priority, weight, easeInTime, timeSyncCntrSeq, frequency, 
		stTime, cjFalse, cjFalse);
}

cjBool cjAniCntrManager::DeactivateCntrSeq(const cjName& cntrSeqName, cjFLOAT easeOutTime)
{
    cjAssert(cntrSeqName.IsValid());

    cjAniCntrSeq* cntrSeq = GetCntrSeqByName(cntrSeqName);

    if(cntrSeq)
        return DeactivateCntrSeq(cntrSeq, easeOutTime);

    return cjFalse;
}

cjBool cjAniCntrManager::DeactivateCntrSeq(cjAniCntrSeq* cntrSeq, cjFLOAT easeOutTime)
{
    cjAssert(cntrSeq && cntrSeq->GetOwner() == this);

    return cntrSeq->Deactivate(easeOutTime, cjFalse, cjFalse, cjTrue);
}

void cjAniCntrManager::DeactivateAllCntrSeq(cjFLOAT easeOutTime)
{
    for(cjAryOff i=0; i<m_cntrSeqs.Count(); i++)
    {
        cjAniCntrSeq* cntrSeq = m_cntrSeqs(i);

        if(cntrSeq)
            DeactivateCntrSeq(cntrSeq, easeOutTime);
    }
}


//
// CrossFade / Blend / Morph / BlendFromPose
//

cjAniCntrSeq* cjAniCntrManager::CrossFade(const cjName& srcCntrSeqName, const cjName& dstSeqName, 
	cjFLOAT duration, cjINT32 priority, cjFLOAT dstWeight, const cjName& dstTimeSyncCntrSeqName, cjFLOAT dstFrequency)
{
    cjAssert(srcCntrSeqName.IsValid() && dstSeqName.IsValid());

    cjAniSeq* dstSeq = GetSeqByName(dstSeqName);

    if(dstSeq)
    {
        cjAniCntrSeq* srcCntrSeq = GetCntrSeqByName(srcCntrSeqName);

        if(srcCntrSeq)
        {
            cjAniCntrSeq* dstTimeSyncCntrSeq = cjNULL;

            if(dstTimeSyncCntrSeqName.IsValid())
            {
                dstTimeSyncCntrSeq = GetCntrSeqByName(dstTimeSyncCntrSeqName);

                if(!dstTimeSyncCntrSeq)
                    return cjNULL;
            }

            return CrossFade(srcCntrSeq, dstSeq, duration, priority, dstWeight, dstTimeSyncCntrSeq, dstFrequency);
        }
    }

    return cjNULL;
}

cjAniCntrSeq* cjAniCntrManager::CrossFade(cjAniCntrSeq* srcCntrSeq, cjAniSeq* dstSeq, cjFLOAT duration, 
	cjINT32 priority, cjFLOAT dstWeight, cjAniCntrSeq* dstTimeSyncCntrSeq, cjFLOAT dstFrequency)
{
    cjAssert(srcCntrSeq && srcCntrSeq->GetOwner() == this);
    cjAssert(dstSeq);

    if(srcCntrSeq->GetSeqState() == cjAniSeqState_InActive)
        return cjNULL;

    cjAniCntrSeq* dstCntrSeq = ActivateSeq(dstSeq, priority, dstWeight, duration,
		dstTimeSyncCntrSeq, dstFrequency, 0.0f, cjTrue, cjFalse);

    if(!dstSeq)
        return cjNULL;

    srcCntrSeq->Deactivate(0.0f, cjFalse, cjFalse, cjFalse);

    if(!srcCntrSeq->Reactivate(priority, srcCntrSeq->GetWeight(), 0.0f, srcCntrSeq->GetTimeSyncSequence(), 
		CJANI_SEQUENCE_CUR_FRAME, cjFalse, cjFalse))
    {
        dstCntrSeq->Deactivate(0.0f);
        return cjNULL;
    }

    srcCntrSeq->Deactivate(duration, cjTrue, cjFalse, cjTrue);

    return dstCntrSeq;
}

cjAniCntrSeq* cjAniCntrManager::Morph(const cjName& srcCntrSeqName, const cjName& dstSeqName, cjFLOAT duration, 
	cjINT32 priority, cjFLOAT srcWeight, cjFLOAT dstWeight, cjFLOAT dstFrequency)
{
    cjAssert(srcCntrSeqName.IsValid() && dstSeqName.IsValid());

    cjAniSeq* dstSeq = GetSeqByName(dstSeqName);

    if(dstSeq)
    {
        cjAniCntrSeq* srcCntrSeq = GetCntrSeqByName(srcCntrSeqName);

		if(srcCntrSeq)
            return Morph(srcCntrSeq, dstSeq, duration, priority, srcWeight, dstWeight, dstFrequency);
    }

    return cjNULL;
}

cjAniCntrSeq* cjAniCntrManager::Morph(cjAniCntrSeq* srcCntrSeq, cjAniSeq* dstSeq, cjFLOAT duration, 
	cjINT32 priority, cjFLOAT srcWeight, cjFLOAT dstWeight, cjFLOAT dstFrequency)
{
    cjAssert(srcCntrSeq && srcCntrSeq->GetOwner() == this);
    cjAssert(dstSeq);

    if(srcCntrSeq->GetSeqState() == cjAniSeqState_InActive)
        return cjNULL;

    return srcCntrSeq->StartMorph(dstSeq, duration, priority, srcWeight, dstWeight, dstFrequency);
}

cjBool cjAniCntrManager::StopMorph(const cjName& srcCntrSeqName, const cjName& dstCntrSeqName, cjFLOAT easeOutTime)
{
    cjAssert(srcCntrSeqName.IsValid() && dstCntrSeqName.IsValid());

    cjAniCntrSeq* dstCntrSeq = GetCntrSeqByName(dstCntrSeqName);

    if(dstCntrSeq)
    {
        cjAniCntrSeq* srcCntrSeq = GetCntrSeqByName(srcCntrSeqName);

        if(srcCntrSeq)
            return StopMorph(srcCntrSeq, dstCntrSeq, easeOutTime);
    }

    return cjFalse;
}

cjBool cjAniCntrManager::StopMorph(cjAniCntrSeq* srcCntrSeq, cjAniCntrSeq* dstCntrSeq, cjFLOAT easeOutTime)
{
    cjAssert(srcCntrSeq && dstCntrSeq);

    if(srcCntrSeq->GetOwner() == this && srcCntrSeq->GetSeqState() != cjAniSeqState_InActive &&
        dstCntrSeq->GetOwner() == this && dstCntrSeq->GetSeqState() != cjAniSeqState_InActive)
    {
        srcCntrSeq->Deactivate(easeOutTime, cjTrue, cjFalse, cjTrue);
        dstCntrSeq->Deactivate(easeOutTime, cjTrue, cjFalse, cjTrue);

        return cjTrue;
    }

    return cjFalse;
}

cjAniCntrSeq* cjAniCntrManager::BlendFromSeq(const cjName& srcCntrSeqName, const cjName& dstSeqName, 
	cjFLOAT duration, const cjName& dstTxtKey, cjINT32 priority, cjFLOAT srcWeight,
    cjFLOAT dstWeight, const cjName& dstTimeSyncCntrSeqName, cjFLOAT dstFrequency)
{
    cjAssert(srcCntrSeqName.IsValid() && dstSeqName.IsValid());

    cjAniSeq* dstSeq = GetSeqByName(dstSeqName);

    if(dstSeq)
    {
        cjAniCntrSeq* srcCntrSeq = GetCntrSeqByName(srcCntrSeqName);

        if(srcCntrSeq && srcCntrSeq->GetSeqState() != cjAniSeqState_InActive)
        {
            cjAniCntrSeq* dstTimeSyncCntrSeq = cjNULL;

            if(dstTimeSyncCntrSeqName.IsValid())
            {
                dstTimeSyncCntrSeq = GetCntrSeqByName(dstTimeSyncCntrSeqName);

				if(!dstTimeSyncCntrSeq)
                    return cjNULL;
            }

            return BlendFromSeq(srcCntrSeq, dstSeq, duration, dstTxtKey, priority, 
				srcWeight, dstWeight, dstTimeSyncCntrSeq, dstFrequency);
        }
    }

    return cjNULL;
}

cjAniCntrSeq* cjAniCntrManager::BlendFromSeq(const cjName& srcCntrSeqName, const cjName& dstSeqName, 
	cjFLOAT duration, cjFLOAT dstTime, cjINT32 priority, cjFLOAT srcWeight, cjFLOAT dstWeight, 
	const cjName& dstTimeSyncCntrSeqName, cjFLOAT dstFrequency)
{
    cjAssert(srcCntrSeqName.IsValid() && dstSeqName.IsValid());

    cjAniSeq* dstSeq = GetSeqByName(dstSeqName);

    if(dstSeq)
    {
        cjAniCntrSeq* srcCntrSeq = GetCntrSeqByName(srcCntrSeqName);

        if(srcCntrSeq && srcCntrSeq->GetSeqState() != cjAniSeqState_InActive)
        {
            cjAniCntrSeq* dstTimeSyncCntrSeq = cjNULL;

            if(dstTimeSyncCntrSeqName.IsValid())
            {
                dstTimeSyncCntrSeq = GetCntrSeqByName(dstTimeSyncCntrSeqName);

                if(!dstTimeSyncCntrSeq)
                    return cjNULL;
            }

            return BlendFromSeq(srcCntrSeq, dstSeq, duration, dstTime, priority, srcWeight, 
                dstWeight, dstTimeSyncCntrSeq, dstFrequency);
        }
    }

    return cjNULL;
}

cjAniCntrSeq* cjAniCntrManager::BlendFromSeq(cjAniCntrSeq* cntrSeq, cjAniSeq* dstSeq, cjFLOAT duration, 
	const cjName& dstTxtKey, cjINT32 priority, cjFLOAT srcWeight, cjFLOAT dstWeight, 
	cjAniCntrSeq* dstTimeSyncCntrSeq, cjFLOAT dstFrequency)
{
    cjAssert(cntrSeq && cntrSeq->GetOwner() == this);
    cjAssert(dstSeq);

    if(cntrSeq->GetSeqState() == cjAniSeqState_InActive)
        return cjNULL;

    return cntrSeq->StartBlend(dstSeq, duration, dstTxtKey, priority, srcWeight, dstWeight, 
        dstTimeSyncCntrSeq, dstFrequency);
}

cjAniCntrSeq* cjAniCntrManager::BlendFromSeq(cjAniCntrSeq* cntrSeq, cjAniSeq* dstSeq, cjFLOAT duration, 
	cjFLOAT dstTime, cjINT32 priority, cjFLOAT srcWeight, cjFLOAT dstWeight, cjAniCntrSeq* dstTimeSyncCntrSeq, 
	cjFLOAT dstFrequency)
{
    cjAssert(cntrSeq && cntrSeq->GetOwner() == this);
    cjAssert(dstSeq);

    if(cntrSeq->GetSeqState() == cjAniSeqState_InActive)
        return cjNULL;

    return cntrSeq->StartBlend(dstSeq, duration, dstTime, priority, srcWeight, dstWeight, dstTimeSyncCntrSeq, dstFrequency);
}

cjBool cjAniCntrManager::StopBlendFromSeq(const cjName& srcCntrSeqName, const cjName& dstCntrSeqName, cjFLOAT easeOutTime)
{
    cjAssert(srcCntrSeqName.IsValid() && dstCntrSeqName.IsValid());

    cjAniCntrSeq* dstCntrSeq = GetCntrSeqByName(dstCntrSeqName);

    if(dstCntrSeq)
    {
        cjAniCntrSeq* srcCntrSeq = GetCntrSeqByName(srcCntrSeqName);

        if(srcCntrSeq)
            return StopBlendFromSeq(srcCntrSeq, dstCntrSeq, easeOutTime);
    }

    return cjFalse;
}

cjBool cjAniCntrManager::StopBlendFromSeq(cjAniCntrSeq* srcCntrSeq, cjAniCntrSeq* dstCntrSeq, cjFLOAT easeOutTime)
{
    cjAssert(srcCntrSeq && dstCntrSeq);

    if(srcCntrSeq->GetOwner() == this && srcCntrSeq->GetSeqState() != cjAniSeqState_InActive && dstCntrSeq->GetOwner() == this &&
        dstCntrSeq->GetSeqState() != cjAniSeqState_InActive)
    {
        srcCntrSeq->Deactivate(easeOutTime, cjTrue, srcCntrSeq->GetSeqState() == cjAniSeqState_TransitionSrc, cjTrue);
        dstCntrSeq->Deactivate(easeOutTime, cjTrue, dstCntrSeq->GetSeqState() == cjAniSeqState_TransitionDst, cjTrue);

        return cjTrue;
    }

    return cjFalse;
}

cjBool cjAniCntrManager::BlendFromPose(const cjName& dstSeqName, const cjName& dstTxtKey, 
	cjFLOAT duration, cjINT32 priority, const cjName& seqToSyncName, cjAniCntrSeq*& poseCntrSeq, 
    cjAniCntrSeq*& dstCntrSeq)
{
    cjAssert(dstSeqName.IsValid() && seqToSyncName.IsValid());

    cjAniSeq* dstSeq = GetSeqByName(dstSeqName);

    if(dstSeq)
    {
        cjAniSeq* seqToSync = GetSeqByName(seqToSyncName);

        if(seqToSync)
            return BlendFromPose(dstSeq, dstTxtKey, duration, priority, seqToSync, poseCntrSeq, dstCntrSeq);
    }

    return cjFalse;
}

cjBool cjAniCntrManager::BlendFromPose(const cjName& dstSeqName, cjFLOAT dstTime, cjFLOAT duration, 
	cjINT32 priority, const cjName& seqToSyncName, cjAniCntrSeq*& poseCntrSeq, cjAniCntrSeq*& dstCntrSeq)
{
    cjAssert(dstSeqName.IsValid() && seqToSyncName.IsValid());

    cjAniSeq* dstSeq = GetSeqByName(dstSeqName);

    if(dstSeq)
    {
        cjAniSeq* seqToSync = GetSeqByName(seqToSyncName);

        if(seqToSync)
            return BlendFromPose(dstSeq, dstTime, duration, priority, seqToSync, poseCntrSeq, dstCntrSeq);
    }

    return cjFalse;
}

cjBool cjAniCntrManager::BlendFromPose(cjAniSeq* dstSeq, const cjName& dstTxtKey, cjFLOAT duration, 
	cjINT32 priority, cjAniSeq* seqToSync, cjAniCntrSeq*& poseCntrSeq, cjAniCntrSeq*& dstCntrSeq)
{
    cjAssert(dstCntrSeq && seqToSync);
    
	cjAniSeq* poseSeq = AddPoseSeq(seqToSync);

	if(!poseSeq)
        return cjFalse;

    poseCntrSeq = ActivateSeq(poseSeq, priority);

    if(!poseCntrSeq)
    {
        RemoveSeq(poseSeq);
        return cjFalse;
    }

    dstCntrSeq = poseCntrSeq->StartBlend(dstSeq, duration, dstTxtKey, priority, 1.0f, 1.0f, cjNULL, 1.0f);

    if(!dstCntrSeq)
    {
        poseCntrSeq->Deactivate(0.0f, cjFalse, cjFalse, cjTrue);
        poseCntrSeq = cjNULL;

        return cjFalse;
    }

    return cjTrue;
}

cjBool cjAniCntrManager::BlendFromPose(cjAniSeq* dstSeq, cjFLOAT dstTime, cjFLOAT duration, 
	cjINT32 priority, cjAniSeq* seqToSync, cjAniCntrSeq*& poseCntrSeq, cjAniCntrSeq*& dstCntrSeq)
{
    cjAssert(dstSeq && seqToSync);

    cjAniSeq* poseSeq = AddPoseSeq(seqToSync);
    if(!poseSeq)
        return cjFalse;

    poseCntrSeq = ActivateSeq(poseSeq, priority);

    if(!poseCntrSeq)
    {
        RemoveSeq(poseSeq);
        return cjFalse;
    }

    dstCntrSeq = poseCntrSeq->StartBlend(dstSeq, duration, dstTime, priority, 1.0f, 1.0f, cjNULL, 1.0f);

    if(!dstCntrSeq)
    {
        poseCntrSeq->Deactivate(0.0f, cjFalse, cjFalse, cjTrue);
        poseCntrSeq = cjNULL;
        return cjFalse;
    }

    return cjTrue;
}


//
// CntrSequence Pool
//

cjBool cjAniCntrManager::PushFreeCntrSeq(cjAniCntrSeq* cntrSeq)
{
    cjAssert(cntrSeq);
    cjAssert(cntrSeq->GetOwner() == this);
    cjAssert(cntrSeq->GetRef() == 1);

    cjWORD freeCntrSeqCount = GetFreeCntrSeqCount();

    if(freeCntrSeqCount < m_maxFreeCntrSeqCount)
    {
        m_freeCntrSeqs.Push(cntrSeq);

        cjAssert(cntrSeq->GetRef() == 2);
        return cjTrue;
    }

    return cjFalse;
}

void cjAniCntrManager::SetMaxFreeCntrSeqCount(cjSWORD maxFreeCntrSeqCount)
{
    m_maxFreeCntrSeqCount = maxFreeCntrSeqCount;

    for(cjAryOff i=m_freeCntrSeqs.Count(); i>maxFreeCntrSeqCount; i--)
    {
        cjAniCntrSeq* cntrSeq = m_freeCntrSeqs(i - 1);
        cjAssert(cntrSeq);
        m_freeCntrSeqs.Remove(cntrSeq);
    }
}

void cjAniCntrManager::RemoveAllFreeCntrSeqs()
{
	cjSWORD maxRecySeqCount = m_maxFreeCntrSeqCount;
    SetMaxFreeCntrSeqCount(0); // remove all cntrSeq
    SetMaxFreeCntrSeqCount(maxRecySeqCount);
}

cjAniCntrSeqRPtr cjAniCntrManager::GetFreeCntrSeq()
{
	cjUINT32 recycledSeqCount = m_freeCntrSeqs.Count();

	if(recycledSeqCount == 0)
		return cjNULL;

	cjAniCntrSeqRPtr spCntrSeq = m_freeCntrSeqs(recycledSeqCount - 1);

	cjAssert(spCntrSeq);
	cjAssert(spCntrSeq->GetRef() == 2);

	m_freeCntrSeqs.Remove(spCntrSeq);

	cjAssert(spCntrSeq->GetRef() == 1);

	return spCntrSeq;
}


void cjAniCntrManager::Init(cjRefObj* target, cjBool cumulativeAni, cjAniPoseBind* poseBind, 
	cjBool moveAccumTMToChild)
{
    cjAssert(target);
    cjAssert(!m_poseBind);
    cjAssert(!m_poseBlend);
    cjAssert(!m_hdrMTPose);

    cjAniCntr::SetTarget(target);

	if(!m_nodesHash)
        m_nodesHash = new cjRefObjNameHash(target);

    cjRefObj* accumRoot = cjNULL;
    cjAssert(m_cumulative == cjFalse);

    if(cumulativeAni)
    {
		cjAssert(cjrIsKindOf<cjSceneNode>(GetTarget()));

		accumRoot = FindAccumRoot((cjSceneNode*)GetTarget());
        m_cumulative = (accumRoot != cjNULL);
    }

    if(!poseBind)
        m_poseBind = new cjAniPoseBind(this, accumRoot);
    else
        m_poseBind = poseBind;

    cjAssert(m_cumulative == m_poseBind->GetCumulativeAni());

    m_poseBlend = new cjAniPoseBlend(this);	
    m_hdrMTPose = new cjAniMTargetPoseHandler(this, moveAccumTMToChild);
}

void cjAniCntrManager::Shutdown()
{
    SetMaxFreeCntrSeqCount(0);

    RemoveAllSeq();
    RemoveAllCntrSeq();

    m_nodesHash = cjNULL;
    m_poseBind = cjNULL;

    m_poseBlend = cjNULL;
    m_hdrMTPose = cjNULL;
}


//cjBool cjAniCntrManager::IsEqual(cjRefObj* obj)
//{
//    if(!cjAniCntr::IsEqual(obj))
//        return cjFalse;
//
//    cjAniCntrManager* cntrMan = (cjAniCntrManager*)obj;
//    cjUINT32 seqCount = m_seqs.Count();
//
//	if(seqCount != pkManager->m_seqs.Count())
//        return cjFalse;
//
//    for(cjUINT32 i=0; i<seqCount; i++)
//    {
//        cjAniSeq* seq0 = m_seqs(i);
//        cjAniSeq* seq1 = cntrMan->m_seqs(i);
//
//        if(seq0 != seq1)
//        {
//            if(seq0 && seq1)
//            {
//                if(!seq0->IsEqual(seq1))
//                    return cjFalse;
//            }
//            else
//            {
//                return cjFalse;
//            }
//        }
//    }
//
//    if(m_poseBind != cntrMan->m_poseBind)
//    {
//        if(!m_poseBind || !cntrMan->m_poseBind || !m_poseBind->IsEqual(cntrMan->m_poseBind))
//            return cjFalse;
//    }
//
//    return cjTrue;
//}



//
// cjAnimator
//



CJSER_IMPL_CLASS(cjAnimator, cjRefObj)

//cjdefer
cjSerializable* cjAnimator::Clone(cjSerObjCloner* cloner)
{
	cjBool needToDeleteCloner = cjFalse;

    if(!cloner)
    {
		cloner = new cjSerObjCloner;
		needToDeleteCloner = cjTrue;
    }

	cjSceneNode* sceneRoot = (cjSceneNode*)m_sceneRoot->Clone(cloner);

	if(needToDeleteCloner)
	{
		delete cloner;
		cloner = cjNULL;
	}

    // Get the controller manager from the clone.
    cjAniCntrManager* cntrMan = cjAniCntrManager::FindCntrManager(sceneRoot);
    cjAssert(cntrMan);

	//
	// Clone cjAnimator
	//

	cjAnimator* newAnimator = new cjAnimator(m_animatorDat, m_isCumulativeAniRequire);
	newAnimator->m_sceneRoot = sceneRoot;
	newAnimator->m_aniCntrMan = cntrMan;

    // Copy member data.
    newAnimator->m_animatorCB = m_animatorCB;

	// clone aniseq
	SeqIDToSeqRPtrHash::Iter seqIter = m_seqMap.GetLast();
	SeqIDToSeqRPtrHash::Pair* seqV;

    while(m_seqMap.GetNext(seqIter, seqV))
    {
		cjAniSeqRPtr newSeqRPtr = newAnimator->m_aniCntrMan->GetSeqByName(seqV->m_val->GetName());
		cjAssert(newSeqRPtr);
		newAnimator->m_seqMap.Set(seqV->m_key, newSeqRPtr);
    }

    for(cjAryOff i=0; i<m_animatorCBDats.Count(); i++)
    {
		cjAnimatorCBDat* cbDat = m_animatorCBDats(i);

        if(cbDat)
            newAnimator->m_animatorCBDats.Push(cbDat);
    }

    return newAnimator;
}

cjAnimator::cjAnimator()
{
}

cjAnimator::cjAnimator(cjAnimatorDat* animatorDat, cjBool isCumulativeAni) : m_seqMap(20)
{
	m_animatorDat				= animatorDat;
    
//	m_sceneRoot					= cjNULL;
//	m_aniCntrTarget				= cjNULL;

//  m_aniCntrMan				= cjNULL;

    m_animatorCB				= cjNULL;
	
    m_targetSeqID				= 0;
    m_targetKey					= cjNULL;

    m_needToBuildTimeline		= cjFalse;
    m_curTime					= 0.0f;
    m_timelineUpdateTime		= 0.0f;

    m_isInTransition			= cjFalse;
    m_transState				= cjAnimatorTS_NoTrans;
    m_transStTime				= 0.0f;
    m_transEnTime				= 0.0f;
    m_transStFrameTime			= 0.0f;

    m_curSeqID					= 0;

	m_nextSeqID					= 0;

    m_nextChainTime				= 0.0f;

    m_isChainActive				= cjFalse;
    m_isCumulativeAniRequire	= isCumulativeAni;

    Reset();

    m_animatorCBDats.SetSiz(3);
}

cjAnimator::~cjAnimator()
{
    m_animatorCB = cjNULL;

    if(m_aniCntrMan)
    {
        m_aniCntrMan->SetMaxFreeCntrSeqCount(0);
        m_aniCntrMan->RemoveAllSeq();
    }

    Reset();

    m_sceneRoot			= cjNULL;
	m_aniCntrManTarget	= cjNULL;
    m_aniCntrMan		= cjNULL;
	m_animatorDat		= cjNULL;
}

void cjAnimator::OnAniSeqChange(cjAniCntrSeq* cntrSeq, cjAniSeqState seqState)
{
    if(seqState == cjAniSeqState_InActive)
    {
        cjBool isExtraSeq = cjTrue;        		
		cjAniSeqID seqID = GetSeqID(cntrSeq, cjTrue, cjFalse);

        if(seqID == cjAniSeqID_InvalidSeq)
        {
            isExtraSeq = cjFalse;			
            seqID = GetSeqID(cntrSeq, cjFalse, cjTrue);
        }

        if(seqID != cjAniSeqID_InvalidSeq)
        {
            cjUINT32 callbackDatCount = m_animatorCBDats.Count();

            for(cjUINT32 i=0; i<callbackDatCount; i++)
            {
                cjAnimatorCBDat* animatorCBDat = m_animatorCBDats(i);

                if(animatorCBDat)
                {
                    if(animatorCBDat->m_evtType == cjAnimatorAnim_Completed &&
                        (animatorCBDat->m_seqID == seqID || animatorCBDat->m_seqID == cjAniSeqID_AnySeq))
                    {
                        MakeAniEvtCompleted(seqID);
                        break;
                    }
                }
            }
 
            cntrSeq->RemoveActivationCallback(this);

            if(isExtraSeq)
                m_extCntrSeqMap.Remove(seqID);
        }
    }
}

cjAnimator* cjAnimator::Create(const cjNameChar* resCharName, cjBool isCumulativeAni, 
	cjAniPoseBind* poseBind)
{
	//cjdefer

	return cjNULL;
}

void cjAnimator::Update(cjFLOAT etime)
{
    cjAnimatorRst rst;

    m_curTime = etime;

    ProcessCallbacks();

    if(m_transState != cjAnimatorTS_NoTrans)
    {
        if(m_isInTransition && m_curTime >= m_transStTime)
        {
            if(m_transState == cjAnimatorTS_Blend)
            {
                cjFLOAT duration = m_transEnTime - m_transStTime;
                m_nextCntrSeq = m_aniCntrMan->BlendFromSeq(m_curCntrSeq, m_nextSeq, duration, m_targetKey);

				cjAssert(m_nextCntrSeq != cjNULL);

                m_transStTime = m_curTime;
                m_transEnTime = m_transStTime + duration;

                m_nextCntrSeq->AddActivationCallback(this);
            }

            m_isInTransition = cjFalse;
        }

        if(m_curTime > m_transEnTime)
        {
            if(m_curCntrSeq && m_curCntrSeq->GetSeqState() != cjAniSeqState_InActive)
                m_aniCntrMan->DeactivateCntrSeq(m_curCntrSeq);

            m_transState			= cjAnimatorTS_NoTrans;
            m_curSeqID				= m_nextSeqID;
            m_curCntrSeq			= m_nextCntrSeq;
            m_nextSeqID				= cjAniSeqID_InvalidSeq;
            m_nextSeq				= cjNULL;
            m_nextCntrSeq			= cjNULL;
            m_needToBuildTimeline	= cjTrue;
        }

        return;
    }

    if(m_curTime > m_timelineUpdateTime)
    {
        m_needToBuildTimeline = cjFalse;
        BuildTimeline();
    }

    cjAniSeqID dstSeqID;

	if(m_isChainActive)
    {
        if(m_nextChainTime == cjAnimatorInvalidTime)
        {
            cjAssert(!m_chainIDs.IsEmpty() && !m_chainDurations.IsEmpty());
									
            m_chainIDs.RemoveHeadV(dstSeqID);

            cjFLOAT duration;
			m_chainDurations.RemoveHeadV(duration);

            if(!m_chainIDs.IsEmpty())
            {
                if(duration == cjAnimatorMaxDuration)
                {
                    cjAssert(GetSeq(dstSeqID) == m_curCntrSeq->GetSeq());
                    cjAssert(dstSeqID == m_curSeqID);

                    duration = m_curCntrSeq->GetScaledAniNoteTimeOnlyNotOverETime(
									cjAniConstants::GetEndTextKey(), m_curTime) - m_curTime;
                    duration -= 0.0333f; // 0.0333 for 30fps, frame delay

                    if(duration < 0.0f)
                        duration = 0.0f;
                }

                m_nextChainTime = m_curTime + duration;

                return;
            }
            else
            {
                cjAssert(m_chainDurations.IsEmpty());

                m_nextChainTime = cjAnimatorInvalidTime;
                m_isChainActive = cjFalse;
            }
        }
        else if(m_curTime > m_nextChainTime)
        {
            cjAssert(!m_chainIDs.IsEmpty() && !m_chainDurations.IsEmpty());
            dstSeqID = m_chainIDs.GetHeadV();

            m_nextChainTime = cjAnimatorInvalidTime;
        }
        else
        {
            return;
        }
    }
    else
    {
        dstSeqID = m_targetSeqID;
    }

    if(dstSeqID == m_curSeqID) // already animating desired seq
    {
        if(m_needToBuildTimeline)
        {
            m_needToBuildTimeline = cjFalse;
            BuildTimeline();
        }

        return;
    }

    if(dstSeqID == cjAniSeqID_InvalidSeq)
    {
		MakeAniEvtDeactivated(m_curSeqID);
        m_curSeqID = cjAniSeqID_InvalidSeq;

        if(m_curCntrSeq)
        {
            m_aniCntrMan->DeactivateCntrSeq(m_curCntrSeq);
            m_curCntrSeq = cjNULL;
        }

        return;
    }

    if(m_curSeqID == cjAniSeqID_InvalidSeq)
    {
        cjAniSeq* dstSeq = GetSeq(dstSeqID);
        cjAssert(dstSeq);

        cjAniCntrSeq* dstCntrSeq = m_aniCntrMan->ActivateSeq(dstSeq, 0);
        cjAssert(dstCntrSeq);

        m_curSeqID = dstSeqID;
        m_curCntrSeq = dstCntrSeq;

        dstCntrSeq->AddActivationCallback(this);

        MakeAniEvtActivated(m_curSeqID);

        m_aniCntrMan->Update(m_curTime);

        m_needToBuildTimeline = cjFalse;

        BuildTimeline();

        ProcessCallbacks();

        return;
    }


	//
	// Transition
	//

    cjBool isTransitionNeed = cjFalse;

    rst = m_animatorDat->IsTransitionNeed(m_curSeqID, dstSeqID, isTransitionNeed);
    cjAssert(rst == cjAnimatorRst_Ok);

    if(isTransitionNeed)
    {
        cjAniSeq* dstSeq = GetSeq(dstSeqID);
        cjAssert(dstSeq);

        cjAniTransition* transition = m_animatorDat->GetTransition(m_curSeqID, dstSeqID);
        cjAssert(transition);

        if(transition->GetType() == cjAniTransitionType_Chain)
        {
            cjAssert(!m_isChainActive);
            cjAssert(m_animatorDat->IsValidChainTransition(m_curSeqID, dstSeqID, transition));

            m_chainIDs.RemoveAll();
            m_chainDurations.RemoveAll();

            cjAryOff chainAniCount = transition->GetChains().Count();
            cjAssert(chainAniCount > 0);

            for(cjAryOff i=0; i<chainAniCount; i++)
            {
                cjUINT32 chainSeqID = transition->GetChains()(i).m_seqID;
                cjFLOAT duration = transition->GetChains()(i).m_duration;

                m_chainIDs.PushBack(chainSeqID);
                m_chainDurations.PushBack(duration);
            }

            m_chainIDs.PushBack(dstSeqID);
            m_chainDurations.PushBack(cjAnimatorMaxDuration);

            dstSeqID			= m_chainIDs.GetHeadV();
            dstSeq				= GetSeq(dstSeqID);
            cjAssert(dstSeq);

            transition			= m_animatorDat->GetTransition(m_curSeqID, dstSeqID);
            cjAssert(transition);

            m_nextChainTime		= cjAnimatorInvalidTime;
            m_isChainActive		= cjTrue;
        }

        m_transStTime = m_curTime;
        m_targetKey = cjName::NullName;

        if(transition->GetType() == cjAniTransitionType_Change)
        {
            if(transition->GetBlendPairs().Count() > 0)
            {
                m_curCntrSeq->Update(m_curTime, 0, cjFalse);
                m_transStTime = CJ_FTIME_INFINITY;

                for(cjAryOff i=0; i<transition->GetBlendPairs().Count(); i++)
                {
                    cjAniTxtMarkerPair* aniTxtMarkerPair = transition->GetBlendPairs()(i);

                    if(!aniTxtMarkerPair->GetStMarker())
                    {
                        m_transStTime = m_curTime;
                        m_targetKey = aniTxtMarkerPair->GetEnMarker();
                        break;
                    }

                    cjFLOAT tmpTime = m_curCntrSeq->GetScaledAniNoteTimeOnlyNotOverETime(
														aniTxtMarkerPair->GetStMarker(), m_curTime);

                    if( tmpTime != cjAnimatorInvalidTime &&
                        tmpTime >= m_curTime &&
                        tmpTime < m_transStTime )
                    {
                        m_transStTime = tmpTime;
                        m_targetKey = aniTxtMarkerPair->GetEnMarker();
                    }
                }
            }
        }

        cjAssert(transition->GetDuration() > 0.0f);

        m_transEnTime = m_transStTime + transition->GetDuration();
        cjAniCntrSeq* dstCntrSeq = cjNULL;

        switch(transition->GetType())
        {
        case cjAniTransitionType_Change:
			{
				m_transStFrameTime = m_curCntrSeq->GetLastCalcTime();

				if(m_transStTime == m_curTime)
				{
					dstCntrSeq = m_aniCntrMan->BlendFromSeq(m_curCntrSeq, dstSeq, transition->GetDuration(), m_targetKey);
					cjAssert(dstCntrSeq);
				}
				else
				{
					m_transStFrameTime += m_curCntrSeq->_CalcTime(m_transStTime - m_curTime, cjFalse);
					m_isInTransition = cjTrue;
				}

				m_transState = cjAnimatorTS_Blend;
			}
            break;

        /*case cjAniTransitionType_Morph:
			{
				m_transStFrameTime = m_curCntrSeq->GetLastCalcTime();
				
				dstCntrSeq = m_aniCntrMan->Morph(m_curCntrSeq, dstSeq, transition->GetDuration());
				cjAssert(dstCntrSeq);

				m_transState = cjAnimatorTS_Morph;
			}
			break;*/

        case cjAniTransitionType_CrossFade:
			{
				m_transStFrameTime = m_curCntrSeq->GetLastCalcTime();
				
				dstCntrSeq = m_aniCntrMan->CrossFade(m_curCntrSeq, dstSeq, transition->GetDuration(), 0, cjTrue);
				cjAssert(dstCntrSeq);

				m_transState = cjAnimatorTS_CrossFade;
			}
			break;

        default:
            cjAssert(cjFalse);
            break;
        }

        if(dstCntrSeq)
            dstCntrSeq->AddActivationCallback(this);

        m_nextSeqID				= dstSeqID;
        m_nextSeq				= dstSeq;
        m_nextCntrSeq			= dstCntrSeq;
        m_needToBuildTimeline	= cjTrue;
    }

    if(m_needToBuildTimeline)
    {
        m_needToBuildTimeline = cjFalse;
        BuildTimeline();
    }
}

void cjAnimator::Reset()
{
	if(m_aniCntrMan)
        m_aniCntrMan->DeactivateAllCntrSeq();
		
	m_targetSeqID			= cjAniSeqID_InvalidSeq;
	m_targetKey				= cjName::NullName;

	m_needToBuildTimeline	= cjFalse;
	m_curTime				= CJ_FTIME_INVALID;
	m_timelineUpdateTime	= CJ_FTIME_INFINITY;


	m_isInTransition		= cjFalse;
	m_transState			= cjAnimatorTS_NoTrans;
    m_transStTime			= CJ_FTIME_INFINITY;
    m_transEnTime			= CJ_FTIME_INVALID;
    m_transStFrameTime		= CJ_FTIME_INVALID;


	m_curSeqID				= cjAniSeqID_InvalidSeq;
	m_curCntrSeq			= cjNULL;

	m_nextSeqID				= cjAniSeqID_InvalidSeq;
	m_nextSeq				= cjNULL;
    m_nextCntrSeq			= cjNULL;
	
	m_nextChainTime			= CJ_FTIME_INVALID;

	m_chainIDs.RemoveAll();
	m_chainDurations.RemoveAll();
	m_isChainActive			= cjFalse;

	m_timelineDats.RemoveAll();
	m_extCntrSeqMap.RemoveAll();
}


//
// cjAnimator-Mesh
//

//cjBool cjAnimator::ReloadMesh(cjMesh* mesh, cjAniPoseBind* poseBind)
//{
//	//cjdefer
//	return cjFalse;
//}

cjBool cjAnimator::ChangeSceneRoot(cjRefObj* sceneRoot, cjRefObj* sceneModelRoot, 
	cjAniPoseBind* poseBind)
{
    if(!sceneRoot)
        return cjFalse;

	m_sceneRoot = sceneRoot;

	if(!sceneModelRoot)
		m_aniCntrManTarget = m_sceneRoot;

    Reset();

	cjAniCntrManager* newAniCntrMan = new cjAniCntrManager(m_aniCntrManTarget, 
		m_isCumulativeAniRequire, poseBind);

    if(m_aniCntrMan)
    {
		cjAryOff seqCount = m_aniCntrMan->GetSeqCount();

        for(cjAryOff i=0; i<seqCount; i++)
        {
            cjAniSeq* seq = m_aniCntrMan->GetSeq(i);

            if(!seq || seq->IsTempPose())
                continue;

            newAniCntrMan->AddSeq(seq);
        }

        m_aniCntrMan->RemoveAllCntrSeq();
        m_aniCntrMan->RemoveAllSeq();
    }

	m_sceneRoot = sceneRoot;
    m_aniCntrMan = newAniCntrMan;

    return cjTrue;
}

void cjAnimator::RefreshControllerManager()
{
	m_aniCntrMan = cjAniCntrManager::FindCntrManager(m_sceneRoot);
	cjAssert(m_aniCntrMan);
}


//
// animation
//

	// callback
cjBool cjAnimator::RegCallback(cjAnimatorEvtType evtType, cjAniSeqID seqID, const cjName& txtKeyName)
{
    cjNameMatch* match = cjNULL;

    if(evtType == cjAnimatorAnim_TxtKeyEvt)
    {
        if(txtKeyName.IsValid())
		{
            match = new cjNameMatch(txtKeyName); // ref = 0;
			cjAssert(match->GetRef() == 0);
		}
        else
            return cjFalse;
    }

    UnRegCallback(evtType, seqID, txtKeyName);

    return _RegCallback(evtType, seqID, match);
}

cjBool cjAnimator::_RegCallback(cjAnimatorEvtType evtType, cjAniSeqID seqID, cjNameMatch* refNameMatch)
{
    if(evtType == cjAnimatorAnim_TxtKeyEvt)
    {
        if(!refNameMatch)
            return cjFalse;
    }
    else
    {
		//cjwarn - need checking wheater cjNameMatch is created from stack or new
		// this function must be not called with refNameMatch parameter that is made as new cjNameMatch
        refNameMatch = cjNULL;
    }

    UnRegCallback(evtType, seqID, refNameMatch);

	cjAnimatorCBDat* cbDat = new cjAnimatorCBDat(evtType, seqID, refNameMatch);
    m_animatorCBDats.Push(cbDat);

    m_needToBuildTimeline = cjTrue;

    return cjTrue;
}

void cjAnimator::UnRegCallback(cjAnimatorEvtType evtType, cjAniSeqID seqID, const cjName& txtKeyName)
{
    for(cjAryOff i=0; i<m_animatorCBDats.Count(); i++)
    {
        cjAnimatorCBDat* animatorCBDat = m_animatorCBDats(i);

        if(!animatorCBDat)
            continue;

        if(animatorCBDat->m_evtType == evtType && 
			animatorCBDat->m_seqID == seqID)
        {
            cjNameMatch* curNameMatch = animatorCBDat->GetNameMatch();

            if(!curNameMatch || 
				(curNameMatch && curNameMatch->IsMatch(txtKeyName)))
            {
                RemoveTimelineDataObjects(evtType, seqID, curNameMatch);
                m_animatorCBDats.Remove(i);

                break;
            }
        }
    }
}

void cjAnimator::UnRegCallback(cjAnimatorEvtType evtType, cjAniSeqID seqID, cjNameMatch* noRefNameMatch)
{
    if(evtType != cjAnimatorAnim_TxtKeyEvt)
        noRefNameMatch = cjNULL;

    for(cjAryOff i=0; i<m_animatorCBDats.Count(); i++)
    {
        cjAnimatorCBDat* animatorCBDat = m_animatorCBDats(i);
      
        if(animatorCBDat && animatorCBDat->m_evtType == evtType &&
            animatorCBDat->m_seqID == seqID &&
            animatorCBDat->GetNameMatch() == noRefNameMatch)
        {
            RemoveTimelineDataObjects(evtType, seqID, noRefNameMatch);
            m_animatorCBDats.Remove(i);

            break;
        }
    }
}

void cjAnimator::ClearAllRegCallbacks()
{
	//cjneed: check releasing callback object in m_animatorCBDats
	m_animatorCBDats.RemoveAll();
    m_needToBuildTimeline = cjTrue;
}


//
// cjAnimator - Sequence Management
//

cjAniCntrSeq* cjAnimator::ActivateSeq(cjAniSeqID seqID, cjINT32 priority, cjFLOAT seqWeight, 
    cjFLOAT easeInTime, cjAniSeqID timeSyncSeqID, cjFLOAT freq, cjFLOAT stTime)
{
    cjAniSeq* seq = GetSeq(seqID);

    if(!seq)
        return cjNULL;

    cjAniCntrSeq* timeSyncCntrSeq = cjNULL;

    if(timeSyncSeqID != cjAniSeqID_InvalidSeq)
    {
        timeSyncCntrSeq = GetActiveSeq(timeSyncSeqID, cjTrue, cjTrue);

		if(!timeSyncCntrSeq)
            return cjNULL;
    }

    cjAniCntrSeqRPtr extCntrSeq = GetExtraSeq(seqID);

    if(extCntrSeq)
    {
        if(m_curTime != cjAnimatorInvalidTime)
            extCntrSeq->Update(m_curTime, 0, cjFalse);

        if(extCntrSeq->GetSeqState() != cjAniSeqState_InActive)
            return cjNULL;
    }

    cjAniCntrSeq* cntrSeq = m_aniCntrMan->ActivateSeq(seq, priority, seqWeight, easeInTime, 
		timeSyncCntrSeq, freq, stTime, cjFalse, cjFalse);

    if(cntrSeq)
    {
		cjAniCntrSeqRPtr _tmp(cntrSeq);
		m_extCntrSeqMap.Set(seqID, _tmp);

        cntrSeq->AddActivationCallback(this); // to determine when the seq complete

        if(m_curTime != cjAnimatorInvalidTime)
            cntrSeq->Update(m_curTime, 0, cjFalse);

        if(easeInTime == 0.0f)
            MakeAniEvtActivated(seqID);

        if(m_curTime != cjAnimatorInvalidTime)
            BuildTimeline();
        else
            m_needToBuildTimeline = cjTrue;
    }

    return cntrSeq;
}

cjBool cjAnimator::DeactivateSeq(cjAniSeqID seqID, cjFLOAT easeOutTime)
{
    cjAniCntrSeqRPtr extCntrSeq = GetExtraSeq(seqID);

    if(!extCntrSeq)
        return cjFalse;

    if(extCntrSeq->GetSeqState() == cjAniSeqState_EaseOut || 
		extCntrSeq->GetSeqState() == cjAniSeqState_EaseIn)
        return cjFalse;

    if(m_curTime != cjAnimatorInvalidTime)
        extCntrSeq->Update(m_curTime, 0, cjFalse);

    if(extCntrSeq->GetSeqState() == cjAniSeqState_InActive)
        return cjFalse;

    if(easeOutTime == 0.0f)
        MakeAniEvtDeactivated(seqID);

	cjBool rst = m_aniCntrMan->DeactivateCntrSeq(extCntrSeq, easeOutTime);
    cjAssert(rst);

    m_needToBuildTimeline = cjTrue;

    if(m_curTime != cjAnimatorInvalidTime)
        extCntrSeq->Update(m_curTime, 0, cjFalse);

    return rst;
}

cjAniSeq* cjAnimator::GetSeq(cjAniSeqID seqID) const
{
	cjAniSeqRPtr seqPtr;

	if(m_seqMap.Find(seqID, &seqPtr))
		return seqPtr;

	return cjNULL;
}

cjAniCntrSeq* cjAnimator::GetActiveSeq(cjAniSeqID seqID, cjBool checkExtraSeq, cjBool checkStateSeq) const
{
    if(checkExtraSeq)
    {
		cjAniCntrSeq* cntrSeq = GetExtraSeq(seqID);

		if(cntrSeq && cntrSeq->GetSeqState() != cjAniSeqState_InActive)
            return cntrSeq;
    }

    if(checkStateSeq)
    {
        if(m_curSeqID == seqID && m_curCntrSeq && m_curCntrSeq->GetSeqState() != cjAniSeqState_InActive)
            return m_curCntrSeq;

        if(m_nextSeqID == seqID && m_nextCntrSeq && m_nextCntrSeq->GetSeqState() != cjAniSeqState_InActive)
            return m_nextCntrSeq;
    }

    return cjNULL;
}

cjAniSeqID cjAnimator::GetSeqID(cjAniCntrSeq* cntrSeq, cjBool checkExtraSeq, cjBool checkStateSeq) const
{
    cjAssert(cntrSeq);

    if(checkExtraSeq)
    {
		SeqIDToCntrSeqRPtrHash::Iter iter = m_extCntrSeqMap.GetLast();
		SeqIDToCntrSeqRPtrHash::Pair* cur;

		while(m_extCntrSeqMap.GetNext(iter, cur))
		{
			if(cntrSeq == cur->m_val)
                return cur->m_key;
        }
    }

    if(checkStateSeq)
    {
        if(cntrSeq == m_curCntrSeq)
            return m_curSeqID;

        if(cntrSeq == m_nextCntrSeq)
            return m_nextSeqID;
    }

    return cjAniSeqID_InvalidSeq;
}

cjAnimatorRst cjAnimator::ChangeSeqID(cjAniSeqID oldSeqID, cjAniSeqID newSeqID)
{
    cjAnimatorRst rst = m_animatorDat->UpdateSeqID(oldSeqID, newSeqID);

    if(m_targetSeqID == oldSeqID)
        m_targetSeqID = newSeqID;

	if(m_curSeqID == oldSeqID)
        m_curSeqID = newSeqID;

	if(m_nextSeqID == oldSeqID)
        m_nextSeqID = newSeqID;
	
	{
		ChainIDLst::Iter chainIter = m_chainIDs.Begin();
		cjAniSeqID chainSeqID;

		while(chainIter)
		{
			chainSeqID = m_chainIDs.GetValue(chainIter);

			if(chainSeqID == oldSeqID)
			{
				ChainIDLst::Iter tmpIter = m_chainIDs.PushAfter(chainIter, newSeqID);
				m_chainIDs.RemoveByIter(chainIter);
				chainIter = tmpIter;
			}

			m_chainIDs.GetNext(chainIter);
		}
	}

	{
		SeqIDToSeqRPtrHash::Iter iter = m_seqMap.GetLast();
		SeqIDToSeqRPtrHash::Pair* cur = m_seqMap.GetLast();

		while(m_seqMap.GetNext(iter, cur))
		{
			if(cur->m_key == oldSeqID)
			{
				cjAniSeqRPtr _tmpSeqPtr(cur->m_val);

				m_seqMap.Remove(oldSeqID);
				m_seqMap.Set(newSeqID, _tmpSeqPtr);

				break;
			}			
		}
	}

	// callback
	{
		for(cjAryOff i=0; i<m_animatorCBDats.Count(); i++)
		{
			cjAnimatorCBDat* curCBDat = m_animatorCBDats(i);

			if(curCBDat && curCBDat->m_seqID == oldSeqID)
				curCBDat->m_seqID = newSeqID;
		}
	}

	// timelinedat
	{
		TimelineDatLst::Iter iter = m_timelineDats.Begin();
		
		while(m_timelineDats.GetNext(iter))
		{
			cjAnimatorTimeline& curTimeline = m_timelineDats.GetValue(iter);

			if(curTimeline.m_seqID == oldSeqID)
				curTimeline.m_seqID = newSeqID;
		}
	}

	// extra seq
	{
		cjAniCntrSeqRPtr extCntrSeqRPtr = GetExtraSeq(oldSeqID);

		if(extCntrSeqRPtr)
		{
			m_extCntrSeqMap.Remove(oldSeqID);
			m_extCntrSeqMap.Set(newSeqID, extCntrSeqRPtr);
		}
	}

    return rst;
}

cjBool cjAnimator::ChangeSeq(cjAniSeqID seqID, cjAniSeq* seq)
{
    UnloadSeq(seqID);

    cjBool rst = m_aniCntrMan->AddSeq(seq);

    if(rst)
	{
		cjAniSeqRPtr _tmp(seq);
        m_seqMap.Set(seqID, _tmp);
	}

    return rst;
}

void cjAnimator::UnloadSeq(cjAniSeqID seqID)
{
    cjAssert(m_aniCntrMan);

    cjAniSeq* seq = GetSeq(seqID);

	if(!seq)
        return;

    DeactivateSeq(seqID, 0.0f);

    cjAniCntrSeqRPtr extCntrSeq = GetExtraSeq(seqID);

    if(seqID == m_curSeqID || seqID == m_nextSeqID || seqID == m_targetSeqID || extCntrSeq != cjNULL)
        Reset();

    m_aniCntrMan->RemoveSeq(seq);
    m_seqMap.Remove(seqID);
}

cjAniSeqID cjAnimator::FindSeqID(const cjNameChar* seqName) const
{
    cjAssert(seqName);

	SeqIDToSeqRPtrHash::Iter iter = m_seqMap.GetLast();
	SeqIDToSeqRPtrHash::Pair* pair;

	while(m_seqMap.GetNext(iter, pair))
	{
		cjAniSeq* curSeq = pair->m_val;
		const cjName& curSeqName = curSeq->GetName();

        if(curSeqName.IsValid() && !cjStrcmp(*curSeqName, seqName))
            return pair->m_key;
	}

    return cjAniSeqID_InvalidSeq;
}

cjBool cjAnimator::SetTargetSeqID(cjAniSeqID seqID)
{
    if(m_targetSeqID == seqID)
        return cjTrue;

    if(seqID != cjAniSeqID_InvalidSeq)
    {
		if(!m_seqMap.Find(seqID))
            return cjFalse;
    }

    m_targetSeqID = seqID;
    m_needToBuildTimeline = cjTrue;

    return cjTrue;
}

cjFLOAT	cjAnimator::GetNextEventTime(cjAnimatorEvtType evtType, cjAniSeqID seqID, const cjName& txtKey)
{
    if(evtType == cjAnimatorAnim_TxtKeyEvt && txtKey.IsValid())
    {
        cjNameMatch txtKeyMatch(txtKey);
        return GetNextEventTime(evtType, seqID, &txtKeyMatch);
    }
    else
    {
        return GetNextEventTime(evtType, seqID);
    }
}

cjFLOAT	cjAnimator::GetNextEventTime(cjAnimatorEvtType evtType, cjAniSeqID seqID, cjNameMatch* noRefNameMatch)
{
    switch(evtType)
    {
    case cjAnimatorAnim_Activated:
        return GetNextAnimActivatedTime(seqID);

    case cjAnimatorAnim_Deactivated:
        return GetNextAnimDeactivatedTime(seqID);

    case cjAnimatorAnim_TxtKeyEvt:
        return GetNextTextKeyEventTime(seqID, noRefNameMatch);

    case cjAnimatorAnim_EndOfSeq:
        return GetNextEndOfSequenceTime(seqID);

    case cjAnimatorAnim_Completed:
    default:
        return CJ_FTIME_INVALID;
    }
}

cjFLOAT cjAnimator::GetNextTextKeyEventTime(cjAniSeqID seqID, const cjName& txtKeyName)
{
    cjNameMatch nameMatch(txtKeyName); // ref=0
    return GetNextTextKeyEventTime(seqID, &nameMatch);
}

cjFLOAT cjAnimator::GetNextTextKeyEventTime(cjAniSeqID seqID, cjNameMatch* noRefNameMatch)
{
    if(seqID == cjAniSeqID_InvalidSeq)
        return cjAnimatorInvalidTime;

    cjAssert(noRefNameMatch);
    cjAssert(m_curTime != cjAnimatorInvalidTime);

    cjFLOAT txtKeyTime;
    cjAniCntrSeqRPtr extCntrSeq = GetExtraSeq(seqID);

	if(extCntrSeq)
    {
        if(extCntrSeq->GetSeqState() == cjAniSeqState_InActive)
        {
            extCntrSeq->RemoveActivationCallback(this);
            m_extCntrSeqMap.Remove(seqID);

            return cjAnimatorInvalidTime;
        }

        txtKeyTime = extCntrSeq->GetScaledAniNoteTimeOnlyNotOverETime(noRefNameMatch, m_curTime);

        if(txtKeyTime == cjAnimatorInvalidTime || 
			txtKeyTime < m_curTime ||
			(extCntrSeq->GetSeqState() == cjAniSeqState_EaseOut && txtKeyTime > extCntrSeq->GetEaseEndTime()))
        {
            return cjAnimatorInvalidTime;
        }

        return txtKeyTime;
    }

    if(seqID == m_curSeqID)
    {
        txtKeyTime = m_curCntrSeq->GetScaledAniNoteTimeOnlyNotOverETime(noRefNameMatch, m_curTime);
        if(txtKeyTime == cjAnimatorInvalidTime || 
			txtKeyTime < m_curTime ||
            (txtKeyTime == m_curTime && m_curCntrSeq->GetAniNoteTime(noRefNameMatch) == m_curCntrSeq->GetDuration()))
        {
            return cjAnimatorInvalidTime;
        }

        if(m_nextSeqID != cjAniSeqID_InvalidSeq && txtKeyTime > m_transStTime)
            return cjAnimatorInvalidTime;

        return txtKeyTime;
    }

    if(m_nextSeqID == cjAniSeqID_InvalidSeq || 
		(seqID != m_nextSeqID && m_chainIDs.IsEmpty()) ||
        (seqID != m_nextSeqID && !m_chainIDs.Find(seqID))  )
    {
        return cjAnimatorInvalidTime;
    }

    txtKeyTime = m_transEnTime;

    cjAniTransition* transition = m_animatorDat->GetTransition(m_curSeqID, m_nextSeqID);
    cjAssert(transition && transition->GetType() != cjAniTransitionType_Chain);

    cjFLOAT transitionStTime = m_transStFrameTime;

    if(transitionStTime == cjAnimatorInvalidTime)
        transitionStTime = 0.0f;

	cjAnimatorCompletionInfo completionInfo;
    FindAniTransitionCompletionInfo(m_curSeqID, m_nextSeqID, transition, 
		transitionStTime, transitionStTime, &completionInfo);

    cjFLOAT transCompleteTime = completionInfo.m_dstTimeTransitionComplete;
    cjFLOAT curTransitionTime = 0.0f;

	ChainIDLst::Iter chainIDIter = m_chainIDs.Begin();
	ChainDurationLst::Iter chainDurationIter = m_chainDurations.Begin();

    if(!m_chainDurations.IsEmpty())
    {
		chainDurationIter = m_chainDurations.Begin();
		cjFLOAT duration = m_chainDurations.GetValue(chainDurationIter);

        if(duration == cjAnimatorMaxDuration)
            duration = m_nextSeq->GetDuration() - transCompleteTime;
        else
            duration = m_nextSeq->GetMulByFreq(duration);

        curTransitionTime = transCompleteTime + duration;
        cjAssert(chainIDIter && chainDurationIter);

        m_chainIDs.GetNext(chainIDIter);
        m_chainDurations.GetNext(chainDurationIter);
    }

    cjAniSeqID srcSeqID = m_nextSeqID;
    cjAniSeq* srcSeq = GetSeq(m_nextSeqID);

    for(;;)
    {
        if(seqID == srcSeqID)
        {
            cjFLOAT aniNoteTime = srcSeq->GetAniNoteTime(noRefNameMatch);

            if(aniNoteTime == cjAnimatorInvalidTime || 
				(chainIDIter && aniNoteTime > curTransitionTime))
			{
                return cjAnimatorInvalidTime;
			}

            cjFLOAT delayTime = aniNoteTime - transCompleteTime;
			if(delayTime < 0.0f)
                delayTime += srcSeq->GetDuration();

            cjFLOAT unScaledTime = delayTime / srcSeq->GetFrequency();
            txtKeyTime += unScaledTime;

            return txtKeyTime;
        }

        if(!chainIDIter)
            return cjAnimatorInvalidTime;

		cjAssert(chainDurationIter);

        cjAniSeqID chainSeqID;
		m_chainIDs.GetNextV(chainIDIter, chainSeqID);

        cjFLOAT chainDuration;
		m_chainDurations.GetNextV(chainDurationIter, chainDuration);

	    cjAniSeq* chainSeq = GetSeq(chainSeqID);
        cjAssert(chainSeq);
	
        cjAniTransition* chainTransition = m_animatorDat->GetTransition(srcSeqID, chainSeqID);
        cjAssert(chainTransition && chainTransition->GetType() != cjAniTransitionType_Chain);

        FindAniTransitionCompletionInfo(srcSeqID, chainSeqID, chainTransition, 
			transCompleteTime, curTransitionTime, &completionInfo);

        txtKeyTime += completionInfo.m_timeToCompleteTransition;

        srcSeqID = chainSeqID;
        srcSeq = chainSeq;
        transCompleteTime = completionInfo.m_dstTimeTransitionComplete;

        if(chainDuration == cjAnimatorMaxDuration)
            chainDuration = chainSeq->GetDuration() - transCompleteTime;
        else
            chainDuration = chainSeq->GetMulByFreq(chainDuration);

        curTransitionTime = transCompleteTime + chainDuration;
    }
}

cjBool cjAnimator::FindAniTransitionCompletionInfo(cjAniSeqID transSrcID, cjAniSeqID transDstID, 
	cjAniTransition* transition, cjFLOAT stTime, cjFLOAT enTime, cjAnimatorCompletionInfo* rstInfo)
{
    cjAssert(rstInfo);
    cjAssert(transition);

    cjAniSeq* srcSeq = GetSeq(transSrcID);
    cjAniSeq* dstSeq = GetSeq(transDstID);

    cjAssert(srcSeq && dstSeq);
    cjAssert(enTime >= 0.0);
    cjAssert(enTime >= stTime);

    switch(transition->GetType())
    {
    case cjAniTransitionType_Change:
		{
			cjBool doImmediateBlend;

			if(transition->GetBlendPairs().IsEmpty())
			{
				doImmediateBlend = cjTrue;
			}
			else if(transition->GetBlendPairs().Count() == 1 && 
					!transition->GetBlendPairs()(0)->GetStMarker().IsValid())
			{
				doImmediateBlend = cjTrue;
			}
			else
			{
				doImmediateBlend = cjFalse;
			}

			if(doImmediateBlend)
			{
				cjFLOAT beginTransitionTime = enTime - stTime;
				cjName immOffTxtKey = cjAniConstants::GetStartTextKey();

				if(transition->GetBlendPairs().Count() == 1)
					immOffTxtKey = transition->GetBlendPairs()(0)->GetEnMarker();

				rstInfo->m_dstTimeTransitionComplete = dstSeq->GetAniNoteTime(immOffTxtKey);
				rstInfo->m_timeToCompleteTransition = srcSeq->GetDivByFreq(beginTransitionTime) + transition->GetDuration();
				rstInfo->m_srcTransitionOccurTime = enTime;
			}
			else
			{
				cjFLOAT transitionUnboundTime = 0;
				cjAniTxtMarkerPair* blendPair = GetNextBlendPair(transSrcID, transDstID, transition, enTime, transitionUnboundTime);
				rstInfo->m_blendPair = blendPair;

				if(!blendPair)
				{
					rstInfo->m_srcTransitionOccurTime		= srcSeq->GetDuration();
					rstInfo->m_dstTimeTransitionComplete	= 0.0f;
					rstInfo->m_timeToCompleteTransition		= srcSeq->GetDivByFreq(transitionUnboundTime - stTime) + transition->GetDuration();
				}
				else
				{
					cjName stKey = blendPair->GetStMarker();

					if(!stKey.IsValid())
						stKey = cjAniConstants::GetStartTextKey();

					cjFLOAT transitionTime = srcSeq->GetAniNoteTime(stKey);
					cjAssert(transitionTime != cjAnimatorInvalidTime);

					rstInfo->m_srcTransitionOccurTime = transitionTime;

					cjName enKey = blendPair->GetEnMarker();

					if(!enKey.IsValid())
						enKey = cjAniConstants::GetStartTextKey();

					rstInfo->m_dstTimeTransitionComplete = dstSeq->GetAniNoteTime(enKey);
					rstInfo->m_timeToCompleteTransition  = srcSeq->GetDivByFreq(transitionUnboundTime - stTime) + transition->GetDuration();
				}
			}
		}
		break; // case TYPE_BLEND

	//case cjAniTransitionType_Morph:
	//	{
	//		rstInfo->m_srcTransitionOccurTime = enTime;
	//		rstInfo->m_timeToCompleteTransition = srcSeq->GetDivByFreq(enTime - stTime) + transition->GetDuration();

	//		cjFLOAT dstTransitionTime = srcSeq->GetDivByFreq(enTime);
	//		cjFLOAT dstTransitionStTime = dstSeq->FindCorrespondingMorphFrame(srcSeq, dstTransitionTime);
	//		cjFLOAT partialDstSeqTimeLen = dstSeq->GetDuration()- dstTransitionStTime;
	//		cjFLOAT transDuration = transition->GetDuration();
	//		cjFLOAT curDstTransitionDuration;

	//		if(transDuration == cjAnimatorMaxDuration)
	//		{
	//			cjAssert(transDuration != cjAnimatorMaxDuration);
	//			curDstTransitionDuration = dstSeq->GetDuration() - stTime;
	//		}
	//		else
	//		{
	//			curDstTransitionDuration = dstSeq->GetMulByFreq(transDuration);
	//		}
	//		
	//		if(curDstTransitionDuration > partialDstSeqTimeLen)
	//		{
	//			cjFLOAT dstDuration = dstSeq->GetDuration();
	//			cjFLOAT remainingDuration = curDstTransitionDuration - partialDstSeqTimeLen;
	//			cjFLOAT ceiling = cjCeilF(remainingDuration / dstDuration);
	//			cjFLOAT dstLoopTime = (ceiling * dstDuration);
	//			cjFLOAT transFinal = (dstDuration - dstTransitionStTime) + dstLoopTime;
	//			rstInfo->m_dstTimeTransitionComplete = dstDuration - (transFinal - curDstTransitionDuration);
	//		}
	//		else
	//		{
	//			rstInfo->m_dstTimeTransitionComplete = dstTransitionStTime + curDstTransitionDuration;
	//		}
	//	}
	//	break;

    case cjAniTransitionType_CrossFade:
		{
			rstInfo->m_srcTransitionOccurTime = enTime;
			cjFLOAT transitionDuration = transition->GetDuration();
			rstInfo->m_timeToCompleteTransition = srcSeq->GetDivByFreq(enTime - stTime) + transitionDuration;

			cjFLOAT dstDuration = dstSeq->GetDuration();
			cjFLOAT curDstTransitionDuration = dstSeq->GetMulByFreq(transitionDuration);

			if(curDstTransitionDuration > dstDuration)
			{
				if(dstSeq->GetLoopType() == cjAniCntr::ANILOOP_ONE)
				{
					rstInfo->m_dstTimeTransitionComplete = dstDuration;
				}
				else
				{
					cjFLOAT ceiling = cjCeilF(curDstTransitionDuration / dstDuration);
					cjFLOAT timeOfDstLoops = (ceiling * dstDuration);

					rstInfo->m_dstTimeTransitionComplete = dstDuration - (timeOfDstLoops - curDstTransitionDuration);
				}
			}
			else
			{
				rstInfo->m_dstTimeTransitionComplete = curDstTransitionDuration;
			}
		}
		break;

    case cjAniTransitionType_Chain:
		{
			cjFLOAT totalTime = 0.0f;
			cjUINT32 transitionChainCount = transition->GetChains().Count();
			cjAssert(transitionChainCount > 0);

			cjAniSeq* chainSrcSeq = srcSeq;
			cjUINT32 srcSeqID = transSrcID;
			cjAniSeqIDChainInfo* chainInfo;

			cjFLOAT chainSrcStTime = 0.0f;
			cjFLOAT chainDuration = enTime;
			cjAnimatorChainCompletionInfo* chainCompletionInfo;
        
			for(cjUINT32 i=0; i<transitionChainCount; i++)
			{
				chainInfo = &transition->GetChains()(i);
				cjUINT32 dstSeqChainID = chainInfo->m_seqID;
				cjAniSeq* dstSeqChain = GetSeq(dstSeqChainID);

				chainCompletionInfo = FillChainComplete(srcSeqID, dstSeqChainID, chainDuration, chainSrcStTime, totalTime);
				rstInfo->AddChainCompletionInfo(chainCompletionInfo);

				chainSrcSeq = dstSeqChain;
				srcSeqID = dstSeqChainID;

				chainDuration = chainInfo->m_duration;

				if(chainDuration == cjAnimatorMaxDuration)
					chainDuration = chainSrcSeq->GetDuration() - chainSrcStTime;
				else
					chainDuration = dstSeqChain->GetMulByFreq(chainDuration);
			}

			chainCompletionInfo = FillChainComplete(srcSeqID, transDstID, chainDuration, chainSrcStTime, totalTime);
			rstInfo->AddChainCompletionInfo(chainCompletionInfo);

			chainSrcSeq = GetSeq(srcSeqID);
			totalTime += dstSeq->GetDurationDivFreq() - chainSrcSeq->GetDivByFreq(chainSrcStTime);
			rstInfo-> m_chainTimeToComplete = totalTime;
		}
		break;

    default:
		{
			cjAssert(cjFalse);
			return cjFalse;
		}
        break;
    }

    return cjTrue;
}

cjAniCntrSeq* cjAnimator::GetExtraSeq(cjAniSeqID seqID) const
{
	cjAniCntrSeqRPtr cntrSeqPtr;

    if(m_extCntrSeqMap.Find(seqID, &cntrSeqPtr))
        return cntrSeqPtr;

    return cjNULL;
}

void cjAnimator::RemoveInactiveExtraSequences()
{
	SeqIDToCntrSeqRPtrHash::Iter iter = m_extCntrSeqMap.GetLast();
	SeqIDToCntrSeqRPtrHash::Pair* pair;

    while(m_extCntrSeqMap.GetNext(iter, pair))
    {	
		cjAniSeqID seqID = pair->m_key;
		cjAniCntrSeq* cntrSeq = pair->m_val;

		if(cntrSeq->GetSeqState() == cjAniSeqState_InActive)
        {
            cntrSeq->RemoveActivationCallback(this);
            m_extCntrSeqMap.Remove(seqID);
        }
    }
}

cjAnimatorChainCompletionInfo* cjAnimator::FillChainComplete(cjAniSeqID srcSeqID, 
	cjAniSeqID dstSeqID, cjFLOAT duration, cjFLOAT& rstSrcBeginTime, cjFLOAT& rstTotalTime)
{
    cjAniSeq* srcSeq = GetSeq(srcSeqID);
    cjAniSeq* dstSeq = GetSeq(dstSeqID);

    cjAniTransition* transition = m_animatorDat->GetTransition(srcSeqID, dstSeqID);    
	cjAssert(transition);
    
    cjFLOAT enTransitionTime;

    if(duration == cjAnimatorMaxDuration)
    {
        enTransitionTime = srcSeq->GetDuration();

        if(enTransitionTime < rstSrcBeginTime)
            enTransitionTime = rstSrcBeginTime;
    }
    else
    {
        enTransitionTime = rstSrcBeginTime + duration;
    }

    cjAnimatorCompletionInfo completionInfo;

    FindAniTransitionCompletionInfo(srcSeqID, dstSeqID, transition, 
		rstSrcBeginTime, enTransitionTime, &completionInfo);

    cjAnimatorChainCompletionInfo* chainCompletionInfo 
		= new cjAnimatorChainCompletionInfo;
    
	chainCompletionInfo->SetName(srcSeq->GetName());

    chainCompletionInfo->m_seqStart			= rstTotalTime;
    chainCompletionInfo->m_inSeqBeginTime	= rstSrcBeginTime;

    rstTotalTime += completionInfo.m_timeToCompleteTransition;

    chainCompletionInfo->m_transStTime		= rstTotalTime - transition->GetDuration();
    chainCompletionInfo->m_transEnTime		= rstTotalTime;
    chainCompletionInfo->m_transition		= transition;
    chainCompletionInfo->m_srcSeqID			= srcSeqID;
    chainCompletionInfo->m_dstSeqID			= dstSeqID;
    chainCompletionInfo->SetNextName(dstSeq->GetName());

    rstSrcBeginTime = completionInfo.m_dstTimeTransitionComplete;

    return chainCompletionInfo;
}

void cjAnimator::ProcessCallbacks()
{
    if(!m_animatorCB || m_timelineDats.IsEmpty())
        return;

    cjBool callbackGenerated = cjFalse;

    while(!m_timelineDats.IsEmpty())
    {
		TimelineDatLst::Iter headIter = m_timelineDats.Begin();
        cjAnimatorTimeline& cur = *(*m_timelineDats.Begin());

        if(m_curTime >= cur.m_evtTime)
        {
            switch(cur.m_evtType)
            {
            case cjAnimatorAnim_Activated:
				m_animatorCB->Animator_OnAniActivated(this, cur.m_seqID, m_curTime, cur.m_evtTime);
                break;

            case cjAnimatorAnim_Deactivated:
				m_animatorCB->Animator_OnAniDeactivated(this, cur.m_seqID, m_curTime, cur.m_evtTime);
                break;

            case cjAnimatorAnim_Completed:
				cjAssert(cjFalse);
                break;

            case cjAnimatorAnim_TxtKeyEvt:
				m_animatorCB->Animator_OnTxtKeyEvt(this, cur.m_seqID, cur.m_txtKeyMatched, cur.m_nameMatch, 
					m_curTime, cur.m_evtTime);
                break;

            case cjAnimatorAnim_EndOfSeq:
				m_animatorCB->Animator_OnEndOfSeq(this, cur.m_seqID, m_curTime, cur.m_evtTime);
                break;

			default:
				break;
            }

			// if processed head timelinedat
            if(headIter == m_timelineDats.Begin())
                m_timelineDats.RemoveByIter(headIter);

			callbackGenerated = cjTrue;
        }
        else
        {
            break;
        }
    }

    if(callbackGenerated)
        m_needToBuildTimeline = cjTrue;
}

void cjAnimator::BuildTimeline()
{
    cjAssert(m_curTime != cjAnimatorInvalidTime);

    RemoveInactiveExtraSequences();

    m_timelineDats.RemoveAll();

    for(cjAryOff i=0; i<m_animatorCBDats.Count(); i++)
    {
        cjAnimatorCBDat* animatorCBDat = m_animatorCBDats(i);

        if(animatorCBDat)
        {
            cjAniSeqID seqID = animatorCBDat->m_seqID;

            if(seqID == cjAniSeqID_AnySeq)
            {
                SeqIDToSeqRPtrHash::Iter iter = m_seqMap.GetLast();
				SeqIDToSeqRPtrHash::Pair* v;

                while(m_seqMap.GetNext(iter, v))
                {                    
					seqID = v->m_key;
                    AddTimelineDataObject(animatorCBDat->m_evtType, seqID, animatorCBDat->GetNameMatch());
                }
            }
            else
            {
                AddTimelineDataObject(animatorCBDat->m_evtType, seqID, animatorCBDat->GetNameMatch());
            }
        }
    }

	TimelineDatLst::Iter timelineIter = m_timelineDats.End();

    if(timelineIter)
        m_timelineUpdateTime = m_timelineDats.GetBackV().m_evtTime;
    else
        m_timelineUpdateTime = CJ_FTIME_INFINITY;
}

void cjAnimator::AddTimelineDataObject(cjAnimatorEvtType evtType, cjAniSeqID seqID, cjNameMatch* refNameMatch)
{
    cjFLOAT evtTime = GetNextEventTime(evtType, seqID, refNameMatch);

    if(evtTime != cjAnimatorInvalidTime)
    {
        cjBool isAddded = cjFalse;
        cjAnimatorTimeline* newTimelineDat = cjNULL;
		
		TimelineDatLst::Iter iter = m_timelineDats.Begin();
		cjAnimatorTimeline tmpTimeLine;

        while(iter)
        {
            cjAnimatorTimeline* oldTimelineDat = &m_timelineDats.GetValue(iter);

            if(oldTimelineDat->m_evtTime > evtTime)
            {
				TimelineDatLst::Iter insIter;
                insIter = m_timelineDats.PushBefore(iter, tmpTimeLine);
				newTimelineDat = &m_timelineDats.GetValue(insIter);

				isAddded = cjTrue;
                break;
            }

			m_timelineDats.GetNext(iter);
        }

        if(!isAddded)
		{			
            m_timelineDats.PushBack(tmpTimeLine);
			newTimelineDat = &m_timelineDats.GetBackV();
		}

		cjName txtKeyMatched(cjName_None);

        if(refNameMatch)
            txtKeyMatched = refNameMatch->GetLastMatchedName();

        if(newTimelineDat)
            newTimelineDat->SetValues(evtTime, evtType, seqID, refNameMatch, txtKeyMatched);
    }
}

void cjAnimator::RemoveTimelineDataObjects(cjAnimatorEvtType evtType, cjAniSeqID seqID, cjNameMatch* noRefNameMatch)
{
    if(evtType != cjAnimatorAnim_TxtKeyEvt)
        noRefNameMatch = cjNULL;

	TimelineDatLst::Iter iter = m_timelineDats.Begin();

	while(iter)
	{
		cjAnimatorTimeline& curTimeline = m_timelineDats.GetValue(iter);

		if(curTimeline.m_evtType == evtType && 
			(curTimeline.m_seqID == seqID || seqID == cjAniSeqID_AnySeq) && 
            (!noRefNameMatch || curTimeline.m_nameMatch == noRefNameMatch))
        {
            iter = m_timelineDats.RemoveByIter(iter);
        }
		else
		{
			m_timelineDats.GetNext(iter);
		}
	}
}


cjAniTxtMarkerPair* cjAnimator::GetNextBlendPair(cjUINT32 transSrcID, cjUINT32, cjAniTransition* transition, 
	cjFLOAT etime, cjFLOAT& nextBlendPairTime)
{
    // find lowest blend pair
    if(transition->GetType() != cjAniTransitionType_Change ||
        transition->GetBlendPairs().IsEmpty() ||
        !(transition->GetBlendPairs())(0)->m_startMarker.IsValid())
    {
        return cjNULL;
    }

    cjAniSeq* srcSeq = GetSeq(transSrcID);
    cjFLOAT srcDuration = srcSeq->GetDuration();
    
    cjAssert(etime >= 0.0f);
    nextBlendPairTime = etime;

    if(etime > srcDuration)
    {
        if(srcSeq->GetLoopType() == cjAniCntr::ANILOOP_ONE)
            return cjNULL;
        else
			etime = cjGetFMODTime(etime, srcDuration);
    }

    cjAniTxtMarkerPair* smallestBlendPair = cjNULL;
    cjFLOAT smallestBlendPairTime = CJ_FTIME_INFINITY;

    for(cjAryOff i=0; i<transition->GetBlendPairs().Count(); i++)
    {
        cjAniTxtMarkerPair* blendPair = transition->GetBlendPairs()(i);
        cjFLOAT blendPairTime = srcSeq->GetAniNoteTime(blendPair->m_startMarker) - etime;

        if(blendPairTime < 0.0f)
            blendPairTime += srcDuration;

        if(blendPairTime < smallestBlendPairTime)
        {
            smallestBlendPairTime = blendPairTime;
            smallestBlendPair = blendPair;
        }
    }
    
    nextBlendPairTime += smallestBlendPairTime;
    cjAssert(smallestBlendPair);

    return smallestBlendPair;
}

cjFLOAT cjAnimator::GetNextAnimActivatedTime(cjAniSeqID seqID)
{
    if(seqID == cjAniSeqID_InvalidSeq)
        return cjAnimatorInvalidTime;

    cjAssert(m_curTime != cjAnimatorInvalidTime);

    cjAniCntrSeqRPtr extCntrSeq = GetExtraSeq(seqID);

    if(extCntrSeq)
    {
        if(extCntrSeq->GetSeqState() == cjAniSeqState_InActive)
        {
            extCntrSeq->RemoveActivationCallback(this);
            m_extCntrSeqMap.Remove(seqID);

			return cjAnimatorInvalidTime;
        }

        if(extCntrSeq->GetSeqState() == cjAniSeqState_EaseIn)
        {
            cjFLOAT enTime = extCntrSeq->GetEaseEndTime();

            if(enTime > m_curTime)
                return enTime;
            else
                return cjAnimatorInvalidTime;
        }

        return cjAnimatorInvalidTime;
    }
    
    if(seqID == m_curSeqID)
        return cjAnimatorInvalidTime;

    if(seqID == m_nextSeqID)
        return m_transEnTime;

    if(m_nextSeqID == cjAniSeqID_InvalidSeq || !m_chainIDs.Find(seqID))
        return cjAnimatorInvalidTime;

    cjFLOAT animActivatedTime = m_transEnTime;
    cjAniTransition* transition = m_animatorDat->GetTransition(m_curSeqID, m_nextSeqID);
    cjAssert(transition && transition->GetType() != cjAniTransitionType_Chain);
	
    cjFLOAT transitionStTime = m_transStFrameTime;

    if(transitionStTime == cjAnimatorInvalidTime)
        transitionStTime = 0.0f;

	cjAnimatorCompletionInfo completionInfo;
    FindAniTransitionCompletionInfo(m_curSeqID, m_nextSeqID, transition, 
		transitionStTime, transitionStTime, &completionInfo);
    
	cjFLOAT transCompleteTime = completionInfo.m_dstTimeTransitionComplete;
	cjFLOAT curTransitionTime = 0.0f;

	ChainIDLst::Iter chainIDIter = m_chainIDs.Begin();
	ChainDurationLst::Iter chainDurationIter = m_chainDurations.Begin();

	if(!m_chainDurations.IsEmpty())
	{
		chainDurationIter = m_chainDurations.Begin();
		cjFLOAT duration = m_chainDurations.GetValue(chainDurationIter);

		if(duration == cjAnimatorMaxDuration)
			duration = m_nextSeq->GetDuration() - transCompleteTime;
		else
			duration = m_nextSeq->GetMulByFreq(duration);

		curTransitionTime = transCompleteTime + duration;
		cjAssert(chainIDIter && chainDurationIter);

		m_chainIDs.GetNext(chainIDIter);
		m_chainDurations.GetNext(chainDurationIter);
	}
	
    cjAniSeqID srcSeqID = m_nextSeqID;
    cjAniSeq* srcSeq = GetSeq(m_nextSeqID);

    for(;;)
    {
        if(seqID == srcSeqID)
            return animActivatedTime;

        if(!chainIDIter)
            return cjAnimatorInvalidTime;

		cjAssert(chainDurationIter);

        cjAniSeqID chainSeqID;
		m_chainIDs.GetNextV(chainIDIter, chainSeqID);

        cjFLOAT chainDuration;
		m_chainDurations.GetNextV(chainDurationIter, chainDuration);

        cjAniSeq* chainSeq = GetSeq(chainSeqID);
        cjAssert(chainSeq);

        cjAniTransition* chainTransition = m_animatorDat->GetTransition(srcSeqID, chainSeqID);
        cjAssert(chainTransition && chainTransition->GetType() != cjAniTransitionType_Chain);

        FindAniTransitionCompletionInfo(srcSeqID, chainSeqID, chainTransition, 
			transCompleteTime, curTransitionTime, &completionInfo);

        animActivatedTime += completionInfo.m_timeToCompleteTransition;

        srcSeqID = chainSeqID;
        srcSeq = chainSeq;
        transCompleteTime = completionInfo.m_dstTimeTransitionComplete;

        if(chainDuration == cjAnimatorMaxDuration)
            chainDuration = chainSeq->GetDuration() - transCompleteTime;
        else
            chainDuration = chainSeq->GetMulByFreq(chainDuration);

        curTransitionTime = transCompleteTime + chainDuration;
    }
}

cjFLOAT cjAnimator::GetNextAnimDeactivatedTime(cjAniSeqID seqID)
{
    if(seqID == cjAniSeqID_InvalidSeq)
        return cjAnimatorInvalidTime;

    cjAssert(m_curTime != cjAnimatorInvalidTime);
    cjAniCntrSeqRPtr extCntrSeq = GetExtraSeq(seqID);

    if(extCntrSeq != cjNULL)
    {
        if(extCntrSeq->GetSeqState() == cjAniSeqState_InActive)
        {
            extCntrSeq->RemoveActivationCallback(this);
            m_extCntrSeqMap.Remove(seqID);

            return cjAnimatorInvalidTime;
        }

        if(extCntrSeq->GetSeqState() == cjAniSeqState_EaseOut)
        {
            cjFLOAT fEndTime = extCntrSeq->GetEaseEndTime();

			if(fEndTime > m_curTime)
                return fEndTime;
            else
                return cjAnimatorInvalidTime;
        }

        return cjAnimatorInvalidTime;
    }

    if(seqID == m_curSeqID)
    {
        if(m_nextSeqID == cjAniSeqID_InvalidSeq)
            return cjAnimatorInvalidTime;

        if(m_curTime > m_transStTime)
            return cjAnimatorInvalidTime;

        return m_transStTime;
    }

    if(m_nextSeqID == cjAniSeqID_InvalidSeq ||
        (seqID == m_nextSeqID && m_chainIDs.IsEmpty()) || 
		!m_chainIDs.Find(seqID))
    {
        return cjAnimatorInvalidTime;
    }

    cjFLOAT aniDeactivatedTime = m_transStTime;

    cjAniTransition* transition = m_animatorDat->GetTransition(m_curSeqID,m_nextSeqID);
    cjAssert(transition && transition->GetType() != cjAniTransitionType_Chain);

	cjFLOAT transitionStTime = m_transStFrameTime;
    
	if(transitionStTime == cjAnimatorInvalidTime)
        transitionStTime = 0.0f;

    cjAnimatorCompletionInfo completionInfo;
    FindAniTransitionCompletionInfo(m_curSeqID, m_nextSeqID, transition, 
		transitionStTime, transitionStTime, &completionInfo);

    cjFLOAT transCompleteTime = completionInfo.m_dstTimeTransitionComplete;
    cjFLOAT curTransitionTime = 0.0f;	
    cjFLOAT transitionDuration = transition->GetDuration();

	ChainIDLst::Iter chainIDIter = m_chainIDs.Begin();
	ChainDurationLst::Iter chainDurationIter = m_chainDurations.Begin();

    if(!m_chainDurations.IsEmpty())
    {
		chainDurationIter = m_chainDurations.Begin();
		cjFLOAT duration = m_chainDurations.GetValue(chainDurationIter);

        if(duration == cjAnimatorMaxDuration)
            duration = m_nextSeq->GetDuration() - transCompleteTime;
        else
            duration = m_nextSeq->GetMulByFreq(duration);

        curTransitionTime = transCompleteTime + duration;
        cjAssert(chainIDIter && chainDurationIter);

        m_chainIDs.GetNext(chainIDIter);
        m_chainDurations.GetNext(chainDurationIter);
    }

    cjAniSeqID srcSeqID = m_nextSeqID;
    cjAniSeq* srcSeq = GetSeq(m_nextSeqID);

    for(;;)
    {
        if(!chainIDIter)
            return cjAnimatorInvalidTime;

		cjAssert(chainDurationIter);

        cjAniSeqID chainSeqID;
		m_chainIDs.GetNextV(chainIDIter, chainSeqID);

        cjFLOAT chainDuration;
		m_chainDurations.GetNextV(chainDurationIter, chainDuration);

        cjAniSeq* chainSeq = GetSeq(chainSeqID);
        cjAssert(chainSeq);

        cjAniTransition* chainTransition = m_animatorDat->GetTransition(srcSeqID, chainSeqID);
        cjAssert(chainTransition && chainTransition->GetType() != cjAniTransitionType_Chain);

        FindAniTransitionCompletionInfo(srcSeqID, chainSeqID, chainTransition, 
			transCompleteTime, curTransitionTime, &completionInfo);

        aniDeactivatedTime += (completionInfo.m_srcTransitionOccurTime - transCompleteTime) + transitionDuration;

        if(seqID == srcSeqID)
            return aniDeactivatedTime;

        srcSeqID = chainSeqID;
        srcSeq = chainSeq;
        transCompleteTime = completionInfo.m_dstTimeTransitionComplete;

        if(chainDuration == cjAnimatorMaxDuration)
            chainDuration = chainSeq->GetDuration() - transCompleteTime;
        else
            chainDuration = chainSeq->GetMulByFreq(chainDuration);

        curTransitionTime = transCompleteTime + chainDuration;
        transitionDuration = chainTransition->GetDuration();
    }
}


cjFLOAT cjAnimator::GetNextEndOfSequenceTime(cjAniSeqID seqID)
{
	cjNameMatch endAniNoteKey(cjAniConstants::GetEndTextKey());
    return GetNextTextKeyEventTime(seqID, &endAniNoteKey);
}

void cjAnimator::MakeAniEvtActivated(cjAniSeqID evtSeqID)
{
    for(cjAryOff i=0; i<m_animatorCBDats.Count(); i++)
    {
        cjAnimatorCBDat* animatorCBDat = m_animatorCBDats(i);

        if(animatorCBDat)
        {
            if(animatorCBDat->m_evtType == cjAnimatorAnim_Activated)
            {
                cjAniSeqID seqID = animatorCBDat->m_seqID;

                if(seqID == cjAniSeqID_AnySeq)
                {
                    if(GetExtraSeq(evtSeqID) != cjNULL)
                        m_animatorCB->Animator_OnAniActivated(this, evtSeqID, m_curTime, m_curTime);
                }
                else if(seqID == evtSeqID)
                {
                    m_animatorCB->Animator_OnAniActivated(this, evtSeqID, m_curTime, m_curTime);
                }
            }
        }
    }
}

void cjAnimator::MakeAniEvtDeactivated(cjAniSeqID evtSeqID)
{
    for(cjAryOff i=0; i<m_animatorCBDats.Count(); i++)
    {
        cjAnimatorCBDat* animatorCBDat = m_animatorCBDats(i);

        if(animatorCBDat)
        {
            if(animatorCBDat->m_evtType == cjAnimatorAnim_Deactivated)
            {
                cjAniSeqID seqID = animatorCBDat->m_seqID;

                if(seqID == cjAniSeqID_AnySeq)
                {
                    if(GetExtraSeq(evtSeqID))
                        m_animatorCB->Animator_OnAniDeactivated(this, evtSeqID, m_curTime, m_curTime);
                }
                else if(seqID == evtSeqID)
                {
                    m_animatorCB->Animator_OnAniDeactivated(this, evtSeqID, m_curTime, m_curTime);
                }
            }
        }
    }
}

void cjAnimator::MakeAniEvtCompleted(cjAniSeqID evtSeqID)
{
    for(cjAryOff i=0; i<m_animatorCBDats.Count(); i++)
    {
        cjAnimatorCBDat* animatorCBDat = m_animatorCBDats(i);

        if(animatorCBDat)
        {
            if(animatorCBDat->m_evtType == cjAnimatorAnim_Completed)
            {
                cjAniSeqID seqID = animatorCBDat->m_seqID;

                if(seqID == cjAniSeqID_AnySeq)
                    m_animatorCB->Animator_OnAniCompleted(this, evtSeqID, m_curTime, m_curTime);
                else if(seqID == evtSeqID)
                    m_animatorCB->Animator_OnAniCompleted(this, evtSeqID, m_curTime, m_curTime);
            }
        }
    }
}



//
//
//
//cjAnimator* cjAnimator::Create(const char* pcKFMFilename, cjBool isCumulativeAni, cjBool bLoadFilesFromDisk, 
//    cjFStream* stream, cjAniPoseBind* poseBind)
//{
//    cjAssert(pcKFMFilename);
//	
//
//    cjAnimatorRst eRC, eSuccess = NiKFMTool::KFM_SUCCESS;
//
//    NiKFMToolRPtr spKFMTool = NiNew NiKFMTool;
//    eRC = spKFMTool->LoadFile(pcKFMFilename);
//
//    if(eRC != eSuccess)
//        return cjNULL;
//
//    return Create(spKFMTool, pcKFMFilename, isCumulativeAni, bLoadFilesFromDisk, stream, poseBind);
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
//
//
//
//
//cjAnimator* cjAnimator::Create(NiKFMTool* pkKFMTool, const char* pcKFMFilePath, cjBool isCumulativeAni,
//    cjBool bLoadFilesFromDisk, cjFStream* stream, cjAniPoseBind* poseBind)
//{
//    cjAssert(pkKFMTool && pcKFMFilePath);
//
//    NiFilename kFilename(pcKFMFilePath);
//    char acKFMPath[cjMAX_PATH];
//    NiSprintf(acKFMPath, cjMAX_PATH, "%s%s", kFilename.GetDrive(), kFilename.GetDir());
//    pkKFMTool->SetBaseKFMPath(acKFMPath);
//
//    cjAnimator* animator = new cjAnimator(pkKFMTool, isCumulativeAni);
//
//	if(bLoadFilesFromDisk)
//    {
//        cjBool bDeleteStream = cjFalse;
//
//        if(!stream)
//        {
//            stream = new cjFStream;
//            bDeleteStream = cjTrue;
//        }
//
//        if(!animator->Initialize(*stream, poseBind))
//        {
//            delete animator;
//            return cjNULL;
//        }
//
//        if(bDeleteStream)
//        {
//            delete stream;
//			stream = cjNULL;
//        }
//    }
//
//    return animator;
//}
//
//
//cjBool cjAnimator::LoadNIFFile(cjFStream& stream, cjBool bLoadNIFFile, cjAniPoseBind* poseBind)
//{
//    const char* pcModelPath = m_spKFMTool->GetModelPath();
//
//    if(!pcModelPath)
//        return cjFalse;
//
//    if(bLoadNIFFile)
//    {
//        const char* pcNIFFilename = m_spKFMTool->GetFullModelPath();
//        cjAssert(pcNIFFilename);
//
//        cjBool bSuccess = stream.Load(pcNIFFilename);
//
//		if(!bSuccess)
//            return cjFalse;
//    }
//
//    cjRefObj* pkNIFRoot = NiDynamicCast(cjRefObj, stream.GetObjectAt(0));
//
//    if(!pkNIFRoot)
//        return cjFalse;
//
//    return ChangeNIFRoot(pkNIFRoot, poseBind);
//}
//
//cjBool cjAnimator::Initialize(cjFStream& stream, cjAniPoseBind* poseBind)
//{
//    if(!LoadNIFFile(stream, cjTrue, poseBind))
//        return cjFalse;
//
//	LoadAllSeq(&stream);
//
//    return cjTrue;
//}

//
//cjBool cjAnimator::LoadAllSeq(cjFStream* stream)
//{
//    cjBool bDeleteStream = cjFalse;
//
//    if(!stream)
//    {
//        stream = NiNew cjFStream;
//        bDeleteStream = cjTrue;
//    }
//
//    cjBool bAllSuccessful = cjTrue;
//    cjAniSeqID* pSequenceIDs;
//    cjUINT32 seqCount;
//
//    m_spKFMTool->GetSequenceIDs(pSequenceIDs, seqCount);
//
//    char acLastKFFilename[cjMAX_PATH];
//    acLastKFFilename[0] = '\0';
//
//    for(cjUINT32 i=0; i<seqCount; i++)
//    {
//        cjAniSeqID seqID = pSequenceIDs[ui];
//
//        cjName kKFFilename = m_spKFMTool->GetFullKFFilename(seqID);
//        cjAssert(kKFFilename.IsValid());
//
//        cjBool bLoadKFFile = cjFalse;
//
//        if(cjStrcmp(acLastKFFilename, kKFFilename) != 0)
//        {
//            bLoadKFFile = cjTrue;
//            cjStrcpy(acLastKFFilename, cjMAX_PATH, kKFFilename);
//        }
//
//        if(!AddSeq(seqID, *stream, bLoadKFFile))
//            bAllSuccessful = cjFalse;
//
//    }
//
//    cjMemDeAlloc(pSequenceIDs);
//    
//    if(bDeleteStream)
//        delete stream;
//    
//    return bAllSuccessful;
//}
//
//
//cjBool cjAnimator::AddSeq(cjAniSeqID seqID, cjFStream& stream, cjBool bLoadKFFile)
//{
//    if(!m_aniCntrMan)
//        return cjFalse;
//
//    cjAnimatorDat::Sequence* pkKFMSequence = m_animatorDat->GetSeq(seqID);
//
//    if(!pkKFMSequence)
//        return cjFalse;
//
//    cjName kKFFilename = m_spKFMTool->GetFullKFFilename(seqID);
//    cjAssert(kKFFilename.IsValid());
//
//    if(bLoadKFFile)
//    {
//        cjBool bSuccess = stream.Load(kKFFilename);
//        if(!bSuccess)
//            return cjFalse;
//    }
//
//    cjAniSeqRPtr spSeqData;
//    const cjName& kSeqName = pkKFMSequence->GetSequenceName();
//    
//    if(kSeqName.IsValid())
//    {
//        spSeqData = cjAniSeq::CreateSequenceDataFromFile(stream, kSeqName);
//
//        if(!spSeqData)
//            return cjFalse;
//    }
//    else
//    {
//        cjINT32 iAnimIndex = pkKFMSequence->GetAnimIndex();
//        
//		spSeqData = cjAniSeq::CreateSequenceDataFromFile(stream, iAnimIndex);
//
//		if(!spSeqData)
//            return cjFalse;
//    }
//
//    return ChangeSequenceData(seqID, spSeqData);
//}
//
//cjAnimator* cjAnimator::Clone(NiCloningProcess* pkCloningProcess)
//{
//    // Clone the NIF root.
//    cjBool bDeleteCloningProcess = cjFalse;
//
//    if(!pkCloningProcess)
//    {
//        pkCloningProcess = NiNew NiCloningProcess();
//        bDeleteCloningProcess = cjTrue;
//    }
//
//    pkCloningProcess->m_eCopyType = NiObjectNET::COPY_EXACT;
//
//    cjRefObj* pkNIFRoot = (cjRefObj*) m_animateSceneRoot->Clone(*pkCloningProcess);
//
//    if(bDeleteCloningProcess)
//        delete pkCloningProcess;
//
//    // Get the controller manager from the clone.
//    cjAniCntrMan* pkManager = cjAniCntrMan::FindControllerManager(pkNIFRoot);
//    cjAssert(pkManager);
//
//    cjAnimator* animator	= new cjAnimator(m_spKFMTool, m_isCumulativeAniRequire);
//    animator->m_animateSceneRoot	= pkNIFRoot;
//    animator->m_aniCntrMan	= pkManager;
//    animator->m_animatorCB	= m_animatorCB;
//
//    NiTMapIterator kIter = m_seqMap.GetFirstPos();
//    while(kIter)
//    {
//        cjAniSeqID seqID;
//        cjAniSeqRPtr oriSeq;
//        m_seqMap.GetNext(kIter, seqID, oriSeq);
//
//        cjAniSeq* newSeq = animator->m_aniCntrMan->GetSeqByName(oriSeq->GetName());
//        animator->m_seqMap.SetAt(seqID, newSeq);
//    }
//
//    for(cjUINT32 i=0; i<m_animatorCBDats.Count(); i++)
//    {
//        cjAnimatorCB* animatorCB = m_animatorCBDats(i);
//
//        if(animatorCB)
//            animator->m_animatorCBDats.Push(animatorCB);
//    }
//
//    return animator;
//}


//
//cjBool cjAnimator::ReloadNIFFile(cjFStream* stream, cjBool bLoadNIFFile, cjAniPoseBind* poseBind)
//{
//    if(!bLoadNIFFile && !stream)
//        return cjFalse;
//
//    cjBool bDeleteStream = cjFalse;
//
//    if(!stream)
//    {
//        stream = NiNew cjFStream;
//        bDeleteStream = cjTrue;
//    }
//
//    cjBool bSuccess = LoadNIFFile(*stream, bLoadNIFFile, poseBind);
//
//    if(bDeleteStream)
//        delete stream;
//
//    return bSuccess;
//}
//
//cjBool cjAnimator::ChangeNIFRoot(cjRefObj* pkNIFRoot, cjAniPoseBind* poseBind)
//{
//    if(!pkNIFRoot)
//        return cjFalse;
//
//    cjAssert(m_spKFMTool);
//
//    const char* pcModelRoot = m_spKFMTool->GetModelRoot();
//    if(!pcModelRoot)
//        return cjFalse;
//
//    cjRefObj* pkActorRoot = pkNIFRoot->GetObjectByName(pcModelRoot);
//    if(!pkActorRoot)
//        return cjFalse;
//
//    Reset();
//
//    cjAniCntrManager* pkManager = new cjAniCntrManager(pkActorRoot, m_isCumulativeAniRequire, poseBind);
//
//    if(m_aniCntrMan)
//    {
//        cjUINT32 uiCount = m_aniCntrMan->GetSequenceDataCount();
//        for(cjUINT32 ui = 0; ui < uiCount; ui++)
//        {
//            cjAniSeq* seq = m_aniCntrMan->GetSequenceDataAt(ui);
//            if(!seq || seq->GetTempPose())
//                continue;
//
//            pkManager->AddSequenceData(seq);
//        }
//
//        m_aniCntrMan->RemoveAllSequences();
//        m_aniCntrMan->RemoveAllSequenceDatas();
//    }
//
//    m_animateSceneRoot = pkNIFRoot;
//    m_aniCntrMan = pkManager;
//
//    return cjTrue;
//}
//
//cjBool cjAnimator::LoadSequenceData(cjAniSeqID seqID, cjBool bLoadKFFile, cjFStream* stream)
//{
//    if(!bLoadKFFile && !stream)
//        return cjFalse;
//
//    cjBool bDeleteStream = cjFalse;
//
//	if(!stream)
//    {
//        stream = NiNew cjFStream;
//        bDeleteStream = cjTrue;
//    }
//
//    cjBool bSuccess = AddSequenceData(seqID, *stream, bLoadKFFile);
//
//    if(bDeleteStream)
//        delete stream;
//
//    return bSuccess;
//}