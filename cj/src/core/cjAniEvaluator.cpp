
#include "cjCorePCH.h"
#include "cjAniEvaluator.h"


//
// cjAniEvaluator
//

CJSER_IMPL_CLASS(cjAniEvaluator, cjRefObj)

CJSER_IMPL_SERIALIZE_BEGIN(cjAniEvaluator)

	if(ser.IsLoading() || ser.IsSaving())
	{
		ser << m_tag;

		for(cjEnumType i=0; i<cjAniEvalChan_Item_MAX; i++)
		{
			ser << m_channelItem[i];
		}
	}
	
CJSER_IMPL_SERIALIZE_END(cjAniEvaluator)

CJSER_IMPL_SERIALIZE_PTR_ARY(cjAniEvaluator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjAniEvaluator)
	
	d->m_tag = m_tag;
	cjMemCpy(d->m_channelItem, m_channelItem, sizeof(m_channelItem));

CJSER_IMPL_CLONE_PROPCOPY_END(cjAniEvaluator)


cjAniEvaluator::cjAniEvaluator()
{
	_CommInit();
}

cjAniEvaluator::cjAniEvaluator(const cjName& id, const cjName& propRttiName, const cjName& cntrRttiName, 
	const cjName& cntrID, const cjName& evaluatorID)
	: m_tag(id, propRttiName, cntrRttiName, cntrID, evaluatorID)
{
	_CommInit();
}

cjAniEvaluator::cjAniEvaluator(const cjAniEvaluatorTag& tag) : m_tag(tag)
{
	_CommInit();
}

cjAniEvaluator::~cjAniEvaluator()
{
	m_tag.Reset();
}

void cjAniEvaluator::_CommInit()
{
    m_channelItem[cjAniEvalChan_Pos] = cjAniEvalChanItem_NoData;
    m_channelItem[cjAniEvalChan_Rot] = cjAniEvalChanItem_NoData;
    m_channelItem[cjAniEvalChan_Scl] = cjAniEvalChanItem_NoData;
	m_channelItem[cjAniEvalChan_Flag] = 0;
}

void cjAniEvaluator::GetTagInfo(const cjNameChar*& id, const cjNameChar*& propRttiName, 
	const cjNameChar*& cntrRttiName, const cjNameChar*& cntrID, const cjNameChar*& evaluatorID) const
{
	id = *m_tag.GetID();
	propRttiName = *m_tag.GetPropRttiName();
	cntrRttiName = *m_tag.GetCntrRttiName();
	cntrID = *m_tag.GetCntrID();
	evaluatorID = *m_tag.GetEvaluatorID();
}

void cjAniEvaluator::GetTagInfo(cjName& id, cjName& propRttiName, cjName& cntrRttiName, 
	cjName& cntrID, cjName& evaluatorID) const
{
	id = m_tag.GetID();
	propRttiName = m_tag.GetPropRttiName();
	cntrRttiName = m_tag.GetCntrRttiName();
	cntrID = m_tag.GetCntrID();
	evaluatorID = m_tag.GetEvaluatorID();
}

void cjAniEvaluator::SetTagInfo(const cjName& id, const cjName& propRttiName, const cjName& cntrRttiName, 
	const cjName& cntrID, const cjName& evaluatorID)
{
	m_tag.SetID(id);
	m_tag.SetPropRttiName(propRttiName);
	m_tag.SetCntrRttiName(cntrRttiName);
	m_tag.SetCntrID(cntrID);
	m_tag.SetEvaluatorID(evaluatorID);
}


//
// channel management
//

cjAniPoseBufChanItem cjAniEvaluator::GetPBChannelItemFromChanItem(cjUINT32 channel) const
{
    cjAssert(channel < GetMaxChannels());

    cjAniPoseBufChanItem poseBufChanItem = cjAniPoseBufChanItem_Ref;

    if(!IsReferenced())
    {
        cjAssert(m_channelItem[channel] != (cjAniEvalChanItemMask_Constant | cjAniEvalChanItem_NoData));

        if(m_channelItem[channel] != cjAniEvalChanItem_NoData)
            poseBufChanItem = (cjAniPoseBufChanItem)(1 << ((m_channelItem[channel] & ~cjAniEvalChanItemMask_Constant) - 1));
        else
            poseBufChanItem = cjAniPoseBufChanItem_Invalid;
    }

    cjAssert(poseBufChanItem != cjAniPoseBufChanItem_Invalid || IsTransformEvaluator());

    return poseBufChanItem;
}

void cjAniEvaluator::GetPBChannelItemFromChanItem(cjUINT32 channel, cjAniPoseBufChanItem& pbChannelItem, cjBool& isPosed) const
{
	cjAssert(channel < GetMaxChannels());

	if(!IsReferenced())
    {
        cjAssert(m_channelItem[channel] != (cjAniEvalChanItemMask_Constant | cjAniEvalChanItem_NoData));

        if(m_channelItem[channel] != cjAniEvalChanItem_NoData)
        {
            pbChannelItem = (cjAniPoseBufChanItem)(1 << ((m_channelItem[channel] & ~cjAniEvalChanItemMask_Constant) - 1));
            isPosed = (m_channelItem[channel] & cjAniEvalChanItemMask_Constant) != 0;
        }
        else
        {
            pbChannelItem = cjAniPoseBufChanItem_Invalid;
            isPosed = cjFalse;
        }
    }
    else
    {
        pbChannelItem = cjAniPoseBufChanItem_Ref;
        isPosed = cjTrue;
    }

    cjAssert(pbChannelItem != cjAniPoseBufChanItem_Invalid || IsTransformEvaluator());
}

cjAniPoseBufChanItem cjAniEvaluator::GetPBChannelItemFromChanItemWithNoCareFlag(cjUINT32 channel) const
{
	cjAssert(channel < GetMaxChannels());

	cjAniPoseBufChanItem pbChanItem = cjAniPoseBufChanItem_Invalid;
	cjAssert(m_channelItem[channel] != (cjAniEvalChanItemMask_Constant | cjAniEvalChanItem_NoData));

    if(m_channelItem[channel] != cjAniEvalChanItem_NoData)
    {
		cjBYTE chanType = m_channelItem[channel] & (~cjAniEvalChanItemMask_Constant);
		pbChanItem = (cjAniPoseBufChanItem)(1 << (chanType -1)); 
		// -1 for(0,1,2)   1 << 0 : 1,  1 << 1 : 2,  1 << 2 : 4
    }

	cjAssert(pbChanItem != cjAniPoseBufChanItem_Invalid || IsTransformEvaluator());

	return pbChanItem;
}


//
// cjConstTMEvaluator
// 

CJSER_IMPL_CLASS(cjConstTMEvaluator, cjAniEvaluator)

CJSER_IMPL_SERIALIZE_BEGIN(cjConstTMEvaluator)

	if(ser.IsLoading() || ser.IsSaving())
	{
		ser << m_value;
	}

CJSER_IMPL_SERIALIZE_END(cjConstTMEvaluator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjConstTMEvaluator)
	d->m_value = m_value;
CJSER_IMPL_CLONE_PROPCOPY_END(cjConstTMEvaluator)

CJSER_IMPL_CLONE(cjConstTMEvaluator)


cjConstTMEvaluator::cjConstTMEvaluator()
{
    m_channelItem[cjAniEvalChan_Flag] |= cjAniEvalFlagChanVal_Transform;
}

cjConstTMEvaluator::cjConstTMEvaluator(const cjQuatTM& poseValue)
{
	m_value = poseValue;
    m_channelItem[cjAniEvalChan_Flag] |= cjAniEvalFlagChanVal_Transform;

    UpdateChannelItem();
}

cjConstTMEvaluator::~cjConstTMEvaluator()
{
}

cjBool cjConstTMEvaluator::GetChannelPosedValue(cjUINT32 channel, void* rst) const
{
    cjAssert(channel < cjAniEvalChan_PRS_Max);
    cjAssert(rst);

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChannelItemInvalid(channel));

        if(channel == cjAniEvalChan_Scl)
        {
            *(cjVec3*)rst = m_value.GetScale();
        }
        else if(channel == cjAniEvalChan_Pos)
        {
            *(cjVec3*)rst = m_value.GetPos();
        }
        else
        {
            cjAssert(channel == cjAniEvalChan_Rot);
            *(cjQuat*)rst = m_value.GetRotate();
        }
        return cjTrue;
    }

    cjAssert(IsChannelItemInvalid(channel));

    return cjFalse;
}

cjBool cjConstTMEvaluator::UpdateChannel(cjFLOAT , cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const
{
    cjAssert(channel < cjAniEvalChan_PRS_Max);
    cjAssert(evalSPDat);
    cjAssert(evalSPDat->GetEvaluator() == this);
	cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(rst);

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChannelItemInvalid(channel));

        if(channel == cjAniEvalChan_Scl)
        {
            cjAniSPad::CubicVec3Seg* cubicVec3Seg = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();
            cjAssert(cubicVec3Seg);

			*(cjVec3*)rst = cubicVec3Seg->m_val0;

            cjAssert(cubicVec3Seg->m_outTangent0 == cjVec3::ZERO);
            cjAssert(cubicVec3Seg->m_a0 == cjVec3::ZERO);
            cjAssert(cubicVec3Seg->m_b0 == cjVec3::ZERO);
        }
        else if(channel == cjAniEvalChan_Pos)
        {
            cjAniSPad::CubicVec3Seg* cubicVec3Seg = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();
            cjAssert(cubicVec3Seg);

			*(cjVec3*)rst = cubicVec3Seg->m_val0;

			cjAssert(cubicVec3Seg->m_outTangent0 == cjVec3::ZERO);
			cjAssert(cubicVec3Seg->m_a0 == cjVec3::ZERO);
			cjAssert(cubicVec3Seg->m_b0 == cjVec3::ZERO);
        }
        else
        {
            cjAssert(channel == cjAniEvalChan_Rot);

            cjAniSPad::SlerpRotSeg* slerpRotSeg = (cjAniSPad::SlerpRotSeg*)evalSPDat->GetSegDat();
            cjAssert(slerpRotSeg);

			*(cjQuat*)rst = slerpRotSeg->m_quat0;
            cjAssert(slerpRotSeg->m_quat1 == slerpRotSeg->m_quat0);
        }

        return cjTrue;
    }

    cjAssert(IsChannelItemInvalid(channel));

    return cjFalse;
}

cjBool cjConstTMEvaluator::GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, 
    cjRefObjNameHash*, cjUINT32& fillSiz, cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, 
    cjBSplineBasisF3Dat*& bsBasisDat) const
{
    cjAssert(channel < cjAniEvalChan_PRS_Max);

    if(IsChannelItemInvalid(channel) || (!forceAlwaysUpdate && !IsAlwaysUpdate()))
        return cjFalse; // no need to handle for constant or invalid 

    fillSiz = 0;
    isSharedFillDat = cjFalse;
    bsBasisDat = 0;

    if(channel == cjAniEvalChan_Pos)
    {
        spadBlk = cjAniSPADB_CubicVec3PSeg;
    }
    else if(channel == cjAniEvalChan_Rot)
    {
        spadBlk = cjAniSPADB_SlerpRotSeg;
    }
    else
    {
        cjAssert(channel == cjAniEvalChan_Scl);
        spadBlk = cjAniSPADB_CubicVec3SSeg;
    }

    return cjTrue;
}

cjBool cjConstTMEvaluator::InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, 
    cjBSplineBasisF3Dat*, cjBool, cjRefObjNameHash*, cjAniPoseBufHandle pbHandle) const
{
    cjAssert(channel < cjAniEvalChan_PRS_Max);
    cjAssert(evalSPDat);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(!IsChannelItemInvalid(channel));
    cjAssert(IsChannelItemPosed(channel));

    evalSPDat->SetSegTimeRange(0.0f, CJ_FLT_MAX);
    cjAssert(evalSPDat->GetFillFunc() == cjNULL);

    if(channel == cjAniEvalChan_Pos)
    {
        cjAniSPad::CubicVec3Seg* cubicVec3Seg = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();
        cjAssert(cubicVec3Seg);

		cubicVec3Seg->m_hdr.m_lod = evalSPDat->GetLOD();

        if(IsReferenced())
        {
			cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
            cubicVec3Seg->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
        }
        else
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Vec3P);
			cubicVec3Seg->m_hdr.m_outputPBIndex = (cjWORD)pbHandle.GetChannelItem();
        }

        cubicVec3Seg->m_stTime			= 0.0f;
		cubicVec3Seg->m_invDTime		= 0.0f;
        cubicVec3Seg->m_val0			= m_value.GetPos();
        cubicVec3Seg->m_outTangent0		= cjVec3::ZERO;
        cubicVec3Seg->m_a0				= cjVec3::ZERO;
        cubicVec3Seg->m_b0				= cjVec3::ZERO;

    }
    else if(channel == cjAniEvalChan_Rot)
    {
        cjAniSPad::SlerpRotSeg* slerpRotSeg = (cjAniSPad::SlerpRotSeg*)evalSPDat->GetSegDat();

        cjAssert(slerpRotSeg);
        slerpRotSeg->m_hdr.m_lod = evalSPDat->GetLOD();

        if(IsReferenced())
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
            slerpRotSeg->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
        }
        else
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Rot);
			slerpRotSeg->m_hdr.m_outputPBIndex = (cjWORD)pbHandle.GetChannelItem();
        }

        slerpRotSeg->m_stTime		= 0.0f;
		slerpRotSeg->m_invDTime		= 0.0f;
        slerpRotSeg->m_quat0		= m_value.GetRotate();
        slerpRotSeg->m_quat1		= m_value.GetRotate();
    }
    else
    {
        cjAssert(channel == cjAniEvalChan_Scl);

        cjAniSPad::CubicVec3Seg* cubicVec3Seg = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();
        cjAssert(cubicVec3Seg);

        cubicVec3Seg->m_hdr.m_lod = evalSPDat->GetLOD();

        if(IsReferenced())
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
			cubicVec3Seg->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
        }
        else
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Vec3S);
            cubicVec3Seg->m_hdr.m_outputPBIndex = (cjWORD)pbHandle.GetChannelItem();
        }

        cubicVec3Seg->m_stTime			= 0.0f;
        cubicVec3Seg->m_invDTime		= 0.0f;
        cubicVec3Seg->m_val0			= m_value.GetScale();
        cubicVec3Seg->m_outTangent0		= cjVec3::ZERO;
        cubicVec3Seg->m_a0				= cjVec3::ZERO;
        cubicVec3Seg->m_b0				= cjVec3::ZERO;
    }

    return cjTrue;
}

void cjConstTMEvaluator::UpdateChannelItem()
{
    if(m_value.IsPosValid())
        m_channelItem[cjAniEvalChan_Pos] = cjAniEvalChanItem_Vec3P | cjAniEvalChanItemMask_Constant;
    else
        m_channelItem[cjAniEvalChan_Pos] = cjAniEvalChanItem_NoData;

    if(m_value.IsRotateValid())
        m_channelItem[cjAniEvalChan_Rot] = cjAniEvalChanItem_Rot | cjAniEvalChanItemMask_Constant;
    else
        m_channelItem[cjAniEvalChan_Rot] = cjAniEvalChanItem_NoData;

    if(m_value.IsScaleValid())
        m_channelItem[cjAniEvalChan_Scl] = cjAniEvalChanItem_Vec3S | cjAniEvalChanItemMask_Constant;
    else
        m_channelItem[cjAniEvalChan_Scl] = cjAniEvalChanItem_NoData;
}


//
// cjConstBoolEvaluator
//

CJSER_IMPL_CLASS(cjConstBoolEvaluator, cjAniEvaluator)

CJSER_IMPL_SERIALIZE_BEGIN(cjConstBoolEvaluator)

	if(ser.IsLoading() || ser.IsSaving())
	{
		ser << m_value;
	}

CJSER_IMPL_SERIALIZE_END(cjConstBoolEvaluator)

CJSER_IMPL_CLONE(cjConstBoolEvaluator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjConstBoolEvaluator)
	d->m_value = m_value;
CJSER_IMPL_CLONE_PROPCOPY_END(cjConstBoolEvaluator)


cjConstBoolEvaluator::cjConstBoolEvaluator()
{
	m_value = CJ_FLOAT_INVALID;
}

cjConstBoolEvaluator::cjConstBoolEvaluator(cjFLOAT poseBool)
{
	m_value = poseBool;
    SetEvalChannelItem();
}

cjConstBoolEvaluator::~cjConstBoolEvaluator()
{
}

cjBool cjConstBoolEvaluator::GetChannelPosedValue(cjUINT32 channel, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(rst);

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChanneItemlInvalid(channel));

        *(cjFLOAT*)rst = m_value;
        return cjTrue;
    }

    cjAssert(IsChanneItemlInvalid(channel));

    return cjFalse;
}

cjBool cjConstBoolEvaluator::UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChanneItemlInvalid(channel));

        cjAniSPad::CBoolSeg* constBoolSeg = (cjAniSPad::CBoolSeg*)evalSPDat->GetSegDat();
        cjAssert(constBoolSeg);

        *(cjFLOAT*)rst = constBoolSeg->m_val0;

        return cjTrue;
    }

    cjAssert(IsChanneItemlInvalid(channel));

    return cjFalse;
}

cjBool cjConstBoolEvaluator::GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, 
	cjUINT32& fillSiz, cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const
{
    cjAssert(channel == cjAniEvalChan_Base);

    if(IsChanneItemlInvalid(channel) || (!forceAlwaysUpdate && !IsAlwaysUpdate()))
        return cjFalse; // channel is invalid or constant.. no scratch pad is need
	    
    fillSiz = 0;
    isSharedFillDat = cjFalse;
    spadBlk = cjAniSPADB_CBoolSeg;
    bsBasisDat = cjNULL;

    return cjTrue;
}

cjBool cjConstBoolEvaluator::InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, 
	cjBSplineBasisF3Dat* bsBasisDat, cjBool initSharedDat, cjRefObjNameHash* nodeHash, 
	cjAniPoseBufHandle pbHandle) const
{
    cjAssert(channel == cjAniEvalChan_Base);

	cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(!IsChanneItemlInvalid(channel));

    evalSPDat->SetSegTimeRange(0.0f, CJ_FLT_INFINITY);
    cjAssert(evalSPDat->GetFillFunc() == cjNULL);

    cjAniSPad::CBoolSeg* constBoolSeg = (cjAniSPad::CBoolSeg*)evalSPDat->GetSegDat();

    cjAssert(constBoolSeg);
    constBoolSeg->m_hdr.m_lod = evalSPDat->GetLOD();

    if(IsReferenced())
    {
        cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
        constBoolSeg->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
    }
    else
    {
        cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Bool);
        constBoolSeg->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
    }

    constBoolSeg->m_val0 = m_value;

    return cjTrue;
}

void cjConstBoolEvaluator::SetEvalChannelItem()
{
    if(m_value != CJ_FLOAT_INVALID)
        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_Bool | cjAniEvalChanItemMask_Constant;
    else
        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_NoData;
}


//
// cjConstFloatEvaluator
//

CJSER_IMPL_CLASS(cjConstFloatEvaluator, cjAniEvaluator)

CJSER_IMPL_SERIALIZE_BEGIN(cjConstFloatEvaluator)

	if(ser.IsLoading() || ser.IsSaving())
	{
		ser << m_value;
	}

CJSER_IMPL_SERIALIZE_END(cjConstFloatEvaluator)

CJSER_IMPL_CLONE(cjConstFloatEvaluator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjConstFloatEvaluator)
	d->m_value = m_value;
CJSER_IMPL_CLONE_PROPCOPY_END(cjConstFloatEvaluator)


cjConstFloatEvaluator::cjConstFloatEvaluator() : m_value(CJ_FLT_INVALID)
{
	m_value = CJ_FLT_INVALID;
	// SetEvalChannelTypes();
}

cjConstFloatEvaluator::cjConstFloatEvaluator(cjFLOAT poseValue)
{
    m_value = poseValue;
	SetEvalChannelTypes();
}

cjConstFloatEvaluator::~cjConstFloatEvaluator()
{
}

cjBool cjConstFloatEvaluator::GetChannelPosedValue(cjUINT32 channel, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChannelItemInvalid(channel));
        *(cjFLOAT*)rst = m_value;

        return cjTrue;
    }

    cjAssert(IsChannelItemInvalid(channel));
    return cjFalse;
}

cjBool cjConstFloatEvaluator::UpdateChannel(cjFLOAT, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(evalSPDat);
    cjAssert(evalSPDat->GetEvaluator() == this);
	cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(rst);

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChannelItemInvalid(channel));

		cjAniSPad::CubicFloatSeg* cubicFloatSeg = (cjAniSPad::CubicFloatSeg*)evalSPDat->GetSegDat();
        cjAssert(cubicFloatSeg);

		*(cjFLOAT*)rst = cubicFloatSeg->m_value0;

        cjAssert(cubicFloatSeg->m_outTangent0 == 0.0f);
        cjAssert(cubicFloatSeg->m_a0 == 0.0f);
        cjAssert(cubicFloatSeg->m_b0 == 0.0f);

        return cjTrue;
    }

    cjAssert(IsChannelItemInvalid(channel));

    return cjFalse;
}

cjBool cjConstFloatEvaluator::GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, 
	cjUINT32& fillSiz, cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const
{
    cjAssert(channel == cjAniEvalChan_Base);

    if(IsChannelItemInvalid(channel) || (!forceAlwaysUpdate && !IsAlwaysUpdate()))
        return cjFalse; // no need spad

    fillSiz = 0;
    isSharedFillDat = cjFalse;
    spadBlk = cjAniSPADB_CubicFloatSeg;
    bsBasisDat = 0;

    return cjTrue;
}

cjBool cjConstFloatEvaluator::InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, 
	cjBool initSharedDat, cjRefObjNameHash* nodeHash, cjAniPoseBufHandle pbHandle) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(!IsChannelItemInvalid(channel));

    evalSPDat->SetSegTimeRange(0.0f, CJ_FLT_INFINITY);
    cjAssert(evalSPDat->GetFillFunc() == cjNULL);

    cjAniSPad::CubicFloatSeg* cubicFloatSeg = (cjAniSPad::CubicFloatSeg*)evalSPDat->GetSegDat();
    cjAssert(cubicFloatSeg);

	cubicFloatSeg->m_hdr.m_lod = evalSPDat->GetLOD();

    if(IsReferenced())
    {
		cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
		cubicFloatSeg->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
    }
    else
    {
        cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Float);
		cubicFloatSeg->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
    }

    cubicFloatSeg->m_stTime			= 0.0f;
	cubicFloatSeg->m_invDTime		= 0.0f;
    cubicFloatSeg->m_value0			= m_value;
    cubicFloatSeg->m_outTangent0	= 0.0f;
    cubicFloatSeg->m_a0				= 0.0f;
    cubicFloatSeg->m_b0				= 0.0f;

    return cjTrue;
}

void cjConstFloatEvaluator::SetEvalChannelTypes()
{
    if(m_value != CJ_FLT_INVALID)
        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_Float | cjAniEvalChanItemMask_Constant;
    else
        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_NoData;
}


//
// cjConstVec3SEvaluator
//

CJSER_IMPL_CLASS(cjConstVec3SEvaluator, cjAniEvaluator);

CJSER_IMPL_SERIALIZE_BEGIN(cjConstVec3SEvaluator)

	if(ser.IsLoading() || ser.IsSaving())
	{
		ser << m_value;
	}

CJSER_IMPL_SERIALIZE_END(cjConstVec3SEvaluator)

CJSER_IMPL_CLONE(cjConstVec3SEvaluator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjConstVec3SEvaluator)
	d->m_value = m_value;
CJSER_IMPL_CLONE_PROPCOPY_END(cjConstVec3SEvaluator)


cjConstVec3SEvaluator::cjConstVec3SEvaluator()
{
	m_value = cjVec3::INVALID;
}

cjConstVec3SEvaluator::cjConstVec3SEvaluator(const cjVec3& poseValue)
{
    m_value = poseValue;
	SetEvalChannelTypes();
}

cjConstVec3SEvaluator::~cjConstVec3SEvaluator()
{
}

cjBool cjConstVec3SEvaluator::GetChannelPosedValue(cjUINT32 channel, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(rst);

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChannelItemInvalid(channel));
        *(cjVec3*)rst = m_value;
        return cjTrue;
    }

    cjAssert(IsChannelItemInvalid(channel));

    return cjFalse;
}

cjBool cjConstVec3SEvaluator::UpdateChannel(cjFLOAT, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChannelItemInvalid(channel));

        cjAniSPad::CubicVec3Seg* cubicVec3Seg = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();
        
		cjAssert(cubicVec3Seg);

		*(cjVec3*)rst = cubicVec3Seg->m_val0;

        cjAssert(cubicVec3Seg->m_outTangent0 == cjVec3::ZERO);
        cjAssert(cubicVec3Seg->m_a0 == cjVec3::ZERO);
        cjAssert(cubicVec3Seg->m_b0 == cjVec3::ZERO);

        return cjTrue;
    }

    cjAssert(IsChannelItemInvalid(channel));

    return cjFalse;
}

cjBool cjConstVec3SEvaluator::GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, 
	cjUINT32& fillSiz, cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const
{
    cjAssert(channel == cjAniEvalChan_Base);

    if(IsChannelItemInvalid(channel) || (!forceAlwaysUpdate && !IsAlwaysUpdate()))
        return cjFalse;

    fillSiz = 0;
    isSharedFillDat = cjFalse;
    spadBlk = cjAniSPADB_CubicVec3SSeg;
    bsBasisDat = 0;

    return cjTrue;
}

cjBool cjConstVec3SEvaluator::InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat,
	cjBool initSharedDat, cjRefObjNameHash* nodeHash, cjAniPoseBufHandle pbHandle) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(!IsChannelItemInvalid(channel));

    evalSPDat->SetSegTimeRange(0.0f, CJ_FLT_INFINITY);
    cjAssert(evalSPDat->GetFillFunc() == cjNULL);

    cjAniSPad::CubicVec3Seg* cubicVec3Seg = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();
    cjAssert(cubicVec3Seg);

    cubicVec3Seg->m_hdr.m_lod = evalSPDat->GetLOD();

    if(IsReferenced())
    {
        cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
        cubicVec3Seg->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
    }
    else
    {
        cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Vec3S);
        cubicVec3Seg->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
    }

    cubicVec3Seg->m_stTime		= 0.0f;
    cubicVec3Seg->m_invDTime	= 0.0f;
	cubicVec3Seg->m_val0		= m_value;
    cubicVec3Seg->m_outTangent0 = cjVec3::ZERO;
    cubicVec3Seg->m_a0			= cjVec3::ZERO;
    cubicVec3Seg->m_b0			= cjVec3::ZERO;

    return cjTrue;
}

void cjConstVec3SEvaluator::SetEvalChannelTypes()
{
	if(m_value != cjVec3::INVALID)
        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_Vec3S | cjAniEvalChanItemMask_Constant;
    else
        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_NoData;
}


//
// cjConstVec3PEvaluator
//

CJSER_IMPL_CLASS(cjConstVec3PEvaluator, cjAniEvaluator);

CJSER_IMPL_SERIALIZE_BEGIN(cjConstVec3PEvaluator)

	if(ser.IsLoading() || ser.IsSaving())
	{
		ser << m_value;
	}

CJSER_IMPL_SERIALIZE_END(cjConstVec3PEvaluator)

CJSER_IMPL_CLONE(cjConstVec3PEvaluator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjConstVec3PEvaluator)
	d->m_value = m_value;
CJSER_IMPL_CLONE_PROPCOPY_END(cjConstVec3PEvaluator)


cjConstVec3PEvaluator::cjConstVec3PEvaluator()
{
	m_value = cjVec3::INVALID;
}

cjConstVec3PEvaluator::cjConstVec3PEvaluator(const cjVec3& poseValue)
{
    m_value = poseValue;
	SetEvalChannelTypes();
}

cjConstVec3PEvaluator::~cjConstVec3PEvaluator()
{
}

cjBool cjConstVec3PEvaluator::GetChannelPosedValue(cjUINT32 channel, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(rst);

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChannelItemInvalid(channel));
        *(cjVec3*)rst = m_value;
        return cjTrue;
    }

    cjAssert(IsChannelItemInvalid(channel));

    return cjFalse;
}

cjBool cjConstVec3PEvaluator::UpdateChannel(cjFLOAT, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChannelItemInvalid(channel));

        cjAniSPad::CubicVec3Seg* cubicVec3Seg = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();
        
		cjAssert(cubicVec3Seg);

		*(cjVec3*)rst = cubicVec3Seg->m_val0;

        cjAssert(cubicVec3Seg->m_outTangent0 == cjVec3::ZERO);
        cjAssert(cubicVec3Seg->m_a0 == cjVec3::ZERO);
        cjAssert(cubicVec3Seg->m_b0 == cjVec3::ZERO);

        return cjTrue;
    }

    cjAssert(IsChannelItemInvalid(channel));

    return cjFalse;
}

cjBool cjConstVec3PEvaluator::GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, 
	cjUINT32& fillSiz, cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const
{
    cjAssert(channel == cjAniEvalChan_Base);

    if(IsChannelItemInvalid(channel) || (!forceAlwaysUpdate && !IsAlwaysUpdate()))
        return cjFalse;

    fillSiz = 0;
    isSharedFillDat = cjFalse;
    spadBlk = cjAniSPADB_CubicVec3PSeg; // pos
    bsBasisDat = 0;

    return cjTrue;
}

cjBool cjConstVec3PEvaluator::InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat,
	cjBool initSharedDat, cjRefObjNameHash* nodeHash, cjAniPoseBufHandle pbHandle) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(!IsChannelItemInvalid(channel));

    evalSPDat->SetSegTimeRange(0.0f, CJ_FLT_INFINITY);
    cjAssert(evalSPDat->GetFillFunc() == cjNULL);

    cjAniSPad::CubicVec3Seg* cubicVec3Seg = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();
    cjAssert(cubicVec3Seg);

    cubicVec3Seg->m_hdr.m_lod = evalSPDat->GetLOD();

    if(IsReferenced())
    {
        cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
        cubicVec3Seg->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
    }
    else
    {
        cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Vec3P); // pos
        cubicVec3Seg->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
    }

    cubicVec3Seg->m_stTime		= 0.0f;
    cubicVec3Seg->m_invDTime	= 0.0f;
	cubicVec3Seg->m_val0		= m_value;
    cubicVec3Seg->m_outTangent0 = cjVec3::ZERO;
    cubicVec3Seg->m_a0			= cjVec3::ZERO;
    cubicVec3Seg->m_b0			= cjVec3::ZERO;

    return cjTrue;
}

void cjConstVec3PEvaluator::SetEvalChannelTypes()
{
	if(m_value != cjVec3::INVALID)
        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_Vec3P | cjAniEvalChanItemMask_Constant;
    else
        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_NoData;
}


//
// cjConstQuatEvaluator
//

CJSER_IMPL_CLASS(cjConstQuatEvaluator, cjAniEvaluator);

CJSER_IMPL_SERIALIZE_BEGIN(cjConstQuatEvaluator)

	if(ser.IsLoading() || ser.IsSaving())
	{
		ser << m_value;
	}

CJSER_IMPL_SERIALIZE_END(cjConstQuatEvaluator)

CJSER_IMPL_CLONE(cjConstQuatEvaluator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjConstQuatEvaluator)
	d->m_value = m_value;
CJSER_IMPL_CLONE_PROPCOPY_END(cjConstQuatEvaluator)

cjConstQuatEvaluator::cjConstQuatEvaluator()
{
	m_value = cjQuat::INVALID;
}

cjConstQuatEvaluator::cjConstQuatEvaluator(const cjQuat& poseValue)
{
	m_value = poseValue;
    SetEvalChannelTypes();
}

cjConstQuatEvaluator::~cjConstQuatEvaluator()
{
}

cjBool cjConstQuatEvaluator::GetChannelPosedValue(cjUINT32 channel, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChannelItemInvalid(channel));
        *(cjQuat*)rst = m_value;

        return cjTrue;
    }

    cjAssert(IsChannelItemInvalid(channel));

    return cjFalse;
}

cjBool cjConstQuatEvaluator::UpdateChannel(cjFLOAT, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChannelItemInvalid(channel));

        cjAniSPad::SlerpRotSeg* slerpRotSeg = (cjAniSPad::SlerpRotSeg*)evalSPDat->GetSegDat();
		*(cjQuat*)rst = slerpRotSeg->m_quat0;

        cjAssert(slerpRotSeg->m_quat1 == slerpRotSeg->m_quat0);

        return cjTrue;
    }

    cjAssert(IsChannelItemInvalid(channel));

    return cjFalse;
}

cjBool cjConstQuatEvaluator::GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, 
	cjUINT32& fillSiz, cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const
{
    cjAssert(channel == cjAniEvalChan_Base);

    if(IsChannelItemInvalid(channel) || (!forceAlwaysUpdate && !IsAlwaysUpdate()))
        return cjFalse;

    fillSiz = 0;
    isSharedFillDat = cjFalse;
    spadBlk = cjAniSPADB_SlerpRotSeg;
    bsBasisDat = 0;

    return cjTrue;
}

cjBool cjConstQuatEvaluator::InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, 
	cjBool initSharedDat, cjRefObjNameHash* nodeHash, cjAniPoseBufHandle pbHandle) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(!IsChannelItemInvalid(channel));

    evalSPDat->SetSegTimeRange(0.0f, CJ_FLT_INFINITY);
    cjAssert(evalSPDat->GetFillFunc() == cjNULL);

    cjAniSPad::SlerpRotSeg* slerpRotSeg = (cjAniSPad::SlerpRotSeg*)evalSPDat->GetSegDat();
    slerpRotSeg->m_hdr.m_lod = evalSPDat->GetLOD();

    if(IsReferenced())
    {
        cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
        slerpRotSeg->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
    }
    else
    {
        cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Rot);
        slerpRotSeg->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
    }

    slerpRotSeg->m_stTime = 0.0f;
    slerpRotSeg->m_invDTime = 0.0f;
    slerpRotSeg->m_quat0 = m_value;
    slerpRotSeg->m_quat1 = m_value;

    return cjTrue;
}

void cjConstQuatEvaluator::SetEvalChannelTypes()
{
    if(m_value != cjQuat::INVALID)
        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_Rot | cjAniEvalChanItemMask_Constant;
    else
        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_NoData;
}


//
// cjConstColorEvaluator
//

CJSER_IMPL_CLASS(cjConstColorEvaluator, cjAniEvaluator);

CJSER_IMPL_SERIALIZE_BEGIN(cjConstColorEvaluator)

	if(ser.IsLoading() || ser.IsSaving())
	{
		ser << m_value;
	}

CJSER_IMPL_SERIALIZE_END(cjConstColorEvaluator)

CJSER_IMPL_CLONE(cjConstColorEvaluator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjConstColorEvaluator)
	d->m_value = m_value;
CJSER_IMPL_CLONE_PROPCOPY_END(cjConstColorEvaluator)


cjConstColorEvaluator::cjConstColorEvaluator()
{
	m_value = cjColorF::INVALID;
}

cjConstColorEvaluator::cjConstColorEvaluator(const cjColorF& poseValue)    
{
	m_value = poseValue;
    SetEvalChannelTypes();
}

cjConstColorEvaluator::~cjConstColorEvaluator()
{
}

cjBool cjConstColorEvaluator::GetChannelPosedValue(cjUINT32 channel, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChannelItemInvalid(channel));
        *(cjColorF*)rst = m_value;

        return cjTrue;
    }

    cjAssert(IsChannelItemInvalid(channel));

    return cjFalse;
}

cjBool cjConstColorEvaluator::UpdateChannel(cjFLOAT, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChannelItemInvalid(channel));
        cjAniSPad::LColorSeg* linearColorSeg = (cjAniSPad::LColorSeg*)evalSPDat->GetSegDat();

		*(cjColorF*)rst = linearColorSeg->m_value0;
        cjAssert(linearColorSeg->m_outTangent0 == cjColorF(0, 0, 0, 0));

        return cjTrue;
    }

    cjAssert(IsChannelItemInvalid(channel));

    return cjFalse;
}

cjBool cjConstColorEvaluator::GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, 
	cjUINT32& fillSiz, cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const
{
    cjAssert(channel == cjAniEvalChan_Base);

    if(IsChannelItemInvalid(channel) || (!forceAlwaysUpdate && !IsAlwaysUpdate()))
        return cjFalse;

    fillSiz = 0;
    isSharedFillDat = cjFalse;
    spadBlk = cjAniSPADB_LinearColorSeg;
    bsBasisDat = 0;

    return cjTrue;
}

cjBool cjConstColorEvaluator::InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, 
	cjBool initSharedDat, cjRefObjNameHash* nodeHash, cjAniPoseBufHandle pbHandle) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(!IsChannelItemInvalid(channel));

    evalSPDat->SetSegTimeRange(0.0f, CJ_FLT_INFINITY);
    cjAssert(evalSPDat->GetFillFunc() == cjNULL);

    cjAniSPad::LColorSeg* linearColorSeg = (cjAniSPad::LColorSeg*)evalSPDat->GetSegDat();
    linearColorSeg->m_hdr.m_lod = evalSPDat->GetLOD();

    if(IsReferenced())
    {
        cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
        linearColorSeg->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
    }
    else
    {
        cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Color);
        linearColorSeg->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
    }

    linearColorSeg->m_stTime = 0.0f;
	linearColorSeg->m_invDTime = 0.0f;
    linearColorSeg->m_value0 = m_value;
    linearColorSeg->m_outTangent0 = cjColorF(0, 0, 0, 0);

    return cjTrue;
}

void cjConstColorEvaluator::SetEvalChannelTypes()
{
	if(m_value != cjColorF::INVALID)
        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_Color | cjAniEvalChanItemMask_Constant;
    else
        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_NoData;
}


//
// cjKeyBasedEvaluator
//

CJSER_IMPL_CLASS(cjKeyBasedEvaluator, cjAniEvaluator)

CJSER_IMPL_SERIALIZE_BEGIN(cjKeyBasedEvaluator)
CJSER_IMPL_SERIALIZE_END(cjKeyBasedEvaluator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjKeyBasedEvaluator)
CJSER_IMPL_CLONE_PROPCOPY_END(cjKeyBasedEvaluator)


cjKeyBasedEvaluator::cjKeyBasedEvaluator()
{
}

cjKeyBasedEvaluator::~cjKeyBasedEvaluator()
{
}

void cjKeyBasedEvaluator::GetValidTimeRange(cjFLOAT& stTime, cjFLOAT& enTime) const
{
    stTime = CJ_FLT_INFINITY;
    enTime = CJ_FLT_INVALID;

    for(cjWORD i=0; i<GetKeyChannelCount(); i++)
    {
        cjAniKey* beginKey = GetKey(0, i);
        cjAniKey* endKey = GetKey(GetKeyCount(i) - 1, i);

        if(beginKey && endKey)
        {
            if(beginKey->GetTime() < stTime)
                stTime = beginKey->GetTime();

            if(endKey->GetTime() > enTime)
                enTime = endKey->GetTime();
        }
    }

    if(stTime == CJ_FLT_INFINITY && enTime == CJ_FLT_INVALID)
    {
        stTime = 0.0f;
        enTime = 0.0f;
    }
}

cjUINT32 cjKeyBasedEvaluator::GetAllocSiz(cjUINT32 channel) const
{
    cjAssert(channel < GetKeyChannelCount());

    if(GetKeyType(channel) == cjAniKey::EKT_EULERKEY)
    {
        cjAniKeyEulerRot* eulerRotKey = (cjAniKeyEulerRot*)GetKeyAry(channel);
        cjUINT32 keySiz = 0;

        if(eulerRotKey)
        {
            keySiz = sizeof(cjAniKeyEulerRot);

            for(cjBYTE c=0; c<3; c++)
            {
                cjUINT32 curKeyCount = eulerRotKey->GetKeyCount(c);
                cjUINT32 curKeySiz = eulerRotKey->GetKeySiz(c);
                keySiz += curKeyCount * curKeySiz;
            }           
        }
        return keySiz;
    }

    return GetKeyCount(channel) * GetKeySiz(channel);
}

cjAniKey* cjKeyBasedEvaluator::GetKey(cjUINT32 index, cjUINT32 channel) const
{
    cjAssert(channel < GetKeyChannelCount());
    return GetKeyAry(channel)->GetKey(index, GetKeySiz(channel));
}

cjAniKey* cjKeyBasedEvaluator::GetKeysWithChannel(cjUINT32& rstKeyCount, cjAniKey::EKEY_TYPE& rstKeyType, 
	cjBYTE& rstKeySiz, cjUINT32 channel) const
{
    cjAssert(channel < GetKeyChannelCount());

    rstKeyCount = GetKeyCount(channel);
    rstKeyType = GetKeyType(channel);
    rstKeySiz = GetKeySiz(channel);

    return GetKeyAry(channel);
}

//
// cjKeyBasedEvaluator
// - cjColorEvaluator
//

CJSER_IMPL_CLASS(cjColorEvaluator, cjKeyBasedEvaluator);

CJSER_IMPL_SERIALIZE_BEGIN(cjColorEvaluator)
	
	if(ser.IsQuerySerObjAndStr())
	{
		if(m_spColorDat)
			(*m_spColorDat) >> ser;

		return;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		ser.SerializeSerObjPtr(&m_spColorDat);
	}
	
CJSER_IMPL_SERIALIZE_END(cjColorEvaluator)

CJSER_IMPL_SERIALIZE_POST_BEGIN(cjColorEvaluator)

	if(ser.IsLoading())
	{
		ser.PostSerializeSerObjPtr(m_spColorDat);
	}

CJSER_IMPL_SERIALIZE_POST_END(cjColorEvaluator)

CJSER_IMPL_CLONE(cjColorEvaluator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjColorEvaluator)
	d->m_spColorDat = m_spColorDat;
CJSER_IMPL_CLONE_PROPCOPY_END(cjColorEvaluator)


cjColorEvaluator::cjColorEvaluator()
{
}

cjColorEvaluator::cjColorEvaluator(cjAniColorDat* colorDat)
{
	m_spColorDat = colorDat;
    SetEvalChannelTypes();
}

cjColorEvaluator::~cjColorEvaluator()
{
}

cjBool cjColorEvaluator::GetChannelPosedValue(cjUINT32 channel, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChannelItemInvalid(channel));
        cjAssert(m_spColorDat);

        cjUINT32 keyCount; cjAniKey::EKEY_TYPE keyType; cjBYTE keySiz;
        cjAniKeyColor* keys = m_spColorDat->GetKeys(keyCount, keyType, keySiz);
        cjAssert(keyCount == 1);

        *(cjColorF*)rst = keys->GetKey(0, keySiz)->GetValue();

        return cjTrue;
    }

    return cjFalse;
}

cjBool cjColorEvaluator::UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(rst);

    if(IsChannelItemInvalid(channel))
        return cjFalse;

    if(!evalSPDat->IsSegDatValid(etime))
    {
        cjAniEvalSPadFillFunc pfFill = evalSPDat->GetFillFunc();

        if(!pfFill || !(*pfFill)(etime, evalSPDat))
            return cjFalse;
    }

    cjAniSPad::LColorSeg* linearColorSeg = (cjAniSPad::LColorSeg*)evalSPDat->GetSegDat();
    cjFLOAT ratio = (etime - linearColorSeg->m_stTime) * linearColorSeg->m_invDTime;

    *(cjColorF*)rst = linearColorSeg->m_value0 + linearColorSeg->m_outTangent0 * ratio;

    return cjTrue;
}

void cjColorEvaluator::CacheKeys(cjFLOAT stTime, cjFLOAT enTime)
{
    if(m_spColorDat)
        m_spColorDat->CacheKeys(stTime, enTime);
}

cjAniEvaluator* cjColorEvaluator::GetEvaluator(cjFLOAT stTime, cjFLOAT enTime)
{
    cjColorEvaluator* eval = (cjColorEvaluator*)cjKeyBasedEvaluator::GetEvaluator(stTime, enTime);

    if(m_spColorDat)
    {
        cjAniColorDatRPtr newColorDat = m_spColorDat->GetDat(stTime, enTime);
        eval->SetColorDat(newColorDat);
    }

    return eval;
}

cjBool cjColorEvaluator::GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, 
	cjUINT32& fillSiz, cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const
{
    cjAssert(channel == cjAniEvalChan_Base);

    cjBool isPosed = IsChannelItemPosed(channel);

    if(IsChannelItemInvalid(channel) || (isPosed && !forceAlwaysUpdate && !IsAlwaysUpdate()))
        return cjFalse;

    fillSiz = isPosed ? 0 : sizeof(cjAniSPad::LColorFill);
    isSharedFillDat = cjFalse;
    spadBlk = cjAniSPADB_LinearColorSeg;
    bsBasisDat = 0;

    return cjTrue;
}

cjBool cjColorEvaluator::InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, 
	cjBool initSharedDat, cjRefObjNameHash* nodeHash, cjAniPoseBufHandle pbHandle) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(evalSPDat);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(!IsChannelItemInvalid(channel));
    cjAssert(m_spColorDat);

    cjAniSPad::LColorSeg* linearColorSeg = (cjAniSPad::LColorSeg*)evalSPDat->GetSegDat();
    linearColorSeg->m_hdr.m_lod = evalSPDat->GetLOD();

    if(IsReferenced())
    {
        cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
        linearColorSeg->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
    }
    else
    {
        cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Color);
        linearColorSeg->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
    }

    cjUINT32 keyCount; cjAniKey::EKEY_TYPE keyType; cjBYTE keySiz;
    cjAniKeyColor* keys = m_spColorDat->GetKeys(keyCount, keyType, keySiz);

    cjAssert(keys);
    cjAssert(keyCount > 0);

    cjBool isPosed = IsChannelItemPosed(channel);
    if(isPosed)
    {
        evalSPDat->SetSegTimeRange(0.0f, CJ_FLT_INFINITY);
        cjAssert(evalSPDat->GetFillFunc() == cjNULL);

        linearColorSeg->m_stTime = 0.0f;
        linearColorSeg->m_invDTime = 0.0f;
        linearColorSeg->m_value0 = keys->GetKey(0, keySiz)->GetValue();
        linearColorSeg->m_outTangent0 = cjColorF(0, 0, 0, 0);
    }
    else
    {
        if(keyType == cjAniKey::EKT_STEPKEY)
            evalSPDat->SetStepFunction(cjTrue);

		cjAssert(evalSPDat->GetSegMinTime() == CJ_FLT_INFINITY);
        cjAssert(evalSPDat->GetSegMaxTime() == CJ_FLT_INVALID);

        evalSPDat->SetFillFunc(&LinearColorFillFunc);

        cjAniSPad::LColorFill* fillDat = (cjAniSPad::LColorFill*)evalSPDat->GetFillDat();
        cjAssert(fillDat);

        fillDat->m_lastIndex = 0;
        fillDat->m_keys = keys;
        fillDat->m_keyCount = keyCount;
        fillDat->m_keyType = keyType;
        fillDat->m_keySiz = keySiz;
    }

    return cjTrue;
}

void cjColorEvaluator::Collapse()
{
    if(m_spColorDat)
    {
        cjUINT32 keyCount; cjAniKey::EKEY_TYPE keyType; cjBYTE keySiz;
        cjAniKeyColor* keys = m_spColorDat->GetKeys(keyCount, keyType, keySiz);

        if(keyCount == 0)
        {
            m_spColorDat = cjNULL;
        }
        else
        {
            cjAniKey::PFIsPosed pfIsPosed = cjAniKeyColor::GetIsPosedKeyPF(keyType);

			if(pfIsPosed(keys, keyCount))
            {
                cjAniKeyColor* linColorKey = (cjAniKeyColor*)new cjAniKeyLinColor[1];
                linColorKey[0].SetTime(0.0f);

                cjColorF v = keys->GetKey(0, keySiz)->GetValue();
                linColorKey[0].SetValue(v);                    

                m_spColorDat->ReplaceKeys(linColorKey, 1, cjAniKey::EKT_LINKEY);
            }
        }
    }

    SetEvalChannelTypes();
}

cjUINT32 cjColorEvaluator::GetKeyCount(cjUINT32 channel) const
{
    if(!m_spColorDat)
        return 0;
    else
        return m_spColorDat->GetKeyCount();
}

cjAniKey::EKEY_TYPE cjColorEvaluator::GetKeyType(cjUINT32 channel) const
{
    if(!m_spColorDat)
        return cjAniKey::EKT_NOINTERP;

    cjUINT32 keyCount; cjAniKeyColor::EKEY_TYPE keyType; cjBYTE keySiz;
    m_spColorDat->GetKeys(keyCount, keyType, keySiz);

    return keyType; 
}

cjAniKey* cjColorEvaluator::GetKeyAry(cjUINT32 channel) const 
{
    if(!m_spColorDat)
        return cjNULL;

    cjUINT32 keyCount; cjAniKeyColor::EKEY_TYPE keyType; cjBYTE keySiz;
    cjAniKeyColor* keys= m_spColorDat->GetKeys(keyCount, keyType, keySiz);

    return keys; 
}

cjBYTE cjColorEvaluator::GetKeySiz(cjUINT32 channel) const 
{
    if(!m_spColorDat)
        return 0;

    cjUINT32 keyCount; cjAniKeyColor::EKEY_TYPE keyType; cjBYTE keySiz;
    m_spColorDat->GetKeys(keyCount, keyType, keySiz);

    return keySiz; 
}

cjAniKeyColor* cjColorEvaluator::GetKeys(cjUINT32& keyCount, cjAniKeyColor::EKEY_TYPE& keyType, cjBYTE& keySiz) const
{
    if(m_spColorDat)
        return m_spColorDat->GetKeys(keyCount, keyType, keySiz);

    keyCount = 0; keyType = cjAniKeyColor::EKT_NOINTERP; keySiz = 0;

    return cjNULL;
}

void cjColorEvaluator::ReplaceKeys(cjAniKeyColor* keys, cjUINT32 keyCount, cjAniKeyColor::EKEY_TYPE keyType)
{
    if(keyCount > 0)
    {
        if(!m_spColorDat)
            m_spColorDat = new cjAniColorDat;

        m_spColorDat->ReplaceKeys(keys, keyCount, keyType);
    }
    else if(m_spColorDat)
    {
        m_spColorDat->ReplaceKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
    }

    SetEvalChannelTypes();
}

void cjColorEvaluator::SetKeys(cjAniKeyColor* keys, cjUINT32 keyCount, cjAniKeyColor::EKEY_TYPE keyType)
{
    if(keyCount > 0)
    {
        if(!m_spColorDat)
            m_spColorDat = new cjAniColorDat;

        m_spColorDat->SetKeys(keys, keyCount, keyType);
    }
    else if(m_spColorDat)
    {
        m_spColorDat->SetKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
    }

    SetEvalChannelTypes();
}


cjBool cjColorEvaluator::LinearColorFillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat)
{
    cjAssert(evalSPDat);

    cjAniSPad::LColorFill* fillDat = (cjAniSPad::LColorFill*)evalSPDat->GetFillDat();

    cjAssert(fillDat);
    cjAssert(fillDat->m_keys);
    cjAssert(fillDat->m_keyCount > 0);
	    
    cjAniSPad::LColorSeg* linearColorSeg = (cjAniSPad::LColorSeg*)evalSPDat->GetSegDat();
    cjAssert(linearColorSeg);

    cjFLOAT minTime, maxTime;
    cjColorF a0, b0;

	cjAniKeyColor::GenCubicCoefs(etime, fillDat->m_keys, fillDat->m_keyType, fillDat->m_keyCount, 
        fillDat->m_lastIndex, fillDat->m_keySiz, minTime, maxTime, linearColorSeg->m_value0, 
        linearColorSeg->m_outTangent0, a0, b0);

    cjAssert(a0 == cjColorF(0, 0, 0, 0));
    cjAssert(b0 == cjColorF(0, 0, 0, 0));

    linearColorSeg->m_stTime = minTime;
    linearColorSeg->m_invDTime = (maxTime != minTime) ? 1.0f / (maxTime - minTime) : 0.0f;

    evalSPDat->SetSegTimeRange(minTime, maxTime);

    return cjTrue;
}

void cjColorEvaluator::SetEvalChannelTypes()
{
    if(m_spColorDat && m_spColorDat->GetKeyCount() > 0)
    {
        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_Color;

        if(m_spColorDat->GetKeyCount() == 1)
            m_channelItem[cjAniEvalChan_Base] |= cjAniEvalChanItemMask_Constant;
    }
    else
    {
        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_NoData;
    }
}


//
// cjTMEvaluator
//

CJSER_IMPL_CLASS(cjTMEvaluator, cjKeyBasedEvaluator)

CJSER_IMPL_SERIALIZE_BEGIN(cjTMEvaluator)
	
	if(ser.IsQuerySerObjAndStr())
	{
		if(m_spTMDat)
			(*m_spTMDat) >> ser;

		return;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		ser << m_value;
		ser.SerializeSerObjPtr(&m_spTMDat);
	}
	
CJSER_IMPL_SERIALIZE_END(cjTMEvaluator)

CJSER_IMPL_SERIALIZE_POST_BEGIN(cjTMEvaluator)

	if(ser.IsLoading())
	{
		ser.PostSerializeSerObjPtr(m_spTMDat);
	}

CJSER_IMPL_SERIALIZE_POST_END(cjTMEvaluator)

CJSER_IMPL_CLONE(cjTMEvaluator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjTMEvaluator)
	d->m_value = m_value;
	d->m_spTMDat = m_spTMDat;
CJSER_IMPL_CLONE_PROPCOPY_END(cjTMEvaluator)


cjTMEvaluator::cjTMEvaluator()
{
	m_spTMDat = cjNULL;
    m_channelItem[cjAniEvalChan_Flag] |= cjAniEvalFlagChanVal_Transform;
}

cjTMEvaluator::cjTMEvaluator(cjAniTMDat* tmDat)
{
	m_spTMDat = tmDat;
    m_channelItem[cjAniEvalChan_Flag] |= cjAniEvalFlagChanVal_Transform;

    SetEvalChannelTypes();
}

cjTMEvaluator::cjTMEvaluator(const cjQuatTM& poseValue)
{
	m_value = poseValue;
	m_spTMDat = cjNULL;
    m_channelItem[cjAniEvalChan_Flag] |= cjAniEvalFlagChanVal_Transform;

    SetEvalChannelTypes();
}

cjTMEvaluator::~cjTMEvaluator()
{
}

cjBool cjTMEvaluator::GetChannelPosedValue(cjUINT32 channel, void* rst) const
{
    cjAssert(channel < cjAniEvalChan_PRS_Max);
    cjAssert(rst);

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChannelItemInvalid(channel));

        if(channel == cjAniEvalChan_Scl)
        {
            *(cjVec3*)rst = m_value.GetScale();
        }
        else if(channel == cjAniEvalChan_Pos)
        {
            *(cjVec3*)rst = m_value.GetPos();			
        }
        else
        {
            cjAssert(channel == cjAniEvalChan_Rot);
            *(cjQuat*)rst = m_value.GetRotate();
        }

        return cjTrue;
    }

    return cjFalse;
}

cjBool cjTMEvaluator::UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const
{
    cjAssert(channel < cjAniEvalChan_PRS_Max);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);

    if(IsChannelItemInvalid(channel))
        return cjFalse;

    if(!evalSPDat->IsSegDatValid(etime))
    {
        cjAniEvalSPadFillFunc pfFill = evalSPDat->GetFillFunc();

        if(!pfFill || !(*pfFill)(etime, evalSPDat))
            return cjFalse;
    }

    if(channel == cjAniEvalChan_Pos)
    {
        cjAniSPad::CubicVec3Seg* cubicVec3Seg = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();
        cjAssert(cubicVec3Seg);

        cjFLOAT ratio = (etime - cubicVec3Seg->m_stTime) * cubicVec3Seg->m_invDTime;
        *(cjVec3*)rst = cubicVec3Seg->m_val0 + (cubicVec3Seg->m_outTangent0 + (cubicVec3Seg->m_a0 + cubicVec3Seg->m_b0 * ratio) * ratio) * ratio;
    }
    else if(channel == cjAniEvalChan_Rot)
    {        
        if(!evalSPDat->IsAlternateSPData()) // Require slerp or squad interpolation.
        {
            cjAniSPad::SlerpRotSeg* slerpRotSegDat = (cjAniSPad::SlerpRotSeg*)evalSPDat->GetSegDat();
			cjFLOAT ratio = (etime - slerpRotSegDat->m_stTime) * slerpRotSegDat->m_invDTime;

            if(ratio == 0.0f)
                *(cjQuat*)rst = slerpRotSegDat->m_quat0;
            else
                *(cjQuat*)rst = cjQuat::Slerp(ratio, slerpRotSegDat->m_quat0, slerpRotSegDat->m_quat1);
        }
        else
        {
            cjAniSPad::SquadRotSeg* squdRotSegDat = (cjAniSPad::SquadRotSeg*)evalSPDat->GetSegDat();
            cjAssert(squdRotSegDat);

            cjAniSPad::SlerpRotSeg* baseSlerpRotSegDat = &squdRotSegDat->m_baseDat;

            cjFLOAT ratio = (etime - baseSlerpRotSegDat->m_stTime) * baseSlerpRotSegDat->m_invDTime;
            *(cjQuat*)rst = cjQuat::Squad(ratio, baseSlerpRotSegDat->m_quat0, squdRotSegDat->m_outQuat0, 
				squdRotSegDat->m_inQuat1, baseSlerpRotSegDat->m_quat1);
        }
    }
    else
    {
        cjAssert(channel == cjAniEvalChan_Scl);
        cjAniSPad::CubicVec3Seg* cubicVec3SegDat = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();
        cjAssert(cubicVec3SegDat);

        cjFLOAT ratio = (etime - cubicVec3SegDat->m_stTime) * cubicVec3SegDat->m_invDTime;
        *(cjVec3*)rst = cubicVec3SegDat->m_val0 + (cubicVec3SegDat->m_outTangent0 + (cubicVec3SegDat->m_a0 + cubicVec3SegDat->m_b0 * ratio) * ratio) * ratio;
    }

    return cjTrue;
}

void cjTMEvaluator::GetValidTimeRange(cjFLOAT& stTime, cjFLOAT& enTime) const
{
    stTime = CJ_FLT_INFINITY;
    enTime = CJ_FLT_INVALID;

    cjBool keyFound = cjFalse; 
	cjUINT32 keyCount; cjBYTE keySiz;
		
	cjAniKeyVec3::EKEY_TYPE posKeyType;
    cjAniKeyVec3* posKeys = GetPosKeys(keyCount, posKeyType, keySiz);

    if(keyCount > 0)
    {
        cjFLOAT keyTime = posKeys->GetKey(0, keySiz)->GetTime();

		if(keyTime < stTime)
            stTime = keyTime;

		keyTime = posKeys->GetKey(keyCount - 1, keySiz)->GetTime();

        if(keyTime > enTime)
            enTime = keyTime;

        keyFound = cjTrue;
    }
	
    cjAniKeyRot::EKEY_TYPE rotKeyType;
    cjAniKeyRot* rotKeys = GetRotKeys(keyCount, rotKeyType, keySiz);

    if(keyCount > 0)
    {
        if(rotKeyType == cjAniKeyRot::EKT_EULERKEY)
        {
            cjAniKeyEulerRot* eulerRotKey = (cjAniKeyEulerRot*)rotKeys->GetKey(0, keySiz);

            for(cjBYTE c=0; c<3; c++)
            {
                cjUINT32 cjFLOATKeyCount = eulerRotKey->GetKeyCount(c);

                if(cjFLOATKeyCount > 0)
                {
                    cjAniKeyFloat* cjFLOATKeys = eulerRotKey->GetKeys(c);
                    cjBYTE cjFLOATKeySiz = eulerRotKey->GetKeySiz(c);

                    cjFLOAT keyTime = cjFLOATKeys->GetKey(0, cjFLOATKeySiz)->GetTime();

                    if(keyTime < stTime)
                        stTime = keyTime;

					keyTime = cjFLOATKeys->GetKey(cjFLOATKeyCount-1, cjFLOATKeySiz)->GetTime();

                    if(keyTime > enTime)
                        enTime = keyTime;

					keyFound = cjTrue;
                }
            }
        }
        else
        {
            cjFLOAT keyTime = rotKeys->GetKey(0, keySiz)->GetTime();

            if(keyTime < stTime)
                stTime = keyTime;

            keyTime = rotKeys->GetKey(keyCount-1, keySiz)->GetTime();

            if(keyTime > enTime)
                enTime = keyTime;

			keyFound = cjTrue;
        }
    }

    cjAniKeyVec3::EKEY_TYPE scaleKeyType;
    cjAniKeyVec3* scaleKeys = GetScaleKeys(keyCount, scaleKeyType, keySiz);

    if(keyCount > 0)
    {
        cjFLOAT keyTime = scaleKeys->GetKey(0, keySiz)->GetTime();
        
		if(keyTime < stTime)
            stTime = keyTime;

		keyTime = scaleKeys->GetKey(keyCount - 1, keySiz)->GetTime();

		if(keyTime > enTime)
            enTime = keyTime;

        keyFound = cjTrue;
    }

    if(!keyFound)
    {
        stTime = 0.0f;
        enTime = 0.0f;
    }
}

void cjTMEvaluator::CacheKeys(cjFLOAT stTime, cjFLOAT enTime)
{
    if(m_spTMDat)
        m_spTMDat->CacheKeys(stTime, enTime);
}

cjAniEvaluator* cjTMEvaluator::GetEvaluator(cjFLOAT stTime, cjFLOAT enTime)
{
    cjTMEvaluator* seqEvaluator = (cjTMEvaluator*)cjKeyBasedEvaluator::GetEvaluator(stTime, enTime);

    if(m_spTMDat)
    {
        cjAniTMDatRPtr spNewDat = m_spTMDat->GetDat(stTime, enTime);
        seqEvaluator->SetTMDat(spNewDat);
    }

    return seqEvaluator;
}

cjBool cjTMEvaluator::GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, 
	cjUINT32& fillSiz, cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const
{
    cjAssert(channel < cjAniEvalChan_PRS_Max);
    cjBool isPosed = IsChannelItemPosed(channel);

	if(IsChannelItemInvalid(channel) || (isPosed && !forceAlwaysUpdate && !IsAlwaysUpdate()))
        return cjFalse;
		
    if(channel == cjAniEvalChan_Pos) // position
    {
        fillSiz = isPosed ? 0 : sizeof(cjAniSPad::CubicVec3Fill);
        spadBlk = cjAniSPADB_CubicVec3PSeg;
    }	
    else if(channel == cjAniEvalChan_Rot) // rotation
    {        
		// Bezier/TCB = squad interpolation 
		// other      = slerp interpolation
        if(isPosed)
        {
            fillSiz = 0;
            spadBlk = cjAniSPADB_SlerpRotSeg;
        }
        else
        {
            cjAssert(m_spTMDat);
            cjUINT32 keyCount; cjAniKeyRot::EKEY_TYPE keyType; cjBYTE keySiz;
            m_spTMDat->GetRotKeys(keyCount, keyType, keySiz);

            if(keyType == cjAniKeyRot::EKT_EULERKEY)
                fillSiz = sizeof(cjAniSPad::EulerRotFill);
            else
                fillSiz = sizeof(cjAniSPad::RotFill);

            if(keyType == cjAniKeyRot::EKT_BEZKEY || keyType == cjAniKeyRot::EKT_TCBKEY)
                spadBlk = cjAniSPADB_SquadRotSeg;
            else
                spadBlk = cjAniSPADB_SlerpRotSeg;
        }
    }	
    else // scale
    {
        cjAssert(channel == cjAniEvalChan_Scl);

        fillSiz = isPosed ? 0 : sizeof(cjAniSPad::CubicVec3Fill);
        spadBlk = cjAniSPADB_CubicVec3SSeg;
    }

    isSharedFillDat = cjFalse;
    bsBasisDat = 0;

    return cjTrue;
}

cjBool cjTMEvaluator::InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, 
	cjBSplineBasisF3Dat* bsBasisDat, cjBool initSharedDat, cjRefObjNameHash* nodeHash,
	cjAniPoseBufHandle pbHandle) const
{
    cjAssert(channel < cjAniEvalChan_PRS_Max);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(!IsChannelItemInvalid(channel));

    cjBool isPosed = IsChannelItemPosed(channel);

    if(!isPosed && GetKeyType((cjWORD)channel) == cjAniKey::EKT_STEPKEY)
        evalSPDat->SetStepFunction(cjTrue);

	cjAssert(evalSPDat->GetSegMinTime() == CJ_FLT_INFINITY);
    cjAssert(evalSPDat->GetSegMaxTime() == CJ_FLT_INVALID);

	// position
    if(channel == cjAniEvalChan_Pos)
    {
        cjAniSPad::CubicVec3Seg* cubicVec3SegDat = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();
        cjAssert(cubicVec3SegDat);

        cubicVec3SegDat->m_hdr.m_lod = evalSPDat->GetLOD();

        if(IsReferenced())
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
            cubicVec3SegDat->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
        }
        else
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Vec3P);
            cubicVec3SegDat->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
        }

        if(isPosed)
        {
            evalSPDat->SetSegTimeRange(0.0f, CJ_FLT_INFINITY);
            cjAssert(evalSPDat->GetFillFunc() == cjNULL);

            cubicVec3SegDat->m_stTime = 0.0f;
            cubicVec3SegDat->m_invDTime = 0.0f;
            cubicVec3SegDat->m_val0 = m_value.GetPos();
            cubicVec3SegDat->m_outTangent0 = cjVec3::ZERO;
            cubicVec3SegDat->m_a0 = cjVec3::ZERO;
            cubicVec3SegDat->m_b0 = cjVec3::ZERO;
        }
        else
        {
            evalSPDat->SetFillFunc(&cjVec3PEvaluator::CubicVec3FillFunc);

            cjAniSPad::CubicVec3Fill* fillDat = (cjAniSPad::CubicVec3Fill*)evalSPDat->GetFillDat();
            fillDat->m_lastIndex = 0;
            cjAssert(m_spTMDat);

            fillDat->m_keys = m_spTMDat->GetPosKeys(fillDat->m_keyCount, fillDat->m_keyType, fillDat->m_keySiz);
            cjAssert(fillDat->m_keys);
            cjAssert(fillDat->m_keyCount > 0);
        }
    }

	// rotation
    else if(channel == cjAniEvalChan_Rot)
    {
        cjAniSPad::SlerpRotSeg* slerpRotSegDat = (cjAniSPad::SlerpRotSeg*)evalSPDat->GetSegDat();
        cjAssert(slerpRotSegDat);

        slerpRotSegDat->m_hdr.m_lod = evalSPDat->GetLOD();

        if(IsReferenced())
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
            slerpRotSegDat->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
        }
        else
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Rot);
            slerpRotSegDat->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
        }

        if(isPosed)
        {
            evalSPDat->SetSegTimeRange(0.0f, CJ_FLT_INFINITY);
            cjAssert(evalSPDat->GetFillFunc() == cjNULL);

            slerpRotSegDat->m_stTime = 0.0f;
            slerpRotSegDat->m_invDTime = 0.0f;
            slerpRotSegDat->m_quat0 = m_value.GetRotate();
            slerpRotSegDat->m_quat1 = slerpRotSegDat->m_quat0;
        }
        else
        {
            cjAssert(m_spTMDat);

			cjUINT32 keyCount; cjAniKey::EKEY_TYPE keyType; cjBYTE keySiz;
            cjAniKeyRot* keys = m_spTMDat->GetRotKeys(keyCount, keyType, keySiz);

            if(keyType == cjAniKeyRot::EKT_EULERKEY)
            {
                cjAniKeyEulerRot* eulerRotKey = (cjAniKeyEulerRot*)keys->GetKey(0, sizeof(cjAniKeyEulerRot));
                cjAssert(eulerRotKey);

                cjAniSPad::EulerRotFill* eulerRotFillDat = (cjAniSPad::EulerRotFill*)evalSPDat->GetFillDat();
                cjAssert(eulerRotFillDat);

                for(cjBYTE c=0; c<3; c++)
                {
                    eulerRotFillDat->m_minTime[c] = CJ_FLT_INFINITY;
                    eulerRotFillDat->m_maxTime[c] = CJ_FLT_INVALID;

                    cjAniSPad::CubicFloatFill* fillDat = &eulerRotFillDat->m_floatFill[c];
                    fillDat->m_lastIndex = 0;
                    fillDat->m_keyCount = eulerRotKey->GetKeyCount(c);
                    fillDat->m_keys = eulerRotKey->GetKeys(c);
                    fillDat->m_keyType = eulerRotKey->GetType(c);
                    fillDat->m_keySiz = eulerRotKey->GetKeySiz(c);

                    cjAniSPad::CubicFloatSeg* segDat = &eulerRotFillDat->m_floatSeg[c];
                    segDat->m_hdr.m_lod = evalSPDat->GetLOD();
                    segDat->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
                }

                evalSPDat->SetFillFunc(&cjQuatEvaluator::EulerRotFillFunc);
            }
            else
            {
                cjAniSPad::RotFill* fillDat = (cjAniSPad::RotFill*)evalSPDat->GetFillDat();

                fillDat->m_lastIndex = 0;
                fillDat->m_keys = keys;
                fillDat->m_keyCount = keyCount;
                fillDat->m_keyType = keyType;
                fillDat->m_keySiz = keySiz;
				                
                if(keyType == cjAniKeyRot::EKT_BEZKEY || keyType == cjAniKeyRot::EKT_TCBKEY)
                {					
                    evalSPDat->SetAlternateSPData(cjTrue); // use squad interpolation
                    evalSPDat->SetFillFunc(&cjQuatEvaluator::SquadRotFillFunc);
                }
                else
                {
                    evalSPDat->SetFillFunc(&cjQuatEvaluator::SlerpRotFillFunc);
                }
            }
        }
    }
    else
    {
        cjAssert(channel == cjAniEvalChan_Scl);

        cjAniSPad::CubicVec3Seg* segDat = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();
        segDat->m_hdr.m_lod = evalSPDat->GetLOD();

        if(IsReferenced())
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
            segDat->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
        }
        else
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Vec3S);            
			segDat->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
        }

        if(isPosed)
        {
            evalSPDat->SetSegTimeRange(0.0f, CJ_FLT_INFINITY);
            cjAssert(evalSPDat->GetFillFunc() == cjNULL);

            segDat->m_stTime = 0.0f;
            segDat->m_invDTime = 0.0f;
            segDat->m_val0 = m_value.GetScale();
            segDat->m_outTangent0 = cjVec3::ZERO;
            segDat->m_a0 = cjVec3::ZERO;
            segDat->m_b0 = cjVec3::ZERO;
        }
        else
        {
            evalSPDat->SetFillFunc(&cjVec3SEvaluator::CubicVec3FillFunc);

            cjAniSPad::CubicVec3Fill* fillDat = (cjAniSPad::CubicVec3Fill*)evalSPDat->GetFillDat();
            cjAssert(fillDat);

            fillDat->m_lastIndex = 0;
            fillDat->m_keys = m_spTMDat->GetScaleKeys(fillDat->m_keyCount, fillDat->m_keyType, fillDat->m_keySiz);

            cjAssert(fillDat->m_keys);
            cjAssert(fillDat->m_keyCount > 0);
        }
    }

    return cjTrue;
}

void cjTMEvaluator::Collapse()
{
    if(!m_spTMDat)
        return;

    cjQuatTM collapsedTM;
	
	//
	// pos
	//

    cjUINT32 keyCount; cjAniKey::EKEY_TYPE keyType; cjBYTE keySiz;
    cjAniKeyVec3* posKeys = m_spTMDat->GetPosKeys(keyCount, keyType, keySiz);
    if(!keyCount)
    {
        m_spTMDat->ReplacePosKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);

		if(m_value.IsPosValid())
			collapsedTM.SetPos(m_value.GetPos());
    }
    else
    {
        cjAniKey::PFIsPosed pfIsPosed = cjAniKeyVec3::GetIsPosedKeyPF(keyType);
        if(pfIsPosed(posKeys, keyCount))
        {
            cjVec3 value = posKeys->GetKey(0, keySiz)->GetValue();
            collapsedTM.SetPos(value);

            m_spTMDat->ReplacePosKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
        }
        else if(m_value.IsPosValid())
        {
            collapsedTM.SetPos(m_value.GetPos());
        }
    }
		
	//
    // rot
	//

    cjAniKeyRot* rotKeys = m_spTMDat->GetRotKeys(keyCount, keyType, keySiz);

    if(!keyCount)
    {
        m_spTMDat->ReplaceRotKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);

		if(m_value.IsRotateValid())
            collapsedTM.SetRotate(m_value.GetRotate());
    }
    else
    {
        cjAniKey::PFIsPosed pfIsPosed = cjAniKeyRot::GetIsPosedKeyPF(keyType);

        if(pfIsPosed(rotKeys, keyCount))
        {
            cjQuat value;

            if(keyType == cjAniKeyRot::EKT_EULERKEY)
            {
                cjAniKeyRot::PFInterpolation pfInterp = cjAniKeyRot::GetInterpKeyPF(keyType);
                pfInterp(0.0f, rotKeys->GetKey(0, keySiz), 0, &value);
            }
            else
            {
                value = rotKeys->GetKey(0, keySiz)->GetValue();
            }

            collapsedTM.SetRotate(value);
            m_spTMDat->ReplaceRotKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
        }
        else if(m_value.IsRotateValid())
        {
            collapsedTM.SetRotate(m_value.GetRotate());
        }
    }
		
	//
    // scale
	//

    cjAniKeyVec3* scaleKeys = m_spTMDat->GetScaleKeys(keyCount, keyType, keySiz);

    if(!keyCount)
    {
        m_spTMDat->ReplaceScaleKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);

        if(m_value.IsScaleValid())
            collapsedTM.SetScale(m_value.GetScale());
    }
    else
    {
        cjAniKey::PFIsPosed pfIsPosed =  cjAniKeyVec3::GetIsPosedKeyPF(keyType);

        if(pfIsPosed(scaleKeys, keyCount))
        {
            cjVec3 value = scaleKeys->GetKey(0, keySiz)->GetValue();
            collapsedTM.SetScale(value);

            m_spTMDat->ReplaceScaleKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
        }
        else if(m_value.IsScaleValid())
        {
            collapsedTM.SetScale(m_value.GetScale());
        }
    }

    if(!m_spTMDat->GetPosKeyCount() && !m_spTMDat->GetRotKeyCount() && !m_spTMDat->GetScaleKeyCount())
        m_spTMDat = cjNULL;

	m_value = collapsedTM;

    SetEvalChannelTypes();
}

cjUINT32 cjTMEvaluator::GetKeyCount(cjUINT32 channel) const
{
    if(!m_spTMDat)
        return 0;

    if(channel == cjAniEvalChan_Pos)
        return m_spTMDat->GetPosKeyCount();
    else if(channel == cjAniEvalChan_Rot)
        return m_spTMDat->GetRotKeyCount();
    else if(channel == cjAniEvalChan_Scl)
        return m_spTMDat->GetScaleKeyCount();

    cjAssert(channel < 3);

    return 0;
}

cjAniKey::EKEY_TYPE cjTMEvaluator::GetKeyType(cjUINT32 channel) const
{
    if(!m_spTMDat)
        return cjAniKey::EKT_NOINTERP;

    if(channel == cjAniEvalChan_Pos)
    {
        cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE keySiz;
        m_spTMDat->GetPosKeys(keyCount, keyType, keySiz);

        return keyType; 
    }
    else if(channel == cjAniEvalChan_Rot)
    {
        cjUINT32 keyCount; cjAniKeyRot::EKEY_TYPE keyType; cjBYTE keySiz;
        m_spTMDat->GetRotKeys(keyCount, keyType, keySiz);

        return keyType;     
    }
    else if(channel == cjAniEvalChan_Scl)
    {
        cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE keySiz;
        m_spTMDat->GetScaleKeys(keyCount, keyType, keySiz);

        return keyType;     
    }

    cjAssert(channel < 3);

    return cjAniKey::EKT_NOINTERP;
}

cjAniKey::EKEY_VALTYPE cjTMEvaluator::GetKeyValType(cjUINT32 channel) const
{
    if(channel == cjAniEvalChan_Pos)
		return cjAniKey::EKVAL_VEC3;
    else if(channel == cjAniEvalChan_Rot)
        return cjAniKey::EKVAL_ROT;
    else if(channel == cjAniEvalChan_Scl)
        return cjAniKey::EKVAL_VEC3;
    
    cjAssert(channel < 3);

    return cjAniKey::EKVAL_MAX;
}

cjAniKey* cjTMEvaluator::GetKeyAry(cjUINT32 channel) const
{
    if(!m_spTMDat)
        return cjNULL;

    if(channel == cjAniEvalChan_Pos)
    {       
        cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE keySiz;
        cjAniKeyVec3* keys = m_spTMDat->GetPosKeys(keyCount, keyType, keySiz);

        return keys; 
    }
    else if(channel == cjAniEvalChan_Rot)
    {
        cjUINT32 keyCount; cjAniKeyRot::EKEY_TYPE keyType; cjBYTE keySiz;
        cjAniKeyRot* keys = m_spTMDat->GetRotKeys(keyCount, keyType, keySiz);

        return keys; 
    }
    else if(channel == cjAniEvalChan_Scl)
    {
        cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE keySiz;
        cjAniKeyVec3* keys = m_spTMDat->GetScaleKeys(keyCount, keyType, keySiz);

        return keys; 
    }

    cjAssert(channel < 3);

    return cjNULL;
}

cjBYTE cjTMEvaluator::GetKeySiz(cjUINT32 channel) const
{
    if(!m_spTMDat)
        return 0;

    if(channel == cjAniEvalChan_Pos)
    {       
        cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE keySiz;
        m_spTMDat->GetPosKeys(keyCount, keyType, keySiz);

        return keySiz; 
    }
    else if(channel == cjAniEvalChan_Rot)
    {
        cjUINT32 keyCount; cjAniKeyRot::EKEY_TYPE keyType; cjBYTE keySiz;
        m_spTMDat->GetRotKeys(keyCount, keyType, keySiz);

        return keySiz; 
    }
    else if(channel == cjAniEvalChan_Scl)
    {
        cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE keySiz;
        m_spTMDat->GetScaleKeys(keyCount, keyType, keySiz);

        return keySiz; 
    }

    cjAssert(channel < 3);

    return 0;
}

void cjTMEvaluator::SetTMDat(cjAniTMDat* tmDat)
{
	m_spTMDat = tmDat;
	SetEvalChannelTypes();
}

void cjTMEvaluator::SetPoseValue(const cjQuatTM& poseValue)
{
    if(poseValue.IsRotateValid())
        SetPoseRotate(poseValue.GetRotate());

    if(poseValue.IsScaleValid())
        SetPoseScale(poseValue.GetScale());

	if(poseValue.IsPosValid())
        SetPosePos(poseValue.GetPos());

    SetEvalChannelTypes();
}

cjAniKeyVec3* cjTMEvaluator::GetScaleKeys(cjUINT32& keyCount, cjAniKeyVec3::EKEY_TYPE& keyType, cjBYTE& keySiz) const
{
    if(m_spTMDat)
        return m_spTMDat->GetScaleKeys(keyCount, keyType, keySiz);

    keyCount = 0; keyType = cjAniKeyVec3::EKT_NOINTERP; keySiz = 0;

    return cjNULL;
}

void cjTMEvaluator::ReplaceScaleKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType)
{
    if(keyCount > 0)
    {
        if(!m_spTMDat)
            m_spTMDat = new cjAniTMDat;

        m_spTMDat->ReplaceScaleKeys(keys, keyCount, keyType);
    }
    else if(m_spTMDat)
    {
         m_spTMDat->ReplaceScaleKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
    }

    SetEvalChannelTypes();
}

void cjTMEvaluator::SetPoseScale(const cjVec3& poseScale)
{
    m_value.SetScale(poseScale);
    
	if(m_spTMDat)
        m_spTMDat->ReplaceScaleKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);

    SetEvalChannelTypes();
}

cjAniKeyRot* cjTMEvaluator::GetRotKeys(cjUINT32& keyCount, cjAniKeyRot::EKEY_TYPE& keyType, cjBYTE& keySiz) const
{
    if(m_spTMDat)
		return m_spTMDat->GetRotKeys(keyCount, keyType, keySiz);

    keyCount = 0; keyType = cjAniKeyRot::EKT_NOINTERP; keySiz = 0;

    return cjNULL;
}

void cjTMEvaluator::ReplaceRotKeys(cjAniKeyRot* keys, cjUINT32 keyCount, cjAniKeyRot::EKEY_TYPE keyType)
{
    if(keyCount > 0)
    {
        if(!m_spTMDat)
            m_spTMDat = new cjAniTMDat;

        m_spTMDat->ReplaceRotKeys(keys, keyCount, keyType);
    }
    else if(m_spTMDat)
    {
         m_spTMDat->ReplaceRotKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
    }

    SetEvalChannelTypes();
}

void cjTMEvaluator::SetPoseRotate(const cjQuat& rot)
{
    m_value.SetRotate(rot);

    if(m_spTMDat)
        m_spTMDat->ReplaceRotKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);

    SetEvalChannelTypes();
}

cjAniKeyVec3* cjTMEvaluator::GetPosKeys(cjUINT32& keyCount, cjAniKeyVec3::EKEY_TYPE& keyType, cjBYTE& keySiz) const
{
    if(m_spTMDat)
        return m_spTMDat->GetPosKeys(keyCount, keyType, keySiz);

    keyCount = 0; keyType = cjAniKeyVec3::EKT_NOINTERP; keySiz = 0;

    return cjNULL;
}

void cjTMEvaluator::ReplacePosKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType)
{
    if(keyCount > 0)
    {
        if(!m_spTMDat)
            m_spTMDat = new cjAniTMDat;

        m_spTMDat->ReplacePosKeys(keys, keyCount, keyType);
    }
    else if(m_spTMDat)
    {
         m_spTMDat->ReplacePosKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
    }

    SetEvalChannelTypes();
}

void cjTMEvaluator::SetPosePos(const cjVec3& pos)
{
    m_value.SetPos(pos);

    if(m_spTMDat)
		m_spTMDat->ReplacePosKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);

    SetEvalChannelTypes();
}

void cjTMEvaluator::SetEvalChannelTypes()
{

    cjBool isPos = (GetKeyCount(cjAniEvalChan_Pos) > 0);
	if(isPos || m_value.IsPosValid())
    {
        m_channelItem[cjAniEvalChan_Pos] = cjAniEvalChanItem_Vec3P;

        if(!isPos)
            m_channelItem[cjAniEvalChan_Pos] |= cjAniEvalChanItemMask_Constant;
    }
    else
    {
        m_channelItem[cjAniEvalChan_Pos] = cjAniEvalChanItem_NoData;
    }

    cjBool isRot = (GetKeyCount(cjAniEvalChan_Rot) > 0);
    if(isRot || m_value.IsRotateValid())
    {
        m_channelItem[cjAniEvalChan_Rot] = cjAniEvalChanItem_Rot;
        
		if(!isRot)
            m_channelItem[cjAniEvalChan_Rot] |= cjAniEvalChanItemMask_Constant;
    }
    else
    {
        m_channelItem[cjAniEvalChan_Rot] = cjAniEvalChanItem_NoData;
    }

    cjBool isScale = (GetKeyCount(cjAniEvalChan_Scl) > 0);
    if(isScale || m_value.IsScaleValid())
    {
        m_channelItem[cjAniEvalChan_Scl] = cjAniEvalChanItem_Vec3S;

		if(!isScale)
            m_channelItem[cjAniEvalChan_Scl] |= cjAniEvalChanItemMask_Constant;
    }
    else
    {
        m_channelItem[cjAniEvalChan_Scl] = cjAniEvalChanItem_NoData;
    }
}


//
// cjKeyBasedEvaluator
//  - cjQuatEvaluator
//

CJSER_IMPL_CLASS(cjQuatEvaluator, cjKeyBasedEvaluator)

CJSER_IMPL_SERIALIZE_BEGIN(cjQuatEvaluator)
	
	if(ser.IsQuerySerObjAndStr())
	{
		if(m_spQuatDat)
			(*m_spQuatDat) >> ser;

		return;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		cjSerID serID;
	
		{
			if(ser.IsSaving())
				serID = ser.GetSerObjID(m_spQuatDat);

			cjSerID_Serialize(ser, serID);

			if(ser.IsLoading())
				m_spQuatDat = ser.GetSerObj<cjAniRotDat>(serID);
		}
	}
	
CJSER_IMPL_SERIALIZE_END(cjQuatEvaluator)

CJSER_IMPL_CLONE(cjQuatEvaluator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjQuatEvaluator)
	d->m_spQuatDat = m_spQuatDat;
CJSER_IMPL_CLONE_PROPCOPY_END(cjQuatEvaluator)


cjQuatEvaluator::cjQuatEvaluator()
{
}

cjQuatEvaluator::cjQuatEvaluator(cjAniRotDat* dat)
{
    SetEvalChannelTypes();
}

cjQuatEvaluator::~cjQuatEvaluator()
{
}

cjBool cjQuatEvaluator::GetChannelPosedValue(cjUINT32 channel, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(rst);

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChannelItemInvalid(channel));

        cjUINT32 keyCount; cjAniKey::EKEY_TYPE keyType; cjBYTE keySiz;
        cjAniKeyRot* keys = m_spQuatDat->GetKeys(keyCount, keyType, keySiz);
        cjAssert(keyCount == 1);

        cjAssert(keyType != cjAniKeyRot::EKT_EULERKEY);

        *(cjQuat*)rst = keys->GetKey(0, keySiz)->GetValue();

        return cjTrue;
    }

    return cjFalse;
}

cjBool cjQuatEvaluator::UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(rst);

    if(IsChannelItemInvalid(channel))
        return cjFalse;

    if(!evalSPDat->IsSegDatValid(etime))
    {
        cjAniEvalSPadFillFunc pfFill = evalSPDat->GetFillFunc();

		if(!pfFill || !(*pfFill)(etime, evalSPDat))
            return cjFalse;
    }

    if(!evalSPDat->IsAlternateSPData())
    {
        cjAniSPad::SlerpRotSeg* slerpRotSegDat = (cjAniSPad::SlerpRotSeg*)evalSPDat->GetSegDat();
        cjAssert(slerpRotSegDat);

        cjFLOAT ratio = (etime - slerpRotSegDat->m_stTime) * slerpRotSegDat->m_invDTime;
        if(ratio == 0.0f)
            *(cjQuat*)rst = slerpRotSegDat->m_quat0;
        else
            *(cjQuat*)rst = cjQuat::Slerp(ratio, slerpRotSegDat->m_quat0, slerpRotSegDat->m_quat1);
    }
    else
    {
        // Squad [Bez/TCB]
        cjAniSPad::SquadRotSeg* squadRotSeg = (cjAniSPad::SquadRotSeg*)evalSPDat->GetSegDat();
        cjAssert(squadRotSeg);

        cjAniSPad::SlerpRotSeg* baseSeg = &squadRotSeg->m_baseDat;
        cjFLOAT ratio = (etime - baseSeg->m_stTime) * baseSeg->m_invDTime;

		*(cjQuat*)rst = cjQuat::Squad(ratio, baseSeg->m_quat0, squadRotSeg->m_outQuat0, squadRotSeg->m_inQuat1, baseSeg->m_quat1);
    }

    return cjTrue;
}

void cjQuatEvaluator::GetValidTimeRange(cjFLOAT& stTime, cjFLOAT& enTime) const
{
    cjUINT32 keyCount; cjAniKeyRot::EKEY_TYPE keyType; cjBYTE keySiz;
    cjAniKeyRot* keys = GetKeys(keyCount, keyType, keySiz);
    cjBool foundKey = cjFalse;

    if(keyCount > 0)
    {
        if(keyType == cjAniKeyRot::EKT_EULERKEY)
        {
            cjAniKeyEulerRot* eulerRotKey = (cjAniKeyEulerRot*)keys->GetKey(0, keySiz);

            cjFLOAT tmpBeginKeyTime = CJ_FLT_INFINITY;
            cjFLOAT tmpEndKeyTime = CJ_FLT_INVALID;

            for(cjBYTE c=0; c<3; c++)
            {
                cjUINT32 cjFLOATKeyCount = eulerRotKey->GetKeyCount(c);
                cjBYTE cjFLOATKeySiz = eulerRotKey->GetKeySiz(c);

                if(cjFLOATKeyCount > 0)
                {
                    cjAniKeyFloat* cjFLOATKeys = eulerRotKey->GetKeys(c);
                    cjFLOAT keyTime = cjFLOATKeys->GetKey(0, cjFLOATKeySiz)->GetTime();

                    if(keyTime < tmpBeginKeyTime)
                        tmpBeginKeyTime = keyTime;

					keyTime = cjFLOATKeys->GetKey(cjFLOATKeyCount-1, cjFLOATKeySiz)->GetTime();

                    if(keyTime > tmpEndKeyTime)
                        tmpEndKeyTime = keyTime;

					foundKey = cjTrue;
                }
            }

            if(foundKey)
            {
                stTime = tmpBeginKeyTime;
                enTime = tmpEndKeyTime;
            }
        }
        else
        {
            stTime = keys->GetKey(0, keySiz)->GetTime();
            enTime = keys->GetKey(keyCount - 1, keySiz)->GetTime();

            foundKey = cjTrue;
        }
    }

    if(!foundKey)
    {
        stTime = 0.0f;
        enTime = 0.0f;
    }
}

void cjQuatEvaluator::CacheKeys(cjFLOAT stTime, cjFLOAT enTime)
{
    if(m_spQuatDat)
        m_spQuatDat->CacheKeys(stTime, enTime);
}

cjAniEvaluator* cjQuatEvaluator::GetEvaluator(cjFLOAT stTime, cjFLOAT enTime)
{
    cjQuatEvaluator* eval = (cjQuatEvaluator*)cjKeyBasedEvaluator::GetEvaluator(stTime, enTime);

    if(m_spQuatDat)
    {
        cjAniRotDatRPtr spNewRotDat = m_spQuatDat->GetDat(stTime, enTime);
        eval->SetRotDat(spNewRotDat);
    }

    return eval;
}

cjBool cjQuatEvaluator::GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, 
	cjUINT32& fillSiz, cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjBool isPosed = IsChannelItemPosed(channel);

    if(IsChannelItemInvalid(channel) || (isPosed && !forceAlwaysUpdate && !IsAlwaysUpdate()))
        return cjFalse;

    cjAssert(m_spQuatDat);
    cjUINT32 keyCount; cjAniKeyRot::EKEY_TYPE keyType; cjBYTE keySiz;
    m_spQuatDat->GetKeys(keyCount, keyType, keySiz);

    if(isPosed)
    {
        fillSiz = 0;
        spadBlk = cjAniSPADB_SlerpRotSeg;
    }
    else
    {
        if(keyType == cjAniKeyRot::EKT_EULERKEY)
            fillSiz = sizeof(cjAniSPad::EulerRotFill);
        else
            fillSiz = sizeof(cjAniSPad::RotFill);

        if(keyType == cjAniKeyRot::EKT_BEZKEY || keyType == cjAniKeyRot::EKT_TCBKEY)
            spadBlk = cjAniSPADB_SquadRotSeg;
        else
            spadBlk = cjAniSPADB_SlerpRotSeg;
    }

    isSharedFillDat = cjFalse;
    bsBasisDat = 0;

    return cjTrue;
}

cjBool cjQuatEvaluator::InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, 
	cjBool initSharedDat, cjRefObjNameHash* nodeHash, cjAniPoseBufHandle pbHandle) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(evalSPDat);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(!IsChannelItemInvalid(channel));
    cjAssert(m_spQuatDat);

    cjAniSPad::SlerpRotSeg* slerpRotSegDat = (cjAniSPad::SlerpRotSeg*)evalSPDat->GetSegDat();    
    slerpRotSegDat->m_hdr.m_lod = evalSPDat->GetLOD();

    if(IsReferenced())
    {
        cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
        slerpRotSegDat->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
    }
    else
    {
        cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Rot);
        slerpRotSegDat->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
    }

    cjUINT32 keyCount; cjAniKey::EKEY_TYPE keyType; cjBYTE keySiz;
    cjAniKeyRot* keys = m_spQuatDat->GetKeys(keyCount, keyType, keySiz);
    cjAssert(keys);
    cjAssert(keyCount > 0);

    cjBool isPosed = IsChannelItemPosed(channel);
    if(isPosed)
    {
        evalSPDat->SetSegTimeRange(0.0f, CJ_FLT_INFINITY);
        cjAssert(evalSPDat->GetFillFunc() == cjNULL);

        slerpRotSegDat->m_stTime = 0.0f;
        slerpRotSegDat->m_invDTime = 0.0f;
        cjAssert(keyType != cjAniKeyRot::EKT_EULERKEY);
        slerpRotSegDat->m_quat0 = keys->GetKey(0, keySiz)->GetValue();
        slerpRotSegDat->m_quat1 = slerpRotSegDat->m_quat0;
    }
    else
    {
        if(keyType == cjAniKey::EKT_STEPKEY)
            evalSPDat->SetStepFunction(cjTrue);

		cjAssert(evalSPDat->GetSegMinTime() == CJ_FLT_INFINITY);
        cjAssert(evalSPDat->GetSegMaxTime() == CJ_FLT_INVALID);

        if(keyType == cjAniKeyRot::EKT_EULERKEY)
        {
            cjAniKeyEulerRot* eulerRotKey = (cjAniKeyEulerRot*)keys->GetKey(0, sizeof(cjAniKeyEulerRot));
            cjAssert(eulerRotKey);

            cjAniSPad::EulerRotFill* eulerRotFillDat = (cjAniSPad::EulerRotFill*)evalSPDat->GetFillDat();
            cjAssert(eulerRotFillDat);

            for(cjBYTE c=0; c<3; c++)
            {
                eulerRotFillDat->m_minTime[c] = CJ_FLT_INFINITY;
                eulerRotFillDat->m_maxTime[c] = CJ_FLT_INVALID;

                cjAniSPad::CubicFloatFill* fill = &eulerRotFillDat->m_floatFill[c];

                fill->m_lastIndex = 0;
                fill->m_keyCount = eulerRotKey->GetKeyCount(c);
                fill->m_keys = eulerRotKey->GetKeys(c);
                fill->m_keyType = eulerRotKey->GetType(c);
                fill->m_keySiz = eulerRotKey->GetKeySiz(c);

                cjAniSPad::CubicFloatSeg* pkSeg = &eulerRotFillDat->m_floatSeg[c];
                pkSeg->m_hdr.m_lod = evalSPDat->GetLOD();
                pkSeg->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
            }

            evalSPDat->SetFillFunc(&EulerRotFillFunc);
        }
        else
        {
            cjAniSPad::RotFill* rotFillDat = (cjAniSPad::RotFill*)evalSPDat->GetFillDat();
            cjAssert(rotFillDat);

            rotFillDat->m_lastIndex = 0;
            rotFillDat->m_keys = keys;
            rotFillDat->m_keyCount = keyCount;
            rotFillDat->m_keyType = keyType;
            rotFillDat->m_keySiz = keySiz;

			// user squad interpolation for BEZ/TCB
            if(keyType == cjAniKeyRot::EKT_BEZKEY || keyType == cjAniKeyRot::EKT_TCBKEY)
            {
                evalSPDat->SetAlternateSPData(cjTrue); // for squad interpolation
                evalSPDat->SetFillFunc(&SquadRotFillFunc);
            }
            else
            {
                evalSPDat->SetFillFunc(&SlerpRotFillFunc);
            }
        }
    }

    return cjTrue;
}

void cjQuatEvaluator::Collapse()
{
    if(m_spQuatDat)
    {
        cjUINT32 keyCount; cjAniKey::EKEY_TYPE keyType; cjBYTE keySiz;
        cjAniKeyRot* keys = m_spQuatDat->GetKeys(keyCount, keyType, keySiz);

        if(!keyCount)
        {
            m_spQuatDat = cjNULL;
        }
        else
        {
            cjAniKey::PFIsPosed pfIsPosed = cjAniKeyRot::GetIsPosedKeyPF(keyType);
            cjAssert(pfIsPosed);

            if(pfIsPosed(keys, keyCount))
            {
                cjAniKeyRot* linRotKeys = (cjAniKeyRot*)new cjAniKeyLinRot[1];
                linRotKeys[0].SetTime(0.0f);

                cjQuat value;
                if(keyType == cjAniKeyRot::EKT_EULERKEY)
                {
                    cjAniKeyRot::PFInterpolation pfInterp = cjAniKeyRot::GetInterpKeyPF(keyType);
                    pfInterp(0.0f, keys->GetKey(0, keySiz), 0, &value);
                }
                else
                {
                    value = keys->GetKey(0, keySiz)->GetValue();
                }

				linRotKeys[0].SetValue(value); 
                m_spQuatDat->ReplaceKeys(linRotKeys, 1, cjAniKey::EKT_LINKEY);
            }
        }
    }

    SetEvalChannelTypes();
}

cjUINT32 cjQuatEvaluator::GetKeyCount(cjUINT32 ) const
{
    if(!m_spQuatDat)
        return 0;
    else
        return m_spQuatDat->GetKeyCount();
}

cjAniKey::EKEY_TYPE cjQuatEvaluator::GetKeyType(cjUINT32 ) const
{
    if(!m_spQuatDat)
        return cjAniKey::EKT_NOINTERP;

    cjUINT32 keyCount; cjAniKeyRot::EKEY_TYPE keyType; cjBYTE keySiz;
    m_spQuatDat->GetKeys(keyCount, keyType, keySiz);

    return keyType; 
}

cjAniKey* cjQuatEvaluator::GetKeyAry(cjUINT32 ) const
{
    if(!m_spQuatDat)
        return cjNULL;

    cjUINT32 keyCount; cjAniKeyRot::EKEY_TYPE keyType; cjBYTE keySiz;
    cjAniKeyRot* keys = m_spQuatDat->GetKeys(keyCount, keyType, keySiz);

    return keys; 
}

cjBYTE cjQuatEvaluator::GetKeySiz(cjUINT32 ) const
{
    if(!m_spQuatDat)
        return 0;

    cjUINT32 keyCount; cjAniKeyRot::EKEY_TYPE keyType; cjBYTE keySiz;
    m_spQuatDat->GetKeys(keyCount, keyType, keySiz);

    return keySiz; 
}

void cjQuatEvaluator::SetRotDat(cjAniRotDat* dat)
{
    m_spQuatDat = dat;
    SetEvalChannelTypes();
}

cjAniKeyRot* cjQuatEvaluator::GetKeys(cjUINT32& keyCount, cjAniKeyRot::EKEY_TYPE& keyType, cjBYTE& keySiz) const
{
    if(m_spQuatDat)
        return m_spQuatDat->GetKeys(keyCount, keyType, keySiz);

    keyCount = 0; keyType = cjAniKeyRot::EKT_NOINTERP; keySiz = 0;

    return cjNULL;
}

void cjQuatEvaluator::ReplaceKeys(cjAniKeyRot* keys, cjUINT32 keyCount, cjAniKeyRot::EKEY_TYPE keyType)
{
    if(keyCount > 0)
    {
        if(!m_spQuatDat)
            m_spQuatDat = new cjAniRotDat;

        m_spQuatDat->ReplaceKeys(keys, keyCount, keyType);
    }
    else if(m_spQuatDat)
    {
        m_spQuatDat->ReplaceKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
    }

    SetEvalChannelTypes();
}

void cjQuatEvaluator::SetKeys(cjAniKeyRot* keys, cjUINT32 keyCount, cjAniKeyRot::EKEY_TYPE keyType)
{
    if(keyCount > 0)
    {
        if(!m_spQuatDat)
            m_spQuatDat = new cjAniRotDat;

        m_spQuatDat->SetKeys(keys, keyCount, keyType);
    }
    else if(m_spQuatDat)
    {
        m_spQuatDat->SetKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
    }

    SetEvalChannelTypes();
}

cjBool cjQuatEvaluator::SlerpRotFillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat)
{
    cjAssert(evalSPDat);

	cjAniSPad::RotFill* rotFillDat = (cjAniSPad::RotFill*)evalSPDat->GetFillDat();

    cjAssert(rotFillDat);
    cjAssert(rotFillDat->m_keys);
    cjAssert(rotFillDat->m_keyCount > 0);

    cjAniSPad::SlerpRotSeg* slerpRotSegDat = (cjAniSPad::SlerpRotSeg*)evalSPDat->GetSegDat();
    cjAssert(slerpRotSegDat);

    cjFLOAT minTime, maxTime;
    cjQuat outQuat0, inQuat1;

    cjAniKeyRot::GenCubicCoefs(etime, rotFillDat->m_keys, rotFillDat->m_keyType, rotFillDat->m_keyCount, 
        rotFillDat->m_lastIndex, rotFillDat->m_keySiz, minTime, maxTime, 
		slerpRotSegDat->m_quat0, slerpRotSegDat->m_quat1, outQuat0, inQuat1);

    cjAssert(outQuat0 == slerpRotSegDat->m_quat0);
    cjAssert(inQuat1 == slerpRotSegDat->m_quat1);

    slerpRotSegDat->m_stTime = minTime;
    slerpRotSegDat->m_invDTime = (maxTime != minTime) ? 1.0f / (maxTime - minTime) : 0.0f;

    evalSPDat->SetSegTimeRange(minTime, maxTime);

    return cjTrue;
}

cjBool cjQuatEvaluator::SquadRotFillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat)
{
    cjAssert(evalSPDat);
    cjAniSPad::RotFill* rotFillDat = (cjAniSPad::RotFill*)evalSPDat->GetFillDat();

    cjAssert(rotFillDat);
    cjAssert(rotFillDat->m_keys);
    cjAssert(rotFillDat->m_keyCount > 0);

    cjAniSPad::SquadRotSeg* squadRotSeg = (cjAniSPad::SquadRotSeg*)evalSPDat->GetSegDat();
    cjAniSPad::SlerpRotSeg* baseSeg = &squadRotSeg->m_baseDat;

    cjFLOAT minTime, maxTime;
    cjAniKeyRot::GenCubicCoefs(etime, rotFillDat->m_keys, rotFillDat->m_keyType, rotFillDat->m_keyCount, 
        rotFillDat->m_lastIndex, rotFillDat->m_keySiz, minTime, maxTime, 
		baseSeg->m_quat0, baseSeg->m_quat1, squadRotSeg->m_outQuat0, squadRotSeg->m_inQuat1);

    baseSeg->m_stTime = minTime;
    baseSeg->m_invDTime = (maxTime != minTime) ? 1.0f / (maxTime - minTime) : 0.0f;

    evalSPDat->SetSegTimeRange(minTime, maxTime);

    return cjTrue;
}

cjBool cjQuatEvaluator::EulerRotFillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat)
{
    cjAssert(evalSPDat);

    cjAniSPad::EulerRotFill* eulerFill = (cjAniSPad::EulerRotFill*)evalSPDat->GetFillDat();
    cjFLOAT angle[3];

    for(cjUINT32 i=0; i<3; i++)
    {
        cjAniSPad::CubicFloatSeg* cubicFloatSeg = &eulerFill->m_floatSeg[i];

        if(etime > eulerFill->m_maxTime[i] || etime < eulerFill->m_minTime[i])
        {
            cjAniSPad::CubicFloatFill* cubicFloatFillDat = &eulerFill->m_floatFill[i];
            cjAssert(cubicFloatFillDat->m_keys);
            cjAssert(cubicFloatFillDat->m_keyCount > 0);

            cjFLOAT minTime, maxTime;

            cjAniKeyFloat::GenCubicCoefs(etime, cubicFloatFillDat->m_keys, 
                cubicFloatFillDat->m_keyType, cubicFloatFillDat->m_keyCount, 
                cubicFloatFillDat->m_lastIndex, cubicFloatFillDat->m_keySiz, 
                minTime, maxTime, 
				cubicFloatSeg->m_value0, cubicFloatSeg->m_outTangent0, cubicFloatSeg->m_a0, cubicFloatSeg->m_b0);

            cubicFloatSeg->m_stTime = minTime;
            cubicFloatSeg->m_invDTime = (maxTime != minTime) ? 1.0f / (maxTime - minTime) : 0.0f;

            eulerFill->m_minTime[i] = minTime;
            eulerFill->m_maxTime[i] = maxTime;
        }

        cjFLOAT ratio = (etime - cubicFloatSeg->m_stTime) * cubicFloatSeg->m_invDTime;
        angle[i] = cubicFloatSeg->m_value0 + (cubicFloatSeg->m_outTangent0 + (cubicFloatSeg->m_a0 + cubicFloatSeg->m_b0 * ratio) * ratio) * ratio;
    }

    cjAniSPad::SlerpRotSeg* slerpRotSegDat = (cjAniSPad::SlerpRotSeg*)evalSPDat->GetSegDat();
    cjAssert(slerpRotSegDat);

    slerpRotSegDat->m_stTime = etime;
    slerpRotSegDat->m_invDTime = 0.0f;
    slerpRotSegDat->m_quat0.MakeFrom3AxisAngle(angle[0], angle[1], angle[2]);
    slerpRotSegDat->m_quat1 = slerpRotSegDat->m_quat0;

    evalSPDat->SetSegTimeRange(etime, etime);
        
    return cjTrue;
}

void cjQuatEvaluator::SetEvalChannelTypes()
{
    if(m_spQuatDat)
    {
        cjUINT32 keyCount; cjAniKeyRot::EKEY_TYPE keyType; cjBYTE keySiz;
        GetKeys(keyCount, keyType, keySiz);

        if(keyCount)
        {
            m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_Rot;

            if(m_spQuatDat->GetKeyCount() == 1 && keyType != cjAniKeyRot::EKT_EULERKEY)
                m_channelItem[cjAniEvalChan_Base] |= cjAniEvalChanItemMask_Constant;
        }
        else
        {
            m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_NoData;
        }
    }
    else
    {
        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_NoData;
    }
}


//
// cjKeyBasedEvaluator
//  - cjFloatEvaluator
//

CJSER_IMPL_CLASS(cjFloatEvaluator, cjKeyBasedEvaluator)

CJSER_IMPL_SERIALIZE_BEGIN(cjFloatEvaluator)
	
	if(ser.IsQuerySerObjAndStr())
	{
		if(m_spFloatDat)
			(*m_spFloatDat) >> ser;

		return;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		cjSerID serID;
	
		{
			if(ser.IsSaving())
				serID = ser.GetSerObjID(m_spFloatDat);

			cjSerID_Serialize(ser, serID);

			if(ser.IsLoading())
				m_spFloatDat = ser.GetSerObj<cjAniFloatDat>(serID);
		}
	}
	
CJSER_IMPL_SERIALIZE_END(cjFloatEvaluator)

CJSER_IMPL_CLONE(cjFloatEvaluator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjFloatEvaluator)
	d->m_spFloatDat = m_spFloatDat;
CJSER_IMPL_CLONE_PROPCOPY_END(cjFloatEvaluator)


cjFloatEvaluator::cjFloatEvaluator()
{
}

cjFloatEvaluator::cjFloatEvaluator(cjAniFloatDat* dat)
{
	m_spFloatDat = dat;
    SetEvalChannelTypes();
}

cjFloatEvaluator::~cjFloatEvaluator()
{
}

cjBool cjFloatEvaluator::GetChannelPosedValue(cjUINT32 channel, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChannelItemInvalid(channel));
        cjAssert(m_spFloatDat);

        cjUINT32 keyCount; cjAniKey::EKEY_TYPE keyType; cjBYTE keySiz;
        cjAniKeyFloat* keys = m_spFloatDat->GetKeys(keyCount, keyType, keySiz);
        cjAssert(keyCount == 1);

        *(cjFLOAT*)rst = keys->GetKey(0, keySiz)->GetValue();

        return cjTrue;
    }

    return cjFalse;
}

cjBool cjFloatEvaluator::UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(evalSPDat);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(rst);

    if(IsChannelItemInvalid(channel))
        return cjFalse;

    if(!evalSPDat->IsSegDatValid(etime))
    {
        cjAniEvalSPadFillFunc pfFill = evalSPDat->GetFillFunc();

        if(!pfFill || !(*pfFill)(etime, evalSPDat))
            return cjFalse;
    }

    cjAniSPad::CubicFloatSeg* cubicFloatSegDat = (cjAniSPad::CubicFloatSeg*)evalSPDat->GetSegDat();
    cjAssert(cubicFloatSegDat);

    cjFLOAT ratio = (etime - cubicFloatSegDat->m_stTime) * cubicFloatSegDat->m_invDTime;

    *(cjFLOAT*)rst = cubicFloatSegDat->m_value0 + (cubicFloatSegDat->m_outTangent0 
		+ (cubicFloatSegDat->m_a0 + cubicFloatSegDat->m_b0 * ratio) * ratio) * ratio;

    return cjTrue;
}

void cjFloatEvaluator::CacheKeys(cjFLOAT stTime, cjFLOAT enTime)
{
    if(m_spFloatDat)
        m_spFloatDat->CacheKeys(stTime, enTime);
}

cjAniEvaluator* cjFloatEvaluator::GetEvaluator(cjFLOAT stTime, cjFLOAT enTime)
{
    cjFloatEvaluator* eval = (cjFloatEvaluator*)cjKeyBasedEvaluator::GetEvaluator(stTime, enTime);

    if(m_spFloatDat)
    {
        cjAniFloatDatRPtr spNewFloatDat = m_spFloatDat->GetDat(stTime, enTime);
        eval->SetFloatDat(spNewFloatDat);
    }

    return eval;
}

cjBool cjFloatEvaluator::GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, 
	cjUINT32& fillSiz, cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const
{
    cjAssert(channel == cjAniEvalChan_Base);

    cjBool isPosed = IsChannelItemPosed(channel);
    if(IsChannelItemInvalid(channel) ||
        (isPosed && !forceAlwaysUpdate && !IsAlwaysUpdate()))
    {
        return cjFalse;
    }

    fillSiz = isPosed ? 0 : sizeof(cjAniSPad::CubicFloatFill);
    isSharedFillDat = cjFalse;
    spadBlk =  cjAniSPADB_CubicFloatSeg;
    bsBasisDat = 0;
    return cjTrue;
}

cjBool cjFloatEvaluator::InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, 
	cjBool initSharedDat, cjRefObjNameHash* nodeHash, cjAniPoseBufHandle pbHandle) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(evalSPDat);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(!IsChannelItemInvalid(channel));
    cjAssert(m_spFloatDat);

    cjAniSPad::CubicFloatSeg* cubicFloatSegDat = (cjAniSPad::CubicFloatSeg*)evalSPDat->GetSegDat();
    cjAssert(cubicFloatSegDat);
    cubicFloatSegDat->m_hdr.m_lod = evalSPDat->GetLOD();

    if(IsReferenced())
    {
        cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
        cubicFloatSegDat->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
    }
    else
    {
        cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Float);
        cubicFloatSegDat->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
    }

    cjUINT32 keyCount; cjAniKey::EKEY_TYPE keyType; cjBYTE keySiz;
    cjAniKeyFloat* keys = m_spFloatDat->GetKeys(keyCount, keyType, keySiz);

    cjBool isPosed = IsChannelItemPosed(channel);
    if(isPosed)
    {
        evalSPDat->SetSegTimeRange(0.0f, CJ_FLT_INFINITY);
        cjAssert(evalSPDat->GetFillFunc() == cjNULL);

        cubicFloatSegDat->m_stTime = 0.0f;
        cubicFloatSegDat->m_invDTime = 0.0f;
        cubicFloatSegDat->m_value0 = keys->GetKey(0, keySiz)->GetValue();
        cubicFloatSegDat->m_outTangent0 = 0.0f;
        cubicFloatSegDat->m_a0 = 0.0f;
        cubicFloatSegDat->m_b0 = 0.0f;
    }
    else
    {
        if(keyType == cjAniKey::EKT_STEPKEY)
            evalSPDat->SetStepFunction(cjTrue);

		cjAssert(evalSPDat->GetSegMinTime() == CJ_FLT_INFINITY);
        cjAssert(evalSPDat->GetSegMaxTime() == CJ_FLT_INVALID);
        evalSPDat->SetFillFunc(&CubicFloatFillFunc);

        cjAniSPad::CubicFloatFill* cubicFloatFillDat = (cjAniSPad::CubicFloatFill*)evalSPDat->GetFillDat();

		cubicFloatFillDat->m_lastIndex = 0;
        cubicFloatFillDat->m_keys = keys;
        cubicFloatFillDat->m_keyCount = keyCount;
        cubicFloatFillDat->m_keyType = keyType;
        cubicFloatFillDat->m_keySiz = keySiz;
    }

    return cjTrue;
}

void cjFloatEvaluator::Collapse()
{
    if(m_spFloatDat)
    {
        cjUINT32 keyCount; cjAniKey::EKEY_TYPE keyType; cjBYTE keySiz;
        cjAniKeyFloat* keys = m_spFloatDat->GetKeys(keyCount, keyType, keySiz);

        if(!keyCount)
        {
            m_spFloatDat = cjNULL;
        }
        else
        {
            cjAniKey::PFIsPosed pfIsPosed = cjAniKeyFloat::GetIsPosedKeyPF(keyType);

            if(pfIsPosed(keys, keyCount))
            {
                cjAniKeyFloat* linFloatKeys = (cjAniKeyFloat*)new cjAniKeyLinFloat[1];

                linFloatKeys[0].SetTime(0.0f);

                cjFLOAT value = keys->GetKey(0, keySiz)->GetValue();
                linFloatKeys[0].SetValue(value);

                m_spFloatDat->ReplaceKeys(linFloatKeys, 1, cjAniKey::EKT_LINKEY);
            }
        }
    }

    SetEvalChannelTypes();
}

cjUINT32 cjFloatEvaluator::GetKeyCount(cjUINT32 ) const
{
    if(!m_spFloatDat)
        return 0;
    else
        return m_spFloatDat->GetKeyCount();
}

cjAniKey::EKEY_TYPE cjFloatEvaluator::GetKeyType(cjUINT32 ) const
{
    if(!m_spFloatDat)
        return cjAniKey::EKT_NOINTERP;

    cjUINT32 keyCount; cjAniKeyFloat::EKEY_TYPE keyType; cjBYTE keySiz;
    m_spFloatDat->GetKeys(keyCount, keyType, keySiz);

    return keyType; 
}

cjAniKey* cjFloatEvaluator::GetKeyAry(cjUINT32 ) const
{
    if(!m_spFloatDat)
        return cjNULL;

    cjUINT32 keyCount; cjAniKeyFloat::EKEY_TYPE keyType; cjBYTE keySiz;
    cjAniKeyFloat* keys = m_spFloatDat->GetKeys(keyCount, keyType, keySiz);

    return keys; 
}

cjBYTE cjFloatEvaluator::GetKeySiz(cjUINT32 ) const
{
    if(!m_spFloatDat)
        return 0;

    cjUINT32 keyCount; cjAniKeyFloat::EKEY_TYPE keyType; cjBYTE keySiz;
    m_spFloatDat->GetKeys(keyCount, keyType, keySiz);

    return keySiz; 
}

void cjFloatEvaluator::SetFloatDat(cjAniFloatDat* dat)
{
    m_spFloatDat = dat;
    SetEvalChannelTypes();
}

cjAniKeyFloat* cjFloatEvaluator::GetKeys(cjUINT32& keyCount, cjAniKeyFloat::EKEY_TYPE& keyType, cjBYTE& keySiz) const
{
    if(m_spFloatDat)
        return m_spFloatDat->GetKeys(keyCount, keyType, keySiz);

    keyCount = 0; keyType = cjAniKeyFloat::EKT_NOINTERP; keySiz = 0;

    return cjNULL;
}

void cjFloatEvaluator::ReplaceKeys(cjAniKeyFloat* keys, cjUINT32 keyCount, cjAniKeyFloat::EKEY_TYPE keyType)
{
    if(keyCount > 0)
    {
        if(!m_spFloatDat)
            m_spFloatDat = new cjAniFloatDat;

        m_spFloatDat->ReplaceKeys(keys, keyCount, keyType);
    }
    else if(m_spFloatDat)
    {
        m_spFloatDat->ReplaceKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
    }

    SetEvalChannelTypes();
}

void cjFloatEvaluator::SetKeys(cjAniKeyFloat* keys, cjUINT32 keyCount, cjAniKeyFloat::EKEY_TYPE keyType)
{
    if(keyCount > 0)
    {
        if(!m_spFloatDat)
            m_spFloatDat = new cjAniFloatDat;

        m_spFloatDat->SetKeys(keys, keyCount, keyType);
    }
    else if(m_spFloatDat)
    {
        m_spFloatDat->SetKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
    }

    SetEvalChannelTypes();
}

cjBool cjFloatEvaluator::CubicFloatFillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat)
{
    cjAssert(evalSPDat);

    cjAniSPad::CubicFloatFill* cubicFloatFillDat = (cjAniSPad::CubicFloatFill*)evalSPDat->GetFillDat();

    cjAssert(cubicFloatFillDat);
    cjAssert(cubicFloatFillDat->m_keys);
    cjAssert(cubicFloatFillDat->m_keyCount > 0);

    cjAniSPad::CubicFloatSeg* cubicFloatSegDat = (cjAniSPad::CubicFloatSeg*)evalSPDat->GetSegDat();

	cjFLOAT minTime, maxTime;

    cjAniKeyFloat::GenCubicCoefs(etime, cubicFloatFillDat->m_keys, cubicFloatFillDat->m_keyType, 
		cubicFloatFillDat->m_keyCount, cubicFloatFillDat->m_lastIndex, cubicFloatFillDat->m_keySiz, 
        minTime, maxTime, cubicFloatSegDat->m_value0, cubicFloatSegDat->m_outTangent0, cubicFloatSegDat->m_a0, 
        cubicFloatSegDat->m_b0);

    cubicFloatSegDat->m_stTime = minTime;
    cubicFloatSegDat->m_invDTime = (maxTime != minTime) ? 1.0f / (maxTime - minTime) : 0.0f;

    evalSPDat->SetSegTimeRange(minTime, maxTime);

    return cjTrue;
}

void cjFloatEvaluator::SetEvalChannelTypes()
{
    if(m_spFloatDat && m_spFloatDat->GetKeyCount() > 0)
    {
        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_Float;

        if(m_spFloatDat->GetKeyCount() == 1)
            m_channelItem[cjAniEvalChan_Base] |= cjAniEvalChanItemMask_Constant;
    }
    else
    {
        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_NoData;
    }
}


//
// cjKeyBasedEvaluator
//  - cjVec3SEvaluator
//

CJSER_IMPL_CLASS(cjVec3SEvaluator, cjKeyBasedEvaluator)

CJSER_IMPL_SERIALIZE_BEGIN(cjVec3SEvaluator)
	
	if(ser.IsQuerySerObjAndStr())
	{
		if(m_spVec3ScaleDat)
			(*m_spVec3ScaleDat) >> ser;

		return;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		cjSerID serID;
	
		{
			if(ser.IsSaving())
				serID = ser.GetSerObjID(m_spVec3ScaleDat);

			cjSerID_Serialize(ser, serID);

			if(ser.IsLoading())
				m_spVec3ScaleDat = ser.GetSerObj<cjAniVec3Dat>(serID);
		}
	}
	
CJSER_IMPL_SERIALIZE_END(cjVec3SEvaluator)

CJSER_IMPL_CLONE(cjVec3SEvaluator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjVec3SEvaluator)
	d->m_spVec3ScaleDat = m_spVec3ScaleDat;
CJSER_IMPL_CLONE_PROPCOPY_END(cjVec3SEvaluator)


cjVec3SEvaluator::cjVec3SEvaluator()
{
}

cjVec3SEvaluator::cjVec3SEvaluator(cjAniVec3Dat* dat)    
{
	m_spVec3ScaleDat = dat;
    SetEvalChannelTypes();
}

cjVec3SEvaluator::~cjVec3SEvaluator()
{
}

cjBool cjVec3SEvaluator::GetChannelPosedValue(cjUINT32 channel, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(rst);

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChannelItemInvalid(channel));

        cjUINT32 keyCount; cjAniKey::EKEY_TYPE keyType; cjBYTE keySiz;
        cjAniKeyVec3* keys = m_spVec3ScaleDat->GetKeys(keyCount, keyType, keySiz);
        cjAssert(keyCount == 1);

        *(cjVec3*)rst = keys->GetKey(0, keySiz)->GetValue();

        return cjTrue;
    }

    return cjFalse;
}

cjBool cjVec3SEvaluator::UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);

    if(IsChannelItemInvalid(channel))
        return cjFalse;

    if(!evalSPDat->IsSegDatValid(etime))
    {
        cjAniEvalSPadFillFunc pfFill = evalSPDat->GetFillFunc();

        if(!pfFill || !(*pfFill)(etime, evalSPDat))
            return cjFalse;
    }

    cjAniSPad::CubicVec3Seg* cubicVec3SegDat = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();
    cjAssert(cubicVec3SegDat);

    cjFLOAT ratio = (etime - cubicVec3SegDat->m_stTime) * cubicVec3SegDat->m_invDTime; 
	*(cjVec3*)rst = cubicVec3SegDat->m_val0 + (cubicVec3SegDat->m_outTangent0 + (cubicVec3SegDat->m_a0 + cubicVec3SegDat->m_b0 * ratio) * ratio) * ratio;

    return cjTrue;
}

void cjVec3SEvaluator::CacheKeys(cjFLOAT stTime, cjFLOAT enTime)
{
    if(m_spVec3ScaleDat)
        m_spVec3ScaleDat->CacheKeys(stTime, enTime);
}

cjAniEvaluator* cjVec3SEvaluator::GetEvaluator(cjFLOAT stTime, cjFLOAT enTime)
{
    cjVec3SEvaluator* eval = (cjVec3SEvaluator*)cjKeyBasedEvaluator::GetEvaluator(stTime, enTime);

    if(m_spVec3ScaleDat)
    {
        cjAniVec3DatRPtr spNewDat = m_spVec3ScaleDat->GetDat(stTime, enTime);
        eval->SetVec3ScaleDat(spNewDat);
    }

    return eval;
}

cjBool cjVec3SEvaluator::GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, 
	cjUINT32& fillSiz, cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjBool isPosed = IsChannelItemPosed(channel);

    if(IsChannelItemInvalid(channel) || (isPosed && !forceAlwaysUpdate && !IsAlwaysUpdate()))
        return cjFalse;

    fillSiz = isPosed ? 0 : sizeof(cjAniSPad::CubicVec3Fill);

    isSharedFillDat = cjFalse;
    spadBlk = cjAniSPADB_CubicVec3SSeg;
    bsBasisDat = 0;

    return cjTrue;
}

cjBool cjVec3SEvaluator::InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, 
	cjBool initSharedDat, cjRefObjNameHash* nodeHash, cjAniPoseBufHandle pbHandle) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(!IsChannelItemInvalid(channel));
    cjAssert(m_spVec3ScaleDat);

    cjAniSPad::CubicVec3Seg* cubicVec3SegDat = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();
    cubicVec3SegDat->m_hdr.m_lod = evalSPDat->GetLOD();

    if(IsReferenced())
    {
        cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
        cubicVec3SegDat->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
    }
    else
    {
        cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Vec3S);
        cubicVec3SegDat->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
    }

    cjUINT32 keyCount; cjAniKey::EKEY_TYPE keyType; cjBYTE keySiz;
    cjAniKeyVec3* keys = m_spVec3ScaleDat->GetKeys(keyCount, keyType, keySiz);
    cjAssert(keys);
    cjAssert(keyCount > 0);

    cjBool isPosed = IsChannelItemPosed(channel);
    if(isPosed)
    {
        evalSPDat->SetSegTimeRange(0.0f, CJ_FLT_INFINITY);
        cjAssert(evalSPDat->GetFillFunc() == cjNULL);

        cubicVec3SegDat->m_stTime = 0.0f;
        cubicVec3SegDat->m_invDTime = 0.0f;
        cubicVec3SegDat->m_val0 = keys->GetKey(0, keySiz)->GetValue();
        cubicVec3SegDat->m_outTangent0 = cjVec3::ZERO;
        cubicVec3SegDat->m_a0 = cjVec3::ZERO;
        cubicVec3SegDat->m_b0 = cjVec3::ZERO;
    }
    else
    {
        if(keyType == cjAniKey::EKT_STEPKEY)
            evalSPDat->SetStepFunction(cjTrue);

		cjAssert(evalSPDat->GetSegMinTime() == CJ_FLT_INFINITY);
        cjAssert(evalSPDat->GetSegMaxTime() == CJ_FLT_INVALID);

        evalSPDat->SetFillFunc(&CubicVec3FillFunc);

        cjAniSPad::CubicVec3Fill* cubicVec3FillDat = (cjAniSPad::CubicVec3Fill*)evalSPDat->GetFillDat();
        cjAssert(cubicVec3FillDat);

        cubicVec3FillDat->m_lastIndex = 0;
        cubicVec3FillDat->m_keys = keys;
        cubicVec3FillDat->m_keyCount = keyCount;
        cubicVec3FillDat->m_keyType = keyType;
        cubicVec3FillDat->m_keySiz = keySiz;
    }

    return cjTrue;
}

void cjVec3SEvaluator::Collapse()
{
    if(m_spVec3ScaleDat)
    {
        cjUINT32 keyCount; cjAniKey::EKEY_TYPE keyType; cjBYTE keySiz;
        cjAniKeyVec3* keys = m_spVec3ScaleDat->GetKeys(keyCount, keyType, keySiz);

        if(keyCount == 0)
        {
            m_spVec3ScaleDat = cjNULL;
        }
        else
        {
            cjAniKey::PFIsPosed pfIsPosed = cjAniKeyVec3::GetIsPosedKeyPF(keyType);

            if(pfIsPosed(keys, keyCount))
            {
                cjAniKeyVec3* linVec3Keys = (cjAniKeyVec3*)new cjAniKeyLinVec3[1];
                linVec3Keys[0].SetTime(0.0f);

                cjVec3 value = keys->GetKey(0, keySiz)->GetValue();

                linVec3Keys[0].SetValue(value);
                m_spVec3ScaleDat->ReplaceKeys(linVec3Keys, 1, cjAniKey::EKT_LINKEY);
            }
        }
    }

    SetEvalChannelTypes();
}

cjUINT32 cjVec3SEvaluator::GetKeyCount(cjUINT32 ) const 
{
    if(!m_spVec3ScaleDat)
        return 0;
    else
        return m_spVec3ScaleDat->GetKeyCount();
}

cjAniKey::EKEY_TYPE cjVec3SEvaluator::GetKeyType(cjUINT32 ) const
{
    if(!m_spVec3ScaleDat)
        return cjAniKey::EKT_NOINTERP;

    cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE keySiz;
    m_spVec3ScaleDat->GetKeys(keyCount, keyType, keySiz);

    return keyType; 
}

cjAniKey* cjVec3SEvaluator::GetKeyAry(cjUINT32 ) const
{
    if(!m_spVec3ScaleDat)
        return cjNULL;

    cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE keySiz;
    cjAniKeyVec3* keys = m_spVec3ScaleDat->GetKeys(keyCount, keyType, keySiz);

    return keys; 
}

cjBYTE cjVec3SEvaluator::GetKeySiz(cjUINT32 ) const
{
    if(!m_spVec3ScaleDat)
        return 0;

    cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE keySiz;
    m_spVec3ScaleDat->GetKeys(keyCount, keyType, keySiz);

    return keySiz; 
}

void cjVec3SEvaluator::SetVec3ScaleDat(cjAniVec3Dat* dat)
{
    m_spVec3ScaleDat = dat;
    SetEvalChannelTypes();
}

cjAniKeyVec3* cjVec3SEvaluator::GetKeys(cjUINT32& keyCount, cjAniKeyVec3::EKEY_TYPE& keyType, cjBYTE& keySiz) const
{
    if(m_spVec3ScaleDat)
        return m_spVec3ScaleDat->GetKeys(keyCount, keyType, keySiz);

    keyCount = 0; keyType = cjAniKeyVec3::EKT_NOINTERP; keySiz = 0;

    return cjNULL;
}

void cjVec3SEvaluator::ReplaceKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType)
{
    if(keyCount > 0)
    {
        if(!m_spVec3ScaleDat)
            m_spVec3ScaleDat = new cjAniVec3Dat;

        m_spVec3ScaleDat->ReplaceKeys(keys, keyCount, keyType);
    }
    else if(m_spVec3ScaleDat)
    {
        m_spVec3ScaleDat->ReplaceKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
    }

    SetEvalChannelTypes();
}

void cjVec3SEvaluator::SetKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType)
{
    if(keyCount > 0)
    {
        if(!m_spVec3ScaleDat)
            m_spVec3ScaleDat = new cjAniVec3Dat;

        m_spVec3ScaleDat->SetKeys(keys, keyCount, keyType);
    }
    else if(m_spVec3ScaleDat)
    {
        m_spVec3ScaleDat->SetKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
    }

    SetEvalChannelTypes();
}

cjBool cjVec3SEvaluator::CubicVec3FillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat)
{
    cjAniSPad::CubicVec3Fill* cubicVec3FillDat = (cjAniSPad::CubicVec3Fill*)evalSPDat->GetFillDat();
    cjAniSPad::CubicVec3Seg* cubicVec3SegDat = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();

	cjFLOAT minTime, maxTime;

    cjAniKeyVec3::GenCubicCoefs(etime, cubicVec3FillDat->m_keys, cubicVec3FillDat->m_keyType, 
		cubicVec3FillDat->m_keyCount, cubicVec3FillDat->m_lastIndex, cubicVec3FillDat->m_keySiz, 
        minTime, maxTime, cubicVec3SegDat->m_val0, cubicVec3SegDat->m_outTangent0, 
		cubicVec3SegDat->m_a0, cubicVec3SegDat->m_b0);

    cubicVec3SegDat->m_stTime = minTime;
	cubicVec3SegDat->m_invDTime = (maxTime != minTime) ? 1.0f / (maxTime - minTime) : 0.0f;

    evalSPDat->SetSegTimeRange(minTime, maxTime);

    return cjTrue;
}

void cjVec3SEvaluator::SetEvalChannelTypes()
{
    if(m_spVec3ScaleDat && m_spVec3ScaleDat->GetKeyCount() > 0)
    {
        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_Vec3S;

        if(m_spVec3ScaleDat->GetKeyCount() == 1)
            m_channelItem[cjAniEvalChan_Base] |= cjAniEvalChanItemMask_Constant;
    }
    else
    {
        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_NoData;
    }
}


//
// cjKeyBasedEvaluator
//  - cjVec3PEvaluator
//

CJSER_IMPL_CLASS(cjVec3PEvaluator, cjKeyBasedEvaluator)

CJSER_IMPL_SERIALIZE_BEGIN(cjVec3PEvaluator)
	
	if(ser.IsQuerySerObjAndStr())
	{
		if(m_spVec3PosDat)
			(*m_spVec3PosDat) >> ser;

		return;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		cjSerID serID;
	
		{
			if(ser.IsSaving())
				serID = ser.GetSerObjID(m_spVec3PosDat);

			cjSerID_Serialize(ser, serID);

			if(ser.IsLoading())
				m_spVec3PosDat = ser.GetSerObj<cjAniVec3Dat>(serID);
		}
	}
	
CJSER_IMPL_SERIALIZE_END(cjVec3PEvaluator)

CJSER_IMPL_CLONE(cjVec3PEvaluator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjVec3PEvaluator)
	d->m_spVec3PosDat = m_spVec3PosDat;
CJSER_IMPL_CLONE_PROPCOPY_END(cjVec3PEvaluator)


cjVec3PEvaluator::cjVec3PEvaluator()
{
}

cjVec3PEvaluator::cjVec3PEvaluator(cjAniVec3Dat* dat)    
{
	m_spVec3PosDat = dat;
    SetEvalChannelTypes();
}

cjVec3PEvaluator::~cjVec3PEvaluator()
{
}

cjBool cjVec3PEvaluator::GetChannelPosedValue(cjUINT32 channel, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Pos);
    cjAssert(rst);

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChannelItemInvalid(channel));

        cjUINT32 keyCount; cjAniKey::EKEY_TYPE keyType; cjBYTE keySiz;
        cjAniKeyVec3* keys = m_spVec3PosDat->GetKeys(keyCount, keyType, keySiz);
        cjAssert(keyCount == 1);

        *(cjVec3*)rst = keys->GetKey(0, keySiz)->GetValue();

        return cjTrue;
    }

    return cjFalse;
}

cjBool cjVec3PEvaluator::UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Pos);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);

    if(IsChannelItemInvalid(channel))
        return cjFalse;

    if(!evalSPDat->IsSegDatValid(etime))
    {
        cjAniEvalSPadFillFunc pfFill = evalSPDat->GetFillFunc();

        if(!pfFill || !(*pfFill)(etime, evalSPDat))
            return cjFalse;
    }

    cjAniSPad::CubicVec3Seg* cubicVec3SegDat = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();

	cjFLOAT ratio = (etime - cubicVec3SegDat->m_stTime) * cubicVec3SegDat->m_invDTime; 
	*(cjVec3*)rst = cubicVec3SegDat->m_val0 + (cubicVec3SegDat->m_outTangent0 
		+ (cubicVec3SegDat->m_a0 + cubicVec3SegDat->m_b0 * ratio) * ratio) * ratio;

    return cjTrue;
}

void cjVec3PEvaluator::CacheKeys(cjFLOAT stTime, cjFLOAT enTime)
{
    if(m_spVec3PosDat)
        m_spVec3PosDat->CacheKeys(stTime, enTime);
}

cjAniEvaluator* cjVec3PEvaluator::GetEvaluator(cjFLOAT stTime, cjFLOAT enTime)
{
    cjVec3PEvaluator* eval = (cjVec3PEvaluator*)cjKeyBasedEvaluator::GetEvaluator(stTime, enTime);

    if(m_spVec3PosDat)
    {
        cjAniVec3DatRPtr spNewDat = m_spVec3PosDat->GetDat(stTime, enTime);
        eval->SetVec3Dat(spNewDat);
    }

    return eval;
}

cjBool cjVec3PEvaluator::GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, 
	cjUINT32& fillSiz, cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const
{
    cjAssert(channel == cjAniEvalChan_Pos);
    cjBool isPosed = IsChannelItemPosed(channel);

    if(IsChannelItemInvalid(channel) || (isPosed && !forceAlwaysUpdate && !IsAlwaysUpdate()))
        return cjFalse;

    fillSiz = isPosed ? 0 : sizeof(cjAniSPad::CubicVec3Fill);

    isSharedFillDat = cjFalse;
    spadBlk = cjAniSPADB_CubicVec3PSeg;
    bsBasisDat = 0;

    return cjTrue;
}

cjBool cjVec3PEvaluator::InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, 
	cjBool initSharedDat, cjRefObjNameHash* nodeHash, cjAniPoseBufHandle pbHandle) const
{
    cjAssert(channel == cjAniEvalChan_Pos);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(!IsChannelItemInvalid(channel));
    cjAssert(m_spVec3PosDat);

    cjAniSPad::CubicVec3Seg* cubicVec3SegDat = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();
    cjAssert(cubicVec3SegDat);

    cubicVec3SegDat->m_hdr.m_lod = evalSPDat->GetLOD();

    if(IsReferenced())
    {
        cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
        cubicVec3SegDat->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
    }
    else
    {
        cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Vec3P);
        cubicVec3SegDat->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
    }

    cjUINT32 keyCount; cjAniKey::EKEY_TYPE keyType; cjBYTE keySiz;
    cjAniKeyVec3* keys = m_spVec3PosDat->GetKeys(keyCount, keyType, keySiz);
    cjAssert(keys);
    cjAssert(keyCount > 0);

    cjBool isPosed = IsChannelItemPosed(channel);
    if(isPosed)
    {
        evalSPDat->SetSegTimeRange(0.0f, CJ_FLT_INFINITY);
        cjAssert(evalSPDat->GetFillFunc() == cjNULL);

        cubicVec3SegDat->m_stTime = 0.0f;
        cubicVec3SegDat->m_invDTime = 0.0f;
        cubicVec3SegDat->m_val0 = keys->GetKey(0, keySiz)->GetValue();
        cubicVec3SegDat->m_outTangent0 = cjVec3::ZERO;
        cubicVec3SegDat->m_a0 = cjVec3::ZERO;
        cubicVec3SegDat->m_b0 = cjVec3::ZERO;
    }
    else
    {
        if(keyType == cjAniKey::EKT_STEPKEY)
            evalSPDat->SetStepFunction(cjTrue);

		cjAssert(evalSPDat->GetSegMinTime() == CJ_FLT_INFINITY);
        cjAssert(evalSPDat->GetSegMaxTime() == CJ_FLT_INVALID);
        evalSPDat->SetFillFunc(&CubicVec3FillFunc);

        cjAniSPad::CubicVec3Fill* cubicVec3FillDat = (cjAniSPad::CubicVec3Fill*)evalSPDat->GetFillDat();
        cjAssert(cubicVec3FillDat);

        cubicVec3FillDat->m_lastIndex = 0;
        cubicVec3FillDat->m_keys = keys;
        cubicVec3FillDat->m_keyCount = keyCount;
        cubicVec3FillDat->m_keyType = keyType;
        cubicVec3FillDat->m_keySiz = keySiz;
    }

    return cjTrue;
}

void cjVec3PEvaluator::Collapse()
{
    if(m_spVec3PosDat)
    {
        cjUINT32 keyCount; cjAniKey::EKEY_TYPE keyType; cjBYTE keySiz;
        cjAniKeyVec3* keys = m_spVec3PosDat->GetKeys(keyCount, keyType, keySiz);

        if(keyCount == 0)
        {
            m_spVec3PosDat = cjNULL;
        }
        else
        {
            cjAniKey::PFIsPosed pfIsPosed = cjAniKeyVec3::GetIsPosedKeyPF(keyType);

            if(pfIsPosed(keys, keyCount))
            {
                cjAniKeyVec3* linVec3Keys = (cjAniKeyVec3*)new cjAniKeyLinVec3[1];
                linVec3Keys[0].SetTime(0.0f);

                cjVec3 value = keys->GetKey(0, keySiz)->GetValue();

                linVec3Keys[0].SetValue(value);
                m_spVec3PosDat->ReplaceKeys(linVec3Keys, 1, cjAniKey::EKT_LINKEY);
            }
        }
    }

    SetEvalChannelTypes();
}

cjUINT32 cjVec3PEvaluator::GetKeyCount(cjUINT32 ) const 
{
    if(!m_spVec3PosDat)
        return 0;
    else
        return m_spVec3PosDat->GetKeyCount();
}

cjAniKey::EKEY_TYPE cjVec3PEvaluator::GetKeyType(cjUINT32 ) const
{
    if(!m_spVec3PosDat)
        return cjAniKey::EKT_NOINTERP;

    cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE keySiz;
    m_spVec3PosDat->GetKeys(keyCount, keyType, keySiz);

    return keyType; 
}

cjAniKey* cjVec3PEvaluator::GetKeyAry(cjUINT32 ) const
{
    if(!m_spVec3PosDat)
        return cjNULL;

    cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE keySiz;
    cjAniKeyVec3* keys = m_spVec3PosDat->GetKeys(keyCount, keyType, keySiz);

    return keys; 
}

cjBYTE cjVec3PEvaluator::GetKeySiz(cjUINT32 ) const
{
    if(!m_spVec3PosDat)
        return 0;

    cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE keySiz;
    m_spVec3PosDat->GetKeys(keyCount, keyType, keySiz);

    return keySiz; 
}

void cjVec3PEvaluator::SetVec3Dat(cjAniVec3Dat* dat)
{
    m_spVec3PosDat = dat;
    SetEvalChannelTypes();
}

cjAniKeyVec3* cjVec3PEvaluator::GetKeys(cjUINT32& keyCount, cjAniKeyVec3::EKEY_TYPE& keyType, cjBYTE& keySiz) const
{
    if(m_spVec3PosDat)
        return m_spVec3PosDat->GetKeys(keyCount, keyType, keySiz);

    keyCount = 0; keyType = cjAniKeyVec3::EKT_NOINTERP; keySiz = 0;

    return cjNULL;
}

void cjVec3PEvaluator::ReplaceKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType)
{
    if(keyCount > 0)
    {
        if(!m_spVec3PosDat)
            m_spVec3PosDat = new cjAniVec3Dat;

        m_spVec3PosDat->ReplaceKeys(keys, keyCount, keyType);
    }
    else if(m_spVec3PosDat)
    {
        m_spVec3PosDat->ReplaceKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
    }

    SetEvalChannelTypes();
}

void cjVec3PEvaluator::SetKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType)
{
    if(keyCount > 0)
    {
        if(!m_spVec3PosDat)
            m_spVec3PosDat = new cjAniVec3Dat;

        m_spVec3PosDat->SetKeys(keys, keyCount, keyType);
    }
    else if(m_spVec3PosDat)
    {
        m_spVec3PosDat->SetKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
    }

    SetEvalChannelTypes();
}

cjBool cjVec3PEvaluator::CubicVec3FillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat)
{
    cjAniSPad::CubicVec3Fill* cubicVec3FillDat = (cjAniSPad::CubicVec3Fill*)evalSPDat->GetFillDat();
    cjAniSPad::CubicVec3Seg* cubicVec3SegDat = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();

	cjFLOAT minTime, maxTime;
    cjAniKeyVec3::GenCubicCoefs(etime, cubicVec3FillDat->m_keys, cubicVec3FillDat->m_keyType, 
		cubicVec3FillDat->m_keyCount, cubicVec3FillDat->m_lastIndex, cubicVec3FillDat->m_keySiz, 
        minTime, maxTime, cubicVec3SegDat->m_val0, cubicVec3SegDat->m_outTangent0, 
		cubicVec3SegDat->m_a0, cubicVec3SegDat->m_b0);
    cubicVec3SegDat->m_stTime = minTime;
	cubicVec3SegDat->m_invDTime = (maxTime != minTime) ? 1.0f / (maxTime - minTime) : 0.0f;

    evalSPDat->SetSegTimeRange(minTime, maxTime);

    return cjTrue;
}

void cjVec3PEvaluator::SetEvalChannelTypes()
{
    if(m_spVec3PosDat && m_spVec3PosDat->GetKeyCount() > 0)
    {
        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_Vec3P;

        if(m_spVec3PosDat->GetKeyCount() == 1)
            m_channelItem[cjAniEvalChan_Base] |= cjAniEvalChanItemMask_Constant;
    }
    else
    {
        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_NoData;
    }
}


//
// cjBoolEvaluator
//

CJSER_IMPL_CLASS(cjBoolEvaluator, cjKeyBasedEvaluator);

CJSER_IMPL_SERIALIZE_BEGIN(cjBoolEvaluator)
	
	if(ser.IsQuerySerObjAndStr())
	{
		if(m_spBoolDat)
			(*m_spBoolDat) >> ser;

		return;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		cjSerID serID;
	
		{
			if(ser.IsSaving())
				serID = ser.GetSerObjID(m_spBoolDat);

			cjSerID_Serialize(ser, serID);

			if(ser.IsLoading())
				m_spBoolDat = ser.GetSerObj<cjAniBoolDat>(serID);
		}
	}
	
CJSER_IMPL_SERIALIZE_END(cjBoolEvaluator)

CJSER_IMPL_CLONE(cjBoolEvaluator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjBoolEvaluator)
	d->m_spBoolDat = m_spBoolDat;
CJSER_IMPL_CLONE_PROPCOPY_END(cjBoolEvaluator)


cjBoolEvaluator::cjBoolEvaluator()
{
}

cjBoolEvaluator::cjBoolEvaluator(cjAniBoolDat* dat)
{
	m_spBoolDat = dat;
    SetEvalChannelTypes();
}

cjBoolEvaluator::~cjBoolEvaluator()
{
}

cjBool cjBoolEvaluator::GetChannelPosedValue(cjUINT32 channel, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChannelItemInvalid(channel));
        cjAssert(m_spBoolDat);

        cjUINT32 keyCount; cjAniKey::EKEY_TYPE keyType; cjBYTE keySiz;
        cjAniKeyBool* keys = m_spBoolDat->GetKeys(keyCount, keyType, keySiz);
        cjAssert(keyCount == 1);

        *(cjFLOAT*)rst = keys->GetKey(0, keySiz)->GetValue() ? 1.0f : 0.0f;
        return cjTrue;
    }

    return cjFalse;
}

cjBool cjBoolEvaluator::UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(evalSPDat);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(rst);

    if(IsChannelItemInvalid(channel))
        return cjFalse;

	if(!evalSPDat->IsSegDatValid(etime))
    {
		cjAniEvalSPadFillFunc pfFillFunc = evalSPDat->GetFillFunc();

        if(!pfFillFunc || !(*pfFillFunc)(etime, evalSPDat))
            return cjFalse;
    }

    cjAniSPad::CBoolSeg* cboolSeg = (cjAniSPad::CBoolSeg*)evalSPDat->GetSegDat();
    *(cjFLOAT*)rst = cboolSeg->m_val0;

    return cjTrue;
}

void cjBoolEvaluator::CacheKeys(cjFLOAT stTime, cjFLOAT enTime)
{
    if(m_spBoolDat)
		m_spBoolDat->CacheKeys(stTime, enTime);
}

cjAniEvaluator* cjBoolEvaluator::GetEvaluator(cjFLOAT stTime, cjFLOAT enTime)
{
    cjBoolEvaluator* eval = (cjBoolEvaluator*)cjKeyBasedEvaluator::GetEvaluator(stTime, enTime);

    if(m_spBoolDat)
    {
		cjAniBoolDatRPtr spNewBoolData = m_spBoolDat->GetDat(stTime, enTime);
        eval->SetBoolDat(spNewBoolData);
    }

    return eval;
}

cjBool cjBoolEvaluator::GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, 
	cjUINT32& fillSiz, cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const
{
    cjAssert(channel == cjAniEvalChan_Base);

    cjBool isPosed = IsChannelItemPosed(channel);

    if(IsChannelItemInvalid(channel) || (isPosed && !forceAlwaysUpdate && !IsAlwaysUpdate()))
        return cjFalse;

    fillSiz = isPosed ? 0 : sizeof(cjAniSPad::CBoolFill);
    isSharedFillDat = cjFalse;
    spadBlk = cjAniSPADB_CBoolSeg;
    bsBasisDat = 0;

    return cjTrue;
}

cjBool cjBoolEvaluator::InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat,
	cjBool initSharedDat, cjRefObjNameHash* nodeHash, cjAniPoseBufHandle pbHandle) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(!IsChannelItemInvalid(channel));
    cjAssert(m_spBoolDat);

    cjAniSPad::CBoolSeg* cboolSeg = (cjAniSPad::CBoolSeg*)evalSPDat->GetSegDat();
    cboolSeg->m_hdr.m_lod = evalSPDat->GetLOD();

    if(IsReferenced())
    {
        cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
        cboolSeg->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
    }
    else
    {
        cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Bool);
        cboolSeg->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
    }

    cjUINT32 keyCount; cjAniKey::EKEY_TYPE keyType; cjBYTE keySiz;
    cjAniKeyBool* keys = m_spBoolDat->GetKeys(keyCount, keyType, keySiz);
    cjAssert(keys);
    cjAssert(keyCount > 0);

    cjBool isPosed = IsChannelItemPosed(channel);
    if(isPosed)
    {
        evalSPDat->SetSegTimeRange(0.0f, CJ_FLT_INFINITY);
        cjAssert(evalSPDat->GetFillFunc() == cjNULL);

        cboolSeg->m_val0 = keys->GetKey(0, keySiz)->GetValue() ? 1.0f : 0.0f;
    }
    else
    {
        if(keyType == cjAniKey::EKT_STEPKEY)
            evalSPDat->SetStepFunction(cjTrue);

		cjAssert(evalSPDat->GetSegMinTime() == CJ_FLT_INFINITY);
        cjAssert(evalSPDat->GetSegMaxTime() == CJ_FLT_INVALID);

        evalSPDat->SetFillFunc(&ConstantBoolFillFunc);

        cjAniSPad::CBoolFill* fillDat = (cjAniSPad::CBoolFill*)evalSPDat->GetFillDat();
        cjAssert(fillDat);
		fillDat->m_lastIndex = 0;
        fillDat->m_keys = keys;
        fillDat->m_keyCount = keyCount;
        fillDat->m_keyType = keyType;
        fillDat->m_keySiz = keySiz;
    }

    return cjTrue;
}


void cjBoolEvaluator::Collapse()
{
    if(m_spBoolDat)
    {
        cjUINT32 keyCount; cjAniKey::EKEY_TYPE keyType; cjBYTE keySiz;
        cjAniKeyBool* keys = m_spBoolDat->GetKeys(keyCount, keyType, keySiz);

        if(!keyCount)
        {
            m_spBoolDat = cjNULL;
        }
        else
        {
			cjAniKey::PFIsPosed pfIsPosed = cjAniKeyBool::GetIsPosedKeyPF(keyType);
            if(pfIsPosed(keys, keyCount))
            {
                cjAniKeyBool* stepKeys = (cjAniKeyBool*)new cjAniKeyStepBool[1];
                stepKeys[0].SetTime(0.0f);

                cjBool value = keys->GetKey(0, keySiz)->GetValue();

                stepKeys[0].SetValue(value);
				m_spBoolDat->ReplaceKeys(stepKeys, 1, cjAniKey::EKT_STEPKEY);
            }
        }
    }

    SetEvalChannelTypes();
}

cjUINT32 cjBoolEvaluator::GetKeyCount(cjUINT32) const
{
    if(!m_spBoolDat)
        return 0;
    else
		return m_spBoolDat->GetKeyCount();
}

cjAniKey::EKEY_TYPE cjBoolEvaluator::GetKeyType(cjUINT32) const
{
    if(!m_spBoolDat)
        return cjAniKey::EKT_NOINTERP;

    cjUINT32 keyCount; cjAniKeyBool::EKEY_TYPE keyType; cjBYTE keySiz;
	m_spBoolDat->GetKeys(keyCount, keyType, keySiz);

    return keyType; 
}

cjAniKey* cjBoolEvaluator::GetKeyAry(cjUINT32 ) const 
{
    if(!m_spBoolDat)
        return cjNULL;

    cjUINT32 keyCount; cjAniKeyBool::EKEY_TYPE keyType; cjBYTE keySiz;
    cjAniKeyBool* keys = m_spBoolDat->GetKeys(keyCount, keyType, keySiz);

    return keys; 
}

cjBYTE cjBoolEvaluator::GetKeySiz(cjUINT32) const 
{
    if(!m_spBoolDat)
        return 0;

    cjUINT32 keyCount; cjAniKeyBool::EKEY_TYPE keyType; cjBYTE keySiz;
    m_spBoolDat->GetKeys(keyCount, keyType, keySiz);

    return keySiz; 
}

cjAniKeyBool* cjBoolEvaluator::GetKeys(cjUINT32& keyCount, cjAniKeyBool::EKEY_TYPE& keyType, cjBYTE& keySiz) const
{
    if(m_spBoolDat)
        return m_spBoolDat->GetKeys(keyCount, keyType, keySiz);

    keyCount = 0;
    keyType = cjAniKeyBool::EKT_NOINTERP;
    keySiz = 0;

    return cjNULL;
}

void cjBoolEvaluator::ReplaceKeys(cjAniKeyBool* keys, cjUINT32 keyCount, cjAniKeyBool::EKEY_TYPE keyType)
{
    if(keyCount > 0)
    {
        if(!m_spBoolDat)
            m_spBoolDat = new cjAniBoolDat;

        m_spBoolDat->ReplaceKeys(keys, keyCount, keyType);
    }
    else if(m_spBoolDat)
    {
        m_spBoolDat->ReplaceKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
    }

    SetEvalChannelTypes();
}

void cjBoolEvaluator::SetKeys(cjAniKeyBool* keys, cjUINT32 keyCount, cjAniKeyBool::EKEY_TYPE keyType)
{
    if(keyCount > 0)
    {
        if(!m_spBoolDat)
            m_spBoolDat = new cjAniBoolDat;

        m_spBoolDat->SetKeys(keys, keyCount, keyType);
    }
    else if(m_spBoolDat)
    {
        m_spBoolDat->SetKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
    }

    SetEvalChannelTypes();
}

cjBool cjBoolEvaluator::ConstantBoolFillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat)
{
    cjAssert(evalSPDat);

    cjAniSPad::CBoolFill* fillDat = (cjAniSPad::CBoolFill*)evalSPDat->GetFillDat();
    cjAssert(fillDat->m_keys);
    cjAssert(fillDat->m_keyCount > 0);

    cjAniSPad::CBoolSeg* cboolSeg = (cjAniSPad::CBoolSeg*)evalSPDat->GetSegDat();

	cjFLOAT minTime, maxTime;
    cjFLOAT outTangent0 , a0, b0;

    cjAniKeyBool::GenCubicCoefs(etime, fillDat->m_keys, fillDat->m_keyType, fillDat->m_keyCount, 
        fillDat->m_lastIndex, fillDat->m_keySiz, minTime, maxTime, cboolSeg->m_val0, outTangent0, a0, b0);

    cjAssert(outTangent0 == 0.0f);
    cjAssert(a0 == 0.0f);
    cjAssert(b0 == 0.0f);

    evalSPDat->SetSegTimeRange(minTime, maxTime);

    return cjTrue;
}

void cjBoolEvaluator::SetEvalChannelTypes()
{
    if(m_spBoolDat && m_spBoolDat->GetKeyCount() > 0)
    {
        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_Bool;

        if(m_spBoolDat->GetKeyCount() == 1)
        {
            m_channelItem[cjAniEvalChan_Base] |= cjAniEvalChanItemMask_Constant;
        }
    }
    else
    {
        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_NoData;
    }
}


//
// cjBoolTimelineEvaluator
//

CJSER_IMPL_CLASS(cjBoolTimelineEvaluator, cjBoolEvaluator);

CJSER_IMPL_SERIALIZE_BEGIN(cjBoolTimelineEvaluator)
	
	if(ser.IsQuerySerObjAndStr())
	{
		return;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
	}
	
CJSER_IMPL_SERIALIZE_END(cjBoolTimelineEvaluator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjBoolTimelineEvaluator)
CJSER_IMPL_CLONE_PROPCOPY_END(cjBoolTimelineEvaluator)

CJSER_IMPL_CLONE(cjBoolTimelineEvaluator)


cjBoolTimelineEvaluator::cjBoolTimelineEvaluator()
{
}

cjBoolTimelineEvaluator::cjBoolTimelineEvaluator(cjAniBoolDat* dat)
	: cjBoolEvaluator(dat)
{
}

cjBoolTimelineEvaluator::~cjBoolTimelineEvaluator()
{
}

cjBool cjBoolTimelineEvaluator::GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, 
	cjUINT32& fillSiz, cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const
{
    cjAssert(channel == cjAniEvalChan_Base);

    if(!cjBoolEvaluator::GetSPadInfo(channel, forceAlwaysUpdate, nodeHash, fillSiz, isSharedFillDat, spadBlk, bsBasisDat))
        return cjFalse;

    if(fillSiz > 0)
        fillSiz = sizeof(cjAniSPad::CBoolTimelineFill);

    return cjTrue;
}

cjBool cjBoolTimelineEvaluator::InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, 
	cjBool initSharedDat, cjRefObjNameHash* nodeHash, cjAniPoseBufHandle pbHandle) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(!IsChannelItemInvalid(channel));
    cjAssert(m_spBoolDat);

    if(!cjBoolEvaluator::InitSPadDat(channel, evalSPDat, bsBasisDat, initSharedDat, nodeHash, pbHandle))
        return cjFalse;

    if(IsChannelItemPosed(channel))
    {
        cjAssert(evalSPDat->GetSegMinTime() == 0.0f);
        cjAssert(evalSPDat->GetSegMaxTime() == CJ_FLT_INFINITY);
        cjAssert(evalSPDat->GetFillFunc() == cjNULL);

        return cjTrue;
    }

    evalSPDat->SetFillFunc(&ConstantBoolTimelineFillFunc);

    cjAniSPad::CBoolTimelineFill* fillDat = (cjAniSPad::CBoolTimelineFill*)evalSPDat->GetFillDat();
    cjAssert((void*)&fillDat->m_baseDat == (void*)fillDat);

	fillDat->m_prevIndex = 0;
	fillDat->m_prevValue = cjFalse;

    return cjTrue;
}

cjBool cjBoolTimelineEvaluator::ConstantBoolTimelineFillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat)
{
    cjAssert(evalSPDat);

    cjAniSPad::CBoolTimelineFill* fillDat = (cjAniSPad::CBoolTimelineFill*)evalSPDat->GetFillDat();
	cjAniSPad::CBoolFill* baseDat = &fillDat->m_baseDat;
    cjAssert(baseDat->m_keys);
    cjAssert(baseDat->m_keyCount > 0);

    fillDat->m_prevIndex = baseDat->m_lastIndex;
    cjFLOAT minTime, maxTime;
    cjFLOAT value0, outTangent0, a0, b0;

    cjAniKeyBool::GenCubicCoefs(etime, baseDat->m_keys, baseDat->m_keyType, baseDat->m_keyCount, 
        baseDat->m_lastIndex, baseDat->m_keySiz, minTime, maxTime, value0, outTangent0, a0, b0);

    cjAssert(outTangent0 == 0.0f);
    cjAssert(a0 == 0.0f);
    cjAssert(b0 == 0.0f);

    cjUINT32 currentIdx = baseDat->m_lastIndex;

    cjBool interpValue = (value0 >= 0.5f);
    cjBool boolValue = interpValue;

	if(boolValue == fillDat->m_prevValue)
    {
        cjBool valueToFind = !boolValue;

        if(fillDat->m_prevIndex != currentIdx && fillDat->m_prevIndex+1 > currentIdx)
        {
            for(cjUINT32 i=fillDat->m_prevIndex+1; i<baseDat->m_keyCount; i++)
            {
                if(baseDat->m_keys->GetKey(i, baseDat->m_keySiz)->GetValue() == valueToFind)
                {
                    boolValue = valueToFind;
                    break;
                }
            }

            if(boolValue != valueToFind)
            {
                for(cjUINT32 i=0; i<currentIdx; i++)
                {
                    if(baseDat->m_keys->GetKey(i, baseDat->m_keySiz)->GetValue() == valueToFind)
                    {
                        boolValue = valueToFind;
                        break;
                    }
                }
            }
        }
        else
        {
            for(cjUINT32 i=fillDat->m_prevIndex+1; i<currentIdx && i<baseDat->m_keyCount; i++)
            {
                if(baseDat->m_keys->GetKey(i, baseDat->m_keySiz)->GetValue() == valueToFind)
                {
                    boolValue = valueToFind;
                    break;
                }
            }
        }
    }

	fillDat->m_prevValue = interpValue;

    cjAniSPad::CBoolSeg* constantSeg = (cjAniSPad::CBoolSeg*)evalSPDat->GetSegDat();
    constantSeg->m_val0 = boolValue ? 1.0f : 0.0f;

    if(boolValue == interpValue)
        evalSPDat->SetSegTimeRange(minTime, maxTime);
    else
        evalSPDat->SetSegTimeRange(etime, etime);

    return cjTrue;
}


//
// cjBSplineEvaluator
//

CJSER_IMPL_CLASS(cjBSplineEvaluator, cjAniEvaluator);

CJSER_IMPL_SERIALIZE_BEGIN(cjBSplineEvaluator)
	
	if(ser.IsQuerySerObjAndStr())
	{
		if(m_spDat)
			(*m_spDat) >> ser;

		if(m_spBasisDat)
			(*m_spBasisDat) >> ser;

		return;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		ser << m_stTime;
		ser << m_enTime;

		ser.SerializeSerObjPtr(&m_spDat);
		ser.SerializeSerObjPtr(&m_spBasisDat);
	}
	
CJSER_IMPL_SERIALIZE_END(cjBSplineEvaluator)

CJSER_IMPL_SERIALIZE_POST_BEGIN(cjBSplineEvaluator)

	if(ser.IsLoading())
	{
		ser.PostSerializeSerObjPtr(m_spDat);
		ser.PostSerializeSerObjPtr(m_spBasisDat);
	}

CJSER_IMPL_SERIALIZE_POST_END(cjBSplineEvaluator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjBSplineEvaluator)
	d->m_stTime = m_stTime;
	d->m_enTime = m_enTime;
	d->m_spDat = m_spDat;

	// immediate data, so should not be shared.
	d->m_spBasisDat = cjNULL;

	if(m_spBasisDat)
		d->m_spBasisDat = (cjBSplineBasisF3Dat*)m_spBasisDat->_CloneShared(cloner);
	
CJSER_IMPL_CLONE_PROPCOPY_END(cjBSplineEvaluator)


cjBSplineEvaluator::cjBSplineEvaluator()
{
	m_stTime = CJ_FLT_INFINITY;
    m_enTime = CJ_FLT_INVALID;
}

cjBSplineEvaluator::cjBSplineEvaluator(cjBSplineDat* dat, cjBSplineBasisF3Dat* bsBasisDat) 
{
	m_stTime = CJ_FLT_INFINITY;
    m_enTime = CJ_FLT_INVALID;
	m_spDat = dat;
	m_spBasisDat = bsBasisDat;
}

cjBSplineEvaluator::~cjBSplineEvaluator()
{
}

void cjBSplineEvaluator::GetValidTimeRange(cjFLOAT& stTime, cjFLOAT& enTime) const
{
    if(stTime == CJ_FLT_INFINITY && enTime == CJ_FLT_INVALID)
    {
        stTime = 0.0f;
        enTime = 0.0f;
    }
    else
    {
        stTime = m_stTime;
        enTime = m_enTime;
    }
}

void cjBSplineEvaluator::CacheKeys(cjFLOAT, cjFLOAT)
{
    cjAssert(!"incorrect CacheKeys call for cjBSplineEvaluator");
}

cjAniEvaluator* cjBSplineEvaluator::GetEvaluator(cjFLOAT, cjFLOAT)
{
    cjAssert(!"incorrect GetEvaluator call for cjBSplineEvaluator");
    return cjNULL;
}

cjUINT32 cjBSplineEvaluator::GetAllocSiz(cjUINT32 channel) const
{
    cjAssert(channel < GetChannelCount());

    if(IsCompressedCP())
        return GetCPCount(channel) * GetFloatValCount(channel) * sizeof(cjSWORD);
    else
        return GetCPCount(channel) * GetFloatValCount(channel) * sizeof(cjFLOAT);
}

cjUINT32 cjBSplineEvaluator::GetCPCount(cjUINT32 channel) const
{
    if(channel >= GetChannelCount())
        return 0;

    if(m_spDat == 0 || cjBSplineDat::INVALID_BSOFFSET == GetCPOffset(channel))
        return 0;

    cjBSplineBasisF3& basis = m_spBasisDat->GetBasis();

	return basis.GetControlPointCount();
}


//
// cjBSColorEvaluator
//

CJSER_IMPL_CLASS(cjBSColorEvaluator, cjBSplineEvaluator);

CJSER_IMPL_SERIALIZE_BEGIN(cjBSColorEvaluator)
	
	if(ser.IsQuerySerObjAndStr())
	{
		return;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		ser << m_valueCPOffset;
	}
	
CJSER_IMPL_SERIALIZE_END(cjBSColorEvaluator)

CJSER_IMPL_CLONE(cjBSColorEvaluator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjBSColorEvaluator)
	d->m_valueCPOffset = m_valueCPOffset;	
CJSER_IMPL_CLONE_PROPCOPY_END(cjBSColorEvaluator)


cjBSColorEvaluator::cjBSColorEvaluator()
{
    m_valueCPOffset = cjBSplineDat::INVALID_BSOFFSET;
}

cjBSColorEvaluator::cjBSColorEvaluator(cjBSplineDat* dat, cjBSplineDat::BSOffset colorCPOffset,
    cjBSplineBasisF3Dat* bsBasisDat) 
	: cjBSplineEvaluator(dat, bsBasisDat)
{
	m_valueCPOffset = colorCPOffset;
    SetEvalChannelTypes();
}

cjBSColorEvaluator::cjBSColorEvaluator(cjBSplineDat* dat, cjBSplineDat::BSOffset colorCPOffset,
	cjBSplineBasisF3Dat* bsBasisDat, cjBool isCompressCPs) 
	: cjBSplineEvaluator(dat, bsBasisDat) 
{
    m_valueCPOffset = colorCPOffset;
	cjAssert(isCompressCPs);
}

cjBSColorEvaluator::~cjBSColorEvaluator()
{
}

cjBool cjBSColorEvaluator::GetChannelPosedValue(cjUINT32 channel, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(rst);

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChannelItemInvalid(channel));
        cjAssert(GetCPCount(cjAniEvalChan_Base) > 0);
        cjAssert(m_spDat);

		const cjColorF* value = (const cjColorF*)m_spDat->GetCP(m_valueCPOffset, 0, 4);
        *(cjColorF*)rst = *value;

        return cjTrue;
    }

    return cjFalse;
}

cjBool cjBSColorEvaluator::UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);
	cjAssert(evalSPDat);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
	cjAssert(rst);

    if(IsChannelItemInvalid(channel))
        return cjFalse;

	if(!evalSPDat->IsSegDatValid(etime))
    {
        cjAniEvalSPadFillFunc pfFillFunc = evalSPDat->GetFillFunc();

		if(!pfFillFunc || !(*pfFillFunc)(etime, evalSPDat))
            return cjFalse;
    }

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChannelItemInvalid(channel));
        cjAniSPad::LColorSeg* linearColorSeg = (cjAniSPad::LColorSeg*)evalSPDat->GetSegDat();
        *(cjColorF*)rst = linearColorSeg->m_value0;

        cjAssert(linearColorSeg->m_outTangent0 == cjColorF(0, 0, 0, 0));
    }
    else
    {
        cjAniSPad::BSColorSeg* bsColorSeg = (cjAniSPad::BSColorSeg*)evalSPDat->GetSegDat();
		cjBSplineBasisF3& bs = bsColorSeg->m_bsBasisDat->GetBasis();
        
		cjFLOAT bsval0 = bs.Get(0); cjFLOAT bsval1 = bs.Get(1); cjFLOAT bsval2 = bs.Get(2); cjFLOAT bsval3 = bs.Get(3);

        ((cjFLOAT*)rst)[0] = bsval0 * bsColorSeg->m_src[0] + bsval1 * bsColorSeg->m_src[4] + bsval2 * bsColorSeg->m_src[8] + bsval3 * bsColorSeg->m_src[12];
        ((cjFLOAT*)rst)[1] = bsval0 * bsColorSeg->m_src[1] + bsval1 * bsColorSeg->m_src[5] + bsval2 * bsColorSeg->m_src[9] + bsval3 * bsColorSeg->m_src[13];
		((cjFLOAT*)rst)[2] = bsval0 * bsColorSeg->m_src[2] + bsval1 * bsColorSeg->m_src[6] + bsval2 * bsColorSeg->m_src[10] + bsval3 * bsColorSeg->m_src[14];
        ((cjFLOAT*)rst)[3] = bsval0 * bsColorSeg->m_src[3] + bsval1 * bsColorSeg->m_src[7] + bsval2 * bsColorSeg->m_src[11] + bsval3 * bsColorSeg->m_src[15];
    }

    return cjTrue;
}

cjBool cjBSColorEvaluator::GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, 
	cjUINT32& fillSiz, cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjBool isPosed = IsChannelItemPosed(channel);

    if(IsChannelItemInvalid(channel) || (isPosed && !forceAlwaysUpdate && !IsAlwaysUpdate()))
        return cjFalse;

    if(isPosed)
    {
        fillSiz = 0;spadBlk = cjAniSPADB_LinearColorSeg; bsBasisDat = cjNULL;
    }
    else
    {
        fillSiz = sizeof(cjAniSPad::BSColorFill); spadBlk = cjAniSPADB_LinearColorSeg; bsBasisDat = m_spBasisDat;
    }

	isSharedFillDat = cjFalse;

    return cjTrue;
}

cjBool cjBSColorEvaluator::InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, 
	cjBool initSharedDat, cjRefObjNameHash* nodeHash, cjAniPoseBufHandle pbHandle) const
{
    cjAssert(channel == cjAniEvalChan_Base);
	cjAssert(evalSPDat);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(!IsChannelItemInvalid(channel));
    cjAssert(m_valueCPOffset != cjBSplineDat::INVALID_BSOFFSET);
    cjAssert(m_spDat);
    cjAssert(m_spBasisDat);

    if(IsChannelItemPosed(channel))
    {
        evalSPDat->SetSegTimeRange(0.0f, CJ_FLT_INFINITY);
        cjAssert(evalSPDat->GetFillFunc() == cjNULL);

        cjAniSPad::LColorSeg* linearColorSeg = (cjAniSPad::LColorSeg*)evalSPDat->GetSegDat();
		cjAssert(linearColorSeg);
        linearColorSeg->m_hdr.m_lod = evalSPDat->GetLOD();

        if(IsReferenced())
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
            linearColorSeg->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
        }
        else
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Color);
            linearColorSeg->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
        }

        linearColorSeg->m_stTime = 0.0f;
        linearColorSeg->m_invDTime = 0.0f;

        GetChannelPosedValue(channel, &linearColorSeg->m_value0);

        linearColorSeg->m_outTangent0 = cjColorF(0, 0, 0, 0);
    }
    else
    {
        cjAssert(evalSPDat->GetSegMinTime() == CJ_FLT_INFINITY);
        cjAssert(evalSPDat->GetSegMaxTime() == CJ_FLT_INVALID);

        evalSPDat->SetFillFunc(&BSColorFillFunc);
        cjAniSPad::BSColorFill* fillDat = (cjAniSPad::BSColorFill*)evalSPDat->GetFillDat();

		fillDat->m_cpOffset = m_valueCPOffset;
        fillDat->m_stTime = m_stTime;
        fillDat->m_invDTime = (m_enTime != m_stTime) ? 1.0f / (m_enTime - m_stTime) : 0.0f;
        fillDat->m_lastMin = CJ_INT_INVALID;
        fillDat->m_cpAry = m_spDat;

        cjAniSPad::BSColorSeg* bsColorSeg = (cjAniSPad::BSColorSeg*)evalSPDat->GetSegDat();
        bsColorSeg->m_hdr.m_lod = evalSPDat->GetLOD();

        if(IsReferenced())
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
            bsColorSeg->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
        }
        else
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Color);
            bsColorSeg->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
        }

		cjAssert(bsBasisDat);
        bsColorSeg->m_bsBasisDat = bsBasisDat;
    }

    return cjTrue;
}

cjBool cjBSColorEvaluator::BSColorFillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat)
{
    cjAssert(evalSPDat);

    cjAniSPad::BSColorFill* fillDat = (cjAniSPad::BSColorFill*)evalSPDat->GetFillDat();
	cjAniSPad::BSColorSeg* bsColorSeg = (cjAniSPad::BSColorSeg*)evalSPDat->GetSegDat();
	
    cjINT32 vMin, vMax;
	cjBSplineBasisF3& basis = bsColorSeg->m_bsBasisDat->GetBasis();
    cjFLOAT ratio = (etime - fillDat->m_stTime) * fillDat->m_invDTime;

    basis.Compute(ratio, vMin, vMax);

    if(vMin != fillDat->m_lastMin)
    {
		const cjFLOAT* src = fillDat->m_cpAry->GetCP(fillDat->m_cpOffset, vMin, 4);
        cjFLOAT* dst = bsColorSeg->m_src;
        cjFLOAT* endDst = dst + 16;

        while(dst < endDst)
        {
            *dst = *src;
            src++;
            dst++;
        }
        fillDat->m_lastMin = vMin;
    }

    evalSPDat->SetSegTimeRange(etime, etime); // no need scartch fill dat

    return cjTrue;
}

void cjBSColorEvaluator::SetEvalChannelTypes()
{
    cjUINT32 cpCount = GetCPCount(cjAniEvalChan_Base);

    if(cpCount > 0)
    {
        cjAssert(m_valueCPOffset != cjBSplineDat::INVALID_BSOFFSET);
        cjAssert(m_spDat);
        cjAssert(m_spBasisDat);

        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_Color;

        cjBool isPosed = cjTrue;
        if(IsCompressedCP())
        {
			const cjSWORD* val = m_spDat->GetHalfFloatCP(m_valueCPOffset, 0, 4);
            const cjSWORD* endVal = val + cpCount * 4;

            cjSWORD v0 = *val; cjSWORD v1 = *(val+1); cjSWORD v2 = *(val+2); cjSWORD v3 = *(val+3);
            val += 4;

            while(val < endVal)
            {
                if(*val != v0 || *(val+1) != v1 || *(val+2) != v2 || *(val+3) != v3)
                {
                    isPosed = cjFalse;
                    break;
                }
                val += 4;
            }
        }
        else
        {
            const cjColorF* val = (const cjColorF*)m_spDat->GetCP(m_valueCPOffset, 0, 4);
            const cjColorF* endVal = val + cpCount;

            cjColorF value0 = *val;
            val++;

            while(val < endVal)
            {
                if(*val != value0)
                {
                    isPosed = cjFalse;
                    break;
                }

                val++;
            }
        }

        if(isPosed)
            m_channelItem[cjAniEvalChan_Base] |= cjAniEvalChanItemMask_Constant;
    }
    else
    {
        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_NoData;
    }
}


//
// cjBSFloatEvaluator
//

CJSER_IMPL_CLASS(cjBSFloatEvaluator, cjBSplineEvaluator);

CJSER_IMPL_SERIALIZE_BEGIN(cjBSFloatEvaluator)
	
	if(ser.IsQuerySerObjAndStr())
	{
		return;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		ser << m_valueCPOffset;
	}
	
CJSER_IMPL_SERIALIZE_END(cjBSFloatEvaluator)

CJSER_IMPL_CLONE(cjBSFloatEvaluator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjBSFloatEvaluator)
	d->m_valueCPOffset = m_valueCPOffset;	
CJSER_IMPL_CLONE_PROPCOPY_END(cjBSFloatEvaluator)


cjBSFloatEvaluator::cjBSFloatEvaluator()   
{
	m_valueCPOffset = cjBSplineDat::INVALID_BSOFFSET;
}

cjBSFloatEvaluator::cjBSFloatEvaluator(cjBSplineDat* dat, cjBSplineDat::BSOffset floatCPOffset, 
	cjBSplineBasisF3Dat* bsBasisDat) 
	: cjBSplineEvaluator(dat, bsBasisDat) 
{
	m_valueCPOffset = floatCPOffset;
    SetEvalChannelTypes();
}

cjBSFloatEvaluator::cjBSFloatEvaluator(cjBSplineDat* dat, cjBSplineDat::BSOffset floatCPOffset, 
	cjBSplineBasisF3Dat* bsBasisDat, cjBool isCompressCPs) 
	: cjBSplineEvaluator(dat, bsBasisDat)
{
	cjAssert(isCompressCPs);
	m_valueCPOffset = floatCPOffset;
}

cjBSFloatEvaluator::~cjBSFloatEvaluator()
{
}

cjBool cjBSFloatEvaluator::GetChannelPosedValue(cjUINT32 channel, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(rst);

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChannelItemInvalid(channel));
        cjAssert(GetCPCount(cjAniEvalChan_Base) > 0);
        cjAssert(m_spDat);

        const cjFLOAT* value = m_spDat->GetCP(m_valueCPOffset, 0, 1);
        *(cjFLOAT*)rst = *value;

        return cjTrue;
    }

    return cjFalse;
}

cjBool cjBSFloatEvaluator::UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(evalSPDat);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(rst);

    if(IsChannelItemInvalid(channel))
        return cjFalse;

    if(!evalSPDat->IsSegDatValid(etime))
    {
        cjAniEvalSPadFillFunc pfFillFunc = evalSPDat->GetFillFunc();

        if(!pfFillFunc || !(*pfFillFunc)(etime, evalSPDat))
            return cjFalse;
    }

    if(IsChannelItemPosed(channel))
    {        
        cjAssert(!IsChannelItemInvalid(channel));
        cjAniSPad::CubicFloatSeg* cubicFloatSeg = (cjAniSPad::CubicFloatSeg*)evalSPDat->GetSegDat();

		*(cjFLOAT*)rst = cubicFloatSeg->m_value0;

        cjAssert(cubicFloatSeg->m_outTangent0 == 0.0f);
        cjAssert(cubicFloatSeg->m_a0 == 0.0f);
        cjAssert(cubicFloatSeg->m_b0 == 0.0f);
    }
    else
    {
        cjAniSPad::BSFloatSeg* bsFloatSeg = (cjAniSPad::BSFloatSeg*)evalSPDat->GetSegDat();
		cjBSplineBasisF3& bs = bsFloatSeg->m_bsBasisDat->GetBasis();

		cjFLOAT bsval0 = bs.Get(0); cjFLOAT bsval1 = bs.Get(1); cjFLOAT bsval2 = bs.Get(2); cjFLOAT bsval3 = bs.Get(3);
        *(cjFLOAT*)rst = bsval0 * bsFloatSeg->m_src[0] + bsval1 * bsFloatSeg->m_src[1] + bsval2 * bsFloatSeg->m_src[2] + bsval3 * bsFloatSeg->m_src[3];
    }

    return cjTrue;
}

cjBool cjBSFloatEvaluator::GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, 
	cjUINT32& fillSiz, cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjBool isPosed = IsChannelItemPosed(channel);

    if(IsChannelItemInvalid(channel) || (isPosed && !forceAlwaysUpdate && !IsAlwaysUpdate()))
        return cjFalse;

    if(isPosed)
    {
        fillSiz = 0; spadBlk = cjAniSPADB_CubicFloatSeg; bsBasisDat = cjNULL;
    }
    else
    {
        fillSiz = sizeof(cjAniSPad::BSFloatFill); spadBlk = cjAniSPADB_BSFloatSeg; bsBasisDat = m_spBasisDat;
    }

    isSharedFillDat = cjFalse;

    return cjTrue;
}

cjBool cjBSFloatEvaluator::InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, 
	cjBool initSharedDat, cjRefObjNameHash* nodeHash, cjAniPoseBufHandle pbHandle) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(evalSPDat);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(!IsChannelItemInvalid(channel));
    cjAssert(m_valueCPOffset != cjBSplineDat::INVALID_BSOFFSET);
    cjAssert(m_spDat);
    cjAssert(m_spBasisDat);

    if(IsChannelItemPosed(channel))
    {
        evalSPDat->SetSegTimeRange(0.0f, CJ_FLT_INFINITY);
        cjAssert(evalSPDat->GetFillFunc() == cjNULL);

        cjAniSPad::CubicFloatSeg* cubicFloatSeg = (cjAniSPad::CubicFloatSeg*)evalSPDat->GetSegDat();
        cjAssert(cubicFloatSeg);
        cubicFloatSeg->m_hdr.m_lod = evalSPDat->GetLOD();

        if(IsReferenced())
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
            cubicFloatSeg->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
        }
        else
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Float);
            cubicFloatSeg->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
        }

        cubicFloatSeg->m_stTime = 0.0f;
        cubicFloatSeg->m_invDTime = 0.0f;

        GetChannelPosedValue(channel, &cubicFloatSeg->m_value0);

        cubicFloatSeg->m_outTangent0 = 0.0f;
        cubicFloatSeg->m_a0 = 0.0f;
        cubicFloatSeg->m_b0 = 0.0f;
    }
    else
    {
        cjAssert(evalSPDat->GetSegMinTime() == CJ_FLT_INFINITY);
        cjAssert(evalSPDat->GetSegMaxTime() == CJ_FLT_INVALID);

		evalSPDat->SetFillFunc(&BSFloatFillFunc);

        cjAniSPad::BSFloatFill* fillDat = (cjAniSPad::BSFloatFill*)evalSPDat->GetFillDat();
        cjAssert(fillDat);

        fillDat->m_cpOffset = m_valueCPOffset;
        fillDat->m_stTime = m_stTime;
        fillDat->m_invDTime = (m_enTime != m_stTime) ? 1.0f / (m_enTime - m_stTime) : 0.0f;
        fillDat->m_lastMin = CJ_INT_INVALID;
        fillDat->m_bsDat = m_spDat;

        cjAniSPad::BSFloatSeg* bsFloatSeg = (cjAniSPad::BSFloatSeg*)evalSPDat->GetSegDat();
        cjAssert(bsFloatSeg);

        bsFloatSeg->m_hdr.m_lod = evalSPDat->GetLOD();

        if(IsReferenced())
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
            bsFloatSeg->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
        }
        else
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Float);
            bsFloatSeg->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
        }

        cjAssert(bsBasisDat);
        bsFloatSeg->m_bsBasisDat = bsBasisDat;
    }

    return cjTrue;
}

cjBool cjBSFloatEvaluator::BSFloatFillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat)
{
    cjAssert(evalSPDat);

    cjAniSPad::BSFloatFill* fillDat = (cjAniSPad::BSFloatFill*)evalSPDat->GetFillDat();
    cjAssert(fillDat);

    cjAniSPad::BSFloatSeg* bsFloatSeg = (cjAniSPad::BSFloatSeg*)evalSPDat->GetSegDat();
    cjAssert(bsFloatSeg);

    cjINT32 vMin, vMax;
    cjBSplineBasisF3& basis = bsFloatSeg->m_bsBasisDat->GetBasis();
    cjFLOAT ratio = (etime - fillDat->m_stTime) * fillDat->m_invDTime;

    basis.Compute(ratio, vMin, vMax);

    if(vMin != fillDat->m_lastMin)
    {
        const cjFLOAT* src = fillDat->m_bsDat->GetCP(fillDat->m_cpOffset, vMin, 1);
        cjFLOAT* dst = bsFloatSeg->m_src;
        cjFLOAT* endDst = dst + 4;

        while(dst < endDst)
        {
            *dst = *src;
            src++;
            dst++;
        }

        fillDat->m_lastMin = vMin;
    }

    evalSPDat->SetSegTimeRange(etime, etime);

    return cjTrue;
}

void cjBSFloatEvaluator::SetEvalChannelTypes()
{
    cjUINT32 cpCount = GetCPCount(cjAniEvalChan_Base);

    if(cpCount > 0)
    {
        cjAssert(m_valueCPOffset != cjBSplineDat::INVALID_BSOFFSET);
        cjAssert(m_spDat);
        cjAssert(m_spBasisDat);

        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_Float;

        cjBool isPosed = cjTrue;

        if(IsCompressedCP())
        {
            const cjSWORD* cpCur = m_spDat->GetHalfFloatCP(m_valueCPOffset, 0, 1);
            const cjSWORD* cpEnd = cpCur + cpCount;

            cjSWORD value = *cpCur;
            cpCur++;

            while(cpCur < cpEnd)
            {
                if(*cpCur != value)
                {
                    isPosed = cjFalse;
                    break;
                }

                cpCur++;
            }
        }
        else
        {
            const cjFLOAT* cpCur = m_spDat->GetCP(m_valueCPOffset, 0, 1);
            const cjFLOAT* cpEnd = cpCur + cpCount;

            cjFLOAT value = *cpCur;
            cpCur++;

            while(cpCur < cpEnd)
            {
                if(*cpCur != value)
                {
                    isPosed = cjFalse;
                    break;
                }

                cpCur++;
            }
        }

        if(isPosed)
            m_channelItem[cjAniEvalChan_Base] |= cjAniEvalChanItemMask_Constant;
    }
    else
    {
        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_NoData;
    }
}


//
// cjBSVec3SEvaluator
//

CJSER_IMPL_CLASS(cjBSVec3SEvaluator, cjBSplineEvaluator);

CJSER_IMPL_SERIALIZE_BEGIN(cjBSVec3SEvaluator)
	
	if(ser.IsQuerySerObjAndStr())
	{
		return;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		ser << m_valueCPOffset;
	}
	
CJSER_IMPL_SERIALIZE_END(cjBSVec3SEvaluator)

CJSER_IMPL_CLONE(cjBSVec3SEvaluator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjBSVec3SEvaluator)
	d->m_valueCPOffset = m_valueCPOffset;	
CJSER_IMPL_CLONE_PROPCOPY_END(cjBSVec3SEvaluator)


cjBSVec3SEvaluator::cjBSVec3SEvaluator()
{
    m_valueCPOffset = cjBSplineDat::INVALID_BSOFFSET;
}

cjBSVec3SEvaluator::cjBSVec3SEvaluator(cjBSplineDat* dat, cjBSplineDat::BSOffset vec3CPOffset,
    cjBSplineBasisF3Dat* bsBasisDat) 
	: cjBSplineEvaluator(dat,bsBasisDat)
{
	m_valueCPOffset = vec3CPOffset;
    SetEvalChannelTypes();
}

cjBSVec3SEvaluator::cjBSVec3SEvaluator(cjBSplineDat* dat, cjBSplineDat::BSOffset vec3CPOffset,
    cjBSplineBasisF3Dat* bsBasisDat, cjBool isCompressCPs) 
	: cjBSplineEvaluator(dat, bsBasisDat)
{
    m_valueCPOffset = vec3CPOffset;
    cjAssert(isCompressCPs);
}

cjBSVec3SEvaluator::~cjBSVec3SEvaluator()
{
}

cjBool cjBSVec3SEvaluator::GetChannelPosedValue(cjUINT32 channel, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(rst);

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChannelItemInvalid(channel));
        cjAssert(GetCPCount(cjAniEvalChan_Base) > 0);
        cjAssert(m_spDat);

        const cjVec3* val0 = (const cjVec3*)m_spDat->GetCP(m_valueCPOffset, 0, 3);
        *(cjVec3*)rst = *val0;

        return cjTrue;
    }

    return cjFalse;
}

cjBool cjBSVec3SEvaluator::UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);

    if(IsChannelItemInvalid(channel))
        return cjFalse;

    if(!evalSPDat->IsSegDatValid(etime))
    {
        cjAniEvalSPadFillFunc pfFillFunc = evalSPDat->GetFillFunc();

        if(!pfFillFunc || !(*pfFillFunc)(etime, evalSPDat))
            return cjFalse;
    }

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChannelItemInvalid(channel));
        cjAniSPad::CubicVec3Seg* cubicVec3Seg = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();
		*(cjVec3*)rst = cubicVec3Seg->m_val0;

        cjAssert(cubicVec3Seg->m_outTangent0 == cjVec3::ZERO);
        cjAssert(cubicVec3Seg->m_a0 == cjVec3::ZERO);
        cjAssert(cubicVec3Seg->m_b0 == cjVec3::ZERO);
    }
    else
    {
        cjAniSPad::BSVec3Seg* bsVec3Seg = (cjAniSPad::BSVec3Seg*)evalSPDat->GetSegDat();
        cjAssert(bsVec3Seg->m_bsBasisDat);
		cjBSplineBasisF3& basis = bsVec3Seg->m_bsBasisDat->GetBasis();

        cjFLOAT bsval0 = basis.Get(0); cjFLOAT bsval1 = basis.Get(1); cjFLOAT bsval2 = basis.Get(2); cjFLOAT bsval3 = basis.Get(3);

        ((cjFLOAT*)rst)[0] = bsval0 * bsVec3Seg->m_src[0] + bsval1 * bsVec3Seg->m_src[3] + bsval2 * bsVec3Seg->m_src[6] + bsval3 * bsVec3Seg->m_src[9];
        ((cjFLOAT*)rst)[1] = bsval0 * bsVec3Seg->m_src[1] + bsval1 * bsVec3Seg->m_src[4] + bsval2 * bsVec3Seg->m_src[7] + bsval3 * bsVec3Seg->m_src[10];
        ((cjFLOAT*)rst)[2] = bsval0 * bsVec3Seg->m_src[2] + bsval1 * bsVec3Seg->m_src[5] + bsval2 * bsVec3Seg->m_src[8] + bsval3 * bsVec3Seg->m_src[11];
    }

    return cjTrue;
}

cjBool cjBSVec3SEvaluator::GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, 
	cjUINT32& fillSiz, cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const
{
    cjAssert(channel == cjAniEvalChan_Base);

    cjBool isPosed = IsChannelItemPosed(channel);

    if(IsChannelItemInvalid(channel) || (isPosed && !forceAlwaysUpdate && !IsAlwaysUpdate()))
        return cjFalse;

	if(isPosed)
    {
        fillSiz = 0; 
		spadBlk = cjAniSPADB_CubicVec3SSeg; //cjcheck - cjAniSPADB_CubicVec3SSeg
		bsBasisDat = cjNULL; 
    }
    else
    {
        fillSiz = sizeof(cjAniSPad::BSVec3Fill);
		spadBlk = cjAniSPADB_BSVec3SSeg;
		bsBasisDat = m_spBasisDat;
    }

    isSharedFillDat = cjFalse;

    return cjTrue;
}

cjBool cjBSVec3SEvaluator::InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, 
	cjBool initSharedDat, cjRefObjNameHash* nodeHash, cjAniPoseBufHandle pbHandle) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(!IsChannelItemInvalid(channel));
    cjAssert(m_valueCPOffset != cjBSplineDat::INVALID_BSOFFSET);
    cjAssert(m_spDat);
    cjAssert(m_spBasisDat);

    if(IsChannelItemPosed(channel))
    {
        evalSPDat->SetSegTimeRange(0.0f, CJ_FLT_INFINITY);
        cjAssert(evalSPDat->GetFillFunc() == cjNULL);

        cjAniSPad::CubicVec3Seg* cubicVec3Seg = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();
        cubicVec3Seg->m_hdr.m_lod = evalSPDat->GetLOD();

        if(IsReferenced())
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
            cubicVec3Seg->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
        }
        else
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Vec3S);
            cubicVec3Seg->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
        }

        cubicVec3Seg->m_stTime = 0.0f;
        cubicVec3Seg->m_invDTime = 0.0f;
        GetChannelPosedValue(channel, &cubicVec3Seg->m_val0);
        cubicVec3Seg->m_outTangent0 = cjVec3::ZERO;
        cubicVec3Seg->m_a0 = cjVec3::ZERO;
        cubicVec3Seg->m_b0 = cjVec3::ZERO;
    }
    else
    {
        cjAssert(evalSPDat->GetSegMinTime() == CJ_FLT_INFINITY);
        cjAssert(evalSPDat->GetSegMaxTime() == CJ_FLT_INVALID);

        evalSPDat->SetFillFunc(&BSVec3SFillFunc);

        cjAniSPad::BSVec3Fill* fillDat = (cjAniSPad::BSVec3Fill*)evalSPDat->GetFillDat();

        fillDat->m_cpOffset = m_valueCPOffset;
        fillDat->m_stTime = m_stTime;
		fillDat->m_invDTime = (m_enTime != m_stTime) ? 1.0f / (m_enTime - m_stTime) : 0.0f;
        fillDat->m_lastMin = CJ_INT_INVALID;
		fillDat->m_bsDat = m_spDat;

        cjAniSPad::BSVec3Seg* bsVec3Seg = (cjAniSPad::BSVec3Seg*)evalSPDat->GetSegDat();
        bsVec3Seg->m_hdr.m_lod = evalSPDat->GetLOD();

        if(IsReferenced())
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
            bsVec3Seg->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
        }
        else
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Vec3S);
            bsVec3Seg->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
        }

        cjAssert(bsBasisDat);
        bsVec3Seg->m_bsBasisDat = bsBasisDat;
    }

    return cjTrue;
}

cjBool cjBSVec3SEvaluator::BSVec3SFillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat)
{
    cjAssert(evalSPDat);

    cjAniSPad::BSVec3Fill* fillDat = (cjAniSPad::BSVec3Fill*)evalSPDat->GetFillDat();
    cjAniSPad::BSVec3Seg* bsVec3Seg = (cjAniSPad::BSVec3Seg*)evalSPDat->GetSegDat();

    cjINT32 vMin, vMax;
    cjBSplineBasisF3& basis = bsVec3Seg->m_bsBasisDat->GetBasis();
	cjFLOAT ratio = (etime - fillDat->m_stTime) * fillDat->m_invDTime;
    basis.Compute(ratio, vMin, vMax);

    if(vMin != fillDat->m_lastMin)
    {
		const cjFLOAT* src = fillDat->m_bsDat->GetCP(fillDat->m_cpOffset, vMin, 3);
        cjFLOAT* dst = bsVec3Seg->m_src;
		cjFLOAT* endDst = dst + 12;

        while(dst < endDst)
        {
            *dst = *src;
            src++;
            dst++;
        }

        fillDat->m_lastMin = vMin;
    }

    evalSPDat->SetSegTimeRange(etime, etime);

    return cjTrue;
}

void cjBSVec3SEvaluator::SetEvalChannelTypes()
{
    cjUINT32 cpCount = GetCPCount(cjAniEvalChan_Base);

    if(cpCount > 0)
    {
        cjAssert(m_valueCPOffset != cjBSplineDat::INVALID_BSOFFSET);
        cjAssert(m_spDat);
        cjAssert(m_spBasisDat);

        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_Vec3S;

        cjBool isPosed = cjTrue;

        if(IsCompressedCP())
        {
            const cjSWORD* cpCur = m_spDat->GetHalfFloatCP(m_valueCPOffset, 0, 3);
            const cjSWORD* cpEnd = cpCur + cpCount * 3;

            cjSWORD v0 = *cpCur; cjSWORD v1 = *(cpCur+1); cjSWORD v2 = *(cpCur+2);
            cpCur += 3;

            while(cpCur < cpEnd)
            {
                if(*cpCur != v0 || *(cpCur+1) != v1 || *(cpCur+2) != v2)
                {
                    isPosed = cjFalse;
                    break;
                }
                cpCur += 3;
            }
        }
        else
        {
            const cjVec3* cpCur = (const cjVec3*)m_spDat->GetCP(m_valueCPOffset, 0, 3);
            const cjVec3* cpEnd = cpCur + cpCount;

            cjVec3 value = *cpCur;
            cpCur++;

            while(cpCur < cpEnd)
            {
                if(*cpCur != value)
                {
                    isPosed = cjFalse;
                    break;
                }

                cpCur++;
            }
        }

        if(isPosed)
            m_channelItem[cjAniEvalChan_Base] |= cjAniEvalChanItemMask_Constant;
    }
    else
    {
        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_NoData;
    }
}


//
// cjBSVec3PEvaluator
//

CJSER_IMPL_CLASS(cjBSVec3PEvaluator, cjBSplineEvaluator);

CJSER_IMPL_SERIALIZE_BEGIN(cjBSVec3PEvaluator)
	
	if(ser.IsQuerySerObjAndStr())
	{
		return;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		ser << m_valueCPOffset;
	}
	
CJSER_IMPL_SERIALIZE_END(cjBSVec3PEvaluator)

CJSER_IMPL_CLONE(cjBSVec3PEvaluator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjBSVec3PEvaluator)
	d->m_valueCPOffset = m_valueCPOffset;	
CJSER_IMPL_CLONE_PROPCOPY_END(cjBSVec3PEvaluator)


cjBSVec3PEvaluator::cjBSVec3PEvaluator()
{
    m_valueCPOffset = cjBSplineDat::INVALID_BSOFFSET;
}

cjBSVec3PEvaluator::cjBSVec3PEvaluator(cjBSplineDat* dat, cjBSplineDat::BSOffset vec3CPOffset,
    cjBSplineBasisF3Dat* bsBasisDat) 
	: cjBSplineEvaluator(dat,bsBasisDat)
{
	m_valueCPOffset = vec3CPOffset;
    SetEvalChannelTypes();
}

cjBSVec3PEvaluator::cjBSVec3PEvaluator(cjBSplineDat* dat, cjBSplineDat::BSOffset vec3CPOffset,
    cjBSplineBasisF3Dat* bsBasisDat, cjBool isCompressCPs) 
	: cjBSplineEvaluator(dat, bsBasisDat)
{
    m_valueCPOffset = vec3CPOffset;
    cjAssert(isCompressCPs);
}

cjBSVec3PEvaluator::~cjBSVec3PEvaluator()
{
}

cjBool cjBSVec3PEvaluator::GetChannelPosedValue(cjUINT32 channel, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(rst);

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChannelItemInvalid(channel));
        cjAssert(GetCPCount(cjAniEvalChan_Base) > 0);
        cjAssert(m_spDat);

        const cjVec3* val0 = (const cjVec3*)m_spDat->GetCP(m_valueCPOffset, 0, 3);
        *(cjVec3*)rst = *val0;

        return cjTrue;
    }

    return cjFalse;
}

cjBool cjBSVec3PEvaluator::UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);

    if(IsChannelItemInvalid(channel))
        return cjFalse;

    if(!evalSPDat->IsSegDatValid(etime))
    {
        cjAniEvalSPadFillFunc pfFillFunc = evalSPDat->GetFillFunc();

        if(!pfFillFunc || !(*pfFillFunc)(etime, evalSPDat))
            return cjFalse;
    }

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChannelItemInvalid(channel));
        cjAniSPad::CubicVec3Seg* cubicVec3Seg = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();
		*(cjVec3*)rst = cubicVec3Seg->m_val0;

        cjAssert(cubicVec3Seg->m_outTangent0 == cjVec3::ZERO);
        cjAssert(cubicVec3Seg->m_a0 == cjVec3::ZERO);
        cjAssert(cubicVec3Seg->m_b0 == cjVec3::ZERO);
    }
    else
    {
        cjAniSPad::BSVec3Seg* bsVec3Seg = (cjAniSPad::BSVec3Seg*)evalSPDat->GetSegDat();
        cjAssert(bsVec3Seg->m_bsBasisDat);
		cjBSplineBasisF3& basis = bsVec3Seg->m_bsBasisDat->GetBasis();

        cjFLOAT bsval0 = basis.Get(0); cjFLOAT bsval1 = basis.Get(1); cjFLOAT bsval2 = basis.Get(2); cjFLOAT bsval3 = basis.Get(3);

        ((cjFLOAT*)rst)[0] = bsval0 * bsVec3Seg->m_src[0] + bsval1 * bsVec3Seg->m_src[3] + bsval2 * bsVec3Seg->m_src[6] + bsval3 * bsVec3Seg->m_src[9];
        ((cjFLOAT*)rst)[1] = bsval0 * bsVec3Seg->m_src[1] + bsval1 * bsVec3Seg->m_src[4] + bsval2 * bsVec3Seg->m_src[7] + bsval3 * bsVec3Seg->m_src[10];
        ((cjFLOAT*)rst)[2] = bsval0 * bsVec3Seg->m_src[2] + bsval1 * bsVec3Seg->m_src[5] + bsval2 * bsVec3Seg->m_src[8] + bsval3 * bsVec3Seg->m_src[11];
    }

    return cjTrue;
}

cjBool cjBSVec3PEvaluator::GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, 
	cjUINT32& fillSiz, cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const
{
    cjAssert(channel == cjAniEvalChan_Base);

    cjBool isPosed = IsChannelItemPosed(channel);

    if(IsChannelItemInvalid(channel) || (isPosed && !forceAlwaysUpdate && !IsAlwaysUpdate()))
        return cjFalse;

	if(isPosed)
    {
        fillSiz = 0; 
		spadBlk = cjAniSPADB_CubicVec3PSeg;
		bsBasisDat = cjNULL; 
    }
    else
    {
        fillSiz = sizeof(cjAniSPad::BSVec3Fill);
		spadBlk = cjAniSPADB_BSVec3PSeg;
		bsBasisDat = m_spBasisDat;
    }

    isSharedFillDat = cjFalse;

    return cjTrue;
}

cjBool cjBSVec3PEvaluator::InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, 
	cjBool initSharedDat, cjRefObjNameHash* nodeHash, cjAniPoseBufHandle pbHandle) const
{
    cjAssert(channel == cjAniEvalChan_Base);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(!IsChannelItemInvalid(channel));
    cjAssert(m_valueCPOffset != cjBSplineDat::INVALID_BSOFFSET);
    cjAssert(m_spDat);
    cjAssert(m_spBasisDat);

    if(IsChannelItemPosed(channel))
    {
        evalSPDat->SetSegTimeRange(0.0f, CJ_FLT_INFINITY);
        cjAssert(evalSPDat->GetFillFunc() == cjNULL);

        cjAniSPad::CubicVec3Seg* cubicVec3Seg = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();
        cubicVec3Seg->m_hdr.m_lod = evalSPDat->GetLOD();

        if(IsReferenced())
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
            cubicVec3Seg->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
        }
        else
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Vec3P);
            cubicVec3Seg->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
        }

        cubicVec3Seg->m_stTime = 0.0f;
        cubicVec3Seg->m_invDTime = 0.0f;
        GetChannelPosedValue(channel, &cubicVec3Seg->m_val0);
        cubicVec3Seg->m_outTangent0 = cjVec3::ZERO;
        cubicVec3Seg->m_a0 = cjVec3::ZERO;
        cubicVec3Seg->m_b0 = cjVec3::ZERO;
    }
    else
    {
        cjAssert(evalSPDat->GetSegMinTime() == CJ_FLT_INFINITY);
        cjAssert(evalSPDat->GetSegMaxTime() == CJ_FLT_INVALID);

        evalSPDat->SetFillFunc(&BSVec3PFillFunc);

        cjAniSPad::BSVec3Fill* fillDat = (cjAniSPad::BSVec3Fill*)evalSPDat->GetFillDat();

        fillDat->m_cpOffset = m_valueCPOffset;
        fillDat->m_stTime = m_stTime;
		fillDat->m_invDTime = (m_enTime != m_stTime) ? 1.0f / (m_enTime - m_stTime) : 0.0f;
        fillDat->m_lastMin = CJ_INT_INVALID;
		fillDat->m_bsDat = m_spDat;

        cjAniSPad::BSVec3Seg* bsVec3Seg = (cjAniSPad::BSVec3Seg*)evalSPDat->GetSegDat();
        bsVec3Seg->m_hdr.m_lod = evalSPDat->GetLOD();

        if(IsReferenced())
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
            bsVec3Seg->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
        }
        else
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Vec3P);
            bsVec3Seg->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
        }

        cjAssert(bsBasisDat);
        bsVec3Seg->m_bsBasisDat = bsBasisDat;
    }

    return cjTrue;
}

cjBool cjBSVec3PEvaluator::BSVec3PFillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat)
{
    cjAssert(evalSPDat);

    cjAniSPad::BSVec3Fill* fillDat = (cjAniSPad::BSVec3Fill*)evalSPDat->GetFillDat();
    cjAniSPad::BSVec3Seg* bsVec3Seg = (cjAniSPad::BSVec3Seg*)evalSPDat->GetSegDat();

    cjINT32 vMin, vMax;
    cjBSplineBasisF3& basis = bsVec3Seg->m_bsBasisDat->GetBasis();
	cjFLOAT ratio = (etime - fillDat->m_stTime) * fillDat->m_invDTime;
    basis.Compute(ratio, vMin, vMax);

    if(vMin != fillDat->m_lastMin)
    {
		const cjFLOAT* src = fillDat->m_bsDat->GetCP(fillDat->m_cpOffset, vMin, 3);
        cjFLOAT* dst = bsVec3Seg->m_src;
		cjFLOAT* endDst = dst + 12;

        while(dst < endDst)
        {
            *dst = *src;
            src++;
            dst++;
        }

        fillDat->m_lastMin = vMin;
    }

    evalSPDat->SetSegTimeRange(etime, etime);

    return cjTrue;
}

void cjBSVec3PEvaluator::SetEvalChannelTypes()
{
    cjUINT32 cpCount = GetCPCount(cjAniEvalChan_Base);

    if(cpCount > 0)
    {
        cjAssert(m_valueCPOffset != cjBSplineDat::INVALID_BSOFFSET);
        cjAssert(m_spDat);
        cjAssert(m_spBasisDat);

        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_Vec3P;

        cjBool isPosed = cjTrue;

        if(IsCompressedCP())
        {
            const cjSWORD* cpCur = m_spDat->GetHalfFloatCP(m_valueCPOffset, 0, 3);
            const cjSWORD* cpEnd = cpCur + cpCount * 3;

            cjSWORD v0 = *cpCur; cjSWORD v1 = *(cpCur+1); cjSWORD v2 = *(cpCur+2);
            cpCur += 3;

            while(cpCur < cpEnd)
            {
                if(*cpCur != v0 || *(cpCur+1) != v1 || *(cpCur+2) != v2)
                {
                    isPosed = cjFalse;
                    break;
                }
                cpCur += 3;
            }
        }
        else
        {
            const cjVec3* cpCur = (const cjVec3*)m_spDat->GetCP(m_valueCPOffset, 0, 3);
            const cjVec3* cpEnd = cpCur + cpCount;

            cjVec3 value = *cpCur;
            cpCur++;

            while(cpCur < cpEnd)
            {
                if(*cpCur != value)
                {
                    isPosed = cjFalse;
                    break;
                }

                cpCur++;
            }
        }

        if(isPosed)
            m_channelItem[cjAniEvalChan_Base] |= cjAniEvalChanItemMask_Constant;
    }
    else
    {
        m_channelItem[cjAniEvalChan_Base] = cjAniEvalChanItem_NoData;
    }
}


//
// cjBSTMEvaluator
//

CJSER_IMPL_CLASS(cjBSTMEvaluator, cjBSplineEvaluator);


CJSER_IMPL_SERIALIZE_BEGIN(cjBSTMEvaluator)
	
	if(ser.IsQuerySerObjAndStr())
	{
		return;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		ser << m_value;
		ser << m_posCPOffset;
		ser << m_rotCPOffset;
		ser << m_scaleCPOffset;
	}
	
CJSER_IMPL_SERIALIZE_END(cjBSTMEvaluator)

CJSER_IMPL_CLONE(cjBSTMEvaluator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjBSTMEvaluator)
	d->m_value			= m_value;
	d->m_posCPOffset	= m_posCPOffset;
	d->m_rotCPOffset	= m_rotCPOffset;
	d->m_scaleCPOffset	= m_scaleCPOffset;
CJSER_IMPL_CLONE_PROPCOPY_END(cjBSTMEvaluator)


cjBSTMEvaluator::cjBSTMEvaluator()
{
    m_posCPOffset = cjBSplineDat::INVALID_BSOFFSET; 
    m_rotCPOffset = cjBSplineDat::INVALID_BSOFFSET;
    m_scaleCPOffset = cjBSplineDat::INVALID_BSOFFSET;

    m_channelItem[cjAniEvalChan_Flag] |= cjAniEvalFlagChanVal_Transform;
}

cjBSTMEvaluator::cjBSTMEvaluator(cjBSplineDat* dat, 
	cjBSplineDat::BSOffset posCPOffset,
    cjBSplineDat::BSOffset rotCPOffset, 
	cjBSplineDat::BSOffset scaleCPOffset, 
	cjBSplineBasisF3Dat* bsBasisDat) 
		: cjBSplineEvaluator(dat, bsBasisDat) 
{
    m_posCPOffset = posCPOffset; 
    m_rotCPOffset = rotCPOffset;
    m_scaleCPOffset = scaleCPOffset;

	m_channelItem[cjAniEvalChan_Flag] |= cjAniEvalFlagChanVal_Transform;

    SetEvalChannelTypes();
}

cjBSTMEvaluator::cjBSTMEvaluator(const cjQuatTM& poseValue)
{
    m_value = poseValue;

    m_posCPOffset = cjBSplineDat::INVALID_BSOFFSET;
    m_rotCPOffset = cjBSplineDat::INVALID_BSOFFSET;
    m_scaleCPOffset = cjBSplineDat::INVALID_BSOFFSET;

    m_channelItem[cjAniEvalChan_Flag] |= cjAniEvalFlagChanVal_Transform;

    SetEvalChannelTypes();
}

cjBSTMEvaluator::cjBSTMEvaluator(cjBSplineDat* dat, 
	cjBSplineDat::BSOffset posCPOffset,
    cjBSplineDat::BSOffset rotCPOffset, 
	cjBSplineDat::BSOffset scaleCPOffset, 
	cjBSplineBasisF3Dat* bsBasisDat, cjBool isCompressCPs) 
		: cjBSplineEvaluator(dat, bsBasisDat)
{
    m_posCPOffset = posCPOffset; 
    m_rotCPOffset = rotCPOffset; 
    m_scaleCPOffset = scaleCPOffset;

    cjAssert(isCompressCPs);
    m_channelItem[cjAniEvalChan_Flag] |= cjAniEvalFlagChanVal_Transform;
}

cjBSTMEvaluator::cjBSTMEvaluator(const cjQuatTM& poseValue, cjBool isCompressCPs) 
{
	m_value = poseValue; 

    m_posCPOffset = cjBSplineDat::INVALID_BSOFFSET;
    m_rotCPOffset = cjBSplineDat::INVALID_BSOFFSET;
    m_scaleCPOffset = cjBSplineDat::INVALID_BSOFFSET;

    cjAssert(isCompressCPs);

    m_channelItem[cjAniEvalChan_Flag] |= cjAniEvalFlagChanVal_Transform;
}

cjBSTMEvaluator::~cjBSTMEvaluator()
{
}

cjBool cjBSTMEvaluator::GetChannelPosedValue(cjUINT32 channel, void* rst) const
{
    cjAssert(channel < cjAniEvalChan_PRS_Max);
    cjAssert(rst);

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChannelItemInvalid(channel));

        if(channel == cjAniEvalChan_Scl)
        {
			*(cjVec3*)rst = m_value.GetScale();
        }
        else if(channel == cjAniEvalChan_Pos)
        {
            *(cjVec3*)rst = m_value.GetPos();
        }
        else
        {
            cjAssert(channel == cjAniEvalChan_Rot);
            *(cjQuat*)rst = m_value.GetRotate();
        }

        return cjTrue;
    }

    return cjFalse;
}

cjBool cjBSTMEvaluator::UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const
{
    cjAssert(channel < cjAniEvalChan_PRS_Max);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(rst);

    if(IsChannelItemInvalid(channel))
        return cjFalse;

    if(!evalSPDat->IsSegDatValid(etime))
    {
        cjAniEvalSPadFillFunc pfFillFunc = evalSPDat->GetFillFunc();

		if(!pfFillFunc || !(*pfFillFunc)(etime, evalSPDat))
            return cjFalse;
    }

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChannelItemInvalid(channel));

		if(channel == cjAniEvalChan_Scl)
        {
            cjAniSPad::CubicVec3Seg* cubicVec3Seg = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();
            *(cjVec3*)rst = cubicVec3Seg->m_val0;

            cjAssert(cubicVec3Seg->m_outTangent0 == cjVec3::ZERO);
            cjAssert(cubicVec3Seg->m_a0 == cjVec3::ZERO);
            cjAssert(cubicVec3Seg->m_b0 == cjVec3::ZERO);
        }
        else if(channel == cjAniEvalChan_Pos)
        {
            cjAniSPad::CubicVec3Seg* cubicVec3Seg = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();
            *(cjVec3*)rst = cubicVec3Seg->m_val0;

            cjAssert(cubicVec3Seg->m_outTangent0 == cjVec3::ZERO);
            cjAssert(cubicVec3Seg->m_a0 == cjVec3::ZERO);
            cjAssert(cubicVec3Seg->m_b0 == cjVec3::ZERO);
        }
        else
        {
            cjAssert(channel == cjAniEvalChan_Rot);

            cjAniSPad::SlerpRotSeg* slerpRotSeg = (cjAniSPad::SlerpRotSeg*)evalSPDat->GetSegDat();
            *(cjQuat*)rst = slerpRotSeg->m_quat0;

            cjAssert(slerpRotSeg->m_quat1 == slerpRotSeg->m_quat0);
        }
    }
    else
    {
        if(channel == cjAniEvalChan_Pos)
        {
            cjAniSPad::BSVec3Seg* bsVec3Seg = (cjAniSPad::BSVec3Seg*)evalSPDat->GetSegDat();
            cjAssert(bsVec3Seg->m_bsBasisDat);

            cjBSplineBasisF3& basis = bsVec3Seg->m_bsBasisDat->GetBasis();

            cjFLOAT bsval0 = basis.Get(0); cjFLOAT bsval1 = basis.Get(1); cjFLOAT bsval2 = basis.Get(2); cjFLOAT bsval3 = basis.Get(3);

			((cjFLOAT*)rst)[0] = bsval0 * bsVec3Seg->m_src[0] + bsval1 * bsVec3Seg->m_src[3] + bsval2 * bsVec3Seg->m_src[6] + bsval3 * bsVec3Seg->m_src[9];
            ((cjFLOAT*)rst)[1] = bsval0 * bsVec3Seg->m_src[1] + bsval1 * bsVec3Seg->m_src[4] + bsval2 * bsVec3Seg->m_src[7] + bsval3 * bsVec3Seg->m_src[10];
            ((cjFLOAT*)rst)[2] = bsval0 * bsVec3Seg->m_src[2] + bsval1 * bsVec3Seg->m_src[5] + bsval2 * bsVec3Seg->m_src[8] + bsval3 * bsVec3Seg->m_src[11];
        }
        else if(channel == cjAniEvalChan_Rot)
        {
            cjAniSPad::BSRotSeg* bsRotSeg = (cjAniSPad::BSRotSeg*)evalSPDat->GetSegDat();
            cjAssert(bsRotSeg->m_bsBasisDat);

            cjBSplineBasisF3& basis = bsRotSeg->m_bsBasisDat->GetBasis();
            cjFLOAT bsval0 = basis.Get(0); cjFLOAT bsval1 = basis.Get(1); cjFLOAT bsval2 = basis.Get(2); cjFLOAT bsval3 = basis.Get(3);

			cjQuat quat;
			            
			quat.x = bsval0 * bsRotSeg->m_quatRotCP[1] + bsval1 * bsRotSeg->m_quatRotCP[5] + bsval2 * bsRotSeg->m_quatRotCP[9] + bsval3 * bsRotSeg->m_quatRotCP[13];
            quat.y = bsval0 * bsRotSeg->m_quatRotCP[2] + bsval1 * bsRotSeg->m_quatRotCP[6] + bsval2 * bsRotSeg->m_quatRotCP[10] + bsval3 * bsRotSeg->m_quatRotCP[14];
            quat.z = bsval0 * bsRotSeg->m_quatRotCP[3] + bsval1 * bsRotSeg->m_quatRotCP[7] + bsval2 * bsRotSeg->m_quatRotCP[11] + bsval3 * bsRotSeg->m_quatRotCP[15];
			quat.w = bsval0 * bsRotSeg->m_quatRotCP[0] + bsval1 * bsRotSeg->m_quatRotCP[4] + bsval2 * bsRotSeg->m_quatRotCP[8] + bsval3 * bsRotSeg->m_quatRotCP[12];
			quat.Normalize();

            *(cjQuat*)rst = quat;
        }
        else
        {
            cjAssert(channel == cjAniEvalChan_Scl);

            cjAniSPad::BSVec3Seg* bsVec3Seg = (cjAniSPad::BSVec3Seg*)evalSPDat->GetSegDat();
            cjAssert(bsVec3Seg->m_bsBasisDat);

            cjBSplineBasisF3& basis = bsVec3Seg->m_bsBasisDat->GetBasis();

            cjFLOAT bsval0 = basis.Get(0); cjFLOAT bsval1 = basis.Get(1); cjFLOAT bsval2 = basis.Get(2); cjFLOAT bsval3 = basis.Get(3);

			((cjFLOAT*)rst)[0] = bsval0 * bsVec3Seg->m_src[0] + bsval1 * bsVec3Seg->m_src[3] + bsval2 * bsVec3Seg->m_src[6] + bsval3 * bsVec3Seg->m_src[9];
            ((cjFLOAT*)rst)[1] = bsval0 * bsVec3Seg->m_src[1] + bsval1 * bsVec3Seg->m_src[4] + bsval2 * bsVec3Seg->m_src[7] + bsval3 * bsVec3Seg->m_src[10];
            ((cjFLOAT*)rst)[2] = bsval0 * bsVec3Seg->m_src[2] + bsval1 * bsVec3Seg->m_src[5] + bsval2 * bsVec3Seg->m_src[8] + bsval3 * bsVec3Seg->m_src[11];
        }
    }

    return cjTrue;
}

cjBool cjBSTMEvaluator::GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, cjUINT32& fillSiz, cjBool& isSharedFillDat, 
	cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const
{
    cjAssert(channel < cjAniEvalChan_PRS_Max);

    cjBool isPosed = IsChannelItemPosed(channel);
    if(IsChannelItemInvalid(channel) || (isPosed && !forceAlwaysUpdate && !IsAlwaysUpdate()))
        return cjFalse;

    if(channel == cjAniEvalChan_Pos)
    {
        if(isPosed)
        {
            fillSiz = 0; 
			spadBlk = cjAniSPADB_CubicVec3PSeg;
        }
        else
        {
            fillSiz = sizeof(cjAniSPad::BSVec3Fill); 
			spadBlk = cjAniSPADB_BSVec3PSeg;
        }
    }
    else if(channel == cjAniEvalChan_Rot)
    {
        if(isPosed)
        {
            fillSiz = 0; 
			spadBlk = cjAniSPADB_SlerpRotSeg;
        }
        else
        {
            fillSiz = sizeof(cjAniSPad::BSRotFill);
            spadBlk = cjAniSPADB_BSRotSeg;
        }
    }
    else
    {
        cjAssert(channel == cjAniEvalChan_Scl);

        if(isPosed)
        {
            fillSiz = 0;
            spadBlk = cjAniSPADB_CubicVec3SSeg;
        }
        else
        {
            fillSiz = sizeof(cjAniSPad::BSVec3Fill);
            spadBlk = cjAniSPADB_BSVec3SSeg;
        }
    }

    isSharedFillDat = cjFalse;
    bsBasisDat = isPosed ? cjNULL : m_spBasisDat;

    return cjTrue;
}

cjBool cjBSTMEvaluator::InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, 
	cjBool initSharedDat, cjRefObjNameHash* nodeHash, cjAniPoseBufHandle pbHandle) const
{
    cjAssert(channel < cjAniEvalChan_PRS_Max);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(!IsChannelItemInvalid(channel));

    cjAssert(evalSPDat->GetSegMinTime() == CJ_FLT_INFINITY);
    cjAssert(evalSPDat->GetSegMaxTime() == CJ_FLT_INVALID);

    cjBool isPosed = IsChannelItemPosed(channel);
	
    if(channel == cjAniEvalChan_Pos)
    {
        if(isPosed)
        {
            evalSPDat->SetSegTimeRange(0.0f, CJ_FLT_INFINITY);
            cjAssert(evalSPDat->GetFillFunc() == cjNULL);

            cjAniSPad::CubicVec3Seg* cubicVec3PSeg = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();
            cubicVec3PSeg->m_hdr.m_lod = evalSPDat->GetLOD();

            if(IsReferenced())
            {
                cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
                cubicVec3PSeg->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
            }
            else
            {
                cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Vec3P);
                cubicVec3PSeg->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
            }

           cubicVec3PSeg->m_stTime = 0.0f;
           cubicVec3PSeg->m_invDTime = 0.0f;
           cubicVec3PSeg->m_val0 = m_value.GetPos();
           cubicVec3PSeg->m_outTangent0 = cjVec3::ZERO;
           cubicVec3PSeg->m_a0 = cjVec3::ZERO;
           cubicVec3PSeg->m_b0 = cjVec3::ZERO;
        }
        else
        {
            cjAssert(m_posCPOffset != cjBSplineDat::INVALID_BSOFFSET);
            cjAssert(m_spDat);
            cjAssert(m_spBasisDat);

            evalSPDat->SetFillFunc(&cjBSVec3PEvaluator::BSVec3PFillFunc);

            cjAniSPad::BSVec3Fill* fillDat = (cjAniSPad::BSVec3Fill*)evalSPDat->GetFillDat();
			fillDat->m_cpOffset = m_posCPOffset;
            fillDat->m_stTime = m_stTime;
            fillDat->m_invDTime = (m_enTime != m_stTime) ? 1.0f / (m_enTime - m_stTime) : 0.0f;
            fillDat->m_lastMin = CJ_INT_INVALID;
			fillDat->m_bsDat = m_spDat;

            cjAniSPad::BSVec3Seg* bsVec3PSeg = (cjAniSPad::BSVec3Seg*)evalSPDat->GetSegDat();
            bsVec3PSeg->m_hdr.m_lod = evalSPDat->GetLOD();
            if(IsReferenced())
            {
                cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
                bsVec3PSeg->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
            }
            else
            {
                cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Vec3P);
                bsVec3PSeg->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
            }

			cjAssert(bsBasisDat);
            bsVec3PSeg->m_bsBasisDat = bsBasisDat;
        }
    }
    else if(channel == cjAniEvalChan_Rot)
    {
        if(isPosed)
        {
            evalSPDat->SetSegTimeRange(0.0f, CJ_FLT_INFINITY);
            cjAssert(evalSPDat->GetFillFunc() == cjNULL);

            cjAniSPad::SlerpRotSeg* slerpRotSeg = (cjAniSPad::SlerpRotSeg*)evalSPDat->GetSegDat();
            slerpRotSeg->m_hdr.m_lod = evalSPDat->GetLOD();

            if(IsReferenced())
            {
                cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
                slerpRotSeg->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
            }
            else
            {
                cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Rot);
                slerpRotSeg->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
            }

            slerpRotSeg->m_stTime = 0.0f;
            slerpRotSeg->m_invDTime = 0.0f;
            slerpRotSeg->m_quat0 = m_value.GetRotate();
            slerpRotSeg->m_quat1 = slerpRotSeg->m_quat0;
        }
        else
        {
            cjAssert(m_rotCPOffset != cjBSplineDat::INVALID_BSOFFSET);
            cjAssert(m_spDat);
            cjAssert(m_spBasisDat);

            evalSPDat->SetFillFunc(&BSRotFillFunc);
            cjAniSPad::BSRotFill* fillDat = (cjAniSPad::BSRotFill*)evalSPDat->GetFillDat();

			fillDat->m_cpOffset = m_rotCPOffset;
            fillDat->m_stTime = m_stTime;
            fillDat->m_invDTime = (m_enTime != m_stTime) ? 1.0f / (m_enTime - m_stTime) : 0.0f;
            fillDat->m_lastMin = CJ_INT_INVALID;
            fillDat->m_bsDat = m_spDat;

            cjAniSPad::BSRotSeg* bsRotSeg = (cjAniSPad::BSRotSeg*)evalSPDat->GetSegDat();
            bsRotSeg->m_hdr.m_lod = evalSPDat->GetLOD();

            if(IsReferenced())
            {
                cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
                bsRotSeg->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
            }
            else
            {
                cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Rot);
                bsRotSeg->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
            }

            cjAssert(bsBasisDat);
            bsRotSeg->m_bsBasisDat = bsBasisDat;
        }
    }
    else
    {
        cjAssert(channel == cjAniEvalChan_Scl);

        if(isPosed)
        {
            evalSPDat->SetSegTimeRange(0.0f, CJ_FLT_INFINITY);
            cjAssert(evalSPDat->GetFillFunc() == cjNULL);

            cjAniSPad::CubicVec3Seg* cubicVec3SSeg = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();
            cubicVec3SSeg->m_hdr.m_lod = evalSPDat->GetLOD();

            if(IsReferenced())
            {
                cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
                cubicVec3SSeg->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
            }
            else
            {
                cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Vec3S);
                cubicVec3SSeg->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
            }

            cubicVec3SSeg->m_stTime = 0.0f;
            cubicVec3SSeg->m_invDTime = 0.0f;
            cubicVec3SSeg->m_val0 = m_value.GetScale();
            cubicVec3SSeg->m_outTangent0 = cjVec3::ZERO;
            cubicVec3SSeg->m_a0 = cjVec3::ZERO;
            cubicVec3SSeg->m_b0 = cjVec3::ZERO;
        }
        else
        {
            cjAssert(m_scaleCPOffset != cjBSplineDat::INVALID_BSOFFSET);
            cjAssert(m_spDat);
            cjAssert(m_spBasisDat);

            evalSPDat->SetFillFunc(&cjBSVec3SEvaluator::BSVec3SFillFunc);
            cjAniSPad::BSVec3Fill* fillDat = (cjAniSPad::BSVec3Fill*)evalSPDat->GetFillDat();

			fillDat->m_cpOffset = m_scaleCPOffset;
            fillDat->m_stTime = m_stTime;
            fillDat->m_invDTime = (m_enTime != m_stTime) ? 1.0f / (m_enTime - m_stTime) : 0.0f;
            fillDat->m_lastMin = CJ_INT_INVALID;
            fillDat->m_bsDat = m_spDat;

            cjAniSPad::BSVec3Seg* bsVec3SSeg = (cjAniSPad::BSVec3Seg*)evalSPDat->GetSegDat();
            bsVec3SSeg->m_hdr.m_lod = evalSPDat->GetLOD();

            if(IsReferenced())
            {
                cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
                bsVec3SSeg->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
            }
            else
            {
                cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Vec3S);
                bsVec3SSeg->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
            }

			cjAssert(bsBasisDat);
            bsVec3SSeg->m_bsBasisDat = bsBasisDat;
        }
    }

    return cjTrue;
}

cjBSplineDat::BSOffset cjBSTMEvaluator::GetCPOffset(cjUINT32 channel) const
{
    switch(channel)
    {
        case cjAniEvalChan_Pos:	return m_posCPOffset;
        case cjAniEvalChan_Rot:	return m_rotCPOffset;
        case cjAniEvalChan_Scl:	return m_scaleCPOffset;
        default: return cjBSplineDat::INVALID_BSOFFSET;
    }
}

void cjBSTMEvaluator::SetCPOffset(cjBSplineDat::BSOffset cpOffset, cjUINT32 channel)
{
    switch(channel)
    {
        case cjAniEvalChan_Pos:	m_posCPOffset = cpOffset; break;
        case cjAniEvalChan_Rot: m_rotCPOffset = cpOffset; break;
        case cjAniEvalChan_Scl:	m_scaleCPOffset = cpOffset; break;
    }

    SetEvalChannelTypes();
}

void cjBSTMEvaluator::SetPosePos(const cjVec3& pos)
{
    m_value.SetPos(pos);

    if(m_spDat)
        m_posCPOffset = cjBSplineDat::INVALID_BSOFFSET;

    SetEvalChannelTypes();
}

void cjBSTMEvaluator::SetPoseRotate(const cjQuat& rotate)
{
    m_value.SetRotate(rotate);

    if(m_spDat)
        m_rotCPOffset = cjBSplineDat::INVALID_BSOFFSET;

    SetEvalChannelTypes();
}

void cjBSTMEvaluator::SetPoseScale(const cjVec3& scale)
{
    m_value.SetScale(scale);

    if(m_spDat)
        m_scaleCPOffset = cjBSplineDat::INVALID_BSOFFSET;

    SetEvalChannelTypes();
}

void cjBSTMEvaluator::SetPoseValue(const cjQuatTM& poseValue)
{
    SetPosePos(poseValue.GetPos());
    SetPoseRotate(poseValue.GetRotate());
    SetPoseScale(poseValue.GetScale());

    m_spDat = cjNULL;

    SetEvalChannelTypes();
}

cjBool cjBSTMEvaluator::BSRotFillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat)
{
    cjAssert(evalSPDat);

    cjAniSPad::BSRotFill* fillDat = (cjAniSPad::BSRotFill*)evalSPDat->GetFillDat();
    cjAniSPad::BSRotSeg* bsRotSeg = (cjAniSPad::BSRotSeg*)evalSPDat->GetSegDat();

    cjINT32 vMin, vMax;
    cjBSplineBasisF3& basis = bsRotSeg->m_bsBasisDat->GetBasis();
    cjFLOAT ratio = (etime - fillDat->m_stTime) * fillDat->m_invDTime;
    basis.Compute(ratio, vMin, vMax);

    if(vMin != fillDat->m_lastMin)
    {
		const cjFLOAT* src = fillDat->m_bsDat->GetCP(fillDat->m_cpOffset, vMin, 4);

        cjFLOAT* dst = bsRotSeg->m_quatRotCP;
        cjFLOAT* endDst = dst + 16;

        while(dst < endDst)
        {
            *dst = *src;
            src++;
            dst++;
        }

        fillDat->m_lastMin = vMin;
    }

    evalSPDat->SetSegTimeRange(etime, etime);

    return cjTrue;
}

void cjBSTMEvaluator::SetEvalChannelTypes()
{
    cjBool isValidBSPos = (GetCPOffset(cjAniEvalChan_Pos) > 0);

	if(isValidBSPos || m_value.IsPosValid())
    {
        m_channelItem[cjAniEvalChan_Pos] = cjAniEvalChanItem_Vec3P;

        if(!isValidBSPos)
            m_channelItem[cjAniEvalChan_Pos] |= cjAniEvalChanItemMask_Constant;
    }
    else
    {
        m_channelItem[cjAniEvalChan_Pos] = cjAniEvalChanItem_NoData;
    }

    cjBool isValidBSRot = (GetCPOffset(cjAniEvalChan_Rot) > 0);
    if(isValidBSRot || m_value.IsRotateValid())
    {
        m_channelItem[cjAniEvalChan_Rot] = cjAniEvalChanItem_Rot;

        if(!isValidBSRot)
            m_channelItem[cjAniEvalChan_Rot] |= cjAniEvalChanItemMask_Constant;
    }
    else
    {
        m_channelItem[cjAniEvalChan_Rot] = cjAniEvalChanItem_NoData;
    }

    cjBool isValidBSScale = (GetCPOffset(cjAniEvalChan_Scl) > 0);

    if(isValidBSScale || m_value.IsScaleValid())
    {
        m_channelItem[cjAniEvalChan_Scl] = cjAniEvalChanItem_Vec3S;

        if(!isValidBSScale)
            m_channelItem[cjAniEvalChan_Scl] |= cjAniEvalChanItemMask_Constant;
    }
    else
    {
        m_channelItem[cjAniEvalChan_Scl] = cjAniEvalChanItem_NoData;
    }
}


//
// cjLookAtEvaluator
//

CJSER_IMPL_CLASS(cjLookAtEvaluator, cjAniEvaluator);

CJSER_IMPL_SERIALIZE_BEGIN(cjLookAtEvaluator)
	
	if(ser.IsQuerySerObjAndStr())
	{
		for(cjINT32 i=0; i<EVAL_MAX; i++)
		{
			if(m_spEvaluators[i])
				(*m_spEvaluators[i]) >> ser;
		}

		return;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		CJFLAG_SERIALIZE

		ser << m_lookAt;
		ser << m_driven;

		for(cjINT32 i=0; i<EVAL_MAX; i++)
			ser.SerializeSerObjPtr(&m_spEvaluators[i]);
	}
	
CJSER_IMPL_SERIALIZE_END(cjLookAtEvaluator)

CJSER_IMPL_SERIALIZE_POST_BEGIN(cjLookAtEvaluator)

	if(ser.IsLoading())
	{
		for(cjINT32 i=0; i<EVAL_MAX; i++)
			ser.PostSerializeSerObjPtr(m_spEvaluators[i]);
	}

CJSER_IMPL_SERIALIZE_POST_END(cjLookAtEvaluator)

CJSER_IMPL_CLONE(cjLookAtEvaluator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjLookAtEvaluator)

	CJFLAG_CLONE

	d->m_lookAt = m_lookAt;
	d->m_driven = m_driven;

	for(cjINT32 i=0; i<cjLookAtEvaluator::EVAL_MAX; i++)
	{
		if(m_spEvaluators[i])
			d->m_spEvaluators[i] = (cjAniEvaluator*)(m_spEvaluators[i]->Clone());
	}

CJSER_IMPL_CLONE_PROPCOPY_END(cjLookAtEvaluator)


cjLookAtEvaluator::cjLookAtEvaluator() 
{
	CJFLAG_IMPL_INIT

    SetFlip(cjFalse);
    SetAxis(cjAxisX);

    for(cjUINT32 i=0; i<EVAL_MAX; i++)
        m_spEvaluators[i] = cjNULL;

    m_channelItem[cjAniEvalChan_Flag] |= (cjAniEvalFlagChanVal_Transform | 
		cjAniEvalFlagChanVal_AlwaysUpdate | cjAniEvalFlagChanVal_ShutDown);

    SetEvalChannelTypes();
}

cjLookAtEvaluator::cjLookAtEvaluator(const cjName& lookAt, const cjName& driven, cjAxis axis, cjBool isFlip)
{
	CJFLAG_IMPL_INIT

	m_lookAt = lookAt;
	m_driven = driven;

	SetAxis(axis);
	SetFlip(isFlip);
    
    for(cjUINT32 i=0; i<EVAL_MAX; i++)
        m_spEvaluators[i] = cjNULL;

    m_channelItem[cjAniEvalChan_Flag] |= (cjAniEvalFlagChanVal_Transform | 
		cjAniEvalFlagChanVal_AlwaysUpdate | cjAniEvalFlagChanVal_ShutDown);

    SetEvalChannelTypes();
}

cjLookAtEvaluator::~cjLookAtEvaluator()
{
    for(cjUINT32 i=0; i<EVAL_MAX; i++)
        m_spEvaluators[i] = cjNULL;
}

cjBool cjLookAtEvaluator::GetChannelPosedValue(cjUINT32 channel, void* rst) const
{
    cjAssert(channel < cjAniEvalChan_PRS_Max);

    if(IsChannelItemPosed(channel))
    {
        cjAssert(!IsChannelItemInvalid(channel));
        cjAssert(channel != cjAniEvalChan_Rot);

        if(channel == cjAniEvalChan_Pos)
        {
            cjAssert(m_spEvaluators[EVAL_POS]);
            m_spEvaluators[EVAL_POS]->GetChannelPosedValue(0, rst);
        }
        else
        {
            cjAssert(channel == cjAniEvalChan_Scl);

            cjAssert(m_spEvaluators[EVAL_SCALE]);
            m_spEvaluators[EVAL_SCALE]->GetChannelPosedValue(0, rst);
        }

        return cjTrue;
    }

    return cjFalse;
}

cjBool cjLookAtEvaluator::UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const
{
    cjAssert(channel < cjAniEvalChan_PRS_Max);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);

    if(IsChannelItemInvalid(channel))
        return cjFalse;

    if(!evalSPDat->IsSegDatValid(etime))
    {
        cjAniEvalSPadFillFunc pfFillFunc = evalSPDat->GetFillFunc();

        if(!pfFillFunc || !(*pfFillFunc)(etime, evalSPDat))
            return cjFalse;
    }

    if(channel == cjAniEvalChan_Rot)
    {
        cjAniSPad::SlerpRotSeg* slerpRot = (cjAniSPad::SlerpRotSeg*)evalSPDat->GetSegDat();
        *(cjQuat*)rst = slerpRot->m_quat0;
    }
    else if(channel == cjAniEvalChan_Pos)
    {
        cjAniSPad::CubicVec3Seg* cubicVec3P = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();
        *(cjVec3*)rst = cubicVec3P->m_val0;
    }
    else
    {
        cjAssert(channel == cjAniEvalChan_Scl);

        cjAniSPad::CubicVec3Seg* cubicVec3S = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();
		*(cjVec3*)rst = cubicVec3S->m_val0;
    }

    return cjTrue;
}

void cjLookAtEvaluator::GetValidTimeRange(cjFLOAT& stTime, cjFLOAT& enTime) const
{
    stTime = CJ_FLT_INFINITY;
    enTime = CJ_FLT_INVALID;
    
    for(cjUINT32 i=0; i<EVAL_MAX; i++)
    {
        if(m_spEvaluators[i])
        {
            cjFLOAT st, en;
            m_spEvaluators[i]->GetValidTimeRange(st, en);

            if(st != en)
            {
                if(st < stTime)
                    stTime = st;

                if(en > enTime)
                    enTime = en;
            }
        }
    }

    if(stTime == CJ_FLT_INFINITY && enTime == CJ_FLT_INVALID)
    {
        stTime = 0.0f;
        enTime = 0.0f;
    }
}

void cjLookAtEvaluator::CacheKeys(cjFLOAT stTime, cjFLOAT enTime)
{
    for(cjUINT32 i=0; i< EVAL_MAX; i++)
    {
        if(m_spEvaluators[i])
            m_spEvaluators[i]->CacheKeys(stTime, enTime);
    }
}

cjAniEvaluator* cjLookAtEvaluator::GetEvaluator(cjFLOAT stTime, cjFLOAT enTime)
{
    cjLookAtEvaluator* eval = (cjLookAtEvaluator*)cjAniEvaluator::GetEvaluator(stTime, enTime);

    for(cjUINT32 i=0; i<EVAL_MAX; i++)
    {
        if(m_spEvaluators[i])
            eval->m_spEvaluators[i] = m_spEvaluators[i]->GetEvaluator(stTime, enTime);
    }

    return eval;
}

cjBool cjLookAtEvaluator::GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, 
	cjUINT32& fillSiz, cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const
{
    cjAssert(channel < cjAniEvalChan_PRS_Max);
    cjAssert(nodeHash);

    cjBool isPosed = IsChannelItemPosed(channel);

    if(IsChannelItemInvalid(channel) || (isPosed && !forceAlwaysUpdate && !IsAlwaysUpdate()))
        return cjFalse;

	if(!m_lookAt.IsValid() || !m_driven.IsValid())
        return cjFalse;

	cjRefObj* lookAtObj = nodeHash->FindObjWithSetObjWhenRootIsSetted(m_lookAt);
    if(!lookAtObj)
        return cjFalse;

	cjRefObj* drivenObj = nodeHash->FindObjWithSetObjWhenRootIsSetted(m_driven);
    if(!drivenObj)
        return cjFalse;

    cjUINT32 totalExtFillSiz = 0;
    cjUINT32 extFillSiz;
    cjBool isExtSharedFillDat;

    cjAniSPadBlock extSPBSegBlk;
    cjBSplineBasisF3Dat* extBsBasisDat;

    if(m_spEvaluators[EVAL_POS])
    {
        if(m_spEvaluators[EVAL_POS]->GetSPadInfo(0, cjFalse, nodeHash, extFillSiz, isExtSharedFillDat, extSPBSegBlk, extBsBasisDat))
        {
            totalExtFillSiz += extFillSiz;

            if(extSPBSegBlk == cjAniSPADB_CubicVec3PSeg)
            {
                totalExtFillSiz += sizeof(cjAniSPad::CubicVec3Seg);
                cjAssert(!extBsBasisDat);
            }
            else
            {
                cjAssert(extSPBSegBlk == cjAniSPADB_BSVec3PSeg);
				cjAssert(extBsBasisDat);

                totalExtFillSiz += sizeof(cjAniSPad::BSVec3Seg);
                totalExtFillSiz += sizeof(cjBSplineBasisF3Dat);
            }
        }
    }

    if(m_spEvaluators[EVAL_ROLL])
    {
        if(m_spEvaluators[EVAL_ROLL]->GetSPadInfo(0, cjFalse, nodeHash, extFillSiz, isExtSharedFillDat, extSPBSegBlk, extBsBasisDat))
        {
            totalExtFillSiz += extFillSiz;

            if(extSPBSegBlk == cjAniSPADB_CubicFloatSeg)
            {
                totalExtFillSiz += sizeof(cjAniSPad::CubicFloatSeg);
                cjAssert(!extBsBasisDat);
            }
            else
            {
                cjAssert(extSPBSegBlk == cjAniSPADB_BSFloatSeg);
				cjAssert(extBsBasisDat);

                totalExtFillSiz += sizeof(cjAniSPad::BSFloatSeg);
                totalExtFillSiz += sizeof(cjBSplineBasisF3Dat);
            }
        }
    }

    if(m_spEvaluators[EVAL_SCALE])
    {
        if(m_spEvaluators[EVAL_SCALE]->GetSPadInfo(0, cjFalse, nodeHash, extFillSiz, isExtSharedFillDat, extSPBSegBlk, extBsBasisDat))
        {
            totalExtFillSiz += extFillSiz;

            if(extSPBSegBlk == cjAniSPADB_CubicVec3SSeg)
            {
                totalExtFillSiz += sizeof(cjAniSPad::CubicVec3Seg);
                cjAssert(!extBsBasisDat);
            }
            else
            {
                cjAssert(extSPBSegBlk == cjAniSPADB_BSVec3SSeg);
				cjAssert(extBsBasisDat);

                totalExtFillSiz += sizeof(cjAniSPad::BSVec3Seg);                
                totalExtFillSiz += sizeof(cjBSplineBasisF3Dat);
            }
        }
    }

    fillSiz = sizeof(cjAniSPad::LookAtFill) + totalExtFillSiz;

    if(channel == cjAniEvalChan_Pos)
    {
        spadBlk = cjAniSPADB_CubicVec3PSeg;
    }
    else if(channel == cjAniEvalChan_Rot)
    {
        spadBlk = cjAniSPADB_SlerpRotSeg;
    }
    else
    {
        cjAssert(channel == cjAniEvalChan_Scl);
        spadBlk = cjAniSPADB_CubicVec3SSeg;
    }

    isSharedFillDat = cjTrue;
    bsBasisDat = 0;

    return cjTrue;
}

cjBool cjLookAtEvaluator::InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* /*bsBasisDat*/, 
	cjBool initSharedDat, cjRefObjNameHash* nodeHash, cjAniPoseBufHandle pbHandle) const
{
    cjAssert(channel < cjAniEvalChan_PRS_Max);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(!IsChannelItemInvalid(channel));
    cjAssert(nodeHash);

	if(!m_lookAt.IsValid() || !m_driven.IsValid())
        return cjFalse;

	cjRefObj* lookAtObj = nodeHash->FindObjWithSetObjWhenRootIsSetted(m_lookAt);

    if(!lookAtObj)
        return cjFalse;

	cjRefObj* drivenObj = nodeHash->FindObjWithSetObjWhenRootIsSetted(m_driven);
    if(!drivenObj)
        return cjFalse;

	if(!lookAtObj->GetSceneNode() || !drivenObj->GetSceneNode())
		return cjFalse;

    cjBool isPosed = IsChannelItemPosed(channel);
    cjAssert(evalSPDat->GetSegMinTime() == CJ_FLT_INFINITY);
    cjAssert(evalSPDat->GetSegMaxTime() == CJ_FLT_INVALID);

	evalSPDat->SetFillFunc(&LookAtFillFunc);

    cjAniSPad::LookAtFill* lookAtFill = (cjAniSPad::LookAtFill*)evalSPDat->GetFillDat();
    cjAssert(lookAtFill);

    if(initSharedDat)
    {
        cjUINT32 fillSiz;
        cjBool isSharedFillDat;
        cjAniSPadBlock spadBlk;
        cjBSplineBasisF3Dat* spadBSBasisDat;

        GetSPadInfo(channel, cjTrue, nodeHash, fillSiz, isSharedFillDat, spadBlk, spadBSBasisDat);
        memset(lookAtFill, 0, fillSiz);

        lookAtFill->m_lastTime = CJ_FLT_INVALID;
        lookAtFill->m_flags = (cjWORD)CJFLAG_GET;
        lookAtFill->m_lookAt = lookAtObj->GetSceneNode();
        lookAtFill->m_driven = drivenObj->GetSceneNode();
        lookAtFill->m_roll = CJ_FLT_INVALID;

		// [lookAtFill][extFillDat]
        cjBYTE* extFillDat = (cjBYTE*)(lookAtFill + 1);

        for(cjUINT32 i=0; i<EVAL_MAX; i++)
        {
            cjAssert(lookAtFill->m_bsBasisDat[i] == cjNULL);

            cjAniEvalSPDat* curEvaluatorSPDat = &lookAtFill->m_evalSPDat[i];

            new(curEvaluatorSPDat) cjAniEvalSPDat();

            curEvaluatorSPDat->SetLOD(evalSPDat->GetLOD());

            cjAssert(curEvaluatorSPDat->GetEvalChannel() == cjAniEvalChan_Base);
            cjAssert(curEvaluatorSPDat->GetFlag() == 0);
            cjAssert(curEvaluatorSPDat->GetSegMinTime() == CJ_FLT_INFINITY);
            cjAssert(curEvaluatorSPDat->GetSegMaxTime() == CJ_FLT_INVALID);
            cjAssert(curEvaluatorSPDat->GetFillFunc() == cjNULL);
            cjAssert(curEvaluatorSPDat->GetFillDat() == cjNULL);
            cjAssert(curEvaluatorSPDat->GetSegDat() == cjNULL);

            curEvaluatorSPDat->SetEvaluator(m_spEvaluators[i]);

            if(m_spEvaluators[i])
            {
                cjUINT32 extFillSiz;
                cjBool isExtSharedFillDat;
                cjAniSPadBlock extSPBSegBlk;
                cjBSplineBasisF3Dat* extBsBasisDat;

                if(m_spEvaluators[i]->GetSPadInfo(0, cjFalse, nodeHash, extFillSiz, isExtSharedFillDat, extSPBSegBlk, extBsBasisDat))
                {
                    if(extFillSiz > 0)
                    {
                        curEvaluatorSPDat->SetFillDat(extFillDat);
                        extFillDat += extFillSiz;
                    }

                    cjAniPoseBufChanItem pbChanItem = cjAniPoseBufChanItem_Float;
                    curEvaluatorSPDat->SetSegDat(extFillDat);

                    switch(extSPBSegBlk)
                    {
                        case cjAniSPADB_CubicFloatSeg:  extFillDat += sizeof(cjAniSPad::CubicFloatSeg); break;
                        case cjAniSPADB_BSFloatSeg:		extFillDat += sizeof(cjAniSPad::BSFloatSeg); break;

                        case cjAniSPADB_CubicVec3SSeg:	extFillDat += sizeof(cjAniSPad::CubicVec3Seg);	pbChanItem = cjAniPoseBufChanItem_Vec3S; break;
                        case cjAniSPADB_BSVec3SSeg:		extFillDat += sizeof(cjAniSPad::BSVec3Seg);		pbChanItem = cjAniPoseBufChanItem_Vec3S; break;

                        case cjAniSPADB_CubicVec3PSeg:	extFillDat += sizeof(cjAniSPad::CubicVec3Seg);	pbChanItem = cjAniPoseBufChanItem_Vec3P; break;
                        case cjAniSPADB_BSVec3PSeg:		extFillDat += sizeof(cjAniSPad::BSVec3Seg);		pbChanItem = cjAniPoseBufChanItem_Vec3P; break;

						default:
                            cjAssert(cjFalse);
                            break;
                    }

                    cjBSplineBasisF3Dat* tmpBSBasisDat = cjNULL;

                    if(extBsBasisDat)
                    {
                        //new(extFillDat) cjBSplineBasisF3Dat();
						cjBSplineBasisF3Dat::PNew(extFillDat);

                        tmpBSBasisDat = (cjBSplineBasisF3Dat*)extFillDat;
						lookAtFill->m_bsBasisDat[i] = tmpBSBasisDat;
                        extFillDat += sizeof(cjBSplineBasisF3Dat);

                        spadBSBasisDat->AddRef();
                        spadBSBasisDat->SetBasis(&extBsBasisDat->GetBasis());
                    }

                    m_spEvaluators[i]->InitSPadDat(0, curEvaluatorSPDat, tmpBSBasisDat, cjTrue, nodeHash, 
                        cjAniPoseBufHandle(pbChanItem, cjAniSPad::EINVALID_OUTPUT_INDEX));
                }
            }
        }

        if(m_spEvaluators[EVAL_POS])
        {
            cjVec3 pos;

            if(m_spEvaluators[EVAL_POS]->GetChannelPosedValue(0, &pos))
                lookAtFill->m_pos = pos;
        }

        if(m_spEvaluators[EVAL_ROLL])
        {
            cjFLOAT roll;

			if(m_spEvaluators[EVAL_ROLL]->GetChannelPosedValue(0, &roll))
                lookAtFill->m_roll = roll;
        }

        if(m_spEvaluators[EVAL_SCALE])
        {
            cjVec3 scale;

            if(m_spEvaluators[EVAL_SCALE]->GetChannelPosedValue(0, &scale))
                lookAtFill->m_scale = scale;
        }
    }

    if(channel == cjAniEvalChan_Pos)
    {
        cjAniSPad::CubicVec3Seg* cubicVec3Seg = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();
        cubicVec3Seg->m_hdr.m_lod = evalSPDat->GetLOD();

        if(IsReferenced())
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
            cubicVec3Seg->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
        }
        else
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Vec3P);
            cubicVec3Seg->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
        }

        cubicVec3Seg->m_stTime = 0.0f;
        cubicVec3Seg->m_invDTime = 0.0f;
        cubicVec3Seg->m_outTangent0 = cjVec3::ZERO;
        cubicVec3Seg->m_a0 = cjVec3::ZERO;
        cubicVec3Seg->m_b0 = cjVec3::ZERO;

        if(isPosed)
        {
            cubicVec3Seg->m_val0 = lookAtFill->m_pos;
            evalSPDat->SetSegTimeRange(0.0f, CJ_FLT_INFINITY);
        }
    }
    else if(channel == cjAniEvalChan_Rot)
    {
        cjAssert(!isPosed);

        cjAniSPad::SlerpRotSeg* slerpRotSeg = (cjAniSPad::SlerpRotSeg*)evalSPDat->GetSegDat();
        slerpRotSeg->m_hdr.m_lod = evalSPDat->GetLOD();

        if(IsReferenced())
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
            slerpRotSeg->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
        }
        else
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Rot);
            slerpRotSeg->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
        }

        slerpRotSeg->m_stTime = 0.0f;
        slerpRotSeg->m_invDTime = 0.0f;
    }
    else
    {
        cjAssert(channel == cjAniEvalChan_Scl);

        cjAniSPad::CubicVec3Seg* cubicVec3Seg = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();
        cubicVec3Seg->m_hdr.m_lod = evalSPDat->GetLOD();

        if(IsReferenced())
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
            cubicVec3Seg->m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
        }
        else
        {
            cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Vec3S);
            cubicVec3Seg->m_hdr.m_outputPBIndex = pbHandle.GetAuxIndex();
        }

       cubicVec3Seg->m_stTime = 0.0f;
       cubicVec3Seg->m_invDTime = 0.0f;
       cubicVec3Seg->m_outTangent0 = cjVec3::ZERO;
       cubicVec3Seg->m_a0 = cjVec3::ZERO;
       cubicVec3Seg->m_b0 = cjVec3::ZERO;
       // cubicFloatSeg->m_val0 = 0.0f;

        if(isPosed)
        {
            cubicVec3Seg->m_val0 = lookAtFill->m_scale;
            evalSPDat->SetSegTimeRange(0.0f, CJ_FLT_INFINITY);
        }
    }

    return cjTrue;
}

void cjLookAtEvaluator::DisposeSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat) const
{
    cjAssert(channel < cjAniEvalChan_PRS_Max);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(!IsChannelItemInvalid(channel));

    cjAniSPad::LookAtFill* lookAtFill = (cjAniSPad::LookAtFill*)evalSPDat->GetFillDat();
    cjAssert(lookAtFill);

    if(CJFLAG_EXTERNAL_GETMASKED_VAL(lookAtFill->m_flags, MASK_SHUTDOWN) == 0)
    {
		CJFLAG_EXTERNAL_SETMASKED_VAL(lookAtFill->m_flags, MASK_SHUTDOWN);

        lookAtFill->m_lookAt = cjNULL;
        lookAtFill->m_driven = cjNULL;

        for(cjUINT32 i=0; i<EVAL_MAX; i++)
            lookAtFill->m_evalSPDat[i].~cjAniEvalSPDat();

        for(cjUINT32 i=0; i<EVAL_MAX; i++)
        {
            if(lookAtFill->m_bsBasisDat[i])
            {
                cjAssert(lookAtFill->m_bsBasisDat[i]->GetRef() == 1);
                //lookAtFill->m_bsBasisDat[i]->~cjBSplineBasisF3Dat();				
				lookAtFill->m_bsBasisDat[i]->PDel();
            }
        }
    }
}

void cjLookAtEvaluator::Collapse()
{
    for(cjUINT32 i=0; i<EVAL_MAX; i++)
    {
        if(m_spEvaluators[i])
            m_spEvaluators[i]->Collapse();
    }

    SetEvalChannelTypes();
}

cjBool cjLookAtEvaluator::LookAtFillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat)
{
    cjAssert(evalSPDat);

    cjAniSPad::LookAtFill* lookAtFill = (cjAniSPad::LookAtFill*)evalSPDat->GetFillDat();
    cjAssert(lookAtFill->m_lookAt);
    cjAssert(lookAtFill->m_driven);

    for(cjUINT32 i=0; i<EVAL_MAX; i++)
    {
        cjAniEvalSPDat* curEvaluatorSPDat = &lookAtFill->m_evalSPDat[i];
        cjAniEvaluator* curEvaluator = curEvaluatorSPDat->GetEvaluator();

        if(curEvaluator && (curEvaluator->IsChannelItemPosed(0) || etime == lookAtFill->m_lastTime))
            continue;

        if(i == EVAL_POS)
        {
            if(curEvaluator)
                curEvaluator->UpdateChannel(etime, 0, curEvaluatorSPDat, &lookAtFill->m_pos);
            else
                lookAtFill->m_pos = lookAtFill->m_driven->GetPos();
        }
        else if(i == EVAL_ROLL)
        {
            if(curEvaluator)
                curEvaluator->UpdateChannel(etime, 0, curEvaluatorSPDat, &lookAtFill->m_roll);
        }
        else
        {
            cjAssert(i == EVAL_SCALE);

            if(curEvaluator)
                curEvaluator->UpdateChannel(etime, 0, curEvaluatorSPDat, &lookAtFill->m_scale);
            else
                lookAtFill->m_scale = lookAtFill->m_driven->GetScale();
        }
    }

    cjUINT32 channel = evalSPDat->GetEvalChannel();

	// pos
    if(channel == cjAniEvalChan_Pos)
    {
        cjAniSPad::CubicVec3Seg* cubicVec3Seg = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();

        cjAssert(cubicVec3Seg->m_stTime == 0);
        cjAssert(cubicVec3Seg->m_invDTime == 0);

        cubicVec3Seg->m_val0 = lookAtFill->m_pos;

        cjAssert(cubicVec3Seg->m_outTangent0 == cjVec3::ZERO);
        cjAssert(cubicVec3Seg->m_a0 == cjVec3::ZERO);
        cjAssert(cubicVec3Seg->m_b0 == cjVec3::ZERO);
    }
    else if(channel == cjAniEvalChan_Rot)
    {
		// rotation
        cjMatrix3 parentWorldRot;
        cjVec3 worldPos;
        cjNode* parent = lookAtFill->m_driven->GetParent();

        if(parent)
        {
			cjSceneNode* parentSceneNode = parent->GetSceneNode();
            parentWorldRot = parentSceneNode->GetWorldRotate();
            worldPos = parentSceneNode->GetWorldPos() + parentSceneNode->GetWorldScale() * (parentWorldRot * lookAtFill->m_pos);
        }
        else
        {
            parentWorldRot = cjMatrix3::Identity;
            worldPos = lookAtFill->m_pos;
        }

		// up axis
        cjVec3 lookAtWorldPos = lookAtFill->m_lookAt->GetWorldPos();    
        cjMatrix3 rot;
        cjVec3 at = lookAtWorldPos - worldPos;

        if(at.SquareXYZ() < 0.001f)
        {
            rot.SetIdentity();
        }
        else    
        {
            cjVec3 up;
            cjFLOAT dot;

            at.Normalize();

			// up & look-at are too close, use y axis as up
            if(((at.z < 1.001f) && (at.z > 0.999f)) || ((at.z > -1.001f) && (at.z < -0.999f)))
            {
                up = cjVec3::UNIT_Y;	dot = at.y;
            }
            else
            {
                up = cjVec3::UNIT_Z;	dot = at.z;
            }

            up -= dot*at;
            up.Normalize();

            if(CJFLAG_EXTERNAL_GETMASKED_VAL(lookAtFill->m_flags, MASK_FLIP) == 0)
                at = -at;

            cjVec3 right = up.Cross(at);

            switch (CJFLAG_EXTERNAL_GETMASKED_VAL(lookAtFill->m_flags, MASK_AXIS) >> BPOS_AXIS)
            {
                case cjAxisX: rot.SetCol(0, at);	rot.SetCol(1, up); rot.SetCol(2, -right);	break;
                case cjAxisY: rot.SetCol(0, right); rot.SetCol(1, at); rot.SetCol(2, -up);		break;
                case cjAxisZ: rot.SetCol(0, right); rot.SetCol(1, up); rot.SetCol(2, at);		break;
            }

			// column x column multiply
            rot = parentWorldRot.MultiplyColXCol(rot);

			if(lookAtFill->m_roll != CJ_FLT_INVALID)
            {
                cjMatrix3 roll;
                roll.MakeZRotation(-lookAtFill->m_roll);
                rot = rot * roll;
            }
        }

        cjQuat quatValue;
        quatValue.MakeFromRotation(rot);

        cjAniSPad::SlerpRotSeg* slerpRot = (cjAniSPad::SlerpRotSeg*)evalSPDat->GetSegDat();
        slerpRot->m_quat0 = quatValue;
        slerpRot->m_quat1 = quatValue;
    }
    else
    {
		// scale
        cjAssert(channel == cjAniEvalChan_Scl);

        cjAniSPad::CubicVec3Seg* cubicVec3Seg = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();

        cjAssert(cubicVec3Seg->m_stTime == 0);
        cjAssert(cubicVec3Seg->m_invDTime == 0);

        cubicVec3Seg->m_val0 = lookAtFill->m_scale;

        cjAssert(cubicVec3Seg->m_outTangent0 == cjVec3::ZERO);
        cjAssert(cubicVec3Seg->m_a0 == cjVec3::ZERO);
        cjAssert(cubicVec3Seg->m_b0  == cjVec3::ZERO);
    }

    lookAtFill->m_lastTime = etime;

    return cjTrue;
}

void cjLookAtEvaluator::SetEvalChannelTypes()
{    
	// rotation
    m_channelItem[cjAniEvalChan_Rot] = cjAniEvalChanItem_Rot; // rotation channel is never posed.. 

	// position
	m_channelItem[cjAniEvalChan_Pos] = cjAniEvalChanItem_NoData;
    if(m_spEvaluators[EVAL_POS])
    {
        cjAssert(m_spEvaluators[EVAL_POS]->GetPBChannelItemFromChanItemWithNoCareFlag(0) == cjAniPoseBufChanItem_Vec3P);
        if(!m_spEvaluators[EVAL_POS]->IsChannelItemInvalid(0))
        {
            m_channelItem[cjAniEvalChan_Pos] = cjAniEvalChanItem_Vec3P;

            if(m_spEvaluators[EVAL_POS]->IsChannelItemPosed(0))
                m_channelItem[cjAniEvalChan_Pos] |= cjAniEvalChanItemMask_Constant;
        }
    }

	// scale
    m_channelItem[cjAniEvalChan_Scl] = cjAniEvalChanItem_NoData;
    if(m_spEvaluators[EVAL_SCALE])
    {
        cjAssert(m_spEvaluators[EVAL_SCALE]->GetPBChannelItemFromChanItemWithNoCareFlag(0) == cjAniPoseBufChanItem_Vec3S);

        if(!m_spEvaluators[EVAL_SCALE]->IsChannelItemInvalid(0))
        {
            m_channelItem[cjAniEvalChan_Scl] = cjAniEvalChanItem_Vec3S;

            if(m_spEvaluators[EVAL_SCALE]->IsChannelItemPosed(0))
                m_channelItem[cjAniEvalChan_Scl] |= cjAniEvalChanItemMask_Constant;
        }
    }
}

void cjLookAtEvaluator::SetPosEvaluator(cjAniEvaluator* evaluator)
{
    if(evaluator)
    {
        cjAssert(evaluator->GetPBChannelItemFromChanItemWithNoCareFlag(0) == cjAniPoseBufChanItem_Vec3P);
        m_spEvaluators[EVAL_POS] = evaluator;
    }
    else
    {
        m_spEvaluators[EVAL_POS] = cjNULL;
    }

    SetEvalChannelTypes();
}

void cjLookAtEvaluator::SetRollEvaluator(cjAniEvaluator* evaluator)
{
    if(evaluator)
    {
        cjAssert(evaluator->GetPBChannelItemFromChanItemWithNoCareFlag(0) == cjAniPoseBufChanItem_Float);
        m_spEvaluators[EVAL_ROLL] = evaluator;
    }
    else
    {
        m_spEvaluators[EVAL_ROLL] = cjNULL;
    }

    // SetEvalChannelTypes();
}

void cjLookAtEvaluator::SetScaleEvaluator(cjAniEvaluator* evaluator)
{
    if(evaluator)
    {
        cjAssert(evaluator->GetPBChannelItemFromChanItemWithNoCareFlag(0) == cjAniPoseBufChanItem_Vec3S);
        m_spEvaluators[EVAL_SCALE] = evaluator;
    }
    else
    {
        m_spEvaluators[EVAL_SCALE] = cjNULL;
    }

    SetEvalChannelTypes();
}


//
// cjPathEvaluator
//

CJSER_IMPL_CLASS(cjPathEvaluator, cjKeyBasedEvaluator);

CJSER_IMPL_SERIALIZE_BEGIN(cjPathEvaluator)
	
	if(ser.IsQuerySerObjAndStr())
	{
		if(m_spPathDat)
			(*m_spPathDat) >> ser;

		if(m_spPCTDat)
			(*m_spPCTDat) >> ser;

		return;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		CJFLAG_SERIALIZE

		cjFStreamEnum(ser, m_bankDir);
		ser << m_maxBankAngle;
		ser << m_smooth;
		// ser << m_maxCurvature - do not serialize
		ser << m_followAxis;

		ser.SerializeSerObjPtr(&m_spPathDat);
		ser.SerializeSerObjPtr(&m_spPCTDat);

		if(ser.IsSaving())
		{
			SetConstSpeedDatDirty(cjTrue);

			if(IsConstSpeed())
				SetConstSpeedDat();

			cjUINT32 pathKeyCount; cjAniKeyVec3::EKEY_TYPE pathKeyType; cjBYTE pathKeySiz;
			cjAniKeyVec3* pathKeys = GetPathKeys(pathKeyCount, pathKeyType, pathKeySiz);

			if(pathKeyCount)
			{
				cjAniKeyVec3::PFCurvedRatio pfCurvedRatio = cjAniKeyVec3::GetCurvedRatioKeyPF(pathKeyType);
				m_maxCurvature = pfCurvedRatio((cjAniKeyVec3*)pathKeys, pathKeyCount);
			}
		}
	}
	
CJSER_IMPL_SERIALIZE_END(cjPathEvaluator)

CJSER_IMPL_SERIALIZE_POST_BEGIN(cjPathEvaluator)

	if(ser.IsLoading())
	{
		ser.PostSerializeSerObjPtr(m_spPathDat);
		ser.PostSerializeSerObjPtr(m_spPCTDat);
	}

CJSER_IMPL_SERIALIZE_POST_END(cjPathEvaluator)


CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjPathEvaluator)

	CJFLAG_CLONE
	
    if(m_spPathDat)
        d->SetPathDat((cjAniVec3Dat*)(m_spPathDat->_Clone(cloner)));

    if(m_spPCTDat)
        d->SetPCTDat((cjAniFloatDat*)(m_spPCTDat->_Clone(cloner)));

    d->SetAllowFlip(IsAllowFlip());
    d->SetBanking(IsBanking());
    d->SetBankDir(GetBankDir());
    d->SetConstSpeed(IsConstSpeed());
    d->SetFollow(IsFollow());
    d->SetMaxBankAngle(GetMaxBankAngle());
    d->SetSmoothing(GetSmoothing());
    d->SetFollowAxis(GetFollowAxis());
    d->SetFlip(GetFlip());
    d->SetOpenCurve(IsOpenCurve());

	d->m_pathLengthCount = 0;

	cjAssert(d->m_pathLengths == cjNULL);

    if(m_pathLengths)
    {
        cjUINT32 pathKeyCount; cjAniKeyVec3::EKEY_TYPE pathKeyType; cjBYTE pathKeySiz;
        GetPathKeys(pathKeyCount, pathKeyType, pathKeySiz);

        cjAssert(pathKeyCount > 0);
        d->m_pathLengths = (cjFLOAT*)cjMemAlloc(sizeof(cjFLOAT) * pathKeyCount);

        cjUINT32 dstSiz = pathKeyCount * sizeof(cjFLOAT);
		d->m_pathLengthCount = pathKeyCount;

        cjMemCpy(d->m_pathLengths, dstSiz, m_pathLengths, sizeof(m_pathLengths[0]) * pathKeyCount);
    }
	
	d->m_totalPathLength = m_totalPathLength;

CJSER_IMPL_CLONE_PROPCOPY_END(cjPathEvaluator)

CJSER_IMPL_CLONE(cjPathEvaluator)

CJSER_IMPL_CLONE_POST_BEGIN(cjPathEvaluator)

    if(m_spPathDat)
        m_spPathDat->_Clone(cloner);

    if(m_spPCTDat)
        m_spPCTDat->_Clone(cloner);

CJSER_IMPL_CLONE_POST_END(cjPathEvaluator)


cjPathEvaluator::cjPathEvaluator()
{
	CJFLAG_IMPL_INIT

    SetAllowFlip(cjFalse);
    SetConstSpeed(cjFalse);
    SetFollow(cjFalse);

	m_bankDir = BANKDIR_POSITIVE;
    m_maxBankAngle = 0.0f;
    m_smooth = 0.0f;
	m_maxCurvature = 0.0f;    
    m_followAxis = 0;

    SetFlip(cjFalse);
    SetOpenCurve(cjTrue);

	m_pathLengthCount = 0;
    m_pathLengths = 0;
    m_totalPathLength = -1.0f;

    SetConstSpeedDatDirty(cjTrue);

    m_channelItem[cjAniEvalChan_Flag] |= cjAniEvalFlagChanVal_Transform;
}

cjPathEvaluator::cjPathEvaluator(cjAniVec3Dat* pathDat, cjAniFloatDat* pctDat)
{
    CJFLAG_IMPL_INIT

    m_spPathDat = pathDat; 
    m_spPCTDat = pctDat;

    SetAllowFlip(cjFalse);
    SetConstSpeed(cjFalse);
    SetFollow(cjFalse);

	m_bankDir = BANKDIR_POSITIVE;
    m_maxBankAngle = 0.0f;
    m_smooth = 0.0f;
	m_maxCurvature = 0.0f;
    m_followAxis = 0;

    SetFlip(cjFalse);
    SetOpenCurve(cjTrue);   

	m_pathLengthCount = 0;
    m_pathLengths = 0;
    m_totalPathLength = -1.0f;

    SetConstSpeedDatDirty(cjTrue);

    m_channelItem[cjAniEvalChan_Flag] |= cjAniEvalFlagChanVal_Transform;

    SetEvalChannelTypes();
}

cjPathEvaluator::~cjPathEvaluator()
{
    m_spPathDat = 0;
    m_spPCTDat = 0;
    cjMemDeAlloc(m_pathLengths);
}


cjBool cjPathEvaluator::GetChannelPosedValue(cjUINT32 channel, void* rst) const
{
    cjAssert(channel < cjAniEvalChan_PRS_Max);
    cjAssert(rst);
    cjAssert(!IsChannelItemPosed(channel));

    return cjFalse;
}

cjBool cjPathEvaluator::UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const
{
    cjAssert(channel < cjAniEvalChan_PRS_Max);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);

    if(IsChannelItemInvalid(channel))
        return cjFalse;

    if(!evalSPDat->IsSegDatValid(etime))
    {
        cjAniEvalSPadFillFunc pfFillFunc = evalSPDat->GetFillFunc();

        if(!pfFillFunc || !(*pfFillFunc)(etime, evalSPDat))
            return cjFalse;
    }

    if(channel == cjAniEvalChan_Pos)
    {
        cjAniSPad::CubicVec3Seg* cubicVec3 = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();
        cjAssert(cubicVec3);
        *(cjVec3*)rst = cubicVec3->m_val0;
    }
    else
    {
        cjAssert(channel == cjAniEvalChan_Rot);
        cjAniSPad::SlerpRotSeg* slerpRot = (cjAniSPad::SlerpRotSeg*)evalSPDat->GetSegDat();
        *(cjQuat*)rst = slerpRot->m_quat0;
    }

    return cjTrue;
}

void cjPathEvaluator::GetValidTimeRange(cjFLOAT& stTime, cjFLOAT& enTime) const
{
    stTime = CJ_FLT_INFINITY;
    enTime = CJ_FLT_INVALID;

	// channel 1 : percent key
    cjUINT32 channel = ECHAN_PERCENT_WEIGHT;

    if(GetKeyCount(channel))
    {
        cjAniKey* beginKey = GetKey(0, channel);
        cjAniKey* endKey = GetKey(GetKeyCount(channel)-1, channel);

        if(beginKey && endKey)
        {
            if(beginKey->GetTime() < stTime)
                stTime = beginKey->GetTime();

            if(endKey->GetTime() > enTime)
                enTime = endKey->GetTime();
        }
    }

    if(stTime == CJ_FLT_INFINITY && enTime == CJ_FLT_INVALID)
    {
        stTime = 0.0f;
        enTime = 0.0f;
    }
}

void cjPathEvaluator::CacheKeys(cjFLOAT stTime, cjFLOAT enTime)
{
    if(m_spPCTDat)
        m_spPCTDat->CacheKeys(stTime, enTime);
}

cjAniEvaluator* cjPathEvaluator::GetEvaluator(cjFLOAT stTime, cjFLOAT enTime)
{
    cjPathEvaluator* eval = (cjPathEvaluator*)cjKeyBasedEvaluator::GetEvaluator(stTime, enTime);

    if(m_spPCTDat)
    {
		cjAniFloatDatRPtr spNewPCTDat= m_spPCTDat->GetDat(stTime, enTime);
        eval->SetPCTDat(spNewPCTDat);
    }

    return eval;
}

cjBool cjPathEvaluator::GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, 
	cjUINT32& fillSiz, cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const
{
    cjAssert(channel < cjAniEvalChan_PRS_Max);

    cjBool isPosed = IsChannelItemPosed(channel);

	if(IsChannelItemInvalid(channel) || (isPosed && !forceAlwaysUpdate && !IsAlwaysUpdate()))
        return cjFalse;

    fillSiz = sizeof(cjAniSPad::PathFill);

    if(m_pathLengths)
        fillSiz += m_spPathDat->GetKeyCount() * sizeof(cjFLOAT); // adjust fill siz

    isSharedFillDat = cjTrue;

    if(channel == cjAniEvalChan_Pos)
    {
        spadBlk = cjAniSPADB_CubicVec3PSeg;
    }
    else
    {
        cjAssert(channel == cjAniEvalChan_Rot);
        spadBlk = cjAniSPADB_SlerpRotSeg;
    }

    bsBasisDat = 0;

    return cjTrue;
}

cjBool cjPathEvaluator::InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, 
	cjBool initSharedDat, cjRefObjNameHash* nodeHash, cjAniPoseBufHandle pbHandle)  const
{
    cjAssert(channel == cjAniEvalChan_Pos || channel == cjAniEvalChan_Rot);
    cjAssert(evalSPDat->GetEvaluator() == this);
    cjAssert((cjUINT32)evalSPDat->GetEvalChannel() == channel);
    cjAssert(!IsChannelItemInvalid(channel));
    cjAssert(m_spPathDat && m_spPathDat->GetKeyCount() > 0);
    cjAssert(m_spPCTDat && m_spPCTDat->GetKeyCount() > 0);

    cjAniSPad::SegHdr* segHdr = (cjAniSPad::SegHdr*)evalSPDat->GetSegDat();
    cjAssert(segHdr);

    segHdr->m_lod = evalSPDat->GetLOD();

    if(IsReferenced())
    {
        cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Ref);
        segHdr->m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;
    }
    else
    {
        cjAssert(pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Vec3P || 
			pbHandle.GetChannelItem() == cjAniPoseBufChanItem_Rot);
        segHdr->m_outputPBIndex = pbHandle.GetAuxIndex();
    }

    cjAniSPad::CubicVec3Seg* posSeg = cjNULL;
    cjAniSPad::SlerpRotSeg* rotSeg = cjNULL;

    if(channel == cjAniEvalChan_Pos)
    {
        posSeg = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();
    }
    else
    {
        rotSeg = (cjAniSPad::SlerpRotSeg*)evalSPDat->GetSegDat();
    }

    if(initSharedDat)
    {
        cjAniSPad::PathFill* fillDat = (cjAniSPad::PathFill*)evalSPDat->GetFillDat();
        cjAssert(fillDat);

        fillDat->m_lastTime = CJ_FLT_INVALID;
        fillDat->m_pctMinTime = CJ_FLT_INFINITY;
        fillDat->m_pctMaxTime = CJ_FLT_INVALID;

        fillDat->m_pctFill.m_lastIndex = 0;
        fillDat->m_pctFill.m_keys = m_spPCTDat->GetKeys(fillDat->m_pctFill.m_keyCount, 
			fillDat->m_pctFill.m_keyType, fillDat->m_pctFill.m_keySiz);

        fillDat->m_pctSeg.m_hdr.m_lod = evalSPDat->GetLOD();
        fillDat->m_pctSeg.m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;

        fillDat->m_posMinPct = CJ_FLT_INFINITY;
        fillDat->m_posMaxPct = CJ_FLT_INVALID;

        fillDat->m_posFill.m_lastIndex = 0;
        fillDat->m_posFill.m_keys = m_spPathDat->GetKeys(fillDat->m_posFill.m_keyCount, 
            fillDat->m_posFill.m_keyType, fillDat->m_posFill.m_keySiz);

        fillDat->m_posSeg.m_hdr.m_lod = evalSPDat->GetLOD();
        fillDat->m_posSeg.m_hdr.m_outputPBIndex = cjAniSPad::EINVALID_OUTPUT_INDEX;

        fillDat->m_flag = (cjWORD)CJFLAG_GET;
        fillDat->m_followAxis = m_followAxis;
        fillDat->m_maxBankAngle = m_maxBankAngle;
        fillDat->m_smoothing = m_smooth;
        fillDat->m_maxCurvature = m_maxCurvature;
        fillDat->m_bankDir = m_bankDir;
        fillDat->m_totalPathLength = m_totalPathLength;

        if(IsConstSpeed())
        {
            fillDat->m_pathLengths = (cjFLOAT*)((cjBYTE*)fillDat + sizeof(cjAniSPad::PathFill));
            cjUINT32 uiNumLengths = fillDat->m_posFill.m_keyCount;

            for(cjUINT32 i=0; i<uiNumLengths; i++)
                fillDat->m_pathLengths[i] = m_pathLengths[i];
        }
        else
        {
            fillDat->m_pathLengths = cjNULL;
        }
    }

    cjAssert(evalSPDat->GetSegMinTime() == CJ_FLT_INFINITY);
    cjAssert(evalSPDat->GetSegMaxTime() == CJ_FLT_INVALID);

    evalSPDat->SetFillFunc(&PathFillFunc);

    return cjTrue;
}

cjUINT32 cjPathEvaluator::GetKeyCount(cjUINT32 channel) const
{
    if(channel == ECHAN_PATH)
    {
        if(!m_spPathDat)
            return 0;
        else
            return m_spPathDat->GetKeyCount();
    }
    else if(channel == ECHAN_PERCENT_WEIGHT)
    {
        if(!m_spPCTDat)
            return 0;
        else
            return m_spPCTDat->GetKeyCount();
    }
    else
    {
        cjAssert(channel < 2);
        return 0;
    }
}

cjAniKey::EKEY_TYPE cjPathEvaluator::GetKeyType(cjUINT32 channel) const
{
    if(channel == ECHAN_PATH)
    {
        if(!m_spPathDat)
            return cjAniKey::EKT_NOINTERP;

        cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE keySiz;
        m_spPathDat->GetKeys(keyCount, keyType, keySiz);

        return keyType; 
    }
    else if(channel == ECHAN_PERCENT_WEIGHT)
    {
        if(!m_spPCTDat)
            return cjAniKey::EKT_NOINTERP;

        cjUINT32 keyCount; cjAniKeyFloat::EKEY_TYPE keyType; cjBYTE keySiz;
        m_spPCTDat->GetKeys(keyCount, keyType, keySiz);

        return keyType;     
    }

	cjAssert(channel < 2);

    return cjAniKey::EKT_NOINTERP;
}

cjAniKey::EKEY_VALTYPE cjPathEvaluator::GetKeyValType(cjUINT32 channel) const
{
    if(channel == ECHAN_PATH)
        return cjAniKey::EKVAL_VEC3;
    else if(channel == ECHAN_PERCENT_WEIGHT)
        return cjAniKey::EKVAL_FLOAT;
    
    cjAssert(channel < 2);

    return cjAniKey::EKVAL_MAX;
}

cjAniKey* cjPathEvaluator::GetKeyAry(cjUINT32 channel) const
{
    if(channel == ECHAN_PATH)
    {       
        if(!m_spPathDat)
            return cjNULL;

        cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE keySiz;
        cjAniKeyVec3* keys = m_spPathDat->GetKeys(keyCount, keyType, keySiz);

        return keys; 
    }
    else if(channel == ECHAN_PERCENT_WEIGHT)
    {
        if(!m_spPCTDat)
            return cjNULL;
    
        cjUINT32 keyCount; cjAniKeyFloat::EKEY_TYPE keyType; cjBYTE keySiz;
        cjAniKeyFloat * keys = m_spPCTDat->GetKeys(keyCount, keyType, keySiz);

        return keys; 
    }

	cjAssert(channel < 2);

	return cjNULL;
}

cjBYTE cjPathEvaluator::GetKeySiz(cjUINT32 channel) const
{
    if(channel == ECHAN_PATH)
    {       
        if(!m_spPathDat)
            return 0;

        cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE keySiz;
        m_spPathDat->GetKeys(keyCount, keyType, keySiz);

        return keySiz; 
    }
    else if(channel == ECHAN_PERCENT_WEIGHT)
    {
        if(!m_spPCTDat)
            return 0;
    
        cjUINT32 keyCount; cjAniKeyFloat::EKEY_TYPE keyType; cjBYTE keySiz;
        m_spPCTDat->GetKeys(keyCount, keyType, keySiz);

        return keySiz; 
    }

	cjAssert(channel < 2);

	return 0;
}

cjAniKeyFloat* cjPathEvaluator::GetPCTKeys(cjUINT32& keyCount, cjAniKeyFloat::EKEY_TYPE& keyType, 
	cjBYTE& keySiz) const
{
    cjAniKeyFloat* keys = cjNULL;

    if(m_spPCTDat)
    {
        keys = m_spPCTDat->GetKeys(keyCount, keyType, keySiz);
    }
    else
    {
        keyCount = 0; 
		keyType = cjAniKey::EKT_NOINTERP; 
		keySiz = 0;
    }

    return keys;
}

void cjPathEvaluator::ReplacePCTKeys(cjAniKeyFloat* keys, cjUINT32 keyCount, cjAniKeyFloat::EKEY_TYPE keyType)
{
    if(!m_spPCTDat)
        m_spPCTDat = new cjAniFloatDat;

    m_spPCTDat->ReplaceKeys(keys, keyCount, keyType);

    SetEvalChannelTypes();
}

cjAniKeyVec3* cjPathEvaluator::GetPathKeys(cjUINT32& keyCount, cjAniKeyVec3::EKEY_TYPE& keyType, cjBYTE& keySiz) const
{
    cjAniKeyVec3* keys = cjNULL;

    if(m_spPathDat)
    {
        keys = m_spPathDat->GetKeys(keyCount, keyType, keySiz);
    }
    else
    {
        keyCount = 0; 
		keyType = cjAniKey::EKT_NOINTERP; 
		keySiz = 0;
    }

    return keys;
}

void cjPathEvaluator::ReplacePathKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType)
{
    if(!m_spPathDat)
        m_spPathDat = new cjAniVec3Dat;

    m_spPathDat->ReplaceKeys(keys, keyCount, keyType);

    cjBYTE keySiz = cjAniKeyVec3::GetKeySiz(keyType);

    SetOpenCurve(keys->GetKey(0, keySiz)->GetValue() != keys->GetKey(keyCount-1, keySiz)->GetValue());

	SetConstSpeedDatDirty(cjTrue);
	
	if(IsConstSpeed())
        SetConstSpeedDat();

    cjAniKeyVec3::PFCurvedRatio pfCurvature = cjAniKeyVec3::GetCurvedRatioKeyPF(keyType);
	m_maxCurvature = pfCurvature((cjAniKey*)keys, keyCount);

    SetEvalChannelTypes();
}

cjFLOAT cjPathEvaluator::GetTotalPathLen()
{
    if(m_totalPathLength < 0.0f)
    {
        cjUINT32 pathKeyCount; cjAniKeyVec3::EKEY_TYPE pathKeyType; cjBYTE pathKeySiz;
        GetPathKeys(pathKeyCount, pathKeyType, pathKeySiz);		       
		cjAssert(pathKeyCount >= 1);

		cjMemDeAlloc(m_pathLengths);

        m_pathLengths = (cjFLOAT*)cjMemAlloc(sizeof(cjFLOAT) * pathKeyCount);
        m_pathLengths[0] = 0.0f;

        for(cjUINT32 k0=0, k1=1; k0<pathKeyCount-1; k0++, k1++)
        {
            cjFLOAT len = GetPathLength(k0, k1, 1.0f);
            m_pathLengths[k1] = m_pathLengths[k0] + len;
        }

        m_totalPathLength = m_pathLengths[pathKeyCount-1];
    }

    return m_totalPathLength;
}

cjFLOAT cjPathEvaluator::GetPathSpeed(cjINT32 keyIdx0, cjINT32 keyIdx1, cjFLOAT rtime) const
{
    cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType;	cjBYTE keySiz;
    cjAniKeyVec3* keys = GetPathKeys(keyCount, keyType, keySiz);

    cjAniKeyVec3::PFInterpolation pfInterpolationD1 = cjAniKeyVec3::GetInterpD1KeyPF(keyType);

    cjVec3 speed;
    pfInterpolationD1(rtime, keys->GetKey(keyIdx0, keySiz), keys->GetKey(keyIdx1, keySiz), &speed);

    return speed.Length();
}

cjFLOAT cjPathEvaluator::GetPathLength(cjINT32 keyIdx0, cjINT32 keyIdx1, cjFLOAT rtime) const
{
    static const cjINT32 degree = 5;
    static const cjFLOAT modRoot[degree]  = { 0.046910077f, 0.230765345f, 0.5f, 0.769234655f, 0.953089922f };
    static const cjFLOAT modCoeff[degree] = { 0.118463442f, 0.239314335f, 0.284444444f, 0.239314335f, 0.118463442f };

    cjFLOAT rst = 0.0f;
    for(cjINT32 i=0; i<degree; i++)
        rst += modCoeff[i] * GetPathSpeed(keyIdx0,keyIdx1, rtime * modRoot[i]);

    rst *= rtime;

    return rst;
}

cjFLOAT cjPathEvaluator::GetPathLength(cjFLOAT ratio, const cjAniKeyVec3* posKey1, const cjAniKeyVec3* posKey2, 
	cjAniKeyVec3::PFInterpolation interpD1)
{
    static const cjINT32 degree = 5;
    static const cjFLOAT modRoot[degree]  = { 0.046910077f, 0.230765345f, 0.5f, 0.769234655f, 0.953089922f };
    static const cjFLOAT modCoeff[degree] = { 0.118463442f, 0.239314335f, 0.284444444f, 0.239314335f, 0.118463442f };
	
	cjFLOAT rst = 0.0f;
    for(cjINT32 i=0; i<degree; i++)
    {
        cjVec3 d;
        interpD1(ratio * modRoot[i], posKey1, posKey2, &d);
        rst += modCoeff[i] * d.Length();
    }

	rst *= ratio;

	return rst;
}

void cjPathEvaluator::SetConstSpeedDat()
{
    if(!IsConstSpeedDatDirty())
        return;

    m_totalPathLength = GetTotalPathLen();

    SetConstSpeedDatDirty(cjFalse);
}

cjBool cjPathEvaluator::PathFillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat)
{
    cjAssert(evalSPDat);

    cjAniSPad::PathFill* pathFill = (cjAniSPad::PathFill*)evalSPDat->GetFillDat();
    cjAssert(pathFill);

    if(etime != pathFill->m_lastTime)
    {
        cjAniSPad::CubicFloatSeg* pctSeg = &pathFill->m_pctSeg;

        if(etime > pathFill->m_pctMaxTime || etime < pathFill->m_pctMinTime)
        {
            cjAniSPad::CubicFloatFill* pctFill = &pathFill->m_pctFill;
            cjAssert(pctFill->m_keys);
            cjAssert(pctFill->m_keyCount > 0);

            cjAniKeyFloat::GenCubicCoefs(etime, pctFill->m_keys, pctFill->m_keyType, pctFill->m_keyCount, 
                pctFill->m_lastIndex, pctFill->m_keySiz, pathFill->m_pctMinTime, pathFill->m_pctMaxTime, 
                pctSeg->m_value0, pctSeg->m_outTangent0, pctSeg->m_a0, pctSeg->m_b0);

			pctSeg->m_stTime = pathFill->m_pctMinTime;
			pctSeg->m_invDTime = (pathFill->m_pctMaxTime != pathFill->m_pctMinTime) 
				? 1.0f / (pathFill->m_pctMaxTime - pathFill->m_pctMinTime) : 0.0f;
        }

        cjFLOAT ratio = (etime - pctSeg->m_stTime) * pctSeg->m_invDTime;
        cjFLOAT pct = pctSeg->m_value0 + (pctSeg->m_outTangent0 + (pctSeg->m_a0 + pctSeg->m_b0 * ratio) * ratio) * ratio;

        cjAniSPad::CubicVec3Fill* posFill = &pathFill->m_posFill;
        cjAssert(posFill->m_keys);
        cjAssert(posFill->m_keyCount > 0);

        if(CJFLAG_EXTERNAL_GETMASKED_VAL(pathFill->m_flag, MASK_CONST_SPEED) != 0)
        {
            if(pct < 0.0f)
                pct = cjFMod(pct, 1.0f) + 1.0f;
            else if(pct > 1.0f)
                pct = cjFMod(pct, 1.0f);

            cjFLOAT newPCT = pct;

            if(pct > 0.0f && pct < 1.0f)
            {
                cjFLOAT length = pct * pathFill->m_totalPathLength;
                cjFLOAT dist = 0.0f;
                cjFLOAT pathRatio = 0.0f;
                cjFLOAT* pfPartialLength = pathFill->m_pathLengths;
                cjUINT32 posKeyCountM1 = posFill->m_keyCount - 1;
                cjUINT32 i0, i1;

                for(i0=0, i1=1; i0<posKeyCountM1; i0++, i1++)
                {
                    if(length <= pfPartialLength[i1])
                    {
                        dist = length - pfPartialLength[i0];
                        pathRatio = dist / (pfPartialLength[i1] - pfPartialLength[i0]);
                        break;
                    }
                }

                const cjAniKeyVec3* posKey1 = posFill->m_keys->GetKey(i0, posFill->m_keySiz);
                const cjAniKeyVec3* posKey2 = posFill->m_keys->GetKey(i1, posFill->m_keySiz);
                cjAniKeyVec3::PFInterpolation interpD1 = cjAniKeyVec3::GetInterpD1KeyPF(posFill->m_keyType);

                const cjINT32 maxIterations = 32;
                for(cjINT32 i=0; i<maxIterations; i++)
                {
                    cjFLOAT diff = GetPathLength(pathRatio, posKey1, posKey2, interpD1) - dist;
                    if(cjAbs(diff) <= 1e-04f)
                        break;

                    cjVec3 d;
                    interpD1(pathRatio, posKey1, posKey2, &d);
                    pathRatio -= diff / d.Length();
                }

                newPCT = posKey1->GetTime() + pathRatio * (posKey2->GetTime() - posKey1->GetTime());
            }

            pct = newPCT;
        }

        if(pct < 0.0f)
            pct = cjFMod(pct, 1.0f) + 1.0f;
        else if(pct > 1.0f)
            pct = cjFMod(pct, 1.0f);

        cjAniSPad::CubicVec3Seg* posSeg = &pathFill->m_posSeg;

        if(pct > pathFill->m_posMaxPct || pct < pathFill->m_posMinPct)
        {
            cjAniKeyVec3::GenCubicCoefs(pct, posFill->m_keys, posFill->m_keyType, posFill->m_keyCount, 
                posFill->m_lastIndex, posFill->m_keySiz, pathFill->m_posMinPct, pathFill->m_posMaxPct, 
                posSeg->m_val0, posSeg->m_outTangent0, posSeg->m_a0, posSeg->m_b0);

			posSeg->m_stTime = pathFill->m_posMinPct;
            posSeg->m_invDTime = (pathFill->m_posMaxPct != pathFill->m_posMinPct) 
				? 1.0f / (pathFill->m_posMaxPct - pathFill->m_posMinPct) : 0.0f;
        }

        cjFLOAT r = (pct - posSeg->m_stTime) * posSeg->m_invDTime;
        pathFill->m_pos = posSeg->m_val0 + (posSeg->m_outTangent0 + (posSeg->m_a0 + posSeg->m_b0 * r) * r) * r;

        if(CJFLAG_EXTERNAL_GETMASKED_VAL(pathFill->m_flag, MASK_FOLLOW))
        {
            cjMatrix3 matPathRot;
            GetPathRotation(pct, r, (const void*)pathFill, matPathRot);
            pathFill->m_rot.MakeFromRotation(matPathRot);
        }

        pathFill->m_lastTime = etime;
    }

    if(evalSPDat->GetEvalChannel() == cjAniEvalChan_Pos)
    {
        cjAniSPad::CubicVec3Seg* cubicPosSeg = (cjAniSPad::CubicVec3Seg*)evalSPDat->GetSegDat();

        cubicPosSeg->m_val0 = pathFill->m_pos;
        evalSPDat->SetSegTimeRange(etime, etime);

        return cjTrue;
    }

    if(evalSPDat->GetEvalChannel() == cjAniEvalChan_Rot)
    {
        cjAniSPad::SlerpRotSeg* slerpRot = (cjAniSPad::SlerpRotSeg*)evalSPDat->GetSegDat();

        slerpRot->m_quat0 = pathFill->m_rot;
        slerpRot->m_quat1 = pathFill->m_rot;
        evalSPDat->SetSegTimeRange(etime, etime);

        return cjTrue;
    }

    return cjFalse;
}

void cjPathEvaluator::GetPathRotation(cjFLOAT pct, cjFLOAT ratio, const void* _pathFill, 
	cjMatrix3& matPathRot)
{    
	const cjAniSPad::PathFill* pathFill = (const cjAniSPad::PathFill*)_pathFill;
	cjMatrix3 tmpMat = cjMatrix3::Identity;

    const cjAniSPad::CubicVec3Fill* posFill = &pathFill->m_posFill;

    if(posFill->m_keyCount < 2)
    {
        matPathRot = tmpMat;
        return;
    }

    cjAniKeyVec3* posKeys = posFill->m_keys;
	cjAssert(posFill->m_keys);

    cjUINT32 posKeyCountM1 = posFill->m_keyCount - 1;
    cjAniKeyVec3::EKEY_TYPE posKeyType = posFill->m_keyType;
    cjBYTE posKeySiz = posFill->m_keySiz;
	    
    cjUINT32 lastIndex = posFill->m_lastIndex;
    const cjAniKeyVec3* posKey1 = posKeys->GetKey(lastIndex, posKeySiz);
    cjAssert(pct >= posKey1->GetTime());

    cjUINT32 lastIndexP1 = lastIndex + 1;
    cjAssert(lastIndexP1 <= posKeyCountM1);
    const cjAniKeyVec3* posKey2 = posKeys->GetKey(lastIndexP1, posKeySiz);
    cjAssert(pct <= posKey2->GetTime());

    cjVec3 tang, nrm, bnrm;
    cjFLOAT curvature;

    cjAniKeyVec3::CoordinateFrame(ratio, posKey1, posKey2, posKeyType, tang, nrm, bnrm, curvature);
    cjBool isOpenCurve = CJFLAG_EXTERNAL_GETMASKED_VAL(pathFill->m_flag, MASK_OPEN_CURVE) != 0 ? cjTrue : cjFalse;

    if(CJFLAG_EXTERNAL_GETMASKED_VAL(pathFill->m_flag, MASK_ALLOWFLIP))
    {
        cjVec3 tmpTan, nrm1;
        cjFLOAT a0, a1, curve1;

        if(pathFill->m_smoothing)
        {
            if(ratio >= 1.0f - pathFill->m_smoothing)
            {
                cjUINT32 keyIdx = (!isOpenCurve && lastIndexP1 == posKeyCountM1 ? 0 : lastIndexP1);

                if(keyIdx < posKeyCountM1)
                {
                    a0 = (1.0f - ratio + pathFill->m_smoothing) / (2.0f * pathFill->m_smoothing);
                    a1 = 1.0f - a0;

                    cjAniKeyVec3::CoordinateFrame(1.0f - pathFill->m_smoothing, posKey1, posKey2, posKeyType, tmpTan, nrm, bnrm, curvature);
                    cjAniKeyVec3::CoordinateFrame(pathFill->m_smoothing, posKeys->GetKey(keyIdx, posKeySiz), posKeys->GetKey(keyIdx + 1, posKeySiz), posKeyType, tmpTan, nrm1, bnrm, curve1);

                    nrm = a0 * nrm + a1 * nrm1;
                    nrm.Normalize();

                    curvature = a0 * curvature + a1 * curve1;
                    bnrm = tang.Cross(nrm);
                }
            }
            else if(ratio <= pathFill->m_smoothing)
            {
                cjUINT32 keyIdx = (!isOpenCurve && lastIndex == 0 ? posKeyCountM1 : lastIndex);

                if(keyIdx >= 1)
                {
                    a1 = (pathFill->m_smoothing - ratio) / (2.0f * pathFill->m_smoothing);
                    a0 = 1.0f - a1;

                    cjAniKeyVec3::CoordinateFrame(pathFill->m_smoothing, posKey1, posKey2, posKeyType, tmpTan, nrm, bnrm, curvature);
                    cjAniKeyVec3::CoordinateFrame(1.0f - pathFill->m_smoothing, posKeys->GetKey(keyIdx - 1, posKeySiz), posKeys->GetKey(keyIdx, posKeySiz), posKeyType, tmpTan, nrm1, bnrm, curve1);

                    nrm = a0 * nrm + a1 * nrm1;
                    nrm.Normalize();

                    curvature = a0 * curvature + a1 * curve1;
                    bnrm = tang.Cross(nrm);
                }
            }
        }
                
        tmpMat.SetCol(0, tang); // direction
        tmpMat.SetCol(1, -nrm); // up        
        tmpMat.SetCol(2, -bnrm); // right
    }
    else
    {        
        tmpMat.SetCol(0, tang); // direction        

        cjVec3 up = cjVec3::UNIT_Z.UnitCross(tang);
        tmpMat.SetCol(1, up); // up
		        
        cjVec3 right = tang.Cross(up); // right
        tmpMat.SetCol(2, right);
    }

    if(CJFLAG_EXTERNAL_GETMASKED_VAL(pathFill->m_flag, MASK_FLIP))
    {
        cjVec3 v;
        
        tmpMat.GetCol(0, v);  // look-at
        tmpMat.SetCol(0, -v);

        tmpMat.GetCol(1, v);  // up
        tmpMat.SetCol(1, -v);
    }

    cjVec3 t1, t2;

    switch(pathFill->m_followAxis)
    {
        case 1:
            tmpMat.GetCol(0, t1);		tmpMat.GetCol(1, t2);
            tmpMat.SetCol(0, -t2);		tmpMat.SetCol(1, t1);
            break;

        case 2:
            tmpMat.GetCol(0, t1);		tmpMat.GetCol(2, t2);
            tmpMat.SetCol(0, -t2);		tmpMat.SetCol(2, t1);
            break;
    }

    if(CJFLAG_EXTERNAL_GETMASKED_VAL(pathFill->m_flag, MASK_BANKING))
    {
        static cjFLOAT s_fourOverPI = 4.0f / CJ_PI;

        cjFLOAT bankingAmount = cjFLOAT(pathFill->m_bankDir) * pathFill->m_maxBankAngle;

        if(curvature < pathFill->m_maxCurvature)
            bankingAmount *= s_fourOverPI * cjATan(curvature / pathFill->m_maxCurvature);

        cjMatrix3 matBankingRot;

        matBankingRot.MakeRotationFromAxis(bankingAmount, tang);
        tmpMat = matBankingRot * tmpMat;
    }

    matPathRot = tmpMat;
}

void cjPathEvaluator::SetEvalChannelTypes()
{
    m_channelItem[cjAniEvalChan_Pos] = cjAniEvalChanItem_NoData;
    m_channelItem[cjAniEvalChan_Rot] = cjAniEvalChanItem_NoData;
    m_channelItem[cjAniEvalChan_Scl] = cjAniEvalChanItem_NoData;

	// assume channel is not posed
    if(m_spPathDat && m_spPathDat->GetKeyCount() && m_spPCTDat && m_spPCTDat->GetKeyCount())
    {
        m_channelItem[cjAniEvalChan_Pos] = cjAniEvalChanItem_Vec3P;

        if(IsFollow())
            m_channelItem[cjAniEvalChan_Rot] = cjAniEvalChanItem_Rot;
    }
}


//
// cjEvaluatorConv
//

cjAniEvaluator* cjEvaluatorConv::Convert(cjInterpolator* interpolator, const cjAniEvaluatorTag& tag, cjBool doCollapse)
{
    cjAssert(interpolator);

    if(doCollapse)
        interpolator->Collapse();

    cjAniEvaluator* evaluator = cjNULL;

    if(interpolator->IsTMValue())
    {
        evaluator = DoTMInterpolator(interpolator, tag.GetID(), doCollapse);
    }
    else
    {
        if(interpolator->IsQuatValue())
            evaluator = DoQuatInterpolator(interpolator);
        else if(interpolator->IsVec3Value())
            evaluator = DoVec3Interpolator(interpolator);
        else if(interpolator->IsFloatValue())
            evaluator = DoFloatInterpolator(interpolator);
        else if(interpolator->IsBoolValue())
            evaluator = DoBoolInterpolator(interpolator);
        else
            evaluator = DoColorInterpolator(interpolator);
    }

    if(!evaluator)		
		return cjNULL;

    cjName cntrRttiName = tag.GetCntrRttiName();

    if(cntrRttiName.IsValid())
    {   
        if(cntrRttiName == cjAniConstants::GetFlipAniCntrName())
            evaluator->SetChannelFlagReferenced();
    }

    evaluator->SetTagInfo(tag.GetID(), tag.GetPropRttiName(), cntrRttiName, tag.GetCntrID(), tag.GetEvaluatorID());

    return evaluator;
}

cjAniEvaluator* cjEvaluatorConv::DoColorInterpolator(cjInterpolator* interpolator)
{
    cjBSColorInterpolator* bsColorInterp = cjrCast<cjBSColorInterpolator>(interpolator);

    if(bsColorInterp)
    {
        cjFLOAT stTime, enTime;

		if(bsColorInterp->GetChannelPosed(0))
        {
            cjColorF poseValue;
            interpolator->Update(0, cjNULL, poseValue);

            return new cjConstColorEvaluator(poseValue);
        }

        cjBSColorEvaluator* bsColorEval = new cjBSColorEvaluator();

		bsColorInterp->GetValidTimeRange(stTime, enTime);

        bsColorEval->SetDat(bsColorInterp->GetDat(), bsColorInterp->GetBasisDat());
        bsColorEval->SetTimeRange(stTime, enTime);
        bsColorEval->SetCPOffset(bsColorInterp->GetCPOffset(0), 0);

        return bsColorEval;
    }

    cjColorInterpolator* colorInterp = cjrCast<cjColorInterpolator>(interpolator);

    if(colorInterp)
    {
        if(colorInterp->GetChannelPosed(0))
        {
            cjColorF poseValue;
            interpolator->Update(0, cjNULL, poseValue);

            return new cjConstColorEvaluator(poseValue);
        }

        cjAniColorDat* dat = colorInterp->GetColorDat();

        if(dat)
            return new cjColorEvaluator(dat);
    }

	cjMLOG("ERR! - unsupported interpolator...");
	cjAssert(!"ERR!");

	return cjNULL;
}

cjAniEvaluator* cjEvaluatorConv::DoBoolInterpolator(cjInterpolator* interpolator)
{
    cjAssert(interpolator->IsBoolValue());

    cjBoolTimelineInterpolator* boolTimeLineInterp = cjrCast<cjBoolTimelineInterpolator>(interpolator);

    if(boolTimeLineInterp)
    {
        if(boolTimeLineInterp->GetChannelPosed(0))
        {
            cjBool cjBoolValue;
            interpolator->Update(0, cjNULL, cjBoolValue);

            cjFLOAT value = cjBoolValue ? 1.0f : 0.0f;

            return new cjConstBoolEvaluator(value);
        }

        cjAniBoolDat* dat = boolTimeLineInterp->GetBoolDat();

		if(dat)
            return new cjBoolTimelineEvaluator(dat);
    }

    cjBoolInterpolator* boolInterp = cjrCast<cjBoolInterpolator>(interpolator);
    if(boolInterp)
    {
        if(boolInterp->GetChannelPosed(0))
        {
            cjBool cjBoolValue;
            interpolator->Update(0, cjNULL, cjBoolValue);
            cjFLOAT value = cjBoolValue ? 1.0f : 0.0f;

            return new cjConstBoolEvaluator(value);
        }

        cjAniBoolDat* dat = boolInterp->GetBoolDat();

        if(dat)
            return new cjBoolEvaluator(dat);
    }

	cjMLOG("ERR! - failed converting cjBool Interpolator...");
	cjAssert(!"ERR!");

    return cjNULL;
}

cjAniEvaluator* cjEvaluatorConv::DoFloatInterpolator(cjInterpolator* interpolator)
{
    cjAssert(interpolator->IsFloatValue());

    cjBSFloatInterpolator* bsFloatInterp = cjrCast<cjBSFloatInterpolator>(interpolator);

    if(bsFloatInterp)
    {
		if(bsFloatInterp->GetChannelPosed(0))
        {
            cjFLOAT value;
            interpolator->Update(0, cjNULL, value);

            return new cjConstFloatEvaluator(value);
        }

        cjUINT32 cpCount = bsFloatInterp->GetCPCount(0);
        if(cpCount > 0)
        {
			cjFLOAT stTime, enTime;
            cjBSFloatEvaluator* bsFloatEvaluator = new cjBSFloatEvaluator(bsFloatInterp->GetDat(), 
				bsFloatInterp->GetCPOffset(0), bsFloatInterp->GetBasisDat());

            bsFloatInterp->GetValidTimeRange(stTime, enTime);
            bsFloatEvaluator->SetTimeRange(stTime, enTime);

            return bsFloatEvaluator;
        }
    }

    cjFloatInterpolator* floatInterp = cjrCast<cjFloatInterpolator>(interpolator);

    if(floatInterp)
    {
        if(floatInterp->GetChannelPosed(0))
        {
            cjFLOAT value;
            interpolator->Update(0, cjNULL, value);

            return new cjConstFloatEvaluator(value);
        }

        cjAniFloatDat* dat = floatInterp->GetFloatDat();

        if(dat)
            return new cjFloatEvaluator(dat);
    }

	cjMLOG("ERR! - failed converting cjFLOAT Interpolator...");
	cjAssert(!"ERR!");

    return cjNULL;
}

cjAniEvaluator* cjEvaluatorConv::DoVec3Interpolator(cjInterpolator* interpolator)
{
    cjAssert(interpolator->IsVec3Value());

	// vec3s (scale)
	{
		cjBSVec3SInterpolator* bsVec3SInterp = cjrCast<cjBSVec3SInterpolator>(interpolator);

		if(bsVec3SInterp)
		{
			cjFLOAT stTime, enTime;

			if(bsVec3SInterp->GetChannelPosed(0))
			{
				cjVec3 poseValue;
				interpolator->Update(0, cjNULL, poseValue);

				return new cjConstVec3SEvaluator(poseValue);
			}

			bsVec3SInterp->GetValidTimeRange(stTime, enTime);

			cjBSVec3SEvaluator* bsVec3SEvaluator = new cjBSVec3SEvaluator();
			bsVec3SEvaluator->SetDat(bsVec3SInterp->GetDat(), bsVec3SInterp->GetBasisDat());
			bsVec3SEvaluator->SetTimeRange(stTime, enTime);
			bsVec3SEvaluator->SetCPOffset(bsVec3SInterp->GetCPOffset(0), 0);

			return bsVec3SEvaluator;
		}
		
		cjVec3SInterpolator* vec3SInterp = cjrCast<cjVec3SInterpolator>(interpolator);

		if(vec3SInterp)
		{
			if(vec3SInterp->GetChannelPosed(0))
			{
				cjVec3 poseValue;
				interpolator->Update(0, cjNULL, poseValue);

				return new cjConstVec3SEvaluator(poseValue);
			}

			cjAniVec3Dat* dat = vec3SInterp->GetVec3Dat();

			if(dat)
				return new cjVec3SEvaluator(dat);
		}
	}

	// vec3P (pos)
	{
	   cjBSVec3PInterpolator* bsVec3PInterp = cjrCast<cjBSVec3PInterpolator>(interpolator);

		if(bsVec3PInterp)
		{
			cjFLOAT stTime, enTime;

			if(bsVec3PInterp->GetChannelPosed(0))
			{
				cjVec3 poseValue;
				interpolator->Update(0, cjNULL, poseValue);

				return new cjConstVec3PEvaluator(poseValue);
			}

			bsVec3PInterp->GetValidTimeRange(stTime, enTime);

			cjBSVec3PEvaluator* bsVec3PEvaluator = new cjBSVec3PEvaluator();
			bsVec3PEvaluator->SetDat(bsVec3PInterp->GetDat(), bsVec3PInterp->GetBasisDat());
			bsVec3PEvaluator->SetTimeRange(stTime, enTime);
			bsVec3PEvaluator->SetCPOffset(bsVec3PInterp->GetCPOffset(0), 0);

			return bsVec3PEvaluator;
		}
		
		cjVec3PInterpolator* vec3PInterp = cjrCast<cjVec3PInterpolator>(interpolator);

		if(vec3PInterp)
		{
			if(vec3PInterp->GetChannelPosed(0))
			{
				cjVec3 poseValue;
				interpolator->Update(0, cjNULL, poseValue);

				return new cjConstVec3PEvaluator(poseValue);
			}

			cjAniVec3Dat* dat = vec3PInterp->GetVec3Dat();

			if(dat)
				return new cjVec3PEvaluator(dat);
		}
	}
	
	cjMLOG("ERR! - failed converting vec3 Interpolator...");
	cjAssert(!"ERR!");

    return cjNULL;
}

cjAniEvaluator* cjEvaluatorConv::DoQuatInterpolator(cjInterpolator* interpolator)
{
    cjAssert(interpolator->IsQuatValue());

	cjQuatInterpolator* quatInterp = cjrCast<cjQuatInterpolator>(interpolator);
    if(quatInterp)
    {
        if(quatInterp->GetChannelPosed(0))
        {
            cjQuat poseValue;
            interpolator->Update(0, cjNULL, poseValue);

            return new cjConstQuatEvaluator(poseValue);
        }

        cjAniRotDat* dat = quatInterp->GetRotDat();

        if(dat)
            return new cjQuatEvaluator(dat);
    }

	cjMLOG("ERR! - failed converting quat Interpolator...");
	cjAssert(!"ERR!");

    return cjNULL;
}

cjAniEvaluator* cjEvaluatorConv::DoTMInterpolator(cjInterpolator* interpolator, const cjName& name, cjBool doCollapse)
{
    cjAssert(interpolator->IsTMValue());

    cjLookAtInterpolator* lookAtInterp = cjrCast<cjLookAtInterpolator>(interpolator);

    if(lookAtInterp)
        return DoLookAtInterpolator(lookAtInterp, name, doCollapse);

    cjPathInterpolator* pathInterpolator = cjrCast<cjPathInterpolator>(interpolator);

    if(pathInterpolator)
        return DoPathInterpolator(pathInterpolator);

    cjQuatTM poseValue;
    interpolator->Update(0, cjNULL, poseValue);

    cjBSTMInterpolator* bsTMInterp = cjrCast<cjBSTMInterpolator>(interpolator);

	if(bsTMInterp)
    {
        if(bsTMInterp->GetChannelPosed(0) && bsTMInterp->GetChannelPosed(1) && bsTMInterp->GetChannelPosed(2))
            return new cjConstTMEvaluator(poseValue);

        return DoBSTMInterpolator(bsTMInterp, poseValue);
    }

    cjTMInterpolator* tmInterp = cjrCast<cjTMInterpolator>(interpolator);

    if(tmInterp)
    {
        if(tmInterp->GetChannelPosed(0) && tmInterp->GetChannelPosed(1) && tmInterp->GetChannelPosed(2))
            return new cjConstTMEvaluator(poseValue);

        return DoTMInterpolator(tmInterp, poseValue);
    }

	cjMLOG("ERR! - failed converting lookat Interpolator...");
	cjAssert(!"ERR!");

    return cjNULL;
}

cjAniEvaluator* cjEvaluatorConv::DoBSTMInterpolator(cjBSTMInterpolator* bsTMInterp, const cjQuatTM& poseValue)
{
	cjFLOAT stTime, enTime;

    cjBSTMEvaluator* bsTMEvaluator = new cjBSTMEvaluator();
    bsTMEvaluator->SetDat(bsTMInterp->GetDat(), bsTMInterp->GetBasisDat());
    
    bsTMInterp->GetValidTimeRange(stTime, enTime);
    bsTMEvaluator->SetTimeRange(stTime, enTime);

    // Pos
    if(poseValue.IsPosValid())
    {
		if(bsTMInterp->GetChannelPosed(0))
            bsTMEvaluator->SetPosePos(poseValue.GetPos());
        else
            bsTMEvaluator->SetCPOffset(bsTMInterp->GetCPOffset(0), 0);
    }

    if(poseValue.IsRotateValid())
    {
		if(bsTMInterp->GetChannelPosed(1))
            bsTMEvaluator->SetPoseRotate(poseValue.GetRotate());
        else
            bsTMEvaluator->SetCPOffset(bsTMInterp->GetCPOffset(1), 1);
    }

    if(poseValue.IsScaleValid())
    {
        if(bsTMInterp->GetChannelPosed(2))
            bsTMEvaluator->SetPoseScale(poseValue.GetScale());
        else
            bsTMEvaluator->SetCPOffset(bsTMInterp->GetCPOffset(2), 2);
    }

    return bsTMEvaluator;
}

cjAniEvaluator* cjEvaluatorConv::DoTMInterpolator(cjTMInterpolator* tmInterpolator, const cjQuatTM& poseValue)
{
    cjTMEvaluator* tmEvaluator = new cjTMEvaluator();

    if(poseValue.IsPosValid())
    {
        if(tmInterpolator->GetChannelPosed(0))
        {
            tmEvaluator->SetPosePos(poseValue.GetPos());
        }
        else
        {
            cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE keySiz;
            cjAniKeyVec3* keys = tmInterpolator->GetPosKeys(keyCount, keyType, keySiz);

            if(keys)
            {
                cjAniKeyVec3::PFCreate pfCreate = cjAniKeyVec3::GetCreateKeyPF(keyType);
                cjAniKeyVec3* newKeys = (cjAniKeyVec3*)pfCreate(keyCount);
                cjAniKeyVec3::PFCopy copy = cjAniKeyVec3::GetCopyKeyPF(keyType);

                for(cjUINT32 k=0; k<keyCount; k++)
                    copy(newKeys->GetKey(k, keySiz), keys->GetKey(k, keySiz));

                tmEvaluator->ReplacePosKeys(newKeys, keyCount, keyType);
            }
        }
    }

    if(poseValue.IsRotateValid())
    {
        if(tmInterpolator->GetChannelPosed(1))
        {
            tmEvaluator->SetPoseRotate(poseValue.GetRotate());
        }
        else
        {
            cjUINT32 keyCount; cjAniKeyRot::EKEY_TYPE keyType; cjBYTE keySiz;
            cjAniKeyRot* keys = tmInterpolator->GetRotKeys(keyCount, keyType, keySiz);

            if(keys)
            {
                cjAniKeyRot::PFCreate pfCreate = cjAniKeyRot::GetCreateKeyPF(keyType);
                cjAniKeyRot* newKeys = (cjAniKeyRot*)pfCreate(keyCount);
                cjAniKeyRot::PFCopy copy = cjAniKeyRot::GetCopyKeyPF(keyType);

                for(cjUINT32 k=0; k<keyCount; k++)
                    copy(newKeys->GetKey(k, keySiz), keys->GetKey(k, keySiz));

                tmEvaluator->ReplaceRotKeys(newKeys, keyCount, keyType);
            }
        }
    }
	
    // Scale
    if(poseValue.IsScaleValid())
    {
        if(tmInterpolator->GetChannelPosed(2))
        {
            tmEvaluator->SetPoseScale(poseValue.GetScale());
        }
        else
        {
            cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE keySiz;
            cjAniKeyVec3* keys = tmInterpolator->GetScaleKeys(keyCount, keyType, keySiz);

            if(keys)
            {
                cjAniKeyVec3::PFCreate pfCreate = cjAniKeyVec3::GetCreateKeyPF(keyType);
                cjAniKeyVec3* newKeys = (cjAniKeyVec3*)pfCreate(keyCount);
                cjAniKeyVec3::PFCopy copy = cjAniKeyVec3::GetCopyKeyPF(keyType);

                for(cjUINT32 k=0; k<keyCount; k++)
                    copy(newKeys->GetKey(k, keySiz), keys->GetKey(k, keySiz));

                tmEvaluator->ReplaceScaleKeys(newKeys, keyCount, keyType);
            }
        }
    }

    return tmEvaluator;
}

cjAniEvaluator* cjEvaluatorConv::DoLookAtInterpolator(cjLookAtInterpolator* lookAtInterp, const cjName& name, 
	cjBool doCollapse)
{
    cjAniEvaluatorRPtr spPosEvaluator;
    cjAniEvaluatorRPtr spRollEvaluator;
    cjAniEvaluatorRPtr spScaleEvaluator;

    cjAniEvaluatorTag tag;

    cjInterpolator* interp = lookAtInterp->GetPosInterpolator();
    if(interp)
    {
        spPosEvaluator = cjEvaluatorConv::Convert(interp, tag, doCollapse);

        if(!spPosEvaluator)
            return cjNULL;
    }

    interp = lookAtInterp->GetRollInterpolator();
    if(interp)
    {
        spRollEvaluator = cjEvaluatorConv::Convert(interp, tag, doCollapse);

		if(!spRollEvaluator)
            return cjNULL;
    }

    interp = lookAtInterp->GetScaleInterpolator();
    if(interp)
    {
        spScaleEvaluator = cjEvaluatorConv::Convert(interp, tag, doCollapse);

        if(!spScaleEvaluator)
            return cjNULL;
    }

    cjLookAtEvaluator* lookAtEvaluator = new cjLookAtEvaluator();

    lookAtEvaluator->SetPosEvaluator(spPosEvaluator);
    lookAtEvaluator->SetRollEvaluator(spRollEvaluator);
    lookAtEvaluator->SetScaleEvaluator(spScaleEvaluator);
    lookAtEvaluator->SetFlip(lookAtInterp->GetFlip());
    lookAtEvaluator->SetAxis((cjAxis)lookAtInterp->GetAxis());

    lookAtEvaluator->SetLookAtName(lookAtInterp->GetLookAtName());
    lookAtEvaluator->SetDrivenName(name);

    return lookAtEvaluator;
}

cjAniEvaluator* cjEvaluatorConv::DoPathInterpolator(cjPathInterpolator* pathInterpolator)
{
    cjAniVec3Dat* pathDat = pathInterpolator->GetPathDat();
    cjAniFloatDat* pctDat = pathInterpolator->GetPCTDat();

    if(!pathDat || !pctDat)
        return cjNULL;

    cjPathEvaluator* pathEvaluator = new cjPathEvaluator();

    pathEvaluator->SetPathDat(pathDat);
    pathEvaluator->SetPCTDat(pctDat);

    pathEvaluator->SetAllowFlip(pathInterpolator->IsAllowFlip());
    pathEvaluator->SetBanking(pathInterpolator->IsBanking());
    pathEvaluator->SetBankDir((cjPathEvaluator::BANKDIR)pathInterpolator->GetBankDir());
    pathEvaluator->SetConstSpeed(pathInterpolator->IsConstSpeed());
    pathEvaluator->SetFollow(pathInterpolator->IsFollow());
    pathEvaluator->SetMaxBankAngle(pathInterpolator->GetMaxBankAngle());
    pathEvaluator->SetSmoothing(pathInterpolator->GetSmoothing());
    pathEvaluator->SetFollowAxis(pathInterpolator->GetFollowAxis());
    pathEvaluator->SetFlip(pathInterpolator->GetFlip());

    return pathEvaluator;
}

