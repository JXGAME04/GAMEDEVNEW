-- ==========================================================================
-- VIEM DE - HOOK VAO/ROI BAN DO PHONG CHO
-- SINH TU DONG boi ReverseTools\viemde\v12_gen_readymap.py - DUNG SUA TAY
--
-- MapList.ini tro <id>_NewWorldScript cua 15 ban do trong be vao tep nay.
-- Engine ban "OnLeaveWorld" cho ban do CU va "OnNewWorld" cho ban do MOI moi
-- khi nguoi choi doi ban do (KNpc.cpp:9995-9996), voi bien toan cuc SubWorld
-- va PlayerIndex da duoc bom san (ScriptFuns.cpp:11270).
--
-- Ban Linux khong can tep nay: engine cua no goi thang OnEnterMap/OnLeaveMap
-- cua doi tuong pho ban.
-- ==========================================================================

Include("\\script\\lib\\common.lua")
Include("\\script\\missions\\basemission\\dungeon.lua")
Include("\\script\\missions\\yandibaozang\\readymap\\readymap.lua")

-- Tim lai doi tuong pho ban cua ban do dang xet. DungeonList la bien Lua nen
-- CHI song trong state cua tep nay; neu chua co thi dung lai tu bien mission.
function YDBZ_HOOK_LayPhoBan(nMapId)
	local p = DungeonList[nMapId]
	if p then
		return p
	end
	if DG_GetVar(nMapId, DG_V_TYPE) ~= DG_TYPE_YDBZ_READY then
		return nil
	end
	-- dung lai vo boc: moi so lieu that deu nam o bien mission cua ban do
	local tb = {}
	local k, v
	for k, v in pairs(ReadyMap) do
		tb[k] = v
	end
	tb.nMapId = nMapId
	tb.nTemplateMapId = DG_GetVar(nMapId, DG_V_TPL)
	tb.tbNpc = {}
	DungeonList[nMapId] = tb
	return tb
end

function OnNewWorld(szParam)
	local nMapId = SubWorldIdx2ID(SubWorld)
	local p = YDBZ_HOOK_LayPhoBan(nMapId)
	if p then
		p:OnEnterMap()
	end
end

function OnLeaveWorld(szParam)
	local nMapId = SubWorldIdx2ID(SubWorld)
	local p = YDBZ_HOOK_LayPhoBan(nMapId)
	if p then
		p:OnLeaveMap()
	end
end
