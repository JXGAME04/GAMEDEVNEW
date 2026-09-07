-- ================================================================================================
-- [HE THONG] script/global/trangbixanh.lua
-- Muc dich  : TRANG BI XANH.
-- Duoc nap  : Include tu 4 tep (vd lenhbaiadmin.lua, lenhbaitanthu.lua, hotrotanthu.lua, hotrotest.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Ham (dong): laydoxanh0 (129), laydoxanh1 (144), laydoxanh2 (154), laydoxanh3 (165), no (186)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
--Author: Fong KiÒu
--Date: 09/07/2021
--Function: Lib Trang bÞ xanh

FREECELL_GETITEMBLUE = 10

tbDoXanh =
{
	[1]=
	{
		szName = "D©y chuyÒn",
		tbEquip =
		{
			{"D©y chuyÒn n÷",0,4,0},
			{"D©y chuyÒn nam ",0,4,1},
		}
	},
	[2]=
	{
		szName = "Gi¸p ¸o",
		tbEquip =
		{
			{"Aã t¨ng",0,2,0},
			{"Aã thæ",0,2,1},
			{"MËt trang nam",0,2,n2},
			{"Gi¸ng Sa Bµo",0,2,3},
			{"Gi¸p nam",0,2,4},
			{"V¹n L­u Quy T«ng Y",0,2,5},
			{"TuyÒn Long bµo",0,2,6},
			{"Long Tiªu ®¹o Y",0,2,8},
			{"Cöu VÜ B¹ch Hå trang",0,2,9},
			{"TrÇm H­¬ng sam",0,2,10},
			{"Gi¸p n÷",0,2,11},
			{"V¹n Chóng TÒ T©m Y",0,2,12},
			{"L­u Tiªn QuÇn",0,2,13},
		}
	},
	[3]=
	{
		szName = "§ai l­ng",
		tbEquip =
		{
			{"Thiªn Tµm Yªu §¸i",0,6,0},
			{"B¹ch Kim Yªu §¸i",0,6,1},
		}
	},
	[4]=
	{
		szName = "Giµy ngoa",
		tbEquip =
		{
			{"Cöu TiÕt X­¬ng VÜ Ngoa",0,5,0},
			{"Thiªn Tµm Ngoa",0,5,1},
			{"Kim Lò hµi",0,5,2},
			{"Phi Phông Ngoa",0,5,3},
		}
	},
	[5]=
	{
		szName = "Bao tay",
		tbEquip =
		{
			{"Long Phông HuyÕt Ngäc Tr¹c",0,8,0},
			{"Thiªn Tµm Hé UyÓn",0,8,1},
		}
	},
	[6]=
	{
		szName = "Nãn m·o",
		tbEquip =
		{
			{"Tú L« m·o",0,7,0},
			{"Ngò l·o qu¸n",0,7,1},
			{"Tu La Ph¸t kÕt",0,7,2},
			{"Th«ng Thiªn Ph¸t Qu¸n",0,7,3},
			{"YÓm NhËt kh«i",0,7,4},
			{"TrÝch Tinh hoµn",0,7,5},
			{"¤ Tµm M·o",0,7,6},
			{"Quan ¢m Ph¸t Qu¸n",0,7,7},
			{"¢m D­¬ng V« Cùc qu¸n",0,7,8},
			{"HuyÒn Tª DiÖn Tr¸o",0,7,9},
			{"Long HuyÕt §Çu hoµn",0,7,10},
			{"Long L©n Kh«i",0,7,11},
			{"Thanh Tinh Thoa",0,7,12},
			{"Kim Phông TriÓn SÜ",0,7,13},
		}
	},
	[7]=
	{
		szName = "Vò khÝ cËn chiÕn",
		tbEquip =
		{
			{"KiÕm",0,0,0},
			{"§ao",0,0,1},
			{"Bæng",0,0,2},
			{"KÝch",0,0,3},
			{"Chïy",0,0,4},
			{"Song ®ao",0,0,5},
		}
	},
	[8]=
	{
		szName = "Ngäc béi",
		tbEquip =
		{
			{"H­¬ng Nang n÷",0,9,0},
			{"Ngäc béi nam",0,9,1},
		}
	},
	[8]=
	{
		szName = "Vò khÝ tÇm xa",
		tbEquip =
		{
			{"Tiªu",0,1,0},
			{"Phi §ao",0,1,1},
			{"Ná",0,1,2},
		}
	},
	[9]=
	{
		szName = "Giíi chØ",
		tbEquip =
		{
			{"Giíi ChØ ",0,3,0},
		}
	},
}

function laydoxanh0()
	
	if CalcFreeItemCellCount() < FREECELL_GETITEMBLUE then
		Talk(1,"","Kho¶ng trèng hµnh trang kh«ng ®ñ "..FREECELL_GETITEMBLUE.." « . §Ò nghÞ dän dÑp hµnh trang tr­íc!")
		return
	end	
		
	local tbOpt = {"<npc> Xin mêi <sex> lùa chän trang bÞ:"}
	for i=1, getn(tbDoXanh) do
		tinsert(tbOpt, format("%s/#laydoxanh1(%d)", tbDoXanh[i].szName, i))
	end
	tinsert(tbOpt, "Kh«ng nhËn/no")
	SayEx(tbOpt)
end

function laydoxanh1(nType)
	local tbEquip = tbDoXanh[nType]["tbEquip"]
	local tbOpt = {"<npc> Xin mêi <sex> lùa chän trang bÞ:"}
	for i=1, getn(tbEquip) do
		tinsert(tbOpt, format("%s/#laydoxanh2(%d,%d)", tbEquip[i][1], i, nType))
	end
	tinsert(tbOpt, "Kh«ng nhËn/no")
	SayEx(tbOpt)
end

function laydoxanh2(nIndex, nType)
	local tbOpt = {"<npc> Xin mêi <sex> chän hÖ:"}
	tinsert(tbOpt, format("Kim/#laydoxanh3(%d,%d,%d)", nIndex, nType, 0))
	tinsert(tbOpt, format("Méc/#laydoxanh3(%d,%d,%d)", nIndex, nType, 1))
	tinsert(tbOpt, format("Thuû/#laydoxanh3(%d,%d,%d)", nIndex, nType, 2))
	tinsert(tbOpt, format("Ho¶/#laydoxanh3(%d,%d,%d)", nIndex, nType, 3))
	tinsert(tbOpt, format("Thæ/#laydoxanh3(%d,%d,%d)", nIndex, nType, 4))
	tinsert(tbOpt, "Kh«ng nhËn/no")
	SayEx(tbOpt)
end

function laydoxanh3(nIndex, nType, nSeries)
	if CalcFreeItemCellCount() < FREECELL_GETITEMBLUE then
		Talk(1,"","Kho¶ng trèng hµnh trang kh«ng ®ñ "..FREECELL_GETITEMBLUE.." « . §Ò nghÞ dän dÑp hµnh trang tr­íc!")
		return
	end	
		
	local tbEquipSelect = tbDoXanh[nType]["tbEquip"][nIndex]
	local nLevel = 1
	local nLucky = 0
	local nMagicLevel = 0
	local nNum = 1
	
	for num = 1, nNum do
		local VKCB = AddItem(tbEquipSelect[2], tbEquipSelect[3], tbEquipSelect[4], nLevel, nSeries, nLucky, nMagicLevel)
		-- SetPlayerItemLock(VKCB, -2)	
		-- local VKCBB = AddItem(tbEquipSelect[2], tbEquipSelect[3], tbEquipSelect[4], 1, nSeries, nLucky, nMagicLevel)
		-- SetPlayerItemLock(VKCBB, -2)	
	end
	
end

function no()
	
end
