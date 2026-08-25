# -*- coding: utf-8 -*-
"""Sinh script\\tinhnang\\3hoatdong\\hd3_reload.lua - NAP LAI NHANH toan bo script
cua 3 hoat dong bang MOT NUT tren lenh bai admin.

Vi sao khong dung lenh GM "?gm RLAS": TextGMFilter (KGMCommand.cpp) chi duoc GOI
trong KPlayerChat.cpp:467 khi bien dich co #ifdef _CHAT_SCRIPT_OPEN - macro nay
KHONG duoc dinh nghia trong Core.vcxproj lan bat ky header nao => moi lenh "?gm"
KHONG HOAT DONG tren ban build hien tai. (Bang GM_Command van nam trong DLL vi no
la du lieu tinh, nen grep thay chuoi nhung lenh van vo tac dung.)

Thay vao do dung ham Lua ReLoadScript(<duong dan>) - CO dang ky (ScriptFuns.cpp
{"ReLoadScript", LuaReLoadScript}) va chinh HD3_DriverInit da dung. Sinh san danh
sach duong dan de nap lai tung tep.
"""
import io, os, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"
REL = r"script\tinhnang\3hoatdong\hd3_reload.lua"

THUMUC = [
    r"script\task\tollgate\killer",
    r"script\missions\fengling_ferry",
    r"script\missions\challengeoftime",
    r"script\vng_feature\challengeoftime",
    r"script\tinhnang\3hoatdong",
]
TEPLE = [
    r"script\task\tollgate\killbosshead.lua",
    r"script\global\autoexec_npc_hd3.lua",
    r"script\header\cauhinh_hoatdong.lua",
    r"script\item\hd3_admin.lua",
    r"settings\trigger_include.lua",
    r"settings\trigger_challengeoftime.lua",
]

tep = []
for t in THUMUC:
    d = os.path.join(SRV, t)
    if not os.path.isdir(d):
        continue
    for dp, dn, fs in os.walk(d):
        for f in sorted(fs):
            if f.endswith(".lua"):
                tep.append(os.path.relpath(os.path.join(dp, f), SRV))
for t in TEPLE:
    if os.path.exists(os.path.join(SRV, t)) and t not in tep:
        tep.append(t)
tep = [t for t in tep if not t.endswith("hd3_reload.lua")]
print("so tep se nap lai:", len(tep))

BS = chr(92)
L = []
A = L.append
A("-- ============================================================================")
A("-- HD3_RELOAD.LUA - SINH TU DONG boi ReverseTools/gen_hd3_reload.py - DUNG SUA TAY")
A("-- Nap lai NHANH toan bo script cua 3 hoat dong ma KHONG can restart server.")
A("--")
A("-- Vi sao khong dung lenh GM '?gm RLAS': TextGMFilter chi duoc goi khi bien dich")
A("-- co #ifdef _CHAT_SCRIPT_OPEN - macro nay KHONG duoc dinh nghia o dau trong du an")
A("-- => moi lenh '?gm' vo tac dung. Ham Lua ReLoadScript thi CO dang ky va dung duoc.")
A("--")
A("-- Dung: bam Lenh bai admin -> muc 'Nap lai toan bo script'.")
A("-- ============================================================================")
A("")
A("HD3_RELOAD_LIST = {")
for t in tep:
    A('\t"' + BS + BS + t.replace("\\", BS + BS) + '",')
A("}")
A("")
A("function HD3_ReloadAll()")
A("\tlocal n = 0")
A("\tfor i = 1, getn(HD3_RELOAD_LIST) do")
A("\t\tReLoadScript(HD3_RELOAD_LIST[i])")
A("\t\tn = n + 1")
A("\tend")
A("\tif (HD_NapLaiCauHinh ~= nil) then")
A("\t\tHD_NapLaiCauHinh()")
A("\tend")
A("\treturn n")
A("end")
A("")
body = "\r\n".join(L)
p = os.path.join(SRV, REL)
io.open(p, "w", encoding="latin-1", newline="").write(body)
dst = os.path.join(MIR, REL)
os.makedirs(os.path.dirname(dst), exist_ok=True)
io.open(dst, "w", encoding="latin-1", newline="").write(body)
print("da sinh", REL)
