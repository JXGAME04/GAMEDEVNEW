# PHÂN TÍCH: GIAO DIỆN BANG HỘI HIỆN TẠI so với BẢN LINUX (07/09/2026 — CHỈ ĐỐI CHIẾU, CHƯA SỬA MÃ)

> Yêu cầu chủ game: đọc lại toàn bộ bàn giao bang hội (đợt 1→12), mổ bản Linux, so thật kỹ giao diện
> bang hội hiện tại với bản gốc — đặc biệt "phần xem tin tức bang" đang thiếu và "vài tính năng bị sao".
> Mọi kết luận dưới đây đều có bằng chứng tệp:dòng. Chưa đụng một dòng mã nào.

## 0. Nguồn đối chiếu

| Bên | Nguồn |
|---|---|
| Mã ta | `origin/main` **d706057c** (07/09 10:42): `Sources/S3Client/Ui/UiCase/UiTongJX2.{h,cpp}` (3.120 dòng, sửa lần cuối 01/09 fa4235ef chỉ thêm 3 phái), `clientui_jx2/UiTongJX2.ini` (412 section, md5 `b62cf16c…` = bản đang chạy ở `bin\client` và PATCHFULL), `Sources/Core/Src/KTongJX2.cpp` (BuildClientView :2249), `Sources/Core/Src/KProtocol.h` (COP 0..37 :2448-2485) |
| Bản Linux | 17 file `.ini` bang hội trong `D:\ServerLinux\Patch\ui\ui3_1024` (dump giải mã GBK+TCVN3 ở scratchpad `bp\*.txt`), `D:\ServerLinux\Patch\game_y_unpacked.bin` (22,5 MB — quét chuỗi để biết **khoá ini nào client gốc thật sự đọc**), `server1\lang\vn\stringtable_client.txt` / `stringtable_core.txt`, `gateway\s3relay\lang\vn\stringtable_relay.txt`, `Patch\script\tong\*.lua` |
| Bàn giao đã đọc | `BANGIAO_BANGHOI.md`, `_DOT10/11/12`, `KEHOACH_BANGHOI_DOT6/DOT9`, `CLIENT_BANGHOI_SPEC/CONTROLS`, `KHAOSAT_KHOILUONG_DOT12`, `BANGHOI_JX2_PHANTICH.md` (1.390 dòng) |

Phương pháp: script `coverage.py` ánh xạ **từng section** của 17 blueprint → section trong `UiTongJX2.ini`
của ta → có dòng C++ nào `Init`/dùng không; sau đó đọc tay toàn bộ `UiTongJX2.cpp` để loại kết quả
trùng tên (ví dụ `BtnForceToRetire` xuất hiện trong bảng quyền `Rt_` nhưng **không** có trong bảng nút
trang Chức năng `s_sFunBtn`).

---

## 1. KẾT LUẬN NHANH — "phần xem tin tức bang" là gì và đang thiếu ra sao

Bản Linux có **ba** chỗ "xem tin tức bang" mà bản ta chưa có hoặc chỉ có vỏ. Khả năng cao chủ game
đang nói tới (A); (B) và (C) cùng họ, ghi luôn để làm một lượt.

### (A) Xem TRANG TIN TỨC của bang KHÁC — nút đáy "Xem tin Bang khác" `[BtnTongList]` — **THIẾU HẲN**

Bằng chứng luồng gốc:
- `帮会主窗口.ini [BtnTongList]` sprite `帮会列表按钮.spr` (đã nung chữ "Xem tin Bang khác" — DOT9 #19).
- Nó mở cửa sổ riêng **`帮会列表窗口.ini`** (120×463, `Image=\Spr\Ui3\帮会列表\查询别帮.spr` = "tra cứu
  bang khác", `[MemberList]` rộng 108 px = **một cột tên bang**, 25 dòng/trang, Trước/Kế). Tên file này
  **có trong `game_y.exe`** (đếm chuỗi = 1) ⇒ client gốc thật sự mở cửa sổ này.
- Chọn một bang ⇒ cửa sổ chính hiện **trang Tin tức + danh sách thành viên của bang đó** (chế độ xem),
  nút đáy đổi ảnh: `[BtnEnterMap] ImageEnterMapSelf=…进入本帮 (Vào bổn bang)` /
  `ImageEnterMapOther=…进入他帮 (Vào bang khác)` — **cả hai khoá đều được `game_y.exe` đọc** (đếm = 1/1).
  Chuỗi `G_STR_APPLY_JION_LEAGUE "Xin gia nhập liên minh"` (stringtable_client:497) cũng được exe dùng
  ⇒ từ bang đang xem có thể xin vào liên minh của họ.

Bản ta (`UiTongJX2.cpp`):
- `m_Bot[2]` (Init từ `[BtnTongList]`, :690) khi bấm **chuyển sang trang 2×2 xem chiêu mộ**
  `TJX2_UI_PAGE_OTHERZM` (:2677-2682). Trang 2×2 đó vốn là của **`[BtnOtherZhaoMu]`** (tab 2 dành cho
  người **chưa có bang**, blueprint đặt cùng toạ độ với `[BtnZhaoMu]`), không phải của nút "Xem tin Bang khác".
- Trang danh sách bang 3 cột `TJX2_UI_PAGE_TONGLIST` **không còn đường bấm tới**: chỉ `m_BtnList` mở nó
  (:2594-2597) mà `m_BtnList` bị `Hide()+Enable(false)` vĩnh viễn (:1528-1529). Server vẫn trả
  `PAGE_TONGLIST` (KTongJX2.cpp:2255-2300) nhưng chỉ có tên/bang chủ/phe/cấp/nhân số.
- Máy chủ **chỉ dựng được trang Tin tức của bang mình**: `BuildClientView` lấy
  `sJX2_PlayerTong(nPlayerIdx)` (:2254) và `return 0` nếu không thuộc bang (:2360) ⇒ không có cách
  xin "INFO của bang X". Nút "Xem chi tiết" trên thẻ 2×2 vì thế chỉ hiện hộp thoại chữ (:3041-3060,
  DOT11 deviation 3) thay vì mở trang tin tức bang đó như gốc.
- `[BtnEnterMap]` của ta chỉ gửi `COP_ENTER_MAP` vào lãnh địa **bang mình** (:2662), không có chế độ
  "Vào bang khác".

### (B) Trang "Sử dụng chức năng": 4 trang con gốc đổi CẢ PANEL PHẢI — ta chỉ đổi cụm nút

- Blueprint `帮会功能使用页面.ini [ImgSubPageMask]`: 4 ảnh tên trang con `查询帮众 / 查询帮会 / 查询联盟 /
  查询国家` = **Tra cứu bang chúng / Tra cứu bang hội / Tra cứu liên minh / Tra cứu quốc gia**.
  Panel phải có **hai** nút sắp xếp: `[BtnMemberSortMenu]` (7 mục thành viên) và **`[BtnTongSortMenu]`**
  (5 mục: đẳng cấp / Nhân số / đẳng cấp / Liên minh / Số lượng) — tức panel phải chuyển sang **danh sách
  BANG** ở trang con 2-4; `[TitleName] MemberTitle=Thành viên / UnionTongFigureTitle=Chức vị` là tiêu đề cột
  đổi theo (bang trong liên minh: Minh chủ / thành viên). `BtnTongSortMenu`, `MemberTitle`,
  `UnionTongFigureTitle` đều được `game_y.exe` đọc (đếm = 1).
- Bản ta: `m_nFunSub` chỉ lọc cụm nút giữa (`s_nFunBtnSub` :1346-1348); panel phải **luôn là danh sách
  thành viên** (`RenderFunUse` :2199-2200); `Fun_BtnTongSortMenu` có trong ini nhưng không dòng nào Init.
  Cột 3 của ta hiện "giá trị tiêu chí sắp xếp" (DOT11 ④) là đúng cho trang con 1 nhưng trang con 2-4 chưa có.

### (C) Nhật ký › mục con "Mục tiêu tuần" = BÁO CÁO MỤC TIÊU TUẦN — ta chỉ ghi một dòng chữ

- Gốc: `帮会记录页面.ini [BtnWeekDaily] Label=Mục tiêu tuần`; `[Main]` có `LastWeekDesc=Mục tiêu tuần
  trước`, `CurWeekDesc=Mục tiêu tuần này` (ta đã chép vào `[PageBg4]` :300-301, exe đọc cả hai). Nội dung
  dựng từ `stringtable_client.txt:465-473`: `G_STR_TONG_DATE_DESC` "Bang hội lập được %u tuần và %u ngày",
  `G_STR_WEEKGOAL_LEVEL` "Độ khó mục tiêu tuần: %d cấp", `G_STR_TONG_WEEKLY_AIM` / `G_STR_PERSONAL_WEEKLY_AIM`
  "cần hoàn thành … / đã hoàn thành …", `G_STR_PROCESS_STATUS` "Số ngày còn lại: %d  Bang hội: <%s> Cá nhân: <%s>",
  `G_STR_TONG_PRIZE`/`_IB` "Phần thưởng bang hội: %d vạn ngân sách chiến bị / %d Boss triệu hoán phù …" — tất
  cả đều có trong `game_y.exe`.
- Bản ta: `RenderRecord` nhánh `m_nRcSub == 0` chỉ `m_Row[0].SetText("Mục tiêu tuần xem trong bang thông tin.")`
  (:2219-2226). Máy chủ **đã có đủ số liệu** (field 20/21 ngày-tuần, 22-28 tuần này, 29-35 tuần trước, 36
  CurWeekGoalLevel — `KTongJX2.cpp:1471-1485`; member KV 9/10) nhưng **chưa có gói sync** nào mang chúng
  xuống client (`TONG_JX2_INFO_SYNC` không có field 22-36).

---

## 2. ĐỐI CHIẾU TỪNG CỬA SỔ / TRANG (blueprint → bản ta)

Ký hiệu: ✅ có và chạy · ⚠️ có nhưng lệch/vỏ · ❌ thiếu · ☠️ **chết ngay trong bản gốc** (đừng làm).

### 2.1 Cửa sổ chính `帮会主窗口.ini` (13 section)

| Gốc | Ta | Ghi chú |
|---|---|---|
| 7 tab: BaseInfo 22 · ZhaoMu 116 (OtherZhaoMu cùng ô) · **Workshop 209** · **FunUse 303** · **RightManage 398** · TongRecord 491 | 5 tab + nút `m_BtnFun`: Tab0 Tin tức 20 · Tab1 Chiêu mộ 114 · **BtnFunUse 208** · **Tab2 Phân phối 302** · **Tab3 Tác phường 396** · Tab4 Nhật ký 490 (ini :162-215, `s_nTabPage` :2520) | ⚠️ **thứ tự tab lệch gốc**: gốc Tác phường→Sử dụng chức năng→Phân phối; ta Sử dụng chức năng→Phân phối→Tác phường. Sprite tab đúng bộ `招募页vn`. |
| `[BtnTongHelp]` (18×18 góc phải trên) | không Init | ❌ exe gốc có đọc `BtnTongHelp`; nhưng file trợ giúp `帮会改造系统.ini` **không được exe tham chiếu** ⇒ chưa rõ nó mở gì — không ưu tiên. |
| `[BtnTongList]` Xem tin Bang khác | mở trang 2×2 | ❌ xem mục 1(A) |
| `[BtnEnterMap]` Self/Other | chỉ Self | ⚠️ thiếu chế độ "Vào bang khác" |
| `[BtnRefresh]`, `[BtnClose]` | ✅ | |

### 2.2 Trang Tin tức `帮会基础信息页面.ini` (56 section) — bảng ta `s_sInfoCtl` (:155-179)

| Section gốc | Ta | Ghi chú |
|---|---|---|
| TongName/Master/League/Camp/TongLevel/MemberNum/BuildLevel/TongCapital/BuildFund/BattleFund/TotalOffer/StoredBuildFund/PersonalOffer/WeeklyOffer + 3 tiêu đề | ✅ | đúng toạ độ (+18/+52) |
| **`TitleStandFund/TxtStandFund`** "Tiền duy trì chiến bị tuần" Top=155 (ô trống, không chồng ai) | ❌ | `TxtStandFund` **được exe đọc**. Số = `TONG_GetStandFund` = (thành viên − ẩn sĩ) × field 17 (BANGHOI_JX2_PHANTICH §5.3); gói INFO đã có `m_dwPerStand` + `m_wMemberTotal` ⇒ tính được ngay ở client, không cần đổi gói. |
| `TitleServiceFee/TxtServiceFee` "Chi phí bảo trì" (90,78) | ❌ | exe có đọc, nhưng **chồng toạ độ** với `TitleBuildLevel` (2,78 rộng 122) ngay trong blueprint ⇒ gốc chắc chắn ẩn/hiện theo điều kiện — cần ảnh chụp client gốc trước khi thêm. |
| `TitleLiveness/MyLiveness/DailyCost` + `[BtnFreshTongLiveness]` | ❌ | ☠️ `TxtLiveness/TxtMyLiveness/TxtDailyCost/BtnFreshTongLiveness` **= 0 tham chiếu trong `game_y.exe`** ⇒ bản VN gốc không dùng (hệ 活跃度 chưa bật). Bỏ. |
| `[TxtHelp]` (250×45, "Giúp đỡ") | tiêu đề có, thân ❌ | exe có đọc `TxtHelp`; nội dung động gốc chưa xác định (không có trong stringtable). |
| `[Main]` `DefWarnColor=255,0,0` / `DefThresholdColor=255,255,0` (ta chép vào `[PageBg0]` :229-231) | ❌ | exe đọc cả hai ⇒ gốc **đổi màu đỏ/vàng** ô số khi dưới ngưỡng (vd chiến bị < bảo trì tuần). Ta luôn một màu. |
| Panel phải MemberList/TxtRank/Title/Type/Online/Sort/Jump/Page | ✅ | dùng bộ `Fun_*` chung |

### 2.3 Trang Sử dụng chức năng `帮会功能使用页面.ini` (79 section) — bảng ta `s_sFunTxt`/`s_sFunBtn` (:233-306)

Khối số liệu trái: ✅ TongName/Union/BuildLevel/TotalOffer/TongMoney/BuildFund/BattleFund; ❌ **StoredBuildFund**
(hàng cuối, không chồng ai — `s_sFunTxt` dừng ở BattleFund); ☠️ Liveness (0 ref).

Nút cạnh số liệu:

| Nút gốc | Nghĩa | Ta |
|---|---|---|
| BtnUpgradeBuildLevel Thăng cấp | | ✅ nAct 0 |
| BtnAssignTongOffer Phát | phát cống hiến dự trữ | ✅ mở `KUiTongGrant(false)` |
| BtnGetTongMoney Rút · BtnAssignTongMoney Phát | | ✅ hộp nhập / `KUiTongGrant(true)` |
| **BtnTransformMoney "Chuyển"** ngân quỹ → ngân sách kiến thiết (Tip gốc) | hook gốc `MONEYFUND2BUILDFUND` (tong_mix.lua) | ⚠️ có section, `nAct=-1` ⇒ **ẩn vĩnh viễn** (:282); server không có COP |
| **BtnTransformBuildFund "Chuyển"** kiến thiết → chiến bị | hook gốc `BUILDFUND2WARFUND_R` :195 | ❌ không có trong `s_sFunBtn`; server không có COP. Chuỗi `G_STR_TRANSFORM_BUILD_FUND`/`G_STR_INPUT_TONG_MONEY` đều được exe dùng ⇒ hai nút này **sống** ở gốc. |

Cụm nút giữa theo trang con (gốc chia 4 trang bằng chú thích trong file, DOT9 #13):

| Trang con gốc | Nút gốc | Ta |
|---|---|---|
| 1 Tra cứu bang chúng (9 nút) | BtnRecruit ✅(nAct 4) · **BtnDemise "Chuyển vị"** (nhường bang chủ) ❌ · **BtnForceToRetire "Thoái ẩn"** (ép thoái ẩn, quyền 1902) ❌ · BtnKickOut ✅ · BtnDepose ✅(cửa sổ bổ nhiệm) · Btn_DispenseOffer ✅ · **BtnChangeMaleTitle / BtnChangeFemaleTitle / BtnChangeTitle** (đổi danh hiệu nam/nữ/cá nhân) ❌ | 4/9 |
| 2 Tra cứu bang hội (6 nút) | BtnChangeCamp ✅(nAct 12) · BtnCreateTongMap ✅(5) · **BtnConfigureTongMap "Thiết lập"** ⚠️ `nAct=-1` ẩn (server có `COP_MAP_SET` 17 nhưng không đường bấm) · **BtnTongStunt "Kỹ năng"** ⚠️ `nAct=-1` ẩn (server có `COP_SETSTUNT` 9; hiện tuyệt kỹ chỉ đặt qua NPC Đồ Đằng) · BtnTongChallenge / BtnTongDetect ☠️ (**blueprint đặt `Enable=0`** — chết ở gốc) | 2/4 sống |
| 3 Liên minh (5 nút) | ✅ đủ 5 (DOT11) | |
| 4 Đại thần (2 nút) | ✅ 2 nút, nhưng chọn chức bằng **hộp nhập số "1 TT/2 NS/3 TP"** (:2788-2803) thay cho cửa sổ gốc `帮会委任大臣.ini` (3 ô kiểm Official_A/B/C "Phong làm Đại Thừa Tướng / Binh Mã Đại Nguyên Soái / Tiên Phong Quan") | ⚠️ |

Hàng đáy: BtnStorePersonalOffer/StoreTongMoney/StoreBuildFund ✅ · BtnLeaveTong ✅ · **BtnRetire** (tự thoái ẩn,
Tip "hơn 1 tháng không lên mạng…") ❌ · BtnHelp ☠️ (0 ref, sprite cũng thiếu trong pak).

Panel phải: xem mục 1(B) — thiếu 3 chế độ danh sách bang.

### 2.4 Trang Phân phối chức năng `帮会权限分配页面.ini` (31 section)

- 14 ô quyền + Chọn tất cả + Phân quyền ✅ (mặt nạ 14 đúng gốc).
- **`[BtnSubPage_0] "Quyền hạn"` / `[BtnSubPage_1] "Phân phát"`** ❌: ini ta có `Rt_BtnSubPage_0/1` (:36,60 / :136,60)
  và khoá `SubPage=` trên từng ô, nhưng C++ **không Init hai nút**, không đọc `SubPage`, và `SwitchPage` **Show cả 14 ô
  cùng lúc** (:1311-1318). Hệ quả đo trong ini ta: `Rt_BtnFundManagement (25,81)` đè **`Rt_BtnDepose (25,81)`**,
  `Rt_BtnWeekGoalManagement (149,81)` đè `Rt_BtnChangeCamp`, `Rt_BtnCityManagement (25,107)` đè `Rt_BtnChangeTitle`,
  `Rt_BtnStuntManagement (149,107)` đè `Rt_BtnKickOut`. Bốn ô sau AddChild sau nên **vẽ đè và nuốt chuột** ⇒ 4 quyền
  "Bổ nhiệm / Đổi phe / Đổi tên / Trục xuất" **không bấm được, nhìn thấy chữ chồng** — đây là DOT9 #16 **chưa làm**.
  Khả năng cao là một trong các "tính năng bị sao" chủ game thấy.

### 2.5 Trang Chiêu mộ `帮会招募页面.ini` (25) và trang 2×2 `其它帮会招募页面.ini` (39)

- Chiêu mộ ✅ trọn bộ (văn án, khuynh hướng, 4 hoạt động, 2 ngưỡng cấp, Lưu, danh sách đơn 8/trang + Đồng ý/Cự tuyệt).
  ⚠️ `QingXiangBtn/HuoDongBtn` gốc là **menu thả** (`MenuItemCount=6/11`, exe đọc) — ta bấm xoay vòng (DOT6 #22 chấp nhận).
  `Rec_ToggleRecruit` đã ẩn đúng (gốc không có công tắc).
- 2×2 ✅ đúng blueprint; ⚠️ "Xem chi tiết" = hộp thoại chữ (gốc = mở trang tin tức bang đó, mục 1A).

### 2.6 Trang Tác phường `帮会作坊信息页面.ini` (26) + `作坊子分页.ini` (12)

✅ số khu / giới hạn cấp / bảo trì ngày, 6 nút, lưới icon 7 khu (nền + cao sáng + cấp + khung chọn), hộp xác nhận
`[WarnInfo]`. ⚠️ gốc có 3 trang con icon (`SubPageBtn_0..2`) — ta 1 trang vì chỉ 7 khu (hợp lý).
`WorkshopEffectImg/Sound` (hiệu ứng khi thao tác) ❌ nhỏ.

### 2.7 Trang Nhật ký `帮会记录页面.ini` (15)

| Mục con | Gốc | Ta |
|---|---|---|
| Mục tiêu tuần | báo cáo (mục 1C) | ❌ một dòng chữ |
| Công cáo bang hội | `[AnnounceEditor] Enable=0` (chỉ đọc) + `[BtnEditAnnounce]` "Hiệu chỉnh thông báo" bật sửa (quyền 1903) | ⚠️ ta Init editor **luôn sửa được** cho mọi người (:843, :2210-2217), nút Edit = Lưu; quyền chỉ chặn ở server |
| Sự kiện / Lịch sử | RecordList 541×310 + cuộn | ✅ 16 dòng ring, MessageListBox |
| `[BtnLeaveWord]` "Tin nhắn bang hội" | ghi lời nhắn vào sổ | ⚠️ ta lấy chữ trong **khung sửa công cáo** làm lời nhắn (:2873-2882) — khung đó chỉ hiện ở mục "Công cáo", nên ở mục Sự kiện/Lịch sử bấm Lưu lời không gửi gì |

### 2.8 Cửa sổ con

| Blueprint | Ta | |
|---|---|---|
| `帮会发钱界面` phát ngân lượng/cống hiến | `UiTongGrant.ini` 16/16 section | ✅ (sprite tiêu đề `发放界面标题_银两` thiếu trong pak — DOT12 đã đổi đường) |
| `帮会职位任命` bổ nhiệm | `UiTongAssignBox0.ini` 13/13 | ✅ |
| `帮会委任大臣` uỷ nhiệm đại thần | không có | ❌ dùng hộp nhập số (2.3) |
| `帮会指派称号` chỉ định danh hiệu (`EditTitle MaxLen=80`) | JX1 có `UiTongChangeTitle` cũ, không nối từ cửa sổ JX2 | ❌ |
| `帮会创建单` tạo bang | `UiTongCreateSheet` JX1 cũ (khác bố cục, cùng chức năng) | ✅ |
| `帮会列表窗口` danh sách bang | không có | ❌ mục 1A |
| `城市管理界面` thuế/vật giá thành (Thái thú) | không có | ❌ thuộc hệ công thành — exe gốc có đọc |
| `帮会改造系统` 17 trang trợ giúp | không có | ☠️ exe không tham chiếu tên file |

---

## 3. NHỮNG THỨ "BỊ SAO" NGAY BÂY GIỜ (lỗi hiện hữu, độc lập với việc bổ sung tính năng)

1. **4 cặp ô quyền chồng khít** trên tab Phân phối (2.4) — 4 quyền không tick được. Nguyên nhân: chưa làm 2 trang con.
2. **Nút "Xem tin Bang khác" làm việc khác gốc** (mở chiêu mộ 2×2), trang danh sách bang không còn lối vào (1A).
3. **Thứ tự tab lệch gốc** (2.1) — người quen bản Linux thấy "Tác phường" nằm sai chỗ.
4. **3 nút có trên ini nhưng ẩn vĩnh viễn** vì `nAct=-1`: Chuyển (ngân quỹ→kiến thiết), Thiết lập (lãnh địa), Kỹ năng (tuyệt kỹ).
5. **Khung công cáo sửa được với mọi thành viên** — chỉ server chặn; gốc khoá cho tới khi bấm Hiệu chỉnh.
6. "Lưu lời nhắn" chỉ hoạt động khi đang ở mục Công cáo (2.7).
7. Trang Tin tức thiếu hàng "Tiền duy trì chiến bị tuần" và không đổi màu cảnh báo (2.2); trang Chức năng thiếu hàng "Kiến thiết dự trữ" (2.3).
8. Mục "Mục tiêu tuần" chỉ là dòng chữ (1C).
9. Nút "Chiêu mộ" ở trang con 1 vẫn là nút chữ 5 ký tự vì sprite `帮会信息-招人按钮.spr` chưa có trong pak (DOT9 #15, chưa vẽ).

## 4. THIẾU PHÍA MÁY CHỦ đứng sau các lỗ hổng UI (để ước lượng, chưa làm)

| Tính năng UI | Server hiện có? | Cần thêm |
|---|---|---|
| Xem tin tức bang khác (1A) | `PAGE_TONGLIST` chỉ 6 trường; `BuildClientView` chỉ bang mình | tham số "bang đích" cho `JX2VIEW` INFO/MEMBER (chế độ chỉ đọc) — **đổi gói `TONG_JX2VIEW_COMMAND`/thêm trang ⇒ client + GS build cùng nhau** |
| Danh sách bang liên minh / quốc gia (1B) | field 10 UnionID, 50-54 | trang view mới liệt kê bang cùng UnionID (+ Minh chủ) |
| Mục tiêu tuần (1C) | field 20-36 + Lua `jitan.lua` đã nạp | gói sync trang WEEKGOAL (server → client), không cần op mới |
| Chuyển quỹ 2 chiều | không có COP | 2 COP mới + hook gốc `MONEYFUND2BUILDFUND` / `BUILDFUND2WARFUND` (tong_mix.lua :195) + quyền 3001 |
| Thoái ẩn / ép thoái ẩn (figure 4) | `COP_SET_FIGURE` chỉ 1..3 (DOT6 #27) | 2 COP + luật gốc `MEMBER_RETIRE_R` (≤50 %, offline ≥7 ngày, quyền 1902) |
| Nhường bang chủ | JX1 cũ có trọn bộ: `enumTONG_COMMAND_ID_APPLY_CHANGE_MASTER` (KProtocolDef.h:519), `KPlayerTong::ApplyChangeMaster` (KPlayerTong.h:72), relay CHANGE_MASTER/ACCEPT_MASTER | nối nút `BtnDemise` vào đường JX1 (**lưu ý** gốc Linux trừ tiền: stringtable_relay :263-264 "ngân quỹ bang bị khấu trừ %d vạn"; đường JX1 kiểm túi tiền JX1 = 0 — bẫy HAI TÚI TIỀN DOT10 #1) |
| Đổi danh hiệu nam/nữ/cá nhân | JX1 cũ có `APPLY_CHANGE_TITLE` / `APPLY_CHANGE_SEX_TITLE` (:520-521), `ApplyChangeTitle/ApplyChangeSexTitle` (KPlayerTong.h:88-89), relay `G_TONG_MALE/FEMALE/MEMBER_TITLE_CHANGE` | nối 3 nút + cửa sổ `帮会指派称号` (tái dùng `UiTongChangeTitle`) |
| Thiết lập lãnh địa | `COP_MAP_SET` 17 | chỉ client |
| Chọn tuyệt kỹ | `COP_SETSTUNT` 9 | chỉ client (hoặc giữ NPC) |
| Uỷ nhiệm đại thần bằng cửa sổ | `COP_MINISTER_SET/FIRE` | chỉ client (ini `帮会委任大臣` chép sang) |

## 5. THỨ TỰ ĐỀ XUẤT (khi chủ game duyệt)

1. Vá lỗi thuần client, không đổi gói: chồng ô quyền (2 trang con) · thứ tự tab · hàng StandFund · hàng StoredBuildFund
   · khoá khung công cáo · lời nhắn · 3 nút `nAct=-1` nối vào COP sẵn có (Thiết lập, Kỹ năng) — ~1 ngày.
2. **Xem tin tức bang khác** (1A) + danh sách bang liên minh/quốc gia (1B): thêm chế độ xem chỉ-đọc ở `BuildClientView`
   + cửa sổ danh sách gốc + nút "Vào bang khác" — đổi gói, build client + GS cùng lúc — ~2-3 ngày.
3. Mục tiêu tuần (1C): gói sync mới + dựng chuỗi theo `G_STR_*` — ~1 ngày.
4. Nhóm nhân sự còn thiếu: nhường bang chủ, đổi danh hiệu (nối đường JX1 cũ), thoái ẩn (COP mới) — ~2 ngày.
5. Chuyển quỹ 2 chiều — 0,5 ngày (server + client).

## 6. CÂU HỎI CẦN CHỦ GAME CHỐT

1. "Xem tin tức bang" chủ đang nói là **(A)** xem trang tin tức của bang khác, **(B)** danh sách bang liên minh/quốc gia
   ở trang Chức năng, hay **(C)** báo cáo Mục tiêu tuần? (Tôi nghiêng về A vì nút đáy gốc ghi đúng chữ "Xem tin Bang khác".)
2. Có làm lại cửa sổ danh sách bang **đúng bố cục gốc** (cửa sổ nhỏ 120×463 một cột tên, bên trái) hay dùng lại trang
   3 cột đã có nhưng đang bị giấu?
3. Nhường bang chủ: theo gốc Linux **trừ ngân quỹ** (bao nhiêu vạn — chưa dịch ngược số) hay miễn phí?
4. Hàng "Chi phí bảo trì" chồng toạ độ với "Đẳng cấp kiến thiết" trong chính blueprint — chủ có ảnh chụp client gốc trang
   Tin tức để tôi biết bản VN hiện hàng nào không?


## 7. THI CÔNG [BH100 07/09] — làm 100% theo bản Linux (đặc tả rút từ `game_y.exe` + mã đã sửa)

Chủ game chốt: "làm toàn bộ theo linux 100%" và "phần xem tin tức thành viên bang hội hiện tại cũng khác". Mục này ghi
đặc tả gốc (dịch ngược `game_y_unpacked.bin`, công cụ `rev.py` ở scratchpad phiên 4b437a48) và cách đã thi công.

### 7.1 Panel thành viên (khác biệt chủ nêu)
| Bản gốc (exe) | Bản cũ của ta | Đã làm |
|---|---|---|
| 3 cột thật: Hạng `[TxtRank]` x343 · Tên `[TxtTitle]` x395 · giá trị `[TxtType]` x468; 25 dòng/trang (`MaxMsgCount=25`) | 1 dòng chữ `%2d %-16s %-12s`, 10 dòng/trang, panel 6 dòng nền xanh (DOT12 tự chế) | `m_Row`/`m_MList`/`m_RowDim` = 3 cột, bước 13 px, 25 dòng (`Row16..24` thêm vào ini), sắp xếp + "online trước" làm **trên GS** (`BuildClientViewEx`, `SJX2Row`, `sJX2_CmpRow`) |
| Bấm tên (`LIST_ITEM_ACTIVE`) → **tooltip** 5 dòng: `<màu chức vụ>Danh hiệu:<chức vụ>\ndanh hiệu:<danh hiệu ghế>\n\nĐẳng cấp hiện tại / Điểm cống hiến hiện tại / Thời gian nhập bang` (màu ffff33/00ffff/9966ff/999999/555555 theo chức vụ; số màu xanh lá); bấm kép → menu người chơi; chuột phải: không | panel 6 dòng đè lên danh sách | `ShowMemberTip` qua `g_MouseOver` (màu tên JX1: yellow/cyan/purple/gray/DBlue); bấm kép (2 lần < 400 ms) → menu "Mật đàm" (`KUiPlayerBar::InputNameMsg`) — JX2 có menu người chơi đầy đủ, JX1 chỉ có mật đàm |
| Cột giá trị theo menu `[Fun_BtnMemberSortMenu]` 0 cấp / 1 TB ngày / 2 tuần (KV11) / 3 mục tiêu tuần (KV9) / 4 ẩn sĩ / 5 chức vụ / 6 `yy/mm/dd` hoặc `--/--/--` | xoay vòng khi bấm nút | `KPopupMenu` 7 mục đọc từ ini (`PopupIniMenu`), chọn → xin lại trang |
| Danh hiệu ghế = relay (`GetMasterTitle/DirectorTitle/ManagerTitle/MemberTitle`) | không có | `STONG_JX2_ONE_MEMBER.m_szTitle` (relay→GS) + `TONG_JX2_ONE_MEMBER.m_szTitle` (GS→client) |
| Ẩn sĩ = chức vụ 4 | không có | GS: `sJX2_DispFigure` = 4 khi bang chúng có KV16 (ngày thoái ẩn); Nhân số không tính ẩn sĩ |

### 7.2 Tab + trang
- 6 tab đúng gốc: `BtnBaseInfo` / `BtnZhaoMu` / `BtnWorkshop` / `BtnFunUse` / `BtnRightManage` / `BtnTongRecord` (Tab0..4 cũ bỏ; nút riêng `m_BtnFun` ẩn).
- **Phân phối**: 2 trang con `Rt_BtnSubPage_0/1` (đọc `SubPage=` mỗi ô), Chọn tất cả chỉ tác dụng trang con đang mở,
  Phân quyền hỏi `[PageBg2] PromptInfo` "…giao quyền này cho %s?" và chỉ sáng khi người chọn là Trưởng lão.
- **Nhật ký**: mục 0 = **báo cáo Mục tiêu tuần** (gói `TONG_JX2_WEEKGOAL_SYNC` field 20-36 + KV9/10 + cờ 1006; chuỗi
  `G_STR_TONG_DATE_DESC/WEEKGOAL_LEVEL/TONG_WEEKLY_AIM/PERSONAL_WEEKLY_AIM/PROCESS_STATUS/TONG_PRIZE`, tiêu đề
  `CurWeekDesc/LastWeekDesc`, tên sự kiện `[Rc_EventType]`; ngày = `8 − số ngày còn lại` như exe); mục 1 = công cáo **chỉ đọc**,
  `BtnEditAnnounce` bật/tắt khung sửa, `BtnLeaveWord` gửi; mục 2 `BtnLeaveWord` = lời nhắn; mục 3 ẩn cả hai.
  Dòng nhật ký có tiền tố `YYYY-MM-DD: ` (relay `JX2_SetString`).
- **Tin tức**: thêm hàng `TitleStandFund` (= (TV − ẩn sĩ) × field 17), đơn vị " vạn" cho 4 hàng quỹ, ngân quỹ theo bậc
  (`sTJX2_FmtMoney`: <1e8 nguyên, <1e12 "trăm triệu", còn lại "vạn trăm triệu"), chiến bị **đỏ** khi < StandFund, kiến thiết
  **vàng** khi đạt trần tuần + thanh tiến độ (`KTJX2Bar`), cống hiến cá nhân thanh tiến độ /22400, bấm tiêu đề → `HelpInfo` vào `Info_TxtHelp`.
- **Sử dụng chức năng**: thêm hàng `StoredBuildFund`; 10 nút gốc còn thiếu nối xong: Chuyển quỹ 2 chiều (COP 38/39),
  Thiết lập lãnh địa (COP 45 → `map_management.lua tongmap_management`), Kỹ năng (COP 46 → `tong_totempole.lua main`),
  Ép thoái ẩn (41), Chuyển vị (42), Đổi danh hiệu (43), Đổi tên nam/nữ (44), Thoái ẩn bản thân (40, nhãn `RetireLabel/UnRetireLabel`);
  ô (31,228) trang con 1: bang chủ thấy Chuyển vị, người khác thấy Chiêu mộ; nút nhóm chỉ hiện với bang chủ/trưởng lão;
  trang con 3 (Liên minh) panel phải = **danh sách bang liên minh** (`PAGE_UNIONLIST`, cột 3 " Minh Chủ "/"Liên minh bang hội",
  menu `[Fun_BtnTongSortMenu]`); Ủy nhiệm đại thần = cửa sổ 3 ô (`KUiTongAssignBox::ArrangeDataMinister`);
  hộp nhập dùng đúng chuỗi gốc (`G_STR_STORE_MONEY/BUILD_FUND_TITLE/INPUT_TONG_MONEY/TRANSFORM_BUILD_FUND/STORE_PERSONAL_OFFER`).
- **Xem tin tức bang khác**: `[BtnTongList]` mở cửa sổ danh sách bang (`KUiTongListJX2`, ini `TL_*`, sprite `查询别帮.spr` trong
  `update03.pak`), chọn bang → cửa sổ chính chế độ xem (`ViewTong`): chỉ trang Tin tức + thành viên chỉ đọc, tab khoá,
  nút đáy đổi ảnh `ImageEnterMapOther` "Vào bang khác" (COP 36 với `dwTarget`), Đóng → về bang mình; "Xem chi tiết" ở
  trang 2×2 chiêu mộ cũng vào chế độ này.
- Chiêu mộ: khuynh hướng / 4 hoạt động = menu thả xuống (`Rec_QingXiangMenu`, `Rec_HuoDongMenu`).

### 7.3 Giao thức / máy chủ (build client + GS + relay cùng lúc)
- `KProtocol.h` (2 bản đồng bộ): `LIST_ROWS`/`VIEW_MEMBERS` 10→25; `TONG_JX2VIEW_COMMAND` + `m_dwTarget/m_btSort/m_btOnline`;
  `INFO_SYNC` + `m_dwTongID/m_btViewOther/m_dwStandFund/m_wRetired`; `ONE_MEMBER` + `m_dwWeeklyOffer/m_dwRetireDate/m_szTitle`;
  `ONE_TONG` + `m_btUnionLeader`; trang 8 UNIONLIST, 9 WEEKGOAL; COP 38-46.
- GS: `SGDI_TONG_JX2VIEW2` + `SJX2_VIEW_REQ` (KSOServer → CoreServerShell → `BuildClientViewEx`); client core
  `GTOI_TONG_JX2_VIEW2` + `KUiTongJX2View` → `KPlayerTong::JX2_RequestViewEx`.
- Relay: `JX2MemberBrief.szTitle`; TOP 24 `SET_TITLE` (`JX2_SetTitleByNameID`), 25 `SET_SEX_TITLE` (đi `DBChangeSexTitle`),
  26 `DEMISE` (`JX2_DemiseByNameID` → `AcceptMaster` có sẵn).
- Luật thoái ẩn theo `MEMBER_RETIRE_R` (chỉ đội trưởng/bang chúng; ẩn sĩ ≤ 50 %; ép người khác cần offline ≥ 7 ngày; huỷ sau ≥ 7 ngày).
  Chuyển quỹ theo `MONEYFUND2BUILDFUND_R` (1000 lượng = 1 kiến thiết, không vượt trần tuần) và `BUILDFUND2WARFUND_R`.

### 7.4 Còn khác gốc (đã cân nhắc)
- Menu người chơi khi bấm kép: gốc JX2 có nhiều mục (giao dịch, tổ đội, kết bạn…); JX1 client không có hạ tầng đó → chỉ "Mật đàm".
- `帮会改造系统.ini` (trợ giúp), `BtnTongChallenge/Detect`, Liveness/DailyCost/BtnHelp: chết trong chính bản gốc — không port.
- Cửa sổ thuế Thái thú (`城市管理界面`) thuộc hệ công thành — ngoài phạm vi bang hội.


### 7.5 Rà soát lại sau khi chủ hỏi "còn thiếu không / nút đã đủ chưa" (đối chiếu exe lần 2)
Nguồn: `cm_funuse.txt` (74 control exe nạp ở trang chức năng), `wp_funuse_refresh` + `dis_refresh_tail*.txt` (0x4ea120),
`wp_funuse.txt` (bấm trang con), `wp_main.txt` (cửa sổ chính), `cm_baseinfo.txt`, `cm_zhaomu/workshop/otherzm`.

**Kết quả — mọi nút exe có xử lý đều đã nối; các chỗ chỉnh thêm ở vá 4 (`Game.exe.moi` mới):**
- Trang chức năng, đúng `refresh` gốc: mặt nạ tên trang con + nhóm tiền + nhóm trang con chỉ hiện với bang chủ/trưởng lão;
  sau đó **bật/tắt theo quyền** (2001 nâng cấp; 3001 rút/phát/chuyển quỹ/phát cống hiến; 1901 đuổi; 1902 ép thoái ẩn;
  1002 bổ nhiệm; 1004 danh hiệu; 1003 đổi phe; 2004 lãnh địa; 2006 kỹ năng; 1101 liên minh; chuyển vị chỉ bang chủ);
  nút nhân sự chỉ sáng khi đã chọn thành viên (không phải bản thân). Rời bang / Thoái ẩn ẩn với bang chủ (`figure > 0`).
- `BtnRecruit` và `BtnHelp` trang chức năng: exe **không nạp** → tắt (chiêu mộ ở tab riêng). `Van1/Van2` "vạn lượng": exe không nạp → bỏ.
- Bấm trang con gốc = `SetMode(0/1/2/0)` cho panel phải: trang con 2 → **danh sách toàn bộ bang** (nay có, `RenderTongListPanel`,
  sắp xếp theo `[Fun_BtnTongSortMenu]`), trang con 3 → bang liên minh, 1/4 → thành viên.
- Cửa sổ chính gốc 13 control: `BtnTongHelp` (562,3) exe mở hệ trợ giúp chi tiết JX2 (`详细帮助界面.ini`, chủ đề `帮会改造系统.ini`
  = 4 ảnh 800×600 trong `update03.pak`) → ta thêm `KUiTongHelpJX2` (ini `TH_*`) lật 4 ảnh Trước/Kế/Đóng. `BtnOtherZhaoMu`
  = tab duy nhất khi **chưa có bang** (ẩn 6 tab, chỉ trang 2×2). `m_Bot[2]` (trùng section `BtnTongList`) ẩn khi nút Danh sách bang hiện.
- `BtnAcceptUnionReq` không có trong exe (relay gốc duyệt kiểu khác) — giữ của ta vì luồng xin/duyệt liên minh của ta cần nút này.
- Không port (chết trong exe / không phải bang hội): `BtnTongChallenge/Detect` (Enable=0), `TitleWeeklyAim/TxtWeeklyOffer*`
  (không có trong ini VN), hàng Liveness/DailyCost/ServiceFee, thuế Thái thú.


### 7.6 Sau lượt test đầu của chủ (14:25 07/09) — 4 lỗi và cách sửa (vá 5)
| Lỗi chủ báo | Nguyên nhân (đo được) | Sửa |
|---|---|---|
| Thành viên bang hội không hiện | `jx_tongjx2.log`: sau `[REQ]` chỉ về `trangSV=0` (INFO), không bao giờ có `trangSV=1`. `TONG_JX2_MEMBER_SYNC` 25 dòng × 102 B = 2560 B > đệm `byOut[2048]` (KSOServer + `PushViewTo`) và `m_byMember[2048]` client → `BuildClientViewEx` trả 0, gói bị vứt | 3 đệm → 4096 (build lại CoreServer + GameServer + Game.exe) |
| Tác phường không hiện | ini VN `[BtnFunUse] Left=208` trùng `[BtnWorkshop] 209` (blueprint gốc + patch Linux đều 303): tab Chức năng đè lên tab Tác phường, ô 303 trống | `Left=303` |
| Huỷ liên minh không được | client đã gửi `op=32` 2 lần; GS chặn `if (!bMaster)` cho mọi op liên minh trong khi exe bật nút theo quyền 1101 (trưởng lão có quyền) | 5 op liên minh: bang chủ **hoặc** quyền 1101 |
| Thông báo bang đi kênh Hệ thống | `sJX2_Msg2Tong` dùng `SendSystemInfo(... MESSAGE_SYSTEM_ANNOUCE_HEAD)` | đi `AUC_MsgTongC` (đầu "Tin bang" + id kênh `\O<tong>` học từ relay — đường đã có của đấu giá); Lua `Msg2Tong` không kèm id kênh cũng lấy id đã học |

`ChayGameServer.bat` chỉ đổi tên `CoreServer.dll.moi` → đã thêm dòng `call :capnhat GameServer.exe` (bản cũ `.truoc_bh100`); `S3Relay.exe` vẫn phải chép tay khi relay tắt.


### 7.7 Test lần 2 của chủ (cửa sổ danh sách bang + tooltip) — vá 6
- **Bấm thành viên chỉ hiện "Mật đàm", không hiện thông tin**: cách "bấm rồi gọi `g_MouseOver`" bị chính nút dòng xoá ngay khi
  rê chuột (`KWndButton` chỉ hiện tip qua `WM_MOUSEHOVER` → hàm ảo `GetToolTipInfo`, và `Wnds.cpp:320` huỷ hover khi đổi cửa sổ con).
  Sửa: nút dòng là lớp `KTJX2RowBtn` kế thừa `KWndButton`, `GetToolTipInfo` trả 5 dòng thông tin thành viên (`GetMemberTip`,
  đã `TEncodeText`, ≤ 255 byte), đặt `SetToolTipInfo(" ")` để nút chịu báo hover. Rê chuột lên tên là hiện; bấm = chọn; bấm kép = Mật đàm.
- **Cửa sổ "Tìm bang khác" bị lệch**: blueprint gốc đặt tuyệt đối (50,45) cho màn 800×600; ta đặt sát trái cửa sổ chính, cùng đỉnh (`GetAbsolutePos` − 122).
- **Chữ Trước/Kế bị đè**: sprite `查询别帮.spr` đã nung sẵn chữ nút và giữa khung trong suốt (thấy cảnh game/khung chat); ta thêm nền tối
  `KTJX2Shade` (alpha 232) bên trong khung rồi tự vẽ nhãn Trước/Kế/Đóng (nút không ảnh) — bỏ nhãn trùng.

### 7.8 Lượt 3 của chủ (15:0x 07/09) — "UI Tìm bang khác sai, tìm đúng ảnh gốc" + "thông tin thành viên sai với gốc" + bộ test lệnh bài — vá 7

**Tooltip thành viên — giải mã hàm dựng chuỗi gốc `0x4db1f0` (game_y.exe), gọi từ `ShowMemberTip 0x4dc1c0`:**

```
<color=BẢNG[figure]>                       BẢNG 0x6e77a0: 0 ffff33 (bang chủ) / 1 00ffff / 2 9966ff / 3 999999 / 4 555555 (ẩn sĩ)
G_STR_NAME:<TÊN nhân vật>                  0x195 "Danh hiệu" (bảng VN dịch nhầm NAME), tên = tra NameID (+4) qua danh sách 0xc
G_STR_TITLE:<danh hiệu ghế>                0x196 "danh hiệu", chuỗi +0x24 của bản ghi thành viên (rỗng nếu chưa có)
<dòng trống>
G_STR_CURRENT_LEVEL: <color=0x33ff00>%d<color>        0x197, cấp +0xc
G_STR_CURRENT_OFFER: <color=0x33ff00>%d<color>        0x198, cống hiến +0x10
G_STR_JION_TIME: <color=0x33ff00>%d-%d-%d<color>      0x199, localtime(+8) → năm-tháng-ngày
```

Bản vá 6 in dòng 1 là **tên chức vụ** thay vì **tên nhân vật** và đóng màu sớm → chủ báo sai. Nay `GetMemberTip`/`ShowMemberTip`
dùng đúng chuỗi trên (`pM->m_szName`, màu `<color=R,G,B>` — `TEncodeText` nhận dạng dạng thập phân qua `TGetColor`, tên màu chỉ có 26).

**Cột giá trị panel thành viên** (hàm vẽ dòng `0x4dc7c0`, dạng `"%d\t%s\t…"`): chế độ 5 chức vụ in tên 0..4, ngoài khoảng in `"(%d)"`;
chế độ 6 ngày `"%02d/%02d/%02d"` hoặc `"--/--/--"`; các chế độ số in `"%d"` (vá 6 in `%.1f` cho TB ngày → đổi về số nguyên).
Nhánh "tên khớp 3 chức quốc chiến (`G_STR_NW_MINISTER/MARSHAL/PIONEER`, dữ liệu 0x138b/0x138c)" là hệ quốc chiến JX2 — JX1 không có, bỏ.

**Cửa sổ "Tìm bang khác" — ảnh gốc:** giải mã `查询别帮.spr` (update03.pak, 120×463, 1 khung, bảng 256 màu; bộ giải `spr2png.py`
theo `KBmp2Spr::ConvertLine` [đếm][alpha][điểm…]): **không có điểm trong suốt** — thân là alpha **192** (mờ tối), khung/chữ alpha 255,
chữ nung sẵn **"Tìm bang khác" / "Trước" / "Sau" / "Đóng"** (không phải "Kế"). Lượt trước tôi tưởng thân trong suốt nên chồng
`KTJX2Shade` + nhãn tự vẽ (đè lên chữ nung) → sai. Sửa: bỏ shade + nhãn (`Label=` trong `TL_Btn*`), 3 nút chỉ là vùng bấm,
`[TL_Main] Trans=1` để Represent vẽ theo alpha từng điểm (Trans=0 = OPACITY bỏ alpha). `关闭.spr` cùng thư mục là nút Trung
(chữ 关闭, 3 khung) không được ini gốc dùng → bỏ. Vị trí vẫn sát trái cửa sổ chính (blueprint 50,45 tuyệt đối gây lệch).

**Bộ test bang hội trên lệnh bài admin — viết lại đầy đủ:** `script\kiemthu\item\test_banghoi_admin.lua` (933 dòng, TCVN3, gương
`serverscript_live`), Include từ `lenhbaiadmin.lua` (đường dẫn thật, không bí danh), mục `ADM_TestHoatDong` → "Bang hội - bộ test đầy đủ"
(`BH_TestRoot`; bộ cũ `TX_Root` giữ là "công thành (bộ cũ)"). 9 mục: hồ sơ + bảo trì ngày/tuần + nâng/hạ cấp + toàn server;
thành viên (danh sách 25, chọn thành viên, chi tiết mọi khoá, cống hiến ±, quyền 1901/1002/1101/2004/9001 cấp-gỡ, thoái ẩn, đuổi có hỏi);
kinh tế 12 lệnh (quỹ, kiến thiết, chiến bị, dự trữ, tổng, tuần/trần, phí duy trì, góp `ContributeOffer`, phát nhóm/thành viên, cá nhân);
mục tiêu tuần (đặt cấp/sự kiện/tổng/cá nhân/thưởng, cộng điểm, hoàn thành ngay, chốt tuần); nhật ký/lịch sử/công cáo/tuyệt kỹ/ngoại hình/
tạm ngưng/log; lãnh địa + tác phường (gọi `TLD_/TPT_` cũ + `TWS_MaintainAll`); bang khác (liệt kê 12 bang, liên minh, xin vào bang
`TONG_ApplyJoin`, tìm theo tên); hoạt động (công thành `TCT_`, vận tiêu bang `VT_ADM_Bang`, bang chiến `HD_`, danh hiệu-thuế `TDT_`,
`TongClaimWar`); tiện ích (vào bang test, rời bang, `TONG_ApplyInit`, nạp lại). Không có lệnh Lua cho: thăng/giáng chức, chuyển bang chủ
(`SetTongMaster` là stub), lập/duyệt/rời liên minh, danh hiệu ghế — menu ghi rõ "chỉ qua cửa sổ bang hội".
Kiểm cú pháp bằng Lua 5.4 thật (`luacheck54.py` qua `lua54.dll` của Wireshark): 3 tệp OK. Bẫy: TCVN3 không có chữ HOA có dấu
(À/Ầ/Ọ…) → `tc.enc` báo lỗi từng dòng, phải viết thường.

Triển khai 15:26: `Game.exe.moi` f4eb3bdd (client, chờ `ChoiGame.bat`), `UiTongJX2.ini` 4f65666a chép thẳng vào `bin\client\Ui\Ui3`
(bản cũ `.truoc_bh100b`) + `J:\CayChay\pakgame\volamngaothe\PATCHFULL_NGAOTHE_MK_123456\Ui\Ui3`; script server sống ngay (lệnh bài dofile).
Máy chủ đang có `CoreServer.dll.moi` của phiên khác — không đụng.

### 7.9 Lượt 4 của chủ (16:xx 07/09) — trần 150 thành viên, "Đẳng cấp bang hội chưa hoạt động", hộp thoại lệnh bài thiếu nút, thêm test công thành / Thái thú — vá 8 [BHLV] [BH150]

**Trần thành viên 150/bang — khảo sát:** không có trần nào < 150 trong mã: relay chặn ở `CTongControl::AddMember`
(`m_nCapSize` = `relay_config.ini [tong] tongcap`, live đang **160**); GS `sJX2_DoApplyJoin` không giới hạn; JX2 gốc không có trần
theo cấp bang (chỉ `stuntData MaxMemberCnt` cho tuyệt kỹ). Hai con số thấp hơn 150 đang có: (a) `[tongjx2] NormalMemberLimit`
mặc định 100 (JX2 `[MoneyToExp]`: trên mức này mỗi người trừ thêm 50 lượng/750 s khi đổi quỹ → kinh nghiệm) → thêm mục
`[tongjx2] NormalMemberLimit = 150` vào `relay_config.ini` (áp khi relay khởi động lại); (b) bang bot `settings\simcity\botbang.txt`
cột SONGUOI 120 → **150** cho 5 bang bot (bản cũ `.truoc_bh150`, `nMuc = bb.nSoNguoi` không có kẹp trong mã).

**Đẳng cấp bang hội:** ô "Đẳng cấp" (`TxtTongLevel`) = `m_nTongLevel` = cấp JX1 của relay (`CTongControl::m_nLevel`), trước nay
chỉ đổi qua `SetTongLevel` của script JX1 (`lib_ham.lua UpdateTongExpAndLevel`, bảng `DIEMBANGHOINANGCAP`, gọi khi giết boss)
→ bang JX2 luôn 0. Bản Linux: quỹ → `[MoneyToExp]` (relay, 750 s, quỹ > 1.000.000: −5.000 lượng, +120 kinh nghiệm) → field 6 →
**cấp bang = `[LevelExp]` của `settings\tong\tong_setting.ini`** (MaxLevel 100, cấp n = n²×1000: 1.000, 4.000, 9.000 …) →
`TONG_GetExpLevel` (Linux `infocenter_head.lua:719` dùng). Thi công [BHLV]:
- Relay `KTongJX2Relay.cpp`: `JX2_ExpLevelOf(exp)` đọc `..\server\settings\tong\tong_setting.ini [LevelExp]` (thiếu → n²×1000),
  `JX2_CheckExpLevel()` chỉ NÂNG `m_nLevel` (không hạ) và đi đúng đường `DBChangeTongLevel` (đồng bộ thành viên online
  `STONG_BE_CHANGED_LEVEL_SYNC` + câu "Bang hội thay đổi đẳng cấp thành %d" trên kênh bang; người gọi lưu DB). Gọi sau
  `JX2_MoneyToExpTick` (tick 750 s) và sau lệnh field 6 từ GS (`JX2_ProcTongField`), nên `TONG_ApplyAddTaskValue(nT, 6, x)` lên cấp ngay.
- GS `KTongJX2.cpp`: `TONG_GetExpLevel` thật (trước là `DEF_TONG_GETF(ExpLevel, 6)` = trả kinh nghiệm), `TONG_GetLevelExpNeed(n)` mới,
  gói INFO `m_nTongLevel = max(cấp JX1 đồng bộ, cấp theo kinh nghiệm)` (xem bang khác cũng có cấp). Client không đổi.
- `lib_ham.lua UpdateTongExpAndLevel`: chỉ `SetTongLevel` khi cao hơn `GetTongLevel()` (bảng JX1 của boss không kéo cấp xuống).
- Kiểm: lệnh bài → Bang hội → trang 3 → "10. Cấp bang + kinh nghiệm bang": +1.000 / +50.000 kinh nghiệm → cấp 1 / 7 ngay; +2.000.000 quỹ → chờ 750 s.

**Hộp thoại lệnh bài:** `UiMsgSel.ini [Select_List] Height=92` chỉ vẽ 6 dòng, `[InfoText] Height=96` = 6 dòng → bộ test viết lại
(1461 dòng): 3 trang gốc, **mọi hộp thoại ≤ 6 nút** (kiểm tự động `SayEx` ≤ 6, `#Ham(...)` ≤ 31 ký tự), câu hỏi ≤ 3 dòng, luôn có
"Quay lại" + "Thoát/no" (trừ menu chọn 6 mục), trạng thái dài in `Msg2Player`.

**Công thành / Thái thú (mục 8, `BH_CT*`):** in 7 thành (chủ, Thái thú, thuế, đang chiến, đang báo danh); chọn thành 1..7 hoặc
thành đang đứng (`BH_CITY[tên admin]`); **bang tôi làm chủ thành = `AppointViceroy` (Thái thú = bang chủ, cấp danh hiệu 152+thành,
thu của chủ cũ, ghi ngày chiếm thành field 48)**; khiêu chiến giả `AppointChallenger`; trạng thái 0/1/2 `CTC_JX2_SetCityState`;
bang báo danh `NumOfSignUpTongs`; ép pha 18h/19h/20h/0h `CTC_JX2_Tick` (hỏi trước); kết trận `NotifyWarResult` công/thủ thắng;
thuế `CTC_JX2_SetTax` (mã trả về diễn giải); danh hiệu Thái thú `Title_AddTitle/ActiveTitle/RemoveTitle` + `SetPlayerTitle(167+thành)`;
reset 7 thành (`SetViewTongOwnCity("", i)`), ở Tiện ích trang 2. Bộ cũ `TX_Root` vẫn gọi được.

**Triển khai 16:0x:** `bin\server\CoreServer.dll.moi` 1ed4d726 (⊇ DELTA i b9b4cb4a đã swap 15:2x), `bin\multiserver\S3Relay.exe.moi`
26d7df5d (**chép tay khi relay tắt**, bat không swap relay; `relay_config.ini` mới cũng áp khi relay chạy lại), `Game.exe.moi`
f4eb3bdd (vá 7) vẫn chờ; script + `lib_ham.lua` sống ngay (lệnh bài dofile), `botbang.txt` áp khi GS chạy lại.

### 7.10 Lượt 5 của chủ (16:4x 07/09) — "phường thợ chưa lập được, kích không báo gì", "tìm các nút phường thợ để kiểm", "tạo lãnh địa xong không báo", "trap trong map lãnh địa chưa hoạt động" — vá 9 [BHWS] [BHMAP]

**Nút trang Tác phường (6 nút gốc `帮会作坊信息页面.ini`, đã nối đủ, ini `Ws_Btn*`, hỏi trước bằng `[Ws_WarnInfo]`):**

| Nút | Client → GS | GS → relay | Điều kiện / ai kiểm |
|---|---|---|---|
| Lập tác phường (`BtnLearnWorkshop`) | `COP_WS_ADD` khu đang chọn (bấm biểu tượng 1..7, mặc định 1) | `TOP_WS_OP` nAct 0 | quyền 9001/bang chủ (GS); khu chưa có (GS mã 9); relay: không tạm ngưng (field 44), số khu < `MAX_WORKSHOP_NUM` theo cấp KIẾN THIẾT (`tong_level_data.txt`: cấp 0 = **0 khu**, cấp 1 = 6…), đủ ngân sách kiến thiết (field 12) theo `workshop_setting` |
| Mở / Đóng (`BtnOpen/BtnClose`) | `COP_WS_OPEN` / `COP_WS_CLOSE` | nAct 1 / 2 | khu đã lập (GS mã 10); mở tốn `OPEN_FUND`, đóng miễn phí |
| Thăng cấp (`BtnUpgrade`) | `COP_WS_UP` | nAct 3 | cấp mới ≤ `WORKSHOP_UPPER_LEVEL` của cấp kiến thiết; đủ quỹ |
| Đổi cấp sử dụng (`BtnSetUseLevel`) | `COP_WS_SETLV` (xoay vòng 1..cấp) | lệnh field attr4 | ép ≤ cấp thật; relay echo field → GS đẩy lại trang |
| Huỷ tác phường (`BtnDelete`) | `COP_WS_DEL` | SET 0 sáu attr | quyền 9001 |
| Sử dụng khu | không có nút — dùng qua NPC Tổng quản trong lãnh địa (`ws_*.lua` → `TWS_ApplyUse`) | | |

**Vì sao "kích vào không báo gì":** GS trả mã 20 (im lặng) rồi relay báo kết quả bằng `sJX2_SayTong` = `g_ChannelMgr.GetChannelID("\O<id>", 0)` +
`SayOnChannel`; kênh chỉ tồn tại trên relay khi đã có người đăng ký (`B_Subscribe`) → không kênh = **nuốt** cả 9 câu báo (thiếu quỹ, quá số khu,
đã xây…). Thêm nữa thao tác thành công chỉ phát `TONG_SYNC` (không có `m_dwParam`) nên cửa sổ đang mở không được đẩy lại trang → không thấy
khu mới. Sửa [BHWS]: relay `sJX2_NotifyOp` = nói trên kênh (nếu có) **và** gửi `STRING_SYNC` kind mới `defTONG_JX2_STR_NOTIFY` (5,
`m_dwParam` = chỉ số người bấm) → GS `SendSystemInfo` thẳng cho người bấm; `sJX2_EchoField` echo field khu (có `m_dwParam`) → GS đẩy lại trang
Tác phường (đường VIEC7 có sẵn). 9 câu báo trong `TOP_WS_OP` đổi sang `sJX2_NotifyOp`; 3 chỗ thành công thêm echo.

**Tạo lãnh địa không báo:** `CreatMap` (tong_mix.lua) chỉ `TONG_ApplyCreatMap` → relay ghi field 45/46 rồi thôi; bản gốc relay tạo bản đồ
động rồi gọi lại `MAP_CREATED_R` (Msg2Tong "Xin chúc mừng…", ghi lịch sử/sự kiện) + `MAP_CREATED_G_2` (SetWorldName/SetMapType/SetMapParam,
NPC, toà xưởng) — JX1 không có đường gọi lại → im lặng. Sửa [BHMAP]: `CreatMap`/`PublicMap` tự phát thông báo (Msg2Player + Msg2Tong +
ghi lịch sử/sự kiện) và gán `SetMapType(idx,1)`/`SetMapParam(idx,0,tong)`; relay echo field 45 (`m_dwParam`) → GS đẩy lại trang Tin tức
(nút "Vào bổn bang" đổi ngay); `TOP_DELETE_MAP` báo "đã huỷ khu vực". GS `COP_ENTER_MAP` gán lại chủ bản đồ mỗi lần vào (khu riêng =
field 46 ≠ 0 → bang; khu chung 586/595–597 → 0) vì GS chạy lại mất bảng. NPC/toà xưởng 11 bản đồ đã đặt sẵn lúc boot
(`startgame\tongjx2npc.lua JX2Tong_AddTerritoryNpc` → `add_tongnpc()`), không đặt lại khi tạo.

**Trap lãnh địa:** `kiem_trap_map.py` (đã quét cả `maps.pak`) → 36 ID trap "chết từ trước", **24 = 8 khu × 3 trap của 11 bản đồ 586–597**
(`\script\tong\map\{public,bianjing,chengdu,dali,fengxiang,linan,xiangyang,yangzhou}\{entrance,spacenorth,spacesouth}_trap.lua`, băm đường
dẫn Linux; cây thật ở `scriptjx2\tong_vn\map\`, `_duongdan_cu.txt` không có dòng nào cho `script\tong\`). Thêm 28 bí danh (24 + 4 tệp gốc).
Chưa đủ: `entrance_trap.lua` gọi `GetMapType/GetMapParam` (KSubWorld bản Linux) — JX1 **không có** 4 hàm này (`SetMapType/GetMapType/
SetMapParam/GetMapParam`, thêm `SetWorldName`) → thêm vào Core (`g_TongJX2.m_mapMapType/m_mapMapParam`, khoá = chỉ số SubWorld, không đồng
bộ). Trap north/south (SetPos + TaskTemp 2777 + SetTempRevPos/SetRevPos/SetCurCamp) dùng hàm có sẵn. Còn 12 ID trap chết khác không thuộc
lãnh địa (chưa động).

**Triển khai:** CoreServer.dll + S3Relay.exe build lại (⊇ DELTA k b1308b4c); client không đổi. Script sống ngay (tong_mix.lua qua ExecuteScript2
đọc tệp; bí danh chỉ đăng ký lúc GS boot → cần chạy lại GS = lúc swap).
