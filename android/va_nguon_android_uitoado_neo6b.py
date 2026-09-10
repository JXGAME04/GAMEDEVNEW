# -*- coding: utf-8 -*-
#
# [UITOADO 12/09 NEO e] (sau NEO d) (1) cua so cap 1 la ANH EM cua goc lop (Wnd_AddWindow -> AddBrother) chu khong phai con
# -> duyet tu goc->GetNextWnd(); (2) nhat ky tung lan UiToaDo_ApChoO khi NhatKyBoCuc>=2 de biet vi sao KUiPlayerBar|Item_0..3 khong ap.
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[UITOADO 12/09 NEO e]"


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
    for lop in ("WL_LOWEST", "WL_NORMAL", "WL_TOPMOST"):
        s = thay(s, ["\tDumpCayThuc(Wnd_GetLayerRoot(%s)->GetFirstChild());" % lop],
                    ["\tDumpCayThuc(Wnd_GetLayerRoot(%s)->GetNextWnd());%s" % (lop, "\t// %s cap 1 = anh em cua goc" % DAU if lop == "WL_LOWEST" else "")], lop)
    s = thay(s, ["\tif (TaoKhoaTuOCon(pWnd, szKhoa, sizeof(szKhoa)))",
                 "\t\tApMotO(pWnd, TimKhoa(szKhoa));",
                 "}",
                 "#endif"],
                ["\tif (TaoKhoaTuOCon(pWnd, szKhoa, sizeof(szKhoa)))",
                 "\t{",
                 "\t\tint nMuc = TimKhoa(szKhoa);",
                 "\t\tApMotO(pWnd, nMuc);",
                 "\t\tif (s_nNhatKyBoCuc >= 2)\t// %s" % DAU,
                 "\t\t{",
                 "\t\t\tint rx = 0, ry = 0;",
                 "\t\t\tpWnd->GetPosition(&rx, &ry);",
                 "\t\t\tg_DebugLog(\"[BOCUC-AP] %s muc %d -> rel %d,%d\", szKhoa, nMuc, rx, ry);",
                 "\t\t}",
                 "\t}",
                 "}",
                 "#endif"], "ApChoO log")
    s = thay(s, ["static char\t\t\ts_szKhoaKeo[UITOADO_CO_KHOA] = \"\";"],
                ["static char\t\t\ts_szKhoaKeo[UITOADO_CO_KHOA] = \"\";",
                 "static int\t\t\ts_nNhatKyBoCuc = 0;\t// %s [Ui] NhatKyBoCuc, doc mot lan khi nap bang" % DAU], "bien log")
    CFG = "\".\\\\config.ini\""       # trong nguon C: ".\\config.ini"
    s = thay(s, ["\tif (GetPrivateProfileInt(\"Ui\", \"NhatKyBoCuc\", 0, " + CFG + "))",
                 "\t{",
                 "\t\tfor (i = 0; i < s_nSo; i++)"],
                ["\ts_nNhatKyBoCuc = GetPrivateProfileInt(\"Ui\", \"NhatKyBoCuc\", 0, " + CFG + ");\t// " + DAU,
                 "\tif (s_nNhatKyBoCuc)",
                 "\t{",
                 "\t\tfor (i = 0; i < s_nSo; i++)"], "doc co")
    if cao(s) != cao(s0):
        raise SystemExit("byte cao doi")
    ghi(T, s); print("da va:", T)
print("xong")
