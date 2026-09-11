# -*- coding: utf-8 -*-
#
# [VATPHAM 12/09 g] Chu 16:15: "chinh lai DE VAO ITEM VAI GIAY LA TU CAM LEN TAY - luc di chuyen toi o nao trong
# ruong hoac hanh trang se SANG nhung o do len de biet cho dat".
#
# Hien tai giu ngon lau tren giao dien = chuot phai (KSdlApp::NhipCham) = dung / mac vat pham. Nay:
#   * Giu ngon lau ngay tren mot O CO VAT PHAM  -> bam TRAI kem co "giu" -> hai lop chua vat pham BO QUA dai nut
#     ([VATPHAM 12/09]) va chay duong goc cua ban PC = NHAC MON LEN TAY.
#   * Sau do ngon van dang de: moi lan di chuyen deu bom WM_MOUSEMOVE, ma ban PC da co san
#     OBJCONT_S_TRACE_PUT_POS (hanh trang / ruong deu bat) -> o dich duoc to bong. Tren dien thoai bong do qua mo
#     (alpha 10/255) nen ban Android to dam han.
#   * Nha ngon TREN giao dien = tha mon vao dung o do; nha ngon NGOAI giao dien thi khong bam gi (bam ra ban do la
#     nem mon xuong dat -> mat do).
#   * Giu lau o cho KHAC (ban do, hoac giao dien khong phai o vat pham) van la chuot phai nhu cu.
#   * Dang cam mon tren tay ma cham vao o vat pham thi cung bo qua dai nut (phai tha duoc mon xuong).
# Chi Android.
import io
import os

os.chdir(r"D:\GAMEDEVNEW\.claude\worktrees\full-wauto-mobile-analysis-f781df")
DAU = "[VATPHAM 12/09 g]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def nl_cua(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def thay(s, cu, moi, ten):
    nl = nl_cua(s)
    c = nl.join(cu)
    if s.count(c) != 1:
        raise SystemExit("khong tim thay dung 1 cho (%d): %s" % (s.count(c), ten))
    return s.replace(c, nl.join(moi))


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


# ---------------- 1) ma thong diep hoi "co o vat pham khong" ----------------
M = "Sources/S3Client/Ui/Elem/WndMessage.h"
s = doc(M)
if DAU not in s:
    s0 = s
    s = thay(s, ["#define WND_M_BUTTON_BASE\t\t\tWND_M_BASE + 100"],
                ["#ifdef JX_ANDROID",
                 "//\t%s Hoi mot cua so: tai diem nay (toa do CUA SO trong nParam) co dang la mot O CO VAT PHAM khong?" % DAU,
                 "//\tChi hai lop chua vat pham (KWndObjectBox / KWndObjectMatrix) tra 1, con lai tra 0 (WndProc mac dinh).",
                 "//\tDung cho \"giu ngon lau tren o vat pham = nhac mon len tay\" (KSdlApp::NhipCham).",
                 "#define\tWND_M_JX_CO_VATPHAM\t\tWND_M_BASE + 990",
                 "#endif",
                 "#define WND_M_BUTTON_BASE\t\t\tWND_M_BASE + 100"], "ma thong diep")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (WndMessage.h)")
    ghi(M, s)
    print("da va:", M)

# ---------------- 2) Wnds.cpp: hoi cua so tren cung tai diem ----------------
W = "Sources/S3Client/Ui/Elem/Wnds.cpp"
s = doc(W)
if DAU not in s:
    s0 = s
    s = thay(s, ["extern \"C\" int JxUi_CoGiaoDienTaiDiem(int x, int y)",
                 "{",
                 "\tKWndWindow* pWnd = Wnd_GetActive(x, y, false);",
                 "\treturn (pWnd != NULL && pWnd != s_WndStation.pGameSpaceWnd) ? 1 : 0;",
                 "}"],
                ["extern \"C\" int JxUi_CoGiaoDienTaiDiem(int x, int y)",
                 "{",
                 "\tKWndWindow* pWnd = Wnd_GetActive(x, y, false);",
                 "\treturn (pWnd != NULL && pWnd != s_WndStation.pGameSpaceWnd) ? 1 : 0;",
                 "}",
                 "",
                 "//\t%s Hoi CA CAY cua so con: o vat pham (KWndObjectBox / KWndObjectMatrix) luon la cua so CON cua" % DAU,
                 "//\thop thoai (hanh trang, ruong...), ma Wnd_GetActive chi tra cua so TOP nen phai tu di xuong.",
                 "static int JxUi_HoiCoVatPham(KWndWindow* pWnd, int x, int y)",
                 "{",
                 "\tKWndWindow* pCon = pWnd->GetFirstChild();",
                 "\tint nAbsX = 0, nAbsY = 0;",
                 "",
                 "\twhile (pCon)",
                 "\t{",
                 "\t\tif (pCon->PtInWindow(x, y) && JxUi_HoiCoVatPham(pCon, x, y))",
                 "\t\t\treturn 1;",
                 "\t\tpCon = pCon->GetNextWnd();",
                 "\t}",
                 "\tpWnd->GetAbsolutePos(&nAbsX, &nAbsY);",
                 "\treturn pWnd->WndProc(WND_M_JX_CO_VATPHAM, 0, MAKELPARAM(x - nAbsX, y - nAbsY)) ? 1 : 0;",
                 "}",
                 "",
                 "//\t%s Diem (x, y) co nam tren mot O DANG CO VAT PHAM khong (hanh trang, ruong, o trang bi...)?" % DAU,
                 "//\tDung cho \"giu ngon lau tren o vat pham = nhac mon len tay\": chi khi co mon that moi doi giu lau",
                 "//\ttu chuot phai sang bam trai, con lai (nut, danh sach, ban do) giu nguyen nhu cu.",
                 "extern \"C\" int JxUi_CoVatPhamTaiDiem(int x, int y)",
                 "{",
                 "\tKWndWindow* pWnd = Wnd_GetActive(x, y, false);",
                 "",
                 "\tif (pWnd == NULL || pWnd == s_WndStation.pGameSpaceWnd)",
                 "\t\treturn 0;",
                 "\treturn JxUi_HoiCoVatPham(pWnd, x, y);",
                 "}"], "JxUi_CoVatPhamTaiDiem")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (Wnds.cpp)")
    ghi(W, s)
    print("da va:", W)

# ---------------- 3) WndObjContainer: tra loi cau hoi + bo qua dai nut khi dang giu / dang cam ----------------
C = "Sources/S3Client/Ui/Elem/WndObjContainer.cpp"
s = doc(C)
if DAU not in s:
    s0 = s
    #   (a) co "giu lau" (dat trong UiVatPham.cpp)
    s = thay(s, ["#ifdef JX_ANDROID",
                 "#include \"../UiCase/UiVatPham.h\"\t// [VATPHAM 12/09] bang thong tin vat pham + nut thao tac (chi mobile)",
                 "#endif"],
                ["#ifdef JX_ANDROID",
                 "#include \"../UiCase/UiVatPham.h\"\t// [VATPHAM 12/09] bang thong tin vat pham + nut thao tac (chi mobile)",
                 "extern \"C\" int JxVatPham_LayGiu(void);\t// %s UiVatPham.cpp: 1 = cu bam nay do GIU NGON LAU sinh ra" % DAU,
                 "#endif"], "khai bao co giu")
    #   (b) KWndObjectBox: bo qua dai nut khi giu lau / dang cam mon
    s = thay(s, ["#ifdef JX_ANDROID",
                 "\t\tif (m_Object.uGenre != CGOG_NOTHING)\t// [VATPHAM 12/09]",
                 "\t\t{"],
                ["#ifdef JX_ANDROID",
                 "\t\t//\t%s Giu ngon lau, hoac dang cam mon tren tay -> KHONG mo dai nut: chay duong goc cua ban PC" % DAU,
                 "\t\t//\t(nhac mon len tay / tha mon xuong o nay).",
                 "\t\tif (m_Object.uGenre != CGOG_NOTHING && !JxVatPham_LayGiu() && !Wnd_GetDragObj(NULL))\t// [VATPHAM 12/09]",
                 "\t\t{"], "box bo qua")
    #   (c) KWndObjectMatrix: nhu tren
    s = thay(s, ["#ifdef JX_ANDROID",
                 "\t\t{\t// [VATPHAM 12/09] cham vao o co vat pham -> mo bang thong tin + nut, khong nhac len tay",
                 "\t\t\tint nO = GetObjectAt(LOWORD(nParam), HIWORD(nParam));",
                 "\t\t\tif (nO >= 0)"],
                ["#ifdef JX_ANDROID",
                 "\t\t{\t// [VATPHAM 12/09] cham vao o co vat pham -> mo bang thong tin + nut, khong nhac len tay",
                 "\t\t\t//\t%s giu ngon lau / dang cam mon tren tay thi bo qua dai nut (nhac len tay - tha xuong o)." % DAU,
                 "\t\t\tint nO = (JxVatPham_LayGiu() || Wnd_GetDragObj(NULL)) ? -1 : GetObjectAt(LOWORD(nParam), HIWORD(nParam));",
                 "\t\t\tif (nO >= 0)"], "matrix bo qua")
    #   (d) tra loi WND_M_JX_CO_VATPHAM
    s = thay(s, ["\tcase WM_MOUSEMOVE:",
                 "\t\tm_Style |= OBJCONT_F_MOUSE_HOVER;",
                 "\t\tif (m_Object.uGenre != CGOG_NOTHING && g_MouseOver.IsMoseHoverWndObj(this, 0) == 0)"],
                ["#ifdef JX_ANDROID",
                 "\tcase WND_M_JX_CO_VATPHAM:\t// %s o don: co mon la duoc" % DAU,
                 "\t\treturn (m_Object.uGenre != CGOG_NOTHING) ? 1 : 0;",
                 "#endif",
                 "\tcase WM_MOUSEMOVE:",
                 "\t\tm_Style |= OBJCONT_F_MOUSE_HOVER;",
                 "\t\tif (m_Object.uGenre != CGOG_NOTHING && g_MouseOver.IsMoseHoverWndObj(this, 0) == 0)"], "box tra loi")
    s = thay(s, ["\tcase WM_MOUSEMOVE:",
                 "\t\tm_Style |= OBJCONT_F_MOUSE_HOVER;",
                 "\t\t{",
                 "\t\t\tint\tx = LOWORD(nParam);"],
                ["#ifdef JX_ANDROID",
                 "\tcase WND_M_JX_CO_VATPHAM:\t// %s luoi: phai cham dung mot o dang co mon" % DAU,
                 "\t\treturn (GetObjectAt(LOWORD(nParam), HIWORD(nParam)) >= 0) ? 1 : 0;",
                 "#endif",
                 "\tcase WM_MOUSEMOVE:",
                 "\t\tm_Style |= OBJCONT_F_MOUSE_HOVER;",
                 "\t\t{",
                 "\t\t\tint\tx = LOWORD(nParam);"], "matrix tra loi")
    #   (e) bong o dich: to dam hon tren dien thoai
    s = thay(s, ["\t\tShadow.Color.Color_dw = l_BgColors[4];",
                 "\t\tg_pRepresentShell->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);"],
                ["\t\tShadow.Color.Color_dw = l_BgColors[4];",
                 "#ifdef JX_ANDROID",
                 "\t\t//\t%s Chu: \"di chuyen toi o nao thi SANG o do len de biet cho dat\". Mau trong ui.ini co alpha 10/255," % DAU,
                 "\t\t//\tnhin tren dien thoai gan nhu khong thay -> ban Android to dam han (xanh sang).",
                 "\t\tShadow.Color.Color_dw = 0x7028a0ff;",
                 "#endif",
                 "\t\tg_pRepresentShell->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);"], "bong dam hon")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (WndObjContainer.cpp)")
    ghi(C, s)
    print("da va:", C)

# ---------------- 4) UiVatPham.cpp: co "giu lau" ----------------
U = "Sources/S3Client/Ui/UiCase/UiVatPham.cpp"
s = doc(U)
if DAU not in s:
    s0 = s
    s = thay(s, ["static unsigned int\ts_uNemCho  = 0;\t// ma dw mon dang cho nem (0 = khong cho)"],
                ["//\t%s 1 = cu bam trai dang gui la do GIU NGON LAU tren o vat pham sinh ra (KSdlApp::NhipCham)." % DAU,
                 "//\tHai lop chua vat pham xem co nay de BO QUA dai nut va chay duong goc = nhac mon len tay.",
                 "static int\t\t\ts_nGiuLau  = 0;",
                 "",
                 "extern \"C\" void JxVatPham_DatGiu(int nBat) { s_nGiuLau = nBat ? 1 : 0; }",
                 "extern \"C\" int  JxVatPham_LayGiu(void)     { return s_nGiuLau; }",
                 "",
                 "static unsigned int\ts_uNemCho  = 0;\t// ma dw mon dang cho nem (0 = khong cho)"], "co giu lau")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (UiVatPham.cpp)")
    ghi(U, s)
    print("da va:", U)

# ---------------- 5) KSdlApp.h: them trang thai CHAM_CAM ----------------
H = "Sources/S3Client/Platform/KSdlApp.h"
s = doc(H)
if DAU not in s:
    s0 = s
    s = thay(s, ["\tenum { CHAM_KHONG = 0, CHAM_CHO, CHAM_KEO, CHAM_PHAI, CHAM_RE, CHAM_CAN, CHAM_CUON,",
                 "\t\tCHAM_KYNANG };"],
                ["\t// %s CHAM_CAM = vua giu lau tren o vat pham -> da nhac mon len tay, ngon van dang de:" % DAU,
                 "\t// di chuyen thi o dich sang len, nha ngon tren giao dien thi tha mon vao o do.",
                 "\tenum { CHAM_KHONG = 0, CHAM_CHO, CHAM_KEO, CHAM_PHAI, CHAM_RE, CHAM_CAN, CHAM_CUON,",
                 "\t\tCHAM_KYNANG, CHAM_CAM };"], "trang thai CHAM_CAM")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (KSdlApp.h)")
    ghi(H, s)
    print("da va:", H)

# ---------------- 6) KSdlApp.cpp: giu lau tren o vat pham + nha ngon ----------------
K = "Sources/S3Client/Platform/KSdlApp.cpp"
s = doc(K)
if DAU not in s:
    s0 = s
    s = thay(s, ["extern \"C\" int JxUi_CoGiaoDienTaiDiem(int x, int y);\t// Wnds.cpp"],
                ["extern \"C\" int JxUi_CoGiaoDienTaiDiem(int x, int y);\t// Wnds.cpp",
                 "extern \"C\" int JxUi_CoVatPhamTaiDiem(int x, int y);\t// %s Wnds.cpp" % DAU,
                 "extern \"C\" void JxVatPham_DatGiu(int nBat);\t// %s UiVatPham.cpp" % DAU], "khai bao")
    #   NhipCham: giu lau tren o vat pham -> bam trai kem co giu
    s = thay(s, ["\t// CHAM_RE khong con dung: cham giu o DAU cung la chuot phai.",
                 "\t// Tren GIAO DIEN, chuot phai chinh la duong MAC / THAO / DUNG vat pham cua ban PC - bo no di thi",
                 "\t// nguoi choi khong mac duoc do. Tren BAN DO, chuot phai la danh ep / chon muc tieu."],
                ["\t//\t%s Chu 16:15: \"de vao item vai giay la tu cam len tay\". Giu ngon lau ngay tren mot O CO VAT PHAM:" % DAU,
                 "\t//\tbam TRAI kem co \"giu\" -> lop chua vat pham bo qua dai nut va chay duong goc = nhac mon len tay.",
                 "\t//\tNgon van dang de (CHAM_CAM): di chuyen thi o dich sang len, nha ngon tren giao dien la tha vao o do.",
                 "\tif (JxUi_CoVatPhamTaiDiem(m_nChamX0, m_nChamY0))",
                 "\t{",
                 "\t\tLPARAM l = MAKELPARAM(m_nChamX0, m_nChamY0);",
                 "",
                 "\t\tg_DebugLog(\"[CHAM] giu tai %d,%d -> nhac vat pham len tay\", m_nChamX0, m_nChamY0);",
                 "\t\tm_nCham = CHAM_CAM;",
                 "\t\tJxVatPham_DatGiu(1);",
                 "\t\tGhiChuot(MK_LBUTTON, l);",
                 "\t\tMsgProc(g_GetMainHWnd(), WM_LBUTTONDOWN, MK_LBUTTON, l);",
                 "\t\tGhiChuot(0, l);",
                 "\t\tMsgProc(g_GetMainHWnd(), WM_LBUTTONUP, 0, l);",
                 "\t\tJxVatPham_DatGiu(0);",
                 "\t\treturn;",
                 "\t}",
                 "\t// CHAM_RE khong con dung: cham giu o DAU cung la chuot phai.",
                 "\t// Tren GIAO DIEN, chuot phai chinh la duong MAC / THAO / DUNG vat pham cua ban PC - bo no di thi",
                 "\t// nguoi choi khong mac duoc do. Tren BAN DO, chuot phai la danh ep / chon muc tieu."], "nhip cham")
    #   nha ngon khi dang cam mon
    s = thay(s, ["\t\telse if (nTruoc == CHAM_PHAI)",
                 "\t\t{",
                 "\t\t\tGhiChuot(0, MAKELPARAM(m_nChamX0, m_nChamY0));",
                 "\t\t\tMsgProc(hWnd, WM_RBUTTONUP, 0, MAKELPARAM(m_nChamX0, m_nChamY0));",
                 "\t\t}"],
                ["\t\telse if (nTruoc == CHAM_PHAI)",
                 "\t\t{",
                 "\t\t\tGhiChuot(0, MAKELPARAM(m_nChamX0, m_nChamY0));",
                 "\t\t\tMsgProc(hWnd, WM_RBUTTONUP, 0, MAKELPARAM(m_nChamX0, m_nChamY0));",
                 "\t\t}",
                 "\t\telse if (nTruoc == CHAM_CAM)",
                 "\t\t{",
                 "\t\t\t//\t%s dang cam mon tren tay: nha ngon TREN GIAO DIEN = tha mon vao dung o do (bam trai nhu ban" % DAU,
                 "\t\t\t//\tPC). Nha ngon NGOAI giao dien thi KHONG bam gi: bam ra ban do la nem mon xuong dat, mat do.",
                 "\t\t\tLPARAM l = MAKELPARAM((int)fx, (int)fy);",
                 "",
                 "\t\t\tif (JxUi_CoGiaoDienTaiDiem((int)fx, (int)fy))",
                 "\t\t\t{",
                 "\t\t\t\tGhiChuot(MK_LBUTTON, l);",
                 "\t\t\t\tMsgProc(hWnd, WM_LBUTTONDOWN, MK_LBUTTON, l);",
                 "\t\t\t\tGhiChuot(0, l);",
                 "\t\t\t\tMsgProc(hWnd, WM_LBUTTONUP, 0, l);",
                 "\t\t\t}",
                 "\t\t}"], "nha ngon cam mon")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi (KSdlApp.cpp)")
    ghi(K, s)
    print("da va:", K)
print("xong")
