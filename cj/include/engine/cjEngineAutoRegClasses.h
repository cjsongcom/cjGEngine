
/*
	This file only be located in cjEngine.cpp!
	Do not include this in anywhere except cjEngine.cpp!
*/

#ifndef _CJENGINEAUTOREGCLASSES_H_
#define _CJENGINEAUTOREGCLASSES_H_


#ifdef CJDLL
	#define CJENGINE_AUTOREG_CLASSES 
#else

	#include "cjAppEngine.h"
	#include "cjAppClient.h"

    #ifdef CJ_PLATFORM_WIN32
        #include "cjWinClient.h"
        #include "cjWinViewport.h"

        #define ENGINE_AUTOREG_WIN32_ONLY \
            cjWinClient::cjGetStaticCls(); \
            cjWinViewport::cjGetStaticCls(); \
            cjWGLVertexStream::cjGetStaticCls(); \


    #endif

    #ifdef CJ_PLATFORM_APPLE
        #include "cjIOSViewport.h"

        #define ENGINE_AUTOREG_APPLE_ONLY \
            cjIOSViewport::cjGetStaticCls(); \


    #endif


    //
    // Common
    //

	#include "cjGLRenDev.h"
	#include "cjGLRenInterface.h"
    #include "cjGeometry.h"
    #include "cjMesh.h"
    #include "cjObjFactoryForEngine.h"


	#define ENGINE_AUTOREG_CLASSES_COMMON \
		cjAppClient::cjGetStaticCls(); \
		cjAppEngine::cjGetStaticCls(); \
		cjClient::cjGetStaticCls(); \
		cjEngine::cjGetStaticCls(); \
		cjGeometry::cjGetStaticCls(); \
		cjGLRenDev::cjGetStaticCls(); \
		cjMaterial::cjGetStaticCls(); \
		cjMtlModifier::cjGetStaticCls(); \
		cjColorMtlModifier::cjGetStaticCls(); \
		cjFinalBlendMtlModifier::cjGetStaticCls(); \
		cjTexMtlModifier::cjGetStaticCls(); \
		cjTexCoordSrcStreamMtlModifer::cjGetStaticCls(); \
		cjTexMatrixMtlModifier::cjGetStaticCls(); \
		cjTexEnvMapMtlModifier::cjGetStaticCls(); \
		cjRenderedMaterial::cjGetStaticCls(); \
		cjBitmapMaterial::cjGetStaticCls(); \
		cjMesh::cjGetStaticCls(); \
		cjLODMesh::cjGetStaticCls(); \
		cjTestMesh::cjGetStaticCls(); \
		cjMeshInstance::cjGetStaticCls(); \
		cjLODMeshInstance::cjGetStaticCls(); \
		cjTestMeshInstance::cjGetStaticCls(); \
		cjTextureFactory::cjGetStaticCls(); \
		cjRenDev::cjGetStaticCls(); \
		cjResChr::cjGetStaticCls(); \
		cjTexture::cjGetStaticCls(); \
		cjViewport::cjGetStaticCls(); \
		\
		cjExtDatColor::cjGetStaticCls(); \
		cjGLResCache::cjGetStaticCls(); \
		cjGLTexture::cjGetStaticCls(); \
		cjGLVertexStreamAttribLayout::cjGetStaticCls(); \
		cjGLVertexStream::cjGetStaticCls(); \
		cjGLIndexBuffer::cjGetStaticCls(); \
		cjGLIndexBufferNonVBO::cjGetStaticCls(); \
		cjGLIndexBufferVBO::cjGetStaticCls(); \
		cjGLShaderCache::cjGetStaticCls(); \
		cjGLShaderProgram::cjGetStaticCls(); \
		cjGLShader::cjGetStaticCls(); \
		cjGLVertexShader::cjGetStaticCls(); \
		cjGLPixelShader::cjGetStaticCls(); \
		cjGLRenTargetGrp::cjGetStaticCls(); \
		cjGLRenTarget::cjGetStaticCls(); \
		cjSceneCamera::cjGetStaticCls(); \
		cjSceneMesh::cjGetStaticCls(); \
		cjSceneSkinMeshMod::cjGetStaticCls(); \
		cjSceneRenProp::cjGetStaticCls(); \
		cjSceneRenPropAlpha::cjGetStaticCls(); \
		cjSceneRenPropMaterial::cjGetStaticCls(); \
		cjSceneRenPropTextures::cjGetStaticCls(); \
		cjSceneRenPropShade::cjGetStaticCls(); \
		cjSceneRenPropSpecular::cjGetStaticCls(); \
		cjSceneRenPropWireframe::cjGetStaticCls(); \
		cjSceneRenPropTwoSide::cjGetStaticCls(); \
		cjTextureTM::cjGetStaticCls(); 
		

		// cjSceneRenPropVtxColor::cjGetStaticCls(); \

    #define ENGINE_AUTOREG_CLASSES_MERGE(x,y) x y


    #ifdef CJ_PLATFORM_WIN32
        #define CJENGINE_AUTOREG_CLASSES \
            ENGINE_AUTOREG_CLASSES_MERGE(ENGINE_AUTOREG_CLASSES_COMMON, ENGINE_AUTOREG_WIN32_ONLY)
    #elif defined(CJ_PLATFORM_APPLE)
        #define CJENGINE_AUTOREG_CLASSES \
            ENGINE_AUTOREG_CLASSES_MERGE(ENGINE_AUTOREG_CLASSES_COMMON, ENGINE_AUTOREG_APPLE_ONLY)
    #else
        #error "not impl.."
    #endif


#endif


#endif

