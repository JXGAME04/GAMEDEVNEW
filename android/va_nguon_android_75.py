# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 b] Sau khi chu chay APK va 74 trong Tong Kim: "con lag rung luon man hinh", "vong sang vien nhay o o ky nang
# lam lai cho vong mong thoi ... va cho luan chuyen nhanh hon".
#
# 1. MAILBOX mac dinh TAT (Rep3GpuMailbox=0): MAILBOX khong cho vblank nen nhip trinh chieu khong con khop 60 Hz -> khung
#    hien 2 lan / bo 1 khung theo chu ky = "rung" khi cuon man hinh. Giu ma + cong tac de A/B; VSYNC (nhu truoc) lam nhip.
# 2. Can dieu khien: mac dinh ve DICH GAN 2 buoc nhu Goto() cu (CanBuocXa=2) nhung chi gui toi da 1 lan / 1 tick
#    (CanGacTick=1, ~18 goi/giay thay vi 125..1000) - hanh vi client/may chu Y NHU CU (NPC chi buoc 1 lan/tick), chi bo
#    goi trung trong cung tick. Dich xa 8 buoc + cong gac 5 tick (nhu chuot PC) van co: CanBuocXa=8, CanGacTick=5.
#    JxCore_GotoHuong: tham so thu 4 doi nghia: nGac = so tick toi thieu giua hai lan gui (0 = gui ngay).
# 3. Luan chuyen vong sang: LuanChuyenMs mac dinh 500 -> 300 ms.
# 4. Vong xoay tren o vong sang: anh moi vong MONG (android/lam_vong_xoay_mong.py) thay cho effect_skill.spr day.
#
# Chay lai vo hai. Doc/ghi latin-1, giu CRLF, chi them/sua dong ASCII.

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 11/09 b]"


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


def va_coreshell(s):
    s = thay(s, ['extern "C" int JxCore_GotoHuong(int nDir, int mode, int nBuoc, int bEp)'],
             ['// %s tham so 4: nGac = so tick toi thieu ke tu lan gui truoc (m_nSendMoveFrames); 0 = gui ngay.' % DAU,
              'extern "C" int JxCore_GotoHuong(int nDir, int mode, int nBuoc, int nGac)'], "CoreShell.cpp: chu ky JxCore_GotoHuong")
    s = thay(s, ["\tif (!bEp && Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames < defMAX_PLAYER_SEND_MOVE_FRAME)"],
             ["\tif (nGac > 0 && Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames < nGac)"], "CoreShell.cpp: cong gac")
    return s


def va_can(s):
    s = thay(s, ['extern "C" int\t\tJxCore_GotoHuong(int nDir, int mode, int nBuoc, int bEp);'],
             ['extern "C" int\t\tJxCore_GotoHuong(int nDir, int mode, int nBuoc, int nGac);'], "JxCanDieuKhien.cpp: khai bao")
    s = thay(s, ["static int\ts_nBuocXa = 8;"],
             ["static int\ts_nBuocXa = 2;", "static int\ts_nGacTick = 1;\t\t// %s [Cham] CanGacTick: gui toi da 1 lan / n tick (1 = ~18 goi/giay; chuot PC = 5)" % DAU],
             "JxCanDieuKhien.cpp: static s_nBuocXa")
    s = thay(s, ['\ts_nBuocXa   = GetPrivateProfileInt("Cham", "CanBuocXa", 8, szCfg);'],
             ['\ts_nBuocXa   = GetPrivateProfileInt("Cham", "CanBuocXa", 2, szCfg);',
              '\ts_nGacTick  = GetPrivateProfileInt("Cham", "CanGacTick", 1, szCfg);\t// %s' % DAU],
             "JxCanDieuKhien.cpp: doc CanBuocXa")
    s = thay(s, ["\tif (s_nBuocXa > 30) s_nBuocXa = 30;"],
             ["\tif (s_nBuocXa > 30) s_nBuocXa = 30;",
              "\tif (s_nGacTick < 1) s_nGacTick = 1;\t// %s" % DAU,
              "\tif (s_nGacTick > 10) s_nGacTick = 10;"], "JxCanDieuKhien.cpp: kep CanBuocXa")
    s = thay(s, ["\t\tJxCore_GotoHuong(s_nHuongGui, 0, 2, 1);"], ["\t\tJxCore_GotoHuong(s_nHuongGui, 0, 2, 0);"], "JxCanDieuKhien.cpp: dung lai")
    s = thay(s, ["\tif (JxCore_GotoHuong(s_nHuong, 0, s_nBuocXa, bEp))"],
             ["\tif (JxCore_GotoHuong(s_nHuong, 0, s_nBuocXa, bEp ? 0 : s_nGacTick))\t// %s" % DAU], "JxCanDieuKhien.cpp: goi trong Nhip")
    s = thay(s, ["static int\t\t\ts_nKNLuanMs = 500;"], ["static int\t\t\ts_nKNLuanMs = 300;"], "JxCanDieuKhien.cpp: s_nKNLuanMs")
    s = thay(s, ['\ts_nKNLuanMs = GetPrivateProfileInt("Cham", "LuanChuyenMs", 500, szCfg);'],
             ['\ts_nKNLuanMs = GetPrivateProfileInt("Cham", "LuanChuyenMs", 300, szCfg);\t// %s chu muon nhanh hon 0,5 s' % DAU],
             "JxCanDieuKhien.cpp: doc LuanChuyenMs")
    return s


def va_shell3(s):
    return thay(s, ["int g_nRep3GpuMailbox = 1;\t// [ANDROID 11/09 MAILBOX] Android: mac dinh BAT"],
                ["int g_nRep3GpuMailbox = 0;\t// [ANDROID 11/09 MAILBOX] %s mac dinh TAT: nhip trinh chieu khong khop 60 Hz -> rung khi cuon; bat de A/B" % DAU],
                "KRepresentShell3.cpp: mac dinh mailbox")


def va_config(s):
    s = thay(s, ["Rep3GpuMailbox=1"], ["; %s tat mac dinh: chu thay rung man hinh khi bat" % DAU, "Rep3GpuMailbox=0"], "config: mailbox")
    s = thay(s, ["CanBuocXa=8"],
             ["; %s 2 = dich gan nhu ban cu (mac dinh); 8 + CanGacTick=5 = giong chuot PC (it goi nhat)" % DAU,
              "CanBuocXa=2",
              "; %s gui lenh di toi da 1 lan / n tick (1 tick ~ 55 ms). 1 = ~18 goi/giay (truoc day 125..1000); 5 = nhu chuot PC" % DAU,
              "CanGacTick=1",
              "; %s luan chuyen vong sang moi bao nhieu ms (chu muon nhanh hon 500)" % DAU,
              "LuanChuyenMs=300"], "config: CanBuocXa")
    return s


va("Sources/Core/Src/CoreShell.cpp", va_coreshell)
va("Sources/S3Client/Platform/JxCanDieuKhien.cpp", va_can)
va("Sources/Represent/Represent3/KRepresentShell3.cpp", va_shell3)
va("android/du_lieu_ghi_de/config.ini", va_config)
print("xong")
