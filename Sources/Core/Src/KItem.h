//---------------------------------------------------------------------------
// Sword3 Core (c) 2002 by Kingsoft
//
// File:	KItem.h
// Date:	2021.08
// Code:	Fong KiÒu
// Desc:	Header File
//---------------------------------------------------------------------------

#ifndef	KItemH
#define	KItemH

#include	"KBasPropTbl.h"
#include	"KMagicAttrib.h"
#include	"GameDataDef.h"
#ifndef _SERVER
#include	 "../../Represent/iRepresent/KRepresentUnit.h"
#endif
#include	"KTabFile.h"
#include	"KSG_StringProcess.h"
#include "time.h"
class KPlayer;
class KIniFile;

#define		IN
#define		OUT

#ifdef _SERVER
#define		MAX_ITEM	10000		//gioi han item idx trong server
#else
#define		MAX_ITEM	512				//gioi han item set trong 1 player
#endif
#define		KItemNormalAttrib KMagicAttrib

typedef struct
{
	int		nItemGenre;				
	int		nDetailType;			
	int		nParticularType;		
	int		nObjIdx;				
	BOOL	bStack;					
	int		nWidth;					
	int		nHeight;	
	int		nPrice;					
	int		nNewPrice;
	int		nLevel;					
	int		nSeries;				
	char	szItemName[80];			
	int 	nColor;
#ifndef _SERVER
	char	szImageName[80];		
	char	szIntro[256];			
	int		uPrice;
#endif
	char	szScript[256];	
	int		nNpcImage;				
	int		nSet;					
	int		nSetId;	
	int		nSetNum;				
	int		nBigSet;				
	int		nGoldId;				
	int		nStackNum;
	int		nMaxStack;
	int		nEnChance;
	int		nPoint;					
	KTime	LimitTime;
	int		nIsSell;
	int		nIsTrade;
	int		nPriceXu;
	int		nItemNature;
	BOOL	bNewArrival;
	int		bShortKey;
	BYTE	nMantle;
	int			nPickExecute;//add by phong kiÒu item sö dông ngay
	int			nParam;//sè lÇn sö dông item
	int			nGlowLight; //vò khÝ ph¸t s¸ng ngo¹i trang
} KItemCommonAttrib;

typedef struct
{
	UINT	uRandomSeed;
	int		nGeneratorLevel[6];
	int		nVersion;
	int		nLuck;
} KItemGeneratorParam;

class KNpc;

class KItem
{
public:
	KItem();
	~KItem();

public:
	KItemCommonAttrib	m_CommonAttrib;			// ¸÷ÖÖµÀ¾ßµÄ¹²Í¬ÊôÐÔ
	KItemNormalAttrib	m_aryRequireAttrib[6];	// µÀ¾ßµÄÐèÇóÊôÐÔ
	int		InsuranceCourse;									//tr¹ng th¸i kho¸
	int		InsuranceHourCourse;							//thêi gian kho¸
	int		nExpPointSec;

public:
	KItemGeneratorParam	m_GeneratorParam;		// µÀ¾ßµÄÉú³É²ÎÊý
	KItemNormalAttrib	m_aryMagicAttrib[6];	// µÀ¾ßµÄÄ§·¨ÊôÐÔ
	KItemNormalAttrib	m_aryBaseAttrib[7];		// µÀ¾ßµÄ»ù´¡ÊôÐÔ
private:
	int		m_nIndex;							
	DWORD	m_dwID;								
	int		m_nCurrentDur;						
#ifndef _SERVER
	KRUImage	m_Image;
#endif

public:
	void	ApplyMagicAttribToNPC(IN KNpc*, IN int = 0) const;
	void	RemoveMagicAttribFromNPC(IN KNpc*, IN int = 0) const;
	void	ApplyHiddenMagicAttribToNPC(IN KNpc*, IN int) const;
	void	RemoveHiddenMagicAttribFromNPC(IN KNpc*, IN int) const;
	KItemGeneratorParam * GetItemParam(){return &m_GeneratorParam;};
	void	SetID(DWORD dwID) { m_dwID = dwID; };
	DWORD	GetID() const { return m_dwID; };
	int		GetIndex() const { return m_nIndex; };
	BYTE	GetKind(); 
	BOOL	GetPickExecute() { return m_CommonAttrib.nPickExecute;};//add by phong kiÒu item sö dông ngay
	int		GetLine() const { return m_CommonAttrib.nGoldId; };
	int		GetDetailType() const { return m_CommonAttrib.nDetailType; };
	int		GetNature() const { return m_CommonAttrib.nItemNature; };
	int		GetGenre() const { return m_CommonAttrib.nItemGenre; };
	int		GetIsSell() const { return m_CommonAttrib.nIsSell; };
	int		GetIsTrade() const { return m_CommonAttrib.nIsTrade; };
	int		GetSeries() const { return m_CommonAttrib.nSeries; };
	int		GetParticular() { return m_CommonAttrib.nParticularType; };
	BOOL		GetNewArrival() { return m_CommonAttrib.bNewArrival; };
	BOOL		GetBShortKey() const { return m_CommonAttrib.bShortKey; };
	void			SetItemBroken()
	{
		m_CommonAttrib.nWidth = 1;
		m_CommonAttrib.nHeight = 1;
	}
	int		GetLevel() { return m_CommonAttrib.nLevel; };
	void	SetSeries(int nSeries) { m_CommonAttrib.nSeries = nSeries; };
	int		GetWidth() const { return m_CommonAttrib.nWidth; };
	int		GetHeight() const { return m_CommonAttrib.nHeight; };
	int		GetPrice() const { return m_CommonAttrib.nPrice; };
	int		GetOrgPrice() const { return m_CommonAttrib.nPrice; };
	int		GetSalePrice();
	void	SetNewPrice(int uPrice) {m_CommonAttrib.nNewPrice = uPrice;};
	int		GetNewPrice(){ return m_CommonAttrib.nNewPrice; };
	int		GetCurPrice() const 
	{
		if (m_CommonAttrib.nNewPrice > 0)
			return m_CommonAttrib.nNewPrice;
		else
			return m_CommonAttrib.nPrice; 
	};
	int		GetColorItem();
	int		CheckSubStrInText(char* pstart, char* strItemInfo, int nLen, int sublen);
	bool	IsFkItemSkill()
	{
		if(m_CommonAttrib.nItemGenre == item_medicine && m_CommonAttrib.nDetailType == 13)
			return true;
		return false;
	};
	//
#ifndef _SERVER
	char*	GetImageName() const // post item
	{ 
		if(m_nCurrentDur == 0)
			return (char*)BROKEN_ITEM_SPR36;		//#trang bÞ háng
		return (char*)m_CommonAttrib.szImageName;
	};
#endif
	//
	char*	GetName() const { return (char *)m_CommonAttrib.szItemName; };
	int		GetObjIdx() { return m_CommonAttrib.nObjIdx;};
	void*	GetRequirement(IN int);
	int		GetMaxDurability();
	int		GetTotalMagicLevel();
	int		GetRepairPrice();
	void	Remove();
	BOOL	SetBaseAttrib(IN const KItemNormalAttrib*);
	BOOL	SetRequireAttrib(IN const KItemNormalAttrib*);
	BOOL	SetMagicAttrib(IN const KItemNormalAttrib*);
	void	SetDurability(IN const int nDur) 
	{ 
		m_nCurrentDur = nDur;
		if(nDur == 0)
			SetItemBroken();
	};
	int		GetDurability() { return m_nCurrentDur; };
	KItemGeneratorParam*	GetGeneratorParam() { return &m_GeneratorParam; }
	int		Abrade(IN const int nAbradeP, IN const int nRange);
	BOOL	CanBeRepaired();
	int		GetSet() const { return m_CommonAttrib.nSet; };
	int		GetSetId() const { return m_CommonAttrib.nSetId; };
	int		GetSetNum() const { return m_CommonAttrib.nSetNum; };
	int		GetGoldId() const { return m_CommonAttrib.nGoldId; };
	int		GetBigSet() const { return m_CommonAttrib.nBigSet; };
	int 	IsPurple();
	BOOL	CanStack( int nOldIdx);
	BOOL	CanStack();
	int			Stack( int nIdx ); 
	int			GetStackNum() const { return m_CommonAttrib.nStackNum; };
	void	SetStackNum(int nNum) {m_CommonAttrib.nStackNum = nNum;};
	void	SetPlayerItemLock(int nLock) { InsuranceCourse = nLock;};
	int		  GetPlayerItemLock() 
	{
		return InsuranceCourse;
	};
	void	SetPlayerItemHLock(int Hour) {InsuranceHourCourse = Hour;};
	int		  GetPlayerItemHLock() {return InsuranceHourCourse;};
	BOOL	GetPlayerItemIsHoureOpen()
	{
		if(InsuranceHourCourse != 0 && time(0) > InsuranceHourCourse)
			return TRUE;
		else 
			return FALSE;
	};
	void	SetExpirePoint(int	sec) { nExpPointSec = sec; };
	int		  GetExpirePoint() { return nExpPointSec;};
	int		 GetEnChance() const { return m_CommonAttrib.nEnChance; };
	int		 EnChance(int nEnChance = 1);
	int		GetBaseMagic()  const { return m_aryBaseAttrib[0].nValue[0]; };
	char*	GetScript() const { return (char *)m_CommonAttrib.szScript; };
	void	SetLevel(int i) { m_CommonAttrib.nLevel = i;};
	int		GetAttribType( int i = 0) const { return m_aryMagicAttrib[i].nAttribType; };
	void		SetPoint(int i) { m_CommonAttrib.nPoint = i;};
	void	SetGoodPrice(int uPrice) {m_CommonAttrib.nPrice = uPrice;};
	void	SetExpTime(int bYear,BYTE bMonth,BYTE bDay,BYTE bHour);
	KTime*	GetTime() { return & m_CommonAttrib.LimitTime; }; 
	BOOL	GetTimeYearIsExp()																					//vat pham het han
	{
		if(m_CommonAttrib.LimitTime.bYear)
		{
			if(time(0) > m_CommonAttrib.LimitTime.bYear)
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}; 
	BOOL	IsStack() const {
		if (m_CommonAttrib.nStackNum <= 1)
			return FALSE;
		else
			return TRUE;
	};

	int		GetMantle(){ return m_CommonAttrib.nMantle; };
	void	SetMantle(int n){m_CommonAttrib.nMantle = n;};

	void	SetParam(int i) { m_CommonAttrib.nParam = i;};
	int		GetParam() {return m_CommonAttrib.nParam;};

	void	SetItemGlowLight(int i) { m_CommonAttrib.nGlowLight = i;};
	int		GetItemGlowLight() {return m_CommonAttrib.nGlowLight;};
	
	BOOL	HaveMaigc(int nAttribe,int nValue1Min,int nValue1Max,int nValue2Min,int nValue2Max,int nValue3Min,int nValue3Max);
	int		GetBaseMagicAttribType(int i = 0)  const { return m_aryBaseAttrib[i].nAttribType; };
#ifndef _SERVER
	void	SetPrice(int uPrice) {m_CommonAttrib.uPrice = uPrice;};
	int		GetSetPrice() {return m_CommonAttrib.uPrice;};
	void	PaintItem(int nX, int nY, bool bResize = false, bool bPaintStack = true, unsigned int sidx = 0);
	void	GetDesc(char* pszMsg, bool bShowPrice = false, int nPriceScale = 1, int nActiveAttrib = 0);
#endif

friend class	KItemGenerator;
friend class	KPlayer;
friend class	KItemList;
private:
	BOOL SetAttrib_CBR(IN const KBASICPROP_EQUIPMENT*);
	BOOL SetAttrib_CBR(IN const KBASICPROP_EQUIPMENT_GOLD*);
	BOOL SetAttrib_MA(IN const KItemNormalAttrib*);
	BOOL SetAttrib_MA(IN const KMACP*);
	void operator = (const KBASICPROP_EQUIPMENT&);
	void operator = (const KBASICPROP_MEDMATERIAL&);
	void operator = (const KBASICPROP_MINE&);
	void operator = (const KBASICPROP_MEDICINE&);
	void operator = (const KBASICPROP_QUEST&);
	void operator = (const KBASICPROP_TOWNPORTAL&);
	void operator = (const KBASICPROP_MAGICSCRIPT&);
	void operator = (const KBASICPROP_EQUIPMENT_GOLD&);

private:
	BOOL SetAttrib_Base(const KEQCP_BASIC*);
	BOOL SetAttrib_Req(const KEQCP_REQ*);
};

extern KItem Item[MAX_ITEM];

#endif
