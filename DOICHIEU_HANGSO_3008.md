# ĐỐI CHIẾU HẰNG SỐ CÒN LẠI (30/08/2026)

> Kết quả đối chiếu của 6 nhóm khảo sát, đã lọc trùng và loại những gì đã nối dây.
> **Chưa thi công** — đây là danh sách việc cho các đợt sau.
>
> Trạng thái lúc đối chiếu: 152 khoá đã nối. Danh sách dưới đây là phần còn lại.

---

# ĐỐI CHIẾU & LỌC 6 NHÓM — KẾT QUẢ CHỐT (30/08)

## 0. TRẠNG THÁI THỰC TẾ VỪA ĐO — LOẠI BỎ NGAY 2 NHÓM RƯỠI

Tôi đọc thẳng cây script trước khi lọc. **Cấu hình đã chạy trước báo cáo của 3 nhóm** — các nhóm đó khảo sát trên bản cũ:

**A. NHÓM 1 (GLB_) — BỎ TOÀN BỘ, ĐÃ NỐI DÂY XONG.**
`script\lib\lib_server.lua` nay **27 dòng** (không phải 14), đã dùng `GLB_CFG`. 11 khoá đã khai đủ tại `script\cauhinh\ch_chung.lua:29-39`:
`GLB_GIO_MO_SERVER GLB_CHE_DO_TEST GLB_TILE_TIEN GLB_TILE_EXP GLB_SATTHUONG_BOSS_HK GLB_SATTHUONG_QUAI_PLD GLB_SATTHUONG_BOSS_TTDL GLB_MANH_BOSS_SATTHU GLB_MANH_BOSS_VUOTAI GLB_MANH_NGUYENSOAI_TK GLB_MAX_DONG_THUOCTINH`
Số dòng nhóm 1 báo (4-14) nay là **16-26**. Hai tên khoá nhóm 1 đề xuất đã bị bản chạy đặt khác: `GLB_SATTHUONG_BOSS_HOANGKIM`→`GLB_SATTHUONG_BOSS_HK`, `GLB_MAX_DONG_THUOCTINH_DOROI`→`GLB_MAX_DONG_THUOCTINH`.

**B. NHÓM 3 (TKT_) — 17 khoá đã có, bỏ khỏi danh sách.**
Đã khai trong `cauhinh`: `TKT_CAP_TOI_THIEU TKT_LECH_QUANSO_TOIDA TKT_PHI_BAO_DANH TKT_THUONG_TICHLUY_THANG TKT_THUONG_TICHLUY_THUA TKT_THUONG_TICHLUY_HOA TKT_DIEM_TOITHIEU_NHAN_THUONG TKT_SO_CO_TOIDA TKT_DIEM_CAM_CO TKT_MUC_GIAM_TOCDO_GIU_CO TKT_GIAY_TRONG_DOANHTRAI TKT_GIAY_DELAY_RA_TRAI TKT_MAU_NGUYENSOAI TKT_DIEM_GIET_NGUYENSOAI`
Đã đọc trong script **nhưng chưa khai** (chủ sửa sẽ không tìm thấy): `TKT_PHI_TRINHSAT TKT_GIA_DOI_NHACVUONGKIEM TKT_SL_HONTHACH_DOI_KIEM`

**C. NHÓM 2 (CTC_/CTLD_) — 18 khoá đã có, bỏ khỏi danh sách.**
`CTC_CHUKY_LOA_GIAY CTC_DODAI_TRAN_PHUT CTC_EXP_TOP10 CTC_SO_LONGTRU CTC_SO_CONG_THANH CTC_MAX_NGUOI_PHE_THU CTC_MAX_NGUOI_PHE_CONG CTC_MAX_LIENMINH_THU CTC_MAX_LIENMINH_CONG CTC_CAP_LONGTRU CTC_CAP_CONG_THANH CTC_DIEM_GIET_NGUOI CTC_DIEM_LIEN_TRAM CTLD_MAX_NGUOI_MOI_PHE CTLD_EXP_BANG_THANG CTLD_CHUKY_LOA_GIAY CTLD_DODAI_TRAN_PHUT CTLD_GIAY_CHO_VAO_DAUTRUONG`

**D. Bộ khoá cũ đầy đủ tính đến lúc đo** (đừng khai lại): `BAT_ TK_ TW_ TS_ BR_ BW_ TC_ HD3_ HCD_ QDHK_ YDBZ_ SKD_ DRQ_ BRXP_ VT_ GLB_ CH_` + 17 `TKT_` + 18 `CTC_/CTLD_` ở trên.

---

## 1. HẰNG SỐ CÒN LẠI — ĐANG CHẠY — XẾP THEO GIÁ TRỊ CHỈNH

### HẠNG A — VÒI EXP/TIỀN ĐANG CHẢY (chỉnh là thấy ngay tốc độ server)

```
event\trongbanghoi\lib.lua:14        EXP_TBH_DAY   = 200000000  -> TBH_EXP_TRAN_NGAY        -- trần exp/ngày từ đánh trống bang; kiểm tra làm TRƯỚC vòng lặp nên vượt được thêm một nhịp
event\trongbanghoi\lib.lua:15        EXP_RAN1      = 1000000    -> TBH_EXP_MIN              -- exp thấp nhất MỖI nhịp; một lần đánh có 10 nhịp
event\trongbanghoi\lib.lua:16        EXP_RAN2      = 2000000    -> TBH_EXP_MAX              -- exp cao nhất MỖI nhịp; 5 giây ăn 10-20 triệu
event\trongbanghoi\tungtung1.lua:58  FnDanhTrongOkay1 = 10      -> TBH_SO_NHIP_MOI_LAN      -- số lần cộng exp cho MỘT lần đánh trống; hạ về 1 là hết lạm phát
event\trongbanghoi\tungtung1.lua:43  main          = 5          -> TBH_GIAY_DANH_TRONG      -- số giây mỗi nhịp đánh trống của bang mình
missions\citywar_global\infocenter_head.lua:271  sure_GiveTiaoZhanLing = 50000 -> CTC_EXP_MOI_LENHBAI  -- exp cho MỖI khiêu chiến lệnh nộp; nhân trần 300/ngày = 15 triệu/người/ngày
global\seasonnpc.lua:111             Task_NewVersionAward = 30000000  -> DT_EXP_MOC_NGAY_1  -- exp mốc làm 30 nhiệm vụ Đả Tẩu trong ngày
global\seasonnpc.lua:117             Task_NewVersionAward = 100000000 -> DT_EXP_MOC_NGAY_2  -- exp mốc làm tròn 40 nhiệm vụ Đả Tẩu trong ngày
tinhnang\tong_kim_tcap\lib_tktc.lua:715  ThuongPheThangTongKim = 15000000 -> TKT_EXP_COBAN_THANG -- exp gốc phe thắng; nhân quân hàm x3, trần 270 triệu
tinhnang\tong_kim_tcap\lib_tktc.lua:733  ThuongPheThuaTongKim  = 10000000 -> TKT_EXP_COBAN_THUA  -- exp gốc phe thua; CAO GẤP ĐÔI phe hoà, gần chắc là gõ nhầm
tinhnang\tong_kim_tcap\lib_tktc.lua:697  ThuongHaiPheHoaTongKim = 5000000 -> TKT_EXP_COBAN_HOA   -- exp gốc khi trận hoà; nhân quân hàm x2
tinhnang\tong_kim_tcap\lib_tktc.lua:714  ThuongPheThangTongKim = 3        -> TKT_HESO_THUONG_THANG -- hệ số nhân quân hàm cho phe thắng
tinhnang\tong_kim_tcap\lib_tktc.lua:731  ThuongPheThuaTongKim  = 1        -> TKT_HESO_THUONG_THUA
tinhnang\tong_kim_tcap\lib_tktc.lua:696  ThuongHaiPheHoaTongKim = 2       -> TKT_HESO_THUONG_HOA
tinhnang\tong_kim_tcap\quanquan.lua:91   okaytichluytk2exp = 1000  -> TKT_TYLE_TICHLUY_RA_EXP -- 1 điểm tích luỹ Tống Kim đổi ra 1000 exp
petsys\transferexp.lua:11            PetSys:TransferExp = 50000000 -> BDH_EXP_TRU_MOI_LAN_TRUYEN -- exp trừ mỗi lần truyền cho Bạn Đồng Hành
petsys\transferexp.lua:10            PetSys:TransferExp = 60000000 -> BDH_EXP_CAN_DE_TRUYEN      -- exp phải có sẵn mới được truyền
tinhnang\loidaihonchien\bigiet.lua:12  OnPlayerDeath = 10000000 -> LDHC_EXP_MOI_MANG        -- exp mỗi mạng ở Lôi Đài Hỗn Chiến; NPC báo danh vẫn đứng dù BAT_=0
task\newtask\tasklink\tasklink_award.lua:99   tl_giveplayeraward = 0.36  -> DT_HESO_EXP_THUONG  -- hệ số quy giá trị nhiệm vụ Đả Tẩu ra exp (rồi nhân ngẫu nhiên 80-120%)
task\newtask\tasklink\tasklink_award.lua:76   tl_giveplayeraward = 0.0575 -> DT_HESO_TIEN_THUONG -- hệ số ra tiền thưởng Đả Tẩu (0.05*1.15, nhân ngẫu nhiên 60-100%)
petsys\feed.lua:38                   main = 1000000  -> BDH_EXP_THUONG_KHI_CHO_AN  -- exp nhân vật mỗi lần cho Bạn Đồng Hành ăn
```

### HẠNG B — GIỚI HẠN LƯỢT MỖI NGÀY (van chặn cày quá tay)

```
global\seasonnpc.lua:51              checkTask_Limit = 40   -> DT_LUOT_MOI_NGAY          -- số lượt nhiệm vụ Đả Tẩu mỗi ngày
global\seasonnpc.lua:279             tasklink_entence = 40  -> DT_LUOT_MOI_NGAY          -- bản chép lại của cùng con số, dùng để hiện "còn bao nhiêu lượt"
global\seasonnpc.lua:109             Task_NewVersionAward = 30 -> DT_MOC_NGAY_1          -- mốc thưởng thứ nhất trong ngày
global\seasonnpc.lua:115             Task_NewVersionAward = 40 -> DT_MOC_NGAY_2          -- mốc thưởng thứ hai (không huỷ lần nào)
global\seasonnpc.lua:829             Task_GiveAward = 10       -> DT_CHU_KY_MOC_ITEM     -- cứ 10 nhiệm vụ được thêm một phần thưởng đặc biệt
global\seasonnpc.lua:704             Task_Cancel = 2           -> DT_SO_LAN_HUY_TRUOC_PHAT -- huỷ quá số lần này liên tiếp thì bị phạt chờ
global\seasonnpc.lua:256             tasklink_entence = 10890  -> DT_TICK_PHAT_HUY_NHIEU -- thời gian bị phạt, tính bằng tick (605 giây ~10 phút)
global\seasonnpc.lua:685             Task_Cancel = 100         -> DT_SHXT_DE_HUY_NV      -- số mảnh Sơn Hà Xã Tắc để mua một lần huỷ nhiệm vụ
task\newtask\tasklink\tasklink_head.lua:13  TL_MAXTIMES  = 20  -> DT_SO_NV_MOI_VONG      -- số nhiệm vụ cho một vòng
task\newtask\tasklink\tasklink_head.lua:14  TL_MAXLINKS  = 20  -> DT_SO_VONG_MOI_CHU_KY
task\newtask\tasklink\tasklink_head.lua:15  TL_MAXLOOPS  = 20  -> DT_SO_CHU_KY_TOI_DA
missions\citywar_global\infocenter_head.lua:197  GiveTiaoZhanLing = 300 -> CTC_MAX_NOP_LENHBAI_NGAY -- lệnh bài nộp tối đa mỗi người mỗi ngày
missions\citywar_global\infocenter_head.lua:241  sure_GiveTiaoZhanLing = 300 -> CTC_MAX_NOP_LENHBAI_NGAY -- nơi kiểm thứ hai của cùng hạn mức
missions\citywar_global\infocenter_head.lua:331  DealBuyCard = 30       -> CTC_MAX_MUA_LENHBAI_MOI_LAN
missions\citywar_global\infocenter_head.lua:647  want_signupcitywar = 1000000 -> CTC_MAX_DAT_LENHBAI_MOI_LAN
missions\citywar_global\infocenter_head.lua:681  sure_signupcitywar = 1000000 -> CTC_MAX_DAT_LENHBAI_MOI_LAN -- nơi kiểm thứ hai
missions\citywar_global\infocenter_head.lua:216  GiveTiaoZhanLing = 2000000000 -> CTC_TRAN_TONG_LENHBAI_BANG -- trần tổng lệnh bài một bang tích được
missions\citywar_global\citywar_function.lua:201 get_city_orecount = 300 -> CTC_SO_HUYENTINH_DO_THANH   -- huyền tinh cấp 5 tối đa/tuần nếu chiếm đô thành
missions\citywar_global\citywar_function.lua:203 get_city_orecount = 200 -> CTC_SO_HUYENTINH_THANH_THUONG
tinhnang\boss_hoangkim\ruong.lua:9   main = 4       -> BHK_SO_RUONG_TOI_DA_MOI_NGAY  -- mỗi ngày nhặt tối đa 4 rương boss (kiểm hai lần, dòng 9 và 25)
petsys\common.lua:49                 MAX_FRUIT_COUNT_DAILY = 4 -> BDH_SO_LAN_CHO_AN_MOI_NGAY -- đếm riêng từng loại quả
petsys\transferexp.lua:12            PetSys:TransferExp = 25 -> BDH_SO_LAN_TRUYEN_MOI_NGAY
task\partner\task_head.lua:186       CheckRewindState = 3    -> BDH_LUOT_NV_HOIUC_MOI_NGAY
task\partner\reward\partner_reward.lua:34,41  reward_startreward = 5 -> BDH_LUOT_TULUYEN_MOI_NGAY_BAC1
task\partner\reward\partner_reward2.lua:39   reward_startreward2 = 10 -> BDH_LUOT_TULUYEN_MOI_NGAY_BAC2
task\partner\reward\partner_reward3.lua:37   reward_startreward3 = 20 -> BDH_LUOT_TULUYEN_MOI_NGAY_BAC3
huoyuedu\huoyuedu.lua:10             tbHuoYueDu.nMaxHuoYueDu = 100 -> HN_DIEM_NANGDONG_TOI_DA_NGAY -- phải khớp MaxHuoyuedu bên client activityinfo.ini
huoyuedu\huoyuedu.lua:18             STATE = 1  -> HN_BAT_DO_NANGDONG   -- công tắc bật/tắt toàn hệ độ năng động
global\seasonnpc.lua:215             make_round = 2 -> HN_LIENHOA_LUOT_MOI_NGAY -- mỗi ngày kết được 2 vòng Liên Hoa cùng hệ
```

### HẠNG C — GIỜ GIẤC & LỊCH (Công Thành JX2 đang chạy thật)

```
timerserver_ctc.lua:10   TB_CTC6 = {{3,4},{1,2},{2,3},{5,6},{4,5},{0,1},{6,0}} -> CTC_LICH_TUAN -- lịch 7 thành: {thứ báo danh, thứ đánh}, 0=Chủ Nhật
missions\citywar_global\citywar_function.lua:6  TB_CITYWAR_ARRANGE = (y hệt bảng trên) -> CTC_LICH_TUAN -- BẢN SAO THỨ HAI, NPC dùng bảng này; phải gộp về một khoá kẻo NPC và đồng hồ lệch nhau
timerserver_ctc.lua:201  CTC_JX2_Tick = 18 -> CTC_GIO_MO_BAODANH   -- giờ mở cửa báo danh công thành (nơi 1/5)
missions\citywar_global\infocenter_head.lua:147,428,671,722  = 18 -> CTC_GIO_MO_BAODANH -- 4 nơi chép lại cùng giờ 18 trên NPC Sứ Giả
timerserver_ctc.lua:212  CTC_JX2_Tick = 19 -> CTC_GIO_CHOT_BAODANH -- giờ chốt bang khiêu chiến + bốc thăm cặp đấu
missions\citywar_global\infocenter_head.lua:147,428,671,722  = 19 -> CTC_GIO_CHOT_BAODANH -- 4 nơi chép lại
timerserver_ctc.lua:225  CTC_JX2_Tick = 20 -> CTC_GIO_KHAI_CHIEN   -- 20h mở Lôi Đài (ngày báo danh) hoặc Công Thành (ngày đánh)
timerserver_ctc.lua:196  CTC_JX2_Tick = 0  -> CTC_GIO_DONDEP_TU    -- đầu cửa sổ dọn dẹp hằng ngày
timerserver_ctc.lua:196  CTC_JX2_Tick = 6  -> CTC_GIO_DONDEP_DEN   -- cuối cửa sổ; server tắt suốt 0-6h là hôm đó bỏ dọn
missions\citywar_global\citywar_function.lua:97  check_award_condition = 1  -> CTC_THU_LANH_THUONG   -- thứ Hai mới được lãnh thưởng chiếm thành
missions\citywar_global\citywar_function.lua:97  check_award_condition = 9  -> CTC_GIO_LANHTHUONG_TU
missions\citywar_global\citywar_function.lua:97  check_award_condition = 20 -> CTC_GIO_LANHTHUONG_DEN
missions\citywar_global\citywar_function.lua:147 take_tong_award = 18   -> CTC_GIO_HET_THUONG_THANHVIEN -- sau giờ này thành viên hết tự lãnh, phải nhờ bang chủ
missions\citywar_global\citywar_function.lua:172 take_tong_resaward = 18 -> CTC_GIO_BANGCHU_LAYDU_TU
missions\citywar_global\citywar_function.lua:172 take_tong_resaward = 20 -> CTC_GIO_BANGCHU_LAYDU_DEN
missions\citywar_global\infocenter_head.lua:531  checkIsTakeQingtongDing = 19 -> CTC_GIO_NHANLAI_LENHBAI_TU -- 19h-24h bang thua cuộc đua nhận lại lệnh bài
missions\citywar_global\mission.lua:2    INTERVAL = 5 -> CTC_CHUKY_QUET_MO_TRAN_PHUT -- 5 phút quét một lần xem có trận nào tới giờ; là độ trễ giữa "đồng hồ báo 20h" và "trận mở thật"
global\seasonnpc.lua:139  main = 60808..60815 -> HN_LIENHOA_KHOANG_NGAY -- khoảng ngày mở hoạt động Vu Lan trên NPC Đả Tẩu
tinhnang\tong_kim_tcap\mobinhtk.lua:376  common_kim = 5 -> TKT_PHUT_IDLE_DAY_RA -- đứng im 5 phút là bị đẩy khỏi Tống Kim (bản chép lại ở dòng 445)
tinhnang\tong_kim_tcap\lib_tktc.lua:684  AddRunSpeed = 60 -> TKT_PHUT_GIAM_TOCDO_GIU_CO -- người vác cờ bị giảm tốc trong 60 phút
event\trongbanghoi\lib.lua:12  nStartTime = 1900 -> TBH_GIO_MO  -- giờ sớm nhất dùng được Trống Khải Hoàn (HHMM); TÊN BIẾN QUÁ CHUNG, bắt buộc đổi
event\trongbanghoi\lib.lua:13  nCloseTime = 2359 -> TBH_GIO_DONG -- giờ muộn nhất; tên biến quá chung, bắt buộc đổi
tinhnang\loidaihonchien\mainloidai.lua:30  thamgialoidai = 16 -> LDHC_GIO_1  -- giờ NPC nhận báo danh Lôi Đài Hỗn Chiến
tinhnang\loidaihonchien\mainloidai.lua:30  thamgialoidai = 22 -> LDHC_GIO_2
tinhnang\loidaihonchien\mainloidai.lua:30  thamgialoidai = 59 -> LDHC_PHUT_DONG_BAODANH
```

### HẠNG D — ĐIỀU KIỆN VÀO CỬA (cấp, phí, ô trống, thời gian trong bang)

```
missions\citywar_global\infocenter_head.lua:724  checkSignUpCityWar = 18 -> CTC_CAP_BANG_TOITHIEU -- cấp bang tối thiểu để báo danh; TRÙNG con số 18 với giờ báo danh, phải tách khoá riêng
missions\citywar_city\zhongzhuan_map\trap.lua:44  CheckAndJoin = 7200  -> CTC_PHUT_VAO_BANG_TOITHIEU -- phải ở trong bang 7200 phút (5 ngày) mới vào với tư cách người bang
missions\citywar_city\zhongzhuan_map\trap.lua:98  JoinWithCard = 5     -> CTC_HAN_LENHBAI_NGAY  -- hạn Thành Chiến Lệnh Bài tính bằng NGÀY
missions\citywar_global\infocenter_head.lua:364   CheckCard = 7200     -> CTC_HAN_LENHBAI_PHUT  -- CÙNG một hạn nhưng viết bằng PHÚT ở chỗ khác; sửa phải sửa cả hai
missions\citywar_global\head.lua:20   CardPrice = 200000        -> CTC_GIA_LENHBAI       -- giá một Thành Chiến Lệnh Bài
missions\citywar_global\head.lua:22   ReturnCardPrice = 10000   -> CTC_GIA_TRA_LAI_LENHBAI -- chỉ hoàn 5% giá mua
missions\citywar_global\citywar_function.lua:183  take_tong_resaward = 20 -> CTC_ODU_TRONG_BANGCHU
missions\citywar_global\infocenter_head.lua:585   sure_takeQingtongDing = 6 -> CTC_ODU_TRONG_NHANLAI
missions\citywar_arena\camper.lua:81  OnJoin = 90  -> CTLD_CAP_TOITHIEU  -- cấp tối thiểu vào Lôi Đài, phe thứ nhất
missions\citywar_arena\camper.lua:87  OnJoin = 90  -> CTLD_CAP_TOITHIEU  -- phe thứ hai; sửa một nơi quên nơi kia là hai phe khác chuẩn
missions\citywar_global\infocenter_head.lua:94   SignupACity = 1000000   -> CTLD_TIEN_DAUTHAU_TOITHIEU
missions\citywar_global\infocenter_head.lua:111  SignUpTheOne = 99999999 -> CTLD_TIEN_DAUTHAU_TOIDA
tinhnang\loidaihonchien\mainloidai.lua:21  thamgialoidai = 90 -> LDHC_CAP_TOITHIEU
tinhnang\loidaihonchien\mainloidai.lua:13  cost = 0           -> LDHC_PHI_BAODANH  -- đang 0 nên vào miễn phí
event\trongbanghoi\tungtung1.lua:18  main = 80  -> TBH_CAP_TOITHIEU  -- cấp tối thiểu đánh trống bang mình
event\trongbanghoi\drum.lua:23       main = 80  -> TBH_CAP_TOITHIEU  -- cấp tối thiểu dùng vật phẩm Trống Khải Hoàn; PHẢI đổ cùng khoá với dòng trên
petsys\common.lua:55   MIN_LEVEL = 150      -> BDH_CAP_NHANVAT_TOI_THIEU  -- cấp nhân vật để tạo Bạn Đồng Hành
petsys\petcard.lua:11  main = 2             -> BDH_SO_LAN_CHUYENSINH_TOI_THIEU -- phải chuyển sinh 2 lần (nếu đúng 2 thì phải từ cấp 150)
petsys\common.lua:53   MAX_LEVEL = 130      -> BDH_CAP_TOI_DA
petsys\common.lua:106  EXT_SKILL_OPEN_PET_LEVEL = 21 -> BDH_CAP_MO_KYNANG_BIKIP
petsys\common.lua:108  EXT_SKILL_GET_NEW_LEVEL  = 5  -> BDH_BUOC_CAP_MO_O_KYNANG
petsys\common.lua:107  EXT_SKILL_MAX_COUNT      = 4  -> BDH_SO_O_KYNANG_TOI_DA
petsys\common.lua:54   PET_LEVEL_STEP = 10  -> BDH_BUOC_CAP_NGOAI_QUAN
petsys\common.lua:51   CHANGE_FEATURE_COIN = 5 -> BDH_GIA_DOI_NGOAI_QUAN  -- số Xu đổi ngoại quan
petsys\common.lua:52   CHANGE_NAME_COIN    = 5 -> BDH_GIA_DOI_TEN
petsys\xiuzhen.lua:6   ZHENYUAN_RATE  = 200   -> BDH_CHANNGUYEN_DOI_1_TUCHAN
petsys\xiuzhen.lua:7   XIUZHEN_MAX    = 20000 -> BDH_TUCHAN_TOI_DA
petsys\petequip.lua:9  PETEQUIP_O_SO = 10     -> BDH_SO_O_TRANGBI
task\partner\reward\partner_reward.lua:66  reward_beckon = 10 -> BDH_CHENH_CAP_TOI_DA_NV -- cuộn nhiệm vụ cao hơn cấp pet tối đa 10 cấp
task\partner\master\partner_master_main_05.lua:112  taskProcess_005_02 = 90/80 -> BDH_CAP_YEUCAU_NV_SUPHU_CUOI -- nhân vật 90 + pet 80
task\partner\master\partner_master_main_05.lua:172  taskProcess_005_Finish = 15 -> BDH_SO_NGAY_NV_SUPHU_CUOI -- lặp ở dòng 161 và 232
global\seasonnpc.lua:153  menglanjie = 30 -> HN_LIENHOA_CAP_TOI_THIEU  -- cấp 30 + đã nạp thẻ mới vào Liên Hoa
global\seasonnpc.lua:169  MAKING_COST  = 10000 -> HN_LIENHOA_GIA_KET_VONG
global\seasonnpc.lua:170  MAKING_COUNT = 9     -> HN_LIENHOA_SO_HOA_CAN  -- khai biến nhưng mã lại viết cứng số 9 ở dòng 203 và 221
global\seasonnpc.lua:463,818,917  Task_Accept/Task_GiveAward/mySG = 5 -> DT_O_TRONG_CO_BAN -- ô trống cơ bản, ba nơi
global\seasonnpc.lua:119  Task_NewVersionAward = 6 -> DT_O_TRONG_CAN_MOC40
global\seasonnpc.lua:832  Task_GiveAward = 3       -> DT_O_TRONG_CAN_MOC10
item\ruong_datau_tasklink.lua:13  main = 1 -> DT_RUONG_CAN_CHUY   -- số Huyền Thiên Chuỳ để mở rương Đả Tẩu (chủ đã hạ từ 6 xuống 1)
item\ruong_datau_tasklink.lua:17  main = 4 -> DT_RUONG_O_TRONG
petsys\petbox.lua:18  main = 1 -> BDH_RUONG_CAN_CHIAKHOA
tinhnang\tong_kim_tcap\quany.lua:30  muamaunhanh = 1 -> TKT_GIA_1_O_MAU -- giá mỗi ô thuốc mua nhanh tại Quân Y
tinhnang\tong_kim_tcap\trinhsat.lua:93  tong_ratran = 3 -> TKT_GIAY_BAOVE_SAU_TRINHSAT -- giây bất tử sau khi Trinh Sát đưa ra trận (lặp ở kim_ratran)
```

### HẠNG E — SỨC MẠNH NPC / BOSS

```
tinhnang\boss_hoangkim\lib_bosshk.lua:25  LIFE_BOSS_DAIH = 1 -> BHK_MAU_BOSS_DAI -- MÁU BOSS ĐANG LÀ 1 (ghi chú nói mặc định 18.000.000); dùng chung cho boss đại + boss máy chủ + boss PLD
tinhnang\boss_hoangkim\lib_bosshk.lua:24  LIFE_BOSS_TIEU = 18000000 -> BHK_MAU_BOSS_TIEU
tinhnang\boss_hoangkim\lib_bosshk.lua:13  TIME_LIFE_BOSS = 129600  -> BHK_PHUT_BOSS_TON_TAI  -- boss sống 120 phút rồi biến mất
tinhnang\boss_hoangkim\lib_bosshk.lua:26  NETRANHBOSS = 5000 -> BHK_NETRANH_BOSS
tinhnang\boss_hoangkim\lib_bosshk.lua:27  PHSLBOSS    = 300  -> BHK_HOIMAU_BOSS
tinhnang\boss_hoangkim\lib_bosshk.lua:372 addnpcbossdaihk  = 2 -> BHK_HESO_HOIMAU_BOSS_DAI
tinhnang\boss_hoangkim\lib_bosshk.lua:95  addnpcbosspld    = 3 -> BHK_HESO_HOIMAU_BOSS_PLD  -- boss máy chủ dòng 265 cũng x3
tinhnang\boss_hoangkim\lib_bosshk.lua:95  addnpcbosspld    = 2 -> BHK_HESO_NETRANH_BOSS_PLD
tinhnang\boss_hoangkim\lib_bosshk.lua:226 addnpcbosstieuhk = 8000  -> BHK_DOCHINHXAC_BOSS_TIEU
tinhnang\boss_hoangkim\lib_bosshk.lua:372 addnpcbossdaihk  = 10000 -> BHK_DOCHINHXAC_BOSS_DAI
tinhnang\boss_hoangkim\lib_bosshk.lua:226 addnpcbosstieuhk = 80    -> BHK_TGPHUCHOI_BOSS  -- lặp ở 95, 265, 372
tinhnang\boss_hoangkim\lib_bosshk.lua:98  addnpcbosspld    = 70 -> BHK_GIAM_SATTHUONG_BOSS_PLD
tinhnang\boss_hoangkim\lib_bosshk.lua:268 addnpcbossserver = 70 -> BHK_GIAM_SATTHUONG_BOSS_MAYCHU
tinhnang\boss_hoangkim\lib_bosshk.lua:279 QY_GOLDBOSS_APOS_INFO = 95 -> BHK_CAP_BOSS_DAI -- cấp của cả 11 boss đại (cột 3, lặp tới dòng 301)
tinhnang\boss_hoangkim\lib_bosshk.lua:353 addnpcbossdaihk = 10 -> BHK_SOLAN_THU_TIM_MAP -- hết 10 lần bốc không ra map trống là con boss đó không được đặt
tinhnang\boss_hoangkim\deathhktieu.lua:58 OnRevive = 500 -> BHK_SATTHUONG_HE_BOSS_TIEU  -- lặp ở 66,68,70,72
tinhnang\boss_hoangkim\deathhktieu.lua:60 OnRevive = 200 -> BHK_SATTHUONG_DOC_BOSS_TIEU
tinhnang\boss_hoangkim\deathhktieu.lua:78 OnRevive = 75  -> BHK_KHANG_BOSS_TIEU
tinhnang\boss_hoangkim\deathhkdai.lua:63  OnRevive = 500 -> BHK_SATTHUONG_HE_BOSS_DAI   -- lặp ở 69,75,81,87,93
tinhnang\boss_hoangkim\deathhkdai.lua:102 OnRevive = 95  -> BHK_KHANG_BOSS_DAI
tinhnang\boss_hoangkim\deathhkdai.lua:96  OnRevive = 10  -> BHK_TOCDO_BOSS_DAI
tinhnang\boss_hoangkim\deathbosspld.lua:60 OnRevive = 100 -> BHK_SATTHUONG_HE_BOSS_PLD  -- lặp ở 66,72,78,84,90
tinhnang\boss_hoangkim\deathbosspld.lua:99 OnRevive = 75  -> BHK_KHANG_BOSS_PLD
tinhnang\boss_hoangkim\deathhksv.lua:177  OnRevive = 20  -> BHK_TOCDO_BOSS_MAYCHU
tinhnang\boss_hoangkim\deathhksv.lua:182  OnRevive = 95,120,95,120,95 -> BHK_KHANG_BOSS_MAYCHU -- hai hệ để 120, vượt mốc 100%
tinhnang\boss_hoangkim\deathhktieu.lua:74 OnRevive = 600 -> BHK_TAM_TRUY_DUOI_BOSS -- lặp ở deathhkdai:99, deathbosspld:96, deathhksv:178
timertask\task02.lua:90   AddSoaiPheTong = 1000 -> TKT_SATTHUONG_NGUYENSOAI
timertask\task02.lua:106  AddSoaiPheKim  = 1000 -> TKT_SATTHUONG_NGUYENSOAI  -- bản chép lại, sửa phải sửa cả hai
tinhnang\tong_kim_tcap\lib_tktc.lua:515  addnpcquaitktrungcap = 36000  -> TKT_MAU_NPC_BINHSI     -- bản phe Kim y hệt ở dòng 546
tinhnang\tong_kim_tcap\lib_tktc.lua:515  addnpcquaitktrungcap = 100/200 -> TKT_SATTHUONG_MIN_BINHSI / TKT_SATTHUONG_MAX_BINHSI
tinhnang\tong_kim_tcap\lib_tktc.lua:515  addnpcquaitktrungcap = 2320   -> TKT_PHUCSINH_BINHSI
tinhnang\tong_kim_tcap\lib_tktc.lua:521  addnpcquaitktrungcap = 70000  -> TKT_MAU_NPC_HIEUUY      -- bản phe Kim ở dòng 552
tinhnang\tong_kim_tcap\lib_tktc.lua:521  addnpcquaitktrungcap = 200/300 -> TKT_SATTHUONG_MIN_HIEUUY / TKT_SATTHUONG_MAX_HIEUUY
tinhnang\tong_kim_tcap\lib_tktc.lua:521  addnpcquaitktrungcap = 4320   -> TKT_PHUCSINH_HIEUUY
tinhnang\tong_kim_tcap\lib_tktc.lua:527  addnpcquaitktrungcap = 150000 -> TKT_MAU_NPC_THONGLINH   -- bản phe Kim ở dòng 558
tinhnang\tong_kim_tcap\lib_tktc.lua:527  addnpcquaitktrungcap = 350/500 -> TKT_SATTHUONG_MIN_THONGLINH / TKT_SATTHUONG_MAX_THONGLINH
tinhnang\tong_kim_tcap\lib_tktc.lua:527  addnpcquaitktrungcap = 7560   -> TKT_PHUCSINH_THONGLINH
tinhnang\tong_kim_tcap\lib_tktc.lua:533  addnpcquaitktrungcap = 250000 -> TKT_MAU_NPC_PHOTUONG    -- bản phe Kim ở dòng 564
tinhnang\tong_kim_tcap\lib_tktc.lua:533  addnpcquaitktrungcap = 400/580 -> TKT_SATTHUONG_MIN_PHOTUONG / TKT_SATTHUONG_MAX_PHOTUONG
tinhnang\tong_kim_tcap\lib_tktc.lua:533  addnpcquaitktrungcap = 7560   -> TKT_PHUCSINH_PHOTUONG
tinhnang\tong_kim_tcap\lib_tktc.lua:539  addnpcquaitktrungcap = 350000 -> TKT_MAU_NPC_DAITUONG    -- bản phe Kim ở dòng 570
tinhnang\tong_kim_tcap\lib_tktc.lua:539  addnpcquaitktrungcap = 450/670 -> TKT_SATTHUONG_MIN_DAITUONG / TKT_SATTHUONG_MAX_DAITUONG
tinhnang\tong_kim_tcap\lib_tktc.lua:539  addnpcquaitktrungcap = 7560   -> TKT_PHUCSINH_DAITUONG
petsys\levelup.lua:8   tbLevelUpAttr  = "4 chỉ số +1..+5; sinh/nội lực +10..+50" -> BDH_CHISO_MOI_CAP
petsys\levelup.lua:18  attrInitValue  = "4 chỉ số 5..10; sinh/nội lực 200..250" -> BDH_CHISO_BAN_DAU
```

### HẠNG F — TỈ LỆ RƠI ĐỒ / BỐC THƯỞNG

```
item\ruong_datau_tasklink.lua:27  main = "25%/25%/30%/10%/5%/3%/2%" -> DT_RUONG_BANG_TILE -- bảng tỉ lệ mở Bảo rương thần bí Đả Tẩu, quay trên thang 1..100000
item\ruong_datau_tasklink.lua:37  main = 604800 -> DT_RUONG_HAN_QUECAO -- hạn Quế Huy Hoàng cao rơi từ rương (7 ngày)
task\newtask\tasklink\tasklink_award.lua:31  tl_giveplayeraward = {20,33,34,8,5} -> DT_TRONGSO_LOAI_THUONG -- trọng số 3 ô thưởng: tiền/exp/vật phẩm/quay lại/cơ hội huỷ
task\newtask\tasklink\tasklink_award.lua:341 tl_giveplayeraward_goods = 2000000 -> DT_GIATRI_THUONG_MOC_VONG -- càng lớn càng dễ ra đồ xịn ở mốc vòng/chu kỳ
task\newtask\tasklink\tasklink_award.lua:339 tl_giveplayeraward_goods = 0.001 -> DT_HESO_MAYMAN_THUONG   -- mỗi điểm may mắn cộng 0,1%
task\newtask\tasklink\tasklink_award.lua:390 tl_getawardagin = 0.03 -> DT_HESO_MAYMAN_QUAYLAI -- mạnh gấp 30 lần ô thưởng thường
task\newtask\tasklink\tasklink_award.lua:109 tl_giveplayeraward = 1000 -> DT_CHIA_EXP_KHI_METMOI -- lúc mệt mỏi exp bị chia 1000 rồi đổi thành điểm tích luỹ
task\newtask\tasklink\tasklink_award.lua:663 tl_linkaward_give = 86400 -> DT_GIAY_MOI_NGAY_HANDO -- quy đổi cột ExpDay của award_link.txt
tinhnang\tong_kim_tcap\drop.lua:25  DropRate = 3  -> TKT_HESO_SO_LAN_QUAY_DROP -- NPC bậc 2-5 quay (bậc x3) lần; Đại Tướng 15 lần
tinhnang\tong_kim_tcap\drop.lua:28  DropRate = 30 -> TKT_DROP_MAUSO   -- mẫu số quay drop (0..30); hạ xuống là mọi thứ rơi nhiều hơn
tinhnang\tong_kim_tcap\drop.lua:29  DropRate = 28 -> TKT_DROP_NGUONG_PHONGCU -- ~6,45% mỗi lần quay ra Tống Kim Phong Cụ
tinhnang\tong_kim_tcap\drop.lua:31  DropRate = 8  -> TKT_DROP_GIATRI_COHIEU  -- ~3,2% ra Cờ Hiệu
tinhnang\tong_kim_tcap\drop.lua:33  DropRate = 4  -> TKT_DROP_GIATRI_BAOVAT  -- ~3,2% ra Bảo Vật
tinhnang\tong_kim_tcap\drop.lua:30  DropRate = 5  -> TKT_GIAY_DO_THUOC_VE    -- đồ rơi thuộc riêng người hạ NPC 5 giây (lặp ở 32, 34)
tinhnang\boss_hoangkim\drophktieu.lua:33  DropRate = 10 -> BHK_SO_LUOT_ROI_BANG_BOSS_TIEU
tinhnang\boss_hoangkim\drophkdai.lua:35   DropRate = 10 -> BHK_SO_LUOT_ROI_BANG_BOSS_DAI
tinhnang\boss_hoangkim\drophktieu.lua:57  DropRate = 90 -> BHK_NGUONG_TUI_BIKIP_TIEU  -- ~10% ra Túi Bí Kíp
tinhnang\boss_hoangkim\drophktieu.lua:59  DropRate = 70 -> BHK_NGUONG_LENHBAI_DATAU_TIEU
tinhnang\boss_hoangkim\drophkdai.lua:39   DropRate = 10 -> BHK_NGUONG_VOLAM_MATTICH_DAI -- ~9%; lặp ở drophksv:38, dropbosspld:39
tinhnang\boss_hoangkim\drophkdai.lua:59   DropRate = 40 -> BHK_NGUONG_TUI_BIKIP_DAI     -- phải trúng đúng 40, tức 1%; lặp ở drophksv:58, dropbosspld:59
tinhnang\boss_hoangkim\drophkdai.lua:61   DropRate = 90 -> BHK_NGUONG_NGUA_9X_DAI       -- ~10%; lặp ở drophksv:60, dropbosspld:61
tinhnang\boss_hoangkim\drophkdai.lua:63   DropRate = 604800 -> BHK_NGAY_HAN_NGUA_9X     -- ngựa 9x rơi ra chỉ dùng 7 ngày
tinhnang\boss_hoangkim\drophksv.lua:74    DropRate = 10  -> BHK_SO_RUONG_ROI_BOSS_MAYCHU
tinhnang\boss_hoangkim\drophksv.lua:79    DropRate = 5400 -> BHK_PHUT_RUONG_TON_TAI     -- rương nằm trên đất 5 phút
tinhnang\boss_hoangkim\ruong.lua:33  Pickup = 2      -> BHK_HESO_EXP_MO_RUONG  -- exp = cấp độ mũ ba nhân hệ số này
tinhnang\boss_hoangkim\ruong.lua:35  Pickup = 5..30  -> BHK_SO_HUYENMINHDON_RUONG
tinhnang\boss_hoangkim\ruong.lua:16  main = 5        -> BHK_GIAY_MO_RUONG
task\partner\task_award.lua:451  skillbook_select = 30 -> BDH_TILE_RA_SACH_KYNANG
task\partner\task_award.lua:439  skillbook_select = "random(199,599)/100" -> BDH_KHOANG_CAP_SACH_KYNANG
petsys\head.lua:43  PetSys.tbPetSkill = "30/10/30/30" -> BDH_TILE_KYNANG_KHI_TAO -- tỉ lệ 4 kỹ năng khi tạo pet
tinhnang\tong_kim_tcap\lib_tktc.lua:465  RandPosTK = 500 -> TKT_TYLE_DOI_VITRI_TREN_DUOI -- 50% hai phe đổi chỗ trên-dưới mỗi trận
tinhnang\tong_kim_tcap\lib_tktc.lua:670  addnpccotongkim = 6 -> TKT_SO_VITRI_DAT_CO -- bốc 1 trong 6; bảng toạ độ có 7 dòng, dòng 7 không bao giờ trúng
```

### HẠNG G — ĐIỂM SỐ, QUÂN HÀM, PHẦN THƯỞNG VẬT PHẨM

```
tinhnang\tong_kim_tcap\kimtu.lua:74   OnPlayerDeath = 30 -> TKT_DIEM_GIET_NGUOI_COBAN -- 30 + 30*quân_hàm*2 + 30*liên_trảm; gõ cứng 3 lần ở dòng 74-76
tinhnang\tong_kim_tcap\tongtu.lua:74  OnPlayerDeath = 30 -> TKT_DIEM_GIET_NGUOI_COBAN -- bản chép lại cho phe Tống
tinhnang\tong_kim_tcap\kimtu.lua:74   OnPlayerDeath = 2  -> TKT_HESO_NHAN_QUANHAM_CUUSAT -- giết người quân hàm cao ăn điểm gấp đôi
tinhnang\tong_kim_tcap\lib_tktc.lua:123  TAB_QUANHAM[2][1] = 1000  -> TKT_MOC_QUANHAM_HIEUUY
tinhnang\tong_kim_tcap\lib_tktc.lua:124  TAB_QUANHAM[3][1] = 3000  -> TKT_MOC_QUANHAM_THONGLINH
tinhnang\tong_kim_tcap\lib_tktc.lua:125  TAB_QUANHAM[4][1] = 7000  -> TKT_MOC_QUANHAM_PHOTUONG
tinhnang\tong_kim_tcap\lib_tktc.lua:126  TAB_QUANHAM[5][1] = 10000 -> TKT_MOC_QUANHAM_DAITUONG
tinhnang\tong_kim_tcap\lib_tktc.lua:127  TAB_QUANHAM[6][1] = 35000 -> TKT_MOC_QUANHAM_DAISOAI
tinhnang\tong_kim_tcap\lib_tktc.lua:127  TAB_QUANHAM[6][2] = 1000000 -> TKT_TRAN_TICHLUY_QUANHAM -- vượt 1 triệu điểm là quân hàm ĐỨNG NGUYÊN bậc cũ
tinhnang\tong_kim_tcap\lib_tktc.lua:131  TICHLUYTK[1] = 5    -> TKT_DIEM_GIET_BINHSI
tinhnang\tong_kim_tcap\lib_tktc.lua:132  TICHLUYTK[2] = 100  -> TKT_DIEM_GIET_HIEUUY
tinhnang\tong_kim_tcap\lib_tktc.lua:133  TICHLUYTK[3] = 200  -> TKT_DIEM_GIET_THONGLINH
tinhnang\tong_kim_tcap\lib_tktc.lua:134  TICHLUYTK[4] = 400  -> TKT_DIEM_GIET_PHOTUONG
tinhnang\tong_kim_tcap\lib_tktc.lua:135  TICHLUYTK[5] = 1000 -> TKT_DIEM_GIET_DAITUONG
tinhnang\tong_kim_tcap\lib_tktc.lua:45..49  ITEM_BV_POINT = 200/220/240/260/300 -> TKT_DIEM_BAOVAT_206..210 -- điểm nhặt 5 loại bảo vật
tinhnang\tong_kim_tcap\lib_tktc.lua:699..707  ThuongHaiPheHoaTongKim = 10/2/2/100/20/2/50/2 -> TKT_SL_ITEM1023_HOA, TKT_SL_ITEM3846_HOA, TKT_SL_ITEM4728_HOA, TKT_SL_ITEM4844_HOA, TKT_SL_HUYCHUONG_HOA, TKT_SL_RUONGXANH_HOA, TKT_DIEM_SUKIEN_HOA, TKT_XU_HOA
tinhnang\tong_kim_tcap\lib_tktc.lua:717..725  ThuongPheThangTongKim = 30/5/5/500/50/10/50/5 -> TKT_SL_ITEM1023_THANG, TKT_SL_ITEM3846_THANG, TKT_SL_ITEM4728_THANG, TKT_SL_ITEM4844_THANG, TKT_SL_HUYCHUONG_THANG, TKT_SL_RUONGXANH_THANG, TKT_DIEM_SUKIEN_THANG, TKT_XU_THANG
tinhnang\tong_kim_tcap\lib_tktc.lua:735..743  ThuongPheThuaTongKim  = 20/3/3/300/30/5/50/2  -> TKT_SL_ITEM1023_THUA, TKT_SL_ITEM3846_THUA, TKT_SL_ITEM4728_THUA, TKT_SL_ITEM4844_THUA, TKT_SL_HUYCHUONG_THUA, TKT_SL_RUONGXANH_THUA, TKT_DIEM_SUKIEN_THUA, TKT_XU_THUA
timertask\task03.lua:187  PlayerEndTongKim = 3000 -> TKT_MOC_NHAN_NGUYENLIEU -- mốc điểm trong trận để nhận nguyên liệu Chân Nguyên Đan
timertask\task03.lua:188  PlayerEndTongKim = 2    -> TKT_SL_CHANNGUYENDON_3000
tinhnang\tong_kim_tcap\quanquan.lua:77,78,79,80  tichluytk2exp = 1000/2000/5000/10000 -> TKT_MUC_DOI_EXP_1..4
tinhnang\tong_kim_tcap\quanquan.lua:113  okaytichluytk2exp_bch = 2000 -> TKT_GIA_1_CHANNGUYENDAI
tinhnang\tong_kim_tcap\quanquan.lua:99,100  tichluytk2expBCH = 2000/20000 -> TKT_MUC_DOI_CHANNGUYEN_1..2
tinhnang\tong_kim_tcap\quanquan.lua:137  okaytichluytk2exp_hctk = 50 -> TKT_GIA_1_HUYCHUONG
tinhnang\tong_kim_tcap\quanquan.lua:123..126  tichluytk2expHCTK = 50/500/2500/5000 -> TKT_MUC_DOI_HUYCHUONG_1..4
missions\citywar_city\head.lua:181  TAB_RANKBONUS = {0,1000,3000,6000,10000} -> CTC_MOC_DIEM_QUANHAM -- mốc lên 5 bậc quân hàm trong trận công thành
missions\citywar_city\head.lua:163  RANK_PKBONUS  = (ma trận 5x5) -> CTC_HESO_DIEM_THEO_QUANHAM -- giết người quân hàm cao được tới gấp 2 điểm
missions\citywar_city\head.lua:171  TAB_SERIESKILL = (ma trận 5x5 0/1) -> CTC_BANG_TINH_LIENTRAM -- chặn quân hàm cao farm người yếu để nối chuỗi
missions\citywar_city\playerdeath.lua:30  OnPlayerDeath = 3 -> CTC_LIENTRAM_MOI_N_MANG
missions\citywar_city\camper.lua:38  GameOver = 5  -> CTC_SO_NGUOI_THUONG_NGAUNHIEN -- bốc 5 người cuối trận tặng Lễ hộp quà huy hoàng
missions\citywar_city\camper.lua:50  GameOver = 10 -> CTC_SO_TOP_NHAN_EXP
tinhnang\congthanhchien\lib_ctc.lua:96  KILL_TRU_POINT = 300 -> CTC_DIEM_PHA_LONGTRU -- ĐANG CHẠY THẬT: death_tru.lua:56 và :89 đọc số này
missions\citywar_global\citybulletin.lua:14  ManageCity = "0/5/10/15/20" -> CTC_CAC_NAC_THUE -- 5 nấc thuế Thái Thú; trần thật và khung giờ nằm trong C++ KJx2CityWar.cpp
tinhnang\boss_hoangkim\deathhktieu.lua:33  OnDeath = 2 -> BHK_SO_RUONG_XANH_BOSS_TIEU
tinhnang\boss_hoangkim\deathhkdai.lua:34   OnDeath = 3 -> BHK_SO_RUONG_XANH_BOSS_DAI
tinhnang\boss_hoangkim\deathbosspld.lua:31 OnDeath = 3 -> BHK_SO_RUONG_XANH_BOSS_PLD
tinhnang\boss_hoangkim\deathhksv.lua:36    OnDeath = 5 -> BHK_SO_RUONG_XANH_BOSS_MAYCHU
tinhnang\boss_hoangkim\deathhktieu.lua:35  OnDeath = 20  -> BHK_DIEM_BANG_BOSS_TIEU
tinhnang\boss_hoangkim\deathhkdai.lua:37   OnDeath = 50  -> BHK_DIEM_BANG_BOSS_DAI
tinhnang\boss_hoangkim\deathbosspld.lua:34 OnDeath = 200 -> BHK_DIEM_BANG_BOSS_PLD
tinhnang\boss_hoangkim\deathhksv.lua:48    OnDeath = 100 -> BHK_DIEM_BANG_BOSS_MAYCHU
tinhnang\boss_hoangkim\deathhksv.lua:46    OnDeath = 50  -> BHK_XU_BOSS_MAYCHU
tinhnang\boss_hoangkim\deathhksv.lua:56    OnDeath = 5..10 -> BHK_MANH_HOANGKIM_VAO_KHO_BANG
tinhnang\boss_hoangkim\deathhksv.lua:57    OnDeath = 1..3  -> BHK_THUYTINH_VAO_KHO_BANG
tinhnang\boss_hoangkim\deathhksv.lua:60    OnDeath = 1     -> BHK_KNB_VAO_KHO_BANG
tinhnang\boss_hoangkim\deathhksv.lua:282   rutmanh = 10    -> BHK_O_TRONG_TOITHIEU_RUT_KHO -- lặp ở dòng 299
tinhnang\boss_hoangkim\deathhktieu.lua:9   KILLBOSSEXPAWARD     = 20000000 -> BHK_EXP_GIET_BOSS_TIEU   -- x GLB_TILE_EXP
tinhnang\boss_hoangkim\deathhktieu.lua:10  KILLBOSSNEAREXPAWARD = 10000000 -> BHK_EXP_LANCAN_BOSS_TIEU
tinhnang\boss_hoangkim\deathhkdai.lua:9    KILLBOSSEXPAWARD     = 25000000 -> BHK_EXP_GIET_BOSS_DAI
tinhnang\boss_hoangkim\deathhkdai.lua:10   KILLBOSSNEAREXPAWARD = 10000000 -> BHK_EXP_LANCAN_BOSS_DAI
tinhnang\boss_hoangkim\deathbosspld.lua:9  KILLBOSSEXPAWARD     = 50000000 -> BHK_EXP_GIET_BOSS_PLD
tinhnang\boss_hoangkim\deathbosspld.lua:10 KILLBOSSNEAREXPAWARD = 20000000 -> BHK_EXP_LANCAN_BOSS_PLD
tinhnang\boss_hoangkim\deathhktieu.lua:11  PHAMVI_HUONGEXP = 200 -> BHK_PHAMVI_HUONG_EXP -- khai lại y hệt ở deathhkdai:11, deathbosspld:11, deathhksv:13
tinhnang\boss_hoangkim\deathhksv.lua:87    OnDeath = 500000000 -> BHK_EXP_GIET_BOSS_MAYCHU_SOLO -- nhân biến VIP; ai VIP=0 được 0 exp
tinhnang\boss_hoangkim\deathhksv.lua:101   OnDeath = 200000000 -> BHK_EXP_LANCAN_BOSS_MAYCHU
tinhnang\boss_hoangkim\deathhksv.lua:113   OnDeath = 300000000 -> BHK_EXP_TODOI_BOSS_MAYCHU
tinhnang\boss_hoangkim\deathhksv.lua:116   OnDeath = 100000000 -> BHK_EXP_LANCAN_TODOI_BOSS_MAYCHU
tinhnang\boss_hoangkim\deathhksv.lua:100   OnDeath = 40/80 -> BHK_PHAMVI_X_BOSS_MAYCHU / BHK_PHAMVI_Y_BOSS_MAYCHU -- lặp ở dòng 115
global\seasonnpc.lua:122   Task_NewVersionAward = 5 -> DT_SL_RUONG_MOC_NGAY_2
global\seasonnpc.lua:837   Task_GiveAward = 3       -> DT_SL_BOSS_MOC10
global\seasonnpc.lua:1114  PayPlayerLinkAward = 8000 -> DT_MOC_CUOI_CUNG -- mốc "đền đáp bất ngờ"; phần thưởng gốc đã bị comment, hiện dừng lại không làm gì
global\seasonnpc.lua:768   Task_Info = 8000 -> DT_MOC_CUOI_CUNG -- con số chép lại trong lời thoại NPC
item\lbhtdatau.lua:30      main = 6 -> DT_SO_LOAI_NHIEMVU -- 6 loại nhiệm vụ Đả Tẩu hợp lệ
petsys\feed.lua:36         main = 2 -> BDH_DIEM_TRUONGTHANH_MOI_LAN_AN
task\partner\reward\partner_reward.lua:154  reward_killfinish = 0.1 -> BDH_HESO_EXP_THEO_LUOT -- mỗi lượt đã làm cộng 10% exp cho lượt sau (lặp ở reward2:152, reward3:149)
task\partner\reward\partner_reward.lua:214  reward_givetask = 5/10/20 -> BDH_BANG_MOC_TULUYEN -- mốc lên danh hiệu 77/78/79 và nới lượt/ngày
task\partner\reward\partner_reward2.lua:213 reward_givetask2 = 30 -> BDH_MOC_TULUYEN_CUOI -- danh hiệu 80 + nới lên 20 lượt/ngày
task\partner\reward\partner_reward.lua:15   TITLETIME = 1679616000 -> BDH_THOIHAN_DANHHIEU -- khai lại y hệt ở partner_reward2.lua:15
task\partner\task_award.lua:259  aryMasterAward[5] = 5000000 -> BDH_EXP_THUONG_CHUOI_CUOI -- kèm 1 triệu exp cho pet
tinhnang\loidaihonchien\bigiet.lua:11  OnPlayerDeath = 4 -> LDHC_SO_MANG_TINH_THUONG -- trần 4 mạng; bộ đếm bị xoá về 0 mỗi lần báo danh lại nên KHÔNG chặn được
```

---

## 2. KHÔNG ĐANG CHẠY — ĐỔ RA NHƯNG PHẢI GHI RÕ "ĐANG TẮT"

**2.1 Công Thành bản Việt** (`BAT_CONGTHANH_VIET=0` tại `cauhinh\ch_lich.lua:58` + NPC Công Thành Quan bị comment ở `lib_ctc.lua:231-232` và `startgame\thon\balanghuyen.lua:79`)
```
tinhnang\congthanhchien\lib_ctc.lua:98   TAB_TIME_CONG_THANH = {{20,30,4,20,30,5}} -> CTC_LICH_VIET
tinhnang\congthanhchien\lib_ctc.lua:82   ENOUGH_LEVEL   = 80      -> CTC_CAP_TOITHIEU_VAOTRAN
tinhnang\congthanhchien\lib_ctc.lua:87   CTC_MINUS_BD   = 10      -> CTC_PHUT_BAODANH_TRUOCTRAN
tinhnang\congthanhchien\lib_ctc.lua:88   CTC_MINUS_KT   = 60      -> CTC_DODAI_TRAN_PHUT_VIET
tinhnang\congthanhchien\lib_ctc.lua:73   MIN_TAX = 0              -> CTC_THUE_TOITHIEU_VIET
tinhnang\congthanhchien\lib_ctc.lua:74   MAX_TAX = 20             -> CTC_THUE_TOIDA_VIET
tinhnang\congthanhchien\lib_ctc.lua:83   TICH_LUY_THUONG_CT = 100 -> CTC_DIEM_THUONG_THEO_QUANHAM_VIET
tinhnang\congthanhchien\lib_ctc.lua:349  citywar_awardplayer = 1000 -> CTC_NGUONG_DIEM_NHAN_THUONG_VIET
tinhnang\congthanhchien\congthanhquan.lua:12  MAX_NUM_AWD = 200   -> CTC_MAX_HUYENTINH_BANG_VIET
tinhnang\congthanhchien\congthanhquan.lua:30  main = 5            -> CTC_THU_KHAICHIEN_VIET
```

**2.2 Lôi Đài Bang Hội bản Việt** (`BAT_LOIDAI_BANGHOI=0` tại `ch_lich.lua:64`; cả NPC báo danh lẫn NPC vào đấu trường đều bị comment)
```
tinhnang\loidai\lib_loidai.lua:17  MONEY_BD_LD    = 1000000 -> LDBH_PHI_BAODANH -- 1 triệu lượng, thu TRƯỚC mọi kiểm tra và KHÔNG hoàn tiền
tinhnang\loidai\lib_loidai.lua:15  TIME_LOI_DAIBH = {19,49,20,30,1} -> LDBH_LICH -- số cuối là THỨ HAI, trong khi menu NPC chỉ hiện thứ Năm
tinhnang\loidai\lib_loidai.lua:18  EXP_KT_LOIDAI_L = 1000000 -> LDBH_EXP_THUA
tinhnang\loidai\lib_loidai.lua:19  EXP_KT_LOIDAI_W = 2000000 -> LDBH_EXP_THANG
tinhnang\loidai\lib_loidai.lua:30  MAX_MEMBER_COUNT = 16     -> LDBH_NGUOI_MOI_PHE
tinhnang\loidai\vebinhdautruong.lua:75  OnJoin = 7200 -> LDBH_GIAY_TRONG_BANG_TOITHIEU -- lặp ở dòng 81
timerserver.lua:946  sukien_loidaibanghoi = 10 -> LDBH_PHUT_CHO_VAO
timerserver.lua:947  sukien_loidaibanghoi = 20 -> LDBH_PHUT_TRAN
tinhnang\congthanhchien\congthanhquan.lua:24  main = 4  -> LDBH_THU_BAODANH   -- thứ Năm
tinhnang\congthanhchien\congthanhquan.lua:24  main = 18 -> LDBH_GIO_BAODANH_TU
tinhnang\congthanhchien\congthanhquan.lua:24  main = 20 -> LDBH_GIO_BAODANH_DEN
tinhnang\congthanhchien\congthanhquan.lua:27  main = 30 -> LDBH_PHUT_KHAICHIEN
```

**2.3 Phong Lăng Độ bản Việt** (`BAT_PHONGLANGDO_VIET=0` `ch_lich.lua:62` + `startgame.lua:103` comment `addnpcphonglangdo()`, đã thay bằng bản Linux `HD3_PLD_*`)
```
tinhnang\phonglangdo\lib_phonglangdo.lua:88   TAB_TIME_PLD  = 12 khung, 2 tiếng/lần -> PLD_LICH_MO_HOATDONG
tinhnang\phonglangdo\lib_phonglangdo.lua:7    TIME_PLD_BD   = 9720   -> PLD_PHUT_BAODANH (9 phút)
tinhnang\phonglangdo\lib_phonglangdo.lua:8    MSTIME_PLD_KT = 32400  -> PLD_PHUT_TRAN (30 phút)
tinhnang\phonglangdo\lib_phonglangdo.lua:115  MAX_DI_PLD_NGAY = 2    -> PLD_SOLUOT_DI_THUYEN_NGAY
tinhnang\phonglangdo\lib_phonglangdo.lua:24   PLAYER_LIMIT_MS = 100  -> PLD_SO_NGUOI_TOI_DA_MOI_THUYEN
tinhnang\phonglangdo\lib_phonglangdo.lua:25   LIFE_THUYTAC      = 100000  -> PLD_MAU_THUYTAC
tinhnang\phonglangdo\lib_phonglangdo.lua:26   LIFE_BOSS_THUYTAC = 8000000 -> PLD_MAU_BOSS_THUYTAC
tinhnang\phonglangdo\lib_phonglangdo.lua:11   EXPQUAITHUONG  = 100000   -> PLD_EXP_GIET_THUYTAC
tinhnang\phonglangdo\lib_phonglangdo.lua:12   EXP_BOSS_TTDL  = 50000000 -> PLD_EXP_GIET_BOSS_THUYTAC
tinhnang\phonglangdo\lib_phonglangdo.lua:14   EXP_TRA_NVTT   = 50000000 -> PLD_EXP_TRA_NHIEMVU (x GLB_TILE_EXP = 1 tỷ)
tinhnang\phonglangdo\lib_phonglangdo.lua:10   MONEY_VETHANH  = 1000     -> PLD_TIEN_VE_THANH
tinhnang\phonglangdo\lib_phonglangdo.lua:13   TIMER_BOSS_TTDL = 16200   -> PLD_PHUT_BOSS_TON_TAI (15 phút)
tinhnang\phonglangdo\lib_phonglangdo.lua:21   REQ_MDTB = 200            -> PLD_SO_MATDO_THAY_LENHBAI
tinhnang\phonglangdo\lib_phonglangdo.lua:23   NUM_TT_NV = 188           -> PLD_SO_THUYTAC_CAN_GIET (đoạn kiểm tra đã bị comment)
tinhnang\phonglangdo\lib_phonglangdo.lua:114  TAB_TIME_TTDL_PLD = {20,25,30} -> PLD_PHUT_RA_BOSS (mốc 3 ra bản ĐẠI)
tinhnang\phonglangdo\lib_phonglangdo.lua:196  addthuytacdaulinhpld = 6  -> PLD_HESO_DAI_THUYTAC
tinhnang\phonglangdo\lib_phonglangdo.lua:190  addthuytacdaulinhpld = 500/1500 -> PLD_SATTHUONG_MIN_BOSS / PLD_SATTHUONG_MAX_BOSS
tinhnang\phonglangdo\lib_phonglangdo.lua:163  release_npcphonglangdo = 200/500 -> PLD_SATTHUONG_MIN_THUYTAC / PLD_SATTHUONG_MAX_THUYTAC
tinhnang\phonglangdo\lib_phonglangdo.lua:211  addthuytacdaulinhpld = 95 -> PLD_KHANG_BOSS_LUC_TAO
tinhnang\phonglangdo\bossthuytacdaulinhpld.lua:147  OnRevive = 75 -> PLD_KHANG_BOSS_KHI_HOISINH  -- lệch với dòng trên
tinhnang\phonglangdo\bossthuytacdaulinhpld.lua:10   NPCLIFE = 8000000 -> PLD_MAU_BOSS_KHI_HOISINH -- mất hệ số x6 của bản Đại
tinhnang\phonglangdo\quaipld.lua:63   OnRevive = 260 -> PLD_SATTHUONG_HE_THUYTAC
tinhnang\phonglangdo\drop.lua:35      DropRate = 95  -> PLD_NGUONG_ROI_TRANGBI_QUAI
tinhnang\phonglangdo\drop.lua:46      DropRate = 95  -> PLD_NGUONG_ROI_TRUYCONGLENH
tinhnang\phonglangdo\drop.lua:82      DropItemBossDaiTTDL = 6 -> PLD_MAX_LUOT_DROP_BOSS_DAI
tinhnang\phonglangdo\drop.lua:123     DropItemBossTTDL    = 3 -> PLD_MAX_LUOT_DROP_BOSS
```
(còn ~15 hằng PLD_ nhỏ: thưởng Xu/rương/Huyền Minh Đan khi hạ boss, tại `bossthuytacdaulinhpld.lua:56-88` — đổ cùng cụm nếu chủ định hồi sinh bản Việt)

**2.4 Lịch Boss Hoàng Kim** (`BAT_BOSS_HOANGKIM=0` `ch_lich.lua:60` — nhưng boss VẪN gọi tay được qua `item\lenhbaiadmin.lua:511-514`, nên mọi hằng máu/sát thương/drop ở mục 1 vẫn ăn thật)
```
tinhnang\boss_hoangkim\lib_bosshk.lua:52  TAB_TIME_BOSS_TIEU = {{15,0},{20,0}} -> BHK_LICH_BOSS_TIEU
tinhnang\boss_hoangkim\lib_bosshk.lua:61  TAB_TIME_BOSS_DAI  = {{11,0},{17,30}} -> BHK_LICH_BOSS_DAI
tinhnang\boss_hoangkim\lib_bosshk.lua:70  TAB_TIME_BOSS_SV   = {{12,0},{19,30}} -> BHK_LICH_BOSS_MAYCHU
```

**2.5 Lôi Đài Hỗn Chiến — phần chạy theo lịch** (`BAT_LOIDAI_HONCHIEN=0` `ch_lich.lua:70`; NPC báo danh thì vẫn sống)
```
timerserver.lua:469  LoiDaiHonChien = 52 -> LDHC_PHUT_KHAICHIEN
timerserver.lua:572  LoiDaiHonChien = 56 -> LDHC_PHUT_KETTHUC   -- trận thật chỉ dài 4 phút
timerserver.lua:497  LoiDaiHonChien = 11 -> LDHC_PHUT_CHOT_TU   -- cửa sổ chốt mở TRƯỚC khai chiến 40 phút
timerserver.lua:497  LoiDaiHonChien = 54 -> LDHC_PHUT_CHOT_DEN
timerserver.lua:511  LoiDaiHonChien = 1  -> LDHC_SO_NGUOI_CON_LAI_DE_THANG -- một người báo danh đơn là thắng
timerserver.lua:535  LoiDaiHonChien = 500000000 -> LDHC_EXP_QUANQUAN
timerserver.lua:623  LoiDaiHonChien = 500000000 -> LDHC_EXP_QUANQUAN -- nhánh chốt theo tổng dame
timerserver.lua:481  LoiDaiHonChien = 50000000  -> LDHC_EXP_THAMGIA -- chỉ cần đứng đó, không cần đánh
timerserver.lua:482  LoiDaiHonChien = 100 -> LDHC_SL_HOMACHDON
timerserver.lua:485  LoiDaiHonChien = 20  -> LDHC_DIEM_SUKIEN
timerserver.lua:536  LoiDaiHonChien = 5   -> LDHC_SL_MANH_HOANGKIM (bản 2 ở dòng 624, dải mã KHÁC)
timerserver.lua:542,545,549,554,559  LoiDaiHonChien = 30/50/70/90/95 -> LDHC_TL_TINHHONG_1 / LDHC_TL_THUYTINH / LDHC_TL_TINHHONG_2 / LDHC_TL_TIENTHAOLO / LDHC_TL_VOLAMMATTICH (lặp ở 630-647)
timerserver.lua:466  LoiDaiHonChien = 21/35 -> LDHC_GIO_LOA_1 / LDHC_PHUT_HET_LOA -- loa chạy giờ 21, lệch giờ thi đấu thật
```

**2.6 Kiếm Môn Quan** (`BAT_KIEMMONQUAN=0` `ch_lich.lua:66` — nhưng 4 NPC đã dựng sẵn, bật công tắc là chạy ngay)
```
event\kiemmonquan\lib_kmq.lua:18  TAB_TIME_KMQ = {{18,39},{19,00}} -> KMQ_LICH -- hai mốc cách nhau 21 phút
event\kiemmonquan\lib_kmq.lua:13  TIME_KMQKT = 60 -> KMQ_PHUT_TRAN
event\kiemmonquan\lib_kmq.lua:66  GoMap995VT = 3  -> KMQ_GIAY_BATTU
event\kiemmonquan\lib_kmq.lua:67  GoMap995VT = 3  -> KMQ_GIAY_TRANGTHAI_963
event\kiemmonquan\congtonthu.lua:50  muamaunhanh = 3000 -> KMQ_GIA_MUA_MAU
```

**2.7 Trống Bang Hội — phần sự kiện** (`BAT_TRONG_BANGHOI=0`; vật phẩm 2318 thì vẫn chạy, xem mục 1)
```
event\trongbanghoi\lib.lua:21  TIME_TRONGBH = {{19,45,19,40}} -> TBH_LICH
event\trongbanghoi\lib.lua:9   TIME_LIFE_TRONG  = 1200 -> TBH_GIAY_TRONG_SUKIEN
event\trongbanghoi\lib.lua:11  TIME_LIFE_CTT    = 40   -> TBH_PHUT_NPC_CONGTONTOAN
event\trongbanghoi\lib.lua:10  TIME_LIFE_TRONG1 = 30   -> TBH_PHUT_TRONG_BANG (trống bang chủ tự đánh ra — vẫn chạy)
event\trongbanghoi\tungtung.lua:35  main = 23        -> TBH_GIAY_DANH_TRONG_SUKIEN
event\trongbanghoi\tungtung.lua:70  FnDanhTrongOkay = 10000000 -> TBH_TIEN_THUONG
event\trongbanghoi\tungtung.lua:73  FnDanhTrongOkay = 30       -> TBH_SL_PHUCDUYEN
event\trongbanghoi\tungtung.lua:90  FnDanhTrongOkay = 500      -> TBH_TONGEXP_THUONG
event\trongbanghoi\congtontoan.lua:55  muamaunhanh = 3000 -> TBH_GIA_MUA_MAU
```

**2.8 Hoa Đăng / Trạng Nguyên** (`BAT_HOADANG=0` `ch_lich.lua:63`, VÀ mục lãnh thưởng ở `global\npcchucnang\lequan.lua:32-34` đã bị comment — bật công tắc thôi vẫn không lấy được thưởng)
```
event\event_cauhoi\lib.lua:28  TIME_CAU_HOI_TN = {{16,0}} -> HDANG_LICH
event\event_cauhoi\lib.lua:19  MIN_LEVEL_JOIN  = 80  -> HDANG_CAP_TOITHIEU
event\event_cauhoi\lib.lua:22  MAX_NUM         = 5   -> HDANG_SO_CAU_NGAY -- cauhoi.lua:28 dùng dấu '>' nên thực tế cho 6 câu
event\event_cauhoi\lib.lua:23  NUM_NHANTHUONG  = 3   -> HDANG_SO_CAU_DUNG_NHAN_THUONG
event\event_cauhoi\lib.lua:21  TIME_LIFE_QUESTION = 30 -> HDANG_PHUT_NPC_SONG
event\event_cauhoi\lib.lua:25  THUONG_EXP_HOADANG = 2000000 -> HDANG_EXP_THUONG -- x GLB_TILE_EXP = 40 triệu thật
event\event_cauhoi\lib.lua:362,364,366  NhanThuongCauHoi = 95/90/85 -> HDANG_TL_PHUCDUYEN / HDANG_TL_TIENTHAOLO / HDANG_TL_BACHQUALO
event\event_cauhoi\lib.lua:10  EXPCAUHOI = bảng 7 bậc theo cấp -> HDANG_BANG_THUONG_THEO_CAP -- nhánh trao thưởng đã comment ở cauhoi.lua:93-101
```

**2.9 Đả Tẩu hệ CŨ** (`tinhnang\datau\` + `global\npcchucnang\datau.lua`) — **MỒ CÔI, không NPC nào trỏ tới**. Chỉ đổ ra nếu chủ định hồi sinh hệ cũ:
`MAX_DATAU_DAY=40 (lib_datau.lua:16)`, `REQ_LEVEL_DATAU=80 (:17)`, `NUM_NV_LIENTIEP_DAY=40 (:18)`, `EXP_LIENTIEP_DAY=500000000 (:19)`, `MAX_SDLBHT=100 (:20)`, bảng mốc tích luỹ (`:82`), `random(1,1500)` rương hoàn trả (`:144`), `TAB_EXP`/`TAB_MONEY` 17 bậc may mắn (`quest_exp.lua:8`, `quest_money.lua:8`), tỉ lệ loại thưởng (`quest_random.lua:6`), ngưỡng danh vọng 50 (`quest_point.lua:29`), số lần huỷ 5 (`datau.lua:41`), SHXT 100 (`datau.lua:799`).

**2.10 Mã chết rải rác (khai mà KHÔNG AI ĐỌC — đổ ra sẽ tạo ảo giác chỉnh được)**
```
tinhnang\tong_kim_tcap\lib_tktc.lua:42  KILL_PLAYER_POINT = 30 -> (BỎ) -- điểm giết người thật gõ cứng trong kimtu/tongtu
tinhnang\tong_kim_tcap\lib_tktc.lua:110 QUANHAMTK cột 2 và cột 4 -> (BỎ) -- mốc thật nằm ở TAB_QUANHAM
tinhnang\tong_kim_tcap\lib_tktc.lua:441 RandPThucTongKim2 = 300 -> (BỎ) -- hàm không ai gọi, thể thức LUÔN là Cửu Sát
tinhnang\tong_kim_tcap\quanquan.lua:53  TAB_VSMT_REWARD -> (BỎ) -- dòng menu gọi nó đã bị comment
missions\citywar_city\head.lua:56,58,63  g_nMaxTscPerDoor / g_nMaxGccPerDoor / g_nMaxToolCount -> (BỎ) -- đoạn dùng trong mission.lua:144-151 đã comment
missions\citywar_city\head.lua:89  STONELEVEL2 = 20 -> (BỎ)
missions\citywar_city\head.lua:159 RA_KILL_PL_RANK  -> (BỎ)
missions\citywar_city\head.lua:180 BALANCE_MAMCOUNT = 15 -> (BỎ)
missions\citywar_city\head.lua:193 TITLE_BONUSRANK1..6 và TITLE_PL2RANK1..6_N -> (BỎ) -- mã thật chỉ dùng TAB_RANKBONUS
missions\citywar_arena\head.lua:32  LOSE_TONGEXP = 1400 -> (BỎ) -- SubTongExp ở :147 và :151 đã comment
missions\citywar_arena\head.lua:41,42  PKWINBONUS / LOSEBONUS -> (BỎ) -- bản chạy tính thắng thua bằng số người còn sống
missions\citywar_global\citywar_function.lua:5  CW_TITLE_TIME -> (BỎ) -- chỉ dùng trong getUsedLine(), hàm không ai gọi
tinhnang\loidai\lib_loidai.lua:32,33,37,38  WIN_TONGEXP/LOSE_TONGEXP/PKWINBONUS/LOSEBONUS -> (BỎ) -- bản sao chết, bản thật ở citywar_arena\head.lua
event\trongbanghoi\lib.lua:17  LIMIT_LEVEL_TBH = 80 -> (BỎ) -- cấp 80 thật gõ cứng ở tungtung1.lua:18 và drum.lua:23
petsys\common.lua:111,112  ZHENYUAN_TO_XIUZHEN_POINT_RATE / _VALUE -> (BỎ) -- xiuzhen.lua tự khai biến riêng
activitysys\config\32\variables.lua:10  TB_AWARD_TASK -> (BỎ) -- không nơi nào đọc
tinhnang\boss_hoangkim\bosslbdeath.lua:62,63  máu 500000 / exp 9999 -> (BỎ) -- không ai gọi tệp
global\drop\daihoangkim.lua:15 và global\drop\tieuhoangkim.lua:12 -> (BỎ) -- không một tham chiếu nào
tinhnang\boss_hoangkim\lib_bosshk.lua:125  QY_NORMALBOSS_INFO -> (BỎ) -- addnpcbosstieuhk2() không ai gọi
```

---

## 3. ĐÃ LOẠI KHỎI DANH SÁCH (theo quy tắc chọn)

| Hằng | Nơi | Lý do loại |
|---|---|---|
| `campnum = 89` | citywar_city\head.lua:402 | id danh hiệu, không phải nút chỉnh |
| `Title_AddTitle(..,0,9999999)` | citywar_city\head.lua:406 | số ma, tham số 0 mới quyết định vĩnh viễn → chuyển sang cảnh báo |
| `ITEM_TONG_AWD = {6,1,146,5}` | lib_ctc.lua:94 | mã vật phẩm, lại nằm nhánh chết |
| `PetSys.MapList` | petsys\head.lua:27 | danh sách id bản đồ (quy tắc cấm) — nhưng nó **là** whitelist nơi triệu hồi pet, chủ muốn thì đổ riêng |
| `SetTask(3008/3007)` | petsys\head.lua:66 | không phải hằng số, chỉ là mốc so sánh ngày |
| `BOSSDAI_EXP=0` / `BOSSTIEU_EXP=0` | lib_bosshk.lua:14,15 | bằng 0, toàn bộ exp phát ở tệp death* |
| `TKT_HESO_MANH_NGUYENSOAI` | lib_server.lua:25 | **trùng dòng với nhóm 1** → đã là `GLB_MANH_NGUYENSOAI_TK` |
| `CTC_THU_BAODANH_VIET` | congthanhquan.lua:24 | **trùng dòng nhóm 2/nhóm 6** → giữ `LDBH_THU_BAODANH` |
| `CTC_LICH_TUAN_BANSAO_NPC` | citywar_function.lua:6 | gộp về `CTC_LICH_TUAN` để hai bản không trôi lệch |

---

## 4. CẢNH BÁO GỘP — ĐỌC TRƯỚC KHI GÕ MỘT DÒNG NÀO

### 4.1 Của tôi (đo trực tiếp hôm nay, mới nhất)

1. **`script\lib\lib_server.lua` đã bị đổi hôm nay** — 27 dòng, dùng `GLB_CFG`, 11 khoá khai ở `cauhinh\ch_chung.lua:29-39`. Toàn bộ nhóm 1 là công việc đã xong.
2. **Có phiên khác đang GHI ĐỒNG THỜI vào `script\cauhinh\`** — `ch_lich.lua` tăng từ 78 lên 87 dòng giữa hai lần đọc của tôi, cách nhau chưa tới hai phút. Danh sách "đã có" ở mục 0 chốt tại thời điểm đo; **trước khi khai khoá mới bắt buộc grep lại** `cauhinh\*.lua`.
3. **Số dòng của mọi tệp đã nối dây đã DỊCH** (do chèn khối `Include` + hàm `*_CFG` ở đầu tệp). Đã đo được: `lib_tktc.lua` **+2**, `quanquan.lua` **+12**, `trinhsat.lua` **+12**, `citywar_city\head.lua` **+12**, `citywar_arena\head.lua` **+12**, `hd3_driver.lua` **+14**. Số dòng nhóm 2/3 báo cáo là bản CŨ — tôi đã sửa lại trong danh sách trên cho các tệp đo được; các tệp còn lại **phải tìm theo TÊN BIẾN, đừng nhảy theo số dòng**.
4. **3 khoá `TKT_` được ĐỌC nhưng CHƯA KHAI** trong `cauhinh`: `TKT_PHI_TRINHSAT` (`trinhsat.lua:21`), `TKT_GIA_DOI_NHACVUONGKIEM` (`quanquan.lua:24`), `TKT_SL_HONTHACH_DOI_KIEM` (`quanquan.lua:27`). Chủ sửa số sẽ không thấy khoá đâu — phải bổ sung vào bảng.

### 4.2 Trùng tên biến toàn cục — nguy hiểm nhất khi nắn

- `MAX_CAMP1..4COUNT`: `citywar_city\head.lua:77-80` (200/200/50/50, đã nối dây) vs `lib_ctc.lua:90-93` (50/50/5/5). Ai Include sau đè trước.
- `CITYWAR_TOP10_EXP`: `citywar_city\head.lua:40` (đã nối) vs `lib_ctc.lua:89`.
- `KILL_PLAYER_POINT`: `lib_ctc.lua:95` (=30) vs `lib_tktc.lua:42` (=30). `timerserver.lua` Include lib_ctc (dòng 11) TRƯỚC lib_tktc (dòng 12).
- `TAB_QUANHAM`: `lib_ctc.lua:158` một cột vs `lib_tktc.lua:123` hai cột; `congtu.lua:36`/`thutu.lua:35`/`kimtu.lua:91` đọc `[i][2]` → bản lib_ctc thắng thế là đọc phải nil.
- `MAX_MEMBER_COUNT`: `citywar_arena\head.lua:26`(16) / `header\loidai.lua:11`(100) / `missions\bw\bwhead.lua:22`(100) / `missions\clearskill\head.lua:52`(20) / `lib_loidai.lua:30`(16).
- `TIMER_1 / TIMER_2 / GO_TIME`: `citywar_arena\head.lua:44-46` vs `lib_loidai.lua:39-41` vs `battles\battlehead.lua:24-27` vs `missions\tongwar\head.lua:5-7`.
- `JOINSTATE`: `lib_loidai.lua` (=242) vs `missions\clearskill\head.lua:38` (=100).
- `KILLBOSSEXPAWARD / KILLBOSSNEAREXPAWARD / PHAMVI_HUONGEXP`: khai ở **bốn** tệp khác giá trị — `deathhktieu.lua:9-11` (20tr/10tr/200), `deathhkdai.lua:9-11` (25tr/10tr/200), `deathbosspld.lua:9-11` (50tr/20tr/200), `deathhksv.lua:11-13` (20tr/5tr/200). **Bắt buộc 4 khoá riêng**, không được gộp.
- `nStartTime` / `nCloseTime` (`event\trongbanghoi\lib.lua:12-13`) — tên toàn cục cực kỳ chung, phải đổi khi đổ ra.
- `SERVER_OPEN` còn một bản cũ ở `lib\worldlibrary.lua:4` (=2104011900) chưa nối dây. Chạy nhưng không bao giờ thắng (lib_server nạp sau). Nên xoá hẳn kẻo sau này sửa một nơi.

### 4.3 Lỗ hổng / lỗi logic thật, không phải hằng số

- **`GLB_CHE_DO_TEST = 1` đang MỞ**: `startgame.lua:69` sinh hai NPC "Hỗ Trợ Test" phát đồ GM cho **mọi người chơi**; ba nơi khác (`nhieptran.lua:43`, `sugiasatthu.lua:43`, `thuyenphu.lua:21`) cho người thường dùng quyền GM. Đặt 0 trước khi mở cửa thật.
- **`LIFE_BOSS_DAIH = 1`** (`lib_bosshk.lua:25`, ghi chú nói mặc định 18.000.000): boss đại + boss máy chủ + boss PLD chết trong một nhát. Bật boss lên mà không sửa là hỏng ngay.
- **`EXP_LIENTIEP_DAY` Đả Tẩu ăn hệ số kép**: `quest_exp.lua` viết `*EXP_RATE*2` → hệ số thật x40; riêng mốc 17 (`:25`) là `*EXP_RATE*2*2` = x80.
- **Trống bang hội**: `tungtung1.lua:58` cộng exp trong `for i=1,10` → một nhịp 5 giây ăn 10-20 triệu; kiểm trần (`lib.lua:54`) chỉ làm TRƯỚC vòng lặp nên vượt trần thêm một nhịp.
- **Lôi Đài Hỗn Chiến, 5 điểm hở exp**: (a) `timerserver.lua:511` `count == 1` → một người báo danh đơn ăn 500 triệu, 2 lần/ngày; (b) `bigiet.lua:11` trần 4 mạng bị `mainloidai.lua:45 SetTaskTemp(1,0)` xoá mỗi lần báo danh lại; (c) `bigiet.lua:6-8` khi `NpcIdx2PIdx` trả 0 (chết không do người chơi) thì exp rơi vào chính **người vừa chết**; (d) "tổng dame" là số giả — không nơi nào `SetTaskTemp(50)`, nên quán quân nhánh phút 56 luôn là người đầu bảng ngẫu nhiên; (e) `timerserver.lua:598-600` thiếu `return` khi không ai tham gia.
- **Cửa sổ chốt Lôi Đài Hỗn Chiến mở trước khai chiến 40 phút** (`timerserver.lua:497` phút 12 vs `:469` phút 52). Hiện chưa nổ vì `FightState` chỉ do `SetFightState(1)` hoặc `BackToTownPortal` bật; thêm bất kỳ đường nào bật cờ đó là có quán quân lúc 16:12.
- **`seasonnpc.lua:115`** đặt ngoặc sai: `if (nNum == 40 and nCancelNum) == 0 then`. Đang vô tình gần đúng, nhưng sửa số là vỡ — viết lại khi nắn.
- **`phonglangdo\drop.lua:51` và `:54`** là hai lệnh `if` rời nhau cùng rơi Ngũ Hoa → bốc trúng 81..84 là rơi **hai** món.
- **`deathhksv.lua:182`** `SetNpcResist(...,95,120,95,120,95)` — hai hệ vượt mốc 100%.
- **Boss Thuỷ Tặc Đầu Lĩnh đặt máu/kháng hai lần lệch nhau**: tạo ra 8.000.000 ×6 + kháng 95 (`lib_phonglangdo.lua:208-211`), hồi sinh ép về 8.000.000 cố định + kháng 75 (`bossthuytacdaulinhpld.lua:144,147`).
- **`congthanhquan.lua:24`** đặt ngoặc sai `hour >= 18 and (hour <= 20 and nminus < 30)` → mục báo danh **biến mất** trong các phút 30-59 của mọi giờ.
- **`vebinhdautruong.lua:64`** `function GetJoinTongTime() return 7201 end` — luật "phải ở trong bang 2 tiếng" bị vô hiệu hoàn toàn.
- **`citywar_function.lua:194`** dùng biến `count` chưa khai trong `take_tong_resaward` → nổ khi bang chủ lấy phần thưởng còn dư.
- **`event_cauhoi\lib.lua:346`** `nRand = 3` đè kết quả ngẫu nhiên vừa tính ở 338-345 → bản đồ luôn là Hoa Sơn.
- **`event_cauhoi\lib.lua:361-367`** dùng `>95 and <100`, `>90 and <95`, `>85 and <90` → các giá trị 100, 95, 90, 85 rơi vào khe hở, không ra gì.
- **`lib_kmq.lua:100`** báo "12h và 20h" trong khi lịch thật là 18:39 và 19:00.
- Chú thích sai so với mã: `citywar_arena\head.lua:45` ghi "1Hour" nhưng 25 phút; `:46` ghi "10" nhưng 480 giây; `arena\manager.lua:39` và `camper.lua:44` hứa "1000 vạn lượng / 12.000-14.000 điểm exp bang" trong khi mã chỉ `AddTongExp(1200)`; `timerserver.lua:495` báo "thi đấu tối đa 20 phút" nhưng trận dài 4 phút; `timerserver.lua:467` in `(10 - nMi)` nên từ phút 11 báo số âm; `lib_tktc.lua` chú thích chênh lệch quân số ghi 5 trong khi giá trị 20.
- **Điểm thưởng thua > hoà**: `lib_tktc.lua:733` (10 triệu) vs `:697` (5 triệu), hệ số thua x1 vs hoà x2.
- **`NEEDMONEY = 20000` chỉ còn để hiện chữ**: mọi `Pay(NEEDMONEY)` trong `mobinhtk.lua` (dòng 160,171-174,181,273,284-287,298) đã bị comment → **thực tế không trừ tiền báo danh**.

### 4.4 Hằng số nằm NGOÀI Lua — nắn Lua không ăn

- `MAX_MAGIC_LEVEL`: số thật là `int MAX_MAGIC_LEVEL = 6;` tại `D:\GAMEDEVNEW\Sources\Core\Src\KNpc.cpp:8716`, chặn ở `:8810-8812`. Bản Lua = 10 không ai đọc.
- Trần thuế + khung giờ đặt thuế Công Thành: `Sources\Core\Src\KJx2CityWar.cpp:147-151`, mục `[CitySettings]` của tệp INI (`SignUpFee, MinTongLevel, MaxExchangeTax, StartSetTaxTime, EndSetTaxTime, WarCycleValue, ...`).
- Buff sinh lực theo quân hàm Tống Kim: do `SetRankBattle` (C++/settings) quyết định; các tỉ lệ 30%-70% trong `QUANHAMTK` cột 3 chỉ là chữ hiển thị.
- Đả Tẩu mới: mốc thưởng theo số nhiệm vụ nằm ở `settings\task\award_basic.txt`, `award_link.txt`, `award_loop.txt`, `tasklink_*.txt`, `levellink.txt` (cột `Num/TaskValue/Count/LockType/ExpDay/GiveAll/Cells`).
- Bạn Đồng Hành: tỉ lệ thành công nâng cấp + số thuốc ở `settings\petsys\levelup.txt` (`nItemCostCount`, `nSuccessRate`) và `settings\petsys\feature.txt`.
- Bảng xếp hạng đấu giá khiêu chiến lệnh dùng id 10261 khai **hai nơi**: `timerserver_ctc.lua:11` (`CTC6_LADDER`) và `missions\citywar_global\ladder.lua:9`.