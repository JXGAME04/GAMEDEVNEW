# -*- coding: utf-8 -*-
# [ANDROID 09/09] Dot va 13: CAN DIEU KHIEN AO + PHIM BO TRO (Shift/Ctrl/Alt) that su chay.
#
# 1. LOI THAT: g_pfnJxGetKeyState KHONG AI NOI -> GetKeyState()/GetAsyncKeyState() tren Android LUON
#    tra 0. Nghia la moi to hop Shift/Ctrl/Alt + chuot deu chet (Ctrl+phai = menu nguoi choi de giao
#    dich/to doi, Shift+trai = danh ep...), ke ca khi cam ban phim roi. Nay noi vao bang phim cua SDL,
#    cong them mot mat na "phim dinh" de lop cham co the giu ho phim bo tro.
#
# 2. CAN DIEU KHIEN AO (JxCanDieuKhien.cpp): KEO ngon o nua trai man hinh = can dieu khien,
#    moi vong lap goi iCoreShell::Goto(nDir, 0) - dung cach ban JX1 Mobile cua chu lam (lop HRocker).
#    CHAM van la bam chuot trai nhu thuong nen khong mat thao tac nao cua ban PC.
#
# Ban Windows khong doi hanh vi: moi khoi moi deu trong #ifdef JX_ANDROID.
import io, os, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BS = chr(92)


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


KSDL_C = "Sources/S3Client/Platform/KSdlApp.cpp"
KSDL_H = "Sources/S3Client/Platform/KSdlApp.h"

# ---------------------------------------------------------------------------
# 1. Bang phim: noi g_pfnJxGetKeyState + mat na phim dinh
# ---------------------------------------------------------------------------
print("1. Noi GetKeyState vao bang phim cua SDL")
KHOI1 = r'''#ifdef JX_ANDROID
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

'''
s = doc(KSDL_C)
crlf = ("\r\n" in s)
if "JxSdl_TrangThaiPhim" in s:
    print("  bo qua (da co): bang phim")
else:
    neo = nl("static WORD SdlKeyToVk(SDL_Keycode key)", crlf)
    i = s.find(neo)
    if i < 0:
        print("  !! khong tim thay khoi CHAM de dat truoc")
        sys.exit(1)
    ghi(KSDL_C, s[:i] + nl(KHOI1, crlf) + s[i:])
    print("  va xong: bang phim")

va(KSDL_C, "g_pfnJxGetKeyState = JxSdl_TrangThaiPhim",
"""#ifdef JX_POSIX
	HWND hWnd = (HWND)m_pWindow;	// [ANDROID 08/09] tren POSIX "HWND" = SDL_Window* (KPosixWin32: GetClientRect/SetWindowText... hieu no)
	JxPosix_SetMainWindow(m_pWindow);""",
"""#ifdef JX_POSIX
	HWND hWnd = (HWND)m_pWindow;	// [ANDROID 08/09] tren POSIX "HWND" = SDL_Window* (KPosixWin32: GetClientRect/SetWindowText... hieu no)
	JxPosix_SetMainWindow(m_pWindow);
#ifdef JX_ANDROID
	g_pfnJxGetKeyState = JxSdl_TrangThaiPhim;	// [ANDROID 09/09 PHIM] khong noi thi GetKeyState luon tra 0
#endif""", "noi moc GetKeyState")

# ---------------------------------------------------------------------------
# 2. Can dieu khien: noi vao bo nhan cu chi
# ---------------------------------------------------------------------------
print("2. Noi can dieu khien vao bo nhan cu chi")

va(KSDL_C, '#include "JxCanDieuKhien.h"',
"""static WORD SdlKeyToVk(SDL_Keycode key)""",
"""#ifdef JX_ANDROID
#include "JxCanDieuKhien.h"	// [ANDROID 09/09 CAN] can dieu khien ao
#endif

static WORD SdlKeyToVk(SDL_Keycode key)""", "KSdlApp include can dieu khien")

# CHAM_KEO: neu bat dau o vung can (va khong tren giao dien) thi la CAN, khong phai giu chuot trai
va(KSDL_C, "CHAM_CAN",
"""		if (m_nCham == CHAM_CHO &&
			(abs(m_nChamX - m_nChamX0) > CHAM_NGUONG || abs(m_nChamY - m_nChamY0) > CHAM_NGUONG))
		{
			m_nCham = CHAM_KEO;		// da xe dich -> giu chuot trai tu CHO DAT NGON roi keo
			GhiChuot(MK_LBUTTON, MAKELPARAM(m_nChamX0, m_nChamY0));
			MsgProc(hWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(m_nChamX0, m_nChamY0));
		}""",
"""		if (m_nCham == CHAM_CHO &&
			(abs(m_nChamX - m_nChamX0) > CHAM_NGUONG || abs(m_nChamY - m_nChamY0) > CHAM_NGUONG))
		{
			// [ANDROID 09/09 CAN] Keo o vung ben trai (ngoai giao dien) = CAN DIEU KHIEN;
			// keo o cho khac = giu chuot trai roi re nhu ban PC (di lien tuc, keo tha vat pham).
			if (JxCan_TrongVung(m_nChamX0, m_nChamY0) && !JxUi_CoGiaoDienTaiDiem(m_nChamX0, m_nChamY0))
			{
				m_nCham = CHAM_CAN;
				JxCan_BatDau(m_nChamX0, m_nChamY0, m_nChamX, m_nChamY);
				return true;
			}
			m_nCham = CHAM_KEO;		// da xe dich -> giu chuot trai tu CHO DAT NGON roi keo
			GhiChuot(MK_LBUTTON, MAKELPARAM(m_nChamX0, m_nChamY0));
			MsgProc(hWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(m_nChamX0, m_nChamY0));
		}
		if (m_nCham == CHAM_CAN)
		{
			JxCan_Keo(m_nChamX, m_nChamY);
			return true;
		}""", "cu chi keo -> can dieu khien")

va(KSDL_C, "JxCan_Nha();",
"""		int nTruoc = m_nCham;
		m_nCham = CHAM_KHONG;
		if (nTruoc == CHAM_KEO)""",
"""		int nTruoc = m_nCham;
		m_nCham = CHAM_KHONG;
		if (nTruoc == CHAM_CAN)
		{
			JxCan_Nha();
		}
		else if (nTruoc == CHAM_KEO)""", "nha ngon -> tha can")

va(KSDL_H, "CHAM_CAN",
"""	enum { CHAM_KHONG = 0, CHAM_CHO, CHAM_KEO, CHAM_PHAI, CHAM_RE };""",
"""	enum { CHAM_KHONG = 0, CHAM_CHO, CHAM_KEO, CHAM_PHAI, CHAM_RE, CHAM_CAN };""", "KSdlApp.h them CHAM_CAN")

va(KSDL_C, "JxCan_Nhip();",
"""#ifdef JX_ANDROID
		NhipCham();		// [ANDROID 09/09 CHAM] giu ngon du lau ma khong xe dich -> chuot phai
#endif""",
"""#ifdef JX_ANDROID
		NhipCham();		// [ANDROID 09/09 CHAM] giu ngon du lau ma khong xe dich -> chuot phai
		JxCan_Nhip();	// [ANDROID 09/09 CAN] dang cam can thi day nhan vat di theo huong
#endif""", "vong lap goi JxCan_Nhip")

# ---------------------------------------------------------------------------
# 3. Ve can dieu khien o cuoi moi khung ve
# ---------------------------------------------------------------------------
print("3. Ve can dieu khien o cuoi khung")
s = doc("Sources/S3Client/Ui/UiShell.cpp")
crlf = ("\r\n" in s)
if "JxCan_Ve" in s:
    print("  bo qua (da co): ve can")
else:
    neo = nl('#include "UiCase/UiInit.h"', crlf)
    if s.count(neo) != 1:
        print("  !! khong tim thay include UiInit.h")
        sys.exit(1)
    them = '\n#ifdef JX_ANDROID\n#include "../Platform/JxCanDieuKhien.h"\t// [ANDROID 09/09 CAN] can dieu khien ao\n#endif'
    s = s.replace(neo, neo + nl(them, crlf))
    neo2 = nl("\tg_pRepresentShell->RepresentEnd();", crlf)
    if s.count(neo2) != 1:
        print("  !! khong tim thay RepresentEnd (thay %d cho)" % s.count(neo2))
        sys.exit(1)
    truoc = "#ifdef JX_ANDROID\n\tJxCan_Ve();\t// [ANDROID 09/09 CAN] ve can len tren cung, ngay truoc khi ket khung\n#endif\n"
    s = s.replace(neo2, nl(truoc, crlf) + neo2)
    ghi("Sources/S3Client/Ui/UiShell.cpp", s)
    print("  va xong: UiShell ve can dieu khien")

# ---------------------------------------------------------------------------
# 4. Cham GIU tren GIAO DIEN cung phai la CHUOT PHAI.
#    Do la duong duy nhat de MAC / THAO / DUNG vat pham (ban PC: chuot phai trong tui do = dung/mac).
#    Dot 12 da chan chuot phai tren giao dien vi so bam nham - nhung nhu the thi khong mac duoc do,
#    tuc la mat han mot mang thao tac. Ban JX1 Mobile cua chu cung dung cham giu cho viec nay.
# ---------------------------------------------------------------------------
print("4. Cham giu tren giao dien = chuot phai (mac / thao / dung vat pham)")
va(KSDL_C, "CHAM_RE khong con dung",
"""\tif (JxUi_CoGiaoDienTaiDiem(m_nChamX0, m_nChamY0))
\t{
\t\tm_nCham = CHAM_RE;\t// tren giao dien: chi he ra xem, khong bam chuot phai
\t\tg_DebugLog("[CHAM] giu tai %d,%d tren GIAO DIEN -> chi he ra xem (khong bam chuot phai)", m_nChamX0, m_nChamY0);
\t\treturn;
\t}
\tg_DebugLog("[CHAM] giu tai %d,%d tren BAN DO -> chuot phai", m_nChamX0, m_nChamY0);
\tm_nCham = CHAM_PHAI;""",
"""\t// CHAM_RE khong con dung: cham giu o DAU cung la chuot phai.
\t// Tren GIAO DIEN, chuot phai chinh la duong MAC / THAO / DUNG vat pham cua ban PC - bo no di thi
\t// nguoi choi khong mac duoc do. Tren BAN DO, chuot phai la danh ep / chon muc tieu.
\tg_DebugLog("[CHAM] giu tai %d,%d -> chuot phai (%s)", m_nChamX0, m_nChamY0,
\t\tJxUi_CoGiaoDienTaiDiem(m_nChamX0, m_nChamY0) ? "giao dien" : "ban do");
\tm_nCham = CHAM_PHAI;""", "cham giu = chuot phai o dau cung vay")

print("")
print("XONG dot va 13.")
