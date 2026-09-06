-- tongjx2_menu.lua - GIAO DIEN BANG HOI KIEU JX2 (ban NPC menu, du chuc nang)
-- CACH DUNG: chep vao bin\server\script\test\tongjx2_menu.lua roi cho mot NPC goi:
--   dofile("script/test/tongjx2_menu.lua")  main()
-- Moi thao tac GHI di qua relay (Apply) -> mo lai menu de thay so moi.
-- Quyen han kiem theo dung bang ID quyen JX2 (BANGHOI_JX2_PHANTICH.md 4.3).

TJXM_TASKID = 1002

-- ==== ho tro ====

function tjx_id()
	local szTong = GetTongName()
	if (szTong == nil or szTong == "") then
		return 0
	end
	return TONG_Name2ID(szTong)
end

function tjx_can(nRight)
	local id = tjx_id()
	if (id == 0) then
		return 0
	end
	return TONGM_CheckRight(id, GetName(), nRight)
end

function tjx_fig()
	local id = tjx_id()
	if (id == 0) then
		return -1
	end
	return TONGM_GetFigure(id, GetName())
end

-- ==== menu chinh ====

function main()
	local id = tjx_id()
	if (id == 0) then
		local msg = "<color=yellow>He bang hoi JX2<color><enter>Ban sao co "..TONG_GetTongCount().." bang.<enter>"
		msg = msg.."<color=red>Ban chua vao bang.<color> Vao bang roi quay lai."
		Say(msg, 0)
		return
	end
	local msg = "<color=yellow>"..TONG_GetName(id).."<color> - cap <color=green>"..TONG_GetBuildLevel(id).."<color>"
	msg = msg.." - KN bang <color=cyan>"..TONG_GetExp(id).."<color><enter>"
	msg = msg.."Ngan quy: <color=cyan>"..TONG_GetMoney(id).."<color> | Thanh vien: "..TONG_GetMemberCount(id).."<enter>"
	local szAnn = TONG_GetAnnouncement(id)
	if (szAnn ~= nil and szAnn ~= "") then
		msg = msg.."Thong bao: <color=green>"..szAnn.."<color><enter>"
	end
	Say(msg, 9,
		"MO CUA SO BANG HOI (JX2)/tjx_openwin",
		"Thong tin bang/tjx_info",
		"Ngan quy - quyen gop/tjx_fund",
		"Cong hien/tjx_offer",
		"Quyen han/tjx_right",
		"Tac phuong/tjx_ws",
		"Tuyet ky bang/tjx_stunt",
		"Thong bao bang/tjx_ann",
		"Quan tri bang/tjx_admin")
end

-- ==== 1. thong tin ====

function tjx_info()
	local id = tjx_id()
	if (id == 0) then return end
	local msg = "<color=yellow>"..TONG_GetName(id).."<color> (id "..id..")<enter>"
	msg = msg.."Cap bang (BuildLevel): "..TONG_GetBuildLevel(id).." | KN bang: "..TONG_GetExp(id).."<enter>"
	msg = msg.."Bang chu: <color=green>"..TONG_GetMaster(id).."<color><enter>"
	msg = msg.."Thanh vien: "..TONG_GetMemberCount(id).." (truong lao "..TONG_GetMemberCount(id, 1)..", doi truong "..TONG_GetMemberCount(id, 2)..", bang chung "..TONG_GetMemberCount(id, 3)..")<enter>"
	msg = msg.."Online (GS nay): "..TONG_GetOnlineCount(id).."<enter>"
	msg = msg.."Ngay hoat dong: "..TONG_GetDay(id).." | Tuan: "..TONG_GetWeek(id).."<enter>"
	msg = msg.."Chuc vu cua ban: "..tjx_fig().." | Cong hien: "..TONGM_GetOffer(id, GetName()).."<enter>"
	msg = msg.."Ban do bang: "..TONG_GetTongMap(id).." | Tuyet ky: "..TONG_GetTaskValue(id, 1101).."<enter>"
	Say(msg, 1, "Quay lai/main")
end

-- ==== 2. ngan quy ====

function tjx_fund()
	local id = tjx_id()
	if (id == 0) then return end
	local msg = "<color=yellow>Ngan quy "..TONG_GetName(id).."<color><enter>"
	msg = msg.."Ngan quy (64-bit): <color=cyan>"..TONG_GetMoney(id).."<color><enter>"
	msg = msg.."Quy kien thiet: "..TONG_GetBuildFund(id).." | Tuan nay: "..TONG_GetWeekBuildFund(id).."/"..TONG_GetWeekBuildUpper(id).."<enter>"
	msg = msg.."Quy chien bi: "..TONG_GetWarBuildFund(id).." | Duy tri/ngay: "..TONG_GetMaintainFund(id).."<enter>"
	msg = msg.."Quy du tru (offer): "..TONG_GetStoredOffer(id).." | Kien thiet du tru: "..TONG_GetStoredBuildFund(id).."<enter>"
	msg = msg.."Tro cap/nguoi: "..TONG_GetPerStandFund(id).." (tong "..TONG_GetStandFund(id)..")<enter>"
	msg = msg.."Tien cua ban: <color=green>"..GetCash().."<color><enter>"
	Say(msg, 5,
		"Quyen 1 van (+1 kien thiet, +1 cong hien)/tjx_don1",
		"Quyen 10 van/tjx_don10",
		"Quyen 100 van/tjx_don100",
		"Kien thiet -> chien bi (100, can quyen quy)/tjx_b2w",
		"Quay lai/main")
end

function tjx_donate(nVan)
	local id = tjx_id()
	if (id == 0) then return end
	local nMoney = nVan * 10000
	if (GetCash() < nMoney) then
		Msg2Player("Khong du tien (can "..nMoney..").")
		return
	end
	if (Pay(nMoney) ~= 1) then
		Msg2Player("Tru tien that bai.")
		return
	end
	TONG_ApplyAddBuildFund(id, nVan)
	TONG_ApplyAddWeekBuildFund(id, nVan)
	TONG_ApplyAddMoney(id, nMoney)
	TONGM_ApplyAddOffer(id, GetName(), nVan)
	TONG_ApplyAddEventRecord(id, GetName().." quyen "..nVan.." van vao quy kien thiet")
	Msg2Player("Da quyen "..nMoney.." (+"..nVan.." kien thiet, +"..nVan.." cong hien). Mo lai menu de thay so moi.")
end

function tjx_don1()   tjx_donate(1)   end
function tjx_don10()  tjx_donate(10)  end
function tjx_don100() tjx_donate(100) end

function tjx_b2w()
	local id = tjx_id()
	if (id == 0) then return end
	if (tjx_can(3001) ~= 1) then
		Msg2Player("Ban khong co quyen thao tac quy (3001).")
		return
	end
	if (TONG_GetBuildFund(id) < 100) then
		Msg2Player("Quy kien thiet khong du 100.")
		return
	end
	TONG_ApplyAddBuildFund(id, -100)
	TONG_ApplyAddWarBuildFund(id, 100)
	TONG_ApplyAddEventRecord(id, GetName().." chuyen 100 kien thiet sang chien bi")
	Msg2Player("Da chuyen 100 kien thiet -> chien bi.")
end

-- ==== 3. cong hien ====

function tjx_offer()
	local id = tjx_id()
	if (id == 0) then return end
	local msg = "<color=yellow>Cong hien<color><enter>"
	msg = msg.."Cua ban (tich luy): <color=cyan>"..TONGM_GetOffer(id, GetName()).."<color><enter>"
	msg = msg.."Muc tieu tuan nay: "..TONGM_GetTaskValue(id, GetName(), 9).." | Tuan truoc: "..TONGM_GetLWeekGoalOffer(id, GetName()).."<enter>"
	msg = msg.."Quy du tru cua bang: "..TONG_GetStoredOffer(id).."<enter>"
	Say(msg, 4,
		"Nop 10 cong hien vao quy du tru/tjx_off2store",
		"Phat 5 cong hien cho moi truong lao (bang chu)/tjx_dist_elder",
		"Phat 10 cong hien cho ban (tu quy, bang chu)/tjx_dist_me",
		"Quay lai/main")
end

function tjx_off2store()
	local id = tjx_id()
	if (id == 0) then return end
	if (TONGM_GetOffer(id, GetName()) < 10) then
		Msg2Player("Ban khong du 10 cong hien.")
		return
	end
	TONGM_ApplyAddOffer(id, GetName(), -10)
	TONG_ContributeOffer(id, GetName(), 10)
	Msg2Player("Da nop 10 cong hien vao quy du tru.")
end

function tjx_dist_elder()
	local id = tjx_id()
	if (id == 0) then return end
	if (tjx_fig() ~= 0) then
		Msg2Player("Chi bang chu duoc phat cong hien.")
		return
	end
	if (TONG_DistributeOfferToGroup(id, 1, 5) == 1) then
		Msg2Player("Da gui phat 5 cong hien cho moi truong lao (tru tu quy du tru).")
	else
		Msg2Player("Gui that bai.")
	end
end

function tjx_dist_me()
	local id = tjx_id()
	if (id == 0) then return end
	if (tjx_fig() ~= 0) then
		Msg2Player("Chi bang chu duoc phat.")
		return
	end
	TONG_DistributeOfferToMember(id, GetName(), 10)
	Msg2Player("Da gui phat 10 cong hien cho ban (tru tu quy du tru; thieu quy se khong doi).")
end

-- ==== 4. quyen han (bang ID quyen JX2 4.3) ====

function tjx_right()
	local id = tjx_id()
	if (id == 0) then return end
	local me = GetName()
	local msg = "<color=yellow>Quyen han cua ban<color> (figure "..tjx_fig()..")<enter>"
	msg = msg.."1000 giao quyen: "..TONGM_CheckRight(id, me, 1000).." | 1003 doi tran doanh: "..TONGM_CheckRight(id, me, 1003).."<enter>"
	msg = msg.."1901 duoi nguoi: "..TONGM_CheckRight(id, me, 1901).." | 1902 thoai an: "..TONGM_CheckRight(id, me, 1902).."<enter>"
	msg = msg.."2001 nang cap: "..TONGM_CheckRight(id, me, 2001).." | 2004 lanh dia: "..TONGM_CheckRight(id, me, 2004).."<enter>"
	msg = msg.."2005 muc tieu tuan: "..TONGM_CheckRight(id, me, 2005).." | 2006 tuyet ky: "..TONGM_CheckRight(id, me, 2006).."<enter>"
	msg = msg.."2007 tuyen chien: "..TONGM_CheckRight(id, me, 2007).." | 3001 quy: "..TONGM_CheckRight(id, me, 3001).."<enter>"
	msg = msg.."9001 tac phuong: "..TONGM_CheckRight(id, me, 9001).."<enter>"
	msg = msg.."<color=green>Luat JX2: bang chu luon co moi quyen; chi cap duoc cho TRUONG LAO;<enter>ha chuc khoi truong lao la mat sach quyen.<color><enter>"
	Say(msg, 3,
		"Cap 4 quyen thuong dung cho MOI truong lao/tjx_grant_all",
		"Thu quyen tuyet ky cua moi truong lao/tjx_revoke_stunt",
		"Quay lai/main")
end

function tjx_grant_all()
	local id = tjx_id()
	if (id == 0) then return end
	if (tjx_fig() ~= 0 and tjx_can(1000) ~= 1) then
		Msg2Player("Chi bang chu / nguoi co quyen 1000 duoc giao quyen.")
		return
	end
	local nDaCap = 0
	local mid = TONG_GetFirstMember(id, 1)
	while (mid ~= 0) do
		TONGM_ApplyAddRight(id, mid, 1901)
		TONGM_ApplyAddRight(id, mid, 2005)
		TONGM_ApplyAddRight(id, mid, 3001)
		TONGM_ApplyAddRight(id, mid, 9001)
		nDaCap = nDaCap + 1
		mid = TONG_GetNextMember(id, mid, 1)
	end
	Msg2Player("Da gui cap 4 quyen (1901/2005/3001/9001) cho "..nDaCap.." truong lao.")
end

function tjx_revoke_stunt()
	local id = tjx_id()
	if (id == 0) then return end
	if (tjx_fig() ~= 0 and tjx_can(1000) ~= 1) then
		Msg2Player("Chi bang chu / nguoi co quyen 1000 duoc thu quyen.")
		return
	end
	local mid = TONG_GetFirstMember(id, 1)
	local n = 0
	while (mid ~= 0) do
		TONGM_ApplyDelRight(id, mid, 2006)
		n = n + 1
		mid = TONG_GetNextMember(id, mid, 1)
	end
	Msg2Player("Da gui thu quyen 2006 cua "..n.." truong lao.")
end

-- ==== 5. tac phuong ====

TJX_WS_NAME = {"Binh Giap", "Thien Cong", "Mat Na", "Thi Luyen", "Thien Y", "Le Vat", "Hoat Dong"}

function tjx_ws()
	local id = tjx_id()
	if (id == 0) then return end
	local msg = "<color=yellow>Tac phuong ("..TWS_GetWorkshopCount(id).." khu)<color><enter>"
	local t = 1
	while (t <= 7) do
		if (TWS_IsExist(id, t) == 1) then
			local sOpen = "DONG"
			if (TWS_IsOpen(id, t) == 1) then
				sOpen = "MO"
			end
			msg = msg..t..". <color=green>"..TJX_WS_NAME[t].."<color> cap "..TWS_GetLevel(id, t).." ["..sOpen.."] san luong "..TWS_GetDayOutput(id, t).."<enter>"
		else
			msg = msg..t..". "..TJX_WS_NAME[t].." <color=gray>(chua lap)<color><enter>"
		end
		t = t + 1
	end
	Say(msg, 5,
		"Lap khu Binh Giap (quyen 9001)/tjx_ws_add1",
		"Lap khu Thien Cong (quyen 9001)/tjx_ws_add2",
		"Nang cap khu Binh Giap/tjx_ws_up1",
		"Mo - dong khu Binh Giap/tjx_ws_tog1",
		"Quay lai/main")
end

function tjx_ws_guard()
	local id = tjx_id()
	if (id == 0) then
		return 0
	end
	if (tjx_can(9001) ~= 1) then
		Msg2Player("Ban khong co quyen tac phuong (9001).")
		return 0
	end
	return id
end

function tjx_ws_add1()
	local id = tjx_ws_guard()
	if (id == 0) then return end
	if (TWS_ApplyAdd(id, 1) == 1) then
		Msg2Player("Da gui lap khu Binh Giap.")
	else
		Msg2Player("Khong lap duoc (da co?).")
	end
end

function tjx_ws_add2()
	local id = tjx_ws_guard()
	if (id == 0) then return end
	if (TWS_ApplyAdd(id, 2) == 1) then
		Msg2Player("Da gui lap khu Thien Cong.")
	else
		Msg2Player("Khong lap duoc (da co?).")
	end
end

function tjx_ws_up1()
	local id = tjx_ws_guard()
	if (id == 0) then return end
	if (TWS_ApplyUpgrade(id, 1) == 1) then
		Msg2Player("Da gui nang cap khu Binh Giap.")
	else
		Msg2Player("Chua lap khu nay.")
	end
end

function tjx_ws_tog1()
	local id = tjx_ws_guard()
	if (id == 0) then return end
	if (TWS_IsOpen(id, 1) == 1) then
		TWS_ApplyClose(id, 1)
		Msg2Player("Da gui DONG khu Binh Giap.")
	else
		TWS_ApplyOpen(id, 1)
		Msg2Player("Da gui MO khu Binh Giap.")
	end
end

-- ==== 6. tuyet ky ====

TJX_STUNT_NAME = {"Phuong Hoang An", "Luyen Vo Hon", "Vo Song Sat Tran", "Dao Chu Tien", "Vu Than Tu Phu", "(trong)", "Hoang Kim Lenh"}

function tjx_stunt()
	local id = tjx_id()
	if (id == 0) then return end
	local nCur = TONG_GetTaskValue(id, 1101)
	local nOn = TONG_GetTaskValue(id, 1102)
	local msg = "<color=yellow>Tuyet ky bang<color><enter>"
	if (nCur ~= 0 and TJX_STUNT_NAME[nCur] ~= nil) then
		msg = msg.."Dang dat: <color=green>"..TJX_STUNT_NAME[nCur].."<color> (ID "..nCur..", hieu luc "..nOn..")<enter>"
	else
		msg = msg.."Chua dat tuyet ky.<enter>"
	end
	msg = msg.."Bao tri: 6000 chien bi / ngay. Quyen: 2006.<enter>"
	Say(msg, 4,
		"Dat: Phuong Hoang An (1)/tjx_st1",
		"Dat: Vo Song Sat Tran (3)/tjx_st3",
		"Huy tuyet ky/tjx_st0",
		"Quay lai/main")
end

function tjx_setstunt(nID)
	local id = tjx_id()
	if (id == 0) then return end
	if (tjx_can(2006) ~= 1) then
		Msg2Player("Ban khong co quyen tuyet ky (2006).")
		return
	end
	TONG_ApplySetStunt(id, nID)
	Msg2Player("Da gui dat tuyet ky = "..nID..".")
end

function tjx_st1() tjx_setstunt(1) end
function tjx_st3() tjx_setstunt(3) end
function tjx_st0() tjx_setstunt(0) end

-- ==== 7. thong bao ====

function tjx_ann()
	local id = tjx_id()
	if (id == 0) then return end
	local msg = "<color=yellow>Thong bao bang<color><enter>"
	local szAnn = TONG_GetAnnouncement(id)
	if (szAnn == nil or szAnn == "") then
		msg = msg.."(chua co thong bao)<enter>"
	else
		msg = msg.."<color=green>"..szAnn.."<color><enter>"
	end
	Say(msg, 3,
		"Dat thong bao mau (bang chu)/tjx_ann_set",
		"Xoa thong bao/tjx_ann_clear",
		"Quay lai/main")
end

function tjx_ann_set()
	local id = tjx_id()
	if (id == 0) then return end
	if (tjx_fig() ~= 0) then
		Msg2Player("Chi bang chu duoc dat thong bao.")
		return
	end
	TONG_ApplySetAnnouncement(id, "Chao mung den voi "..TONG_GetName(id).."! Online deu de nhan tro cap.")
	Msg2Player("Da gui thong bao. Mo lai menu de xem.")
end

function tjx_ann_clear()
	local id = tjx_id()
	if (id == 0) then return end
	if (tjx_fig() ~= 0) then
		Msg2Player("Chi bang chu duoc xoa thong bao.")
		return
	end
	TONG_ApplySetAnnouncement(id, "")
	Msg2Player("Da gui xoa thong bao.")
end

-- ==== 8. quan tri ====

function tjx_admin()
	local id = tjx_id()
	if (id == 0) then return end
	local msg = "<color=yellow>Quan tri bang<color><enter>"
	msg = msg.."Cap "..TONG_GetBuildLevel(id).." | Quy kien thiet "..TONG_GetBuildFund(id).."<enter>"
	msg = msg.."Len cap can quy + du tac phuong theo bang cap (engine tu kiem).<enter>"
	msg = msg.."Ngay "..TONG_GetDay(id).." | Tuan "..TONG_GetWeek(id).." | Tam dung: "..TONG_GetPauseState(id).."<enter>"
	Say(msg, 6,
		"NANG CAP bang (quyen 2001)/tjx_up_do",
		"Ha cap bang (bang chu)/tjx_down_do",
		"Chay bao tri NGAY ngay bay gio (test)/tjx_mt_day",
		"Chay don TUAN ngay bay gio (test)/tjx_mt_week",
		"Ghi 777 vao TaskValue 1002 (test)/tjx_t777",
		"Quay lai/main")
end

function tjx_up_do()
	local id = tjx_id()
	if (id == 0) then return end
	if (tjx_fig() ~= 0 and tjx_can(2001) ~= 1) then
		Msg2Player("Ban khong co quyen nang cap (2001).")
		return
	end
	TONG_ApplyUpgrade(id)
	Msg2Player("Da gui NANG CAP. Mo lai menu xem cap moi (khong doi = thieu dieu kien).")
end

function tjx_down_do()
	local id = tjx_id()
	if (id == 0) then return end
	if (tjx_fig() ~= 0) then
		Msg2Player("Chi bang chu duoc ha cap.")
		return
	end
	TONG_ApplyDegrade(id)
	Msg2Player("Da gui HA CAP.")
end

function tjx_mt_day()
	local id = tjx_id()
	if (id == 0) then return end
	if (tjx_fig() ~= 0) then
		Msg2Player("Chi bang chu duoc chay bao tri.")
		return
	end
	TONG_ApplyMaintain(id)
	Msg2Player("Da gui bao tri NGAY (tru duy tri, tinh tro cap, Day+1).")
end

function tjx_mt_week()
	local id = tjx_id()
	if (id == 0) then return end
	if (tjx_fig() ~= 0) then
		Msg2Player("Chi bang chu duoc chay don tuan.")
		return
	end
	TONG_ApplyWeeklyMaintain(id)
	Msg2Player("Da gui don TUAN (WeekGoal -> LWeekGoal, Week+1).")
end

function tjx_t777()
	local id = tjx_id()
	if (id == 0) then return end
	TONG_ApplySetTaskValue(id, TJXM_TASKID, 777)
	Msg2Player("Da gui SET TaskValue["..TJXM_TASKID.."] = 777.")
end

function tjx_openwin()
	OpenTongJX2()
end
