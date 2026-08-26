-- ============================================================================
-- SINH TU DONG boi ReverseTools/gen_beidou.py - DUNG SUA TAY.
-- Dich nguoc tu ban Linux script\event\beidoulingpai\ (xem dau tep gen).
-- ============================================================================
Include("\\script\\tinhnang\\3hoatdong\\beidou\\bd_lib.lua")
Include("\\script\\lib\\awardtemplet.lua")

-- ---------------------------------------------------------------- gio mo
function tbBeidou:IsOpenTime()
	local nHour = tonumber(BD_ServerDate("%H"))
	local nBatDau = HD_CFG("HD3_BD_GIO_MO", self.nStartHour)
	local nKetThuc = HD_CFG("HD3_BD_GIO_DONG", self.nEndHour)
	if (nHour < nBatDau or nHour > nKetThuc) then
		return 0
	end
	return 1
end

function tbBeidou:CheckCondition()
	local nCap = HD_CFG("HD3_BD_CAP_TOITHIEU", self.MIN_LEVEL)
	if (ST_GetTransLifeCount ~= nil and ST_GetTransLifeCount() >= 1) then
		return 1
	end
	if (GetLevel() < nCap) then
		return 0
	end
	return 1
end

-- ------------------------------------------------- doi ngay / doi gio
function tbBeidou:UpdateTask()
	local nLast = GetTask(self.TSK_ACCEPT_TIME)
	local nNow = GetCurServerTime()
	if (FormatTime2Date(nLast) ~= FormatTime2Date(nNow)) then
		SetTask(self.TSK_TASK_STATE, 0)
		SetTask(self.TSK_ACCEPT_TIME, nNow)
		SetTask(self.TSK_FINISH_COUNT, 0)
		SetTask(self.TSK_HOUR_COUNT, 0)
		return
	end
	local nGioNay = tonumber(BD_ServerDate("%H"))
	local nGioTruoc = tonumber(FormatTime2String("%H", nLast))
	if (nGioNay > nGioTruoc) then
		SetTask(self.TSK_TASK_STATE, 0)
		SetTask(self.TSK_ACCEPT_TIME, nNow)
		SetTask(self.TSK_HOUR_COUNT, 0)
	end
end

function tbBeidou:CheckOverTime()
	if (GetTask(self.TSK_ACCEPT_TIME) + self.ONE_HOUR < GetCurServerTime()) then
		return 0
	end
	return 1
end

-- ------------------------------------------------------------- menu NPC
-- nThanh = so thu tu thanh cua NPC dang noi chuyen (1..7)
function tbBeidou:Menu(nThanh)
	BD_THANH_HIENTAI = nThanh
	if (self:IsOpenTime() ~= 1) then
		return Talk(1, "", MSG_NO_OPEN)
	end
	if (self:CheckCondition() ~= 1) then
		return Talk(1, "", MSG_LEVEL_LIMIT)
	end
	self:UpdateTask()
	local tb = {
		MSG_WANT_TASK_OPT.."/BD_Want",
		MSG_SUBMIT_TASK_OPT.."/BD_Submit",
		MSG_CANCEL_TASK_OPT.."/BD_Cancel",
		MSG_QUERY_TASK_OPT.."/BD_Query",
		format(BD_MSG_GET_AWARD, self.TOKEN_NUM_AWARD10).."/BD_Doi15",
		format(BD_MSG_GET_AWARD, self.TOKEN_NUM_AWARD13).."/BD_Doi20",
		MSG_END_DIALOG.."/BD_No",
	}
	Say(" "..MSG_TITLE.." Ta lµ B¾c §Èu l·o nh©n. Ng­¬i ®ang gi÷ <color=yellow>"..BD_DemLenhBai().."<color> lÖnh bµi B¾c §Èu.", getn(tb), tb)
end

-- --------------------------------------------------------- nhan nhiem vu
function tbBeidou:Want()
	self:UpdateTask()
	if (GetBitTask(self.TSK_TASK_STATE, self.TSK_BIT_TASK_ID, self.TSK_BIT_ID_LEN) ~= 0) then
		return Talk(1, "", MSG_ALREADY_HAVE_TASK)
	end
	local nMax = HD_CFG("HD3_BD_SO_LAN_NGAY", self.PER_DAY_MAX_TSK_COUNT)
	if (GetTask(self.TSK_FINISH_COUNT) >= nMax) then
		return Talk(1, "", format(MSG_ERR_TASK_DAY_LIMIT, nMax))
	end
	-- Linux: random trong bang nhiem vu. Port nay = di toi 1 thanh khac.
	local nSo = getn(self.THANH)
	local nId = random(1, nSo)
	if (BD_THANH_HIENTAI ~= nil and nId == BD_THANH_HIENTAI and nSo > 1) then
		nId = mod(nId, nSo) + 1
	end
	SetBitTask(self.TSK_TASK_STATE, self.TSK_BIT_TASK_ID, self.TSK_BIT_ID_LEN, nId)
	SetBitTask(self.TSK_TASK_STATE, self.TSK_BIT_TASK_DONE, 1, 0)
	SetTask(self.TSK_ACCEPT_TIME, GetCurServerTime())
	if (GetBitTask(self.TSK_AWARD_STATE, self.TSK_BIT_XUELINGDAN, 1) == 1) then
		SetBitTask(self.TSK_AWARD_STATE, self.TSK_BIT_DOUBLE_AWARD, 1, 1)
		SetBitTask(self.TSK_AWARD_STATE, self.TSK_BIT_XUELINGDAN, 1, 0)
	end
	Msg2Player(format(MSG_GET_TASK, format(BD_MSG_TASK_CITY, self.THANH[nId][1])))
end

function tbBeidou:Query()
	local nId = GetBitTask(self.TSK_TASK_STATE, self.TSK_BIT_TASK_ID, self.TSK_BIT_ID_LEN)
	if (nId == 0) then
		return Talk(1, "", MSG_ERR_NO_TASK)
	end
	local sz = format(MSG_CURR_TASK, format(BD_MSG_TASK_CITY, self.THANH[nId][1]))
	if (GetBitTask(self.TSK_AWARD_STATE, self.TSK_BIT_DOUBLE_AWARD, 1) == 1) then
		sz = sz.." <color=yellow>"..MSG_DOUBLE_AWARD.."<color>"
	end
	Talk(1, "", sz)
end

-- danh dau hoan thanh khi NOI CHUYEN dung NPC o thanh muc tieu
function tbBeidou:CheckWithCity(nThanh)
	local nId = GetBitTask(self.TSK_TASK_STATE, self.TSK_BIT_TASK_ID, self.TSK_BIT_ID_LEN)
	if (nId == 0 or nId ~= nThanh) then
		return 0
	end
	if (GetBitTask(self.TSK_TASK_STATE, self.TSK_BIT_TASK_DONE, 1) == 1) then
		return 0
	end
	if (self:CheckOverTime() ~= 1) then
		return 0
	end
	SetBitTask(self.TSK_TASK_STATE, self.TSK_BIT_TASK_DONE, 1, 1)
	Msg2Player(MSG_FINISH_TASK)
	return 1
end

-- ----------------------------------------------------------- nop nhiem vu
function tbBeidou:Submit()
	local nId = GetBitTask(self.TSK_TASK_STATE, self.TSK_BIT_TASK_ID, self.TSK_BIT_ID_LEN)
	if (nId == 0) then
		return Talk(1, "", MSG_ERR_NO_TASK)
	end
	if (self:CheckOverTime() ~= 1) then
		SetTask(self.TSK_AWARD_STATE, 0)
		SetTask(self.TSK_TASK_STATE, 0)
		return Talk(1, "", MSG_TASK_FAIL)
	end
	if (GetBitTask(self.TSK_TASK_STATE, self.TSK_BIT_TASK_DONE, 1) == 0) then
		return Talk(1, "", MSG_NO_FINISH_TASK)
	end
	if (self:GiveAward() == 1) then
		SetTask(self.TSK_TASK_STATE, 0)
		SetTask(self.TSK_FINISH_COUNT, GetTask(self.TSK_FINISH_COUNT) + 1)
		SetTask(self.TSK_HOUR_COUNT, GetTask(self.TSK_HOUR_COUNT) + 1)
		SetBitTask(self.TSK_AWARD_STATE, self.TSK_BIT_DOUBLE_AWARD, 1, 0)
	end
end

function tbBeidou:GiveAward()
	local nLan = 1
	if (GetBitTask(self.TSK_AWARD_STATE, self.TSK_BIT_DOUBLE_AWARD, 1) == 1) then
		nLan = 2
	end
	if (CountFreeRoomByWH(1, 1, 5) < 5) then
		Talk(1, "", "H·y chõa trèng Ýt nhÊt 5 « hµnh trang.")
		return 0
	end
	-- Linux beidouactivity.lua:248 - nhiem vu di tim NPC thuong 8 trieu exp
	local nExp = HD_CFG("HD3_BD_EXP_NHIEMVU", 8000000) * nLan
	tbAwardTemplet:Give({nExp = nExp}, 1, {self.LOG_TITLE, "get exp"})
	-- Linux:273-277 - 1 Lenh bai Bac Dau (han 1 ngay) + 1 Chan Nguyen Don (Dai)
	local tbLB = {szName = self.ITEM_LENHBAI.szName, tbProp = self.ITEM_LENHBAI.tbProp,
		nBindState = -2, nExpiredTime = HD_CFG("HD3_BD_HAN_LENHBAI", 1440)}
	tbAwardTemplet:Give(tbLB, nLan, {self.LOG_TITLE, "get lenh bai"})
	tbAwardTemplet:Give(self.ITEM_CNDAI, 1, {self.LOG_TITLE, "get CND dai"})
	return 1
end

function tbBeidou:Cancel()
	local nId = GetBitTask(self.TSK_TASK_STATE, self.TSK_BIT_TASK_ID, self.TSK_BIT_ID_LEN)
	if (nId == 0) then
		return Talk(1, "", MSG_ERR_NO_TASK)
	end
	SetTask(self.TSK_TASK_STATE, 0)
	SetTask(self.TSK_AWARD_STATE, 0)
	SetTask(self.TSK_FINISH_COUNT, GetTask(self.TSK_FINISH_COUNT) + 1)
	SetTask(self.TSK_HOUR_COUNT, GetTask(self.TSK_HOUR_COUNT) + 1)
	Msg2Player(format(MSG_CANCEL_TASK, format(BD_MSG_TASK_CITY, self.THANH[nId][1])))
end

-- --------------------------------------------------------- doi lenh bai
function tbBeidou:DoiThuong(nCan, tbThuong)
	if (self:IsOpenTime() ~= 1) then
		return Talk(1, "", MSG_NO_OPEN)
	end
	local nCo = BD_DemLenhBai()
	if (nCo < nCan) then
		return Talk(1, "", format(MSG_ERR_NO_ENOUGH_COUNT, self.ITEM_LENHBAI.szName))
	end
	if (CountFreeRoomByWH(1, 1, 10) < 10) then
		return Talk(1, "", "H·y chõa trèng Ýt nhÊt 10 « hµnh trang.")
	end
	local tp = self.ITEM_LENHBAI.tbProp
	if (ConsumeItem(3, nCan, tp[1], tp[2], tp[3], -1) ~= 1) then
		return Talk(1, "", format(MSG_ERR_NO_ENOUGH_COUNT, self.ITEM_LENHBAI.szName))
	end
	tbAwardTemplet:Give(tbThuong, 1, {self.LOG_TITLE, format("doi %d lenh bai", nCan)})
end

-- ------------------------------------------- phat lenh bai tu hoat dong
-- Ban Linux (beidouactivity.lua:334-340 GiveTokens) phat lenh bai RIENG cua
-- tung hoat dong. NHUNG quet toan cay Linux cho thay 13 ma do KHONG CHO NAO
-- TIEU (SubmitToken10/13 chi dem ma 'Lenh bai Bac Dau'), va truong nAwardExp
-- cua chung cung chet (awardtemplet chi biet khoa tbProp/nExp).
-- Them nua 13 ma do de nMaxStack = 0 => KHONG XEP CHONG, moi cai an 1 o tui.
-- => MAC DINH port nay chi phat 'Lenh bai Bac Dau' (xep chong 50/o, dung de
--    doi thuong). Muon them lenh bai rieng tung hoat dong nhu ban Linux thi
--    dat HD3_BD_LENHBAI_RIENG = 1 trong cauhinh_hoatdong.lua.
function tbBeidou:PhatLenhBai(szKhoa)
	if (HD_CFG("HD3_BD_BAT", 1) ~= 1) then
		return 0
	end
	if (self:CheckCondition() ~= 1) then
		return 0
	end
	local tb = self.LENHBAI_HD[szKhoa]
	if (tb == nil) then
		return 0
	end
	local bRieng = HD_CFG("HD3_BD_LENHBAI_RIENG", 0)
	local bChung = HD_CFG("HD3_BD_QUYDOI_LENHBAI", 1)
	local nCanO = 0
	if (bRieng == 1) then nCanO = nCanO + 1 end
	if (bChung == 1) then nCanO = nCanO + 1 end
	if (nCanO < 1) then
		return 0
	end
	if (CountFreeRoomByWH(1, 1, nCanO) < nCanO) then
		Msg2Player("Hµnh trang ®Çy nªn kh«ng nhËn ®­îc lÖnh bµi B¾c §Èu.")
		return 0
	end
	local nHan = HD_CFG("HD3_BD_HAN_LENHBAI", 1440)
	if (bRieng == 1) then
		tbAwardTemplet:Give({tbProp = {6, 1, tb[1], 1, 0, 0}, nCount = 1, nBindState = -2,
			nExpiredTime = nHan}, 1, {self.LOG_TITLE, "token "..szKhoa})
	end
	if (bChung == 1) then
		tbAwardTemplet:Give({szName = self.ITEM_LENHBAI.szName, tbProp = self.ITEM_LENHBAI.tbProp,
			nCount = 1, nBindState = -2, nExpiredTime = nHan}, 1, {self.LOG_TITLE, "lenh bai "..szKhoa})
	end
	Msg2Player("NhËn ®­îc lÖnh bµi B¾c §Èu tõ ho¹t ®éng <color=yellow>"..tb[2].."<color>")
	return 1
end

-- ============================================================================
-- MOC TU CAC HOAT DONG - moi ham deu an toan khi he Bac Dau chua nap.
-- ============================================================================
function HD3_BD_Co()
	return (tbBeidou ~= nil and tbBeidou.PhatLenhBai ~= nil)
end

-- Tong Kim: Linux OnFinishSongJin - theo diem tich luy (task 751)
function HD3_BD_TongKim(nDiem)
	if (not HD3_BD_Co()) then return 0 end
	nDiem = nDiem or 0
	local szKhoa = nil
	if (nDiem >= 20000) then szKhoa = "phongvan4"
	elseif (nDiem >= 10000) then szKhoa = "phongvan3"
	elseif (nDiem >= 5000) then szKhoa = "phongvan2"
	elseif (nDiem >= 2000) then szKhoa = "phongvan1"
	end
	if (szKhoa == nil) then return 0 end
	return tbBeidou:PhatLenhBai(szKhoa)
end

-- Vuot ai: Linux OnPassChuanGuan - qua ai thu 10 va thu 28
function HD3_BD_VuotAi(nSoAi)
	if (not HD3_BD_Co()) then return 0 end
	if (nSoAi == 28) then return tbBeidou:PhatLenhBai("vuotai2") end
	if (nSoAi == 10) then return tbBeidou:PhatLenhBai("vuotai1") end
	return 0
end

-- Phong Lang Do: Linux OnLanding - PHAI DANG MANG Truy Cong Lenh (khong tru).
-- bNhiemVuThuyTac = 1 (khung gio ton phi) thi duoc lenh bai cap 2.
function HD3_BD_PhongLangDo(bNhiemVuThuyTac)
	if (not HD3_BD_Co()) then return 0 end
	local tp = tbBeidou.ITEM_TRUYCONG
	if (CalcItemCount(3, tp[1], tp[2], tp[3], -1) < 1) then
		return 0
	end
	if (bNhiemVuThuyTac == 1) then
		return tbBeidou:PhatLenhBai("phonglangdo2")
	end
	return tbBeidou:PhatLenhBai("phonglangdo1")
end

-- Tin Su: Linux OnFinishMessenger
function HD3_BD_TinSu()
	if (not HD3_BD_Co()) then return 0 end
	return tbBeidou:PhatLenhBai("tinsu")
end

-- Boss sat thu: Linux OnFinishKillerBoss - CHI nhom boss cap 90
function HD3_BD_SatThu(nCapBoss)
	if (not HD3_BD_Co()) then return 0 end
	if (nCapBoss ~= 90) then return 0 end
	return tbBeidou:PhatLenhBai("satthu")
end

-- --------------------------------------------------- cau noi cho menu Say
function BD_Want()    tbBeidou:Want()   end
function BD_Submit()  tbBeidou:Submit() end
function BD_Cancel()  tbBeidou:Cancel() end
function BD_Query()   tbBeidou:Query()  end
function BD_Doi15()   tbBeidou:DoiThuong(tbBeidou.TOKEN_NUM_AWARD10, tbBeidou.tbAward10) end
function BD_Doi20()   tbBeidou:DoiThuong(tbBeidou.TOKEN_NUM_AWARD13, tbBeidou.tbAward13) end
function BD_No()      end
