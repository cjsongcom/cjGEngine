
#include "core/cjCore.h"
#include "engine/cjEngine.h"


#include "core/w32/cjToolClassDesc.h"


CJUT_DECL( , cjMain, Tool_NodeInspector)


class cjUnitTestCtx_Tool_NodeInspector : public cjUnitTestCtx
{
public:
	cjToolClassDesc*			m_rstToolClsDesc;
};


	
HMODULE gs_sceneModule;
cjToolLibInitPFN				gs_initPFN;
cjToolLibShutdownPFN			gs_shutdownPFN;
cjToolLibCreateClassDescPFN		gs_createClassDescPFN;
cjToolClassDesc*				gs_toolClass;


void LibraryLoadTest()
{
	const char* path = "G:\\mobile\\HelloCJApp\\HelloCJApp.win32\\bin\\libcjNodeInspectorD.dll";
	gs_sceneModule = LoadLibraryExA(path, 0, LOAD_WITH_ALTERED_SEARCH_PATH);
	
	if(!gs_sceneModule)
	{
	}
	else
	{
		if(cjToolClassDesc::ResolveFuncFromDLL(gs_sceneModule, &gs_initPFN, &gs_shutdownPFN, 
			&gs_createClassDescPFN))
		{			
			gs_initPFN();
			gs_toolClass = (cjToolClassDesc*)gs_createClassDescPFN(0);

			gs_toolClass->Init();
		}
	}
}

void LibraryLoadTestEnd()
{
	if(gs_sceneModule)
	{
		gs_shutdownPFN();
		FreeLibrary(gs_sceneModule);
		gs_sceneModule = 0;
	}
}


CJUT_IMPL_BEGIN( ,cjMain, Tool_NodeInspector)

	LibraryLoadTest();

	if(gs_toolClass)
	{
//		gs_toolClass->DoCmd("show", 0);
//		gs_toolClass->DoCmd("update", gs_mesh->GetSceneRoot());
	}

	_ctx->m_rstToolClsDesc = gs_toolClass;
	
CJUT_IMPL_END( , cjMain, Tool_NodeInspector)

