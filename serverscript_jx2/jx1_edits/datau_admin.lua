-- ============================================================================
-- DA TAU - CONG CU ADMIN (19/08/2026 - yeu cau chu game)
--   1. Xoa phat "huy nhieu nhiem vu khong lam duoc" -> cho lam tiep NGAY
--   2. Them co hoi (luot) huy nhiem vu
--
-- Goi tu lenhbaiadmin.lua: muc "Da Tau: xoa phat huy + them luot huy/DT_AdminMenu".
-- Chi dung ham ENGINE (GetTask/SetTask/SyncTaskValue/GetByte/SetByte/Msg2Player)
-- va SayEx/format cua lib_ham.lua, KHONG phu thuoc tasklink_head.lua da nap hay chua.
--
-- Cac so duoi day doc tu MA THAT, khong doan:
--   1020 byte 4 = so luot huy con lai      (tasklink_head.lua:488-500 tl_settaskstate;
--                 tran 254 la do chinh he thong dat)
--   1046        = ban sao chong gian lan   (tasklink_head.lua:21 DEBUG_TASKVALUE;
--                 seasonnpc.lua:1121 _CancelTaskDebug so 1046 voi 1020-byte4, LECH la
--                 CAM huy vinh vien + ghi log gian lan => sua mot cai PHAI sua ca hai)
--   1036        = so lan huy o dau chuoi; ==10 nghia la DANG BI PHAT
--                 (seasonnpc.lua:676-683 dat, :243-251 doc)
--   1029        = moc thoi gian bi phat; phai qua 10890 tick (~10 phut) he thong moi
--                 tu tha (seasonnpc.lua:243-245)
--   2420        = so nhiem vu da lam trong ngay, tran 40 (seasonnpc.lua:47-50)
--   2797        = so lan huy trong ngay; CHI dung tinh thuong moc 30/40
--                 (seasonnpc.lua:106-112), khong chan gi ca
-- ============================================================================

DT_ADM_THEMLUOT = 10      -- moi lan bam thi cong them bao nhieu luot huy
-- TRAN THAT SU LA 127, KHONG PHAI 254 nhu tl_settaskstate ghi:
--   LuaGetByte (ScriptFuns.cpp:121-134) doc byte 4 bang (x & (0xff<<24)) >> 24 tren
--   int CO DAU, nen byte >= 128 doc ra SO AM. Khi do Task_Cancel (seasonnpc.lua:698)
--   thay "luot huy < 1" va roi vao nhanh else :705-725 = XOA SACH chuoi nhiem vu
--   cua nguoi choi. De 100 cho con bien an toan.
DT_ADM_TRANLUOT = 100
DT_ADM_ENDSAY   = "Ket thuc doi thoai./no"

-- Dat so luot huy: PHAI ghi CA 1020-byte4 LAN ban sao 1046.
-- doc so luot huy hien tai, tu chua neu byte 4 da bi day qua 127 (doc ra so am)
function DT_AdminDocLuot()
	local n = GetByte(GetTask(1020), 4)
	if (n < 0) then
		return -1
	end
	return n
end

function DT_AdminDatLuot(nGiaTri)
	if (nGiaTri < 0) then
		nGiaTri = 0
	end
	if (nGiaTri > DT_ADM_TRANLUOT) then
		nGiaTri = DT_ADM_TRANLUOT
	end
	SetTask(1020, SetByte(GetTask(1020), 4, nGiaTri))
	SyncTaskValue(1020)
	SetTask(1046, nGiaTri)
	SyncTaskValue(1046)
	return nGiaTri
end

function DT_AdminMenu()
	local nPhat = GetTask(1036)
	local nLuot = DT_AdminDocLuot()
	local nNgay = GetTask(2420)
	local nHuy  = GetTask(2797)
	local szPhat = "khong bi phat"
	if (nPhat >= 10) then
		szPhat = "DANG BI PHAT (cho ~10 phut, hoac bam xoa phat)"
	elseif (nPhat > 0) then
		szPhat = format("%d/3 lan huy dau chuoi (qua 3 la bi phat)", nPhat)
	end
	local szLuot = format("%d", nLuot)
	if (nLuot < 0) then
		szLuot = "HONG (byte tran qua 127) - bam dat lai ve 0 de chua"
	end
	SayEx({format("<color=yellow>Da Tau - cong cu admin<color>" ..
			"\nTrang thai: <color=green>%s<color>" ..
			"\nLuot huy con lai: <color=gold>%s<color>" ..
			"\nHom nay da lam: <color=gold>%d<color>/40  (da huy %d lan)",
			szPhat, szLuot, nNgay, nHuy),
		"Xoa phat - cho lam tiep nhiem vu ngay/DT_AdminXoaPhat",
		format("Them %d luot huy nhiem vu/DT_AdminThemLuot", DT_ADM_THEMLUOT),
		"Dat lai luot huy ve 0/DT_AdminXoaLuot",
		"Dat lai so nhiem vu hom nay = 0 (an lai duoc thuong moc 30/40)/DT_AdminXoaNgay",
		DT_ADM_ENDSAY})
end

function DT_AdminXoaPhat()
	SetTask(1036, 0)          -- so lan huy dau chuoi (==10 la dang bi phat)
	SyncTaskValue(1036)
	SetTask(1029, 0)          -- moc thoi gian bi phat
	SyncTaskValue(1029)
	-- Chua luon truong hop ban sao 1046 LECH voi luot huy that: khi lech thi
	-- _CancelTaskDebug (seasonnpc.lua:1121) CAM huy vinh vien va nguoi choi ket
	-- han o nhiem vu khong lam duoc. Ghi lai 1046 = dung so luot dang co, KHONG
	-- cong them cho ai.
	local nHienCo = DT_AdminDocLuot()
	if (nHienCo < 0) then
		nHienCo = 0
	end
	DT_AdminDatLuot(nHienCo)
	Msg2Player("[Da Tau] Da xoa phat: gap NPC Da Tau la nhan nhiem vu tiep duoc ngay.")
	DT_AdminMenu()
end

function DT_AdminThemLuot()
	local nCu = DT_AdminDocLuot()
	if (nCu < 0) then
		nCu = 0
	end
	local nMoi = DT_AdminDatLuot(nCu + DT_ADM_THEMLUOT)
	Msg2Player(format("[Da Tau] Luot huy nhiem vu: %d -> %d", nCu, nMoi))
	DT_AdminMenu()
end

function DT_AdminXoaLuot()
	DT_AdminDatLuot(0)
	Msg2Player("[Da Tau] Da dat lai luot huy ve 0.")
	DT_AdminMenu()
end

function DT_AdminXoaNgay()
	SetTask(2420, 0)
	SyncTaskValue(2420)
	SetTask(2797, 0)
	SyncTaskValue(2797)
	Msg2Player("[Da Tau] Da dat lai so nhiem vu hom nay = 0 (lam lai duoc tu dau).")
	DT_AdminMenu()
end
