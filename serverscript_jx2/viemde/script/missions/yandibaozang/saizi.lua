IL("DICEITEM")
Include("\\script\\missions\\yandibaozang\\include.lua")

function YDBZ_DiceDice(ng,gd,np,ntime,double)
	local nRet = ApplyItemDice(1, 100,ntime, "\\script\\missions\\yandibaozang\\saizi.lua", "YDBZ_OnTimeOver", "", GetTeamSize())
	--print(nRet)
	local nBody = 0
	AddDiceItemInfo(nRet,0,ng,gd,np,1,double,1,1,1,1,1)
	local nPlayerOld = PlayerIndex
	for i=1, GetTeamSize() do
		PlayerIndex = GetTeamMember(i)
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
		CallPlayerFunction(list[i], msg)
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
	for index, value in t do
		--print(PlayerIndex,nGenre,nDetial,nPart)
		PlayerIndex = value[5]
		if GetName() == value[1] then	-- 	--
			--print(index)
			local str = "<color=yellow>"..value[1].."<color>§æ xóc x¾c <color=yellow>"..value[2].." ®iÓm<color>."
			if value[3] == 0 then
				str = "<color=yellow>"..value[1].."<color> hñy bá"
			elseif value[3] == 2 then
				if value[4] == 1 then
					if nWinner == value[5] and nSeries == 1 and ndouble == 1 and nPart == 1605 then -- roll
						if random(1,100) < 50  then		
							str = str .. "<color=yellow>[Viªm §Õ LÖnh cã hiÖu lùc]<color>"
							ndsign = 1
						else
							str = str .. "<color=blue>[Viªm ®Õ lÖnh mÊt hiÖu lùc]<color>"
							
						end
						--log
						if nGenre == 6 and nDetial == 1 and nPart == 1606 then
							YDBZ_sdl_writeLog("V­ît ¶i b¶o tµng viªm ®Õ","Trong qu¸ tr×nh v­ît ¶i thu ®­îc 1 Viªm §Õ ®å ®»ng")
						end	
						if nGenre == 6 and nDetial == 1 and nPart == 1605 then
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