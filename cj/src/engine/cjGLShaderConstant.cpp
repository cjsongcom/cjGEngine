
#include "cjEnginePCH.h"

#include "cjGLRenDev.h"
#include "cjGLShaderConstant.h"


//
// GLSL VertexAttrib
//

cjGLVertexAttribInfo::cjGLVertexAttribInfo(cjGLRenDev* renDev, GLint prog)
{
    Generate(renDev, prog);
}

void cjGLVertexAttribInfo::Generate(cjGLRenDev* renDev, GLint prog)
{
    // Reset old stuff
    if(!m_attrs.IsEmpty())
    {
        m_attrs.RemoveAll();
        m_attrLocation.RemoveAll();
    }
	
	// attrib
    cjINT32 numAttribute = 0;
    renDev->glGetProgramiv(prog, GL_ACTIVE_ATTRIBUTES, (GLint*)&numAttribute);
	    
    for(cjINT32 i=0; i<numAttribute; i++)
    {
        // returned value
        GLsizei nameLen = -1;
        GLint aryNum = -1;
        GLenum type = GL_ZERO;
        
        GLchar name[cjEShaderAttributeNameLen_MAX];
        
        renDev->glGetActiveAttrib(prog,
            (GLuint)i,
            (GLsizei)cjEShaderAttributeNameLen_MAX-2,
            (GLsizei*)&nameLen,
            (GLint*)&aryNum,
            (GLenum*)&type,
            (GLchar*)name);
        
        name[cjEShaderAttributeNameLen_MAX-1] = 0;
        
        cjEShaderVtxAttr convAttr = cjConvStrToShaderVtxAttr(name);
        if(convAttr == cjEShaderVtxAttr_Unknown)
        {
            cjLOG("Warn! - Unknown vertex attribute is founded (%s)", name);
            continue;
        }
        
        m_attrs.Push(convAttr);
        
        GLint loc = renDev->glGetAttribLocation(prog, name);
        m_attrLocation.Push(loc);
    }

}

GLint cjGLVertexAttribInfo::GetLocation(cjEShaderVtxAttr attr) const
{
    for(cjAryOff i=0; i<m_attrs.Num(); i++)
    {
        if(m_attrs(i) == attr)
            return m_attrLocation(i);
    }
    
    return -1;
}


//
// GLSL Uniform
//

GLenum ConvertToGLUniformValType(cjEShaderUValType type)
{
    const static GLenum rstGLtype[] =
    {
        GL_FLOAT,           // cjEShaderUValType_FLOAT1
        GL_FLOAT_VEC2,
        GL_FLOAT_VEC3,
        GL_FLOAT_VEC4,
        
        GL_INT,             // cjEShaderUValType_INT1
        GL_INT_VEC2,
        GL_INT_VEC3,
        GL_INT_VEC4,
        
        GL_BOOL,            // cjEShaderUValType_BOOL1
        GL_BOOL_VEC2,
        GL_BOOL_VEC3,
        GL_BOOL_VEC4,
        
        GL_FLOAT_MAT2,      // cjEShaderUValType_MAT2
        GL_FLOAT_MAT3,
        GL_FLOAT_MAT4,
        
        GL_SAMPLER_2D,      // cjEShaderUValType_SAMPLER_2D
        GL_SAMPLER_CUBE
    };
    
    const static cjINT32 numGLType = sizeof(rstGLtype) / sizeof(rstGLtype[0]);
    
    if(numGLType <= type)
    {
        cjError("ERR! - Bad parameter..");
        return GL_ZERO;
    }
    
    return rstGLtype[type];
}

cjEShaderUValType ConvertTocjShaderUValType(GLenum fromGLType)
{
    switch(fromGLType)
    {
    case GL_FLOAT       : return cjEShaderUValType_FLOAT1;
    case GL_FLOAT_VEC2  : return cjEShaderUValType_FLOAT2;
    case GL_FLOAT_VEC3  : return cjEShaderUValType_FLOAT3;
    case GL_FLOAT_VEC4  : return cjEShaderUValType_FLOAT4;
    case GL_INT         : return cjEShaderUValType_INT1;
    case GL_INT_VEC2    : return cjEShaderUValType_INT2;
    case GL_INT_VEC3    : return cjEShaderUValType_INT3;
    case GL_INT_VEC4    : return cjEShaderUValType_INT4;
    case GL_BOOL        : return cjEShaderUValType_BOOL1;
    case GL_BOOL_VEC2   : return cjEShaderUValType_BOOL2;
    case GL_BOOL_VEC3   : return cjEShaderUValType_BOOL3;
    case GL_BOOL_VEC4   : return cjEShaderUValType_BOOL4;
    case GL_FLOAT_MAT2  : return cjEShaderUValType_MAT2;
    case GL_FLOAT_MAT3  : return cjEShaderUValType_MAT3;
    case GL_FLOAT_MAT4  : return cjEShaderUValType_MAT4;
    case GL_SAMPLER_2D  : return cjEShaderUValType_SAMPLER_2D;
    case GL_SAMPLER_CUBE: return cjEShaderUValType_SAMPLER_CUBE;
    default:
        break;
    }
    
    cjAssert(!"ERR! - Bad parameter..");
    
    return cjEShaderUValType_Unknown;
}


//
// Alloc
//

static void _PFN_ALLOC_FLOAT1(void* valSiz, GLint , GLint aryNum, void* val)
{
    GLfloat** v = ((GLfloat**)val); *v = new GLfloat[aryNum];  *((cjINT32*)valSiz) = sizeof(GLfloat);
}

static void _PFN_ALLOC_FLOAT2(void* valSiz, GLint , GLint aryNum, void* val)
{
    GLfloat** v = ((GLfloat**)val); *v = new GLfloat[2*aryNum];  *((cjINT32*)valSiz) = sizeof(GLfloat)*2;
}

static void _PFN_ALLOC_FLOAT3(void* valSiz, GLint , GLint aryNum, void* val)
{
    GLfloat** v = ((GLfloat**)val); *v = new GLfloat[3*aryNum];  *((cjINT32*)valSiz) = sizeof(GLfloat)*3;
}

static void _PFN_ALLOC_FLOAT4(void* valSiz, GLint , GLint aryNum, void* val)
{
    GLfloat** v = ((GLfloat**)val); *v = new GLfloat[4*aryNum];  *((cjINT32*)valSiz) = sizeof(GLfloat)*4;
}

static void _PFN_ALLOC_INT1(void* valSiz, GLint , GLint aryNum, void* val)
{
    GLint** v = ((GLint**)val); *v = new GLint[aryNum]; *((cjINT32*)valSiz) = sizeof(GLint);
}

static void _PFN_ALLOC_INT2(void* valSiz, GLint , GLint aryNum, void* val)
{
    GLint** v = ((GLint**)val); *v = new GLint[2*aryNum]; *((cjINT32*)valSiz) = sizeof(GLint)*2;
}

static void _PFN_ALLOC_INT3(void* valSiz, GLint , GLint aryNum, void* val)
{
    GLint** v = ((GLint**)val); *v = new GLint[3*aryNum]; *((cjINT32*)valSiz) = sizeof(GLint)*3;
}

static void _PFN_ALLOC_INT4(void* valSiz, GLint , GLint aryNum, void* val)
{
    GLint** v = ((GLint**)val); *v = new GLint[4*aryNum]; *((cjINT32*)valSiz) = sizeof(GLint)*4;
}

/*
Either the i, ui or f variants may be used to provide values for uniform variables of type 
bool​, bvec2​, bvec3​, bvec4​, or arrays of these. The uniform variable will be set to false​ 
if the input value is 0 or 0.0f, and it will be set to true​ otherwise. */

static void _PFN_ALLOC_MAT2(void* valSiz, GLint , GLint aryNum, void* val)
{
    GLfloat** v = ((GLfloat**)val); *v = new GLfloat[2*2*aryNum]; *((cjINT32*)valSiz) = sizeof(GLfloat)*2*2;
}

static void _PFN_ALLOC_MAT3(void* valSiz, GLint , GLint aryNum, void* val)
{
    GLfloat** v = ((GLfloat**)val); *v = new GLfloat[3*3*aryNum]; *((cjINT32*)valSiz) = sizeof(GLfloat)*3*3;
}

static void _PFN_ALLOC_MAT4(void* valSiz, GLint , GLint aryNum, void* val)
{
    GLfloat** v = ((GLfloat**)val); *v = new GLfloat[4*4*aryNum]; *((cjINT32*)valSiz) = sizeof(GLfloat)*4*4;
}

static void _PFN_ALLOC_SAMPLER_2D(void* valSiz, GLint , GLint aryNum, void* val)
{
   GLint** v = ((GLint**)val); *v = new GLint[aryNum]; *((cjINT32*)valSiz) = sizeof(GLint);
   **v = 0;
}

static void _PFN_ALLOC_SAMPLER_CUBE(void* valSiz, GLint , GLint aryNum, void* val)
{
   GLint** v = ((GLint**)val); *v = new GLint[aryNum]; *((cjINT32*)valSiz) = sizeof(GLint);
   **v = 0;
}


//
// DeAlloc
//

static void _PFN_DEALLOC_FLOAT1(void* , GLint , GLint , void* val)
{
    GLfloat** v = ((GLfloat**)val); delete [] (*v);
}

static void _PFN_DEALLOC_FLOAT2(void* , GLint , GLint , void* val)
{
    GLfloat** v = ((GLfloat**)val); delete [] (*v);
}

static void _PFN_DEALLOC_FLOAT3(void* , GLint , GLint , void* val)
{
    GLfloat** v = ((GLfloat**)val); delete [] (*v);

}

static void _PFN_DEALLOC_FLOAT4(void* , GLint , GLint , void* val)
{
    GLfloat** v = ((GLfloat**)val); delete [] (*v);
}

static void _PFN_DEALLOC_INT1(void* , GLint , GLint , void* val)
{
    GLint** v = ((GLint**)val); delete [] (*v);
}

static void _PFN_DEALLOC_INT2(void* , GLint , GLint , void* val)
{
    GLint** v = ((GLint**)val); delete [] (*v);
}

static void _PFN_DEALLOC_INT3(void* , GLint , GLint , void* val)
{
    GLint** v = ((GLint**)val); delete [] (*v);
}

static void _PFN_DEALLOC_INT4(void* , GLint , GLint , void* val)
{
    GLint** v = ((GLint**)val); delete [] (*v);
}

/*
Either the i, ui or f variants may be used to provide values for uniform variables of type 
bool​, bvec2​, bvec3​, bvec4​, or arrays of these. The uniform variable will be set to false​ 
if the input value is 0 or 0.0f, and it will be set to true​ otherwise. */

static void _PFN_DEALLOC_MAT2(void* , GLint , GLint , void* val)
{
    GLfloat** v = ((GLfloat**)val); delete [] (*v);
}

static void _PFN_DEALLOC_MAT3(void* , GLint , GLint , void* val)
{
    GLfloat** v = ((GLfloat**)val); delete [] (*v);
}

static void _PFN_DEALLOC_MAT4(void* , GLint , GLint , void* val)
{
    GLfloat** v = ((GLfloat**)val); delete [] (*v);
}

static void _PFN_DEALLOC_SAMPLER_2D(void* , GLint , GLint , void* val)
{
   GLint** v = ((GLint**)val); delete [] (*v);
}

static void _PFN_DEALLOC_SAMPLER_CUBE(void* , GLint , GLint , void* val)
{
   GLint** v = ((GLint**)val); delete [] (*v);
}


//
// Write Operation
//
static void _PFN_WRTOSHADER_FLOAT1(cjGLRenDev* renDev, GLint loc, GLint aryNum, void* val)
{
   renDev->glUniform1fv(loc, aryNum, (GLfloat*)val);
}

static void _PFN_WRTOSHADER_FLOAT2(cjGLRenDev* renDev, GLint loc, GLint aryNum, void* val)
{
   renDev->glUniform2fv(loc, aryNum, (GLfloat*)val);
}

static void _PFN_WRTOSHADER_FLOAT3(cjGLRenDev* renDev, GLint loc, GLint aryNum, void* val)
{
   renDev->glUniform3fv(loc, aryNum, (GLfloat*)val);
}

static void _PFN_WRTOSHADER_FLOAT4(cjGLRenDev* renDev, GLint loc, GLint aryNum, void* val)
{
   renDev->glUniform4fv(loc, aryNum, (GLfloat*)val);
}

static void _PFN_WRTOSHADER_INT1(cjGLRenDev* renDev, GLint loc, GLint aryNum, void* val)
{
   renDev->glUniform1iv(loc, aryNum, (GLint*)val);
}

static void _PFN_WRTOSHADER_INT2(cjGLRenDev* renDev, GLint loc, GLint aryNum, void* val)
{
   renDev->glUniform2iv(loc, aryNum, (GLint*)val);
}

static void _PFN_WRTOSHADER_INT3(cjGLRenDev* renDev, GLint loc, GLint aryNum, void* val)
{
   renDev->glUniform3iv(loc, aryNum, (GLint*)val);
}

static void _PFN_WRTOSHADER_INT4(cjGLRenDev* renDev, GLint loc, GLint aryNum, void* val)
{
   renDev->glUniform4iv(loc, aryNum, (GLint*)val);
}

static void _PFN_WRTOSHADER_MAT2(cjGLRenDev* renDev, GLint loc, GLint aryNum, void* val)
{
	// OpenGL ES 2.0 does not support transpose. so must be GL_FALSE
   renDev->glUniformMatrix2fv(loc, aryNum, GL_FALSE, (GLfloat*)val);
}

static void _PFN_WRTOSHADER_MAT3(cjGLRenDev* renDev, GLint loc, GLint aryNum, void* val)
{
	// OpenGL ES 2.0 does not support transpose. so must be GL_FALSE
   renDev->glUniformMatrix3fv(loc, aryNum, GL_FALSE, (GLfloat*)val);
}

//static void _PFN_WRTOSHADER_MAT3x4(cjGLRenDev* renDev, GLint loc, GLint aryNum, void* val)
//{
//   renDev->glUniformMatrix3x4fv(loc, aryNum, cjGLSL_MAT4_ROW_MAJOR, (GLfloat*)val);
//}

static void _PFN_WRTOSHADER_MAT4(cjGLRenDev* renDev, GLint loc, GLint aryNum, void* val)
{
	// GL_FALSE for transpose
	// if it is GL_TRUE, each matrix is assumed to be supplied in row major order	
	// OpenGL ES 2.0 does not support transpose. so must be GL_FALSE
	renDev->glUniformMatrix4fv(loc, aryNum, GL_FALSE, (GLfloat*)val);
}

static void _PFN_WRTOSHADER_SAMPLER_2D(cjGLRenDev* renDev, GLint loc, GLint , void* val)
{
   renDev->glUniform1i(loc, *((GLint*)val));
}

static void _PFN_WRTOSHADER_SAMPLER_CUBE(cjGLRenDev* renDev, GLint loc, GLint , void* val)
{
   renDev->glUniform1i(loc, *((GLint*)val));
}


const cjGLShaderValWriteOperPFN cjGLShaderUniformVal
    ::ms_operatorStore[cjGLShaderUniformVal::EOPER_MAX][cjEShaderUValType_MAX] =
{
    { // EOPER_ALLOC
        _PFN_ALLOC_FLOAT1,
        _PFN_ALLOC_FLOAT2,
        _PFN_ALLOC_FLOAT3,
        _PFN_ALLOC_FLOAT4,
    
        _PFN_ALLOC_INT1,
        _PFN_ALLOC_INT2,
        _PFN_ALLOC_INT3,
        _PFN_ALLOC_INT4,

        _PFN_ALLOC_INT1,    // GL_Bool1
        _PFN_ALLOC_INT2,
        _PFN_ALLOC_INT3,
        _PFN_ALLOC_INT4,

        _PFN_ALLOC_MAT2,
        _PFN_ALLOC_MAT3,
        _PFN_ALLOC_MAT4,

        _PFN_ALLOC_SAMPLER_2D,
        _PFN_ALLOC_SAMPLER_CUBE,
    },
    
    { // EOPER_DEALLOC
        _PFN_DEALLOC_FLOAT1,
        _PFN_DEALLOC_FLOAT2,
        _PFN_DEALLOC_FLOAT3,
        _PFN_DEALLOC_FLOAT4,
    
        _PFN_DEALLOC_INT1,
        _PFN_DEALLOC_INT2,
        _PFN_DEALLOC_INT3,
        _PFN_DEALLOC_INT4,

        _PFN_DEALLOC_INT1,      // GL_Bool
        _PFN_DEALLOC_INT2,
        _PFN_DEALLOC_INT3,
        _PFN_DEALLOC_INT4,

        _PFN_DEALLOC_MAT2,
        _PFN_DEALLOC_MAT3,
        _PFN_DEALLOC_MAT4,

        _PFN_DEALLOC_SAMPLER_2D,
        _PFN_DEALLOC_SAMPLER_CUBE,
    },

    { // EOPER_COMMIT_TO_SHADER
        (cjGLShaderValWriteOperPFN)_PFN_WRTOSHADER_FLOAT1,
        (cjGLShaderValWriteOperPFN)_PFN_WRTOSHADER_FLOAT2,
        (cjGLShaderValWriteOperPFN)_PFN_WRTOSHADER_FLOAT3,
        (cjGLShaderValWriteOperPFN)_PFN_WRTOSHADER_FLOAT4,
    
        (cjGLShaderValWriteOperPFN)_PFN_WRTOSHADER_INT1,
        (cjGLShaderValWriteOperPFN)_PFN_WRTOSHADER_INT2,
        (cjGLShaderValWriteOperPFN)_PFN_WRTOSHADER_INT3,
        (cjGLShaderValWriteOperPFN)_PFN_WRTOSHADER_INT4,

        (cjGLShaderValWriteOperPFN)_PFN_WRTOSHADER_INT1,   // GL_Bool
        (cjGLShaderValWriteOperPFN)_PFN_WRTOSHADER_INT2,
        (cjGLShaderValWriteOperPFN)_PFN_WRTOSHADER_INT3,
        (cjGLShaderValWriteOperPFN)_PFN_WRTOSHADER_INT4,   // GL_Bool4

        (cjGLShaderValWriteOperPFN)_PFN_WRTOSHADER_MAT2,
        (cjGLShaderValWriteOperPFN)_PFN_WRTOSHADER_MAT3,
        (cjGLShaderValWriteOperPFN)_PFN_WRTOSHADER_MAT4,

        (cjGLShaderValWriteOperPFN)_PFN_WRTOSHADER_SAMPLER_2D,
        (cjGLShaderValWriteOperPFN)_PFN_WRTOSHADER_SAMPLER_CUBE,
    }
};


//
// cjGLShaderUniformVal
//
/*
    GL_FLOAT, GL_FLOAT_VEC2, GL_FLOAT_VEC3, GL_FLOAT_VEC4, GL_INT, GL_INT_VEC2, GL_INT_VEC3, GL_INT_VEC4, GL_BOOL, GL_BOOL_VEC2, GL_BOOL_VEC3, GL_BOOL_VEC4, GL_FLOAT_MAT2, GL_FLOAT_MAT3, GL_FLOAT_MAT4, GL_SAMPLER_2D, or GL_SAMPLER_CUBE
*/


cjGLShaderUniformVal::cjGLShaderUniformVal(GLuint loc, GLenum type, GLint aryNum, const char* name)
{
    m_loc = loc;
    m_aryNum = aryNum;
	m_curAryNum = 0;

    m_type = cjEShaderUValType_Unknown;
    m_val = NULL;	
    m_next = NULL;
    
    if(!name)
        return;
    
    cjStrcpy(m_name, name);
    
    if(!ResolveOperator(type, aryNum))
    {
        cjLOG("Warn!- failed to resolve type - name=%s ", name);
        return;
    }
    
    m_operator[EOPER_ALLOC]((void*)&m_valSiz, 0, m_aryNum, (void*)&m_val);
}

cjGLShaderUniformVal::~cjGLShaderUniformVal()
{
    if(m_val)
    {
        m_operator[EOPER_DEALLOC](NULL, 0, m_aryNum, (void*)&m_val);
        m_val = NULL;
    }
}

void cjGLShaderUniformVal::CommitToShader(cjGLRenDev* renDev)
{
	if(m_curAryNum)
		m_operator[EOPER_COMMIT_TO_SHADER](renDev, m_loc, m_curAryNum, m_val);
}

void cjGLShaderUniformVal::SetValue(void* value, cjINT32 valSiz, cjINT32 aryNum)
{
    if(m_valSiz != valSiz || m_aryNum < aryNum)
        return;
    
	m_curAryNum = aryNum;

	if(m_curAryNum)
		cjMemCpy(m_val, value, m_valSiz * aryNum);
}

cjBool cjGLShaderUniformVal::ResolveOperator(GLenum type, GLint arySiz)
{
    m_type = ConvertTocjShaderUValType(type);
    
    if(m_type == cjEShaderUValType_Unknown)
        return cjFalse;
    
    m_operator[EOPER_ALLOC]            = ms_operatorStore[EOPER_ALLOC][m_type];
    m_operator[EOPER_DEALLOC]          = ms_operatorStore[EOPER_DEALLOC][m_type];
    m_operator[EOPER_COMMIT_TO_SHADER] = ms_operatorStore[EOPER_COMMIT_TO_SHADER][m_type];

    return cjTrue;
}


//
// cjGLShaderUniformValSet
//

cjGLShaderUniformValSet::cjGLShaderUniformValSet()
{
    m_begin = NULL;
    m_numValue = 0;
}

cjGLShaderUniformValSet::~cjGLShaderUniformValSet()
{
    Reset();
}

cjINT32 cjGLShaderUniformValSet::Generate(cjGLRenDev* renDev, GLuint prog)
{
    Reset();
    
    cjINT32 numUniform = 0;
    
	// all the uniforms from vertex and pixel shader
    renDev->glGetProgramiv(prog, GL_ACTIVE_UNIFORMS, (GLint*)&numUniform);
    
    for(cjINT32 i=0; i<numUniform; i++)
    {
        // returned value
        GLsizei uvalNameLen = -1;
        GLint uvalAryNum = -1;
        GLenum uvalType = GL_ZERO;
        
        GLchar uvarName[cjEShaderUniformNameLen_MAX];
        
        renDev->glGetActiveUniform(prog, (GLuint)i, (GLsizei)cjEShaderUniformNameLen_MAX-2, (GLsizei*)&uvalNameLen, (GLint*)&uvalAryNum, (GLenum*)&uvalType, (GLchar*)uvarName);
        
		// uniform mat4 blendMatrix[blendMatCount]; => blendMatrix[0];
        uvarName[cjEShaderUniformNameLen_MAX-1] = 0;
		cjChar* aryPrefix = cjStrchr(uvarName, '[');
		if(aryPrefix)
			*aryPrefix = 0;

        GLuint loc = renDev->glGetUniformLocation(prog, uvarName);
        
        Add(loc, uvalType, uvalAryNum, uvarName);
    }
    
    m_numValue = numUniform;
        
    return numUniform;
}

cjBool cjGLShaderUniformValSet::Add(GLuint loc, GLenum type, GLint aryNum, const char* name)
{
    if(!name)
        return cjFalse;
    
    cjGLShaderUniformVal* val = new cjGLShaderUniformVal(loc, type, aryNum, name);
    if(m_begin)
        val->m_next = m_begin;
    
    m_begin = val;
    
    return cjTrue;
}

//cjINT32 cjGLShaderUniformValSet::GetNumValue()
//{
////    cjGLShaderUniformVal* cur = m_begin;
////    cjINT32 num = 0;
////    
////    while(cur)
////    {
////        num++;
////        cur = cur->m_next;
////    }
////    
////    return num;
////
//}

void cjGLShaderUniformValSet::Reset()
{
    cjGLShaderUniformVal* cur = m_begin;
    
    while(cur)
    {
        cjGLShaderUniformVal* toDel = cur;
        cur = cur->m_next;

        delete toDel;
    }
    
    m_begin = NULL;
    m_numValue = 0;
}

cjGLShaderUniformVal* cjGLShaderUniformValSet::FindVal(const char* uniformName)
{
	//cjopti
    if(!uniformName)
        return NULL;
    
    cjGLShaderUniformVal* cur = Begin();
    while(cur)
    {
        if(!cjStricmp(cur->m_name, uniformName))
            return cur;
    
        cur = cur->m_next;
    }
    
    return NULL;
}

void cjGLShaderUniformValSet::SetValue(const char* uniformName, cjMatrix* mat4, cjINT32 aryNum)
{
    cjGLShaderUniformVal* val = FindVal(uniformName);
    if(!val) return;

    val->SetValue(mat4, sizeof(cjMatrix), aryNum);
}

void cjGLShaderUniformValSet::SetValue(const char* uniformName, cjMatrix3* mat3, cjINT32 aryNum)
{
    cjGLShaderUniformVal* val = FindVal(uniformName);
    if(!val) return;

    val->SetValue(mat3, sizeof(cjMatrix3), aryNum);
}

void cjGLShaderUniformValSet::SetValue(const char* uniformName, cjVec4* vec4, cjINT32 aryNum)
{
    cjGLShaderUniformVal* val = FindVal(uniformName);
    if(!val) return;

    val->SetValue(vec4, sizeof(cjVec4), aryNum);
}

void cjGLShaderUniformValSet::CommitToShader(cjGLRenDev *renDev)
{
    cjGLShaderUniformVal* cur = Begin();
    
    while(cur)
    {
        cur->CommitToShader(renDev);
        cur = cur->m_next;
    }
}
