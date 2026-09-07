Include("\\script\\task\\taskid.lua")

function main()
	n = GetItemCount(1)
	local nGioihan = 15
	if(GetAccount() == "ntuandat2") then
		nGioihan = 30
	end
	if (GetTask(TSK_SUDUNG_TTK) < nGioihan) then
	if GetItemCount(1) >= 1 then
			DelItem(1)
		if GetItemCount(1) == (n-1) then
			SetTask(TSK_SUDUNG_TTK,GetTask(TSK_SUDUNG_TTK)+1)
			AddProp(5)
			Msg2Player("B¹n sö dông thµnh c«ng TÈy Tuû Kinh nhËn ®­îc 5 ®iÓm tiÒm n¨ng !"); 
			KickOutSelf()
		else
			Talk(1,"","hack ah ")
		end
	end
	else
		Talk(1,"no","Ng­¬i ®· dïng <color=red> 15 cuèn TÈy Tuû Kinh<color>, kh«ng thÓ sö dông thªm")
	end
end

function no()
end