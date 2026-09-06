-- [MA2 29/08] nan ma vat pham con sot cua ban Linux (doi chieu TEN bang magicscript cua du an) - xem ReverseTools\viemde\v26_ma_consot.py
IL("DICEITEM")
Include("\\script\\missions\\yandibaozang\\include.lua")

function YDBZ_DiceDice(ng,gd,np,ntime,double)
	-- [XX 29/08] gom danh sach nguoi duoc moi gieo (xem chu thich duoi).
	local tbNhan = {}
	local nCampXX = GetTmpCamp()
	local nIdxXX = 0
	local nPidXX
	if nCampXX ~= nil and nCampXX > 0 then
		for i=1, 10 do
			nIdxXX, nPidXX = GetNextPlayer(YDBZ_MISSION_MATCH, nIdxXX, nCampXX)
			-- [LOCBOT 31/08] chi moi NGUOI THAT - dung dieu kien ban Linux (Linux moi
			-- bang GetTeamSize/GetTeamMember nen chi co nguoi that; JX1 phai doi sang
			-- GetNextPlayer theo phe vi ca hai ban deu LeaveTeam truoc tran, nhung
			-- duong nay gom ca bot). Bot khong bao gio bam nut nen phien khong bao gio
			-- chot som, phai cho het 20 giay va sinh dong "da xuc xac 0 diem" cho tung
			-- bot; nguoi that bam o giay cuoi thi phien da chot, goi tin bi bo im lang.
			if nPidXX ~= nil and nPidXX > 0 and (IsBot == nil or IsBot(nPidXX) ~= 1) then
				tinsert(tbNhan, nPidXX)
			end
			if nIdxXX == 0 then
				break
			end
		end
	end
	if getn(tbNhan) == 0 then
		tbNhan[1] = PlayerIndex
	end
	local nRet = ApplyItemDice(1, 100,ntime, "\\script\\missions\\yandibaozang\\saizi.lua", "YDBZ_OnTimeOver", "", getn(tbNhan))
	--print(nRet)
	local nBody = 0
	AddDiceItemInfo(nRet,0,ng,gd,np,1,double,1,1,1,1,1)
	local nPlayerOld = PlayerIndex
	-- [XX 29/08] TRONG TRAN NGUOI CHOI DA ROI TO DOI (readymap\ready.lua:251
	-- LeaveTeam) nen GetTeamSize() = 0 (ScriptFuns.cpp:2860) => vong cu khong
	-- moi duoc AI gieo => het gio chot 0 nguoi => Hinh nhan bi thu hoi, khong
	-- ai nhan. Gom nguoi CUNG PHE trong mission - dung khuon cua chinh tinh
	-- nang (npc_death.lua:62/:86/:186). PlayerIndex luc nay la nguoi vua ket
	-- lieu (npc_death.lua:34 dat truoc khi goi) nen GetTmpCamp() ra dung phe.
	for i=1, getn(tbNhan) do
		PlayerIndex = tbNhan[i]
		RollItem(nRet)
	end
	PlayerIndex = nPlayerOld
end

function YDBZ_OnTimeOver(dwID, nWinner, nNumber)
	--print("TimeOver!!")
	YDBZ_show_roll_info(dwID, nWinner, nNumber)
end

function YDBZ_show_msg(list, msg)
	for i=1, getn(list) do
		-- [VASAIZI 30/08] CallPlayerFunction doi tham so 2 la MOT HAM
		-- (ScriptFuns.cpp:2232-2258 lua_pushvalue(L,2) roi rawcall).
		-- Truoc day truyen thang chuoi msg -> 'attempt to call a string value'.
		-- Dung co che san co: ham o vi tri 2, tham so tu vi tri 3.
		if (list[i] ~= nil and list[i] > 0) then
			CallPlayerFunction(list[i], Msg2Player, msg)
		end
	end
end

function YDBZ_show_roll_info(dwID, nWinner, nNumber)
	local tbPlayerList = GetItemDicePlayerList(dwID)
	local t, nSize = GetItemDiceRollInfo(dwID)
	local nItemIndex,szItem,quality,nGenre,nDetial,nPart,nLevel,nSeries = GetItemDiceItemInfo(dwID)
	local bAllGiveUp = 1
	local ndsign = 0
	local oldplayindex = PlayerIndex
	local ndouble = YDBZ_sdl_getTaskByte(YDBZ_ITEM_YANDILING,1)
	for index, value in pairs(t) do
		--print(PlayerIndex,nGenre,nDetial,nPart)
		PlayerIndex = value[5]
		if GetName() == value[1] then	-- 	--
			--print(index)
			local str = "<color=yellow>"..value[1].."<color>§æ xóc x¾c <color=yellow>"..value[2].." ®iÓm<color>."
			if value[3] == 0 then
				str = "<color=yellow>"..value[1].."<color> hñy bá"
			elseif value[3] == 2 then
				if value[4] == 1 then
					if nWinner == value[5] and nSeries == 1 and ndouble == 1 and nPart == 1614 then -- roll
						if random(1,100) < 50  then		
							str = str .. "<color=yellow>[Viªm §Õ LÖnh cã hiÖu lùc]<color>"
							ndsign = 1
						else
							str = str .. "<color=blue>[Viªm ®Õ lÖnh mÊt hiÖu lùc]<color>"
							
						end
						--log
						if nGenre == 6 and nDetial == 1 and nPart == 1615 then
							YDBZ_sdl_writeLog("V­ît ¶i b¶o tµng viªm ®Õ","Trong qu¸ tr×nh v­ît ¶i thu ®­îc 1 Viªm §Õ ®å ®»ng")
						end	
						if nGenre == 6 and nDetial == 1 and nPart == 1614 then
							YDBZ_sdl_writeLog("V­ît ¶i b¶o tµng viªm ®Õ","Trong lóc v­ît ¶i thu ®­îc 1 h×nh ném")
						end			
					end
					str = str..format(" -- nhÆt ®­îc <color=yellow>%s<color>",szItem)
				end
				bAllGiveUp = 0
			end
			YDBZ_show_msg(tbPlayerList, str)
			if ndsign == 1 then
				local ndix = AddItem(nGenre,nDetial,nPart,1,0,0)
				YDBZ_sdl_writeLog("V­ît ¶i b¶o tµng viªm ®Õ",format("Lóc v­ît ¶i thu ®­îc 1 %s",GetItemName(ndix)))
			end
		end
	end
	if bAllGiveUp == 1 then
		YDBZ_show_msg(tbPlayerList, "TÊt c¶ ng­êi ch¬i ®Òu hñy bá råi")
	elseif nSize ~= getn(t) then
		YDBZ_show_msg(tbPlayerList, "Ng­êi ch¬i kh¸c v× qu¸ h¹n thêi gian, nªn tÝnh lµ hñy bá")
	end
end

function YDBZ_sdl_writeLog(sztitle,szevent)	--,sztitle=,szevent=
	WriteLog(format("[%s]\t Date:%s\t Account:%s\t Name:%s\t %s",sztitle,GetLocalDate("%y-%m-%d %H:%M:%S"),GetAccount(),GetName(),szevent));
end