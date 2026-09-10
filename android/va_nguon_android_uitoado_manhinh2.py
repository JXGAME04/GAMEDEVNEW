# -*- coding: utf-8 -*-
#
# [UITOADO 12/09 RONG] Dien thoai man rong (khung ve ti le >= 1,9) dung tep bo cuc mac dinh RIENG \Ui\UiToaDo_MacDinh_Rong.ini
# (sinh boi android/sinh_bocuc_rong.py: cot icon phai + ban do nho + cum ky nang sat mep phai, thanh duoi giua, hang icon tren
# phong 1,3 lan, gian ra). Khong co tep -> dung tep mac dinh thuong (va neo theo mep nhu MANHINH). Chi JX_ANDROID.
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[UITOADO 12/09 RONG]"
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


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


s = doc(TEP)
if DAU in s:
    raise SystemExit("da va roi: " + TEP)
s0 = s
s = thay(s, ["#define\tUITOADO_TEP_MACDINH\t\"\\\\Ui\\\\UiToaDo_MacDinh.ini\""],
            ["#define\tUITOADO_TEP_MACDINH\t\"\\\\Ui\\\\UiToaDo_MacDinh.ini\"",
             "#define\tUITOADO_TEP_MACDINH_RONG\t\"\\\\Ui\\\\UiToaDo_MacDinh_Rong.ini\"\t// %s man rong (ti le >= 1,9)" % DAU],
        "define")
s = thay(s, ["void UiToaDo_Nap()", "{", "\ts_nSo   = 0;", "\ts_bTran = 0;", "\tNapTep(UITOADO_TEP_MACDINH);"],
            ["void UiToaDo_Nap()", "{", "\ts_nSo   = 0;", "\ts_bTran = 0;",
             "#ifdef JX_ANDROID",
             "\t// %s khung ve rong (dien thoai 19:9 .. 21:9) -> bo cuc mac dinh rieng neu co tep" % DAU,
             "\tif (SCREEN_WIDTH * 10 >= SCREEN_HEIGHT * 19)",
             "\t{",
             "\t\tchar szRong[MAX_PATH];",
             "\t\tg_GetFullPath(szRong, (char*)UITOADO_TEP_MACDINH_RONG);",
             "\t\tFILE* pThu = fopen(szRong, \"rt\");",
             "\t\tif (pThu)",
             "\t\t{",
             "\t\t\tfclose(pThu);",
             "\t\t\tg_DebugLog(\"[UITOADO] khung ve %dx%d rong -> dung %s\", SCREEN_WIDTH, SCREEN_HEIGHT, UITOADO_TEP_MACDINH_RONG);",
             "\t\t\tNapTep(UITOADO_TEP_MACDINH_RONG);",
             "\t\t\tNapTep(UITOADO_TEP);",
             "\t\t\treturn;",
             "\t\t}",
             "\t}",
             "#endif",
             "\tNapTep(UITOADO_TEP_MACDINH);"], "UiToaDo_Nap")
if cao(s) != cao(s0):
    raise SystemExit("so byte cao doi")
ghi(TEP, s)
print("da va:", TEP)
