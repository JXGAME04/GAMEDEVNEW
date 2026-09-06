-- Author: Fong KiÒu
--Date: 27/06/2021
--Function: Hép lÔ vËt v­ît ¶i

Include("\\script\\global\\vatpham.lua")
Include("\\script\\lib\\lib_task.lua")

EXP_HOPLVVA 							= 2000000
MAX_HOPLEVATVA_DAY 	= 3 --tèi ®a sö dông hép lÔ vËt trªn ngµy

function main(nItemIdx)
	
	-- dofile("script/item/hoplevatvuotai.lua")
	
	local nTaskValue = GetTask(TSK_HOPLEVATVA)
	if(nTaskValue >= MAX_HOPLEVATVA_DAY) then
		Talk(1,"","Mçi ngµy <sex> chØ ®­îc sö dông "..MAX_HOPLEVATVA_DAY.." hép")
		return 
	end
	
	SetTask(TSK_HOPLEVATVA, nTaskValue+1)	
	
	if CalcFreeItemCellCount() < 10 then
		Talk(1,"","Kho¶ng trèng hµnh trang kh«ng ®ñ 10 « . §Ò nghÞ dän dÑp hµnh trang tr­íc khi më !")
		return
	end	
	
	local nRand = random(0, 100)
	if(nRand == 10) then
		AddItem(4,353,0,0,0,0,0)--THBT
	elseif(nRand == 20) then
		AddItem(4,238,0,0,0,0,0)--Thuy tinh
	elseif(nRand == 30) then
		AddItem(4,239,0,0,0,0,0)--Thuy tinh
	elseif(nRand == 40) then
		AddItem(4,240,0,0,0,0,0)--Thuy tinh
	elseif(nRand == 50) then	
		AddItem(6,1,20,0,0,0,0)--Hoa Hång
	elseif(nRand == 60) then
		AddItem(6,1,11,0,0,0,0)--Ph¸o Hoa
	elseif(nRand == 60) then
		AddItem(6,1,random(121, 123),0,0,0,0)--Phuc Duyen
	elseif(nRand == 70) then
		AddItem(6,1,71,0,0,0,0)--Tien thao lo	
	elseif(nRand == 80) then
		AddItem(6,1,124,0,0,0,0)--QuÕ hoa töu					
	end
	
	AddOwnExp(EXP_HOPLVVA)
	
	--AddRandNHKTHoatDong()
	Msg2Player("Chóc mõng nhËn ®­îc phÇn th­ëng khi më lÔ vËt.")
	RemoveItem(nItemIdx,1)
	
end