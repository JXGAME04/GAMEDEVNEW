Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
Include("\\script\\global\\skills_table.lua")
-- [HOASON 01/09] bo (JX1 khong co, xem hs_shim.lua): Include("\\script\\misc\\daiyitoushi\\toushi_function.lua")	-- ´øÒÕÍ¶Ê¦

-- [HOASON 01/09] bo (JX1 khong co, xem hs_shim.lua): Include("\\script\\task\\150skilltask\\g_task.lua")
Include("\\script\\dailogsys\\g_dialog.lua")

function main()
    local UTask_hs = GetTask(3481)

    if (GetLevel() < 30 and UTask_hs > 0) or (UTask_hs < 30*256 and UTask_hs > 0) then --ÊÇ»ªÉ½µÜ×Ó£¬²»Âú30¼¶£¬»òÕßÊÇ»ªÉ½µÜ×Ó£¬Ã»Íê³É20¼¶ÈÎÎñ
	    Talk(1, "", "LËn V©n Mi: ng­êi ta kh©m phôc nhÊt, chİnh lµ s­ huynh ®ã.")
    elseif GetLevel() >= 30 and UTask_hs == 30*256 then
	    Say("LËn V©n Mi: C¸ch ®©y vµi ngµy cã ng­êi cña Thiªn NhÉn Gi¸o xuÊt hiÖn t¹i VÜnh L¹c TrÊn, d­êng nh­ lµ cã ©m m­u g× ®ã, cha thÊy thÕ liÒn mang theo mét sè s­ huynh ®Ö ®Õn ng¨n c¶n, mÆc dï ®· ®¸nh lïi Thiªn NhÉn Gi¸o, nh­ng còng cã nhiÒu s­ huynh ®Ö bŞ th­¬ng, ta cã tr¸ch nhiÖm ch¨m sãc hä. Nh­ng mÊy h«m nay thuèc trŞ th­¬ng s¾p dïng hÕt råi.", 2, "§i mua thuèc gióp./task30_go", "Ta cã viÖc kh¸c ph¶i lµm./no")
    elseif UTask_hs <= 30*256+40 and UTask_hs > 30*256 then
	    Talk(1, "", "LËn V©n Mi: C¸c s­ huynh ®Ö ®ang rÊt cÇn thuèc trŞ th­¬ng, ng­¬i mau ®i ®i. ")
    elseif UTask_hs == 30*256+50 then
	    task30_1()
    elseif UTask_hs >= 40*256 then
	    Talk(1, "", "LËn V©n Mi: ng­¬i ®· gióp ta mét viÖc lín, c¸c s­ huynh ®Ö bŞ th­¬ng ®Òu rÊt c¶m kİch. ")
    else
	    Talk(1, "", "LËn V©n Mi: ®Ö tö Hoa S¬n ta häc vÒ nho thuËt, thi th­, tö häa, kú nghÖ ®Òu rÊt am hiÓu.")
    end
end

function task30_1()
	    if CalcItemCount(3, 6, 1, 4952, -1) >= 1 then
		    Talk(1, "task30_finish", "LËn V©n Mi: Tèt qu¸, sè thuèc nµy ®ñ dïng råi, c¶m ¬n ®¹i hiÖp.")
	    else
		    Talk(2, "", "LËn V©n Mi: C¸c s­ huynh ®Ö ®ang rÊt cÇn thuèc trŞ th­¬ng, ng­¬i mau ®i ®i. ")
	    end
end

function task30_go()
	SetTask(3481, 30*256+10)
	Msg2Player("GÆp LËn V©n Mi, nhËn nhiÖm vô B¨ng Tµm Ngäc Lé Cao, ®Õn tiÖm thuèc Ph­îng t­êng mua B¨ng Tµm Ngäc Lé Cao. ")
	AddNote("GÆp LËn V©n Mi, nhËn nhiÖm vô B¨ng Tµm Ngäc Lé Cao, ®Õn tiÖm thuèc Ph­îng t­êng mua B¨ng Tµm Ngäc Lé Cao. ")
end

function task30_finish()
	ConsumeItem(3, 1, 6, 1, 4952, -1)

	SetTask(3481, 40*256)

	SetRank(85)
	add_hs(40)
	Msg2Player("Håi ®¸p LËn V©n Mi, hoµn thµnh nhiÖm vô B¨ng Tµm Ngäc Lé Cao. §¹i hiÖp t¨ng cÊp thµnh ®Ö tö chÊp kiÕm, häc ®­îc vâ c«ng Kim Nh¹n Hoµnh Kh«ng, Long HuyÒn KiÕm Khİ.")
	AddNote("Håi ®¸p LËn V©n Mi, hoµn thµnh nhiÖm vô B¨ng Tµm Ngäc Lé Cao. §¹i hiÖp t¨ng cÊp thµnh ®Ö tö chÊp kiÕm, häc ®­îc vâ c«ng Kim Nh¹n Hoµnh Kh«ng, Long HuyÒn KiÕm Khİ.")
end

function no()
end
