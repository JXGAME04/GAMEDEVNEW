-------------------------------------------------------------------
-- Filename	: Qu¶ Huy Hoµng
-- Author	: DzungDolby
-- Date		: 2021
-------------------------------------------------------------------

Include("\\script\\lib\\lib_task.lua")
Include("\\script\\log_game\\save_log.lua")
Include("\\script\\event\\event_huyhoang_dungdb\\lib_huyhoang.lua")

function main(nNpcIdx)
	
	--dofile("script/event/event_huyhoang_dungdb/quahuyhoang.lua")
	
	local nPlayerLevel = GetLevel()
	local nType = GetNpcParam(nNpcIdx, 1)
	if ( GetCamp() == 0 ) then	--KiÓm tra gia nhËp m«n ph¸i
		Talk(1,"","<color=green>Qu¶ Huy Hoµng<color> : Ng­¬i ch­a gia nhËp m«n ph¸i kh«ng thÓ h¸i qu¶ !")
		return
	end
	if ( GetFightState() == 0 or GetLife(0) <= 0 ) then	--KiÓm tra tr¹ng th¸i chiÕn ®Êu
		Talk(1,"","<color=green>Qu¶ Huy Hoµng<color> : Kh«ng thÓ h¸i qu¶ !")
		return
	end
	if (nType == 1) then
		if (nPlayerLevel >= 80) then
			Talk(1,"","<color=green>Qu¶ Huy Hoµng<color>: Lo¹i qu¶ nµy chØ nh÷ng ng­êi d­íi cÊp 80 míi cã thÓ h¸i !")
			return
		end
	end	
	if (nType == 2) then
		if (nPlayerLevel < 80 or nPlayerLevel >= 200) then
			Talk(1,"","<color=green>Qu¶ Huy Hoµng<color>: Lo¹i qu¶ nµy chØ nh÷ng ng­êi tõ cÊp 80 trë lªn míi cã thÓ h¸i !")
			return 
		end
	end	
	if  (nType == 3) then
		if (nPlayerLevel < 120) then
			Talk(1,"","<color=green>Qu¶ Huy Hoµng<color>: Lo¹i qu¶ nµy chØ nh÷ng ng­êi tõ cÊp 120 míi cã thÓ h¸i !")
			return 
		end
	end	
	if  (nType == 4) then
		if (nPlayerLevel < 120) then
			Talk(1,"","<color=green>Qu¶ Hoµng Kim<color>: Lo¹i qu¶ nµy chØ nh÷ng ng­êi tõ cÊp 120 míi cã thÓ h¸i !")
			return 
		end
	end
	if(nType == 4) then
		if (GetTask(TSK_HAI_QUA_HK_NGAY) > MAX_COLLECT_PER_DAY_HK) then 
			Talk(1,"","<color=green>Qu¶ Hoµng Kim<color>  : Mçi ngµy chØ h¸i ®­îc tèi ®a "..MAX_COLLECT_PER_DAY_HK.." qu¶ !")
			return
		end
	else
		if (GetTask(TSK_HAI_QUA_HH_NGAY) > MAX_COLLECT_PER_DAY_HH) then 		
			Talk(1,"","<color=green>Qu¶ Huy Hoµng<color>  : Mçi ngµy chØ h¸i ®­îc tèi ®a "..MAX_COLLECT_PER_DAY_HH.." qu¶ !")
			return
		end	
	end
	PaceBar("§ang thu ho¹ch...", TIME_DELAY_TH, "FnThuHoach("..nNpcIdx..")")
end

function FnThuHoach(nNpcIdx)
	
	if(GetFightState() == 0) then 
		Talk(1,"","§ang ë chÕ ®é phi chiÕn ®Êu kh«ng thÓ nhÆt")
		return
	end --tr¹ng th¸i kh«ng chiÕn ®Êu, kh«ng pk kh«ng nhËn ®­îc trèng
	if(GetPKState() == 0) then
		Talk(1,"","§ang ë chÕ ®é luyÖn c«ng kh«ng thÓ nhÆt")
		return 
	end 
	if(GetLife(0) <= 0) then 
		Talk(1,"","Sinh lùc kh«ng ®ñ kh«ng thÓ nhÆt")
		return 
	end 

	if(FindAroundNpc(GetNpcID(nNpcIdx)) <= 0) then
		Talk(1,"","ThËt ®¸ng tiÕc qu¶ ®· bÞ ng­êi kh¸c h¸i mÊt. ")
		return
	end
				
	local nLoaiQuaHuyHoang  = GetNpcParam(nNpcIdx, 1)
	local itemIdx
	local msg = ""
	if (nLoaiQuaHuyHoang > 0 ) then		-- KiÓm tra lo¹i qu¶
		if (nLoaiQuaHuyHoang == 1) then
			Talk(1,"","<color=green>Qu¶ Huy Hoµng<color> : Chóc mõng §¹i HiÖp ®· thu ho¹ch ®­îc Qu¶ Huy Hoµng TiÓu !")
			itemIdx = AddItem(6,1,ID_QHH_THAP,0,0,0,0)--qua tieu
			AddTimeItem(itemIdx, TG_SD_QUAHH)
			msg = format("%s ®· h¸i ®­îc Qu¶ Huy Hoµng. ", GetName())
			SetTask(TSK_HAI_QUA_HH_NGAY, GetTask(TSK_HAI_QUA_HH_NGAY) + 1)
		elseif (nLoaiQuaHuyHoang == 2) then
			Talk(1,"","<color=green>Qu¶ Huy Hoµng<color> : Chóc mõng §¹i HiÖp ®· thu ho¹ch ®­îc Qu¶ Huy Hoµng Trung !")
			itemIdx = AddItem(6,1,ID_QHH_TR,0,0,0,0)--qua trung
			AddTimeItem(itemIdx, TG_SD_QUAHH)
			msg = format("%s ®· h¸i ®­îc Qu¶ Huy Hoµng. ", GetName())	
			SetTask(TSK_HAI_QUA_HH_NGAY, GetTask(TSK_HAI_QUA_HH_NGAY) + 1)
		elseif (nLoaiQuaHuyHoang ==3) then
			Talk(1,"","<color=green>Qu¶ Huy Hoµng<color> : Chóc mõng §¹i HiÖp ®· thu ho¹ch ®­îc Qu¶ Huy Hoµng §¹i !")
			itemIdx = AddItem(6,1,ID_QHH_C,0,0,0,0)--qua dai
			AddTimeItem(itemIdx, TG_SD_QUAHH)	
			msg = format("%s ®· h¸i ®­îc Qu¶ Huy Hoµng. ", GetName())
			SetTask(TSK_HAI_QUA_HH_NGAY, GetTask(TSK_HAI_QUA_HH_NGAY) + 1)
		elseif (nLoaiQuaHuyHoang ==4) then
			Talk(1,"","<color=green>Qu¶ Hoµng Kim<color> : Chóc mõng §¹i HiÖp ®· thu ho¹ch ®­îc Qu¶ Hoµng Kim !")
			itemIdx = AddItem(6,1,ID_QHK,0,0,0,0)--qua hoang kim
			AddTimeItem(itemIdx, TG_SD_QUAHK)	
			msg = format("Chóc mõng <color=yellow>%s<color> ®· h¸i ®­îc Qu¶ Hoµng Kim. ", GetName())
			SetTask(TSK_HAI_QUA_HK_NGAY, GetTask(TSK_HAI_QUA_HK_NGAY) + 1)
		end		
		DelNpc(nNpcIdx)
		Msg2SubWorld(msg)
		logHoatDong(msg)
	end
end

function OnTimer(nNpcIdx)
	DelNpc(nNpcIdx)
end
