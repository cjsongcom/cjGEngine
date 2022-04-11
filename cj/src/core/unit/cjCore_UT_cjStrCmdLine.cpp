
#include "cjCorePCH.h"
#include "core/cjTList.h"


CJCORE_API cjUINT32 CJUT_TEST_cjStrCmdLine()
{
	const cjNameChar* cmdText = _CT("start -name walk01 -ar none -at none -loop");

	cjStrCmdLine testCmd(cmdText);
	
	cjNameChar sequenceName[cjMAX_PATH] = {0};
	testCmd.GetStr(_CT("name"), sequenceName, cjMAX_PATH);


	return 0;
}

