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

## 7. ĐÃ THI CÔNG — TÍN SỨ (21/08 đêm, chờ restart) — phạm vi "chép đúng hiện trạng Linux"

### 7.1 Script (cây chạy thật `E:\...\bin\server`, mirror `serverscript_jx2\tinsu\`)
- **85 tệp chép nguyên byte** = bao đóng Include đệ quy (`ReverseTools\tinsu_closure.py`): `task\tollgate\messenger\**` (61), `item\messenger` (5), `item\xinshirenwu` (8), `task\tollgate\{addtollgatenpc,killbosshead,messenger_prize}.lua`, `dailogsys\{dailog,g_dialog}.lua`, `event\birthday_jieri\200905\{class,taskctrl,message\message}.lua`, `event\jiaoshi_jieri\200910\head.lua`, `tagnewplayer\head.lua`, `vng_event\change_request_baoruong\exp_award.lua`, `vng_feature\checkinmap.lua`.
  Bỏ `task\partner\master\partner_master_main.lua` (JX2 PARTNER) — Include ở 3 NPC ải được chú thích, phần dùng đã bị Linux chú thích sẵn.
- **Sửa có chủ đích** (`ReverseTools\tinsu_patch.py`, mọi thay thế đều assert số lần):

  | Tệp | Sửa |
  |---|---|
  | `posthouse.lua:132-133` | `GetLevel() < 120` → **90** (+ chữ trong thoại) |
  | `posthouse.lua` | 11 id item: 402→401, 885-889→884-888, 2566→2575, 2812→3430, 2813→3431, 30229→4847; `AddGoldItem(0,205)`↔`(0,206)` **hoán đổi** (GoldItem.txt dự án: 205 = Mục Túc, 206 = Kiếm Bài — ngược Linux) |
  | `xinshibaoxiang.lua` | 19 id: 2744→3362, 30191→2953, 4134→4752, 30228→4846, 30229→4847, 3203→4844, 30289→4848 (×3), 3811→4429, 3810→4428, 2812→3430, 7 item mới 30301/30529/30537/30506/30507/30006/30505 → **4857…4863** |
  | `wuxingfu.lua` | 10 id: 2806→3424, 2807-2811→3425-3429 |
  | `item\event\kinhmach\honnguyenchandon.lua` (MỚI) | = `vng_event\item\hunyuanzenyuan.lua` Linux, task 4000 → `TASK_CHANGNGUYENDAN` (362), thêm 2 Include tường minh |

- **Nối dây** (`ReverseTools\tinsu_wire.py`): `task\tollgate\tinsu_addnpc.lua` (MỚI: 18 dòng NPC chép nguyên `global\autoexec_npc.lua` Linux dòng 3-23 + `add_alltollgatenpc()` = 9 Bảo Rương 844 + 9 Bảo Khố Thủ Hộ Giả 849 map 395); `startgame.lua:18/107` Include + `tinsu_addnpc()` **thay `addnpcthienbaokho()`**; `global\npcchucnang\dichquan.lua` menu "Thiên bảo khố" → **"Nhiệm vụ Tín Sứ/especiallymessenger"** (Include posthouse.lua; Linux tự chặn "chỉ Thành Đô/Đại Lý"); `xaphu.lua` thêm mục 6 **"Đi nơi đặc biệt làm Nhiệm vụ Tín Sứ/messenger_wagoner"** (= Linux station.lua mục 9); `station.lua` Include wagoner.lua; `settings\maplist.ini` +`395_NewWorldScript`.
- **Gỡ** `script\tinhnang\thienbaokho\` (7 tệp Ken Nguyen) → `_backup_tinsu_2108\script\tinhnang\thienbaokho\`. Backup các tệp sửa ở `_backup_tinsu_2108\`.

### 7.2 Bảng item `settings\item\magicscript.txt`
- Bind cột Script (cột 10) cho 14 item: 884-888 → `toll_*paixinshi.lua`; 3424-3429 → `wuxingfu/che*fu.lua`; 3430 → `xinshibaoxiang.lua`; 3431 → `qianbaokuling.lua`; 4752 "Chân Nguyên Đan" (trùng tên+ảnh Linux 4134, đang là đồ chết) → `channguyendan.lua` (+10 chân nguyên của dự án).
- **7 item mới 4857-4863** (tên/mô tả/ObjIdx/stack chép nguyên Linux): Hỗn nguyên chân đơn (4857, script mới, +1000 chân nguyên, 1 lần/ngày), Đồ Phổ Đằng Long Y/Khí Giới (4858/4859), Tinh Sương Lệnh (4860), Huyền Thiết (4861), Đồ Phổ Tinh Sương Y/Khí Giới (4862/4863).
  🔴 **3 ảnh VNG không tồn tại ở bất kỳ pak nào trên máy** (`\spr\vng\item\dophodanglong.spr`, `201408_event_pk\bachkimlenhbai.spr`, `201410_event_thang10\huyenthiet.spr`) → tạm dùng `item_huangjintupu.spr` / `canglangling.spr` / `item_xuantiekuang.spr`. Chủ game kiếm được .spr thì đổi cột 5 của 4858-4861.
- Ánh xạ theo TÊN có cân nhắc: Hộ Mạch Đơn 3203 → **4844** (bản kinh mạch có script; 3821 cùng ảnh nhưng đồ chết), Chân Nguyên Đơn (trung/đại) → **4846/4847**, Huyết Long Đằng 30289 → **4848** (cấp 9/11/12 giữ tham số, hệ kinh mạch dự án chỉ đếm số).

### 7.3 Engine (`CoreServer.dll.moi_tinsu` 22:58, build sạch 0 lỗi; KHÔNG đụng client)

| Hàm / chỗ | Nội dung |
|---|---|
| `GetAroundNpcList(nDist[,nKind])` (KJx2WarInfra.cpp) | trả `tbList, nCount` NPC quanh người chơi trong bán kính nDist ô; tham số 2 (Linux luôn 8) bỏ qua — quái đích 849 có Kind 0 nên 8 không thể là bộ lọc Kind |
| `ConsumeEquiproomItem(n,g,d,p[,lv])` | trừ từ hành trang (+ túi mở rộng), trả số đã trừ |
| **`ConsumeItem` nhận thêm DẠNG JX2** `(nPos, nCount, g, d, p[, lv])` | phân biệt bằng tham số 2 ≠ 0 (mọi caller JX1 trong cây truyền nature = 0). 🔴 **Trước đây 3 nơi JX2 là NO-OP** (không trừ đồ!): `songjin_shophead.lua:139` (shop Tống Kim — nhận thưởng không mất rương), `xinshibaoxiang.lua:146/151`, `tong_springfestival\head.lua:251` |
| `GetItemParam(idx,1)` | = ô tham số 1 (nghĩa Linux) thay vì số lượng chồng — quét cây: không script nào dùng nghĩa cũ; yêu bài Tín Sứ đếm số lần dùng bằng ô này |
| **Quy ước JX2 "main() trả ≠ 1 ⇒ engine tự trừ 1 vật phẩm"** (`KItemList.cpp` + `KPlayer::ExecuteItemScriptJX2`) | chỉ cho danh sách đường dẫn JX2: `item\messenger\`, `item\xinshirenwu\`, `item\bosscharm.lua`, `honnguyenchandon.lua`. 🔴 Phát hiện: engine JX1 bỏ qua giá trị trả ⇒ **`bosscharm.lua` (Lệnh bài boss bang hội, `return 0`) trước giờ KHÔNG bao giờ mất lệnh bài** = gọi boss vô hạn; che*fu.lua/honnguyenchandon cũng vậy. Kiểm `SearchID == nIdx` trước khi trừ để script tự `ConsumeItem` không bị trừ đôi |

### 7.4 Đối chiếu đã xác minh
- NPC template 844 Bảo rương / 849 Thiên Bảo Hộ thủ 90 / 842 Thiên Bảo tuần thủ / 377 Dịch quan: trùng id + tên 2 cây.
- 18 skill id (509, 542-546, 548, 631-635, 963, 1038-1042) trùng id + tên (`ReverseTools\npc_skill_remap.py`).
- `messenger_giveprize` (đọc `tollgate_allprize.txt`) **không ai gọi** cả trên Linux → chép nhưng không remap 80 dòng vàng trong bảng.
- Menu Dịch Quan Linux: `messenger_duihuanprize` (đổi điểm → yêu bài / Hoàng Kim) và `messenger_getlevel` **đã bị Linux chú thích** ⇒ yêu bài chỉ có được nếu admin phát; giữ đúng hiện trạng theo lệnh.
- Trap map 395 (`trap_qianbaoku.lua`): Linux map-data trỏ `trap-qianbaoku.lua` (gạch ngang, tệp không tồn tại) ⇒ trên Linux trap cũng chết; dự án maps.pak không có bảng trap ⇒ giống nhau: không có bẫy đẩy lui ở cửa ải.

### 7.5 Chưa làm / chờ
- **Chỉ Nam Nhiệm Vụ (F11)** cho Tín Sứ: phải viết mã CLIENT (`UiTaskGuide.cpp` + `uitasklist.ini`) — chưa.
- Restart GameServer với `CoreServer.dll.moi_tinsu` (đã gồm mọi thứ của `.moi_hoatdongphuong` + log S4 của phiên kia).
- Kiểm sau restart: `ScriptError.log` không thêm dòng `tollgate|messenger|xinshirenwu`; Dịch Quan Thành Đô/Đại Lý menu "Nhiệm vụ Tín Sứ" → "Ta bằng lòng!" chọn tuyến; Xa Phu mục "Đi nơi đặc biệt…"; map 395 có 9 rương + 9 thủ hộ + Tiêu Trấn (1386,2442) + Dịch quan (1412,3203).



## 8. ĐÃ THI CÔNG — LÔI ĐÀI BANG HỘI bản CN gốc (21/08 đêm, chờ restart)

Lựa chọn chủ game: **"Dịch ngược relay, dựng lại bản CN gốc"** (bản Linux VN chết ở tầng engine). Nguồn: workflow 11 tác tử
(`DACTA_LOIDAI_BANGHOI_CN_WORKFLOW.md` — đặc tả + 2 vòng phản biện), mã sinh bởi `ReverseTools\arena_block.cpp` + `arena_patch.py` + `arena_wire.py`.

### 8.1 Engine (`KJx2CityWar.cpp/.h`, `KTongJX2.cpp`, `ScriptFuns.cpp`) — gộp relay + GS vào một chỗ
- **6 trạng thái thành** như gốc (`JX2CW_STATE_*`): 0 rỗi · 1 báo danh · 2 có bảng đấu · 3 lôi đài · 4 có khiêu chiến giả · 5 công thành. Mirror `jx2citywar.txt` thêm `V 2`; mirror cũ (1/2) tự đổi 4/5. `HaveBeginWar` = state 5.
- **4 pha = 4 API Lua trùng tên relay**: `StartSignUp(c)` (xoá dữ liệu thành, state 1, tin SIGNUP; cưỡng chế tắt lôi đài treo tuần trước), `EndSignUp(c)` (0 bang → 0; 1 bang → challenger, state 4; ≥2 → cắt ≤16 theo phí + hoàn tiền + tin SIGNUP_OUT, bốc thăm `BuildBracket` đúng thuật toán khe chẵn/lẻ, state 2), `StartArena(c)` (đòi state 2, bracket ≠ ∅, chưa có kết quả, không lôi đài khác; TongState bang = 2; state 3), `StartCityWar(c)` (đòi challenger; vô chủ → chiếm luôn WAR_RESULT3; có chủ → TongState 4 cả hai, state 5).
- **`SignUpCityWarArena(c, fee)`**: 10 điều kiện gốc (bỏ "khảo nghiệm" — bang ta không có trường), so sánh **có dấu**, trừ quỹ bang qua `KTongJX2_AddMoneyC` (relay bang), tin SIGNUP_OK/SIGNUP_TOALL.
- **`BuildArenaPairs`** chép đúng G `0x0805A480` (ArenaID đánh số lại mỗi vòng, `nLevel` ghi trong nhánh có cặp — theo phản biện); **`AddArenaResult`** kiểm trùng 2 chiều (A,B)/(B,A), từ chối hẳn khi đã có kết quả; hết cây → vô địch = bên thắng bản ghi cuối → `szChallenger`, state 4, TongState 3, tin ARENA_RESULT2.
- **14 hàm Lua GS** thay stub: `IsArenaBegin/GetArenaBothSides/GetArenaCityArea/GetArenaTargetCity/GetArenaLevel/GetArenaTotalLevel[ByCity]/GetArenaSchedule/GetArenaInfoByCity/NotifyArenaResult` + `IsSigningUp` (= state 1), `NumOfSignUpTongs/GetSignUpTongName` (đọc vector báo danh, state 1..3 — không còn League 508), `AppointChallenger/AppointViceroy` xoá dữ liệu lôi đài thành (R `0x080988C4`).
- **Persist**: dòng `G/A/F/B/R` trong `jx2citywar.txt`; nạp = bracket → replay kết quả (`bKeep=1`) → toàn cục (đúng thứ tự GS gốc). `KTongJX2_SetFieldC/GetMoneyC/AddMoneyC` mới.
- Chuỗi thông báo = `lang\vn\stringtable_relay.txt` của relay, byte TCVN3 nguyên văn (31 chuỗi).

### 8.2 Script (cây chạy thật; mirror `serverscript_jx2\loidai_cn\jx1_edits\`)
| Tệp | Sửa |
|---|---|
| `timerserver.lua` | **CUTOVER Đợt E** (trước giờ `CTC_JX2_Tick` CHỈ được gọi từ bộ test GM!): Include `timerserver_ctc.lua` + gọi `CTC_JX2_Tick` mỗi tick (guard `~= nil`). `dofile` mỗi tick ⇒ có hiệu lực ngay, các hàm CN nil-guard tới khi restart. |
| `timerserver_ctc.lua` | 18h `StartSignUp(i)` (+ giữ League 508 task để NPC mở menu) · 19h `EndSignUp(i)` (đường lệnh bài/`GetRandomChallenger` VN không còn chạy) · 20h ngày báo danh `StartArena(i)` · 20h ngày đánh `StartCityWar(i)` · 0h chỉ dọn state 5 (**sửa lỗi cũ: 0h xoá luôn khiêu chiến giả trước giờ đánh**). |
| `infocenter_head.lua` `ArenaMain` | 18-19h thêm **"Báo danh đấu thầu Lôi đài bang hội/SignUpTheOne"** (AskClientForNumber 1.000.000–99.999.999 → `SignUpFinal` → `SignUpCityWarArena`); ngoài giờ thêm **"Tham gia Lôi đài bang hội/PreEnterGame"** (IsArenaBegin → EnterBattle map 213+). NPC = "Sứ Giả Công Thành" Ba Lăng Huyện (1625,3170). |
| `citywar_arena\camper.lua:81/87` | `GetJoinTongTime() >= 7200` → **`GetLevel() >= 90`** (chính sách chủ game). |
| `startgame\citywar_boot.lua` | `CityWar_ArenaNpc()`: spawn NPC 2 phe trên 8 map 213..220 (tpl 178 ô 1581,3257 `camper1.lua`; tpl 124 ô 1603,3236 `camper2.lua` — toạ độ đọc từ region maps.pak; engine bỏ NPC map-data). |
- Lịch thành theo ngày: `TB_CTC6` (Phượng Tường T4/T5 … như relay `TB_CITYWAR_ARRANGE`); mission 9 = `citywar_arena`, timer 18 poll 5', map 213–220 có sẵn.
- **Lôi đài tự chế của dự án (`tinhnang\loidai`, `sukien_loidaibanghoi`, `MS_LOIDAIBH`)**: đã bị Đợt E ngắt (`timerserver.lua` comment, NPC `congthanhquan.lua` bỏ) — chỉ còn `Include lib_loidai.lua` rải ở 8 tệp, giữ nguyên để không gãy Include.

### 8.3 Lệch có chủ đích / chưa làm
- Không có điều kiện "bang đang khảo nghiệm"; `GetCityWarTongCamp` giữ bản dự án (1 thủ/2 công); không thưởng/phạt tiền cược lôi đài (gốc cũng không có mã).
- `AddLocalNews` là tin nội bộ 1 GS (gốc relay phát mọi GS) — ta 1 GS.
- Chưa test chạy thật. Test nhanh sau restart (GM): `CTC_JX2_Tick(TB_CTC6[n][1],18,0)` → 2 bang chủ báo danh ở Sứ Giả → `CTC_JX2_Tick(...,19,0)` → `GetArenaSchedule(n)` có cặp → `CTC_JX2_Tick(...,20,0)` → ≤5' map 213 mở mission 9, NPC Cảnh Tử Kỳ/Độc Tiên Tử cho vào phe.


| 21/08 | Chốt phạm vi + chính sách cấp 90 / bỏ trùng sinh | ✅ |
| 21/08 | Đối chiếu item Tín Sứ (14/14 có sẵn, 0 phải làm thêm) | ✅ |
| 21/08 | Điều tra NPC placement / Chỉ Nam Nhiệm Vụ / boss bang hội / thuế thành | ⏳ đang chạy |
| 21/08 | Lôi Đài Bang Hội CN (mục 8) — engine + script, cutover timerserver | ✅ chờ restart |
| 21/08 | Gỡ `tinhnang/thienbaokho`, chép cây `messenger` (mục 7) | ✅ chờ restart |
