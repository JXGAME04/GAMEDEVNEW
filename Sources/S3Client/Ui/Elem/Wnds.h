/*****************************************************************************************
//	½£ÏÀÒıÇæ£¬½çÃæ´°¿ÚÔªËØÌåÏµÍ·ÎÄ¼ş
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-7-9
------------------------------------------------------------------------------------------
	½çÃæÔªËØ£¨¿Ø¼ş£©µÄ¹²Í¬»ùÀàÎªKWndWindow£¬Wnd_Ç°×º´òÍ·µÄÒ»×éº¯ÊıÓÃÓÚÊ¹´°¿ÚĞÎ³ÉÒ»¸ö´ó¼¯ºÏ£¬
Íê³ÉÒ»Ğ©ÌØ¶¨µÄ¹¦ÄÜ¡£
    ´°¿ÚÏµÍ³Ö§³ÖÈÈ¼ü(HotKey)¹¦ÄÜ£¬×î¶àÍ¬Ê±Ö§³Ö24¸öÈÈ¼üÉèÖÃ£¬´ËÉÏÏŞÊıÖµ¿É±»ĞŞ¸Ä¡£
*****************************************************************************************/

class KWndWindow;
struct KUiDraggedObject;
struct iKWndGameSpace;

enum CURSOR_INDEX
{
	CURSOR_NORMAL = 0,
	CURSOR_POINT_TO_ATTACKABLE,	//ÒÆµ½¹¥»÷Ä¿±êÉÏ
	CURSOR_POINT_TO_OBJ_NPC,	//ÒÆµ½Ò»°ãOBJ»òNPCÉÏ
	CURSOR_PICKABLE,			//¿ÉÊ°È¡
	CURSOR_SKILL_ENABLE,		//ÒÆµ½¿ÉÊ¹ÓÃÔöÒæ¼¼ÄÜµÄ½ÇÉ«¶ÔÏóÉíÉÏ
	CURSOR_BUY,					//Âò¶«Î÷
	CURSOR_SELL,				//Âô¶«Î÷
	CURSOR_REPAIR,				//ĞŞ¶«Î÷
    CURSOR_USE,                 //¿´¶ÔÏóµÄÍ¼±ê
	CURSOR_INDEX_COUNT,
};

//--------------------------------------------------------------------------
//	»æÖÆ±»ÍÏÄÚÈİº¯ÊıÔ­ĞÍ
//--------------------------------------------------------------------------
//	²ÎÊı£º	x, y -> Êó±êÖ¸Õëµ±Ç°µÄ×ø±êÎ»ÖÃ
//			Obj -> ±»ÍÏ¶¯µÄ¶ÔÏóĞÅÏ¢½á¹¹
//			nDropQeuryResult -> µ±Ç°Ä¿±êµØ·½·µ»ØµÄ²éÑ¯¿É·ñ½ÓÊÜÍÏÀ´µÄ¶ÔÏóµÄ½á¹û
//	·µ»Ø£º±íÊ¾»æÖÆÍê±»ÍÏ¶¯µÄÎïÌåÖ®ºóÊÇ·ñÒª¼ÌĞø»æÖÆÊó±êÖ¸Õë¡£Èç¹ûÊó±êÖ¸Õëµ¥Ç°ÊôÓÚÒş²Ø×´Ì¬Ôò£¬´Ëº¯Êı·µ»ØÖµ±»ºöÂÔ
typedef int (*fnDrawDraggedObj)(int x, int y, const KUiDraggedObject& Obj, int nDropQueryResult);

enum WND_LAYER_LIST
{
	WL_LOWEST,		//×îµ×²ã
	WL_NORMAL,		//Ò»°ã
	WL_TOPMOST,		//×î¶¥²ã
};

void		Wnd_Cleanup();			//Çå³ı¹¤×÷£¬Ïú»ÙÈ«²¿µÄ´°¿Ú

void		Wnd_GetScreenSize(int& nWidth, int& nHeight);//»ñÈ¡ÆÁÄ»·¶Î§´óĞ¡
void		Wnd_SetScreenSize(int nWidth, int nHeight);  //ÉèÖÃÆÁÄ»·¶Î§´óĞ¡
void		Wnd_RenderWindows();						//»æÖÆ½çÃæÏµÍ³
void		Wnd_AddWindow(KWndWindow* pWnd, WND_LAYER_LIST eLayer = WL_NORMAL);	//Ìí¼Ó´°¿Ú
void		Wnd_OnWindowDelete(KWndWindow* pWnd);		//ÏìÓ¦´°¿ÚµÄÉ¾³ı
void		Wnd_SetGameSpaceWnd(KWndWindow* pWnd);	//ÉèÖÃÔËĞĞÓÎÏ·´°¿Ú
void		Wnd_GameSpaceHandleInput(bool bHandle);
void		Wnd_TransmitInputToGameSpace(unsigned int uMsg, unsigned int uParam, int nParam);
void		Wnd_ShowHideGameSpace(bool bShow);
//----ÊäÈëÊÂ¼ş´¦Àí----
//´¦ÀíÊäÈë£¬½çÃæÏµÍ³Èç¹û´¦ÀíÁËÊäÈëÊÂ¼ş£¬Ôò·µ»Ø·Ç0Öµ£¬·ñÔò·µ»Ø0Öµ¡£
void		Wnd_ProcessInput(unsigned int uMsg, unsigned int uParam, int nParam);
void		Wnd_Heartbeat();
//----Êó±êÖ¸Õë²Ù×÷----
void		Wnd_ShowCursor(int bShow);						//¸Ä±äÊó±êÖ¸ÕëÏÔÊ¾×´Ì¬
void		Wnd_SetCursorPos(int x, int y);					//ÉèÖÃÊó±êÖ¸ÕëÎ»ÖÃ
void		Wnd_GetCursorPos(int* px, int* py);				//»ñÈ¡Êó±êÖ¸ÕëÎ»ÖÃ
void		Wnd_LoadCursor(int nIndex, const char* pImgFile);//ÔØÈëÊó±êÖ¸Õë×ÊÔ´
int			Wnd_SwitchCursor(int nIndex);					//ÇĞ»»µ±Ç°Êó±êÖ¸ÕëÍ¼ĞÎ
void		Wnd_RestoreCursor();							//»¹Ô­Êó±êÍ¼ĞÎ
//----ÍÏ¶¯¶ÔÏó----
//¿ªÊ¼ÍÏ¶¯ÎïÌå
int			Wnd_DragBegin(const KUiDraggedObject* pObj, fnDrawDraggedObj fnDrawObjFunc);
int			Wnd_GetDragObj(KUiDraggedObject* pObj);
void		Wnd_DragFinished();								//½áÊøÍÏ¶¯×´Ì¬
//----´¦ÀíÊäÈë½¹µã´°¿Ú----
void		Wnd_SetFocusWnd(KWndWindow* pWnd);				//ÉèÖÃÊäÈë½¹µã´°¿Ú
KWndWindow*	Wnd_GetFocusWnd();								//»ñÈ¡ÊäÈë½¹µã´°¿Ú
//----¸ú×ÙÊó±êÊÂ¼ş----
void		Wnd_SetCapture(KWndWindow* pWnd);				//ÉèÖÃ¸ú×ÙÊó±ê¶¯×÷´°¿Ú
void		Wnd_ReleaseCapture();							//ÊÍ·Å´°¿Ú¶ÔÊó±ê¶¯×÷µÄ¸ú×Ù
//----´¦Àí´°¿Ú¶ÀÕ¼----
void		Wnd_SetExclusive(KWndWindow* pWnd);				//ChØ hiÖn thŞ UI trªn cïng c¸c ui d­íi kho¸ hÕt
void		Wnd_ReleaseExclusive(KWndWindow* pWnd);			//ÊÍ·Å´°¿ÚµÄ¶ÀÕ¼×´Ì¬
