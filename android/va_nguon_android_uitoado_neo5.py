# -*- coding: utf-8 -*-
#
# [UITOADO 12/09 NEO c] Nhat ky de KIEM bo cuc tren may ao dat man kieu dien thoai (wm size 1440x617) ma khong can nhin hinh:
# sau khi doi ca tep theo neo, ghi moi muc "khoa = L,T neo(x,y)" vao jx_android.log (g_DebugLog) khi config [Ui] NhatKyBoCuc=1.
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[UITOADO 12/09 NEO c]"
TEP = "Sources/S3Client/Ui/Elem/UiToaDo.cpp"


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


s = doc(TEP)
if DAU in s:
    raise SystemExit("da va roi")
s = thay(s, ["\t\ts_Bang[i].nLeft += nDX * (s_Bang[i].nNeoX - nNeoXCha) / 2;",
             "\t\ts_Bang[i].nTop  += nDY * (s_Bang[i].nNeoY - nNeoYCha) / 2;",
             "\t}",
             "}"],
            ["\t\ts_Bang[i].nLeft += nDX * (s_Bang[i].nNeoX - nNeoXCha) / 2;",
             "\t\ts_Bang[i].nTop  += nDY * (s_Bang[i].nNeoY - nNeoYCha) / 2;",
             "\t}",
             "\t// %s [Ui] NhatKyBoCuc=1 -> ghi ca bang sau khi dich de doi chieu tren may ao" % DAU,
             "\tif (GetPrivateProfileInt(\"Ui\", \"NhatKyBoCuc\", 0, \".\\\\config.ini\"))",
             "\t{",
             "\t\tfor (i = 0; i < s_nSo; i++)",
             "\t\t\tif (s_aTepNap[i] == s_nTepNap)",
             "\t\t\t\tg_DebugLog(\"[BOCUC] %s = %d,%d neo %d,%d tile %d\", s_Bang[i].szKhoa, s_Bang[i].nLeft, s_Bang[i].nTop,",
             "\t\t\t\t\ts_Bang[i].nNeoX, s_Bang[i].nNeoY, s_Bang[i].nTiLe);",
             "\t}",
             "}"], "dump")
ghi(TEP, s)
print("da va:", TEP)
