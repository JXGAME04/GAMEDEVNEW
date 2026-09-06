--Author: Fong KiÒu
--Date: 2021
--Function: Trô bÞ chÕt

Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_map.lua")
Include("\\script\\tinhnang\\congthanhchien\\lib_ctc.lua")
Include("\\script\\log_game\\save_log.lua")

TRUTHANH_BINUT 						= "%s ®· xuÊt hiÖn vÕt r¹n nøt."
TRUTHANH_PHAHUY_B 			= "%s ®· bÞ <color=purple>%s<color> ph¸ huû."
TRUTHANH_PHAHUY_A 			= "%s ®· bÞ <color=green>%s<color> chiÕm gi÷ l¹i."

function OnDeath(nNpcIndex,nDamageIndex)
	local nNpcIdx
	local nType = GetNpcValue(nNpcIndex)
	local nSer = GetNpcSeries(nNpcIndex)
	local nPlayerIndex = NpcIdx2PIdx(nDamageIndex)
	if (nPlayerIndex == 0) then	-- neu la quai vat thi ngung
		return 
	end
	PlayerIndex = nPlayerIndex
	local nPlayerDataIdx = PIdx2MSDIdx(MS_CTHANHCHIEN,PlayerIndex)
	local nPhe = GetMSIdxGroup(MS_CTHANHCHIEN, nPlayerDataIdx)
	if (nPhe == 2) then -- phe c«ng
		if (nType < 4) then -- lÇn ®Çu trô thµnh cßn nguyªn add trô thµnh nøt
			nNpcIdx = AddNpcEx3({531},1,{nSer},ID_MAP_CTC,DATA_TRUTHANH[nType][2]*32,DATA_TRUTHANH[nType][3]*32,nil,DEATHFILE_TRU,DATA_TRUTHANH[nType][4],1,0,250000)
			AddMSNpc(MS_CTHANHCHIEN,nNpcIdx,1) --add vµo group 1 lµ trô thµnh vÉn cßn thuéc phe thñ
			SetNpcValue(nNpcIdx,DATA_TRUTHANH[nType][1]+3)--®¸nh dÊu lµ lo¹i trô nµo
			Msg2MSAll(MS_CTHANHCHIEN,format(TRUTHANH_BINUT,DATA_TRUTHANH[nType][4]))
			AddGlobalNews(format("Tin b¸o tõ c«ng thµnh chiÕn tr­êng: %s %s ®· ®¸nh nøt %s .", GetName(), "phe C«ng", DATA_TRUTHANH[nType][4]))
			return 
		end -- lÇn sau trô thµnh nøt bÞ chÕt add trô thµnh cßn nguyªn
		nNpcIdx = AddNpcEx3({530},1,{nSer},ID_MAP_CTC,DATA_TRUTHANH[nType-3][2]*32,DATA_TRUTHANH[nType-3][3]*32,nil,DEATHFILE_TRU,DATA_TRUTHANH[nType-3][4],2,0,500000)
		AddMSNpc(MS_CTHANHCHIEN,nNpcIdx,2) -- add vµo group 2 lµ trô thuéc vÒ phe c«ng
		SetNpcValue(nNpcIdx,DATA_TRUTHANH[nType-3][1])--®¸nh dÊu lµ lo¹i trô nµo
		SetMission(M_SOCOPHEB,GetMissionV(M_SOCOPHEB)+1)--t¨ng sè trô phe c«ng ®¸nh g·y
		-- DelNpc(nNpcIndex)
		DelMSNpc(MS_CTHANHCHIEN,nNpcIndex)
		Msg2MSAll(MS_CTHANHCHIEN,format(TRUTHANH_PHAHUY_B,DATA_TRUTHANH[nType-3][4],GetName()))
		local str = format("Tin b¸o tõ c«ng thµnh chiÕn tr­êng: %s %s ®· giµnh ®­îc quyÒn khèng chÕ %s .", GetName(), "phe C«ng", DATA_TRUTHANH[nType-3][4])
		AddGlobalNews(str)
		logHoatDong(str)
		local nTypeTru = DATA_TRUTHANH[nType-3][1]
		if(nTypeTru == 1) then
			SetMission(M_LONGTRULD, 		2) --set tru thuoc ve phe cong
		end
		if(nTypeTru == 2) then
			SetMission(M_LONGTRUDX, 		2) --set tru thuoc ve phe cong
		end
		if(nTypeTru == 3) then
			SetMission(M_LONGTRUBG, 		2) --set tru thuoc ve phe cong
		end
		CTCCheckIsCongWin() --check phe c«ng chiÕm ®ñ 3 trô ch­a nÕu 1 lµ win
		SetPMParam(MS_CTHANHCHIEN,nPlayerDataIdx,3,GetPMParam(MS_CTHANHCHIEN,nPlayerDataIdx,3)+1) --kill trô num
		SetPMParam(MS_CTHANHCHIEN,nPlayerDataIdx,6,GetPMParam(MS_CTHANHCHIEN,nPlayerDataIdx,6)+KILL_TRU_POINT) --®iÓm tÝch luü		
		return 
	end
	
	if (nType < 4) then --phe thñ -- lÇn ®Çu trô thµnh cßn nguyªn add trô thµnh nøt
		nNpcIdx = AddNpcEx3({531},1,{nSer},ID_MAP_CTC,DATA_TRUTHANH[nType][2]*32,DATA_TRUTHANH[nType][3]*32,nil,DEATHFILE_TRU,DATA_TRUTHANH[nType][4],2,0,250000)
		AddMSNpc(MS_CTHANHCHIEN,nNpcIdx,2) --add vµo group 2 lµ trô thµnh vÉn cßn thuéc phe c«ng
		SetNpcValue(nNpcIdx,DATA_TRUTHANH[nType][1]+3)--®¸nh dÊu lµ lo¹i trô nµo
		Msg2MSAll(MS_CTHANHCHIEN,format(TRUTHANH_BINUT,DATA_TRUTHANH[nType][4]))
		AddGlobalNews(format("Tin b¸o tõ c«ng thµnh chiÕn tr­êng: %s %s ®· ®¸nh nøt %s .", GetName(), "phe Thñ", DATA_TRUTHANH[nType][4]))
		return 
	end-- lÇn sau trô thµnh nøt bÞ chÕt add trô thµnh cßn nguyªn
	nNpcIdx = AddNpcEx3({530},1,{nSer},ID_MAP_CTC,DATA_TRUTHANH[nType-3][2]*32,DATA_TRUTHANH[nType-3][3]*32,nil,DEATHFILE_TRU,DATA_TRUTHANH[nType-3][4],1,0,500000)
	AddMSNpc(MS_CTHANHCHIEN,nNpcIdx,1)--add vµo group 1 lµ trô thµnh vÉn cßn thuéc phe thñ
	SetNpcValue(nNpcIdx,DATA_TRUTHANH[nType-3][1])--®¸nh dÊu lµ lo¹i trô nµo
	SetMission(M_SOCOPHEA,GetMissionV(M_SOCOPHEA)+1)--t¨ng sè trô phe thñ ®¸nh g·y
	-- DelNpc(nNpcIndex)
	DelMSNpc(MS_CTHANHCHIEN,nNpcIndex)
	Msg2MSAll(MS_CTHANHCHIEN,format(TRUTHANH_PHAHUY_A,DATA_TRUTHANH[nType-3][4],GetName()))
	local str = format("Tin b¸o tõ c«ng thµnh chiÕn tr­êng: %s %s ®· giµnh ®­îc quyÒn khèng chÕ %s .", GetName(), "phe Thñ", DATA_TRUTHANH[nType-3][4])
	AddGlobalNews(str)
	logHoatDong(str)
	local nTypeTru = DATA_TRUTHANH[nType-3][1]
	if(nTypeTru == 1) then
		SetMission(M_LONGTRULD, 		1) --set tru thuoc ve phe thu
	end
	if(nTypeTru == 2) then
		SetMission(M_LONGTRUDX, 		1) --set tru thuoc ve phe thu
	end
	if(nTypeTru == 3) then
		SetMission(M_LONGTRUBG, 		1) --set tru thuoc ve phe thu
	end
	SetPMParam(MS_CTHANHCHIEN,nPlayerDataIdx,3,GetPMParam(MS_CTHANHCHIEN,nPlayerDataIdx,3)+1) --kill trô num
	SetPMParam(MS_CTHANHCHIEN,nPlayerDataIdx,6,GetPMParam(MS_CTHANHCHIEN,nPlayerDataIdx,6)+KILL_TRU_POINT) --®iÓm tÝch luü	
end

function OnRevive(nNpcIndex)
	--SetNpcBoss2(nNpcIndex,5)
end