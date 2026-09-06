#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""kiem_thanhvien.py - nap THAT 22 tep nhanh ca nhan bang Lua 5.4 (Lua54Dll) roi doi chieu
xem con thanh vien LongMenBiaoJu.* nao bi doc ma van nil khong.

Include duoc giai quyet theo 3 goc, uu tien: cay gop (lua54 + moi) -> cay chay that.
Moi bien toan cuc chua co deu tra ve mot "vat gia" vua goi duoc vua tra chi so duoc,
de AutoFunctions:Add / EventSys:GetType(..):Reg(..) khong chet.
"""
import ctypes
import os
import re
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
except Exception:
    pass

HERE = os.path.dirname(os.path.abspath(__file__))
# Cay GOP = cay lua54 da chuyen + cac tep MOI cua dot port, gop vao mot thu muc tam.
import shutil
import tempfile
GOC54 = os.path.join("D:\\", "GAMEDEVNEW_wt_vantieu", "serverscript_jx2", "vantieu", "lua54", "script")
GOCMOI = os.path.join("D:\\", "GAMEDEVNEW_wt_vantieu", "serverscript_jx2", "vantieu", "moi", "script")
_TAM = os.path.join(tempfile.gettempdir(), "lmbj_gop", "script")
if os.path.isdir(os.path.dirname(_TAM)):
    shutil.rmtree(os.path.dirname(_TAM))
shutil.copytree(GOC54, _TAM)
for dp, dn, fn in os.walk(GOCMOI):
    rel = os.path.relpath(dp, GOCMOI)
    dst = os.path.join(_TAM, rel) if rel != "." else _TAM
    os.makedirs(dst, exist_ok=True)
    for f in fn:
        shutil.copy2(os.path.join(dp, f), os.path.join(dst, f))
for dp, dn, fn in os.walk(_TAM):
    for f in fn:
        if f.endswith(".truoc_lmbj"):
            os.remove(os.path.join(dp, f))
GOP = _TAM.replace("\\", "/")
LIVE = r"E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/server/script"
DLL = r"D:\GAMEDEVNEW_wt_vantieu\Lib\lua54\x64\Lua54Dll.dll"

LMBJ = os.path.join(GOP, "event", "longmenbiaoju")
TEP22 = sorted(f for f in os.listdir(LMBJ) if f.endswith(".lua"))

# --- 1. thu thap moi ten bi DOC -------------------------------------------------------
pat = re.compile(rb"LongMenBiaoJu[.:]([A-Za-z_][A-Za-z0-9_]*)")
ten = set()
for root in (LMBJ, os.path.join(GOP, "activitysys", "config", "129")):
    if not os.path.isdir(root):
        continue
    for f in os.listdir(root):
        if f.endswith(".lua"):
            ten |= {m.group(1).decode() for m in pat.finditer(open(os.path.join(root, f), "rb").read())}
ten = sorted(ten)
print("so thanh vien LongMenBiaoJu.* bi doc trong ma:", len(ten))

# --- 2. nap that ----------------------------------------------------------------------
os.environ["LUA54_BO_KIEM"] = "1"
D = ctypes.CDLL(DLL)
D.lua4_open.restype = ctypes.c_void_p
D.lua4_open.argtypes = [ctypes.c_int]
D.lua4_baselibopen.argtypes = [ctypes.c_void_p]
D.lua4_dostring.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
D.lua4_dostring.restype = ctypes.c_int
D.lua4_settop.argtypes = [ctypes.c_void_p, ctypes.c_int]

STUB = r'''
LOI_DS = {}
_ALERT = function(m) LOI_DS[getn(LOI_DS) + 1] = tostring(m) end
_ERRORMESSAGE = function(m) _ALERT("error: " .. tostring(m)) end

-- vat gia: goi duoc, lay chi so duoc, tu tra ve chinh no
GIA = {}
setmetatable(GIA, {
    __index = function(t, k) return GIA end,
    __call  = function(...) return GIA end,
    __newindex = function(t, k, v) end,
})
-- [SUA 06/09 kiem chung] LongMenBiaoJu PHAI thay nil: head.lua dung loi viet
-- nha JX1 "LongMenBiaoJu = LongMenBiaoJu or {}"; neu vat gia tra ve GIA thi
-- __newindex cua GIA nuot sach moi phep gan va bo kiem bao nham.
setmetatable(_G, {__index = function(t, k)
    if k == "LongMenBiaoJu" then return nil end
    return GIA
end})

-- [SUA 06/09 kiem chung] BO DEDUPE - Include cua JX1 KHONG dedupe:
-- Sources/Core/Src/ScriptFuns.cpp:2061 goi lua_dofile MOI LAN (chi IncludeLib
-- moi require-once, :2628-2638). Ban co dedupe che mat loi "head.lua reset
-- LongMenBiaoJu = {}" lam mat 8 lop luc boot.
DA_NAP = {}
SAU = 0
function Include(p)
    local q = gsub(p, strchar(92), "/")
    q = gsub(q, "^/script", "")
    DA_NAP[q] = (DA_NAP[q] or 0) + 1
    SAU = SAU + 1
    if SAU > 60 then _ALERT("QUA SAU " .. q) SAU = SAU - 1 return end
    local fn = loadfile(GOP .. q)
    if not fn then fn = loadfile(LIVE .. q) end
    if not fn then _ALERT("THIEU TEP " .. q) SAU = SAU - 1 return end
    local ok, er = pcall(fn)
    if not ok then _ALERT("Include " .. q .. ": " .. tostring(er)) end
    SAU = SAU - 1
end
IncludeLib = function() end
'''

L = D.lua4_open(200)
D.lua4_baselibopen(L)


def run(src, nhan):
    r = D.lua4_dostring(L, src.encode("latin-1"))
    D.lua4_settop(L, 0)
    if r != 0:
        print("!! %s -> ma %d" % (nhan, r))
    return r


run('GOP = "%s" LIVE = "%s"' % (GOP, LIVE), "duong dan")
run(STUB, "stub")

for f in TEP22:
    run('Include("\\\\script\\\\event\\\\longmenbiaoju\\\\%s")' % f, f)

# --- 3. doi chieu ---------------------------------------------------------------------
lua_ten = "{" + ",".join('"%s"' % t for t in ten) + "}"
run(
    'DS = %s\n'
    'THIEU = {}\n'
    'for i = 1, getn(DS) do\n'
    '  if rawget(LongMenBiaoJu, DS[i]) == nil then\n'
    '    THIEU[getn(THIEU) + 1] = DS[i]\n'
    '  end\n'
    'end\n'
    'BAOCAO = "THIEU(" .. getn(THIEU) .. "): " .. table.concat(THIEU, ", ")\n' % lua_ten,
    "doi chieu",
)

# lay ket qua ve
D.lua4_getglobal = D.lua4_getglobal
D.lua4_getglobal.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
D.lua4_tostring.argtypes = [ctypes.c_void_p, ctypes.c_int]
D.lua4_tostring.restype = ctypes.c_char_p
D.lua4_getglobal(L, b"BAOCAO")
v = D.lua4_tostring(L, -1)
print(v.decode("latin-1") if v else "(khong lay duoc BAOCAO)")
D.lua4_settop(L, 0)

# in loi da gom
run('BAOCAO = table.concat(LOI_DS, "\\n")', "gom loi")
D.lua4_getglobal(L, b"BAOCAO")
v = D.lua4_tostring(L, -1)
print("\n--- loi/canh bao khi nap ---")
print(v.decode("latin-1") if v else "(khong co)")
D.lua4_settop(L, 0)
D.lua4_close = D.lua4_close
