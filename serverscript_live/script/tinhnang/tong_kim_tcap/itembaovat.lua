--Author: Fong Kieu
--Date: 08/06/2021
--Function: nhÆt b¶o vËt tèng kim

Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\lib\\lib_sukien.lua")
Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")

function main(nItemIdx)-- cã s½n PlayerIndex truyÒn tõ src qua råi
	if(RemoveItem(nItemIdx,1) ~= 1) then
		return
	end
	local nGoldId,nGen,nDetail,nParti,nLevel,nSeries = GetCBItem(nItemIdx)
	local nPlayerDataIdx = PIdx2MSDIdx(MS_TONGKIM,PlayerIndex)
	SetPMParam(MS_TONGKIM,nPlayerDataIdx,12,GetPMParam(MS_TONGKIM,nPlayerDataIdx,12)+1)--nhÆt b¶o vËt
	local nPointBV = 0
	for i = 1, getn(ITEM_BV_POINT) do
		if(nParti == ITEM_BV_POINT[i][1]) then
			nPointBV = ITEM_BV_POINT[i][2]
		end
	end
	SetPMParam(MS_TONGKIM,nPlayerDataIdx,16,GetPMParam(MS_TONGKIM,nPlayerDataIdx,16)+nPointBV)--TÝch luü b¶o vËt
	SetPMParam(MS_TONGKIM,nPlayerDataIdx,6,GetPMParam(MS_TONGKIM,nPlayerDataIdx,6) + nPointBV) -- cong diem tich luy
	local nPhe = GetMSIdxGroup(MS_TONGKIM, nPlayerDataIdx)
	SetMission(TAB_PHE_TONGKIM[nPhe][4],GetMissionV(TAB_PHE_TONGKIM[nPhe][4])+nPointBV) -- cong diem tich luy
	Msg2MSAll(MS_TONGKIM,format("%s nhÆt ®­îc b¶o vËt, tÝch luü t¨nng thªm %d ®iÓm. ", GetName(), nPointBV))
end