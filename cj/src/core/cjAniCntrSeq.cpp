
#include "cjCorePCH.h"
#include "cjAniCntrSeq.h"


//
// cjAniCntrSeq
//

CJSER_IMPL_CLASS(cjAniCntrSeq, cjRefObj)


//CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjAniCntrSeq)
// no need property copy
//CJSER_IMPL_CLONE_PROPCOPY_END(cjAniCntrSeq)


// recycled
cjAniCntrSeq::cjAniCntrSeq()
{
	m_owner						= cjNULL;
	m_activationID				= INVALID_ACTIVATION_ID;

	m_poseBuf					= cjNULL;
	m_poseBufDatCount			= 0;
	m_lastPoseBufUpdateLOD		= LODSKIPEVALUATOR;
	m_lastPoseBufUpdateTime		= CJ_FTIME_INVALID;

	m_seq						= cjNULL;
	m_seqState					= cjAniSeqState_InActive;
	
	m_spad						= cjNULL;
	m_aniNote					= cjNULL;
	m_pairCntrSeq				= cjNULL;

	m_duration					= 0.0f;
	m_loopType					= cjAniCntr::ANILOOP_LOOP;
	m_frequency					= 1.0f;
	
	m_lastCalcTime				= CJ_FTIME_INVALID;
	m_accumFreqXTime			= CJ_FTIME_INVALID;
	m_curTime					= CJ_FTIME_INVALID;
	
	m_offset					= CJ_FTIME_INVALID;
	m_stTime					= CJ_FTIME_INVALID;
	m_enTime					= CJ_FTIME_INVALID;
	m_dstTime					= CJ_FTIME_INVALID;
	m_holdDestFrame				= cjFalse;

	m_priority					= 0;
	m_weight					= 1.0f;
	m_transSpinner				= 0.0f;
	m_easeSpinner				= 0.0f;
}

cjAniCntrSeq::~cjAniCntrSeq()
{
    PrepareForRecycling();

	// below stuff is not handled by PrepareForRecycling()
    m_spad			= cjNULL;
    m_poseBuf		= cjNULL;
    m_seq			= cjNULL;
    m_owner		=	 cjNULL;
}

cjBool cjAniCntrSeq::SetOwner(cjAniCntrManager* owner) 
{ 
	if(owner && m_owner) 
		return cjFalse;

	m_owner = owner;

	return cjTrue;
}

cjBool cjAniCntrSeq::SetSeq(cjAniSeq* seq)
{
	if(seq && m_seq)
		return cjFalse;

	m_seq = seq; 
	return cjTrue;
}


//
// Activation/Reactivation
//

cjBool cjAniCntrSeq::Reactivate(cjINT32 priority, cjFLOAT weight, cjFLOAT easeInTime, 
	cjAniCntrSeq* timeSyncSeq, cjFLOAT stTime, cjBool transition,  cjBool processCallback)
{
	if(m_seqState != cjAniSeqState_InActive) // Already in animating
        return cjFalse;

    m_pairCntrSeq = cjNULL;

    if(timeSyncSeq)
    {
        if(!VerifyDependencies(timeSyncSeq) || !VerifyMatchingMorphKeys(timeSyncSeq))
            return cjFalse;

		m_pairCntrSeq = timeSyncSeq;
    }

    m_priority = priority;
    SetWeight(weight);

    if(easeInTime > 0.0f)
    {
        if(transition)
            m_seqState = cjAniSeqState_TransitionDst;
        else
            m_seqState = cjAniSeqState_EaseIn;

		m_stTime = CJ_FTIME_INVALID;
        m_enTime = easeInTime;
    }
    else
    {
        m_seqState = cjAniSeqState_Animating;
    }

    if(stTime != CJ_FTIME_INVALID)
    {
        m_offset	= CJ_FTIME_INVALID;
        m_dstTime	= stTime / m_frequency;
    }

    m_lastCalcTime = CJ_FTIME_INVALID;

    if(processCallback)
    {
        cjUINT32 cbCount = m_actvationCB.Count();

		for(cjUINT32 i=0; i<cbCount; i++)
            m_actvationCB(i)->OnAniSeqChange(this, m_seqState);
    }

    return cjTrue;
}

cjBool cjAniCntrSeq::Deactivate(cjFLOAT easeOutTime, cjBool transitionToNext, cjBool holdTransitionFrame, 
	cjBool processCallback)
{
    if(m_seqState == cjAniSeqState_InActive)
        return cjFalse;

    m_holdDestFrame = cjFalse;

    if(easeOutTime > 0.0f)
    {
        if(transitionToNext)
        {
            m_seqState = cjAniSeqState_TransitionSrc;

            if(holdTransitionFrame)
            {
                m_holdDestFrame = cjTrue;

                if(m_dstTime == CJ_FTIME_INVALID)
                {                    
                    if(m_curTime != CJ_FTIME_INVALID)
                        m_dstTime = m_curTime / m_frequency;
                    else
                        m_dstTime = 0;
                }
            }
        }
        else
        {
            m_seqState = cjAniSeqState_EaseOut;
        }

        m_stTime = CJ_FTIME_INVALID;
        m_enTime = easeOutTime;
    }
    else
    {
		// Stop Animation
        if( m_lastCalcTime		!= CJ_FTIME_INVALID && 
			m_offset			!= CJ_FTIME_INVALID	&& 
			m_accumFreqXTime	!= CJ_FTIME_INVALID		)
		{
            m_offset += (m_accumFreqXTime/m_frequency) - m_lastCalcTime;
		}

        m_seqState = cjAniSeqState_InActive;

		m_pairCntrSeq = cjNULL;
        m_dstTime = CJ_FTIME_INVALID;
    }

    if(processCallback)
    {
        cjUINT32 count = m_actvationCB.Count();

        for(cjUINT32 i=0; i<count; i++)
            m_actvationCB(i)->OnAniSeqChange(this, m_seqState);

        if(m_seqState == cjAniSeqState_InActive)
            m_actvationCB.RemoveAll();
    }

    return cjTrue;
}


//
// Blending
//

cjAniCntrSeq* cjAniCntrSeq::StartBlend(cjAniSeq* dstSeq, cjFLOAT duration, const cjNameChar* dstTxtKeyName, 
	cjINT32 priority, cjFLOAT srcWeight, cjFLOAT dstWeight, cjAniCntrSeq* dstTimeSyncSeq, cjFLOAT dstFreq)
{
    cjFLOAT dstTime = 0.0f;

    if(dstTxtKeyName)
    {
		cjFLOAT keyTime = dstSeq->GetAniNoteTime(dstTxtKeyName);

        if(keyTime != CJ_FTIME_INVALID)
			dstTime = keyTime;
    }

    return StartBlend(dstSeq, duration, dstTime, priority, srcWeight, dstWeight, dstTimeSyncSeq, dstFreq);
}

cjAniCntrSeq* cjAniCntrSeq::StartBlend(cjAniSeq* dstSeq, cjFLOAT duration, cjFLOAT dstTime, cjINT32 priority, 
	cjFLOAT srcWeight, cjFLOAT dstWeight, cjAniCntrSeq* dstTimeSyncSeq, cjFLOAT dstFreq)
{
	cjAniCntrSeq* dstCntrSeq = cjAniCntrSeq::Activate(m_owner, dstSeq, priority, dstWeight, duration, 
		dstTimeSyncSeq, dstFreq, dstTime, cjTrue, cjTrue);
	
    if(!dstCntrSeq)
        return cjNULL;

	// stop at once with no process callback
    Deactivate(0.0f, cjFalse, cjFalse, cjFalse);

	// reactivate with no process callback
    if(!Reactivate(priority, srcWeight, 0.0f, cjNULL, CJ_FTIME_INVALID, cjFalse, cjFalse))
    {
        dstCntrSeq->Deactivate(0.0f);
        return cjNULL;
    }

	// easeOutTime=duration, transitionToNext, holdTransition, process callback
    Deactivate(duration, cjTrue, cjTrue, cjTrue);

    return dstCntrSeq;
}

cjAniCntrSeq* cjAniCntrSeq::StartMorph(cjAniSeq* dstSeq, cjFLOAT duration, cjINT32 priority, cjFLOAT srcWeight, 
	cjFLOAT dstWeight, cjFLOAT dstFreq)
{
    cjAssert(dstSeq && m_owner);

    cjAniCntrSeq* dstCntrSeq = cjAniCntrSeq::Activate(m_owner, dstSeq, priority, dstWeight, 
		duration, cjNULL, dstFreq, 0.0f, cjTrue, cjFalse);

    if(!dstCntrSeq)
        return cjNULL;

    Deactivate(0.0f, cjFalse, cjFalse, cjFalse); // don't process callback

    // process callbacks.
    if(!Reactivate(priority, srcWeight, duration, dstCntrSeq, CJANI_SEQUENCE_CUR_FRAME, cjTrue, cjTrue))
    {
        dstCntrSeq->Deactivate(0.0f);
        return cjNULL;
    }

    m_seqState = cjAniSeqState_MorphSrc;

    return dstCntrSeq;	
}

void cjAniCntrSeq::Update(cjFLOAT etime, cjINT32 lod, cjBool updateEvaluators)
{
    if(m_seqState == cjAniSeqState_InActive)
        return;

    if(m_offset == CJ_FTIME_INVALID)
        m_offset = -etime;

    if(m_dstTime != CJ_FTIME_INVALID && !m_holdDestFrame)
    {
        m_offset += m_dstTime;
        m_dstTime = CJ_FTIME_INVALID;
    }

    if(m_stTime == CJ_FTIME_INVALID)
    {
        m_stTime = etime;
        m_enTime = etime + m_enTime;
    }

    m_easeSpinner = 1.0f;
    m_transSpinner = 1.0f;

	switch(m_seqState)
	{
	case cjAniSeqState_EaseIn:
		{
			if(etime < m_enTime)
			{
				cjAssert(etime >= m_stTime && m_enTime != m_stTime);
				m_easeSpinner = (etime - m_stTime) / (m_enTime - m_stTime);
			}
			else
				m_seqState = cjAniSeqState_Animating;
		}
		break;

	case cjAniSeqState_TransitionDst:
		{
			if(etime < m_enTime)
			{
				cjAssert(etime >= m_stTime && m_enTime != m_stTime);
				m_transSpinner = (etime - m_stTime) / (m_enTime - m_stTime);
			}
			else
			{
				if(m_dstTime != CJ_FTIME_INVALID)
				{
					cjAssert(m_holdDestFrame);
					m_holdDestFrame = cjFalse;
					m_offset = -etime + m_dstTime;
					m_dstTime = CJ_FTIME_INVALID;
				}
				m_seqState = cjAniSeqState_Animating;
			}
		}
		break;

	case cjAniSeqState_EaseOut:
		{
			if(etime < m_enTime)
			{
				cjAssert(etime >= m_stTime && m_enTime != m_stTime);
				m_easeSpinner = (m_enTime - etime) / (m_enTime - m_stTime);
			}
			else
			{
				Deactivate(0.0f, cjFalse, cjFalse, cjTrue);
				return;
			}
		}
		break;

	case cjAniSeqState_MorphSrc:
		{
			cjAssert(!"not impl");
		}
		break;

	case cjAniSeqState_TransitionSrc:
		{
			if(etime < m_enTime)
			{
				cjAssert(etime >= m_stTime && m_enTime != m_stTime);
				m_transSpinner = (m_enTime - etime) / (m_enTime - m_stTime);
			}
			else
			{
				Deactivate(0.0f, cjFalse, cjFalse, cjTrue);
				return;
			}
		}
		break;

	case cjAniSeqState_Animating:
		break;

	default:
		cjAssert(cjFalse);
		break;
	}

    if(updateEvaluators)
    {
        cjFLOAT updateTime;

        if(m_dstTime != CJ_FTIME_INVALID)
		{
            updateTime = m_dstTime;
		}
		else if(m_pairCntrSeq)
        {
            if(m_pairCntrSeq->GetLastCalcTime() != m_pairCntrSeq->m_offset + etime)
                m_pairCntrSeq->Update(etime, lod, cjFalse);

            updateTime = FindCorrespondingMorphTime(m_pairCntrSeq, m_pairCntrSeq->m_offset + etime);
            updateTime /= m_frequency;
        }
        else
		{
            updateTime = m_offset + etime;
		}

        cjFLOAT scaledTime = _CalcTime(updateTime);

        m_spad->Update(etime, scaledTime, (cjSWORD)lod, m_poseBuf);
    }
}

cjFLOAT cjAniCntrSeq::FindCorrespondingMorphTime(cjAniCntrSeq* srcCntrSeq, cjFLOAT srcSeqTime)
{
	//cjdefer
	cjAssert(!"not impl");
    return 0.0f;
}

cjFLOAT cjAniCntrSeq::_CalcTime(cjFLOAT etime, cjBool applyTime)
{
	cjFLOAT curTime = 0.0f;
    cjFLOAT accumFreqXTime = m_accumFreqXTime;
    cjFLOAT dtime;

    if(m_lastCalcTime == CJ_FTIME_INVALID)
    {
        accumFreqXTime = 0.0f;
        dtime = etime;
    }
    else
    {
        dtime = etime - m_lastCalcTime;
    }

    accumFreqXTime += m_frequency * dtime;
    curTime = accumFreqXTime;

    switch(m_loopType)
    {
	case cjAniCntr::ANILOOP_LOOP:
		{
			if(m_duration != 0.0f)
			{
				if(curTime != m_duration)
					curTime = cjFMod(curTime, m_duration);

				if(curTime < 0.0f)
					curTime += m_duration;
				//cjprfer  while(curTime < 0.0f) curTime += m_duration;
			}
			else
			{
				curTime = 0.0f;
			}
			break;
		}

	case cjAniCntr::ANILOOP_ONE: 
		break;

	default: break;
    }

    if(curTime > m_duration)
        curTime = m_duration;
    else if(curTime < 0.0f)
        curTime = 0.0f;

    if(applyTime)
    {
        m_accumFreqXTime	= accumFreqXTime;
        m_lastCalcTime		= etime;
        m_curTime			= curTime;
    }

    return curTime;
}

cjBool cjAniCntrSeq::SetLoopType(cjAniCntr::ANILOOP loopType)
{
	if(loopType == cjAniCntr::ANILOOP_ONE || loopType == cjAniCntr::ANILOOP_LOOP)
	{
		m_loopType = loopType;
		return cjTrue;
	}

	m_loopType = cjAniCntr::ANILOOP_ONE;
	return cjTrue;
}

cjFLOAT cjAniCntrSeq::GetAniNoteTime(const cjName& noteKey) 
{
	if(!noteKey.IsValid())
		return CJ_FTIME_INVALID;

	cjNameMatch match(noteKey);
	return GetAniNoteTime(&match);
}

cjFLOAT cjAniCntrSeq::GetAniNoteTime(cjNameMatch* noRefMatch) 
{
    if(!noRefMatch || !m_aniNote)
		return CJ_FTIME_INVALID;

    cjINT32 noteCount;
	cjAniNote* keys = m_aniNote->GetNoteAry(&noteCount);

    for(cjINT32 i=0; i<noteCount; i++)
    {
		if(noRefMatch->IsMatch(*(keys[i].GetNote())))
            return keys[i].GetTime();
    }

    return CJ_FTIME_INVALID;
}

cjFLOAT	cjAniCntrSeq::GetScaledAniNoteTimeOnlyNotOverETime(const cjName& noteKey, cjFLOAT etime)
{
	cjNameMatch match(noteKey); // ref=0
	return GetScaledAniNoteTimeOnlyNotOverETime(&match, etime);
}

cjFLOAT cjAniCntrSeq::GetScaledAniNoteTimeOnlyNotOverETime(cjNameMatch* noRefMatch, cjFLOAT etime)
{
    cjFLOAT matchTime = 0.0f;
    cjFLOAT dstSTime = _CalcTime(etime + m_offset, cjFalse); // no apply time

    cjAssert(m_loopType == cjAniCntr::ANILOOP_ONE || m_loopType == cjAniCntr::ANILOOP_LOOP);

    cjBool forceGetFirstMatchKey = (m_loopType == cjAniCntr::ANILOOP_LOOP);
    cjBool exactMatch = cjFalse;
    cjBool matchFound = cjFalse;

    if(m_aniNote)
    {
		cjINT32 noteCount;
		cjAniNote* noteAry = m_aniNote->GetNoteAry(&noteCount);

        for(cjINT32 i=0; i<noteCount; i++)
        {
			if(noRefMatch->IsMatch(noteAry[i].GetNote()))
            {
                cjFLOAT noteTime = noteAry[i].GetTime();

                if(noteTime >= dstSTime)
                {
					matchFound	= cjTrue; 
					matchTime	= noteTime; 
					exactMatch	= cjTrue;

                    break;
                }

				// be sure to get at least one result 
                if(forceGetFirstMatchKey)
                {
                    matchFound = cjTrue; 
					matchTime = noteTime; 
					forceGetFirstMatchKey = cjFalse;
                }
            }
        }
    }

    if(!matchFound)
        return CJ_FTIME_INVALID;

    if(m_pairCntrSeq)
    {
        cjAniCntrSeq* timeSyncSeq = m_pairCntrSeq;

        while(timeSyncSeq->GetTimeSyncSequence())
            timeSyncSeq = timeSyncSeq->GetTimeSyncSequence();

		// find associated morph frame
        cjFLOAT timeSyncTime = timeSyncSeq->FindCorrespondingMorphTime(this, matchTime / m_frequency);

        cjFLOAT curSyncTime = timeSyncSeq->_CalcTime(timeSyncSeq->GetOffset() + etime, cjFalse);
        cjFLOAT dtSyncFrame = timeSyncTime - curSyncTime;

        if(dtSyncFrame < 0)
        {
            dtSyncFrame += timeSyncSeq->GetDuration();
            cjAssert(dtSyncFrame >= 0);
        }

        cjFLOAT dtTime = dtSyncFrame / timeSyncSeq->GetFrequency();

        return etime + dtTime;
    }

    if(!exactMatch && m_loopType == cjAniCntr::ANILOOP_LOOP)
        matchTime += m_duration;

    cjFLOAT rstETime = ((matchTime - dstSTime) / m_frequency) + etime;

    return rstETime;
}

void cjAniCntrSeq::DisableEvaluator(cjUINT32 evalutorIdx, cjBool invalidatePBDat)
{
	cjAssert(evalutorIdx < GetEvaluatorCount());
    cjAniEvaluator* evaluator = GetEvaluator(evalutorIdx);

    cjAniEvalSPDat* evalSPDatAry = (cjAniEvalSPDat*)m_spad->GetDatBlock(cjAniSPADB_EvaluatorDat);
    cjINT32 evalSPDatCount = m_spad->GetBlockDatCount(cjAniSPADB_EvaluatorDat);

    for(cjINT32 i=0; i<evalSPDatCount; i++)
    {
        if(evalSPDatAry[i].GetEvaluator() == evaluator)
        {
            evalSPDatAry[i].SetLOD(LODSKIPEVALUATOR);

            cjAniSPad::SegHdr* segHdr = (cjAniSPad::SegHdr*)evalSPDatAry[i].GetSegDat();

            if(segHdr)
                segHdr->m_lod = LODSKIPEVALUATOR;
        }
    }

    // Invalidate the associated data items in the pose buffer, if requested.
    if(invalidatePBDat)
    {
        cjAssert(m_owner->GetPoseBind());
        const cjAniPoseBindInfo* bindInfo = m_owner->GetPoseBind()->GetBindInfo(evaluator->GetTag());

        if(bindInfo)
        {
			cjUINT32 maxEvalChannels = evaluator->GetMaxChannels();
            for(cjUINT32 k=0; k<maxEvalChannels; k++)
            {
				cjAniPoseBufChanItem pbChanItem = evaluator->GetPBChannelItemFromChanItem(k);
                if(pbChanItem != cjAniPoseBufChanItem_Invalid)
                {
					cjAniPoseBufHandle pbHandle = bindInfo->GetPBHandle(pbChanItem);

                    if(pbHandle.IsValid())
                        m_poseBuf->SetValid(pbHandle, cjFalse);
                }
            }
        }
    }
}

void cjAniCntrSeq::DisableEvaluatorChannel(cjUINT32 evaluatorIdx, cjUINT32 channel, cjBool invalidatePBDat)
{
    cjAssert(evaluatorIdx < GetEvaluatorCount());
    cjAssert(channel < EVALMAXCHANNELS);
    cjAssert(m_seq);
    cjAssert(m_spad);

    cjAniEvaluator* evaluator = GetEvaluator(evaluatorIdx);
    cjAniEvalSPDat* evalSPDatAry = (cjAniEvalSPDat*)m_spad->GetDatBlock(cjAniSPADB_EvaluatorDat);
	cjUINT32 evalSPDatCount = m_spad->GetBlockDatCount(cjAniSPADB_EvaluatorDat);

    for(cjUINT32 i=0; i<evalSPDatCount; i++)
    {
		if(evalSPDatAry[i].GetEvaluator() == evaluator && evalSPDatAry[i].GetEvalChannel() == (cjAniEvalChannel)channel)
        {
            evalSPDatAry[i].SetLOD(LODSKIPEVALUATOR);
            cjAniSPad::SegHdr* segHdr = (cjAniSPad::SegHdr*)evalSPDatAry[i].GetSegDat();

            if(segHdr)
                segHdr->m_lod = LODSKIPEVALUATOR;

            break;
        }
    }

    if(invalidatePBDat)
    {
        const cjAniPoseBindInfo* bindInfo = m_owner->GetPoseBind()->GetBindInfo(evaluator->GetTag());

        if(bindInfo)
        {
			cjAniPoseBufChanItem pbChanItem = evaluator->GetPBChannelItemFromChanItem(channel);

            if(pbChanItem != cjAniPoseBufChanItem_Invalid)
            {
                cjAniPoseBufHandle pbHandle = bindInfo->GetPBHandle(pbChanItem);

                if(pbHandle.IsValid())
                    m_poseBuf->SetValid(pbHandle, cjFalse);
            }
        }
    }
}

cjAniCntrSeq* cjAniCntrSeq::Activate(cjAniCntrManager* owner, cjAniSeq* seq, cjINT32 priority, 
	cjFLOAT weight, cjFLOAT easeInTime, cjAniCntrSeq* timeSyncCntrSeq, cjFLOAT frequency, 
	cjFLOAT stTime, cjBool transition, cjBool holdTransitionFrame)
{
    cjAssert(owner && seq);
    cjAssert(owner->GetPoseBind());

    if(!owner->IsKnownSeq(seq))
    {
		cjLOG("ERR! - sequence is not in controller manager..");
        return NULL;
    }

    if(timeSyncCntrSeq)
    {
        if(!seq->CheckMatchMorphAniNote(timeSyncCntrSeq))
            return cjNULL;
    }

	cjAniCntrSeqRPtr spCntrSeq = owner->GetFreeCntrSeq();

    if(spCntrSeq)
    {
        spCntrSeq->m_poseBufDatCount			= 0;
        spCntrSeq->m_lastPoseBufUpdateLOD		= LODSKIPEVALUATOR;
        spCntrSeq->m_lastPoseBufUpdateTime		= CJ_FTIME_INVALID;
        spCntrSeq->m_transSpinner				= 0.0f;
		spCntrSeq->m_easeSpinner				= 0.0f;
        spCntrSeq->m_accumFreqXTime				= CJ_FTIME_INVALID;
		spCntrSeq->m_curTime					= CJ_FTIME_INVALID;
        spCntrSeq->m_offset						= CJ_FTIME_INVALID;
        spCntrSeq->m_stTime						= CJ_FTIME_INVALID;
        spCntrSeq->m_enTime						= CJ_FTIME_INVALID;
	}
    else
    {
        spCntrSeq = new cjAniCntrSeq();
    }

	cjAniCntrSeq* cntrSeq = spCntrSeq;

    cntrSeq->m_activationID			= owner->GetNextActivationID();
    cntrSeq->m_seq					= seq;
    cntrSeq->m_priority				= priority;
    cntrSeq->SetWeight(weight);
    cntrSeq->m_duration				= seq->GetDuration();
	cntrSeq->SetLoopType(seq->GetLoopType());
	cntrSeq->m_aniNote				= seq->GetAniNote();
	    
	if(frequency == CJANI_SEQUENCE_DAT_FREQ)
        cntrSeq->m_frequency = seq->GetFrequency();
    else
		cntrSeq->m_frequency = frequency;

	cjAssert(cntrSeq->m_frequency > 0);

    cntrSeq->m_dstTime = stTime / cntrSeq->m_frequency;
    cntrSeq->m_holdDestFrame = cjFalse;

    if(easeInTime > 0.0f)
    {
        if(transition)
        {
            cntrSeq->m_seqState			= cjAniSeqState_TransitionDst;
            cntrSeq->m_holdDestFrame	= holdTransitionFrame;
        }
        else
        {
            cntrSeq->m_seqState = cjAniSeqState_EaseIn;
        }

        cntrSeq->m_stTime = CJ_FTIME_INVALID;
        cntrSeq->m_enTime = easeInTime;
    }
    else
    {
        cntrSeq->m_seqState = cjAniSeqState_Animating;
    }

    cntrSeq->m_lastCalcTime = CJ_FTIME_INVALID;
	cntrSeq->m_pairCntrSeq = timeSyncCntrSeq;
    
    owner->AddCntrSeq(cntrSeq);

    cjAniPoseBind* poseBind = owner->GetPoseBind();
    if(cntrSeq->m_poseBuf)
    {
        cjAssert(poseBind == cntrSeq->m_poseBuf->GetPoseBind());

		if(!cntrSeq->m_poseBuf->AddNewDatsFromPoseBind(cjFalse, cjFalse))
			cntrSeq->m_poseBuf->InvalidateAllDats();
    }
    else
    {
        cntrSeq->m_poseBuf = new cjAniPoseBuf(poseBind);
    }

    if(cntrSeq->m_spad)
        cntrSeq->m_spad->InitPooledSPad(cntrSeq);
    else
        cntrSeq->m_spad = new cjAniSPad(cntrSeq);

    cjUINT32 callbackCount = seq->GetActivationCBCount();

    for(cjUINT32 i=0; i<callbackCount; i++)
    {
        cjAniSeqCB* callback;
        cjAniCntrManager* manager;

        seq->GetActivationCB(i, callback, manager);

        if(!manager || manager == owner)
            cntrSeq->AddActivationCallback(callback);
    }

    callbackCount = cntrSeq->m_actvationCB.Count();

    for(cjUINT32 i=0; i<callbackCount; i++)
        cntrSeq->m_actvationCB(i)->OnAniSeqChange(cntrSeq, cntrSeq->m_seqState);

    return cntrSeq;
}

cjBool cjAniCntrSeq::IsMoreImportantThan(const cjAniCntrSeq* sequence) const
{
    if(m_seqState == cjAniSeqState_InActive)
        return cjFalse;

	if(sequence->GetSeqState() == cjAniSeqState_InActive)
        return cjTrue;

    if(m_priority < sequence->GetPriority())
        return cjFalse;

	if(m_priority > sequence->GetPriority())
        return cjTrue;

    if(m_easeSpinner < sequence->GetEaseSpinner())
        return cjFalse;

	if(m_easeSpinner > sequence->GetEaseSpinner())
        return cjTrue;
    
    if(GetSpinnerScaledWeight() > sequence->GetSpinnerScaledWeight())
        return cjTrue;

    return cjFalse;
}

void cjAniCntrSeq::PrepareForRecycling()
{
    m_actvationCB.RemoveAll();

    if(m_spad)
        m_spad->ResetAllEvalSpadDat();

	m_pairCntrSeq = cjNULL;
    m_aniNote = cjNULL;
}

cjBool cjAniCntrSeq::CheckMatchMorphTextKeys(cjAniCntrSeq* timeSyncCntrSeq)
{
    cjAssert(timeSyncCntrSeq);

	cjExtDatAniNote* noteExtDatA = timeSyncCntrSeq->GetAniNote();
    cjExtDatAniNote* noteExtDatB = m_aniNote;

    if(!noteExtDatA || !noteExtDatB)
        return cjFalse;

    cjINT32 noteCountA, noteCountB;

	cjAniNote* noteAryA = noteExtDatA->GetNoteAry(&noteCountA);
    cjAniNote* noteAryB = noteExtDatB->GetNoteAry(&noteCountB);

    const cjNameChar* morphTextKey	= *(cjAniConstants::GetMorphTextKey());
    cjINT32 morphTextKeyLen			= cjAniConstants::GetMorphTextKeyLen();
    cjBool morphKeysExistA			= cjFalse;

    for(cjINT32 i=0; i< noteCountA; i++)
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

        if(!cjStrncmp(noteB, morphTextKey, morphTextKeyLen))
        {
            morphKeysExistB = cjTrue;

            cjBool found = cjFalse;
            for(cjINT32 k=0; k< noteCountA; k++)
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

cjBool cjAniCntrSeq::VerifyDependencies(cjAniCntrSeq* cntrSeq)
{
    cjAniCntrSeq* pairCntrSeq = cntrSeq->m_pairCntrSeq;

    if(pairCntrSeq)
    {
        if(pairCntrSeq == this)
            return cjFalse;
        else
            return VerifyDependencies(pairCntrSeq);
    }

    return cjTrue;
}

cjBool cjAniCntrSeq::VerifyMatchingMorphKeys(cjAniCntrSeq* timeSyncSeq)
{
	return cjFalse;
}

