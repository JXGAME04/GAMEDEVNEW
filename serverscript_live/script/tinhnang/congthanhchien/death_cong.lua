--Author: Fong Ki“u
--Date: 2021
--Function: CÊng Thµnh bﬁ ch’t

Include("\\script\\lib\\lib_map.lua")
Include("\\script\\tinhnang\\congthanhchien\\lib_ctc.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\log_game\\save_log.lua")

CONGTHANH_BINUT = "%s Æ∑ xu t hi÷n v’t rπn n¯t."
CONGTHANH_PHAHUY = "%s Æ∑ bﬁ phe c´ng ph∏ hÒy."

function OnDeath(nNpcIndex,nDamageIndex)
	local nNpcIdx
	local nType = GetNpcValue(nNpcIndex)
	local nSer = GetNpcSeries(nNpcIndex)
	local nPlayerIndex = NpcIdx2PIdx(nDamageIndex)
	if (nPlayerIndex == 0) then	-- neu la quai vat thi ngung
		return 
	end

	if (nType < 4) then	
		nNpcIdx = AddNpcEx3({533},1,{nSer},ID_MAP_CTC,DATA_CONGTHANH[nType][2]*32,DATA_CONGTHANH[nType][3]*32,nil,DEATHFILE_CONG,DATA_CONGTHANH[nType][4],1,0,250000)
		AddMSNpc(MS_CTHANHCHIEN,nNpcIdx,1)
		SetNpcValue(nNpcIdx,DATA_CONGTHANH[nType][1]+3)
		Msg2MSAll(MS_CTHANHCHIEN,format(CONGTHANH_BINUT,DATA_CONGTHANH[nType][4]))
		local str = "Theo tin tı chi’n tr≠Íng Æ≠a v“, cÊng thµnh "..DATA_CONGTHANH[nType][4].." cÒa phe thÒ Æ∑ bﬁ "..GetName().." Æ∏nh n¯t, t◊nh th’ r t nguy c p."
		AddGlobalNews(str)
		logHoatDong(str)
		return 
	end
	--DelNpc(nNpcIndex)
	DelMSNpc(MS_CTHANHCHIEN,nNpcIndex)
	SetMission(DATA_CONGTHANH[nType-3][5],1) --cÊng thµnh bﬁ vÏ
	Msg2MSAll(MS_CTHANHCHIEN,format(CONGTHANH_PHAHUY,DATA_CONGTHANH[nType-3][4]))
	local str = "Theo tin tı chi’n tr≠Íng Æ≠a v“, cÊng thµnh "..DATA_CONGTHANH[nType-3][4].." cÒa phe thÒ Æ∑ bﬁ "..GetName().." ph∏ hu˚, t◊nh th’ r t nguy c p."
	AddGlobalNews(str)
	logHoatDong(str)
end

function OnRevive(nNpcIndex)
	--SetNpcBoss2(nNpcIndex,3)
end