-- [LOCAL54 06/09 toi] 1 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local BotAuto_Xem
-- @IncludeOnce  [LUA54 06/09 toi] tep toan ham: Lua54Dll chi chay than mot lan moi state (chon_includeonce.py)
-- ============================================================================
-- BOT_AUTO.LUA - tu goi bot va tu phan cong viec moi lan may chu khoi dong
-- [BOTAUTO 04/09] Chu game: "moi lan mo game len tu goi 1000 bot; tu keu 200 bot
-- di lam Da Tau; tu keu 200 ve bay sap ban hang; tu mo gioi han 500 bot Tong Kim
-- va bat che do tu dong theo gio".
--
-- KHONG can build lai CoreServer: moi thu deu goi bang ham Lua da co san
-- (dang ky o ScriptFuns.cpp, than ham o KPlayerBot.cpp):
--    PB_AddBot(nTu, nDen)      -> xep dai tai khoan "1".."1000" vao hang doi sinh bot
--    PB_BotCount()             -> so bot dang song, tran engine (PB_MAX_BOTS = 1000)
--    PB_SetDaTau(n / -1)       -> so bot di lam Da Tau (dat / doc)
--    PB_SetBanSap(n / -1)      -> so bot ve thanh bay sap (dat / doc)
--    PB_SetTongKimTran(n / -1) -> tran bot moi tran Tong Kim, 0 = khong gioi han
--    PB_SetTongKim(1/0 / -1)   -> bot tu tham gia Tong Kim THEO GIO (dat / doc)
--
-- Thong so sua o script\cauhinh\ch_chung.lua (khoa BOT_*), khong sua trong tep nay.
-- Nhip: goi tu RunTime() cua timerserver.lua, moi phut mot lan.
--
-- BA DIEU CAN BIET (rut tu chinh ma nguon, de sau nay khoi doan lai):
--  1. PB_MAX_BOTS = 1000 (KPlayerBot.h:93) - xin hon 1000 cung bi kep ve 1000.
--  2. PB_SetDaTau CHI chon bot cap >= 70 (PB_DT_CAP_TOI_THIEU, KPlayerBot.cpp:772)
--     va GIU NGUYEN bot dang lam -> goi lai nhieu lan la an toan, chi bu cho du so.
--     Vi vay so Da Tau that co the < so xin neu chua du bot cap 70.
--  3. PB_SetBanSap thi NGUOC LAI: moi lan goi no DONG het sap cu roi chon lai tu dau
--     (KPlayerBot.cpp:1908-1924) -> KHONG duoc goi moi phut, se lam bot dep sap lien
--     tuc. O day chi goi lai khi so sap tut duoi nguong va da qua BOT_SAP_GIAN phut.
-- ============================================================================

Include("\\script\\cauhinh\\ch_lib.lua")

-- Bien trang thai deu gan TRONG HAM (khong khai o cap tep) de song qua dofile -
-- timerserver.lua tu nap lai chinh no moi phut, khai o cap tep se bi xoa moi lan.
--   g_nBotAutoMoc     : dau moc phut da chay (RunTime co the chay 2-3 lan/phut)
--   g_nBotAutoLen     : so phut may chu da chay ke tu khi tep nay chay lan dau
--   g_nBotAutoGoiMoc  : phut goi bot lan cuoi
--   g_nBotAutoSapMoc  : phut dat ban sap lan cuoi

function BotAuto_Tick()
	if (G_CFG == nil) then
		return 0
	end
	if (G_CFG("BOT_TU_GOI", 0) ~= 1) then
		return 0
	end
	if (PB_BotCount == nil) then
		return 0		-- ban CoreServer khong co he bot
	end

	-- ------------------------------------------------ chi chay MOT lan moi phut
	local nYr, nMo, nDy, nHr, nMi = GetTimeNow()
	local nPhutNay = nDy * 1440 + nHr * 60 + nMi
	if (g_nBotAutoMoc == nil) then
		g_nBotAutoMoc = -99999
		g_nBotAutoLen = 0
	end
	if (nPhutNay == g_nBotAutoMoc) then
		return 0
	end
	g_nBotAutoMoc = nPhutNay
	g_nBotAutoLen = g_nBotAutoLen + 1

	-- Doi may chu on dinh (Goddess phai tra loi duoc thi bot moi sinh duoc).
	local nCho = G_CFG("BOT_CHO_PHUT", 2)
	if (g_nBotAutoLen < nCho) then
		return 0
	end

	-- ------------------------------------------------------------- goi bot
	local nMuon = G_CFG("BOT_SO_LUONG", 1000)
	local nCo, nTran = PB_BotCount()
	if (nMuon > nTran) then
		nMuon = nTran
	end
	if (nCo < nMuon) then
		-- Gian cach: PB_Spawn KHONG loc tai khoan da co bot, no chi xep ten vao
		-- hang doi; goi don dap se lam hang doi luon day. Hang doi tu dung khi
		-- da du PB_MAX_BOTS (KPlayerBot.cpp:12338) nen goi thua khong sinh trung.
		local nGian = G_CFG("BOT_GOI_GIAN", 10)
		if (g_nBotAutoGoiMoc == nil or nPhutNay - g_nBotAutoGoiMoc >= nGian) then
			g_nBotAutoGoiMoc = nPhutNay
			local nXep = PB_AddBot(1, nMuon)
			GhiLog("BOTAUTO", format("[BotAuto] dang co %d/%d bot -> xep them %d tai khoan vao hang doi sinh", nCo, nMuon, nXep))
		end
	end

	-- --------------------------------------------------------- Tong Kim theo gio
	-- Dat moi lan may chu khoi dong (hai bien nay khong duoc luu xuong DB).
	local nTkTran = G_CFG("BOT_TK_TRAN", 500)
	if (PB_SetTongKimTran ~= nil and PB_SetTongKimTran(-1) ~= nTkTran) then
		PB_SetTongKimTran(nTkTran)
		GhiLog("BOTAUTO", format("[BotAuto] tran bot moi tran Tong Kim = %d", nTkTran))
	end
	local nTkBat = G_CFG("BOT_TK_TU_DONG", 1)
	if (PB_SetTongKim ~= nil and PB_SetTongKim(-1) ~= nTkBat) then
		PB_SetTongKim(nTkBat)
		GhiLog("BOTAUTO", format("[BotAuto] bot tu tham gia Tong Kim theo gio = %d", nTkBat))
	end

	-- ------------------------------------------------- VAO PHAI + BAT DANH QUAI
	-- [BOTAUTO2 04/09] chu game: "chay lai server bot chua tu dong mo tu danh -> bot dung yen o thanh".
	-- Hai lenh nay la CHE DO BEN o may chu (s_nPbCheDoNhapMon / s_nPbCheDoDanh): bat mot lan thi bot
	-- sinh sau cung tu vao phai va tu danh. Goi lai dinh ky cho chac - lenh re va khong hai.
	-- [BOTAUTO3] Chu game: bot da tren cap 100 nen KHONG vao phai; chi bat danh quai, va chi khi da
	-- add DU so bot (dang sinh do dang thi bat cung chi bat duoc mot phan).
	-- PB_SetFight(1) nhan ca bot o PB_AI_IDLE (chua vao phai) nen khong can PB_JoinFaction; nhung che
	-- do ben cua may chu chi tu ap cho bot DA vao phai, vay nen phai goi lai dinh ky cho bot moi.
	-- "Xong dot sinh" = da du so, HOAC so bot khong nhuc nhich hai nhip lien tiep (het tai khoan de
	-- sinh chang han) - khong co ve nay thi 998/1000 se treo mai o trang thai dung yen.
	-- [BOTAUTO5] Che do vao phai o may chu la co BEN: bat mot lan thi moi nhip no lai keo bot RANH
	-- sang "dang chay toi NPC mon phai", ma PB_SetFight khong cam duoc bot o trang thai do
	-- (hethong.log 10:16-10:26: "vao phai 1000 bot" lap lai, khong lan nao bat duoc danh quai ->
	-- dung la canh "bot dung yen o thanh"). Chu game khong can vao phai nua nen tat han, ke ca khi
	-- co nguoi lo bam lenh bai. PB_SetNhapMon chi co tu ban CoreServer 04/09 tro di.
	if (G_CFG("BOT_VAO_PHAI", 0) ~= 1 and PB_SetNhapMon ~= nil) then
		local nGo = PB_SetNhapMon(0)
		if (nGo > 0) then
			GhiLog("BOTAUTO", format("[BotAuto] tat che do vao phai, go %d bot dang treo ve ranh", nGo))
		end
	end
	local bXongSinh = (nCo >= nMuon) or (nCo > 0 and nCo == g_nBotAutoCoCu)
	g_nBotAutoCoCu = nCo
	local nGianDanh = G_CFG("BOT_DANH_GIAN", 5)
	if (bXongSinh and (g_nBotAutoDanhMoc == nil or nPhutNay - g_nBotAutoDanhMoc >= nGianDanh)) then
		g_nBotAutoDanhMoc = nPhutNay
		if (G_CFG("BOT_VAO_PHAI", 0) == 1 and PB_JoinFaction ~= nil) then
			local nPhai = PB_JoinFaction()
			if (nPhai > 0) then
				GhiLog("BOTAUTO", format("[BotAuto] ra lenh vao phai cho %d bot", nPhai))
			end
		end
		if (G_CFG("BOT_TU_DANH", 1) == 1 and PB_SetFight ~= nil) then
			local nDanh = PB_SetFight(1)
			if (nDanh > 0) then
				GhiLog("BOTAUTO", format("[BotAuto] du %d bot -> BAT danh quai cho %d bot", nCo, nDanh))
			end
		end
	end

	-- ------------------------------------------------------------------ Da Tau
	-- An toan khi goi lai: chi them cho du so, bot dang lam khong bi dung.
	local nDtMuon = G_CFG("BOT_DA_TAU", 200)
	if (PB_SetDaTau ~= nil and nDtMuon > 0) then
		local nDtCo = PB_SetDaTau(-1)
		if (nDtCo < nDtMuon) then
			local nMoi = PB_SetDaTau(nDtMuon)
			if (nMoi ~= nDtCo) then
				GhiLog("BOTAUTO", format("[BotAuto] Da Tau: %d -> %d bot (xin %d; chi lay bot cap >= 70)", nDtCo, nMoi, nDtMuon))
			end
		end
	end

	-- ---------------------------------------------------------------- ban sap
	-- Goi lai la DEP HET sap cu roi chon lai, nen phai thua: chi lam khi tut
	-- duoi nguong va da qua BOT_SAP_GIAN phut ke tu lan dat truoc.
	local nSapMuon = G_CFG("BOT_BAN_SAP", 200)
	if (PB_SetBanSap ~= nil and nSapMuon > 0) then
		local nSapCo = PB_SetBanSap(-1)
		local nNguong = nSapMuon * G_CFG("BOT_SAP_NGUONG", 60) / 100
		local nGianSap = G_CFG("BOT_SAP_GIAN", 15)
		-- Dang co 0 sap thi goi lai KHONG mat gi (dep 0 sap cu), va rat hay gap: luc
		-- Tong Kim dang mo thi gan nhu moi bot deu co nTk khac 0, ma bo loc ban sap
		-- (KPlayerBot.cpp:1927-1940) loai bot Da Tau / Tong Kim / dang o nhom nguoi that
		-- -> khong con ai de chon. Phai thu lai MOI PHUT cho toi khi tran tan.
		if (nSapCo == 0) then
			nGianSap = 0
		end
		if (nSapCo < nNguong) then
			if (g_nBotAutoSapMoc == nil or nPhutNay - g_nBotAutoSapMoc >= nGianSap) then
				g_nBotAutoSapMoc = nPhutNay
				local nMoi = PB_SetBanSap(nSapMuon)
				GhiLog("BOTAUTO", format("[BotAuto] ban sap: %d -> %d bot (xin %d)", nSapCo, nMoi, nSapMuon))
			end
		end
	end

	-- ------------------------------------------------------- nhip bao trang thai
	-- In dinh ky de biet phan tu dong CO DANG SONG hay khong (cac khoi tren chi in
	-- khi CO THAY DOI, nen may chu dang dung dung so se im lang tuyet doi).
	local nBao = G_CFG("BOT_BAO_PHUT", 10)
	if (nBao > 0) then
		if (g_nBotAutoBaoMoc == nil or nPhutNay - g_nBotAutoBaoMoc >= nBao) then
			g_nBotAutoBaoMoc = nPhutNay
			BotAuto_Xem()
		end
	end

	return 1
end

-- Lenh bai admin / go loi: goi tay de xem trang thai hien tai.
function BotAuto_Xem()
	if (PB_BotCount == nil) then
		return "khong co he bot"
	end
	local nCo, nTran = PB_BotCount()
	local nLen = g_nBotAutoLen
	if (nLen == nil) then
		nLen = 0
	end
	local s = format("bot %d/%d | Da Tau %d | ban sap %d | Tong Kim bat=%d tran=%d | may chu len %d phut",
		nCo, nTran, PB_SetDaTau(-1), PB_SetBanSap(-1),
		PB_SetTongKim(-1), PB_SetTongKimTran(-1),
		nLen)
	GhiLog("BOTAUTO", s)
	return s
end
