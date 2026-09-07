-- ================================================================================================
-- [HE THONG] script/global/script_protocol.lua
-- Muc dich  : Giao thuc script toan cuc (C++ SCRIPT_PROTOCOL_FILE nap truc tiep - KHONG doi cho).
-- Duoc nap  : engine nap luc boot (moi tep .lua = 1 lua_State rieng) + C++ goi truc tiep theo ten tep
-- Include   : region.lua, worldlibrary.lua, checkmap.lua
-- Ham (dong): PermitTrade (2), OtherMap:Initialize (33), OtherMap:AddMap (39), OtherMap:DelMap (43), OtherMap:Check (47), IsShopMap (56), PermitSuperShop (66), no (80)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
Include("\\script\\header\\region.lua")

function PermitTrade()
	if(GetFightState()~= 0) then
		Msg2Player("Tr¹ng th¸i kh«ng ®­îc b¶o vÖ kh«ng thÓ thùc hiÖn");
		return 0;
	end;
	if(GetLevel() < 80) then
		Msg2Player("§¼ng cÊp d­íi 80 kh«ng thÓ bµy b¸n");
		return 0;
	end;
	if(PermitRegion() == 0) then
		Msg2Player("B¹n kh«ng thÓ bµy b¸n ë khu vùc nµy");
		return 0;
	end;
	return 1;
end;

Include("\\script\\lib\\worldlibrary.lua");
Include("\\script\\maps\\checkmap.lua");

aryMap = {
	44, 326, 327, 328, 329, 330, 331, 344, 345, 346, 347, 348, 349, 350, 351, 352, 353, 354, 355, 357, 358, 359, 360, 361, 362, 363, 364, 365, 366, 367, 368, 369, 370, 371, 372, 373, 374,375,376,377,378,379,380,381,382,383,384,385,386,--ËÎ½ðÕ½³¡ºóÓª
	323,324,325,
	221,222,223,
	208,
	605,606,607,
	608,609,610,611,612,613,
	520,521,522,523,524,525,526,
}

OtherMap = {m_Maps = {}}

function OtherMap:Initialize()
	for i = 1, getn(aryMap) do
		self.m_Maps[aryMap[i]] = 1
	end
end

function OtherMap:AddMap(mapid)
	self.m_Maps[mapid] = 1
end

function OtherMap:DelMap(mapid)
	self.m_Maps[mapid] = nil
end

function OtherMap:Check(mapid)
	if (self.m_Maps[mapid] ~= nil) then
		return 1
	else
		return 0
	end
end
OtherMap:Initialize()

function IsShopMap(nMapID)
	if (IsCityMap(nMapID) == 1 or
		IsFreshmanMap(nMapID) == 1 or
		OtherMap:Check(nMapID) == 1) then
		return 1
	else
		return 0
	end
end

function PermitSuperShop()
	local numShopTab = 5
	local nMapID, _, _ = GetWorldPos()
	if (GetFightState() >= 1 
	--or IsShopMap(nMapID) == 0
	) then
		Msg2Player("Kú Tr©n C¸c chØ cã thÓ më t¹i c¸c thµnh thÞ, t©n thñ th«n vµ mét sè khu vùc phi chiÕn ®Êu kh¸c!")
	else
		  NewSale(0,1,numShopTab,95,96,97,98,101,100)
		-- NewSale(0,1,numShopTab,96,97,98,99,100)
		 --NewSale(0,1,numShopTab,96,97,99,99,100)
	end
end

function no()
end;