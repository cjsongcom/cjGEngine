
#include "cjCorePCH.h"
#include "cjObjFactory.h"


CJOBJ_IMPL_CLASS(cjObjFactory)

cjObjFactory::cjObjFactory()
{
    // m_factoryFlag, m_supportCls will be initialized with default of cjObjCls,
    // cjObjInitProperties in cjObj::__cjObjAlloc is doing
}

void cjObjFactory::OnDestroy()
{
    cjSuper::OnDestroy();
}

cjObj* cjObjFactory::StaticImportObject(cjObjCls* cls, cjObj* outer, const cjNameChar* name,
    const cjUni* filepath, cjFilePathHint filepathHint, const cjNameChar* paramText, cjObjFactory* impFactory)
{
	if(outer == CJPKG_TEMP)
		outer = cjObj::cjObjGetTempPkg();

	if(!outer)
	{
		cjObj::cjObjResolveNameAndPkg(outer, name, cjTrue);
	}

	if(!outer)
		return cjNULL;

	cjTAry<cjObjFactory*> factories;
    
	if(impFactory)
	{
        cjAssert(impFactory->IsSupportCls(cls));
		factories.Push(impFactory);
	}
	else
	{ 
        // collect object import factory
		for(cjTObjIterator<cjObjCls> it; it; ++it)
		{
			// GetCls() for all the cjObjCls inherited class's is cjObjCls::GetStaticCls()
			if(it->IsChildOf(cjObjFactory::cjObjGetStaticCls()))
			{
                // cur is not a cjObjFactory and not a cjObjCls_cjObjFactory,
                // just property data, casting used to access property.
                cjObjFactory* defaultPropDat = (cjObjFactory*)it->cjObjGetDefaultObj();                
                if(defaultPropDat->m_supportCls == cls)
                {
                    cjObjFactory* factoryObj = cjObjNew<cjObjFactory>(*it);
                    
                    cjAssert(factoryObj);
                
                    factories.Push(factoryObj);
                }
			}
		}
        
	//	Sort(&factories(0), factories.Num());
	}

	cjAssert(!factories.IsEmpty());

	for(cjAryOff i=0; i<factories.Num(); i++)
	{
		cjObjFactory* curFactory = factories(i);
		cjObj* rst = NULL;
        
        cjTFLAG factFlag = curFactory->GetFactoryFlag();
        
		if(factFlag & EFACTF_CREATE_NEWINST)
        {
			if(filepath && *filepath)
			{
				curFactory->cjObjParseParam(paramText);
				rst = curFactory->CreateNew(cls, outer, name, 0, NULL);
			}
		}
		else if(filepath && *filepath)
		{
			if(factFlag & EFACTF_IMPORT_FROM_TEXT)
			{
				cjString txt;
//				if(cjLoadFileToString(txt, filepath))
//				{
//					const cjNameChar* srcStr = *txt;
//					curFactory->cjObjParseParm(paramText);
//					rst = curFactory->ImportFromText(cls, outer, name, appFExt(Filename), srcStr, srcStr+txt.Len());
//				}
			}
			else if(factFlag & EFACTF_IMPORT_FROM_BIN)
			{
				cjTAry<cjBYTE> byteAry;
				if(cjLoadFileToByteArray(byteAry, filepath, filepathHint))
				{
					// byteAry.Push(0); // no reallocation is occured. cjLoadFileToByteArray is allocating with spare space
                    // this is used to calculate length in ImportFromBinary function
                    
					const cjBYTE* dst = &byteAry(0);
					curFactory->cjObjParseParam(paramText);
                    
					rst = curFactory->ImportFromBinary(cls, outer, name,
                        0, NULL, cjGetFileExtStr(filepath), dst, dst+byteAry.Num());
				}
			}
		}
        
		if(rst)
		{
			if(!impFactory)
            {
				for(cjAryOff i=0; i<factories.Num(); i++)
					delete factories(i);
            }
            
			return rst;
		}
        
	} // end of for
    
    
	if(!impFactory)
    {
		for(cjAryOff i=0; i<factories.Num(); i++)
			delete factories(i);
    }
    
	return NULL;
}


