-- ============================================================================
-- TEST_BANGHOI_ADMIN.LUA  [BHTEST 07/09, sua 2]  Bo test BANG HOI DAY DU tren Lenh Bai Admin
-- (Include tu lenhbaiadmin.lua; main() cua lenh bai dofile lai => sua KHONG can restart).
-- 3 trang goc, moi hop thoai TOI DA 6 NUT (khung Select_List chi ve 6 dong, khong cuon),
-- chu hoi TOI DA 3 DONG NGAN (khung InfoText 96 px = 6 dong); moi trang thai dai in ra khung chat.
-- Muc: 1 ho so + bao tri   2 thanh vien  3 kinh te  4 muc tieu tuan  5 nhat ky/cong cao/tuyet ky
--      6 lanh dia + tac phuong (bo TLD_ vµ TPT_)  7 bang khac + lien minh  8 CONG THANH + THAI THU
--      9 hoat dong (van tieu VT_ADM_Bang, bang chien HD_, danh hieu-thue TDT_, tuyen chien)
--      10 cap bang + kinh nghiem (BHLV)  11 tien ich.
-- LUAT: nhan menu CAM dau "/"; "#Ham(thamso)" toi da 31 ky tu; ham nhan dap an lay tham so SO (atoi).
-- Moi lenh Apply*/TONGM_Apply* CHI GUI LEN RELAY (khong ap lac quan) -> bam "Xem lai" sau vai giay.
-- Chon thanh vien: BH_SEL[ten admin]; chon thanh: BH_CITY[ten admin] (1..7, mac dinh 1).
-- ============================================================================

BH_ADM_TITLE = "<color=yellow>Bang héi<color> - bé test"
BH_FIG = {}
BH_FIG[0] = "Bang chñ"
BH_FIG[1] = "Tr­ëng l·o"
BH_FIG[2] = "§éi tr­ëng"
BH_FIG[3] = "§Ö tö"
BH_FIG[4] = "Èn sÜ"
BH_SEL = BH_SEL or {}
BH_CITY = BH_CITY or {}
BH_RIGHT = {}
BH_RIGHT[1901] = "duyÖt ®¬n"
BH_RIGHT[1002] = "bæ nhiÖm"
BH_RIGHT[1101] = "liªn minh"
BH_RIGHT[2004] = "l·nh ®Þa"
BH_RIGHT[9001] = "t¸c ph­êng"

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

function BH_NgayDays(d)
	if d == nil or d <= 0 then
		return "--"
	end
	if d > 1000000 then
		return BH_NgayEpoch(d)
	end
	return BH_NgayEpoch(d * 86400)
end

function BH_HomNay()
	return floor(GetCurrentTime() / 86400)
end

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

function BH_Thanh()
	local n = BH_CITY[GetName()]
	if n == nil or n < 1 or n > 7 then
		return 1
	end
	return n
end

function BH_TenBang()
	local nT = BH_T()
	if nT == 0 then
		return ""
	end
	return TONG_GetName(nT)
end

-- hop thoai chung: chua vao bang
function BH_ChuaVaoBang(szVe)
	SayEx({"B¹n <color=red>ch­a vµo bang<color>. Vµo bang ë môc 7 (xin vµo bang) hoÆc 11 (bang test).",
	"Quay l¹i/"..szVe,
	"Tho¸t/no"})
end

-- ---------------------------------------------------------------- goc (3 trang, moi trang 6 nut)
function BH_TestRoot()
	local nT = BH_T()
	local sz
	if nT == 0 then
		sz = format("%s. B¹n <color=red>ch­a vµo bang<color>.", BH_ADM_TITLE)
	else
		sz = format("%s. Bang <color=green>%s<color>: %d ng­êi, %d online, cÊp %d, quü %d.", BH_ADM_TITLE,
			TONG_GetName(nT), TONG_GetMemberCount(nT, -1), TONG_GetOnlineCount(nT), TONG_GetExpLevel(nT), TONG_GetMoney(nT))
	end
	SayEx({sz,
	"1. Hå s¬ bang + b¶o tr× ngµy, tuÇn/BH_HoSo",
	"2. Thµnh viªn, chøc vô, quyÒn h¹n/BH_ThanhVien",
	"3. Quü, kiÕn thiÕt, chiÕn bÞ, cèng hiÕn/BH_KinhTe",
	"4. Môc tiªu tuÇn/BH_MucTieu",
	"Trang 2/BH_TestRoot2",
	"Tho¸t/no"})
end

function BH_TestRoot2()
	SayEx({format("%s - trang 2.", BH_ADM_TITLE),
	"5. NhËt ký, c«ng c¸o, tuyÖt kü, ngo¹i h×nh/BH_GhiChep",
	"6. L·nh ®Þa, kiÕn thiÕt, t¸c ph­êng/BH_LanhDia",
	"7. Bang kh¸c, liªn minh, xin vµo bang/BH_BangKhac",
	"8. C«ng thµnh: Th¸i thó, khiªu chiÕn, Ðp pha, thuÕ/BH_CongThanh",
	"Trang 3/BH_TestRoot3",
	"Tho¸t/no"})
end

function BH_TestRoot3()
	SayEx({format("%s - trang 3.", BH_ADM_TITLE),
	"9. Ho¹t ®éng: vËn tiªu bang, bang chiÕn, danh hiÖu, tuyªn chiÕn/BH_HoatDong",
	"10. CÊp bang + kinh nghiÖm bang/BH_CapBang",
	"11. TiÖn Ých: bang test, rêi bang, khëi t¹o, n¹p l¹i/BH_TienIch",
	"VÒ trang 1/BH_TestRoot",
	"Quay l¹i bé test ho¹t ®éng/ADM_TestHoatDong",
	"Tho¸t/no"})
end

-- ---------------------------------------------------------------- 1. ho so
function BH_HoSo()
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	Msg2Player(format("<color=yellow>[Hå s¬]<color> %s (id %d) - bang chñ %s - %d thµnh viªn, %d online, %d Èn sÜ, %d ®¬n chê.",
		TONG_GetName(nT), nT, TONG_GetMaster(nT), TONG_GetMemberCount(nT, -1), TONG_GetOnlineCount(nT), TONG_GetMemberCount(nT, 4), TONG_GetApplyCount(nT)))
	Msg2Player(format("CÊp bang %d (kinh nghiÖm %d) - cÊp kiÕn thiÕt %d - tÝn dông %d - ­u ®·i %d.",
		TONG_GetExpLevel(nT), TONG_GetExp(nT), TONG_GetBuildLevel(nT), TONG_GetCredit(nT), TONG_GetPremium(nT)))
	Msg2Player(format("Quü %d - kiÕn thiÕt %d - chiÕn bÞ %d - tæng kiÕn thiÕt %d - kiÕn thiÕt tuÇn %d (trÇn %d).",
		TONG_GetMoney(nT), TONG_GetBuildFund(nT), TONG_GetWarBuildFund(nT), TONG_GetTotalBuildFund(nT), TONG_GetWeekBuildFund(nT), TONG_GetWeekBuildUpper(nT)))
	Msg2Player(format("PhÝ duy tr× %d - gãp mçi ng­êi %d - chiÕn bÞ b¶o tr× tuÇn %d - dù tr÷: cèng hiÕn %d, kiÕn thiÕt %d.",
		TONG_GetMaintainFund(nT), TONG_GetPerStandFund(nT), TONG_GetStandFund(nT), TONG_GetStoredOffer(nT), TONG_GetStoredBuildFund(nT)))
	Msg2Player(format("Ngµy %d - tuÇn %d - t¹m ng­ng t¸c ph­êng %d - tr¹ng th¸i chiÕn %d - ngµy chiÕm thµnh %d - liªn minh %d.",
		TONG_GetDay(nT), TONG_GetWeek(nT), TONG_GetPauseState(nT), TONG_GetWarState(nT), TONG_GetOccupyCityDay(nT), TONG_GetUnionID(nT)))
	Msg2Player(format("L·nh ®Þa map %d, mÉu %d, cÊm ®Þa %d - t¸c ph­êng %d khu - c«ng c¸o: %s",
		TONG_GetTongMap(nT), TONG_GetTongMapTemplate(nT), TONG_GetTongMapBan(nT), TWS_GetWorkshopCount(nT), tostring(TONG_GetAnnouncement(nT))))
	SayEx({format("Hå s¬ bang <color=green>%s<color> ®· in ra khung chat. LÖnh göi lªn relay, xem l¹i sau vµi gi©y.", TONG_GetName(nT)),
	"Xem l¹i hå s¬/BH_HoSo",
	"B¶o tr× ngµy (TONG_ApplyMaintain)/#BH_BaoTri(1)",
	"B¶o tr× tuÇn (TONG_ApplyWeeklyMaintain)/#BH_BaoTri(2)",
	"N©ng-h¹ cÊp kiÕn thiÕt, ngµy +1, tuÇn +1/BH_HoSo2",
	"Quay l¹i/BH_TestRoot",
	"Tho¸t/no"})
end

function BH_HoSo2()
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	SayEx({format("Bang %s: cÊp kiÕn thiÕt %d, ngµy %d, tuÇn %d.", TONG_GetName(nT), TONG_GetBuildLevel(nT), TONG_GetDay(nT), TONG_GetWeek(nT)),
	"N©ng cÊp kiÕn thiÕt (TONG_ApplyUpgrade)/#BH_CapKT(1)",
	"H¹ cÊp kiÕn thiÕt (TONG_ApplyDegrade)/#BH_CapKT(0)",
	"Ngµy +1 (TONG_ApplySetDay)/#BH_NgayTuan(1)",
	"TuÇn +1 (TONG_ApplySetWeek)/#BH_NgayTuan(2)",
	"Quay l¹i/BH_HoSo",
	"Tho¸t/no"})
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
	BH_HoSo2()
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
		Msg2Player(format("Ngµy: %d -> %d.", n, n + 1))
	else
		local n = TONG_GetWeek(nT)
		TONG_ApplySetWeek(nT, n + 1)
		Msg2Player(format("TuÇn: %d -> %d.", n, n + 1))
	end
	BH_HoSo2()
end

-- ---------------------------------------------------------------- 2. thanh vien
function BH_ThanhVien()
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	local tb, n = BH_DanhSach(nT, 25)
	Msg2Player(format("<color=yellow>[Thµnh viªn]<color> %s: %d ng­êi, %d online (in tèi ®a 25).", TONG_GetName(nT), TONG_GetMemberCount(nT, -1), TONG_GetOnlineCount(nT)))
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
	SayEx({format("Danh s¸ch ®· in ra khung chat. §ang chän: <color=green>%s<color>. Th¨ng-gi¸ng chøc, chuyÓn bang chñ chØ cã ë cöa sæ bang.", BH_TenTV(nT, BH_TVChon())),
	"Xem l¹i danh s¸ch/BH_ThanhVien",
	"Chän thµnh viªn/BH_ChonTV",
	"Thao t¸c thµnh viªn ®ang chän/BH_TVMenu",
	"QuyÒn h¹n thµnh viªn ®ang chän/BH_Quyen",
	"Quay l¹i/BH_TestRoot",
	"Tho¸t/no"})
end

function BH_ChonTV()
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	local tb, n = BH_DanhSach(nT, 4)
	local t = {format("Chän thµnh viªn (4 ng­êi ®Çu, bang cã %d). §ang chän: %s.", TONG_GetMemberCount(nT, -1), BH_TenTV(nT, BH_TVChon()))}
	for i = 1, n do
		t[getn(t) + 1] = format("%d. %s - %s/#BH_ChonTVGo(%d)", i, TONGM_GetName(nT, tb[i]), BH_Fig(TONGM_GetFigure(nT, tb[i])), i)
	end
	t[getn(t) + 1] = "ChÝnh t«i/#BH_ChonTVGo(0)"
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
		local tb, n = BH_DanhSach(nT, 4)
		if tb[nIdx] ~= nil then
			BH_SEL[GetName()] = tb[nIdx]
		end
	end
	Msg2Player(format("§· chän thµnh viªn: %s.", BH_TenTV(nT, BH_TVChon())))
	BH_ThanhVien()
end

function BH_TVMenu()
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	local id = BH_TVChon()
	if id == 0 then
		Msg2Player("Ch­a chän thµnh viªn.")
		BH_ChonTV()
		return
	end
	SayEx({format("Thµnh viªn <color=green>%s<color> (%s), cèng hiÕn %d.", BH_TenTV(nT, id), BH_Fig(TONGM_GetFigure(nT, id)), TONGM_GetOffer(nT, id)),
	"Xem chi tiÕt (mäi kho¸)/BH_TVChiTiet",
	"Céng 500 cèng hiÕn (TONGM_ApplyAddOffer)/#BH_TVOffer(500)",
	"Trõ 500 cèng hiÕn/#BH_TVOffer(-500)",
	"Tho¸i Èn, ®uæi, ph¸t dù tr÷/BH_TVMenu2",
	"Quay l¹i/BH_ThanhVien",
	"Tho¸t/no"})
end

function BH_TVMenu2()
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	local id = BH_TVChon()
	SayEx({format("Thµnh viªn <color=green>%s<color>, tho¸i Èn: %s.", BH_TenTV(nT, id), BH_NgayDays(TONGM_GetRetireDate(nT, id))),
	"Tho¸i Èn (SetRetireDate = h«m nay)/#BH_ThoaiAn(1)",
	"Bá tho¸i Èn (SetRetireDate = 0)/#BH_ThoaiAn(0)",
	"Ph¸t 100 cèng hiÕn dù tr÷ cho ng­êi nµy/#BH_Tien(11,100)",
	"§uæi khái bang (hái tr­íc)/BH_DuoiHoi",
	"Quay l¹i/BH_TVMenu",
	"Tho¸t/no"})
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
		s = s .. format("%d %s=%d ", r, ten, TONGM_CheckRight(nT, id, r))
	end
	Msg2Player(s)
	BH_TVMenu()
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
	BH_TVMenu()
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
		BH_ChonTV()
		return
	end
	local t = {format("QuyÒn cña <color=green>%s<color> (%s). BÊm ®Ó cÊp hoÆc gì; bang chñ lu«n cã mäi quyÒn.", BH_TenTV(nT, id), BH_Fig(TONGM_GetFigure(nT, id)))}
	for r, ten in pairs(BH_RIGHT) do
		if TONGM_CheckRight(nT, id, r) == 1 then
			t[getn(t) + 1] = format("%d %s: ®ang cã - gì/#BH_QuyenGo(%d,0)", r, ten, r)
		else
			t[getn(t) + 1] = format("%d %s: kh«ng - cÊp/#BH_QuyenGo(%d,1)", r, ten, r)
		end
	end
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
		Msg2Player(format("§· göi quyÒn %d = %d cho %s (xem l¹i sau vµi gi©y).", nRight, nOn, BH_TenTV(nT, id)))
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
	BH_TVMenu2()
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
	"Th«i/BH_TVMenu2"})
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
	Msg2Player(format("<color=yellow>[Kinh tÕ]<color> %s: quü %d - kiÕn thiÕt %d - chiÕn bÞ %d - tæng kiÕn thiÕt %d - tuÇn %d (trÇn %d).",
		TONG_GetName(nT), TONG_GetMoney(nT), TONG_GetBuildFund(nT), TONG_GetWarBuildFund(nT), TONG_GetTotalBuildFund(nT), TONG_GetWeekBuildFund(nT), TONG_GetWeekBuildUpper(nT)))
	Msg2Player(format("Dù tr÷: cèng hiÕn %d, kiÕn thiÕt %d. PhÝ duy tr× %d, gãp mçi ng­êi %d, chiÕn bÞ b¶o tr× tuÇn %d.",
		TONG_GetStoredOffer(nT), TONG_GetStoredBuildFund(nT), TONG_GetMaintainFund(nT), TONG_GetPerStandFund(nT), TONG_GetStandFund(nT)))
	Msg2Player(format("C¸ nh©n t«i: cèng hiÕn %d - tuÇn nµy %d - tÝch luü %d - môc tiªu tuÇn %d.",
		GetContribution(), GetWeeklyOffer(), GetCumulateOffer(), GetWeekGoalOffer()))
	SayEx({format("Kinh tÕ bang <color=green>%s<color> ®· in ra khung chat.", TONG_GetName(nT)),
	"Xem l¹i/BH_KinhTe",
	"Quü, kiÕn thiÕt, chiÕn bÞ/BH_KinhTe2",
	"Dù tr÷, tæng kiÕn thiÕt, tuÇn/BH_KinhTe3",
	"Cèng hiÕn: gãp, ph¸t, c¸ nh©n, phÝ/BH_KinhTe4",
	"Quay l¹i/BH_TestRoot",
	"Tho¸t/no"})
end

function BH_KinhTe2()
	SayEx({"Quü, kiÕn thiÕt, chiÕn bÞ (lÖnh göi relay).",
	"Quü +1.000.000 l­îng (TONG_ApplyAddMoney)/#BH_Tien(1,1000000)",
	"Quü -1.000.000 l­îng/#BH_Tien(1,-1000000)",
	"KiÕn thiÕt +10.000 (TONG_ApplyAddBuildFund)/#BH_Tien(2,10000)",
	"ChiÕn bÞ +10.000 (TONG_ApplyAddWarBuildFund)/#BH_Tien(3,10000)",
	"Quay l¹i/BH_KinhTe",
	"Tho¸t/no"})
end

function BH_KinhTe3()
	SayEx({"Dù tr÷, tæng kiÕn thiÕt, kiÕn thiÕt tuÇn.",
	"Cèng hiÕn dù tr÷ +5.000 (AddStoredOffer)/#BH_Tien(4,5000)",
	"KiÕn thiÕt dù tr÷ +5.000 (AddStoredBuildFund)/#BH_Tien(5,5000)",
	"Tæng kiÕn thiÕt +10.000 (AddTotalBuildFund)/#BH_Tien(6,10000)",
	"KiÕn thiÕt tuÇn +1.000, trÇn tuÇn = 50.000/#BH_Tien(7,1000)",
	"Quay l¹i/BH_KinhTe",
	"Tho¸t/no"})
end

function BH_KinhTe4()
	SayEx({"Cèng hiÕn: gãp, ph¸t, c¸ nh©n; phÝ duy tr×.",
	"T«i gãp 200 cèng hiÕn vµo dù tr÷ (ContributeOffer)/#BH_Tien(9,200)",
	"Ph¸t 100 dù tr÷ cho nhãm ®Ö tö (DistributeOfferToGroup)/#BH_Tien(10,100)",
	"C¸ nh©n t«i +500: cèng hiÕn, tuÇn, môc tiªu tuÇn/#BH_Tien(12,500)",
	"PhÝ duy tr× = 5.000, gãp mçi ng­êi +100/#BH_Tien(8,5000)",
	"Quay l¹i/BH_KinhTe",
	"Tho¸t/no"})
end

function BH_Tien(nLoai, v)
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	local s = "?"
	local szVe = "BH_KinhTe2"
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
		szVe = "BH_KinhTe3"
	elseif nLoai == 5 then
		TONG_ApplyAddStoredBuildFund(nT, v)
		s = format("kiÕn thiÕt dù tr÷ %+d (®äc l¹i %d)", v, TONG_GetStoredBuildFund(nT))
		szVe = "BH_KinhTe3"
	elseif nLoai == 6 then
		TONG_ApplyAddTotalBuildFund(nT, v)
		s = format("tæng kiÕn thiÕt %+d (®äc l¹i %d)", v, TONG_GetTotalBuildFund(nT))
		szVe = "BH_KinhTe3"
	elseif nLoai == 7 then
		TONG_ApplyAddWeekBuildFund(nT, v)
		TONG_ApplySetWeekBuildUpper(nT, 50000)
		s = format("kiÕn thiÕt tuÇn %+d, trÇn 50000 (®äc l¹i %d / %d)", v, TONG_GetWeekBuildFund(nT), TONG_GetWeekBuildUpper(nT))
		szVe = "BH_KinhTe3"
	elseif nLoai == 8 then
		TONG_ApplySetMaintainFund(nT, v)
		TONG_ApplyAddPerStandFund(nT, 100)
		s = format("phÝ duy tr× = %d, gãp mçi ng­êi +100 (®äc l¹i %d / %d)", v, TONG_GetMaintainFund(nT), TONG_GetPerStandFund(nT))
		szVe = "BH_KinhTe4"
	elseif nLoai == 9 then
		local r = TONG_ContributeOffer(nT, GetTongMemberID(), v)
		s = format("gãp %d cèng hiÕn vµo dù tr÷, tr¶ vÒ %s (c¸ nh©n cßn %d)", v, tostring(r), GetContribution())
		szVe = "BH_KinhTe4"
	elseif nLoai == 10 then
		TONG_DistributeOfferToGroup(nT, 3, v)
		s = format("ph¸t %d dù tr÷ cho nhãm ®Ö tö", v)
		szVe = "BH_KinhTe4"
	elseif nLoai == 11 then
		local id = BH_TVChon()
		if id == 0 then
			s = "ch­a chän thµnh viªn (môc 2)"
		else
			TONG_DistributeOfferToMember(nT, id, v)
			s = format("ph¸t %d dù tr÷ cho %s", v, BH_TenTV(nT, id))
		end
		szVe = "BH_TVMenu2"
	elseif nLoai == 12 then
		AddContribution(v)
		AddWeeklyOffer(v)
		AddWeekGoalOffer(v)
		s = format("c¸ nh©n +%d cèng hiÕn, tuÇn, môc tiªu tuÇn (®äc l¹i %d / %d / %d)", v, GetContribution(), GetWeeklyOffer(), GetWeekGoalOffer())
		szVe = "BH_KinhTe4"
	end
	Msg2Player("§· göi: " .. s .. ".")
	if szVe == "BH_KinhTe2" then
		BH_KinhTe2()
	elseif szVe == "BH_KinhTe3" then
		BH_KinhTe3()
	elseif szVe == "BH_KinhTe4" then
		BH_KinhTe4()
	else
		BH_TVMenu2()
	end
end

-- ---------------------------------------------------------------- 4. muc tieu tuan
function BH_MucTieu()
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	Msg2Player(format("<color=yellow>[Môc tiªu tuÇn]<color> %s: cÊp chän %d - sù kiÖn %d - cÊp %d - bang cÇn %d, ®· %d - c¸ nh©n cÇn %d - th­ëng bang %d, c¸ nh©n %d.",
		TONG_GetName(nT), TONG_GetCurWeekGoalLevel(nT), TONG_GetWeekGoalEvent(nT), TONG_GetWeekGoalLevel(nT), TONG_GetWeekGoalTotal(nT),
		TONG_GetWeekGoalValue(nT), TONG_GetWeekGoalPlayer(nT), TONG_GetWeekGoalPriceTong(nT), TONG_GetWeekGoalPricePlayer(nT)))
	Msg2Player(format("TuÇn tr­íc: sù kiÖn %d - cÊp %d - cÇn %d, ®¹t %d - c¸ nh©n cÇn %d - th­ëng bang %d, c¸ nh©n %d. T«i gãp %d ®iÓm.",
		TONG_GetLWeekGoalEvent(nT), TONG_GetLWeekGoalLevel(nT), TONG_GetLWeekGoalTotal(nT), TONG_GetLWeekGoalValue(nT), TONG_GetLWeekGoalPlayer(nT),
		TONG_GetLWeekGoalPriceTong(nT), TONG_GetLWeekGoalPricePlayer(nT), GetWeekGoalOffer()))
	SayEx({format("Môc tiªu tuÇn bang <color=green>%s<color> ®· in ra khung chat (cöa sæ bang: tab NhËt ký).", TONG_GetName(nT)),
	"Xem l¹i/BH_MucTieu",
	"§Æt cÊp môc tiªu 1, 3, 5/BH_MTCapMenu",
	"§Æt môc tiªu mÉu: sù kiÖn 1, bang 10.000, c¸ nh©n 500/BH_MTDat",
	"§iÓm: céng, hoµn thµnh, c¸ nh©n, chèt tuÇn/BH_MTDiemMenu",
	"Quay l¹i/BH_TestRoot",
	"Tho¸t/no"})
end

function BH_MTCapMenu()
	SayEx({"CÊp môc tiªu tuÇn (TONG_ApplySetCurWeekGoalLevel + SetWeekGoalLevel).",
	"CÊp 1/#BH_MTCap(1)",
	"CÊp 3/#BH_MTCap(3)",
	"CÊp 5/#BH_MTCap(5)",
	"Quay l¹i/BH_MucTieu",
	"Tho¸t/no"})
end

function BH_MTDiemMenu()
	SayEx({"§iÓm môc tiªu tuÇn.",
	"Bang +5.000 ®iÓm (TONG_ApplyAddWeekGoalValue)/#BH_MTDiem(5000)",
	"Bang = tæng cÇn (hoµn thµnh ngay)/#BH_MTDiem(-1)",
	"C¸ nh©n t«i +500 (AddWeekGoalOffer)/#BH_MTCaNhan(500)",
	"Chèt tuÇn (TONG_ApplyWeeklyMaintain)/#BH_BaoTri(2)",
	"Quay l¹i/BH_MucTieu",
	"Tho¸t/no"})
end

function BH_MTCap(lv)
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot")
		return
	end
	TONG_ApplySetCurWeekGoalLevel(nT, lv)
	TONG_ApplySetWeekGoalLevel(nT, lv)
	Msg2Player(format("§· göi cÊp môc tiªu tuÇn = %d.", lv))
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
	Msg2Player("§· göi môc tiªu mÉu: sù kiÖn 1, bang cÇn 10000, c¸ nh©n cÇn 500, th­ëng bang 100, c¸ nh©n 10.")
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
		Msg2Player(format("§· göi ®iÓm bang = tæng cÇn %d.", TONG_GetWeekGoalTotal(nT)))
	else
		TONG_ApplyAddWeekGoalValue(nT, v)
		Msg2Player(format("§· göi céng %d ®iÓm bang.", v))
	end
	BH_MTDiemMenu()
end

function BH_MTCaNhan(v)
	AddWeekGoalOffer(v)
	Msg2Player(format("§iÓm môc tiªu tuÇn c¸ nh©n +%d (®äc l¹i %d).", v, GetWeekGoalOffer()))
	BH_MTDiemMenu()
end

-- ---------------------------------------------------------------- 5. nhat ky, cong cao, tuyet ky
function BH_GhiChep()
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot2")
		return
	end
	SayEx({format("Bang %s - c«ng c¸o: %s", TONG_GetName(nT), tostring(TONG_GetAnnouncement(nT))),
	"Ghi 1 dßng sù kiÖn (TONG_ApplyAddEventRecord)/#BH_GhiDong(1)",
	"Ghi 1 dßng lÞch sö (TONG_ApplyAddHistoryRecord)/#BH_GhiDong(2)",
	"C«ng c¸o, tuyÖt kü/BH_GhiChep2",
	"Ngo¹i h×nh, t¹m ng­ng, 20 dßng, log/BH_GhiChep3",
	"Quay l¹i/BH_TestRoot2",
	"Tho¸t/no"})
end

function BH_GhiChep2()
	SayEx({"C«ng c¸o (TONG_ApplySetAnnouncement) vµ tuyÖt kü (TONG_ApplySetStunt).",
	"§Æt c«ng c¸o test/#BH_CongCao(1)",
	"Xo¸ c«ng c¸o/#BH_CongCao(0)",
	"TuyÖt kü = 1/#BH_TuyetKy(1)",
	"TuyÖt kü = 0/#BH_TuyetKy(0)",
	"Quay l¹i/BH_GhiChep",
	"Tho¸t/no"})
end

function BH_GhiChep3()
	SayEx({"Ngo¹i h×nh toµn bang, t¹m ng­ng t¸c ph­êng, nhËt ký.",
	"§æi ngo¹i h×nh toµn bang 60 gi©y (ChangeAllMemberFeature 1)/#BH_NgoaiHinh(1)",
	"T¹m ng­ng t¸c ph­êng bËt/#BH_TamNgung(1)",
	"T¹m ng­ng t¸c ph­êng t¾t/#BH_TamNgung(0)",
	"Ghi 20 dßng sù kiÖn + 1 dßng log tÖp/#BH_GhiDong(3)",
	"Quay l¹i/BH_GhiChep",
	"Tho¸t/no"})
end

function BH_GhiDong(nLoai)
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot2")
		return
	end
	local sz = format("[Test %s] %s ghi thö lóc %s", GetName(), TONG_GetName(nT), FormatTime2String(GetCurrentTime()))
	if nLoai == 1 then
		TONG_ApplyAddEventRecord(nT, sz)
		Msg2Player("§· göi 1 dßng sù kiÖn: " .. sz)
		BH_GhiChep()
	elseif nLoai == 2 then
		TONG_ApplyAddHistoryRecord(nT, sz)
		Msg2Player("§· göi 1 dßng lÞch sö: " .. sz)
		BH_GhiChep()
	else
		for i = 1, 20 do
			TONG_ApplyAddEventRecord(nT, format("[Test %d/20] dßng sù kiÖn dµi trung b×nh ®Ó thö cuén trang nhËt ký bang héi", i))
		end
		TONG_WriteLog(sz)
		Msg2Player("§· göi 20 dßng sù kiÖn vµ ghi log: " .. sz)
		BH_GhiChep3()
	end
end

function BH_CongCao(nOn)
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot2")
		return
	end
	if nOn == 1 then
		TONG_ApplySetAnnouncement(nT, format("C«ng c¸o test cña %s lóc %s. Chµo mõng thµnh viªn míi!", GetName(), FormatTime2String(GetCurrentTime())))
	else
		TONG_ApplySetAnnouncement(nT, "")
	end
	Msg2Player(format("§· göi c«ng c¸o (%d).", nOn))
	BH_GhiChep2()
end

function BH_TuyetKy(n)
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot2")
		return
	end
	TONG_ApplySetStunt(nT, n)
	Msg2Player(format("§· göi tuyÖt kü = %d.", n))
	BH_GhiChep2()
end

function BH_NgoaiHinh(n)
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot2")
		return
	end
	local r = TONG_ChangeAllMemberFeature(nT, n, 60)
	Msg2Player(format("TONG_ChangeAllMemberFeature(%d, 60 gi©y) tr¶ vÒ %s.", n, tostring(r)))
	BH_GhiChep3()
end

function BH_TamNgung(n)
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot2")
		return
	end
	TONG_ApplySetPauseState(nT, n)
	Msg2Player(format("§· göi t¹m ng­ng t¸c ph­êng = %d.", n))
	BH_GhiChep3()
end

-- ---------------------------------------------------------------- 6. lanh dia + tac phuong
function BH_LanhDia()
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot2")
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
	SayEx({format("L·nh ®Þa vµ t¸c ph­êng bang %s ®· in ra khung chat. Bé TLD_ vµ TPT_ cã nót Trë vÒ cña bé cò.", TONG_GetName(nT)),
	"L·nh ®Þa: xem, chän khu, t¹o, cÊp, cÊm ®Þa, xo¸ (bé TLD_)/TLD_Menu",
	"T¸c ph­êng 7 khu: lËp, n©ng, më-®ãng, s¶n xuÊt (bé TPT_)/TPT_Menu",
	"Vµo l·nh ®Þa bang ngay/TLD_Go",
	"B¶o tr× t¸c ph­êng toµn server (TWS_MaintainAll)/BH_TWSMaintain",
	"Quay l¹i/BH_TestRoot2",
	"Tho¸t/no"})
end

function BH_TWSMaintain()
	local r = TWS_MaintainAll()
	Msg2Player(format("TWS_MaintainAll tr¶ vÒ %s.", tostring(r)))
	BH_LanhDia()
end

-- ---------------------------------------------------------------- 7. bang khac, lien minh
function BH_BangKhac()
	local nT = BH_T()
	Msg2Player(format("<color=yellow>[Bang kh¸c]<color> m¸y chñ cã %d bang (in tèi ®a 12):", TONG_GetTongCount()))
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
		Msg2Player(format("%d. %s (id %d) - bang chñ %s - %d ng­êi - cÊp %d - ®¬n chê %d%s%s",
			n, TONG_GetName(id), id, TONG_GetMaster(id), TONG_GetMemberCount(id, -1), TONG_GetExpLevel(id), TONG_GetApplyCount(id), szLM, szMe))
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
	SayEx({format("%s LËp, xin, duyÖt, rêi liªn minh vµ duyÖt ®¬n chØ cã ë cöa sæ bang (tab Chøc n¨ng).", szLM),
	"Xem l¹i danh s¸ch bang/BH_BangKhac",
	"Xin vµo mét bang (TONG_ApplyJoin)/BH_XinVao",
	"T×m bang theo tªn t«i (TONG_GetTongByRoleName)/BH_TimBang",
	"Quay l¹i/BH_TestRoot2",
	"Tho¸t/no"})
end

function BH_XinVao()
	local nT = BH_T()
	if nT ~= 0 then
		Msg2Player(format("B¹n ®ang ë bang %s, ph¶i rêi bang tr­íc (môc 11).", TONG_GetName(nT)))
		BH_BangKhac()
		return
	end
	local t = {"Chän bang muèn xin vµo (4 bang ®Çu):"}
	local id = TONG_GetFirstTong()
	local n = 0
	while id ~= nil and id ~= 0 and n < 4 do
		n = n + 1
		t[getn(t) + 1] = format("%s - %d ng­êi/#BH_XinVaoGo(%d)", TONG_GetName(id), TONG_GetMemberCount(id, -1), n)
		id = TONG_GetNextTong(id)
	end
	t[getn(t) + 1] = "Quay l¹i/BH_BangKhac"
	t[getn(t) + 1] = "Tho¸t/no"
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
		Msg2Player(format("TONG_ApplyJoin(%s) tr¶ vÒ %d (2 vµo th¼ng, 1 chê duyÖt, -1 d­íi cÊp, 0 thÊt b¹i).", TONG_GetName(id), r))
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

-- ---------------------------------------------------------------- 8. CONG THANH + THAI THU
function BH_CTDong(i)
	local o, m = GetCityOwner(i)
	if o == nil or o == "" then
		o = "(v« chñ)"
	end
	if m == nil or m == "" then
		m = "-"
	end
	local s = format("%d. %s: chñ %s, Th¸i thó %s, thuÕ %d", i, GetCityAreaName(i), o, m, CTC_JX2_GetTax(i))
	if HaveBeginWar(i) == 1 then
		s = s .. " <color=red>[®ang chiÕn]<color>"
	end
	if IsSigningUp(i) == 1 then
		s = s .. " <color=yellow>[®ang b¸o danh]<color>"
	end
	return s
end

function BH_CongThanh()
	Msg2Player("<color=yellow>[C«ng thµnh]<color> 7 thµnh:")
	for i = 1, 7 do
		Msg2Player(BH_CTDong(i))
	end
	local n = BH_Thanh()
	SayEx({format("7 thµnh ®· in ra khung chat. Thµnh ®ang chän: <color=green>%d %s<color>.", n, GetCityAreaName(n)),
	"Chän thµnh/BH_CTChon",
	"Thao t¸c thµnh ®ang chän: Th¸i thó, khiªu chiÕn, tr¹ng th¸i/BH_CTMenu",
	"Ðp pha 18h-19h-20h-0h/BH_CTPha",
	"KÕt trËn, thuÕ, danh hiÖu Th¸i thó/BH_CTMenu2",
	"Quay l¹i/BH_TestRoot2",
	"Tho¸t/no"})
end

function BH_CTChon()
	SayEx({"Chän thµnh (1-4). Thµnh 5-7 ë trang 2.",
	"1 "..GetCityAreaName(1).."/#BH_CTChonGo(1)",
	"2 "..GetCityAreaName(2).."/#BH_CTChonGo(2)",
	"3 "..GetCityAreaName(3).."/#BH_CTChonGo(3)",
	"4 "..GetCityAreaName(4).."/#BH_CTChonGo(4)",
	"Trang 2/BH_CTChon2",
	"Quay l¹i/BH_CongThanh"})
end

function BH_CTChon2()
	SayEx({"Chän thµnh (5-7).",
	"5 "..GetCityAreaName(5).."/#BH_CTChonGo(5)",
	"6 "..GetCityAreaName(6).."/#BH_CTChonGo(6)",
	"7 "..GetCityAreaName(7).."/#BH_CTChonGo(7)",
	"Thµnh ®ang ®øng (GetCityArea)/#BH_CTChonGo(0)",
	"Trang 1/BH_CTChon",
	"Quay l¹i/BH_CongThanh"})
end

function BH_CTChonGo(n)
	if n == 0 then
		n = GetCityArea()
		if n == nil or n < 1 or n > 7 then
			Msg2Player("B¹n ®ang ë ngoµi 7 thµnh.")
			BH_CongThanh()
			return
		end
	end
	BH_CITY[GetName()] = n
	Msg2Player(format("§· chän thµnh %d %s.", n, GetCityAreaName(n)))
	BH_CongThanh()
end

function BH_CTMenu()
	local n = BH_Thanh()
	Msg2Player(BH_CTDong(n))
	Msg2Player(tostring(GetCitySummary(n)))
	SayEx({format("Thµnh <color=green>%d %s<color>: chñ %s. Bang t«i: %s.", n, GetCityAreaName(n), GetCityOwner(n), BH_TenBang()),
	"Bang t«i lµm chñ thµnh, Th¸i thó = bang chñ (AppointViceroy)/BH_CTChuHoi",
	"Bang t«i lµm khiªu chiÕn gi¶ (AppointChallenger)/BH_CTKhieuHoi",
	"Tr¹ng th¸i thµnh: 0 th­êng, 1 cã khiªu chiÕn, 2 ®ang ®¸nh/BH_CTTrangThai",
	"Bang b¸o danh thµnh nµy (NumOfSignUpTongs)/BH_CTBaoDanh",
	"Quay l¹i/BH_CongThanh",
	"Tho¸t/no"})
end

function BH_CTChuHoi()
	local n = BH_Thanh()
	if BH_T() == 0 then
		BH_ChuaVaoBang("BH_CTMenu")
		return
	end
	SayEx({format("§æi chñ thµnh %s thµnh bang %s? Chñ cò [%s] bÞ thu danh hiÖu Th¸i thó, bang t«i nhËn ngµy chiÕm thµnh.", GetCityAreaName(n), BH_TenBang(), GetCityOwner(n)),
	"§ång ý ®æi chñ/BH_CTChuGo",
	"Th«i/BH_CTMenu"})
end

function BH_CTChuGo()
	local n = BH_Thanh()
	local szTong = BH_TenBang()
	if szTong ~= "" then
		AppointViceroy(GetCityAreaName(n), szTong)
		local o, m = GetCityOwner(n)
		Msg2Player(format("§· bæ nhiÖm bang [%s] lµm chñ %s. Th¸i thó: %s. BÊm Xem 7 thµnh ®Ó kiÓm.", szTong, GetCityAreaName(n), tostring(m)))
	end
	BH_CTMenu()
end

function BH_CTKhieuHoi()
	local n = BH_Thanh()
	if BH_T() == 0 then
		BH_ChuaVaoBang("BH_CTMenu")
		return
	end
	SayEx({format("§Æt bang %s lµm khiªu chiÕn gi¶ cña %s? Thµnh chuyÓn sang tr¹ng th¸i cã khiªu chiÕn.", BH_TenBang(), GetCityAreaName(n)),
	"§ång ý khiªu chiÕn/BH_CTKhieuGo",
	"Th«i/BH_CTMenu"})
end

function BH_CTKhieuGo()
	local n = BH_Thanh()
	local szTong = BH_TenBang()
	if szTong ~= "" then
		AppointChallenger(GetCityAreaName(n), szTong)
		Msg2Player(format("§· ®Æt bang [%s] lµm khiªu chiÕn gi¶ %s. HaveBeginWar = %d.", szTong, GetCityAreaName(n), HaveBeginWar(n)))
	end
	BH_CTMenu()
end

function BH_CTTrangThai()
	local n = BH_Thanh()
	SayEx({format("Ðp tr¹ng th¸i thµnh %s (ghi th¼ng d÷ liÖu thËt). HaveBeginWar = %d.", GetCityAreaName(n), HaveBeginWar(n)),
	"0 b×nh th­êng/#BH_CTSt(0)",
	"1 ®· cã khiªu chiÕn/#BH_CTSt(1)",
	"2 ®ang ®¸nh/#BH_CTSt(2)",
	"Quay l¹i/BH_CTMenu",
	"Tho¸t/no"})
end

function BH_CTSt(s)
	local n = BH_Thanh()
	CTC_JX2_SetCityState(n, s)
	Msg2Player(format("§· ®Æt tr¹ng th¸i %d cho %s. HaveBeginWar = %d.", s, GetCityAreaName(n), HaveBeginWar(n)))
	BH_CTTrangThai()
end

function BH_CTBaoDanh()
	local n = BH_Thanh()
	local nCnt = NumOfSignUpTongs(n)
	Msg2Player(format("%s - ®ang b¸o danh %d - sè bang b¸o danh: %d", GetCityAreaName(n), IsSigningUp(n), nCnt))
	local i = 0
	while i < nCnt and i < 16 do
		Msg2Player(format("- %s", tostring(GetSignUpTongName(n, i))))
		i = i + 1
	end
	BH_CTMenu()
end

function BH_CTPha()
	local n = BH_Thanh()
	SayEx({format("Ðp pha cho %s. C¶ 4 pha ®Òu ch¹m tíi c¶ 7 thµnh; ®ång hå 5 pha kh«ng tù ch¹y, chØ Ðp tay.", GetCityAreaName(n)),
	"18h më b¸o danh (xo¸ b¶ng ®Êu gi¸ 7 thµnh)/#BH_CTPhaHoi(18)",
	"19h chèt khiªu chiÕn (h¹ cê b¸o danh 6 thµnh kia)/#BH_CTPhaHoi(19)",
	"20h khai chiÕn (më chiÕn tr­êng 5 phót)/#BH_CTPhaHoi(20)",
	"0h dän ngµy (gì league 508-509 cña 7 thµnh)/#BH_CTPhaHoi(0)",
	"Quay l¹i/BH_CongThanh",
	"Tho¸t/no"})
end

function BH_CTPhaHoi(h)
	SayEx({format("Ðp pha %dh cho thµnh %s? Ghi th¼ng d÷ liÖu thËt.", h, GetCityAreaName(BH_Thanh())),
	"§ång ý/#BH_CTPhaGo("..h..")",
	"Th«i/BH_CTPha"})
end

function BH_CTPhaGo(h)
	local n = BH_Thanh()
	if h == 18 then
		g_CTC6_D18 = nil
		CTC_JX2_Tick(TB_CTC6[n][1], 18, 0)
		g_CTC6_D18 = nil
		Msg2Player(format("§· Ðp 18h. IsSigningUp = %d.", IsSigningUp(n)))
	elseif h == 19 then
		g_CTC6_D19 = nil
		CTC_JX2_Tick(TB_CTC6[n][1], 19, 0)
		g_CTC6_D19 = nil
		Msg2Player(format("§· Ðp 19h. Chñ thµnh: %s.", GetCityOwner(n)))
	elseif h == 20 then
		g_CTC6_D20 = nil
		CTC_JX2_Tick(TB_CTC6[n][2], 20, 0)
		g_CTC6_D20 = nil
		Msg2Player(format("§· Ðp 20h. HaveBeginWar = %d.", HaveBeginWar(n)))
	else
		g_CTC6_D0 = nil
		CTC_JX2_Tick(0, 0, 0)
		g_CTC6_D0 = nil
		Msg2Player("§· Ðp 0h dän ngµy.")
	end
	BH_CTPha()
end

function BH_CTMenu2()
	local n = BH_Thanh()
	local cong, thu = GetCityWarBothSides(n)
	SayEx({format("Thµnh <color=green>%d %s<color>: c«ng %s - thñ %s - thuÕ %d.", n, GetCityAreaName(n), tostring(cong), tostring(thu), CTC_JX2_GetTax(n)),
	"KÕt trËn: C¤NG th¾ng (®æi chñ + danh hiÖu)/#BH_CTWinHoi(1)",
	"KÕt trËn: thñ th¾ng (gi÷ chñ)/#BH_CTWinHoi(0)",
	"ThuÕ thµnh: ®Æt 0, 5, 15, 20/BH_CTThue",
	"Danh hiÖu Th¸i thó cña t«i: cÊp, gì, xem/BH_CTDanhHieu",
	"Quay l¹i/BH_CongThanh",
	"Tho¸t/no"})
end

function BH_CTWinHoi(w)
	local n = BH_Thanh()
	SayEx({format("KÕt trËn thËt thµnh %s (®æi chñ, cÊp danh hiÖu Th¸i thó cho bªn th¾ng, thu cña chñ cò)?", GetCityAreaName(n)),
	"§ång ý kÕt trËn/#BH_CTWinGo("..w..")",
	"Th«i/BH_CTMenu2"})
end

function BH_CTWinGo(w)
	local n = BH_Thanh()
	NotifyWarResult(n, w)
	local o, m = GetCityOwner(n)
	Msg2Player(format("§· kÕt trËn %s: chñ %s, Th¸i thó %s.", GetCityAreaName(n), tostring(o), tostring(m)))
	BH_CTMenu2()
end

function BH_CTThue()
	local n = BH_Thanh()
	local o, m = GetCityOwner(n)
	SayEx({format("ThuÕ %s ®ang %d. Chñ %s, Th¸i thó %s. §Æt ®­îc 22h-23h, 1 lÇn mçi ngµy, trÇn 20, ph¶i lµ Th¸i thó.", GetCityAreaName(n), CTC_JX2_GetTax(n), tostring(o), tostring(m)),
	"ThuÕ 0/#BH_CTThueGo(0)",
	"ThuÕ 5/#BH_CTThueGo(5)",
	"ThuÕ 15/#BH_CTThueGo(15)",
	"ThuÕ 20/#BH_CTThueGo(20)",
	"Quay l¹i/BH_CTMenu2",
	"Tho¸t/no"})
end

function BH_CTThueGo(v)
	local n = BH_Thanh()
	local nRet = CTC_JX2_SetTax(n, v)
	local s = format("CTC_JX2_SetTax(%d, %d) tr¶ vÒ %d: ", n, v, nRet)
	if nRet == 0 then
		s = s .. format("OK, thuÕ = %d", CTC_JX2_GetTax(n))
	elseif nRet == 1 then
		s = s .. "b¹n kh«ng ph¶i Th¸i thó thµnh nµy"
	elseif nRet == 2 then
		s = s .. "ngoµi khung giê 22h-23h"
	elseif nRet == 3 then
		s = s .. "h«m nay ®· ®Æt råi"
	else
		s = s .. "tham sè sai (thuÕ v­ît trÇn 20?)"
	end
	Msg2Player(s)
	BH_CTThue()
end

function BH_CTDanhHieu()
	local n = BH_Thanh()
	local nCur = GetPlayerTitle()
	if nCur == nil then
		nCur = 0
	end
	SayEx({format("Danh hiÖu Th¸i thó thµnh %s = %d (JX2), trªn ®Çu JX1 = %d. §ang bËt JX2: %d.", GetCityAreaName(n), 152 + n, 167 + n, Title_GetActiveTitle()),
	"CÊp + bËt danh hiÖu Th¸i thó thµnh nµy cho t«i/#BH_CTDHGo(1)",
	"Gì danh hiÖu Th¸i thó thµnh nµy/#BH_CTDHGo(0)",
	"Xem danh hiÖu cña t«i (Title_GetTitleTab)/#BH_CTDHGo(2)",
	"T¾t danh hiÖu ®ang bËt/#BH_CTDHGo(3)",
	"Quay l¹i/BH_CTMenu2",
	"Tho¸t/no"})
end

function BH_CTDHGo(nLoai)
	local n = BH_Thanh()
	local nId = 152 + n
	if nLoai == 1 then
		Title_AddTitle(nId, 0, 9999999)
		Title_ActiveTitle(nId)
		SetTask(1122, nId)
		SetPlayerTitle(167 + n, 567648000, 0)
		Msg2Player(format("§· cÊp vµ bËt %d %s (JX1 trªn ®Çu %d).", nId, tostring(Title_GetTitleName(nId)), 167 + n))
	elseif nLoai == 0 then
		Title_RemoveTitle(nId)
		local nCur = GetPlayerTitle()
		if nCur ~= nil and nCur >= 168 and nCur <= 174 then
			RemovePlayerTitle()
		end
		Msg2Player(format("§· gì danh hiÖu %d.", nId))
	elseif nLoai == 2 then
		local tab = Title_GetTitleTab()
		if tab == nil or getn(tab) == 0 then
			Msg2Player("Ch­a së h÷u danh hiÖu nµo.")
		else
			local s = format("§ang bËt %d. Së h÷u %d: ", Title_GetActiveTitle(), getn(tab))
			for i = 1, getn(tab) do
				s = s .. format("%d %s; ", tab[i], tostring(Title_GetTitleName(tab[i])))
			end
			Msg2Player(s)
		end
	else
		Title_ActiveTitle(0)
		SetTask(1122, 0)
		local nCur = GetPlayerTitle()
		if nCur ~= nil and nCur >= 168 and nCur <= 174 then
			RemovePlayerTitle()
		end
		Msg2Player("§· t¾t danh hiÖu ®ang bËt.")
	end
	BH_CTDanhHieu()
end

-- ---------------------------------------------------------------- 9. hoat dong bang
function BH_HoatDong()
	local nT = BH_T()
	local sz = "B¹n ch­a vµo bang."
	if nT ~= 0 then
		sz = format("Bang %s: tr¹ng th¸i chiÕn %d, ngµy chiÕm thµnh %d, chøc ®¹i thÇn cña t«i %d.", TONG_GetName(nT), TONG_GetWarState(nT), TONG_GetOccupyCityDay(nT), GetTongDuty())
	end
	SayEx({sz,
	"VËn tiªu bang héi 10 sao (bé VT_ADM)/VT_ADM_Bang",
	"Bang chiÕn - B¸ch Nh©n - Tû Vâ - Thµnh B¶o (bé HD_)/HD_AdminMenu",
	"Danh hiÖu + thuÕ Th¸i thó (bé TDT_)/TDT_Menu",
	"Tuyªn chiÕn (TongClaimWar)/BH_TuyenChien",
	"Quay l¹i/BH_TestRoot3",
	"Tho¸t/no"})
end

function BH_TuyenChien()
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot3")
		return
	end
	local r = TongClaimWar(nT)
	Msg2Player(format("TongClaimWar(%s) tr¶ vÒ %s; tr¹ng th¸i chiÕn %d.", TONG_GetName(nT), tostring(r), TONG_GetWarState(nT)))
	BH_HoatDong()
end

-- ---------------------------------------------------------------- 10. cap bang + kinh nghiem (BHLV)
function BH_CapBang()
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot3")
		return
	end
	local nExp = TONG_GetExp(nT)
	local nLv = TONG_GetExpLevel(nT)
	local nJX1 = GetTongLevel()
	if nJX1 == nil then
		nJX1 = 0
	end
	Msg2Player(format("<color=yellow>[CÊp bang]<color> %s: kinh nghiÖm bang (field 6) %d -> cÊp theo kinh nghiÖm %d; cÊp JX1 ®ang ®ång bé %d; kinh nghiÖm JX1 (boss) %d.",
		TONG_GetName(nT), nExp, nLv, nJX1, GetTongExp()))
	Msg2Player(format("CÇn: cÊp %d = %d, cÊp %d = %d, cÊp %d = %d (b¶ng [LevelExp] tong_setting.ini, mÆc ®Þnh n*n*1000). Quü %d: relay ®æi quü thµnh kinh nghiÖm mçi 750 gi©y khi quü v­ît 1.000.000 (5.000 l­îng -> 120 kinh nghiÖm).",
		nLv + 1, TONG_GetLevelExpNeed(nLv + 1), nLv + 2, TONG_GetLevelExpNeed(nLv + 2), nLv + 3, TONG_GetLevelExpNeed(nLv + 3), TONG_GetMoney(nT)))
	SayEx({format("Bang %s: kinh nghiÖm %d, cÊp %d (JX1 %d). CÊp chØ lªn, kh«ng xuèng; relay b¸o vµo kªnh bang khi lªn cÊp.", TONG_GetName(nT), nExp, nLv, nJX1),
	"Xem l¹i/BH_CapBang",
	"Kinh nghiÖm bang +1.000/#BH_Exp(1000)",
	"Kinh nghiÖm bang +50.000/#BH_Exp(50000)",
	"Quü +2.000.000 (®îi 750 gi©y xem tù céng kinh nghiÖm)/#BH_ExpQuy(2000000)",
	"Quay l¹i/BH_TestRoot3",
	"Tho¸t/no"})
end

function BH_Exp(v)
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot3")
		return
	end
	TONG_ApplyAddTaskValue(nT, 6, v)
	Msg2Player(format("§· göi kinh nghiÖm bang +%d (relay tÝnh l¹i cÊp ngay, ®äc l¹i sau vµi gi©y).", v))
	BH_CapBang()
end

function BH_ExpQuy(v)
	local nT = BH_T()
	if nT == 0 then
		BH_ChuaVaoBang("BH_TestRoot3")
		return
	end
	TONG_ApplyAddMoney(nT, v)
	Msg2Player(format("§· göi quü +%d (®äc l¹i %d).", v, TONG_GetMoney(nT)))
	BH_CapBang()
end

-- ---------------------------------------------------------------- 11. tien ich
function BH_TienIch()
	local nT = BH_T()
	local sz = "B¹n ch­a vµo bang."
	if nT ~= 0 then
		sz = format("B¹n ë bang %s (id thµnh viªn %d, %s, bang chñ = %d).", TONG_GetName(nT), GetTongMemberID(), BH_Fig(GetTongFigure()), CheckTongMasterPower())
	end
	SayEx({sz,
	"Vµo bang test TESTGAME hoÆc bang ®Çu tiªn (TONG_ApplyJoin)/BH_VaoBangTest",
	"Rêi bang (TONG_ApplyDeleteMember) - hái tr­íc/BH_RoiBangHoi",
	"Khëi t¹o l¹i d÷ liÖu bang (TONG_ApplyInit) - hái tr­íc/BH_InitHoi",
	"B¶o tr× kinh tÕ ngµy toµn server (DailyMaintainAll) - hái tr­íc/BH_BaoTriAllHoi",
	"Trang 2: n¹p l¹i, bé cò, reset 7 thµnh/BH_TienIch2",
	"Quay l¹i/BH_TestRoot3"})
end

function BH_TienIch2()
	SayEx({"TiÖn Ých trang 2.",
	"N¹p l¹i file test (dofile)/BH_NapLai",
	"Bé test cò: c«ng thµnh, l·nh ®Þa, t¸c ph­êng, danh hiÖu, thuÕ (TX_Root)/TX_Root",
	"Reset 7 thµnh vÒ v« chñ - hái tr­íc/BH_CTResetHoi",
	"VÒ trang 1/BH_TienIch",
	"Tho¸t/no"})
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
		BH_ChuaVaoBang("BH_TestRoot3")
		return
	end
	SayEx({format("<color=red>Rêi bang<color> %s? Bang chñ kh«ng rêi ®­îc (chuyÓn vÞ tr­íc ë cöa sæ bang).", TONG_GetName(nT)),
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
		BH_ChuaVaoBang("BH_TestRoot3")
		return
	end
	SayEx({format("<color=red>Khëi t¹o l¹i<color> d÷ liÖu bang %s (relay ®Æt l¹i field bang vÒ mÆc ®Þnh)? ChØ lµm víi bang test.", TONG_GetName(nT)),
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

function BH_BaoTriAllHoi()
	SayEx({"<color=red>Toµn server<color>: b¶o tr× kinh tÕ ngµy thËt cho mäi bang (trõ chiÕn bÞ, t¹m ng­ng t¸c ph­êng, chèt môc tiªu). ChØ lµm trªn m¸y chñ test.",
	"§ång ý, ch¹y TONG_DailyMaintainAll(2)/BH_BaoTriAll",
	"Th«i/BH_TienIch"})
end

function BH_BaoTriAll()
	local n = TONG_DailyMaintainAll(2)
	Msg2Player(format("TONG_DailyMaintainAll(2) tr¶ vÒ %s.", tostring(n)))
	BH_TienIch()
end

function BH_CTResetHoi()
	local s = ""
	for i = 1, 7 do
		local o = GetCityOwner(i)
		if o ~= nil and o ~= "" then
			s = s .. format("%d %s; ", i, o)
		end
	end
	if s == "" then
		s = "c¶ 7 thµnh ®ang v« chñ"
	end
	SayEx({format("Xo¸ chñ c¶ 7 thµnh? §ang cã chñ: %s. Kh«ng thu håi danh hiÖu Th¸i thó cña ng­êi kh¸c.", s),
	"§ång ý reset 7 thµnh/BH_CTResetGo",
	"Th«i/BH_TienIch2"})
end

function BH_CTResetGo()
	for i = 1, 7 do
		SetViewTongOwnCity("", i)
		CTC_JX2_SetCityState(i, 0)
	end
	Msg2Player("§· reset 7 thµnh vÒ v« chñ.")
	BH_CongThanh()
end

function BH_NapLai()
	dofile("script/kiemthu/item/test_banghoi_admin.lua")
	Msg2Player("§· n¹p l¹i test_banghoi_admin.lua.")
	BH_TestRoot()
end
