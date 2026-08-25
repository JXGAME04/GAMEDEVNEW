# -*- coding: utf-8 -*-
r"""[25/08] CAU NOI danh hieu JX2 -> JX1 de danh hieu HIEN THAT tren nhan vat.

Ly do (xem fix_title_display.py): Title_ActiveTitle cua he JX2 KHONG dat
Npc[].m_btPlayerTitle - bien duy nhat client dung de ve danh hieu. Duong dat bien do
la SetPlayerTitle(id, nTime, overlook) cua he JX1, tra bang settings\PlayerTitle.txt
theo DONG = id + 1. Da them 9 dong id 287-295 mang dung ten Bang Chien.

Sua 2 noi:
 1. script\item\hoatdong_admin.lua : menu test goi them SetPlayerTitle(id JX1)
 2. script\missions\tongwar\match\head.lua : quan ham trong tran
    script\event\tongwar\head.lua        : danh hieu cuoi mua
"""
import io, os, re, shutil

E = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
ENC = "latin-1"
def rd(p): return io.open(p, "r", encoding=ENC, newline="").read()
def wr(p, s): io.open(p, "w", encoding=ENC, newline="").write(s)
def bak(p, t):
    q = p + ".truoc_" + t
    if not os.path.isfile(q): shutil.copyfile(p, q)

# ---------- 1) menu admin ----------
p = os.path.join(E, r"item\hoatdong_admin.lua")
s = rd(p)
NL = "\r\n" if "\r\n" in s else "\n"
if "TT_JX1ID" in s:
    print("1: menu da co cau noi")
else:
    old = ("function HD_TT_Rank(nRank)" + NL +
           "\tlocal nId = 100 + nRank - 1" + NL +
           "\tTitle_AddTitle(nId, 0, 9999999)" + NL +
           "\tTitle_ActiveTitle(nId)" + NL)
    assert s.count(old) == 1, "anchor HD_TT_Rank = %d" % s.count(old)
    s = s.replace(old,
        "-- [FIX 25/08] Title_ActiveTitle (he JX2) KHONG dat Npc[].m_btPlayerTitle - bien DUY NHAT" + NL +
        "-- client dung de VE danh hieu (KNpc.cpp:5647 PlayerSync.PlayerTitle). Phai goi THEM" + NL +
        "-- SetPlayerTitle cua he JX1 (ScriptFuns.cpp:10702 -> KNpc::SetCurPlayerTitle:11006)." + NL +
        "-- Id HAI BANG mang nghia KHAC NHAU (105: JX2 = De Nhat Bang, JX1 = Hieu uy) nen phai" + NL +
        "-- tra bang duoi day; 9 dong 287-295 la dong MOI them vao settings\\PlayerTitle.txt." + NL +
        "TT_JX1ID = {[100]=287, [101]=288, [102]=289, [103]=290, [104]=291," + NL +
        "\t\t\t[105]=292, [106]=293, [199]=294, [3000]=295}" + NL + NL +
        "function HD_TT_Hien(nIdJx2, nTimeFrame)" + NL +
        "\tlocal nJx1 = TT_JX1ID[nIdJx2]" + NL +
        "\tif nJx1 then" + NL +
        "\t\tSetPlayerTitle(nJx1, nTimeFrame or 9999999, 0)" + NL +
        "\tend" + NL +
        "end" + NL + NL +
        "function HD_TT_Rank(nRank)" + NL +
        "\tlocal nId = 100 + nRank - 1" + NL +
        "\tTitle_AddTitle(nId, 0, 9999999)" + NL +
        "\tTitle_ActiveTitle(nId)" + NL +
        "\tHD_TT_Hien(nId)\t-- [FIX 25/08] de danh hieu HIEN tren dau nhan vat" + NL, 1)

    old2 = ("function HD_TT_Cap(nId, nNgay, szTen)" + NL +
            "\tTitle_AddTitle(nId, 1, nNgay * 24 * 60 * 60 * 18)" + NL +
            "\tTitle_ActiveTitle(nId)" + NL)
    assert s.count(old2) == 1, "anchor HD_TT_Cap = %d" % s.count(old2)
    s = s.replace(old2, old2 + "\tHD_TT_Hien(nId, nNgay * 24 * 60 * 60 * 18)\t-- [FIX 25/08]" + NL, 1)

    # go: xoa ca danh hieu he JX1
    old3 = "\tRemoveSkillState(661)" + NL
    assert s.count(old3) == 1
    s = s.replace(old3, "\tRemovePlayerTitle()\t-- [FIX 25/08] go danh hieu he JX1 (m_btPlayerTitle)" + NL + old3, 1)
    bak(p, "cauoi_2508"); wr(p, s)
    print("1: menu admin da noi he JX1 (hien danh hieu that)")

# ---------- 2) quan ham trong tran ----------
p = os.path.join(E, r"missions\tongwar\match\head.lua")
s = rd(p)
NL = "\r\n" if "\r\n" in s else "\n"
if "FIX 25/08" in s:
    print("2: match/head.lua da va")
else:
    old = "\tTitle_AddTitle(campnum + rank - 1, 0, 9999999)"
    assert s.count(old) == 1, "anchor quan ham = %d" % s.count(old)
    s = s.replace(old, old + NL +
        "\t-- [FIX 25/08] he JX2 khong dat m_btPlayerTitle nen danh hieu KHONG HIEN tren nhan vat;" + NL +
        "\t-- goi them he JX1 (bang settings\\PlayerTitle.txt, dong 288-292 = id 287-291)." + NL +
        "\tSetPlayerTitle(287 + rank - 1, 9999999, 0)", 1)
    bak(p, "cauoi_2508"); wr(p, s)
    print("2: match/head.lua da noi quan ham sang he JX1")

# ---------- 3) danh hieu cuoi mua ----------
p = os.path.join(E, r"event\tongwar\head.lua")
s = rd(p)
NL = "\r\n" if "\r\n" in s else "\n"
if "FIX 25/08" in s:
    print("3: event/tongwar/head.lua da va")
else:
    old = "\tTitle_AddTitle(n_title, 2, nTime);"
    assert s.count(old) == 1, "anchor danh hieu mua = %d" % s.count(old)
    s = s.replace(old, old + NL +
        "\t-- [FIX 25/08] noi sang he JX1 de danh hieu HIEN that (105->292, 106->293)." + NL +
        "\tif (n_title == 105) then" + NL +
        "\t\tSetPlayerTitle(292, nTime, 0)" + NL +
        "\telseif (n_title == 106) then" + NL +
        "\t\tSetPlayerTitle(293, nTime, 0)" + NL +
        "\tend", 1)
    bak(p, "cauoi_2508"); wr(p, s)
    print("3: event/tongwar/head.lua da noi danh hieu cuoi mua")

print("XONG")
