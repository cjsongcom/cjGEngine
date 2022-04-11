
#ifndef _CJANIEVALUATOR_H_
#define _CJANIEVALUATOR_H_


class cjAniEvalSPDat;
class cjRefObjNameHash;
class cjBSplineBasisF3Dat;
class cjTMEvaluator;


//
// cjAniEvaluator
//

cjRefPtrDefine(cjAniEvaluator)

class CJCORE_API cjAniEvaluator : public cjRefObj
{
	CJSER_DECL_CLASS_ABSTRACT(cjAniEvaluator, cjRefObj,0 , CJCORE_API)
	CJSER_DECL_SERIALIZE
//	CJSER_DECL_SERIALIZE_ARY(cjAniEvaluator)
	CJSER_DECL_SERIALIZE_PTR_ARY(cjAniEvaluator)

	CJSER_DECL_CLONE_ABSTRACT(cjAniEvaluator)

public:
	// tag
	const cjAniEvaluatorTag&		GetTag() const { return m_tag; }
	void							SetTag(const cjAniEvaluatorTag& tag) { m_tag = tag; }

	const cjName&					GetID() const			{ return m_tag.GetID();				}
	const cjName&					GetPropRttiName() const	{ return m_tag.GetPropRttiName();	}
	const cjName&					GetCntrRtti() const		{ return m_tag.GetCntrRttiName();	}
	const cjName&					GetCntrID() const		{ return m_tag.GetCntrID();			}
	const cjName&					GetEvaluatorID() const	{ return m_tag.GetEvaluatorID();	}

    void							GetTagInfo(const cjNameChar*& id, const cjNameChar*& propRttiName, const cjNameChar*& cntrRttiName, const cjNameChar*& cntrID, const cjNameChar*& evaluatorID) const;
    void							GetTagInfo(cjName& id, cjName& propRttiName, cjName& cntrRttiName, cjName& cntrID, cjName& evaluatorID) const;
	void							SetTagInfo(const cjName& id, const cjName& propRttiName, const cjName& cntrRttiName, const cjName& cntrID, const cjName& evaluatorID);
		
	// channel management 
    cjBool							IsChannelItemInvalid(cjUINT32 channel) const { return m_channelItem[channel] == cjAniEvalChanItem_NoData; }
	cjBool							IsChannelItemPosed(cjUINT32 channel) const { return (m_channelItem[channel] & cjAniEvalChanItemMask_Constant) != 0; }
	void							SetChannelItemPosed(cjUINT32 channel) { m_channelItem[channel] |= cjAniEvalChanItemMask_Constant; }

		// 3: TransformEvaluator, 1: Other Evaluator
	cjUINT32						GetMaxChannels() const	{ return (m_channelItem[cjAniEvalChan_Flag] & cjAniEvalFlagChanVal_Transform) ? 3 : 1; }
	cjBool							IsChanneItemlInvalid(cjUINT32 channel) const { return m_channelItem[channel] == cjAniEvalChanItem_NoData; }

	// EvaluatorChanItem <-> PoseBufChanItem
		// [channel item which has flag (cjAniEvalFlagChanVal_Referenced)] is converted cjAniPoseBufChanItem_Ref
		// channel flag (cjAniEvalFlagChanVal_Referenced)
	cjAniPoseBufChanItem			GetPBChannelItemFromChanItem(cjUINT32 channel) const; 
	void							GetPBChannelItemFromChanItem(cjUINT32 channel, cjAniPoseBufChanItem& pbChannelItem, cjBool& isPosed) const;
	cjAniPoseBufChanItem			GetPBChannelItemFromChanItemWithNoCareFlag(cjUINT32 channel) const; // do not care flag channel value

	// flag channel
	cjBool							IsTransformEvaluator()	const { return (m_channelItem[cjAniEvalChan_Flag] & cjAniEvalFlagChanVal_Transform) != 0;		}
    cjBool							IsReferenced()			const { return (m_channelItem[cjAniEvalChan_Flag] & cjAniEvalFlagChanVal_Referenced) != 0;		}
    cjBool							IsShutdownRequired()	const { return (m_channelItem[cjAniEvalChan_Flag] & cjAniEvalFlagChanVal_ShutDown) != 0;		}
    cjBool							IsAlwaysUpdate()		const { return (m_channelItem[cjAniEvalChan_Flag] & cjAniEvalFlagChanVal_AlwaysUpdate) != 0;	}

	void							SetChannelFlagAlwaysUpdate(cjBool enabled=cjTrue) {
										if(enabled) m_channelItem[cjAniEvalChan_Flag] |= cjAniEvalFlagChanVal_AlwaysUpdate;
										else m_channelItem[cjAniEvalChan_Flag] &= ~cjAniEvalFlagChanVal_AlwaysUpdate; }

	void							SetChannelFlagReferenced() { m_channelItem[cjAniEvalChan_Flag] |= cjAniEvalFlagChanVal_Referenced; }


	virtual cjBool					GetChannelPosedValue(cjUINT32 channel, void* rst) const { return cjFalse; } // override at derived class

	// update and time
    virtual cjBool					UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const { return cjFalse; }

	virtual void					GetValidTimeRange(cjFLOAT& stTime, cjFLOAT& enTime) const { stTime = 0.0f; enTime = 0.0f; }
	virtual void					CacheKeys(cjFLOAT , cjFLOAT ) {}
    virtual cjAniEvaluator*			GetEvaluator(cjFLOAT stTime, cjFLOAT enTime) { cjAniEvaluator* evaluator = (cjAniEvaluator*)Clone(); return evaluator; }
	
	// ScartchPad Access
    virtual cjBool					GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, cjUINT32& fillSiz, cjBool& isSharedFillDat, 
											cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const = 0;
    virtual cjBool					InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, cjBool initSharedDat, cjRefObjNameHash* nodeHash,
											cjAniPoseBufHandle pbHandle) const = 0;
	virtual void					DisposeSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat) const {}

	virtual void					Collapse() {}

	virtual cjTMEvaluator*			GetTMEvaluator() { return cjNULL; }

protected:
    cjAniEvaluator(const cjName& id, const cjName& propRttiName, const cjName& cntrRttiName, const cjName& cntrID, const cjName& evaluatorID);
    cjAniEvaluator(const cjAniEvaluatorTag& id);

private:
	void							_CommInit();

protected:
	cjAniEvaluatorTag				m_tag;

public: //cjtemp delete public
	cjBYTE							m_channelItem[cjAniEvalChan_Item_MAX];
	/*
		m_channelItem[cjAniEvalChan_Base, cjAniEvalChan_Pos] = cjAniEvalChanItem_Float, cjAniEvalChanItem_Color, cjAniEvalChanItem_Bool 
		m_channelItem[cjAniEvalChan_Rot]					 = cjAniEvalChanItem_Rot
		m_channelItem[cjAniEvalChan_Scl]					 = cjAniEvalChanItem_Float
		m_channelItem[cjAniEvalChan_Flag]					 = cjAniEvalFlagChanVal_Referenced, cjAniEvalFlagChanVal_Transform ..
	*/
};


//
// Constant
//


//
// cjConstTMEvaluator
//

class CJCORE_API cjConstTMEvaluator : public cjAniEvaluator
{
	CJSER_DECL_CLASS(cjConstTMEvaluator, cjAniEvaluator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjConstTMEvaluator)

public:
	// cjAniEvaluator
    virtual cjBool					GetChannelPosedValue(cjUINT32 channel, void* rst) const;

    virtual cjBool					UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const;

    virtual cjBool					GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, cjUINT32& fillSiz, cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const;
    virtual cjBool					InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, cjBool initSharedDat, cjRefObjNameHash* nodeHash, cjAniPoseBufHandle pbHandle) const;
	
public:
    cjConstTMEvaluator();
    cjConstTMEvaluator(const cjQuatTM& poseValue);

	void							SetPosePos(const cjVec3& pos)		{  m_value.SetPos(pos);		UpdateChannelItem(); }
	void							SetPoseRotate(const cjQuat& rot)	{  m_value.SetRotate(rot);	UpdateChannelItem(); }
	void							SetPoseScale(const cjVec3& scale)	{  m_value.SetScale(scale);	UpdateChannelItem(); }

    void							SetPoseValue(const cjQuatTM& poseValue) {
										if(poseValue.IsRotateValid())	SetPoseRotate(poseValue.GetRotate());
										if(poseValue.IsScaleValid())	SetPoseScale(poseValue.GetScale());
										if(poseValue.IsPosValid())		SetPosePos(poseValue.GetPos());

										UpdateChannelItem();
									}
	
protected:
    void							UpdateChannelItem();

protected:
	cjQuatTM						m_value;
};

cjRefPtrDefine(cjConstTMEvaluator)


//
// cjConstBoolEvaluator
//

class CJCORE_API cjConstBoolEvaluator : public cjAniEvaluator
{
	CJSER_DECL_CLASS(cjConstBoolEvaluator, cjAniEvaluator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjConstBoolEvaluator)

public:
	// cjAniEvaluator
    virtual cjBool					GetChannelPosedValue(cjUINT32 channel, void* rst) const;
	virtual cjBool					UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const;
	virtual cjBool					GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, 
										cjUINT32& fillSiz, cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const;
    virtual cjBool					InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, 
										cjBool initSharedDat, cjRefObjNameHash* nodeHash, cjAniPoseBufHandle pbHandle) const;

public:
    cjConstBoolEvaluator();
    cjConstBoolEvaluator(cjFLOAT poseBool/*cjFalse=CJ_FLT_INVALID*/);

    void							SetPoseValue(cjFLOAT poseBool/*cjFalse=CJ_FLT_INVALID*/) { m_value = poseBool; SetEvalChannelItem(); }

protected:
    void							SetEvalChannelItem();

protected:
    cjFLOAT							m_value;
};

cjRefPtrDefine(cjConstBoolEvaluator);


//
// cjConstFloatEvaluator
//

class CJCORE_API cjConstFloatEvaluator : public cjAniEvaluator
{
	CJSER_DECL_CLASS(cjConstFloatEvaluator, cjAniEvaluator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjConstFloatEvaluator)

public:
	// cjAniEvaluator
    virtual cjBool					GetChannelPosedValue(cjUINT32 channel, void* rst) const;
	virtual cjBool					UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const;
	virtual cjBool					GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, 
										cjUINT32& fillSiz, cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const;
    virtual cjBool					InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, 
										cjBool initSharedDat, cjRefObjNameHash* nodeHash, cjAniPoseBufHandle pbHandle) const;

public:
    cjConstFloatEvaluator();
    cjConstFloatEvaluator(cjFLOAT poseValue);

    void							SetPoseValue(cjFLOAT poseValue) { m_value = poseValue; SetEvalChannelTypes(); }
	
protected:
    void							SetEvalChannelTypes();
   
    cjFLOAT							m_value;
};

cjRefPtrDefine(cjConstFloatEvaluator);


//
// cjConstVec3SEvaluator (scale)
//

class CJCORE_API cjConstVec3SEvaluator : public cjAniEvaluator
{
	CJSER_DECL_CLASS(cjConstVec3SEvaluator, cjAniEvaluator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjConstVec3SEvaluator)

public:
	// cjAniEvaluator
    virtual cjBool					GetChannelPosedValue(cjUINT32 channel, void* rst) const;
	virtual cjBool					UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const;
	virtual cjBool					GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, 
										cjUINT32& fillSiz, cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const;
    virtual cjBool					InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, 
										cjBool initSharedDat, cjRefObjNameHash* nodeHash, cjAniPoseBufHandle pbHandle) const;

public:
    cjConstVec3SEvaluator();
    cjConstVec3SEvaluator(const cjVec3& poseValue);

	void							SetPoseValue(const cjVec3& poseValue) { m_value = poseValue; SetEvalChannelTypes(); }

protected:
    void							SetEvalChannelTypes();

    cjVec3							m_value;
};

cjRefPtrDefine(cjConstVec3SEvaluator);


//
// cjConstVec3PEvaluator  (pos)
//

class CJCORE_API cjConstVec3PEvaluator : public cjAniEvaluator
{
	CJSER_DECL_CLASS(cjConstVec3PEvaluator, cjAniEvaluator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjConstVec3PEvaluator)

public:
	// cjAniEvaluator
    virtual cjBool					GetChannelPosedValue(cjUINT32 channel, void* rst) const;
	virtual cjBool					UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const;
	virtual cjBool					GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, 
										cjUINT32& fillSiz, cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const;
    virtual cjBool					InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, 
										cjBool initSharedDat, cjRefObjNameHash* nodeHash, cjAniPoseBufHandle pbHandle) const;

public:
    cjConstVec3PEvaluator();
    cjConstVec3PEvaluator(const cjVec3& poseValue);

	void							SetPoseValue(const cjVec3& poseValue) { m_value = poseValue; SetEvalChannelTypes(); }

protected:
    void							SetEvalChannelTypes();

    cjVec3							m_value;
};

cjRefPtrDefine(cjConstVec3PEvaluator);


//
// cjConstQuatEvaluator
//

class CJCORE_API cjConstQuatEvaluator : public cjAniEvaluator
{
	CJSER_DECL_CLASS(cjConstQuatEvaluator, cjAniEvaluator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjConstQuatEvaluator)

public:
	// cjAniEvaluator
    virtual cjBool					GetChannelPosedValue(cjUINT32 channel, void* rst) const;
	virtual cjBool					UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const;
	virtual cjBool					GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, 
										cjUINT32& fillSiz, cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const;
    virtual cjBool					InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, 
										cjBool initSharedDat, cjRefObjNameHash* nodeHash, cjAniPoseBufHandle pbHandle) const;

public:
    cjConstQuatEvaluator();
    cjConstQuatEvaluator(const cjQuat& poseValue);

	void							SetPoseValue(const cjQuat& poseValue) { m_value = poseValue; SetEvalChannelTypes(); }

protected:
    void							SetEvalChannelTypes();

    cjQuat							m_value;
};

cjRefPtrDefine(cjConstQuatEvaluator);


//
// cjConstColorEvaluator
//

class CJCORE_API cjConstColorEvaluator : public cjAniEvaluator
{
	CJSER_DECL_CLASS(cjConstColorEvaluator, cjAniEvaluator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjConstColorEvaluator)

public:
	// cjAniEvaluator
    virtual cjBool					GetChannelPosedValue(cjUINT32 channel, void* rst) const;
	virtual cjBool					UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const;
	virtual cjBool					GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, 
										cjUINT32& fillSiz, cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const;
    virtual cjBool					InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, 
										cjBool initSharedDat, cjRefObjNameHash* nodeHash, cjAniPoseBufHandle pbHandle) const;

public:
    cjConstColorEvaluator();
    cjConstColorEvaluator(const cjColorF& poseValue);

    void							SetPoseValue(const cjColorF& poseValue) { m_value = poseValue; SetEvalChannelTypes(); }

protected:
    void							SetEvalChannelTypes();

    cjColorF						m_value;
};

cjRefPtrDefine(cjConstColorEvaluator);


//
// cjKeyBasedEvaluator
//

class CJCORE_API cjKeyBasedEvaluator : public cjAniEvaluator
{
	CJSER_DECL_CLASS_ABSTRACT(cjKeyBasedEvaluator, cjAniEvaluator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE_ABSTRACT(cjKeyBasedEvaluator)
	
public:
	// cjAniEvaluator
	virtual void					GetValidTimeRange(cjFLOAT& stTime, cjFLOAT& enTime) const;

public:
    virtual cjUINT32				GetKeyChannelCount() const = 0;
    virtual cjUINT32				GetKeyCount(cjUINT32 channel) const = 0;
    virtual cjAniKey::EKEY_TYPE		GetKeyType(cjUINT32 channel) const = 0;
    virtual cjAniKey::EKEY_VALTYPE	GetKeyValType(cjUINT32 channel) const = 0;
    virtual cjAniKey*				GetKeyAry(cjUINT32 channel) const = 0;
    virtual cjBYTE					GetKeySiz(cjUINT32 channel) const = 0;

    cjUINT32						GetAllocSiz(cjUINT32 channel) const;
    cjAniKey*						GetKey(cjUINT32 index, cjUINT32 channel) const;
	cjAniKey*						GetKeysWithChannel(cjUINT32& rstKeyCount, cjAniKey::EKEY_TYPE& rstKeyType, cjBYTE& rstKeySiz, cjUINT32 channel=0) const;
    
};

cjRefPtrDefine(cjKeyBasedEvaluator)


//
// cjKeyBasedEvaluator
// - cjColorEvaluator
//

class CJCORE_API cjColorEvaluator : public cjKeyBasedEvaluator
{
	CJSER_DECL_CLASS(cjColorEvaluator, cjKeyBasedEvaluator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_SERIALIZE_POST
	CJSER_DECL_CLONE(cjColorEvaluator)

public:
	// cjAniEvaluator
	virtual cjBool					GetChannelPosedValue(cjUINT32 channel, void* rst) const;
	virtual cjBool					UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const;
		
	//virtual void					GetValidTimeRange(cjFLOAT& stTime, cjFLOAT& enTime) const;
    virtual void					CacheKeys(cjFLOAT stTime, cjFLOAT enTime);
    virtual cjAniEvaluator*			GetEvaluator(cjFLOAT stTime, cjFLOAT enTime);

    virtual cjBool					GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, cjUINT32& fillSiz, cjBool& isSharedFillDat, 
										cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const;
    virtual cjBool					InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, cjBool initSharedDat, cjRefObjNameHash* nodeHash,
										cjAniPoseBufHandle pbHandle) const;	

	virtual void					Collapse();


    // cjKeyBasedEvaluator
	virtual cjUINT32				GetKeyChannelCount() const { return 1; }
    virtual cjUINT32				GetKeyCount(cjUINT32 channel) const;
    virtual cjAniKey::EKEY_TYPE		GetKeyType(cjUINT32 channel) const;
	virtual cjAniKey::EKEY_VALTYPE	GetKeyValType(cjUINT32 channel) const { return cjAniKey::EKVAL_COLOR; }
    virtual cjAniKey*				GetKeyAry(cjUINT32 channel) const;
    virtual cjBYTE					GetKeySiz(cjUINT32 channel) const;


public:
    cjColorEvaluator();
    cjColorEvaluator(cjAniColorDat* colorDat);

    cjAniColorDat*					GetColorDat() const { return m_spColorDat; }
    void							SetColorDat(cjAniColorDat* colorDat) { m_spColorDat=colorDat; SetEvalChannelTypes(); }

    cjAniKeyColor*					GetKeys(cjUINT32& keyCount, cjAniKeyColor::EKEY_TYPE& keyType, cjBYTE& keySiz) const;
    void							ReplaceKeys(cjAniKeyColor* keys, cjUINT32 keyCount, cjAniKeyColor::EKEY_TYPE keyType);
    void							SetKeys(cjAniKeyColor* keys, cjUINT32 keyCount, cjAniKeyColor::EKEY_TYPE keyType);

    static cjBool					LinearColorFillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat);

protected:
    void							SetEvalChannelTypes();

	cjAniColorDatRPtr				m_spColorDat;
};

cjRefPtrDefine(cjColorEvaluator)


//
// cjKeyBasedEvaluator
//  - cjTMEvaluator
//

//cjtodo

// reporting serialized key count of m_spTMDat to know how many key is loaded
// 3dsmax, Forward Key, 30fps is converted 31 keyframe

class CJCORE_API cjTMEvaluator : public cjKeyBasedEvaluator
{
	CJSER_DECL_CLASS(cjTMEvaluator, cjKeyBasedEvaluator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_SERIALIZE_POST
	CJSER_DECL_CLONE(cjTMEvaluator)

public:
	// cjAniEvaluator
	virtual cjBool					GetChannelPosedValue(cjUINT32 channel, void* rst) const;

	virtual cjBool					UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const;

	virtual void					GetValidTimeRange(cjFLOAT& stTime, cjFLOAT& enTime) const;
    virtual void					CacheKeys(cjFLOAT stTime, cjFLOAT enTime);
    virtual cjAniEvaluator*			GetEvaluator(cjFLOAT stTime, cjFLOAT enTime);

    virtual cjBool					GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, cjUINT32& fillSiz, cjBool& isSharedFillDat, 
										cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const;
    virtual cjBool					InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, cjBool initSharedDat, cjRefObjNameHash* nodeHash,
										cjAniPoseBufHandle pbHandle) const;	

	virtual void					Collapse();


    // cjKeyBasedEvaluator
	virtual cjUINT32				GetKeyChannelCount() const { return 3; }
    virtual cjUINT32				GetKeyCount(cjUINT32 channel) const;
    virtual cjAniKey::EKEY_TYPE		GetKeyType(cjUINT32 channel) const;
    virtual cjAniKey::EKEY_VALTYPE	GetKeyValType(cjUINT32 channel) const;
    virtual cjAniKey*				GetKeyAry(cjUINT32 channel) const;
    virtual cjBYTE					GetKeySiz(cjUINT32 channel) const;

	virtual cjTMEvaluator*			GetTMEvaluator() { return this; }
	
public:
    cjTMEvaluator();
    cjTMEvaluator(cjAniTMDat* tmDat);
    cjTMEvaluator(const cjQuatTM& poseValue);

    cjAniTMDat*						GetTMDat() const { return m_spTMDat; }
    void							SetTMDat(cjAniTMDat* tmDat);
    void							SetPoseValue(const cjQuatTM& poseValue);

	// scale
	cjUINT32						GetScaleKeyCount() const { return (m_spTMDat ? m_spTMDat->GetScaleKeyCount() : 0); }
    cjAniKeyVec3*					GetScaleKeys(cjUINT32& keyCount, cjAniKeyVec3::EKEY_TYPE& keyType, cjBYTE& keySiz) const;
    void							ReplaceScaleKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType);
    void							SetPoseScale(const cjVec3& poseScale);

	// rot
	cjUINT32						GetRotKeyCount() const { return (m_spTMDat ? m_spTMDat->GetRotKeyCount() : 0); } 
    cjAniKeyRot*					GetRotKeys(cjUINT32& keyCount, cjAniKeyRot::EKEY_TYPE& keyType, cjBYTE& keySiz) const;
    void							ReplaceRotKeys(cjAniKeyRot* keys, cjUINT32 keyCount, cjAniKeyRot::EKEY_TYPE keyType);
    void							SetPoseRotate(const cjQuat& rot);

	// pos
    cjUINT32						GetPosKeyCount() const { return (m_spTMDat ? m_spTMDat->GetPosKeyCount() : 0); }
    cjAniKeyVec3*					GetPosKeys(cjUINT32& keyCount, cjAniKeyVec3::EKEY_TYPE& keyType, cjBYTE& keySiz) const;
    void							ReplacePosKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType);
    void							SetPosePos(const cjVec3& pos);

protected:
    void SetEvalChannelTypes();

protected:
    cjQuatTM						m_value;   
    cjAniTMDatRPtr					m_spTMDat; // if m_spTMDat is null, there is only pose value(const key frame)
};

cjRefPtrDefine(cjTMEvaluator)


//
// cjKeyBasedEvaluator
//  - cjQuatEvaluator
//

class CJCORE_API cjQuatEvaluator : public cjKeyBasedEvaluator
{
	CJSER_DECL_CLASS(cjQuatEvaluator, cjKeyBasedEvaluator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjQuatEvaluator)

public:
	// cjAniEvaluator
	virtual cjBool					GetChannelPosedValue(cjUINT32 channel, void* rst) const;
	virtual cjBool					UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const;
		
	virtual void					GetValidTimeRange(cjFLOAT& stTime, cjFLOAT& enTime) const;
    virtual void					CacheKeys(cjFLOAT stTime, cjFLOAT enTime);
    virtual cjAniEvaluator*			GetEvaluator(cjFLOAT stTime, cjFLOAT enTime);

    virtual cjBool					GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, cjUINT32& fillSiz, cjBool& isSharedFillDat, 
										cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const;
    virtual cjBool					InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, cjBool initSharedDat, cjRefObjNameHash* nodeHash,
										cjAniPoseBufHandle pbHandle) const;	

	virtual void					Collapse();

    // cjKeyBasedEvaluator
	virtual cjUINT32				GetKeyChannelCount() const { return 1; }
    virtual cjUINT32				GetKeyCount(cjUINT32 channel) const;
    virtual cjAniKey::EKEY_TYPE		GetKeyType(cjUINT32 channel) const;
	virtual cjAniKey::EKEY_VALTYPE	GetKeyValType(cjUINT32 channel) const { return cjAniKey::EKVAL_ROT; }
    virtual cjAniKey*				GetKeyAry(cjUINT32 channel) const;
    virtual cjBYTE					GetKeySiz(cjUINT32 channel) const;

public:
    cjQuatEvaluator();
    cjQuatEvaluator(cjAniRotDat* dat);

	cjAniRotDat*					GetRotDat() const { return m_spQuatDat; }
    void							SetRotDat(cjAniRotDat* dat);

    cjAniKeyRot*					GetKeys(cjUINT32& keyCount, cjAniKeyRot::EKEY_TYPE& keyType, cjBYTE& keySiz) const;
    void							ReplaceKeys(cjAniKeyRot* keys, cjUINT32 keyCount, cjAniKeyRot::EKEY_TYPE keyType);
    void							SetKeys(cjAniKeyRot* keys, cjUINT32 keyCount, cjAniKeyRot::EKEY_TYPE keyType);
	
    static cjBool					SlerpRotFillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat);
    static cjBool					SquadRotFillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat);
    static cjBool					EulerRotFillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat);
	
protected:
    void							SetEvalChannelTypes();

    cjAniRotDatRPtr					m_spQuatDat;
};


//
// cjKeyBasedEvaluator
//  - cjFloatEvaluator
//

class CJCORE_API cjFloatEvaluator : public cjKeyBasedEvaluator
{
	CJSER_DECL_CLASS(cjFloatEvaluator, cjKeyBasedEvaluator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjFloatEvaluator)

public:
	// cjAniEvaluator
	virtual cjBool					GetChannelPosedValue(cjUINT32 channel, void* rst) const;
	virtual cjBool					UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const;
		
    virtual void					CacheKeys(cjFLOAT stTime, cjFLOAT enTime);
    virtual cjAniEvaluator*			GetEvaluator(cjFLOAT stTime, cjFLOAT enTime);

    virtual cjBool					GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, cjUINT32& fillSiz, cjBool& isSharedFillDat, 
										cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const;
    virtual cjBool					InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, cjBool initSharedDat, cjRefObjNameHash* nodeHash,
										cjAniPoseBufHandle pbHandle) const;	

	virtual void					Collapse();

    // cjKeyBasedEvaluator
	virtual cjUINT32				GetKeyChannelCount() const { return 1; }
    virtual cjUINT32				GetKeyCount(cjUINT32 channel) const;
    virtual cjAniKey::EKEY_TYPE		GetKeyType(cjUINT32 channel) const;
	virtual cjAniKey::EKEY_VALTYPE	GetKeyValType(cjUINT32 channel) const { return cjAniKey::EKVAL_FLOAT; }
    virtual cjAniKey*				GetKeyAry(cjUINT32 channel) const;
    virtual cjBYTE					GetKeySiz(cjUINT32 channel) const;

public:
    cjFloatEvaluator();
    cjFloatEvaluator(cjAniFloatDat* dat);

	cjAniFloatDat*					GetFloatDat() const { return m_spFloatDat; }
    void							SetFloatDat(cjAniFloatDat* dat);

    cjAniKeyFloat*					GetKeys(cjUINT32& keyCount, cjAniKeyFloat::EKEY_TYPE& keyType, cjBYTE& keySiz) const;
    void							ReplaceKeys(cjAniKeyFloat* keys, cjUINT32 keyCount, cjAniKeyFloat::EKEY_TYPE keyType);
    void							SetKeys(cjAniKeyFloat* keys, cjUINT32 keyCount, cjAniKeyFloat::EKEY_TYPE keyType);
	
    static cjBool					CubicFloatFillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat);

protected:
    void							SetEvalChannelTypes();
   
    cjAniFloatDatRPtr				m_spFloatDat;
};


//
// cjKeyBasedEvaluator
//  - cjVec3SEvaluator   -- for scale
//

class CJCORE_API cjVec3SEvaluator : public cjKeyBasedEvaluator
{
	CJSER_DECL_CLASS(cjVec3SEvaluator, cjKeyBasedEvaluator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjVec3SEvaluator)

public:
	// cjAniEvaluator
	virtual cjBool					GetChannelPosedValue(cjUINT32 channel, void* rst) const;

	virtual cjBool					UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const;
		
    virtual void					CacheKeys(cjFLOAT stTime, cjFLOAT enTime);
    virtual cjAniEvaluator*			GetEvaluator(cjFLOAT stTime, cjFLOAT enTime);

    virtual cjBool					GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, cjUINT32& fillSiz, cjBool& isSharedFillDat, 
										cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const;
    virtual cjBool					InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, cjBool initSharedDat, cjRefObjNameHash* nodeHash,
										cjAniPoseBufHandle pbHandle) const;	

	virtual void					Collapse();

    // cjKeyBasedEvaluator
	virtual cjUINT32				GetKeyChannelCount() const { return 1; }
    virtual cjUINT32				GetKeyCount(cjUINT32 channel) const;
    virtual cjAniKey::EKEY_TYPE		GetKeyType(cjUINT32 channel) const;
	virtual cjAniKey::EKEY_VALTYPE	GetKeyValType(cjUINT32 channel) const { return cjAniKey::EKVAL_VEC3; }
    virtual cjAniKey*				GetKeyAry(cjUINT32 channel) const;
    virtual cjBYTE					GetKeySiz(cjUINT32 channel) const;
	
public:
    cjVec3SEvaluator();
    cjVec3SEvaluator(cjAniVec3Dat* dat);

	cjAniVec3Dat*					GetVec3ScaleDat() const { return m_spVec3ScaleDat; }
    void							SetVec3ScaleDat(cjAniVec3Dat* dat);

    cjAniKeyVec3*					GetKeys(cjUINT32& keyCount, cjAniKeyVec3::EKEY_TYPE& keyType, cjBYTE& keySiz) const;
    void							ReplaceKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType);
    void							SetKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType);
	
    static cjBool					CubicVec3FillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat);

protected:
    void							SetEvalChannelTypes();

    cjAniVec3DatRPtr				m_spVec3ScaleDat;
};


//
// cjKeyBasedEvaluator
//  - cjVec3PEvaluator
//

class CJCORE_API cjVec3PEvaluator : public cjKeyBasedEvaluator
{
	CJSER_DECL_CLASS(cjVec3PEvaluator, cjKeyBasedEvaluator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjVec3PEvaluator)

public:
	// cjAniEvaluator
	virtual cjBool					GetChannelPosedValue(cjUINT32 channel, void* rst) const;

	virtual cjBool					UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const;
		
    virtual void					CacheKeys(cjFLOAT stTime, cjFLOAT enTime);
    virtual cjAniEvaluator*			GetEvaluator(cjFLOAT stTime, cjFLOAT enTime);

    virtual cjBool					GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, cjUINT32& fillSiz, cjBool& isSharedFillDat, 
										cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const;
    virtual cjBool					InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, cjBool initSharedDat, cjRefObjNameHash* nodeHash,
										cjAniPoseBufHandle pbHandle) const;	

	virtual void					Collapse();

    // cjKeyBasedEvaluator
	virtual cjUINT32				GetKeyChannelCount() const { return 1; }
    virtual cjUINT32				GetKeyCount(cjUINT32 channel) const;
    virtual cjAniKey::EKEY_TYPE		GetKeyType(cjUINT32 channel) const;
	virtual cjAniKey::EKEY_VALTYPE	GetKeyValType(cjUINT32 channel) const { return cjAniKey::EKVAL_VEC3; }
    virtual cjAniKey*				GetKeyAry(cjUINT32 channel) const;
    virtual cjBYTE					GetKeySiz(cjUINT32 channel) const;
	
public:
    cjVec3PEvaluator();
    cjVec3PEvaluator(cjAniVec3Dat* dat);

	cjAniVec3Dat*					GetVec3Dat() const { return m_spVec3PosDat; }
    void							SetVec3Dat(cjAniVec3Dat* dat);

    cjAniKeyVec3*					GetKeys(cjUINT32& keyCount, cjAniKeyVec3::EKEY_TYPE& keyType, cjBYTE& keySiz) const;
    void							ReplaceKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType);
    void							SetKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType);
	
    static cjBool					CubicVec3FillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat);

protected:
    void							SetEvalChannelTypes();

    cjAniVec3DatRPtr				m_spVec3PosDat;
};


//
// cjBoolEvaluator
//

cjRefPtrDefine(cjBoolEvaluator);

class CJCORE_API cjBoolEvaluator : public cjKeyBasedEvaluator
{
	CJSER_DECL_CLASS(cjBoolEvaluator, cjKeyBasedEvaluator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjBoolEvaluator)

public:
	// cjAniEvaluator
	virtual cjBool					GetChannelPosedValue(cjUINT32 channel, void* rst) const;
	virtual cjBool					UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const;
		
    virtual void					CacheKeys(cjFLOAT stTime, cjFLOAT enTime);
    virtual cjAniEvaluator*			GetEvaluator(cjFLOAT stTime, cjFLOAT enTime);

    virtual cjBool					GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, cjUINT32& fillSiz, cjBool& isSharedFillDat, 
										cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const;
    virtual cjBool					InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, cjBool initSharedDat, cjRefObjNameHash* nodeHash,
										cjAniPoseBufHandle pbHandle) const;	

	virtual void					Collapse();

    // cjKeyBasedEvaluator
	virtual cjUINT32				GetKeyChannelCount() const { return 1; }
    virtual cjUINT32				GetKeyCount(cjUINT32 channel) const;
    virtual cjAniKey::EKEY_TYPE		GetKeyType(cjUINT32 channel) const;
	virtual cjAniKey::EKEY_VALTYPE	GetKeyValType(cjUINT32 channel) const { return cjAniKey::EKVAL_BOOL; }
    virtual cjAniKey*				GetKeyAry(cjUINT32 channel) const;
    virtual cjBYTE					GetKeySiz(cjUINT32 channel) const;


public:
    cjBoolEvaluator();
    cjBoolEvaluator(cjAniBoolDat* dat);

	cjAniBoolDat*					GetBoolDat() const { return m_spBoolDat; }
	void							SetBoolDat(cjAniBoolDat* dat) { m_spBoolDat = dat; SetEvalChannelTypes(); }

    cjAniKeyBool*					GetKeys(cjUINT32& keyCount, cjAniKeyBool::EKEY_TYPE& keyType, cjBYTE& keySiz) const;
    void							ReplaceKeys(cjAniKeyBool* keys, cjUINT32 keyCount, cjAniKeyBool::EKEY_TYPE keyType);
    void							SetKeys(cjAniKeyBool* keys, cjUINT32 keyCount, cjAniKeyBool::EKEY_TYPE keyType);

    static cjBool					ConstantBoolFillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat);

protected:
    void							SetEvalChannelTypes();

    cjAniBoolDatRPtr					m_spBoolDat;
};


//
// cjBoolTimelineEvaluator
//

cjRefPtrDefine(cjBoolTimelineEvaluator);

class CJCORE_API cjBoolTimelineEvaluator : public cjBoolEvaluator
{
	CJSER_DECL_CLASS(cjBoolTimelineEvaluator, cjBoolEvaluator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjBoolTimelineEvaluator)

public:
	// cjAniEvaluator
    virtual cjBool					GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, cjUINT32& fillSiz, cjBool& isSharedFillDat, 
										cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const;
    virtual cjBool					InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, cjBool initSharedDat, cjRefObjNameHash* nodeHash,
										cjAniPoseBufHandle pbHandle) const;	

public:
    cjBoolTimelineEvaluator();
    cjBoolTimelineEvaluator(cjAniBoolDat* dat);

    static cjBool					ConstantBoolTimelineFillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat);
};


//
// cjBSplineEvaluator
//

cjRefPtrDefine(cjBSplineEvaluator)

class CJCORE_API cjBSplineEvaluator : public cjAniEvaluator
{
	CJSER_DECL_CLASS_ABSTRACT(cjBSplineEvaluator, cjAniEvaluator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_SERIALIZE_POST
	CJSER_DECL_CLONE_ABSTRACT(cjBSplineEvaluator)

public:
	// cjAniEvaluator
    virtual void					GetValidTimeRange(cjFLOAT& stTime, cjFLOAT& enTime) const;
    virtual void					CacheKeys(cjFLOAT stTime, cjFLOAT enTime);
    virtual cjAniEvaluator*			GetEvaluator(cjFLOAT stTime, cjFLOAT enTime);

	virtual void					Collapse() {}

public:    
	cjBSplineDat*					GetDat() const { return m_spDat; }
	void							SetDat(cjBSplineDat* dat, cjBSplineBasisF3Dat* bsBasisDat) { m_spDat=dat; m_spBasisDat=bsBasisDat; } 
	cjBSplineBasisF3Dat*			GetBasisData() const { return m_spBasisDat; }
    void							SetTimeRange(cjFLOAT stTime, cjFLOAT enTime) { m_stTime = stTime; m_enTime = enTime; }
	
	virtual cjUINT32				GetChannelCount() const = 0;
	
	virtual cjUINT32				GetFloatValCount(cjUINT32 channel) const = 0;
    virtual cjUINT32				GetDegree(cjUINT32 channel) const = 0; 
    virtual cjUINT32				GetAllocSiz(cjUINT32 channel) const;	

	// cjTrue : word size of cjFLOAT /  cjFalse : cjFLOAT
    virtual cjBool					IsCompressedCP() const { return cjFalse; }
    virtual cjUINT32				GetCPCount(cjUINT32 channel) const;	
    virtual cjBSplineDat::BSOffset	GetCPOffset(cjUINT32 channel) const = 0;
    virtual void					SetCPOffset(cjBSplineDat::BSOffset cpOffset, cjUINT32 channel) = 0;

protected:
    cjBSplineEvaluator(cjBSplineDat* dat, cjBSplineBasisF3Dat* bsBasisDat);

    cjFLOAT							m_stTime;
    cjFLOAT							m_enTime;

    cjBSplineDatRPtr				m_spDat;
    cjBSplineBasisF3DatRPtr			m_spBasisDat; // immediate data, so should not be shared.
};


//
// cjBSColorEvaluator
//

class CJCORE_API cjBSColorEvaluator : public cjBSplineEvaluator
{
	CJSER_DECL_CLASS(cjBSColorEvaluator, cjBSplineEvaluator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjBSColorEvaluator)

public:
	// cjAniEvaluator
	virtual cjBool					GetChannelPosedValue(cjUINT32 channel, void* rst) const;

    virtual cjBool					UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const;
	virtual cjBool					GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, cjUINT32& fillSiz, cjBool& isSharedFillDat, 
											cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const;
    virtual cjBool					InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, cjBool initSharedDat, cjRefObjNameHash* nodeHash,
											cjAniPoseBufHandle pbHandle) const;

	// cjBSplineEvaluator
	virtual cjUINT32				GetChannelCount() const { return 1; }
	virtual cjUINT32				GetFloatValCount(cjUINT32 channel) const {
										if(cjAniEvalChan_Base == channel) { return 4; }
										else { return 0; }
									}
    virtual cjUINT32				GetDegree(cjUINT32 channel) const {
										if(cjAniEvalChan_Base == channel) { return 3; }
										else { return 0; }
									}
    virtual cjBSplineDat::BSOffset	GetCPOffset(cjUINT32 channel) const {
										if(cjAniEvalChan_Base == channel) { return m_valueCPOffset; }
										else { return cjBSplineDat::INVALID_BSOFFSET; }
									}
    virtual void					SetCPOffset(cjBSplineDat::BSOffset cpOffset, cjUINT32 channel)
									{
										if(cjAniEvalChan_Base == channel) { m_valueCPOffset = cpOffset; }
										SetEvalChannelTypes();
									}

public:
    cjBSColorEvaluator();
    cjBSColorEvaluator(cjBSplineDat* dat, cjBSplineDat::BSOffset colorCPOffset, cjBSplineBasisF3Dat* bsBasisDat);
    
    static cjBool					BSColorFillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat);

protected:
    cjBSColorEvaluator(cjBSplineDat* dat, cjBSplineDat::BSOffset colorCPOffset, cjBSplineBasisF3Dat* bsBasisDat, cjBool isCompressCPs);
    void							SetEvalChannelTypes();

    cjBSplineDat::BSOffset			m_valueCPOffset;
};

cjRefPtrDefine(cjBSColorEvaluator);


//
// cjBSFloatEvaluator
//

class CJCORE_API cjBSFloatEvaluator : public cjBSplineEvaluator
{
	CJSER_DECL_CLASS(cjBSFloatEvaluator, cjBSplineEvaluator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjBSFloatEvaluator)

public:
	// cjAniEvaluator
	virtual cjBool					GetChannelPosedValue(cjUINT32 channel, void* rst) const;

    virtual cjBool					UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const;
	virtual cjBool					GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, cjUINT32& fillSiz, cjBool& isSharedFillDat, 
											cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const;
    virtual cjBool					InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, cjBool initSharedDat, cjRefObjNameHash* nodeHash,
											cjAniPoseBufHandle pbHandle) const;

	// cjBSplineEvaluator
	virtual cjUINT32				GetChannelCount() const { return 1; }
	virtual cjUINT32				GetFloatValCount(cjUINT32 channel) const { if(cjAniEvalChan_Base == channel) { return 1; } else { return 0; } }
    virtual cjUINT32				GetDegree(cjUINT32 channel) const { if(cjAniEvalChan_Base == channel) { return 3; } else { return 0; } }
    virtual cjBSplineDat::BSOffset	GetCPOffset(cjUINT32 channel) const { if(cjAniEvalChan_Base == channel) { return m_valueCPOffset; } else { return cjBSplineDat::INVALID_BSOFFSET; } }
    virtual void					SetCPOffset(cjBSplineDat::BSOffset cpOffset, cjUINT32 channel) { if(cjAniEvalChan_Base == channel) { m_valueCPOffset = cpOffset; } SetEvalChannelTypes(); }


public:
    cjBSFloatEvaluator();
    cjBSFloatEvaluator(cjBSplineDat* dat, cjBSplineDat::BSOffset floatCPOffset, cjBSplineBasisF3Dat* bsBasisDat);

    static cjBool					BSFloatFillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat);

protected:
    cjBSFloatEvaluator(cjBSplineDat* dat, cjBSplineDat::BSOffset floatCPOffset, cjBSplineBasisF3Dat* bsBasisDat, cjBool isCompressCPs);
    void							SetEvalChannelTypes();
	
    cjBSplineDat::BSOffset			m_valueCPOffset;
};

cjRefPtrDefine(cjBSFloatEvaluator);


//
// cjBSVec3SEvaluator
//

class CJCORE_API cjBSVec3SEvaluator : public cjBSplineEvaluator
{
	CJSER_DECL_CLASS(cjBSVec3SEvaluator, cjBSplineEvaluator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjBSVec3SEvaluator)

public:
	// cjAniEvaluator
	virtual cjBool					GetChannelPosedValue(cjUINT32 channel, void* rst) const;

    virtual cjBool					UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const;
	virtual cjBool					GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, cjUINT32& fillSiz, cjBool& isSharedFillDat, 
											cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const;
    virtual cjBool					InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, cjBool initSharedDat, cjRefObjNameHash* nodeHash,
											cjAniPoseBufHandle pbHandle) const;

	// cjBSplineEvaluator
	virtual cjUINT32				GetChannelCount() const { return 1; }
	virtual cjUINT32				GetFloatValCount(cjUINT32 channel) const { if(cjAniEvalChan_Base == channel) { return 3; } else { return 0; } }
    virtual cjUINT32				GetDegree(cjUINT32 channel) const { if(cjAniEvalChan_Base == channel) { return 3; } else { return 0; } }
    virtual cjBSplineDat::BSOffset	GetCPOffset(cjUINT32 channel) const { if(cjAniEvalChan_Base == channel) { return m_valueCPOffset; } else { return cjBSplineDat::INVALID_BSOFFSET; } }
    virtual void					SetCPOffset(cjBSplineDat::BSOffset cpOffset, cjUINT32 channel) { if(cjAniEvalChan_Base == channel) { m_valueCPOffset = cpOffset; } SetEvalChannelTypes(); }
	
public:
    cjBSVec3SEvaluator();
    cjBSVec3SEvaluator(cjBSplineDat* dat, cjBSplineDat::BSOffset vec3CPOffset, cjBSplineBasisF3Dat* bsBasisDat);

    static cjBool					BSVec3SFillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat);

protected:
    cjBSVec3SEvaluator(cjBSplineDat* dat, cjBSplineDat::BSOffset vec3CPOffset, cjBSplineBasisF3Dat* bsBasisDat, cjBool isCompressCPs);
    void							SetEvalChannelTypes();


    cjBSplineDat::BSOffset			m_valueCPOffset;
};

cjRefPtrDefine(cjBSVec3SEvaluator);


//
// cjBSVec3PEvaluator
//

class CJCORE_API cjBSVec3PEvaluator : public cjBSplineEvaluator
{
	CJSER_DECL_CLASS(cjBSVec3PEvaluator, cjBSplineEvaluator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjBSVec3PEvaluator)

public:
	// cjAniEvaluator
	virtual cjBool					GetChannelPosedValue(cjUINT32 channel, void* rst) const;

    virtual cjBool					UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const;
	virtual cjBool					GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, cjUINT32& fillSiz, cjBool& isSharedFillDat, 
											cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const;
    virtual cjBool					InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, cjBool initSharedDat, cjRefObjNameHash* nodeHash,
											cjAniPoseBufHandle pbHandle) const;

	// cjBSplineEvaluator
	virtual cjUINT32				GetChannelCount() const { return 1; }
	virtual cjUINT32				GetFloatValCount(cjUINT32 channel) const { if(cjAniEvalChan_Base == channel) { return 3; } else { return 0; } }
    virtual cjUINT32				GetDegree(cjUINT32 channel) const { if(cjAniEvalChan_Base == channel) { return 3; } else { return 0; } }
    virtual cjBSplineDat::BSOffset	GetCPOffset(cjUINT32 channel) const { if(cjAniEvalChan_Base == channel) { return m_valueCPOffset; } else { return cjBSplineDat::INVALID_BSOFFSET; } }
    virtual void					SetCPOffset(cjBSplineDat::BSOffset cpOffset, cjUINT32 channel) { if(cjAniEvalChan_Base == channel) { m_valueCPOffset = cpOffset; } SetEvalChannelTypes(); }
	
public:
    cjBSVec3PEvaluator();
    cjBSVec3PEvaluator(cjBSplineDat* dat, cjBSplineDat::BSOffset vec3CPOffset, cjBSplineBasisF3Dat* bsBasisDat);

    static cjBool					BSVec3PFillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat);

protected:
    cjBSVec3PEvaluator(cjBSplineDat* dat, cjBSplineDat::BSOffset vec3CPOffset, cjBSplineBasisF3Dat* bsBasisDat, cjBool isCompressCPs);
    void							SetEvalChannelTypes();

    cjBSplineDat::BSOffset			m_valueCPOffset;
};

cjRefPtrDefine(cjBSVec3PEvaluator);


//
// cjBSTMEvaluator
//

class CJCORE_API cjBSTMEvaluator : public cjBSplineEvaluator
{
	CJSER_DECL_CLASS(cjBSTMEvaluator, cjBSplineEvaluator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjBSTMEvaluator)

public:
	// cjAniEvaluator
	virtual cjBool					GetChannelPosedValue(cjUINT32 channel, void* rst) const;

    virtual cjBool					UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const;
	virtual cjBool					GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, cjUINT32& fillSiz, cjBool& isSharedFillDat, 
											cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const;
    virtual cjBool					InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, cjBool initSharedDat, cjRefObjNameHash* nodeHash,
											cjAniPoseBufHandle pbHandle) const;

	// cjBSplineEvaluator
	virtual cjUINT32				GetChannelCount() const { return 3; }
    virtual cjUINT32				GetFloatValCount(cjUINT32 channel) const
									{   switch(channel) {
										case cjAniEvalChan_Pos : return 3;
										case cjAniEvalChan_Rot : return 4;
										case cjAniEvalChan_Scl : return 3;
										default : return 0; }
									}
    virtual cjUINT32				GetDegree(cjUINT32 channel) const
									{   switch(channel) {
										case cjAniEvalChan_Pos : return 3;
										case cjAniEvalChan_Rot : return 3;
										case cjAniEvalChan_Scl : return 3;
										default : return 0; }
									}

    virtual cjBSplineDat::BSOffset	GetCPOffset(cjUINT32 channel) const;
    virtual void					SetCPOffset(cjBSplineDat::BSOffset cpOffset, cjUINT32 channel);

public:
    cjBSTMEvaluator();
	cjBSTMEvaluator(cjBSplineDat* dat, 
		cjBSplineDat::BSOffset posCPOffset=cjBSplineDat::INVALID_BSOFFSET,
        cjBSplineDat::BSOffset rotCPOffset=cjBSplineDat::INVALID_BSOFFSET, 
		cjBSplineDat::BSOffset scaleCPOffset=cjBSplineDat::INVALID_BSOFFSET,
        cjBSplineBasisF3Dat* bsBasisDat=cjNULL);
    cjBSTMEvaluator(const cjQuatTM& poseValue);

    // Pose methods
    void							SetPosePos(const cjVec3& pos);
    void							SetPoseRotate(const cjQuat& rotate);
    void							SetPoseScale(const cjVec3& scale);
    void							SetPoseValue(const cjQuatTM& poseValue);

    static cjBool					BSRotFillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat);

protected:
    cjBSTMEvaluator(cjBSplineDat* dat, cjBSplineDat::BSOffset posCPOffset, cjBSplineDat::BSOffset rotCPOffset, 
		cjBSplineDat::BSOffset scaleCPOffset, cjBSplineBasisF3Dat* bsBasisDat, cjBool isCompressCPs);
    cjBSTMEvaluator(const cjQuatTM& poseValue, cjBool isCompressCPs);

    void							SetEvalChannelTypes();

protected:
    cjQuatTM						m_value;

    cjBSplineDat::BSOffset			m_posCPOffset;
    cjBSplineDat::BSOffset			m_rotCPOffset;
    cjBSplineDat::BSOffset			m_scaleCPOffset;
};

cjRefPtrDefine(cjBSTMEvaluator);


//
// ETC
//

class CJCORE_API cjLookAtEvaluator : public cjAniEvaluator
{
	CJSER_DECL_CLASS(cjLookAtEvaluator, cjAniEvaluator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_SERIALIZE_POST
	CJSER_DECL_CLONE(cjLookAtEvaluator)

public:
	enum
	{
		EVAL_POS			= 0,
		EVAL_ROLL			= 1, // rolling
		EVAL_SCALE			= 2,

		EVAL_MAX			= 3
	};

CJFLAG_DECL_BEGIN
		MASK_FLIP		= 0x0001,	// 1bit
		BPOS_FLIP		= 0,		
		MASK_AXIS		= 0x0006,	// 2bit
		BPOS_AXIS		= 1,
		MASK_SHUTDOWN	= 0x0008,	// 1bit
		BPOS_SHUTDOWN	= 3
CJFLAG_DECL_END
	
public:
	// cjAniEvaluator
	virtual cjBool					GetChannelPosedValue(cjUINT32 channel, void* rst) const;
	virtual cjBool					UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const;
		
	virtual void					GetValidTimeRange(cjFLOAT& stTime, cjFLOAT& enTime) const;
    virtual void					CacheKeys(cjFLOAT stTime, cjFLOAT enTime);
    virtual cjAniEvaluator*			GetEvaluator(cjFLOAT stTime, cjFLOAT enTime);

    virtual cjBool					GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, cjUINT32& fillSiz, 
										cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const;
    virtual cjBool					InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, 
										cjBool initSharedDat, cjRefObjNameHash* nodeHash, cjAniPoseBufHandle pbHandle) const;	
    virtual void					DisposeSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat) const;

	virtual void					Collapse();

public:
    cjLookAtEvaluator();
    cjLookAtEvaluator(const cjName& lookAt, const cjName& driven, cjAxis axis=cjAxisX, cjBool isFlip=cjFalse);

	const cjName&					GetLookAtName() const { return m_lookAt; }
	void							SetLookAtName(const cjName& name) { m_lookAt = name; }

	const cjName&					GetDrivenName() const { return m_driven; }
	void							SetDrivenName(const cjName& name) { m_driven = name; }

	cjBool							IsFlip() const { return _GetFlagAsBool(MASK_FLIP, BPOS_FLIP); }
	void							SetFlip(cjBool isFlip) { _SetFlagAsBool(MASK_FLIP, BPOS_FLIP, isFlip); }

	cjAxis							GetAxis() const { return (cjAxis)_GetFlag(MASK_AXIS, BPOS_AXIS); }
	void							SetAxis(cjAxis axis) { _SetFlag(MASK_AXIS, BPOS_AXIS, axis); }
	
	// pos/roll/scale
	cjAniEvaluator*					GetPosEvaluator() { return m_spEvaluators[EVAL_POS]; }
    void							SetPosEvaluator(cjAniEvaluator* evaluator);    
    cjAniEvaluator*					GetRollEvaluator() { return m_spEvaluators[EVAL_ROLL]; }
    void							SetRollEvaluator(cjAniEvaluator* evaluator);
    cjAniEvaluator*					GetScaleEvaluator() { return m_spEvaluators[EVAL_SCALE]; }
    void							SetScaleEvaluator(cjAniEvaluator* evaluator);
	
	static cjBool					LookAtFillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat);
	
protected:
    void							SetEvalChannelTypes();
		
protected:
    cjName							m_lookAt;
    cjName							m_driven;
    cjAniEvaluatorRPtr				m_spEvaluators[EVAL_MAX];
};

cjRefPtrDefine(cjLookAtEvaluator);


//
// cjPathEvaluator
//

class CJCORE_API cjPathEvaluator : public cjKeyBasedEvaluator
{
	CJSER_DECL_CLASS(cjPathEvaluator, cjKeyBasedEvaluator, 0, CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_SERIALIZE_POST
	CJSER_DECL_CLONE(cjPathEvaluator)
	CJSER_DECL_CLONE_POST

public:
	// cjAniEvaluator
	virtual cjBool					GetChannelPosedValue(cjUINT32 channel, void* rst) const;
	virtual cjBool					UpdateChannel(cjFLOAT etime, cjUINT32 channel, cjAniEvalSPDat* evalSPDat, void* rst) const;

    virtual void					GetValidTimeRange(cjFLOAT& stTime, cjFLOAT& enTime) const; 	// channel 1 range only
    virtual void					CacheKeys(cjFLOAT stTime, cjFLOAT enTime);  	// stTime~enTime: 0.0~1.0
    virtual cjAniEvaluator*			GetEvaluator(cjFLOAT stTime, cjFLOAT enTime);

    virtual cjBool					GetSPadInfo(cjUINT32 channel, cjBool forceAlwaysUpdate, cjRefObjNameHash* nodeHash, cjUINT32& fillSiz, 
										cjBool& isSharedFillDat, cjAniSPadBlock& spadBlk, cjBSplineBasisF3Dat*& bsBasisDat) const;
    virtual cjBool					InitSPadDat(cjUINT32 channel, cjAniEvalSPDat* evalSPDat, cjBSplineBasisF3Dat* bsBasisDat, 
										cjBool initSharedDat, cjRefObjNameHash* nodeHash, cjAniPoseBufHandle pbHandle) const;

    // cjKeyBasedInterpolator
    virtual cjUINT32				GetKeyChannelCount() const { return 2; } // [0] path [1] percent
    virtual cjUINT32				GetKeyCount(cjUINT32 channel) const;
    virtual cjAniKey::EKEY_TYPE		GetKeyType(cjUINT32 channel) const;
    virtual cjAniKey::EKEY_VALTYPE	GetKeyValType(cjUINT32 channel) const;
    virtual cjAniKey*				GetKeyAry(cjUINT32 channel) const;
    virtual cjBYTE					GetKeySiz(cjUINT32 channel) const;
	

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
    cjPathEvaluator();
    cjPathEvaluator(cjAniVec3Dat* pathDat, cjAniFloatDat* pctDat);
	
	// PCT Dat
	cjAniFloatDat*					GetPCTDat() const { return m_spPCTDat; }
	void							SetPCTDat(cjAniFloatDat* pctDat) { m_spPCTDat = pctDat; SetEvalChannelTypes(); }    
	cjAniKeyFloat*					GetPCTKeys(cjUINT32& keyCount, cjAniKeyFloat::EKEY_TYPE& keyType, cjBYTE& keySiz) const;
    void							ReplacePCTKeys(cjAniKeyFloat* keys, cjUINT32 keyCount, cjAniKeyFloat::EKEY_TYPE keyType);

	// Path Dat		
	cjAniVec3Dat*					GetPathDat() const { return m_spPathDat; }
	void							SetPathDat(cjAniVec3Dat* pathDat) { m_spPathDat = pathDat; SetEvalChannelTypes(); }
    cjAniKeyVec3*					GetPathKeys(cjUINT32& keyCount, cjAniKeyVec3::EKEY_TYPE& keyType, cjBYTE& keySiz) const;
	void							ReplacePathKeys(cjAniKeyVec3* keys, cjUINT32 keyCount, cjAniKeyVec3::EKEY_TYPE keyType);

	cjFLOAT							GetTotalPathLen(); // ignored calling when m_totalPathLength is not -1

    cjFLOAT							GetPathSpeed(cjINT32 keyIdx0, cjINT32 keyIdx1, cjFLOAT rtime) const;
    cjFLOAT							GetPathLength(cjINT32 keyIdx0, cjINT32 keyIdx1, cjFLOAT rtime) const;
    static cjFLOAT					GetPathLength(cjFLOAT ratio, const cjAniKeyVec3* posKey1, const cjAniKeyVec3* posKey2, 
										cjAniKeyVec3::PFInterpolation interpD1);

	void							SetConstSpeedDat();

    static cjBool					PathFillFunc(cjFLOAT etime, cjAniEvalSPDat* evalSPDat);
	static void						GetPathRotation(cjFLOAT pct, cjFLOAT ratio, const void* _pathFill/*const cjAniSPad::PathFill* pathFill*/, 
										cjMatrix3& matPathRot);
	
	// flag & set
	cjBool							IsBanking() const { return _GetFlagAsBool(MASK_BANKING, BPOS_BANKING); }
	void							SetBanking(cjBool banking) { _SetFlag(MASK_BANKING, BPOS_BANKING, banking); }
	BANKDIR							GetBankDir() const { return m_bankDir; }
	void							SetBankDir(BANKDIR dir) { m_bankDir = dir; }

	cjBool							IsConstSpeedDatDirty() const { return _GetFlagAsBool(MASK_CONST_SPEED_DAT_DIRTY, BPOS_CONST_SPEED_DAT_DIRTY); }
	void							SetConstSpeedDatDirty(cjBool dirty) { _SetFlag(MASK_CONST_SPEED_DAT_DIRTY, BPOS_CONST_SPEED_DAT_DIRTY, dirty); }	

	cjBool							IsConstSpeed() const { return _GetFlagAsBool(MASK_CONST_SPEED, BPOS_CONST_SPEED); }
	void							SetConstSpeed(cjBool constSpeed) { _SetFlagAsBool(MASK_CONST_SPEED, BPOS_CONST_SPEED, constSpeed); }

	cjBool							IsOpenCurve() const  { return _GetFlagAsBool(MASK_OPEN_CURVE, BPOS_OPEN_CURVE); }
	void							SetOpenCurve(cjBool openCurve){ _SetFlag(MASK_OPEN_CURVE, BPOS_OPEN_CURVE, openCurve); }

	cjBool							IsFollow() const { return _GetFlagAsBool(MASK_FOLLOW, BPOS_FOLLOW); } // apply rotation at update
	void							SetFollow(cjBool follow) { _SetFlag(MASK_FOLLOW, BPOS_FOLLOW, follow); }
	cjSWORD							GetFollowAxis() const { return m_followAxis; }
	void							SetFollowAxis(cjSWORD axis) { m_followAxis = axis; }
	
	cjFLOAT							GetMaxBankAngle() const { return m_maxBankAngle; }
	void							SetMaxBankAngle(cjFLOAT angle) { m_maxBankAngle = angle; } 

	cjFLOAT							GetSmoothing() const { return m_smooth; }
	void							SetSmoothing(cjFLOAT smooth) { m_smooth = smooth; }

	cjBool							IsAllowFlip() const { return _GetFlagAsBool(MASK_ALLOWFLIP, BPOS_ALLOWFLIP); }
	void							SetAllowFlip(cjBool allowFlip) { _SetFlag(MASK_ALLOWFLIP, BPOS_ALLOWFLIP, allowFlip); }
	cjBool							GetFlip() const { return _GetFlagAsBool(MASK_FLIP, BPOS_FLIP); }	
	void							SetFlip(cjBool flip) { _SetFlag(MASK_FLIP, BPOS_FLIP, flip); }	

protected:
    void							SetEvalChannelTypes();

protected:
    cjAniVec3DatRPtr				m_spPathDat;
    cjAniFloatDatRPtr				m_spPCTDat;

	BANKDIR							m_bankDir;
	cjFLOAT							m_maxBankAngle;
    cjFLOAT							m_smooth;    
    cjFLOAT							m_maxCurvature; // do not serialize
	cjSWORD							m_followAxis;
	
	cjINT32							m_pathLengthCount; // do not serialize
    cjFLOAT*						m_pathLengths;     // do not serialize
    cjFLOAT							m_totalPathLength; // do not serialize , -1 for need to calculate length of all path
};


//
// cjEvaluatorConv
//

class cjInterpolator;
class cjBSTMInterpolator;
class cjTMInterpolator;
class cjLookAtInterpolator;
class cjPathInterpolator;


class CJCORE_API cjEvaluatorConv
{
public:
    static cjAniEvaluator* Convert(cjInterpolator* interpolator, const cjAniEvaluatorTag& tag, cjBool doCollapse);

protected:
	static cjAniEvaluator* DoColorInterpolator(cjInterpolator* interpolator);
    static cjAniEvaluator* DoBoolInterpolator(cjInterpolator* interpolator);	
	static cjAniEvaluator* DoFloatInterpolator(cjInterpolator* interpolator);
	static cjAniEvaluator* DoVec3Interpolator(cjInterpolator* interpolator); // vec3s (scale) / vec3p (pos)
	static cjAniEvaluator* DoQuatInterpolator(cjInterpolator* interpolator);
	static cjAniEvaluator* DoTMInterpolator(cjInterpolator* interpolator, const cjName& name, cjBool doCollapse);        
    static cjAniEvaluator* DoBSTMInterpolator(cjBSTMInterpolator* bsTMInterp, const cjQuatTM& poseValue);    
	static cjAniEvaluator* DoTMInterpolator(cjTMInterpolator* tmInterp, const cjQuatTM& poseValue);

    static cjAniEvaluator* DoLookAtInterpolator(cjLookAtInterpolator* pkLookAtInterp, const cjName& name, cjBool doCollapse);
    static cjAniEvaluator* DoPathInterpolator(cjPathInterpolator* pkPathInterp);

};


#endif
