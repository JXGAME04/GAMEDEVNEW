-- ============================================================================
-- CH_DROP.LUA - rot do phan chinh duoc bang script
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

-- !! DANH MUC vat pham + ti le rot nam o 49 tep settings\droprate\**\*.ini
--    (nap lai NGAY moi lan rot do - KHONG can restart).
--    Viec CHON tep .ini nao theo cap quai nam o
--    script\global\LuaNpcMonsters\Droprate_normal.lua:24-95
--    Xac suat rot cua quai thuong: Droprate_normal.lua:113-118 (hien la 3/31).
--
-- Bang duoi day danh cho cac hang so ti le trong script (lib_sukien.lua...).
-- Duoc do vao o dot ke tiep.

tbCFG_DROP = {

-- [CFGQUAI 29/08] ROT DO QUAI THUONG (script\global\LuaNpcMonsters\Droprate_normal.lua)
-- Nhom nay chi phoi MOI quai thuong tren MOI ban do.
-- Moi so lay tu chinh ma nguon dang chay.

DRQ_HESO_TIEN             = 1     ,	-- nhan vao so tien quai roi ra
DRQ_CAP_BAC_TOITHIEU      = 10    ,	-- duoi cap nay thi xep vao bac 1
DRQ_CAP_BAC_TOIDA         = 100   ,	-- tren cap nay thi boc ngau nhien bac 9-10
DRQ_QUAY_BOSSXANH         = 8     ,	-- boss xanh: so mon roi ra moi lan chet
DRQ_MAU_BOSSXANH          = 10    ,	-- boss xanh: mau so cua phep boc tien
DRQ_NGUONG_TIEN_BOSSXANH  = 8     ,	-- boss xanh: vuot nguong nay moi roi tien (mau 10 => ~18%)
DRQ_MAU_QUAI              = 30    ,	-- quai thuong: mau so cua phep boc
DRQ_NGUONG_TIEN           = 2     ,	-- quai thuong: trung dung so nay thi roi tien (mau 30 => ~3,2%). Dat -1 de tat han
DRQ_NGUONG_ITEM           = 4     ,	-- quai thuong: duoi so nay thi quay bang rot do (mau 30 => 2/31 sau khi tru nhanh tien)
DRQ_QUAY_QUAI             = 1     ,	-- quai thuong: so mon roi ra moi lan trung
DRQ_MAU_SUKIEN            = 4     ,	-- mau so cua phep boc goi dropeventmap
DRQ_CAP_SUKIEN            = 7     ,	-- quai tu bac nay tro len moi goi nhanh su kien
DRQ_NGUONG_SUKIEN         = 2     ,	-- duoi so nay thi goi dropeventmap (mau 4 => 40%). Dat -1 de tat han
-- [CFGDROP 29/08] ROT DO SU KIEN (script\lib\lib_sukien.lua)
-- Moi so duoi day LAY TU CHINH ma nguon dang chay, nen tep nay khong
-- lam doi gi cho toi khi ban sua. Sua xong KHOI DONG LAI GameServer.
--
-- Cach doc ti le: <nguong> so voi <mau so>. Vi du SKD_HMD_NGUONG = 1
-- voi mau SKD_RANDMAP = 100 nghia la 'random(1,100) > 1' => 99%.

SKD_RANDMAP           = 100,   -- mau so cho dropeventmap/HCNT/HMD/manhhkmp
SKD_RANDPLD           = 100,   -- mau so nhanh Phong Lang Do (ham than rong)
SKD_RANDOTHER         = 10,    -- mau so cho dropeventboss / dropntiendong
SKD_RANDTK            = 100,   -- mau so nhanh Tong Kim (ham than rong)
SKD_RANDVA            = 100,   -- mau so nhanh Vuot Ai (ham than rong)
SKD_DROPMDTB          = 95,    -- mau so cho dropnvdt / dropnvdt01
SKD_MAP_NGUONG        = 80,    -- vuot nguong nay moi rot (mau 100 => con 20%)
SKD_MAP_ITEM          = 4854,  -- ma vat pham rot theo su kien tren ban do
SKD_HCNT_NGUONG       = 50,    -- Hiep Cot Nhu Tinh: vuot nguong moi rot (=> 50%)
SKD_HCNT_MA_MIN       = 185,   -- dai ma trang bi Hiep Cot Nhu Tinh - dau
SKD_HCNT_MA_MAX       = 192,   -- dai ma trang bi Hiep Cot Nhu Tinh - cuoi
SKD_PUBG_MA_MIN       = 4851,  -- dai ma vat pham Sinh Ton - dau
SKD_PUBG_MA_MAX       = 4853,  -- dai ma vat pham Sinh Ton - cuoi
SKD_HKMP_MA_MIN       = 0,     -- dai ma trang bi Hoang Kim Mon Phai - dau
SKD_HKMP_MA_MAX       = 139,   -- dai ma trang bi Hoang Kim Mon Phai - cuoi
SKD_HKMP_HAN_NGAY     = 7,     -- so ngay ton tai cua trang bi HKMP rot ra
SKD_HMD_NGUONG        = 1,     -- Huy Hoang Don: vuot nguong moi rot (mau 100 => dang la 99%)
SKD_HMD_SOLUONG       = 10,    -- moi lan rot bao nhieu Huy Hoang Don
SKD_HMD_ITEM          = 4844,  -- ma Huy Hoang Don
SKD_MANHHKMP_NGUONG   = 80,    -- manh do pho HKMP: duoi nguong nay thi rot (mau 100 => dang la 79%)
SKD_BOSS_NGUONG       = 1,     -- boss su kien: vuot nguong moi rot (mau 10 => dang la 90%)
SKD_BOSS_MA_MIN       = 753,   -- dai ma trang bi boss su kien - dau
SKD_BOSS_MA_MAX       = 770,   -- dai ma trang bi boss su kien - cuoi
SKD_TIENDONG_NGUONG   = 1,     -- Tien Dong: vuot nguong moi rot (mau 10 => dang la 90%)
SKD_TIENDONG_SL_MIN   = 1,     -- so Tien Dong moi lan - it nhat
SKD_TIENDONG_SL_MAX   = 3,     -- so Tien Dong moi lan - nhieu nhat
SKD_TIENDONG_ITEM     = 4835,  -- ma Tien Dong

}
