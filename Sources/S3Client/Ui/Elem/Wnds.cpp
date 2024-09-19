/*****************************************************************************************
//	Copyright : Kingsoft 2002
//	Author	:   Fong KiÒu
//	CreateTime:	2020-7-9
*****************************************************************************************/
#include "KWin32.h"

#include "KEngine.h"
#include "KWin32Wnd.h"
#include "../Elem/WndMessage.h"
#include "Wnds.h"
#include "WndWindow.h"
#include "UiCursor.h"
#include "WndGameSpace.h"
#include "MouseHover.h"
#include "PopupMenu.h"
#include "../UiBase.h"
#include "../ShortcutKey.h"

#include "CommCtrl.h"

int WND_SHOW_MOUSE_OVER_WND = false;

//=================
//		´°¿ÚÕ¾
//=================
static struct KWndStation
{
	KWndWindow		LowLayerRoot;
	KWndWindow		NormalLayerRoot;
	KWndWindow		TopLayerRoot;
	KWndWindow*		pFocusWnd;
	KWndWindow*		pMouseOverWnd;
	KWndWindow*		pCaptureMouseWnd;
	KWndWindow*		pExclusiveWnd[4];

	KUiCursor		Cursor;
	SIZE			ScreenSize;

	struct KDragInfo
	{
		int					bDragging;
		KUiDraggedObject	DraggedObj;
		fnDrawDraggedObj	fnDrawObjFnc;
		int					nDropQeuryRsult;
	}		DragInfo;
	
	KWndWindow*		pGameSpaceWnd;	
	unsigned char	bGameSpaceNotHandleInput;
	bool			bPaintGameSpace;

	KWndStation()
	{
		pFocusWnd        = NULL;
		pMouseOverWnd    = NULL;
		pCaptureMouseWnd = NULL;
		pExclusiveWnd[0] = NULL;
		pExclusiveWnd[1] = NULL;
		pExclusiveWnd[2] = NULL;
		pExclusiveWnd[3] = NULL;
		pGameSpaceWnd	 = NULL;
		bGameSpaceNotHandleInput = 1;
		bPaintGameSpace = false;
		LowLayerRoot.Hide();
		NormalLayerRoot.Hide();
		TopLayerRoot.Hide();
		DragInfo.bDragging = false;
		DragInfo.DraggedObj.uGenre = 0;
		DragInfo.DraggedObj.uId = 0;
		ScreenSize.cx = 0;
		ScreenSize.cy = 0;
	}
}	s_WndStation;

static KWndWindow*	Wnd_GetActive(int x, int y, bool bBringToTop);

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÇå³ý¹¤×÷£¬Ïú»ÙÈ«²¿µÄ´°¿Ú
//--------------------------------------------------------------------------
void Wnd_Cleanup()
{
	Wnd_Heartbeat();

	s_WndStation.pFocusWnd = NULL;
	s_WndStation.pMouseOverWnd = NULL;
	s_WndStation.pCaptureMouseWnd = NULL;
	s_WndStation.pExclusiveWnd[0] = NULL;
	s_WndStation.pExclusiveWnd[1] = NULL;
	s_WndStation.pExclusiveWnd[2] = NULL;
	s_WndStation.pExclusiveWnd[3] = NULL;
	s_WndStation.pGameSpaceWnd = NULL;

	s_WndStation.Cursor.Cleanup();
	s_WndStation.DragInfo.bDragging = false;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º»æÖÆ´°¿Ú
//--------------------------------------------------------------------------
void Wnd_RenderWindows()
{
	int	bShowCursor = true;

	if (s_WndStation.pGameSpaceWnd && s_WndStation.bPaintGameSpace)
		s_WndStation.pGameSpaceWnd->Paint();
	s_WndStation.LowLayerRoot.Paint();
	s_WndStation.NormalLayerRoot.Paint();
	s_WndStation.TopLayerRoot.Paint();
	
	if (s_WndStation.DragInfo.bDragging &&
		s_WndStation.DragInfo.fnDrawObjFnc)
	{
		int h, v;
		s_WndStation.Cursor.GetPosition(h, v);
		if (s_WndStation.DragInfo.fnDrawObjFnc(h, v,
			s_WndStation.DragInfo.DraggedObj,
			s_WndStation.DragInfo.nDropQeuryRsult) == 0)
			bShowCursor = false;
	}
	KPopupMenu::PaintMenu();
	g_MouseOver.PaintMouseHoverInfo();

	if (WND_SHOW_MOUSE_OVER_WND && s_WndStation.pMouseOverWnd)
		s_WndStation.pMouseOverWnd->PaintDebugInfo();

	if (bShowCursor)
		s_WndStation.Cursor.Paint();
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÈÃ´°¿Ú»î¶¯
//--------------------------------------------------------------------------
void Wnd_Heartbeat()
{
	s_WndStation.LowLayerRoot.LetMeBreathe();
	s_WndStation.NormalLayerRoot.LetMeBreathe();
	s_WndStation.TopLayerRoot.LetMeBreathe();
	if (s_WndStation.pExclusiveWnd[0])
		s_WndStation.pExclusiveWnd[0]->BringToTop();
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÌí¼Ó´°¿Ú
//--------------------------------------------------------------------------
void Wnd_AddWindow(KWndWindow* pWnd, WND_LAYER_LIST eLayer/* = WL_NORMAL*/)
{
	if (eLayer == WL_LOWEST)
		s_WndStation.LowLayerRoot.AddBrother(pWnd);
	else if (eLayer == WL_TOPMOST)
		s_WndStation.TopLayerRoot.AddBrother(pWnd);
	else
		s_WndStation.NormalLayerRoot.AddBrother(pWnd);	
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÉèÖÃÔËÐÐÓÎÏ·´°¿Ú
//--------------------------------------------------------------------------
void Wnd_SetGameSpaceWnd(KWndWindow* pWnd)
{
	s_WndStation.pGameSpaceWnd = pWnd;
}

void Wnd_GameSpaceHandleInput(bool bHandle)
{
/*	if (bHandle)
	{
		if (s_WndStation.bGameSpaceNotHandleInput)
			s_WndStation.bGameSpaceNotHandleInput --;
	}
	else
		s_WndStation.bGameSpaceNotHandleInput ++;
*/
	s_WndStation.bGameSpaceNotHandleInput = !bHandle;
}

void Wnd_ShowHideGameSpace(bool bShow)
{
	s_WndStation.bPaintGameSpace = bShow;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÏìÓ¦´°¿ÚÎö¹¹
//--------------------------------------------------------------------------
void Wnd_OnWindowDelete(KWndWindow* pWnd)
{
	if (pWnd)
	{		
		if (s_WndStation.pMouseOverWnd == pWnd)
			s_WndStation.pMouseOverWnd = NULL;
		if (s_WndStation.pCaptureMouseWnd == pWnd)
			s_WndStation.pCaptureMouseWnd = NULL;
		if (s_WndStation.pFocusWnd == pWnd)
			s_WndStation.pFocusWnd = NULL;
		if (s_WndStation.pExclusiveWnd[0] == pWnd)
		{
			s_WndStation.pExclusiveWnd[0] = s_WndStation.pExclusiveWnd[1];
			s_WndStation.pExclusiveWnd[1] = s_WndStation.pExclusiveWnd[2];
			s_WndStation.pExclusiveWnd[2] = s_WndStation.pExclusiveWnd[3];
			s_WndStation.pExclusiveWnd[3] = NULL;
		}
		else if (s_WndStation.pExclusiveWnd[1] == pWnd)
		{
			s_WndStation.pExclusiveWnd[1] = s_WndStation.pExclusiveWnd[2];
			s_WndStation.pExclusiveWnd[2] = s_WndStation.pExclusiveWnd[3];
			s_WndStation.pExclusiveWnd[3] = NULL;
		}
		else if (s_WndStation.pExclusiveWnd[2] == pWnd)
		{
			s_WndStation.pExclusiveWnd[2] = s_WndStation.pExclusiveWnd[3];
			s_WndStation.pExclusiveWnd[3] = NULL;
		}
		else if (s_WndStation.pExclusiveWnd[3] == pWnd)
			s_WndStation.pExclusiveWnd[3] = NULL;
		else
			KPopupMenu::OnWndDelete(pWnd);
	}
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¿ªÊ¼ÍÏ¶¯ÎïÌå
//--------------------------------------------------------------------------
int Wnd_DragBegin(const KUiDraggedObject* pObj, fnDrawDraggedObj fnDrawObjFunc)
{
	//Ô­À´ÓÐ¸ö¶ÔÏóÕýÔÚ±»ÍÏ¶¯
	if (s_WndStation.DragInfo.bDragging == false && pObj && pObj->uGenre)
	{
		s_WndStation.DragInfo.DraggedObj = *pObj;
		s_WndStation.DragInfo.fnDrawObjFnc = fnDrawObjFunc;
		s_WndStation.DragInfo.bDragging = true;
		return true;
	}
	return false;
}

int	Wnd_GetDragObj(KUiDraggedObject* pObj)
{
	if (pObj)
	{
		if (s_WndStation.DragInfo.bDragging)
			memcpy(pObj, &s_WndStation.DragInfo.DraggedObj, sizeof(KUiDraggedObject));
		else
			memset(pObj, 0, sizeof(KUiDraggedObject));
	}
	return s_WndStation.DragInfo.bDragging;
}
//--------------------------------------------------------------------------
//	¹¦ÄÜ£º½áÊøÍÏ¶¯×´Ì¬
//--------------------------------------------------------------------------
void Wnd_DragFinished()
{
	s_WndStation.DragInfo.bDragging = false;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º´¦ÀíÊäÈë
//--------------------------------------------------------------------------
void Wnd_ProcessInput(unsigned int uMsg, unsigned int uParam, int nParam)
{
	KWndWindow* pActiveWnd = NULL;
	static bool bLastCursorEventProcessedByGameSpace = false;

	//====Êó±êÖ¸ÕëÏûÏ¢====
	if ((uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) || uMsg == WM_MOUSEHOVER)
	{
		int x = 0;
		int y = 0;

		if (uMsg != WM_MOUSEWHEEL)
		{
			x = LOWORD(nParam);
			y = HIWORD(nParam);
		}
		else
		{
			s_WndStation.Cursor.GetPosition(x, y);
		}
		//====ÉèÖÃÊó±êÖ¸ÕëÍ¼ÐÎÎ»ÖÃ====
		s_WndStation.Cursor.SetPosition(x, y);

		if (KPopupMenu::HandleInput(uMsg, uParam, nParam))
		{
			if (bLastCursorEventProcessedByGameSpace)
			{
				bLastCursorEventProcessedByGameSpace = false;
				g_UiBase.SetCurrentCursor();
			}
			return;
		}
		g_MouseOver.UpdateCursorPos(x, y);
		
		if (s_WndStation.pCaptureMouseWnd)
			pActiveWnd = s_WndStation.pCaptureMouseWnd;
		else if (s_WndStation.pExclusiveWnd[0] == NULL)
		{
			//====È¡µÃÊó±êËùÖ¸µÄ×îÉÏ²ã´°¿ÚÃæ°å====
			//====ÈÃ±»Êó±êµã»÷µÄ´°¿Ú³ÉÎª×îÉÏ²ã´°¿ÚÃæ°å====
			if ((uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN || uMsg == WM_MBUTTONDOWN) &&
				s_WndStation.DragInfo.bDragging == 0)
				pActiveWnd = Wnd_GetActive(x, y, true);
			else
				pActiveWnd = Wnd_GetActive(x, y, false);
		}
		else	//ÓÐ¶ÀÕ¼´°¿ÚÊ±£¬Active´°¿Ú¾ÍÊÇËüÁË
			pActiveWnd = s_WndStation.pExclusiveWnd[0];

		{
			KWndWindow* pTopWnd = NULL;
			if (pActiveWnd)			
				pTopWnd = pActiveWnd->TopChildFromPoint(x, y);	//Ö»´«¸øÎ»ÓÚÊó±êÖ¸ÕëÏÂ·½µÄ×îÉÏ²ãµÄ´°¿Ú
			if (pTopWnd != s_WndStation.pMouseOverWnd)
			{
				g_MouseOver.CancelMouseHoverInfo();
				KWndWindow* pOverParent = NULL;
				if (s_WndStation.pMouseOverWnd)
				{
					s_WndStation.pMouseOverWnd->WndProc(WND_M_MOUSE_LEAVE, 0, 0);
					pOverParent = s_WndStation.pMouseOverWnd->GetParent();
					if (pOverParent)
					{
						pOverParent->WndProc(WND_N_MOUSE_LEAVE_CHILD,
							(unsigned int)s_WndStation.pMouseOverWnd, 0);
					}
				}
				if (pTopWnd)
				{
					pTopWnd->WndProc(WND_M_MOUSE_ENTER, 0, 0);
					pOverParent = pTopWnd->GetParent();
					if (pOverParent)
					{
						pOverParent->WndProc(WND_N_MOUSE_ENTER_CHILD,
							(unsigned int)pTopWnd, 0);
					}
				}
				s_WndStation.pMouseOverWnd = pTopWnd;
			}

			if (s_WndStation.pFocusWnd && pTopWnd != s_WndStation.pFocusWnd &&
				(uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN || uMsg == WM_MBUTTONDOWN))
			{
				Wnd_SetFocusWnd(NULL);
			}
		}

		bool bPrecessedByGameSpace = false;
		if (s_WndStation.pMouseOverWnd)
			s_WndStation.pMouseOverWnd->WndProc(uMsg, uParam, nParam);
		else if (s_WndStation.pCaptureMouseWnd)
			s_WndStation.pCaptureMouseWnd->WndProc(uMsg, uParam, nParam);
		else if (s_WndStation.pExclusiveWnd[0] == NULL &&
			s_WndStation.bGameSpaceNotHandleInput == false && s_WndStation.pGameSpaceWnd)
		{
			s_WndStation.pGameSpaceWnd->WndProc(uMsg, uParam, nParam);
			bLastCursorEventProcessedByGameSpace = true;
			bPrecessedByGameSpace = true;
		}
		if (bPrecessedByGameSpace == false && bLastCursorEventProcessedByGameSpace)
		{
			bLastCursorEventProcessedByGameSpace = false;
			g_UiBase.SetCurrentCursor();
		}
	}
	else
	{//====´¦ÀíÆäËûÏûÏ¢£¨ÆäÊµ¶¼ÊÇ¼üÅÌÊäÈëÏûÏ¢£©====
		if (KPopupMenu::HandleInput(uMsg, uParam, nParam))
			return;

		if (s_WndStation.pFocusWnd && s_WndStation.pFocusWnd->WndProc(uMsg, uParam, nParam))
			return;
		if (s_WndStation.pExclusiveWnd[0])
		{
			s_WndStation.pExclusiveWnd[0]->WndProc(uMsg, uParam, nParam);
			return;
		}
		if (uMsg == WM_KEYDOWN || (uMsg == WM_KEYUP && uParam == VK_SNAPSHOT))
		{
			int nModifier = 0;
			if (GetKeyState(VK_CONTROL) & 0x8000)
				nModifier |= HOTKEYF_CONTROL;

			if (GetKeyState(VK_SHIFT) & 0x8000)
				nModifier |= HOTKEYF_SHIFT;

			if (GetKeyState(VK_MENU) & 0x8000)
				nModifier |= HOTKEYF_ALT;

			if (KShortcutKeyCentre::HandleKeyInput(uParam, nModifier))
				return;
		}
		else if (uMsg == WM_SYSKEYDOWN)
		{
			int nModifier = 0;
			if (GetKeyState(VK_CONTROL) & 0x8000)
				nModifier |= HOTKEYF_CONTROL;

			if (GetKeyState(VK_SHIFT) & 0x8000)
				nModifier |= HOTKEYF_SHIFT;

			if (GetKeyState(VK_MENU) & 0x8000)
				nModifier |= HOTKEYF_ALT;

			if (KShortcutKeyCentre::HandleKeyInput(uParam, nModifier))
				return;
		}

		int	bHandle = false;		
		for (int i = 0; i < 3 && bHandle == false; i++)
		{
			if (i == 0)
				pActiveWnd = s_WndStation.TopLayerRoot.GetNextWnd();
			else if (i == 1)
				pActiveWnd = s_WndStation.NormalLayerRoot.GetNextWnd();
			else
				pActiveWnd = s_WndStation.LowLayerRoot.GetNextWnd();

			while(pActiveWnd)
			{
				KWndWindow* pNextWnd = pActiveWnd->GetNextWnd();
				if (pActiveWnd->IsVisible() &&
					pActiveWnd->WndProc(uMsg, uParam, nParam))
				{
					bHandle = true;
					break;
				}
				pActiveWnd = pNextWnd;
			}
		}
		
		if (bHandle == false && s_WndStation.pGameSpaceWnd &&
			s_WndStation.bGameSpaceNotHandleInput == false)
		{
			s_WndStation.pGameSpaceWnd->WndProc(uMsg, uParam, nParam);
		}
	}
}

void Wnd_TransmitInputToGameSpace(unsigned int uMsg, unsigned int uParam, int nParam)
{
	s_WndStation.pGameSpaceWnd->WndProc(uMsg, uParam, nParam);
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º°Ñ´¦ÓÚÄ³¸öÎ»ÖÃµÄ´°¿Ú´øµ½×îÉÏ²ã
//	·µ»Ø£º×îÉÏ²ã´°¿ÚµÄÖ¸Õë
//--------------------------------------------------------------------------
KWndWindow* Wnd_GetActive(int x, int y, bool bBringToTop)
{
	KWndWindow* pActive = NULL;
	for (int i = 0; i < 3; i++)
	{		
		KWndWindow*	pWnd = NULL;
		if (i == 0)
			pWnd = &s_WndStation.TopLayerRoot;
		else if (i == 1)
			pWnd = &s_WndStation.NormalLayerRoot;
		else
			pWnd = &s_WndStation.LowLayerRoot;

		while(pWnd = pWnd->GetNextWnd())
		{
			if (pWnd->IsVisible())
				pActive = pWnd;
		}
		while(pActive)
		{
			if (pActive->PtInWindow(x, y))
			{
				if (i == 1 && bBringToTop)
					pActive->BringToTop();
				break;
			}
			pActive = pActive->GetPreWnd();
		}
		if (pActive)
			break;
	}
	return pActive;
}

//¸Ä±äÊó±êÖ¸ÕëÏÔÊ¾×´Ì¬
void Wnd_ShowCursor(int bShow)
{
	s_WndStation.Cursor.Show(bShow);
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÉèÖÃÊó±êÖ¸ÕëÎ»ÖÃ
//--------------------------------------------------------------------------
void Wnd_SetCursorPos(int x, int y)
{
	s_WndStation.Cursor.SetPosition(x, y);
	
	HWND	hWnd = g_GetMainHWnd();
	POINT	Pos;
	Pos.x = x;
	Pos.y = y;
	::ClientToScreen(hWnd, &Pos);
	::SetCursorPos(Pos.x, Pos.y);
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º»ñÈ¡Êó±êÖ¸ÕëÎ»ÖÃ
//--------------------------------------------------------------------------
void Wnd_GetCursorPos(int* px, int* py)
{
	int h, v;
	s_WndStation.Cursor.GetPosition(h, v);
	if (px)
		*px = h;
	if (py)
		*py = v;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º»ñÈ¡µ±Ç°¹Ò½ÓµÄÊäÈë½¹µã´°¿Ú
//--------------------------------------------------------------------------
KWndWindow* Wnd_GetFocusWnd()
{
	return s_WndStation.pFocusWnd;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¹Ò½ÓÊäÈë½¹µã´°¿Ú
//--------------------------------------------------------------------------
void Wnd_SetFocusWnd(KWndWindow* pWnd)
{
	if (s_WndStation.pFocusWnd != pWnd)
	{
		if (s_WndStation.pFocusWnd)
			s_WndStation.pFocusWnd->WndProc(WND_M_KILL_FOCUS, (unsigned int)pWnd, 0);
		if (pWnd)
			pWnd->WndProc(WND_M_SET_FOCUS, (unsigned int)s_WndStation.pFocusWnd, 0);
		s_WndStation.pFocusWnd = pWnd;
	}
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÉèÖÃ¸ú×ÙÊó±ê¶¯×÷´°¿Ú
//--------------------------------------------------------------------------
void Wnd_SetCapture(KWndWindow* pWnd)
{
	if (s_WndStation.pCaptureMouseWnd != pWnd)
	{
		Wnd_ReleaseCapture();
		s_WndStation.pCaptureMouseWnd = pWnd;
	}
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÊÍ·Å´°¿Ú¶ÔÊó±ê¶¯×÷µÄ¸ú×Ù
//--------------------------------------------------------------------------
void Wnd_ReleaseCapture()
{
	if (s_WndStation.pCaptureMouseWnd)
	{
		s_WndStation.pCaptureMouseWnd->WndProc(WND_M_CAPTURECHANGED, 0, 0);
		s_WndStation.pCaptureMouseWnd = NULL;
	}
}

//--------------------------------------------------------------------------
//	//ChØ hiÖn thÞ UI trªn cïng c¸c ui d­íi kho¸ hÕt
//--------------------------------------------------------------------------
void Wnd_SetExclusive(KWndWindow* pWnd)
{
	int i;
	if (pWnd)
	{
		if (s_WndStation.pFocusWnd && s_WndStation.pFocusWnd->GetOwner() != pWnd->GetOwner())
			Wnd_SetFocusWnd(NULL);

		for (i = 0; i < 4; i++)
		{
			if (s_WndStation.pExclusiveWnd[i] == pWnd)
			{
				for (int j = i; j > 0; j--)
					s_WndStation.pExclusiveWnd[j] = s_WndStation.pExclusiveWnd[j - 1];
				s_WndStation.pExclusiveWnd[0] = pWnd;
				return;
			}
		}
		s_WndStation.pExclusiveWnd[3] = s_WndStation.pExclusiveWnd[2];
		s_WndStation.pExclusiveWnd[2] = s_WndStation.pExclusiveWnd[1];
		s_WndStation.pExclusiveWnd[1] = s_WndStation.pExclusiveWnd[0];
		s_WndStation.pExclusiveWnd[0] = pWnd;
	}
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÈ¡Ïû¶ÀÕ¼´°¿Ú
//--------------------------------------------------------------------------
void Wnd_ReleaseExclusive(KWndWindow* pWnd)
{
	if (pWnd)
	{
		for (int i = 0; i < 4; i++)
		{
			if (s_WndStation.pExclusiveWnd[i] == pWnd)
			{
				for (int j = i; j < 3; j++)
					s_WndStation.pExclusiveWnd[j] = s_WndStation.pExclusiveWnd[j + 1];
				s_WndStation.pExclusiveWnd[3] = NULL;
			}
		}
	}
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÔØÈëÊó±êÖ¸Õë×ÊÔ´
//--------------------------------------------------------------------------
void Wnd_LoadCursor(int nIndex, const char* pImgFile)
{
	s_WndStation.Cursor.SetImage(nIndex, pImgFile);
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÇÐ»»µ±Ç°Êó±êÖ¸ÕëÍ¼ÐÎ
//--------------------------------------------------------------------------
int Wnd_SwitchCursor(int nIndex)
{
	return s_WndStation.Cursor.SwitchImage(nIndex);
}


//ÉèÖÃÆÁÄ»·¶Î§´óÐ¡
void Wnd_SetScreenSize(int nWidth, int nHeight)
{
	s_WndStation.ScreenSize.cx = nWidth;
	s_WndStation.ScreenSize.cy = nHeight;
}

//»ñÈ¡ÆÁÄ»·¶Î§´óÐ¡
void Wnd_GetScreenSize(int& nWidth, int& nHeight)
{
	nWidth = s_WndStation.ScreenSize.cx;
	nHeight = s_WndStation.ScreenSize.cy;
}

//»¹Ô­Êó±êÍ¼ÐÎ
void	Wnd_RestoreCursor()
{
	s_WndStation.Cursor.RestoreCursor();
}