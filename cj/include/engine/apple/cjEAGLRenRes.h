
#ifndef _CJEAGLRENRES_H_
#define _CJEAGLRENRES_H_

#include "engine/cjGLRenRes.h"

cjRSurfaceFmt cjConvertEAGLFmtToRSurfaceFmt(GLenum fmtColor);


//
// default render target / render target group for EAGL
//

class cjEAGLDefFrmRenTarget : public cjGLRenTarget
{
	CJSER_DECL_CLASS(cjEAGLDefFrmRenTarget, cjGLRenTarget, 0, CJENGINE_API)
    CJSER_NO_CLONEABLE

public:
	virtual void Cache(cjFRenResSrc* renResSrc);
    
protected:
	virtual void ReleaseColorSurface();
	virtual void ReleaseDepthStencil();
    
public:
    cjEAGLDefFrmRenTarget() {}
	void Init(cjGLRenDev* renDev, cjResCacheID cacheID);
    
private:
    static cjEAGLDefFrmRenTarget* ms_eaglDefFrmRenTarget;
};


class cjEAGLDefFrmRenTargetGrp : public cjGLRenTargetGrp
{
	CJSER_DECL_CLASS(cjEAGLDefFrmRenTargetGrp, cjGLRenTargetGrp, 0, CJENGINE_API)
    CJSER_NO_CLONEABLE

public:
	virtual void Cache(cjFRenResSrc* renResSrc);
    
protected:
    virtual void ReleaseFrameBufferObj();

public:
    cjEAGLDefFrmRenTargetGrp() {}
	void Init(cjGLRenDev* renDev, cjResCacheID cacheID);

};


class cjEAGLVertexStream : public cjGLVertexStream
{
	CJSER_DECL_CLASS(cjEAGLVertexStream, cjGLVertexStream, 0, CJENGINE_API)
    CJSER_NO_CLONEABLE

public:
	virtual void Cache(cjFRenResSrc* renResSrc);


public:
    virtual void* GetEAGLVertexStream() { return this; }
	virtual void* GetVertexData();
	virtual void Reallocate(cjINT32 newSiz);
	virtual cjINT32 AddVertices(cjFVertexStream* vertices) { return 0; }

public:
    cjEAGLVertexStream() {}
    void Init(cjGLRenDev* renDev, cjResCacheID cacheID);

};


#endif
