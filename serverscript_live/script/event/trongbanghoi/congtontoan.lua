-- Author: Fong KiÒu
-- Date: 28/11/2020
-- Function: C«ng T«n To¶n

Include("\\script\\lib\\lib_server.lua")
Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_map.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\event\\trongbanghoi\\lib.lua")

function main(NpcIndex)
	
	-- dofile("script/event/trongbanghoi/congtontoan.lua")

	local TAB_QUANY = {
		"Ta cung cÊp nhiÒu lo¹i qu©n nhu, ng­¬i cÇn mua g×?",
		"Mua nhanh Ngò Hoa Ngäc Lé Hoµn/muamaunhanh",		
		"Ta muèn mua d­îc phÈm/muaquanluong",
		"Di chuyÓn ®Õn vÞ trÝ Trèng/GoTrongBH",
		"Ta chØ ghÐ qua./no",
	}	
	SayEx(TAB_QUANY)
end

function GoTrongBH()
	if(GetCamp() == 0) then
		Talk(1,"","T©n thñ kh«ng thÓ di chuyÓn ®Õn ®©y")
		return
	end
	SayEx({"<color=green><npc>: Chän vÞ trÝ ",
		"VÞ trÝ 1/#GoTrongBHOK(1)",
		"VÞ trÝ 2/#GoTrongBHOK(2)",
		"Kh«ng ®i/no"})
end

function GoTrongBHOK(nVT)
	local nx = 197 * 8
	local ny = 208 * 16
	if(nVT == 2) then
		nx = 187 * 8
		ny = 207 * 16		
	end	
	if(NewWorld(37, nx + random(0, 8), ny + random(0, 8)) > 0) then
		SetFightState(1)
	end	
end

function muaquanluong()
	Sale(53,0)
end

function muamaunhanh()
	local nummau = CalcFreeItemCellCount()
	local moneymau = 3000 -- l­îng
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