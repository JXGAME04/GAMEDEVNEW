-- ============================================================
-- WLLS GS DRIVER - thay the task centre cua relay khi gop ve 1 GS.
-- File MOI duy nhat cua dot port leaguematch; moi file khac chep
-- nguyen ban tu ban Linux (xem D:\GAMEDEVNEW\THICONG_LIENDAU_PORT.md).
-- Nhiem vu:
--   1) WLLS_DriverInit(): TaskShedule() (khoi tao dung nhu relay boot:
--      xoa RLGLB 121..145, tinh pha, wlls_set_mid(sid, 0)) roi
--      GameSvrReady(0) (push pha hien tai xuong GS nhu khi GS noi relay).
--   2) OnTimer() moi 30 giay: khi sang quy 15 phut moi theo dong ho
--      tuong (bam :00/:15/:30/:45 dung nhu TaskTime + TaskInterval(15)
--      cua relay goc) thi goi TaskContent().
-- TaskName/TaskTime/TaskInterval/TaskCountLimit la stub no-op phia C
-- nen task.lua duoc giu nguyen byte ban goc.
-- ============================================================
Include("\\script\\leaguematch\\task.lua")

WLLS_DRV_MSKEY   = 27	-- khoa missionId rieng cho GlbTimer cua driver (khong dung mission that)
WLLS_DRV_TIMERID = 52	-- settings\TimerTask.txt dong 53 tro nguoc ve file nay
WLLS_DRV_LASTQ   = -1

-- so thu tu quy 15 phut hien tai (theo dong ho tuong cua may chu)
function WLLS_Drv_CurQ()
	return tonumber(date("%y%m%d%H")) * 4 + floor(tonumber(date("%M")) / 15)
end

function WLLS_DriverInit(szParam)
	TaskShedule()
	GameSvrReady(0)
	WLLS_DRV_LASTQ = WLLS_Drv_CurQ()
	StartGlbMSTimer(WLLS_DRV_MSKEY, WLLS_DRV_TIMERID, 30 * 18)
	OutputMsg("[WLLS] Driver lien dau da khoi dong (quy "..WLLS_DRV_LASTQ..")")
	return 1
end

function OnTimer()
	local q = WLLS_Drv_CurQ()
	if (q ~= WLLS_DRV_LASTQ) then
		WLLS_DRV_LASTQ = q
		TaskContent()
	end
end

-- ====== [21/08] hµm phôc vô LÖnh bµi Admin (script\item\liendau_admin.lua) ======
-- Cho Ðp pha bÊt kú ®Ó test ngoµi giê; n_phase: 1 nghØ mïa / 2 trong mïa / 4 më trËn.
function WLLS_Adm_ForcePhase(n_phase)
	local _, n_mid, n_sid = wlls_calc_phase()
	if (n_phase == 4) then
		if (n_mid == 0) then
			n_mid = tonumber(date("%y%m%d"))*100 + 99	-- m· trËn test ngoµi giê
		end
	else
		n_mid = 0
	end
	wlls_set_phase(n_phase, n_mid, n_sid)
	OutputMsg("[WLLS] ADMIN ep pha "..n_phase.." (mid="..n_mid..", mua "..n_sid..")")
	return 1
end

-- Trë vÒ ®óng pha theo ®ång hå thËt
function WLLS_Adm_PhaThat()
	TaskContent()
	OutputMsg("[WLLS] ADMIN tro ve pha theo dong ho that")
	return 1
end

-- Khëi t¹o l¹i nh­ lóc boot
function WLLS_Adm_KhoiTaoLai()
	TaskShedule()
	GameSvrReady(0)
	OutputMsg("[WLLS] ADMIN da khoi tao lai driver")
	return 1
end
