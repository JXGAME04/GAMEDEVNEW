-- ============================================================================
-- TEST_BANGHOI_ADMIN.LUA  [BHTEST 07/09]  Bo test BANG HOI DAY DU tren Lenh Bai Admin
-- (Include tu lenhbaiadmin.lua; main() cua lenh bai dofile lai => sua KHONG can restart).
-- Muc:  1 ho so + kinh te + bao tri    2 thanh vien, chuc vu, quyen, thoai an, duoi
--       3 quy, kien thiet, chien bi, cong hien du tru, phat cong hien
--       4 muc tieu tuan                5 nhat ky, lich su, cong cao, tuyet ky, ngoai hinh
--       6 lanh dia + tac phuong (goi bo TLD_/TPT_ cua bangthanh_f.lua)
--       7 bang khac: danh sach bang, xin vao bang, don cho, lien minh
--       8 hoat dong bang: cong thanh (TCT_), van tieu bang (VT_ADM_Bang), bang chien (HD_), danh hieu-thue (TDT_)
--       9 tien ich: vao bang test, roi bang, khoi tao, log, nap lai file.
-- LUAT: nhan menu CAM dau "/"; "#Ham(thamso)" toi da 31 ky tu; ham nhan dap an lay tham so SO (atoi).
-- Moi lenh Apply*/TONGM_Apply* CHI GUI LEN RELAY (khong ap lac quan) -> phai bam "Xem" lai sau vai giay.
-- Chon thanh vien: BH_SEL[ten admin] = NameID thanh vien dang chon (menu 2 -> "Chon thanh vien").
-- ============================================================================

BH_ADM_TITLE = "<color=yellow>Bang héi<color> - bé test ®Çy ®ñ"
BH_FIG = {}
BH_FIG[0] = "Bang chñ"
BH_FIG[1] = "Tr­ëng l·o"
BH_FIG[2] = "§éi tr­ëng"
BH_FIG[3] = "§Ö tö"
BH_FIG[4] = "Èn sÜ"
BH_SEL = BH_SEL or {}
BH_RIGHT = {}
BH_RIGHT[1901] = "duyÖt ®¬n xin vµo"
BH_RIGHT[1002] = "bæ nhiÖm chøc vô"
BH_RIGHT[1101] = "qu¶n lý liªn minh"
BH_RIGHT[2004] = "qu¶n lý l·nh ®Þa"
BH_RIGHT[9001] = "qu¶n lý t¸c ph­êng"

-- ---------------------------------------------------------------- tien ich chung
function BH_T()
	local nT = GetTong()
	if nT == nil then
		return 0
	end
	return nT
end

function BH_Fig(n)
	if n == nil then
		return "?"
	end
	return BH_FIG[n] or format("chøc vô %d", n)
end

-- epoch giay -> "YYYY-MM-DD"; 0 = "--"
function BH_NgayEpoch(t)
	if t == nil or t <= 0 then
		return "--"
	end
	local s = FormatTime2String(t)
	if s == nil then
		return tostring(t)
	end
	return string.sub(s, 1, 10)
end

-- so NGAY (KV don vi ngay) -> "YYYY-MM-DD"; 0 = "--"
function BH_NgayDays(d)
	if d == nil or d <= 0 then
		return "--"
	end
	if d > 1000000 then		-- da la epoch
		return BH_NgayEpoch(d)
	end
	return BH_NgayEpoch(d * 86400)
end

function BH_HomNay()
	return floor(GetCurrentTime() / 86400)
end

-- danh sach NameID thanh vien (toi da nMax)
function BH_DanhSach(nT, nMax)
	local tb = {}
	local n = 0
	local id = TONG_GetFirstMember(nT, -1)
	while id ~= nil and id ~= 0 and n < nMax do
		n = n + 1
		tb[n] = id
		id = TONG_GetNextMember(nT, id, -1)
	end
	return tb, n
end

function BH_TVChon()
	local id = BH_SEL[GetName()]
	if id == nil then
		return 0
	end
	return id
end

function BH_TenTV(nT, id)
	if id == nil or id == 0 then
		return "(ch­a chän)"
	end
	local s = TONGM_GetName(nT, id)
	if s == nil or s == "" then
		return format("id %d", id)
	end
	return s
end

function BH_ChuaVaoBang(szVe)
	SayEx({"B¹n <color=red>ch­a vµo bang<color> nªn môc nµy kh«ng dïng ®­îc. Vµo bang b»ng môc 7 (xin vµo bang) hoÆc 9 (bang test).",
	"Quay l¹i/"..szVe})
end

-- ---------------------------------------------------------------- goc
function BH_TestRoot()
	local nT = BH_T()
	local sz
	if nT == 0 then
		sz = format("%s. B¹n <color=red>ch­a vµo bang<color>: chØ dïng ®­îc môc 7 (bang kh¸c, xin vµo bang) vµ 9 (tiÖn Ých).", BH_ADM_TITLE)
	else
		sz = format("%s. Bang <color=green>%s<color> (id %d), bang chñ %s, %d thµnh viªn (%d online), cÊp kiÕn thiÕt %d, quü %d l­îng, b¹n lµ %s.",
			BH_ADM_TITLE, TONG_GetName(nT), nT, TONG_GetMaster(nT), TONG_GetMemberCount(nT, -1), TONG_GetOnlineCount(nT),
			TONG_GetBuildLevel(nT), TONG_GetMoney(nT), BH_Fig(GetTongFigure()))
	end
	SayEx({sz,
	"1. Hå s¬ bang: kinh tÕ, cÊp, ngµy-tuÇn, n©ng-h¹ cÊp, b¶o tr× ngµy vµ tuÇn/BH_HoSo",
	"2. Thµnh viªn: danh s¸ch, chän thµnh viªn, cèng hiÕn, quyÒn h¹n, tho¸i Èn, ®uæi/BH_ThanhVien",
	"3. Quü, kiÕn thiÕt, chiÕn bÞ, dù tr÷, gãp vµ ph¸t cèng hiÕn/BH_KinhTe",
	"4. Môc tiªu tuÇn: xem, ®Æt cÊp, céng ®iÓm, th­ëng, chèt tuÇn/BH_MucTieu",
	"5. NhËt ký, lÞch sö, c«ng c¸o, tuyÖt kü, ngo¹i h×nh, t¹m ng­ng/BH_GhiChep",
	"6. L·nh ®Þa, kiÕn thiÕt, t¸c ph­êng (bé TLD_ vµ TPT_)/BH_LanhDia",
	"7. Bang kh¸c: danh s¸ch bang, xin vµo bang, ®¬n chê, liªn minh/BH_BangKhac",
	"8. Ho¹t ®éng bang: c«ng thµnh, vËn tiªu bang, bang chiÕn, tuyªn chiÕn, danh hiÖu-thuÕ/BH_HoatDong",
	"9. TiÖn Ých: vµo bang test, rêi bang, khëi t¹o, ghi log, n¹p l¹i file test/BH_TienIch",
	"Quay l¹i bé test ho¹t ®éng/ADM_TestHoatDong",
	"KÕt thóc ®èi tho¹i/no"})
end

-- ---------------------------------------------------------------- 1. ho so
function BH_HoSo()
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	Msg2Player(format("<color=yellow>[Hå s¬]<color> %s (id %d) - bang chñ %s - %d thµnh viªn, %d online, %d Èn sÜ, %d ®¬n chê duyÖt.",
		TONG_GetName(nT), nT, TONG_GetMaster(nT), TONG_GetMemberCount(nT, -1), TONG_GetOnlineCount(nT), TONG_GetMemberCount(nT, 4), TONG_GetApplyCount(nT)))
	Msg2Player(format("CÊp kiÕn thiÕt %d (kinh nghiÖm bang %d, cÊp kinh nghiÖm %d, tÝn dông %d, ­u ®·i %d).",
		TONG_GetBuildLevel(nT), TONG_GetExp(nT), TONG_GetExpLevel(nT), TONG_GetCredit(nT), TONG_GetPremium(nT)))
	Msg2Player(format("Quü %d l­îng - kiÕn thiÕt %d - chiÕn bÞ %d - tæng kiÕn thiÕt %d - kiÕn thiÕt tuÇn %d (trÇn %d).",
		TONG_GetMoney(nT), TONG_GetBuildFund(nT), TONG_GetWarBuildFund(nT), TONG_GetTotalBuildFund(nT), TONG_GetWeekBuildFund(nT), TONG_GetWeekBuildUpper(nT)))
	Msg2Player(format("PhÝ duy tr× %d - gãp mçi ng­êi %d - chiÕn bÞ b¶o tr× tuÇn %d - cèng hiÕn dù tr÷ %d - kiÕn thiÕt dù tr÷ %d.",
		TONG_GetMaintainFund(nT), TONG_GetPerStandFund(nT), TONG_GetStandFund(nT), TONG_GetStoredOffer(nT), TONG_GetStoredBuildFund(nT)))
	Msg2Player(format("Ngµy %d - tuÇn %d - t¹m ng­ng t¸c ph­êng %d - tr¹ng th¸i chiÕn %d - ngµy chiÕm thµnh %d - liªn minh id %d.",
		TONG_GetDay(nT), TONG_GetWeek(nT), TONG_GetPauseState(nT), TONG_GetWarState(nT), TONG_GetOccupyCityDay(nT), TONG_GetUnionID(nT)))
	Msg2Player(format("L·nh ®Þa: map %d, mÉu %d, cÊm ®Þa %d - t¸c ph­êng %d khu - c«ng c¸o: %s",
		TONG_GetTongMap(nT), TONG_GetTongMapTemplate(nT), TONG_GetTongMapBan(nT), TWS_GetWorkshopCount(nT), tostring(TONG_GetAnnouncement(nT))))
	SayEx({format("%s. Hå s¬ bang <color=green>%s<color> ®· in ra khung chat. C¸c lÖnh d­íi göi lªn relay, bÊm Xem l¹i sau vµi gi©y.", BH_ADM_TITLE, TONG_GetName(nT)),
	"Xem l¹i hå s¬/BH_HoSo",
	"B¶o tr× ngµy cña bang (TONG_ApplyMaintain: ngµy+1, trõ phÝ duy tr×)/#BH_BaoTri(1)",
	"B¶o tr× tuÇn cña bang (TONG_ApplyWeeklyMaintain: tuÇn+1, chèt môc tiªu tuÇn)/#BH_BaoTri(2)",
	"N©ng cÊp kiÕn thiÕt bang (TONG_ApplyUpgrade)/#BH_CapKT(1)",
	"H¹ cÊp kiÕn thiÕt bang (TONG_ApplyDegrade)/#BH_CapKT(0)",
	"§Æt ngµy +1 (TONG_ApplySetDay)/#BH_NgayTuan(1)",
	"§Æt tuÇn +1 (TONG_ApplySetWeek)/#BH_NgayTuan(2)",
	"B¶o tr× kinh tÕ ngµy toµn server (TONG_DailyMaintainAll) - hái tr­íc/BH_BaoTriAllHoi",
	"Quay l¹i/BH_TestRoot"})
end

function BH_BaoTri(nMode)
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	if nMode == 1 then
		TONG_ApplyMaintain(nT)
		Msg2Player(format("§· göi b¶o tr× ngµy cho bang %s (ngµy hiÖn t¹i %d).", TONG_GetName(nT), TONG_GetDay(nT)))
	else
		TONG_ApplyWeeklyMaintain(nT)
		Msg2Player(format("§· göi b¶o tr× tuÇn cho bang %s (tuÇn hiÖn t¹i %d).", TONG_GetName(nT), TONG_GetWeek(nT)))
	end
	BH_HoSo()
end

function BH_CapKT(nUp)
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	if nUp == 1 then
		TONG_ApplyUpgrade(nT)
		Msg2Player(format("§· göi n©ng cÊp kiÕn thiÕt (®ang cÊp %d).", TONG_GetBuildLevel(nT)))
	else
		TONG_ApplyDegrade(nT)
		Msg2Player(format("§· göi h¹ cÊp kiÕn thiÕt (®ang cÊp %d).", TONG_GetBuildLevel(nT)))
	end
	BH_HoSo()
end

function BH_NgayTuan(nMode)
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	if nMode == 1 then
		local n = TONG_GetDay(nT)
		TONG_ApplySetDay(nT, n + 1)
		Msg2Player(format("Ngµy: %d -> %d (®äc l¹i: %d).", n, n + 1, TONG_GetDay(nT)))
	else
		local n = TONG_GetWeek(nT)
		TONG_ApplySetWeek(nT, n + 1)
		Msg2Player(format("TuÇn: %d -> %d (®äc l¹i: %d).", n, n + 1, TONG_GetWeek(nT)))
	end
	BH_HoSo()
end

function BH_BaoTriAllHoi()
	SayEx({"<color=red>Toµn server<color>: ch¹y b¶o tr× kinh tÕ ngµy thËt cho mäi bang (trõ chiÕn bÞ, t¹m ng­ng t¸c ph­êng, chèt môc tiªu). ChØ lµm trªn m¸y chñ test.",
	"§ång ý, ch¹y TONG_DailyMaintainAll(2)/BH_BaoTriAll",
	"Th«i/BH_HoSo"})
end

function BH_BaoTriAll()
	local n = TONG_DailyMaintainAll(2)
	Msg2Player(format("TONG_DailyMaintainAll(2) tr¶ vÒ %s.", tostring(n)))
	BH_HoSo()
end

-- ---------------------------------------------------------------- 2. thanh vien
function BH_ThanhVien()
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	local tb, n = BH_DanhSach(nT, 25)
	Msg2Player(format("<color=yellow>[Thµnh viªn]<color> bang %s: %d ng­êi, %d online (in tèi ®a 25).", TONG_GetName(nT), TONG_GetMemberCount(nT, -1), TONG_GetOnlineCount(nT)))
	for i = 1, n do
		local id = tb[i]
		local szOn = ""
		if TONGM_GetOnline(nT, id) == 1 then
			szOn = " <color=green>online<color>"
		end
		local szAn = ""
		local nRet = TONGM_GetRetireDate(nT, id)
		if nRet ~= nil and nRet > 0 then
			szAn = " tho¸i Èn " .. BH_NgayDays(nRet)
		end
		Msg2Player(format("%d. %s - %s - cèng hiÕn %d (tuÇn %d) - vµo bang %s%s%s",
			i, TONGM_GetName(nT, id), BH_Fig(TONGM_GetFigure(nT, id)), TONGM_GetOffer(nT, id), TONGM_GetLWeeklyOffer(nT, id),
			BH_NgayEpoch(TONGM_GetJoinTime(nT, id)), szOn, szAn))
	end
	local idSel = BH_TVChon()
	SayEx({format("%s. Danh s¸ch ®· in ra khung chat. Thµnh viªn ®ang chän: <color=green>%s<color>. Chøc vô (th¨ng-gi¸ng) vµ chuyÓn bang chñ chØ cã qua cöa sæ bang héi (relay), Lua kh«ng cã lÖnh.", BH_ADM_TITLE, BH_TenTV(nT, idSel)),
	"Xem l¹i danh s¸ch/BH_ThanhVien",
	"Chän thµnh viªn ®Ó thao t¸c/BH_ChonTV",
	"Chän chÝnh t«i/#BH_ChonTVGo(0)",
	"Xem chi tiÕt thµnh viªn ®ang chän (mäi kho¸ KV)/BH_TVChiTiet",
	"Céng 500 cèng hiÕn cho thµnh viªn ®ang chän (TONGM_ApplyAddOffer)/#BH_TVOffer(500)",
	"Trõ 500 cèng hiÕn cho thµnh viªn ®ang chän/#BH_TVOffer(-500)",
	"QuyÒn h¹n cña thµnh viªn ®ang chän: xem, cÊp, gì/BH_Quyen",
	"Tho¸i Èn thµnh viªn ®ang chän (SetRetireDate = h«m nay)/#BH_ThoaiAn(1)",
	"Bá tho¸i Èn thµnh viªn ®ang chän (SetRetireDate = 0)/#BH_ThoaiAn(0)",
	"§uæi thµnh viªn ®ang chän khái bang (TONG_ApplyKickMember) - hái tr­íc/BH_DuoiHoi",
	"Quay l¹i/BH_TestRoot"})
end

function BH_ChonTV()
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	local tb, n = BH_DanhSach(nT, 8)
	local t = {format("Chän thµnh viªn (8 ng­êi ®Çu; bang cã %d ng­êi). §ang chän: %s.", TONG_GetMemberCount(nT, -1), BH_TenTV(nT, BH_TVChon()))}
	for i = 1, n do
		t[getn(t) + 1] = format("%d. %s - %s/#BH_ChonTVGo(%d)", i, TONGM_GetName(nT, tb[i]), BH_Fig(TONGM_GetFigure(nT, tb[i])), i)
	end
	t[getn(t) + 1] = "Quay l¹i/BH_ThanhVien"
	SayEx(t)
end

function BH_ChonTVGo(nIdx)
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	if nIdx == 0 then
		BH_SEL[GetName()] = GetTongMemberID()
	else
		local tb, n = BH_DanhSach(nT, 8)
		if tb[nIdx] ~= nil then
			BH_SEL[GetName()] = tb[nIdx]
		end
	end
	Msg2Player(format("§· chän thµnh viªn: %s.", BH_TenTV(nT, BH_TVChon())))
	BH_ThanhVien()
end

function BH_TVChiTiet()
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	local id = BH_TVChon()
	if id == 0 then
		Msg2Player("Ch­a chän thµnh viªn.")
		BH_ThanhVien()
		return
	end
	Msg2Player(format("<color=yellow>[%s]<color> id %d - %s - giíi tÝnh %d - online %d - tån t¹i %d",
		TONGM_GetName(nT, id), id, BH_Fig(TONGM_GetFigure(nT, id)), TONGM_GetSex(nT, id), TONGM_GetOnline(nT, id), TONGM_IsExist(nT, id)))
	Msg2Player(format("Cèng hiÕn %d - tuÇn nµy %d - môc tiªu tuÇn %d - tiÒn c¸ nh©n %d",
		TONGM_GetOffer(nT, id), TONGM_GetLWeeklyOffer(nT, id), TONGM_GetLWeekGoalOffer(nT, id), TONGM_GetMoney(nT, id)))
	Msg2Player(format("Vµo bang %s (ngµy %d) - ho¹t ®éng gÇn nhÊt %s - tho¸i Èn %s",
		BH_NgayEpoch(TONGM_GetJoinTime(nT, id)), TONGM_GetJoinDay(nT, id), BH_NgayDays(TONGM_GetLastOnlineDate(nT, id)), BH_NgayDays(TONGM_GetRetireDate(nT, id))))
	local s = "QuyÒn: "
	for r, ten in pairs(BH_RIGHT) do
		s = s .. format("%d(%s)=%d ", r, ten, TONGM_CheckRight(nT, id, r))
	end
	Msg2Player(s)
	BH_ThanhVien()
end

function BH_TVOffer(v)
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	local id = BH_TVChon()
	if id == 0 then
		Msg2Player("Ch­a chän thµnh viªn.")
	else
		TONGM_ApplyAddOffer(nT, id, v)
		Msg2Player(format("§· göi céng %d cèng hiÕn cho %s (®ang %d).", v, BH_TenTV(nT, id), TONGM_GetOffer(nT, id)))
	end
	BH_ThanhVien()
end

function BH_Quyen()
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	local id = BH_TVChon()
	if id == 0 then
		Msg2Player("Ch­a chän thµnh viªn.")
		BH_ThanhVien()
		return
	end
	local t = {format("QuyÒn h¹n cña <color=green>%s<color> (%s). Bang chñ lu«n cã mäi quyÒn. LÖnh cÊp-gì göi lªn relay, xem l¹i sau vµi gi©y.", BH_TenTV(nT, id), BH_Fig(TONGM_GetFigure(nT, id)))}
	for r, ten in pairs(BH_RIGHT) do
		local nCo = TONGM_CheckRight(nT, id, r)
		if nCo == 1 then
			t[getn(t) + 1] = format("%d %s: ®ang cã - gì/#BH_QuyenGo(%d,0)", r, ten, r)
		else
			t[getn(t) + 1] = format("%d %s: kh«ng - cÊp/#BH_QuyenGo(%d,1)", r, ten, r)
		end
	end
	t[getn(t) + 1] = "Xem l¹i/BH_Quyen"
	t[getn(t) + 1] = "Quay l¹i/BH_ThanhVien"
	SayEx(t)
end

function BH_QuyenGo(nRight, nOn)
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	local id = BH_TVChon()
	if id ~= 0 then
		if nOn == 1 then
			TONGM_ApplyAddRight(nT, id, nRight)
		else
			TONGM_ApplyDelRight(nT, id, nRight)
		end
		Msg2Player(format("§· göi quyÒn %d = %d cho %s.", nRight, nOn, BH_TenTV(nT, id)))
	end
	BH_Quyen()
end

function BH_ThoaiAn(nOn)
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	local id = BH_TVChon()
	if id == 0 then
		Msg2Player("Ch­a chän thµnh viªn.")
	else
		local d = 0
		if nOn == 1 then
			d = BH_HomNay()
		end
		TONGM_ApplySetRetireDate(nT, id, d)
		Msg2Player(format("§· göi tho¸i Èn = %d (ngµy %d) cho %s.", nOn, d, BH_TenTV(nT, id)))
	end
	BH_ThanhVien()
end

function BH_DuoiHoi()
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	local id = BH_TVChon()
	if id == 0 then
		Msg2Player("Ch­a chän thµnh viªn.")
		BH_ThanhVien()
		return
	end
	SayEx({format("<color=red>§uæi<color> %s khái bang %s? Kh«ng hoµn t¸c ®­îc.", BH_TenTV(nT, id), TONG_GetName(nT)),
	"§ång ý ®uæi/BH_DuoiGo",
	"Th«i/BH_ThanhVien"})
end

function BH_DuoiGo()
	local nT = BH_T()
	local id = BH_TVChon()
	if nT ~= 0 and id ~= 0 then
		TONG_ApplyKickMember(nT, id, 0)
		Msg2Player(format("§· göi lÖnh ®uæi %s.", BH_TenTV(nT, id)))
		BH_SEL[GetName()] = nil
	end
	BH_ThanhVien()
end

-- ---------------------------------------------------------------- 3. kinh te
function BH_KinhTe()
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	Msg2Player(format("<color=yellow>[Kinh tÕ]<color> %s: quü %d l­îng - kiÕn thiÕt %d - chiÕn bÞ %d - tæng kiÕn thiÕt %d - kiÕn thiÕt tuÇn %d (trÇn %d).",
		TONG_GetName(nT), TONG_GetMoney(nT), TONG_GetBuildFund(nT), TONG_GetWarBuildFund(nT), TONG_GetTotalBuildFund(nT), TONG_GetWeekBuildFund(nT), TONG_GetWeekBuildUpper(nT)))
	Msg2Player(format("Dù tr÷: cèng hiÕn %d - kiÕn thiÕt %d. PhÝ duy tr× %d, gãp mçi ng­êi %d, chiÕn bÞ b¶o tr× tuÇn %d.",
		TONG_GetStoredOffer(nT), TONG_GetStoredBuildFund(nT), TONG_GetMaintainFund(nT), TONG_GetPerStandFund(nT), TONG_GetStandFund(nT)))
	Msg2Player(format("C¸ nh©n t«i: cèng hiÕn %d - tuÇn nµy %d - tÝch luü tuÇn %d - môc tiªu tuÇn %d.",
		GetContribution(), GetWeeklyOffer(), GetCumulateOffer(), GetWeekGoalOffer()))
	SayEx({format("%s. Kinh tÕ bang <color=green>%s<color> ®· in ra khung chat. C¸c lÖnh d­íi göi lªn relay, bÊm Xem l¹i sau vµi gi©y.", BH_ADM_TITLE, TONG_GetName(nT)),
	"Xem l¹i kinh tÕ/BH_KinhTe",
	"Quü +1.000.000 l­îng (TONG_ApplyAddMoney)/#BH_Tien(1,1000000)",
	"Quü -1.000.000 l­îng/#BH_Tien(1,-1000000)",
	"KiÕn thiÕt +10.000 (TONG_ApplyAddBuildFund)/#BH_Tien(2,10000)",
	"ChiÕn bÞ +10.000 (TONG_ApplyAddWarBuildFund)/#BH_Tien(3,10000)",
	"Cèng hiÕn dù tr÷ +5.000 (TONG_ApplyAddStoredOffer)/#BH_Tien(4,5000)",
	"KiÕn thiÕt dù tr÷ +5.000 (TONG_ApplyAddStoredBuildFund)/#BH_Tien(5,5000)",
	"Tæng kiÕn thiÕt +10.000 (TONG_ApplyAddTotalBuildFund)/#BH_Tien(6,10000)",
	"KiÕn thiÕt tuÇn +1.000, trÇn tuÇn = 50.000/#BH_Tien(7,1000)",
	"PhÝ duy tr× = 5.000, gãp mçi ng­êi +100/#BH_Tien(8,5000)",
	"T«i gãp 200 cèng hiÕn c¸ nh©n vµo dù tr÷ (TONG_ContributeOffer)/#BH_Tien(9,200)",
	"Ph¸t 100 dù tr÷ cho nhãm ®Ö tö (DistributeOfferToGroup 3)/#BH_Tien(10,100)",
	"Ph¸t 100 dù tr÷ cho thµnh viªn ®ang chän (DistributeOfferToMember)/#BH_Tien(11,100)",
	"Cèng hiÕn c¸ nh©n t«i +500 (AddContribution), tuÇn +500, môc tiªu tuÇn +500/#BH_Tien(12,500)",
	"Quay l¹i/BH_TestRoot"})
end

function BH_Tien(nLoai, v)
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	local s = "?"
	if nLoai == 1 then
		TONG_ApplyAddMoney(nT, v)
		s = format("quü %+d (®äc l¹i %d)", v, TONG_GetMoney(nT))
	elseif nLoai == 2 then
		TONG_ApplyAddBuildFund(nT, v)
		s = format("kiÕn thiÕt %+d (®äc l¹i %d)", v, TONG_GetBuildFund(nT))
	elseif nLoai == 3 then
		TONG_ApplyAddWarBuildFund(nT, v)
		s = format("chiÕn bÞ %+d (®äc l¹i %d)", v, TONG_GetWarBuildFund(nT))
	elseif nLoai == 4 then
		TONG_ApplyAddStoredOffer(nT, v)
		s = format("cèng hiÕn dù tr÷ %+d (®äc l¹i %d)", v, TONG_GetStoredOffer(nT))
	elseif nLoai == 5 then
		TONG_ApplyAddStoredBuildFund(nT, v)
		s = format("kiÕn thiÕt dù tr÷ %+d (®äc l¹i %d)", v, TONG_GetStoredBuildFund(nT))
	elseif nLoai == 6 then
		TONG_ApplyAddTotalBuildFund(nT, v)
		s = format("tæng kiÕn thiÕt %+d (®äc l¹i %d)", v, TONG_GetTotalBuildFund(nT))
	elseif nLoai == 7 then
		TONG_ApplyAddWeekBuildFund(nT, v)
		TONG_ApplySetWeekBuildUpper(nT, 50000)
		s = format("kiÕn thiÕt tuÇn %+d, trÇn 50000 (®äc l¹i %d / %d)", v, TONG_GetWeekBuildFund(nT), TONG_GetWeekBuildUpper(nT))
	elseif nLoai == 8 then
		TONG_ApplySetMaintainFund(nT, v)
		TONG_ApplyAddPerStandFund(nT, 100)
		s = format("phÝ duy tr× = %d, gãp mçi ng­êi +100 (®äc l¹i %d / %d)", v, TONG_GetMaintainFund(nT), TONG_GetPerStandFund(nT))
	elseif nLoai == 9 then
		local r = TONG_ContributeOffer(nT, GetTongMemberID(), v)
		s = format("gãp %d cèng hiÕn c¸ nh©n vµo dù tr÷, tr¶ vÒ %s (c¸ nh©n cßn %d)", v, tostring(r), GetContribution())
	elseif nLoai == 10 then
		TONG_DistributeOfferToGroup(nT, 3, v)
		s = format("ph¸t %d dù tr÷ cho nhãm ®Ö tö", v)
	elseif nLoai == 11 then
		local id = BH_TVChon()
		if id == 0 then
			s = "ch­a chän thµnh viªn (menu 2)"
		else
			TONG_DistributeOfferToMember(nT, id, v)
			s = format("ph¸t %d dù tr÷ cho %s", v, BH_TenTV(nT, id))
		end
	elseif nLoai == 12 then
		AddContribution(v)
		AddWeeklyOffer(v)
		AddWeekGoalOffer(v)
		s = format("c¸ nh©n +%d cèng hiÕn, tuÇn, môc tiªu tuÇn (®äc l¹i %d / %d / %d)", v, GetContribution(), GetWeeklyOffer(), GetWeekGoalOffer())
	end
	Msg2Player("§· göi: " .. s .. ".")
	BH_KinhTe()
end

-- ---------------------------------------------------------------- 4. muc tieu tuan
function BH_MucTieu()
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	Msg2Player(format("<color=yellow>[Môc tiªu tuÇn]<color> %s: cÊp ®ang chän %d - sù kiÖn %d - cÊp %d - bang cÇn %d, ®· %d - c¸ nh©n cÇn %d - th­ëng bang %d, c¸ nh©n %d.",
		TONG_GetName(nT), TONG_GetCurWeekGoalLevel(nT), TONG_GetWeekGoalEvent(nT), TONG_GetWeekGoalLevel(nT), TONG_GetWeekGoalTotal(nT),
		TONG_GetWeekGoalValue(nT), TONG_GetWeekGoalPlayer(nT), TONG_GetWeekGoalPriceTong(nT), TONG_GetWeekGoalPricePlayer(nT)))
	Msg2Player(format("TuÇn tr­íc: sù kiÖn %d - cÊp %d - cÇn %d, ®¹t %d - c¸ nh©n cÇn %d - th­ëng bang %d, c¸ nh©n %d. T«i ®· gãp %d ®iÓm môc tiªu tuÇn.",
		TONG_GetLWeekGoalEvent(nT), TONG_GetLWeekGoalLevel(nT), TONG_GetLWeekGoalTotal(nT), TONG_GetLWeekGoalValue(nT), TONG_GetLWeekGoalPlayer(nT),
		TONG_GetLWeekGoalPriceTong(nT), TONG_GetLWeekGoalPricePlayer(nT), GetWeekGoalOffer()))
	SayEx({format("%s. Môc tiªu tuÇn bang <color=green>%s<color> ®· in ra khung chat. Cöa sæ bang héi tab NhËt ký hiÖn b¸o c¸o môc tiªu tuÇn.", BH_ADM_TITLE, TONG_GetName(nT)),
	"Xem l¹i/BH_MucTieu",
	"§Æt cÊp môc tiªu tuÇn = 1/#BH_MTCap(1)",
	"§Æt cÊp môc tiªu tuÇn = 3/#BH_MTCap(3)",
	"§Æt cÊp môc tiªu tuÇn = 5/#BH_MTCap(5)",
	"§Æt môc tiªu: sù kiÖn 1, bang cÇn 10.000, c¸ nh©n cÇn 500, th­ëng bang 100, c¸ nh©n 10/BH_MTDat",
	"Céng 5.000 ®iÓm môc tiªu bang (TONG_ApplyAddWeekGoalValue)/#BH_MTDiem(5000)",
	"§iÓm môc tiªu bang = tæng cÇn (hoµn thµnh ngay)/#BH_MTDiem(-1)",
	"§iÓm môc tiªu tuÇn c¸ nh©n t«i +500 (AddWeekGoalOffer)/#BH_MTCaNhan(500)",
	"Chèt tuÇn (TONG_ApplyWeeklyMaintain: chuyÓn sang tuÇn tr­íc, ph¸t th­ëng)/#BH_BaoTri(2)",
	"Quay l¹i/BH_TestRoot"})
end

function BH_MTCap(lv)
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	TONG_ApplySetCurWeekGoalLevel(nT, lv)
	TONG_ApplySetWeekGoalLevel(nT, lv)
	Msg2Player(format("§· göi cÊp môc tiªu tuÇn = %d (®äc l¹i %d / %d).", lv, TONG_GetCurWeekGoalLevel(nT), TONG_GetWeekGoalLevel(nT)))
	BH_MucTieu()
end

function BH_MTDat()
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	TONG_ApplySetWeekGoalEvent(nT, 1)
	TONG_ApplySetWeekGoalTotal(nT, 10000)
	TONG_ApplySetWeekGoalPlayer(nT, 500)
	TONG_ApplySetWeekGoalPriceTong(nT, 100)
	TONG_ApplySetWeekGoalPricePlayer(nT, 10)
	Msg2Player("§· göi ®Æt môc tiªu tuÇn: sù kiÖn 1, bang cÇn 10000, c¸ nh©n cÇn 500, th­ëng bang 100, c¸ nh©n 10.")
	BH_MucTieu()
end

function BH_MTDiem(v)
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	if v < 0 then
		TONG_ApplySetWeekGoalValue(nT, TONG_GetWeekGoalTotal(nT))
		Msg2Player(format("§· göi ®iÓm môc tiªu bang = tæng cÇn %d.", TONG_GetWeekGoalTotal(nT)))
	else
		TONG_ApplyAddWeekGoalValue(nT, v)
		Msg2Player(format("§· göi céng %d ®iÓm môc tiªu bang (®äc l¹i %d).", v, TONG_GetWeekGoalValue(nT)))
	end
	BH_MucTieu()
end

function BH_MTCaNhan(v)
	AddWeekGoalOffer(v)
	Msg2Player(format("§iÓm môc tiªu tuÇn c¸ nh©n +%d (®äc l¹i %d).", v, GetWeekGoalOffer()))
	BH_MucTieu()
end

-- ---------------------------------------------------------------- 5. nhat ky, cong cao, tuyet ky
function BH_GhiChep()
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	SayEx({format("%s. Bang <color=green>%s<color>: c«ng c¸o hiÖn t¹i: %s. TuyÖt kü (tuyªn chiÕn, cÊm ®Þa) ®Æt ë field bang; nhËt ký xem tab NhËt ký cña cöa sæ bang.", BH_ADM_TITLE, TONG_GetName(nT), tostring(TONG_GetAnnouncement(nT))),
	"Ghi 1 dßng sù kiÖn bang (TONG_ApplyAddEventRecord)/#BH_GhiDong(1)",
	"Ghi 1 dßng lÞch sö bang (TONG_ApplyAddHistoryRecord)/#BH_GhiDong(2)",
	"Ghi 20 dßng sù kiÖn (thö cuén nhËt ký)/#BH_GhiDong(3)",
	"§Æt c«ng c¸o test (TONG_ApplySetAnnouncement)/#BH_CongCao(1)",
	"Xo¸ c«ng c¸o/#BH_CongCao(0)",
	"TuyÖt kü = 1 (TONG_ApplySetStunt)/#BH_TuyetKy(1)",
	"TuyÖt kü = 0/#BH_TuyetKy(0)",
	"§æi ngo¹i h×nh toµn bang 60 gi©y (TONG_ChangeAllMemberFeature 1)/#BH_NgoaiHinh(1)",
	"T¹m ng­ng t¸c ph­êng bËt (TONG_ApplySetPauseState 1)/#BH_TamNgung(1)",
	"T¹m ng­ng t¸c ph­êng t¾t/#BH_TamNgung(0)",
	"Ghi 1 dßng log bang ra tÖp (TONG_WriteLog)/#BH_GhiDong(4)",
	"Quay l¹i/BH_TestRoot"})
end

function BH_GhiDong(nLoai)
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	local sz = format("[Test %s] %s ghi thö lóc %s", GetName(), TONG_GetName(nT), FormatTime2String(GetCurrentTime()))
	if nLoai == 1 then
		TONG_ApplyAddEventRecord(nT, sz)
		Msg2Player("§· göi 1 dßng sù kiÖn: " .. sz)
	elseif nLoai == 2 then
		TONG_ApplyAddHistoryRecord(nT, sz)
		Msg2Player("§· göi 1 dßng lÞch sö: " .. sz)
	elseif nLoai == 3 then
		for i = 1, 20 do
			TONG_ApplyAddEventRecord(nT, format("[Test %d/20] dßng sù kiÖn dµi trung b×nh ®Ó thö cuén trang nhËt ký bang héi", i))
		end
		Msg2Player("§· göi 20 dßng sù kiÖn.")
	else
		TONG_WriteLog(sz)
		Msg2Player("§· ghi log bang: " .. sz)
	end
	BH_GhiChep()
end

function BH_CongCao(nOn)
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	if nOn == 1 then
		TONG_ApplySetAnnouncement(nT, format("C«ng c¸o test cña %s lóc %s. Chµo mõng thµnh viªn míi!", GetName(), FormatTime2String(GetCurrentTime())))
	else
		TONG_ApplySetAnnouncement(nT, "")
	end
	Msg2Player(format("§· göi c«ng c¸o (%d). §äc l¹i sau vµi gi©y: %s", nOn, tostring(TONG_GetAnnouncement(nT))))
	BH_GhiChep()
end

function BH_TuyetKy(n)
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	TONG_ApplySetStunt(nT, n)
	Msg2Player(format("§· göi tuyÖt kü = %d.", n))
	BH_GhiChep()
end

function BH_NgoaiHinh(n)
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	local r = TONG_ChangeAllMemberFeature(nT, n, 60)
	Msg2Player(format("TONG_ChangeAllMemberFeature(%d, 60 gi©y) tr¶ vÒ %s.", n, tostring(r)))
	BH_GhiChep()
end

function BH_TamNgung(n)
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	TONG_ApplySetPauseState(nT, n)
	Msg2Player(format("§· göi t¹m ng­ng t¸c ph­êng = %d (®äc l¹i %d).", n, TONG_GetPauseState(nT)))
	BH_GhiChep()
end

-- ---------------------------------------------------------------- 6. lanh dia + tac phuong
function BH_LanhDia()
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	local n = 0
	local t = TWS_GetFirstWorkshop(nT)
	local s = ""
	while t ~= nil and t ~= 0 and n < 7 do
		n = n + 1
		local szMo = "®ãng"
		if TWS_IsOpen(nT, t) == 1 then
			szMo = "më"
		end
		s = s .. format("khu %d cÊp %d %s s¶n l­îng %d; ", t, TWS_GetLevel(nT, t), szMo, TWS_GetDayOutput(nT, t))
		t = TWS_GetNextWorkshop(nT, t)
	end
	if s == "" then
		s = "ch­a lËp khu nµo"
	end
	Msg2Player(format("<color=yellow>[L·nh ®Þa]<color> map %d, mÉu %d, cÊm ®Þa %d, cÊp kiÕn thiÕt %d. T¸c ph­êng: %s",
		TONG_GetTongMap(nT), TONG_GetTongMapTemplate(nT), TONG_GetTongMapBan(nT), TONG_GetBuildLevel(nT), s))
	SayEx({format("%s. L·nh ®Þa vµ t¸c ph­êng bang <color=green>%s<color> ®· in ra khung chat. C¸c bé TLD_/TPT_ (bangthanh_f.lua) cã nót Trë vÒ TX_Root cña bé cò.", BH_ADM_TITLE, TONG_GetName(nT)),
	"Xem l¹i/BH_LanhDia",
	"L·nh ®Þa + kiÕn thiÕt: xem, chän khu chung, t¹o riªng theo mÉu, vµo, n©ng cÊp, cÊm ®Þa, xo¸ (bé TLD_)/TLD_Menu",
	"Vµo l·nh ®Þa bang ngay (TLD_Go)/TLD_Go",
	"T¹o l·nh ®Þa riªng theo mÉu (TONG_ApplyCreatMap)/TLD_Crt",
	"Xo¸ l·nh ®Þa (TONG_ApplyDeleteMap) - hái tr­íc/TLD_Del",
	"T¸c ph­êng 7 khu: xem, lËp, n©ng, më-®ãng, s¶n l­îng, s¶n xuÊt thö (bé TPT_)/TPT_Menu",
	"B¶o tr× t¸c ph­êng toµn server (TWS_MaintainAll)/BH_TWSMaintain",
	"Quay l¹i/BH_TestRoot"})
end

function BH_TWSMaintain()
	local r = TWS_MaintainAll()
	Msg2Player(format("TWS_MaintainAll tr¶ vÒ %s.", tostring(r)))
	BH_LanhDia()
end

-- ---------------------------------------------------------------- 7. bang khac, lien minh
function BH_BangKhac()
	local nT = BH_T()
	local nCnt = TONG_GetTongCount()
	Msg2Player(format("<color=yellow>[Bang kh¸c]<color> m¸y chñ cã %d bang (in tèi ®a 12):", nCnt))
	local id = TONG_GetFirstTong()
	local n = 0
	while id ~= nil and id ~= 0 and n < 12 do
		n = n + 1
		local szLM = ""
		local nU = TONG_GetUnionID(id)
		if nU ~= nil and nU ~= 0 then
			szLM = format(" - liªn minh %d", nU)
		end
		local szMe = ""
		if id == nT then
			szMe = " <color=green>(bang t«i)<color>"
		end
		Msg2Player(format("%d. %s (id %d) - bang chñ %s - %d ng­êi - cÊp kiÕn thiÕt %d - ®¬n chê %d%s%s",
			n, TONG_GetName(id), id, TONG_GetMaster(id), TONG_GetMemberCount(id, -1), TONG_GetBuildLevel(id), TONG_GetApplyCount(id), szLM, szMe))
		id = TONG_GetNextTong(id)
	end
	local szLM = "B¹n ch­a vµo bang."
	if nT ~= 0 then
		local nU = TONG_GetUnionID(nT)
		if nU == nil or nU == 0 then
			szLM = format("Bang %s ch­a cã liªn minh.", TONG_GetName(nT))
		else
			local s = ""
			local k = TONG_GetFirstTong()
			while k ~= nil and k ~= 0 do
				if TONG_GetUnionID(k) == nU then
					s = s .. TONG_GetName(k) .. " "
				end
				k = TONG_GetNextTong(k)
			end
			szLM = format("Bang %s thuéc liªn minh %d gåm: %s", TONG_GetName(nT), nU, s)
		end
	end
	Msg2Player(szLM)
	SayEx({format("%s. %s LËp, xin vµo, duyÖt, rêi, huû liªn minh vµ duyÖt ®¬n xin vµo bang chØ cã qua cöa sæ bang héi (tab Chøc n¨ng, trang Liªn minh) - Lua kh«ng cã lÖnh.", BH_ADM_TITLE, szLM),
	"Xem l¹i danh s¸ch bang/BH_BangKhac",
	"Xin vµo mét bang (TONG_ApplyJoin: ®ñ cÊp tù nhËn th× vµo th¼ng, kh«ng th× vµo danh s¸ch chê)/BH_XinVao",
	"T×m bang theo tªn nh©n vËt cña t«i (TONG_GetTongByRoleName)/BH_TimBang",
	"Quay l¹i/BH_TestRoot"})
end

function BH_XinVao()
	local nT = BH_T()
	if nT ~= 0 then
		Msg2Player(format("B¹n ®ang ë bang %s, ph¶i rêi bang tr­íc (môc 9).", TONG_GetName(nT)))
		BH_BangKhac()
		return
	end
	local t = {"Chän bang muèn xin vµo (8 bang ®Çu):"}
	local id = TONG_GetFirstTong()
	local n = 0
	while id ~= nil and id ~= 0 and n < 8 do
		n = n + 1
		t[getn(t) + 1] = format("%s - %d ng­êi/#BH_XinVaoGo(%d)", TONG_GetName(id), TONG_GetMemberCount(id, -1), n)
		id = TONG_GetNextTong(id)
	end
	t[getn(t) + 1] = "Quay l¹i/BH_BangKhac"
	SayEx(t)
end

function BH_XinVaoGo(nIdx)
	local id = TONG_GetFirstTong()
	local n = 1
	while id ~= nil and id ~= 0 and n < nIdx do
		n = n + 1
		id = TONG_GetNextTong(id)
	end
	if id == nil or id == 0 then
		Msg2Player("Kh«ng t×m thÊy bang.")
	else
		local r = TONG_ApplyJoin(id)
		Msg2Player(format("TONG_ApplyJoin(%s) tr¶ vÒ %d (2 vµo th¼ng, 1 vµo danh s¸ch chê, -1 d­íi cÊp, 0 thÊt b¹i).", TONG_GetName(id), r))
	end
	BH_BangKhac()
end

function BH_TimBang()
	local id = TONG_GetTongByRoleName(GetName())
	if id == nil or id == 0 then
		Msg2Player(format("TONG_GetTongByRoleName(%s) = 0: kh«ng thuéc bang nµo.", GetName()))
	else
		Msg2Player(format("TONG_GetTongByRoleName(%s) = %d (%s); TONG_Name2ID = %d.", GetName(), id, TONG_GetName(id), TONG_Name2ID(TONG_GetName(id))))
	end
	BH_BangKhac()
end

-- ---------------------------------------------------------------- 8. hoat dong bang
function BH_HoatDong()
	local nT = BH_T()
	local sz = "B¹n ch­a vµo bang."
	if nT ~= 0 then
		sz = format("Bang %s: tr¹ng th¸i chiÕn %d, ngµy chiÕm thµnh %d, chøc ®¹i thÇn cña t«i (GetTongDuty) %d.", TONG_GetName(nT), TONG_GetWarState(nT), TONG_GetOccupyCityDay(nT), GetTongDuty())
	end
	SayEx({format("%s. %s", BH_ADM_TITLE, sz),
	"C«ng thµnh chiÕn: 7 thµnh, b¸o danh, bæ nhiÖm, khiªu chiÕn, Ðp pha (bé TCT_)/TCT_Menu",
	"VËn tiªu bang héi 10 sao: biÕn bang 1149, reset tuÇn, r­¬ng, tr¹ng th¸i (bé VT_ADM)/VT_ADM_Bang",
	"Bang chiÕn - B¸ch Nh©n - Tû Vâ - Thµnh B¶o (bé HD_)/HD_AdminMenu",
	"Danh hiÖu + thuÕ Th¸i thó (bé TDT_)/TDT_Menu",
	"Tuyªn chiÕn (TongClaimWar: ®Æt tr¹ng th¸i chiÕn + ghi sù kiÖn)/BH_TuyenChien",
	"Quay l¹i/BH_TestRoot"})
end

function BH_TuyenChien()
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	local r = TongClaimWar(nT)
	Msg2Player(format("TongClaimWar(%s) tr¶ vÒ %s; tr¹ng th¸i chiÕn ®äc l¹i %d.", TONG_GetName(nT), tostring(r), TONG_GetWarState(nT)))
	BH_HoatDong()
end

-- ---------------------------------------------------------------- 9. tien ich
function BH_TienIch()
	local nT = BH_T()
	local sz = "B¹n ch­a vµo bang."
	if nT ~= 0 then
		sz = format("B¹n ë bang %s (NameID thµnh viªn %d, chøc %s, bang chñ = %d).", TONG_GetName(nT), GetTongMemberID(), BH_Fig(GetTongFigure()), CheckTongMasterPower())
	end
	SayEx({format("%s. %s Bang test mÆc ®Þnh: TESTGAME (g_TX_TESTTONG cña bé cò).", BH_ADM_TITLE, sz),
	"Vµo bang test TESTGAME hoÆc bang ®Çu tiªn (TONG_ApplyJoin)/BH_VaoBangTest",
	"Rêi bang (TONG_ApplyDeleteMember chÝnh t«i) - hái tr­íc/BH_RoiBangHoi",
	"Khëi t¹o l¹i d÷ liÖu bang (TONG_ApplyInit) - nguy hiÓm, hái tr­íc/BH_InitHoi",
	"N¹p l¹i file test (dofile test_banghoi_admin.lua)/BH_NapLai",
	"Bé test cò: c«ng thµnh, l·nh ®Þa, t¸c ph­êng, danh hiÖu, thuÕ (TX_Root)/TX_Root",
	"Quay l¹i/BH_TestRoot"})
end

function BH_VaoBangTest()
	local nT = BH_T()
	if nT ~= 0 then
		Msg2Player(format("B¹n ®ang ë bang %s, ph¶i rêi bang tr­íc.", TONG_GetName(nT)))
		BH_TienIch()
		return
	end
	local id = TONG_Name2ID(g_TX_TESTTONG or "TESTGAME")
	if id == nil or id == 0 then
		id = TONG_GetFirstTong()
	end
	if id == nil or id == 0 then
		Msg2Player("M¸y chñ ch­a cã bang nµo.")
	else
		local r = TONG_ApplyJoin(id)
		Msg2Player(format("TONG_ApplyJoin(%s) tr¶ vÒ %d (2 vµo th¼ng, 1 chê duyÖt, -1 d­íi cÊp, 0 thÊt b¹i).", TONG_GetName(id), r))
	end
	BH_TienIch()
end

function BH_RoiBangHoi()
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	SayEx({format("<color=red>Rêi bang<color> %s? Bang chñ kh«ng rêi ®­îc (ph¶i chuyÓn vÞ tr­íc b»ng cöa sæ bang).", TONG_GetName(nT)),
	"§ång ý rêi bang/BH_RoiBangGo",
	"Th«i/BH_TienIch"})
end

function BH_RoiBangGo()
	local nT = BH_T()
	if nT ~= 0 then
		local r = TONG_ApplyDeleteMember(nT, GetTongMemberID(), 0)
		Msg2Player(format("TONG_ApplyDeleteMember tr¶ vÒ %s.", tostring(r)))
	end
	BH_TienIch()
end

function BH_InitHoi()
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	SayEx({format("<color=red>Khëi t¹o l¹i<color> d÷ liÖu bang %s (TONG_ApplyInit: relay ®Æt l¹i field bang vÒ mÆc ®Þnh)? ChØ lµm víi bang test.", TONG_GetName(nT)),
	"§ång ý khëi t¹o/BH_InitGo",
	"Th«i/BH_TienIch"})
end

function BH_InitGo()
	local nT = BH_T()
	if nT ~= 0 then
		TONG_ApplyInit(nT)
		Msg2Player(format("§· göi TONG_ApplyInit cho bang %s.", TONG_GetName(nT)))
	end
	BH_TienIch()
end

function BH_NapLai()
	dofile("script/kiemthu/item/test_banghoi_admin.lua")
	Msg2Player("§· n¹p l¹i test_banghoi_admin.lua.")
	BH_TestRoot()
end
