
#ifndef _CJSHADERCONSTANT_H_
#define _CJSHADERCONSTANT_H_


enum
{
    cjEShaderAttributeNameLen_MAX = 32,
    cjEShaderUniformNameLen_MAX = 32
};

enum cjEShaderVtxAttr
{
    cjEShaderVtxAttr_Pos,
    cjEShaderVtxAttr_Normal,
    cjEShaderVtxAttr_BlendIndices,
    cjEShaderVtxAttr_BlendWeight,
	cjEShaderVtxAttr_Color,
    cjEShaderVtxAttr_BiNormal,
    cjEShaderVtxAttr_Tangent,
    cjEShaderVtxAttr_TexCoord0,
    cjEShaderVtxAttr_TexCoord1,
    cjEShaderVtxAttr_TexCoord2,
    cjEShaderVtxAttr_TexCoord3,
    cjEShaderVtxAttr_TexCoord4,
    cjEShaderVtxAttr_TexCoord5,
    cjEShaderVtxAttr_TexCoord6,
    cjEShaderVtxAttr_TexCoord7,
    cjEShaderVtxAttr_Max,
    
    cjEShaderVtxAttr_Unknown = -1
};

cjEShaderVtxAttr cjConvStrToShaderVtxAttr(const char* str);
extern const char* cjShaderVtxAttrStr[cjEShaderVtxAttr_Max];


// shader uniform value
enum cjEShaderUValType
{
    cjEShaderUValType_FLOAT1,
    cjEShaderUValType_FLOAT2,
    cjEShaderUValType_FLOAT3,
    cjEShaderUValType_FLOAT4,

    cjEShaderUValType_INT1,
    cjEShaderUValType_INT2,
    cjEShaderUValType_INT3,
    cjEShaderUValType_INT4,

    cjEShaderUValType_BOOL1,
    cjEShaderUValType_BOOL2,
    cjEShaderUValType_BOOL3,
    cjEShaderUValType_BOOL4,
    
    cjEShaderUValType_MAT2,  // 2x2
    cjEShaderUValType_MAT3,  // 3x3
    cjEShaderUValType_MAT4,  // 4x4
    
    cjEShaderUValType_SAMPLER_2D,
    cjEShaderUValType_SAMPLER_CUBE,
    
    cjEShaderUValType_MAX,
    
    cjEShaderUValType_Unknown = -1,    
};


#endif
