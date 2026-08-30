-- ============================================================================
-- CH_THUONG_LIB.LUA - CONG TRAO THUONG dung chung
-- [CONGTHUONG 29/08]
--
-- !! TEP NAY LA "LA": CAM Include bat cu gi vao day.
--
-- CACH DUNG trong mot hoat dong:
--    Include("\\script\\cauhinh\\ch_lib.lua")
--    Include("\\script\\cauhinh\\ch_thuong.lua")
--    Include("\\script\\cauhinh\\ch_thuong_lib.lua")
--    ...
--    local nDaTrao = G_TraoThuong("TK_THUONG_QUANQUAN", "TONGKIM")
--
-- VI SAO CAN: hien co 475 cho goi AddItem rai rac 130 tep, moi cho tu kiem tui
-- mot kieu (114 CalcFreeItemCellCount, 16 CountFreeRoomByWH, 13 CheckRoom,
-- 5 CheckFreeBoxItem) va nhieu cho KHONG kiem gi - tui day la mat mon thuong
-- ma khong co dau vet nao.
-- ============================================================================

-- Ghi mot dong log thuong. Dung GhiLog (ham C, ghi logs\hethong.log) neu co;
-- khong thi in ra console de it nhat con thay duoc.
function G_LogThuong(szNoiDung)
	if (G_CFG ~= nil and G_CFG("CH_LOG_THUONG", 1) == 0) then
		return
	end
	if (GhiLog ~= nil) then
		GhiLog("THUONG", szNoiDung)
	else
		print("[THUONG] "..szNoiDung)
	end
end

-- Dem so mon trong mot bang thuong (moi muc: {soluong, {g,d,p}, "ten"}).
function G_DemMonThuong(tbBang)
	if (tbBang == nil) then
		return 0
	end
	return getn(tbBang)
end

-- Trao mot bang thuong cho nguoi choi HIEN TAI.
--   szKhoaBang : ten khoa trong tbCFG_THUONG (tra qua G_CFG)
--   szNguon    : ten hoat dong, chi de ghi log cho de tra
-- Tra ve so mon da trao duoc (0 neu khong trao duoc mon nao).
function G_TraoThuong(szKhoaBang, szNguon)
	local tbBang = G_CFG(szKhoaBang, nil)
	if (tbBang == nil) then
		G_LogThuong("THIEU BANG THUONG: "..szKhoaBang.." (nguon "..szNguon..")")
		return 0
	end

	local nSoMon = getn(tbBang)
	if (nSoMon <= 0) then
		G_LogThuong("BANG THUONG RONG: "..szKhoaBang)
		return 0
	end

	-- Kiem du o TRUOC khi trao mon dau tien. Thieu o thi bao nguoi choi va
	-- KHONG trao gi ca - tot hon la trao nua voi roi mat phan con lai.
	local nOTrong = CalcFreeItemCellCount()
	if (nOTrong < nSoMon) then
		G_LogThuong("TUI DAY: "..szKhoaBang.." can "..nSoMon
			.." o, con "..nOTrong.." (nguon "..szNguon..")")
		Msg2Player("Hµnh trang cÇn trèng "..nSoMon
			.." « míi nhËn ®­îc phÇn"
			.." th­ëng.")
		return 0
	end

	local nDaTrao = 0
	for i = 1, nSoMon do
		local tbMuc = tbBang[i]
		if (tbMuc ~= nil and tbMuc[1] ~= nil and tbMuc[2] ~= nil) then
			local nSL = tbMuc[1]
			local tbMa = tbMuc[2]
			local szTen = tbMuc[3]
			if (szTen == nil) then
				szTen = "?"
			end
			if (tbMa[1] ~= nil and tbMa[2] ~= nil and tbMa[3] ~= nil) then
				AddItem(tbMa[1], tbMa[2], tbMa[3], nSL, 0, 0, 0)
				nDaTrao = nDaTrao + 1
				G_LogThuong(szNguon.." | "..szKhoaBang.." | "..szTen
					.." ("..tbMa[1]..","..tbMa[2]..","..tbMa[3]..") x"..nSL)
			else
				G_LogThuong("MUC SAI DINH DANG: "..szKhoaBang.." muc "..i)
			end
		end
	end

	return nDaTrao
end

-- Trao mot mon le, van co kiem o trong + ghi log. Dung cho cho chi trao 1 mon.
function G_TraoMon(nGenre, nDetail, nParticular, nSoLuong, szTen, szNguon)
	if (CalcFreeItemCellCount() < 1) then
		G_LogThuong("TUI DAY khi trao "..szTen.." (nguon "..szNguon..")")
		Msg2Player("Hµnh trang ®· ®Çy, kh«ng"
			.." nhËn ®­îc phÇn th­ëng.")
		return 0
	end
	AddItem(nGenre, nDetail, nParticular, nSoLuong, 0, 0, 0)
	G_LogThuong(szNguon.." | "..szTen.." ("..nGenre..","..nDetail..","
		..nParticular..") x"..nSoLuong)
	return 1
end
