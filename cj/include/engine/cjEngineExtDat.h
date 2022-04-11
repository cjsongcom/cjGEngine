
#ifndef _CJENGINEEXTDAT_H_
#define _CJENGINEEXTDAT_H_


//
// cjExtDatColor
//

class CJENGINE_API cjExtDatColor : public cjExtraDat
{
	CJSER_DECL_CLASS(cjExtDatColor, cjExtraDat, 0, CJENGINE_API)
	CJSER_DECL_CLONE(cjExtDatColor)
	CJSER_DECL_SERIALIZE 

public:
	enum { EXTDAT_SERID = SERID_COLOR };
	virtual cjBYTE GetSerClsID() { return EXTDAT_SERID; }

public:
	cjExtDatColor() { m_color = cjColorF::BLACK; }
	cjExtDatColor(const cjColorF& color) { m_color = color; }
	cjExtDatColor(const cjColor& color) { m_color = color; }

	void SetValue(const cjColorF& colorF) { m_color = colorF; }	
	const cjColorF& GetValue() const { return m_color; }

	void SetValue(const cjColor color) { m_color = color; }
	void GetValue(cjColor& color) const { color = cjColor(&m_color); }

	void SetR(const cjFLOAT r)	{ m_color.r = r; }
	void SetG(const cjFLOAT g)	{ m_color.g = g; }
	void SetB(const cjFLOAT b)	{ m_color.b = b; }
	void SetA(const cjFLOAT a)	{ m_color.a = a; }

	static void StaticInit();

protected:
	cjColorF						m_color;
};

inline cjExtDatColor::~cjExtDatColor() {}



void cjEngineExtDat_StaticInit();


#endif

