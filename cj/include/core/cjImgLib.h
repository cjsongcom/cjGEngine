
#ifndef _CJIMGLIB_H_
#define _CJIMGLIB_H_


typedef void* (*cjImg_CreatecjObjPFN)(const cjBYTE* dataSt, const cjBYTE* dataEn, void* objCls,
    void* outer, cjName name, cjTFLAG flags);


//
// TGA
//

#pragma pack(push,1)

struct CJCORE_API cjImgTGAHeader
{
	cjBYTE m_idLength;
	cjBYTE m_colorMapType;
	cjBYTE m_imgDataType;  // 10:Compressed-RLE
    cjWORD m_colorMapOrigin;
	cjWORD m_colorMapLength;
	cjBYTE m_colorMapDepth;
	cjWORD m_xOrigin;
	cjWORD m_yOrigin;
	cjWORD m_width;
	cjWORD m_height;
	cjBYTE m_bitsPerPixel;
	cjBYTE m_imgDescriptor;
    
	CJCORE_API friend cjFStream& operator<<(cjFStream& s, cjImgTGAHeader& h)
	{
		s << h.m_idLength << h.m_colorMapType << h.m_imgDataType;
		s << h.m_colorMapOrigin << h.m_colorMapLength << h.m_colorMapDepth;
		s << h.m_xOrigin << h.m_yOrigin << h.m_width << h.m_height << h.m_bitsPerPixel;
		s << h.m_imgDescriptor;
        
		return s;
	}
};

#pragma pack(pop) // end of pack(push,1)


class CJCORE_API cjImgTGA
{
public:
    enum ETGA_TYPE
    {
        ETGA_UNCOMPRESS_RAW     = 2,
        ETGA_COMPRESS_RLE       = 10,
    };

	struct ImgStreamLoadParam
	{
		cjUINT32			m_reserved;
//		cjImgTGAHeader*		m_hdr;
	};


public:
	static cjBool LoadImgStream(const cjBYTE* dataSt, const cjBYTE* dataEn, cjBYTE* dst, void* prm/*ImgStreamLoadParam*/);

	static cjBool CheckExt(const cjChar* ext);
    static cjBool CheckExt(const cjUni* ext);

public:
    static const cjChar*                    STR_EXTA;
	static const cjUni*						STR_EXTW;
	static cjImg_CreatecjObjPFN             cjTextureCreator; // cjImgTGA::CreatecjObj
};


#endif
