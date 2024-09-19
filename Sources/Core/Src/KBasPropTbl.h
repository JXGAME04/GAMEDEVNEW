//---------------------------------------------------------------------------
// Sword3 Core (c) 2002 by Kingsoft
//
// File:	KBasPropTbl.h
// Date:	2020.08.14
// Code:	Fong KiÒu
// Desc:    header file. 
//---------------------------------------------------------------------------

#ifndef	KBasPropTblH
#define	KBasPropTblH

#define		SZBUFLEN_0	80		
#define		SZBUFLEN_1	128		
#define		SZBUFLEN_256	256
#define		SZBUFLEN_512	512

#define		MAX_MAGIC_PREFIX	20
#define		MAX_MAGIC_SUFFIX	20

typedef struct
{
	char		m_szName[SZBUFLEN_0];		
	int			m_nItemGenre;				
	int			m_nDetailType;				
	int			m_nParticularType;			
	char		m_szImageName[SZBUFLEN_0];	
	int			m_nObjIdx;					
	int			m_nWidth;					
	int			m_nHeight;					
	char		m_szIntro[SZBUFLEN_1];		
	int			m_nSeries;					
	int			m_nPrice;					
	int			m_nLevel;					
	BOOL		m_bStack;						
	char		m_szScript[SZBUFLEN_1];		
	int			m_Magic[5];
	int			m_nDelet;
} KBASICPROP_MINE;

typedef struct
{
	int			nAttrib;
	int			nValue;
	int			nTime;
} KMEDATTRIB;

typedef struct
{
	char		m_szName[SZBUFLEN_0];		// Ãû³Æ
	int			m_nItemGenre;				// µÀ¾ßÖÖÀà
	int			m_nDetailType;				// ¾ßÌåÀà±ð
	int			m_nParticularType;			// ÏêÏ¸Àà±ð
	char		m_szImageName[SZBUFLEN_0];	// ½çÃæÖÐµÄ¶¯»­ÎÄ¼þÃû
	int			m_nObjIdx;					// ¶ÔÓ¦Îï¼þË÷Òý
	int			m_nWidth;					// µÀ¾ßÀ¸ÖÐËùÕ¼¿í¶È
	int			m_nHeight;					// µÀ¾ßÀ¸ÖÐËùÕ¼¸ß¶È
	char		m_szIntro[SZBUFLEN_1];		// ËµÃ÷ÎÄ×Ö
	int			m_nSeries;					// ÎåÐÐÊôÐÔ
	int			m_nPrice;					// ¼Û¸ñ
	int			m_nLevel;					// µÈ¼¶
	BOOL		m_bStack;					// ÊÇ·ñ¿Éµþ·Å
	KMEDATTRIB	m_aryAttrib[2];				// Ò©Æ·µÄÊôÐÔ
} KBASICPROP_MEDICINE;

typedef struct
{
	char		m_szName[SZBUFLEN_0];		// Ãû³Æ
	int			m_nItemGenre;				// µÀ¾ßÖÖÀà
	int			m_nDetailType;				// ¾ßÌåÀà±ð
	int			m_nParticularType;			// ÏêÏ¸Àà±ð
	char		m_szImageName[SZBUFLEN_0];	// ½çÃæÖÐµÄ¶¯»­ÎÄ¼þÃû
	int			m_nObjIdx;					// ¶ÔÓ¦Îï¼þË÷Òý
	int			m_nWidth;					// µÀ¾ßÀ¸ÖÐËùÕ¼¿í¶È
	int			m_nHeight;					// µÀ¾ßÀ¸ÖÐËùÕ¼¸ß¶È
	char		m_szIntro[SZBUFLEN_1];		// ËµÃ÷ÎÄ×Ö
	int			m_nSeries;					// ÎåÐÐÊôÐÔ
	int			m_nPrice;					// ¼Û¸ñ
	int			m_nLevel;					// µÈ¼¶
	BOOL		m_bStack;					// ÊÇ·ñ¿Éµþ·Å
	int			m_nAttrib1_Type;			// ÊôÐÔ1ÀàÐÍ
	int			m_nAttrib1_Para;			// ÊôÐÔ1²ÎÊý
	int			m_nAttrib2_Type;			// ÊôÐÔ2ÀàÐÍ
	int			m_nAttrib2_Para;			// ÊôÐÔ2²ÎÊý
	int			m_nAttrib3_Type;			// ÊôÐÔ2ÀàÐÍ
	int			m_nAttrib3_Para;			// ÊôÐÔ2²ÎÊý
} KBASICPROP_MEDMATERIAL;

typedef struct
{
	int			nMin;
	int			nMax;
} KMINMAXPAIR;

typedef struct
{
	int			nType;						// ÊôÐÔÀàÐÍ
	KMINMAXPAIR	sRange;						// È¡Öµ·¶Î§
} KEQCP_BASIC;	// Equipment_CorePara_Basic

typedef struct
{
	int			nType;						// ÊôÐÔÀàÐÍ
	int			nPara;						// ÊýÖµ
} KEQCP_REQ;	// Equipment_CorePara_Requirment

typedef struct
{
	int			nPropKind;					// ÐÞ¸ÄµÄÊôÐÔÀàÐÍ£¨¶ÔÍ¬Ò»¸öÊýÖµ¼Ó°Ù·Ö±ÈºÍ¼ÓµãÊý±»ÈÏÎªÊÇÁ½¸öÊôÐÔ£©
	KMINMAXPAIR	aryRange[3];				// ÐÞ¸ÄÊôÐÔËùÐèµÄ¼¸¸ö²ÎÊý
} KMACP;	// MagicAttrib_CorePara

// Add by Freeway Chen in 2003.5.30
#define			MATF_CBDR		    11      // ÎïÆ·ÀàÐÍ type(ÏÖÔÚµÄÖµÎª equip_detailnum)
#define         MATF_PREFIXPOSFIX   2       // Ç°×ººó×º
#define         MATF_SERIES         5       // ÎåÐÐ
#define         MATF_LEVEL          10      // ×î¶àÓÐ10¸ö¼¶±ð

typedef struct
{
	int			m_nPos;						// Ç°×º»¹ÊÇºó×º
	char		m_szName[SZBUFLEN_0];		// Ãû³Æ
	int			m_nClass;					// ÎåÐÐÒªÇó
	int			m_nLevel;					// µÈ¼¶ÒªÇó
	char		m_szIntro[SZBUFLEN_1];		// ËµÃ÷ÎÄ×Ö
	KMACP		m_MagicAttrib;				// ºËÐÄ²ÎÊý
	int			m_DropRate[MATF_CBDR];		// ³öÏÖ¸ÅÂÊ
    //add by Freeway Chen in 2003.5.30
    int         m_nUseFlag;                 // ¸ÃÄ§·¨ÊÇ·ñ±»Ê¹ÓÃ¹ý
} KMAGICATTRIB_TABFILE;

typedef struct
{
	char		m_szName[SZBUFLEN_0];		// Ãû³Æ
	int			m_nItemGenre;				// µÀ¾ßÖÖÀà (ÎäÆ÷? Ò©Æ·? ¿óÊ¯?)
	int			m_nDetailType;				// ¾ßÌåÀà±ð
	int			m_nParticularType;			// ÏêÏ¸Àà±ð
	char		m_szImageName[SZBUFLEN_0];	// ½çÃæÖÐµÄ¶¯»­ÎÄ¼þÃû
	int			m_nObjIdx;					// ¶ÔÓ¦Îï¼þË÷Òý
	int			m_nWidth;					// µÀ¾ßÀ¸ÖÐËùÕ¼¿í¶È
	int			m_nHeight;					// µÀ¾ßÀ¸ÖÐËùÕ¼¸ß¶È
	char		m_szIntro[SZBUFLEN_1];		// ËµÃ÷ÎÄ×Ö
	int			m_nSeries;					// ÎåÐÐÊôÐÔ
	int			m_nPrice;					// ¼Û¸ñ
	int			m_nLevel;					// µÈ¼¶
	BOOL		m_bStack;					// ÊÇ·ñ¿Éµþ·Å
	KEQCP_BASIC	m_aryPropBasic[7];			// »ù´¡ÊôÐÔ
	KEQCP_REQ	m_aryPropReq[6];			// ÐèÇóÊôÐÔ
} KBASICPROP_EQUIPMENT;

typedef struct
{
	char		m_szName[SZBUFLEN_0];		
	int			m_nItemGenre;				
	int			m_nDetailType;				
	int			m_nParticularType;			
	char		m_szImageName[SZBUFLEN_0];	
	BOOL		m_bStack;					
	int			m_nObjIdx;					
	int			m_nWidth;					
	int			m_nHeight;					
	char		m_szIntro[SZBUFLEN_1];		
	int			m_nSeries;					
	int			m_nPrice;					
	int			m_nLevel;						
	KEQCP_BASIC	m_aryPropBasic[7];			
	KEQCP_REQ	m_aryPropReq[6];			
	int			m_aryMagicAttribs[6];		
	int			m_nId;						
	int			m_nSet;						
	int			m_nSetId;					
	int			m_nSetNum;					
	int			m_nUpSet;					
} KBASICPROP_EQUIPMENT_GOLD;

typedef struct
{
	char		m_szName[SZBUFLEN_0];		// Ãû³Æ
	int			m_nItemGenre;				// µÀ¾ßÖÖÀà
	int			m_nDetailType;				// ¾ßÌåÀà±ð
	char		m_szImageName[SZBUFLEN_0];	// ½çÃæÖÐµÄ¶¯»­ÎÄ¼þÃû
	int			m_nObjIdx;					// ¶ÔÓ¦Îï¼þË÷Òý
	int			m_nWidth;					// µÀ¾ßÀ¸ÖÐËùÕ¼¿í¶È
	int			m_nHeight;					// µÀ¾ßÀ¸ÖÐËùÕ¼¸ß¶È
	char		m_szIntro[SZBUFLEN_1];		// ËµÃ÷ÎÄ×Ö
	char		m_szScript[SZBUFLEN_1];		// Ö´ÐÐ½Å±¾
	int			m_nPrice;
	int			m_nPriceXu;	
	int			m_nDelet;
	int			m_nIsSell;
	int			m_nIsTrade;
	int			m_bShortKey;	//bá xuèng « phÝm t¾t
	int			m_bStack;
} KBASICPROP_QUEST;

typedef struct
{
	char		m_szName[SZBUFLEN_0];		// Ãû³Æ
	int			m_nItemGenre;				// µÀ¾ßÖÖÀà
	char		m_szImageName[SZBUFLEN_0];	// ½çÃæÖÐµÄ¶¯»­ÎÄ¼þÃû
	int			m_nObjIdx;					// ¶ÔÓ¦Îï¼þË÷Òý
	int			m_nWidth;					// µÀ¾ßÀ¸ÖÐËùÕ¼¿í¶È
	int			m_nHeight;					// µÀ¾ßÀ¸ÖÐËùÕ¼¸ß¶È
	int			m_nPrice;					// ¼Û¸ñ
	char		m_szIntro[SZBUFLEN_1];		// ËµÃ÷ÎÄ×Ö
} KBASICPROP_TOWNPORTAL;

typedef struct
{
	char		m_szName[SZBUFLEN_0];		
	int			m_nItemGenre;				
	int			m_nDetailType;			
	int			m_nParticurType;				
	char		m_szImageName[SZBUFLEN_0];	
	int			m_nObjIdx;					
	int			m_nWidth;					
	int			m_nHeight;					
	char		m_szIntro[SZBUFLEN_1];		
	char		m_szScript[SZBUFLEN_1];		
	int			m_nPrice;					
	int			m_bShortKey;	//bá xuèng « phÝm t¾t
	int			m_nMaxStack;
	int			m_nPickExecute; //add by phong kiÒu item sö dông ngay
	int			m_nParam; //sè lÇn sö dông item
} KBASICPROP_MAGICSCRIPT;

class KBasicPropertyTable		
{
public:
	KBasicPropertyTable();
	~KBasicPropertyTable();

protected:
	void*		m_pBuf;						
	int			m_nNumOfEntries;			
    int         m_nSizeOfEntry;				
	char		m_szTabFile[MAX_PATH];	

public:
	virtual BOOL Load();
	int NumOfEntries() const { return m_nNumOfEntries; }

protected:
	BOOL GetMemory();
	void ReleaseMemory();
	void SetCount(int);
	virtual BOOL LoadRecord(int i, KTabFile* pTF) = 0;
};

class KBPT_Mine : public KBasicPropertyTable
{
public:
	KBPT_Mine();
	~KBPT_Mine();

public:
	const KBASICPROP_MINE* GetRecord(IN int) const;
	const KBASICPROP_MINE* FindRecord(IN int, IN int) const;

protected:
	virtual BOOL LoadRecord(int i, KTabFile* pTF);
};

class KBPT_Medicine : public KBasicPropertyTable
{
public:
	KBPT_Medicine();
	~KBPT_Medicine();

public:
	const KBASICPROP_MEDICINE* GetRecord(IN int) const;
	const KBASICPROP_MEDICINE* FindRecord(IN int, IN int) const;

protected:
	virtual BOOL LoadRecord(int i, KTabFile* pTF);
};

class KBPT_Quest : public KBasicPropertyTable
{
public:
	KBPT_Quest();
	~KBPT_Quest();

public:
	const KBASICPROP_QUEST* GetRecord(IN int) const;
	const KBASICPROP_QUEST* FindRecord(IN int) const;

protected:
	virtual BOOL LoadRecord(int i, KTabFile* pTF);
};

class KBPT_MagicScript : public KBasicPropertyTable
{
public:
	KBPT_MagicScript();
	~KBPT_MagicScript();

public:
	const KBASICPROP_MAGICSCRIPT* GetRecord(IN int) const;
	const KBASICPROP_MAGICSCRIPT* FindRecord(IN int) const;

protected:
	virtual BOOL LoadRecord(int i, KTabFile* pTF);
};

class KBPT_TownPortal : public KBasicPropertyTable
{
public:
	KBPT_TownPortal();
	~KBPT_TownPortal();

public:
	const KBASICPROP_TOWNPORTAL* GetRecord(IN int) const;

protected:
	virtual BOOL LoadRecord(int i, KTabFile* pTF);
};

class KBPT_MedMaterial : public KBasicPropertyTable
{
public:
	KBPT_MedMaterial();
	~KBPT_MedMaterial();

public:
	const KBASICPROP_MEDMATERIAL* GetRecord(IN int) const;

protected:
	virtual BOOL LoadRecord(int i, KTabFile* pTF);
};

class KBPT_Equipment : public KBasicPropertyTable
{
public:
	KBPT_Equipment();
	~KBPT_Equipment();

public:
	const KBASICPROP_EQUIPMENT* GetRecord(IN int) const;
	const KBASICPROP_EQUIPMENT* FindRecord(IN int, IN int, IN int) const;
	void Init(IN int);
protected:
	virtual BOOL LoadRecord(int i, KTabFile* pTF);
};

class KBPT_Equipment_Gold : public KBasicPropertyTable
{
public:
	KBPT_Equipment_Gold();
	virtual ~KBPT_Equipment_Gold();

public:
	const KBASICPROP_EQUIPMENT_GOLD* GetRecord(IN int) const;
	const KBASICPROP_EQUIPMENT_GOLD* FindRecord(IN int, IN int, IN int) const;
	int GetRecordCount() const {return KBasicPropertyTable::NumOfEntries();};
	void Init();
protected:
	virtual BOOL LoadRecord(int i, KTabFile* pTF);
};

class KBPT_MagicAttrib_TF : public KBasicPropertyTable
{
public:
	KBPT_MagicAttrib_TF();
	~KBPT_MagicAttrib_TF();

protected:
	int m_naryMACount[2][MATF_CBDR];	
public:
	void GetMACount(int*) const;
	const KMAGICATTRIB_TABFILE* GetRecord(IN int) const;

protected:
	virtual BOOL LoadRecord(int i, KTabFile* pTF);
	void Init();
};

class KBPT_ClassMAIT    // Magic Item Index Table
{
public:
	KBPT_ClassMAIT();
	~KBPT_ClassMAIT();

protected:
	int*	m_pnTable;			
	int		m_nSize;			
	int		m_nNumOfValidData;		
public:
    BOOL Clear();
	BOOL Insert(int nItemIndex);
	int  Get(int i) const;
    int  GetCount() const { return m_nNumOfValidData; }
};

class KBPT_ClassifiedMAT
{
public:
	KBPT_ClassifiedMAT();
	~KBPT_ClassifiedMAT();

protected:
	int*	m_pnTable;				
	int		m_nSize;		
	int		m_nNumOfValidData;	
public:
	BOOL GetMemory(int);
	BOOL Set(int);
	int Get(int) const;
	BOOL GetAll(int*, int*) const;

protected:
	void ReleaseMemory();
};

class KLibOfBPT
{
public:
	KLibOfBPT();
	~KLibOfBPT();

protected:
	KBPT_Medicine			m_BPTMedicine;
	KBPT_TownPortal			m_BPTTownPortal;
	KBPT_MagicScript		m_BPTMagicScript;
	KBPT_Quest				m_BPTQuest;
	KBPT_Mine				m_BPTMine;
	KBPT_Equipment			m_BPTHorse;
	KBPT_Equipment			m_BPTMeleeWeapon;
	KBPT_Equipment			m_BPTRangeWeapon;
	KBPT_Equipment			m_BPTArmor;
	KBPT_Equipment			m_BPTHelm;
	KBPT_Equipment			m_BPTBoot;
	KBPT_Equipment			m_BPTBelt;
	KBPT_Equipment			m_BPTAmulet;
	KBPT_Equipment			m_BPTRing;
	KBPT_Equipment			m_BPTCuff;
	KBPT_Equipment			m_BPTPendant;
	KBPT_Equipment			m_BPTMask;	// mat na
	KBPT_Equipment			m_BPTMantle; //phi phong
    KBPT_MagicAttrib_TF		m_BPTMagicAttrib;
	// Add by flying
	KBPT_Equipment_Gold		m_GoldItem;
    // Add by Freeway Chen in 2003.5.30
    KBPT_ClassMAIT          m_CMAIT[MATF_PREFIXPOSFIX][MATF_CBDR][MATF_SERIES][MATF_LEVEL];
	KBPT_ClassifiedMAT		m_CMAT[2][MATF_CBDR];

public:
	BOOL Init();

	const KMAGICATTRIB_TABFILE* GetMARecord(IN int) const;
	const int					GetMARecordNumber() const;
    // Add by Freeway Chen in 2003.5.30
    const KBPT_ClassMAIT*       GetCMIT(IN int nPrefixPostfix, IN int nType, IN int nSeries, int nLevel) const;
	const KBPT_ClassifiedMAT*	GetCMAT(IN int, int) const;
	// Add by flying on 2003.6.2
	const KBASICPROP_EQUIPMENT_GOLD*	GetGoldItemRecord(IN int nIndex) const;
	const int							GetGoldItemNumber() const;
	const KBASICPROP_EQUIPMENT*	GetMeleeWeaponRecord(IN int) const;
	const int					GetMeleeWeaponRecordNumber() const;
	const KBASICPROP_EQUIPMENT*	GetRangeWeaponRecord(IN int) const;
	const int					GetRangeWeaponRecordNumber() const;
	const KBASICPROP_EQUIPMENT*	GetArmorRecord(IN int) const;
	const int					GetArmorRecordNumber() const;
	const KBASICPROP_EQUIPMENT*	GetHelmRecord(IN int) const;
	const int					GetHelmRecordNumber() const;
	const KBASICPROP_EQUIPMENT* GetBootRecord(IN int) const;
	const int					GetBootRecordNumber() const;
	const KBASICPROP_EQUIPMENT*	GetBeltRecord(IN int) const;
	const int					GetBeltRecordNumber() const;
	const KBASICPROP_EQUIPMENT*	GetAmuletRecord(IN int) const;
	const int					GetAmuletRecordNumber() const;
	const KBASICPROP_EQUIPMENT*	GetRingRecord(IN int) const;
	const int					GetRingRecordNumber() const;
	const KBASICPROP_EQUIPMENT*	GetCuffRecord(IN int) const;
	const int					GetCuffRecordNumber() const;
	const KBASICPROP_EQUIPMENT*	GetPendantRecord(IN int) const;
	const int					GetPendantRecordNumber() const;
	const KBASICPROP_EQUIPMENT* GetHorseRecord(IN int) const;
	const int					GetHorseRecordNumber() const;
	const KBASICPROP_MEDICINE*	GetMedicineRecord(IN int) const;
	const int					GetMedicineRecordNumber() const;
	const KBASICPROP_QUEST*		GetQuestRecord(IN int) const;
	const int					GetQuestRecordNumber() const;
	const KBASICPROP_TOWNPORTAL*	GetTownPortalRecord(IN int) const;
	const int					GetTownPortalRecordNumber() const;
	const KBASICPROP_MINE*		GetMine(IN int) const;
	const int					GetMineRecordNumber() const;
	const KBASICPROP_EQUIPMENT* GetMaskRecord(IN int) const;	// mat na
	const int					GetMaskRecordNumber() const;	// mat na
	const KBASICPROP_MAGICSCRIPT*	GetMagicScript(IN int) const;
	const int					GetMagicScriptRecordNumber() const;
	const KBASICPROP_EQUIPMENT*	GetMantleRecord(IN int) const;//#phi phong
	const int					GetMantleRecordNumber() const;

// 	const KBASICPROP_EQUIPMENT_GOLD*	GetGoldRecord(IN int) const;
// 	const int					GetGoldRecordNumber(IN int) const;

protected:
	BOOL InitMALib();
    // Add by Freeway Chen in 2003.5.30
    BOOL InitMAIT();
};
#endif		// #ifndef KBasPropTblH
