
#include "cjEnginePCH.h"
#include "cjResChr.h"


CJOBJ_IMPL_CLASS(cjResChr)

CJOBJ_IMPL_SERIALIZE_BEGIN(cjResChr)
	
	if(stream.IsObjOnly())
		return;
	
CJOBJ_IMPL_SERIALIZE_END(cjResChr)

CJOBJ_IMPL_STATIC_REG_BEGIN(cjResChr)

	// cjdefer
	// create property in cjResAniSeq class (outer class)
	cjObjProperty* model = new(GetCls(), _CT("model"), (cjTFLAG)0) cjObjProperty(CJ_PROP_NATIVE_OFF(m_model), 0, 0, cjMesh::cjObjGetStaticCls());
	//objAry
	//cjSerObjRefProperty* ser = new(GetCls(), _CT("seq"), (cjTFLAG)0) cjSerObjRefProperty(CJ_PROP_NATIVE_OFF(m_seq), 0, 0, CJRTTI(cjAniSeq));

	cjAryProperty* seqAry = new(GetCls(), _CT("seqs"), 0) cjAryProperty(CJ_PROP_NATIVE_OFF(m_seqs), 0, 0);
	seqAry->m_aryItem = new (seqAry, _CT("objProp0"), 0) cjObjProperty();

	// cjAryProperty* seqAry1 = new(GetCls(), _CT("seqsB"), 0), cjAryProperty(CJ_PROP_NATIVE_OFF(m_seqsB), 0, 0); 
	// seqAry->m_aryItem = new (seqAry, _CT("objProp1"), 0) cjObjProperty();

CJOBJ_IMPL_STATIC_REG_END(cjResChr)

void cjResChr::cjObjPostLoad()
{
	cjSuper::cjObjPostLoad();
	m_animator = GenerateAnimator();
}

cjResChr::cjResChr()
{

}

void cjResChr::OnDestroy()
{
	m_animator = cjNULL;
	cjSuper::OnDestroy();
}

cjAnimator* cjResChr::GenerateAnimator()
{
	cjAnimatorDat* animatorDat = new cjAnimatorDat; 

	cjBool isCumulativeAni = cjFalse;

	cjAnimator* animator = new cjAnimator(animatorDat, cjFalse);

	if(m_model)
		animator->ChangeSceneRoot(m_model->GetSceneRoot());

	for(cjAryOff i=0; i<m_seqs.Count(); i++)
	{
		cjResAniSeq* curResAniSeq = m_seqs(i);
		cjAniSeq* aniSeq = curResAniSeq ? curResAniSeq->m_seq : cjNULL;

		if(aniSeq)
			animator->ChangeSeq(i, aniSeq);
	}

	return animator;
}

cjAnimator* cjResChr::CloneAnimator()
{
	if(!m_animator)
		return cjNULL;

	return (cjAnimator*)m_animator->Clone();
}
