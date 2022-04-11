
#ifndef _CJCONTROLLER_H_
#define _CJCONTROLLER_H_


class cjInterpolator;
class cjAniEvaluator;
class cjAniCntrSeq;
class cjBlendInterpolator;


#define cjGetAniCntr(clsname, serObj) ((clsname*)serObj->FindAniCntr(clsname::cjrStaticRTTI()))


//
// Base Animation Controller
//

cjRefPtrDefine(cjAniCntr);


class CJCORE_API cjAniCntr : public cjRef
{
	CJSER_DECL_CLASS_ABSTRACT(cjAniCntr, cjRef,0 , CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_SERIALIZE_POST
	CJSER_DECL_CLONE_ABSTRACT(cjAniCntr)
	CJSER_DECL_CLONE_POST

public: //special serializer
	static cjSerializer& SerializeList(cjSerializer& ser, cjAniCntrRPtr& spAniCntr);
	static cjSerializer& PostSerializeList(cjSerializer& ser, cjAniCntrRPtr& spAniCntr);

public:
	enum ANISTATUS			{ ANISTATUS_DEACTIVE,	ANISTATUS_ACTIVE	};
	enum ANIBEGIN			{ ANIBEGIN_AUTO,		ANIBEGIN_MANUAL		};
	enum ANILOOP			{ ANILOOP_LOOP,			ANILOOP_ONE,		ANILOOP_REVERSE	};
	enum ANIDIR				{ ANIDIR_FORWARD,		ANIDIR_BACKWARD		};
	enum ANITIMEUPDATE		{ ANITIMEUPDATE_OFF,	ANITIMEUPDATE_ON	};
	enum ANIFORCEUPDATE		{ ANIFORCEUPDATE_OFF,	ANIFORCEUPDATE_ON	};
	enum ANIMANANGERCTL		{ ANIMANAGERCTL_OFF,	ANIMANAGERCTL_ON	};
	
CJFLAG_DECL_BEGIN
		MASK_ANISTATUS					= 0x00000003, // 2bit
		BPOS_ANISTATUS					= 0,
		VM_ANISTATUS					= ANISTATUS_ACTIVE,

		MASK_ANIBEGIN					= 0x0000000C, // 2bit
		BPOS_ANIBEGIN					= 2,
		VM_ANIBEGIN						= ANIBEGIN_MANUAL,
		
		MASK_ANILOOP					= 0x00000030, // 2bit
		BPOS_ANILOOP					= 4,
		VM_ANILOOP						= ANILOOP_ONE | ANILOOP_REVERSE,
			
		MASK_ANIDIR						= 0x000000C0, // 2bit
		BPOS_ANIDIR						= 6,
		VM_ANIDIR						= ANIDIR_BACKWARD,
		
		MASK_ANITIMEUPDATE				= 0x00000300, // 2bit
		BPOS_ANITIMEUPDATE				= 8,
		VM_ANITIMEUPDATE				= ANITIMEUPDATE_ON,

		MASK_ANIFORCEUPDATE				= 0x00000C00, // 2bit
		BPOS_ANIFORCEUPDATE				= 10,
		VM_ANIFORCEUPDATE				= ANIFORCEUPDATE_ON,

		MASK_MANAGER_CONTROLLED			= 0x00003000, // 2bit
		BPOS_MANAGER_CONTROLLED			= 12,
		VM_MANAGER_CONTROLLED			= ANIMANAGERCTL_ON
CJFLAG_DECL_END
		
public:
    void SetActive(cjBool active) { _SetFlag(MASK_ANISTATUS, BPOS_ANISTATUS, active); }
    cjBool GetActive() { return _GetFlagAsBool(MASK_ANISTATUS, BPOS_ANISTATUS); }

	void SetBeginType(ANIBEGIN v) { _SetFlagValMask(MASK_ANIBEGIN, BPOS_ANIBEGIN, VM_ANIBEGIN, v); SetActive(v == ANIBEGIN_AUTO);	}
    ANIBEGIN GetBeginType() { return (ANIBEGIN)_GetFlag(MASK_ANIBEGIN, BPOS_ANIBEGIN); }
    
	void SetLoop(ANILOOP v) { _SetFlagValMask(MASK_ANILOOP, BPOS_ANILOOP, VM_ANILOOP, v); }
    ANILOOP GetLoop() { return (ANILOOP)_GetFlag(MASK_ANILOOP, BPOS_ANILOOP); }

	void SetDir(ANIDIR v) { _SetFlagValMask(MASK_ANIDIR, BPOS_ANIDIR, VM_ANIDIR, v); }
    ANIDIR GetDir() { return (ANIDIR)_GetFlag(MASK_ANIDIR, BPOS_ANIDIR); }

	void SetTimeUpdate(ANITIMEUPDATE v) { _SetFlagValMask(MASK_ANITIMEUPDATE, BPOS_ANITIMEUPDATE, VM_ANISTATUS, v); }
	ANITIMEUPDATE GetTimeUpdate() { return (ANITIMEUPDATE)_GetFlag(MASK_ANITIMEUPDATE, BPOS_ANITIMEUPDATE); }

	void SetForceUpdate(cjBool v) { _SetFlag(MASK_ANIFORCEUPDATE, BPOS_ANIFORCEUPDATE, v); }
	cjBool GetForceUpdate() { return _GetFlagAsBool(MASK_ANIFORCEUPDATE, BPOS_ANIFORCEUPDATE); }
	
	void SetManagerControlled(cjBool v) { _SetFlag(MASK_MANAGER_CONTROLLED, BPOS_MANAGER_CONTROLLED, v); }
	cjBool GetManagerControlled() { return _GetFlagAsBool(MASK_MANAGER_CONTROLLED, BPOS_MANAGER_CONTROLLED); }

	//
	// frequency, phase, hi/lo key
	//
	void SetFrequency(cjFLOAT frequency) { m_frequency = frequency; }
	cjFLOAT GetFrequency() { return m_frequency; }

	void SetPhase(cjFLOAT phase) { m_phase = phase; }
	cjFLOAT GetPhase() { return m_phase; }

	void SetStKeyTime(cjFLOAT stKeyTime) { m_stKeyTime = stKeyTime; }
	cjFLOAT GetStKeyTime() { return m_stKeyTime; }
	void SetEnKeyTime(cjFLOAT enKeyTime) { m_enKeyTime = enKeyTime; }
	cjFLOAT GetEnKeyTime() { return m_enKeyTime; }

	cjFLOAT GetStartCalledTime() { return m_startCalcTime; }
	cjFLOAT GetLastCalcAniTime() { return m_lastCalcTime; }

	cjFLOAT GetCurTime() { return m_curTime; }
	
public:
    virtual void SetTarget(cjRefObj* target);
    cjRefObj* GetTarget() { return m_target; }

	virtual cjBool IsTransformController() const { return cjFalse; }

	// ani controller management
	void SetNextCntr(cjAniCntr* next) { m_nextCntr = next; }
	cjAniCntr* GetNextCntr() { return m_nextCntr; }
	virtual cjUINT32 GetNextCntrCount() const;
	static cjAniCntr* FindLastCntr(cjAniCntr*& cntr);
	
	virtual void Start(cjFLOAT etime=CJ_FTIME_INVALID);
    virtual void Stop();

	virtual void Update(cjFLOAT etime) = 0;
	
	static void RecursiveStartAnimations(cjRefObj* obj);
	static void RecursiveStartAnimations(cjRefObj* obj, cjFLOAT etime);
    static void RecursiveStopAnimations(cjRefObj* obj);

	static cjFLOAT FindMinStKeyTime(cjRefObj* obj);
	static void RecursiveGetMinStKeyTime(cjRefObj* obj, cjFLOAT& minStKeyTime);
	
protected:
    cjBool UpdateTime(cjFLOAT etime); // cjTrue: updated , cjFalse: already updated and no need to update(ANITIMERUN_STOP)
	virtual cjFLOAT CalcTimeAndAccumFreqTime(cjFLOAT etime);
	virtual cjBool _CheckAcceptableTarget() = 0;

protected:
	cjAniCntrRPtr							m_nextCntr;

	// static
	// 1.0, time for one cycle (start~end)	// 
	cjFLOAT									m_frequency;			

	cjFLOAT									m_phase;				// frequency offset , useful in particle
    cjFLOAT									m_stKeyTime;
    cjFLOAT									m_enKeyTime;
	
	// update
	cjRefObj*								m_target;

	cjFLOAT									m_startCalcTime;		// valid only in ANIBEGIN_MANUAL, beging updated when called Start() function 
	cjFLOAT									m_lastCalcTime;			// last _CalcAniTimeAccumFreqTime() called time
		
	cjFLOAT									m_curTime;				// range time: m_lokeyTime ~ m_hiKeyTime
	cjFLOAT									m_accumFreqTime;		// += dtime * m_frequency
};


//
// cjInterpolatorCntr
//

const static cjWORD cjInterpolationAniCntr_INVALID_INDEX = (cjWORD)-1;


class CJCORE_API cjInterpolationAniCntr : public cjAniCntr
{
	CJSER_DECL_CLASS_ABSTRACT(cjInterpolationAniCntr, cjAniCntr,0 , CJCORE_API)
	CJSER_DECL_CLONE_ABSTRACT(cjInterpolationAniCntr)

public:
	// cjAniCntr
    virtual void Update(cjFLOAT etime) = 0;
	virtual void ResetTimeExtrema();
	
public:
    static const cjFLOAT INVALID_TIME;

	virtual const cjNameChar* GetCtlrID() { return cjNULL; }

		// pure
    virtual cjUINT32 GetInterpolatorCount() const = 0;
    virtual const cjNameChar* GetInterpolatorID(cjUINT32 index=0) = 0;
    virtual cjWORD GetInterpolatorIndex(const cjNameChar* id) const = 0;
    virtual cjInterpolator* GetInterpolator(cjUINT32 index=0) const = 0;
    virtual void SetInterpolator(cjInterpolator* interpolator, cjUINT32 index=0) = 0;
	
    virtual cjBool UpdateValue(cjFLOAT /*etime*/,	cjBool	   /*v*/, cjUINT32 index=0) { cjUNUSED_VALDEF(index); return cjFalse; }
    virtual cjBool UpdateValue(cjFLOAT ,			cjFLOAT			, cjUINT32 index=0) { cjUNUSED_VALDEF(index); return cjFalse; }
    virtual cjBool UpdateValue(cjFLOAT ,			const cjColorF&	, cjUINT32 index=0) { cjUNUSED_VALDEF(index); return cjFalse; }
    virtual cjBool UpdateValue(cjFLOAT ,			const cjVec3&	, cjUINT32 index=0) { cjUNUSED_VALDEF(index); return cjFalse; }
    virtual cjBool UpdateValue(cjFLOAT ,			const cjQuat&	, cjUINT32 index=0) { cjUNUSED_VALDEF(index); return cjFalse; }
    virtual cjBool UpdateValue(cjFLOAT ,			cjAniCntrSeq* , cjAniEvaluator* , void** , cjUINT32 index=0) { cjUNUSED_VALDEF(index); return cjFalse; }

public:
		// pure
    virtual cjAniEvaluator*			_CreatePoseEvaluator(cjUINT32 index=0) = 0;
    virtual cjInterpolator*			_CreatePoseInterpolator(cjUINT32 index=0) = 0;
    virtual void					_SynchronizePoseInterpolator(cjInterpolator* interpolator, cjUINT32 index=0) = 0;
    virtual cjBlendInterpolator*	_CreateBlendInterpolator(cjUINT32 index=0, cjBool isManagerControlled=cjFalse, cjFLOAT weightThreshold=0.0f, cjBYTE arySiz=2) const = 0;    
	// called from _GuaranteeTimeRange(cjRefObj* obj..)
	virtual void					_GuaranteeTimeRange(cjFLOAT stTime, cjFLOAT enTime) = 0; 

    static void						_RecusriveGuaranteeTimeRange(cjRefObj* obj, cjFLOAT stTime, cjFLOAT enTime, const cjNameChar* excludedCntrType);

protected:
//	virtual cjBool _IsCorrectInterpolator(cjInterpolator* interpolator, cjUINT32 index) const = 0;

};

cjRefPtrDefine(cjInterpolationAniCntr);


//
// cjInterpolationCntr
//

cjRefPtrForwardDefine(cjInterpolator);

class CJCORE_API cjSingleInterpolationAniCntr : public cjInterpolationAniCntr
{
	CJSER_DECL_CLASS_ABSTRACT(cjSingleInterpolationAniCntr, cjInterpolationAniCntr,0 , CJCORE_API)
	CJSER_DECL_CLONE_ABSTRACT(cjSingleInterpolationAniCntr)
	CJSER_DECL_CLONE_POST

public:
		// cjAniCntr
	virtual void					ResetTimeExtrema();
	virtual cjBool					_CheckAcceptableTarget();

		// cjInterpolationAniCntr
	virtual cjUINT32				GetInterpolatorCount() const { return 1; }
	virtual const cjNameChar*		GetInterpolatorID(cjUINT32 ) { return cjNULL; }
	virtual cjWORD					GetInterpolatorIndex(const cjNameChar* ) const { return 0;}
	virtual cjInterpolator*			GetInterpolator(cjUINT32 index=0) const;
	virtual void					SetInterpolator(cjInterpolator* interpolator, cjUINT32 index=0);

    virtual void					_GuaranteeTimeRange(cjFLOAT fStartTime, cjFLOAT fEndTime);
	
protected:
    virtual cjBool					_CheckAcceptableInterpolator(cjRefObj* target) const = 0;

protected:
	cjInterpolatorRPtr				m_spInterpolator;
};

cjRefPtrDefine(cjSingleInterpolationAniCntr);


//
// cjSingleInterpolationAniCntr
//  - cjBoolInterpAniCntr
//

class CJCORE_API cjBoolInterpAniCntr : public cjSingleInterpolationAniCntr
{
	CJSER_DECL_CLASS_ABSTRACT(cjBoolInterpAniCntr, cjSingleInterpolationAniCntr, 0, CJCORE_API)
	CJSER_DECL_CLONE_ABSTRACT(cjBoolInterpAniCntr)

public:
		// cjInterpolationAniCntr
    virtual cjAniEvaluator*			_CreatePoseEvaluator(cjUINT32 index=0);
    virtual cjInterpolator*			_CreatePoseInterpolator(cjUINT32 index=0);
    virtual void					_SynchronizePoseInterpolator(cjInterpolator* interpolator, cjUINT32 index=0);
	virtual cjBlendInterpolator*	_CreateBlendInterpolator(cjUINT32 index=0, cjBool isManagerControlled=cjFalse, 
										cjFLOAT weightThreshold=0.0f, cjBYTE arySiz=2) const;

public:
	virtual void					GetTargetValueBool(cjBool& value) = 0;
};

cjRefPtrDefine(cjBoolInterpAniCntr)


//
// cjSingleInterpolationAniCntr
//  - cjBoolInterpAniCntr
//    - cjVisibleAniCntr
//

class CJCORE_API cjVisibleAniCntr : public cjBoolInterpAniCntr
{
	CJSER_DECL_CLASS(cjVisibleAniCntr, cjBoolInterpAniCntr, 0, CJCORE_API)
	CJSER_DECL_CLONE(cjVisibleAniCntr)

public:
	// cjAniCntr
	virtual void Update(cjFLOAT etime);

	// cjInterpolationAniCntr
	virtual cjBool UpdateValue(cjFLOAT etime, cjBool v, cjUINT32 index=0);

	// cjSingleInterpolationAniCntr
	cjBool _CheckAcceptableInterpolator(cjRefObj* target) const;

	// cjBoolInterpAniCntr
	virtual void GetTargetValueBool(cjBool& value); // get boolean from object (m_target)
       
public:
	cjVisibleAniCntr();

};

cjRefPtrDefine(cjVisibleAniCntr);


//
// cjTransformAniCntr
//

class CJCORE_API cjTransformAniCntr : public cjSingleInterpolationAniCntr
{
	CJSER_DECL_CLASS(cjTransformAniCntr, cjSingleInterpolationAniCntr,0 , CJCORE_API)
	CJSER_DECL_CLONE(cjTransformAniCntr)

public:
		// cjAniCntr
	virtual void Update(cjFLOAT etime);
	virtual cjBool IsTransformController() const { return cjTrue; }
	
		// cjInterpolationAniCntr
    virtual cjAniEvaluator*			_CreatePoseEvaluator(cjUINT32 index=0);
    virtual cjInterpolator*			_CreatePoseInterpolator(cjUINT32 index=0);
    virtual void					_SynchronizePoseInterpolator(cjInterpolator* interpolator, cjUINT32 index=0);

	virtual cjBlendInterpolator*	_CreateBlendInterpolator(cjUINT32 index=0, cjBool isManagerControlled=cjFalse, cjFLOAT weightThreshold=0.0f, cjBYTE arySiz=2) const;
	//virtual cjBool _IsCorrectInterpolator(cjInterpolator* interpolator, cjUINT32 index) const;

		// cjSingleInterpolationAniCntr
    virtual cjBool _CheckAcceptableInterpolator(cjRefObj* target) const;
	
public:
    cjTransformAniCntr();
};

cjRefPtrDefine(cjTransformAniCntr)


//
// cjAniCntrManager 
//



#endif
