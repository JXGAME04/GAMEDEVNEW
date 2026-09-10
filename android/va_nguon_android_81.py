# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 ICON d] Chu: "icon nay thay cho icon giao tiep ban moi bo vao" - anh doi_thoai.spr (kho VNKU
# \spr\UiNew\UiToolsControlBar\doi_thoai.spr, 86x86, 2 khung: trang / vang) thay nut "Giao tiep". Ve 44 px, mep tren cach
# chan NPC 100 px (icon nam tren dau), hai khung nhap nhay 0,5 s. Anh da chep vao lop ghi de spr/uinew/uitoolscontrolbar/.

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 11/09 ICON d]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def nl_cua(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def thay(s, cu, moi, ten, so=1):
    nl = nl_cua(s)
    c = nl.join(cu)
    if s.count(c) == so:
        return s.replace(c, nl.join(moi))
    raise SystemExit("khong tim thay dung %d cho (%d): %s" % (so, s.count(c), ten))


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


def va(p, ham):
    s = doc(p)
    if DAU in s:
        print("da va roi, bo qua:", p)
        return
    s2 = ham(s)
    if cao(s) != cao(s2):
        raise SystemExit("so byte cao doi o " + p)
    ghi(p, s2)
    print("da va:", p)


ANH_CU = '"\\\\spr\\\\UiNew\\\\InterRact\\\\giao_tiep.spr"'
ANH_MOI = '"\\\\spr\\\\UiNew\\\\UiToolsControlBar\\\\doi_thoai.spr"'


def va_can(s):
    s = thay(s, ['static char\ts_szIconAnh[128] = ' + ANH_CU + ';\t// [ANDROID 11/09 ICON b] nut "Giao tiep" cua kho VNKU (224x50)'],
             ['static char\ts_szIconAnh[128] = ' + ANH_MOI + ';\t// ' + DAU + ' icon "doi thoai" 86x86, 2 khung (kho VNKU) - chu chon thay nut "Giao tiep"'],
             "anh mac dinh")
    s = thay(s, ["static int\ts_nIconRong = 112;\t\t// [ANDROID 11/09 ICON b] be rong ve icon (px); cao theo ti le anh. [Cham] IconNpcRong"],
             ["static int\ts_nIconRong = 44;\t\t// [ANDROID 11/09 ICON b] be rong ve icon (px); cao theo ti le anh. [Cham] IconNpcRong. " + DAU + " 44 cho doi_thoai"],
             "rong mac dinh")
    s = thay(s, ['\ts_nIconCao   = GetPrivateProfileInt("Cham", "IconNpcCao", 72, szCfg);\t// [ANDROID 11/09 ICON b] mep TREN cua icon cao hon chan NPC bao nhieu',
                 '\ts_nIconRong  = GetPrivateProfileInt("Cham", "IconNpcRong", 112, szCfg);\t// [ANDROID 11/09 ICON b]'],
                ['\ts_nIconCao   = GetPrivateProfileInt("Cham", "IconNpcCao", 100, szCfg);\t// [ANDROID 11/09 ICON b] mep TREN cua icon cao hon chan NPC bao nhieu. ' + DAU + ' 100 = icon 44 px nam tren dau',
                 '\ts_nIconRong  = GetPrivateProfileInt("Cham", "IconNpcRong", 44, szCfg);\t// [ANDROID 11/09 ICON b] ' + DAU],
                "doc ini cao/rong")
    s = thay(s, ["static void VeAnhRong(const char* pszAnh, int nX, int nY, int nRong, int nCao)"],
             ["static void VeAnhRong(const char* pszAnh, int nX, int nY, int nRong, int nCao, int nKhung = 0)\t// " + DAU + " nKhung"],
             "VeAnhRong chu ky")
    s = thay(s, ["\ta.nFrame = 0;",
                 "\tstrncpy(a.szImage, pszAnh, sizeof(a.szImage) - 1);",
                 "\ta.oPosition.nX = nX;",
                 "\ta.oPosition.nY = nY;",
                 "\ta.oPosition.nZ = 0;",
                 "\ta.oEndPos.nX = nX + nRong;"],
                ["\ta.nFrame = nKhung;",
                 "\tstrncpy(a.szImage, pszAnh, sizeof(a.szImage) - 1);",
                 "\ta.oPosition.nX = nX;",
                 "\ta.oPosition.nY = nY;",
                 "\ta.oPosition.nZ = 0;",
                 "\ta.oEndPos.nX = nX + nRong;"], "VeAnhRong khung")
    s = thay(s, ["\tVeAnhRong(s_szIconAnh, x - nRong / 2, y, nRong, nCao);"],
             ["\tVeAnhRong(s_szIconAnh, x - nRong / 2, y, nRong, nCao, (int)(((unsigned int)GetTickCount() / 500) % (unsigned int)KyNang_SoKhung(s_szIconAnh)));\t// " + DAU + " 2 khung nhap nhay 0,5 s"],
             "ve icon nhap nhay")
    return s


def va_config(s):
    s = thay(s, ['IconNpcAnh=\\spr\\UiNew\\InterRact\\giao_tiep.spr', 'IconNpcRong=112', 'IconNpcCao=72'],
             ['; ' + DAU + ' doi_thoai.spr (86x86, 2 khung) - chu chon thay nut "Giao tiep"',
              'IconNpcAnh=\\spr\\UiNew\\UiToolsControlBar\\doi_thoai.spr', 'IconNpcRong=44', 'IconNpcCao=100'], "config icon")
    return s


va("Sources/S3Client/Platform/JxCanDieuKhien.cpp", va_can)
va("android/du_lieu_ghi_de/config.ini", va_config)
p = "D:/jx1_android_data/config.ini"
if os.path.isfile(p):
    s = doc(p)
    if DAU not in s:
        ghi(p, va_config(s))
        print("da va: config song")
print("xong")
