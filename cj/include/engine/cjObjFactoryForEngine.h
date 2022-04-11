
#ifndef _CJOBJFACTORYFORENGINE_H_
#define _CJOBJFACTORYFORENGINE_H_


//
// cjTextureFactory
//

CJOBJ_DECL_CLASS_BEGIN(CJENGINE_API, cjTextureFactory, cjObjFactory, 0, cjEngine)
   
	CJOBJ_DECL_STATIC_REG

public:
    virtual cjBool IsSupport(cjObjCls* cls);
	virtual cjObj* ImportFromBinary(cjObj* cls, cjObj* outer, cjName name, cjTFLAG flags,
        cjObj* ctx, const cjUni* type, const cjBYTE* src, const cjBYTE* srcEnd);

public:
	cjTextureFactory();

protected:
    
CJOBJ_DECL_CLASS_END(cjTextureFactory)


#endif

