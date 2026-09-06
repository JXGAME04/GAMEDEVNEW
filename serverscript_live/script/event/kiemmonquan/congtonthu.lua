-- Author: Fong KiÒu
-- Date: 28/11/2020
-- Function: C«ng T«n Th­

Include("\\script\\lib\\lib_server.lua")
Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_map.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\event\\kiemmonquan\\lib_kmq.lua")

function main(NpcIndex)
	
	-- dofile("script/event/kiemmonquan/congtonthu.lua")

	local TAB_QUANY = {
		"Ta cung cÊp nhiÒu lo¹i qu©n nhu, ng­¬i cÇn mua g×?",
		"Mua nhanh Ngò Hoa Ngäc Lé Hoµn/muamaunhanh",		
		"Ta muèn mua d­îc phÈm/muaquanluong",
		"Ta muèn quay vÒ thµnh/quayvethanh",
		"Di chuyÓn theo vÞ trÝ/GoMap995",
		"Ta chØ ghÐ qua./no",
	}	
	SayEx(TAB_QUANY)
end

function quayvethanh()
	SubWorld = SubWorldID2Idx(995)
	if(NewWorld(53, 1622, 3189) > 0) then
		SetFightState(0)
		SetPKMode(0, 0)--phuc hoi pk tù do
		SetPunish(0) --tat tinh nang chet khong mat gi
		--SetCurCamp(GetCamp())
		SetDeathScript("")
		local nPlayerDataIdx = PIdx2MSDIdx(MS_KIEMMONQUAN, PlayerIndex)
		local szName = GetName()
		if(nPlayerDataIdx > 0) then	
			SetPMParam(MS_KIEMMONQUAN, nPlayerDataIdx, 0, 0)
			Msg2MSAll(MS_KIEMMONQUAN, format("%s ®· rêi khái KiÕm M«n Quan.", szName)) --thong bao roi khoi~
		end		
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