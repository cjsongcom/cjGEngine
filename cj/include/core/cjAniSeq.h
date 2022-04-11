
#ifndef _CJANISEQ_H_
#define _CJANISEQ_H_


class cjAniCntrSeq;
class cjAniCntrManager;
class cjRefObjNameHash;
//cjRefPtrDefine(cjExtDatAniNote)

cjRefPtrForwardDefine(cjAniSeq)
cjRefPtrForwardDefine(cjAniEvaluator)


class CJCORE_API cjAniSeqCB 
	// do not use : public cjMem , causes c2385 ambiguous delete operator
	//										    dupplicate cjMem operator with cjSerializeable derived class
{
public:
	virtual ~cjAniSeqCB() {}
	virtual void OnAniSeqChange(cjAniCntrSeq* cntrSeq, cjAniSeqState state) = 0;
};


//
// cjAniSeqCBItem
//

class CJCORE_API cjAniSeqCBItem : public cjMem
{
public:
    cjAniSeqCBItem() : m_cb(cjNULL), m_man(cjNULL) {}
    void ClearValues() { m_cb = cjNULL; m_man = cjNULL;	}

    cjAniSeqCB*							m_cb;
    cjAniCntrManager*					m_man; // null for all manager
};


//
// cjAniSeq
//

cjRefPtrDefine(cjAniSeq)
typedef cjTNullableAry<cjAniSeqRPtr>	cjAniSeqRPtrAry;


class CJCORE_API cjAniSeq : public cjRef
{
	CJSER_DECL_CLASS(cjAniSeq, cjRef,0 , CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_SERIALIZE_POST
	CJSER_DECL_CLONE(cjAniSeq)

public:
    enum 
	{ 
		MAX_UNIQUE_BASIS_DAT		= 16,
		MAX_EVALUATOR				= 12
	};

    enum
    {        
        FL_ACCUM_XPOS				= 0x0001,
        FL_ACCUM_YPOS				= 0x0002,
        FL_ACCUM_ZPOS				= 0x0004,
        
        FL_ACCUM_XROT				= 0x0010,
        FL_ACCUM_YROT				= 0x0020,
        FL_ACCUM_ZROT				= 0x0040,

		// front-facing
        FL_ACCUM_XFACING			= 0x0100,
        FL_ACCUM_YFACING			= 0x0200,
        FL_ACCUM_ZFACING			= 0x0400,

        FL_ACCUM_NEG_FACING			= 0x1000,
        FL_TEMP_POSE				= 0x2000, // save temp pose, automatically removed when completed
    };

public:
	cjAniSeq(const cjName& name, cjUINT32 maxEvaluatorCount=MAX_EVALUATOR);
	CJSER_DECL_SERIALIZE_ARY(cjAniSeq)
	
	// animation sequence name -- 'idle00'
    const cjName&					GetName() const { return m_name; }
	void							SetName(const cjName& name) { m_name = name; }

	const cjName&					GetAccumRootName() const { return m_accumRootName; }
	void							SetAccumRootName(const cjName& name) { m_accumRootName = name; }

	// time	
    cjFLOAT							CalcTime(cjFLOAT etime); // 0.0 ~ (duration)

	cjFLOAT							GetDuration() const { return m_duration; }
	cjFLOAT							GetDurationDivFreq() const { return m_duration / m_frequency; }

    void							SetDuration(cjFLOAT duration) { m_duration = duration; }
    cjAniCntr::ANILOOP				GetLoopType() const { return (cjAniCntr::ANILOOP)m_loopType; }
    void							SetLoopType(cjAniCntr::ANILOOP loopType) { m_loopType = loopType; }
    cjFLOAT							GetFrequency() const { return m_frequency; }
    void							SetFrequency(cjFLOAT frequency) { m_frequency = frequency; }

    cjFLOAT							GetDivByFreq(cjFLOAT t) const { return (t / m_frequency); }
    cjFLOAT							GetMulByFreq(cjFLOAT t) const { return (t * m_frequency); }


	// text key	
    cjExtDatAniNote*				GetAniNote() const { return m_aniNote; }
    void							SetAniNote(cjExtDatAniNote* note) { m_aniNote = note; }

	cjFLOAT							GetAniNoteTime(const cjNameChar* noteName) const;
    cjFLOAT							GetAniNoteTime(cjNameMatch* match) const;

	// evaluators
    cjINT32							GetMaxEvaluatorCount() const { return m_maxEvaluatorCount; }
    void							SetMaxEvaluatorCount(cjINT32 maxCount);

    cjINT32							GetEvaluatorCount() const { return m_evaluatorCount; }
	cjAniEvaluator*					GetEvaluator(cjINT32 index) const { return m_evaluatorPtrAry[index]; }

    cjUINT32						AddEvaluator(cjAniEvaluator* evaluator);
    cjAniEvaluatorRPtr				RemoveEvaluator(cjUINT32 index);

    void							SortEvaluators(cjRefObjNameHash* sceneNodeHash);
    void							SortEvaluatorsByPBChannelType();

	// evaluator pose flag
	cjBool							IsTempPose() const { return (m_flag & FL_TEMP_POSE) ? cjTrue : cjFalse; }
    void							SetTempPose(cjBool set=cjTrue) { m_flag &= ~FL_TEMP_POSE; if(set) m_flag |= FL_TEMP_POSE; }

	cjUINT32						GetScratchPadBufSiz(cjBool isCumulativeAnimations, cjRefObjNameHash* sceneNodeHash, 
															cjUINT32& uniqueBasisDatCount) const;
	cjBool							CheckMatchMorphAniNote(cjAniCntrSeq* timeSyncCntrSeq);
	cjBool							CheckMatchMorphAniNote(cjExtDatAniNote* timeSyncAniNoteExtDat);

    cjBool							CanSyncTo(cjAniSeq* pkTargetSeqData);

    // Accumulation functions.
	void							GetAccumPos(cjBool& accumX, cjBool& accumY, cjBool& accumZ) const;
    void							SetAccumPos(cjBool accumX, cjBool accumY, cjBool accumZ);

	void							GetAccumRot(cjBool& accumX, cjBool& accumY, cjBool& accumZ) const;
    void							SetAccumRot(cjBool accumX, cjBool accumY, cjBool accumZ);

	void							GetAccumFrontFacing(cjBYTE& axis, cjBool& isNeg) const;
    void							SetAccumFrontFacing(const cjBYTE axis, const cjBool isNeg);

    // activation callback
    void							AddActivationCB(cjAniSeqCB* cb, cjAniCntrManager* man=cjNULL);
    void							RemoveActivationCB(cjAniSeqCB* cb, cjAniCntrManager* man=cjNULL);
    cjUINT32						GetActivationCBCount() const { return m_activationCBCount; }
    void							GetActivationCB(cjUINT32 index, cjAniSeqCB*& cb, cjAniCntrManager*& man);

protected:
    cjAniSeq();
	
	void							_InitCommon();
    void							AdjustActvationCBArySiz(cjUINT32 cbArySiz);

protected:
	cjName							m_name;
	cjName							m_accumRootName;

    cjFLOAT							m_duration;
    cjEnumType						m_loopType;
    cjFLOAT							m_frequency;

	cjExtDatAniNoteRPtr				m_aniNote;

    cjINT32							m_maxEvaluatorCount;
    cjINT32							m_evaluatorCount;
    cjAniEvaluatorRPtr*				m_evaluatorPtrAry;

    cjUINT32						m_activationCBArySiz;
    cjUINT32						m_activationCBCount;
    cjAniSeqCBItem*					m_activationCBs;

private:
	cjUINT32						m_flag;
};


#endif
