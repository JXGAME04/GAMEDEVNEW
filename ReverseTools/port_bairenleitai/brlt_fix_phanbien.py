# -*- coding: utf-8 -*-
r"""[BAIREN 23/08 - hau phan bien] 2 va:
F3: RemoteExc thong bao toan server chet - g_IniScriptEngine KHONG nap scriptjx2\lib thanh
    state (chi \script + \scriptjx2\tong_vn) nen RemoteExecute("\script\lib\remoteexc.lua",
    "ReceiveExc") -> g_GetScript NULL ca 2 duong. Fix: CHEP remoteexc.lua sang script\lib\
    (tu nap thanh state luc boot; Include tu state khac van tim thay duong that).
F4: newworld.lua (map 960) thieu Include common.lua (SPEC P1 co) -> split nil trong
    PraseParam -> 1 dong ScriptError o luot vao map dau tien sau boot.
"""
import io, os, shutil

E   = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIR = r"D:\GAMEDEVNEW\serverscript_jx2\bairenleitai"

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s):
    d = os.path.dirname(p)
    if d and not os.path.isdir(d): os.makedirs(d)
    io.open(p, "w", encoding="latin-1", newline="").write(s)

# F3: chep remoteexc.lua
src = os.path.join(E, r"scriptjx2\lib\remoteexc.lua")
dst = os.path.join(E, r"script\lib\remoteexc.lua")
assert os.path.isfile(src)
if not os.path.isfile(dst):
    shutil.copyfile(src, dst)
    print("F3: da chep script\\lib\\remoteexc.lua")
else:
    print("F3: script\\lib\\remoteexc.lua da co - bo qua")
d = os.path.join(MIR, r"script\lib")
if not os.path.isdir(d): os.makedirs(d)
shutil.copyfile(dst, os.path.join(d, "remoteexc.lua"))

# F4: Include common.lua vao newworld.lua map 960
p = os.path.join(E, r"script\missions\bairenleitai\newworld.lua")
s = rd(p)
if 'common.lua' in s:
    print("F4: da co Include common.lua - bo qua")
else:
    NL = "\r\n" if "\r\n" in s else "\n"
    old = 'Include("\\\\script\\\\maps\\\\newworldscript_default.lua")'
    assert s.count(old) == 1
    s = s.replace(old,
        'Include("\\\\script\\\\lib\\\\common.lua")\t-- [BAIREN 23/08 phan bien F4] split() cho PraseParam (SPEC P1)' + NL + old, 1)
    wr(p, s)
    print("F4: da them Include common.lua")
shutil.copyfile(p, os.path.join(MIR, r"script\missions\bairenleitai\newworld.lua"))
print("XONG")
