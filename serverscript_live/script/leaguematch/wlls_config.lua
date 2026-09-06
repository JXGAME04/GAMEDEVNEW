--============================================================
-- File cÊu h×nh tËp trung hÖ Liªn ®Êu (WLLS) - 21/08/2026
-- ChØnh xong RESTART GameServer (script chØ n¹p lóc boot).
-- File nµy ®­îc kÐo vµo tõng state qua:
--   * \script\leaguematch\switch.lua        (tÇng lÞch / driver)
--   * \script\missions\leaguematch\head.lua (tÇng trËn ®Êu + NPC)
--============================================================
if (WLLS_CFG_STATE_DA_AP) then return end
WLLS_CFG_STATE_DA_AP = 1

WLLS_CFG = {
	-- ==== BËt t¾t hai h¹ng ®Êu ====
	MO_KIET_XUAT   = 1,	-- h¹ng KiÖt xuÊt (cÊp 80-119): 1 më / 0 ®ãng (gèc Linux: 0 ®ãng)
	MO_VO_LAM      = 1,	-- h¹ng Vâ l©m (cÊp 120 trë lªn): 1 më / 0 ®ãng

	-- ==== ThÓ lo¹i ®Êu cña tõng mïa ====
	-- 7 thÓ lo¹i: 1 Song ®Êu (®éi 2 ng­êi), 2 ChiÕn ®éi m«n ph¸i, 3 S­ ®å,
	-- 4 Tam nh©n, 5 §¬n ®Êu, 6 Liªn hoµn, 7 Nam n÷ phèi hîp.
	-- LÞch gèc timetable.lua xoay vßng 2,1,2,5,3,6,2,1,4,5,3,6 - mïa 131
	-- (th¸ng 8/2026) ®ang lµ 1 Song ®Êu.
	LOAI_CO_DINH   = nil,	-- Ðp mäi mïa vÒ mét thÓ lo¹i (1..7); nil = theo lÞch gèc
	LOAI_THEO_MUA  = nil,	-- chØnh riªng tõng mïa; vÝ dô: { [131] = 5, [132] = 1 }
	-- Chó ý: ®æi thÓ lo¹i cña mïa ®ang diÔn ra = ®æi m« thøc, hÖ sÏ xãa toµn
	-- bé chiÕn ®éi hiÖn cã (®óng luËt gèc wlls_set_mid). §æi xong ph¶i restart.

	-- ==== Thêi gian ====
	PHUT_MOI_LUOT  = 15,	-- sè phót mét l­ît trËn (gèc 15)
	GIO_MO         = nil,	-- nil = theo lÞch gèc trong timetable.lua (18h00 mçi ngµy, 4 l­ît)
	-- Muèn tù ®Æt khung giê (¸p cho mäi ngµy trong tuÇn), bá nil vµ khai:
	--   GIO_MO = { {18, 0, 4} }             -- 18h00: 4 l­ît liªn tiÕp
	--   GIO_MO = { {18, 0, 4}, {20, 0, 4} } -- thªm khung 20h00
	--   (mçi môc = { giê, phót, sè l­ît }; ®é dµi 1 l­ît = PHUT_MOI_LUOT)

	-- ==== §iÓm ®éi (céng sau mçi trËn, nh©n víi cÊp h¹ng 1/2) ====
	DIEM_THANG     = 5,	-- gèc 5  (®iÓm = sè trËn th¾ng x DIEM_THANG x cÊp h¹ng)
	DIEM_HOA       = 2,	-- gèc 2

	-- ==== Th­ëng xÕp h¹ng cuèi mïa ====
	NHAN_DIEM_HANG = 1,	-- hÖ sè nh©n ®iÓm th­ëng xÕp h¹ng (b¶ng award_rank)
	NHAN_ITEM_HANG = 1,	-- hÖ sè nh©n sè l­îng item th­ëng xÕp h¹ng
}

--=========== PhÇn ¸p dông (kh«ng cÇn söa d­íi ®©y) ===========

-- 1. C«ng t¾c hai h¹ng (chØ state nµo ®· n¹p switch.lua)
if (WLLS_SWITCH_JUNIOR ~= nil) then
	WLLS_SWITCH_JUNIOR = WLLS_CFG.MO_KIET_XUAT
	WLLS_SWITCH_SENIOR = WLLS_CFG.MO_VO_LAM
end

-- 2. §é dµi l­ît trËn
if (WLLS_MATCHTIME ~= nil and WLLS_CFG.PHUT_MOI_LUOT ~= nil) then
	WLLS_MATCHTIME = WLLS_CFG.PHUT_MOI_LUOT
end

-- 3. Khung giê më trËn: thay lÞch c¶ 7 ngµy cho mäi mïa gi¶i
if (WLLS_SEASON_TB ~= nil and WLLS_CFG.GIO_MO ~= nil) then
	local tb_cal = {}
	for n_wday = 1, 7 do
		tb_cal[n_wday] = {}
		for i = 1, getn(WLLS_CFG.GIO_MO) do
			local tb_g = WLLS_CFG.GIO_MO[i]
			tb_cal[n_wday][i] = { tb_g[1]*60 + tb_g[2], tb_g[3] }
		end
	end
	for n_sid, tb_row in pairs(WLLS_SEASON_TB) do
		if (type(tb_row) == "table" and tb_row[5] ~= nil) then
			tb_row[5] = tb_cal
		end
	end
end

-- 3b. ThÓ lo¹i ®Êu tõng mïa (sentinel 999998/999999 kh«ng bao giê ®­îc chän nªn v« h¹i)
if (WLLS_SEASON_TB ~= nil) then
	if (WLLS_CFG.LOAI_CO_DINH ~= nil) then
		for n_sid, tb_row in pairs(WLLS_SEASON_TB) do
			if (type(tb_row) == "table" and tb_row[1] ~= nil) then
				tb_row[1] = WLLS_CFG.LOAI_CO_DINH
			end
		end
	end
	if (WLLS_CFG.LOAI_THEO_MUA ~= nil) then
		for n_sid, n_loai in pairs(WLLS_CFG.LOAI_THEO_MUA) do
			if (WLLS_SEASON_TB[n_sid] ~= nil) then
				WLLS_SEASON_TB[n_sid][1] = n_loai
			end
		end
	end
end

-- 4. C«ng thøc ®iÓm ®éi (chØ state trËn ®Êu GS - n¬i cã hµm gèc trong head.lua)
if (wlls_GetAddPoint ~= nil) then
	function wlls_GetAddPoint(nLevel, nWin, nTie)
		return nWin * WLLS_CFG.DIEM_THANG * nLevel + nTie * WLLS_CFG.DIEM_HOA * nLevel
	end
end

-- 5. Nh©n th­ëng xÕp h¹ng cuèi mïa
if (WLLS_TAB ~= nil and (WLLS_CFG.NHAN_DIEM_HANG ~= 1 or WLLS_CFG.NHAN_ITEM_HANG ~= 1)) then
	for n_type, tb_data in pairs(WLLS_TAB) do
		if (type(tb_data) == "table" and tb_data.award_rank ~= nil) then
			for n_level, tb_rows in pairs(tb_data.award_rank) do
				for n_row, tb_row in pairs(tb_rows) do
					tb_row[2] = floor(tb_row[2] * WLLS_CFG.NHAN_DIEM_HANG)
					for k = 4, getn(tb_row) do
						if (type(tb_row[k]) == "table" and tb_row[k].nCount ~= nil) then
							tb_row[k].nCount = ceil(tb_row[k].nCount * WLLS_CFG.NHAN_ITEM_HANG)
						end
					end
				end
			end
		end
	end
end
