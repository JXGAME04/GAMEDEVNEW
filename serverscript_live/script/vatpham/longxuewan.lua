-- longxuewan.lua - Long Huyet Hoan (vat pham 6/1/2126).
-- [PORT LINUX 30/08] PORT tu D:\ServerLinux\server1\script\item\longxuewan.lua
-- [LXW-V2 30/08] sua 6 diem bo phan bien bat duoc - xem cuoi tep.
--
-- VI SAO PHAI PORT: ban cu (Fong Kieu 2021) dat SetTask(43,0) va dem task 65.
-- He Vuot Ai / Thach Dau Thoi Gian MOI (ban Linux, dang chay) KHONG doc hai
-- task do -> vat pham VO DUNG. Ben NHAN da san sang tu truoc:
--   challengeoftime\npc\dragonboat_main.lua:154-160  doc TSK_Longxuewan_Date
--                                                     + TSK_Longxuewan_avail
--   task\metempsychosis\translife_5.lua:105-109      doc TSK_Longxuewan_avail
-- chi thieu ben PHAT - la tep nay.
--
-- Ma task CHEP TU script\missions\challengeoftime\include.lua (dong 19-21
-- va 126-128). KHONG Include tep do - ly do THAT: no goi
-- IncludeLib("RELAYLADDER") + Include bigboss.lua + activitysys\functionlib.lua
-- + tong_award_head.lua, tuc keo ca mot manh he thong vao Lua_State cua mot
-- script vat pham. Do la be mat tac dung phu, KHONG phai chuyen so dong:
-- tep do 11.452 byte, con header\cauhinh_hoatdong.lua ma tep nay CO Include
-- lai 24.504 byte. (Ban dau toi ghi ly do sai o cho nay - da sua.)
-- !! Doi lai: 5 hang so duoi day bi NHAN DOI. Neu sua include.lua thi phai
-- !! sua ca day.

Include("\\script\\header\\cauhinh_hoatdong.lua")	-- HD_CFG (tep LA)

-- chep tu challengeoftime\include.lua:19-21
LXW_TSK_JOIN_DATE        = 1551
LXW_TSK_REMAIN_COUNT     = 1550
-- chep tu challengeoftime\include.lua:126-128
LXW_TSK_DATE             = 2641
LXW_TSK_USE              = 2642
LXW_TSK_AVAIL            = 4018
-- ban Linux gioi han 2 vien/ngay (longxuewan.lua:19)
LXW_MAX_NGAY             = 2
-- LXW_CAN_XU = 1	-- bo dau -- de chan nguoi choi 0 Xu (nhu ban Linux)

function LXW_LuotNgay()
	-- ban Linux ghi cung 1; cay ta cho chinh bang cau hinh
	-- (challengeoftime\include.lua:39 COUNT_LIMIT = HD_CFG("HD3_VA_LUOT_NGAY", ...))
	if (HD_CFG ~= nil) then
		return HD_CFG("HD3_VA_LUOT_NGAY", 1)
	end
	return 1
end

function main(nItemIndex)
	-- [LXW-V2] khoi phuc chot cua ban JX1 cu: khong dung trong khu chien dau.
	-- Ban Linux khong co chot nay, nhung bo di la doi hanh vi nguoi choi thay.
	if (GetFightState() ~= 0) then
		Say("Kh«ng thÓ sö dông trong khu vùc chiÕn ®Êu", 0)
		return 1
	end
	-- [LXW-V2] gop mot chot nhu ban Linux de cau thong bao khop ca hai ve.
	if (GetLevel() < 90 or (LXW_CAN_XU ~= nil and GetExtPoint(0) == 0)) then
		Say("Ng­êi ch¬i ph¶i cÊp 90 trë lªn vµ ®· n¹p thÎ míi sö dông ®­îc", 0)
		return 1
	end

	-- [LXW-V2] TINH truoc, chi GHI sau khi tru duoc vat pham. Ban truoc ghi
	-- khoi reset ngay TRUOC chot tru, nen tru that bai van dua task
	-- LXW_TSK_AVAIL tu -1 (dragonboat_main.lua:160 dat) ve 0.
	local ndate = tonumber(GetLocalDate("%y%m%d"))
	local bNgayMoi = 0
	if (ndate ~= GetTask(LXW_TSK_DATE)) then
		bNgayMoi = 1
	end
	local nDaDung = 0
	if (bNgayMoi == 0) then
		nDaDung = GetTask(LXW_TSK_USE)
	end
	if (nDaDung >= LXW_MAX_NGAY) then
		Say("Mçi ngµy <sex> chØ ®­îc sö dông "..LXW_MAX_NGAY.." Long HuyÕt Hoµn", 0)
		return 1
	end

	if (RemoveItemByIndex(nItemIndex) ~= 1) then
		return 1
	end

	if (bNgayMoi == 1) then
		SetTask(LXW_TSK_DATE, ndate)
		SetTask(LXW_TSK_USE, 0)
		SetTask(LXW_TSK_AVAIL, 0)
		if (ndate ~= GetTask(LXW_TSK_JOIN_DATE)) then
			SetTask(LXW_TSK_JOIN_DATE, ndate)
			SetTask(LXW_TSK_REMAIN_COUNT, LXW_LuotNgay())
		end
	end
	SetTask(LXW_TSK_REMAIN_COUNT, GetTask(LXW_TSK_REMAIN_COUNT) + 1)
	SetTask(LXW_TSK_USE, nDaDung + 1)
	SetTask(LXW_TSK_AVAIL, GetTask(LXW_TSK_AVAIL) + 1)
	Say("Thu ®­îc thªm c¬ héi 1 lÇn tham gia ho¹t ®éng <th¸ch ®Êu thêi gian>!", 0)
	return 0
end
