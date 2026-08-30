-- ============================================================================
-- CH_LIB.LUA - bo doc cau hinh dung chung
-- [CAUHINH 29/08]
--
-- !! TEP NAY LA "LA": CAM Include bat cu gi vao day.
--    (Bai hoc that: vong Include 23/08 lam GameServer boot stack overflow -
--     xem ghi chu dau script\header\cauhinh_hoatdong.lua)
--
-- CACH DUNG: sua so trong bang duoi day roi KHOI DONG LAI GameServer.
--    Muon doc trong script:  local x = G_CFG("TEN_KHOA", giatri_mac_dinh)
--    (nho Include \script\cauhinh\ch_lib.lua truoc khi goi G_CFG)
-- ============================================================================

-- Tra cuu lan luot qua cac bang cau hinh DANG CO MAT trong state nay.
-- Bang nao chua nap thi bo qua - khong loi, khong canh bao.
-- Bang cuoi cung la tbCHD cua cauhinh_hoatdong.lua (tuong thich nguoc: moi
-- khoa TW_/BR_/BW_/TC_/YDBZ_ cu van tra duoc bang G_CFG).
-- [PHANBIEN 29/08] LUAT KHONG GIAN KHOA (quan trong):
--   Du an da co san ham HD_CFG doc bang tbCHD (header\cauhinh_hoatdong.lua).
--   HD_CFG CHI doc tbCHD, con G_CFG doc cac bang moi TRUOC roi moi den tbCHD.
--   => Neu mot khoa duoc khai o CA HAI noi thi hai ham cho HAI gia tri khac
--   nhau tuy cho goi. Vi vay: 5 tien to cu TW_ BR_ BW_ TC_ YDBZ_ VAN THUOC
--   tbCHD - CAM khai lai chung trong cac bang tbCFG_*.
function G_CFG(szKhoa, macdinh)
	if (szKhoa == nil) then
		return macdinh
	end
	if (tbCFG_CHUNG ~= nil and tbCFG_CHUNG[szKhoa] ~= nil) then
		return tbCFG_CHUNG[szKhoa]
	end
	if (tbCFG_LICH ~= nil and tbCFG_LICH[szKhoa] ~= nil) then
		return tbCFG_LICH[szKhoa]
	end
	if (tbCFG_THUONG ~= nil and tbCFG_THUONG[szKhoa] ~= nil) then
		return tbCFG_THUONG[szKhoa]
	end
	if (tbCFG_EXP ~= nil and tbCFG_EXP[szKhoa] ~= nil) then
		return tbCFG_EXP[szKhoa]
	end
	if (tbCFG_DROP ~= nil and tbCFG_DROP[szKhoa] ~= nil) then
		return tbCFG_DROP[szKhoa]
	end
	if (tbCHD ~= nil and tbCHD[szKhoa] ~= nil) then
		return tbCHD[szKhoa]
	end
	return macdinh
end

-- Ban NGHIEM NGAT: thieu khoa thi ghi log de con biet ma sua, roi van tra
-- gia tri mac dinh (khong bao gio nem loi lam dut ca hoat dong).
function G_CFG_BUOC(szKhoa, macdinh)
	-- [PHANBIEN 29/08] szKhoa co the la nil (ten khoa lay tu bien chua gan);
	-- noi chuoi voi nil lam DUT ca hoat dong, dung cai ma ham nay hua la
	-- khong bao gio nem loi.
	local szK = szKhoa
	if (szK == nil or type(szK) ~= "string") then
		szK = "(khong ten)"
	end
	local ra = G_CFG(szKhoa, nil)
	if (ra == nil) then
		if (GhiLog ~= nil) then
			GhiLog("CAUHINH", "thieu khoa: "..szK)
		else
			print("[CAUHINH] thieu khoa: "..szK)
		end
		return macdinh
	end
	return ra
end

-- 1730 -> "17:30" (ghep vao cau thoai NPC)
function G_GioPhut(nHHMM)
	return format("%02d:%02d", floor(nHHMM/100), mod(nHHMM, 100))
end

-- Doi HHMM sang so phut trong ngay, de so sanh moc gio cho gon.
function G_HHMM2Phut(nHHMM)
	return floor(nHHMM/100) * 60 + mod(nHHMM, 100)
end

-- Kiem gio hien tai co nam trong khoang [nBatDau, nKetThuc] dang HHMM khong.
-- Khoang qua nua dem (vd 2300 -> 0100) cung dung.
function G_TrongKhoangGio(nBatDau, nKetThuc)
	local _, _, _, nHr, nMi = GetTimeNow()
	local nNay = nHr * 60 + nMi
	local nA = G_HHMM2Phut(nBatDau)
	local nB = G_HHMM2Phut(nKetThuc)
	if (nA <= nB) then
		if (nNay >= nA and nNay <= nB) then
			return 1
		end
		return 0
	end
	if (nNay >= nA or nNay <= nB) then
		return 1
	end
	return 0
end
