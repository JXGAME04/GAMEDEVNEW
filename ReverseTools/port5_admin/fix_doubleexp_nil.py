# -*- coding: utf-8 -*-
r"""[25/08] CHAN BOM NIL o he x2 exp (Tong Kim + Vuot Ai) sau khi hoat dong 41 bi tat.

Chuoi: battlehead.lua:711  point = Songjin_checkdoubleexp(point)
       -> DynamicExecuteByPlayer("...\\config\\41\\extend.lua", "pActivity:DoubleExp_Songjin", nExp)
       -> thu muc 41 da doi ten (41.KHONG_DUNG_3HD) => g_GetScript tra NULL
          (ScriptFuns.cpp:2354-2358 "script chua nap, bo qua") => Lua nhan NIL
       -> nExp = NIL -> X2SongJin(nil): IsActive false tra nil / true thi nil*2 = LOI
       -> point = nil giua tran TONG KIM => mat diem / loi script.
Cung chuoi ben missions\challengeoftime\doubleexp.lua (Vuot Ai).

VA PHONG NIL trung tinh - KHONG doi hanh vi khi extend nap duoc, KHONG bat lai
hoat dong 41 (y do tat cua phien 3HD giu nguyen):
    local nRet = DynamicExecuteByPlayer(...)
    if nRet then nExp = nRet end
"""
import io, os, shutil

E = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
ENC = "latin-1"

VIEC = [
    (r"battles\doubleexp.lua",
     "\tfor i=1, getn(%tbDouble) do\r\n\t\tlocal tbfunc = %tbDouble[i]\r\n\t\tnExp = DynamicExecuteByPlayer(PlayerIndex, tbfunc[1], tbfunc[2], nExp)\r\n\tend",
     "\tfor i=1, getn(%tbDouble) do\r\n\t\tlocal tbfunc = %tbDouble[i]\r\n"
     "\t\t-- [FIX 25/08] extend.lua cua hoat dong 41 da bi tat (doi ten thu muc) => DynamicExecuteByPlayer\r\n"
     "\t\t-- tra NIL (engine bo qua script chua nap, ScriptFuns.cpp:2354). Khong phong thi nExp = nil\r\n"
     "\t\t-- chay tiep vao X2SongJin/battlehead:711 lam MAT DIEM tran Tong Kim. Phong nil, giu nguyen\r\n"
     "\t\t-- hanh vi khi extend con nap duoc.\r\n"
     "\t\tlocal nRet = DynamicExecuteByPlayer(PlayerIndex, tbfunc[1], tbfunc[2], nExp)\r\n"
     "\t\tif nRet then\r\n\t\t\tnExp = nRet\r\n\t\tend\r\n\tend"),
    (r"missions\challengeoftime\doubleexp.lua", None, None),  # xu ly rieng ben duoi (co the khac ten ham)
]

# ---- battles\doubleexp.lua ----
p = os.path.join(E, VIEC[0][0])
s = io.open(p, "r", encoding=ENC, newline="").read()
if "FIX 25/08" in s:
    print("battles/doubleexp: da va")
else:
    cu = VIEC[0][1]
    if cu not in s:
        cu = cu.replace("\r\n", "\n")
        moi = VIEC[0][2].replace("\r\n", "\n")
    else:
        moi = VIEC[0][2]
    assert cu in s, "anchor battles/doubleexp khong khop"
    bak = p + ".truoc_nilfix_2508"
    if not os.path.isfile(bak): shutil.copyfile(p, bak)
    io.open(p, "w", encoding=ENC, newline="").write(s.replace(cu, moi, 1))
    print("battles/doubleexp: DA VA phong nil")

# ---- challengeoftime\doubleexp.lua ----
p = os.path.join(E, r"missions\challengeoftime\doubleexp.lua")
s = io.open(p, "r", encoding=ENC, newline="").read()
if "FIX 25/08" in s:
    print("challengeoftime/doubleexp: da va")
else:
    NL = "\r\n" if "\r\n" in s else "\n"
    cu = ("\t\tnExp = DynamicExecuteByPlayer(PlayerIndex, tbfunc[1], tbfunc[2], nExp)")
    assert s.count(cu) == 1, "anchor COT = %d" % s.count(cu)
    moi = ("\t\t-- [FIX 25/08] phong nil nhu battles\\doubleexp.lua (hoat dong 41 da tat)" + NL +
           "\t\tlocal nRet = DynamicExecuteByPlayer(PlayerIndex, tbfunc[1], tbfunc[2], nExp)" + NL +
           "\t\tif nRet then" + NL + "\t\t\tnExp = nRet" + NL + "\t\tend")
    bak = p + ".truoc_nilfix_2508"
    if not os.path.isfile(bak): shutil.copyfile(p, bak)
    io.open(p, "w", encoding=ENC, newline="").write(s.replace(cu, moi, 1))
    print("challengeoftime/doubleexp: DA VA phong nil")
print("XONG")
