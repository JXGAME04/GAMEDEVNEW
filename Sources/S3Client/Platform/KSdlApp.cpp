//---------------------------------------------------------------------------
// [SDL 08/09] Lop nen SDL3 cho client - xem KSdlApp.h. Chi bien dich khi JX_PLATFORM_SDL.
// Nguyen tac: KHONG doi hanh vi so voi KWin32App::Run/MsgProc:
//  - nhip GameLoop 8 ms nhu cu (luoi chia het 55,56 ms - xem chu thich trong KWin32App::Run);
//  - moi su kien SDL duoc dich thanh dung thong diep WM_* ma UiProcessInput/Wnd_ProcessInput dang dung, roi
//    dua vao KWin32App::MsgProc (giu bookkeeping hover chuot, WM_ACTIVATEAPP, HandleInput);
//  - WM_CLOSE tu xu ly (MsgProc goc goi DestroyWindow - khong duoc pha cua so cua SDL);
//  - WM_COPYDATA (WAuto/script), thong diep khay, TaskbarCreated: qua moc thong diep Windows cua SDL;
//  - WM_SETCURSOR: chan de SDL khong de lai con tro ma UiCursor da SetCursor.
//---------------------------------------------------------------------------
#include "KWin32.h"		// PCH cua S3Client (/Yu"KWin32.h"): phai la dong include DAU TIEN, moi thu truoc no bi bo qua
#ifdef JX_PLATFORM_SDL
#include "../S3Client.h"
#include "KStrBase.h"		// g_StrCpy
#include "KWin32Wnd.h"	// g_SetMainHWnd / g_SetDrawHWnd / g_GetMainHWnd
extern int SCREEN_WIDTH, SCREEN_HEIGHT;	// S3Client.cpp (WND_INIT_* cua Engine khong export)
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>	// SDL_RegisterApp/SDL_UnregisterApp (SDL.h khong include SDL_main.h; SDL_MAIN_HANDLED da define nen khong dinh nghia lai main)

static KSdlApp* s_pSdlApp = NULL;

//---------------------------------------------------------------------------
// SDL_Keycode -> ma phim ao Windows (VK_*) ma KWnd/ShortcutKey dang dung
//---------------------------------------------------------------------------
static WORD SdlKeyToVk(SDL_Keycode key)
{
	if (key >= SDLK_A && key <= SDLK_Z)			return (WORD)('A' + (key - SDLK_A));
	if (key >= SDLK_0 && key <= SDLK_9)			return (WORD)('0' + (key - SDLK_0));
	if (key >= SDLK_F1 && key <= SDLK_F12)		return (WORD)(VK_F1 + (key - SDLK_F1));
	if (key >= SDLK_KP_1 && key <= SDLK_KP_9)	return (WORD)(VK_NUMPAD1 + (key - SDLK_KP_1));
	switch (key)
	{
	case SDLK_KP_0:			return VK_NUMPAD0;
	case SDLK_RETURN:		return VK_RETURN;
	case SDLK_KP_ENTER:		return VK_RETURN;
	case SDLK_ESCAPE:		return VK_ESCAPE;
	case SDLK_BACKSPACE:	return VK_BACK;
	case SDLK_TAB:			return VK_TAB;
	case SDLK_SPACE:		return VK_SPACE;
	case SDLK_LEFT:			return VK_LEFT;
	case SDLK_UP:			return VK_UP;
	case SDLK_RIGHT:		return VK_RIGHT;
	case SDLK_DOWN:			return VK_DOWN;
	case SDLK_HOME:			return VK_HOME;
	case SDLK_END:			return VK_END;
	case SDLK_PAGEUP:		return VK_PRIOR;
	case SDLK_PAGEDOWN:		return VK_NEXT;
	case SDLK_INSERT:		return VK_INSERT;
	case SDLK_DELETE:		return VK_DELETE;
	case SDLK_LSHIFT:		case SDLK_RSHIFT:	return VK_SHIFT;
	case SDLK_LCTRL:		case SDLK_RCTRL:	return VK_CONTROL;
	case SDLK_LALT:			case SDLK_RALT:		return VK_MENU;
	case SDLK_CAPSLOCK:		return VK_CAPITAL;
	case SDLK_PRINTSCREEN:	return VK_SNAPSHOT;
	case SDLK_PAUSE:		return VK_PAUSE;
	case SDLK_NUMLOCKCLEAR:	return VK_NUMLOCK;
	case SDLK_SCROLLLOCK:	return VK_SCROLL;
	case SDLK_KP_PLUS:		return VK_ADD;
	case SDLK_KP_MINUS:		return VK_SUBTRACT;
	case SDLK_KP_MULTIPLY:	return VK_MULTIPLY;
	case SDLK_KP_DIVIDE:	return VK_DIVIDE;
	case SDLK_KP_PERIOD:	return VK_DECIMAL;
	case SDLK_MINUS:		return VK_OEM_MINUS;
	case SDLK_EQUALS:		return VK_OEM_PLUS;
	case SDLK_LEFTBRACKET:	return VK_OEM_4;
	case SDLK_RIGHTBRACKET:	return VK_OEM_6;
	case SDLK_BACKSLASH:	return VK_OEM_5;
	case SDLK_SEMICOLON:	return VK_OEM_1;
	case SDLK_APOSTROPHE:	return VK_OEM_7;
	case SDLK_GRAVE:		return VK_OEM_3;
	case SDLK_COMMA:		return VK_OEM_COMMA;
	case SDLK_PERIOD:		return VK_OEM_PERIOD;
	case SDLK_SLASH:		return VK_OEM_2;
	default:				return 0;
	}
}

// co MK_* cua wParam thong diep chuot
static UINT SdlMouseFlags(Uint32 uBtnState)
{
	UINT f = 0;
	if (uBtnState & SDL_BUTTON_LMASK) f |= MK_LBUTTON;
	if (uBtnState & SDL_BUTTON_RMASK) f |= MK_RBUTTON;
	if (uBtnState & SDL_BUTTON_MMASK) f |= MK_MBUTTON;
	SDL_Keymod m = SDL_GetModState();
	if (m & SDL_KMOD_SHIFT) f |= MK_SHIFT;
	if (m & SDL_KMOD_CTRL)  f |= MK_CONTROL;
	return f;
}

//---------------------------------------------------------------------------
// Moc thong diep Windows cua SDL: goi truoc khi SDL xu ly moi thong diep cua cua so.
// true = de SDL xu ly tiep, false = nuot.
//---------------------------------------------------------------------------
static bool SDLCALL KSdlApp_WinMsgHook(void* pUser, MSG* pMsg)
{
	KSdlApp* pApp = (KSdlApp*)pUser;
	if (!pApp || !pMsg)
		return true;
	switch (pMsg->message)
	{
	case WM_SETCURSOR:
		return false;	// UiCursor da SetCursor(); khong de SDL dat lai con tro cua no
	case WM_COPYDATA:	// WAuto / ExcuteHWNDScript
	case WMAPP_TRAY:
		pApp->RawWinMsg(pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam);
		return false;
	default:
		if (pApp->g_uTaskbarCreated && pMsg->message == pApp->g_uTaskbarCreated)
		{
			pApp->RawWinMsg(pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam);
			return false;
		}
		return true;
	}
}

//---------------------------------------------------------------------------
KSdlApp::KSdlApp()
{
	m_pWindow = NULL;
	m_uMsgQuit = 0;
	m_uHoverSetting = 0;
	m_uHoverStart = 0;
	m_uHoverLastStatus = 0;
	m_nHoverLastPos = 0;
	m_cHoverCounter = 0;
	s_pSdlApp = this;
}

BOOL KSdlApp::Init(HINSTANCE hInstance, char* AppName)
{
	g_StrCpy(m_szClass, AppName);
	strcat_s(m_szClass, " Class");
	g_StrCpy(m_szTitle, AppName);
	strcat_s(m_szTitle, " Title");

	// SDL3: khong con SDL_SetMainReady; SDL_MAIN_HANDLED (define cua cau hinh) la du vi WinMain cua S3Client tu goi Init/Run
	// [SDL 08/09 WAUTO] WAuto ngoai tim game bang EnumWindows + GetClassNameA == "JXWC Class" (WAuto.cpp EnumWindowsProc) roi lay PID -> mo
	// Local\Auto_Name_MMFSV_<pid>. Dang ky lop cua so cua SDL bang dung ten m_szClass TRUOC SDL_Init(VIDEO) (SDL chi tu dang ky khi chua co).
	if (!SDL_RegisterApp(m_szClass, CS_DBLCLKS | CS_BYTEALIGNCLIENT | CS_OWNDC, hInstance))
		g_DebugLog("[SDL] SDL_RegisterApp(%s) loi: %s", m_szClass, SDL_GetError());
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
	{
		g_DebugLog("[SDL] SDL_Init loi: %s", SDL_GetError());
		return FALSE;
	}
	// cua so co dung kich thuoc vung ve nhu KWin32App::InitWindow (WND_INIT_WIDTH x WND_INIT_HEIGHT), khong doi co
	m_pWindow = SDL_CreateWindow(m_szTitle, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	if (!m_pWindow)
	{
		g_DebugLog("[SDL] SDL_CreateWindow loi: %s", SDL_GetError());
		return FALSE;
	}
	HWND hWnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(m_pWindow), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
	if (!hWnd)
	{
		g_DebugLog("[SDL] khong lay duoc HWND tu cua so SDL");
		return FALSE;
	}
	g_SetMainHWnd(hWnd);	// Represent3 (D3D9) va cac chi SetWindowText/MessageBox cu van dung HWND nay
	g_SetDrawHWnd(hWnd);
	SDL_SetWindowsMessageHook(KSdlApp_WinMsgHook, this);
	SDL_StartTextInput(m_pWindow);	// WM_CHAR tu SDL_EVENT_TEXT_INPUT (o mobile se bat/tat theo o nhap)
	SDL_ShowCursor();
	g_DebugLog("[SDL] cua so %dx%d, SDL %d.%d.%d, HWND %p", SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_VERSIONNUM_MAJOR(SDL_GetVersion()), SDL_VERSIONNUM_MINOR(SDL_GetVersion()), SDL_VERSIONNUM_MICRO(SDL_GetVersion()), hWnd);
	return GameInit();
}

void KSdlApp::ShowMouse(BOOL bShow)
{
	KWin32App::ShowMouse(bShow);
	if (bShow)
		SDL_ShowCursor();
	else
		SDL_HideCursor();
}

//---------------------------------------------------------------------------
// Hover chuot: ban sao dung logic KWin32App::MsgProc (bookkeeping) + GenerateMsgHoverMsg (private ben Engine, khong goi duoc
// ma khong doi Engine.dll). 0 = chua co, 1 = vua co su kien chuot, 2 = da gui WM_MOUSEHOVER (gui lai moi 7 nhip), >= 3 = moc bat dau dung yen.
//---------------------------------------------------------------------------
void KSdlApp::SetMouseHoverTime(unsigned int uHoverTime)
{
	m_uHoverSetting = uHoverTime;
	KWin32App::SetMouseHoverTime(uHoverTime);
}

void KSdlApp::GhiChuot(WPARAM wParam, LPARAM lParam)
{
	m_uHoverLastStatus = (unsigned int)wParam;
	m_nHoverLastPos = (int)lParam;
	m_uHoverStart = 1;
}

void KSdlApp::SinhHover()
{
	if (m_uHoverStart == 2)
	{
		if ((++m_cHoverCounter) == 7)
		{
			m_cHoverCounter = 0;
			HandleInput(WM_MOUSEHOVER, m_uHoverLastStatus, m_nHoverLastPos);
		}
	}
	else if (m_uHoverStart >= 3)
	{
		unsigned int uNow = (unsigned int)GetTickCount();
		if ((uNow - m_uHoverStart) >= m_uHoverSetting)
		{
			HandleInput(WM_MOUSEHOVER, m_uHoverLastStatus, m_nHoverLastPos);
			m_uHoverStart = 2;
		}
	}
	else if (m_uHoverStart == 1)
	{
		m_uHoverStart = 0;
	}
	else if (m_uHoverSetting)
	{
		m_uHoverStart = (unsigned int)GetTickCount();
		if (m_uHoverStart < 3)
			m_uHoverStart = 3;
	}
}

//---------------------------------------------------------------------------
// Vong lap: giong KWin32App::Run - luoi 8 ms, cho su kien toi da het khe, roi GameLoop
//---------------------------------------------------------------------------
void KSdlApp::Run()
{
	const Uint64 uInterval = 8;
	Uint64 uNext = SDL_GetTicks() + uInterval;
	bool bQuit = false;
	while (!bQuit)
	{
		SDL_Event ev;
		while (SDL_PollEvent(&ev))
		{
			if (!TranslateEvent(ev))
			{
				bQuit = true;
				break;
			}
		}
		if (bQuit)
			break;
		if (m_bActive || m_bMultiGame)
		{
			Uint64 uNow = SDL_GetTicks();
			bool bTick = false;
			if (uNext > uNow)
			{
				Uint64 uWait = uNext - uNow;
				if (uWait > uInterval)
					uWait = uInterval;
				if (!SDL_WaitEventTimeout(NULL, (Sint32)uWait))	// het gio ma khong co su kien
				{
					uNext = uNow + uWait + uInterval;
					bTick = true;
				}
			}
			else
			{
				uNext = uNow + uInterval;
				bTick = true;
			}
			if (bTick)
			{
				SinhHover();
				if (!GameLoop())
				{
					// KWin32App: PostMessage(WM_CLOSE) -> MsgProc -> HandleInput(WM_CLOSE) (hoi thoat neu chua thoat)
					if (HandleInput(WM_CLOSE, 0, 0) == 0)
						bQuit = true;
				}
			}
		}
		else
		{
			SDL_WaitEventTimeout(NULL, 100);
		}
	}
	GameExit();
	if (m_pWindow)
	{
		SDL_StopTextInput(m_pWindow);
		SDL_DestroyWindow(m_pWindow);
		m_pWindow = NULL;
	}
	SDL_Quit();
	SDL_UnregisterApp();	// doi voi SDL_RegisterApp o Init
}

//---------------------------------------------------------------------------
// Dich su kien SDL -> WM_* -> MsgProc cu. Tra ve false khi thoat.
//---------------------------------------------------------------------------
// [GPU 08/09 khung ao] cua so (toan man hinh / dien thoai) co the khac khung logic SCREEN_WIDTH x SCREEN_HEIGHT: bo ve (D3D9onGPU) phong
// khung logic len cua so voi letterbox (scale = min, can giua) -> toa do chuot phai doi nguoc lai cung cong thuc.
static void SdlToLogical(SDL_Window* pWin, float& x, float& y)
{
	int w = 0, h = 0; SDL_GetWindowSize(pWin, &w, &h);
	if (w <= 0 || h <= 0 || SCREEN_WIDTH <= 0 || SCREEN_HEIGHT <= 0 || (w == SCREEN_WIDTH && h == SCREEN_HEIGHT)) return;
	float sx = (float)w / (float)SCREEN_WIDTH, sy = (float)h / (float)SCREEN_HEIGHT;
	float sc = (sx < sy) ? sx : sy;
	float ox = ((float)w - (float)SCREEN_WIDTH * sc) * 0.5f, oy = ((float)h - (float)SCREEN_HEIGHT * sc) * 0.5f;
	x = (x - ox) / sc; y = (y - oy) / sc;
	if (x < 0.0f) x = 0.0f; if (y < 0.0f) y = 0.0f;
	if (x > (float)(SCREEN_WIDTH - 1)) x = (float)(SCREEN_WIDTH - 1); if (y > (float)(SCREEN_HEIGHT - 1)) y = (float)(SCREEN_HEIGHT - 1);
}

bool KSdlApp::TranslateEvent(const SDL_Event& ev)
{
	HWND hWnd = g_GetMainHWnd();
	switch (ev.type)
	{
	case SDL_EVENT_QUIT:
	case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
		m_uMsgQuit++;
		return HandleInput(WM_CLOSE, 0, 0) != 0;	// 0 = dong y thoat (KMyApp::HandleInput hoi truoc)

	case SDL_EVENT_WINDOW_FOCUS_GAINED:
		MsgProc(hWnd, WM_ACTIVATEAPP, TRUE, 0);
		break;
	case SDL_EVENT_WINDOW_FOCUS_LOST:
		MsgProc(hWnd, WM_ACTIVATEAPP, FALSE, 0);
		break;

	case SDL_EVENT_MOUSE_MOTION:
	{
		float mx = ev.motion.x, my = ev.motion.y; SdlToLogical(m_pWindow, mx, my);
		WPARAM w = SdlMouseFlags(ev.motion.state); LPARAM l = MAKELPARAM((int)mx, (int)my);
		GhiChuot(w, l);
		MsgProc(hWnd, WM_MOUSEMOVE, w, l);
		break;
	}
		break;

	case SDL_EVENT_MOUSE_BUTTON_DOWN:
	case SDL_EVENT_MOUSE_BUTTON_UP:
	{
		bool bDown = (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN);
		bool bDbl = bDown && (ev.button.clicks == 2);	// Windows: nhan lan hai thanh WM_xBUTTONDBLCLK (lop cua so co CS_DBLCLKS)
		UINT uMsg = 0;
		switch (ev.button.button)
		{
		case SDL_BUTTON_LEFT:	uMsg = bDown ? (bDbl ? WM_LBUTTONDBLCLK : WM_LBUTTONDOWN) : WM_LBUTTONUP; break;
		case SDL_BUTTON_RIGHT:	uMsg = bDown ? (bDbl ? WM_RBUTTONDBLCLK : WM_RBUTTONDOWN) : WM_RBUTTONUP; break;
		case SDL_BUTTON_MIDDLE:	uMsg = bDown ? (bDbl ? WM_MBUTTONDBLCLK : WM_MBUTTONDOWN) : WM_MBUTTONUP; break;
		default: return true;
		}
		{
			float bx = ev.button.x, by = ev.button.y; SdlToLogical(m_pWindow, bx, by);
			WPARAM w = SdlMouseFlags(SDL_GetMouseState(NULL, NULL)); LPARAM l = MAKELPARAM((int)bx, (int)by);
			GhiChuot(w, l);
			MsgProc(hWnd, uMsg, w, l);
		}
		break;
	}

	case SDL_EVENT_MOUSE_WHEEL:
	{
		int nDelta = (int)(ev.wheel.y * WHEEL_DELTA);
		if (ev.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
			nDelta = -nDelta;
		WPARAM w = MAKEWPARAM(SdlMouseFlags(SDL_GetMouseState(NULL, NULL)), (WORD)(short)nDelta);
		float wx = ev.wheel.mouse_x, wy = ev.wheel.mouse_y; SdlToLogical(m_pWindow, wx, wy);
		GhiChuot(w, MAKELPARAM((int)wx, (int)wy));
		MsgProc(hWnd, WM_MOUSEWHEEL, w, MAKELPARAM((int)wx, (int)wy));
		break;
	}

	case SDL_EVENT_KEY_DOWN:
	case SDL_EVENT_KEY_UP:
	{
		WORD wVk = SdlKeyToVk(ev.key.key);
		if (!wVk)
			break;
		bool bDown = (ev.type == SDL_EVENT_KEY_DOWN);
		bool bAlt = (ev.key.mod & SDL_KMOD_ALT) != 0;
		UINT uMsg = bDown ? (bAlt ? WM_SYSKEYDOWN : WM_KEYDOWN) : (bAlt ? WM_SYSKEYUP : WM_KEYUP);
		LPARAM l = 1 | ((LPARAM)(ev.key.scancode & 0xFF) << 16);
		if (bDown && ev.key.repeat)	l |= ((LPARAM)1 << 30);
		if (!bDown)					l |= ((LPARAM)1 << 30) | ((LPARAM)1 << 31);
		if (bAlt)					l |= ((LPARAM)1 << 29);
		MsgProc(hWnd, uMsg, wVk, l);
		if (bDown)
		{
			// TranslateMessage cua Windows sinh WM_CHAR cho Enter/Backspace/Tab/Esc va Ctrl+chu; SDL_EVENT_TEXT_INPUT khong co
			int nCh = 0;
			switch (wVk)
			{
			case VK_RETURN: nCh = '\r'; break;
			case VK_BACK:   nCh = '\b'; break;
			case VK_TAB:    nCh = '\t'; break;
			case VK_ESCAPE: nCh = 27;   break;
			}
			if (!nCh && !bAlt && (ev.key.mod & SDL_KMOD_CTRL) && wVk >= 'A' && wVk <= 'Z')
				nCh = wVk - 'A' + 1;
			if (nCh)
				MsgProc(hWnd, WM_CHAR, (WPARAM)nCh, l);
		}
		break;
	}

	case SDL_EVENT_TEXT_INPUT:
	{
		// UTF-8 -> bang ma he thong (nhu WM_CHAR cua cua so ANSI) -> WM_CHAR tung byte. Tieng Viet: game tu ghep (Telex) tu ASCII.
		wchar_t wBuf[64];
		int n = MultiByteToWideChar(CP_UTF8, 0, ev.text.text, -1, wBuf, 64);
		if (n <= 0)
			break;
		char aBuf[128];
		int m = WideCharToMultiByte(CP_ACP, 0, wBuf, -1, aBuf, sizeof(aBuf), NULL, NULL);
		if (m <= 0)
			break;
		for (int i = 0; aBuf[i]; i++)
			MsgProc(hWnd, WM_CHAR, (WPARAM)(BYTE)aBuf[i], 1);
		break;
	}

	default:
		break;
	}
	return true;
}

#endif // JX_PLATFORM_SDL
