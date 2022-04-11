
#ifndef _CJRESANI_H_
#define _CJRESANI_H_


class cjAniSeqCB;

CJOBJ_DECL_CLASS_BEGIN(CJCORE_API, cjResAniSeq, cjObj, 0, cjCore)

	CJOBJ_DECL_SERIALIZE
	CJOBJ_DECL_STATIC_REG

public:
	cjResAniSeq();

protected:
	virtual void OnDestroy();

public:
	cjAniSeqRPtr					m_seq;

CJOBJ_DECL_CLASS_END(cjResAniSeq)


//class CJCORE_API cjResAniSeq : public cjObj
//{
//	CJOBJ_DECL_CLASS(cjResAniSeq, cjObj, 0, cjCore)
//	friend class cjAniSeq;
//
//public:
//
//
//
//    enum 
//	{ 
//		MAX_UNIQUE_BASIS_DAT		= 16,
//		MAX_EVALUATOR				= 12
//	};
//
//    enum
//    {        
//        FL_ACCUM_XPOS				= 0x0001,
//        FL_ACCUM_YPOS				= 0x0002,
//        FL_ACCUM_ZPOS				= 0x0004,
//        
//        FL_ACCUM_XROT				= 0x0010,
//        FL_ACCUM_YROT				= 0x0020,
//        FL_ACCUM_ZROT				= 0x0040,
//
//		// front-facing
//        FL_ACCUM_XFACING			= 0x0100,
//        FL_ACCUM_YFACING			= 0x0200,
//        FL_ACCUM_ZFACING			= 0x0400,
//
//        FL_ACCUM_NEG_FACING			= 0x1000,
//        FL_TEMP_POSE				= 0x2000, // save temp pose, automatically removed when completed
//    };
//
//	cjResAniSeq();
//
//	cjFLOAT						CalcTime(cjFLOAT etime) const;
//	
//	const cjName&				GetAccumRootName() const { return m_accumRootName; }
//    void						SetAccumRootName(const cjName& name) { m_accumRootName = name; }
//
//	cjFLOAT						GetDuration() const { return m_duration; }
//    void						SetDuration(cjFLOAT duration) { m_duration = duration; }
//
//    cjAniCntr::ANILOOP			GetLoopType() const { return m_loopType; }
//    void						SetLoopType(cjAniCntr::ANILOOP loopType) { m_loopType = loopType; }
//
//    cjFLOAT						GetFrequency() const { return m_frequency; }
//    void						SetFrequency(cjFLOAT frequency) { m_frequency = frequency; }
//
//    cjExtDatAniNote*			GetAniNote() const { return m_aniNote; }
//
//	cjFLOAT						GetAniNoteTime(const cjNameChar* noteName) const;
//	cjFLOAT						GetAniNoteTime(cjNameMatch* match) const;
//	void						SetAniNote(cjExtDatAniNote* note) { m_aniNote = note; }
//
//    cjUINT32					GetMaxEvaluatorCount() const { return m_maxEvaluatorCount; }
//	void						SetMaxEvaluatorCount(cjUINT32 maxCount=MAX_EVALUATOR) { 
//									cjAssert(maxCount < MAX_EVALUATOR);
//									m_maxEvaluatorCount = maxCount; 
//								}
//
//    cjUINT32					AddEvaluator(cjAniEvaluator* evaluator);
//    cjAniEvaluatorPtr			RemoveEvaluator(cjUINT32 index);
//	void						SortEvaluatorsByPBChannelType();
//	void						SortEvaluators(cjRefObjNameHash* sceneNodeHash);
//
//    cjUINT32					GetEvaluatorCount() const { return m_evaluatorCount; }
//	cjAniEvaluator*				GetEvaluator(cjUINT32 index) const { return m_evaluators[index]; }
//	void						SetEvaluator(cjUINT32 index, cjAniEvaluator* evaluator) { m_evaluators[index] = evaluator; }
//
//	cjUINT32					GetScratchPadBufSiz(cjBool isCumulativeAnimations, cjRefObjNameHash* sceneNodeHash, 
//									cjUINT32& uniqueBasisDatCount) const;
//
//	cjBool						CheckMatchMorphAniNote(cjAniCntrSeq* timeSyncCntrSeq);
//	cjBool						CheckMatchMorphAniNote(cjExtDatAniNote* timeSyncAniNoteExtDat);
//
//    cjBool						CanSyncTo(cjAniSeq* pkTargetSeqData);
//
//    // Accumulation functions.
//	void						GetAccumPos(cjBool& accumX, cjBool& accumY, cjBool& accumZ) const;
//    void						SetAccumPos(cjBool accumX, cjBool accumY, cjBool accumZ);
//
//	void						GetAccumRot(cjBool& accumX, cjBool& accumY, cjBool& accumZ) const;
//    void						SetAccumRot(cjBool accumX, cjBool accumY, cjBool accumZ);
//
//	void						GetAccumFrontFacing(cjBYTE& axis, cjBool& isNeg) const;
//    void						SetAccumFrontFacing(const cjBYTE axis, const cjBool isNeg);
//
//	// evaluator pose flag
//	cjBool						GetTempPoseFlag() const { return (m_flag & FL_TEMP_POSE) ? cjTrue : cjFalse; }
//    void						SetTempPoseFlag(cjBool set=cjTrue) { m_flag &= ~FL_TEMP_POSE; if(set) m_flag |= FL_TEMP_POSE; }
//
//protected:
//	virtual void OnDestroy();
//
//private:
//    cjName						m_accumRootName;
//
//    // Timing variables.
//    cjFLOAT						m_duration;
//    cjAniCntr::ANILOOP			m_loopType;
//    cjFLOAT						m_frequency;
//
//	// aniNote
//    cjExtDatAniNotePtr			m_aniNote;
//
//    // evaluator
//    cjUINT32					m_maxEvaluatorCount;
//    cjUINT32					m_evaluatorCount;
//    cjAniEvaluator**			m_evaluators;
//
//private:
//	cjUINT32					m_flag;
//};

#endif
