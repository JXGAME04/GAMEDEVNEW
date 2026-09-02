Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
Include("\\script\\global\\skills_table.lua")
-- [HOASON 01/09] bo (JX1 khong co, xem hs_shim.lua): Include("\\script\\misc\\daiyitoushi\\toushi_function.lua")	-- ´øÒÕÍ¶Ê¦

-- [HOASON 01/09] bo (JX1 khong co, xem hs_shim.lua): Include("\\script\\task\\150skilltask\\g_task.lua")
Include("\\script\\dailogsys\\g_dialog.lua")

function main()
    local UTask_hs = GetTask(3481)
    local UTask_hs_step = GetTask(3482)
    local UTask_hs_90 = GetTask(3486)

    if GetFaction() == "huashan" then
    	if (GetLevel() < 50 and UTask_hs > 0) or (UTask_hs < 50*256 and UTask_hs > 0) then --ÊÇ»ªÉ½µÜ×Ó£¬²»Âú50¼¶£¬»òÕßÊÇ»ªÉ½µÜ×Ó£¬Ã»Íê³É40¼¶ÈÎÎñ
	        Talk(1, "", "LËn H¹o Thiªn: NÕu cã thêi gian th× h·y rÌn luyÖn b¶n th©n thªm nhÐ!")
    	elseif GetLevel() >= 50 and UTask_hs == 50*256 then
	        Say("LËn H¹o Thiªn: GÇn ®©y b¸ch tÝnh ë VÜnh L¹c TrÊn bÞ ®¹o tÆc quÊy nhiÔu, mµ còng ch¼ng coi ph¸i Hoa S¬n ta ra g×, ng­¬i ®· gia nhËp ph¸i còng l©u råi, còng ®· ®Õn lóc xuèng nói råi, viÖc nµy giao cho ng­¬i gi¶i quyÕt.", 2, "§Ö tö tu©n mÖnh./task50_go", "§Ö tö n¨ng lùc kh«ng ®ñ./no")
	elseif UTask_hs < 50*256+40 and UTask_hs > 50*256 then
	        Talk(1, "", "LËn H¹o Thiªn: mau ®i ®i.")
    	elseif UTask_hs == 50*256+40 then
	        task50_1()
    	elseif UTask_hs >= 60*256 then
	        Talk(1, "", "LËn H¹o Thiªn: Dùa vµo t­ chÊt cña ng­¬i, thµnh tùu sau nµy kh«ng chØ dõng ë ®ã.")
        else
	        Talk(1, "", "LËn H¹o Thiªn: D¸m lõa ph¸i Hoa S¬n ta, ®¸ng bÞ trõng ph¹t! ")
        end
    else
	if UTask_hs_90 < 100 and UTask_hs_90 > 0 then
		huashantask90(UTask_hs_90)
		return
	end

	Talk(1, "", "LËn H¹o Thiªn: D¸m lõa ph¸i Hoa S¬n ta, ®¸ng bÞ trõng ph¹t! ")
    end
end

function task50_1()
	Talk(3, "task50_finish", "LËn H¹o Thiªn: §· lµm xong viÖc ch­a? ", "Ng­êi ch¬i: V©ng, ®Ö tö ®· rêi khái S¬n PhØ, ®· lÊy nh÷ng vËt bÞ c­íp tr¶ l¹i cho d©n råi.", "LËn H¹o Thiªn: Tèt l¾m, kh«ng hæ danh ®Ö tö ph¸i Hoa S¬n.")
end

function task50_go()
	SetTask(3481, 50*256+10)
	Msg2Player("GÆp LËn H¹o Thiªn, nhËn nhiÖm vô VÜnh L¹c Phong V©n, h·y ®Õn gióp ®ì d©n c­ VÜnh L¹c TrÊn.")
	AddNote("GÆp LËn H¹o Thiªn, nhËn nhiÖm vô VÜnh L¹c Phong V©n, h·y ®Õn gióp ®ì d©n c­ VÜnh L¹c TrÊn.")
end

function task50_finish()
	SetTask(3481, 60*256)

	SetRank(87)
	add_hs(60)
	Msg2Player("Håi ®¸p LËn H¹o Thiªn, hoµn thµnh nhiÖm vô VÜnh L¹c Phong V©n, ng­¬i th¨ng cÊp thµnh DÞch Thu Sø, häc ®­îc vâ c«ng Cµn Kh«n §¶o HuyÒn, KhÝ Qu¸n Tr­êng Hång.")
	AddNote("Håi ®¸p LËn H¹o Thiªn, hoµn thµnh nhiÖm vô VÜnh L¹c Phong V©n, ng­¬i th¨ng cÊp thµnh DÞch Thu Sø, häc ®­îc s vâ c«ng Cµn Kh«n §¶o HuyÒn, KhÝ Qu¸n Tr­êng Hång.")
end

function huashantask90(UTask_hs_90)
	local UTask_hs = GetTask(3481)
	if (UTask_hs == 0 or UTask_hs == 70*256) and UTask_hs_90 < 60 then
		Talk(1, "", "LËn H¹o Thiªn: Ng­¬i lµ ai? ®Õn ph¸i Hoa S¬n lµm g×?")
	elseif UTask_hs > 0 and UTask_hs_90 < 60 then
		Talk(1, "", "LËn H¹o Thiªn: ng­¬i lµ ®Ö tö míi nhËp m«n ­, t×m ta ChuyÖn g×?")
	elseif UTask_hs_90 == 60 and CalcItemCount(3, 6, 1, 4962, -1) > 0 then
		Talk(8, "task90_1", "Ng­êi ch¬i: LËn tiÒn bèi, ®©y lµ tinh cña Nh¹c Minh Phi ë T­¬ng D­¬ng nhê ta ®­a tíi, «ng ta bÞ ®¸nh lÐn bÞ th­¬ng, kh«ng tiÖn trë vÒ.", "LËn H¹o Thiªn: Thiªn NhÉn Gi¸o cã ý ®å m­u h¹i quan viªn triÒu ®×nh.", "LËn H¹o Thiªn: §· lµ ng­êi cña Thiªn NhÉn Gi¸o, ph¸i Hoa S¬n kh«ng thÓ kh«ng qu¶n, ta vèn muèn cho T« Phãng ®i gióp ®ì Nh¹c Minh Phi, nh­ng h¾n bÞ S­ huynh nhèt t¹i T­ Qu¸ Nhai råi.", "Ng­êi ch¬i: H¾n sao råi?", "LËn H¹o Thiªn: §õng nh¾c chuyÖn nµy n÷a, S­ huynh thùc sù ®· suy nghÜ qu¸ nhiÒu råi, ®¹i hiÖp, kh«ng biÕt cã thÓ nhê ng­êi mét viÖc kh«ng?", "Ng­êi ch¬i: LËn tiÒn bèi cø sai b¶o.", "LËn H¹o Thiªn: Ta vµ Minh Phi vèn lµ t©m giao, bÌn nhê T« Phãng gióp chuyÕn nµy, ph¸ tan ©m m­u cña Thiªn NhÉn gi¸o.", "Ng­êi ch¬i: TiÒn bèi yªn t©m, ta ®i ngay.")
	elseif UTask_hs_90 == 60 and CalcItemCount(3, 6, 1, 4962, -1) < 1 then
		Talk(1, "", "LËn H¹o Thiªn: Ng­¬i lµ ai? ®Õn ph¸i Hoa S¬n lµm g×?")
	elseif UTask_hs_90 > 60 then
		Talk(1, "", "LËn H¹o Thiªn: mau ®i ®i.")
	end
end

function task90_1()
	ConsumeItem(3, 1, 6, 1, 4962, -1)
	SetTask(3486, 70)
	Msg2Player("§Õn T­¬ng D­¬ng gióp Nh¹c Minh Phi. ")
	AddNote("§Õn T­¬ng D­¬ng gióp Nh¹c Minh Phi. ")
end

function no()
end
