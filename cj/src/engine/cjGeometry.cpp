
#include "cjEnginePCH.h"
#include "cjGeometry.h"


//
// cjGeometry
//

CJOBJ_IMPL_CLASS(cjGeometry)
CJOBJ_IMPL_SERIALIZE_BEGIN(cjGeometry)
	stream << m_bbox;
	stream << m_bsphere;
CJOBJ_IMPL_SERIALIZE_END(cjGeometry)


cjGeometry::cjGeometry()
{

}

void cjGeometry::OnDestroy()
{
    cjSuper::OnDestroy();
}
