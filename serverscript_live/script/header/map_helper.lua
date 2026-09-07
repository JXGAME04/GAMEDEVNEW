-- ================================================================================================
-- [HE THONG] script/header/map_helper.lua
-- Muc dich  : TRO GIUP MAP (48 ham): id <-> ten, toa do, kiem tra map thanh/thi.
-- Duoc nap  : Include tu 2 tep (vd vebinh.lua, factionhead.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Ham (dong): map_help (52), skill_help (69), Uworld1000_knowmagic (100), Uworld1000_emei (115), Uworld1000_tangmen (123), Uworld1000_tianwang (132), Uworld1000_tianren (141), Uworld1000_wudang (149), Uworld1000_cuiyan (157), Uworld1000_shaolin (165), Uworld1000_gaibang (174), Uworld1000_kunlun (182), Uworld1000_wudu (191), Uworld1000_emeijianzong (200), Uworld1000_emeiqizong (204), Uworld1000_emeifuzhu (208), Uworld1000_tangmenfeidao (212), Uworld1000_tangmennujian (216), Uworld1000_tangmenfeibiao (220), Uworld1000_tangmenxianjing (224), Uworld1000_daotianwang (228), Uworld1000_qiangtianwang (232), Uworld1000_chuitianwang (236), Uworld1000_fuzhutianwang (240) ... (48 ham)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
lv_str={	"ch­a ®Õn cÊp 10 ","h¬n cÊp 10","h¬n cÊp 20","h¬n cÊp 30","h¬n cÊp 40","h¬n cÊp 50","h¬n cÊp 60","h¬n cÊp 70","h¬n cÊp 80","h¬n cÊp 90"}
map0 = {	"C¸c T©n thñ th«n ",
			"GÇn bªn Bæn m«n, c¸c thµnh thŞ næi danh ",
			"KiÕm C¸c Trung Nguyªn, KiÕm C¸c T©y Nam, KiÕm C¸c T©y B¾c, La Tiªu s¬n, Vò L¨ng s¬n, TÇn L¨ng ",
			"Thôc C­¬ng s¬n, Miªu LÜnh, YÕn tö ®éng, vµ D­îc V­¬ng ®éng tÇng 1, Kho¸i Ho¹t L©m ",
			"Kinh Hoµng ®éng, Phôc Ng­u s¬n T©y, §iÓm Th­¬ng ®éng tÇng 3, ThiÕu L©m mËt thÊt ",
			"H­ëng Thñy ®éng, Thiªn T©m ®éng, vµ §éng §×nh Hå s¬n ®éng, B¨ng HuyÖt Mª Cung ",
			"Thanh Loa ®¶o, 108 La H¸n TrËn, vµ Hoµnh s¬n ph¸i, T­¬ng D­¬ng vÖ m«n mËt ®¹o ",
			"§¹i Tï ®éng, Thanh Loa ®¶o s¬n ®éng, vµ Linh Cèc ®éng, L·o Hæ ®éng, L©m Du Quan ",
			"Sa m¹c §Şa BiÓu, Phi Thiªn ®éng, Phï Dung ®éng, L­ìng Thñy ®éng, B¨ng Hµ ®éng, Thanh Khª ®éng, Tr­êng B¹ch s¬n C­íc ",
			"Sa m¹c s¬n ®éng tÇng 1, 2 vµ TiÕn Cóc ®éng, Kho¶ Lang ®éng, Tr­êng B¹ch s¬n Nam Léc, Tr­êng B¹ch s¬n B¾c Léc "}
map1 = {	"C¸c T©n thñ th«n ",
			"GÇn bªn Bæn m«n, c¸c thµnh thŞ næi danh ",
			"KiÕm C¸c Trung Nguyªn, KiÕm C¸c T©y Nam, KiÕm C¸c T©y B¾c, La Tiªu s¬n, Vò L¨ng s¬n, TÇn L¨ng ",
			"Vò Di s¬n, TÇn L¨ng tÇng 1, vµ B¹ch V©n ®éng, thæ phØ ®éng ",
			"Thanh Thµnh s¬n, §iÓm Th­¬ng s¬n, vµ Phong Nh·n ®éng, §iÓm Th­¬ng ®éng tÇng 1, ThÇn Tiªn ®éng ",
			"H­ëng Thñy ®éng, NghiÖt Long ®éng, B¶o Ngäc Hoa ®éng, vµ Thiªn TÇm Th¸p tÇng 1,2 ",
			"D­¬ng Gi¸c ®éng, Tróc T¬ ®éng tÇng 1, vµ Hoµnh s¬n ph¸i, Thiªn TÇm Th¸p tÇng 3 ",
			"Long Nh·n ®éng, TÇn L¨ng tÇng 2, vµ Tróc T¬ ®éng tÇng 2, Tr­êng Giang §Çu, L©m Du Quan ",
			"Sa m¹c §Şa BiÓu, Vò L¨ng ®éng, D­¬ng Trung ®éng, Cæ D­¬ng ®éng, Nh¹n Th¹ch ®éng, D­îc V­¬ng ®éng tÇng 3, TÇn L¨ng tÇng 3, Tr­êng B¹ch s¬n C­íc ",
			"Sa m¹c s¬n ®éng tÇng 1,2,3, vµ D­îc V­¬ng ®éng tÇng 4, TiÕn Cóc ®éng, Tr­êng B¹ch s¬n Nam Léc, Tr­êng B¹ch s¬n B¾c Léc "}
map2 = {	"C¸c T©n thñ th«n ",
			"GÇn bªn Bæn m«n, c¸c thµnh thŞ næi danh ",
			"KiÕm C¸c Trung Nguyªn, KiÕm C¸c T©y Nam, KiÕm C¸c T©y B¾c, La Tiªu s¬n, Vò L¨ng s¬n, TÇn L¨ng ",
			"B¹ch V©n ®éng, Thôc C­¬ng s¬n, thæ phØ ®éng, vµ Phôc Ng­u s¬n §«ng, Kim Quang ®éng ",
			"Phong Nh·n ®éng, §iÓm Th­¬ng ®éng tÇng 1, ThÇn Tiªn ®éng, vµ Phôc Ng­u s¬n T©y, Kinh Hoµng ®éng ",
			"H­ëng Thñy ®éng, Thiªn TÇm Th¸p tÇng 1,2, vµ Tİn T­íng tù ®Şa ®¹o, ¸c B¸ ®Şa ®¹o ",
			"Thiªn TÇm Th¸p tÇng 3, 108 La H¸n TrËn, vµ CÊm ®Şa mª cung, Hoµnh s¬n ph¸i ",
			"Tr­êng Giang Nguyªn §Çu, S¬n B¶o ®éng, vµ §µo Hoa Nguyªn, D­îc V­¬ng ®éng tÇng 2, L©m Du Quan ",
			"Sa m¹c §Şa BiÓu, Phi Thiªn ®éng, Phï Dung ®éng, D­¬ng Trung ®éng, Cæ D­¬ng ®éng, D­îc V­¬ng ®éng tÇng 3, Tr­êng B¹ch s¬n C­íc ",
			"Sa m¹c s¬n ®éng tÇng 1, 2, 3, vµ Kho¶ Lang ®éng, TiÕn Cóc ®éng, Tr­êng B¹ch s¬n Nam Léc, Tr­êng B¹ch s¬n B¾c Léc "}
map3 = {	"C¸c T©n thñ th«n ",
			"GÇn bªn Bæn m«n, c¸c thµnh thŞ næi danh ",
			"KiÕm C¸c Trung Nguyªn, KiÕm C¸c T©y Nam, KiÕm C¸c T©y B¾c, La Tiªu s¬n, Vò L¨ng s¬n, TÇn L¨ng ",
			"Phôc Ng­u s¬n §«ng, Kim Quang ®éng, vµ TÇn L¨ng tÇng 1, Vò Di s¬n ",
			"Táa V©n ®éng, Long Cung ®éng, vµ §iÓm Th­¬ng ®éng tÇng 2, Thanh Thµnh s¬n, §iÓm Th­¬ng s¬n ",
			"H­ëng Thñy ®éng, BiÖn Kinh ThiÕt Th¸p tÇng 1, 2, 3, vµ T­êng V©n ®éng tÇng 1, Ngù Hoa Viªn s¬n ®éng ",
			"T­êng V©n ®éng tÇng 2, 3, 4, vµ Thiªn NhÉn Th¸nh ®éng, D­¬ng Gi¸c ®éng, Tróc T¬ ®éng tÇng 1 ",
			"D­îc V­¬ng ®éng tÇng 2, L·o Hæ ®éng, T­êng V©n ®éng 5 tÇng, Linh Cèc ®éng, L©m Du Quan ",
			"Sa m¹c §Şa BiÓu, Vò L¨ng ®éng, V« danh ®éng, Nh¹n Th¹ch ®éng, TÇn L¨ng tÇng 3, Tr­êng B¹ch s¬n C­íc ",
			"Sa m¹c s¬n ®éng tÇng 1, 2, 3, vµ D­îc V­¬ng ®éng tÇng 4, C¸n Viªn ®éng, Tr­êng B¹ch s¬n Nam Léc, Tr­êng B¹ch s¬n B¾c Léc "}
map4 = {	"C¸c T©n thñ th«n ",
			"GÇn bªn Bæn m«n, c¸c thµnh thŞ næi danh ",
			"KiÕm C¸c Trung Nguyªn, KiÕm C¸c T©y Nam, KiÕm C¸c T©y B¾c, La Tiªu s¬n, Vò L¨ng s¬n, TÇn L¨ng ",
			"Miªu LÜnh, D­îc V­¬ng ®éng tÇng 1, Kho¸i Ho¹t L©m, vµ Phôc Ng­u s¬n §«ng, Kim Quang ®éng, TuyÕt B¸o ®éng tÇng 1, 2 ",
			"KiÕn Tİnh Phong s¬n ®éng, Kª Qu¸n ®éng, Hoµng Hµ Nguyªn §Çu, vµ Táa V©n ®éng, §iÓm Th­¬ng ®éng tÇng 2 ",
			"H­ëng Thñy ®éng, B¨ng HuyÖt Mª Cung, L­u Tiªn ®éng tÇng 1, vµ ThiÕt Th¸p tÇng 1,2,3 ",
			"Hoµnh s¬n ph¸i, T­¬ng D­¬ng VÖ m«n mËt ®¹o, vµ L­u Tiªn ®éng tÇng 2,3,4 ",
			"§µo Hoa Nguyªn, L­u Tiªn ®éng tÇng 5, 6 vµ TÇn L¨ng tÇng 2, Long Nh·n ®éng, L©m Du Quan ",
			"Sa m¹c §Şa BiÓu, L­ìng Thñy ®éng, V« danh ®éng, B¨ng Hµ ®éng, Thanh Khª ®éng, Tr­êng B¹ch s¬n C­íc ",
			"Sa m¹c s¬n ®éng tÇng 1, 2, 3, vµ D­îc V­¬ng ®éng tÇng 4, C¸n Viªn ®éng, Tr­êng B¹ch s¬n Nam Léc, Tr­êng B¹ch s¬n B¾c Léc "}

function map_help()
	ser = GetSeries()
	lvl = floor(GetLevel()/10 + 1)
	if (lvl > 10) then lvl = 10 end
	if (ser == 0) then
		Say("<npc>: §èi víi <color=Metal>hÖ Kim<color> <color=Red>"..lv_str[lvl].."<color> nh­ ng­¬i th× ®Şa ®iÓm luyÖn c«ng tèt nhÊt lµ "..map0[lvl].."",0)
	elseif (ser == 1) then
		Say("<npc>: §èi víi <color=Wood>hÖ Méc<color> <color=Red>"..lv_str[lvl].."<color> nh­ ng­¬i th× ®Şa ®iÓm luyÖn c«ng tèt nhÊt lµ "..map1[lvl].."",0)
	elseif (ser == 2) then
		Say("<npc>: §èi víi <color=Water>hÖ Thñy<color> <color=Red>"..lv_str[lvl].."<color> nh­ ng­¬i th× ®Şa ®iÓm luyÖn c«ng tèt nhÊt lµ "..map2[lvl].."",0)
	elseif (ser == 3) then
		Say("<npc>: §èi víi <color=Fire>hÖ Háa<color> <color=Red>"..lv_str[lvl].."<color> nh­ ng­¬i th× ®Şa ®iÓm luyÖn c«ng tèt nhÊt lµ "..map3[lvl].."",0)
	else
		Say("<npc>: §èi víi <color=Earth>hÖ Thæ<color> <color=Red>"..lv_str[lvl].."<color> nh­ ng­¬i th× ®Şa ®iÓm luyÖn c«ng tèt nhÊt lµ "..map4[lvl].."",0)
	end
end

function skill_help()
	fact_num = GetFactionNo();
	if (fact_num == 0) then
		Talk(5,"","Nh÷ng ng­êi theo <color=red>ThiÕu L©m ph¸i<color>, vâ nghÖ ph©n theo 3 h­íng:","H­íng thø nhÊt gäi lµ: 'QuyÒn ThiÕu L©m', lÊy Hµng Long BÊt Vò, ThiÕu L©m QuyÒn Ph¸p, La H¸n TrËn, Long Tr¶o Hæ Tr¶o, Nh­ Lai Thiªn DiÖp, §¹t Ma §é Gianglµm chñ.","H­íng thø hai gäi lµ: 'C«n ThiÕu L©m', lÊy Kim Cang Phôc Ma, ThiÕu L©m C«n ph¸p, La H¸n TrËn, Hoµnh T¶o Lôc Hîp, Nh­ Lai Thiªn DiÖp, Hoµnh T¶o Thiªn Qu©n lµm chñ.","H­íng thø ba gäi lµ: '§ao ThiÕu L©m', lÊy Kim Cang Phôc Ma, ThiÕu L©m §ao ph¸p, La H¸n TrËn, Ma Ha V« L­îng, Nh­ Lai Thiªn DiÖp, V« T­íng Tr¶m lµm chñ.","Ngoµi ra cßn cã BÊt §éng Minh V­¬ng, S­ Tö Hèng, DŞch C©n kinh c¸c lo¹i vâ c«ng nµy gióp İch cho ng­¬i rÊt nhiÒu ph­¬ng diÖn.")
	elseif (fact_num == 1) then
		Talk(5,"","Nh÷ng ng­êi theo <color=red>Thiªn V­¬ng bang<color>, vâ nghÖ ph©n theo 3 h­íng:","H­íng thø nhÊt gäi lµ: '§ao Thiªn V­¬ng', lÊy Kinh L«i Tr¶m, Thiªn V­¬ng  §ao ph¸p, B¸t Phong Tr¶m, V« T©m Tr¶m, Thiªn V­¬ng ChiÕn ı, Ph¸ Thiªn Tr¶m lµm chñ.","H­íng thø hai gäi lµ: 'Th­¬ng Thiªn V­¬ng', lÊy Håi Phong L¹c Nh¹n, Thiªn V­¬ng Th­¬ng ph¸p, D­¬ng Quan Tam §iÖp, HuyÕt ChiÕn B¸t Ph­¬ng, Thiªn V­¬ng ChiÕn ı, Truy Tinh Trôc NguyÖt lµm chñ.","H­íng thø ba gäi lµ: 'Chïy Thiªn V­¬ng', lÊy Tr¶m Long quyÕt, Thiªn V­¬ng Chïy Ph¸p, Hµng V©n QuyÕt, Thõa Long QuyÕt, Thiªn V­¬ng ChiÕn ı, Truy Phong QuyÕt lµm chñ.","Ngoµi ra cßn cã TÜnh T©m QuyÕt, §o¹n Hån Thİch, Kim Chung Tr¸o c¸c lo¹i vâ c«ng nµy gióp İch cho ng­¬i rÊt nhiÒu ph­¬ng diÖn.")
	elseif (fact_num == 2) then
		Talk(5,"","Nh÷ng ng­êi theo <color=red>§­êng M«n<color> vâ nghÖ ph©n theo 4 h­íng:","H­íng thø nhÊt gäi lµ: 'Phi §ao §­êng', lÊy Tİch LŞch §¬n, §­êng M«n ¸m khİ, Truy T©m TiÔn, TiÓu Lı Phi §ao, T©m Nh·n, NhiÕp Hån NguyÖt ¶nh lµm chñ.","H­íng thø hai gäi lµ: 'Tô TiÔn §­êng', lÊy Tİch LŞch §¬n, §­êng M«n ¸m khİ, M¹n Thiªn Hoa Vò, Thiªn La §Şa Vâng, T©m Nh·n, B¹o Vò Lª Hoa lµm chñ.","H­íng thø ba gäi lµ: 'Phi Tiªu §­êng', lÊy Tİch LŞch §¬n, §­êng M«n ¸m khİ, §o¹t Hån Tiªu, T¸n Hoa Tiªu, T©m Nh·n, Cöu Cung Phi Tinh lµm chñ.","H­íng thø t­ gäi lµ: 'H·m TÜnh §­êng', lÊy §Şa DiÖm Háa, §éc Thİch Cèt, Xuyªn T©m Thİch, Hµn B¨ng Thİch, L«i Kİch ThuËt, Lo¹n Hoµn Kİch lµm chñ.")
	elseif (fact_num == 3) then
		Talk(5,"","Nh÷ng ng­êi theo <color=red>Ngò §éc gi¸o<color>, vâ nghÖ ph©n theo 3 h­íng:","H­íng thø nhÊt gäi lµ: 'Ch­ëng §éc', lÊy §éc Sa Ch­ëng, Ngò §éc Ch­ëng Ph¸p, U Minh Kh« L©u, Thiªn C­¬ng §Şa S¸t, Ngò §éc Kú Kinh, ¢m Phong Thùc Cèt lµm chñ.","H­íng thø hai gäi lµ: '§ao ®éc', lÊy HuyÕt §ao §éc S¸t, Ngò §éc §ao ph¸p, B¸ch §éc Xuyªn T©m, Chu C¸p Thanh Minh, Ngò §éc Kú Kinh, HuyÒn ¢m Tr¶m lµm chñ.","H­íng thø ba gäi lµ: 'Chó thuËt ®éc', lÊy Cöu Thiªn Cuång L«i, Xİch DiÖm Thùc Thiªn, B¨ng Lam HuyÒn Tinh, Xuyªn Y Ph¸ Gi¸p, Xuyªn T©m §éc Thİch, §o¹n C©n Hñ Cèt lµm chñ.","Ngoµi ra cßn cã T¹p Nan D­îc Kinh, V« H×nh §éc, V¹n §éc Thùc T©m c¸c lo¹i vâ c«ng nµy gióp İch cho ng­¬i rÊt nhiÒu ph­¬ng diÖn.")
	elseif (fact_num == 4) then
		Talk(4,"","Nh÷ng ng­êi theo <color=red>Nga Mi ph¸i<color>, vâ nghÖ ph©n theo 3 h­íng:","H­íng thø nhÊt gäi lµ: 'KiÕm T«ng Nga Mi', lÊy NhÊt DiÖp Tri Thu, Nga Mi KiÕm ph¸p, Th«i Song Väng NguyÖt, BÊt DiÖt BÊt TuyÖt, PhËt Ph¸p V« Biªn, Tam Nga TÒ TuyÕt lµm chñ.","H­íng thø hai gäi lµ: 'Khİ T«ng Nga Mi', lÊy Phiªu TuyÕt Xuyªn V©n, Nga Mi Ch­ëng ph¸p, Tø T­îng §ång Quy, PhËt Quang Phæ ChiÕu, PhËt Ph¸p V« Biªn, Phong S­¬ng To¸i ¶nh lµm chñ.","H­íng thø ba hai gäi lµ: 'Toµn Phô Nga Mi', lÊy Tõ Hµng Phæ §é, Méng §iÖp, L­u Thñy, PhËt T©m Tõ H÷u, Thanh ¢m Ph¹n X­íng, Phæ §é Chóng Sinh lµm chñ.")
	elseif (fact_num == 5) then
		Talk(4,"","Nh÷ng ng­êi theo <color=red>Thóy Yªn m«n<color>, vâ nghÖ ph©n theo 2 h­íng:","H­íng thø nhÊt gäi lµ: '§¬n ®ao Thóy Yªn', lÊy Phong Hoa TuyÕt NguyÖt, Thóy Yªn §ao ph¸p, Vò §¶ Lª Hoa, Môc D· L­u Tinh, B¨ng Cèt TuyÕt T©m, B¨ng Tung V« ¶nh lµm chñ.","H­íng thø hai gäi lµ: 'Song ®aoThóy Yªn', lÊy Phong QuyÓn Tµn TuyÕt, Thóy Yªn Song ®ao, Phï V©n T¸n TuyÕt, Bİch H¶i TriÒu Sinh, B¨ng Cèt TuyÕt T©m, B¨ng T©m Tiªn Tö  lµm chñ.","Ngoµi ra cßn cã B¨ng T©m Tr¸i ¶nh, Hé ThÓ Hµn B¨ng, TuyÕt ¶nh c¸c lo¹i vâ c«ng nµy gióp İch cho ng­¬i rÊt nhiÒu ph­¬ng diÖn")
	elseif (fact_num == 6) then
		Talk(4,"","Nh÷ng ng­êi theo <color=red>C¸i Bang<color>, vâ nghÖ ph©n theo 2 h­íng:","H­íng thø nhÊt gäi lµ: 'Ch­ëng C¸i', lÊy KiÕn Nh©n ThÇn Thñ, C¸i Bang Ch­ëng Ph¸p, Gi¸ng Long Ch­ëng, Kh¸ng Long H÷u Hèi, Tóy §iÖp Cuång Vò, Phi Long T¹i Thiªn lµm chñ.","H­íng thø hai gäi lµ: 'C«n C¸i', lÊy Diªn M«n Th¸c B¸t, C¸i Bang Bæng ph¸p, §¶ CÈu Bæng, Bæng §¶ ¸c CÈu, Tóy §iÖp Cuång Vò, Thiªn H¹ V« CÈu lµm chñ.","Ngoµi ra cßn cã Hãa HiÓm Vi Di, Ho¹t BÊt L­u Thñ, Tiªu Diªu C«ng  c¸c lo¹i vâ c«ng nµy gióp İch cho ng­¬i rÊt nhiÒu ph­¬ng diÖn")
	elseif (fact_num == 7) then
		Talk(4,"","Nh÷ng ng­êi theo <color=red>Thiªn NhÉn gi¸o<color>, vâ nghÖ ph©n theo 3 h­íng:","H­íng thø nhÊt gäi lµ: 'ChiÕn NhÉn', lÊy Tµn D­¬ng Nh­ HuyÕt, Thiªn NhÉn M©u ph¸p, LiÖt Háa T×nh Thiªn, Th©u Thiªn Ho¸n NhËt, Thiªn Ma Gi¶i ThÓ,V©n Long Kİch lµm chñ.","H­íng thø hai gäi lµ: 'Ma NhÉn', lÊy §¬n ChØ LiÖt DiÖm, Thiªn NhÉn §ao ph¸p, Th«i S¬n §iÒn H¶i, Ma DiÖm ThÊt S¸t, Thiªn Ma Gi¶i ThÓ, Thiªn Ngo¹i L­u Tinh lµm chñ.","H­íng thø ba gäi lµ: 'Chó thuËt nhÉn', lÊy Háa Liªn PhÇn Hoa, ¶o ¶nh Phi Hå, Phi Hång V« Tİch, Bi T« Thanh Phong, LŞch Ma §o¹t Hån, NhiÕp Hån Lo¹n T©m lµm chñ.")
	elseif (fact_num == 8) then
		Talk(4,"","Nh÷ng ng­êi theo <color=red>Vâ §ang ph¸i<color>, vâ nghÖ ph©n theo 2 h­íng:","H­íng thø nhÊt gäi lµ: 'Khİ T«ng Vâ §ang', lÊy Né L«i ChØ, Vâ §ang QuyÒn Ph¸p, B¸c CÊp Nhi Phôc, V« Ng· V« KiÕm, Th¸i Cùc ThÇn C«ng, Thiªn §Şa V« Cùc lµm chñ.","H­íng thø hai gäi lµ: 'KiÕm T«ng Vâ §ang', lÊy Th­¬ng H¶i Minh NguyÖt, Vâ §ang KiÕm ph¸p, KiÕm Phi Kinh Thiªn, Tam Hoµn Thao NguyÖt, Th¸i Cùc ThÇn C«ng, Nh©n KiÕm Hîp NhÊt  lµm chñ.","Ngoµi ra cßn cã ThÊt Tinh TrËn, ThÕ V©n Tung, Täa Väng V« Ng· c¸c lo¹i vâ c«ng nµy gióp İch cho ng­¬i rÊt nhiÒu ph­¬ng diÖn")
	elseif (fact_num == 9) then
		Talk(5,"","Nh÷ng ng­êi theo <color=red>C«n L«n ph¸i<color>, vâ nghÖ ph©n theo 3 h­íng:","H­íng thø nhÊt gäi lµ: '§ao C«n L«n', lÊy H« Phong Ph¸p, C«n L«n §ao ph¸p, NhÊt Khİ Tam Thanh, Cuång Phong SËu §iÖn, S­¬ng Ng¹o C«n L«n, Ng¹o TuyÕt Tiªu Phong lµm chñ.","H­íng thø hai gäi lµ: 'KiÕm C«n L«n', lÊy Cuång L«i ChÊn §Şa, C«n L«n KiÕm ph¸p, Thiªn TÕ TÊn L«i, Ngò L«i chİnh Ph¸p, S­¬ng Ng¹o C«n L«n, L«i §éng Cöu Thiªn lµm chñ.","H­íng thø ba gäi lµ: 'Chó thuËt C«n L«n', lÊy Thóc Ph­îc Chó, Ki B¸n phï, B¾c Minh §¸o H¶i, Khi Hµn Ng¹o TuyÕt, Mª Tung ¶o ¶nh, Tóy Tiªn T¸ Cèt lµm chñ.","Ngoµi ra cßn cã Thanh Phong phï, Thiªn Thanh §Şa Träc, Khİ T©m Phï c¸c lo¹i vâ c«ng nµy gióp İch cho ng­¬i rÊt nhiÒu ph­¬ng diÖn")
	elseif (fact_num == 10) then
		Talk(3,"","Nh÷ng ng­êi theo <color=red>Hao S¬n ph¸i<color>, vâ nghÖ ph¸t triÓn theo 2 ®­êng kh¸c nhau:","§­êng thø nhÊt lµ 'Hoa S¬n KiÕm T«ng', cã nh÷ng kü n¨ng chİnh nh­: B¹ch Hång Qu¸n NhËt, KiÕm T«ng Tæng QuyÕt, D­ìng Ng« KiÕm Ph¸p, Kim Nh¹n Hoµnh Kh«ng, Hy Di KiÕm Ph¸p, Thiªn Th©n §¶o HuyÒn, Th­¬ng Tïng Nghªnh Kh¸ch, HuyÔn Nh·n V©n Yªn, §o¹t MÖnh Liªn","§­êng thø hai lµ 'Hoa S¬n Khİ T«ng', cã nh÷ng kü n¨ng chİnh nh­: Thanh Phong Tèng S¶ng, Long NhiÔu Quyªn, H¶i N¹p B¸ch Xuyªn, Long TuyÒn KiÕm Khİ, Khİ ChÊn S¬n Hµ, Khİ Qu¸n Tr­êng Hång, Ma V©n KiÕm Khİ, HuyÔn Nh·n V©n Yªn, Ph¸ Th¸ch Ph¸ Ngäc")
	else
		Say("<npc>: Ng­¬i thËm chİ ch­a hÒ tham gia m«n ph¸i nµo, nãi g× ®Õn chuyÖn luyÖn hay kh«ng luyÖn ph¸p ",0)
	end
end

DescLink_GeDaMenPaiSkills="LINK:\\spr\\ui3\\ĞÂÈÎÎñÏµÍ³\\ÈÎÎñÍ¼±ê\\menpai011202.spr"

function Uworld1000_knowmagic()
	Say(DescLink_GeDaMenPaiSkills,0,"Vâ häc kh«ng cã c¶nh giíi tèi cao. ThËt ra vâ c«ng c¸c m«n ph¸i quan träng nhÊt vÉn lµ siªng n¨ng luyÖn tËp.",11,
	"Nga My ph¸i/Uworld1000_emei",
	"§­êng M«n/Uworld1000_tangmen",
	"Thiªn V­¬ng Bang/Uworld1000_tianwang",
	"Thiªn NhÉn Gi¸o/Uworld1000_tianren",
	"Vâ §ang ph¸i/Uworld1000_wudang",
	"Thóy Yªn m«n/Uworld1000_cuiyan",
	"ThiÕu L©m ph¸i/Uworld1000_shaolin",
	"C¸i Bang/Uworld1000_gaibang",
	"C«n L«n ph¸i/Uworld1000_kunlun",
	"Ngò §éc Gi¸o/Uworld1000_wudu",
	"Sau sÏ hái tiÕp/no")
end

function Uworld1000_emei()
	Say(DescLink_GeDaMenPaiSkills,0,"Nga Mi lµ m«n ph¸i do n÷ nhi s¸ng lËp. 'Nga Mi' võa lµ tªn gäi cña vŞ s­ tæ s¸ng lËp m«n ph¸i võa lµ th¸nh ®Şa nói Nga Mi",4,
	"KiÕm T«ng/Uworld1000_emeijianzong",
	"Khİ T«ng/Uworld1000_emeiqizong",
	"Hç trî/Uworld1000_emeifuzhu",
	"Sau sÏ hái tiÕp/no")
end

function Uworld1000_tangmen()
	Say(DescLink_GeDaMenPaiSkills,0,"§­êng m«n hïng cø Xuyªn Thôc, ¸m khİ vµ háa khİ ®· lõng danh thiªn h¹. Ng­êi trong giang hå nghe ®Õn §­êng m«n th¶y ®Òu biÕn s¾c.",5,
	"Phi §ao/Uworld1000_tangmenfeidao",
	"Ná tiÔn/Uworld1000_tangmennujian",
	"Phi Tiªu/Uworld1000_tangmenfeibiao",
	"C¹m bÉy/Uworld1000_tangmenxianjing",
	"Sau sÏ hái tiÕp/no")
end

function Uworld1000_tianwang()
	Say(DescLink_GeDaMenPaiSkills,0,"Ngµy tr­íc Së V­¬ng Chung T­¬ng khëi binh t¹i §éng §×nh hå, kh¸ng Kim chèng Tèng! VÒ sau Së V­¬ng bŞ s¸t h¹i, nghÜa qu©n t«n D­¬ng Yªu lµm thñ lÜnh, x­ng lµ §¹i Th¸nh Thiªn V­¬ng! NghÜa qu©n ph¸t triÓn thÇn tèc. VÒ sau bŞ Nh¹c Phi tiªu diÖt. Sau nµy con g¸i D­¬ng yªu lµ D­¬ng Anh trë l¹i n¬i ®©y, chiªu tËp nghÜa qu©n, nèi tiÕp nghiÖp lín!",5,
	"§ao Thiªn V­¬ng/Uworld1000_daotianwang",
	"Th­¬ng Thiªn V­¬ng/Uworld1000_qiangtianwang",
	"Chïy Thiªn V­¬ng/Uworld1000_chuitianwang",
	"Hç trî/Uworld1000_fuzhutianwang",
	"Sau sÏ hái tiÕp/no")
end

function Uworld1000_tianren()
	Say(DescLink_GeDaMenPaiSkills,0,"Thiªn NhÉn Gi¸o lµ tæ chøc cña Kim Quèc nh»m ®èi phã víi Vâ L©m Nh©n sÜ cña Tèng Quèc, ®­îc phong lµ T¸t M·n Gi¸o Quèc gi¸o cña §¹i Kim, gi¸o chñ lµ Kim Quèc Quèc s­ Hoµn Nhan Hïng LiÖt, cao thñ trong gi¸o nhiÒu v« sè, cßn chiªu mé thªm ph¶n ®å Chİnh ph¸i vµ Tµ ph¸i cña Tèng Quèc",4,
	"ChiÕn NhÉn/Uworld1000_zhanren",
	"Ma NhÉn/Uworld1000_moren",
	"Chó NhÉn/Uworld1000_zuzhouren",
	"Sau sÏ hái tiÕp/no")
end

function Uworld1000_wudang()
	Say(DescLink_GeDaMenPaiSkills,0,"Vâ §ang s¬n cßn gäi lµ Th¸i Hßa s¬n, ®­îc mÖnh danh lµ:'Trung Quèc ®¹o gi¸o danh s¬n'. Vâ §ang ph¸i ®­îc phong lµ 'HuyÒn Thiªn TrÊn Vâ §¹i §Õ'. Vâ §ang ph¸i ®· cã lŞch sö l©u ®êi. S¸ng lËp m«n ph¸i lµ tæ s­ 'L¹p Th¸p ®¹o nh©n' Tr­¬ng Tam Phong",4,
	"KiÕm T«ng/Uworld1000_wudangjianzong",
	"Khİ T«ng/Uworld1000_wudangqizong",
	"Hç trî /Uworld1000_wudangfuzhu",
	"Sau sÏ hái tiÕp/no")
end

function Uworld1000_cuiyan()
	Say(DescLink_GeDaMenPaiSkills,0,"Lµ 1 m«n ph¸i thÇn bİ ë V©n Nam, t­¬ng truyÒn Tæ s­ lËp ph¸i lµ 1 vŞ n÷ hiÖp bŞ t×nh nh©n phô rÉy, bá ®i Èn c­ ®Ó quªn hÕt sù ®êi?®· ®Õn vïng nói biªn thïy V©n Nam xa x«i hÎo l¸nh,trªn ®­êng ®i Ng­êi gÆp nhiÒu c« nhi bŞ bá r¬i ®em vÒ nu«i d­ìng vµ truyÒn thô vâ c«ng.Thóy Yªn m«n vµ §­êng M«n cïng x­ng lµ 'Vâ L©m NhŞ M«n', chØ tiÕc lµ hµnh tung thÇn bİ nªn x­a nay İt Nam tö h¸n nµo ®­îc tháa niÒm mong ­íc.Thóy Yªn m«n cuèi cïng lµ Thiªn ®­êng hay §Şa ngôc.",4,
	"§¬n §ao/Uworld1000_cuiyandandao",
	"Song ®ao/Uworld1000_cuiyanshuangdao",
	"Hç trî /Uworld1000_cuiyanfuzhu",
	"§Ó ta suy nghÜ l¹i!/no")
end

function Uworld1000_shaolin()
	Say(DescLink_GeDaMenPaiSkills,0," ThiÕu L©m Tù s¸ng lËp tõ thêi B¾c Ngôy HiÕu V¨n §Õ N¨m Th¸i Hßa thø 19 (C«ng Nguyªn n¨m 495) ",5,
	"QuyÒn ThiÕu L©m/Uworld1000_quanshaolin",
	"C«n ThiÕu L©m/Uworld1000_gunshaolin",
	"§ao ThiÕu L©m/Uworld1000_daoshaolin",
	"Hç trî /Uworld1000_shaolinfuzhu",
	"§Ó ta suy nghÜ l¹i!/no")
end

function Uworld1000_gaibang()
	Say(DescLink_GeDaMenPaiSkills,0,"LŞch sö cña C¸i Bang nãi ra rÊt dµi, ®­îc mÖnh danh Thiªn H¹ §Ö NhÊt Bang. C¸i Bang bang chóng nh­ Ngäa hæ tµng long nh©n tµi v« sè.",4,
	"Ch­ëng C¸i/Uworld1000_zhanggai",
	"C«n C¸i/Uworld1000_gungai",
	"Hç trî /Uworld1000_gaibangfuzhu",
	"§Ó ta suy nghÜ l¹i!/no")
end

function Uworld1000_kunlun()
	Say(DescLink_GeDaMenPaiSkills,0,"C«n L«n ph¸i ë tËn T©y Vùc, tuy ®­îc s¸ng lËp ®· l©u nh­ng sau khi S­ Tæ khai ph¸i qua ®êi, trong ph¸i kh«ng cã ai lµ xuÊt chóng. M·i cho ®Õn h¬n tr¨m n¨m tr­íc, C«n Lu©n ph¸i xuÊt hiÖn mét vŞ kú nh©n. Ng­êi nµy kh«ng nh÷ng t¹o ®­îc uy danh lõng lÉy trong vâ l©m Trung Nguyªn mµ cßn gióp tªn tuæi cña C«n Lu©n b¾t ®Çu lan réng kh¾p vâ l©m.",5,
	"§ao C«n L«n/Uworld1000_daokunlun",
	"KiÕm C«n L«n/Uworld1000_jiankunlun",
	"Bïa chó /Uworld1000_zuzhoukunlun",
	"Hç trî /Uworld1000_kunlunfuzhu",
	"§Ó ta suy nghÜ l¹i!/no")
end

function Uworld1000_wudu()
	Say(DescLink_GeDaMenPaiSkills,0,"Bæn gi¸o chØ míi thµnh lËp vµi n¨m nay nh­ng ph¸t triÓn rÊt m¹nh. Ngò §éc gi¸o tæ chøc nghiªm ngÆt, Gi¸o chñ s¸ng lËp ph¸i H¾c DiÖn Lang Qu©n. Ngò §éc gi¸o lµ 1 tæ chøc thÇn bİ, ng­êi trong giang hå nghe ®Õn ®Òu biÕn s¾c. T«n chØ cña Ngò §éc gi¸o lµ 'Lîi İch quyÕt ®Şnh b¹n thï', cã lîi míi lµm, kh«ng qu¶n bÊt kÓ chuyÖn thŞ phi chİnh tµ. Ngò §éc gi¸o cßn cã 1 ®iÒu lµ 'KÎ ®¾c téi víi Ngò §éc gi¸o, sÏ chÕt kh«ng toµn th©y'.",5,
	"Ch­ëng ®éc/Uworld1000_zhangdu",
	"§ao ®éc/Uworld1000_daodu",
	"Bïa chó /Uworld1000_zuzhouwudu",
	"Hç trî /Uworld1000_wudufuzhu",
	"§Ó ta suy nghÜ l¹i!/no")
end

function Uworld1000_emeijianzong()
	Say(DescLink_GeDaMenPaiSkills,0,"KiÕm T«ng: NhÊt DiÖp Tri Thu (NhËp m«n), Nga Mi KiÕm Ph¸p (cÊp 10), Th«i Song Väng NguyÖt (cÊp 30), BÊt DiÖt BÊt TuyÖt (cÊp 60), PhËt Ph¸p V« Biªn (trÊn ph¸i), Tam Nga TÒ TuyÕt (cÊp 90).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_emeiqizong()
	Say(DescLink_GeDaMenPaiSkills,0,"Khİ T«ng: Phiªu TuyÕt Xuyªn V©n (NhËp m«n), Nga Mi Ch­ëng Ph¸p (cÊp 10), Tø T­îng §ång Quy (cÊp 30), PhËt Quang Phæ ChiÕu (cÊp 60), PhËt Ph¸p V« Biªn (trÊn ph¸i), Phong S­¬ng To¸i ¶nh (cÊp 90).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_emeifuzhu()
	Say(DescLink_GeDaMenPaiSkills,0,"Vâ c«ng hç trî: Tõ Hµng Phæ §é (cÊp 20), Méng §iÖp (cÊp 30), L­u Thñy (cÊp 40), PhËt T©m Tõ H÷u (cÊp 50), Thanh ¢m Ph¹n X­íng (cÊp 60), Phæ §é Chóng Sinh (cÊp 90).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_tangmenfeidao()
	Say(DescLink_GeDaMenPaiSkills,0,"Phi ®ao: Tİch LŞch §¬n (NhËp m«n), §­êng M«n ¸m Khİ (cÊp 10), Truy T©m TiÔn (cÊp 30), TiÓu Lı Phi ®ao (cÊp 60), T©m Nh·n (trÊn ph¸i), NhiÕp Hån NguyÖt ¶nh (cÊp 90).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_tangmennujian()
	Say(DescLink_GeDaMenPaiSkills,0,"Tô TiÔn: Tİch LŞch §¬n (NhËp m«n), §­êng M«n ¸m Khİ (cÊp 10), M¹n Thiªn Hoa Vò (cÊp 30), Thiªn La §Şa Vâng (cÊp 60), T©m Nh·n (trÊn ph¸i), B¹o Vò Lª Hoa (cÊp 90).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_tangmenfeibiao()
	Say(DescLink_GeDaMenPaiSkills,0,"Phi Tiªu: Tİch LŞch §¬n (NhËp m«n), §­êng M«n ¸m Khİ (cÊp 10), §o¹t Hån Tiªu (cÊp 30), T¶n Hoa Tiªu (cÊp 60), T©m Nh·n (trÊn ph¸i), Cöu Cung Phi Tinh (cÊp 90).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_tangmenxianjing()
	Say(DescLink_GeDaMenPaiSkills,0,"C¹m bÉy: §Şa DiÖm Háa (cÊp 10), §éc Thİch Cèt (cÊp 20), Xuyªn T©m Thİch (cÊp 30), Hµn B¨ng Thİch (cÊp 40), L«i Kİch ThuËt (cÊp 50), Lo¹n hoµn Kİch (cÊp 90).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_daotianwang()
	Say(DescLink_GeDaMenPaiSkills,0,"Thiªn V­¬ng §ao: Kinh L«i Tr¶m (NhËp m«n), Thiªn V­¬ng §ao ph¸p (cÊp 10), B¸t Phong Tr¶m (cÊp 30), V« T©m Tr¶m (cÊp 60), Thiªn V­¬ng ChiÕn ı (trÊn ph¸i), Ph¸ Thiªn Tr¶m (cÊp 90).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_qiangtianwang()
	Say(DescLink_GeDaMenPaiSkills,0,"Thiªn V­¬ng Th­¬ng: Håi Phong L¹c Nh¹n (NhËp m«n), Thiªn V­¬ng Th­¬ng ph¸p (cÊp 10), D­¬ng Quan Tam §iÖp (cÊp 30), HuyÕt ChiÕn B¸t Ph­¬ng (cÊp 60), Thiªn V­¬ng ChiÕn ı (trÊn ph¸i), Truy Tinh Trôc NguyÖt (cÊp 90).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_chuitianwang()
	Say(DescLink_GeDaMenPaiSkills,0,"Thiªn V­¬ng Chïy: Tr¶m Long QuyÕt (NhËp m«n), Thiªn V­¬ng Chïy ph¸p (cÊp 10), Hµng V©n QuyÕt (cÊp 30), Thõa Long QuyÕt (cÊp 60), Thiªn V­¬ng ChiÕn ı (trÊn ph¸i), Truy Phong QuyÕt (cÊp 90).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_fuzhutianwang()
	Say(DescLink_GeDaMenPaiSkills,0,"Hç trî: TÜnh T©m QuyÕt (cÊp 20), §o¹n Hån Thİch (cÊp 40), Kim Chung Tr¸o (cÊp 50).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_zhanren()
	Say(DescLink_GeDaMenPaiSkills,0,"ChiÕn NhÉn: Tµn D­¬ng Nh­ HuyÕt (NhËp m«n), Thiªn NhÉn M©u Ph¸p (cÊp 10), LiÖt Háa T×nh Thiªn (cÊp 30), Th©u Thiªn Ho¸n NhËt (cÊp 60), Thiªn Ma Gi¶i ThÓ (trÊn ph¸i), V©n Long Kİch (cÊp 90).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_moren()
	Say(DescLink_GeDaMenPaiSkills,0,"Ma NhÉn: §¬n ChØ LiÖt DiÖm (NhËp m«n), Thiªn NhÉn §ao Ph¸p (cÊp 10), Th«i S¬n §iÒn H¶i (cÊp 30), Ma DiÖm ThÊt S¸t (cÊp 60), Thiªn Ma Gi¶i ThÓ (trÊn ph¸i), Thiªn Ngo¹i L­u Tinh (cÊp 90).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_zuzhouren()
	Say(DescLink_GeDaMenPaiSkills,0,"Bïa Chó: Háa Liªn PhÇn Hoa (cÊp 10), ¶o ¶nh Phi Hå (cÊp 20), Phi Hång V« Tİch (cÊp 30), Bi T« Thanh Phong (cÊp 40), LŞch Ma §o¹t Hån (cÊp 50), NhiÕp Hån Lo¹n T©m (cÊp 90).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_wudangjiancong()
	Say(DescLink_GeDaMenPaiSkills,0,"KiÕm T«ng: Th­¬ng H¶i Minh NguyÖt (NhËp m«n), Vâ §ang KiÕm Ph¸p (cÊp 10), KiÕm Phi Kinh Thiªn (cÊp 30), Tam Hoµn Thao NguyÖt (cÊp 60), Th¸i Cùc ThÇn C«ng (trÊn ph¸i), Nh©n KiÕm Hîp NhÊt (cÊp 90).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_wudangqizong()
	Say(DescLink_GeDaMenPaiSkills,0,"Khİ T«ng: Né L«i ChØ (NhËp m«n ), Vâ §ang QuyÒn Ph¸p (cÊp 10), B¸c CÊp Nhi Phôc (cÊp 30), V« Ng· V« KiÕm (cÊp 60), Th¸i Cùc ThÇn C«ng (trÊn ph¸i), Thiªn §Şa V« Cùc (cÊp 90).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_wudangfuzhu()
	Say(DescLink_GeDaMenPaiSkills,0,"Vâ c«ng Hç trî:  ThÊt Tinh TrËn (cÊp 20), ThÕ V©n Tung (cÊp 40), Täa Väng V« Ng· (cÊp 50).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_cuiyandandao()
	Say(DescLink_GeDaMenPaiSkills,0,"§¬n §ao: Phong Hoa TuyÕt NguyÖt (NhËp m«n), Thóy Yªn §ao Ph¸p (cÊp 10), Vò §¶ L«i Hoa (cÊp 30), Môc D· L­u Tinh (cÊp 60), B¨ng Cèt TuyÕt T©m (trÊn ph¸i), B¨ng Tung V« ¶nh (cÊp 90).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_cuiyanshuangdao()
	Say(DescLink_GeDaMenPaiSkills,0,"Song §ao: Phong QuyÓn Tµn TuyÕt (NhËp m«n), Thóy Yªn Song §ao (cÊp 10), Phï V©n T¸n TuyÕt (cÊp 30), Bİch H¶i TriÒu Sinh (cÊp 60), B¨ng Cèt TuyÕt T©m?trÊn ph¸i), B¨ng T©m Tiªn Tö  cÊp 90).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_cuiyanfuzhu()
	Say(DescLink_GeDaMenPaiSkills,0,"Hç trî: B¨ng T©m Tr¸i ¶nh (cÊp 20), Hé ThÓ Hµn B¨ng (cÊp 40), TuyÕt ¶nh (cÊp 50).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_quanshaolin()
	Say(DescLink_GeDaMenPaiSkills,0,"ThiÕu L©m QuyÒn: Hµng Long BÊt Vò (NhËp m«n), ThiÕu L©m QuyÒn Ph¸p (cÊp 10), La H¸n TrËn (cÊp 30), Long Tr¶o Hæ Tr¶o (cÊp 50), Nh­ Lai Thiªn DiÖp (trÊn ph¸i), §¹t Ma §é Giang (cÊp 90).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_gunshaolin()
	Say(DescLink_GeDaMenPaiSkills,0,"ThiÕu L©m C«n: Kim Cang Phôc Ma (NhËp m«n), ThiÕu L©m C«n Ph¸p (cÊp 10), La H¸n TrËn (cÊp 30), Hoµnh T¶o Lôc Hîp (cÊp 50), Nh­ Lai Thiªn DiÖp (trÊn ph¸i), Hoµnh T¶o Thiªn Qu©n (cÊp 90).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_daoshaolin()
	Say(DescLink_GeDaMenPaiSkills,0,"ThiÕu L©m §ao: Kim Cang Phôc Ma (NhËp m«n), ThiÕu L©m §ao Ph¸p (cÊp 10), La H¸n TrËn (cÊp 30), Ma Ha V« L­îng (cÊp 50), Nh­ Lai Thiªn DiÖp (trÊn ph¸i), V« T­íng Tr¶m (cÊp 90).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_shaolinfuzhu()
	Say(DescLink_GeDaMenPaiSkills,0,"Hç trî: BÊt ®éng Minh V­¬ng (cÊp 20), S­ Tö Hèng (cÊp 40), DŞch C©n Kinh (cÊp 60).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_zhanggai()
	Say(DescLink_GeDaMenPaiSkills,0,"Ch­ëng C¸i: KiÕn Nh©n ThÇn Thñ (NhËp m«n), C¸i Bang Ch­ëng Ph¸p (cÊp 10), Gi¸ng Long Ch­ëng (cÊp 30), Kh¸ng Long H÷u Hèi (cÊp 50), Tóy §iÖp Cuång Vò (trÊn ph¸i), Phi Long T¹i Thiªn (cÊp 90).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_gungai()
	Say(DescLink_GeDaMenPaiSkills,0,"C«n C¸i: Duyªn M«n Th¸c B¸t (NhËp m«n), C¸i Bang Bæng Ph¸p (cÊp 10), §¶ CÈu TrËn (cÊp 30), Bæng §¶ ¸c CÈu (cÊp 50), Tóy §iÖp Cuång Vò (trÊn ph¸i), Thiªn H¹ V« CÈu (cÊp 90).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_gaibangfuzhu()
	Say(DescLink_GeDaMenPaiSkills,0,"Hç trî: Hãa HiÓm Vi Di (cÊp 20), Ho¹t BÊt L­u Thñ (cÊp 40), Tiªu Diªu C«ng (cÊp 60).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_daokunlun()
	Say(DescLink_GeDaMenPaiSkills,0,"§ao C«n L«n: H« Phong Ph¸p (NhËp m«n), §ao C«n L«n ph¸p (cÊp 10), NhÊt Khİ Tam Thanh (cÊp 30), Cuång Phong SËu §iÖn (cÊp 50), S­¬ng Ng¹o C«n L«n (trÊn ph¸i), Ng¹o TuyÕt Tiªu Phong (cÊp 90).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_jiankunlun()
	Say(DescLink_GeDaMenPaiSkills,0,"KiÕm C«n L«n: Cuång L«i ChÊn §Şa (NhËp m«n), KiÕm C«n L«n Ph¸p (cÊp 10), Thiªn TÕ TÊn L«i (cÊp 30), Ngò L«i Chİnh Ph¸p (cÊp 60), S­¬ng Ng¹o C«n L«n (trÊn ph¸i), L«i ®éng Cöu Thiªn (cÊp 90).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_zuzhoukunlun()
	Say(DescLink_GeDaMenPaiSkills,0,"Bïa Chó: Thóc Ph­îc Chó (cÊp 10), Ky B¸n Phï (cÊp 20), B¾c Minh §¸o H¶i (cÊp 30), Khi Hµn Ng¹o TuyÕt (cÊp 40), Mª Tung ¶o ¶nh (cÊp 50), Tóy Tiªn T¸ Cèt (cÊp 90).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_kunlunfuzhu()
	Say(DescLink_GeDaMenPaiSkills,0,"Hç trî: Thanh Phong Phï (cÊp 10), Thiªn Thanh §Şa Träc (cÊp 30), Khİ T©m Phï (cÊp 40).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_zhangdu()
	Say(DescLink_GeDaMenPaiSkills,0,"Ch­ëng §éc: §éc Sa Ch­ëng (NhËp m«n), Ngò §éc Ch­ëng Ph¸p (cÊp 10), U Minh Kh« L©u (cÊp 30), Thiªn C­¬ng §Şa S¸t (cÊp 60), Ngò §éc Kú Kinh (trÊn ph¸i), ¢m Phong Thùc Cèt (cÊp 90).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_daodu()
	Say(DescLink_GeDaMenPaiSkills,0,"§ao §éc: HuyÕt §ao §éc S¸t (NhËp m«n), Ngò §éc §ao Ph¸p (cÊp 10), B¸ch §éc Xuyªn T©m (cÊp 30), Chu C¸p Thanh Minh (cÊp 60), Ngò §éc Kú Kinh (trÊn ph¸i), HuyÒn ¢m Tr¶m (cÊp 90).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_zuzhouwudu()
	Say(DescLink_GeDaMenPaiSkills,0,"Bïa Chó: Cöu Thiªn Cuång L«i (cÊp 10), Xİch DiÖm Thùc Thiªn (cÊp 20), B¨ng Lam HuyÒn Tinh (cÊp 30), Xuyªn Y Ph¸ Gi¸p (cÊp 40), Xuyªn T©m §éc Thİch (cÊp 50), §o¹n C©n Hñ Cèt (cÊp 90).",1,"Sau sÏ quay l¹i/no")
end

function Uworld1000_wudufuzhu()
	Say(DescLink_GeDaMenPaiSkills,0,"Vâ c«ng Hç trî:  T¹p Nan D­îc Kinh (cÊp 20), V« H×nh §éc (cÊp 30), V¹n §éc Thùc T©m (cÊp 40).",1,"Sau sÏ quay l¹i/no")
end