# KHẢO SÁT LỊCH + THƯỞNG TOÀN GAME (29/08/2026)

> Tài liệu này là **kết quả khảo sát**, chưa phải bản đã thi công.
> Đợt 29/08 mới nối dây phần **20 công tắc bật/tắt hoạt động**
> (`script\cauhinh\ch_lich.lua`). Phần còn lại là danh sách việc cho đợt sau.
>
> Sáu nhóm quét song song: trận chiến lớn · bang hội · boss/rơi đồ ·
> sự kiện theo thời gian · nhiệm vụ hằng ngày · settings + C++.

---

# ĐỀ XUẤT BẢNG KHÓA CẤU HÌNH — TỔNG HỢP 6 NHÓM QUÉT

Đã đối chiếu với `header\cauhinh_hoatdong.lua` (533 dòng, **120 khóa** đang có: `TW_ TS_ BR_ BW_ TC_ HD3_ HCD_ QDHK_ YDBZ_`). Mọi tiền tố đề xuất dưới đây **không trùng** 120 khóa đó. Giá trị mặc định = **đúng giá trị đang chạy**, không làm tròn, không "cải thiện".

---

## 0. GỘP TRÙNG LẶP — 14 chỗ nhiều nhóm cùng báo (đã hợp nhất)

| # | Thứ bị báo trùng | Nhóm báo | Xử lý |
|---|---|---|---|
| 1 | Lôi Đài Bang Hội bản Việt | tongkim-plandanho, congthanh-banghoi | **1 tiền tố `LDBH_`** |
| 2 | Lôi Đài đấu trường công thành (`citywar_arena`) | nhóm 1 gọi `LDCT_`, nhóm 2 gọi `CTLD_` | **chốt `CTLD_`** (giữ họ với `CTC_`) |
| 3 | `WIN_TONGEXP`/`LOSE_TONGEXP` + 20 hằng khác | nhóm 1, nhóm 2 | **1 khóa duy nhất**, sửa CẢ `lib_loidai.lua` LẪN `citywar_arena\head.lua` |
| 4 | `CITYWAR_TOP10_EXP = 3000000` | nhóm 2 (2 lần) | **1 khóa `CTC_EXP_TOP10`**, sửa 2 tệp |
| 5 | `KILL_PLAYER_POINT = 30` | nhóm 1, nhóm 2 | **1 khóa**, sửa `lib_tktc.lua:28` + `lib_ctc.lua:95` |
| 6 | `MONEYNHANNV`/`MONEY_GO_BOSS`/`MAX_STACK_NUM`/`MAX_NVSATTHU` | nhóm 1, nhóm 3, nhóm 5 | **1 bộ khóa**, hợp nhất 2 tệp lệch số |
| 7 | Kiếm Môn Quan | congthanh-banghoi, sukien-thoigian | **1 tiền tố `KMQ_`** |
| 8 | Trống Bang Hội | congthanh-banghoi, sukien-thoigian | **1 tiền tố `TBH_`** |
| 9 | Vượt Ải bản Việt cũ | nhóm 1 gọi `VAV_`, nhóm 5 gọi `VAC_` | **chốt `VAV_`** |
| 10 | Boss Sát Thủ | nhóm 3 (`BST_` bản Việt), nhóm 5 (`HD3_ST_` bản Linux) | **KHÔNG gộp** — 2 hệ khác nhau |
| 11 | Lôi Đài Hỗn Chiến | nhóm 1, nhóm 4 (ghi_chú) | **1 tiền tố `LDHC_`** (nhóm 4 bổ sung nhánh 22h `:456-486`) |
| 12 | Công Thành giờ đặt thuế 22-23h | nhóm 2 (Lua), nhóm 6 (ini) | **1 khóa, đánh `[ENGINE]`** — Lua chỉ là text |
| 13 | `EXP_RATE`/`STRONGBOSS_*` | nhóm 1 (`GLB_`), nhóm 3 (mục F) | **1 bộ `GLB_`** |
| 14 | Chuyển sinh | nhóm 4 (`CSDS_` event), nhóm 5 (`CS_` metempsychosis) | **KHÔNG gộp** — 2 hệ khác nhau, xem cảnh báo 🔴C4 |

---

# TỆP 1 — `ch_lich.lua` (THỜI GIAN)

## ⭐ Ưu tiên 1 — 17 CÔNG TẮC bật/tắt hoạt động (hiện phải sửa mã nguồn)

Toàn bộ nằm cùng một chỗ: hàm `RunTime()` của `script\timerserver.lua`. Đây là khóa chủ game động vào nhiều nhất.

```
BAT_TONGKIM          = 1   -- Tống Kim (ĐANG BẬT - hoạt động sống)              [timerserver.lua:80]
BAT_HD3              = 1   -- cụm 3 hoạt động Linux: Sát Thủ/PLD/Vượt Ải        [timerserver.lua:82]
BAT_VIEMDE           = 1   -- = YDBZ_BAT (đã có khóa, giữ nguyên)               [timerserver.lua:83]
BAT_CTC_JX2          = 1   -- Công Thành + Lôi Đài JX2                          [timerserver.lua:85-87]
BAT_LIENDAU          = 0   -- sukien_liendau — ĐANG COMMENT                     [timerserver.lua:79]
BAT_CONGTHANH_VIET   = 0   -- sukien_congthanh (bản Việt cũ) — COMMENT          [timerserver.lua:88]
BAT_HATHUYHOANG      = 0   -- sukien_hathuyhoang — COMMENT                      [timerserver.lua:89]
BAT_BOSS_HOANGKIM    = 0   -- sukien_bosshk (tiểu+đại+máy chủ) — COMMENT        [timerserver.lua:90]
BAT_VUOTAI_VIET      = 0   -- sukien_vuotai — COMMENT                           [timerserver.lua:91]
BAT_PHONGLANGDO_VIET = 0   -- sukien_phonglangdo — COMMENT                      [timerserver.lua:92]
BAT_HOADANG          = 0   -- sukien_trangnguyen (Hoa Đăng) — COMMENT           [timerserver.lua:93]
BAT_LOIDAI_BANGHOI   = 0   -- sukien_loidaibanghoi — COMMENT                    [timerserver.lua:94]
BAT_TRONG_BANGHOI    = 0   -- sukien_trongbanghoi — COMMENT                     [timerserver.lua:95]
BAT_KIEMMONQUAN      = 0   -- sukien_kiemmonquan — COMMENT                      [timerserver.lua:96]
BAT_VANTIEU_LOA      = 0   -- sukien_vantieu (CHỈ loa; NPC vẫn chạy) — COMMENT  [timerserver.lua:97]
BAT_DUATOP           = 0   -- XepHangDuaTop — COMMENT                           [timerserver.lua:98]
BAT_SONGBAC          = 0   -- songbac — COMMENT                                 [timerserver.lua:99]
BAT_LOIDAI_HONCHIEN  = 0   -- LoiDaiHonChien — COMMENT (🔴 xem cảnh báo A1)     [timerserver.lua:100]
BAT_CHECK_KICK       = 0   -- check_and_kick — COMMENT                          [timerserver.lua:101]
BAT_PUBG             = 0   -- pubg_runner (Sinh Tồn) — COMMENT 2 chỗ            [timerserver.lua:102, timerserver.lua:107]
```
> Cách thi công: thay 20 dòng bằng `if (HD_CFG("BAT_X",0) == 1) then ... end`. **Riêng 4 dòng đang BẬT phải giữ mặc định 1.**

## ⭐ Ưu tiên 2 — TỐNG KIM (hoạt động sống duy nhất trong nhóm 1)

```
TK_GIO_BAODANH       = {{23,46,19,1},{17,50,18,3},{20,50,21,4},{22,50,23,5}}
                            -- 4 khung mở báo danh {giờ,phút,<cột 3 CHẾT>,số hiệu trận}   [tinhnang\tong_kim_tcap\lib_tktc.lua:70-82, timerserver.lua:711-719]
TK_PHUT_BAODANH      = 1    -- TIME_BD_TK, phút báo danh (chú thích ghi 10 - SAI)          [lib_tktc.lua:56, timerserver.lua:714]
TK_PHUT_TRAN         = 30   -- TIME_KT_TK, độ dài toàn trận (chú thích ghi 70 - SAI)       [lib_tktc.lua:57, timerserver.lua:718]
TK_PHUT_RA_NGUYENSOAI= 1    -- TIME_NS_TK (chú thích ghi 30 - SAI; nhánh này KHÔNG chạy)   [lib_tktc.lua:58, timerserver.lua:716]
TK_GIAY_HAUDOANH     = 90   -- TIME_IN_TRAI, giây tối đa ở hậu doanh                       [lib_tktc.lua:65, maps\tongkim\trap\tongvaotrai.lua:38, kimvaotrai.lua:37, tong_kim_tcap\kimtu.lua:22]
TK_GIAY_DELAY_RATRAI = 10   -- TIME_DELAY_RA_TRAI                                          [lib_tktc.lua:66, maps\tongkim\trap\tongratrai.lua:21, kimratrai.lua:21]
TK_PHUT_GIAMTOC_GIUCO= 60   -- người giữ cờ bị giảm tốc (mã ghi 60*60*18)                  [lib_tktc.lua:682, lib_tktc.lua:64]
TK_GIO_VONGSANG_TOP  = 24   -- hạn vòng sáng 1552/1553/1554 hạng 1/2/3 (60*60*24*18)       [timertask\task03.lua:98, task03.lua:109, task03.lua:120]
```

## ⭐ Ưu tiên 3 — CÔNG THÀNH + LÔI ĐÀI JX2 (đang chạy thật)

```
CTC_LICH_TUAN        = {{3,4},{1,2},{2,3},{5,6},{4,5},{0,1},{6,0}}
                            -- {thứ báo danh, thứ đánh} 7 thành, 0=CN. 🔴 CHÉP LÀM HAI     [timerserver_ctc.lua:10, missions\citywar_global\citywar_function.lua:6-14]
CTC_GIO_BAODANH      = 18   -- giờ mở báo danh. 🔴 VIẾT CỨNG 6 NƠI                         [timerserver_ctc.lua:201, missions\citywar_global\infocenter_head.lua:147, infocenter_head.lua:428, infocenter_head.lua:671, infocenter_head.lua:722]
CTC_GIO_CHOT         = 19   -- chốt báo danh + bốc thăm bảng đấu                           [timerserver_ctc.lua:212, infocenter_head.lua:147, infocenter_head.lua:428, infocenter_head.lua:531, infocenter_head.lua:671, infocenter_head.lua:722]
CTC_GIO_KHAICHIEN    = 20   -- StartArena (ngày báo danh) / StartCityWar (ngày đánh)       [timerserver_ctc.lua:225]
CTC_GIO_DONDEP_TU    = 0    -- cửa sổ dọn dữ liệu ngày                                     [timerserver_ctc.lua:196]
CTC_GIO_DONDEP_DEN   = 6    --                                                             [timerserver_ctc.lua:196]
CTC_PHUT_TRAN        = 90   -- GAMETIME công thành (mã ghi 90*60*18)                       [missions\citywar_city\head.lua:27]
CTC_GIAY_LOA         = 60   -- REPORTTIME chu kỳ loa trong trận                            [missions\citywar_city\head.lua:26]
CTC_PHUT_POLL        = 5    -- INTERVAL của GlbMission 8                                   [missions\citywar_global\mission.lua:2]
CTC_NGAY_DANHHIEU    = 7    -- CW_TITLE_TIME hạn danh hiệu                                 [missions\citywar_global\citywar_function.lua:5]
CTC_GIO_MOC_DANHHIEU = 20   -- mốc tính hạn danh hiệu (hardcode -20)                       [citywar_function.lua:63]
CTC_THU_LANHTHUONG   = 1    -- thứ Hai mới lãnh thưởng chiếm thành                         [citywar_function.lua:97, tinhnang\congthanhchien\congthanhquan.lua:190]
CTC_GIO_LANHTHUONG_TU  = 9  --                                                             [citywar_function.lua:97]
CTC_GIO_LANHTHUONG_DEN = 20 --                                                             [citywar_function.lua:97]
CTC_GIO_HETNHAN_THANHVIEN = 18  -- thành viên nhận huyền tinh trước 18h                    [citywar_function.lua:147, congthanhquan.lua:237]
CTC_GIO_BANGCHU_TU   = 18   -- bang chủ nhận phần còn lại                                  [citywar_function.lua:172, congthanhquan.lua:262]
CTC_GIO_BANGCHU_DEN  = 20   --                                                             [citywar_function.lua:172]
CTC_GIO_TRALAI_LENH  = 19   -- bang thua nhận lại Khiêu Chiến Lệnh từ 19h                  [infocenter_head.lua:531]
CTC_GIO_DATTHUE_TU   = 22   -- [ENGINE] gate thật ở C++, Lua chỉ là text                   [settings\citywar.ini:84, missions\citywar_global\citybulletin.lua:13]
CTC_GIO_DATTHUE_DEN  = 23   -- [ENGINE]                                                    [settings\citywar.ini:85]
CTC_CHUKY_NGAY       = 7    -- [ENGINE] WarCycleValue                                      [settings\citywar.ini:88]

CTLD_PHUT_TRAN       = 25   -- TIMER_2 độ dài trận Lôi Đài. 🔴 KHAI 2 NƠI                  [missions\citywar_arena\head.lua:33, tinhnang\loidai\lib_loidai.lua:40]
CTLD_GIAY_NHIP       = 20   -- TIMER_1 nhịp báo tin. 🔴 KHAI 2 NƠI                         [citywar_arena\head.lua:32, lib_loidai.lua:39]
CTLD_PHUT_VAODAI     = 8    -- GO_TIME=480*18/TIMER_1=24 nhịp. 🔴 KHAI 2 NƠI               [citywar_arena\head.lua:34, lib_loidai.lua:41]
```

## Ưu tiên 4 — LÔI ĐÀI HỖN CHIẾN (đang có lỗ exp, vá trước khi bật)

```
LDHC_GIO             = {16,22}  -- giờ báo danh, hardcode trong `if`                       [tinhnang\loidaihonchien\mainloidai.lua:30]
LDHC_PHUT_MO         = 0        -- phút >= 0                                               [mainloidai.lua:30]
LDHC_PHUT_DONG       = 59       -- phút < 59                                               [mainloidai.lua:30]
LDHC_GIO_NHACNHO     = {21,22}  -- 🔴 LỆCH với LDHC_GIO                                    [timerserver.lua:299]
LDHC_PHUT_NHAC       = 35       -- nMi < 35                                                [timerserver.lua:299]
LDHC_PHUT_KHAICHIEN  = 52       -- ép vào trạng thái chiến + phát thưởng tham gia          [timerserver.lua:302]
LDHC_CUASO_CHOT_TU   = 12       -- 🔴 BẮT ĐẦU TRƯỚC khai chiến - lỗi logic (cảnh báo A2)   [timerserver.lua:330]
LDHC_CUASO_CHOT_DEN  = 54       --                                                         [timerserver.lua:330]
LDHC_PHUT_DONTRAN    = 56       -- dọn trận, ghi log, trả người về                         [timerserver.lua:405]
```

## Ưu tiên 5 — Nhiệm vụ / lượt hằng ngày (chủ game hay chỉnh)

```
BU_GIO_LAMMOI        = 0    -- giờ làm mới lượt toàn server (UpdateNgayMoiAllPlayer)       [timerserver.lua:48-52, lib\lib_ham.lua:222-231, player\playerlogin.lua:23]
BU_SO_NGAY_TOIDA     = 7    -- MAX_BU_DAYS, số ngày được bù lượt                           [lib\lib_ham.lua:322]

DT_LUOT_NGAY         = 40   -- Dã Tẩu, số nhiệm vụ/ngày. 🔴 BA NGUỒN                       [global\seasonnpc.lua:51, seasonnpc.lua:279, lib\lib_ham.lua:303]
DT_SO_LAN_HUY        = 3    -- huỷ tới lần thứ 3 thì bị phạt (mã viết `> 2`)               [global\seasonnpc.lua:704]
DT_TICK_PHAT         = 10890 -- thời gian phạt sau khi huỷ (tick 18/giây = 605 giây)       [global\seasonnpc.lua:254-256]
DT_NV_MOI_CHUOI      = 20   -- TL_MAXTIMES                                                 [task\newtask\tasklink\tasklink_head.lua:13]
DT_CHUOI_MOI_VONG    = 20   -- TL_MAXLINKS                                                 [tasklink_head.lua:14]
DT_VONG_TOI_DA       = 20   -- TL_MAXLOOPS                                                 [tasklink_head.lua:15]
DT_VULAN_NGAY_TU     = 60808 -- cửa sổ thoại Vu Lan/Liên Hoa (YYMMDD)                      [global\seasonnpc.lua:139]
DT_VULAN_NGAY_DEN    = 60815 --                                                            [global\seasonnpc.lua:139]

BDH_TL_LUOT_NGAY_B1  = 5    -- BĐH tu luyện bậc 1. 🔴 SỐ 5 LẶP 3 CHỖ + task 1246           [task\partner\reward\partner_reward.lua:34, partner_reward.lua:41, partner_reward.lua:208]
BDH_TL_LUOT_NGAY_B2  = 10   -- bậc 2. 🔴 LẶP 2 CHỖ                                         [task\partner\reward\partner_reward2.lua:39, partner_reward2.lua:207]
BDH_TL_LUOT_NGAY_B3  = 20   -- bậc 3                                                       [task\partner\reward\partner_reward3.lua:37]
BDH_TL_MOC_VONG      = {5,10,20,30}  -- mốc vòng thăng danh hiệu 77/78/79/80               [partner_reward.lua:214, partner_reward.lua:221, partner_reward.lua:228, partner_reward2.lua:213]
BDH_HT_LUOT_NGAY     = 3    -- lượt hồi tưởng/ngày mỗi chuỗi                                [task\partner\task_head.lua:186]
BDH_SM_LUOT_NGAY     = 1    -- chuỗi tái luyện sư môn/ngày                                 [task\partner\master\partner_mastertask.lua:68, partner_mastertask.lua:104-115]
BDH_KH_LUOT_NGAY     = 1    -- Kiếm Hoàng phát bạn đồng hành/ngày. 🔴 CHÉP TAY ~20 KHỐI    [task\partner\education\swordking_people.lua:240-254]
BDH_SM_TICK_CHO      = 350  -- tick chờ giữa 2 bước chuỗi 1                                [task\partner\master\partner_master_main_01.lua:613]
BDH_TD_CHUKY_GIO     = 2    -- Tế Đàn thức tỉnh mỗi 2 giờ, phút 00                         [task\partner\train\bdh_jitan_driver.lua:10-15, timerserver.lua:44]
BDH_TD_SO_DAN        = 5    -- số tế đàn nạp lại mỗi lượt                                  [bdh_jitan_driver.lua:17-19]

HD3_VA_CHUKY_GIAY    = 3600 -- INTERVAL_MATCH (chưa có khóa)                               [missions\challengeoftime\include.lua:12]
HD3_VA_NHIP_BANG     = 1    -- INTERVAL_BOARD                                              [missions\challengeoftime\include.lua:11]
HD3_VA_GIO_NHAN_XEPHANG_TU  = 4    -- HHMM 0004                                            [missions\challengeoftime\rank_perday.lua:19]
HD3_VA_GIO_NHAN_XEPHANG_DEN = 2300 --                                                      [rank_perday.lua:19]
VA30_GIO_MO          = 10   -- mật phòng cửa ải 30                                         [missions\challengeoftime\include.lua:30]
VA30_GIO_DONG        = 22   --                                                             [include.lua:31]
VA30_GIAY_GIOIHAN    = 780  -- phải vượt 9 ải trong 13*60 giây                             [include.lua:29]
VA30_GIAY_DONG_THEM  = 1440 -- =780+600+60                                                 [missions\challengeoftime\chuangguang30.lua:263]

CS_NGAY_GIANCACH     = {0,0,0,0,0,0}  -- giãn cách giữa 2 lần chuyển sinh (=0: không chặn) [task\metempsychosis\task_head.lua:53, task_func.lua:141-142]
```

## Ưu tiên 6 — Hoạt động ĐANG TẮT (gom sẵn để bật lại nhanh)

```
KMQ_GIO              = {{18,39},{19,0}}  -- TAB_TIME_KMQ ({02,51} đã comment)              [event\kiemmonquan\lib_kmq.lua:17-21, timerserver.lua:639-658]
KMQ_PHUT_TRAN        = 60   -- TIME_KMQKT                                                  [lib_kmq.lua:13, timerserver.lua:650]
KMQ_GIAY_BAOHO       = 3    -- SetProtectTime(18*3) + AddSkillState(963,...,18*3)          [lib_kmq.lua:66-67]

TBH_GIO_SINH         = 1945 -- Trống Bang Hội, HHMM                                        [event\trongbanghoi\lib.lua:20-22, timerserver.lua:964-976]
TBH_GIO_BAOTRUOC     = 1940 -- loa + đặt NPC Công Tôn Toản                                 [event\trongbanghoi\lib.lua:20-22]
TBH_PHUT_TRONG_THEGIOI = 20 -- TIME_LIFE_TRONG (mã ghi 1200*18)                            [lib.lua:9]
TBH_PHUT_TRONG_BANG  = 30   -- TIME_LIFE_TRONG1                                            [lib.lua:10]
TBH_PHUT_NPC         = 40   -- TIME_LIFE_CTT                                               [lib.lua:11]
TBH_GIO_MO           = 1900 -- khung dùng item Trống Khải Hoàn (nStartTime)                [lib.lua:12, event\trongbanghoi\drum.lua:28-32]
TBH_GIO_DONG         = 2359 -- nCloseTime                                                  [lib.lua:13]
TBH_GIAY_DANH_THEGIOI= 23   -- PaceBar cướp trống server                                   [event\trongbanghoi\tungtung.lua:35]
TBH_GIAY_DANH_BANG   = 5    -- PaceBar trống bang                                          [event\trongbanghoi\tungtung1.lua:43]

HDANG_GIO            = {{16,0}}  -- Hoa Đăng, {17,0} đã comment                            [event\event_cauhoi\lib.lua:27-29, timerserver.lua:915-921]
HDANG_PHUT_TONTAI    = 30   -- TIME_LIFE_QUESTION                                          [event\event_cauhoi\lib.lua:21]

HH_GIO               = {{12,0},{12,10},{12,20},{19,0},{19,10},{19,20}}
                            -- Hạt Huy Hoàng, đợt lẻ gieo hạt / đợt chẵn quả chín          [event\event_huyhoang_dungdb\lib_huyhoang.lua:27-34, timerserver.lua:876-884]
HH_PHUT_CHIN         = 5    -- TIME_SWITCH (5*60*18)                                       [lib_huyhoang.lua:8]
HH_PHUT_HUY          = 5    -- TIME_DELETE_QHH                                             [lib_huyhoang.lua:9]
HH_GIAY_THUHOACH     = 5    -- TIME_DELAY_TH                                               [lib_huyhoang.lua:25]
HH_HAN_QUA_NGAY      = 7    -- TG_SD_QUAHH / TG_SD_QUAHK (60*60*24*7)                      [lib_huyhoang.lua:19-20]

VT_GIO_TU            = 13   -- Vận Tiêu, giờ chặn THẬT. 🔴 BA NGUỒN LỆCH (cảnh báo A3)     [event\event_vantieu\lib_vt.lua:11, event\event_vantieu\tieudau.lua:24]
VT_GIO_DEN           = 23   --                                                             [lib_vt.lua:11]
VT_PHUT_HANGIAO      = 30   -- TIME_LIMIT_VT (30*60*18)                                    [lib_vt.lua:96]

PLDV_GIO             = {0,2,4,6,8,10,12,14,16,18,20,22}  -- Phong Lăng Độ Việt, phút :00   [tinhnang\phonglangdo\lib_phonglangdo.lua:88-106, timerserver.lua:926-927]
PLDV_PHUT_BAODANH    = 9    -- TIME_PLD_BD (9*60*18)                                       [lib_phonglangdo.lua:7, timerserver.lua:934]
PLDV_PHUT_TRAN       = 30   -- MSTIME_PLD_KT                                               [lib_phonglangdo.lua:8, timertask\task04.lua:53]
PLDV_PHUT_XOA_NPC    = 1    -- MSTIME_PLD_XOANPC                                           [lib_phonglangdo.lua:9, timertask\task05.lua:66, task10.lua:38-41]
PLDV_PHUT_RA_BOSS    = {20,25,30}  -- mốc 3 ra ĐẠI Thuỷ Tặc Đầu Lĩnh                       [lib_phonglangdo.lua:114, timerserver.lua:942-956]
PLDV_PHUT_BOSS_SONG  = 15   -- TIMER_BOSS_TTDL                                             [lib_phonglangdo.lua:13, lib_phonglangdo.lua:213]
PLDV_GIAY_BAOTIN     = 30   -- M_TB_TIMER                                                  [lib_phonglangdo.lua:22, thuyenphu.lua:160]
PLDV_GIO_TONPHI      = {10,14,16,18,20,22}  -- giờ cần Lệnh Bài + rơi Truy Công Lệnh       [lib_phonglangdo.lua:130-145]

VAV_GIO              = {{13,0},{15,0},{17,0},{19,0},{23,0}}  -- Vượt Ải Việt cũ            [tinhnang\vuot_ai\lib_vuotai.lua:21-31, timerserver.lua:887-888]
VAV_PHUT_BAODANH     = 10   -- 🔴 BỊ DÙNG LẠI làm phút so sánh loa (cảnh báo A4)           [lib_vuotai.lua:12, timerserver.lua:900, timerserver.lua:907]
VAV_PHUT_NHIEMVU     = 30   -- MSTIME_VUOT_AI_KT                                           [lib_vuotai.lua:13, timertask\task01.lua:146, die_boss.lua:39]
VAV_GIAY_CHOT_THUONG = 5    -- 5*18 sau khi giết boss cuối                                 [tinhnang\vuot_ai\die_boss.lua:36]
VAV_GIAY_BAOTIN      = 30   -- SetTimer(30*18,1)                                           [global\npcchucnang\nhieptran.lua:376]
VAV_PHUT_PHANDON     = 5    -- skill Phản Đòn ải 27 (60*18*5)                              [lib_vuotai.lua:219]

LDBH_LICH            = {{19,49,20,30,1}}  -- {giờ BD,phút,giờ đánh,phút,thứ}. 🔴 3 NGUỒN   [tinhnang\loidai\lib_loidai.lua:13-16, timerserver.lua:769-780]
LDBH_PHUT_CHO        = 10   -- 10*1080                                                     [timerserver.lua:779]
LDBH_PHUT_TRAN       = 20   -- 20*1080                                                     [timerserver.lua:780]

CTCV_LICH            = {{20,30,4,20,30,5}}  -- CTC Việt cũ {giờ LĐ,phút,thứ,giờ CT,phút,thứ} [tinhnang\congthanhchien\lib_ctc.lua:98-101]
CTCV_PHUT_BAODANH    = 10   -- CTC_MINUS_BD                                                [lib_ctc.lua:87, timerserver.lua:814]
CTCV_PHUT_TRAN       = 60   -- CTC_MINUS_KT (cũng là hạn sống NPC trụ/cổng)                [lib_ctc.lua:88, timerserver.lua:815, lib_ctc.lua:193, lib_ctc.lua:201]
CTCV_MOC_LOA         = {7,3}    -- phút loa nhắc trước khai chiến                          [timerserver.lua:827]
CTCV_THU_BAODANH     = 4    -- NPC mở mục báo danh Lôi Đài (thứ 5)                         [tinhnang\congthanhchien\congthanhquan.lua:24]
CTCV_THU_LOIDAI      = 4    --                                                             [congthanhquan.lua:27]
CTCV_THU_CONGTHANH   = 5    --                                                             [congthanhquan.lua:30]

BHK_PHUT_TONTAI      = 120  -- Boss Hoàng Kim, TIME_LIFE_BOSS dùng chung CẢ 4 loại         [tinhnang\boss_hoangkim\lib_bosshk.lua:13]
BHKT_GIO             = {{15,0},{20,0}}   -- boss Tiểu (11/13/17/22h đã comment)            [lib_bosshk.lua:52-59, timerserver.lua:855-860]
BHKD_GIO             = {{11,0},{17,30}}  -- boss Đại (15:30/18/22:30 đã comment)           [lib_bosshk.lua:61-67, timerserver.lua:862-867]
BHKSV_GIO            = {{12,0},{19,30}}  -- boss Máy Chủ (17:30/22:30/23 đã comment)       [lib_bosshk.lua:70-76, timerserver.lua:868-873]
BHKSV_PHUT_RUONG     = 5    -- rương rơi tồn tại (5*60*18)                                 [tinhnang\boss_hoangkim\drophksv.lua:79]
BST_PHUT_HOISINH     = 2    -- boss Sát Thủ Việt hồi sinh (2*60*18)                        [tinhnang\boss_satthu\death.lua:77]

TW_NGAY_THIDAU       = {29,30,31,1,2,3,5}  -- Bang Chiến khoá theo NGÀY-TRONG-THÁNG. 🔴 5 NƠI [event\tongwar\head.lua:523, head.lua:549-557, head.lua:498, event\tongwar\headinfo.lua:126-134, relay\tongwar.lua SCHEDULE_TABLE]
```

---

# TỆP 2 — `ch_thuong.lua` (PHẦN THƯỞNG)

## ⭐ Ưu tiên 1 — TỐNG KIM

```
TK_THUONG_THANG      = {exp=15000000, nhan=3, item={{1023,30},{3846,5},{4728,5},{4844,500},{4831,50},{4850,10}}, dsk=50, xu=5}
                            -- ThuongPheThangTongKim: exp = 15tr * (rank*3)                [tinhnang\tong_kim_tcap\lib_tktc.lua:711-726]
TK_THUONG_THUA       = {exp=10000000, nhan=1, item={{1023,20},{3846,3},{4728,3},{4844,300},{4831,30},{4850,5}},  dsk=50, xu=2}
                            -- 🔴 exp phe THUA (10tr) CAO HƠN phe HOÀ (5tr)                [lib_tktc.lua:728-744]
TK_THUONG_HOA        = {exp=5000000,  nhan=2, item={{1023,10},{3846,2},{4728,2},{4844,100},{4831,20},{4850,2}},  dsk=50, xu=2}
                                                                                            [lib_tktc.lua:693-708]
TK_DIEM_TOITHIEU_NHANTHUONG = 1000  -- MIN_POINT_AWARD                                     [lib_tktc.lua:25, timertask\task03.lua:193, task03.lua:200, task03.lua:206]
TK_TICHLUY_THANG     = 1200 -- điểm tích luỹ cộng cuối trận (nhân quân hàm)                [lib_tktc.lua:22, timertask\task03.lua:192]
TK_TICHLUY_THUA      = 300  --                                                             [lib_tktc.lua:23, task03.lua:199]
TK_TICHLUY_HOA       = 600  --                                                             [lib_tktc.lua:24, task03.lua:205]
TK_XU_TOP1           = 20   -- Xu bảng xếp hạng (nhân TK_HESO_XU_CUUSAT)                   [timertask\task03.lua:49-59]
TK_XU_TOP2           = 15   --                                                             [task03.lua:49-59]
TK_XU_TOP3           = 10   --                                                             [task03.lua:49-59]
TK_XU_TOP10          = 5    -- hạng 4..10                                                  [task03.lua:49-59]
TK_HESO_XU_CUUSAT    = 3    -- nXutk; phương thức khác = 1 (thực tế LUÔN = 3)              [task03.lua:49-59]
TK_SO_HANG_THUONG    = 10   -- nTotalRank = 9 -> vòng i=0..9                               [timertask\task03.lua:79-83]
TK_MOC_CHANNGUYEN    = 3000 -- điểm tích luỹ để nhận Chân Nguyên Đơn                       [timertask\task03.lua:187-190]
TK_SL_CHANNGUYEN     = 2    -- item 4846                                                   [task03.lua:187-190]
TK_DIEM_GIET_NPC     = {5,100,200,400,1000}  -- TICHLUYTK theo cấp NPC 1..5                [lib_tktc.lua:128-134, tinhnang\tong_kim_tcap\binhsi.lua:30]
TK_DIEM_GIET_SOAI    = 5000 -- TICHLUY_GIETSOAI                                            [lib_tktc.lua:137, tinhnang\tong_kim_tcap\nguyensoai.lua:40]
TK_DIEM_CAM_CO       = 300  -- FLAG_POINT                                                  [lib_tktc.lua:27, maps\tongkim\trap\pos_cokim.lua:16-18, pos_cotong.lua]
TK_SO_CO_TOIDA       = 30   -- MAX_CAMCO                                                   [lib_tktc.lua:26, tinhnang\tong_kim_tcap\cotk.lua:30]
TK_DIEM_BAOVAT       = {{206,200},{207,220},{208,240},{209,260},{210,300}}                 [lib_tktc.lua:30-36, tinhnang\tong_kim_tcap\itembaovat.lua:17-21]
TK_DIEM_GIET_NGUOI   = 30   -- 🔴 hằng KILL_PLAYER_POINT CHẾT; số 30 hardcode 6 chỗ/tệp    [lib_tktc.lua:28, tinhnang\tong_kim_tcap\tongtu.lua:73-81, tinhnang\tong_kim_tcap\kimtu.lua, tinhnang\congthanhchien\lib_ctc.lua:95]
TK_MOC_QUANHAM      = {{0,1000,100},{1000,3000,200},{3000,7000,400},{7000,10000,600},{10000,35000,800},{35000,1000000,1000}}
                            -- TAB_QUANHAM (cột 3 KHÔNG ai đọc)                            [lib_tktc.lua:119-126, lib_tktc.lua:108-116]
TK_GIA_NHACVUONGKIEM = 10000000  -- NEED_MONEY lượng                                       [tinhnang\tong_kim_tcap\quanquan.lua:12]
TK_SL_NVHT           = 100  -- NVHT_TO_NVK, số Nhạc Vương Hàn Thạch                        [quanquan.lua:15]
TK_HESO_DOI_EXP      = 1000 -- điểm tích luỹ x 1000 = exp                                  [quanquan.lua:62-82]
TK_GIA_CHANNGUYEN    = 2000 -- điểm đổi 1 Chân Nguyên Đơn (4847)                           [quanquan.lua:84-106]
TK_GIA_HUYCHUONG    = 50   -- điểm đổi 1 Huy Chương Tống Kim                              [quanquan.lua:108-117]
TK_GIA_MAUNHANH      = 1    -- lượng/ô Ngũ Hoa Ngọc Lộ Hoàn                                [tinhnang\tong_kim_tcap\quany.lua:30]
TK_PHI_TRINHSAT      = 500  -- MONEY_TS quan tiền                                          [tinhnang\tong_kim_tcap\trinhsat.lua:9, trinhsat.lua:61, trinhsat.lua:74]
```

## ⭐ Ưu tiên 2 — CÔNG THÀNH + LÔI ĐÀI JX2

```
CTC_EXP_TOP10        = 3000000  -- 🔴 KHAI 2 NƠI cùng giá trị                              [missions\citywar_city\head.lua:28, tinhnang\congthanhchien\lib_ctc.lua:89]
CTC_DIEM_KILL        = 75   -- BONUS_KILLPLAYER                                            [missions\citywar_city\head.lua:149]
CTC_DIEM_LIENTRAM    = 150  -- BONUS_MAXSERIESKILL                                         [citywar_city\head.lua:150]
CTC_CHUKY_LIENTRAM   = 3    -- mod(serieskill,3)==0                                        [missions\citywar_city\playerdeath.lua:30]
CTC_MOC_QUANHAM      = {0,1000,3000,6000,10000}   -- TAB_RANKBONUS                         [citywar_city\head.lua:169]
CTC_HESO_QUANHAM     = <ma trận 5x5>  -- RANK_PKBONUS                                      [citywar_city\head.lua:151-157]
CTC_BANG_LIENTRAM    = <ma trận 5x5 0/1>  -- TAB_SERIESKILL                                [citywar_city\head.lua:159-166]
CTC_MOC_DANHHIEU     = {0,10000,20000,40000,60000,80000}                                   [citywar_city\head.lua:181-186]
CTC_SONGUOI_DANHHIEU = {400,60,25,10,5,1}                                                  [citywar_city\head.lua:188-193]
CTC_ITEM_THUONG      = {6,1,1076}  -- Lễ hộp quả huy hoàng cuối trận                       [missions\citywar_city\camper.lua:85]
CTC_SONGUOI_QUAY     = 5    -- số người bốc ngẫu nhiên                                     [citywar_city\camper.lua:38]
CTC_GIA_THUOC        = {[1307]=1000,[1308]=800,[1309]=800,[1310]=800}                      [citywar_city\head.lua:16-22]
CTC_GIA_LENHBAI      = 200000   -- CardPrice                                               [missions\citywar_global\head.lua:20]
CTC_GIA_THUHOI_LENHBAI = 10000  -- ReturnCardPrice                                         [citywar_global\head.lua:22]
CTC_MUA_LENHBAI_TOIDA  = 30                                                                [missions\citywar_global\infocenter_head.lua:331]
CTC_HAN_LENHBAI_PHUT   = 7200   -- 5 ngày                                                  [infocenter_head.lua:341, infocenter_head.lua:364]
CTC_NOP_LENH_NGAY      = 300                                                               [infocenter_head.lua:197, infocenter_head.lua:242]
CTC_EXP_MOI_LENH       = 50000                                                             [infocenter_head.lua:221, infocenter_head.lua:271]
CTC_TRAN_TONG_LENH     = 2000000000                                                        [infocenter_head.lua:216]
CTC_TRAN_NOP_1LAN      = 1000000                                                           [infocenter_head.lua:647-649, infocenter_head.lua:681]
CTC_THAU_MIN         = 1000000  -- đấu thầu báo danh Lôi Đài                               [infocenter_head.lua:94]
CTC_THAU_MAX         = 99999999                                                            [infocenter_head.lua:111]
CTC_ITEM_CHIEMTHANH  = {6,1,146,5}  -- Huyền tinh cấp 5                                    [citywar_function.lua:154, citywar_function.lua:191, tinhnang\congthanhchien\lib_ctc.lua:94]
CTC_SO_HUYENTINH_DOTHANH  = 300  -- thành 4 và 7                                           [citywar_function.lua:199-205]
CTC_SO_HUYENTINH_THANHTHI = 200  -- các thành khác                                         [citywar_function.lua:199-205]
CTC_PHI_BAODANH      = 1000000  -- [ENGINE] SignUpFee                                      [settings\citywar.ini:69]
CTC_THUE_TOIDA       = 20   -- [ENGINE] MaxExchangeTax                                     [settings\citywar.ini:75]
CTC_GIA_TOIDA        = 20   -- [ENGINE] MaxPriceParam                                      [settings\citywar.ini:78]
CTC_TILE_TRICH_THUE  = 30   -- [ENGINE] SupplyLineBuildScale                               [settings\citywar.ini:91]

CTLD_TONGEXP_THANG   = 1200 -- WIN_TONGEXP. 🔴 KHAI 2 NƠI                                  [missions\citywar_arena\head.lua:19, tinhnang\loidai\lib_loidai.lua:32]
CTLD_TONGEXP_THUA    = 1400 -- LOSE_TONGEXP — 🔴 SỐ CHẾT (SubTongExp đã comment)           [citywar_arena\head.lua:20, citywar_arena\head.lua:147, head.lua:151, lib_loidai.lua:33]
CTLD_DIEM_MOI_MANG   = 1    -- +1 MS_TONGxVALUE mỗi mạng                                   [missions\citywar_arena\death.lua:16-25, tinhnang\loidai\playerdeath.lua:20-32]
```

## Ưu tiên 3 — Hoạt động đang tắt / nửa vời

```
LDHC_EXP_MOI_MANG    = 10000000  -- 🔴 LỖ EXP ĐANG MỞ (cảnh báo A1)                        [tinhnang\loidaihonchien\bigiet.lua:10-16]
LDHC_TRAN_MANG       = 4    -- trần 4 mạng/trận (GetTaskTemp(1))                           [bigiet.lua:10-16, mainloidai.lua:45, timerserver.lua:312]
LDHC_THUONG_THAMGIA  = {exp=50000000, item={{4844,100}}, dsk=20}                           [timerserver.lua:314-321]
LDHC_THUONG_QUANQUAN = {exp=500000000, manhHK={753,770,5}, quay=<6 nhánh 30/50/70/90/95>}
                            -- 🔴 CÓ HAI BẢN (nhánh 16h và nhánh 22h)                      [timerserver.lua:368-398, timerserver.lua:456-486]
LDHC_PHI_BAODANH     = 0    -- cost, chưa từng có lệnh Pay                                 [tinhnang\loidaihonchien\mainloidai.lua:13]

LDBH_PHI_BAODANH     = 1000000  -- 🔴 VẪN THU THẬT dù trận không mở (cảnh báo A5)          [tinhnang\loidai\lib_loidai.lua:17, tinhnang\congthanhchien\congthanhquan.lua:25]
LDBH_EXP_THANG       = 2000000  -- AddOwnExp, KHÔNG nhân EXP_RATE                          [lib_loidai.lua:19, lib_loidai.lua:242]
LDBH_EXP_THUA        = 1000000  --                                                         [lib_loidai.lua:18, lib_loidai.lua:239]

CTCV_TICHLUY_THANG   = 100  -- TICH_LUY_THUONG_CT                                          [tinhnang\congthanhchien\lib_ctc.lua:83]
CTCV_DIEM_PHATRU     = 300  -- KILL_TRU_POINT                                              [lib_ctc.lua:96]
CTCV_DIEM_QUANHAM    = {100,200,400,600,800}  -- TAB_QUANHAM 5 bậc                         [lib_ctc.lua:158-164]
CTCV_HESO_LIENTRAM   = 5    -- nLTramT*5*nCRank                                            [tinhnang\congthanhchien\congtu.lua:33, congtu.lua:36, tinhnang\congthanhchien\thutu.lua:32, thutu.lua:35]
CTCV_MOC_THUONG      = 1000 -- điểm để nhận item 6,1,1075                                  [lib_ctc.lua:348-355]
CTCV_THUE_MIN        = 0    -- MIN_TAX                                                     [lib_ctc.lua:73]
CTCV_THUE_MAX        = 20   -- MAX_TAX                                                     [lib_ctc.lua:74]
CTCV_SO_HUYENTINH_TUAN = 200  -- MAX_NUM_AWD                                               [congthanhquan.lua:12]
CTCV_XU_MUA_DAOCU    = 100  -- NUM_XU_MUA_DC                                               [tinhnang\congthanhchien\quanquan.lua:9]

TBH_EXP_NGAY         = 200000000  -- EXP_TBH_DAY, trần/người/ngày (task 105)               [event\trongbanghoi\lib.lua:14, lib\lib_task.lua:126, lib\lib_ham.lua:276]
TBH_EXP_MIN          = 1000000    -- EXP_RAN1                                              [event\trongbanghoi\lib.lua:15]
TBH_EXP_MAX          = 2000000    -- EXP_RAN2                                              [lib.lua:16]
TBH_SO_NHIP          = 10   -- 🔴 vòng lặp KHÔNG kiểm lại trần (cảnh báo B1)               [event\trongbanghoi\tungtung1.lua:58]
TBH_BANG_THUONG      = <6 nhánh random(1,6)>  -- 353x3 / 238+239+240 / 10tr lượng / 123x30 / 26x5 / 22x5  [event\trongbanghoi\tungtung.lua:62-87]
TBH_EXPBANG          = 500  -- SetTongExp(nExp+500)                                        [tungtung.lua:89-91]
TBH_ITEM_TRONG       = 2318 -- Trống Khải Hoàn rơi ra                                      [tungtung.lua:61]
TBH_GIA_THUOC        = 3000 -- lượng/ô tại Công Tôn Toản                                   [event\trongbanghoi\congtontoan.lua:55-56]

KMQ_GIA_THUOC        = 3000 -- lượng/ô Ngũ Hoa Ngọc Lộ Hoàn                                [event\kiemmonquan\congtonthu.lua:50]
KMQ_SHOP_ID          = 53   -- Sale(53,0)                                                  [congtonthu.lua:45]

VT_GIA_DAOCU         = {[4771]=500000,[4772]=1000000,[4773]=2000000,[4774]=150000}         [event\event_vantieu\lib_vt.lua:84-87]
VT_PHI_NHAN          = 500000   -- NEED_MONEY                                              [lib_vt.lua:98]
VT_PHI_TRUYENTONG    = 100000   -- NEED_MONEY_GO                                           [lib_vt.lua:99]
VT_LUOT_NGAY         = 20   -- MAXVANTIEU                                                  [lib_vt.lua:97]
VT_CUOP_NGAY         = 5    -- MAX_CUOP_TIEU                                               [lib_vt.lua:95]
VT_TILE_LOAI_TIEU    = {20,40,50}  -- random(1,50): <20 Đồng, <40 Bạc, else Vàng           [event\event_vantieu\tieudau.lua:101-110]
VT_MAU_TIEUXA        = 2000000     -- nLifeNpc = nKind * 2.000.000                         [tieudau.lua:152]
VT_THUONG_THEO_LOAI  = {dong={exp=100000000,i={{4844,300},{4847,50},{4850,2}},tong=100},
                        bac ={exp=300000000,i={{4844,400},{4847,70},{4850,5}},tong=200},
                        vang={exp=500000000,i={{4844,500},{4847,100},{4850,10}},tong=300}}
                            -- 🔴 TEXT thoại ghi số NHỎ HƠN 10 LẦN (cảnh báo A3)           [tieudau.lua:246-279]
VT_THUONG_RANDOM     = <10 Lệnh Bài Boss 1023 + 1 nhánh random(1,100)>                     [tieudau.lua:280-300]
VT_THUONG_CUOPTIEU   = {{500000,1},{1000000,2},{2000000,3}}  -- exp(*EXP_RATE) + số HTL    [event\event_vantieu\drop_tieu.lua:28-40]

PLDV_THUONG_GIET_BOSS = {xu=3, item={{1023,5},{4850,2},{4844,50}}, manhHKMP=1}             [tinhnang\phonglangdo\bossthuytacdaulinhpld.lua:54-79]
PLDV_THUONG_TODOI_BOSS= {xu=1, expChia=2, item={{1023,3},{4844,30}}}                       [bossthuytacdaulinhpld.lua:82-100]
PLDV_DIEM_SUKIEN_LENTHUYEN = 20                                                            [tinhnang\phonglangdo\thuyenphu.lua:152-153]
PLDV_THUONG_TRA_NV   = {item={6,1,71}, tien=1000}                                          [tinhnang\phonglangdo\thuyenphubac.lua:54-58]
PLDV_PHI_VETHANH     = 1000  -- MONEY_VETHANH                                              [lib_phonglangdo.lua:10, thuyenphu.lua:64-68, thuyenphubac.lua:35-39]
PLDV_SO_MATDO        = 200   -- REQ_MDTB                                                   [lib_phonglangdo.lua:21, thuyenphu.lua:131-137]
PLDV_LUOT_DITHUYEN_NGAY = 2  -- MAX_DI_PLD_NGAY                                            [lib_phonglangdo.lua:115, thuyenphu.lua:79]
PLDV_LUOT_TRA_NV_NGAY   = 1  -- 🔴 dùng `>` nên thực tế 2 lượt                             [lib_phonglangdo.lua:116, thuyenphubac.lua:74]
PLDV_LUOT_NHANNV_NGAY   = 1  -- NUM_NHAN_NVTTINDAY                                         [lib_phonglangdo.lua:16, thuyenphu.lua:47]
PLDV_SO_QUAI_NV      = 188   -- 🔴 VÔ NGHĨA (điều kiện kiểm đã comment)                    [lib_phonglangdo.lua:23, thuyenphubac.lua:70-73]

VAV_THUONG_HOANTHANH = {exp=1600000, item={{1401,3},{1023,3},{4844,50}}}  -- exp x EXP_RATE [timertask\task06.lua:52-64, tinhnang\vuot_ai\lib_vuotai.lua:16]
VAV_PHI_GHEP_SATTHUGIAN = 80000  -- MONEYGHEPSTG                                           [global\npcchucnang\nhieptran.lua:17, nhieptran.lua:136-166]

BHKT_SL_RUONG_XANH   = 2    -- item 4850 cho người giết một mình                           [tinhnang\boss_hoangkim\deathhktieu.lua:34]
BHKT_SL_MANH_HKMP    = 1                                                                   [deathhktieu.lua:35]
BHKT_EXP_BANG        = 20                                                                  [deathhktieu.lua:36]
BHKD_SL_RUONG_XANH   = 3                                                                   [tinhnang\boss_hoangkim\deathhkdai.lua:34]
BHKD_SL_MANH_HKMP    = 0    -- 🔴 dòng phát ĐÃ COMMENT -> hiện KHÔNG phát                  [deathhkdai.lua:36]
BHKD_EXP_BANG        = 50                                                                  [deathhkdai.lua:37]
BHKSV_QUA_NGUOIGIET  = {{6,1,26},{6,1,22},{4850,5}}  -- VLMT + TTK + 5 rương xanh          [tinhnang\boss_hoangkim\deathhksv.lua:34-36]
BHKSV_XU             = 50   -- task 251 T_PLAYER_XU                                        [deathhksv.lua:46]
BHKSV_EXP_BANG       = 100                                                                 [deathhksv.lua:48]
BHKSV_MANH_HK_MIN    = 5    -- random(5,10) ký gửi kho bang                                 [deathhksv.lua:56]
BHKSV_MANH_HK_MAX    = 10                                                                  [deathhksv.lua:56]
BHKSV_THUYTINH_MIN   = 1                                                                   [deathhksv.lua:57]
BHKSV_THUYTINH_MAX   = 3                                                                   [deathhksv.lua:57]
BHKSV_KNB            = 1                                                                   [deathhksv.lua:60]
BHKSV_SO_RUONG       = 10   -- rương sinh quanh xác boss                                   [tinhnang\boss_hoangkim\drophksv.lua:74]
BHKSV_RUONG_NGAY     = 4    -- trần rương nhặt/ngày (byte 1 task 358)                      [tinhnang\boss_hoangkim\ruong.lua:9, ruong.lua:25]
BHKSV_HMD_MIN        = 5    -- Huy Hoàng Đơn 4844 trong 1 rương                            [ruong.lua:35-38]
BHKSV_HMD_MAX        = 30                                                                  [ruong.lua:35-38]
BHKSV_O_TUI_TOITHIEU = 10   -- ô trống để rút kho bang                                     [deathhksv.lua:282, deathhksv.lua:299]
BHKPLD_SL_RUONG_XANH = 3                                                                   [tinhnang\boss_hoangkim\deathbosspld.lua:31]
BHKPLD_EXP_BANG      = 200                                                                 [deathbosspld.lua:34]
BHK_BANG_MANH_HKMP   = {253,258,263,239,243,248,313,308,318,298,303,268,276,283,288,331,333,338,343,353,358,363,368}
                            -- 23 mã, dùng chung boss tiểu + máy chủ + PLD                 [tinhnang\boss_hoangkim\deathhktieu.lua:23, deathhksv.lua:38-42, tinhnang\phonglangdo\bossthuytacdaulinhpld.lua:54-79]

BST_ITEM_LENH        = {6,1,398,10}  -- Sát Thủ Lệnh cố định rơi                           [tinhnang\boss_satthu\drop.lua:49]
BST_BUFF_ID          = 541  -- AddSkillState(541,1,0,108*2,-1)                             [tinhnang\boss_satthu\death.lua:22]
BST_NV_NGAY          = 8    -- MAX_NVSATTHU. 🔴 BA BẢN SỐ 8 (cảnh báo A6)                  [global\npcchucnang\nhieptran.lua:14, tinhnang\vuot_ai\sugiasatthu.lua:14]
BST_TIEN_NHAN_NV     = 5000 -- 🔴 XUNG ĐỘT 5000 vs 50000 (cảnh báo A7)                     [global\npcchucnang\nhieptran.lua:18, tinhnang\vuot_ai\sugiasatthu.lua:18]
BST_TIEN_DEN_BOSS    = 1000 -- 🔴 XUNG ĐỘT 1000 vs 5000                                    [nhieptran.lua:19, sugiasatthu.lua:19]
BST_STACK_TOIDA      = 100  -- 🔴 XUNG ĐỘT 100 vs 1                                        [nhieptran.lua:24, sugiasatthu.lua:24]
```

## Ưu tiên 4 — Nhiệm vụ hằng ngày / bạn đồng hành / chuyển sinh

```
DT_MOC30_SO_LUOT     = 30   -- hoàn thành 30 NV/ngày                                       [global\seasonnpc.lua:109-112]
DT_MOC30_EXP         = 30000000                                                            [seasonnpc.lua:109-112]
DT_MOC40_SO_LUOT     = 40   -- 40 NV KHÔNG huỷ lần nào                                     [seasonnpc.lua:115-128]
DT_MOC40_EXP         = 100000000                                                           [seasonnpc.lua:115-128]
DT_MOC40_RUONG       = {{6,1,2383},5}  -- 5 Bảo rương thần bí + loa toàn server 10s        [seasonnpc.lua:115-128]
DT_MOC10_CHUKY       = 10   -- mỗi 10 NV tích luỹ                                          [seasonnpc.lua:829-839]
DT_MOC10_ITEM        = {{6,1,1023},3}  -- 3 Boss Triệu Hoán Phù                            [seasonnpc.lua:829-839]
DT_O_TRONG_TOITHIEU  = 5                                                                   [seasonnpc.lua:818, seasonnpc.lua:917]
DT_TILE_LOAI_THUONG  = {20,33,34,8,5}  -- tiền/exp/vật phẩm/đổi lại/cơ hội huỷ             [task\newtask\tasklink\tasklink_award.lua:31]
DT_LIENHOA_GIA       = 10000   -- MAKING_COST                                              [global\seasonnpc.lua:169-170]
DT_LIENHOA_SOHOA     = 9       -- MAKING_COUNT                                             [seasonnpc.lua:169-170]
DT_NGUONG_LOG_TIEN   = 300000                                                              [seasonnpc.lua:987]
DT_RUONG2383_CHUY    = 1    -- cần 1 Huyền Thiên Chuỳ (6,1,2357)                           [item\ruong_datau_tasklink.lua:13]
DT_RUONG2383_O       = 4    -- ô trống                                                     [item\ruong_datau_tasklink.lua:17]
DT_RUONG2383_BANG    = {25000,50000,80000,90000,95000,98000,100000}  -- thang 1..100000    [item\ruong_datau_tasklink.lua:24-51]

BDH_DS_SACH          = {849,850,851,853,854,855,859,861,862,863,864,868,870}  -- ARY_SKILLBOOK [task\partner\task_award.lua:48]
BDH_SACH_DAI_CAP     = {199,599}  -- floor(random(199,599)/100) -> cấp 1..5                [task\partner\task_award.lua:439-455]
BDH_SACH_TILE        = 30   -- chế độ 3 chỉ cho 30%                                        [task_award.lua:439-455]
BDH_TD_TILE_SUKIEN   = {40,20,20,20}  -- sách / Boss / exp / trắng                         [task\partner\train\partner_jitan.lua:5-9]
BDH_TD_BOSS_ID       = 1115 -- boss gọi ra, cấp 95                                         [partner_jitan.lua:117]
BDH_TL_HESO_LUOT     = 0.1  -- prize_parameter = lượt*0.1 + 1                              [task\partner\reward\partner_reward.lua:154-155]
BDH_TL_CHENH_CAP     = 10   -- cấp BĐH + 10 >= cấp nhiệm vụ                                [partner_reward.lua:66]
BDH_TL_HAN_DANHHIEU  = 30*24*60*60*18*36  -- TITLETIME (GIỮ NGUYÊN biểu thức)              [partner_reward.lua:15, partner_reward2.lua:15]
BDH_TL_DANHHIEU      = {77,78,79,80}                                                       [partner_reward.lua:217-218, partner_reward.lua:224-225, partner_reward.lua:232-233, partner_reward2.lua:217]
BDH_KH_CHISO         = {5,5,5,5,5,5}  -- 6 chỉ số khởi tạo bạn đồng hành                   [task\partner\education\swordking_people.lua:242]

CS_SO_LAN_TOIDA      = 7    -- NTRANSLIFE_MAX                                              [task\metempsychosis\task_head.lua:16]
CS_CAP_MOI_LAN       = {160,170,180,200,200,200,200}  -- TB_LEVEL_LIMIT                    [task_head.lua:52]
CS_HOCPHI            = 100000000  -- ZHUANSHENG_TUITION                                    [task_head.lua:67]
CS4_HOCPHI           = 100000000  -- ZHUANSHENG_TUITION_4                                  [task_head.lua:109]
CS4_GIA_TAY_DIEM     = 10000000   -- CLEAR_SKILL_4_PRICE                                   [task_head.lua:110]
CS4_CAP_BATDAU       = 105  -- NSTARTLEVEL_4                                               [task_head.lua:116]
CS4_CAP_MOI_DIEM     = 5    -- NPERPOINTNEEDLEVEL                                          [task_head.lua:117]
CS4_VATPHAM          = {{6,1,2973,999},{6,1,2974,1}}  -- TBITEMNEED_4                      [task_head.lua:55-58]
CS5_SO_CAY           = 40   -- N_TRANSLIFE_TREE_MIN                                        [task_head.lua:123]
CS5_MOC              = {award=5, songjin=300, chuanguan=17, ydbz=6, translife=4, level=200} [task\metempsychosis\translife_5.lua:18-23]
CS5_VATPHAM          = {{6,1,30246},2}                                                     [task_head.lua:126-130]
CS5_THUONG           = {{6,1,3895},5}  -- Vô Cực Tiên Đơn, khoá vĩnh viễn                  [translife_5.lua:17]
CS6_DIEUKIEN_NV      = {5,150}     -- {số lần CS, cấp}                                     [task\metempsychosis\translife_6.lua:27]
CS6_DIEUKIEN_CS      = {5,200}                                                             [translife_6.lua:28]
CS6_BANG_NHIEMVU     = <6 dòng: cây 800/30/1; rương 2000/1500/1; Viêm Đế 60/200/6; Tống Kim 40/200/4000; vượt ải 60/200/20; PLD 60/200/1>  [translife_6.lua:30-37]
CS_BACDAU_DICHUYEN   = 5    -- BEIDOU_TRANSFER_MAX                                         [task_head.lua:119]

CSDS_CAP_YEUCAU      = {150,170,180,200}  -- Chuyển Sinh Đại Sư (HỆ KHÁC, event NPC)       [event\chuyensinhdaisu.lua:26, chuyensinhdaisu.lua:40, chuyensinhdaisu.lua:46, chuyensinhdaisu.lua:52, chuyensinhdaisu.lua:59]
CSDS_GIA_TIEN        = {30000000,50000000,100000000,200000000}  -- 🔴 VIẾT 2 LẦN + text    [chuyensinhdaisu.lua:41, :47, :53, :60, :75, :82, :89, :96, :17]
CSDS_GIA_XU          = {500,1000,1700,3000}                                                [chuyensinhdaisu.lua:41, :47, :53, :60, :75, :82, :89, :96]
CSDS_CAP_TICHLUY     = {130,130,140,150}  -- nTranLevle cộng mỗi lần                       [chuyensinhdaisu.lua:45, :51, :57, :64]
CSDS_CAP_SAU_CS      = 120  -- SetLevel(120)                                               [chuyensinhdaisu.lua:111]

TDV_GIA_QUASUNG      = 1    -- Tết Đoàn Viên, xu/quả                                       [event\eventtet\npcsukien.lua:29-31]
TDV_TOIDA_MOI_LAN    = 500                                                                 [npcsukien.lua:43]
TDV_CONGTHUC_GHEP    = {4836,4837,4838,4839,4840}  -- mỗi thứ 1 -> Mâm Vàng 4842           [npcsukien.lua:61-86]
TDV_MOC_THUONG       = {2000,4000,6000,8000,10000}  -- 🔴 MENU ĐÃ COMMENT (cảnh báo C3)    [npcsukien.lua:121-203, npcsukien.lua:19-20]
TDV_HAN_ITEM_NGAY    = 30   -- bình luận ghi 7 ngày - SAI                                  [npcsukien.lua:177, :180, :195, :198]
TDV_RESET_MOC        = 10000                                                               [npcsukien.lua:96-105]
TDV_RESET_PHI        = 30   -- 🔴 kiểm 30 nhưng CHỈ TRỪ 10                                 [npcsukien.lua:99-105]

TTPLD_BANG_EXP       = {{10,30},{15,40},{20,20},{25,5},{30,5}}  -- {triệu exp, %}          [event\jiefang_jieri\200904\shuizei\shuizei.lua:7-14]
TTPLD_SO_CHANNGUYEN  = 5    -- 4847 (vá 28/08)                                             [shuizei.lua:81-82]
TTPLD_LUOT_NGAY      = 1                                                                   [event\jiefang_jieri\200904\taskctrl.lua:46-50]
TNLB_AI_VUOTAI       = 20   -- Thập Niên Lệnh Bài, ải Vượt Ải cần qua                      [vng_event\thapnienlenhbai\lenhbai_def.lua:23]
TNLB_AI_VIEMDE       = 6                                                                   [lenhbai_def.lua:24]
TNLB_O_TRONG         = 30                                                                  [vng_event\thapnienlenhbai\mainfuc.lua:111]
HKA_TILE_THANHCONG   = {35,45,55,65,75}  -- Hoàng Kim Ấn, cấp 1->2 ... 5->6                [event\equip_publish\wuxingyin\wuxingyin.lua:13-19]
HKA_PHU_LIEU         = {6,1,4889}  -- Vương Thiết Tượng Lệnh Phù                           [wuxingyin.lua:57]
HKA_HAN_CAP_THAP     = 7    -- ngày, cấp 2..5                                              [wuxingyin.lua:67-77]
HKA_HAN_CAP_6        = 30   -- ngày                                                        [wuxingyin.lua:67-77]
HD3_VA_THUONG_MOC_AI = {{15,2},{28,2}}  -- Bảo Rương Vượt ải 6,1,3360                      [missions\challengeoftime\award.lua:88-93]
HD3_VA_THUONG_TIEUNIE= {ruong=1, exp=10000000}                                             [award.lua:116-122]
HD3_VA_DAI_AI_TIEUNIE= {5,15}  -- random(5,15)                                             [missions\challengeoftime\timer_match.lua:35-36]
HD3_VA_SO_HANG       = 5    -- số hạng hiển thị                                            [missions\challengeoftime\rank_perday.lua:79-82]
VA30_SO_AI_PHU       = 10   -- AWARD_COUNT                                                 [missions\challengeoftime\include.lua:19, chuangguang30.lua:169]
VA30_THUONG_P1       = {{6,1,4429,20},{6,1,4428,10}}  -- khoáng                            [missions\challengeoftime\chuangguang30.lua:38-42]
VA30_THUONG_P2       = {{4813,30},{214,50,40},{3,15},{6,15}}  -- dược phẩm                 [chuangguang30.lua:43-48]
VA30_THUONG_P3       = {{30533,0.3},{4866,5,6},{30009,0.4},{30010,0.4}}  -- đồ phổ hiếm    [chuangguang30.lua:49-54]
```

---

# TỆP 3 — `ch_exp.lua` (KINH NGHIỆM + EXP KỸ NĂNG)

## ⭐ Ưu tiên 1 — Hệ số nền toàn cục (một số này nhân vào rất nhiều nơi)

```
GLB_EXP_RATE         = 20   -- nhân vào PLDV/VAV/BST/BHK/Hoa Đăng... 🔴 KHÁC ExpRate ini   [lib\lib_server.lua:7]
GLB_MONEY_RATE       = 1    -- dùng ở lib câu hỏi + Hoa Đăng                               [lib\lib_server.lua:6]
GLB_SERVER_TEST      = 1    -- 🔴 ĐANG BẬT: mở mục admin cho MỌI người (cảnh báo A8)       [lib\lib_server.lua:5, tinhnang\phonglangdo\thuyenphu.lua:21-23, global\npcchucnang\nhieptran.lua:43-46]
GLB_DAMAGE_UPPER_BOSS= 1    -- boss tiểu HK                                                [lib\lib_server.lua:8, tinhnang\boss_hoangkim\deathhktieu.lua:58-72]
GLB_DAMAGE_UP_TTPLD  = 1    -- quái thuyền PLD                                             [lib\lib_server.lua:9, tinhnang\phonglangdo\quaipld.lua:63-93]
GLB_DAMAGE_UPPER_TTDL= 1    -- boss thuỷ tặc đầu lĩnh                                      [lib\lib_server.lua:10, bossthuytacdaulinhpld.lua:111-141]
GLB_STRONGBOSS_ST    = 1    -- boss sát thủ                                                [lib\lib_server.lua:11, tinhnang\boss_satthu\lib_boss_st.lua:36]
GLB_STRONGBOSS_VA    = 1    -- boss + npc vượt ải                                          [lib\lib_server.lua:12, tinhnang\vuot_ai\lib_vuotai.lua:210, lib_vuotai.lua:229]
GLB_STRONGBOSS_NSTK  = 1    -- Nguyên Soái Tống Kim                                        [lib\lib_server.lua:13, tinhnang\tong_kim_tcap\lib_tktc.lua:67]
GLB_MAX_MAGIC_LEVEL  = 10                                                                  [lib\lib_server.lua:14]
```

## ⭐ Ưu tiên 2 — TRẦN KINH TẾ ẨN (chủ game rất dễ bỏ sót)

```
BRXP_TRAN_CS4        = 50   -- trần exp/ngày TỪ MỌI BẢO RƯƠNG, chuyển sinh <=4 (triệu)     [vng_event\change_request_baoruong\exp_award.lua:70-77]
BRXP_TRAN_CS5        = 80   -- chuyển sinh = 5                                             [exp_award.lua:70-77]
BRXP_TRAN_CS6        = 100  -- chuyển sinh > 5                                             [exp_award.lua:70-77]
                     -- áp cho 9 loại rương, gọi từ 4 nơi:
                     --   [item\xinshirenwu\xinshibaoxiang.lua:2, missions\challengeoftime\item\chuangguanbaoxiang.lua:6,
                     --    missions\fengling_ferry\hd3_baoruongthuytac.lua:10, missions\yandibaozang\item\yandimibao.lua:5]
BU_EXP_MOI_NGAY      = {PLD=1000000000, VA=1000000000, VT=1000000000, HangNgay=1000000000, Vip=1000000000, DT=10000000}
                            -- exp quy đổi 1 ngày thiếu khi bù lượt                        [lib\lib_ham.lua:296-305]
BU_BANG_DINHMUC      = {PLD=2, VA=2, VT=2, HangNgay=1, Vip=1, DT=40}
                            -- 🔴 VA=2 LỆCH với HD3_VA_LUOT_NGAY=1 (cảnh báo A9)           [lib\lib_ham.lua:296-305, header\cauhinh_hoatdong.lua HD3_VA_LUOT_NGAY]
```

## Ưu tiên 3 — EXP theo hoạt động

```
TK_EXP_TOP10         = 100000000  -- nTongExp = 100tr * nXutk (thực tế luôn x3 = 300tr)    [timertask\task03.lua:92]
LDHC_EXP_THAMGIA     = 50000000   -- (xem ch_thuong)                                       [timerserver.lua:314-321]
LDHC_EXP_QUANQUAN    = 500000000                                                           [timerserver.lua:368-372, timerserver.lua:456-460]
PLDV_EXP_QUAI        = 100000     -- EXPQUAITHUONG (đồng đội 1/2, x2 nếu NpcExpRate>100)   [tinhnang\phonglangdo\lib_phonglangdo.lua:11, quaipld.lua:21, quaipld.lua:48-50]
PLDV_EXP_BOSS        = 50000000   -- EXP_BOSS_TTDL (đại boss x10)                          [lib_phonglangdo.lua:12, bossthuytacdaulinhpld.lua:21-24]
PLDV_EXP_TRA_NV      = 50000000   -- 🔴 x EXP_RATE = 1 TỶ/lần (cảnh báo B2)                [lib_phonglangdo.lua:14, thuyenphubac.lua:47]
PLDV_EXP_CAPBEN      = 0          -- EXP_CAPBEN_BACPLD, đang 0 = không phát                [lib_phonglangdo.lua:15, timertask\task05.lua:58]
VAV_EXP_MOI_AI       = 50000      -- ExpQuaAi = 50000 * ải kế tiếp                         [tinhnang\vuot_ai\die_normal.lua:35-46]
VAV_EXP_QUAI         = {30000,35000,40000,45000,50000}  -- NPC_VUOTAI cột 4, x EXP_RATE, boss x4 [tinhnang\vuot_ai\lib_vuotai.lua:143-169, die_normal.lua:13, die_normal.lua:63-85]
VAV_EXP_HOANTHANH    = 1600000    -- EXP_BOSS_LAST_D, x EXP_RATE = 32tr                    [lib_vuotai.lua:16, timertask\task06.lua:52]
BHKT_EXP_GIET        = 20000000   -- KILLBOSSEXPAWARD, x EXP_RATE. 🔴 TÊN TRÙNG 4 TỆP      [tinhnang\boss_hoangkim\deathhktieu.lua:9]
BHKT_EXP_LANCAN      = 10000000                                                            [deathhktieu.lua:10]
BHKT_BANKINH_EXP     = 200        -- PHAMVI_HUONGEXP                                       [deathhktieu.lua:11]
BHKD_EXP_GIET        = 25000000                                                            [tinhnang\boss_hoangkim\deathhkdai.lua:9]
BHKD_EXP_LANCAN      = 10000000                                                            [deathhkdai.lua:10]
BHKD_BANKINH_EXP     = 200                                                                 [deathhkdai.lua:11]
BHKPLD_EXP_GIET      = 50000000                                                            [tinhnang\boss_hoangkim\deathbosspld.lua:9]
BHKPLD_EXP_LANCAN    = 20000000                                                            [deathbosspld.lua:10]
BHKPLD_BANKINH_EXP   = 200                                                                 [deathbosspld.lua:11]
BHKSV_EXP_MOTMINH    = 500000000  -- 🔴 x nVIP; nVIP=0 -> exp = 0 (cảnh báo B3)            [tinhnang\boss_hoangkim\deathhksv.lua:87]
BHKSV_EXP_TODOI      = 300000000  -- 🔴 KHÔNG kiểm map/khoảng cách                         [deathhksv.lua:113, deathhksv.lua:112]
BHKSV_EXP_LANCAN_1   = 200000000                                                           [deathhksv.lua:101]
BHKSV_EXP_LANCAN_2   = 100000000                                                           [deathhksv.lua:116]
BHKSV_TASK_VIP       = 378        -- TASK_NEWTHOREN7                                       [deathhksv.lua:43, lib\lib_task.lua:226]
BHKSV_BANKINH_X      = 40                                                                  [deathhksv.lua:100]
BHKSV_BANKINH_Y      = 80                                                                  [deathhksv.lua:115]
BHKSV_EXP_RUONG_HESO = 2          -- GetLevel()^3 * 2                                      [tinhnang\boss_hoangkim\ruong.lua:33]
BST_EXP              = 300000     -- SATTHU_EXP, x EXP_RATE = 6tr                          [tinhnang\boss_satthu\drop.lua:11]
HH_EXP_TIEU          = 20000000   -- EXP_QHH_T                                             [event\event_huyhoang_dungdb\lib_huyhoang.lua:14]
HH_EXP_TRUNG         = 30000000   -- EXP_QHH_TR                                            [lib_huyhoang.lua:15]
HH_EXP_CAO           = 50000000   -- EXP_QHH_C                                             [lib_huyhoang.lua:16]
HH_EXP_HOANGKIM      = 100000000  -- EXP_QHK                                               [lib_huyhoang.lua:17]
HH_TIEN_HOANGKIM     = 500000     -- TIENVAN_QHK                                           [lib_huyhoang.lua:18]
HDANG_BANG_EXP       = <7 bậc 100000..700000 x EXP_RATE>  -- 🔴 BẢNG CHẾT (cảnh báo C2)    [event\event_cauhoi\lib.lua:9-17, event\event_cauhoi\cauhoi.lua:91-102]
HDANG_EXP_THUONG     = 2000000    -- x EXP_RATE, nhận ở Lễ Quan                            [event\event_cauhoi\lib.lua:25]
DT_HESO_TIEN         = {0.1,0.2,0.05,1.15,60,100}  -- công thức tiền Dã Tẩu                [task\newtask\tasklink\tasklink_award.lua:72, tasklink_award.lua:76]
DT_HESO_EXP          = {0.1,0.2,0.36,80,120}       -- công thức exp Dã Tẩu                 [tasklink_award.lua:96, tasklink_award.lua:99]
DT_MET_CHIA          = 1000       -- mệt mỏi=2: exp -> điểm tích luỹ (6,1,1475)            [tasklink_award.lua:108-118]
BDH_SM_THUONG_C1..C5 = <5 bảng exp người/bạn theo bước>                                    [task\partner\task_award.lua:53-106, :110-143, :147-197, :201-251, :255-263]
BDH_HT_THUONG_C1/C3/C4 = <3 bảng exp hồi tưởng, bọc CountDoubleMode>                       [task_award.lua:272-324, :328-379, :383-434]
BDH_TD_BANG_EXP      = {3000,32000,100000,180000,250000,350000,500000,1000000,2000000,3000000}
                            -- 🔴 BẢNG VÔ NGHĨA vì lỗi biến ở :123 (cảnh báo B4)           [task\partner\train\partner_jitan.lua:60-71, partner_jitan.lua:123]
HD3_VA_HESO_EXP      = 10000      -- point = experience * 10000                            [missions\challengeoftime\award.lua:191]
HD3_VA_BONUS_DOITRUONG = 1.2                                                               [award.lua:204]
HD3_VA_HESO_BAC2     = 2          -- nhân đôi cho bậc cao cấp                              [award.lua:207]
HD3_ST_EXP_SKILL120  = 140000     -- exp kỹ năng 120 khi giết boss cấp 90 (chưa có khóa)   [task\tollgate\killer\lib_killlevel.lua:127]
TS_TRAN_DIEM_NGAY    = 3500       -- trần điểm Tín Sứ/ngày (chưa có khóa)                  [task\tollgate\messenger\lib_messenger.lua:104-107]
TS_TRAN_DIEM_SUKIEN  = 7000       -- khi có Hoàng Chi Trượng bậc 2/3                       [lib_messenger.lua:104-107]
TS_MOC_THANGCAP      = {50,150,450,800,1500}  -- Mộc/Đồng/Bạc/Vàng/Ngự Tứ (chưa có khóa)   [task\tollgate\messenger\posthouse.lua:277, :296, :315, :334, :353]
TNLB_EXP_TRAN        = 20000000   -- trần exp hỗ trợ mỗi lần                               [vng_event\thapnienlenhbai\lenhbai_def.lua:25]
TNLB_EXP_LAN         = 5000000                                                             [lenhbai_def.lua:26]
BDCG_HAN_DUNG        = 30         -- ngày chờ giữa 2 lần Hoàng Chân Đơn (30*60*24*60 giây) [event\BeiDouChuanGong\head.lua:10, item\huangzhendan.lua:19, huangzhendan.lua:59]
```

## Ưu tiên 4 — EXP mức ENGINE (settings, phải RESTART GameServer)

```
SVC_HESO_EXP         = 1    -- [ENGINE] ExpRate. 🔴 KHÁC GLB_EXP_RATE=20                   [settings\gamesetting.ini:243, Core\Src\KCore.cpp:752, KPlayer.cpp:2582, KPlayer.cpp:2589]
SVC_HESO_TIEN        = 1    -- [ENGINE] MoneyRate                                          [settings\gamesetting.ini:244, KCore.cpp:753, KNpc.cpp:8567-8568]
SVC_HESO_KN90        = 10   -- [ENGINE] Skill90Rate                                        [settings\gamesetting.ini:245, KCore.cpp:754, KNpc.cpp:4024, KNpc.cpp:4029]
SVC_HESO_KN120       = 10   -- [ENGINE] Skill120Rate                                       [settings\gamesetting.ini:246, KCore.cpp:755, KNpc.cpp:4006, KNpc.cpp:4011]
```
> Nhóm `[Exp]` 23 khóa (`SVE_*`) → xem mục **can_build_lai**.

---

# TỆP 4 — `ch_drop.lua` (RỚT ĐỒ)

## ⭐ Ưu tiên 1 — Rớt đồ quái thường (chi phối MỌI bản đồ)

```
DRQ_TEP              = "\script\global\LuaNpcMonsters\Droprate_normal.lua"
                            -- [ENGINE] NormalDropRate                                     [settings\gamesetting.ini:265, Core\Src\KNpcSet.cpp:511-513]
DRQ_TEP_LUA          = "\script\global\luanpcmonsters\droprate_normal.lua"  -- biến DROPFILE khi tự sinh NPC [lib\lib_map.lua:44]
DRQ_HESO_TIEN        = 1    -- nXMoney                                                     [global\LuaNpcMonsters\Droprate_normal.lua:15]
DRQ_NGUONG_TIEN      = 2    -- random(0,30) == 2 thì rơi tiền                              [Droprate_normal.lua:113-118]
DRQ_NGUONG_ITEM      = 4    -- random(0,30) < 4 thì quay droprate 1 lần                    [Droprate_normal.lua:113-118]
DRQ_QUAY_BOSSXANH    = 8    -- số lần quay cho boss xanh                                   [Droprate_normal.lua:100]
DRQ_NGUONG_TIEN_BOSSXANH = 8  -- random(0,10) > 8 (~18%)                                   [Droprate_normal.lua:101-104]
DRQ_CAP_SUKIEN       = 7    -- quái bậc > 7 mới gọi dropeventmap                           [Droprate_normal.lua:119-124]
DRQ_NGUONG_SUKIEN    = 2    -- random(0,4) < 2 (40%)                                       [Droprate_normal.lua:119-124]
DRQ_BANG_BAC         = <9 bậc: tiền + npcdropratexx.ini + goldennpc\npcdropratexx.ini>     [Droprate_normal.lua:32-95]
DRQ_BAT_HCNT         = 0    -- khối rớt Hiệp Cốt Nhu Tình từ boss xanh ĐANG COMMENT        [Droprate_normal.lua:105-109]
```

## ⭐ Ưu tiên 2 — Rớt đồ sự kiện dùng chung (99% / 79% — tỉ lệ RẤT cao)

```
SKD_RANDMAP          = 100  -- mẫu số cho dropeventmap/HCNT/HMD/manhhkmp                   [lib\lib_sukien.lua:10]
SKD_RANDOTHER        = 10   -- mẫu số cho dropeventboss/dropother/dropntiendong            [lib\lib_sukien.lua:12]
SKD_DROPMDTB         = 95   -- mẫu số cho dropnvdt/dropnvdt01 (mã chết)                    [lib\lib_sukien.lua:15]
SKD_MAP_NGUONG       = 80   -- random(1,100) > 80 -> 20% rơi item 4854                     [lib\lib_sukien.lua:21-26]
SKD_MAP_ITEM         = 4854                                                                [lib_sukien.lua:21-26]
SKD_HCNT_NGUONG      = 50   -- 50% rớt trang bị Hiệp Cốt Nhu Tình                          [lib_sukien.lua:28-34]
SKD_HCNT_MA_MIN      = 185                                                                 [lib_sukien.lua:28-34]
SKD_HCNT_MA_MAX      = 192                                                                 [lib_sukien.lua:28-34]
SKD_PUBG_MA_MIN      = 4851 -- dropeventPUBG luôn rơi 1                                    [lib_sukien.lua:36-39]
SKD_PUBG_MA_MAX      = 4853                                                                [lib_sukien.lua:36-39]
SKD_HKMP_MA_MIN      = 0    -- droptrangbihkmp LUÔN rơi 1 trang bị HKMP                    [lib_sukien.lua:41-44, tinhnang\boss_hoangkim\drophksv.lua:67]
SKD_HKMP_MA_MAX      = 139                                                                 [lib_sukien.lua:41-44]
SKD_HKMP_HAN_NGAY    = 7                                                                   [lib_sukien.lua:41-44]
SKD_HMD_NGUONG       = 1    -- 🔴 99% rơi                                                  [lib_sukien.lua:47-54]
SKD_HMD_SOLUONG      = 10   -- 10 cái Huy Hoàng Đơn 4844 mỗi lần                           [lib_sukien.lua:47-54]
SKD_MANHHKMP_NGUONG  = 80   -- 🔴 79% rơi 1 mảnh đồ phổ HKMP                               [lib_sukien.lua:56-64]
SKD_MANHHKMP_BANG    = <24 mã, có 348 và 2433 mà bảng của death*.lua KHÔNG có>             [lib_sukien.lua:56-64]
SKD_BOSS_NGUONG      = 1    -- 90% rơi 1 trang bị random(753,770)                          [lib_sukien.lua:72-87]
SKD_BOSS_MA_MIN      = 753                                                                 [lib_sukien.lua:72-87]
SKD_BOSS_MA_MAX      = 770                                                                 [lib_sukien.lua:72-87]
SKD_TIENDONG_NGUONG  = 1    -- 90% rơi Tiền Đồng 4835                                      [lib_sukien.lua:97-105]
SKD_TIENDONG_SL_MIN  = 1                                                                   [lib_sukien.lua:97-105]
SKD_TIENDONG_SL_MAX  = 3                                                                   [lib_sukien.lua:97-105]
```
> **Áp vào:** `drophktieu.lua:64-65`, `drophkdai.lua:65,:68`, `drophksv.lua:66`, `dropbosspld.lua:65-66`, `tinhnang\phonglangdo\drop.lua:92-93, :130-131`.

## Ưu tiên 3 — Bảng rớt theo hoạt động

```
BHK_FILE_DROPRATE    = "\settings\droprate\goldennpc\npcdroprate90.ini"
                            -- 🔴 CẢ 6 TỆP DROP BOSS DÙNG CHUNG                            [tinhnang\boss_hoangkim\drophktieu.lua:31, drophkdai.lua:33, drophksv.lua:33, dropbosspld.lua:33, bosslbdrop.lua:33, tinhnang\boss_satthu\drop.lua:35]
BHK_SO_LAN_QUAY      = 10   -- tham số 2 của DropRateItem, cả 6 tệp                        [drophktieu.lua:33, drophkdai.lua:35, drophksv.lua:35, dropbosspld.lua:35, bosslbdrop.lua:35, tinhnang\boss_satthu\drop.lua:48]
BHKT_BANG_QUA        = <random(1,100): 10 VLMT; 11 TTK; 12 THBT; 13/14/15 Thuỷ Tinh; 16/17/18; 19 Hoa Hồng; >90 Túi Bí Kíp 4815; <70 Lệnh Bài Dã Tẩu 4818>  [tinhnang\boss_hoangkim\drophktieu.lua:36-61]
BHKD_BANG_QUA        = <random(1,100): <10 VLMT; 11 TTK; 12 THBT; 13/14/15 Thuỷ Tinh; 16..19; 40 Túi Bí Kíp; >90 ngựa 9x>  -- 🔴 GIỐNG HỆT BHKSV_ và BHKPLD_ [drophkdai.lua:38-64, drophksv.lua:38-63, dropbosspld.lua:38-64]
BHKD_HAN_NGUA_NGAY   = 7    -- AddTimeItem(60*60*24*7)                                     [drophkdai.lua:63]
TK_TILE_DROP         = {28,8,4}  -- random(0,30): >28 phong cụ 174-176; ==8 -> 155-157; ==4 -> bảo vật 206-210  [tinhnang\tong_kim_tcap\drop.lua:22-36]
TK_DROP_HESO         = 3    -- dropNum = npcValue*3 (cấp 2..5)                             [tinhnang\tong_kim_tcap\drop.lua:22-36]
PLDV_TILE_TRUYCONG   = 95   -- Truy Công Lệnh 6,1,2024: random(1,100)>=95, chỉ giờ tốn phí [tinhnang\phonglangdo\drop.lua:46-50]
PLDV_SO_LUOT_DROPTABLE = {1,10}  -- quái thường 1 lượt @>=95; boss 10 lượt                 [tinhnang\phonglangdo\drop.lua:35-41]
PLDV_TILE_NGUHOA     = {80,85,20}  -- 🔴 HAI KHỐI CHỒNG NHAU (cảnh báo B5)                 [tinhnang\phonglangdo\drop.lua:51-56]
PLDV_THUONG_DROP_DAIBOSS = <trần 6 lượt; 121/122/123 chắc chắn + bảng random(1,100)>       [tinhnang\phonglangdo\drop.lua:80-119]
PLDV_THUONG_DROP_BOSS    = <trần 3 lượt; bảng random(1,100)>                               [tinhnang\phonglangdo\drop.lua:121-148]
HD3_ST_TILE_BIBAO    = 50   -- Sát Thủ Bí Bảo 6,1,2356 (chưa có khóa)                      [task\tollgate\killer\lib_killlevel.lua:130]
HD3_VA_THUONG_QUAY   = <47 mục, mẫu số 100000>  -- map_random_awards (chưa có khóa)        [missions\challengeoftime\award.lua:9-57]
HD3_VA_TILE_ROI_1401 = 5    -- 5% rơi thêm item 1401 khi giết NPC ải                       [award.lua:233-234]
BST_TILE_DROP        = 10   -- số lần quay (drop.lua:48)                                   [tinhnang\boss_satthu\drop.lua:48]
DRP_*                = <Droprate_pubg.lua — CHẾ ĐỘ SINH TỒN ĐANG TẮT>  -- 🔴 THIẾU 4 TỆP INI (cảnh báo B6)  [global\LuaNpcMonsters\Droprate_pubg.lua:37, :43, :49, :61]
```

---

# TỆP 5 — `ch_chung.lua` (CÒN LẠI)

## ⭐ Ưu tiên 1 — Điều kiện tham gia (cấp / số người / trần)

```
TK_CAP_TOITHIEU      = 80   -- LEVEL_ENOUGH_TK                                             [tinhnang\tong_kim_tcap\lib_tktc.lua:19, mobinhtk.lua:141, mobinhtk.lua:253]
TK_CHENH_QUANSO      = 20   -- MAX_PLAYER_CL (chú thích ghi 5 - SAI)                       [lib_tktc.lua:20, mobinhtk.lua:132, mobinhtk.lua:244]
TK_NGUOI_MOI_PHE     = 2000 -- PLAYER_MS_LIMIT (chú thích ghi 55 - SAI)                    [lib_tktc.lua:55, mobinhtk.lua:125, mobinhtk.lua:237]
TK_PHI_BAODANH       = 20000 -- 🔴 KHÔNG TÁC DỤNG, mọi Pay() đã comment                    [lib_tktc.lua:21, mobinhtk.lua:160, :171-174, :181, :273, :284-287, :298]
TK_MAP_BAODANH       = 324  -- 🔴 BẢN SAO ở C++ (cảnh báo A10)                             [lib_tktc.lua:53, Core\Src\KTongKimTables.h:8]
TK_MAP_TRAN          = 379  -- 🔴 BẢN SAO ở C++                                            [lib_tktc.lua:54, Core\Src\KTongKimTables.h:9]
TK_HP_NGUYENSOAI     = 5000000  -- x STRONGBOSS_NSTK                                       [lib_tktc.lua:67]
TK_HP_QUAI           = {36000,70000,150000,250000,350000}  -- 5 loại quái mỗi phe          [lib_tktc.lua:513-570]

CTC_NGUOI_TOIDA_THU        = 200  -- MAX_CAMP1COUNT. 🔴 TRÙNG TÊN với CTCV_ (50)           [missions\citywar_city\head.lua:65-68, tinhnang\congthanhchien\lib_ctc.lua:90-93]
CTC_NGUOI_TOIDA_CONG       = 200  -- MAX_CAMP2COUNT (CTCV_ = 50)                           [citywar_city\head.lua:65-68, lib_ctc.lua:90-93]
CTC_NGUOI_TOIDA_NGHIASI_THU  = 50 -- MAX_CAMP3COUNT (CTCV_ = 5)                            [citywar_city\head.lua:65-68, lib_ctc.lua:90-93]
CTC_NGUOI_TOIDA_NGHIASI_CONG = 50 -- MAX_CAMP4COUNT (CTCV_ = 5)                            [citywar_city\head.lua:65-68, lib_ctc.lua:90-93]
CTC_SO_LONGTRU       = 3    -- MS_SYMBOLCOUNT, thắng khi chiếm > 1/2                       [citywar_city\head.lua:35, missions\citywar_city\totaltimer.lua:17]
CTC_SO_CONG          = 3    -- g_nDoorCount                                                [citywar_city\head.lua:43-51]
CTC_SO_XE_MOI_CONG   = 3    -- g_nMaxTscPerDoor                                            [citywar_city\head.lua:43-51]
CTC_CAP_BIA_THU      = 10   -- STONELEVEL1                                                 [citywar_city\head.lua:76-80]
CTC_CAP_BIA_CONG     = 20   -- STONELEVEL2                                                 [citywar_city\head.lua:76-80]
CTC_CAP_CONG         = 60   -- DOORLEVEL                                                   [citywar_city\head.lua:76-80]
CTC_CANBANG_MAM      = 15   -- BALANCE_MAMCOUNT                                            [citywar_city\head.lua:168]
CTC_SKILL_QUANHAM    = 661  -- RANK_SKILL (AddSkillState ĐÃ COMMENT ở :408)                [citywar_city\head.lua:170]
CTC_CAP_BANG_TOITHIEU = 18  -- [ENGINE trùng] MinTongLevel                                 [missions\citywar_global\infocenter_head.lua:724, settings\citywar.ini:72, Core\Src\KJx2CityWar.cpp:1101-1103]
CTC_SO_THANHVIEN_TOITHIEU = 37  -- [ENGINE] MinTongCrowNumber                              [settings\citywar.ini:81, KJx2CityWar.cpp:1095-1097]
CTC_OTRONG_TOITHIEU  = 20                                                                  [citywar_function.lua:183]
CTC_ITEM_KHIEUCHIEN  = 1508 -- nCityWar_Item_ID_P (gốc JX2 = 1499)                         [missions\citywar_global\infocenter_head.lua:29]
CTC_BANG_LENHBAI     = {363,362,355,354,367,366,359,358,357,356,365,364,361,360}  -- CardTab 14 mã [missions\citywar_global\head.lua:4-17]
CTC_GIA_DAOCU        = 343  -- 1 Kim Nguyên Bảo -> item 6,1,29..32                         [citywar_function.lua:44-46]
CTLD_NGUOI_TOIDA     = 16   -- MAX_MEMBER_COUNT. 🔴 KHAI 2 NƠI                             [missions\citywar_arena\head.lua:14, tinhnang\loidai\lib_loidai.lua:30]
CTLD_SO_DAI          = 8    -- ARENACOUNT, map 213..220                                    [missions\citywar_global\timer.lua:3-6]
CTC_SO_THANH         = 7    -- CITYCOUNT, CITYWARIDX=221                                   [citywar_global\timer.lua:3-6]

LDHC_CAP_TOITHIEU    = 90   -- hardcode trong `if`                                         [tinhnang\loidaihonchien\mainloidai.lua:21-24]
LDHC_MAP             = 210  -- sân đấu (trước là 209, đã nhường cho Tỷ Võ)                 [mainloidai.lua:46]
PLDV_CAP_TOITHIEU    = —    -- (không có; sức chứa thuyền = PLAYER_LIMIT_MS)
PLDV_SUC_CHUA        = 100  -- PLAYER_LIMIT_MS                                             [tinhnang\phonglangdo\lib_phonglangdo.lua:24, thuyenphu.lua:102]
PLDV_HP_QUAI         = 100000    -- LIFE_THUYTAC                                           [lib_phonglangdo.lua:25]
PLDV_HP_BOSS         = 8000000   -- LIFE_BOSS_THUYTAC (đại boss x6)                        [lib_phonglangdo.lua:26, lib_phonglangdo.lua:196-197]
PLDV_MAP             = {337,338,339}  -- 🔴 THỰC TẾ chỉ 337 chạy (:167-176 comment)        [lib_phonglangdo.lua:108-112]
VAV_CAP_TOITHIEU     = 90   -- LEVELENOUGH. 🔴 KHAI 2 TỆP                                  [global\npcchucnang\nhieptran.lua:20, tinhnang\vuot_ai\sugiasatthu.lua:20]
VAV_NGUOI_TOITHIEU   = 5    -- MIN_PLAYERINVUOTAI (trần 8 do vòng i=0..7)                  [nhieptran.lua:15, sugiasatthu.lua:15, nhieptran.lua:261]
VAV_LUOT_NGAY        = 2    -- MAX_NUM_VA_DAY                                              [tinhnang\vuot_ai\lib_vuotai.lua:18, nhieptran.lua:274]
VAV_LUOT_LONGHUYETHOAN_NGAY = 1                                                            [lib_vuotai.lua:19, item\longxuewan.lua:27]
VAV_MAP              = {480,481,482,483,484,485,486,487,488,489}                           [lib_vuotai.lua:33-44]
VAV_HP_QUAI          = 100000  -- lifemt x STRONGBOSS_VA                                   [lib_vuotai.lua:210]
VAV_BANG_BOSS        = <12 boss, HP = (300000 + nBoss*10000) x STRONGBOSS_VA>              [lib_vuotai.lua:172-185, lib_vuotai.lua:229]
VAV_BOSS_CUOI        = {hp=800000, dmg={500,1000}, hoiphuc=500, chinhxac=50, ne=20}        [lib_vuotai.lua:197-201]
LDBH_NGUOI_TOIDA     = 16   -- = CTLD_NGUOI_TOIDA (cùng tên MAX_MEMBER_COUNT)              [tinhnang\loidai\lib_loidai.lua:30]
CTCV_CAP_TOITHIEU    = 80   -- ENOUGH_LEVEL                                                [tinhnang\congthanhchien\lib_ctc.lua:82]
CTCV_HP_TRU          = 500000  -- Long Trụ nguyên                                          [tinhnang\congthanhchien\death_tru.lua:27, :34, :61, :68]
CTCV_HP_TRU_NUT      = 250000                                                              [death_tru.lua:27, :34, :61, :68]
CTCV_HP_CONG         = 500000  -- máu cổng thành                                           [lib_ctc.lua:190]
KMQ_MAP              = {995}   -- 🔴 DÙNG CHUNG với Hạt Huy Hoàng (cảnh báo C1)            [event\kiemmonquan\lib_kmq.lua:14-16, event\event_huyhoang_dungdb\lib_huyhoang.lua:36-56]
KMQ_THANH_VAO        = {{78,"Tương Dương"},{80,"Dương Châu"},{162,"Đại Lý"}}                [lib_kmq.lua:71-80]
TBH_CAP_TOITHIEU     = 80   -- 🔴 LIMIT_LEVEL_TBH KHÔNG AI ĐỌC; hardcode 80 hai chỗ        [event\trongbanghoi\lib.lua:17, event\trongbanghoi\drum.lua:23, tungtung1.lua:18]
TBH_NPC_MAP          = 37   -- 🔴 map trống (Biện Kinh) KHÁC map dùng item = 54            [event\trongbanghoi\lib.lua:27, event\trongbanghoi\drum.lua:43]
HDANG_CAP_TOITHIEU   = 80   -- MIN_LEVEL_JOIN                                              [event\event_cauhoi\lib.lua:19]
HDANG_SO_CAU_NGAY    = 5    -- MAX_NUM                                                     [event\event_cauhoi\lib.lua:22]
HDANG_SO_CAU_DUNG    = 3    -- NUM_NHANTHUONG                                              [event\event_cauhoi\lib.lua:23]
VT_CAP_TOITHIEU      = 50   -- LEVEL_LIMIT_VT                                              [event\event_vantieu\lib_vt.lua:100]
BST_CAP_TOITHIEU     = 90   -- LEVELENOUGH (dùng chung với VAV_)                           [nhieptran.lua:20, sugiasatthu.lua:20]
BST_MA_LENH          = {6,1,398}  -- ITEM_STL                                              [nhieptran.lua:22, sugiasatthu.lua:22]
BST_MA_GIAN          = {6,1,399}  -- ITEM_STG                                              [nhieptran.lua:23, sugiasatthu.lua:23]
BST_MAU_GOC          = 500000  -- x STRONGBOSS_ST                                          [tinhnang\boss_satthu\lib_boss_st.lua:36]
BHK_HP_TIEU          = 18000000  -- LIFE_BOSS_TIEU (chú thích ghi 8000000)                 [tinhnang\boss_hoangkim\lib_bosshk.lua:24]
BHK_HP_DAI           = 1    -- 🔴 LIFE_BOSS_DAIH = 1 MÁU (cảnh báo B7) - dùng cho đại+máy chủ+PLD [lib_bosshk.lua:25, :95, :265, :372]
BHK_NETRANH          = 5000 -- NETRANHBOSS                                                 [lib_bosshk.lua:26]
BHK_PHSL             = 300  -- PHSLBOSS (đại x2, máy chủ x3)                               [lib_bosshk.lua:27, :265, :372]
BHKSV_GIAM_SATTHUONG = 70   -- SetNpcDamageReduction (cũng dùng cho PLD)                   [lib_bosshk.lua:268, lib_bosshk.lua:98]
HD3_VA_DAI_CAP       = {{50,90},{90,201}}  -- 🔴 GIỚI HẠN THẬT (HD3_VA_CAP_TOITHIEU chỉ là hiển thị) [missions\challengeoftime\include.lua:90-95, header\cauhinh_hoatdong.lua:368-370]
HD3_VA_MAP           = {{464..479},{480..495}}                                             [missions\challengeoftime\include.lua:97-110]
HD3_VA_DAI_NPC       = {{994-1001},{1002-1005},{1026-1031},{1035-1037}}                    [include.lua:113-124]
HD3_VA_PHI_BAODANH   = 10000                                                               [missions\challengeoftime\npc\dragonboat_main.lua:16-17]
HD3_ST_NHOM_THUONG   = {141,160}  -- chỉ nhóm boss cấp 90 còn phát thưởng                  [task\tollgate\killer\kill_level.lua:27, :69, :90]
TS_MAP_NHAN_NV       = {11,162}   -- các thành khác đã comment (chưa có khóa)              [task\tollgate\messenger\posthouse.lua:107, :192, :209, :164-215]
TS_SO_QUAI           = 30         -- (hoặc 25) số quái NV tiền bảo khố (chưa có khóa)      [posthouse.lua:393]
```

## Ưu tiên 2 — Cấu hình máy chủ mức ENGINE (đổi tệp + RESTART)

```
SVC_MAX_OPT_MULTIPLY = 1    -- [ENGINE] MaxOptMultiply                                     [settings\gamesetting.ini:260, Core\Src\KCore.cpp:749, Core\Src\KItemSet.h:59-60]
SVC_XMETHOD          = 1    -- [ENGINE] công thức nhân thuộc tính (0..4)                    [settings\gamesetting.ini:261, KCore.cpp:750, Core\Src\KItem.cpp:68]
SVC_LOG_NPC_SCRIPT   = 1    -- [ENGINE] WriteScriptNpcLog                                   [settings\gamesetting.ini:258, KCore.cpp:783, Core\Src\KObj.cpp:1238, KPlayer.cpp:8477]
SVC_KHONG_SINH_NPC_THUONG = 1  -- [ENGINE] NotAddNpcNormal                                  [settings\gamesetting.ini:259, KCore.cpp:781, Core\Src\KRegion.cpp:270, KRegion.cpp:483, KNpc.cpp:8485]
```

## Ưu tiên 3 — Cửa sổ ngày sự kiện (phần lớn đã hết hạn, chỉ gom nếu chủ muốn bật lại)

```
TTPLD_NGAY_MO        = 20090428  -- 🔴 CHẠY VĨNH VIỄN: hàm kiểm BỎ QUA ngày đóng           [event\jiefang_jieri\200904\taskctrl.lua:23, shuizei\shuizei.lua:107-110]
TTPLD_NGAY_DONG      = 20090601  -- khai nhưng KHÔNG dùng                                  [taskctrl.lua:24]
TKAL_NGAY_MO         = 20090428  -- Thiên Kim Anh Liệt (hệ số mặt nạ Tống Kim) - ĐÃ CHẾT   [event\jiefang_jieri\200904\qianqiu_yinglie\head.lua:15]
TKAL_NGAY_DONG       = 20090531  --                                                        [head.lua:16]
TKAL_HESO_MATNA      = {1.5,2,3} -- 🔴 điều chỉnh KINH TẾ Tống Kim đang bị khoá theo ngày  [qianqiu_yinglie\head.lua:56-89, battles\battlehead.lua:673-674, :681-682]
TKHT_NGAY_MO         = 201404070000  -- Nộp vật phẩm hỗ trợ Tống Kim (chết kép)            [vng_event\give_support_item.lua:27]
TKHT_NGAY_DONG       = 201404172400                                                        [give_support_item.lua:28]
TW_THDNB_NGAY_MO     = 20140407  -- báo danh THDNB (chết)                                  [event\tongwar\npc_shizhe.lua:16]
TW_THDNB_NGAY_DONG   = 20140417                                                            [npc_shizhe.lua:17]
TW_THDNB_TOIDA       = 54        -- ONETONG_MAX_SIGNMEM                                    [npc_shizhe.lua:22]
SN09_NGAY_MO/DONG    = 20090619 / 20090719  -- sinh nhật 2009 (chết, còn Include 10 nơi)   [event\birthday_jieri\200905\class.lua:14-16]
NTCM_NGAY_MO/DONG    = 20090828 / 20091011  -- Nghịch Thiên Cải Mệnh (chết)                [event\change_destiny\head.lua:7-8]
NNG09_NGAY_MO/DONG   = 20091113 / 20091202  -- Ngày Nhà Giáo 2009 (chết)                   [event\jiaoshi_jieri\200910\head.lua:3-4]
PHONGBA_NGAY_DONG    = 275       -- STORM_END_DAY. 🔴 BẢNG NĂM CHỈ TỚI 2010 (cảnh báo B8)  [event\storm\head.lua:64, event\storm\head.lua:37-52, head.lua:86]
HCC_HESO_THUONG      = 2         -- Hoàng Chi Chương nhân đôi (nhánh x3 cuối tuần comment) [event\great_night\huangzhizhang\event.lua:41-48]
```

---

# CAN_BUILD_LAI (phải sửa C++ / phải viết mã mới — KHÔNG để lẫn vào 5 tệp trên)

### C1. Bầu Cua — 100% cứng trong C++, không đọc tệp nào
```
BAU_GIAY_MOT_VAN  = 60         [Core\Src\BauCua.cpp:24, BauCua.cpp:243, BauCua.cpp:490]
BAU_NGUONG_LAMCAI = 100        [BauCua.cpp:25, :213, :269, :291]
BAU_TILE_NHACAI   = 10         [BauCua.cpp:26, :580-583]
BAU_VON_MOI       = 10000000   [BauCua.cpp:46, :587, :615]
```
> `[DiceGame]` trong `gamesetting.ini:182-206` là bảng CŨ **đã chết** (grep toàn Sources = 0) — đừng gom.

### C2. Trần hệ thống — INI nói dối, giá trị thật nằm trong header C++
`MaxSubWorldCount/MaxPlayerCount/MaxNpcCount/MaxItemCount/MaxObjCount/MaxMissleCount/MaxFreeLevel/NpcPoisonDamageMax/PlayerPoisonDamageMax/FreezeTimeReduceMax` (`settings\gamesetting.ini:247-257`) **không nơi nào đọc**. Thật: `MAX_SUBWORLD=1000` [`Core\Src\KSubWorld.h:5`], `MAX_PLAYER=1500` [`KPlayerDef.h:19`], `MAX_NPC=98000` [`KNpc.h:21`], `MAX_ITEM=1000000` [`KItem.h:30`], `MAX_OBJECT=10000` [`KObj.h:12`], `MAX_MISSLE=20000` [`KMissle.h:8`], `MAX_POISON_DAMAGE=200000` [`KNpc.h:35`] (**INI ghi 123456/600 — sai**), `MAX_REDUCE=75` [`GameDataDef.h:435`], `MAX_REPUTE_VALUE=100000` [`GameDataDef.h:61`] (**INI `[Repute]:178-180` ghi 32000 — sai**).

### C3. Nhóm `[Exp]` 23 khóa `SVE_*` — mã đã có, tệp INI CHƯA có (nửa vời)
`MocCap1=50, HeSo1=80, MocCap2=80, HeSo2=70, MocCap3=140, HeSo3=280, HeSo4=100, VipCong=20, CsMap=341, CsLanToiThieu=3, CsDuoi=160, CsTren=50, ChenhCapMax=9, ChiaKhiChenh=10, MienTruCap=90, ToDoi2=80, ToDoi3=70, ToDoi4=60, ToDoi5=55, ToDoi6=55, ToDoi7=50, ToDoi8=50, ToDoiKhac=60` [`Core\Src\KCore.cpp:758-780`, mặc định `KCore.cpp:137-159`, dùng `KPlayer.cpp:2458`, `:2522`, `:2529`, `:2538`].
> ⚠️ `Core\Src\KCore.cpp` **bị một phiên làm việc KHÁC sửa lúc 18:18 ngày 29/08** (khối `[CFGEXP 29/08]`/`[CFGEXP2 29/08]`) — số dòng có thể lệch. Khối đọc nằm trong `#ifdef _SERVER` nhưng 23 biến khai NGOÀI → **bản CLIENT không bao giờ đọc `[Exp]`**, ước lượng exp phía client sẽ lệch nếu server đổi khác mặc định.

### C4. Bảy tệp `.h` SINH TỰ ĐỘNG — bản sao Lua cho auto ngoài/bot
`KTongKimTables.h`, `KLienDauTables.h`, `KHoatDongTables.h`, `KDaTauTables.h`/`KDaTauSpots.h`, `KSatThuBossPos.h`, `KMapSuKien.h`, `KTuiDuocPham.h`.
**Đổi lịch/thoại bên Lua → PHẢI chạy lại bộ sinh (`ReverseTools\gen_*.py`) + build lại DLL**, nếu không auto/bot chạy sai giờ, mất nhận diện NPC. Xem 🔴A10.

### C5. Hằng auto/bot cứng trong C++
`ST_*` [`Core\Src\CoreShell.cpp:11334-11356`], `TK_GANTRAI=1440` [`CoreShell.cpp:7120`], `HD_LECH_GIO` [`CoreShell.cpp:9886-9891`, `:9913-9919`], `PB_*` [`Core\Src\KPlayerBot.cpp:326-331`, `:651-678`, `:1379`, `:4005`, `:4289`, `Core\Src\KPlayerBot.h:93,96,99`].

### C6. Danh sách trắng đường dẫn script vật phẩm
`Core\Src\KItemList.cpp:1945-1967` — thêm vật phẩm hoạt động kiểu Linux phải thêm dòng rồi build.

### C7. Phải VIẾT MÃ LUA MỚI (không phải build C++, nhưng không chỉ là thêm khóa)
- **`BHKPLD_GIO`**: Boss Hoàng Kim Phong Lăng Độ **chưa từng có bảng giờ** — phải tạo bảng + thêm vòng quét trong `timerserver.lua` (mẫu `:855-873`). Hiện chỉ chạy tay qua `item\lenhbaiadmin.lua:514`.
- **`TDV_NGAY_MO/DONG`**: Tết Đoàn Viên **không có mã kiểm ngày** ở đâu cả (`event\eventtet\npcsukien.lua:16` chỉ là chuỗi thoại) — phải thêm hàm kiểm.
- **`SKD_BAT`**: `EVENT_ACTIVE=1` [`lib\lib_sukien.lua:6`] là **công tắc giả** — muốn có công tắc thật phải thêm kiểm tra vào từng hàm `drop*`.
- **20 dòng `BAT_*`** ở đầu `ch_lich`: phải bọc `if HD_CFG(...)` quanh 20 lời gọi trong `timerserver.lua`.

### C8. Đổi là ĐỔI CÂN BẰNG — cần chủ game quyết trước
`GLB_EXP_RATE=20`, `BHK_HP_DAI=1`, `PLDV_EXP_TRA_NV` (1 tỷ/lần), `LDHC_EXP_MOI_MANG` (lỗ exp đang mở), `GLB_SERVER_TEST=1`.

### C9. TUYỆT ĐỐI KHÔNG tạo khóa Lua song song
`settings\PlayerTitle.txt` (danh hiệu + vòng sáng, tra theo **DÒNG = TitleId+1**) và `settings\HonorSetting.txt` — engine lấy trực tiếp [`Core\Src\KNpc.cpp:11310-11333`, `KNpc.cpp:11349-11366`, `KNpc.cpp:6302`, `:6547-6551`]. Thêm nguồn thứ hai = tự tạo lệch. Đã có ghi chú đúng ở `cauhinh_hoatdong.lua:78-95` — giữ nguyên tinh thần đó.

### C10. BẢNG nên giữ ở `settings\`, chỉ ghi ĐƯỜNG DẪN vào tệp cấu hình
`settings\task\award_basic.txt` (105 dòng) · `award_link.txt` (183) · `award_loop.txt` (12) · `levellink.txt` · `tasklink_mainlink.txt` · `partner\reward\reward_allprize.txt` (382) · `partner\reward\index_taskid.txt` (382) · `metempsychosis\translife.txt` · `tollgate\messenger\messenger_tollprize.txt` · `tollgate\messenger\tollgate_allprize.txt` · `tollgate\killer\killer.txt` (160) · `event\thapnienlenhbai\taskthapnien.txt` · `droprate\**`. Cũng nên tách riêng: `event\event_cauhoi\lib.lua:31-334` (~300 dòng toạ độ) + `:380-535` (155 câu hỏi), `tinhnang\datau\lib_datau.lua:312-697` (360 vật phẩm).

---

# CANH_BAO

## 🔴 A. Lỗi / bẫy phải báo chủ game NGAY

**A1. LỖ EXP ĐANG MỞ — Lôi Đài Hỗn Chiến.** NPC báo danh **vẫn sống** [`startgame\thon\balanghuyen.lua:84`] nhưng hàm điều phối **đã tắt** [`timerserver.lua:100`]. Người chơi vào map 210 lúc 16h/22h, giết nhau ăn **10 triệu exp/mạng** [`bigiet.lua:12`], nhưng lệnh reset `GetTaskTemp(1)` ở `timerserver.lua:312` **không bao giờ chạy** → trần 4 mạng vô hiệu, và không bao giờ được trả về thành. **Vá trước khi làm gì khác.**

**A2. LỖI LOGIC cửa sổ chốt quán quân.** `timerserver.lua:330` kiểm "còn 1 người sống → 500 triệu exp + 5 mảnh Hoàng Kim" trong dải phút **12..54**, tức **TRƯỚC** mốc khai chiến `:52`. Bật lại là chỉ cần 1 người đứng trong map 210 lúc phút 13 là ăn giải quán quân mà không cần đánh ai.

**A3. VẬN TIÊU — thưởng thật GẤP 10 LẦN thông báo.** `tieudau.lua:251` ghi "nhận 50.000.000" nhưng mã cộng `AddSumExp(500000000)`; tương tự `:260` (10tr ghi / 100tr cộng), `:269` (30tr/300tr), `:278` (50tr/500tr). Kèm **3 nguồn giờ lệch**: mã chặn 13h-23h [`lib_vt.lua:11`], thoại NPC "15h đến 23h" [`tieudau.lua:25`], loa server "15h đến 21h" [`timerserver.lua:626`].

**A4. `MSTIME_VUOT_AI_BD` bị dùng 2 vai.** `timerserver.lua:900` dùng làm số phút timer, `timerserver.lua:907` dùng chính nó làm **số phút so sánh** để bắn loa (`if nMi == MSTIME_VUOT_AI_BD`). Đổi 10→15 thì timer đúng nhưng loa bắn ở phút :15 — trùng may rủi.

**A5. LÔI ĐÀI BANG HỘI thu tiền cho trận không mở.** Driver `timerserver.lua:94` và NPC vào cửa `balanghuyen.lua:80` đều tắt, **NHƯNG** `RegisterLD` vẫn sống qua NPC Công Thành Quan [`congthanhquan.lua:25`] và **vẫn thu 1.000.000 lượng** của bang chủ.

**A6. SỐ 8 (lượt Sát Thủ/ngày) tồn tại BA BẢN.** `nhieptran.lua:14` + `sugiasatthu.lua:14` (bản Việt) · `HD3_ST_MAX_NGAY` trong `cauhinh_hoatdong.lua` (bản Linux) · `ST_TRAN_NGAY=8` [`Core\Src\CoreShell.cpp:11338`] (auto ngoài). Sửa một bên là auto chạy lệch server.

**A7. HAI TỆP GIỐNG NHAU, SỐ LỆCH.** `global\npcchucnang\nhieptran.lua` vs `tinhnang\vuot_ai\sugiasatthu.lua`, cùng khai biến **toàn cục**: `MONEYNHANNV` 5000 vs 50000 (dòng 18), `MONEY_GO_BOSS` 1000 vs 5000 (dòng 19), `MAX_STACK_NUM` 100 vs 1 (dòng 24). Giá trị thật phụ thuộc **thứ tự Include**. Đây là bẫy trùng tên nguy hiểm nhất trong cả 6 nhóm. Ngoài ra cả hai đều `dofile` **chính nó** ở đầu `main()` (`:28`) — mở NPC là nạp lại cả tệp.

**A8. `SERVER_TEST = 1` ĐANG BẬT.** [`lib\lib_server.lua:5`] → mở mục "nhận lệnh bài / nhận 5 Sát Thủ Giản" cho **MỌI người chơi**, không chỉ admin [`tinhnang\phonglangdo\thuyenphu.lua:21-23`, `global\npcchucnang\nhieptran.lua:43-46`].

**A9. VƯỢT ẢI — bảng bù lượt LỆCH cấu hình.** `HD3_VA_LUOT_NGAY = 1` trong `cauhinh_hoatdong.lua` nhưng `TB_BU_HD` [`lib\lib_ham.lua:299`] ghi **2** → hệ bù đang tính người chơi thiếu 1 lượt/ngày dù thực tế chỉ được vào 1 lần.

**A10. 🔥 TỐNG KIM — LỊCH LUA ≠ LỊCH C++ (phát hiện khi đối chiếu chéo).** Đã mở cả hai tệp để xác minh:
- Lua (chạy thật): khung 1 = **`{23,46,19,1}`** [`tinhnang\tong_kim_tcap\lib_tktc.lua:71`]
- C++ (auto/bot đọc): khung 1 = **`{ 13, 58 }`** [`Core\Src\KTongKimTables.h:37`]

3 khung còn lại (17:50 / 20:50 / 22:50) khớp. Tức **bảng sinh tự động đã CŨ so với script sống** — auto ngoài đang chờ trận lúc 13:58 trong khi máy chủ mở lúc 23:46. Phải chạy lại `ReverseTools\gen_tongkim_tables.py` + build lại DLL.

## 🔴 B. Lỗi trong mã (chưa nổ hoặc đang âm thầm sai)

**B1.** `tungtung1.lua:54` kiểm trần exp **một lần** trước vòng lặp, rồi `:58-66` cộng 10 lần `random(1tr..2tr)` mà không kiểm lại → một cú đánh cuối ngày vượt trần `EXP_TBH_DAY` tới ~20 triệu.
**B2.** `EXP_TRA_NVTT` bị nhân `EXP_RATE=20` → **1 tỷ exp/lần trả nhiệm vụ** [`lib_phonglangdo.lua:14`, `thuyenphubac.lua:47`].
**B3.** `deathhksv.lua:43` `nVIP = GetTask(378)`; nếu = 0 thì **toàn bộ exp boss máy chủ = 0** (dòng 87/101/113/116) dù thông báo vẫn ghi 500.000.000. Thêm: `:112` nhánh tổ đội **không kiểm map/khoảng cách** → cả đội ở bất kỳ đâu cũng nhận exp; `:129-133` `closefile(LoginLog)` nằm **ngoài** `if LoginLog then`; `:45` còn dòng gỡ lỗi `Msg2Player("Test ... cái!")` hiện cho người chơi.
**B4.** `task\partner\train\partner_jitan.lua:123` dò bảng exp bằng **`partneridx`** (chỉ số bạn đồng hành) thay vì **`partnerlvl`** (cấp) → bạn đồng hành mọi cấp chỉ nhận **3.000 exp** thay vì tới 3.000.000. **Gom bảng `BDH_TD_BANG_EXP` mà không sửa dòng 123 là vô nghĩa.**
**B5.** `tinhnang\phonglangdo\drop.lua:51-56` hai khối rơi Ngũ Hoa chồng nhau: `(80<n<85)` là tập con của `(n>20)` → dải 81-84 rơi 2 viên.
**B6.** `Droprate_pubg.lua` trỏ tới **4 tệp không tồn tại**: `golden_lv10.ini` (`:37`), `golden_lv20.ini` (`:43`), `golden_lv30.ini` (`:49`), `golden_lv50.ini` (`:61`) — thư mục chỉ có `golden_lv40/60/70`.
**B7.** `LIFE_BOSS_DAIH = 1` [`lib_bosshk.lua:25`] (chú thích ghi mặc định 18000000) → boss **Đại + Máy Chủ + PLD** đều sinh ra với **1 MÁU**. Gần như chắc chắn là số test còn sót.
**B8.** BẪY NĂM: `event\storm\head.lua:86` tính `TB_STORM_DAYS[nowyear]` nhưng bảng chỉ khai **tới `[2010]`** [`head.lua:37-43`] → ở 2026 là `nil` → **lỗi cộng nil**. Hiện chưa nổ vì `storm_ask2start` đã bị comment toàn thân [`function.lua:57-71`]; bật lại là nổ ở Tống Kim [`battles\battlehead.lua:734`], Vượt Ải [`missions\challengeoftime\award.lua:182`], Dã Tẩu [`global\seasonnpc.lua:898`].
**B9.** `event\trongbanghoi\lib.lua:45` truyền `nSzName` (S hoa) trong khi biến khai là `nszName` (s thường, dòng 40) → **NPC trống sinh ra KHÔNG CÓ TÊN**. Chưa nổ vì driver tắt.
**B10.** `lib\lib_sukien.lua:75-79` `if nRand > 1 ... elseif nRand == 9` → nhánh `== 9` **không bao giờ vào** → dây chuyền 747-752 không bao giờ rơi.
**B11.** `Droprate_normal.lua:81-82` và `Droprate_pubg.lua:81-82`: `local nRanDrop = random(1,2); if (nRanDrop > 2)` → nhánh trên là **mã chết**, bậc 9 luôn dùng 411-502.
**B12.** `lib_bosshk.lua:46` và `:49` gọi `random(...)` **ngay trong bảng hằng** → mã NPC và hệ của Boss Cửu Thiên / Boss PLD bị **chốt lúc nạp tệp**, không đổi giữa các lần sinh. `lib_bosshk.lua:317` và `:331` khai **trùng khoá** `["Phong Lăng độ"]` → dòng 331 đè dòng 317.
**B13.** `global\seasonnpc.lua:115` viết `if (nNum == 40 and nCancelNum) == 0 then` — **dấu ngoặc sai**; kết quả tình cờ đúng ý định nhưng cực dễ vỡ nếu ai sửa lại. **CHƯA CHẮC có nên đụng vào.**
**B14.** `vng_event\item\biggoldenseed.lua:55,:57,:59` tính "**356 ngày/năm**" — sai, đúng là 365, lệch 9 ngày/năm.
**B15.** `citywar_function.lua:194` dùng biến `count` chưa khai (format ghi nil) — lỗi log, không chặn thưởng.
**B16.** `MAX_GIAO_TRUYCONGL` dùng `>` [`thuyenphubac.lua:74`] → số 1 thực tế cho **2 lượt/ngày**. `quahuyhoang.lua:50,:55` cũng dùng `>` → hái được **MAX+1** quả/ngày.
**B17.** `event\eventtet\npcsukien.lua:99-105` kiểm 30 Tiền Đồng nhưng **chỉ trừ 10**. `:37`/`:41` dùng `<` rồi `>` nên số xu **đúng bằng** tổng tiền rơi vào nhánh "không đủ".
**B18.** `timerserver.lua:364-400` dùng lại biến `i` của vòng ngoài trong vòng lặp con (`:369 for i=1,5`) — Lua 4 dùng biến toàn cục cho `for`, nguy cơ phá vòng ngoài. `:363` gán `idxtemp = PlayerIndex` **SAU** khi vòng trước đã phá `PlayerIndex` → khôi phục sai con trỏ ở `:401`. **CHƯA CHẮC mức tác hại.**
**B19.** `header\tongkim.lua:34-56` `SAVEDULIEU` **ghi đè cả tệp `LIMITED_1_ACCOUNT.lua` mỗi lần báo danh** trên luồng game — chi phí O(n²).

## 🔴 C. Bẫy cấu trúc / hai nguồn

**C1.** Map **995** dùng chung giữa Kiếm Môn Quan [`lib_kmq.lua:14-16`] và Hạt Huy Hoàng [`lib_huyhoang.lua:36-56`].
**C2.** Hoa Đăng tắt **BA tầng**: lịch [`timerserver.lua:93`], menu nhận thưởng [`global\npcchucnang\lequan.lua:32-34`], và phần cộng exp/tiền khi trả lời đúng cũng đã comment [`event\event_cauhoi\cauhoi.lua:91-102`] → bảng `EXPCAUHOI` là **BẢNG CHẾT**.
**C3.** Tết Đoàn Viên: 2 mục menu "Nhận Thưởng Mốc Event" và "Reset Event" **đã comment** [`npcsukien.lua:19-20`] → 5 mốc thưởng không lấy được, nhưng bán Quả Sung 1 xu vẫn chạy vĩnh viễn.
**C4.** **HAI hệ chuyển sinh cùng tồn tại**: `task\metempsychosis\*` (7 lần, học phí 100tr) và `event\chuyensinhdaisu.lua` (4 lần, giá xu 500-3000, NPC Ba Lăng `balanghuyen.lua:95`). **Không được gộp khóa.** Riêng `chuyensinhdaisu.lua` giá viết cứng **8 chỗ** + text thoại `:17` = nguồn thứ ba; kiểm tiền (`:41`) tách rời việc trừ tiền (`:80`).
**C5.** `task_head.lua:53` `TB_TRANSTIME_LIMIT` chỉ có **6 phần tử** trong khi `NTRANSLIFE_MAX = 7`. Chưa vỡ vì `task_func.lua:111` chặn ≥7, nhưng nâng lên 8 mà quên nối dài bảng là nổ `nil*24*60*60`.
**C6.** `event\change_destiny\head.lua:6` có cờ `bDeBug` — bật thành 1 là **bỏ qua CẢ ngày lẫn điều kiện chuyển sinh 160-179**. **Đừng gom cờ này thành khóa cho chủ game bấm nhầm.**
**C7.** `startgame.lua:114-115` vẫn gọi `addnpccongthanh()`/`addtrapcongthanh()` của hệ CTC **CŨ**, đặt 3 trap `chancong_1/2/3` [`lib_ctc.lua:266-268`] lên **chính map 221** mà hệ JX2 đang dùng làm chiến trường. **CHƯA CHẮC có chặn đường trong trận hay không — cần chạy thử một trận.**
**C8.** `settings\gamesetting.ini` có **HAI section `[Series]`** (`:97` và `:293`). `KIniFile::SetKeyValue` [`Engine\Src\KIniFile.cpp:775-797`, nhánh else `:831-838`] **gộp section trùng ID và ghi đè khóa trùng**. Chưa mất dữ liệu (khóa khác nhau) nhưng thêm khóa trùng vào một trong hai là bản SAU thắng, **im lặng**.
**C9.** `HD3_Tick` **không có biến chống gọi lặp trong cùng 1 phút** (khác `YDBZ_Tick` có `YDBZ_MOC_CUOI` [`ydbz_driver.lua:77`]) → nếu `RunTime` bị gọi hơn 1 lần/phút thì PLD/Vượt Ải khai cuộc 2 lần. **CHƯA CHẮC `RunTime` có bị gọi lặp hay không.**
**C10.** `missions\citywar_arena\head.lua` còn ở mã hoá **GBK** (chữ Trung gốc JX2), **không phải TCVN3** — đọc bằng bộ giải TCVN3 ra rác. Cẩn thận khi sửa.
**C11.** Chú thích `header\cauhinh_hoatdong.lua:98` ghi "*Lịch tick driver: settings\TimerTask.txt dòng khoá 54 (30 giây/lần)*" là **SAI**: `TimerTask.txt` chỉ có 2 cột `TASK<TAB>SCRIPT`, **không có cột chu kỳ** [`Core\Src\KTaskFuns.cpp:104-119`, `:177-186`]. Con số 30 giây nằm trong chính script driver. **Nên sửa lại chú thích khi gom.**
**C12.** `settings\huoyuedu\huoyuedu.txt` (41 hoạt động) có thể **KHÔNG chạy thật**: móc nối `LuaDynamicExecuteByPlayer` [`Core\Src\ScriptFuns.cpp:2371-2390`] chỉ ghi log rồi bỏ qua, chú thích ghi rõ "*hook huoyuedu ngu dong tren JX1*". **CHƯA CHẮC — cần thử tại chỗ.**
**C13.** `quanlykhuvucbang.lua:2` và `thoren.lua:2` Include `\datascript\banghoi\mapbanghoi.lua` — **thư mục KHÔNG tồn tại**. Hệ thật dùng `script\tinhnang\congthanhchien\mapbanghoi.lua` [`item\lenhbaitanthu.lua:654`] — **hai tệp dữ liệu khác nhau, khác cả số cột** (cũ 16 cột, mới 3 cột). Thêm: `trapra1.lua:2` gọi `dofile("script/strartgame/...")` — **sai chính tả**; `quanlythidau.lua:33` `SetDeathScript("\\script\\khuvucbang\\...")` — **thiếu `startgame\`**.
**C14.** Đổi tên/thoại NPC bên Lua **PHÁ auto ngoài**: `KHoatDongTables.h:104-119`, `:151-159` và `KTongKimTables.h:15-32` chứa ~30 chuỗi **MARKER hội thoại TCVN3** khớp byte-for-byte. Đây là ràng buộc chéo script↔C++ nguy hiểm nhất.

## 🔴 D. Hằng CHẾT / mã CHẾT — ĐỀ NGHỊ **KHÔNG** đưa vào tệp cấu hình

| Thứ | Vị trí | Ghi chú |
|---|---|---|
| `TIME_XOANPC_TK` | `lib_tktc.lua:59` | khai rồi bỏ không |
| `TAB_TIME_TONG_KIM` cột 3 (19/18/21/23) | `lib_tktc.lua:70-82` | không nơi nào đọc — **dễ tưởng là giờ kết thúc** |
| `TAB_QUANHAM` cột 3 / `QUANHAMTK` cột 4 | `lib_tktc.lua:119-126`, `:108-116` | không tìm thấy nơi đọc |
| `RandPThucTongKim()` | `lib_tktc.lua:452-459` | cả 2 nhánh đều `return PT_CuuSat` → 2 phương thức kia CHẾT, `nXutk` luôn = 3 |
| `EXP_BOSS_LAST_H` / `MSTIME_VUOT_AI_XOANPC` | `lib_vuotai.lua:17`, `:14` | hằng chết |
| `PKWINBONUS` / `LOSEBONUS` | `lib_loidai.lua:37-38`, `citywar_arena\head.lua:29-30` | `GetBonus` đã comment [`citywar_arena\death.lua:30`] |
| `LIMIT_LEVEL_TBH` | `event\trongbanghoi\lib.lua:17` | không ai đọc; 2 nơi hardcode 80 |
| `EVENT_ACTIVE` / `RANDPLD` / `RANDTK` / `RANDVA` | `lib\lib_sukien.lua:6, :11, :13, :14` | + 6 hàm rỗng `:68`, `:91`, `:109`, `:116`, `:120`, `:126` |
| `settings\RankBattle.txt` | — | engine đọc `PlayerTitle.txt` thay thế [`KNpc.cpp:11352`] |
| `settings\killer.ini`, `auction.ini`, `ExpandRank.txt`, `lottery.txt`, `meridian.txt`, `normalunique.txt`, `speicalunique.txt`, `portrait.ini` | — | grep C++ **và** Lua = 0 kết quả |
| `[DiceGame]` `[Repute]` `[AutoHang]` `[WeaponChart]` `[WeaponType]` `[Coin]` `[SkillType]` `[DetailSer]` `[Emotes]` `[Actions]` `[OFFLINE]` `[Item]` `[DefaultDieSpr]` `[SkillsIcon]` `[TransLife]` `[NationalEmblem]` `[VipRank]` | `gamesetting.ini` | không ai đọc, hoặc `KPlayerSet.cpp:117,127,132` đã comment |
| `tinhnang\datau\*` (7 tệp) + `global\npcchucnang\datau.lua` (827 dòng) | — | Dã Tẩu bản Việt **MỒ CÔI** [`item\lbhtdatau.lua:3-5`]; `OTHER_DATAU` [`lib\lib_map.lua:9`] không ai dùng |
| `global\drop\daihoangkim.lua` + `tieuhoangkim.lua` | — | 0 tham chiếu toàn cây |
| `tinhnang\boss_hoangkim\bosslbdrop.lua` + `bosslbdeath.lua` | — | chỉ 1 dòng `ReLoadScript` bị comment [`item\lenhbaiadmin.lua:132`] |
| `tinhnang\vuot_ai\drop.lua` | — | bản sao mồ côi của `boss_satthu\drop.lua` |
| `event\demhuyhoang\*` + `event\event_demhuyhoang\*` | — | 0 tham chiếu; **trùng chức năng** với `event_huyhoang_dungdb` đang Include [`timerserver.lua:17`] — **ba bản Huy Hoàng cùng tồn tại** |
| `lib_bosshk.lua` mã mồ côi | `:194-216`, `:238-254`, `:35-39`, `:106-113`, `:124-135` | 3 nguồn boss đại, 2 nguồn boss tiểu — chỉ 1 có tác dụng mỗi loại |
| `messenger_giveprize()` + `tollgate_allprize.txt` | `task\tollgate\messenger_prize.lua:80-81` | tệp **tự ghi** "Hàm này hiện MÃ CHẾT" |
| `event\eventhead.lua:67-79` (6 hằng ngày 2005) | — | chỉ Include ở `citywar_global\timer.lua:1`, không hoạt động nào dùng |
| `bossbanghoi1..5.lua` | `startgame\khuvucbang\` | 0 tham chiếu; menu gọi đã comment [`quanlykhuvucbang.lua:91,93`]. `bossbanghoi5.lua:11` cộng exp bằng **vòng lặp 1000 lần** `AddOwnExp(20000)` — bật lại là giật server |
| `thuongtieu2()` | `event\event_vantieu\tieudau.lua:311-331` | bản thưởng cũ, không ai gọi |
| `mobinhtk.lua:49-52` chặn tên "LongYaz" | — | rác thử nghiệm còn sót |

## 🔴 E. Tệp KHÔNG được để công cụ gom quét nhầm
`event\event_vantieu.rar` (tệp nén lẫn trong cây script) · `event\event_huyhoang_dungdb\ScriptError.log` · `missions\challengeoftime\ScriptError.log` · và mọi bản sao `.truoc_*`: `wuxingyin.lua.truoc_mobang_2908`, `tongwar\head.lua.truoc_cauoi_2508`, `shuizei.lua.truoc_p28_2808`, `lenhbai_def.lua.truoc_nanthuong`, `chuangguang30.lua.truoc_nanthuong`, `kill_level.lua` (bản `.truoc_nanthuong`), và **5 bản** của `posthouse.lua` (`truoc_additem7_2508`/`truoc_cfgts_2508`/`truoc_cspace_2508`/`truoc_rutgon_2508`/`truoc_taskfix_2408`).

**Cảnh báo phiên song song:** `task\tollgate\killer\kill_level.lua:1` ghi "*[MATHUONG 29/08] nắn mã vật phẩm bảng thưởng theo TÊN*" — bảng này **vừa bị công cụ nắn mã đụng vào hôm nay**; đối chiếu với bản `.truoc_nanthuong` trước khi gom. Và `Core\Src\KCore.cpp` **đang bị một phiên khác sửa** (18:18 ngày 29/08).

## 🔴 F. Tệp dữ liệu bị GHI ĐÈ lúc chạy — chỉ ghi chú, KHÔNG đưa vào cấu hình
`tinhnang\congthanhchien\danhsach_bang.lua` (hiện **RỖNG** → `NAME_BANGTHU`/`NAME_BANGCONG` = `"0"`) · `tinhnang\congthanhchien\mapbanghoi.lua` (4 bang: TESTGAME/998, NgaoThe/996, KiemThe/1000, PhongThan/1002) · `tinhnang\congthanhchien\log_tax.lua` · `tinhnang\loidai\danhsach_bang.lua` · `settings\jx2citywar.txt` · `dulieu\LoiDaiHonChien_MangTam.txt` · `script\event\data\event_rank_save.lua`.

---

# THỨ TỰ THI CÔNG ĐỀ NGHỊ

1. **Vá A1 (lỗ exp Lôi Đài Hỗn Chiến)** — mất tiền thật, không chờ được.
2. **Xác minh A10 (lệch lịch Tống Kim Lua↔C++)** — chạy lại bộ sinh + build.
3. **Hợp nhất A7 (`nhieptran` vs `sugiasatthu`)** và các cặp trùng tên toàn cục (mục 0) — **phải làm TRƯỚC khi gom**, nếu không mỗi cặp sẽ sinh 2 khóa.
4. **Gom 20 `BAT_*`** (`ch_lich` mục ưu tiên 1) — giá trị lớn nhất trên công sức thấp nhất.
5. **`TK_` + `CTC_`/`CTLD_`** — 2 hoạt động sống thật.
6. **`GLB_*` + `BRXP_*`** — hệ số nền và trần kinh tế ẩn.
7. **`DT_` + `BDH_`** (đồng thời sửa B4) — nhóm chưa có khóa nào.
8. `PLDV_` / `VAV_` / `LDBH_` / `CTCV_` / `KMQ_` / `TBH_` / `HDANG_` / `HH_` — chỉ khi chủ xác nhận muốn giữ đường lui.

**Số khóa đề xuất: ~330** (chưa kể bảng lớn giữ ở `settings\`), **0 trùng** với 120 khóa hiện có. Tiền tố mới: `BAT_ TK_ CTC_ CTLD_ CTCV_ LDBH_ LDHC_ KMQ_ TBH_ HDANG_ HH_ VT_ PLDV_ VAV_ BHK_ BHKT_ BHKD_ BHKSV_ BHKPLD_ BST_ SKD_ DRQ_ DRP_ DT_ BDH_ CS_ CSDS_ VA30_ BU_ BRXP_ GLB_ SVC_ SVE_ TDV_ TTPLD_ TKAL_ TNLB_ HKA_ BAU_`.