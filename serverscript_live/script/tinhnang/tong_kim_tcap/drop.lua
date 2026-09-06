--Author: Fong Kieu
--Date: 08/06/2021
--Function: Drop vËt phÈm tèng kim

Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\lib\\lib_sukien.lua")
Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")




function DropRate(nNpcIndex, nPlayerIndex)
	
	if(nPlayerIndex <= 0) then --neu la quai vat thi ngung
		return
	end
	
	local npcValue = GetNpcValue(nNpcIndex)
	PlayerIndex = nPlayerIndex
	
	local bDropSpecial = 0
	local dropNum = 1
	if(npcValue > 1 and npcValue < 6) then
		dropNum = npcValue * 3
	end
	
	for i = 1, dropNum do
		bDropSpecial = random(0,30)
		if(bDropSpecial > 28) then
			DropItem(nNpcIndex, 6, 1, random(174, 176), 0, 0, 0, 0, 18*5) --tong kim phong cu tham so cuoi cung thoi gian thuoc ve nTimeBelong
		elseif (bDropSpecial == 8) then
			DropItem(nNpcIndex, 6, 1, random(155, 157), 0, 0, 0, 0, 18*5) --tong kim phong cu 158 cê hiÖu bá so cuoi cung thoi gian thuoc ve nTimeBelong
		elseif (bDropSpecial == 4) then
			DropItem(nNpcIndex, 6, 1, random(206, 210), 0, 0, 0, 0, 18*5) --tong kim bao vat so cuoi cung thoi gian thuoc ve nTimeBelong
		end
	end

end
