-- [PETSYS 30/08] SINH TU DONG tu bang goc VLTK (slistcache 0xdf37e2dc).
-- Moi mon co 3 THUOC TINH: {ma_attrib, gia_tri_min, gia_tri_max}
-- (cot 16..24 cua bang goc). Duc lai = roll lai 3 gia tri trong khoang.
-- Ma attrib theo enum KMagicAttrib.h cua engine (vd 268 =
-- anti_block_rate, 276/280 = addskilldamage2/6).
PETEQUIP_DEF = {
	[4907] = {nSlot = 1, nSuit = 0, tbAttrib = {{268, 8, 10}, {276, 3, 4}, {280, 3, 4}}},	-- Bich Huyet Vu khi
	[4908] = {nSlot = 2, nSuit = 0, tbAttrib = {{241, 8, 10}, {278, 3, 4}, {282, 3, 4}}},	-- Bich Huyet Y phuc
	[4909] = {nSlot = 3, nSuit = 0, tbAttrib = {{243, 83, 110}, {284, 3, 4}, {190, 11, 15}}},	-- Bich Huyet Nhan
	[4910] = {nSlot = 4, nSuit = 0, tbAttrib = {{227, 5, 7}, {277, 3, 4}, {281, 3, 4}}},	-- Bich Huyet Ho uyen
	[4911] = {nSlot = 5, nSuit = 0, tbAttrib = {{271, 5, 7}, {279, 3, 4}, {283, 3, 4}}},	-- Bich Huyet Ho than phu
	[4912] = {nSlot = 6, nSuit = 0, tbAttrib = {{246, 750, 1000}, {285, 3, 4}, {97, 75, 100}}},	-- Bich Huyet Thuc yeu
	[4913] = {nSlot = 7, nSuit = 0, tbAttrib = {{226, 5, 7}, {236, 23, 30}, {234, 23, 30}}},	-- Bich Huyet Day chuyen
	[4914] = {nSlot = 8, nSuit = 0, tbAttrib = {{247, 750, 1000}, {269, 75, 100}, {99, 75, 100}}},	-- Bich Huyet Chien ngoa
	[4915] = {nSlot = 9, nSuit = 0, tbAttrib = {{270, 5, 7}, {248, 11, 15}, {100, 75, 100}}},	-- Bich Huyet Yeu truy
	[4916] = {nSlot = 10, nSuit = 0, tbAttrib = {{308, 38, 50}, {88, 15, 20}, {98, 75, 100}}},	-- Bich Huyet Non
	[4917] = {nSlot = 1, nSuit = 1, tbAttrib = {{268, 10, 14}, {276, 4, 6}, {280, 4, 6}}},	-- Kim Lan Vu khi
	[4918] = {nSlot = 2, nSuit = 1, tbAttrib = {{241, 10, 14}, {278, 4, 6}, {282, 4, 6}}},	-- Kim Lan Y phuc
	[4919] = {nSlot = 3, nSuit = 1, tbAttrib = {{243, 110, 130}, {284, 4, 6}, {190, 15, 20}}},	-- Kim Lan Nhan
	[4920] = {nSlot = 4, nSuit = 1, tbAttrib = {{227, 7, 10}, {277, 4, 6}, {281, 4, 6}}},	-- Kim Lan Ho uyen
	[4921] = {nSlot = 5, nSuit = 1, tbAttrib = {{271, 7, 10}, {279, 4, 6}, {283, 4, 6}}},	-- Kim Lan Ho than phu
	[4922] = {nSlot = 6, nSuit = 1, tbAttrib = {{246, 1000, 1250}, {285, 4, 6}, {97, 100, 125}}},	-- Kim Lan Thuc yeu
	[4923] = {nSlot = 7, nSuit = 1, tbAttrib = {{226, 7, 10}, {236, 30, 40}, {234, 30, 40}}},	-- Kim Lan Day chuyen
	[4924] = {nSlot = 8, nSuit = 1, tbAttrib = {{247, 1000, 1250}, {269, 100, 125}, {99, 100, 125}}},	-- Kim Lan Chien ngoa
	[4925] = {nSlot = 9, nSuit = 1, tbAttrib = {{270, 7, 12}, {248, 15, 20}, {100, 100, 125}}},	-- Kim Lan Yeu truy
	[4926] = {nSlot = 10, nSuit = 1, tbAttrib = {{308, 50, 65}, {88, 20, 25}, {98, 100, 125}}},	-- Kim Lan Non
}

PETEQUIP_SUIT_TEN = {
	[0] = "Bich Huyet",
	[1] = "Kim Lan",
	[2] = "Dan Tam",
}

PETEQUIP_VITRI = {
	[1] = "Vu khi",
	[2] = "Y phuc",
	[3] = "Nhan",
	[4] = "Ho uyen",
	[5] = "Ho than phu",
	[6] = "Thuc yeu",
	[7] = "Day chuyen",
	[8] = "Chien ngoa",
	[9] = "Yeu truy",
	[10] = "Non",
}

-- o task: 5143..5152 = id mon dang deo; 5170..5199 = 3 gia tri da roll
-- cua tung mon (mon i -> 5170 + (i-1)*3 .. +2)
PETEQUIP_O_DAU = 5143
PETEQUIP_O_SO = 10
PETEQUIP_O_BOCOUNT = 5163
PETEQUIP_O_ATTRIB = 5170

-- nguyen lieu / nguon do (bang goc VLTK)
PETEQUIP_KETTINH = 4927
PETEQUIP_KETTINH_CAO = 4928
PETEQUIP_RUONG = {4929, 4930, 4931}
PETEQUIP_CHIAKHOA = 4932
