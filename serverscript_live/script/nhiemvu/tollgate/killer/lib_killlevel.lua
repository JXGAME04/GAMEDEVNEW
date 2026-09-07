-- [LOCAL54 06/09 toi] 2 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local HD3_ST_ThuongBoss, jiefangri_award
-------------------------------------------------------------------------
Include("\\script\\tinhnang\\3hoatdong\\beidou\\bd_activity.lua")
Include("\\script\\header\\cauhinh_hoatdong.lua")	-- [3HD C29] noi cauhinh thuong
-- FileName		:	lib_killlevel.lua
-- Author		:	xiaoyang
-- CreateTime	:	2005-03-31 10:42:00
-- Desc			:	×é¶ÓµÄËùÓÐÍæ¼ÒÈÎÎñ±äÁ¿½Ô¸Ä±äµÄÀà
-------------------------------------------------------------------------

-- myTaskNumber £ºÐèÒª¸Ä±äµÄÈÎÎñ±äÁ¿±àºÅ
-- myOrgValue   £º·ûºÏÌõ¼þµÄÅÐ¶ÏÖµ
-- myTaskValue  £º¸Ä±äºóµÄ±äÁ¿Öµ
-- SetMemberTask µÄ·µ»ØÖµÈç¹ûÊÇ 0 ÔòÖ»¸Ä±ä²»×é¶ÓÍæ¼ÒµÄ±äÁ¿£¬´óÓÚ»òÕßµÈÓÚ 1 ÔòÎª¸Ä±äÁË×é¶ÓÍæ¼Ò±äÁ¿µÄÊýÁ¿
-- ÎÞÂÛÊÇµ¥ÈË»¹ÊÇ×é¶ÓÊ±¶¼Ã»ÓÐ¸Ä±äÍæ¼ÒµÄ±äÁ¿Ê±Ôò·µ»Ø 80
-- ¶øÈÎºÎÌõ¼þ¶¼²»Âú×ãµÄÒì³£´¦ÀíÔò·µ»Ø 110

Include("\\script\\task\\newtask\\newtask_head.lua")
Include("\\script\\tong\\tong_award_head.lua")
Include("\\script\\lib\\awardtemplet.lua");		-- ½±ÀøÄ£°å
Include("\\script\\task\\tollgate\\killer\\mibao_head.lua")
Include("\\script\\activitysys\\playerfunlib.lua")

function SetMemberTask(myTaskNumber,myOrgValue,myTaskValue,fnCallback, series)

	local nPreservedPlayerIndex = PlayerIndex
	local nMemCount = GetTeamSize()
	local myMemberTask
	local myChangeMember = 0
	local Uworld1217 = nt_getTask(1217);
	
	DynamicExecuteByPlayer(PlayerIndex, "\\script\\huoyuedu\\huoyuedu.lua", "tbHuoYueDu:AddHuoYueDu", "shashourenwu")
	if (nMemCount == 0 ) then
		myMemberTask = GetTask(myTaskNumber)
		if (myMemberTask == myOrgValue) then
			add_shashouling(myOrgValue, series)
			
			nt_setTask(TSKID_KILLERMAXCOUNT, GetTask(TSKID_KILLERMAXCOUNT) + 1);
			
			tongaward_killer()
			nt_setTask(myTaskNumber,myTaskValue);
			
			fnCallback()
		end
		
	else
		myMemberTask = GetTask(myTaskNumber)
		if (myMemberTask == myOrgValue) then
			tongaward_killer()
		end
		for i = 1, nMemCount do
			PlayerIndex = GetTeamMember(i)
			myMemberTask = GetTask(myTaskNumber)
			if (myMemberTask == myOrgValue) then	-- ·ûºÏÁËÌõ¼þµÄ¶ÓÓÑ²Å¸Ä±äÈÎÎñ±äÁ¿	
				add_shashouling(myOrgValue, series)
				
				nt_setTask(TSKID_KILLERMAXCOUNT, GetTask(TSKID_KILLERMAXCOUNT) + 1);
				
				nt_setTask(myTaskNumber,myTaskValue)
				myChangeMember = myChangeMember + 1
				
				fnCallback();
			end
			
		end
		
		PlayerIndex = nPreservedPlayerIndex;
		
	end
	
end;


-- [3HD C29] Thuong 1 lan giet boss sat thu - noi cauhinh_hoatdong.lua:
--   HD3_ST_EXP (bang exp theo nhom cap) / HD3_ST_HESO_EXP (%) / HD3_ST_SO_LENH.
-- Dung AddSumExp thay AddOwnExp: AddOwnExp dat m_nExp = 0 khi len cap
-- (KPlayer.cpp:2629) => MAT phan exp du. Day la loi that cua JX1, khong phai
-- khac biet voi ban Linux.
function HD3_ST_ThuongBoss(nCapNhom, nExpGoc, nSeries)
	-- [BAC DAU 25/08] Linux beidouactivity.lua:460-472 OnFinishKillerBoss:
	-- CHI nhom boss cap 90 moi duoc lenh bai (ham tu loc nCapNhom ~= 90).
	if (HD3_BD_SatThu ~= nil) then
		HD3_BD_SatThu(nCapNhom)
	end
	local tbExp = HD_CFG("HD3_ST_EXP", nil)
	local nExp = nExpGoc
	if (tbExp ~= nil and tbExp[nCapNhom] ~= nil) then
		nExp = tbExp[nCapNhom]
	end
	local nHeSo = HD_CFG("HD3_ST_HESO_EXP", 100)
	if (nHeSo ~= 100) then
		nExp = floor(nExp * nHeSo / 100)
	end
	if (nExp > 0) then
		AddSumExp(nExp)
	end
	local nSo = HD_CFG("HD3_ST_SO_LENH", 1)
	if (nSo < 1) then nSo = 1 end
	for i = 1, nSo do
		AddItem(6, 1, 398, nCapNhom, nSeries, 0, 0)
	end
end
function add_shashouling(nvalue, series)
	if ( nvalue >= 1 ) and ( nvalue<= 20  ) then
		HD3_ST_ThuongBoss(20, 15000, series)
		Msg2Player("B¹n nhËn ®­îc 1 s¸t thñ lÖnh cÊp 20")
	elseif ( nvalue >= 21 ) and ( nvalue<= 40  ) then
		HD3_ST_ThuongBoss(30, 20000, series)
		Msg2Player("B¹n nhËn ®­îc 1 s¸t thñ lÖnh cÊp 30")
	elseif ( nvalue >= 41 ) and ( nvalue<= 60  ) then
		HD3_ST_ThuongBoss(40, 30000, series)
		Msg2Player("B¹n nhËn ®­îc 1 s¸t thñ lÖnh cÊp 40")
	elseif ( nvalue >= 61 ) and ( nvalue<= 80  ) then
		HD3_ST_ThuongBoss(50, 50000, series)
		Msg2Player("B¹n nhËn ®­îc 1 s¸t thñ lÖnh cÊp 50")
	elseif ( nvalue >= 81 ) and ( nvalue<= 100  ) then
		HD3_ST_ThuongBoss(60, 60000, series)
		Msg2Player("B¹n nhËn ®­îc 1 s¸t thñ lÖnh cÊp 60")
	elseif ( nvalue >= 101 ) and ( nvalue<= 120  ) then
		HD3_ST_ThuongBoss(70, 80000, series)
		Msg2Player("B¹n nhËn ®­îc 1 s¸t thñ lÖnh cÊp 70")
	elseif ( nvalue >= 121 ) and ( nvalue<= 140  ) then
		HD3_ST_ThuongBoss(80, 100000, series)
		Msg2Player("B¹n nhËn ®­îc 1 s¸t thñ lÖnh cÊp 80")
	elseif ( nvalue >= 141 ) and ( nvalue<= 160  ) then

		--Ö»ÓÐ×ö90¼¶ÉÏµÄ ²ÅÓÐ¿ÉÄÜÑ§Ï°120¼¶¼¼ÄÜ
		AddExp_Skill_Extend(140000);
		HD3_ST_ThuongBoss(90, 140000, series)
		Msg2Player("B¹n nhËn ®­îc 1 s¸t thñ lÖnh cÊp 90")
		tbAwardTemplet:GiveAwardByList({{szName = "S¸t Thñ BÝ B¶o", tbProp = {6,1,2356,1,1,0}, nRate = 50}}, format("Get %s", "S¸t Thñ BÝ B¶o"), 1)
		jiefangri_award()	
	end
end

function jiefangri_award()
	local nLevel = 150
	
	if PlayerFunLib:CheckTotalLevel(nLevel, "", ">=") ~= 1 then
		return
	end

	local tbItem = {
		[1]={szName="Huy Ch­¬ng ChiÕn C«ng",tbProp={6,1,1827,1,0,0},nExpiredTime=20110523},
		[2]={szName="C©y Bót",tbProp={6,1,2183,1,0,0},nExpiredTime=20110523},
		[3]={szName="Phï HiÖu",tbProp={6,1,3444,1,0,0},nExpiredTime=20110523},
	}
	
	local tbshashou = {
		[1] = 2,
		[2] = 1,
		[3] = 2,
	}	
	local tbStartDate = {
		[1] = 201104210000,
		[2] = 201105020000,
		[3] = 201105160000,
		}
		
	local tbMaiDian = {
		[1] = "jiefangri_shashouchanchuzhangongjiangzhang",
		[2] = "jiefangri_shashouchanchuzhibi",
		[3] = "jiefangri_shashouchanchujianzhang",
		}	
	local nEndDate = 201105230000
	local ndate = tonumber(GetLocalDate("%Y%m%d%H%M"))
	
	for i=1,getn(tbStartDate) do
		if ndate >= tbStartDate[i] and ndate <= nEndDate then
			tbAwardTemplet:Give(tbItem[i], tbshashou[i], {"EventChienThang042011", "NhanDuocNguyenLieuTuBossSatThu"})
			AddStatData(tbMaiDian[i], tbshashou[i])
		end
	end
end