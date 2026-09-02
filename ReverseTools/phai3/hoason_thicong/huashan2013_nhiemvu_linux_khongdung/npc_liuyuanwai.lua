Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
Include("\\script\\global\\skills_table.lua")
-- [HOASON 01/09] bo (JX1 khong co, xem hs_shim.lua): Include("\\script\\misc\\daiyitoushi\\toushi_function.lua")	-- ´øÒÕÍ¶Ê¦

-- [HOASON 01/09] bo (JX1 khong co, xem hs_shim.lua): Include("\\script\\task\\150skilltask\\g_task.lua")
Include("\\script\\dailogsys\\g_dialog.lua")

function main()
    local UTask_hs = GetTask(3481)

    if UTask_hs == 40*256+20 then
	    Talk(4, "task40_1", "L­u Viªn Ngo¹i: Tù Cæ T­íng Qu©n ChiÕn Th­¬ng Tr­êng, dÞch gi¶ ®Êu trÝ thu b×nh gi¸n. S¸t ph¹t quyÕt ®o¹n tïy t©m ý, ng· tù s¬ cuång bÊt dÞ tiªn. ha ha!", "Ng­êi ch¬i: §· nghe kú nghÖ cña v­¬ng ngo¹i tõ l©u, kh«ng biÕt t¹i h¹ cã thÓ thØnh gi¸o 1 v¸n kh«ng?", "L­u Viªn Ngo¹i: Ha ha, trong ®êi ta thÝch nhÊt lµ ch¬i cê, chØ tiÕc lµ ng­êi ch¬i cê hay th× nhiÒu, nh­ng ng­êi hiÓu lý luËn vÒ cê rÊt Ýt, h«m nay ®­îc bµn luËn cïng c¸c h¹ thËt lµ tèt qu¸!", "Ng­êi ch¬i: §­¬ng nhiªn lµ ®­îc.")
    elseif UTask_hs < 40*256+60 and UTask_hs > 40*256+20 then
	    task40_1()
    elseif UTask_hs == 40*256+60 then
	    if CalcItemCount(3, 6, 1, 4953, -1) < 1  then
		    task40_2()
		    Talk(1, "", "L­u Viªn Ngo¹i: Ta võa nh×n thÊy bé V©n Tö Tr¾c Thu B×nh nµy trªn ®­êng, cã ph¶i cña c¸c h¹ ®¸nh r¬i kh«ng? Dông cô ®¸nh cê nµy rÊt quý, c¸c h¹ nªn gi÷ cÈn thËn.")
	    else
		    Talk(1, "", "L­u Viªn Ngo¹i: Ta thÝch nhÊt lµ cïng ng­êi kh¸c uèng r­îu ®èi Èm vµ ®¸nh cê.")
	    end
    else
	    Talk(1, "", "L­u Viªn Ngo¹i: Ta thÝch nhÊt lµ cïng ng­êi kh¸c uèng r­îu ®èi Èm vµ ®¸nh cê.")
    end
end

function task40_1()
	local tbQuestion = {
		[1] = {
			szTitle="Ng­êi miÒn B¾c còng rÊt thÝch ®¸nh cê v©y. Côc TÊt Ph­¬ng ChÝnh, ______; ______, ThÇn Minh §øc D·; cê cã ph©n biÖt tr¾ng ®en, ©m d­¬ng, BiÒn la liÖt bè, ______. ",
			[1] = "A, T­îng thiªn t¾c d·, ®¹o tÊt khóc chiÕt, hiÖu tinh h¸n d·.",
			[2] = "B, T­îng ®Þa t¾c d·, ®¹o tÊt khóc chiÕt, hiÖu tinh h¸n d·.",
			[3] = "C, T­îng ®Þa t¾c d·, ®¹o tÊt chÝnh trùc, hiÖu tinh v¨n d·.",
			nAnswer = 3,
	        },

		[2] = {
			szTitle="Tù cã Trung vµ HÒ, h·y nãi víi ®èi thñ, tiªn c­______hÒ, ______Y Bµng. ",
			[1] = "A, Thiªn Nguyªn, B¶o Gi¸c.",
			[2] = "B, Tø §¹o, B¶o Gi¸c.",
			[3] = "C, Tø Gi¸c, B¶o biªn.",
			nAnswer = 2,
	        },

		[3] = {
			szTitle="Phï K×______, ______, ­íc khinh vu n¹i, n¹i khinh vu tÞ, gi¸p h÷u h­ thùc, ®¶ h÷u t×nh ngôy.",
			[1] = "A, Gi¸c bÊt nh­ biªn, biªn bÊt nh­ phóc.",
			[2] = "B, Phóc bÊt nh­ biªn, biªn bÊt nh­ gi¸c.",
			[3] = "C, Biªn bÊt nh­ gi¸c, gi¸c bÊt nh­ phóc.",
			nAnswer = 3,
	        },
	}

	local UTask_hs = GetTask(3481)
	local nIdx = 0
	if UTask_hs == 40*256+20 then
		nIdx = 1
	elseif UTask_hs == 40*256+30 then
		nIdx = 2
	elseif UTask_hs == 40*256+40 then
		nIdx = 3
	end

	if UTask_hs < 40*256+50 then
		local nAnswer = tbQuestion[nIdx].nAnswer
		local nStep = UTask_hs+10

		Say(tbQuestion[nIdx].szTitle, 3, format(tbQuestion[nIdx][1].."/#check_answer(%d,%d,1)",nStep,nAnswer), 
	    	    format(tbQuestion[nIdx][2].."/#check_answer(%d,%d,2)",nStep,nAnswer), 
	    	    format(tbQuestion[nIdx][3].."/#check_answer(%d,%d,3)",nStep,nAnswer))
	else
		Talk(1, "task40_2", "L­u Viªn Ngo¹i: Kh«ng sai, xem ra c¸c h¹ còng cã tµi n¨ng vµ phong th¸i, bé V©n Tö Tr¾c Thu B×nh nµy ta tÆng l¹i cho c¸c h¹ lµm quµ.")
	end

end

function task40_2()
	if CalcFreeItemCellCount() < 1 then
		Msg2Player("Hµnh trang ®· ®Çy, kh«ng thÓ nhËn vËt phÈm nhiÖm vô. ")
	else
		local nItemIdx = AddItem(6,1,4953,1,0,0)
		SetItemBindState(nItemIdx,-2)
		Msg2Player("NhËn ®­îc V©n Tö Tr¾c Thu B×nh.")
		SetTask(3481, 40*256+60)
		Msg2Player("ChÝnh thøc tr¶ lêi 3 c©u hái cña L­u Viªn Ngo¹, NhËn ®­îc V©n Tö Tr¾c Thu B×nh. ")
		AddNote("ChÝnh thøc tr¶ lêi 3 c©u hái cña L­u Viªn Ngo¹, NhËn ®­îc V©n Tö Tr¾c Thu B×nh. ")
	end
end

function check_answer(nStep, nAnswer, nPlayerAnswer)
	if nAnswer ~= nPlayerAnswer then
		Talk(1, "", "L­u Viªn Ngo¹i: Kh«ng ®óng, c¸c h¹ h·y suy nghÜ l¹i.")
	else
		SetTask(3481, nStep)
		task40_1()
	end
end

function no()
end
