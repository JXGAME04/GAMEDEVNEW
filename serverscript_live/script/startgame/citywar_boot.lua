-- citywar_boot.lua - DOT E (E5): boot cong thanh JX2.
-- Goc autoexec.lua:142 OpenGlbMission(8) + :147 buildAllCityInfoLeague; rieng
-- checkCityWarLeague GS goc KHONG ai goi (league 508/509 do relay tao 0h) -
-- ta 1 GS nen PHAI tu goi luc boot + 0h (DIEUTRA_LEAGUE_SPEC muc 7).
-- Cac ham chay trong STATE nay nhung kho LG_/GlbValue la C++ toan cuc (Q2).
Include("\\script\\misc\\league_cityinfo.lua")
Include("\\script\\missions\\citywar_global\\citywar_function.lua")

function CityWar_Boot()
	buildAllCityInfoLeague()
	checkCityWarLeague()
	OpenGlbMission(8)
	-- NPC bao danh cong thanh (infocenter.lua goc): goc JX2 dat o cac Tan Thu
	-- Thon bang npc-data map cua HO - cay ta phai tu spawn (canh 2 NPC Ho Tro,
	-- Ba Lang Huyen). Khong co NPC nay la KHONG CO CUA nop Khieu chien lenh.
	local nIC = AddNpcEx1({373},1,nil,53,1625*32,3170*32,"","\\script\\missions\\citywar_global\\sugia_congthanh.lua","Sø Gi¶ C«ng Thµnh",6)
	WriteLog("[citywar] boot: league 4/508/509 + GlbMission 8 OK, NPC infocenter="..nIC)
	CityWar_ArenaNpc()
end

-- [LOI DAI CN 21/08] NPC 2 phe tren 8 map loi dai 213..220 - goc nam trong npc-data
-- map (doc region maps.pak: tpl 178 o (1581,3257) camper1.lua, tpl 124 o (1603,3236)
-- camper2.lua) nhung engine ta bo NPC thoai trong map-data (NotAddNpcNormal=1) -> tu spawn.
function CityWar_ArenaNpc()
	local n = 0
	for m = 213, 220 do
		if (SubWorldID2Idx(m) >= 0) then
			AddNpcEx1({178},1,nil,m,1581*32,3257*32,"","\\script\\missions\\citywar_arena\\camper1.lua",nil,6)
			AddNpcEx1({124},1,nil,m,1603*32,3236*32,"","\\script\\missions\\citywar_arena\\camper2.lua",nil,6)
			n = n + 1
		end
	end
	WriteLog("[citywar] boot: NPC loi dai tren "..n.." map")
end
