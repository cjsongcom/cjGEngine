
#ifndef _CJAPPCVTCCS_H_
#define _CJAPPCVTCCS_H_

template<cjUINT32 parseType>
struct cjToCCRect
{
	template<class T2>
	CCRect operator()(const cjRect<T2> rect) const
		{	return CCRect((cjFLOAT)rect.l, (cjFLOAT)rect.t, 
			  (cjFLOAT)(rect.r-rect.l), (cjFLOAT)(rect.b-rect.t));	}
};

template<>
struct cjToCCRect<1>
{
	template<class T2>
	CCRect operator()(const cjRect<T2> rect) const
		{	return CCRect((cjFLOAT)rect.l, (cjFLOAT)rect.t, (cjFLOAT)(rect.r), (cjFLOAT)(rect.b));	}
};

#endif
