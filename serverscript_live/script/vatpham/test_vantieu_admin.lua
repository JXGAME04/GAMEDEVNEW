-- ============================================================================
-- TEST_VANTIEU_ADMIN.LUA  [VTCN 06/09]  Bo test VAN TIEU Long Mon Tieu Cuc tren Lenh Bai
-- Admin (Include tu lenhbaiadmin.lua, main() dofile lai => sua KHONG can restart).
-- Muc: 1 xe tieu engine (thu_xetieu.lua) - 2 nhanh ca nhan - 3 nhanh bang - 4 co test
--      5 dich chuyen NPC - 6 trang thai - 7 nhat ky - 8 sinh lai NPC.
-- CAM dau "/" trong nhan menu (ScriptFuns.cpp:717 cat o dau / dau tien).
-- Co test = 3 bit trong bien nhiem vu 4169 CUA NHAN VAT admin (VT_TestBoQua o lib_lmbiaoche):
--   bit 1 bo qua gio va thu (ca hai nhanh), bit 2 bo qua vao bang 7 ngay,
--   bit 3 coi bang minh la bang chiem thanh dang dung. Chi tac dung voi nguoi bat co.
-- ============================================================================
Include("\\script\\lib\\lib_lmbiaoche.lua")
Include("\\script\\activitysys\\config\\129\\npc_consigner.lua")	-- pActivity + extend + variables + tbTongTask
Include("\\script\\global\\maplist.lua")
Include("\\script\\startgame\\lmbj_addnpc.lua")						-- lmbj_addnpc() sinh 45 NPC

VT_ADM_TITLE = "<color=yellow>VËn tiªu Long M«n Tiªu Côc<color> - bé test"

function VT_ADM_Co(nBit)
	if VT_TestBoQua(nBit) == 1 then
		return "bËt"
	end
	return "t¾t"
end

function VT_ADM_TenMap(nMap)
	local sz = nil
	if tbGlobalMapId2Name then
		sz = tbGlobalMapId2Name[nMap]
	end
	if sz == nil and GetMapName then
		sz = GetMapName(nMap)
	end
	if sz == nil then
		sz = format("map %d", nMap)
	end
	return sz
end

function VT_TestRoot()
	SayEx({format("%s. Cê test: giê-thø=%s, vµo bang 7 ngµy=%s, chiÕm thµnh=%s. NhËt ký m¸y chñ: Logs\\KSG_YunBiaoLog_<ngµy>.txt", VT_ADM_TITLE, VT_ADM_Co(1), VT_ADM_Co(2), VT_ADM_Co(3)),
	"1. Xe tiªu (engine): t¹o, vÞ trÝ, cßn sèng, xo¸, bËt-t¾t hÖ/VT_ADM_Xe",
	"2. Nh¸nh c¸ nh©n 1-9 sao: dÞch chuyÓn, vËt phÈm, reset, huû, tr¹ng th¸i/VT_ADM_CaNhan",
	"3. Nh¸nh bang héi 10 sao: biÕn bang 1149, reset tuÇn, huû, r­¬ng, tr¹ng th¸i/VT_ADM_Bang",
	"4. Cê test: bá qua giê-thø, vµo bang 7 ngµy, chiÕm thµnh/VT_ADM_CoMenu",
	"5. DÞch chuyÓn tíi NPC: Ch­ëng quü, 14 Tiªu S­, 7 TiÕp DÉn, 23 NhËn Hµng/VT_ADM_DiChuyen",
	"6. Xem tr¹ng th¸i ®Çy ®ñ (c¸ nh©n + bang + engine)/VT_ADM_TrangThai",
	"7. Xem 12 dßng cuèi nhËt ký vËn tiªu h«m nay/VT_ADM_Log",
	"8. Sinh l¹i 45 NPC vËn tiªu (chØ khi boot lçi, gäi lÆp sÏ nh©n b¶n NPC)/VT_ADM_SinhNpc",
	"Quay l¹i bé test ho¹t ®éng/ADM_TestHoatDong",
	"KÕt thóc ®èi tho¹i/no"})
end

-- ---------------------------------------------------------------- 1. xe tieu engine
function VT_ADM_Xe()
	SayEx({"<color=yellow>Xe tiªu (engine)<color>: bé thö AI b¸m chñ cña script\\event\\lmbiaoche\\thu_xetieu.lua. Xe 10 sao mÉu 2233, sèng 30 phót, b¸m khi c¸ch h¬n 6-7 «, qu¸ xa 5 phót th× biÕn mÊt (xem BANGIAO_VANTIEU_TEST_0609.md môc 2)",
	"T¹o Tiªu Xa c¹nh m×nh (XT_Tao)/VT_ADM_XeTao",
	"VÞ trÝ Tiªu Xa (XT_ViTri)/VT_ADM_XeViTri",
	"Tiªu Xa cßn sèng kh«ng (XT_Song)/VT_ADM_XeSong",
	"Xo¸ Tiªu Xa (XT_Xoa)/VT_ADM_XeXoa",
	"T¾t hÖ xe tiªu ngay, kh«ng cÇn lïi nhÞ ph©n (BC_SetEnable 0)/VT_ADM_XeTat",
	"BËt l¹i hÖ xe tiªu (BC_SetEnable 1)/VT_ADM_XeBat",
	"Quay l¹i/VT_TestRoot"})
end
function VT_ADM_XeGoi(szHam)
	DynamicExecuteByPlayer(PlayerIndex, "\\script\\event\\lmbiaoche\\thu_xetieu.lua", szHam)
	VT_ADM_Xe()
end
function VT_ADM_XeTao() VT_ADM_XeGoi("XT_Tao") end
function VT_ADM_XeViTri() VT_ADM_XeGoi("XT_ViTri") end
function VT_ADM_XeSong() VT_ADM_XeGoi("XT_Song") end
function VT_ADM_XeXoa() VT_ADM_XeGoi("XT_Xoa") end
function VT_ADM_XeTat() VT_ADM_XeGoi("XT_Tat") end
function VT_ADM_XeBat() VT_ADM_XeGoi("XT_Bat") end

-- ---------------------------------------------------------------- 2. nhanh ca nhan
function VT_ADM_CaNhan()
	SayEx({"<color=yellow>Nh¸nh c¸ nh©n 1-9 sao<color>: cÊp 90 trë lªn, cã m«n ph¸i, 10:00-23:00, 3 lÇn mét ngµy, 5 lÇn lµm míi miÔn phÝ. NhËn ë ¤ng chñ Tiªu côc (Long M«n trÊn), xuÊt ph¸t ë Tiªu S­ ®iÓm ®Çu, tr¶ ë Tiªu S­ ®iÓm cuèi trong 30 phót.",
	"DÞch chuyÓn tíi ¤ng chñ Tiªu côc Lôc Tam C©n (Long M«n trÊn)/VT_ADM_ToiChuongQuy",
	"NhËn vËt phÈm: 5 Ho¸n Tiªu ChØ, 2 ¸p tiªu ñy nhiÖm tr¹ng cao cÊp, 20 Hé Tiªu LÖnh/VT_ADM_ItemCaNhan",
	"NhËn 3 ®¹o cô Tiªu côc: Kho¸i M· Gia Tiªn, Kiªn BÊt Kh¶ Táa, Tiªu Xa Di VÞ/VT_ADM_ItemDaoCu",
	"NhËn 1 Tiªu Kú (thö tr¶ nhiÖm vô khi mÊt xe)/VT_ADM_ItemTieuKy",
	"Reset bé ®Õm ngµy: sè lÇn nhËn, lµm míi miÔn phÝ, nhÆt tiªu vËt/VT_ADM_ResetNgayCaNhan",
	"Huû nhiÖm vô c¸ nh©n ®ang nhËn (xo¸ 4179-4184 vµ xe, kh«ng ph¹t)/VT_ADM_HuyCaNhan",
	"Xem tr¹ng th¸i c¸ nh©n/VT_ADM_TrangThaiCaNhan",
	"Quay l¹i/VT_TestRoot"})
end
function VT_ADM_ToiChuongQuy()
	NewWorld(121, 1989 + 2, 4476 + 2)
	Msg2Player("§· tíi Long M«n trÊn, c¹nh ¤ng chñ Tiªu côc Lôc Tam C©n.")
end
function VT_ADM_Cho(szTen, nP, nSo)
	local tbItem = {szName = szTen, tbProp = {6, 1, nP, 1, 0, 0}, nBindState = -2}
	PlayerFunLib:GetItem(tbItem, nSo, "[VT test] admin nhËn vËt phÈm thö")
	Msg2Player(format("§· nhËn %d %s (6,1,%d).", nSo, szTen, nP))
end
function VT_ADM_ItemCaNhan()
	if PlayerFunLib:CheckFreeBagCell(3, "CÇn 3 « trèng.") ~= 1 then
		return
	end
	VT_ADM_Cho("Ho¸n Tiªu ChØ", 4772, 5)
	VT_ADM_Cho("¸p tiªu ñy nhiÖm tr¹ng cao cÊp", 4773, 2)
	VT_ADM_Cho("Hé Tiªu LÖnh", 4774, 20)
	VT_ADM_CaNhan()
end
function VT_ADM_ItemDaoCu()
	if PlayerFunLib:CheckFreeBagCell(3, "CÇn 3 « trèng.") ~= 1 then
		return
	end
	VT_ADM_Cho("Kho¸i M· Gia Tiªn", 4775, 1)
	VT_ADM_Cho("Kiªn BÊt Kh¶ Táa", 4776, 1)
	VT_ADM_Cho("Tiªu Xa Di VÞ", 4778, 1)
	VT_ADM_CaNhan()
end
function VT_ADM_ItemTieuKy()
	if PlayerFunLib:CheckFreeBagCell(1, "CÇn 1 « trèng.") ~= 1 then
		return
	end
	VT_ADM_Cho("Tiªu Kú", 4771, 1)
	VT_ADM_CaNhan()
end
function VT_ADM_ResetNgayCaNhan()
	SetTask(TSK_DailyTaskNum, 0)
	SetTask(TSK_DailyFreeCount, 0)
	SetTask(TSK_DailyPickNum, 0)
	SetTask(4163, 0)
	SetTask(4164, 0)
	SetTask(4165, 0)
	SetTask(4166, 0)
	Msg2Player("§· reset bé ®Õm ngµy c¸ nh©n (4178, 4183, 4185 vµ 4163-4166 nh¸nh cò).")
	VT_ADM_CaNhan()
end
function VT_ADM_HuyCaNhan()
	SetTask(TSK_LMBJTaskFlag, 0)
	SetTask(TSK_LMBJTaskTime, 0)
	SetTask(TSK_LMBJLowTskFlag, 0)
	SetTask(TSK_LMBJRanTskFlag, 0)
	SetTask(TSK_LMBJCarIndex, 0)
	SetTask(4160, 0)
	SetTask(4161, 0)
	SetTask(4162, 0)
	local nXoa = DeleteBiaoChe()
	Msg2Player(format("§· huû nhiÖm vô c¸ nh©n (4179-4184 = 0, 4160-4162 nh¸nh cò = 0); xo¸ xe: %d.", nXoa))
	VT_ADM_CaNhan()
end
function VT_ADM_TrangThaiCaNhan()
	local nFlag = GetTask(TSK_LMBJTaskFlag)
	if nFlag == 0 then
		Msg2Player("C¸ nh©n: ch­a nhËn nhiÖm vô (4179 = 0).")
	else
		local nId = floor(nFlag / 10)
		local bFlag = mod(nFlag, 10)
		local tb = pActivity.tbBJPathLevel[nId]
		if tb then
			local nS = tb[1][1 + bFlag]
			local nE = tb[1][2 - bFlag]
			Msg2Player(format("NhiÖm vô tuyÕn %d: %d sao, %s tíi %s (4179 = %d).", nId, tb[2], pActivity.tbBJPoints[nS][1], pActivity.tbBJPoints[nE][1], nFlag))
		else
			Msg2Player(format("4179 = %d kh«ng tra ®­îc tuyÕn.", nFlag))
		end
	end
	local nT = GetTask(TSK_LMBJTaskTime)
	if nT ~= 0 then
		Msg2Player(format("Xe ®· xuÊt ph¸t %d gi©y tr­íc (4180), cßn %d gi©y; chØ sè NPC xe 4184 = %d.", GetCurrentTime() - nT, 1800 - (GetCurrentTime() - nT), GetTask(TSK_LMBJCarIndex)))
	else
		Msg2Player("Xe ch­a xuÊt ph¸t (4180 = 0).")
	end
	Msg2Player(format("H«m nay: nhËn %d/3 (4178), lµm míi miÔn phÝ %d/5 (4183), nhÆt tiªu vËt %d/5 (4185).", pActivity:GetTaskDailyA(TSK_DailyTaskNum), pActivity:GetTaskDailyA(TSK_DailyFreeCount), pActivity:GetTaskDailyA(TSK_DailyPickNum)))
	Msg2Player(format("Tói: Tiªu Kú %d, Ho¸n Tiªu ChØ %d, ñy nhiÖm tr¹ng %d, Hé Tiªu LÖnh %d.", CalcItemCount(-1, 6, 1, 4771, -1), CalcItemCount(-1, 6, 1, 4772, -1), CalcItemCount(-1, 6, 1, 4773, -1), CalcItemCount(-1, 6, 1, 4774, -1)))
	VT_ADM_TrangThaiEngine()
	if GetTask(4160) ~= 0 or GetTask(4161) ~= 0 then
		Msg2Player(format("C¶nh b¸o: cßn d÷ liÖu nh¸nh cò 4160 = %d, 4161 = %d, dïng môc Huû nhiÖm vô c¸ nh©n ®Ó xo¸.", GetTask(4160), GetTask(4161)))
	end
	VT_ADM_CaNhan()
end
function VT_ADM_TrangThaiEngine()
	local nSong = IsBiaoCheAlive()
	local a, b, c = GetBiaoChePos()
	if c == nil or c < 0 then
		Msg2Player(format("Engine: IsBiaoCheAlive = %d, GetBiaoChePos = kh«ng cã xe.", nSong))
	else
		Msg2Player(format("Engine: IsBiaoCheAlive = %d, xe ë %s « (%d,%d).", nSong, VT_ADM_TenMap(SubWorldIdx2ID(c)), floor(a / 32), floor(b / 32)))
	end
end

-- ---------------------------------------------------------------- 3. nhanh bang hoi
function VT_ADM_Bang()
	SayEx({"<color=yellow>Nh¸nh bang héi 10 sao<color>: thø b¶y vµ chñ nhËt 12:00-23:00, bang ph¶i ®ang chiÕm thµnh, bang chñ më (sè l­ît = (sè thµnh chiÕm tuÇn + 1) chia 2), tr­ëng l·o hoÆc bang chñ nhËn, cÊp 90, vµo bang 7 ngµy. BiÕn bang 1149 hiÖn ch­a ®­îc c«ng thµnh chiÕn ghi nªn ph¶i ®Æt tay ®Ó test.",
	"§Æt sè thµnh chiÕm tuÇn nµy cña bang m×nh = 1 (biÕn bang 1149, ®­îc 1 l­ît)/VT_ADM_Bang1149_1",
	"§Æt sè thµnh chiÕm tuÇn nµy = 3 (®­îc 2 l­ît)/VT_ADM_Bang1149_3",
	"Reset tr¹ng th¸i tuÇn cña bang: 1150 më, 1151 l­ît cßn, 1152 mèc nhËn, 1154 tuyÕn/VT_ADM_BangReset",
	"Huû nhiÖm vô bang ®ang nhËn cña m×nh (4179 = 0, 1152 = 0, xo¸ xe)/VT_ADM_HuyBang",
	"NhËn 8 R­¬ng Tiªu VËt Bang (thö nép r­¬ng cho bang chñ vµ nhËn th­ëng nhãm)/VT_ADM_ItemRuongBang",
	"NhËn 1 Th­ëng TËn Trung vµ 1 Th­ëng Trung Thµnh (gãi th­ëng b¸m xe)/VT_ADM_ItemThuongBamXe",
	"Xem tr¹ng th¸i bang/VT_ADM_TrangThaiBang",
	"Quay l¹i/VT_TestRoot"})
end
function VT_ADM_BangDat1149(n)
	local szTong, nTongId = GetTongName()
	if nTongId == 0 then
		Msg2Player("Ch­a vµo bang.")
		VT_ADM_Bang()
		return
	end
	pActivity:SetTongWeekTaskValue(nTongId, TONG_TASK_OCCUPY_CITYS, n)
	Msg2Player(format("Bang %s: 1149 (thµnh chiÕm tuÇn) = %d, sè l­ît më ®­îc = %d.", szTong, n, floor((n + 1) / 2)))
	VT_ADM_Bang()
end
function VT_ADM_Bang1149_1() VT_ADM_BangDat1149(1) end
function VT_ADM_Bang1149_3() VT_ADM_BangDat1149(3) end
function VT_ADM_BangReset()
	local szTong, nTongId = GetTongName()
	if nTongId == 0 then
		Msg2Player("Ch­a vµo bang.")
		VT_ADM_Bang()
		return
	end
	pActivity:SetTongWeekTaskValue(nTongId, TONG_TASK_YABIAO_STATE, 0)
	pActivity:SetTongWeekTaskValue(nTongId, TONG_TASK_YABIAO_COUNT, 0)
	TONG_ApplySetTaskValue(nTongId, TONG_TASK_YABIAO_TIME, 0)
	TONG_ApplySetTaskValue(nTongId, TONG_TASK_YABIAO_RAND, 0)
	Msg2Player(format("Bang %s: ®· reset 1150, 1151, 1152, 1154 (gi÷ 1149 vµ 1153 r­¬ng).", szTong))
	VT_ADM_Bang()
end
function VT_ADM_HuyBang()
	local szTong, nTongId = GetTongName()
	SetTask(TSK_LMBJTaskFlag, 0)
	SetTask(TSK_LMBJTaskTime, 0)
	SetTask(TSK_LMBJCarIndex, 0)
	if nTongId ~= 0 then
		TONG_ApplySetTaskValue(nTongId, TONG_TASK_YABIAO_TIME, 0)
	end
	local nXoa = DeleteBiaoChe()
	Msg2Player(format("§· huû nhiÖm vô bang cña m×nh (4179, 4180, 4184 = 0; 1152 = 0); xo¸ xe: %d.", nXoa))
	VT_ADM_Bang()
end
function VT_ADM_ItemRuongBang()
	if PlayerFunLib:CheckFreeBagCell(8, "CÇn 8 « trèng.") ~= 1 then
		return
	end
	VT_ADM_Cho("R­¬ng Tiªu VËt Bang", 4980, 8)
	VT_ADM_Bang()
end
function VT_ADM_ItemThuongBamXe()
	if PlayerFunLib:CheckFreeBagCell(2, "CÇn 2 « trèng.") ~= 1 then
		return
	end
	VT_ADM_Cho("Th­ëng TËn Trung", 4978, 1)
	VT_ADM_Cho("Th­ëng Trung Thµnh", 4979, 1)
	VT_ADM_Bang()
end
function VT_ADM_TrangThaiBang()
	local szTong, nTongId = GetTongName()
	if nTongId == 0 then
		Msg2Player("Bang: ch­a vµo bang.")
		VT_ADM_Bang()
		return
	end
	local nOcc = pActivity:GetTongWeekTaskValue(nTongId, TONG_TASK_OCCUPY_CITYS)
	local nMo = pActivity:GetTongWeekTaskValue(nTongId, TONG_TASK_YABIAO_STATE)
	local nLuot = pActivity:GetTongWeekTaskValue(nTongId, TONG_TASK_YABIAO_COUNT)
	local nMoc = TONG_GetTaskValue(nTongId, TONG_TASK_YABIAO_TIME)
	local nRuong = TONG_GetTaskValue(nTongId, TONG_TASK_YABIAO_AWARD)
	local nTuyen = TONG_GetTaskValue(nTongId, TONG_TASK_YABIAO_RAND)
	Msg2Player(format("Bang %s (id %d): thµnh chiÕm tuÇn 1149 = %d (®­îc %d l­ît), ®· më 1150 = %d, l­ît cßn 1151 = %d, r­¬ng 1153 = %d, tuyÕn 1154 = %d.", szTong, nTongId, nOcc, floor((nOcc + 1) / 2), nMo, nLuot, nRuong, nTuyen))
	if nMoc ~= 0 then
		Msg2Player(format("LÇn nhËn gÇn nhÊt 1152: %d gi©y tr­íc (kho¸ 1800 gi©y).", GetCurrentTime() - nMoc))
	else
		Msg2Player("Ch­a ai trong bang nhËn xe (1152 = 0).")
	end
	local nOccT, szOccT = pActivity:GetOccupyTongId()
	if nOccT == nil then
		nOccT = 0
	end
	if szOccT == nil then
		szOccT = "(kh«ng ph¶i thµnh hoÆc ch­a ai chiÕm)"
	end
	local szDung = "kh«ng"
	if nOccT == nTongId then
		szDung = "®óng"
	end
	Msg2Player(format("Thµnh ®ang ®øng do bang %s (id %d) chiÕm, lµ bang m×nh: %s. Chøc vÞ: bang chñ %d, tr­ëng l·o %d, vµo bang %d phót (cÇn 10080).", szOccT, nOccT, szDung, CheckIsMaster(), CheckIsElder(), GetJoinTongTime()))
	local nFlag = GetTask(TSK_LMBJTaskFlag)
	if nFlag ~= 0 then
		local nId = floor(nFlag / 10)
		local tb = pActivity.tbBJPathLevel[nId]
		if tb and tb[2] >= 10 then
			Msg2Player(format("M×nh ®ang nhËn xe bang tuyÕn %d: %s tíi %s, xuÊt ph¸t %d gi©y tr­íc.", nId, pActivity.tbBJPoints[tb[1][1]][1], pActivity.tbBJPoints[tb[1][2]][1], GetCurrentTime() - GetTask(TSK_LMBJTaskTime)))
		end
	end
	Msg2Player(format("B¸m xe tuÇn (3542-3545): %d, %d, %d, %d phót.", pActivity:GetFollowAwardCount(TSK_WeekFollowCount), pActivity:GetFollowAwardCount(TSK_WeekFollowCount + 1), pActivity:GetFollowAwardCount(TSK_WeekFollowCount + 2), pActivity:GetFollowAwardCount(TSK_WeekFollowCount + 3)))
	VT_ADM_TrangThaiEngine()
	VT_ADM_Bang()
end

-- ---------------------------------------------------------------- 4. co test
function VT_ADM_CoMenu()
	SayEx({format("<color=yellow>Cê test<color> (bit trong biÕn 4169 cña nh©n vËt nµy, chØ t¸c dông víi m×nh): giê-thø = %s, vµo bang 7 ngµy = %s, chiÕm thµnh = %s", VT_ADM_Co(1), VT_ADM_Co(2), VT_ADM_Co(3)),
	"§¶o cê: bá qua giê vµ thø (c¸ nh©n 10-23h, bang T7-CN 12-23h)/VT_ADM_CoGio",
	"§¶o cê: bá qua ®iÒu kiÖn vµo bang ®ñ 7 ngµy/VT_ADM_Co7Ngay",
	"§¶o cê: coi bang m×nh lµ bang chiÕm thµnh ®ang ®øng/VT_ADM_CoChiemThanh",
	"T¾t hÕt cê test/VT_ADM_CoTat",
	"Quay l¹i/VT_TestRoot"})
end
function VT_ADM_CoDao(nBit)
	if VT_TestBoQua(nBit) == 1 then
		VT_TestDat(nBit, 0)
	else
		VT_TestDat(nBit, 1)
	end
	VT_ADM_CoMenu()
end
function VT_ADM_CoGio() VT_ADM_CoDao(1) end
function VT_ADM_Co7Ngay() VT_ADM_CoDao(2) end
function VT_ADM_CoChiemThanh() VT_ADM_CoDao(3) end
function VT_ADM_CoTat()
	SetTask(VT_TEST_TASK, 0)
	VT_ADM_CoMenu()
end

-- ---------------------------------------------------------------- 5. dich chuyen
function VT_ADM_DiChuyen()
	SayEx({"<color=yellow>DÞch chuyÓn tíi NPC vËn tiªu<color> (r¬i c¹nh NPC 2 «)",
	"¤ng chñ Tiªu côc Lôc Tam C©n - Long M«n trÊn/VT_ADM_ToiChuongQuy",
	"14 Long M«n Tiªu S­ (nh¸nh c¸ nh©n)/VT_ADM_TeleTieuSu",
	"7 TiÕp DÉn ¸p Tiªu Bang (nhËn xe bang ë thµnh)/VT_ADM_TeleTiepDan",
	"NhËn Hµng ¸p Tiªu Bang 1-12 (®iÓm cuèi xe bang)/VT_ADM_TeleNhanHang1",
	"NhËn Hµng ¸p Tiªu Bang 13-23/VT_ADM_TeleNhanHang2",
	"Quay l¹i/VT_TestRoot"})
end
-- tbBJPoints (extend.lua): 1-14 Tieu Su; 15,19,23,28,33,37,41 = Tiep Dan; con lai = Nhan Hang
VT_ADM_TIEPDAN = {15, 19, 23, 28, 33, 37, 41}
VT_ADM_NHANHANG = {16, 17, 18, 20, 21, 22, 24, 25, 26, 27, 29, 30, 31, 32, 34, 35, 36, 38, 39, 40, 42, 43, 44}
function VT_ADM_TeleDiem(i)
	local tb = pActivity.tbBJPoints[i]
	if not tb then
		return
	end
	NewWorld(tb[2][1], tb[2][2] + 2, tb[2][3] + 2)
	Msg2Player(format("§· tíi ®iÓm %d: %s (%s).", i, tb[1], VT_ADM_TenMap(tb[2][1])))
end
function VT_ADM_TeleMenu(szTitle, tbDs, nTu, nDen, szBack)
	local tbOpt = {szTitle}
	for k = nTu, nDen do
		local i = tbDs[k]
		if i then
			local tb = pActivity.tbBJPoints[i]
			tinsert(tbOpt, format("%s - %s/#VT_ADM_TeleDiem(%d)", tb[1], VT_ADM_TenMap(tb[2][1]), i))
		end
	end
	tinsert(tbOpt, "Quay l¹i/" .. szBack)
	SayEx(tbOpt)
end
function VT_ADM_TeleTieuSu()
	local tb = {}
	for i = 1, 14 do
		tb[i] = i
	end
	VT_ADM_TeleMenu("<color=yellow>14 Long M«n Tiªu S­<color>", tb, 1, 14, "VT_ADM_DiChuyen")
end
function VT_ADM_TeleTiepDan()
	VT_ADM_TeleMenu("<color=yellow>7 TiÕp DÉn ¸p Tiªu Bang<color>", VT_ADM_TIEPDAN, 1, 7, "VT_ADM_DiChuyen")
end
function VT_ADM_TeleNhanHang1()
	VT_ADM_TeleMenu("<color=yellow>NhËn Hµng ¸p Tiªu Bang 1-12<color>", VT_ADM_NHANHANG, 1, 12, "VT_ADM_DiChuyen")
end
function VT_ADM_TeleNhanHang2()
	VT_ADM_TeleMenu("<color=yellow>NhËn Hµng ¸p Tiªu Bang 13-23<color>", VT_ADM_NHANHANG, 13, 23, "VT_ADM_DiChuyen")
end

-- ---------------------------------------------------------------- 6. trang thai day du
function VT_ADM_TrangThai()
	VT_ADM_TrangThaiCaNhan()
	VT_ADM_TrangThaiBang()
	VT_TestRoot()
end

-- ---------------------------------------------------------------- 7. nhat ky
function VT_ADM_Log()
	local szDay = GetLocalDate("%Y%m%d")
	local szPath = "Logs/KSG_YunBiaoLog_" .. szDay .. ".txt"
	local tbDong = {}
	local bOk = pcall(function()
		local f = io.open(szPath, "rb")
		if not f then
			return
		end
		for szLine in f:lines() do
			tinsert(tbDong, szLine)
		end
		f:close()
	end)
	local n = getn(tbDong)
	if not bOk or n == 0 then
		Msg2Player("Kh«ng ®äc ®­îc hoÆc nhËt ký trèng: " .. szPath)
		VT_TestRoot()
		return
	end
	local nTu = n - 11
	if nTu < 1 then
		nTu = 1
	end
	Msg2Player(format("%s: %d dßng, hiÖn %d dßng cuèi", szPath, n, n - nTu + 1))
	for i = nTu, n do
		Msg2Player(tbDong[i])
	end
	VT_TestRoot()
end

-- ---------------------------------------------------------------- 8. sinh lai NPC
function VT_ADM_SinhNpc()
	lmbj_addnpc()
	Msg2Player("§· gäi lmbj_addnpc(): 7 TiÕp DÉn, 23 NhËn Hµng, 14 Tiªu S­, 1 ¤ng chñ Tiªu côc. Gäi lÆp sÏ nh©n b¶n NPC, chØ dïng khi boot lçi.")
	VT_TestRoot()
end
