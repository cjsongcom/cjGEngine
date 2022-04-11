
#ifndef _CJANICNTRSEQ_H_
#define _CJANICNTRSEQ_H_
 

class cjAniCntrManager;
class cjAniSPad;


//
// cjAniCntrSeq (Recycleable)
//


class CJCORE_API cjAniCntrSeq : public cjRef
{
	CJSER_DECL_CLASS(cjAniCntrSeq, cjRef,0 , CJCORE_API)

public:
    enum
	{
		INVALID_ACTIVATION_ID = 0,
        LODSKIPEVALUATOR = -2,
        EVALMAXCHANNELS = cjAniEvalChan_PRS_Max,
    };

public:
	cjAniCntrSeq();

    cjAniCntrManager*	GetOwner() const { return m_owner; }
    cjBool				SetOwner(cjAniCntrManager* owner);
	
	// sequence
    const cjName&		GetSeqName() const { return m_seq->GetName(); }
    cjBool				SetSeq(cjAniSeq* seq);
	
	// Reactivation/Deactivate
    cjBool				Reactivate(cjINT32 priority, cjFLOAT weight, cjFLOAT easeInTime, cjAniCntrSeq* timeSyncSeq=cjNULL, cjFLOAT stTime=CJ_FTIME_INVALID, cjBool transition=cjFalse, cjBool processCallback=cjTrue);
    cjBool				Deactivate(cjFLOAT easeOutTime, cjBool transitionToNext=cjFalse, cjBool holdTransitionFrame=cjFalse, cjBool processCallback=cjTrue);

		// activation callback
	cjUINT32			GetActivationID() const { return m_activationID; }
	void				AddActivationCallback(cjAniSeqCB* callback) { m_actvationCB.Push(callback); }
	void				RemoveActivationCallback(cjAniSeqCB* callback) 
						{ 
							if(callback) { 
								m_actvationCB.Remove(callback); 
								m_actvationCB.Shrink(); 
							} 
						}
	cjUINT32			GetActivationCallbackCount() const { return m_actvationCB.Count(); }
	cjAniSeqCB*			GetActivationCallbackAt(cjUINT32 index) { return m_actvationCB(index); }

	// Blending
    cjAniCntrSeq*		StartBlend(cjAniSeq* dstSeq, cjFLOAT duration, const cjNameChar* dstTxtKeyName, cjINT32 priority, 
									cjFLOAT srcWeight, cjFLOAT dstWeight, cjAniCntrSeq* dstTimeSyncSeq=cjNULL, cjFLOAT dstFreq=CJANI_SEQUENCE_DAT_FREQ);
    cjAniCntrSeq*		StartBlend(cjAniSeq* dstSeq, cjFLOAT duration, cjFLOAT dstFrame, cjINT32 priority, 
									cjFLOAT srcWeight, cjFLOAT dstWeight, cjAniCntrSeq* dstTimeSyncSeq=cjNULL, cjFLOAT dstFreq=CJANI_SEQUENCE_DAT_FREQ);

    cjAniCntrSeq*		StartMorph(cjAniSeq* dstSeq, cjFLOAT duration, cjINT32 priority, cjFLOAT srcWeight, 
									cjFLOAT dstWeight, cjFLOAT dstFreq=CJANI_SEQUENCE_DAT_FREQ);

	cjFLOAT				FindCorrespondingMorphTime(cjAniCntrSeq* srcCntrSeq, cjFLOAT srcSeqTime);

	// time, update
	void				Update(cjFLOAT etime, cjINT32 lod, cjBool updateEvaluators);

	cjFLOAT				_CalcTime(cjFLOAT etime, cjBool applyTime=cjTrue);
	cjFLOAT				GetLastCalcTime() const { return m_lastCalcTime; }
	cjFLOAT				GetCurTime() const { return m_curTime; }
	cjFLOAT				GetDuration() const { return m_duration; }
	cjFLOAT				GetFrequency() const { return m_frequency; }

    cjAniCntr::ANILOOP	GetLoopType() const { return m_loopType; }
    cjBool				SetLoopType(cjAniCntr::ANILOOP loopType);

	// AniNote
	cjExtDatAniNote*	GetAniNote() const { return m_aniNote; }

    cjFLOAT				GetAniNoteTime(const cjName& noteKey); // GetKeyTimeAt(
	cjFLOAT				GetAniNoteTime(cjNameMatch* noRefMatch);

						//GetTimeAt
	cjFLOAT				GetScaledAniNoteTimeOnlyNotOverETime(const cjName& noteKey, cjFLOAT etime);
	cjFLOAT				GetScaledAniNoteTimeOnlyNotOverETime(cjNameMatch* noRefMatch, cjFLOAT etime);
		
	// Animation State
	cjAniSeq*			GetSeq() const { return m_seq; }
    cjAniSeqState		GetSeqState() const { return m_seqState; }
    cjFLOAT				GetOffset() const { return m_offset; }
	void				SetOffset(cjFLOAT offset) { m_offset = offset; }

	// Seq Control
    void				ResetSequence() { m_offset = CJ_FLT_INVALID; }
	cjAniSeq*			GetSeqDat() const { return m_seq; }
   
	// Evaluator
    cjUINT32			GetEvaluatorCount() const { return m_seq->GetEvaluatorCount(); }
	cjAniEvaluator*		GetEvaluator(cjUINT32 idx) const { return m_seq->GetEvaluator(idx); }
    void				DisableEvaluator(cjUINT32 evalutorIdx, cjBool invalidatePBDat);
    void				DisableEvaluatorChannel(cjUINT32 evaluatorIdx, cjUINT32 channel, cjBool invalidatePBDat);

	cjAniPoseBuf*		GetPoseBuf() const { return m_poseBuf; }
	cjAniSPad*			GetScratchPad() const { return m_spad; }
	cjAniCntrSeq*		GetTimeSyncSequence() const { return m_pairCntrSeq; } 

	// priority, weight
    cjINT32				GetPriority() const { return m_priority; }
    cjFLOAT				GetWeight() const { return m_weight; }
    void				SetWeight(cjFLOAT weight) { m_weight = weight; if(m_weight < 0.0f) m_weight = 0.0f; }
    cjFLOAT				GetSpinnerScaledWeight() const { return m_easeSpinner * m_transSpinner * m_weight; }
    cjFLOAT				GetEaseSpinner() const { return m_easeSpinner; }
    cjFLOAT				GetEaseEndTime() const { return m_enTime; }
    cjFLOAT				GetDestTime() const { return m_dstTime; }

	cjBool				VerifyDependencies(cjAniCntrSeq* cntrSeq);
	cjBool				VerifyMatchingMorphKeys(cjAniCntrSeq* timeSyncSeq);

public:
    static cjAniCntrSeq* Activate(cjAniCntrManager* pkOwner, cjAniSeq* seq, cjINT32 priority, cjFLOAT weight, cjFLOAT easeInTime, 
							cjAniCntrSeq* timeSyncCntrSeq=NULL, cjFLOAT frequency=CJANI_SEQUENCE_DAT_FREQ, cjFLOAT stTime=0.0f, cjBool transition=cjFalse, cjBool holdTransitionFrame=cjFalse);

	cjBool				IsMoreImportantThan(const cjAniCntrSeq* cntrSeq) const;    
	void				PrepareForRecycling();

	void				SetLastAccumTransform(const cjQuatTM& tm) { cjAssert(m_spad); m_spad->SetLastAccumTM(tm); }
	void				ClearAccumTransformData() { cjAssert(m_spad); m_spad->ClearAccumTM(); }

protected:
	cjBool				CheckMatchMorphTextKeys(cjAniCntrSeq* timeSyncCntrSeq);
	
protected:
	cjName										m_name;

	cjAniCntrManager*							m_owner;
	cjUINT32									m_activationID;

	cjTNullableAry<cjAniSeqCB*>					m_actvationCB;
	
	cjAniPoseBufRPtr							m_poseBuf;
	cjWORD										m_poseBufDatCount;
	cjSWORD										m_lastPoseBufUpdateLOD;
	cjFLOAT										m_lastPoseBufUpdateTime;
	    
    cjAniSeqRPtr								m_seq;
    cjAniSeqState								m_seqState;

	cjAniSPadRPtr								m_spad;
	cjExtDatAniNoteRPtr							m_aniNote;
	cjAniCntrSeqRPtr							m_pairCntrSeq;
	
    cjFLOAT										m_duration;
    cjAniCntr::ANILOOP							m_loopType;
    cjFLOAT										m_frequency;
    cjFLOAT										m_lastCalcTime;
    cjFLOAT										m_accumFreqXTime;	// accum frequency * dtime
    cjFLOAT										m_curTime;			// 0.0~1.0 (first frame~last frame)
	
	cjFLOAT										m_offset;
    cjFLOAT										m_stTime;
    cjFLOAT										m_enTime;
    cjFLOAT										m_dstTime;
    cjBool										m_holdDestFrame;

    cjINT32										m_priority;
    cjFLOAT										m_weight;
    cjFLOAT										m_transSpinner;
    cjFLOAT										m_easeSpinner;

};


#endif

