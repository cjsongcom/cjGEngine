
#include "core/cjCore.h"
#include "engine/cjEngine.h"


CJUT_DECL( , cjMain, LoadAniPkg)



static cjObjPackage* g_pkg[100];


static void CreateAniTestPack()
{
	g_pkg[0] = cjObj::cjObjCreatePkg(cjNULL, _CT("AniTest"));

	cjResAniSeq* aniSeq = cjObjNew<cjResAniSeq>(cjResAniSeq::cjObjGetStaticCls(), g_pkg[0], _CT("ani_idle_00"));

	cjAniSeqRPtr aniSeqRPtr= new cjAniSeq(_CT("test"));

	aniSeq->m_seq = aniSeqRPtr;

	cjObj::cjObjSavePackage(g_pkg[0], 0, 0, _CT("cjp/mdl/Ani.cjp"));

	int a=0;
	a=1;
}

static void LoadAniTestPack()
{
	g_pkg[0] = cjObj::cjObjCreatePkg(cjNULL, _CT("AniTest"));
		
	cjResAniSeq* aniSeq = cjObjLoadPkgObj<cjResAniSeq>(NULL, _CT("AniTest.Test_ani_idle12heartattack"));

//	cjResAniSeq* aniSeq = cjObjNew<cjResAniSeq>(cjResAniSeq::cjObjGetStaticCls(), g_pkg[0], _CT("ani_idle_00"));
//	cjAniSeqRPtraniSeqRPtr= new cjAniSeq(_CT("test"));
//  aniSeq->m_seq = aniSeqPtr;

	int a=0;
	a=1;

}


static void TestAniTestPack()
{
//	cjResAniSeq* aniSeq = cjNULL;
//
//	cjResAniSeq* resAniSeq = cjObjLoadPkgObj<cjResAniSeq>(0, _CU("AniTest.Test_ani_idle04lookup"));
//
//	const cjNameChar* pkgName = _CT("AniTest");
//	cjObjPackage* pkg = (cjObjPackage*)cjObj::cjObjFind(cjObjPackage::cjObjGetStaticCls(), 0, pkgName);
//
////	aniSeq = cjObjNew<cjResAniSeq>(cjResAniSeq::cjObjGetStaticCls(), pkg, L"Test_ani_idle04lookup");
//
//	int a=0;
//	a=1;
}


static void SaveTestResChar()
{
	g_pkg[0] = cjObj::cjObjCreatePkg(cjNULL, _CT("UT_TestResChar"));
		
	cjResChr* chr = cjObjNew<cjResChr>(cjResChr::cjObjGetStaticCls(), g_pkg[0], _CT("Cat"));

	cjResAniSeq* aniSeq0 
		= cjObjNew<cjResAniSeq>(cjResAniSeq::cjObjGetStaticCls(), g_pkg[0], _CT("R0"));

	cjResAniSeq* aniSeq1 
		= cjObjNew<cjResAniSeq>(cjResAniSeq::cjObjGetStaticCls(), g_pkg[0], _CT("R1"));

	chr->m_model = cjNULL;
	chr->m_seqs.Push(aniSeq0);
	chr->m_seqs.Push(aniSeq1);

	cjObj::cjObjSavePackage(g_pkg[0], cjNULL, 0, _CT("cjp/mdl/UT_TestResChar.cjp"));
	
	int a=0;
	a=1;
}


int g_dat[] = 
{ 
	567347	,
	504909	,
	267275	,
	279932	,
	539894	,
	//551630	,
	//514294	,
	//525002	,
	//254001	,
	//38371	,
	//49849	,
	594		,
	//20315	,
	//70388	,
	//218637	,
	//235540	,
	//82989	,
	//157850	,
	//295381	,
};

int g_dat_num = sizeof(g_dat) / sizeof(g_dat[0]);


cjINT32 fnCmp(int& a, int& b)
{
	return a < b;
}

#ifdef _USE_CJTQSORT_DBG_

class cjTSortDBGViewEx2 : public cjTSortDBGView
{
public:
	cjTSortDBGViewEx2()
	{
		m_seq = 0;
	}

	virtual void DBGViewQ(void* ary, cjAryOff first, cjAryOff last, cjAryOff cen) 
	{
		int* a = (int*)ary;

		cjMLOG(L"**** begin[%03d] %d,%d,c=%d ", m_seq, first, last, cen);

		for(cjAryOff i=0; i<m_count; i++)
		{
			cjMLOG(L"[%03d] %u ", i, a[i]);
		}

		cjMLOG(L"**** end[%03d] %d,%d,c=%d ", m_seq, first, last, cen);

		m_seq++;
	}

	cjUINT32		m_seq;
	cjAryOff		m_count;
};

#endif


static void LoadTestResChar()
{
//	g_pkg[0] = cjObj::cjObjCreatePkg(cjNULL, _CT("UT_TestResChar"));
//	cjTSortDBGViewEx2 dve;

//	dve.m_count = g_dat_num;
//	cjTQSort(g_dat, 0, g_dat_num-1, fnCmp, &dve);

	cjResChr* chr = cjObjLoadPkgObj<cjResChr>(cjNULL, _CT("TestResChar.Cat"));

	cjAniSeq* seq = chr->m_animator->GetSeq(0);

	cjAnimatorRPtr cloned = chr->CloneAnimator();
	
	cloned->SetTargetSeqID(0);
	cloned->Update(0.0f);

	cjSceneNode* sn = (cjSceneNode*)(cloned->GetSceneRoot()->GetSceneNode());
	sn->Update(0.5, cjTrue);

//	cloned->ReleaseRef();

	cjAnimatorRPtr cloned2 = (cjAnimator*)cloned->Clone();
	

	int a=0;
	a=1;
}


CJUT_IMPL_BEGIN( ,cjMain, LoadAniPkg)

//	CreateAniTestPack();
//	TestAniTestPack();
//	LoadAniTestPack();
	
//	SaveTestResChar();

//	quicksort(g_dat, 0, g_dat_num-1);

	LoadTestResChar();

CJUT_IMPL_END( , cjMain, LoadAniPkg)

