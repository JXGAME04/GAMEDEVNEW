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
		Say("B¶n CoreServer ®ang ch¹y ch­a cã hÖ ®Êu gi¸, ch­a ®óc mÉu ®­îc.", 0)
		return
	end
	GiveItemUI("§óc mÉu th­ëng ChiÕn LÖnh",
		"Bá vµo ®©y mãn ®å ®· chØnh ®óng ý (dung luyÖn, kh¶m n¹m, kho¸, h¹n dïng).<enter>Mãn sÏ bÞ lÊy khái hµnh trang vµ l­u lµm mÉu ®Ó ph¸t th­ëng.",
		"CL_NhanMau", "no")
end

function CL_NhanMau()
	local nIdx = GetGiveItemUnit(1)
	if (nIdx == nil or nIdx <= 0) then
		Say("Ch­a bá mãn nµo vµo hép.", 0)
		return
	end
	local szRec, szName, szDesc, nCells, nStack = AUC_ItemToRec(nIdx)
	if (szRec == nil or szRec == "") then
		Say("Mãn nµy kh«ng dùng l¹i ®­îc (quÆng, nguyªn liÖu, trang bÞ háng) nªn kh«ng ®óc mÉu ®­îc.", 0)
		return
	end
	-- Goi THANG AUC_PutOn (khong qua duong ky gui) de luu duoc CA MON KHOA.
	-- end_time = 0 -> AUC_Sweep khong dung toi.
	local nId = AUC_PutOn(CL_MAU_ATYPE, "chienlenh", 2, GetName(), 0,
		szName, szDesc, szRec, nCells, 1, 0, 0, 0, 0, 0, 0, 0, 0)
	if (nId == nil or nId <= 0) then
		Say("L­u mÉu thÊt b¹i, xem l¹i nhËt ký m¸y chñ.", 0)
		return
	end
	AUC_SetState(nId, CL_MAU_STATE, 1)
	RemoveItemByIndex(nIdx, nStack or 1)
	GhiLog("HE", format("[CL] duc mau vat pham id=%d ten=%s cells=%d boi %s",
		nId, szName or "?", nCells or 1, GetName()))
	Say(format("§· ®óc mÉu, m· sè <color=yellow>%d<color>.<enter>"
		.. "Vµo trang admin, « chän VËt phÈm mÉu sÏ thÊy nã. Chuçi th­ëng lµ "
		.. "<color=green>aucitem:%d<color>", nId, nId), 0)
end

-- Xem nhanh trang thai Chien Lenh cua chinh minh (dung khi thu nghiem).
function CL_XemTrangThai()
	if (CL_Info == nil) then
		Say("B¶n CoreServer ®ang ch¹y ch­a cã ChiÕn LÖnh.", 0)
		return
	end
	local tb = CL_Info()
	local szNd = format("Mïa %d | s½n sµng %d | ®ang më %d | cfg_ver %d<enter>"
		.. "Sè mèc %d | ®iÓm mçi cÊp %d",
		tb.season_id, tb.ready, tb.dangmo, tb.cfg_ver, tb.so_moc, tb.level_score)
	local p = CL_TrangThai()
	if (p ~= nil) then
		szNd = szNd .. format("<enter>§iÓm cña ta %d | cÊp %d | Hµo Hoa %d"
			.. " | ®· nhËn Th­êng %d | ®· nhËn Hµo Hoa %d",
			p.score, p.cap, p.vip, p.got_low, p.got_vip)
	end
	Say(szNd, 0)
end

-- ============ MENU CON cua lenh bai admin ============
-- Nhan SayEx co tran 512 byte va CAM dau '/' trong nhan (dau '/' la cho tach
-- ten ham), nen menu chinh chi them DUNG MOT muc tro toi day.
function CL_MenuAdmin()
	SayEx({
		"--- ChiÕn LÖnh (qu¶n trÞ) ---",
		"§óc mÉu vËt phÈm th­ëng/CL_DucMau",
		"Xem tr¹ng th¸i ChiÕn LÖnh/CL_XemTrangThai",
		"N¹p l¹i cÊu h×nh tõ MySQL ngay/CL_NapLai",
		"BËt Hµo Hoa cho chÝnh m×nh (thö nghiÖm)/CL_BatVipThu",
		"§ãng/",
	}, 0)
end

-- Ep nap lai cau hinh ma khong cho vong 30 giay.
function CL_NapLai()
	if (CL_Reload == nil) then
		Say("B¶n CoreServer ®ang ch¹y ch­a cã ChiÕn LÖnh.", 0)
		return
	end
	local nLoi = CL_Reload(1)
	if (nLoi == 0) then
		Say("§· n¹p l¹i cÊu h×nh. Xem b¶ng st_cfg_log ®Ó biÕt chi tiÕt.", 0)
	else
		Say(format("CÊu h×nh cã <color=red>%d lçi<color> nªn gi÷ nguyªn b¶n cò.<enter>"
			.. "Më tab NhËt ký cÊu h×nh trªn trang admin ®Ó xem tõng lçi.", nLoi), 0)
	end
end

-- CHI DE THU NGHIEM. Ban chinh thuc: the Hao Hoa mua o Ky Tran Cac (xem ghi chu
-- cuoi tep). Ham nay goi dung CL_MuaVip nen di qua het cac phep kiem that.
function CL_BatVipThu()
	if (CL_MuaVip == nil) then
		Say("B¶n CoreServer ®ang ch¹y ch­a cã ChiÕn LÖnh.", 0)
		return
	end
	local nKq = CL_MuaVip()
	if (nKq == 1) then
		Say("§· bËt ChiÕn LÖnh Hµo Hoa vµ céng ®iÓm th­ëng kÝch ho¹t.", 0)
	elseif (nKq == 2) then
		Say("Nh©n vËt nµy ®· cã Hµo Hoa råi - kh«ng tiªu thÎ, kh«ng céng thªm.", 0)
	else
		Say("Kh«ng bËt ®­îc: mïa ch­a më hoÆc ch­a n¹p ®­îc tr¹ng th¸i.", 0)
	end
end

-- ============ THE HAO HOA BAN CHINH THUC ============
-- CL_MuaVip() da san sang, nhung CHUA CO VAT PHAM nao goi no.
-- Muon ban o Ky Tran Cac thi can hai thu, deu la SUA BANG DU LIEU nen phai hoi chu:
--   1. mot dong moi trong settings\item\magicscript.txt (vat pham "Chien Lenh Hao Hoa")
--      voi cot Script tro toi mot tep .lua goi CL_MuaVip;
--   2. mot dong trong bang hang cua Ky Tran Cac, gia 500 xu.
-- Tep .lua do phai theo dung luat: goi CL_MuaVip() TRUOC, chi khi tra ve 1 moi
-- xoa the. Tra ve 2 (da co Hao Hoa) thi KHONG DUOC tieu the, neu khong nguoi choi
-- mat 500 xu ma khong duoc gi.
