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

-- --- DANH HIEU + VONG SANG cua Bang Chien ---------------------------------
-- [BANG] KHONG co khoa Lua o day, va do la CO Y: engine lay ca hai tu BANG
--   settings\PlayerTitle.txt (tra theo DONG = id + 1) va tu cast lai moi lan
--   dang nhap (KNpc.cpp:11016-11023, KPlayer.cpp:6897). Them nguon thu hai trong
--   Lua se thanh HAI NGUON cho cung mot thu -> sua mot noi sai noi con lai.
--   Muon doi thi sua THANG bang do, CA BAN SERVER LAN BAN CLIENT:
--     dong 288-292 (id 287-291) = quan ham 1..5, cot ExtSkill1 = 830 831 832 833 834
--                                 cot ExtSkillLevel1 = 2 4 6 8 10
--     dong 293-296 (id 292-295) = 4 danh hieu bang, ExtSkill1 = 1169 cap 1
--   Cot TitleName la TEN hien tren dau nhan vat; de trong = khong hien gi.
--   Bo vong sang: dat ExtSkill1 = 0. Doi vong sang: xem bang tra o duoi.
--   Cac vong sang CO SAN trong ban nay (skill -> anh, deu ve DUOI CHAN):
--     830..834 = 5 cap quan ham chien truong (title_new_zw/yl/bj/ab/dg.spr)
--     1169     = vong tron Vo Lam Dai Hoi (title_yl.spr) - id 286 THDNB dang dung
--     1557..1563 = bo vong sang VIP (vongsangvip1..7.spr)
--   Bang tra anh: settings\npcres\state_magic_table_name.txt, DONG = StateSpecialId + 1
--   (cot 3 = "Foot" ve duoi chan, "Head" ve tren dau).
--   LUU Y: doi ten danh hieu thi CLIENT phai thoat ra vao lai moi thay - bang nay
--   client chi nap MOT LAN (KNpc.cpp:6049-6056 s_bInfoTabsLoaded).

-- [ENGINE] Khong chinh o day:
--   * Lich tick driver: settings\TimerTask.txt dong khoa 54 (30 giay/lan).
--   * Mission slot 33: settings\task\missions.txt dong 34.
--   * Map 605-613: settings\MapList.ini + WorldSet.ini.

-- ===========================================================================
-- [1b] NHIÖM Vô TÝN Sø (Thiªn B¶o Khè ®­a tin) - tiÒn tè TS_
-- ===========================================================================
-- NhËn ë DÞch Quan Ba L¨ng HuyÖn (map 11) / ®¹i Lý (162); Xa phu ®­a vµo ¶i map 395.
-- Chuçi lµm: giÕt Thñ Hé Gi¶ -> më 5/9 B¶o R­¬ng ®óng thø tù -> Tiªu TrÊn ra -> tr¶ nhiÖm vô.

-- CÊp tèi thiÓu nhËn nhiÖm vô (gèc Linux 120, chñ game ®· h¹ 90). [RESTART]
TS_CAP_TOITHIEU = 90,

-- Sè l­ît th­êng/ngµy + sè l­ît mua thªm b»ng item Thiªn Khè B¶o LÖnh (6,1,3431).
-- ®Õm theo ngµy ë task 4128. [RESTART]
TS_LUOT_THUONG_NGAY = 2,
TS_LUOT_ITEM_NGAY = 1,

-- --- PHÇN TH­ëNG (nil = dïng ®óng b¶ng gèc Linux) ---------------------------
-- B¶ng gèc: settings\task\tollgate\messenger\messenger_tollprize.txt, hµng 10 (qian120):
--    më 1 B¶o R­¬ng    = 10000 exp + 9 ®iÓm TÝn Sø (céng vµo task 1205)
--    giÕt 1 Thñ Hé Gi¶ = 40000 exp + 30 ®iÓm TÝn Sø
-- ®iÒn sè vµo ®©y lµ GHI ®Ì b¶ng; ®Ó nil lµ gi÷ nguyªn b¶ng (100% Linux). [RESTART]
TS_EXP_MO_RUONG = nil,
TS_DIEM_MO_RUONG = nil,
TS_EXP_GIET_THUHO = nil,
TS_DIEM_GIET_THUHO = nil,

-- Th­ëng khi TR¶ nhiÖm vô ë tr¹m dÞch (posthouse.lua, hµm messenger_treasureprize):
--    lÇn ®ÇU trong ngµy: TS_TRA_HANHHIEP c¸i Hµnh HiÖp LÖnh (6,1,2575)
--    MäI lÇn tr¶: TS_TRA_BAORUONG c¸i TÝn Sø B¶o R­¬ng (6,1,3430)
--    (riªng nguyªn liÖu Kinh M¹ch 4847 tèi ®a 2 lÇn/ngµy theo task 3073 - gi÷ nguyªn)
-- [RESTART]
TS_TRA_HANHHIEP = 3,
TS_TRA_BAORUONG = 2,

-- [ENGINE/B¶NG] Kh«ng chØnh ë ®©y:
--   * Sè r­¬ng ph¶i më (5) / tæng r­¬ng (9): messenger_baoxiangtask.lua dßng 10-11,
--     g¾n cøng víi 9 vÞ trÝ NPC trong killbosshead.lua - ®õng ®æi.
--   * Giíi h¹n 2 giê trong ¶i (task 1222): BÞ COMMENT Tõ GèC ë c¶ Linux
--     (messenger_timeer.lua) - kh«ng giíi h¹n thêi gian lµ ®óng b¶n gèc.
--   * Danh s¸ch tr¹m tr¶ nhiÖm vô: b¶ng citygo trong posthouse.lua.

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
-- [6] 3 HOAT DONG BAN LINUX (port 25/08) - tien to HD3_
--   (A) SAN BOSS SAT THU  = Killer Boss  (thuong truc, khong theo lich)
--   (B) PHONG LANG DO      = fengling ferry (dua thuyen theo gio)
--   (C) VUOT AI            = challengeoftime (thi dau to doi theo gio)
-- Nguon 100% ban Linux; so lieu duoi = doc thang tu script Linux, sua o day
-- roi RESTART (hoac Lenh Bai Admin -> "Hoat dong Linux" -> "Nap lai CONFIG").
-- ===========================================================================

-- ---- (A) SAN BOSS SAT THU ----
-- Cap toi thieu - CHI HIEN THI tren menu admin (ban Linux khong chan cap o
-- buoc nhan nhiem vu; nhom boss chia theo cap 20..90 san trong killbosshead). [HIEN THI]
HD3_ST_CAP_TOITHIEU = 90,
-- Tran so lan giet boss / ngay (goc Linux KILLER_MAXCOUNT = 8; da noi vao
-- nieshichen.lua). [RESTART]
HD3_ST_MAX_NGAY = 8,
-- Chi nhom boss cap 90 (chi so 141..160) con phat thuong (nhom 20..80 da bi
-- comment trong nieshichen.lua goc Linux - GIU DUNG NGUYEN BAN). [ENGINE]
--   * 160 NPC boss + 7 NPC 769: engine tu sinh luc boot (hd3_driver HD3_DriverInit).
--   * Bang toa do boss: settings\task\tollgate\killbosshead.lua (da chep).
--   * Bang roi do: settings\droprate\boss\bosstask_lev90.ini (da chep).
--   * 5 Sat Thu lenh cung cap -> 1 Sat Thu Gian (ve vao VUOT AI).

-- ---- (B) PHONG LANG DO ----
-- Cap toi thieu - CHI HIEN THI (ban Linux chi doi co mon phai, fld_head.lua:40). [HIEN THI]
HD3_PLD_CAP_TOITHIEU = 1,
-- Gio mo trong ngay (dang HHMM, moi so = 1 luot dua). Ban Linux relay chay
-- MOI GIO dung phut :00 (24 luot/ngay) - DUNG 100% Linux (TaskList.ini Task_86
-- dang ky ban interval 60; cac tep _NN00 gio chan KHONG duoc dang ky). [RESTART]
HD3_PLD_GIO = {0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200,
               1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000, 2100, 2200, 2300},
-- Suc chua moi thuyen (fld_haveroom - da noi HD_CFG doc luc chay). [RESTART]
HD3_PLD_SUC_CHUA = 100,
-- Cac gio "ton phi" (dung Lenh Bai Thuy Tac 6,1,3363) dang HHMM - goc Linux
-- 10/14/16/18/20h; da noi vao fld_head check_new_shuizeitask.
-- Ngoai gio nay dung Lenh bai PLD (item 4,489) hoac 200 Mat do than bi. [LIVE]
HD3_PLD_GIO_TONPHI = {1000, 1400, 1600, 1800, 2000},
-- [ENGINE] Khong chinh o day:
--   * Nhip thoi gian (18 khung/giay): fld_head.lua:10-30 (timer 20s / 39 phut).
--   * 3 ban do thuyen 337/338/339 + ban do bo Bac 336: settings\MapList.ini.
--   * Bang toa do sinh Thuy tac: settings\maps\ZhongYuanBeiQu\DuChuan\DuChuanShuaGuaiDian.txt (ten thu muc chu Han, da chep tu goc B Patch).
--   * Quai 724 (Thuy tac x30 cap95), boss 725 (dau linh x3 cap85),
--     1692 (dai dau linh x2, chi gio ton phi): npcs.txt.

-- ---- (C) VUOT AI ----
-- Cap toi thieu - CHI HIEN THI (gioi han that = tbLevels trong include.lua:
-- so cap 50-89, cao cap 90+; doi o day KHONG co tac dung). [HIEN THI]
HD3_VA_CAP_TOITHIEU = 50,
-- Gio bao danh (HHMM) - Linux relay chay moi gio phut :00. [RESTART]
HD3_VA_GIO = {0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200,
              1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000, 2100, 2200, 2300},
-- Phut bao danh + phut lam nhiem vu (da noi vao challengeoftime include.lua). [RESTART]
HD3_VA_PHUT_BAODANH = 10,
HD3_VA_PHUT_NHIEMVU = 30,
-- So nguoi toi da 1 doi (LIMIT_PLAYER_COUNT = 8). [RESTART]
HD3_VA_NGUOI_TOIDA = 8,
-- Gio bang xep hang ngay, so sanh dang HHMM (0 = 00:00, 130 = 01:30). [RESTART]
HD3_VA_GIO_XEPHANG = 0,
-- So luot vao Vuot ai / nguoi / ngay (COUNT_LIMIT goc Linux = 1). [RESTART]
HD3_VA_LUOT_NGAY = 1,

-- ============ PHAN THUONG (nil = dung bang goc ban Linux) ============
-- Muon doi: chep nguyen bang goc tu tep script neu duoi day vao thay cho nil
-- roi sua so; bang co the chua ca function (giu nguyen cau truc goc).

-- (A) SAT THU - bang thuong hoan thanh nhom cap 90 (25 dong vat pham +
--     10.000.000 exp). Goc: kill_level.lua:89-118 OnFinishKillerTask. [RESTART]
HD3_ST_THUONG = nil,
-- (A) SAT THU - gia thue xe toi cho boss khi bam dan duong tren F11 (khuon
--     cu cua ban Viet MONEY_GO_BOSS = 1000 luong). [LIVE]
HD3_ST_TIEN_XE = 1000,

-- (B) PLD - so Thi Gia Chi An (6,1,1095) roi tu boss dau linh 725 (goc 2;
--     bang co Dao Chu tien dai thi tu x2 theo getSignetDropRate). [RESTART]
HD3_PLD_SO_AN_BOSS = 2,
-- (B) PLD - ti le roi Hai Long Chau 6,1,2124 tu boss (goc 0.005 = 0,5%). [RESTART]
HD3_PLD_TILE_HAILONG = 0.005,
-- (B) PLD - ti le roi Truy Cong Lenh 6,1,2024 tu Thuy tac thuong trong gio
--     su kien (goc: nCurRate < 50 tren random(1,100) = 49%). [RESTART]
HD3_PLD_TILE_TRUYCONG = 50,
-- (B) PLD - so Bao Ruong Thuy Tac 6,1,3361 khi cap ben thanh cong. [RESTART]
HD3_PLD_THUONG_CAPBEN = 2,

-- (C) VUOT AI - bang exp hoan thanh 28 ai (2 cap do, co function tinh theo
--     thoi gian). Goc: award.lua:58-83 tbAward_Success. [RESTART]
HD3_VA_THUONG_HOANTHANH = nil,
-- (C) VUOT AI - thuong hang 1 bang xep hang ngay (goc: 1 Thien Nien Linh Duoc
--     6,1,2125, han 24h). Goc: rank_perday.lua:13. [RESTART]
HD3_VA_THUONG_HANG_NGAY = nil,
-- (C) VUOT AI - bang do trong Bao Ruong Vuot ai (theo loai chia khoa).
--     Goc: chuangguanbaoxiang.lua:21-90 tbCOT_Box_Award. [RESTART]
HD3_VA_THUONG_RUONG = nil,

-- [ENGINE] Khong chinh o day:
--   * 2 cap do + ban do: challengeoftime\include.lua (tbLevels / tbLevelMaps
--     464-479 so cap, 480-495 cao cap).
--   * Bang doi hinh 28 ai: settings\maps\challengeoftime\lineup*.txt (da chep).
--   * Che do "chuangguan30" (map 957): challengeoftime\chuangguang30.lua.
--   * Ve vao: 1 Sat Thu Gian (6,1,399 sau anh xa) - san o hoat dong SAT THU.

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
