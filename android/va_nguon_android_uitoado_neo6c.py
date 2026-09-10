# -*- coding: utf-8 -*-
#
# [UITOADO 12/09 NEO f] (sau NEO d/e) logcat cua Android cat bot dong khi ghi doi dao (logd "chatty": 400 dong -> con 163)
# -> nhat ky vi tri thuc ghi ra TEP <thu muc du lieu>\bocuc_thuc.txt (ghi de moi lan), khong qua g_DebugLog.
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[UITOADO 12/09 NEO f]"


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
    s = thay(s, ["static void DumpCayThuc(KWndWindow* pWnd)\t// [UITOADO 12/09 NEO d]",
                 "{",
                 "\tchar szKhoa[UITOADO_CO_KHOA];"],
                ["static FILE* s_fDumpThuc = NULL;\t// %s" % DAU,
                 "static void DumpCayThuc(KWndWindow* pWnd)\t// [UITOADO 12/09 NEO d]",
                 "{",
                 "\tchar szKhoa[UITOADO_CO_KHOA];"], "bien tep")
    s = thay(s, ["\t\tif (TaoKhoaTuOCon(pWnd, szKhoa, sizeof(szKhoa)) && strchr(szKhoa, '#') == NULL)",
                 "\t\t\tg_DebugLog(\"[BOCUC-THUC] %s = abs %d,%d rel %d,%d %dx%d\", szKhoa, x, y, rx, ry, w, h);"],
                ["\t\tif (TaoKhoaTuOCon(pWnd, szKhoa, sizeof(szKhoa)) && strchr(szKhoa, '#') == NULL)",
                 "\t\t{",
                 "\t\t\tif (s_fDumpThuc)\t// %s" % DAU,
                 "\t\t\t\tfprintf(s_fDumpThuc, \"%s = abs %d,%d rel %d,%d %dx%d%s\\n\", szKhoa, x, y, rx, ry, w, h, pWnd->IsVisible() ? \"\" : \" an\");",
                 "\t\t\telse",
                 "\t\t\t\tg_DebugLog(\"[BOCUC-THUC] %s = abs %d,%d rel %d,%d %dx%d\", szKhoa, x, y, rx, ry, w, h);",
                 "\t\t}"], "ghi tep")
    s = thay(s, ["\tg_DebugLog(\"[BOCUC-THUC] khung ve %dx%d\", SCREEN_WIDTH, SCREEN_HEIGHT);"],
                ["\tg_DebugLog(\"[BOCUC-THUC] khung ve %dx%d\", SCREEN_WIDTH, SCREEN_HEIGHT);",
                 "\t{\t// %s ghi ra tep de khoi bi logcat cat bot" % DAU,
                 "\t\tchar szTep[MAX_PATH];",
                 "\t\tg_GetFullPath(szTep, \"\\\\bocuc_thuc.txt\");",
                 "\t\ts_fDumpThuc = fopen(szTep, \"w\");",
                 "\t\tif (s_fDumpThuc)",
                 "\t\t\tfprintf(s_fDumpThuc, \"khung ve %dx%d\\n\", SCREEN_WIDTH, SCREEN_HEIGHT);",
                 "\t}"], "mo tep")
    s = thay(s, ["\tDumpCayThuc(Wnd_GetLayerRoot(WL_TOPMOST)->GetNextWnd());"],
                ["\tDumpCayThuc(Wnd_GetLayerRoot(WL_TOPMOST)->GetNextWnd());",
                 "\tif (s_fDumpThuc)\t// %s" % DAU,
                 "\t{",
                 "\t\tfclose(s_fDumpThuc);",
                 "\t\ts_fDumpThuc = NULL;",
                 "\t\tg_DebugLog(\"[BOCUC-THUC] da ghi bocuc_thuc.txt\");",
                 "\t}"], "dong tep")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi")
    ghi(T, s); print("da va:", T)
print("xong")
