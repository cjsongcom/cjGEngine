
#ifndef _CJSCENEPROP_H_
#define _CJSCENEPROP_H_


cjRefPtrDefine(cjSceneProp)

//cjdefer change cjTPtrList<cjScenePropRPtr> -> change to manual pointer list like cjNode type
typedef cjTList<cjScenePropRPtr>			cjScenePropRPtrList;


class CJCORE_API cjSceneProp : public cjRefObj
{
	CJSER_DECL_CLASS(cjSceneProp, cjRefObj,0 , CJCORE_API)
	CJSER_DECL_SERIALIZE
	CJSER_DECL_CLONE(cjSceneProp)

	friend class cjSceneNode;

public:
	//static cjSerializer& SerializeAry(cjSerializer& ser, cjSceneProp** d, cjINT32* count=0);
	static cjSerializer& SerializeList(cjSerializer& ser, cjScenePropRPtrList* scenePropList, cjINT32* count=0);
	static cjSerializer& PostSerializeList(cjSerializer& ser, cjScenePropRPtrList* scenePropList);
	//static void CloneList(cjSceneProp*& dstPropList, const cjSceneProp* srcPropList);

public:
	enum EPropCate
	{
		EPropCate_None,
		EPropCate_Render,
	};

	cjSceneProp() 
	{ 
//		_nextProp = 0; 
	} 

	virtual EPropCate GetPropCate() { return m_cate; }
	virtual cjSceneProp* GetRenProp() { return cjNULL; }

	virtual void Update(cjFLOAT deltaTime);

//	const cjSceneProp* GetNextSceneProp() const { return _nextProp; }
//	cjSceneProp* GetNextSceneProp() { return _nextProp; }
	
protected:
	EPropCate						m_cate;
	
private:
	//static cjBool Find(cjSceneProp* list, cjSceneProp* dst, cjSceneProp** prev=NULL);
	//static cjSceneProp* FindByRtti(cjSceneProp* list, const cjRTTI* rtti);
	//static cjSceneProp* FindByRttiName(cjSceneProp* list, const cjNameChar* rttiName);
	//static void ReleaseRefPropList(cjSceneProp* list);
	//static cjINT32 GetNumProp(cjSceneProp* list);

	//cjSceneProp*							_nextProp;	// managed by only in cjSceneNode, do not release in ~cjSceneProp()
};


template<class T>
T* cjScenePropUtil_Find(cjScenePropRPtrList& lst, cjBool createAtNone=cjFalse)
{
	cjScenePropRPtrList::Iter iter = lst.Begin();
	cjScenePropRPtr v;
	T* dst = cjNULL;

	while(lst.GetNextV(iter, v))
	{
		if(v->cjrRTTI() == T::cjrStaticRTTI())
		{
			dst = (T*)(v._Ptr());
			break;
		}
	}

	if(!dst && createAtNone)
	{
		dst = T::NewInst();
		v = dst;

		lst.PushBack(v);
	}

	return dst;
}

template<class T>
const T* cjScenePropUtil_Find(const cjScenePropRPtrList& lst, cjBool createAtNone=cjFalse)
{
	return (const T*)cjScenePropUtil_Find<T>(const_cast<cjScenePropRPtrList&>(lst), createAtNone);
}

#endif

