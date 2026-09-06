-- Author: Fong KiÒu
-- Date: 28/11/2020
-- Function: Npc Cét cê

Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")

function main(nIndex)
	local nValue = GetNpcValue(nIndex)
	local nPlayerDataIdx = PIdx2MSDIdx(MS_TONGKIM,PlayerIndex)
	local nPhe = GetMSIdxGroup(MS_TONGKIM, nPlayerDataIdx)
	local nSLuong = GetMissionV(TAB_PHE_TONGKIM[nPhe][8])

	if(GetFightState() == 0) then
		Talk(1,"","<sex> ®ang ë tr¹ng th¸i phi chiÕn ®Êu kh«ng thÓ ®o¹t cê.")
		return 		
	end	
	
	if(GetItemCount(0, ITEM_CO_HIEU[1],ITEM_CO_HIEU[2],ITEM_CO_HIEU[3],-1,-1, pos_equiproom) < 1) then
		Talk(1,"","<sex> kh«ng cã cê hiÖu.")
		return 
	end
	
	if (nPhe ~= nValue) then
		Talk(1,"","<sex> kh«ng thÓ lÊy cê cña phe kh¸c.")
		return
	end
	
	if (nSLuong >= MAX_CAMCO) then
		Talk(1,"","Mçi phe chØ ®­îc c¾m ®èi ®a "..MAX_CAMCO.." cê mµ th«i.")
		return 
	end

	if(ConsumeItem(1 ,0, ITEM_CO_HIEU[1],ITEM_CO_HIEU[2],ITEM_CO_HIEU[3],-1,-1, pos_equiproom) >= 1) then
		SetMask(643)
		AddRunSpeed()
		DelMSNpc(MS_TONGKIM,nIndex)
		SetPMParam(MS_TONGKIM,nPlayerDataIdx,8,nPhe)
		Msg2MSAll(MS_TONGKIM,"Ng­êi ch¬i <color=green>"..GetName().."<color> ®· ®o¹t ®­îc cê "..TAB_PHE_TONGKIM[nPhe][2]..".")
		if(nPhe) then
			Msg2Player(format("ChuyÓn cê ®Õn to¹ ®é [%d,%d] c¾m lªn ®Ó hoµn thµnh",POST_DES_CAMCO[nPhe][1]/8,POST_DES_CAMCO[nPhe][2]/16))
		end
	else
		Talk(1,"","<sex> kh«ng cã cê hiÖu.")		
	end

end