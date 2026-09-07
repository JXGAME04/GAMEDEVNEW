-- ================================================================================================
-- [HE THONG] script/cauhinh/ch_lich.lua
-- Muc dich  : LICH HOAT DONG (gio mo/dong theo ngay).
-- Duoc nap  : Include tu 4 tep (vd ch_all.lua, head.lua, mission.lua, lib_tktc.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Ham       : (khong co - tep du lieu/cau hinh)
-- Sua nong  : CO - trong chuoi Include cua timerserver.lua, tu nap lai trong <= 1 phut sau khi ghi tep
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
-- ============================================================================
-- CH_LICH.LUA - THOI GIAN moi hoat dong / su kien
-- [CAUHINH 29/08]
--
-- !! TEP NAY LA "LA": CAM Include bat cu gi vao day.
--    (Bai hoc that: vong Include 23/08 lam GameServer boot stack overflow -
--     xem ghi chu dau script\header\cauhinh_hoatdong.lua)
--
-- CACH DUNG: sua so trong bang duoi day roi KHOI DONG LAI GameServer.
--    Muon doc trong script:  local x = G_CFG("TEN_KHOA", giatri_mac_dinh)
--    (nho Include \script\cauhinh\ch_lib.lua truoc khi goi G_CFG)
-- ============================================================================

-- Quy uoc dat ten khoa:  <TIENTO>_GIO_<viec>   gio dang HHMM (2000 = 20:00)
--                        <TIENTO>_THU_<viec>   thu trong tuan (0=CN..6=T7)
--                        <TIENTO>_PHUT_<viec>  do dai tinh bang phut
--                        <TIENTO>_BAT          cong tac 0/1
-- Tien to dang dung: TW_ (bang chien) BR_ (bach nhan) BW_ (ty vo)
--                    TC_ (thanh bao) YDBZ_ (viem de) - 5 nhom NAY DA CO SAN
--                    trong script\header\cauhinh_hoatdong.lua, KHONG chep
--                    lai vao day de tranh hai noi khai mot khoa.
-- Cac hoat dong con lai duoc do vao day o dot ke tiep (dang khao sat).

tbCFG_LICH = {

-- [CFGTAT 30/08] LOI DAI HON CHIEN - DANG TAT
-- 
-- !! PHAI VA MAY CHO HO EXP TRUOC KHI BAT - xem BAOCAO_LOHONG_2908.md

-- [BOLDHC 30/08] bo khoa LDHC_PHI_BAODANH: hoat dong Loi Dai Hon Chien da bo han

-- [BOSUNG 30/08] bo sung cac khoa script DA DOC nhung tep nay
-- chua khai (loi cua bo khung, da sua goc trong noi_cauhinh.py).

CTC_CHUKY_QUET_MO_TRAN_PHUT= 5           ,	-- doc tai missions\citywar_global\mission.lua

-- [CFGCTC 30/08] LOI DAI DAU TRUONG (missions\citywar_arena\head.lua)
-- (khoa CTLD_EXP_BANG_THANG la phan THUONG, de chung o day cho
--  cung mot cho voi cac khoa khac cua Loi Dai)

CTLD_MAX_NGUOI_MOI_PHE    = 16          ,	-- so nguoi toi da moi bang vao dau truong Loi Dai
CTLD_CHUKY_LOA_GIAY       = 20          ,	-- chu ky loa trong Loi Dai (giay)
CTLD_DODAI_TRAN_PHUT      = 25          ,	-- do dai mot tran Loi Dai (phut). !! chu thich trong ma ghi nham la mot tieng
CTLD_GIAY_CHO_VAO_DAUTRUONG= 480         ,	-- thoi gian cho hai ben vao dau truong (giay). !! chu thich ghi nham la 10 phut
CTLD_EXP_BANG_THANG       = 1200        ,	-- kinh nghiem bang hoi cong cho ben thang mot tran Loi Dai

-- [CFGTK 29/08] TONG KIM (script\tinhnang\tong_kim_tcap\lib_tktc.lua)
--
-- !! CAN CHU XEM LAI: tep lib_tktc.lua duoc luu luc 28/08 23:45:56
--    va khung gio dau trong bang lich la 23h46 - tuc dat de no sau
--    dung 4 giay. Rat co the do la cau hinh THU NGHIEM bi bo quen.
--    Ba con so thoi gian cung lech han voi chu thich ngay canh:
--      bao danh dang la 1 phut  (chu thich ghi 10)
--      ca tran   dang la 30 phut (chu thich ghi 70)
--      nguyen soai dang la 1 phut (chu thich ghi 30)
--    Toi khong tu doi vi khong biet so dung - rieng khung gio dau
--    da tung co ba gia tri khac nhau (13h23 / 13h58 / 23h46).

-- Bang lich: moi khung la {gio, phut, gio_ket_thuc, so_hieu_tran}
-- TK_LICH = {{17,50,18,3}, {20,50,21,4}, {22,50,23,5}},

TK_PHUT_BAODANH       = 1     ,	-- so PHUT bao danh. !! chu thich trong ma ghi 10 - dang chay 1
TK_PHUT_TRAN          = 30    ,	-- so PHUT ca tran. !! chu thich trong ma ghi 70 - dang chay 30
TK_PHUT_NGUYENSOAI    = 1     ,	-- so PHUT den luc ra Nguyen Soai. !! chu thich ghi 30 - dang chay 1
TK_PHUT_XOANPC        = 1     ,	-- so PHUT xoa NPC sau tran
TK_NGUOI_MOI_PHE      = 2000  ,	-- so nguoi toi da moi phe
TK_LICH               = {{21,08,19,1}, {17,50,18,3}, {20,50,21,4}, {22,50,23,5}},
-- [CONGTAC 29/08] CONG TAC BAT/TAT HOAT DONG
-- 1 = bat, 0 = tat. Gia tri duoi day DUNG BANG trang thai dang chay
-- hom nay, nen file nay khong lam doi gi ca cho toi khi ban sua.
-- Sua xong phai KHOI DONG LAI GameServer.
--
-- LUU Y: hoat dong dang tat lau ngay co the chua duoc nap thu vien.
-- Bat len ma khong thay chay thi xem logs\hethong.log - se co dong
-- "<khoa> bat nhung ham <ten> chua nap".

-- [GOHECU 30/08] bo khoa BAT_LIENDAU: he Lien Dau cu da go han.
-- Lien Dau dang chay = ban WLLS (missions\leaguematch + leaguematch\gsdriver.lua),
-- chay theo lich rieng, KHONG doc khoa nay.
BAT_TONGKIM           = 1,	-- Tong Kim
-- [GOHECU 30/08] bo khoa BAT_CONGTHANH_VIET: he cu da go, ban thay = Cong Thanh ban JX2 (BAT_CTC_JX2 = 1, CTC_JX2_Tick)
BAT_HATHUYHOANG       = 0,	-- Ha Thuy Hoang
BAT_BOSS_HOANGKIM     = 0,	-- Boss Hoang Kim
-- [GOHECU 30/08] bo khoa BAT_VUOTAI_VIET: he cu da go, ban thay = Vuot Ai trong cum 3 hoat dong ban Linux (BAT_HD3 = 1, HD3_Tick)
-- [GOHECU 30/08] bo khoa BAT_PHONGLANGDO_VIET: he cu da go, ban thay = Phong Lang Do trong cum 3 hoat dong ban Linux (BAT_HD3 = 1, HD3_Tick)
BAT_HOADANG           = 0,	-- Hoa Dang / Trang Nguyen
-- [GOHECU 30/08] bo khoa BAT_LOIDAI_BANGHOI: he cu da go, ban thay = Loi Dai bang hoi ban JX2 (CTC_JX2_Tick + TimerTask 16/17 citywar_arena)
BAT_TRONG_BANGHOI     = 0,	-- Trong Bang Hoi
BAT_KIEMMONQUAN       = 0,	-- Kiem Mon Quan
BAT_VANTIEU_LOA       = 0,	-- loa Van Tieu (NPC van chay khong can khoa nay)
BAT_DUATOP            = 0,	-- Dua Top
BAT_SONGBAC           = 0,	-- Song Bac
-- [BOLDHC 30/08] bo khoa BAT_LOIDAI_HONCHIEN: hoat dong Loi Dai Hon Chien da bo han
BAT_CHECK_KICK        = 0,	-- tu kich tai khoan
BAT_PUBG              = 0,	-- Sinh Ton (PUBG)
BAT_HD3               = 1,	-- cum 3 hoat dong Linux: Sat Thu / Phong Lang Do / Vuot Ai
BAT_VIEMDE            = 1,	-- Viem De Bao Tang
BAT_CTC_JX2           = 1,	-- Cong Thanh + Loi Dai bang hoi (ban JX2)

}
