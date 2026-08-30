-- ============================================================================
-- CH_THUONG_LIB.LUA - CONG TRAO THUONG dung chung
-- [CONGTHUONG 29/08] + [PHANBIEN 29/08] (sua 5 loi bo phan bien bat duoc)
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
-- DINH DANG MOT MUC THUONG:
--    { <so luong>, {<genre>, <detail>, <particular>}, "<ten>", <cap do> }
--    - <cap do> tuy chon, khong ghi thi 0. Chi trang bi moi can.
--    - Mon KHONG XEP CHONG DUOC (trang bi) thi phai khai TUNG DONG mot, moi
--      dong so luong 1 - vi so luong duoc truyen vao o "stack" cua engine.
--
-- VI SAO CAN CONG NAY: hien co 475 cho goi AddItem rai rac 130 tep, moi cho tu
-- kiem tui mot kieu (114 CalcFreeItemCellCount, 16 CountFreeRoomByWH, 13
-- CheckRoom, 5 CheckFreeBoxItem) va nhieu cho KHONG kiem gi.
-- ============================================================================

-- Doi mot gia tri bat ky thanh chuoi an toan de noi. [PHANBIEN 29/08]
-- (noi chuoi voi nil lam DUT ca hoat dong - Lua 4 nem loi o luaV_strconc)
function G_Chuoi(x, macdinh)
	if (x == nil) then
		if (macdinh == nil) then
			return "?"
		end
		return macdinh
	end
	if (type(x) == "string") then
		return x
	end
	if (type(x) == "number") then
		return ""..x
	end
	return "?"
end

-- Ghi mot dong log thuong. Dung GhiLog (ham C, ghi logs\hethong.log) neu co;
-- khong thi in ra console de it nhat con thay duoc.
function G_LogThuong(szNoiDung)
	if (G_CFG ~= nil and G_CFG("CH_LOG_THUONG", 1) == 0) then
		return
	end
	local sz = G_Chuoi(szNoiDung, "(rong)")
	if (GhiLog ~= nil) then
		GhiLog("THUONG", sz)
	else
		print("[THUONG] "..sz)
	end
end

-- Dem so mon trong mot bang thuong. Tra 0 neu khong phai bang.
function G_DemMonThuong(tbBang)
	if (tbBang == nil or type(tbBang) ~= "table") then
		return 0
	end
	return getn(tbBang)
end

-- Trao mot mon. Tra 1 neu vao duoc tui, 0 neu khong.
--   [PHANBIEN 29/08] AddItem cua du an co chu ky:
--     AddItem(genre, detail, particular, nLevel, nSeries, nLuck, nItemLevel0,
--             nItemLevel1..5, nStackNum, nEnChance, nPoint)
--   THAM SO 4 LA CAP DO, KHONG PHAI SO LUONG. So luong nam o THAM SO 13 va chi
--   duoc doc khi co DU 15 tham so (ScriptFuns.cpp:4959-4966). Khuon nay chep
--   theo global\vatpham.lua:38.
function G_TraoMotMon(nGenre, nDetail, nParticular, nSoLuong, nCapDo, szTen,
		szNguon)
	local nSL = nSoLuong
	if (nSL == nil or nSL < 1) then
		nSL = 1
	end
	local nCap = nCapDo
	if (nCap == nil) then
		nCap = 0
	end
	local szT = G_Chuoi(szTen)
	local szN = G_Chuoi(szNguon, "?")

	local nIdx = AddItem(nGenre, nDetail, nParticular, nCap, 0, 0, 0,
		0, 0, 0, 0, 0, nSL, 0, 0)

	-- [PHANBIEN 29/08] AddItem tra 0 khi that bai (khong co nguoi choi / thieu tham so /
	-- AddItemSet2 that bai vi het khe item hoac ma vat pham khong co trong
	-- magicscript). Truoc day bo qua tri tra ve nen log bao "da trao" ca khi
	-- nguoi choi khong nhan duoc gi.
	if (nIdx == nil or nIdx <= 0) then
		G_LogThuong("TRAO THAT BAI: "..szN.." | "..szT.." ("..nGenre..","
			..nDetail..","..nParticular..") x"..nSL)
		return 0
	end
	G_LogThuong(szN.." | "..szT.." ("..nGenre..","..nDetail..","
		..nParticular..") x"..nSL)
	return 1
end

-- Trao mot bang thuong cho nguoi choi HIEN TAI.
--   szKhoaBang : ten khoa trong tbCFG_THUONG (tra qua G_CFG)
--   szNguon    : ten hoat dong, de ghi log cho de tra
-- Tra ve so MUC da trao duoc (0 neu khong trao duoc gi).
function G_TraoThuong(szKhoaBang, szNguon)
	local szKhoa = G_Chuoi(szKhoaBang, "(khong ten)")
	local szN = G_Chuoi(szNguon, "?")
	local tbBang = G_CFG(szKhoa, nil)

	if (tbBang == nil) then
		G_LogThuong("THIEU BANG THUONG: "..szKhoa.." (nguon "..szN..")")
		return 0
	end
	-- [PHANBIEN 29/08] G_CFG tra cuu MOT khong gian ten phang, nen go nham ten khoa co
	-- the tra ve mot con SO. getn(so) nem loi lam sap ca hoat dong.
	if (type(tbBang) ~= "table") then
		G_LogThuong("KHOA KHONG PHAI BANG THUONG: "..szKhoa.." (nguon "..szN..")")
		return 0
	end

	local nSoMuc = getn(tbBang)
	if (nSoMuc <= 0) then
		G_LogThuong("BANG THUONG RONG: "..szKhoa)
		return 0
	end

	-- [PHANBIEN 29/08] Kiem so o trong TRUOC khi trao. Luu y that: goi khong tham so
	-- thi CalcFreeItemCellCount chi DEM SO O 1x1 CON TRONG, RAI RAC
	-- (ScriptFuns.cpp:5981 -> KInventory::FindFreeCell(1,1)). No KHONG bao dam
	-- mon to (1x3, 2x3) nhet vua. Vi vay day chi la RAO SO BO de tranh truong
	-- hop tui day han - khong hua chac chan.
	local nOTrong = CalcFreeItemCellCount()
	if (nOTrong ~= nil and nOTrong < nSoMuc) then
		G_LogThuong("TUI DAY: "..szKhoa.." can "..nSoMuc.." o, con "..nOTrong
			.." (nguon "..szN..")")
		Msg2Player("Hµnh trang cÇn trèng "..nSoMuc
			.." « míi nhËn ®­îc phÇn"
			.." th­ëng.")
		return 0
	end

	local nDaTrao = 0
	for i = 1, nSoMuc do
		local tbMuc = tbBang[i]
		if (tbMuc ~= nil and type(tbMuc) == "table") then
			local tbMa = tbMuc[2]
			if (tbMa ~= nil and type(tbMa) == "table" and tbMa[1] ~= nil
				and tbMa[2] ~= nil and tbMa[3] ~= nil) then
				nDaTrao = nDaTrao + G_TraoMotMon(tbMa[1], tbMa[2], tbMa[3],
					tbMuc[1], tbMuc[4], tbMuc[3], szN.." | "..szKhoa)
			else
				G_LogThuong("MUC SAI DINH DANG: "..szKhoa.." muc "..i)
			end
		else
			G_LogThuong("MUC SAI DINH DANG: "..szKhoa.." muc "..i)
		end
	end

	return nDaTrao
end

-- Trao mot mon le, van co rao so bo + ghi log. Dung cho cho chi trao 1 mon.
function G_TraoMon(nGenre, nDetail, nParticular, nSoLuong, szTen, szNguon)
	local szN = G_Chuoi(szNguon, "?")
	local nOTrong = CalcFreeItemCellCount()
	if (nOTrong ~= nil and nOTrong < 1) then
		G_LogThuong("TUI DAY khi trao "..G_Chuoi(szTen).." (nguon "..szN..")")
		Msg2Player("Hµnh trang ®· ®Çy, kh«ng"
			.." nhËn ®­îc phÇn th­ëng.")
		return 0
	end
	return G_TraoMotMon(nGenre, nDetail, nParticular, nSoLuong, 0, szTen, szN)
end
