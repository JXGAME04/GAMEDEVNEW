-- Bo test he PHI PHONG - AN - TRANG SUC - MAT NA cho lenh bai admin.
-- [31/08d] Viet lai: nhan/thong bao TIENG VIET CO DAU (TCVN3) + phat theo BO
-- dung tung luong de khong con canh 'bo vao ep bao thieu nguyen lieu':
--   * bo TANG SAO: 1 Tuyet The + 50 Tinh Ngoc (tang sao nhan moi so luong)
--   * bo DOT PHA:  1 Tuyet The 10 sao day chuc phuc + DUNG 1 Thien Tinh Ngoc
--                  (hop dot pha doi dung 1 vien - bo ca chong la bi tu choi)
--   * bo KHAM:     1 Ngu Phong 5 lo (4 lo kich hoat, lo cuoi 0 sao) + 5 da
-- BAY khi sua tep nay (deu da dinh that):
--   * bang chon nhoi TIEU DE + MOI NHAN vao chung bo dem 512 byte - CAT IM LANG
--   * cam dau gach cheo trong nhan; cam dau so dung cuoi nhan/tieu de
--   * TCVN3 khong ma hoa duoc CHU HOA co dau (tru A^ A( D D- E^ O^ O+ U+)
--   * AddItem doi DU 15 tham so moi nhan duoc so luong chong
Include("\\script\\lib\\common.lua")

-- WXY_MoBang() phai nam CHUNG Lua_State voi lenh bai (callback theo m_ActionScriptID)
Include("\\script\\event\\equip_publish\\wuxingyin\\wuxingyin.lua")

-- [PF13 31/08b] muon PF_RollAnAttr/PF_LayKheAn/PF_LaPhiPhongCoAn de bo thu
-- tu sinh dong an (StarLevelUp tho khong qua Tho Ren nen khong tu no)
Include("\\script\\global\\mantlesystem\\mantleupgrade_head.lua")

PP_O_MATNA	= 12;
PP_O_PHIPHONG	= 13;
PP_O_AN		= 14;
PP_O_TRANGSUC	= 15;

-- 9 nguyen lieu (magicscript.txt 4881..4889)
PP_NGUYENLIEU = { 4881, 4882, 4883, 4884, 4885, 4886, 4887, 4888, 4889,
	4933, 4934, 4935, 4936, 4937 }; -- [PF13 31/08] DBL/PhachTN/DaiTT/KimTinh/LenhBai

-- chuoi dot pha hoang kim: 5374 -> 5375 -> 5376 -> 5377 -> 5378 -> 3485
PP_HOANGKIM = { 5374, 5375, 5376, 5377, 5378, 3485 };
-- [PF13 31/08] 8 bac cao chuoi VLTK: Phe Quang -> So Phuong (key goldequip)
PP_CAOKIM = { 5939, 5940, 5959, 5960, 5961, 6733, 6734, 6735 };
-- bach kim + Vo Cuc 1..5 o kham
PP_BACHKIM = { 5300, 5301, 5302, 5303, 5304, 4835, 4836, 4837, 4838, 4839 };

PP_AN		= { 3225, 3230, 5635, 5930, 5931, 5932 };
PP_TRANGSUC	= { 3491, 3492, 3493, 5281, 5289, 5657 };
PP_MATNA	= { 4493, 4496, 5312, 5316, 5320, 5321 };
PP_AN_CUONGHOA	= { 3205, 3206, 3207, 3208, 3209, 3210 };
PP_AN_NHUOCHOA	= { 3215, 3216, 3217, 3218, 3219, 3220 };

PP_TIEN		= 1000000000;
PP_SOLUONG	= 200;	-- so nguyen lieu moi lan phat o muc 'du bo'
PP_SODA		= 1;	-- da kham phat 1 vien moi loai (da xep chong bi tu choi kham)

-- ------------------------------------------------------------------ tro giup

function PP_Cho(nP, nSo)
	-- AddItem doi DU 15 tham so moi nhan duoc so luong chong (ScriptFuns.cpp)
	AddItem(6, 1, nP, 0, 0, 0, 0, 0, 0, 0, 0, 0, nSo, 0, 0);
end

function PP_MonMac(nO)
	local tb = GetAllEquipment();
	if not tb then
		return 0;
	end
	local n = tb[nO];
	if not n then
		return 0;
	end
	return n;
end

function PP_TenMon(nIdx)
	if nIdx <= 0 then
		return "kh«ng cã";
	end
	return GetItemName(nIdx);
end

-- ep mot mon VUA TAO (chua mac) len 10 sao va kham day - an toan vi mon chua
-- di qua ApplyMagicAttribToNPC lan nao
function PP_EpMon(nIdx)
	if nIdx <= 0 then
		return 0;
	end
	local i = 1;
	while i <= 10 do
		StarLevelUp(nIdx);
		i = i + 1;
	end
	local nMax = GetMaxEquipWishValue(nIdx);
	if nMax > 0 then
		SetEquipWishValue(nIdx, nMax);
	end
	local nO = GetEquipMaxStoneNum(nIdx);
	i = 1;
	while i <= nO do
		SetStoneLevelOnEquip(nIdx, i, 10);
		InlayStarStone(nIdx, i, i);
		i = i + 1;
	end
	-- [PF13 31/08b] Long Ngam+ ep san 10 sao: tu sinh 2 dong an luon
	if PF_LaPhiPhongCoAn and PF_LaPhiPhongCoAn(nIdx) == 1 and PF_LayKheAn(nIdx) == 0 then
		PF_RollAnAttr(nIdx);
	end
	SyncItem(nIdx);
	return nO;
end

-- [VA 31/08f] kich hoat lo theo trang thai TU NHIEN cua kinh te that:
-- hoang kim = sau dot pha thuong (lo 1..nO-1 muoi sao, lo cuoi 0 sao);
-- bach kim  = sau dot pha lan 2 (du het muoi sao). Khong dung thi thuoc
-- tinh da nam ngu het va tooltip khong co khoi thuoc tinh tim.
function PP_KichHoatLo(nIdx, nBachKim)
	if nIdx <= 0 then
		return 0;
	end
	local nO = GetEquipMaxStoneNum(nIdx);
	local nToi = nO;
	if nBachKim ~= 1 then
		nToi = nO - 1;
	end
	local i = 1;
	while i <= nToi do
		SetStoneLevelOnEquip(nIdx, i, 10);
		i = i + 1;
	end
	SyncItem(nIdx);
	return nO;
end

-- ---------------------------------------------------------------- bang chinh

function PP_Root()
	local nIdx = PP_MonMac(PP_O_PHIPHONG);
	local szTin = "";
	if nIdx <= 0 then
		szTin = "Ch­a mÆc Phi Phong nµo.";
	else
		szTin = format("§ang mÆc %s: %d sao, %d « kh¶m, chóc phóc %d trªn %d",
			GetItemName(nIdx), GetStarLevel(nIdx), GetEquipMaxStoneNum(nIdx),
			GetCurEquipWishValue(nIdx), GetMaxEquipWishValue(nIdx));
	end
	SayEx({ szTin,
	"NhËn bé thö t¨ng sao/PP_BoTangSao",
	"NhËn bé thö ®ét ph¸/PP_BoDotPha",
	"NhËn bé thö kh¶m ®¸/PP_BoKham",
	"Bé thö ®ét ph¸ bËc cao/PP_BoCao",
	"Bé thö 13 lç kh¶m/PP_Bo13Lo",
	"Bé thö thuéc tÝnh Èn/PP_BoAn",
	"NhËn nguyªn liÖu ®ñ bé/PP_ChoNguyenLieu",
	"NhËn ®¸ kh¶m ®ñ lo¹i/PP_ChoDa",
	"NhËn Phi Phong theo bËc/PP_MenuPhiPhong",
	"NhËn Ên, trang søc, mÆt n¹/PP_MenuBonMon",
	"Ðp th¼ng mãn ®ang mÆc/PP_MenuEp",
	"Hoµng Kim Ên/PP_MenuAn",
	"Soi chi tiÕt mãn ®ang mÆc/PP_Soi",
	"KÕt thóc/no" });
end

-- --------------------------------------------------------- ba bo thu nhanh

function PP_BoTangSao()
	AddGoldEquipByRow(0, 5374);
	PP_Cho(4881, 50);
	Earn(PP_TIEN);
	Talk(1, "", "§· cho 1 Phi phong TuyÖt ThÕ, 50 Tinh Ngäc vµ tiÒn. Tíi Thî RÌn chän Phi Phong t¨ng sao, ®Æt phi phong cïng vµi viªn Tinh Ngäc (mçi viªn céng thªm tØ lÖ, tèn 100 v¹n l­îng mçi lÇn).");
end

function PP_BoDotPha()
	local nIdx = AddGoldEquipByRow(0, 5374);
	if nIdx > 0 then
		local i = 1;
		while i <= 10 do
			StarLevelUp(nIdx);
			i = i + 1;
		end
		local nMax = GetMaxEquipWishValue(nIdx);
		if nMax > 0 then
			SetEquipWishValue(nIdx, nMax);
		end
		SyncItem(nIdx);
	end
	PP_Cho(4882, 1);
	Earn(PP_TIEN);
	Talk(1, "", "§· cho 1 Phi phong TuyÖt ThÕ 10 sao ®Çy chóc phóc (ch¾c ch¾n thµnh c«ng) vµ ®óng 1 viªn Thiªn Tinh Ngäc. <color=red>Hép ®ét ph¸ ®ßi ®óng 1 viªn, bá nguyªn chång nhiÒu viªn sÏ bÞ tõ chèi.<color> Chó ý ®õng nhÇm víi M¶nh Thiªn Tinh Ngäc.");
end

function PP_BoKham()
	local nIdx = AddGoldEquipByRow(0, 5378);
	local nO = 0;
	if nIdx > 0 then
		nO = GetEquipMaxStoneNum(nIdx);
		local i = 1;
		while i < nO do
			SetStoneLevelOnEquip(nIdx, i, 10);
			i = i + 1;
		end
		SyncItem(nIdx);
	end
	local i = 1;
	while i <= 5 do
		AddItem(9, 1, i, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0);
		i = i + 1;
	end
	Talk(1, "", format("§· cho 1 Phi phong Ngù Phong %d lç (4 lç ®· kÝch ho¹t, lç cuèi 0 sao nh­ sau ®ét ph¸ thËt) vµ 5 viªn ®¸ kh¸c lo¹i. §Æt phi phong vµo « gi÷a, ®¸ vµo ®óng « lç muèn kh¶m. §¸ kh¶m tªn cã tiÒn tè, ®õng nhÇm víi nguyªn liÖu Tinh ThÇn Th¹ch.", nO));
end

-- [PF13 31/08] bo thu DOT PHA BAC CAO: Ngu Phong 10 sao + du 2 loai Bach Luyen.
function PP_BoCao()
	local nIdx = AddGoldEquipByRow(0, 5378);
	if nIdx > 0 then
		local i = 1;
		while i <= 10 do
			StarLevelUp(nIdx);
			i = i + 1;
		end
		PP_KichHoatLo(nIdx, 0);
	end
	PP_Cho(4884, 250);
	PP_Cho(4933, 100);
	Earn(PP_TIEN);
	Talk(1, "", "§· cho 1 Ngù Phong 10 sao, 250 B¸ch LuyÖn, 100 §¹i B¸ch LuyÖn vµ tiÒn. Tíi Thî RÌn chän ®ét ph¸ bËc cao, ®i lÇn l­ît tõng bËc tíi Så Ph­îng.");
end

-- [PF13 31/08] bo thu 13 LO: So Phuong 12 lo kich hoat + 13 vien da khac loai.
function PP_Bo13Lo()
	local nIdx = AddGoldEquipByRow(0, 6735);
	local nO = 0;
	if nIdx > 0 then
		nO = GetEquipMaxStoneNum(nIdx);
		local i = 1;
		while i < nO do
			SetStoneLevelOnEquip(nIdx, i, 10);
			i = i + 1;
		end
		SyncItem(nIdx);
	end
	local i = 1;
	while i <= 13 do
		AddItem(9, 1, i, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0);
		i = i + 1;
	end
	Talk(1, "", format("§· cho 1 Så Ph­îng %d lç (12 lç kÝch ho¹t) vµ 13 viªn ®¸ kh¸c lo¹i. Më b¶ng kh¶m sÏ thÊy ®ñ 13 « quanh « Phi Phong.", nO));
end

-- [PF13 31/08] bo thu THUOC TINH AN: Long Ngam + nguyen lieu tay.
-- GM ep sao bang StarLevelUp KHONG chay duong Tho Ren nen dong an chua sinh;
-- dung menu 'Tay thuoc tinh an' cua Tho Ren de sinh lan dau (thiet ke co y).
function PP_BoAn()
	local nIdx = AddGoldEquipByRow(0, 6734);
	if nIdx > 0 then
		local i = 1;
		while i <= 10 do
			StarLevelUp(nIdx);
			i = i + 1;
		end
		PP_KichHoatLo(nIdx, 0);
		-- [PF13 31/08b] tu sinh dong an ngay de thay khoi tim lien
		if PF_LaPhiPhongCoAn and PF_LaPhiPhongCoAn(nIdx) == 1 then
			PF_RollAnAttr(nIdx);
		end
	end
	PP_Cho(4937, 5);
	PP_Cho(4933, 20);
	Earn(PP_TIEN);
	Talk(1, "", "§· cho 1 Long Ng©m 10 sao, 5 LÖnh Bµi, 20 §¹i B¸ch LuyÖn vµ tiÒn. Tíi Thî RÌn chän tÈy thuéc tÝnh Èn ®Ó sinh hoÆc quay l¹i 2 dßng Èn.");
end

function PP_HK_Cao()
	Talk(1, "", format("§· cho %d Phi Phong bËc cao, PhÖ Quang tíi Så Ph­îng.", PP_ChoTheoBang(PP_CAOKIM, 0, 0)));
end

-- ------------------------------------------------------------- nguyen lieu

function PP_ChoNguyenLieu()
	local i = 1;
	while i <= getn(PP_NGUYENLIEU) do
		PP_Cho(PP_NGUYENLIEU[i], PP_SOLUONG);
		i = i + 1;
	end
	Talk(1, "", format("§· cho %d lo¹i nguyªn liÖu, mçi lo¹i %d c¸i. NÕu tói ®Çy phÇn thõa r¬i xuèng ®Êt. T¨ng sao dïng Tinh Ngäc, ®ét ph¸ dïng ®óng 1 Thiªn Tinh Ngäc, n©ng V« Cùc dïng B¸ch LuyÖn Thµnh C­¬ng.", getn(PP_NGUYENLIEU), PP_SOLUONG));
end

function PP_ChoDa()
	local i = 1;
	while i <= 34 do
		AddItem(9, 1, i, 0, 0, 0, 0, 0, 0, 0, 0, 0, PP_SODA, 0, 0);
		i = i + 1;
	end
	Talk(1, "", format("§· cho ®ñ 34 lo¹i Tinh ThÇn Th¹ch (®¸ kh¶m thËt, tªn cã tiÒn tè), mçi lo¹i %d viªn. CÇn 34 « tói trèng.", PP_SODA));
end

-- ------------------------------------------------------------- theo bac

function PP_ChoTheoBang(tb, nBachKim, nEp)
	local i = 1;
	local nCuoi = 0;
	while i <= getn(tb) do
		local nIdx = 0;
		if nBachKim == 1 then
			nIdx = AddPlatinaItem(0, tb[i]);
		else
			nIdx = AddGoldEquipByRow(0, tb[i]);
		end
		if nIdx > 0 then
			if nEp == 1 then
				PP_EpMon(nIdx);
			else
				PP_KichHoatLo(nIdx, nBachKim);
			end
			nCuoi = nCuoi + 1;
		end
		i = i + 1;
	end
	return nCuoi;
end

function PP_MenuPhiPhong()
	SayEx({ "Phi Phong theo bËc, chuçi ®ét ph¸ cña b¶n Linux",
	"Hoµng kim c¶ s¸u bËc/PP_HK_All",
	"TuyÖt ThÕ, bËc ®Çu chuçi/PP_HK_1",
	"Ngù Phong, bËc cuèi/PP_HK_5",
	"V« Cùc hoµng kim/PP_HK_6",
	"BËc cao, PhÖ Quang tíi Så Ph­îng/PP_HK_Cao",
	"B¹ch kim c¶ m­êi bËc/PP_BK_All",
	"V« Cùc mét « kh¶m/PP_BK_1",
	"V« Cùc n¨m « kh¶m/PP_BK_5",
	"Ðp s½n m­êi sao kh¶m ®Çy/PP_MenuEpSan",
	"Quay l¹i/PP_Root" });
end

function PP_HK_All()
	Talk(1, "", format("§· cho %d Phi Phong hoµng kim.", PP_ChoTheoBang(PP_HOANGKIM, 0, 0)));
end

function PP_HK_1()
	PP_KichHoatLo(AddGoldEquipByRow(0, 5374), 0);
	Talk(1, "", "§· cho Phi phong TuyÖt ThÕ. §em tíi Thî RÌn ®Ó t¨ng sao.");
end

function PP_HK_5()
	PP_KichHoatLo(AddGoldEquipByRow(0, 5378), 0);
	Talk(1, "", "§· cho Phi phong Ngù Phong. §ñ 10 sao th× dïng nh¸nh n©ng cÊp V« Cùc.");
end

function PP_HK_6()
	PP_KichHoatLo(AddGoldEquipByRow(0, 3485), 0);
	Talk(1, "", "§· cho Phi phong V« Cùc hoµng kim.");
end

function PP_BK_All()
	Talk(1, "", format("§· cho %d Phi Phong b¹ch kim.", PP_ChoTheoBang(PP_BACHKIM, 1, 0)));
end

function PP_BK_1()
	PP_KichHoatLo(AddPlatinaItem(0, 4835), 1);
	Talk(1, "", "§· cho Phi phong V« Cùc mét « kh¶m.");
end

function PP_BK_5()
	PP_KichHoatLo(AddPlatinaItem(0, 4839), 1);
	Talk(1, "", "§· cho Phi phong V« Cùc n¨m « kh¶m.");
end

function PP_MenuEpSan()
	SayEx({ "Mãn Ðp s½n: 10 sao, ®Çy chóc phóc, kh¶m ®Çy « b»ng ®¸ sè mét trë ®i",
	"Ngù Phong m­êi sao n¨m «/PP_ES_NguPhong",
	"V« Cùc b¹ch kim n¨m «/PP_ES_VoCuc",
	"C¶ s¸u bËc hoµng kim/PP_ES_HK",
	"Quay l¹i/PP_MenuPhiPhong" });
end

function PP_ES_NguPhong()
	local nIdx = AddGoldEquipByRow(0, 5378);
	local nO = PP_EpMon(nIdx);
	Talk(1, "", format("§· cho Ngù Phong 10 sao, kh¶m ®Çy %d «. MÆc vµo råi soi thuéc tÝnh.", nO));
end

function PP_ES_VoCuc()
	local nIdx = AddPlatinaItem(0, 4839);
	local nO = PP_EpMon(nIdx);
	Talk(1, "", format("§· cho V« Cùc b¹ch kim 10 sao, kh¶m ®Çy %d «.", nO));
end

function PP_ES_HK()
	Talk(1, "", format("§· cho %d Phi Phong hoµng kim ®· Ðp s½n.", PP_ChoTheoBang(PP_HOANGKIM, 0, 1)));
end

-- ------------------------------------------------- an - trang suc - mat na

function PP_ChoDanhSach(tb)
	local i = 1;
	local n = 0;
	while i <= getn(tb) do
		if AddGoldEquipByRow(0, tb[i]) > 0 then
			n = n + 1;
		end
		i = i + 1;
	end
	return n;
end

function PP_MenuBonMon()
	SayEx({ "Ba « cßn l¹i, ph¸t ®óng dßng ®· thay tõ b¶n Linux ®Ó ®èi chiÕu tªn vµ thuéc tÝnh",
	"NhËn s¸u Ên/PP_ChoAn",
	"NhËn s¸u trang søc/PP_ChoTrangSuc",
	"NhËn s¸u mÆt n¹/PP_ChoMatNa",
	"NhËn hÕt c¶ ba lo¹i/PP_ChoBaLoai",
	"Quay l¹i/PP_Root" });
end

function PP_ChoAn()
	Talk(1, "", format("§· cho %d Ên.", PP_ChoDanhSach(PP_AN)));
end

function PP_ChoTrangSuc()
	Talk(1, "", format("§· cho %d trang søc.", PP_ChoDanhSach(PP_TRANGSUC)));
end

function PP_ChoMatNa()
	Talk(1, "", format("§· cho %d mÆt n¹.", PP_ChoDanhSach(PP_MATNA)));
end

function PP_ChoBaLoai()
	local n = PP_ChoDanhSach(PP_AN) + PP_ChoDanhSach(PP_TRANGSUC) + PP_ChoDanhSach(PP_MATNA);
	Talk(1, "", format("§· cho %d mãn cña ba « Ên, trang søc, mÆt n¹.", n));
end

-- ---------------------------------------------------- ep thang mon dang mac

function PP_MenuEp()
	local nIdx = PP_MonMac(PP_O_PHIPHONG);
	if nIdx <= 0 then
		Talk(1, "", "Ch­a mÆc Phi Phong nµo. H·y mÆc vµo råi më l¹i b¶ng chän.");
		return;
	end
	SayEx({ format("SÏ Ðp th¼ng %s. Söa xong b¾t buéc tho¸t nh©n vËt råi vµo l¹i, ®õng cëi ra tr­íc.", GetItemName(nIdx)),
	"T¨ng mét sao/PP_Ep_Sao1",
	"Lªn th¼ng m­êi sao/PP_Ep_Sao10",
	"N¹p ®Çy ®iÓm chóc phóc/PP_Ep_ChucPhuc",
	"Kh¶m ®Çy « b»ng ®¸ sè mét trë ®i/PP_Ep_KhamDay",
	"KÝch ho¹t lç kh¶m m­êi sao/PP_Ep_KichHoat",
	"Gì hÕt ®¸ kh¶m/PP_Ep_GoDa",
	"§Æt l¹i giê ®ét ph¸ lÇn tr­íc/PP_Ep_GioDotPha",
	"Quay l¹i/PP_Root" });
end

-- vi sao phai thoat nhan vat: thuoc tinh da kham CONG luc mac, TRU luc coi;
-- doi da khi dang mac -> luc coi tru theo da MOI chua tung duoc cong = lem chi so
PP_NHAC = "Xong. H·y tho¸t nh©n vËt råi vµo l¹i ®Ó thuéc tÝnh tÝnh l¹i ®óng.";

function PP_Ep_Sao1()
	local nIdx = PP_MonMac(PP_O_PHIPHONG);
	if nIdx <= 0 then
		return;
	end
	StarLevelUp(nIdx);
	SyncItem(nIdx);
	Talk(1, "", format("Sao hiÖn t¹i %d. %s", GetStarLevel(nIdx), PP_NHAC));
end

function PP_Ep_Sao10()
	local nIdx = PP_MonMac(PP_O_PHIPHONG);
	if nIdx <= 0 then
		return;
	end
	local i = 1;
	while i <= 10 do
		StarLevelUp(nIdx);
		i = i + 1;
	end
	SyncItem(nIdx);
	Talk(1, "", format("Sao hiÖn t¹i %d. %s", GetStarLevel(nIdx), PP_NHAC));
end

function PP_Ep_ChucPhuc()
	local nIdx = PP_MonMac(PP_O_PHIPHONG);
	if nIdx <= 0 then
		return;
	end
	local nMax = GetMaxEquipWishValue(nIdx);
	SetEquipWishValue(nIdx, nMax);
	SyncItem(nIdx);
	Talk(1, "", format("§iÓm chóc phóc %d trªn %d.", GetCurEquipWishValue(nIdx), nMax));
end

function PP_Ep_KhamDay()
	local nIdx = PP_MonMac(PP_O_PHIPHONG);
	if nIdx <= 0 then
		return;
	end
	local nO = GetEquipMaxStoneNum(nIdx);
	if nO <= 0 then
		Talk(1, "", "Mãn nµy kh«ng cã « kh¶m nµo. Ph¶i ®ét ph¸ lªn bËc cã « kh¶m tr­íc.");
		return;
	end
	local i = 1;
	while i <= nO do
		SetStoneLevelOnEquip(nIdx, i, 10);
		InlayStarStone(nIdx, i, i);
		i = i + 1;
	end
	SyncItem(nIdx);
	Talk(1, "", format("§· kh¶m ®Çy %d «. %s", nO, PP_NHAC));
end

function PP_Ep_KichHoat()
	local nIdx = PP_MonMac(PP_O_PHIPHONG);
	if nIdx <= 0 then
		return;
	end
	local nO = GetEquipMaxStoneNum(nIdx);
	local i = 1;
	while i <= nO do
		SetStoneLevelOnEquip(nIdx, i, 10);
		i = i + 1;
	end
	SyncItem(nIdx);
	Talk(1, "", format("§· kÝch ho¹t %d lç lªn 10 sao, ®¸ ®ang n»m ngñ sÏ ph¸t huy. %s", nO, PP_NHAC));
end

function PP_Ep_GoDa()
	local nIdx = PP_MonMac(PP_O_PHIPHONG);
	if nIdx <= 0 then
		return;
	end
	local nO = GetEquipMaxStoneNum(nIdx);
	local i = 1;
	while i <= nO do
		InlayStarStone(nIdx, i, 0);
		SetStoneLevelOnEquip(nIdx, i, 0);
		i = i + 1;
	end
	SyncItem(nIdx);
	Talk(1, "", format("§· gì hÕt ®¸ ë %d «. %s", nO, PP_NHAC));
end

function PP_Ep_GioDotPha()
	local nIdx = PP_MonMac(PP_O_PHIPHONG);
	if nIdx <= 0 then
		return;
	end
	SetLastBreakTime(nIdx, 0);
	SyncItem(nIdx);
	Talk(1, "", "§· ®Æt l¹i giê ®ét ph¸ lÇn tr­íc vÒ 0, kh«ng cßn ph¶i chê 15 ngµy.");
end

-- --------------------------------------------------- tien va Hoang Kim an

function PP_ChoTien()
	Earn(PP_TIEN);
	Talk(1, "", format("§· cho %d l­îng. §ang cã %d l­îng.", PP_TIEN, GetCash()));
end

function PP_MenuAn()
	SayEx({ "Hoµng Kim Ên, hÖ n©ng cÊp b»ng V­¬ng ThiÕt T­îng LÖnh Phï",
	"NhËn Hoµng Kim Ên c¸c cÊp/PP_ChoHoangKimAn",
	"NhËn tiÒn/PP_ChoTien",
	"Më b¶ng n©ng cÊp/PP_NangAn",
	"Quay l¹i/PP_Root" });
end

function PP_ChoHoangKimAn()
	local n = PP_ChoDanhSach(PP_AN_CUONGHOA) + PP_ChoDanhSach(PP_AN_NHUOCHOA);
	PP_Cho(4889, PP_SOLUONG);
	Talk(1, "", format("§· cho %d Hoµng Kim Ên (6 cÊp c­êng hãa, 6 cÊp nh­îc hãa) vµ %d V­¬ng ThiÕt T­îng LÖnh Phï.", n, PP_SOLUONG));
end

function PP_NangAn()
	if not WXY_MoBang then
		Talk(1, "", "Ch­a n¹p ®­îc hÖ n©ng cÊp Hoµng Kim Ên (wuxingyin.lua).");
		return;
	end
	WXY_MoBang();
end

-- ------------------------------------------------------------------ soi mon

function PP_Soi()
	local nIdx = PP_MonMac(PP_O_PHIPHONG);
	if nIdx <= 0 then
		Talk(1, "", "Ch­a mÆc Phi Phong nµo.");
		return;
	end
	local nO = GetEquipMaxStoneNum(nIdx);
	local sz = format("Tªn: %s\nSao: %d\nSè « kh¶m: %d\nChóc phóc: %d trªn %d\nGiê ®ét ph¸ lÇn tr­íc: %d\n",
		GetItemName(nIdx), GetStarLevel(nIdx), nO,
		GetCurEquipWishValue(nIdx), GetMaxEquipWishValue(nIdx), GetLastBreakTime(nIdx));
	local i = 1;
	while i <= nO do
		sz = sz .. format("  « %d: ®¸ sè %d, cÊp %d\n", i,
			GetStarStoneOnEquip(nIdx, i), GetStoneLevelOnEquip(nIdx, i));
		i = i + 1;
	end
	local n2 = PP_MonMac(PP_O_MATNA);
	local n3 = PP_MonMac(PP_O_AN);
	local n4 = PP_MonMac(PP_O_TRANGSUC);
	sz = sz .. format("MÆt n¹: %s\nÊn: %s\nTrang søc: %s",
		PP_TenMon(n2), PP_TenMon(n3), PP_TenMon(n4));
	Talk(1, "", sz);
end
