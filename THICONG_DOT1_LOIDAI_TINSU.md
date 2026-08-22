# PHIẾU THI CÔNG ĐỢT 1 — LÔI ĐÀI BANG HỘI + TÍN SỨ

> Bắt đầu 21/08/2026. Tài liệu này **cập nhật liên tục trong lúc thi công**.
> Nền: `PHANTICH_LOIDAI_TINSU_BANGHOI.md` + `DIEUKIEN_THAMGIA_9_HOATDONG.md`.

## Lệnh gốc của chủ game (21/08)

1. Làm **Lôi Đài Bang Hội** + **Tín Sứ**; bản dự án trùng thì **gỡ đi, thay bản Linux vào**.
2. **Giống 100% bản Linux từ item đến hình ảnh**, chỉ đổi **giới hạn cấp độ → 90** và **bỏ trùng sinh**.
3. Tính năng khác bản Linux có sẵn và đầy đủ thì **làm luôn**.
4. **Ưu tiên: hoạt động bang hội + BOSS BANG HỘI.**
5. Thiếu item thì **làm thêm item**.
6. Làm luôn **bang hội chiếm lĩnh trên bản đồ + thông tin thuế mỗi thành**.
7. **Nhiệm vụ lúc nhận phải hiện thông tin ở Chỉ Nam Nhiệm Vụ (F11).**

Chốt phạm vi: Tín Sứ **chép đúng hiện trạng Linux** (2 tuyến Thành Đô ↔ Đại Lý, 1 ải Thiên Bảo Khố
map 395). **Chủ game tự restart** sau mỗi đợt.

---

## 1. ITEM TÍN SỨ — ĐÃ GIẢI QUYẾT XONG ✅

Quét toàn bộ `task/tollgate/messenger` + `item/messenger` bản Linux: **chỉ 9 item id được dùng thật**.
Đối chiếu sang dự án **bằng TÊN** (không bằng số — bẫy "lệch 1 chỉ số" đã ghi trong memory):

| Linux id | Tên | **Dự án id** | Ảnh `.spr` | Ghi chú |
|---|---|---|---|---|
| 402 | Thần bí Đại Hồng Bao | **401** | — | lệch −1 |
| 885 | Tín Sứ Mộc yêu bài | **884** | `\spr\item\task\item_xinshimu.spr` | **trùng đường dẫn ảnh** |
| 886 | Tín Sứ Đồng yêu bài | **885** | `…item_xinshitong.spr` | trùng |
| 887 | Tín Sứ Ngân yêu bài | **886** | `…item_xinshiyin.spr` | trùng |
| 888 | Tín Sứ Kim yêu bài | **887** | `…item_xinshijin.spr` | trùng |
| 889 | Ngự Tứ Tín Sứ yêu bài | **888** | `…item_xinshiyuci.spr` | trùng |
| 2566 | Hành Hiệp Lệnh | **2575** | `\spr\item\script\xingxialing.spr` | trùng |
| 2812 | Tín Sứ Bảo Rương | **3430** | `\spr\item\script\xinshibaoxiang.spr` | trùng |
| 2813 | Thiên Bảo Khố Lệnh | **3431** | `\spr\item\script\tianbaokuling.spr` | trùng |
| 30229 | Chân Nguyên Đơn (Đại) | **4847** | `\spr\item\kinhmach\channguyendon.spr` | bản kinh mạch của dự án |

Nhóm phụ (Ngũ Hành Phù dùng trong ải, gọi từ `\script\item\` ngoài cây messenger):

| Linux | Tên | Dự án |
|---|---|---|
| 2806 | Ngũ Hành Phù | **3424** |
| 2807…2811 | Triệt Kim / Mộc / Thuỷ / Hoả / Thổ Phù | **3425…3429** |

**Kết luận: KHÔNG PHẢI LÀM THÊM ITEM NÀO, KHÔNG PHẢI VẼ ẢNH NÀO.**
14/14 item đã có sẵn trong `settings\item\magicscript.txt` của dự án, **đường dẫn `.spr` trùng
từng ký tự** với bản Linux ⇒ hình ảnh trong game sẽ giống hệt.

**Việc phải làm:** đổi 9 id trong script khi chép sang + **bind cột `Script` (cột 10)** của 5 yêu bài
và 6 phù về `\script\item\messenger\toll_*.lua` (hiện đang là `0` = chưa gắn).

> Công cụ tái lập: `ReverseTools/item_remap.py <thư-mục-Linux>` — trích mọi `(6,1,N)` rồi tra ngược
> theo tên sang bảng dự án. Dùng lại được cho mọi đợt sau (bang hội, boss bang hội…).

---

## 2. LÔI ĐÀI BANG HỘI — trạng thái

Script đã port (trùng 9/10 tệp từng byte), map 213-220 + region data đủ, timer 16/17 + mission 9
đã khai. **0 item cần, 0 ảnh cần.**
⚠️ Câu "0 hàm engine thiếu" ở bản đầu của mục này **ĐÃ SAI** — nó chỉ đúng ở tầng Lua.
Tầng C++ có 3 stub chặn cứng: **xem mục 4.1**.

**Ba điểm nối dây đang bị comment tắt:**
| Tệp : dòng | Nội dung |
|---|---|
| `script/startgame/thon/balanghuyen.lua:80` | `-- AddNpcNew(373,…,"\script\tinhnang\loidai\vebinhdautruong.lua",…)` |
| `script/timerserver.lua:76` | `-- sukien_loidaibanghoi(nDyfW,nHr,nMi)` |
| `script/item/lenhbaiadmin.lua:25` | `-- Include(".../loidai/lib_loidai.lua")` |

**Phải gỡ (bản tự viết trùng chức năng):** `script/tinhnang/loidai/` + các điểm gọi trong
`missions/mission06.lua`, `timertask/task06.lua`, `timerserver.lua:743-770`.

**Đã điều tra xong** — kết quả ở mục 4.1: NPC nằm trong dữ liệu bản đồ nhưng engine dự án bỏ qua,
`manager.lua` là mã chết, trap thì đủ.

**Sửa điều kiện theo chính sách cấp 90:**
| Tệp : dòng | Hiện tại | Đổi thành |
|---|---|---|
| `missions/citywar_arena/camper.lua:81` và `:87` | `GetJoinTongTime() >= 7200` (5 ngày) | bỏ, thay `GetLevel() >= 90` |

---

## 4. KẾT QUẢ ĐIỀU TRA CHẶN-ĐƯỜNG (workflow 9 tác tử, 2,0 triệu token, 2 vòng phản biện)

### 4.1 ĐÍNH CHÍNH NẶNG: Lôi Đài Bang Hội KHÔNG phải "0 hàm engine, gỡ 3 dòng comment"

Phân tích trước chỉ đối chiếu tầng **Lua**. Tầng **C++** có stub:

| Chặn | Bằng chứng | Phải làm |
|---|---|---|
| `IsArenaBegin` trả **cứng 0** | `Sources/Core/Src/KJx2CityWar.cpp:679` (chú thích `:675-678`: *"Nhom ARENA (E4) - idle"*) | Viết logic ghép cặp 2 bang + mở sân |
| `GetArenaBothSides` trả rỗng | `KJx2CityWar.cpp:685` | Trả tên 2 bang được ghép |
| `GetArenaCityArea` trả 0 | `KJx2CityWar.cpp:692` | Trả khu thành |
| NPC map-data **bị engine bỏ** | `KRegion.cpp:474-484` lọc theo `g_NotAddNpcNormal`; `kind_dialoger=3` (`GameDataDef.h:1372`); `settings/gamesetting.ini:259 NotAddNpcNormal=1` | Tự `AddNpc` bằng Lua trong `InitMission` mission 9 |
| Ngay bản Linux cũng **không có cửa vào** | `citywar_global/infocenter_head.lua:34 PreEnterGame()` và `:53 EnterGame()` — **0 call site** | Tự dựng cửa vào |

Hệ quả: timer 18 poll `IsArenaBegin` luôn trả 0 → **không bao giờ `OpenMission(9)`**;
`camper.lua:26` luôn rơi vào nhánh *"Thời gian chiến đấu vẫn chưa đến!"*.

**Tin tốt (đo thật — tự viết bộ giải nén UCL nrv2b để đọc dữ liệu region trong `maps.pak` cả hai cây):**
- Map lôi đài có **đúng 2 NPC**: tpl 178 ô (1581,3257) chạy `camper1.lua`; tpl 124 ô (1603,3236)
  chạy `camper2.lua`. **Hai cây trùng từng trường.**
- `manager.lua` là **MÃ CHẾT** — không bản đồ nào trỏ tới.
- **Trap đủ**: 1 trap id `0x67E8E3EB`, băm `g_FileName2Id` khớp tuyệt đối tệp dự án **đã có**.
- **0 item, 0 ảnh** — `citywar_arena` không dùng vật phẩm nào, không tham chiếu `.spr` nào.

### 4.2 Tín Sứ — 4 hàm engine + 7 item nhánh phụ

- **4 hàm engine thiếu**: `SetSpecItemParam` · `ConsumeEquiproomItem` · `NpcName2Replace` ·
  **`GetAroundNpcList`** (phản biện tìm ra — 5 Triệt X Phù gọi ở dòng đầu `main()`).
- **~85 tệp .lua + 2 settings** phải chép; 1 dòng Include đổi sang `scriptjx2/tong_vn`.
- **Bản dump Linux THIẾU script đặt NPC Dịch Quan** (`especiallymessenger()` 0 call site).
- **7 item thiếu**, đều ở nhánh thưởng "Chìa Khoá Vàng" của Tín Sứ Bảo Rương:
  30301 · 30529 · 30537 · 30506 · 30507 · 30006 · 30505 → **làm thêm** theo lệnh chủ game.

> BẪY: `magicscript.txt` **hai cây KHÁC BỐ CỤC CỘT** — Linux cột **14** = script, dự án cột **10**.
> `AddGoldItem` **đảo thứ tự tham số** giữa hai cây.
> Bảng mặt nạ dự án tách thành cặp `(particular, level)`.
> `safeshow` **KHÔNG thiếu** — là hàm engine đã đăng ký (`ScriptFuns.cpp:13188`, `:14018`).

### 4.3 Chỉ Nam Nhiệm Vụ (F11) — phải viết mã CLIENT

Bảng F11 dự án là **C++ viết cứng cho ĐÚNG MỘT nhiệm vụ Dã Tẩu**
(`TASKGUIDE_DATAU_TASKID 6`, `UiTaskGuide.cpp:29`); TaskId khác in *"Chưa hỗ trợ hiển thị"*
(`UiTaskGuide.cpp:365-369`). Dự án **không có** bảng `task id → mô tả` (Linux có
`settings/task/taskguide.txt`, không có trong pak nào của dự án) và **không có tầng Lua giao diện**.

**Đường dữ liệu thì ĐÃ THÔNG**: `SetTask` → `KPlayerTask::SetSaveVal` tự đẩy mọi id xuống client
(`KPlayerTask.cpp:77-85`), id ≥ 256 đi kênh `UI_TASKVALUE` — server không cần gọi thêm hàm nào.
⇒ Việc phải làm: thêm mục vào `UI/uitasklist.ini` + **viết nhánh C++ `BuildTinSuText()`**. **Build lại client.**

### 4.4 Boss bang hội — dự án đã port gần đủ nhưng đã bị sửa khác gốc

Chuỗi 3 khâu: NPC **Tổng quản BINH GIÁP phường** (không phải Hoạt động phường — tài liệu cũ dẫn nhầm;
mục ở Hoạt động phường bị cổng vùng "cn" chặn nên chết) bán **Lệnh bài gọi Boss** bằng ngân sách
kiến thiết bang → vật phẩm **Boss Triệu Hoán Phù** (Linux `6/1/1022` → dự án **1023**) → dùng
**chỉ trong map bang hội 586-604** triệu **Boss Hoàng Kim cấp 95**.

Dự án đã có menu, item, `bosscharm.lua`, npc template, engine `TWS_ApplyUse`. Nhưng đã sửa khác gốc:
cấp boss, số phù, kịch bản rơi đồ, hẹn giờ 18h, +500k exp, và `bosscharm.lua:88 RemoveItem(nItemIdx)`
(gốc không có — giữ nguyên là **trừ phù 2 lần**).

> **HẠNG MỤC "PHẢI LÀM THÊM MAP" (không phải item):** map **592** và **598-604** (8/19 map bang hội)
> **không có `.wor` trong bất kỳ pak nào** trong 37 pak `bin/client/data`, và không có trong
> `WorldSet_GameServer.ini` (chỉ nạp 586-591 + 593-597 = 11 map). Cần **art bản đồ mới**.
> Trọng số bốc boss: bộ 511/513/523 chiếm **81%** (810/1000). `goldboss.txt` chỉ phủ **11/19 boss**.

### 4.5 Thuế thành — server gần đủ, CLIENT chết hẳn

Bản Linux hiện thông tin ở **ba nơi, tất cả đều client**: (1) bản đồ thế giới — 7 nhãn `PureTextBtn`
chuỗi `G_VICEROY_<thành>`; (2) bản đồ lớn — `[CityInfo1]` (Thái Thú + bang) và `[CityInfo2]`
(thuế + vật giá); (3) cửa sổ `KUiCityManage` cho Thái Thú chỉnh thuế.

Dự án: **tài nguyên client đã đủ** (`UiWorldMap.ini` 7 khu, `UiMiniMapBig.ini` có `[CityInfo1]/[CityInfo2]`,
`spr.pak` có sprite thuế suất) nhưng **mã client chết**: `UiWorldMap.cpp` chỉ set chữ Tương Dương,
`KUiMiniMap::LoadScheme` không nạp `CityInfo1/2`, `c2s_getcityowntong` **0 call site**.
Thêm nữa **hệ 7 thành chưa cutover**, `jx2citywar.txt` 7 dòng đều vô chủ, thuế 0.

---

## 5. KHỐI LƯỢNG THẬT SAU ĐIỀU TRA

| Đợt | Việc | C++ | Lua | Build client |
|---|---|---|---|---|
| 1a | **Lôi Đài Bang Hội** | 3 hàm arena `KJx2CityWar.cpp` | AddNpc + cửa vào + gỡ `tinhnang/loidai` + cấp 90 | không |
| 1b | **Tín Sứ** | 4 hàm engine | chép ~85 tệp + remap id + 7 item mới + gỡ `thienbaokho` | không |
| 1c | **Chỉ Nam Nhiệm Vụ** | nhánh `BuildTinSuText()` | `uitasklist.ini` | **CÓ** |
| 2 | **Bang hội + boss bang hội** | 17 hàm | `missions/tong` 41 tệp + kéo `bosscharm` về gốc | không |
| 3 | **Thuế thành** | mở 3 chỗ client + nối giao thức | cutover `timerserver` | **CÓ** |

**Duy nhất một thứ không làm được bằng mã: art bản đồ cho map 592 + 598-604 (8 map bang hội).**

---

## 6. ĐÃ THI CÔNG — BOSS BANG HỘI + 3 HOẠT ĐỘNG PHƯỜNG (21/08, chờ restart)

### 6.1 Boss bang hội (`c2ea67fa`, `612b7dbb`)
- `script\item\bosscharm.lua`, `script\misc\boss\callbossdeath.lua` ← chép Linux, **trùng từng byte**.
- `scriptjx2\tong_vn\workshop\ws_bingjia.lua`: gỡ cổng giờ 12h30–22h tự thêm quanh `use_g_1`.
- Engine: `AddNpcEx` (dịch ngược `0x0811BF40`) + `SubWorldIdx2MapCopy`. **DLL đang chạy (19:49) đã có.**
- Item: Linux 1022 → dự án **1023**, cùng ảnh, cột Script đã trỏ sẵn. Không làm item.

### 6.2 Ba Hoạt động Phường (`17339638`, `8bf2dc3c`)
**Script/settings đã đặt vào cây chạy thật:**
| Gì | Ở đâu | Ghi chú |
|---|---|---|
| 41 tệp `missions\tong\**` | `script\missions\tong\` | **trùng 100% Linux** (diff -rq sạch) |
| 3 tệp lịch relay | `script\missions\tong\relay\` | chép nguyên từ `gateway\s3relay\relaysetting\task\` |
| **`tong_driver.lua`** (MỚI) | `script\missions\tong\` | thay task-centre relay, khuôn `gsdriver.lua`; gọi `TaskShedule`/`TaskContent` của 3 tệp relay qua `DynamicExecute` (mỗi tệp 1 state) |
| `settings\maps\chrismas` (17) + `springfestival2006` (21) | `settings\maps\` | trùng Linux |
| `settings\task\task_id.txt` | cho `TaskNo` | |
| `settings\task\missions.txt` | nối 27→45; **37/38 đệ tử, 39/40 Niên Thú, 44/45 thu thập**, còn lại `mission_trong.lua` | tra theo SỐ DÒNG |
| `settings\TimerTask.txt` | +65–70, 75–77 (đúng số Linux) + **53** = `tong_driver.lua` | tra theo KHOÁ |
| `script\startgame.lua:101` | `DynamicExecute(tong_driver, "TONG_DriverInit")` | ngay sau dòng WLLS |
| `scriptjx2\tong_vn\workshop\ws_huodong.lua` | **gỡ khối stub 15/08** (16 dòng) | còn 3 lệch có chủ đích: item 1023, 2×`RemoveSkillState` |
| `tong_springfestival\head.lua:19` | `SF_LEVELLIMIT = 50` → **90** | chính sách chủ game |

**NPC template:** 1121 / 323 / 361 / 1141 **trùng cả id lẫn tên** với `npcs.txt` dự án → không remap.

**Engine: 18 hàm mới** (17 + `GetSpecItemParam`), mỗi hàm dịch ngược từ ELF, đặc tả ở
`ReverseTools\dac_ta_17_ham_hoatdong_phuong.json`. Điểm chặn thêm vào engine:
`KSkill::CanCastSkill` (cấm chiêu), `c2sPKApplyEnmity` (cấm cừu sát), `ChangeAuraSkill` (cấm vòng sáng),
`KPlayer::UpdataCurData` (áp lại chiêu tạm), client `s2cPlayerSync_MA` (SetImmedSkill).
**Giao thức:** +1 sub-id `enumS2C_PLAYERSYNC_ID_IMMEDSKILL` **cuối enum** ⇒ **client phải cập nhật** cùng server.

**Binary chờ:** `bin\server\CoreServer.dll.moi_hoatdongphuong` + `bin\client\CoreClient.dll.moi_hoatdongphuong`
(GameServer + Game.exe đang chạy nên không đè). 21/21 tên hàm xác minh có trong DLL.

**Lệch có chủ đích (ghi tại chỗ trong mã):**
- Linux gửi gói `0x63/0x13` để client xám thanh chiêu khi `ForbitSkill` — JX1 chỉ chặn server.
- `AddMapTrap` tham số 5 (`nParam`) không lưu theo ô — 0 call site dùng.
- `ChangeOwnFeature` nType 1 (có hạn) + nhánh `nIdx<0` coi như nType 0 — call site duy nhất là `(0,0,1141)`.
- `FileName2Id` không `tolower` như Linux — để khớp hàm băm engine đang dùng.
- **Không port `task/random`** (kéo cả hệ PARTNER/TASKSYS JX2) — chỉ phục vụ nhánh thưởng "quyển nhiệm vụ"
  của Thu thập vật tư, hoạt động **đã chết trên chính Linux** (menu comment + `settings/maps/dragonboatfestival_06` không tồn tại).

**Restart xong cần kiểm:** `[TONG] Driver hoat dong phuong da khoi dong` trong log; `ScriptError.log` không thêm;
NPC Tổng quản Hoạt động phường bấm được, menu không báo lỗi; đến giờ `mod(giờ,3)==0` (đệ tử) / `==1` (Niên Thú)
map 821/823 mở báo danh. Test ngoài giờ: `DynamicExecute("\script\missions\tong\tong_driver.lua","TONG_Adm_MoNgay",1|2)`.

## 3. NHẬT KÝ THI CÔNG

| Thời điểm | Việc | Trạng thái |
|---|---|---|
| 21/08 | Chốt phạm vi + chính sách cấp 90 / bỏ trùng sinh | ✅ |
| 21/08 | Đối chiếu item Tín Sứ (14/14 có sẵn, 0 phải làm thêm) | ✅ |
| 21/08 | Điều tra NPC placement / Chỉ Nam Nhiệm Vụ / boss bang hội / thuế thành | ⏳ đang chạy |
| — | Gỡ `tinhnang/loidai`, bật `citywar_arena` | ⬜ |
| — | Gỡ `tinhnang/thienbaokho`, chép cây `messenger` | ⬜ |
