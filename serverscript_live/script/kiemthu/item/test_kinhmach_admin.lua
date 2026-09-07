-- ============================================================================
-- BO TEST KINH MACH cho Lenh bai Admin (soan 27/08/2026).
-- Chi dung ham Lua co that trong cay: SetMeridian / GetTask / SetTask /
-- AddItem / Msg2Player / Say / SayEx.
-- Ma mach truyen cho SetMeridian DEM TU 0 (mach 1 = 0), giong setmeridian.lua.
-- ============================================================================
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\player\\meridian_data.lua")

KMT_TASK_ZY   = 362    -- diem Chan Nguyen (mach 1-8)
KMT_TASK_XY   = 4318   -- diem Huyen Nguyen (mach 9-12)
KMT_TASK_MACH = 4001   -- 4001..4008 cap mach 1-8; 4319..4322 cap mach 9-12
KMT_MACH = 1           -- mach dang chon

KMT_TEN = {
	[1] = "§èc m¹ch",
	[2] = "Nh©m m¹ch",
	[3] = "Xung m¹ch",
	[4] = "§íi m¹ch",
	[5] = "¢m duy m¹ch",
	[6] = "D­¬ng duy m¹ch",
	[7] = "¢m khiªu m¹ch",
	[8] = "D­¬ng khiªu m¹ch",
	[9] = "Thñ Tam ¢m Kinh",
	[10] = "Thñ Tam D­¬ng Kinh",
	[11] = "Tóc Tam ¢m Kinh",
	[12] = "Tóc Tam D­¬ng Kinh",
};

KMT_BAODAY = {
	[1]={4440,1}, [2]={4440,2}, [3]={4440,3}, [4]={4440,4},
	[5]={4441,1}, [6]={4441,2}, [7]={4441,3}, [8]={4441,4},
	[9]={4491,1}, [10]={4491,2},[11]={4491,3},[12]={4491,4},
};

KMT_VATPHAM = {
	[1] = {4844, "Hé M¹ch §¬n"},
	[2] = {4870, "§¹i Hé M¹ch §¬n"},
	[3] = {4871, "§Þnh M¹ch §an"},
	[4] = {4872, "§Þnh M¹ch §an (Lv1)"},
	[5] = {4848, "HuyÕt Long §»ng"},
	[6] = {4849, "HuyÕt Long §¬n"},
	[7] = {4873, "KhÝ Doanh §an"},
	[8] = {4846, "Ch©n Nguyªn §¬n (trung)"},
	[9] = {4847, "Ch©n Nguyªn §¬n (®¹i)"},
};

-- cap hien tai cua mot mach, doc tu bien nhiem vu (may chu ghi khi SetMeridian)
function KMT_LayCap(nMach)
	-- [27/08b] doc cap THAT bang binding GetMeridian; du phong DLL cu thi ve 0
	if GetMeridian ~= nil then
		return GetMeridian(nMach - 1)
	end
	return 0
end

function KMT_Pow256(n)
	local d = 1
	local i = 1
	while i < n do
		d = d * 256
		i = i + 1
	end
	return d
end

function KMT_LayBaoDay(nMach)
	local bd = KMT_BAODAY[nMach]
	if bd == nil then return 0 end
	local d = KMT_Pow256(bd[2])
	return mod(floor(GetTask(bd[1]) / d), 256)
end

-- ---------------- menu goc ----------------
function KM_TestRoot()
	SayEx({ "Bé test kinh m¹ch. Chän viÖc cÇn lµm:",
		"Chän kinh m¹ch ®Ó chØnh cÊp/KM_ChonMach",
		"CÊp ®iÓm Ch©n Nguyªn - HuyÒn Nguyªn/KM_CapDiem",
		"CÊp vËt phÈm xung huyÖt/KM_CapVatPham",
		"Xem tr¹ng th¸i 12 kinh m¹ch/KM_XemTrangThai",
		"Xo¸ s¹ch 12 kinh m¹ch (vÒ cÊp 0)/KM_XoaSach",
		"Quay l¹i lÖnh bµi/chucnangadmin",
		"KÕt thóc ®èi tho¹i/no"})
end

-- ---------------- chon mach ----------------
function KM_ChonMach()
	local tb = {}
	tb[1] = "Chän kinh m¹ch muèn chØnh cÊp:"
	local i = 1
	while i <= 12 do
		tb[i + 1] = format("%s (%s: %s)/KM_Chon%s", KMT_TEN[i], "cÊp", KMT_LayCap(i), i)
		i = i + 1
	end
	tb[14] = "Quay l¹i/KM_TestRoot"
	SayEx(tb)
end

function KM_Chon1() KMT_MACH = 1 KM_MenuCap() end
function KM_Chon2() KMT_MACH = 2 KM_MenuCap() end
function KM_Chon3() KMT_MACH = 3 KM_MenuCap() end
function KM_Chon4() KMT_MACH = 4 KM_MenuCap() end
function KM_Chon5() KMT_MACH = 5 KM_MenuCap() end
function KM_Chon6() KMT_MACH = 6 KM_MenuCap() end
function KM_Chon7() KMT_MACH = 7 KM_MenuCap() end
function KM_Chon8() KMT_MACH = 8 KM_MenuCap() end
function KM_Chon9() KMT_MACH = 9 KM_MenuCap() end
function KM_Chon10() KMT_MACH = 10 KM_MenuCap() end
function KM_Chon11() KMT_MACH = 11 KM_MenuCap() end
function KM_Chon12() KMT_MACH = 12 KM_MenuCap() end

function KM_MenuCap()
	local nM = KMT_MACH
	SayEx({
		format("%s: %s (%s %s)", "§ang chØnh", KMT_TEN[nM], "cÊp", KMT_LayCap(nM)),
		"T¨ng 1 cÊp/KM_Cong1",
		"T¨ng 5 cÊp/KM_Cong5",
		"§Æt th¼ng cÊp 16/KM_Dat16",
		"§Æt th¼ng cÊp 31/KM_Dat31",
		"§Æt th¼ng cÊp 32 (nhËn th­ëng trän bé)/KM_Dat32",
		"VÒ cÊp 0/KM_Ve0",
		"Chän m¹ch kh¸c/KM_ChonMach",
		"Quay l¹i/KM_TestRoot"})
end

function KMT_DatCap(nCapMoi)
	local nM = KMT_MACH
	if nCapMoi < 0 then nCapMoi = 0 end
	if nCapMoi > KM_MAX_CAP then nCapMoi = KM_MAX_CAP end
	SetMeridian(nM - 1, nCapMoi)
	Msg2Player(format("%s %s -> %s %s", "[Test] Kinh m¹ch", KMT_TEN[nM], "cÊp", nCapMoi))
	KM_MenuCap()
end

function KM_Cong1() KMT_DatCap(KMT_LayCap(KMT_MACH) + 1) end
function KM_Cong5() KMT_DatCap(KMT_LayCap(KMT_MACH) + 5) end
function KM_Dat16() KMT_DatCap(16) end
function KM_Dat31() KMT_DatCap(31) end
function KM_Dat32() KMT_DatCap(32) end
function KM_Ve0()   KMT_DatCap(0) end

-- ---------------- cap diem ----------------
function KM_CapDiem()
	SayEx({
		format("%s: %s - %s: %s", "Ch©n Nguyªn", GetTask(KMT_TASK_ZY), "HuyÒn Nguyªn", GetTask(KMT_TASK_XY)),
		"+10.000 Ch©n Nguyªn/KM_ZY1",
		"+1.000.000 Ch©n Nguyªn/KM_ZY2",
		"+10.000 HuyÒn Nguyªn/KM_XY1",
		"+1.000.000 HuyÒn Nguyªn/KM_XY2",
		"Quay l¹i/KM_TestRoot"})
end

function KMT_ThemDiem(nTask, nSo, szTen)
	SetTask(nTask, GetTask(nTask) + nSo)
	Msg2Player(format("%s %s %s. %s: %s", "[Test] Céng", nSo, szTen, "HiÖn cã", GetTask(nTask)))
	KM_CapDiem()
end

function KM_ZY1() KMT_ThemDiem(KMT_TASK_ZY, 10000, "®iÓm Ch©n Nguyªn") end
function KM_ZY2() KMT_ThemDiem(KMT_TASK_ZY, 1000000, "®iÓm Ch©n Nguyªn") end
function KM_XY1() KMT_ThemDiem(KMT_TASK_XY, 10000, "®iÓm HuyÒn Nguyªn") end
function KM_XY2() KMT_ThemDiem(KMT_TASK_XY, 1000000, "®iÓm HuyÒn Nguyªn") end

-- ---------------- cap vat pham ----------------
function KM_CapVatPham()
	local tb = {}
	tb[1] = "Chän vËt phÈm muèn nhËn:"
	local i = 1
	while i <= 9 do
		tb[i + 1] = format("%s %s (x200)/KM_VP%s", "NhËn", KMT_VATPHAM[i][2], i)
		i = i + 1
	end
	tb[11] = "Quay l¹i/KM_TestRoot"
	SayEx(tb)
end

function KMT_ChoVatPham(k)
	local vp = KMT_VATPHAM[k]
	if vp == nil then return end
	local n = 0
	while n < 200 do
		AddItem(6, 1, vp[1], 1, 0, 0)
		n = n + 1
	end
	Msg2Player(format("%s 200 %s", "[Test] §· nhËn", vp[2]))
	KM_CapVatPham()
end

function KM_VP1() KMT_ChoVatPham(1) end
function KM_VP2() KMT_ChoVatPham(2) end
function KM_VP3() KMT_ChoVatPham(3) end
function KM_VP4() KMT_ChoVatPham(4) end
function KM_VP5() KMT_ChoVatPham(5) end
function KM_VP6() KMT_ChoVatPham(6) end
function KM_VP7() KMT_ChoVatPham(7) end
function KM_VP8() KMT_ChoVatPham(8) end
function KM_VP9() KMT_ChoVatPham(9) end

-- ---------------- xem trang thai ----------------
function KM_XemTrangThai()
	Msg2Player("===== Tr¹ng th¸i kinh m¹ch =====")
	local i = 1
	while i <= 12 do
		Msg2Player(format("%s. %s: %s %s - %s %s", i, KMT_TEN[i], "cÊp", KMT_LayCap(i), "sè lÇn háng:", KMT_LayBaoDay(i)))
		i = i + 1
	end
	Msg2Player(format("%s: %s - %s: %s", "Ch©n Nguyªn", GetTask(KMT_TASK_ZY), "HuyÒn Nguyªn", GetTask(KMT_TASK_XY)))
	KM_TestRoot()
end

-- ---------------- xoa sach ----------------
function KM_XoaSach()
	local i = 1
	while i <= 12 do
		SetMeridian(i - 1, 0)
		i = i + 1
	end
	Msg2Player("[Test] §· ®­a c¶ 12 kinh m¹ch vÒ cÊp 0.")
	KM_TestRoot()
end
