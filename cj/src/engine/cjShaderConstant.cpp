
#include "cjEnginePCH.h"
#include "cjShaderConstant.h"


const char* cjShaderVtxAttrStr[cjEShaderVtxAttr_Max] =
{
    "a_position",
    "a_normal",
    "a_blendindices",
    "a_blendweight",
	"a_color",
	"a_binormal",
    "a_tangent",
    "a_texcoord0",
    "a_texcoord1",
    "a_texcoord2",
    "a_texcoord3",
    "a_texcoord4",
    "a_texcoord5",
    "a_texcoord6",
    "a_texcoord7"
};

cjEShaderVtxAttr cjConvStrToShaderVtxAttr(const char* str)
{
    if(!str)
        return cjEShaderVtxAttr_Unknown;

    for(cjINT32 i=0; i<cjEShaderVtxAttr_Max; i++)
    {
        if(!cjStricmp(cjShaderVtxAttrStr[i], str))
            return (cjEShaderVtxAttr)i;
    }
    
	cjLOG("Failed to resolve shader vertex attrib (%s)", str);

    return cjEShaderVtxAttr_Unknown;
}


