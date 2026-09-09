# -*- coding: utf-8 -*-
# [ANDROID 09/09] Dot va 12: DIEU KHIEN BANG NGON TAY (muc 1 cua LOTRINH_MOBILE_0909.md).
#
# Truoc dot nay game chi nhan cham qua kieu "gia lap chuot" cua SDL: MOT NGON = CHUOT TRAI, het.
# Thieu chuot phai (danh ep / menu), thieu re chuot (thong tin vat pham), ban phim ao bat suot che
# nua man hinh, nut Back cua Android khong lam gi.
#
# Bo nhan cu chi (KSdlApp):
#   cham nhanh roi nha        -> chuot trai (bam tai CHO DAT NGON, khong phai cho nha)
#   cham hai lan lien         -> bam dup (dung vat pham)
#   cham roi keo di           -> giu chuot trai + re (di chuyen lien tuc, keo tha vat pham)
#   cham giu tai cho tren BAN DO  -> CHUOT PHAI (danh ep, menu chuot phai)
#   cham giu tai cho tren GIAO DIEN -> chi de "chuot" o do cho hien thong tin; KHONG bam chuot phai
#      (chuot phai trong tui do la DUNG vat pham - bam nham la mat do)
#
# Ban phim ao: chi bat khi mot o nhap (KWndEdit) co tieu diem, tat khi mat tieu diem.
# Nut Back cua Android: thanh phim ESC.
#
# Ban Windows khong doi hanh vi: moi khoi moi deu trong #ifdef JX_ANDROID.
import io, os, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def doc(p):
    return io.open(os.path.join(ROOT, p), encoding="latin-1", newline="").read()

def ghi(p, s):
    io.open(os.path.join(ROOT, p), "w", encoding="latin-1", newline="").write(s)

def nl(s, crlf):
    return s.replace("\n", "\r\n") if crlf else s

def va(duong, dau_da_co, cu, moi, ten):
    s = doc(duong)
    crlf = ("\r\n" in s)
    if nl(dau_da_co, crlf) in s:
        print("  bo qua (da co): %s" % ten)
        return
    cu2, moi2 = nl(cu, crlf), nl(moi, crlf)
    if s.count(cu2) != 1:
        print("  !! KHONG VA DUOC %s: tim thay %d cho (can dung 1)" % (ten, s.count(cu2)))
        sys.exit(1)
    ghi(duong, s.replace(cu2, moi2))
    print("  va xong: %s" % ten)


KSDL_H = "Sources/S3Client/Platform/KSdlApp.h"
KSDL_C = "Sources/S3Client/Platform/KSdlApp.cpp"

# ---------------------------------------------------------------------------
# 1. KSdlApp.h: them trang thai bo nhan cu chi
# ---------------------------------------------------------------------------
print("1. KSdlApp.h: khai bao bo nhan cu chi")
va(KSDL_H, "ChamSuKien",
"""	SDL_Window*		m_pWindow;""",
"""#ifdef JX_ANDROID
	// [ANDROID 09/09 CHAM] bo nhan cu chi ngon tay -> chuot (xem KSdlApp.cpp)
	enum { CHAM_KHONG = 0, CHAM_CHO, CHAM_KEO, CHAM_PHAI, CHAM_RE };
	bool			ChamSuKien(const SDL_Event& ev);	// true = da nuot su kien, dung dich tiep
	void			NhipCham();							// goi moi vong lap: giu du lau thi thanh chuot phai
	int				m_nCham;			// mot trong CHAM_*
	int				m_nChamX0, m_nChamY0;	// cho dat ngon (toa do khung ve)
	int				m_nChamX, m_nChamY;		// cho ngon dang o
	unsigned int	m_uChamDat;			// luc dat ngon (ms)
	unsigned int	m_uChamNhaTruoc;	// luc nha ngon lan truoc (de nhan cham hai lan)
	int				m_nChamNhaX, m_nChamNhaY;
#endif
	SDL_Window*		m_pWindow;""", "KSdlApp.h thanh vien")

# ---------------------------------------------------------------------------
# 2. Nut Back cua Android = ESC
# ---------------------------------------------------------------------------
print("2. Nut Back = ESC")
va(KSDL_C, "SDLK_AC_BACK",
"""	case SDLK_ESCAPE:		return VK_ESCAPE;""",
"""	case SDLK_ESCAPE:		return VK_ESCAPE;
#ifdef JX_ANDROID
	// [ANDROID 09/09 CHAM] nut Back cua may = ESC (mo bang he thong / dong cua so dang mo).
	// Manifest da dat SDL_ANDROID_TRAP_BACK_BUTTON=1 nen SDL dua nut nay vao day thay vi thoat app.
	case SDLK_AC_BACK:		return VK_ESCAPE;
#endif""", "SdlKeyToVk them AC_BACK")

# ---------------------------------------------------------------------------
# 3. Ban phim ao: khong bat san
# ---------------------------------------------------------------------------
print("3. Ban phim ao chi bat khi co o nhap")
va(KSDL_C, "JxSdl_BanPhimAo",
"""	SDL_StartTextInput(m_pWindow);	// WM_CHAR tu SDL_EVENT_TEXT_INPUT (o mobile se bat/tat theo o nhap)""",
"""#ifdef JX_ANDROID
	// [ANDROID 09/09 CHAM] KHONG bat go chu san: tren dien thoai SDL_StartTextInput = BAT BAN PHIM AO ngay
	// va no che nua man hinh cho toi luc thoat. Chi bat khi mot o nhap co tieu diem - KWndEdit goi
	// JxSdl_BanPhimAo() o WND_M_SET_FOCUS / WND_M_KILL_FOCUS.
#else
	SDL_StartTextInput(m_pWindow);	// WM_CHAR tu SDL_EVENT_TEXT_INPUT
#endif""", "Init khong bat ban phim ao")

# ---------------------------------------------------------------------------
# 4. Than bo nhan cu chi + JxSdl_BanPhimAo (dat ngay truoc TranslateEvent)
# ---------------------------------------------------------------------------
print("4. Than bo nhan cu chi")
THAN = r'''#ifdef JX_ANDROID
//---------------------------------------------------------------------------
// [ANDROID 09/09 CHAM] BAN PHIM AO
// Tren dien thoai SDL_StartTextInput = day ban phim ao len ngay. Nen chi goi khi mot o nhap co tieu diem.
// KWndEdit::WndProc goi ham nay o WND_M_SET_FOCUS (bat) va WND_M_KILL_FOCUS (tat).
//---------------------------------------------------------------------------
extern "C" void JxSdl_BanPhimAo(int bBat)
{
	SDL_Window* pWin = (SDL_Window*)JxPosix_MainWindow();
	if (!pWin)
		return;
	if (bBat)
		SDL_StartTextInput(pWin);
	else
		SDL_StopTextInput(pWin);
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

static const unsigned int CHAM_GIU_MS = 400;	// giu lau bao nhieu thi thanh chuot phai
static const int          CHAM_NGUONG = 12;		// xe dich qua bao nhieu diem anh thi coi la KEO
static const unsigned int CHAM_HAI_MS = 400;	// hai lan cham cach nhau duoi bao nhieu = bam dup
static const int          CHAM_HAI_XA = 24;		// ... va cach nhau khong qua bao nhieu diem anh

bool KSdlApp::ChamSuKien(const SDL_Event& ev)
{
	HWND hWnd = g_GetMainHWnd();
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
			// Dua "chuot" toi cho ngon tay ngay: de game biet dang tro vao dau (dem hover, thong tin vat pham).
			GhiChuot(0, MAKELPARAM(m_nChamX0, m_nChamY0));
			MsgProc(hWnd, WM_MOUSEMOVE, 0, MAKELPARAM(m_nChamX0, m_nChamY0));
			return true;
		}
		int nTruoc = m_nCham;
		m_nCham = CHAM_KHONG;
		if (nTruoc == CHAM_KEO)
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
		m_nChamX = (int)fx; m_nChamY = (int)fy;
		if (m_nCham == CHAM_CHO &&
			(abs(m_nChamX - m_nChamX0) > CHAM_NGUONG || abs(m_nChamY - m_nChamY0) > CHAM_NGUONG))
		{
			m_nCham = CHAM_KEO;		// da xe dich -> giu chuot trai tu CHO DAT NGON roi keo
			GhiChuot(MK_LBUTTON, MAKELPARAM(m_nChamX0, m_nChamY0));
			MsgProc(hWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(m_nChamX0, m_nChamY0));
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
	if (JxUi_CoGiaoDienTaiDiem(m_nChamX0, m_nChamY0))
	{
		m_nCham = CHAM_RE;	// tren giao dien: chi he ra xem, khong bam chuot phai
		g_DebugLog("[CHAM] giu tai %d,%d tren GIAO DIEN -> chi he ra xem (khong bam chuot phai)", m_nChamX0, m_nChamY0);
		return;
	}
	g_DebugLog("[CHAM] giu tai %d,%d tren BAN DO -> chuot phai", m_nChamX0, m_nChamY0);
	m_nCham = CHAM_PHAI;
	GhiChuot(MK_RBUTTON, MAKELPARAM(m_nChamX0, m_nChamY0));
	MsgProc(g_GetMainHWnd(), WM_RBUTTONDOWN, MK_RBUTTON, MAKELPARAM(m_nChamX0, m_nChamY0));
}
#endif

'''
s = doc(KSDL_C)
crlf = ("\r\n" in s)
if "KSdlApp::ChamSuKien" in s:
    print("  bo qua (da co): than bo nhan cu chi")
else:
    neo = nl("bool KSdlApp::TranslateEvent(const SDL_Event& ev)\n{", crlf)
    if s.count(neo) != 1:
        print("  !! khong tim thay TranslateEvent")
        sys.exit(1)
    i = s.find(neo)
    ghi(KSDL_C, s[:i] + nl(THAN, crlf) + s[i:])
    print("  va xong: than bo nhan cu chi")

# ---------------------------------------------------------------------------
# 5. Noi vao TranslateEvent + vong lap Run
# ---------------------------------------------------------------------------
print("5. Noi vao TranslateEvent va vong lap")
va(KSDL_C, "if (ChamSuKien(ev))",
"""	switch (ev.type)
	{
	case SDL_EVENT_QUIT:""",
"""#ifdef JX_ANDROID
	if (ChamSuKien(ev))		// [ANDROID 09/09 CHAM] su kien chuot do NGON TAY sinh ra di duong rieng
		return true;
#endif
	switch (ev.type)
	{
	case SDL_EVENT_QUIT:""", "TranslateEvent goi bo nhan cu chi")

va(KSDL_C, "NhipCham();",
"""		if (bQuit)
			break;
		if (m_bActive || m_bMultiGame)""",
"""		if (bQuit)
			break;
#ifdef JX_ANDROID
		NhipCham();		// [ANDROID 09/09 CHAM] giu ngon du lau ma khong xe dich -> chuot phai
#endif
		if (m_bActive || m_bMultiGame)""", "Run goi NhipCham")

# ---------------------------------------------------------------------------
# 6. Khoi tao thanh vien trong ham dung
# ---------------------------------------------------------------------------
va(KSDL_C, "m_uChamNhaTruoc = 0;",
"""	m_cHoverCounter = 0;
	s_pSdlApp = this;""",
"""	m_cHoverCounter = 0;
#ifdef JX_ANDROID
	m_nCham = CHAM_KHONG;
	m_nChamX0 = m_nChamY0 = m_nChamX = m_nChamY = 0;
	m_uChamDat = 0;
	m_uChamNhaTruoc = 0;
	m_nChamNhaX = m_nChamNhaY = 0;
#endif
	s_pSdlApp = this;""", "ham dung khoi tao")

# ---------------------------------------------------------------------------
# 7. Wnds.cpp: hoi xem diem co nam tren giao dien khong
# ---------------------------------------------------------------------------
print("6. Wnds.cpp: hoi diem co tren giao dien khong")
va("Sources/S3Client/Ui/Elem/Wnds.cpp", "JxUi_CoGiaoDienTaiDiem",
"""KWndWindow* Wnd_GetActive(int x, int y, bool bBringToTop)
{""",
"""#ifdef JX_ANDROID
// [ANDROID 09/09 CHAM] Diem (x, y) co nam tren mot cua so GIAO DIEN khong (khac vung ban do)?
// KSdlApp dung de quyet dinh y nghia cua "cham giu tai cho": tren ban do thi la CHUOT PHAI,
// con tren giao dien thi chi he ra xem thong tin - vi chuot phai trong tui do la DUNG vat pham.
extern "C" int JxUi_CoGiaoDienTaiDiem(int x, int y)
{
	KWndWindow* pWnd = Wnd_GetActive(x, y, false);
	return (pWnd != NULL && pWnd != s_WndStation.pGameSpaceWnd) ? 1 : 0;
}
#endif

KWndWindow* Wnd_GetActive(int x, int y, bool bBringToTop)
{""", "Wnds.cpp JxUi_CoGiaoDienTaiDiem")

# ---------------------------------------------------------------------------
# 8. WndEdit.cpp: bat/tat ban phim ao theo tieu diem
# ---------------------------------------------------------------------------
print("7. WndEdit.cpp: ban phim ao theo tieu diem")
va("Sources/S3Client/Ui/Elem/WndEdit.cpp", "JxSdl_BanPhimAo",
"""	case WND_M_SET_FOCUS:
		if (m_pText)
		{""",
"""	case WND_M_SET_FOCUS:
#ifdef JX_ANDROID
		JxSdl_BanPhimAo(1);		// [ANDROID 09/09 CHAM] o nhap co tieu diem -> day ban phim ao len
#endif
		if (m_pText)
		{""", "WndEdit bat ban phim ao")

va("Sources/S3Client/Ui/Elem/WndEdit.cpp", "JxSdl_BanPhimAo(0)",
"""	case WND_M_KILL_FOCUS:""",
"""	case WND_M_KILL_FOCUS:
#ifdef JX_ANDROID
		JxSdl_BanPhimAo(0);		// [ANDROID 09/09 CHAM] khong con o nhap nao -> cat ban phim ao di
#endif""", "WndEdit tat ban phim ao")

va("Sources/S3Client/Ui/Elem/WndEdit.cpp", 'extern "C" void JxSdl_BanPhimAo',
"""static KIme	s_Ime;""",
"""static KIme	s_Ime;
#ifdef JX_ANDROID
extern "C" void JxSdl_BanPhimAo(int bBat);	// KSdlApp.cpp
#endif""", "WndEdit khai bao JxSdl_BanPhimAo")

print("")
print("XONG dot va 12.")
