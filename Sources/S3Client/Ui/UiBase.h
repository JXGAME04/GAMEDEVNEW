/*****************************************************************************************
//	Copyright : Kingsoft 2002
//	Author	:   Fong KiÒu
//	CreateTime:	2002-7-17
*****************************************************************************************/
#pragma once

enum UISYS_STATUS
{
	UIS_S_IDLE,
	UIS_S_TRADE_PLAYER,		//Player
	UIS_S_TRADE_NPC,		//Npc
	UIS_S_TRADE_SALE,		//B¸n
	UIS_S_TRADE_BUY,		//Mua
	UIS_S_TRADE_REPAIR,		//Söa
	UIS_S_TRADE_SETPRICE,	//Set gi¸
	UIS_S_TRADE_LOCKITEM,	//Kho¸
	UIS_S_TRADE_UNLOCKITEM	//Më kho¸
};

enum UISYS_OPERATION
{
	UIS_O_MOVE_ITEM,
	UIS_O_TRADE_ITEM,
	UIS_O_USE_ITEM,
};

#define APP_START	"Start()"
#define APP_EXIT	"Exit()"
#define APP_STARTGAME	"StartGame(\"%s\")"
#define APP_EXITGAME	"ExitGame()"
#define APP_SAY		"Say(\"%s\", \"%s\")"
#define APP_CHAT	"Chat(\"%s\", \"%s\")"
#define APP_CREATEUNIT	"CreateUnit(\"%s\")"
#define APP_DELETEUNIT	"DeleteUnit(\"%s\")"
#define APP_CREATEUNITGROUP	"CreateUnitGroup(\"%s\", \"%s\")"
#define APP_RENAMEUNITROUP	"RenameUnitGroup(\"%s\", \"%s\", \"%s\")"
#define APP_DELETEUNITROUP	"DeleteUnitGroup(\"%s\", \"%s\")"
#define APP_MOVEUNITROUP	"MoveUnitGroup(\"%s\", \"%s\", \"%s\")"
#define APP_CREATEUNITMEMBER	"CreateUnitMember(\"%s\", \"%s\", \"%s\")"
#define APP_DELETEUNITMEMBER	"DeleteUnitMember(\"%s\", \"%s\")"
#define APP_MOVEUNITMEMBER	"MoveUnitMember(\"%s\", \"%s\", \"%s\")"
#define APP_SETUNITMEMBERSTATUS	"SetUnitMemberStatus(\"%s\", \"%s\", \"%s\")"

class KIniFile;

extern const char*	$Main;

#pragma warning(disable:4786)
#include "map"
#include "string"

class KUiBase
{
public:
	KUiBase();
	~KUiBase();
	int		Init();											//³õÊ¼»¯
	void	Exit();											//ÍË³ö

	void	SetUserAccount(char* pszId, char* pszRole);		//ÉèÖÃÓÃ»§ÕËºÅ
	int		GetUserPrivateDataFolder(char* pBuffer, int nSize);	//»ñµÃµ±Ç°Íæ¼ÒË½ÓĞÊı¾İµÄ´æ´¢Ä¿Â¼
	const char*	GetUserTempDataFolder();					//»ñµÃÍæ¼ÒÊı¾İµÄÁÙÊ±´æÅÌÄ¿Â¼Î»ÖÃ
	void	CleanTempDataFolder();							//Çå³ıÍæ¼ÒÊı¾İµÄÁÙÊ±´æÅÌÄ¿Â¼ÄÚµÄÈ«²¿ÎÄ¼ş
	void	CleanPrivateDataFolder();						//Çå³ıµ±Ç°Íæ¼ÒË½ÓĞÊı¾İµÄ´æ´¢Ä¿Â¼
	void	DeletePrivateDataFile(const char* pszFileName);	//Çå³ıµ±Ç°Íæ¼ÒË½ÓĞÊı¾İµÄÄ³¸öÎÄ¼ş

	int		LoadScheme(const char* pSchemeName);			//°´ÕÕÄ³ÖÖ·½°¸ÔØÈë½çÃæ(Skin)
	int		SchemeCount();									//»ñµÃ½çÃæ·½°¸µÄÊıÄ¿
	int		GetScheme(int nIndex, char* pName, char* pPath);//»ñµÃÄ³¸ö½çÃæ·½°¸µÄÃû³ÆÓëÂ·¾¶
	int		GetCurSchemePath(char* pBuffer, int nSize);		//»ñµÃµ±Ç°½çÃæ·½°¸µÄÂ·¾¶
	
	KIniFile*	GetCommSettingFile();						//´ò¿ª±£´æ½çÃæ¹«¹²ÉèÖÃµÄÎÄ¼ş
	void		CloseCommSettingFile(bool bSave);			//¹Ø±Õ±£´æ½çÃæ¹«¹²ÉèÖÃµÄÎÄ¼ş
	KIniFile*	GetPrivateSettingFile();					//´ò¿ª´ò¿ªµ±Ç°ÕËºÅµÄÉèÖÃÎÄ¼ş
	void		ClosePrivateSettingFile(bool bSave);		//¹Ø±Õ´ò¿ªµ±Ç°ÕËºÅµÄÉèÖÃÎÄ¼ş
	KIniFile*	GetCommConfigFile();
	void		CloseCommConfigFile();
	KIniFile*	GetAutoSettingFile();
	void		SetGlobalConfigFile(bool bGlobal);
	void		SetGlobalConfigFilename(char* zsGlobalFilename);
	char*		GetGlobalConfigFilename();
	BOOL		GetGlobalConfigFile() { return m_bUseGlobalAutoSetting; }
	//´ò¿ª´ò¿ªµ±Ç°ÕËºÅµÄÉèÖÃÎÄ¼ş
	void		CloseAutoSettingFile(bool bSave);		//¹Ø±Õ´ò¿ªµ±Ç°ÕËºÅµÄÉèÖÃÎÄ¼ş
	KIniFile*	GetGameSettingFile();
	void		CloseGameSettingFile();

	UISYS_STATUS GetStatus() { return m_Status; }
	void		 SetStatus(UISYS_STATUS eStatus);
	void		 SetCurrentCursor();						
	int			 IsOperationEnable(UISYS_OPERATION uOper);
	void		LoadPrivateConfig();						
	int			SavePrivateConfig();						
	void		RegisterEvent(const char* szName, HANDLE hWnd);
	void		UnregisterEvent(const char* szName, HANDLE hWnd);
	int			NotifyEvent(const char* szEvent);		
	int			NotifyOneEvent(const char* szName, const char* szEvent);		

private:
	void	LoadSchemeForEachWnd();
	int		GetSchemePath(const char* pScheme);				
	void	ClearFolder(const char* pszFolder, bool bDeleteFolder);	

private:
	UISYS_STATUS	m_Status;				//½çÃæ²Ù×÷×´Ì¬

	char		m_CurScheme[32];			//µ±Ç°½çÃæ·½°¸µÄÃû³Æ
	char		m_CurSchemePath[40];		//µ±Ç°½çÃæ·½°¸µÄÂ·¾¶
	char		m_UserAccountId[32];		//µ±Ç°Íæ¼ÒÕËºÅÔÚ½çÃæÏµÍ³ÖĞµÄid

	KIniFile*	m_pUiCommSettingFile;		//½çÃæ¹«¹²ÉèÖÃµÄÎÄ¼ş
	KIniFile*	m_pUiPrivateSettingFile;	//µ±Ç°ÕËºÅµÄÉèÖÃÎÄ¼ş
	KIniFile*	m_pUiCommConfigFile;
	KIniFile*	m_pUiAutoSettingFile;
	KIniFile*	m_pUiGameSettingFile;
	BOOL		m_bUseGlobalAutoSetting;
	char		m_zsGlobalFilename[64];

	typedef std::map<std::string, HANDLE> EVENTWNDLIST;
	EVENTWNDLIST m_EventWndList;
};

extern KUiBase	g_UiBase;