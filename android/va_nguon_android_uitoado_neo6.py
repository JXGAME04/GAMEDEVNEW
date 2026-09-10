# -*- coding: utf-8 -*-
#
# [UITOADO 12/09 NEO d] Nhat ky VI TRI THUC cua moi cua so (duyet cay tu 3 goc lop) 15 s sau khi vao game khi [Ui] NhatKyBoCuc>=2:
# "[BOCUC-THUC] <khoa> = abs x,y rel x,y WxH" - de doi chieu bang toa do voi cai game thuc su ve (may ao dat man kieu dien thoai).
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[UITOADO 12/09 NEO d]"


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


T = "Sources/S3Client/Ui/Elem/UiToaDo.cpp"
s = doc(T)
if DAU not in s:
    s0 = s
    s = thay(s, ["void UiToaDo_ApChoO(KWndWindow* pWnd)"],
                ["static void DumpCayThuc(KWndWindow* pWnd)\t// %s" % DAU,
                 "{",
                 "\tchar szKhoa[UITOADO_CO_KHOA];",
                 "\twhile (pWnd)",
                 "\t{",
                 "\t\tint x = 0, y = 0, w = 0, h = 0, rx = 0, ry = 0;",
                 "\t\tpWnd->GetAbsolutePos(&x, &y);",
                 "\t\tpWnd->GetSize(&w, &h);",
                 "\t\tpWnd->GetPosition(&rx, &ry);",
                 "\t\tif (TaoKhoaTuOCon(pWnd, szKhoa, sizeof(szKhoa)) && strchr(szKhoa, '#') == NULL)",
                 "\t\t\tg_DebugLog(\"[BOCUC-THUC] %s = abs %d,%d rel %d,%d %dx%d\", szKhoa, x, y, rx, ry, w, h);",
                 "\t\tDumpCayThuc(pWnd->GetFirstChild());",
                 "\t\tpWnd = pWnd->GetNextWnd();",
                 "\t}",
                 "}",
                 "void UiToaDo_DumpThuc()",
                 "{",
                 "\tstatic int s_nLan = 0;",
                 "\tif (s_nLan++ != 0)",
                 "\t\treturn;",
                 "\tg_DebugLog(\"[BOCUC-THUC] khung ve %dx%d\", SCREEN_WIDTH, SCREEN_HEIGHT);",
                 "\tDumpCayThuc(Wnd_GetLayerRoot(WL_LOWEST)->GetFirstChild());",
                 "\tDumpCayThuc(Wnd_GetLayerRoot(WL_NORMAL)->GetFirstChild());",
                 "\tDumpCayThuc(Wnd_GetLayerRoot(WL_TOPMOST)->GetFirstChild());",
                 "}",
                 "void UiToaDo_ApChoO(KWndWindow* pWnd)"], "DumpThuc")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi")
    ghi(T, s); print("da va:", T)

H = "Sources/S3Client/Ui/Elem/UiToaDo.h"
sh = doc(H)
if DAU not in sh:
    sh = thay(sh, ["void\tUiToaDo_ApChoO(KWndWindow* pWnd);"],
                  ["void\tUiToaDo_ApChoO(KWndWindow* pWnd);",
                   "void\tUiToaDo_DumpThuc();\t// %s nhat ky vi tri thuc moi cua so (mot lan)" % DAU], "UiToaDo.h")
    ghi(H, sh); print("da va:", H)

J = "Sources/S3Client/Platform/JxCanDieuKhien.cpp"
sj = doc(J)
if DAU not in sj:
    sj = thay(sj, ["void JxKyNang_Ve()", "{", "\tint i, nX, nY, nR, nIcon;", "\tKUiGameObject o;", ""],
                  ["void JxKyNang_Ve()", "{", "\tint i, nX, nY, nR, nIcon;", "\tKUiGameObject o;",
                   "\t{\t// %s 900 khung (~15 s) sau khi vao the gioi: ghi vi tri thuc moi cua so neu [Ui] NhatKyBoCuc>=2" % DAU,
                   "\t\tstatic int s_nKhungBoCuc = 0;",
                   "\t\tif (KyNang_TrongGame() && ++s_nKhungBoCuc == 900",
                   "\t\t\t&& GetPrivateProfileInt(\"Ui\", \"NhatKyBoCuc\", 0, \".\\\\config.ini\") >= 2)",
                   "\t\t\tUiToaDo_DumpThuc();",
                   "\t}",
                   ""], "JxKyNang_Ve")
    ghi(J, sj); print("da va:", J)
print("xong")
