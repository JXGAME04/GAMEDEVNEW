//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
// File:	KObj.h
// Date:	2002.01.06
// Code:	Fong KiÒu
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KObj_H
#define KObj_H

#ifdef	_SERVER
#define	MAX_OBJECT	10000
#else
#define	MAX_OBJECT	256
#endif

#include "KPolygon.h"

#ifndef _SERVER
#include "KSprite.h"
#include "KWavSound.h"
#include "KSprControl.h"
#endif

#include "KIniFile.h"
#include "GameDataDef.h"
#ifndef	_SERVER
#include "../../Represent/iRepresent/KRepresentUnit.h"
#endif
#define		OBJ_BAR_WIDTH		3								// Îï¼şÕÏ°­ ³¤
#define		OBJ_BAR_HEIGHT		5								// Îï¼şÕÏ°­ ¿í
#define		OBJ_BAR_SIZE		(OBJ_BAR_WIDTH*OBJ_BAR_HEIGHT)	// Îï¼şÕÏ°­ ³¤x¿í

#define		OBJ_BOX_STATE_CLOSE		0		// ±¦Ïä×´Ì¬ ¹Ø
#define		OBJ_BOX_STATE_OPEN		1		// ±¦Ïä×´Ì¬ ¿ª
#define		OBJ_DOOR_STATE_CLOSE	0		// ÃÅ×´Ì¬ ¹Ø
#define		OBJ_DOOR_STATE_OPEN		1		// ÃÅ×´Ì¬ ¿ª
#define		OBJ_PROP_STATE_DISPLAY	0		// µÀ¾ß×´Ì¬ ¹Ø ²»ÏÔÊ¾
#define		OBJ_PROP_STATE_HIDE		1		// µÀ¾ß×´Ì¬ ¿ª ÏÔÊ¾
#define		OBJ_TRAP_STATE_STOP		0		// ÏİÚå×´Ì¬ ¹Ø
#define		OBJ_TRAP_STATE_ACTIVE	1		// ÏİÚå×´Ì¬ ¿ª
#define		OBJ_TRAP_STATE_ACTING	2		// ÏİÚå×´Ì¬ ÕıÔÚ¶¯×÷

#define		OBJ_BELONG_TIME		600				//thoi gian item roi duoi dat thuoc ve nguoi khac sau thoi gian nay co the nhat duoc

enum	Obj_Bar_Type			
{
	Obj_Bar_Empty = 0,			// ¿Õ£¬ÎŞÕÏ°­
	Obj_Bar_Full,				// ÍêÕûÕÏ°­
	Obj_Bar_LT,					// ×óÉÏÇøÓòÎªÕÏ°­
	Obj_Bar_RT,					// ÓÒÉÏÇøÓòÎªÕÏ°­
	Obj_Bar_LB,					// ×óÏÂÇøÓòÎªÕÏ°­
	Obj_Bar_RB,					// ÓÒÏÂÇøÓòÎªÕÏ°­
	Obj_Bar_Type_Num,			// ÕÏ°­ÀàĞÍÊı
};

typedef struct SOBJ_LIGHT_DATA
{
	int			m_nRadius;			// Îï¼şµÄ¹âÁÁ¶È
	int			m_nRed;				// ºìÆ«É«
	int			m_nGreen;			// ÂÌÆ«É«
	int			m_nBlue;			// À¶Æ«É«
	int			m_nAlpha;			// alpha Öµ
	int			m_nReflectType;		// ÊÜ¹â·½Ê½£¨²»ÊÜ¹â¡¢ÖØĞÄ¡¢Ë®Æ½¡¢×óĞ±¡¢ÓÒĞ±£©
	SOBJ_LIGHT_DATA() {Release();};
	void	Release() {	m_nRadius = 0; m_nRed = 0; m_nGreen = 0; m_nBlue = 0; m_nAlpha = 0; m_nReflectType = 0; };
} OBJ_LIGHT;		// Îï¼ş¹âÏßÏà¹Ø

enum	ObjTrap_Kind
{
	ObjTrap_Kind_Cycle_Image_Static = 0,
	ObjTrap_Kind_Cycle_Image_Dir,
	ObjTrap_Kind_Auto_Image_Static,
	ObjTrap_Kind_Auto_Image_Dir,
	ObjTrap_Kind_Auto_Image_Target,
	ObjTrap_Kind_Auto_Delay_Static,
	ObjTrap_Kind_Auto_Delay_Dir,
	ObjTrap_Kind_Auto_Delay_Target,
};

typedef struct OBJ_SKILL_DATA
{
	int		m_nKind;			// µØÍ¼ÏİÚåÀàĞÍ
	int		m_nCamp;			// ´ËÏİÚåÊôÓÚÄÄÒ»·½npc
	int		m_nRange;			// ÏİÚå´¥·¢·¶Î§
	int		m_nCastTime;		// ×Óµ¯·¢ÉäÊ±»ú
	int		m_nID;				// ¼¼ÄÜÀàĞÍ
	int		m_nLevel;			// ¼¼ÄÜµÈ¼¶
	int		m_nTarX;			// Ä¿±ê×ø±ê x
	int		m_nTarY;			// Ä¿±ê×ø±ê y
	OBJ_SKILL_DATA() {Release();};
	void	Release() {m_nKind = 0; m_nCamp = camp_animal; m_nRange = 0; m_nCastTime = 0; m_nID = 0; m_nLevel = 0; m_nTarX = 0; m_nTarY = 0;};
} OBJ_SKILL;		// µØÍ¼ÏİÚåÏà¹Ø(·¢Éä¼¼ÄÜ)

//---------------------------------------------------------------------------

#ifdef _SERVER
class ServerImage
{
public:
	int			m_nTotalFrame;						// ×ÜÖ¡Êı
	int			m_nCurFrame;						// µ±Ç°Ö¡
	int			m_nTotalDir;						// ×Ü·½ÏòÊı
	int			m_nCurDir;							// µ±Ç°·½Ïò
	DWORD		m_dwTimer;							// Ê±¼ä¼ÆÊıÆ÷
	DWORD		m_dwInterval;						// Ö¡¼ä¸ô£¨¶¯»­ËÙ¶È£©

protected:
	int			m_nDirFrames;						// Ã¿·½ÏòÖ¡Êı

public:
	ServerImage();									// ¹¹Ôìº¯Êı
	void		Release();							// Çå¿Õ
	void		SetTotalFrame(int nTotalFrame);		// Éè¶¨×ÜÖ¡Êı
	void		SetCurFrame(int nCurFrame);			// Éè¶¨µ±Ç°Ö¡
	void		SetTotalDir(int nTotalDir);			// Éè¶¨×Ü·½ÏòÊı
	BOOL		SetCurDir(int nDir);				// Éè¶¨µ±Ç°·½Ïò
	void		SetInterVal(DWORD dwInterval);		// Éè¶¨Ö¡¼ä¸ô
	int			GetOneDirFrames();					// »ñµÃµ¥·½ÏòÖ¡Êı
	BOOL		GetNextFrame(BOOL bLoop = TRUE);	// ×Ô¶¯Ïòºó»»Ö¡
	BOOL		GetPrevFrame(BOOL bLoop = TRUE);	// ×Ô¶¯ÏòÇ°»»Ö¡
	BOOL		CheckEnd();							// ÅĞ¶Ï¶¯»­ÊÇ·ñ²¥·Åµ½×îºó
	void		SetDirStart();						// Éè¶¨µ±Ç°·½ÏòµÄµ±Ç°Ö¡ÎªµÚÒ»Ö¡
	void		SetDirEnd();						// Éè¶¨µ±Ç°·½ÏòµÄµ±Ç°Ö¡Îª×îºóÒ»Ö¡
	BOOL		SetCurDir64(int nDir);				// Éè¶¨µ±Ç°·½Ïò(·½ÏòĞè´Ó64·½Ïò×ª»»µ½ÕæÕıµÄ·½Ïò)
	int			GetCurDirFrameNo();
};
#endif
#ifdef TOOLVERSION
class CORE_API KObj
#else
class KObj
#endif
{
	friend class KObjSet;
public:
	int			m_nID;					// Îï¼şµÄÊÀ½çÎ¨Ò»ID£¬¿Í»§¶ËÓë·şÎñÆ÷¶ËÒ»Ò»¶ÔÓ¦£¬Èç¹ûÖµÎª 0 £¬´ËÎª¿Í»§¶Ë obj £¬·şÎñÆ÷¶ËÃ»ÓĞ¶ÔÓ¦µÄ obj
	int			m_nDataID;				// ´ËÎï¼şÔÚÎï¼şÊı¾İ±íÖĞµÄÎ»ÖÃ(¾ÍÊÇÔÚÄ£°åÖĞµÄ±àºÅ)
	int			m_nKind;				// ÀàĞÍ	¶ÔÓ¦ Obj_Kind_XXXX
	int			m_nIndex;				// ÔÚ Object Êı×éÖĞµÄÎ»ÖÃ
	int			m_nBelongRegion;		// ±¾ÎïÌåÊôÓÚÄÄ¸öregion£¬²»ÊÇ×ø±ê£¬×ø±ê¿ÉÒÔ±ä¶¯£¬Õâ¸öÊÇËæ²úÉúÊ±ÄÇ¸öregion£¬¿Í»§¶Ë´æ´¢µÄÊÇ region id
	int			m_nSubWorldID;			// ×ÓÊÀ½çID
	int			m_nRegionIdx;			// ÇøÓòidx
	int			m_nMapX;				// µØÍ¼¸ñ×Ó×ø±ê x
	int			m_nMapY;				// µØÍ¼¸ñ×Ó×ø±ê y
	int			m_nOffX;				// µØÍ¼¸ñ×ÓÖĞµÄÆ«ÒÆ x (×¢£ºÆäÖµÎªÏñËØµãÖµ * 1024)
	int			m_nOffY;				// µØÍ¼¸ñ×ÓÖĞµÄÆ«ÒÆ y (×¢£ºÆäÖµÎªÏñËØµãÖµ * 1024)
	int			m_nDir;					// ·½Ïò£¬´Ë·½ÏòÎªÎï¼şµÄÂß¼­·½Ïò£¬°´64·½Ïò¼ÆËã£¬²»ÊÇ¾ßÌåµÄÍ¼Ïñ·½Ïò
	int			m_nState;				// ×´Ì¬
	int			m_nLifeTime;			// Îï¼şµÄÏûÊ§Ê±¼ä£¨Ö÷ÒªÓÃÓÚÊ¬ÌåÀà¡¢×°±¸Àà¡¢±¦ÏäÀàµÈ£©
	int			m_nBornTime;			// ÖØÉúµ¹¼ÆÊ±£¨box´ò¿ª×´Ì¬¾­¹ılifetime×Ô¶¯¹Ø±Õ£¬propÒşÉí×´Ì¬¾­¹ılifetime×Ô¶¯ÏÔÊ¾£©
	int			m_nWaitTime;			// µÈ´ıÊ±¼ä£¨ÓÃÓÚobj trap£¬ÔİÊ±ÎŞÓÃ£©
	OBJ_SKILL	m_cSkill;				// Îï¼şËù´øÄ§·¨¼¼ÄÜ£¨ÓÃÓÚobj trap£¬ÔİÊ±ÎŞÓÃ£©
	char		m_szName[32];			// Îï¼şÃû³Æ
	char		m_szScriptName[80];		// Ëù´ø½Å±¾ÎÄ¼şÃû
	DWORD		m_dwScriptID;			// Ëù´ø½Å±¾ÎÄ¼şID
	int			m_nColorID;				// Îï¼şÃû×ÖÏÔÊ¾Ê±µÄÑÕÉ«id£¬¿Í»§¶ËÍ¨¹ı²é±íÕÒµ½ÏàÓ¦ÑÕÉ«
	int			m_nItemDataID;			// ÓÃÓÚÎïÆ·£¬Ö¸Ã÷ÎïÆ·µÄ¾ßÌåÊı¾İ
	int			m_nItemWidth;			// ÎïÆ·³¤£¨·ÅÖÃÔÚÈİÆ÷ÖĞµÄÊ±ºò£©
	int			m_nItemHeight;			// ÎïÆ·¿í£¨·ÅÖÃÔÚÈİÆ÷ÖĞµÄÊ±ºò£©
	int			m_nMoneyNum;			// Èç¹ûÊÇµôÔÚµØÉÏµÄÇ®£¬Ç®µÄÊıÁ¿
	int 		m_nGenre;
	int 		m_nDetailType;
	int			m_nParticularType;
	int			m_dwNpcId2;
#ifdef _SERVER
	int			m_nBelong;
	int			m_nBelongTime;
	BOOL		m_bPickExecute;//add by phong kiÒu object sö dông ngay
	ServerImage m_cImage;				
#endif

#ifndef _SERVER
	char		m_szImageName[80];		// Í¼ÏñÎÄ¼şÃû
	char		m_szSoundName[80];		// ÉùÒôÎÄ¼şÃû
	KSprControl	m_cImage;				// Îï¼şÍ¼Ïñ
	char		m_szImageDropName[80];	// Èç¹ûÊÇitemÀàĞÍ£¬µô³öÀ´µÄÊ±ºò²¥·ÅµÄ¶¯»­ÎÄ¼şÃû
	KSprControl	m_cImageDrop;			// Èç¹ûÊÇitemÀàĞÍ£¬µô³öÀ´µÄÊ±ºò²¥·ÅµÄ¶¯»­µÄÍ¼Ïñ¿ØÖÆ
	int			m_nDropState;			// Èç¹ûÊÇitemÀàĞÍ£¬µ±Ç°ÊÇµô³ö¶¯»­×´Ì¬(1)»¹ÊÇ·ÅÖÃÔÚµØ°åÉÏ×´Ì¬(0)
	KRUImage	m_Image;
	unsigned int	m_SceneID;
	int			m_nLayer;				// Îï¼şÎ»ÓÚµÚ¼¸²ã
	int			m_nHeight;				// Îï¼şµÄÂß¼­¸ß¶È
	OBJ_LIGHT	m_sObjLight;			// Îï¼şÓëÁÁ¶È¡¢·¢¹âÏà¹ØµÄÊı¾İ
	DWORD		m_dwNameColor;			// Îï¼şÃû×ÖµÄÑÕÉ«
	BOOL		m_bAutoCheck;		//fkauto
	BOOL		m_bAutoLag;				//fkauto
#endif

	KPolygon	Polygon;				// µ×Ãæ¶à±ßĞÎ
	BYTE		m_btBar[OBJ_BAR_SIZE];	// Îï¼şµ×ÃæÕÏ°­ĞÅÏ¢

private:
	DWORD		m_dwTimer;				// Ê±¼ä¼ÆÊıÆ÷(ÓÃÓÚÉùÒô²¥·Å)

#ifndef _SERVER
	KCacheNode*	m_pSoundNode;			// ÉùĞ§Ö¸Õë
	KWavSound*	m_pWave;				// ÉùĞ§wavÖ¸Õë

	BOOL		m_bDrawFlag;			// »æÖÆÊÇ·ñÌØÊâ´¦Àí£¨FALSE »æÖÆ   TRUE ²»»æÖÆ£©
#endif

public:
	KObj();
	void		GetMpsPos(int *pX, int *pY);
	void		Release();
	void		Activate();
//	void		Save(KIniFile *IniFile, LPSTR Section);
//	void		Load(int nObjIndex, int nSubWorldID, KIniFile *IniFile, LPSTR Section);
	void		ExecScript(int nPlayerIdx);
	void		CastSkill(int nDir);
	void		CastSkill(int nXpos, int nYpos);
	void		SetIndex(int nIndex);
	void		SetWorldID(int nID);
	void		SetLifeTime(int nLifeTime);
	BOOL		SetDir(int n64Dir);
	void		SetState(int nState, int nPlaySoundFlag = 0);
	void		SetImageDir(int nDir);
	void		Remove(BOOL bSoundFlag);
	void		SetScriptFile(char *lpszScriptFile);
	int			GetKind();
	int			GetDistanceSquare(int nNpcIndex);
#ifdef _SERVER
	int			FindEnemy();
	void		TrapAct();
	void		SetItemDataID(int nItemDataID);
	BOOL		SyncAdd(int nClient);
	void		SyncState();
	void		SyncDir();
	void		SyncRemove(BOOL bSoundFlag);
	int			GetItemDataID();
	void		SetItemBelong(int nPlayerIdx);
	void		SetItemBelong2(int nPlayerIdx, int nTime);
	void		SetEntireBelong(int nPlayerIdx);
	void		SetObjPickExecute(BOOL bFlag) {m_bPickExecute = (BOOL)bFlag;};//add by phong kiÒu object sö dông ngay
	BOOL		GetObjPickExecute() {return m_bPickExecute;};
#endif

#ifndef _SERVER
	void		Draw();
	void		DrawInfo();
	void		DrawInfo_Backup();
	void		DrawBorder();
#endif

	void		PolygonChangeToBar(
					KPolygon Polygon,		// Í¹¶à±ßĞÎ
					int nGridWidth,			// ¸ñ×Ó³¤
					int nGridHeight,		// ¸ñ×Ó¿í
					int nTableWidth,		// ±í¸ñ³¤
					int nTableHeight,		// ±í¸ñ¿í
					BYTE *lpbBarTable);		// ±í¸ñÄÚÈİ

private:
	void		BoxOpen();
	void		BoxClose();
	void		DoorOpen();
	void		DoorClose();
#ifndef _SERVER
	void		PlayLoopSound();
	void		PlayRandSound();
	int			GetSoundPan();
	int			GetSoundVolume();
	void		PlaySound();
#endif
};

extern	KObj	Object[MAX_OBJECT];
//---------------------------------------------------------------------------
#endif

/*
1  ObjTrap_Kind_Cycle_Image_Static
2  ObjTrap_Kind_Cycle_Image_Dir
3  ObjTrap_Kind_Auto_Image_Static
4  ObjTrap_Kind_Auto_Image_Dir
5  ObjTrap_Kind_Auto_Image_Target
6  ObjTrap_Kind_Auto_Delay_Static
7  ObjTrap_Kind_Auto_Delay_Dir
8  ObjTrap_Kind_Auto_Delay_Target
*/





