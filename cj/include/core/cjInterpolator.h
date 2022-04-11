
#ifndef _CJINTERPOLATOR_H_
#define _CJINTERPOLATOR_H_

/*
	cjInterpolator
	- cjLookAtInterpolator
	- cjBlendInterpolator
	- cjBSplineInterpolator
	- cjKeyBasedInterpolator
		- cjBoolInterpolator
		- cjColorInterpolator
		- cjFloatInterpolator
		- cjPathInterpolator
		- cjVec3Interpolator
		- cjQuatInterpolator
		- cjTMInterpolator		
*/

class cjSceneNode;
class cjRefObjNameHash;
class cjRefObj;


//
// cjInterpolator
//

class CJCORE_API cjInterpolator : public cjRef
{
	CJSER_DECL_CLASS(cjInterpolator, cjRef,0 , CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE_ABSTRACT(cjInterpolator)

public:
    virtual cjBool Update(cjFLOAT /*etime*/	, cjRefObj* /*target*/	, cjBool&	 /*v*/	) { return cjFalse; }
    virtual cjBool Update(cjFLOAT			, cjRefObj*				, cjFLOAT&			) { return cjFalse; }
    virtual cjBool Update(cjFLOAT			, cjRefObj*				, cjQuat&			) { return cjFalse; }
    virtual cjBool Update(cjFLOAT			, cjRefObj*				, cjVec3&			) { return cjFalse; }
	virtual cjBool Update(cjFLOAT			, cjRefObj*				, cjColorF&			) { return cjFalse; }
	virtual cjBool Update(cjFLOAT			, cjRefObj*				, cjQuatTM&			) { return cjFalse; }

    virtual cjBool IsBoolValue()	const { return cjFalse; }
    virtual cjBool IsFloatValue()	const { return cjFalse; }
    virtual cjBool IsQuatValue()	const { return cjFalse; }
    virtual cjBool IsVec3Value()	const { return cjFalse; }
    virtual cjBool IsColorValue()	const { return cjFalse; }
    virtual cjBool IsTMValue()		const { return cjFalse; }

	virtual void Collapse() {}   
	void ForceNextUpdate() { m_lastTime = CJ_FTIME_INVALID; }
	
public:	
	// internal use
    virtual void GetValidTimeRange(cjFLOAT& stTime, cjFLOAT& enTime) const { stTime = 0.0f; enTime = 0.0f; }
	virtual void Cache(cjFLOAT /*stTime*/, cjFLOAT /*enTime*/) {}
	virtual cjInterpolator* GetInterpolator(cjFLOAT stTime, cjFLOAT enTime);
    virtual cjBool IsAlwaysUpdate() const { return cjFalse; }

    static const cjBool			INVALID_BOOL;
    static const cjFLOAT		INVALID_FLOAT;
    static const cjQuat			INVALID_QUAT;
    static const cjVec3			INVALID_VEC3;
    static const cjColorF		INVALID_COLORF;
	
protected:
	cjInterpolator();
    cjBool TimeHasChanged(cjFLOAT etime) { return m_lastTime != etime ? cjTrue : cjFalse; }

protected:
    cjFLOAT									m_lastTime;
};

cjRefPtrDefine(cjInterpolator);


//
// cjInterpolator
// - cjLookAtInterpolator
//

class CJCORE_API cjLookAtInterpolator : public cjInterpolator
{
	CJSER_DECL_CLASS(cjLookAtInterpolator, cjInterpolator,0 , CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjLookAtInterpolator)
	CJSER_DECL_CLONE_POST
		
public:
	// cjInterpolator
	virtual cjBool					Update(cjFLOAT etime, cjRefObj* interpTarget, cjQuatTM& v);
	virtual cjBool					IsTMValue() const { return cjTrue; }
	virtual void					Collapse();
	virtual void					GetValidTimeRange(cjFLOAT& stTime, cjFLOAT& enTime) const;
	virtual void					Cache(cjFLOAT stTime, cjFLOAT enTime);
	virtual cjInterpolator*			GetInterpolator(cjFLOAT stTime, cjFLOAT enTime);
    virtual cjBool					IsAlwaysUpdate() const { return cjTrue; }

public:
	enum CJTFL_MASK
	{
		CJTFL_MASK_FLIP		= 0x0001,
		CJTFL_BPOS_FLIP		= 0,

		CJTFL_MASK_AXIS		= 0x0006,
		CJTFL_BPOS_AXIS		= 1
	};
	
    void SetCJTFL(CJTFL_MASK mask, cjWORD bpos, cjWORD valMask, cjUINT32 val) 
		{ _cjtfl_flag = (_cjtfl_flag & ~mask) | ((cjUINT32)((val & valMask) << bpos)); }
	void SetCJTFL(CJTFL_MASK mask, cjWORD bpos, cjBool val) { 
		cjUINT32 _val = val ? 1 : 0; 
		  _cjtfl_flag = (_cjtfl_flag & ~mask) | (_val << bpos);}
	cjUINT32 GetCJTFL(CJTFL_MASK mask, cjWORD bpos) const { return (_cjtfl_flag & mask) >> bpos; }
	
public:
	cjLookAtInterpolator();
	cjLookAtInterpolator(cjRefObj* lookAt, cjAxis axis=cjAxisX, cjBool isFlip=cjFalse);
		
	cjRefObj*						GetLookAt() const { return m_lookAt; }
	void							SetLookAt(cjRefObj* lookAt) { m_lookAt = lookAt; SetLookAtName(cjName_None); }

	cjBool							GetFlip() const { return GetCJTFL(CJTFL_MASK_FLIP, CJTFL_BPOS_FLIP) ? cjTrue : cjFalse; }
	void							SetFlip(cjBool isFlip) { SetCJTFL(CJTFL_MASK_FLIP, CJTFL_BPOS_FLIP, isFlip); }

	cjAxis							GetAxis() const { return (cjAxis)GetCJTFL(CJTFL_MASK_AXIS, CJTFL_BPOS_AXIS); }
	void							SetAxis(cjAxis axis) { SetCJTFL(CJTFL_MASK_AXIS, CJTFL_BPOS_AXIS, CJTFL_MASK_AXIS, (cjUINT32)axis); }

	cjInterpolator*					GetScaleInterpolator() { return m_interpolators[0]; }
	void							SetScaleInterpolator(cjInterpolator* interp) { m_interpolators[0] = interp; }

	cjInterpolator*					GetRollInterpolator() { return m_interpolators[1]; }
	void							SetRollInterpolator(cjInterpolator* interp) { m_interpolators[1] = interp; }

	cjInterpolator*					GetPosInterpolator() { return m_interpolators[2]; }
	void							SetPosInterpolator(cjInterpolator* interp) { m_interpolators[2] = interp; }
	
	const cjName&					GetLookAtName() const { return m_lookAtName; }

	virtual cjBool					ResolveDependObj(cjRefObjNameHash* objHash);
	virtual cjBool					SetUpDependObj();

protected:
	void SetLookAtName(const cjName& name) { m_lookAtName = name; }
	
private:
	void							_InitCommon();

private:
	cjTFLAG							_cjtfl_flag;

	cjRefObj*						m_lookAt;
	cjName							m_lookAtName;

	cjQuatTM						m_tm;

	// [0] scale  [1] rot [2] pos
	cjInterpolatorRPtr				m_interpolators[3];
};


//
// cjKeyBasedInterpolator
//

class CJCORE_API cjKeyBasedInterpolator : public cjInterpolator
{
	CJSER_DECL_CLASS_ABSTRACT(cjKeyBasedInterpolator, cjInterpolator,0 , CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE_ABSTRACT(cjKeyBasedInterpolator)

public:
    virtual void					GetValidTimeRange(cjFLOAT& stTime, cjFLOAT& enTime) const;

public:
    virtual cjUINT32				GetKeyChannelCount() const = 0;
    virtual cjUINT32				GetKeyCount(cjUINT32 channel) const = 0;
    virtual cjAniKey::EKEY_TYPE		GetKeyType(cjUINT32 channel) const = 0;
    virtual cjAniKey::EKEY_VALTYPE	GetKeyValType(cjUINT32 channel) const = 0;
    virtual cjAniKey*				GetKeyAry(cjUINT32 channel) const = 0;
    virtual cjBYTE					GetKeySiz(cjUINT32 channel) const = 0;    
    virtual cjBool					GetChannelPosed(cjUINT32 channel) const = 0;
    
	cjUINT32						GetAllocSiz(cjUINT32 channel) const;
    cjAniKey*						GetKey(cjUINT32 index, cjUINT32 channel) const;
    void							FillDerivedValues(cjUINT32 channel);
    cjAniKey*						GetKeysWithChannel(cjUINT32& rstKeyCount, cjAniKey::EKEY_TYPE& rstKeyType, cjBYTE& rstKeySiz, cjUINT32 channel=0) const;

protected:
};

cjRefPtrDefine(cjKeyBasedInterpolator);


//
// cjKeyBasedInterpolator
// - cjFloatInterpolator
//

class CJCORE_API cjFloatInterpolator : public cjKeyBasedInterpolator
{
    CJSER_DECL_CLASS(cjFloatInterpolator, cjKeyBasedInterpolator,0 , CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjFloatInterpolator)

public:
	// cjInterpolator
    virtual cjBool					Update(cjFLOAT etime, cjRefObj* target, cjFLOAT& v);
    virtual cjBool					IsFloatValue() const { return cjTrue; }
	virtual void					Collapse();
    virtual void					Cache(cjFLOAT stTime, cjFLOAT enTime);
    virtual cjInterpolator*			GetInterpolator(cjFLOAT stTime, cjFLOAT enTime);

    // cjKeyBasedInterpolator
	virtual cjUINT32				GetKeyChannelCount() const { return 1; }
    virtual cjUINT32				GetKeyCount(cjUINT32 channel) const;
    virtual cjAniKey::EKEY_TYPE		GetKeyType(cjUINT32 channel) const;
    virtual cjAniKey::EKEY_VALTYPE	GetKeyValType(cjUINT32 ) const { return cjAniKey::EKVAL_FLOAT; }
    virtual cjAniKey*				GetKeyAry(cjUINT32 channel) const;
    virtual cjBYTE					GetKeySiz(cjUINT32 channel) const;
    virtual cjBool					GetChannelPosed(cjUINT32 channel) const;
	
public:
    cjFloatInterpolator();
    cjFloatInterpolator(cjAniFloatDat* aniFloatDat);
    cjFloatInterpolator(cjFLOAT poseValue);

	// cjAniFloatDat
    cjAniFloatDat*					GetFloatDat() const { return m_spFloatDat; }
    void							SetFloatDat(cjAniFloatDat* floatDat) { m_spFloatDat = floatDat; }
    void							SetPoseValue(cjFLOAT value) { m_spFloatDat = cjNULL; m_value = value; }

    cjAniKeyFloat*					GetKeys(cjUINT32& keyCount, cjAniKeyFloat::EKEY_TYPE& keyType, cjBYTE& siz) const;
    void							ReplaceKeys(cjAniKeyFloat* keys, cjUINT32 keyCount, cjAniKeyFloat::EKEY_TYPE keyType);
    void							SetKeys(cjAniKeyFloat* keys, cjUINT32 keyCount, cjAniKeyFloat::EKEY_TYPE keyType);

protected:
    cjFLOAT							m_value;

    cjAniFloatDatRPtr				m_spFloatDat;
    cjUINT32						m_lastIndex;
};

cjRefPtrDefine(cjFloatInterpolator)


//
// cjKeyBasedInterpolator
// - cjBoolInterpolator
//

class CJCORE_API cjBoolInterpolator : public cjKeyBasedInterpolator
{
    CJSER_DECL_CLASS(cjBoolInterpolator, cjKeyBasedInterpolator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjBoolInterpolator)

public:
	// cjInterpolator
    virtual cjBool					Update(cjFLOAT etime, cjRefObj* target, cjBool& v);
    virtual cjBool					IsBoolValue() const { return cjTrue; }
	virtual void					Collapse();
    virtual void					Cache(cjFLOAT stTime, cjFLOAT enTime);
    virtual cjInterpolator*			GetInterpolator(cjFLOAT stTime, cjFLOAT enTime);

    // cjKeyBasedInterpolator
	virtual cjUINT32				GetKeyChannelCount() const { return 1; }
    virtual cjUINT32				GetKeyCount(cjUINT32 channel) const;
    virtual cjAniKey::EKEY_TYPE		GetKeyType(cjUINT32 channel) const;
    virtual cjAniKey::EKEY_VALTYPE	GetKeyValType(cjUINT32 ) const { return cjAniKey::EKVAL_BOOL; }
    virtual cjAniKey*				GetKeyAry(cjUINT32 channel) const;
    virtual cjBYTE					GetKeySiz(cjUINT32 channel) const;
    virtual cjBool					GetChannelPosed(cjUINT32 channel) const;

public:
    cjBoolInterpolator();
    cjBoolInterpolator(cjAniBoolDat* boolDat);
    cjBoolInterpolator(cjBool poseValue);

    cjAniBoolDat* GetBoolDat() const { return m_spBoolDat;  }
    void SetBoolData(cjAniBoolDat* boolDat) { m_spBoolDat = boolDat; }
    void SetPoseValue(cjBool value);

    cjAniKeyBool* GetKeys(cjUINT32& keyCount, cjAniKeyBool::EKEY_TYPE& keyType, cjBYTE& keySiz) const;
    void ReplaceKeys(cjAniKeyBool* keys, cjUINT32 keyCount, cjAniKeyBool::EKEY_TYPE keyType);
	void SetKeys(cjAniKeyBool* keys, cjUINT32 keyCount, cjAniKeyBool::EKEY_TYPE keyType);

protected:
	void _CommInit();

	cjBool							m_value;

    cjAniBoolDatRPtr				m_spBoolDat;
    cjUINT32						m_lastIndex;
};

cjRefPtrDefine(cjBoolInterpolator)


//
// cjBoolTimelineInterpolator - cjBoolInterpolator
//

class CJCORE_API cjBoolTimelineInterpolator : public cjBoolInterpolator
{
    CJSER_DECL_CLASS(cjBoolTimelineInterpolator, cjBoolInterpolator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjBoolTimelineInterpolator)

public:
	// cjInterpolator
    virtual cjBool					Update(cjFLOAT etime, cjRefObj* target, cjBool& value);

public:
    cjBoolTimelineInterpolator();
    cjBoolTimelineInterpolator(cjAniBoolDat* dat);
    cjBoolTimelineInterpolator(cjBool poseValue);

protected:
    cjUINT32						m_prevIdx;
    cjBool							m_prevValue;
};

cjRefPtrDefine(cjBoolTimelineInterpolator)


//
// cjKeyBasedInterpolator
// - cjVec3SInterpolator
//

class CJCORE_API cjVec3SInterpolator : public cjKeyBasedInterpolator
{
	CJSER_DECL_CLASS(cjVec3SInterpolator, cjKeyBasedInterpolator,0 , CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjVec3SInterpolator)

public:
	// cjInterpolator
    virtual cjBool					Update(cjFLOAT etime, cjRefObj* target, cjVec3& v);
	virtual cjBool					IsVec3Value() const { return cjTrue; }
    virtual void					Collapse();
    virtual void					Cache(cjFLOAT stTime, cjFLOAT enTime);
    virtual cjInterpolator*			GetInterpolator(cjFLOAT stTime, cjFLOAT enTime);
	
	// cjKeyBasedInterpolator
	virtual cjUINT32				GetKeyChannelCount() const { return 1; }
    virtual cjUINT32				GetKeyCount(cjUINT32 channel) const;
    virtual cjAniKey::EKEY_TYPE		GetKeyType(cjUINT32 channel) const;
	virtual cjAniKey::EKEY_VALTYPE	GetKeyValType(cjUINT32 ) const { return cjAniKey::EKVAL_VEC3; }
    virtual cjAniKey*				GetKeyAry(cjUINT32 channel) const;
    virtual cjBYTE					GetKeySiz(cjUINT32 channel) const;
    virtual cjBool					GetChannelPosed(cjUINT32 channel) const;
	
public:
    cjVec3SInterpolator();
    cjVec3SInterpolator(cjAniVec3Dat* aniVec3Dat);
    cjVec3SInterpolator(const cjVec3& poseValue);

	cjAniVec3Dat*					GetVec3Dat() const { return m_spVec3Dat; }
	void							SetVec3Dat(cjAniVec3Dat* vec3Dat) { m_spVec3Dat = vec3Dat; }
	void							SetPoseValue(const cjVec3& poseValue) { m_spVec3Dat = cjNULL; m_value = poseValue; }

    cjAniKeyVec3*					GetKeys(cjUINT32& keyCount, cjAniKeyVec3::EKEY_TYPE& keyType, cjBYTE& keySiz) const;
    void							ReplaceKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType);
    void							SetKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType);
	
protected:
    cjVec3							m_value;
    cjAniVec3DatRPtr				m_spVec3Dat;
    cjUINT32						m_lastIndex;
};

cjRefPtrDefine(cjVec3SInterpolator)


//
// cjKeyBasedInterpolator
// - cjVec3PInterpolator
//

class CJCORE_API cjVec3PInterpolator : public cjKeyBasedInterpolator
{
	CJSER_DECL_CLASS(cjVec3PInterpolator, cjKeyBasedInterpolator,0 , CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjVec3PInterpolator)

public:
	// cjInterpolator
    virtual cjBool					Update(cjFLOAT etime, cjRefObj* target, cjVec3& v);
	virtual cjBool					IsVec3Value() const { return cjTrue; }
    virtual void					Collapse();
    virtual void					Cache(cjFLOAT stTime, cjFLOAT enTime);
    virtual cjInterpolator*			GetInterpolator(cjFLOAT stTime, cjFLOAT enTime);
	
	// cjKeyBasedInterpolator
	virtual cjUINT32				GetKeyChannelCount() const { return 1; }
    virtual cjUINT32				GetKeyCount(cjUINT32 channel) const;
    virtual cjAniKey::EKEY_TYPE		GetKeyType(cjUINT32 channel) const;
	virtual cjAniKey::EKEY_VALTYPE	GetKeyValType(cjUINT32 ) const { return cjAniKey::EKVAL_VEC3; }
    virtual cjAniKey*				GetKeyAry(cjUINT32 channel) const;
    virtual cjBYTE					GetKeySiz(cjUINT32 channel) const;
    virtual cjBool					GetChannelPosed(cjUINT32 channel) const;
	
public:
    cjVec3PInterpolator();
    cjVec3PInterpolator(cjAniVec3Dat* aniVec3Dat);
    cjVec3PInterpolator(const cjVec3& poseValue);

	cjAniVec3Dat*					GetVec3Dat() const { return m_spVec3Dat; }
	void							SetVec3Dat(cjAniVec3Dat* vec3Dat) { m_spVec3Dat = vec3Dat; }
	void							SetPoseValue(const cjVec3& poseValue) { m_spVec3Dat = cjNULL; m_value = poseValue; }

    cjAniKeyVec3*					GetKeys(cjUINT32& keyCount, cjAniKeyVec3::EKEY_TYPE& keyType, cjBYTE& keySiz) const;
    void							ReplaceKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType);
    void							SetKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType);
	
protected:
    cjVec3							m_value;
    cjAniVec3DatRPtr				m_spVec3Dat;
    cjUINT32						m_lastIndex;
};

cjRefPtrDefine(cjVec3PInterpolator)


//
// cjKeyBasedInterpolator
// - cjQuatInterpolator
//

class CJCORE_API cjQuatInterpolator : public cjKeyBasedInterpolator
{
	CJSER_DECL_CLASS(cjQuatInterpolator, cjKeyBasedInterpolator,0 , CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjQuatInterpolator)

public:
	// cjKeyBasedInterpolator
    virtual cjBool					Update(cjFLOAT etime, cjRefObj* target, cjQuat& v);
    virtual cjBool					IsQuatValue() const { return cjTrue; }
    virtual void					Collapse();
    virtual void					GetValidTimeRange(cjFLOAT& stTime, cjFLOAT& enTime) const;
    virtual void					Cache(cjFLOAT stTime, cjFLOAT enTime);
    virtual cjInterpolator*			GetInterpolator(cjFLOAT stTime, cjFLOAT enTime);

	// Implemented from cjKeyBasedInterpolator
    virtual cjUINT32				GetKeyChannelCount() const { return 1; }
	virtual cjUINT32				GetKeyCount(cjUINT32 channel) const;
    virtual cjAniKey::EKEY_TYPE		GetKeyType(cjUINT32 channel) const;
    virtual cjAniKey::EKEY_VALTYPE	GetKeyValType(cjUINT32 ) const { return cjAniKey::EKVAL_ROT; }
    virtual cjAniKey*				GetKeyAry(cjUINT32 channel) const;
    virtual cjBYTE					GetKeySiz(cjUINT32 channel) const;

    virtual cjBool					GetChannelPosed(cjUINT32 channel) const;

public:
    cjQuatInterpolator();
    cjQuatInterpolator(cjAniRotDat* rotDat);
    cjQuatInterpolator(const cjQuat& poseValue);

	cjAniRotDat*					GetRotDat() const { return m_spRotDat; }
    void							SetRotDat(cjAniRotDat* rotDat) { m_spRotDat = rotDat; }
    void							SetPoseValue(const cjQuat& poseValue);

    cjAniKeyRot*					GetKeys(cjUINT32& keyCount, cjAniKeyRot::EKEY_TYPE& keyType, cjBYTE& siz) const;
    void							ReplaceKeys(cjAniKeyRot* keys, cjUINT32 keyCount, cjAniKeyRot::EKEY_TYPE keyType);
    void							SetKeys(cjAniKeyRot* keys, cjUINT32 keyCount, cjAniKeyRot::EKEY_TYPE keyType);

protected:
	cjQuat							m_value;
	cjAniRotDatRPtr					m_spRotDat;
	cjUINT32						m_lastIndex;
};

cjRefPtrDefine(cjQuatInterpolator)


//
// cjKeyBasedInterpolator
// - cjTMInterpolator
//

class CJCORE_API cjTMInterpolator : public cjKeyBasedInterpolator
{
	CJSER_DECL_CLASS(cjTMInterpolator, cjKeyBasedInterpolator,0 , CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjTMInterpolator)

public:
	// cjInterpolator
	virtual cjBool					Update(cjFLOAT etime, cjRefObj* interploationTarget, cjQuatTM& rstVal);
    virtual cjBool					IsTMValue() const { return cjTrue; }
    virtual void					Collapse();
    virtual void					GetValidTimeRange(cjFLOAT& stTime, cjFLOAT& enTime) const;
    virtual void					Cache(cjFLOAT stTime, cjFLOAT enTime);
    virtual cjInterpolator*			GetInterpolator(cjFLOAT stTime, cjFLOAT enTime);

    // cjKeyBasedInterpolator
	virtual cjUINT32				GetKeyChannelCount() const { return 3; } // Scale,Rot,Pos
    virtual cjUINT32				GetKeyCount(cjUINT32 channel) const;
    virtual cjAniKey::EKEY_TYPE		GetKeyType(cjUINT32 channel) const;
    virtual cjAniKey::EKEY_VALTYPE	GetKeyValType(cjUINT32 channel) const;
    virtual cjAniKey*				GetKeyAry(cjUINT32 channel) const;
    virtual cjBYTE					GetKeySiz(cjUINT32 channel) const;
    virtual cjBool					GetChannelPosed(cjUINT32 channel) const;

public:
	cjTMInterpolator();
	cjTMInterpolator(cjAniTMDat* aniTMDat);
    cjTMInterpolator(const cjQuatTM& poseValue);

    cjAniTMDat*						GetAniTMDat() const { return m_spAniTMDat; }
    void							SetAniTMDat(cjAniTMDat* aniTMDat);

    // scale
   cjUINT32							GetScaleKeyCount() const { return (m_spAniTMDat ? m_spAniTMDat->GetScaleKeyCount() : 0); }
   cjAniKeyVec3*					GetScaleKeys(cjUINT32& keyCount, cjAniKeyVec3::EKEY_TYPE& keyType, cjBYTE& keySiz) const;
   void								ReplaceScaleKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType);
   void								SetPoseScale(const cjVec3& poseScale);

    // rot
    cjUINT32						GetRotKeyCount() const { return (m_spAniTMDat ? m_spAniTMDat->GetRotKeyCount() : 0); }
    cjAniKeyRot*					GetRotKeys(cjUINT32& keyCount, cjAniKeyRot::EKEY_TYPE& keyType, cjBYTE& keySiz) const;
    void							ReplaceRotKeys(cjAniKeyRot* keys, cjUINT32 keyCount, cjAniKeyRot::EKEY_TYPE keyType);
    void							SetPoseRotate(const cjQuat& rot);

	// pos
    cjUINT32						GetPosKeyCount() const { return (m_spAniTMDat ? m_spAniTMDat->GetPosKeyCount() : 0); }
    cjAniKeyVec3*					GetPosKeys(cjUINT32& keyCount, cjAniKeyVec3::EKEY_TYPE& keyType, cjBYTE& keySiz) const;
    void							ReplacePosKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType);
    void							SetPosePos(const cjVec3& pos);
	
	void							SetPoseValue(const cjQuatTM& poseValue);

protected:
    cjQuatTM						m_value;

	cjAniTMDatRPtr					m_spAniTMDat;

    cjUINT32						m_lastTransIdx;
    cjUINT32						m_lastRotIdx;
    cjUINT32						m_lastScaleIdx;
};

cjRefPtrDefine(cjTMInterpolator)


//
// cjKeyBasedInterpolator
// - cjPathInterpolator
//

class CJCORE_API cjPathInterpolator : public cjKeyBasedInterpolator
{
	CJSER_DECL_CLASS(cjPathInterpolator, cjKeyBasedInterpolator,0 , CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjPathInterpolator)
	CJSER_DECL_CLONE_POST

public:
	// rtime = 0.0~1.0
    virtual cjBool					Update(cjFLOAT rtime, cjRefObj* target, cjQuatTM& value);
    virtual cjBool					IsTMValue() const { return cjTrue; }

	// channel 1 range only
    virtual void					GetValidTimeRange(cjFLOAT& stTime, cjFLOAT& enTime) const;
	// stTime~enTime: 0.0~1.0
    virtual void					Cache(cjFLOAT stTime, cjFLOAT enTime); 
    virtual cjInterpolator*			GetInterpolator(cjFLOAT stTime, cjFLOAT enTime);
	
    // cjKeyBasedInterpolator
    virtual cjUINT32				GetKeyChannelCount() const { return 2; } // [0] path [1] percent
    virtual cjUINT32				GetKeyCount(cjUINT32 channel) const;
    virtual cjAniKey::EKEY_TYPE		GetKeyType(cjUINT32 channel) const;
    virtual cjAniKey::EKEY_VALTYPE	GetKeyValType(cjUINT32 channel) const;
    virtual cjAniKey*				GetKeyAry(cjUINT32 channel) const;
    virtual cjBYTE					GetKeySiz(cjUINT32 channel) const;
	virtual cjBool					GetChannelPosed(cjUINT32 ) const { return cjFalse; }
	
public:

    enum ECHANNEL
    {
        ECHAN_PATH				= 0,
        ECHAN_PERCENT_WEIGHT
    };
	
	enum BANKDIR { BANKDIR_NEG = -1, BANKDIR_POSITIVE = 1 };
	
	CJFLAG_DECL_BEGIN
		MASK_OPEN_CURVE			= 0x00000001,
		BPOS_OPEN_CURVE			= 0,

		MASK_CONST_SPEED_DAT_DIRTY	= 0x00000002,
		BPOS_CONST_SPEED_DAT_DIRTY	= 1,

		MASK_ALLOWFLIP			= 0x00000004,
		BPOS_ALLOWFLIP			= 2,

		MASK_FLIP				= 0x00000008,
		BPOS_FLIP				= 3,

		MASK_FOLLOW				= 0x00000010,
		BPOS_FOLLOW				= 4,

		MASK_CONST_SPEED		= 0x00000020,
		BPOS_CONST_SPEED		= 5,

		MASK_BANKING			= 0x00000040,
		BPOS_BANKING			= 6,
	CJFLAG_DECL_END
				
public:
    cjPathInterpolator();
    cjPathInterpolator(cjAniVec3Dat* pathDat, cjAniFloatDat* pctDat);
    	
	// Flag
	cjBool							IsConstSpeedDatDirty() const { return _GetFlagAsBool(MASK_CONST_SPEED_DAT_DIRTY, BPOS_CONST_SPEED_DAT_DIRTY); }
	void							SetConstSpeedDatDirty(cjBool dirty) { _SetFlag(MASK_CONST_SPEED_DAT_DIRTY, BPOS_CONST_SPEED_DAT_DIRTY, dirty); }	

		// opened curve / closed curve 
	cjBool							IsOpenCurve() const  { return _GetFlagAsBool(MASK_OPEN_CURVE, BPOS_OPEN_CURVE); }
	void							SetOpenCurve(cjBool openCurve){ _SetFlag(MASK_OPEN_CURVE, BPOS_OPEN_CURVE, openCurve); }

	cjBool							IsAllowFlip() const { return _GetFlagAsBool(MASK_ALLOWFLIP, BPOS_ALLOWFLIP); }
	void							SetAllowFlip(cjBool allowFlip) { _SetFlag(MASK_ALLOWFLIP, BPOS_ALLOWFLIP, allowFlip); }
	cjBool							GetFlip() const { return _GetFlagAsBool(MASK_FLIP, BPOS_FLIP); }	
	void							SetFlip(cjBool flip) { _SetFlag(MASK_FLIP, BPOS_FLIP, flip); }	
	
		// banking
	BANKDIR							GetBankDir() const { return m_bankDir; }
	void							SetBankDir(BANKDIR dir) { m_bankDir = dir; }
	cjFLOAT							GetMaxBankAngle() const { return m_maxBankAngle; }
	void							SetMaxBankAngle(cjFLOAT angle) { m_maxBankAngle = angle; } 
	cjBool							IsBanking() const { return _GetFlagAsBool(MASK_BANKING, BPOS_BANKING); }
	void							SetBanking(cjBool banking) { _SetFlag(MASK_BANKING, BPOS_BANKING, banking); }
		
		// etc					
	cjBool							IsFollow() const { return _GetFlagAsBool(MASK_FOLLOW, BPOS_FOLLOW); } // apply rotation at update
	void							SetFollow(cjBool follow) { _SetFlag(MASK_FOLLOW, BPOS_FOLLOW, follow); }

	cjBool							IsConstSpeed() const { return _GetFlagAsBool(MASK_CONST_SPEED, BPOS_CONST_SPEED); }
	void							SetConstSpeed(cjBool constSpeed);
	
	cjSWORD							GetFollowAxis() const { return m_followAxis; }
	void							SetFollowAxis(cjSWORD axis) { m_followAxis = axis; }
		
	cjFLOAT							GetSmoothing() const { return m_smooth; }
	void							SetSmoothing(cjFLOAT smooth) { m_smooth = smooth; }

	// PCT Dat
	cjAniFloatDat*					GetPCTDat() const { return m_spPCTDat; }
	void							SetPCTDat(cjAniFloatDat* pctDat) { m_spPCTDat = pctDat; }    
	cjAniKeyFloat*					GetPCTKeys(cjUINT32& keyCount, cjAniKeyFloat::EKEY_TYPE& keyType, cjBYTE& keySiz) const;
    void							ReplacePCTKeys(cjAniKeyFloat* keys, cjUINT32 keyCount, cjAniKeyFloat::EKEY_TYPE keyType);

	// Path Dat		
	cjAniVec3Dat*					GetPathDat() const { return m_spPathDat; }
	void							SetPathDat(cjAniVec3Dat* pathDat) { m_spPathDat = pathDat; }
    cjAniKeyVec3*					GetPathKeys(cjUINT32& keyCount, cjAniKeyVec3::EKEY_TYPE& keyType, cjBYTE& keySiz) const;
	void							ReplacePathKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType);
	cjFLOAT							GetTotalPathLen(); // ignored calling when m_totalPathLength is not -1

protected:
	void							_InitCommon();
	
	// 0<= rtime <=1
	void							GetPathInfo(cjFLOAT etime, cjUINT32& rstKeyIdx0, cjUINT32& rstKeyIdx1, cjFLOAT& rtime);
	void							GetPathInfoByRatio(cjFLOAT ratio, cjUINT32& rstKeyIdx0, cjUINT32& rstKeyIdx1, cjFLOAT& rtime);
	void							GetPathRotation(cjUINT32 keyIdx0, cjUINT32 keyIdx1, cjFLOAT rtime, cjMatrix3& rstMatRot);
	cjVec3							GenPathInterp(cjINT32 keyIdx0, cjINT32 keyIdx1, cjFLOAT rtime);	
    cjFLOAT							GetPathSpeed(cjINT32 keyIdx0, cjINT32 keyIdx1, cjFLOAT rtime) const;
    cjFLOAT							GetPathLength(cjINT32 keyIdx0, cjINT32 keyIdx1, cjFLOAT rtime) const;

	void							SetConstSpeedDat();

protected:
    cjUINT32						m_lastPathIndex;
    cjUINT32						m_lastPCTIndex;
	
	BANKDIR							m_bankDir;
    cjFLOAT							m_maxBankAngle;
    cjFLOAT							m_smooth;    
    cjFLOAT							m_maxCurvature;
    
    cjQuat							m_rot;
    cjVec3							m_pos;
    cjFLOAT							m_lastTime;

    cjAniVec3DatRPtr				m_spPathDat;
    cjAniFloatDatRPtr				m_spPCTDat;

	cjINT32							m_pathLengthCount;
    cjFLOAT*						m_pathLengths;
    cjFLOAT							m_totalPathLength;  // -1 for need to calculate length of all path

	cjSWORD							m_followAxis;
	cjWORD							m_pad;
};

cjRefPtrDefine(cjPathInterpolator)


//
// cjColorInterpolator - cjKeyBasedInterpolator
//

class CJCORE_API cjColorInterpolator : public cjKeyBasedInterpolator
{
    CJSER_DECL_CLASS(cjColorInterpolator, cjKeyBasedInterpolator,0 , CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjColorInterpolator)

public:
	// cjInterpolator
    virtual cjBool					Update(cjFLOAT etime, cjRefObj* target, cjColorF& v);
    virtual cjBool					IsColorValue() const { return cjTrue; }
	virtual void					Collapse();
    virtual void					Cache(cjFLOAT stTime, cjFLOAT enTime);
    virtual cjInterpolator*			GetInterpolator(cjFLOAT stTime, cjFLOAT enTime);

    // cjKeyBasedInterpolator
	virtual cjUINT32				GetKeyChannelCount() const { return 1; }
    virtual cjUINT32				GetKeyCount(cjUINT32 channel) const;
    virtual cjAniKey::EKEY_TYPE		GetKeyType(cjUINT32 channel) const;
    virtual cjAniKey::EKEY_VALTYPE	GetKeyValType(cjUINT32 ) const { return cjAniKey::EKVAL_COLOR; }
    virtual cjAniKey*				GetKeyAry(cjUINT32 channel) const;
    virtual cjBYTE					GetKeySiz(cjUINT32 channel) const;
    virtual cjBool					GetChannelPosed(cjUINT32 channel) const;
	
public:
    cjColorInterpolator();
    cjColorInterpolator(cjAniColorDat* colorDat);
    cjColorInterpolator(const cjColorF& poseValue);

    cjAniColorDat*					GetColorDat() const { return m_spColorDat; }
    void							SetColorDat(cjAniColorDat* colorDat) { m_spColorDat = colorDat; }
    void							SetPoseValue(const cjColorF& poseValue);

    cjAniKeyColor*					GetKeys(cjUINT32& keyCount, cjAniKeyColor::EKEY_TYPE& keyType, cjBYTE& keySiz) const;
    void							ReplaceKeys(cjAniKeyColor* keys, cjUINT32 keyCount, cjAniKeyColor::EKEY_TYPE keyType);
    void							SetKeys(cjAniKeyColor* keys, cjUINT32 keyCount, cjAniKeyColor::EKEY_TYPE keyType);

protected:
    cjColorF						m_value;

    cjAniColorDatRPtr				m_spColorDat;
    cjUINT32						m_lastIndex;
};

cjRefPtrDefine(cjColorInterpolator);


//
// Blend Interpolator
//

class CJCORE_API cjBlendInterpItem : public cjMem
{
public:
	void operator >> (cjSerializer& ser);

public:
	cjBlendInterpItem();
    void							ClearValues();
    cjBool							IsEqual(cjBlendInterpItem& dst);

public:
    cjInterpolatorRPtr				m_spInterpolator;
    cjFLOAT							m_weight;
    cjFLOAT							m_normalizedWeight;
    cjBYTE							m_priority;
    cjFLOAT							m_easeSpinner;
    cjFLOAT							m_updateTime;
};


class CJCORE_API cjBlendInterpolator : public cjInterpolator
{
	CJSER_DECL_CLASS_ABSTRACT(cjBlendInterpolator, cjInterpolator,0 , CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE_ABSTRACT(cjBlendInterpolator)
	CJSER_DECL_CLONE_POST

public:
	// cjInterpolator
    virtual void					Collapse();
	virtual void					Cache(cjFLOAT stTime, cjFLOAT enTime);

public:
    static const cjBYTE INVALID_INDEX;

	static const cjBYTE FL_MANAGER_CONTROLLED;			// = 0x1
	static const cjBYTE FL_USE_HIGHWEIGHT_ONLY;			// = 0x2
	static const cjBYTE FL_COMPUTE_NORMALIZED_WEIGHT;	// = 0x4

		
    cjBYTE							GetArySiz() const { return m_arySiz; }
    static cjBYTE					GetAryGrowCount() { return ms_aryGrowCount; }
    static void						SetAryGrowCount(cjBYTE aryCountGrowBy) { ms_aryGrowCount = aryCountGrowBy; }

    char							GetPriority(cjBYTE index=0) const { cjAssert(index < m_arySiz); return m_interpAry[index].m_priority; }
    void							SetPriority(char priority, cjBYTE index=0);

    cjInterpolator*					GetInterpolator(cjBYTE index=0) const;
    void							SetInterpolator(cjInterpolator* interpolator, cjBYTE index=0);
    virtual cjBYTE					AddInterpInfo(cjInterpolator* interpolator, cjFLOAT weight, char priority=0, cjFLOAT easeSpinner=1.0f);
    virtual	cjInterpolatorRPtr		RemoveInterpInfo(cjBYTE index);

    cjFLOAT							GetWeight(cjBYTE index=0) const;
    void							SetWeight(cjFLOAT weight, cjBYTE index=0);
	cjBool							IsUseHighestWeightOnly() const { return _GetFlag(FL_USE_HIGHWEIGHT_ONLY); }
    void							SetUseHighestWeightOnly(cjBool isUseHighestWeightOnly) { _SetFlag(FL_USE_HIGHWEIGHT_ONLY, isUseHighestWeightOnly); }
    cjFLOAT							GetWeightThreshold() const { return m_weightThreshold; }
    void							SetWeightThreshold(cjFLOAT weightThreshold) 
									{
										m_weightThreshold = weightThreshold;
										if(m_weightThreshold < 0.0f)
											m_weightThreshold = 0.0f;
									}

    void							ComputeNormalizedWeights();
    cjFLOAT							GetNormalizedWeight(cjBYTE index=0) const;
    cjBYTE							GetHighestWeightedIndex() const;

	cjFLOAT							GetEaseSpinner(cjBYTE index=0) const 
									{ 
										cjAssert(index < m_arySiz);
										if(m_interpCount == 1 && index == m_singleIdx)
											return 1.0f;
										return m_interpAry[index].m_easeSpinner;
									}
    void							SetEaseSpinner(cjFLOAT easeSpinner, cjBYTE index=0);

    cjFLOAT							GetTime(cjBYTE index=0) const 
									{
										cjAssert(index < m_arySiz);
										if(m_interpCount == 1 && index == m_singleIdx) { return m_singleTime; }
										return m_interpAry[index].m_updateTime;	
									}
    void							SetTime(cjFLOAT etime, cjBYTE index=0) 
									{ 
										cjAssert(index < m_arySiz);
										if(m_interpCount == 1 && index == m_singleIdx) { m_singleTime = etime; return; }
										m_interpAry[index].m_updateTime = etime; 
									}

	cjBool							_GetFlag(cjBYTE flag) const { return (m_flag & flag) ? cjTrue : cjFalse; }
	void							_SetFlag(cjBYTE flag, cjBool setFlag) { m_flag &= ~flag; if(setFlag) m_flag |= flag; }
	
	cjBool							IsManagerControlled() const { return _GetFlag(FL_MANAGER_CONTROLLED); }
	void							SetManagerControlled(cjBool isManagerControlled) { _SetFlag(FL_MANAGER_CONTROLLED, isManagerControlled); }
	
protected:
	cjBlendInterpolator(cjBool managerControlled, cjFLOAT weightThreshold, cjBYTE arySiz);

    virtual cjBool					CheckInterpolatorType(cjInterpolator* interpolator) const = 0;
    virtual cjBool					ReallocAry();

    void							ClearWeightSums();
	cjBool							NeedComputingNormalizedWeights() const { return _GetFlag(FL_COMPUTE_NORMALIZED_WEIGHT); }
    void							SetNeedComputingNormalizedWeights(cjBool needNormalizedWeights) { _SetFlag(FL_COMPUTE_NORMALIZED_WEIGHT, needNormalizedWeights); }
    void							ComputeNormalized2Weight();

    cjBool							GetSingleUpdateTime(cjFLOAT& etime);
    cjBool							GetUpdateTimeForItem(cjFLOAT& etime, cjBlendInterpItem& item);

	void							_CommonInit(cjFLOAT weightThreshold, cjBYTE arySiz);

protected:
    cjBlendInterpItem*				m_interpAry;
    cjInterpolator*					m_singleInterpolator;

    cjFLOAT							m_singleTime;

    cjFLOAT							m_weightThreshold;
    cjFLOAT							m_highSumOfWeights;
    cjFLOAT							m_nextHighSumOfWeights;
    cjFLOAT							m_highEaseSpinner;

	cjBYTE							m_flag;   
	cjBYTE							m_arySiz;
    cjBYTE							m_interpCount;
	cjBYTE							m_singleIdx;

    cjSBYTE							m_highPriority;
    cjSBYTE							m_nextHighPriority;

    static cjBYTE					ms_aryGrowCount;
};

cjRefPtrDefine(cjBlendInterpolator)


//
// cjBlendTMInterpolator
// Blend - BlendInterpolator - BlendTransformInterpolator
//

class CJCORE_API cjBlendTMInterpolator : public cjBlendInterpolator
{
	CJSER_DECL_CLASS(cjBlendTMInterpolator, cjBlendInterpolator,0 , CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjBlendTMInterpolator)

public:
	// cjInterpolator
    virtual cjBool					Update(cjFLOAT etime, cjSceneNode* target, cjQuatTM& v);
	virtual cjBool					IsTMValue() const { return cjTrue; }

protected:
	// cjBlendInterpolator
	virtual cjBool					CheckInterpolatorType(cjInterpolator* interpolator) const;
	
public:
	cjBlendTMInterpolator();
    cjBlendTMInterpolator(cjBool managerControlled, cjFLOAT weightThreshold, cjBYTE arySiz);
	
protected:
    cjBool							StoreSingleValue(cjFLOAT etime, cjSceneNode* interpTarget, cjQuatTM& tm);
    cjBool							BlendValues(cjFLOAT etime, cjSceneNode* interpTarget, cjQuatTM& tm);
};

cjRefPtrDefine(cjBlendTMInterpolator)


//
// cjBlendInterpolator 
//   - cjBlendBoolInterpolator
//

class CJCORE_API cjBlendBoolInterpolator : public cjBlendInterpolator
{
	CJSER_DECL_CLASS(cjBlendBoolInterpolator, cjBlendInterpolator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjBlendBoolInterpolator)

public:
	// cjInterpolator
    virtual cjBool					Update(cjFLOAT etime, cjRefObj* target, cjBool& v);
    virtual cjBool					IsBoolValue() const { return cjTrue; }

	// cjBlendInterpolator
    virtual cjBool					CheckInterpolatorType(cjInterpolator* interpolator) const { return interpolator->IsBoolValue(); }

public:
	cjBlendBoolInterpolator();
    cjBlendBoolInterpolator(cjBool managerControlled, cjFLOAT weightThreshold, cjBYTE arySiz);
	
protected:
    cjBool							StoreSingleValue(cjFLOAT etime, cjRefObj* target, cjBool& v);
    cjBool							BlendValues(cjFLOAT etime, cjRefObj* target, cjBool& v);

protected:
    cjBool							m_value;
};

cjRefPtrDefine(cjBlendBoolInterpolator);


//
// cjBSInterpolator - cjInterpolator
//

class CJCORE_API cjBSInterpolator : public cjInterpolator
{
	CJSER_DECL_CLASS_ABSTRACT(cjBSInterpolator, cjInterpolator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_SERIALIZE_POST
	CJSER_DECL_CLONE_ABSTRACT(cjBSInterpolator)

public:
	// cjInterpolator
    virtual void					Collapse();
    virtual void					GetValidTimeRange(cjFLOAT& stTime, cjFLOAT& enTime) const;
    virtual void					Cache(cjFLOAT stTime, cjFLOAT enTime);
    virtual cjInterpolator*			GetInterpolator(cjFLOAT stTime, cjFLOAT enTime);
	
public:
    cjBSplineDat*					GetDat() const { return m_spDat; }
	void							SetDat(cjBSplineDat* bsDat, cjBSplineBasisF3Dat* basisDat) { m_spDat = bsDat; m_spBasisDat = basisDat; }
	cjBSplineBasisF3Dat*			GetBasisDat() const { return m_spBasisDat; }
	void							SetTimeRange(cjFLOAT stTime, cjFLOAT enTime) { m_stTime = stTime; m_enTime = enTime; }
	
	virtual cjUINT32				GetChannelCount() const = 0;
    virtual cjBool					GetChannelPosed(cjUINT32 channel) const = 0;

    virtual cjUINT32				GetAllocSiz(cjUINT32 channel) const;	
    virtual cjUINT32				GetCPCount(cjUINT32 channel) const;
    virtual cjUINT32				GetFloatValCount(cjUINT32 channel) const = 0;
    virtual cjUINT32				GetDegree(cjUINT32 channel) const = 0; 

    virtual cjBSplineDat::BSOffset	GetCPOffset(cjUINT32 channel) const = 0;
    virtual void					SetCPOffset(cjBSplineDat::BSOffset cpOffset, cjUINT32 channel) = 0;

	// cjTrue : word size of cjFLOAT ,  cjFalse : cjFLOAT
    virtual cjBool					IsCompressedCP() const { return cjFalse; }

protected:
    cjBSInterpolator(cjBSplineDat* dat, cjBSplineBasisF3Dat* basisDat);

protected:
    cjFLOAT							m_stTime;
    cjFLOAT							m_enTime;
    
    cjBSplineDatRPtr				m_spDat;    
    cjBSplineBasisF3DatRPtr			m_spBasisDat;   // caches values, cloning required
};

cjRefPtrDefine(cjBSInterpolator);


//
// cjBSColorInterpolator - cjBSInterpolator
//

class CJCORE_API cjBSColorInterpolator : public cjBSInterpolator
{
	CJSER_DECL_CLASS(cjBSColorInterpolator, cjBSInterpolator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjBSColorInterpolator)

public:
	// cjInterpolator
    virtual cjBool					Update(cjFLOAT etime, cjRefObj* target, cjColorF& value);
    virtual cjBool					IsColorValue() const { return cjTrue; }

	// cjBSInterpolator
	virtual cjUINT32				GetChannelCount() const { return 1; }
    virtual cjBool					GetChannelPosed(cjUINT32 channel) const;
    virtual cjUINT32				GetFloatValCount(cjUINT32 channel) const;
    virtual cjUINT32				GetDegree(cjUINT32 channel) const; 

	virtual cjBSplineDat::BSOffset	GetCPOffset(cjUINT32 channel) const;
    virtual void					SetCPOffset(cjBSplineDat::BSOffset cpOffset, cjUINT32 channel);

public:
	cjBSColorInterpolator();
    cjBSColorInterpolator(const cjColorF& poseValue);
	cjBSColorInterpolator(cjBSplineDat* bsDat, cjBSplineDat::BSOffset colorCPOffset=cjBSplineDat::INVALID_BSOFFSET,
		cjBSplineBasisF3Dat* basisDat=cjNULL);

    void							SetPoseValue(cjColorF& poseValue);

protected:
    cjColorF						m_value;
    cjBSplineDat::BSOffset			m_colorCPOffset;
};

cjRefPtrDefine(cjBSColorInterpolator);


//
// cjBSFloatInterpolator - cjBSInterpolator
//

class CJCORE_API cjBSFloatInterpolator : public cjBSInterpolator
{
	CJSER_DECL_CLASS(cjBSFloatInterpolator, cjBSInterpolator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjBSFloatInterpolator)
	
public:
	// cjInterpolator
    virtual cjBool					Update(cjFLOAT etime, cjRefObj* target, cjFLOAT& value);
    virtual cjBool					IsFloatValue() const { return cjTrue; }

	// cjBSInterpolator
	virtual cjUINT32				GetChannelCount() const { return 1; }
    virtual cjBool					GetChannelPosed(cjUINT32 channel) const;
    virtual cjUINT32				GetFloatValCount(cjUINT32 channel) const;
    virtual cjUINT32				GetDegree(cjUINT32 channel) const; 

	virtual cjBSplineDat::BSOffset	GetCPOffset(cjUINT32 channel) const;
    virtual void					SetCPOffset(cjBSplineDat::BSOffset cpOffset, cjUINT32 channel);
	
public:
    cjBSFloatInterpolator();
    cjBSFloatInterpolator(cjFLOAT poseValue);
	cjBSFloatInterpolator(cjBSplineDat* bsDat, cjBSplineDat::BSOffset floatCPOffset=cjBSplineDat::INVALID_BSOFFSET, 
		cjBSplineBasisF3Dat* basisDat=cjNULL);

    void							SetPoseValue(cjFLOAT poseValue);

protected:
    cjFLOAT							m_value;
    cjBSplineDat::BSOffset			m_valueCPOffset;
};

cjRefPtrDefine(cjBSFloatInterpolator);


//
// cjBSVec3SInterpolator - cjBSInterpolator
//

class CJCORE_API cjBSVec3SInterpolator : public cjBSInterpolator
{
	CJSER_DECL_CLASS(cjBSVec3SInterpolator, cjBSInterpolator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjBSVec3SInterpolator)

public:
	// cjInterpolator
    virtual cjBool Update(cjFLOAT etime, cjRefObj* target, cjVec3& value);
    virtual cjBool IsVec3Value() const { return cjTrue; }

	// cjBSInterpolator
	virtual cjUINT32				GetChannelCount() const { return 1; }
    virtual cjBool					GetChannelPosed(cjUINT32 channel) const;
    virtual cjUINT32				GetFloatValCount(cjUINT32 channel) const;
    virtual cjUINT32				GetDegree(cjUINT32 channel) const;

	virtual cjBSplineDat::BSOffset	GetCPOffset(cjUINT32 channel) const;
    virtual void					SetCPOffset(cjBSplineDat::BSOffset cpOffset, cjUINT32 channel);

public:
    cjBSVec3SInterpolator();
    cjBSVec3SInterpolator(cjBSplineDat* bsDat, cjBSplineDat::BSOffset vec3CPOffset=cjBSplineDat::INVALID_BSOFFSET, 
							cjBSplineBasisF3Dat* basisDat=cjNULL);
    cjBSVec3SInterpolator(const cjVec3& poseValue);

    void							SetPoseValue(const cjVec3& poseValue);

protected:
    cjVec3							m_value;
    cjBSplineDat::BSOffset			m_vec3CPOffset;
};

cjRefPtrDefine(cjBSVec3SInterpolator);


//
// cjBSVec3PInterpolator - cjBSInterpolator
//

class CJCORE_API cjBSVec3PInterpolator : public cjBSInterpolator
{
	CJSER_DECL_CLASS(cjBSVec3PInterpolator, cjBSInterpolator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjBSVec3PInterpolator)

public:
	// cjInterpolator
    virtual cjBool Update(cjFLOAT etime, cjRefObj* target, cjVec3& value);
    virtual cjBool IsVec3Value() const { return cjTrue; }

	// cjBSInterpolator
	virtual cjUINT32				GetChannelCount() const { return 1; }
    virtual cjBool					GetChannelPosed(cjUINT32 channel) const;
    virtual cjUINT32				GetFloatValCount(cjUINT32 channel) const;
    virtual cjUINT32				GetDegree(cjUINT32 channel) const;

	virtual cjBSplineDat::BSOffset	GetCPOffset(cjUINT32 channel) const;
    virtual void					SetCPOffset(cjBSplineDat::BSOffset cpOffset, cjUINT32 channel);

public:
    cjBSVec3PInterpolator();
    cjBSVec3PInterpolator(cjBSplineDat* bsDat, cjBSplineDat::BSOffset vec3CPOffset=cjBSplineDat::INVALID_BSOFFSET, 
							cjBSplineBasisF3Dat* basisDat=cjNULL);
    cjBSVec3PInterpolator(const cjVec3& poseValue);

    void							SetPoseValue(const cjVec3& poseValue);

protected:
    cjVec3							m_value;
    cjBSplineDat::BSOffset			m_vec3CPOffset;
};

cjRefPtrDefine(cjBSVec3PInterpolator);



//
// cjBSTMInterpolator - cjBSInterpolator
//

class CJCORE_API cjBSTMInterpolator : public cjBSInterpolator
{
	CJSER_DECL_CLASS(cjBSTMInterpolator, cjBSInterpolator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjBSTMInterpolator)

public:
	// cjInterpolator
    virtual cjBool Update(cjFLOAT etime, cjRefObj* target, cjQuatTM& value);
    virtual cjBool IsTMValue() const { return cjTrue; }

	// cjBSInterpolator
	virtual cjUINT32				GetChannelCount() const { return 3; }
    virtual cjBool					GetChannelPosed(cjUINT32 channel) const;
    virtual cjUINT32				GetFloatValCount(cjUINT32 channel) const;
    virtual cjUINT32				GetDegree(cjUINT32 channel) const;

	virtual cjBSplineDat::BSOffset	GetCPOffset(cjUINT32 channel) const;
    virtual void					SetCPOffset(cjBSplineDat::BSOffset cpOffset, cjUINT32 channel);
	
public:
    cjBSTMInterpolator();
    cjBSTMInterpolator(cjBSplineDat* bsDat, 
        cjBSplineDat::BSOffset posCPOffset = cjBSplineDat::INVALID_BSOFFSET,
        cjBSplineDat::BSOffset rotCPOffset = cjBSplineDat::INVALID_BSOFFSET,
        cjBSplineDat::BSOffset scaleCPOffset = cjBSplineDat::INVALID_BSOFFSET,
        cjBSplineBasisF3Dat* basisDat = cjNULL);

    cjBSTMInterpolator(const cjQuatTM& poseValue);

    void							SetPosePos(const cjVec3& pos);
    void							SetPoseRotate(const cjQuat& rot);
    void							SetPoseScale(const cjVec3& scale);

    void							SetPoseValue(const cjQuatTM& poseValue);

protected:
    cjQuatTM						m_value;

    cjBSplineDat::BSOffset			m_posCPOffset;
    cjBSplineDat::BSOffset			m_rotCPOffset;
    cjBSplineDat::BSOffset			m_scaleCPOffset;
};

cjRefPtrDefine(cjBSTMInterpolator);


#endif

