/*****************************************************************************************
//	Copyright : Kingsoft 2002
//	Author	:   Fong KiÒu
//	CreateTime:	2020-8-16
*****************************************************************************************/
#pragma once

#include "../Elem/WndButton.h"
#include "../Elem/WndPureTextBtn.h"
#include "../Elem/WndText.h"
#include "../Elem/WndEdit.h"
#include "../Elem/WndValueImage.h"
#include "../Elem/WndObjContainer.h"
#include "../Elem/WndLabeledButton.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
#include "../../../Core/src/GameDataDef.h"
#include "UiMsgCentrePad.h"
#include "../Elem/ComWindow.h"

#define	INVISIBLE_POS_X		-300
#define	INVISIBLE_POS_Y		100
#define	INVISIBLE_POS		INVISIBLE_POS_X,INVISIBLE_POS_Y // chat item

class GameWorld_DateTime : public KWndLabeledButton
{
public:
	DECLARE_COMCLASS(GameWorld_DateTime)
	GameWorld_DateTime();
	virtual int		Init(KIniFile* pIniFile, const char* pSection);	
	void            UpdateData();
	int				m_nNetStatus; //add by Phong Kieu
	int				GetNetStatus();
private:
	int             m_nSmoothPing, m_nCrowdPing, m_nBlockPing;
	bool			m_bTrueWorld;
	bool            m_bNetStatus;
	bool			m_bLogo;
	char            m_szLogo[32], m_szSmoothMsg[32], m_szCrowdMsg[32], m_szBlockMsg[32];
	unsigned int	m_uLastSwitchTime, m_uDefaultColor, m_uSmoothColor, m_uCrowdColor, m_uBlockColor;
private:
	void OnButtonClick();
	void UpdateNetStatus();
};

class KImmediaItem : public KWndObjectBox
{
public:
	int m_nIndex, m_nOrder;
	DWORD m_TextColor;
	DWORD m_TextColorIndex;
	KImmediaItem() :m_nIndex(-1), m_nOrder(0), m_TextColor(0xFFFFFF00), m_TextColorIndex(0xFFFFFF00){}
	void PaintWindow();
	int	Init(KIniFile* pIniFile, const char* pSection);
};

struct KUiDraggedObject;
struct KPopupMenuData;
struct KStateTempNode;
struct KStatePos;
class KIniFile;

struct KMsgItemRecent
{
	int		pItemLen;
	int		pItemValue;
};

class KUiPlayerBar : protected KWndImage
{
public:

	static KUiPlayerBar* OpenWindow();					
	static KUiPlayerBar* GetIfVisible();				
	static void			CloseWindow(bool bDestroy);		
	static void			LoadScheme(const char* pScheme);
	static	void		LoadSchemeTeamMNG(const char* pScheme);
	static void			InputNameMsg(char bChannel, const char* szName, bool bFocus);
	static int			FindRecentPlayer(const char* szName);
	static int          AddRecentPlayer(const char* szName);
	static char*        GetRecentPlayerName(int nIndex);
	static const char*	SelfName()
	{
		if (m_pSelf)
			return m_pSelf->m_szSelfName;
		return NULL;
	}
	static BOOL	IsSelfName(char* szName)
	{
		if (m_pSelf && szName)
		{
			return (strcmpi(m_pSelf->m_szSelfName, szName) == 0);
		}
		return FALSE;
	}
	static void	OnUseItem(int nIndex);				
	static BOOL	IsCanSendMessage(const char* Buffer, int nLen, char* szDestName, int nChannelID);				
	static void	OnSendChannelMessage(DWORD nChannelID, const char* Buffer, int nLen);				
	static void	OnSendSomeoneMessage(const char* Name, const char* Buffer, int nLen);				
	void	UpdateItem(int nIndex, unsigned int uGenre, unsigned int uId);	
	void	UpdateSkill(int nIndex, unsigned int uGenre, unsigned int uId);	
	static	bool		GetExp(double& nFull, double& nCurrLevelExp, double& nCurrentExp);
	static BOOL			LoadPrivateSetting(KIniFile* pFile);
	static int			SavePrivateSetting(KIniFile* pFile);
	static void         SetCurrentChannel(int nIndex);
	static void         SwitchChannel(BOOL bUp = FALSE);  
    static int          GetCurChannel();                  
	BOOL				CheckEditItem();
	static void			InputItemMsg(const char* szName, const char* szDesc, const int nColor, const int pItemLen, const int pItemValue);
	static int			SplitItemString(char* bItem, int& bItemLen, char* bBuff, int& bBuffLen, const char* pBuff, int nLen);
	static void			LoadStatePos();	
	static void			Clear();
	void				  ChangeWifiStatus(int nStatus);
private:
	KUiPlayerBar();
	~KUiPlayerBar();
	void	Breathe();									
	void	Initialize();								
	void	PaintWindow();
	void	UpdateData();								
	void	UpdateXXXNumber(int& nMana, int& nFullMana);							
	void	UpdateRuntimeAttribute(int& nMoney, int& nLevel);
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	OnObjPickedDropped(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
	void	LoadScheme(KIniFile* pIni);
	void	OnSend(BOOL bDirectSend);
	void	OnDirectSendChannelMessage(DWORD nChannelID, BYTE cost, const char* Buffer, int nLen);				
	void	PopupChannelMenu(int x, int y);
	void	PopupPhraseMenu(int x, int y, bool bFirstItem);
	void	InputRecentMsg(bool bPrior);
	void	OnSwitchSize();			
	int GetChannelIndex(const char* pTitle);
	//void SetCurrentChannel(int nIndex);
	//int AddRecentPlayer(const char* szName);
	void		InputCurrentChannel(int nIndex, bool bFocus);
	void		ReplaceSpecialName(char* szDest, size_t nDestSize, char* szSrc);
	int			  IsHasCost(BYTE cost, int nMoney, int nLevel, int nMana, int nFullMana, int& nUseMoney, int& nUseLevel, int& nUseMana);
	int			  GetStateAtPos(int x, int y);
	void		UpdateStateTip(int x, int y);
private:
	struct	KStatePos
	{
		int		wSkillID;
		char	szName[32];
		char	szImage[64];	
		char	szDesc[64];		
	};
	static KUiPlayerBar	* m_pSelf;
	static KStatePos	* ms_pStateList;
	static int			  ms_nNumState;		
private:
	KWndButton		m_Face;		
	KWndButton		m_ChatBar;
	KWndButton		m_ShorcutKeyBar;
	GameWorld_DateTime		m_DateTime;		
	KImmediaItem	m_ImmediaItem[UPB_IMMEDIA_ITEM_COUNT];
	KWndObjectBox	m_ImmediaSkill[2];
	KWndButton		m_SendBtn;	
	KWndEdit512		m_InputEdit;	
	KWndPureTextBtn	m_ChannelSwitchBtn;	
	KWndButton		m_ChannelOpenBtn;	
	KWndButton		m_SwitchBtn;	
	KWndButton		m_Friend;
	KWndButton		m_Options;
	KWndButton		m_ChatRoom;
	KWndButton		m_Status;
	KWndButton		m_Items;
	KWndButton		m_ItemEx;
	KWndButton		m_Skills;
	KWndButton		m_Team;
	KWndButton		m_Faction;
	KWndButton		m_Market;
	KWndButton		m_WifiStatus;
	KWndButton		m_Auto;
	KWndButton		m_HideWindow;	

#define	MAX_BUTTON_STATE		20
	KWndButton		m_StateImg[MAX_BUTTON_STATE];
	KWndText32		m_StateLife[MAX_BUTTON_STATE];
	char			m_cPreMsgCounter;
	char			m_cLatestMsgIndex;
#define	MAX_RECENT_MSG_COUNT	8
	char			m_RecentMsg[MAX_RECENT_MSG_COUNT][512];
	KMsgItemRecent 	m_RecentIten[MAX_RECENT_MSG_COUNT];
	int m_nCurChannel;
#define	MAX_RECENTPLAYER_COUNT	10
	char 			m_RecentPlayerName[MAX_RECENTPLAYER_COUNT][32];
	int 			m_nRecentPlayerName;
	int				m_bMiniMode;
	char			m_szSelfName[32];	
	double				m_nExperienceFull;
	double				m_nCurLevelExperience;
	double				m_nExperience;
	int				m_nCurrIndex;	
	int 			nX, nY, nBegin, nEnd;	
	
	// chat item
	#define			MAX_ITEMBUTTON			120  //sè nµy vµ sè trong ui b»ng nhau míi kh«ng bÞ mÊt item chat khi ch¹m max
	//
public:
	static void		SetChatItem(ChatItem CItem, unsigned int uId);//ghi nho 1 chat item
	static void		SetItemBtnInfo(int nBtnNo, ChatItem * pItem); // dat id item rao ban
	static void		ClearItemBtn(); //xoa het button
	static void		SetItemBtnPos(int nBtnNo,int X, int Y); // dat vi tri button chat
private:
	KWndPureTextBtn		m_ItemBtn[MAX_ITEMBUTTON];//Item rao ban
	char			m_ChatItemInfo[128]; //thong tin item se duoc gui di
	char			m_ItemName[64]; // ten item
};
