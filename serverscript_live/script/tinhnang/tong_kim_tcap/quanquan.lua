-- Author: Fong KiÒu
-- Date: 28/11/2020
-- Function: Qu©n Nhu Quan Tèng Kim

Include("\\script\\lib\\lib_vatpham.lua")
-- [CFGTKT 30/08] hai tep duoi day la LA (khong Include gi).
Include("\\script\\cauhinh\\ch_lib.lua")
Include("\\script\\cauhinh\\ch_thuong.lua")
-- [CFGTKT 30/08] Bo doc cau hinh cho tep nay. Tra ve MAC DINH (= so cu)
-- khi bo cau hinh chua nap, nen kem nhat cung khong the doi hanh vi.
function TKT_CFG(szKhoa, macdinh)
	if (G_CFG ~= nil) then
		return G_CFG(szKhoa, macdinh)
	end
	return macdinh
end

Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\log_game\\save_log.lua")
Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")

NOW_END_SAY 				= "KÕt thóc ®èi tho¹i./no"
NEED_MONEY 					= TKT_CFG("TKT_GIA_DOI_NHACVUONGKIEM", 10000000)								--sè tiÒn cÇn ®æi nh¹c v­¬ng kiÕm
ITEM_NVHT						= {4,507,0}
ITEM_NVKIEM					= {4,195,0}
NVHT_TO_NVK					= TKT_CFG("TKT_SL_HONTHACH_DOI_KIEM", 100)											--100 viªn nh¹c v­¬ng hån th¹ch ®æi 1 nh¹c v­¬ng kiÕm

function main_quannhu(sel)

	 dofile("script/tinhnang/tong_kim_tcap/quanquan.lua")
	
	local nSubWorldId = SubWorldID2Idx(MAP_TK_TC)
	if nSubWorldId < 0 then	--chua mo map, ngung ham`
		return 
	end
	SubWorld = nSubWorldId
	local TAB_MSG = 
	{
		"HiÖn t¹i <sex> ®ang cã:\n- <color=fire>"..GetTask(T_TONGKIM).."<color> ®iÓm tèng kim\n- <color=fire>"..GetTask(T_TICHLUYCT).."<color> ®iÓm c«ng tr¹ng\r- <color=fire>"..GetTask(T_LIENDAU).."<color> ®iÓm vinh dù.",
		"§æi Nh¹c V­¬ng KiÕm/doinhacvuongkiem",
		"Cöa hµng ®iÓm Tèng Kim/giaodichtongkim",
		"§æi ®iÓm tÝch luü tèng kim nhËn Ch©n Nguyªn/tichluytk2expBCH",
		"§æi ®iÓm tÝch luü tèng kim nhËn Huy Ch­¬ng Tèng Kim/tichluytk2expHCTK",
		"§æi ®iÓm tÝch luü tèng kim nhËn ®iÓm kinh nghiÖm/tichluytk2exp",
		
		--"B¶ng xÕp h¹ng [V« Song M·nh T­íng] tuÇn/vosongmanhtuong",
		NOW_END_SAY,
	}
	SayEx(TAB_MSG)
end

TAB_VSMT_REWARD = {
	{8000*2,8000,212,222, 	1,		1,			96,273},		-- 1 vâ l©m liªn ®Êu qu¸n qu©n
	{6000*2,6000,213,223, 	2,		2,			97,274},		-- 2 vâ l©m liªn ®Êu ¸ qu©n
	{5000*2,5000,214,224, 	3,		3,			98,275},		-- 3 vâ l©m liªn ®Êu h¹ng 3
	{4000*2,4000,215,225, 	4,		4,			99,276},		-- 4 vâ l©m liªn ®Êu h¹ng t­
	{3500*2,3500,216,226, 	5, 	8,			0,277},		-- 5
	{3000*2,3000,217,227, 	9,		16,		0,278},		-- 6
	{2500*2,2500,218,228, 	17,	32,		0,279},		-- 7
	{2000*2,2000,219,229, 	33,	64,		0,280},		-- 8
	{1500*2,1500,220,230, 	65,	128,	0,281},	-- 9
	{1000*2,1000,221,231, 	129,256,		0,282},	-- 10
}	

function vosongmanhtuong()
	--Talk(1,"","TÝnh n¨ng nµy ®ang x©y dùng sÏ ra m¾t trong thêi gian tíi.")
	local aindex = random(1,10)
	if(TAB_VSMT_REWARD[aindex][8] > 0) then --vong sang lien dau
		SetPlayerTitle(TAB_VSMT_REWARD[aindex][8], 30*24*60*1080, 0)
	end	
end

function tichluytk2exp()
	SayEx({
		"<npc>: <sex> muèn ®æi bao nhiªu ®iÓm xin mêi chän",
		format("1000 ®iÓm tÝch luü nhËn ®iÓm kinh nghiÖm/#okaytichluytk2exp(%d)",1000),
		format("2000 ®iÓm tÝch luü nhËn ®iÓm kinh nghiÖm/#okaytichluytk2exp(%d)",2000),
		format("5000 ®iÓm tÝch luü nhËn ®iÓm kinh nghiÖm/#okaytichluytk2exp(%d)",5000),
		format("10000 ®iÓm tÝch luü nhËn ®iÓm kinh nghiÖm/#okaytichluytk2exp(%d)",10000),
		NOW_END_SAY
	})
end

function okaytichluytk2exp(nTichLuy)
	if(GetTask(T_TONGKIM) < nTichLuy) then
		Talk(1,"","<sex> kh«ng ®ñ ®iÓm "..nTichLuy.." tÝch luü mµ muèn lõa ta sao?")
		return
	end
	SetTask(T_TONGKIM, GetTask(T_TONGKIM) - nTichLuy)
	local nExpO = nTichLuy * 1000
	AddOwnExp(nExpO)
	Msg2Player("Xin chóc mõng ®· nhËn ®­îc "..nExpO.." ®iÓm kinh nghiÖm")
end

function tichluytk2expBCH()
	SayEx({
		"<npc>: <sex> muèn ®æi bao nhiªu ®iÓm xin mêi chän",
		format("2000 ®iÓm tÝch luü nhËn 1 Ch©n Nguyªn §¹i/#okaytichluytk2exp_bch(%d)",2000),
		format("20000 ®iÓm tÝch luü nhËn 10 Ch©n Nguyªn §¹i/#okaytichluytk2exp_bch(%d)",20000),
	-- format("1000 ®iÓm tÝch luü nhËn 1 B¹ch CÇu Hoµn/#okaytichluytk2exp(%d)",5000),
	--	format("1000 ®iÓm tÝch luü nhËn 1 B¹ch CÇu Hoµn/#okaytichluytk2exp(%d)",10000),
		NOW_END_SAY
	})
end

function okaytichluytk2exp_bch(nTichLuy_BCH)
	if(GetTask(T_TONGKIM) < nTichLuy_BCH) then
		Talk(1,"","<sex> kh«ng ®ñ ®iÓm "..nTichLuy_BCH.." tÝch luü mµ muèn lõa ta sao?")
		return
	end
	SetTask(T_TONGKIM, GetTask(T_TONGKIM) - nTichLuy_BCH)
	local SLBCH = nTichLuy_BCH / 2000
	for i=1,SLBCH do
	AddItem(6,1,4847,0,0,0,0) --- BCH
	Msg2Player("Xin chóc mõng ®· nhËn ®­îc "..SLBCH.."Ch©n Nguyªn §¹i")
	end
end

function tichluytk2expHCTK()
	SayEx({
		"<npc>: <sex> muèn ®æi bao nhiªu ®iÓm xin mêi chän",
		format("50 ®iÓm tÝch luü nhËn 1 Huy Ch­¬ng Tèng Kim/#okaytichluytk2exp_hctk(%d)",50),
		format("500 ®iÓm tÝch luü nhËn 10 Huy Ch­¬ng Tèng Kim/#okaytichluytk2exp_hctk(%d)",500),
		format("2500 ®iÓm tÝch luü nhËn 50 Huy Ch­¬ng Tèng Kim/#okaytichluytk2exp_hctk(%d)",2500),
		format("5000 ®iÓm tÝch luü nhËn 100 Huy Ch­¬ng Tèng Kim/#okaytichluytk2exp_hctk(%d)",5000),
		NOW_END_SAY
	})
end

function okaytichluytk2exp_hctk(nTichLuy_BCH)
	if(GetTask(T_TONGKIM) < nTichLuy_BCH) then
		Talk(1,"","<sex> kh«ng ®ñ ®iÓm "..nTichLuy_BCH.." tÝch luü mµ muèn lõa ta sao?")
		return
	end
	SetTask(T_TONGKIM, GetTask(T_TONGKIM) - nTichLuy_BCH)
	local SLBCH = nTichLuy_BCH / 50
	AddItemSL(4831,SLBCH,0)
	Msg2Player("Xin chóc mõng ®· nhËn ®­îc "..SLBCH.."Huy Ch­¬ng Tèng Kim")
end

function giaodichtongkim()
	local iddiem_tichluy = 4
	local numShopTab = 1
	local idshoptk = 92
	local typeShop = 1
	NewSale(typeShop,iddiem_tichluy,numShopTab,idshoptk)
end

function doinhacvuongkiem()
	OpenGiveBox("§Æt vµo vËt phÈm","§Æt vµo "..NVHT_TO_NVK.." Nh¹c V­¬ng Hån Th¹ch, "..NEED_MONEY.." l­îng.","nvhonthachtonvkiem");
end

function nvhonthachtonvkiem()
	if GetCash() < NEED_MONEY then
		Talk(1,"",format("Ng­¬i kh«ng ®ñ %d l­îng.",NEED_MONEY))
		return 
	end
	
	if(GetItemCount(0, ITEM_NVHT[1],ITEM_NVHT[2],ITEM_NVHT[3],-1,-1, pos_affairitem)>=NVHT_TO_NVK) then
		if(ConsumeItem(NVHT_TO_NVK ,0, ITEM_NVHT[1],ITEM_NVHT[2],ITEM_NVHT[3],-1,-1, pos_affairitem) >= NVHT_TO_NVK) then
			EndGiveBox()
			Pay(NEED_MONEY)
			AddItem(ITEM_NVKIEM[1],ITEM_NVKIEM[2],ITEM_NVKIEM[3],0,0,0)
			logDoiNhacVuongKiem(""..GetName().."\tdoi thanh cong 1 nhac vuong kiem")
			Msg2Player("§æi thµnh c«ng 1 Nh¹c V­¬ng KiÕm, giang hå s¾p cã mét phen dËy sãng.")			
		end
	else
		Talk(1,"","<sex> vui lßng ®Æt vµo chÝnh x¸c nguyªn liÖu.")
	end		
	
end

function no()
end