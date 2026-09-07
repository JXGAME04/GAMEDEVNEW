-- ================================================================================================
-- [HE THONG] script/global/Ç®×¯¹¦ÄÜ.lua
-- Muc dich  : Thu vien TOAN CUC (global) - nap boi Include tu nhieu tep.
-- Duoc nap  : engine nap luc boot (moi tep .lua = 1 lua_State rieng)
-- Include   : head_qianzhuang.lua
-- Ham (dong): main_proc (7), buy_sele (17), buy1 (22), buy2 (32), buy3 (42), no (52), hesui_hongbao (55), Package_Hesui (59), ruyi_hongbao (70), Package_Ruyi (74)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
--Author: Fong KiÒu
--Date:2021
--Function: Chñ tiÒn trang

--Include("\\script\\global\\head_qianzhuang.lua")
--IncludeForRegionVer("\\script\\global\\", "gamebank_proc.lua")

function main_proc()
	do return end
	--dofile("script/global/Ç®×¯¹¦ÄÜ.lua")
	if (GetSex() == 0) then
		Say("¤ng chñ tiÒn trang:  VÞ thiÕu hiÖp kia cÇn l·o phu gióp g× ? ",3,"§©y cã b¸n bao l× x× kh«ng ? /buy_sele","Ta muèn ®æi nguyªn b¶o thµnh tiÒn xu./change_ingot_to_coin","Ta chØ ghÐ qua th«i./no")
	else
		Say("¤ng chñ tiÒn trang:  VÞ c« n­¬ng kia cÇn l·o phu gióp g× ? ",3,"§©y cã b¸n bao l× x× kh«ng ? /buy_sele","Ta muèn ®æi nguyªn b¶o thµnh tiÒn xu./change_ingot_to_coin","Ta chØ ghÐ qua th«i./no")
	end
end

function buy_sele()
	--Say("§­îc ®­îc, chç nµy ®óng lµ cã b¸n bao l× x×, tiÓu hång bao 10v, ®¹i hång bao 100v.\n§Æc biÖt cã <color=green>Hång bao nh­ ý vµ Hång bao mõng thä<color> cã thÓ cã nhiÒu <color=green>TiÒn V¹n<color> vµ <color=yellow>TiÒn §ång<color>. \nNgµi muèn mua lo¹i nµo ? ",5,"TiÓu hång bao/buy1","§¹i hång bao /buy2","Hång bao nh­ ý /buy3","Hång bao mõng thä /hesui_hongbao","Kh«ng mua n÷a./no")
	Say("§­îc ®­îc, chç nµy ®óng lµ cã b¸n bao l× x×, tiÓu hång bao 1v, ®¹i hång bao 10v, ng­êi muèn mua lo¹i nµo ? ",3,"Mua tiÓu hång bao/buy1","Mua ®¹i hång bao/buy2","Kh«ng mua n÷a./no")
end

function buy1()
	if (GetCash() >= 100000) then
		Pay(100000)
		AddItem(6,1,14,1,0,0,0)
		Talk(1,"","¤ng chñ tiÒn trang: §©y lµ TiÓu hång bao cña ng­¬i, xin h·y cÇm lÊy!")
	else
		Talk(1,"","¤ng chñ tiÒn trang: H©y dµ! Ngµi kh«ng cã ®ñ tiÒn råi!")
	end
end

function buy2()
	if (GetCash() >= 1000000) then
		Pay(1000000)
		AddItem(6,1,13,1,0,0,0)
		Talk(1,"","¤ng chñ tiÒn trang: §©y lµ §¹i hång bao cña ng­¬i, xin h·y cÇm lÊy!")
	else
		Talk(1,"","¤ng chñ tiÒn trang: H©y dµ! Ngµi kh«ng cã ®ñ tiÒn råi!")
	end
end

function buy3()
	if (GetCash() >= 10000000) then
		Pay(10000000)
		AddItem(6,1,13,1,0,0,0)
		Talk(1,"","¤ng chñ tiÒn trang: §©y lµ §¹i hång bao cña ng­¬i, xin h·y cÇm lÊy!")
	else
		Talk(1,"","¤ng chñ tiÒn trang: H©y dµ! Ngµi kh«ng cã ®ñ tiÒn råi!")
	end
end

function no()
end

function hesui_hongbao()
	AskClientForNumber("Package_Hesui", 1, 100, "Xin mêi ®­a vµo tiÒn xu.")
end
function Package_Hesui(nMoney)
	if (CalcEquiproomItemCount(4, 417, 1, 1) < nMoney) then
		Talk(1,"","¤ng chñ tiÒn trang: Ngµi kh«ng mang ®ñ "..nMoney.." tiÒn xu.")
	else
		ConsumeEquiproomItem(nMoney, 4, 417, 1, -1)
		local nItem = AddItem(6,1,1052,1,0,0,0)
		SetSpecItemParam(nItem, 1, nMoney)
		SyncItem(nItem)
		Talk(1,"","¤ng chñ tiÒn trang: §©y lµ hång bao cña ng­¬i, xin h·y cÇm lÊy!")
	end
end

function ruyi_hongbao()
	AskClientForNumber("Package_Ruyi", 1, 10000000, "Xin mêi ®­a vµo bao tiÒn l× x× ")
end
function Package_Ruyi(nMoney)
	if (GetCash() < nMoney) then
		Talk(1,"","¤ng chñ tiÒn trang: ngµi kh«ng mang ®ñ "..nMoney.." ")
	else
		Pay(nMoney)
		local nItem = AddItem(6,1,1051,1,0,0,0)
		SetSpecItemParam(nItem, 1, nMoney)
		local nM1 = floor(nMoney / 1000000)
		local nM2 = floor(mod(nMoney, 1000000))
		local nM3 = mod(nMoney, 1000)
		SetSpecItemParam(nItem, 2, nM1)
		SetSpecItemParam(nItem, 3, nM2)
		SetSpecItemParam(nItem, 4, nM3)
		SyncItem(nItem)
		Talk(1,"","¤ng chñ tiÒn trang: §©y lµ hång bao cña ng­¬i, xin h·y cÇm lÊy!")
	end
end
