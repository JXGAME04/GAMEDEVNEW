--Author: Fong KiÒu
--Function: Item tèng kim

Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")

function main(nItemIdx)
	Say("Xin lùa chän n¬i ®Õn chiÕn tr­êng Tèng Kim:",3,
	format("Vµo ®iÓm b¸o danh bªn Tèng/gotong(%d)", nItemIdx),
	format("Vµo ®iÓm b¸o danh bªn Kim/gokim(%d)", nItemIdx),
	"Ch­a muèn ®i/no")
end

function gotong(nItemIdx)
	if (NewWorld(MAP_BD_TC,floor(48928/32), floor(102272/32)) == 1) then--bao danh tong kim cao cap--phe Tong
		RemoveItem(nItemIdx,1)
	end
end

function gokim(nItemIdx)
	if (NewWorld(MAP_BD_TC,floor(50944/32), floor(98400/32)) == 1) then--bao danh tong kim cao cap--phe Kim
		RemoveItem(nItemIdx,1)
	end
end

function no()
end

