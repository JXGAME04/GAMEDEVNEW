-- @IncludeOnce  [LUA54 06/09 toi] tep toan ham: Lua54Dll chi chay than mot lan moi state (chon_includeonce.py)
-- ================================================================================================
-- [HE THONG] script/startgame/thon/xaphu_thon5.lua
-- Muc dich  : [VTCN 06/09] chu game: "sua lai Xa Phu cho day du thanh thon" - 5 thon/tran co ben
--             trong settings\Station.txt (Long Tuyen 174, Dao Huong 101, Vinh Lac 99, Chu Tien 100,
--             Thach Co 153) truoc day KHONG co NPC Xa Phu -> nguoi choi (va Chi nam nhiem vu F11)
--             toi do xong khong di tiep duoc. Dat Xa Phu cach diem den cua ben 3 o, o DI DUOC
--             (da soi luoi Maps\<id>_srv.fp bang ReverseTools\fp_view.py).
-- Duoc nap  : Include tu script\startgame.lua, goi addnpcxaphuthon5() trong addfullnpc()
-- Mau NPC   : 239 = "Xa phu At 3" (npcs.txt), cung mau Ba Lang huyen / Nam Nhac tran
-- Sua nong  : KHONG - can restart GameServer
-- ================================================================================================

function addnpcxaphuthon5()
	-- {map, x, y}: diem den Station.txt -> 174 (1632,3199) 101 (1621,3104) 99 (1606,3165) 100 (1615,3100) 153 (1632,3182)
	local tbXaPhu = {
		{174, 1629, 3202},	-- Thon Long Tuyen
		{101, 1624, 3105},	-- Thon Dao Huong
		{ 99, 1603, 3168},	-- Tran Vinh Lac
		{100, 1612, 3099},	-- Tran Chu Tien
		{153, 1635, 3185},	-- Tran Thach Co
	}
	for i = 1, getn(tbXaPhu) do
		local d = tbXaPhu[i]
		AddNpcNew(239, 1, d[1], d[2]*32, d[3]*32, "\\script\\global\\npcchucnang\\xaphu.lua", 6, 42)
	end
end
