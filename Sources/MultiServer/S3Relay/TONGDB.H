// TongDB.h: interface for the CTongDB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TONGDB_H__8C487F07_C490_4562_AD01_77953CCCBCE2__INCLUDED_)
#define AFX_TONGDB_H__8C487F07_C490_4562_AD01_77953CCCBCE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "DBTable.h"
#include "KTongControl.h"

#define MAX_TONG_COUNT 10000

/*
enum enumMemberClass		//°ïÖÚÀàĞÍ
{
	mcMaster = 0,			//°ïÖ÷
	mcDirector,			//³¤ÀÏ
	mcManager,			//³¤ÀÏ
	mcMember			//°ïÖÚ
};
*/

typedef struct
{
	int			nCamp;								// °ï»áÕóÓª
	DWORD		dwMoney;								// °ï»á×Ê½ğ
	int			nCredit;								// °ï»áÉùÍû
	int			nLevel;								// ®¼ng cÊp bang héi
	int			nRecruit;							//tr¹ng th¸i tuyÓn ng­êi

	char		szName[defTONG_STR_LENGTH];			// °ï»áÃû
	char		szPassword[defTONG_STR_LENGTH];		// °ï»áÃÜÂë

	char		szMasterTitle[defTONG_STR_LENGTH];	// °ïÖ÷³ÆºÅ
	char		szDirectorTitle[defTONG_MAX_DIRECTOR][defTONG_STR_LENGTH];// ¸÷³¤ÀÏ³ÆºÅ
	char		szManagerTitle[defTONG_MAX_MANAGER][defTONG_STR_LENGTH];	// ¸÷¶Ó³¤³ÆºÅ
	char		szNormalTitle[defTONG_STR_LENGTH];	// °ï»á°ïÖÚ³ÆºÅ
	char		szNormalBoyTitle[defTONG_STR_LENGTH];
	char		szNormalGirlTitle[defTONG_STR_LENGTH];
	//add by Fong KiÒu
	//int			 nStatusGuide;			//tr¹ng th¸i tuyÓn ng­êi bang héi
	char			szWayEdit[defTONG_STR_LENGTH];				//tiªu chİ bang héi
	char			szNextTargetEdit[defTONG_STR_LENGTH];		//môc tiªu bang héi
	int				  nExpGuide;		//®iÓm kinh nghiÖm bang héi
	int				  nCityGuide;		//thµnh thŞ chiÕm lÜnh
	//int				  nTongLevel;		//®¼ng cÊp bang héi
	char			szLeagueTName[defTONG_STR_LENGTH];	//bang liªn minh
	//end add by Fong KiÒu
}TTongStruct;	//ÓÃ×÷´æÈëÊı¾İ¿âµÄ½á¹¹

typedef struct
{
	char		szName[defTONG_STR_LENGTH];		// °ïÖÚÃû
	char		szTong[defTONG_STR_LENGTH];		// °ï»áÃû
	TONG_MEMBER_FIGURE		MemberClass;			//°ïÖÚÀàĞÍ
	int			nTitleIndex;					//°ïÖÚ³ÆºÅË÷Òı
	int			nSex;
}TMemberStruct;	//ÓÃ×÷´æÈëÊı¾İ¿âµÄ½á¹¹

typedef struct
{
	char		szTong[defTONG_STR_LENGTH];
	DWORD		m_dwMoney;
}TMoneyStruct;
class CTongDB  
{
private:
	ZDBTable* m_TongTable;						//°ï»áÊı¾İ±í
	ZDBTable* m_MemberTable;					//°ïÖÚÊı¾İ±í
	ZDBTable* m_MoneyTable;
	
	TTongList m_TongList[MAX_TONG_COUNT];	//°ï»áÁĞ±í
	int m_TongListCount;						//°ï»áÁĞ±íÊı
public:
	CTongDB();
	virtual ~CTongDB(){};

	bool Open();
	bool OpenNew();
	void Close();
	
	bool ChangeTong(CTongControl& aTongControl);	//´´½¨ºÍ¸Ä±ä°ïÅÉ
	bool DelTong(char* aTongName);	//É¾³ı°ïÅÉ
	bool SearchTong(char* aTongName, CTongControl& aTongControl);	//´ÓÊı¾İ¿âÖĞÈ¡µÃ°ïÅÉ
	int GetTongCount();							//È¡µÃÃÅÅÉÊı
	int GetTongList(TTongList* aTongList, int aMaxNum);	//È¡µÃ°ï»áÁĞ±í

	bool ChangeMember(TMemberStruct aMember);//Ôö¼Ó°ïÖÚ
	bool DelMember(char* aMemberName);		//É¾³ı°ïÖÚ
	bool ChangeMoney(TMoneyStruct aMoney);
	bool DelMoney(char* aTongName);

	bool UpdateExtPoint(char* szAccount, int nExtPoint);
	int	 GetExtPoint(char* szAccount);
};

#endif // !defined(AFX_TONGDB_H__8C487F07_C490_4562_AD01_77953CCCBCE2__INCLUDED_)
