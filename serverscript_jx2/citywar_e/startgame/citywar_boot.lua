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
	WriteLog("[citywar] boot: league 4/508/509 + GlbMission 8 OK")
end
