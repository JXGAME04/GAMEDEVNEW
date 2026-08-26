-------------------------------------------------------------------------
-- FileName		:	posthouse.lua
-- Author		:	xiaoyang
-- CreateTime	:	2005-04-04 15:30:14
-- Desc			:	ËÍĞÅÈÎÎñæä¹Ù½Å±¾
-------------------------------------------------------------------------
Include("\\script\\tinhnang\\3hoatdong\\beidou\\bd_activity.lua")
Include("\\script\\task\\newtask\\newtask_head.lua")
Include("\\script\\header\\cauhinh_hoatdong.lua")	-- [CFG TS 25/08] doc TS_* (file LA, khong Include gi)
Include("\\script\\task\\tollgate\\messenger\\messenger_timeer.lua")    --µ÷ÓÃ¼ÆÊ±Æ÷
Include("\\script\\task\\tollgate\\messenger\\messenger_losetask.lua")  --È¡ÏûÈÎÎñ
Include("\\script\\task\\task_addplayerexp.lua")  --¸øÍæ¼ÒÀÛ¼Ó¾­ÑéµÄ¹«¹²º¯Êı
Include("\\script\\task\\tollgate\\killbosshead.lua") --°üº¬ÁËÍ¼Ïóµ÷ÓÃ
Include("\\script\\activitysys\\g_activity.lua")
Include("\\script\\task\\tollgate\\messenger\\qianbaoku\\messenger_baoxiangtask.lua")--¿ª±¦Ïä¹¤¾ßº¯Êı
Include("\\script\\activitysys\\playerfunlib.lua")
Include("\\script\\lib\\log.lua")
Include("\\script\\misc\\eventsys\\eventsys.lua")
--Include("\\script\\lib\\awardtemplet.lua")--¸ø½±Àø½Ó¿Ú

IncludeLib("RELAYLADDER");	--ÅÅĞĞ°ñ


--¹¹Ôì¸÷³ÇÊĞÈ¥ÆäËû³ÇÊĞµÄÈÎÎñ±äÁ¿¶ÔÓ¦±í        60-89¼¶£¬90-119¼¶£¬120ÒÔÉÏÈıÖÖ
--citygo µÄ¹¹Ôì·½Ê½
--µ½¸÷³ÇÊĞµÄÈÎÎñ±äÁ¿£¬¶ÔÓ¦Öµ£¬µ±Ç°³ÇÊĞÃû×Ö£¬È¥µ½³ÇÊĞÃû×Ö£¬Ïà¶ÔÈÎÎñid£¬µ±Ç°³ÇÊĞid£¬È¥µ½³ÇÊĞµØÍ¼id£¬x£¬y
--1201	ËÍĞÅÈÎÎñ·çÖ®ÆïÈÎÎñ±äÁ¿
--1202	ËÍĞÅÈÎÎñÉ½ÉñÃíÈÎÎñ±äÁ¿
--1203	ËÍĞÅÈÎÎñÇ§±¦¿âÈÎÎñ±äÁ¿

citygo = {
----ÑïÖİÈ¥ÆäËû³ÇÊĞ(mapid:80)
--{1204,1,"ÑïÖİ","ãê¾©",1201,80,37,1601,3001},
--{1204,2,"ÑïÖİ","·ïÏè",1201,80,1,1561,3114},
--{1204,3,"ÑïÖİ","ÁÙ°²",1201,80,176,1592,2925},
--{1204,4,"ÑïÖİ","³É¶¼",1201,80,11,3021,5090},
--{1204,5,"ÑïÖİ","ÏåÑô",1201,80,78,1596,3379},
--{1204,6,"ÑïÖİ","´óÀí",1201,80,162,1674,3132},
--
----ãê¾©È¥ÆäËû³ÇÊĞ(mapid:37)
--{1204,7,"ãê¾©","ÑïÖİ",1201,37,80,1676,3000},
--{1204,8,"ãê¾©","·ïÏè",1201,37,1,1561,3114},
--{1204,9,"ãê¾©","ÁÙ°²",1201,37,176,1592,2925},
--{1204,10,"ãê¾©","³É¶¼",1201,37,11,3021,5090},
--{1204,11,"ãê¾©","ÏåÑô",1201,37,78,1596,3379},
--{1204,12,"ãê¾©","´óÀí",1201,37,162,1674,3132},
--
----·ïÏèÈ¥ÆäËû³ÇÊĞ(mapid:1)
--{1204,13,"·ïÏè","ÑïÖİ",1202,1,80,1676,3000},
--{1204,14,"·ïÏè","ãê¾©",1202,1,37,1601,3001},
--{1204,15,"·ïÏè","ÁÙ°²",1202,1,176,1592,2925},
--{1204,16,"·ïÏè","³É¶¼",1202,1,11,3021,5090},
--{1204,17,"·ïÏè","ÏåÑô",1202,1,78,1596,3379},
--{1204,18,"·ïÏè","´óÀí",1202,1,162,1674,3132},
--
----ÁÙ°²È¥ÆäËû³ÇÊĞ(mapid:176)
--{1204,19,"ÁÙ°²","ÑïÖİ",1202,176,80,1676,3000},
--{1204,20,"ÁÙ°²","·ïÏè",1202,176,1,1561,3114},
--{1204,21,"ÁÙ°²","ãê¾©",1202,176,37,1601,3001},
--{1204,22,"ÁÙ°²","³É¶¼",1202,176,11,3021,5090},
--{1204,23,"ÁÙ°²","ÏåÑô",1202,176,78,1596,3379},
--{1204,24,"ÁÙ°²","´óÀí",1202,176,162,1674,3132},
--
----³É¶¼È¥ÆäËû³ÇÊĞ(mapid:11)
--{1204,25,"³É¶¼","ÑïÖİ",1203,11,80,1676,3000},
--{1204,26,"³É¶¼","·ïÏè",1203,11,1,1561,3114},
--{1204,27,"³É¶¼","ãê¾©",1203,11,37,1601,3001},
--{1204,28,"³É¶¼","ÁÙ°²",1203,11,176,1592,2925},
--{1204,29,"³É¶¼","ÏåÑô",1203,11,78,1596,3379},
--{1204,30,"³É¶¼","´óÀí",1203,11,162,1674,3132},
--
----ÏåÑôÈ¥ÆäËû³ÇÊĞ(mapid:78)
--{1204,31,"ÏåÑô","ÑïÖİ",1203,78,80,1676,3000},
--{1204,32,"ÏåÑô","·ïÏè",1203,78,1,1561,3114},
--{1204,33,"ÏåÑô","ãê¾©",1203,78,37,1601,3001},
--{1204,34,"ÏåÑô","ÁÙ°²",1203,78,176,1592,2925},
--{1204,35,"ÏåÑô","³É¶¼",1203,78,11,3021,5090},
--{1204,36,"ÏåÑô","´óÀí",1203,78,162,1674,3132},
--
----´óÀíÈ¥ÆäËû³ÇÊĞ(mapid:162)
--{1204,37,"´óÀí","ÑïÖİ",1201,162,80,1676,3000},
--{1204,38,"´óÀí","·ïÏè",1201,162,1,1561,3114},
--{1204,39,"´óÀí","ãê¾©",1202,162,37,1601,3001},
--{1204,40,"´óÀí","ÁÙ°²",1202,162,176,1592,2925},
--{1204,41,"´óÀí","ÏåÑô",1203,162,78,1596,3379},
--{1204,42,"´óÀí","³É¶¼",1203,162,11,3021,5090},

--³É¶¼È¥ÆäËû³ÇÊĞ(mapid:11)
{1204,1," Thµnh §«"," §¹i Lı ",1203,11,162,1674,3132},

--´óÀíÈ¥ÆäËû³ÇÊĞ(mapid:162)
{1204,2," §¹i Lı "," Thµnh §«",1203,162,11,3021,5090},

}

-------------------------------------------ÌØÊâĞÅÊ¹ÈÎÎñµÄÖ÷½çÃæ-------------------------------------------------
function especiallymessenger()
--	if ( nt_getTask(1270) == 0 ) then
--		nt_setTask(1270,1)
--		nt_setTask(1205,0)
--		Msg2Player("ÄúµÄĞÅÊ¹»ı·ÖÒÑ±»Çå¿Õ£¬´ÓÏÖÔÚ¿ªÊ¼Õı³£¼ÆËã»ı·Ö¡£Çë¼ÓÓÍ¡£")
--	end

	--Æß´óÖ÷³Çæä¹ÙÖ»ÓĞ³É¶¼ºÍ´óÀíÓĞÈÎÎñ
	local _, _, nMapIndex = GetPos()
	local MapId = SubWorldIdx2ID( nMapIndex )
	if MapId ~= 11 and MapId ~= 162 then
		Talk(1,"","Tham gia nhiÖm vô Tİn Sø h·y ®i t×m DŞch Quan <color=red>Thµnh §« <color> hoÆc <color=red> §¹i Lı <color>!")
		return
	end
	
	nt_setTask(1211,0)
--	Ladder_NewLadder(10187, GetName(),nt_getTask(1205),1);--»ı·ÖÅÅÃû
	Describe(DescLink_YiGuan..": ViÖc qu©n c¬ cña TriÒu ®×nh gÇn ®©y lu«n bŞ giÆc Kim biÕt tr­íc, ta nghi ngê cã néi gi¸n. Nh­ng tr­íc khi mÊy tªn b¹i lo¹i nµy bŞ ph¸t hiÖn, ta muèn r»ng tin nµy kh¾p n¬i ®Òu biÕt, ng­¬i cã muèn gãp mét phÇn søc cho Quèc gia kh«ng?",7,
	"Ta b»ng lßng!/messenger_ido",
	"Ta ®Õn ®Ó giao nhiÖm vô Tİn sø /messenger_finishtask",
	"Ta muèn xãa bá nhiÖm vô Tİn sø /messenger_losemytask",
	--"ÎÒÒªÇóÉı¼¶ÎÒµÄĞÅÊ¹³ÆºÅ/messenger_getlevel",
	--"ÎÒÒªÓÃ»ı·Ö¶Ò»»ĞÅÊ¹µÄ½±Àø/messenger_duihuanprize",
	"Ta muèn biÕt vÒ nhiÖm vô Tİn sø!/messenger_what",
	"GÇn ®©y ta cã nhiÒu chuyÖn cÇn lµm!/no")
end


-------------------------------------------½ÓÊÜÌØÊâĞÅÊ¹ÈÎÎñµÄÖ÷½çÃæ-----------------------------------------------


function messenger_ido()
	local _, _, nMapIndex = GetPos()
	
	local Uworld1204 = nt_getTask(1204)  --¼ÇÂ¼Íæ¼ÒµÄÈÎÎñ±äÁ¿£¬Ã¿´ÎÈÎÎñ½áÊøÊ±Çå¿Õ
	local Uworld1028 = nt_getTask(1028)  --ÈÎÎñÁ´ÈÎÎñ±äÁ¿
	local MapId = SubWorldIdx2ID( nMapIndex )
	if ( GetLevel() < HD_CFG("TS_CAP_TOITHIEU", 90) ) then	-- [TIN SU 21/08] 120 -> 90 (lenh chu game)
		Describe(DescLink_YiGuan..": Xin l­îng thø, ®¼ng cÊp cña ng­¬i hiÖn t¹i ch­a ®ñ 90. h·y tiÕp tôc t«i luyÖn råi ®Õn gÆp ta.",1," KÕt thóc ®èi tho¹i!/no")	
	elseif ( Uworld1204 ~= 0 )  then
		Describe(DescLink_YiGuan..":Xin lçi! NhiÖm vô Tİn sø cña b¹n ch­a hoµn thµnh, kh«ng thÓ tiÕp nhËn nhiÖm vô gièng nhau, xin hoµn thµnh nhiÖm vô nµy tr­íc, c¸m ¬n!",1," KÕt thóc ®èi tho¹i!/no")
--	elseif (  messenger_givetime() == 10 ) then  --²é¿´½ñÈÕÊÇ·ñ»¹ÓĞ¹Ø¿¨Ê±¼ä
--		Describe(DescLink_YiGuan.."£º¶Ô²»Æğ£¬Äú½ñÌìÔÚĞÅÊ¹ÈÎÎñÖĞµÄÊ±¼äÒÑºÄ¾¡£¬ÇëÃ÷ÈÕÔÙÀ´,Ğ»Ğ»¡£",1,"½áÊø¶Ô»°/no")
	else
    	local nTaskFlag = check_daily_task_count()--¼ì²é»¹ÓĞÃ»ÓĞÈÎÎñ»ú»á
    	if nTaskFlag == 0 then
    		Describe(DescLink_YiGuan..": Xin l­îng thø, H«m nay ng­¬i ®· vÊt v¶ råi. Ngµy mai h·y quay l¹i nhĞ.",1," KÕt thóc ®èi tho¹i!/no")
    		return
    	elseif nTaskFlag == -1 then
    		Describe(DescLink_YiGuan..": H«m nay ng­¬i ®· mÖt mái råi. NÕu ng­¬i cã <color=yellow>Thiªn Khè B¶o LÖnh<color>, ta sÏ ph¸ lÖ cho ng­¬i tham gia thªm mét lÇn n÷a.",1," KÕt thóc ®èi tho¹i!/no")
    		return
    	elseif nTaskFlag == 2 then
    		Msg2Player("Ng­¬i cø cÇm Thiªn Khè B¶o LÖnh tham gia nhiÖm vô, <color=yellow>nhiÖm vô hoµn thµnh<color> ta sÏ thu håi LÖnh Bµi")
    	end
    	
    	WriteLog(format("Account:%s[Name:%s] tiÕp nhËn nhiÖm vô Tİn Sø, h«m nay ®· hoµn thµnh [%d] lÇn.",
				GetAccount(),
				GetName(),
				nTaskFlag
				)
			);
	if nTaskFlag == 2 then
		tbLog:PlayerActionLog("TinhNangKey","NhanNhiemVuTinSuSuDungThienBaoKhoLenh")		
	else
		tbLog:PlayerActionLog("TinhNangKey","NhanNhiemVuTinSu")
	end		
-- ¸ù¾İÍæ¼ÒËùÔÚµØÍ¼Ëæ»ú³öÒ»¸öµ½´ï³ÇÊĞ£¬²¢½«¸ÃÁ½³ÇÊĞÖ®¼äµÄ¶ÔÓ¦±äÁ¿ºÍ¶ÔÓ¦¹Ø¿¨ÉèÖÃÊıÖµ
--		if ( MapId == 80 ) then
--			local CityId = random(1,6)
--			for i=1,6 do
--				if ( messenger_choice(CityId,i) == 10 ) then
--					break
--				end
--			end
--		elseif ( MapId == 37 ) then
--			local CityId = random(7,12)
--			for i=7,12 do
--				if ( messenger_choice(CityId,i) == 10 ) then
--					break
--				end
--			end
--		elseif ( MapId == 1 ) then
--			local CityId = random(13,18)
--			for i=13,18 do
--				if ( messenger_choice(CityId,i) == 10 ) then
--					break
--				end
--			end	
--		elseif ( MapId == 176 ) then
--			local CityId = random(19,24)
--			for i=19,24 do
--				if ( messenger_choice(CityId,i) == 10 ) then
--					break
--				end
--			end		
		if ( MapId == 11 ) then
			if ( messenger_choice(1,1) ~= 10 ) then
					print("messenger_choice error:1")
			end
--			local CityId = random(25,30)
--			for i=25,30 do
--				if ( messenger_choice(CityId,i) == 10 ) then
--					break
--				end
--			end		
--		elseif ( MapId == 78 ) then
--			local CityId = random(31,36)
--			for i=31,36 do
--				if ( messenger_choice(CityId,i) == 10 ) then
--					break
--				end
--			end
		elseif ( MapId == 162 ) then
			if ( messenger_choice(2,2) ~= 10 ) then
					print("messenger_choice error:2")
			end
--			local CityId = random(37,42)
--			for i=37,42 do
--				if ( messenger_choice(CityId,i) == 10 ) then
--					break
--				end
--			end
		else
			Msg2Player("Xin lçi! Kh«ng biÕt t¹i sao b¹n l¹i kh«ng thÓ nhËn nhiÖm vô Tİn sø? Xin liªn hÖ víi GM!")
		end
	end
end

--Ç°ÍùÇ§±¦¿â
function messenger_storewagoner()
	SetRevPos(11,10)
	
	NewWorld(395,1417,3207)
end

-----------------------------------------------¸ø·ûºÏÒªÇóµÄÍæ¼ÒÒ»¸öÌØÊâĞÅÊ¹ÈÎÎñ£¬²¢¸Ä±äÆäĞÅÊ¹ÈÎÎñ±äÁ¿ºÍµ½´ï¹Ø¿¨±äÁ¿------------------------
function messenger_choice(CityIdTwo,j)
	if ( CityIdTwo == citygo[j][2] ) then	
		--ÉèÖÃÓÉ´ËµØÈ¥±ËµØµÄÈÎÎñ±äÁ¿	
		nt_setTask( 1204,citygo[j][2] )
		--ÉèÖÃÈ¥µ½ÄÄ¸ö¹Ø¿¨µÄÈÎÎñ±äÁ¿   
		nt_setTask( citygo[j][5],10) --ÉèÖÃĞÅÊ¹ÈÎÎñµÄ²½Öè
		local name = GetName()
		Talk(1,"","B¹n nhËn ®­îc tõ "..citygo[j][3].."®Õn"..citygo[j][4].."cña nhiÖm vô Tİn sø ®Æc biÖt,"..citygo[j][3].."DŞch tr¹m DŞch quan cung tèng"..name.."§¹i nh©n!")		
		return 10
	end
end


---------------------------------------------------Éı¼¶ĞÅÊ¹³ÆºÅ----------------------------------------------------------------------
--function messenger_getlevel()
--	local Uworld1205 = nt_getTask(1205)
--	local Uworld1206 = nt_getTask(1206)
--	local messenger_reallevelname = ""
--	if ( Uworld1206 == 1 ) then
--		messenger_reallevelname = "Ä¾ÅÆĞÅÊ¹"
--	elseif ( Uworld1206 == 2 ) then
--		messenger_reallevelname = "Í­ÅÆĞÅÊ¹"
--	elseif ( Uworld1206 == 3 ) then
--		messenger_reallevelname = "ÒøÅÆĞÅÊ¹"
--	elseif ( Uworld1206 == 4 ) then
--		messenger_reallevelname = "½ğÅÆĞÅÊ¹"
--	elseif ( Uworld1206 == 5 ) then
--		messenger_reallevelname = "Óù´Í½ğÅÆĞÅÊ¹"
--	else	
--		messenger_reallevelname = "Ã»ÓĞÈÎºÎ³ÆºôµÄĞÅÊ¹"
--	end 
--	Describe(DescLink_YiGuan.."£ºÄúÄ¿Ç°ÊÇ"..messenger_reallevelname.."£¬ÄúµÄĞÅÊ¹»ı·ÖÎª"..Uworld1205.."µã£¬ÄúÏëÒªÉı¼¶ÎªÄÄÖÖĞÅÊ¹ÄØ£¿",6,
--		"Ä¾ÅÆĞÅÊ¹[50µã]/messenger_levelmu",
--		"Í­ÅÆĞÅÊ¹[150µã]/messenger_leveltong",
--		"ÒøÅÆĞÅÊ¹[450µã]/messenger_levelyin",
--		"½ğÅÆĞÅÊ¹[800µã]/messenger_leveljin",
--		"Óù´Í½ğÅÆĞÅÊ¹[1500µã]/messenger_levelyuci",
--		"ÎÒÖ»ÊÇËæ±ã¿´¿´/no")
--end

function messenger_levelmu()
	local Uworld1205 = nt_getTask(1205)
	local Uworld1206 = nt_getTask(1206)
	local name = GetName()
	if ( Uworld1205 >= 50 ) then
		if ( Uworld1206 < 1 ) then
			nt_setTask(1206,1)
			nt_setTask(1205,Uworld1205-50)
			Describe(DescLink_YiGuan..": Chóc mõng"..name.."§¹i nh©n trë thµnh Tİn sø Méc!",1," KÕt thóc ®èi tho¹i!/no")
		elseif ( Uworld1206 == 1 ) then
			Describe(DescLink_YiGuan..":"..name.."! B¹n ®· lµ Tİn sø - Méc, kh«ng cÇn t¨ng cÊp, xin h·y cè g¾ng!",1," KÕt thóc ®èi tho¹i!/no")		
		else
			Describe(DescLink_YiGuan..":"..name.."! §¼ng cÊp cña b¹n v­ît qu¸ Tİn sø - Méc, xin h·y chän Tİn sø kh¸c.",2,"Quay l¹i/messenger_getlevel"," KÕt thóc ®èi tho¹i!/no")				
		end
	else
		Describe(DescLink_YiGuan..":Xin lçi!  B¹n kh«ng phï hîp víi ®iÒu kiÖn t¨ng cÊp cña Tİn sø - Méc, xin h·y nç lùc, c¸m ¬n!",2,"Quay l¹i/messenger_getlevel"," KÕt thóc ®èi tho¹i!/no")
	end
end

function messenger_leveltong()
	local Uworld1205 = nt_getTask(1205)
	local Uworld1206 = nt_getTask(1206)
	local name = GetName()
	if ( Uworld1205 >= 150 )  then
		if ( Uworld1206 < 2 ) then
			nt_setTask(1206,2)
			nt_setTask(1205,Uworld1205-150)
			Describe(DescLink_YiGuan..": Chóc mõng"..name.."§¹i nh©n th¨ng thµnh Tİn sø - §ång!",1," KÕt thóc ®èi tho¹i!/no")
		elseif ( Uworld1206 == 2 ) then
			Describe(DescLink_YiGuan..":"..name.."! Ban ®· lµ Tİn sø - §ång, kh«ng cÇn t¨ng cÊp n÷a, xin h·y tiÕp tôc nç lùc!",1," KÕt thóc ®èi tho¹i!/no")				
		else
			Describe(DescLink_YiGuan..":"..name.."! §¼ng cÊp cña b¹n ®· v­ît qu¸ Tİn sø-§ång, xin h·y chän Tİn sø kh¸c.",2,"Quay l¹i/messenger_getlevel"," KÕt thóc ®èi tho¹i!/no")				
		end
	else
		Describe(DescLink_YiGuan..":Xin lçi!  §¼ng cÊp cña b¹n kh«ng phï hîp ®iÒu kiÖn t¨ng cÊp cña Tİn sø - §ång, xin h·y tiÕp tôc nç lùc, c¸m ¬n!",2,"Quay l¹i/messenger_getlevel"," KÕt thóc ®èi tho¹i!/no")
	end
end

function messenger_levelyin()
	local Uworld1205 = nt_getTask(1205)
	local Uworld1206 = nt_getTask(1206)
	local name = GetName()
	if ( Uworld1205 >= 450 ) then
		if ( Uworld1206 < 3 ) then
			nt_setTask(1206,3)
			nt_setTask(1205,Uworld1205-450)
			Describe(DescLink_YiGuan..": Chóc mõng"..name.."§¹i nh©n th¨ng thµnh Tİn sø - B¹c!",1," KÕt thóc ®èi tho¹i!/no")
		elseif ( Uworld1206 == 3 ) then
			Describe(DescLink_YiGuan..":"..name.."! B¹n ®· lµ Tİn sø - B¹c, kh«ng cÇn th¨ng cÊp, h·y tiÕp tôc cè g¾ng!",1," KÕt thóc ®èi tho¹i!/no")				
		else
			Describe(DescLink_YiGuan..":"..name.."! §¼ng cÊp cña b¹n ®· v­ît qua Tİn Sø - B¹c, xin h·y chän Tİn sø kh¸c!",2,"Quay l¹i/messenger_getlevel"," KÕt thóc ®èi tho¹i!/no")				
		end
	else
		Describe(DescLink_YiGuan..":Xin lçi!  B¹n kh«ng phï hîp víi ®iÒu kiÖn th¨ng cÊp cña Tİn sø - B¹c, xin b¹n h·y tiÕp tôc cè g¾ng, c¸m ¬n!",2,"Quay l¹i/messenger_getlevel"," KÕt thóc ®èi tho¹i!/no")
	end
end

function messenger_leveljin()
	local Uworld1205 = nt_getTask(1205)
	local Uworld1206 = nt_getTask(1206)
	local name = GetName()
	if ( Uworld1205 >= 800 ) then
		if ( Uworld1206 < 4 ) then
			nt_setTask(1206,4)
			nt_setTask(1205,Uworld1205-800)
			Describe(DescLink_YiGuan..": Chóc mõng"..name.."§¹i nh©n th¨ng thµnh Tİn sø - Vµng!",1," KÕt thóc ®èi tho¹i!/no")
		elseif ( Uworld1206 == 4 ) then
			Describe(DescLink_YiGuan..":"..name.."! B¹n ®· lµ Tİn sø - Vµng, kh«ng cÇn t¨ng cÊp n÷a, h·y tiÕp tôc cè g¾ng!",1," KÕt thóc ®èi tho¹i!/no")		
		else
			Describe(DescLink_YiGuan..":"..name.."! §¼ng cÊp cña b¹n ®· v­ît qu¸ Tİn sø - Vµng, xin chän Tİn sø kh¸c!",2,"Quay l¹i/messenger_getlevel"," KÕt thóc ®èi tho¹i!/no")				
		end
	else
		Describe(DescLink_YiGuan..":Xin lçi!  B¹n kh«ng phï hîp víi ®iÒu kiÖn th¨ng cÊp cña Tİn sø - Vµng, h·y tiÕp tôc cè g¾ng, c¸m ¬n!",2,"Quay l¹i/messenger_getlevel"," KÕt thóc ®èi tho¹i!/no")
	end
end

function messenger_levelyuci()
	local Uworld1205 = nt_getTask(1205)
	local Uworld1206 = nt_getTask(1206)
	local name = GetName()
	if ( Uworld1205 >= 1500 ) then
		if ( Uworld1206 < 5 ) then
			nt_setTask(1206,5)
			nt_setTask(1205,Uworld1205-1500)
			Describe(DescLink_YiGuan..": Chóc mõng"..name.."§¹i nh©n th¨ng thµnh Ngù Tø Tİn sø yªu bµi!",1," KÕt thóc ®èi tho¹i!/no")
		elseif ( Uworld1206 == 5 ) then
			Describe(DescLink_YiGuan..":"..name.."B¹n ®· lµ Ngù Tø Tİn sø - Vµng, kh«ng cÇn t¨ng cÊp n÷a, xin h·y tiÕp tôc cè g¾ng!",1," KÕt thóc ®èi tho¹i!/no")				
		end
	else
		Describe(DescLink_YiGuan..":Xin lçi! B¹n kh«ng phï hîp ®iÒu kiÖn víi ®iÒu kiÖn t¨ng cÊp cña Ngù Tø Tİn sø - Vµng, xin h·y tiÕp tôc cè g¾ng, c¸m ¬n!",2,"Quay l¹i/messenger_getlevel"," KÕt thóc ®èi tho¹i!/no")
	end
end

--------------------------------------------½»ĞÅÊ¹ÈÎÎñ------------------------------------------------------
function messenger_finishtask()
	local Uworld1201= nt_getTask(1201)	--ËÍĞÅÈÎÎñ·çÖ®ÆïÈÎÎñ±äÁ¿
	local Uworld1202= nt_getTask(1202)	--ËÍĞÅÈÎÎñÉ½ÉñÃíÈÎÎñ±äÁ¿
	local Uworld1203= nt_getTask(1203)	--ËÍĞÅÈÎÎñÇ§±¦¿âÈÎÎñ±äÁ¿
	local Uworld1204= nt_getTask(1204)	--ËÍĞÅÈÎÎñËùµ½µØµãÈÎÎñ±äÁ¿
--	local Uworld1205= nt_getTask(1205)	--ĞÅÊ¹»ı·Ö
--	local Uworld1206= nt_getTask(1206)	--ĞÅÊ¹³ÆºÅ
--	local Uworld1208= nt_getTask(1208)	--·çÖ®ÆïÉ±¹Ö¼ÇÊıÆ÷
--	local Uworld1209= nt_getTask(1209)	--É½ÉñÃíÉ±¹Ö¼ÇÊıÆ÷
--	local Uworld1210= nt_getTask(1210)	--Ç§±¦¿âÉ±¹Ö¼ÇÊıÆ÷
	local Uworld1218= nt_getTask(1218)  --ĞÅÊ¹ÈÎÎñÍê³É´ÎÊı¼ÇÊıÆ÷

--	if ( Uworld1201 == 30 ) then
--		if ( messenger_gettime() == 10 ) then
--			Describe(DescLink_YiGuan.."£º¶Ô²»Æğ£¬ÄãÃ»ÓĞÔÚÍê³ÉµÄµ±ÌìÀ´ÁìÈ¡½±Àø£¬Õâ¸öÈÎÎñ±»¿´×öÊ§°ÜÁË£¡",1,"½áÊø¶Ô»°/no")
--		else
--			nt_setTask(1218,Uworld1218+1)
--			Describe(DescLink_YiGuan.."£º¹§Ï²ÄúÒÑ¾­Íê³ÉÁËµ±Ç°ÈÎÎñ£¡",1,"ÁìÈ¡½±Àø/messenger_flyprize")
--		end
--	elseif ( Uworld1202 == 30 ) then
--		if ( messenger_gettime() == 10 ) then
--			Describe(DescLink_YiGuan.."£º¶Ô²»Æğ£¬ÄãÃ»ÓĞÔÚÍê³ÉµÄµ±ÌìÀ´ÁìÈ¡½±Àø£¬Õâ¸öÈÎÎñ±»¿´×öÊ§°ÜÁË£¡",1,"½áÊø¶Ô»°/no")
--		else
--			nt_setTask(1218,Uworld1218+1)
--			Describe(DescLink_YiGuan.."£º¹§Ï²ÄúÒÑ¾­Íê³ÉÁËµ±Ç°ÈÎÎñ£¡",1,"ÁìÈ¡½±Àø/messenger_templeprize")
--		end
	if ( Uworld1203 == 30 or Uworld1203 == 25 ) then
		if ( messenger_gettime() == 10 ) then
			Describe(DescLink_YiGuan..":Xin lçi!  B¹n ch­a hoµn thµnh nhiÖm vô mµ h«m tr­íc ®Õn l·nh th­ëng, nhiÖm vô nµy coi nh­ ®· thÊt b¹i råi!",1," KÕt thóc ®èi tho¹i!/no")
		else
			nt_setTask(1218,Uworld1218+1)
			Describe(DescLink_YiGuan..":  Chóc mõng b¹n ®· hoµn thµnh nhiÖm vô!",1,"NhËn l·nh phÇn th­ëng/messenger_treasureprize")
		end
--	elseif ( Uworld1201 == 25 ) or ( Uworld1202 == 25 ) or ( Uworld1203 == 25 ) then   --Íê³ÉÁË¼òµ¥ĞÅÊ¹ÈÎÎñ
--		if ( messenger_gettime() == 10 ) then
--			Describe(DescLink_YiGuan.."£º¶Ô²»Æğ£¬ÄãÃ»ÓĞÔÚÍê³ÉµÄµ±ÌìÀ´ÁìÈ¡½±Àø£¬Õâ¸öÈÎÎñ±»¿´×öÊ§°ÜÁË£¡",1,"½áÊø¶Ô»°/no")
--		else
--			nt_setTask(1218,Uworld1218+1)
--			Describe(DescLink_YiGuan.."£º¹§Ï²ÄúÒÑ¾­Íê³ÉÁËµ±Ç°ÈÎÎñ£¡",1,"ÁìÈ¡½±Àø/messenger_simpleprize")	
--		end
--	elseif ( Uworld1201 ~= 0 or Uworld1202 ~= 0 or Uworld1203 ~= 0 ) then
--		Describe(DescLink_YiGuan.."£ºÄúµÄĞÅÊ¹ÈÎÎñ»¹Ã»ÓĞÍê³É£¬Çë¼ÌĞøÅ¬Á¦£¡",1,"½áÊø¶Ô»°/no")
	elseif Uworld1203 ~= 0 then
		Describe(DescLink_YiGuan..": Xin h·y l­îng thø, ng­¬i vÉn ch­a hoµn thµnh nhiÖm vô hiÖn t¹i. L·o Xa Phu bªn c¹nh ta cã thÓ ®­a ng­¬i ®Õn b¶n ®å nhiÖm vô !",1," KÕt thóc ®èi tho¹i!/no")
	else
		Describe(DescLink_YiGuan..": Ch­a tiÕp nhËn nhiÖm vô nµo ng­¬i cßn l¹i tr¶ nhiÖm vô! Ng­êi ®©u? B»m c¸i tªn nµy lµm nh©n b¸nh bao cho ta!",1," KÕt thóc ®èi tho¹i!/no")
	end
end

function messenger_flyprize()
--	Ladder_NewLadder(10187, GetName(),nt_getTask(1205),1);
--	local Uworld1207 = nt_getTask(1207)
--	local i = random(1,5)
--	AddRepute(i)
--	Msg2Player("¹§Ï²Äã»ñµÃÁË"..i.."µãÉùÍû¡£")
--	if ( nt_getTask(1224) ~= 1 ) then
--		if ( GetLevel()>=60 and GetLevel()<=89 ) then
--			if ( nt_getTask(1223) >= 150 ) then
--				messenger_giveplayerexp(1500000)
--			end
--		elseif ( GetLevel()>=90 and GetLevel()<=119 ) then
--			if ( nt_getTask(1223) >= 200 ) then
--				messenger_giveplayerexp(2000000)
--			end			
--		elseif ( GetLevel()>=120 and GetLevel()<=129 ) then
--			if ( nt_getTask(1223) >= 300 ) then
--				messenger_giveplayerexp(3000000)
--			end		
--		elseif ( GetLevel()>=130 ) then
--			if ( nt_getTask(1223) >= 600 ) then
--				messenger_giveplayerexp(6000000)
--			end		
--		end
--	end
--	
--	nt_setTask(1201,0)	--ËÍĞÅÈÎÎñ·çÖ®ÆïÈÎÎñ±äÁ¿
--	nt_setTask(1204,0)	--ËÍĞÅÈÎÎñËùµ½µØµãÈÎÎñ±äÁ¿
--	nt_setTask(1207,0)	--ËÍĞÅÈÎÎñµ±Ç°É±¹Ö¼ÇÊı
--	nt_setTask(1211,0)
end

function messenger_templeprize()
-- 	Ladder_NewLadder(10187, GetName(),nt_getTask(1205),1);
--	local Uworld1207 = nt_getTask(1207)
--	local i = random(1,5)
--	AddRepute(i)
--	Msg2Player("¹§Ï²Äã»ñµÃÁË"..i.."µãÉùÍû¡£")
--	if ( nt_getTask(1224) ~= 1 ) then
--		if ( GetLevel()>=60 and GetLevel()<=89 ) then
--			if ( nt_getTask(1223) >= 150 ) then
--				messenger_giveplayerexp(1500000)
--			end
--		elseif ( GetLevel()>=90 and GetLevel()<=119 ) then
--			if ( nt_getTask(1223) >= 200 ) then
--				messenger_giveplayerexp(2000000)
--			end			
--		elseif ( GetLevel()>=120 and GetLevel()<=129 ) then
--			if ( nt_getTask(1223) >= 300 ) then
--				messenger_giveplayerexp(3000000)
--			end		
--		elseif ( GetLevel()>=130 ) then
--			if ( nt_getTask(1223) >= 600 ) then
--				messenger_giveplayerexp(6000000)
--			end		
--		end
--	end
--	
--	nt_setTask(1202,0)	--ËÍĞÅÈÎÎñÉ½ÉñÃíÈÎÎñ±äÁ¿
--	nt_setTask(1204,0)	--ËÍĞÅÈÎÎñËùµ½µØµãÈÎÎñ±äÁ¿
--	nt_setTask(1207,0)	--ËÍĞÅÈÎÎñµ±Ç°É±¹Ö¼ÇÊı
--	nt_setTask(1211,0)
end

function messenger_treasureprize()
	-- [BAC DAU 25/08] Linux beidouactivity.lua:450-458 OnFinishMessenger.
	if (HD3_BD_TinSu ~= nil) then
		HD3_BD_TinSu()
	end
  local nTaskFlag = check_daily_task_count()--¼ì²é»¹ÓĞÃ»ÓĞÈÎÎñ»ú»á
  if nTaskFlag == 0 then
  	Describe(DescLink_YiGuan..": Xin h·y l­îng thø, h«m nay ng­¬i ®· qu¸ mÖt råi. Hay lµ ngµy mai l¹i ®Õn ®æi nhiÖm vô nhĞ.",1," KÕt thóc ®èi tho¹i!/no")
  	return
  elseif nTaskFlag == -1 then
  	Describe(DescLink_YiGuan..": Cã ph¶i ng­¬i ®· lÊy <color=yellow> Thiªn Khè B¶o LÖnh <color> lµm mÊt råi h¶? T×m LÖnh Bµi vÒ ®©y ta míi th­ëng cho ng­¬i.",1," KÕt thóc ®èi tho¹i!/no")
  	return
  end
  
  --¼ì²é±³°ü¿Õ¼ä
  local nTodayTaskCount = %get_task_daily(4128)
  local nBagCellNeed = 5
--  if nTodayTaskCount == 0 then
--  	nBagCellNeed = 5
--  end
  if (CalcFreeItemCellCount() < nBagCellNeed) then
		Talk(1, "", format("<#>Hµnh trang cña ng­¬i kh«ng ®ñ chç trèng, h·y s¾p xÕp l¹i hµnh trang cßn trèng %d « råi h·y ®Õn nhËn th­ëng nhĞ.",nBagCellNeed))
		return
	end
	local n_level = GetLevel();
	
	--¿Û³ıÇ§±¦¿âÁî
	if nTaskFlag == 2 then
		--Fix bug kh«ng trõ ®­îc thiªn b¶o khè lÖnh trong r­¬ng bang héi - Modifiec by DinhHQ - 20110502
    	if ConsumeEquiproomItem(1, 6, 1, 3431, -1) == 1 then--¿Û³ıÇ§±¦¿âÁî
    		Msg2Player("Ng­¬i ®· giao 1 Thiªn B¶o Khè LÖnh")
    	else
    		Talk(1, "", MSG_CONSUME_ITEM2813_FAIL)
    		return
    	end    	
    end
    
   	%add_task_daily(4128, 1)
	nt_setTask(1201,0)	--
	nt_setTask(1202,0)
	nt_setTask(1203,0)
	nt_setTask(1204,0)
	G_ACTIVITY:OnMessage("FinishMail", 1, n_level);	
	EventSys:GetType("Messenger"):OnPlayerEvent("OnFinish", PlayerIndex)
	nTodayTaskCount = %get_task_daily(4128)--»ñÈ¡½»ÈÎÎñºóµÄ¼ÆÊıÖµ
	WriteLog(format("Account:%s[Name:%s] nhËn phÇn th­ëng nhiÖm vô Tİn Sø, h«m nay ®· hoµn thµnh [%d] lÇn.",
			GetAccount(),
			GetName(),
			nTodayTaskCount
			)
		);
	
--	local n_level = GetLevel();

	--Ã¿ÌìµÚÒ»´ÎÍê³ÉÈÎÎñ½±Àø3¸öĞĞÏÀÁî
	if nTodayTaskCount == 1 then
		for i = 1, HD_CFG("TS_TRA_HANHHIEP", 3) do
			AddItem(6,1,2575,1,0,0,0)--»ñÈ¡µÄĞĞÏÀÁîÎª²»°ó¶¨×´Ì¬
		end;
		Msg2Player("Ng­¬i nhËn ®­îc "..HD_CFG("TS_TRA_HANHHIEP", 3).." Hµnh HiÖp LÖnh")
	end
	
	--Ã¿´Î½»ÈÎÎñ½±Àø2¸öĞÅÊ¹±¦Ïä
	for i = 1, HD_CFG("TS_TRA_BAORUONG", 2) do
		AddItem(6,1,3430,0,0,0,0)
	end;
	Msg2Player("Ng­¬i nhËn ®­îc "..HD_CFG("TS_TRA_BAORUONG", 2).." Tİn Sø B¶o R­¬ng")
	if PlayerFunLib:CheckTaskDaily(3073,2,"nomsg","<") == 1 then
		PlayerFunLib:AddTaskDaily(3073,1)
		PlayerFunLib:GetItem({tbProp={6,1,4847,1,0,0},nBindState=-2},2,"KinhMach", "TinSuNhanNguyenLieu")
	end
end

function messenger_simpleprize()

--	Ladder_NewLadder(10187, GetName(),nt_getTask(1205),1);
--	local i = random(1,5)
--	AddRepute(i)
--	Msg2Player("¹§Ï²Äã»ñµÃÁË"..i.."µãÉùÍû¡£")
--	nt_setTask(1201,0)	
--	nt_setTask(1202,0)	
--	nt_setTask(1203,0)	
--	nt_setTask(1204,0)
--	nt_setTask(1207,0)
--	nt_setTask(1211,0)
		
end

--------------------------------------------»ı·Ö»»½±Àø-----------------------------------------------------
function messenger_duihuanprize()
	Describe(DescLink_YiGuan..": PhÇn th­ëng gåm cã: Tİn sø yªu bµi vµ Trang bŞ Hoµng Kim. Yªu bµi cã thÓ t¨ng kh¸ng tİnh cña nh©n vËt, trang bŞ Hoµng Kim cã thÓ t¨ng kinh nghiÖm. B¹n muèn chän lo¹i nµo?",3,
		"Tİn sø yªu bµi /messenger_prize_yaopai",
		"Trang bŞ Hoµng Kim/messenger_prize_huangjin",
--		"Í¬°éÏà¹Ø/messenger_prize_partner",
--		"ÆäËûÎïÆ·/messenger_prize_other",
		"Sau nµy h·y nãi!/no")
end

function messenger_prize_partner()
	Describe(DescLink_YiGuan..": Thñ lÜnh Long ngò võa míi mang vÒ mét sè <color=red>MÆt n¹ §ång hµnh kú bİ <color>, ng­êi cã muèn dïng thö kh«ng?",5,
		"Kü n¨ng §ång hµnh/no",
		"MÆt n¹ §ång hµnh/messenger_prize_mianju",
		"VËt phÈm §ång hµnh/no",
		"Quay l¹i/messenger_duihuanprize",
		"Sau nµy h·y nãi!/no")
end

function messenger_prize_mianju()
	Describe(DescLink_YiGuan..": MÆt n¹ ®ång hµnh chia thµnh 2 lo¹i: 1 lÇn sö dung vµ 10 lÇn sö dông.  Muèn ®æi c¸i nµo?",4,
	"§æi MÆt n¹ 1 lÇn/prize_mianju_dan",
	"§æi MÆt n¹ 10 lÇn/prize_mianju_shi",
	"Quay l¹i/messenger_prize_partner",
	"T¹m thêi kh«ng ®æi ®­îc!/no")
end

function prize_mianju_shi()
	Describe(DescLink_YiGuan..": B¹n muèn ®æi MÆt n¹ (10 lÇn) ThiÕu Niªn Vâ sÜ L«i KiÕm hay lµ Thanh Niªn Vâ sÜ L«i KiÕm?  Xem râ ch­a ch­a?",4,
	"§æi MÆt n¹ ThiÕu Niªn Vâ SÜ /prize_mianju_shi_shaonian",
	"§æi MÆt n¹ Thanh Niªn Vâ /prize_mianju_shi_qingnian",
	"Quay l¹i/messenger_prize_mianju",
	"T¹m thêi kh«ng ®æi ®­îc!/no")
end

function prize_mianju_shi_shaonian()
	Describe(DescLink_YiGuan..": B¾t ®Çu ®æi ®©y!",7,
	"MÆt n¹ (10 lÇn) - ThiÕu Niªn L«i KiÕm [40000 phót]/prize_mianju_shi_shaolin_leijian",
	"MÆt n¹ (10 lÇn) - ThiÕu Niªn TiÕu S­¬ng [40000 phót]/prize_mianju_shi_shaolin_xiaoshuang",
	"MÆt n¹ (10 lÇn) - ThiÕu Niªn Háa NhËn [40000 phót]/prize_mianju_shi_shaolin_huoren",
	"MÆt n¹ (10 lÇn) - ThiÕu Niªn H¶i §­êng [40000 phót]/prize_mianju_shi_shaolin_haitang",
	"MÆt n¹ (10 lÇn) - ThiÕu Niªn Kim Phong [40000 phót]/prize_mianju_shi_shaolin_jinfeng",
	"Quay l¹i/prize_mianju_shi",
	"T¹m thêi kh«ng ®æi ®­îc!/no")
end

function prize_mianju_shi_qingnian()
	Describe(DescLink_YiGuan..": B¾t ®Çu ®æi ®©y!",7,
	"MÆt n¹ (10 lÇn) - Thanh Niªn L«i KiÕm [40000 phót]/prize_mianju_shi_qinglin_leijian",
	"MÆt n¹ (10 lÇn) - Thanh Niªn TiÕu S­¬ng [40000 phót]/prize_mianju_shi_qinglin_xiaoshuang",
	"MÆt n¹ (10 lÇn) - Thanh Niªn Háa NhËn [40000 phót]/prize_mianju_shi_qinglin_huoren",
	"MÆt n¹ (10 lÇn) - Thanh Niªn H¶i §­êng [40000 phót]/prize_mianju_shi_qinglin_haitang",
	"MÆt n¹ (10 lÇn) - Thanh Niªn Kim Phong [40000 phót]/prize_mianju_shi_qinglin_jinfeng",
	"Quay l¹i/prize_mianju_shi",
	"T¹m thêi kh«ng ®æi ®­îc!/no")
end

function prize_mianju_shi_qinglin_leijian()
	if ( nt_getTask(1205) >= 40000 ) then
		AddItem(0,11,112,0,0,0,0)
		local Uworld1205 = nt_getTask(1205) - 40000
		nt_setTask(1205,Uworld1205)
		Msg2Player("B¹n nhËn ®­îc 1 MÆt n¹ ®ång hµnh")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end 
end

function prize_mianju_shi_qinglin_xiaoshuang()
	if ( nt_getTask(1205) >= 40000 ) then
		AddItem(0,11,113,0,0,0,0)
		local Uworld1205 = nt_getTask(1205) - 40000
		nt_setTask(1205,Uworld1205)
		Msg2Player("B¹n nhËn ®­îc 1 MÆt n¹ ®ång hµnh")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end 
end

function prize_mianju_shi_qinglin_huoren()
	if ( nt_getTask(1205) >= 40000 ) then
		AddItem(0,11,114,0,0,0,0)
		local Uworld1205 = nt_getTask(1205) - 40000
		nt_setTask(1205,Uworld1205)
		Msg2Player("B¹n nhËn ®­îc 1 MÆt n¹ ®ång hµnh")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end 
end

function prize_mianju_shi_qinglin_haitang()
	if ( nt_getTask(1205) >= 40000 ) then
		AddItem(0,11,115,0,0,0,0)
		local Uworld1205 = nt_getTask(1205) - 40000
		nt_setTask(1205,Uworld1205)
		Msg2Player("B¹n nhËn ®­îc 1 MÆt n¹ ®ång hµnh")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end 
end

function prize_mianju_shi_qinglin_jinfeng()
	if ( nt_getTask(1205) >= 40000 ) then
		AddItem(0,11,116,0,0,0,0)
		local Uworld1205 = nt_getTask(1205) - 40000
		nt_setTask(1205,Uworld1205)
		Msg2Player("B¹n nhËn ®­îc 1 MÆt n¹ ®ång hµnh")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end 
end

function prize_mianju_shi_shaolin_leijian()
	if ( nt_getTask(1205) >= 40000 ) then
		AddItem(0,11,102,0,0,0,0)
		local Uworld1205 = nt_getTask(1205) - 40000
		nt_setTask(1205,Uworld1205)
		Msg2Player("B¹n nhËn ®­îc 1 MÆt n¹ ®ång hµnh")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end 
end

function prize_mianju_shi_shaolin_xiaoshuang()
	if ( nt_getTask(1205) >= 40000 ) then
		AddItem(0,11,103,0,0,0,0)
		local Uworld1205 = nt_getTask(1205) - 40000
		nt_setTask(1205,Uworld1205)
		Msg2Player("B¹n nhËn ®­îc 1 MÆt n¹ ®ång hµnh")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end 
end

function prize_mianju_shi_shaolin_huoren()
	if ( nt_getTask(1205) >= 40000 ) then
		AddItem(0,11,104,0,0,0,0)
		local Uworld1205 = nt_getTask(1205) - 40000
		nt_setTask(1205,Uworld1205)
		Msg2Player("B¹n nhËn ®­îc 1 MÆt n¹ ®ång hµnh")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end 
end

function prize_mianju_shi_shaolin_haitang()
	if ( nt_getTask(1205) >= 40000 ) then
		AddItem(0,11,105,0,0,0,0)
		local Uworld1205 = nt_getTask(1205) - 40000
		nt_setTask(1205,Uworld1205)
		Msg2Player("B¹n nhËn ®­îc 1 MÆt n¹ ®ång hµnh")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end 
end

function prize_mianju_shi_shaolin_jinfeng()
	if ( nt_getTask(1205) >= 40000 ) then
		AddItem(0,11,106,0,0,0,0)
		local Uworld1205 = nt_getTask(1205) - 40000
		nt_setTask(1205,Uworld1205)
		Msg2Player("B¹n nhËn ®­îc 1 MÆt n¹ ®ång hµnh")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end 
end

function prize_mianju_dan()
	Describe(DescLink_YiGuan..": B¹n muèn ®æi MÆt n¹ (1 lÇn) ThiÕu Niªn Vâ sÜ L«i KiÕm hay lµ Thanh Niªn Vâ sÜ L«i KiÕm?  Xem râ ch­a ch­a?",4,
	"§æi MÆt n¹ ThiÕu Niªn Vâ SÜ /prize_mianju_dan_shaonian",
	"§æi MÆt n¹ Thanh Niªn Vâ /prize_mianju_dan_qingnian",
	"Quay l¹i/messenger_prize_mianju",
	"T¹m thêi kh«ng ®æi ®­îc!/no")
end

function prize_mianju_dan_qingnian()
	Describe(DescLink_YiGuan..": B¾t ®Çu ®æi ®©y.",7,
	"MÆt n¹ (1 lÇn) - Thanh Niªn L«i KiÕm [10000 phót]/prize_mianju_dan_qinglin_leijian",
	"MÆt n¹ (1 lÇn) - Thanh Niªn TiÕu S­¬ng [10000 phót]/prize_mianju_dan_qinglin_xiaoshuang",
	"MÆt n¹ (1 lÇn) - Thanh Niªn Háa NhËn [10000 phót]/prize_mianju_dan_qinglin_huoren",
	"MÆt n¹ (1 lÇn) - Thanh Niªn H¶i §­êng [10000 phót]/prize_mianju_dan_qinglin_haitang",
	"MÆt n¹ (1 lÇn) - Thanh Niªn Kim Phong [10000 phót]/prize_mianju_dan_qinglin_jinfeng",
	"Quay l¹i/prize_mianju_dan",
	"T¹m thêi kh«ng ®æi ®­îc!/no")
end

function prize_mianju_dan_shaonian()
	Describe(DescLink_YiGuan..": B¾t ®Çu ®æi ®©y.",7,
	"MÆt n¹ (1 lÇn) - ThiÕu Niªn L«i KiÕm [10000 phót]/prize_mianju_dan_shaolin_leijian",
	"MÆt n¹ (1 lÇn) - ThiÕu Niªn TiÕu S­¬ng [10000 phót]/prize_mianju_dan_shaolin_xiaoshuang",
	"MÆt n¹ (1 lÇn) - ThiÕu Niªn Háa NhËn [10000 phót]/prize_mianju_dan_shaolin_huoren",
	"MÆt n¹ (1 lÇn) - ThiÕu Niªn H¶i §­êng [10000 phót]/prize_mianju_dan_shaolin_haitang",
	"MÆt n¹ (1 lÇn) - ThiÕu Niªn Kim Phong [10000 phót]/prize_mianju_dan_shaolin_jinfeng",
	"Quay l¹i/prize_mianju_dan",
	"T¹m thêi kh«ng ®æi ®­îc!/no")
end

function prize_mianju_dan_shaolin_leijian()
	if ( nt_getTask(1205) >= 10000 ) then
		AddItem(0,11,97,0,0,0,0)
		local Uworld1205 = nt_getTask(1205) - 10000
		nt_setTask(1205,Uworld1205)
		Msg2Player("B¹n nhËn ®­îc 1 MÆt n¹ ®ång hµnh")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end 
end

function prize_mianju_dan_shaolin_xiaoshuang()
	if ( nt_getTask(1205) >= 10000 ) then
		AddItem(0,11,98,0,0,0,0)
		local Uworld1205 = nt_getTask(1205) - 10000
		nt_setTask(1205,Uworld1205)
		Msg2Player("B¹n nhËn ®­îc 1 MÆt n¹ ®ång hµnh")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end 
end

function prize_mianju_dan_shaolin_huoren()
	if ( nt_getTask(1205) >= 10000 ) then
		AddItem(0,11,99,0,0,0,0)
		local Uworld1205 = nt_getTask(1205) - 10000
		nt_setTask(1205,Uworld1205)
		Msg2Player("B¹n nhËn ®­îc 1 MÆt n¹ ®ång hµnh")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end 
end

function prize_mianju_dan_shaolin_haitang()
	if ( nt_getTask(1205) >= 10000 ) then
		AddItem(0,11,100,0,0,0,0)
		local Uworld1205 = nt_getTask(1205) - 10000
		nt_setTask(1205,Uworld1205)
		Msg2Player("B¹n nhËn ®­îc 1 MÆt n¹ ®ång hµnh")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end 
end

function prize_mianju_dan_shaolin_jinfeng()
	if ( nt_getTask(1205) >= 10000 ) then
		AddItem(0,11,101,0,0,0,0)
		local Uworld1205 = nt_getTask(1205) - 10000
		nt_setTask(1205,Uworld1205)
		Msg2Player("B¹n nhËn ®­îc 1 MÆt n¹ ®ång hµnh")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end 
end

function prize_mianju_dan_qinglin_leijian()
	if ( nt_getTask(1205) >= 10000 ) then
		AddItem(0,11,107,0,0,0,0)
		local Uworld1205 = nt_getTask(1205) - 10000
		nt_setTask(1205,Uworld1205)
		Msg2Player("B¹n nhËn ®­îc 1 MÆt n¹ ®ång hµnh")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end 
end

function prize_mianju_dan_qinglin_xiaoshuang()
	if ( nt_getTask(1205) >= 10000 ) then
		AddItem(0,11,108,0,0,0,0)
		local Uworld1205 = nt_getTask(1205) - 10000
		nt_setTask(1205,Uworld1205)
		Msg2Player("B¹n nhËn ®­îc 1 MÆt n¹ ®ång hµnh")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end 
end

function prize_mianju_dan_qinglin_huoren()
	if ( nt_getTask(1205) >= 10000 ) then
		AddItem(0,11,109,0,0,0,0)
		local Uworld1205 = nt_getTask(1205) - 10000
		nt_setTask(1205,Uworld1205)
		Msg2Player("B¹n nhËn ®­îc 1 MÆt n¹ ®ång hµnh")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end 
end

function prize_mianju_dan_qinglin_haitang()
	if ( nt_getTask(1205) >= 10000 ) then
		AddItem(0,11,110,0,0,0,0)
		local Uworld1205 = nt_getTask(1205) - 10000
		nt_setTask(1205,Uworld1205)
		Msg2Player("B¹n nhËn ®­îc 1 MÆt n¹ ®ång hµnh")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end 
end

function prize_mianju_dan_qinglin_jinfeng()
	if ( nt_getTask(1205) >= 10000 ) then
		AddItem(0,11,111,0,0,0,0)
		local Uworld1205 = nt_getTask(1205) - 10000
		nt_setTask(1205,Uworld1205)
		Msg2Player("B¹n nhËn ®­îc 1 MÆt n¹ ®ång hµnh")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end 
end


function messenger_prize_yaopai()
	Describe(DescLink_YiGuan..": Yªu bµi Tİn sø t­¬ng øng víi danh hiÖu Tİn sø b¹n ®¹t ®­îc. Vİ dô b¹n lµ Tİn sø - Vµng th× cã thÓ ®æi bÊt cø Tİn sø kh¸c, nh­ng nÕu b¹n lµ Tİn sø - Méc b¹n kh«ng thÓ ®æi Tİn sø - Vµng. §· hiÓu râ råi chø? B¾t ®Çu ®æi ®i.",7,
	"Tİn sø lÖnh – méc [50 ®iÓm]/prize_yaopai_mu",
	"Tİn sø lÖnh – §ång [150 ®iÓm]/prize_yaopai_tong",
	"Tİn sø lÖnh – B¹c [450 ®iÓm]/prize_yaopai_yin",
	"§æi Tİn sø Kim bµi [800 ®iÓm]/prize_yaopai_jin",
	"Ngù Tø lÖnh bµi - vµng [1500 ®iÓm]/prize_yaopai_yuci",
	"Quay l¹i/messenger_duihuanprize",
	"T¹m thêi kh«ng ®æi ®­îc!/no")
end

function prize_yaopai_mu()
	if ( nt_getTask(1205) >= 50 ) then
		AddItem(6,1,884,1,0,0,0)
		local Uworld1205 = nt_getTask(1205) - 50
		nt_setTask(1205,Uworld1205)
		Msg2Player("B¹n nhËn ®­îc tÊm Tİn sø lÖnh – méc.")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end 
end

function prize_yaopai_tong()
	if ( nt_getTask(1205) >= 150 ) then
		AddItem(6,1,885,2,0,0,0)
		local Uworld1205 = nt_getTask(1205) - 150
		nt_setTask(1205,Uworld1205)
		Msg2Player("B¹n nhËn ®­îc tÊm Tİn sø lÖnh – §ång.")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end 
end

function prize_yaopai_yin()
	if ( nt_getTask(1205) >= 450 ) then
		AddItem(6,1,886,3,0,0,0)
		local Uworld1205 = nt_getTask(1205) - 450
		nt_setTask(1205,Uworld1205)
		Msg2Player("B¹n nhËn ®­îc Tİn sø lÖnh – B¹c.")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end
end

function prize_yaopai_jin()
	if ( nt_getTask(1205) >= 800 ) then
		AddItem(6,1,887,4,0,0,0)
		local Uworld1205 = nt_getTask(1205) - 800
		nt_setTask(1205,Uworld1205)
		Msg2Player("B¹n nhËn ®­îc 1 tÊm Tİn sø lÖnh - Vµng.")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end 
end

function prize_yaopai_yuci()
	if ( nt_getTask(1205) >= 1500 ) then
		AddItem(6,1,888,5,0,0,0)
		local Uworld1205 = nt_getTask(1205) - 1500
		nt_setTask(1205,Uworld1205)
		Msg2Player("B¹n nhËn ®­îc 1 tÊm Ngù Tø Tİn Sø yªu bµi")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end 
end

function messenger_prize_huangjin()
	Describe(DescLink_YiGuan..": TriÒu ®×nh muèn th­ëng cho b¹n nªn ®· chuÈn bŞ 1 bé Trang bŞ Hoµng Kim khi giÕt ®Şch sÏ ®­îc nh©n ®«i kinh nghiÖm. B¹n muèn ®æi c¸i nµo?",6,
		"Hång ¶nh trÇm viªn v·n tóy [100000 ®iÓm]/prize_huangjin_shenyuan",
		"Hång ¶nh qua kiÕm t©y ph­¬ng [70000 ®iÓm]/prize_huangjin_guajian",
		"Hång ¶nh môc tóc t­¬ng vong [70000 ®iÓm]/prize_huangjin_muxu",
		"Hång ¶nh hång tô chiªu [50000 ®iÓm]/prize_huangjin_hongxiu",
		"Quay l¹i/messenger_duihuanprize",
		"T¹m thêi kh«ng ®æi/no")
end

function messenger_prize_other()
	Describe(DescLink_YiGuan..": TriÒu ®×nh cã nhiÒu phÇn th­ëng, hiÖn b¹n vËt phÈm b¹n cã thÓ ®æi:",3,
		"ThÇn bİ §¹i Hång Bao [2000 phót]/prize_other_hongbao",
		"Quay l¹i/messenger_duihuanprize",
		"T¹m thêi kh«ng ®æi/no")
end

function prize_other_hongbao()
	if ( nt_getTask(1205) >= 2000 ) then
		local Uworld1205 = nt_getTask(1205) - 2000 
		nt_setTask(1205,Uworld1205)
		AddItem(6,1,401,0,0,0,0)
		Msg2Player("B¹n nhËn ®­îc 1 ThÇn bİ §¹i Hång Bao.")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end 
end

function prize_huangjin_shenyuan()
	if ( nt_getTask(1205) >= 100000 ) then
		AddGoldItem(0,204)
		local Uworld1205 = nt_getTask(1205) - 100000
		nt_setTask(1205,Uworld1205)
		Msg2Player("B¹n nhËn ®­îc 1 bé trang bŞ Hoµng Kim Hång ¶nh.")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end 
end

function prize_huangjin_guajian()
	if ( nt_getTask(1205) >= 70000 ) then
		AddGoldItem(0,206)	-- [TIN SU 21/08] Linux 205 Kiem Bai = du an GoldItem 206
		local Uworld1205 = nt_getTask(1205) - 70000
		nt_setTask(1205,Uworld1205)
		Msg2Player("B¹n nhËn ®­îc 1 bé trang bŞ Hoµng Kim Hång ¶nh.")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end 
end

function prize_huangjin_muxu()
	if ( nt_getTask(1205) >= 70000 ) then
		AddGoldItem(0,205)	-- [TIN SU 21/08] Linux 206 Muc Tuc = du an GoldItem 205
		local Uworld1205 = nt_getTask(1205) - 70000
		nt_setTask(1205,Uworld1205)
		Msg2Player("B¹n nhËn ®­îc 1 bé trang bŞ Hoµng Kim Hång ¶nh.")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end 
end

function prize_huangjin_hongxiu()
	if ( nt_getTask(1205) >= 50000 ) then
		AddGoldItem(0,207)
		local Uworld1205 = nt_getTask(1205) - 50000
		nt_setTask(1205,Uworld1205)
		Msg2Player("B¹n nhËn ®­îc 1 bé trang bŞ Hoµng Kim Hång ¶nh.")
	else
		Describe(DescLink_YiGuan..": Xin lçi! §iÓm tİch lòy cña b¹n kh«ng ®ñ.",1,"KÕt thóc! /no")
	end 
end
-------------------------------------------È¡ÏûĞÅÊ¹ÈÎÎñ----------------------------------------------------
function messenger_losemytask()
	if ( GetTask(1204) == 0 ) then
		Describe(DescLink_YiGuan..": B¹n ch­a tiÕp nhËn nhiÖm vô Tİn sø, kh«ng thÓ xãa bá!",1," KÕt thóc ®èi tho¹i!/no")
	else
		Describe(DescLink_YiGuan..": B¹n x¸c ®Şnh muèn xãa bá?",2,"§óng vËy!/messenger_lostall","§Ó ta suy nghÜ l¹i!/no")
	end
end
--------------------------------------------½âÊÍÊ²Ã´ÊÇĞÅÊ¹ÈÎÎñ----------------------------------------------
function messenger_what()
	Describe(DescLink_YiGuan..": NhiÖm vô Tİn Sø: ®­a th­ gióp TriÒu ®×nh, cÇn ®¹t <color=red> cÊp "..HD_CFG("TS_CAP_TOITHIEU", 90).." trë lªn <color>. V­ît ¶i <color=red>Thiªn B¶o Khè <color>: giÕt boss gi÷ r­¬ng råi më 5 B¶o R­¬ng ®óng thø tù lµ qua ¶i. Hoµn thµnh nhËn ®iÓm tİch lòy ®æi th­ëng; mçi ngµy 2 l­ît, thªm 1 l­ît b»ng Thiªn Khè B¶o LÖnh. Cè lªn chiÕn h÷u!",4,
	"Giíi thiÖu ®¼ng cÊp ®Şa ®å/messenger_levelmap",
	--"·çÖ®Æï¹Ø¿¨½éÉÜ/messenger_flyhorse",
	--"É½ÉñÃí¹Ø¿¨½éÉÜ/messenger_mountaindeity",
	"Giíi thiÖu Thiªn B¶o khè /messenger_storehouse",
	--"ĞÅÊ¹ÑüÅÆ½éÉÜ/messenger_orderlevel",
	--"ÈÎÎñÏŞÊ±½éÉÜ/messenger_limittotask",
	" KÕt thóc ®èi tho¹i!/no")
end

function messenger_levelmap()
	Describe(DescLink_YiGuan..": Tr­íc m¾t chØ cã <color=red> cÊp "..HD_CFG("TS_CAP_TOITHIEU", 90).." trë lªn <color> mét møc ®é nhiÖm vô",2,"Quay l¹i/messenger_what"," KÕt thóc ®èi tho¹i!/no")
	Describe(DescLink_YiGuan..": Tr­íc m¾t chØ cã <color=red> cÊp 120 trë lªn<color> mét møc ®é nhiÖm vô",2,"Quay l¹i/messenger_what"," KÕt thóc ®èi tho¹i!/no")
end

function messenger_flyhorse()
	Describe(DescLink_YiGuan..": Trong ¶i Phong Kú, b¹n cã thÓ chän ®Õn cæng ra, nh­ thÕ coi nh­ hoµn thµnh nhiÖm vô nh­ng chØ nhËn ®­îc ®iÓm danh väng. Trong ¶i, chóng t«i cã lËp nh÷ng môc tiªu nhiÖm vô ®¬n gi¶n: Däc ®­êng b¹n sÏ thÊy nhiÒu kı hiÖu cña qu©n ta ®Ó l¹i, cïng lóc cã nhiÒu gian tÕ Kim Quèc. B¹n chØ cÇn tiÕp xóc 5 täa ®å trong b¶n ®å cã thÓ ®Õn cæng ra t×m B¹ch Dùc T­íng qu©n qua ¶i. Nh­ng biÓu hiÖn cña b¹n ®Òu n»m trong tÇm m¾t cña trinh s¸t ta ph¸i ®Õn. §èi víi viÖc t¨ng ®¼ng cÊp Yªu bµi cña b¹n sÏ rÊt cã lîi.",2,"Quay l¹i/messenger_what"," KÕt thóc ®èi tho¹i!/no")
end

function messenger_mountaindeity()
	Describe(DescLink_YiGuan..": Trong ¶i S¬n ThÇn miÕu, b¹n cã thÓ chän trùc tiÕp ®Õn cæng, nh­ thÕ xem nh­ hoµn thµnh nhiÖm vô, nh­ng chØ th­ëng ®iÓm danh väng. Trong ¶i, chóng t«i cã lËp nh÷ng môc tiªu nhiÖm vô ®¬n gi¶n: Trong nói xuÊt hiÖn ph¶n ®å cña Kinh S­ ThÇn ®ao doanh, bän hä ë trong rõng c¶n trë th­ tİn. Thñ lÜnh cña bän chóng x­ng lµ §ao Yªu. B¹n chØ cÇn ®¸nh b¹i 2 tªn §ao Yªu th× cã thÓ ®Õn cæng t×m N¹p Lang Thanh Thµnh qua ¶i. NÕu hoµn thµnh nhiÖm vô ®èi víi viÖc t¨ng ®¼ng cÊp Yªu bµi cña b¹n rÊt cã lîi.",2,"Quay l¹i/messenger_what"," KÕt thóc ®èi tho¹i!/no")
end

function messenger_storehouse()
	Describe(DescLink_YiGuan..": Khi nhËn nhiÖm vô, hÖ thèng ph¸t ngÉu nhiªn 5 ch÷ sè - më <color=red>B¶o R­¬ng <color>theo ®óng thø tù ®ã lµ qua ¶i. Muèn më r­¬ng ph¶i giÕt <color=red>B¶o Khè Thñ Hé Gi¶ <color>®øng c¹nh. <color=red>Ngò Hµnh Phï <color>(b¸n ë Kú Tr©n C¸c) më ra TriÖt Kim/Méc/Thñy/Háa/Thæ Phï, dïng ®Ó ®¸nh boss dÔ h¬n h¼n.",2,"Quay l¹i/messenger_what"," KÕt thóc ®èi tho¹i!/no")
end

function messenger_orderlevel()
	Describe(DescLink_YiGuan..": C¨n cø biÓu hiÖn cña b¹n trong ¶i, chóng t«i sÏ phong cho b¹n vµi Quan hµm, b¹n sÏ nhËn ®­îc nhiÒu phÇn th­ëng. Yªu b¸i hiÖn t¹i chia thµnh Tİn sø - Méc, Tİn sø - §ång, Tİn sø - B¹c, Tİn sø - Vµng vµ Ngù Tø Tİn sø -  Vµng. §ång thêi, b¹n nhÊn <color=red>F12<color>, xem trong b¶ng <color=red>®iÓm tİch lòy<color> cña nhiÖm vô Tİn sø, biÕt ®­îc vŞ trİ chøc quan cña m×nh. B¹n cã thÓ dïng ®iÓm tİch lòy ®æi danh hiÖu Tİn sø. Sau ®ã cã thÓ dïng ®iÓm tİch lòy Tİn sø ®æi Yªu bµi. Nh÷ng Yªu bµi nµy sÏ t¨ng kh¶ n¨ng <color=red>phôc håi<color> cña b¹n, nã cã t¸c dông trong <color=red>1 giê <color>, sè lÇn sö dông lµ <color=red>5 lÇn<color>.",2,"Quay l¹i/messenger_what"," KÕt thóc ®èi tho¹i!/no")
end

function messenger_limittotask()
	Describe(DescLink_YiGuan..": Sau khi tiÕp nhËn nhiÖm vô Tİn sø, b¹n cã thêi gian <color=red>2 giê <color> ®Ó hoµn thµnh. Thêi gian lµm nhiÖm vô mçi ngµy lµ <color=red>2 giê <color>. Chó ı! Khi ë trong ¶i, b¹n bŞ chÕt hoÆc rít m¹ng xem nh­ <color=red>nhiÖm vô thÊt b¹i<color>!",2,"Quay l¹i/messenger_what"," KÕt thóc ®èi tho¹i!/no")
end

function messenger_giveplayerexp(playerexp)

	local j =random(1,3)

	if ( j == 1 ) then
		tl_addPlayerExp(playerexp-500000)
	elseif ( j == 2 ) then
		tl_addPlayerExp(playerexp)
	elseif ( j == 3 ) then
		tl_addPlayerExp(playerexp+500000)
	end
	Msg2Player("Do b¹n lµm nhiÖm vô Tİn sø cã biÓu hiÖn tèt, DŞch tr¹m cho b¹n 1 phÇn th­ëng kinh nghiÖm!")
	nt_setTask(1224,1)  --ÉèÖÃµ±Ìì½±Àø¿ª¹ØÎª¿ª  
	nt_setTask(1223,0)
end

function messenger_lostall()
	nt_setTask(1201,0)
	nt_setTask(1202,0)
	nt_setTask(1203,0)
	nt_setTask(1204,0)
	nt_setTask(1207,0)
	nt_setTask(1212,0)
	nt_setTask(1213,0)
	nt_setTask(1214,0)
	nt_setTask(1215,0)
	Msg2Player("Ng­¬i ®· hñy bá thµnh c«ng nhiÖm vô Tİn Sø.")
	WriteLog(format("Account:%s[Name:%s] hñy bá nhiÖm vô Tİn Sø",
			GetAccount(),
			GetName()
			)
		);
end

function no()
end

-- [FIX 24/08] don gia tri RAC cua task 1205: bug cu dung chung 1205 cho ca
-- diem tich luy VA bo dem ngay (yymmdd*256) nen diem bi ghi de thanh ~66 trieu.
-- Moc doi thuong cao nhat chi 1500 => >= 1.000.000 chac chan la rac.
function messenger_clean_point_rac()
	local nPoint = nt_getTask(1205)
	if (nPoint >= 1000000) then
		nt_setTask(1205, 0)
	end
end

function check_daily_task_count()
	messenger_clean_point_rac()	-- [FIX 24/08]
	local nNormalTaskLimit = HD_CFG("TS_LUOT_THUONG_NGAY", 2)--Ò»Ìì×î¶àÖ»ÔÊĞíÁ½´ÎÆÕÍ¨ÈÎÎñ
	local nIBTaskLimit = HD_CFG("TS_LUOT_ITEM_NGAY", 1)--Ò»Ìì×î¶àÖ»ÔÊĞí1´ÎIBÈÎÎñ
	local nTodayCnt = %get_task_daily(4128)
	if nTodayCnt < nNormalTaskLimit then
		return 1--»¹ÓĞÆÕÍ¨ÈÎÎñ»ú»á
	elseif  nTodayCnt < (nNormalTaskLimit + nIBTaskLimit) then
		local nCountIb = CalcItemCount(-1, 6, 1, 3431, -1)
		if nCountIb >= 1 then
			return 2--¿ÉÒÔÊ¹ÓÃµÀ¾ß±¨Ãû
		else
			return -1--¿ÉÒÔÊ¹ÓÃµÀ¾ßµ«ÊÇÉíÉÏÃ»ÓĞ
		end
	end
	return 0--Ã»ÓĞÈÎÎñ»ú»áÁË
end