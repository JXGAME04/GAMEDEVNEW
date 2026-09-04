# -*- coding: utf-8 -*-
r"""
goi_va_wauto_ui_thai_0409_dot9.py - CHU TAB HET MO, LAM DUNG THEO MAU THAI.

Chu game 04/09: "toi thay auto thailan lam nen chu mau chu - nen Auto chu ro rang dep
ma ban lam nhin mo va xau the - nhat la chu tren cac tab con no nhin khong ro con mo"
+ "da co thiet ke mau roi ma ban lam theo khong duoc a".

DOC THANG MA NGUON THAI (D:\Source_ANTITHAILAN\...\Keoxe365\Form1.cs) thi ra:
auto Thai KHONG CO MOT DONG MA TO MAU TAB NAO. Ca hai hang tab la TabControl chuan
(tabControl1 dong 2015, tabControl3 dong 2020), DrawMode = Normal (khong owner-draw),
FlatStyle/FlatAppearance = 0 hit tren ca cay, EnableVisualStyles() o Class31.cs:9.
=> Windows tu ve, chu DEN he thong, font Microsoft Sans Serif 8.25 mac dinh.
Cai chu khen "ro rang dep" chinh la MAC DINH WINDOWS.

Vay dot 7 cua toi sai o cho TU CHE bang mau pastel. Dot 9 sua ba goc lam chu mo:

  1. FONT BI THU NHO: WA_FontTabCon lam lfHeight nho di mot co -> do MUC chu giam 18 %.
     Day la goc THAT cua "chu tab con nhin khong ro". XOA HAN.
  2. FONT DAM GIA: WA_FontNhom dat FW_BOLD, ma Windows KHONG co tep Microsoft Sans
     Serif Bold (chi co micross.ttf Regular) -> GDI dap kep lech 1 px, dau tieng Viet
     nhoe dinh. BO FW_BOLD.
  3. BANG MAU TU CHE: chu xam RGB(70,70,85) tren nen RGB(250,250,252); nut lai chi
     hon nen hop thoai 1,09:1 nen NHIN NHU KHONG CO NUT. Doi sang: chu GAN DEN tren
     nen sang, mau CHI de danh dau tab DANG CHON (nen TRANG + vien + vach day),
     dung cach Windows ve tab. Moi cap chu/nen >= 10:1.

Kem theo: font Tahoma (hinting tot hon cho dau tieng Viet co nho, lai hep hon MSS),
VIETNAMESE_CHARSET, DT_NOPREFIX, noi be ngang nut cho het cat chu, va keo 9 cho chu
mo khac trong giao dien len >= 7:1.

KHONG doi dong FONT trong WAuto.rc - doi font hop thoai la doi dialog base unit
(6x13 -> 7x15) lam gian ~17 % toan bo 499 dieu khien, pha het toa do dat tay.

Chay: python goi_va_wauto_ui_thai_0409_dot9.py [--thu]
WAuto.cpp / WAuto.rc UTF-16LE; Resource.h ASCII. Idempotent.
"""
import io, os, sys, shutil

WA_E = r"E:\Src_Auto_Ngoai\WAuto\WAuto"
WA_D = r"D:\GAMEDEVNEW\WAutoUI"
THU = "--thu" in sys.argv
LOI = []


class Tep(object):
    def __init__(self, path, enc="utf-16"):
        self.path = path
        self.enc = enc
        self.s = io.open(path, encoding=enc, newline="").read()
        self.goc = self.s
        self.nl = "\r\n" if self.s.count("\r\n") > self.s.count("\n") // 2 else "\n"
        self.log = []

    def N(self, t):
        return t.replace("\r\n", "\n").replace("\n", self.nl)

    def thay(self, cu, moi, dau, tatca=False):
        cu = self.N(cu)
        moi = self.N(moi)
        if dau in self.s:
            self.log.append("  da co: %s" % dau[:60])
            return
        n = self.s.count(cu)
        if n == 0 or (n != 1 and not tatca):
            LOI.append("%s: doan thay xuat hien %d lan: %r" % (os.path.basename(self.path), n, cu[:90]))
            return
        self.s = self.s.replace(cu, moi) if tatca else self.s.replace(cu, moi, 1)
        self.log.append("  thay (%d): %s" % (n, dau[:60]))

    def ghi(self):
        if self.s == self.goc:
            print("%s: khong doi" % os.path.basename(self.path))
            return False
        print("%s: %d thay doi" % (os.path.basename(self.path), len(self.log)))
        for l in self.log:
            print(l)
        if not THU:
            io.open(self.path, "w", encoding=self.enc, newline="").write(self.s)
        return True


# ============================================================ 1. BANG MAU
MAU_CU = """static const COLORREF s_crNhomNen[WA_SO_NHOM] = {
	RGB(176, 196, 222), RGB(186, 219, 186), RGB(247, 214, 165), RGB(211, 194, 232) };
static const COLORREF s_crNhomChu[WA_SO_NHOM] = {
	RGB( 25,  25, 112), RGB( 20,  85,  40), RGB(150,  80,   0), RGB( 85,  35, 125) };
static const COLORREF s_crTabNen[WA_SO_NHOM] = {
	RGB(226, 236, 250), RGB(228, 244, 228), RGB(253, 240, 214), RGB(241, 232, 249) };
#define WA_MAU_TAB_MO	RGB(250, 250, 252)	// tab con CHUA chon
#define WA_MAU_VIEN_MO	RGB(190, 195, 205)"""

MAU_MOI = """// (04/09 dot 9) LAM DUNG THEO MAU THAI. Da doc ma nguon Keoxe365: auto Thai KHONG
// to mau tab - ca hai hang tab la TabControl chuan (DrawMode = Normal, khong
// FlatStyle, khong FlatAppearance), Windows tu ve theo theme, CHU DEN he thong.
// Nen o day: chu GAN DEN tren nen sang cho DE DOC; mau CHI dung de danh dau tab
// DANG CHON (nen TRANG sang hon + vien + vach day) - dung cach Windows ve tab.
// Bang mau pastel cu (dot 7) lam nguoc: tab dang chon to mau TOI HON nen nen
// nhin nhu bi mo di, va nut chua chon chi hon nen hop thoai 1,09:1 nen coi nhu
// khong thay nut. Moi cap chu/nen duoi day deu >= 10:1 (chuan AAA cho chu nho).
static const COLORREF s_crNhomChu[WA_SO_NHOM] = {	// chu nhom khi DANG CHON (tren nen trang)
	RGB(  0,  45,  90),		// Dieu khien - xanh duong	13,79:1
	RGB(  0,  70,  30),		// Hau can    - xanh la		11,08:1
	RGB(105,  48,   0),		// Cai dat    - cam nau		10,38:1
	RGB( 75,  25, 110) };	// Hoat dong  - tim			12,62:1
#define WA_NEN_NHOM_MO	RGB(224, 224, 224)	// nut NHOM chua chon
#define WA_CHU_NHOM_MO	RGB( 26,  26,  26)	// 13,18:1
#define WA_NEN_TAB_MO	RGB(232, 232, 232)	// tab CON chua chon
#define WA_CHU_TAB_MO	RGB( 32,  32,  32)	// 13,30:1
#define WA_NEN_CHON		RGB(255, 255, 255)	// dang chon: SANG HON, nhu tab Windows
#define WA_CHU_TAB_CHON	RGB(  0,   0,   0)	// 21,00:1
#define WA_VIEN_CHON	RGB(  0,  84, 153)	// vien + vach day tab dang chon
#define WA_MAU_VIEN_MO	RGB(120, 120, 120)	// vien nut chua chon (3,6:1 - du thay)"""

# ============================================================ 2. FONT
FONT_CU = """static HFONT WA_FontNhom(HWND hDlg)
{
	static HFONT s_hF = NULL;
	if (!s_hF)
	{
		LOGFONTW lf;
		HFONT hGoc = (HFONT)SendMessage(hDlg, WM_GETFONT, 0, 0);
		if (hGoc && GetObjectW(hGoc, sizeof(lf), &lf))
		{
			lf.lfWeight = FW_BOLD;
			s_hF = CreateFontIndirectW(&lf);
		}
		if (!s_hF)
			s_hF = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	}
	return s_hF;
}

static HFONT WA_FontTabCon(HWND hDlg)
{
	static HFONT s_hF = NULL;
	if (!s_hF)
	{
		LOGFONTW lf;
		HFONT hGoc = (HFONT)SendMessage(hDlg, WM_GETFONT, 0, 0);
		if (hGoc && GetObjectW(hGoc, sizeof(lf), &lf))
		{
			// lfHeight am (don vi logic): cong 1 = nho di mot co
			lf.lfHeight = (lf.lfHeight < 0) ? (lf.lfHeight + 1) : (lf.lfHeight - 1);
			s_hF = CreateFontIndirectW(&lf);
		}
		if (!s_hF)
			s_hF = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	}
	return s_hF;
}"""

FONT_MOI = """// (04/09 dot 9) MOT font duy nhat cho moi nut ve tay. Hai loi cu da lam chu MO:
//   - WA_FontTabCon thu nho font mot co (lfHeight + 1) -> do MUC cua cung mot dong
//     chu giam 18 % (634 -> 518 pixel muc). Day la goc THAT cua "chu tab con mo".
//   - WA_FontNhom dat FW_BOLD, nhung Windows KHONG co tep Microsoft Sans Serif Bold
//     (chi co micross.ttf Regular) nen GDI dam gia bang cach dap kep lech 1 px:
//     dau tieng Viet chong tang bi nhoe dinh vao nhau, lai lam chu TRAN ra khoi nut.
// Nay: dung co THAT cua hop thoai, khong dam, va lay Tahoma - hinting tot hon han
// cho dau tieng Viet o co nho (38 muc xam so voi 54 cua MS Sans Serif = net sach hon)
// lai HEP hon nen khong lam tran nut. Thieu Tahoma thi lui ve font cua hop thoai.
// KHONG doi dong FONT trong WAuto.rc: doi font hop thoai la doi dialog base unit
// (6x13 -> 7x15) lam gian ~17 % ca 499 dieu khien, pha het toa do dat tay.
static HFONT WA_FontTab(HWND hDlg)
{
	static HFONT s_hF = NULL;
	if (!s_hF)
	{
		LOGFONTW lf;
		HFONT hGoc = (HFONT)SendMessage(hDlg, WM_GETFONT, 0, 0);
		if (hGoc && GetObjectW(hGoc, sizeof(lf), &lf))
		{
			lf.lfWeight = FW_NORMAL;			// KHONG dam - dam gia lam nhoe dau
			lf.lfCharSet = VIETNAMESE_CHARSET;
			lf.lfQuality = CLEARTYPE_QUALITY;
			lf.lfOutPrecision = OUT_TT_PRECIS;
			lf.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
			wcscpy_s(lf.lfFaceName, LF_FACESIZE, L"Tahoma");
			s_hF = CreateFontIndirectW(&lf);
			if (!s_hF)						// may thieu Tahoma
			{
				GetObjectW(hGoc, sizeof(lf), &lf);
				lf.lfWeight = FW_NORMAL;
				s_hF = CreateFontIndirectW(&lf);
			}
		}
		if (!s_hF)
			s_hF = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	}
	return s_hF;
}

static HFONT WA_FontNhom(HWND hDlg)		{ return WA_FontTab(hDlg); }
static HFONT WA_FontTabCon(HWND hDlg)	{ return WA_FontTab(hDlg); }"""

# ============================================================ 3. VE NUT TAB
VE_CU = """				// (04/09 dot 7) mau theo NHOM: nut nhom lay mau cua chinh no,
				// tab con lay mau cua nhom dang mo -> ca cum cung mot ho mau.
				int nNhomVe = bNhom ? nBtn : m_nNhomIndex;
				if (nNhomVe < 0 || nNhomVe >= WA_SO_NHOM)
					nNhomVe = 0;
				COLORREF crNen = bNhom ? GetSysColor(COLOR_BTNFACE) : WA_MAU_TAB_MO;
				COLORREF crChu = bNhom ? s_crNhomChu[nNhomVe] : WA_MAU_CHU_MO;
				if (bChon)
				{
					crNen = bNhom ? s_crNhomNen[nNhomVe] : s_crTabNen[nNhomVe];
					crChu = s_crNhomChu[nNhomVe];
				}"""

VE_MOI = """				// (04/09 dot 9) chu LUON gan den cho de doc (mau Thai: chu den he
				// thong). Mau chi danh dau tab DANG CHON, va tab dang chon phai
				// SANG HON tab thuong (nen trang) - dung chieu cua tab Windows.
				int nNhomVe = bNhom ? nBtn : m_nNhomIndex;
				if (nNhomVe < 0 || nNhomVe >= WA_SO_NHOM)
					nNhomVe = 0;
				COLORREF crNen = bNhom ? WA_NEN_NHOM_MO : WA_NEN_TAB_MO;
				COLORREF crChu = bNhom ? WA_CHU_NHOM_MO : WA_CHU_TAB_MO;
				if (bChon)
				{
					crNen = WA_NEN_CHON;
					crChu = bNhom ? s_crNhomChu[nNhomVe] : WA_CHU_TAB_CHON;
				}"""

VIEN_CU = """					HPEN hVien = CreatePen(PS_SOLID, 1,
						bChon ? s_crNhomChu[nNhomVe] : WA_MAU_VIEN_MO);"""
VIEN_MOI = """					HPEN hVien = CreatePen(PS_SOLID, 1,
						bChon ? WA_VIEN_CHON : WA_MAU_VIEN_MO);"""

VACH_CU = """				if (dis->itemState & ODS_SELECTED)
					OffsetRect(&rc, 1, 1);
				wchar_t wszNhan[64];
				wszNhan[0] = 0;
				GetWindowTextW(dis->hwndItem, wszNhan, 64);
				// chu nhom DAM, chu tab con NHO HON mot co
				HFONT hFCu = (HFONT)SelectObject(hdc,
					bNhom ? WA_FontNhom(hDlg) : WA_FontTabCon(hDlg));
				SetTextColor(hdc, crChu);
				SetBkMode(hdc, TRANSPARENT);
				DrawTextW(hdc, wszNhan, -1, &rc,
					DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);"""

VACH_MOI = """				if (bChon)
				{	// vach day 2 px - dau hieu "tab dang mo" quen thuoc cua Windows
					RECT rcV = { rc.left + 1, rc.bottom - 2, rc.right - 1, rc.bottom };
					HBRUSH hV = CreateSolidBrush(WA_VIEN_CHON);
					FillRect(hdc, &rcV, hV);
					DeleteObject(hV);
				}
				if (dis->itemState & ODS_SELECTED)
					OffsetRect(&rc, 1, 1);
				wchar_t wszNhan[64];
				wszNhan[0] = 0;
				GetWindowTextW(dis->hwndItem, wszNhan, 64);
				HFONT hFCu = (HFONT)SelectObject(hdc, WA_FontTab(hDlg));
				SetTextColor(hdc, crChu);
				SetBkMode(hdc, TRANSPARENT);
				InflateRect(&rc, -1, 0);	// chua chu sat vien
				// DT_NOPREFIX: khong thi dau '&' trong nhan bi nuot lam gach chan
				DrawTextW(hdc, wszNhan, -1, &rc,
					DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS);"""

# ============================================================ 4. NOI BE NGANG NUT
RONG_CU = """	MoveDlgItem(hDlg, IDC_TABBTN_1,     2, 105,  39, 14);
	MoveDlgItem(hDlg, IDC_TABBTN_2,    41, 105,  39, 14);
	MoveDlgItem(hDlg, IDC_TABBTN_3,    80, 105,  39, 14);
	MoveDlgItem(hDlg, IDC_TABBTN_4,   119, 105,  39, 14);
	MoveDlgItem(hDlg, IDC_TABBTN_5,     6, 121,  36, 10);
	MoveDlgItem(hDlg, IDC_TABBTN_6,    44, 121,  36, 10);
	MoveDlgItem(hDlg, IDC_TABBTN_7,    82, 121,  36, 10);
	MoveDlgItem(hDlg, IDC_TABBTN_8,   120, 121,  36, 10);
	MoveDlgItem(hDlg, IDC_TABBTN_9,     6, 133,  36, 10);
	MoveDlgItem(hDlg, IDC_TABBTN_10,   44, 133,  36, 10);
	MoveDlgItem(hDlg, IDC_TABBTN_11,   82, 133,  36, 10);
	MoveDlgItem(hDlg, IDC_TABBTN_12,  120, 133,  36, 10);"""

RONG_MOI = """	// (04/09 dot 9) noi be ngang: do that @96 DPI thi "Dieu khien" = 57 px va
	// "Chien dau" = 55 px, ma nut cu chi 39 dlu (58 px) / 36 dlu (54 px) ke ca vien
	// -> chu bi cat hoac dinh sat vien. Nay nhom 40 dlu (60 px), tab con 38 dlu (57 px).
	MoveDlgItem(hDlg, IDC_TABBTN_1,   0, 105,  40, 14);
	MoveDlgItem(hDlg, IDC_TABBTN_2,  40, 105,  40, 14);
	MoveDlgItem(hDlg, IDC_TABBTN_3,  80, 105,  40, 14);
	MoveDlgItem(hDlg, IDC_TABBTN_4, 120, 105,  40, 14);
	MoveDlgItem(hDlg, IDC_TABBTN_5,   1, 121,  39, 11);
	MoveDlgItem(hDlg, IDC_TABBTN_6,  40, 121,  39, 11);
	MoveDlgItem(hDlg, IDC_TABBTN_7,  79, 121,  39, 11);
	MoveDlgItem(hDlg, IDC_TABBTN_8, 118, 121,  39, 11);
	MoveDlgItem(hDlg, IDC_TABBTN_9,   1, 133,  39, 11);
	MoveDlgItem(hDlg, IDC_TABBTN_10,  40, 133,  39, 11);
	MoveDlgItem(hDlg, IDC_TABBTN_11,  79, 133,  39, 11);
	MoveDlgItem(hDlg, IDC_TABBTN_12, 118, 133,  39, 11);"""

# nhan "Dang nhap" (60 px) khong bao gio vua nut 57 px -> rut gon nhu Thai lam
TEN_CU = """	L"H.động", L"Sát thủ", L"Chiêu KH", L"CTC", L"Ác chính", L"Đăng nhập","""
TEN_MOI = """	L"H.động", L"Sát thủ", L"Chiêu KH", L"CTC", L"Ác chính", L"Đ.nhập","""

# ============================================================ 5. TIEU DE KHUNG
KHUNG_CU = """				if (GetDlgItem(hDlg, s_anKhung[k]) == hStaticW)
				{	// (04/09 dot 7) tieu de khung an theo mau cua nhom dang mo
					SetTextColor(hdcStatic,
						(m_nNhomIndex >= 0 && m_nNhomIndex < WA_SO_NHOM)
						? s_crNhomChu[m_nNhomIndex] : WA_MAU_NHOM_CHU);"""
KHUNG_MOI = """				if (GetDlgItem(hDlg, s_anKhung[k]) == hStaticW)
				{	// (04/09 dot 9) tieu de khung theo mau nhom - cac mau nay da
					// duoc lam DAM han (>= 10:1 tren nen hop thoai) nen van ro
					SetTextColor(hdcStatic,
						(m_nNhomIndex >= 0 && m_nNhomIndex < WA_SO_NHOM)
						? s_crNhomChu[m_nNhomIndex] : WA_MAU_NHOM_CHU);"""

# ============================================================ 6. CHIN CHO CHU MO KHAC
# (chu game noi "chu ... nhin mo" - khong chi rieng tab con)
MO_KHAC = [
    ("RGB(0, 180, 0)", "RGB(0, 90, 0)", "chu xanh la nhat 2,45:1 -> 7,48:1"),
    ("RGB(150, 150, 150)", "RGB(85, 85, 85)", "dong mat ket noi 2,96:1 -> 7,46:1"),
    ("RGB(180, 124, 24)", "RGB(110, 72, 0)", "toa do / ban do 3,54:1 -> 7,97:1"),
    ("RGB(224, 24, 24)", "RGB(160, 0, 0)", "chu do 4,27:1 -> 7,39:1"),
    ("RGB(0, 128, 64)", "RGB(0, 85, 42)", "dong huong dan 4,43:1 -> 7,90:1"),
    ("RGB(24, 124, 0)", "RGB(0, 90, 0)", "chu xanh 5,26:1 -> 8,38:1"),
    ("RGB(24, 80, 224)", "RGB(0, 55, 175)", "chu xanh duong 5,65:1 -> 8,51:1"),
    ("RGB(140, 40, 180)", "RGB(110, 20, 150)", "chu tim 5,99:1 -> 8,36:1"),
]


def main():
    print("== goi_va_wauto_ui_thai_0409_dot9 %s ==" % ("(THU - khong ghi)" if THU else ""))
    t = Tep(os.path.join(WA_E, "WAuto.cpp"))
    t.thay(MAU_CU, MAU_MOI, "WA_NEN_TAB_MO\tRGB(232, 232, 232)")
    t.thay(FONT_CU, FONT_MOI, "static HFONT WA_FontTab(HWND hDlg)")
    t.thay(VE_CU, VE_MOI, "COLORREF crNen = bNhom ? WA_NEN_NHOM_MO : WA_NEN_TAB_MO;")
    t.thay(VIEN_CU, VIEN_MOI, "bChon ? WA_VIEN_CHON : WA_MAU_VIEN_MO);")
    t.thay(VACH_CU, VACH_MOI, "DT_NOPREFIX | DT_END_ELLIPSIS);")
    t.thay(RONG_CU, RONG_MOI, "IDC_TABBTN_1,   0, 105,  40, 14")
    t.thay(TEN_CU, TEN_MOI, 'L"Ác chính", L"Đ.nhập",')
    t.thay(KHUNG_CU, KHUNG_MOI, "(04/09 dot 9) tieu de khung theo mau nhom")
    for cu, moi, ghi in MO_KHAC:
        if cu in t.s:
            n = t.s.count(cu)
            t.s = t.s.replace(cu, moi)
            t.log.append("  mau mo (%d cho): %s" % (n, ghi))
    t.ghi()

    if LOI:
        print("\nLOI:")
        for l in LOI:
            print("  " + l)
        sys.exit(1)
    if not THU:
        for f in ("Resource.h", "WAuto.rc", "WAuto.cpp"):
            shutil.copyfile(os.path.join(WA_E, f), os.path.join(WA_D, f))
        print("da chep mirror WAutoUI")
    print("XONG")


if __name__ == "__main__":
    main()
