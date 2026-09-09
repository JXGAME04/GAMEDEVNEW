# -*- coding: utf-8 -*-
"""goi_va_napchieu_b_0909.py - [NAPCHIEU 09/09 b] Do rieng hieu qua nap truoc: KIP / TRE.

bo_ve toan cuc khong tach duoc anh chieu (anh than NPC chiem gan het). Them co m_bNapTruoc vao muc do
NapTruoc chen; LAN HOI DAU TIEN (GetImage) cua muc do:
  - da nap xong (m_pTextureRes != NULL)  -> kip  (nap truoc co ich: khung dau cua dan co hinh)
  - con dang nap (m_bDangNap)            -> tre  (nap truoc chua ke kip; ve hoac hoi dong bo nhu cu)
  - nap hong                             -> khong dem
In them vao [NAPCHIEU]: "| lan dung dau: kip N, tre N" (ky 30 s, PaintLog=1). Chi Represent3, Core khong doi.
Cong tac A/B: [Client] Rep3NapChieu = 1 (mac dinh bat; 0 = Rep3_NapTruoc tra 0 ngay, khong dem) - de chu so sanh
trai nghiem bat/tat ma khong dong den luong nap nen (Rep3NapNen).
"""
import io
import sys

NL = "\r\n"
T = "\t"
R3 = "D:/GAMEDEVNEW_wt_delta/Sources/Represent/Represent3/"
F_TRM_H = R3 + "TextureResMgr.h"
F_TRM_C = R3 + "TextureResMgr.cpp"
F_SH_C = R3 + "KRepresentShell3.cpp"
TAG = "[NAPCHIEU 09/09 b]"


def doc(p):
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    return s, s.count("\r\n"), s.count("\n") - s.count("\r\n"), sum(1 for c in s if ord(c) >= 0x80)


def ghi(p, s, crlf0, lf0, h0, ten):
    ok = (s.count("\n") - s.count("\r\n") == lf0) if crlf0 else (s.count("\r\n") == 0)
    if not ok or sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s:
        print("FAIL ma hoa " + ten); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("OK " + ten)


def rep(s, old, new, ten):
    n = s.count(old)
    if n != 1:
        print("FAIL neo %s: %d" % (ten, n)); sys.exit(1)
    return s.replace(old, new)


# ---------------------------------------------------------------- TextureResMgr.h: co + 2 bien dem
s, c0, l0, h0 = doc(F_TRM_H)
if TAG not in s:
    old = T + "ResNode() : m_bDangNap(false), m_nLanHong(0) {}" + T + "// [NAP 08/09 b/e]"
    new = T + "ResNode() : m_bDangNap(false), m_nLanHong(0), m_bNapTruoc(false) {}" + T + "// [NAP 08/09 b/e] " + TAG
    s = rep(s, old, new, "TRM.h ctor ResNode")
    old = "m_bDangNap;" + T*4 + "// [NAP 08/09 b] dang nap o luong nen (m_pTextureRes NULL tam thoi)"
    new = old + NL + T + "bool" + T*2 + "m_bNapTruoc;" + T*4 + "// " + TAG + " muc do NapTruoc chen, chua duoc hoi lan nao (lan hoi dau: kip/tre)"
    s = rep(s, old, new, "TRM.h co")
    old = T + "unsigned m_nNapNenGui, m_nNapNenXong, m_nNapNenHong, m_nNapNenBoVe;" + T + "// thong ke ky ([REP3-NAP])"
    new = old + NL + T + "unsigned m_nNapTruocKip, m_nNapTruocTre;" + T + "// " + TAG + " lan hoi dau cua muc nap truoc: da nap xong / con dang nap"
    s = rep(s, old, new, "TRM.h bien dem")
    ghi(F_TRM_H, s, c0, l0, h0, "TextureResMgr.h")
else:
    print("TextureResMgr.h da co")

# ---------------------------------------------------------------- TextureResMgr.cpp: khoi tao, dat co, dem
s, c0, l0, h0 = doc(F_TRM_C)
if TAG not in s:
    old = "m_bVeDangDien = false; m_nNapNenGui = 0; m_nNapNenXong = 0; m_nNapNenHong = 0; m_nNapNenBoVe = 0;" + T + "// [NAP 08/09 b]"
    new = old + NL + T + "m_nNapTruocKip = 0; m_nNapTruocTre = 0;" + T + "// " + TAG
    s = rep(s, old, new, "TRM.cpp ctor")
    old = T + "node.m_bDangNap = true;" + NL + T + "node.m_bCacheable = true;"
    new = T + "node.m_bDangNap = true;" + NL + T + "node.m_bNapTruoc = true;" + T + "// " + TAG + NL + T + "node.m_bCacheable = true;"
    s = rep(s, old, new, "TRM.cpp NapTruoc dat co")
    old = T*3 + "m_TextureResList[nImagePosition].m_nLastUsedTime = GetTickCount();" + NL + \
          T*3 + "if (m_TextureResList[nImagePosition].m_bDangNap)" + T + "// [NAP 08/09 b] dang nap o luong nen"
    new = NL.join([
        T*3 + "m_TextureResList[nImagePosition].m_nLastUsedTime = GetTickCount();",
        T*3 + "if (m_TextureResList[nImagePosition].m_bNapTruoc)" + T + "// " + TAG + " lan hoi dau tien cua muc nap truoc",
        T*3 + "{",
        T*4 + "m_TextureResList[nImagePosition].m_bNapTruoc = false;",
        T*4 + "if (m_TextureResList[nImagePosition].m_bDangNap) m_nNapTruocTre++;",
        T*4 + "else if (m_TextureResList[nImagePosition].m_pTextureRes) m_nNapTruocKip++;",
        T*3 + "}",
        T*3 + "if (m_TextureResList[nImagePosition].m_bDangNap)" + T + "// [NAP 08/09 b] dang nap o luong nen",
    ])
    s = rep(s, old, new, "TRM.cpp GetImage dem")
    ghi(F_TRM_C, s, c0, l0, h0, "TextureResMgr.cpp")
else:
    print("TextureResMgr.cpp da co")

# ---------------------------------------------------------------- KRepresentShell3.cpp: in kip/tre vao [NAPCHIEU]
s, c0, l0, h0 = doc(F_SH_C)
if TAG not in s:
    old = 'Rep3Log("[NAPCHIEU] nap truoc anh chieu: goi %u | da co %u, giao nen %u, khong %u", g_uRep3NapTruoc[0] + g_uRep3NapTruoc[1] + g_uRep3NapTruoc[2], g_uRep3NapTruoc[1], g_uRep3NapTruoc[2], g_uRep3NapTruoc[0]);'
    new = 'Rep3Log("[NAPCHIEU] nap truoc anh chieu (Rep3NapChieu=%d): goi %u | da co %u, giao nen %u, khong %u | lan dung dau: kip %u, tre %u", g_nRep3NapChieu, g_uRep3NapTruoc[0] + g_uRep3NapTruoc[1] + g_uRep3NapTruoc[2], g_uRep3NapTruoc[1], g_uRep3NapTruoc[2], g_uRep3NapTruoc[0], m_TextureResMgr.m_nNapTruocKip, m_TextureResMgr.m_nNapTruocTre);' + T + "// " + TAG + NL + \
          T*3 + "m_TextureResMgr.m_nNapTruocKip = 0; m_TextureResMgr.m_nNapTruocTre = 0;"
    s = rep(s, old, new, "SH.cpp log")
    # cong tac Rep3NapChieu: bien, doc ini, kiem trong NapTruoc
    old = "static unsigned g_uRep3NapTruoc[3] = { 0, 0, 0 };" + T + "// [NAPCHIEU 09/09] ket qua NapTruoc: [0] khong, [1] da co, [2] giao nen"
    new = old + NL + "int g_nRep3NapChieu = 1;" + T + "// " + TAG + " cong tac [Client] Rep3NapChieu: 1 = nap truoc anh chieu khi nhan goi 95 (mac dinh), 0 = tat (A/B)"
    s = rep(s, old, new, "SH.cpp bien cong tac")
    old = T + 'g_nRep3ChuGiuMs  = Rep3Ini("Rep3ChuGiuMs", 12);' + T + "// [CHUGIU 09/09]"
    new = old + NL + T + 'g_nRep3NapChieu  = Rep3Ini("Rep3NapChieu", 1) ? 1 : 0;' + T + "// " + TAG
    s = rep(s, old, new, "SH.cpp doc ini")
    old = T + "const int n = m_TextureResMgr.NapTruoc(pszImage, ISI_T_SPR);"
    new = T + "if (!g_nRep3NapChieu) return 0;" + T + "// " + TAG + " cong tac tat: khong nap truoc, khong dem" + NL + old
    s = rep(s, old, new, "SH.cpp cong tac trong NapTruoc")
    ghi(F_SH_C, s, c0, l0, h0, "KRepresentShell3.cpp")
else:
    print("KRepresentShell3.cpp da co")
print("XONG NAPCHIEU b")
