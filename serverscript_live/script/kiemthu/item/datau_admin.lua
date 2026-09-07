-- @IncludeOnce  [LUA54 06/09 toi] tep toan ham: Lua54Dll chi chay than mot lan moi state (chon_includeonce.py)
-- ============================================================================
-- DA TAU - CONG CU ADMIN (19/08/2026 - yeu cau chu game)
--   1. Xoa phat "huy nhieu nhiem vu khong lam duoc" -> cho lam tiep NGAY
--   2. Them co hoi (luot) huy nhiem vu
-- Goi tu lenhbaiadmin.lua: muc "Da Tau: xoa phat huy + them luot huy/DT_AdminMenu".
--
-- SUA CHO AI: mac dinh sua chinh GM dang bam lenh bai. Bam "Chon nhan vat" de sua
-- cho nguoi choi khac (khuon quanly.lua: FindPlayer + SetTaskTemp(TMP_INDEX_PLAYER)
-- roi doi bien toan cuc PlayerIndex, xong TRA LAI). GetTask/SetTask deu bam theo
-- PlayerIndex nen khong doi thi chi sua duoc chinh minh.
--
-- Cac so duoi day doc tu MA THAT (dan theo TEN HAM cho khoi lech so dong):
--   1020 byte 4 = so luot huy con lai  (tasklink_head.lua ham tl_gettaskstate /
--                 tl_settaskstate; seasonnpc.lua ham SelectAward_Cancel cong 1)
--   1046        = ban sao chong gian lan (DEBUG_TASKVALUE trong tasklink_head.lua).
--                 seasonnpc.lua ham _CancelTaskDebug so 1046 voi 1020-byte4: LECH la
--                 CAM huy vinh vien + ghi log gian lan => doi mot cai PHAI doi ca hai
--                 (rieng 1046 == 0 thi luon cho qua).
--   1036        = so lan huy o dau chuoi; ==10 la DANG BI PHAT (seasonnpc.lua ham
--                 Task_Cancel dat, ham tasklink_entence doc)
--   1029        = moc thoi gian bi phat (xem BAY o ham DT_AdminXoaPhat ben duoi)
--   2420        = so nhiem vu da lam trong ngay, tran 40 (ham checkTask_Limit)
--   2797        = so lan huy trong ngay; chi dung tinh thuong moc 30/40
--                 (ham Task_NewVersionAward), khong chan gi ca
--
-- GHI CHU: KHONG goi SyncTaskValue - o ban JX1 no la HAM RONG (KTongJX2.cpp
-- LuaJX2_SyncTaskValue chi push roi return), va id >= 256 cung khong toi client
-- dung duoc (TASK_VALUE_SYNC.nTaskId la BYTE). Nguoi choi xem so luot huy qua
-- cau thoai cua NPC Da Tau.
-- ============================================================================

DT_ADM_THEMLUOT = 10      -- moi lan bam thi cong them bao nhieu luot huy
-- TRAN THAT SU LA 127, KHONG PHAI 254 nhu tl_settaskstate ghi:
--   LuaGetByte (ScriptFuns.cpp ham LuaGetByte) doc byte 4 bang
--   (x & (0xff<<24)) >> 24 tren int CO DAU, nen byte >= 128 doc ra SO AM. Khi do
--   Task_Cancel thay "luot huy < 1" va roi vao nhanh else = XOA SACH chuoi nhiem
--   vu cua nguoi choi. De 100 cho con bien an toan.
DT_ADM_TRANLUOT = 100
DT_ADM_ENDSAY   = "Ket thuc doi thoai./no"

-- ---------------------------------------------------------------- muc tieu

-- Tra chi so nhan vat dang duoc chon; 0 = chinh GM dang bam lenh bai.
function DT_AdminMucTieu()
	local n = GetTaskTemp(TMP_INDEX_PLAYER)
	if (n == nil) then
		return 0
	end
	if (n <= 0) then
		return 0
	end
	return n
end

function DT_AdminChonNV()
	OpenGetString("Ten nhan vat can sua (phai dang online)", "DT_AdminChonNV_OK")
end

function DT_AdminChonNV_OK()
	local szTen = GetStringFromUI()
	local nIdx = FindPlayer(szTen)
	if (nIdx == nil or nIdx == 0) then
		SetTaskTemp(TMP_INDEX_PLAYER, 0)
		Msg2Player("[Da Tau] Khong thay nhan vat dang online, quay ve sua cho chinh minh.")
	else
		SetTaskTemp(TMP_INDEX_PLAYER, nIdx)
		Msg2Player(format("[Da Tau] Da chon nhan vat: %s", szTen))
	end
	DT_AdminMenu()
end

function DT_AdminBoChon()
	SetTaskTemp(TMP_INDEX_PLAYER, 0)
	Msg2Player("[Da Tau] Da bo chon - tu gio sua cho chinh minh.")
	DT_AdminMenu()
end

-- ---------------------------------------------------------------- doc/ghi

-- doc so luot huy hien tai; -1 = byte da bi day qua 127 (doc ra so am)
function DT_AdminDocLuot()
	local n = GetByte(GetTask(1020), 4)
	if (n < 0) then
		return -1
	end
	return n
end

-- Dat so luot huy: PHAI ghi CA 1020-byte4 LAN ban sao 1046.
function DT_AdminDatLuot(nGiaTri)
	if (nGiaTri < 0) then
		nGiaTri = 0
	end
	if (nGiaTri > DT_ADM_TRANLUOT) then
		nGiaTri = DT_ADM_TRANLUOT
	end
	SetTask(1020, SetByte(GetTask(1020), 4, nGiaTri))
	SetTask(1046, nGiaTri)
	return nGiaTri
end

-- ---------------------------------------------------------------- menu

function DT_AdminMenu()
	local nGoc = PlayerIndex
	local nMuc = DT_AdminMucTieu()
	if (nMuc > 0) then
		PlayerIndex = nMuc
	end
	local szAi = GetName()
	local nPhat = GetTask(1036)
	local nLuot = DT_AdminDocLuot()
	local nNgay = GetTask(2420)
	local nHuy  = GetTask(2797)
	PlayerIndex = nGoc

	local szPhat = "khong bi phat"
	if (nPhat >= 10) then
		szPhat = "DANG BI PHAT (cho ~10 phut)"
	elseif (nPhat > 0) then
		szPhat = format("da huy %d lan o dau chuoi (qua 3 lan la bi phat)", nPhat)
	end
	local szLuot = format("%d", nLuot)
	if (nLuot < 0) then
		szLuot = "HONG (tran 127) - dat lai ve 0"
	end
	-- TRAN 512 BYTE: tieu de + tat ca nhan dung CHUNG mot dem
	-- (MAX_SCIRPTACTION_BUFFERNUM, sUiAppendAnswer cat AM THAM khong bao loi).
	-- Ban nay ~430 byte luc xau nhat. Them muc moi thi PHAI do lai.
	SayEx({format("<color=yellow>Da Tau - admin<color>" ..
			"\nNV: %s" ..
			"\nPhat: %s" ..
			"\nLuot huy: %s" ..
			"\nHom nay: %d/40 (huy %d)",
			szAi, szPhat, szLuot, nNgay, nHuy),
		"Xoa phat - lam tiep duoc ngay/DT_AdminXoaPhat",
		format("Them %d luot huy/DT_AdminThemLuot", DT_ADM_THEMLUOT),
		"Dat luot huy = 0/DT_AdminXoaLuot",
		"Dat so nhiem vu hom nay = 0/DT_AdminXoaNgay",
		"Chon nhan vat khac/DT_AdminChonNV",
		"Bo chon (sua cho minh)/DT_AdminBoChon",
		DT_ADM_ENDSAY})
end

-- ---------------------------------------------------------------- viec

function DT_AdminXoaPhat()
	local nGoc = PlayerIndex
	local nMuc = DT_AdminMucTieu()
	if (nMuc > 0) then
		PlayerIndex = nMuc
	end
	-- Dat 1036 = 0 la DU de go phat: nhanh phat trong ham tasklink_entence CHI chay
	-- khi 1036 == 10, nen sau lenh nay moc thoi gian 1029 khong con duoc doc nua.
	-- BAY (quan trong): TUYET DOI khong "reset rieng 1029 ve 0" ma de nguyen 1036 = 10 -
	-- khi do n = GetGameTime() - 0 rat lon, nhanh else se goi Task_Confirm() va
	-- Task_Confirm XOA SACH tien do chuoi (dat lai state 1/2/3/4 va 1046 = 0).
	SetTask(1036, 0)
	-- Chua luon truong hop ban sao 1046 LECH voi luot huy that: khi lech thi
	-- _CancelTaskDebug CAM huy vinh vien va nguoi choi ket han o nhiem vu khong lam
	-- duoc. Ghi lai 1046 = dung so luot dang co, KHONG cong them cho ai.
	local nHienCo = DT_AdminDocLuot()
	if (nHienCo < 0) then
		nHienCo = 0
	end
	DT_AdminDatLuot(nHienCo)
	local szAi = GetName()
	PlayerIndex = nGoc
	Msg2Player(format("[Da Tau] %s: da xoa phat, gap NPC Da Tau la lam tiep duoc ngay.", szAi))
	DT_AdminMenu()
end

function DT_AdminThemLuot()
	local nGoc = PlayerIndex
	local nMuc = DT_AdminMucTieu()
	if (nMuc > 0) then
		PlayerIndex = nMuc
	end
	local nCu = DT_AdminDocLuot()
	if (nCu < 0) then
		nCu = 0
	end
	local nMoi = DT_AdminDatLuot(nCu + DT_ADM_THEMLUOT)
	local szAi = GetName()
	PlayerIndex = nGoc
	Msg2Player(format("[Da Tau] %s: luot huy nhiem vu %d -> %d", szAi, nCu, nMoi))
	DT_AdminMenu()
end

function DT_AdminXoaLuot()
	local nGoc = PlayerIndex
	local nMuc = DT_AdminMucTieu()
	if (nMuc > 0) then
		PlayerIndex = nMuc
	end
	DT_AdminDatLuot(0)
	local szAi = GetName()
	PlayerIndex = nGoc
	Msg2Player(format("[Da Tau] %s: da dat lai luot huy ve 0.", szAi))
	DT_AdminMenu()
end

-- CANH BAO KINH TE: 2420 ve 0 thi bo dem di qua moc 30 THEM MOT LAN => phat lai
-- 30 trieu exp (ham Task_NewVersionAward). CO Y khong dong toi 2797 (so lan huy
-- trong ngay): giu nguyen thi nguoi da huy van khong an lai duoc moc 40
-- (100 trieu exp + 5 ruong) - moc do doi "ca ngay khong huy lan nao".
function DT_AdminXoaNgay()
	local nGoc = PlayerIndex
	local nMuc = DT_AdminMucTieu()
	if (nMuc > 0) then
		PlayerIndex = nMuc
	end
	SetTask(2420, 0)
	local szAi = GetName()
	PlayerIndex = nGoc
	Msg2Player(format("[Da Tau] %s: da dat lai so nhiem vu hom nay = 0"
		.." (CANH BAO: nguoi nay se an LAI thuong moc 30).", szAi))
	DT_AdminMenu()
end
