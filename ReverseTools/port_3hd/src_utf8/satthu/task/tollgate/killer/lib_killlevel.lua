-------------------------------------------------------------------------
-- FileName		:	lib_killlevel.lua
-- Author		:	xiaoyang
-- CreateTime	:	2005-03-31 10:42:00
-- Desc			:	组队的所有玩家任务变量皆改变的类
-------------------------------------------------------------------------

-- myTaskNumber ：需要改变的任务变量编号
-- myOrgValue   ：符合条件的判断值
-- myTaskValue  ：改变后的变量值
-- SetMemberTask 的返回值如果是 0 则只改变不组队玩家的变量，大于或者等于 1 则为改变了组队玩家变量的数量
-- 无论是单人还是组队时都没有改变玩家的变量时则返回 80
-- 而任何条件都不满足的异常处理则返回 110

Include("\\script\\task\\newtask\\newtask_head.lua")
Include("\\script\\tong\\tong_award_head.lua")
Include("\\script\\lib\\awardtemplet.lua");		-- 奖励模板
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
			if (myMemberTask == myOrgValue) then	-- 符合了条件的队友才改变任务变量	
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

function add_shashouling(nvalue, series)
	if ( nvalue >= 1 ) and ( nvalue<= 20  ) then
		AddOwnExp(15000)
		AddItem(6,1,399,20,series,0)
		Msg2Player("Bạn nhận được 1 sát thủ lệnh cấp 20")
	elseif ( nvalue >= 21 ) and ( nvalue<= 40  ) then
		AddOwnExp(20000)
		AddItem(6,1,399,30,series,0)
		Msg2Player("Bạn nhận được 1 sát thủ lệnh cấp 30")
	elseif ( nvalue >= 41 ) and ( nvalue<= 60  ) then
		AddOwnExp(30000)
		AddItem(6,1,399,40,series,0)
		Msg2Player("Bạn nhận được 1 sát thủ lệnh cấp 40")
	elseif ( nvalue >= 61 ) and ( nvalue<= 80  ) then
		AddOwnExp(50000)
		AddItem(6,1,399,50,series,0)
		Msg2Player("Bạn nhận được 1 sát thủ lệnh cấp 50")
	elseif ( nvalue >= 81 ) and ( nvalue<= 100  ) then
		AddOwnExp(60000)
		AddItem(6,1,399,60,series,0)
		Msg2Player("Bạn nhận được 1 sát thủ lệnh cấp 60")
	elseif ( nvalue >= 101 ) and ( nvalue<= 120  ) then
		AddOwnExp(80000)
		AddItem(6,1,399,70,series,0)
		Msg2Player("Bạn nhận được 1 sát thủ lệnh cấp 70")
	elseif ( nvalue >= 121 ) and ( nvalue<= 140  ) then
		AddOwnExp(100000)
		AddItem(6,1,399,80,series,0)
		Msg2Player("Bạn nhận được 1 sát thủ lệnh cấp 80")
	elseif ( nvalue >= 141 ) and ( nvalue<= 160  ) then
		AddOwnExp(140000)
		--只有做90级上的 才有可能学习120级技能
		AddExp_Skill_Extend(140000);
		AddItem(6,1,399,90,series,0)
		Msg2Player("Bạn nhận được 1 sát thủ lệnh cấp 90")
		tbAwardTemplet:GiveAwardByList({{szName = "Sát Thủ Bí Bảo", tbProp = {6,1,2347,1,1,0}, nRate = 50}}, format("Get %s", "Sát Thủ Bí Bảo"), 1)
		jiefangri_award()	
	end
end

function jiefangri_award()
	local nLevel = 150
	
	if PlayerFunLib:CheckTotalLevel(nLevel, "", ">=") ~= 1 then
		return
	end

	local tbItem = {
		[1]={szName="Huy Chương Chiến Công",tbProp={6,1,2823,1,0,0},nExpiredTime=20110523},
		[2]={szName="Cây Bút",tbProp={6,1,2825,1,0,0},nExpiredTime=20110523},
		[3]={szName="Phù Hiệu",tbProp={6,1,2826,1,0,0},nExpiredTime=20110523},
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