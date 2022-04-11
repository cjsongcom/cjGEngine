
#ifndef _CJAPPCLIENT_H_
#define _CJAPPCLIENT_H_


CJOBJ_DECL_CLASS_BEGIN(CJENGINE_API, cjAppClient, cjClient, 0, cjEngine)

public:
	cjAppClient();

	virtual cjBool Init(cjEngine* engine);
	virtual void Tick();
    virtual void Draw();
	virtual cjBool Exec(const cjNameChar* cmd);

	virtual void FlushCache();

	virtual cjViewport* CreateViewport(cjTFLAG vpFlag, const cjNameChar* name,
		cjINT32 left, cjINT32 top, cjINT32 width, cjINT32 height, void* prm=NULL);

	virtual void OnDestroyViewport(cjViewport* viewport);

protected:
	virtual void OnDestroy();

	cjBool InitInputDevice();
	void FinalizeInputDevice();

CJOBJ_DECL_CLASS_END(cjAppClient)


#endif
