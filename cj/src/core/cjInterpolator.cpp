
#include "cjCorePCH.h"
#include "cjInterpolator.h"


//
// cjInterpolator
//

CJSER_IMPL_CLASS(cjInterpolator, cjRef)

CJSER_IMPL_SERIALIZE_BEGIN(cjInterpolator)
	
	if(ser.IsLoading() || ser.IsSaving())
	{
		ser << m_lastTime;
	}

CJSER_IMPL_SERIALIZE_END(cjInterpolator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjInterpolator)
//	d->m_lastTime = m_lastTime;
CJSER_IMPL_CLONE_PROPCOPY_END(cjInterpolator)

const cjBool cjInterpolator::INVALID_BOOL		= (cjBool)0;
const cjFLOAT cjInterpolator::INVALID_FLOAT		= -CJ_FLT_MAX;
const cjQuat cjInterpolator::INVALID_QUAT		= cjQuat(-CJ_FLT_MAX, -CJ_FLT_MAX, -CJ_FLT_MAX, -CJ_FLT_MAX);
const cjVec3 cjInterpolator::INVALID_VEC3		= cjVec3(-CJ_FLT_MAX, -CJ_FLT_MAX, -CJ_FLT_MAX);
const cjColorF cjInterpolator::INVALID_COLORF	= cjColorF(-CJ_FLT_MAX, -CJ_FLT_MAX, -CJ_FLT_MAX, -CJ_FLT_MAX);


cjInterpolator::cjInterpolator() : m_lastTime(CJ_FTIME_INVALID)
{
}

cjInterpolator::~cjInterpolator()
{
}

cjInterpolator* cjInterpolator::GetInterpolator(cjFLOAT , cjFLOAT )
{
    cjInterpolator* interp = (cjInterpolator*)Clone();
    return interp;
}


//
// cjLookAtInterpolator
//

CJSER_IMPL_CLASS(cjLookAtInterpolator, cjInterpolator);

CJSER_IMPL_SERIALIZE_BEGIN(cjLookAtInterpolator)

	if(ser.IsQuerySerObjAndStr())
	{
		if(m_lookAt)
			(*m_lookAt) >> ser;

		for(cjINT32 i=0; i<3; i++)
		{
			if(m_interpolators[i])
				(*m_interpolators[i]) >> ser;
		}

		return;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		cjSerID serID;
		ser << _cjtfl_flag;

		// m_lookAt
		if(ser.IsSaving())
			serID = ser.GetSerObjID(m_lookAt);

		cjSerID_Serialize(ser, serID);

		if(ser.IsLoading())
			m_lookAt = ser.GetSerObj<cjRefObj>(serID);

		ser << m_lookAtName;
		ser << m_tm;

		// m_interpolators
		for(cjINT32 i=0; i<3; i++)
		{
			if(ser.IsSaving())		
				serID = ser.GetSerObjID(m_interpolators[i]);

			cjSerID_Serialize(ser, serID);

			if(ser.IsLoading())
				m_interpolators[i] = ser.GetSerObj<cjInterpolator>(serID);
		}
	}

CJSER_IMPL_SERIALIZE_END(cjLookAtInterpolator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjLookAtInterpolator)
	
	d->_cjtfl_flag = _cjtfl_flag;

	d->SetFlip(GetFlip());
	d->SetAxis(GetAxis());
	d->SetLookAt(GetLookAt());

	for(cjUINT32 i=0; i<3; i++)
	{
		if(m_interpolators[i])
			d->m_interpolators[i] = (cjInterpolator*)m_interpolators[i]->Clone();
	}
 
CJSER_IMPL_CLONE_PROPCOPY_END(cjLookAtInterpolator)

CJSER_IMPL_CLONE(cjLookAtInterpolator)

CJSER_IMPL_CLONE_POST_BEGIN(cjLookAtInterpolator)

	cjLookAtInterpolator* dstLookAtInterp = (cjLookAtInterpolator*)cloner->GetCloned(this);
	cjRefObj* clonedLookAt = (cjRefObj*)cloner->GetCloned(m_lookAt);

	if(clonedLookAt)
		dstLookAtInterp->m_lookAt = (cjRefObj*)clonedLookAt;
	else
		dstLookAtInterp->m_lookAt = m_lookAt;

CJSER_IMPL_CLONE_POST_END(cjLookAtInterpolator)

void cjLookAtInterpolator::_InitCommon()
{
	_cjtfl_flag				= 0;
	m_lookAt				= cjNULL;

    m_interpolators[0]		= cjNULL;
    m_interpolators[1]		= cjNULL;
    m_interpolators[2]		= cjNULL;
}

cjLookAtInterpolator::cjLookAtInterpolator()
{
	_InitCommon();

    SetFlip(cjFalse);
    SetAxis(cjAxisX);
}

cjLookAtInterpolator::cjLookAtInterpolator(cjRefObj* lookAt, cjAxis axis, cjBool isFlip)
{
	_InitCommon();

	SetFlip(isFlip);
    SetAxis(axis);
}

cjLookAtInterpolator::~cjLookAtInterpolator()
{
    SetLookAtName(cjNULL);
    m_lookAt = cjNULL;
}

cjBool cjLookAtInterpolator::Update(cjFLOAT etime, cjRefObj* interpTarget, cjQuatTM& quatTM)
{
    if(!m_lookAt || !m_lookAt->GetSceneNode() || !interpTarget || !interpTarget->GetSceneNode())
    {
        quatTM.MakeInvalid();
        m_tm.MakeInvalid();

        return cjFalse;
    }

	cjSceneNode*		target = interpTarget->GetSceneNode();
    cjMatrix3			parentWorldRot;
    cjVec3				worldPos;
    cjVec3				targetWorldPos = m_lookAt->GetSceneNode()->GetWorldPos();    
    cjSceneNode*		parent = target->GetParentSceneNode();
    cjInterpolator*		posInterp = GetPosInterpolator();
    cjVec3				localPos;

    if(posInterp)
    {
        if(!posInterp->Update(etime, interpTarget, localPos))
        {
            m_tm.MakeInvalid();
            return cjFalse;
        }

        m_tm.SetPos(localPos);
    }
    else
    {
        localPos = target->GetPos();
    }

    // recalc world pos
    if(parent)
    {
        parentWorldRot = parent->GetWorldRotate();
        worldPos = parent->GetWorldPos() + parent->GetWorldScale() * (parentWorldRot * localPos);
    }
    else
    {
        parentWorldRot = cjMatrix3::Identity;
        worldPos = localPos;
    }
    
    cjMatrix3 rot;
    cjVec3 lookAt = targetWorldPos - worldPos;

    if(lookAt.SquareXYZ() < 0.001f)
    {
        rot = cjMatrix3::Identity;
    }
    else    
    {
        cjVec3 up, right;
        cjFLOAT dot;

        lookAt.Normalize();

        if(((lookAt.z < 1.001f) && (lookAt.z > 0.999f)) || ((lookAt.z > -1.001f) && (lookAt.z < -0.999f)))
        { 	// too close up and lookat, so use unit y as up
            up = cjVec3::UNIT_Y;
            dot = lookAt.y;
        }
        else
        {
            up = cjVec3::UNIT_Z;
            dot = lookAt.z;
        }
		        
        up -= dot*lookAt;
        up.Normalize();

        if(!GetFlip())
            lookAt = -lookAt;

        right = up.Cross(lookAt);

        switch(GetAxis())
        {
		case cjAxisX: { rot.SetCol(0, lookAt); rot.SetCol(1, up);		rot.SetCol(2, -right);	} break;
		case cjAxisY: { rot.SetCol(0, right);	 rot.SetCol(1, lookAt); rot.SetCol(2, -up);		} break;
		case cjAxisZ: { rot.SetCol(0, right);  rot.SetCol(1, up);		rot.SetCol(2, lookAt);	} break;
        }

		cjMatrix3 parWorldRotTP;
		parentWorldRot.Transpose(parWorldRotTP);
        rot = parWorldRotTP * rot;

        cjInterpolator* rollInterp = GetRollInterpolator();
        if(rollInterp)
        {
            cjFLOAT roll;

            if(!rollInterp->Update(etime, interpTarget, roll))
            {
                m_tm.MakeInvalid();
                return cjFalse;
            }
            
            cjMatrix3 rotRoll;
            rotRoll.MakeZRotation(-roll);
            rot = rot * rotRoll;
        }
    }

    cjQuat quat;
    quat.MakeFromRotation(rot);
    m_tm.SetRotate(quat);

    cjInterpolator* scaleInterp = GetScaleInterpolator();

    if(scaleInterp)
    {
        cjVec3 scale;

        if(!scaleInterp->Update(etime, interpTarget, scale))
        {
            m_tm.MakeInvalid();
            return cjFalse;
        }
        
        m_tm.SetScale(scale);
    }
   
    if(m_tm.IsTMInvalid())
    {
        quatTM.MakeInvalid();

        return cjFalse;
    }

    quatTM = m_tm;
    m_lastTime = etime;

    return cjTrue;
}

void cjLookAtInterpolator::Collapse()
{
    for(cjUINT32 i=0; i<3; i++)
    {
        if(m_interpolators[i])
            m_interpolators[i]->Collapse();
    }
}

void cjLookAtInterpolator::GetValidTimeRange(cjFLOAT& stKeyTime, cjFLOAT& enKeyTime) const
{
    stKeyTime = CJ_FTIME_INFINITY;
    enKeyTime = CJ_FTIME_INVALID;
    
    for(cjUINT32 i=0; i<3; i++)
    {
        if(m_interpolators[i])
        {
            cjFLOAT st, en;

			m_interpolators[i]->GetValidTimeRange(st, en);

            if(st != en)
            {
                if(st < stKeyTime)
                    stKeyTime = st;

                if(en > enKeyTime)
                    enKeyTime = en;
            }
        }
    }

    if(stKeyTime == CJ_FTIME_INFINITY && enKeyTime == CJ_FTIME_INVALID)
    {
        stKeyTime = 0.0f;
        enKeyTime = 0.0f;
    }
}

void cjLookAtInterpolator::Cache(cjFLOAT stTime, cjFLOAT enTime)
{
    for(cjUINT32 i=0; i<3; i++)
    {
        if(m_interpolators[i])
            m_interpolators[i]->Cache(stTime, enTime);
    }
}

cjInterpolator* cjLookAtInterpolator::GetInterpolator(cjFLOAT stTime, cjFLOAT enTime)
{
    cjLookAtInterpolator* seqInterp 
		= (cjLookAtInterpolator*)cjInterpolator::GetInterpolator(stTime, enTime);

    cjAssert(seqInterp);

    seqInterp->SetUpDependObj();

    for(cjUINT32 i=0; i<3; i++)
    {
        if(m_interpolators[i])
            seqInterp->m_interpolators[i] = m_interpolators[i]->GetInterpolator(stTime, enTime);
    }

    return seqInterp;
}

cjBool cjLookAtInterpolator::ResolveDependObj(cjRefObjNameHash* objHash)
{
    if(m_lookAt != cjNULL)
        return cjTrue;
    
    cjRefObj* target = objHash->FindObjWithSetObjWhenRootIsSetted(m_lookAtName);

    if(target)
    {
        m_lookAt = target;
        return cjTrue;
    }
    else
    {
        return cjFalse;
    }
}

cjBool cjLookAtInterpolator::SetUpDependObj()
{
    cjAssert(m_lookAt);

    const cjName name = m_lookAt->GetName();

    if(name.IsValid())
    {
        SetLookAtName(name);
        m_lookAt = cjNULL;

        return cjTrue;
    }
    else
    {
        SetLookAtName(cjName_None);

        return cjFalse;
    }
}


//
// cjKeyBasedInterpolator
//

CJSER_IMPL_CLASS(cjKeyBasedInterpolator, cjInterpolator)

CJSER_IMPL_SERIALIZE_BEGIN(cjKeyBasedInterpolator)

CJSER_IMPL_SERIALIZE_END(cjKeyBasedInterpolator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjKeyBasedInterpolator)
CJSER_IMPL_CLONE_PROPCOPY_END(cjKeyBasedInterpolator)


cjKeyBasedInterpolator::cjKeyBasedInterpolator()
{
}

cjKeyBasedInterpolator::~cjKeyBasedInterpolator()
{
}

void cjKeyBasedInterpolator::GetValidTimeRange(cjFLOAT& stKeyTime, cjFLOAT& enKeyTime) const
{
    stKeyTime = CJ_FTIME_INFINITY;
    enKeyTime = CJ_FTIME_INVALID;

    for(cjUINT32 i=0; i<GetKeyChannelCount(); i++)
    {
        cjAniKey* pkBeginKey = GetKey(0, (cjBYTE)i);
        cjAniKey* pkEndKey = GetKey(GetKeyCount((cjBYTE)i)-1, (cjBYTE)i);

        if(pkBeginKey && pkEndKey)
        {
            if(pkBeginKey->GetTime() < stKeyTime)
                stKeyTime = pkBeginKey->GetTime();

            if(pkEndKey->GetTime() > enKeyTime)
                enKeyTime = pkEndKey->GetTime();
        }
    }

    if(stKeyTime == CJ_FTIME_INFINITY && enKeyTime == CJ_FTIME_INVALID)
    {
        stKeyTime = 0.0f;
        enKeyTime = 0.0f;
    }
}

cjUINT32 cjKeyBasedInterpolator::GetAllocSiz(cjUINT32 channel) const
{
    cjAssert(channel < GetKeyChannelCount());

    if(GetKeyType(channel) == cjAniKey::EKT_EULERKEY)
    {
        cjAniKeyEulerRot* eulerKey = (cjAniKeyEulerRot*)GetKeyAry(channel);

        cjUINT32 eulerKeySiz = 0;
        if(eulerKey)
        {
            eulerKeySiz = sizeof(cjAniKeyEulerRot);

            for(cjUINT32 i=0; i<3; i++)
            {
                cjUINT32 keyCount = eulerKey->GetKeyCount((cjBYTE)i);
                cjUINT32 keySiz = eulerKey->GetKeySiz((cjBYTE)i);
                eulerKeySiz += keyCount * keySiz;
            }           
        }

        return eulerKeySiz;
    }

    return GetKeyCount(channel) * GetKeySiz(channel);
}

cjAniKey* cjKeyBasedInterpolator::GetKey(cjUINT32 index, cjUINT32 channel) const
{
    cjAssert(channel < GetKeyChannelCount());

    return GetKeyAry(channel)->GetKey(index, GetKeySiz(channel));
}

cjAniKey* cjKeyBasedInterpolator::GetKeysWithChannel(cjUINT32& rstKeyCount, cjAniKey::EKEY_TYPE& rstKeyType, 
	cjBYTE& rstKeySiz, cjUINT32 channel) const
{
    cjAssert(channel < GetKeyChannelCount());

    rstKeyCount = GetKeyCount(channel);
    rstKeyType = GetKeyType(channel);
    rstKeySiz = GetKeySiz(channel);

    return GetKeyAry(channel);
}

void cjKeyBasedInterpolator::FillDerivedValues(cjUINT32 channel)
{
    cjUINT32 keyCount;
    cjAniKey::EKEY_TYPE keyType;
    cjBYTE keySiz;

    cjAniKey* keys = GetKeysWithChannel(keyCount, keyType, keySiz, channel);
    cjAssert(keys);

    cjAniKey::PFFillDerivedVals pfnFillDerivedVals;
    pfnFillDerivedVals = cjAniKey::GetFillDerivedValsKeyPF(GetKeyValType(channel), keyType);
    cjAssert(pfnFillDerivedVals);

    (*pfnFillDerivedVals)(keys, keyCount, keySiz);
}


//
// cjKeyBasedInterpolator
// - cjFloatInterpolator
//

CJSER_IMPL_CLASS(cjFloatInterpolator, cjKeyBasedInterpolator)

CJSER_IMPL_SERIALIZE_BEGIN(cjFloatInterpolator)

	if(ser.IsQuerySerObjAndStr())
	{
		if(m_spFloatDat)
			*(m_spFloatDat) >> ser;

		return;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		cjSerID serID = cjSerID_Invalid;

		ser << m_value;

		if(ser.IsSaving())
		{
			serID = ser.GetSerObjID(m_spFloatDat);
			cjSerID_Serialize(ser, serID);
		}
		else if(ser.IsLoading())
		{
			cjSerID_Serialize(ser, serID);
			m_spFloatDat = ser.GetSerObj<cjAniFloatDat>(serID);
		}
		
		ser << m_lastIndex;
	}
	
CJSER_IMPL_SERIALIZE_END(cjFloatInterpolator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjFloatInterpolator)

	d->m_value = m_value;
	d->m_spFloatDat = m_spFloatDat;
	d->m_lastIndex = m_lastIndex;

CJSER_IMPL_CLONE_PROPCOPY_END(cjFloatInterpolator)

CJSER_IMPL_CLONE(cjFloatInterpolator)


cjFloatInterpolator::cjFloatInterpolator() :
    m_value(CJ_FLOAT_INVALID), m_spFloatDat(cjNULL), m_lastIndex(0) 
{
}

cjFloatInterpolator::cjFloatInterpolator(cjAniFloatDat* aniFloatDat) :
    m_value(CJ_FLOAT_INVALID), m_spFloatDat(aniFloatDat), m_lastIndex(0) 
{
}

cjFloatInterpolator::cjFloatInterpolator(cjFLOAT poseValue) :
    m_value(poseValue), m_spFloatDat(cjNULL), m_lastIndex(0)
{
}

cjFloatInterpolator::~cjFloatInterpolator()
{
}

cjBool cjFloatInterpolator::Update(cjFLOAT etime, cjRefObj*, cjFLOAT& v)
{
    if(!TimeHasChanged(etime))
    {
         v = m_value;

         if(m_value == CJ_FLOAT_INVALID)
            return cjFalse;

         return cjTrue;
    }

    cjUINT32 keyCount; cjAniKeyFloat::EKEY_TYPE keyType; cjBYTE siz;
    cjAniKeyFloat* keys = GetKeys(keyCount, keyType, siz);

    if(keyCount > 0)
        m_value = cjAniKeyFloat::GenInterp(etime, keys, keyType, keyCount, m_lastIndex, siz);
    
    if(m_value == CJ_FLOAT_INVALID)
        return cjFalse;

    v = m_value;
    m_lastTime = etime;

    return cjTrue;
}

void cjFloatInterpolator::Collapse()
{
    if(m_spFloatDat)
    {
        cjUINT32 keyCount; cjAniKey::EKEY_TYPE keyType; cjBYTE siz;
        cjAniKeyFloat* keys = m_spFloatDat->GetKeys(keyCount, keyType, siz);

        if(keyCount == 0)
        {
            m_spFloatDat = cjNULL;
            m_value = CJ_FLOAT_INVALID;
        }
        else
        {
            cjAniKey::PFIsPosed pfIsPosed = cjAniKeyFloat::GetIsPosedKeyPF(keyType);
            if(pfIsPosed(keys, keyCount))
            {
                m_value = keys->GetKey(0, siz)->GetValue();
                m_spFloatDat = cjNULL;
            }
        }
    }
}

void cjFloatInterpolator::Cache(cjFLOAT stTime, cjFLOAT enTime)
{
    if(m_spFloatDat)
        m_spFloatDat->CacheKeys(stTime, enTime);
}

cjInterpolator* cjFloatInterpolator::GetInterpolator(cjFLOAT stTime, cjFLOAT enTime)
{
    cjFloatInterpolator* interp = (cjFloatInterpolator*)cjKeyBasedInterpolator::GetInterpolator(stTime, enTime);

    if(m_spFloatDat)
    {
        cjAniFloatDatRPtr spNewFloatDat = m_spFloatDat->GetDat(stTime, enTime);
        interp->SetFloatDat(spNewFloatDat);
        interp->m_lastIndex = 0;
    }

    return interp;
}

cjUINT32 cjFloatInterpolator::GetKeyCount(cjUINT32) const
{
    if(!m_spFloatDat)
        return 0;
    else
        return m_spFloatDat->GetKeyCount();
}

cjAniKey::EKEY_TYPE cjFloatInterpolator::GetKeyType(cjUINT32) const
{
    if(!m_spFloatDat)
        return cjAniKey::EKT_NOINTERP;

    cjUINT32 keyCount; cjAniKeyFloat::EKEY_TYPE keyType; cjBYTE siz;
    m_spFloatDat->GetKeys(keyCount, keyType, siz);

    return keyType; 
}

cjAniKey* cjFloatInterpolator::GetKeyAry(cjUINT32) const
{
    if(!m_spFloatDat)
        return cjNULL;

    cjUINT32 keyCount; cjAniKeyFloat::EKEY_TYPE keyType; cjBYTE keySiz;
    cjAniKeyFloat* keys = m_spFloatDat->GetKeys(keyCount, keyType, keySiz);

    return keys; 
}

cjBYTE cjFloatInterpolator::GetKeySiz(cjUINT32) const
{
    if(!m_spFloatDat)
        return 0;

    cjUINT32 keyCount; cjAniKeyFloat::EKEY_TYPE keyType; cjBYTE siz;
    m_spFloatDat->GetKeys(keyCount, keyType, siz);

    return siz; 
}

cjBool cjFloatInterpolator::GetChannelPosed(cjUINT32) const
{
    if(m_spFloatDat)
        return cjFalse;

    if(m_value == CJ_FLOAT_INVALID)
        return cjFalse;

    return cjTrue;
}

cjAniKeyFloat* cjFloatInterpolator::GetKeys(cjUINT32& keyCount, cjAniKeyFloat::EKEY_TYPE& keyType, cjBYTE& siz) const
{
	if(m_spFloatDat)
		return m_spFloatDat->GetKeys(keyCount, keyType, siz);

	keyCount=0; keyType=cjAniKeyFloat::EKT_NOINTERP; siz=0;

	return cjNULL;
}
    
void cjFloatInterpolator::ReplaceKeys(cjAniKeyFloat* keys, cjUINT32 keyCount, cjAniKeyFloat::EKEY_TYPE keyType)
{
	if(keyCount > 0) 
	{
		if(!m_spFloatDat)
			m_spFloatDat = new cjAniFloatDat;

		m_spFloatDat->ReplaceKeys(keys, keyCount, keyType);
		m_lastIndex = 0;
	}
	else if(m_spFloatDat)
	{
		m_spFloatDat->ReplaceKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
	}
}

void cjFloatInterpolator::SetKeys(cjAniKeyFloat* keys, cjUINT32 keyCount, cjAniKeyFloat::EKEY_TYPE keyType)
{
	if(keyCount > 0)
	{
		if(!m_spFloatDat)
			m_spFloatDat = new cjAniFloatDat;

		m_spFloatDat->SetKeys(keys, keyCount, keyType);
		m_lastIndex = 0;
	}
	else if(m_spFloatDat)
	{
		m_spFloatDat->SetKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
	}		
}


//
// cjKeyBasedInterpolator
// - cjBoolInterpolator
//

CJSER_IMPL_CLASS(cjBoolInterpolator, cjKeyBasedInterpolator)

CJSER_IMPL_SERIALIZE_BEGIN(cjBoolInterpolator)

	if(ser.IsQuerySerObjAndStr())
	{
		if(m_spBoolDat)
			*(m_spBoolDat) >> ser;

		return;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		cjSerID serID = cjSerID_Invalid;

		ser << m_value;

		if(ser.IsSaving())
		{
			serID = ser.GetSerObjID(m_spBoolDat);
			cjSerID_Serialize(ser, serID);
		}
		else if(ser.IsLoading())
		{
			cjSerID_Serialize(ser, serID);
			m_spBoolDat = ser.GetSerObj<cjAniBoolDat>(serID);
		}
		
		ser << m_lastIndex;
	}
	
CJSER_IMPL_SERIALIZE_END(cjBoolInterpolator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjBoolInterpolator)

	d->m_value = m_value;
	d->m_spBoolDat = m_spBoolDat;
	d->m_lastIndex = m_lastIndex;

CJSER_IMPL_CLONE_PROPCOPY_END(cjBoolInterpolator)

CJSER_IMPL_CLONE(cjBoolInterpolator)


void cjBoolInterpolator::_CommInit()
{
	m_value = cjFalse; 
	m_spBoolDat = cjNULL;
	m_lastIndex = 0;
}

cjBoolInterpolator::cjBoolInterpolator()
{
	_CommInit();
}

cjBoolInterpolator::cjBoolInterpolator(cjAniBoolDat* boolDat)
{
	_CommInit();
	m_spBoolDat = boolDat;
}

cjBoolInterpolator::cjBoolInterpolator(cjBool poseValue)
{
	_CommInit();
	m_value = poseValue;
}

cjBoolInterpolator::~cjBoolInterpolator()
{
}

cjBool cjBoolInterpolator::Update(cjFLOAT etime, cjRefObj* target, cjBool& v)
{
    if(!TimeHasChanged(etime))
    {
        if(!m_value)
        {
            v = cjFalse;
            return cjFalse;
        }
		
        v = (m_value != cjFalse);
        return cjTrue;
    }

    cjUINT32 keyCount; cjBYTE keySiz; cjAniKeyBool::EKEY_TYPE keyType;
    cjAniKeyBool* keys = GetKeys(keyCount, keyType, keySiz);

    if(keyCount > 0)
    {
        cjBool interpValue = cjAniKeyBool::GenInterp(etime, keys, keyType, keyCount, m_lastIndex, keySiz);
        m_value = (interpValue != cjFalse);
    }

    if(m_value == INVALID_BOOL)
    {
        v = cjFalse;
        return cjFalse;
    }

    v = (m_value != cjFalse);

    m_lastTime = etime;

    return cjTrue;
}

void cjBoolInterpolator::Cache(cjFLOAT stTime, cjFLOAT enTime)
{
    if(m_spBoolDat)
        m_spBoolDat->CacheKeys(stTime, enTime);
}

cjInterpolator* cjBoolInterpolator::GetInterpolator(cjFLOAT stTime, cjFLOAT enTime)
{
    cjBoolInterpolator* interp = (cjBoolInterpolator*)cjKeyBasedInterpolator::GetInterpolator(stTime, enTime);

    if(m_spBoolDat)
    {
        cjAniBoolDatRPtr spNewBoolDat = m_spBoolDat->GetDat(stTime, enTime);

        interp->SetBoolData(spNewBoolDat);
        interp->m_lastIndex = 0;
    }

    return interp;
}

cjUINT32 cjBoolInterpolator::GetKeyCount(cjUINT32 ) const
{
    if(!m_spBoolDat)
        return 0;
    else
        return m_spBoolDat->GetKeyCount();
}

cjAniKey::EKEY_TYPE cjBoolInterpolator::GetKeyType(cjUINT32 ) const
{
    if(!m_spBoolDat)
        return cjAniKey::EKT_NOINTERP;

    cjUINT32 keyCount; cjAniKeyBool::EKEY_TYPE keyType; cjBYTE keySiz;
    m_spBoolDat->GetKeys(keyCount, keyType, keySiz);

    return keyType; 
}

cjAniKey* cjBoolInterpolator::GetKeyAry(cjUINT32 ) const 
{
    if(!m_spBoolDat)
        return cjNULL;

    cjUINT32 keyCount; cjAniKeyBool::EKEY_TYPE keyType; cjBYTE keySiz;
    cjAniKeyBool* keys = m_spBoolDat->GetKeys(keyCount, keyType, keySiz);

    return keys; 
}

cjBYTE cjBoolInterpolator::GetKeySiz(cjUINT32 ) const 
{
    if(!m_spBoolDat)
        return 0;

    cjUINT32 keyCount; cjAniKeyBool::EKEY_TYPE keyType; cjBYTE keySiz;
    m_spBoolDat->GetKeys(keyCount, keyType, keySiz);

    return keySiz; 
}

cjBool cjBoolInterpolator::GetChannelPosed(cjUINT32 ) const
{
    if(m_spBoolDat)
        return cjFalse;

    if(m_value == cjFalse)
        return cjFalse;

    return cjTrue;
}

void cjBoolInterpolator::Collapse()
{
    if(m_spBoolDat)
    {
        cjUINT32 keyCount; cjAniKey::EKEY_TYPE keyType; cjBYTE keySiz;
        cjAniKeyBool* keys = m_spBoolDat->GetKeys(keyCount, keyType, keySiz);

        if(keyCount == 0)
        {
            m_spBoolDat = cjNULL;
            m_value = cjFalse;
        }
        else
        {
            cjAniKey::PFIsPosed pfnIsPosed = cjAniKeyBool::GetIsPosedKeyPF(keyType);
            cjAssert(pfnIsPosed);

            if(pfnIsPosed(keys, keyCount))
            {
                m_value = keys->GetKey(0, keySiz)->GetValue();
                m_spBoolDat = cjNULL;
            }
        }
    }
}

void cjBoolInterpolator::SetPoseValue(cjBool value)
{
    m_spBoolDat = cjNULL;
    m_value = value;
}

cjAniKeyBool* cjBoolInterpolator::GetKeys(cjUINT32& keyCount, cjAniKeyBool::EKEY_TYPE& keyType, cjBYTE& keySiz) const
{
	if(m_spBoolDat)
		return m_spBoolDat->GetKeys(keyCount, keyType, keySiz);

	keyCount = 0; keyType = cjAniKeyBool::EKT_NOINTERP; keySiz = 0;

	return cjNULL;
}

void cjBoolInterpolator::ReplaceKeys(cjAniKeyBool* keys, cjUINT32 keyCount, cjAniKeyBool::EKEY_TYPE keyType)
{
	if(keyCount > 0)
	{
		if(!m_spBoolDat)
			m_spBoolDat = new cjAniBoolDat;

		m_spBoolDat->ReplaceKeys(keys, keyCount, keyType);
		m_lastIndex = 0;
	}
	else if(m_spBoolDat)
	{
		m_spBoolDat->ReplaceKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
	}
}

void cjBoolInterpolator::SetKeys(cjAniKeyBool* keys, cjUINT32 keyCount, cjAniKeyBool::EKEY_TYPE keyType)
{
	if(keyCount > 0)
	{
		if(!m_spBoolDat)
			m_spBoolDat = new cjAniBoolDat;

		m_spBoolDat->SetKeys(keys, keyCount, keyType);
		m_lastIndex = 0;
	}
	else if(m_spBoolDat)
	{
		m_spBoolDat->SetKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
	}
}

//
// cjBoolTimelineInterpolator - cjBoolInterpolator
//

CJSER_IMPL_CLASS(cjBoolTimelineInterpolator, cjBoolInterpolator);

CJSER_IMPL_SERIALIZE_BEGIN(cjBoolTimelineInterpolator)

	if(ser.IsQuerySerObjAndStr())
	{
		return;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		// m_prevIdx, m_prevValue is not serialized
	}

CJSER_IMPL_SERIALIZE_END(cjBoolTimelineInterpolator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjBoolTimelineInterpolator)
CJSER_IMPL_CLONE_PROPCOPY_END(cjBoolTimelineInterpolator)

CJSER_IMPL_CLONE(cjBoolTimelineInterpolator)


cjBoolTimelineInterpolator::cjBoolTimelineInterpolator()
{
	m_prevIdx = 0;
	m_prevValue = cjFalse;
}

cjBoolTimelineInterpolator::cjBoolTimelineInterpolator(cjAniBoolDat* dat)
	: cjBoolInterpolator(dat)
{
    m_prevIdx = 0;
    m_prevValue = cjFalse;
}

cjBoolTimelineInterpolator::cjBoolTimelineInterpolator(cjBool poseValue) :
    cjBoolInterpolator(poseValue)
{
    m_prevIdx = 0;
    m_prevValue = cjFalse;
}

cjBoolTimelineInterpolator::~cjBoolTimelineInterpolator()
{
}


cjBool cjBoolTimelineInterpolator::Update(cjFLOAT etime, cjRefObj*, cjBool& value)
{
    if(!TimeHasChanged(etime))
    {
        if(m_value == INVALID_BOOL)
        {
            value = cjFalse;
            return cjFalse;
        }

        value = (m_value != 0);
        return cjTrue;
    }

    cjUINT32 keyCount; cjBYTE keySiz; cjAniKey::EKEY_TYPE keyType;

    cjAniKeyBool* keys = GetKeys(keyCount, keyType, keySiz);
    if(keyCount > 0)
    {
        m_prevIdx = m_lastIndex;
        cjBool interpValue = cjAniKeyBool::GenInterp(etime, keys, keyType, keyCount, m_lastIndex, keySiz);

        cjUINT32 curIndex = m_lastIndex;
        cjBool boolValue = interpValue;

        if(boolValue == m_prevValue)
        {
            cjBool valueToFind = !boolValue;

            if(m_prevIdx != curIndex && (m_prevIdx+1) > curIndex)
            {
                for(cjUINT32 i=m_prevIdx+1; i<keyCount; i++)
                {
                    if(keys->GetKey(i, keySiz)->GetValue() == valueToFind)
                    {
                        boolValue = valueToFind;
                        break;
                    }
                }

                if(boolValue != valueToFind)
                {
                    for(cjUINT32 i=0; i<curIndex; i++)
                    {
                        if(keys->GetKey(i, keySiz)->GetValue() == valueToFind)
                        {
                            boolValue = valueToFind;
                            break;
                        }
                    }
                }
            }
            else
            {
                for(cjUINT32 i=m_prevIdx+1; i<curIndex && i<keyCount; i++)
                {
                    if(keys->GetKey(i, keySiz)->GetValue() == valueToFind)
                    {
                        boolValue = valueToFind;
                        break;
                    }
                }
            }
        }

        m_value = (boolValue != 0);
        m_prevValue = interpValue;
    }
    
    if(m_value == INVALID_BOOL)
    {
        value = cjFalse;
        return cjFalse;
    }

    value = (m_value != 0);
    m_lastTime = etime;

    return cjTrue;
}


//
// cjKeyBasedInterpolator
// - cjVec3SInterpolator
//

CJSER_IMPL_CLASS(cjVec3SInterpolator, cjKeyBasedInterpolator)

CJSER_IMPL_SERIALIZE_BEGIN(cjVec3SInterpolator)

	if(ser.IsQuerySerObjAndStr())
	{
		if(m_spVec3Dat)
			*(m_spVec3Dat) >> ser;

		return;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		cjSerID serID = cjSerID_Invalid;
		ser << m_value;

		if(ser.IsSaving())
		{
			serID = ser.GetSerObjID(m_spVec3Dat);
			cjSerID_Serialize(ser, serID);
		}
		else if(ser.IsLoading())
		{
			cjSerID_Serialize(ser, serID);
			m_spVec3Dat = ser.GetSerObj<cjAniVec3Dat>(serID);
		}
		
		ser << m_lastIndex;
	}
	
CJSER_IMPL_SERIALIZE_END(cjVec3SInterpolator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjVec3SInterpolator)
	d->m_value = m_value;
	d->m_spVec3Dat = m_spVec3Dat;
	d->m_lastIndex = m_lastIndex;
CJSER_IMPL_CLONE_PROPCOPY_END(cjVec3SInterpolator)

CJSER_IMPL_CLONE(cjVec3SInterpolator)


cjVec3SInterpolator::cjVec3SInterpolator() 
	: m_value(cjVec3::INVALID), m_spVec3Dat(cjNULL), m_lastIndex(0)
{
}

cjVec3SInterpolator::cjVec3SInterpolator(cjAniVec3Dat* vec3Dat) 
	: m_value(cjVec3::INVALID), m_spVec3Dat(vec3Dat), m_lastIndex(0)
{
}

cjVec3SInterpolator::cjVec3SInterpolator(const cjVec3& poseValue) 
	: m_value(poseValue), m_spVec3Dat(cjNULL), m_lastIndex(0)
{
}

cjVec3SInterpolator::~cjVec3SInterpolator()
{
}

cjBool cjVec3SInterpolator::Update(cjFLOAT etime, cjRefObj*, cjVec3& v)
{
	if(!TimeHasChanged(etime))
	{
		v = m_value;

		if(m_value == cjVec3::INVALID)
			return cjFalse;

		return cjTrue;
	}

	cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE siz;
	cjAniKeyVec3* keys = GetKeys(keyCount, keyType, siz);

	if(keyCount > 0)
		m_value = cjAniKeyVec3::GenInterp(etime, keys, keyType, keyCount, m_lastIndex, siz);

	if(m_value == cjVec3::INVALID)
		return cjFalse;

	v = m_value;
	m_lastTime = etime;

	return cjTrue;
}

void cjVec3SInterpolator::Collapse()
{
	if(m_spVec3Dat)
	{
		cjUINT32 keyCount; cjAniKey::EKEY_TYPE keyType; cjBYTE siz;
		cjAniKeyVec3* keys = m_spVec3Dat->GetKeys(keyCount, keyType, siz);

		if(keyCount == 0)
		{
			m_spVec3Dat = cjNULL;
			m_value = cjVec3::INVALID;
		}
		else
		{
			cjAniKey::PFIsPosed pfIsPosed = cjAniKeyVec3::GetIsPosedKeyPF(keyType);

			if(pfIsPosed(keys, keyCount))
			{
				m_value = keys->GetKey(0, siz)->GetValue();
				m_spVec3Dat = cjNULL;
			}
		}
	}
}

void cjVec3SInterpolator::Cache(cjFLOAT stTime, cjFLOAT enTime)
{
	if(m_spVec3Dat)
		m_spVec3Dat->CacheKeys(stTime, enTime);
}

cjInterpolator* cjVec3SInterpolator::GetInterpolator(cjFLOAT stTime, cjFLOAT enTime)
{
	cjVec3SInterpolator* seqInterp 
		= (cjVec3SInterpolator*)cjKeyBasedInterpolator::GetInterpolator(stTime, enTime);

	if(m_spVec3Dat)
	{
		cjAniVec3DatRPtr spNewVec3Dat = m_spVec3Dat->GetDat(stTime, enTime);

		seqInterp->SetVec3Dat(spNewVec3Dat);
		seqInterp->m_lastIndex = 0;
	}

	return seqInterp;
}

cjUINT32 cjVec3SInterpolator::GetKeyCount(cjUINT32) const 
{
	if(!m_spVec3Dat)
		return 0;
	else
		return m_spVec3Dat->GetKeyCount();
}

cjAniKey::EKEY_TYPE cjVec3SInterpolator::GetKeyType(cjUINT32) const
{
	if(!m_spVec3Dat)
		return cjAniKey::EKT_NOINTERP;

	cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE siz;
	m_spVec3Dat->GetKeys(keyCount, keyType, siz);

	return keyType; 
}

cjAniKey* cjVec3SInterpolator::GetKeyAry(cjUINT32) const
{
	if(!m_spVec3Dat)
		return cjNULL;

	cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE siz;
	cjAniKeyVec3* keys = m_spVec3Dat->GetKeys(keyCount, keyType, siz);

	return keys; 
}

cjBYTE cjVec3SInterpolator::GetKeySiz(cjUINT32) const
{
	if(!m_spVec3Dat)
		return 0x00;

	cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE siz;
	m_spVec3Dat->GetKeys(keyCount, keyType, siz);

	return siz; 
}

cjBool cjVec3SInterpolator::GetChannelPosed(cjUINT32) const
{
	if(m_spVec3Dat)
		return cjFalse;

	if(m_value == cjVec3::INVALID)
		return cjFalse;

	return cjTrue;
}

cjAniKeyVec3* cjVec3SInterpolator::GetKeys(cjUINT32& keyCount, cjAniKeyVec3::EKEY_TYPE& keyType, cjBYTE& keySiz) const
{
	if(m_spVec3Dat)
		return m_spVec3Dat->GetKeys(keyCount, keyType, keySiz);

	keyCount = 0;
	keyType = cjAniKeyVec3::EKT_NOINTERP;
	keySiz = 0;

	return cjNULL;
}

void cjVec3SInterpolator::ReplaceKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType)
{
	if(keyCount > 0)
	{
		if(!m_spVec3Dat)
			m_spVec3Dat = new cjAniVec3Dat;

		m_spVec3Dat->ReplaceKeys(keys, keyCount, keyType);
		m_lastIndex = 0;
	}
	else if(m_spVec3Dat)
	{
		m_spVec3Dat->ReplaceKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
	}
}

void cjVec3SInterpolator::SetKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType)
{
	if(keyCount > 0)
	{
		if(!m_spVec3Dat)
			m_spVec3Dat = new cjAniVec3Dat;

		m_spVec3Dat->SetKeys(keys, keyCount, keyType);
		m_lastIndex = 0;
	}
	else if(m_spVec3Dat)
	{
		m_spVec3Dat->SetKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
	}
}


//
// cjKeyBasedInterpolator
// - cjVec3PInterpolator
//

CJSER_IMPL_CLASS(cjVec3PInterpolator, cjKeyBasedInterpolator)

CJSER_IMPL_SERIALIZE_BEGIN(cjVec3PInterpolator)

	if(ser.IsQuerySerObjAndStr())
	{
		if(m_spVec3Dat)
			*(m_spVec3Dat) >> ser;

		return;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		cjSerID serID = cjSerID_Invalid;
		ser << m_value;

		if(ser.IsSaving())
		{
			serID = ser.GetSerObjID(m_spVec3Dat);
			cjSerID_Serialize(ser, serID);
		}
		else if(ser.IsLoading())
		{
			cjSerID_Serialize(ser, serID);
			m_spVec3Dat = ser.GetSerObj<cjAniVec3Dat>(serID);
		}
		
		ser << m_lastIndex;
	}
	
CJSER_IMPL_SERIALIZE_END(cjVec3PInterpolator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjVec3PInterpolator)
	d->m_value = m_value;
	d->m_spVec3Dat = m_spVec3Dat;
	d->m_lastIndex = m_lastIndex;
CJSER_IMPL_CLONE_PROPCOPY_END(cjVec3PInterpolator)

CJSER_IMPL_CLONE(cjVec3PInterpolator)


cjVec3PInterpolator::cjVec3PInterpolator() 
	: m_value(cjVec3::INVALID), m_spVec3Dat(cjNULL), m_lastIndex(0)
{
}

cjVec3PInterpolator::cjVec3PInterpolator(cjAniVec3Dat* vec3Dat) 
	: m_value(cjVec3::INVALID), m_spVec3Dat(vec3Dat), m_lastIndex(0)
{
}

cjVec3PInterpolator::cjVec3PInterpolator(const cjVec3& poseValue) 
	: m_value(poseValue), m_spVec3Dat(cjNULL), m_lastIndex(0)
{
}

cjVec3PInterpolator::~cjVec3PInterpolator()
{
}

cjBool cjVec3PInterpolator::Update(cjFLOAT etime, cjRefObj*, cjVec3& v)
{
	if(!TimeHasChanged(etime))
	{
		v = m_value;

		if(m_value == cjVec3::INVALID)
			return cjFalse;

		return cjTrue;
	}

	cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE siz;
	cjAniKeyVec3* keys = GetKeys(keyCount, keyType, siz);

	if(keyCount > 0)
		m_value = cjAniKeyVec3::GenInterp(etime, keys, keyType, keyCount, m_lastIndex, siz);

	if(m_value == cjVec3::INVALID)
		return cjFalse;

	v = m_value;
	m_lastTime = etime;

	return cjTrue;
}

void cjVec3PInterpolator::Collapse()
{
	if(m_spVec3Dat)
	{
		cjUINT32 keyCount; cjAniKey::EKEY_TYPE keyType; cjBYTE siz;
		cjAniKeyVec3* keys = m_spVec3Dat->GetKeys(keyCount, keyType, siz);

		if(keyCount == 0)
		{
			m_spVec3Dat = cjNULL;
			m_value = cjVec3::INVALID;
		}
		else
		{
			cjAniKey::PFIsPosed pfIsPosed = cjAniKeyVec3::GetIsPosedKeyPF(keyType);

			if(pfIsPosed(keys, keyCount))
			{
				m_value = keys->GetKey(0, siz)->GetValue();
				m_spVec3Dat = cjNULL;
			}
		}
	}
}

void cjVec3PInterpolator::Cache(cjFLOAT stTime, cjFLOAT enTime)
{
	if(m_spVec3Dat)
		m_spVec3Dat->CacheKeys(stTime, enTime);
}

cjInterpolator* cjVec3PInterpolator::GetInterpolator(cjFLOAT stTime, cjFLOAT enTime)
{
	cjVec3PInterpolator* seqInterp 
		= (cjVec3PInterpolator*)cjKeyBasedInterpolator::GetInterpolator(stTime, enTime);

	if(m_spVec3Dat)
	{
		cjAniVec3DatRPtr spNewVec3Dat = m_spVec3Dat->GetDat(stTime, enTime);

		seqInterp->SetVec3Dat(spNewVec3Dat);
		seqInterp->m_lastIndex = 0;
	}

	return seqInterp;
}

cjUINT32 cjVec3PInterpolator::GetKeyCount(cjUINT32) const 
{
	if(!m_spVec3Dat)
		return 0;
	else
		return m_spVec3Dat->GetKeyCount();
}

cjAniKey::EKEY_TYPE cjVec3PInterpolator::GetKeyType(cjUINT32) const
{
	if(!m_spVec3Dat)
		return cjAniKey::EKT_NOINTERP;

	cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE siz;
	m_spVec3Dat->GetKeys(keyCount, keyType, siz);

	return keyType; 
}

cjAniKey* cjVec3PInterpolator::GetKeyAry(cjUINT32) const
{
	if(!m_spVec3Dat)
		return cjNULL;

	cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE siz;
	cjAniKeyVec3* keys = m_spVec3Dat->GetKeys(keyCount, keyType, siz);

	return keys; 
}

cjBYTE cjVec3PInterpolator::GetKeySiz(cjUINT32) const
{
	if(!m_spVec3Dat)
		return 0x00;

	cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE siz;
	m_spVec3Dat->GetKeys(keyCount, keyType, siz);

	return siz; 
}

cjBool cjVec3PInterpolator::GetChannelPosed(cjUINT32) const
{
	if(m_spVec3Dat)
		return cjFalse;

	if(m_value == cjVec3::INVALID)
		return cjFalse;

	return cjTrue;
}

cjAniKeyVec3* cjVec3PInterpolator::GetKeys(cjUINT32& keyCount, cjAniKeyVec3::EKEY_TYPE& keyType, cjBYTE& keySiz) const
{
	if(m_spVec3Dat)
		return m_spVec3Dat->GetKeys(keyCount, keyType, keySiz);

	keyCount = 0;
	keyType = cjAniKeyVec3::EKT_NOINTERP;
	keySiz = 0;

	return cjNULL;
}

void cjVec3PInterpolator::ReplaceKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType)
{
	if(keyCount > 0)
	{
		if(!m_spVec3Dat)
			m_spVec3Dat = new cjAniVec3Dat;

		m_spVec3Dat->ReplaceKeys(keys, keyCount, keyType);
		m_lastIndex = 0;
	}
	else if(m_spVec3Dat)
	{
		m_spVec3Dat->ReplaceKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
	}
}

void cjVec3PInterpolator::SetKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType)
{
	if(keyCount > 0)
	{
		if(!m_spVec3Dat)
			m_spVec3Dat = new cjAniVec3Dat;

		m_spVec3Dat->SetKeys(keys, keyCount, keyType);
		m_lastIndex = 0;
	}
	else if(m_spVec3Dat)
	{
		m_spVec3Dat->SetKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
	}
}


//
// cjKeyBasedInterpolator
// - cjQuatInterpolator
//

CJSER_IMPL_CLASS(cjQuatInterpolator, cjKeyBasedInterpolator)

CJSER_IMPL_SERIALIZE_BEGIN(cjQuatInterpolator)
	
	if(ser.IsQuerySerObjAndStr())
	{
		if(m_spRotDat)
			*(m_spRotDat) >> ser;

		return;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		cjSerID serID;
		ser << m_value;

		if(ser.IsSaving())
		{
			serID = ser.GetSerObjID(m_spRotDat);
			cjSerID_Serialize(ser, serID);
		}
		else if(ser.IsLoading())
		{
			cjSerID_Serialize(ser, serID);
			m_spRotDat = ser.GetSerObj<cjAniRotDat>(serID);
		}
		
		ser << m_lastIndex;
	}

CJSER_IMPL_SERIALIZE_END(cjQuatInterpolator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjQuatInterpolator)

	d->m_value = m_value;
	d->m_spRotDat = m_spRotDat;
	d->m_lastIndex = m_lastIndex;

CJSER_IMPL_CLONE_PROPCOPY_END(cjQuatInterpolator)

CJSER_IMPL_CLONE(cjQuatInterpolator)


cjQuatInterpolator::cjQuatInterpolator() 
	: m_value(cjQuat::INVALID), m_spRotDat(cjNULL), m_lastIndex(0)	
{
}

cjQuatInterpolator::cjQuatInterpolator(cjAniRotDat* rotDat)
	: m_value(cjQuat::INVALID), m_spRotDat(rotDat), m_lastIndex(0)	
{
}

cjQuatInterpolator::cjQuatInterpolator(const cjQuat& poseVal) 
	: m_value(poseVal), m_spRotDat(cjNULL), m_lastIndex(0)			
{
}

cjQuatInterpolator::~cjQuatInterpolator()
{
}

cjBool cjQuatInterpolator::Update(cjFLOAT etime, cjRefObj* target, cjQuat& v)
{
    if(!TimeHasChanged(etime))
    {
        v = m_value;

        if(m_value == cjQuat::INVALID)
            return cjFalse;

        return cjTrue;
    }

    cjUINT32 keyCount; cjAniKeyRot::EKEY_TYPE keyType; cjBYTE siz;
    cjAniKeyRot* keys = GetKeys(keyCount, keyType,siz);

    if(keyCount > 0)
        cjAniKeyRot::GenInterp(etime, keys, keyType, keyCount, m_lastIndex, siz, &m_value);
    
    if(m_value == cjQuat::INVALID)
        return cjFalse;

    v = m_value;
    m_lastTime = etime;

    return cjTrue;
}

void cjQuatInterpolator::Collapse()
{
    if(m_spRotDat)
    {
        cjUINT32 keyCount; cjAniKey::EKEY_TYPE keyType; cjBYTE siz;
        cjAniKeyRot* keys = m_spRotDat->GetKeys(keyCount, keyType, siz);

        if(keyCount == 0)
        {
            m_spRotDat = cjNULL;
            m_value = cjQuat::INVALID;
        }
        else
        {
			cjAniKey::PFIsPosed pfIsPosed = cjAniKeyRot::GetIsPosedKeyPF(keyType);

            if(pfIsPosed(keys, keyCount))
            {
				if(keyType == cjAniKeyRot::EKT_EULERKEY)
                {
					cjAniKeyRot::PFInterpolation pfInterp = cjAniKeyRot::GetInterpKeyPF(keyType);
                    pfInterp(0.0f, keys->GetKey(0, siz), 0, &m_value);
                }
                else
                {
					m_value = keys->GetKey(0, siz)->GetValue();
                }

                m_spRotDat = cjNULL;
            }
        }
    }
}

void cjQuatInterpolator::GetValidTimeRange(cjFLOAT& stTime, cjFLOAT& enTime) const
{
    cjUINT32 keyCount; cjAniKeyRot::EKEY_TYPE keyType; cjBYTE siz;
    cjAniKeyRot* keys = GetKeys(keyCount, keyType, siz);
    cjBool existKey = cjFalse;

    if(keyCount > 0)
    {
		if(keyType == cjAniKeyRot::EKT_EULERKEY)
        {
            cjAniKeyEulerRot* rotKeys = (cjAniKeyEulerRot*)keys->GetKey(0, siz);

            cjFLOAT tmpStKeyTime = CJ_FLT_MAX;
            cjFLOAT tmpEnKeyTime = CJ_FLT_INVALID;

            for(cjBYTE c=0; c<3; c++)
            {
                cjUINT32 floatKeyCount = rotKeys->GetKeyCount(c);
                cjBYTE floatKeySiz = rotKeys->GetKeySiz(c);

                if(floatKeyCount > 0)
                {
                    cjAniKeyFloat* floatKeys = rotKeys->GetKeys(c);
                    cjFLOAT keyTime = floatKeys->GetKey(0, floatKeySiz)->GetTime();

                    if(keyTime < tmpStKeyTime)
                        tmpStKeyTime = keyTime;

					keyTime = floatKeys->GetKey(floatKeyCount - 1, floatKeySiz)->GetTime();

                    if(keyTime > tmpEnKeyTime)
                        tmpEnKeyTime = keyTime;

					existKey = cjTrue;
                }
            }

            if(existKey)
            {
                stTime = tmpStKeyTime;
                enTime = tmpEnKeyTime;
            }
        }
        else
        {
            stTime = keys->GetKey(0, siz)->GetTime();
            enTime = keys->GetKey(keyCount - 1, siz)->GetTime();
            existKey = cjTrue;
        }
    }

    if(!existKey)
    {
        stTime = 0.0f;
        enTime = 0.0f;
    }
}

void cjQuatInterpolator::Cache(cjFLOAT stTime, cjFLOAT enTime)
{
    if(m_spRotDat)
        m_spRotDat->CacheKeys(stTime, enTime);
}

cjInterpolator* cjQuatInterpolator::GetInterpolator(cjFLOAT stTime, cjFLOAT enTime)
{
    cjQuatInterpolator* interp = (cjQuatInterpolator*)cjKeyBasedInterpolator::GetInterpolator(stTime, enTime);

    if(m_spRotDat)
    {
        cjAniRotDatRPtr spNewRotDat = m_spRotDat->GetDat(stTime, enTime);

        interp->SetRotDat(spNewRotDat);
        interp->m_lastIndex = 0;
    }

    return interp;
}

cjUINT32 cjQuatInterpolator::GetKeyCount(cjUINT32) const 
{ 
	if(!m_spRotDat) 
		return 0;
	else
		return m_spRotDat->GetKeyCount(); 
}

cjAniKey::EKEY_TYPE cjQuatInterpolator::GetKeyType(cjUINT32) const
{
    if(!m_spRotDat)
        return cjAniKey::EKT_NOINTERP;

    cjUINT32 keyCount; cjAniKeyRot::EKEY_TYPE keyType; cjBYTE siz;
    m_spRotDat->GetKeys(keyCount, keyType, siz);

    return keyType; 
}

cjAniKey* cjQuatInterpolator::GetKeyAry(cjUINT32) const
{
    if(!m_spRotDat)
        return cjNULL;

    cjUINT32 keyCount; cjAniKeyRot::EKEY_TYPE keyType; cjBYTE siz;
    cjAniKeyRot* keys = m_spRotDat->GetKeys(keyCount, keyType, siz);

    return keys; 
}

cjBYTE cjQuatInterpolator::GetKeySiz(cjUINT32) const
{
    if(!m_spRotDat)
        return 0;

    cjUINT32 keyCount; cjAniKeyRot::EKEY_TYPE keyType; cjBYTE siz;
    m_spRotDat->GetKeys(keyCount, keyType, siz);

    return siz; 
}

cjBool cjQuatInterpolator::GetChannelPosed(cjUINT32) const
{
    if(m_spRotDat)
        return cjFalse;

    if(m_value == cjQuat::INVALID)
        return cjFalse;

    return cjTrue;
}

void cjQuatInterpolator::SetPoseValue(const cjQuat& poseValue)
{
    m_spRotDat = cjNULL;
    m_value = poseValue;
}

cjAniKeyRot* cjQuatInterpolator::GetKeys(cjUINT32& keyCount, cjAniKeyRot::EKEY_TYPE& keyType, cjBYTE& siz) const
{
	if(m_spRotDat)
		return m_spRotDat->GetKeys(keyCount, keyType, siz);

	keyCount = 0; keyType = cjAniKeyRot::EKT_NOINTERP; siz = 0;

	return cjNULL;
}

void cjQuatInterpolator::ReplaceKeys(cjAniKeyRot* keys, cjUINT32 keyCount, cjAniKeyRot::EKEY_TYPE keyType)
{
	if(keyCount > 0)
	{
		if(!m_spRotDat)
			m_spRotDat = new cjAniRotDat;

		m_spRotDat->ReplaceKeys(keys, keyCount, keyType);
		m_lastIndex = 0;
	}
	else if(m_spRotDat)
	{
		m_spRotDat->ReplaceKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
	}
}

void cjQuatInterpolator::SetKeys(cjAniKeyRot* keys, cjUINT32 keyCount, cjAniKeyRot::EKEY_TYPE keyType)
{
	if(keyCount > 0)
	{
		if(!m_spRotDat)
			m_spRotDat = new cjAniRotDat;

		m_spRotDat->SetKeys(keys, keyCount, keyType);
		m_lastIndex = 0;
	}
	else if(m_spRotDat)
	{
		m_spRotDat->SetKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
	}
}


//
// cjKeyBasedInterpolator
// - cjTMInterpolator
//

CJSER_IMPL_CLASS(cjTMInterpolator, cjKeyBasedInterpolator)

CJSER_IMPL_SERIALIZE_BEGIN(cjTMInterpolator)
	
	if(ser.IsQuerySerObjAndStr())
	{
		if(m_spAniTMDat)
			(*m_spAniTMDat) >> ser;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		cjSerID serID;

		ser << m_value;

		if(ser.IsSaving())
			serID = ser.GetSerObjID(m_spAniTMDat);

		cjSerID_Serialize(ser, serID);

		if(ser.IsLoading())
			m_spAniTMDat = ser.GetSerObj<cjAniTMDat>(serID);

		ser << m_lastTransIdx;
		ser << m_lastRotIdx;
		ser << m_lastScaleIdx;
	}

CJSER_IMPL_SERIALIZE_END(cjTMInterpolator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjTMInterpolator)
	d->m_value = m_value;
	d->m_spAniTMDat = m_spAniTMDat;
	d->m_lastTransIdx = m_lastTransIdx;
	d->m_lastRotIdx = m_lastRotIdx;
	d->m_lastScaleIdx = m_lastScaleIdx;
CJSER_IMPL_CLONE_PROPCOPY_END(cjTMInterpolator)

CJSER_IMPL_CLONE(cjTMInterpolator)


cjTMInterpolator::cjTMInterpolator()
{
	m_spAniTMDat		= cjNULL;
	m_lastTransIdx		= 0;
	m_lastRotIdx		= 0;
	m_lastScaleIdx		= 0;
}

cjTMInterpolator::~cjTMInterpolator()
{
}

cjTMInterpolator::cjTMInterpolator(cjAniTMDat* aniTMDat)
{
	m_spAniTMDat		= aniTMDat;
	m_lastTransIdx		= 0;
	m_lastRotIdx		= 0;
	m_lastScaleIdx		= 0;
}

cjTMInterpolator::cjTMInterpolator(const cjQuatTM& poseValue)
{
    m_value				= poseValue;
	m_spAniTMDat		= cjNULL; 
    m_lastTransIdx		= 0;
	m_lastRotIdx		= 0;
	m_lastScaleIdx		= 0;
}

void cjTMInterpolator::SetAniTMDat(cjAniTMDat* aniTMDat)
{
    m_spAniTMDat		= aniTMDat;
    m_lastRotIdx		= 0;
    m_lastScaleIdx		= 0;
    m_lastTransIdx		= 0;
}

cjAniKeyVec3* cjTMInterpolator::GetPosKeys(cjUINT32& keyCount, 
	cjAniKeyVec3::EKEY_TYPE& keyType, cjBYTE& keySiz) const
{
    if(m_spAniTMDat)
        return m_spAniTMDat->GetPosKeys(keyCount, keyType, keySiz);

    keyCount = 0;
    keyType = cjAniKeyVec3::EKT_NOINTERP;
    keySiz = 0;

    return cjNULL;
}

void cjTMInterpolator::ReplacePosKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, 
	cjAniKeyVec3::EKEY_TYPE keyType)
{
    if(keyCount > 0)
    {
        if(!m_spAniTMDat)
            m_spAniTMDat = cjAniTMDat::NewInst();

		m_spAniTMDat->ReplacePosKeys(keys, keyCount, keyType);
        m_lastTransIdx = 0;
    }
    else if(m_spAniTMDat)
    {		
		m_spAniTMDat->ReplacePosKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP); // clean
    }
}

void cjTMInterpolator::SetPosePos(const cjVec3& pos)
{
    m_value.SetPos(pos);

    if(m_spAniTMDat)
        m_spAniTMDat->ReplacePosKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
}

cjAniKeyRot* cjTMInterpolator::GetRotKeys(cjUINT32& keyCount, cjAniKeyRot::EKEY_TYPE& keyType, 
	cjBYTE& keySiz) const
{
    if(m_spAniTMDat)
        return m_spAniTMDat->GetRotKeys(keyCount, keyType, keySiz);

    keyCount = 0;
    keyType = cjAniKeyRot::EKT_NOINTERP;
    keySiz = 0;

	return cjNULL;
}

void cjTMInterpolator::ReplaceRotKeys(cjAniKeyRot* keys, cjUINT32 keyCount, 
	cjAniKeyRot::EKEY_TYPE keyType)
{
    if(keyCount > 0)
    {
        if(!m_spAniTMDat)
            m_spAniTMDat = cjAniTMDat::NewInst();

        m_spAniTMDat->ReplaceRotKeys(keys, keyCount, keyType);
        m_lastRotIdx = 0;
    }
    else if(m_spAniTMDat)
    {
         m_spAniTMDat->ReplaceRotKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
    }
}

void cjTMInterpolator::SetPoseRotate(const cjQuat& rot)
{
    m_value.SetRotate(rot);

	if(m_spAniTMDat)
        m_spAniTMDat->ReplaceRotKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
}

cjAniKeyVec3* cjTMInterpolator::GetScaleKeys(cjUINT32& keyCount, cjAniKeyVec3::EKEY_TYPE& keyType, 
	cjBYTE& keySiz) const
{
    if(m_spAniTMDat)
        return m_spAniTMDat->GetScaleKeys(keyCount, keyType, keySiz);

    keyCount = 0;
    keyType = cjAniKeyVec3::EKT_NOINTERP;
    keySiz = 0;

    return cjNULL;
}

void cjTMInterpolator::ReplaceScaleKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, 
	cjAniKeyFloat::EKEY_TYPE keyType)
{
    if(keyCount > 0)
    {
        if(!m_spAniTMDat)
            m_spAniTMDat = cjAniTMDat::NewInst();

        m_spAniTMDat->ReplaceScaleKeys(keys, keyCount, keyType);
        m_lastScaleIdx = 0;
    }
    else if(m_spAniTMDat)
    {
		// key count is zero, so clear
		m_spAniTMDat->ReplaceScaleKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
    }
}

void cjTMInterpolator::SetPoseScale(const cjVec3& poseScale)
{
    m_value.SetScale(poseScale);

	if(m_spAniTMDat)
        m_spAniTMDat->ReplaceScaleKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
}

void cjTMInterpolator::SetPoseValue(const cjQuatTM& poseValue)
{
    if(poseValue.IsScaleValid())
        SetPoseScale(poseValue.GetScale());

	if(poseValue.IsRotateValid())
        SetPoseRotate(poseValue.GetRotate());

    if(poseValue.IsPosValid())
        SetPosePos(poseValue.GetPos());
}

cjUINT32 cjTMInterpolator::GetKeyCount(cjUINT32 channel) const
{
    if(channel == cjAniChan_Pos)
    {
        if(!m_spAniTMDat)
            return 0;
        else
            return m_spAniTMDat->GetPosKeyCount();
    }
    else if(channel == cjAniChan_Rot)
    {
        if(!m_spAniTMDat)
            return 0;
        else
            return m_spAniTMDat->GetRotKeyCount();
    }
    else if(channel == cjAniChan_Scale)
    {
        if(!m_spAniTMDat)
            return 0;
        else
            return m_spAniTMDat->GetScaleKeyCount();
    }

	cjAssert(channel < 3);

	return 0;
}

cjAniKey::EKEY_TYPE cjTMInterpolator::GetKeyType(cjUINT32 channel) const
{
    if(channel == cjAniChan_Pos)
    {
        if(!m_spAniTMDat)
            return cjAniKey::EKT_NOINTERP;

        cjUINT32 keyCount;
        cjAniKeyVec3::EKEY_TYPE keyType;
        cjBYTE keySiz;

        m_spAniTMDat->GetPosKeys(keyCount, keyType, keySiz);

        return keyType; 
    }
    else if(channel == cjAniChan_Rot)
    {
        if(!m_spAniTMDat)
            return cjAniKey::EKT_NOINTERP;

        cjUINT32 keyCount;
        cjAniKeyRot::EKEY_TYPE keyType;
        cjBYTE keySiz;

        m_spAniTMDat->GetRotKeys(keyCount, keyType, keySiz);

        return keyType;     
    }
    else if(channel == cjAniChan_Scale)
    {
        if(!m_spAniTMDat)
            return cjAniKey::EKT_NOINTERP;

        cjUINT32 keyCount;
        cjAniKeyVec3::EKEY_TYPE keyType;
        cjBYTE keySiz;

        m_spAniTMDat->GetScaleKeys(keyCount, keyType, keySiz);

        return keyType;     
    }

	cjAssert(channel < 3);

	return cjAniKey::EKT_NOINTERP;    
}

cjAniKey::EKEY_VALTYPE cjTMInterpolator::GetKeyValType(cjUINT32 channel) const
{
    if(channel == cjAniChan_Pos)
        return cjAniKey::EKVAL_VEC3;
    else if(channel == cjAniChan_Rot)
        return cjAniKey::EKVAL_ROT;
    else if(channel == cjAniChan_Scale)
        return cjAniKey::EKVAL_VEC3;
    
    cjAssert(channel < 3);

    return cjAniKey::EKVAL_MAX;
}

cjAniKey* cjTMInterpolator::GetKeyAry(cjUINT32 channel) const
{
    if(channel == cjAniChan_Pos)
    {       
        if(!m_spAniTMDat)
            return cjNULL;

        cjUINT32 keyCount;
        cjAniKeyVec3::EKEY_TYPE keyType;
        cjBYTE keySiz;

        cjAniKeyVec3* keys = m_spAniTMDat->GetPosKeys(keyCount, keyType, keySiz);

        return keys; 
    }
    else if(channel == cjAniChan_Rot)
    {
        if(!m_spAniTMDat)
            return cjNULL;
    
        cjUINT32 keyCount;
        cjAniKeyRot::EKEY_TYPE keyType;
        cjBYTE keySiz;
        
		cjAniKeyRot* keys = m_spAniTMDat->GetRotKeys(keyCount, keyType, keySiz);

        return keys;
    }
    else if(channel == cjAniChan_Scale)
    {
        if(!m_spAniTMDat)
            return cjNULL;
    
        cjUINT32 keyCount;
        cjAniKeyVec3::EKEY_TYPE keyType;
        cjBYTE keySiz;

        cjAniKeyVec3* keys = m_spAniTMDat->GetScaleKeys(keyCount, keyType, keySiz);

        return keys;
    }

	cjAssert(channel < 3);

    return cjNULL;
}

cjBYTE cjTMInterpolator::GetKeySiz(cjUINT32 channel) const
{
    if(channel == cjAniChan_Pos)
    {       
        if(!m_spAniTMDat)
            return 0;

        cjUINT32 keyCount;
        cjAniKeyVec3::EKEY_TYPE keyType;
        cjBYTE keySiz;

        m_spAniTMDat->GetPosKeys(keyCount, keyType, keySiz);

        return keySiz; 
    }
    else if(channel == cjAniChan_Rot)
    {
        if(!m_spAniTMDat)
            return 0;
    
        cjUINT32 keyCount;
        cjAniKeyRot::EKEY_TYPE keyType;
        cjBYTE keySiz;

        m_spAniTMDat->GetRotKeys(keyCount, keyType, keySiz);

        return keySiz; 
    }
    else if(channel == cjAniChan_Scale)
    {
        if(!m_spAniTMDat)
            return 0;
    
        cjUINT32 keyCount;
        cjAniKeyVec3::EKEY_TYPE keyType;
        cjBYTE keySiz;

        m_spAniTMDat->GetScaleKeys(keyCount, keyType, keySiz);

        return keySiz; 
    }

	cjAssert(channel < 3);

    return 0;
}

void cjTMInterpolator::GetValidTimeRange(cjFLOAT& stTime, cjFLOAT& enTime) const
{
    stTime = CJ_FTIME_INFINITY;
    enTime = CJ_FTIME_INVALID;
    cjBool foundKeys = cjFalse;

    cjUINT32 keyCount;
    cjAniKeyVec3::EKEY_TYPE posKeyType;
    cjBYTE keySiz;

	// Pos
    cjAniKeyVec3* posKeys = GetPosKeys(keyCount, posKeyType, keySiz);

    if(keyCount > 0)
    {
        cjFLOAT keyTime = posKeys->GetKey(0, keySiz)->GetTime();

		if(keyTime < stTime)
            stTime = keyTime;
        
		keyTime = posKeys->GetKey(keyCount - 1, keySiz)->GetTime();
        
		if(keyTime > enTime)
            enTime = keyTime;

		foundKeys = cjTrue;
    }

	// Rot
    cjAniKeyRot::EKEY_TYPE rotKeyType;
    cjAniKeyRot* rotKeys = GetRotKeys(keyCount, rotKeyType, keySiz);

    if(keyCount > 0)
    {
        if(rotKeyType == cjAniKeyRot::EKT_EULERKEY)
        {
            cjAniKeyEulerRot* eulerKey = (cjAniKeyEulerRot*)rotKeys->GetKey(0, keySiz);

            for(cjBYTE c=0; c<3; c++)
            {
                cjUINT32 floatKeyCount = eulerKey->GetKeyCount(c);

                if(floatKeyCount > 0)
                {
                    cjAniKeyFloat* floatKeys = eulerKey->GetKeys(c);
                    cjBYTE floatKeySiz = eulerKey->GetKeySiz(c);
                    cjFLOAT keyTime	= floatKeys->GetKey(0, floatKeySiz)->GetTime();

                    if(keyTime < stTime)
                        stTime = keyTime;

					keyTime = floatKeys->GetKey(floatKeyCount - 1, floatKeySiz)->GetTime();

                    if(keyTime > enTime)
                        enTime = keyTime;

					foundKeys = cjTrue;
                }
            }
        }
        else
        {
            cjFLOAT keyTime = rotKeys->GetKey(0, keySiz)->GetTime();

            if(keyTime < stTime)
                stTime = keyTime;

			keyTime = rotKeys->GetKey(keyCount - 1, keySiz)->GetTime();

            if(keyTime > enTime)
                enTime = keyTime;

			foundKeys = cjTrue;
        }
    }

	// scale
    cjAniKeyVec3::EKEY_TYPE scaleType;
    cjAniKeyVec3* scaleKeys = GetScaleKeys(keyCount, scaleType, keySiz);

    if(keyCount > 0)
    {
        cjFLOAT keyTime = scaleKeys->GetKey(0, keySiz)->GetTime();

        if(keyTime < stTime)
            stTime = keyTime;

		keyTime = scaleKeys->GetKey(keyCount - 1, keySiz)->GetTime();

        if(keyTime > enTime)
            enTime = keyTime;

		foundKeys = cjTrue;
    }

    if(!foundKeys)
    {
        stTime = 0.0f;
        enTime = 0.0f;
    }
}

void cjTMInterpolator::Cache(cjFLOAT stTime, cjFLOAT enTime)
{
    if(m_spAniTMDat)
        m_spAniTMDat->CacheKeys(stTime, enTime);
}

cjInterpolator* cjTMInterpolator::GetInterpolator(cjFLOAT stTime, cjFLOAT enTime)
{
    cjTMInterpolator* interp = (cjTMInterpolator*)cjKeyBasedInterpolator::GetInterpolator(stTime, enTime);

    if(m_spAniTMDat)
    {
        cjAniTMDatRPtr spNewAniTMDat = m_spAniTMDat->GetDat(stTime, enTime);

        interp->SetAniTMDat(spNewAniTMDat);
        interp->m_lastTransIdx	= 0;
        interp->m_lastRotIdx	= 0;
        interp->m_lastScaleIdx	= 0;
    }

    return interp;
}

cjBool cjTMInterpolator::Update(cjFLOAT etime, cjRefObj*, cjQuatTM& rstVal)
{
    if(!TimeHasChanged(etime))
    {
        rstVal = m_value;
            
        if(m_value.IsTMInvalid())
            return cjFalse;

        return cjTrue;
    }

    cjUINT32 keyCount;
    cjBYTE keySiz;
		
		// pos
    cjAniKeyVec3::EKEY_TYPE posKeyType;
	cjAniKeyVec3* posKeys = GetPosKeys(keyCount, posKeyType, keySiz);
    if(keyCount > 0)
        m_value.SetPos(cjAniKeyVec3::GenInterp(etime, posKeys, posKeyType, keyCount, m_lastTransIdx, keySiz));

		// rot
    cjAniKeyRot::EKEY_TYPE rotKeyType;
    cjAniKeyRot* rotKeys = GetRotKeys(keyCount, rotKeyType, keySiz);
    if(keyCount > 0)
        cjAniKeyRot::GenInterp(etime, rotKeys, rotKeyType, keyCount, m_lastRotIdx, keySiz, m_value.GetRotatePtr());

		// scale
    cjAniKeyVec3::EKEY_TYPE scaleKeyType;
    cjAniKeyVec3* scaleKeys = GetScaleKeys(keyCount, scaleKeyType, keySiz);
    if(keyCount > 0)
        m_value.SetScale(cjAniKeyVec3::GenInterp(etime, scaleKeys, scaleKeyType, keyCount, m_lastScaleIdx, keySiz));
    
    rstVal = m_value;

    if(m_value.IsTMInvalid())
        return cjFalse;

    m_lastTime = etime;

    return cjTrue;
}

cjBool cjTMInterpolator::GetChannelPosed(cjUINT32 channel) const
{
    switch(channel)
    {
    case cjAniChan_Pos:
        if(m_spAniTMDat && GetKeyCount(channel) > 0)
            return cjFalse;
        else 
            return m_value.IsPosValid();

    case cjAniChan_Rot:
        if(m_spAniTMDat && GetKeyCount(channel) > 0)
            return cjFalse;
        else 
            return m_value.IsRotateValid();

    case cjAniChan_Scale:
        if(m_spAniTMDat && GetKeyCount(channel) > 0)
            return cjFalse;
        else 
            return m_value.IsScaleValid();

    default:
        break;
    }

	return cjFalse;
}

void cjTMInterpolator::Collapse()
{
    if(!m_spAniTMDat)
        return;

    cjQuatTM collapsedTM;

    cjUINT32 keyCount;
    cjAniKey::EKEY_TYPE keyType;
    cjBYTE keySiz;

	//
	// pos
	//
    cjAniKeyVec3* posKeys = m_spAniTMDat->GetPosKeys(keyCount, keyType, keySiz);

    if(keyCount == 0)
    {
        m_spAniTMDat->ReplacePosKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
        if(m_value.IsPosValid())
            collapsedTM.SetPos(m_value.GetPos());
    }
    else
    {
        cjAniKey::PFIsPosed pfnIsPosed = cjAniKeyVec3::GetIsPosedKeyPF(keyType);
        cjAssert(pfnIsPosed);

        if(pfnIsPosed(posKeys, keyCount))
        {
            cjVec3 v = posKeys->GetKey(0, keySiz)->GetValue();
            collapsedTM.SetPos(v);

            m_spAniTMDat->ReplacePosKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
        }
        else if(m_value.IsPosValid())
        {
            collapsedTM.SetPos(m_value.GetPos());
        }
    }

    // rot
    cjAniKeyRot* rotKeys = m_spAniTMDat->GetRotKeys(keyCount, keyType, keySiz);

    if(keyCount == 0)
    {
        m_spAniTMDat->ReplaceRotKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);

		if(m_value.IsRotateValid())
            collapsedTM.SetRotate(m_value.GetRotate());
    }
    else
    {
        cjAniKey::PFIsPosed pfnIsPosed = cjAniKeyRot::GetIsPosedKeyPF(keyType);
        cjAssert(pfnIsPosed);

        if(pfnIsPosed(rotKeys, keyCount))
        {
            cjQuat v;

            if(keyType == cjAniKeyRot::EKT_EULERKEY)
            {
                cjAniKeyRot::PFInterpolation interp = cjAniKeyRot::GetInterpKeyPF(keyType);
                cjAssert(interp);
                interp(0.0f, rotKeys->GetKey(0, keySiz), 0, &v);
            }
            else
            {
                v = rotKeys->GetKey(0, keySiz)->GetValue();
            }

            collapsedTM.SetRotate(v);
            m_spAniTMDat->ReplaceRotKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
        }
        else if(m_value.IsRotateValid())
        {
            collapsedTM.SetRotate(m_value.GetRotate());
        }
    }

    // Collapse scale keys.
    cjAniKeyVec3* scaleKeys = m_spAniTMDat->GetScaleKeys(keyCount, keyType, keySiz);

    if(keyCount == 0)
    {
        m_spAniTMDat->ReplaceScaleKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);

        if(m_value.IsScaleValid())
            collapsedTM.SetScale(m_value.GetScale());
    }
    else
    {
        cjAniKey::PFIsPosed pfnIsPosed = cjAniKeyFloat::GetIsPosedKeyPF(keyType);
        cjAssert(pfnIsPosed);

        if(pfnIsPosed(scaleKeys, keyCount))
        {
            cjVec3 v = scaleKeys->GetKey(0, keySiz)->GetValue();
            collapsedTM.SetScale(v);
            m_spAniTMDat->ReplaceScaleKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
        }
        else if(m_value.IsScaleValid())
        {
            collapsedTM.SetScale(m_value.GetScale());
        }
    }

    if(m_spAniTMDat->GetPosKeyCount() == 0 && m_spAniTMDat->GetRotKeyCount() == 0 && 
		m_spAniTMDat->GetScaleKeyCount() == 0)
	{
        m_spAniTMDat = cjNULL;
	}

    m_value = collapsedTM;
}

//
// cjPathInterpolator
//

CJSER_IMPL_CLASS(cjPathInterpolator, cjKeyBasedInterpolator)

CJSER_IMPL_SERIALIZE_BEGIN(cjPathInterpolator)

	if(ser.IsQuerySerObjAndStr())
	{

	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		cjSerID serID;

		ser << m_lastPathIndex;
		ser << m_lastPCTIndex;

		cjFStreamEnum(ser, m_bankDir);

		ser << m_maxBankAngle;
		ser << m_smooth;    
		ser << m_maxCurvature;

		ser << m_rot;
		ser << m_pos;
		ser << m_lastTime;

		if(ser.IsSaving())
			serID = ser.GetSerObjID(m_spPathDat);

		cjSerID_Serialize(ser, serID);

		if(ser.IsLoading())
			m_spPathDat = ser.GetSerObj<cjAniVec3Dat>(serID);
		
		if(ser.IsSaving())
			serID = ser.GetSerObjID(m_spPCTDat);

		cjSerID_Serialize(ser, serID);

		if(ser.IsLoading())
			m_spPCTDat = ser.GetSerObj<cjAniFloatDat>(serID);

		ser << m_followAxis;
		//m_pad;
		
		// m_pathLengthCount, m_pathLengths, m_pathLengths is generated from m_spPathDat
		// no need to do streaming
		if(ser.IsLoading())
			GetTotalPathLen();
	}
	
CJSER_IMPL_SERIALIZE_END(cjPathInterpolator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjPathInterpolator)

	CJFLAG_CLONE
	
    if(m_spPathDat)
        d->SetPathDat((cjAniVec3Dat*)m_spPathDat->_Clone(cloner));

    if(m_spPCTDat)
        d->SetPCTDat((cjAniFloatDat*)m_spPCTDat->_Clone(cloner));

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

	if(m_pathLengths)
    {
        cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE keySiz;        
		GetPathKeys(keyCount, keyType, keySiz);
        
		d->m_pathLengths = (cjFLOAT*)cjMemAlloc(sizeof(cjFLOAT) * keyCount);

        cjUINT32 cpySiz = keyCount * sizeof(cjFLOAT);
        cjMemCpy(d->m_pathLengths, m_pathLengths, cpySiz);
    }

    d->m_totalPathLength = m_totalPathLength;

CJSER_IMPL_CLONE_PROPCOPY_END(cjPathInterpolator)

CJSER_IMPL_CLONE(cjPathInterpolator)

CJSER_IMPL_CLONE_POST_BEGIN(cjPathInterpolator)

	if(m_spPathDat)
        m_spPathDat->_PostClone(cloner);

	if(m_spPCTDat)
        m_spPCTDat->_PostClone(cloner);

CJSER_IMPL_CLONE_POST_END(cjPathInterpolator)


void cjPathInterpolator::_InitCommon()
{
	CJFLAG_IMPL_INIT

    m_lastPathIndex = 0;
    m_lastPCTIndex = 0;

    m_bankDir = BANKDIR_POSITIVE;
    m_maxBankAngle = 0.0f;
    m_smooth = 0.0f;
	m_maxCurvature = 0.0f;

	m_rot = cjQuat::INVALID;
	m_pos = cjVec3::INVALID;
	m_lastTime = CJ_FLT_INVALID;

    m_spPathDat = cjNULL;
	m_spPCTDat = cjNULL;

	m_pathLengthCount = 0;
    m_pathLengths = 0;
    m_totalPathLength = -1.0f;

	m_followAxis = 0;
	m_pad = 0;

//  SetAllowFlip(cjFalse);
	SetConstSpeed(cjFalse);
//  SetFollow(cjFalse);    
//  SetFlip(cjFalse);
    SetOpenCurve(cjTrue);   
    SetConstSpeedDatDirty(cjTrue);  	
}

cjPathInterpolator::cjPathInterpolator()
{
	_InitCommon();   
}

cjPathInterpolator::cjPathInterpolator(cjAniVec3Dat* pathDat, cjAniFloatDat* pctDat)
{
	_InitCommon();

	m_spPathDat = pathDat;
	m_spPCTDat = pctDat;
}

cjPathInterpolator::~cjPathInterpolator()
{
    m_spPathDat = cjNULL;
    m_spPCTDat = cjNULL;
	
    cjMemDeAlloc(m_pathLengths);
}

cjBool cjPathInterpolator::Update(cjFLOAT etime, cjRefObj*, cjQuatTM& value)
{
    if(!TimeHasChanged(etime))
    {
        if(m_pos == cjVec3::INVALID)
        {
            value.MakeInvalid();
            return cjFalse;
        }

        value.SetPos(m_pos);
        
		if(IsFollow())
            value.SetRotate(m_rot);

        return cjTrue;
    }

	// Percent Key
    cjUINT32 pctKeyCount; cjAniKeyFloat::EKEY_TYPE pctKeyType; cjBYTE pctKeySiz;
    cjAniKeyFloat* pctKeys = GetPCTKeys(pctKeyCount, pctKeyType, pctKeySiz);

    if(!pctKeys)
        return cjFalse;

	// Path Key
    cjUINT32 pathKeyCount; cjAniKeyVec3::EKEY_TYPE pathKeyType; cjBYTE pathKeySiz;
    cjAniKeyVec3* pathKeys = GetPathKeys(pathKeyCount, pathKeyType, pathKeySiz);

    if(!pathKeys)
        return cjFalse;

	// Update
	cjFLOAT percentRatio = cjAniKeyFloat::GenInterp(etime, pctKeys, pctKeyType, pctKeyCount, m_lastPCTIndex, pctKeySiz);
	
    cjUINT32 keyIdx0, keyIdx1; cjFLOAT rtime;
    GetPathInfoByRatio(percentRatio, keyIdx0, keyIdx1, rtime);

    if(IsFollow())
    {
        cjMatrix3 matRot;
        GetPathRotation(keyIdx0, keyIdx1, rtime, matRot);
        m_rot.MakeFromRotation(matRot);
    }

    m_pos = GenPathInterp(keyIdx0, keyIdx1, rtime);
    value.SetPos(m_pos);

    if(IsFollow())
        value.SetRotate(m_rot);

    m_lastTime = etime;

    return cjTrue;
}

void cjPathInterpolator::GetValidTimeRange(cjFLOAT& stTime, cjFLOAT& enTime) const
{
    stTime = CJ_FLT_INFINITY;
    enTime = CJ_FLT_INVALID;
 
	// channel 1 range only
	cjUINT32 channel = 1;

    if(GetKeyCount((cjBYTE)channel) > 0)
    {
        cjAniKey* beginKey = GetKey(0, (cjBYTE)channel);
        cjAniKey* endKey = GetKey(GetKeyCount((cjBYTE)channel) - 1, (cjBYTE)channel);

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

void cjPathInterpolator::Cache(cjFLOAT stTime, cjFLOAT enTime)
{
    if(m_spPCTDat)
        m_spPCTDat->CacheKeys(stTime, enTime);
}

cjInterpolator* cjPathInterpolator::GetInterpolator(cjFLOAT stTime, cjFLOAT enTime)
{
    cjPathInterpolator* interp = (cjPathInterpolator*)cjKeyBasedInterpolator::GetInterpolator(stTime, enTime);

    if(m_spPCTDat)
    {
        cjAniFloatDatRPtr spNewPCTDat = m_spPCTDat->GetDat(stTime, enTime);

        interp->SetPCTDat(spNewPCTDat);
        interp->m_lastPCTIndex = 0;
    }

    return interp;
}

cjUINT32 cjPathInterpolator::GetKeyCount(cjUINT32 channel) const
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

cjAniKey::EKEY_TYPE cjPathInterpolator::GetKeyType(cjUINT32 channel) const
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

cjAniKey::EKEY_VALTYPE cjPathInterpolator::GetKeyValType(cjUINT32 channel) const
{
    if(channel == ECHAN_PATH)
	{
        return cjAniKey::EKVAL_VEC3;
	}
    else if(channel == ECHAN_PERCENT_WEIGHT)
	{
        return cjAniKey::EKVAL_FLOAT;
	}
    
    cjAssert(channel < 2);

    return cjAniKey::EKVAL_MAX;
}

cjAniKey* cjPathInterpolator::GetKeyAry(cjUINT32 channel) const
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
        cjAniKeyFloat* keys = m_spPCTDat->GetKeys(keyCount, keyType, keySiz);

        return keys; 
    }

	cjAssert(channel < 2);

    return cjNULL;
}

cjBYTE cjPathInterpolator::GetKeySiz(cjUINT32 channel) const
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

void cjPathInterpolator::SetConstSpeed(cjBool constSpeed)
{
	_SetFlag(MASK_CONST_SPEED, BPOS_CONST_SPEED, constSpeed);

	if(constSpeed)
		SetConstSpeedDat();
}

cjAniKeyFloat* cjPathInterpolator::GetPCTKeys(cjUINT32& keyCount, cjAniKeyFloat::EKEY_TYPE& keyType, cjBYTE& keySiz) const
{
	cjAniKeyFloat* keys = cjNULL;

	if(m_spPCTDat)
	{
		keys = m_spPCTDat->GetKeys(keyCount, keyType, keySiz);
	}
	else
	{
		keyCount = 0; keyType = cjAniKey::EKT_NOINTERP; keySiz = 0;
	}

	return keys;
}

void cjPathInterpolator::ReplacePCTKeys(cjAniKeyFloat* keys, cjUINT32 keyCount, cjAniKeyFloat::EKEY_TYPE keyType)
{
    if(!m_spPCTDat)
        m_spPCTDat = new cjAniFloatDat;

    m_spPCTDat->ReplaceKeys(keys, keyCount, keyType);
    m_lastPCTIndex = 0;
}

cjFLOAT cjPathInterpolator::GetTotalPathLen()
{
    if(m_totalPathLength < 0.0f)
    {
        cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE keySiz;
        GetPathKeys(keyCount, keyType, keySiz);
        cjAssert(keyCount >= 1);

        cjMemDeAlloc(m_pathLengths);

        m_pathLengths = (cjFLOAT*)cjMemAlloc(sizeof(cjFLOAT) * keyCount);
        cjAssert(m_pathLengths);

        m_pathLengths[0] = 0.0f;

		for(cjUINT32 k0=0, k1=1; k0 < keyCount-1; k0++, k1++)
        {
            cjFLOAT len = GetPathLength(k0,k1,1.0f);
            m_pathLengths[k1] = m_pathLengths[k0] + len;
        }

        m_totalPathLength = m_pathLengths[keyCount-1];
    }

    return m_totalPathLength;
}

cjAniKeyVec3* cjPathInterpolator::GetPathKeys(cjUINT32& keyCount, cjAniKeyVec3::EKEY_TYPE& keyType, cjBYTE& keySiz) const
{
	cjAniKeyVec3* keys = cjNULL;

	if(m_spPathDat)
	{
		keys = m_spPathDat->GetKeys(keyCount, keyType, keySiz);
	}
	else
	{
		keyCount = 0; keyType = cjAniKey::EKT_NOINTERP; keySiz = 0;
	}

	return keys;
}

void cjPathInterpolator::ReplacePathKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType)
{
    if(!m_spPathDat)
        m_spPathDat = new cjAniVec3Dat;

    m_spPathDat->ReplaceKeys(keys, keyCount, keyType);

    cjBYTE keySiz = cjAniKeyVec3::GetKeySiz(keyType);
    m_lastPathIndex = 0;

    SetOpenCurve(keys->GetKey(0, keySiz)->GetValue() != keys->GetKey(keyCount-1, keySiz)->GetValue());

    SetConstSpeedDatDirty(cjTrue);

    if(IsConstSpeed())
        SetConstSpeedDat();

    cjAniKeyVec3::PFCurvedRatio pfnCurvature = cjAniKeyVec3::GetCurvedRatioKeyPF(keyType);
    m_maxCurvature = pfnCurvature((cjAniKey*)keys, keyCount);
}

void cjPathInterpolator::SetConstSpeedDat()
{
    if(!IsConstSpeedDatDirty())
        return;

    m_totalPathLength = GetTotalPathLen();
    SetConstSpeedDatDirty(cjFalse);
}

void cjPathInterpolator::GetPathInfo(cjFLOAT etime, cjUINT32& rstKeyIdx0, cjUINT32& rstKeyIdx1, cjFLOAT& rtime)
{
    if(IsConstSpeed())
    {
        GetPathInfoByRatio(etime, rstKeyIdx0, rstKeyIdx1, rtime);
        m_lastPathIndex = rstKeyIdx0;
        return;
    }

    cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE keySiz;
    cjAniKeyVec3* keys = GetPathKeys(keyCount, keyType, keySiz);
    cjAssert(keyCount >= 1);

    if(etime <= keys->GetKey(0,keySiz)->GetTime())
    {
        rstKeyIdx0 = 0; rstKeyIdx1 = 1; rtime = 0.0f;
        return;
    }

    cjAssert(keyCount >= 2);
    if(etime >= keys->GetKey(keyCount - 1,keySiz)->GetTime())
    {
        rstKeyIdx0 = keyCount - 2; rstKeyIdx1 = keyCount - 1; rtime = 1.0f;
        return;
    }

    cjBool found = cjFalse;
    cjBool firstLoop = cjTrue;
    cjUINT32 k0;
    cjUINT32 k1=0;

    for(k0=m_lastPathIndex; k0<keyCount-1;)
    {
        if(etime == keys->GetKey(k0, keySiz)->GetTime())
        {
            m_lastPathIndex = k0; k1 = k0 + 1; found = cjTrue;
            break;
        }
        else if(etime > keys->GetKey(k0, keySiz)->GetTime() && etime < keys->GetKey(k0+1, keySiz)->GetTime())
        {
            m_lastPathIndex = k0; k1 = k0 + 1; found = cjTrue;
            break;
        }

        if(k0 == keyCount-2 && firstLoop)
        {
            k0 = 0; firstLoop = cjFalse;
            continue;
        }

        k0++;
    }
        
    cjAssert(found);
        
    rtime = (etime - keys->GetKey(k0, keySiz)->GetTime()) / (keys->GetKey(k1, keySiz)->GetTime() - keys->GetKey(k0, keySiz)->GetTime());
    rstKeyIdx0 = k0;
    rstKeyIdx1 = k1;
}

void cjPathInterpolator::GetPathInfoByRatio(cjFLOAT ratio, cjUINT32& rstKeyIdx0, cjUINT32& rstKeyIdx1, cjFLOAT& rtime)
{
    cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE keySiz;
    GetPathKeys(keyCount, keyType, keySiz);

    cjAssert(keyCount >= 2);

    if(ratio == 0.0f)
    {
        rstKeyIdx0 = 0; rstKeyIdx1 = 1; rtime = 0.0f;
        return;
    }
    else if(ratio < 0.0f)
    {
        ratio = cjFMod(ratio, 1.0f) + 1.0f;
    }

    if(ratio == 1.0f)
    {
        rstKeyIdx0 = keyCount - 2; rstKeyIdx1 = keyCount - 1; rtime = 1.0f;
        return;
    }
    else if(ratio > 1.0f)
    {
        ratio = cjFMod(ratio, 1.0f);
    }

    cjFLOAT len = ratio * m_totalPathLength;
    cjFLOAT dist = 0.0f;
    cjFLOAT tmpRTime = 0.0f;
    cjUINT32 k0, k1;

    for(k0=0, k1=1; k0<keyCount-1; k0++, k1++)
    {
        if(len <= m_pathLengths[k1])
        {
            dist = len - m_pathLengths[k0];
            tmpRTime = dist / (m_pathLengths[k1] - m_pathLengths[k0]);
            break;
        }
    }

    static const cjINT32 maxIterations = 32;

    for(cjINT32 i=0; i<maxIterations; i++)
    {
        cjFLOAT diff = GetPathLength(k0, k1, tmpRTime) - dist;
        const cjFLOAT tolerance = 1e-04f;

        if(cjAbs(diff) <= tolerance)
            break;

        tmpRTime -= diff / GetPathSpeed(k0, k1, tmpRTime);
    }

    rstKeyIdx0 = k0;
    rstKeyIdx1 = k1;

    rtime = tmpRTime;
}

void cjPathInterpolator::GetPathRotation(cjUINT32 keyIdx0, cjUINT32 keyIdx1, cjFLOAT rtime, cjMatrix3& rstMatRot)
{
    cjMatrix3 matTM = cjMatrix3::Identity;
    
	cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE keySiz;
    cjAniKeyVec3* keys = GetPathKeys(keyCount, keyType, keySiz);

    cjVec3 tang, nrm, binrm;
    cjFLOAT curvature;

    if(keyCount < 2)
    {
        rstMatRot = matTM;
        return;
    }
    
    if(IsAllowFlip())
    {
        cjAniKeyVec3::CoordinateFrame(rtime, keys->GetKey(keyIdx0, keySiz), 
			keys->GetKey(keyIdx1, keySiz), keyType, tang, nrm, binrm, curvature);

        cjVec3 tmpTang, tmpNrm;
        cjFLOAT a0, a1, tmpCurvature;

        if(m_smooth)
        {
            if(rtime >= 1.0f - m_smooth)
            {
                cjUINT32 desireKeyIdx = (!IsOpenCurve() && keyIdx1 == keyCount-1) ? 0 : keyIdx1;

                if(desireKeyIdx < keyCount-1)
                {
                    a0 = (1.0f - rtime + m_smooth) / (2.0f*m_smooth);
                    a1 = 1.0f - a0;

                    cjAniKeyVec3::CoordinateFrame(1.0f - m_smooth, keys->GetKey(keyIdx0, keySiz), 
						keys->GetKey(keyIdx1, keySiz), keyType, tmpTang, nrm, binrm, curvature);

                    cjAniKeyVec3::CoordinateFrame(m_smooth, keys->GetKey(desireKeyIdx, keySiz), 
						keys->GetKey(desireKeyIdx + 1, keySiz), keyType, tmpTang, tmpNrm, binrm, tmpCurvature);

                    nrm = a0*nrm + a1*tmpNrm;
                    nrm.Normalize();

                    curvature = a0*curvature + a1*tmpCurvature;
                    binrm = tang.Cross(nrm);
                }
            }
            else if(rtime <= m_smooth)
            {
                cjUINT32 desireKeyIdx = (!IsOpenCurve() && keyIdx0 == 0 ? keyCount-1 : keyIdx0);

                if(desireKeyIdx >= 1)
                {
                    a1 = (m_smooth - rtime) / (2.0f * m_smooth);
                    a0 = 1.0f - a1;

                    cjAniKeyVec3::CoordinateFrame(m_smooth, 
						keys->GetKey(keyIdx0, keySiz), keys->GetKey(keyIdx1, keySiz), keyType, 
						tmpTang, nrm, binrm, curvature);

                    cjAniKeyVec3::CoordinateFrame(1.0f - m_smooth, 
						keys->GetKey(desireKeyIdx - 1, keySiz), keys->GetKey(desireKeyIdx, keySiz), keyType, 
						tmpTang, tmpNrm, binrm, tmpCurvature);

                    nrm = a0*nrm + a1*tmpNrm;
                    nrm.Normalize();

                    curvature = a0*curvature + a1*tmpCurvature;
                    binrm = tang.Cross(nrm);
                }
            }
        }
                
        matTM.SetCol(0, tang);   // direction        
        matTM.SetCol(1, -nrm);   // up        
        matTM.SetCol(2, -binrm); // right
    }
    else
    {
        cjAniKeyVec3::CoordinateFrame(rtime, keys->GetKey(keyIdx0, keySiz), 
			keys->GetKey(keyIdx1, keySiz), keyType, tang, nrm, binrm, curvature);
		        
		// direction
        matTM.SetCol(0, tang); 
        
		cjVec3 up = cjVec3::UNIT_Z.UnitCross(tang);
        matTM.SetCol(1, up);        

        cjVec3 right = tang.Cross(up);
        matTM.SetCol(2, right);
    }

    if(GetFlip())
    {
        cjVec3 t;

		// look-at
		matTM.GetCol(0, t);	
        matTM.SetCol(0, -t);

		// up
        matTM.GetCol(1, t);
        matTM.SetCol(1, -t);
    }

	{
		cjVec3 t1, t2;
		switch(m_followAxis)
		{
			case 1:
				{
					matTM.GetCol(0,  t1); matTM.GetCol(1, t2); 
					matTM.SetCol(0, -t2); matTM.SetCol(1, t1);
				}
				break;

			case 2:
				{
					matTM.GetCol(0,  t1); matTM.GetCol(2, t2);
					matTM.SetCol(0, -t2); matTM.SetCol(2, t1);
				}
				break;
		}
	}

    if(IsBanking())
    {
        const cjFLOAT pi4 = 4.0f / CJ_PI;
        cjFLOAT bankingAmount = cjFLOAT(m_bankDir) * m_maxBankAngle;

        if(curvature < m_maxCurvature)
            bankingAmount *= pi4 * cjATan(curvature / m_maxCurvature);

        cjMatrix3 matBanking;        
        matBanking.MakeRotationFromAxis(bankingAmount, tang.x, tang.y, tang.z);

        matTM = matBanking * matTM;
    }

    rstMatRot = matTM;
}

cjVec3 cjPathInterpolator::GenPathInterp(cjINT32 keyIdx0, cjINT32 keyIdx1, cjFLOAT rtime)
{
    cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE keySiz;
    cjAniKeyVec3* keys = GetPathKeys(keyCount, keyType, keySiz);

    cjAniKeyVec3::PFInterpolation pfnInterp = cjAniKeyVec3::GetInterpKeyPF(keyType);
    cjAssert(pfnInterp);

    cjVec3 rst;
	pfnInterp(rtime, keys->GetKey(keyIdx0, keySiz), keys->GetKey(keyIdx1, keySiz), &rst);

    return rst;
}

cjFLOAT cjPathInterpolator::GetPathSpeed(cjINT32 keyIdx0, cjINT32 keyIdx1, cjFLOAT rtime) const
{
    cjUINT32 keyCount; cjAniKeyVec3::EKEY_TYPE keyType; cjBYTE keySiz;
    cjAniKeyVec3* keys = GetPathKeys(keyCount, keyType, keySiz);

    cjAniKeyVec3::PFInterpolation pfInterpolationD1 = cjAniKeyVec3::GetInterpD1KeyPF(keyType);

    cjVec3 rst;
    pfInterpolationD1(rtime, keys->GetKey(keyIdx0, keySiz), keys->GetKey(keyIdx1, keySiz), &rst);

    return rst.Length();
}

cjFLOAT cjPathInterpolator::GetPathLength(cjINT32 keyIdx0, cjINT32 keyIdx1, cjFLOAT rtime) const
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


//
// cjColorInterpolator - cjKeyBasedInterpolator
//

CJSER_IMPL_CLASS(cjColorInterpolator, cjKeyBasedInterpolator);

CJSER_IMPL_SERIALIZE_BEGIN(cjColorInterpolator)

	if(ser.IsQuerySerObjAndStr())
	{
		if(m_spColorDat)
			(*m_spColorDat) >> ser;

		return;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		cjSerID serID;

		ser << m_value;

		if(ser.IsSaving())
			serID = ser.GetSerObjID(m_spColorDat);

		cjSerID_Serialize(ser, serID);

		if(ser.IsLoading())
			m_spColorDat = ser.GetSerObj<cjAniColorDat>(serID);

		// m_lastIndex is not serialized
	}

CJSER_IMPL_SERIALIZE_END(cjColorInterpolator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjColorInterpolator)

	d->m_value = m_value;
	d->m_spColorDat = m_spColorDat;
	d->m_lastIndex = m_lastIndex;

CJSER_IMPL_CLONE_PROPCOPY_END(cjColorInterpolator)

CJSER_IMPL_CLONE(cjColorInterpolator)


cjColorInterpolator::cjColorInterpolator()
{
    m_value = INVALID_COLORF;	
    m_lastIndex = 0;
}

cjColorInterpolator::cjColorInterpolator(cjAniColorDat* colorDat)
{
    m_value = INVALID_COLORF;
	m_spColorDat = colorDat;
    m_lastIndex = 0;
}

cjColorInterpolator::cjColorInterpolator(const cjColorF& poseValue)
{
    m_value = poseValue;
    m_lastIndex = 0;
}

cjColorInterpolator::~cjColorInterpolator()
{
}

cjBool cjColorInterpolator::Update(cjFLOAT etime, cjRefObj*, cjColorF& value)
{
    if(!TimeHasChanged(etime))
    {
        value = m_value;  
        
		if(m_value == INVALID_COLORF)
            return cjFalse;

        return cjTrue;
    }

    cjUINT32 keyCount; cjAniKeyColor::EKEY_TYPE keyType; cjBYTE keySiz;

    cjAniKeyColor* keys = GetKeys(keyCount, keyType, keySiz);
    
	if(keyCount > 0)
    {
        m_value = cjAniKeyColor::GenInterp(etime, keys, keyType,
            keyCount, m_lastIndex, keySiz);
    }

    if(m_value == INVALID_COLORF)
        return cjFalse;

    value = m_value;  
    m_lastTime = etime;

    return cjTrue;
}

void cjColorInterpolator::Collapse()
{
    if(m_spColorDat)
    {
        cjUINT32 keyCount; cjAniKey::EKEY_TYPE keyType; cjBYTE keySiz;
        cjAniKeyColor* keys = m_spColorDat->GetKeys(keyCount, keyType, keySiz);

        if(keyCount == 0)
        {
            m_spColorDat = cjNULL;
            m_value = INVALID_COLORF;
        }
        else
        {
            cjAniKey::PFIsPosed pfIsPosed = cjAniKeyColor::GetIsPosedKeyPF(keyType);

			if(pfIsPosed(keys, keyCount))
            {
                m_value = keys->GetKey(0, keySiz)->GetValue();
                m_spColorDat = cjNULL;
            }
        }
    }
}

void cjColorInterpolator::Cache(cjFLOAT stTime, cjFLOAT enTime)
{
    if(m_spColorDat)
        m_spColorDat->CacheKeys(stTime, enTime);
}

cjInterpolator* cjColorInterpolator::GetInterpolator(cjFLOAT stTime, cjFLOAT enTime)
{
    cjColorInterpolator* interp 
		= (cjColorInterpolator*)cjKeyBasedInterpolator::GetInterpolator(stTime, enTime);

    if(m_spColorDat)
    {
        cjAniColorDatRPtr spNewColorDat = m_spColorDat->GetDat(stTime, enTime);

        interp->SetColorDat(spNewColorDat);
        interp->m_lastIndex = 0;
    }

    return interp;
}

cjUINT32 cjColorInterpolator::GetKeyCount(cjUINT32) const
{
    if(!m_spColorDat)
        return 0;
    else
        return m_spColorDat->GetKeyCount();
}

cjAniKey::EKEY_TYPE cjColorInterpolator::GetKeyType(cjUINT32) const
{
    if(!m_spColorDat)
        return cjAniKey::EKT_NOINTERP;

    cjUINT32 keyCount; cjAniKeyColor::EKEY_TYPE keyType; cjBYTE keySiz;
    m_spColorDat->GetKeys(keyCount, keyType, keySiz);

    return keyType; 
}

cjAniKey* cjColorInterpolator::GetKeyAry(cjUINT32) const 
{
    if(!m_spColorDat)
        return cjNULL;

    cjUINT32 keyCount; cjAniKeyColor::EKEY_TYPE keyType; cjBYTE keySiz;
    cjAniKeyColor* keys = m_spColorDat->GetKeys(keyCount, keyType, keySiz);

    return keys; 
}

cjBYTE cjColorInterpolator::GetKeySiz(cjUINT32) const 
{
    if(!m_spColorDat)
        return 0;

    cjUINT32 keyCount; cjAniKeyColor::EKEY_TYPE keyType; cjBYTE keySiz;
    m_spColorDat->GetKeys(keyCount, keyType, keySiz);

    return keySiz; 
}

cjBool cjColorInterpolator::GetChannelPosed(cjUINT32) const
{
    if(m_spColorDat)
        return cjFalse;

    if(m_value == INVALID_COLORF)
        return cjFalse;

    return cjTrue;
}

void cjColorInterpolator::SetPoseValue(const cjColorF& poseValue)
{
    m_spColorDat = cjNULL;
    m_value = poseValue;
}

cjAniKeyColor* cjColorInterpolator::GetKeys(cjUINT32& keyCount, cjAniKeyColor::EKEY_TYPE& keyType, cjBYTE& keySiz) const
{
	if(m_spColorDat)
		return m_spColorDat->GetKeys(keyCount, keyType, keySiz);

	keyCount = 0;
	keyType = cjAniKeyColor::EKT_NOINTERP;
	keySiz = 0;

	return cjNULL;
}

void cjColorInterpolator::ReplaceKeys(cjAniKeyColor* keys, cjUINT32 keyCount, cjAniKeyColor::EKEY_TYPE keyType)
{
	if(keyCount > 0)
	{
		if(!m_spColorDat)
			m_spColorDat = new cjAniColorDat;

		m_spColorDat->ReplaceKeys(keys, keyCount, keyType);
		m_lastIndex = 0;
	}
	else if(m_spColorDat)
	{
		m_spColorDat->ReplaceKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
	}
}

void cjColorInterpolator::SetKeys(cjAniKeyColor* keys, cjUINT32 keyCount, cjAniKeyColor::EKEY_TYPE keyType)
{
	if(keyCount > 0)
	{
		if(!m_spColorDat)
			m_spColorDat = new cjAniColorDat;

		m_spColorDat->SetKeys(keys, keyCount, keyType);
		m_lastIndex = 0;
	}
	else if(m_spColorDat)
	{
		m_spColorDat->SetKeys(cjNULL, 0, cjAniKey::EKT_NOINTERP);
	}
}


//
// Blend interpAryItem
//

void cjBlendInterpItem::operator >> (cjSerializer& ser)
{
	if(ser.IsQuerySerObjAndStr())
	{
		if(m_spInterpolator)
			(*m_spInterpolator) >> ser;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		cjSerID serID;

		if(ser.IsSaving())
		{
			serID = ser.GetSerObjID(m_spInterpolator);
			cjSerID_Serialize(ser, serID);
		}
		else if(ser.IsLoading())
		{
			cjSerID_Serialize(ser, serID);
			m_spInterpolator = ser.GetSerObj<cjInterpolator>(serID);
		}

		ser << m_weight;
		ser << m_normalizedWeight;
		ser << m_priority;
		ser << m_easeSpinner;
		ser << m_updateTime;
	}
}

cjBlendInterpItem::cjBlendInterpItem()
{
	ClearValues();
}

void cjBlendInterpItem::ClearValues()
{
    m_spInterpolator	= cjNULL;
    m_weight			= 0.0f;
    m_normalizedWeight	= 0.0f;
    m_priority			= 0;
    m_easeSpinner		= 0.0f;
    m_updateTime		= CJ_FTIME_INVALID;
}


//
// Blend interpolator
//

CJSER_IMPL_CLASS(cjBlendInterpolator, cjInterpolator)

CJSER_IMPL_SERIALIZE_BEGIN(cjBlendInterpolator)

	if(ser.IsQuerySerObjAndStr())
	{
		if(m_interpCount)
		{
			for(cjBYTE c=0; c<m_interpCount; c++)
				m_interpAry[c] >> ser;		
		}

		if(m_singleInterpolator)
			(*m_singleInterpolator) >> ser;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		ser << m_flag;
		ser << m_arySiz;
		ser << m_interpCount;
		ser << m_singleIdx;

		ser << m_highPriority;
		ser << m_nextHighPriority;

		for(cjINT32 i=0; i<m_interpCount; i++)
			m_interpAry[i] >> ser;

		cjSerID serID = cjSerID_Invalid;

		if(ser.IsSaving())
		{
			serID = ser.GetSerObjID(m_singleInterpolator);
			cjSerID_Serialize(ser, serID);
		}
		else if(ser.IsLoading())
		{
			cjSerID_Serialize(ser, serID);
			m_singleInterpolator = ser.GetSerObj<cjInterpolator>(serID);
		}

		ser << m_singleTime;
		ser << m_weightThreshold;
		ser << m_highSumOfWeights;
		ser << m_nextHighSumOfWeights;
		ser << m_highEaseSpinner;
	}

CJSER_IMPL_SERIALIZE_END(cjBlendInterpolator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjBlendInterpolator)

	d->m_flag                   = m_flag;

	d->m_arySiz 				= m_arySiz;
	d->m_interpAry				= new cjBlendInterpItem[m_arySiz];

	d->m_weightThreshold 		= m_weightThreshold;
	d->m_interpAry				= cjNULL;

	if(!IsManagerControlled())
	{
		for(cjUINT32 i=0; i<m_arySiz; i++)
		{
			cjBlendInterpItem& srcItem = m_interpAry[i];
			cjBlendInterpItem& dstItem = d->m_interpAry[i];

			if(srcItem.m_spInterpolator)
				dstItem.m_spInterpolator = (cjInterpolator*)(srcItem.m_spInterpolator->_Clone(cloner));

			dstItem.m_weight = srcItem.m_weight;
			dstItem.m_normalizedWeight = srcItem.m_normalizedWeight;
			dstItem.m_priority = srcItem.m_priority;
			dstItem.m_easeSpinner = srcItem.m_easeSpinner;
			dstItem.m_updateTime = srcItem.m_updateTime;
		}

		d->m_interpCount = m_interpCount;
		d->m_singleIdx = m_singleIdx;
		d->m_singleTime = m_singleTime;
		d->m_highPriority = m_highPriority;
		d->m_nextHighPriority = m_nextHighPriority;
	}

CJSER_IMPL_CLONE_PROPCOPY_END(cjBlendInterpolator)
	
CJSER_IMPL_CLONE_POST_BEGIN(cjBlendInterpolator)

    cjRefObj* clone = (cjRefObj*)cloner->GetCloned(this);
    cjBlendInterpolator* dstInterp = (cjBlendInterpolator*)clone;

    for(cjINT32 i=0; i<m_arySiz; i++)
    {
        cjInterpolator* interp = m_interpAry[i].m_spInterpolator;

        if(interp)
            interp->_PostClone(cloner);
    }

    if(m_singleInterpolator)
    {
        cjInterpolator* clonedInterp;
		clone = (cjRefObj*)cloner->GetCloned(m_singleInterpolator);

        if(clone)
            clonedInterp = (cjInterpolator*)clone;
        else
            clonedInterp = m_singleInterpolator;

		dstInterp->m_singleInterpolator = clonedInterp;
    }

CJSER_IMPL_CLONE_POST_END(cjBlendInterpolator)


const cjBYTE cjBlendInterpolator::INVALID_INDEX					= (cjBYTE)0xff;

const cjBYTE cjBlendInterpolator::FL_MANAGER_CONTROLLED			= 0x1;
const cjBYTE cjBlendInterpolator::FL_USE_HIGHWEIGHT_ONLY		= 0x2;
const cjBYTE cjBlendInterpolator::FL_COMPUTE_NORMALIZED_WEIGHT	= 0x4;	
	
cjBYTE cjBlendInterpolator::ms_aryGrowCount						= 2;


cjBlendInterpolator::cjBlendInterpolator() 
{
	_CommonInit(0.0f, 0);
}

cjBlendInterpolator::cjBlendInterpolator(cjBool managerControlled, cjFLOAT weightThreshold, cjBYTE arySiz)
{
	_CommonInit(weightThreshold, arySiz);
    SetManagerControlled(managerControlled);

    if(m_arySiz > 0)
        m_interpAry = new cjBlendInterpItem[m_arySiz];
}

void cjBlendInterpolator::_CommonInit(cjFLOAT weightThreshold, cjBYTE arySiz)
{
    m_flag						= 0;
    m_arySiz					= arySiz;
    m_interpCount				= 0;
    m_singleIdx					= INVALID_INDEX; 
    m_highPriority				= CJ_CHAR_MIN;
    m_nextHighPriority			= CJ_CHAR_MIN; 
    m_interpAry					= cjNULL;
    m_singleInterpolator		= cjNULL;
    m_weightThreshold			= weightThreshold;
    m_singleTime				= CJ_FTIME_INVALID; 
    m_highSumOfWeights			= CJ_FLT_INVALID;
    m_nextHighSumOfWeights		= CJ_FLT_INVALID; 
    m_highEaseSpinner			= CJ_FLT_INVALID;
}

cjBlendInterpolator::~cjBlendInterpolator()
{
	if(m_interpAry)
	{
		delete [] m_interpAry;
		m_interpAry = cjNULL;
	}

    m_singleInterpolator = cjNULL;
}

void cjBlendInterpolator::SetPriority(char priority, cjBYTE index)
{
    cjAssert(index < m_arySiz);

    if(m_interpAry[index].m_priority == priority)
        return;

    m_interpAry[index].m_priority = priority;

    if(priority > m_highPriority)
    {
        m_nextHighPriority = m_highPriority;
        m_highPriority = priority;
    }
    else
    {
        m_highPriority = m_nextHighPriority = CJ_CHAR_MIN;
        for(cjBYTE k=0; k<m_arySiz; k++)
        {
            cjBlendInterpItem& tmpItem = m_interpAry[k];

            if(tmpItem.m_spInterpolator)
            {
                if(tmpItem.m_priority > m_nextHighPriority)
                {
                    if(tmpItem.m_priority > m_highPriority)
                    {
                        m_nextHighPriority = m_highPriority;
                        m_highPriority = tmpItem.m_priority;
                    }
                    else if(tmpItem.m_priority < m_highPriority)
                    {
                        m_nextHighPriority = tmpItem.m_priority;
                    }
                }
            }
        }
    }

    ClearWeightSums();
    SetNeedComputingNormalizedWeights(cjTrue);
}

cjBYTE cjBlendInterpolator::AddInterpInfo(cjInterpolator* interpolator, cjFLOAT weight, char priority, cjFLOAT easeSpinner)
{
    cjAssert(interpolator && CheckInterpolatorType(interpolator));

    cjBYTE index;

    for(index=0; index<m_arySiz; index++)
    {
        if(!m_interpAry[index].m_spInterpolator)
            break;
    }

    if(index == m_arySiz && !ReallocAry())
        return INVALID_INDEX;

    cjBlendInterpItem& newItem = m_interpAry[index];

    newItem.m_spInterpolator	= interpolator;
    newItem.m_weight			= weight;
    newItem.m_priority			= priority;
    newItem.m_easeSpinner		= easeSpinner;

    if(priority > m_nextHighPriority)
    {
        if(priority > m_highPriority)
        {
            m_nextHighPriority = m_highPriority;
            m_highPriority = priority;
        }
        else if(priority != m_highPriority)
        {
            m_nextHighPriority = priority;
        }
    }

    if(++m_interpCount == 1)
    {
        m_singleIdx = index;
        m_singleInterpolator = interpolator;
    }
    else
    {
        m_singleIdx = INVALID_INDEX;
        m_singleInterpolator = cjNULL;
        m_singleTime = CJ_FTIME_INVALID;
    }

    ClearWeightSums();    
    SetNeedComputingNormalizedWeights(cjTrue);

    return index;
}

cjInterpolatorRPtr cjBlendInterpolator::RemoveInterpInfo(cjBYTE index)
{
    cjAssert(index < m_arySiz);
    cjBlendInterpItem& item = m_interpAry[index];

    cjInterpolatorRPtr spInterpolator = item.m_spInterpolator;
    char priority = item.m_priority;
    item.ClearValues();

    if(priority == m_highPriority || priority == m_nextHighPriority)
    {
        m_highPriority = m_nextHighPriority = CJ_CHAR_MIN;

        for(cjBYTE k=0; k<m_arySiz; k++)
        {
            cjBlendInterpItem& tmpItem = m_interpAry[k];

            if(tmpItem.m_spInterpolator)
            {
                if(tmpItem.m_priority > m_nextHighPriority)
                {
                    if(tmpItem.m_priority > m_highPriority)
                    {
                        m_nextHighPriority = m_highPriority;
                        m_highPriority = tmpItem.m_priority;
                    }
                    else if(tmpItem.m_priority < m_highPriority)
                    {
                        m_nextHighPriority = tmpItem.m_priority;
                    }
                }
            }
        }
    }

    if(--m_interpCount == 1)
    {
        if(m_arySiz == 2)
        {
            if(index == 0)
                m_singleIdx = 1;
            else
                m_singleIdx = 0;
        }
        else
        {
            for(cjBYTE k=0; k<m_arySiz; k++)
            {
                if(m_interpAry[k].m_spInterpolator)
                    m_singleIdx = k;
            }
        }

        m_singleInterpolator = m_interpAry[m_singleIdx].m_spInterpolator;
        m_singleTime = m_interpAry[m_singleIdx].m_updateTime;
    }
    else if(m_interpCount == 0)
    {
        m_singleIdx				= INVALID_INDEX;
        m_singleInterpolator	= cjNULL;
        m_singleTime			= CJ_FTIME_INVALID;
    }

    ClearWeightSums();
	SetNeedComputingNormalizedWeights(cjTrue);

    return spInterpolator;
}

cjFLOAT cjBlendInterpolator::GetWeight(cjBYTE index) const
{
    cjAssert(index < m_arySiz);

    if(m_interpCount == 1 && index == m_singleIdx)
        return 1.0f; // not meaning at single interpolator

    return m_interpAry[index].m_weight;
}

void cjBlendInterpolator::SetWeight(cjFLOAT weight, cjBYTE index)
{
    cjAssert(index < m_arySiz);
    cjAssert(weight >= 0.0f);

    if(m_interpCount == 1 && index == m_singleIdx)
        return; // skip at single interpolator

    if(m_interpAry[index].m_weight == weight)
        return;

    m_interpAry[index].m_weight = weight;
    ClearWeightSums();

	SetNeedComputingNormalizedWeights(cjTrue);
}

void cjBlendInterpolator::ComputeNormalizedWeights()
{
    if(!NeedComputingNormalizedWeights())
        return;
        
    SetNeedComputingNormalizedWeights(cjFalse);

    if(m_interpCount == 1)
    {
        m_interpAry[m_singleIdx].m_normalizedWeight = 1.0f;
        return;
    }
    else if(m_interpCount == 2)
    {
        ComputeNormalized2Weight();
        return;
    }

    if(m_highSumOfWeights == CJ_FLT_INVALID)
    {
        m_highSumOfWeights		= 0.0f;
        m_nextHighSumOfWeights	= 0.0f;
        m_highEaseSpinner		= 0.0f;

        for(cjBYTE k=0; k< m_arySiz; k++)
        {
            cjBlendInterpItem& item = m_interpAry[k];

            if(item.m_spInterpolator)
            {
                cjFLOAT realWeight = item.m_weight * item.m_easeSpinner;
                if(item.m_priority == m_highPriority)
                {
                    m_highSumOfWeights += realWeight;

                    if(item.m_easeSpinner > m_highEaseSpinner)
                        m_highEaseSpinner = item.m_easeSpinner;
                }
                else if(item.m_priority == m_nextHighPriority)
                {
                    m_nextHighSumOfWeights += realWeight;
                }
            }
        }
    }

    cjAssert(m_highEaseSpinner >= 0.0f && m_highEaseSpinner <= 1.0f);

    cjFLOAT oneMinusHighEaseSpinner		= 1.0f - m_highEaseSpinner;
    cjFLOAT totalSumOfWeights			= m_highEaseSpinner * m_highSumOfWeights + oneMinusHighEaseSpinner * m_nextHighSumOfWeights;
    cjFLOAT oneOverTotalSumOfWeights	= (totalSumOfWeights > 0.0f) ? (1.0f / totalSumOfWeights) : 0.0f;

    for(cjBYTE k=0; k< m_arySiz; k++)
    {
        cjBlendInterpItem& item = m_interpAry[k];

        if(item.m_spInterpolator)
        {
            if(item.m_priority == m_highPriority)
                item.m_normalizedWeight = m_highEaseSpinner * item.m_weight * item.m_easeSpinner * oneOverTotalSumOfWeights;
            else if(item.m_priority == m_nextHighPriority)
                item.m_normalizedWeight = oneMinusHighEaseSpinner * item.m_weight * item.m_easeSpinner * oneOverTotalSumOfWeights;
            else
                item.m_normalizedWeight = 0.0f;
        }
    }

    cjFLOAT sumOfNormalizedWeights = 1.0f;

    if(m_weightThreshold > 0.0f)
    {
        sumOfNormalizedWeights = 0.0f;

        for(cjBYTE m=0; m<m_arySiz; m++)
        {
            cjBlendInterpItem& item = m_interpAry[m];

            if(item.m_spInterpolator && item.m_normalizedWeight != 0.0f)
            {
                if(item.m_normalizedWeight < m_weightThreshold)
                    item.m_normalizedWeight = 0.0f;

				sumOfNormalizedWeights += item.m_normalizedWeight;
            }
        }
    }

    if(sumOfNormalizedWeights != 1.0f)
    {
        cjFLOAT oneOverSumOfNormalizedWeights = (sumOfNormalizedWeights > 0.0f) ? (1.0f / sumOfNormalizedWeights) : 0.0f;

        for(cjBYTE m=0; m<m_arySiz; m++)
        {
            cjBlendInterpItem& item = m_interpAry[m];

            if(item.m_normalizedWeight != 0.0f)
                item.m_normalizedWeight = item.m_normalizedWeight * oneOverSumOfNormalizedWeights;
        }
    }

    if(IsUseHighestWeightOnly())
    {
        cjFLOAT highest = -1.0f;
        cjBYTE highIndex = INVALID_INDEX;

        for(cjBYTE m=0; m<m_arySiz; m++)
        {
            if(m_interpAry[m].m_normalizedWeight > highest)
            {
                highIndex = m;
                highest = m_interpAry[m].m_normalizedWeight;
            }

            m_interpAry[m].m_normalizedWeight = 0.0f;
        }

        cjAssert(highIndex != INVALID_INDEX);
        m_interpAry[highIndex].m_normalizedWeight = 1.0f; // set the highest 1.0
    }
}

cjFLOAT cjBlendInterpolator::GetNormalizedWeight(cjBYTE index) const
{
    cjAssert(index < m_arySiz);

    if(m_interpCount == 1 && index == m_singleIdx)
        return 1.0f; // skip for single interpolator

    return m_interpAry[index].m_normalizedWeight;
}

cjBYTE cjBlendInterpolator::GetHighestWeightedIndex() const
{
    if(m_interpCount == 1)
        return m_singleIdx;

    cjFLOAT highestWeight = 0.0f;
    cjBYTE highIndex = CJ_UCHAR_MAX;

    for(cjBYTE k=0; k<m_arySiz; k++)
    {
        if(m_interpAry[k].m_normalizedWeight > highestWeight)
        {
            highIndex = k;
            highestWeight = m_interpAry[k].m_normalizedWeight;
        }
    }

    if(highIndex != CJ_UCHAR_MAX)
        return highIndex;

    return INVALID_INDEX;
}

void cjBlendInterpolator::SetEaseSpinner(cjFLOAT easeSpinner, cjBYTE index)
{
    cjAssert(index < m_arySiz);
    cjAssert(easeSpinner >= 0.0f && easeSpinner <= 1.0f);

    if(m_interpCount == 1 && index == m_singleIdx)
        return; // skip single interpolator

    if(m_interpAry[index].m_easeSpinner == easeSpinner)
        return;

    m_interpAry[index].m_easeSpinner = easeSpinner;

    ClearWeightSums();
    SetNeedComputingNormalizedWeights(cjTrue);
}

cjBool cjBlendInterpolator::ReallocAry()
{
    if(!ms_aryGrowCount)
        return cjFalse;

    cjBYTE newArySiz = m_arySiz + ms_aryGrowCount;
    cjAssert(newArySiz > m_arySiz);
    
    cjBlendInterpItem* newInterpAry = new cjBlendInterpItem[newArySiz];

    for(cjBYTE k=0; k<m_arySiz; k++)
        newInterpAry[k] = m_interpAry[k];

	if(m_interpAry)
		delete [] m_interpAry;

	m_arySiz = newArySiz;
    m_interpAry = newInterpAry;

    return cjTrue;
}

void cjBlendInterpolator::ComputeNormalized2Weight()
{
    cjAssert(m_interpCount == 2);

    cjBlendInterpItem* item1 = cjNULL;
    cjBlendInterpItem* item2 = cjNULL;

    for(cjBYTE i=0; i<m_arySiz; i++)
    {
        if(m_interpAry[i].m_spInterpolator)
        {
            if(!item1)
            {
                item1 = &m_interpAry[i];
                continue;
            }
            else if(!item2)
            {
                item2 = &m_interpAry[i];
                break;
            }
        }
    }
    
	cjAssert(item1 && item2);

    cjFLOAT realWeight1 = item1->m_weight * item1->m_easeSpinner;
    cjFLOAT realWeight2 = item2->m_weight * item2->m_easeSpinner;

    if(realWeight1 == 0.0f && realWeight2 == 0.0f)
    {
        item1->m_normalizedWeight = 0.0f;
        item2->m_normalizedWeight = 0.0f;
        return;
    }

    if(item1->m_priority > item2->m_priority)
    {
        if(item1->m_easeSpinner == 1.0f)
        {
            item1->m_normalizedWeight = 1.0f;
            item2->m_normalizedWeight = 0.0f;
            return;
        }

        cjFLOAT oneMinusEaseSpinner = 1.0f - item1->m_easeSpinner;
        cjFLOAT oneOverSumOfWeights = 1.0f / (item1->m_easeSpinner * realWeight1 + oneMinusEaseSpinner * realWeight2);

        item1->m_normalizedWeight = item1->m_easeSpinner * realWeight1 * oneOverSumOfWeights;
        item2->m_normalizedWeight = oneMinusEaseSpinner * realWeight2 * oneOverSumOfWeights;
    }
    else if(item1->m_priority < item2->m_priority)
    {
        if(item2->m_easeSpinner == 1.0f)
        {
            item1->m_normalizedWeight = 0.0f;
            item2->m_normalizedWeight = 1.0f;
            return;
        }

        cjFLOAT oneMinusEaseSpinner = 1.0f - item2->m_easeSpinner;
        cjFLOAT oneOverSumOfWeights = 1.0f / (item2->m_easeSpinner * realWeight2 + oneMinusEaseSpinner * realWeight1);
        item1->m_normalizedWeight = oneMinusEaseSpinner * realWeight1 * oneOverSumOfWeights;
        item2->m_normalizedWeight = item2->m_easeSpinner * realWeight2 * oneOverSumOfWeights;
    }
    else
    {
        cjFLOAT oneOverSumOfWeights = 1.0f / (realWeight1 + realWeight2);
        item1->m_normalizedWeight = realWeight1 * oneOverSumOfWeights;
        item2->m_normalizedWeight = realWeight2 * oneOverSumOfWeights;
    }

    if(IsUseHighestWeightOnly())
    {
        if(item1->m_normalizedWeight >= item2->m_normalizedWeight)
        {
            item1->m_normalizedWeight = 1.0f;
            item2->m_normalizedWeight = 0.0f;
        }
        else
        {
            item1->m_normalizedWeight = 0.0f;
            item2->m_normalizedWeight = 1.0f;
        }

        return;
    }

    if(m_weightThreshold > 0.0f)
    {
        cjBool reduced1 = cjFalse;
        if(item1->m_normalizedWeight < m_weightThreshold)
        {
            item1->m_normalizedWeight = 0.0f;
            reduced1 = cjTrue;
        }

        cjBool reduced2 = cjFalse;
        if(item2->m_normalizedWeight < m_weightThreshold)
        {
            item2->m_normalizedWeight = 0.0f;
            reduced2 = cjTrue;
        }

        if(reduced1 && reduced2)
        {
            return;
        }
        else if(reduced1)
        {
            item2->m_normalizedWeight = 1.0f;
            return;
        }
        else if(reduced2)
        {
            item1->m_normalizedWeight = 1.0f;
            return;
        }
    }
}

void cjBlendInterpolator::Collapse()
{
    if(m_interpCount == 1)
    {
        m_singleInterpolator->Collapse();
        return;
    }

    for(cjBYTE i=0; i<m_arySiz; i++)
    {
        cjBlendInterpItem& item = m_interpAry[i];

        if(item.m_spInterpolator)
            item.m_spInterpolator->Collapse();
    }
}

void cjBlendInterpolator::Cache(cjFLOAT stTime, cjFLOAT enTime)
{
    if(m_interpCount == 1)
    {
        m_singleInterpolator->Cache(stTime, enTime);
        return;
    }

    for(cjBYTE i=0; i< m_arySiz; i++)
    {
        cjBlendInterpItem& item = m_interpAry[i];

		if(item.m_spInterpolator)
            item.m_spInterpolator->Cache(stTime, enTime);
    }
}

void cjBlendInterpolator::ClearWeightSums()
{
    m_highSumOfWeights		= CJ_FLT_INVALID;
    m_nextHighSumOfWeights	= CJ_FLT_INVALID;
    m_highEaseSpinner		= CJ_FLT_INVALID;
}

cjBool cjBlendInterpolator::GetSingleUpdateTime(cjFLOAT& etime)
{
	cjAssert(m_singleIdx != INVALID_INDEX && m_singleInterpolator);
    
    if(IsManagerControlled())
        etime = m_singleTime;

    if(etime == CJ_FTIME_INVALID)
        return cjFalse;

    return cjTrue;
}

cjBool cjBlendInterpolator::GetUpdateTimeForItem(cjFLOAT& etime, cjBlendInterpItem& item)
{
    cjInterpolator* interpolator = item.m_spInterpolator;

    if(interpolator && item.m_normalizedWeight != 0.0f)
    {
        if(IsManagerControlled())
            etime = item.m_updateTime;

        if(etime == CJ_FTIME_INVALID)
            return cjFalse;

		return cjTrue;
    }

	return cjFalse;
}


//
// cjBlendTMInterpolator
//

CJSER_IMPL_CLASS(cjBlendTMInterpolator, cjBlendInterpolator)

CJSER_IMPL_SERIALIZE_BEGIN(cjBlendTMInterpolator)
CJSER_IMPL_SERIALIZE_END(cjBlendTMInterpolator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjBlendTMInterpolator)
CJSER_IMPL_CLONE_PROPCOPY_END(cjBlendTMInterpolator)

CJSER_IMPL_CLONE(cjBlendTMInterpolator)


cjBlendTMInterpolator::cjBlendTMInterpolator()
{
}

cjBlendTMInterpolator::cjBlendTMInterpolator(cjBool managerControlled, cjFLOAT weightThreshold, cjBYTE arySiz) 
	: cjBlendInterpolator(managerControlled, weightThreshold, arySiz)
{
}

cjBlendTMInterpolator::~cjBlendTMInterpolator()
{
}

cjBool cjBlendTMInterpolator::Update(cjFLOAT etime, cjSceneNode* target, cjQuatTM& v)
{
    cjBool r = cjFalse;

    if(m_interpCount == 1)
    {
        r = StoreSingleValue(etime, target, v);
    }
    else if(m_interpCount > 0)
    {
        ComputeNormalizedWeights();
        r = BlendValues(etime, target, v);
    }
    
    m_lastTime = etime;

    return r;
}

cjBool cjBlendTMInterpolator::CheckInterpolatorType(cjInterpolator* interpolator) const
{
    return interpolator->IsTMValue();
}

cjBool cjBlendTMInterpolator::StoreSingleValue(cjFLOAT etime, cjSceneNode* interpTarget, cjQuatTM& tm)
{
    if(!GetSingleUpdateTime(etime))
    {
        tm.MakeInvalid();

        return cjFalse;
    }

    cjAssert(m_singleInterpolator);

    if(m_singleInterpolator->Update(etime, interpTarget, tm))
    {
        return cjTrue;
    }
    else
    {
        tm.MakeInvalid();
        return cjFalse;
    }
}

cjBool cjBlendTMInterpolator::BlendValues(cjFLOAT etime, cjSceneNode* interpTarget, cjQuatTM& rstTM)
{
    cjFLOAT totalPosWeight = 1.0f;
    cjFLOAT totalScaleWeight = 1.0f;

    cjVec3	finalPos		= cjVec3::ZERO;
    cjQuat	finalRotate		= cjQuat(0.0f, 0.0f, 0.0f, 0.0f);
    cjVec3	finalScale		= cjVec3::UNIT;

    cjBool posChanged		= cjFalse;
    cjBool rotChanged		= cjFalse;
    cjBool scaleChanged		= cjFalse;
    cjBool firstRotation	= cjTrue;
	
    for(cjBYTE i=0; i<m_arySiz; i++)
    {
        cjBlendInterpItem& item = m_interpAry[i];

        if(item.m_spInterpolator && item.m_normalizedWeight > 0.0f)
        {
            cjFLOAT updateTime = etime;

            if(!GetUpdateTimeForItem(updateTime, item))
            {
                totalPosWeight		-= item.m_normalizedWeight;
                totalScaleWeight	-= item.m_normalizedWeight;

                continue;
            }

            cjQuatTM curTM;
            cjBool r = item.m_spInterpolator->Update(updateTime, interpTarget, curTM);

            if(r)
            {
                if(curTM.IsPosValid())
                {
                    finalPos += curTM.GetPos() * item.m_normalizedWeight;
                    posChanged = cjTrue;
                }
                else
                {
                    totalPosWeight -= item.m_normalizedWeight; // invalid pos, so remove
                }

                if(curTM.IsRotateValid())
                {
                    cjQuat rot = curTM.GetRotate();

                    if(!firstRotation)
                    {
                        cjFLOAT ang = cjQuat::Dot(finalRotate, rot);

                        if(ang < 0.0f)
                            rot = -rot; // neg angle, so invert quaternion to get best path
                    }
                    else
                    {
                        firstRotation = cjFalse;
                    }

                    rot = rot * item.m_normalizedWeight; // multiply, and non-rotation status

                    // accum rotation					
                    rot.x += finalRotate.x;
                    rot.y += finalRotate.y;
                    rot.z += finalRotate.z;
					rot.w += finalRotate.w;
					                    
                    rotChanged = cjTrue; // need to re-normalize
                }

                if(curTM.IsScaleValid())
                {
                    finalScale += curTM.GetScale() * item.m_normalizedWeight;
                    scaleChanged = cjTrue;
                }
                else 
                {
                    totalScaleWeight -= item.m_normalizedWeight; // invalid scale, so remove
                }
            }
            else
            {
				// update fail, so remove
                totalPosWeight	 -= item.m_normalizedWeight;
                totalScaleWeight -= item.m_normalizedWeight;
            }
        }
    }

    rstTM.MakeInvalid();

    if(posChanged || rotChanged || scaleChanged)
    {
        if(posChanged)
        {
            cjAssert(totalPosWeight != 0.0f);

            finalPos /= totalPosWeight;
            rstTM.SetPos(finalPos);
        }

		if(rotChanged)
        {
			// summed quaternion is non-unit quaternion, not rotation, which is needed to normalize
            finalRotate.Normalize();
            rstTM.SetRotate(finalRotate);
        }

        if(scaleChanged)
        {
            // remove the effect of invalid scales
            cjAssert(totalScaleWeight != 0.0f);

            finalScale /= totalScaleWeight;
            rstTM.SetScale(finalScale);
        }
    }

    if(rstTM.IsTMInvalid())
        return cjFalse;

    return cjTrue;
}


//
// cjBlendInterpolator 
//   - cjBlendBoolInterpolator
//

CJSER_IMPL_CLASS(cjBlendBoolInterpolator, cjBlendInterpolator)

CJSER_IMPL_SERIALIZE_BEGIN(cjBlendBoolInterpolator)

	if(ser.IsQuerySerObjAndStr())
	{
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		ser << m_value;
	}

CJSER_IMPL_SERIALIZE_END(cjBlendBoolInterpolator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjBlendBoolInterpolator)
	d->m_value = m_value;
CJSER_IMPL_CLONE_PROPCOPY_END(cjBlendBoolInterpolator)

CJSER_IMPL_CLONE(cjBlendBoolInterpolator)


cjBlendBoolInterpolator::cjBlendBoolInterpolator()
{
	m_value = cjFalse;
}

cjBlendBoolInterpolator::cjBlendBoolInterpolator(cjBool managerControlled, cjFLOAT weightThreshold, cjBYTE arySiz) 
	:  cjBlendInterpolator(managerControlled, weightThreshold, arySiz)
{
	m_value = cjFalse;
}

cjBlendBoolInterpolator::~cjBlendBoolInterpolator()
{
}

cjBool cjBlendBoolInterpolator::Update(cjFLOAT etime, cjRefObj* target, cjBool& v)
{
	cjBool rst = cjFalse;

	if(m_interpCount == 1)
	{
		rst = StoreSingleValue(etime, target, v);
	}
	else if(m_interpCount > 0)
	{
		ComputeNormalizedWeights();       
		rst = BlendValues(etime, target, v);
	}

	m_lastTime = etime;

	return rst;
}

cjBool cjBlendBoolInterpolator::StoreSingleValue(cjFLOAT etime, cjRefObj* target, cjBool& v)
{
	if(!GetSingleUpdateTime(etime))
		return cjFalse;

	cjAssert(m_singleInterpolator != cjNULL);
	cjBool rst = m_singleInterpolator->Update(etime, target, v);

	if(rst)
	{
		m_value = v;
		return cjTrue;
	}
	else
	{
		v = cjFalse;
		m_value = cjFalse;

		return cjFalse;
	}
}

cjBool cjBlendBoolInterpolator::BlendValues(cjFLOAT etime, cjRefObj* target, cjBool& v)
{
	cjBool modifiedValue = cjFalse;
	cjFLOAT blendValue = 0.0f;
	cjFLOAT totalNormalizedWeights = 1.0f;

	for(cjBYTE c=0; c<m_arySiz; c++)
	{
		cjInterpolator* interpolator = m_interpAry[c].m_spInterpolator;

		if(interpolator && m_interpAry[c].m_normalizedWeight > 0.0f)
		{
			cjBool bVal;
			cjFLOAT fUpdateTime = etime;

			if(!GetUpdateTimeForItem(fUpdateTime, m_interpAry[c]))
			{
				totalNormalizedWeights -= m_interpAry[c].m_normalizedWeight;
				continue;
			}

			cjBool bSuccess = interpolator->Update(fUpdateTime, target, bVal);

			if(bSuccess)
			{
				cjFLOAT fValue = (cjFLOAT) bVal;
				blendValue += fValue * m_interpAry[c].m_normalizedWeight;
				modifiedValue = cjTrue;
			}
			else
			{
				totalNormalizedWeights -= m_interpAry[c].m_normalizedWeight;
			}
		}
	}

	blendValue /= totalNormalizedWeights;

	if(!modifiedValue)
	{
		v = cjFalse;
		m_value = cjFalse;

		return cjFalse;
	}
	else if(blendValue < 0.5f)
	{
		m_value = cjFalse;
		v = cjFalse;

		return cjTrue;
	}
	else
	{
		m_value = cjTrue;
		v = cjTrue;

		return cjTrue;
	}
}


//
// cjBSInterpolator - cjInterpolator
//

CJSER_IMPL_CLASS(cjBSInterpolator, cjInterpolator);

CJSER_IMPL_SERIALIZE_BEGIN(cjBSInterpolator)

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

CJSER_IMPL_SERIALIZE_END(cjBSInterpolator)

CJSER_IMPL_SERIALIZE_POST_BEGIN(cjBSInterpolator)

	if(ser.IsLoading())
	{
		ser.PostSerializeSerObjPtr(m_spDat);
		ser.PostSerializeSerObjPtr(m_spBasisDat);
	}

CJSER_IMPL_SERIALIZE_POST_END(cjBSInterpolator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjBSInterpolator)
 
	d->m_stTime = m_stTime;
	d->m_enTime = m_enTime;

	d->m_spDat = m_spDat;

	// clone cached value
	if(m_spBasisDat)
		d->m_spBasisDat = (cjBSplineBasisF3Dat*)m_spBasisDat->_CloneShared(cloner);
		
CJSER_IMPL_CLONE_PROPCOPY_END(cjBSInterpolator)

cjBSInterpolator::cjBSInterpolator()
{
	m_stTime = CJ_FLT_INFINITY;
    m_enTime = CJ_FLT_INVALID;

	m_spDat = cjNULL;
	m_spBasisDat = cjNULL;
}

cjBSInterpolator::cjBSInterpolator(cjBSplineDat* bsDat, cjBSplineBasisF3Dat* basisDat)
{
	m_stTime = CJ_FLT_INFINITY;
    m_enTime = CJ_FLT_INVALID;

	m_spDat = bsDat;
	m_spBasisDat = basisDat;
}

cjBSInterpolator::~cjBSInterpolator()
{
}

void cjBSInterpolator::Collapse()
{
}

void cjBSInterpolator::GetValidTimeRange(cjFLOAT& stTime, cjFLOAT& enTime) const
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

void cjBSInterpolator::Cache(cjFLOAT, cjFLOAT)
{
	cjAssert(!"ERR! - not supported..!");
}

cjInterpolator* cjBSInterpolator::GetInterpolator(cjFLOAT, cjFLOAT)
{
    cjAssert(!"ERR! - not supported..!");
    return cjNULL;
}

cjUINT32 cjBSInterpolator::GetAllocSiz(cjUINT32 channel) const
{
    cjAssert(channel < GetChannelCount());

    if(IsCompressedCP())
        return GetCPCount(channel) * GetFloatValCount(channel) * sizeof(cjSWORD);
    else
        return GetCPCount(channel) * GetFloatValCount(channel) * sizeof(cjFLOAT);
}

cjUINT32 cjBSInterpolator::GetCPCount(cjUINT32 channel) const
{
    if(channel >= GetChannelCount())
        return 0;

    if(m_spDat == 0 || cjBSplineDat::INVALID_BSOFFSET == GetCPOffset(channel))
        return 0;

    cjBSplineBasisF3& bsBasisF3 = m_spBasisDat->GetBasis();

    return bsBasisF3.GetControlPointCount();
}


//
// cjBSColorInterpolator - cjBSInterpolator
//

CJSER_IMPL_CLASS(cjBSColorInterpolator, cjBSInterpolator);

CJSER_IMPL_SERIALIZE_BEGIN(cjBSColorInterpolator)

	if(ser.IsQuerySerObjAndStr())
	{
		return;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		ser << m_value;
		ser << m_colorCPOffset;
	}

CJSER_IMPL_SERIALIZE_END(cjBSColorInterpolator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjBSColorInterpolator)
	d->m_value = m_value;
	d->m_colorCPOffset = m_colorCPOffset; 
CJSER_IMPL_CLONE_PROPCOPY_END(cjBSColorInterpolator)

CJSER_IMPL_CLONE(cjBSColorInterpolator)


cjBSColorInterpolator::cjBSColorInterpolator()
{
	m_colorCPOffset = cjBSplineDat::INVALID_BSOFFSET;
}

cjBSColorInterpolator::cjBSColorInterpolator(const cjColorF& poseValue)
{
    m_value = poseValue; 
    m_colorCPOffset = cjBSplineDat::INVALID_BSOFFSET;
}

cjBSColorInterpolator::cjBSColorInterpolator(cjBSplineDat* bsDat, cjBSplineDat::BSOffset colorCPOffset,
    cjBSplineBasisF3Dat* basisDat) : cjBSInterpolator(bsDat, basisDat) 
{
	m_colorCPOffset = colorCPOffset;
}

cjBSColorInterpolator::~cjBSColorInterpolator()
{
}

cjBool cjBSColorInterpolator::Update(cjFLOAT etime, cjRefObj*, cjColorF& value)
{
    if(!TimeHasChanged(etime))
    {
        value = m_value;
        return cjTrue;
    }

    if(cjBSplineDat::INVALID_BSOFFSET != m_colorCPOffset)
    {        
        cjFLOAT ratio = (etime - m_stTime)/(m_enTime - m_stTime);
        cjFLOAT bsValues[4];

        m_spDat->GetValueF3(ratio, (cjFLOAT*)bsValues, 4, m_spBasisDat, m_colorCPOffset);
        m_value = cjColorF(bsValues[0], bsValues[1], bsValues[2], bsValues[3]);
    }
    
    value = m_value;
    m_lastTime = etime;

    return cjTrue;
}

cjBool cjBSColorInterpolator::GetChannelPosed(cjUINT32 channel) const
{
    if(cjAniChan_Base == channel)
        return (GetCPCount(channel) <= 0);
    else
        return cjFalse;
}

cjUINT32 cjBSColorInterpolator::GetFloatValCount(cjUINT32 channel) const
{
    if(cjAniChan_Base == channel)
        return 4;
    else
        return 0;
}

cjUINT32 cjBSColorInterpolator::GetDegree(cjUINT32 channel) const
{
    if(cjAniChan_Base == channel)
        return 3;
    else
        return 0;
}

cjBSplineDat::BSOffset cjBSColorInterpolator::GetCPOffset(cjUINT32 channel) const
{
	if(channel == cjAniChan_Base)
		return m_colorCPOffset;

	return cjBSplineDat::INVALID_BSOFFSET;
}

void cjBSColorInterpolator::SetCPOffset(cjBSplineDat::BSOffset cpOffset, cjUINT32 channel)
{
	if(channel == cjAniChan_Base)
		m_colorCPOffset = cpOffset;
}

void cjBSColorInterpolator::SetPoseValue(cjColorF& poseValue)
{
    m_value = poseValue;

    if(m_spDat)
        m_colorCPOffset = cjBSplineDat::INVALID_BSOFFSET;

    m_spDat = cjNULL;
}


//
// cjBSFloatInterpolator - cjBSInterpolator
//

CJSER_IMPL_CLASS(cjBSFloatInterpolator, cjBSInterpolator);

CJSER_IMPL_SERIALIZE_BEGIN(cjBSFloatInterpolator)

	if(ser.IsQuerySerObjAndStr())
	{
		return;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		ser << m_value;
		ser << m_valueCPOffset;
	}

CJSER_IMPL_SERIALIZE_END(cjBSFloatInterpolator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjBSFloatInterpolator)
	d->m_value = m_value;
	d->m_valueCPOffset = m_valueCPOffset; 
CJSER_IMPL_CLONE_PROPCOPY_END(cjBSFloatInterpolator)

CJSER_IMPL_CLONE(cjBSFloatInterpolator)
	

cjBSFloatInterpolator::cjBSFloatInterpolator()
{
	m_valueCPOffset = cjBSplineDat::INVALID_BSOFFSET;
}

cjBSFloatInterpolator::cjBSFloatInterpolator(cjFLOAT poseValue)
{
    m_value = poseValue;
    m_valueCPOffset = cjBSplineDat::INVALID_BSOFFSET;
}

cjBSFloatInterpolator::cjBSFloatInterpolator(cjBSplineDat* bsDat, cjBSplineDat::BSOffset floatCPOffset,
    cjBSplineBasisF3Dat* basisDat) : cjBSInterpolator(bsDat, basisDat) 
{	
	m_valueCPOffset = floatCPOffset;
}

cjBSFloatInterpolator::~cjBSFloatInterpolator()
{
}

cjBool cjBSFloatInterpolator::Update(cjFLOAT etime, cjRefObj* target, cjFLOAT& value)
{
    if(!TimeHasChanged(etime))
    {
        value = m_value;
        return cjTrue;
    }

    if(cjBSplineDat::INVALID_BSOFFSET != m_valueCPOffset)
    {
        cjFLOAT ratio = (etime - m_stTime)/(m_enTime - m_stTime);

        cjAssert(m_spDat);
        cjAssert(m_spBasisDat);

        m_spDat->GetValueF3(ratio, &m_value, 1, m_spBasisDat, m_valueCPOffset);
    }
    
    value = m_value;
    m_lastTime = etime;

    return cjTrue;
}

cjBool cjBSFloatInterpolator::GetChannelPosed(cjUINT32 channel) const
{
    if(cjAniChan_Base == channel)
        return (GetCPCount(channel) <= 0);
    else
        return cjFalse;
}

cjUINT32 cjBSFloatInterpolator::GetFloatValCount(cjUINT32 channel) const
{
    if(cjAniChan_Base == channel)
        return 1;
    else
        return 0;
}

cjUINT32 cjBSFloatInterpolator::GetDegree(cjUINT32 channel) const
{
    if(cjAniChan_Base == channel)
        return 3;
    else
        return 0;
}

cjBSplineDat::BSOffset cjBSFloatInterpolator::GetCPOffset(cjUINT32 channel) const
{
	if(channel == cjAniChan_Base)
		return m_valueCPOffset;

    return cjBSplineDat::INVALID_BSOFFSET;
}

void cjBSFloatInterpolator::SetCPOffset(cjBSplineDat::BSOffset cpOffset, cjUINT32 channel)
{
	if(channel == cjAniChan_Base)
		m_valueCPOffset = cpOffset;
}

void cjBSFloatInterpolator::SetPoseValue(cjFLOAT poseValue)
{
    m_value = poseValue;

    if(m_spDat)
        m_valueCPOffset = cjBSplineDat::INVALID_BSOFFSET;

    m_spDat = cjNULL;
}


//
// cjBSVec3SInterpolator - cjBSInterpolator
//

CJSER_IMPL_CLASS(cjBSVec3SInterpolator, cjBSInterpolator);

CJSER_IMPL_SERIALIZE_BEGIN(cjBSVec3SInterpolator)

	if(ser.IsQuerySerObjAndStr())
	{
		return;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		ser << m_value;
		ser << m_vec3CPOffset;
	}

CJSER_IMPL_SERIALIZE_END(cjBSVec3SInterpolator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjBSVec3SInterpolator)
	d->m_value = m_value;
	d->m_vec3CPOffset = m_vec3CPOffset;
CJSER_IMPL_CLONE_PROPCOPY_END(cjBSVec3SInterpolator)

CJSER_IMPL_CLONE(cjBSVec3SInterpolator)


cjBSVec3SInterpolator::cjBSVec3SInterpolator()
{
	m_vec3CPOffset = cjBSplineDat::INVALID_BSOFFSET;
}

cjBSVec3SInterpolator::cjBSVec3SInterpolator(cjBSplineDat* bsDat, cjBSplineDat::BSOffset vec3CPOffset,
    cjBSplineBasisF3Dat* basisDat) : cjBSInterpolator(bsDat, basisDat) 
{
	m_vec3CPOffset = vec3CPOffset;
}

cjBSVec3SInterpolator::cjBSVec3SInterpolator(const cjVec3& poseValue)
{
    m_value = poseValue; 
    m_vec3CPOffset = cjBSplineDat::INVALID_BSOFFSET;
}

cjBSVec3SInterpolator::~cjBSVec3SInterpolator()
{
}

cjBool cjBSVec3SInterpolator::Update(cjFLOAT etime, cjRefObj*, cjVec3& value)
{
    if(!TimeHasChanged(etime))
    {
        value = m_value;
        return cjTrue;
    }

    if(cjBSplineDat::INVALID_BSOFFSET != m_vec3CPOffset)
    {        
        cjFLOAT ratio = (etime - m_stTime)/(m_enTime - m_stTime);
        cjFLOAT bsValues[3];

        m_spDat->GetValueF3(ratio, (cjFLOAT*)bsValues, 3, m_spBasisDat, m_vec3CPOffset);
        m_value = cjVec3(bsValues[0], bsValues[1], bsValues[2]);
    }
    
    value = m_value;
    m_lastTime = etime;

    return cjTrue;
}

cjBool cjBSVec3SInterpolator::GetChannelPosed(cjUINT32 channel) const
{
    if(cjAniChan_Base == channel)
        return (GetCPCount(channel) <= 0);
    else
        return cjFalse;
}

cjUINT32 cjBSVec3SInterpolator::GetFloatValCount(cjUINT32 channel) const
{
    if(cjAniChan_Base == channel)
        return 3;
    else
        return 0;
}

cjUINT32 cjBSVec3SInterpolator::GetDegree(cjUINT32 channel) const
{
    if(cjAniChan_Base == channel)
        return 3;
    else
        return 0;
}

cjBSplineDat::BSOffset cjBSVec3SInterpolator::GetCPOffset(cjUINT32 channel) const
{
	if(channel == cjAniChan_Base)
		return m_vec3CPOffset;

	return cjBSplineDat::INVALID_BSOFFSET;
}

void cjBSVec3SInterpolator::SetCPOffset(cjBSplineDat::BSOffset cpOffset, cjUINT32 channel)
{
	if(channel == cjAniChan_Base)
		m_vec3CPOffset = cpOffset;
}

void cjBSVec3SInterpolator::SetPoseValue(const cjVec3& poseValue)
{
    m_value = poseValue;

    if(m_spDat)
        m_vec3CPOffset = cjBSplineDat::INVALID_BSOFFSET;

    m_spDat = cjNULL;
}


//
// cjBSVec3PInterpolator - cjBSInterpolator
//

CJSER_IMPL_CLASS(cjBSVec3PInterpolator, cjBSInterpolator);

CJSER_IMPL_SERIALIZE_BEGIN(cjBSVec3PInterpolator)

	if(ser.IsQuerySerObjAndStr())
	{
		return;
	}
	else if(ser.IsLoading() || ser.IsSaving())
	{
		ser << m_value;
		ser << m_vec3CPOffset;
	}

CJSER_IMPL_SERIALIZE_END(cjBSVec3PInterpolator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjBSVec3PInterpolator)
	d->m_value = m_value;
	d->m_vec3CPOffset = m_vec3CPOffset;
CJSER_IMPL_CLONE_PROPCOPY_END(cjBSVec3PInterpolator)

CJSER_IMPL_CLONE(cjBSVec3PInterpolator)


cjBSVec3PInterpolator::cjBSVec3PInterpolator()
{
	m_vec3CPOffset = cjBSplineDat::INVALID_BSOFFSET;
}

cjBSVec3PInterpolator::cjBSVec3PInterpolator(cjBSplineDat* bsDat, cjBSplineDat::BSOffset vec3CPOffset,
    cjBSplineBasisF3Dat* basisDat) : cjBSInterpolator(bsDat, basisDat) 
{
	m_vec3CPOffset = vec3CPOffset;
}

cjBSVec3PInterpolator::cjBSVec3PInterpolator(const cjVec3& poseValue)
{
    m_value = poseValue; 
    m_vec3CPOffset = cjBSplineDat::INVALID_BSOFFSET;
}

cjBSVec3PInterpolator::~cjBSVec3PInterpolator()
{
}

cjBool cjBSVec3PInterpolator::Update(cjFLOAT etime, cjRefObj*, cjVec3& value)
{
    if(!TimeHasChanged(etime))
    {
        value = m_value;
        return cjTrue;
    }

    if(cjBSplineDat::INVALID_BSOFFSET != m_vec3CPOffset)
    {        
        cjFLOAT ratio = (etime - m_stTime)/(m_enTime - m_stTime);
        cjFLOAT bsValues[3];

        m_spDat->GetValueF3(ratio, (cjFLOAT*)bsValues, 3, m_spBasisDat, m_vec3CPOffset);
        m_value = cjVec3(bsValues[0], bsValues[1], bsValues[2]);
    }
    
    value = m_value;
    m_lastTime = etime;

    return cjTrue;
}

cjBool cjBSVec3PInterpolator::GetChannelPosed(cjUINT32 channel) const
{
    if(cjAniChan_Base == channel)
        return (GetCPCount(channel) <= 0);
    else
        return cjFalse;
}

cjUINT32 cjBSVec3PInterpolator::GetFloatValCount(cjUINT32 channel) const
{
    if(cjAniChan_Base == channel)
        return 3;
    else
        return 0;
}

cjUINT32 cjBSVec3PInterpolator::GetDegree(cjUINT32 channel) const
{
    if(cjAniChan_Base == channel)
        return 3;
    else
        return 0;
}

cjBSplineDat::BSOffset cjBSVec3PInterpolator::GetCPOffset(cjUINT32 channel) const
{
	if(channel == cjAniChan_Base)
		return m_vec3CPOffset;

	return cjBSplineDat::INVALID_BSOFFSET;
}

void cjBSVec3PInterpolator::SetCPOffset(cjBSplineDat::BSOffset cpOffset, cjUINT32 channel)
{
	if(channel == cjAniChan_Base)
		m_vec3CPOffset = cpOffset;
}

void cjBSVec3PInterpolator::SetPoseValue(const cjVec3& poseValue)
{
    m_value = poseValue;

    if(m_spDat)
        m_vec3CPOffset = cjBSplineDat::INVALID_BSOFFSET;

    m_spDat = cjNULL;
}


//
// cjBSTMInterpolator
//

CJSER_IMPL_CLASS(cjBSTMInterpolator, cjBSInterpolator);

CJSER_IMPL_SERIALIZE_BEGIN(cjBSTMInterpolator)

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

CJSER_IMPL_SERIALIZE_END(cjBSTMInterpolator)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjBSTMInterpolator)
	d->m_value = m_value;
	d->m_posCPOffset = m_posCPOffset;
	d->m_rotCPOffset = m_rotCPOffset;
	d->m_scaleCPOffset = m_scaleCPOffset;
CJSER_IMPL_CLONE_PROPCOPY_END(cjBSTMInterpolator)

CJSER_IMPL_CLONE(cjBSTMInterpolator)


cjBSTMInterpolator::cjBSTMInterpolator()
{
    m_posCPOffset = cjBSplineDat::INVALID_BSOFFSET; 
	m_rotCPOffset = cjBSplineDat::INVALID_BSOFFSET;
    m_scaleCPOffset = cjBSplineDat::INVALID_BSOFFSET;
}

cjBSTMInterpolator::cjBSTMInterpolator(const cjQuatTM& poseValue)
{
    m_value = poseValue;

    m_posCPOffset = cjBSplineDat::INVALID_BSOFFSET;
    m_rotCPOffset = cjBSplineDat::INVALID_BSOFFSET;
    m_scaleCPOffset = cjBSplineDat::INVALID_BSOFFSET;
}

cjBSTMInterpolator::cjBSTMInterpolator(cjBSplineDat* bsDat, cjBSplineDat::BSOffset posCPOffset,
    cjBSplineDat::BSOffset rotCPOffset, cjBSplineDat::BSOffset scaleCPOffset, cjBSplineBasisF3Dat* basisDat) 
	: cjBSInterpolator(bsDat, basisDat)
{
	m_posCPOffset = posCPOffset;
	m_rotCPOffset = rotCPOffset;
	m_scaleCPOffset = scaleCPOffset;
}

cjBSTMInterpolator::~cjBSTMInterpolator()
{
}

cjBool cjBSTMInterpolator::Update(cjFLOAT etime, cjRefObj*, cjQuatTM& value)
{
    if(!TimeHasChanged(etime))
    {
        value = m_value;
            
        if(m_value.IsTMInvalid())
            return cjFalse;

        return cjTrue;
    }

    cjFLOAT ratio = (etime - m_stTime)/(m_enTime - m_stTime);
    cjFLOAT bsValues[4];

    if(cjBSplineDat::INVALID_BSOFFSET != m_posCPOffset)
    {
        m_spDat->GetValueF3(ratio, (cjFLOAT*)bsValues, 3, m_spBasisDat, m_posCPOffset);
        m_value.SetPos(cjVec3(bsValues[0], bsValues[1], bsValues[2]));
    }

    if(cjBSplineDat::INVALID_BSOFFSET != m_rotCPOffset)
    {
		m_spDat->GetValueF3(ratio, (cjFLOAT*)bsValues, 4, m_spBasisDat, m_rotCPOffset);

        cjQuat quat(bsValues[0], bsValues[1], bsValues[2], bsValues[3]);

		//cjopti
        quat.Normalize();
        m_value.SetRotate(quat);
    }

    if(cjBSplineDat::INVALID_BSOFFSET != m_scaleCPOffset)
    {
        m_spDat->GetValueF3(ratio, (cjFLOAT*)bsValues, 1, m_spBasisDat, m_scaleCPOffset); 
        m_value.SetScale(cjVec3(bsValues[0], bsValues[1], bsValues[2]));
    }
    
    value = m_value;

    if(m_value.IsTMInvalid())
        return cjFalse;
	
    m_lastTime = etime;

    return cjTrue;
}

cjBool cjBSTMInterpolator::GetChannelPosed(cjUINT32 channel) const
{
    switch(channel)
    {
        case cjAniChan_Pos:
			{
				if(GetCPCount(channel) > 0)
					return cjFalse;
				else 
					return m_value.IsPosValid();
			}
			break;

        case cjAniChan_Rot:
			{
				if(GetCPCount(channel) > 0)
					return cjFalse;
				else 
					return m_value.IsRotateValid();
			}
			break;

        case cjAniChan_Scale:
			{
				if(GetCPCount(channel) > 0)
					return cjFalse;
				else 
					return m_value.IsScaleValid();
			}
			break;

        default:
            return cjFalse;
    }
}

cjUINT32 cjBSTMInterpolator::GetFloatValCount(cjUINT32 channel) const
{
    switch(channel)
    {
        case cjAniChan_Pos:		return 3;
        case cjAniChan_Rot:		return 4; // quat
        case cjAniChan_Scale:	return 3; // vector3
        default:
            return 0;
    }
}

cjUINT32 cjBSTMInterpolator::GetDegree(cjUINT32 channel) const
{
    switch(channel)
    {
        case cjAniChan_Pos:		return 3;
        case cjAniChan_Rot:		return 3;
        case cjAniChan_Scale:	return 3;
        default:
            return 0;
    }
}

void cjBSTMInterpolator::SetPosePos(const cjVec3& pos)
{
    m_value.SetPos(pos);

    if(m_spDat)
        m_posCPOffset = cjBSplineDat::INVALID_BSOFFSET;
}

void cjBSTMInterpolator::SetPoseRotate(const cjQuat& rot)
{
    m_value.SetRotate(rot);

    if(m_spDat)
        m_rotCPOffset = cjBSplineDat::INVALID_BSOFFSET;
}

void cjBSTMInterpolator::SetPoseScale(const cjVec3& scale)
{
    m_value.SetScale(scale);

    if(m_spDat)
        m_scaleCPOffset = cjBSplineDat::INVALID_BSOFFSET;
}

void cjBSTMInterpolator::SetPoseValue(const cjQuatTM& poseValue)
{
    SetPosePos(poseValue.GetPos());
    SetPoseRotate(poseValue.GetRotate());
    SetPoseScale(poseValue.GetScale());

    m_spDat = cjNULL;
}

cjBSplineDat::BSOffset cjBSTMInterpolator::GetCPOffset(cjUINT32 channel) const
{
    switch(channel)
    {
        case cjAniChan_Pos:		return m_posCPOffset;
        case cjAniChan_Rot:		return m_rotCPOffset;
        case cjAniChan_Scale:	return m_scaleCPOffset;
        default:		
			return cjBSplineDat::INVALID_BSOFFSET;
    }
}

void cjBSTMInterpolator::SetCPOffset(cjBSplineDat::BSOffset cpOffset, cjUINT32 channel)
{
    switch(channel)
    {
        case cjAniChan_Pos:		m_posCPOffset = cpOffset;	break;
        case cjAniChan_Rot:		m_rotCPOffset = cpOffset;	break;
        case cjAniChan_Scale:	m_scaleCPOffset = cpOffset;	break;
		default:
			cjAssert(!"!ERR - cjBSTMInterpolator::SetCPOffset..");
			break;
    }
}
