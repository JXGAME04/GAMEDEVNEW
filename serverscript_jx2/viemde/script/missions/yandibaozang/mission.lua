-- 
-- by 
-- 2007.10.24
-- ÎÒ..
-- ..
-- ..


Include("\\script\\missions\\yandibaozang\\include.lua")
Include("\\script\\missions\\yandibaozang\\npc_death.lua")

---- 
function YDBZ_start_match_timer()
	-- 1
	--broadcast(",");
	StartMissionTimer(YDBZ_MISSION_MATCH, YDBZ_TIMER_MATCH, YDBZ_LIMIT_SIGNUP * 18);
end

-- 
function YDBZ_start_signup()
	-- 
	SetMissionV(YDBZ_VARV_STATE, 3);
	SetMissionV(YDBZ_STATE_SIGN,0)
	SetMissionV(YDBZ_TEAM_COUNT,0)
	SetMissionV(YDBZ_TEAM_SUM,0)
	for i=1,3 do 
		SetMissionV(YDBZ_NPC_COUNT[i],0)
		SetMissionV(YDBZ_NPC_WAY[i],0)
	end
	SetMissionV(YDBZ_NPC_BOSS_COUNT,0)
		
--	SetMissionV(YDBZ_VARV_BOARD_TIMER, 0);
--	SetMissionS(YDBZ_VARS_TEAM_NAME, "");
end

function InitMission()
	YDBZ_close_match()
	--print("mission")
	YDBZ_start_signup();
	YDBZ_start_match_timer();
	for i=1,getn(YDBZ_mapfile_trap) do
		for x =1,YDBZ_mapfile_trap[1][3] do 
			local mapfile = YDBZ_mapfile_trap[i][1].."trap"..x..".txt"
			--print(mapfile,scriptfile)
			YDBZ_bt_addZhangai(mapfile)
		end
		local mapfile = YDBZ_mapfile_trap[i][1].."trapenter.txt"
		local scriptfile = YDBZ_mapfile_trap[i][2].."trapenter.lua"
		YDBZ_bt_addtrap(mapfile,scriptfile)
	end

end

function EndMission()
	SetMissionV(YDBZ_VARV_STATE, 0);
	for i=1,3 do 
		SetMissionV(YDBZ_NPC_COUNT[i],0)
		SetMissionV(YDBZ_NPC_WAY[i],0)
	end
	SetMissionV(YDBZ_NPC_BOSS_COUNT,0)
	StopMissionTimer(YDBZ_MISSION_MATCH,YDBZ_TIMER_MATCH)
	YDBZ_close_match();

end

-- 
function OnLeave(index)

	local oldplayindex = PlayerIndex
	PlayerIndex = index
	local oldworld = SubWorld
	local world = GetMissionV(YDBZ_SIGNUP_WORLD);
	local pos_x = GetMissionV(YDBZ_SIGNUP_POSX);
	local pos_y = GetMissionV(YDBZ_SIGNUP_POSY); 

	local nteams = YDBZ_sdl_getTaskByte(YDBZ_ITEM_YANDILING,2)
	local szstr = format("<color=yellow>%s<color> rêi khái b¶n ®å b¶o tµng viªm ®Õ.",GetName())

	SetTempRevPos(world,pos_x*32,pos_y*32)
	Msg2MSAll(YDBZ_MISSION_MATCH,str);
	Msg2Player(szstr)
	YDBZ_restore(PlayerIndex,YDBZ_MISSION_MATCH,nteams)
	SubWorld = oldworld

	-- [JX1 26/08] chan nteams == 0: neu tran doanh chua duoc dat thi cau tren
	-- se dem nhom 0 (luon rong) va bao nham "ca to doi da tu tran".
	if nteams > 0 and GetMSPlayerCount(YDBZ_MISSION_MATCH,nteams) == 0 then
		local pname = GetMissionS(YDBZ_TEAM_NAME[nteams])
		--broadcast(format("%s",nteams))
		Msg2MSAll(YDBZ_MISSION_MATCH,format("Toµn bé ng­êi trong tæ ®éi <color=yellow>%s<color> ®· tö trËn trong b¶o tµng viªm ®Õ.",pname))
		SetMissionS(YDBZ_TEAM_NAME[nteams],"")
		local nteamscount = GetMissionV(YDBZ_TEAM_COUNT)
		--print(nteamscount,SubWorld)
		SetMissionV(YDBZ_TEAM_COUNT,(nteamscount-1))
	end
	-- [JX1 26/08] BAY #3: y muon hoi "con ai trong tran khong".
	-- JX1: GetMSPlayerCount(id, 0) dem RIENG NHOM 0 (LuaMissionPlayerCount:
	-- nGroupId >= 0 -> GetGroupPlayerCount). Nguoi choi luon o nhom 1..3 nen
	-- nhom 0 LUON RONG -> tran se DONG NGAY khi mot nguoi roi ban do.
	-- Bo tham so nhom thi engine dung GetPlayerCount() = tat ca.
	if GetMSPlayerCount(YDBZ_MISSION_MATCH) == 0 then
		CloseMission(YDBZ_MISSION_MATCH)
		return
	end
	if GetMissionV(YDBZ_TEAM_COUNT) == 1 and (GetMissionV(YDBZ_STATE_SIGN) == 1 or GetMissionV(YDBZ_STATE_SIGN) == 2) then

		local x = 1781 * 32
		local y = 3563 * 32 

		YDBZ_add_final_npc(SubWorld,x,y)
		Msg2MSAll(YDBZ_MISSION_MATCH,"<color=yellow>L­¬ng Mi Nhi<color> t¹i n¬i s©u th¼m nhÊt cña b¶o tµng Viªm §Õ ®· ®­îc gäi ra")
		SetMissionV(YDBZ_STATE_SIGN,3)
	end

	PlayerIndex = oldplayindex;
end
