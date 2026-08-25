-------------------------------------------------------------------------
-- FileName		:	kill_level.lua
-- Author		:	xiaoyang
-- CreateTime	:	2005-03-31 10:56:14
-- Desc			:	boss杀手任务怪物死亡脚本
-------------------------------------------------------------------------
IncludeLib("RELAYLADDER");	--排行榜
IncludeLib("ITEM")
IncludeLib("NPCINFO")
Include("\\script\\task\\newtask\\newtask_head.lua")
Include("\\script\\task\\tollgate\\killer\\lib_killlevel.lua")
Include( "\\script\\task\\tollgate\\killbosshead.lua" )
Include("\\script\\event\\change_destiny\\mission.lua");	-- 逆天改命
Include( "\\script\\tong\\contribution_entry.lua" )
Include("\\script\\misc\\eventsys\\eventsys.lua")
Include("\\script\\lib\\awardtemplet.lua")

-- TSKID_KILLERDATE	= 1192;	--接任务时的日期
-- TSKID_KILLERMAXCOUNT	= 1193;	--每天杀人次数


function OnDeath( nNpcIndex )
local Uworld1082 = nt_getTask(TSKID_KILLTASKID)

	if ( Uworld1082 >= 1 ) and ( Uworld1082 <= 160 ) then

		kill_level20(nNpcIndex)
	end
end
 
-----------------------------------------------------------------------

--给符合杀死boss的玩家发奖并改变任务变量
function kill_level20(nNpcIndex)
	local Uworld1082 = nt_getTask(TSKID_KILLTASKID);
	local nNpcSign = GetNpcParam( nNpcIndex, 1 );
	local nseries = NPCINFO_GetSeries(nNpcIndex)
	local Uworld1217 = nt_getTask(1217)	
	if( Uworld1082 == nNpcSign ) then
		
		nt_setTask(1217,Uworld1217+1);
		
		-- 设置任务完成次数排名
		Ladder_NewLadder(10119, GetName(),Uworld1217+1,1);
		
--		if ( Uworld1082 >= 1 and Uworld1082 <= 20 ) then
--			ITEM_DropRateItem(nNpcIndex, 8,"\\settings\\droprate\\boss\\bosstask_lev20.ini", 0, 4, nseries);
--		end
--		if ( Uworld1082 >= 21 and Uworld1082 <= 40 ) then
--			ITEM_DropRateItem(nNpcIndex, 8,"\\settings\\droprate\\boss\\bosstask_lev30.ini", 0, 5, nseries);
--		end
--		if ( Uworld1082 >= 41 and Uworld1082 <= 60 ) then
--			ITEM_DropRateItem(nNpcIndex, 8,"\\settings\\droprate\\boss\\bosstask_lev40.ini", 0, 6, nseries);
--		end
--		if ( Uworld1082 >= 61 and Uworld1082 <= 80 ) then
--			ITEM_DropRateItem(nNpcIndex, 8,"\\settings\\droprate\\boss\\bosstask_lev50.ini", 0, 7, nseries);
--		end
--		if ( Uworld1082 >= 81 and Uworld1082 <= 100 ) then
--			ITEM_DropRateItem(nNpcIndex, 8,"\\settings\\droprate\\boss\\bosstask_lev60.ini", 0, 8, nseries);
--		end
--		if ( Uworld1082 >= 101 and Uworld1082 <= 120 ) then
--			ITEM_DropRateItem(nNpcIndex, 8,"\\settings\\droprate\\boss\\bosstask_lev70.ini", 0, 9, nseries);
--		end
--		if ( Uworld1082 >= 121 and Uworld1082 <= 140 ) then
--			ITEM_DropRateItem(nNpcIndex, 8,"\\settings\\droprate\\boss\\bosstask_lev80.ini", 0, 10, nseries);
--		end
		if ( Uworld1082 >= 141 and Uworld1082 <= 160 ) then
			tbChangeDestiny:completeMission_Killer();	-- 逆天改命
			ITEM_DropRateItem(nNpcIndex, 8,"\\settings\\droprate\\boss\\bosstask_lev90.ini", 0, 10, nseries);
		end		
		G_ACTIVITY:OnMessage("FinishKillerBoss", PlayerIndex, TSKID_KILLTASKID, Uworld1082, 0)
		EventSys:GetType("KillerBoss"):OnPlayerEvent("OnKillBoss", PlayerIndex, TSKID_KILLTASKID, Uworld1082)
		--EventSys:GetType("KillerBoss"):OnTeamEvent("OnFinish", PlayerIndex, TSKID_KILLTASKID, Uworld1082)
		EventSys:GetType("KillerBoss"):OnPlayerEvent("OnFinish", PlayerIndex, TSKID_KILLTASKID, Uworld1082)
		SetMemberTask(TSKID_KILLTASKID,Uworld1082,0,killbossall, nseries)
	end
end

-----------------------------------------------------------------------

function killbossall()
	AddSkillState(541,1,0,54)
	Msg2Player("Bạn đã thu phục được tên sát thủ, hoàn thành nhiệm vụ!")
end

function OnFinishKillerTask(TSKID_KILL)
	local Uworld1082 = nt_getTask(TSKID_KILL);
	if Uworld1082 >= 141 and Uworld1082 <= 160 then
		local tbAward = {
			[1] = {
				{szName="Tinh Tinh Khoáng",tbProp={6,1,3811,1,0,0},nCount=1,nRate=1.04},
				{szName="Tinh Thiết Khoáng",tbProp={6,1,3810,1,0,0},nCount=1,nRate=0.52},
				{szName="Chiếu Dạ Ngọc Sư Tử ",tbProp={0,10,5,5,0,0},nCount=1,nRate=0.0021},
				{szName="Xích Thố ",tbProp={0,10,5,2,0,0},nCount=1,nRate=0.0021},
				{szName="Tuyệt ảnh",tbProp={0,10,5,3,0,0},nCount=1,nRate=0.0021},
				{szName="Đích Lô ",tbProp={0,10,5,4,0,0},nCount=1,nRate=0.0021},
				{szName="Ô Vân Đạp Tuyết",tbProp={0,10,5,1,0,0},nCount=1,nRate=0.0021},
				{szName="Phi Vân",tbProp={0,10,8,1,0,0},nCount=1,nRate=0.0007},
				{szName="Bôn Tiêu",tbProp={0,10,6,1,0,0},nCount=1,nRate=0.0007},
				{szName="Phiên Vũ ",tbProp={0,10,7,1,0,0},nCount=1,nRate=0.0007},
				{szName="Phong Vân Bạch Mã",tbProp={0,10,19,1,0,0},nCount=1,nRate=0.0007,nExpiredTime=43200},
				{szName="Phong Vân Chiến Mã",tbProp={0,10,20,1,0,0},nCount=1,nRate=0.0003,nExpiredTime=43200},
				{szName="Phong Vân Thần Mã",tbProp={0,10,21,1,0,0},nCount=1,nRate=0.0001,nExpiredTime=43200},
				{szName="Túi Dược Phẩm",tbProp={6,1,30557,1,0,0},nCount=1,nRate=68.2639},
				{szName="Đại Lực hoàn",tbProp={6,0,3,1,0,0},nCount=1,nRate=15},
				{szName="Phi Tốc hoàn",tbProp={6,0,6,1,0,0},nCount=1,nRate=15},
				{szName="Đồ Phổ Đằng Long Khôi",tbProp={6,1,30528,1,0,0},nCount=1,nRate=0.0003},
				{szName="Đồ Phổ Đằng Long Hài",tbProp={6,1,30530,1,0,0},nCount=1,nRate=0.0003},
				{szName="Đồ Phổ Đằng Long Yêu Đái",tbProp={6,1,30531,1,0,0},nCount=1,nRate=0.0003},
				{szName="Đồ Phổ Đằng Long Hộ Uyển",tbProp={6,1,30532,1,0,0},nCount=1,nRate=0.0003},
				{szName="Đồ Phổ Đằng Long Hạng Liên",tbProp={6,1,30533,1,0,0},nCount=1,nRate=0.0003},
				{szName="Đồ Phổ Đằng Long Bội",tbProp={6,1,30534,1,0,0},nCount=1,nRate=0.0003},
				{szName="Đồ Phổ Đằng Long Thượng Giới Chỉ",tbProp={6,1,30535,1,0,0},nCount=1,nRate=0.0003},
				{szName="Đồ Phổ Đằng Long Hạ Giới Chỉ",tbProp={6,1,30536,1,0,0},nCount=1,nRate=0.0003},
				{szName="Đằng Long Thạch - Hạ",tbProp={6,1,30538,1,0,0},nCount=1,nRate=0.16},},
			[2] = { -- Phần thưởng cố định
				{szName="Điểm kinh nghiệm", nExp=10000000,},},
		}
		tbAwardTemplet:Give(tbAward, 1, {"KillerTask", "FinishLevel90"})
	end
end

EventSys:GetType("KillerBoss"):Reg("OnFinish", OnFinishKillerTask)