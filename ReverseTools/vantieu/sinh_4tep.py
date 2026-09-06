# -*- coding: utf-8 -*-
"""[LMBC 06/09] Sinh 4 tep thu vien JX1 con thieu cho Long Mon Tieu Cuc.

CHAY:  set PYTHONIOENCODING=utf-8 && python sinh_4tep.py
GHI:   D:\\GAMEDEVNEW_wt_vantieu\\serverscript_jx2\\vantieu\\moi\\...

Moi tep ghi bang encoding="latin-1", newline="" -> byte TCVN3/GBK giu nguyen.
Ten ban do CHEP NGUYEN BYTE tu settings\\MapList.ini (da la TCVN3) nen KHONG
can vn_to_octal - khong sinh chu Viet moi.
"""
import io
import os
import re
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
except Exception:
    pass

BS = chr(92)          # mot dau backslash
Q = chr(34)           # dau nhay kep
GOC = r"D:\GAMEDEVNEW_wt_vantieu\serverscript_jx2\vantieu\moi"
MAPLIST_INI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\MapList.ini"
LUA54 = r"D:\GAMEDEVNEW_wt_vantieu\serverscript_jx2\vantieu\lua54"


def ghi(rel, noidung):
    p = os.path.join(GOC, rel)
    os.makedirs(os.path.dirname(p), exist_ok=True)
    if isinstance(noidung, str):
        noidung = noidung.encode("latin-1")
    # chuan hoa xuong dong CRLF (script JX1 dung CRLF)
    noidung = noidung.replace(b"\r\n", b"\n").replace(b"\n", b"\r\n")
    io.open(p, "wb").write(noidung)
    print("GHI %-46s %6d byte" % (rel, len(noidung)))


# ---------------------------------------------------------------------------
# 1. script\lib\lib_lmbiaoche.lua
# ---------------------------------------------------------------------------
lib = """-- =========================================================================
-- lib_lmbiaoche.lua   [LMBC 06/09]  thu vien Long Mon Tieu Cuc cho JX1
--
-- WriteYunBiaoLog(szMsg)
--   Ban Linux day la HAM ENGINE (chuoi "WriteYunBiaoLog" tai offset 2179155
--   trong D:\\ServerLinux\\server1\\jx_linux_y). Engine JX1 KHONG co ham nay
--   nen boc lai THUAN LUA o day.
--
--   Bang chung dinh dang, doc thang tu jx_linux_y:
--     offset 2137119  "Logs/KSG_YunBiaoLog"                     <- goc ten tep
--     offset 2184420  "[%04d-%02d-%02d %02d:%02d:%02d]__TAB__%s__CRLF__"   <- mau mot dong
--   Tep that ben Linux: D:\\ServerLinux\\server1\\Logs\\KSG_YunBiaoLog_20260806.txt
--     -> engine noi them "_YYYYMMDD.txt", tuc XOAY THEO NGAY. Ta lam y het.
--
-- HAI HAM ENGINE JX1 DUNG O DAY (da doc nguon xac nhan chu ky):
--   GetLocalDate(szFormat) -> szKetQua
--     Sources\\Core\\Src\\ScriptFuns.cpp:3465. NHAN MOT CHUOI dinh dang
--     strftime va tra ve MOT CHUOI. KHONG PHAI bang 6 so nhu ban Linux.
--     Khi strftime that bai ham tra ve 0 gia tri (=> nil ben Lua), nen moi
--     cho goi deu phai co duong lui - loi Lua trong timer la CHET VINH VIEN.
--   WriteStringToFile(szDuongDan, szNoiDung) -> 1 / 0
--     Sources\\Core\\Src\\KTongJX2.cpp:4077. Mo che do "ab" (noi duoi), TU TAO
--     thu muc cha, dau '\\' o dau duong dan = GOC MAY CHU (khong phai goc o dia).
-- =========================================================================

-- Duong dan goc cua tep log (dau '\\' = goc may chu). Tep that se la
--   <goc may chu>\\Logs\\KSG_YunBiaoLog_YYYYMMDD.txt
LMBC_LOG_PREFIX   = "\\\\Logs\\\\KSG_YunBiaoLog_"
LMBC_LOG_SUFFIX   = ".txt"
LMBC_LOG_FMT_DAY  = "%Y%m%d"                -- strftime: ngay dat vao ten tep
LMBC_LOG_FMT_TIME = "%Y-%m-%d %H:%M:%S"     -- strftime: dau dong, = %04d-%02d-...

function WriteYunBiaoLog(szMsg)
\tif szMsg == nil then
\t\treturn 0;
\tend
\tif type(szMsg) ~= "string" then
\t\tszMsg = tostring(szMsg);
\tend

\t-- GetLocalDate tra ve MOT CHUOI, hoac nil khi strftime that bai.
\tlocal szDay = GetLocalDate(LMBC_LOG_FMT_DAY);
\tlocal szTime = GetLocalDate(LMBC_LOG_FMT_TIME);
\tif type(szDay) ~= "string" or szDay == "" then
\t\tszDay = "00000000";
\tend
\tif type(szTime) ~= "string" or szTime == "" then
\t\tszTime = "0000-00-00 00:00:00";
\tend

\t-- Mau ban Linux: "[<thoi gian>]<TAB><noi dung><CR><LF>"
\tlocal szLine = "[" .. szTime .. "]\\t" .. szMsg .. "\\r\\n";
\treturn WriteStringToFile(LMBC_LOG_PREFIX .. szDay .. LMBC_LOG_SUFFIX, szLine);
end
"""
lib = lib.replace("__TAB__", BS + "t").replace("__CRLF__", BS + "r" + BS + "n")
ghi(r"script\lib\lib_lmbiaoche.lua", lib)


# ---------------------------------------------------------------------------
# 2. script\global\maplist.lua  (sinh tu MapList.ini cua CAY CHAY THAT)
# ---------------------------------------------------------------------------
ini = io.open(MAPLIST_INI, "rb").read()
ten = {}
for m in re.finditer(rb"(?m)^(\d+)_name=([^\r\n]*)", ini):
    ten[int(m.group(1))] = m.group(2)
assert ten, "khong doc duoc MapList.ini"
for k, v in ten.items():
    assert Q.encode() not in v and BS.encode() not in v, "ten ban do %d co ky tu phai thoat" % k

dau = """-- =========================================================================
-- maplist.lua   [LMBC 06/09]  tbGlobalMapId2Name - ID ban do -> ten hien thi
--
-- SINH TU DONG tu settings\\MapList.ini cua CAY CHAY THAT
--   E:\\SourceTuanLe\\SourceVs22\\TESTLOFFF_ONLINE\\bin\\server\\settings\\MapList.ini
-- (lay dung cac dong "<id>_name="). Ten da la TCVN3 trong .ini nen chep
-- NGUYEN BYTE - khong sinh chu Viet moi.
--
-- Bo sinh: D:\\GAMEDEVNEW_wt_vantieu\\ReverseTools\\vantieu\\sinh_4tep.py (muc 2).
-- So ban do: __SOMAP__ (moi ban do co dinh nghia trong .ini deu co ten).
--
-- Long Mon Tieu Cuc doc bang nay o:
--   script\\global\\yunbiao_system.lua              (3 cho)
--   script\\activitysys\\config\\129\\extend.lua      (7 cho)
--   script\\activitysys\\config\\129\\npc_lmbiaoche.lua, npc_consigner.lua,
--   npc_receiver.lua, npc_lmbiaowu.lua
-- =========================================================================

tbGlobalMapId2Name = {
"""
dau = dau.replace("__SOMAP__", str(len(ten)))
than = []
for k in sorted(ten):
    than.append("\t[%d] = %s%s%s," % (k, Q, ten[k].decode("latin-1"), Q))
duoi = """}

-- Duong lui: ID ban do khong co trong MapList.ini (ban do dac biet do engine
-- tao) se tra ve mot chuoi thay vi nil. Ban Linux khong co doan nay, nhung
-- ben JX1 format("%s", nil) trong mot timer la LOI LUA => timer chet VINH VIEN
-- (xem MEMORY: "timer script chet vinh vien khi Lua loi"). Chi __index, khong
-- cham pairs/getn nen khong doi ngu nghia doc bang.
setmetatable(tbGlobalMapId2Name, {
\t__index = function(tb, nMapID)
\t\treturn "Map " .. tostring(nMapID);
\tend,
});
"""
ghi(r"script\global\maplist.lua", dau + "\n".join(than) + "\n" + duoi)


# ---------------------------------------------------------------------------
# 3. script\activitysys\npcfunlib.lua  (chi AddObjNpc)
# ---------------------------------------------------------------------------
npcfun = """-- =========================================================================
-- npcfunlib.lua   [LMBC 06/09]
--
-- CHI port phuong thuc NpcFunLib:AddObjNpc cua ban Linux
--   D:\\ServerLinux\\server1\\script\\activitysys\\npcfunlib.lua:237-247 (11 dong)
--
-- KHONG be ca 271 dong ban goc: cac phuong thuc con lai (Hide, CheckInMap,
-- CheckNpcSeries, DropAward, AddFightNpc...) can NPCINFO_GetSeries, HideNpc,
-- tbDropTemplet:GiveAwardByList va lib:CheckInList - JX1 khong co.
-- Da ra soat CA HAI nhanh van tieu, chi goi DUNG phuong thuc nay:
--   script\\activitysys\\config\\129\\config.lua:18  NpcFunLib:AddObjNpc
--   script\\activitysys\\config\\129\\config.lua:19  NpcFunLib:AddObjNpc
-- (hai dong AddDialogNpc trong config.lua da bi chu thich san tu ban goc).
--
-- Can them phuong thuc ve sau thi THEM VAO DAY, dung ghi de tep.
-- =========================================================================

if not NpcFunLib then
\tNpcFunLib = {};
end

-- AddNpc cua JX1: AddNpc(nNpcId, nLevel, nSubWorldIdx, nX32, nY32, nSeries, szName)
--   Sources\\Core\\Src\\ScriptFuns.cpp:7034 - DUNG THU TU tham so voi ban Linux
--   (khac AddNpcEx: ban Linux dao nSeries/nSubWorldIdx, xem chu thich
--    ScriptFuns.cpp truoc LuaAddNpcEx) nen chep nguyen 11 dong goc.
-- SubWorldID2Idx tra ve -1 khi ban do chua nap (ScriptFuns.cpp:542) nen
-- phep so sanh nMapIndex >= 0 luon hop le.
function NpcFunLib:AddObjNpc(szNpcName, nNpcId, tbNpcPos, szLuaFile)
\tfor i = 1, getn(tbNpcPos) do
\t\tlocal pPos = tbNpcPos[i];

\t\tlocal nMapIndex = SubWorldID2Idx(pPos[1]);
\t\tif nMapIndex >= 0 then
\t\t\tlocal nNpcIndex = AddNpc(nNpcId, 1, nMapIndex, pPos[2] * 32, pPos[3] * 32, 0, szNpcName);
\t\t\tSetNpcScript(nNpcIndex, szLuaFile);
\t\tend
\tend
end
"""
ghi(r"script\activitysys\npcfunlib.lua", npcfun)


# ---------------------------------------------------------------------------
# 4. script\global\yunbiao_system.lua  (port 75 dong + sua nId < 45 -> <= 49)
# ---------------------------------------------------------------------------
src = io.open(os.path.join(LUA54, r"script\global\yunbiao_system.lua"), "rb").read().decode("latin-1")

cu = "        elseif nId >= 27 and nId < 45 then -- 10"
assert cu in src, "khong tim thay dieu kien nId < 45"
i = src.index(cu)
j = src.index("\n", i)
dong_cu = src[i:j].rstrip("\r")
moi = (
    "        -- [LMBC 06/09] SUA: ban goc la 'nId >= 27 and nId < 45' -> mat loa 5\r\n"
    "        -- tuyen bang hoi 10 sao cuoi bang. tbBJPathLevel trong\r\n"
    "        -- script" + BS + "activitysys" + BS + "config" + BS + "129" + BS + "extend.lua co ID 27..49\r\n"
    "        -- (44,45,46 = Phuong Tuong; 47,48,49 = Dai Ly), nen phai la nId <= 49.\r\n"
    "        elseif nId >= 27 and nId <= 49 then " + dong_cu[dong_cu.index("--"):]
)
src = src[:i] + moi + src[j:]

# ghi chu them cho getLevel (bien cuc bo, khong phai ham thieu)
neo = "    local getLevel = function(nId)"
assert neo in src
src = src.replace(
    neo,
    "    -- [LMBC 06/09] getLevel la BIEN CUC BO giu mot ham vo danh (khong phai\r\n"
    "    -- ham engine bi thieu). Bang tbBiaoCheLevel bi chu thich o tren chi de\r\n"
    "    -- doi chieu; nguon that la pActivity.tbBJPathLevel ben extend.lua.\r\n"
    + neo,
    1,
)

# [LMBC-KIEM 06/09] SUA THEM: Msg2Tong dang 2 tham so doi PlayerIndex hop le
# (ScriptFuns.cpp:15199 LuaMsgToTong -> GetPlayerIndex, "if (nPlayerIndex <= 0)
# return 0"), ma BC_FireCartScript dat SCRIPT_PLAYERINDEX = 0 (KBiaoChe.cpp:197)
# => trong OnBiaoCheDisapper no IM LANG. Doi sang AUC_MsgTong(nTong, szMsg)
# (KAuctionServer.cpp:1001) - khong can PlayerIndex.
cu_mt = "\t    \tMsg2Tong(nTongId, szMsg);\r\n"
assert src.count(cu_mt) == 1, "khong tim thay dung 1 dong Msg2Tong"
src = src.replace(cu_mt, (
    "\t    \t-- [LMBC-KIEM 06/09] SUA: Msg2Tong(nTong, szMsg) dang 2 THAM SO lay\r\n"
    "\t    \t-- PlayerIndex TU STATE (ScriptFuns.cpp:15199 -> LuaMsgToTong, cau\r\n"
    "\t    \t-- 'nPlayerIndex = GetPlayerIndex(L); if (nPlayerIndex <= 0) return 0;').\r\n"
    "\t    \t-- Ba goi nguoc xe tieu chay qua BC_FireCartScript, ham nay DAT\r\n"
    "\t    \t-- SCRIPT_PLAYERINDEX = 0 (KBiaoChe.cpp:197-198) => GetPlayerIndex tra -1\r\n"
    "\t    \t-- => Msg2Tong IM LANG khong gui gi. AUC_MsgTong(nTong, szMsg)\r\n"
    "\t    \t-- (KAuctionServer.cpp:1001, dang ky ScriptFuns.cpp:15129) KHONG can\r\n"
    "\t    \t-- PlayerIndex - dung chinh ham do, giu Msg2Tong lam duong lui.\r\n"
    "\t    \tif AUC_MsgTong then\r\n"
    "\t    \t\tAUC_MsgTong(nTongId, szMsg);\r\n"
    "\t    \telse\r\n"
    "\t    \t\tMsg2Tong(nTongId, szMsg);\r\n"
    "\t    \tend\r\n"
), 1)

dauyb = (
    "-- =========================================================================\r\n"
    "-- yunbiao_system.lua   [LMBC 06/09]  port tu ban Linux\r\n"
    "--   D:" + BS + "ServerLinux" + BS + "server1" + BS + "script" + BS + "global" + BS + "yunbiao_system.lua\r\n"
    "-- Ba ham duoi day la GOI NGUOC tu engine (commit 16f38a84):\r\n"
    "--   OnBiaoCheFarAwayPlayerDisapper / OnBiaoCheDisapper  goi tren XE tieu\r\n"
    "--   OnBiaoCheChangeMapNotice                            goi tren NGUOI CHOI\r\n"
    "-- Can WriteYunBiaoLog (script" + BS + "lib" + BS + "lib_lmbiaoche.lua) va\r\n"
    "-- tbGlobalMapId2Name (script" + BS + "global" + BS + "maplist.lua).\r\n"
    "-- =========================================================================\r\n"
    'Include("' + BS + BS + 'script' + BS + BS + 'lib' + BS + BS + 'lib_lmbiaoche.lua")\r\n'
)
src = dauyb + src
ghi(r"script\global\yunbiao_system.lua", src)

print("XONG")
