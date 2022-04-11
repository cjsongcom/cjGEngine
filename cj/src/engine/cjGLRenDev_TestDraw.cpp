
#include "cjEnginePCH.h"
#include "cjGLRenDev.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))



static void SetupGeometry(cjGLRenDev* renDev);
static void Cleanup(cjGLRenDev* renDev);
static void Update(cjGLRenDev* renDev);
static void Draw(cjGLRenDev* renDev);
static cjBool LoadShaders();
static cjBool CompileShader(cjGLRenDev* renDev, GLuint* shader, GLenum type, const char* shaderTextSrc);
static cjBool LinkProgram(cjGLRenDev* renDev, GLuint prog);
static cjBool ValidateProgram(cjGLRenDev* renDev, GLuint prog);


enum EVTXAttr
{
    EVTXAttrPosition,
	EVTXAttrBlendWeight,
	EVTXAttrBlendIndices,
    EVTXAttrNormal,
    EVTXAttrColor,
    EVTXAttrTexCoord0,
    EVTXAttrTexCoord1,
    EVTXAttrTexCoord2,
    EVTXAttrTexCoord3,
};


// Uniform index.
enum
{
    UNIFORM_MODELVIEWPROJECTION_MATRIX,
    UNIFORM_NORMAL_MATRIX,
    NUM_UNIFORMS
};

GLint uniforms[NUM_UNIFORMS];

// Attribute index.
enum
{
    ATTRIB_VERTEX,
    ATTRIB_NORMAL,
    NUM_ATTRIBUTES
};

GLfloat gCubeVertexData[] =
{
    // Data layout for each line below is:
    // positionX, positionY, positionZ,     normalX, normalY, normalZ,   texU, texV
    0.5f, -0.5f, -0.5f,        1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
    0.5f, 0.5f, -0.5f,         1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
    0.5f, -0.5f, 0.5f,         1.0f, 0.0f, 0.0f,    1.0f, 0.0f,    
    0.5f, -0.5f, 0.5f,         1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
    0.5f, 0.5f, -0.5f,         1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
    0.5f, 0.5f, 0.5f,          1.0f, 0.0f, 0.0f,    1.0f, 0.0f,
    
    0.5f, 0.5f, -0.5f,         0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
    -0.5f, 0.5f, -0.5f,        0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
    0.5f, 0.5f, 0.5f,          0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
    0.5f, 0.5f, 0.5f,          0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
    -0.5f, 0.5f, -0.5f,        0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
    -0.5f, 0.5f, 0.5f,         0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
    
    -0.5f, 0.5f, -0.5f,        -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,       -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
    -0.5f, 0.5f, 0.5f,         -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f,         -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,       -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f,        -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
    
    -0.5f, -0.5f, -0.5f,       0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
    0.5f, -0.5f, -0.5f,        0.0f, -1.0f, 0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f,        0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f,        0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
    0.5f, -0.5f, -0.5f,        0.0f, -1.0f, 0.0f,   0.0f, 1.0f,
    0.5f, -0.5f, 0.5f,         0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
    
    0.5f, 0.5f, 0.5f,          0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f,         0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
    0.5f, -0.5f, 0.5f,         0.0f, 0.0f, 1.0f,    1.0f, 0.0f,
    0.5f, -0.5f, 0.5f,         0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f,         0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f,        0.0f, 0.0f, 1.0f,    1.0f, 0.0f,
    
    0.5f, -0.5f, -0.5f,        0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,       0.0f, 0.0f, -1.0f,   0.0f, 1.0f,
    0.5f, 0.5f, -0.5f,         0.0f, 0.0f, -1.0f,   1.0f, 0.0f,
    0.5f, 0.5f, -0.5f,         0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,       0.0f, 0.0f, -1.0f,   0.0f, 1.0f,
    -0.5f, 0.5f, -0.5f,        0.0f, 0.0f, -1.0f,   1.0f, 0.0f
};


GLuint      g_program;
cjMatrix   g_modelViewProjectionMatrix;
cjMatrix3   g_normalMatrix;
cjFLOAT       g_rotation;
GLuint      g_vertexArray;
GLuint      g_vertexBuffer;

cjFLOAT       g_timeSinceLastUpdate;
cjMatrix   g_matModelView;
cjMatrix   g_matProj;


void SetupGeometry(cjGLRenDev* renDev)
{
	/*
	Are you using a VAO? You don't need to bind the VBO and IBO while rendering. Just bind the VAO and call glDrawElements.
 Also, try using unsigned short instead of unsigned byte for the indices

	The VAO sets the draw conditions so it binds the buffers and sets the vertex bindings.

 When you create the VAO, use the following sequence

 generate vao 
 bind vao
 generate vbo (optional can be created earlier)
 bind vbo
 generate veb (opional can be created earlier)
 bind veb (if used)
 set vertex binding - after vbo bound
 unbind vao (locks things in place any subsequent changes to vbo, veb or vertex binding will not affect this vao)

 When you draw
 bind vao */

//    self.effect = [[GLKBaseEffect alloc] init];
//    self.effect.light0.enabled = GL_TRUE;
//    self.effect.light0.diffuseColor = GLKVector4Make(1.0f, 0.4f, 0.4f, 1.0f);	
//    
 /*   renDev->glEnable(GL_DEPTH_TEST);
    
    renDev->glGenVertexArrays(1, &g_vertexArray);
   // renDev->glBindVertexArray(g_vertexArray);
    
    renDev->glGenBuffers(1, &g_vertexBuffer);
   // renDev->glBindBuffer(GL_ARRAY_BUFFER, g_vertexBuffer);
    renDev->glBufferData(GL_ARRAY_BUFFER, sizeof(gCubeVertexData), gCubeVertexData, GL_STATIC_DRAW);
    
    renDev->glEnableVertexAttribArray(EVTXAttrPosition);
    renDev->glVertexAttribPointer(EVTXAttrPosition, 3, GL_FLOAT, GL_FALSE, 24, BUFFER_OFFSET(0));
    renDev->glEnableVertexAttribArray(EVTXAttrNormal);
    renDev->glVertexAttribPointer(EVTXAttrNormal, 3, GL_FLOAT, GL_FALSE, 24, BUFFER_OFFSET(12));
    
   // renDev->glBindVertexArray(0);

	// renDev->glBindBuffer(GL_ARRAY_BUFFER, 0);*/
}

void Cleanup(cjGLRenDev* renDev)
{
    if(g_vertexBuffer)
    {
        renDev->glDeleteBuffers(1, &g_vertexBuffer);
    }
    
    if(g_vertexArray)
    {
        renDev->glDeleteVertexArrays(1, &g_vertexArray);
    }
    
    if(g_program)
    {
        renDev->glDeleteProgram(g_program);
        g_program = 0;
    }
}

void Update(cjGLRenDev* renDev, cjFLOAT width, cjFLOAT height)
{
//    cjFLOAT aspect = cjAbs(width / height);
//    
//    cjMatrix matProj;
//    cjGLMatrixPerspective(&matProj, cjDegree2Rad(65.0f), aspect, 0.1f, 100.0f);
//    g_matProj = matProj;
//    
//    cjMatrix baseModelViewMatrix;    
//    cjMatrix matTrans; matTrans.MakeTranslation(0.0f, 0.0f, -4.0f);
//	baseModelViewMatrix.MakeRotation(g_rotation, 0.0f, 1.0f, 0.0f);
//	baseModelViewMatrix = matTrans * baseModelViewMatrix;
//    
//    cjMatrix matModelView, matModelRot;
//	matModelView.MakeTranslation(0.0f,0.0f,-1.5f);
//	matModelRot.MakeRotation(g_rotation, 1.0f, 1.0f, 1.0f);
//	matModelView = baseModelViewMatrix * matModelView;
//    
//    g_matModelView = matModelView;
//
////    // 2nd
////    cjMatrix matModelView2;
////    cjMatrixTranslation(&matModelView2, 0, 0, 1.5);
////    cjMatrixAccumRotByAxis(&matModelView2, g_rotation, 1.0f, 1.0f, 1.0f);
////    cjGLMatrixMultiply(&matModelView2, &baseModelViewMatrix, &matModelView2);
//
//    // calc normal
////    cjMatrix3 matNormal;
////    cjGetMatrix3FromMatrix4(&matNormal, &matModelView2);
//    cjMatrix invMat;
//	matModelView.Inverse(invMat);
//	invMat.Transpose();
//	invMat.CopyMatrix3(g_normalMatrix);
//
//	g_modelViewProjectionMatrix = matProj * matModelView;
//    
//    cjFLOAT fv = cjGetEngineTimeElapsedSec();
//    
//    g_rotation += fv;
//    
//    if(g_rotation > CJ_PI *2.0)
//    {
//        while(g_rotation > (CJ_PI * 2.0))
//            g_rotation -= (CJ_PI*2.0);
//    }
//    
//    g_timeSinceLastUpdate += fv;
}

void Draw(cjGLRenDev* renDev)
{
 /*   static cjFLOAT vtmp = 0.0f;
    vtmp += cjGetEngineTimeElapsedSec();
    
    if(vtmp > 1.0f)
        vtmp = vtmp - ((cjUINT32)vtmp);
        

    renDev->glClearColor(0.0f, 0.65f, vtmp, 1.0f);
    renDev->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
   // renDev->glBindVertexArray(g_vertexArray);
    renDev->glUseProgram(g_program);
    
    renDev->glUniformMatrix4fv(uniforms[UNIFORM_MODELVIEWPROJECTION_MATRIX], 1, 0,
        (const GLfloat*)g_modelViewProjectionMatrix.m);
    renDev->glUniformMatrix3fv(uniforms[UNIFORM_NORMAL_MATRIX], 1, 0,
        (const GLfloat*)g_normalMatrix.m);
    
    renDev->glDrawArrays(GL_TRIANGLES, 0, 36);*/
}

cjBool LoadShaders(cjGLRenDev* renDev)
{
    GLuint vertShader, fragShader;
    
 static const char* vsh = "attribute vec4 position;"
"attribute vec3 normal;"
"varying lowp vec4 colorVarying;"
"uniform mat4 modelViewProjectionMatrix;"
"uniform mat3 normalMatrix;"
"void main()"
"{ "
"   vec3 eyeNormal = normalize(normalMatrix * normal);"
"   vec3 lightPosition = vec3(0.0, 0.0, 1.0);"
"   vec4 diffuseColor = vec4(0.4, 0.4, 1.0, 1.0);"
"   cjFLOAT nDotVP = max(0.0, dot(eyeNormal, normalize(lightPosition)));"
"   colorVarying = diffuseColor * nDotVP;"
"   gl_Position = modelViewProjectionMatrix * position;"
"} ";
    
static const char* fsh = "varying lowp vec4 colorVarying;"
"void main()"
"{ "
"    gl_FragColor = colorVarying;"
"} ";
    
    
    // Create shader program.
    g_program = renDev->glCreateProgram();
    
    // Create and compile vertex shader.
    if(!CompileShader(renDev, &vertShader, GL_VERTEX_SHADER, vsh))
    {
        cjLOG("Failed to compile vertex shader");
        return cjFalse;
    }
    
    // Create and compile fragment shader.
    if(!CompileShader(renDev, &fragShader, GL_FRAGMENT_SHADER, fsh))
    {
        cjLOG("Failed to compile fragment shader");
        return cjFalse;
    }
    
    // Attach vertex shader to program.
    renDev->glAttachShader(g_program, vertShader);
    
    // Attach fragment shader to program.
    renDev->glAttachShader(g_program, fragShader);
    
    // Bind attribute locations.
    // This needs to be done prior to linking.
    renDev->glBindAttribLocation(g_program, EVTXAttrPosition, "position");
    renDev->glBindAttribLocation(g_program, EVTXAttrNormal, "normal");
    
    // Link program.
    if(!LinkProgram(renDev, g_program))
    {
        cjLOG("Failed to link program: %d", g_program);
        
        if (vertShader)
        {
            renDev->glDeleteShader(vertShader);
            vertShader = 0;
        }
        
        if (fragShader)
        {
            renDev->glDeleteShader(fragShader);
            fragShader = 0;
        }
        
        if(g_program)
        {
            renDev->glDeleteProgram(g_program);
            g_program = 0;
        }
        
        return cjFalse;
    }
    
    // Get uniform locations.
    uniforms[UNIFORM_MODELVIEWPROJECTION_MATRIX] = renDev->glGetUniformLocation(g_program, "modelViewProjectionMatrix");
    uniforms[UNIFORM_NORMAL_MATRIX] = renDev->glGetUniformLocation(g_program, "normalMatrix");
    
    // Release vertex and fragment shaders.
    if (vertShader)
    {
        renDev->glDetachShader(g_program, vertShader);
        renDev->glDeleteShader(vertShader);
    }
    
    if (fragShader)
    {
        renDev->glDetachShader(g_program, fragShader);
        renDev->glDeleteShader(fragShader);
    }
    
    return cjTrue;
}


cjBool CompileShader(cjGLRenDev* renDev, GLuint* shader, GLenum type, const char* shaderTextSrc)
{
    GLint status;
    const GLchar *source = (GLchar*)shaderTextSrc;
    
//    source = (GLchar *)[[NSString stringWithContentsOfFile:file encoding:NSUTF8StringEncoding error:nil] UTF8String];
    
    if (!source)
    {
        cjLOG("Failed to load vertex shader");
        return cjFalse;
    }
    
    *shader = renDev->glCreateShader(type);
    renDev->glShaderSource(*shader, 1, &source, NULL);
    renDev->glCompileShader(*shader);
    
    GLint logLength;
    renDev->glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
    
    if(logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        renDev->glGetShaderInfoLog(*shader, logLength, &logLength, log);
        cjLOG("Shader compile log:\n%s", log);
        free(log);
    }
    
    renDev->glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    
    if(status == 0)
    {
        renDev->glDeleteShader(*shader);
        return cjFalse;
    }
    
    return cjTrue;
}


cjBool LinkProgram(cjGLRenDev* renDev, GLuint prog)
{
    GLint status;
    renDev->glLinkProgram(prog);
    
    GLint logLength;
    renDev->glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        renDev->glGetProgramInfoLog(prog, logLength, &logLength, log);
        cjLOG("Program link log:\n%s", log);
        free(log);
    }
    
    renDev->glGetProgramiv(prog, GL_LINK_STATUS, &status);
    
    if (status == 0)
        return cjFalse;
    
    return cjTrue;
}


cjBool ValidateProgram(cjGLRenDev* renDev, GLuint prog)
{
    GLint logLength, status;
    
    renDev->glValidateProgram(prog);
    renDev->glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    
    if(logLength > 0)
    {
        GLchar*log = (GLchar *)malloc(logLength);
        renDev->glGetProgramInfoLog(prog, logLength, &logLength, log);
        cjLOG("Program validate log:\n%s", log);
        free(log);
    }
    
    renDev->glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
    
    if(status == 0)
        return cjFalse;
    
    return cjTrue;
}


void xcj_gltestdraw_init(cjGLRenDev* renDev)
{

g_program = 0;
//g_modelViewProjectionMatrix;
//g_normalMatrix;
g_rotation = 0.0f;
g_vertexArray = 0;
g_vertexBuffer = 0;
g_timeSinceLastUpdate = 0;
//g_matModelView;
//g_matProj;




    LoadShaders(renDev);
    SetupGeometry(renDev);
}

void cj_gltestdraw_draw(cjGLRenDev* renDev, cjFLOAT width, cjFLOAT height)
{
//    Update(renDev, width, height);
//    Draw(renDev);
}

void cj_gltestdraw_end(cjGLRenDev* renDev)
{
return;

 //   Cleanup(renDev);
}

