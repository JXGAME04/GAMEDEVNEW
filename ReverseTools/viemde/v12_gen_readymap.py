# -*- coding: utf-8 -*-
"""VIEM DE - buoc 6b-2b: sinh lai readymap\\readymap.lua + hook ban do ydbz_maphook.lua.

readymap.lua cua ban Linux giu 2 thu khong song duoc o JX1:
  * self.nCount (so nguoi trong phong) - bien Lua, ma OnEnterMap/OnLeaveMap lai
    duoc engine goi tu MOT STATE KHAC -> phai xuong bien mission (Dungeon:AddCount).
  * AddNpc(...) - bay #5: tham so 6 cua JX1 la NGU HANH, cua Linux la co bNoRevive.
    Doi sang AddNpcEx(nId, nLevel, nSeries, nSubWorldIdx, nX, nY, nCamp, szTen).
    NPC 389 "Chu duoc diem 1" o bang npcs.txt JX1: Kind=3 Camp=6 Series=0.

Hook: JX1 khong tu goi OnEnterMap/OnLeaveMap cua pho ban. Nhung MapList.ini co khoa
<id>_NewWorldScript va engine ban "OnNewWorld"/"OnLeaveWorld" cho script do khi nguoi
choi vao/roi ban do (KNpc.cpp:9995-9996, ScriptFuns.cpp:11270 KSubWorld_FireMapScript).
Buoc 2 da tro 15 ban do trong be vao ydbz_maphook.lua; tep nay chuyen tiep len ReadyMap.
"""
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from bangtxt import uni2tcvn
# 🔴 BAY DA DINH 26/08: ma hoa HAI LAN.
# `V(...)` long ben trong roi ham bao ngoai lai ma hoa lan nua -> byte TCVN3 (vd 0xB8)
# bi dua vao unicode_to_tcvn3_bytes lan hai, khong khop bang nao va BI BO -> chu Viet
# rung het dau ("Cach choi vuot ai moi" -> "Cch chi vt i mii").
# Cach chan: V() o day la KHONG LAM GI; chi ham bao ngoai (s()/r()) moi ma hoa, DUNG MOT LAN.
def V(x):
    return x


SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
GUONG = r"D:\GAMEDEVNEW\serverscript_jx2\viemde"

# --------------------------------------------------------------- readymap.lua
R = []


def r(txt):
    R.append(uni2tcvn(txt))


r("-- ==========================================================================")
r("-- VIEM DE - PHONG CHO (lop pho ban)  ban JX1")
r("-- SINH TU DONG boi ReverseTools\\viemde\\v12_gen_readymap.py - DUNG SUA TAY")
r("--")
r("-- Dich tu \\script\\missions\\yandibaozang\\readymap\\readymap.lua cua ban Linux.")
r("-- Hai cho phai doi (xem dau tep sinh):")
r("--   * self.nCount -> Dungeon:AddCount/GetCount (bien mission, dung chung duoc)")
r("--   * AddNpc -> AddNpcEx (bay #5: tham so 6 lech nghia)")
r("-- ==========================================================================")
r("")
r('Include("\\\\script\\\\lib\\\\common.lua")')
r('Include("\\\\script\\\\missions\\\\basemission\\\\dungeon.lua")')
r('Include("\\\\script\\\\missions\\\\yandibaozang\\\\head.lua")')
r('Include("\\\\script\\\\missions\\\\yandibaozang\\\\readymap\\\\ydbz_share.lua")')
r('Include("\\\\script\\\\missions\\\\yandibaozang\\\\readymap\\\\ready.lua")')
r('Include("\\\\script\\\\global\\\\autoexec_head.lua")')
r("")
r('ReadyMap = Dungeon:new_type("yandibaozang_ready")')
r("")
r("YDBZ_NPC_DUOCDIEM      = 389    -- \"Chu duoc diem 1\" (npcs.txt JX1: Kind=3 Camp=6 Series=0)")
r("YDBZ_NPC_DUOCDIEM_CAMP = 6")
r("YDBZ_NPC_DUOCDIEM_HE   = 0")
r('YDBZ_NPC_DUOCDIEM_SCRIPT = "\\\\script\\\\missions\\\\yandibaozang\\\\npc\\\\yaoshang.lua"')
r("")
r("function ReadyMap:_init(tbParam)")
r("\tDG_SetVar(self.nMapId, DG_V_COUNT, 0)")
r("\tDG_SetVar(self.nMapId, DG_V_TYPE, DG_TYPE_YDBZ_READY)")
r("\tself:AddTimer(18*60, self.OnTime, {self})")
r("")
r("\tlocal nMapIndex = SubWorldID2Idx(self.nMapId)")
r("\tif nMapIndex < 0 then")
r("\t\treturn nil")
r("\tend")
r("")
r("\t-- ban Linux: AddNpc(389, 80, nMapIndex, 1577*32, 3184*32, 0, \"Chu duoc diem\")")
r("\tlocal nNpc1 = AddNpcEx(YDBZ_NPC_DUOCDIEM, 80, YDBZ_NPC_DUOCDIEM_HE, nMapIndex, 1577*32, 3184*32, YDBZ_NPC_DUOCDIEM_CAMP, \"%s\")" % V("Chủ dược điếm"))
r("\tif nNpc1 and nNpc1 > 0 then")
r("\t\tSetNpcScript(nNpc1, YDBZ_NPC_DUOCDIEM_SCRIPT)")
r("\t\tself:RememberNpc(nNpc1)")
r("\tend")
r("")
r("\tlocal nNpc2 = AddNpcEx(YDBZ_NPC_DUOCDIEM, 80, YDBZ_NPC_DUOCDIEM_HE, nMapIndex, 1607*32, 3178*32, YDBZ_NPC_DUOCDIEM_CAMP, \"%s\")" % V("Chủ dược điếm"))
r("\tif nNpc2 and nNpc2 > 0 then")
r("\t\tSetNpcScript(nNpc2, YDBZ_NPC_DUOCDIEM_SCRIPT)")
r("\t\tself:RememberNpc(nNpc2)")
r("\tend")
r("")
r("\treturn 1")
r("end")
r("")
r("function ReadyMap:OnEnterMap()")
r("\tself:AddCount(1)")
r("\tLeaveTeam()")
r("\tSetTmpCamp(1);")
r("\tSetFightState(0)")
r("\tSetLogoutRV(1);")
r("\tSetPunish(0);")
r("\tSetCreateTeam(0)")
r("\tSetPKFlag(1)")
r("\tForbidChangePK(1)")
r("\tDisabledUseTownP(1)")
r("\tForbitTrade(1)")
r("\tForbidEnmity(1)")
r("\tSetTaskTemp(YDBZ_TASKTEMP_TRONGTRAN,1)")
r("end")
r("")
r("function ReadyMap:OnLeaveMap()")
r("\tYDBZ_BoNguoi(PlayerIndex)")
r("\tself:AddCount(-1)")
r("\tSetTmpCamp(0);")
r("\tSetFightState(0)")
r("\tSetLogoutRV(0);")
r("\tSetPunish(1);")
r("\tSetCreateTeam(1)")
r("\tSetPKFlag(0)")
r("\tForbidChangePK(0)")
r("\tDisabledUseTownP(0)")
r("\tForbitTrade(0)")
r("\tForbidEnmity(0)")
r("\tSetTaskTemp(YDBZ_TASKTEMP_TRONGTRAN,0)")
r("end")
r("")
r("function ReadyMap:OnTime()")
r("\tif self:GetCount() == 0 then")
r("\t\tself:close()")
r("\t\treturn 0")
r("\tend")
r("\treturn 18*10")
r("end")
r("")
r("function ReadyMap:OnClose()")
r('\tYDBZ_MoiPhong(format("%s", YDBZ_DemPhong() - 1))' % V("có đại hiệp lâm trận trọng thương, hiện tại có %d chi đội tham gia."))
r("end")
r("")
r("function ReadyMap:GetFreeMap()")
r("\tif YDBZ_DemPhong() >= YDBZ_LIMIT_TEAMS_COUNT then")
r("\t\treturn 0")
r("\tend")
r("\tlocal pDungeon = self:new_dungeon(tbReady.nTemplateMapId, {})")
r("\tif pDungeon == nil then")
r("\t\treturn 0")
r("\tend")
r('\tYDBZ_MoiPhong(format("%s", YDBZ_DemPhong()))' % V("tranh đoạt Kho Báu Viêm Đế sắp diễn ra, hiện tại có %d chi đội tham gia."))
r("\treturn pDungeon.nMapId")
r("end")
r("")
r("local _autoexec = function()")
r("\tPreApplyDungeonMap(852, 0, 0)")
r("end")
r("")
r("AutoFunctions:Add(_autoexec)")
r("")

# --------------------------------------------------------- ydbz_maphook.lua
H = []


def h(txt):
    H.append(uni2tcvn(txt))


h("-- ==========================================================================")
h("-- VIEM DE - HOOK VAO/ROI BAN DO PHONG CHO")
h("-- SINH TU DONG boi ReverseTools\\viemde\\v12_gen_readymap.py - DUNG SUA TAY")
h("--")
h("-- MapList.ini tro <id>_NewWorldScript cua 15 ban do trong be vao tep nay.")
h("-- Engine ban \"OnLeaveWorld\" cho ban do CU va \"OnNewWorld\" cho ban do MOI moi")
h("-- khi nguoi choi doi ban do (KNpc.cpp:9995-9996), voi bien toan cuc SubWorld")
h("-- va PlayerIndex da duoc bom san (ScriptFuns.cpp:11270).")
h("--")
h("-- Ban Linux khong can tep nay: engine cua no goi thang OnEnterMap/OnLeaveMap")
h("-- cua doi tuong pho ban.")
h("-- ==========================================================================")
h("")
h('Include("\\\\script\\\\lib\\\\common.lua")')
h('Include("\\\\script\\\\missions\\\\basemission\\\\dungeon.lua")')
h('Include("\\\\script\\\\missions\\\\yandibaozang\\\\readymap\\\\readymap.lua")')
h("")
h("-- Tim lai doi tuong pho ban cua ban do dang xet. DungeonList la bien Lua nen")
h("-- CHI song trong state cua tep nay; neu chua co thi dung lai tu bien mission.")
h("function YDBZ_HOOK_LayPhoBan(nMapId)")
h("\tlocal p = DungeonList[nMapId]")
h("\tif p then")
h("\t\treturn p")
h("\tend")
h("\tif DG_GetVar(nMapId, DG_V_TYPE) ~= DG_TYPE_YDBZ_READY then")
h("\t\treturn nil")
h("\tend")
h("\t-- dung lai vo boc: moi so lieu that deu nam o bien mission cua ban do")
h("\tlocal tb = {}")
h("\tlocal k, v")
h("\tfor k, v in ReadyMap do")
h("\t\ttb[k] = v")
h("\tend")
h("\ttb.nMapId = nMapId")
h("\ttb.nTemplateMapId = DG_GetVar(nMapId, DG_V_TPL)")
h("\ttb.tbNpc = {}")
h("\tDungeonList[nMapId] = tb")
h("\treturn tb")
h("end")
h("")
h("function OnNewWorld(szParam)")
h("\tlocal nMapId = SubWorldIdx2ID(SubWorld)")
h("\tlocal p = YDBZ_HOOK_LayPhoBan(nMapId)")
h("\tif p then")
h("\t\tp:OnEnterMap()")
h("\tend")
h("end")
h("")
h("function OnLeaveWorld(szParam)")
h("\tlocal nMapId = SubWorldIdx2ID(SubWorld)")
h("\tlocal p = YDBZ_HOOK_LayPhoBan(nMapId)")
h("\tif p then")
h("\t\tp:OnLeaveMap()")
h("\tend")
h("end")
h("")

for rel, lines in ((r"\script\missions\yandibaozang\readymap\readymap.lua", R),
                   (r"\script\missions\yandibaozang\readymap\ydbz_maphook.lua", H)):
    noi_dung = "\r\n".join(lines)
    for goc in (SRV, GUONG):
        p = goc + rel
        os.makedirs(os.path.dirname(p), exist_ok=True)
        tam = p + ".dangghi"
        with open(tam, "wb") as f:
            f.write(noi_dung.encode("latin-1"))
        os.replace(tam, p)
    print("da ghi %-30s (%d dong)" % (os.path.basename(rel), len(lines)))
