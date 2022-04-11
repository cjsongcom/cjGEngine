
#include "core/cjCore.h"
#include "engine/cjEngine.h"



CJUT_DECL( , cjMain, cjStrAryPropertyTest)
	

static cjObjPackage* g_pkg[100];


static void SaveStrAryProp()
{
	//cjObjPackage* pkg = cjObj::cjObjCreatePkg(NULL, _CT("TestStrAryProp"));

	//cjShaderRes* shRes = cjObjNew<cjShaderRes>(cjShaderRes::cjObjGetStaticCls(), pkg, _CT("testStrAry"));

	//shRes->m_before = cjName(_CT("beforename"));
	//
	//shRes->m_name.Push(_CT("aaa"));
	//shRes->m_name.Push(_CT("bbb"));
	//shRes->m_name.Push(_CT("ccc"));

	//shRes->m_after = cjName(_CT("aftername"));

	//shRes->m_testAry.Push(_CT("ddd"));

	//cjObj::cjObjSavePackage(pkg, NULL, 0, _CT("cjp/mdl/TestStrARyProp.cjp"));
}

static void LoadStrAryProp()
{
	cjShaderRes* shRes = cjObjLoadPkgObj<cjShaderRes>(cjNULL, _CT("TestStrARyProp.testStrAry"));

	/*if(shRes)
	{
		cjLOG(_CT("m_before = %s "), *shRes->m_before);

		for(cjAryOff i=0; i<shRes->m_name.Count(); i++)
			cjLOG(_CT("[cjShaderRes] %02d - %s"), i, *shRes->m_name(i));

		cjLOG(_CT("m_after = %s "), *shRes->m_after);

		for(cjAryOff i=0; i<shRes->m_testAry.Count(); i++)
			cjLOG(_CT("[cjShaderRes-testary] %02d - %s"), i, *shRes->m_testAry(i));

	}*/
}


CJUT_IMPL_BEGIN( ,cjMain, cjStrAryPropertyTest)

//	SaveStrAryProp();

	LoadStrAryProp();

CJUT_IMPL_END( , cjMain, cjStrAryPropertyTest)