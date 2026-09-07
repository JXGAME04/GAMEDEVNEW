-- ================================================================================================
-- [HE THONG] script/cauhinh/ch_chung.lua
-- Muc dich  : CAU HINH CHUNG (ten server, gioi han, tham so he thong). Web CFGW (bang gcfg) co the ghi de gia tri khi khoi dong.
-- Duoc nap  : Include tu 10 tep (vd ch_all.lua, lib_server.lua, head.lua, head.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Ham       : (khong co - tep du lieu/cau hinh)
-- Sua nong  : CO - trong chuoi Include cua timerserver.lua, tu nap lai trong <= 1 phut sau khi ghi tep
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
-- ============================================================================
-- CH_CHUNG.LUA - thong so chung toan game
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

tbCFG_CHUNG = {

-- [CFGBDH 30/08] DA TAU - so vong va chu ky
-- 
-- !! Moc thuong theo so nhiem vu va gia tri goc de tinh tien/exp
-- !! KHONG o day - chung nam trong settings\task\award_basic.txt,
-- !! award_link.txt, award_loop.txt, tasklink_*.txt.

DT_SO_NV_MOI_VONG         = 20          ,	-- so nhiem vu phai lam xong de qua mot vong moi
DT_SO_VONG_MOI_CHU_KY     = 20          ,	-- so vong nhiem vu trong mot chu ky
DT_SO_CHU_KY_TOI_DA       = 20          ,	-- so chu ky toi da truoc khi dem quay ve 0


BDH_SO_O_TRANGBI          = 10          ,	-- so o trang bi cua Ban Dong Hanh


BDH_CHANNGUYEN_DOI_1_TUCHAN= 200         ,	-- so diem chan nguyen bo ra de doi lay 1 diem Tu Chan
BDH_TUCHAN_TOI_DA         = 20000       ,	-- tran diem Tu Chan cua Ban Dong Hanh

-- [CFGBDH 30/08] BAN DONG HANH - gioi han va gia ca (petsys\common.lua)
-- 
-- !! Ti le thanh cong nang cap va so thuoc can KHONG o day - chung
-- !! nam trong settings\petsys\levelup.txt (cot nSuccessRate,
-- !! nItemCostCount). Sua o day khong cham toi chung.

BDH_SO_LAN_CHO_AN_MOI_NGAY= 4           ,	-- so lan moi ngay duoc cho an MOI LOAI qua (dem rieng tung loai)
BDH_GIA_DOI_NGOAI_QUAN    = 5           ,	-- so Xu phai tra de doi ngoai quan
BDH_GIA_DOI_TEN           = 5           ,	-- so Xu phai tra de doi ten
BDH_CAP_TOI_DA            = 130         ,	-- cap toi da cua Ban Dong Hanh. !! ten bien nay rat chung - chi doi trong tep petsys\common.lua
BDH_BUOC_CAP_NGOAI_QUAN   = 10          ,	-- cu moi bao nhieu cap thi mo them mot nhom ngoai quan
BDH_CAP_NHANVAT_TOI_THIEU = 150         ,	-- cap nhan vat toi thieu de tao duoc Ban Dong Hanh
BDH_CAP_MO_KYNANG_BIKIP   = 21          ,	-- cap Ban Dong Hanh toi thieu de bat dau hoc ky nang tu Bi kip
BDH_SO_O_KYNANG_TOI_DA    = 4           ,	-- so o ky nang bi dong toi da
BDH_BUOC_CAP_MO_O_KYNANG  = 5           ,	-- cu moi bao nhieu cap thi mo them mot o ky nang
BDH_KYNANG_BIKIP_CAP_TOI_DA= 30         ,	-- tran cap MOI ky nang bi kip (nang bang diem Tu Chan)
BDH_TUCHAN_MOI_CAP_KYNANG = 10          ,	-- ky nang cap N len N+1 ton N x (so nay) diem Tu Chan

-- [CFGBHK 30/08] BOSS HOANG KIM - suc manh va thoi gian ton tai
-- (tinhnang\boss_hoangkim\lib_bosshk.lua)
-- 
-- !! Lich tu dong dang TAT (BAT_BOSS_HOANGKIM = 0), nhung boss van ra
-- !! duoc bang tay qua menu 'Test Boss' cua lenh bai admin - luc do
-- !! cac so nay an that.
-- 
-- !! BHK_MAU_BOSS_DAI dang la 1 (MOT mau) trong khi ghi chu trong ma
-- !! noi mac dinh la 18.000.000. So nay dung cho CA boss dai, boss may
-- !! chu VA boss Phong Lang Do - ca ba deu chet trong mot nhat.

BHK_PHUT_BOSS_TON_TAI     = 120         ,	-- boss song bao lau (phut) roi tu bien mat neu khong ai giet
BHK_MAU_BOSS_TIEU         = 18000000    ,	-- mau cua Boss Tieu Hoang Kim (ghi chu trong ma: mac dinh cu 8.000.000)
BHK_MAU_BOSS_DAI          = 1           ,	-- mau cua boss dai + boss may chu + boss PLD. !! dang la 1, ghi chu noi mac dinh 18.000.000
BHK_NETRANH_BOSS          = 5000        ,	-- chi so ne tranh cua boss - cang cao nguoi choi cang hay danh truot
BHK_HOIMAU_BOSS           = 300         ,	-- luong mau boss tu hoi moi nhip
BHK_EXP_NPC_BOSS_DAI      = 0           ,	-- exp gan thang vao con NPC boss dai (dang 0 - exp thuong phat o tep death*)
BHK_EXP_NPC_BOSS_TIEU     = 0           ,	-- exp gan thang vao con NPC boss tieu (dang 0)

-- [BOSUNG 30/08] bo sung cac khoa script DA DOC nhung tep nay
-- chua khai (loi cua bo khung, da sua goc trong noi_cauhinh.py).

CTC_DIEM_PHA_LONGTRU      = 300         ,	-- doc tai tinhnang\congthanhchien\lib_ctc.lua
CTC_EXP_TOP10_BANSAO      = 3000000     ,	-- doc tai tinhnang\congthanhchien\lib_ctc.lua
CTC_GIA_LENHBAI           = 200000      ,	-- doc tai missions\citywar_global\head.lua
CTC_GIA_TRA_LAI_LENHBAI   = 10000       ,	-- doc tai missions\citywar_global\head.lua
CTC_MAX_LIENMINH_CONG_VEBINH= 5           ,	-- doc tai tinhnang\congthanhchien\lib_ctc.lua
CTC_MAX_LIENMINH_THU_VEBINH= 5           ,	-- doc tai tinhnang\congthanhchien\lib_ctc.lua
CTC_MAX_NGUOI_PHE_CONG_VEBINH= 50          ,	-- doc tai tinhnang\congthanhchien\lib_ctc.lua
CTC_MAX_NGUOI_PHE_THU_VEBINH= 50          ,	-- doc tai tinhnang\congthanhchien\lib_ctc.lua

-- [CFGCTC 30/08] CONG THANH CHIEN - tran dia (missions\citywar_city\head.lua)
-- 
-- !! MAX_CAMP* o day la ban ma CUA TRAN dung (camper.lua).
-- !! Con NPC ve binh quan doanh dung ban KHAC o lib_ctc.lua - xem cac khoa
-- !! CTC_MAX_*_VEBINH ben duoi. Hai ban dang LECH NHAU.

CTC_CHUKY_LOA_GIAY        = 60          ,	-- chu ky loa trong tran: bao nhieu giay bao mot lan tinh hinh
CTC_DODAI_TRAN_PHUT       = 90          ,	-- do dai mot tran Cong Thanh (phut)
CTC_EXP_TOP10             = 3000000     ,	-- kinh nghiem thuong cho 10 nguoi dan dau bang cong trang. !! co mot ban nua o lib_ctc.lua
CTC_SO_LONGTRU            = 3           ,	-- so Long Tru trong tran. Pha du ca ba la thang ngay
CTC_SO_CONG_THANH         = 3           ,	-- so cong thanh phai pha. !! phai khop so dong trong bang DoorPos
CTC_MAX_NGUOI_PHE_THU     = 200         ,	-- so nguoi toi da phe THU vao tran (ban CUA TRAN dung). !! NPC ve binh dung so khac
CTC_MAX_NGUOI_PHE_CONG    = 200         ,	-- so nguoi toi da phe CONG vao tran (ban CUA TRAN dung)
CTC_MAX_LIENMINH_THU      = 50          ,	-- so nguoi ngoai bang tro giup phe THU (ban CUA TRAN dung)
CTC_MAX_LIENMINH_CONG     = 50          ,	-- so nguoi ngoai bang tro giup phe CONG (ban CUA TRAN dung)
CTC_CAP_LONGTRU           = 10          ,	-- cap do NPC Long Tru khi sinh ra (anh huong mau)
CTC_CAP_CONG_THANH        = 60          ,	-- cap do NPC cong thanh - quyet dinh cong kho pha den dau
CTC_DIEM_GIET_NGUOI       = 75          ,	-- diem cong trang co ban moi lan ha guc doi phuong
CTC_DIEM_LIEN_TRAM        = 150         ,	-- diem thuong moi lan dat moc lien tram

-- [CFGNEN 30/08] HE SO NEN TOAN CUC (script\lib\lib_server.lua)
-- 
-- Day la tep 14 dong khai cac he so nhan vao rat nhieu noi khac.
-- Moi so DUNG BANG gia tri dang chay. Sua xong KHOI DONG LAI GameServer.
-- 
-- !! GLB_CHE_DO_TEST dang la 1: hai NPC 'Ho Tro Test' o lang tan thu (ban do 53)
-- !! mo menu GM day du cho MOI nguoi choi - 1 ty luong + 100.000 Xu + len
-- !! thang cap 200, khong kiem tai khoan GM, khong gioi han so lan.
-- !! Dat 0 de doi ve NPC tan thu that. Xem BAOCAO_LOHONG_2908.md muc 1.
-- 
-- !! GLB_GIO_MO_SERVER bi TRUNG TEN voi mot ban trong script\lib\worldlibrary.lua:4
-- !! (gia tri cu 2104011900). Ban do van chay nhung khong bao gio thang.

GLB_GIO_MO_SERVER         = 2506251900  ,	-- moc gio mo cua, dang yymmddHHMM. 7 cho trong game doc de chan nguoi choi
GLB_CHE_DO_TEST           = 1           ,	-- 1 = MO che do thu nghiem. !! Dang mo: hai NPC 'Ho Tro Test' o lang tan thu phat do GM cho MOI nguoi choi. Dat 0 de tat
GLB_TILE_TIEN             = 1           ,	-- he so nhan tien thuong toan server (1 = giu nguyen)
GLB_TILE_EXP              = 20          ,	-- he so nhan kinh nghiem toan server. NUT CHINH de dieu chinh toc do len cap
GLB_SATTHUONG_BOSS_HK     = 1           ,	-- he so sat thuong Boss Hoang Kim tieu. (dang tat cung hoat dong)
GLB_SATTHUONG_QUAI_PLD    = 1           ,	-- he so sat thuong quai Thuy Tac tren thuyen Phong Lang Do ban Viet cu. (dang tat)
GLB_SATTHUONG_BOSS_TTDL   = 1           ,	-- he so sat thuong boss Thuy Tac Dau Linh ban Viet cu. (dang tat)
-- [PB 30/08] bo GLB_MANH_BOSS_SATTHU: he Boss Sat Thu ban Viet da go; STRONGBOSS_ST khong noi nao doc
GLB_MANH_BOSS_VUOTAI      = 1           ,	-- he so mau + sat thuong quai va boss Vuot Ai ban Viet cu. (dang tat)
GLB_MANH_NGUYENSOAI_TK    = 1           ,	-- he so mau + sat thuong Nguyen Soai hai phe trong Tong Kim - DANG CHAY
GLB_MAX_DONG_THUOCTINH    = 10          ,	-- tran so dong thuoc tinh do roi tu quai. !! khong tep script nao doc so nay - so that dang chan nam trong ma C++

-- ---------------------------------------------------------------------------
-- NHIP NAP LAI SCRIPT
-- ---------------------------------------------------------------------------
-- timerserver.lua:40 co dong `dofile("script/timerserver.lua")` tu nap lai
-- CHINH NO moi phut. Nho do sua script la an ngay khong can restart, nhung
-- moi lan la nap lai 33 Include (~103 tep, ~0,97 MB) trong 1 phut.
-- So duoi day = cu bao nhieu PHUT thi nap lai mot lan.
--   1 = y het hanh vi cu (mac dinh, khong doi gi)
--   5 = nhe hon 5 lan, sua script cho toi da 5 phut moi an
--   0 = TAT han (nhe nhat; sua script phai restart moi an)
CH_NAPLAI_PHUT = 1,

-- ---------------------------------------------------------------------------
-- GHI LOG
-- ---------------------------------------------------------------------------
-- Bat/tat log cua he cau hinh + he thuong (0/1). Duong log:
--   logs\hethong.log  (ham GhiLog cua script - dang ky o ScriptFuns.cpp)
--   ScriptError.log    (loi runtime Lua, co ca stack traceback + tep:dong)
CH_LOG_CAUHINH = 1,
CH_LOG_THUONG = 1,

-- ---------------------------------------------------------------------------
-- BOT TU DONG ([BOTAUTO 04/09] chu game: moi lan mo game tu goi bot va phan viec)
-- ---------------------------------------------------------------------------
-- Chay o script\tinhnang\botauto\bot_auto.lua, goi tu RunTime() moi phut.
-- Sua so o day roi doi TOI DA 1 phut la an (timerserver tu nap lai), KHONG can restart.
BOT_TU_GOI = 1,		-- 1 = bat toan bo phan tu dong duoi day | 0 = tat het
BOT_SO_LUONG = 1000,	-- so bot goi khi may chu len (tran engine PB_MAX_BOTS = 1000)
BOT_CHO_PHUT = 2,	-- doi bao nhieu phut sau khi may chu len roi moi goi (cho Goddess san sang)
BOT_GOI_GIAN = 10,	-- toi thieu bao nhieu phut giua hai lan xep hang doi sinh bot
BOT_DA_TAU = 200,	-- so bot di lam Da Tau (CHI chon duoc bot cap >= 70)
BOT_BAN_SAP = 200,	-- so bot ve thanh bay sap ban hang
BOT_SAP_NGUONG = 60,	-- sap tut duoi bao nhieu %% so tren thi moi dat lai (dat lai = dep het sap cu)
BOT_SAP_GIAN = 15,	-- toi thieu bao nhieu phut giua hai lan dat lai ban sap
BOT_TK_TRAN = 500,	-- tran bot moi tran Tong Kim (0 = khong gioi han)
BOT_TK_TU_DONG = 1,	-- 1 = bot tu vao Tong Kim THEO GIO | 0 = tat
BOT_BAO_PHUT = 10,	-- cu bao nhieu phut in mot dong trang thai [BotAuto] vao logs\hethong.log (0 = tat)
BOT_VAO_PHAI = 0,	-- [BOTAUTO3] chu game: bot da tren cap 100, KHONG can vao phai nua (1 = bat lai)
BOT_TU_DANH = 1,	-- [BOTAUTO2] 1 = tu BAT DANH QUAI (khong bat thi bot dung yen o thanh)
BOT_DANH_GIAN = 5,	-- cu bao nhieu phut goi lai lenh bat danh quai (bat cho bot moi sinh)

}
