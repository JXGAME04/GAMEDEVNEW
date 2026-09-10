# -*- coding: utf-8 -*-
#
# [UITOADO 12/09 MANHINH] Chu (01:30, anh dien thoai 21:9): "Choi dien thoai no khong ra full man" - bo cuc UI luu trong
# UiToaDo (mac dinh ui\uitoado_macdinh.ini + userdata\UiToaDo.ini) la toa do TUYET DOI cua may ao 1040x604; dien thoai co khung
# ve rong hon (1371x617...) thi cot icon phai, ban do nho, cum nut ky nang van nam o cho 1040 -> dai ben phai trong.
# Sua trong Sources/S3Client/Ui/Elem/UiToaDo.cpp (chi JX_ANDROID, ban PC giu nguyen):
#   * tep bo cuc co dong "ManHinh=W,H" trong [Pos] = khung ve luc luu (game ghi khi luu; tep mac dinh ghi 1040,604; thieu = 1040x604)
#   * luc NAP: o nao co X >= 70 % rong thiet ke thi neo theo mep PHAI (cong them SCREEN_WIDTH - W), Y >= 70 % cao -> neo mep DUOI;
#     con lai giu nguyen (thanh chat, hop thoai giua, cot trai). Tat bang config.ini [Ui] NeoTheoMep=0.
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[UITOADO 12/09 MANHINH]"
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

# 1. bien + ham doi toa do theo khung ve
s = thay(s, ["extern int SCREEN_WIDTH;\t\t// S3Client.cpp / KSdlApp.cpp: co khung ve that", "extern int SCREEN_HEIGHT;"],
            ["extern int SCREEN_WIDTH;\t\t// S3Client.cpp / KSdlApp.cpp: co khung ve that", "extern int SCREEN_HEIGHT;",
             "",
             "#ifdef JX_ANDROID",
             "// %s Bo cuc luu theo khung ve luc thiet ke (dong 'ManHinh=W,H' trong [Pos]; thieu = 1040x604 cua may ao)." % DAU,
             "// Khung ve khac (dien thoai 21:9 ~1371x617): o co X >= 70 %% rong thiet ke neo theo mep PHAI, Y >= 70 %% cao neo mep DUOI,",
             "// con lai giu nguyen. Chu: \"choi dien thoai no khong ra full man\". Tat: config.ini [Ui] NeoTheoMep=0.",
             "static int\ts_nManHinhW = 0, s_nManHinhH = 0;\t// doc tu tep dang nap",
             "static int\ts_nNeoTheoMep = -1;",
             "static void DoiTheoManHinh(int* pnX, int* pnY)",
             "{",
             "\tint nW0 = (s_nManHinhW > 0) ? s_nManHinhW : 1040;",
             "\tint nH0 = (s_nManHinhH > 0) ? s_nManHinhH : 604;",
             "\tint nDX = SCREEN_WIDTH - nW0, nDY = SCREEN_HEIGHT - nH0;",
             "\tif (s_nNeoTheoMep < 0)",
             "\t\ts_nNeoTheoMep = GetPrivateProfileInt(\"Ui\", \"NeoTheoMep\", 1, \".\\\\config.ini\") ? 1 : 0;",
             "\tif (!s_nNeoTheoMep || SCREEN_WIDTH <= 0 || SCREEN_HEIGHT <= 0 || (nDX == 0 && nDY == 0))",
             "\t\treturn;",
             "\tif (*pnX * 10 >= nW0 * 7)",
             "\t\t*pnX += nDX;",
             "\tif (*pnY * 10 >= nH0 * 7)",
             "\t\t*pnY += nDY;",
             "}",
             "#endif"], "bien + DoiTheoManHinh")

# 2. NapTep: dong ManHinh + doi toa do truoc khi vao bang
s = thay(s, ["\t\tDatKhoa(p, nGiaTri[0], nGiaTri[1], nGiaTri[2], nGiaTri[3]);"],
            ["#ifdef JX_ANDROID",
             "\t\tif (strcmpi(p, \"ManHinh\") == 0)\t// %s khung ve luc luu tep nay" % DAU,
             "\t\t{",
             "\t\t\ts_nManHinhW = nGiaTri[0];",
             "\t\t\ts_nManHinhH = nGiaTri[1];",
             "\t\t\tcontinue;",
             "\t\t}",
             "\t\tDoiTheoManHinh(&nGiaTri[0], &nGiaTri[1]);",
             "#endif",
             "\t\tDatKhoa(p, nGiaTri[0], nGiaTri[1], nGiaTri[2], nGiaTri[3]);"], "NapTep")

# 3. moi tep nap xong -> quen co man hinh (tep sau khong co dong ManHinh thi ve mac dinh)
s = thay(s, ["\tfclose(pTep);", "\tg_DebugLog(\"[UITOADO] nap xong %s -> bang co %d muc\", szDuongDan, s_nSo);"],
            ["\tfclose(pTep);",
             "#ifdef JX_ANDROID",
             "\tg_DebugLog(\"[UITOADO] %s: tep thiet ke %dx%d, khung ve %dx%d\", \"" + DAU + "\", s_nManHinhW, s_nManHinhH, SCREEN_WIDTH, SCREEN_HEIGHT);",
             "\ts_nManHinhW = s_nManHinhH = 0;",
             "#endif",
             "\tg_DebugLog(\"[UITOADO] nap xong %s -> bang co %d muc\", szDuongDan, s_nSo);"], "NapTep cuoi")

# 4. GhiTepVao: ghi khung ve hien tai
s = thay(s, ["\tfprintf(pTep, \"%s\\n\", UITOADO_MUC);"],
            ["\tfprintf(pTep, \"%s\\n\", UITOADO_MUC);",
             "#ifdef JX_ANDROID",
             "\tfprintf(pTep, \"ManHinh=%d,%d\\n\", SCREEN_WIDTH, SCREEN_HEIGHT);\t// " + DAU,
             "#endif"], "GhiTepVao")

if cao(s) != cao(s0):
    raise SystemExit("so byte cao doi")
ghi(TEP, s)
print("da va:", TEP)
