
#include "core/cjCore.h"
#include "engine/cjEngine.h"


CJUT_DECL( , cjMain, PkgMeshSave)


static cjObjPackage* g_pkg[100];
static cjLODMesh* g_mesh;




static  void cjSerializeMeshSaveTest()
{
	cjObjPackage* serMeshTestPkg = cjObj::cjObjCreatePkg(NULL, _CT("SerMeshTest"));

	cjTestMesh* mesh = cjObjNew<cjTestMesh>(cjTestMesh::cjObjGetStaticCls(), serMeshTestPkg, _CT("serMesh00"));
	cjTestMesh::CreateTestStuffIndex(mesh);
	cjTexture* objTex = cjObjLoadPkgObj<cjTexture>(0, _CT("TestMeshPak_Test.texRGB"));
	
	cjObjPackage* pkg = 0;
	{
		pkg = cjObj::cjObjCreatePkg(NULL, _CT("TestMeshEx"));
		mesh = cjObjNew<cjTestMesh>(cjTestMesh::cjObjGetStaticCls(), pkg, _CT("sceneCube"));
		cjTestMesh::CreateTestStuffIndex(mesh);
	}

	cjSceneNode* root = new cjSceneNode();
	root->AddRef();
	
	cjSceneNode* child00 = new cjSceneNode();

			cjSceneRenPropAlpha* alphaProp = new cjSceneRenPropAlpha();
			alphaProp->SetAlphaBlending(cjFalse);
			alphaProp->SetSrcBlend(cjSceneRenPropAlpha::EALPHABL_SRCALPHA);
			alphaProp->SetDstBlend(cjSceneRenPropAlpha::EALPHABL_INVSRCALPHA);
			alphaProp->SetAlphaTestUse(cjTrue);
			alphaProp->SetAlphaTestFunc(cjSceneRenPropAlpha::EALPHATESTFN_GREATEREQUAL);
			alphaProp->SetAlphaTestRef(254);
			child00->AttachSceneProp(alphaProp);


			cjSceneRenPropMaterial* mtlProp = new cjSceneRenPropMaterial();
			mtlProp->SetAmbientColor (cjColorF(0.8f, 0.7f, 0.6f, 0.234f));
			mtlProp->SetDiffuseColor (cjColorF(0.1f, 0.2f, 0.3f, 0.456f));
			mtlProp->SetSpecularColor(cjColorF(0.9f, 0.8f, 0.7f, 0.134f));
			mtlProp->SetEmissiveColor(cjColorF(0.5f, 0.6f, 0.7f, 0.234f));
			mtlProp->SetShineness(0.543f);
			mtlProp->SetAlpha(0.123f);
			child00->AttachSceneProp(mtlProp);


			cjSceneRenPropShade* shadeProp = new cjSceneRenPropShade();
			shadeProp->SetGouraudShade(cjTrue);
			child00->AttachSceneProp(shadeProp);


			cjSceneRenPropSpecular* specularProp = new cjSceneRenPropSpecular();
			specularProp->SetSpecular(cjTrue);
			child00->AttachSceneProp(specularProp);


			cjSceneRenPropWireframe* wireProp = new cjSceneRenPropWireframe();
			wireProp->SetUseWireframe(cjTrue);
			child00->AttachSceneProp(wireProp);


			cjSceneRenPropTwoSide* twoSideProp = new cjSceneRenPropTwoSide();
			twoSideProp->SetUseTwoSide(cjTrue);
			child00->AttachSceneProp(twoSideProp);


			cjSceneRenPropTextures* texProp = new cjSceneRenPropTextures();
				cjSceneRenPropTextures::Tex* tex = new cjSceneRenPropTextures::Tex();

				tex->SetTexObj(objTex);
				tex->SetClampModeU(cjTexClamp_Clamp);
				tex->SetClampModeV(cjTexClamp_Mirror);
				tex->SetFilterMode(cjTexFilter_AnisoTropic);
				tex->SetMaxAnisotropy(1);
				tex->SetTexUVSrcIndex(0);

			texProp->SetTex(cjSceneRenPropTextures::TEX_DIFFUSE, tex);
			child00->AttachSceneProp(texProp);
									
	cjSceneNode* child01 = new cjSceneNode();
		{
//			cjSceneSkinMeshMod* skinMeshMod = new cjSceneSkinMeshMod;
			cjSceneMesh* sceneMesh = new cjSceneMesh();

			sceneMesh->SetOwnerMesh(mesh);
			sceneMesh->SetVertexStream(mesh->GetVertexStream(0));
			sceneMesh->SetIndexBuffer(mesh->GetIndexBuffer(0));	
//			sceneMesh->AddModifier(skinMeshMod);

			child01->AddChild(sceneMesh);
		}
		

	cjSceneNode* child02 = new cjSceneNode();
		
	root->AddChild(child00);
	root->AddChild(child01);
	root->AddChild(child02);

	{
		mesh->SetSceneRoot(root);				
		cjObj::cjObjSavePackage(pkg, NULL, 0, _CT("cjp/mdl/TestMeshEx.cjp"));
	}
	
	//cjFStream* wstream = cjFileSystem::Get()->CreateFileWriteStream("testMesh.obj");
	//cjSerObjStreamBroker wser(wstream);

	//wser.SerializeNode((cjNode**)&root);

	//delete wstream;

	root->ReleaseRef();
	
//	cjObj::cjObjSavePackage(serMeshTestPkg, NULL, 0, _CT("cjp/mdl/SerMeshTest.cjp"));
}


static void cjSerializeMeshLoadTest()
{
	cjNode* root = 0;

	//cjFStream* rstream = cjFileSystem::Get()->CreateFileReadStream("testMesh.obj");
	//cjSerObjStreamBroker rser(rstream);

	//rser.SerializeNode((cjNode**)&root);

	//delete rstream;

//	cjObjPackage* pkg = cjObj::cjObjCreatePkg(NULL, _CT("TestMeshEx"));
	cjTestMesh* mesh = cjObjLoadPkgObj<cjTestMesh>(NULL, _CT("TestMeshEx.sceneCube"));
		
		//cjObjNew<cjTestMesh>(cjTestMesh::cjObjGetStaticCls(), pkg, _CT("sceneCube"));

	cjUINT32 i=0;
	i=1;
	
}


static void cjCreateLODMeshAndAddScaneNode()
{
	g_pkg[0] = cjObj::cjObjCreatePkg(cjNULL, _CT("UT_PkgMeshSaveTest"));

	g_mesh = cjObjNew<cjLODMesh>(cjLODMesh::cjObjGetStaticCls(), g_pkg[0], _CT("TestMesh"));
		

	cjObj::cjObjSavePackage(g_pkg[0], cjNULL, 0, _CT("cjp/mdl/UT_PkgMeshSaveTest.cjp"));		
}

CJUT_IMPL_BEGIN( ,cjMain, PkgMeshSave)

	cjCreateLODMeshAndAddScaneNode();

CJUT_IMPL_END( , cjMain, PkgMeshSave)



