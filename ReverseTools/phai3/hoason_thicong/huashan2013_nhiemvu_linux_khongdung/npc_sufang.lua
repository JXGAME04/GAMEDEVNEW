Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
Include("\\script\\global\\skills_table.lua")
-- [HOASON 01/09] bo (JX1 khong co, xem hs_shim.lua): Include("\\script\\misc\\daiyitoushi\\toushi_function.lua")	-- ´øÒÕÍ¶Ê¦
-- [HOASON 01/09] bo (JX1 khong co, xem hs_shim.lua): Include("\\script\\task\\lv120skill\\head.lua")
-- [HOASON 01/09] bo (JX1 khong co, xem hs_shim.lua): Include("\\script\\task\\150skilltask\\g_task.lua")
Include("\\script\\dailogsys\\g_dialog.lua")

function main()
	local UTask_hs_150 = GetTask(2885)

	if GetLevel() >= 150 and (HaveMagic(1364) ~= -1 or HaveMagic(1382) ~= -1) and GetTask(LV120_SKILL_ID) > 0 and UTask_hs_150 <= 4 then
		Say("T« Phãng: ChuyÖn g×?", 2, "NhiÖm vô kü n¨ng cÊp 150/main_150", "Ta ®Õn t×m «ng cã viÖc kh¸c/othermain")
	else
		othermain()
	end
end

function othermain()
    	local UTask_hs_90 = GetTask(3486)

    	if UTask_hs_90 == 50 and CalcItemCount(3, 6, 1, 4962, -1) > 0 then
	    	Talk(4, "task90_1", "Ng­êi ch¬i: §©y lµ cña Minh Phi huynh nhê ta ®­a cho ng­¬i……", "Ng­êi ch¬i: ……Sù t×nh ®¹i ®Ó lµ nh­ vËy.", "T« Phãng: §¸ng ghĞt, Thiªn NhÉn Gi¸o qu¶ lµ ng¹o m¹n!", "T« Phãng: MÊy h«m nay ta bŞ s­ phô nhèt trong T­ Qu¸ Nhai, t¹m thêi kh«ng thÓ ra ngoµi, ng­¬i h·y ®em phong th­ nµy ®­a cho LËn s­ thóc, «ng Êy tù quyÕt ®o¸n.")
	elseif UTask_hs_90 == 60 and CalcItemCount(3, 6, 1, 4962, -1) < 1 then
	    	if CalcFreeItemCellCount() >= 1 then
		    	local nItemIdx = AddItem(6,1,4962,1,0,0)
		    	SetItemBindState(nItemIdx,-2)
	    	else
		    	Msg2Player("Hµnh trang ®· ®Çy, h·y s¾p xÕp l¹i cßn İt nhÊt 1 « trèng råi quay l¹i. ")
	    	end
	    	Talk(1, "", "T« Phãng: Cã ph¶i ng­¬i võa ®¸nh r¬i phong th­ n¬i nµy!")
    	else
	    	Talk(1, "", "ChuyÖn g×?")
    	end
end

function main_150()
	local UTask_hs_150 = GetTask(2885)

	if GetLevel() >= 150 and (HaveMagic(1364) ~= -1 or HaveMagic(1382) ~= -1) and GetTask(LV120_SKILL_ID) > 0 and UTask_hs_150 == 0 then
		Talk(4, "task150_go", "T« Phãng: C¬ héi lín, ta d· bŞ nhèt trong T­ Qu¸ Nhai nµy l©u l¾m råi....", "Ng­êi ch¬i: Xem ra S­ huynh chäc giËn Tõ s­ thóc råi, cã chuyÖn g× cÇn ta gióp kh«ng?", "T« Phãng: Ta nghe §Ö tö ®­a c¬m h«m qua cã nãi, ®¸m cÆn b· Thiªn NhÉn Gi¸o ®ang lµm g× ®ã ë Chu TiÕn TrÊn, tiÕc lµ ta kh«ng ®i ®­îc, ng­¬i cã thÓ gióp ta t×m hiÓu chuyÖn nµy kh«ng?", "Ng­êi ch¬i: §­¬ng nhiªn råi, ta ®i ngay.")
	elseif UTask_hs_150 == 1 then
		Talk(1, "", "T« Phãng: cËy nhê ®¹i hiÖp.")
	elseif UTask_hs_150 == 2 then
		Talk(4, "task150_1", "§¹i hiÖp kÓ l¹i c©u chuyÖn.....", "T« Phãng: §©y ch¾c ch¾n lµ ©m m­u cña Thiªn NhÉn Gi¸o, chØ do mét c« g¸i ®­a tin, xem nh­ ta xui xÎo.", "T« Phãng: ViÖc quan träng nh­ vËy, ta ph¶i ®i mét chuyÕn, ph¸ vì ı ®å cña chóng, nh©n dŞp s­ phô ®ang bÕ quan tu hµnh, ta sÏ lĞn ra ngoµi mét chuyÕn! Ng­¬i kh«ng ®­îc nãi chuyÖn nµy cho ng­êi kh¸c biÕt.", "Ng­êi ch¬i: (Néi t©m) Sao l¹i c¶m thÊy bÊt an thÕ nµy, hay lµ ta nãi cho s­ huynh biÕt nhØ.")
	elseif UTask_hs_150 == 3 or UTask_hs_150 == 4 then
		Talk(1, "", "T« Phãng: TuyÖt ®èi kh«ng ®­îc nãi víi ng­êi kh¸c.")
	end
end

function task90_1()
	SetTask(3486, 60)
	Msg2Player("Nãi cho LËn H¹o Thiªn. ")
	AddNote("Nãi cho LËn H¹o Thiªn. ")
end

function task150_go()
	SetTask(2885, 1)
	Msg2Player("§Õn chç Chu TiÕn TrÊn t×m C« g¸i khãc. ")
end

function task150_1()
	SetTask(2885, 3)
	Msg2Player("kÓ l¹i sù t×nh cho V¹n T­ ViÔn. ")
end
