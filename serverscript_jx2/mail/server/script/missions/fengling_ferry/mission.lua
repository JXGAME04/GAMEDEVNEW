Include("\\script\\tinhnang\\3hoatdong\\beidou\\bd_activity.lua")
Include("\\script\\missions\\fengling_ferry\\fld_head.lua")
Include("\\script\\event\\jiefang_jieri\\200904\\taskctrl.lua");
Include("\\script\\activitysys\\g_activity.lua")
Include("\\script\\activitysys\\playerfunlib.lua")
Include("\\script\\misc\\eventsys\\type\\func.lua")

function InitMission()
	for i = 1, 100 do 
		SetMissionV(i , 0);
	end
	worldid= SubWorldIdx2ID(SubWorld)
	ClearMapNpc(worldid)	
	ClearMapTrap(worldid) 
	ClearMapObj(worldid)
	StartMissionTimer(MISSIONID, 29, FLD_TIMER_1)		--20Ãë¼ÆÊ±
	StartMissionTimer(MISSIONID, 28, FLD_TIMER_2)		--È«³¡
end

function RunMission()
	for i=1, 30 do
		posx, posy = fld_getadata(npcthiefpos)
		local npcindex	= HD3_AddNpc(724, 95, SubWorld, posx, posy, 0, "Thñy TÆc ");
		SetNpcDeathScript(npcindex, "\\script\\missions\\fengling_ferry\\shuizeideath.lua");
	end
	idx = 0
	local nCount = GetMSPlayerCount(MISSIONID)
	for i = 1 , nCount do 
 		idx, pidx = GetNextPlayer(MISSIONID,idx, 0);

 		if (pidx > 0) then
   			PlayerIndex = pidx;
 		end
		--´ËÊ±ÎªGM·¢²¼ÃüÁî
		SetFightState(1)
		PutMessage("ThuyÒn ®i råi! 30 phót sau sÏ ®Õn bê B¾c Phong L¨ng §é.")
		if (idx == 0) then 
 			break
 		end 
 	end
 	JiluAttendCount()		-- Êı¾İÂñµãµÚÒ»ÆÚ
end

function JiluAttendCount()
	-- µÃµ½µ±Ç°±¨ÃûµÄÈËÊı 
 	local nCount = GetMSPlayerCount(MISSIONID)
	local mapid = SubWorldIdx2ID(SubWorld)
	if (mapid == 337) then
		AddStatData("fld_chuan1canjiarenshu", nCount)
	elseif (mapid == 338) then
		AddStatData("fld_chuan2canjiarenshu", nCount)
	elseif (mapid == 339) then
		AddStatData("fld_chuan3canjiarenshu", nCount)
	end
end

function EndMission()
	StopMissionTimer(MISSIONID, 28)
	StopMissionTimer(MISSIONID ,29)
	Landing()
	for i = 1, 100 do 
		SetMissionV(i , 0);
	end
end

function OnLeave(RoleIndex)
	oldPlayerIndex = PlayerIndex
	PlayerIndex = RoleIndex
	Msg2MSAll(MISSIONID, GetName().."B¹n kh«ng may tö vong trong lóc ®i thuyÒn.")
	SetCreateTeam(1)
	ForbidEnmity(0);
--	SetTaskTemp(200,0);
	SetDeathScript("")
	SetFightState(0)		
	ForbidChangePK(0)
	SetPKFlag(0)
	PlayerIndex = oldPlayerIndex
end

function Landing()
	Msg2MSAll(MISSIONID, "®· ®Õn bê B¾c Phong L¨ng §é.")
	
	local nDate = tonumber(GetLocalDate("%Y%m%d"));		-- by bel µ½°¶ÒÔºó·¢³ö¡°ÏûÃğË®Ôô¡±»î¶¯µÄÁì½±¹«¸æ
	if (nDate >= jf0904_act_dateS and nDate < jf0904_act_dateE) then
		local szNews = "Quan phñ vµ c¸c cao thñ ®· hoµn tÊt chiÕn dŞch tiªu diÖt Thñy TÆc, h·y nhanh chãng giao [Truy C«ng LÖnh] cho ThuyÒn Phu ®Ó nhËn th­ëng!";
		AddGlobalNews(szNews);
	end
	
	local tbPlayer = {}
	idx = 0
	local nCount = GetMSPlayerCount(MISSIONID)
	for i = 1 , nCount do 
		idx, pidx = GetNextPlayer(MISSIONID,idx, 0)
		
		if (pidx > 0) then
   			tbPlayer[i] = pidx;
 		end
 		
		if (idx == 0) then 
	 		break
	 	end
	 	
	end
	
	worldid= SubWorldIdx2ID(SubWorld)
	local bShuizeiTask = check_new_shuizeitask()
	
	G_ACTIVITY:OnMessage("FinishFengLingDu",tbPlayer)
	EventSys:GetType("FengLingDu"):OnEvent("OnLanding", tbPlayer, bShuizeiTask)
	
 	for i= 1, getn(tbPlayer) do 
	 	PlayerIndex = tbPlayer[i]
		-- [BAC DAU 25/08] Linux beidouactivity.lua:404-420 OnLanding: nguoi choi
		-- PHAI DANG MANG Truy Cong Lenh moi duoc lenh bai Bac Dau (KHONG bi tru).
		-- bShuizeiTask = 1 (khung gio ton phi) -> lenh bai Phong Lang Do cap 2.
		if (HD3_BD_PhongLangDo ~= nil) then
			HD3_BD_PhongLangDo(bShuizeiTask)
		end
		camp = GetCamp()
		SetCurCamp(camp)
		SetCreateTeam(1)
		SetDeathScript("")
--		SetTaskTemp(200, 0)
		ForbidEnmity(0);
		
		if (bShuizeiTask == 1) then
			-- [MAIL 03/09 D9] thuong cap ben gui qua thu (khong can cho trong tui)
			Include("\\script\\mail\\mailmanager.lua")
			MailManager_SendRewardTemplet("phonglangdo", nil, "Th­ëng cËp bÕn Phong L¨ng §é", "§¹i hiÖp ®· cËp bÕn an toµn, phÇn th­ëng ®İnh kÌm trong th­.<enter>Tr©n träng", {{tbProp = {6, 1, 3361, 1, 0, 0}, nCount = HD_CFG("HD3_PLD_THUONG_CAPBEN", 2)}}, 30)
			Msg2Player("PhÇn th­ëng cËp bÕn Phong L¨ng §é ®· göi vµo hép th­.")
		end
		
		local mapid = SubWorldIdx2ID(SubWorld)
		if (mapid == 337) then
			SetLogoutRV(0)
			NewWorld(fld_landingpos(1))
			SetFightState(1)
			DisabledUseTownP(1) -- ÏŞÖÆÆä½øÈë·çÁê¶É±±°¶Ê¹ÓÃ»Ø³Ç·û
			SetRevPos(175,1)	--Éè¶¨ËÀÍöÖØÉúµãÎªÎ÷É½´å
		elseif (mapid == 338) then
			SetLogoutRV(0)
			NewWorld(fld_landingpos(2))
			SetFightState(1)
			DisabledUseTownP(1) -- ÏŞÖÆÆä½øÈë·çÁê¶É±±°¶Ê¹ÓÃ»Ø³Ç·û
			SetRevPos(175,1)	--Éè¶¨ËÀÍöÖØÉúµãÎªÎ÷É½´å
		elseif (mapid == 339) then
			SetLogoutRV(0)
			NewWorld(fld_landingpos(3))
			SetFightState(1)
			DisabledUseTownP(1) -- ÏŞÖÆÆä½øÈë·çÁê¶É±±°¶Ê¹ÓÃ»Ø³Ç·û
			SetRevPos(175,1)	--Éè¶¨ËÀÍöÖØÉúµãÎªÎ÷É½´å
		else
			print("error:i don't know why")
		end
		
	end
	
	ClearMapNpc(worldid)	
	ClearMapTrap(worldid) 
	ClearMapObj(worldid)
end

