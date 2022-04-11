
#ifndef _CJAPPRES_H_
#define _CJAPPRES_H_


namespace cjApp {


enum ESPRITEID
{
	SPRID_GEM_THUNDER,
	SPRID_GEM_RUNE,
	SPRID_GEM_WATER,
	SPRID_GEM_HEAL,
	SPRID_GEM_FIRE,
	SPRID_GEM_TREE,
	SPRID_BACK_TILE_0,
	SPRID_BACK_TILE_1,
	SPRID_MAX
};

enum 
{ 
	SPRNAME_SIZ = 64,
	SPRFILEPATH_SIZ = 128
};


class SprPkg
{
	friend class SprManager;
public:
	enum EDF
	{
		DF_SPRIMG				= 0x10000000,
		DF_SPR_FRMLIST			= 0x20000000,
		
		DF_SPRITEM				= 0x00000001,
		DF_SPRITEM_BASETEX		= 0x00000002,		
	};
	
	// build instance
	CCSprite* SpawnCCSprite(const char* name);
	CCSpriteBatchNode* SpawnCCSpriteBatchNode();

	void GarbageCollect();

private:
	SprPkg() {}

protected:

	struct Sprite
	{
	public:
		enum ETYPE
		{
			ESPR_STATIC,
			ESPR_FRAMED
		};

		enum EDF
		{
			DF_INIT				= 0x10000000,
			DF_BASETEX			= 0x20000000,
			DF_TYPECHANGED		= 0x40000000,

			DF_RECT				= 0x00000001,
		};

		// ClearDirty needs		
		CCSprite* SpawnCCSprite(CCSpriteBatchNode* batchNode=NULL);
		void GarbageCollect();

		Sprite(SprPkg* owner, const char* name);
		~Sprite();

		void SetType(ETYPE type);

		void SetFrameInfo(const char* framePrefix, INT frameNum, FLOAT tweenTime);
		void SetArea(const CCRect& rect);
		void SetDirty(DWORD flag);
		void ClearDirty();

		SprPkg*					m_owner;
		CCSprite*				m_sprInstances;		// instance linkedlist

		char					m_name[SPRNAME_SIZ];
		ETYPE					m_type;
		
		// Framed Sprite
		std::string				m_framePrefix;
		INT						m_frameNum;
		CCArray*				m_frameSprites;   // CCArray<CCSpriteFrame*>
		FLOAT					m_frameTweenTime;
		CCAnimation*			m_frameAnimation;

		// Static Sprite
		CCRect					m_rect; // left,top,w,h
		DWORD					m_flag;
	};

	SprPkg(const char* name);
	~SprPkg();

	void SetSpriteImgPath(const char* path);
	void SetFrameListFile(const char* path);

	Sprite* GetSprite(const char* name, bool createAtNone=false); // ex) "thunder"
	CCSpriteFrame* GetSpriteFrameByName(const char* name);

	void SetDirty(DWORD flag, bool clearDirty=false) 
		{ m_flag |= flag; if(clearDirty) ClearDirty(); }

	void ClearDirty();

	inline CCTexture2D* GetBaseTex() { return m_base; }

protected:
	DWORD											m_flag;
	char											m_name[SPRNAME_SIZ];
	char											m_path[SPRFILEPATH_SIZ];

	std::string										m_frameListFile;
	
	CCTexture2D*									m_base;
	typedef	cjHashMap<std::string, Sprite*>			SprHashMap;
	SprHashMap										m_sprites;

	CCSpriteBatchNode*								m_sprBNodeInstances;
};


class SprManager
{
	friend class ResManager;

public:
	void Tick();
	bool Reload();

	CCSpriteBatchNode* SpawnCCSprBatchNode(const char* sprPkgName); 
	CCSprite* SpawnCCSprite(const char* pkgsprName); // "sprPkgName.name"
	
protected:	
	SprManager();
	~SprManager();
	void Clear();

	SprPkg* GetSpritePkg(const char* name, bool createAtNone=false);

protected:
	void*							m_spritesJsnDat;

	typedef cjHashMap<const char*, SprPkg* > SprPkgMap;
	SprPkgMap						m_sprPkg;
};


//
// cjAppRes
//

class ResManager
{
public:
	inline static ResManager* Get() { return ms_instance; }
	inline SprManager* GetSprMan() { return m_sprMan; }

	void Tick();
	void ReloadSprite(DWORD opt=0);
	
	static CCTexture2D* LoadCCTex(const char* path);
	
	static void StaticCreate();
	static void StaticShutDown();

	// "pkgname.name"
	static bool ExtractPkgName(const char* path, std::string& pkgName, std::string& name);
	
protected:
	ResManager();
	~ResManager();

	bool Init();
	void Finalize();

	bool LoadSprites();

protected:
	static ResManager*				ms_instance;
	
	SprManager*						m_sprMan;
};


} // end of cjApp namespace

#endif