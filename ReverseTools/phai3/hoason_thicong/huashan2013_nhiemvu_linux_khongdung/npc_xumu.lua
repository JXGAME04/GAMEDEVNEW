Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
Include("\\script\\global\\skills_table.lua")
-- [HOASON 01/09] bo (JX1 khong co, xem hs_shim.lua): Include("\\script\\misc\\daiyitoushi\\toushi_function.lua")	-- ´øÒÕÍ¶Ê¦

-- [HOASON 01/09] bo (JX1 khong co, xem hs_shim.lua): Include("\\script\\task\\150skilltask\\g_task.lua")
Include("\\script\\dailogsys\\g_dialog.lua")

function main()
    local UTask_hs = GetTask(3481)  

    if (GetLevel() < 20 and UTask_hs > 0) or (UTask_hs < 20*256 and UTask_hs > 0) then --ÊÇ»ªÉ½µÜ×Ó£¬²»Âú20¼¶£¬»òÕßÊÇ»ªÉ½µÜ×Ó£¬Ã»Íê³É10¼¶ÈÎÎñ
	    Talk(1, "", "Tõ Mé: T×nh tr¹ng cña Phãng Nhi gÇn ®©y, lµm ta v« cïng lo l¾ng……")
    elseif GetLevel() >= 20 and UTask_hs == 20*256 then
	    Talk(2, "task20_1", "Tõ Mé: Ng­¬i gia nhËp ph¸i Hoa S¬n còng ®­îc mét thêi gian råi, ta thÊy ng­¬i ngµy ®ªm khæ luyÖn dông c«ng, hiÖn giê ng­¬i cã g× ch­a hiÓu kh«ng?", "Ng­êi ch¬i: §Ö tö gÇn ®©y cã tû thİ víi c¸c vŞ S­ huynh, th¾ng İt thua nhÒu, §Ö tö thÊy ®éng t¸c xuÊt chiªn cña S­ huynh thËt kh«ng gièng víi ®Ö tö, t¹i sao uy lùc chiªu thøc l¹i kh¸c nhau nhiÒu nh­ vËy?")
    elseif UTask_hs < 20*256+40 and UTask_hs > 20*256 then
	    if CalcItemCount(3, 6, 1, 4948, -1) < 1 and CalcItemCount(3, 6, 1, 4947, -1) < 1 then
		    Talk(1, "", "Tõ Mé: Ng­¬i lµm mÊt Thïng N­íc råi ­? Sao l¹i bÊt cÈn nh­ vËy. Mau ®i lÊy c¸i kh¸c ®i.")
		    if  CalcFreeItemCellCount() < 1 then
			    Msg2Player("Hµnh trang ®· ®Çy, kh«ng thÓ nhËn vËt phÈm nhiÖm vô, h·y s¾p xÕp l¹i cßn İt nhÊt 1 « trèng råi quay l¹i.")
		    else
			    local nItemIdx = AddItem(6,1,4947,1,0,0)
			    SetItemBindState(nItemIdx,-2)
			    Msg2Player("NhËn ®­îc Thïng N­íc")
		    end
	    else
		    Talk(2, "", "Tõ Mé: Cßn kh«ng mau ®i?")
	    end
    elseif UTask_hs == 20*256+40 then
	    task20_2()
    elseif UTask_hs >= 30*256 then
	    Talk(1, "", "Tõ Mé: H·y nhí lÊy c¶m nhËn cña ng­¬i, sÏ häc ®­îc cµng nhiÒu thø.")
    else
	    Talk(1, "", "Tõ Mé: GÇn ®©y mét sè ®Ö tö trong ph¸i s«i sôc muèn b¸o thï, viÖc nµy kh«ng tháa ®¸ng..... ")
    end
end

function task20_1()
	Say("Tõ Mé: Nh­ vËy, ng­¬i h·y ®i mét chuyÕn ®Õn sau nói, lÊy Nhai Bİch Chi Th¹ch, Dung TuyÕt Chi Thñy vµ Th­¬ng Tïng Ch©m DiÖp, ®em ®Õn chç s­ huyenh ®ång m«n cña ng­¬i nhê chØ gi¸o ®i.", 2, "§Ö tö ®i ngay./task20_go", "§Ö tö sÏ ®i sau./no")
end

function task20_2()
	    if CalcItemCount(3, 6, 1, 4948, -1) >= 1 and CalcItemCount(3, 6, 1, 4949, -1) >= 1 and CalcItemCount(3, 6, 1, 4950, -1) >= 1 then
		    Talk(4, "task20_finish", "Tõ Mé: B©y giê ng­¬i suy nghÜ thÕ nµo?", "Ng­êi ch¬i: v©ng, ®Ö tö chiªu thøc ®Ö tö ®¸nh ra Kh«ng h÷u h×nh nh­ng v« thÇn", "Ng­êi ch¬i: ta thÊy nói hoa s¬n khİ thÕ hïng v×, c¶m thô sù uy dòng cña thÕ vâ, nÕu nh­ cã thÓ bÊt ®éng nh­ s¬n, kiªn cè nh­ th¹ch, cang khinh nh­ tïng, nhu nhuyÔn nh­ thñy nhÊt ®Şnh sÏ ®¹t ®­îc ®Ønh cao cña kiÕm chiªu, hiÖu qu¶ gÊp béi.", "Tõ Mé: rÊt tèt, nÕu nh­ ng­¬i cã c¶m thô nh­ thÕ, sau nµy nhÊt ®Şnh sÏ tiÕn bé v­ît bËc, t¨ng sÏ t¨ng cÊp cho ng­¬i lµ T­ M¹c ®Ö tö")
	    elseif CalcItemCount(3, 6, 1, 4948, -1) < 1 and CalcItemCount(3, 6, 1, 4947, -1) < 1 then
		    Talk(1, "", "Tõ Mé: Ng­¬i lµm mÊt Thïng N­íc råi ­? Sao l¹i bÊt cÈn nh­ vËy. Mau ®i lÊy c¸i kh¸c ®i.")
		    if  CalcFreeItemCellCount() < 1 then
			    Msg2Player("Hµnh trang ®· ®Çy, kh«ng thÓ nhËn vËt phÈm nhiÖm vô, h·y s¾p xÕp l¹i cßn İt nhÊt 1 « trèng råi quay l¹i.")
		    else
			    local nItemIdx = AddItem(6,1,4947,1,0,0)
			    SetItemBindState(nItemIdx,-2)
			    Msg2Player("NhËn ®­îc Thïng N­íc")
		    end
	    else
		    Talk(2, "", "Tõ Mé: vËt ta nhê ng­¬i t×m ®©u?")
	    end
end

function task20_go()
	if CalcFreeItemCellCount() < 1 then
		Msg2Player("Hµnh trang ®· ®Çy, h·y s¾p xÕp l¹i cßn İt nhÊt 1 « trèng råi quay l¹i nhËn nhiÖm vô. ")
	else
		local nItemIdx = AddItem(6,1,4947,1,0,0)
		SetItemBindState(nItemIdx,-2)
		Msg2Player("NhËn ®­îc Thïng N­íc")
		SetTask(3481, 20*256+10)
		Msg2Player("GÆp Tõ Mé, nhËn nhiÖm vô Ngé KiÕm, ®Õn ph¸i Hoa S¬n hËu s¬n ®o¹t lÊy Nhai Bİch Th¹ch, Thôc TuyÕt Chi Thñy,Th­¬ng Tïng Ch©m DiÖp. ")
		AddNote("GÆp Tõ Mé, nhËn nhiÖm vô Ngé KiÕm, ®Õn ph¸i Hoa S¬n hËu s¬n ®o¹t lÊy Nhai Bİch Th¹ch, Thôc TuyÕt Chi Thñy,Th­¬ng Tïng Ch©m DiÖp. ")
	end
end

function task20_finish()
	ConsumeItem(3, 1, 6, 1, 4948, -1)
	ConsumeItem(3, 1, 6, 1, 4949, -1)
	ConsumeItem(3, 1, 6, 1, 4950, -1)

	SetTask(3481, 30*256)

	SetRank(84)
	add_hs(30)
	Msg2Player("Håi ®¸p Tõ Mé, hoµn thµnh nhiÖm vô Ngé KiÕm. §¹i hiÖp t¨ng cÊp thµnh T­ M¹c §Ö Tö, häc ®­îc vâ c«ng D­ìng Ngé KiÕm Ph¸i, H¶i N¹i B¸ch Xuyªn.")
	AddNote("Håi ®¸p Tõ Mé, hoµn thµnh nhiÖm vô Ngé KiÕm. §¹i hiÖp t¨ng cÊp thµnh T­ M¹c §Ö Tö, häc ®­îc vâ c«ng D­ìng Ngé KiÕm Ph¸i, H¶i N¹i B¸ch Xuyªn.")
end

function no()
end
