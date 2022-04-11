
#include "core/cjCore.h"
#include "engine/cjEngine.h"


CJUT_DECL( , cjMain, TextureTest)
	

static cjObjPackage* g_pkg[100];


static void SaveTestTexture()
{
	cjObjPackage* pkg = cjObj::cjObjCreatePkg(NULL, _CT("TestMeshTex"));

//	cjTexture* tex = cjObjImport<cjTexture>(pkg, _CT("texRGB"), "spr/baby32.tga");
	cjTexture* tex = cjObjImport<cjTexture>(cjNULL, _CT("TestMeshTex.texRGB"), "spr/baby32.tga");

//	cjTestMesh* mesh = cjObjNew<cjTestMesh>(cjTestMesh::cjObjGetStaticCls(), pkg, _CT("cube"));
// cjTestMesh::CreateTestStuffIndex(mesh);

	const cjNameChar* texPath = _CT("G:\\mobile\\HelloCJApp\\model\\SANDSS_32.tga");

//	tex->ReplaceTextureFromFile(texPath);

	//tex = cjObjImport<cjTexture>(pkg, _CT("texRGB"), "spr/SANDSS_32.tga");
			
	cjObj::cjObjSavePackage(pkg, NULL, 0, _CT("cjp/mdl/TestMeshTex.cjp"));
}

static void LoadTestTexture()
{
	cjTexture* tex = cjObjLoadPkgObj<cjTexture>(NULL, _CT("TestMeshPak.texRGB"));
//	cjTestMesh* mesh = cjObjLoadPkgObj<cjTestMesh>(NULL, _CT("TestMeshPak.cube"));
}


//{
//	cjLODMesh* lm = (cjLODMesh*)chr->m_model;
//
//	cjSceneMesh* m = lm->m_sceneMeshs(0);
//
//	cjScenePropRPtrList& prop = m->GetScenePropList();
//
//	cjScenePropRPtrList::Iter iter = prop.Begin();
//	cjScenePropRPtr curProp;
//
//	while(prop.GetNextV(iter, curProp))
//	{
//		cjSceneRenProp* renProp = (cjSceneRenProp*)(curProp._Ptr());
//
//		if(renProp->GetRenPropCate() == cjSceneRenProp::ERenProp_Textures)
//		{
//			cjSceneRenPropTextures* renPropTex = (cjSceneRenPropTextures*)renProp;
//
//			for(cjINT32 i=0; i<renPropTex->GetMapAry().Count(); i++)
//			{
//				cjSceneRenPropTextures::Tex* texMap 
//					= renPropTex->GetMap((cjSceneRenPropTextures::ETexType)i);
//
//				if(texMap)
//				{
//					cjTexture* tex = texMap->GetTex();
//
//					cjRSurfaceFmt fmt = tex->GetFormat();
//				}
//			}
//		}		
//	}
//
//}

CJUT_IMPL_BEGIN( ,cjMain, TextureTest)

//	SaveTestTexture();

//	LoadTestTexture()

CJUT_IMPL_END( , cjMain, TextureTest)