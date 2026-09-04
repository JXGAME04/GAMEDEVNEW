# -*- coding: utf-8 -*-
"""
goi_va_wauto_ui_thai_0409_dot7.py - DOT 7 (3 viec chu game bao 04/09):

  1. O NHAP LOI CHAT bi HAI DUONG NGANG: vien duoi o EDITTEXT chat (day y=246) TRUNG
     duong ke IDC_SEP_0B (y=246), lai them IDC_SEP_0C (y=248, keo len tu dot 6) ngay
     duoi -> nhin thanh 2 vach. Sua: SEP_0B xuong 250, BO HAN SEP_0C khoi tab 0,
     khoi "nhe may" xuong 256/268, s_aTabDay[0] 280 -> 286.
  2. MAU TAB CON: moi NHOM mot ho mau rieng (xanh duong / xanh la / cam / tim);
     nut nhom, tab con dang chon va tieu de khung deu an theo mau cua nhom do.
  3. FONT NHU AUTO THAI: doi ca 4 hop thoai tu "Segoe UI" 9 sang
     "Microsoft Sans Serif" 8 (Thai: Microsoft Sans Serif 8.25, AutoScaleDimensions 6x13).
     Moi toa do la DIALOG UNIT nen ca cua so tu co lai theo font - be ngang ~280 -> ~240 px
     (Thai 264 px), chieu cao giam ~13 %.

Chay: python goi_va_wauto_ui_thai_0409_dot7.py [--thu]
WAuto.cpp / WAuto.rc UTF-16LE; Resource.h ASCII. Idempotent.
"""
import io, os, sys, shutil

WA_E = r"E:\Src_Auto_Ngoai\WAuto\WAuto"
WA_D = r"D:\GAMEDEVNEW\WAutoUI"
THU = "--thu" in sys.argv
LOI = []


class Tep(object):
    def __init__(self, path, enc="utf-16"):
        self.path = path; self.enc = enc
        self.s = io.open(path, encoding=enc, newline="").read()
        self.goc = self.s
        self.nl = "\r\n" if self.s.count("\r\n") > self.s.count("\n") // 2 else "\n"
        self.log = []

    def N(self, t):
        return t.replace("\r\n", "\n").replace("\n", self.nl)

    def thay(self, cu, moi, dau, tatca=False):
        cu = self.N(cu); moi = self.N(moi)
        if dau in self.s:
            self.log.append("  da co: %s" % dau[:58]); return
        n = self.s.count(cu)
        if n == 0 or (n != 1 and not tatca):
            LOI.append("%s: doan thay xuat hien %d lan: %r" % (os.path.basename(self.path), n, cu[:80])); return
        self.s = self.s.replace(cu, moi) if tatca else self.s.replace(cu, moi, 1)
        self.log.append("  thay (%d): %s" % (n, dau[:58]))

    def them_truoc_dong(self, chua, moi, dau):
        if dau in self.s:
            self.log.append("  da co: %s" % dau[:58]); return
        L = self.s.split(self.nl)
        a = [i for i, l in enumerate(L) if chua in l]
        if len(a) != 1:
            LOI.append("%s: dong %r xuat hien %d lan" % (os.path.basename(self.path), chua, len(a))); return
        self.s = self.nl.join(L[:a[0]] + self.N(moi).split(self.nl) + L[a[0]:])
        self.log.append("  them truoc dong: %s" % dau[:58])

    def ghi(self):
        if self.s == self.goc:
            print("%s: khong doi" % os.path.basename(self.path)); return False
        print("%s: %d thay doi" % (os.path.basename(self.path), len(self.log)))
        for l in self.log: print(l)
        if not THU:
            io.open(self.path, "w", encoding=self.enc, newline="").write(self.s)
        return True


MAU = """// (04/09 dot 7) MOI NHOM MOT HO MAU - nut nhom, tab con dang chon va tieu de khung
// deu lay mau cua nhom dang mo, cho de phan biet (auto Thai chi co xam, day lam dep hon).
//   0 Dieu khien = xanh duong | 1 Hau can = xanh la | 2 Cai dat = cam | 3 Hoat dong = tim
static const COLORREF s_crNhomNen[WA_SO_NHOM] = {
	RGB(176, 196, 222), RGB(186, 219, 186), RGB(247, 214, 165), RGB(211, 194, 232) };
static const COLORREF s_crNhomChu[WA_SO_NHOM] = {
	RGB( 25,  25, 112), RGB( 20,  85,  40), RGB(150,  80,   0), RGB( 85,  35, 125) };
static const COLORREF s_crTabNen[WA_SO_NHOM] = {
	RGB(226, 236, 250), RGB(228, 244, 228), RGB(253, 240, 214), RGB(241, 232, 249) };
#define WA_MAU_TAB_MO	RGB(250, 250, 252)	// tab con CHUA chon
#define WA_MAU_VIEN_MO	RGB(190, 195, 205)

"""

DRAW_CU = """				COLORREF crNen = GetSysColor(COLOR_BTNFACE);
				COLORREF crChu = WA_MAU_CHU_MO;
				if (bChon)
				{
					crNen = bNhom ? WA_MAU_NHOM_NEN : WA_MAU_TAB_NEN;
					crChu = bNhom ? WA_MAU_NHOM_CHU : WA_MAU_TAB_CHU;
				}
				else if (bNhom)
					crChu = WA_MAU_NHOM_CHU;"""
DRAW_MOI = """				// (04/09 dot 7) mau theo NHOM: nut nhom lay mau cua chinh no,
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

DRAW_VIEN_CU = """					HPEN hVien = CreatePen(PS_SOLID, 1,
						bChon ? WA_MAU_NHOM_CHU : RGB(168, 172, 182));"""
DRAW_VIEN_MOI = """					HPEN hVien = CreatePen(PS_SOLID, 1,
						bChon ? s_crNhomChu[nNhomVe] : WA_MAU_VIEN_MO);"""

KHUNG_CU = """				if (GetDlgItem(hDlg, s_anKhung[k]) == hStaticW)
				{
					SetTextColor(hdcStatic, WA_MAU_NHOM_CHU);"""
KHUNG_MOI = """				if (GetDlgItem(hDlg, s_anKhung[k]) == hStaticW)
				{	// (04/09 dot 7) tieu de khung an theo mau cua nhom dang mo
					SetTextColor(hdcStatic,
						(m_nNhomIndex >= 0 && m_nNhomIndex < WA_SO_NHOM)
						? s_crNhomChu[m_nNhomIndex] : WA_MAU_NHOM_CHU);"""


def main():
    print("== goi_va_wauto_ui_thai_0409_dot7 %s ==" % ("(THU - khong ghi)" if THU else ""))

    # ---------------- WAuto.rc ----------------
    rc = Tep(os.path.join(WA_E, "WAuto.rc"))
    # 1a. FONT nhu auto Thai (ca 4 hop thoai)
    rc.thay('FONT 9, "Segoe UI"\n', 'FONT 8, "Microsoft Sans Serif"\n', 'FONT 8, "Microsoft Sans Serif"', tatca=True)
    rc.thay('FONT 9, "Segoe UI", 400, 0, 0xa3', 'FONT 8, "Microsoft Sans Serif", 400, 0, 0xa3',
            'FONT 8, "Microsoft Sans Serif", 400, 0, 0xa3')
    # 1b. het HAI DUONG NGANG o o nhap chat
    rc.thay('CONTROL "", IDC_SEP_0B, "Static", SS_ETCHEDHORZ, 6, 246, 148, 1',
            'CONTROL "", IDC_SEP_0B, "Static", SS_ETCHEDHORZ, 6, 250, 148, 1',
            'IDC_SEP_0B, "Static", SS_ETCHEDHORZ, 6, 250')
    rc.thay('IDC_CHECKBOX_0_NTS, "Button", BS_AUTOCHECKBOX | WS_TABSTOP, 4, 252, 152, 10',
            'IDC_CHECKBOX_0_NTS, "Button", BS_AUTOCHECKBOX | WS_TABSTOP, 4, 256, 152, 10',
            'IDC_CHECKBOX_0_NTS, "Button", BS_AUTOCHECKBOX | WS_TABSTOP, 4, 256')
    rc.thay('IDC_CHECKBOX_0_MIS, "Button", BS_AUTOCHECKBOX | WS_TABSTOP, 4, 264, 122, 10',
            'IDC_CHECKBOX_0_MIS, "Button", BS_AUTOCHECKBOX | WS_TABSTOP, 4, 268, 122, 10',
            'IDC_CHECKBOX_0_MIS, "Button", BS_AUTOCHECKBOX | WS_TABSTOP, 4, 268')
    rc.thay("EDITTEXT IDC_EDITOR_0_MIS, 128, 264, 28, 10", "EDITTEXT IDC_EDITOR_0_MIS, 128, 268, 28, 10",
            "IDC_EDITOR_0_MIS, 128, 268")
    rc.ghi()

    # ---------------- WAuto.cpp ----------------
    t = Tep(os.path.join(WA_E, "WAuto.cpp"))
    # bang mau theo nhom (dat ngay sau cac #define mau cu)
    t.them_truoc_dong("static HFONT WA_FontNhom(HWND hDlg)", MAU, "s_crNhomNen[WA_SO_NHOM]")
    t.thay(DRAW_CU, DRAW_MOI, "mau theo NHOM: nut nhom lay mau cua chinh no")
    t.thay(DRAW_VIEN_CU, DRAW_VIEN_MOI, "bChon ? s_crNhomChu[nNhomVe] : WA_MAU_VIEN_MO")
    t.thay(KHUNG_CU, KHUNG_MOI, "tieu de khung an theo mau cua nhom dang mo")
    # bo IDC_SEP_0C khoi tab 0 (khong con ke doi)
    t.thay("\t\tfor(i=IDC_SEP_0C;i<=IDC_EDITOR_0_MIS;++i)\t// [REP3 03/09] khoi hien thi khi dong nguoi (dai 628..631)",
           "\t\t// (04/09 dot 7) BO IDC_SEP_0C: no nam ngay duoi IDC_SEP_0B nen nhin thanh HAI vach.\n"
           "\t\tfor(i=IDC_CHECKBOX_0_NTS;i<=IDC_EDITOR_0_MIS;++i)\t// [REP3] khoi hien thi khi dong nguoi (629..631)",
           "for(i=IDC_CHECKBOX_0_NTS;i<=IDC_EDITOR_0_MIS;++i)")
    # tab 0 cao them 6 don vi cho khoi nhe may vua chuyen xuong
    t.thay("280, 279, 279, 304, 282, 260, 266, 303, 294, 268, 326, 328, 225, 335, 270, 208, 224",
           "286, 279, 279, 304, 282, 260, 266, 303, 294, 268, 326, 328, 225, 335, 270, 208, 224",
           "286, 279, 279, 304")
    t.ghi()

    if LOI:
        print("LOI:")
        for l in LOI: print("  " + l)
        sys.exit(1)
    if not THU:
        for f in ("Resource.h", "WAuto.rc", "WAuto.cpp"):
            shutil.copyfile(os.path.join(WA_E, f), os.path.join(WA_D, f))
        print("da chep mirror WAutoUI")
    print("XONG")


if __name__ == "__main__":
    main()
