-- tongjx2npc.lua - dat NPC bang hoi JX2 (goi tu OnGame trong startgame.lua)
-- Ten NPC + template + script lay NGUYEN VAN tu addtongnpc.lua ban Linux.
-- Map lanh dia bang (586-597) chua port -> cum NPC dat TAM o Ba Lang Huyen
-- (map 53, canh NPC Ho Tro) de test; Te Tuu dat dung 7 thanh nhu ban Linux.

function JX2Tong_AddNpc()
	local nW53 = SubWorldID2Idx(53)
	local nIdx
	if (nW53 >= 0) then
		nIdx = AddNpc(235, 1, nW53, 1598*32, 3190*32, 1, "Xa phu")
		if (nIdx and nIdx > 0) then
			SetNpcScript(nIdx, "\\scriptjx2\\tong_vn\\npc\\tong_chefu.lua")
		end
		nIdx = AddNpc(625, 1, nW53, 1603*32, 3190*32, 1, "R­¬ng chøa ®å")
		if (nIdx and nIdx > 0) then
			SetNpcScript(nIdx, "\\scriptjx2\\tong_vn\\npc\\tong_chuwuxiang.lua")
		end
		nIdx = AddNpc(329, 1, nW53, 1608*32, 3190*32, 1, "TÕ Töu")
		if (nIdx and nIdx > 0) then
			SetNpcScript(nIdx, "\\scriptjx2\\tong_vn\\npc\\jitan.lua")
		end
		nIdx = AddNpc(649, 1, nW53, 1613*32, 3190*32, 1, "Tæng qu¶n khu chiÕn ®Êu")
		if (nIdx and nIdx > 0) then
			SetNpcScript(nIdx, "\\scriptjx2\\tong_vn\\npc\\zhandouqu_zongguan.lua")
		end
		nIdx = AddNpc(1151, 1, nW53, 1618*32, 3190*32, 1, "Khu Binh gi¸p ")
		if (nIdx and nIdx > 0) then
			SetNpcScript(nIdx, "\\scriptjx2\\tong_vn\\npc\\bingjia_zongguan.lua")
		end
		nIdx = AddNpc(1156, 1, nW53, 1623*32, 3190*32, 1, "Khu Thiªn C«ng ")
		if (nIdx and nIdx > 0) then
			SetNpcScript(nIdx, "\\scriptjx2\\tong_vn\\npc\\tiangong_zongguan.lua")
		end
		nIdx = AddNpc(1165, 1, nW53, 1628*32, 3190*32, 1, "Khu MÆt n¹ ")
		if (nIdx and nIdx > 0) then
			SetNpcScript(nIdx, "\\scriptjx2\\tong_vn\\npc\\mianju_zongguan.lua")
		end
		nIdx = AddNpc(1170, 1, nW53, 1633*32, 3190*32, 1, "Khu LuyÖn tËp")
		if (nIdx and nIdx > 0) then
			SetNpcScript(nIdx, "\\scriptjx2\\tong_vn\\npc\\shilian_zongguan.lua")
		end
		nIdx = AddNpc(1175, 1, nW53, 1638*32, 3190*32, 1, "Khu Thiªn ý ")
		if (nIdx and nIdx > 0) then
			SetNpcScript(nIdx, "\\scriptjx2\\tong_vn\\npc\\tianyi_zongguan.lua")
		end
		nIdx = AddNpc(1180, 1, nW53, 1643*32, 3190*32, 1, "Khu LÔ vËt ")
		if (nIdx and nIdx > 0) then
			SetNpcScript(nIdx, "\\scriptjx2\\tong_vn\\npc\\liwu_zongguan.lua")
		end
		nIdx = AddNpc(1185, 1, nW53, 1648*32, 3190*32, 1, "Khu ho¹t ®éng ")
		if (nIdx and nIdx > 0) then
			SetNpcScript(nIdx, "\\scriptjx2\\tong_vn\\npc\\huodong_zongguan.lua")
		end
	end
	-- Te Tuu 7 thanh (toa do nguyen van ban Linux)
	local nWJ1 = SubWorldID2Idx(1)
	if (nWJ1 >= 0) then
		nIdx = AddNpc(329, 1, nWJ1, 1562*32, 3252*32, 1, "TÕ Töu")
		if (nIdx and nIdx > 0) then
			SetNpcScript(nIdx, "\\scriptjx2\\tong_vn\\npc\\jitan.lua")
		end
	end
	local nWJ11 = SubWorldID2Idx(11)
	if (nWJ11 >= 0) then
		nIdx = AddNpc(329, 1, nWJ11, 3096*32, 5025*32, 1, "TÕ Töu")
		if (nIdx and nIdx > 0) then
			SetNpcScript(nIdx, "\\scriptjx2\\tong_vn\\npc\\jitan.lua")
		end
	end
	local nWJ37 = SubWorldID2Idx(37)
	if (nWJ37 >= 0) then
		nIdx = AddNpc(329, 1, nWJ37, 1697*32, 3117*32, 1, "TÕ Töu")
		if (nIdx and nIdx > 0) then
			SetNpcScript(nIdx, "\\scriptjx2\\tong_vn\\npc\\jitan.lua")
		end
	end
	local nWJ78 = SubWorldID2Idx(78)
	if (nWJ78 >= 0) then
		nIdx = AddNpc(329, 1, nWJ78, 1580*32, 3204*32, 1, "TÕ Töu")
		if (nIdx and nIdx > 0) then
			SetNpcScript(nIdx, "\\scriptjx2\\tong_vn\\npc\\jitan.lua")
		end
	end
	local nWJ80 = SubWorldID2Idx(80)
	if (nWJ80 >= 0) then
		nIdx = AddNpc(329, 1, nWJ80, 1693*32, 3066*32, 1, "TÕ Töu")
		if (nIdx and nIdx > 0) then
			SetNpcScript(nIdx, "\\scriptjx2\\tong_vn\\npc\\jitan.lua")
		end
	end
	local nWJ162 = SubWorldID2Idx(162)
	if (nWJ162 >= 0) then
		nIdx = AddNpc(329, 1, nWJ162, 1520*32, 3159*32, 1, "TÕ Töu")
		if (nIdx and nIdx > 0) then
			SetNpcScript(nIdx, "\\scriptjx2\\tong_vn\\npc\\jitan.lua")
		end
	end
	local nWJ176 = SubWorldID2Idx(176)
	if (nWJ176 >= 0) then
		nIdx = AddNpc(329, 1, nWJ176, 1561*32, 2942*32, 1, "TÕ Töu")
		if (nIdx and nIdx > 0) then
			SetNpcScript(nIdx, "\\scriptjx2\\tong_vn\\npc\\jitan.lua")
		end
	end
	print("[JX2TONG] da dat NPC bang hoi")
end

-- ===================================================================
-- LANH DIA BANG: sinh NPC + 7 toa xuong tren 11 map 586-597.
-- addtongnpc.lua cua ban Linux nam trong khoi "if MODEL_GAMESERVER == 1"
-- ma bien do KHONG duoc dinh nghia o dau trong cay nay -> ca file chua
-- tung duoc nap, LoadPosSetting chua tung chay. Dat co roi nap thang o
-- day (state rieng cua startgame) thay vi bat khoi 600 dong cua
-- tong_mix.lua - de khong keo theo cac include chua co.
function JX2Tong_AddTerritoryNpc()
	MODEL_GAMESERVER = 1
	Include("\\script\\tong\\addtongnpc.lua")
	if (add_tongnpc ~= nil) then
		add_tongnpc()
	end
end

