-- [PETSYS 29/08] SINH TU DONG tu bang goc VLTK (slistcache entry
-- 0xdf37e2dc): 20 mon trang bi Dong Hanh, goc Genre 12 / DetailType
-- 0..9 = vi tri / ParticularType 0..1 = bo. JX1 nan sang 4907..4926.
PETEQUIP_DEF = {
	[4907] = {nSlot = 1, nSuit = 0},	-- Bich Huyet Vu khi
	[4908] = {nSlot = 2, nSuit = 0},	-- Bich Huyet Y phuc
	[4909] = {nSlot = 3, nSuit = 0},	-- Bich Huyet Nhan
	[4910] = {nSlot = 4, nSuit = 0},	-- Bich Huyet Ho uyen
	[4911] = {nSlot = 5, nSuit = 0},	-- Bich Huyet Ho than phu
	[4912] = {nSlot = 6, nSuit = 0},	-- Bich Huyet Thuc yeu
	[4913] = {nSlot = 7, nSuit = 0},	-- Bich Huyet Day chuyen
	[4914] = {nSlot = 8, nSuit = 0},	-- Bich Huyet Chien ngoa
	[4915] = {nSlot = 9, nSuit = 0},	-- Bich Huyet Yeu truy
	[4916] = {nSlot = 10, nSuit = 0},	-- Bich Huyet Non
	[4917] = {nSlot = 1, nSuit = 1},	-- Kim Lan Vu khi
	[4918] = {nSlot = 2, nSuit = 1},	-- Kim Lan Y phuc
	[4919] = {nSlot = 3, nSuit = 1},	-- Kim Lan Nhan
	[4920] = {nSlot = 4, nSuit = 1},	-- Kim Lan Ho uyen
	[4921] = {nSlot = 5, nSuit = 1},	-- Kim Lan Ho than phu
	[4922] = {nSlot = 6, nSuit = 1},	-- Kim Lan Thuc yeu
	[4923] = {nSlot = 7, nSuit = 1},	-- Kim Lan Day chuyen
	[4924] = {nSlot = 8, nSuit = 1},	-- Kim Lan Chien ngoa
	[4925] = {nSlot = 9, nSuit = 1},	-- Kim Lan Yeu truy
	[4926] = {nSlot = 10, nSuit = 1},	-- Kim Lan Non
}

PETEQUIP_SUIT_TEN = {
	[0] = "B›ch Huy’t",
	[1] = "Kim L©n",
	[2] = "ßan T©m",
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

-- nguyen lieu duc lai va nguon do (theo bang goc VLTK)
PETEQUIP_KETTINH = 4927		-- Ket Tinh Dong Hanh (goc 5063)
PETEQUIP_KETTINH_CAO = 4928	-- Ket Tinh Dong Hanh (Cao) (goc 5257)
PETEQUIP_RUONG = {4929, 4930, 4931}	-- Ruong trang bi 1/2/3 (goc 5064..5066)
PETEQUIP_CHIAKHOA = 4932	-- Chia khoa ruong (goc 5067)
