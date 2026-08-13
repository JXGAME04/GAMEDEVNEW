//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KNpcResNode.h
// Date:	2002.01.06
// Code:	Fong KiÒu
// Desc:	Obj Class
//---------------------------------------------------------------------------

#pragma once

#ifndef _SERVER
#include	"KNode.h"

enum
{
	NPC_RES_SPECIAL = 0,
	NPC_RES_NORMAL,
	MAX_NPC_RES_KIND,
};

#define	MAX_BODY_PART			5//4		//  //#phi phong
#define	MAX_BODY_PART_SECT		4		
#define	MAX_PART				(MAX_BODY_PART*MAX_BODY_PART_SECT)
#define MAX_NPC_IMAGE_NUM		(1 + MAX_PART + 6 + 1 + 1)			

#define	MAX_ACTION				128
#define MAX_RES_STATE			256								//Hieu ung magic len nhan vat nhu lai thien diep, da cau tran, luu thuy, mong diep ....

class CActionName
{
private:
	char	m_szName[MAX_ACTION][FILE_NAME_LENGTH];	
	int		m_nMaxAction;				
	int		m_nCurActionNo;			

public:
	CActionName();
	// »ñÈ¡¶¯×÷ÖÖÀà¡¢Ãû³ÆµÈĞÅÏ¢
	BOOL	Init(char *lpszFileName);
	// ÓÉ¶¯×÷Ãû³ÆµÃµ½¶¯×÷±àºÅ
	int		GetActionNo(char *lpszName);
	// µÃµ½¶¯×÷ÖÖÀàÊı
	int		GetActionCount();
	// ÓÉ¶¯×÷±àºÅµÃµ½¶¯×÷Ãû³Æ
	BOOL	GetActionName(int nNo, char *lpszName, int nSize);

};

class CShadowFileName
{
private:
	struct	SShadowFileInfo
	{
		char	m_szName[FILE_NAME_LENGTH];
		int		m_nTotalFrame;
		int		m_nTotalDir;
		int		m_nInterval;
		int		m_nCgX;
		int		m_nCgY;
	}	*m_psInfo;
	int		m_nInfoLength;

public:
	CShadowFileName();
	~CShadowFileName();
	// ³õÊ¼»¯ÄÚ´æ·ÖÅä
	void		Init(int nActionCount);
	// µÃµ½Ä³¸ö¶¯×öµÄÒõÓ°ÎÄ¼şÃû¼°ÆäÏà¹ØÎÄ¼şĞÅÏ¢
	BOOL		GetFile(int nActionNo, int *pnFrame, int *pnDir, int *pnInterval, int *pnCgX, int *pnCgY, char *lpszName);
friend class KNpcResNode;
};

enum StateMagicType
{
	STATE_MAGIC_HEAD = 0,
	STATE_MAGIC_BODY,
	STATE_MAGIC_FOOT,
	STATE_MAGIC_TYPE_NUM,
};

// ×´Ì¬ÓëËæÉí¹âĞ§¶ÔÓ¦±í
class CStateMagicTable
{
private:
	int			m_nType[MAX_RES_STATE];			// ÀàĞÍ£ºÍ·¶¥¡¢½Åµ×¡¢ÉíÉÏ	//Head Foot //Loop
	int			m_nPlayType[MAX_RES_STATE];		// ²¥·ÅÀàĞÍ£ºÑ­»·²¥·Å
	int			m_nBackStart[MAX_RES_STATE];	// ÉíÉÏÀàĞÍnpc±³ºó¿ªÊ¼Ö¡
	int			m_nBackEnd[MAX_RES_STATE];		// ÉíÉÏÀàĞÍnpc±³ºó½áÊøÖ¡
	int			m_nTotalFrame[MAX_RES_STATE];	// spr ÎÄ¼ş×ÜÖ¡Êı
	int			m_nTotalDir[MAX_RES_STATE];		// spr ÎÄ¼ş×Ü·½ÏòÊı
	int			m_nInterVal[MAX_RES_STATE];// spr ÎÄ¼şÖ¡¼ä¸ô
	char		m_szName[MAX_RES_STATE][80];	// spr ÎÄ¼şÃû
public:
	CStateMagicTable();							// ¹¹Ôìº¯Êı
	BOOL		Init();							// ³õÊ¼»¯
	// ¹¦ÄÜ:	µÃµ½Ä³ÖÖ×´Ì¬µÄ¹âÓ°Ğ§¹ûµÄĞÅÏ¢
	void		GetInfo(int nNo, char *lpszGetName, int *pnType, int *pnPlayType, int *pnBackStart, int *pnBackEnd, int *pnTotalFrame, int *pnTotalDir, int *pnInterVal);
};

typedef struct SPR_INFO
{
	int		nTotalFrames;
	int		nTotalDirs;
	int		nInterval;
	int		nTotalEffectFrames;
	int		nTotalEffectDirs;
	int		nEffectInterval;
	char	szFileName[80];
	char	szEffectFileName[80];
} CSPR_INFO;
// Ä³²¿¼şµÄËùÓĞ×°±¸ÔÚ¸÷ÖÖ¶¯×÷ÏÂµÄÍ¼ÏñÎÄ¼şÃû
class CRESINFO
{
public:
	int			m_nActionKind;				// ¶¯×÷ÖÖÀàÊı
	int			m_nEquipKind;				// ×°±¸ÖÖÀàÊı
	CSPR_INFO	*m_cSprInfo;				// Ä³²¿¼şËùÓĞ×ÊÔ´ÎÄ¼şµÄÏà¹ØĞÅÏ¢
public:
	CRESINFO();
	~CRESINFO();
	// Çå³ıÊı¾İ
	void		AutoDelete();
	// Éè¶¨¶¯×÷ÖÖÀàÊı£¬×°±¸ÖÖÀàÊı£¬·ÖÅä±í¸ñÄÚ´æ»º³åÇø
	BOOL		AutoNew(int nWidth, int nHeight);
	// ¸ù¾İ¶¯×÷±àºÅ¡¢×°±¸±àºÅµÃµ½×ÊÔ´ÎÄ¼şÃû
	BOOL		GetName(int nActionNo, int nEquipNo, char *lpszDefault, char *lpszGetName, int nStrLen, bool bEffect=false);
	int			GetInterval(int nActionNo, int nEquipNo, int nDefault, bool bEffect=false);
	int			GetTotalFrames(int nActionNo, int nEquipNo, int nDefault, bool bEffect=false);
	int			GetTotalDirs(int nActionNo, int nEquipNo, int nDefault, bool bEffect=false);
};


// ×°±¸¡¢×´Ì¬Óë¶¯×÷¶ÔÕÕ±í£¬KNpcResNode ÖĞÓĞÁ½¸ö£¬Ò»¸öÓÃÓÚÃ»ÆïÂíµÄ£¬Ò»¸öÓÃÓÚÆïÂíµÄ
class CEquipStyleTable
{
private:
	int			m_nStyleKind;		// ×´Ì¬ÖÖÀàÊı
	int			m_nEquipKind;		// ×°±¸ÖÖÀàÊı
	int			*m_lpnActionNo;		// ±í¸ñÊı¾İ£¬´óĞ¡Îª m_nStyleKind * m_nEquipKind
public:
	CEquipStyleTable();
	~CEquipStyleTable();
	// Çå³ıÊı¾İ
	void		AutoDelete();
	// Éè¶¨±í¸ñ³¤¿í£¬³õÊ¼»¯±í¸ñ»º³åÇø
	BOOL		AutoNew(int nWidth, int nHeight);
	// Éè¶¨±í¸ñÖĞÄ³Ò»ÏîµÄÖµ
	void		SetValue(int nXpos, int nYpos, int nValue);
	// µÃµ½±í¸ñÖĞÄ³Ò»ÏîµÄÖµ
	int			GetValue(int nXpos, int nYpos);
};


// ÓÃÓÚÅÅĞò±í
typedef struct SACTTABLEOFF
{
	int		nActOff;			// ´Ë¶¯×÷ÔÚË³Ğò¶ÔÓ¦±íÖĞĞÅÏ¢µÄÎ»ÖÃ
	int		nLineNum;			// ´Ë¶¯×÷ÔÚ¶ÔÓ¦±íÖĞµÄÏàÓ¦ĞÅÏ¢ÓĞ¶àÉÙĞĞ
	BOOL	bUseDefault;		// ´Ë¶¯×÷¸÷¸ö·½ÏòµÄË³ĞòĞÅÏ¢ÊÇ·ñÊ¹ÓÃÄ¬ÈÏ·½Ê½
	SACTTABLEOFF() { Clear(); };
	void	Clear() { nActOff = 0; nLineNum = 0; bUseDefault = TRUE; };
} SActTableOff;

// ¶ÔÅÅĞò±íµÄ´¦Àí
class CSortTable
{
private:
	SActTableOff	m_sActTableOff[MAX_ACTION];	// Ã¿¸ö¶¯×÷ÔÚ±í¸ñÖĞµÄ²éÕÒ·½Ê½
	int				*m_lpnSortTable;			// ÅÅĞò±í£¬´óĞ¡Îª£ºm_nSortTableSize * (m_nPartNum + 1)
	int				m_nSortTableSize;			// ±í¸ñÓĞ¶àÉÙĞĞ
	int				m_nPartNum;					// ÓĞ¶àÉÙÖÖ²¿¼ş£¬±í¸ñÃ¿Ò»ĞĞµÚÒ»¸öµ¥Ôª±íÊ¾µÚ¼¸èå£¬
												// ºóÃæ¸ú×Å m_nPartNum ¸öµ¥Ôª£¬´æ´¢Ã¿¸ö²¿¼şµÄ±àºÅ
	
public:
	CSortTable();
	~CSortTable();
	char			m_sSortTableFileName[100]; //ÅÅĞò±íµÄÎÄ¼şÃû£¬ÓÃÓÚ±à¼©Æ÷
	// ´ÓÅÅĞòÎÄ¼şÖĞ°ÑÅÅĞòĞÅÏ¢¶ÁÈ¡³öÀ´£¬ĞèÒª´«ÈëµÄĞÅÏ¢ÓĞ£ºÅÅĞòÎÄ¼şÃû¡¢¸÷ÖÖ¶¯×÷µÄÃû³ÆµÈĞÅÏ¢¡¢²¿¼şµÄÊıÁ¿
	BOOL			GetTable(char *lpszFileName, CActionName *cActionName, int nPartNum);
	BOOL			GetTable(KIniFile *pIni, CActionName *cActionName, int nPartNum);
	// ´Ó±í¸ñÖĞµÃµ½Ò»×éÅÅĞòĞÅÏ¢
	BOOL			GetSort(int nActNo, int nDir, int nFrameNo, int *lpnTable, int nTableLen);
private:
	// Çå³ıÊı¾İ
	void			Release();
	// ´Ó×Ö·û´®ÖĞ°ÑÒ»ÏµÁĞÊı×ÖÈ¡³öÀ´
	void			SortStrToNum(char *lpszStr, int *lpnTable, int nTableSize);
};
// ×¢£º±í¸ñÇ°Ãæ MAX_SORT_DIR ĞĞÊÇËùÓĞ¶¯×÷ÔÚÄ¬ÈÏ·½Ê½ÏÂµÄÅÅĞò·½Ê½£¬µ±¶¯×÷Ö¸Ã÷Ê¹ÓÃÄ¬ÈÏ
//     ·½Ê½¶øÇÒ²»ÔÚÌØÊâèåµÄÇé¿öÏÂÊ¹ÓÃÕâĞ©ĞÅÏ¢£»¸ú×ÅÊÇËùÓĞ¶¯×÷µÄÌØÊâÅÅĞòĞÅÏ¢£¬Ã¿Ò»¸ö
//     ¶¯×÷Ò»¸öµ¥Ôª£¬Ã¿¸öµ¥Ôª·ÖÎªÁ½²¿·Ö£ºµÚÒ»²¿·ÖÊÇËùÓĞ MAX_SORT_DIR ¸ö·½ÏòµÄÅÅĞòĞÅ
//     Ï¢£¬Èç¹û´Ë¶¯×÷Ö¸Ã÷²»ÓÃÄ¬ÈÏ·½Ê½£¬ÔòÕâ¸ö²¿·Ö´æÔÚ²¢ÇÒÕ¼ MAX_SORT_DIR ĞĞ£»µÚ¶ş²¿
//     ·ÖÊÇÌØÊâèåµÄÅÅĞò·½Ê½£¬ÌØÊâèåµÄÊıÁ¿ÓÉ¶¯×÷Ö¸Ã÷£¨µ±´æÔÚµÚÒ»²¿·ÖÊ±Îª nLineNum - 
//     MAX_SORT_DIR£¬µ±²»´æÔÚµÚÒ»²¿·ÖÊ±Îª nLineNum£©£¬Èç¹ûÃ»ÓĞÌØÊâèå£¬ÔòÕâ²¿·Ö²»´æÔÚ

#ifndef TOOLVERSION
class KNpcResNode : public KNode
#else
class CORE_API KNpcResNode : public KNode
#endif
{
private:
	int					m_nNpcKind;				// ÄĞÖ÷½Ç Å®Ö÷½Ç ÆÕÍ¨npc kiÓu npc kieu npc bao gåm nam vµ n÷ man and lady
	int					m_nPartNum;
	char				m_szResPath[80];		// Í¼ĞÎÎÄ¼ş(spr)´æ·ÅµÄÂ·¾¶
	CRESINFO			m_cResInfo[MAX_PART];	// Ä³²¿¼şµÄËùÓĞ×ÊÔ´ÎÄ¼şĞÅÏ¢£¨°üÀ¨ËùÓĞ×ÊÔ´ÎÄ¼şÃû£©
	CEquipStyleTable	m_NoHorseTable;
	CEquipStyleTable	m_OnHorseTable;
	char				m_szSoundName[MAX_ACTION][80];
	
public:
	CSortTable			m_cSortTable;

	struct SECTINFO
	{
		int				nFlag;					// ´Ë²¿¼şÊÇ·ñ´æÔÚ
		char			szSectName[32];			// ´Ë²¿¼şµÄ²¿¼şÃû
		char			szSectResName[80];		// ´Ë²¿¼ş¶ÔÓ¦µÄ×ÊÔ´ËµÃ÷ÎÄ¼şÃû
		char			szSectSprInfoName[80];	// ´Ë²¿¼ş¶ÔÓ¦µÄ×ÊÔ´ĞÅÏ¢ËµÃ÷ÎÄ¼ş
		char			szSectEffectName[80];
		char			szSectEffectInfoName[80];
		SECTINFO() { Clear(); };
		void	Clear() { nFlag = 0; szSectName[0] = 0; szSectResName[0] = 0; szSectSprInfoName[0] = 0; szSectEffectName[0] = 0; szSectEffectInfoName[0] = 0; };
	}		 			m_nSectInfo[MAX_PART];	// ²¿¼şĞÅÏ¢

	CShadowFileName		m_cShadowInfo;			// ¶ÔÓ¦ËùÓĞ¶¯×÷µÄÒõÓ°ÎÄ¼şĞÅÏ¢

	char				m_szNpcName[80];		// Íæ¼ÒÀàĞÍÃû

public:
	KNpcResNode();
	~KNpcResNode();
	//	³õÊ¼»¯£¬ÔØÈëËùÓĞÏàÓ¦Êı¾İ
	BOOL			Init(char *lpszNpcName, CActionName *cActionName, CActionName *cNpcAction);
	// ÅĞ¶Ï×ÊÔ´ÀàĞÍ(·ÇÆ´½ÓÀàĞÍ»¹ÊÇÆ´½ÓÀàĞÍ)
	int				GetNpcKind() { return m_nNpcKind; };
	// ÅĞ¶ÏÄ³¸ö²¿¼şÊÇ·ñ´æÔÚ
	BOOL			CheckPartExist(int nPartNo);
	// »ñµÃÄ³¸ö²¿¼şµÄÄ³¸ö×°±¸ÔÚÄ³¸ö¶¯×÷ÏÂµÄ spr ÎÄ¼şÃû
	BOOL			GetFileName(int nPartNo, int nActionNo, int nEquipNo, char *lpszDefault, char *lpszGetName, int nStrLen, bool bEffect = false);
	// »ñµÃÄ³¸ö²¿¼şµÄÄ³¸ö×°±¸ÔÚÄ³¸ö¶¯×÷ÏÂµÄ spr ÎÄ¼şµÄÖ¡¼ä¸ô
	int				GetInterval(int nPartNo, int nActionNo, int nEquipNo, int nDefault, bool bEffect = false);
	// »ñµÃÄ³¸ö²¿¼şµÄÄ³¸ö×°±¸ÔÚÄ³¸ö¶¯×÷ÏÂµÄ spr ÎÄ¼şµÄ×ÜÖ¡Êı
	int				GetTotalFrames(int nPartNo, int nActionNo, int nEquipNo, int nDefault, bool bEffect = false);
	// »ñµÃÄ³¸ö²¿¼şµÄÄ³¸ö×°±¸ÔÚÄ³¸ö¶¯×÷ÏÂµÄ spr ÎÄ¼şµÄ×Ü·½ÏòÊı
	int				GetTotalDirs(int nPartNo, int nActionNo, int nEquipNo, int nDefault, bool bEffect = false);
	// »ñµÃÄ³ÎäÆ÷Ä³×´Ì¬ÏÂµÄ¶¯×÷±àºÅ
	int				GetActNo(int nDoing, int nEquipNo, BOOL bRideHorse);
	// ´Ó±í¸ñÖĞµÃµ½Ò»×éÅÅĞòĞÅÏ¢
	BOOL			GetSort(int nActNo, int nDir, int nFrameNo, int *lpnTable, int nTableLen);

	// °ÑÂ·¾­ºÍÎÄ¼şÃûºÏ³ÉÔÚÒ»Æğ³ÉÎª´øÂ·¾¶µÄÎÄ¼şÃû
	void			ComposePathAndName(char *lpszGet, char *lpszPath, char *lpszName);
	// »ñµÃÄ³¸ö¶¯×÷¶ÔÓ¦µÄÒôĞ§ÎÄ¼şÃû
	void			GetActionSoundName(int nAction, char *lpszSoundName);

	static void		GetShadowName(char *lpszShadow, char *lpszSprName);
};
#endif