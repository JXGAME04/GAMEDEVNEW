-- ============================================================
-- TONGWAR DRIVER -- [TONGWAR 23/08] - thay task centre relay cho Bang Chien / Vo Lam De Nhat Bang
-- (khuon y het \script\missions\tong\tong_driver.lua cua dot Hoat dong Phuong 21/08).
-- Relay Linux: tasklist.ini [Task_64] tongwar.lua - TaskShedule() luc boot roi TaskContent()
-- moi 30 phut; cua so +-5 phut cua 20:00/21:30 chi khop tick :00/:30 -> tick 15 phut tuong duong.
-- ============================================================

TONGWAR_DRV_MSKEY   = 29	-- khoa missionId rieng cho GlbTimer (27 = WLLS, 28 = TONG)
TONGWAR_DRV_TIMERID = 54	-- settings\TimerTask.txt khoa 54 tro nguoc ve tep nay
TONGWAR_DRV_LASTQ   = -1
TONGWAR_DRV_RELAY   = "\\script\\missions\\tongwar\\relay\\tongwar.lua"

function TONGWAR_Drv_CurQ()
	return tonumber(date("%y%m%d%H")) * 4 + floor(tonumber(date("%M")) / 15)
end

function TONGWAR_DriverInit(szParam)
	DynamicExecute(TONGWAR_DRV_RELAY, "TaskShedule")
	TONGWAR_DRV_LASTQ = TONGWAR_Drv_CurQ()
	StartGlbMSTimer(TONGWAR_DRV_MSKEY, TONGWAR_DRV_TIMERID, 30 * 18)
	OutputMsg("[TONGWAR] Driver Bang Chien khoi dong (quy "..TONGWAR_DRV_LASTQ..")")
	return 1
end

function OnTimer()
	local q = TONGWAR_Drv_CurQ()
	if (q ~= TONGWAR_DRV_LASTQ) then
		TONGWAR_DRV_LASTQ = q
		DynamicExecute(TONGWAR_DRV_RELAY, "TaskContent")
	end
end

-- ====== Lenh bai Admin: ep chay ngay TaskContent / khoi tao mua de test ======
function TONGWAR_Adm_TaskContent()
	DynamicExecute(TONGWAR_DRV_RELAY, "TaskContent")
	return 1
end
function TONGWAR_Adm_InitMatch(nSeason)
	DynamicExecute(TONGWAR_DRV_RELAY, "tongwar_initmatch", nSeason or 9)
	return 1
end
