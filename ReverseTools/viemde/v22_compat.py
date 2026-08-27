# -*- coding: utf-8 -*-
"""VIEM DE - lop dem cho vai ham ban Linux co ma JX1 goi ten khac.

Hien co DUNG MOT ham: GetItemBindState (yandiduihuan.lua:128).
  ban Linux : GetItemBindState(nItemIndex)      -> trang thai khoa cua vat pham
  ban JX1   : GetPlayerItemIsLock(nItemIndex)   -> Item[idx].GetPlayerItemLock()
Da doc ma engine (ScriptFuns LuaGetPlayerItemIsLock): tra ve DUNG cai truong ma
LuaHD3_SetItemBindState ghi (Item[idx].SetPlayerItemLock) - tuc la 1:1, khong
phai xap xi. thoren.lua:359 cua chinh JX1 dang dung no theo nghia do (== -2 la khoa).

Sinh 1 tep + chen 1 dong Include vao yandiduihuan.lua (tep do do v09 sinh lai
tu ban Linux nen bo va nay phai chay SAU v09).
"""
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from bangtxt import uni2tcvn

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
GUONG = r"D:\GAMEDEVNEW\serverscript_jx2\viemde"
DICH = r"\script\missions\yandibaozang\ydbz_compat.lua"
DICH_INC = '\\script\\missions\\yandibaozang\\ydbz_compat.lua'
TEP_VA = SRV + r"\script\missions\yandibaozang\yandiduihuan.lua"
NHAN = "[JX1 26/08 compat]"

L = [
    "-- ==========================================================================",
    "-- VIEM DE - LOP DEM: ham ban Linux co ma JX1 goi ten khac",
    "-- SINH TU DONG boi ReverseTools\\viemde\\v22_compat.py - DUNG SUA TAY",
    "-- ==========================================================================",
    "",
    "-- Ban Linux: GetItemBindState(nItemIndex)",
    "-- Ban JX1  : GetPlayerItemIsLock(nItemIndex) - da doc ma engine",
    "--            (ScriptFuns LuaGetPlayerItemIsLock -> Item[idx].GetPlayerItemLock()),",
    "--            tra ve DUNG truong ma SetItemBindState ghi vao. Anh xa 1:1.",
    "if GetItemBindState == nil then",
    "\tfunction GetItemBindState(nItemIndex)",
    "\t\tif GetPlayerItemIsLock == nil then",
    "\t\t\treturn 0",
    "\t\tend",
    "\t\treturn GetPlayerItemIsLock(nItemIndex) or 0",
    "\tend",
    "end",
    "",
]


def main():
    noi_dung = "\r\n".join(uni2tcvn(x) for x in L)
    for goc in (SRV, GUONG):
        p = goc + DICH
        os.makedirs(os.path.dirname(p), exist_ok=True)
        tam = p + ".dangghi"
        with open(tam, "wb") as f:
            f.write(noi_dung.encode("latin-1"))
        os.replace(tam, p)
    print("da ghi ydbz_compat.lua (%d dong)" % len(L))

    # chen Include vao yandiduihuan.lua
    d = io.open(TEP_VA, encoding="latin-1", newline="").read()
    if NHAN in d:
        print("yandiduihuan.lua: da chen Include roi.")
        return
    # chen ngay sau dong Include DAU TIEN de bao dam nap truoc khi dung
    dong = d.split("\r\n")
    vt = None
    for i, l in enumerate(dong):
        if l.strip().startswith("Include(") or l.strip().startswith("IncludeLib("):
            vt = i
    if vt is None:
        print("!! khong tim thay dong Include nao trong yandiduihuan.lua - DUNG LAI")
        return
    dong.insert(vt + 1, '-- %s bo sung GetItemBindState (JX1 goi ten khac)' % NHAN)
    dong.insert(vt + 2, 'Include("%s")' % DICH_INC.replace("\\", "\\\\"))
    d2 = "\r\n".join(dong)
    tam = TEP_VA + ".dangghi"
    with open(tam, "wb") as f:
        f.write(d2.encode("latin-1"))
    os.replace(tam, TEP_VA)
    # guong
    q = GUONG + r"\script\missions\yandibaozang\yandiduihuan.lua"
    os.makedirs(os.path.dirname(q), exist_ok=True)
    io.open(q, "w", encoding="latin-1", newline="").write(d2)
    print("da chen Include vao yandiduihuan.lua (sau dong %d)" % (vt + 1))

    b = open(TEP_VA, "rb").read()
    print("KIEM: FFFD=%d, LF-don=%d" % (b.count(b"\xef\xbf\xbd"), b.count(b"\n") - b.count(b"\r\n")))
    assert b.count(b"\n") - b.count(b"\r\n") == 0


main()
