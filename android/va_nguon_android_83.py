# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 ICON f] Chu: "kem them ten rut gon cua npc o duoi icon do". Ve ten NPC (cat con 12 ky tu + "..") ngay duoi
# icon doi thoai, chu 12 px vien den, can giua theo icon. IconNpcGan mac dinh 130: chu muon NGAN vi nhieu NPC dung gan nhau (11/09).

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 11/09 ICON f]"


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


def va_can(s):
    s = thay(s, ["static int\ts_nIconGan = 220;\t// [ANDROID 11/09 ICON e] NPC cach nhan vat <= bao nhieu px (man hinh) thi hien icon. [Cham] IconNpcGan"],
             ["static int\ts_nIconGan = 130;\t// [ANDROID 11/09 ICON e] NPC cach nhan vat <= bao nhieu px (man hinh) thi hien icon. [Cham] IconNpcGan. " + DAU + " 260"],
             "s_nIconGan")
    s = thay(s, ['\ts_nIconGan   = GetPrivateProfileInt("Cham", "IconNpcGan", 220, szCfg);\t// [ANDROID 11/09 ICON e]'],
             ['\ts_nIconGan   = GetPrivateProfileInt("Cham", "IconNpcGan", 130, szCfg);\t// [ANDROID 11/09 ICON e] ' + DAU],
             "doc IconNpcGan")
    s = thay(s, ["\t\ts_nIconVeY0 = nTY - nCao / 2 - 8;",
                 "\t\ts_nIconVeY1 = nTY + nCao / 2 + 8;",
                 "\t\ts_uIconVeLuc = (unsigned int)GetTickCount();",
                 "\t}",
                 "}"],
                ["\t\ts_nIconVeY0 = nTY - nCao / 2 - 8;",
                 "\t\ts_nIconVeY1 = nTY + nCao / 2 + 8;",
                 "\t\ts_uIconVeLuc = (unsigned int)GetTickCount();",
                 "\t\tif (bGan)",
                 "\t\t{\t// " + DAU + " ten NPC rut gon (toi da 12 ky tu + \"..\") ngay duoi icon, can giua",
                 "\t\t\tchar szTen[32];",
                 "\t\t\tint nDai;",
                 "",
                 "\t\t\tstrncpy(szTen, s_szIconTen, sizeof(szTen) - 1);",
                 "\t\t\tszTen[sizeof(szTen) - 1] = 0;",
                 "\t\t\tnDai = (int)strlen(szTen);",
                 "\t\t\tif (nDai > 14)",
                 "\t\t\t{",
                 "\t\t\t\tszTen[12] = '.'; szTen[13] = '.'; szTen[14] = 0;",
                 "\t\t\t\tnDai = 14;",
                 "\t\t\t}",
                 "\t\t\tif (nDai > 0)",
                 "\t\t\t\tg_pRepresentShell->OutputText(12, szTen, nDai, nTX - nDai * 31 / 10, nTY + nCao / 2 + 3, 0xFFFFE28A,",
                 "\t\t\t\t\t0, TEXT_IN_SINGLE_PLANE_COORD, 0xFF000000);",
                 "\t\t}",
                 "\t}",
                 "}"], "ten duoi icon")
    return s


def va_config(s):
    if "IconNpcGan=220" not in s:
        raise SystemExit("config: khong co IconNpcGan=220")
    return s.replace("IconNpcGan=220", "IconNpcGan=130")


va("Sources/S3Client/Platform/JxCanDieuKhien.cpp", va_can)
for p in ["android/du_lieu_ghi_de/config.ini", "D:/jx1_android_data/config.ini"]:
    if os.path.isfile(p):
        s = doc(p)
        if "IconNpcGan=220" in s:
            ghi(p, va_config(s))
            print("da va:", p)
print("xong")
