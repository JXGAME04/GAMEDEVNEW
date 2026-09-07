-- [LOCAL54 06/09 toi] 1 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local JiluDeathCount
Include("\\script\\missions\\fengling_ferry\\fld_head.lua")

function OnDeath(Launcher)
	Msg2Player("B¹n kh«ng may tö vong trong lóc ®i thuyÒn.")
--	Msg2MSAll(GetName().."ÔÚ¶É´¬ÉÏ²»ÐÒÉíÍö¡£")
	camp = GetCamp()
	SetCurCamp(camp)
--	SetCreateTeam(1)
--	SetDeathScript("")
	JiluDeathCount(1)		-- Êý¾ÝÂñµãµÚÒ»ÆÚ
	
	DelMSPlayer(MISSIONID, 0)	--Ç¿ÖÆ¸ÃÍæ¼ÒÀë¿ªMission
	SetLogoutRV(0);
		
	local mapid = SubWorldIdx2ID(SubWorld)
	if (mapid == 337) then
		NewWorld(fld_landingpos(1))
	elseif (mapid == 338) then
		NewWorld(fld_landingpos(2))
	elseif (mapid == 339) then
		NewWorld(fld_landingpos(3))
	else
		print("error:i don't know why")
	end
end

function JiluDeathCount(nCount)
	local mapid = SubWorldIdx2ID(SubWorld)
	if (mapid == 337) then
		AddStatData("fld_chuan1siwangrenshu", nCount)
	elseif (mapid == 338) then
		AddStatData("fld_chuan2siwangrenshu", nCount)
	elseif (mapid == 339) then
		AddStatData("fld_chuan3siwangrenshu", nCount)
	end
end

-- [3HD 25/08 C41] Engine JX1 goi OnRevive cho MOI NPC co script chet moi lan
-- hoi sinh; ban Linux khong dinh nghia (engine JX2 khong goi) => ScriptError
-- "attempt to call a nil value" lap lai moi nhip. Ham rong = giu nguyen hanh vi.
function OnRevive()
end
