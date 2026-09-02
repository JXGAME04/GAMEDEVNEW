Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
Include("\\script\\task\\newtask\\newtask_head.lua")
-- [HOASON 01/09] bo (JX1 khong co, xem hs_shim.lua): Include("\\script\\task\\newtask\\education\\knowmagic.lua")
Include("\\script\\global\\skills_table.lua")
Include("\\script\\global\\map_helper.lua")
-- [HOASON 01/09] bo (JX1 khong co, xem hs_shim.lua): Include( "\\script\\missions\\freshman_match\\head.lua" )
npc_name = "Hoa S¬n Èn SÜ."

function main()
	player_Faction = GetFaction()

	if (player_Faction == "emei") then				
		Talk(1,"","Hoa S¬n Èn SÜ: N÷ hiÖp, t¹i h¹ cã lÔ.")
	elseif (player_Faction == "cuiyan") then				
		Talk(1,"","Hoa S¬n Èn SÜ: Sím nghe danh tiÕng cña n÷ hiÖp, h«m nay gÆp mÆt qu¶ lµ danh bÊt h­ truyÒn.")
	elseif (player_Faction == "tagnmen") then
		Talk(1,"","Hoa S¬n Èn SÜ: Thuèc ®éc vµ ¸m khİ cña quı ph¶i qu¶ lµ lîi h¹i, nh­ng kh«ng ph¶i lµ hµnh vi cña qu©n tö.")
	elseif (player_Faction == "wudu") then				
		Talk(1,"","Hoa S¬n Èn SÜ: ph¸i Hoa S¬n ta kh«ng kÕt giao víi yªu ta, c¸c h¹ mêi tù nhiªn.")
	elseif (player_Faction == "shaolin") then				
		Talk(1,"","Hoa S¬n Èn SÜ: Kh«ng biÕt ®¹i s­ cã g× chØ gi¸o?")
	elseif (player_Faction == "wudang") then				
		Talk(1,"","Hoa S¬n Èn SÜ: Cïng lµ m«n ph¸i dïng kiÕm, t¹i h¹ v« cïng høng thó víi vâ c«ng cña quı ph¸i.")
	elseif (player_Faction == "kunlun") then				
		Talk(1,"","Hoa S¬n Èn SÜ: Quı ph¸i vµ tÖ ph¸i kh«ng cã liªn quan, ch¼ng hay ®¹o tr­ëng cã g× chØ gi¸o?")
	elseif (player_Faction == "tianren") then				
		Talk(1,"","Hoa S¬n Èn SÜ: Hø, hãa ra lµ Thiªn NhÉn tµ gi¸o, h·y nhËn mét kiÕm cña ta!")
	elseif (player_Faction == "gaibang") then
		Talk(1,"","Hoa S¬n Èn SÜ: Nghe nãi quı ph¸i ®Òu lµ nhòng ng­êi ch©n chİnh, t¹i h¹ v« cïng kh©m phôc.")
	elseif (player_Faction == "tianwang") then
		Talk(1,"","Hoa S¬n Èn SÜ: Nghe nãi bang chñ D­¬ng Anh cña quı bang lµ n÷ trung hµo kiÖt hiÕm cã trªn ®êi, kh«ng biÕt khi nµo cã thÓ diÖn kiÕn 1 lÇn.")
	elseif (player_Faction == "huashan") then
		Say("Hoa S¬n Èn SÜ: cÇn ta tiÔn ng­¬i vÒ Hoa S¬n kh«ng? ",4,"§­îc, C¶m ¬n!/return_yes", "T×m hiÓu khu vùc luyÖn c«ng/map_help","T×m hiÓu vâ nghÖ bæn m«n/skill_help","Kh«ng cÇn ®©u! §a t¹!  /no")
	elseif (nt_getTask(75) == 255) then		-- Ñ§ÁËÒ°ÇòÈ­µÄ£¬²»ÔÊĞíÔÙÈëÃÅÅÉ
		Talk(1,"","Hoa S¬n Èn SÜ: tù häc khæ luyÖn, bÊt luËn nh­ thÕ nµo t¹i h¹ v« cïng kh©m phôc nghŞ lùc nµy.")
	else						-- Ö»Òª²»ÊôÓÚ11¸öÃÅÅÉ£¬²»ÔÙµ¥¶À¼ì²â¡°ÎŞÃÅÅÉ¡±
		UTask_hs = nt_getTask(3481)

		if (UTask_hs >= 70*256) then					--»ªÉ½ÅÉ³öÊ¦		
			local szSex = nil
			if GetSex() == 0 then
				szSex = "S­ huynh"
			else
				szSex = "S­ tû"
			end

			Say(format("Hoa S¬n Èn SÜ: %s sau khi xuèng nói ch¬i cã nghe ngãng ®­îc g× kh«ng? ", szSex),3,"T×m hiÓu së tr­êng luyÖn c«ng/map_help","T×m hiÓu vâ nghÖ bæn m«n/skill_help"," KÕt thóc ®èi tho¹i!/no")
		else
			Talk(3, "select", "Hoa S¬n Èn SÜ: Bæn ph¸i do tæ s­ phô Tiªu Tö L¨ng s¸ng lËp vµo n¨m Hµm Th«ng tiÒn triÒu, kh«ng râ manh mèi g× vÒ D­¬ng ThŞ ®êi TÇn, v× lŞch sö ®· qu¸ l©u ®êi. Vâ c«ng cña bæn ph¸i ph©n thµnh 2 nh¸nh, KiÕm T«ng lµ tu th©n, Khİ T«ng lµ tu t©m, dung hîp trong vâ c«ng, khİ t«ng chiªu thøc cÇu kú, tinh x¶o v« song, v¹n ph¸p giai th«ng, khİ t«ng cÇu ch©n vu t©m, nhÊt ph¸p th«ng v¹n ph¸p.", "Hoa S¬n Èn SÜ: Do tæ s­ truyÒn l¹i, bæn ph¸i h­íng ®Õn b¸c t«n nho häc. Trõ vâ häc, ®Ö tö Hoa S¬n cßn kiªm tu lôc nghÖ, trong ®ã kh«ng thiÕu trµ ®¹o, cê v©y. Cã thªm phong ph¹m qu©n tö.", "Hoa S¬n Èn SÜ: §ang thêi c¬ vâ l©m phong v©n lo¹n l¹c, phông mªnh ch­ëng m«n, bæn ph¸i chiªu mé ®Ö tö, hoµi b·o b¸o thï, nh÷ng ng­êi chİnh trùc gan d¹ cã thÓ gia nhËp bæn ph¸i.")
		end
	end
end;

function select()
	UTask_em = nt_getTask(1)
	UTask_cy = nt_getTask(6)
	UTask_hs = nt_getTask(3481)
	if UTask_cy > 5*256 then
		Talk(1,"","Hoa S¬n Èn SÜ: VŞ ®¹i hiÖp nµy vèn dÜ gia nhËp Thiªn NhÉn, mçi ng­êi mét ı chİ, ta còng kh«ng miÔn c­ìng.")
	elseif UTask_cy > 5*256 then
		Talk(1,"","Hoa S¬n Èn SÜ: VŞ ®¹i hiÖp nµy vèn dÜ gia nhËp Nga Mi, mçi ng­êi mét ı chİ, ta còng kh«ng miÔn c­ìng.")
	elseif (GetSeries() == 2) and (GetCamp() == 0) and (UTask_cy < 5*256) and (UTask_em < 5*256) and UTask_hs == 0 then   --Ë®Ïµ¡¢Î´Èë´äÑÌ¶ëáÒ
		if (GetLevel() >= 10) then						--µÈ¼¶´ïµ½Ê®¼¶
			Say("Hoa S¬n Èn SÜ: ng­¬i muèn gia nhËp ph¸i Hoa S¬n cña ta? ", 3, "Gia nhËp ph¸i Hoa S¬n./on_go", "Kh«ng gia nhËp/no","T×m hiÓu tinh hoa vâ nghÖ c¸c m«n ph¸i/Uworld1000_knowmagic")
		else
			Say("Hoa S¬n Èn SÜ: §¹i hiÖp c¬ b¶n lµ ch­a tËp ®ñ, cÇn mét thêi gian n÷a ®Ó luyÖn tËp, kh«ng ®­îc hÊp tÊp. H·y ra ngoµi luyÖn tËp thªm, ®Õn <color=Red> cÊp 10 <color> h·y quay l¹i t×m ta.", 0)		
		end
	end		-- ²»ÊÇË®ÏµµÄ¾Í²»ÔÙ¶àËµ»°ÁË¡£
end;

function on_go()
	Say("NhÊp vµo lµ x¸c nhËn lËp tøc Gia nhËp <color=yellow>ph¸i Hoa S¬n <color>#, h·y suy nghÜ kü!", 2, "Ta x¸c nhËn Gia nhËp./go", "VËy th× th«i./no")
end

function go()
	SetRevPos(987,1)		  			--ÉèÖÃÖØÉúµã
	nt_setTask(3481, 10*256)				-- Ö±½ÓÈëÃÅ
	nt_setTask(3482, 0)
	SetFaction("huashan")       			--Íæ¼Ò¼ÓÈë»ªÉ½ÅÉ
	SetCamp(3)
	SetCurCamp(3)
	SetRank(82)
	nt_setTask(137,71)
	SetLastFactionNumber(10)
	add_hs(10)			-- µ÷ÓÃskills_table.luaÖĞµÄº¯Êı£¬²ÎÊıÎªÑ§µ½¶àÉÙ¼¶¼¼ÄÜ¡£
-- ½áÊø
	Msg2Player("§¹i hiÖp ®· Gia nhËp ph¸i Hoa S¬n, trë thµnh th­ ®ång, hä ®­îc vâ c«ng B¸ch Hång Qu¸n NhËt, Thanh Phong Tèng S¶ng.")
	AddNote("§¹i hiÖp ®· Gia nhËp ph¸i Hoa S¬n, trë thµnh th­ ®ång, hä ®­îc vâ c«ng B¸ch Hång Qu¸n NhËt, Thanh Phong Tèng S¶ng.")
	Msg2Faction("Hoa S¬n ph¸i",GetName().."KÓ tõ h«m nay Gia nhËp ph¸i Hoa S¬n xin b¸i kiÕn c¸c vŞ S­ huynh S­ tû, mong c¸c S­ huynhS­ tû chiÕu cè!",GetName())
	NewWorld(987,1346,3128)			--°ÑÍæ¼Ò´«ËÍµ½ÃÅÅÉÈë¿Ú
end;

function return_yes()
	NewWorld(987,1346,3128)			--°ÑÍæ¼Ò´«ËÍµ½ÃÅÅÉÈë¿Ú
end;

function no()
end;
