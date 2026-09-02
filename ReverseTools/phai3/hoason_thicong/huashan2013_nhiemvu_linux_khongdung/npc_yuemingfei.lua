Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
--»ªÉ½90¼¶ÈÎÎñNPCÔÀÃù·É
Include("\\script\\global\\repute_head.lua")
Include("\\script\\item\\skillbook.lua")

function main()
	local UTask_hs_90 = GetTask(3486)
	local UTask_hs_jmdz = GetTask(3487)

	local nSex = GetSex()
	local szSex = nil
	local szSex2 = nil
	if GetSex() == 0 then
		szSex = "Huynh ®µi."
		szSex2 = "hHuy ®µi cao danh quı t¸nh lµ g×?"
	else
		szSex = "C« n­¬ng"
		szSex2 = "Quı danh c« n­¬ng."
	end
	local szName = GetName()

	if(UTask_hs_90 < 10) and (GetLevel() >= 90) and (UTask_hs_jmdz == 30*256) and (GetReputeLevel(GetRepute()) >= 5) and (GetFaction() ~= "Hoa S¬n ph¸i") and (GetCamp() ~= 0 or GetTask(75) >= 255) then
		Talk(10, "task90_go", "Ng­êi ch¬i: ThËp niªn giang hå vò phiªu linh, ChÊp ®ao tr­îng kiÕm ®­¬ng bÊt b×nh. Håi Thñ méng trung th©n nh­ kh¸ch, bÊt nh­îc quy khø v¹n trïng thanh. VŞ c«ng tö nµy qu¶ thËt kh«ng tÇm th­êng, kh«ng biÕt t¹i h¹ cã thÓ ®èi t­ víi c«ng tö.", format("Nh¹c Minh Phi : %s qu¸ khen råi, t¹i h¹ chØ lµ ng­êi tÇm th­êng trong v¹n v¹n chóng sinh th«i.",szSex), "Ng­êi ch¬i: c«ng tö kh«ng cÇn qu¸ khiªm nh­êng? HiÖn nay thiªn h¹ ®¹i lo¹n, kh«ng İt ng­êi bŞ tôc l­u c¶ ®êi, nh­ng c«ng tö l¹i cã thÓ ng©m ra thÕ sù thøc tØnh,kh«ng khái khiÕn ng­êi kh¸c ng­ìng mé.", format("Nh¹c Minh Phi : Ha ha, %s hiÓu ta! Kh«ng biÕt %s quı danh, cã thÓ chóng ta sÏ trë thµnh mét tri giao!", szSex, szSex2), format("Ng­êi ch¬i: t¹i h¹ %s. ", szName), format("Nh¹c Minh Phi : t¹i h¹ Nh¹c Minh Phi, h«m nay ®­îc uèng r­îu ng©m th¬ cïng %s, kh«ng say kh«ng vÒ", szSex), "Töu qua 3 giê……", "Ng­êi ch¬i: ta thÊy Minh Phi huynh t­ ch©t kh«ng nh­ c¸c th­ sinh th«ng th­êng, thÕ sao l¹i b¸n th­ häa t¹i chî phiªn nh­ thÕ?", format("Nh¹c Minh Phi : kh«ng dèi %s, ta vèn lµ ®Ö tö Hoa S¬n ®Õn do th¸m t×nh b¸o t¹i T­¬ng D­¬ng, h«m tr­íc truy lïng 1 nhãm ®¹o tÆc, kh«ng ngê mÊt ®i tung tİch t¹i T­¬ng D­¬ng, nªn míi më cöa hµng th­ häa ®Ó dß th¸m tin tøc", szSex), "Ng­êi ch¬i: (nghÜ thÇm) ngoµi thµnh T­¬ng D­¬ng, umm………….…")
	elseif UTask_hs_90 == 40 then
		Talk(10, "task90_1", "Nh¹c Minh Phi : µi……", "Ng­êi ch¬i: Minh Phi Huynh sao thÕ", "Nh¹c Minh Phi : ……khi n·y cã mÊy tªn h¾c y nh©n ®Õn hµnh thİch ta, ta ®· cùc lùc chèng l¹i nh­ng vÉn bŞ th­¬ng, may thay khi Êy quan binh ®i ngang, nªn chóng t¹m rót lui… nãi thÕ ch¼ng lÏ c¸c h¹ còng ®· ®ông ®é chóng??", "Ng­êi ch¬i: ®óng t¹i, t¹i h¹ ph¸t hiÖn ra 1 sè m¸u tİch t¹i thµnh ngoµi, ®ang tİnh truy lïng th× bŞ mét tªn ng­êi ThÇn Bİ ®ét kİch, trªn ng­êi h¾n cã 1 th­ mËt lÖnh nghe nãi sÏ g©y bÊt lîi víi huynh, nh­ kh«ng ngê ®Õn trÔ 1 b­íc……", "Nh¹c Minh Phi : kh«ng sao, ®©y kh«ng ph¶i lµ vÕt th­¬ng nÆng g×", format("Nh¹c Minh Phi : nh÷ng tªn lóc n·y cã thÓ thÊy r»ng chóng ®Õn tõ Thiªn NhÉn Gi¸o, viÖc nµy nhÊt ®Şnh liªn quan ®Õn Thiªn NhÉn m«n, %s, t¹i muèn nhê mét viÖc", szName), "Ng­êi ch¬i: huynh cø nãi.", "Nh¹c Minh Phi : ta cã 1 bøc th­, bªn trong lµ t×nh b¸o mµ t¹i h¹ ®· thu thËp bÊt l©u nay, h·y mang chóng ®Õn ph¸i Hoa S¬n chuyÓn giao T« Phãng. ", "Ng­êi ch¬i: nhÊt ®Şnh, thÕ cßn huynh??", "Nh¹c Minh Phi : kh«ng sao, t¹i h¹ sÏ cè t×nh dô chóng khái sù chó ı cña c¸c h¹, c¸c h¹ h·y nhanh chãng ®i giao th­.")
	elseif UTask_hs_90 == 50 then
		if CalcItemCount(3, 6, 1, 4962, -1) < 1 then
			Talk(1, "", "Nh¹c Minh Phi : th­ bŞ mÊt råi? Kh«ng sao ®Ó ta viÕt l¹i th­ kh¸c.")
			if CalcFreeItemCellCount() >= 1 then
				local nItemIdx = AddItem(6,1,4962,1,0,0)
				SetItemBindState(nItemIdx,-2)
			else
				Msg2Player("Hµnh trang ®· ®Çy, h·y s¾p xÕp l¹i cßn İt nhÊt 1 « trèng råi quay l¹i. ")
			end
		else
			Talk(1, "", "Nh¹c Minh Phi : thÕ nµo? §· göi th­ ch­a?")
		end
	elseif UTask_hs_90 == 70 then
		Talk(6, "task90_2", "Ng­êi ch¬i: Minh Phi Huynh, LËn tiÒn bèi nhê ta ®Õn hç trî huynh.", "Nh¹c Minh Phi : cuèi cïng còng ph¶i nhê ®Õn c¸c h¹.", "Ng­êi ch¬i: hai ta ®· lµ tri giao th× kh«ng cÇn ph¶i nãi nh­ thÕ, huynh cø s¾p xÕp.", "Nh¹c Minh Phi : thÕ th× Minh Phi kh«ng kh¸ch khi n÷a, ta ®· th¸m thİnh ®­îc ngµy mai s¸t thñ cña Thiªn NhÉn Gi¸o sÏ ®Õn Phôc Ng­u S¬n mai phôc, ta hµnh ®éng bÊt tiÖn, ®µnh nhê c¸c h¹ ®i ng¨n c¶n chóng", "Ng­êi ch¬i: nhÊt ®Şnh kh«ng phô së th¸c.", "Nh¹c Minh Phi : c¸c h¹ h·y cÈn träng.")
	elseif UTask_hs_90 >= 80 and UTask_hs_90 < 90 then
		Talk(1, "", format("Nh¹c Minh Phi : %s, trªn ®­êng cÈn thËn.", szSex))
	elseif UTask_hs_90 == 90 then
		Talk(7, "task90_finish", "Ng­êi ch¬i: t¹i h¹ ®· gi¶i quyÕt toµn bé s¸t thñ.", format("Nh¹c Minh Phi : Tèt qu¸, lÇn nµy Thiªn NhÉn Gi¸o sÏ kh«ng cßn thùc hiÖn ®­îc ©m m­u n÷a. %s, qu¶ thËt ®a t¹.", szName), "Ng­êi ch¬i: chØ lµ chuyÖn nhá nh¹t, huynh kh«ng cÇn kh¸ch s¸o.", "Nh¹c Minh Phi : haha, t¹i h¹ cã mét quyÓn MËt TŞch Hoa S¬n, do chİnh tay ta sao chĞp, xem nh­ lÔ vËt håi ®¸p.", "Ng­êi ch¬i: c¸i nµy, t¹i h¹ sao d¸m nhËn….", format("Nh¹c Minh Phi : kh«ng sao, víi vâ c«ng vµ phÈm chÊt nh­ %s ®©y, quyÓn mËt tİch nµy kh«ng cßn ai thİch hîp h¬n.", szSex), "Ng­êi ch¬i: thÕ nµy t¹i h¹ ®µnh thÊt lÔ.")
	elseif UTask_hs_90 >= 10 and UTask_hs_90 < 100 then
		Talk(1, "", format("Nh¹c Minh Phi : thÕ %s cßn muèn uèng víi ta vµi ly kh«ng, haha.", szSex))
	else
		Talk(1, "", "Nh¹c Minh Phi : ThËp niªn giang hå vò phiªu linh, ChÊp ®ao tr­îng kiÕm ®­¬ng bÊt b×nh. Håi Thñ méng trung th©n nh­ kh¸ch, bÊt nh­îc quy khø v¹n trïng thanh.")
	end
end

function task90_go()
	SetTask(3486, 10)
	Msg2Player("kÕt giao víi Nh¹c Minh Phi,nhËn nhiÖm vô T­¬ng D­¬ng KÕt Tri Giao, §Õn ngoµi « T­¬ng D­¬ng kiÓm tra. ")
	AddNote("kÕt giao víi Nh¹c Minh Phi,nhËn nhiÖm vô T­¬ng D­¬ng KÕt Tri Giao, §Õn ngoµi « T­¬ng D­¬ng kiÓm tra. ")
end

function task90_1()
	if CalcItemCount(-1, 6, 1, 4961, -1) > 0 then
		ConsumeItem(-1, 1, 6, 1, 4961, -1)
	end

	if CalcFreeItemCellCount() < 1 then
		Msg2Player("Hµnh trang ®· ®Çy, h·y s¾p xÕp l¹i cßn İt nhÊt 1 « trèng råi quay l¹i nhËn nhiÖm vô. ")
	else
		local nItemIdx = AddItem(6,1,4962,1,0,0)
		SetItemBindState(nItemIdx,-2)
		Msg2Player("NhËn ®­îc th­ cña Nh¹c Minh Phi.")
		SetTask(3486, 50)
		Msg2Player("§Õn chç ph¸i Hoa S¬n mang th­ cña Nh¹c Minh Phi giao choT« Phãng. ")
		AddNote("§Õn chç ph¸i Hoa S¬n mang th­ cña Nh¹c Minh Phi giao choT« Phãng. ")
	end
end

function task90_2()
	SetTask(3486, 80)
	Msg2Player("§Õn Phôc Ng­u S¬n phİa ®«ng, tiªu diÖt S¸t Thñ Thiªn NhÉn Gi¸o. ")
	AddNote("§Õn Phôc Ng­u S¬n phİa ®«ng, tiªu diÖt S¸t Thñ Thiªn NhÉn Gi¸o. ")
end

function task90_finish()
   	if(GetTask(3481) >= 70*256) and (GetTask(3481) ~= 75*256) and GetLastFactionNumber() == 10 then			-- ÊÇ»ªÉ½µÜ×Ó»ò³öÊ¦µÜ×Ó
		SetTask(3486,255)
		if (HaveMagic(1364) == -1) then		-- ¶áÃüÁ¬»·ÈıÏÉ½£
			AddMagic(1364,1)
		end
		if (HaveMagic(1382) == -1) then		-- ÅüÊ¯ÆÆÓñ
			AddMagic(1382,1)
		end
		CheckIsCanGet150SkillTask()
   		Msg2Player("Häc ®­îc vâ c«ng §o¹t MÖnh Liªn Hoµn Tam Tiªn KiÕm, Bİch Th¹ch Ph¸ Ngäc.")
   		AddNote("Häc ®­îc vâ c«ng §o¹t MÖnh Liªn Hoµn Tam Tiªn KiÕm, Bİch Th¹ch Ph¸ Ngäc.")
	else
		SetTask(3486,245) 			-- Ö»»ñµÃÉùÍûµÄÉèÖÃ±äÁ¿Îª245
	end
   	AddRepute(1)
   	Msg2Player("Hoµn thµnh nhiÖm vô, NhËn ®­îc 1 ®iÓm danh väng.")
   	AddNote("Hoµn thµnh nhiÖm vô, NhËn ®­îc 1 ®iÓm danh väng.")
end
