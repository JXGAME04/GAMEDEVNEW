-- Author: Fong KiÒu
-- Date: 28/11/2016
-- Function: Qu©n L­¬ng
Include("\\script\\lib\\lib_ham.lua")

function main(NpcIndex)
	
	-- dofile("script/tinhnang/tong_kim_tcap/quany.lua")

	SetTaskTemp(TMP_INDEX_NPC, NpcIndex)	
	local TAB_QUANY = {
		"Ta cung cÊp nhiÒu lo¹i qu©n nhu, ng­¬i cÇn mua g×?",
		"Mua nhanh Ngò Hoa Ngäc Lé Hoµn/muamaunhanh",		
		"Ta muèn mua d­îc phÈm/muaquanluong",
		"Ta chØ ghÐ qua./no",
	}	
	SayEx(TAB_QUANY)
end

function muaquanluong()
	--local nParam = GetNpcValue(GetTaskTemp(TMP_INDEX_NPC))
	--if(nParam ~= nil and nParam > 0) then
	--	Sale(53,0)
	--end	
	SetFightState(0) Sale(53,0)
end

function muamaunhanh()
	local nummau = CalcFreeItemCellCount()
	local moneymau = 1 -- l­îng
	local tongtien = nummau * moneymau
	if(GetCash() < tongtien) then
		Talk(1,"","<sex> kh«ng mang theo ®ñ ng©n l­îng.")
		return
	end
	Pay(tongtien)
	for i=1, nummau do
		AddItem(1, 2, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0)
	end
end

function no()
end