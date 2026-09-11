/*****************************************************************************************
//	Copyright : Kingsoft 2002
//	Author	:   Fong Ki襲
//	CreateTime:	2020-7-9
*****************************************************************************************/
#include "KWin32.h"

#include "KEngine.h"
#include "KWin32Wnd.h"
#include "../Elem/WndMessage.h"
#include "KDebug.h"	// [VATPHAM 12/09 g] g_DebugLog cho nhat ky chan doan
#include "Wnds.h"
#include "WndWindow.h"
#include "UiCursor.h"
#include "WndGameSpace.h"
#include "MouseHover.h"
#include "PopupMenu.h"
#include "UiToaDo.h"
#include "../UiBase.h"
#include "../ShortcutKey.h"

#include "CommCtrl.h"

int WND_SHOW_MOUSE_OVER_WND = false;

//=================
//		窗口站
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
	BOOL			bLBtnDown;
	UINT			uPKKeyDown;
	BOOL			bPKKeyDown;
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
		bLBtnDown = FALSE;
		uPKKeyDown = 0;
		bPKKeyDown = FALSE;
	}
}	s_WndStation;

static KWndWindow*	Wnd_GetActive(int x, int y, bool bBringToTop);

//--------------------------------------------------------------------------
//	功能：清除工作，销毁全部的窗口
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
//	功能：绘制窗口
//--------------------------------------------------------------------------
void Wnd_RenderWindows()
{
	int	bShowCursor = true;

	if (s_WndStation.pGameSpaceWnd && s_WndStation.bPaintGameSpace)
		s_WndStation.pGameSpaceWnd->Paint();
#ifdef JX_ANDROID
	// [ANDROID 09/09 NEO] Dat lai moi cua so GOC tu khung ve chuan 1024x768 sang man hinh that,
	// truoc khi ve. Moi cua so chi can mot lan (m_bNeedFit dat trong KWndWindow::Init).
	// Cua so ban do (pGameSpaceWnd) dat rieng, khong nam trong ba chuoi nay.
	{
		KWndWindow* apGoc[3] = { &s_WndStation.LowLayerRoot, &s_WndStation.NormalLayerRoot, &s_WndStation.TopLayerRoot };
		for (int nL = 0; nL < 3; nL++)
		{
			KWndWindow* pW = apGoc[nL]->GetNextWnd();
			while (pW)
			{
				pW->FitToScreen();
				pW = pW->GetNextWnd();
			}
		}
	}
#endif
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
	g_MouseOverCompare.PaintMouseHoverInfo();

	if (WND_SHOW_MOUSE_OVER_WND && s_WndStation.pMouseOverWnd)
		s_WndStation.pMouseOverWnd->PaintDebugInfo();

	UiToaDo_Ve();		// [UITOADO] khung + dong huong dan khi dang sua

	if (bShowCursor)
		s_WndStation.Cursor.Paint();
}

//--------------------------------------------------------------------------
//	功能：让窗口活动
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
//	功能：添加窗口
//--------------------------------------------------------------------------
void Wnd_AddWindow(KWndWindow* pWnd, WND_LAYER_LIST eLayer/* = WL_NORMAL*/)
{
	if (eLayer == WL_LOWEST)
		s_WndStation.LowLayerRoot.AddBrother(pWnd);
	else if (eLayer == WL_TOPMOST)
		s_WndStation.TopLayerRoot.AddBrother(pWnd);
	else
		s_WndStation.NormalLayerRoot.AddBrother(pWnd);	

	// [UITOADO] cua so vua duoc dat vao he thong: ap toa do nguoi choi
	// da tu dat (neu co) de len toa do doc tu \Ui\ui3\*.ini
	UiToaDo_ApChoCuaSo(pWnd);
}

//--------------------------------------------------------------------------
//	[UITOADO] lay cua so goc cua mot lop, de duyet het cua so cap 1
//--------------------------------------------------------------------------
KWndWindow* Wnd_GetLayerRoot(int nLayer)
{
	if (nLayer == WL_LOWEST)
		return &s_WndStation.LowLayerRoot;
	if (nLayer == WL_TOPMOST)
		return &s_WndStation.TopLayerRoot;
	return &s_WndStation.NormalLayerRoot;
}

//--------------------------------------------------------------------------
//	功能：设置运行游戏窗口
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
//	功能：响应窗口析构
//--------------------------------------------------------------------------
void Wnd_OnWindowDelete(KWndWindow* pWnd)
{
	if (pWnd)
	{		
		// [UITOADO] bo moi con tro che do sua giao dien dang giu toi cua so nay
		UiToaDo_QuenCuaSo(pWnd);
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
//	功能：开始拖动物体
//--------------------------------------------------------------------------
int Wnd_DragBegin(const KUiDraggedObject* pObj, fnDrawDraggedObj fnDrawObjFunc)
{
	//原来有个对象正在被拖动
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
//	功能：结束拖动状态
//--------------------------------------------------------------------------
void Wnd_DragFinished()
{
	s_WndStation.DragInfo.bDragging = false;
}

//--------------------------------------------------------------------------
//	功能：处理输入
//--------------------------------------------------------------------------
void Wnd_ProcessInput(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)
{
	KWndWindow* pActiveWnd = NULL;
	static bool bLastCursorEventProcessedByGameSpace = false;

	//====鼠标指针消息====
	if ((uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) || uMsg == WM_MOUSEHOVER)
	{
		int x = 0;
		int y = 0;

		if (uMsg != WM_MOUSEWHEEL)
		{
			x = LOWORD(nParam);
			y = HIWORD(nParam);
			if(uMsg == WM_LBUTTONUP)
				s_WndStation.bLBtnDown = FALSE;
		}
		else
		{
			s_WndStation.Cursor.GetPosition(x, y);
		}
		//====设置鼠标指针图形位置====
		s_WndStation.Cursor.SetPosition(x, y);

		// [UITOADO] dang o che do sua giao dien thi chuot danh rieng cho
		// viec keo tha o giao dien, khong cho giao dien phia duoi nhan.
		if (UiToaDo_NhanChuot(uMsg, uParam, nParam))
			return;

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
			//====取得鼠标所指的最上层窗口面板====
			//====让被鼠标点击的窗口成为最上层窗口面板====
			if ((uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN || uMsg == WM_MBUTTONDOWN) &&
				s_WndStation.DragInfo.bDragging == 0)
				pActiveWnd = Wnd_GetActive(x, y, true);
			else
				pActiveWnd = Wnd_GetActive(x, y, false);
		}
		else	//有独占窗口时，Active窗口就是它了
			pActiveWnd = s_WndStation.pExclusiveWnd[0];

		{
			KWndWindow* pTopWnd = NULL;
			if (pActiveWnd)			
				pTopWnd = pActiveWnd->TopChildFromPoint(x, y);	//只传给位于鼠标指针下方的最上层的窗口
			if (pTopWnd != s_WndStation.pMouseOverWnd)
			{
				g_MouseOver.CancelMouseHoverInfo();
				g_MouseOverCompare.CancelMouseHoverInfo();
				KWndWindow* pOverParent = NULL;
				if (s_WndStation.pMouseOverWnd)
				{
					s_WndStation.pMouseOverWnd->WndProc(WND_M_MOUSE_LEAVE, 0, 0);
					pOverParent = s_WndStation.pMouseOverWnd->GetParent();
					if (pOverParent)
					{
						pOverParent->WndProc(WND_N_MOUSE_LEAVE_CHILD,
							(KUPARAM)s_WndStation.pMouseOverWnd, 0);
					}
				}
				if (pTopWnd)
				{
					pTopWnd->WndProc(WND_M_MOUSE_ENTER, 0, 0);
					pOverParent = pTopWnd->GetParent();
					if (pOverParent)
					{
						pOverParent->WndProc(WND_N_MOUSE_ENTER_CHILD,
							(KUPARAM)pTopWnd, 0);
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
	{//====处理其他消息（其实都是键盘输入消息）====
		if(uMsg == WM_KEYUP)
		{
			if(uParam == s_WndStation.uPKKeyDown)
			{
				s_WndStation.bPKKeyDown = FALSE;
			}
		}
		if (KPopupMenu::HandleInput(uMsg, uParam, nParam))
			return;

		if (s_WndStation.pFocusWnd && s_WndStation.pFocusWnd->WndProc(uMsg, uParam, nParam))
			return;
		if (s_WndStation.pExclusiveWnd[0])
		{
			s_WndStation.pExclusiveWnd[0]->WndProc(uMsg, uParam, nParam);
			return;
		}
		if(uMsg == WM_KEYDOWN)
		{
			if(uParam == s_WndStation.uPKKeyDown
			&& !(GetKeyState(VK_CONTROL) & 0x8000)
			&& !(GetKeyState(VK_SHIFT) & 0x8000)
			&& !(GetKeyState(VK_MENU) & 0x8000)
			)
			{
				s_WndStation.bPKKeyDown = TRUE;
			}
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

void Wnd_TransmitInputToGameSpace(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)
{
	s_WndStation.pGameSpaceWnd->WndProc(uMsg, uParam, nParam);
}

//--------------------------------------------------------------------------
//	功能：把处于某个位置的窗口带到最上层
//	返回：最上层窗口的指针
//--------------------------------------------------------------------------
#ifdef JX_ANDROID
// [ANDROID 09/09 CHAM] Diem (x, y) co nam tren mot cua so GIAO DIEN khong (khac vung ban do)?
// KSdlApp dung de quyet dinh y nghia cua "cham giu tai cho": tren ban do thi la CHUOT PHAI,
// con tren giao dien thi chi he ra xem thong tin - vi chuot phai trong tui do la DUNG vat pham.
extern "C" int JxUi_CoGiaoDienTaiDiem(int x, int y)
{
	KWndWindow* pWnd = Wnd_GetActive(x, y, false);
	return (pWnd != NULL && pWnd != s_WndStation.pGameSpaceWnd) ? 1 : 0;
}

// [BANPHIM 14/09] Cham (x, y) khi dang co cua so giu tieu diem (thuong la o nhap -> ban phim ao dang mo):
//   0 = khong cua so nao giu tieu diem; 1 = cham DUNG cua so do (giu nguyen); 2 = cham NGOAI -> da bo tieu diem
//   (KILL_FOCUS -> KWndEdit goi JxSdl_BanPhimAo(0) -> tat ban phim ao sau 200 ms). KSdlApp goi ngay luc dat ngon, TRUOC cac
//   nhanh can dieu khien / nut ky nang / icon NPC von nuot cham (khong toi Wnd_HandleMsg nen dieu kien pTopWnd != pFocusWnd
//   o tren khong bao gio chay) - chu 14/09: "chat de bi ket ban phim, khong an lai duoc".
extern "C" int JxUi_ChamKhiCoTieuDiem(int x, int y)
{
	KWndWindow* pTD = s_WndStation.pFocusWnd;
	if (!pTD)
		return 0;
	if (pTD->PtInWindow(x, y))
		return 1;
	Wnd_SetFocusWnd(NULL);
	return 2;
}

// [BANPHIM 14/09] IME bi dong ngoai y game (Back / nut an cua IME): SDL da StopTextInput nhung game van giu tieu diem o nhap ->
// cham lai dung o khong phat SET_FOCUS nen ban phim khong mo lai. KSdlApp (JxSdl_BanPhimNhip) goi de bo tieu diem cho dong bo.
extern "C" void JxUi_BoTieuDiem(void)
{
	Wnd_SetFocusWnd(NULL);
}

//	[VATPHAM 12/09 g] Hoi CA CAY cua so con: o vat pham (KWndObjectBox / KWndObjectMatrix) luon la cua so CON cua
//	hop thoai (hanh trang, ruong...), ma Wnd_GetActive chi tra cua so TOP nen phai tu di xuong.
static int JxUi_HoiCoVatPham(KWndWindow* pWnd, int x, int y)
{
	KWndWindow* pCon = pWnd->GetFirstChild();

	while (pCon)
	{
		if (pCon->PtInWindow(x, y) && JxUi_HoiCoVatPham(pCon, x, y))
			return 1;
		pCon = pCon->GetNextWnd();
	}
	//	Gui TOA DO TUYET DOI: WM_LBUTTONDOWN / WM_MOUSEMOVE cua bo giao dien nay deu mang toa do tuyet doi
	//	(KWndObjectMatrix::GetObjectAt tu tru m_nAbsoluteLeft/Top ben trong).
	return pWnd->WndProc(WND_M_JX_CO_VATPHAM, 0, MAKELPARAM(x, y)) ? 1 : 0;
}

//	[VATPHAM 12/09 g] Diem (x, y) co nam tren mot O DANG CO VAT PHAM khong (hanh trang, ruong, o trang bi...)?
//	Dung cho "giu ngon lau tren o vat pham = nhac mon len tay": chi khi co mon that moi doi giu lau
//	tu chuot phai sang bam trai, con lai (nut, danh sach, ban do) giu nguyen nhu cu.
int g_nJxNhatKyGiu = 0;	// [VATPHAM 12/09 g] [Cham] NhatKyGiu=1 -> ghi nhat ky khi hoi o vat pham

extern "C" int JxUi_CoVatPhamTaiDiem(int x, int y)
{
	KWndWindow* pWnd = Wnd_GetActive(x, y, false);
	int nRa;

	if (pWnd == NULL || pWnd == s_WndStation.pGameSpaceWnd)
	{
		if (g_nJxNhatKyGiu)
			g_DebugLog("[GIU] %d,%d: khong co cua so giao dien", x, y);
		return 0;
	}
	nRa = JxUi_HoiCoVatPham(pWnd, x, y);
	if (g_nJxNhatKyGiu)
	{
		int nL = 0, nT = 0;
		pWnd->GetAbsolutePos(&nL, &nT);
		g_DebugLog("[GIU] %d,%d: cua so top goc %d,%d -> co vat pham = %d", x, y, nL, nT, nRa);
	}
	return nRa;
}
#endif

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

//改变鼠标指针显示状态
void Wnd_ShowCursor(int bShow)
{
	s_WndStation.Cursor.Show(bShow);
}

//--------------------------------------------------------------------------
//	功能：设置鼠标指针位置
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
//	功能：获取鼠标指针位置
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
//	功能：获取当前挂接的输入焦点窗口
//--------------------------------------------------------------------------
KWndWindow* Wnd_GetFocusWnd()
{
	return s_WndStation.pFocusWnd;
}

//--------------------------------------------------------------------------
//	功能：挂接输入焦点窗口
//--------------------------------------------------------------------------
void Wnd_SetFocusWnd(KWndWindow* pWnd)
{
	if (s_WndStation.pFocusWnd != pWnd)
	{
		if (s_WndStation.pFocusWnd)
			s_WndStation.pFocusWnd->WndProc(WND_M_KILL_FOCUS, (KUPARAM)pWnd, 0);
		if (pWnd)
			pWnd->WndProc(WND_M_SET_FOCUS, (KUPARAM)s_WndStation.pFocusWnd, 0);
		s_WndStation.pFocusWnd = pWnd;
	}
}

//--------------------------------------------------------------------------
//	功能：设置跟踪鼠标动作窗口
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
//	功能：释放窗口对鼠标动作的跟踪
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
//	//Ch?hi謓 th?UI tr猲 c飊g c竎 ui di kho?h誸
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
//	功能：取消独占窗口
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
//	功能：载入鼠标指针资源
//--------------------------------------------------------------------------
void Wnd_LoadCursor(int nIndex, const char* pImgFile)
{
	s_WndStation.Cursor.SetImage(nIndex, pImgFile);
}

//--------------------------------------------------------------------------
//	功能：切换当前鼠标指针图形
//--------------------------------------------------------------------------
int Wnd_SwitchCursor(int nIndex)
{
	return s_WndStation.Cursor.SwitchImage(nIndex);
}


//设置屏幕范围大小
void Wnd_SetScreenSize(int nWidth, int nHeight)
{
	s_WndStation.ScreenSize.cx = nWidth;
	s_WndStation.ScreenSize.cy = nHeight;
}

//获取屏幕范围大小
void Wnd_GetScreenSize(int& nWidth, int& nHeight)
{
	nWidth = s_WndStation.ScreenSize.cx;
	nHeight = s_WndStation.ScreenSize.cy;
}

//还原鼠标图形
void	Wnd_RestoreCursor()
{
	s_WndStation.Cursor.RestoreCursor();
}

void Wnd_LButtonDown()
{
	s_WndStation.bLBtnDown = TRUE;
}

BOOL Wnd_IsLButtonDown()
{
	return s_WndStation.bLBtnDown;
}

void Wnd_SetPKKey(UINT uKey)
{
	if(uKey != s_WndStation.uPKKeyDown)
	{
		s_WndStation.uPKKeyDown = uKey;
		s_WndStation.bPKKeyDown = FALSE;
	}
}

BOOL Wnd_IsPKKeyDown()
{
	return s_WndStation.bPKKeyDown;
}
