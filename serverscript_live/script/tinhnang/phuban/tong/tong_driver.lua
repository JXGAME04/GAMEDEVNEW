-- [LOCAL54 06/09 toi] 1 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local TONG_Drv_CurQ
-- ============================================================
-- TONG DRIVER - thay the task centre cua RELAY cho 3 Hoat dong Phuong bang hoi
-- (khuon y het \script\leaguematch\gsdriver.lua cua dot Lien Dau 20/08).
-- File MOI duy nhat cua dot port missions\tong (21/08); moi file khac chep
-- nguyen ban tu D:\ServerLinux (41 tep missions\tong + 3 tep relay\).
--
-- Ban Linux: relay chay 3 task trong relaysetting\task\ (tong_disciple /
-- tong_springfestival / tong_collectgoods): TaskShedule() luc boot, roi
-- TaskContent() moi 15 phut (TaskInterval(15), bam :00/:15/:30/:45); den gio
-- (mod(gio,3) == 0 / 1 / 2, phut <= 55) thi GlobalExecute("dwf ... open...(map)")
-- -> GS mo mission chuan bi (37 / 39 / 44).
-- Ta: 3 tep relay chep nguyen ban vao missions\tong\relay\, MOI TEP MOT lua_State
-- (engine ta), nen driver goi qua DynamicExecute de TaskShedule/TaskContent cua
-- 3 tep KHONG de len nhau. TaskName/TaskTime/TaskInterval/TaskCountLimit la stub C.
-- ============================================================

TONG_DRV_MSKEY   = 28	-- khoa missionId rieng cho GlbTimer cua driver (27 = WLLS)
TONG_DRV_TIMERID = 53	-- settings\TimerTask.txt khoa 53 tro nguoc ve file nay
TONG_DRV_LASTQ   = -1

TONG_DRV_RELAY = {
	"\\script\\missions\\tong\\relay\\tong_disciple.lua",
	"\\script\\missions\\tong\\relay\\tong_springfestival.lua",
	"\\script\\missions\\tong\\relay\\tong_collectgoods.lua",
}

-- so thu tu quy 15 phut hien tai (theo dong ho tuong cua may chu)
function TONG_Drv_CurQ()
	return tonumber(date("%y%m%d%H")) * 4 + floor(tonumber(date("%M")) / 15)
end

function TONG_DriverInit(szParam)
	for i = 1, getn(TONG_DRV_RELAY) do
		DynamicExecute(TONG_DRV_RELAY[i], "TaskShedule")
	end
	TONG_DRV_LASTQ = TONG_Drv_CurQ()
	StartGlbMSTimer(TONG_DRV_MSKEY, TONG_DRV_TIMERID, 30 * 18)
	OutputMsg("[TONG] Driver hoat dong phuong da khoi dong (quy "..TONG_DRV_LASTQ..")")
	return 1
end

function OnTimer()
	local q = TONG_Drv_CurQ()
	if (q ~= TONG_DRV_LASTQ) then
		TONG_DRV_LASTQ = q
		for i = 1, getn(TONG_DRV_RELAY) do
			DynamicExecute(TONG_DRV_RELAY[i], "TaskContent")
		end
	end
end

-- ====== ham phuc vu Lenh bai Admin: ep mo ngay mot hoat dong de test ngoai gio ======
-- n: 1 chieu mo de tu (map chuan bi 821) / 2 nien thu (823) / 3 thu thap vat tu (827)
function TONG_Adm_MoNgay(n)
	if (n == 1) then
		DynamicExecute("\\script\\missions\\tong\\tong_disciple\\dis_gmscript.lua", "tong_opendisciple", 821)
	elseif (n == 2) then
		DynamicExecute("\\script\\missions\\tong\\tong_springfestival\\sf_gmscript.lua", "tong_openspringfestival", 823)
	elseif (n == 3) then
		DynamicExecute("\\script\\missions\\tong\\collectgoods\\collg_gmscript.lua", "collg_opencellectgoods", 827)
	end
	OutputMsg("[TONG] ADMIN ep mo hoat dong "..n)
	return 1
end
