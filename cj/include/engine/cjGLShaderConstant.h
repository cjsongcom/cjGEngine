
#ifndef _CJGLSHADERCONSTANT_H_
#define _CJGLSHADERCONSTANT_H_


#include "cjShaderConstant.h"

//
//http://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/attributes.php
//
//A custom, user-defined attribute can also be defined. The OpenGL function glBindAttribLocation associates the name of the variable with an index.
//For example, glBindAttribLocation(ProgramObject, 10, "myAttrib") would bind the attribute "myAttrib" to index 10.
//The maximum number of attribute locations is limited by the graphics hardware. You can retrieve the maximum supported number of vertex attributes with glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &n).
//Setting attribute values can be done using glVertexAttrib function.
//Unfortunately there are certain limitations when using this on NVidia Hardware. According to NVidia:
//"GLSL attempts to eliminate aliasing of vertex attributes but this is integral to NVIDIA’s hardware approach and necessary for maintaining compatibility with existing OpenGL applications that NVIDIA customers rely on. NVIDIA’s GLSL implementation therefore does not allow built-in vertex attributes to collide with a generic vertex attributes that is assigned to a particular vertex attribute index with glBindAttribLocation. For example, you should not use gl_Normal (a built-in vertex attribute) and also use glBindAttribLocation to bind a generic vertex attribute named "whatever" to vertex attribute index 2 because gl_Normal aliases to index 2."
//In other words, NVidia hardware indices are reserved for built-in attributes:
//gl_Vertex	0
//gl_Normal	2
//gl_Color	3
//gl_SecondaryColor	4
//gl_FogCoord	5
//gl_MultiTexCoord0	8
//gl_MultiTexCoord1	9
//gl_MultiTexCoord2	10
//gl_MultiTexCoord3	11
//gl_MultiTexCoord4	12
//gl_MultiTexCoord5	13
//gl_MultiTexCoord6	14
//gl_MultiTexCoord7	15
//



enum ESHADER_VAL_SEMANTIC
{
    ESHDR_VAL_SEM_MAT4_WORLD,
    ESHDR_VAL_SEM_MAT4_VIEW,
    ESHDR_VAL_SEM_MAT4_PROJECTION,
    
    ESHDR_VAL_SEM_SAMPLER_0,
};

class cjGLRenDev;


//
// GLSL VertexAttribute
//

class cjGLVertexAttribInfo : public cjMem
{
public:
    cjGLVertexAttribInfo(cjGLRenDev* renDev, GLint prog);
    void Generate(cjGLRenDev* renDev, GLint prog);
    
    cjINT32 GetNumAttrib() const { return m_attrs.Num(); }
    
    void GetInfo(cjINT32 index, cjEShaderVtxAttr* attr, GLint* location) const
        {
            *attr = m_attrs(index);
            *location = m_attrLocation(index);
        }
    
    GLint GetLocation(cjEShaderVtxAttr attr) const;
   
private:
    cjTAry<cjEShaderVtxAttr>              m_attrs;
    cjTAry<GLint>                         m_attrLocation;
};


//
// GLSL Uniform
//

typedef void (*cjGLShaderValWriteOperPFN)(void* , GLint , GLint, void* );

GLenum ConvertToGLUniformValType(cjEShaderUValType type);
cjEShaderUValType ConvertTocjShaderUValType(GLenum fromGLType);


class cjGLShaderUniformVal : public cjMem
{
    friend class cjGLShaderUniformValSet;
public:
    enum EOPER_VAL
    {
        EOPER_ALLOC,
        EOPER_DEALLOC,
        EOPER_COMMIT_TO_SHADER,
        EOPER_MAX
    };

    cjGLShaderUniformVal(GLuint loc, GLenum type, GLint aryNum, const char* name);
    ~cjGLShaderUniformVal();
    
    void CommitToShader(cjGLRenDev* renDev);
    void SetValue(void* value, cjINT32 valSiz, cjINT32 aryNum=1);
    
private:
    cjBool ResolveOperator(GLenum type, GLint arySiz);

    cjINT32                                         m_loc;
    cjEShaderUValType                               m_type;
    
    cjINT32                                         m_valSiz;
    cjINT32                                         m_aryNum;
	cjINT32											m_curAryNum; // by SetValue
	    
    char                                            m_name[cjEShaderUniformNameLen_MAX];
    
    void*                                           m_val;	
    cjGLShaderValWriteOperPFN                       m_operator[EOPER_MAX];

    cjGLShaderUniformVal*                           m_next;
    
    const static cjGLShaderValWriteOperPFN          ms_operatorStore[EOPER_MAX][cjEShaderUValType_MAX];
};


class cjGLShaderUniformValSet : public cjMem
{
public:
    cjGLShaderUniformValSet();
    ~cjGLShaderUniformValSet();
    
    cjINT32 Generate(cjGLRenDev* renDev, GLuint prog);
    
    cjGLShaderUniformVal* Begin() { return m_begin; }
    cjINT32 GetNumValue() { return m_numValue; }
    
    void SetValue(const char* uniformName, cjMatrix* mat4, cjINT32 aryNum=1);
    void SetValue(const char* uniformName, cjMatrix3* mat3, cjINT32 aryNum=1);
	void SetValue(const char* uniformName, cjVec4* vec4, cjINT32 aryNum=1);

    void CommitToShader(cjGLRenDev* renDev);
    
    cjGLShaderUniformVal* FindVal(const char* uniformName);
    
protected:
    cjBool Add(GLuint loc, GLenum type, GLint aryNum, const char* name);
    void Reset();

private:
    cjGLShaderUniformVal*							m_begin;
    cjINT32											m_numValue;
};


#endif
