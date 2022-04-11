
#ifndef _CJAPPLERENRES_H_
#define _CJAPPLERENRES_H_

#include "engine/cjRenRes.h"


class cjEAGLAdapter;


class cjFAppleDefFrmRenTarget : public cjFRenTarget
{
public:
    virtual cjFAppleDefFrmRenTarget* GetAppleDefFrmRenTarget() { return this; }
    
public:
    enum EAGL_IOS_DEFAULT_FRAME_RENBUFFER { cjEAGL_IOS_DEFAULT_FRAME_RENBUFFER = 0 };
    
    enum EAPPLE_RT_FLAG
    {
        EARF_IOS_EAGL_DEFAULT_RENBUFFER = 0x00000001,
    };

 	cjFAppleDefFrmRenTarget();
	virtual ~cjFAppleDefFrmRenTarget();
    
    cjEAGLAdapter* GetEAGLAdapter();
    
private:
    cjTFLAG                                     m_appleRTFlag;
};


class cjFAppleDefFrmRenTargetGrp : public cjFRenTargetGrp
{
public:
	virtual cjResCacheIDType GetResCacheIDType() { return cjRSCID_RenderTarget; }
    virtual class cjFAppleDefFrmRenTargetGrp* GetAppleDefFrmRenTargetGrp() { return this; }

public:
	cjFAppleDefFrmRenTargetGrp();
	virtual ~cjFAppleDefFrmRenTargetGrp();
    
    cjEAGLAdapter* GetEAGLAdapter();
};


#endif
