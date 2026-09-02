Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
Include("\\script\\global\\skills_table.lua")
-- [HOASON 01/09] bo (JX1 khong co, xem hs_shim.lua): Include("\\script\\misc\\daiyitoushi\\toushi_function.lua")	-- ´øÒÕÍ¶Ê¦

-- [HOASON 01/09] bo (JX1 khong co, xem hs_shim.lua): Include("\\script\\task\\150skilltask\\g_task.lua")
Include("\\script\\dailogsys\\g_dialog.lua")

function main()
	local UTask_hs_150 = GetTask(2885)
	if UTask_hs_150 == 4 or UTask_hs_150 == 5 then
		Say("ThÈm MÆc: ThÕ giíi vâ l©m...rÊt hçn lo¹n!", 2, "NhiÖm vô kü n¨ng cÊp 150/main_150", "Ta ®Õn t×m «ng cã viÖc kh¸c/othermain")
	else
		othermain()
	end
end

function othermain()
    local UTask_hs = GetTask(3481)

    if (GetLevel() < 40 and UTask_hs > 0) or (UTask_hs < 40*256 and UTask_hs > 0) then --ÊÇ»ªÉ½µÜ×Ó£¬²»Âú40¼¶£¬»òÕßÊÇ»ªÉ½µÜ×Ó£¬Ã»Íê³É30¼¶ÈÎÎñ
	    Talk(1, "", "ThÈm MÆc: S­ phô lu«n quë tr¸ch ta nh¸t gan, trong m¾t ng­êi, ta vÉn kh«ng b»ng T« S­ huynh……")
    elseif GetLevel() >= 40 and UTask_hs == 40*256 then
	    Talk(1, "task40_1", "ThÈm MÆc: Kh«ng l©u n÷a lµ ®Õn sinh nhËt s­ phô råi, s­ phô b×nh th­êng rÊt nghiªm nghŞ, chØ cã së thİch ®¸nh cê, ta vµ T« s­ huynh bµn víi nhau lµm mét dông cô ®¸nh cê tinh tÕ tÆng cho s­ phô.")
    elseif UTask_hs < 40*256+60 and UTask_hs > 40*256 then
	    Talk(1, "", "ThÈm MÆc: Ng­¬i cã thÓ ®Õn Ph­îng T­êng Phñ t×m thö.")
    elseif UTask_hs == 40*256+60 then
	    task40_2()
    elseif UTask_hs >= 50*256 then
	    Talk(1, "", "ThÈm MÆc: MÆc dï s­ phô ch­a bao nãi ra, nh­ng chóng ta ®Òu biÕt ng­êi rÊt thİch ch¬i cê.")
    else
	    Talk(1, "", "ThÈm MÆc: ThÕ giíi vâ l©m...rÊt hçn lo¹n!")
    end
end

function main_150()
    local UTask_hs_150 = GetTask(2885)

    if UTask_hs_150 == 4 then
        Talk(2, "task150_1", "Ng­êi ch¬i: T« S­ huynh h¹ s¬n ®i Chu TiÕn TrÊn, Ph­¬ng S­ huynh ®· ®i råi, b¶o ta ®Õn nãi víi ng­¬i mét tiÕng.", "ThÈm MÆc: Sao? vËy ta còng ph¶i ®i theo tiÕp øng.")
    elseif UTask_hs_150 == 5 then
	Talk(1, "", "ThÈm MÆc: ta sÏ hµnh ®éng ngay.")
    end
end

function task40_1()
	Say("ThÈm MÆc: Vèn dù ®Şnh mÊy h«m nay sÏ xuèng nói t×m, kÕt qu¶ lµ T« S­ huynh nghe nãi ë l©n cËn BiÖn Kinh cã vô c­íp tiªu liªn quan ®Õn Thiªn NhÉn Gi¸o, bÌn ®İch th©n xuèng nói, c­ìi ngùa kh«ng ngõng nghØ ®Ó ®Õn BiÖn Kinh, lÇm nµy e r»ng khã mµ quay vÒ, chØ cã m×nh ta, xuèng nói mét m×nh cã nhiÒu bÊt tiÖn, ng­¬i cã thÓ ®i víi ta kh«ng? ", 2, "Ta sÏ gióp ng­¬i./task40_go", "Ng­¬i t×m ng­êi kh¸c ®i./no")
end

function task40_2()
	    if CalcItemCount(3, 6, 1, 4953, -1) >= 1  then
		    Talk(2, "task40_finish", "Ng­êi ch¬i: bé V©n Tö Tr¾c Thu B×nh nµy dïng ®Ó lµm quµ cho LËn s­ thóc.", "ThÈm MÆc: §©y lµ mãn ®å quı gi¸, s­ phô sÏ thİch l¾m ®©y, c¶m ¬n!")
	    else
		    Talk(1, "", "ThÈm MÆc: Ng­¬i nãi dông cô ch¬i cê mÊt råi sao? ng­¬i h·y ®Õn Ph­îng T­êng t×m thö xem sao.")
	    end
end

function task40_go()
	Talk(1, "", "ThÈm MÆc: Ng­¬i cã thÓ ®Õn Ph­îng T­êng Phñ t×m thö.")

	SetTask(3481, 40*256+10)
	Msg2Player("GÆp ThÈm MÆc, tiÕp nhËn nhiÖm vô V©n Tö Tr¾c Thu B×nh, ®Õn TiÖm t¹p hãa Ph­îng T­êng hái th¨m.")
	AddNote("GÆp ThÈm MÆc, tiÕp nhËn nhiÖm vô V©n Tö Tr¾c Thu B×nh, ®Õn TiÖm t¹p hãa Ph­îng T­êng hái th¨m.")
end

function task40_finish()
	ConsumeItem(3, 1, 6, 1, 4953, -1)

	SetTask(3481, 50*256)

	SetRank(86)
	add_hs(50)
	Msg2Player("Håi ®¸p ThÈm MÆc, hoµn thµnh V©n Tö Tr¾c Thu B×nh nhiÖm vô. §¹i hiÖp t¨ng cÊp thµnh §an Thanh Sø, häc ®­îc vâ c«ng Hi Di KiÕm Ph¸p, Khİ ChÊn S¬n Hµ. ")
	AddNote("Håi ®¸p ThÈm MÆc, hoµn thµnh V©n Tö Tr¾c Thu B×nh nhiÖm vô. §¹i hiÖp t¨ng cÊp thµnh §an Thanh Sø, häc ®­îc vâ c«ng Hi Di KiÕm Ph¸p, Khİ ChÊn S¬n Hµ. ")
end

function task150_1()
	SetTask(2885, 5)
	Msg2Player("§Õn chç Chu TiÕn TrÊn t×m ThÈm MÆc. ")
end

function no()
end
