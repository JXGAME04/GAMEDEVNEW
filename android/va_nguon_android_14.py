# -*- coding: utf-8 -*-
# [ANDROID 09/09] Dot va 14: TU CAN CHINH GIAO DIEN THEO MAN HINH (he neo FitFlags).
#
# Chu yeu cau: "phai co che do tu can chinh cac spr de man hinh dien thoai nao deu can chinh duoc".
#
# Ban JX1 Mobile cua chu (D:\USVOLAM, "dot UI-1/UI-2") da co san he nay trong KWndWindow va no
# duoc THIET KE dung cho viec do. Ban nay mang y nguyen sang, chi doi ten bien man hinh
# (g_SCREEN_WIDTH -> SCREEN_WIDTH) va RAO TOAN BO trong #ifdef JX_ANDROID de ban PC khong doi gi.
#
# Cach lam: moi tep .ini giao dien duoc ve theo khung chuan 1024x768. Khi man hinh that khac di,
# moi cua so goc duoc dat lai mot lan:
#   - mac dinh   : dich ca khung 1024 vao GIUA man hinh (giu nguyen bo cuc tuong doi da chinh)
#   - FIT_LEFT/TOP    : giu nguyen goc (bam le trai / tren)
#   - FIT_RIGHT/BOTTOM: bam le phai / duoi
#   - FIT_HCENTER/VCENTER: can giua that
#   - LEFT|RIGHT (hoac TOP|BOTTOM): keo cang cho day
#   - FIT_NOFIT  : cua so tu dat cho, khong dung toi
# Moi cua so chi duoc can MOT LAN (co m_bNeedFit dat trong KWndWindow::Init).
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


H = "Sources/S3Client/Ui/Elem/WndWindow.h"
C = "Sources/S3Client/Ui/Elem/WndWindow.cpp"
W = "Sources/S3Client/Ui/Elem/Wnds.cpp"

# ---------------------------------------------------------------------------
# 1. WndWindow.h: enum + thanh vien + ham
# ---------------------------------------------------------------------------
print("1. WndWindow.h: khai bao he neo")
va(H, "FIT_HCENTER",
"""class KIniFile;""",
"""#ifdef JX_ANDROID
// [ANDROID 09/09 NEO] Chinh sach neo cua so khi man hinh khac khung ve chuan 1024x768.
// Mang tu ban JX1 Mobile cua chu (USVOLAM, "dot UI-1/UI-2").
enum FitFlags
{
	FIT_NONE    = 0,
	FIT_LEFT    = 1 << 0,	// giu nguyen Left (bam le trai)
	FIT_RIGHT   = 1 << 1,	// bam le phai
	FIT_HCENTER = 1 << 2,	// can giua ngang
	FIT_TOP     = 1 << 3,	// giu nguyen Top (bam le tren)
	FIT_BOTTOM  = 1 << 4,	// bam le duoi
	FIT_VCENTER = 1 << 5,	// can giua doc
	FIT_NOFIT   = 1 << 7,	// cua so tu dat cho, dung dung toi
};
#endif

class KIniFile;""", "WndWindow.h enum FitFlags")

va(H, "m_FitFlags",
"""	unsigned int m_Style;""",
"""	unsigned int m_Style;
#ifdef JX_ANDROID
	unsigned char	m_FitFlags;		// [ANDROID 09/09 NEO] chinh sach neo (0 = mac dinh: dich vao giua)
	unsigned char	m_bNeedFit;		// [ANDROID 09/09 NEO] Init vua dat lai khung -> con mot luot can
#endif""", "WndWindow.h thanh vien")

va(H, "SetFitFlags",
"""	KWndWindow*		GetNextWnd() const { return m_pNextWnd; }""",
"""#ifdef JX_ANDROID
	// [ANDROID 09/09 NEO] dat chinh sach neo cho cua so nay (xem enum FitFlags)
	void			SetFitFlags(unsigned char nFlags) { m_FitFlags = nFlags; }
	// [ANDROID 09/09 NEO] dat lai mot cua so GOC tu khung ve chuan 1024x768 sang man hinh that
	virtual void	FitToScreen();
	void			ComputeFit(int nRefL, int nRefT, int* pOutL, int* pOutT);
#endif
	KWndWindow*		GetNextWnd() const { return m_pNextWnd; }""", "WndWindow.h ham")

# ---------------------------------------------------------------------------
# 2. WndWindow.cpp: khoi tao + than ham + danh dau trong Init
# ---------------------------------------------------------------------------
print("2. WndWindow.cpp: than he neo")
va(C, "m_FitFlags = 0;",
"""	m_Style			= WND_S_VISIBLE;""",
"""	m_Style			= WND_S_VISIBLE;
#ifdef JX_ANDROID
	m_FitFlags = 0;		// [ANDROID 09/09 NEO]
	m_bNeedFit = 0;
#endif""", "WndWindow.cpp ham dung")

THAN = r'''
#ifdef JX_ANDROID
//--------------------------------------------------------------------------
// [ANDROID 09/09 NEO] TU CAN CHINH GIAO DIEN THEO MAN HINH
//
// Moi tep .ini giao dien duoc ve theo KHUNG CHUAN 1024x768. Tren dien thoai khung ve la thu khac
// (vi du 1040x604, 1188x616, 1370x616) nen cua so nao neo goc tren-trai se nam sai cho, con cua so
// le phai / le duoi thi hoac loi ra ngoai hoac de ho mot mang.
//
// Cach lam (mang tu ban JX1 Mobile cua chu, "dot UI-1/UI-2"):
//   mac dinh              : dich ca khung 1024 vao GIUA man hinh -> giu nguyen bo cuc tuong doi
//   FIT_LEFT / FIT_TOP    : giu nguyen goc (bam le trai / le tren)
//   FIT_RIGHT / FIT_BOTTOM: bam le phai / le duoi
//   FIT_HCENTER/VCENTER   : can giua that (theo be ngang thuc cua cua so)
//   LEFT|RIGHT, TOP|BOTTOM: keo cang cho day man hinh
//   FIT_NOFIT             : cua so tu dat cho (hop thoai tu can giua) - khong dung toi
//
// Moi cua so chi can MOT LAN: KWndWindow::Init dat m_bNeedFit, Wnd_RenderWindows quet mot luot.
//--------------------------------------------------------------------------
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

void KWndWindow::ComputeFit(int nRefL, int nRefT, int* pOutL, int* pOutT)
{
	const int nRefW = 1024;		// khung ve chuan ma cac tep .ini duoc ve theo
	const int nRefH = 768;
	int nFlags = m_FitFlags & ~FIT_NOFIT;
	int nDX = SCREEN_WIDTH - nRefW;
	int nDY = SCREEN_HEIGHT - nRefH;

	int eW = m_Width, eH = m_Height;
	if ((m_Style & WND_S_SIZE_WITH_ALL_CHILD) || m_Width <= 0 || m_Height <= 0)
	{
		RECT rcAll;		// cua so "rong" (chi chua o con) phai lay be ngang that cua dam con
		GetAllChildLayoutRect(&rcAll);
		if (rcAll.right > rcAll.left) eW = rcAll.right - rcAll.left;
		if (rcAll.bottom > rcAll.top) eH = rcAll.bottom - rcAll.top;
	}

	// Khong dat FitFlags thi TU SUY RA neo theo cho cua so nam trong khung chuan:
	//   nam o mot phan ba dau  -> bam le tren / le trai
	//   nam o mot phan ba cuoi -> bam le duoi / le phai
	//   nam o giua             -> dich vao giua
	// KHAC ban USVOLAM (ho mac dinh luon "dich ca khung vao giua"): ho chi bat he neo khi man hinh
	// RONG VA CAO hon khung chuan. Man hinh dien thoai thi nguoc lai - rong hon nhung THAP hon
	// nhieu (604 / 616 so voi 768), dich vao giua se keo thanh cong cu o day man hinh LEN ~82 diem
	// anh (da nhin thay tan mat). Tu suy ra neo thi thanh tren o tren, thanh duoi o duoi, hop thoai
	// van o giua - dung cho moi co man hinh ma khong phai danh dau tung cua so.
	int nGiuaX = nRefL + (eW > 0 ? eW / 2 : 0);
	int nGiuaY = nRefT + (eH > 0 ? eH / 2 : 0);

	int nNewL;
	if ((nFlags & FIT_LEFT) && (nFlags & FIT_RIGHT)) nNewL = nRefL;			// keo cang: giu goc
	else if (nFlags & FIT_RIGHT)   nNewL = nRefL + nDX;						// bam le phai
	else if (nFlags & FIT_LEFT)    nNewL = nRefL;							// bam le trai
	else if (nFlags & FIT_HCENTER) nNewL = (SCREEN_WIDTH - eW) / 2;			// can giua that
	else if (nGiuaX < nRefW / 3)   nNewL = nRefL;							// tu suy: nua trai -> giu goc
	else if (nGiuaX > nRefW * 2 / 3) nNewL = nRefL + nDX;					// tu suy: nua phai -> bam le phai
	else                           nNewL = nRefL + nDX / 2;					// tu suy: giua -> dich vao giua

	int nNewT;
	if ((nFlags & FIT_TOP) && (nFlags & FIT_BOTTOM)) nNewT = nRefT;
	else if (nFlags & FIT_BOTTOM)  nNewT = nRefT + nDY;						// bam le duoi
	else if (nFlags & FIT_TOP)     nNewT = nRefT;							// bam le tren
	else if (nFlags & FIT_VCENTER) nNewT = (SCREEN_HEIGHT - eH) / 2;
	else if (nGiuaY < nRefH / 3)   nNewT = nRefT;							// tu suy: phan tren -> giu goc
	else if (nGiuaY > nRefH * 2 / 3) nNewT = nRefT + nDY;					// tu suy: phan duoi -> bam le duoi
	else                           nNewT = nRefT + nDY / 2;

	if (nNewL < 0) nNewL = 0;
	if (nNewT < 0) nNewT = 0;
	*pOutL = nNewL;
	*pOutT = nNewT;
}

void KWndWindow::FitToScreen()
{
	if (!m_bNeedFit)
		return;
	m_bNeedFit = 0;
	if (m_pParentWnd != NULL)		// chi cua so GOC (phong xa)
		return;
	// CHI neo cua so nao TU DANG KY (SetFitFlags). Da thu ap cho MOI cua so mot luot (nhu ban
	// USVOLAM) va DO thay HONG: ban nay da tu chinh san nhieu cua so theo SCREEN_WIDTH/HEIGHT ngay
	// trong ma (vi du UiPlayerBar co nhanh rieng cho 1024 va goi SetSize(SCREEN_WIDTH, ...)), nen neo
	// lai tu khung chuan 1024x768 la CHINH HAI LAN: thanh cong cu duoi day bi keo len ~82 diem anh va
	// khung trang tri lac cho. Vi vay de opt-in: cua so moi cua ban mobile goi SetFitFlags, cua so cu
	// giu nguyen duong da chay.
	if (m_FitFlags == 0)
		return;
	if (m_FitFlags & FIT_NOFIT)		// cua so tu dat cho
		return;
	if (SCREEN_WIDTH == 1024 && SCREEN_HEIGHT == 768)	// dung khung chuan: khong phai dich gi
		return;

	int nFlags = m_FitFlags & ~FIT_NOFIT;
	if (((nFlags & FIT_LEFT) && (nFlags & FIT_RIGHT)) ||
		((nFlags & FIT_TOP) && (nFlags & FIT_BOTTOM)))
	{
		int nNewW = ((nFlags & FIT_LEFT) && (nFlags & FIT_RIGHT)) ? SCREEN_WIDTH - m_Left : m_Width;
		int nNewH = ((nFlags & FIT_TOP) && (nFlags & FIT_BOTTOM)) ? SCREEN_HEIGHT - m_Top : m_Height;
		if (nNewW != m_Width || nNewH != m_Height)
			SetSize(nNewW, nNewH);
	}

	int nNewL, nNewT;
	ComputeFit(m_Left, m_Top, &nNewL, &nNewT);
	if (nNewL != m_Left || nNewT != m_Top)
		SetPosition(nNewL, nNewT);
}
#endif	// JX_ANDROID

'''
s = doc(C)
crlf = ("\r\n" in s)
if "KWndWindow::FitToScreen" in s:
    print("  bo qua (da co): than he neo")
else:
    neo = nl("void KWndWindow::BringToTop()", crlf)
    if s.count(neo) != 1:
        print("  !! khong tim thay BringToTop de dat truoc")
        sys.exit(1)
    i = s.find(neo)
    ghi(C, s[:i] + nl(THAN, crlf) + s[i:])
    print("  va xong: than he neo")

print("3. WndWindow.cpp: Init danh dau can can lai")
s = doc(C)
crlf = ("\r\n" in s)
if "m_bNeedFit = 1;" in s:
    print("  bo qua (da co): danh dau trong Init")
else:
    neo = nl('\t\tpIniFile->GetInteger(pSection, "DummyWnd", 0, &nValue1);', crlf)
    if s.count(neo) != 1:
        print("  !! khong tim thay doan DummyWnd trong Init")
        sys.exit(1)
    them = nl('#ifdef JX_ANDROID\n\t\tm_bNeedFit = 1;\t// [ANDROID 09/09 NEO] khung vua dat tu ini -> con mot luot can lai\n#endif\n', crlf)
    # dat NGAY TRUOC "return true;" cuoi khoi Init: tim tu vi tri DummyWnd tro di
    i = s.find(neo)
    j = s.find(nl("\t\treturn true;", crlf), i)
    if j < 0:
        print("  !! khong tim thay return true cua Init")
        sys.exit(1)
    ghi(C, s[:j] + them + s[j:])
    print("  va xong: danh dau trong Init")

# ---------------------------------------------------------------------------
# 4. Wnds.cpp: quet mot luot truoc khi ve
# ---------------------------------------------------------------------------
print("4. Wnds.cpp: quet can chinh truoc khi ve")
va(W, "FitToScreen();",
"""	if (s_WndStation.pGameSpaceWnd && s_WndStation.bPaintGameSpace)
		s_WndStation.pGameSpaceWnd->Paint();
	s_WndStation.LowLayerRoot.Paint();""",
"""	if (s_WndStation.pGameSpaceWnd && s_WndStation.bPaintGameSpace)
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
	s_WndStation.LowLayerRoot.Paint();""", "Wnds.cpp quet can chinh")

print("")
print("XONG dot va 14.")
