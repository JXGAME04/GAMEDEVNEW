-- ============================================================================
-- HD3_THUYENPHU.LUA - wrapper NPC thuyen phu Phong Lang Do (logic 100% Linux).
-- Dat bien toan cuc BOATID theo gia tri NPC (1/2/3) roi goi fld_wanttakeboat
-- cua ban Linux (fld_head.lua). Toan bo dang ky/le vat do fld_head xu ly.
-- ============================================================================
Include("\\script\\missions\\fengling_ferry\\fld_head.lua")

function main(NpcIndex)
	BOATID = GetNpcValue(NpcIndex)	-- PHAI truyen NpcIndex: LuaGetNpcParam thieu tham so -> tra 0 gia tri
	if (BOATID == nil or BOATID < 1 or BOATID > 3) then BOATID = 1 end
	fld_wanttakeboat(BOATID)
end
