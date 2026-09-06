--Author: Fong KiÒu
--Function: Thæ §Þa phï
--Date: 21/07/2021


Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\lib\\lib_task.lua")

function main(nIndex)
	local nDiemKN = random(1,10)
	AddMagicPoint(nDiemKN)
	Msg2Player("B¹n nhËn ®­îc "..nDiemKN.." ®iÓm kü n¨ng ")
	RemoveItem(nIndex)
end


