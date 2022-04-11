
#ifndef _CJANICNTRMAN_H_
#define _CJANICNTRMAN_H_


class cjAniPoseBind;
class cjMesh;


//cjtemp
//template<>
//struct cjTPairCloner<const cjName, cjAniSeq*, cjTCloneByRefCopy, cjTCloneByCloneCopy>
//{
//	void operator()(cjTPair<const cjName, cjAniSeq*>& dst, const cjTPair<const cjName, cjAniSeq*>& src) const
//	{
//		dst.m_key = src.m_key;
//		dst.m_val = src.m_val;
//	}
//};


//
// cjAniCntrManager
//

class CJCORE_API cjAniCntrManager : public cjAniCntr
{
	CJSER_DECL_CLASS(cjAniCntrManager, cjAniCntr, 0, CJCORE_API)
	CJSER_DECL_CLONE(cjAniCntrManager)
	CJSER_DECL_CLONE_POST
	
public:
    virtual void				Start(cjFLOAT)		{}
    virtual void				Start()				{}
    virtual void				Stop()				{}
    virtual void				SetTarget(cjRefObj*){}

	virtual void				Update(cjFLOAT etime);

protected:
    virtual cjBool				_CheckAcceptableTarget(); 

public:	
	static cjAniCntrManager*	FindCntrManager(cjRefObj* obj); // find recursively in node
	static cjRefObj*			FindAccumRoot(cjRefObj* obj);

	cjAniCntrManager(cjRefObj* target, cjBool cumulativeAni=cjFalse, 
		cjAniPoseBind* poseBind=cjNULL, cjBool moveAccumTMToChild=cjTrue);

	cjRefObjNameHash*			GetNodesHash()				const	{ return m_nodesHash;	}
    cjAniPoseBind*				GetPoseBind()				const	{ return m_poseBind;	}
	cjAniPoseBlend*				GetPoseBlend()				const	{ return m_poseBlend; }
    cjAniMTargetPoseHandler*	GetMTPoseHandler()			const	{ return m_hdrMTPose;	}
	
	cjUINT32					GetMaxUniqueBasisDats()				{ return m_maxUniqueBasisDatCount; }
	cjUINT32					GetMaxSPadBufSiz()					{ return m_maxSPadBufSiz; }
    cjSWORD						GetMaxEvaluators()			const	{ return m_maxEvaluatorCount; }
    cjSWORD						GetMaxUniqueBasisDatas()	const	{ return m_maxUniqueBasisDatCount; }
    cjUINT32					GetMaxScratchPadBufferSize()const	{ return m_maxSPadBufSiz; }

    cjBool						IsCumulativeAni()			const	{ return m_cumulative; }
    void						ClearCumulativeAni();

	const cjName&				GetAccumRootName() const			{ cjAssert(m_hdrMTPose); return m_hdrMTPose->GetAccumRootName(); }
    cjRefObj*					GetAccumRoot() const				{ cjAssert(m_hdrMTPose); return m_hdrMTPose->GetAccumRoot(); }
    cjBool						GetAccumulatedTM(cjQuatTM& tm) const;
    void						SetAccumulatedTM(const cjQuatTM& tm);

    cjBool						IsKnownSeq(const cjAniSeq* seq) const;

	// cjAniSeq
	cjAniSeq*					AddSeq(const cjNameChar* filename, cjUINT32 index);
    cjAniSeq*					AddSeq(const cjFStream& stream, cjUINT32 index);
	cjBool						AddSeq(cjAniSeq* seq);

    cjBool						AddMultipleSeq(const cjNameChar* filename);
    cjAniSeq*					AddPoseSeq(const cjAniSeq* seqToSync, cjBool tmpPose=cjTrue, const cjName& name=cjNULL);

	cjAniSeqRPtr				RemoveSeq(const cjName& name);
	cjAniSeqRPtr				RemoveSeq(cjAniSeq* seq);
	void						RemoveAllSeq();
    void						RemoveAllDeActiveCntrSeq(cjBool ignoreRefCount=cjFalse);

	cjUINT32					GetSeqCount() const { return m_seqs.Count(); }
	cjAniSeq*					GetSeq(cjUINT32 index) const { return m_seqs(index); }
    cjAniSeq*					GetSeqByName(const cjName& name) const;
	
	// cjAniCntrSeq
    void						AddCntrSeq(cjAniCntrSeq* cntrSeq); 
    
	void						RemoveCntrSeq(cjAniCntrSeq* cntrSeq);
	void						RemoveAllCntrSeq();

	cjUINT32					GetCntrSeqCount() const { return m_cntrSeqs.Count(); }
	cjAniCntrSeq*				GetCntrSeq(cjAryOff index) const { cjAssert(index < m_cntrSeqs.Count()); return m_cntrSeqs(index); }
    cjAniCntrSeq*				GetCntrSeqByName(const cjName& seqName) const;
    cjAniCntrSeq*				GetCntrSeqBySeq(const cjAniSeq* seq) const;

	cjBool						SetCntrSeqWeight(const cjName& cntrSeqName, cjFLOAT weight);
    cjBool						SetCntrSeqWeight(cjAniCntrSeq* cntrSeq, cjFLOAT weight);
	
	// Activate/Deactivate
    cjUINT32					GetNextActivationID() { return ++m_activationID; }

    cjAniCntrSeq*				ActivateSeq(const cjName& seqName, cjINT32 priority=0, cjFLOAT weight=1.0f, cjFLOAT easeInTime=0.0f, cjAniCntrSeq* timeSyncCntrSeq=cjNULL, cjFLOAT frequency=CJANI_SEQUENCE_DAT_FREQ, cjFLOAT stTime=0.0f);
    cjAniCntrSeq*				ActivateSeq(cjAniSeq* seq, cjINT32 priority=0, cjFLOAT weight=1.0f, cjFLOAT easeInTime=0.0f, cjAniCntrSeq* timeSyncCntrSeq=cjNULL, cjFLOAT frequency=CJANI_SEQUENCE_DAT_FREQ, cjFLOAT stTime=0.0f);
    cjAniCntrSeq*				ActivateSeq(cjAniSeq* seq, cjINT32 priority, cjFLOAT weight, cjFLOAT easeInTime, cjAniCntrSeq* timeSyncCntrSeq, cjFLOAT frequency, cjFLOAT stTime, cjBool isTransition, cjBool holdTransitionTime);

    cjBool						DeactivateCntrSeq(cjAniCntrSeq* cntrSeq, cjFLOAT easeOutTime=0.0f);
    cjBool						DeactivateCntrSeq(const cjName& cntrSeqName, cjFLOAT easeOutTime=0.0f);
    void						DeactivateAllCntrSeq(cjFLOAT easeOutTime=0.0f);

    // CrossFade / Blend / Morph / BlendFromPose
    cjAniCntrSeq*				CrossFade(const cjName& srcCntrSeqName, const cjName& dstSeqName, cjFLOAT duration, cjINT32 priority=0, cjFLOAT dstWeight=1.0f, const cjName& dstTimeSyncCntrSeqName=cjNULL, cjFLOAT dstFrequency=CJANI_SEQUENCE_DAT_FREQ);
    cjAniCntrSeq*				CrossFade(cjAniCntrSeq* srcCntrSeq, cjAniSeq* dstSeq, cjFLOAT duration, cjINT32 priority=0, cjFLOAT dstWeight=1.0f, cjAniCntrSeq* dstTimeSyncCntrSeq=cjNULL, cjFLOAT dstFrequency=CJANI_SEQUENCE_DAT_FREQ);

    cjAniCntrSeq*				Morph(const cjName& srcCntrSeqName, const cjName& dstSeqName, cjFLOAT duration, cjINT32 priority=0, cjFLOAT srcWeight=1.0f, cjFLOAT dstWeight=1.0f, cjFLOAT dstFreq=CJANI_SEQUENCE_DAT_FREQ);
    cjAniCntrSeq*				Morph(cjAniCntrSeq* srcCntrSeq, cjAniSeq* dstSeq, cjFLOAT duration, cjINT32 priority=0, cjFLOAT srcWeight=1.0f, cjFLOAT dstWeight=1.0f, cjFLOAT dstFreq=CJANI_SEQUENCE_DAT_FREQ);
    cjBool						StopMorph(const cjName& srcCntrSeqName,const cjName& dstCntrSeqName, cjFLOAT easeOutTime=0.0f);
	cjBool						StopMorph(cjAniCntrSeq* srcCntrSeq, cjAniCntrSeq* dstCntrSeq, cjFLOAT easeOutTime=0.0f);

    cjAniCntrSeq*				BlendFromSeq(const cjName& srcCntrSeqName, const cjName& dstSeqName, cjFLOAT duration, const cjName& dstTextKey, cjINT32 priority=0, cjFLOAT srcWeight=1.0f, cjFLOAT dstWeight=1.0f, const cjName& dstTimeSyncCntrSeqName=cjNULL, cjFLOAT dstFreq=CJANI_SEQUENCE_DAT_FREQ);
	cjAniCntrSeq*				BlendFromSeq(const cjName& srcCntrSeqName, const cjName& dstSeqName, cjFLOAT duration, cjFLOAT dstTime=0.0f, cjINT32 priority=0, cjFLOAT srcWeight=1.0f, cjFLOAT dstWeight=1.0f, const cjName& dstTimeSyncCntrSeqName=cjNULL, cjFLOAT dstFreq=CJANI_SEQUENCE_DAT_FREQ);
    cjAniCntrSeq*				BlendFromSeq(cjAniCntrSeq* cntrSeq, cjAniSeq* dstSeq, cjFLOAT duration, const cjName& dstTxtKey, cjINT32 priority=0, cjFLOAT srcWeight=1.0f, cjFLOAT dstWeight=1.0f, cjAniCntrSeq* dstTimeSyncSeq=cjNULL, cjFLOAT dstFreq=CJANI_SEQUENCE_DAT_FREQ);
    cjAniCntrSeq*				BlendFromSeq(cjAniCntrSeq* cntrSeq, cjAniSeq* dstSeq, cjFLOAT duration, cjFLOAT dstTime=0.0f, cjINT32 priority=0, cjFLOAT srcWeight=1.0f, cjFLOAT dstWeight=1.0f, cjAniCntrSeq* dstTimeSyncSeq=cjNULL, cjFLOAT dstFreq=CJANI_SEQUENCE_DAT_FREQ);

    cjBool						StopBlendFromSeq(const cjName& srcCntrSeqName, const cjName& dstCntrSeqName, cjFLOAT easeOutTime=0.0f);
    cjBool						StopBlendFromSeq(cjAniCntrSeq* srcCntrSeq, cjAniCntrSeq* dstCntrSeq, cjFLOAT easeOutTime=0.0f);

    cjBool						BlendFromPose(const cjName& dstSeqName, const cjName& dstTxtKey, cjFLOAT duration, cjINT32 priority, const cjName& seqToSyncName, cjAniCntrSeq*& poseCntrSeq, cjAniCntrSeq*& dstCntrSeq);
    cjBool						BlendFromPose(const cjName& dstSeqName, cjFLOAT dstTime, cjFLOAT duration, cjINT32 priority, const cjName& seqToSyncName, cjAniCntrSeq*& poseCntrSeq, cjAniCntrSeq*& dstCntrSeq);
    cjBool						BlendFromPose(cjAniSeq* dstSeq, const cjName& dstTxtKey, cjFLOAT duration, cjINT32 priority, cjAniSeq* seqToSync, cjAniCntrSeq*& poseCntrSeq, cjAniCntrSeq*& dstCntrSeq);
    cjBool						BlendFromPose(cjAniSeq* dstSeq, cjFLOAT dstTime, cjFLOAT duration, cjINT32 priority, cjAniSeq* seqToSync, cjAniCntrSeq*& poseCntrSeq, cjAniCntrSeq*& dstCntrSeq);

	// cjAniCntrSeq Pool	
    cjSWORD						GetFreeCntrSeqCount() const { return (cjSWORD)m_freeCntrSeqs.Count(); }
    cjSWORD						GetMaxFreeCntrSeqCount() const { return m_maxFreeCntrSeqCount; }
    void						SetMaxFreeCntrSeqCount(cjSWORD maxFreeCntrSeqCount);
    void						RemoveAllFreeCntrSeqs();
	cjAniCntrSeqRPtr			GetFreeCntrSeq();
//
protected:
	cjAniCntrManager();

    void						Init(cjRefObj* target, cjBool cumulativeAni, cjAniPoseBind* poseBind, cjBool moveAccumTMToChild);
    void						Shutdown();

    cjBool						PushFreeCntrSeq(cjAniCntrSeq* cntrSeq);

protected:
	typedef cjTSerHash<cjName, cjAniSeq*, 
		cjTPairCloner<cjName, cjAniSeq*, cjTCloneByCopy, cjTCloneByCopy>, 
		cjTHashDefaultNoneRemove<cjAniSeq*> > cjTHashName2AniSeq;

	cjUINT32												m_activationID;

	cjUINT32												m_maxSPadBufSiz;
	cjSWORD													m_maxEvaluatorCount;
	cjSWORD													m_maxUniqueBasisDatCount;
	cjSWORD													m_maxFreeCntrSeqCount;
	
	cjAniSeqRPtrAry											m_seqs;
	cjTHashName2AniSeq										m_seqNameHash;

	cjTNullableAry<cjAniCntrSeqRPtr>						m_cntrSeqs;
	cjTNullableAry<cjAniCntrSeqRPtr>						m_freeCntrSeqs;

	cjRefObjNameHashRPtr									m_nodesHash;
	cjAniPoseBindRPtr										m_poseBind;
	cjAniPoseBlendRPtr										m_poseBlend;
	cjAniMTargetPoseHandlerRPtr								m_hdrMTPose;

	cjBool													m_cumulative;
};

cjRefPtrDefine(cjAniCntrManager)


//
// cjAnimator
//

class CJCORE_API cjAnimator : public cjRefObj, public cjAniSeqCB
{
	friend class cjResChr;

	CJSER_DECL_CLASS(cjAnimator, cjRefObj, 0, CJCORE_API)
	CJSER_DECL_CLONE_MANUAL(cjAnimator)

public:
		// cjAniSeqCB
    virtual void				OnAniSeqChange(cjAniCntrSeq* cntrSeq, cjAniSeqState seqState);

public:
	static cjAnimator*			Create(const cjNameChar* resCharName, cjBool isCumulativeAni=cjFalse, cjAniPoseBind* poseBind=cjNULL);

	void						Update(cjFLOAT etime);
	void						Reset();
	
	//
	// Mesh
	//
//	cjBool						ReloadMesh(cjMesh* mesh, cjAniPoseBind* poseBind=cjNULL);
    cjBool						ChangeSceneRoot(cjRefObj* sceneRoot, cjRefObj* sceneModelRoot=cjNULL, 
									cjAniPoseBind* poseBind=cjNULL);

	void						RefreshControllerManager();
	cjRefObj*					GetSceneRoot() const { return m_sceneRoot; }

	cjRefObj*					GetAccumRoot() const { return m_aniCntrMan->GetAccumRoot(); }
    cjRefObj*					GetAniCntrTarget() const // model root
								{
									cjAssert(m_aniCntrMan && m_aniCntrMan->GetTarget());
									return m_aniCntrMan->GetTarget();
								}

	//
	// animation
	//
	
	cjAnimatorDat*				GetAnimatorDat() const { return m_animatorDat; }
    cjAniCntrManager*			GetAniCntrManager() const { return m_aniCntrMan; }

		// callback
	cjAnimatorCB*				GetCallback() const { return m_animatorCB; }
	void						SetCallback(cjAnimatorCB* animatorCB) { m_animatorCB = animatorCB; }

	cjBool						RegCallback(cjAnimatorEvtType evtType, cjAniSeqID seqID, const cjName& txtKeyName);
	cjBool						_RegCallback(cjAnimatorEvtType evtType, cjAniSeqID seqID, cjNameMatch* refNameMatch=cjNULL); // causes m_needToBuildTimeline = cjTrue
	void						UnRegCallback(cjAnimatorEvtType evtType, cjAniSeqID seqID, const cjName& txtKeyName);    
    void						UnRegCallback(cjAnimatorEvtType evtType, cjAniSeqID seqID, cjNameMatch* noRefNameMatch=cjNULL);
	void						ClearAllRegCallbacks(); // m_needToBuildTimeline = cjTrue;

		// sequence management    
	cjAniCntrSeq*				ActivateSeq(cjAniSeqID seqID, cjINT32 priority=0, cjFLOAT seqWeight=1.0f, cjFLOAT easeInTime=0.0f, 
											cjAniSeqID timeSyncSeqID=cjAniSeqID_InvalidSeq, cjFLOAT freq=cjAnimatorSeqDatFrequency, 
											cjFLOAT stTime=0.0f);
    cjBool						DeactivateSeq(cjAniSeqID seqID, cjFLOAT easeOutTime=0.0f);
	
    cjAniSeq*					GetSeq(cjAniSeqID seqID) const;
    cjAniCntrSeq*				GetActiveSeq(cjAniSeqID seqID, cjBool checkExtraSeq, cjBool checkStateSeq) const;
    cjAniSeqID					GetSeqID(cjAniCntrSeq* cntrSeq, cjBool checkExtraSeq, cjBool checkStateSeq) const;

    cjAnimatorRst				ChangeSeqID(cjAniSeqID eOldID, cjAniSeqID eNewID);
    cjBool						ChangeSeq(cjAniSeqID seqID, cjAniSeq* seq);

    void						UnloadSeq(cjAniSeqID seqID);

    cjAniSeqID					FindSeqID(const cjNameChar* seqName) const;

    cjAniSeqID					GetCurSeqID() const { return m_curSeqID; }
    cjAniSeqID					GetNextSeqID() const { return m_nextSeqID; }
    cjAniSeqID					GetTargetSeqID() const { return m_targetSeqID; }
    cjBool						SetTargetSeqID(cjAniSeqID seqID);

	// event/timeline
    cjFLOAT						GetNextEventTime(cjAnimatorEvtType evtType, cjAniSeqID seqID, const cjName& txtKey);
    cjFLOAT						GetNextEventTime(cjAnimatorEvtType evtType, cjAniSeqID seqID, cjNameMatch* noRefNameMatch=cjNULL);

    void						RebuildTimeline() { m_needToBuildTimeline = cjTrue; }

    cjBool						FindAniTransitionCompletionInfo(cjAniSeqID transSrcID, cjAniSeqID transDstID, 
									cjAniTransition* transition, cjFLOAT stTime, cjFLOAT enTime, 
									cjAnimatorCompletionInfo* rstInfo);

	cjAnimatorTransState		GetTransitionState() const { return m_transState; }


protected:
	cjAnimator();
    cjAnimator(cjAnimatorDat* animatorDat, cjBool isCumulativeAni);

//	cjBool						Initialize(cjFStream& kStream, cjAniPoseBind* poseBind);
//  cjBool						LoadNIFFile(cjFStream& kStream, cjBool bLoadNIFFile, cjAniPoseBind* poseBind);


	//
	// animation
	//

		// sequence management
	cjBool						AddSeqDat(cjAniSeqID seqID, cjFStream& kStream, cjBool bLoadKFFile);
    cjAniCntrSeq*				GetExtraSeq(cjAniSeqID seqID) const;
    void						RemoveInactiveExtraSequences();

	cjAnimatorChainCompletionInfo* FillChainComplete(cjAniSeqID srcSeqID, cjAniSeqID dstSeqID, cjFLOAT duration, 
									cjFLOAT& rstSrcBeginTime, cjFLOAT& totalTime);

		// callback
    void						ProcessCallbacks();

		// timeline
    void						BuildTimeline();
    void						AddTimelineDataObject(cjAnimatorEvtType evtType, cjAniSeqID seqID, cjNameMatch* refNameMatch);
    void						RemoveTimelineDataObjects(cjAnimatorEvtType evtType, cjAniSeqID seqID, cjNameMatch* noRefNameMatch);

		// event
    cjAniTxtMarkerPair*			GetNextBlendPair(cjUINT32 transSrcID, cjUINT32, cjAniTransition* transition, 
									cjFLOAT etime, cjFLOAT& nextBlendPairTime);

	cjFLOAT						GetNextAnimActivatedTime(cjAniSeqID seqID);
    cjFLOAT						GetNextAnimDeactivatedTime(cjAniSeqID seqID);
    cjFLOAT						GetNextTextKeyEventTime(cjAniSeqID seqID, cjNameMatch* noRefNameMatch);
    cjFLOAT						GetNextTextKeyEventTime(cjAniSeqID seqID, const cjName& txtKeyName);
    cjFLOAT						GetNextEndOfSequenceTime(cjAniSeqID seqID);

	void						MakeAniEvtActivated(cjAniSeqID evtSeqID);
    void						MakeAniEvtDeactivated(cjAniSeqID evtSeqID);
    void						MakeAniEvtCompleted(cjAniSeqID evtSeqID);
	

protected:
	cjAnimatorDatRPtr						m_animatorDat;

    cjRefObjRPtr							m_sceneRoot;
	cjRefObjRPtr							m_aniCntrManTarget;  // in m_sceneRoot

    cjAniCntrManagerRPtr					m_aniCntrMan;

    cjAnimatorCB*							m_animatorCB;
	cjTNullableAry<cjAnimatorCBDatRPtr>		m_animatorCBDats;

    cjAniSeqID								m_targetSeqID;
	cjName									m_targetKey;
    
	cjBool									m_needToBuildTimeline;
	cjFLOAT									m_curTime;	
    cjFLOAT									m_timelineUpdateTime;	
    
	// transition
    cjBool									m_isInTransition;
    cjAnimatorTransState					m_transState;
    cjFLOAT									m_transStTime;
    cjFLOAT									m_transEnTime;
    cjFLOAT									m_transStFrameTime;
	
	cjAniSeqID								m_curSeqID;
	cjAniCntrSeqRPtr						m_curCntrSeq;
        
    cjAniSeqID								m_nextSeqID;
    cjAniSeqRPtr							m_nextSeq;
    cjAniCntrSeqRPtr						m_nextCntrSeq;

    cjFLOAT									m_nextChainTime;

	typedef cjTList<cjUINT32>				ChainIDLst;
	typedef cjTList<cjFLOAT>				ChainDurationLst;

	ChainIDLst								m_chainIDs;	
    ChainDurationLst						m_chainDurations;
    cjBool									m_isChainActive;

    cjBool									m_isCumulativeAniRequire;

	typedef cjTHash<cjAniSeqID, cjAniSeqRPtr> SeqIDToSeqRPtrHash;
	SeqIDToSeqRPtrHash						m_seqMap;

	typedef cjTList<cjAnimatorTimeline>		TimelineDatLst;
    TimelineDatLst							m_timelineDats;

	typedef cjTHash<cjAniSeqID, cjAniCntrSeqRPtr> SeqIDToCntrSeqRPtrHash;
    SeqIDToCntrSeqRPtrHash					m_extCntrSeqMap;	
};

cjRefPtrDefine(cjAnimator)


#endif

