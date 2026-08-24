# -*- coding: utf-8 -*-
r"""[TONGCASTLE 23/08] Thi cong Bang Hoi Thanh Bao (missions/tongcastle, map 984).
Kien truc: GS 9 tep + relay 1 tep (chay cuc bo, RemoteExecute noi 2 chieu - engine PORT5).
Engine ta 1 lua_State/tep -> moi truy cap xuyen bang TongCastle.* phai qua cau noi
DynamicExecuteByPlayer vao STATE CHU (tongcastle.lua) - cung khuon bairenleitai.

- 12 tep chep (7 va) + 3 tep MOI (newworld / driver / awardtype zhenyuan)
- settings\maps\tongcastle 11 tep; magicscript gan script 3204-3207 (item CO SAN cung id);
  MapList 984_NewWorldScript; WorldSet +World909=984; npcs.txt dong bo duoi client + 4 template
  Thu Ve moi (Linux 1908-1911 bi du an dung lai cho boss khac); thodiaphuvh chan 984
- Chinh sach: bo trung sinh 4 + cap 150 -> GetLevel() >= 90 (giu 'vao bang > 1 ngay')
LUU Y: du lieu map 984 phia CLIENT (Region_C) KHONG ton tai trong moi pak tren may - nguoi
choi chua the vao map cho den khi chu game co pak client; toan bo phan server van hoat dong.
"""
import io, os, re, shutil, sys

sys.path.insert(0, r"C:/Users/nguye/.claude/skills/swordonline-dev/scripts")
from vn_edit import vn

LINUX = r"D:\ServerLinux\server1"
RELAY = r"D:\ServerLinux\gateway\s3relay"
E     = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CLI   = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
MIR   = r"D:\GAMEDEVNEW\serverscript_jx2\tongcastle"

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s):
    d = os.path.dirname(p)
    if d and not os.path.isdir(d): os.makedirs(d)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
def put_both(rel, content):
    wr(os.path.join(E, rel), content); wr(os.path.join(MIR, rel), content)
def copy_both(src, rel):
    for root in (E, MIR):
        dst = os.path.join(root, rel); d = os.path.dirname(dst)
        if not os.path.isdir(d): os.makedirs(d)
        shutil.copyfile(src, dst)
def mirror_edit(rel):
    src = os.path.join(E, rel); dst = os.path.join(MIR, "jx1_edits", rel)
    d = os.path.dirname(dst)
    if not os.path.isdir(d): os.makedirs(d)
    shutil.copyfile(src, dst)
def rep1(s, old, new, tag=""):
    assert s.count(old) == 1, "rep1 %s: %d" % (tag or old[:60], s.count(old))
    return s.replace(old, new, 1)

MARK = "-- [TONGCASTLE 23/08] "
HA = "\\\\script\\\\missions\\\\tongcastle\\\\tongcastle.lua"

# ============================================================
# 1) chep nguyen byte
# ============================================================
copy_both(LINUX + r"\script\missions\tongcastle\guideperson_inside.lua", r"script\missions\tongcastle\guideperson_inside.lua")
copy_both(LINUX + r"\script\missions\tongcastle\shenmuling.lua",        r"script\missions\tongcastle\shenmuling.lua")
copy_both(LINUX + r"\script\item\forbiditem.lua",                       r"script\item\forbiditem.lua")
copy_both(LINUX + r"\script\global\autoexec_head.lua",                  r"script\global\autoexec_head.lua")
copy_both(RELAY + r"\script\mission\tongcastle\tongcastle.lua",         r"script\mission\tongcastle\tongcastle.lua")
copy_both(LINUX + r"\settings\item_type.txt",   r"settings\item_type.txt")
copy_both(LINUX + r"\settings\item_detail.txt", r"settings\item_detail.txt")
n = 0
for f in os.listdir(LINUX + r"\settings\maps\tongcastle"):
    copy_both(LINUX + r"\settings\maps\tongcastle" + "\\" + f, r"settings\maps\tongcastle" + "\\" + f)
    n += 1
assert n == 11, "settings tongcastle: %d" % n
print("chep nguyen: 7 tep + 11 settings")

# ============================================================
# 2) game.lua: guard FORBITMAP_LIST (heart_head du an la ban JX1 khong co bang nay)
# ============================================================
s = rd(LINUX + r"\script\missions\tongcastle\game.lua")
NL = "\r\n" if "\r\n" in s else "\n"
s = rep1(s, "function Game:SetForbitItem()",
            "function Game:SetForbitItem()" + NL +
            "\t" + MARK + "heart_head JX1 khong co FORBITMAP_LIST (ban Linux :27) - tu khoi tao" + NL +
            "\tFORBITMAP_LIST = FORBITMAP_LIST or {}", "forbitmap")
put_both(r"script\missions\tongcastle\game.lua", s)
print("game.lua ok")

# ============================================================
# 3) tongcastle.lua (STATE CHU): bo RegAll + cau noi JX1
# ============================================================
src_guard = rd(LINUX + r"\script\missions\tongcastle\guard.lua")
TALKS = re.findall(r'Talk\(1, "", "([^"]+)"\)', src_guard)
assert len(TALKS) == 4, "guard.lua Talk: %d" % len(TALKS)
src_td = rd(LINUX + r"\script\missions\tongcastle\treedeath.lua")
m = re.search(r'Msg2Map\(nMapId, format\("([^"]+)", szName, GetNpcName\(nNpcIndex\)\)\)', src_td)
assert m, "treedeath Msg2Map"
SZ_TREEMSG = m.group(1)

s = rd(LINUX + r"\script\missions\tongcastle\tongcastle.lua")
NL = "\r\n" if "\r\n" in s else "\n"
s = rep1(s, "\tself:SetForbitItem()" + NL + "\tself:Init()" + NL + "\tself:RegAll()",
            "\tself:SetForbitItem()" + NL + "\tself:Init()" + NL +
            "\t" + MARK + "RegAll (EventSys EnterMap/LeaveMap + DynamicExecute tra table) KHONG chay duoc" + NL +
            "\t-- tren JX1 (1 state/tep, DynamicExecute khong tra gia tri) -> thay bang newworld.lua cua map 984" + NL +
            "\t--self:RegAll()", "regall")
bridge = NL.join([
"",
"-- ===== " + MARK.strip() + " cau noi 1 state/tep: trap/guard/treedeath/newworld goi sang STATE CHU nay =====",
"TC_JX1_GUARDTYPE = {[1] = 4, [2] = 5, [3] = 6, [4] = 7}\t-- guard.lua tbGuardType -> nNpcParam",
"TC_JX1_JOINTONGTIME = 1440\t-- guideperson JOIN_TONG_TIME = 60*24 phut",
"",
"function TongCastle:JX1_OnEnterMap()\t-- = castleplayer CastlePlayer:OnEnterMap",
"\tSetFightState(1)",
"\tDisabledUseTownP(1)",
"\tSetLogoutRV(1)",
"\tDisabledStall(1)",
"\tSetDeathScript(\"\\\\script\\\\missions\\\\tongcastle\\\\castleplayer.lua\")",
"\tlocal nMapId = GetWorldPos()",
"\tself:AddPlayer2List(GetName(), nMapId, GetTongName())",
"end",
"",
"function TongCastle:JX1_OnLeaveMap()\t-- = castleplayer CastlePlayer:OnLeaveMap (quet moi map cho chac)",
"\tSetFightState(0)",
"\tDisabledUseTownP(0)",
"\tDisabledStall(0)",
"\tSetDeathScript(\"\")",
"\tlocal szName = GetName()",
"\tfor nMapId, bFlag in self.tbMapList do",
"\t\tif bFlag and self.tbPlayerList[nMapId] and self.tbPlayerList[nMapId][szName] then",
"\t\t\tself:DelPlayer2List(szName, nMapId)",
"\t\tend",
"\tend",
"end",
"",
"function TongCastle:JX1_Trap(nTrapId)\t-- = trap.lua main",
"\tlocal _, _, nMapIndex = GetPos()",
"\tlocal nMapId = SubWorldIdx2ID(nMapIndex)",
"\tself:TrapMoveTo(nMapId, nTrapId)",
"end",
"",
"function TongCastle:JX1_CallGuard(nType, nItemIndex)\t-- = guard.lua CallGuard (chay trong state chu de RegANpc/GuardIsLimit dung kho that)",
"\tlocal nNpcParam = TC_JX1_GUARDTYPE[nType]",
"\tif not nNpcParam then",
"\t\treturn",
"\tend",
"\tlocal nX32, nY32, nMapIndex = GetPos()",
"\tlocal nMapId = SubWorldIdx2ID(nMapIndex)",
"\tif %tbCity[nMapId] == nil then",
"\t\tTalk(1, \"\", \"" + TALKS[0] + "\")",
"\t\treturn",
"\tend",
"\tlocal szCityTongName = GetCityOwner(%tbCity[nMapId][1])",
"\tif szCityTongName ~= GetTongName() then",
"\t\tTalk(1, \"\", \"" + TALKS[1] + "\")",
"\t\treturn",
"\tend",
"\tif self:GuardIsLimit(nMapId) ~= 1 then",
"\t\tTalk(1, \"\", \"" + TALKS[2] + "\")",
"\t\treturn",
"\tend",
"\tlocal tbNpc, nCount = GetAroundNpcList(15)",
"\tlocal bNear = nil",
"\tfor i = 1, nCount do",
"\t\tlocal nP = GetNpcParam(tbNpc[i], 1)",
"\t\tif nP >= 1 and nP <= 3 and GetTmpCamp(tbNpc[i]) == 1 then",
"\t\t\tbNear = 1",
"\t\tend",
"\tend",
"\tif bNear ~= 1 then",
"\t\tTalk(1, \"\", \"" + TALKS[3] + "\")",
"\t\treturn",
"\tend",
"\tlocal szNpcName = self.tbNpcParam2Type[nNpcParam]",
"\tlocal tbNpcT = self.tbNpcTypeList[szNpcName]",
"\tlocal nNewNpcIndex = self:basemission_CallNpc(tbNpcT, nMapId, nX32, nY32)",
"\tlocal nStackCount = GetItemStackCount(nItemIndex)",
"\tif nStackCount > 1 then",
"\t\tSetItemStackCount(nItemIndex, nStackCount - 1)",
"\telse",
"\t\tRemoveItemByIndex(nItemIndex)",
"\tend",
"\tif nNewNpcIndex > 0 then",
"\t\tSetTmpCamp(1, nNewNpcIndex)",
"\t\tself:RegANpc(nMapId, nNpcParam, 0, nNewNpcIndex, GetNpcId(nNewNpcIndex))",
"\tend",
"end",
"",
"function TongCastle:JX1_TreeDeath(nNpcIndex)\t-- = treedeath.lua OnDeathEx (boi canh = ke giet, qua DynamicExecuteByPlayer)",
"\tlocal nX32, nY32, nMapIndex = GetNpcPos(nNpcIndex)",
"\tlocal nMapId = SubWorldIdx2ID(nMapIndex)",
"\tlocal nNpcParam = GetNpcParam(nNpcIndex, 1)",
"\tlocal nDir = GetNpcParam(nNpcIndex, 2)",
"\tif self:FindNpc(nMapId, nNpcIndex, GetNpcId(nNpcIndex)) ~= 1 then",
"\t\tprint(\"Npc Death Error!!!!!!!!!!\")",
"\t\treturn",
"\tend",
"\tself:UnRegANpc(nMapId, nNpcParam, nDir, nNpcIndex, GetNpcId(nNpcIndex))",
"\tself:UpdateObstacleObj(nMapId)",
"\tlocal szTongName = \"\"",
"\tlocal szName = \"\"",
"\tif (GetJoinTongTime() > TC_JX1_JOINTONGTIME) then",
"\t\tszTongName = GetTongName()",
"\t\tszName = GetName()",
"\tend",
"\tMsg2Map(nMapId, format(\"" + SZ_TREEMSG + "\", szName, GetNpcName(nNpcIndex)))",
"\tif 1 <= nNpcParam and nNpcParam <= 3 then",
"\t\tlocal handle = OB_Create()",
"\t\tObjBuffer:PushObject(handle, nMapId)",
"\t\tObjBuffer:PushObject(handle, nNpcParam)",
"\t\tObjBuffer:PushObject(handle, nX32)",
"\t\tObjBuffer:PushObject(handle, nY32)",
"\t\tObjBuffer:PushObject(handle, nDir)",
"\t\tObjBuffer:PushObject(handle, szTongName)",
"\t\tObjBuffer:PushObject(handle, szName)",
"\t\tRemoteExecute(\"\\\\script\\\\mission\\\\tongcastle\\\\tongcastle.lua\", \"tbS3TongCastle:DelOneTreeData\", handle)",
"\t\tOB_Release(handle)",
"\t\tself:BroadcastTreeDeath(nMapId, nNpcParam, nDir)",
"\tend",
"end",
""])
if not s.endswith(NL): s += NL
s += bridge
put_both(r"script\missions\tongcastle\tongcastle.lua", s)
print("tongcastle.lua (state chu) ok")

# ============================================================
# 4) castleplayer.lua: guard tbKillCount (copy-state chua Init)
# ============================================================
s = rd(LINUX + r"\script\missions\tongcastle\castleplayer.lua")
NL = "\r\n" if "\r\n" in s else "\n"
s = rep1(s, "\t\tlocal tbKillCount = TongCastle.tbKillCount",
            "\t\tTongCastle.tbKillCount = TongCastle.tbKillCount or {}\t" + MARK + "ban sao state nay chua qua Init()" + NL +
            "\t\tlocal tbKillCount = TongCastle.tbKillCount", "killcount")
put_both(r"script\missions\tongcastle\castleplayer.lua", s)
print("castleplayer.lua ok")

# ============================================================
# 5) trap.lua: main -> cau noi state chu
# ============================================================
s = rd(LINUX + r"\script\missions\tongcastle\trap.lua")
NL = "\r\n" if "\r\n" in s else "\n"
s = rep1(s, "function main(nTrapId)" + NL + "\tlocal _,_, nMapIndex = GetPos()" + NL + "\tlocal nMapId = SubWorldIdx2ID(nMapIndex)" + NL + "\tTongCastle:TrapMoveTo(nMapId, nTrapId)" + NL + "end",
            "function main(nTrapId)" + NL +
            "\t" + MARK + "tbTrap song trong state chu (engine ta 1 state/tep)" + NL +
            "\tDynamicExecuteByPlayer(PlayerIndex, \"" + HA + "\", \"TongCastle:JX1_Trap\", nTrapId)" + NL +
            "end", "trapmain")
put_both(r"script\missions\tongcastle\trap.lua", s)
print("trap.lua ok")

# ============================================================
# 6) guard.lua: CallGuard -> cau noi
# ============================================================
s = src_guard
NL = "\r\n" if "\r\n" in s else "\n"
i0 = s.find("function CallGuard(nType, nItemIndex)")
assert i0 > 0
i1 = s.find("function main(nItemIndex)")
assert i1 > i0
new_cg = ("function CallGuard(nType, nItemIndex)" + NL +
          "\t" + MARK + "kho Thu Ve (GuardIsLimit/RegANpc) song trong state chu -> chuyen ca luong sang do" + NL +
          "\tDynamicExecuteByPlayer(PlayerIndex, \"" + HA + "\", \"TongCastle:JX1_CallGuard\", nType, nItemIndex)" + NL +
          "end" + NL + NL)
s = s[:i0] + new_cg + s[i1:]
put_both(r"script\missions\tongcastle\guard.lua", s)
print("guard.lua ok")

# ============================================================
# 7) treedeath.lua: + OnDeath adapter JX1
# ============================================================
s = src_td
NL = "\r\n" if "\r\n" in s else "\n"
adapter = NL.join([
"",
MARK.strip() + " engine JX1 goi OnDeath(nNpcIdx, nLastDamageIdx) cua ActionScript khi NPC bi nguoi",
"-- choi giet (KNpc.cpp) - Linux dung EventSys NpcDeath theo TEN (RegisterNpc, khong chay tren JX1).",
"-- Kho dem cay/ve binh song trong STATE CHU -> goi sang do (boi canh = ke giet).",
"function OnDeath(nNpcIndex, nDamageIdx)",
"\tlocal nAtk = NpcIdx2PIdx(nDamageIdx)",
"\tif nAtk and nAtk > 0 then",
"\t\tDynamicExecuteByPlayer(nAtk, \"" + HA + "\", \"TongCastle:JX1_TreeDeath\", nNpcIndex)",
"\tend",
"\ttbLog:PlayerActionLog(\"LOGTINHNANGJX1_npc_BHTB\",\"BHTBKillTree\", GetNpcName(nNpcIndex))",
"end",
""])
if not s.endswith(NL): s += NL
s += adapter
put_both(r"script\missions\tongcastle\treedeath.lua", s)
print("treedeath.lua ok")

# ============================================================
# 8) guideperson.lua: Include + chinh sach cap 90
# ============================================================
s = rd(LINUX + r"\script\missions\tongcastle\guideperson.lua")
NL = "\r\n" if "\r\n" in s else "\n"
s = rep1(s, 'Include("\\\\script\\\\activitysys\\\\npcdailog.lua")',
            'Include("\\\\script\\\\dailogsys\\\\g_dialog.lua")\t' + MARK + 'thay npcdailog (keo 18 tep bonusvlmc): DailogClass' + NL +
            'Include("\\\\script\\\\dailogsys\\\\dailogsay.lua")\t-- g_AskClientNumberEx' + NL +
            'Include("\\\\script\\\\activitysys\\\\g_activity.lua")\t-- G_ACTIVITY (FunSet rong tren JX1)', "include")
SZ_LIMIT = vn("Phải đạt cấp 90 trở lên và vào bang nhiều hơn 1 ngày mới có thể tham gia Bang Hội Thành Bảo")
blk = re.search(r'MSG_PLAYER_LIMIT = format\(.*?\r?\n.*?\r?\n', s, re.S)
assert blk, "MSG_PLAYER_LIMIT"
s = s[:blk.start()] + MARK + "chinh sach: bo trung sinh 4 + cap 150, giu 'vao bang > 1 ngay'" + NL + 'MSG_PLAYER_LIMIT = "' + SZ_LIMIT + '"' + NL + s[blk.end():]
blkTL = re.search(r'\tlocal nTransLifeCount = ST_GetTransLifeCount\(\)\r?\n\tif \(nTransLifeCount < 4\) or \(nTransLifeCount == 4 and GetLevel\(\) < 150\) then\r?\n\t\tTalk\(1, "", %MSG_PLAYER_LIMIT\)\r?\n\t\treturn\r?\n\tend\r?\n', s)
assert blkTL, "CheckPlayer translife"
s = (s[:blkTL.start()] +
    "\t" + MARK + "bo trung sinh, cong cap >= 90" + NL +
    "\tif (GetLevel() < 90) then" + NL +
    "\t\tTalk(1, \"\", %MSG_PLAYER_LIMIT)" + NL +
    "\t\treturn" + NL +
    "\tend" + NL + s[blkTL.end():])
put_both(r"script\missions\tongcastle\guideperson.lua", s)
print("guideperson.lua ok")

# ============================================================
# 9) 3 tep MOI: newworld / driver / awardtype zhenyuan
# ============================================================
NLn = "\n"
nw = NLn.join([
"-- " + MARK.strip() + " map 984: thay EventSys EnterMap/LeaveMap (Game:RegAll khong chay duoc tren JX1)",
"-- khuon missions\\leaguematch\\combat\\newworld.lua",
"Include(\"\\\\script\\\\maps\\\\newworldscript_default.lua\")",
"TC_HA = \"" + HA + "\"",
"",
"function OnNewWorld(szParam)",
"\tDynamicExecuteByPlayer(PlayerIndex, TC_HA, \"TongCastle:JX1_OnEnterMap\")",
"\tOnNewWorldDefault(szParam)",
"end",
"",
"function OnLeaveWorld(szParam)",
"\tDynamicExecuteByPlayer(PlayerIndex, TC_HA, \"TongCastle:JX1_OnLeaveMap\")",
"\tOnLeaveWorldDefault(szParam)",
"end",
""])
put_both(r"script\missions\tongcastle\newworld.lua", nw)

drv = NLn.join([
"-- ============================================================",
"-- TONGCASTLE DRIVER " + MARK.strip() + " - thay relaysetting\\task\\tongcastle.lua (tick MOI PHUT)",
"-- + autoexec.lua AutoFunctions:Run (khuon tong_driver / tongwar_driver).",
"-- CHI Run() o state chu; guideperson goi thang CreateGuideNpc (Run() o do se chay ca",
"-- TongCastle.Start cua BAN SAO -> nhan doi cay/trap).",
"-- ============================================================",
"",
"TC_DRV_MSKEY   = 30\t-- khoa missionId GlbTimer (27 WLLS / 28 TONG / 29 TONGWAR)",
"TC_DRV_TIMERID = 55\t-- settings\\TimerTask.txt khoa 55",
"TC_DRV_LASTMIN = -1",
"TC_RELAY = \"\\\\script\\\\mission\\\\tongcastle\\\\tongcastle.lua\"",
"",
"function TONGCASTLE_DriverInit(szParam)",
"\tDynamicExecute(\"" + HA + "\", \"AutoFunctions:Run\")\t-- TongCastle:Start (trap/cay/diem)",
"\tDynamicExecute(\"\\\\script\\\\missions\\\\tongcastle\\\\guideperson.lua\", \"CreateGuideNpc\")",
"\tDynamicExecute(\"\\\\script\\\\missions\\\\tongcastle\\\\guideperson.lua\", \"CreateGuideNpcInside\")",
"\tTC_DRV_LASTMIN = tonumber(date(\"%y%m%d%H%M\"))",
"\tStartGlbMSTimer(TC_DRV_MSKEY, TC_DRV_TIMERID, 30 * 18)",
"\tOutputMsg(\"[TONGCASTLE] Bang Hoi Thanh Bao khoi dong\")",
"\treturn 1",
"end",
"",
"function OnTimer()",
"\tlocal m = tonumber(date(\"%y%m%d%H%M\"))",
"\tif (m ~= TC_DRV_LASTMIN) then",
"\t\tTC_DRV_LASTMIN = m",
"\t\tDynamicExecute(TC_RELAY, \"tbS3TongCastle:CheckAndReviveTree\")",
"\t\tDynamicExecute(TC_RELAY, \"tbS3TongCastle:CheckAndDeleteTree\")",
"\tend",
"end",
"",
"-- Lenh bai Admin: ep hoi sinh cay loai n (1/2/3) de test ngoai gio",
"function TONGCASTLE_Adm_ReviveTree(n)",
"\tDynamicExecute(\"" + HA + "\", \"TongCastle:AddTreeInMap\", 984, n or 1)",
"\treturn 1",
"end",
""])
put_both(r"script\missions\tongcastle\tongcastle_driver.lua", drv)

zy = NLn.join([
"-- zhenyuan_jx1.lua " + MARK.strip() + " awardtype nZhenYuanPoint (shenmuling Than Moc Lenh):",
"-- diem Chan Nguyen kinh mach JX1 = PlayerFunLib:AddTask(TASK_CHANGNGUYENDAN, n)",
"-- (khuon item\\event\\kinhmach\\honnguyenchandon.lua:44)",
"Include(\"\\\\script\\\\lib\\\\log.lua\")",
"Include(\"\\\\script\\\\lib\\\\lib_task.lua\")",
"-- [23/08 vong Include] playerfunlib:4 Include nguoc awardtemplet = vong vo han - nap luoi luc Give()",
"",
"ZhenYuanJX1Type = {}",
"",
"function ZhenYuanJX1Type:Give(tbItem, nAwardCount, tbLogTitle)",
"\tlocal nPoint = (tbItem.nZhenYuanPoint or 0) * (nAwardCount or 1)",
"\tif nPoint <= 0 then",
"\t\treturn 0",
"\tend",
"\tif not PlayerFunLib then",
"\t\tInclude(\"\\\\script\\\\activitysys\\\\playerfunlib.lua\")\t-- nap luoi (xem chu thich dau tep)",
"\tend",
"\tPlayerFunLib:AddTask(TASK_CHANGNGUYENDAN, nPoint)",
"\tWriteLog(format(\"[TongCastle awardtype zhenyuan] %s +%d diem Chan Nguyen\", GetName() or \"\", nPoint))",
"\treturn 1",
"end",
"",
"if tbAwardTemplet then",
"\ttbAwardTemplet:RegType(\"nZhenYuanPoint\", ZhenYuanJX1Type)",
"end",
""])
put_both(r"script\lib\awardtype\zhenyuan_jx1.lua", zy)
print("3 tep moi ok")

# ============================================================
# 10) awardtemplet.lua (du an): + Include zhenyuan_jx1
# ============================================================
p = os.path.join(E, r"script\lib\awardtemplet.lua")
s = rd(p)
NLa = "\r\n" if "\r\n" in s else "\n"
assert "item_jx1" in s, "awardtemplet chua co item_jx1?"
m = re.search(r'Include\("\\\\script\\\\lib\\\\awardtype\\\\item_jx1\.lua"\)[^\r\n]*', s)
assert m, "khong thay Include item_jx1"
if "zhenyuan_jx1" not in s:
    s = s[:m.end()] + NLa + 'Include("\\\\script\\\\lib\\\\awardtype\\\\zhenyuan_jx1.lua")\t' + MARK + 'diem Chan Nguyen (shenmuling)' + s[m.end():]
wr(p, s)
mirror_edit(r"script\lib\awardtemplet.lua")
print("awardtemplet.lua ok")

# ============================================================
# 11) magicscript: gan script 3204-3207 (item da co san cung id)
# ============================================================
p = os.path.join(E, r"settings\item\magicscript.txt")
s = rd(p)
NLi = "\r\n" if "\r\n" in s else "\n"
ls = s.split(NLi)
fixed = 0
for i, l in enumerate(ls):
    parts = l.split("\t")
    if len(parts) > 10 and parts[1] == "6" and parts[2] == "1" and parts[3] in ("3204", "3205", "3206", "3207"):
        want = "\\script\\missions\\tongcastle\\guard.lua" if parts[3] == "3204" else "\\script\\missions\\tongcastle\\shenmuling.lua"
        if parts[9] == want:
            fixed += 1
            continue
        assert parts[9] == "0", "magicscript %s cot 10 = %r" % (parts[3], parts[9])
        parts[9] = want
        ls[i] = "\t".join(parts)
        fixed += 1
assert fixed == 4, "magicscript 3204-3207: %d" % fixed
wr(p, NLi.join(ls))
mirror_edit(r"settings\item\magicscript.txt")
print("magicscript 3204-3207 ok")

# ============================================================
# 12) npcs.txt: dong bo duoi client vao server + 4 template Thu Ve moi (CA server + client)
# ============================================================
def parse_npcs(p):
    s = rd(p)
    NLx = "\r\n" if "\r\n" in s else "\n"
    ls = s.split(NLx)
    while ls and ls[-1] == "":
        ls.pop()
    return s, NLx, ls

sv_s, sv_nl, sv = parse_npcs(os.path.join(E, r"settings\npcs.txt"))
cl_s, cl_nl, cl = parse_npcs(os.path.join(CLI, r"settings\npcs.txt"))
lx_s, lx_nl, lx = parse_npcs(LINUX + r"\settings\npcs.txt")
hdr_p = sv[0].split("\t")
hdr_l = lx[0].split("\t")
assert sv[0] == cl[0], "header server != client"
# 12a. dong bo duoi: server nhan cac dong client vuot qua (giu id thang hang)
assert len(cl) >= len(sv), "client ngan hon server?"
extra = cl[len(sv):]
sv = sv + extra
# 12b. 4 template Thu Ve (Linux id 1908-1911) -> id moi cuoi bang, map cot theo TEN
guard_ids = {}
for lid in (1908, 1909, 1910, 1911):
    row_l = lx[lid + 1].split("\t")     # id = dong-2 -> chi so mang = id+1
    d = dict(zip(hdr_l, row_l))
    # idempotent: neu da append lan truoc (Name trung) thi dung lai id do
    found = None
    for k in range(2000, len(sv)):
        if sv[k].split("\t")[0] == d.get("Name", "?"):
            found = k - 1  # id = dong-2 = chi_so(k) + 1 - 2
            break
    if found is not None:
        guard_ids[lid] = found
        continue
    new_row = [d.get(c, "") for c in hdr_p]
    sv.append("\t".join(new_row))
    cl.append("\t".join(new_row))
    new_id = len(sv) - 2  # id = dong-2, dong = chi so + 1
    guard_ids[lid] = new_id
print("guard template moi:", guard_ids)
wr(os.path.join(E, r"settings\npcs.txt"), sv_nl.join(sv) + sv_nl)
wr(os.path.join(CLI, r"settings\npcs.txt"), cl_nl.join(cl) + cl_nl)
mirror_edit(r"settings\npcs.txt")
shutil.copyfile(os.path.join(CLI, r"settings\npcs.txt"), os.path.join(MIR, "jx1_edits", "client_npcs.txt"))

# 12c. va id guard trong tongcastle.lua (STATE CHU) + kiem tra 1912-1914 (cay giu nguyen id)
p = os.path.join(E, r"script\missions\tongcastle\tongcastle.lua")
s = rd(p)
for lid in (1908, 1909, 1910, 1911):
    old = "nNpcId = %d," % lid
    assert s.count(old) == 1, "tongcastle nNpcId %d: %d" % (lid, s.count(old))
    s = s.replace(old, "nNpcId = %d,\t%sid Linux %d bi du an dung cho boss khac - template moi chep tu Linux" % (guard_ids[lid], MARK, lid), 1)
wr(p, s)
shutil.copyfile(p, os.path.join(MIR, r"script\missions\tongcastle\tongcastle.lua"))
print("npcs.txt + remap guard ok")

# ============================================================
# 13) MapList 984 + WorldSet + thodiaphuvh + startgame + TimerTask 55
# ============================================================
p = os.path.join(E, r"settings\MapList.ini")
s = rd(p)
if "984_NewWorldScript=\\script\\missions\\tongcastle\\newworld.lua" not in s:
    s = rep1(s, "984_NewWorldScript=\\script\\maps\\newworldscript.lua",
                "984_NewWorldScript=\\script\\missions\\tongcastle\\newworld.lua", "maplist984")
wr(p, s)
mirror_edit(r"settings\MapList.ini")

p = os.path.join(E, r"Maps\WorldSet_GameServer.ini")
s = rd(p)
NLw = "\r\n" if "\r\n" in s else "\n"
if "World909=984" not in s:
    assert "=984" not in s
    s = rep1(s, "Count=909", "Count=910", "worldcount")
    m = re.search(r"World908=995[^\r\n]*", s)
    assert m
    s = s[:m.end()] + NLw + "World909=984 --- Bang hoi thanh bao [TONGCASTLE 23/08]" + s[m.end():]
wr(p, s)
mirror_edit(r"Maps\WorldSet_GameServer.ini")

p = os.path.join(E, r"script\item\ib\thodiaphuvh.lua")
s = rd(p)
old = "nSubWorldID == 212 or"
new = "nSubWorldID == 212 or nSubWorldID == 984 or"
if "984" not in s:
    assert s.count(old) == 1, "thodiaphuvh 212: %d" % s.count(old)
    s = s.replace(old, new)
wr(p, s)
mirror_edit(r"script\item\ib\thodiaphuvh.lua")

p = os.path.join(E, r"settings\TimerTask.txt")
s = rd(p)
NLt = "\r\n" if "\r\n" in s else "\n"
if not re.search(r"^55\t", s, re.M):
    if not s.endswith(NLt): s += NLt
    s += "55\t\\script\\missions\\tongcastle\\tongcastle_driver.lua" + NLt
wr(p, s)
mirror_edit(r"settings\TimerTask.txt")

p = os.path.join(E, r"script\startgame.lua")
s = rd(p)
NLs = "\r\n" if "\r\n" in s else "\n"
if "tongcastle_driver.lua" not in s:
    old = 'Include("\\\\script\\\\missions\\\\bw\\\\bw_addnpc.lua")'
    i = s.find(old); assert i >= 0
    j = s.find(NLs, i)
    s = s[:j] + NLs + 'Include("\\\\script\\\\missions\\\\tongcastle\\\\tongcastle_driver.lua")\t-- [TONGCASTLE 23/08] Bang Hoi Thanh Bao' + s[j:]
    old = '\tDynamicExecute("\\\\script\\\\missions\\\\bairenleitai\\\\bairen_boot.lua", "BairenLeitai_Init")'
    i = s.find(old); assert i >= 0
    j = s.find(NLs, i)
    s = s[:j] + NLs + '\tDynamicExecute("\\\\script\\\\missions\\\\tongcastle\\\\tongcastle_driver.lua", "TONGCASTLE_DriverInit")\t-- [TONGCASTLE 23/08]' + s[j:]
wr(p, s)
mirror_edit(r"script\startgame.lua")
print("maplist + worldset + thodiaphu + timertask + startgame ok")

# ============================================================
# 14) kiem tra phu: task id va cham? skill guard? spr icon?
# ============================================================
hits = {}
for root, dirs, files in os.walk(os.path.join(E, "script")):
    if "tongcastle" in root: continue
    for f in files:
        if not f.endswith(".lua"): continue
        try: t = rd(os.path.join(root, f))
        except Exception: continue
        for tid in ("4056", "4057", "4058", "4059", "4060", "4061", "3399"):
            if ("Task(" + tid) in t or ("Task( " + tid) in t:
                hits.setdefault(tid, []).append(os.path.join(root, f))
print("task va cham ngoai tongcastle:", {k: v[:2] for k, v in hits.items()} if hits else "KHONG")
sk = rd(os.path.join(E, r"settings\skills.txt")).split("\n")
for sid in (93, 1210, 1208, 1212):
    ok = len(sk) > sid + 1
    print("skill", sid, "row:", (sk[sid + 1].split("\t")[0][:30] if ok else "THIEU"))
print("TAT CA XONG")
