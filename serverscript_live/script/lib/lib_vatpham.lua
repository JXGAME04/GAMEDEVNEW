-- ================================================================================================
-- [HE THONG] script/lib/lib_vatpham.lua
-- Muc dich  : VAT PHAM: them/xoa/kiem/dem vat pham trong tui.
-- Duoc nap  : Include tu 51 tep (vd chuyensinhdaisu.lua, cauhoi.lua, drop_npc.lua, drop_tieu.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Ham (dong): DropNpcItemEx1 (62), DropNpcItemEx2 (66), DropSpecialItem (70)
-- Sua nong  : CO - trong chuoi Include cua timerserver.lua, tu nap lai trong <= 1 phut sau khi ghi tep
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
-- Author: Fong KiÒu
-- Date: 28/11/2020
-- Chuc nang: Ham ho tro vat pham
----------------------------

ID_NGUYEN_BAO 						= 342
ID_KIM_NGUYEN_BAO 				= 343
ID_XU_TIENDONG 						= 417
ITEM_NHKT 									= {6,1,2134}

GOLDITEM	= 0
PURPLEITEM	= 1
BLUEITEM 	= 2
GREENITEM	= 3
----------------------------
GENRE		= 0
DETAIL		= 1
PARTICULAR	= 2
LEVEL		= 3
SERIES		= 4
LUCK		= 5
VERSION		= 6
RANDOMSEED	= 7
----------------------------
equip		= 0		-- quan ao, giay, vu khi, nhan, ngoc boi, ngua, that lung, hang lien, bao tay, phi phong, trang suc, an 
medicine	= 1		-- medicine
mine		= 2		-- mine
materials	= 3		-- materials
task 		= 4		-- questkey
townportal	= 5		-- townportal
magicscript = 6		--item magic script
----------------------------
pos_hand = 1		
pos_equip = 2			
pos_equiproom = 3		
pos_repositoryroom = 4	
pos_exbox1room = 5
pos_exbox2room = 6
pos_exbox3room = 7
pos_equiproomex = 8
pos_traderoom = 9
pos_gambleroom =10
pos_trade1 = 11
pos_immediacy = 12
pos_give = 13
pos_affairitem = 14
----------------------------
ROOME		= 0		-- room_equipment
ROOMEB		= 1		-- room_equipmentback
ROOMR		= 2		-- room_repository
ROOME1		= 3		-- room_exbox1
ROOME2		= 4		-- room_exbox2
ROOME3		= 5		-- room_exbox3
ROOMEX		= 6		-- room_equipmentex
ROOMTRADE 	= 7  -- room_trade
ROOMTRADEB 	= 8 -- room_tradeback
ROOMTRADE1 	= 9 -- room_trade1
ROOMIMMEDIACY = 10 -- room_immediacy
ROOMG		= 11	-- room_give
ROOMGB		= 12	-- room_giveback
ROOAFFAIR	= 13	-- room_affairitem

function DropNpcItemEx1(nIdex, nGen, nDetail, nParti, nLevel, nSeries, nLuck, nItemLv)

end

function DropNpcItemEx2(nIdex, nGen, nDetail, nParti, nLevel, nSeries, nLuck, nItemLv1, nItemLv2, nItemLv3, nItemLv4, nItemLv5, nItemLv6)

end

function DropSpecialItem(nIdex, nGen, nDetail, nParti, nLevel, nSeries, nLuck, nItemLv)

end;