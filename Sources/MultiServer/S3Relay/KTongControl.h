// KTongControl.h: interface for the CTongControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KTONGCONTROL_H__62D04F9A_67CD_419B_B475_BF0F8727A91E__INCLUDED_)
#define AFX_KTONGCONTROL_H__62D04F9A_67CD_419B_B475_BF0F8727A91E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define		defTONG_INIT_MEMBER_SIZE		100		// ³ÉÔ±ÄÚ´æ³õÊ¼»¯Ê±µÄ´óÐ¡
#define		defTONG_MEMBER_SIZE_ADD			100		// ³ÉÔ±ÄÚ´æÃ¿´ÎÔö¼ÓµÄ´óÐ¡
#define		defTONG_MASTER_TITLE        "Bang ch\xF1"          // "Bang chñ"
#define		defTONG_DIRECTOR_TITLE      "Tr\xAD\xEBng l\xB7o"  // "Tr­ëng l·o"
#define		defTONG_MANAGER_TITLE       "\xA7\xAD\xEAng ch\xF1"  // "§­êng chñ"
#define		defTONG_MEMBER_TITLE        "M\xABn \xD0\xCA"      // "M«n ®Ö"
#define		defTONG_NAME_SAY_ON_CHANNEL "C\xABng b\xE8"        // "C«ng bè"


typedef struct
{
	int			nCamp;								// °ï»áÕóÓª
	DWORD		dwMoney;								// °ï»á×Ê½ð
	int			nCredit;								// °ï»áÉùÍû
	int			nLevel;								// ®¼ng cÊp bang héi
	int			nRecruit;							//tr¹ng th¸i tuyÓn ng­êi

	char		szName[defTONG_STR_LENGTH];			// °ï»áÃû
	char		szPassword[defTONG_STR_LENGTH];		// °ï»áÃÜÂë

	char		szMasterName[defTONG_STR_LENGTH];
	char		szMasterTitle[defTONG_STR_LENGTH];	// °ïÖ÷³ÆºÅ
	char		szDirectorTitle[defTONG_MAX_DIRECTOR][defTONG_STR_LENGTH];// ¸÷³¤ÀÏ³ÆºÅ
	char		szManagerTitle[defTONG_MAX_MANAGER][defTONG_STR_LENGTH];	// ¸÷¶Ó³¤³ÆºÅ
	char		szNormalTitle[defTONG_STR_LENGTH];	// °ï»á°ïÖÚ³ÆºÅ
	char		szNormalBoyTitle[defTONG_STR_LENGTH];
	char		szNormalGirlTitle[defTONG_STR_LENGTH];
	//add by Fong KiÒu
	//int			 nStatusGuide;			//tr¹ng th¸i tuyÓn ng­êi bang héi
	char			szWayEdit[defTONG_STR_LENGTH];				//tiªu chÝ bang héi
	char			szNextTargetEdit[defTONG_STR_LENGTH];		//môc tiªu bang héi
	int				  nExpGuide;		//®iÓm kinh nghiÖm bang héi
	int				  nCityGuide;		//thµnh thÞ chiÕm lÜnh
	//int				  nTongLevel;		//®¼ng cÊp bang héi
	char			szLeagueTName[defTONG_STR_LENGTH];	//bang liªn minh
	//end add by Fong KiÒu
	int MemberCount;
}TTongStruct;	//ÓÃ×÷´æÈëÊý¾Ý¿âµÄ½á¹¹

struct STONG_MEMBER
{
	DWORD	m_dwNameID;
	char	m_szName[defTONG_STR_LENGTH];
	int		m_nSex;
};

typedef struct
{
	char		szName[defTONG_STR_LENGTH];			// °ï»áÃû
	int			MemberCount;						//Ò»°ã°ïÖÚÊý
}TTongList;	//ÓÃ×÷°ï»áÁÐ±í

class CTongControl
{
	friend class CTongSet;
	friend class CTongDB;
public:
	// ÓÃÓÚ´´½¨°ï»á
	CTongControl(int nCamp, char *lpszPlayerName, char *lpszTongName, int nMasterSex);
	// ÓÃÓÚÊý¾Ý¿âÔØÈë°ï»áÊý¾Ý
	CTongControl(TTongStruct sList);
	virtual ~CTongControl();

private:

	int			m_nCamp;								// °ï»áÕóÓª
	DWORD		m_dwMoney;								// °ï»á×Ê½ð
	int			m_nCredit;								// °ï»áÉùÍû
	int			m_nLevel;								// §¼ng cÊp bang
	int			m_nDirectorNum;							// ³¤ÀÏÈËÊý
	int			m_nManagerNum;							// ¶Ó³¤ÈËÊý
	int			m_nMemberNum;							// °ïÖÚÈËÊý
	BOOL	m_nRecruit;										//add by Fong Kieu tr¹ng th¸i ®ãng më bang
	char		m_szWayEdit[defTONG_STR_LENGTH];		//tiªu chÝ bang héi
	char		m_szNextTargetEdit[defTONG_STR_LENGTH];	//môc tiªu bang héi
	int				m_nExpGuide;		//®iÓm kinh nghiÖm
	int				m_nCityGuide;		//thµnh thÞ chiÕm lÜnh
	char			m_szLeagueTName[defTONG_STR_LENGTH];	//bang liªn minh

	DWORD		m_dwNameID;								// °ï»áÃûID
	char		m_szName[defTONG_STR_LENGTH];			// °ï»áÃû
	char		m_szPassword[defTONG_STR_LENGTH];		// °ï»áÃÜÂë

	char		m_szMasterTitle[defTONG_STR_LENGTH];	// °ïÖ÷³ÆºÅ
	char		m_szDirectorTitle[defTONG_MAX_DIRECTOR][defTONG_STR_LENGTH];// ¸÷³¤ÀÏ³ÆºÅ
	char		m_szManagerTitle[defTONG_MAX_MANAGER][defTONG_STR_LENGTH];	// ¸÷¶Ó³¤³ÆºÅ
	char		m_szNormalTitle[defTONG_STR_LENGTH];	// °ï»á°ïÖÚ³ÆºÅ

	DWORD		m_dwMasterID;							// °ïÖ÷ÃûID
	char		m_szMasterName[defTONG_STR_LENGTH];		// °ïÖ÷Ãû

	DWORD		m_dwDirectorID[defTONG_MAX_DIRECTOR];	// ¸÷³¤ÀÏÃûID
	char		m_szDirectorName[defTONG_MAX_DIRECTOR][defTONG_STR_LENGTH];	// ¸÷³¤ÀÏÃû

	DWORD		m_dwManagerID[defTONG_MAX_MANAGER];		// ¸÷¶Ó³¤ÃûID
	char		m_szManagerName[defTONG_MAX_MANAGER][defTONG_STR_LENGTH];	// ¸÷¶Ó³¤Ãû

	STONG_MEMBER	*m_psMember;
	int				m_nMemberPointSize;
	char		m_szNormalBoyTitle[defTONG_STR_LENGTH];
	char		m_szNormalGirlTitle[defTONG_STR_LENGTH];
	int			m_nMasterSex;
	int			m_nDirectorSex[defTONG_MAX_DIRECTOR];
	int			m_nManagerSex[defTONG_MAX_DIRECTOR];

public:
	BOOL		SearchOne(char *lpszName, STONG_ONE_LEADER_INFO *pInfo);

	BOOL		AddMember(char *lpszPlayerName, int nSex = 0);		// Ìí¼ÓÒ»¸ö³ÉÔ±

	BOOL		ChangePassword(char *lpOld, char *lpNew);	// ¸Ä±ä°ï»áÃÜÂë

	BOOL		GetTongHeadInfo(STONG_HEAD_INFO_SYNC *pInfo);	// °ï»áÐÅÏ¢

	BOOL		GetTongManagerInfo(STONG_GET_MANAGER_INFO_COMMAND *pApply, STONG_MANAGER_INFO_SYNC *pInfo);

	BOOL		GetTongMemberInfo(STONG_GET_MEMBER_INFO_COMMAND *pApply, STONG_MEMBER_INFO_SYNC *pInfo);

	BOOL		Instate(STONG_INSTATE_COMMAND *pInstate, STONG_INSTATE_SYNC *pSync);

	BOOL		Kick(STONG_KICK_COMMAND *pKick, STONG_KICK_SYNC *pSync);

	BOOL		Leave(STONG_LEAVE_COMMAND *pLeave, STONG_LEAVE_SYNC *pSync);

	BOOL		AcceptMaster(STONG_ACCEPT_MASTER_COMMAND *pAccept);

	BOOL		GetLoginData(STONG_GET_LOGIN_DATA_COMMAND *pLogin, STONG_LOGIN_DATA_SYNC *pSync);

	BOOL		DBInstate(char *lpszPlayerName, BYTE nSite);

	BOOL		DBChangeTitle(STONG_ACCEPT_TITLE_COMMAND *pAccept);
	
	BOOL		DBChangeSexTitle(STONG_ACCEPT_SEX_TITLE_COMMAND *pAccept);
	
	BOOL		GetMasterTitle(char *lpszTitle);

	BOOL		GetDirectorTitle(char *lpszTitle, int nPos);

	BOOL		GetManagerTitle(char *lpszTitle, int nPos);

	BOOL		GetMemberTitle(char *lpszTitle, int nSex);

	BOOL		ChangeMoney(STONG_MONEY_COMMAND* pMoney,STONG_MONEY_SYNC *Sync);
	
	BOOL		DBChangeMoney(DWORD dwMoney) { m_dwMoney = dwMoney;};

	BOOL		DBChangeCamp(STONG_CHANGE_CAMP_COMMAND *pChange);

	BOOL		DBChangeRecruit(STONG_CHANGE_RECRUIT_COMMAND *pChange);

	BOOL		DBChangeTongLevel(STONG_CHANGE_LEVEL_COMMAND *pChange);

	BOOL		DBChangeTongWayEdit(STONG_CHANGE_WAYEDIT_COMMAND *pChange);

	BOOL		DBChangeTongNextTarget(STONG_CHANGE_NEXTTARGET_COMMAND *pChange);
	
};

#endif // !defined(AFX_KTONGCONTROL_H__62D04F9A_67CD_419B_B475_BF0F8727A91E__INCLUDED_)
