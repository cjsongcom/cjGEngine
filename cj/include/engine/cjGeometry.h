
#ifndef _CJGEOMETRY_H_
#define _CJGEOMETRY_H_


CJOBJ_DECL_ABSTRACT_BEGIN(CJENGINE_API, cjGeometry, cjObj, 0, cjEngine)

	CJOBJ_DECL_SERIALIZE

public:
    cjGeometry();


protected:
    virtual void OnDestroy();

private:
	cjFBox									m_bbox;
	cjFSphere								m_bsphere;

CJOBJ_DECL_ABSTRACT_END(cjGeometry)


#endif
