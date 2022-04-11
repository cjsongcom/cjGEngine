
#include "cjCorePCH.h"
#include "cjAniDef.h"


//
// cjAniEvaluatorTag
//

cjAniEvaluatorTag::cjAniEvaluatorTag() 
{ 
	m_hashS=m_hashL=0; 
}

cjAniEvaluatorTag::cjAniEvaluatorTag(const cjName& id, const cjName& propRttiName, 
	const cjName& cntrRttiName, const cjName& cntrID, const cjName& evaluatorID)
{
	m_id			= id;
	m_propRttiName	= propRttiName;
	m_cntrRttiName	= cntrRttiName;
	m_cntrID		= cntrID;
	m_evaluatorID	= evaluatorID;

	m_hashS = 0;
	m_hashL = 0;

	DoHash();
}

cjAniEvaluatorTag::cjAniEvaluatorTag(const cjAniEvaluatorTag& rhs)
{
	operator=(rhs);
}

cjAniEvaluatorTag& cjAniEvaluatorTag::operator=(const cjAniEvaluatorTag& rhs)
{
	if(this == &rhs)
		return *this;

	m_id			= rhs.m_id;
	m_propRttiName	= rhs.m_propRttiName;
	m_cntrRttiName	= rhs.m_cntrRttiName;
	m_cntrID		= rhs.m_cntrID;
	m_evaluatorID	= rhs.m_evaluatorID;

	m_hashS			= rhs.m_hashS;
	m_hashL			= rhs.m_hashL;

	return *this;
}

cjBool cjAniEvaluatorTag::IsEqual(const cjAniEvaluatorTag& dst)
{
	if(	m_id			!= dst.m_id				||
		m_propRttiName	!= dst.m_propRttiName	||
		m_cntrRttiName	!= dst.m_cntrRttiName	||
		m_cntrID		!= dst.m_cntrID			||
		m_evaluatorID	!= dst.m_evaluatorID	)
	{
		return cjFalse;
	}

	return cjTrue;
}

void cjAniEvaluatorTag::Reset()
{
	m_id.SetNone();
	m_propRttiName.SetNone();
	m_cntrRttiName.SetNone();
	m_cntrID.SetNone();
	m_evaluatorID.SetNone();

	m_hashS = m_hashL = 0;
}

void cjAniEvaluatorTag::DoHash()
{
    cjWORD hash = GetHash(*m_id, *m_propRttiName, *m_cntrRttiName, *m_cntrID, *m_evaluatorID);

    m_hashS = (cjWORD)(hash % cjAniEvalHashBucketSizSmall);
    m_hashL = (cjWORD)(hash % cjAniEvalHashBucketSizLarge);
}

cjWORD cjAniEvaluatorTag::GetHash(const cjNameChar* id, const cjNameChar* propRttiName,
	const cjNameChar* cntrRttiName, const cjNameChar* cntrID, const cjNameChar* evaluatorID)
{
	cjUINT32 hash = 0;

    if(id) 
	{
        while(*id) 
			hash = (hash << 5) + hash + *id++;
    }

    if(propRttiName)
    {
        while(*propRttiName)
            hash = (hash << 5) + hash + *propRttiName++;
    }

    if(cntrRttiName)
    {
        while(*cntrRttiName)
            hash = (hash << 5) + hash + *cntrRttiName++;
    }

    if(cntrID)
    {
        while(*cntrID)
            hash = (hash << 5) + hash + *cntrID++;
    }

    if(evaluatorID)
    {
        while(*evaluatorID)
            hash = (hash << 5) + hash + *evaluatorID++;
    }

    return (cjWORD)hash;
}


//
// cjAniEvalSPDat
//





//
// cjAniConstants
//

cjName			cjAniConstants::ms_transformCntrID			= cjName(cjName_cjAniTMCntrID);

cjName			cjAniConstants::ms_nonAccumSuffix			= cjName(cjName_cjAniNonAccumSuffix);
const cjINT32	cjAniConstants::ms_nonAccumSuffixNameLen	= 9;

cjName			cjAniConstants::ms_stTxtKey					= cjName(cjName_cjAniStTxtKey);
cjName			cjAniConstants::ms_enTxtKey					= cjName(cjName_cjAniEnTxtKey);

cjName			cjAniConstants::ms_morphTextKey				= cjName(cjName_cjAniMorphTxtKey);
const cjINT32	cjAniConstants::ms_morphTextKeyNameLen		= 6; // "morph:"

cjName			cjAniConstants::ms_flipAniCntrName			= cjName(cjName_cjFlipAniCntr);


// 
// cjAniPoseBufHandle
// 


//
// cjAnimatorCBDat
//

CJSER_IMPL_CLASS(cjAnimatorCBDat, cjRefObj)


//
// cjAniTransition
//

cjAniTransition cjAniTransition::ms_default;

const cjAniTransition& cjAniTransition::GetDefault()
{
	static int inited = 0;

	if(inited)
	{
		ms_default.m_type = cjAniTransitionType_Chain;
		ms_default.m_duration = 0.01f;

		inited = 1;
	}

	return ms_default;
}


//
// cjAnimatorDat
//

CJSER_IMPL_CLASS(cjAnimatorDat, cjRefObj)

const cjUINT32 cjAnimatorDat::MAX_CHAIN_SEQID_COUNT = 1000;

cjAnimatorDat::cjAnimatorDat()
{
}

cjAnimatorDat::~cjAnimatorDat()
{
}

cjAniTransition* cjAnimatorDat::GetTransition(cjAniSeqID srcID, cjAniSeqID dstID) const
{
	Sequence* seq;

	if(!m_sequenceMap.Find(srcID, &seq))
		return cjNULL;

	cjAniTransition* transition;

	if(!seq->m_transitions.Find(dstID, &transition))
		return cjNULL;

	return transition;
}

cjAnimatorRst cjAnimatorDat::IsTransitionNeed(cjAniSeqID srcID, cjAniSeqID dstID, 
	cjBool& rstIsTransitionNeed) const
{
	Sequence* seq;

	if(!m_sequenceMap.Find(srcID, &seq))
	{
		rstIsTransitionNeed = cjFalse;
		return cjAnimatorRst_Err_Invalid_Sequence;
	}

	if(seq->m_transitions.Find(dstID))
		rstIsTransitionNeed = cjTrue;
	else
		rstIsTransitionNeed = cjFalse;

	return cjAnimatorRst_Ok;
}

cjBool cjAnimatorDat::IsValidChainTransition(cjAniSeqID srcID, cjAniSeqID dstID, 
	cjAniTransition* transition)
{
    cjAniSeqIDAry chainIDs;

    RecursiveCollectChainIDs(srcID, dstID, transition, chainIDs);

    if(chainIDs.Count() > MAX_CHAIN_SEQID_COUNT)
        return cjFalse;

    return cjTrue;
}

void cjAnimatorDat::RecursiveCollectChainIDs(cjAniSeqID srcID, cjAniSeqID dstID, 
	cjAniTransition* transition, cjAniSeqIDAry& chainIDs)
{
    if(transition->GetType() != cjAniTransitionType_Chain)
        return;

    if(chainIDs.Count() > MAX_CHAIN_SEQID_COUNT)
        return;

    cjAryOff chainCount = transition->GetChains().Count();

    if(chainCount == 0)
        return;

    chainIDs.Push(srcID);

    for(cjAryOff i=0; i<chainCount; i++)
    {
        cjAniSeqID curSeqID = transition->GetChains()(i).m_seqID;

        cjAniTransition* tmpTransition = GetTransition(srcID, curSeqID);

        if(tmpTransition && tmpTransition->GetType() == cjAniTransitionType_Chain)
            RecursiveCollectChainIDs(srcID, curSeqID, tmpTransition, chainIDs);
        else
            chainIDs.Push(curSeqID);

		srcID = curSeqID;
    }

	// finally
    chainIDs.Push(dstID);
}

cjAnimatorRst cjAnimatorDat::UpdateSeqID(cjAniSeqID oldSeqID, cjAniSeqID newSeqID)
{
    Sequence* seq;
	
	if(!m_sequenceMap.Find(oldSeqID, &seq))
		return cjAnimatorRst_Err_Invalid_Sequence;
	
    m_sequenceMap.Remove(oldSeqID);

	seq->m_seqID = newSeqID;
    m_sequenceMap.Set(newSeqID, seq);

    UpdateTransitionsContainingSeq(oldSeqID, newSeqID);
//  UpdateSeqGrpsContainingSeq(oldSeqID, newSeqID);

    return cjAnimatorRst_Ok;
}

void cjAnimatorDat::UpdateTransitionsContainingSeq(cjAniSeqID oldSeqID, cjAniSeqID newSeqID)
{
    AniID2SeqMap::Iter iter = m_sequenceMap.GetLast();
	AniID2SeqMap::Pair* v;
	cjAniSeqID srcSeqID;
    Sequence* seq;

    while(m_sequenceMap.GetNext(iter, v))
    {
		srcSeqID = v->m_key;
		seq = v->m_val;

		cjAnimatorDat::Sequence::Transitions::Iter transIter = seq->m_transitions.GetLast();
		cjAnimatorDat::Sequence::Transitions::Pair* tv;
		cjAniSeqID dstSeqID;
        cjAniTransition* transition;
		
        while(seq->m_transitions.GetNext(transIter, tv))
        {
			dstSeqID = tv->m_key;
			transition = tv->m_val;

            if(dstSeqID == oldSeqID)
            {
                seq->m_transitions.Remove(oldSeqID);
                seq->m_transitions.Set(newSeqID, transition);
            }

            for(cjAryOff i=0; i<transition->GetChains().Count(); i++)
            {
                cjAniSeqIDChainInfo& chainInfo = transition->GetChains()(i);
                
                if(chainInfo.m_seqID == oldSeqID)
                    chainInfo.m_seqID = newSeqID;
            }
        }
    }
}


//
// cjAnimatorChainCompletionInfo
//

CJSER_IMPL_CLASS(cjAnimatorChainCompletionInfo, cjRefObj)


//
// cjAnimatorCompletionInfo
//

CJSER_IMPL_CLASS(cjAnimatorCompletionInfo, cjRefObj)



//
// cjAnimatorTimeline
//



