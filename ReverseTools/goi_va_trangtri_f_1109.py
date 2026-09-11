# -*- coding: utf-8 -*-
r"""goi_va_trangtri_f_1109.py - [TRANGTRI 11/09 f] chu: "the dong vat che luon dong vat -
bo the cua dong vat di".

Anh chu chup: con ga nho (~40 px) bi che boi chong THANH MAU + THE TEN + bieu tuong
trang thai. Ca ba thu deu duoc ve cho MOI NPC, khong loc theo loai:
  - CoreDrawGameObj.cpp:28-54 (lop IPOT_RL_INFRONTOF_ALL): PaintBlood, PaintChat,
    PaintLife (khi CheckShowLife), PaintInfo (khi CheckShowName).
  - KNpc::Paint:9198 `DrawMenuState(nHeight)` = bieu tuong tron vang.
Loai NPC doc tu du lieu map la dung (GameDataDef.h NPCKIND): buom/chuon chuon/se = 4
kind_bird, ga/ech/cho = 5 kind_mouse - nhung ban goc khong loc hai loai nay o duong ve the.

Sua: NPC trang tri (Npc[].m_sClientNpcID.m_dwRegionID > 0) thi BO HAN lop the:
khong thanh mau, khong chat, khong the ten, khong bieu tuong trang thai. Chi ve than.

Do them: g_uTTVe = so lan VE THAN cua NPC trang tri trong 10 s, vao dong [TRANGTRI].
Neu g_uTTVe > 0 ma van khong thay con vat thi loi nam o bo anh chu khong o lop the.
"""
import io
import sys

NL = "\r\n"
T = "\t"
TAG = "[TRANGTRI 11/09 f]"
D = "D:/GAMEDEVNEW_wt_delta/Sources/Core/Src/"


def doc(p):
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    return s, sum(1 for c in s if ord(c) >= 0x80), s.count("\n") - s.count("\r\n")


def ghi(p, s, h0, lf0, ten):
    if (s.count("\n") - s.count("\r\n")) != lf0 or sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s:
        print("FAIL ma hoa " + ten)
        sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("OK " + ten)


def rep(s, old, new, ten, n_mong=1):
    n = s.count(old)
    if n != n_mong:
        print("FAIL neo %s: %d (mong %d)" % (ten, n, n_mong))
        sys.exit(1)
    return s.replace(old, new)


# ---------------------------------------------- CoreDrawGameObj.cpp: bo lop the, dem lan ve than
P = D + "CoreDrawGameObj.cpp"
s, h0, lf0 = doc(P)
if TAG not in s:
    s = rep(s, T + "case CGOG_NPC:" + NL + T*2 + "if (uId > 0)" + NL + T*2 + "{" + NL,
            T + "case CGOG_NPC:" + NL + T*2 + "if (uId > 0)" + NL + T*2 + "{" + NL +
            T*3 + "// " + TAG + " NPC trang tri (ga, buom, chuon chuon...) cao chi vai chuc diem" + NL +
            T*3 + "// anh, bi chong THANH MAU + THE TEN + bieu tuong che kin. Lop the khong co y" + NL +
            T*3 + "// nghia gi voi chung (da khong the chon lam muc tieu) nen bo han, chi ve than." + NL +
            T*3 + "if (Npc[uId].m_sClientNpcID.m_dwRegionID > 0)" + NL +
            T*3 + "{" + NL +
            T*4 + "if ((nParam & IPOT_RL_OBJECT) == IPOT_RL_OBJECT)" + NL +
            T*4 + "{" + NL +
            T*5 + "extern unsigned g_uTTVe;" + NL +
            T*5 + "g_uTTVe++;" + NL +
            T*5 + "Npc[uId].Paint();" + NL +
            T*4 + "}" + NL +
            T*4 + "break;" + NL +
            T*3 + "}" + NL, "C bo the")
    ghi(P, s, h0, lf0, "CoreDrawGameObj.cpp")
else:
    print("CoreDrawGameObj.cpp da co")

# ---------------------------------------------- KNpc.cpp: khong ve bieu tuong trang thai
P = D + "KNpc.cpp"
s, h0, lf0 = doc(P)
if TAG not in s:
    s = rep(s, T + "//End add by Fong Kieu" + NL + T + "DrawMenuState(nHeight);" + NL,
            T + "//End add by Fong Kieu" + NL +
            T + "if (m_sClientNpcID.m_dwRegionID == 0)" + T + "// " + TAG + " NPC trang tri khong co bieu tuong trang thai" + NL +
            T*2 + "DrawMenuState(nHeight);" + NL, "N bieu tuong")
    ghi(P, s, h0, lf0, "KNpc.cpp")
else:
    print("KNpc.cpp da co")

# ---------------------------------------------- KRegion.cpp: bien dem
P = D + "KRegion.cpp"
s, h0, lf0 = doc(P)
if TAG not in s:
    s = rep(s, "unsigned g_uTTNpcThieuAnh = 0;",
            "unsigned g_uTTVe = 0;" + T + "// " + TAG + " so lan VE THAN cua NPC trang tri trong 10 s" + NL +
            "unsigned g_uTTNpcThieuAnh = 0;", "R dem ve")
    ghi(P, s, h0, lf0, "KRegion.cpp")
else:
    print("KRegion.cpp da co")

# ---------------------------------------------- KSubWorldSet.cpp: in them
P = D + "KSubWorldSet.cpp"
s, h0, lf0 = doc(P)
if TAG not in s:
    s = rep(s, T*3 + "extern unsigned g_uTTNpcThieuAnh;" + T + "// [KHOI 11/09 c]" + NL,
            T*3 + "extern unsigned g_uTTNpcThieuAnh;" + T + "// [KHOI 11/09 c]" + NL +
            T*3 + "extern unsigned g_uTTVe;" + T + "// " + TAG + NL, "SW extern ve")
    s = rep(s, "| obj them %u | khoi ve %u bo qua %u | map %s |",
            "| obj them %u ve_than %u | khoi ve %u bo qua %u | map %s |", "SW dinh dang ve")
    s = rep(s, T*6 + "g_uTTNpcThem, g_uTTNpcHong, g_uTTNpcThieuAnh, g_uTTNpcXoa, g_uTTObjThem, g_uKhoiVe, g_uKhoiBoQua," + NL,
            T*6 + "g_uTTNpcThem, g_uTTNpcHong, g_uTTNpcThieuAnh, g_uTTNpcXoa, g_uTTObjThem, g_uTTVe, g_uKhoiVe, g_uKhoiBoQua," + NL, "SW tham so ve")
    s = rep(s, T*3 + "g_uTTNpcThieuAnh = 0; g_uKhoiVe = 0; g_uKhoiBoQua = 0;",
            T*3 + "g_uTTNpcThieuAnh = 0; g_uKhoiVe = 0; g_uKhoiBoQua = 0; g_uTTVe = 0;", "SW dat lai ve")
    ghi(P, s, h0, lf0, "KSubWorldSet.cpp")
else:
    print("KSubWorldSet.cpp da co")

print("XONG " + TAG)
