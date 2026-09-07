-- 
-- by 
-- 2007.10.24
-- ÎÒ..
-- ..
-- ..

Include("\\script\\missions\\yandibaozang\\npc.lua")
Include("\\script\\missions\\yandibaozang\\include.lua")
Include("\\script\\missions\\yandibaozang\\saizi.lua")
Include("\\script\\missions\\yandibaozang\\readymap\\include.lua")
Include("\\script\\activitysys\\g_activity.lua")
Include("\\script\\activitysys\\playerfunlib.lua")
Include("\\script\\lib\\awardtemplet.lua")
Include("\\script\\missions\\yandibaozang\\doubleexp.lua")
Include("\\script\\misc\\eventsys\\type\\func.lua")
Include("\\script\\activitysys\\config\\1005\\partysupport.lua")
Include("\\script\\vng_event\\thapnienlenhbai\\mainfuc.lua")
Include("\\script\\lib\\lib_ham.lua")	-- [KM 28/08] AddItemSL

function YDBZ_award(index,ntype,nplayindex)
	local x, y, world = GetNpcPos(index);
	for x1,y1 in pairs(YDBZ_tbaward_item[ntype]) do
		for i=1,y1[2] do
			local p = random(1,100)
			if p <= y1[1] then
				if y1[4] == 0 then
					-- [DROPEX 31/08] chu ky Linux DropItem(subworld,x,y,belonger,genre,...)
					-- (jx_linux_y 0x081200B0: arg1 imul sizeof KSubWorld, arg2/3 = X/Y MPS, arg4
					-- = belonger) KHAC ham DropItem cua JX1 (tha theo NPC index, ScriptFuns.cpp
					-- :4505) nen arg2 = toa do X bi doc thanh GENRE => moi lan boss ai chet lai
					-- spam "AddItemSet2 khong tim thay nItemGenre=59082" (59082 = toa do X cua
					-- boss) va Ngu Hoa Ngoc Lo Hoan KHONG BAO GIO roi. Doi sang DropItemEx = ban
					-- port trung thanh cua ham tha-theo-TOA-DO cua Linux (KJx2WarInfra.cpp:1596),
					-- giu belonger = -1 (do roi CHUNG, ai cung nhat duoc - dung y ban Linux;
					-- DropItem cua JX1 ep belong = nguoi giet nen khong the thay the). Ba so 0
					-- chen giua la nVersion/szRandSeed/nQuality (JX1 bo qua) - dung khuon da song
					-- trong cay o tong\collectgoods\npcpoint.lua:126.
					DropItemEx(world, x, y, -1, 0, 0, 0, y1[3][1], y1[3][2], y1[3][3], y1[3][4], y1[3][5], y1[3][6], 0, 0, 0, 0, 0, 0);
				elseif y1[4] == 1 then
						local idx= 0
						local pidx,nj
						
						local oldplayer = PlayerIndex
						PlayerIndex = nplayindex
						YDBZ_DiceDice(y1[3][1],y1[3][2],y1[3][3],20,y1[6])
						PlayerIndex = oldplayer	
				end
			end
		end
	end
end

function YDBZ_fun_award_byCount(nCount, szMsg)
	nCount = nCount or 0
	if nCount <= 0 then
		return 
	end
	local tbAward = {}
	tbAward[1] = YDBZ_tbaward_item_ex[1]
	
	szMsg = szMsg or ""
	local szLogTitle = format("%s award",szMsg)
	tbAwardTemplet:Give(tbAward, nCount, {"YDBZ", szLogTitle})
end

-- 2011.03.23 1
function YDBZ_fun_award_ydmibao(camp, nnpcway)
	local idx= 0
	local nCount = 1
	local pidx,nj, nTeamSize = GetTeamSize()
	for nj = 1, 10 do
		idx , pidx = GetNextPlayer(YDBZ_MISSION_MATCH, idx,camp );
		if (pidx > 0 and nCount >= 1) then
			local sMsg = format("V­ît qua ¶i thø %d",nnpcway)
			G_ACTIVITY:OnMessage("YDBZguoguan", pidx, nnpcway);
			-- [KM 28/08] chuan 1001 config[9] YDBZguoguan{6}: qua ai 6 -> 6 CND (dai)
			if (nnpcway == 6) then
				CallPlayerFunction(pidx, KM_ThuongViemDeAi6)
			end
			EventSys:GetType("YanDiBaoZang"):OnPlayerEvent("OnPass", pidx, nnpcway)
			if (mod(nnpcway,2) == 0) then
				CallPlayerFunction(pidx,YDBZ_fun_award_byCount, nCount, sMsg)
			end
		end
		if (idx == 0) then
			break
		end
	end
end

function YDBZ_fun_award_exp(camp,ns)
	local idx= 0
	local pidx,nj
	local oldplayer = PlayerIndex
	for nj = 1, 10 do
		idx , pidx = GetNextPlayer(YDBZ_MISSION_MATCH, idx,camp );
		if (pidx > 0) then
				PlayerIndex = pidx
				if ns == 1 then
					local nexp = YDBZ_AWARD_EXP
					local szdstr = ""
					if YDBZ_sdl_getTaskByte(YDBZ_ITEM_YANDILING,1) == 1 then
						 nexp = YDBZ_AWARD_EXP * 2 
						 szdstr = "<color=yellow>[Viªm §Õ LÖnh cã hiÖu lùc]<color>"
					end
					nexp = YDBZ_checkdoubleexp(nexp)
					AddOwnExp(nexp)
					Msg2Player(format("%s chóc mõng v­ît ¶i thø nhÊt, thu ®­îc <color=yellow>%s<color> kinh nghiÖm",szdstr,nexp))
				elseif ns == 2 then
					local nexp = YDBZ_Faninl_AWARD_EXP
					local szdstr = ""
					if YDBZ_sdl_getTaskByte(YDBZ_ITEM_YANDILING,1) == 1 then
						 nexp = YDBZ_Faninl_AWARD_EXP * 2 
						 szdstr = "<color=yellow>[Viªm §Õ LÖnh cã hiÖu lùc]<color>"
					end
					nexp = YDBZ_checkdoubleexp(nexp)
					AddOwnExp(nexp)
					Msg2Player(format("%s chóc mõng tæ ®éi ®· giÕt thµnh c«ng 1 boss cã né khÝ, thu ®­îc <color=yellow>%s<color> kinh nghiÖm",szdstr,nexp))					
				end
		end
		if (idx == 0) then
			break;
		end;
	end
	PlayerIndex = oldplayer
end
--
function YDBZ_savepaihang(camp)
	
	local teamname = GetMissionS(camp)
	local nstate = GetMissionV(YDBZ_VARV_STATE)
	local laddertime = YDBZ_LIMIT_FINISH - ( ((nstate-4)* YDBZ_LIMIT_BOARDTIME) + floor(GetMSRestTime(YDBZ_MISSION_MATCH,YDBZ_TIMER_MATCH)/18) )
	local bfind = 0
	--print(":"..laddertime.." state:"..nstate.."relast:"..GetMSRestTime(YDBZ_MISSION_MATCH,YDBZ_TIMER_MATCH))
	local nteamsum = GetMissionV(YDBZ_TEAM_SUM)
	if nteamsum < 3 then
		return laddertime
	end
	if (laddertime > YDBZ_LIMIT_FINISH or laddertime < 0) then
		return laddertime
	end
	for i = 1, 10 do 
		name , value = Ladder_GetLadderInfo(YDBZ_PAIHANG_ID, i)
		if (name  == teamname) then
			bfind = 1
			if ( laddertime > value) then
				Ladder_NewLadder(YDBZ_PAIHANG_ID, teamname, laddertime,1)
				break
			end
		end
	end
	
	if (bfind == 0) then
		Ladder_NewLadder(YDBZ_PAIHANG_ID, teamname, laddertime, 1)
	end
	return laddertime
end

-- todo
-- 40
function YDBZ_award_finalboss_ex(camp, nTotalCount)
	local nNum = {}
	local nSize = GetMSPlayerCount(YDBZ_MISSION_MATCH, camp)
	for i = 1, nSize do
		nNum[i] = 1
	end
	
	for i = nSize + 1, nTotalCount do                     
		local p = random(1,nSize)
		nNum[p] = nNum[p] + 1
	end
	print("Nh©n Sè ®éi ngò =" .. nSize)
	local idx, pidx
	for nj = 1, nSize do
		idx , pidx = GetNextPlayer(YDBZ_MISSION_MATCH, idx, camp );
		if (pidx > 0 and nNum[nj] >= 1) then
			local szMsg = format("V­ît qua ¶i cuèi cïng")
			print(format("pidx = %d, nNum[nj] = %d", pidx, nNum[nj]))
			CallPlayerFunction(pidx,YDBZ_fun_award_byCount, nNum[nj], szMsg)
		end
		if (idx == 0) then
			break
		end
	end
	
end

function YDBZ_award_finalboss(camp)
	local idx= 0
	local pidx,nj
	local nplaynum = 0
	local tbAward = YDBZ_tbaward_item[4]
	local szLogTitle = "GietBossCuoiTinhNangViemDe"
	local oldplayer = PlayerIndex
	for nj = 1, 10 do
		idx , pidx = GetNextPlayer(YDBZ_MISSION_MATCH, idx,camp );
		if (pidx > 0) then
			PlayerIndex = pidx
			tbAwardTemplet:Give(tbAward, 1, {"YDBZ", szLogTitle})
			nexp = YDBZ_checkdoubleexp(20e6)
			AddOwnExp(nexp)
			Msg2Player(format("<color=yellow>Chóc mõng hoµn thµnh v­ît ¶i viªm ®Õ, thu ®­îc <color>%s kinh nghiÖm<color=yellow>.<color>",nexp))
		end
		if (idx == 0) then
			break;
		end;
	end
	PlayerIndex = oldplayer
	SetMissionV(YDBZ_STATE_SIGN,4)
	local pname = GetMissionS(camp)
	local nmapid = SubWorldIdx2ID(SubWorld)
	broadcast(format("Chóc mõng tæ ®éi %s ®· v­ît ¶i viªm ®Õ thµnh c«ng, thu ®­îc phÇn th­ëng v« cïng quý gi¸.",pname))
	local ntime = YDBZ_savepaihang(camp)
	local szstr = format("[V­ît ¶i b¶o tµng viªm ®Õ] thêi gian %s, chiÕn ®éi %s cßn l¹i sè ng­êi %s, sè hiÖu b¶n ®å %s, trËn doanh lµ %s giÕt BOSS cuèi, d­ thêi gian %s gi©y",GetLocalDate("%y-%m-%d %H:%M:%S"),pname,nplaynum,nmapid,camp,ntime) 
	WriteLog(szstr)
	StopMissionTimer(YDBZ_MISSION_MATCH, YDBZ_TIMER_MATCH);	
	StartMissionTimer(YDBZ_MISSION_MATCH,YDBZ_TIMER_FIGHTSTATE,YDBZ_TIME_WAIT_STATE3 * 18)

end
--1	1 -10 ABoss
--1 11-20 BBoss
--1 21-30 CBoss
--1 40		 10boss
--1 50		 boss
--2 1-10		 A
--2 11-20		 B
--2 21-30		 C
function OnDeath(index)
	local oldworld = SubWorld
	local x, y,world = GetNpcPos(index);
	SubWorld = world
	local nband = GetNpcParam(index,1)
	local nband2 = GetNpcParam(index,2)
	--print("mon npc dead! param:"..nband)
	--print("mon npc dead! param2:"..nband2)
	local npccount = 0
	if nband == 0 then												--
		local nteam = floor((nband2-1)/10)+1		--
		local npctype = nband2 - 10*(nteam-1)		--
		local npcallsum = 0
		--print(nteam,npctype,YDBZ_NPC_ATTRIDX_COUNT,world,YDBZ_NPC_COUNT[nteam])
		npcallsum = YDBZ_map_npc[npctype][1][YDBZ_NPC_ATTRIDX_COUNT]

		local npccount = GetMissionV(YDBZ_NPC_COUNT[nteam])
		local npcway = GetMissionV(YDBZ_NPC_WAY[nteam])

		--print(npcallsum,npccount,nteam)
		if npccount + 1 == npcallsum then
			SetMissionV(YDBZ_NPC_COUNT[nteam],0)
			local file = YDBZ_map_posfiles[1][nteam][npcway+1]
			YDBZ_add_npc(file,nteam,npcway+1,2)
			if npcway + 1 == 3 or npcway + 1 == 6 then
				YDBZ_add_npc(file,nteam,npcway+1,21)
				SetMissionV(YDBZ_NPC_COUNT[nteam],2)
			else
				SetMissionV(YDBZ_NPC_COUNT[nteam],1)
			end
		else
			SetMissionV(YDBZ_NPC_COUNT[nteam],(npccount + 1))
		end

	elseif nband >= 1 and nband <= 30 then	--10boss		
			local nteam = (floor((nband-1)/10))+1					--
			local npcway = nband - 10*(nteam-1)			--¹Ø
			
			local nCount = GetMissionV(YDBZ_NPC_COUNT[nteam])
			if nCount == 1 then
				SetMissionV(YDBZ_NPC_COUNT[nteam], 0)
				SetMissionV(YDBZ_NPC_WAY[nteam],npcway)
			if npcway == 10 then
				if GetMissionV(YDNZ_STATE_SIGN) ~= 0 then
					Msg2MSAll(YDBZ_MISSION_MATCH,"§· b­íc vßa giai ®o¹n tranh ®o¹t, cöa ¶i ®· ®ãng.")
					return 
					end
				end
			--print("teams:"..nteam.." way:"..npcway)
			local mapfile = YDBZ_mapfile_trap[nteam][4].."trap"..npcway..".txt"
			YDBZ_bt_clearzhangai(mapfile)
			YDBZ_fun_award_exp(nteam,1)
			
			YDBZ_fun_award_ydmibao(nteam,npcway)	-- 2011.03.23 
			YDBZ_award(index,2,PlayerIndex)
			local teamname = GetMissionS(YDBZ_TEAM_NAME[nteam])
			local szstr = format("<color=yellow>%s<color>®· thµnh c«ng v­ît ¶i <color=yellow>thø %s<color>.",teamname,npcway)
			if npcway == 10 then
				szstr = szstr .. "TiÕn vµo n¬i s©u nhÊt cña s¬n ®éng."	
				else				
					local file = YDBZ_map_posfiles[1][nteam][npcway+1]
					YDBZ_add_npc(file,nteam,npcway+1,1)
				end
				Msg2MSAll(YDBZ_MISSION_MATCH,szstr)
			else
				YDBZ_fun_award_exp(nteam,1)
				YDBZ_award(index,2,PlayerIndex)
				SetMissionV(YDBZ_NPC_COUNT[nteam], nCount-1) 
			end
			
	elseif nband == 40 then							--10boss
			npccount = GetMissionV(YDBZ_NPC_BOSS_COUNT)
			local nteam = GetTmpCamp()
			SetMissionV(YDBZ_NPC_BOSS_COUNT,npccount-1)
			YDBZ_award(index,3,PlayerIndex)
			YDBZ_fun_award_exp(nteam,2)
			if npccount-1 == 0 then
				Msg2MSAll(YDBZ_MISSION_MATCH,"V­ît ¶i b¶o tµng viªm ®Õ ®· tiÕn vµo <color=yellow>giai ®o¹n tranh ®o¹t<color>, <color=yellow>10 gi©y<color> sau sÏ triÖu gäi tÊt c¶ thµnh viªn trong tæ ®éi vµo b¶n ®å tranh ®o¹t, chØ cßn <color=yellow>1 tæ ®éi<color> míi cã thÓ triÖu gäi BOSS cuèi.")
				SetMissionV(YDBZ_STATE_SIGN,1)
				for i=1,3 do 
					local trapfile = YDBZ_mapfile_trap[i][4].."trap10.txt"
					YDBZ_bt_addZhangai(trapfile)
				end
				StartMissionTimer(YDBZ_MISSION_MATCH,YDBZ_TIMER_FIGHTSTATE,YDBZ_TIME_WAIT_STATE1*18)
			end

	elseif nband == 50 then							--boss
			nteam = GetTmpCamp()
			YDBZ_award_finalboss(nteam)
			G_ACTIVITY:OnMessage("YDBZ_KillMaxBoss", YDBZ_MISSION_MATCH, nteam);
			EventSys:GetType("YanDiBaoZang"):OnEvent("OnLastBigBossDeath", index, PlayerIndex)
	end
	SubWorld = oldworld
end


-- [KM 28/08] thuong nguyen lieu kinh mach khi vuot ai 6 (chay trong ngu canh nguoi choi)
function KM_ThuongViemDeAi6()
	AddItemSL(4847, 6, 0)
	Msg2Player("V­ît ¶i 6 Viªm §Õ B¶o T¹ng: nhËn 6 Ch©n Nguyªn §¬n (®¹i).")
end
