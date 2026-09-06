--Author: Fong KiÒu
--Date: 08/07/2021
--Function: ThÇn bİ th­¬ng nh©n t­¬ng d­¬ng

function main()
	-- dofile("script/global/Â·ÈË_ÉñÃØÉÌÈËÁøÒÒ.lua")
	local tab_Content = {
		"Cöa hµng phóc duyªn/OnBuy", 
		--"§Õn mét n¬i thÇn bİ /GotoHuanZhuan",
		"Liªn quan tíi phóc duyªn.../OnAbout"
	}
	 --tinsert(tab_Content, "Më r­¬ng 4/moruong4")
	tinsert(tab_Content, "KÕt thóc ®èi tho¹i/OnCancel")
	Say ( "<npc>: Kh¸ch quan muèn mua g× Bæn trang hµng g× còng cã!", getn(tab_Content), tab_Content)
end

function OnBuy()
	local iddiem_tichluy = 2
	local numShopTab = 1
	local idshoptk = 94
	local typeShop = 1
	NewSale(typeShop,iddiem_tichluy,numShopTab,idshoptk)
end

function OnAbout()
	Talk( 3, "", "Phóc duyªn ®iÓm ®Õm: Th«ng qua tİch lòy thêi gian ch¬i, cã thÓ ®æi ®iÓm phóc duyªn. Cã thÓ tíi T­¬ng D­¬ng thÇn bİ th­¬ng nh©n ®Ó ®æi c¸c lo¹i vËt phÈm ®Æc thï", " Phóc duyªn ®æi:tİch lòy hai giê online trë lªn , ®i c¸c ®¹i thµnh thŞ t×m Vâ l©m truyÒn nh©n ®Ó ®æi . mét ngµy ®­îc ®æi 2 lÇn, tİch lòy thêi gian 4 giê trë lªn, mçi giê ®æi 1 ®iÓm phóc duyªn.", "H¹n chÕ ®iÒu kiÖn :Ngåi tİ kh«ng ®­îc tİch lòy ®iÓm phóc duyªn." )                                                                                                                                                                                                                                                                                      
end

function OnCancel()
end

function GotoHuanZhuan()
	NewWorld(934, 49632 / 32, 105696 /32)
end


