
#include "core/cjCore.h"
#include "engine/cjEngine.h"


CJUT_DECL( , cjMain, SaveLoadNode)

static cjObjPackage* g_pkg = cjNULL;

cjNode* g_serRoot = 0;
cjNode* g_serRoot_Load = 0;


static void TestTScenePropListPtr()
{

	{
//		typedef cjScenePropPtrList
		cjScenePropRPtrList propLst;	
		cjScenePropRPtr dat = new cjSceneRenPropAlpha;

		propLst.PushBack(dat);
	}

	int iu=0;
	iu=1;

}

static void MakeSerNodes()
{
	g_serRoot = cjNode::NewInst();
	g_serRoot->SetName(L"Root");
	g_serRoot->AddRef();
	
	cjSceneNode* row00 = cjSceneNode::NewInst();
	{
		row00->SetName(L"row00");
		g_serRoot->AddChild(row00);

			cjSceneRenPropAlpha* srp00_alpha = cjSceneRenPropAlpha::NewInst();		
			row00->AttachSceneProp(srp00_alpha);

			cjSceneRenPropShade* srp01_shade = cjSceneRenPropShade::NewInst();
			row00->AttachSceneProp(srp01_shade);

			cjNode* row00_row10 = cjNode::NewInst();
			row00_row10->SetName(L"row00_row10");
			row00->AddChild(row00_row10);
	}
	
	cjNode* row01 = cjNode::NewInst();
	{
		row01->SetName(L"row01");
		g_serRoot->AddChild(row01);

		cjNode* row01_row10 = cjNode::NewInst();
		{
			row01_row10->SetName(L"row01_row10");
			row01->AddChild(row01_row10);
		}

		cjNode* row01_row11 = cjNode::NewInst();
		{
			row01_row11->SetName(L"row01_row11");
			row01->AddChild(row01_row11);

			cjExtDatBool* iB = cjExtDatBool::NewInst();
			iB->SetValue(cjTrue);
			row01_row11->AddExtDat(iB);

			cjExtDatInt* iV = cjExtDatInt::NewInst();
			iV->SetValue(100);
			row01_row11->AddExtDat(iV);

			cjExtDatBinary* bin = cjExtDatBinary::NewInst();

			cjBYTE bd[6] = { 0xa, 0xb, 0xc, 0xd, 0xe, 0xf };
			bin->SetValue(6, bd);
			row01_row11->AddExtDat(bin);

			cjExtDatVector* vec = cjExtDatVector::NewInst();

			cjVec4 v4 = cjVec4(1, 0.5, 0.3, 0.1);
			vec->SetValue(v4);
			row01_row11->AddExtDat(vec);

			const static cjNameChar* strAry[3] = {
				L"abc",
				L"def",
				L"ghi"
			};

			cjINT32 numNotes = 3;
			cjAniNote* noteAry = cjAniNote::NewNoteAry(numNotes);

			for(cjINT32 k=0; k<numNotes; k++)
			{
				noteAry[k].SetTime(0.1+k);
				noteAry[k].SetNote(strAry[k]);
			}
			 
			cjExtDatAniNote* aniNote = cjExtDatAniNote::NewInst();
			aniNote->SetNotes(noteAry, numNotes);
			row01_row11->AddExtDat(aniNote);
			
			//cjExtDatStr* str = cjExtDatStr::NewInst();
			//	str->SetString(L"TestString");


			//cjExtDatStr* mstr = cjExtDatStr::NewInst();
			//	mstr->SetMultiString(strAry, 3);

			//row01_row11->AddExtDat(mstr);
		}
	}

	cjNode* row02 = cjNode::NewInst();
	{
		row02->SetName(L"row02");
		g_serRoot->AddChild(row02);
			
			cjNode* row02_row10 = cjNode::NewInst();
			{
				row02_row10->SetName(L"row02_row10");
				row02->AddChild(row02_row10);
			}
	}

//
//
////			cjExtDatMatrix* mat = cjExtDatMatrix::NewInst();
//
//
//		cjNode* row01_row12 = cjNode::NewInst();
//			row01_row12->SetName(L"row01_row12");
//
//		row01->AddChild(row01_row10);
//		row01->AddChild(row01_row11);
//		row01->AddChild(row01_row12);
//

}


void SerilaizeTest()
{
	MakeSerNodes();

	cjFStream* wstream = cjFileSystem::Get()->CreateFileWriteStream("testScene.obj");
	cjSerObjStreamBroker wser(wstream);

	//////	// saving
	wser.Serialize(&g_serRoot);
	wser.Report(0);

	wstream->Close();
	
	delete wstream;

	g_serRoot->ReleaseRef();
	g_serRoot = 0;
		
	//-----------------------------------

	cjFStream* rstream = cjFileSystem::Get()->CreateFileReadStream("testScene.obj");
	cjSerObjStreamBroker rser(rstream);

	rser.Serialize(&g_serRoot_Load);
	rstream->Close();

	delete rstream;

	CJ_REF_SAFE_RELEASE(g_serRoot_Load);
}


static void SaveTest()
{
	g_pkg = cjObj::cjObjCreatePkg(NULL, _CT("PkgNodeTest"));

	cjPkgNodeSaveTest* saveTest = cjObjNew<cjPkgNodeSaveTest>(cjPkgNodeSaveTest::cjObjGetStaticCls(), 
		g_pkg, _CT("testNode"));

	MakeSerNodes();

	saveTest->m_node = g_serRoot;

	cjObj::cjObjSavePackage(g_pkg, NULL, 0, _CT("cjp/mdl/PkgNodeTest.cjp"));

	g_serRoot->ReleaseRef();
}

static void LoadTest()
{
	g_pkg = cjObj::cjObjCreatePkg(NULL, _CT("PkgNodeTest"));

	cjPkgNodeSaveTest* saveTest = cjObjLoadPkgObj<cjPkgNodeSaveTest>(cjNULL, _CT("PkgNodeTest.testNode"));
}


cjUINT32 CJUT_cjMain_PkgSaveNode()
{
//	TestTScenePropListPtr();

	// SerilaizeTest();	

	//MakeSerNodes();
	//g_serRoot->ReleaseRef();

	SaveTest();
	LoadTest();

	return 0;
}


CJUT_IMPL_BEGIN( ,cjMain, SaveLoadNode)

//	CreateAniTestPack();
//	TestAniTestPack();
//	LoadAniTestPack();
	
//	SaveTestResChar();

//	quicksort(g_dat, 0, g_dat_num-1);

	CJUT_cjMain_PkgSaveNode();

CJUT_IMPL_END( , cjMain, SaveLoadNode)