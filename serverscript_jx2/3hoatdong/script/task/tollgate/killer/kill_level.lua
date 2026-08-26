-------------------------------------------------------------------------
-- FileName		:	kill_level.lua
-- Author		:	xiaoyang
-- CreateTime	:	2005-03-31 10:56:14
-- Desc			:	bossÉ±ÊÖÈÎÎñ¹ÖÎïËÀÍö½Å±¾
-------------------------------------------------------------------------
IncludeLib("RELAYLADDER");	--ÅÅÐÐ°ñ
IncludeLib("ITEM")
IncludeLib("NPCINFO")
Include("\\script\\task\\newtask\\newtask_head.lua")
Include("\\script\\task\\tollgate\\killer\\lib_killlevel.lua")
Include( "\\script\\task\\tollgate\\killbosshead.lua" )
Include("\\script\\event\\change_destiny\\mission.lua");	-- ÄæÌì¸ÄÃü
Include( "\\script\\tong\\contribution_entry.lua" )
Include("\\script\\misc\\eventsys\\eventsys.lua")
Include("\\script\\lib\\awardtemplet.lua")
Include("\\script\\header\\cauhinh_hoatdong.lua")	-- [3HD] noi cauhinh

-- TSKID_KILLERDATE	= 1192;	--½ÓÈÎÎñÊ±µÄÈÕÆÚ
-- TSKID_KILLERMAXCOUNT	= 1193;	--Ã¿ÌìÉ±ÈË´ÎÊý


function OnDeath( nNpcIndex )
local Uworld1082 = nt_getTask(TSKID_KILLTASKID)

	if ( Uworld1082 >= 1 ) and ( Uworld1082 <= 160 ) then

		kill_level20(nNpcIndex)
	end
end
 
-----------------------------------------------------------------------

--¸ø·ûºÏÉ±ËÀbossµÄÍæ¼Ò·¢½±²¢¸Ä±äÈÎÎñ±äÁ¿
function kill_level20(nNpcIndex)
	local Uworld1082 = nt_getTask(TSKID_KILLTASKID);
	local nNpcSign = GetNpcParam( nNpcIndex, 1 );
	local nseries = NPCINFO_GetSeries(nNpcIndex)
	local Uworld1217 = nt_getTask(1217)	
	if( Uworld1082 == nNpcSign ) then
		
		nt_setTask(1217,Uworld1217+1);
		
		-- ÉèÖÃÈÎÎñÍê³É´ÎÊýÅÅÃû
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
			tbChangeDestiny:completeMission_Killer();	-- ÄæÌì¸ÄÃü
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
	Msg2Player("B¹n ®· thu phôc ®­îc tªn s¸t thñ, hoµn thµnh nhiÖm vô!")
end

function OnFinishKillerTask(TSKID_KILL)
	local Uworld1082 = nt_getTask(TSKID_KILL);
	if Uworld1082 >= 141 and Uworld1082 <= 160 then
		-- [3HD 25/08 C51] Ban LINUX danh so khoi hoan duoc bang DETAIL = 0
		-- (magicscript.txt:6 Dai Luc hoan 6,0,3 / :9 Phi Toc hoan 6,0,6); JX1 danh
		-- so bang DETAIL = 1 (6,1,3 / 6,1,6, script potion15.lua). Giu nguyen 6,0,*
		-- thi AddItem IM LANG khong tao gi => 15% + 15% = 30,0024% moi lan giet boss
		-- cap 90 khong ra vat pham nao. Doi sang ma JX1 - tra THEO TEN.
		local tbAward = {
			[1] = {
				{szName="Tinh Tinh Kho¸ng",tbProp={6,1,4429,1,0,0},nCount=1,nRate=1.04},
				{szName="Tinh ThiÕt Kho¸ng",tbProp={6,1,4428,1,0,0},nCount=1,nRate=0.52},
				{szName="ChiÕu D¹ Ngäc S­ Tö ",tbProp={0,10,5,5,0,0},nCount=1,nRate=0.0021},
				{szName="XÝch Thè ",tbProp={0,10,5,2,0,0},nCount=1,nRate=0.0021},
				{szName="TuyÖt ¶nh",tbProp={0,10,5,3,0,0},nCount=1,nRate=0.0021},
				{szName="§Ých L« ",tbProp={0,10,5,4,0,0},nCount=1,nRate=0.0021},
				{szName="¤ V©n §¹p TuyÕt",tbProp={0,10,5,1,0,0},nCount=1,nRate=0.0021},
				{szName="Phi V©n",tbProp={0,10,8,1,0,0},nCount=1,nRate=0.0007},
				{szName="B«n Tiªu",tbProp={0,10,6,1,0,0},nCount=1,nRate=0.0007},
				{szName="Phiªn Vò ",tbProp={0,10,7,1,0,0},nCount=1,nRate=0.0007},
				{szName="Phong V©n B¹ch M·",tbProp={0,10,19,1,0,0},nCount=1,nRate=0.0007,nExpiredTime=43200},
				{szName="Phong V©n ChiÕn M·",tbProp={0,10,20,1,0,0},nCount=1,nRate=0.0003,nExpiredTime=43200},
				{szName="Phong V©n ThÇn M·",tbProp={0,10,21,1,0,0},nCount=1,nRate=0.0001,nExpiredTime=43200},
				{szName="Tói D­îc PhÈm",tbProp={6,1,4813,1,0,0},nCount=1,nRate=68.2639},
				{szName="§¹i Lùc hoµn",tbProp={6,1,3,1,0,0},nCount=1,nRate=15},
				{szName="Phi Tèc hoµn",tbProp={6,1,6,1,0,0},nCount=1,nRate=15},
				{szName="§å Phæ §»ng Long Kh«i",tbProp={6,1,30528,1,0,0},nCount=1,nRate=0.0003},
				{szName="§å Phæ §»ng Long Hµi",tbProp={6,1,30530,1,0,0},nCount=1,nRate=0.0003},
				{szName="§å Phæ §»ng Long Yªu §¸i",tbProp={6,1,30531,1,0,0},nCount=1,nRate=0.0003},
				{szName="§å Phæ §»ng Long Hé UyÓn",tbProp={6,1,30532,1,0,0},nCount=1,nRate=0.0003},
				{szName="§å Phæ §»ng Long H¹ng Liªn",tbProp={6,1,30533,1,0,0},nCount=1,nRate=0.0003},
				{szName="§å Phæ §»ng Long Béi",tbProp={6,1,30534,1,0,0},nCount=1,nRate=0.0003},
				{szName="§å Phæ §»ng Long Th­îng Giíi ChØ",tbProp={6,1,30535,1,0,0},nCount=1,nRate=0.0003},
				{szName="§å Phæ §»ng Long H¹ Giíi ChØ",tbProp={6,1,30536,1,0,0},nCount=1,nRate=0.0003},
				{szName="§»ng Long Th¹ch - H¹",tbProp={6,1,30538,1,0,0},nCount=1,nRate=0.16},},
			[2] = { -- PhÇn th­ëng cè ®Þnh
				{szName="§iÓm kinh nghiÖm", nExp=10000000,},},
		}
		tbAward = HD_CFG("HD3_ST_THUONG", nil) or tbAward	-- nil = bang goc Linux o tren
		tbAwardTemplet:Give(tbAward, 1, {"KillerTask", "FinishLevel90"})
	end
end

EventSys:GetType("KillerBoss"):Reg("OnFinish", OnFinishKillerTask)

-- [3HD 25/08] JX1 goi OnRevive moi lan NPC hoi sinh (KNpc.cpp). Ban Linux khong
-- co ham nay o ActionScript nen sinh 160 loi 'attempt to call a nil value' moi dot
-- (da thay trong ScriptError.log 25/08). Ham rong = giu dung hanh vi ban Linux.
function OnRevive()
end
