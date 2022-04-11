
#include "cjCorePCH.h"
#include "cjController.h"


//
// cjAniCntr
//

CJSER_IMPL_CLASS(cjAniCntr, cjRef)

// serialize
CJSER_IMPL_SERIALIZE_BEGIN(cjAniCntr)

	if(ser.IsQuerySerObjAndStr())
	{
		if(m_nextCntr)
			ser << m_nextCntr;

		return;
	}
	
	if(ser.IsSaving() || ser.IsLoading())
	{
		ser.SerializeSerObjPtr(&m_nextCntr);

		CJFLAG_SERIALIZE
		ser << m_frequency;
		ser << m_phase;
		ser << m_stKeyTime;
		ser << m_enKeyTime;

		ser.SerializeObj(m_target);

		// m_startCalcTime, m_lastCalcTime, m_curTime, m_accumFreqTime
		// no need to serialize

		if(ser.IsLoading())
		{
			SetTimeUpdate(ANITIMEUPDATE_ON);
			SetForceUpdate(cjFalse);
		}
	}

CJSER_IMPL_SERIALIZE_END(cjAniCntr)

CJSER_IMPL_SERIALIZE_POST_BEGIN(cjAniCntr)

	if(ser.IsLoading())
	{
		ser.PostSerializeSerObjPtr(m_nextCntr);
		ser.PostSerializeSerObj(m_target);
	}

CJSER_IMPL_SERIALIZE_POST_END(cjAniCntr)


CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjAniCntr)

	CJFLAG_CLONE
	d->m_nextCntr		= m_nextCntr;

	d->m_frequency		= m_frequency;
	d->m_phase			= m_phase;
	d->m_stKeyTime		= m_stKeyTime;
	d->m_enKeyTime		= m_enKeyTime;
	
	d->m_target			= cjNULL;

	d->m_startCalcTime	= m_startCalcTime;
	d->m_lastCalcTime	= m_lastCalcTime;

	d->m_curTime		= m_curTime;
	d->m_accumFreqTime	= m_accumFreqTime;
	
	if(m_target)
		d->SetTarget(cloner->GetClonedAsRefObj(m_target));

	if(m_nextCntr)
		d->m_nextCntr = (cjAniCntr*)m_nextCntr->_Clone(cloner);

CJSER_IMPL_CLONE_PROPCOPY_END(cjAniCntr)

CJSER_IMPL_CLONE_POST_BEGIN(cjAniCntr)

	if(m_nextCntr)
        m_nextCntr->_PostClone(cloner);

CJSER_IMPL_CLONE_POST_END(cjAniCntr)


cjSerializer& cjAniCntr::SerializeList(cjSerializer& ser, cjAniCntrRPtr& spAniCntr)
{
	if(ser.IsQuerySerObjAndStr())
	{
		if(!spAniCntr)
			return ser;

		cjAniCntr* cur = spAniCntr;

		while(cur)
		{
			*cur >> ser;
			cur = cur->m_nextCntr;
		}

		return ser;
	}	
	else if(ser.IsLoading() || ser.IsSaving())
	{
		cjINT32 count = 0;
		
		if(ser.IsSaving())
		{
			if(spAniCntr)
				count = spAniCntr->GetNextCntrCount();
		}

		ser << cjFStreamCmprI32(count);

		cjAniCntr* cur = spAniCntr;

		if(ser.IsSaving())
		{
			while(cur)
			{
				ser.SerializeObj(cur);
				cur = cur->m_nextCntr;
			}
		}
		else 
		{
			ser.EnqueuePostSerializeObjCount(count);

			for(cjAryOff i=0; i<count; i++)
				ser.SerializeObjForPaddingOnLoad();
		}
	}

	return ser;
}

cjSerializer& cjAniCntr::PostSerializeList(cjSerializer& ser, cjAniCntrRPtr& spAniCntr)
{
	if(ser.IsLoading())
	{
		cjAryOff count = 0;
		ser.PopPostSerializeObjCount(count);

		spAniCntr = cjNULL;

		if(count > 0)
		{			
			ser.PostSerializeSerObjPtr(spAniCntr);
			count--;

			cjAniCntrRPtr* prev = &(spAniCntr->m_nextCntr);

			while(count)
			{
				ser.PostSerializeSerObjPtr(*prev);				
				prev = &((*prev)->m_nextCntr);

				count--;
			}
		}
	}

	return ser;
}

cjAniCntr::cjAniCntr()
{
	CJFLAG_IMPL_INIT
	//m_nextCntr;

	// flag
	SetBeginType(ANIBEGIN_AUTO);
	SetLoop(ANILOOP_ONE);
	SetActive(cjTrue);
	SetTimeUpdate(ANITIMEUPDATE_ON);
	SetForceUpdate(cjFalse);

	// static
	m_frequency = 1.0f;
	m_phase = 0.0f;
	m_stKeyTime = CJ_FTIME_INFINITY;
	m_enKeyTime = CJ_FTIME_INVALID;
	
	// update
	m_target = 0;

	m_startCalcTime	= CJ_FTIME_INVALID;
	m_lastCalcTime	= CJ_FTIME_INVALID;
	m_curTime		= CJ_FTIME_INVALID;
}

cjAniCntr::~cjAniCntr()
{
}

void cjAniCntr::SetTarget(cjRefObj* target)
{
    if(m_target == target)
        return;

	AddRef();

    if(m_target) // remove from old list
    {
        if(m_target->GetFirstAniCntr())
            m_target->DelAniCntr(this);
    }

	m_target = target;

    if(m_target) // add on last	
	{

#ifdef CJDEBUG
		if(target) 
		{
			if(!_CheckAcceptableTarget())
				return;
		}
#endif	
		
		// check already inserted.
		cjAniCntr* cntr = m_target->GetFirstAniCntr();
		while(cntr)
		{
			if(cntr == this)
			{
				ReleaseRef();
				return;
			}
			cntr = cntr->GetNextCntr();
		}

		m_target->AddAniCntr(this);
    }
	
	ReleaseRef();
}

void cjAniCntr::Start(cjFLOAT etime)
{
	SetActive(cjTrue);

	m_lastCalcTime = CJ_FTIME_INVALID;

	if(GetBeginType() == ANIBEGIN_MANUAL)
		m_startCalcTime = etime;
}

void cjAniCntr::Stop()
{
    SetActive(cjFalse);
	m_lastCalcTime = CJ_FTIME_INVALID;

    if(GetBeginType() == ANIBEGIN_MANUAL)
		m_lastCalcTime = CJ_FTIME_INVALID;
}

void cjAniCntr::RecursiveStartAnimations(cjRefObj* obj)
{
    cjAniCntr* cntr = obj->GetFirstAniCntr();
    for(; cntr; cntr=cntr->GetNextCntr())
		cntr->Start();

	if(!obj->cjsIsKindOf<cjNode>())
		return;

	cjNode* child = ((cjNode*)obj)->GetFirChild();
	while(child)
	{
		RecursiveStartAnimations(child);
		child = child->GetSibling();
	}	
}

void cjAniCntr::RecursiveStartAnimations(cjRefObj* obj, cjFLOAT etime)
{
    cjAniCntr* cntr = obj->GetFirstAniCntr();
    for(; cntr; cntr=cntr->GetNextCntr())
		cntr->Start();

	if(!obj->cjsIsKindOf<cjNode>())
		return;
	
	cjNode* child = ((cjNode*)obj)->GetFirChild();
	while(child)
	{
		RecursiveStartAnimations(child, etime);
		child = child->GetSibling();
	}
}

void cjAniCntr::RecursiveStopAnimations(cjRefObj* obj)
{
	cjAniCntr* cntr = obj->GetFirstAniCntr();
	for(;cntr; cntr = cntr->GetNextCntr())
		cntr->Stop();

	if(!obj->cjsIsKindOf<cjNode>())
		return;

	cjNode* child = ((cjNode*)obj)->GetFirChild();
	while(child)
	{
		RecursiveStopAnimations(child);
		child = child->GetSibling();
    }
}

cjFLOAT cjAniCntr::FindMinStKeyTime(cjRefObj* obj)
{
	cjFLOAT minTime = CJ_FTIME_INFINITY;
	RecursiveGetMinStKeyTime(obj, minTime);

	if(minTime == CJ_FTIME_INFINITY)
		minTime = 0.0f;

	return minTime;
}

void cjAniCntr::RecursiveGetMinStKeyTime(cjRefObj* obj, cjFLOAT& minStKeyTime)
{
	cjAniCntr* cntr = obj->GetFirstAniCntr();
	while(cntr)
	{
		if(cntr->GetStKeyTime() < minStKeyTime)
			minStKeyTime = cntr->GetStKeyTime();

		cntr = cntr->GetNextCntr();
	}

	if(!obj->cjsIsKindOf<cjNode>())
		return;

	cjNode* child = ((cjNode*)obj)->GetFirChild();
	while(child)
	{
		RecursiveGetMinStKeyTime(child, minStKeyTime);
		child = child->GetSibling();
    }
}

cjBool cjAniCntr::UpdateTime(cjFLOAT etime)
{
   if(!GetActive() || (m_lastCalcTime == etime && !GetForceUpdate()))
        return cjTrue;

    if(GetTimeUpdate())
    {
        cjFLOAT calcTime = CalcTimeAndAccumFreqTime(etime);

        if(m_curTime == calcTime && !GetForceUpdate())
            return cjTrue;

        m_curTime = calcTime;
    }

    SetForceUpdate(cjFalse);

    return cjFalse;
}

cjFLOAT cjAniCntr::CalcTimeAndAccumFreqTime(cjFLOAT etime)
{
    cjFLOAT curTime = 0.0f;
    cjFLOAT dtime;

    if(m_startCalcTime == CJ_FTIME_INVALID)
		m_startCalcTime = etime;
	
    if(m_lastCalcTime == CJ_FTIME_INVALID)
    {
        m_accumFreqTime = 0.0f;

        if(GetBeginType() == ANIBEGIN_MANUAL)
            dtime = 0.0f;
        else
            dtime = etime;
    }
    else
	{
		dtime = etime - m_lastCalcTime;
	}
	
    m_accumFreqTime += dtime * m_frequency;
	curTime = m_accumFreqTime + m_phase;

	m_lastCalcTime = etime;

	ANILOOP loop = GetLoop();

    if(m_enKeyTime != CJ_FTIME_INVALID && m_stKeyTime != CJ_FTIME_INFINITY)
    {
		switch(loop)
		{
		case ANILOOP_LOOP:
			{
				cjFLOAT dtHLKey = m_enKeyTime - m_stKeyTime;
				if(dtHLKey != 0.0f)
				{
					curTime = cjFMod((curTime - m_stKeyTime), dtHLKey) + m_stKeyTime;
					if(curTime < m_stKeyTime)
						curTime += dtHLKey;
				}
				else
					curTime = m_stKeyTime;
			}
			break;
		
		case ANILOOP_REVERSE:
			{
				cjFLOAT dtHLKey = m_enKeyTime - m_stKeyTime;
				cjFLOAT dtHLElapsed;

				if(dtHLKey != 0.0f)
				{
					cjFLOAT dtHLKey2X = dtHLKey* 2.0f;

					// mod = curTime - ((int)(curTime / dtHLKey2X) * dtHLKey2X);
					dtHLElapsed = cjFMod(curTime, dtHLKey2X);

					if(dtHLElapsed < 0.0f)
						dtHLElapsed += dtHLKey2X;

					if(dtHLElapsed > dtHLKey)
						curTime = dtHLKey2X - dtHLElapsed + m_stKeyTime;
					else
						curTime = dtHLElapsed + m_stKeyTime;
				}
				else
					curTime = m_stKeyTime;
			}
			break;

        case ANILOOP_ONE:
            break;

        default:
            break;
		}
    }

	//
	// clip calculated time
	//

    if(curTime > m_enKeyTime)
		curTime = m_enKeyTime;
    else if(curTime < m_stKeyTime)
		curTime = m_stKeyTime;

    if(GetDir() == ANIDIR_BACKWARD)
        curTime = m_enKeyTime - (curTime - m_stKeyTime);

    return curTime;
}

cjUINT32 cjAniCntr::GetNextCntrCount() const
{
    const cjAniCntr* cur = this;
    cjUINT32 count = 0;

    for(; cur; cur=cur->m_nextCntr)
		count++;

    return count;
}


//
// cjInterpolationAniCntr
//

CJSER_IMPL_CLASS(cjInterpolationAniCntr, cjAniCntr)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjInterpolationAniCntr)
CJSER_IMPL_CLONE_PROPCOPY_END(cjInterpolationAniCntr)


const cjFLOAT cjInterpolationAniCntr::INVALID_TIME = CJ_FTIME_INVALID;

cjInterpolationAniCntr::cjInterpolationAniCntr()
{
}

cjInterpolationAniCntr::~cjInterpolationAniCntr()
{
}

void cjInterpolationAniCntr::ResetTimeExtrema()
{
    m_stKeyTime = 0.0f;
    m_enKeyTime = 0.0f;
}

void cjInterpolationAniCntr::_RecusriveGuaranteeTimeRange(cjRefObj* obj, cjFLOAT stTime, cjFLOAT enTime, 
	const cjNameChar* excludedCntrType)
{
	cjNameChar tmp[cjMAX_PATH];

	cjAniCntr* cntr = obj->GetFirstAniCntr();
    while(cntr)
    {
		cjInterpolationAniCntr* ipCntr = cntr->cjsCast<cjInterpolationAniCntr>();
        if(ipCntr)
        {
            cjSprintf(tmp, cjMAX_PATH, _CT("%s;"), ipCntr->cjrRTTI()->cjrGetNameW());

            if(!excludedCntrType || !cjStrstr(excludedCntrType, tmp))
				ipCntr->_GuaranteeTimeRange(stTime, enTime);
        }
		cntr = cntr->GetNextCntr();
    }

	cjNode* node = obj->cjsCast<cjNode>();
    if(node)
    {
		cjNode* child = node->GetFirChild();
		while(child)
		{
			_RecusriveGuaranteeTimeRange(child, stTime, enTime, excludedCntrType);
			child = child->GetSibling();
		}
    }
}


//
// cjInterpolationCntr
//

CJSER_IMPL_CLASS(cjSingleInterpolationAniCntr, cjInterpolationAniCntr)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjSingleInterpolationAniCntr)
	if(m_spInterpolator)
		d->m_spInterpolator = (cjInterpolator*)m_spInterpolator->_CloneShared(cloner);
CJSER_IMPL_CLONE_PROPCOPY_END(cjSingleInterpolationAniCntr)

CJSER_IMPL_CLONE_POST_BEGIN(cjSingleInterpolationAniCntr)
	if(m_spInterpolator)
        m_spInterpolator->_PostClone(cloner);
CJSER_IMPL_CLONE_POST_END(cjSingleInterpolationAniCntr)


cjSingleInterpolationAniCntr::cjSingleInterpolationAniCntr()
{
}

cjSingleInterpolationAniCntr::~cjSingleInterpolationAniCntr()
{
}

void cjSingleInterpolationAniCntr::ResetTimeExtrema()
{
	cjSuper::ResetTimeExtrema();

	if(m_spInterpolator)
        m_spInterpolator->GetValidTimeRange(m_stKeyTime, m_enKeyTime);
}

cjInterpolator* cjSingleInterpolationAniCntr::GetInterpolator(cjUINT32 index) const
{
	CJ_UNREF_VALUE(index)
	cjAssert(index == 0);

	return m_spInterpolator;
}

void cjSingleInterpolationAniCntr::SetInterpolator(cjInterpolator* interpolator, cjUINT32 index)
{
	CJ_UNREF_VALUE(index);
	cjAssert(index == 0);
//	cjAssert(!interpolator || _IsCorrectInterpolator(interpolator, index));

    m_spInterpolator = interpolator;
}

void cjSingleInterpolationAniCntr::_GuaranteeTimeRange(cjFLOAT stTime, cjFLOAT enTime)
{
	if(m_spInterpolator)
		m_spInterpolator->Cache(stTime, enTime);
}

cjBool cjSingleInterpolationAniCntr::_CheckAcceptableTarget()
{
    return _CheckAcceptableInterpolator(m_target);
}


//
// cjSingleInterpolationAniCntr
//  - cjBoolInterpAniCntr
//

CJSER_IMPL_CLASS(cjBoolInterpAniCntr, cjSingleInterpolationAniCntr)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjBoolInterpAniCntr)	
CJSER_IMPL_CLONE_PROPCOPY_END(cjBoolInterpAniCntr)


cjBoolInterpAniCntr::cjBoolInterpAniCntr()
{
}

cjBoolInterpAniCntr::~cjBoolInterpAniCntr()
{
}

cjAniEvaluator* cjBoolInterpAniCntr::_CreatePoseEvaluator(cjUINT32 index)
{
	cjAssert(index == 0);
    cjBool targetValue;

    GetTargetValueBool(targetValue);
    cjFLOAT value = targetValue ? 1.0f : 0.0f;

    return new cjConstBoolEvaluator(value);
}

cjInterpolator* cjBoolInterpAniCntr::_CreatePoseInterpolator(cjUINT32 index)
{
	CJ_UNREF_VALUE(index)
	cjAssert(index == 0);
    cjBool targetValue;

	GetTargetValueBool(targetValue);

    return new cjBoolInterpolator(targetValue);
}

void cjBoolInterpAniCntr::_SynchronizePoseInterpolator(cjInterpolator* interpolator, cjUINT32 index)
{
	CJ_UNREF_VALUE(index)
	cjAssert(index == 0);
    cjBool targetValue;
   
	cjBoolInterpolator* boolInterp = cjrCast<cjBoolInterpolator>(interpolator);
    cjAssert(boolInterp);

	GetTargetValueBool(targetValue);
    boolInterp->SetPoseValue(targetValue);
}   

cjBlendInterpolator* cjBoolInterpAniCntr::_CreateBlendInterpolator(cjUINT32 index, 
	cjBool isManagerControlled, cjFLOAT weightThreshold, cjBYTE arySiz) const
{
    CJ_UNREF_VALUE(index);
    cjAssert(index == 0);

    return new cjBlendBoolInterpolator(isManagerControlled, weightThreshold, arySiz);
}


//
// cjSingleInterpolationAniCntr
//  - cjBoolInterpAniCntr
//    - cjVisibleAniCntr
//

CJSER_IMPL_CLASS(cjVisibleAniCntr, cjBoolInterpAniCntr)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjVisibleAniCntr)	
CJSER_IMPL_CLONE_PROPCOPY_END(cjVisibleAniCntr)

CJSER_IMPL_CLONE(cjVisibleAniCntr)


cjVisibleAniCntr::cjVisibleAniCntr()
{
}

cjVisibleAniCntr::~cjVisibleAniCntr()
{
}

void cjVisibleAniCntr::Update(cjFLOAT etime)
{
    if(GetManagerControlled())
        return;

    if(UpdateTime(etime) && (!m_spInterpolator || !m_spInterpolator->IsAlwaysUpdate()))
        return;

    if(m_spInterpolator)
    {
        cjBool value;

		if(m_spInterpolator->Update(m_curTime, m_target, value))
            ((cjRefObj*)m_target)->SetCulled(!value);
    }
}

cjBool cjVisibleAniCntr::UpdateValue(cjFLOAT , cjBool v, cjUINT32 )
{
    cjAssert(GetManagerControlled());
    cjAssert(m_target);

    ((cjRefObj*)m_target)->SetCulled(!v);

    return cjTrue;
}

void cjVisibleAniCntr::GetTargetValueBool(cjBool& value)
{
    value = !((cjRefObj*)m_target)->IsCulled();
}

cjBool cjVisibleAniCntr::_CheckAcceptableInterpolator(cjRefObj* target) const
{
    return cjrIsKindOf<cjSceneNode>(target);
}



//
// cjTransformAniCntr
//

CJSER_IMPL_CLASS(cjTransformAniCntr, cjSingleInterpolationAniCntr)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjTransformAniCntr)
CJSER_IMPL_CLONE_PROPCOPY_END(cjTransformAniCntr)

CJSER_IMPL_CLONE(cjTransformAniCntr)


cjTransformAniCntr::cjTransformAniCntr() 
{
}

cjTransformAniCntr::~cjTransformAniCntr() 
{
}

cjAniEvaluator* cjTransformAniCntr::_CreatePoseEvaluator(cjUINT32 index)
{
	cjAssert(index == 0);
	cjSceneNode* target = (cjSceneNode*)m_target;

	cjQuat rot;
	target->GetRotate(rot);
	cjQuatTM tm(target->GetScale(), rot, target->GetPos());

	return new cjConstTMEvaluator(tm);
}

cjInterpolator* cjTransformAniCntr::_CreatePoseInterpolator(cjUINT32 index)
{
	CJ_UNREF_VALUE(index);
	cjAssert(index == 0);

	cjSceneNode* target = (cjSceneNode*)m_target;

	cjQuat rot;
	target->GetRotate(rot);

	cjQuatTM tm(target->GetScale(), rot, target->GetPos());

	return new cjTMInterpolator(tm);
}

void cjTransformAniCntr::_SynchronizePoseInterpolator(cjInterpolator* interpolator, 
	cjUINT32 index)
{
	CJ_UNREF_VALUE(index);
	cjAssert(index == 0);

	cjTMInterpolator* tmInterpolator = cjrCast<cjTMInterpolator>(interpolator);
	cjAssert(tmInterpolator);

	if(!tmInterpolator)
		return;

	cjSceneNode* target = (cjSceneNode*)m_target;

	cjQuat rot;
	target->GetRotate(rot);

	cjQuatTM tm(target->GetScale(), rot, target->GetPos());
	tmInterpolator->SetPoseValue(tm);
}   

cjBlendInterpolator* cjTransformAniCntr::_CreateBlendInterpolator(cjUINT32 index,
	cjBool isManagerControlled, cjFLOAT weightThreshold, cjBYTE arySiz) const
{
	CJ_UNREF_VALUE(index);
	cjAssert(index == 0);

	return new cjBlendTMInterpolator(isManagerControlled, weightThreshold, arySiz);
}

void cjTransformAniCntr::Update(cjFLOAT etime)
{
	if(!UpdateTime(etime) && (!m_spInterpolator || !m_spInterpolator->IsAlwaysUpdate()))
		return;

	if(m_spInterpolator)
	{
		cjQuatTM tm;

		if(m_spInterpolator->Update(m_curTime, m_target, tm))
		{
			cjSceneNode* target = (cjSceneNode*)m_target;

			if(tm.IsPosValid())
				target->SetPos(tm.GetPos());

			if(tm.IsRotateValid())
				target->SetRotate(tm.GetRotate());

			if(tm.IsScaleValid())
				target->SetScale(tm.GetScale());
		}   
	}
}

cjBool cjTransformAniCntr::_CheckAcceptableInterpolator(cjRefObj* target) const
{
	return cjrIsKindOf<cjSceneNode>(target);
}


