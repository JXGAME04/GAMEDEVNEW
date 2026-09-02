Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
--»ªÉ½ÅÉ°ïÖúNPC
Include("\\script\\global\\map_helper.lua")
-- [HOASON 01/09] bo (JX1 khong co, xem hs_shim.lua): Include("\\script\\task\\newtask\\education\\knowmagic.lua")

npc_name = "Tiªu Gi¸c"

function main()
	local UTask_hs = GetTask(3481)
	local szSex = nil
	if GetSex() == 0 then
		szSex = "S­ ®Ö"
	else
		szSex = "S­ muéi"
	end

	if UTask_hs == 0 then
		Say("Tiªu Gi¸c: §¹i hiÖp ®Õn tÖ ph¸i, ch¼ng hay cã chuyÖn g×?", 2, "T×m hiÓu quı ph¸i/talk_1", "T×m hiÓu c¸c vŞ cao thñ cña quı ph¸i./talk_2")
	elseif UTask_hs > 0 and UTask_hs ~= 70*256 then
		Say(format("Tiªu Gi¸c: Hãa ra lµ %s, cÇn gióp ®ì g×? ", szSex), 5, "T×m hiÓu bæn ph¸i/talk_1", "T×m hiÓu ®ång m«n/talk_2", "T×m hiÓu nhiÖm vô /talk_3", "T×m hiÓu së tr­êng luyÖn c«ng/map_help", "T×m hiÓu vâ nghÖ bæn m«n/skill_help")
	elseif UTask_hs == 70*256 then
		Say(format("Tiªu Gi¸c: Th× ra lµ %s ®· h¹ s¬n tõ l©u, khã kh¨n l¾m míi vÒ mét chuyÕn, h·y cïng ta dïng b÷a, uèng ly trµ, råi kÓ chuyÖn d­íi nói cho ta nghe.", szSex), 5, "T×m hiÓu ph¸i Hoa S¬n./talk_1", "T×m hiÓu l¹i ®ång m«n tr­íc kia/talk_2", "Cßn nhiÖm vô nµo ®Ó lµm n÷a/talk_3", "T×m hiÓu së tr­êng luyÖn c«ng/map_help", "T×m hiÓu vâ nghÖ bæn m«n/skill_help")
	end
end

function talk_1()
	Describe("Tiªu Gi¸c: Muèn t×m hiÓu c¸i g×?", 5, "Khëi nguyªn m«n ph¸i/talk_1_1", "VŞ trİ ®Şa lı /talk_1_2", "§Şa vŞ giang hå /talk_1_3", "§Æc s¾c m«n ph¸i/talk_1_4", "Kh«ng hái n÷a!/exit")
end

function talk_1_1()
	Describe("Tiªu Gi¸c: LŞch sö ph¸i Hoa S¬n ®· rÊt l©u ®êi, t­¬ng truyÒn b¾t ®Çu tõ D­¬ng B¸ KiÒu thêi Xu©n Thu ®· cã ng­êi lªn ®Ønh Hoa S¬n dùng lÒu khæ häc. Gi÷a thêi tiÒn triÒu n¨m Hµm Th«ng, cã mét th­ sinh tªn lµ Tiªu Tö L¨ng, lµ kÕ nghiÖp cña ®¹i t­íng qu©n Bïi M©n, khi ®Õn ngao du Hoa S¬n ®· nhËn ®­îc sù trî gióp cña ®Ö tö D­¬ng ThŞ, vÒ sau c¸c cao nh©n trong thiªn h¹ Èn c­ t¹i Hoa S¬n, khiÕn cho vâ c«ng cã dŞp tô héi, t¹o nªn ph¸i Hoa S¬n vang danh thiªn h¹ ngµy nay víi vâ c«ng KiÕm Khİ kú diÖu.", 4, "VŞ trİ ®Şa lı /talk_1_2", "§Şa vŞ giang hå /talk_1_3", "§Æc s¾c m«n ph¸i/talk_1_4", "Kh«ng hái n÷a!/exit")
end

function talk_1_2()
	Describe("Tiªu Gi¸c: Hoa S¬n nçi tiÕng lµ 'Tø nh¹c' trong 'ngò nh¹c', phİa B¾c gi¸p víi b×nh nguyªn VŞ Hµ b»ng ph¼ng vµ s«ng Hoµng Hµ, phİa Nam gi¸p TÇn LÜnh, trÊn gi÷ cöa ¶i T©y B¾c tiÕn vµo trung nguyªn. Hoa S¬n kh«ng chØ hïng vÜ hiÓm trë, mµ ®Şa thÕ nói non nguy nga ®å sé, tõ l©u ®· ®­îc mÖnh danh lµ 'Hoa S¬n Thiªn H¹ hiÓm', 'Kú hiÓm thiªn h¹ ®Ö nhÊt s¬n'.", 4, "Khëi nguyªn m«n ph¸i/talk_1_1", "§Şa vŞ giang hå /talk_1_3", "§Æc s¾c m«n ph¸i/talk_1_4", "Kh«ng hái n÷a!/exit")
end

function talk_1_3()
	Describe("Tiªu Gi¸c: Ph¸i Hoa S¬n lµ ®¹i ph¸i vâ l©m næi tiÕng trung nguyªn, ®Ö tö th­êng ë Hoa S¬n ®äc s¸ch häc ch÷ ch¬i cê luyÖn kiÕm, rÊt hiÕm lo toan chuyÖn vâ l©m thÕ tôc. Do nhiÒu n¨m tr­íc ch­ëng m«n Phông HÊp Nh­ t¹o ph¶n råi ®Õn Thiªn NhÉn Gi¸o, ph¸i Hoa S¬n bŞ ®¶ kİch m¹nh bÌn mai danh Èn tİch, kh«ng muèn mµng ®Õn viÖc trong vâ l©m.ThÕ nh­ng gÇn ®©y cã tin ®ån m¶nh b¶n ®å S¬n Hµ X· T¾c xuÊt hiÖn l¹i ë Hoa S¬n, c¸c ph¸i giang hå rôc rŞch manh ®éng, muèn ®Õn Hoa S¬n kiÕm t×m, khi Êy giang hå dËy sãng, nçi lªn mét trËn phong ba quyÕt liÖt. C¸c ch­ëng m«n ®êi tr­íc cña Hoa S¬n bÊt ®¾c dÜ ph¶i ph¸ tö quan ®Ó tho¸t th©n, l·nh ®¹o ®Ö tö Hoa S¬n ®èi phã víi cuéc phong v©n thiªn h¹ nµy.", 4, "Khëi nguyªn m«n ph¸i/talk_1_1", "VŞ trİ ®Şa lı /talk_1_2", "§Æc s¾c m«n ph¸i/talk_1_4", "Kh«ng hái n÷a!/exit")
end

function talk_1_4()
	Describe("Tiªu Gi¸c: §Ö tö ph¸i Hoa S¬n chia thµnh tu th©n h÷u vi vµ tu t©m v« vi, kiÕm t«ng nghiªn cøu chiªu thøc tinh x¶o kú diÖu, v¹n ph¸p tinh th«ng; khİ t«ng nghiªn cøu cÇu ch©n t¹i t©m, nhÊt ph¸p th«ng v¹n ph¸p. §Ö tö Hoa S¬n rÊt hiÕm khi xuèng nói, chØ khi nµo tu vi vâ c«ng ®¹t ®Õn c¶nh giíi nhÊt ®Şnh míi xuèng nói ®Ó rÌn luyÖn tr¶i nghiÖm, ®ét ph¸ ch­íng ng¹i, mong cã thÓ khiÕn tu vi cña b¶n th©n ®¹t ®Õn tÇm cao míi, lµ mét ph­¬ng ph¸p ®Ó t×m thÊy chİnh m×nh.", 4, "Khëi nguyªn m«n ph¸i/talk_1_1", "VŞ trİ ®Şa lı /talk_1_2", "§Şa vŞ giang hå /talk_1_3", "Kh«ng hái n÷a!/exit")
end

function talk_2()
	Describe("Tiªu Gi¸c: Ng­¬i muèn t×m ai?", 13, " BiÕt råi!/exit", "Ch­ëng m«n Nam Cung TuyÖt/talk_2_1", "Khİ T«ng S­ Phô Tõ Mé/talk_2_2", "KiÕm T«ng S­ Phô LËn H¹o Thiªn/talk_2_3", "V¹n T­ ViÔn/talk_2_4", "T« Phãng/talk_2_5", "LËn V©n Mi/talk_2_6", "ThÈm MÆc/talk_2_7", "¢n KiÕm Thu/talk_2_8", "Tiªu Ngäc/talk_2_9", "M¹c Phong Nhø/talk_2_10", "Phô Dung/talk_2_11", "TiÓu Viªn Tö/talk_2_12")
end

function talk_2_1()
	Describe("VŞ trİ: Chİnh Khİ §­êng Täa ®é: 177,191  C«ng dông: nhiÖm vô<enter> s­ thóc cña Phông HÊp Nh­, nguyªn ch­ëng m«n ph¸i Hoa S¬n, còng lµ ch­ëng m«n ®êi tr­íc cña ph¸i Hoa S¬n, trong cuéc ®¹i häa m«n ph¸i do Phông HÊp Nh­ lµm ph¶n, do bÕ quan nªn kh«ng bŞ ¶nh h­ëng nhiÒu. Vµi n¨m tr­íc, Nam Cung TuyÖt ®ét nhiªn xuÊt quan, g¸nh v¸c ®¹i cuéc, l·nh ®¹o m«n ®Ö cßn l¹i cña Hoa S¬n cïng nhau vùc dËy danh tiÕng Hoa S¬n Ph¸i. Sau khi xuÊt quan, nghe nãi vâ c«ng cña Nam Cung TuyÖt ®· ®¹t ®Õn møc kh«ng thÓ ngê tíi.", 12, " BiÕt råi!/exit", "Khİ T«ng S­ Phô Tõ Mé/talk_2_2", "KiÕm T«ng S­ Phô LËn H¹o Thiªn/talk_2_3", "V¹n T­ ViÔn/talk_2_4", "T« Phãng/talk_2_5", "LËn V©n Mi/talk_2_6", "ThÈm MÆc/talk_2_7", "¢n KiÕm Thu/talk_2_8", "Tiªu Ngäc/talk_2_9", "M¹c Phong Nhø/talk_2_10", "Phô Dung/talk_2_11", "TiÓu Viªn Tö/talk_2_12")
end

function talk_2_2()
	Describe("VŞ trİ: Ngäc N÷ §iÖn  Täa ®é: 188,180  C«ng dông: nhiÖm vô <enter> S­ Phô Khİ T«ng ph¸i Hoa S¬n, KiÕm ph¸p th©m hËu, tinh th«ng lôc nghÖ, cã phong th¸i nho sÜ, «n hßa lÔ ®é, th­êng d¹y cho ®Ö tö trong ph¸i tu luyÖn kiÕm thuËt, nghiªn cøu häc thuËt triÕt lı, ®­îc chóng ®Ö tö v« cïng t«n kİnh.", 12, " BiÕt råi!/exit", "Ch­ëng m«n Nam Cung TuyÖt/talk_2_1", "KiÕm T«ng S­ Phô LËn H¹o Thiªn/talk_2_3", "V¹n T­ ViÔn/talk_2_4", "T« Phãng/talk_2_5", "LËn V©n Mi/talk_2_6", "ThÈm MÆc/talk_2_7", "¢n KiÕm Thu/talk_2_8", "Tiªu Ngäc/talk_2_9", "M¹c Phong Nhø/talk_2_10", "Phô Dung/talk_2_11", "TiÓu Viªn Tö/talk_2_12")
end

function talk_2_3()
	Describe("VŞ trİ: Liªn Hoa §iÖn  Täa ®é: 170,191  C«ng dông: nhiÖm vô <enter> S­ Phô KiÕm T«ng ph¸i Hoa S¬n, tİnh t×nh nghiªm nghŞ c­¬ng trùc, İt khi c­êi nãi, «m hËn kÎ ph¶n gi¸o Phông HÊp Nh­, ngµy ®ªm t×m c¸ch b¸o thï Thiªn NhÉn Gi¸o.", 12, " BiÕt råi!/exit", "Ch­ëng m«n Nam Cung TuyÖt/talk_2_1", "Khİ T«ng S­ Phô Tõ Mé/talk_2_2", "V¹n T­ ViÔn/talk_2_4", "T« Phãng/talk_2_5", "LËn V©n Mi/talk_2_6", "ThÈm MÆc/talk_2_7", "¢n KiÕm Thu/talk_2_8", "Tiªu Ngäc/talk_2_9", "M¹c Phong Nhø/talk_2_10", "Phô Dung/talk_2_11", "TiÓu Viªn Tö/talk_2_12")
end

function talk_2_4()
	Describe("VŞ trİ: DËt Mİnh Hiªn  Täa ®é: 182,185  C«ng dông: nhiÖm vô <enter> §å ®Ö cña Tõ Mé, lín lªn ë Hoa S¬n, c­êng tr¸ng linh ho¹t, tİnh t×nh còng «n hßa lÔ ®é gièng s­ phô, rÊt quı mÕn S­ ®ÖT« Phãng.", 12, " BiÕt råi!/exit", "Ch­ëng m«n Nam Cung TuyÖt/talk_2_1", "Khİ T«ng S­ Phô Tõ Mé/talk_2_2", "KiÕm T«ng S­ Phô LËn H¹o Thiªn/talk_2_3", "T« Phãng/talk_2_5", "LËn V©n Mi/talk_2_6", "ThÈm MÆc/talk_2_7", "¢n KiÕm Thu/talk_2_8", "Tiªu Ngäc/talk_2_9", "M¹c Phong Nhø/talk_2_10", "Phô Dung/talk_2_11", "TiÓu Viªn Tö/talk_2_12")
end

function talk_2_5()
	Describe("VŞ trİ: T­ Qu¸ Nhai  Täa ®é: 195,197  C«ng dông: nhiÖm vô <enter> S­ ®Ö cña V¹n T­ ViÔn, tİnh t×nh ngang ng­îc ng¹o m¹n, b¹n th©n cña h¾n ®· chÕt trong cuéc ph¶n gi¸o cña Phông HÊp Nh­, nªn rÊt o¸n hËn Phông HÊp Nh­ vµ Thiªn NhÉn Gi¸o, nªn th­êng xuyªn theo dâi t×nh h×nh biÕn ®éng cña Thiªn NhÉn Gi¸o, hy väng sím ngµy b¸o thï Thiªn NhÉn Gi¸o.", 12, " BiÕt råi!/exit", "Ch­ëng m«n Nam Cung TuyÖt/talk_2_1", "Khİ T«ng S­ Phô Tõ Mé/talk_2_2", "KiÕm T«ng S­ Phô LËn H¹o Thiªn/talk_2_3", "V¹n T­ ViÔn/talk_2_4", "LËn V©n Mi/talk_2_6", "ThÈm MÆc/talk_2_7", "¢n KiÕm Thu/talk_2_8", "Tiªu Ngäc/talk_2_9", "M¹c Phong Nhø/talk_2_10", "Phô Dung/talk_2_11", "TiÓu Viªn Tö/talk_2_12")
end

function talk_2_6()
	Describe("VŞ trİ: L¹c Nh¹n C¸c  Täa ®é: 163,189  C«ng dông: nhiÖm vô <enter> con g¸i cña LËn H¹o Thiªn, th«ng minh l­¬ng thiÖn, cïng lín lªn víi V¹n T­ ViÔn vµ T« Phãng, nªn quan hÖ víi 2 ng­êi nµy rÊt th©n thiÕt.", 12, " BiÕt råi!/exit", "Ch­ëng m«n Nam Cung TuyÖt/talk_2_1", "Khİ T«ng S­ Phô Tõ Mé/talk_2_2", "KiÕm T«ng S­ Phô LËn H¹o Thiªn/talk_2_3", "V¹n T­ ViÔn/talk_2_4", "T« Phãng/talk_2_5", "ThÈm MÆc/talk_2_7", "¢n KiÕm Thu/talk_2_8", "Tiªu Ngäc/talk_2_9", "M¹c Phong Nhø/talk_2_10", "Phô Dung/talk_2_11", "TiÓu Viªn Tö/talk_2_12")
end

function talk_2_7()
	Describe("VŞ trİ: §iÓm MÆc Hiªn Täa ®é: 179,185  C«ng dông: nhiÖm vô <enter> ®å ®Ö cña LËn H¹o Thiªn, tõ nhá ®· rÊt th«ng minh, hiÓu biÕt s©u réng. Vµo n¨m Phông HÊp Nh­ ph¶n gi¸o khi Êy tuæi cßn nhá, ®· bŞ kİch ®éng lín nªn tİnh t×nh nhót nh¸t, rÊt sî xuèng nói.", 12, " BiÕt råi!/exit", "Ch­ëng m«n Nam Cung TuyÖt/talk_2_1", "Khİ T«ng S­ Phô Tõ Mé/talk_2_2", "KiÕm T«ng S­ Phô LËn H¹o Thiªn/talk_2_3", "V¹n T­ ViÔn/talk_2_4", "T« Phãng/talk_2_5", "LËn V©n Mi/talk_2_6", "¢n KiÕm Thu/talk_2_8", "Tiªu Ngäc/talk_2_9", "M¹c Phong Nhø/talk_2_10", "Phô Dung/talk_2_11", "TiÓu Viªn Tö/talk_2_12")
end

function talk_2_8()
	Describe("VŞ trİ: §iÓm MÆc Hiªn  Täa ®é: 179,185  C«ng dông: nhiÖm vô<enter> s­ phô ®­c kiÕm ph¸i Hoa S¬n, kü n¨ng ®óc kiÕm cùc giái.", 12, " BiÕt råi!/exit", "Ch­ëng m«n Nam Cung TuyÖt/talk_2_1", "Khİ T«ng S­ Phô Tõ Mé/talk_2_2", "KiÕm T«ng S­ Phô LËn H¹o Thiªn/talk_2_3", "V¹n T­ ViÔn/talk_2_4", "T« Phãng/talk_2_5", "LËn V©n Mi/talk_2_6", "ThÈm MÆc/talk_2_7", "Tiªu Ngäc/talk_2_9", "M¹c Phong Nhø/talk_2_10", "Phô Dung/talk_2_11", "TiÓu Viªn Tö/talk_2_12")
end

function talk_2_9()
	Describe("Täa ®é: 186,191  C«ng dông: nhiÖm vô<enter> tİnh t×nh tho¶i m¸i, thİch uèng r­îu.", 12, " BiÕt råi!/exit", "Ch­ëng m«n Nam Cung TuyÖt/talk_2_1", "Khİ T«ng S­ Phô Tõ Mé/talk_2_2", "KiÕm T«ng S­ Phô LËn H¹o Thiªn/talk_2_3", "V¹n T­ ViÔn/talk_2_4", "T« Phãng/talk_2_5", "LËn V©n Mi/talk_2_6", "ThÈm MÆc/talk_2_7", "¢n KiÕm Thu/talk_2_8", "M¹c Phong Nhø/talk_2_10", "Phô Dung/talk_2_11", "TiÓu Viªn Tö/talk_2_12")
end

function talk_2_10()
	Describe("Täa ®é: 160,190  C«ng dông: Giao dŞch trang bŞ.", 12, " BiÕt råi!/exit", "Ch­ëng m«n Nam Cung TuyÖt/talk_2_1", "Khİ T«ng S­ Phô Tõ Mé/talk_2_2", "KiÕm T«ng S­ Phô LËn H¹o Thiªn/talk_2_3", "V¹n T­ ViÔn/talk_2_4", "T« Phãng/talk_2_5", "LËn V©n Mi/talk_2_6", "ThÈm MÆc/talk_2_7", "¢n KiÕm Thu/talk_2_8", "Tiªu Ngäc/talk_2_9", "Phô Dung/talk_2_11", "TiÓu Viªn Tö/talk_2_12")
end

function talk_2_11()
	Describe("Täa ®é: 179,198  C«ng dông: giao dŞch d­îc phÈm.", 12, " BiÕt råi!/exit", "Ch­ëng m«n Nam Cung TuyÖt/talk_2_1", "Khİ T«ng S­ Phô Tõ Mé/talk_2_2", "KiÕm T«ng S­ Phô LËn H¹o Thiªn/talk_2_3", "V¹n T­ ViÔn/talk_2_4", "T« Phãng/talk_2_5", "LËn V©n Mi/talk_2_6", "ThÈm MÆc/talk_2_7", "¢n KiÕm Thu/talk_2_8", "Tiªu Ngäc/talk_2_9", "M¹c Phong Nhø/talk_2_10", "TiÓu Viªn Tö/talk_2_12")
end

function talk_2_12()
	Describe("Täa ®é: 166,184  C«ng dông: giao dŞch binh khİ.", 12, " BiÕt råi!/exit", "Ch­ëng m«n Nam Cung TuyÖt/talk_2_1", "Khİ T«ng S­ Phô Tõ Mé/talk_2_2", "KiÕm T«ng S­ Phô LËn H¹o Thiªn/talk_2_3", "V¹n T­ ViÔn/talk_2_4", "T« Phãng/talk_2_5", "LËn V©n Mi/talk_2_6", "ThÈm MÆc/talk_2_7", "¢n KiÕm Thu/talk_2_8", "Tiªu Ngäc/talk_2_9", "M¹c Phong Nhø/talk_2_10", "Phô Dung/talk_2_11")
end

function talk_3()
local UTask_hs = GetTask(3481)

if (UTask_hs == 10*256) then	
	Describe("<#>Tiªu Gi¸c: Muèn gia nhËp m«n ph¸i häc vâ c«ng, cã thÓ th«ng qua viÖc hoµn thµnh nhiÖm vô nhËp m«n ®Ó thùc hiÖn. Sau khi nhËp m«n, lÇn l­ît hoµn thµnh nhiÖm vô 5 m«n ph¸i lµ cã thÓ häc ®­îc vâ c«ng, nhËn ®­îc cÊp hµm, sau khi hoµn thµnh nhiÖm vô xuÊt s­ lµ cã thÓ xuÊt s­ dÏ dµng. §¸nh dÊu nhiÖm vô cña ng­¬i lµ:"..UTask_hs.."<#>, TiÕp theo nh÷ng viÖc ng­¬i cã thÓ lµm lµ:", 1, "NhiÖm vô lÊy n­íc pha trµ./v2")
end;

if (UTask_hs > 10*256) and (UTask_hs < 20*256) then	
	Describe("<#>Tiªu Gi¸c: Muèn gia nhËp m«n ph¸i häc vâ c«ng, cã thÓ th«ng qua viÖc hoµn thµnh nhiÖm vô nhËp m«n ®Ó thùc hiÖn. Sau khi nhËp m«n, lÇn l­ît hoµn thµnh nhiÖm vô 5 m«n ph¸i lµ cã thÓ häc ®­îc vâ c«ng, nhËn ®­îc cÊp hµm, sau khi hoµn thµnh nhiÖm vô xuÊt s­ lµ cã thÓ xuÊt s­ dÏ dµng. §¸nh dÊu nhiÖm vô cña ng­¬i lµ:"..UTask_hs.."<#>, NhiÖm vô ®ang tiÕn hµnh lµ:", 1, "NhiÖm vô lÊy n­íc pha trµ./v2")
end;

if (UTask_hs == 20*256) then	
	Describe("<#>Tiªu Gi¸c: Muèn gia nhËp m«n ph¸i häc vâ c«ng, cã thÓ th«ng qua viÖc hoµn thµnh nhiÖm vô nhËp m«n ®Ó thùc hiÖn. Sau khi nhËp m«n, lÇn l­ît hoµn thµnh nhiÖm vô 5 m«n ph¸i lµ cã thÓ häc ®­îc vâ c«ng, nhËn ®­îc cÊp hµm, sau khi hoµn thµnh nhiÖm vô xuÊt s­ lµ cã thÓ xuÊt s­ dÏ dµng. §¸nh dÊu nhiÖm vô cña ng­¬i lµ:"..UTask_hs.."<#>, TiÕp theo nh÷ng viÖc ng­¬i cã thÓ lµm lµ:", 1, "NhiÖm vô Ngé KiÕm./v3")
end;

if (UTask_hs > 20*256) and (UTask_hs < 30*256) then	
	Describe("<#>Tiªu Gi¸c: Muèn gia nhËp m«n ph¸i häc vâ c«ng, cã thÓ th«ng qua viÖc hoµn thµnh nhiÖm vô nhËp m«n ®Ó thùc hiÖn. Sau khi nhËp m«n, lÇn l­ît hoµn thµnh nhiÖm vô 5 m«n ph¸i lµ cã thÓ häc ®­îc vâ c«ng, nhËn ®­îc cÊp hµm, sau khi hoµn thµnh nhiÖm vô xuÊt s­ lµ cã thÓ xuÊt s­ dÏ dµng. §¸nh dÊu nhiÖm vô cña ng­¬i lµ:"..UTask_hs.."<#>, NhiÖm vô ®ang tiÕn hµnh lµ:", 1, "NhiÖm vô Ngé KiÕm./v3")
end;

if (UTask_hs == 30*256) then	
	Describe("<#>Tiªu Gi¸c: Muèn gia nhËp m«n ph¸i häc vâ c«ng, cã thÓ th«ng qua viÖc hoµn thµnh nhiÖm vô nhËp m«n ®Ó thùc hiÖn. Sau khi nhËp m«n, lÇn l­ît hoµn thµnh nhiÖm vô 5 m«n ph¸i lµ cã thÓ häc ®­îc vâ c«ng, nhËn ®­îc cÊp hµm, sau khi hoµn thµnh nhiÖm vô xuÊt s­ lµ cã thÓ xuÊt s­ dÏ dµng. §¸nh dÊu nhiÖm vô cña ng­¬i lµ:"..UTask_hs.."<#>, TiÕp theo nh÷ng viÖc ng­¬i cã thÓ lµm lµ:", 1, "NhiÖm vô B¨ng Tµm Ngäc Lé Cao./v4")
end;

if (UTask_hs > 30*256) and (UTask_hs < 40*256) then	
	Describe("<#>Tiªu Gi¸c: Muèn gia nhËp m«n ph¸i häc vâ c«ng, cã thÓ th«ng qua viÖc hoµn thµnh nhiÖm vô nhËp m«n ®Ó thùc hiÖn. Sau khi nhËp m«n, lÇn l­ît hoµn thµnh nhiÖm vô 5 m«n ph¸i lµ cã thÓ häc ®­îc vâ c«ng, nhËn ®­îc cÊp hµm, sau khi hoµn thµnh nhiÖm vô xuÊt s­ lµ cã thÓ xuÊt s­ dÏ dµng. §¸nh dÊu nhiÖm vô cña ng­¬i lµ:"..UTask_hs.."<#>, NhiÖm vô ®ang tiÕn hµnh lµ:", 1, "NhiÖm vô B¨ng Tµm Ngäc Lé Cao./v4")
end;

if (UTask_hs == 40*256) then	
	Describe("<#>Tiªu Gi¸c: Muèn gia nhËp m«n ph¸i häc vâ c«ng, cã thÓ th«ng qua viÖc hoµn thµnh nhiÖm vô nhËp m«n ®Ó thùc hiÖn. Sau khi nhËp m«n, lÇn l­ît hoµn thµnh nhiÖm vô 5 m«n ph¸i lµ cã thÓ häc ®­îc vâ c«ng, nhËn ®­îc cÊp hµm, sau khi hoµn thµnh nhiÖm vô xuÊt s­ lµ cã thÓ xuÊt s­ dÏ dµng. §¸nh dÊu nhiÖm vô cña ng­¬i lµ:"..UTask_hs.."<#>, TiÕp theo nh÷ng viÖc ng­¬i cã thÓ lµm lµ:", 1, "NhiÖm vô V©n Tö Tr¾c Thu B×nh./v5")
end;

if (UTask_hs > 40*256) and (UTask_hs < 50*256) then	
	Describe("<#>Tiªu Gi¸c: Muèn gia nhËp m«n ph¸i häc vâ c«ng, cã thÓ th«ng qua viÖc hoµn thµnh nhiÖm vô nhËp m«n ®Ó thùc hiÖn. Sau khi nhËp m«n, lÇn l­ît hoµn thµnh nhiÖm vô 5 m«n ph¸i lµ cã thÓ häc ®­îc vâ c«ng, nhËn ®­îc cÊp hµm, sau khi hoµn thµnh nhiÖm vô xuÊt s­ lµ cã thÓ xuÊt s­ dÏ dµng. §¸nh dÊu nhiÖm vô cña ng­¬i lµ:"..UTask_hs.."<#>, NhiÖm vô ®ang tiÕn hµnh lµ:", 1, "NhiÖm vô V©n Tö Tr¾c Thu B×nh./v5")
end;

if (UTask_hs == 50*256) then	
	Describe("<#>Tiªu Gi¸c: Muèn gia nhËp m«n ph¸i häc vâ c«ng, cã thÓ th«ng qua viÖc hoµn thµnh nhiÖm vô nhËp m«n ®Ó thùc hiÖn. Sau khi nhËp m«n, lÇn l­ît hoµn thµnh nhiÖm vô 5 m«n ph¸i lµ cã thÓ häc ®­îc vâ c«ng, nhËn ®­îc cÊp hµm, sau khi hoµn thµnh nhiÖm vô xuÊt s­ lµ cã thÓ xuÊt s­ dÏ dµng. §¸nh dÊu nhiÖm vô cña ng­¬i lµ:"..UTask_hs.."<#>, TiÕp theo nh÷ng viÖc ng­¬i cã thÓ lµm lµ:", 1, "NhiÖm vô VÜnh L¹c Phong V©n ./v6")
end;

if (UTask_hs > 50*256) and (UTask_hs < 60*256) then	
	Describe("<#>Tiªu Gi¸c: Muèn gia nhËp m«n ph¸i häc vâ c«ng, cã thÓ th«ng qua viÖc hoµn thµnh nhiÖm vô nhËp m«n ®Ó thùc hiÖn. Sau khi nhËp m«n, lÇn l­ît hoµn thµnh nhiÖm vô 5 m«n ph¸i lµ cã thÓ häc ®­îc vâ c«ng, nhËn ®­îc cÊp hµm, sau khi hoµn thµnh nhiÖm vô xuÊt s­ lµ cã thÓ xuÊt s­ dÏ dµng. §¸nh dÊu nhiÖm vô cña ng­¬i lµ:"..UTask_hs.."<#>, NhiÖm vô ®ang tiÕn hµnh lµ:", 1, "NhiÖm vô VÜnh L¹c Phong V©n ./v6")
end;

if (UTask_hs == 60*256) then	
	Describe("<#>Tiªu Gi¸c: Muèn gia nhËp m«n ph¸i häc vâ c«ng, cã thÓ th«ng qua viÖc hoµn thµnh nhiÖm vô nhËp m«n ®Ó thùc hiÖn. Sau khi nhËp m«n, lÇn l­ît hoµn thµnh nhiÖm vô 5 m«n ph¸i lµ cã thÓ häc ®­îc vâ c«ng, nhËn ®­îc cÊp hµm, sau khi hoµn thµnh nhiÖm vô xuÊt s­ lµ cã thÓ xuÊt s­ dÏ dµng. §¸nh dÊu nhiÖm vô cña ng­¬i lµ:"..UTask_hs.."<#>, TiÕp theo nh÷ng viÖc ng­¬i cã thÓ lµm lµ:", 1, "NhiÖm vô xuÊt s­ /v7")
end;

if (UTask_hs > 60*256) and (UTask_hs < 70*256) then	
	Describe("<#>Tiªu Gi¸c: Muèn gia nhËp m«n ph¸i häc vâ c«ng, cã thÓ th«ng qua viÖc hoµn thµnh nhiÖm vô nhËp m«n ®Ó thùc hiÖn. Sau khi nhËp m«n, lÇn l­ît hoµn thµnh nhiÖm vô 5 m«n ph¸i lµ cã thÓ häc ®­îc vâ c«ng, nhËn ®­îc cÊp hµm, sau khi hoµn thµnh nhiÖm vô xuÊt s­ lµ cã thÓ xuÊt s­ dÏ dµng. §¸nh dÊu nhiÖm vô cña ng­¬i lµ:"..UTask_hs.."<#>, NhiÖm vô ®ang tiÕn hµnh lµ:", 1, "NhiÖm vô xuÊt s­ /v7")
end;

if (UTask_hs >= 70*256) and (UTask_hs < 80*256) then	
	Describe("<#>Tiªu Gi¸c: Muèn gia nhËp m«n ph¸i häc vâ c«ng, cã thÓ th«ng qua viÖc hoµn thµnh nhiÖm vô nhËp m«n ®Ó thùc hiÖn. Sau khi nhËp m«n, lÇn l­ît hoµn thµnh nhiÖm vô 5 m«n ph¸i lµ cã thÓ häc ®­îc vâ c«ng, nhËn ®­îc cÊp hµm, sau khi hoµn thµnh nhiÖm vô xuÊt s­ lµ cã thÓ xuÊt s­ dÏ dµng. §¸nh dÊu nhiÖm vô cña ng­¬i lµ:"..UTask_hs.."<#>, NhiÖm vô ®ang tiÕn hµnh lµ:", 1, "Trïng ph¶n s­ m«n/v8")
end;
end

function v1()
end;


function v2()
	Talk(4, "", "Th«ng qua hoµn thµnh nhiÖm vô nµy, ng­¬i sÏ t¨ng cÊp thµnh ®Ö tö phông trµ, häc ®­îc vâ c«ng KiÕm T«ng Tæng QuyÕt, Long NhiÔu Th©n <enter> tiÕp nhËn nhiÖm vô nµy ph¶i lµ ®Ö tö bæn m«n cÊp 10 trë lªn.","B­íc 1: t¹i chç V¹n T­ ViÔn (182,185) nhËn nhiÖm vô, NhËn ®­îc Gµo n­íc. ", "B­íc 2: t¹i chç khe suèi ngo¹i cæng Nam ph¸i Hoa S¬n sö dông Gµo n­íc thu thËp, NhËn ®­îc Thanh LiÖt Kho¸ng Thñy. ", "B­íc 3: Giao n­íc Thanh LiÖt Kho¸ng Thñy cho V¹n T­ ViÔn. ")
end;


function v3()
	Talk(6, "", "Th«ng qua hoµn thµnh nhiÖm vô nµy, ng­¬i sÏ t¨ng cÊp thµnh T­ MÆc §Ö tö, häc ®­îc vâ c«ng D­ìng Ng« KiÕm Ph¸p, H¶i N¹p B¸ch Xuyªn. <enter> TiÕp nhËn nhiÖm vô nµy ph¶i lµ ®Ö tö bæn m«n cÊp 20 trë lªn ,®ång thêi ®· hoµn thµnh nhiÖm vô lÊy n­íc pha trµ.","B­íc 1: t¹i chç Tõ Mé(188,180)nhËn nhiÖm vô, NhËn ®­îc Thïng N­íc. ", "B­íc 2: t¹i chç th¸c n­íc phİa T©y B¾c ph¸i Hoa S¬n sö dông Thïng N­íc nhËn ®­îc Dung TuyÕt Chi Thñy, ®Õn T­ Qu¸ Nhai phİa ®«ng ph¸i Hoa S¬n thu thËp Nhai Bİch Chi Th¹ch, Th­¬ng Tïng Ch©m DiÖp. ", "B­íc 3: t¹i chç qu·ng tr­êng trung t©m ph¸i Hoa S¬n t×m ®­îc §Ö tö luyÖn kiÕm Hoa S¬n(173,192). ", "B­íc 4: §Õn LuyÖn Vâ §­êng ph¸i Hoa S¬n tû thİ víi §Ö tö luyÖn kiÕm Hoa S¬n?184,186?vµ giµnh chiÕn th¾ng. ", "B­íc 5: Giao Dung TuyÕt Chi Thñy, Nhai Bİch Chi Th¹ch,Th­¬ng Tïng Ch©m DiÖp cho Tõ Mé. ")
end;


function v4()
	Talk(7, "", "Th«ng qua hoµn thµnh nhiÖm vô nµy, ng­¬i sÏ t¨ng cÊp thµnh §Ö tö chÊp kiÕm, häc ®­îc vâ c«ng Kim Nh¹n Hoµnh Kh«ng, Long HuyÒn KiÕm Khİ <enter> TiÕp nhËn nhiÖm vô nµy ph¶i lµ ®Ö tö bæn m«n cÊp 30 trë lªn, vµ ®· hoµn thµnh nhiÖm vô Ngé KiÕm. ","B­íc 1: §Õn chç LËn V©n Mi(163,189)nhËn nhiÖm vô. ", "B­íc 2: §Õn Ph­îng T­êng t×m ¤ng chñ d­îc ®iÕm mua B¨ng Tµm Ngäc Lé Cao. ", "B­íc 3: §Õn KiÕm C¸c T©y B¾c tiªu diÖt c­êng ®¹o, NhËn ®­îc B¨ng Tµm TuÕ. ", "B­íc 4: Trë vÒ Ph­îng T­êng, giao B¨ng Tµm TuÕ cho ¤ng chñ d­îc ®iÕm. ", "B­íc 5: §èi tho¹i víi ¤ng chñ d­îc ®iÕm. NhËn ®­îc B¨ng Tµm Ngäc Lé Cao. ", "B­íc 6: Giao B¨ng Tµm Ngäc Lé Cao cho LËn V©n Mi. ")
end;


function v5()
	Talk(5, "", "Th«ng qua hoµn thµnh nhiÖm vô nµy, ng­¬i sÏ t¨ng cÊp thµnh §an Thanh Sø, häc ®­îc vâ c«ng Hi Di KiÕm Ph¸p, Khİ ChÊn S¬n Hµ. <enter>TiÕp nhËn nhiÖm vô nµy ph¶i lµ ®Ö tö bæn m«n cÊp 40 trë lªn, vµ ®· hoµn thµnh B¨ng Tµm Ngäc Lé Cao. ","B­íc 1: §Õn chç ThÈm MÆc?179,185? nhËn nhiÖm vô. ", "B­íc 2: §Õn Ph­îng T­êng t×m ¤ng chñ t¹p hãa t×m hiÓu th«ng tin vÒ dông cô ®¸nh cê.", "B­íc 3: t×m ®­îc L­u Viªn Ngo¹i ë Ph­îng T­êng, hái han vÒ dông cô ®¸nh cê, tr¶ lêi ®óng 3 c©u hái sÏ nhËn ®­îc V©n Tö Tr¾c Thu B×nh. ", "B­íc 4: giao V©n Tö Tr¾c Thu B×nh cho ThÈm MÆc. ")
end;


function v6()
	Talk(6, "", "Th«ng qua hoµn thµnh nhiÖm vô nµy, ng­¬i sÏ t¨ng cÊp thµnh DŞch Thu Sø, häc ®­îc vâ c«ngThiªn Kh«n §¶o HuyÒn, Khİ Qu¸n Tr­êng Hång. <enter>TiÕp nhËn nhiÖm vô nµy ph¶i lµ ®Ö tö bæn m«n cÊp 50 trë lªn, vµ ®· hoµn thµnh V©n Tö Tr¾c Thu B×nh nhiÖm vô. ","B­íc 1: t¹i chç LËn H¹o Thiªn(170,191) nhËn nhiÖm vô. ", "B­íc 2: §Õn VÜnh L¹c TrÊn ®èi tho¹i víi Hoµng Thóc, §æng §¹i Thóc, Th­ N·i N·i, t×m hiÓu sù t×nh.", "B­íc 3: §Õn Kim Quang §éng tiªu diÖt S¬n PhØ, NhËn ®­îc 3 s¶n vËt cña th«n d©n. ", "B­íc 4: Quay l¹i VÜnh L¹c TrÊn, giao s¶n vËt th«n d©n cho Hoµng thóc, §æng §¹i Thóc, Th­ N·i N·i. ", "B­íc 5: Trë vÒ ph¸i Hoa S¬n phôc mÖnh LËn H¹o Thiªn.")
end;


function v7()
	Talk(11, "", "Th«ng qua hoµn thµnh nhiÖm vô nµy, ng­¬i sÏ t¨ng cÊp thµnh Lôc NghÖ TruyÒn Nh©n, xuÊt s­ thuËn lîi. <enter>TiÕp nhËn nhiÖm vô nµy ph¶i lµ ®Ö tö bæn m«n cÊp 50 trë lªn, vµ ®· hoµn thµnh nhiÖm vô VÜnh L¹c Phong V©n. ","B­íc 1: t¹i chç Ch­ëng m«n Nam Cung TuyÖt(177,191) nhËn nhiÖm vô, NhËn ®­îc cÈm nang. ", "B­íc 2: kiÓm tra cÈm nang, NhËn ®­îc lêi nh¾n #thø 1.", "B­íc 3: §Õn chç ¢n KiÕm Thu(171,182), nhËn nhiÖm vô lÊy B¹ch Hæ Chi HuyÕt.", "B­íc 4: §Õn TuyÕt Lang §éng tiªu diÖt §iÕu T×nh B¹ch Hæ, NhËn ®­îc B¹ch Hæ HuyÕt. ", "B­íc 5: giao B¹ch Hæ HuyÕt cho ¢n KiÕm Thu. ", "B­íc 6: kiÓm tra cÈm nang, NhËn ®­îc lêi nh¾n #thø 2. ", "B­íc 7: §Õn thung lòng ph¸i Hoa S¬n tÜnh t©m. ", "B­íc 8: kiÓm tra cÈm nang, NhËn ®­îc lêi nh¾n #thø 3. ", "B­íc 9: §Õn KiÕm C¸c T©y Nam tiªu diÖt ph¶n ®å Khóc V« H×nh ph¸i Hoa S¬n.", "B­íc 10: Trë vÒ ph¸i Hoa S¬n phôc mÖnh Ch­ëng m«n Nam Cung TuyÖt.")
end;

function v8()
	Talk(3,"","Trë l¹i Hoa S¬n, §¹i hiÖp t¨ng cÊp thµnh Th¸i Häc C¸t Chñ, häc ®­îc vâ c«ng HuyÒn Nh¹n V©n Yªn, Th­¬ng Tïng Nghªn Kh¸ch, Ma V©n KiÕm Khİ. ","B­íc 1: Sau khi ®¹i hiÖp t¨ng ®Õn cÊp 60, §Õn chç Ch­ëng m«n Nam Cung TuyÖt(177,191), xin trë vÒ ph¸i Hoa S¬n. ","B­íc 2: N¹p 50 l­îng cã thÓ trë vÒ ph¸i Hoa S¬n. ")
end;

function exit()
end
