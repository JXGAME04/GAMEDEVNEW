Include("\\script\\header\\map_helper.lua")
Include("\\script\\header\\repute_head.lua")

DEFAULT_TALK = {
"D¹o nµy t×nh h×nh kh«ng ®­îc b×nh th­êng, chóng ta cÇn ph¶i cÈn thËn c¶nh gi¸c, nÕu kh«ng cã m­êi c¸i ®Çu còng kh«ng ®ñ ®Òn téi.",
"Nhµ ta bµ mÑ ®ang chê ta trë vÒ ¨n c¬m dÆm, ta kh«ng d¸m c·i lêi, lµm con ph¶i thÕ ",
"Tèi h«m qua uèng nhiÒu qu¸ r­îu, ®Õn b©y giê ®Çu vÉn cßn ®au...",
"L·o tö ®· ®øng rÊt l©u míi cã thÓ c©u ®­îc nhiÒu c¸, thËt lµ nhí b×nh r­îu hoa yªu",
"NhÜ l·o vÉn cßn ë chç nµy mµ, huhu! Ch¾c ®Çu l·o b¾t ®Çu h«n mª ",
};

TANGAU = 
{
	[1] = {
	"VÖ binh Thµnh m«n: Ph­îng T­êng phñ nµy n»m ë T©y B¾c träng trÊn, mÆc dï kh«ng thÓ so víi Giang Nam phån hoa tr¸ng lÖ, nh­ng lµ n¬i thŞ tø phån thŞnh, lµ n¬i lui tíi cña ®¸m d©n th­¬ng.",
	"VÖ binh Thµnh m«n: §· tíi Ph­îng T­êng th× ®õng quªn ®i LiÔu L©m töu ph­êng nÕm thö mét chót r­îu ë n¬i ®©y",
	"VÖ binh Thµnh m«n: Ng­¬i tíi lµm ¨n hay chØ ®Õn ®Ó du ngo¹n?",
	"VÖ binh Thµnh m«n: Xem d¸ng vÎ phong trÇn cña nhµ ng­¬i ... Tõ xa tíi ®©y ph¶i kh«ng?",
	},
	[78] = {
	"VÖ binh Thµnh m«n: N¨m ®ã Nh¹c t­íng qu©n tõng ®¸nh h¹ ng­êi Kim trÊn thñ T­¬ng D­¬ng, tõ ®ã vÒ sau, T­¬ng D­¬ng lµ yÕu ®Şa cña §¹i Tèng. T­¬ng D­¬ng mÊt, §¹i Tèng ¾t l©m nguy.",
	"VÖ binh Thµnh m«n: Nhµ ta ngô ë phİa thµnh Nam, ta ®· h¬n mét th¸ng ch­a cã trë vÒ nhµ.",
	"VÖ binh Thµnh m«n: Ngµy ngµy ph¶i ®øng canh nghiªm chØnh ë ®©y, thËt lµ mÖt chÕt ®i ®­îc !",
	"VÖ binh Thµnh m«n: Thµnh T­¬ng D­¬ng x­a nay binh gia tÊt tranh ®Êt, triÒu ®×nh hµng n¨m ®Òu ph¸i träng binh ®ãng ë n¬i nµy, cã thÓ gäi lµ 'Nam Tèng yÕt hÇu' .",
	},
	[37] = {
	"VÖ binh Thµnh m«n: Thµnh ®« Hoa lÖ L©m An nµy sím muèn g× còng thuéc vÒ Kim quèc chóng ta th«i! Ha! Ha! Ha!",
	"VÖ binh Thµnh m«n: Th­îng ty cã lÖnh! Ph¸t hiÖn gian tÕ, tiÒn tr¶m hËu tÊu!",
	"VÖ binh Thµnh m«n: Ng­¬i m¾t l¾m la l¾m loĞt! Cã ph¶i gian tÕ kh«ng?",
	"VÖ binh Thµnh m«n: Bän ng­êi Kim kh«ng gièng ng­êi H¸n chóng ta! Chóng ch¼ng biÕt gİ ®Õn chuyÖn liªn sØ",
	},
};

function main(NpcIndex)
	if(nvthanhmon() == 1) then
	return end;
	local TALK_MORE = "T¸n gÉu/tangau";
	local END_TALK = "Rêi khái/no";
	Say("LINK:<npc>",0,10833,5,
		"Tham gia chiÕn tr­êng Tèng Kim/baodanh",
		"T×m hiÓu khu vùc luyÖn c«ng/map_help",
		"T×m hiÓu vâ nghÖ bæn m«n/skill_help",
		--"T×m hiÓu tinh hoa vâ häc c¸c m«n ph¸i/Uworld1000_knowmagic",
		TALK_MORE,
		END_TALK)
end;

function baodanh()
	if ( GetLevel() < 40 ) then
		Talk( 1, "", "ChiÕn tr­êng Tèng Kim gian khæ khèc liÖt, ng­¬i ch­a ®¹t ®Õn cÊp 40 h·y vÒ luyÖn thªm råi h·y tİnh." );
	return end;
	Say("<npc>: Trong Tèng Kim, bªn lîi thÕ vÒ sè ng­êi tuy cã chiÕm ­u thÕ nh­ng sÏ nhËn ®­îc ®iÓm tİch lòy İt h¬n, c¸c h¹ muèn chän b¸o danh bªn nµo?",3,
	"Vµo ®iÓm b¸o danh bªn Tèng/baodanhtong",
	"Vµo ®iÓm b¸o danh bªn Kim/baodanhkim",
	"Ch­a muèn ®i/no")
end;

function baodanhtong()
	if(GetCash() < 5000) then
	return end;
	if (NewWorld(325,floor(48928/32), floor(102272/32)) == 1) then--bao danh tong kim cao cap--phe Tong
		Pay(5000);
		Msg2Player( "§Õn n¬i b¸o danh ChiÕn Tr­êng Tèng Kim Cao CÊp" );
	end
end;

function baodanhkim()
	if(GetCash() < 5000) then
	return end;
	if (NewWorld(325,floor(50944/32), floor(98400/32)) == 1) then--bao danh tong kim cao cap--phe Kim
		Pay(5000);
		Msg2Player( "§Õn n¬i b¸o danh ChiÕn Tr­êng Tèng Kim Cao CÊp" );
	end
end;

function tangau()
	local W,X,Y = GetWorldPos();
	if(TANGAU[W]) then
		Talk(1,"",TANGAU[W][random(1,getn(TANGAU[W]))]);
	else
		Talk(1,"",DEFAULT_TALK[random(1,getn(DEFAULT_TALK))]);
	end;
end;

function nvthanhmon()
--	UTask_world42 = GetTask(42)
--	if ((UTask_world42 == 0) and (GetLevel() >= 80) and (GetReputeLevel(GetRepute()) >= 7)) then		-- ÈÎÎñÆô¶¯£¬ÉùİûµÈ¼¶ºİÉùİûÖµµÄ¹ØÏµ²»ÊÇºÜ·÷È·£¬ÒªÎÊ·÷°×
--		Talk(6,"nhannvthanhmon","³Ç·ÅÎµ±ø¼×£ºÌıËµ³Çµï×î½üµÏÓĞĞ¡º¢ÎŞ¹ÊÊ§×ï£¿","³Ç·ÅÎµ±øÒÒ£ºÊÇ°¡¡£ÎÒÔ­µ´µÄæ°İ·Ò¶»¨£¬Ëı¶şĞ¡×ÓÒ²²»¼û¸Ë£¬×î½ü³£µ´·³ÎÒ°ïËıÕÒ¶ù×Ó¡£","İæ¼Ò£º¸½Î»£¬¾¿¾¹Ôõ·´»ØÊ¢£¿","³Ç·ÅÎµ±ø¼×£ººÇºÇ£¬ÕâÎ»´óÏµÒÇ±íÌ·Ì·£¬Ó¢·÷ÉñÎä£¬¿´µ´ÊÇÏë¹Ü¹ÜÕâµµ×ÓÊ¢¿©£¿","İæ¼Ò£º¿İÆø£¬¢·¼û²»Æ½°Îµ¶ÏµÖúÊÇÎÒ±²±¾·Ö¡£","³Ç·ÅÎµ±øÒÒ£º°¥Ñ½£¬½ñİíµÄ¾ÆÇ®º·Ïó·»´ø¹»°¡£¡")
--		return 1;
--	end;
	return 0;
end;

function nhannvthanhmon()
	Say("§óng råi, lµ 5 v¹n l­îng.",2,"Hai vŞ ®¹i hiÖp thËt thó vŞ/yes1","G¹t ®­îc ng­êi kh¸c, lµm sao g¹t ®­îc ta!/no")
end;

function yes1()
	if (GetCash()>=50000) then 
		Pay(50000)
		Talk(1,"","C¸c h¹ thËt hµo phãng. Nhanh ®i D­îc v­¬ng cèc hái Qu¶i nh©n ch©n nh©n. H¾n biÕt bİ mËt nµy.")
		--SetTask(42, 10)
		Msg2Player("NhËn nhiÖm vô: §i D­îc V­¬ng Cèc t×m qu¶i ch©n ®¹o nh©n hái th¨m tin tøc ®øa trÎ.")
	else
		Talk(1,"","§i ®i, kh«ng cã tiÒn th× ®ßi lµm c¸i g×, trë vÒ lµm n«ng ®i!")
	end
end;

function no()
	Talk(1,"","§i ®i, kh«ng cã tiÒn th× ®ßi lµm c¸i g×, trë vÒ lµm n«ng ®i!")
end;

function no()
end;
