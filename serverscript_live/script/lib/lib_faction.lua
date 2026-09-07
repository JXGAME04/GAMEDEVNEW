-- ================================================================================================
-- [HE THONG] script/lib/lib_faction.lua
-- Muc dich  : MON PHAI: doi phai, ky nang phai, kiem tra phai/he.
-- Duoc nap  : Include tu 1 tep (vd char.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Include   : lib_task.lua
-- Ham (dong): GetFactionEx (155), GetFactionEx1 (169), GetFactionEx2 (188), AddFaction (209), LeaveFaction (222), ReturnFaction (233), hockhinhcong (244), add_skill_90 (251), add_skill_120 (317), add_skill_150 (346), SavetStrMagic (409), ReturnStrMagic (441)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
-- Author: Fong KiÒu
-- Date: 28/11/2020
-- Chuc nang: Ham ho tro mon phai

Include("//script//lib//lib_task.lua")

tbSkillBase = {
	{10,14,4,6,8,15,16,20,271,11,19,273,21},
	{34,30,29,23,24,26,33,37,35,31,40,42,36,32,41,324},
	{45,43,347,303,50,54,47,343,345,349,249,58,341,48},
	{63,65,62,60,67,70,66,68,384,64,69,356,73,72,75,71,74},
	{85,80,77,79,93,385,82,89,86,92,252,88,91,282},
	{99,102,95,97,269,105,113,100,109,114,108,111},
	{122,119,116,115,129,274,124,277,128,125,130,360},
	{135,145,132,131,136,137,141,138,140,364,143,150,142,148},
	{153,155,152,151,159,164,158,160,157,166,165,267},
	{169,179,167,168,174,171,392,178,172,393,173,175,181,176,182,90,275,630},
	{1347,1372,1349,1374,1350,1375,1351,1376,1354,1378,1355,1379,1358,1360,1380}, -- [HOASON 01/09c] Hoa Son
	{1972,1974,1964,1975,1976,1965,1977,1963,1979,1980,1982,1971,1981}, -- [VHTD 02/09] Vu Hon
	{2114,2136,2115,2137,2116,2118,2138,2120,2140,2121,2123,2122}, -- [VHTD 02/09] Tieu Dao
}

tbSkill90 = {
	{
		{"§¹t Ma §é Giang", 318},
		{"Hoµnh T¶o Thiªn Qu©n", 319},
		{"V« T­íng Tr¶m", 321}
	},
	{
		{"Ph¸ Thiªn Tr¶m", 322},
		{"Truy Tinh Trôc NguyÖt", 323},
		{"Truy Phong QuyÕt", 325}
	},
	{
		{"Cöu Cung Phi Tinh", 342},
		{"NhiÕp Hån NguyÖt ¶nh", 339},
		{"B¹o Vò Lª Hoa", 302},
		{"Lo¹n Hoµn KÝch", 351},
	},
	{
		{"¢m Phong Thùc Cèt", 353},
		{"HuyÒn ¢m Tr¶m", 355},
		{"§o¹n C©n Hñ Cèt", 390}
	},
	{
		{"Tam Nga TÒ TuyÕt", 328},
		{"Phong S­¬ng To¸i ¶nh", 380},
		{"Phæ §é Chóng Sinh", 332}
	},
	{
		{"B¨ng Tung V« ¶nh", 336},
		{"B¨ng T©m Tiªn Tö ", 337},
	},
	{
		{"Phi Long T¹i Thiªn", 357},
		{"Thiªn H¹ V« CÈu", 359},
	},
	{
		{"V©n Long KÝch", 361},
		{"Thiªn Ngo¹i L­u Tinh", 362},
		{"NhiÕp Hån Lo¹n T©m", 391}
	},
	{
		{"Thiªn §Þa V« Cùc", 365},
		{"Nh©n KiÕm Hîp NhÊt", 368},
	},
	{
		{"Ng¹o TuyÕt Tiªu Phong", 372},
		{"L«i ®éng Cöu Thiªn", 375},
		{"Tóy Tiªn T¸ Cèt", 394}
	},
	{
		{"§o¹t MÖnh Liªn Hoµn Tam KiÕm",1364},
		{"Ph¸ch Th¹ch Ph¸ Ngäc",1382}
	},
}

tbSkill120 = {
	{"§¹i Thõa Nh­ Lai Chó ",709},
	{"§¶o H­ Thiªn",708},
	{"Mª ¶nh Tung",710},
	{"HÊp Tinh YÓm",711},
	{"BÕ NguyÖt PhÊt TrÇn",712},
	{"Ngù TuyÕt Èn",713},
	{"Hçn Thiªn KhÝ C«ng",714},
	{"Ma ¢m PhÖ Ph¸ch",715},
	{"XuÊt ø BÊt NhiÔm",716},
	{"L­ìng Nghi Ch©n KhÝ",717},
	{"Tö Hµ KiÕm KhÝ",1365}, -- [HOASON 01/09c]
	{"Trung Vu L­u Phong",1984}, -- [VHTD 02/09] Vu Hon
	{"¡n Uèng No Say",2127}, -- [VHTD 02/09] Tieu Dao
}

tbSkill150 = {
	{
		{"§¹i Lùc Kim Cang Ch­ëng", 1055},
		{"Vi §µ HiÕn Xö", 1056},
		{"Tam Giíi Quy ThiÒn", 1057}
	},
	{
		{"Hµo Hïng Tr¶m", 1058},
		{"B¸ V­¬ng T¹m Kim", 1060},
		{"Tung Hoµnh B¸t Hoang", 1059}
	},
	{
		{"V« ¶nh Xuyªn", 1069},
		{"ThiÕt Liªn Tø S¸t", 1070},
		{"Cµn Kh«n NhÊt TrÞch", 1071},
	},
	{
		{"H×nh Tiªu Cèt LËp", 1066},
		{"U Hån PhÖ ¶nh", 1067},
	},
	{
		{"KiÕm Hoa V·n Tinh", 1061},
		{"B¨ng Vò L¹c Tinh", 1062},
	},
	{
		{"B¨ng T­íc Ho¹t Kú", 1063},
		{"Thñy Anh Man Tó", 1065},
	},
	{
		{"Thêi ThÆng Lôc Long", 1073},
		{"Bæng Huýnh L­îc §Þa", 1074},
	},
	{
		{"Giang H¶i Né Lan", 1075},
		{"TËt Háa LiÖu Nguyªn", 1076},
	},
	{
		{"T¹o Hãa Th¸i Thanh", 1078},
		{"KiÕm Thïy Tinh Hµ", 1079},
	},
	{
		{"Cöu Thiªn C­¬ng Phong", 1080},
		{"Thiªn L«i ChÊn Nh¹c", 1081},
	},
	{ -- [HOASON 01/09c] Hoa Son
		{"Cöu KiÕm Hîp NhÊt", 1369},
		{"ThÇn Quang Toµn NhiÔu", 1384},
	},
	{ -- [VHTD 02/09] Vu Hon 150
		{"Huy S­ DiÖt Lç", 1969},
		{"Trõ Gian DiÖt NÞnh", 1985},
	},
	{ -- [VHTD 02/09] Tieu Dao 150
		{"ThËp Bé NhÊt S¸t", 2129},
		{"Mai Hoa Tam Léng", 2142},
	},
}

----------------------------------
-- Phô trî hµm 1
----------------------------------
function GetFactionEx()
	local F = ""
	if GetFaction() == "CB" then F = "CB"	return F end
	if GetFaction() == "TNG" then F = "TN"	return F end
	if GetFaction() == "TLP" then F = "TL"	return F end
	if GetFaction() == "TVB" then F = "TV"	return F end
	if GetFaction() == "DM" then F = "DM"	return F end
	if GetFaction() == "NDG" then F = "ND"	return F end
	if GetFaction() == "NMP" then F = "NM"	return F end
	if GetFaction() == "TYM" then F = "TY"	return F end
	if GetFaction() == "VDP" then F = "VD"	return F end
	if GetFaction() == "CLP" then F = "CL"	return F end
	return F
end

function GetFactionEx1(nFaction)
	local F = "S¸t thñ"
	if nFaction == "ÉÙÁÖÅÉ" then F = "ThiÕu L©m"	return F end
	if nFaction == "ÌìÍõ°ï" then F = "Thiªn V­¬ng"	return F end
	if nFaction == "ÌÆÃÅ" then F = "§­êng M«n"	return F end
	if nFaction == "Îå¶¾½Ì" then F = "Ngò §éc"	return F end
	if nFaction == "¶ëáÒÅÉ" then F = "Nga My"	return F end
	if nFaction == "´äÑÌÃÅ" then F = "Thuý Yªn"	return F end
	if nFaction == "Ø¤°ï" then F = "C¸i Bang"	return F end		
	if nFaction == "ÌìÈÌ½Ì" then F = "Thiªn NhÉn"	return F end
	if nFaction == "Îäµ±ÅÉ" then F = "Vâ §ang"	return F end
	if nFaction == "À¥ÂØÅÉ" then F = "C«n L«n"	return F end
	if nFaction == "»ªÉ½ÅÉ" then F = "Hoa S¬n"	return F end -- [HOASON 01/09]
	if nFaction == "Îä»êÌÃ" then F = "Vò Hån"	return F end -- [VHTD 02/09]
	if nFaction == "åÐÒ£ÅÉ" then F = "Tiªu Dao"	return F end -- [VHTD 02/09]
	if nFaction == "³õÈë½­ºþ" then F = "S¸t thñ"	return F end
	return F
end

function GetFactionEx2(nFaction)
	local F = "S¸t thñ"
	if nFaction == "shaolin" then F = "ThiÕu L©m"	return F end
	if nFaction == "tianwang" then F = "Thiªn V­¬ng"	return F end
	if nFaction == "tangmen" then F = "§­êng M«n"	return F end
	if nFaction == "wudu" then F = "Ngò §éc"	return F end
	if nFaction == "emei" then F = "Nga My"	return F end
	if nFaction == "cuiyan" then F = "Thuý Yªn"	return F end
	if nFaction == "gaibang" then F = "C¸i Bang"	return F end		
	if nFaction == "tianren" then F = "Thiªn NhÉn"	return F end
	if nFaction == "wudang" then F = "Vâ §ang"	return F end
	if nFaction == "kunlun" then F = "C«n L«n"	return F end
	if nFaction == "huashan" then F = "Hoa S¬n"	return F end
	if nFaction == "" then F = "S¸t thñ"	return F end
	return F
end

----------------------------------
-- Gia nhËp m«n ph¸i
----------------------------------
function AddFaction(nCamp, nIndex, sFactionName)
	SetFaction(sFactionName) 
	SetCurCamp(nCamp)
	SetCamp(nCamp)    
	SetTask(T_VAOPHAI, nIndex)
	for i = 1, getn(tbSkillBase[nIndex]) do
		AddMagic(tbSkillBase[nIndex][i])
	end
end

----------------------------------
-- 
----------------------------------

function LeaveFaction(nCamp, nIndex)
	SetTask(T_XUATSU,nIndex)
	SetCamp(nCamp)
	SetCurCamp(nCamp)
	Talk(1,"", 10071)
end

----------------------------------
-- 
----------------------------------

function ReturnFaction(nCamp, nIndex)
	SetTask(T_XUATSU,0)
	SetCamp(nCamp)
	SetCurCamp(nCamp)
	Talk(1,"", 10075)
end

----------------------------------
-- 
----------------------------------
function hockhinhcong()
	AddMagic(210,1)
end

----------------------------------
-- 
----------------------------------
function add_skill_90()
	local nFactionID = GetTask(T_VAOPHAI)
	if nFactionID == 0 then
		return
	end
	local TAB_MSG = 
	{
		"Chóc mõng b¹n ®· häc ®­îc<color=green> kü n¨ng 90 "..GetFactionS()..".",
		"Ng­¬i ®· lÜnh héi kü n¨ng nµy råi!",
		"H·y luyÖn lªn ®¼ng cÊp 90 ®·.",
	}
	if (GetLevel() >= 90) then
		if(nFactionID == 6 or nFactionID == 7 or nFactionID == 9 or nFactionID == 11 or nFactionID == 12 or nFactionID == 13) then
			if (HaveMagic(tbSkill90[nFactionID][1][2]) == -1) then
				AddMagic(tbSkill90[nFactionID][1][2], 20)
				AddMagic(tbSkill90[nFactionID][2][2], 20)
				Msg2Player(TAB_MSG[1])
				return
			else
				Msg2Player(TAB_MSG[2])
				return
			end
		elseif(nFactionID == 3) then
			if (HaveMagic(tbSkill90[nFactionID][1][2]) == -1) then
				AddMagic(tbSkill90[nFactionID][1][2], 20)
				AddMagic(tbSkill90[nFactionID][2][2], 20)
				AddMagic(tbSkill90[nFactionID][3][2], 20)
				AddMagic(tbSkill90[nFactionID][4][2], 0)
				Msg2Player(TAB_MSG[1])
				return
			else
				Msg2Player(TAB_MSG[2])
				return
			end
		elseif(nFactionID == 1 or nFactionID == 2) then
			if (HaveMagic(tbSkill90[nFactionID][1][2]) == -1) then
				AddMagic(tbSkill90[nFactionID][1][2], 20)
				AddMagic(tbSkill90[nFactionID][2][2], 20)
				AddMagic(tbSkill90[nFactionID][3][2], 20)
				Msg2Player(TAB_MSG[1])
				return
			else
				Msg2Player(TAB_MSG[2])
				return
			end
		else
			if (HaveMagic(tbSkill90[nFactionID][1][2]) == -1) then
				AddMagic(tbSkill90[nFactionID][1][2], 20)
				AddMagic(tbSkill90[nFactionID][2][2], 20)
				AddMagic(tbSkill90[nFactionID][3][2], 0)
				Msg2Player(TAB_MSG[1])
				return
			else
				Msg2Player(TAB_MSG[2])
				return
			end
		end
	else
		Msg2Player(TAB_MSG[3])
		return
	end
end;

----------------------------
-- NhËn kü n¨ng 120
----------------------------
function add_skill_120()
	local nFactionID = GetTask(T_VAOPHAI)
	if nFactionID == 0 then
		return
	end
	local TAB_MSG = 
	{
		"Chóc mõng b¹n ®· häc ®­îc <color=green>kü n¨ng 120 "..GetFactionS()..".",
		"Ng­¬i ®· lÜnh héi kü n¨ng nµy råi!",
		"H·y luyÖn lªn ®¼ng cÊp 120 ®·.",
	}
	if (GetLevel() >= 120) then
		if (HaveMagic(tbSkill120[nFactionID][2]) == -1) then
			AddMagic(tbSkill120[nFactionID][2], 20)
			Msg2Player(TAB_MSG[1])
			return
		else
			Msg2Player(TAB_MSG[2])
			return
		end
	else
		Msg2Player(TAB_MSG[3])
		return
	end
end

----------------------------
-- NhËn kü n¨ng 150
----------------------------
function add_skill_150()
	local nFactionID = GetTask(T_VAOPHAI)
	if nFactionID == 0 then
		return
	end
	local TAB_MSG = {
			"Chóc mõng b¹n ®· häc ®­îc<color=green> kü n¨ng 150 "..GetFactionS()..".",
			"Ng­¬i ®· lÜnh héi kü n¨ng nµy råi!",
			"H·y luyÖn lªn ®¼ng cÊp 150 ®·.",
		}
	if (GetLevel() >= 150) then
		if(nFactionID == 6 or nFactionID == 7 or nFactionID == 9 or nFactionID == 11 or nFactionID == 12 or nFactionID == 13) then
			if (HaveMagic(tbSkill150[nFactionID][1][2]) == -1) then
				AddMagic(tbSkill150[nFactionID][1][2], 20)
				AddMagic(tbSkill150[nFactionID][2][2], 20)
				Msg2Player(TAB_MSG[1])
				return
			else
				Msg2Player(TAB_MSG[2])
				return
			end
		elseif(nFactionID == 3) then
			if (HaveMagic(tbSkill150[nFactionID][1][2]) == -1) then
				AddMagic(tbSkill150[nFactionID][1][2], 20)
				AddMagic(tbSkill150[nFactionID][2][2], 20)
				AddMagic(tbSkill150[nFactionID][3][2], 20)
				Msg2Player(TAB_MSG[1])
				return
			else
				Msg2Player(TAB_MSG[2])
				return
			end
		elseif(nFactionID == 1 or nFactionID == 2) then
			if (HaveMagic(tbSkill150[nFactionID][1][2]) == -1) then
				AddMagic(tbSkill150[nFactionID][1][2], 20)
				AddMagic(tbSkill150[nFactionID][2][2], 20)
				AddMagic(tbSkill150[nFactionID][3][2], 20)
				Msg2Player(TAB_MSG[1])
				return
			else
				Msg2Player(TAB_MSG[2])
				return
			end
		else
			if (HaveMagic(tbSkill150[nFactionID][1][2]) == -1) then
				AddMagic(tbSkill150[nFactionID][1][2], 20)
				AddMagic(tbSkill150[nFactionID][2][2], 20)
				Msg2Player(TAB_MSG[1])
				return
			else
				Msg2Player(TAB_MSG[2])
				return
			end
		end
	else
		Msg2Player(TAB_MSG[3])
		return
	end
end

----------------------------
-- 
----------------------------
function SavetStrMagic() 					-- kickout cap nhat diem	
	local nFactionID = GetTask(T_VAOPHAI)
	if(nFactionID == 0) then
		return 
	end
	
	local nPoint1,nPoint2, nPoint3,nPoint4 = 0,0,0,0;
	local nCount = getn(tbSkill90[nFactionID])
	
	if(nCount == 2) then
		nPoint1 = GetMagicLevel(tbSkill90[nFactionID][1][2])
		nPoint2 = GetMagicLevel(tbSkill90[nFactionID][2][2])
		SetTask(T_LUUSKILL1, nPoint1)
		SetTask(T_LUUSKILL2, nPoint2)			
	elseif(nCount == 3) then
		nPoint1 = GetMagicLevel(tbSkill90[nFactionID][1][2])
		nPoint2 = GetMagicLevel(tbSkill90[nFactionID][2][2])
		nPoint3 = GetMagicLevel(tbSkill90[nFactionID][3][2])
		SetTask(T_LUUSKILL1, nPoint1)
		SetTask(T_LUUSKILL2, nPoint2)
		SetTask(T_LUUSKILL3, nPoint3)		
	end

	if (HaveMagic(tbSkill120[nFactionID][2]) ~= -1) then
		nPoint4 = GetMagicLevel(tbSkill120[nFactionID][2])
		SetTask(T_LUUSKILL4, nPoint4)			
	end
end

----------------------------
-- 
----------------------------
function ReturnStrMagic() 					-- kickout cap nhat diem	
	local nFactionID = GetTask(T_VAOPHAI)
	if(nFactionID == 0) then
		return 
	end
	
	local nPoint1,nPoint2, nPoint3,nPoint4 = 0,0,0,0;
	local nCount = getn(tbSkill90[nFactionID])
	
	if(nCount == 2) then
		nPoint1 = GetTask(T_LUUSKILL1);
		if(nPoint1 > 0) then
		AddMagic(tbSkill90[nFactionID][1][2], nPoint1)
		end
		nPoint2 = GetTask(T_LUUSKILL2);		
		if (nPoint2 > 0) then
		AddMagic(tbSkill90[nFactionID][2][2], nPoint2)
		end
	elseif (nCount == 3) then
		nPoint1 = GetTask(T_LUUSKILL1);	
		if(nPoint1 > 0) then
		AddMagic(tbSkill90[nFactionID][1][2], nPoint1)
		end
		nPoint2 = GetTask(T_LUUSKILL2);		
		if (nPoint2 > 0) then
		AddMagic(tbSkill90[nFactionID][2][2], nPoint2)
		end
		nPoint3 = GetTask(T_LUUSKILL3);		
		if (nPoint3 > 0) then
		AddMagic(tbSkill90[nFactionID][3][2], nPoint3)
		end
	end
	
	nPoint4 = GetTask(T_LUUSKILL4);		
	if (nPoint4 > 0) then
	AddMagic(tbSkill120[nFactionID][2], nPoint4)
	end
end


