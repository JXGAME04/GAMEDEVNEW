//---------------------------------------------------------------------------
// Sword3 Core (c) 2002 by Kingsoft
//
// File:	KBasPropTbl.h
// Date:	2020.08.14
// Code:	Fong Ki
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

// ÒÔÏÂ½á¹¹ÓÃÓÚÃèÊö¿óÊ¯µÄ»ù±¾ÊôÐÔ. Ïà¹ØÊôÐÔÓÉÅäÖÃÎÄ¼þ(tab file)Ìá¹©
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
	char		m_szScript[SZBUFLEN_1];		// ËµÃ÷ÎÄ×Ö
	int			m_nPrice;					// ¼Û¸ñ
	BOOL		m_bShortKey;
	int			m_nMaxStack;
} KBASICPROP_EVENTITEM;

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
	int			mSuiteNo;
	int			nActiveCount1;
	int			nActiveCount2;
} KSUITE_ACTIVE_COUNT;

typedef struct
{
	//PlatinaItem	OldLevel	MaxDurability	Comment
	int m_nPlatinaItem;
	int m_nOldLevel;
	int m_nMaxDurability;
} KPLATINA_DURABILITY;

typedef struct
{
	//AttributeType	Value1	Value2	Value3	Comment
	int m_nAttributeType;
	int m_nValue1;
	int m_nValue2;
	int m_nValue3;
} KPLATINA_MAGICATTR;

typedef struct
{
	//PlatinaItem	Level	SkillNo	ActiveRate	Rate1	MagicIdx1	Rate2	MagicIdx2	Rate3	MagicIdx3	Comment
	int m_nPlatinaItem;
	int m_nLevel;
	int m_nSkillNo;
	int m_nActiveRate;
	int m_nRate1;
	int m_nMagicIdx1;
	int m_nRate2;
	int m_nMagicIdx2;
	int m_nRate3;
	int m_nMagicIdx3;
} KPLATINA_MAGICRATE;

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
	int			m_aryMagicAttribs[MAX_ITEM_MAGICATTRIB];
	int			m_nId;						
	int			m_nSet;						
	int			m_nSetId;					
	int			m_nSetNum;					
	int			m_nUpSet;					
} KBASICPROP_EQUIPMENT_GOLD;

typedef struct
{
	char		m_szName[SZBUFLEN_0];		// Ãû³Æ
	int			m_nItemGenre;				// µÀ¾ßÖÖÀà (ÎäÆ÷? Ò©Æ·? ¿óÊ¯?)
	int			m_nDetailType;				// ¾ßÌåÀà±ð
	int			m_nParticularType;			// ÏêÏ¸Àà±ð
	char		m_szImageName[SZBUFLEN_0];	// ½çÃæÖÐµÄ¶¯»­ÎÄ¼þÃû
	int			m_nObjIdx;					// ¶ÔÓ¦Îï¼þË÷Òý
	int			m_nWidth;					// ÎïÆ·À¸¿í¶È
	int			m_nHeight;					// ÎïÆ·À¸¸ß¶È
	char		m_szIntro[SZBUFLEN_1];		// ËµÃ÷ÎÄ×Ö
	int			m_nSeries;					// ÎåÐÐÊôÐÔ
	int			m_nPrice;					// ¼Û¸ñ
	int			m_nLevel;					// µÈ¼¶	
	int			m_nStack;
	KEQCP_BASIC	m_aryPropBasic[7];			// »ù´¡ÊôÐÔ
	KEQCP_REQ	m_aryPropReq[6];			// ÐèÇóÊôÐÔ
	int			m_aryMagicAttribs[MAX_ITEM_MAGICATTRIB];		// Ä§·¨ÊôÐÔ
	int			m_nGroup;						// ËùÔÚÌ××°
	int			m_nSetID;					// ËùÊôÐòºÅ
	int			m_nSetIDNo;
	int			m_nNeedToActive1;					// À©Õ¹Ì××°
	int			m_nNeedToActive2;					// À©Õ¹Ì××°
} KBASICPROP_EQUIPMENT_GOLD2;

// ÒÔÏÂ½á¹¹ÓÃÓÚÃèÊöÎ¨Ò»×°±¸µÄ³õÊ¼ÊôÐÔ. Ïà¹ØÊý¾ÝÓÉÅäÖÃÎÄ¼þ(tab file)Ìá¹©
typedef struct
{
	char		m_szName[SZBUFLEN_0];		// Ãû³Æ
	int			m_nItemGenre;				// µÀ¾ßÖÖÀà (ÎäÆ÷? Ò©Æ·? ¿óÊ¯?)
	int			m_nDetailType;				// ¾ßÌåÀà±ð
	int			m_nParticularType;			// ÏêÏ¸Àà±ð
	char		m_szImageName[SZBUFLEN_0];	// ½çÃæÖÐµÄ¶¯»­ÎÄ¼þÃû
	int			m_nObjIdx;					// ¶ÔÓ¦Îï¼þË÷Òý
	int			m_nWidth;					// ÎïÆ·À¸¿í¶È
	int			m_nHeight;					// ÎïÆ·À¸¸ß¶È
	char		m_szIntro[SZBUFLEN_1];		// ËµÃ÷ÎÄ×Ö
	int			m_nSeries;					// ÎåÐÐÊôÐÔ
	int			m_nPrice;					// ¼Û¸ñ
	int			m_nLevel;					// µÈ¼¶	
	int			m_nStack;
	KEQCP_BASIC	m_aryPropBasic[7];			// »ù´¡ÊôÐÔ
	KEQCP_REQ	m_aryPropReq[6];			// ÐèÇóÊôÐÔ
	int			m_aryMagicAttribs_0[MAX_ITEM_MAGICATTRIB];		// Ä§·¨ÊôÐÔ
	int			m_aryMagicAttribs_10[MAX_ITEM_MAGICATTRIB];		// Ä§·¨ÊôÐÔ
	int			m_nGroup;						// ËùÔÚÌ××°
	int			m_nSetID;					// ËùÊôÐòºÅ
	int			m_nBuffSkillLevel6;
	int			m_nBuffSkillLevel10;
	int			m_nExpandCode;
	int			m_nSetIDNo;
	int			m_nNeedToActive1;					// À©Õ¹Ì××°
	int			m_nNeedToActive2;					// À©Õ¹Ì××°
} KBASICPROP_EQUIPMENT_PLATINA;

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
	// [LOREN 27/08] 128 byte KHONG du: Intro cua khoang thuoc tinh o ban goc
	// (Linux 004 + client VLTK) dai 186..191 byte. KTabFile cat o byte 127,
	// cat trung giua mot the "<color=...>" -> the ho -> bo phan tich the di
	// tim '>' ra ngoai pham vi. Dich chep sang la szIntro[256] (KItem.h:64)
	// nen du cho; nap dung sizeof() nen tu dong nap du hon.
	char		m_szIntro[SZBUFLEN_256];	
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

class KBPT_Event : public KBasicPropertyTable
{
public:
	KBPT_Event();
	~KBPT_Event();

public:
	const KBASICPROP_EVENTITEM* GetRecord(IN int) const;
	const KBASICPROP_EVENTITEM* FindRecord(IN int) const;
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


class KBPT_ClassSuite : public KBasicPropertyTable
{
public:
	KBPT_ClassSuite();
	~KBPT_ClassSuite();

public:
	const KSUITE_ACTIVE_COUNT* GetRecord(IN int) const;
	const KSUITE_ACTIVE_COUNT* FindRecord(IN int) const;
	void Init();
	void Init_Ext();
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

class KBPT_Equipment_Gold2 : public KBasicPropertyTable
{
public:
	KBPT_Equipment_Gold2();
	virtual ~KBPT_Equipment_Gold2();

public:
	const KBASICPROP_EQUIPMENT_GOLD2* GetRecord(IN int) const;
	const KBASICPROP_EQUIPMENT_GOLD2* FindRecord(IN int, IN int, IN int) const;
	int GetRecordCount() const { return KBasicPropertyTable::NumOfEntries(); };
	void Init();
protected:
	virtual BOOL LoadRecord(int i, KTabFile* pTF);
};

class KBPT_Equipment_Platina : public KBasicPropertyTable
{
public:
	KBPT_Equipment_Platina();
	virtual ~KBPT_Equipment_Platina();

	// ÒÔÏÂÊÇ¶ÔÍâ½Ó¿Ú
public:
	const KBASICPROP_EQUIPMENT_PLATINA* GetRecord(IN int) const;
	const KBASICPROP_EQUIPMENT_PLATINA* FindRecord(IN int, IN int, IN int) const;
	int GetRecordCount() const { return KBasicPropertyTable::NumOfEntries(); };
	void Init();
	// ÒÔÏÂÊÇ¸¨Öúº¯Êý
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

class KBPT_ClassPlatinaDurability : public KBasicPropertyTable
{
public:
	KBPT_ClassPlatinaDurability();
	~KBPT_ClassPlatinaDurability();

public:
	const KPLATINA_DURABILITY* GetRecord(IN int) const;
	const KPLATINA_DURABILITY* FindRecord(IN int) const;
	void Init();
	void Init_Ext();
protected:
	virtual BOOL LoadRecord(int i, KTabFile* pTF);
};

class KBPT_ClassPlatinaMagicAttr : public KBasicPropertyTable
{
public:
	KBPT_ClassPlatinaMagicAttr();
	~KBPT_ClassPlatinaMagicAttr();

public:
	const KPLATINA_MAGICATTR* GetRecord(IN int) const;
	const KPLATINA_MAGICATTR* FindRecord(IN int) const;
	void Init();
	void Init_Ext();
protected:
	virtual BOOL LoadRecord(int i, KTabFile* pTF);
};

class KBPT_ClassPlatinaMagicRate : public KBasicPropertyTable
{
public:
	KBPT_ClassPlatinaMagicRate();
	~KBPT_ClassPlatinaMagicRate();

public:
	const KPLATINA_MAGICRATE* GetRecord(IN int) const;
	const KPLATINA_MAGICRATE* FindRecord(IN int, IN int, IN int) const;
	void Init();
	void Init_Ext();
protected:
	virtual BOOL LoadRecord(int i, KTabFile* pTF);
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
	KBPT_Event				m_BPTEvent;
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
	KBPT_Equipment			m_BPTSignet;
	KBPT_Equipment			m_BPTShipin;
	KBPT_Equipment			m_BPTHoods;
	KBPT_Equipment			m_BPTCloak;

    KBPT_MagicAttrib_TF		m_BPTMagicAttrib;
	// Add by flying
	KBPT_Equipment_Gold		m_GoldItem;
	KBPT_Equipment_Gold2	m_BPTGoldEquip;
	KBPT_Equipment_Platina	m_BPTPlatinaEquip;
	KBPT_ClassSuite			m_ActiveSuite;
	KBPT_ClassSuite			m_ExtActiveSuite;
	KBPT_ClassPlatinaDurability	m_PlatinaDurability;
	KBPT_ClassPlatinaMagicAttr	m_PlatinaMagicAttrib;
	KBPT_ClassPlatinaMagicRate	m_PlatinaMagicRate;
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

	const KBASICPROP_EQUIPMENT_GOLD2* GetGoldEquipRecord(IN int nIndex) const;
	const int							GetGoldEquipNumber() const;

	const KBASICPROP_EQUIPMENT_PLATINA* GetPlatinaEquipRecord(IN int nIndex) const;
	const int							GetPlatinaEquipNumber() const;

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
	const KBASICPROP_EQUIPMENT* GetSignetRecord(IN int) const;
	const int					GetSignetRecordNumber() const;
	const KBASICPROP_EQUIPMENT* GetShipinRecord(IN int) const;
	const int					GetShipinRecordNumber() const;
	const KBASICPROP_EQUIPMENT* GetHoodsRecord(IN int) const;
	const int					GetHoodsRecordNumber() const;
	const KBASICPROP_EQUIPMENT* GetCloakRecord(IN int) const;
	const int					GetCloakRecordNumber() const;
	const KBASICPROP_EQUIPMENT_PLATINA* FindEquipmentUnique(IN int, IN int, IN int) const;
	const KBASICPROP_MEDICINE* FindMedicine(IN int, IN int) const;
	const KBASICPROP_EVENTITEM* GetEvent(IN int) const;
	const int					GetEventRecordNumber() const;
	 
	const KSUITE_ACTIVE_COUNT* GetActiveSuiteRecord(IN int) const;
	const KSUITE_ACTIVE_COUNT* GetExtActiveSuiteRecord(IN int) const;
	const int GetActiveSuiteCount(IN int suiteID) const;
	const int GetActiveSuiteRecordNumber() const;
	const int GetExtActiveSuiteRecordNumber() const;
	const KPLATINA_DURABILITY* GetPlatinaDurabilityRecord(IN int) const;
	const int GetPlatinaDurabilityRecordNumber() const;
	const KPLATINA_MAGICATTR* GetPlatinaMagicAttrRecord(IN int) const;
	const int GetPlatinaMagicAttrRecordNumber() const;
	const KPLATINA_MAGICRATE* GetPlatinaMagicRateFindRecord(IN int, IN int, IN int) const;
	const int GetPlatinaMagicRateRecordNumber() const;
// 	const KBASICPROP_EQUIPMENT_GOLD*	GetGoldRecord(IN int) const;
// 	const int					GetGoldRecordNumber(IN int) const;

protected:
	BOOL InitMALib();
    // Add by Freeway Chen in 2003.5.30
    BOOL InitMAIT();
};



#endif		// #ifndef KBasPropTblH
