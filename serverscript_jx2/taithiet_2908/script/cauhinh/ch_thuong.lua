-- ============================================================================
-- CH_THUONG.LUA - BANG THUONG moi hoat dong / su kien
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

-- Quy uoc: moi bang thuong la mang cac muc
--     { <so luong>, {<genre>, <detail>, <particular>}, "<ten de doi chieu>" }
-- Ten de doi chieu KHONG dung de tim vat pham - no chi de nguoi doc biet mon
-- gi, va de bo kiem `ReverseTools\cauhinh\ktr_cauhinh.py` doi chieu voi
-- settings\item\magicscript.txt roi BAO LOI neu lech.
--
-- !! Ma vat pham cua ban Linux (dai 30000+, hoac {6,0,x}) KHONG dung duoc o
--    JX1: magicscript cua du an CHI co detail = 1. Xem thieu_vatpham.txt.

tbCFG_THUONG = {

-- [CFGBDH 30/08] BAN DONG HANH - danh hieu

BDH_THOIHAN_DANHHIEU_NGAY = 30          ,	-- so NGAY danh hieu Ban Dong Hanh ton tai (ma viet dang 30*24*60*60*18*36 nen day la con so dau tien). !! khai lai y het o partner_reward2.lua


BHK_EXP_GIET_BOSS_PLD     = 50000000    ,	-- exp cho nguoi ha Boss Hoang Kim ban do Phong Lang Do
BHK_EXP_LANCAN_BOSS_PLD   = 20000000    ,	-- exp cho nguoi dung gan cho ha boss Phong Lang Do
BHK_PHAMVI_HUONG_EXP_PLD  = 200         ,	-- ban kinh chia exp quanh xac boss Phong Lang Do


BHK_EXP_GIET_BOSS_DAI     = 25000000    ,	-- exp cho nguoi ha Boss Dai Hoang Kim va to doi cua ho
BHK_EXP_LANCAN_BOSS_DAI   = 10000000    ,	-- exp cho nguoi dung gan cho ha boss dai nhung khac to doi
BHK_PHAMVI_HUONG_EXP_DAI  = 200         ,	-- ban kinh chia exp quanh xac boss dai

-- [CFGBHK 30/08] BOSS HOANG KIM - thuong khi ha boss

BHK_EXP_GIET_BOSS_TIEU    = 20000000    ,	-- exp cho nguoi ha Boss Tieu Hoang Kim va to doi cua ho
BHK_EXP_LANCAN_BOSS_TIEU  = 10000000    ,	-- exp cho nguoi dung gan cho danh boss tieu nhung khac to doi
BHK_PHAMVI_HUONG_EXP_TIEU = 200         ,	-- ban kinh quanh xac boss ma nguoi choi phai dung trong do moi duoc chia exp (boss tieu)

-- [BOSUNG 30/08] bo sung cac khoa script DA DOC nhung tep nay
-- chua khai (loi cua bo khung, da sua goc trong noi_cauhinh.py).

TKT_GIA_DOI_NHACVUONGKIEM = 10000000    ,	-- so tien can de doi Nhac Vuong Kiem
TKT_PHI_TRINHSAT          = 500         ,	-- phi moi lan dung chuc nang trinh sat
TKT_SL_HONTHACH_DOI_KIEM  = 100         ,	-- so Nhac Vuong Hon Thach can de doi mot Nhac Vuong Kiem

-- [CFGTKT 30/08] TONG KIM - DIEM, THUONG va DIEU KIEN THAM GIA
-- (phan THOI GIAN va LICH nam o ch_lich.lua, tien to TK_)
-- 
-- Moi so DUNG BANG gia tri dang chay. Sua xong KHOI DONG LAI GameServer.

TKT_CAP_TOI_THIEU         = 80          ,	-- cap toi thieu moi duoc bao danh
TKT_LECH_QUANSO_TOIDA     = 20          ,	-- chenh lech quan so toi da giua hai phe; qua muc nay phe dong hon bi chan bao danh
TKT_PHI_BAO_DANH          = 20000       ,	-- phi bao danh. !! hien CHI HIEN CHU, khong tru tien - moi lenh Pay da bi comment
TKT_THUONG_TICHLUY_THANG  = 1200        ,	-- diem tich luy cong them cho phe THANG, nhan voi cap quan ham 1-6
TKT_THUONG_TICHLUY_THUA   = 300         ,	-- diem tich luy cong them cho phe THUA, nhan voi cap quan ham
TKT_THUONG_TICHLUY_HOA    = 600         ,	-- diem tich luy cong them khi tran HOA, nhan voi cap quan ham
TKT_DIEM_TOITHIEU_NHAN_THUONG= 1000        ,	-- duoi nguong diem nay thi het tran KHONG duoc goi qua
TKT_SO_CO_TOIDA           = 30          ,	-- moi phe chi duoc cam toi da bay nhieu la co trong mot tran
TKT_DIEM_CAM_CO           = 300         ,	-- diem moi lan mang co ve cam thanh cong (cong ca ca nhan lan phe)
TKT_MUC_GIAM_TOCDO_GIU_CO = 60          ,	-- muc giam toc do chay cua nguoi dang vac co
TKT_GIAY_TRONG_DOANHTRAI  = 90          ,	-- so GIAY toi da duoc dung trong doanh trai truoc khi bi day ra
TKT_GIAY_DELAY_RA_TRAI    = 10          ,	-- so GIAY con lai duoc giu khi buoc ra khoi trai
TKT_MAU_NGUYENSOAI        = 5000000     ,	-- mau cua Nguyen Soai hai phe. !! con duoc nhan them voi GLB_MANH_NGUYENSOAI_TK
TKT_DIEM_GIET_NGUYENSOAI  = 5000        ,	-- diem cho nguoi ha guc Nguyen Soai (nguoi danh don cuoi an tron)
-- [CFGVT 29/08] THUONG VAN TIEU (script\event\event_vantieu\tieudau.lua)
--
-- !! DOC KY TRUOC KHI DOI: hien MOI CHUYEN cong that la
--      tieu dong    650.000.000 exp  (thong bao ghi  60.000.000)
--      tieu bac     850.000.000 exp  (thong bao ghi  80.000.000)
--      tieu vang  1.050.000.000 exp  (thong bao ghi 100.000.000)
--    Tran 20 luot/ngay => toi da 21 TY exp mot nguoi mot ngay.
--    Exp nay di qua AddSumExp nen KHONG chiu he so nao - chinh
--    ExpRate hay nhom [Exp] deu khong cham toi.
--
-- Cac so duoi day DUNG BANG gia tri dang chay. Doi la an ngay sau
-- khi khoi dong lai GameServer.

VT_EXP_NEN_LAN        = 1000        ,	-- so lan cong exp nen
VT_EXP_NEN_MOI        = 50000       ,	-- moi lan cong bao nhieu (1000 x 50.000 = 50 trieu)
VT_EXP_NEN_SUM        = 500000000   ,	-- exp nen cong THEM (thong bao chi ghi 50.000.000)
VT_EXP_DONG           = 100000000   ,	-- tieu dong: exp cong them (thong bao ghi 10.000.000)
VT_EXP_BAC            = 300000000   ,	-- tieu bac: exp cong them (thong bao ghi 30.000.000)
VT_EXP_VANG           = 500000000   ,	-- tieu vang: exp cong them (thong bao ghi 50.000.000)
VT_HOMACH_DONG        = 300         ,	-- tieu dong: so Ho Mach Don
VT_CHANNGUYEN_DONG    = 50          ,	-- tieu dong: so Chan Nguyen Don
VT_RUONG_DONG         = 2           ,	-- tieu dong: so ruong trang bi xanh
VT_HOMACH_BAC         = 400         ,	-- tieu bac: so Ho Mach Don
VT_CHANNGUYEN_BAC     = 70          ,	-- tieu bac: so Chan Nguyen Don
VT_RUONG_BAC          = 5           ,	-- tieu bac: so ruong trang bi xanh
VT_HOMACH_VANG        = 500         ,	-- tieu vang: so Ho Mach Don
VT_CHANNGUYEN_VANG    = 100         ,	-- tieu vang: so Chan Nguyen Don
VT_RUONG_VANG         = 10          ,	-- tieu vang: so ruong trang bi xanh
VT_LENHBAI_BOSS       = 10          ,	-- so Lenh Bai Boss moi chuyen (moi loai tieu)
VT_DIEM_TONG_DONG     = 100         ,	-- tieu dong: diem cong cho bang hoi
VT_DIEM_TONG_BAC      = 200         ,	-- tieu bac: diem cong cho bang hoi
VT_DIEM_TONG_VANG     = 300         ,	-- tieu vang: diem cong cho bang hoi

}
