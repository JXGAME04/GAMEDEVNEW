-- ============================================================
-- TONGCASTLE DRIVER -- [TONGCASTLE 23/08] - thay relaysetting\task\tongcastle.lua (tick MOI PHUT)
-- + autoexec.lua AutoFunctions:Run (khuon tong_driver / tongwar_driver).
-- CHI Run() o state chu; guideperson goi thang CreateGuideNpc (Run() o do se chay ca
-- TongCastle.Start cua BAN SAO -> nhan doi cay/trap).
-- ============================================================

TC_DRV_MSKEY   = 30	-- khoa missionId GlbTimer (27 WLLS / 28 TONG / 29 TONGWAR)
TC_DRV_TIMERID = 55	-- settings\TimerTask.txt khoa 55
TC_DRV_LASTMIN = -1
TC_RELAY = "\\script\\mission\\tongcastle\\tongcastle.lua"

function TONGCASTLE_DriverInit(szParam)
	DynamicExecute("\\script\\missions\\tongcastle\\tongcastle.lua", "AutoFunctions:Run")	-- TongCastle:Start (trap/cay/diem)
	DynamicExecute("\\script\\missions\\tongcastle\\guideperson.lua", "CreateGuideNpc")
	DynamicExecute("\\script\\missions\\tongcastle\\guideperson.lua", "CreateGuideNpcInside")
	TC_DRV_LASTMIN = tonumber(date("%y%m%d%H%M"))
	StartGlbMSTimer(TC_DRV_MSKEY, TC_DRV_TIMERID, 30 * 18)
	OutputMsg("[TONGCASTLE] Bang Hoi Thanh Bao khoi dong")
	return 1
end

function OnTimer()
	local m = tonumber(date("%y%m%d%H%M"))
	if (m ~= TC_DRV_LASTMIN) then
		TC_DRV_LASTMIN = m
		DynamicExecute(TC_RELAY, "tbS3TongCastle:CheckAndReviveTree")
		DynamicExecute(TC_RELAY, "tbS3TongCastle:CheckAndDeleteTree")
	end
end

-- Lenh bai Admin: ep hoi sinh cay loai n (1/2/3) de test ngoai gio
function TONGCASTLE_Adm_ReviveTree(n)
	-- [phan bien F16] relay SaveNpcData APPEND record - phai xoa record loai n truoc, khong thi x2 diem thu/cay chong
	DynamicExecute(TC_RELAY, "tbS3TongCastle:DelAllTreeData", n or 1)
	DynamicExecute("\\script\\missions\\tongcastle\\tongcastle.lua", "TongCastle:AddTreeInMap", 984, n or 1)
	return 1
end
