-- [BAIREN 23/08] Thay activitysys\config\9 (ServerStart) + zhaojingling\enternpc.lua (dat NPC).
-- Engine ta moi tep mot lua_State nen G_ACTIVITY khong dang ky duoc nhu Linux (SPEC 1.2);
-- Linux con XOA NPC nay luc boot (autoexec.lua:214 ClearNPCNewVersion) - chu game quyet dat lai.
Include("\\script\\missions\\basemission\\lib.lua")	-- basemission_CallNpc
BAIREN_HA = "\\script\\missions\\bairenleitai\\hundred_arena.lua"

function BairenLeitai_Init()
	-- = config\9\config.lua:12-16: Include hundred_arena.lua roi HundredArena.IniEnvironment(HundredArena)
	DynamicExecute(BAIREN_HA, "HundredArena:IniEnvironment")
	-- = zhaojingling\enternpc.lua:7-15 (toa do doc tu settings\maps\chrismas\enternpc.txt)
	local szPos = "\\settings\\maps\\chrismas\\enternpc.txt"
	if (TabFile_Load(szPos, szPos) == 0) then
		OutputMsg("[BAIREN] thieu "..szPos)
		return 0
	end
	basemission_CallNpc({
		szName = "Quan nh¾c nhë Hoµng Thµnh T­", nNpcId = 1747, nLevel = 95, nMapId = 176,
		nPosX = tonumber(TabFile_GetCell(szPos, 2, "POSX")),
		nPosY = tonumber(TabFile_GetCell(szPos, 2, "POSY")),
		szScriptPath = "\\script\\missions\\bairenleitai\\npc_quan_jx1.lua",
	})
	OutputMsg("[BAIREN] Bach Nhan Loi Dai khoi dong")
	return 1
end

-- Lenh bai Admin: khoi tao lai (CHI dung khi boot loi - goi lan 2 se tao them timer 1 phut)
function BairenLeitai_Adm_Init()
	return BairenLeitai_Init()
end
