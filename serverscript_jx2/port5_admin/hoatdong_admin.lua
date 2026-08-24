-- ============================================================================
-- HOATDONG_ADMIN.LUA - [24/08] Menu test 4 hoat dong PORT5 tren Lenh Bai Admin
--   (Bang Chien tongwar / Bach Nhan bairenleitai / Ty Vo bw / Thanh Bao tongcastle)
-- Duoc Include tu lenhbaiadmin.lua; lenhbaiadmin main() dofile lai chinh no moi
-- lan dung => SUA FILE NAY KHONG CAN RESTART GameServer.
-- Cau hinh so lieu: script\header\cauhinh_hoatdong.lua (1 file chinh tat ca).
-- ============================================================================
Include("\\script\\header\\cauhinh_hoatdong.lua")

HD_TW_RELAY = "\\script\\missions\\tongwar\\relay\\tongwar.lua"
HD_TC_DRV   = "\\script\\missions\\tongcastle\\tongcastle_driver.lua"
HD_TC_RELAY = "\\script\\mission\\tongcastle\\tongcastle.lua"

function HD_AdminMenu()
	SayEx({"<color=yellow>Hoat dong 23-24/08<color>: chon muc test (cau hinh: script\\header\\cauhinh_hoatdong.lua)",
	"1. Bang Chien - Vo Lam De Nhat Bang/HD_TW_Menu",
	"2. Bach Nhan Loi Dai/HD_BR_Menu",
	"3. Ty Vo Dai/HD_BW_Menu",
	"4. Bang Hoi Thanh Bao/HD_TC_Menu",
	"5. Nhan item thuong (test)/HD_Item_Menu",
	"6. BOT Tong Kim: BAT/HD_TK_Bat",
	"7. BOT Tong Kim: TAT/HD_TK_Tat",
	"8. NAP LAI CONFIG (khi khong co tran chay)/HD_ReloadCfg",
	"Ket thuc doi thoai/no"})
end

-- ================= 1) BANG CHIEN =================
function HD_TW_Menu()
	SayEx({"<color=yellow>Bang Chien<color> - mua 9: 29/10-05/11/2026, tran "..HD_GioPhut(HD_CFG("TW_GIO_KHAICHIEN", 2000)).."-"..HD_GioPhut(HD_CFG("TW_GIO_KETTHUC", 2130)).." (can >=2 bang chiem thanh de khoi tao)",
	"Xem trang thai (co tran? file mua?)/HD_TW_TrangThai",
	"B1. Khoi tao mua 9 NGAY (tongwar_initmatch)/HD_TW_InitMatch",
	"B2. Ep tick lich NGAY (TaskContent)/HD_TW_Tick",
	"B3. Ep mo pha 2 - bao danh 3 cap test/HD_TW_Redo",
	"Dich chuyen: NPC bao danh Ba Lang Huyen/HD_TW_Tele53",
	"Dich chuyen: map bao danh 608/HD_TW_Tele608",
	"Dich chuyen: map bao danh 610/HD_TW_Tele610",
	"Dich chuyen: map bao danh 612/HD_TW_Tele612",
	"Quay lai/HD_AdminMenu"})
end

function HD_TW_TrangThai()
	local nTran = GetGlbValue(850)
	Msg2Player("Co van dang mo tran (GlbValue 850) = "..tostring(nTran).." (1 = dang co tran)")
	local f = openfile("jx2league.txt", "r")
	if f then
		local n = 0
		while 1 do
			local dong = read(f, "*l")
			if not dong then break end
			n = n + 1
			if n <= 8 then Msg2Player("jx2league: "..dong) end
		end
		closefile(f)
		Msg2Player("jx2league.txt tong "..n.." dong (7 dong G = da khoi tao mua)")
	else
		Msg2Player("CHUA co jx2league.txt (mua chua khoi tao - bam B1)")
	end
	HD_TW_Menu()
end

function HD_TW_InitMatch()
	DynamicExecute(HD_TW_RELAY, "tongwar_initmatch", 9)
	Msg2Player("Da goi tongwar_initmatch(9) - xem jx2league.txt (can >=2 bang dang chiem thanh)")
end

function HD_TW_Tick()
	DynamicExecute(HD_TW_RELAY, "TaskContent")
	Msg2Player("Da ep TaskContent (tick lich Bang Chien)")
end

function HD_TW_Redo()
	local t = HD_CFG("TW_TEST_REDO", {1, 2, 1, 3, 4, 2, 5, 6, 3})
	DynamicExecute("\\script\\gmscript.lua", "HD_TW_RedoStart", t[1], t[2], t[3], t[4], t[5], t[6], t[7], t[8], t[9])
	Msg2Player("Da ep pha 2 (bao danh) cho 3 cap test - id bang sua o TW_TEST_REDO trong config")
end

function HD_TW_Tele53()  NewWorld(53, 1628, 3173) end
function HD_TW_Tele608() NewWorld(608, 1582, 3174) end
function HD_TW_Tele610() NewWorld(610, 1588, 3160) end
function HD_TW_Tele612() NewWorld(612, 1604, 3147) end

-- ================= 2) BACH NHAN =================
function HD_BR_Menu()
	SayEx({"<color=yellow>Bach Nhan Loi Dai<color> - mo "..HD_GioPhut(HD_CFG("BR_GIO_MO", 1200)).."-24:00 hang ngay, map 960, cap >="..HD_CFG("BR_CAP_TOITHIEU", 90),
	"Xem cau hinh dang chay/HD_BR_TrangThai",
	"Dich chuyen: NPC loi vao (Lam An)/HD_BR_TeleNpc",
	"Dich chuyen: vao map 960 (dai 1)/HD_BR_Tele960",
	"Goi lai Init (CHI khi boot loi)/HD_BR_Init",
	"Quay lai/HD_AdminMenu"})
end

function HD_BR_TrangThai()
	Msg2Player(format("Gio mo %s | exp %d/%d phut (Loi Chu +%d) | tran %d luot/ngay | buff x2: %d%% nguoi, moi %d phut",
		HD_GioPhut(HD_CFG("BR_GIO_MO", 1200)), HD_CFG("BR_EXP_TICK", 1000000), HD_CFG("BR_PHUT_CHUKY_EXP", 5),
		HD_CFG("BR_EXP_LOICHU", 2000000), HD_CFG("BR_TRAN_LUOT_NGAY", 50),
		floor(HD_CFG("BR_TILE_BUFFX2", 0.2)*100), HD_CFG("BR_PHUT_COTHU", 30)))
	Msg2Player(format("Da dung hom nay (task 2709): %d luot", GetTaskDailyCount and GetTaskDailyCount(2709) or -1))
	HD_BR_Menu()
end

function HD_BR_TeleNpc() NewWorld(HD_CFG("BR_NPC_MAP", 176), 1464, 3223) end
function HD_BR_Tele960() NewWorld(960, 1784, 3099) end
function HD_BR_Init()
	DynamicExecute("\\script\\missions\\bairenleitai\\bairen_boot.lua", "BairenLeitai_Adm_Init")
	Msg2Player("Da goi BairenLeitai_Adm_Init (CHU Y: goi lap se tao them timer)")
end

-- ================= 3) TY VO =================
function HD_BW_Menu()
	SayEx({"<color=yellow>Ty Vo Dai<color> - khong co lich, 2 doi truong (moi doi du 2 nguoi) bao danh o Cong Binh Tu; tran "..HD_CFG("BW_PHUT_TRAN", 12).." phut (cho "..HD_CFG("BW_PHUT_CHO", 2).." phut)",
	"Dich chuyen: Cong Binh Tu DUONG CHAU/HD_BW_Tele80",
	"Dich chuyen: Cong Binh Tu TUONG DUONG/HD_BW_Tele78",
	"Dich chuyen: Cong Binh Tu THANH DO/HD_BW_Tele11",
	"Vao dau truong 209 (cho khan gia)/HD_BW_Tele209",
	"Go ket tran treo cua TOI (task 200/2340-2342)/HD_BW_GoKet",
	"Quay lai/HD_AdminMenu"})
end

function HD_BW_Tele80()  NewWorld(80, 1659, 3020) end
function HD_BW_Tele78()  NewWorld(78, 1464, 3183) end
function HD_BW_Tele11()  NewWorld(11, 3071, 5002) end
function HD_BW_Tele209() NewWorld(209, 1598, 3216) end

function HD_BW_GoKet()
	SetTaskTemp(200, 0)
	SetTask(2340, 0)
	SetTask(2341, 0)
	SetTask(2342, 0)
	Msg2Player("Da xoa co trong-tran (TaskTemp 200) + vi tri luu (task 2340-2342) cua nhan vat nay")
end

-- ================= 4) THANH BAO =================
function HD_TC_Menu()
	SayEx({"<color=yellow>Bang Hoi Thanh Bao<color> - thu "..HD_CFG("TC_THU", 0).." (0=CN), map 984 (CLIENT CHUA CO DATA - vao se den man)",
	"Hoi sinh cay 1 (Thanh Dong) NGAY/HD_TC_Cay1",
	"Hoi sinh cay 2 (Bach Ngan) NGAY/HD_TC_Cay2",
	"Hoi sinh cay 3 (Hoang Kim) NGAY/HD_TC_Cay3",
	"Xoa het cay + record (don test)/HD_TC_XoaCay",
	"Xem diem Than Moc cua toi/HD_TC_XemDiem",
	"Cong 500 diem Than Moc (test doi lenh bai)/HD_TC_CongDiem",
	"Dich chuyen: NPC Thanh Bao (Lam An)/HD_TC_TeleNpc",
	"Vao map 984 (CANH BAO: client den man)/HD_TC_Tele984",
	"Quay lai/HD_AdminMenu"})
end

function HD_TC_Cay1() DynamicExecute(HD_TC_DRV, "TONGCASTLE_Adm_ReviveTree", 1) Msg2Player("Da ep hoi sinh cay loai 1 o map 984") end
function HD_TC_Cay2() DynamicExecute(HD_TC_DRV, "TONGCASTLE_Adm_ReviveTree", 2) Msg2Player("Da ep hoi sinh cay loai 2 o map 984") end
function HD_TC_Cay3() DynamicExecute(HD_TC_DRV, "TONGCASTLE_Adm_ReviveTree", 3) Msg2Player("Da ep hoi sinh cay loai 3 o map 984") end

function HD_TC_XoaCay()
	DynamicExecute(HD_TC_RELAY, "tbS3TongCastle:DelAllTreeData", 1)
	DynamicExecute(HD_TC_RELAY, "tbS3TongCastle:DelAllTreeData", 2)
	DynamicExecute(HD_TC_RELAY, "tbS3TongCastle:DelAllTreeData", 3)
	Msg2Player("Da xoa record cay ca 3 loai o relay (cay dang dung trong map se bi don o tick sau)")
end

function HD_TC_XemDiem()
	Msg2Player("Diem Than Moc (task 3399) = "..GetTask(3399))
	HD_TC_Menu()
end

function HD_TC_CongDiem()
	SetTask(3399, GetTask(3399) + 500)
	Msg2Player("Da cong 500 diem Than Moc -> hien co "..GetTask(3399))
end

function HD_TC_TeleNpc() NewWorld(176, 1663, 3262) end
function HD_TC_Tele984() NewWorld(984, 1311, 3515) end

-- ================= 5) ITEM THUONG TEST =================
function HD_Item_Menu()
	SayEx({"Nhan item thuong de test (genre 6/detail 1)",
	"Qua Dai Hoang Kim 4864 ("..floor(HD_CFG("QDHK_EXP", 200000000)/1e6).."tr exp) x1/HD_It4864",
	"Hoang Chan Don 2273 ("..floor(HD_CFG("HCD_EXP", 2000000000)/1e6).."tr exp co tran) x1/HD_It2273",
	"Bua trieu Thu Ve 3204 x5/HD_It3204",
	"Thanh Dong Than Moc Lenh 3205 x1/HD_It3205",
	"Bach Ngan Than Moc Lenh 3206 x1/HD_It3206",
	"Hoang Kim Than Moc Lenh 3207 x1/HD_It3207",
	"Han nguyen chan dan 4857 x1/HD_It4857",
	"Quay lai/HD_AdminMenu"})
end

function HD_It4864() AddItem(6, 1, 4864, 0, 0, 0) Msg2Player("Da nhan Qua Dai Hoang Kim") end
function HD_It2273() AddItem(6, 1, 2273, 0, 0, 0) Msg2Player("Da nhan Hoang Chan Don") end
function HD_It3204()
	for i = 1, 5 do AddItem(6, 1, 3204, 0, 0, 0) end
	Msg2Player("Da nhan 5 Bua trieu Thu Ve")
end
function HD_It3205() AddItem(6, 1, 3205, 0, 0, 0) Msg2Player("Da nhan Thanh Dong Than Moc Lenh") end
function HD_It3206() AddItem(6, 1, 3206, 0, 0, 0) Msg2Player("Da nhan Bach Ngan Than Moc Lenh") end
function HD_It3207() AddItem(6, 1, 3207, 0, 0, 0) Msg2Player("Da nhan Hoang Kim Than Moc Lenh") end
function HD_It4857() AddItem(6, 1, 4857, 0, 0, 0) Msg2Player("Da nhan Han nguyen chan dan") end

-- ================= 6/7) BOT TONG KIM =================
function HD_TK_Bat() PB_SetTongKim(1) Msg2Player("Da BAT bot tu tham gia Tong Kim (PB_SetTongKim 1)") end
function HD_TK_Tat() PB_SetTongKim(0) Msg2Player("Da TAT bot tu tham gia Tong Kim (PB_SetTongKim 0)") end

-- ================= 8) NAP LAI CONFIG =================
-- ReLoadScript = chay lai ca file trong state cua no => moi HD_CFG/%upvalue
-- deu nhan gia tri moi tu cauhinh_hoatdong.lua vua sua tren dia.
-- CHU Y: KHONG bam khi dang co tran chay (bang trang thai trong file se bi reset).
HD_RELOAD_LIST = {
	"\\script\\missions\\tongwar\\relay\\tongwar.lua",
	"\\script\\event\\tongwar\\tongwar_signup.lua",
	"\\script\\missions\\tongwar\\trap\\tongwar_trap.lua",
	"\\script\\missions\\bairenleitai\\hundred_arena.lua",
	"\\script\\missions\\bairenleitai\\bairen_boot.lua",
	"\\script\\missions\\bw\\bwmanager.lua",
	"\\script\\missions\\tongcastle\\tongcastle.lua",
	"\\script\\mission\\tongcastle\\tongcastle.lua",
	"\\script\\missions\\tongcastle\\guideperson.lua",
	"\\script\\missions\\tongcastle\\treedeath.lua",
	"\\script\\missions\\tongcastle\\guard.lua",
	"\\script\\missions\\tongcastle\\shenmuling.lua",
	"\\script\\item\\huangzhendan.lua",
	"\\script\\vng_event\\item\\biggoldenseed.lua",
}

function HD_ReloadCfg()
	Include("\\script\\header\\cauhinh_hoatdong.lua")
	local nOk = 0
	for i = 1, getn(HD_RELOAD_LIST) do
		ReLoadScript(HD_RELOAD_LIST[i])
		nOk = nOk + 1
	end
	Msg2Player("Da nap lai CONFIG + "..nOk.." file tinh nang. Cac muc [RESTART] trong config van can restart GS.")
end
