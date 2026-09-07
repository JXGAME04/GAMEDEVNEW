-- -- [TONGWAR 23/08] map chien truong 605-607: KNpc::ChangeWorld JX1 KHONG go nguoi choi khoi mission
-- khi doi map (m_MissionArray.RemovePlayer bi chu thich, chi go luc logout) -> OnLeave cua
-- mission.lua khong bao gio chay khi bi day ra / dung Tho Dia Phu; tu go o day.
-- Khuon: missions\leaguematch\combat\newworld.lua
Include("\\script\\lib\\common.lua")	-- [TONGWAR 23/08 phan bien F11] split() cho PraseParam
Include("\\script\\maps\\newworldscript_default.lua")

function OnNewWorld(szParam)
	OnNewWorldDefault(szParam)
end

function OnLeaveWorld(szParam)
	DelMSPlayer(33, 0)	-- mission 33 Bang Chien
	OnLeaveWorldDefault(szParam)
end
