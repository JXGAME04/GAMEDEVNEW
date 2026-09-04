-- cl_gm.lua - [CL 04/09] Lenh GM: DUC MAU VAT PHAM cho phan thuong Chien Lenh.
--
-- Vi sao can: chuoi "item:g,d,p,..." chi dat duoc 11 truong, va thuoc tinh phu cua
-- trang bi duoc SINH NGAU NHIEN moi lan phat. Muon phat DUNG mot mon da dung luyen /
-- kham nam / khoa / dat han thi phai luu BAN GHI NGUYEN VEN roi phat bang "aucitem:<id>".
-- Mot dong mau phat duoc cho BAO NHIEU NGUOI CUNG DUOC: duong trao chi DOC cot
-- item_rec roi dung lai, KHONG tieu thu dong.
--
-- Quy uoc dau MAU: atype = 9 va state = 9.
--   state = 9 la dieu kien QUYET DINH - da doc ca 9 cau lenh dung auction_item trong
--   KAuctionServer.cpp: moi duong dau gia chi nhin state 0 hoac 1, nen 9 vo hinh voi
--   tat ca. end_time = 0 la chot thu hai (AUC_Sweep con doi end_time > 0).
--   atype = 9 chi la dau phu cho de nhin.
--
-- KHONG CO LENH XOA MAU. Luat: mau da phat thi khong bao gio xoa, chi ngung dung.
-- Ly do: "aucitem:" la CON TRO, doc lai bang vao luc nguoi choi bam Nhan - co the 30
-- ngay sau. Xoa dong mau la moi la thu dang tro toi no tra ve rong.

if (MODEL_GAMECLIENT == 1) then
	return
end

Include("\\script\\chienlenh\\cl_def.lua")

CL_MAU_ATYPE = 9
CL_MAU_STATE = 9

function CL_DucMau()
	if (AUC_ItemToRec == nil) then
		Say("B∂n CoreServer Æang chπy ch≠a c„ h÷ Æ u gi∏, ch≠a ÆÛc m…u Æ≠Óc.", 0)
		return
	end
	GiveItemUI("ßÛc m…u th≠Îng Chi’n L÷nh",
		"B· vµo Æ©y m„n ÆÂ Æ∑ chÿnh ÆÛng ˝ (dung luy÷n, kh∂m nπm, kho∏, hπn dÔng).<enter>M„n sœ bﬁ l y kh·i hµnh trang vµ l≠u lµm m…u Æ” ph∏t th≠Îng.",
		"CL_NhanMau", "no")
end

function CL_NhanMau()
	local nIdx = GetGiveItemUnit(1)
	if (nIdx == nil or nIdx <= 0) then
		Say("Ch≠a b· m„n nµo vµo hÈp.", 0)
		return
	end
	local szRec, szName, szDesc, nCells, nStack = AUC_ItemToRec(nIdx)
	if (szRec == nil or szRec == "") then
		Say("M„n nµy kh´ng d˘ng lπi Æ≠Óc (qu∆ng, nguy™n li÷u, trang bﬁ h·ng) n™n kh´ng ÆÛc m…u Æ≠Óc.", 0)
		return
	end
	-- Goi THANG AUC_PutOn (khong qua duong ky gui) de luu duoc CA MON KHOA.
	-- end_time = 0 -> AUC_Sweep khong dung toi.
	local nId = AUC_PutOn(CL_MAU_ATYPE, "chienlenh", 2, GetName(), 0,
		szName, szDesc, szRec, nCells, 1, 0, 0, 0, 0, 0, 0, 0, 0)
	if (nId == nil or nId <= 0) then
		Say("L≠u m…u th t bπi, xem lπi nhÀt k˝ m∏y chÒ.", 0)
		return
	end
	AUC_SetState(nId, CL_MAU_STATE, 1)
	RemoveItemByIndex(nIdx, nStack or 1)
	GhiLog("HE", format("[CL] duc mau vat pham id=%d ten=%s cells=%d boi %s",
		nId, szName or "?", nCells or 1, GetName()))
	Say(format("ß∑ ÆÛc m…u, m∑ sË <color=yellow>%d<color>.<enter>"
		.. "Vµo trang admin, ´ ch‰n VÀt ph»m m…u sœ th y n„. ChuÁi th≠Îng lµ "
		.. "<color=green>aucitem:%d<color>", nId, nId), 0)
end

-- Xem nhanh trang thai Chien Lenh cua chinh minh (dung khi thu nghiem).
function CL_XemTrangThai()
	if (CL_Info == nil) then
		Say("B∂n CoreServer Æang chπy ch≠a c„ Chi’n L÷nh.", 0)
		return
	end
	local tb = CL_Info()
	local szNd = format("MÔa %d | sΩn sµng %d | Æang mÎ %d | cfg_ver %d<enter>"
		.. "SË mËc %d | Æi”m mÁi c p %d",
		tb.season_id, tb.ready, tb.dangmo, tb.cfg_ver, tb.so_moc, tb.level_score)
	local p = CL_TrangThai()
	if (p ~= nil) then
		szNd = szNd .. format("<enter>ßi”m cÒa ta %d | c p %d | Hµo Hoa %d"
			.. " | Æ∑ nhÀn Th≠Íng %d | Æ∑ nhÀn Hµo Hoa %d",
			p.score, p.cap, p.vip, p.got_low, p.got_vip)
	end
	Say(szNd, 0)
end
