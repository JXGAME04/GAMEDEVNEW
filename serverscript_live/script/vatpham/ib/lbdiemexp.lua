--Author: Fong KiÒu
--Function: Thæ §Þa phï
--Date: 21/07/2021


Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\lib\\lib_task.lua")

function main(nIndex)
	local nDiemEXP = random(100000000,500000000)
	AddSumExp(nDiemEXP)
	Msg2Player("B¹n nhËn ®­îc "..nDiemEXP.." ®iÓm Exp !"); 
	RemoveItem(nIndex)
end

