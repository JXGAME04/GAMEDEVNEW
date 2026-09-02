Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
Include("\\script\\global\\skills_table.lua")
-- [HOASON 01/09] bo (JX1 khong co, xem hs_shim.lua): Include("\\script\\misc\\daiyitoushi\\toushi_function.lua")	-- ´øÒÕÍ¶Ê¦

-- [HOASON 01/09] bo (JX1 khong co, xem hs_shim.lua): Include("\\script\\task\\150skilltask\\g_task.lua")
Include("\\script\\dailogsys\\g_dialog.lua")

function main()
	local UTask_hs_150 = GetTask(2885)
	if UTask_hs_150 == 3 or UTask_hs_150 == 4 or UTask_hs_150 == 8 then
		Say("V¹n T­ ViÔn: TiÓu Phãng tİnh t×nh lç m·ng kİch ®éng, haizz...", 2, "NhiÖm vô kü n¨ng cÊp 150/main_150", "Ta ®Õn t×m «ng cã viÖc kh¸c/othermain")
	else
		othermain()
	end
end

function othermain()
    local UTask_hs = GetTask(3481)

    if GetLevel() >= 10 and UTask_hs == 10*256 then
	    Talk(2, "task10_1", "V¹n T­ ViÔn: trµ ngon, tiÕc lµ...", "Ng­êi ch¬i: S­ huynh, trµ nµy cã g× kh«ng tháa ®¸ng?")
    elseif UTask_hs == 10*256 + 10 then
	    if CalcItemCount(3, 6, 1, 4945, -1) < 1 then
		    Talk(1, "", "V¹n T­ ViÔn: Gµo n­íc mÊt råi sao? mau ®i lÊy c¸i kh¸c, lÇn nµy ph¶i cÈn thËn ®ã.")
		    if  CalcFreeItemCellCount() < 1 then
			    Msg2Player("Hµnh trang ®· ®Çy, kh«ng thÓ nhËn vËt phÈm nhiÖm vô, h·y s¾p xÕp l¹i cßn İt nhÊt 1 « trèng råi quay l¹i.")
		    else
			    local nItemIdx = AddItem(6,1,4945,1,0,0)
			    SetItemBindState(nItemIdx,-2)
			    Msg2Player("NhËn ®­îc Gµo n­íc ")
		    end
	    else
		    Talk(2, "", "V¹n T­ ViÔn: §Õn dßng suèi lµ cã thÓ nhËn ®­îc Thanh TuyÒn Kho¸ng Thñy.")
	    end
    elseif UTask_hs == 10*256 + 20 then
	    task10_2()
    elseif UTask_hs >= 20*256 then
	    Talk(1, "", "V¹n T­ ViÔn: Kú thñy, dông s¬n thñy th­îng, giang thñy trung, c¶nh thñy h¹. Kú s¬n thñy luyÖn khæng tuyÒn, th¹ch tr× m¹n l­u gi¶ th­îng, vËt thùc chi...")
    else
	    Talk(1, "", "V¹n T­ ViÔn: TiÓu Phãng tİnh t×nh lç m·ng kİch ®éng, haizz...")
    end
end

function main_150()
    	    local UTask_hs_150 = GetTask(2885)

	    if UTask_hs_150 == 3 then
		    Talk(5, "task150_1", "Ng­êi ch¬i: T« S­ huynh nghe nãi Chu TiÕn TrÊn cã tung tİch cña Thiªn NhÉn Gi¸o, ®· ®İch th©n xuèng nói t×m hiÓu.", "V¹n T­ ViÔn: C¸i g×?! H¾n nghe ®­îc tõ ®©u?", "Ng­êi ch¬i: lµ...lµ do ta nãi víi h¾n, chuyÖn lµ nh­ thÕ nµy.", "V¹n T­ ViÔn: ……uh, còng kh«ng tr¸ch ng­¬i ®­îc, ®Çu tiªn lµ ®Ö tö ®­a c¬m nãi lé. ChØ lµ chuyÖn cña Chu Tiªn TrÊn cßn mét sè ®iÓm ®¸ng nghi, ta e r»ng cã sù lõa g¹t...", "V¹n T­ ViÔn: S­ phô ®ang bÕ quan, chØ cã thÓ ®i theo xem, còng cÇn phèi hîp víi ta, chuyÖn chóng ta xuèng nói, ng­¬i h·y nãi víi ThÈm MÆc S­ ®Ö mét tiÕng.")
	    elseif UTask_hs_150 == 4 then
		    Talk(1, "", "V¹n T­ ViÔn: Mau ®i ®i. ")
	    elseif UTask_hs_150 == 8 then
		    Talk(6, "task150_finish", "Ng­êi ch¬i: S­ huynh! m¾t huynh lµm sao thÕ? ", "V¹n T­ ViÔn: ……§õng lo, V©n Mi ®· gióp ta ®i t×m thuèc råi.", "Ng­êi ch¬i: ……S­ huynh, lµ do lçi cña ®Ö.", "V¹n T­ ViÔn: Kh«ng cÇn ¸y n¸y, viÖc nµy kh«ng ph¶i lçi cña ®Ö, ng­îc l¹i ta ph¶i c¶m ¬n ®Ö ®· cøu ThÈm MÆc.", "Ng­êi ch¬i: Kh«ng cã g× ®©u, t­¬ng trî ®ång m«n lµ chuyÖn nªn lµm.", "V¹n T­ ViÔn: Kú thùc chuyÖn nµy lµ trong häa cã may, TiÓu Phãng sau nµy sÏ kh«ng cßn lç m·ng n÷a. ChØ lµ tr­íc ®©y vµi ngµy ta cã nhËn ®­îc 1 lÖnh bµi, t×nh h×nh thÕ nµy th× e lµ kh«ng thùc hiÖn ®­îc, ph¶i nhê ®Ö thay ta chuyÕn nµy vËy.", "Ng­êi ch¬i: LÖnh bµi? §Ö sÏ ®i thö xem sao.")
	    end
end

function task10_1()
	local szSex = nil
	if GetSex() == 0 then
		szSex = "S­ ®Ö"
	else
		szSex = "S­ muéi"
	end

	Say(format("V¹n T­ ViÔn: §©y lµ trµ Tö D­¬ng Ng©n Ch©m lÇn tr­íc ta xuèng nói mang vÒ, vèn lµ trµ ngon, nh­ng ph¶i dïng n­íc giÕng ®Ó nÊu, nÕu dïng n­íc kh¸c sÏ mÊt vŞ ngon. §¹i hiÖp %s, ®¹i hiÖp cã thÓ ®i lÊy İt n­íc suèi trong nói vÒ ®©y kh«ng?", szSex), 2, "Ta ®i ngay./task10_go", "Ta cã viÖc kh¸c ph¶i lµm./no")
end

function task10_2()
	    if CalcItemCount(3, 6, 1, 4946, -1) >= 1 then
		    Talk(2, "task10_finish", "Ng­êi ch¬i: S­ huynh, huynh xem n­íc suèi nµy s¹ch thËt.", "V¹n T­ ViÔn: N­íc suèi nµy thËt trong lµnh thuÇn khiÕt, ®óng lµ th­îng phÈm, rÊt hîp ®Ó pha trµ, ®a t¹!")
	    elseif CalcItemCount(3, 6, 1, 4945, -1) < 1 then
		    Talk(1, "", "V¹n T­ ViÔn: Gµo n­íc mÊt råi sao? mau ®i lÊy c¸i kh¸c, lÇn nµy ph¶i cÈn thËn ®ã.")
		    if  CalcFreeItemCellCount() < 1 then
			    Msg2Player("Hµnh trang ®· ®Çy, kh«ng thÓ nhËn vËt phÈm nhiÖm vô, h·y s¾p xÕp l¹i cßn İt nhÊt 1 « trèng råi quay l¹i.")
		    else
			    local nItemIdx = AddItem(6,1,4945,1,0,0)
			    SetItemBindState(nItemIdx,-2)
			    Msg2Player("NhËn ®­îc Gµo n­íc ")
		    end
	    else
		    Talk(2, "", "V¹n T­ ViÔn: §Õn dßng suèi lµ cã thÓ nhËn ®­îc Thanh TuyÒn Kho¸ng Thñy.")
	    end
end

function task10_go()
	if CalcFreeItemCellCount() < 1 then
		Msg2Player("Hµnh trang ®· ®Çy, h·y s¾p xÕp l¹i cßn İt nhÊt 1 « trèng råi quay l¹i nhËn nhiÖm vô. ")
	else
		local nItemIdx = AddItem(6,1,4945,1,0,0)
		SetItemBindState(nItemIdx,-2)
		SetTask(3481, 10*256+10)
		Msg2Player("NhËn ®­îc Gµo n­íc ")
		Msg2Player("GÆp V¹n T­ ViÔn, nhËn nhiÖm vô lÊy n­íc pha trµ, ®Õn khe suèi ®Ó lÊy n­íc.")
		AddNote("GÆp V¹n T­ ViÔn, nhËn nhiÖm vô lÊy n­íc pha trµ, ®Õn khe suèi ®Ó lÊy n­íc.")
	end
end

function task10_finish()
	if ConsumeItem(3, 1, 6, 1, 4946, -1) == 1 then
		SetTask(3481, 20*256)

		SetRank(83)
		add_hs(20)
		Msg2Player("Håi ®¸p V¹n T­ ViÔn, hoµn thµnh nhiÖm vô lÊy n­íc pha trµ. ®¹i hiÖp t¨ng cÊp thµnh ®Ö tö phông trµ, ho¹c ®­îc vâ c«ng KiÕm T«ng Tæng QuyÕt, Long NhiÔu Th©n.")
		AddNote("Håi ®¸p V¹n T­ ViÔn, hoµn thµnh nhiÖm vô lÊy n­íc pha trµ. ®¹i hiÖp t¨ng cÊp thµnh ®Ö tö phông trµ, ho¹c ®­îc vâ c«ng KiÕm T«ng Tæng QuyÕt, Long NhiÔu Th©n.")
	else
		Msg2Player("Trõ vËt phÈm nhiÖm vô thÊt b¹i, h·y thö l¹i.")
	end
end

function task150_1()
	SetTask(2885, 4)
	Msg2Player("kÓ l¹i sù t×nh cho ThÈm MÆc. ")
end

function task150_finish()
	if CalcFreeItemCellCount() < 1 then
		Msg2Player("Hµnh trang ®· ®Çy, h·y s¾p xÕp l¹i cßn İt nhÊt 1 « trèng råi quay l¹i. ")
	else
		local nItemIdx = AddItem(6,1,4944,1,0,0)
		SetItemBindState(nItemIdx,-2)
		SetTask(2885, 9)
		Msg2Player("NhËn ®­îc NguyÖt Ca LÖnh.")
		WriteLog(format("[NhiÖm vô kü n¨ng 150 ph¸i Hoa S¬n cho NguyÖt Ca LÖnh]Ng­êi ch¬i%sHoµn thµnh nhiÖm vô nhËn ®­îc 1 NguyÖt Ca LÖnh.", GetName()))
		Msg2Player("§Õn chç D­¬ng Ch©u t×m thuyÒn phu lªn ®¶o.")
	end
end

function no()
end
