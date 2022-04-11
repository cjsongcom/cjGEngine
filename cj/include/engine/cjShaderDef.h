
#ifndef _CJSHADERDEF_H_
#define _CJSHADERDEF_H_


class cjShader;
class cjShaderMtl;
class cjSceneMesh;


//
// cjShaderMtlDesc
//

class CJENGINE_API cjShaderMtlDesc : public cjMem
{
	friend class cjShMtlDescUtil;
	friend class HashKeyCmp;

public:
	enum { EARY_COUNT = 4 };

	typedef cjUINT32 MtlFlagType[EARY_COUNT];		// cjUINT3 [3]
	typedef struct { cjUINT32 v[EARY_COUNT]; } HashKey;
	
	class CJENGINE_API HashKeyCmp
	{
	public:
		bool operator()(const HashKey& lhs, const HashKey& rhs) const
		{
			return cjMemCmp(lhs.v, rhs.v, sizeof(lhs.v)) == 0;
		}
	};

	class CJENGINE_API KeyHasher
	{
	public:
		cjTHasherValType operator()(const HashKey& key) const
		{
			cjUINT32 hash = key.v[0];

			for(cjUINT32 i=1; i<EARY_COUNT; i++)
				hash = (hash << 5) + hash + key.v[i];
			
			return hash;
		}
	};
	
public:
	cjShaderMtlDesc();
	cjShaderMtlDesc(const cjShaderMtlDesc& rhs);

	cjBool GetFlagString(cjNameChar* buf, cjUINT32 bufSiz) const;
	const HashKey& GetHashKey() const { return *((HashKey*)m_descFlag); }

	const cjName& GetShaderMtlClsName() const { return m_shMtlClsName; }
	void SetShaderMtlClsName(const cjName& name) { m_shMtlClsName = name; } 

	cjShaderMtlDesc& operator= (const cjShaderMtlDesc& desc);

	bool operator== (const cjShaderMtlDesc& rhs) const;
	bool operator!= (const cjShaderMtlDesc& rhs) const { return !operator==(rhs); }

protected:	
	MtlFlagType						m_descFlag;
	cjName							m_shMtlClsName;
};



//
// cjShaderProgramDesc
//

class CJENGINE_API cjShaderProgramDesc : public cjMem
{
	friend class HashKeyCmp;

public:
	enum { EARY_COUNT = 4 };
	typedef cjUINT32 ShaderProgramFlagType[EARY_COUNT];	// cjUINT3 [3]
	typedef struct { cjUINT32 v[EARY_COUNT]; } HashKey;

	class CJENGINE_API HashKeyCmp
	{
	public:
		bool operator()(const HashKey& lhs, const HashKey& rhs) const
		{
			return cjMemCmp(lhs.v, rhs.v, sizeof(lhs.v)) == 0;
		}
	};

	class CJENGINE_API KeyHasher
	{
	public:
		cjTHasherValType operator()(const HashKey& key) const
		{
			cjUINT32 hash = key.v[0];

			for(cjUINT32 i=1; i<EARY_COUNT; i++)
				hash = (hash << 5) + hash + key.v[i];
			
			return hash;
		}
	};
	
public:
	cjShaderProgramDesc();
	cjShaderProgramDesc(const cjShaderProgramDesc& rhs);

	const cjStringA::T* GetFlagStringA(cjStringA& outFlagStr) const;
	const HashKey& GetHashKey() const { return *((HashKey*)m_descFlag); }

	const cjName& GetShaderMtlClsName() const { return m_shMtlClsName; }
	void SetShaderMtlClsName(const cjName& name) { m_shMtlClsName = name; } 

	cjShaderProgramDesc& operator=(const cjShaderProgramDesc& desc);

	bool operator== (const cjShaderProgramDesc& rhs) const;
	bool operator!= (const cjShaderProgramDesc& rhs) const { return !operator==(rhs); }

	void Fill(HashKey& dst) const { cjMemCpy(dst.v, m_descFlag, sizeof(dst.v)); }
	
protected:	
	ShaderProgramFlagType			m_descFlag;
	cjName							m_shMtlClsName;
};




#define CJSHDESC_DEF_BEGIN(name) \
	enum _E##name { \

#define CJSHDESC_DEF_END(name, prev)	\
			_A##name##_LAST_	\
		}; \
	static const cjUINT32 _A##name##_BPOS	= _A##prev##_DNBPOS-1; \
	static const cjUINT32 _A##name##_SHCOUNT= _A##name##_BPOS % 32; \
	static const cjUINT32 _A##name##_ARYIDX	= _A##name##_BPOS / 32; \
	static const cjUINT32 _A##name##_VMASK	= (0xffffffff) >> (31 - ((_A##name##_LAST_-1)/2)); \
	static const cjUINT32 _A##name##_DNBPOS	= _A##name##_BPOS + ((_A##name##_LAST_-1)/2) + 2; \
	cjUINT32 Get##name() const { \
		return (m_descFlag[_A##name##_ARYIDX] >> _A##name##_SHCOUNT) & _A##name##_VMASK; } \
	void Set##name(cjUINT32 v) { \
		m_descFlag[_A##name##_ARYIDX] &= ~(_A##name##_VMASK << _A##name##_SHCOUNT); \
		m_descFlag[_A##name##_ARYIDX] |= v << _A##name##_SHCOUNT; } \


#define CJSHDESC_DEF_BIT(name, bitsiz, prev)	\
	static const cjUINT32 _A##name##_BPOS	= _A##prev##_DNBPOS-1; \
	static const cjUINT32 _A##name##_SHCOUNT= _A##name##_BPOS % 32; \
	static const cjUINT32 _A##name##_ARYIDX	= _A##name##_BPOS / 32; \
	static const cjUINT32 _A##name##_VMASK	= (0xffffffff) >> (32 - bitsiz); \
	static const cjUINT32 _A##name##_DNBPOS	= _A##name##_BPOS + (bitsiz) + 1; \
	cjUINT32 Get##name() const { \
		return (m_descFlag[_A##name##_ARYIDX] >> _A##name##_SHCOUNT) & _A##name##_VMASK; } \
	void Set##name(cjUINT32 v) { \
		m_descFlag[_A##name##_ARYIDX] &= ~(_A##name##_VMASK << _A##name##_SHCOUNT); \
		m_descFlag[_A##name##_ARYIDX] |= v << _A##name##_SHCOUNT; } \


#define CJSHDESC_DEF_BIT_CHECK(name, bitsiz, prev, seqint)	\
	CJSHDESC_DEF_BIT(name, bitsiz, prev) \
	static const cjUINT32 _A##name##_CHECK0 = 1 << (seqint - _A##name##_ARYIDX); \
	static const cjUINT32 _A##name##_CHECK1 = 1 << (0 - _A##name##_SHCOUNT); \
		

#endif
