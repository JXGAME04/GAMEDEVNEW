--Author: Fong KiÒu
Include("\\script\\task\\taskid.lua")

function main()
	n = GetItemCount(0)
	local nGioihan = 15
	if(GetAccount() == "ntuandat2") then
		nGioihan = 30
	end
	if (GetTask(TSK_SUDUNG_VLMT) < nGioihan) then
	if GetItemCount(0) >= 1 then
			DelItem(0)
		if GetItemCount(0) == (n-1) then
			SetTask(TSK_SUDUNG_VLMT,GetTask(TSK_SUDUNG_VLMT)+1)
			AddMagicPoint(1)
			Msg2Player("B¹n sö dông thµnh c«ng Vâ L©m MËt TÞch. nhËn ®­îc 1 ®iÓm kü n¨ng !")
			KickOutSelf()
		else
			Talk(1,"","Hack ah !")
		end
	end
	else
		Talk(1,"no","Ng­¬i ®· dïng <color=red> 15 cuèn Vâ L©m MËt TÞch<color>, kh«ng thÓ dïng thªm")
	end
end

function no()
end




