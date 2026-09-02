Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
Include("\\script\\global\\repute_head.lua")

function main()
	local UTask_jmdz = GetTask(3487)
	if (UTask_jmdz == 0) and (GetFaction() ~= "huashan") and (GetLevel() >= 10) then	-- ¼ÇÃûµÜ×ÓÈÎÎñÆô¶¯
		Say("Tiªu Ngäc: Tõ khi bæn ph¸i truyÒn tin thu nhËn ®Ö tö ra kh¾p giang hå, mçi ngµy l­îng ng­êi ®Õn ®Çu s­ ®«ng nh­ trÈy héi, ng­¬i còng nghe tin nµy ph¶i kh«ng?", 2, "Muèn/task_jmdz_go", "Ta chØ qua ®­êng/exit")
	elseif UTask_jmdz >= 10 * 256 and UTask_jmdz < 20*256 then
		Talk(1, "", "Tiªu Ngäc: Thu thËp ®ñ Hång Méc th× ®Õn t×m ta.")
	elseif UTask_jmdz >= 20 * 256 and UTask_jmdz < 30*256 then
		task_jmdz_finish()
	else
		Talk(1, "", "Tiªu Ngäc: T¹i h¹ chØ lµ mét ng­êi nhµn rçi ë Hoa S¬n, c¸c h¹ t×m cã chuyÖn g× kh«ng?")
	end
end

function task_jmdz_go()
	SetTask(3487, 10*256)
	Talk(2, "", "Tiªu Ngäc: Muèn gia nhËp vµo bæn ph¸i, kh«ng thÓ chØ nãi su«ng lµ ®­îc, nh©n dŞp cã líp ®Ö tö míi quy n¹p, cÇn cÊp vò khİ cho chóng, v× hiÖn t¹i chóng vÉn ch­a tu th©n, kiÕm ph¸p cña bæn ph¸i næi tiÕng lµ nhanh vµ chİnh x¸c, kh«ng g©y ngé th­¬ng, c¸c vŞ s­ thóc ®· bµn b¹c víi nhau lµ sÏ cho chóng luyÖn tËp méc kiÕm tr­íc. Nghe nãi ë vïng Nam C­¬ng cã rÊt nhiÒu hång méc, lo¹i gç nµy rÊt s¾c bĞn, lµ nguyªn liÖu tèt ®Ó lµm méc kiÕm, ng­¬i nªn ®Õn ®ã mét chuyÕn.", "Tiªu Ngäc: Nghe nãi lo¹i c©y nµy cã linh vËt thñ hé, ng­¬i ®i lÇn nµy ph¶i cÈn thËn ®Êy.")

	Msg2Player("NhËn nhiÖm vô chÆt c©y lµm kiÕm, §Õn §iÓm Th­¬ng S¬n thu thËp Hång Méc.")
	AddNote("NhËn nhiÖm vô chÆt c©y lµm kiÕm, §Õn §iÓm Th­¬ng S¬n thu thËp Hång Méc.")
end

function task_jmdz_finish()
	    if CalcItemCount(3, 6, 1, 4960, -1) >= 1 then
		    ConsumeItem(3, 1, 6, 1, 4960, -1)
		    SetTask(3487, 30*256)
		    Talk(2, "", "Ng­êi ch¬i: Ta ®· ®em Hång Méc vÒ nh­ ®· høa.", "Tiªu Ngäc: Uhm...Hång Méc th­îng h¹ng qu¶ lµ tèt thËt, ng­¬i ®· gióp bæn m«n mét viÖc lín, nh­ng muèn gia nhËp bæn ph¸i cÇn ph¶i kiÓm tra t­ chÊt. ThÕ nµy, ®îi ta b¸o l¹i víi s­ phô, nhËn ng­¬i lµm ®Ö tö ghi danh tr­íc nhĞ!")
		    AddRepute(1) --½±Àø1µãÉùÍû
		    Msg2Player("Hoµn thµnh nhiÖm vô chÆt c©y lµm kiÕm, ng­¬i ®· trë thµnh ®Ö tö ghi danh cña ph¸i Hoa S¬n, NhËn ®­îc 1 diÓm danh väng.")
		    AddNote("Hoµn thµnh nhiÖm vô chÆt c©y lµm kiÕm, ng­¬i ®· trë thµnh ®Ö tö ghi danh cña ph¸i Hoa S¬n, NhËn ®­îc 1 diÓm danh väng.")
	    else
		    Talk(1, "", "Tiªu Ngäc: Thu thËp ®ñ Hång Méc th× ®Õn t×m ta.")
	    end
end

function  exit()
end
