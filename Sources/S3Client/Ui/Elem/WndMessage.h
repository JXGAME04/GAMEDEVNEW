/*****************************************************************************************
//	Copyright : Kingsoft 2002
//	Author	:   Fong KiÒu
//	CreateTime:	2021-7-9
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once
#pragma warning(disable:4786)	//bá qua c¶nh b¸o khëi t¹o c¸c tham sè vßng s¸ng item

#define	WND_GAMESPACE			0xFFFFFFFF

#ifndef WM_MOUSEHOVER
	#define	WM_MOUSEHOVER	0x02A1
#endif

#define	WND_M_BASE				0x500	//WM_USER + 0x100

//------------------------------------------------
//	´°¿ÚÒ»°ãÏûÏ¢
#define WND_M_OTHER_WORK_RESULT	WND_M_BASE + 1				//½çÃæ²Ù×÷½á¹û·µ»Ø
//uParam = ²Ù×÷ĞèÇó½çÃæÖ´ĞĞ¶¨ÒåµÄ²Ù×÷²ÎÊı£¬±ÈÈçĞèÇó½çÃæÓÃ´Ë²ÎÊıÀ´Çø·Ö×Ô¼º¿ÉÒÔ·¢³öµÄ¶à¸öĞèÇó
//nParam = È¡ÖµÎªWND_OPER_RESULT_0»òÕßWND_OPER_RESULT(n), nÖ¸´úÈÎÒâÒ»¸öÕûÊıÖµ£¬Æäº¬ÒåÒÀ¾İ¾ßÌåµÄ½çÃæÓ¦ÓÃ×Ô¼ºÈ·ÈÏ
#define	WND_M_MENUITEM_SELECTED	WND_M_BASE + 2
#define	WND_M_MENUITEMRIGHT_SELECTED	WND_M_BASE + 3
//(KWndWindow*)uParam = µ÷ÓÃ²Ëµ¥µÄµÄ´°¿Ú
//	(short)(nParam & 0xffff) = ËùÑ¡µÄ²Ëµ¥ÏîµÄË÷Òı£¬Èç¹ûÑ¡ÔñÈ¡ÏûÔòÎª-1
//	(short)((nParam >> 16) & 0xffff) = ²Ù×÷ĞèÇó½çÃæÖ´ĞĞ¶¨ÒåµÄ²Ù×÷²ÎÊı£¬±ÈÈçĞèÇó½çÃæÓÃ´Ë²ÎÊıÀ´Çø·Ö×Ô¼º¿ÉÒÔ·¢³öµÄ¶à¸öĞèÇó
/*
#define	WND_M_CREATE			WND_M_BASE + 1
#define	WND_M_DESTROY			WND_M_BASE + 2
#define	WND_M_CLOSE				WND_M_BASE + 3*/
#define WND_M_MOUSE_ENTER		WND_M_BASE + 6				//Êó±êÖ¸Õë½øÈë´°¿Ú·¶Î§
#define	WND_M_MOUSE_LEAVE		WND_M_BASE + 7				//Êó±êÖ¸ÕëÀë¿ª´°¿Ú·¶Î§

#define	WND_N_MOUSE_ENTER_CHILD	WND_M_BASE + 8				//Êó±êÖ¸Õë½øÈëÄ³×Ó´°¿Ú·¶Î§
//(KWndWindow*)uParam = Êó±êÖ¸Õë½øÈëµÄ×Ó´°¿ÚµÄÖ¸Õë

#define	WND_N_MOUSE_LEAVE_CHILD	WND_M_BASE + 9				//Êó±êÖ¸ÕëÀë¿ªÄ³×Ó´°¿Ú·¶Î§
//(KWndWindow*)uParam = Êó±êÖ¸ÕëÀë¿ªµÄ×Ó´°¿ÚµÄÖ¸Õë

#define	WND_M_SET_FOCUS			WND_M_BASE + 10				//ÉèÖÃÊäÈë½¹µã
//(KWndWindow*)uParam = Ô­À´ÓµÓĞÊäÈë½¹µãµÄ´°¿ÚµÄÖ¸Õë

#define	WND_M_KILL_FOCUS		WND_M_BASE + 11				//Ê§È¥ÊäÈë½¹µã
//(KWndWindow*)uParam = ½«ÒªÓµÓĞÊäÈë½¹µãµÄ´°¿ÚµÄÖ¸Õë

#define	WND_M_CAPTURECHANGED	WND_M_BASE + 12				//Ê§È¥¶ÔÊó±êÖ¸ÕëÏûÏ¢¸ú×Ù

#define	WND_M_HOTKEY			WND_M_BASE + 13				//ÈÈ¼ü
//uParam = ÈÈ¼üID

#define WND_M_POPUPMENU			WND_M_BASE + 14				//ÓÒ¼ü²Ëµ¥µ¯³ö
//(KWndWindow*)uParam = ±»ÓÒ¼üµã»÷µÄ×Ó´°¿ÚµÄÖ¸Õë
//	(short)(nParam & 0xffff) = xÆÁÄ»×ø±ê
//	(short)((nParam >> 16) & 0xffff) = yÆÁÄ»×ø±ê


#define WND_N_ITEM_PICKDROP		WND_M_BASE + 17				//¼ñÆğ/·ÅÏÂÎïÆ·
//(ITEM_PICKDROP_PLACE*)uParam = Óû¼ñÆğÎïÆ·µÄËùÔÚÎ»ÖÃ£¬ÈçÎŞÎïÆ·Òª±»¼ñÆğ£¬ÔòuParam = 0
//(ITEM_PICKDROP_PLACE*)nParam = Óû·ÅÏÂÎïÆ·µÄ·ÅÏÂÎ»ÖÃ£¬ÈçÎŞ¶«Î÷Òª±»·ÅÏÂ£¬ÔònParam = 0

#define WND_N_LEFT_CLICK_ITEM	WND_M_BASE + 18				//×ó¼üµã°´ÎïÆ·¶ÔÏó
//(const KUiDraggedObject*)uParam = ±»µã°´µÄÎïÆ·µÄĞÅÏ¢
//(KWndWindow*)nParam = ÎïÆ·ËùÔÚµÄ´°¿ÚµÄÖ¸Õë
#define WND_N_RIGHT_CLICK_ITEM	WND_M_BASE + 19				//ÓÒ¼üµã°´ÎïÆ·¶ÔÏó
//²ÎÊıº¬ÒåÍ¬WND_N_LEFT_CLICK_ITEM

#define WND_N_CHILD_MOVE		WND_M_BASE + 20				//×Ó´°¿Ú±»ÍÏ¶¯ÁË
//(KWndWindow*)uParam = ±»ÍÏ¶¯ÁËµÄ×Ó´°¿ÚµÄÖ¸Õë
//µ±nParam != 0 Ê±
//	(short)(nParam & 0xffff) = ºáÏòÒÆ¶¯µÄ¾àÀë
//	(short)((nParam >> 16) & 0xffff) = ×İÏòÒÆ¶¯µÄ¾àÀë
//µ±nParam == 0 Ê±£¬±íÊ¾´°¿ÚµÄÍÏ¶¯²Ù×÷½áÊøÁË

//------------------------------------------------
//	°´¼üÏûÏ¢
#define WND_M_BUTTON_BASE			WND_M_BASE + 100
#define WND_N_BUTTON_CLICK			WND_M_BUTTON_BASE + 1	//µã»÷°´Å¥²Ù×÷
//(KWndWindow*)uParam = ±»µã»÷µÄ°´Å¥´°¿ÚµÄÖ¸Õë
//nParam = ¶ÔÓÚCheckBoxÀàĞÍµÄ°´Å¥£¬´Ë²ÎÊı±íÊ¾°´Å¥±»ºóÊÇ·ñ´¦ÓÚ°´ÏÂ×´Ì¬

#define	WND_N_BUTTON_DOWN			WND_M_BUTTON_BASE + 2	//°´Å¥±»°´ÏÂ²Ù×÷
//(KWndWindow*)uParam = ±»µã»÷µÄ°´Å¥´°¿ÚµÄÖ¸Õë

#define	WND_N_BUTTON_MR_DOWN		WND_M_BUTTON_BASE + 3	//Êó±êÓÒ°´¼ü°´ÏÂ
//(KWndWindow*)uParam = ±»µã»÷µÄ°´Å¥´°¿ÚµÄÖ¸Õë

#define	WND_N_BUTTON_HOLD			WND_M_BUTTON_BASE + 4	//°´Å¥±»³ÖĞø°´×¡²»·Å
//(KWndWindow*)uParam = ±»°´×¡µÄ°´Å¥´°¿ÚµÄÖ¸Õë

#define WND_N_BUTTON_OVER			WND_M_BUTTON_BASE + 5	//Êó±êÒÆµ½°´Å¥ÉÏ
//(KWndWindow*)uParam = °´Å¥´°¿ÚµÄÖ¸Õë

#define WND_N_BUTTON_DB_CLICK		WND_M_BUTTON_BASE + 6	//Ë«»÷°´Å¥²Ù×÷

#define WND_N_BUTTON_RCLICK			WND_M_BUTTON_BASE + 7	//Ë«»÷°´Å¥²Ù×÷
//(KWndWindow*)uParam = ±»µã»÷µÄ°´Å¥´°¿ÚµÄÖ¸Õë
//¶ÔÓÚCheckBoxÀàĞÍµÄ°´Å¥²»»áÉú³É´ËÏûÏ¢

//------------------------------------------------
// Scroll BarÏûÏ¢
#define WND_M_SCORLLBAR_BASE		WND_M_BASE + 200
#define WND_N_SCORLLBAR_POS_CHANGED	WND_M_SCORLLBAR_BASE + 1//¹ö¶¯Ìõ»¬¿éÎ»ÖÃ·¢Éú±ä»¯
//(KWndWindow*)uParam = ±»µã»÷µÄ¹ö¶¯Ìõ´°¿ÚµÄÖ¸Õë
//nParam = ¹ö¶¯Ìõ»¬¿éÎ»ÖÃ¹ØÁªµÄÊıÖµ

//------------------------------------------------
// ÎÄ±¾±à¼­¿òÏûÏ¢
#define	WND_M_EDIT_BASE				WND_M_BASE + 300
#define	WND_N_EDIT_CHANGE			WND_M_EDIT_BASE + 1		//ÎÄ±¾±à¼­´°¿ÚÄÚµÄÄÚÈİ·¢ÉúÁË±ä»¯
//(KWndWindow*)uParam = ÎÄ±¾±à¼­¿ò´°¿ÚµÄÖ¸Õë

#define	WND_N_EDIT_SPECIAL_KEY_DOWN	WND_M_EDIT_BASE + 2		//ÎÄ±¾±à¼­´°¿Ú»ñµÃÌØ¶¨µÄ°´¼ü²Ù×÷
//(KWndWindow*)uParam = ÎÄ±¾±à¼­¿ò´°¿ÚµÄÖ¸Õë
//nParam = °´¼üµÄvisual key code
//ÌØ¶¨°´¼ü°üÀ¨ »Ø³µ¼ü¡¢Esc¼ü¡¢ÖÆ±í·ûtab¼ü¡¢ÏòÉÏ¼üÍ·¼ü¡¢ÏòÏÂ¼üÍ·¼ü¡¢Ç°·­Ò³¼ü£¬ºó·­Ò³¼ü

//------------------------------------------------
// ÁĞ±í¿ò´°¿ÚÏûÏ¢
#define	WND_M_LIST_BASE				WND_M_BASE + 400
#define	WND_N_LIST_ITEM_SEL			WND_M_LIST_BASE + 1		//ÁĞ±í¿òÑ¡ÖĞÏî·¢Éú±ä»»
//(KWndWindow*)uParam = ÁĞ±í¿ò´°¿ÚµÄÖ¸Õë
//nParam = ĞÂÑ¡ÔñµÄÏîµÄË÷Òı
#define	WND_N_LIST_ITEM_ACTIVE		WND_M_LIST_BASE + 2		//ÁĞ±í¿òÑ¡ÖĞÏî±»¼¤»î£¨µã×ó¼ü»ò°´enter£©
//²ÎÊıº¬ÒåÍ¬WND_N_LIST_ITEM_SEL
#define	WND_N_LIST_ITEM_D_CLICK		WND_M_LIST_BASE + 3
//(KWndWindow*)uParam = ÁĞ±í¿ò´°¿ÚµÄÖ¸Õë
//nParam = Ë«»÷µÄÏîµÄË÷Òı
#define WND_N_LIST_ITEM_R_CLICK		WND_M_LIST_BASE + 4
//(KWndWindow*)uParam = ÁĞ±í¿ò´°¿ÚµÄÖ¸Õë
//nParam = ÓÒ¼üµã»÷µÄÏîµÄË÷Òı
#define	WND_N_LIST_ITEM_HIGHLIGHT	WND_M_LIST_BASE + 5		//ÁĞ±í¿òÑ¡Êó±êÖ¸ÕëÖ¸ÏòµÄÏî·¢Éú±ä»»
//(KWndWindow*)uParam = ÁĞ±í¿ò´°¿ÚµÄÖ¸Õë
//nParam = ÁĞ±í¿òÑ¡Êó±êÖ¸ÕëÖ¸ÏòµÄÏîµÄË÷Òı

//====½çÃæ²Ù×÷½á¹û·µ»ØÖµ¶¨Òå====
#define WND_OPER_RESULT_0		0		//½çÃæ²Ù×÷·µ»ØµÄ½á¹û0
#define WND_OPER_RESULT(n)		(n)		//½çÃæ²Ù×÷·µ»ØµÄ½á¹û

// flying add the macro so that my compile work can continue.
// in m$ header file, there's this condition: 
// #if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
#if !defined WM_MOUSEWHEEL
#define WM_MOUSEWHEEL                   0x020A
#endif
#if !defined WHEEL_DELTA
#define WHEEL_DELTA                     120
#endif

#include "../../../Represent/iRepresent/KRepresentUnit.h"
struct KUiDraggedObject
{
	unsigned int	uGenre;
	unsigned int	uId;
	int				DataX;
	int				DataY;
	int				DataW;
	int				DataH;

	//start sö dông vÏ vßng s¸ng item add by Fong KiÒu
	int m_posvs;
	unsigned char m_dirvs;
	unsigned char m_vs_lastframe;
	//khëi t¹o gi¸ trŞ ban ®Çu 
	KUiDraggedObject()
	{
		m_posvs = 0;
		m_dirvs = 0;
		m_vs_lastframe = 0;
	};
	//end sö dông vÏ vßng s¸ng item

};

//=========================
//	ÃèÊö¶ÔÏóÎ»ÖÃµÄ½á¹¹
//=========================
struct ITEM_PICKDROP_PLACE
{
	class KWndWindow*	pWnd;
	int					h;
	int					v;
};
