-- ================================================================================================
-- [HE THONG] script/global/equip_system.lua
-- Muc dich  : NPC THO REN - cua vao he lo ren (dung luyen, cuong hoa).
-- Duoc nap  : engine nap luc boot (moi tep .lua = 1 lua_State rieng)
-- Include   : compound_header.lua, atlas.lua
-- Ham (dong): OnFoundry (58), OnFoundry2 (67), OnQueryPiece (77), OnEpDoTim (89), OnEpDoPho (94), OnKhamNam (100), OnNangKhoang (106), OnHopHuyenTinh (112), OnTinhLuyen (118), OnHuyenKim (124), OnHuyenKimTinh (130), OnNhanEpDoTim (136), OnNhanEpDoPho (138), OnNhanKhamNam (139), OnNhanNangKhoang (140), OnNhanHopHuyenTinh (141), OnNhanTinhLuyen (142), OnNhanHuyenKim (143), OnNhanHuyenKimTinh (144), LoRenChay (153), OnCancel (229), no (232), OnAbout (239), OnAbout_2 (245)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
-- Chøc n¨ng: NPC Thî rÌn - cöa vµo cña c¶ hÖ lß rÌn.
-- DŞch ng­îc tõ  D:\ServerLinux\server1\script\global\equip_system.lua
--
-- Kh¸c b¶n gèc ®óng mét chç, vµ chØ ë phÇn vËn chuyÓn:
--   B¶n gèc gäi `FoundryItem()` ®Ó më cöa sæ lß rÌn 7 thÎ cña client, ng­êi
--   ch¬i kĞo ®å vµo c¸c « råi bÊm chÕ t¹o.
--   JX1 ch­a cã cöa sæ ®ã. Thay vµo ®ã dïng hép giao vËt phÈm - kªnh ®· cã
--   s½n vµ ®ang ch¹y thËt trªn m¸y chñ nµy. Ng­êi ch¬i bá nguyªn liÖu vµo hép,
--   bÊm x¸c nhËn, m¸y chñ gom danh s¸ch råi gäi th¼ng `FoundryCompound`.
--
-- Toµn bé luËt ch¬i (®iÒu kiÖn nguyªn liÖu, c«ng thøc tİnh, x¸c suÊt, s¶n
-- phÈm) vÉn n»m nguyªn trong c¸c kŞch b¶n cña b¶n gèc ë \script\item\compound\
-- vµ kh«ng bŞ ®ông tíi mét dßng nµo.
--
-- M· thao t¸c lÊy tõ \settings\item\compoundscript.txt:
--   0 hîp thµnh HuyÒn Tinh  |  1,2 n©ng cÊp kho¸ng  |  3 tinh luyÖn thuéc tİnh
--   4 ®óc trang bŞ tİm      |  5 kh¶m n¹m           |  6 §å phæ Hoµng Kim
--   7 HuyÒn Kim             |  8 HuyÒn Kim chi tinh

Include( "\\script\\item\\compound\\compound_header.lua" );
Include( "\\script\\item\\compound\\atlas.lua" );

CMP_XUANJING		= 0;
CMP_ORE_UPGRADE		= 1;
CMP_MAGIC_DISTILL	= 3;
CMP_EQUIP_COMPOUND	= 4;
CMP_EQUIP_ENCHASE	= 5;
CMP_ATLAS			= 6;
CMP_FANTASYGOLD		= 7;
CMP_FANTASYGOLD_ESS	= 8;

-- M¶nh thiªn th¹ch: chİn lo¹i, ë JX1 lµ 4/1317..1325 vµ kh«ng cã
-- ParticularType (b¶ng questkey cña JX1 thiÕu cét ®ã nªn engine ®ãng cøng 0).
-- ChØ riªng thao t¸c §å phæ míi t¸ch m¶nh nµy ra « nguyªn liÖu tuú chän.
TBMANH_THIENTHACH =
{
	["4,1317,0"] = 1, ["4,1318,0"] = 2, ["4,1319,0"] = 3,
	["4,1320,0"] = 4, ["4,1321,0"] = 5, ["4,1322,0"] = 6,
	["4,1323,0"] = 7, ["4,1324,0"] = 8, ["4,1325,0"] = 9,
}

-- Lêi b¸o cho tõng m· kÕt qu¶, khíp mét-mét víi FOUNDRY_RESULT_TYPE.
TBLOIBAO =
{
	"Thî rÌn: LÇn nµy kh«ng thµnh, nguyªn liÖu ®· hao hÕt råi. §õng n¶n, hiÖp sÜ thö l¹i xem!",
	"Thî rÌn: Ng­¬i kh«ng ®ñ tiÒn c«ng cho l·o phu.",
	"Thî rÌn: Nguyªn liÖu nµy ®· ë cÊp cao nhÊt, kh«ng n©ng thªm ®­îc n÷a.",
	"Thî rÌn: Nguyªn liÖu ch­a ®ñ, hoÆc ng­¬i bá nhÇm mãn.",
	"Thî rÌn: CÊp cña nguyªn liÖu kh«ng khíp nhau.",
	"Thî rÌn: Thuéc tİnh phĞp cña nguyªn liÖu kh«ng hîp.",
	"Thî rÌn: Ngò hµnh cña nguyªn liÖu kh«ng khíp.",
	"Thî rÌn: Nguyªn liÖu kh«ng ®óng phĞp chÕ t¹o.",
}

--------------------------------------------------------------------------------
-- Cöa vµo
--------------------------------------------------------------------------------
function OnFoundry()
	Say( "NÕu ®¹i hiÖp muèn cã mét mãn ®å võa ı th× ®em ®Çy ®ñ nguyªn liÖu tíi ®©y. Ta sÏ ®óc luyÖn mét vËt phÈm ng­¬i thİch nhÊt!", 6,
		"§óc trang bŞ tİm/OnEpDoTim",
		"GhĞp §å phæ Hoµng Kim/OnEpDoPho",
		"Kh¶m n¹m trang bŞ/OnKhamNam",
		"N©ng cÊp kho¸ng th¹ch/OnNangKhoang",
		"Xem thªm/OnFoundry2",
		"Ta sÏ quay l¹i sau./no" );
end

function OnFoundry2()
	Say( "Cßn mÊy nghÒ n÷a l·o phu còng biÕt chót İt.", 6,
		"Hîp thµnh HuyÒn Tinh/OnHopHuyenTinh",
		"Tinh luyÖn thuéc tİnh/OnTinhLuyen",
		"N©ng cÊp HuyÒn Kim/OnHuyenKim",
		"N©ng HuyÒn Kim chi tinh/OnHuyenKimTinh",
		"T×m hiÓu m¶nh Hoµng Kim/OnQueryPiece",
		"Quay l¹i/OnFoundry" );
end

function OnQueryPiece()
	QueryPiece();
end

--------------------------------------------------------------------------------
-- B¶y cöa thao t¸c. Mçi cöa më hép giao vËt phÈm víi lêi nh¾c riªng.
--------------------------------------------------------------------------------
-- Mçi thao t¸c cã hµm nhËn riªng, KH¤NG dïng biÕn toµn côc ®Ó nhí lo¹i.
-- Lı do: JX1 n¹p mét tr¹ng th¸i Lua cho mçi tÖp kŞch b¶n, dïng chung cho mäi
-- ng­êi ch¬i. Mét biÕn toµn côc ë ®©y lµ chung c¶ m¸y chñ - hai ng­êi cïng më
-- thî rÌn th× ng­êi sau ghi ®Ì lo¹i thao t¸c cña ng­êi tr­íc.
function OnEpDoTim()
	GiveItemUI( "§óc trang bŞ tİm",
		"Bá vµo mét trang bŞ mµu tr¾ng hoÆc xanh, kÌm kho¸ng th¹ch. Cµng nhiÒu kho¸ng th× cµng dÔ ra nhiÒu « kh¶m n¹m.",
		"OnNhanEpDoTim" );
end

function OnEpDoPho()
	GiveItemUI( "GhĞp §å phæ Hoµng Kim",
		"Bá vµo §å phæ Hoµng Kim, mét viªn HuyÒn Tinh Kho¸ng Th¹ch, c¸c nguyªn liÖu c«ng thøc ®ßi, vµ ®óng mét M¶nh thiªn th¹ch.",
		"OnNhanEpDoPho" );
end

function OnKhamNam()
	GiveItemUI( "Kh¶m n¹m trang bŞ",
		"Bá vµo mét trang bŞ tİm cßn « trèng, kÌm viªn kho¸ng thuéc tİnh khíp víi « ®ã.",
		"OnNhanKhamNam" );
end

function OnNangKhoang()
	GiveItemUI( "N©ng cÊp kho¸ng th¹ch",
		"Bá vµo ®óng ba viªn kho¸ng cïng lo¹i, cïng cÊp.",
		"OnNhanNangKhoang" );
end

function OnHopHuyenTinh()
	GiveItemUI( "Hîp thµnh HuyÒn Tinh",
		"Bá vµo trang søc mµu tr¾ng hoÆc xanh ®Ó luyÖn ra HuyÒn Tinh Kho¸ng Th¹ch.",
		"OnNhanHopHuyenTinh" );
end

function OnTinhLuyen()
	GiveItemUI( "Tinh luyÖn thuéc tİnh",
		"Bá vµo mét trang bŞ cã thuéc tİnh, kÌm nguyªn kho¸ng ®Ó rót thuéc tİnh ra thµnh kho¸ng.",
		"OnNhanTinhLuyen" );
end

function OnHuyenKim()
	GiveItemUI( "N©ng cÊp HuyÒn Kim",
		"Bá vµo HuyÒn Kim cïng nguyªn liÖu n©ng cÊp.",
		"OnNhanHuyenKim" );
end

function OnHuyenKimTinh()
	GiveItemUI( "N©ng cÊp HuyÒn Kim Chi Tinh",
		"Bá vµo HuyÒn Kim Chi Tinh cïng nguyªn liÖu n©ng cÊp.",
		"OnNhanHuyenKimTinh" );
end

function OnNhanEpDoTim( nCount )      LoRenChay( CMP_EQUIP_COMPOUND,  nCount ) end
function OnNhanEpDoPho( nCount )      LoRenChay( CMP_ATLAS,           nCount ) end
function OnNhanKhamNam( nCount )      LoRenChay( CMP_EQUIP_ENCHASE,   nCount ) end
function OnNhanNangKhoang( nCount )   LoRenChay( CMP_ORE_UPGRADE,     nCount ) end
function OnNhanHopHuyenTinh( nCount ) LoRenChay( CMP_XUANJING,        nCount ) end
function OnNhanTinhLuyen( nCount )    LoRenChay( CMP_MAGIC_DISTILL,   nCount ) end
function OnNhanHuyenKim( nCount )     LoRenChay( CMP_FANTASYGOLD,     nCount ) end
function OnNhanHuyenKimTinh( nCount ) LoRenChay( CMP_FANTASYGOLD_ESS, nCount ) end

--------------------------------------------------------------------------------
-- NhËn nguyªn liÖu tõ hép giao vËt phÈm råi ch¹y lß rÌn.
--
-- L­u ı vÒ hép giao: mét chång N ®¬n vŞ ®­îc tr¶i thµnh N môc cïng trá vÒ mét
-- chØ sè vËt phÈm. Phİa C++ ®· khö trïng lÆp nªn chång chØ tİnh mét lÇn - vµ
-- ®ã míi ®óng, v× gi¸ trŞ vËt phÈm vèn ®· nh©n theo c¶ chång.
--------------------------------------------------------------------------------
function LoRenChay( nType, nCount )
	if( nCount == nil or nCount < 1 ) then
		Say( "Thî rÌn: Ng­¬i ch­a bá nguyªn liÖu nµo vµo c¶.", 1, "Quay l¹i/OnFoundry" );
		EndGiveBox();	-- dong hop giao vat pham
		return
	end

	local aryNec = {};
	local aryAlt = {};
	local nNec = 0;
	local nAlt = 0;

	local daGom = {};
	for i = 1, nCount do
		local nIdx = GetGiveItemUnit( i );
		-- Hop giao TRAI mot chong N don vi thanh N muc cung mot chi so.
		-- Phai khu trung lap NGAY O DAY: neu de nguyen thi mot chong 50
		-- vien chiem het tran o nguyen lieu, va mon trang bi bo cung khong
		-- con cho de di xuong kich ban.
		local bDaCo = 0;
		if( nIdx ~= nil and nIdx > 0 ) then
			for k = 1, getn( daGom ) do
				if( daGom[k] == nIdx ) then
					bDaCo = 1;
				end
			end
		end
		if( nIdx ~= nil and nIdx > 0 and bDaCo == 0 ) then
			daGom[getn( daGom ) + 1] = nIdx;
			local bLaManh = 0;
			-- ChØ thao t¸c §å phæ míi t¸ch M¶nh thiªn th¹ch ra khay tuú chän.
			-- Lua 4 kh«ng cã kiÓu luËn lı: phĞp so s¸nh tr¶ vÒ 1 hoÆc nil, vµ
			-- `true` chØ lµ mét biÕn ch­a g¸n, tøc b»ng nil. Nªn ph¶i so b»ng
			-- sè, tuyÖt ®èi kh«ng viÕt `x == true`.
			if( nType == CMP_ATLAS ) then
				local nGenre, nDetailType, nParticular = GetItemProp( nIdx );
				local szKey = format( "%d,%d,%d", nGenre, nDetailType, nParticular );
				if( TBMANH_THIENTHACH[szKey] ~= nil ) then
					bLaManh = 1;
				end
			end
			if( bLaManh == 1 ) then
				nAlt = nAlt + 1;
				aryAlt[nAlt] = nIdx;
			else
				nNec = nNec + 1;
				aryNec[nNec] = nIdx;
			end
		end
	end

	if( nNec < 1 ) then
		Say( "Thî rÌn: Ng­¬i ch­a bá nguyªn liÖu chİnh nµo vµo c¶.", 1, "Quay l¹i/OnFoundry" );
		EndGiveBox();	-- dong hop giao vat pham
		return
	end

	local nItemIdx, nResult = FoundryCompound( nType, aryNec, aryAlt, 0 );

	if( nResult == RESULT_SUCCEED and nItemIdx > 0 ) then
		Say( format( "Thî rÌn: Xong! Ng­¬i nhËn ®­îc <color=yellow>%s<color>.", GetItemName( nItemIdx ) ),
			2, "Lµm tiÕp/OnFoundry", "Ta ®i ®©y./no" );
		EndGiveBox();	-- dong hop giao vat pham
		return
	end

	-- Bang xep 1..8 khop THANG voi FOUNDRY_RESULT_FAIL=1 den RULE_ERROR=8
	-- (KItemCompound.h). Cong them 1 la lech mot bac: that bai thi bao
	-- "khong du tien".
	local szLoi = TBLOIBAO[nResult];
	if( szLoi == nil ) then
		szLoi = "Thî rÌn: L·o phu ch­a nghÜ ra c¸ch chÕ mãn nµy.";
	end
	Say( szLoi, 2, "Thö l¹i/OnFoundry", "Ta ®i ®©y./no" );
	EndGiveBox();	-- dong hop giao vat pham
end

function OnCancel()
end

function no()
end

--------------------------------------------------------------------------------
-- Lêi giíi thiÖu, gi÷ nguyªn v¨n b¶n cña b¶n gèc.
--------------------------------------------------------------------------------
function OnAbout()
	Talk( 3, "OnAbout_2",
	"Sö dông c¸c nguyªn liÖu ®Æc biÖt nh­ Kho¸ng Th¹ch cã thuéc tİnh, HuyÒn Tinh Kho¸ng Th¹ch, Trang bŞ HuyÒn Tinh, §å phæ Hoµng Kim cã thÓ chÕ t¹o ®­îc Trang bŞ HuyÒn Tinh hoÆc trang bŞ Hoµng Kim. §óc luyÖn trang bŞ ®­îc ph©n ra: tinh luyÖn, chÕ t¹o, n©ng cÊp, kh¶m n¹m vµ ®å phæ 5 bé phËn nµy. NÕu cã ®­îc c¸c thø nguyªn liÖu nµy th× cã thÓ ®Õn chç ta nhê ®óc luyÖn.",
	"<color=yellow>HuyÒn Tinh Kho¸ng Th¹ch<color>: Lµ mét lo¹i kho¸ng th¹ch ®Æc biÖt, thø nguyªn liÖu dïng ®Ó n©ng cÊp, chÕ t¹o, kh¶m n¹m, ®Ó n©ng cao chÊt l­îng thuéc tİnh trong qu¸ tr×nh ®óc luyÖn hoÆc tİnh thµnh c«ng cao. Cã tÊt c¶ 10 cÊp ®é, ®¼ng cÊp kho¸ng cµng cao th× hiÖu qu¶ cho ra cµng lín.",
	"<color=yellow>Kho¸ng th¹ch cã thuéc tİnh<color>: Ta ®em kho¸ng cã thuéc tİnh n©ng cÊp t­¬ng øng lªn trang bŞ mµu xanh cã thuéc tİnh sÏ cã c¬ héi nhËn ®­îc thuéc tİnh cña kho¸ng th¹ch ®ã. NÕu sö dông kho¸ng mang thuéc tİnh Èn th× ngò hµnh thuéc tİnh cña trang bŞ mµu xanh ph¶i gièng víi thuéc tİnh Èn cña kho¸ng. Kho¸ng th¹ch cã thuéc tİnh cã tíi 10 cÊp ®é, ®¼ng cÊp cµng cao khi kh¶m n¹m cho ra ®iÓm thuéc tİnh cµng cao." );
end

function OnAbout_2()
	Talk( 2, "OnCancel",
	"<color=yellow>Trang bŞ HuyÒn Tinh<color>: Dïng 1 trang bŞ mµu xanh hoÆc mµu tr¾ng kÕt hîp víi HuyÒn Tinh Kho¸ng Th¹ch chÕ t¹o sÏ cã c¬ héi nhËn ®­îc 1 Trang bŞ HuyÒn Tinh tõ 1~5 lÇn c¬ héi kh¶m n¹m. C¸c hµng thuéc tİnh kh¸c cña trang bŞ sÏ gièng víi nguyªn liÖu trang bŞ ®­îc sö dông.",
	"<color=yellow>§å phæ Hoµng Kim<color>: Mçi mét bé trang bŞ Hoµng Kim ®Òu cã nh÷ng ®å phæ Hoµng Kim t­¬ng øng, theo nh÷ng g× ghi l¹i trªn ®å phæ Hoµng Kim cã ®­îc b¹n sÏ t×m ®­îc c¸c mãn cßn l¹i kÕt hîp chång thµnh bé trang bŞ Hoµng Kim." );
end
