// KTongControl.h: interface for the CTongControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KTONGCONTROL_H__62D04F9A_67CD_419B_B475_BF0F8727A91E__INCLUDED_)
#define AFX_KTONGCONTROL_H__62D04F9A_67CD_419B_B475_BF0F8727A91E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>	// JX2 port


#define		defTONG_INIT_MEMBER_SIZE		100		// ³ÉÔ±ÄÚ´æ³õÊ¼»¯Ê±µÄ´óÐ¡
#define		defTONG_MEMBER_SIZE_ADD			100		// ³ÉÔ±ÄÚ´æÃ¿´ÎÔö¼ÓµÄ´óÐ¡
#define		defTONG_MASTER_TITLE        "Bang Chñ"          // "Bang chñ"
#define		defTONG_DIRECTOR_TITLE      "Tr­ëng L·o"  // "Tr­ëng l·o"
#define		defTONG_MANAGER_TITLE       "§­êng Chñ"  // "§­êng chñ"
#define		defTONG_MEMBER_TITLE        "M«n ®Ö"      // "M«n ®Ö"
#define		defTONG_NAME_SAY_ON_CHANNEL "C«ng bè"        // "C«ng bè"


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
	// ==== JX2 port: KV field cap bang (them o CUOI struct - theo bay-2) ====
	int			nJX2FieldCount;
	WORD		wJX2FieldKey[defTONG_JX2_MAX_FIELDS];
	DWORD		dwJX2FieldVal[defTONG_JX2_MAX_FIELDS];
	char		szJX2Announce[defTONG_JX2_ANNOUNCE_LEN];	// thong bao bang
	char		szJX2Event[defTONG_JX2_RECORD_NUM][defTONG_JX2_RECORD_LEN];	// so su kien (vong)
	char		szJX2History[defTONG_JX2_RECORD_NUM][defTONG_JX2_RECORD_LEN];	// so lich su (vong)
	int			nJX2EventHead;
	int			nJX2HistoryHead;
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
	BOOL		m_bIsFull;
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

	size_t m_nCapSize;
	char		m_szNormalBoyTitle[defTONG_STR_LENGTH];
	char		m_szNormalGirlTitle[defTONG_STR_LENGTH];
	int			m_nMasterSex;
	int			m_nDirectorSex[defTONG_MAX_DIRECTOR];
	int			m_nManagerSex[defTONG_MAX_MANAGER];

public:
	BOOL		SearchOne(char *lpszName, STONG_ONE_LEADER_INFO *pInfo);

	BOOL		AddMember(char *lpszPlayerName, int nSex = 0, bool fromAdd = false);		// Ìí¼ÓÒ»¸ö³ÉÔ±

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

	BOOL		DBChangeTongExp(STONG_CHANGE_EXP_COMMAND* pChange);

	BOOL		DBChangeTongWayEdit(STONG_CHANGE_WAYEDIT_COMMAND *pChange);

	BOOL		DBChangeTongNextTarget(STONG_CHANGE_NEXTTARGET_COMMAND *pChange);
	
	// ==== JX2 port: du lieu KV + quyen (xem BANGHOI_JX2_PHANTICH.md) ====
public:
	struct JX2Member
	{
		int	nFieldCount;
		WORD	wFieldKey[defTONG_JX2_MEMBER_FIELDS];
		DWORD	dwFieldVal[defTONG_JX2_MEMBER_FIELDS];
		int	nRightCount;
		DWORD	dwRight[defTONG_JX2_MEMBER_RIGHTS];
	};
	int	m_nJX2FieldCount;
	WORD	m_wJX2FieldKey[defTONG_JX2_MAX_FIELDS];
	DWORD	m_dwJX2FieldVal[defTONG_JX2_MAX_FIELDS];
	char	m_szJX2Announce[defTONG_JX2_ANNOUNCE_LEN];
	char	m_szJX2Event[defTONG_JX2_RECORD_NUM][defTONG_JX2_RECORD_LEN];
	char	m_szJX2History[defTONG_JX2_RECORD_NUM][defTONG_JX2_RECORD_LEN];
	int	m_nJX2EventHead;
	int	m_nJX2HistoryHead;
	std::map<DWORD, JX2Member>	m_mapJX2Member;	// khoa = NameID thanh vien

	void	JX2_Reset();
	int	JX2_FindFieldIdx(WORD wKey);
	DWORD	JX2_GetField(WORD wKey);
	BOOL	JX2_SetField(WORD wKey, DWORD dwValue);
	BOOL	JX2_AddField(WORD wKey, int nDelta, BOOL bUnsigned);
	__int64	JX2_GetMoney64();
	void	JX2_SetMoney64(__int64 nMoney);
	JX2Member*	JX2_GetMember(DWORD dwNameID, BOOL bCreate);
	DWORD	JX2_GetMemberField(DWORD dwNameID, WORD wKey);
	BOOL	JX2_SetMemberField(DWORD dwNameID, WORD wKey, DWORD dwValue);
	BOOL	JX2_AddMemberField(DWORD dwNameID, WORD wKey, int nDelta, BOOL bUnsigned);
	BOOL	JX2_ChangeRight(DWORD dwNameID, DWORD dwRightID, BOOL bAdd);
	BOOL	JX2_GetMemberName(DWORD dwNameID, char* pszOut, int nOutSize);
	void	JX2_SaveToStruct(TTongStruct* pStruct);
	void	JX2_LoadFromStruct(const TTongStruct* pStruct);
	void	JX2_LoadMemberFromStruct(const void* pMemberRecord);
	int	JX2_CollectMembers(struct JX2MemberBrief* pArr, int nMax);
	int	JX2_BuildTongSync(void* pBuffer, int nBufSize, int nMemberTotal);
	int	JX2_BuildMemberSync(void* pBuffer, int nBufSize,
			const struct JX2MemberBrief* pArr, int nTotal, int* pnStartIdx, int nMaxPerPacket);
	// dot 2:
	BOOL	JX2_IsValid() { return m_szName[0] != 0; }
	const char*	JX2_Name() { return m_szName; }
	BOOL	JX2_SetString(int nKind, const char* pszText);
	BOOL	JX2_KickByNameID(DWORD dwNameID);
	BOOL	JX2_SetFigureByNameID(DWORD dwNameID, int nNewFigure);
	BOOL	JX2_Upgrade();
	BOOL	JX2_Degrade();
	void	JX2_DailyMaintain(int nTodayDay, int nWeekday);
	void	JX2_WeeklyMaintain(int nTodayDay);
	void	JX2_MoneyToExpTick();
	BOOL	JX2_Distribute(int nOpCode, DWORD dwMemberNameID, int nOffer, int nFigure);
};

#endif // !defined(AFX_KTONGCONTROL_H__62D04F9A_67CD_419B_B475_BF0F8727A91E__INCLUDED_)
