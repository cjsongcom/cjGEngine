
#include "cjCorePCH.h"
#include "cjImgLib.h"


//
// cjImgTGA
//

CJCORE_API const cjChar* cjImgTGA::STR_EXTA = "TGA";
CJCORE_API const cjUni* cjImgTGA::STR_EXTW = _CU("TGA");

// initialized at cjTextureFactory::_cjObjOnStaticRegister
CJCORE_API cjImg_CreatecjObjPFN cjImgTGA::cjTextureCreator = cjNULL;

cjBool cjImgTGA::CheckExt(const cjChar* ext)
{
    if(ext && !cjStricmp(ext, STR_EXTA))
        return cjTrue;
    
    return cjFalse;
}

cjBool cjImgTGA::CheckExt(const cjUni* ext)
{
    if(ext && !cjStricmp(ext, STR_EXTW))
        return cjTrue;
    
    return cjFalse;
}

cjBool cjImgTGA::LoadImgStream(const cjBYTE* dataSt, const cjBYTE* dataEn, cjBYTE* dst, void* prm)
{
//	ImgStreamLoadParam* loadPrm = (ImgStreamLoadParam*)prm;
	cjINT32 dataLen = (cjINT32)(dataEn - dataSt);

	if(dataLen < sizeof(cjImgTGAHeader))
        return cjFalse;

	cjImgTGAHeader* hdr = (cjImgTGAHeader*)dataSt;

	cjBYTE* body = ((cjBYTE*)hdr) + sizeof(cjImgTGAHeader);
    cjBYTE* colorMapOff = body + hdr->m_idLength;
    cjBYTE* imgDat = (cjBYTE*)(colorMapOff + (hdr->m_colorMapDepth + 4)/8 * hdr->m_colorMapLength);
    cjINT32 imgW = hdr->m_width;
	cjINT32 imgH = hdr->m_height;
	
	if(hdr->m_imgDataType == cjImgTGA::ETGA_COMPRESS_RLE)
	{
		if(hdr->m_bitsPerPixel == 32)
		{
            cjBYTE samplePixel[4];
            cjINT32 runRLE = 0;
            cjINT32 runRAW = 0;
            
			for(cjINT32 y=imgH-1; y>=0; y--) // y-flip
			{
				for(cjINT32 x=0; x<imgW; x++)
				{
					if(runRLE > 0)
                    {
						runRLE--;
                    }
					else if(runRAW == 0)
					{
						cjBYTE chunkRLE = *(imgDat++);
						
                        if(chunkRLE & 0x80)
						{
							runRLE = (chunkRLE & 0x7F) + 1;
							runRAW = 1;
						}
						else
						{
							runRAW = (chunkRLE & 0x7F) + 1;
						}
                    }
                        
					if(runRAW > 0)
					{
						*(cjUINT32*)&samplePixel = *(cjUINT32*)imgDat; // sample RGBA 32bit
						imgDat += 4;
						runRAW--;
						runRLE--;
					}
						
					*(cjUINT32*)(((dst + y*imgW)+x)) = *(cjUINT32*)&samplePixel;
                    
				} // end of for width
			}        
        }
		else if(hdr->m_bitsPerPixel == 24)
		{
            cjBYTE samplePixel[4];
            samplePixel[3] = 0xff; // samplePixel[3] is always 0xff
            cjINT32 runRLE = 0;
            cjINT32 runRAW = 0;
				
			for(cjINT32 y=imgH-1; y>=0; y--) // y-flip
			{
				for(cjINT32 x=0; x<imgW; x++)
				{
					if(runRLE > 0)
					{
                    	runRLE--;
					}
                    else if(runRAW == 0)
					{
						cjBYTE chunkRLE = *(imgDat++);
						
                        if(chunkRLE & 0x80)
						{
							runRLE = (chunkRLE & 0x7F) + 1;
							runRAW = 1;
						}
						else
						{
							runRAW = (chunkRLE & 0x7F) + 1;
						}
					}
                    
					if(runRAW > 0)
					{   
						// sample RGB
						samplePixel[0] = *(imgDat++);
						samplePixel[1] = *(imgDat++);
						samplePixel[2] = *(imgDat++);
                        
						runRAW--;
						runRLE--;
					}
					
					*(cjUINT32*)(((dst + y*imgW) +x)) = *(cjUINT32*)&samplePixel;
				}
			}
        }
        else
        {
            cjAssert(!"ERR!");
			return cjFalse;
        }
    }
    else if(hdr->m_imgDataType == cjImgTGA::ETGA_UNCOMPRESS_RAW)
    {
        if(hdr->m_bitsPerPixel == 32)
        {
			const cjUINT32 rowSiz = imgW*4;
						
			// RGBA copy
			// (imgH-y-1) for opengl bottom(0.0) -> top(1.0)
            for(cjINT32 y=0; y<imgH;y++) 
                cjMemCpy(dst + rowSiz*y, imgDat+ (imgH-y-1) * rowSiz, rowSiz); 
        }
        else if(hdr->m_bitsPerPixel == 24)
        {
            cjBYTE  samplePixel[4];
            samplePixel[3] = 0xff; // samplePixel[3] is always 0xff
			
            for(cjINT32 y=0; y<imgH; y++)
            {
                for(cjINT32 x=0; x<imgW; x++)
                {
                    cjBYTE* src = (imgDat + (imgH-y-1)*imgW*3)+x*3;
                
                    samplePixel[0] = *src;
                    samplePixel[1] = *(src+1);
                    samplePixel[2] = *(src+2);
                    
                    *((cjUINT32*)((dst+y*imgW*3)+x)) = *(cjUINT32*)&samplePixel;
                }
            }
        }
    }
    else
    {
        return cjFalse;
    }

	return cjTrue;
}

