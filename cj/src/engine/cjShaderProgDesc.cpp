
#include "cjEnginePCH.h"
#include "cjShaderProgDesc.h"


//
// cjDefaultShaderVtxProgDesc
//

void cjDefaultShaderVtxProgDesc::SetTexStageUVSetIndexAndOutType(
	cjUINT32 texStage, cjUINT32 uvSetIndex, cjShaderMtlDefault::TEXUVOUT_TYPE uvOutType)
{
    switch(texStage)
    {
    case 0:	SetUV0_SRC_INDEX(uvSetIndex); SetUV0OUT_TYPE(uvOutType); break;
    case 1: SetUV1_SRC_INDEX(uvSetIndex); SetUV1OUT_TYPE(uvOutType); break;
    case 2: SetUV2_SRC_INDEX(uvSetIndex); SetUV2OUT_TYPE(uvOutType); break;
    case 3: SetUV3_SRC_INDEX(uvSetIndex); SetUV3OUT_TYPE(uvOutType); break;
    case 4: SetUV4_SRC_INDEX(uvSetIndex); SetUV4OUT_TYPE(uvOutType); break;
    case 5: SetUV5_SRC_INDEX(uvSetIndex); SetUV5OUT_TYPE(uvOutType); break;
    case 6: SetUV6_SRC_INDEX(uvSetIndex); SetUV6OUT_TYPE(uvOutType); break;
    case 7: SetUV7_SRC_INDEX(uvSetIndex); SetUV7OUT_TYPE(uvOutType); break;
    default:
		cjAssert(!"invlaid uvOut Index!");
		break;
    }
}


//
// cjDefaultShaderPxlProgDesc
//

// texcoordIndex 0:none, 1:texcoord0 ~ 7:tecoord6
void cjDefaultShaderPxlProgDesc::SetSamplerTexcoord(cjUINT32 samplerIndex, cjUINT32 texcoordIndex)
{
    switch(samplerIndex)
    {
    case 0:	SetSAMP0_TEXCOORD(texcoordIndex);  break; 
    case 1: SetSAMP1_TEXCOORD(texcoordIndex);  break;
    case 2: SetSAMP2_TEXCOORD(texcoordIndex);  break;
    case 3: SetSAMP3_TEXCOORD(texcoordIndex);  break;
    case 4: SetSAMP4_TEXCOORD(texcoordIndex);  break; 
    case 5: SetSAMP5_TEXCOORD(texcoordIndex);  break; 
    case 6: SetSAMP6_TEXCOORD(texcoordIndex);  break; 
    case 7: SetSAMP7_TEXCOORD(texcoordIndex);  break;  
    default: 
		cjAssert(!"invalid sampler index..!"); break;
    }
}