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
ENGINE_API void SetEngineResolution(int width, int height);	// KDDraw.h (Engine)
extern int g_nDoPhanGiaiTheoManHinh;	// S3Client.cpp
extern "C" void JxSdl_BanPhimNhip(void);	// [DANGNHAP 12/09] dinh nghia phia duoi, vong lap chinh goi
#include <SDL3/SDL.h>
#ifndef JX_POSIX
#include <SDL3/SDL_main.h>	// SDL_RegisterApp/SDL_UnregisterApp (SDL.h khong include SDL_main.h; SDL_MAIN_HANDLED da define nen khong dinh nghia lai main)
#endif

static KSdlApp* s_pSdlApp = NULL;

//---------------------------------------------------------------------------
// SDL_Keycode -> ma phim ao Windows (VK_*) ma KWnd/ShortcutKey dang dung
//---------------------------------------------------------------------------
#ifdef JX_ANDROID
#include "JxCanDieuKhien.h"
#include "../Ui/Elem/UiToaDo.h"	// [ANDROID 09/09 SUAKEO] UiToaDo_DangSua	// [ANDROID 09/09 CAN] can dieu khien ao
#endif

#ifdef JX_ANDROID
//---------------------------------------------------------------------------
// [ANDROID 09/09 PHIM] BANG PHIM CHO GetKeyState()
//
// LOI THAT tim ra hom nay: KPosixWin32.cpp co san moc g_pfnJxGetKeyState nhung KHONG AI NOI vao,
// nen GetKeyState()/GetAsyncKeyState() tren Android LUON tra 0. Hau qua: moi cho trong game hoi
// "dang giu Shift/Ctrl/Alt khong" deu tra lai KHONG - tuc la
//     Ctrl+chuot phai (menu nguoi choi: giao dich, to doi, ket ban)
//     Shift+chuot trai (danh ep), Alt+chuot (ban dong hanh, menu bieu cam)
// deu khong the lam duoc, KE CA khi cam ban phim roi vao may.
//
// Nay tra loi bang BANG PHIM THAT cua SDL, cong them MAT NA PHIM DINH de lop cham (hoac mot nut ao
// sau nay) giu ho phim bo tro ma khong can ban phim.
//---------------------------------------------------------------------------
static unsigned int s_uPhimDinh = 0;	// bit 0 = Shift, 1 = Ctrl, 2 = Alt

extern "C" void JxSdl_DatPhimDinh(unsigned int uMatNa) { s_uPhimDinh = uMatNa; }
extern "C" unsigned int JxSdl_LayPhimDinh(void) { return s_uPhimDinh; }

static SHORT JxSdl_TrangThaiPhim(int vk)
{
	int nSo = 0;
	const bool* pPhim = SDL_GetKeyboardState(&nSo);
	SDL_Scancode sc = SDL_SCANCODE_UNKNOWN;
	bool bNhan = false;
	switch (vk)
	{
	case VK_SHIFT:
		bNhan = (s_uPhimDinh & 1) != 0;
		if (pPhim) bNhan = bNhan || pPhim[SDL_SCANCODE_LSHIFT] || pPhim[SDL_SCANCODE_RSHIFT];
		return bNhan ? (SHORT)0x8000 : 0;
	case VK_CONTROL:
		bNhan = (s_uPhimDinh & 2) != 0;
		if (pPhim) bNhan = bNhan || pPhim[SDL_SCANCODE_LCTRL] || pPhim[SDL_SCANCODE_RCTRL];
		return bNhan ? (SHORT)0x8000 : 0;
	case VK_MENU:
		bNhan = (s_uPhimDinh & 4) != 0;
		if (pPhim) bNhan = bNhan || pPhim[SDL_SCANCODE_LALT] || pPhim[SDL_SCANCODE_RALT];
		return bNhan ? (SHORT)0x8000 : 0;
	case VK_LSHIFT:   sc = SDL_SCANCODE_LSHIFT;  break;
	case VK_RSHIFT:   sc = SDL_SCANCODE_RSHIFT;  break;
	case VK_LCONTROL: sc = SDL_SCANCODE_LCTRL;   break;
	case VK_RCONTROL: sc = SDL_SCANCODE_RCTRL;   break;
	case VK_LMENU:    sc = SDL_SCANCODE_LALT;    break;
	case VK_RMENU:    sc = SDL_SCANCODE_RALT;    break;
	case VK_RETURN:   sc = SDL_SCANCODE_RETURN;  break;
	case VK_ESCAPE:   sc = SDL_SCANCODE_ESCAPE;  break;
	case VK_SPACE:    sc = SDL_SCANCODE_SPACE;   break;
	case VK_TAB:      sc = SDL_SCANCODE_TAB;     break;
	case VK_BACK:     sc = SDL_SCANCODE_BACKSPACE; break;
	case VK_LEFT:     sc = SDL_SCANCODE_LEFT;    break;
	case VK_RIGHT:    sc = SDL_SCANCODE_RIGHT;   break;
	case VK_UP:       sc = SDL_SCANCODE_UP;      break;
	case VK_DOWN:     sc = SDL_SCANCODE_DOWN;    break;
	default:
		if (vk >= 'A' && vk <= 'Z')
			sc = (SDL_Scancode)(SDL_SCANCODE_A + (vk - 'A'));
		else if (vk == '0')
			sc = SDL_SCANCODE_0;
		else if (vk > '0' && vk <= '9')
			sc = (SDL_Scancode)(SDL_SCANCODE_1 + (vk - '1'));
		else if (vk >= VK_F1 && vk <= VK_F12)
			sc = (SDL_Scancode)(SDL_SCANCODE_F1 + (vk - VK_F1));
		break;
	}
	if (sc == SDL_SCANCODE_UNKNOWN || !pPhim || (int)sc >= nSo)
		return 0;
	return pPhim[sc] ? (SHORT)0x8000 : 0;
}
#endif

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
#ifdef JX_ANDROID
	// [ANDROID 09/09 CHAM] nut Back cua may = ESC (mo bang he thong / dong cua so dang mo).
	// Manifest da dat SDL_ANDROID_TRAP_BACK_BUTTON=1 nen SDL dua nut nay vao day thay vi thoat app.
	case SDLK_AC_BACK:		return VK_ESCAPE;
#endif
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
#ifdef JX_ANDROID
	m_nCham = CHAM_KHONG;
	m_nChamX0 = m_nChamY0 = m_nChamX = m_nChamY = 0;
	m_uChamDat = 0;
	m_uChamNhaTruoc = 0;
	m_nChamNhaX = m_nChamNhaY = 0;
	m_nNgonDangDat = 0;
	m_nNgonToiDa = 0;
	m_nNgonKyNang = -1;	// [ANDROID 09/09 HAINGON]
	m_nNgonCan = -1;
	m_nCuonDon = 0;
#endif
	s_pSdlApp = this;
}

#ifdef JX_ANDROID
// [ANDROID 09/09 DPG] DO PHAN GIAI CHUAN CHO DIEN THOAI.
//
// Giao dien JX1 la giao dien "diem anh co dinh": chu 12 px, nut ~100 px, thanh ky nang ~500 px.
// Nen co hai cach SAI va mot cach DUNG:
//   SAI 1 - ve co dinh 1024x768 roi keo cang len man hinh: chu to nhung MO va MEO (dien thoai 20:9,
//           khong phai 4:3). Day la trang thai truoc pha nay.
//   SAI 2 - ve dung 1:1 theo diem anh that (2400x1080): sac tuyet doi nhung chu chi cao ~1,5 mm,
//           khong ai doc noi tren man 6,5 inch.
//   DUNG  - ve o KHUNG NHO HON theo mot HE SO GIAO DIEN, GIU DUNG TI LE khung hinh cua may
//           (nen khong co vien den, khong meo), roi de Represent3 phong len man hinh.
//
// He so duoc chon sao cho chieu cao khung ve roi vao quanh CHIEU CAO MUC TIEU (mac dinh 640 px: giua
// 604 px da chay tot tren LDPlayer va 768 px goc cua game). Vi du:
//     1040x604  (LDPlayer)      -> he so 1,00 -> khung ve 1040x604  (ve 1:1, net tuyet doi)
//     2400x1080 (dien thoai)    -> he so 1,75 -> khung ve 1370x616  (chu to gap 1,75 lan so voi ve 1:1)
//     1920x1080                 -> he so 1,75 -> khung ve 1096x616
//     2048x1536 (may tinh bang) -> he so 2,50 -> khung ve  818x614
//
// Represent3 (CDevGpu::Letterbox) tu phong khung ve len swapchain va KSdlApp::SdlToLogical doi toa do cham
// nguoc lai -> khong cho nao khac trong game phai biet den chuyen nay.
//
// Goi TRE, ngay truoc khi tao thiet bi ve (KMyApp::GameInit): luc KSdlApp::Init vua tao cua so thi Android
// chua dan trang xong.
//
// config.ini:
//   [Resolution] TheoManHinh=1       ; 0 = tat han, dung Width/Height nhu ban PC
//                ChieuCaoMucTieu=640 ; nho hon = giao dien TO hon (mo hon); lon hon = net hon (nho hon)
//                HeSoGiaoDien=0      ; 0 = tu chon theo muc tieu; 100/125/150/175/200/250/300 = ep (he so x100)
extern "C" void JxSdl_ChotDoPhanGiaiTheoManHinh(void)
{
	SDL_Window* pWin = (SDL_Window*)JxPosix_MainWindow();
	if (!pWin)
		return;
	char szCfg[MAX_PATH] = { 0 };
	GetCurrentDirectory(MAX_PATH, szCfg);
	strcat(szCfg, "\\Config.ini");
	if (!GetPrivateProfileInt("Resolution", "TheoManHinh", 1, szCfg))
		return;
	// Doi co cua so DUNG YEN moi chot (Android tra co that sau khi dan trang xong: tai tho, thanh dieu huong...).
	// KHONG doi che do toan man hinh o day: tren Android doi che do = Activity bi tao lai -> game khoi dong vong lap.
	int nW = 0, nH = 0, nWTruoc = -1, nHTruoc = -1, nYen = 0;
	for (int nLan = 0; nLan < 150; nLan++)
	{
		SDL_PumpEvents();
		SDL_Delay(10);
		SDL_GetWindowSizeInPixels(pWin, &nW, &nH);
		if (nW == nWTruoc && nH == nHTruoc) { if (++nYen >= 25 && nLan >= 40) break; }
		else { nYen = 0; nWTruoc = nW; nHTruoc = nH; }
	}
	if (nW < 640 || nH < 360)
	{
		g_DebugLog("[DPG] cua so %dx%d qua nho, giu %dx%d cua config.ini", nW, nH, SCREEN_WIDTH, SCREEN_HEIGHT);
		return;
	}
	int nMucTieu = GetPrivateProfileInt("Resolution", "ChieuCaoMucTieu", 640, szCfg);
	if (nMucTieu < 400) nMucTieu = 400;
	if (nMucTieu > 1200) nMucTieu = 1200;
	int nHeSo = GetPrivateProfileInt("Resolution", "HeSoGiaoDien", 0, szCfg);	// x100
	if (nHeSo <= 0)
	{
		// Chi lay cac nac "chan" cho phep phong it rang cua nhat: 1,00 1,25 1,50 1,75 2,00 2,50 3,00.
		// Hoa thi giu nac NHO hon (khung ve to hon = net hon) vi vong lap chi doi khi lech NHO HON HAN.
		static const int aNac[] = { 100, 125, 150, 175, 200, 250, 300 };
		int nChon = 100, nLechTotNhat = -1;
		for (int i = 0; i < (int)(sizeof(aNac) / sizeof(aNac[0])); i++)
		{
			int nCao = nH * 100 / aNac[i];
			int nLech = (nCao > nMucTieu) ? (nCao - nMucTieu) : (nMucTieu - nCao);
			if (nLechTotNhat < 0 || nLech < nLechTotNhat) { nLechTotNhat = nLech; nChon = aNac[i]; }
		}
		nHeSo = nChon;
	}
	if (nHeSo < 100) nHeSo = 100;	// khong bao gio ve LON hon man hinh: ton bo nho ma khong net them
	if (nHeSo > 400) nHeSo = 400;
	// Giao dien JX1 can it nhat 800x480 moi bay du (thanh ky nang, tui do, cua so chat) -> ha he so cho du cho.
	while (nHeSo > 100 && (nW * 100 / nHeSo < 800 || nH * 100 / nHeSo < 480))
		nHeSo -= 25;
	int nVeW = (nW * 100 / nHeSo) & ~1;
	int nVeH = (nH * 100 / nHeSo) & ~1;
	{
		SDL_Rect rcCa = { 0, 0, 0, 0 };
		SDL_GetDisplayBounds(SDL_GetPrimaryDisplay(), &rcCa);
		g_DebugLog("[DPG] man hinh %dx%d | cua so %dx%d px | cao muc tieu %d -> he so %d,%02d -> khung ve %dx%d",
			rcCa.w, rcCa.h, nW, nH, nMucTieu, nHeSo / 100, nHeSo % 100, nVeW, nVeH);
	}
	SCREEN_WIDTH = nVeW; SCREEN_HEIGHT = nVeH;
	SetEngineResolution(nVeW, nVeH);
	g_nDoPhanGiaiTheoManHinh = 1;	// S3Client.cpp: chan LoadResolutionFromConfig doc lai config.ini
}
#endif
BOOL KSdlApp::Init(HINSTANCE hInstance, char* AppName)
{
	g_StrCpy(m_szClass, AppName);
	strcat_s(m_szClass, " Class");
	g_StrCpy(m_szTitle, AppName);
	strcat_s(m_szTitle, " Title");

	// SDL3: khong con SDL_SetMainReady; SDL_MAIN_HANDLED (define cua cau hinh) la du vi WinMain cua S3Client tu goi Init/Run
	// [SDL 08/09 WAUTO] WAuto ngoai tim game bang EnumWindows + GetClassNameA == "JXWC Class" (WAuto.cpp EnumWindowsProc) roi lay PID -> mo
	// Local\Auto_Name_MMFSV_<pid>. Dang ky lop cua so cua SDL bang dung ten m_szClass TRUOC SDL_Init(VIDEO) (SDL chi tu dang ky khi chua co).
#ifndef JX_POSIX
	if (!SDL_RegisterApp(m_szClass, CS_DBLCLKS | CS_BYTEALIGNCLIENT | CS_OWNDC, hInstance))
		g_DebugLog("[SDL] SDL_RegisterApp(%s) loi: %s", m_szClass, SDL_GetError());
#endif
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
#ifdef JX_ANDROID
	SDL_SetWindowFullscreen(m_pWindow, true);	// an thanh he thong; co that duoc chot trong JxSdl_ChotDoPhanGiaiTheoManHinh()
#endif
#ifdef JX_POSIX
	HWND hWnd = (HWND)m_pWindow;	// [ANDROID 08/09] tren POSIX "HWND" = SDL_Window* (KPosixWin32: GetClientRect/SetWindowText... hieu no)
	JxPosix_SetMainWindow(m_pWindow);
#ifdef JX_ANDROID
	g_pfnJxGetKeyState = JxSdl_TrangThaiPhim;	// [ANDROID 09/09 PHIM] khong noi thi GetKeyState luon tra 0
#endif
#else
	HWND hWnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(m_pWindow), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
#endif
	if (!hWnd)
	{
		g_DebugLog("[SDL] khong lay duoc HWND tu cua so SDL");
		return FALSE;
	}
	g_SetMainHWnd(hWnd);	// Represent3 (D3D9) va cac chi SetWindowText/MessageBox cu van dung HWND nay
	g_SetDrawHWnd(hWnd);
#ifndef JX_POSIX
	SDL_SetWindowsMessageHook(KSdlApp_WinMsgHook, this);
#endif
#ifdef JX_ANDROID
	// [ANDROID 09/09 CHAM] KHONG bat go chu san: tren dien thoai SDL_StartTextInput = BAT BAN PHIM AO ngay
	// va no che nua man hinh cho toi luc thoat. Chi bat khi mot o nhap co tieu diem - KWndEdit goi
	// JxSdl_BanPhimAo() o WND_M_SET_FOCUS / WND_M_KILL_FOCUS.
#else
	SDL_StartTextInput(m_pWindow);	// WM_CHAR tu SDL_EVENT_TEXT_INPUT
#endif
	SDL_ShowCursor();
	g_DebugLog("[SDL] cua so %dx%d, SDL %d.%d.%d, HWND %p", SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_VERSIONNUM_MAJOR(SDL_GetVersion()), SDL_VERSIONNUM_MINOR(SDL_GetVersion()), SDL_VERSIONNUM_MICRO(SDL_GetVersion()), hWnd);
	BOOL bInit = GameInit();
	if (!bInit)
		g_DebugLog("[SDL] KMyApp::GameInit that bai -> khong vao Run (xem cac dong [REP3]/[script] truoc do)");	// [ANDROID 08/09]
	return bInit;
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
// Vong lap: giong KWin32App::Run - luoi g_GetLoopInterval() ms (8; 1 khi ve > 60 fps), cho su kien toi da het khe, roi GameLoop
//---------------------------------------------------------------------------
void KSdlApp::Run()
{
	Uint64 uInterval = g_GetLoopInterval();	// [NHIP->SDL 08/09] 8 ms mac dinh; 1 ms khi PaintFps > 60 / PaintVsync (S3Client GameInit)
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
#ifdef JX_ANDROID
		NhipCham();		// [ANDROID 09/09 CHAM] giu ngon du lau ma khong xe dich -> chuot phai
		JxSdl_BanPhimNhip();	// [DANGNHAP 12/09] mo lai ban phim sau khi IME tu dong
		JxCan_Nhip();	// [ANDROID 09/09 CAN] dang cam can thi day nhan vat di theo huong
		JxKyNang_Nhip();	// [ANDROID 09/09 KYNANG I] dang de nut ky nang thi cu danh tiep
#endif
		if (m_bActive || m_bMultiGame)
		{
			uInterval = g_GetLoopInterval();	// [NHIP->SDL 08/09] doc lai moi vong nhu KWin32App::Run
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
#ifndef JX_POSIX
	SDL_UnregisterApp();	// doi voi SDL_RegisterApp o Init
#endif
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

#ifdef JX_ANDROID
//---------------------------------------------------------------------------
// [ANDROID 09/09 CHAM] BAN PHIM AO
// Tren dien thoai SDL_StartTextInput = day ban phim ao len ngay. Nen chi goi khi mot o nhap co tieu diem.
// KWndEdit::WndProc goi ham nay o WND_M_SET_FOCUS (bat) va WND_M_KILL_FOCUS (tat).
//---------------------------------------------------------------------------
// [DANGNHAP 12/09] ban phim theo KIEU o nhap: khong tu viet hoa chu dau (tai khoan phai chu thuong), khong tu sua chu,
// o mat khau = kieu mat khau an. IME Android hay tu dong sau phim Enter du o nhap ke tiep da nhan tieu diem ->
// hen mo lai sau 0,3 s (JxSdl_BanPhimNhip trong vong lap chinh).
static Uint64 s_uBanPhimLai = 0;
static int s_nBanPhimMatKhau = 0;
static void BanPhimMo(SDL_Window* pWin, int nMatKhau)
{
	SDL_PropertiesID p = SDL_CreateProperties();
	SDL_SetNumberProperty(p, SDL_PROP_TEXTINPUT_TYPE_NUMBER, nMatKhau ? SDL_TEXTINPUT_TYPE_TEXT_PASSWORD_HIDDEN : SDL_TEXTINPUT_TYPE_TEXT);
	SDL_SetNumberProperty(p, SDL_PROP_TEXTINPUT_CAPITALIZATION_NUMBER, SDL_CAPITALIZE_NONE);
	SDL_SetBooleanProperty(p, SDL_PROP_TEXTINPUT_AUTOCORRECT_BOOLEAN, false);
	SDL_StartTextInputWithProperties(pWin, p);
	SDL_DestroyProperties(p);
}
extern "C" void JxSdl_BanPhimAo(int bBat, int nMatKhau)
{
	SDL_Window* pWin = (SDL_Window*)JxPosix_MainWindow();
	if (!pWin)
		return;
	if (bBat)
	{
		BanPhimMo(pWin, nMatKhau);
		s_nBanPhimMatKhau = nMatKhau;
		s_uBanPhimLai = SDL_GetTicks() + 300;
	}
	else
	{
		SDL_StopTextInput(pWin);
		s_uBanPhimLai = 0;
	}
}
// goi moi vong lap: den hen ma o nhap van giu tieu diem -> dong roi mo lai ban phim (IME da tu dong thi hien lai)
extern "C" void JxSdl_BanPhimNhip(void)
{
	if (!s_uBanPhimLai || SDL_GetTicks() < s_uBanPhimLai)
		return;
	s_uBanPhimLai = 0;
	SDL_Window* pWin = (SDL_Window*)JxPosix_MainWindow();
	if (!pWin)
		return;
	SDL_StopTextInput(pWin);
	BanPhimMo(pWin, s_nBanPhimMatKhau);
}

//---------------------------------------------------------------------------
// [ANDROID 09/09 CHAM] BO NHAN CU CHI NGON TAY -> CHUOT
//
// SDL tu gia lap chuot tu ngon tay, nhung chi ra duoc CHUOT TRAI. JX1 thi song bang chuot phai
// (danh ep, menu chuot phai) va bang re chuot (thong tin vat pham, ten NPC) - ngon tay khong co
// hai thu do. Nen chan cac su kien chuot DO NGON TAY sinh ra (which == SDL_TOUCH_MOUSEID) va dich lai:
//
//   cham nhanh roi nha      -> chuot trai, bam tai CHO DAT NGON (khong phai cho nha: ngon tay hay
//                              truot vai diem anh luc nhac len)
//   cham hai lan lien       -> bam dup (dung vat pham)
//   cham roi keo di         -> giu chuot trai roi re: di chuyen lien tuc, keo tha vat pham
//   giu tai cho tren BAN DO -> CHUOT PHAI (danh ep quai da co nguoi danh, mo menu nguoi choi)
//   giu tai cho tren GIAO DIEN -> KHONG bam chuot phai (chuot phai trong tui do la DUNG vat pham -
//                              bam nham la mat do); "chuot" van nam do nen game kip hien thong tin vat pham,
//                              va khi nha ngon van bam chuot TRAI nhu mot cai cham thuong - vi tren dien
//                              thoai nguoi ta hay an nut lau hon 400 ms, khong the vi the ma nut chet.
//
// Vi sao cham nhanh lai doi den luc NHA moi bam: neu bam ngay luc dat ngon thi khong the phan biet
// duoc voi "giu de bam chuot phai" - se bam trai roi lai bam phai, tren NPC la mo thoai roi danh.
// Doi den luc nha (thuong duoi 150 ms) la cach moi giao dien cam ung deu lam.
//---------------------------------------------------------------------------
extern "C" int JxUi_CoGiaoDienTaiDiem(int x, int y);	// Wnds.cpp
extern "C" void JxSdl_DatPhimDinh(unsigned int uMatNa);	// dinh nghia o khoi PHIM ben tren


static const unsigned int CHAM_GIU_MS = 400;	// giu lau bao nhieu thi thanh chuot phai
static const int          CHAM_NGUONG = 12;		// xe dich qua bao nhieu diem anh thi coi la KEO
static const unsigned int CHAM_HAI_MS = 400;	// hai lan cham cach nhau duoi bao nhieu = bam dup
static const int          CHAM_HAI_XA = 24;		// ... va cach nhau khong qua bao nhieu diem anh

bool KSdlApp::ChamSuKien(const SDL_Event& ev)
{
	HWND hWnd = g_GetMainHWnd();
	// [ANDROID 09/09 NGON] SDL chi gia lap chuot cho ngon THU NHAT, nen phai nghe su kien ngon tay that
	// moi biet dang co may ngon. Dem de sau nay bo qua cham nhieu ngon lo tay (cham hai ngon KHONG dung
	// lam loi tat nao: theo y chu, menu doi tuong di duong "cham doi tuong -> thanh thong tin -> menu").
	// [ANDROID 09/09 HAINGON] duong RIENG cho ngon THU HAI tro di: SDL chi gia lap
	// chuot cho ngon thu nhat, nen muon vua giu can vua bam nut ky nang thi phai doc
	// thang su kien ngon tay. Ngon thu nhat van di duong cu, khong doi gi.
	if (ev.type == SDL_EVENT_FINGER_DOWN || ev.type == SDL_EVENT_FINGER_MOTION
		|| ev.type == SDL_EVENT_FINGER_UP || ev.type == SDL_EVENT_FINGER_CANCELED)
	{
		long long nNgon = (long long)ev.tfinger.fingerID;
		int nW = 0, nH = 0;
		float fx, fy;

		SDL_GetWindowSize(m_pWindow, &nW, &nH);
		fx = ev.tfinger.x * (float)nW;
		fy = ev.tfinger.y * (float)nH;
		SdlToLogical(m_pWindow, fx, fy);

		if (ev.type == SDL_EVENT_FINGER_DOWN)
		{
			bool bNgonDau = (m_nNgonDangDat == 0);

			m_nNgonDangDat++;
			if (m_nNgonDangDat > m_nNgonToiDa)
				m_nNgonToiDa = m_nNgonDangDat;
			if (bNgonDau)
				return false;	// ngon thu nhat: de chuot gia lap lo nhu cu

			// Ngon thu hai tro di: uu tien nut ky nang, roi den can dieu khien.
			// [ANDROID 10/09 GANTOADO] dang sua giao dien thi khong bat nut ky nang
			if (m_nNgonKyNang < 0 && !UiToaDo_DangSua())
			{
				int nNut = JxKyNang_TrungNut((int)fx, (int)fy);
				if (nNut > 0)
				{
					m_nNgonKyNang = nNgon;
					JxKyNang_BatDau(nNut, (int)fx, (int)fy);
					return true;
				}
			}
			if (m_nNgonCan < 0 && !JxCan_DangCam()
				&& JxCan_TrongVung((int)fx, (int)fy)
				&& !JxUi_CoGiaoDienTaiDiem((int)fx, (int)fy))
			{
				m_nNgonCan = nNgon;
				JxCan_BatDau((int)fx, (int)fy, (int)fx, (int)fy);
				return true;
			}
			return false;
		}

		if (ev.type == SDL_EVENT_FINGER_MOTION)
		{
			if (nNgon == m_nNgonKyNang)
			{
				JxKyNang_Keo((int)fx, (int)fy);
				return true;
			}
			if (nNgon == m_nNgonCan)
			{
				JxCan_Keo((int)fx, (int)fy);
				return true;
			}
			return false;
		}

		// FINGER_UP / FINGER_CANCELED
		if (m_nNgonDangDat > 0)
			m_nNgonDangDat--;
		if (nNgon == m_nNgonKyNang)
		{
			JxKyNang_Nha();
			m_nNgonKyNang = -1;
			return true;
		}
		if (nNgon == m_nNgonCan)
		{
			JxCan_Nha();
			m_nNgonCan = -1;
			return true;
		}
		return false;
	}
	if (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN || ev.type == SDL_EVENT_MOUSE_BUTTON_UP)
	{
		if (ev.button.which != SDL_TOUCH_MOUSEID || ev.button.button != SDL_BUTTON_LEFT)
			return false;
		float fx = ev.button.x, fy = ev.button.y; SdlToLogical(m_pWindow, fx, fy);
		if (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
		{
			m_nCham = CHAM_CHO;
			m_nChamX0 = m_nChamX = (int)fx; m_nChamY0 = m_nChamY = (int)fy;
			m_uChamDat = (unsigned int)SDL_GetTicks();
			// [ANDROID 09/09 KYNANG] Dat ngon trung mot nut ky nang thi bat NGAY, khong doi
			// xe dich: nut la mot o cu the nen dat trung no la chac chan muon dung no.
			// Nho vay cham vao nut cung khong lot mot cu bam chuot xuong duoi game.
			// [ANDROID 10/09 BANGCHON] bang 3 nut canh ky nang dang mo: cham vao no thi bang nhan, khong cho roi
			// xuong cua so ky nang ben duoi; cham ra ngoai thi bang tu dong, cu cham di tiep.
			if (JxKyNang_ChamBangChon(m_nChamX0, m_nChamY0))
			{
				m_nCham = CHAM_KHONG;
				return true;
			}
			{	// [ANDROID 11/09 ICON b] cham icon "Giao tiep" tren dau NPC = bam vao than NPC (di toi + mo thoai),
				// ke ca khi icon nam ngoai hinh NPC. Chu: "khi bam vao icon do thi se tu di chuyen toi npc mo hoi thoai".
				int nNpcX = 0, nNpcY = 0;
				if (!UiToaDo_DangSua() && JxIconNpc_Cham(m_nChamX0, m_nChamY0, &nNpcX, &nNpcY))
				{
					LPARAM lNpc = MAKELPARAM(nNpcX, nNpcY);
					GhiChuot(0, lNpc);
					MsgProc(hWnd, WM_MOUSEMOVE, 0, lNpc);
					GhiChuot(MK_LBUTTON, lNpc);
					MsgProc(hWnd, WM_LBUTTONDOWN, MK_LBUTTON, lNpc);
					GhiChuot(0, lNpc);
					MsgProc(hWnd, WM_LBUTTONUP, 0, lNpc);
					g_DebugLog("[ICON] cham icon giao tiep -> bam NPC tai %d,%d", nNpcX, nNpcY);
					m_nCham = CHAM_KHONG;
					return true;
				}
			}
			// [ANDROID 10/09 GANTOADO] Dang sua giao dien thi KHONG cho nut ky nang nuot
			// cu cham - phai de no di xuong UiToaDo thi moi keo cum nut di duoc.
			if (!UiToaDo_DangSua())
			{
				int nNutKN = JxKyNang_TrungNut(m_nChamX0, m_nChamY0);
				if (nNutKN > 0)
				{
					m_nCham = CHAM_KYNANG;
					JxKyNang_BatDau(nNutKN, m_nChamX0, m_nChamY0);
					return true;
				}
			}
			m_nNgonToiDa = m_nNgonDangDat;	// [ANDROID 09/09 HAINGON] bat dau dem lai cho lan cham nay
			// Dua "chuot" toi cho ngon tay ngay: de game biet dang tro vao dau (dem hover, thong tin vat pham).
			GhiChuot(0, MAKELPARAM(m_nChamX0, m_nChamY0));
			MsgProc(hWnd, WM_MOUSEMOVE, 0, MAKELPARAM(m_nChamX0, m_nChamY0));
			return true;
		}
		int nTruoc = m_nCham;
		m_nCham = CHAM_KHONG;
		if (nTruoc == CHAM_KYNANG)
		{
			JxKyNang_Nha();		// [ANDROID 09/09 KYNANG] nha ngon = danh
		}
		else if (nTruoc == CHAM_CAN)
		{
			JxCan_Nha();
		}
		else if (nTruoc == CHAM_KEO)
		{
			GhiChuot(0, MAKELPARAM((int)fx, (int)fy));
			MsgProc(hWnd, WM_LBUTTONUP, 0, MAKELPARAM((int)fx, (int)fy));
		}
		else if (nTruoc == CHAM_PHAI)
		{
			GhiChuot(0, MAKELPARAM(m_nChamX0, m_nChamY0));
			MsgProc(hWnd, WM_RBUTTONUP, 0, MAKELPARAM(m_nChamX0, m_nChamY0));
		}
		else if (nTruoc == CHAM_CHO || nTruoc == CHAM_RE)
		{
			// CHAM_RE = da giu lau tren GIAO DIEN. Van bam chuot trai khi nha: tren dien thoai nguoi ta
			// hay an nut lau hon 400 ms, khong the vi the ma nut chet. (Trong luc giu thi "chuot" da nam
			// san o do nen game da kip hien thong tin vat pham.)
			unsigned int uNay = (unsigned int)SDL_GetTicks();
			bool bDup = (uNay - m_uChamNhaTruoc <= CHAM_HAI_MS) &&
				(abs(m_nChamX0 - m_nChamNhaX) <= CHAM_HAI_XA) &&
				(abs(m_nChamY0 - m_nChamNhaY) <= CHAM_HAI_XA);
			LPARAM l = MAKELPARAM(m_nChamX0, m_nChamY0);
			// [ANDROID 11/09 KHOAMT c] cham ngoai giao dien: khoa muc tieu tai dung cho cham -> thanh thong tin bam theo nguoi vua cham,
			// khong chay theo hover (SinhHover bom WM_MOUSEMOVE moi khung, the gioi troi nen NPC duoi con tro doi lien tuc).
			if (!JxUi_CoGiaoDienTaiDiem(m_nChamX0, m_nChamY0))
				JxMucTieu_Khoa(m_nChamX0, m_nChamY0);
			GhiChuot(MK_LBUTTON, l);
			MsgProc(hWnd, bDup ? WM_LBUTTONDBLCLK : WM_LBUTTONDOWN, MK_LBUTTON, l);
			GhiChuot(0, l);
			MsgProc(hWnd, WM_LBUTTONUP, 0, l);
			m_uChamNhaTruoc = uNay; m_nChamNhaX = m_nChamX0; m_nChamNhaY = m_nChamY0;
		}
		return true;
	}
	if (ev.type == SDL_EVENT_MOUSE_MOTION)
	{
		if (ev.motion.which != SDL_TOUCH_MOUSEID)
			return false;
		if (m_nCham == CHAM_KHONG)
			return true;	// nuot: ngon tay da nhac len roi, dung de con tro chay lung tung
		float fx = ev.motion.x, fy = ev.motion.y; SdlToLogical(m_pWindow, fx, fy);
		int nYTruoc = m_nChamY;		// [ANDROID 09/09 CUON] de tinh doan vua vuot duoc
		m_nChamX = (int)fx; m_nChamY = (int)fy;
		if (m_nCham == CHAM_CHO &&
			(abs(m_nChamX - m_nChamX0) > CHAM_NGUONG || abs(m_nChamY - m_nChamY0) > CHAM_NGUONG))
		{
			// [ANDROID 09/09 SUAKEO] Dang sua giao dien: y dinh luon la KEO o giao dien,
			// khong bao gio la cuon danh sach hay cam can. Khong co dong nay thi vuot doc
			// tren mot o se thanh CUON (nhanh ngay duoi) va o do khong bao gio doi cho duoc.
			if (UiToaDo_DangSua())
			{
				m_nCham = CHAM_KEO;
				GhiChuot(MK_LBUTTON, MAKELPARAM(m_nChamX0, m_nChamY0));
				MsgProc(hWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(m_nChamX0, m_nChamY0));
				return true;	// khong duoc roi xuong nhanh CUON ben duoi
			}
			// [ANDROID 09/09 CAN] Keo o vung ben trai (ngoai giao dien) = CAN DIEU KHIEN;
			// keo o cho khac = giu chuot trai roi re nhu ban PC (di lien tuc, keo tha vat pham).
			if (JxCan_TrongVung(m_nChamX0, m_nChamY0) && !JxUi_CoGiaoDienTaiDiem(m_nChamX0, m_nChamY0))
			{
				m_nCham = CHAM_CAN;
				JxCan_BatDau(m_nChamX0, m_nChamY0, m_nChamX, m_nChamY);
				return true;
			}
			// [ANDROID 09/09 CUON] Vuot DOC tren giao dien = cuon danh sach (dich thanh lan chuot).
			// Moi lop danh sach cua bo giao dien nay deu nhan WM_MOUSEWHEEL (WndList, WndList2,
			// WndMessageListBox) nen thoai NPC / chat / danh sach may chu deu vuot duoc.
			// Vuot NGANG thi van la giu chuot trai roi re (keo cua so di cho khac).
			if (JxUi_CoGiaoDienTaiDiem(m_nChamX0, m_nChamY0) &&
				abs(m_nChamY - m_nChamY0) > abs(m_nChamX - m_nChamX0))
			{
				m_nCham = CHAM_CUON;
				m_nCuonDon = 0;
				// [ANDROID 10/09 CUON] WM_MOUSEWHEEL khong mang toa do - Wnd_ProcessInput lay cho CON TRO,
				// ma con tro con o cham truoc (cho khac) -> lan chuot roi vao cua so khac, khung chat
				// khong cuon. Dua con tro ve cho dat ngon truoc.
				MsgProc(hWnd, WM_MOUSEMOVE, 0, MAKELPARAM(m_nChamX0, m_nChamY0));
				g_DebugLog("[CHAM] cuon tai %d,%d", m_nChamX0, m_nChamY0);
				return true;
			}
			m_nCham = CHAM_KEO;		// da xe dich -> giu chuot trai tu CHO DAT NGON roi keo
			GhiChuot(MK_LBUTTON, MAKELPARAM(m_nChamX0, m_nChamY0));
			MsgProc(hWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(m_nChamX0, m_nChamY0));
		}
		if (m_nCham == CHAM_KYNANG)
		{
			JxKyNang_Keo(m_nChamX, m_nChamY);	// [ANDROID 09/09 KYNANG] ngam huong danh
			return true;
		}
		if (m_nCham == CHAM_CAN)
		{
			JxCan_Keo(m_nChamX, m_nChamY);
			return true;
		}
		if (m_nCham == CHAM_CUON)
		{
			// Ngon di XUONG = doc nguoc len = lan chuot VE PHIA TRUOC (delta duong), giong moi may.
			const int CHAM_CUON_BUOC = 28;		// bao nhieu diem anh vuot thi thanh mot nac lan
			m_nCuonDon += (m_nChamY - nYTruoc);
			while (m_nCuonDon >= CHAM_CUON_BUOC || m_nCuonDon <= -CHAM_CUON_BUOC)
			{
				int nDau = (m_nCuonDon > 0) ? 1 : -1;
				m_nCuonDon -= nDau * CHAM_CUON_BUOC;
				WPARAM w = MAKEWPARAM(0, (WORD)(short)(nDau * WHEEL_DELTA));
				MsgProc(hWnd, WM_MOUSEWHEEL, w, MAKELPARAM(m_nChamX0, m_nChamY0));
			}
			return true;
		}
		WPARAM w = (m_nCham == CHAM_KEO) ? MK_LBUTTON : (WPARAM)((m_nCham == CHAM_PHAI) ? MK_RBUTTON : 0);
		GhiChuot(w, MAKELPARAM(m_nChamX, m_nChamY));
		MsgProc(hWnd, WM_MOUSEMOVE, w, MAKELPARAM(m_nChamX, m_nChamY));
		return true;
	}
	return false;
}

void KSdlApp::NhipCham()
{
	if (m_nCham != CHAM_CHO)
		return;
	if ((unsigned int)SDL_GetTicks() - m_uChamDat < CHAM_GIU_MS)
		return;
	// CHAM_RE khong con dung: cham giu o DAU cung la chuot phai.
	// Tren GIAO DIEN, chuot phai chinh la duong MAC / THAO / DUNG vat pham cua ban PC - bo no di thi
	// nguoi choi khong mac duoc do. Tren BAN DO, chuot phai la danh ep / chon muc tieu.
	g_DebugLog("[CHAM] giu tai %d,%d -> chuot phai (%s)", m_nChamX0, m_nChamY0,
		JxUi_CoGiaoDienTaiDiem(m_nChamX0, m_nChamY0) ? "giao dien" : "ban do");
	m_nCham = CHAM_PHAI;
	GhiChuot(MK_RBUTTON, MAKELPARAM(m_nChamX0, m_nChamY0));
	MsgProc(g_GetMainHWnd(), WM_RBUTTONDOWN, MK_RBUTTON, MAKELPARAM(m_nChamX0, m_nChamY0));
}
#endif

bool KSdlApp::TranslateEvent(const SDL_Event& ev)
{
	HWND hWnd = g_GetMainHWnd();
#ifdef JX_POSIX	// [ANDROID 08/09] chan doan: su kien ban phim / chuot co toi khong
	{
		static int s_nPhim = 0, s_nChuot = 0;
		if (ev.type >= SDL_EVENT_KEY_DOWN && ev.type <= SDL_EVENT_TEXT_EDITING_CANDIDATES && s_nPhim < 60)
			{ s_nPhim++; g_DebugLog("[GO] su kien SDL 0x%X", (unsigned)ev.type); }
		if (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN && s_nChuot < 10)
			{ s_nChuot++; g_DebugLog("[GO] chuot xuong tai %d,%d", (int)ev.button.x, (int)ev.button.y); }
	}
#endif
#ifdef JX_ANDROID
	if (ChamSuKien(ev))		// [ANDROID 09/09 CHAM] su kien chuot do NGON TAY sinh ra di duong rieng
		return true;
#endif
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
#ifdef JX_POSIX	// [ANDROID 08/09] chan doan ban phim mem
		{
			static int s_nPhim = 0;
			if (s_nPhim < 30) { s_nPhim++; g_DebugLog("[GO] phim %s key=0x%X scancode=%d -> vk=0x%X",
				(ev.type == SDL_EVENT_KEY_DOWN) ? "xuong" : "len", (unsigned)ev.key.key, (int)ev.key.scancode, (unsigned)wVk); }
		}
#endif
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
#ifdef JX_POSIX	// [ANDROID 08/09] chan doan ban phim mem
		{
			static int s_nChu = 0;
			if (s_nChu < 30) { s_nChu++; g_DebugLog("[GO] chu vao: \"%.20s\"", ev.text.text ? ev.text.text : "(rong)"); }
		}
#endif
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
