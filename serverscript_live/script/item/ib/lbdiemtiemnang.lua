--Author: Fong Ki“u
--Function: ThÊ ßﬁa phÔ
--Date: 21/07/2021


Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\lib\\lib_task.lua")

function main(nIndex)
	local nDiemTN = random(10,100)
	AddProp(nDiemTN)
	Msg2Player("Bπn nhÀn Æ≠Óc "..nDiemTN.." Æi”m ti“m n®ng ")
	RemoveItem(nIndex)
end

