# -*- coding: utf-8 -*-
"""
goi_va_wauto_ui_thai_0409.py - dung lai giao dien WAuto theo auto Thai Lan (vlhkmp).
Dac ta: workflow 04/09 "DANG THAI - RUOT WAUTO" (17 tac tu) + vong phan bien.

  DOT 1: doi 4 nhom cap 1 + thu tu tab con theo kieu Thai; rut ten tab cho vua nut;
         mo mac dinh vao tab "Chien dau" (Thai mo vao "Bang 1").
  DOT 2: danh sach nhan vat 5 cot "Nhan vat | SL | NL | Viec | Ban do"
         (Thai: "Ten nhan vat | Doing | Ban do").

Chay:  python goi_va_wauto_ui_thai_0409.py [--thu] [--dot 1,2]
WAuto.cpp / WAuto.rc la UTF-16LE (io.open encoding='utf-16', newline=''); Resource.h ASCII.
Idempotent: ap lai nhieu lan khong hong (moi hunk co dau nhan dien).
BAY: nhieu dong "trong" trong WAuto.cpp that ra la "\t\t" -> KHONG dung khoi cung lam neo,
     dung neo THEO DONG (chua chuoi) nhu duoi day.
"""
import io, os, sys, shutil

WA_E = r"E:\Src_Auto_Ngoai\WAuto\WAuto"
WA_D = r"D:\GAMEDEVNEW\WAutoUI"
THU = "--thu" in sys.argv
DOT = "1,2"
for i, a in enumerate(sys.argv):
    if a == "--dot" and i + 1 < len(sys.argv):
        DOT = sys.argv[i + 1]
DOTS = set(int(x) for x in DOT.split(","))
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

    def _lines(self):
        return self.s.split(self.nl)

    def _set(self, lines):
        self.s = self.nl.join(lines)

    def thay(self, cu, moi, dau):
        cu = self.N(cu); moi = self.N(moi)
        if dau in self.s:
            self.log.append("  da co: %s" % dau); return
        n = self.s.count(cu)
        if n != 1:
            LOI.append("%s: doan thay xuat hien %d lan: %r" % (os.path.basename(self.path), n, cu[:80])); return
        self.s = self.s.replace(cu, moi, 1)
        self.log.append("  thay: %s" % dau)

    def _mot_dong(self, chua, tu=0):
        idx = [i for i, l in enumerate(self._lines()) if i >= tu and chua in l]
        return idx

    def thay_doan(self, dong_dau, dong_cuoi, moi, dau):
        """Thay tu dong chua 'dong_dau' den dong chua 'dong_cuoi' (ke ca hai dong do)."""
        if dau in self.s:
            self.log.append("  da co: %s" % dau); return
        L = self._lines()
        a = [i for i, l in enumerate(L) if dong_dau in l]
        if len(a) != 1:
            LOI.append("%s: dau doan %r xuat hien %d lan" % (os.path.basename(self.path), dong_dau, len(a))); return
        b = [i for i, l in enumerate(L) if i > a[0] and dong_cuoi in l]
        if not b:
            LOI.append("%s: khong thay cuoi doan %r" % (os.path.basename(self.path), dong_cuoi)); return
        moi_l = self.N(moi).split(self.nl)
        self._set(L[:a[0]] + moi_l + L[b[0] + 1:])
        self.log.append("  thay doan: %s (%d dong -> %d dong)" % (dau, b[0] - a[0] + 1, len(moi_l)))

    def them_sau_dong(self, chua, moi, dau):
        if dau in self.s:
            self.log.append("  da co: %s" % dau); return
        L = self._lines()
        a = [i for i, l in enumerate(L) if chua in l]
        if len(a) != 1:
            LOI.append("%s: dong %r xuat hien %d lan" % (os.path.basename(self.path), chua, len(a))); return
        moi_l = self.N(moi).split(self.nl)
        self._set(L[:a[0] + 1] + moi_l + L[a[0] + 1:])
        self.log.append("  them sau dong: %s" % dau)

    def ghi(self):
        if self.s == self.goc:
            print("%s: khong doi" % os.path.basename(self.path)); return False
        print("%s: %d thay doi" % (os.path.basename(self.path), len(self.log)))
        for l in self.log: print(l)
        if not THU:
            io.open(self.path, "w", encoding=self.enc, newline="").write(self.s)
        return True


# =====================================================================
# DOT 1 - dai tab kieu Thai
# =====================================================================
NHOM_MOI = """// (04/09) XEP LAI THEO AUTO THAI (vlhkmp): 4 nhom cap 1 dung thu tu
// "Dieu khien | Hau can | Cai dat | Hoat dong" (Thai: Dieu khien | Hau can | Cai game | Phu tro),
// tab con xep theo tan suat dung. KHONG doi mot ID hay toa do nao - chi doi bang nay.
static const WANhomTab s_aNhomTab[WA_SO_NHOM] = {
\t{ L"Điều khiển", { 2, 13, 7, 15, 3, 4 }, 6 },\t// Chien dau, Chieu KH, PK, Ac chinh, Di chuyen, Nhat do
\t{ L"Hậu cần",   { 5, 1, 6 },            3 },\t// Hau can, Phuc hoi, To doi
\t{ L"Cài đặt",   { 0 },                  1 },\t// Co ban (+ tab Dang nhap o dot 6)
\t{ L"Hoạt động", { 8, 9, 14, 10, 12, 11 }, 6 },\t// Da Tau, TK, CTC, Lien dau, Sat thu, H.dong
};"""


def dot1(t):
    t.thay_doan("static const WANhomTab s_aNhomTab[WA_SO_NHOM] = {", "};", NHOM_MOI, "XEP LAI THEO AUTO THAI")
    # ten tab: rut gon cho vua nut 36 don vi + sua dau "Ac chinh" -> "Ac chinh" co dau
    t.thay('L"Dã Tẩu", L"Tống Kim", L"Liên đấu",', 'L"Dã Tẩu", L"TK", L"Liên đấu",', 'L"Dã Tẩu", L"TK",')
    t.thay('L"Chiêu KH", L"Công Thành", L"Ac chính",', 'L"Chiêu KH", L"CTC", L"Ác chính",', 'L"CTC", L"Ác chính",')
    t.thay("\t\tShowTab(hDlg, 0);",
           "\t\tShowTab(hDlg, 2);\t// (04/09) mo mac dinh vao tab Chien dau (Thai mo vao Bang 1)",
           "ShowTab(hDlg, 2);")


# =====================================================================
# DOT 2 - danh sach nhan vat 5 cot
# =====================================================================
COT_MOI = """\t\t// (04/09) 5 COT theo auto Thai ("Ten nhan vat | Doing | Ban do"): nhin mot cai
\t\t// biet ac nao dang lam gi, o map nao - khong phai bam tung dong xem.
\t\t// Ty le 84/36/36/52/con lai tren 256 phan be ngang that (da tru thanh cuon doc).
\t\tint width0 = total * 84 / 256;
\t\tint width1 = total * 36 / 256;
\t\tint width2 = total * 36 / 256;
\t\tint width3 = total * 52 / 256;
\t\tint width4 = total - width0 - width1 - width2 - width3;
\t\tif (width4 < 20)
\t\t\twidth4 = 20;
\t\t
\t\tLVCOLUMNW col{};
\t\tcol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
\t\tcol.fmt  = LVCFMT_LEFT | LVCFMT_FIXED_WIDTH;  // Key: FIXED_WIDTH block resize
\t\tcol.pszText = (LPWSTR)L"Nhân vật";
\t\tcol.cx   = width0;
\t\tListView_InsertColumn(hCtrl, 0, &col);
\t\t
\t\tcol.pszText = (LPWSTR)L"SL";
\t\tcol.cx   = width1;
\t\tListView_InsertColumn(hCtrl, 1, &col);
\t\t
\t\tcol.pszText = (LPWSTR)L"NL";
\t\tcol.cx   = width2;
\t\tListView_InsertColumn(hCtrl, 2, &col);
\t\t
\t\tcol.pszText = (LPWSTR)L"Việc";
\t\tcol.cx   = width3;
\t\tListView_InsertColumn(hCtrl, 3, &col);
\t\t
\t\tcol.pszText = (LPWSTR)L"Bản đồ";
\t\tcol.cx   = width4;
\t\tListView_InsertColumn(hCtrl, 4, &col);"""

MAP_MOI = """\t\t\t\t{\t// (04/09) cot 4 "Ban do" cua danh sach nhan vat. szMap la TCVN3
\t\t\t\t\t// -> BAT BUOC qua g_Convert2UCEChar roi delete[]; cam dua thang char* vao ListView.
\t\t\t\t\twchar_t* pMapCot = g_Convert2UCEChar(pInfo->szMap, strlen(pInfo->szMap));
\t\t\t\t\tListView_SetItemText(hCtrl, foundIndex, 4, pMapCot ? (LPWSTR)pMapCot : (LPWSTR)L"—");
\t\t\t\t\tdelete [] pMapCot;
\t\t\t\t\tpMapCot = nullptr;
\t\t\t\t}"""

HD_MOI = """\t\t\tif(strcmp(szHDCu, gnode.player.szHoatDong))
\t\t\t{\t// (04/09) cot 3 "Viec" - chi ve lai khi that su doi chu (khoi nhay chu).
\t\t\t\t// szHoatDong la TCVN3 -> phai qua g_Convert2UCEChar roi delete[].
\t\t\t\tif(gnode.player.szHoatDong[0])
\t\t\t\t{
\t\t\t\t\twchar_t* pHDCot = g_Convert2UCEChar(gnode.player.szHoatDong, strlen(gnode.player.szHoatDong));
\t\t\t\t\tListView_SetItemText(hCtrl, foundIndex, 3, pHDCot ? (LPWSTR)pHDCot : (LPWSTR)L"—");
\t\t\t\t\tdelete [] pHDCot;
\t\t\t\t\tpHDCot = nullptr;
\t\t\t\t}
\t\t\t\telse
\t\t\t\t\tListView_SetItemText(hCtrl, foundIndex, 3, (LPWSTR)L"—");
\t\t\t}"""


def dot2(t):
    t.thay_doan("int width0 = total / 2;", "ListView_InsertColumn(hCtrl, 2, &col);", COT_MOI, "5 COT theo auto Thai")
    t.them_sau_dong('ListView_SetItemText(hCtrl, nCurSel, 2, (LPWSTR)L"0");',
                    '\t\t\t\tListView_SetItemText(hCtrl, nCurSel, 3, (LPWSTR)L"—");\t// (04/09) cot Viec\n'
                    '\t\t\t\tListView_SetItemText(hCtrl, nCurSel, 4, (LPWSTR)L"—");\t// (04/09) cot Ban do',
                    'nCurSel, 3, (LPWSTR)L"—"')
    t.them_sau_dong('ListView_SetItemText(hCtrl, foundIndex, 2, (LPWSTR)L"0");',
                    '\t\t\t\t\tListView_SetItemText(hCtrl, foundIndex, 3, (LPWSTR)L"—");\t// (04/09) cot Viec\n'
                    '\t\t\t\t\tListView_SetItemText(hCtrl, foundIndex, 4, (LPWSTR)L"—");\t// (04/09) cot Ban do',
                    'foundIndex, 3, (LPWSTR)L"—");\t// (04/09) cot Viec')
    t.them_sau_dong("strcpy(gnode.player.szMap, pInfo->szMap);", MAP_MOI, "pMapCot")
    # nho ban CU cua szHoatDong truoc khi ghi de
    t.them_sau_dong("// r2: dong trang thai auto (Game.exe cu gui goi ngan -> coi nhu rong)",
                    '\t\t\t// (04/09) giu ban CU de biet co phai ve lai cot "Viec" khong.\n'
                    "\t\t\tchar szHDCu[sizeof(gnode.player.szHoatDong)];\n"
                    "\t\t\tlstrcpynA(szHDCu, gnode.player.szHoatDong, (int)sizeof(szHDCu));",
                    "char szHDCu[sizeof(gnode.player.szHoatDong)];")
    t.them_sau_dong("\t\t\t\tgnode.player.szHoatDong[0] = 0;", HD_MOI, "pHDCot")


def main():
    print("== goi_va_wauto_ui_thai_0409 %s dot=%s ==" % ("(THU - khong ghi)" if THU else "", sorted(DOTS)))
    p = os.path.join(WA_E, "WAuto.cpp")
    t = Tep(p)
    if 1 in DOTS:
        dot1(t)
    if 2 in DOTS:
        dot2(t)
    doi = t.ghi()
    if LOI:
        print("LOI:")
        for l in LOI: print("  " + l)
        sys.exit(1)
    if doi and not THU:
        shutil.copyfile(p, os.path.join(WA_D, "WAuto.cpp"))
        print("da chep mirror WAutoUI")
    print("XONG")


if __name__ == "__main__":
    main()
