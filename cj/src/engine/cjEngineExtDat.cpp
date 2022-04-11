
#include "cjEnginePCH.h"
#include "cjEngineExtDat.h"


CJSER_IMPL_CLASS(cjExtDatColor, cjExtraDat)

CJSER_IMPL_SERIALIZE_BEGIN(cjExtDatColor)
	ser << m_color;
CJSER_IMPL_SERIALIZE_END(cjExtDatColor)

CJSER_IMPL_CLONE_PROPCOPY_BEGIN(cjExtDatColor)
	d->m_color = m_color;
CJSER_IMPL_CLONE_PROPCOPY_END(cjExtDatColor)

CJSER_IMPL_CLONE(cjExtDatColor)


void cjExtDatColor::StaticInit()
{
	RegisterCreator(EXTDAT_SERID, (CJExtraDatCreatorType)&NewInst);
}


//
// engine ExtDat
//

void cjEngineExtDat_StaticInit()
{
	cjExtDatColor::StaticInit();
}