
#ifndef _CJRESCHR_H_
#define _CJRESCHR_H_


class cjMesh;


CJOBJ_DECL_CLASS_BEGIN(CJENGINE_API, cjResChr, cjObj, 0, cjEngine)

	CJOBJ_DECL_SERIALIZE
	CJOBJ_DECL_STATIC_REG

public:
	//cjObj
	virtual void cjObjPostLoad();
	
public:
	cjResChr();

	cjAnimator* CloneAnimator();

protected:
	virtual void OnDestroy();

private:
	cjAnimator* GenerateAnimator();

public:
	cjMesh*							m_model;
	cjTAry<cjResAniSeq*>			m_seqs;
	cjAnimatorRPtr					m_animator;
//	AniID2ResSeqInfoMap				m_seqInfoMap;

CJOBJ_DECL_CLASS_END(cjResChr)


#endif
