
#include "cjCorePCH.h"
#include "cjAniPose.h"


//
// cjAniPoseBindInfo
//

cjAniPoseBindInfo::cjAniPoseBindInfo()
{
	m_lod = 0;

    m_nextHash			= cjAniPoseInvalidHashIndex;

    m_pbIndicesForTM[0] = cjAniPoseInvalidChanIndex;
    m_pbIndicesForTM[1] = cjAniPoseInvalidChanIndex;
}

void cjAniPoseBindInfo::SetPBHandleInfo(const cjAniPoseBufHandle& pbHandle)
{
    if(IsTransformData())
    {
        cjAniPoseBufChanItem pbChanItem = pbHandle.GetChannelItem();

        if(pbChanItem == cjAniPoseBufChanItem_Rot) // rot
        {
            m_pbHandle = pbHandle;
        }
        else if(pbChanItem == cjAniPoseBufChanItem_Vec3P) // pos
        {
			// pos
            m_pbIndicesForTM[0] = pbHandle.GetAuxIndex();
        }
        else if(pbChanItem == cjAniPoseBufChanItem_Vec3S) // scale
        {
            cjAssert(pbChanItem == cjAniPoseBufChanItem_Vec3S);

			// scale
            m_pbIndicesForTM[1] = pbHandle.GetAuxIndex();
        }
    }
    else
    {
        m_pbHandle = pbHandle;
    }
}
    
cjAniPoseBufHandle cjAniPoseBindInfo::GetFirstPBHandle() const
{
    cjAniPoseBufHandle pbHandle = m_pbHandle;

    if(!m_pbHandle.IsValid())
    {
        if(m_pbIndicesForTM[0] != cjAniPoseInvalidChanIndex) // pos
        {
            cjAssert(IsTransformData());
            pbHandle.Init(cjAniPoseBufChanItem_Vec3P, m_pbIndicesForTM[0]);
        }
        else if(m_pbIndicesForTM[1] != cjAniPoseInvalidChanIndex) // scale
        {
            cjAssert(IsTransformData());
            pbHandle.Init(cjAniPoseBufChanItem_Vec3S, m_pbIndicesForTM[1]);
        }
    }

    return pbHandle;
}

cjAniPoseBufHandle cjAniPoseBindInfo::GetPBHandle(cjAniPoseBufChanItem pbChanItem) const
{
    cjAniPoseBufHandle pbHandle;

    if(m_pbHandle.GetChannelItem() == pbChanItem)
	{
        pbHandle = m_pbHandle;
	}
    else if(m_pbIndicesForTM[0] != cjAniPoseInvalidChanIndex && pbChanItem == cjAniPoseBufChanItem_Vec3P)
    {   // pos
        cjAssert(IsTransformData());
        pbHandle.Init(cjAniPoseBufChanItem_Vec3P, m_pbIndicesForTM[0]);
    }
    else if(m_pbIndicesForTM[1] != cjAniPoseInvalidChanIndex && pbChanItem == cjAniPoseBufChanItem_Vec3S)
    {  // scale
        cjAssert(IsTransformData());
        pbHandle.Init(cjAniPoseBufChanItem_Vec3S, m_pbIndicesForTM[1]);
    }

    return pbHandle;
}

cjBool cjAniPoseBindInfo::MatchesIDTag(const cjAniEvaluatorTag& tag) const
{
    if(m_id == tag.GetID() && m_propRttiName == tag.GetPropRttiName() && m_cntrRttiName == tag.GetCntrRttiName()
		&& m_cntrID == tag.GetCntrID() && m_evaluatorID == tag.GetEvaluatorID())
    {
        return cjTrue;
    }

    return cjFalse;
}

cjBool cjAniPoseBindInfo::MatchesIDTag(const cjName& id, const cjName& propRttiName, const cjName& cntrRttiName, 
	const cjName& cntrID, const cjName& evaluatorID) const
{
    if(m_id == id && m_propRttiName == propRttiName && m_cntrRttiName == cntrRttiName &&
        m_cntrID == cntrID && m_evaluatorID == evaluatorID)
    {
        return cjTrue;
    }

    return cjFalse;
}

cjBool cjAniPoseBindInfo::IsTransformData() const
{
	//cjdbgcheck
	cjAssert(!cjStrcmp(cjName(cjName_cjAniTMCntrID), cjAniConstants::GetTransformCntrID()));

    return (m_cntrRttiName == cjAniConstants::GetTransformCntrID());
}

cjBool cjAniPoseBindInfo::IsEqual(const cjAniPoseBindInfo& dst)
{
    if(m_id != dst.m_id || m_propRttiName != dst.m_propRttiName || m_cntrRttiName != dst.m_cntrRttiName 
		|| m_cntrID != dst.m_cntrID || m_evaluatorID != dst.m_evaluatorID || m_lod != dst.m_lod)
    {
        if(m_pbHandle != dst.m_pbHandle || 
			m_pbIndicesForTM[0] != dst.m_pbIndicesForTM[0] ||		// pos
            m_pbIndicesForTM[1] != dst.m_pbIndicesForTM[1]			// scale
		)
        {
            return cjFalse;
        }
    }

    return cjTrue;
}


//
// cjAniPoseBind
//

CJSER_IMPL_CLASS(cjAniPoseBind, cjRefObj)

CJSER_IMPL_SERIALIZE_BEGIN(cjAniPoseBind)
	
	if(ser.IsQuerySerObjAndStr())
	{
//		ser.AddString(m_accumRootName);

//		for(cjUINT32 i=0; i<m_bindInfoCount; i++)
//			m_bindInfoIndexList
		
		return;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		ser << m_bindInfoCount;

		ser << m_accumRootName;
		ser << m_nonAccumStIndex;

		// no need ser << m_lodCount;
				
		ser << m_colorBindCount;
		ser << m_boolBindCount;
		ser << m_floatBindCount;
		ser << m_vec3SBindCount;
		ser << m_vec3PBindCount;
		ser << m_rotBindCount;		
		ser << m_refDatBindCount;

		// ser << m_lod;
		
		if(ser.IsLoading())
		{
			cjWORD chkTotalBindCount;
			ser << chkTotalBindCount;

			ReallocBinds(m_colorBindCount, m_boolBindCount, m_floatBindCount, 
				m_vec3SBindCount, m_vec3PBindCount, m_rotBindCount,
				m_refDatBindCount, m_bindInfoCount);

			if(m_totalBindCount != chkTotalBindCount)
			{
				cjAssert(!"invalid total bindCount..");
				return;
			}			
		}
		else
		{
			ser << m_totalBindCount;
		}

		// cjWORD Array
		for(cjWORD i=0; i<m_totalBindCount; i++)
			ser << m_bindInfoIndexList[i];

		// cjAniPoseBindInfo Array
		for(cjWORD i=0; i<m_bindInfoCount; i++)
			ser << m_bindInfoAry[i];

		ser << m_hashBucketSiz;

		if(m_hashBucketSiz > 0)
		{
			if(ser.IsLoading())
			{
				cjAssert(!m_hash);
				m_hash = (cjWORD*)cjMemAlloc(sizeof(cjWORD) * m_hashBucketSiz);
			}

			for(cjUINT32 i=0; i<m_hashBucketSiz; i++)
				ser << m_hash[i];
		}
	}
	
CJSER_IMPL_SERIALIZE_END(cjAniPoseBind)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjAniPoseBind)

	d->m_accumRootName			= m_accumRootName;
	d->m_nonAccumStIndex		= m_nonAccumStIndex;
	d->m_lodCount				= m_lodCount;

	d->ReallocBinds(m_colorBindCount, m_boolBindCount, m_floatBindCount,
		m_vec3SBindCount, m_vec3PBindCount, m_rotBindCount,
		m_refDatBindCount, m_bindInfoCount);

    for(cjUINT32 i=0; i<m_totalBindCount; i++)
        d->m_bindInfoIndexList[i] = m_bindInfoIndexList[i];

    for(cjUINT32 i=0; i<m_bindInfoCount; i++)
        d->m_bindInfoAry[i] = m_bindInfoAry[i];

	d->m_hashBucketSiz = m_hashBucketSiz;

	if(m_hashBucketSiz)
	{
		d->m_hash = (cjWORD*)cjMemAlloc(sizeof(cjWORD) * m_hashBucketSiz);
		cjMemCpy(d->m_hash, m_hash, sizeof(cjWORD) * m_hashBucketSiz);
	}

CJSER_IMPL_CLONE_PROPCOPY_END(cjAniPoseBind)

CJSER_IMPL_CLONE(cjAniPoseBind)

CJSER_IMPL_CLONE_POST_BEGIN(cjAniPoseBind)
CJSER_IMPL_CLONE_POST_END(cjAniPoseBind)


cjAniPoseBind::cjAniPoseBind()
{
}

cjAniPoseBind::cjAniPoseBind(cjAniCntrManager* cntrMan, const cjRefObj* accumRoot)
{
	m_hash						= cjNULL;
	m_bindInfoIndexList			= 0;
    m_bindInfoAry				= 0;

	m_hashBucketSiz				= 0;
	m_bindInfoCount				= 0;   

    m_accumRootName				= cjName::NullName;
	m_nonAccumStIndex			= 0; 
	m_totalBindCount			= 0;

	m_lodCount					= 1;

	m_colorBindCount			= 0;
	m_colorStIndex				= 0;

	m_boolBindCount				= 0;
	m_boolStIndex				= 0;

	m_floatBindCount			= 0;
	m_floatStIndex				= 0;

	m_vec3SBindCount			= 0;
	m_vec3SStIndex				= 0;

	m_vec3PBindCount			= 0;
	m_vec3PStIndex				= 0;

	m_rotBindCount				= 0;
	m_rotStIndex				= 0;

	m_refDatBindCount				= 0;
	m_refDatStIndex				= 0;

    Init(cntrMan, accumRoot);
}

cjAniPoseBind::~cjAniPoseBind()
{
    Shutdown();
}

void cjAniPoseBind::SyncBindInfoToSeqEvaluator(const cjAniSeq* seq, cjAniCntrManager* cntrMan)
{
    cjAssert(seq);
    cjAssert(cntrMan);

    if(m_hashBucketSiz <= cjAniEvalHashBucketSizSmall)
    {
        cjUINT32 bucketSiz = seq->GetEvaluatorCount();

        if(bucketSiz < m_bindInfoCount)
            bucketSiz = m_bindInfoCount;

        if(bucketSiz <= cjAniEvalHashBucketSizSmall + 5)
            bucketSiz = cjAniEvalHashBucketSizSmall;
        else
            bucketSiz = cjAniEvalHashBucketSizLarge;

		ReallocHashTable(bucketSiz);
    }

    cjAssert(m_hashBucketSiz > 0 && m_hash);

    cjWORD newColorBindCount	= 0;
    cjWORD newBoolBindCount		= 0;
    cjWORD newFloatBindCount	= 0;
    cjWORD newVec3SBindCount	= 0;
	cjWORD newVec3PBindCount	= 0;
    cjWORD newRotBindCount		= 0;
    cjWORD newRefDatBindCount	= 0;
    cjWORD newBindInfoCount		= 0;

    cjUINT32 evaluatorCount = seq->GetEvaluatorCount();
    cjBYTE* needRebuildBindAry = cjNULL;

    if(evaluatorCount > 0)
    {
        needRebuildBindAry = (cjBYTE*)cjMemAlloc(sizeof(cjBYTE) * evaluatorCount);
        memset(needRebuildBindAry, 0, sizeof(cjBYTE) * evaluatorCount);
    }

    for(cjUINT32 i=0; i<evaluatorCount; i++)
    {
        cjAniEvaluator* evaluator = seq->GetEvaluator(i);
        cjAssert(evaluator);

        const cjAniEvaluatorTag& evalTag = evaluator->GetTag();
        cjAniPoseBindInfo* bindInfo = GetBindInfo(evalTag);

        if(!bindInfo)
            newBindInfoCount++;


		//
		// what is base channel item
		//

        cjAniPoseBufChanItem pbChanItem = evaluator->GetPBChannelItemFromChanItem(cjAniEvalChan_Base);

        if((pbChanItem != cjAniPoseBufChanItem_Invalid) && 
			(!bindInfo || !bindInfo->GetPBHandle(pbChanItem).IsValid()))
        {
            switch(pbChanItem)
            {
            case cjAniPoseBufChanItem_Color:	newColorBindCount++;		break;
            case cjAniPoseBufChanItem_Bool:		newBoolBindCount++;			break;
            case cjAniPoseBufChanItem_Float:	newFloatBindCount++;		break;
            case cjAniPoseBufChanItem_Vec3S:	newVec3SBindCount++;		break;
			case cjAniPoseBufChanItem_Vec3P:	newVec3PBindCount++;		break;
            case cjAniPoseBufChanItem_Rot:		newRotBindCount++;			break;
            case cjAniPoseBufChanItem_Ref:		newRefDatBindCount++;		break;
            default:	cjAssert(cjFalse);									break;
            }

			needRebuildBindAry[i] = 1; // needs building new bind
        }


		//
		// if evaluator is TransformEvaluator,  Handle Rotation, Scale Channel
		//
        if(evaluator->IsTransformEvaluator())
        {
			// bindInfo->m_pbHandle			= cjAniEvalChan_Rot
			// bindInfo->m_pbIndicesForTM[0]= cjAniEvalChan_Pos
			// bindInfo->m_pbIndicesForTM[1]= cjAniEvalChan_Scale

			// position 
			
			// rotation - cjAniEvalChan_Rot
            pbChanItem = evaluator->GetPBChannelItemFromChanItem(cjAniEvalChan_Rot);
            if(pbChanItem != cjAniPoseBufChanItem_Invalid)
            {
                cjAssert(pbChanItem == cjAniPoseBufChanItem_Rot);
								
                if(!bindInfo || !bindInfo->GetPBHandle(cjAniPoseBufChanItem_Rot).IsValid())
                {
                    newRotBindCount++;
                    needRebuildBindAry[i] = 1;
                }
            }

			// scale - cjAniEvalChan_Scl
            pbChanItem = evaluator->GetPBChannelItemFromChanItem(cjAniEvalChan_Scl);
            if(pbChanItem != cjAniPoseBufChanItem_Invalid)
            {
                cjAssert(pbChanItem == cjAniPoseBufChanItem_Vec3S);
                if(!bindInfo || !bindInfo->GetPBHandle(cjAniPoseBufChanItem_Vec3S).IsValid())
                {
                    newVec3SBindCount++;
                    needRebuildBindAry[i] = 1;
                }
            }
        }
    }

    // rebuild bind array
    cjWORD newTotalBindCount = newColorBindCount + newBoolBindCount + newFloatBindCount 
		+ newVec3SBindCount + newVec3PBindCount + newRotBindCount + newRefDatBindCount;

    if(newTotalBindCount == 0) // no need newerly bind array
    {
        cjMemDeAlloc(needRebuildBindAry); 
        return;
    }

    ReallocBinds(
		m_colorBindCount		+ newColorBindCount,
        m_boolBindCount			+ newBoolBindCount,
        m_floatBindCount		+ newFloatBindCount,
        m_vec3SBindCount		+ newVec3SBindCount,
		m_vec3PBindCount		+ newVec3PBindCount,
        m_rotBindCount			+ newRotBindCount,
        m_refDatBindCount		+ newRefDatBindCount, 
        m_bindInfoCount			+ newBindInfoCount);

    cjRefObjNameHash* objHash = cntrMan->GetNodesHash();
    cjAssert(objHash);

    cjWORD colorBindCount		= m_colorBindCount		- newColorBindCount;
    cjWORD boolBindCount		= m_boolBindCount		- newBoolBindCount;
    cjWORD floatBindCount		= m_floatBindCount		- newFloatBindCount;
	cjWORD vec3SBindCount		= m_vec3SBindCount		- newVec3SBindCount;
    cjWORD vec3PBindCount		= m_vec3PBindCount		- newVec3PBindCount;
    cjWORD rotBindCount			= m_rotBindCount		- newRotBindCount;
    cjWORD refBindCount			= m_refDatBindCount		- newRefDatBindCount;
    cjWORD bindInfoCount		= m_bindInfoCount		- newBindInfoCount;

    for(cjUINT32 i=0; i<evaluatorCount; i++)
    {
        if(!needRebuildBindAry[i])
            continue;

        cjAniEvaluator* evaluator = seq->GetEvaluator(i);
        cjAssert(evaluator);

        cjBool isTMEvaluator = evaluator->IsTransformEvaluator();
        const cjAniEvaluatorTag& evalTag = evaluator->GetTag();

        cjAniPoseBindInfo* bindInfo = GetBindInfo(evalTag);

        if(!bindInfo)
        {
            bindInfo					= m_bindInfoAry + bindInfoCount;
            bindInfo->m_id				= evalTag.GetID();
            bindInfo->m_propRttiName	= evalTag.GetPropRttiName();
            bindInfo->m_cntrRttiName	= evalTag.GetCntrRttiName();
			bindInfo->m_cntrID			= evalTag.GetCntrID();
            bindInfo->m_evaluatorID		= evalTag.GetEvaluatorID();

            bindInfo->m_lod				= (cjSWORD)(m_lodCount - 1);

			cjAssert((cjSWORD)(bindInfo->m_lod) > -10);
			cjAssert(bindInfo->m_lod < 10);

            if(isTMEvaluator)
                objHash->FindObjWithSetObjWhenRootIsSetted(evalTag.GetID());

            cjWORD hashIndex;
            if(m_hashBucketSiz == cjAniEvalHashBucketSizSmall)
            {
				hashIndex = evalTag.GetHashSmall();
            }
            else
            {
                cjAssert(m_hashBucketSiz == cjAniEvalHashBucketSizLarge);
				hashIndex = evalTag.GetHashLarge();
            }

			bindInfo->m_nextHash = m_hash[hashIndex];
            m_hash[hashIndex] = bindInfoCount;
            bindInfoCount++;

            cjAssert(!bindInfo->m_pbHandle.IsValid());
            cjAssert(bindInfo->m_pbIndicesForTM[0] == cjAniPoseInvalidChanIndex); // pos
            cjAssert(bindInfo->m_pbIndicesForTM[1] == cjAniPoseInvalidChanIndex); // scale
        }

        cjAniPoseBufChanItem pbChanItem = evaluator->GetPBChannelItemFromChanItem(cjAniEvalChan_Base);

        if((pbChanItem != cjAniPoseBufChanItem_Invalid) && (!bindInfo->GetPBHandle(pbChanItem).IsValid()))
        {
            cjWORD* bindInfoIndex;
            cjAniPoseBufHandle pbHandle;

            switch(pbChanItem)
            {
            case cjAniPoseBufChanItem_Color:
				{
					bindInfoIndex = m_bindInfoIndexList + m_colorStIndex + colorBindCount;
					pbHandle.Init(cjAniPoseBufChanItem_Color, colorBindCount);
					colorBindCount++;
				}
                break;

            case cjAniPoseBufChanItem_Bool:
				{
					bindInfoIndex = m_bindInfoIndexList + m_boolStIndex + boolBindCount;
					pbHandle.Init(cjAniPoseBufChanItem_Bool, boolBindCount);
					boolBindCount++;
				}
                break;

            case cjAniPoseBufChanItem_Float:
				{
					bindInfoIndex = m_bindInfoIndexList + m_floatStIndex + floatBindCount;
					pbHandle.Init(cjAniPoseBufChanItem_Float, floatBindCount);
					floatBindCount++;
				}
                break;

            case cjAniPoseBufChanItem_Vec3S:
				{
					bindInfoIndex = m_bindInfoIndexList + m_vec3SStIndex + vec3SBindCount;
					pbHandle.Init(cjAniPoseBufChanItem_Vec3S, vec3SBindCount);
					vec3SBindCount++;
				}
                break;

            case cjAniPoseBufChanItem_Vec3P:
				{
					bindInfoIndex = m_bindInfoIndexList + m_vec3PStIndex + vec3PBindCount;
					pbHandle.Init(cjAniPoseBufChanItem_Vec3P, vec3PBindCount);
					vec3PBindCount++;
				}
                break;

            case cjAniPoseBufChanItem_Rot:
				{
					bindInfoIndex = m_bindInfoIndexList + m_rotStIndex + rotBindCount;
					pbHandle.Init(cjAniPoseBufChanItem_Rot, rotBindCount);
					rotBindCount++;
				}
                break;

            case cjAniPoseBufChanItem_Ref:
				{
					bindInfoIndex = m_bindInfoIndexList + m_refDatStIndex + refBindCount;
					pbHandle.Init(cjAniPoseBufChanItem_Ref, refBindCount);
					refBindCount++;
				}
                break;

            default:
                cjAssert(cjFalse);
                continue;
            }

            *bindInfoIndex = (cjWORD)(bindInfo - m_bindInfoAry);
            bindInfo->SetPBHandleInfo(pbHandle);
        }

        if(isTMEvaluator) 
        {
			// by bindInfo->SetPBHandleInfo(pbHandle);

			// Base Channel is already processed
			/*
				case #0
				base channel = cjAniEvalChan_Pos,  
				bindInfo->GetPBHandle()
				  pbHandle.m_poseBufChanItem = cjAniPoseBufChanItem_Rot
				  bindInfo->m_pbIndicesForTM[0] : cjAniPoseBufChanItem_Pos 
				  bindInfo->m_pbIndicesForTM[1] : cjAniPoseBufChanItem_Scale

				  so result of bindInfo->GetPBHandle(cjAniPoseBufChanItem_Rot).IsValid() is cjTrue
			*/

			// Handle Rotation
            pbChanItem = evaluator->GetPBChannelItemFromChanItem(cjAniEvalChan_Rot);
            if(pbChanItem != cjAniPoseBufChanItem_Invalid)
            {
                cjAssert(pbChanItem == cjAniPoseBufChanItem_Rot);
                if(!bindInfo->GetPBHandle(cjAniPoseBufChanItem_Rot).IsValid())
                {
                    m_bindInfoIndexList[m_rotStIndex + rotBindCount] = (cjWORD)(bindInfo - m_bindInfoAry);

                    bindInfo->SetPBHandleInfo(cjAniPoseBufHandle(cjAniPoseBufChanItem_Rot, rotBindCount));
                    rotBindCount++;
                }
            }

			// Handle Scale
            pbChanItem = evaluator->GetPBChannelItemFromChanItem(cjAniEvalChan_Scl);
            if(pbChanItem != cjAniPoseBufChanItem_Invalid)
            {
                cjAssert(pbChanItem == cjAniPoseBufChanItem_Vec3S);
                if(!bindInfo->GetPBHandle(cjAniPoseBufChanItem_Vec3S).IsValid())
                {
                    m_bindInfoIndexList[m_vec3SStIndex + vec3SBindCount] = (cjWORD)(bindInfo - m_bindInfoAry);
                    bindInfo->SetPBHandleInfo(cjAniPoseBufHandle(cjAniPoseBufChanItem_Vec3S, vec3SBindCount));
                    vec3SBindCount++;
                }
            }
        }
    }


	// on assert , there may be evaluator which have dupplicated evalTag
    cjAssert(colorBindCount	== m_colorBindCount);
    cjAssert(boolBindCount	== m_boolBindCount);
    cjAssert(floatBindCount	== m_floatBindCount);
    cjAssert(vec3SBindCount	== m_vec3SBindCount);
	cjAssert(vec3PBindCount	== m_vec3PBindCount);
    cjAssert(rotBindCount	== m_rotBindCount);
    cjAssert(refBindCount	== m_refDatBindCount);
    cjAssert(bindInfoCount	== m_bindInfoCount);

	cjMemDeAlloc(needRebuildBindAry);
}

cjAniPoseBufHandle cjAniPoseBind::_AddBind(cjAniPoseBindInfo* bindInfo, cjAniPoseBufChanItem pbChanItem)
{
    cjAssert(bindInfo);
    cjAssert(pbChanItem != cjAniPoseBufChanItem_Invalid);
    cjAssert(!bindInfo->GetPBHandle(pbChanItem).IsValid());

    cjAniPoseBufHandle pbHandle;

    cjWORD newColorBindCount	= 0;
    cjWORD newBoolBindCount		= 0;
    cjWORD newFloatBindCount	= 0;
    cjWORD newVec3SBindCount	= 0;
	cjWORD newVec3PBindCount	= 0;
    cjWORD newRotBindCount		= 0;
    cjWORD newRefDatBindCount		= 0;

    switch(pbChanItem)
    {
    case cjAniPoseBufChanItem_Color:	newColorBindCount		= 1; break;
    case cjAniPoseBufChanItem_Bool:		newBoolBindCount		= 1; break;
    case cjAniPoseBufChanItem_Float:	newFloatBindCount		= 1; break;
    case cjAniPoseBufChanItem_Vec3S:	newVec3SBindCount		= 1; break;
	case cjAniPoseBufChanItem_Vec3P:	newVec3PBindCount		= 1; break;
    case cjAniPoseBufChanItem_Rot:		newRotBindCount			= 1; break;
    case cjAniPoseBufChanItem_Ref:		newRefDatBindCount			= 1; break;
    default:
		cjAssert(cjFalse);		
		return pbHandle;
    }

    // expand bind array
    cjAssert(newColorBindCount + newBoolBindCount + newFloatBindCount + newVec3SBindCount 
		+ newVec3PBindCount + newRotBindCount + newRefDatBindCount == 1);

    ReallocBinds(
		m_colorBindCount		+ newColorBindCount,
        m_boolBindCount			+ newBoolBindCount,
        m_floatBindCount		+ newFloatBindCount,
        m_vec3SBindCount		+ newVec3SBindCount,
		m_vec3PBindCount		+ newVec3PBindCount,
        m_rotBindCount			+ newRotBindCount,
        m_refDatBindCount		+ newRefDatBindCount,
        m_bindInfoCount);

	// init new bind
    cjWORD colorBindCount		= m_colorBindCount	- newColorBindCount;
    cjWORD boolBindCount		= m_boolBindCount	- newBoolBindCount;
    cjWORD floatBindCount		= m_floatBindCount	- newFloatBindCount;
    cjWORD vec3SBindCount		= m_vec3SBindCount	- newVec3SBindCount;
	cjWORD vec3PBindCount		= m_vec3PBindCount	- newVec3PBindCount;
    cjWORD rotBindCount			= m_rotBindCount	- newRotBindCount;
    cjWORD refBindCount			= m_refDatBindCount	- newRefDatBindCount;

    cjWORD* curBindInfoIndex;

    switch(pbChanItem)
    {
    case cjAniPoseBufChanItem_Color:
		{
			curBindInfoIndex = m_bindInfoIndexList + m_colorStIndex + colorBindCount;
			pbHandle.Init(cjAniPoseBufChanItem_Color, colorBindCount);
			colorBindCount++;
		}
        break;

    case cjAniPoseBufChanItem_Bool:
		{
			curBindInfoIndex = m_bindInfoIndexList + m_boolStIndex + boolBindCount;
			pbHandle.Init(cjAniPoseBufChanItem_Bool, boolBindCount);
			boolBindCount++;
		}
        break;

    case cjAniPoseBufChanItem_Float:
		{
			curBindInfoIndex = m_bindInfoIndexList + m_floatStIndex + floatBindCount;
			pbHandle.Init(cjAniPoseBufChanItem_Float, floatBindCount);
			floatBindCount++;
		}
        break;

    case cjAniPoseBufChanItem_Vec3S:
		{
			curBindInfoIndex = m_bindInfoIndexList + m_vec3SStIndex + vec3SBindCount;
			pbHandle.Init(cjAniPoseBufChanItem_Vec3S, vec3SBindCount);
			vec3SBindCount++;
		}
        break;

    case cjAniPoseBufChanItem_Vec3P:
		{
			curBindInfoIndex = m_bindInfoIndexList + m_vec3PStIndex + vec3PBindCount;
			pbHandle.Init(cjAniPoseBufChanItem_Vec3P, vec3PBindCount);
			vec3PBindCount++;
		}
        break;

    case cjAniPoseBufChanItem_Rot:
		{
			curBindInfoIndex = m_bindInfoIndexList + m_rotStIndex + rotBindCount;
			pbHandle.Init(cjAniPoseBufChanItem_Rot, rotBindCount);
			rotBindCount++;
		}
        break;

    case cjAniPoseBufChanItem_Ref:
		{
			curBindInfoIndex = m_bindInfoIndexList + m_refDatStIndex + refBindCount;
			pbHandle.Init(cjAniPoseBufChanItem_Ref, refBindCount);
			refBindCount++;
		}
        break;

    default:
		{
			cjAssert(cjFalse);
			curBindInfoIndex = cjNULL;
		}
		break;
    }

    *curBindInfoIndex = (cjWORD)(bindInfo - m_bindInfoAry);
    bindInfo->SetPBHandleInfo(pbHandle);

    cjAssert(colorBindCount		== m_colorBindCount);
    cjAssert(boolBindCount		== m_boolBindCount);
    cjAssert(floatBindCount		== m_floatBindCount);
    cjAssert(vec3SBindCount		== m_vec3SBindCount);
	cjAssert(vec3PBindCount		== m_vec3PBindCount);
    cjAssert(rotBindCount		== m_rotBindCount);
    cjAssert(refBindCount		== m_refDatBindCount);

    return pbHandle;
}

const cjAniPoseBindInfo* cjAniPoseBind::GetBindInfo(const cjAniPoseBufHandle& pbHandle) const
{
    if(!pbHandle.IsValid())
        return cjNULL;

    cjAniPoseBufChanItem pbChanItem = pbHandle.GetChannelItem();

    cjWORD auxIndex = pbHandle.GetAuxIndex();
    cjBool isValid = cjFalse;

    switch(pbChanItem)
    {
    case cjAniPoseBufChanItem_Color:
		{
			isValid = (auxIndex < m_colorBindCount);
			auxIndex = (cjWORD)(auxIndex + m_colorStIndex);
		}
        break;

    case cjAniPoseBufChanItem_Bool:
		{
			isValid = (auxIndex < m_boolBindCount);
			auxIndex = (cjWORD)(auxIndex + m_boolStIndex);
		}
        break;

    case cjAniPoseBufChanItem_Float:
		{
			isValid = (auxIndex < m_floatBindCount);
			auxIndex = (cjWORD)(auxIndex + m_floatStIndex);
		}
        break;

    case cjAniPoseBufChanItem_Vec3S:
		{
			isValid = (auxIndex < m_vec3SBindCount);
			auxIndex = (cjWORD)(auxIndex + m_vec3SStIndex);
		}
        break;

    case cjAniPoseBufChanItem_Vec3P:
		{
			isValid = (auxIndex < m_vec3PBindCount);
			auxIndex = (cjWORD)(auxIndex + m_vec3PStIndex);
		}
        break;

    case cjAniPoseBufChanItem_Rot:
		{
			isValid = (auxIndex < m_rotBindCount);
			auxIndex = (cjWORD)(auxIndex + m_rotStIndex);
		}
        break;

    case cjAniPoseBufChanItem_Ref:
		{
			isValid = (auxIndex < m_refDatBindCount);
			auxIndex = (cjWORD)(auxIndex + m_refDatStIndex);
		}
        break;

    default:
		{
			cjAssert(cjFalse);
		}
		break;
    }

    if(isValid)
    {
        cjAssert(m_bindInfoIndexList);
        cjAssert(auxIndex < m_totalBindCount);

        cjWORD bindInfoIndex = m_bindInfoIndexList[auxIndex];

        cjAssert(m_bindInfoAry);
        cjAssert(bindInfoIndex < m_bindInfoCount);

        return m_bindInfoAry + bindInfoIndex;
    }

    return cjNULL;
}

cjAniPoseBindInfo* cjAniPoseBind::GetBindInfo(const cjAniEvaluatorTag& evalTag) const
{
    if(m_bindInfoAry)
    {
        cjWORD hash; 
		
		if(m_hashBucketSiz == cjAniEvalHashBucketSizSmall)
        {
            hash = evalTag.GetHashSmall();
        }
        else
        {
            cjAssert(m_hashBucketSiz == cjAniEvalHashBucketSizLarge);
            hash = evalTag.GetHashLarge();
        }

        cjAssert(m_hash);
        hash = m_hash[hash];

        while(hash != cjAniPoseInvalidHashIndex)
        {
            cjAssert(hash < m_bindInfoCount);

            cjAniPoseBindInfo* bindInfo = m_bindInfoAry + hash;

            if(bindInfo->MatchesIDTag(evalTag))
                return bindInfo;

			hash = bindInfo->m_nextHash;
        }
    }

    return cjNULL;
}

cjAniPoseBindInfo* cjAniPoseBind::GetBindInfo(const cjName& id, const cjName& propRttiName, 
    const cjName& cntrRttiName, const cjName& cntrID, const cjName& evaluatorID) const
{
    if(m_bindInfoAry)
    {
        cjAssert(m_hashBucketSiz > 0 && m_hash);

        cjWORD hash = cjAniEvaluatorTag::GetHash(*id, *propRttiName, *cntrRttiName, *cntrID, *evaluatorID) % m_hashBucketSiz;
        hash = m_hash[hash];

        while(hash != cjAniPoseInvalidHashIndex)
        {
            cjAssert(hash < m_bindInfoCount);

            cjAniPoseBindInfo* bindInfo = m_bindInfoAry + hash;

            if(bindInfo->MatchesIDTag(id, propRttiName, cntrRttiName, cntrID, evaluatorID))
                return bindInfo;

			hash = bindInfo->m_nextHash;
        }
    }

    return cjNULL;
}

cjBool cjAniPoseBind::GetHandleAndLOD(const cjName& id, const cjName& propName, const cjName& cntrRttiName, 
	const cjName& cntrID, const cjName& evaluatorID, cjAniPoseBufChanItem pbChanType, 
	cjAniPoseBufHandle& rstPBHandle, cjSWORD& rstLOD) const
{
    const cjAniPoseBindInfo* bindInfo = GetBindInfo(id, propName, cntrRttiName, cntrID, evaluatorID);

    if(bindInfo)
    {
        rstPBHandle = bindInfo->GetPBHandle(pbChanType);

		if(rstPBHandle.IsValid())
        {
            rstLOD = bindInfo->m_lod;
            return cjTrue;
        }
    }

    rstPBHandle.Invalidate();

    return cjFalse;
}

cjBool cjAniPoseBind::GetHandleAndLOD(const cjAniEvaluatorTag& tag, cjAniPoseBufChanItem pbChanType, 
	cjAniPoseBufHandle& rstPBHandle, cjSWORD& rstLOD) const
{
    const cjAniPoseBindInfo* bindInfo = GetBindInfo(tag);

    if(bindInfo)
    {
        rstPBHandle = bindInfo->GetPBHandle(pbChanType);

        if(rstPBHandle.IsValid())
        {
            rstLOD = bindInfo->m_lod;
            return cjTrue;
        }
    }

	rstPBHandle.Invalidate();

    return cjFalse;
}

void cjAniPoseBind::Init(cjAniCntrManager* cntrMan, const cjRefObj* accumRoot)
{
    cjAssert(cntrMan);
    cjAssert(cntrMan->GetTarget());

    cjAssert(m_nonAccumStIndex == 0);

    if(accumRoot)
    {
		// check accumRoot
        cjAssert(accumRoot == cjAniCntrManager::FindAccumRoot((cjSceneNode*)cntrMan->GetTarget()));

        m_nonAccumStIndex	= ACCUMENDINDEX;
        m_accumRootName		= accumRoot->GetName();

        cjAssert(m_accumRootName.IsValid());

        InitAccumRootBind(cntrMan, accumRoot);
    }
}

void cjAniPoseBind::Shutdown()
{
	m_totalBindCount= 0;

	if(m_bindInfoIndexList)
	{
		cjMemDeAlloc(m_bindInfoIndexList);
		m_bindInfoIndexList = cjNULL;
	}

	if(m_bindInfoAry)
	{
		delete [] m_bindInfoAry;
		m_bindInfoAry = cjNULL;
	}

	m_bindInfoCount = 0;

	if(m_hash)
	{
		cjMemDeAlloc(m_hash);
		m_hash = cjNULL;
	}

    m_hashBucketSiz = 0; 

    m_colorBindCount = 0;
    m_colorStIndex = 0;

    m_boolBindCount = 0;
    m_boolStIndex = 0;

    m_floatBindCount = 0;
    m_floatStIndex = 0;

    m_vec3SBindCount = 0;
    m_vec3SStIndex = 0;

    m_vec3PBindCount = 0;
    m_vec3PStIndex = 0;

	m_rotBindCount = 0;
    m_rotStIndex = 0;

    m_refDatBindCount = 0;
    m_refDatStIndex = 0;
}

void cjAniPoseBind::InitAccumRootBind(cjAniCntrManager* cntrMan, const cjRefObj* accumRoot)
{
    cjAssert(cntrMan);
    cjAssert(accumRoot);

    cjAssert(GetCumulativeAni());
	cjAssert(m_accumRootName.IsValid());

    ReallocHashTable(cjAniEvalHashBucketSizSmall);

    cjWORD newVec3SBindCount	= ACCUMENDINDEX;  // Scale (ACCUMENDINDEX-2:accum value, ACCUMENDINDEX-1:accum delta
    cjWORD newVec3PBindCount	= ACCUMENDINDEX;  // Pos
    cjWORD newRotBindCount		= ACCUMENDINDEX;  // Rot
    cjWORD newBindInfoCount		= 1;

	cjAniPoseBindInfo* bindInfo = GetBindInfo(m_accumRootName, cjNULL, cjAniConstants::GetTransformCntrID(), cjNULL, cjNULL);

	if(bindInfo)
    {
        newBindInfoCount = 0;

		// scale
        cjAniPoseBufHandle pbHandle = bindInfo->GetPBHandle(cjAniPoseBufChanItem_Vec3S);
        if(pbHandle.IsValid())
            newVec3SBindCount = 0;

		// pos
		pbHandle = bindInfo->GetPBHandle(cjAniPoseBufChanItem_Vec3P);
        if(pbHandle.IsValid())
            newVec3PBindCount = 0;

		// rot
		pbHandle = bindInfo->GetPBHandle(cjAniPoseBufChanItem_Rot);
        if(pbHandle.IsValid())
            newRotBindCount = 0;
    }

    if((newVec3SBindCount + newVec3PBindCount + newRotBindCount) > 0)
    {
        ReallocBinds(
			m_colorBindCount, 
			m_boolBindCount, 
			m_floatBindCount,
			m_vec3SBindCount		+ newVec3SBindCount,
            m_vec3PBindCount		+ newVec3PBindCount,
            m_rotBindCount			+ newRotBindCount,
            m_refDatBindCount, 
            m_bindInfoCount			+ newBindInfoCount);

        if(!bindInfo)
        {
            cjAssert(newBindInfoCount == 1);

            bindInfo					= m_bindInfoAry + m_bindInfoCount - 1;
            bindInfo->m_id				= m_accumRootName;
            bindInfo->m_propRttiName	= cjNULL;
            bindInfo->m_cntrRttiName	= cjAniConstants::GetTransformCntrID();
            bindInfo->m_cntrID			= cjNULL;
            bindInfo->m_evaluatorID		= cjNULL;

            bindInfo->m_lod = (cjSWORD)(m_lodCount - 1);
            cjAssert(bindInfo->m_lod >= 0);

            cjAssert(m_hashBucketSiz > 0 && m_hash);

            cjWORD hash = cjAniEvaluatorTag::GetHash(*m_accumRootName, cjNULL, *cjAniConstants::GetTransformCntrID(), cjNULL, cjNULL) % m_hashBucketSiz;
            bindInfo->m_nextHash = m_hash[hash];
            m_hash[hash] = m_bindInfoCount - 1;

            cjAssert(!bindInfo->m_pbHandle.IsValid());
            cjAssert(bindInfo->m_pbIndicesForTM[0] == cjAniPoseInvalidChanIndex); // pos
            cjAssert(bindInfo->m_pbIndicesForTM[1] == cjAniPoseInvalidChanIndex); // scale
        }

        cjAssert(bindInfo);
        cjWORD bindInfoIndex = (cjWORD)(bindInfo - m_bindInfoAry);

		// scale, vec3, aux=0
        if(newVec3SBindCount > 0)
        {
            for(cjUINT32 m = m_vec3SBindCount + m_vec3SStIndex - 1;
                m >= m_vec3SStIndex + (cjUINT32)ACCUMENDINDEX;
				m--)
            {
                m_bindInfoIndexList[m] = m_bindInfoIndexList[m - ACCUMENDINDEX];
            }

            m_bindInfoIndexList[m_vec3SStIndex]		= bindInfoIndex;	// scale
            m_bindInfoIndexList[m_vec3SStIndex + 1]	= bindInfoIndex;	// scale delta

            bindInfo->SetPBHandleInfo(cjAniPoseBufHandle(cjAniPoseBufChanItem_Vec3S, 0));
        }

		// pos, vec3, aux=0
        if(newVec3PBindCount > 0)
        {
            for(cjUINT32 m = m_vec3PBindCount + m_vec3PStIndex - 1;
				m >= m_vec3PStIndex + (cjUINT32)ACCUMENDINDEX;
				m--)
            {
                m_bindInfoIndexList[m] = m_bindInfoIndexList[m - ACCUMENDINDEX];
            }

            m_bindInfoIndexList[m_vec3PStIndex]		= bindInfoIndex;	// pos
            m_bindInfoIndexList[m_vec3PStIndex + 1] = bindInfoIndex;	// not pos delta

            bindInfo->SetPBHandleInfo(cjAniPoseBufHandle(cjAniPoseBufChanItem_Vec3P, 0));
        }

		// rotation, rot, aux=0
        if(newRotBindCount > 0)
        {
            for(cjUINT32 m = m_rotBindCount + m_rotStIndex - 1;
                m >= m_rotStIndex + (cjUINT32)ACCUMENDINDEX; 
				m--)
            {
                m_bindInfoIndexList[m] = m_bindInfoIndexList[m - ACCUMENDINDEX];
            }

            m_bindInfoIndexList[m_rotStIndex]		= bindInfoIndex;	// rotation
            m_bindInfoIndexList[m_rotStIndex + 1]	= bindInfoIndex;	// rotation delta

            bindInfo->SetPBHandleInfo(cjAniPoseBufHandle(cjAniPoseBufChanItem_Rot, 0));
        }
    }
}

void cjAniPoseBind::ReallocBinds(cjUINT32 colorCount, cjUINT32 boolCount, cjUINT32 floatCount, 
    cjUINT32 vec3SCount, cjUINT32 vec3PCount, cjUINT32 rotCount, cjUINT32 refDatCount, cjUINT32 bindInfoCount)
{
    cjAssert(colorCount >= m_colorBindCount);
    cjAssert(boolCount >= m_boolBindCount);
    cjAssert(floatCount >= m_floatBindCount);
    cjAssert(vec3SCount >= m_vec3SBindCount);
	cjAssert(vec3PCount >= m_vec3PBindCount);
    cjAssert(rotCount >= m_rotBindCount);
    cjAssert(refDatCount >= m_refDatBindCount);
    cjAssert(bindInfoCount >= m_bindInfoCount);

    cjWORD totalBindCount = (cjWORD)(colorCount + boolCount + floatCount + vec3SCount + vec3PCount + rotCount + refDatCount);

    cjAssert(totalBindCount > m_totalBindCount);
    cjWORD* newIndexList = (cjWORD*)cjMemAlloc(sizeof(cjWORD)*totalBindCount);
    cjWORD* newIndex = newIndexList;

    for(cjUINT32 i=0; i<m_totalBindCount; i++)
    {
		// color

        if(i == m_boolStIndex)
            newIndex += (colorCount - m_colorBindCount);

        if(i == m_floatStIndex)
            newIndex += (boolCount - m_boolBindCount);

		if(i == m_vec3SStIndex)
            newIndex += (floatCount - m_floatBindCount);

		if(i == m_vec3PStIndex)
            newIndex += (vec3SCount - m_vec3SBindCount);

        if(i == m_rotStIndex)
            newIndex += (vec3PCount - m_vec3PBindCount);

		if(i == m_refDatStIndex)
            newIndex += (rotCount - m_rotBindCount);

		*newIndex = m_bindInfoIndexList[i];
        newIndex++;
    }

	if(m_bindInfoIndexList)
	{
		cjMemDeAlloc(m_bindInfoIndexList);
		m_bindInfoIndexList = cjNULL;
	}
	

	m_bindInfoIndexList		= newIndexList;
    m_totalBindCount		= totalBindCount;

	// new bind count
    m_colorBindCount		= (cjWORD)colorCount;
    m_boolBindCount			= (cjWORD)boolCount;
    m_floatBindCount		= (cjWORD)floatCount;

    m_vec3SBindCount		= (cjWORD)vec3SCount;
	m_vec3PBindCount		= (cjWORD)vec3PCount;
    m_rotBindCount			= (cjWORD)rotCount;

    m_refDatBindCount		= (cjWORD)refDatCount;
    
	// new index
	cjAssert(m_colorStIndex == 0);

    m_boolStIndex			= m_colorStIndex	+ m_colorBindCount;
    m_floatStIndex			= m_boolStIndex		+ m_boolBindCount;
    m_vec3SStIndex			= m_floatStIndex	+ m_floatBindCount;
    m_vec3PStIndex			= m_vec3SStIndex	+ m_vec3SBindCount;
	m_rotStIndex			= m_vec3PStIndex	+ m_vec3PBindCount;
	m_refDatStIndex			= m_rotStIndex		+ m_rotBindCount;

    cjAssert(m_totalBindCount == m_refDatStIndex + m_refDatBindCount);

    if(bindInfoCount > m_bindInfoCount)
    {
        cjAniPoseBindInfo* newBindInfoList = new cjAniPoseBindInfo[bindInfoCount];
        cjAniPoseBindInfo* newBindInfo = newBindInfoList;

        for(cjUINT32 i=0; i<m_bindInfoCount; i++)
        {
            *newBindInfo = m_bindInfoAry[i];
            newBindInfo++;
        }

		if(m_bindInfoAry)
		{
			delete [] m_bindInfoAry;
			m_bindInfoAry = cjNULL;
		}

        m_bindInfoAry = newBindInfoList;
        m_bindInfoCount = (cjWORD)bindInfoCount;
    }
}

void cjAniPoseBind::ReallocHashTable(cjUINT32 bucketSiz)
{
    if(bucketSiz <= m_hashBucketSiz)
        return;

    cjPrimeNumberTable::GetNearestPrime(bucketSiz, bucketSiz);

    if(m_hash)
    {
		cjMemDeAlloc(m_hash);
        m_hash = cjNULL;
    }

    m_hash = (cjWORD*)cjMemAlloc(sizeof(cjWORD) * bucketSiz);
    m_hashBucketSiz = (cjWORD)bucketSiz;

    for(cjUINT32 i=0; i<bucketSiz; i++)
        m_hash[i] = cjAniPoseInvalidHashIndex;

    cjAssert(m_hashBucketSiz > 0 && m_hash);

    for(cjUINT32 i=0; i<m_bindInfoCount; i++)
    {
        cjAniPoseBindInfo& bindInfo = m_bindInfoAry[i];

        cjWORD hash = cjAniEvaluatorTag::GetHash(*bindInfo.m_id, *bindInfo.m_propRttiName, 
			*bindInfo.m_cntrRttiName, *bindInfo.m_cntrID, *bindInfo.m_evaluatorID) % m_hashBucketSiz;

        bindInfo.m_nextHash = m_hash[hash];
        m_hash[hash] = (cjWORD)i;
    }
}


//
// cjAniPoseBuffer
//

CJSER_IMPL_CLASS(cjAniPoseBuf, cjRefObj)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjAniPoseBuf)

	d->m_poseBind           = m_poseBind;
							   
	d->m_totalDatCount		= m_totalDatCount;
	d->m_validDatCount		= m_validDatCount;
	d->m_nonAccumStIndex	= m_nonAccumStIndex;
						   
	d->m_colorCount			= m_colorCount;
	d->m_boolCount			= m_boolCount;
	d->m_floatCount			= m_floatCount;
	d->m_vec3SCount			= m_vec3SCount;
	d->m_vec3PCount			= m_vec3PCount;
	d->m_rotCount			= m_rotCount;
	d->m_refDatCount		= m_refDatCount;

	cjAssert(m_colorStIndex == 0);
	d->m_colorStIndex		= m_colorStIndex;
	d->m_boolStIndex		= m_boolStIndex + m_colorCount;
	d->m_floatStIndex		= m_floatStIndex + m_boolCount;
	d->m_vec3SStIndex		= m_vec3SStIndex + m_floatCount;
	d->m_vec3PStIndex		= m_vec3PStIndex + m_vec3SCount;
	d->m_rotStIndex			= m_rotStIndex + m_vec3PCount;
	d->m_refDatStIndex		= m_refDatStIndex + m_rotCount;

    cjAssert(d->m_totalDatCount == d->m_refDatStIndex + d->m_refDatCount);

	if(m_totalDatCount > 0)
	{
		cjUINT32 newSiz = 
			m_colorCount	* sizeof(ColorDat) + 
			m_boolCount		* sizeof(BoolDat) + 
            m_floatCount	* sizeof(FloatDat) + 
			m_vec3SCount	* sizeof(Vec3SDat) + 
            m_vec3PCount	* sizeof(Vec3PDat) + 
            m_rotCount		* sizeof(RotDat) +
            m_refDatCount	* sizeof(RefDat);

        cjAssert(newSiz % 4 == 0);
        newSiz /= 4;
		d->m_dats = (cjFLOAT*)cjMemAlloc(sizeof(cjFLOAT)*newSiz);

		for(cjUINT32 i=0; i<newSiz; i++)
			d->m_dats[i] = m_dats[i];
							   
		// reposition
		d->m_colors		= (cjAniPoseBuf::ColorDat*)(d->m_dats);
		d->m_bools		= (cjAniPoseBuf::BoolDat*)(&d->m_colors[m_colorCount]);
		d->m_floats		= (cjAniPoseBuf::FloatDat*)(&d->m_bools[m_boolCount]);
		d->m_vec3Ss		= (cjAniPoseBuf::Vec3SDat*)(&d->m_floats[m_floatCount]);
		d->m_vec3Ps		= (cjAniPoseBuf::Vec3PDat*)(&d->m_vec3Ss[m_vec3SCount]);
		d->m_rots		= (cjAniPoseBuf::RotDat*)(&d->m_vec3Ps[m_vec3PCount]);
		d->m_refDats	= (cjAniPoseBuf::RefDat*)(&d->m_rots[m_rotCount]);
		
		cjAssert(&d->m_dats[newSiz] == (cjFLOAT*)(&d->m_refDats[m_refDatCount]));

		d->m_weightAry = (cjUINT32*)cjMemAlloc(sizeof(cjUINT32)*m_totalDatCount);
		
		cjMemCpy(d->m_weightAry, m_weightAry, sizeof(cjUINT32)*m_totalDatCount);
	}
CJSER_IMPL_CLONE_PROPCOPY_END(cjAniPoseBuf)

CJSER_IMPL_CLONE(cjAniPoseBuf)

CJSER_IMPL_CLONE_POST_BEGIN(cjAniPoseBuf)
CJSER_IMPL_CLONE_POST_END(cjAniPoseBuf)


void cjAniPoseBuf::_InitCommon()
{
	m_totalDatCount			= 0;
	m_validDatCount			= 0;
	m_nonAccumStIndex		= 0;

	// m_poseBind = cjNULL;
	m_weightAry				= cjNULL;
	m_dats					= cjNULL;

	m_colorCount			= 0;
	m_colorStIndex			= 0;
	m_colors				= cjNULL;

	m_boolCount				= 0;
	m_boolStIndex			= 0;
	m_bools					= cjNULL;

	m_floatCount			= 0;
	m_floatStIndex			= 0;
	m_floats				= cjNULL;

	m_vec3SCount			= 0;
	m_vec3SStIndex			= 0;
	m_vec3Ss				= cjNULL;

	m_vec3PCount			= 0;
	m_vec3PStIndex			= 0;
	m_vec3Ps				= cjNULL;

	m_rotCount				= 0;
	m_rotStIndex			= 0;
	m_rots					= cjNULL;

	m_refDatCount			= 0;
	m_refDatStIndex			= 0;
	m_refDats				= cjNULL;
}

cjAniPoseBuf::cjAniPoseBuf() 
{
	_InitCommon();
}

cjAniPoseBuf::cjAniPoseBuf(cjAniPoseBind* poseBind) 
{
	_InitCommon();
	Init(poseBind);
}

cjAniPoseBuf::~cjAniPoseBuf()
{
	Shutdown();
}


//
// color
//
cjBool cjAniPoseBuf::GetColor(cjAniPoseBufHandle pbHandle, cjColorF& v, cjBool notCulledOnly) const
{
	cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Color);
	cjWORD auxIndex = pbHandle.GetAuxIndex();

    if(auxIndex < m_colorCount) 
	{
		if(notCulledOnly) 
		{
			if(!IsValidWeightAndNotCulled(m_weightAry[m_colorStIndex + auxIndex]))
				return cjFalse;
		}
		else
		{
			if(!IsValidWeight(m_weightAry[m_colorStIndex + auxIndex])) 
				return cjFalse;
		}
	}
			
	v = m_colors[auxIndex].m_color;

	return cjTrue;
}

cjBool cjAniPoseBuf::IsColorValid(cjAniPoseBufHandle pbHandle) const
{
	cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Color);
    cjWORD auxIndex = pbHandle.GetAuxIndex();

    return (auxIndex < m_colorCount && IsValidWeight(m_weightAry[m_colorStIndex + auxIndex]));
}

 void cjAniPoseBuf::SetColor(cjAniPoseBufHandle pbHandle, const cjColorF& v)
{
    cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Color);
    
	cjWORD auxIndex = pbHandle.GetAuxIndex();
    cjAssert(auxIndex < m_colorCount);

    m_colors[auxIndex].m_color = v;
}

 void cjAniPoseBuf::SetColorValid(cjAniPoseBufHandle pbHandle, cjBool valid)
{
    cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Color);

    cjWORD auxIndex = pbHandle.GetAuxIndex();
    cjAssert(auxIndex < m_colorCount);

    cjUINT32& weightDat = m_weightAry[m_colorStIndex + auxIndex];
    
	if(IsValidWeight(weightDat))
    {
        cjAssert(m_validDatCount > 0);
        m_validDatCount--;
    }

    if(valid)
    {
        cjAssert(m_validDatCount < m_totalDatCount);
        m_validDatCount++;
        weightDat = (cjUINT32)WDAT_FLAG_VALID;
    }
    else
    {
        weightDat = 0;
    }
}


//
// Bool
//
cjBool cjAniPoseBuf::GetBool(cjAniPoseBufHandle pbHandle, cjFLOAT& v, cjBool notCulledOnly) const
{
    cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Bool);
    cjWORD auxIndex = pbHandle.GetAuxIndex();

    if(auxIndex < m_boolCount)
    {
		if(notCulledOnly) 
		{
			if(!IsValidWeightAndNotCulled(m_weightAry[m_boolStIndex + auxIndex]))
				return cjFalse;
		}
		else
		{
			if(!IsValidWeight(m_weightAry[m_boolStIndex + auxIndex])) 
				return cjFalse;
		}
    }

	v = m_bools[auxIndex].m_bool;

    return cjTrue;
}

cjBool cjAniPoseBuf::IsBoolValid(cjAniPoseBufHandle pbHandle) const
{
    cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Bool);
    cjWORD auxIndex = pbHandle.GetAuxIndex();
    return (auxIndex < m_boolCount && IsValidWeight(m_weightAry[m_boolStIndex + auxIndex]));
}

void cjAniPoseBuf::SetBool(cjAniPoseBufHandle pbHandle, cjFLOAT v)
{
    cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Bool);
    cjWORD auxIndex = pbHandle.GetAuxIndex();
    cjAssert(auxIndex < m_boolCount);
    m_bools[auxIndex].m_bool = v;
}

void cjAniPoseBuf::SetBoolValid(cjAniPoseBufHandle pbHandle, cjBool valid)
{
    cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Bool);

    cjWORD auxIndex = pbHandle.GetAuxIndex();
    cjAssert(auxIndex < m_boolCount);

    cjUINT32& weight = m_weightAry[m_boolStIndex + auxIndex];

	if(IsValidWeight(weight))
    {
        cjAssert(m_validDatCount > 0);
        m_validDatCount--;
    }

    if(valid)
    {
        cjAssert(m_validDatCount < m_totalDatCount);
        m_validDatCount++;
        weight = (cjUINT32 )WDAT_FLAG_VALID;
    }
    else
        weight = 0;
}


//
// Float
//

cjBool cjAniPoseBuf::GetFloat(cjAniPoseBufHandle pbHandle, cjFLOAT& v, cjBool notCulledOnly) const
{
    cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Float);

    cjWORD auxIndex = pbHandle.GetAuxIndex();
    if(auxIndex < m_floatCount)		
    {
		if(notCulledOnly) 
		{
			if(!IsValidWeightAndNotCulled(m_weightAry[m_floatStIndex + auxIndex]))
				return cjFalse;
		}
		else
		{
			if(!IsValidWeight(m_weightAry[m_floatStIndex + auxIndex])) 
				return cjFalse;
		}
	}

	v = m_floats[auxIndex].m_float;

    return cjTrue;
}

cjBool cjAniPoseBuf::IsFloatValid(cjAniPoseBufHandle pbHandle) const
{
    cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Float);
    cjWORD auxIndex = pbHandle.GetAuxIndex();

    return (auxIndex < m_floatCount && IsValidWeight(m_weightAry[m_floatStIndex + auxIndex]));
}

void cjAniPoseBuf::SetFloat(cjAniPoseBufHandle pbHandle, cjFLOAT v)
{
    cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Float);

    cjWORD auxIndex = pbHandle.GetAuxIndex();
    cjAssert(auxIndex < m_floatCount);

    m_floats[auxIndex].m_float = v;
}

void cjAniPoseBuf::SetFloatValid(cjAniPoseBufHandle pbHandle, cjBool valid)
{
    cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Float);

    cjWORD auxIndex = pbHandle.GetAuxIndex();
    cjAssert(auxIndex < m_floatCount);

    cjUINT32& weight = m_weightAry[m_floatStIndex + auxIndex];

    if(IsValidWeight(weight))
    {
        cjAssert(m_validDatCount > 0);
        m_validDatCount--;
    }

    if(valid)
    {
        cjAssert(m_validDatCount < m_totalDatCount);
        m_validDatCount++;
        weight = (cjUINT32 )WDAT_FLAG_VALID;
    }
    else
        weight = 0;
}


//
// cjVec3S
//

cjBool cjAniPoseBuf::GetVec3S(cjAniPoseBufHandle pbHandle, cjVec3& v, cjBool notCulledOnly) const
{
    cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Vec3S);
    cjWORD auxIndex = pbHandle.GetAuxIndex();

    if(auxIndex < m_vec3SCount) 
	{
		if(notCulledOnly) 
		{
			if(!IsValidWeightAndNotCulled(m_weightAry[m_vec3SStIndex + auxIndex]))
				return cjFalse;
		}
		else
		{
			if(!IsValidWeight(m_weightAry[m_vec3SStIndex + auxIndex])) 
				return cjFalse;
		}
	}
			
	v = m_vec3Ss[auxIndex].m_vec3;

	return cjTrue;
}

cjBool cjAniPoseBuf::IsVec3SValid(cjAniPoseBufHandle pbHandle) const
{
    cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Vec3S);
    cjWORD auxIndex = pbHandle.GetAuxIndex();

    return (auxIndex < m_vec3SCount && IsValidWeight(m_weightAry[m_vec3SStIndex + auxIndex]));
}

void cjAniPoseBuf::SetVec3S(cjAniPoseBufHandle pbHandle, const cjVec3& v)
{
    cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Vec3S);

    cjWORD auxIndex = pbHandle.GetAuxIndex();
    cjAssert(auxIndex < m_vec3SCount);

    m_vec3Ss[auxIndex].m_vec3 = v;
}

 void cjAniPoseBuf::SetVec3SValid(cjAniPoseBufHandle pbHandle, cjBool valid)
{
    cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Vec3S);

    cjWORD auxIndex = pbHandle.GetAuxIndex();
    cjAssert(auxIndex < m_vec3SCount);

    cjUINT32 & weight = m_weightAry[m_vec3SStIndex + auxIndex];
	
	if(IsValidWeight(weight))
    {
        cjAssert(m_validDatCount > 0);
        m_validDatCount--;
    }

    if(valid)
    {
        cjAssert(m_validDatCount < m_totalDatCount);
        m_validDatCount++;
        weight = (cjUINT32 )WDAT_FLAG_VALID;
    }
    else
        weight = 0;
}


//
// cjVec3P
//

cjBool cjAniPoseBuf::GetVec3P(cjAniPoseBufHandle pbHandle, cjVec3& v, cjBool notCulledOnly) const
{
    cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Vec3P);
    cjWORD auxIndex = pbHandle.GetAuxIndex();

    if(auxIndex < m_vec3PCount) 
	{
		if(notCulledOnly) 
		{
			if(!IsValidWeightAndNotCulled(m_weightAry[m_vec3PStIndex + auxIndex]))
				return cjFalse;
		}
		else
		{
			if(!IsValidWeight(m_weightAry[m_vec3PStIndex + auxIndex])) 
				return cjFalse;
		}
	}
			
	v = m_vec3Ps[auxIndex].m_vec3;

	return cjTrue;
}

cjBool cjAniPoseBuf::IsVec3PValid(cjAniPoseBufHandle pbHandle) const
{
    cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Vec3P);
    cjWORD auxIndex = pbHandle.GetAuxIndex();

    return (auxIndex < m_vec3PCount && IsValidWeight(m_weightAry[m_vec3PStIndex + auxIndex]));
}

void cjAniPoseBuf::SetVec3P(cjAniPoseBufHandle pbHandle, const cjVec3& v)
{
    cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Vec3P);

    cjWORD auxIndex = pbHandle.GetAuxIndex();
    cjAssert(auxIndex < m_vec3PCount);

    m_vec3Ps[auxIndex].m_vec3 = v;
}

void cjAniPoseBuf::SetVec3PValid(cjAniPoseBufHandle pbHandle, cjBool valid)
{
    cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Vec3P);

    cjWORD auxIndex = pbHandle.GetAuxIndex();
    cjAssert(auxIndex < m_vec3PCount);

    cjUINT32 & weight = m_weightAry[m_vec3PStIndex + auxIndex];
	
	if(IsValidWeight(weight))
    {
        cjAssert(m_validDatCount > 0);
        m_validDatCount--;
    }

    if(valid)
    {
        cjAssert(m_validDatCount < m_totalDatCount);
        m_validDatCount++;

        weight = (cjUINT32)WDAT_FLAG_VALID;
    }
    else
        weight = 0;
}


//
// Rot - quaternion
//

cjBool cjAniPoseBuf::GetRot(cjAniPoseBufHandle pbHandle, cjQuat& v, cjBool notCulledOnly) const
{
    cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Rot);
    cjWORD auxIndex = pbHandle.GetAuxIndex();

    if(auxIndex < m_rotCount) 
	{
		if(notCulledOnly) 
		{
			if(!IsValidWeightAndNotCulled(m_weightAry[m_rotStIndex + auxIndex]))
				return cjFalse;
		}
		else
		{
			if(!IsValidWeight(m_weightAry[m_rotStIndex + auxIndex])) 
				return cjFalse;
		}
	}

	v = m_rots[auxIndex].m_rotQuat;

	return cjTrue;
}

cjBool cjAniPoseBuf::IsRotValid(cjAniPoseBufHandle pbHandle) const
{
    cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Rot);
    cjWORD auxIndex = pbHandle.GetAuxIndex();

    return (auxIndex < m_rotCount && IsValidWeight(m_weightAry[m_rotStIndex + auxIndex]));
}

 void cjAniPoseBuf::SetRot(cjAniPoseBufHandle pbHandle, const cjQuat& v)
{
    cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Rot);
    cjWORD auxIndex = pbHandle.GetAuxIndex();
    cjAssert(auxIndex < m_rotCount);
    m_rots[auxIndex].m_rotQuat = v;
}

 void cjAniPoseBuf::SetRotValid(cjAniPoseBufHandle pbHandle, cjBool valid)
{
    cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Rot);

    cjWORD auxIndex = pbHandle.GetAuxIndex();
    cjAssert(auxIndex < m_rotCount);

    cjUINT32 & weight = m_weightAry[m_rotStIndex + auxIndex];
    if(IsValidWeight(weight))
    {
        cjAssert(m_validDatCount > 0);
        m_validDatCount--;
    }

    if(valid)
    {
        cjAssert(m_validDatCount < m_totalDatCount);
        m_validDatCount++;
        weight = (cjUINT32 )WDAT_FLAG_VALID;
    }
    else
        weight = 0;
}

//
// ReferencedDat
//

cjBool cjAniPoseBuf::GetRefDat(cjAniPoseBufHandle pbHandle, RefDat& v, cjBool notCulledOnly) const
{
    cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
    cjWORD auxIndex = pbHandle.GetAuxIndex();

    if(auxIndex < m_refDatCount) 
	{
		if(notCulledOnly) 
		{
			if(!IsValidWeightAndNotCulled(m_weightAry[m_refDatStIndex + auxIndex]))
				return cjFalse;
		}
		else
		{
			if(!IsValidWeight(m_weightAry[m_refDatStIndex + auxIndex])) 
				return cjFalse;
		}
	}

	v = m_refDats[auxIndex];

	return cjTrue;
}

cjBool cjAniPoseBuf::IsRefDatValid(cjAniPoseBufHandle pbHandle) const
{
    cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
    cjWORD auxIndex = pbHandle.GetAuxIndex();

    return (auxIndex < m_refDatCount && IsValidWeight(m_weightAry[m_refDatStIndex + auxIndex]));
}

void cjAniPoseBuf::SetRefDat(cjAniPoseBufHandle pbHandle, const RefDat& refDat)
{
    cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);

    cjWORD auxIndex = pbHandle.GetAuxIndex();
    cjAssert(auxIndex < m_refDatCount);

    m_refDats[auxIndex] = refDat;
}

void cjAniPoseBuf::SetRefDatValid(cjAniPoseBufHandle pbHandle, cjBool valid)
{
    cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
    
	cjWORD auxIndex = pbHandle.GetAuxIndex();
    cjAssert(auxIndex < m_refDatCount);

    cjUINT32& weight = m_weightAry[m_refDatStIndex + auxIndex];

    if(IsValidWeight(weight))
    {
        cjAssert(m_validDatCount > 0);
        m_validDatCount--;
    }

    if(valid)
    {
        cjAssert(m_validDatCount < m_totalDatCount);
        m_validDatCount++;

        weight = (cjUINT32)WDAT_FLAG_VALID;
    }
    else
	{
        weight = 0;
	}
}

cjFLOAT* cjAniPoseBuf::GetDat(cjAniPoseBufHandle pbHandle) const
{
    cjFLOAT* dat = cjNULL;
	cjWORD auxIndex = pbHandle.GetAuxIndex();

	cjAniPoseBufChanItem pbChanItem = pbHandle.GetChannelItem();

    switch(pbChanItem)
    {
    case cjAniPoseBufChanItem_Color:
        if(auxIndex < m_colorCount && IsValidWeight(m_weightAry[m_colorStIndex + auxIndex]))
            dat = (cjFLOAT*)(m_colors + auxIndex);        
		break;

    case cjAniPoseBufChanItem_Bool:
        if(auxIndex < m_boolCount && IsValidWeight(m_weightAry[m_boolStIndex + auxIndex]))
            dat = (cjFLOAT*)(m_bools + auxIndex);        
		break;

    case cjAniPoseBufChanItem_Float:
        if(auxIndex < m_floatCount && IsValidWeight(m_weightAry[m_floatStIndex + auxIndex]))
            dat = (cjFLOAT*)(m_floats + auxIndex);        
		break;

    case cjAniPoseBufChanItem_Vec3S:
        if(auxIndex < m_vec3SCount && IsValidWeight(m_weightAry[m_vec3SStIndex + auxIndex]))
            dat = (cjFLOAT*)(m_vec3Ss + auxIndex);
        break;

    case cjAniPoseBufChanItem_Vec3P:
        if(auxIndex < m_vec3PCount && IsValidWeight(m_weightAry[m_vec3PStIndex + auxIndex]))
            dat = (cjFLOAT*)(m_vec3Ps + auxIndex);
        break;

    case cjAniPoseBufChanItem_Rot:
        if(auxIndex < m_rotCount && IsValidWeight(m_weightAry[m_rotStIndex + auxIndex]))
            dat = (cjFLOAT*)(m_rots + auxIndex);
        break;

    case cjAniPoseBufChanItem_Ref:
        if(auxIndex < m_refDatCount && IsValidWeight(m_weightAry[m_refDatStIndex + auxIndex]))
            dat = (cjFLOAT*)(m_refDats + auxIndex);
		break;

    default: cjAssert(cjFalse); 
		break;
    }

	return dat;
}

cjBool cjAniPoseBuf::IsValid(cjAniPoseBufHandle pbHandle) const
{
	cjAniPoseBufChanItem pbChanItem = pbHandle.GetChannelItem();

    switch(pbChanItem)
    {
    case cjAniPoseBufChanItem_Color:	return IsColorValid(pbHandle);
    case cjAniPoseBufChanItem_Bool:		return IsBoolValid(pbHandle);
    case cjAniPoseBufChanItem_Float:	return IsFloatValid(pbHandle);
    case cjAniPoseBufChanItem_Vec3S:	return IsVec3SValid(pbHandle);
	case cjAniPoseBufChanItem_Vec3P:	return IsVec3PValid(pbHandle);
    case cjAniPoseBufChanItem_Rot:      return IsRotValid(pbHandle);
    case cjAniPoseBufChanItem_Ref:      return IsRefDatValid(pbHandle);
    default:
        cjAssert(cjFalse);
		break;
    }

    return cjFalse;
}

void cjAniPoseBuf::SetValid(cjAniPoseBufHandle pbHandle, cjBool isValid)
{
    cjAniPoseBufChanItem pbChanItem = pbHandle.GetChannelItem();

    switch(pbChanItem)
    {
    case cjAniPoseBufChanItem_Color:  SetColorValid(pbHandle, isValid);		break;
    case cjAniPoseBufChanItem_Bool:   SetBoolValid(pbHandle, isValid);		break;
    case cjAniPoseBufChanItem_Float:  SetFloatValid(pbHandle, isValid);		break;
    case cjAniPoseBufChanItem_Vec3S:  SetVec3SValid(pbHandle, isValid);		break;
	case cjAniPoseBufChanItem_Vec3P:  SetVec3PValid(pbHandle, isValid);		break;
    case cjAniPoseBufChanItem_Rot:    SetRotValid(pbHandle, isValid);       break;
    case cjAniPoseBufChanItem_Ref:    SetRefDatValid(pbHandle, isValid);    break;
    default: cjAssert(cjFalse); break;
    }
}

cjBool cjAniPoseBuf::GetHandle(const cjName& id, const cjName& propRttiName, const cjName& cntrRttiName,
	const cjName& cntrID, const cjName& evaluatorID, cjAniPoseBufChanItem pbChanItem, 
	cjAniPoseBufHandle& rstPBHandle, cjSWORD& rstLOD) const
{
    if(m_poseBind)
        return m_poseBind->GetHandleAndLOD(id, propRttiName, cntrRttiName, cntrID, evaluatorID, pbChanItem, rstPBHandle, rstLOD);

    rstPBHandle.Invalidate();

    return cjFalse;
}

cjBool cjAniPoseBuf::GetHandle(const cjAniEvaluatorTag& tag, cjAniPoseBufChanItem pbChanItem, 
	cjAniPoseBufHandle& rstPBHandle, cjSWORD& rstLod) const
{
    if(m_poseBind)
        return m_poseBind->GetHandleAndLOD(tag, pbChanItem, rstPBHandle, rstLod);

	rstPBHandle.Invalidate();

    return cjFalse;
}

void cjAniPoseBuf::ValidateAccumDelta()
{
    cjAssert(m_nonAccumStIndex == 2);
		
    // scale delta.
    if(m_vec3SCount > 1 && IsValidWeight(m_weightAry[m_vec3SStIndex]))
        SetVec3SValid(cjAniPoseBufHandle(cjAniPoseBufChanItem_Vec3S, 1), cjTrue);

    // pos delta.
    if(m_vec3PCount > 1 && IsValidWeight(m_weightAry[m_vec3PStIndex]))
        SetVec3PValid(cjAniPoseBufHandle(cjAniPoseBufChanItem_Vec3P, 1), cjTrue);

    if(m_rotCount > 1 && IsValidWeight(m_weightAry[m_rotStIndex]))
        SetRotValid(cjAniPoseBufHandle(cjAniPoseBufChanItem_Rot, 1), cjTrue);
}

cjBool cjAniPoseBuf::AddNewDatsFromPoseBind(cjBool copyFloatWeights, cjBool copyDats)
{
    cjAssert(m_poseBind);

    cjAniPoseBind* poseBind = m_poseBind;
	cjWORD totalDatCount = (cjWORD)poseBind->GetTotalBindCount();

    if(m_totalDatCount >= totalDatCount)
    {
        cjAssert(m_totalDatCount == totalDatCount);
        return cjFalse;
    }

    cjWORD colorDatCount	= (cjWORD)poseBind->GetColorBindCount();
    cjWORD boolDatCount		= (cjWORD)poseBind->GetBoolBindCount();
    cjWORD floatDatCount	= (cjWORD)poseBind->GetFloatBindCount();
    cjWORD vec3SDatCount	= (cjWORD)poseBind->GetVec3SBindCount();
	cjWORD vec3PDatCount	= (cjWORD)poseBind->GetVec3PBindCount();
    cjWORD rotDatCount		= (cjWORD)poseBind->GetRotBindCount();
    cjWORD refDatCount		= (cjWORD)poseBind->GetRefDatBindCount();

    cjAssert(totalDatCount == colorDatCount + boolDatCount + floatDatCount 
		+ vec3SDatCount + vec3PDatCount + rotDatCount + refDatCount);

    cjAssert(colorDatCount >= m_colorCount);
    cjAssert(boolDatCount >= m_boolCount);
    cjAssert(floatDatCount >= m_floatCount);
    cjAssert(vec3SDatCount >= m_vec3SCount);
	cjAssert(vec3PDatCount >= m_vec3PCount);
    cjAssert(rotDatCount >= m_rotCount);
    cjAssert(refDatCount >= m_refDatCount);

    cjUINT32* newFlagWeightAry = (cjUINT32*)cjMemAlloc(sizeof(cjUINT32) * totalDatCount);
    cjMemSet(newFlagWeightAry, 0, totalDatCount * sizeof(cjUINT32));
	    
    if(copyFloatWeights) // copy previous flag + weight ary
    {
        cjUINT32* curNewFlagWeight = newFlagWeightAry;

        for(cjUINT32 i=0; i<m_totalDatCount; i++)
        {
            if(i == m_boolStIndex)
                curNewFlagWeight += (colorDatCount - m_colorCount);

            if(i == m_floatStIndex)
                curNewFlagWeight += (boolDatCount - m_boolCount);

			if(i == m_vec3SStIndex)
                curNewFlagWeight += (floatDatCount - m_floatCount);

			if(i == m_vec3PStIndex)
                curNewFlagWeight += (vec3SDatCount - m_vec3SCount);

			if(i == m_rotStIndex)
                curNewFlagWeight += (vec3PDatCount - m_vec3PCount);

			if(i == m_refDatStIndex)
                curNewFlagWeight += (rotDatCount - m_rotCount);

			*curNewFlagWeight = m_weightAry[i];
            curNewFlagWeight++;
        }
    }
    else
    {
        m_validDatCount = 0;
    }

    cjMemDeAlloc(m_weightAry);
    m_weightAry = newFlagWeightAry;

    cjUINT32 datSiz = 
		colorDatCount	* sizeof(ColorDat) + 
        boolDatCount	* sizeof(BoolDat) + 
        floatDatCount	* sizeof(FloatDat) + 
        vec3SDatCount	* sizeof(Vec3SDat) + 
		vec3PDatCount	* sizeof(Vec3PDat) + 
        rotDatCount		* sizeof(RotDat) +
        refDatCount		* sizeof(RefDat);

    cjAssert(datSiz % 4 == 0);

	cjUINT32 newDatSiz = sizeof(cjFLOAT) * (datSiz/4);
    cjFLOAT* newDatBlock = (cjFLOAT*)cjMemAlloc(newDatSiz);

    if(copyDats)
    {
        cjFLOAT* prevNewDat = m_dats;
        cjFLOAT* curNewDat = newDatBlock;

        cjUINT32 itemSiz = sizeof(ColorDat) / 4;

        for(cjUINT32 i=0; i<m_totalDatCount; i++)
        {
            if(i == m_boolStIndex)
            {
                curNewDat += (colorDatCount - m_colorCount) * itemSiz;
                itemSiz = sizeof(BoolDat) / 4;
            }

            if(i == m_floatStIndex)
            {
                curNewDat += (boolDatCount - m_boolCount) * itemSiz;
                itemSiz = sizeof(FloatDat) / 4;
            }

            if(i == m_vec3SStIndex)
            {
                curNewDat += (floatDatCount - m_floatCount) * itemSiz;
                itemSiz = sizeof(Vec3SDat) / 4;
            }

            if(i == m_vec3PStIndex)
            {
                curNewDat += (vec3SDatCount - m_vec3SCount) * itemSiz;
                itemSiz = sizeof(Vec3SDat) / 4;
            }

            if(i == m_rotStIndex)
            {
                curNewDat += (vec3PDatCount - m_vec3PCount) * itemSiz;
                itemSiz = sizeof(RotDat) / 4;
            }

            if(i == m_refDatStIndex)
            {
                curNewDat += (rotDatCount - m_rotCount) * itemSiz;
                itemSiz = sizeof(RefDat) / 4;
            }

            for(cjUINT32 k=0; k<itemSiz; k++)
            {
                *curNewDat = *prevNewDat;
                prevNewDat++;
                curNewDat++;
            }
        }
    }

	cjMemDeAlloc(m_dats);
    m_dats = newDatBlock;

    m_totalDatCount			= totalDatCount;
    m_colorCount			= colorDatCount;
    m_boolCount				= boolDatCount;
    m_floatCount			= floatDatCount;
    m_vec3SCount			= vec3SDatCount;
	m_vec3PCount			= vec3SDatCount;
    m_rotCount				= rotDatCount;
    m_refDatCount			= refDatCount;	
    cjAssert(m_colorStIndex == 0);

    m_boolStIndex			= m_colorStIndex	+ m_colorCount;
    m_floatStIndex			= m_boolStIndex		+ m_boolCount;
    m_vec3SStIndex			= m_floatStIndex	+ m_floatCount;
	m_vec3PStIndex			= m_vec3SStIndex	+ m_vec3SCount;
    m_rotStIndex			= m_vec3PStIndex	+ m_vec3PCount;
    m_refDatStIndex			= m_rotStIndex		+ m_rotCount;
    cjAssert(m_totalDatCount == m_refDatStIndex	+ m_refDatCount);

    m_colors				= (ColorDat*)(m_dats);
    m_bools					= (BoolDat*)(&m_colors[m_colorCount]);
    m_floats				= (FloatDat*)(&m_bools[m_boolCount]);
    m_vec3Ss				= (Vec3SDat*)(&m_floats[m_floatCount]);
	m_vec3Ps				= (Vec3PDat*)(&m_vec3Ss[m_vec3SCount]);
    m_rots					= (RotDat*)(&m_vec3Ps[m_vec3PCount]);
    m_refDats				= (RefDat*)(&m_rots[m_rotCount]);

    cjAssert(&m_dats[datSiz / 4] == (cjFLOAT*)(&m_refDats[m_refDatCount]));

    return cjTrue;
}

void cjAniPoseBuf::Init(cjAniPoseBind* poseBind)
{
    cjAssert(poseBind);
    cjAssert(!m_poseBind);
    cjAssert(m_totalDatCount == 0);
    cjAssert(m_validDatCount == 0);
    cjAssert(!m_dats);
    cjAssert(!m_weightAry);

    m_poseBind = poseBind;

    m_nonAccumStIndex	= poseBind->GetNonAccumStIndex();
    m_totalDatCount		= (cjWORD)poseBind->GetTotalBindCount();
    m_colorCount		= (cjWORD)poseBind->GetColorBindCount();
    m_boolCount			= (cjWORD)poseBind->GetBoolBindCount();
    m_floatCount		= (cjWORD)poseBind->GetFloatBindCount();
    m_vec3SCount		= (cjWORD)poseBind->GetVec3SBindCount();
	m_vec3PCount		= (cjWORD)poseBind->GetVec3PBindCount();
    m_rotCount			= (cjWORD)poseBind->GetRotBindCount();
    m_refDatCount		= (cjWORD)poseBind->GetRefDatBindCount();

    cjAssert(m_totalDatCount == m_colorCount + m_boolCount + m_floatCount + m_vec3SCount + m_vec3PCount + m_rotCount + m_refDatCount);

    if(m_totalDatCount > 0)
    {
        m_weightAry = (cjUINT32*)cjMemAlloc(sizeof(cjUINT32) * m_totalDatCount);

        cjUINT32 datSiz = 
			m_colorCount	* sizeof(ColorDat) + 
            m_boolCount		* sizeof(BoolDat) + 
            m_floatCount	* sizeof(FloatDat) + 
            m_vec3SCount	* sizeof(Vec3SDat) + 
			m_vec3PCount	* sizeof(Vec3PDat) + 
            m_rotCount		* sizeof(RotDat) +
            m_refDatCount	* sizeof(RefDat);

        cjAssert(datSiz % 4 == 0);

        m_dats = (cjFLOAT*)cjMemAlloc(sizeof(cjFLOAT) * (datSiz/4));

        cjAssert(m_colorStIndex == 0);
        m_boolStIndex	= m_colorStIndex + m_colorCount;
        m_floatStIndex	= m_boolStIndex + m_boolCount;
        m_vec3SStIndex	= m_floatStIndex + m_floatCount;
		m_vec3PStIndex	= m_vec3SStIndex + m_vec3SCount;
        m_rotStIndex	= m_vec3PStIndex + m_vec3PCount;
        m_refDatStIndex = m_rotStIndex + m_rotCount;

        m_colors	= (ColorDat*)(m_dats);
        m_bools		= (BoolDat*)(&m_colors[m_colorCount]);
        m_floats	= (FloatDat*)(&m_bools[m_boolCount]);
        m_vec3Ss	= (Vec3SDat*)(&m_floats[m_floatCount]);
		m_vec3Ps	= (Vec3PDat*)(&m_vec3Ss[m_vec3SCount]);
        m_rots		= (RotDat*)(&m_vec3Ps[m_vec3PCount]);
        m_refDats	= (RefDat*)(&m_rots[m_rotCount]);
		
		cjAssert((cjBYTE*)m_refDats == ((cjBYTE*)m_dats + sizeof(cjFLOAT)*(datSiz/4)));
        cjAssert(&m_dats[datSiz / 4] == (cjFLOAT*)(&m_refDats[m_refDatCount]));

        InvalidateAllDats();
    }
}

void cjAniPoseBuf::Shutdown()
{
    m_poseBind = cjNULL;

    m_totalDatCount = 0;
    m_validDatCount = 0;
	m_nonAccumStIndex = 0;

	if(m_weightAry)
	{
		cjMemDeAlloc(m_weightAry);
		m_weightAry = cjNULL;
	}    

	if(m_dats)
	{
		cjMemDeAlloc(m_dats);
		m_dats = cjNULL;
	}

    m_colorCount = 0;
    m_colorStIndex = 0;
    m_colors = cjNULL;

    m_boolCount = 0;
    m_boolStIndex = 0;
    m_bools = cjNULL;

    m_floatCount = 0;
    m_floatStIndex = 0;
    m_floats = cjNULL;

    m_vec3SCount = 0;
    m_vec3SStIndex = 0;
    m_vec3Ss = cjNULL;

	m_vec3PCount = 0;
	m_vec3PStIndex = 0;
	m_vec3Ps = cjNULL;

    m_rotCount = 0;
    m_rotStIndex = 0;
    m_rots = cjNULL;

    m_refDatCount = 0;
    m_refDatStIndex = 0;
    m_refDats = cjNULL;
}

//
// cjAniMTPoseItpCntrInfo
//


//
// cjAniMultiTargetPoseHdr
//

CJSER_IMPL_CLASS(cjAniMTargetPoseHandler, cjRefObj)

//CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjAniMTargetPoseHandler)
// no need property copy
//CJSER_IMPL_CLONE_PROPCOPY_END(cjAniMTargetPoseHandler)

	 
void cjAniMTargetPoseHandler::_CommInit()
{
    m_owner						= cjNULL;
	m_lastUpdateTime			= CJ_FTIME_INVALID,

	m_accumRoot					= cjNULL; 
	m_moveAccumTMToChild		= cjFalse;

    m_interpCntrInfoCount		= 0;
	m_interpCntrInfoAry			= cjNULL;

    m_colorStIndex				= 0;
	m_boolStIndex				= 0;
    m_floatStIndex				= 0;
	m_vec3SStIndex				= 0;
	m_vec3PStIndex				= 0;
    m_rotStIndex				= 0;
	m_refDatStIndex				= 0;
}

cjAniMTargetPoseHandler::cjAniMTargetPoseHandler()
{
	_CommInit();
}

cjAniMTargetPoseHandler::cjAniMTargetPoseHandler(cjAniCntrManager* owner, cjBool moveAccumTMToChild)
{
	_CommInit();
	m_moveAccumTMToChild = moveAccumTMToChild;
    Init(owner, moveAccumTMToChild);
}

cjAniMTargetPoseHandler::~cjAniMTargetPoseHandler()
{
    Shutdown();
}

void cjAniMTargetPoseHandler::SetAccumTM(const cjQuatTM& tm)
{
    if(tm.IsScaleValid())
        m_accumTM.SetScale(tm.GetScale());

	if(tm.IsRotateValid())
        m_accumTM.SetRotate(tm.GetRotate());
 
	if(tm.IsPosValid())
        m_accumTM.SetPos(tm.GetPos());

    m_soleCntrSeq = NULL;

    if(m_accumRoot)
    {
		cjSceneNode* sceneNode = m_accumRoot->GetSceneNode();

		if(sceneNode)
		{
			cjAssert(m_accumTM.IsScaleValid());
			sceneNode->SetScale(m_accumTM.GetScale());

			cjAssert(m_accumTM.IsRotateValid());
			sceneNode->SetRotate(m_accumTM.GetRotate());

			cjAssert(m_accumTM.IsPosValid());
			sceneNode->SetPos(m_accumTM.GetPos());
		}
    }
}

void cjAniMTargetPoseHandler::ClearAccumTM()
{
    m_accumTM.SetScale(cjVec3::UNIT);
    m_accumTM.SetRotate(cjQuat::IDENTITY);
    m_accumTM.SetPos(cjVec3::ZERO);

    m_soleCntrSeq = cjNULL;

    if(m_accumRoot)
    {
		cjSceneNode* sceneNode = m_accumRoot->GetSceneNode();

		if(sceneNode)
		{
			sceneNode->SetScale(cjVec3::UNIT);
			sceneNode->SetRotate(cjQuat::IDENTITY);
			sceneNode->SetPos(cjVec3::ZERO);
		}
    }
}

void cjAniMTargetPoseHandler::Update(cjFLOAT etime, cjINT32 lod, cjAniPoseBuf* poseBuf, cjAniCntrSeq* aniCntrSeq)
{
    cjAssert(poseBuf);
    cjAssert(poseBuf->GetPoseBind());
    cjAssert(m_owner);
    cjAssert(poseBuf->GetPoseBind() == m_owner->GetPoseBind());

	cjAniMTPoseItpCntrInfo* curMTPoseInterpCntrInfo = cjNULL;

	cjUINT32 k = 0;
	cjUINT32 curNonAccumStIndex = cjAniPoseAccumEndIndex;
	cjQuatTM seqAccumTM[cjAniPoseAccumEndIndex];
	cjUINT32* flagWeights = poseBuf->GetWeightAry();
	
	//
	// #0 Color
	//
	{
		cjAniPoseBuf::ColorDat* colorDat	= poseBuf->GetColorDats();
		cjAniPoseBuf::ColorDat* endColorDat	= colorDat + poseBuf->GetColorCount();

		// color -> next: cjBool
		cjAssert((endColorDat - colorDat) <= (m_boolStIndex - m_colorStIndex));				
		curMTPoseInterpCntrInfo = m_interpCntrInfoAry + m_colorStIndex;

		while(colorDat < endColorDat)
		{
			if(lod <= curMTPoseInterpCntrInfo->m_lod && cjAniPoseBuf::IsValidWeight(*flagWeights))
			{
				cjAssert(!curMTPoseInterpCntrInfo->IsTransformEvaluator());
				cjAssert(curMTPoseInterpCntrInfo->m_dstAsInterpolationCntr);

				curMTPoseInterpCntrInfo->m_dstAsInterpolationCntr
					->UpdateValue(etime, colorDat->m_color, curMTPoseInterpCntrInfo->m_interpolatorIndex);
			}

			flagWeights++;
			curMTPoseInterpCntrInfo++;
			colorDat++;
		}
	}
	

	//
	// #1 cjBool
	//
	{
		cjAniPoseBuf::BoolDat* boolDat = poseBuf->GetBoolDats();
		cjAniPoseBuf::BoolDat* endBoolDat = boolDat + poseBuf->GetBoolCount();

		// cjBool -> next: cjFLOAT
		cjAssert((endBoolDat - boolDat) <= (m_floatStIndex - m_boolStIndex));
		cjAssert(flagWeights == poseBuf->GetWeightAry() + poseBuf->GetColorCount());

		curMTPoseInterpCntrInfo = m_interpCntrInfoAry + m_boolStIndex;
		while(boolDat < endBoolDat)
		{
			if(lod <= curMTPoseInterpCntrInfo->m_lod && cjAniPoseBuf::IsValidWeight(*flagWeights))
			{
				cjAssert(!curMTPoseInterpCntrInfo->IsTransformEvaluator());
				cjAssert(curMTPoseInterpCntrInfo->m_dstAsInterpolationCntr);

				curMTPoseInterpCntrInfo->m_dstAsInterpolationCntr->UpdateValue(
                        etime,
                        (cjBool)((boolDat->m_bool >= 0.5f) ? cjTrue : cjFalse),
                        curMTPoseInterpCntrInfo->m_interpolatorIndex);
			}

			flagWeights++;
			curMTPoseInterpCntrInfo++;
			boolDat++;
		}
	}


	//
	// #2 cjFLOAT
	//
	{
		curNonAccumStIndex = poseBuf->GetNonAccumStartIndex();
		cjAssert(curNonAccumStIndex <= cjAniPoseAccumEndIndex);
		
		cjAniPoseBuf::FloatDat*	floatDat	= poseBuf->GetFloatDat();
		cjAniPoseBuf::FloatDat*	endFloatDat	= floatDat + poseBuf->GetFloatCount();
				
		cjAssert((endFloatDat - floatDat) <= (m_vec3SStIndex - m_floatStIndex)); // cjFLOAT -> next: vec3		
		cjAssert(flagWeights == poseBuf->GetWeightAry() + poseBuf->GetColorCount() + poseBuf->GetBoolCount()); // color, cjBool

		curMTPoseInterpCntrInfo = m_interpCntrInfoAry + m_floatStIndex;
		while(k < curNonAccumStIndex && floatDat < endFloatDat)
		{
			if(lod <= curMTPoseInterpCntrInfo->m_lod && cjAniPoseBuf::IsValidWeight(*flagWeights))
			{
				cjAssert(curMTPoseInterpCntrInfo->IsTransformEvaluator());
				//seqAccumTM[k].SetScale(floatDat->m_float);
			}

			flagWeights++;
			curMTPoseInterpCntrInfo++;
			floatDat++;
			k++;
		}

		while(floatDat < endFloatDat)
		{
			if(lod <= curMTPoseInterpCntrInfo->m_lod && cjAniPoseBuf::IsValidWeight(*flagWeights))
			{
				if(curMTPoseInterpCntrInfo->IsTransformEvaluator())
				{
					cjAssert(!"this functions should not be called..!!");
					cjAssert(curMTPoseInterpCntrInfo->m_dstAsObj);
					//curMTPoseInterpCntrInfo->m_dstAsObj->SetScale(floatDat->m_float);
				}
				else
				{
					cjAssert(curMTPoseInterpCntrInfo->m_dstAsInterpolationCntr);
					curMTPoseInterpCntrInfo->m_dstAsInterpolationCntr->UpdateValue(etime, floatDat->m_float, 
						curMTPoseInterpCntrInfo->m_interpolatorIndex);
				}
			}
			flagWeights++;
			curMTPoseInterpCntrInfo++;
			floatDat++;
		}
	}
	
	
	//
	// Vec3S (for scale)
	//
	{
		cjAniPoseBuf::Vec3SDat* vec3SDat	= poseBuf->GetVec3SDat();
		cjAniPoseBuf::Vec3SDat* endVec3SDat	= vec3SDat + poseBuf->GetVec3SCount();

		// vec3S -> next: vec3P
		cjAssert((endVec3SDat - vec3SDat) <= (m_vec3PStIndex - m_vec3SStIndex));

		// color, cjBool cjFLOAT
		cjAssert(
			flagWeights == poseBuf->GetWeightAry() +
			poseBuf->GetColorCount() + 
			poseBuf->GetBoolCount() + 
			poseBuf->GetFloatCount());

		curMTPoseInterpCntrInfo = m_interpCntrInfoAry + m_vec3SStIndex;		
		
		k = 0;
		while(k < curNonAccumStIndex && vec3SDat < endVec3SDat)
		{
			if(lod <= curMTPoseInterpCntrInfo->m_lod && cjAniPoseBuf::IsValidWeight(*flagWeights))
			{
				cjAssert(curMTPoseInterpCntrInfo->IsTransformEvaluator());
				seqAccumTM[k].SetScale(vec3SDat->m_vec3);
			}

			flagWeights++;
			curMTPoseInterpCntrInfo++;
			vec3SDat++;
			k++;
		}

		while(vec3SDat < endVec3SDat)
		{
			if(lod <= curMTPoseInterpCntrInfo->m_lod && cjAniPoseBuf::IsValidWeight(*flagWeights))
			{
				if(curMTPoseInterpCntrInfo->IsTransformEvaluator())
				{
					cjAssert(curMTPoseInterpCntrInfo->m_dstAsObj);
					cjSceneNode* curDstSceneNode = curMTPoseInterpCntrInfo->m_dstAsObj->GetSceneNode();

					if(curDstSceneNode)					
						curDstSceneNode->SetScale(vec3SDat->m_vec3);
				}
				else
				{
					cjAssert(curMTPoseInterpCntrInfo->m_dstAsInterpolationCntr);
					curMTPoseInterpCntrInfo->m_dstAsInterpolationCntr->UpdateValue(etime, vec3SDat->m_vec3, 
						curMTPoseInterpCntrInfo->m_interpolatorIndex);
				}
			}
			flagWeights++;
			curMTPoseInterpCntrInfo++;
			vec3SDat++;
		}
	}


	//
	// Vec3P (for position)
	//
	{
		cjAniPoseBuf::Vec3PDat* vec3PDat		= poseBuf->GetVec3PDat();
		cjAniPoseBuf::Vec3PDat* endVec3PDat		= vec3PDat + poseBuf->GetVec3PCount();

		// vec3p -> next: rot
		cjAssert((endVec3PDat - vec3PDat) <= (m_rotStIndex - m_vec3PStIndex));

		// color, cjBool, cjFLOAT, vec3(scale)
		cjAssert(
			flagWeights == poseBuf->GetWeightAry() + 
			poseBuf->GetColorCount() + 
			poseBuf->GetBoolCount() + 
			poseBuf->GetFloatCount() +
			poseBuf->GetVec3SCount()
		);

		curMTPoseInterpCntrInfo = m_interpCntrInfoAry + m_vec3PStIndex;
		
		k = 0;
		while(k < curNonAccumStIndex && vec3PDat < endVec3PDat)
		{
			if(lod <= curMTPoseInterpCntrInfo->m_lod && cjAniPoseBuf::IsValidWeight(*flagWeights))
			{
				cjAssert(curMTPoseInterpCntrInfo->IsTransformEvaluator());
				seqAccumTM[k].SetPos(vec3PDat->m_vec3);
			}

			flagWeights++;
			curMTPoseInterpCntrInfo++;
			vec3PDat++;
			k++;
		}

		while(vec3PDat < endVec3PDat)
		{
			if(lod <= curMTPoseInterpCntrInfo->m_lod && cjAniPoseBuf::IsValidWeight(*flagWeights))
			{
				if(curMTPoseInterpCntrInfo->IsTransformEvaluator())
				{
					cjAssert(curMTPoseInterpCntrInfo->m_dstAsObj);
					cjSceneNode* curDstSceneNode = curMTPoseInterpCntrInfo->m_dstAsObj->GetSceneNode();

					if(curDstSceneNode)
						curDstSceneNode->SetPos(vec3PDat->m_vec3);
				}
				else
				{
					cjAssert(curMTPoseInterpCntrInfo->m_dstAsInterpolationCntr);
					curMTPoseInterpCntrInfo->m_dstAsInterpolationCntr->UpdateValue(etime, vec3PDat->m_vec3, 
						curMTPoseInterpCntrInfo->m_interpolatorIndex);
				}
			}
			flagWeights++;
			curMTPoseInterpCntrInfo++;
			vec3PDat++;
		}
	}


	//
	// Rot
	//
	{
		// Get rotation delta for accum root.
		cjAniPoseBuf::RotDat* rotDat	= poseBuf->GetRotDats();
		cjAniPoseBuf::RotDat* endRotDat = rotDat + poseBuf->GetRotCount();

		// rot -> next: reference
		cjAssert((endRotDat - rotDat) <= (m_refDatStIndex - m_rotStIndex));

		// color, cjBool, cjFLOAT, vec3S(scale), vec3P(pos)
		cjAssert(
			flagWeights == poseBuf->GetWeightAry() + 
			poseBuf->GetColorCount() + 
			poseBuf->GetBoolCount() +
			poseBuf->GetFloatCount() + 
			poseBuf->GetVec3SCount() +
			poseBuf->GetVec3PCount()
		);

		curMTPoseInterpCntrInfo = m_interpCntrInfoAry + m_rotStIndex;
		
		k = 0;
		while(k < curNonAccumStIndex && rotDat < endRotDat)
		{
			if(lod <= curMTPoseInterpCntrInfo->m_lod && cjAniPoseBuf::IsValidWeight(*flagWeights))
			{
				cjAssert(curMTPoseInterpCntrInfo->IsTransformEvaluator());
				seqAccumTM[k].SetRotate(rotDat->m_rotQuat);
			}
			flagWeights++;
			curMTPoseInterpCntrInfo++;
			rotDat++;
			k++;
		}

		while(rotDat < endRotDat)
		{
			if(lod <= curMTPoseInterpCntrInfo->m_lod && cjAniPoseBuf::IsValidWeight(*flagWeights))
			{
				if(curMTPoseInterpCntrInfo->IsTransformEvaluator())
				{
					cjAssert(curMTPoseInterpCntrInfo->m_dstAsObj);
					cjSceneNode* curDstSceneNode = curMTPoseInterpCntrInfo->m_dstAsObj->GetSceneNode();

					if(curDstSceneNode)
						curDstSceneNode->SetRotate(rotDat->m_rotQuat);
				}
				else
				{
					cjAssert(curMTPoseInterpCntrInfo->m_dstAsInterpolationCntr);
					curMTPoseInterpCntrInfo->m_dstAsInterpolationCntr->UpdateValue(
						etime, rotDat->m_rotQuat, curMTPoseInterpCntrInfo->m_interpolatorIndex);
				}
			}

			flagWeights++;
			curMTPoseInterpCntrInfo++;
			rotDat++;
		}
	}


	//
	// referenced item
	//
	{
		cjAniPoseBuf::RefDat* refItem		= poseBuf->GetRefDat();
		cjAniPoseBuf::RefDat* endRefItem	= refItem + poseBuf->GetRefDatCount();

		// referenced -> next : interpolationCntrInfo
		cjAssert((endRefItem - refItem) <= (cjINT32)(m_interpCntrInfoCount - m_refDatStIndex));

		// Color, Bool, Float,Vec3S,Vec3P,Rot
		cjAssert(flagWeights == poseBuf->GetWeightAry() + 
			poseBuf->GetColorCount() + 
			poseBuf->GetBoolCount() +
			poseBuf->GetFloatCount() + 
			poseBuf->GetVec3SCount() +
			poseBuf->GetVec3PCount() +
			poseBuf->GetRotCount() );

		curMTPoseInterpCntrInfo = m_interpCntrInfoAry + m_refDatStIndex;

		while(refItem < endRefItem)
		{
			if(lod <= curMTPoseInterpCntrInfo->m_lod && cjAniPoseBuf::IsValidWeight(*flagWeights))
			{
				cjAssert(!curMTPoseInterpCntrInfo->IsTransformEvaluator());
				cjAssert(curMTPoseInterpCntrInfo->m_dstAsInterpolationCntr);
				cjAssert(refItem->m_refEvaluator);

				curMTPoseInterpCntrInfo->m_dstAsInterpolationCntr->UpdateValue(etime, 
					refItem->m_cntrSeq, 
					refItem->m_refEvaluator, 
					(void**)&refItem->m_evalSPDat[0], // single or pos
					curMTPoseInterpCntrInfo->m_interpolatorIndex);
			}

			flagWeights++;
			curMTPoseInterpCntrInfo++;
			refItem++;
		}
	}
	
	
	//
	// end check
	//
			
	// color, cjBool, cjFLOAT, vec3S, vec3P, rot, ref block
    cjAssert(flagWeights == poseBuf->GetWeightAry() + 
        poseBuf->GetColorCount() + 
		poseBuf->GetBoolCount() +
        poseBuf->GetFloatCount() + 
		poseBuf->GetVec3SCount() +
		poseBuf->GetVec3PCount() +
        poseBuf->GetRotCount() + 
		poseBuf->GetRefDatCount());

    if(m_soleCntrSeq && m_soleCntrSeq->GetSeqState() == cjAniSeqState_InActive)
        m_soleCntrSeq = cjNULL;

 	// seqAccumTM[0] : cur sequence accum
	// seqAccumTM[1] : accum tm delta
    if(etime != m_lastUpdateTime && !seqAccumTM[1].IsTMInvalid())
    {
        if(aniCntrSeq)
        {
            if(aniCntrSeq != m_soleCntrSeq) // single sequence
            {
                m_soleCntrSeq = aniCntrSeq;

                if(seqAccumTM[0].IsTMInvalid())
                {
                    m_stAccumTM.SetScale(cjVec3::UNIT);
                    m_stAccumTM.SetRotate(cjQuat::IDENTITY);
                    m_stAccumTM.SetPos(cjVec3::ZERO);
                    m_stAccumRot.SetIdentity();
                }
                else
                {
                    // make inverse of last accum TM
                    cjQuatTM curSeqAccmInvTM;
                    seqAccumTM[0].InvertEachSRT(curSeqAccmInvTM);                    
					cjQuatTM lastInvTM = seqAccumTM[1] * curSeqAccmInvTM;

                    if(lastInvTM.IsRotateValid())
                    {
                        cjMatrix3 refRot;
						cjQuat lastRot = cjQuat::InverseAxis(lastInvTM.GetRotate());
                        lastRot.ToRotateMatrix(refRot);

                        lastInvTM.SetPos(curSeqAccmInvTM.GetPos() + refRot * seqAccumTM[1].GetPos());
                    }

					// scale
                    cjAssert(m_accumTM.IsScaleValid());
                    cjVec3 scale = m_accumTM.GetScale();

                    if(lastInvTM.IsScaleValid())
                        scale *= lastInvTM.GetScale();

					m_stAccumTM.SetScale(scale);

					// rotation
                    cjAssert(m_accumTM.IsRotateValid());
                    cjQuat rot = m_accumTM.GetRotate();
                    
					if(lastInvTM.IsRotateValid())
                        rot = rot * lastInvTM.GetRotate();

					m_stAccumTM.SetRotate(rot);
                    rot.ToRotateMatrix(m_stAccumRot);

					// pos
                    cjAssert(m_accumTM.IsPosValid());
                    cjVec3 pos = m_accumTM.GetPos();

                    if(lastInvTM.IsPosValid())
                        pos += m_stAccumRot * lastInvTM.GetPos();

					m_stAccumTM.SetPos(pos);
                }
            }

            if(seqAccumTM[0].IsPosValid())
                seqAccumTM[0].SetPos(m_stAccumRot * seqAccumTM[0].GetPos());

			cjQuatTM curAccumTM = m_stAccumTM * seqAccumTM[0];

			if(curAccumTM.IsScaleValid())
                m_accumTM.SetScale(curAccumTM.GetScale());

			if(curAccumTM.IsRotateValid())
                m_accumTM.SetRotate(curAccumTM.GetRotate());

			if(curAccumTM.IsPosValid())
                m_accumTM.SetPos(curAccumTM.GetPos());
        }
        else
        {
            m_soleCntrSeq = cjNULL;

			// seqAccumTM[1] contains accum tm delta
			if(seqAccumTM[1].IsPosValid())
            {
                cjAssert(m_accumTM.IsRotateValid());
                cjMatrix3 accumRot;

                m_accumTM.GetRotate().ToRotateMatrix(accumRot);
                seqAccumTM[1].SetPos(accumRot * seqAccumTM[1].GetPos());
            }

            cjQuatTM curAccmTM = m_accumTM * seqAccumTM[1];

            if(curAccmTM.IsScaleValid())
                m_accumTM.SetScale(curAccmTM.GetScale());

			if(curAccmTM.IsRotateValid())
                m_accumTM.SetRotate(curAccmTM.GetRotate());

			if(curAccmTM.IsPosValid())
                m_accumTM.SetPos(curAccmTM.GetPos());
        }
		
		cjSceneNode* accumRootSceneNode = m_accumRoot ? m_accumRoot->GetSceneNode() : cjNULL;

        if(accumRootSceneNode)
        {
            cjAssert(m_accumTM.IsScaleValid());
            accumRootSceneNode->SetScale(m_accumTM.GetScale());

            cjAssert(m_accumTM.IsRotateValid());
            accumRootSceneNode->SetRotate(m_accumTM.GetRotate());

            cjAssert(m_accumTM.IsPosValid());
            accumRootSceneNode->SetPos(m_accumTM.GetPos());
        }
    }

    m_lastUpdateTime = etime;
}

cjBool cjAniMTargetPoseHandler::FindTarget(const cjName& id, const cjName& propName, const cjName& cntrRttiName, 
    const cjName& cntrID, const cjName& evaluatorID, cjInterpolationAniCntr*& interpolationCntr, 
	cjSceneNode*& sceneNode)
{
    cjAssert(m_owner);
    cjAssert(m_owner->GetPoseBind());
    cjAssert(m_interpCntrInfoCount == m_owner->GetPoseBind()->GetTotalBindCount());

    interpolationCntr = cjNULL;
    sceneNode = cjNULL;

    cjAniPoseBind* poseBind = m_owner->GetPoseBind();
    cjAniPoseBindInfo* bindInfo = poseBind->GetBindInfo(id, propName, cntrRttiName, cntrID, evaluatorID);

    if(bindInfo)
    {
        cjAniPoseBufHandle pbHandle = bindInfo->GetFirstPBHandle();

        if(pbHandle.IsValid())
        {
            cjWORD auxIndex = pbHandle.GetAuxIndex();
            cjAniPoseBufChanItem pbChanItem = pbHandle.GetChannelItem();

            switch(pbChanItem)
            {
            case cjAniPoseBufChanItem_Color:		auxIndex = auxIndex + m_colorStIndex;	break;
            case cjAniPoseBufChanItem_Bool:			auxIndex = auxIndex + m_boolStIndex;	break;
            case cjAniPoseBufChanItem_Float:		auxIndex = auxIndex + m_floatStIndex;	break;
            case cjAniPoseBufChanItem_Vec3S:		auxIndex = auxIndex + m_vec3SStIndex;	break;
            case cjAniPoseBufChanItem_Vec3P:		auxIndex = auxIndex + m_vec3PStIndex;	break;
			case cjAniPoseBufChanItem_Rot:			auxIndex = auxIndex + m_rotStIndex;		break;
            case cjAniPoseBufChanItem_Ref:			auxIndex = auxIndex + m_refDatStIndex;		break;
            default:	cjAssert(cjFalse);
            }
			            
            cjAssert(auxIndex < m_interpCntrInfoCount);

            if(m_interpCntrInfoAry[auxIndex].IsTransformEvaluator())
				sceneNode = m_interpCntrInfoAry[auxIndex].m_dstAsObj->GetSceneNode();
            else
				interpolationCntr = m_interpCntrInfoAry[auxIndex].m_dstAsInterpolationCntr;

			return cjTrue;
        }
    }

    return cjFalse;
}

void cjAniMTargetPoseHandler::FillPoseSeqDat(const cjAniSeq* seqDatToSync, cjAniSeq* poseSeqDat)
{
    cjAssert(seqDatToSync);
    cjAssert(poseSeqDat);
    cjAssert(m_owner);

    cjAniPoseBind* poseBind = m_owner->GetPoseBind();
    cjAssert(poseBind);
    cjUINT32 evaluatorCount = seqDatToSync->GetEvaluatorCount();

    for(cjUINT32 i=0; i<evaluatorCount; i++)
    {
        cjAniEvaluator* evaluator = seqDatToSync->GetEvaluator(i);
        cjAssert(evaluator);

        const cjAniEvaluatorTag& evalTag = evaluator->GetTag();
        const cjAniPoseBindInfo* bindInfo = poseBind->GetBindInfo(evalTag);

        cjAniPoseBufHandle pbHandle;
        cjUINT32 maxEvalChannels = evaluator->GetMaxChannels(); // 1 or 3(TM Eval)
        cjBool foundChannelType[cjAniEvalChan_PRS_Max];

        if(bindInfo)
        {
            for(cjUINT32 k=0; k<maxEvalChannels; k++)
            {
                foundChannelType[k] = cjFalse;

                cjAniPoseBufChanItem pbChanItem = evaluator->GetPBChannelItemFromChanItem(k);
                if(pbChanItem != cjAniPoseBufChanItem_Invalid)
                {
                    cjAniPoseBufHandle pbHandleTemp = bindInfo->GetPBHandle(pbChanItem);
                    if(pbHandleTemp.IsValid())
                    {
                        pbHandle = pbHandleTemp;
                        foundChannelType[k] = cjTrue;
                    }
                    else
                    {
						cjAssert(!"unknown taget!");
                    }
                }
            }
        }

        if(!pbHandle.IsValid())
        {
			cjAssert(!"failed to find target..");
            continue;
        }

        cjWORD auxIndex = pbHandle.GetAuxIndex();
        cjAniPoseBufChanItem pbChanItem = pbHandle.GetChannelItem();

        switch(pbChanItem)
        {
        case cjAniPoseBufChanItem_Color:	auxIndex = auxIndex + m_colorStIndex;		break;
        case cjAniPoseBufChanItem_Bool:		auxIndex = auxIndex + m_boolStIndex;		break;
        case cjAniPoseBufChanItem_Float:	auxIndex = auxIndex + m_floatStIndex;		break;
        case cjAniPoseBufChanItem_Vec3S:	auxIndex = auxIndex + m_vec3SStIndex;		break;
		case cjAniPoseBufChanItem_Vec3P:	auxIndex = auxIndex + m_vec3PStIndex;		break;
        case cjAniPoseBufChanItem_Rot:		auxIndex = auxIndex + m_rotStIndex;			break;
        case cjAniPoseBufChanItem_Ref:		auxIndex = auxIndex + m_refDatStIndex;	break;
        default:  cjAssert(cjFalse); break;
        }

        cjAniEvaluator* poseEvaluator;
        cjAssert(auxIndex < m_interpCntrInfoCount);
        cjAniMTPoseItpCntrInfo* curMTPoseInterpCntrInfo = m_interpCntrInfoAry + auxIndex;

        if(curMTPoseInterpCntrInfo->IsTransformEvaluator())
        {
			cjAssert(evalTag.GetCntrID() == cjAniConstants::GetTransformCntrID());
            cjSceneNode* targetSceneNode = curMTPoseInterpCntrInfo->m_dstAsObj->GetSceneNode();
            
            cjQuatTM quatTM;

			if(targetSceneNode)
			{
				if(foundChannelType[cjAniEvalChan_Pos])
					quatTM.SetPos(targetSceneNode->GetPos());

				if(foundChannelType[cjAniEvalChan_Rot])
				{
					cjQuat rot;			
					targetSceneNode->GetRotate(rot);
					quatTM.SetRotate(rot);
				}

				if(foundChannelType[cjAniEvalChan_Scl])
					quatTM.SetScale(targetSceneNode->GetScale());
			}

            poseEvaluator = new cjConstTMEvaluator(quatTM);
        }
        else
        {
			cjAssert(evalTag.GetCntrID() != cjAniConstants::GetTransformCntrID());

            cjInterpolationAniCntr* interpCntr = curMTPoseInterpCntrInfo->m_dstAsInterpolationCntr;

            cjAssert(interpCntr);
            cjAssert(curMTPoseInterpCntrInfo->m_interpolatorIndex != cjInterpolationAniCntr_INVALID_INDEX);
            cjAssert(curMTPoseInterpCntrInfo->m_interpolatorIndex == interpCntr->GetInterpolatorIndex(evalTag.GetEvaluatorID()));

			poseEvaluator = interpCntr->_CreatePoseEvaluator(curMTPoseInterpCntrInfo->m_interpolatorIndex);
        }

        if(pbChanItem == cjAniPoseBufChanItem_Ref)
			poseEvaluator->SetChannelFlagReferenced();

        poseEvaluator->SetTag(evalTag);
        poseSeqDat->AddEvaluator(poseEvaluator);
    }
}

void cjAniMTargetPoseHandler::Init(cjAniCntrManager* ownerCntrMan, cjBool moveAccumTMToChild)
{
    cjAssert(ownerCntrMan && !m_owner);
    cjAssert(ownerCntrMan->GetPoseBind());

    m_owner = ownerCntrMan;

    cjAniPoseBind* poseBind		= ownerCntrMan->GetPoseBind();
    cjBool cumulativeAnimations	= ownerCntrMan->IsCumulativeAni();

    cjAssert(cumulativeAnimations == (poseBind->GetNonAccumStIndex() == cjAniPoseAccumEndIndex));

    if(cumulativeAnimations)
    {
        const cjName& accumRootName = poseBind->GetAccumRootName();

		if(accumRootName.IsValid())
        {
			cjRefObjNameHash* objHash = ownerCntrMan->GetNodesHash();

            if(objHash)
				m_accumRoot = objHash->FindObjWithSetObjWhenRootIsSetted(accumRootName);
            else
                m_accumRoot = cjAniCntrManager::FindAccumRoot((cjSceneNode*)ownerCntrMan->GetTarget());

            if(m_accumRoot)
            {
				cjSceneNode* accumRootSceneNode = m_accumRoot->GetSceneNode();

                cjAssert(m_accumRoot->GetName() == accumRootName);
                m_accumRootName = accumRootName;
				                
                cjQuat rot;

				if(accumRootSceneNode)
					accumRootSceneNode->GetRotate(rot);

				// if root transform is not identity, move it
				if(moveAccumTMToChild && accumRootSceneNode &&
					(	rot != cjQuat::IDENTITY || 
						accumRootSceneNode->GetPos() != cjVec3::ZERO || 
						accumRootSceneNode->GetScale() != cjVec3::UNIT
					))
                {
                    cjSceneNode* nonAccumRoot = 0;
                    const cjName nonAccumSuffixName = cjAniConstants::GetNonAccumSuffixName();
                    cjINT32 nonAccumLen = cjAniConstants::GetNonAccumSuffixNameLen();

					cjSceneNode* child = accumRootSceneNode->GetFirstChildSceneNode();

                    while(child)
                    {
						const cjName& childName = child->GetName();
						cjINT32 len = childName.GetLen() - nonAccumLen;

						if(len >= 0)
						{
							const cjNameChar* suffix = &childName[len];

							if(!cjStrcmp(suffix, nonAccumSuffixName))
							{
								// compare child name to accum root name when 'NonAccum' is founded in suffix
								if(cjStrncmp(childName, accumRootName, len) == 0)
								{
									nonAccumRoot = child;
									break;
								}
							}
						}
						
						child = child->GetSibilSceneNode();
					}

                    if(nonAccumRoot)
                    {
						nonAccumRoot->SetScale(	accumRootSceneNode->GetScale());
                        nonAccumRoot->SetRotate(accumRootSceneNode->GetRotate());
                        nonAccumRoot->SetPos(	accumRootSceneNode->GetPos());                        

						accumRootSceneNode->SetScale(cjVec3::UNIT);
                        accumRootSceneNode->SetRotate(cjQuat::IDENTITY);
                        accumRootSceneNode->SetPos(cjVec3::ZERO);                        
                    }
                }

                m_accumTM.SetScale(accumRootSceneNode->GetScale());

                accumRootSceneNode->GetRotate(rot);
                m_accumTM.SetRotate(rot);
                m_accumTM.SetPos(accumRootSceneNode->GetPos());

                if(!m_accumTM.IsScaleValid())
                    m_accumTM.SetScale(cjVec3::UNIT);

				if(!m_accumTM.IsRotateValid())
                    m_accumTM.SetRotate(cjQuat::IDENTITY);

				if(!m_accumTM.IsPosValid())
                    m_accumTM.SetPos(cjVec3::ZERO);
            }
        }
    }

    AddInterpControllerInfos();
}

void cjAniMTargetPoseHandler::Shutdown()
{
    m_soleCntrSeq = cjNULL;
    RemoveAllInterpControllerInfos();

    m_owner = cjNULL;
    m_accumRoot = cjNULL;
}

void cjAniMTargetPoseHandler::AddInterpControllerInfos()
{
    cjAssert(m_owner);
    cjAssert(m_owner->GetPoseBind());

	cjRefObjNameHash* objHash = m_owner->GetNodesHash();
	if(!objHash)
        return;

    cjAniPoseBind* poseBind = m_owner->GetPoseBind();
    cjUINT32 totalBindCount = poseBind->GetTotalBindCount();

    cjAssert(m_interpCntrInfoCount <= totalBindCount);
    if(m_interpCntrInfoCount == totalBindCount)
        return;

    cjAniMTPoseItpCntrInfo* newItpCntrAry = new cjAniMTPoseItpCntrInfo[totalBindCount];

    // Copy existing data.
    cjAniMTPoseItpCntrInfo* newInfo		= newItpCntrAry;
    cjAniMTPoseItpCntrInfo* oldInfo		= m_interpCntrInfoAry;
    cjAniMTPoseItpCntrInfo* endOldInfo	= m_interpCntrInfoAry;

    cjUINT32 channelIndex = 0;
    cjAssert(m_colorStIndex == 0);

    for(cjUINT32 k=0; k<m_interpCntrInfoCount; k++)
    {
        while(oldInfo == endOldInfo)
        {
            switch(channelIndex)
            {
            case 0:  // cjBool
				{
					endOldInfo = m_interpCntrInfoAry + m_boolStIndex;
				}
				break;

            case 1: // cjFLOAT
				{
					endOldInfo	= m_interpCntrInfoAry + m_floatStIndex;
					newInfo		+= poseBind->GetColorBindCount() - (m_boolStIndex - m_colorStIndex);
				}
                break;

            case 2: // vec3S (scale)
				{
					endOldInfo	 = m_interpCntrInfoAry + m_vec3SStIndex;
					newInfo		+= poseBind->GetBoolBindCount() - (m_floatStIndex - m_boolStIndex);
				}
                break;

            case 3: // vec3P (pos)
				{
					endOldInfo	 = m_interpCntrInfoAry + m_vec3PStIndex;
					newInfo		+= poseBind->GetFloatBindCount() - (m_vec3SStIndex - m_floatStIndex);
				}
                break;

            case 4: // rot
				{
					endOldInfo	 = m_interpCntrInfoAry + m_rotStIndex;
					newInfo		+= poseBind->GetVec3SBindCount() - (m_vec3PStIndex - m_vec3SStIndex);
				}
                break;

            case 5: // reference
				{
					endOldInfo	 = m_interpCntrInfoAry + m_refDatStIndex;
					newInfo		+= poseBind->GetVec3PBindCount() - (m_rotStIndex - m_vec3PStIndex);
				}
                break;

            case 6: // 
				{
					endOldInfo	 = m_interpCntrInfoAry + m_interpCntrInfoCount;
					newInfo		+= poseBind->GetRotBindCount() - (m_refDatStIndex - m_rotStIndex);
				}
                break;

            default:
                cjAssert(cjFalse);
				break;
            }

            channelIndex++;
        }
        
		*newInfo = *oldInfo;

        newInfo++;
        oldInfo++;

    } // end of while(oldInfo == endOldInfo)

	// change to newer
	if(m_interpCntrInfoAry)
		delete [] m_interpCntrInfoAry;

	m_interpCntrInfoAry = newItpCntrAry;

    cjBool needToUpdateSceneGraphFlags		= cjFalse;
    cjAniPoseBindInfo* bindInfoAry			= poseBind->GetBindInfoAry();
    cjWORD* bindInfoIndexList				= poseBind->GetBindInfoIndexList();
		
    channelIndex = 0;
    cjAniMTPoseItpCntrInfo* endNewInfo		= m_interpCntrInfoAry;
    cjAniMTPoseItpCntrInfo* endNewInfoAry	= m_interpCntrInfoAry + totalBindCount;

    for(newInfo = m_interpCntrInfoAry; newInfo < endNewInfoAry; newInfo++)
    {
        while(newInfo == endNewInfo && newInfo < endNewInfoAry)
        {
            switch(channelIndex)
            {
            case 0: // color
				{
					newInfo += (m_boolStIndex - m_colorStIndex);
					endNewInfo += poseBind->GetColorBindCount();
				}
                break;

            case 1: // cjBool
				{
					newInfo += (m_floatStIndex - m_boolStIndex);
					endNewInfo += poseBind->GetBoolBindCount();
				}
                break;

            case 2: // cjFLOAT
				{
					newInfo += (m_vec3SStIndex - m_floatStIndex);
					endNewInfo += poseBind->GetFloatBindCount();
				}
                break;

            case 3: // vec3S
				{
					newInfo += (m_vec3PStIndex - m_vec3SStIndex);
					endNewInfo += poseBind->GetVec3SBindCount();
				}
                break;

            case 4: // vec3P
				{
					newInfo += (m_rotStIndex - m_vec3PStIndex);
					endNewInfo += poseBind->GetVec3PBindCount();
				}
                break;

            case 5:  // Rot
                {
					newInfo += (m_refDatStIndex - m_rotStIndex);
					endNewInfo += poseBind->GetRotBindCount();
				}
                break;

            case 6:  // Ref
                {
					newInfo += (m_interpCntrInfoCount - m_refDatStIndex);
					endNewInfo = endNewInfoAry;
				}
                break;

            default:
                cjAssert(cjFalse);
            }

            channelIndex++;
        }

        if(newInfo >= endNewInfoAry)
        {
            cjAssert(newInfo == endNewInfoAry);
            break;
        }

        cjAssert(!newInfo->m_dstAsInterpolationCntr);
        cjAssert(newInfo->m_lod == LOD_SKIP_INTERPOLATION);

        cjWORD index = bindInfoIndexList[newInfo - m_interpCntrInfoAry];
        cjAssert(index < poseBind->GetBindInfoCount());

        cjAniPoseBindInfo& bindInfo = bindInfoAry[index];
		cjRefObj* dstObj = objHash->FindObjWithSetObjWhenRootIsSetted(bindInfo.m_id);

        if(!dstObj)
        {
			cjAssert(!"failed to find target");
            continue;
        }

		cjSceneNode* dstSceneNode = dstObj->GetSceneNode();

        if( bindInfo.m_cntrRttiName.IsValid() && 
			bindInfo.m_cntrRttiName == cjAniConstants::GetTransformCntrID())
        {
            cjTransformAniCntrRPtr spTMAniCntr = cjGetAniCntr(cjTransformAniCntr, dstObj);

            if(spTMAniCntr)
            {
				// transform controller is treated by specially, so remove
				dstObj->DelAniCntr(spTMAniCntr); 
                needToUpdateSceneGraphFlags = cjTrue;
            }

            newInfo->m_lod					= bindInfo.m_lod;
            newInfo->m_interpolatorIndex	= cjInterpolationAniCntr_INVALID_INDEX; 
            newInfo->m_dstAsObj				= dstObj;

			if(dstSceneNode)
			{
				dstSceneNode->SetSUFlagForceUpdateTM(cjTrue);
			}

            continue;
        }

        cjInterpolationAniCntr* interpCntr = cjNULL;

		// Resolve property target
		cjRefObj* targetPropObj = dstObj;
        if(bindInfo.m_propRttiName.IsValid())
        {
			const cjNameChar* propName = bindInfo.m_propRttiName;

			if(dstSceneNode)
				targetPropObj = dstSceneNode->FindScenePropByRttiName(propName);
			else
				targetPropObj = cjNULL;

            if(!targetPropObj)
            {
				cjAssert(!"Failed to find target(propObj)!");
                continue;
            }
        }

		cjAniCntr* targetCntr = targetPropObj->GetFirstAniCntr();
        while(targetCntr)
        {
			if(!cjStrcmp(targetCntr->cjrRTTI()->cjrGetNameW(), bindInfo.m_cntrRttiName))
            {
				cjAssert(cjrIsKindOf<cjInterpolationAniCntr>(targetCntr));

                const cjNameChar* targetCntrID = ((cjInterpolationAniCntr*)targetCntr)->GetCtlrID();
                if((!targetCntrID && !bindInfo.m_cntrID.IsValid()) || 
					(targetCntrID && bindInfo.m_cntrID.IsValid() && !cjStrcmp(targetCntrID, bindInfo.m_cntrID)))
                {
                    interpCntr = (cjInterpolationAniCntr*)targetCntr;
                    break;
                }
            }
			targetCntr = targetCntr->GetNextCntr();
        }

        if(!interpCntr)
        {
			//cjAssert(!"failed to find target!");
            continue;
        }

        cjWORD interpIdx = interpCntr->GetInterpolatorIndex(bindInfo.m_evaluatorID);
        if(interpIdx == cjInterpolationAniCntr_INVALID_INDEX)
        {
			cjAssert(!"failed to find target");
            continue;
        }

		interpCntr->SetManagerControlled(cjTrue);

        newInfo->m_lod						= bindInfo.m_lod;
        newInfo->m_interpolatorIndex		= interpIdx;
        newInfo->m_dstAsInterpolationCntr	= interpCntr;
    }

    m_interpCntrInfoCount = totalBindCount;
    cjAssert(m_colorStIndex == 0);

    m_boolStIndex		= (cjWORD)poseBind->GetColorBindCount()	+ m_colorStIndex;
    m_floatStIndex		= (cjWORD)poseBind->GetBoolBindCount()	+ m_boolStIndex;
    m_vec3SStIndex		= (cjWORD)poseBind->GetFloatBindCount()	+ m_floatStIndex;
	m_vec3PStIndex		= (cjWORD)poseBind->GetVec3SBindCount()	+ m_vec3SStIndex;
    m_rotStIndex		= (cjWORD)poseBind->GetVec3PBindCount()	+ m_vec3PStIndex;
    m_refDatStIndex		= (cjWORD)poseBind->GetRotBindCount()	+ m_rotStIndex;

    cjAssert(m_interpCntrInfoCount == m_refDatStIndex + poseBind->GetRefDatBindCount());

	// if removed tm aniCntr, update SelectiveUpdate
    if(needToUpdateSceneGraphFlags)
    {
        cjAssert(m_owner->GetTarget());
        cjAssert(cjrIsKindOf<cjSceneNode>(m_owner->GetTarget()));

        cjBool selectiveUpdate=cjFalse;
		cjBool isRigid=cjFalse;

        ((cjSceneNode*)m_owner->GetTarget())->SetNGetSeletiveUpdateFlags(selectiveUpdate, isRigid, cjTrue);
    }
}

void cjAniMTargetPoseHandler::RemoveAllInterpControllerInfos()
{
    for(cjUINT32 i=0; i<m_interpCntrInfoCount; i++)
        m_interpCntrInfoAry[i].ClearValues();

	if(m_interpCntrInfoAry)
	{
		delete [] m_interpCntrInfoAry;
		m_interpCntrInfoAry = cjNULL;
	}

    m_interpCntrInfoCount		= 0;
    m_colorStIndex			= 0;
    m_boolStIndex			= 0;
    m_floatStIndex			= 0;
    m_vec3SStIndex			= 0;
	m_vec3PStIndex			= 0;
    m_rotStIndex			= 0; 
    m_refDatStIndex		= 0;
}


//
// cjAniPoseBlend
//

CJSER_IMPL_CLASS(cjAniPoseBlend, cjRefObj)

//CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjAniPoseBlend)
// no need property copy
//CJSER_IMPL_CLONE_PROPCOPY_END(cjAniPoseBlend)

void cjAniPoseBlend::_InitCommon()
{
    m_owner					= cjNULL;

	m_poseBuf				= cjNULL;

	m_lodCount				= 0; 
    m_psLODs				= cjNULL;

	m_actPoseCount			= 0;
	m_actPoses				= cjNULL;

    m_bitPtnCount			= 0;

	m_flagWeightCount		= 0;
	m_flagAndWeightAry		= cjNULL;
}

cjAniPoseBlend::cjAniPoseBlend() 
{
	_InitCommon();
}

cjAniPoseBlend::cjAniPoseBlend(cjAniCntrManager* cntrMan) 
{
	_InitCommon();

    Init(cntrMan);
}

cjAniPoseBlend::~cjAniPoseBlend()
{
    Shutdown();
}

cjBool cjAniPoseBlend::Update(cjUINT32 blendCntrSeqCount, cjINT32 lod, cjAniPoseBuf*& poseBuf, 
	cjAniCntrSeq*& rstSequence)
{
    cjAssert(blendCntrSeqCount > 1);
    cjAssert(blendCntrSeqCount <= m_owner->GetSeqCount());
    cjAssert(m_owner);
    cjAssert(m_owner->GetPoseBind());

    if(blendCntrSeqCount > cjAniPoseBlend_MaxBlendSeqs)
    {
		cjLOG("WARN! - too many blend sequences..(%d)", blendCntrSeqCount);
        blendCntrSeqCount = cjAniPoseBlend_MaxBlendSeqs;
    }

    cjUINT32 flagAndWeightAryCount = blendCntrSeqCount * cjAniPoseBlend_MaxBitPatterns;

    if(flagAndWeightAryCount > m_flagWeightCount)
    {
        cjMemDeAlloc(m_flagAndWeightAry);

        m_flagAndWeightAry = (cjUINT32*)cjMemAlloc(flagAndWeightAryCount * sizeof(cjUINT32));
        m_flagWeightCount = flagAndWeightAryCount;
    }

    cjAniPoseBind* poseBind	= m_owner->GetPoseBind();
    cjUINT32 totalBindCount = poseBind->GetTotalBindCount();

    if(totalBindCount > m_lodCount)
        AddNewLODsFromPoseBind();

	cjAssert(m_lodCount == totalBindCount);

    if(blendCntrSeqCount > m_actPoseCount)
    {
        cjMemDeAlloc(m_actPoses);

        m_actPoses = (cjAniPoseActInfo*)cjMemAlloc(blendCntrSeqCount * sizeof(cjAniPoseActInfo));
        m_actPoseCount = blendCntrSeqCount;
    }

    for(cjUINT32 i=0; i<blendCntrSeqCount; i++)
    {
        cjAniCntrSeq* cntrSeq = m_owner->GetCntrSeq(i);
        cjAssert(cntrSeq);

        cjAniPoseBuf* poseBuf = cntrSeq->GetPoseBuf();
        cjAssert(poseBuf);
        cjAssert(poseBuf->GetPoseBind());
        cjAssert(poseBuf->GetPoseBind() == poseBind);
        cjAssert(poseBuf->GetTotalDatCount() == poseBuf->GetPoseBind()->GetTotalBindCount());

        cjAniPoseActInfo& activePose = m_actPoses[i];

        activePose.m_poseBuf			= poseBuf;
        activePose.m_flagAndWeightAry	= poseBuf->GetWeightAry();
        activePose.m_hasBlendItems		= cjFalse;
        activePose.m_priority			= cntrSeq->GetPriority();
        activePose.m_weight				= cntrSeq->GetSpinnerScaledWeight();
        activePose.m_cntrSeq			= cntrSeq;
    }
	
    m_bitPtnCount	= 0;
    cjSWORD slod	= (cjSWORD)lod;
    cjSWORD* psLOD	= m_psLODs;

    for(cjUINT32 i=0; i<m_lodCount; i++, psLOD++)
    {
        if(slod <= *psLOD)
        {
            cjUINT32 bitPattern = 0;
            cjUINT32 bit = 1 << (blendCntrSeqCount - 1);

            for(cjUINT32 m=0; m<blendCntrSeqCount; m++)
            {
                if(cjAniPoseBuf::IsValidWeight(*m_actPoses[m].m_flagAndWeightAry))
                    bitPattern |= bit;

				bit >>= 1;
            }

            cjUINT32 k = 0;
            for(; k < m_bitPtnCount; k++)
            {
                if((bitPattern & m_bitMasks[k]) == m_bitPtns[k])
                {
                    cjUINT32* flagAndWeight = m_flagAndWeightAry + k * blendCntrSeqCount;

                    for(cjUINT32 t=0; t<blendCntrSeqCount; t++)
                    {
                        if(cjAniPoseBuf::IsValidWeight(*m_actPoses[t].m_flagAndWeightAry))
                            *m_actPoses[t].m_flagAndWeightAry = *flagAndWeight; // block copy

						flagAndWeight++; // block move
                    }

                    break;
                }
            }

            if(k >= m_bitPtnCount)
            {
                cjINT32 highPriority		= CJ_INT_INVALID;
                cjINT32 secondPriority		= CJ_INT_INVALID;

                cjINT32 highCount			= 0;
                cjINT32 secondCount			= 0;

                cjFLOAT totalHighWeight		= 0.0f;
                cjFLOAT totalSecondWeight	= 0.0f;

                cjFLOAT maxHighEaseSpinner = 0.0f;

                cjUINT32 nonCulledValidDatCount = 0;
                cjUINT32 currentBit = 1 << (blendCntrSeqCount - 1);

                for(cjUINT32 m=0; m<blendCntrSeqCount; m++)
                {
                    cjAniPoseActInfo& activePose = m_actPoses[m];

                    if((bitPattern & currentBit) == 0)
                    {
                        // Item is invalid: no need to cull it.
                        cjAssert(!cjAniPoseBuf::IsValidWeight(*activePose.m_flagAndWeightAry));
                        cjAssert(*activePose.m_flagAndWeightAry == 0);

                        currentBit >>= 1;
                        continue;
                    }

                    // clear flag and weight
                    *activePose.m_flagAndWeightAry = (cjUINT32)cjAniPoseBuf::WDAT_FLAG_VALID;
                    nonCulledValidDatCount++;

                    if(highPriority == CJ_INT_INVALID)
                    {
                        highPriority = activePose.m_priority;

						maxHighEaseSpinner = activePose.m_cntrSeq->GetEaseSpinner();

                        cjAssert(maxHighEaseSpinner >= 0.0f);
                        cjAssert(maxHighEaseSpinner <= 1.0f);

                        totalHighWeight = activePose.m_weight;
                        highCount = 1;
                    }
                    else if(activePose.m_priority == highPriority)
                    {
                        totalHighWeight += activePose.m_weight;
                        highCount++;
                    }
                    else if(maxHighEaseSpinner < 1.0f && activePose.m_priority >= secondPriority)
                    {
                        cjAssert(activePose.m_priority < highPriority);

                        secondPriority		= activePose.m_priority;
                        totalSecondWeight	+= activePose.m_weight;
                        secondCount++;
                    }
                    else
                    {
                        cjAssert(cjAniPoseBuf::IsValidWeight(*activePose.m_flagAndWeightAry));

                        *activePose.m_flagAndWeightAry = (cjUINT32)(cjAniPoseBuf::WDAT_FLAG_VALID | cjAniPoseBuf::WDAT_FLAG_CULLED);
                        m++;

                        nonCulledValidDatCount--;

                        while(m < blendCntrSeqCount)
                        {
                            if(cjAniPoseBuf::IsValidWeight(*m_actPoses[m].m_flagAndWeightAry))
							{
                                *m_actPoses[m].m_flagAndWeightAry = (cjUINT32)(cjAniPoseBuf::WDAT_FLAG_VALID | cjAniPoseBuf::WDAT_FLAG_CULLED);
							}
                            else
							{
                                cjAssert(*m_actPoses[m].m_flagAndWeightAry == 0);
							}

                            m++;
                        }

                        break;
                    }

                    currentBit >>= 1;

                } // end of for 'm'

                cjFLOAT secondNormalizer = 0.0f;

                if(totalSecondWeight > 0.0f)
                {
                    secondNormalizer = (1.0f - maxHighEaseSpinner) / totalSecondWeight;
                }
                else
                {
                    if(secondCount == 0)
                        maxHighEaseSpinner = 1.0f;
                    else
                        secondNormalizer = (1.0f - maxHighEaseSpinner) / (cjFLOAT)secondCount;
                }

                cjFLOAT highNormalizer = 0.0f;

                if(totalHighWeight > 0.0f)
                {
                    highNormalizer = maxHighEaseSpinner / totalHighWeight;
                }
                else
                {
                    if(highCount > 0)
                        highNormalizer = maxHighEaseSpinner / (cjFLOAT)highCount;
                }

                cjUINT32 remainWeight = cjAniPoseBuf::WDAT_WEIGHT_CONV_INT_SCALER;
                cjUINT32 actPoseIndex = 0;

                while(nonCulledValidDatCount > 0)
                {
                    cjAssert(actPoseIndex < blendCntrSeqCount);

                    cjAniPoseActInfo& activePose = m_actPoses[actPoseIndex];
                    actPoseIndex++;

                    if(cjAniPoseBuf::IsValidWeightAndNotCulled(*activePose.m_flagAndWeightAry))
                    {
                        cjUINT32 curWeight = remainWeight;
                        nonCulledValidDatCount--;

                        if(nonCulledValidDatCount > 0)
                        {
                            cjFLOAT actWeight = activePose.m_weight;

							if(activePose.m_priority == highPriority)
                            {
                                if(totalHighWeight == 0.0f)
                                    actWeight = highNormalizer;
                                else
                                    actWeight *= highNormalizer;
                            }
                            else
                            {
                                if(totalSecondWeight == 0.0f)
                                    actWeight = secondNormalizer;
                                else
                                    actWeight *= secondNormalizer;
                            }

                            cjAssert(actWeight >= 0.0f && actWeight <= 1.0f);
                            curWeight = (cjUINT32)(actWeight * (cjFLOAT)cjAniPoseBuf::WDAT_WEIGHT_CONV_INT_SCALER);

                            if(curWeight > remainWeight)
                            {
                                cjAssert((curWeight - remainWeight) < 4);
                                curWeight = remainWeight;
                            }
                        }

                        if(curWeight > 0)
                        {
                            activePose.m_hasBlendItems = cjTrue;
                            *activePose.m_flagAndWeightAry = cjAniPoseBuf::WDAT_FLAG_VALID | curWeight;
                        }
                        else
                        {
                            // zero weight, cull
                            cjAssert(cjAniPoseBuf::IsValidWeight(*activePose.m_flagAndWeightAry));
                            *activePose.m_flagAndWeightAry = (cjUINT32)(cjAniPoseBuf::WDAT_FLAG_VALID | cjAniPoseBuf::WDAT_FLAG_CULLED);
                        }

                        cjAssert(remainWeight >= curWeight);
                        remainWeight -= curWeight;
                    }

                }

                cjAssert(remainWeight == 0 || remainWeight == cjAniPoseBuf::WDAT_WEIGHT_CONV_INT_SCALER);

                if(m_bitPtnCount < cjAniPoseBlend_MaxBitPatterns)
                {
                    cjUINT32 bitMask = 0;

                    if(currentBit > 0)
                        bitMask = (currentBit << 1) - 1;

					bitMask = ~bitMask;

                    m_bitMasks[m_bitPtnCount] = bitMask;
                    m_bitPtns[m_bitPtnCount] = bitPattern & bitMask;

                    cjUINT32* puiFlagWeight = m_flagAndWeightAry + m_bitPtnCount * blendCntrSeqCount;
                    currentBit = 1 << (blendCntrSeqCount - 1);

                    for(cjUINT32 t=0; t<blendCntrSeqCount; t++)
                    {
                        if((bitMask & currentBit) != 0)
                            *puiFlagWeight = *m_actPoses[t].m_flagAndWeightAry;
                        else
                            *puiFlagWeight = (cjUINT32)(cjAniPoseBuf::WDAT_FLAG_VALID | cjAniPoseBuf::WDAT_FLAG_CULLED);

                        currentBit >>= 1;
                        puiFlagWeight++;
                    }

                    m_bitPtnCount++;
                }

            } // end of if(k >= m_bitPtnCount)

            for(cjUINT32 t=0; t<blendCntrSeqCount; t++)
                m_actPoses[t].m_flagAndWeightAry++;
        }
        else
        {
            for(cjUINT32 m=0; m<blendCntrSeqCount; m++)
            {
                if(cjAniPoseBuf::IsValidWeight(*m_actPoses[m].m_flagAndWeightAry))
				{
                    *m_actPoses[m].m_flagAndWeightAry = (cjUINT32)(cjAniPoseBuf::WDAT_FLAG_VALID | cjAniPoseBuf::WDAT_FLAG_CULLED);
				}
                else
				{
                    cjAssert(*m_actPoses[m].m_flagAndWeightAry == 0);
				}

                m_actPoses[m].m_flagAndWeightAry++;
            }
        }
    }

    cjAniPoseActInfo* dstCopyIntoActivePose	= m_actPoses;
    cjAniPoseActInfo* curActivePose			= m_actPoses;
    cjAniPoseActInfo* endActivePose			= m_actPoses + blendCntrSeqCount;

    while(curActivePose < endActivePose)
    {
        if(curActivePose->m_hasBlendItems)
        {
            if(curActivePose != dstCopyIntoActivePose)
                *dstCopyIntoActivePose = *curActivePose; // copy

			dstCopyIntoActivePose++; // move block
        }

        curActivePose++;
    }

    blendCntrSeqCount = (cjUINT32)(dstCopyIntoActivePose - m_actPoses);

    if(blendCntrSeqCount == 0)
    {
        rstSequence = cjNULL;
        poseBuf = cjNULL;

        return cjFalse;
    }

    if(blendCntrSeqCount == 1)
    {
        cjAssert(m_actPoses[0].m_hasBlendItems);
        rstSequence = m_actPoses[0].m_cntrSeq;
        poseBuf = m_actPoses[0].m_poseBuf;

        return cjTrue;
    }

    poseBuf = m_poseBuf;
    cjAssert(poseBuf);
    cjAssert(poseBuf->GetPoseBind() == m_owner->GetPoseBind());

    if(!poseBuf->AddNewDatsFromPoseBind(cjFalse, cjFalse))
        poseBuf->InvalidateAllDats();

    curActivePose = m_actPoses;
    endActivePose = m_actPoses + blendCntrSeqCount;

    while(curActivePose < endActivePose)
    {
        curActivePose->m_flagAndWeightAry = curActivePose->m_poseBuf->GetWeightAry();
        curActivePose++;
    }

	
	//
    // Color
	//

    cjUINT32 colorCount = poseBuf->GetColorCount();

    for(cjUINT32 i=0; i<colorCount; i++)
    {
        cjBool valid = cjFalse;

        cjColorF finalColor(cjColorF::BLACK);
        cjAniPoseBufHandle pbHandle(cjAniPoseBufChanItem_Color, (cjWORD)i);
        curActivePose = m_actPoses;

        while(curActivePose < endActivePose)
        {
            cjColorF color;
			if(curActivePose->m_poseBuf->GetColor(pbHandle, color, cjTrue))
            {
                valid = cjTrue;

                cjFLOAT weight = cjAniPoseBuf::GetUnitScaledWeight(*curActivePose->m_flagAndWeightAry);
                finalColor += color * weight;
            }

            curActivePose->m_flagAndWeightAry++;
            curActivePose++;
        }

        if(valid)
        {
            poseBuf->SetColorValid(pbHandle, cjTrue);
            poseBuf->SetColor(pbHandle, finalColor);
        }
    }

	
	//
    // cjBool
	//

    cjUINT32 countBool = poseBuf->GetBoolCount();

    for(cjUINT32 i=0; i<countBool; i++)
    {
        cjBool valid = cjFalse;
        cjFLOAT finalBool = 0.0f;
        
		cjAniPoseBufHandle pbHandle(cjAniPoseBufChanItem_Bool, (cjWORD)i);
        curActivePose = m_actPoses;
    
		while(curActivePose < endActivePose)
        {
            cjFLOAT valBool;
            if(curActivePose->m_poseBuf->GetBool(pbHandle, valBool, cjTrue))
            {
                valid = cjTrue;
                cjFLOAT weight = cjAniPoseBuf::GetUnitScaledWeight(*curActivePose->m_flagAndWeightAry);
                finalBool += valBool * weight;
            }
            curActivePose->m_flagAndWeightAry++;
            curActivePose++;
        }

        if(valid)
        {
            poseBuf->SetBoolValid(pbHandle, cjTrue);
            poseBuf->SetBool(pbHandle, finalBool);
        }
    }


	//
    // cjFLOAT.
	//

    cjUINT32 floatCount = poseBuf->GetFloatCount();

    for(cjUINT32 i=0; i<floatCount; i++)
    {
        cjBool valid = cjFalse;
        cjFLOAT finalFloat = 0.0f;

        cjAniPoseBufHandle pbHandle(cjAniPoseBufChanItem_Float, (cjWORD)i);
        curActivePose = m_actPoses;

        while(curActivePose < endActivePose)
        {
            cjFLOAT valFloat;
            if(curActivePose->m_poseBuf->GetFloat(pbHandle, valFloat, cjTrue))
            {
                valid = cjTrue;
                cjFLOAT weight = cjAniPoseBuf::GetUnitScaledWeight(*curActivePose->m_flagAndWeightAry);

                finalFloat += valFloat * weight;
            }

            curActivePose->m_flagAndWeightAry++;
            curActivePose++;
        }

        if(valid)
        {
            poseBuf->SetFloatValid(pbHandle, cjTrue);
            poseBuf->SetFloat(pbHandle, finalFloat);
        }
    }


	//
    // Vec3S
	//

    cjUINT32 vec3SCount = poseBuf->GetVec3SCount();

    for(cjUINT32 i=0; i<vec3SCount; i++)
    {
        cjBool valid = cjFalse;
        cjVec3 finalVec3S(cjVec3::ZERO);

        cjAniPoseBufHandle pbHandle(cjAniPoseBufChanItem_Vec3S, (cjWORD)i);
        curActivePose = m_actPoses;

        while(curActivePose < endActivePose)
        {
            cjVec3 vec3;
            if(curActivePose->m_poseBuf->GetVec3S(pbHandle, vec3, cjTrue))
            {
                valid = cjTrue;
                cjFLOAT weight = cjAniPoseBuf::GetUnitScaledWeight(*curActivePose->m_flagAndWeightAry);
                finalVec3S += vec3 * weight;
            }

            curActivePose->m_flagAndWeightAry++;
            curActivePose++;
        }

        if(valid)
        {
            poseBuf->SetVec3SValid(pbHandle, cjTrue);
            poseBuf->SetVec3S(pbHandle, finalVec3S);
        }
    }


	//
    // Vec3P
	//

    cjUINT32 vec3PCount = poseBuf->GetVec3PCount();

    for(cjUINT32 i=0; i<vec3PCount; i++)
    {
        cjBool valid = cjFalse;
        cjVec3 finalVec3P(cjVec3::ZERO);

        cjAniPoseBufHandle pbHandle(cjAniPoseBufChanItem_Vec3P, (cjWORD)i);
        curActivePose = m_actPoses;

        while(curActivePose < endActivePose)
        {
            cjVec3 vec3;
            if(curActivePose->m_poseBuf->GetVec3P(pbHandle, vec3, cjTrue))
            {
                valid = cjTrue;
                cjFLOAT weight = cjAniPoseBuf::GetUnitScaledWeight(*curActivePose->m_flagAndWeightAry);
                finalVec3P += vec3 * weight;
            }

            curActivePose->m_flagAndWeightAry++;
            curActivePose++;
        }

        if(valid)
        {
            poseBuf->SetVec3PValid(pbHandle, cjTrue);
            poseBuf->SetVec3P(pbHandle, finalVec3P);
        }
    }


	//
	// rot  (normalized lerp)
	//

    cjUINT32 rotCount = poseBuf->GetRotCount();

    for(cjUINT32 i=0; i<rotCount; i++)
    {
        cjBool valid = cjFalse;

        cjQuat finalRot(0.0f, 0.0f, 0.0f, 0.0f);
        cjAniPoseBufHandle pbHandle(cjAniPoseBufChanItem_Rot, (cjWORD)i);
        curActivePose = m_actPoses;

        while(curActivePose < endActivePose)
        {
            cjQuat rot;
            if(curActivePose->m_poseBuf->GetRot(pbHandle, rot, cjTrue))
            {
                if(valid)
                {
                    cjFLOAT angle = cjQuat::Dot(finalRot, rot);

                    if(angle < 0.0f)
                        rot = -rot; // negative angle, invert
                }

                valid = cjTrue;
                cjFLOAT weight = cjAniPoseBuf::GetUnitScaledWeight(*curActivePose->m_flagAndWeightAry);

                finalRot.x = rot.x * weight + finalRot.x;
                finalRot.y = rot.y * weight + finalRot.y;
                finalRot.z = rot.z * weight + finalRot.z;
				finalRot.w = rot.w * weight + finalRot.w;
            }

            curActivePose->m_flagAndWeightAry++;
            curActivePose++;
        }

        if(valid)
        {
            finalRot.Normalize();

            poseBuf->SetRotValid(pbHandle, cjTrue);
            poseBuf->SetRot(pbHandle, finalRot);
        }
    }


	//
	// RefDat
	//
    
    cjUINT32 refDatCount = poseBuf->GetRefDatCount();

    for(cjUINT32 i=0; i<refDatCount; i++)
    {
        cjFLOAT maxWeight = 0.0f;

		cjAniPoseBuf::RefDat refDat;

        refDat.m_cntrSeq = cjNULL;
        refDat.m_refEvaluator = cjNULL;

        cjAniPoseBufHandle pbHandle(cjAniPoseBufChanItem_Ref, (cjWORD)i);

        curActivePose = m_actPoses;

        while(curActivePose < endActivePose)
        {
            cjAniPoseBuf::RefDat refDat;

            if(curActivePose->m_poseBuf->GetRefDat(pbHandle, refDat, cjTrue))
            {
                cjFLOAT weight = cjAniPoseBuf::GetUnitScaledWeight(*curActivePose->m_flagAndWeightAry);

                if(weight > maxWeight)
                {
                    maxWeight = weight;
                    refDat = refDat;
                }
            }

            curActivePose->m_flagAndWeightAry++;
            curActivePose++;
        }

        if(maxWeight > 0.0f)
        {
            poseBuf->SetRefDatValid(pbHandle, cjTrue);
            poseBuf->SetRefDat(pbHandle, refDat);
        }
    }

    rstSequence = cjNULL;

    return cjTrue;
}

void cjAniPoseBlend::Init(cjAniCntrManager* cntrMan)
{
    cjAssert(cntrMan && !m_owner);
    cjAssert(cntrMan->GetPoseBind());
    cjAssert(!m_poseBuf);
    cjAssert(!m_psLODs && m_lodCount == 0);

    m_owner = cntrMan;

    cjAniPoseBind* poseBind = cntrMan->GetPoseBind();	
    m_poseBuf = new cjAniPoseBuf(poseBind);

    cjUINT32 totalBindCount = poseBind->GetTotalBindCount();

	if(totalBindCount > 0)
        AddNewLODsFromPoseBind();
}

void cjAniPoseBlend::Shutdown()
{
    cjMemDeAlloc(m_actPoses);
    m_actPoses			= cjNULL;
    m_actPoseCount		= 0;

    cjMemDeAlloc(m_psLODs);
    m_psLODs			= cjNULL;
    m_lodCount			= 0;

    cjMemDeAlloc(m_flagAndWeightAry);
    m_flagAndWeightAry		= cjNULL;
    m_flagWeightCount	= 0;
    m_bitPtnCount		= 0;

    m_poseBuf			= cjNULL;
    m_owner				= cjNULL;
}

void cjAniPoseBlend::AddNewLODsFromPoseBind()
{
    cjAssert(m_owner);
    cjAssert(m_owner->GetPoseBind());

    cjMemDeAlloc(m_psLODs);

    cjAniPoseBind* poseBind = m_owner->GetPoseBind();

    cjAssert(m_lodCount < poseBind->GetTotalBindCount());
    m_lodCount = poseBind->GetTotalBindCount();
    m_psLODs = (cjSWORD*)cjMemAlloc(m_lodCount * sizeof(cjSWORD));

    cjAniPoseBindInfo* bindInfos = poseBind->GetBindInfoAry();
    cjWORD* bindInfoIndexLst = poseBind->GetBindInfoIndexList();

    for(cjUINT32 i=0; i<m_lodCount; i++)
    {
        cjWORD index = bindInfoIndexLst[i];
        cjAssert(index < poseBind->GetBindInfoCount());

        m_psLODs[i] = bindInfos[index].m_lod;
    }
}

