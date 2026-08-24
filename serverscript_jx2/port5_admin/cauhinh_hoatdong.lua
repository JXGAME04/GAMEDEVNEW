-- ============================================================================
-- CAUHINH_HOATDONG.LUA - CONFIG TRUNG TAM 4 HOAT DONG PORT 23-24/08/2026
--   1) BANG CHIEN / VO LAM DE NHAT BANG (tongwar)  - tien to TW_
--   2) BACH NHAN LOI DAI (bairenleitai)            - tien to BR_
--   3) TY VO DAI (bw)                              - tien to BW_
--   4) BANG HOI THANH BAO (tongcastle)             - tien to TC_
--   + item thuong dung chung (HCD_ / QDHK_)
--
-- CACH DUNG:
--   * Sua so trong bang tbCHD duoi day roi:
--       - Cach 1 (chac chan 100%): RESTART GameServer.
--       - Cach 2 (khong can restart): dung Lenh Bai Admin -> "Hoat dong moi
--         (test)" -> "Nap lai CONFIG" (chi ap dung cac muc danh dau [LIVE];
--         cac muc [RESTART] van phai restart; KHONG bam khi dang co tran chay).
--   * Muc danh dau [ENGINE] KHONG chinh duoc o day - ghi ro file settings
--     phai sua (sua xong bat buoc restart).
--   * FILE NAY LA "LA": CAM Include bat cu gi vao day (bai hoc vong Include
--     23/08 lam GameServer boot stack Overflow).
-- ============================================================================

tbCHD = {

-- ===========================================================================
-- [1] BANG CHIEN (tongwar) - danh bang vs bang theo mua, map 605-607
-- ===========================================================================
-- Cap toi thieu de bao danh + vao dau truong. [LIVE]
TW_CAP_TOITHIEU = 90,

-- Gio khai chien / gio dong tran, dang HHMM (2000 = 20:00, 2130 = 21:30).
-- Driver tick 15 phut mot lan, cua so khop +-5 phut quanh moc. [LIVE]
-- LUU Y: text NPC o event\tongwar\head.lua:492 dang ghi "08:00~08:29 /
-- 08:30~09:30" - do la text hien thi cu cua VNG, gio chay THAT la 2 so nay.
TW_GIO_KHAICHIEN = 2000,
TW_GIO_KETTHUC = 2130,

-- Ngay mua 9: {bao danh, ti thi bat dau, ti thi ket thuc, linh thuong bat dau,
-- linh thuong ket thuc} dang YYMMDD. [LIVE - ap dung tick ke tiep]
-- !! DOI NGAY PHAI GIU NGAY-TRONG-THANG la 29,30,31,1,2,3,5 (SCHEDULE_TABLE
-- va TAB_CALENDAR khoa theo ngay-trong-thang; muon doi han thi phai sua
-- relay\tongwar.lua SCHEDULE_TABLE + event\tongwar\head.lua:517,543 +
-- headinfo.lua:125 TONGWAR_CALENDAR - 5 cho, xem BANGIAO_PORT5_2308.md).
TW_MUA9 = {261029, 261029, 261105, 261106, 261111},

-- Do dai tran (phut) va thoi gian chuan bi truoc khai chien (phut). [RESTART]
-- Nen de TW_PHUT_TRAN khop voi (TW_GIO_KETTHUC - TW_GIO_KHAICHIEN).
TW_PHUT_TRAN = 90,
TW_PHUT_CHUANBI = 30,

-- Nguoi moi phe: tran vao tran + so nguoi toi thieu (duoi muc = xu thua).
-- [RESTART]
TW_NGUOI_TOIDA = 150,
TW_NGUOI_TOITHIEU = 5,

-- So giay toi da nam o hau doanh (qua = day ra ngoai). [RESTART]
TW_GIAY_HAUDOANH = 120,

-- So mang chet toi da 1 tran (du = bi loai khoi tran). [LIVE]
TW_SO_MANG = 10,

-- Diem cong khi giet 1 dich (nhan he so quan ham RANK_PKBONUS o
-- missions\tongwar\head.lua:44) va thuong lien tram (moi 3 mang lien tiep).
-- [RESTART]
TW_DIEM_KILL = 75,
TW_DIEM_LIENTRAM = 150,

-- Bang thuong cuoi mua (nil = dung bang goc TAB_AWORD_GOOD o
-- event\tongwar\headinfo.lua:145 - Qua Dai HK 4864 / Hoang Chan Don 2273 /
-- Cuong Lan 4491 / Han nguyen 4857 / Phi Phong 3476, kem so luong theo hang
-- 1..7). Muon doi thi chep nguyen bang goc vao day roi sua. [RESTART]
TW_THUONG = nil,

-- 9 so cho nut test "ep pha 2" tren Lenh Bai Admin = 3 cap dau + 3 san:
-- {tongA1,tongB1,san1, tongA2,tongB2,san2, tongA3,tongB3,san3}
-- (id bang lay tu jx2league.txt sau khi khoi tao mua). [LIVE]
TW_TEST_REDO = {1, 2, 1, 3, 4, 2, 5, 6, 3},

-- [ENGINE] Khong chinh o day:
--   * Lich tick driver: settings\TimerTask.txt dong khoa 54 (30 giay/lan).
--   * Mission slot 33: settings\task\missions.txt dong 34.
--   * Map 605-613: settings\MapList.ini + WorldSet.ini.

-- ===========================================================================
-- [2] BACH NHAN LOI DAI (bairenleitai) - map 960, vao tu NPC Lam An (176)
-- ===========================================================================
-- Cap toi thieu. [LIVE]
BR_CAP_TOITHIEU = 90,

-- Gio mo trong ngay dang HHMM (1200 = 12:00 trua) va gio dong dang GIO
-- (0 = 0h dem - tra nguoi ve). [LIVE]
BR_GIO_MO = 1200,
BR_GIO_DONG_H = 0,

-- Exp moi tick: chu ky (phut), exp nguoi trong map, exp CONG THEM cho Loi Chu
-- dang giu dai. Co buff Co Thu thi exp thuong x2. [LIVE]
BR_PHUT_CHUKY_EXP = 5,
BR_EXP_TICK = 1000000,
BR_EXP_LOICHU = 2000000,

-- Tran so tick exp / nguoi / ngay (task daily 2709). [LIVE]
BR_TRAN_LUOT_NGAY = 50,

-- NPC Co Thu (phat buff x2): chu ky xuat hien (phut) va ti le so nguoi duoc
-- buff moi dot (0.2 = 20%). [LIVE]
BR_PHUT_COTHU = 30,
BR_TILE_BUFFX2 = 0.2,

-- Nhip dau: giay cho nguoi khieu chien (het gio -> goi NPC cao thu),
-- giay dem nguoc truoc khi danh, phut moi luot dau (het gio -> so sat thuong,
-- Loi Chu thang neu chiu it sat thuong hon). [RESTART]
BR_GIAY_CHO_KHIEUCHIEN = 30,
BR_GIAY_DEM_NGUOC = 3,
BR_PHUT_MOI_LUOT = 3,

-- Phut dung yen toi da (bi da ra) va phut hieu luc buff x2. [RESTART]
BR_PHUT_DUNG_YEN = 90,
BR_PHUT_BUFF_X2 = 30,

-- Chuoi thang toi da 1 doi (den muc = loa "truyen thuyet" + reset).
-- Moi 10 tran thang o dai 1 se loa toan server. [LIVE]
BR_TRAN_CHUOI = 100,

-- Cap goc NPC cao thu (cap that = goc + so_luot_thang/10, tran goc+9). [LIVE]
BR_CAP_NPC_GOC = 90,

-- NPC loi vao (dat luc boot): map / template / cap. [RESTART]
BR_NPC_MAP = 176,
BR_NPC_ID = 1747,
BR_NPC_LEVEL = 95,

-- [ENGINE] Khong chinh o day:
--   * 5 dai + toa do + 10 NPC cao thu 1786-1795: missions\bairenleitai\head.lua
--     (tbArena_Info / tbNpcTypeList - bang lon, sua truc tiep roi RESTART).
--   * Mau NPC cao thu: settings\npcs.txt template 1786-1795.
--   * Toa do NPC loi vao: settings\maps\chrismas\enternpc.txt.

-- ===========================================================================
-- [3] TY VO DAI (bw) - dang ky qua NPC Cong Binh Tu, map dau 209
-- (khong co lich - mo ca ngay; 2 doi truong bao danh la mo tran)
-- ===========================================================================
-- Cap toi thieu (khan gia khong bi chan). [LIVE]
BW_CAP_TOITHIEU = 90,

-- So nguoi toi da moi doi (menu NPC hien "1 vs 1".."8 vs 8" - neu tang qua 8
-- thi menu van chi hien 8 lua chon, sua them text o bwmanager.lua:55). [LIVE]
BW_DOI_TOIDA = 8,

-- Phut cho truoc khi danh (mac dinh 2) va tong phut 1 tran ke ca cho
-- (mac dinh 12 = 2 cho + 10 danh). [RESTART]
BW_PHUT_CHO = 2,
BW_PHUT_TRAN = 12,

-- [ENGINE] Khong chinh o day:
--   * Timer 20/21: settings\TimerTask.txt dong 35-36; mission 11:
--     settings\task\missions.txt dong 12.
--   * 12 NPC Cong Binh Tu (map 80/78/11): script\missions\bw\bw_addnpc.lua.
--   * Toa do vao dai (209): script\missions\bw\bwhead.lua:11-13.

-- ===========================================================================
-- [4] BANG HOI THANH BAO (tongcastle) - map 984, thanh Lam An (176)
-- !! CLIENT CHUA CO DU LIEU MAP 984 - nguoi choi chua vao duoc map cho den
--    khi chu game bo sung pak client (server da san sang).
-- ===========================================================================
-- Cap toi thieu + so phut phai vao bang truoc khi tham gia (1440 = 1 ngay).
-- [LIVE] (ap dung ca 3 cho: guideperson, state chu, treedeath)
TC_CAP_TOITHIEU = 90,
TC_VAO_BANG_PHUT = 1440,

-- Thu mo cua trong tuan: 0 = Chu nhat, 1 = Thu hai ... 6 = Thu bay. [RESTART]
TC_THU = 0,

-- Khung gio: mo cua/cay dot 1 (17:00), cay dot 2 (18:00), cay dot 3 (18:30),
-- dong cua + xoa cay (19:00). Dang {gio, phut}. [RESTART]
-- LUU Y: text thoai NPC o guideperson.lua:90 va :173 ghi cung gio cu -
-- doi lich thi sua text 2 cho do cho khop (chi la hien thi).
TC_GIO_MO = {17, 00},
TC_GIO_CAY2 = {18, 00},
TC_GIO_CAY3 = {18, 30},
TC_GIO_DONG = {19, 00},

-- Khung nhan diem Than Moc (HHMM) + so phut luu tru toi thieu. [LIVE]
TC_GIO_NHANDIEM_TU = 1900,
TC_GIO_NHANDIEM_DEN = 2400,
TC_PHUT_TOITHIEU = 45,

-- Diem cay (nil = bang goc: AttTong 15/30/150, DefendTong 10/20/100,
-- Personal 20/40/200 theo loai cay 1=Thanh Dong, 2=Bach Ngan, 3=Hoang Kim).
-- Muon doi thi mo comment va sua: [LIVE - ap dung phut ke tiep]
-- TC_DIEM_CAY = {
--     ["AttTong"]    = {[1] = 15,  [2] = 30,  [3] = 150},
--     ["DefendTong"] = {[1] = 10,  [2] = 20,  [3] = 100},
--     ["Personal"]   = {[1] = 20,  [2] = 40,  [3] = 200},
-- },
TC_DIEM_CAY = nil,

-- Gia doi Than Moc Lenh (diem Than Moc): 3205 Thanh Dong / 3206 Bach Ngan /
-- 3207 Hoang Kim; tran so lenh doi 1 lan; tran moi loai / tuan. [LIVE]
TC_GIA_LENH_1 = 10,
TC_GIA_LENH_2 = 20,
TC_GIA_LENH_3 = 120,
TC_DOI_LENH_TOIDA = 100,
TC_LENH_TUAN = 5,

-- Thuong khi DUNG Than Moc Lenh (nil = goc: 3205 = 10tr exp; 3206 = 20 chan
-- nguyen; 3207 = 60tr exp + 120 chan nguyen). Muon doi thi mo comment: [LIVE]
-- TC_THUONG_LENH = {
--     ["6,1,3205"] = {nExp = 10000000},
--     ["6,1,3206"] = {nZhenYuanPoint = 20},
--     ["6,1,3207"] = {{nExp = 60000000}, {nZhenYuanPoint = 120}},
-- },
TC_THUONG_LENH = nil,

-- Bua trieu Thu Ve 3204: gia (diem Than Moc), tran mua 1 lan, tran tong so
-- Thu Ve dung trong map, ban kinh phai dung gan cay (met). [LIVE]
TC_GIA_BUA = 200,
TC_MUA_BUA_TOIDA = 100,
TC_TRAN_THUVE = 100,
TC_BANKINH_BUA = 15,

-- [ENGINE] Khong chinh o day:
--   * HP cay 1912/1913/1914 = 96tr/288tr/960tr: settings\npcs.txt dong
--     1914-1916 cot LifeParam3; HP Thu Ve 2031-2034 = 9.6tr: dong 2033-2036.
--   * Vi tri cay: settings\maps\tongcastle\bronzetree/silvertree/goldtree.txt.
--   * Driver tick: settings\TimerTask.txt dong khoa 55.

-- ===========================================================================
-- [5] ITEM THUONG DUNG CHUNG
-- ===========================================================================
-- Exp Hoang Chan Don 2273 (duong exp CO tran cap 200). [LIVE]
HCD_EXP = 2000000000,
-- Exp Qua Dai Hoang Kim 4864 (duong exp thuong). [LIVE]
QDHK_EXP = 200000000,

}

-- ============================================================================
-- HAM DUNG CHUNG (cac file tinh nang goi - KHONG sua)
-- ============================================================================
function HD_CFG(szKhoa, macdinh)
	if (tbCHD and tbCHD[szKhoa] ~= nil) then
		return tbCHD[szKhoa]
	end
	return macdinh
end

-- doi 1730 -> "17:30" de ghep vao text thoai
function HD_GioPhut(nHHMM)
	return format("%02d:%02d", floor(nHHMM/100), mod(nHHMM, 100))
end

-- dung boi tongcastle: bang {[thu] = giatri} theo TC_THU
function HD_TC_Ngay(giatri)
	local tb = {}
	tb[HD_CFG("TC_THU", 0)] = giatri
	return tb
end

-- nut "Nap lai CONFIG" cua Lenh Bai Admin goi ham nay trong tung state
function HD_NapLaiCauHinh()
	Include("\\script\\header\\cauhinh_hoatdong.lua")
	return 1
end
