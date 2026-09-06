-- map_helper.lua
-- By: Dan_Deng(2004-06-28)
-- Ö÷Î¬»¤±í£¬¸÷ÎåĞĞ¡¢µÈ¼¶Á·¼¶ÃÔ¹¬
Include("\\script\\activitysys\\npcdailog.lua")

lv_str={	"<#>kh«ng tíi cÊp 10","<#>H¬n cÊp 10","<#>H¬n cÊp 20","<#>H¬n cÊp 30","<#>H¬n cÊp 40","<#>H¬n cÊp 50","<#>H¬n cÊp 60","<#>H¬n cÊp 70","<#>H¬n cÊp 80","<#>CÊp 90 trë lªn"}
map0 = {	"<#>Khu vùc t©n thñ th«n",
			"<#>Khu vùc c¸c ph¸i vµ c¸c thµnh thŞ",
			"<#>KiÕm c¸c trung nguyªn,kiÕm c¸c t©y b¾c, kiÕm c¸c t©y nam, la tiªu s¬n, vò l¨n s¬n,tÇn l¨ng",
			"<#>Thôc c­¬ng s¬n,miªu lÜnh,yÕn tö ®éng, d­îc v­¬ng ®éng tÇng mét,kho¸i häat l©m",
			"<#>Kinh hoµng ®éng,phôc ng­u s¬n t©y,®iÓm th­¬ng ®éng tÇng ba,thiÕu l©m mËt thÊt",
			"<#>H­íng thñy ®éng,thiªn t©m ®éng,®éng ®×nh hå,b¨ng nguyÖt mª cung",
			"<#>Thanh loa ®¶o,La h¸n trËn,Hµnh s¬n ph¸i,t­¬ng d­¬ng nha m«n mËt ®¹o",
			"<#>§¹i tï ®éng,thanh loa ®¶o,Linh cèc ®éng,L·o hæ ®éng,L©m du quan",
			"<#>Sa m¹c ®Şa biÓu,phi thiªn ®éng,phï dung ®éng,l­ìng thñy ®éng,b¨ng hµ ®éng,thanh khª ®éng,tr­êng b¹ch s¬n c­íc",
			"<#>Sa m¹c s¬n ®éng tÇng mét,hai,ba,xi v­u ®éng,tr­êng b¹ch s¬n nam,tr­êng b¹ch s¬n b¾c"}
map1 = {	"<#>Khu vùc t©n thñ th«n",
			"<#>RÇn thu vùc c¸c ph¸i vµ c¸c thµnh thŞ",
			"<#>KiÕm c¸c trung nguyªn,kiÕm c¸c t©y b¾c, kiÕm c¸c t©y nam, la tiªu s¬n, vò l¨n s¬n,tÇn l¨ng",
			"<#>Vò di s¬n,tÇn l¨ng tÇng mét,b¹ch v©n ®éng,thæ phØ ®éng",
			"<#>Thanh thµnh s¬n,®iÓm th­¬ng s¬n,ph­îng nh·n ®éng,®iÓm th­¬ng ®éng tÇng mét,thÇn tiªn ®éng",
			"<#>H­íng thñy ®éng,nghiÖt long ®éng,ngäc hoa ®éng,thiªn t©m th¸p tÇng mét,hai",
			"<#>D­¬ng gi¸c ®éng,Tróc ti ®éng tÇng mét,hµnh s¬n ph¸i,thiªn t©m th¸p tÇng ba",
			"<#>Long nh·n ®éng,TÇn l¨ng tÇng hai,Tróc ti ®éng tÇng hai,tr­êng giang nguyªn ®Çu,l©m du quan",
			"<#>Sa m¹c ®Şa biÓu,vò l¨ng ®éng,d­¬ng trung ®éng,cæ d­¬ng ®éng,nh¹n th¹ch ®éng,d­îc v­¬ng ®éng tÇng ba,tÇn l¨ng tÇng ba,tr­êng b¹ch s¬n c­íc",
			"<#>Sa m¹c s¬n ®éng tÇng,mét,hai,ba,d­îc v­¬ng ®éng tÇng bèn,tiÕn cóc ®éng,tr­êng b¹ch s¬n nam,tr­êng b¹ch s¬n b¾c"}
map2 = {	"<#>Khu vùc t©n thñ th«n",
			"<#>RÇn thu vùc c¸c ph¸i vµ c¸c thµnh thŞ",
			"<#>KiÕm c¸c trung nguyªn,kiÕm c¸c t©y b¾c, kiÕm c¸c t©y nam, la tiªu s¬n, vò l¨n s¬n,tÇn l¨ng",
			"<#>B¹ch v©n ®éng,thôc c­¬ng s¬n,thæ phØ ®éng,phôc ng­u s¬n ®«ng,kim quang ®éng",
			"<#>ph­îng nh·n ®éng,®iÓm th­¬ng ®éng tÇng mét,thÇn tiªn ®éng,phôc ng­u s¬n t©y,kinh hoµng ®éng",
			"<#>H­íng thñy ®éng,thiªn t©m th¸p tÇng mét,hai,¸c b¸ ®Şa ®¹o",
			"<#>Thiªn t©m th¸p tÇng ba,la h¸n trËn,cÊm ®Şa mª cung,hµnh s¬n ph¸i",
			"<#>Tr­êng giang nguyªn ®Çu,s¬n b¶o ®éng,®µo hoa nguyªn,d­îc v­¬ng ®éng tÇng hai,l©m du quan",
			"<#>Sa m¹c ®Şa biÓu,phi thiªn ®éng,phï dung ®éng,d­¬ng trung ®éng,cæ d­¬ng ®éng,d­îc v­¬ng ®éng tÇng ba,tr­êng b¹ch s¬n c­íc",
			"<#>Sa m¹c s¬n ®éng tÇng mét,hai,ba ,Xi v­u ®éng,tiÕn cóc ®éng,tr­êng b¹ch s¬n nam,tr­êng b¹ch s¬n b¾c"}
map3 = {	"<#>Khu vùc t©n thñ th«n",
			"<#>RÇn thu vùc c¸c ph¸i vµ c¸c thµnh thŞ",
			"<#>KiÕm c¸c trung nguyªn,kiÕm c¸c t©y b¾c, kiÕm c¸c t©y nam, la tiªu s¬n, vò l¨n s¬n,tÇn l¨ng",
			"<#>Phôc ng­u s¬n ®éng,kim quang ®éng,t©n l¨ng tÇng mét,vò di s¬n",
			"<#>Táa v©n ®éng,long cung ®éng,®iÓm th­¬ng ®éng tÇng hai,thanh thµnh s¬n,®iÓm th­¬ng s¬n",
			"<#>H­íng thñy ®éng,biÖn kinh thiÕt th¸p tÇng mét,hai,ba,t­êng v©n ®éng tÇng mét,ngù hoa viªn s¬n ®éng",
			"<#>T­êng v©n ®éng tÇng hai,ba,bèn,thiªn nhÉn th¸nh ®éng,d­¬ng gi¸c ®éng,tróc ti ®éng",
			"<#>D­îc v­¬ng ®éng tÇng hai,l·o hæ ®éng,t­êng v©n ®éng tÇng n¨m,linh cèc ®éng,l©m du quan",
			"<#>Sa m¹c ®Şa biÓu,vò l¨ng ®éng,v« danh ®éng,nh¹n th¹ch ®éng,tÇn l¨ng tÇng ba,tr­êng b¹ch s¬n c­íc",
			"<#>Sa m¹c s¬n ®éng tÇng mét,hai,ba,d­îc v­¬ng ®éng tÇng bèn,hiªn viªn ®éng,tr­êng b¹ch s¬n nam,tr­êng b¹ch s¬n b¾c"}
map4 = {	"<#>Khu vùc t©n thñ th«n",
			"<#>RÇn thu vùc c¸c ph¸i vµ c¸c thµnh thŞ",
			"<#>KiÕm c¸c trung nguyªn,kiÕm c¸c t©y b¾c, kiÕm c¸c t©y nam, la tiªu s¬n, vò l¨n s¬n,tÇn l¨ng",
			"<#>Miªu lÜnh,d­îc v­¬ng ®éng tÇng mét,kho¸i ho¹t l©m,phôc ng­u s¬n ®éng,kim quang ®éng,tuyÕt b¸o ®éng tÇng mét,hai",
			"<#>Phong s¬n ®éng,kª quan ®éng,hoµng ha nguyªn ®Çu,táa v©n ®éng,®iÓm th­¬ng ®éng tÇng hai",
			"<#>H­íng thñy ®éng,b¨ng huyªt mª cung,l­u tiªn ®éng tÇng mét,thiÕt th¸p mª cung tÇng mét,hai,ba",
			"<#>Hµnh s¬n ph¸i,t­¬ng d­¬ng nha m«n mËt ®¹o,l­u tiªn ®éng tÇng hai,ba,bèn",
			"<#>§µo hoa nguyªn,l­u tiªn ®éng tÇng n¨m,s¸u, tÇn l¨ng tÇng hai,long nh·n ®éng,l©m du quan",
			"<#>Sa m¹c ®Şa biÓu,l­ìng thñy ®éng,v« danh ®éng,b¨ng hµ ®éng,thanh khª ®éng,tr­êng b¹ch s¬n b¾c",
			"<#>Sa m¹c s¬n ®éng tÇng mét,hai,ba,d­îc v­¬ng ®éng tÇng bèn,hiªn viªn ®éng,tr­êng b¹ch s¬n nam,tr­êng b¹ch s¬n b¾c"}

function map_help()
	ser = GetSeries() + 1
	lvl = floor(GetLevel()/10 + 1)
	if (lvl > 10) then lvl = 10 end
	if (ser == 1) then
		Say(npc_name.."<#>§èi víi<color=red>HÖ kim"..lv_str[lvl].."<#><color>B¶n ®å phï hîp luyÖn c«ng lµ "..map0[lvl].."<#>®Õn ®Şa ®iÓm",0)
	elseif (ser == 2) then
		Say(npc_name.."<#>§èi víi<color=red>HÖ méc"..lv_str[lvl].."<#><color>B¶n ®å phï hîp luyÖn c«ng lµ "..map1[lvl].."<#>®Õn ®Şa ®iÓm",0)
	elseif (ser == 3) then
		Say(npc_name.."<#>§èi víi<color=red>HÖ thñy"..lv_str[lvl].."<#><color>B¶n ®å phï hîp luyÖn c«ng lµ "..map2[lvl].."<#>®Õn ®Şa ®iÓm",0)
	elseif (ser == 4) then
		Say(npc_name.."<#>§èi víi<color=red>HÖ háa"..lv_str[lvl].."<#><color>B¶n ®å phï hîp luyÖn c«ng lµ "..map3[lvl].."<#>®Õn ®Şa ®iÓm",0)
	else
		Say(npc_name.."<#>§èi víi<color=red>HÖ thæ"..lv_str[lvl].."<#><color>B¶n ®å phï hîp luyÖn c«ng lµ "..map4[lvl].."<#>®Õn ®Şa ®iÓm",0)
	end
end

function skill_help()
	fact_num = GetLastFactionNumber() + 1
	if (fact_num == 1) then				-- ÉÙÁÖ
		Talk(5,"","§èi víi<color=red>ThiÕu l©m<color>Ph¸i thiÕu l©m vâ nghÖ ®¹i th¸i chia lµm ba lo¹i","Thø nhËt lµ thiÕu l©m quyÒn ph¸p sì h÷u,hµng long b¸t vò,thiÕu l©m quyÒn ph¸p,s­ tö hãng,long hæ tr¶o,®¹t ma ®é giang","Thø hai lµ thiÕu l©m c«n ph¸p sì h÷u kim cang phôc ma,thiÕu l©m c«n ph¸p,hoµnh t¶o lôc hîp,hoµnh t¶o thiªn qu©n","Thø ba lµ ThiÕu l©m ®ao ph¸p sì h÷u kim c­¬ng phôc ma,thiÕu l©m ®ao ph¸p,ha ma v« l­îng,v« t­íng tr·m","Ngoµi c¸c chiªu thøc ®ã ra cßn cã bÊt ®éng minh v­¬ng, la h¸n trËn,dŞch ch©n kinh,nh­ lai thiªn ®iÖp hç trî rÊt nhiÒu cho thiÕu l©m")
	elseif (fact_num == 2) then		-- ÌìÍõ
		Talk(5,"","§èi víi<color=red>Thiªn v­¬ng<color>Vâ nghÖ thiªn v­¬ng chia lµm ba ®­êng riªng biÖt","§­êng thø nhÊt thiªn v­¬ng ®ao sì h÷u kinh l«i tr¶m,thiªn v­¬ng ®ao ph¸p,b¸t phong tr¶m,®o¹t hån thİch,v« t©m tr¶m,ph¸ thiªn tr¶m","§­êng thø hai Thiªn v­¬ng th­¬ng ph¸p sì h÷u Håi phong l¹c nh¹n,thiªn v­¬ng th­¬ng ph¸p,d­¬ng quan tam ®iÖp, ®o¹t hån thİch,huyÕt chiÕn b¸t ph­¬ng, truy t×m trôc nguyÖt","§­êng thø ba Thiªn v­¬ng chïy sì h÷u Tr¶m long quyÕt,thiªn v­¬ng chïy ph¸p, hµng v©n quyÕt,§o¹t hån thİch,thõa long quyÕt, truy phong nguyÖt","Ngoµi ra cßn sì h÷u c¸c kü n¨ng kh¸c nh­ TÜnh t©m quyÕt, kim trung ch¸o,thiªn v­¬ng chiÕn ı")
	elseif (fact_num == 3) then		-- ÌÆÃÅ
		Talk(5,"","§èi víi<color=red>§­êng m«n<color>Vâ nghÖ chia lµm bèn h­íng riªng biÖt","H­íng thø nhËt lµ phi ®ao sì h÷u c¸c tuyÖt kü nh­ phİch lŞch ®¹n,truy t©m tiÔn,tiÓu lı phi ®ao,nhiÕp hån nguyÖt ¶nh","H­íng thø hai sö dông phi tiªu vµ sì h÷u c¸c kü n¨ng phİch lŞch ®¹n,®o¹t hån tiªu,t¸n hoa tiªu,c÷u cung phi tinh","H­íng thø ba sñ dông tô tiÖn thuËt sì h÷u c¸c kü n¨ng phİch lŞch ®¹n,m¹n thiªn hoa vò,thiªn la ®Şa vâng,b¹o vò lª hoa,","H­íng thø t­ sö dòng bÉy sì h÷u c¸c kü n¨ng ®Şa diÔm háa,®éc thø cèt,xuyªn t©m thİch,hµn b¨ng thİch,l«i kİch thuËt,lo¹n hoµn kİch")
	elseif (fact_num == 4) then		-- Îå¶¾
		Talk(5,"","§èi víi<color=red>Ngò ®éc<color>Vâ c«ng chia lµm ba h­íng kh¸c nhau","H­íng thø nhÊt ch­ëng ph¸p sì h÷u c¸c kü n¨ng §éc sa ch­ëng,ngò ®éc ch­ëng ph¸p,U minh kh« l©u,Thiªn c­¬ng ®Şa s¸t,¢m phong thôc cèt","H­íng thø hai lµ ®ao ph¸p sì h÷u kü n¨ng HuyÕt ®ao ®éc s¸t,ngò ®éc ®ao ph¸p, b¸ch ®éc xuyªn t©m,chu c¸p thanh minh, huyÒn ©m tr·m","H­íng thø ba sö dông bïa chó sì h÷u c¸c tuyÖt kü cöu thiªn cuång l«i,Xİch diÖn thôc thiªn,b¨ng lam huyÒn tinh,xuyªn y ph¸ gi¸p,v¹n c¬ thùc t©m,Xuyªn t©m ®éc thİch,®o¹n c©n hñ cèt","Ngoµi ra cßn sì h÷u mét sè tuyÖt kü hæ trî nh­ Tap nan d­îc kinh,v« h×nh ®éc,ngò ®éc k× kinh")
	elseif (fact_num == 5) then		-- ¶ëáÒ
		Talk(4,"","§èi víi<color=red>Nga mi<color>Vâ c«ng chia lµm ba h­íng kh¸c nhau","H­íng thø nhÊt nga mi kiÕm ph¸p së h÷u c¸c kü n¨ng nhÊt diÖp tri tru, nga mi kiÕm ph¸p,th«i song väng nguyÖt,bÊt diÖt bÊt tuyÖt,tam nga tÕ nguyÖt","H­íng thø hai nga mi ch­ëng ph¸p sì h÷u kü n¨ng phiªu tuyÕt xuyªn v©n,nga mi ch­ëng,tø t­îng ®ång quy,phËt quang phæ chiÕu,phong s­¬ng to¸i ¶nh","H­íng thö ba nghiªn vª hç trî víi c¸c kü n¨ng nh­ tø hµng phæ ®é,méng ®iÖp, l­u thñy,phËt t©m tõ h÷u,thanh ©m ph¹m x­íng,phËt ph¸p v« biªn,phæ ®é chóng sinh")
	elseif (fact_num == 6) then		-- ´äÑÌ
		Talk(4,"","§èi víi<color=red>Thóy yªn<color>Vâ nghÖ chia lµm hai h­íng kh¸c nhau","h­íng thø nhÊt thóy yªn ®¬n ®ao sì h÷u Phong hoa tuyÕt nguyÖt,thóy yªn ®ao ph¸p,vò ®µ lª hoa,môc d· l­u tinh,b¨ng tung v« ¶nh","H­íng thø hai thóy yªn song ®ao Phong quyªn tµn tuyÕt,thóy yªn song ®ao,phï v©n t¸n tuyÕt,bİch h¶i triÒu sinh,b¨ng t©m tiªn tö","Ngoµi ra cßn hç trî c¸c kü n¨ng kh¸c nh­ B¨ng t©m tr¸i ¶nh,Hé thÎ hµn b¨ng,TuyÕt ¶nh,b¨ng cèt tuyÕt t©m")
	elseif (fact_num == 7) then		-- Ø¤°ï
		Talk(4,"","§èi víi<color=red>C¸i bang<color>Vâ nghÖ chia lµm hai ®­êng kh¸c nhau","H­íng thø nhÊt C¸i bang ch­ëng ph¸p sì h÷u kü n¨ng KiÕn nh©n thÇn phï, C¸i bang ch­ëng ph¸p, gi¸ng long ch­ëng,kh¸ng long h÷u hèi,phi long t¹i thiªn","H­íng thø hai C¸i bang bæng ph¸p sì h÷u kü n¨ng Diªn m«n th¸c b¸t,C¸i bang bæng ph¸p, ®· cÈu trËn,Bæng ®· ¸c cÈu,Thiªn h¹ v« cÈu","Ngoµi ra cßn sì h÷u mét sè kü n¨ng nh­ Hãa hiªm vi di,Ho¹t b¸t l­u thñ,Tiªu diªu c«ng,Tóy ®iÖp cuång vò")
	elseif (fact_num == 8) then		-- ÌìÈÌ
		Talk(4,"","§èi víi<color=red>Thiªn nhÉn<color>Vâ c«ng chia lµm ba lo¹i kh¸c nhau","§­êng thø nhÊt Thiªn nhÉn m©u ph¸p sì h÷u tuyÖt kü Tµn d­¬ng nh­ tuyÕt,Thiªn nhÉn m©u ph¸p,liÖt háa t×nh thiªn,Th©u thiªn ho¸n nhËt,V©n long kİch","§­êng thø hai Thiªn nh©n §ao ph¸p sì h÷u tuyÖt kü §¬n chØ liÖt diÖm,thiªn nhËn ®ao ph¸p, Th«i s¬n ®iÒn h¶i,Ma diÖm thÊt s¸t,thiªn ngo¹i l­u tinh","§­êng thø ba Thiªn nhÉn bïa chó sì h÷u c¸c kü n¨ng nh­ Háa liªn phÇn hoa,¶o ¶nh phi hæ,Phi hång v« tİch,B× t« thanh phong,LŞch ma ®o¹t hån,nhiÕp hån lo¹n t©m")
	elseif (fact_num == 9) then		-- Îäµ±
		Talk(4,"","§èi víi<color=red>Vâ ®ang<color> Vâ c«ng chia lµm hai h­íng kh¸c nhau","H­íng thø nhÊt Vâ ®ang quyÒn ph¸p sì h÷u c¸c tuyÖt kü nh­ Néi l«i chØ, Vâ ®ang quyÒn ph¸p, B¸c cËp nhi thôc, v« ng· v« kiÕm, Thiªn ®Şa v« cùc","H­íng thø hai Vâ ®ang kiÕm ph¸p sî h÷u c¸c kü n¨ng nh­ Th­îng h¶i minh nguyÖt,Vâ ®ang kiÕm ph¸p,KiÕm phi kinh thiªn,Tam hoµn thao nguyÖt,Nh©n kiÕm hîp nhÊt","Ngoµi ra Vâ ®ang cßn sî h÷u mét sè tuyÖt kü nh­ ThÊt tinh trËn, thÕ v©n tung,täa vâng v« ng·, Th¸i cùc thÇn c«ng")
	elseif (fact_num == 10) then		-- À¥¢Ø
		Talk(5,"","§èi víi<color=red>C«n l«n<color> Vâ c«ng C«n l«n chia lµm ba ®­êng","§­êng thø nhÊt C«n l«n ®ao ph¸p sì h÷u kü n¨ng H« phong ph¸p, c«n l«n ®ao ph¸p,NhÊt khİ tam thanh, Cuång phong sËu ®iÖn,Ng¹o tuyÕt tiªu phong","§­êng thø hai C«n l«n kiÕm ph¸p sì h÷u c¸c kü n¨ng Cuång l«i chÊn ®Şa, c«n l«n kiÕm ph¸p, thiªn tÕ tÊn l«i,ngò l«i chİnh ph¸p,l«i ®éng cöu thiªn","§­êng thø ba C«n l«n bïa chó sì h÷u c¸c kü n¨ng Thóc ph­îc chó,thanh phong chó,K× b¸n phó,B¾c minh ®¸o h¶i,thiªn thanh ®Şa träc,Mª tung ¶o ¶nh,tóy tiªn t¸ cèt","C¸c chiªu thøc c«n l«n biÕn ¶o th«n l­êng")
	else									-- °×Ãû
		Say(npc_name.."<#>Ng­¬i ®Õn c¶ m«n ph¸i cßn kh«ng cã cßn nãi g× luyÖn v· ch· tu",0)
	end
end

GV_TSK_CD_OPEN = 35
tbDEFENCE_SIGNMAP = { 518, 519 }
tbSIGNMAP_POS = {
					{ 1582,3174 },
					{ 1588,3160 },
					{ 1604,3147 },
				}
tbDEFENCE_RETURN = {
						[1] = { 1520,3228, "Ph­îng t­êng", 2 },	--·ïÏè
						[11] = { 3018,5089, "Thµnh ®«", 1 },	--³É¶¼
						[37] = { 1632,3185, "BiÖn kinh", 2 },	--ãê¾©
						[78] = { 1439,3214, "T­¬ng d­¬ng", 1 },	--ÏåÑô
						[80] = { 1593,3197, "D­¬ng ch©u", 1 },	--ÑïÖİ
						[162] = { 1470,3269, "§¹i lı", 1 },	--´óÀí
						[176] = { 1392,3313, "L©m an", 1 },	--ÁÙ°²
					}
CD_LEVEL_LIMIT = 80		--×îµÍ²Î¼ÓÊØ³ÇµÈ¼¶

function main(sel)
	
	local nNpcIndex = GetLastDiagNpc();
	local szNpcName = GetNpcName(nNpcIndex)
	if NpcName2Replace then
		szNpcName = NpcName2Replace(szNpcName)
	end
    Msg2Player(szNpcName)
	local tbDailog = DailogClass:new(szNpcName)
	EventSys:GetType("AddNpcOption"):OnEvent(szNpcName, tbDailog, nNpcIndex)
	G_ACTIVITY:OnMessage("ClickNpc", tbDailog, nNpcIndex)
	--µ¯³ö¶Ô»°¿ò
	
	tbDailog.szTitleMsg = npc_name.."<#>§õng nghÜ ta kh«ng ph¶i lµ ng­êi trong giang hå råi kh«ng biÕt g×, chuyÖn ta biÕt cßn nhiÒu h¬n c¸c ng­¬i"
	
	local mapid = SubWorldIdx2ID( SubWorld )
	local camp = tbDEFENCE_RETURN[mapid][4]
						
	tbDailog:AddOptEntry("ChiÕn tr­êng Tèng Kim ", CallRescrptFunc )
	tbDailog:AddOptEntry("KiÓm tra b¶n ®å luyÖn cÊp ", map_help )
	tbDailog:AddOptEntry("Trî gióp kü n¨ng ", skill_help )
	tbDailog:AddOptEntry("T¸n gÉu ", default_talk )
	tbDailog:AddOptEntry("KÕt thóc")
	
	if ( GetGlbValue( GV_TSK_CD_OPEN ) == camp ) then
		tbDailog:AddOptEntry("Tham gia chiÕn dŞch thñ thµnh ", want2cd_signmap)
	end
	
	
	tbDailog:Show()
end

function want2cd_signmap()
	if ( GetLevel() < CD_LEVEL_LIMIT ) then
		Say(npc_name.."Ng­¬i còng muèn tham gia chiÕn tr­êng µ, tiÕc lµ cËp bËc ch­a ®ñ"..CD_LEVEL_LIMIT.."CÊp bËc cña ng­¬i cßn thÊp qu¸ vµo chiÕn tr­êng chØ tæ bá m¹ng, h·y quay vÒ thµnh tËp luyÖn thªm ®i", 0)
		return
	end
	Say(npc_name.."Ng­¬i thËt sù muèn ®i thñ thµnh ? ng­¬i rÊt dòng c¶m, ta nghe nãi b©y giê t×nh h×nh rÊt khÈn cÊp, bªn trong chØ cßn chèng ®ì ®­îc kho¶ng 2 canh giê, ta chØ cã thÓ dÉn ng­¬i ®Õn gÆp t­íng qu©n", 2, "Ta biÕt dÉn ta ®i mau/sure2cd_signmap", "§Ó ta chuÈn bŞ sau/no")
end

function sure2cd_signmap()
	LeaveTeam();
	local mapid = SubWorldIdx2ID( SubWorld )
	local camp = tbDEFENCE_RETURN[mapid][4]
	if ( camp == nil ) then
		Say(npc_name.."H×nh nh­ qu©n t×nh kh«ng æn", 0)
		return
	end
	if (camp == 1) then
		Msg2Player("Vµo b¶n ®å chiÕn tr­êng phe tèng")
	else
		Msg2Player("Vµo b¶n ®å chiÕn tr­êng phe kim")
	end
	SetTask(TASKID_FIRE_ADDR, SubWorldIdx2ID(SubWorld))
	SetFightState(0)
	NewWorld(bt_getsignpos(camp))	--campÔÚÊØ³ÇÎÀ±ø´¦¶¨Òå
end

function bt_getsignpos(camp)
	if ( camp ~= 1 and camp ~= 2 ) then
		camp = 1;
	end;
	local a = random(getn(tbSIGNMAP_POS))
	
	return tbDEFENCE_SIGNMAP[camp], tbSIGNMAP_POS[a][1], tbSIGNMAP_POS[a][2]
end

function cd_about_compose()
	Say(npc_name..":RÇn ®©y chiÕn sù thËt nhiÒu ta kh«ng r·nh gióp ng­¬i s÷a trang bŞ,ng­¬i mau ®i t×m thÇn bİ thî rÌn t×m h¾n gióp ®ì, tµi rÌn cña h¾n còng häc tõ ta nhÊt ®Şnh kh«ng thµnh vÊn ®Ò", 0)
end

function CallRescrptFunc()
	if ( GetLevel() < 40 ) then
		Talk( 1, "", npc_name..": §ao kiÕm v« t×nh cÊp thÊp h¬n 40 kh«ng thÓ vµo chiÕn tr­êng" );
	else
		Say ( npc_name..": Mêi lùa chiÕn tr­êng thİch hîp", 3, "ChiÕn tr­êng tèng kim Phe Tèng/#DoRescriptFunc(1)", "ChiÕn tr­êng tèng kim Phe Kim/#DoRescriptFunc(2)","Hñy bá/no" );
	end;
end;

function DoRescriptFunc(nSel)
	local tbsongjin_pos = {1608,3254};	--ËÎ·½×ø±êµã
	local szstr = "ËÎ";
	if (nSel == 2) then
		tbsongjin_pos = {1566,3277};
		szstr = "½ğ";
	end;
	if ( GetLevel() >= 40 and GetLevel() < 60 ) then
		NewWorld( 162, tbsongjin_pos[1], tbsongjin_pos[2]);
		SetFightState(0);
		--DisabledUseTownP(0); -- ²»ÏŞÖÆÆäÊ¹ÓÃ»Ø³Ç·û
		Msg2Player( "ChiÕn tr­êng tèng kim cÊp thÊp" );
	elseif ( GetLevel() >= 60 and GetLevel() < 90 ) then
		NewWorld( 162, tbsongjin_pos[1], tbsongjin_pos[2]);
		SetFightState(0);
		--DisabledUseTownP(0); -- ²»ÏŞÖÆÆäÊ¹ÓÃ»Ø³Ç·û
		Msg2Player( "ChiÕn tr­êng tèng kim cÊp trung" );
	else
		NewWorld( 162, tbsongjin_pos[1], tbsongjin_pos[2]);
		SetFightState(0);
		--DisabledUseTownP(0); -- ²»ÏŞÖÆÆäÊ¹ÓÃ»Ø³Ç·û
		Msg2Player( "ChiÕn tr­êng tèng kim cÊp cao" );
	end
end;

function no()
end
