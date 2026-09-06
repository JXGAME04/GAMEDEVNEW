-- Bé test hÖ lß rÌn cho lÖnh bµi admin.
-- G¾n vµo  \script\item\lenhbaiadmin.lua  b»ng mét dßng Include.
--
-- Môc ®Ých: kiÓm ®­îc tõng m¾t xÝch mµ kh«ng ph¶i mß trong game -
--   1. b¶ng d÷ liÖu cã n¹p ®ñ kh«ng (®©y lµ chç háng ©m thÇm hay gÆp nhÊt)
--   2. ph¸t ®óng bé nguyªn liÖu cña mét c«ng thøc cã thËt trong b¶ng
--   3. soi chi tiÕt mét vËt phÈm: phÈm chÊt, phiªn b¶n, h¹t, « kh¶m n¹m, gi¸ trÞ
--   4. ch¹y th¼ng tõng thao t¸c lß rÌn
--
-- L­u ý khi söa tÖp nµy:
--   * mçi b¶ng chän chØ vÏ ®­îc tèi ®a 6 nót
--   * cÊm dÊu g¹ch chÐo trong nh·n nót (nã lµ dÊu t¸ch nh·n víi tªn hµm)
--   * tr¸nh viÕt hoa ch÷ cã dÊu (b¶ng m· mét byte cña game kh«ng m· ho¸ ®­îc)

-- CO Y khong Include equip_system.lua hay atlas.lua: atlas.lua chay
-- initAtlasConfig() ngay luc nap, neu no hong thi keo sap ca lenh bai admin -
-- ma lenh bai la thu dang chay that. Tep nay tu lo lay vai ham nho.

-- [27/08] cua so lo ren that (7 khung): chi dinh nghia ham, khong chay gi luc nap
Include( "\\script\\item\\compound\\compound_ui.lua" );

FILE_ATLAS_TEST = "atlas_compound.txt";

LR_RESULT_SUCCEED = 0;

-- ban sao cua item_header.lua, de khong phai keo ca chuoi Include vao day
function LR_DuongBang( nItemVer, strFileName )
	return format( "\\settings\\item\\%03d\\%s", nItemVer, strFileName );
end

-- Bay thao tac lo ren, ma lay tu \settings\item\compoundscript.txt
LR_CMP_XUANJING		= 0;
LR_CMP_ORE_UPGRADE	= 1;
LR_CMP_MAGIC_DISTILL	= 3;
LR_CMP_EQUIP_COMPOUND	= 4;
LR_CMP_EQUIP_ENCHASE	= 5;
LR_CMP_ATLAS		= 6;
LR_CMP_FANTASYGOLD	= 7;
LR_CMP_FANTASYGOLD_ESS	= 8;

LR_TBMANH =
{
	["4,1317,0"] = 1, ["4,1318,0"] = 2, ["4,1319,0"] = 3,
	["4,1320,0"] = 4, ["4,1321,0"] = 5, ["4,1322,0"] = 6,
	["4,1323,0"] = 7, ["4,1324,0"] = 8, ["4,1325,0"] = 9,
}

LR_TBLOIBAO =
{
	"Lo ren that bai, nguyen lieu da hao het.",
	"Khong du tien cong.",
	"Nguyen lieu da o cap cao nhat.",
	"Thieu nguyen lieu hoac bo nham mon.",
	"Cap cua nguyen lieu khong khop nhau.",
	"Thuoc tinh phep cua nguyen lieu khong hop.",
	"Ngu hanh cua nguyen lieu khong khop.",
	"Nguyen lieu khong dung phep che tao.",
}

-- S¸u lo¹i kho¸ng thuéc tÝnh cña JX1 vµ « kh¶m n¹m t­¬ng øng.
-- NGUYEN KHOANG (rong) cua JX1 - Linux 149..154 lech -1 sang 148..153.
-- Cot 3: 1 = o AN (pos chan, bat buoc cung ngu hanh -> phat du 5 he)
--        0 = o HIEN (pos le, khong rang buoc -> 1 vien)
TBNGUYENKHOANG =
{
	{ 148, "Nguyªn Kho¸ng hiÖn 1", 0 },
	{ 149, "Nguyªn Th¹ch Èn 1", 1 },
	{ 150, "Nguyªn Kho¸ng hiÖn 2", 0 },
	{ 151, "Nguyªn Th¹ch Èn 2", 1 },
	{ 152, "Nguyªn Kho¸ng hiÖn 3", 0 },
	{ 153, "Nguyªn Th¹ch Èn 3", 1 },
}

TBKHOANG_TEST =
{
	{ 199, "HuyÒn ThiÕt kho¸ng", 126 },
	{ 200, "Khæng T­íc Th¹ch",   166 },
	{ 201, "MËt Ng©n kho¸ng",    115 },
	{ 202, "Phï Dung Th¹ch",     117 },
	{ 203, "Chu Sa kho¸ng",      113 },
	{ 204, "Chung Nhò th¹ch",    139 },
}

--------------------------------------------------------------------------------
function LR_Root()
	SayEx({ "Bé test hÖ lß rÌn. Chän viÖc cÇn kiÓm:",
		"KiÓm b¶ng d÷ liÖu/LR_KiemBang",
		"LÊy nguyªn liÖu chung/LR_LayChung",
		"LÊy bé §å phæ (ra nguyªn mãn)/LR_LayKhoangCap",
		"LÊy trän bé mét c«ng thøc §å phæ/LR_LayBoDoPho",
		"Soi chi tiÕt mét vËt phÈm/LR_XemVatPham",
		"Më thî rÌn ngay t¹i ®©y/LR_MoThoRen",
		"Dän s¹ch kho¸ng lçi trong tói/LR_DonKhoangLoi",
		"Quay l¹i/chucnangadmin" })
end

--------------------------------------------------------------------------------
-- 1. B¶ng d÷ liÖu cã n¹p ®ñ kh«ng.
--    §©y lµ chç háng ©m thÇm: thiÕu mét tÖp lµ kÞch b¶n háng ngay lóc n¹p vµ
--    bÞ lo¹i khái c©y, kh«ng mét lêi b¸o nµo.
--------------------------------------------------------------------------------
function LR_KiemBang()
	local nVer = ITEM_GetLatestItemVersion();
	local szMsg = format( "Phiªn b¶n vËt phÈm míi nhÊt: <color=yellow>%d<color>\n", nVer );

	local nTong = 0;
	local nHong = 0;
	for i = 0, nVer do
		local aryTep =
		{
			"atlas_compound.txt",
			"magicattriblevel.txt",
			"magicattriblevel_index.txt",
			"itemvalue\\ore.txt",
			"itemvalue\\equip_normal_magic.txt",
			"itemvalue\\equip_enchasable_socket.txt",
			"itemvalue\\equip_enchasable_level.txt",
			"itemvalue\\equip_enchasable_type.txt",
			"itemvalue\\equip_enchasable_magic_pos.txt",
			"itemvalue\\magicattrib_combine.txt",
			"itemvalue\\equip_gold.txt",
		}
		for j = 1, getn( aryTep ) do
			local szPath = LR_DuongBang( i, aryTep[j] );
			TabFile_Load( szPath, szPath );
			local nRow = TabFile_GetRowCount( szPath );
			nTong = nTong + 1;
			if( nRow == nil or nRow < 2 ) then
				nHong = nHong + 1;
				szMsg = szMsg..format( "<color=red>thiÕu<color> %s\n", aryTep[j] );
			end
		end
	end

	if( nHong == 0 ) then
		szMsg = szMsg..format( "<color=green>§ñ c¶ %d b¶ng.<color>\n", nTong );
	else
		szMsg = szMsg..format( "<color=red>Háng %d trong %d b¶ng.<color>\n", nHong, nTong );
	end

	-- Thö tÝnh gi¸ trÞ mét viªn kho¸ng cÊp 8: nÕu ra 0 th× ®­êng tra gi¸ trÞ
	-- (itemvaluescript.txt -> ore.lua -> itemvalue\ore.txt) ®ang ®øt.
	local nGiaTri = ITEM_CalcItemValue( nVer, 0, 6, 1, 199, 8, 2, 0, nil, nil, "TEST" );
	szMsg = szMsg..format( "Gi¸ trÞ thö cña HuyÒn ThiÕt kho¸ng cÊp 8: <color=yellow>%0.0f<color>\n", nGiaTri );
	if( nGiaTri <= 0 ) then
		szMsg = szMsg.."<color=red>§­êng tra gi¸ trÞ ®ang ®øt. KiÓm itemvaluescript.txt.<color>\n";
	end

	SayEx({ szMsg, "Quay l¹i/LR_Root", "KÕt thóc/no" })
end

--------------------------------------------------------------------------------
-- 2. Nguyªn liÖu chung: HuyÒn Tinh vµ s¸u lo¹i kho¸ng, ®ñ cÊp ®Ó thö.
--------------------------------------------------------------------------------
function LR_LayChung()
	local nVer = ITEM_GetLatestItemVersion();
	local nDem = 0;

	-- HuyÒn Tinh Kho¸ng Th¹ch cÊp 1 tíi 10
	for lv = 1, 10 do
		if( AddItemEx( nVer, 0, 0, 6, 1, 146, lv, 0, 0, 0 ) > 0 ) then
			nDem = nDem + 1;
		end
	end

	-- NGUYEN KHOANG (rong) - chep dung cach ban goc phat:
	--   trangbitim.lua:136-157 (NPC thu nghiem cua ban Linux)
	-- O HIEN (pos le) khong rang buoc ngu hanh -> chi 1 vien.
	-- O AN  (pos chan) BAT BUOC cung ngu hanh voi trang bi
	--   (equip_enchase.lua:67) -> phat du 5 he de co ma chon.
	-- Ban goc dung nLevel = 1, va KHONG truyen o phep nao (da rong).
	-- KHONG phat day 199..204 (khoang DA mang phep): ban goc khong bao gio
	-- phat truc tiep, chung chi sinh ra qua the Trich lay / gop 3 vien.
	for i = 1, getn( TBNGUYENKHOANG ) do
		local nPtc = TBNGUYENKHOANG[i][1];
		if( TBNGUYENKHOANG[i][3] == 1 ) then
			for he = 0, 4 do
				if( AddItemEx( nVer, 0, 0, 6, 1, nPtc, 1, he, 0, 0 ) > 0 ) then
					nDem = nDem + 1;
				end
			end
		else
			if( AddItemEx( nVer, 0, 0, 6, 1, nPtc, 1, 0, 0, 0 ) > 0 ) then
				nDem = nDem + 1;
			end
		end
	end

	SayEx({ format( "§· ph¸t <color=yellow>%d<color> mãn nguyªn liÖu.\nGåm HuyÒn Tinh cÊp 1 tíi 10 vµ nguyªn kho¸ng rçng: « hiÖn 1 viªn, « Èn ®ñ 5 ngò hµnh.", nDem ),
		"LÊy tiÕp/LR_LayChung", "Quay l¹i/LR_Root", "KÕt thóc/no" })
end

--------------------------------------------------------------------------------
-- 3. LÊy trän bé mét c«ng thøc §å phæ cã thËt trong b¶ng.
--    QuÐt b¶ng t×m dßng ®Çu tiªn cßn sèng (cét DES_PIECE tõ 1 tíi 9), råi ph¸t
--    ®óng cuèn §å phæ, mét viªn HuyÒn Tinh, c¸c nguyªn liÖu c«ng thøc ®ßi vµ
--    ®óng mét M¶nh thiªn th¹ch t­¬ng øng.
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
-- [LOREN 28/08] Lay khoang THUOC TINH du cap.
-- LR_LayChung CO Y khong phat dai 199..204 (ban goc chi sinh chung qua the
-- Trich lay), nen khi test Do pho se thieu khoang dung cap. Muc nay phat
-- rieng de test: 6 loai x 4 cap (5,6,7,8).
--------------------------------------------------------------------------------
function LR_LayKhoangCap()
	-- [LOREN 28/08] Phat DUNG BO cho cong thuc RA NGUYEN MON (DES_PIECE = 0).
	-- Luat kiem o atlas.lua:verifyAtlasMaterials doi DU CA BA: dung ptc,
	-- cap >= cap trong bang, va DUNG MagicID. Menu cu phat MagicID = 0 nen
	-- khong bao gio khop - do la ly do bao thieu nguyen lieu.
	local nVer = ITEM_GetLatestItemVersion();
	local szPath = LR_DuongBang( nVer, FILE_ATLAS_TEST );
	TabFile_Load( szPath, szPath );
	local nRowCount = TabFile_GetRowCount( szPath );
	local nDong = 0;
	local j;
	for j = 2, nRowCount do
		local nPiece = tonumber( TabFile_GetCell( szPath, j, "DES_PIECE", -1 ) );
		local ag = tonumber( TabFile_GetCell( szPath, j, "ATLAS_GENRE", -1 ) );
		if( nPiece == 0 and ag >= 0 ) then
			nDong = j;
			break;
		end
	end
	if( nDong == 0 ) then
		SayEx({ "<color=red>Kh«ng t×m thÊy c«ng thøc ra nguyªn mãn.<color>", "Quay l¹i/LR_Root" })
		return
	end

	local szMsg = format( "C«ng thøc dßng <color=yellow>%d<color> - ra nguyªn mãn:\n", nDong );
	local nDem = 0;
	-- cuon Do pho
	local ag = tonumber( TabFile_GetCell( szPath, nDong, "ATLAS_GENRE", -1 ) );
	local ad = tonumber( TabFile_GetCell( szPath, nDong, "ATLAS_DETAILTYPE", -1 ) );
	local ap = tonumber( TabFile_GetCell( szPath, nDong, "ATLAS_PARTICULAR", -1 ) );
	if( AddItemEx( nVer, 0, 0, ag, ad, ap, 1, 0, 0, 0 ) > 0 ) then
		nDem = nDem + 1;
		szMsg = szMsg..format( "  §å phæ: %s\n", TabFile_GetCell( szPath, nDong, "ATLAS_NAME", "" ) );
	end
	-- mot vien Huyen Tinh
	if( AddItemEx( nVer, 0, 0, 6, 1, 146, 5, 0, 0, 0 ) > 0 ) then
		nDem = nDem + 1;
		szMsg = szMsg.."  HuyÒn Tinh Kho¸ng Th¹ch cÊp 5\n";
	end
	-- sau o nguyen lieu: DUNG cap va DUNG MagicID
	local k;
	for k = 1, 6 do
		local g = tonumber( TabFile_GetCell( szPath, nDong, k.."_GENRE", -1 ) );
		if( g >= 0 ) then
			local d = tonumber( TabFile_GetCell( szPath, nDong, k.."_DETAILTYPE", -1 ) );
			local p = tonumber( TabFile_GetCell( szPath, nDong, k.."_PARTICULAR", -1 ) );
			local lv = tonumber( TabFile_GetCell( szPath, nDong, k.."_LEVEL", -1 ) );
			local se = tonumber( TabFile_GetCell( szPath, nDong, k.."_SERIES", -1 ) );
			local mg = tonumber( TabFile_GetCell( szPath, nDong, k.."_MAGIC_ID", -1 ) );
			if( d < 0 ) then d = 1; end
			if( p < 0 ) then p = 0; end
			if( lv < 0 ) then lv = 1; end
			if( se < 0 ) then se = 0; end
			if( mg < 0 ) then mg = 0; end
			if( AddItemEx( nVer, 0, 0, g, d, p, lv, se, 0, mg ) > 0 ) then
				nDem = nDem + 1;
				szMsg = szMsg..format( "  %s cÊp %d (phÐp %d)\n", TabFile_GetCell( szPath, nDong, k.."_NAME", "" ), lv, mg );
			end
		end
	end
	szMsg = szMsg..format( "\n§· ph¸t <color=yellow>%d<color> mãn. Bá hÕt vµo lß rÌn lµ ghÐp ®­îc.", nDem );
	SayEx({ szMsg, "Më thî rÌn/LR_MoThoRen", "LÊy l¹i/LR_LayKhoangCap", "Quay l¹i/LR_Root" })
end

function LR_LayBoDoPho()
	local nVer = ITEM_GetLatestItemVersion();
	local szPath = LR_DuongBang( nVer, FILE_ATLAS_TEST );
	TabFile_Load( szPath, szPath );
	local nRowCount = TabFile_GetRowCount( szPath );
	if( nRowCount == nil or nRowCount < 2 ) then
		SayEx({ "<color=red>Ch­a n¹p ®­îc atlas_compound.txt.<color>", "Quay l¹i/LR_Root" })
		return
	end

	local nDong = 0;
	for j = 2, nRowCount do
		local nPiece = tonumber( TabFile_GetCell( szPath, j, "DES_PIECE", -1 ) );
		local nGenre = tonumber( TabFile_GetCell( szPath, j, "ATLAS_GENRE", -1 ) );
		-- [LOREN 28/08] DOI 1..9 -> == 0: isAtlas (atlas.lua:110) LAY DONG DAU
		-- TIEN khop ma do pho, tuc voi moi do pho chi cong thuc o dong dau
		-- (ra nguyen mon, DES_PIECE=0) la duoc kiem that. Bo MANH (1..9) o cac
		-- dong sau khong bao gio toi luot -> phat bo do la luon 'thieu nguyen
		-- lieu' (cap thap hon yeu cau dong dau).
		if( nPiece == 0 and nGenre >= 0 ) then
			nDong = j;
			break;
		end
	end
	if( nDong == 0 ) then
		SayEx({ "<color=red>Kh«ng t×m thÊy c«ng thøc nµo cßn sèng trong b¶ng.<color>", "Quay l¹i/LR_Root" })
		return
	end

	local szMsg = format( "C«ng thøc ë dßng <color=yellow>%d<color> cña b¶ng:\n", nDong );
	local nDem = 0;

	-- cuèn §å phæ
	local ag = tonumber( TabFile_GetCell( szPath, nDong, "ATLAS_GENRE", -1 ) );
	local ad = tonumber( TabFile_GetCell( szPath, nDong, "ATLAS_DETAILTYPE", -1 ) );
	local ap = tonumber( TabFile_GetCell( szPath, nDong, "ATLAS_PARTICULAR", -1 ) );
	if( AddItemEx( nVer, 0, 0, ag, ad, ap, 1, 0, 0, 0 ) > 0 ) then
		nDem = nDem + 1;
		szMsg = szMsg..format( "  §å phæ %s\n", TabFile_GetCell( szPath, nDong, "ATLAS_NAME", "" ) );
	end

	-- mét viªn HuyÒn Tinh
	if( AddItemEx( nVer, 0, 0, 6, 1, 146, 5, 0, 0, 0 ) > 0 ) then
		nDem = nDem + 1;
		szMsg = szMsg.."  HuyÒn Tinh Kho¸ng Th¹ch cÊp 5\n";
	end

	-- s¸u « nguyªn liÖu
	for k = 1, 6 do
		local g = tonumber( TabFile_GetCell( szPath, nDong, k.."_GENRE", -1 ) );
		if( g >= 0 ) then
			local d = tonumber( TabFile_GetCell( szPath, nDong, k.."_DETAILTYPE", -1 ) );
			local p = tonumber( TabFile_GetCell( szPath, nDong, k.."_PARTICULAR", -1 ) );
			local lv = tonumber( TabFile_GetCell( szPath, nDong, k.."_LEVEL", -1 ) );
			local se = tonumber( TabFile_GetCell( szPath, nDong, k.."_SERIES", -1 ) );
			local mg = tonumber( TabFile_GetCell( szPath, nDong, k.."_MAGIC_ID", -1 ) );
			if( d < 0 ) then d = 1; end
			if( p < 0 ) then p = 0; end
			if( lv < 0 ) then lv = 1; end
			if( se < 0 ) then se = 0; end
			if( mg < 0 ) then mg = 0; end
			if( AddItemEx( nVer, 0, 0, g, d, p, lv, se, 0, mg ) > 0 ) then
				nDem = nDem + 1;
				szMsg = szMsg..format( "  %s\n", TabFile_GetCell( szPath, nDong, k.."_NAME", "" ) );
			end
		end
	end

	-- m¶nh thiªn th¹ch ®óng sè hiÖu
	local nPiece = tonumber( TabFile_GetCell( szPath, nDong, "DES_PIECE", -1 ) );
	-- [LOREN 28/08] piece 0 thi 1316+0 = detail 1316 khong ton tai - chi phat
	-- manh khi cong thuc that su ra manh.
	if( nPiece >= 1 and AddItemEx( nVer, 0, 0, 4, 1316 + nPiece, 0, 1, 0, 0, 0 ) > 0 ) then
		nDem = nDem + 1;
		szMsg = szMsg..format( "  M¶nh thiªn th¹ch sè %d\n", nPiece );
	end

	szMsg = szMsg..format( "\n§· ph¸t <color=yellow>%d<color> mãn. Më thî rÌn råi bá hÕt vµo lµ ghÐp ®­îc.", nDem );
	SayEx({ szMsg, "Më thî rÌn/LR_MoThoRen", "LÊy bé kh¸c/LR_LayBoDoPho", "Quay l¹i/LR_Root" })
end

--------------------------------------------------------------------------------
-- 4. Soi chi tiÕt mét vËt phÈm.
--------------------------------------------------------------------------------
function LR_XemVatPham()
	GiveItemUI( "Soi vËt phÈm", "Bá vµo mãn cÇn soi.", "LR_XemVatPhamNhan" );
end

function LR_XemVatPhamNhan( nCount )
	if( nCount == nil or nCount < 1 ) then
		SayEx({ "Ch­a bá mãn nµo vµo.", "Quay l¹i/LR_Root" })
		return
	end
	local nIdx = GetGiveItemUnit( 1 );
	if( nIdx == nil or nIdx <= 0 ) then
		SayEx({ "Kh«ng ®äc ®­îc vËt phÈm.", "Quay l¹i/LR_Root" })
		return
	end

	local nGenre, nDetailType, nParticular, nLevel, nSeries, nLuck = GetItemProp( nIdx );
	local nQuality = GetItemQuality( nIdx );
	local nItemVer = ITEM_GetItemVersion( nIdx );
	local nSeed = ITEM_GetItemRandSeed( nIdx );
	local aryMag = GetItemAllParams( nIdx );
	local nStack = GetItemStackCount( nIdx );

	local szPham = "th­êng";
	if( nQuality == 1 ) then szPham = "hoµng kim"; end
	if( nQuality == 2 ) then szPham = "kh¶m n¹m ®­îc (®å tÝm)"; end
	if( nQuality == 4 ) then szPham = "b¹ch kim"; end

	local szMsg = format( "<color=yellow>%s<color>\n", GetItemName( nIdx ) );
	szMsg = szMsg..format( "m·: %d,%d,%d   cÊp %d   ngò hµnh %d   may m¾n %d\n",
		nGenre, nDetailType, nParticular, nLevel, nSeries, nLuck );
	szMsg = szMsg..format( "phÈm chÊt: %d (%s)   phiªn b¶n: %d   chång: %d\n",
		nQuality, szPham, nItemVer, nStack );
	szMsg = szMsg..format( "h¹t ngÉu nhiªn: %0.0f\n", nSeed );

	local szO = "";
	local nORong = 0;
	for i = 1, 6 do
		local v = aryMag[i];
		if( v == nil ) then v = 0; end
		if( v == -1 ) then
			szO = szO.."[trèng]";
			nORong = nORong + 1;
		elseif( v == 0 ) then
			szO = szO.."[-]";
		else
			szO = szO..format( "[%d]", v );
		end
	end
	szMsg = szMsg..format( "« kh¶m n¹m: %s  (cßn trèng %d)\n", szO, nORong );

	local nGiaTri = ITEM_CalcItemValue( nIdx, "TEST" );
	szMsg = szMsg..format( "gi¸ trÞ: <color=yellow>%0.0f<color>", nGiaTri );

	SayEx({ szMsg, "Soi mãn kh¸c/LR_XemVatPham", "Quay l¹i/LR_Root", "KÕt thóc/no" })
end

--------------------------------------------------------------------------------
-- 5. Ch¹y th¼ng tõng thao t¸c lß rÌn, khái ph¶i ®i t×m NPC thî rÌn.
--------------------------------------------------------------------------------
function LR_MoThoRen()
	SayEx({ "Chän thao t¸c cÇn ch¹y:",
		"Më cöa sæ lß rÌn thËt/LR_UI_Mo",
		"Soi « lß rÌn (xem mãn nµo kh«ng hîp luËt)/LR_UI_Soi",
		-- "§óc trang bÞ tÝm/LR_ChayEpTim",
		-- "GhÐp §å phæ Hoµng Kim/LR_ChayDoPho",
		-- "Kh¶m n¹m trang bÞ/LR_ChayKhamNam",
		-- "N©ng cÊp kho¸ng th¹ch/LR_ChayNangKhoang",
		-- "Thao t¸c kh¸c/LR_MoThoRen2",
		"Quay l¹i/LR_Root" })
end

function LR_MoThoRen2()
	SayEx({ "Bèn thao t¸c cßn l¹i:",
		"Hîp thµnh HuyÒn Tinh/LR_ChayHuyenTinh",
		"Tinh luyÖn thuéc tÝnh/LR_ChayTinhLuyen",
		"N©ng cÊp HuyÒn Kim/LR_ChayHuyenKim",
		"N©ng HuyÒn Kim chi tinh/LR_ChayHKTinh",
		"Quay l¹i/LR_MoThoRen" })
end

-- Mçi thao t¸c mét hµm nhËn riªng. KH¤NG dïng biÕn toµn côc ®Ó nhí lo¹i:
-- JX1 n¹p mét tr¹ng th¸i Lua cho mçi tÖp kÞch b¶n, dïng chung cho mäi ng­êi
-- ch¬i, nªn biÕn toµn côc ë ®©y lµ chung c¶ m¸y chñ.
function LR_Mo( szNhac, szNhan )
	GiveItemUI( "Lß rÌn: bá nguyªn liÖu vµo", szNhac, szNhan );
end

function LR_ChayEpTim()      LR_Mo( "Mét trang bÞ tr¾ng hoÆc xanh, kÌm kho¸ng th¹ch.", "LR_NhanEpTim" ) end
function LR_ChayDoPho()      LR_Mo( "§å phæ, mét HuyÒn Tinh, nguyªn liÖu c«ng thøc, vµ ®óng mét M¶nh thiªn th¹ch.", "LR_NhanDoPho" ) end
function LR_ChayKhamNam()    LR_Mo( "Mét trang bÞ tÝm cßn « trèng, kÌm kho¸ng thuéc tÝnh khíp « ®ã.", "LR_NhanKhamNam" ) end
function LR_ChayNangKhoang() LR_Mo( "§óng ba viªn kho¸ng cïng lo¹i, cïng cÊp.", "LR_NhanNangKhoang" ) end
function LR_ChayHuyenTinh()  LR_Mo( "Trang søc tr¾ng hoÆc xanh.", "LR_NhanHuyenTinh" ) end
function LR_ChayTinhLuyen()  LR_Mo( "Mét trang bÞ cã thuéc tÝnh, kÌm nguyªn kho¸ng.", "LR_NhanTinhLuyen" ) end
function LR_ChayHuyenKim()   LR_Mo( "HuyÒn Kim cïng nguyªn liÖu n©ng cÊp.", "LR_NhanHuyenKim" ) end
function LR_ChayHKTinh()     LR_Mo( "HuyÒn Kim Chi Tinh cïng nguyªn liÖu n©ng cÊp.", "LR_NhanHKTinh" ) end

function LR_NhanEpTim( n )      LR_ChayNhan( LR_CMP_EQUIP_COMPOUND,  n ) end
function LR_NhanDoPho( n )      LR_ChayNhan( LR_CMP_ATLAS,           n ) end
function LR_NhanKhamNam( n )    LR_ChayNhan( LR_CMP_EQUIP_ENCHASE,   n ) end
function LR_NhanNangKhoang( n ) LR_ChayNhan( LR_CMP_ORE_UPGRADE,     n ) end
function LR_NhanHuyenTinh( n )  LR_ChayNhan( LR_CMP_XUANJING,        n ) end
function LR_NhanTinhLuyen( n )  LR_ChayNhan( LR_CMP_MAGIC_DISTILL,   n ) end
function LR_NhanHuyenKim( n )   LR_ChayNhan( LR_CMP_FANTASYGOLD,     n ) end
function LR_NhanHKTinh( n )     LR_ChayNhan( LR_CMP_FANTASYGOLD_ESS, n ) end

function LR_ChayNhan( nType, nCount )
	if( nCount == nil or nCount < 1 ) then
		SayEx({ "Ch­a bá nguyªn liÖu nµo vµo.", "Quay l¹i/LR_MoThoRen" })
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
			if( nType == LR_CMP_ATLAS ) then
				local g, d, p = GetItemProp( nIdx );
				if( LR_TBMANH[format( "%d,%d,%d", g, d, p )] ~= nil ) then
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

	local nItemIdx, nResult = FoundryCompound( nType, aryNec, aryAlt, 0 );

	local szMsg = format( "Thao t¸c <color=yellow>%d<color>: nhËn %d mãn chÝnh, %d mãn phô.\n", nType, nNec, nAlt );
	if( nResult == LR_RESULT_SUCCEED and nItemIdx > 0 ) then
		szMsg = szMsg..format( "<color=green>Thµnh c«ng<color>: <color=yellow>%s<color>", GetItemName( nItemIdx ) );
	else
		local szLoi = LR_TBLOIBAO[nResult + 1];
		if( szLoi == nil ) then szLoi = "Loi khong ro."; end
		szMsg = szMsg..format( "<color=red>Ma ket qua %d<color>: %s", nResult, szLoi );
	end
	SayEx({ szMsg, "Ch¹y tiÕp/LR_MoThoRen", "Quay l¹i/LR_Root", "KÕt thóc/no" })
	EndGiveBox();	-- dong hop giao vat pham
end

--------------------------------------------------------------------------------
-- Don sach khoang thuoc tinh dang nam trong tui.
-- Bo test cu sinh khoang voi nMagLvl1 = 1 (o kham co cap nhung rong) -> re chuot
-- vao la client sap khi ve mo ta, nen KHONG the ban/vut bang tay. Xoa thang bang
-- ConsumeItem dang JX2 (khong doc mo ta, khong dung toi chuot).
--------------------------------------------------------------------------------
function LR_DonKhoangLoi()
	local nDem = 0;
	for i = 1, getn( TBKHOANG_TEST ) do
		local nPtc = TBKHOANG_TEST[i][1];
		local nCon = 1;
		while nCon == 1 do
			if( ConsumeItem( -1, 1, 6, 1, nPtc ) == 1 ) then
				nDem = nDem + 1;
			else
				nCon = 0;
			end
		end
	end
	SayEx({ format( "§· dän <color=yellow>%d<color> viªn kho¸ng thuéc tÝnh khái tói.\nGiê lÊy l¹i nguyªn liÖu lµ s¹ch, kh«ng cßn sËp n÷a.", nDem ),
		"LÊy nguyªn liÖu míi/LR_LayChung", "Quay l¹i/LR_Root", "KÕt thóc/no" })
end

--------------------------------------------------------------------------------
-- [LOREN 28/08] XEM TRUOC DO PHO - nut PreviewBtn cua trang Do pho goi ham nay.
-- Doi chieu 8 o CHINH cua phong 10 voi cong thuc that: cung 6 dieu kien nhu
-- atlas.lua:verifyAtlasMaterials (genre/detail/ptc khop, cap >=, ngu hanh khop,
-- MagicID khop) + dem so mon phai dung. In tung dong Khop/Thieu de nguoi choi
-- biet chinh xac con thieu gi truoc khi bam Ket hop.
--------------------------------------------------------------------------------
function LR_UI_AtlasPreview()
	local nVer = ITEM_GetLatestItemVersion();
	local szPath = LR_DuongBang( nVer, FILE_ATLAS_TEST );
	TabFile_Load( szPath, szPath );
	local nRowCount = TabFile_GetRowCount( szPath );
	local nAtlasRow = 0;
	local bXJ = 0;
	local aryMat = {};
	local nMat = 0;
	local i;
	for i = 0, 7 do
		local nIdx = GetIdItem( 10, i );
		if( nIdx ~= nil and nIdx > 0 ) then
			local g, d, p, lv, se, lu = GetItemProp( nIdx );
			if( g == 6 and d == 1 and p == 146 ) then
				bXJ = 1;
			else
				local nDong = 0;
				local r;
				for r = 2, nRowCount do
					if( tonumber( TabFile_GetCell( szPath, r, "ATLAS_GENRE", -1 ) ) == g and
						tonumber( TabFile_GetCell( szPath, r, "ATLAS_DETAILTYPE", -1 ) ) == d and
						tonumber( TabFile_GetCell( szPath, r, "ATLAS_PARTICULAR", -1 ) ) == p ) then
					nDong = r;
					break;
					end
				end
				if( nDong > 0 and nAtlasRow == 0 ) then
					nAtlasRow = nDong;
				else
					nMat = nMat + 1;
					aryMat[nMat] = { g = g, d = d, p = p, lv = lv, se = se, mg = GetItemParam( nIdx, 1 ) };
				end
			end
		end
	end

	Msg2Player( "<color=yellow>Xem tr­íc ®å phæ:" );
	if( nAtlasRow == 0 ) then
		Msg2Player( "<color=red>Ch­a ®Æt §å phæ vµo «." );
		return;
	end
	local bOk = 1;
	if( bXJ ~= 1 ) then
		Msg2Player( "<color=red>ThiÕu HuyÒn Tinh Kho¸ng Th¹ch (« nhá bªn ph¶i)." );
		bOk = 0;
	end
	local nReq = 0;
	local k;
	for k = 1, 6 do
		local qg = tonumber( TabFile_GetCell( szPath, nAtlasRow, k.."_GENRE", -1 ) );
		if( qg >= 0 ) then
			nReq = nReq + 1;
			local qd = tonumber( TabFile_GetCell( szPath, nAtlasRow, k.."_DETAILTYPE", -1 ) );
			local qp = tonumber( TabFile_GetCell( szPath, nAtlasRow, k.."_PARTICULAR", -1 ) );
			local ql = tonumber( TabFile_GetCell( szPath, nAtlasRow, k.."_LEVEL", -1 ) );
			local qs = tonumber( TabFile_GetCell( szPath, nAtlasRow, k.."_SERIES", -1 ) );
			local qm = tonumber( TabFile_GetCell( szPath, nAtlasRow, k.."_MAGIC_ID", -1 ) );
			local szTen = TabFile_GetCell( szPath, nAtlasRow, k.."_NAME", "" );
			local bCo = 0;
			local j;
			for j = 1, nMat do
				if( aryMat[j].g == qg and
					( qd < 0 or aryMat[j].d == qd ) and
					( qp < 0 or aryMat[j].p == qp ) and
					( ql < 0 or aryMat[j].lv >= ql ) and
					( qs < 0 or aryMat[j].se == qs ) and
					( qm < 0 or aryMat[j].mg == qm ) ) then
				bCo = 1;
				break;
				end
			end
			local szDong;
			if( bCo == 1 ) then
				szDong = "<color=green>[Khíp] "..szTen;
			else
				szDong = "<color=red>[ThiÕu] "..szTen;
				bOk = 0;
			end
			if( ql >= 0 ) then
				szDong = szDong.." - cÊp "..ql;
			end
			if( qm >= 0 ) then
				szDong = szDong.." - phÐp "..qm;
			end
			Msg2Player( szDong );
		end
	end
	if( nReq ~= nMat ) then
		Msg2Player( format( "<color=red>Sè mãn nguyªn liÖu: cÇn %d, ®ang cã %d.", nReq, nMat ) );
		bOk = 0;
	end
	if( bOk == 1 ) then
		Msg2Player( "<color=green>§ñ nguyªn liÖu - bÊm KÕt hîp ®­îc." );
	else
		Msg2Player( "<color=red>Ch­a ®ñ - xem c¸c dßng ®á." );
	end
end

