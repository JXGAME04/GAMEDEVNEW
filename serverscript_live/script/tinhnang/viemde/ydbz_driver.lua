-- ============================================================================
-- YDBZ_DRIVER.LUA - Boot + Lich chay VIEM DE BAO TANG   (port 26/08)
-- SINH TU DONG boi ReverseTools\viemde\v19_gen_driver.py - DUNG SUA TAY
--
-- Thay tang S3Relay cua ban Linux (gateway\s3relay\relaysetting\task\
-- baotangviemde\*.lua) bang dong ho GameServer - dung KHUON da chung cua cac
-- dot port truoc (HD3_DriverInit / HD3_Tick trong hd3_driver.lua).
--   * YDBZ_DriverInit()   : goi 1 lan trong OnGame (startgame.lua) - dat NPC.
--   * YDBZ_Tick(nHr,nMi)  : goi moi phut trong RunTime (timerserver.lua) - lich.
-- Cau hinh so lieu: script\header\cauhinh_hoatdong.lua khoi [7], tien to YDBZ_.
--
-- LOGIC 100% BAN LINUX: den gio thi goi thang YDBZ_OnTrigger() cua
-- yandibaozang_trigger.lua, y het cau GlobalExecute cua ban Linux.
-- ============================================================================
Include("\\script\\header\\cauhinh_hoatdong.lua")
Include("\\script\\missions\\yandibaozang\\yandibaozang_trigger.lua")

YDBZ_SCRIPT_NPC = "\\script\\missions\\yandibaozang\\npc\\yandibaozang_main.lua"
YDBZ_NPC_TPL    = 1320    -- "Binh Binh co nuong" (npcs.txt JX1: Kind=3 Camp=6 Series=0)
YDBZ_NPC_LEVEL  = 80
YDBZ_NPC_SERIES = 4       -- goc: autoexec_npc.lua:141 tham so 3

-- 4 toa do (o) tren map 37 Bien Kinh - goc autoexec_npc.lua:141-144.
-- TRUNG KHOP voi tbReady:GetSignUpPos (noi day nguoi choi ra khi bi loai).
YDBZ_NPC_VITRI = {
	{1714, 3173},
	{1642, 3145},
	{1622, 3019},
	{1857, 2968},
}

-- ---------------------------------------------------------------------------
-- BOOT: dat 4 NPC bao danh
-- ---------------------------------------------------------------------------
function YDBZ_DriverInit()
	if HD_CFG("YDBZ_BAT", 1) ~= 1 then
		print("[ViemDe] YDBZ_BAT = 0 - bo qua boot.")
		return 0
	end
	-- Don NPC cu (chay lai DriverInit khong nhan ban NPC). HD3_DelNpcByScript
	-- dung strstr nen ten script phai KHONG la tien to cua tep khac (bay #13).
	if HD3_DelNpcByScript ~= nil then
		local nXoa = HD3_DelNpcByScript("yandibaozang_main")
		if nXoa > 0 then
			print("[ViemDe] don "..nXoa.." NPC bao danh cu.")
		end
	end
	local nMapId = HD_CFG("YDBZ_NPC_MAP", 37)
	local nIdx = SubWorldID2Idx(nMapId)
	if nIdx < 0 then
		print("[ViemDe] ban do "..nMapId.." CHUA NAP - khong dat duoc NPC bao danh.")
		return 0
	end
	local nDat = 0
	local i
	for i = 1, getn(YDBZ_NPC_VITRI) do
		local t = YDBZ_NPC_VITRI[i]
		local npc = nil
		if HD3_AddNpc ~= nil then
			npc = HD3_AddNpc(YDBZ_NPC_TPL, YDBZ_NPC_LEVEL, nIdx, t[1]*32, t[2]*32, YDBZ_NPC_SERIES, "B×nh B×nh c« n­¬ng")
		else
			-- lui ve AddNpcEx: (tpl, cap, ngu hanh, chi so ban do, x, y, phe, ten)
			npc = AddNpcEx(YDBZ_NPC_TPL, YDBZ_NPC_LEVEL, YDBZ_NPC_SERIES, nIdx, t[1]*32, t[2]*32, 6, "B×nh B×nh c« n­¬ng")
		end
		if npc ~= nil and npc > 0 then
			SetNpcScript(npc, YDBZ_SCRIPT_NPC)
			nDat = nDat + 1
		end
	end
	print("[ViemDe] da dat "..nDat.."/"..getn(YDBZ_NPC_VITRI).." NPC bao danh tren ban do "..nMapId..".")
	return nDat
end

-- ---------------------------------------------------------------------------
-- LICH: goi moi phut
-- ---------------------------------------------------------------------------
YDBZ_MOC_CUOI = -1   -- chong goi hai lan trong cung mot phut

function YDBZ_Tick(nHr, nMi)
	if HD_CFG("YDBZ_BAT", 1) ~= 1 then
		return 0
	end
	local nMoc = nHr * 100 + nMi
	if nMoc == YDBZ_MOC_CUOI then
		return 0
	end
	local szGio = HD_CFG("YDBZ_GIO", "0825,1025,1425,1625,1825,2025,2225")
	-- bao dau phay hai dau roi tim ",HHMM," - khop TRON MOC, khong the trung
	-- mot phan cua moc khac du nguoi van hanh go dang khac (vd "825").
	local nTim = strfind(","..szGio..",", ","..format("%04d", nMoc)..",", 1, 1)
	if nTim == nil then
		return 0
	end
	YDBZ_MOC_CUOI = nMoc
	print(format("[ViemDe] den gio %02d:%02d - mo bao danh.", nHr, nMi))
	if YDBZ_OnTrigger == nil then
		print("[ViemDe] KHONG THAY YDBZ_OnTrigger - kiem Include yandibaozang_trigger.lua.")
		return 0
	end
	YDBZ_OnTrigger()
	return 1
end
