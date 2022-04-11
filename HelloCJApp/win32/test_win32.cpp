
#include "core/cjCore.h"
#include "engine/cjEngine.h"
#include "game/cjGame.h"

#include "engine/cjAppEngine.h"

#include "core/w32/cjToolClassDesc.h"

#include "engine/cjShaderDef.h"
#include "engine/cjShaderMtlDesc.h"
#include "engine/cjShaderProgDesc.h"


////cjtemp
//static void DoTest()
//{
//	cjAppEngine* engine = (cjAppEngine*)cjAppEngine::GetEngine();
//
//	cjNode* testSceneRoot = cjNULL;
//	testSceneRoot = engine->m_cjTestAnimator->GetSceneRoot()->GetSceneNode();
//
//	class cjUnitTestCtx_Tool_NodeInspector : public cjUnitTestCtx
//	{
//	public:
//		class cjToolClassDesc*			m_rstToolClsDesc;
//	};
//
//	cjUnitTestCtx_Tool_NodeInspector nodeInsCtx;
//
//    CJUT_EXTERN_DECL( , cjMain, Tool_NodeInspector)
//	CJUT_DO(cjMain, Tool_NodeInspector, &nodeInsCtx)
//
//	{
//		nodeInsCtx.m_rstToolClsDesc->DoCmd("show", 0);
//		nodeInsCtx.m_rstToolClsDesc->DoCmd("update", testSceneRoot);
//	}
//
//}


void GameMain()
{
	//cjTAry<cjName> tx;
	//tx.Push(cjName(_CT("cjCore")));

	//cjTAry<cjName> tb(tx);
		
//	test_hash();

//	test_mem();

	//cjNode* test = (cjNode*)cjNode::cjrStaticRTTI()->CreateInstance();

	//test->AddChild((cjNode*)cjNode::cjrStaticRTTI()->CreateInstance());
	//test->AddChild((cjNode*)cjNode::cjrStaticRTTI()->CreateInstance());
	//test->AddChild((cjNode*)cjNode::cjrStaticRTTI()->CreateInstance());

	//test->RemoveAllChild();

	//delete test;

//	cjString tmp = cjString(_CT("test"));
//	cjString t2 = tmp;

	//	cjTestMesh* newMesh = cjObjNew<cjTestMesh>();
		
//	cjExtDatStr str(L"testExtara");
//	const cjNameChar* s = str.GetValue();

 	cjObjCls* clsGame = cjObjLoadCls<cjGame>(_CU("cjGame.cjAppGame"));
	cjGame* game = cjObjNew<cjGame>(clsGame);

	cjResChr* chr = cjObjLoadPkgObj<cjResChr>(cjNULL, _CT("test_female.female00"));
//	cjResChr* chr = cjObjLoadPkgObj<cjResChr>(cjNULL, _CT("TestSol.TestSol"));
//	cjResChr* chr = cjObjLoadPkgObj<cjResChr>(cjNULL, _CT("female.testaaa"));
//	cjResChr* chr = cjObjLoadPkgObj<cjResChr>(cjNULL, _CT("TestResChar.Cat"));
//	cjResChr* chr = cjObjLoadPkgObj<cjResChr>(cjNULL, _CT("TestArmy.Soldier"));



	// cjTexture must be setted parent of m

//	cjObjLoadCls<cjGame>(_CU("game.cjAppGame"));
//	cjGame* game = cjObjNew<cjGame>(clsGame);

	cjAssert(game);
	if(game && game->Init())
	{
		while(1)
		{
			game->PumpMessage();

			if(game->Tick() != cjGame::ERUN_OK)
				break;

			game->Draw();			
		}
	}

	if(game)
		delete game;
}


// CJCORE_API cjUINT32 CJUT_TEST_cjTList();
 CJCORE_API cjUINT32 CJUT_TEST_cjStrCmdLine();
 

#include "engine/cjGLShader.h"


INT WINAPI WinMain( HINSTANCE hInInstance, HINSTANCE hPrevInstance, char* InCmdLine, INT nCmdShow )
{
	CJW32CRTDBG_INIT
					

	cjInitParam param;
	cjMemSet(&param, 0, sizeof(cjInitParam));

	cjWinDWORD flag = GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;
	::GetModuleHandleEx(flag, (LPCTSTR)&WinMain, (HMODULE*)&param.m_mainExecutableHInstance);

	cjMemZero(param.m_basePath, sizeof(param.m_basePath));

	{
			if(cjTypeInfo<cjGLRenPassRPtr>::TypeHasDestructor)
			{
				int a;
				a=1;
			}
			else
			{
				int c;
				c=1;
			}
	}

	
	/*{
		cjDefaultShaderPxlProgDesc pxlProgDesc;



		cjUINT32 aryIdx_01	= pxlProgDesc._AIN_WORLD_POS_ARYIDX;
		cjUINT32 bpos_01	= pxlProgDesc._AIN_WORLD_POS_BPOS;
		cjUINT32 sh_01		= pxlProgDesc._AIN_WORLD_POS_SHCOUNT;
		cjUINT32 vm_01		= pxlProgDesc._AIN_WORLD_POS_VMASK;

		cjUINT32 aryIdx_02	= pxlProgDesc._AIN_WORLD_NORMAL_ARYIDX;
		cjUINT32 bpos_02	= pxlProgDesc._AIN_WORLD_NORMAL_BPOS;
		cjUINT32 sh_02		= pxlProgDesc._AIN_WORLD_NORMAL_SHCOUNT;
		cjUINT32 vm_02		= pxlProgDesc._AIN_WORLD_NORMAL_VMASK;

		cjUINT32 aryIdx_03	= pxlProgDesc._AIN_WORLD_NBT_ARYIDX;
		cjUINT32 bpos_03	= pxlProgDesc._AIN_WORLD_NBT_BPOS;
		cjUINT32 sh_03		= pxlProgDesc._AIN_WORLD_NBT_SHCOUNT;
		cjUINT32 vm_03		= pxlProgDesc._AIN_WORLD_NBT_VMASK;

		cjUINT32 aryIdx_04	= pxlProgDesc._AIN_WORLD_VIEW_ARYIDX;
		cjUINT32 bpos_04	= pxlProgDesc._AIN_WORLD_VIEW_BPOS;
		cjUINT32 sh_04		= pxlProgDesc._AIN_WORLD_VIEW_SHCOUNT;
		cjUINT32 vm_04		= pxlProgDesc._AIN_WORLD_VIEW_VMASK;
		
		cjUINT32 aryIdx_05	= pxlProgDesc._APER_VTX_LIGHTING_ARYIDX;
		cjUINT32 bpos_05	= pxlProgDesc._APER_VTX_LIGHTING_BPOS;
		cjUINT32 sh_05		= pxlProgDesc._APER_VTX_LIGHTING_SHCOUNT;
		cjUINT32 vm_05		= pxlProgDesc._APER_VTX_LIGHTING_VMASK;
				
		cjUINT32 aryIdx_1	= pxlProgDesc._ATEX_DIFFUSE_CNT_ARYIDX;
		cjUINT32 bpos_1		= pxlProgDesc._ATEX_DIFFUSE_CNT_BPOS;
		cjUINT32 sh_1		= pxlProgDesc._ATEX_DIFFUSE_CNT_SHCOUNT;
		cjUINT32 vm_1		= pxlProgDesc._ATEX_DIFFUSE_CNT_VMASK;


		cjUINT32 aryIdx0	= pxlProgDesc._ARESERVED_TYPE0_ARYIDX;
		cjUINT32 bpos0		= pxlProgDesc._ARESERVED_TYPE0_BPOS;
		cjUINT32 sh0		= pxlProgDesc._ARESERVED_TYPE0_SHCOUNT;
		cjUINT32 vm0		= pxlProgDesc._ARESERVED_TYPE0_VMASK;


		cjUINT32 aryIdx = pxlProgDesc._ASAMP0_UVSET_INDEX_ARYIDX;
		cjUINT32 bpos = pxlProgDesc._ASAMP0_UVSET_INDEX_BPOS;
		cjUINT32 sh = pxlProgDesc._ASAMP0_UVSET_INDEX_SHCOUNT;
		cjUINT32 vm = pxlProgDesc._ASAMP0_UVSET_INDEX_VMASK;

	}*/
	



	// curPath : G:/mobile/HelloCJApp/resource
	char curPath[cjMAX_PATH] = {0};
	GetCurrentDirectoryA(cjMAX_PATH, curPath);

	cjLogger::SetLogFilePath(curPath, "cjWin32Log.txt");
	cjLogger::_StaticInit();


	cjNameChar* t = L"/var/containers/Bundle/Application/C61C5B46-8420-4696-8B0C-32DEB0653EDE/HelloCJApp.app/cjp/mdl/TestResChar.cjp";
	cjINT32 tn = cjStrlen(t);

//	cjStrcpy(param.m_basePath[cjFilePathHint_Base], cjMAX_PATH, curPath);
	cjFile::NormalizePath(param.m_basePath[cjFilePathHint_Base], cjMAX_PATH, curPath);

	cjInitParam_AddPkgPath(param, "cjp/mdl/*.cjp");

	cjInit(param);


//	CJUT_EXTERN_DECL(CJENGINE_API, cjEngine, SHADER_SCRIPT_TEST)
//	CJUT_DO(cjEngine, SHADER_SCRIPT_TEST, 0)


//	CJUT_EXTERN_DECL(CJCORE_API, cjCore, CJTHASH_TEST)
//	CJUT_DO(cjCore, CJTHASH_TEST, 0)




	//cjShaderMtlDesc desc;
	//cjShMtlDescUtil du(desc, cjTrue);
	//
	//du.SetFOG(cjShMtlDescUtil::FOG_LINEAR);
	//du.SetFOG(cjShMtlDescUtil::FOG_SQUARE);


//	cjUINT32 a[16];

	//a[0] = cjMtlDescUtil::TM_BPOS;
	//a[1] = cjMtlDescUtil::TM_VMASK;

	//a[2] = cjMtlDescUtil::TEXMAPCNT_BPOS;
	//a[3] = cjMtlDescUtil::TEXMAPCNT_VMASK;
	//a[4] = cjMtlDescUtil::TEXMAPCNT_ARYIDX;
	//a[5] = cjMtlDescUtil::TEXMAPCNT_DNBPOS;




	int d=0;
	d=1;


	//a[2] = cjMtlDescUtil::TM_ENBPOS;
	//
	//a[3] = cjMtlDescUtil::NORMAL_BPOS;
	//a[4] = cjMtlDescUtil::NORMAL_VMASK;
	//a[5] = cjMtlDescUtil::NORMAL_ENBPOS;

	//a[6] = cjMtlDescUtil::LIGHT_TYPE_BPOS;
	//a[7] = cjMtlDescUtil::LIGHT_TYPE_VMASK;
	//a[8] = cjMtlDescUtil::LIGHT_TYPE_ENBPOS;

	//a[9] = cjMtlDescUtil::VTXCOLOR_BPOS;
	//a[10] = cjMtlDescUtil::VTXCOLOR_VMASK;
	//a[11] = cjMtlDescUtil::VTXCOLOR_ENBPOS;


	/*const cjChar* aa = "abcd";
	const cjChar* bb = "vcde";

	typedef cjTHashNCase<const char*, cjStringA>        AppFullPathMap;
	AppFullPathMap                     hf;

    AppFullPathMap::Pair* insPair = hf.Insert(aa, bb);

	cjChar  _cc[25] = {"abcd"};
	cjChar  _dd[25] = {"eeee"};

	cjChar* cc = _cc;
	cjChar* dd = _dd;

	typedef cjTHashNCase<char*, cjStringA>        AppFullPathMap2;
	AppFullPathMap2                     hf2;

    AppFullPathMap2::Pair* insPair2 = hf2.Insert(cc, dd);*/


//	extern cjUINT32 CJUT_TEST_cjTNullableAry();
//	CJUT_TEST_cjTNullableAry();

//  CJUT_EXTERN_DECL( , cjMain, LoadAniPkg)
//	CJUT_DO(cjMain, LoadAniPkg, 0)

 //   CJUT_EXTERN_DECL( , cjMain, PkgMeshSave)
	//CJUT_DO(cjMain, PkgMeshSave)

//	CJUT_EXTERN_DECL(CJCORE_API, cjCore, CJTHASH_TEST)
//	CJUT_DO(cjCore, CJTHASH_TEST, 0)

//	CJCORE_API cjUINT32 CJUT_TEST_cjTList();
//	CJUT_TEST_cjTList();

//  CJUT_EXTERN_DECL( , cjMain, TextureTest)
//  CJUT_DO(cjMain, TextureTest, 0)

//	CJUT_EXTERN_DECL( , cjMain, cjStrAryPropertyTest)
//	CJUT_DO(cjMain, cjStrAryPropertyTest, 0)
	

	GameMain();


//    CJUT_EXTERN_DECL( , cjMain, SaveLoadNode)
//	CJUT_DO(cjMain, SaveLoadNode)

	cjShutdown();

	return 0;
}



































////
//
//
//
////
//
//HMODULE gs_sceneModule;
//cjToolLibInitPFN				gs_initPFN;
//cjToolLibShutdownPFN			gs_shutdownPFN;
//cjToolLibCreateClassDescPFN		gs_createClassDescPFN;
//cjToolClassDesc*				gs_toolClass = 0;
//
//
//cjMatrix g_bone[4] = {
//
//	cjMatrix(
//		cjVec4(0.0114f,	-0.9999f,	-0.0000f,	0),
//		cjVec4(0,		0,			-1,			0),
//		cjVec4(0.9999,	0.0114,		0,			0),
//		cjVec4(-0.0870,	0,			-4.0313,	1)),
//
//	cjMatrix(
//		cjVec4(0.9999,		0.0153,	0,		0),
//		cjVec4(-0.0153,		0.9999,	0,		0),
//		cjVec4(0,			0,		1,		0),
//		cjVec4(5.1047,		0,		0,		1)),
//
//	cjMatrix(
//		cjVec4(1,			0,		0,		0),
//		cjVec4(0,			1,		0,		0),
//		cjVec4(0,			0,		1,		0),
//		cjVec4(4.3519,		0,		0,		1)),
//
//	cjMatrix(
//		cjVec4(1,			0,		0,		0),
//		cjVec4(0,			1,		0,		0),
//		cjVec4(0,			0,		1,		0),
//		cjVec4(0,			0,		0,		1))
//};
//
//cjMatrix g_hierBone[3];
//
//
//
//float g_boneWeight[4][4] = {
//	{ 0.933,  0.0677,  0,   0 },
//	{ 0.620,  0.0379,  0,   0 },
//	{ 0.9765, 0.0234,  0,   0 },
//	{ 0.555,  0.444,   0,   0 }
//};
//
//unsigned char g_boneIndex[4][4] = {
//	{ 2, 1, 3, 0 },
//	{ 0, 1, 3, 0 },
//	{ 2, 1, 3, 0 },
//	{ 0, 1, 3, 0 }
//};
//
//cjVec4	g_vtx[4] = 
//{
//	cjVec4( -4,		5,		0, 1 ),
//	cjVec4( -4,		-5,		0, 1 ),
//	cjVec4(  4,		5,		0, 1 ),
//	cjVec4(	 4,		-5,		0, 1 )
//};
//
//
//void cjSkinningTest()
//{
//	g_hierBone[0] = g_bone[0];
//	g_hierBone[1] = g_hierBone[0] * g_bone[1];
//	g_hierBone[2] = g_hierBone[1] * g_bone[2];
//
//	cjVec4 p00 =  (( g_hierBone[ g_boneIndex[0][0] ] * g_vtx[0] ) * g_boneWeight[0][0]);
//	cjVec4 p01 =  (( g_hierBone[ g_boneIndex[0][1] ] * g_vtx[0] ) * g_boneWeight[0][1]);
//	cjVec4 p02 =  (( g_hierBone[ g_boneIndex[0][2] ] * g_vtx[0] ) * g_boneWeight[0][2]);
//	cjVec4 p03 =  (( g_hierBone[ g_boneIndex[0][3] ] * g_vtx[0] ) * g_boneWeight[0][3]);
//
//	cjVec4 p = p00 + p01 + p02 + p03;
//
//	cjLOG("[px] %f  %f  %f ", p.x, p.y, p.z);
//}
//
//
//
//void LibraryLoadTest()
//{
//	const char* path = "G:\\mobile\\HelloCJApp\\HelloCJApp.win32\\bin\\libcjNodeInspectorD.dll";
//	gs_sceneModule = LoadLibraryExA(path, 0, LOAD_WITH_ALTERED_SEARCH_PATH);
//	
//	if(!gs_sceneModule)
//	{
//	}
//	else
//	{
//		if(cjToolClassDesc::ResolveFuncFromDLL(gs_sceneModule, &gs_initPFN, &gs_shutdownPFN, 
//			&gs_createClassDescPFN))
//		{			
//			gs_initPFN();
//			gs_toolClass = (cjToolClassDesc*)gs_createClassDescPFN(0);
//
//			gs_toolClass->Init();
//		}
//	}
//}
//
//void LibraryLoadTestEnd()
//{
//	if(gs_sceneModule)
//	{
//		gs_shutdownPFN();
//		FreeLibrary(gs_sceneModule);
//		gs_sceneModule = 0;
//	}
//}
//
//cjLODMesh* gs_mesh = 0;
//
//void LoadMaxExpTest()
//{
//	gs_mesh = cjObjLoadPkgObj<cjLODMesh>(NULL, _CT("MaxExpTest.testmesh"));
//
//	int i=0;
//	i=1;
//
//}
//
//void Serialize_TestMeshPak(int save=1)
//{
//	if(save)
//	{
//		// saving
//		cjObjPackage* pkg = cjObj::cjObjCreatePkg(NULL, _CT("TestMeshTex"));
//		cjTexture* tex = cjObjImport<cjTexture>(pkg, _CT("texRGB"), "spr/test_rle.tga");
//
////		cjTestMesh* mesh = cjObjNew<cjTestMesh>(cjTestMesh::cjObjGetStaticCls(), pkg, _CT("cube"));
////		cjTestMesh::CreateTestStuffIndex(mesh);
//				
//		cjObj::cjObjSavePackage(pkg, NULL, 0, _CT("cjp/mdl/TestMeshTex.cjp"));
//	}
//	else	
//	{
//	//	cjTexture* tex = cjObjLoadPkgObj<cjTexture>(NULL, _CT("TestMeshPak.texRGB"));
//
//	////	if(tex)
//	//	//{
//	//	//	cjINT32 i=0;
//	//	//	i=1;
//	//	//}
//
//	//	cjTestMesh* mesh = cjObjLoadPkgObj<cjTestMesh>(NULL, _CT("TestMeshPak.cube"));
//
//	//	if(mesh)
//	//	{
//	//		cjINT32 k=0;
//	//		k=2;
//	//	}
//
//	}
//
//}
//
//class test_datXY : public cjMem
//{
//public:
//	test_datXY()
//	{
//		c = 2;
//	}
//
//	~test_datXY()
//	{
//		c = 5;
//	}
//
//	int c;
//};
//
//class test_dat : public cjMem
//{
//public:
//	test_dat()
//	{
//		a = 0xff;
//		 b.c = 0x0f;
//	}
//
//	~test_dat()
//	{
//		a = 0x10;
//	}
//
//	int sum()
//	{
//		return a = a+1;
//	}
//
//	int a;
//
//	test_datXY b;
//};
//
//
//void test_mem()
//{
//	test_dat* data = new test_dat;
//
//	data->a = 0x8f;
//	int tmp = data->sum();
//
//	cjWinDWORD d;
//	d=2;
//
//
//}
//
//void test_hash()
//{
//	typedef cjTHash<const cjString, INT32> TestHash;
//	TestHash hash(40);
//
////	cjString str(L"test");
//	hash.Insert(L"test",  100);
//	hash.Insert(L"test1", 101);
//	hash.Insert(L"test2", 102);
//	hash.Insert(L"test3", 103);
//
//	TestHash::Pair* v = hash.Find(L"test1");
//	cjBool rst;
//	
//	rst = hash.Remove(L"test2");
//	rst = hash.Remove(L"test2");
//	rst = hash.Find(L"test2") ? cjTrue : cjFalse;
//
//	cjWinDWORD d=0;
//	d=2;
//}
//
//
//
//
//void GameMain()
//{
//	//cjTAry<cjName> tx;
//	//tx.Push(cjName(_CT("cjCore")));
//
//	//cjTAry<cjName> tb(tx);
//		
////	test_hash();
//
////	test_mem();
//
//	//cjNode* test = (cjNode*)cjNode::cjrStaticRTTI()->CreateInstance();
//
//	//test->AddChild((cjNode*)cjNode::cjrStaticRTTI()->CreateInstance());
//	//test->AddChild((cjNode*)cjNode::cjrStaticRTTI()->CreateInstance());
//	//test->AddChild((cjNode*)cjNode::cjrStaticRTTI()->CreateInstance());
//
//	//test->RemoveAllChild();
//
//	//delete test;
//
////	cjString tmp = cjString(_CT("test"));
////	cjString t2 = tmp;
//
//	//	cjTestMesh* newMesh = cjObjNew<cjTestMesh>();
//		
////	cjExtDatStr str(L"testExtara");
////	const cjNameChar* s = str.GetValue();
//
// 	cjObjCls* clsGame = cjObjLoadCls<cjGame>(_CU("cjGame.cjAppGame"));
//	cjGame* game = cjObjNew<cjGame>(clsGame);
//
////	cjObjLoadCls<cjGame>(_CU("game.cjAppGame"));
////	cjGame* game = cjObjNew<cjGame>(clsGame);
//
//	cjAssert(game);
//	if(game && game->Init())
//	{
//		while(1)
//		{
//			game->PumpMessage();
//
//			if(game->Tick() != cjGame::ERUN_OK)
//				break;
//
//			game->Draw();
//		}
//	}
//
//	if(game)
//		delete game;
//}
//
////
////class cjRTTIHashA
////{
////public:
////	typedef cjSTLStrHashMap<const cjChar*, cjRTTI*>		cjRTTIHashMap;
////
////	const cjRTTI* FindA(const cjChar* name) const;
////	cjBool AddA(const cjChar* name, cjRTTI* rtti);
////
////private:
////	cjRTTIHashMap										m_rttiHMap;
////};
////
////const cjRTTI* cjRTTIHashA::FindA(const cjChar* name) const
////{
////	cjRTTIHashMap::const_iterator x = m_rttiHMap.find(name);
////	
////	if(x == m_rttiHMap.end())
////		return 0;
////
////	return (*x).second;
////}
////
////cjBool cjRTTIHashA::AddA(const cjChar* name, cjRTTI* rtti)
////{
////	if(!FindA(name))
////	{
////		cjRTTIHashMap::value_type val(name, rtti);
////		m_rttiHMap.insert(val);
////	}
////
////	cjAssert(!"[cjRTTIHash] ERR! - Dupplicated rtti name");
////
////	return cjFalse;
////}
////
////
////cjRTTIHashA gs_test;
//
//
//// CJCORE_API cjUINT32 CJUT_TEST_cjTList();
// CJCORE_API cjUINT32 CJUT_TEST_cjStrCmdLine();
//
//
// //CJTYPEINFO_SUPPORT_IMPL(cjTRefPtr<cjAniSeq>)
// //CJTYPEINFO_SUPPORT_IMPL(cjAniSeqPtr)
//
//
////
////void TestAniTestPack()
////{
////	cjResAniSeq* aniSeq = cjNULL;
////
////	cjResAniSeq* resAniSeq = cjObjLoadPkgObj<cjResAniSeq>(0, _CU("AniTest.Test_ani_idle04lookup"));
////
//////	aniSeq = cjObjNew<cjResAniSeq>(cjResAniSeq::cjObjGetStaticCls(), CJPKG_TEMP, L"Test_ani_idle04lookup");
////
////	int a=0;
////	a=1;
////
////}
////
//
//
//
//
//
//
//
//INT WINAPI WinMain( HINSTANCE hInInstance, HINSTANCE hPrevInstance, char* InCmdLine, INT nCmdShow )
//{
//	CJW32CRTDBG_INIT
//
//	cjInitParam param;
//	cjMemSet(&param, 0, sizeof(cjInitParam));
//
//	cjWinDWORD flag = GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;
//	::GetModuleHandleEx(flag, (LPCTSTR)&WinMain, (HMODULE*)&param.m_mainExecutableHInstance);
//
//	cjMemZero(param.m_basePath, sizeof(param.m_basePath));
//
//	// curPath : G:/mobile/HelloCJApp/resource
//	char curPath[cjMAX_PATH] = {0};
//	GetCurrentDirectoryA(cjMAX_PATH, curPath);
//
//	cjLogger::SetLogFilePath(curPath, "cjWin32Log.txt");
//	cjLogger::_StaticInit();
//
////	cjStrcpy(param.m_basePath[cjFilePathHint_Base], cjMAX_PATH, curPath);
//	cjFile::NormalizePath(param.m_basePath[cjFilePathHint_Base], cjMAX_PATH, curPath);
//
//	cjInitParam_AddPkgPath(param, "cjp/mdl/*.cjp");
//
//	cjInit(param);
//
////	extern cjUINT32 CJUT_TEST_cjTNullableAry();
////	CJUT_TEST_cjTNullableAry();
//
//
//	cjAniNote* aniNoteAry = cjAniNote::NewNoteAry(10);
//
//	aniNoteAry[0].SetNote(L"aa");
//	aniNoteAry[0].SetTime(0.9);
//
//	struct cc
//	{		
//		cjString							m_note;
//		cjFLOAT								m_timeSec;		
//	};
//
//	cc dd;
//
//	dd.m_timeSec = 2.0;
//	dd.m_note = L"aa";
//
//
//
//	cjString	note;
//
//	note = L"aa";
//
//	cjAniNote::DelNoteAry(aniNoteAry);
//
//	
//	//typedef cjTAry<cjAniSeqPtr>				AniSeqDatPtrAry;
//	//AniSeqDatPtrAry							seqResult;
//
//	//seqResult.SetSiz(10);
//
//	//CJUT_TEST_cjStrCmdLine();
//
////	TestAniTestPack();
//
//
//
////
////
////	//////////////////////////////////////////////////////////////////////////
////	
////	cjTransform A, B;
////
////	A.m_rotate.m[0][0] = 1.0f;		A.m_rotate.m[0][1] = 0.0f;			 A.m_rotate.m[0][2] = 0.0f;				
////	A.m_rotate.m[1][0] = 0.0f;		A.m_rotate.m[1][1] = 0.99999994;	 A.m_rotate.m[1][2] = -5.9604645e-008f; 
////	A.m_rotate.m[2][0] = 0.0f;		A.m_rotate.m[2][1] = 5.9604645e-008; A.m_rotate.m[2][2] = 0.99999994f;		
////
////	A.m_pos.x = 0;   A.m_pos.y = 5.9604645e-008; A.m_pos.z = 5.9604645e-008;
////	A.m_scale = cjVec3::UNIT;
////
////
////	B.m_rotate.m[0][0] = 0.011362553;		B.m_rotate.m[0][1] = -0.99993551;	 B.m_rotate.m[0][2] = -2.8014183e-006;
////	B.m_rotate.m[1][0] = -8.9406967e-008;	B.m_rotate.m[1][1] = 2.7418137e-006; B.m_rotate.m[1][2] = -1;				
////	B.m_rotate.m[2][0] = 0.99993545;		B.m_rotate.m[2][1] = 0.011362523;	 B.m_rotate.m[2][2] = 0.0f;				
////
////	B.m_pos.x = -0.087006949;   B.m_pos.y = 1.7621468e-007;		B.m_pos.z = -4.0313220;  
////	B.m_scale = cjVec3::UNIT;
////
////	cjTransform C;
////
////	C.m_rotate.m[0][0] = 0.011362642;		C.m_rotate.m[0][1] = 0.99993533;			 C.m_rotate.m[0][2] = 1.4900776e-007;				
////	C.m_rotate.m[1][0] = -0.99993539;		C.m_rotate.m[1][1] = 0.011362582;			 C.m_rotate.m[1][2] = -2.8305433e-006; 
////	C.m_rotate.m[2][0] = -2.8014183e-006;	C.m_rotate.m[2][1] = -5.9604645e-008;		 C.m_rotate.m[2][2] = 0.99999994;		
////
////	C.m_pos.x = 5.0319853;   C.m_pos.y = -0.029832516; C.m_pos.z = -1.8265347e-008;
////	C.m_scale = cjVec3::UNIT;
////	
////	
////	cjTransform D = A * B * C;
////	
////	cjMatrix matA = A;
////	cjMatrix matB = B;
////	cjMatrix matC = C;
////	cjMatrix matD;
////
////	matD = D;
////
////
////	DWORD _aa;
////	_aa=1;
////
////
////
////
//////	extern void xtest_mem();
//////	xtest_mem();
////
////
////	//typedef cjTStrideArray<cjINT32, 3> TestAry;
////
////	//{
////	//	TestAry ary;
////
////	//	cjINT32 a[3] = { 1, 2, 3 };
////	//	cjINT32 b[3] = { 4, 5, 6 };
////	//	cjINT32 c[3] = { 7, 8, 9 };
////
////	//	ary.Push(a);
////	//	ary.Push(b);
////	//	ary.Push(c);
////
////	//	cjChar tmp[MAX_PATH];
////
////	//	cjSprintf(tmp, MAX_PATH, "index:0 - %d, %d, %d \n", ary(0)[0], ary(0)[1], ary(0)[2]);
////	//	OutputDebugStringA(tmp);
////
////	//	cjSprintf(tmp, MAX_PATH, "index:1 - %d, %d, %d \n", ary(1)[0], ary(1)[1], ary(1)[2]);
////	//	OutputDebugStringA(tmp);
////
////	//	cjSprintf(tmp, MAX_PATH, "index:2 - %d, %d, %d \n", ary(2)[0], ary(2)[1], ary(2)[2]);
////	//	OutputDebugStringA(tmp);
////	//	
////	//	TestAry::Iterator x(ary,1);
////
////	//	cjSprintf(tmp, MAX_PATH, "iterator index:x0 - %d, %d, %d \n", x[0], x[1], x[2]);
////	//	OutputDebugStringA(tmp);
////
////	//	x++;
////
////	//	cjSprintf(tmp, MAX_PATH, "iterator index:x1 - %d, %d, %d \n", x[0], x[1], x[2]);
////	//	OutputDebugStringA(tmp);
////
////	//}
////
//////	Serialize_TestMeshPak(1);
//////	cjSerializeMeshSaveTest();
//////	cjSerializeMeshLoadTest();
////
////	LibraryLoadTest();
////
////	LoadMaxExpTest();
////
////	if(gs_toolClass)
////	{
////		gs_toolClass->DoCmd("show", 0);
////		gs_toolClass->DoCmd("update", gs_mesh->GetSceneRoot());
////	}
//
//	//MSG msg;
//	//for(;;)
//	//{
//	//	if(::PeekMessageA(&msg, 0, 0, 0, PM_NOREMOVE) == 0)
//	//	{
//	//		// no message
//	//		Sleep(1);
//
//	//	}
//	//	else
//	//	{
//	//		if(::GetMessageA(&msg, 0, 0, 0) == 0)
//	//		{
//	//			// wm_quit
//	//			break;
//	//		}
//
//	//		::TranslateMessage(&msg);
//	//		::DispatchMessage(&msg);
//	//	}
//	//}
//
////	Serialize_TestMeshPak();
////	SerilaizeTest();	
//
////	cjSkinningTest();
//
//
////extern cjUINT32 CJUT_cjMain_PkgSaveNode();
////CJUT_cjMain_PkgSaveNode();
//
//CJUT_EXTERN_DECL( , cjMain, LoadAniPkg)
//
//	CJUT_DO(cjMain, LoadAniPkg)
//
//
//	GameMain();
//
////	int* p = new int[1];
//
////	LibraryLoadTestEnd();
//
//	//////////////////////////////////////////////////////////////////////////
//	cjShutdown();
//
//	return 0;
//}

//
//
//HMODULE gs_sceneModule;
//cjToolLibInitPFN				gs_initPFN;
//cjToolLibShutdownPFN			gs_shutdownPFN;
//cjToolLibCreateClassDescPFN		gs_createClassDescPFN;
//cjToolClassDesc*				gs_toolClass = 0;
//
//
//cjMatrix g_bone[4] = {
//
//	cjMatrix(
//		cjVec4(0.0114f,	-0.9999f,	-0.0000f,	0),
//		cjVec4(0,		0,			-1,			0),
//		cjVec4(0.9999,	0.0114,		0,			0),
//		cjVec4(-0.0870,	0,			-4.0313,	1)),
//
//	cjMatrix(
//		cjVec4(0.9999,		0.0153,	0,		0),
//		cjVec4(-0.0153,		0.9999,	0,		0),
//		cjVec4(0,			0,		1,		0),
//		cjVec4(5.1047,		0,		0,		1)),
//
//	cjMatrix(
//		cjVec4(1,			0,		0,		0),
//		cjVec4(0,			1,		0,		0),
//		cjVec4(0,			0,		1,		0),
//		cjVec4(4.3519,		0,		0,		1)),
//
//	cjMatrix(
//		cjVec4(1,			0,		0,		0),
//		cjVec4(0,			1,		0,		0),
//		cjVec4(0,			0,		1,		0),
//		cjVec4(0,			0,		0,		1))
//};
//
//cjMatrix g_hierBone[3];
//
//
//
//float g_boneWeight[4][4] = {
//	{ 0.933,  0.0677,  0,   0 },
//	{ 0.620,  0.0379,  0,   0 },
//	{ 0.9765, 0.0234,  0,   0 },
//	{ 0.555,  0.444,   0,   0 }
//};
//
//unsigned char g_boneIndex[4][4] = {
//	{ 2, 1, 3, 0 },
//	{ 0, 1, 3, 0 },
//	{ 2, 1, 3, 0 },
//	{ 0, 1, 3, 0 }
//};
//
//cjVec4	g_vtx[4] = 
//{
//	cjVec4( -4,		5,		0, 1 ),
//	cjVec4( -4,		-5,		0, 1 ),
//	cjVec4(  4,		5,		0, 1 ),
//	cjVec4(	 4,		-5,		0, 1 )
//};
//
//
//void cjSkinningTest()
//{
//	g_hierBone[0] = g_bone[0];
//	g_hierBone[1] = g_hierBone[0] * g_bone[1];
//	g_hierBone[2] = g_hierBone[1] * g_bone[2];
//
//	cjVec4 p00 =  (( g_hierBone[ g_boneIndex[0][0] ] * g_vtx[0] ) * g_boneWeight[0][0]);
//	cjVec4 p01 =  (( g_hierBone[ g_boneIndex[0][1] ] * g_vtx[0] ) * g_boneWeight[0][1]);
//	cjVec4 p02 =  (( g_hierBone[ g_boneIndex[0][2] ] * g_vtx[0] ) * g_boneWeight[0][2]);
//	cjVec4 p03 =  (( g_hierBone[ g_boneIndex[0][3] ] * g_vtx[0] ) * g_boneWeight[0][3]);
//
//	cjVec4 p = p00 + p01 + p02 + p03;
//
//	cjLOG("[px] %f  %f  %f ", p.x, p.y, p.z);
//}
//
//
//
//void LibraryLoadTest()
//{
//	const char* path = "G:\\mobile\\HelloCJApp\\HelloCJApp.win32\\bin\\libcjNodeInspectorD.dll";
//	gs_sceneModule = LoadLibraryExA(path, 0, LOAD_WITH_ALTERED_SEARCH_PATH);
//	
//	if(!gs_sceneModule)
//	{
//	}
//	else
//	{
//		if(cjToolClassDesc::ResolveFuncFromDLL(gs_sceneModule, &gs_initPFN, &gs_shutdownPFN, 
//			&gs_createClassDescPFN))
//		{			
//			gs_initPFN();
//			gs_toolClass = (cjToolClassDesc*)gs_createClassDescPFN(0);
//
//			gs_toolClass->Init();
//		}
//	}
//}
//
//void LibraryLoadTestEnd()
//{
//	if(gs_sceneModule)
//	{
//		gs_shutdownPFN();
//		FreeLibrary(gs_sceneModule);
//		gs_sceneModule = 0;
//	}
//}
//
//cjLODMesh* gs_mesh = 0;
//
//void LoadMaxExpTest()
//{
//	gs_mesh = cjObjLoadPkgObj<cjLODMesh>(NULL, _CT("MaxExpTest.testmesh"));
//
//	int i=0;
//	i=1;
//
//}
//
//void Serialize_TestMeshPak(int save=1)
//{
//	if(save)
//	{
//		// saving
//		cjObjPackage* pkg = cjObj::cjObjCreatePkg(NULL, _CT("TestMeshTex"));
//		cjTexture* tex = cjObjImport<cjTexture>(pkg, _CT("texRGB"), "spr/test_rle.tga");
//
////		cjTestMesh* mesh = cjObjNew<cjTestMesh>(cjTestMesh::cjObjGetStaticCls(), pkg, _CT("cube"));
////		cjTestMesh::CreateTestStuffIndex(mesh);
//				
//		cjObj::cjObjSavePackage(pkg, NULL, 0, _CT("cjp/mdl/TestMeshTex.cjp"));
//	}
//	else	
//	{
//	//	cjTexture* tex = cjObjLoadPkgObj<cjTexture>(NULL, _CT("TestMeshPak.texRGB"));
//
//	////	if(tex)
//	//	//{
//	//	//	cjINT32 i=0;
//	//	//	i=1;
//	//	//}
//
//	//	cjTestMesh* mesh = cjObjLoadPkgObj<cjTestMesh>(NULL, _CT("TestMeshPak.cube"));
//
//	//	if(mesh)
//	//	{
//	//		cjINT32 k=0;
//	//		k=2;
//	//	}
//
//	}
//
//}
//
//class test_datXY : public cjMem
//{
//public:
//	test_datXY()
//	{
//		c = 2;
//	}
//
//	~test_datXY()
//	{
//		c = 5;
//	}
//
//	int c;
//};
//
//class test_dat : public cjMem
//{
//public:
//	test_dat()
//	{
//		a = 0xff;
//		 b.c = 0x0f;
//	}
//
//	~test_dat()
//	{
//		a = 0x10;
//	}
//
//	int sum()
//	{
//		return a = a+1;
//	}
//
//	int a;
//
//	test_datXY b;
//};
//
//
//void test_mem()
//{
//	test_dat* data = new test_dat;
//
//	data->a = 0x8f;
//	int tmp = data->sum();
//
//	cjWinDWORD d;
//	d=2;
//
//
//}
//
//void test_hash()
//{
//	typedef cjTHash<const cjString, INT32> TestHash;
//	TestHash hash(40);
//
////	cjString str(L"test");
//	hash.Insert(L"test",  100);
//	hash.Insert(L"test1", 101);
//	hash.Insert(L"test2", 102);
//	hash.Insert(L"test3", 103);
//
//	TestHash::Pair* v = hash.Find(L"test1");
//	cjBool rst;
//	
//	rst = hash.Remove(L"test2");
//	rst = hash.Remove(L"test2");
//	rst = hash.Find(L"test2") ? cjTrue : cjFalse;
//
//	cjWinDWORD d=0;
//	d=2;
//}
//
//
//
//
//void GameMain()
//{
//	//cjTAry<cjName> tx;
//	//tx.Push(cjName(_CT("cjCore")));
//
//	//cjTAry<cjName> tb(tx);
//		
////	test_hash();
//
////	test_mem();
//
//	//cjNode* test = (cjNode*)cjNode::cjrStaticRTTI()->CreateInstance();
//
//	//test->AddChild((cjNode*)cjNode::cjrStaticRTTI()->CreateInstance());
//	//test->AddChild((cjNode*)cjNode::cjrStaticRTTI()->CreateInstance());
//	//test->AddChild((cjNode*)cjNode::cjrStaticRTTI()->CreateInstance());
//
//	//test->RemoveAllChild();
//
//	//delete test;
//
////	cjString tmp = cjString(_CT("test"));
////	cjString t2 = tmp;
//
//	//	cjTestMesh* newMesh = cjObjNew<cjTestMesh>();
//		
////	cjExtDatStr str(L"testExtara");
////	const cjNameChar* s = str.GetValue();
//
// 	cjObjCls* clsGame = cjObjLoadCls<cjGame>(_CU("cjGame.cjAppGame"));
//	cjGame* game = cjObjNew<cjGame>(clsGame);
//
////	cjObjLoadCls<cjGame>(_CU("game.cjAppGame"));
////	cjGame* game = cjObjNew<cjGame>(clsGame);
//
//	cjAssert(game);
//	if(game && game->Init())
//	{
//		while(1)
//		{
//			game->PumpMessage();
//
//			if(game->Tick() != cjGame::ERUN_OK)
//				break;
//
//			game->Draw();
//		}
//	}
//
//	if(game)
//		delete game;
//}
//
