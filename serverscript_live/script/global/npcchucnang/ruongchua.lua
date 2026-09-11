-- [LOCAL54 06/09 toi] 1 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local moruongchua
-- Author: Fong KiÒu
-- Date: 28/11/2020
-- Chuc nang: Ruong chua

function main(NpcIndex)
	-- dofile("\script\global\npc\buysell\box.lua")
	moruongchua(NpcIndex)
end

-- (11/09) [RUONGHS] Bang RUONG -> ma diem hoi sinh (RevivePos.ini), sinh tu
-- script/startgame/thanh/*.lua: moi dong la {map, o X, o Y, ma hoi sinh}.
-- VI SAO CAN BANG NAY: cac tep thanh goi AddObj(...) roi SetNpcValue(nIdx, ma). Nhung
-- AddObj tra ve CHI SO TRONG MANG Object[] (ScriptFuns.cpp:4265) con SetNpcValue ghi vao
-- Npc[chi so do].m_nNpcParam[0] (ScriptFuns.cpp:7508) - HAI MANG KHAC NHAU. Luc bam ruong,
-- KObj::ExecScript goi main(chi so obj) (KObj.cpp:1277) nen GetNpcValue doc lai dung o vua
-- ghi: chay duoc la do MAY. Bat ky NPC nao mang CUNG CHI SO goi SetNpcValue (rat nhieu
-- script lam, vi du NPC co Tong Kim dat gia tri 1/2 moi tran) la de mat ma hoi sinh ->
-- SetRevPos dat SAI CHO, hoac nguoc lai ruong de mat tham so cua NPC do.
-- Nay xac dinh ruong bang VI TRI NGUOI CHOI: may chu chi chay script obj khi nguoi choi
-- cach <= 200 mps = 6,25 o (defMAX_EXEC_OBJ_SCRIPT_DISTANCE, KProtocolProcess.cpp:6765/6773)
-- ma cac ruong cach nhau hang chuc o nen khong the nham. GetWorldPos() tra (map, x theo O,
-- y theo O) - ScriptFuns.cpp:4453-4472. Van giu duong cu lam du phong.
RUONG_HS_SO = 29
RUONG_HS = {
	{37,1854,2955,26},	-- bienkinh.lua
	{37,1723,3081,23},	-- bienkinh.lua
	{37,1614,3013,25},	-- bienkinh.lua
	{37,1694,3207,24},	-- bienkinh.lua
	{162,1626,3151,64},	-- daily.lua
	{162,1576,3132,63},	-- daily.lua
	{80,1757,3005,34},	-- duongchau.lua
	{80,1710,3227,35},	-- duongchau.lua
	{80,1662,2985,38},	-- duongchau.lua
	{80,1627,3174,37},	-- duongchau.lua
	{80,1855,3030,36},	-- duongchau.lua
	{176,1688,3277,68},	-- laman.lua
	{176,1391,3312,67},	-- laman.lua
	{176,1574,2933,69},	-- laman.lua
	{1,1592,3170,5},	-- phuongtuong.lua
	{1,1544,3220,3},	-- phuongtuong.lua
	{1,1658,3155,4},	-- phuongtuong.lua
	{1,1632,3261,1},	-- phuongtuong.lua
	{1,1571,3118,2},	-- phuongtuong.lua
	{11,3157,5076,5},	-- thanhdo.lua
	{11,3186,5180,6},	-- thanhdo.lua
	{11,3054,4983,7},	-- thanhdo.lua
	{11,3001,5104,8},	-- thanhdo.lua
	{11,3237,5028,9},	-- thanhdo.lua
	{78,1575,3340,30},	-- tuongduong.lua
	{78,1450,3263,31},	-- tuongduong.lua
	{78,1482,3164,32},	-- tuongduong.lua
	{78,1563,3219,29},	-- tuongduong.lua
	{78,1687,3235,33},	-- tuongduong.lua
}

function moruongchua(NpcIndex)
	OpenBox()
	local nRev = -1
	local W, X, Y = GetWorldPos()
	local i = 1
	while (i <= RUONG_HS_SO) do
		local r = RUONG_HS[i]
		if (r[1] == W) then
			local dx = r[2] - X
			local dy = r[3] - Y
			if (dx < 0) then
				dx = -dx
			end
			if (dy < 0) then
				dy = -dy
			end
			if (dx <= 8 and dy <= 8) then
				nRev = r[4]
				i = RUONG_HS_SO
			end
		end
		i = i + 1
	end
	if (nRev < 0) then
		-- du phong: cach cu (o nho bi lan voi mang NPC - xem chu thich tren)
		local nCu = GetNpcValue(NpcIndex)
		if (nCu ~= nil and nCu >= 0) then
			nRev = nCu
		end
	end
	if (nRev >= 0) then
		SetRevPos(nRev)
	end
end
