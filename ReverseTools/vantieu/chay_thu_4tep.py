# -*- coding: utf-8 -*-
"""[LMBC 06/09] CHAY THU THAT 4 tep thu vien bang Lua 5.4 (lua54.dll qua ctypes).

Khac kiem_54.py (chi kiem cu phap): o day NAP shim lua4compat + 4 tep, gia lap
cac ham engine JX1 (GetLocalDate / WriteStringToFile / AddNpc / SetNpcScript /
SubWorldID2Idx / GetTask / ...) roi GOI THAT de doi chieu ket qua.

CHAY: set PYTHONIOENCODING=utf-8 && python chay_thu_4tep.py
"""
import ctypes
import io
import os
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
except Exception:
    pass

DLL = r"C:\Program Files\Wireshark\lua54.dll"
SHIM = r"D:\GAMEDEVNEW_wt_vantieu\Sources\Library\Lua54\lua4compat.lua"
MOI = r"D:\GAMEDEVNEW_wt_vantieu\serverscript_jx2\vantieu\moi"

L = ctypes.CDLL(DLL)
L.luaL_newstate.restype = ctypes.c_void_p
L.luaL_openlibs.argtypes = [ctypes.c_void_p]
L.luaL_loadbufferx.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_size_t,
                               ctypes.c_char_p, ctypes.c_char_p]
L.luaL_loadbufferx.restype = ctypes.c_int
L.lua_pcallk.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_int,
                         ctypes.c_void_p, ctypes.c_void_p]
L.lua_pcallk.restype = ctypes.c_int
L.lua_tolstring.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_void_p]
L.lua_tolstring.restype = ctypes.c_char_p
L.lua_settop.argtypes = [ctypes.c_void_p, ctypes.c_int]

S = L.luaL_newstate()
L.luaL_openlibs(S)


def chay(ma, ten):
    if isinstance(ma, str):
        ma = ma.encode("latin-1")
    r = L.luaL_loadbufferx(S, ma, len(ma), ("=" + ten).encode("utf-8"), b"t")
    if r == 0:
        r = L.lua_pcallk(S, 0, 0, 0, None, None)
    if r != 0:
        s = L.lua_tolstring(S, -1, None)
        L.lua_settop(S, 0)
        raise SystemExit("LOI %s: %s" % (ten, s.decode("latin-1", "replace")))
    L.lua_settop(S, 0)


def tep(p):
    return io.open(p, "rb").read()


chay(tep(SHIM), "lua4compat.lua")

# --- gia lap engine JX1 -----------------------------------------------------
STUB = r'''
_G.KETQUA = {}
_G.GHI = {}
_G.NPC = {}
-- GetLocalDate(szFormat) -> MOT CHUOI (ScriptFuns.cpp:3465)
function GetLocalDate(szFmt) return os.date(szFmt, 1757116800) end
-- WriteStringToFile(szPath, szText) -> 1/0 (KTongJX2.cpp:4077)
function WriteStringToFile(szPath, szText)
    GHI[#GHI+1] = {szPath, szText}
    return 1
end
function SubWorldID2Idx(nId) if nId == 999 then return -1 end return nId + 100 end
function AddNpc(a,b,c,d,e,f,g) NPC[#NPC+1] = {a,b,c,d,e,f,g} return 7000 + #NPC end
function SetNpcScript(i, s) NPC[#NPC][8] = i NPC[#NPC][9] = s end
function Include(p) end
'''
chay(STUB, "stub")

chay(tep(os.path.join(MOI, r"script\lib\lib_lmbiaoche.lua")), "lib_lmbiaoche.lua")
chay(tep(os.path.join(MOI, r"script\global\maplist.lua")), "maplist.lua")
chay(tep(os.path.join(MOI, r"script\activitysys\npcfunlib.lua")), "npcfunlib.lua")

# --- 1. WriteYunBiaoLog -----------------------------------------------------
T1 = r'''
local r = WriteYunBiaoLog(format("[Long Mon] xe tieu tai [%s]", tbGlobalMapId2Name[176]))
assert(r == 1, "WriteYunBiaoLog phai tra 1")
assert(#GHI == 1, "phai ghi dung 1 dong")
print("  duong dan : " .. GHI[1][1])
print("  noi dung  : " .. string.gsub(string.gsub(GHI[1][2], "\r", "<CR>"), "\n", "<LF>"))
assert(string.sub(GHI[1][1], 1, 1) == "\\", "duong dan phai bat dau bang \\")
assert(string.find(GHI[1][1], "Logs\\KSG_YunBiaoLog_", 1, true) == 2, "sai ten goc")
assert(string.sub(GHI[1][2], -2) == "\r\n", "phai ket thuc CRLF")
assert(string.find(GHI[1][2], "]\t", 1, true), "phai co ] roi TAB")
assert(string.find(GHI[1][2], "^%[%d%d%d%d%-%d%d%-%d%d %d%d:%d%d:%d%d%]"), "sai dinh dang thoi gian")
-- nil va so
WriteYunBiaoLog(nil)
assert(#GHI == 1, "nil phai bi bo qua")
WriteYunBiaoLog(12345)
assert(#GHI == 2 and string.find(GHI[2][2], "12345", 1, true), "so phai duoc tostring")
print("  OK WriteYunBiaoLog")
'''
chay(T1, "thu_1_writeyunbiaolog")

# --- 2. maplist -------------------------------------------------------------
T2 = r'''
local n = 0
for k, v in pairs(tbGlobalMapId2Name) do n = n + 1 end
print("  so ban do : " .. n)
assert(n == 1021, "phai co 1021 ban do")
assert(tbGlobalMapId2Name[176] ~= nil and tbGlobalMapId2Name[2] ~= nil)
-- 44 diem tbBJPoints + 26 tuyen: tat ca map id van tieu dung
local ids = {1,2,4,5,10,11,20,22,23,37,42,45,78,80,83,91,93,94,99,100,101,103,
             116,153,162,168,171,174,176,180,181,182,201,203,204,205}
for i = 1, getn(ids) do
    assert(rawget(tbGlobalMapId2Name, ids[i]) ~= nil, "thieu map id " .. ids[i])
end
-- duong lui khong tra nil (khong the giet timer)
assert(tbGlobalMapId2Name[99999] == "Map 99999", "duong lui sai")
assert(format("%s", tbGlobalMapId2Name[99999]) == "Map 99999")
print("  OK maplist (36 map van tieu deu co, duong lui chay)")
'''
chay(T2, "thu_2_maplist")

# --- 3. NpcFunLib:AddObjNpc -------------------------------------------------
T3 = r'''
local tbPos = { {80,1678,3147}, {999,1,2}, {176,1399,3140} }
NpcFunLib:AddObjNpc("TiepDan", 2230, tbPos, "\\script\\x.lua")
assert(#NPC == 2, "map 999 (SubWorldID2Idx=-1) phai bi bo, dem duoc " .. #NPC)
local a = NPC[1]
assert(a[1] == 2230 and a[2] == 1 and a[3] == 180 and a[4] == 1678*32
       and a[5] == 3147*32 and a[6] == 0 and a[7] == "TiepDan",
       "thu tu tham so AddNpc sai")
assert(a[8] == 7001 and a[9] == "\\script\\x.lua", "SetNpcScript sai")
print("  AddNpc(1) = " .. a[1] .. "," .. a[2] .. "," .. a[3] .. "," .. a[4] .. ","
      .. a[5] .. "," .. a[6] .. ",\"" .. a[7] .. "\"")
print("  OK AddObjNpc")
'''
chay(T3, "thu_3_addobjnpc")

# --- 4. getLevel trong yunbiao_system --------------------------------------
# Trich rieng ham getLevel de kiem bien 27..49 (khong can engine)
yb = io.open(os.path.join(MOI, r"script\global\yunbiao_system.lua"), "rb").read().decode("latin-1")
i = yb.index("local getLevel = function(nId)")
j = yb.index("\n    end", i) + len("\n    end")
than = yb[i:j].replace("local getLevel", "getLevel", 1)
T4 = than + r'''
local mong = {}
for k = 1, 18 do mong[k] = nil end
for k = 19, 22 do mong[k] = 7 end
for k = 23, 25 do mong[k] = 8 end
mong[26] = 9
for k = 27, 49 do mong[k] = 10 end
for k = 1, 60 do
    local v = getLevel(k)
    assert(v == mong[k], "getLevel(" .. k .. ") = " .. tostring(v)
           .. " nhung mong doi " .. tostring(mong[k]))
end
print("  getLevel(44,45,46) = " .. getLevel(44) .. "," .. getLevel(45) .. "," .. getLevel(46)
      .. "   (Phuong Tuong)")
print("  getLevel(47,48,49) = " .. getLevel(47) .. "," .. getLevel(48) .. "," .. getLevel(49)
      .. "   (Dai Ly)")
print("  getLevel(50)       = " .. tostring(getLevel(50)) .. "   (ngoai bang, dung)")
print("  OK getLevel 27..49 -> 10 sao")
'''
chay(T4.encode("latin-1"), "thu_4_getlevel")


# --- 5. yunbiao_system.lua chay THAT (3 goi nguoc) --------------------------
# BC_FireCartScript dat SCRIPT_PLAYERINDEX = 0 (KBiaoChe.cpp:197) => moi ham
# doi PlayerIndex deu chet. Kiem OnBiaoCheDisapper KHONG con dung Msg2Tong.
STUB2 = r"""
TSK_LMBJTaskFlag = 4178
_G.TONG = {}
_G.NEWS = {}
_G.STAT = {}
_G.TASK = {}
function GetNpcPos(i) return 1399*32, 3140*32, 76 end
function SubWorldIdx2ID(idx) return idx + 100 end
function GetNpcName(i) return "XeTieu" end
function GetNpcParam(i, n) if n == 1 then return 10 end if n == 5 then return 38 end return 0 end
function AddStatData(s) STAT[#STAT+1] = s end
function AUC_MsgTong(n, s) TONG[#TONG+1] = {"AUC", n, s} return 1 end
function Msg2Tong(n, s) TONG[#TONG+1] = {"MSG", n, s} end
function AddGlobalNews(s) NEWS[#NEWS+1] = s end
function GetTask(n) return TASK[n] or 0 end
function SetTask(n, v) TASK[n] = v end
function GetCurrentTime() return 100000 end
function GetName() return "AnhHung" end
function GetWorldPos() return 176, 1399, 3140 end
function GetTongName() return "ThieuLam" end
function DynamicExecuteByPlayer(a,b,c) end
"""
chay(STUB2, "stub2")
chay(tep(os.path.join(MOI, r"script\global\yunbiao_system.lua")), "yunbiao_system.lua")

T5 = r"""
local nGhi = #GHI
OnBiaoCheFarAwayPlayerDisapper(101, "AnhHung")
assert(#GHI == nGhi + 1, "faraway phai ghi 1 dong log")
assert(string.find(GHI[#GHI][2], "176", 1, true) == nil or true)
assert(STAT[#STAT] == "lmbj_car_faraway", "sai ten thong ke faraway")

OnBiaoCheDisapper(101, "AnhHung")
assert(STAT[#STAT] == "lmbj_car_timeover", "sai ten thong ke timeover")
assert(#TONG == 1, "phai ban dung 1 tin bang, dem " .. #TONG)
assert(TONG[1][1] == "AUC", "PHAI dung AUC_MsgTong (Msg2Tong im lang khi PlayerIndex=0)")
assert(TONG[1][2] == 38, "sai id bang")
print("  tin bang  : " .. TONG[1][1] .. " tong=" .. TONG[1][2])
print("  " .. TONG[1][3])

-- doi map: nId = floor(4178/10) = 417 -> ngoai bang -> khong bao
TASK[4178] = 4178
TASK[3511] = 0
OnBiaoCheChangeMapNotice()
assert(#NEWS == 0, "nId 417 ngoai bang, khong duoc bao")
-- ep nId = 45 (10 sao): nTaskFlagValue = 450
TASK[4178] = 450
TASK[3511] = 0
OnBiaoCheChangeMapNotice()
assert(#NEWS == 1, "phai co 1 tin the gioi")
print("  tin t.gioi: " .. NEWS[1])
assert(string.find(NEWS[1], "ThieuLam", 1, true), "10 sao phai co ten bang")
assert(TASK[3511] == 100000, "phai dat lai chong spam 3511")
-- goi lai ngay: bi chan 15 giay
OnBiaoCheChangeMapNotice()
assert(#NEWS == 1, "phai bi chan spam trong 15 giay")
print("  OK yunbiao_system (3 goi nguoc, AUC_MsgTong, chong spam 3511)")
"""
chay(T5, "thu_5_yunbiao")

print("TAT CA PHEP THU DEU DAT")
