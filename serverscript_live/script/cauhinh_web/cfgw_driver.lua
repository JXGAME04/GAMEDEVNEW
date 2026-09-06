-- ============================================================================
-- CFGW_DRIVER.LUA - [CFGW 04/09] CAU HINH GAME chinh tu WEB ADMIN (bang MySQL gcfg)
--
-- Chu game 04/09: "config toan bo len web de chinh cho de" (exp, exp ky nang,
-- rot do, tien, thoi gian hoat dong).
--
-- Viec cua tep nay (Include tu timerserver.lua, chay lai moi phut theo nhip dofile):
--   1. KHAI moi khoa cau hinh script (tbCFG_CHUNG/LICH/THUONG/EXP/DROP + tbCHD)
--      len bang gcfg qua CFGW_Khai(...) kem mo ta/nhom/kieu/don vi tu cfgw_meta.lua
--      (C++ nho khoa da khai -> cac lan sau la khong ton gi).
--   2. Nhip 30 giay CFGW_Tick30: hoi CFGW_Tick() -> C++ do gcfg_config.cfg_version,
--      doi thi nap lai. Khoa C++ (ExpRate, MoneyRate, Skill90/120Rate, [Exp].*)
--      an NGAY; khoa script an qua G_CFG/HD_CFG (hai ham do hoi CFGW_Get truoc).
--
-- AN TOAN voi CoreServer CU (chua co ham CFGW_*): moi cho deu kiem nil -> tep nay
-- khong lam gi ca, hanh vi y nguyen.
-- Bang gcfg do MAY CHU tao; web chi SELECT/UPDATE. Xem BANGIAO_CAUHINH_WEB_0409.md
-- ============================================================================

Include("\\script\\cauhinh\\ch_all.lua")
Include("\\script\\header\\cauhinh_hoatdong.lua")
Include("\\script\\cauhinh_web\\cfgw_meta.lua")

CFGW_FRAMES = 30 * 18          -- nhip do cfg_version (khung)
CFGW_GLB    = 9004             -- GLB moc nhip cuoi (9001 mail, 9002/9003 dau gia)

-- Khai MOT bang cau hinh: chi khoa co gia tri so / chuoi (bo nil, bo bang con).
-- Muc meta = { nhom, mo ta, kieu, min, max, ap_dung, nguon, don_vi } (cfgw_meta.lua).
-- Tra ve so khoa khai duoc trong lan goi nay.
function CFGW_KhaiBang(tb, szNguon, szNhomMacDinh)
	if (tb == nil or CFGW_Khai == nil) then
		return 0
	end
	local nOk = 0
	for k, v in pairs(tb) do
		local kieu = -1
		if (type(v) == "number") then
			kieu = 0
			if (v ~= floor(v)) then
				kieu = 1
			end
		elseif (type(v) == "string") then
			kieu = 2
		end
		if (kieu >= 0 and type(k) == "string") then
			local nhom, mota, mn, mx, ap, donvi = szNhomMacDinh, "", 0, 0, 1, ""
			local ten, gt, cb, nc = "", "", "", 1		-- [05/09] tieng Viet: ten / giai thich / canh bao / nguy co
			local m = nil
			if (tbCFGW_META ~= nil) then
				m = tbCFGW_META[k]
			end
			if (m ~= nil) then
				nhom = m[1]
				mota = m[2]
				if (m[3] ~= nil and m[3] >= 0 and kieu ~= 2) then
					kieu = m[3]
				end
				mn = m[4]
				mx = m[5]
				ap = m[6]
				if (m[8] ~= nil) then
					donvi = m[8]
				end
				if (m[9] ~= nil) then
					ten = m[9]
					gt = m[10]
					cb = m[11]
					nc = m[12]
				end
			end
			-- DLL 04/09 (CFGW_Khai 10 doi so) bo qua 4 doi so thua - khong loi
			if (CFGW_Khai(k, tostring(v), nhom, kieu, mn, mx, mota, szNguon, ap, donvi, ten, gt, cb, nc) == 1) then
				nOk = nOk + 1
			end
		end
	end
	return nOk
end

function CFGW_KhaiTatCa()
	if (CFGW_Khai == nil) then
		return 0
	end
	local n = 0
	n = n + CFGW_KhaiBang(tbCFG_CHUNG,  "ch_chung.lua",         "CHUNG")
	n = n + CFGW_KhaiBang(tbCFG_LICH,   "ch_lich.lua",          "LICH")
	n = n + CFGW_KhaiBang(tbCFG_THUONG, "ch_thuong.lua",        "THUONG")
	n = n + CFGW_KhaiBang(tbCFG_EXP,    "ch_exp.lua",           "EXP")
	n = n + CFGW_KhaiBang(tbCFG_DROP,   "ch_drop.lua",          "ROTDO")
	n = n + CFGW_KhaiBang(tbCHD,        "cauhinh_hoatdong.lua", "HOATDONG")
	CFGW_MoTaCpp()
	return n
end

-- [05/09] Tieng Viet cho khoa C++ (ServerConfig.* / Exp.*): C++ tu khai metadata cua no (chu ASCII),
-- script chi gui ten / giai thich / canh bao / nguy co qua CFGW_MoTa (bang tbCFGW_META_CPP trong
-- cfgw_meta.lua). Lam MOT lan moi tien trinh (co CFGW_DA_MOTA_CPP la global, song qua dofile);
-- MySQL chua san sang (tra 0) thi lan sau lam tiep. DLL cu khong co CFGW_MoTa -> bo qua.
function CFGW_MoTaCpp()
	if (CFGW_MoTa == nil or tbCFGW_META_CPP == nil or CFGW_DA_MOTA_CPP == 1) then
		return 0
	end
	local n = 0
	for k, m in pairs(tbCFGW_META_CPP) do
		if (CFGW_MoTa(k, m[1], m[2], m[3], m[4]) == 1) then
			n = n + 1
		end
	end
	if (n > 0) then
		CFGW_DA_MOTA_CPP = 1
	end
	return n
end

function CFGW_LoiLua(szLoi)
	print("[CFGW] loi Lua: "..tostring(szLoi))
end

function CFGW_Body()
	if (CFGW_Tick == nil) then
		return 1
	end
	CFGW_KhaiTatCa()          -- re: C++ bo qua khoa da khai; MySQL chua san sang thi lan sau khai tiep
	CFGW_Tick()               -- do cfg_version, doi thi nap lai
	return 1
end

function CFGW_Tick30(nParam, nTimerId)
	SetGlbValue(CFGW_GLB, GetCurrentTime())
	-- call(..., "x"): loi Lua trong than KHONG lan ra; lan ra la timer bi xoa vinh vien (bai hoc AucWeb)
	if (call(CFGW_Body, {}, "x", CFGW_LoiLua) == nil) then
		print("[CFGW] loi Lua trong CFGW_Body - bo qua nhip nay")
	end
	return CFGW_FRAMES
end

-- Dang ky vong quet 30 giay (khuon AucWeb: GLB = moc nhip cuoi; qua 120 s khong nhip = timer chet).
if (CFGW_DANGKY ~= 1 and CFGW_Tick ~= nil) then
	local nNhip = GetGlbValue(CFGW_GLB) or 0
	if (GetCurrentTime() - nNhip > 120) then
		CFGW_DANGKY = 1
		SetGlbValue(CFGW_GLB, GetCurrentTime())
		AddTimer(CFGW_FRAMES, "CFGW_Tick30", 0)
		print("[CFGW] dang ky vong do cau hinh web 30 giay")
	end
end

-- Khai + nap ngay luc nap script (boot va moi lan timerserver.lua tu nap lai).
if (CFGW_Khai ~= nil) then
	CFGW_KhaiTatCa()
	CFGW_Tick()
end
