-- ================================================================================================
-- [HE THONG] script/global/trangbisieunhan.lua
-- Muc dich  : TRANG BI SIEU NHAN (70 ham).
-- Duoc nap  : Include tu 1 tep (vd hotrotest.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Ham (dong): TrangBiCaoCap (1), NhanHkmp (8), NhanHkmps (29), SetTHkmp (48), SetTHkmp2 (54), NhanBachKim (59), NhanBachKims (81), SetTBachKim (100), SetTBachKim2 (106), DoHoangKim (111), ThanhCau (133), ThanhCaus (152), SetThanhCauMin (170), SetThanhCauMin2 (175), VanLoc (181), VanLocs (200), SetVanLocMin (218), SetVanLocMin2 (224), ThuongLang (230), ThuongLangs (249), SetThuongLangMin (267), SetThuongLangMin2 (272), HuyenVien (278), HuyenViens (297) ... (70 ham)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
function TrangBiCaoCap()
	local tab_Content = {
		"Trang BÞ  Thuéc TÝnh/DoHoangKim",
		"KÕt Thóc/No",
	}
	Say("H·y Chän Lo¹i Trang BÞ", getn(tab_Content), tab_Content)
end
function NhanHkmp() 
Say("H·y Chän M«n Ph¸i",16,
	"Chän: <Thiªu L©m QuyÒn>/SetTHkmp", 
	"Chän: <Thiªu L©m Bæng>/SetTHkmp",
	"Chän: <Thiªu L©m §ao>/SetTHkmp",
	"Chän: <Thiªn V­¬ng Chïy>/SetTHkmp",
	"Chän: <Thiªn V­¬ng Th­¬ng>/SetTHkmp",
	"Chän: <Thiªn V­¬ng §ao>/SetTHkmp",
	"Chän: <Nga My KiÕm>/SetTHkmp",
	"Chän: <Nga My Ch­ëng>/SetTHkmp",
	"Chän: <Nga My Hç Trî>/SetTHkmp",
	"Chän: <Thóy Yªn §ao>/SetTHkmp",
	"Chän: <Thóy Yªn Néi>/SetTHkmp",
	"Chän: <Ngò §éc Ch­ëng>/SetTHkmp",
	"Chän: <Ngò §éc §ao>/SetTHkmp",
	"Chän: <Ngò §éc Bïa>/SetTHkmp",
	"Chän: <§­êng M«n Phi §ao>/SetTHkmp",
	
	
	"Trang KÕ/NhanHkmps")
end

function NhanHkmps() 
Say("H·y Chän M«n Ph¸i",16,
	"Chän: <§­êng M«n Ná TiÔn>/SetTHkmp2",
	"Chän: <§­êng M«n Phi Tiªu>/SetTHkmp2",
	"Chän: <§­êng M«n BÉy>/SetTHkmp2",
	"Chän: <C¸i Bang Ch­ëng>/SetTHkmp2", 
	"Chän: <C¸i Bang Bæng>/SetTHkmp2", 
	"Chän: <Thiªn NhÉn KÝch>/SetTHkmp2", 
	"Chän: <Thiªn NhÉn §ao>/SetTHkmp2",
	"Chän: <Thiªn NhÉn Bïa>/SetTHkmp2",
	"Chän: <Vâ §ang QuyÒn>/SetTHkmp2",
	"Chän: <Vâ §ang KiÕm>/SetTHkmp2",
	"Chän: <C«n L«n §ao>/SetTHkmp2",
	"Chän: <C«n L«n KiÕm>/SetTHkmp2",
	"Chän: <C«n L«n Bïa>/SetTHkmp2",
	"Quay L¹i/NhanHkmp",
	"KÕt Thóc/No")
end

function SetTHkmp(nSel)
		for i=0, 4 do
			AddItem2(2,0,5*nSel + i  ,0,0,0,0)
		end
end
function SetTHkmp2(nSel)
		for i=75, 79 do
			AddItem2(2,0,5*nSel + i  ,0,0,0,0)
		end
end

function NhanBachKim() 
Say("H·y Chän M«n Ph¸i",16,
	"Chän: <Thiªu L©m QuyÒn>/SetTBachKim", 
	"Chän: <Thiªu L©m Bæng>/SetTBachKim",
	"Chän: <Thiªu L©m §ao>/SetTBachKim",
	"Chän: <Thiªn V­¬ng Chïy>/SetTBachKim",
	"Chän: <Thiªn V­¬ng Th­¬ng>/SetTBachKim",
	"Chän: <Thiªn V­¬ng §ao>/SetTBachKim",
	"Chän: <Nga My KiÕm>/SetTBachKim",
	"Chän: <Nga My Ch­ëng>/SetTBachKim",
	"Chän: <Nga My Hç Trî>/SetTBachKim",
	"Chän: <Thóy Yªn §ao>/SetTBachKim",
	"Chän: <Thóy Yªn Néi>/SetTBachKim",
	"Chän: <Ngò §éc Ch­ëng>/SetTBachKim",
	"Chän: <Ngò §éc §ao>/SetTBachKim",
	"Chän: <Ngò §éc Bïa>/SetTBachKim",
	"Chän: <§­êng M«n Phi §ao>/SetTBachKim",
	
	
	"Trang KÕ/NhanBachKims")
end

function NhanBachKims() 
Say("H·y Chän M«n Ph¸i",16,
	"Chän: <§­êng M«n Ná TiÔn>/SetTBachKim2",
	"Chän: <§­êng M«n Phi Tiªu>/SetTBachKim2",
	"Chän: <§­êng M«n BÉy>/SetTBachKim2",
	"Chän: <C¸i Bang Ch­ëng>/SetTBachKim2", 
	"Chän: <C¸i Bang Bæng>/SetTBachKim2", 
	"Chän: <Thiªn NhÉn KÝch>/SetTBachKim2", 
	"Chän: <Thiªn NhÉn §ao>/SetTBachKim2",
	"Chän: <Thiªn NhÉn Bïa>/SetTBachKim2",
	"Chän: <Vâ §ang QuyÒn>/SetTBachKim2",
	"Chän: <Vâ §ang KiÕm>/SetTBachKim2",
	"Chän: <C«n L«n §ao>/SetTBachKim2",
	"Chän: <C«n L«n KiÕm>/SetTBachKim2",
	"Chän: <C«n L«n Bïa>/SetTBachKim2",
	"Quay L¹i/NhanBachKim",
	"KÕt Thóc/No")
end

function SetTBachKim(nSel)
		for i=0, 4 do
			AddItem2(3,0,5*nSel + i  ,0,0,0,0)
		end
end
function SetTBachKim2(nSel)
		for i=75, 79 do
			AddItem2(3,0,5*nSel + i  ,0,0,0,0)
		end
end
function DoHoangKim()
	 local tab_Content = {
		"Trang BÞ Thanh C©u/ThanhCau",
		"Trang BÞ V©n Léc/VanLoc",
		"Trang BÞ Th­¬ng Lang/ThuongLang",
		"Trang BÞ HuyÒn Viªn/HuyenVien",
		"Trang BÞ Tö M·ng/TuMang",
		"Trang BÞ Kim ¤/KimO",
		"Trang BÞ B¹ch Hæ/BachHo",
		"Trang BÞ XÝch L©n/XichLan",
		"Trang BÞ Minh Ph­îng/MinhPhuong",
		"Trang BÞ §»ng Long/DangLong",
		"Trang BÞ Tinh S­¬ng/TinhSuong",
		"Trang BÞ NguyÖt KhuyÕt/NguyetKhuyet",
		"Trang BÞ DiÖu D­¬ng/DieuDuong",
		"Trang BÞ Anh Hµo/AnhHao",
		"Trang BÞ Thiªn MÖnh/ThienMenh",
		"KÕt Thóc/No"
	}
	Say("H·y Chän Lo¹i Trang BÞ", getn(tab_Content), tab_Content)
end
-----------------------------------------------------------------------------
function ThanhCau()
Say("H·y Chän M«n Ph¸i",16,
	"Chän: <Thiªu L©m QuyÒn>/SetThanhCauMin",
	"Chän: <Thiªu L©m Bæng>/SetThanhCauMin",
	"Chän: <Thiªu L©m §ao>/SetThanhCauMin",
	"Chän: <Thiªn V­¬ng Chïy>/SetThanhCauMin",
	"Chän: <Thiªn V­¬ng Th­¬ng>/SetThanhCauMin",
	"Chän: <Thiªn V­¬ng §ao>/SetThanhCauMin",
	"Chän: <Nga My KiÕm>/SetThanhCauMin",
	"Chän: <Nga My Ch­ëng>/SetThanhCauMin",
	"Chän: <Thóy Yªn §ao>/SetThanhCauMin",
	"Chän: <Thóy Yªn Néi>/SetThanhCauMin",
	"Chän: <Ngò §éc Ch­ëng>/SetThanhCauMin",
	"Chän: <Ngò §éc §ao>/SetThanhCauMin",
	"Chän: <§­êng M«n Phi §ao>/SetThanhCauMin",
	"Chän: <§­êng M«n Ná TiÔn>/SetThanhCauMin",
	"Chän: <§­êng M«n Phi Tiªu>/SetThanhCauMin",
	"Trang KÕ/ThanhCaus")
end

function ThanhCaus()
Say("H·y Chän M«n Ph¸i",10,
	"Chän: <C¸i Bang Ch­ëng>/SetThanhCauMin2",
	"Chän: <C¸i Bang Bæng>/SetThanhCauMin2",
	"Chän: <Thiªn NhÉn KÝch>/SetThanhCauMin2",
	"Chän: <Thiªn NhÉn §ao>/SetThanhCauMin2",
	"Chän: <Vâ §ang QuyÒn>/SetThanhCauMin2",
	"Chän: <Vâ §ang KiÕm>/SetThanhCauMin2",
	"Chän: <C«n L«n §ao>/SetThanhCauMin2",
	"Chän: <C«n L«n KiÕm>/SetThanhCauMin2",
	-- "Chän: <Hoa S¬n KhÝ>/SetThanhCauMin",
	-- "Chän: <Hoa S¬n KiÕm>/SetThanhCauMin",
	-- "Chän: <Vò Hån ThuÉn>/SetThanhCauMin",
	-- "Chän: <Vò Hån §ao>/SetThanhCauMin",
	"Quay L¹i/ThanhCau",
	"KÕt Thóc/No")
end
function SetThanhCauMin(nSel)
		for i=904, 913 do
			AddItem2(2,0,10*nSel + i  ,0,0,0,1)
		end
end
function SetThanhCauMin2(nSel)
		for i=1054, 1063 do
			AddItem2(2,0,10*nSel + i  ,0,0,0,1)
		end
end
----------------------------------------------------------------------
function VanLoc()
Say("H·y Chän M«n Ph¸i",16,
	"Chän: <Thiªu L©m QuyÒn>/SetVanLocMin",
	"Chän: <Thiªu L©m Bæng>/SetVanLocMin",
	"Chän: <Thiªu L©m §ao>/SetVanLocMin",
	"Chän: <Thiªn V­¬ng Chïy>/SetVanLocMin",
	"Chän: <Thiªn V­¬ng Th­¬ng>/SetVanLocMin",
	"Chän: <Thiªn V­¬ng §ao>/SetVanLocMin",
	"Chän: <Nga My KiÕm>/SetVanLocMin",
	"Chän: <Nga My Ch­ëng>/SetVanLocMin",
	"Chän: <Thóy Yªn §ao>/SetVanLocMin",
	"Chän: <Thóy Yªn Néi>/SetVanLocMin",
	"Chän: <Ngò §éc Ch­ëng>/SetVanLocMin",
	"Chän: <Ngò §éc §ao>/SetVanLocMin",
	"Chän: <§­êng M«n Phi §ao>/SetVanLocMin",
	"Chän: <§­êng M«n Ná TiÔn>/SetVanLocMin",
	"Chän: <§­êng M«n Phi Tiªu>/SetVanLocMin",
	"Trang KÕ/VanLocs")
end

function VanLocs()
Say("H·y Chän M«n Ph¸i",10,
	"Chän: <C¸i Bang Ch­ëng>/SetVanLocMin2",
	"Chän: <C¸i Bang Bæng>/SetVanLocMin2",
	"Chän: <Thiªn NhÉn KÝch>/SetVanLocMin2",
	"Chän: <Thiªn NhÉn §ao>/SetVanLocMin2",
	"Chän: <Vâ §ang QuyÒn>/SetVanLocMin2",
	"Chän: <Vâ §ang KiÕm>/SetVanLocMin2",
	"Chän: <C«n L«n §ao>/SetVanLocMin2",
	"Chän: <C«n L«n KiÕm>/SetVanLocMin2",
	-- "Chän: <Hoa S¬n KhÝ>/SetVanLocMin2",
	-- "Chän: <Hoa S¬n KiÕm>/SetVanLocMin2",
	-- "Chän: <Vò Hån ThuÉn>/SetVanLocMin2",
	-- "Chän: <Vò Hån §ao>/SetVanLocMin2",
	"Quay L¹i/VanLoc",
	"KÕt Thóc/No")
end

function SetVanLocMin(nSel)
		for i=1134,  1143 do
				AddItem2(2,0,10*nSel + i  ,0,0,0,1)
		end
end
function SetVanLocMin2(nSel)
		for i=1284, 1293 do
			AddItem2(2,0,10*nSel + i  ,0,0,0,1)
		end
end
---------------------------------------------------------------------
function ThuongLang()
Say("H·y Chän M«n Ph¸i",16,
	"Chän: <Thiªu L©m QuyÒn>/SetThuongLangMin",
	"Chän: <Thiªu L©m Bæng>/SetThuongLangMin",
	"Chän: <Thiªu L©m §ao>/SetThuongLangMin",
	"Chän: <Thiªn V­¬ng Chïy>/SetThuongLangMin",
	"Chän: <Thiªn V­¬ng Th­¬ng>/SetThuongLangMin",
	"Chän: <Thiªn V­¬ng §ao>/SetThuongLangMin",
	"Chän: <Nga My KiÕm>/SetThuongLangMin",
	"Chän: <Nga My Ch­ëng>/SetThuongLangMin",
	"Chän: <Thóy Yªn §ao>/SetThuongLangMin",
	"Chän: <Thóy Yªn Néi>/SetThuongLangMin",
	"Chän: <Ngò §éc Ch­ëng>/SetThuongLangMin",
	"Chän: <Ngò §éc §ao>/SetThuongLangMin",
	"Chän: <§­êng M«n Phi §ao>/SetThuongLangMin",
	"Chän: <§­êng M«n Ná TiÔn>/SetThuongLangMin",
	"Chän: <§­êng M«n Phi Tiªu>/SetThuongLangMin",
	"Trang KÕ/ThuongLangs")
end

function ThuongLangs()
Say("H·y Chän M«n Ph¸i",10,
	"Chän: <C¸i Bang Ch­ëng>/SetThuongLangMin2",
	"Chän: <C¸i Bang Bæng>/SetThuongLangMin2",
	"Chän: <Thiªn NhÉn KÝch>/SetThuongLangMin2",
	"Chän: <Thiªn NhÉn §ao>/SetThuongLangMin2",
	"Chän: <Vâ §ang QuyÒn>/SetThuongLangMin2",
	"Chän: <Vâ §ang KiÕm>/SetThuongLangMin2",
	"Chän: <C«n L«n §ao>/SetThuongLangMin2",
	"Chän: <C«n L«n KiÕm>/SetThuongLangMin2",
	-- "Chän: <Hoa S¬n KhÝ>/SetThuongLangMin",
	-- "Chän: <Hoa S¬n KiÕm>/SetThuongLangMin",
	-- "Chän: <Vò Hån ThuÉn>/SetThuongLangMin",
	-- "Chän: <Vò Hån §ao>/SetThuongLangMin",
	"Quay L¹i/ThuongLang",
	"KÕt Thóc/No")
end
function SetThuongLangMin(nSel)
		for i=1364,  1373 do
			AddItem2(2,0,10*nSel + i  ,0,0,0,1)
		end
end
function SetThuongLangMin2(nSel)
		for i=1514,  1523 do
			AddItem2(2,0,10*nSel + i  ,0,0,0,1)
		end
end
---------------------------------------------------------------------
function HuyenVien()
Say("H·y Chän M«n Ph¸i",16,
	"Chän: <Thiªu L©m QuyÒn>/SetHuyenVienMin",
	"Chän: <Thiªu L©m Bæng>/SetHuyenVienMin",
	"Chän: <Thiªu L©m §ao>/SetHuyenVienMin",
	"Chän: <Thiªn V­¬ng Chïy>/SetHuyenVienMin",
	"Chän: <Thiªn V­¬ng Th­¬ng>/SetHuyenVienMin",
	"Chän: <Thiªn V­¬ng §ao>/SetHuyenVienMin",
	"Chän: <Nga My KiÕm>/SetHuyenVienMin",
	"Chän: <Nga My Ch­ëng>/SetHuyenVienMin",
	"Chän: <Thóy Yªn §ao>/SetHuyenVienMin",
	"Chän: <Thóy Yªn Néi>/SetHuyenVienMin",
	"Chän: <Ngò §éc Ch­ëng>/SetHuyenVienMin",
	"Chän: <Ngò §éc §ao>/SetHuyenVienMin",
	"Chän: <§­êng M«n Phi §ao>/SetHuyenVienMin",
	"Chän: <§­êng M«n Ná TiÔn>/SetHuyenVienMin",
	"Chän: <§­êng M«n Phi Tiªu>/SetHuyenVienMin",
	"Trang KÕ/HuyenViens")
end

function HuyenViens()
Say("H·y Chän M«n Ph¸i",10,
	"Chän: <C¸i Bang Ch­ëng>/SetHuyenVienMin2",
	"Chän: <C¸i Bang Bæng>/SetHuyenVienMin2",
	"Chän: <Thiªn NhÉn KÝch>/SetHuyenVienMin2",
	"Chän: <Thiªn NhÉn §ao>/SetHuyenVienMin2",
	"Chän: <Vâ §ang QuyÒn>/SetHuyenVienMin2",
	"Chän: <Vâ §ang KiÕm>/SetHuyenVienMin2",
	"Chän: <C«n L«n §ao>/SetHuyenVienMin2",
	"Chän: <C«n L«n KiÕm>/SetHuyenVienMin2",
	-- "Chän: <Hoa S¬n KhÝ>/SetHuyenVienMin2",
	-- "Chän: <Hoa S¬n KiÕm>/SetHuyenVienMin2",
	-- "Chän: <Vò Hån ThuÉn>/SetHuyenVienMin2",
	-- "Chän: <Vò Hån §ao>/SetHuyenVienMin2",
	"Quay L¹i/HuyenVien",
	"KÕt Thóc/No")
end

function SetHuyenVienMin(nSel)
		for i=1594,  1603 do
			AddItem2(2,0,10*nSel + i  ,0,0,0,1)
		end
end
function SetHuyenVienMin2(nSel)
		for i=1744, 1753 do
			AddItem2(2,0,10*nSel + i  ,0,0,0,1)
		end
end
---------------------------------------------------------------------
function TuMang()
Say("H·y Chän M«n Ph¸i",16,
	"Chän: <Thiªu L©m QuyÒn>/SetTuMangMin",
	"Chän: <Thiªu L©m Bæng>/SetTuMangMin",
	"Chän: <Thiªu L©m §ao>/SetTuMangMin",
	"Chän: <Thiªn V­¬ng Chïy>/SetTuMangMin",
	"Chän: <Thiªn V­¬ng Th­¬ng>/SetTuMangMin",
	"Chän: <Thiªn V­¬ng §ao>/SetTuMangMin",
	"Chän: <Nga My KiÕm>/SetTuMangMin",
	"Chän: <Nga My Ch­ëng>/SetTuMangMin",
	"Chän: <Thóy Yªn §ao>/SetTuMangMin",
	"Chän: <Thóy Yªn Néi>/SetTuMangMin",
	"Chän: <Ngò §éc Ch­ëng>/SetTuMangMin",
	"Chän: <Ngò §éc §ao>/SetTuMangMin",
	"Chän: <§­êng M«n Phi §ao>/SetTuMangMin",
	"Chän: <§­êng M«n Ná TiÔn>/SetTuMangMin",
	"Chän: <§­êng M«n Phi Tiªu>/SetTuMangMin",
	"Trang KÕ/TuMangs")
end

function TuMangs()
Say("H·y Chän M«n Ph¸i",10,
	"Chän: <C¸i Bang Ch­ëng>/SetTuMangMin2",
	"Chän: <C¸i Bang Bæng>/SetTuMangMin2",
	"Chän: <Thiªn NhÉn KÝch>/SetTuMangMin2",
	"Chän: <Thiªn NhÉn §ao>/SetTuMangMin2",
	"Chän: <Vâ §ang QuyÒn>/SetTuMangMin2",
	"Chän: <Vâ §ang KiÕm>/SetTuMangMin2",
	"Chän: <C«n L«n §ao>/SetTuMangMin2",
	"Chän: <C«n L«n KiÕm>/SetTuMangMin2",
	-- "Chän: <Hoa S¬n KhÝ>/SetTuMangMin2",
	-- "Chän: <Hoa S¬n KiÕm>/SetTuMangMin2",
	-- "Chän: <Vò Hån ThuÉn>/SetTuMangMin2",
	-- "Chän: <Vò Hån §ao>/SetTuMangMin2",
	"Quay L¹i/TuMang",
	"KÕt Thóc/No")
end

function SetTuMangMin(nSel)
		for i=1825, 1833 do
			AddItem2(2,0,10*nSel + i  ,0,0,0,1)
		end
end
function SetTuMangMin2(nSel)
		for i=1975, 1983 do
			AddItem2(2,0,10*nSel + i  ,0,0,0,1)
		end
end
---------------------------------------------------------------------
function KimO()
Say("H·y Chän M«n Ph¸i",16,
	"Chän: <Thiªu L©m QuyÒn>/SetKimOMin",
	"Chän: <Thiªu L©m Bæng>/SetKimOMin",
	"Chän: <Thiªu L©m §ao>/SetKimOMin",
	"Chän: <Thiªn V­¬ng Chïy>/SetKimOMin",
	"Chän: <Thiªn V­¬ng Th­¬ng>/SetKimOMin",
	"Chän: <Thiªn V­¬ng §ao>/SetKimOMin",
	"Chän: <Nga My KiÕm>/SetKimOMin",
	"Chän: <Nga My Ch­ëng>/SetKimOMin",
	"Chän: <Thóy Yªn §ao>/SetKimOMin",
	"Chän: <Thóy Yªn Néi>/SetKimOMin",
	"Chän: <Ngò §éc Ch­ëng>/SetKimOMin",
	"Chän: <Ngò §éc §ao>/SetKimOMin",
	"Chän: <§­êng M«n Phi §ao>/SetKimOMin",
	"Chän: <§­êng M«n Ná TiÔn>/SetKimOMin",
	"Chän: <§­êng M«n Phi Tiªu>/SetKimOMin",
	"Trang KÕ/KimOs")
end

function KimOs()
Say("H·y Chän M«n Ph¸i",10,
	"Chän: <C¸i Bang Ch­ëng>/SetKimOMin2",
	"Chän: <C¸i Bang Bæng>/SetKimOMin2",
	"Chän: <Thiªn NhÉn KÝch>/SetKimOMin2",
	"Chän: <Thiªn NhÉn §ao>/SetKimOMin2",
	"Chän: <Vâ §ang QuyÒn>/SetKimOMin2",
	"Chän: <Vâ §ang KiÕm>/SetKimOMin2",
	"Chän: <C«n L«n §ao>/SetKimOMin2",
	"Chän: <C«n L«n KiÕm>/SetKimOMin2",
	-- "Chän: <Hoa S¬n KhÝ>/SetKimOMin2",
	-- "Chän: <Hoa S¬n KiÕm>/SetKimOMin2",
	-- "Chän: <Vò Hån ThuÉn>/SetKimOMin2",
	-- "Chän: <Vò Hån §ao>/SetKimOMin2",
	"Quay L¹i/KimO",
	"KÕt Thóc/No")
end

function SetKimOMin(nSel)
		for i=2054, 2063 do
			AddItem2(2,0,10*nSel + i  ,0,0,0,1)
		end
end
function SetKimOMin2(nSel)
		for i=2204, 2213 do
			AddItem2(2,0,10*nSel + i  ,0,0,0,1)
		end
end


---------------------------------------------------------------------
function BachHo()
Say("H·y Chän M«n Ph¸i",16,
	"Chän: <Thiªu L©m QuyÒn>/SetBachHoMin",
	"Chän: <Thiªu L©m Bæng>/SetBachHoMin",
	"Chän: <Thiªu L©m §ao>/SetBachHoMin",
	"Chän: <Thiªn V­¬ng Chïy>/SetBachHoMin",
	"Chän: <Thiªn V­¬ng Th­¬ng>/SetBachHoMin",
	"Chän: <Thiªn V­¬ng §ao>/SetBachHoMin",
	"Chän: <Nga My KiÕm>/SetBachHoMin",
	"Chän: <Nga My Ch­ëng>/SetBachHoMin",
	"Chän: <Thóy Yªn §ao>/SetBachHoMin",
	"Chän: <Thóy Yªn Néi>/SetBachHoMin",
	"Chän: <Ngò §éc Ch­ëng>/SetBachHoMin",
	"Chän: <Ngò §éc §ao>/SetBachHoMin",
	"Chän: <§­êng M«n Phi §ao>/SetBachHoMin",
	"Chän: <§­êng M«n Ná TiÔn>/SetBachHoMin",
	"Chän: <§­êng M«n Phi Tiªu>/SetBachHoMin",
	"Trang KÕ/BachHos")
end

function BachHos()
Say("H·y Chän M«n Ph¸i",10,
	"Chän: <C¸i Bang Ch­ëng>/SetBachHoMin2",
	"Chän: <C¸i Bang Bæng>/SetBachHoMin2",
	"Chän: <Thiªn NhÉn KÝch>/SetBachHoMin2",
	"Chän: <Thiªn NhÉn §ao>/SetBachHoMin2",
	"Chän: <Vâ §ang QuyÒn>/SetBachHoMin2",
	"Chän: <Vâ §ang KiÕm>/SetBachHoMin2",
	"Chän: <C«n L«n §ao>/SetBachHoMin2",
	"Chän: <C«n L«n KiÕm>/SetBachHoMin2",
	-- "Chän: <Hoa S¬n KhÝ>/SetBachHoMin2",
	-- "Chän: <Hoa S¬n KiÕm>/SetBachHoMin2",
	-- "Chän: <Vò Hån ThuÉn>/SetBachHoMin2",
	-- "Chän: <Vò Hån §ao>/SetBachHoMin2",
	"Quay L¹i/BachHo",
	"KÕt Thóc/No")
end

function SetBachHoMin(nSel)
		for i=2284, 2293 do
			AddItem2(2,0,10*nSel + i  ,0,0,0,1)
		end
end
function SetBachHoMin2(nSel)
		for i=2434, 2443 do
			AddItem2(2,0,10*nSel + i  ,0,0,0,1)
		end
end

---------------------------------------------------------------------
function XichLan()
Say("H·y Chän M«n Ph¸i",16,
	"Chän: <Thiªu L©m QuyÒn>/SetXichLanMin",
	"Chän: <Thiªu L©m Bæng>/SetXichLanMin",
	"Chän: <Thiªu L©m §ao>/SetXichLanMin",
	"Chän: <Thiªn V­¬ng Chïy>/SetXichLanMin",
	"Chän: <Thiªn V­¬ng Th­¬ng>/SetXichLanMin",
	"Chän: <Thiªn V­¬ng §ao>/SetXichLanMin",
	"Chän: <Nga My KiÕm>/SetXichLanMin",
	"Chän: <Nga My Ch­ëng>/SetXichLanMin",
	"Chän: <Thóy Yªn §ao>/SetXichLanMin",
	"Chän: <Thóy Yªn Néi>/SetXichLanMin",
	"Chän: <Ngò §éc Ch­ëng>/SetXichLanMin",
	"Chän: <Ngò §éc §ao>/SetXichLanMin",
	"Chän: <§­êng M«n Phi §ao>/SetXichLanMin",
	"Chän: <§­êng M«n Ná TiÔn>/SetXichLanMin",
	"Chän: <§­êng M«n Phi Tiªu>/SetXichLanMin",
	"Trang KÕ/XichLans")
end

function XichLans()
Say("H·y Chän M«n Ph¸i",10,
	"Chän: <C¸i Bang Ch­ëng>/SetXichLanMin2",
	"Chän: <C¸i Bang Bæng>/SetXichLanMin2",
	"Chän: <Thiªn NhÉn KÝch>/SetXichLanMin2",
	"Chän: <Thiªn NhÉn §ao>/SetXichLanMin2",
	"Chän: <Vâ §ang QuyÒn>/SetXichLanMin2",
	"Chän: <Vâ §ang KiÕm>/SetXichLanMin2",
	"Chän: <C«n L«n §ao>/SetXichLanMin2",
	"Chän: <C«n L«n KiÕm>/SetXichLanMin2",
	"Quay L¹i/XichLan",
	"KÕt Thóc/No")
end

function SetXichLanMin(nSel)
	for i=2514, 2523 do
		AddItem2(2, 0, 10 * nSel + i, 0, 0, 0)
	end
end

function SetXichLanMin2(nSel)
	for i=2664, 2673 do
		AddItem2(2, 0, 10 * nSel + i, 0, 0, 0)
	end
end


---------------------------------------------------------------------
function MinhPhuong()
Say("H·y Chän M«n Ph¸i",16,
	"Chän: <Thiªu L©m QuyÒn>/SetMinhPhuongMin",
	"Chän: <Thiªu L©m Bæng>/SetMinhPhuongMin",
	"Chän: <Thiªu L©m §ao>/SetMinhPhuongMin",
	"Chän: <Thiªn V­¬ng Chïy>/SetMinhPhuongMin",
	"Chän: <Thiªn V­¬ng Th­¬ng>/SetMinhPhuongMin",
	"Chän: <Thiªn V­¬ng §ao>/SetMinhPhuongMin",
	"Chän: <Nga My KiÕm>/SetMinhPhuongMin",
	"Chän: <Nga My Ch­ëng>/SetMinhPhuongMin",
	"Chän: <Thóy Yªn §ao>/SetMinhPhuongMin",
	"Chän: <Thóy Yªn Néi>/SetMinhPhuongMin",
	"Chän: <Ngò §éc Ch­ëng>/SetMinhPhuongMin",
	"Chän: <Ngò §éc §ao>/SetMinhPhuongMin",
	"Chän: <§­êng M«n Phi §ao>/SetMinhPhuongMin",
	"Chän: <§­êng M«n Ná TiÔn>/SetMinhPhuongMin",
	"Chän: <§­êng M«n Phi Tiªu>/SetMinhPhuongMin",
	"Trang KÕ/MinhPhuongs")
end

function MinhPhuongs()
Say("H·y Chän M«n Ph¸i",10,
	"Chän: <C¸i Bang Ch­ëng>/SetMinhPhuongMin2",
	"Chän: <C¸i Bang Bæng>/SetMinhPhuongMin2",
	"Chän: <Thiªn NhÉn KÝch>/SetMinhPhuongMin2",
	"Chän: <Thiªn NhÉn §ao>/SetMinhPhuongMin2",
	"Chän: <Vâ §ang QuyÒn>/SetMinhPhuongMin2",
	"Chän: <Vâ §ang KiÕm>/SetMinhPhuongMin2",
	"Chän: <C«n L«n §ao>/SetMinhPhuongMin2",
	"Chän: <C«n L«n KiÕm>/SetMinhPhuongMin2",
	-- "Chän: <Hoa S¬n KhÝ>/SetMinhPhuongMin2",
	-- "Chän: <Hoa S¬n KiÕm>/SetMinhPhuongMin2",
	-- "Chän: <Vò Hån ThuÉn>/SetMinhPhuongMin2",
	-- "Chän: <Vò Hån §ao>/SetMinhPhuongMin2",
	"Quay L¹i/MinhPhuong",
	"KÕt Thóc/No")
end

function SetMinhPhuongMin(nSel)
	for i=2744, 2753 do
		AddItem2(2, 0, 10 * nSel + i, 0, 0, 0)
	end
end

function SetMinhPhuongMin2(nSel)
	for i=2894, 2903 do
		AddItem2(2, 0, 10 * nSel + i, 0, 0, 0)
	end
end

---------------------------------------------------------------------
function DangLong()
Say("H·y Chän M«n Ph¸i",16,
	"Chän: <Thiªu L©m QuyÒn>/SetDangLongMin",
	"Chän: <Thiªu L©m Bæng>/SetDangLongMin",
	"Chän: <Thiªu L©m §ao>/SetDangLongMin",
	"Chän: <Thiªn V­¬ng Chïy>/SetDangLongMin",
	"Chän: <Thiªn V­¬ng Th­¬ng>/SetDangLongMin",
	"Chän: <Thiªn V­¬ng §ao>/SetDangLongMin",
	"Chän: <Nga My KiÕm>/SetDangLongMin",
	"Chän: <Nga My Ch­ëng>/SetDangLongMin",
	"Chän: <Thóy Yªn §ao>/SetDangLongMin",
	"Chän: <Thóy Yªn Néi>/SetDangLongMin",
	"Chän: <Ngò §éc Ch­ëng>/SetDangLongMin",
	"Chän: <Ngò §éc §ao>/SetDangLongMin",
	"Chän: <§­êng M«n Phi §ao>/SetDangLongMin",
	"Chän: <§­êng M«n Ná TiÔn>/SetDangLongMin",
	"Chän: <§­êng M«n Phi Tiªu>/SetDangLongMin",
	"Trang KÕ/DangLongs")
end

function DangLongs()
Say("H·y Chän M«n Ph¸i",10,
	"Chän: <C¸i Bang Ch­ëng>/SetDangLongMin2",
	"Chän: <C¸i Bang Bæng>/SetDangLongMin2",
	"Chän: <Thiªn NhÉn KÝch>/SetDangLongMin2",
	"Chän: <Thiªn NhÉn §ao>/SetDangLongMin2",
	"Chän: <Vâ §ang QuyÒn>/SetDangLongMin2",
	"Chän: <Vâ §ang KiÕm>/SetDangLongMin2",
	"Chän: <C«n L«n §ao>/SetDangLongMin2",
	"Chän: <C«n L«n KiÕm>/SetDangLongMin2",
	-- "Chän: <Hoa S¬n KhÝ>/SetDangLongMin2",
	-- "Chän: <Hoa S¬n KiÕm>/SetDangLongMin2",
	-- "Chän: <Vò Hån ThuÉn>/SetDangLongMin2",
	-- "Chän: <Vò Hån §ao>/SetDangLongMin2",
	"Quay L¹i/DangLong",
	"KÕt Thóc/No")
end
function SetDangLongMin(nSel)
	for i=2974, 2983 do
		AddItem2(2, 0, 10 * nSel + i, 0, 0, 0)
	end
end

function SetDangLongMin2(nSel)
	for i=3124, 3133 do
		AddItem2(2, 0, 10 * nSel + i, 0, 0, 0)
	end
end



---------------------------------------------------------------------
function TinhSuong()
Say("H·y Chän M«n Ph¸i",16,
	"Chän: <Thiªu L©m QuyÒn>/SetTinhSuongMin",
	"Chän: <Thiªu L©m Bæng>/SetTinhSuongMin",
	"Chän: <Thiªu L©m §ao>/SetTinhSuongMin",
	"Chän: <Thiªn V­¬ng Chïy>/SetTinhSuongMin",
	"Chän: <Thiªn V­¬ng Th­¬ng>/SetTinhSuongMin",
	"Chän: <Thiªn V­¬ng §ao>/SetTinhSuongMin",
	"Chän: <Nga My KiÕm>/SetTinhSuongMin",
	"Chän: <Nga My Ch­ëng>/SetTinhSuongMin",
	"Chän: <Thóy Yªn §ao>/SetTinhSuongMin)",
	"Chän: <Thóy Yªn Néi>/SetTinhSuongMin",
	"Chän: <Ngò §éc Ch­ëng>/SetTinhSuongMin",
	"Chän: <Ngò §éc §ao>/SetTinhSuongMin",
	"Chän: <§­êng M«n Phi §ao>/SetTinhSuongMin",
	"Chän: <§­êng M«n Ná TiÔn>/SetTinhSuongMin",
	"Chän: <§­êng M«n Phi Tiªu>/SetTinhSuongMin",
	"Trang KÕ/TinhSuongs")
end

function TinhSuongs()
Say("H·y Chän M«n Ph¸i",10,
	"Chän: <C¸i Bang Ch­ëng>/SetTinhSuongMin2",
	"Chän: <C¸i Bang Bæng>/SetTinhSuongMin2",
	"Chän: <Thiªn NhÉn KÝch>/SetTinhSuongMin2",
	"Chän: <Thiªn NhÉn §ao>/SetTinhSuongMin2",
	"Chän: <Vâ §ang QuyÒn>/SetTinhSuongMin2",
	"Chän: <Vâ §ang KiÕm>/SetTinhSuongMin2",
	"Chän: <C«n L«n §ao>/SetTinhSuongMin2",
	"Chän: <C«n L«n KiÕm>/SetTinhSuongMin2",
	-- "Chän: <Hoa S¬n KhÝ>/SetTinhSuongMin2",
	-- "Chän: <Hoa S¬n KiÕm>/SetTinhSuongMin2",
	-- "Chän: <Vò Hån ThuÉn>/SetTinhSuongMin2",
	-- "Chän: <Vò Hån §ao>/SetTinhSuongMin2",
	"Quay L¹i/TinhSuong",
	"KÕt Thóc/No")
end
function SetTinhSuongMin(nSel)
	for i=5378, 5387 do
		AddItem2(2, 0, 10 * nSel + i, 0, 0, 0)
	end
end

function SetTinhSuongMin2(nSel)
	for i=5528, 5537 do
		AddItem2(2, 0, 10 * nSel + i, 0, 0, 0)
	end
end


---------------------------------------------------------------------
function NguyetKhuyet()
Say("H·y Chän M«n Ph¸i",16,
	"Chän: <Thiªu L©m QuyÒn>/SetNguyetKhuyetMin",
	"Chän: <Thiªu L©m Bæng>/SetNguyetKhuyetMin",
	"Chän: <Thiªu L©m §ao>/SetNguyetKhuyetMin",
	"Chän: <Thiªn V­¬ng Chïy>/SetNguyetKhuyetMin",
	"Chän: <Thiªn V­¬ng Th­¬ng>/SetNguyetKhuyetMin",
	"Chän: <Thiªn V­¬ng §ao>/SetNguyetKhuyetMin",
	"Chän: <Nga My KiÕm>/SetNguyetKhuyetMin",
	"Chän: <Nga My Ch­ëng>/SetNguyetKhuyetMin",
	"Chän: <Thóy Yªn §ao>/SetNguyetKhuyetMin",
	"Chän: <Thóy Yªn Néi>/SetNguyetKhuyetMin",
	"Chän: <Ngò §éc Ch­ëng>/SetNguyetKhuyetMin",
	"Chän: <Ngò §éc §ao>/SetNguyetKhuyetMin",
	"Chän: <§­êng M«n Phi §ao>/SetNguyetKhuyetMin",
	"Chän: <§­êng M«n Ná TiÔn>/SetNguyetKhuyetMin",
	"Chän: <§­êng M«n Phi Tiªu>/SetNguyetKhuyetMin",
	"Trang KÕ/NguyetKhuyets")
end

function NguyetKhuyets()
Say("H·y Chän M«n Ph¸i",14,
	"Chän: <C¸i Bang Ch­ëng>/SetNguyetKhuyetMin2",
	"Chän: <C¸i Bang Bæng>/SetNguyetKhuyetMin2",
	"Chän: <Thiªn NhÉn KÝch>/SetNguyetKhuyetMin2",
	"Chän: <Thiªn NhÉn §ao>/SetNguyetKhuyetMin2",
	"Chän: <Vâ §ang QuyÒn>/SetNguyetKhuyetMin2",
	"Chän: <Vâ §ang KiÕm>/SetNguyetKhuyetMin2",
	"Chän: <C«n L«n §ao>/SetNguyetKhuyetMin2",
	"Chän: <C«n L«n KiÕm>/SetNguyetKhuyetMin2",
	-- "Chän: <Hoa S¬n KhÝ>/SetNguyetKhuyetMin2",
	-- "Chän: <Hoa S¬n KiÕm>/SetNguyetKhuyetMin2",
	-- "Chän: <Vò Hån ThuÉn>/SetNguyetKhuyetMin2",
	-- "Chän: <Vò Hån §ao>/SetNguyetKhuyetMin2",
	"Quay L¹i/NguyetKhuyet",
	"KÕt Thóc/No")
end
function SetNguyetKhuyetMin(nSel)
	for i=5669, 5678 do
		AddItem2(2, 0, 10 * nSel + i, 0, 0, 0)
	end
end

function SetNguyetKhuyetMin2(nSel)
	for i=5819, 5828 do
		AddItem2(2, 0, 10 * nSel + i, 0, 0, 0)
	end
end



---------------------------------------------------------------------
function DieuDuong()
Say("H·y Chän M«n Ph¸i",16,
	"Chän: <Thiªu L©m QuyÒn>/SetDieuDuongMin",
	"Chän: <Thiªu L©m Bæng>/SetDieuDuongMin",
	"Chän: <Thiªu L©m §ao>/SetDieuDuongMin",
	"Chän: <Thiªn V­¬ng Chïy>/SetDieuDuongMin",
	"Chän: <Thiªn V­¬ng Th­¬ng>/SetDieuDuongMin",
	"Chän: <Thiªn V­¬ng §ao>/SetDieuDuongMin",
	"Chän: <Nga My KiÕm>/SetDieuDuongMin",
	"Chän: <Nga My Ch­ëng>/SetDieuDuongMin",
	"Chän: <Thóy Yªn §ao>/SetDieuDuongMin",
	"Chän: <Thóy Yªn Néi>/SetDieuDuongMin",
	"Chän: <Ngò §éc Ch­ëng>/SetDieuDuongMin",
	"Chän: <Ngò §éc §ao>/SetDieuDuongMin",
	"Chän: <§­êng M«n Phi §ao>/SetDieuDuongMin",
	"Chän: <§­êng M«n Ná TiÔn>/SetDieuDuongMin",
	"Chän: <§­êng M«n Phi Tiªu>/SetDieuDuongMin",
	"Trang KÕ/DieuDuongs")
end

function DieuDuongs()
Say("H·y Chän M«n Ph¸i",10,
	"Chän: <C¸i Bang Ch­ëng>/SetDieuDuongMin2",
	"Chän: <C¸i Bang Bæng>/SetDieuDuongMin2",
	"Chän: <Thiªn NhÉn KÝch>/SetDieuDuongMin2",
	"Chän: <Thiªn NhÉn §ao>/SetDieuDuongMin2",
	"Chän: <Vâ §ang QuyÒn>/SetDieuDuongMin2",
	"Chän: <Vâ §ang KiÕm>/SetDieuDuongMin2",
	"Chän: <C«n L«n §ao>/SetDieuDuongMin2",
	"Chän: <C«n L«n KiÕm>/SetDieuDuongMin2",
	-- "Chän: <Hoa S¬n KhÝ>/SetDieuDuongMin2",
	-- "Chän: <Hoa S¬n KiÕm>/SetDieuDuongMin2",
	-- "Chän: <Vò Hån ThuÉn>/SetDieuDuongMin2",
	-- "Chän: <Vò Hån §ao>/SetDieuDuongMin2",
	"Quay L¹i/DieuDuong",
	"KÕt Thóc/No")
end


function SetDieuDuongMin(nSel)
	for i=5961, 5970 do
		AddItem2(2, 0, 10 * nSel + i, 0, 0, 0)
	end
end

function SetDieuDuongMin2(nSel)
	for i=6111, 6120 do
		AddItem2(2, 0, 10 * nSel + i, 0, 0, 0)
	end
end



--------------------------------------------------------------
function AnhHao()
Say("H·y Chän M«n Ph¸i",16,
	"Chän: <Thiªu L©m QuyÒn>/SetAnhHaoMin",
	"Chän: <Thiªu L©m Bæng>/SetAnhHaoMin",
	"Chän: <Thiªu L©m §ao>/SetAnhHaoMin",
	"Chän: <Thiªn V­¬ng Chïy>/SetAnhHaoMin",
	"Chän: <Thiªn V­¬ng Th­¬ng>/SetAnhHaoMin",
	"Chän: <Thiªn V­¬ng §ao>/SetAnhHaoMin",
	"Chän: <Nga My KiÕm>/SetAnhHaoMin",
	"Chän: <Nga My Ch­ëng>/SetAnhHaoMin",
	"Chän: <Thóy Yªn §ao>/SetAnhHaoMin",
	"Chän: <Thóy Yªn Néi>/SetAnhHaoMin",
	"Chän: <Ngò §éc Ch­ëng>/SetAnhHaoMin",
	"Chän: <Ngò §éc §ao>/SetAnhHaoMin",
	"Chän: <§­êng M«n Phi §ao>/SetAnhHaoMin",
	"Chän: <§­êng M«n Ná TiÔn>/SetAnhHaoMin",
	"Chän: <§­êng M«n Phi Tiªu>/SetAnhHaoMin",
	"Trang KÕ/AnhHaos")
end

function AnhHaos()
Say("H·y Chän M«n Ph¸i",10,
	"Chän: <C¸i Bang Ch­ëng>/SetAnhHaoMin2",
	"Chän: <C¸i Bang Bæng>/SetAnhHaoMin2",
	"Chän: <Thiªn NhÉn KÝch>/SetAnhHaoMin2",
	"Chän: <Thiªn NhÉn §ao>/SetAnhHaoMin2",
	"Chän: <Vâ §ang QuyÒn>/SetAnhHaoMin2",
	"Chän: <Vâ §ang KiÕm>/SetAnhHaoMin2",
	"Chän: <C«n L«n §ao>/SetAnhHaoMin2",
	"Chän: <C«n L«n KiÕm>/SetAnhHaoMin2",
	-- "Chän: <Hoa S¬n KhÝ>/SetAnhHaoMin2",
	-- "Chän: <Hoa S¬n KiÕm>/SetAnhHaoMin2",
	-- "Chän: <Vò Hån ThuÉn>/SetAnhHaoMin2",
	-- "Chän: <Vò Hån §ao>/SetAnhHaoMin2",
	"Quay L¹i/AnhHao",
	"KÕt Thóc/No")
end

function SetAnhHaoMin(nSel)
	for i=6282, 6291 do
		AddItem2(2, 0, 10 * nSel + i, 0, 0, 0)
	end
end

function SetAnhHaoMin2(nSel)
	for i=6432, 6441 do
		AddItem2(2, 0, 10 * nSel + i, 0, 0, 0)
	end
end



--------------------------------------------------------------------------
function ThienMenh()
Say("H·y Chän M«n Ph¸i",16,
	"Chän: <Thiªu L©m QuyÒn>/SetThienMenhMin",
	"Chän: <Thiªu L©m Bæng>/SetThienMenhMin",
	"Chän: <Thiªu L©m §ao>/SetThienMenhMin",
	"Chän: <Thiªn V­¬ng Chïy>/SetThienMenhMin",
	"Chän: <Thiªn V­¬ng Th­¬ng>/SetThienMenhMin",
	"Chän: <Thiªn V­¬ng §ao>/SetThienMenhMin",
	"Chän: <Nga My KiÕm>/SetThienMenhMin",
	"Chän: <Nga My Ch­ëng>/SetThienMenhMin",
	"Chän: <Thóy Yªn §ao>/SetThienMenhMin",
	"Chän: <Thóy Yªn Néi>/SetThienMenhMin",
	"Chän: <Ngò §éc Ch­ëng>/SetThienMenhMin",
	"Chän: <Ngò §éc §ao>/SetThienMenhMin",
	"Chän: <§­êng M«n Phi §ao>/SetThienMenhMin",
	"Chän: <§­êng M«n Ná TiÔn>/SetThienMenhMin",
	"Chän: <§­êng M«n Phi Tiªu>/SetThienMenhMin",
	"Trang KÕ/ThienMenhs")
end

function ThienMenhs()
Say("H·y Chän M«n Ph¸i",10,
	"Chän: <C¸i Bang Ch­ëng>/SetThienMenhMin2",
	"Chän: <C¸i Bang Bæng>/SetThienMenhMin2",
	"Chän: <Thiªn NhÉn KÝch>/SetThienMenhMin2",
	"Chän: <Thiªn NhÉn §ao>/SetThienMenhMin2",
	"Chän: <Vâ §ang QuyÒn>/SetThienMenhMin2",
	"Chän: <Vâ §ang KiÕm>/SetThienMenhMin2",
	"Chän: <C«n L«n §ao>/SetThienMenhMin2",
	"Chän: <C«n L«n KiÕm>/SetThienMenhMin2",
	-- "Chän: <Hoa S¬n KhÝ>/SetThienMenhMin2",
	-- "Chän: <Hoa S¬n KiÕm>/SetThienMenhMin2",
	-- "Chän: <Vò Hån ThuÉn>/SetThienMenhMin2",
	-- "Chän: <Vò Hån §ao>/SetThienMenhMin2",
	"Quay L¹i/ThienMenh",
	"KÕt Thóc/No")
end

function SetThienMenhMin(nSel)
	for i=6807, 6816 do
		AddItem2(2, 0, 10 * nSel + i, 0, 0, 0)
	end
end

function SetThienMenhMin2(nSel)
	for i=6957, 6966 do
		AddItem2(2, 0, 10 * nSel + i, 0, 0, 0)
	end
end


