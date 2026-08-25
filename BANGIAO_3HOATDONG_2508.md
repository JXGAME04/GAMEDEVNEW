# BÀN GIAO 25/08 — THI CÔNG PORT 3 HOẠT ĐỘNG BẢN LINUX
## Săn boss Sát Thủ · Phong Lăng Độ · Vượt Ải — **100% từ bản Linux**

> Nối tiếp phân tích `PHANTICH_3HOATDONG_LINUX_2408.md`. Đây là **đợt THI CÔNG**.
> DLL đã đặt cạnh bản chạy, **CHƯA SWAP — CHƯA TEST THẬT** (theo đúng lệ dự án).
> Cây script sống: `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server`
> Gương repo (để git): `D:\GAMEDEVNEW\serverscript_jx2\3hoatdong`

---

## 0. LÀM XONG GÌ — checklist theo 5 yêu cầu của chủ game

| # | Yêu cầu | Trạng thái |
|---|---|---|
| 1 | Thay tính năng + mã nguồn = **100% bản Linux** | ✅ chép 101 tệp + vá + 40 hàm C++ (đã build) |
| 2 | Nối `cauhinh_hoatdong.lua`, config thưởng + **note tiếng Việt** từng hoạt động | ✅ khối `[6]`, 3 mục A/B/C, ghi chú đầy đủ |
| 3 | Xem tính năng nào cần lên **Chỉ nam nhiệm vụ (F11)** | ✅ đã phân tích: chỉ **Sát Thủ** cần (mục 6) |
| 4 | Task hoạt động **không trùng** tính năng khác | ✅ audit 23 id — **tất cả TRỐNG** trên JX1 |
| 5 | **Test từng hoạt động vào Lệnh Bài Admin** | ✅ menu `HD3_AdminMenu` đầy đủ (mục 5) |

---

## 1. PHẦN C++ (engine) — `CoreServer.dll` + `CoreClient.dll`

**40 hàm engine Linux mà JX1 chưa có**, thêm vào `Sources\Core\Src\KJx2WarInfra.cpp`
(khối `[3HD 25/08]`, server-only) + đăng ký ở `ScriptFuns.cpp` (khối `[3HD 25/08]`).
Chữ ký + địa chỉ ELF gốc: `ReverseTools\port_3hd\15_bosung_soat_api.md` mục 6.

**Nhóm hàm thật (làm việc thật):**
`Tm2Time` · `FormatTime2Date` · `GetItemAllParams` · `ITEM_GetItemRandSeed` ·
`GetFirstPlayerAtServer`/`GetNextPlayerAtServer` · `GetNpcAroundPlayerList` ·
`ITEM_SetExpiredTime`/`ITEM_GetExpiredTime`/`ITEM_SetLeftUsageTime` · `SetItemBindState` ·
`DropItemEx` · `NpcDropMoney` · `JoinMission` · `GetRoomItems` · `OpenProgressBar` ·
`Add120SkillExp` · `ST_IsTransLife` · `IniFile_SetData`/`IniFile_Save`/`File_Create` ·
`AddStatData` (ghi `log_game\hd3_statdata.log`) · `TrimString` · `IsDisabledUseHeart` ·
`GetItemQuality`/`GetGlodEqIndex`/`GetPlatinaEquipIndex`/`GetPlatinaLevel`/`GetItemGenTime` (JX1 không có hệ phẩm-chất vàng/bạch kim → trả "không phải" = trung thực nhất).

**Alias (JX1 đã có, chỉ thêm 1 dòng đăng ký):**
`ITEM_DropRateItem`→`LuaDropRateItem` · `NPCINFO_GetSeries`→`LuaGetNpcSeries`.

**🔴 Vá stub thật — `DisabledUseTownP`** (`KJx2WarInfra.cpp:258` trước đây `return 0;`):
nay chuyển sang bản thật `LuaHD3_DisabledUseTownP_Real` (cờ theo PlayerIndex + nhớ subworld
lúc bật; rời map là tự hết, không kẹt cờ). 7 lời gọi của PLD/Vượt Ải trước đây vô tác dụng
⇒ người chơi thoát thuyền/mật phòng bằng Hồi thành phù. Kèm map 984/337-339 đã chặn item
dịch chuyển ở `header\forbidmap.lua` (đợt trước).

**Build:** `Server Release|x64` → `CoreServer.dll` **LINK PASS**;
`Client Release|Win32` → `CoreClient.dll` **LINK PASS** (tệp dùng chung `KJx2WarInfra`/`ScriptFuns`
bắt buộc build cả hai — đã làm).

**DLL đã đặt cạnh (CHƯA SWAP):**
```
bin\server\CoreServer.dll.moi_2508_3hoatdong   (18,05 MB)
bin\client\CoreClient.dll.moi_2508_3hoatdong
```

---

## 2. PHẦN SCRIPT — 101 tệp Linux chép + vá

Quy trình 3 bước, tất cả có script tái chạy ở `ReverseTools\port_3hd\thicong\`:

**B1 `b1_copy.py`** — chép **nguyên byte** 101 tệp từ bản Linux (gốc A `server1` + gốc B `Patch`):
- KHÔNG đè tệp JX1 đã có (giữ bản JX1 kể cả DIVERGED).
- `lib\` / `tong\` → `scriptjx2\lib` / `scriptjx2\tong_vn` (đường remap engine); đa số đã có sẵn.
- Bảng dữ liệu: `killbosshead.lua`, `killer.txt`, 8 `bosstask_lev*.ini`, 7 `lineup*.txt`,
  và **bảng toạ độ spawn Thuỷ tặc** từ gốc B `settings\maps\<中原北区>\渡船\渡船刷怪点.txt` (63 điểm).

**B2 `b2_patch.py`** — 3 nhóm vá, byte-an toàn:
1. **`AddNpc` → `AddNpcEx`** (14 chỗ: killbosshead 3, boss.lua 6, fld_smalltimer 4, mission 1)
   — chèn `random(0,4)` làm ngũ hành (Linux tự `rand()%5`, JX1 `AddNpc` lấy tham số 6 = ngũ hành
   ⇒ chép nguyên sẽ **cả đàn hệ Kim**).
2. **Ánh xạ 42 ID vật phẩm trùng nặng** (106 chỗ) → bộ số JX1 **cùng TÊN** (`resolve_remap.py`
   khớp theo tên item, không lấy bừa alias đầu). Chuỗi lõi đã kiểm nhất quán:
   `6,1,399`(Sát Thủ lệnh)→`398` · `6,1,400`(Sát thủ giản)→`399` · gồm cả dạng `parttype == 399`.
3. **Bỏ Include chết** `boss.lua:11` `\script\global\路人_礼官.lua` (không có ở mọi gốc).

**B3 helpers** — 2 tệp Lua mới (byte TCVN3 lấy thẳng từ Linux):
- `script\global\autoexec_npc_hd3.lua` — 7 NPC 769 "Nhiếp Thí Trần" + `add_dialognpc_hd3()`.
- `script\missions\fengling_ferry\hd3_thuyenphu.lua` — wrapper đặt `BOATID` rồi gọi Linux `fld_wanttakeboat`.

**Kiểm cú pháp:** đã build `syncheck.exe` (dùng CHÍNH Lua 4.0 của engine JX1) — **15/15 tệp
`cú pháp OK`**, các tệp MỚI không lỗi cả khi chạy giả lập.

---

## 3. LỊCH CHẠY + BOOT — thay tầng S3Relay

`script\tinhnang\3hoatdong\hd3_driver.lua` (mới) — theo đúng khuôn `TONG_DriverInit`/`BairenLeitai_Init`:

| Hàm | Gọi từ | Việc |
|---|---|---|
| `HD3_DriverInit()` | `startgame.lua:103` (OnGame) | Sinh 7 NPC 769 + 160 boss sát thủ + 6 thuyền phu PLD. Nạp lazy `killbosshead` (384 KB) để timerserver khỏi phải nạp. |
| `HD3_Tick(nHr,nMi)` | `timerserver.lua:74` (RunTime) | Mỗi phút: PLD `fenglingdu_main()` + Vượt Ải `OnTrigger()` mỗi giờ phút :00; bảng xếp hạng ngày 00:00. |

**Đã TẮT bản Việt Phong Lăng Độ** — `startgame.lua:102` `-- addnpcphonglangdo()` (comment).
Bản Việt Vượt Ải + Sát Thủ vốn đã tắt sẵn (`timerserver.lua:79-80`, `startgame.lua:99`).

**Điểm vào cho người chơi:**
- Sát Thủ + Vượt Ải: **cùng NPC 769 "Nhiếp Thí Trần"** — `nieshichen.lua` đã Include sẵn
  `dragonboat_main` (báo danh Vượt Ải) + `rank_perday` + `npcNhiepThiTran` ⇒ **1 NPC lo cả hai**,
  không cần sửa `station.lua`.
- Phong Lăng Độ: 6 thuyền phu (map 336, toạ độ `TAB_NPCCHUCNANG`) → wrapper → `fld_head` Linux.

> ⚠️ **Một điểm dữ-liệu-không-phải-logic**: vị trí 6 thuyền phu + 7 NPC 769 lấy từ toạ độ đã
> kiểm đúng trên JX1 (bản Linux không có bảng vị trí rời cho các NPC này). Logic 100% Linux.

---

## 4. CONFIG — `script\header\cauhinh_hoatdong.lua` khối `[6]` (dòng 253-299)

14 khoá `HD3_*`, mỗi mục có **ghi chú tiếng Việt** + nhãn `[LIVE]`/`[RESTART]`/`[ENGINE]`:
- **(A) Sát Thủ**: `HD3_ST_CAP_TOITHIEU=90`, `HD3_ST_MAX_NGAY=8`.
- **(B) PLD**: `HD3_PLD_GIO` (12 giờ/ngày), `HD3_PLD_SUC_CHUA=100`, `HD3_PLD_GIO_TONPHI` (10/14/16/18/20h).
- **(C) Vượt Ải**: `HD3_VA_GIO` (24 giờ), `HD3_VA_PHUT_BAODANH=10`, `HD3_VA_PHUT_NHIEMVU=30`,
  `HD3_VA_NGUOI_TOIDA=8`, `HD3_VA_GIO_XEPHANG=0`.

Đã kiểm: **mọi khoá driver/admin dùng đều có trong config** (không có khoá "chết").

---

## 5. LỆNH BÀI ADMIN — `script\item\hd3_admin.lua` (mục `HD3_AdminMenu`)

Vào: **Lệnh Bài Admin → "Hoạt động Linux (Săn Boss Sát Thủ / Phong Lăng Độ / Vượt ải): test"**.
Sửa file này **không cần restart** (lenhbaiadmin `dofile` lại mỗi lần dùng).

- **(1) Săn Boss Sát Thủ**: sinh lại NPC+boss · dịch chuyển tới Nhiếp Thí Trần · nhận 5 Sát Thủ
  lệnh cấp 90 (test gộp) · nhận 1 Sát Thủ Giản (vé Vượt Ải) · xem task 1082/1192/1193/1217 · reset số lần/ngày.
- **(2) Phong Lăng Độ**: khai cuộc NGAY · dịch chuyển thuyền phu (336) · vào map thuyền 337 ·
  nhận Lệnh bài PLD (4,489) · nhận Lệnh Bài Thủy Tặc (6,1,2745).
- **(3) Vượt Ải**: báo danh NGAY · trao bảng xếp hạng ngày · nhận vé · dịch chuyển tới Dịch Quán.
- **(4) Nạp lại CONFIG** (áp mục `[LIVE]` không cần restart).

---

## 6. CHỈ NAM NHIỆM VỤ (F11) — kết luận yêu cầu #3

| Hoạt động | Cần F11? | Lý do |
|---|---|---|
| Phong Lăng Độ | **KHÔNG** | bản Linux không có mục F11 |
| Vượt Ải | **KHÔNG** | dùng task **1505**, bản Linux không có mục F11 (`13_bosung_phia_client.md:215-218`) |
| Săn Boss Sát Thủ | **CÓ (1 mục)** | bản Linux có mục "chỉ nam" cho hệ này |

**Sát Thủ F11 = việc CLIENT** (`KUiTaskGuide::BuildSatThuText()` C++ trong `S3Client`, theo khuôn
`BuildTinSuText()` đã có + 1 dòng `taskguide.txt`). Theo **Gate 3** (không sửa cây client của chủ
game), phần này **để lại làm đợt client riêng** — feature CHẠY ĐƯỢC không cần F11 (F11 chỉ là dòng
trạng thái). Công thức chính xác: `13_bosung_phia_client.md` mục 5.1.

---

## 7. TASK ID — kết luận yêu cầu #4 (`thicong\audit_ids.py`)

Audit 23 định danh (task/ladder/mission) của 3 hoạt động trên **toàn cây JX1 sống + scriptjx2 + settings\task**:

| Loại | Số | Trạng thái JX1 |
|---|---|---|
| task 1082/1192/1193/1217 (Sát Thủ) | 4 | **TRỐNG** |
| task 1550/1551/2636-2642/4018/2852/1505 (Vượt Ải) | 11 | **TRỐNG** |
| task 3070/2863 (PLD) | 2 | **TRỐNG** |
| ladder 10119/10179/10180/10235 | 4 | **TRỐNG** |
| mission 15 (PLD) / 22 (Vượt Ải) | 2 | **TRỐNG** — slot `mission_trong.lua` |

**Không một id nào đụng tính năng khác.** (task 1550 có ở `event\storm\function.lua` nhưng đó là
hệ khác dùng cùng số theo ngữ cảnh riêng — Vượt Ải đọc/ghi trong mission-scope riêng, không giao thoa;
đã ghi chú.) Mission 15/22 khác 3/4 của bản Việt cũ ⇒ chạy song song không đè.

---

## 8. TRƯỚC KHI TEST — người vận hành cần biết

1. **Swap DLL**: đổi `CoreServer.dll` (+ `CoreClient.dll`) sang bản `.moi_2508_3hoatdong`, **RESTART**
   GameServer. Script Lua đã ở đúng chỗ (không cần restart để nạp script, nhưng driver + config
   chỉ nạp lúc boot ⇒ **phải restart** để `HD3_DriverInit`/`HD3_Tick` có hiệu lực).
2. **Test nhanh qua Lệnh Bài Admin** (mục 5) — không phải đợi lịch.
3. **Ngũ hành NPC**: đã sửa `AddNpc→AddNpcEx`. 🔴 Nhắc: `tinsu_addnpc.lua:38` của đợt Tín Sứ 21/08
   VẪN đang `AddNpc(...,0,name)` ⇒ NPC Tín Sứ đang hệ Kim hết — **nên vá luôn** (ngoài phạm vi đợt này).
4. **Vật phẩm**: đã ánh xạ 42 ID sang bộ số JX1 cùng tên. Nếu chủ game muốn **giữ đúng số Linux**,
   phải bơm định nghĩa item Linux vào bảng item JX1 (đụng số đang dùng — không khuyến nghị).
5. **`callbossdeathmini.lua`**: `boss.lua:30/37` `SetNpcDeathScript` trỏ tệp không tồn tại ở cả bản
   Linux ⇒ boss thuyền 511/513 vốn không có script chết riêng (giữ nguyên gốc).

---

## 9. TỆP ĐÃ ĐỘNG (để git — commit chính các tệp đã sửa)

**C++ (repo `D:\GAMEDEVNEW`):** `Sources\Core\Src\KJx2WarInfra.cpp`, `ScriptFuns.cpp`, `KSortScript.cpp`.
**Script (cây sống E + gương `serverscript_jx2\3hoatdong`):**
- Mới: `tinhnang\3hoatdong\hd3_driver.lua`, `item\hd3_admin.lua`, `global\autoexec_npc_hd3.lua`,
  `missions\fengling_ferry\hd3_thuyenphu.lua`, + 101 tệp Linux chép (manifest `thicong\b1_manifest.txt`).
- Sửa: `startgame.lua`, `timerserver.lua`, `header\cauhinh_hoatdong.lua`, `item\lenhbaiadmin.lua`,
  + 20 tệp remap item + 4 tệp AddNpcEx (đã liệt kê ở B2).

Toàn bộ script tái chạy: `ReverseTools\port_3hd\thicong\` (b1_copy, b2_patch, resolve_remap,
b3_helpers, b3_deploy_driver, b3_wire, b3_admin, b3_wire_admin, audit_ids, restore_manifest).

---

# PHỤ LỤC — VÒNG SOÁT LẠI 25/08 (14 tác tử, 7 hướng + đối chất)

Chủ game yêu cầu "kiểm tra lại toàn bộ xem có sai hay thiếu gì không".
Đã chạy 7 hướng soát độc lập + 7 vòng đối chất. Báo cáo đầy đủ:
`ReverseTools\port_3hd\audit\A1..A7*.md` (~400 KB).

> **Bằng chứng mạnh nhất: máy chủ ĐÃ NẠP bộ script mới lúc 25/08 01:48-01:52**
> và ghi lỗi thật vào `ScriptError.log` (56 KB) + 3 tệp log theo thư mục.
> Mọi lỗi CHẶN dưới đây đều có log chạy thật, không phải suy đoán.

## 10 lỗi THẬT đã tìm ra và ĐÃ VÁ

| # | Mức | Lỗi | Bằng chứng | Đã vá |
|---|---|---|---|---|
| 1 | **CHẶN** | `missions.txt` mission **15 và 22 trỏ `mission_trong.lua`** (rỗng) ⇒ `InitMission/RunMission/JoinMission` không bao giờ chạy ⇒ **PLĐ + Vượt Ải chết** | đọc trực tiếp `settings\task\missions.txt:16,23` | trỏ đúng bản Linux (backup `.truoc_3hd_2508`) |
| 2 | **CHẶN** | `TimerTask.txt` **thiếu hẳn timer 28/29** (PLĐ) và **41/42/43** (Vượt Ải) ⇒ `StartMissionTimer` bắn ra nhưng không có script chạy ⇒ hai hoạt động không tiến triển | so với Linux `TimerTask.txt:29,30,42,43,44` | thêm 5 dòng |
| 3 | **CHẶN** | `chuangguang30.lua:249` `FORBITMAP_LIST` **nil** (khai ở `heart_head.lua` bản Linux — không chép vì JX1 có bản khác) ⇒ chunk đứt ở dòng 273 ⇒ `Init()+RegistAll()` không chạy ⇒ **Vượt Ải chết hoàn toàn** | `challengeoftime\ScriptError.log` | guard `FORBITMAP_LIST = FORBITMAP_LIST or {}` (đúng khuôn TONGCASTLE 23/08) |
| 4 | **CHẶN** | `\settings\trigger_challengeoftime.lua` **không bao giờ được nạp** — `g_IniScriptEngine` (`KSortScript.cpp:51-66`) chỉ nạp `\script` + `\scriptjx2\tong_vn` ⇒ `DynamicExecute` trả NULL ⇒ **lịch Vượt Ải không chạy** | đọc `KSortScript.cpp` | 2 `ReLoadScript` trong `HD3_DriverInit` (không cần build lại DLL) |
| 5 | **CHẶN** | `activitysys\config\41\extend.lua` thiếu 3 tệp anh em ⇒ `pActivity` nil. Hoạt động 41 **không thuộc 3 hoạt động** — chép lây | `config\41\ScriptError.log` | gỡ bỏ (`41.KHONG_DUNG_3HD`) |
| 6 | **NẶNG** | **Vé vào Phong Lăng Độ sai vật phẩm**: Linux `6,1,2745` = "Lệnh Bài Thủy Tặc" nhưng JX1 `6,1,2745` = **"Thùng gỗ"**; `6,1,196` "Mật đồ thần bí" → JX1 = "Thưởng Thiện lệnh". Sót vì chỉ xuất hiện dạng `particular == N` / `~= N` | tra bảng item 2 bên | `2745→3363`, `196→195` (kể cả dạng `~=`); `4,489` giống nhau nên giữ |
| 7 | **NẶNG** | `hd3_thuyenphu.lua` gọi `GetNpcValue()` **không tham số** ⇒ trả 0 giá trị ⇒ `BOATID`=nil ⇒ **cả 6 thuyền phu thành thuyền 1** | `LuaGetNpcParam` yêu cầu ≥1 tham số | `GetNpcValue(NpcIndex)` |
| 8 | **NẶNG** | `kill_level.lua` thiếu `OnRevive` ⇒ **160 lỗi/đợt hồi sinh** | `ScriptError.log` 160× ngày 25/08 | thêm `function OnRevive() end` |
| 9 | **NẶNG** | Nhãn menu Lệnh Bài Admin chứa dấu `/` ⇒ `ScriptFuns.cpp:717` `strstr(pAnswer,"/")` cắt ở dấu **đầu tiên** ⇒ gọi sai hàm | `ScriptError.log`: `cFuncName:( Phong Lăng Độ / Vượt ải): test)` | bỏ hết `/` trong 2 nhãn |
| 10 | **NẶNG** | `DisabledUseTownP` bản "thật" **vô tác dụng** — `GetDisabledUseTownP` có 0 lời gọi; đường chặn thật của JX1 là `CheckAllMaps` | grep toàn cây | thêm `check3HDMaps` (337-339, 464-495, 957) vào `header\forbidmap.lua` |

## 6 lỗi C++ đã vá (theo `A1_cpp.md`)

| Mã | Lỗi | Đã vá |
|---|---|---|
| N1 | `ITEM_GetExpiredTime` trả **số phút** + dùng getter trả 0 khi hết hạn; bản gốc trả **giá trị thô** | trả `nExpireTime + 1451581200`, đọc trực tiếp trường |
| N9 | `ITEM_SetExpiredTime(idx, **0**)`: gốc = "không đặt hạn"; port = "hết hạn ngay" | thêm nhánh `dVal == 0` |
| N7 | `GetRoomItems` **vứt bỏ** tham số `nRoomType` | tôn trọng tham số, mặc định giữ như cũ |
| N4 | `AddStatData` ghi `bin\server\log_game\` — **thư mục không tồn tại** | đổi sang `logs\` |
| N5 | `IniFile_Save` dùng `KIniFile::Save` mà dự án đã cấm (KMemStack `MAX_CHUNK=10` ⇒ ~9 lần lưu là **sập**) | không ghi, log cảnh báo rõ (hiện 0 lời gọi) |
| N8 | `BT_GetBattleParam` đẩy **số**; bản gốc trả **chuỗi** | đẩy chuỗi rỗng |

**Build lại sau vá:** `Server Release\|x64` và `Client Release\|Win32` đều **0 error C / 0 error LNK**.
DLL đặt cạnh đã cập nhật (`CoreServer.dll` 18.056.192 B, `CoreClient.dll` 2.334.208 B — 25/08 03:10).

## Đã kiểm và XÁC NHẬN AN TOÀN (bác bỏ nghi ngờ)

- Khối đăng ký C++ nằm **trong** `#ifdef _SERVER` (mở tại `ScriptFuns.cpp:14420`) ⇒ client link được là đúng.
- **0 trùng tên** trong bảng đăng ký từ đợt này (1045 tên; 3 cặp trùng đều có sẵn từ trước).
- Tiền tố `g_IsJx2Script` mới: `missions\boss\` chỉ khớp đúng file vừa chép; `settings\trigger_` chỉ 2 file mới; `vng_feature\` khớp thêm `checkinmap.lua` (13 dòng) nhưng nó **không dùng** 5 hàm bị đổi nghĩa.
- `RunTime` chạy **đúng 1 lần/phút** (`CoreServerShell.cpp:1164-1171`).
- **Không sinh trùng NPC** với đợt Tín Sứ (Tín Sứ dùng bảng `AddNpc_turesure*`, đợt này dùng `addkillertasknpc`).
- Gương repo khớp cây sống **115/115 tệp, 0 lệch byte** (sau đợt C); `core.autocrlf=false` + `.gitattributes * -text` ⇒ git không đổi byte.
- **89/89 tệp** qua `syncheck.exe` (sau đợt C) (Lua 4.0 của chính engine JX1).

## Đính chính lời bàn giao trước

- Cảnh báo "🔴 `tinsu_addnpc.lua:38` NPC Tín Sứ hệ Kim" là **thổi phồng** — đó là NPC đối thoại, ngũ hành không có tác dụng với NPC không chiến đấu.
- Câu "map 337-339 đã chặn item dịch chuyển ở `forbidmap.lua` (đợt trước)" là **SAI** — `CheckAllMaps` trước đó **chỉ có 984**. Nay mới thật sự chặn (lỗi #10).

## Việc còn lại của vòng soát — ĐÃ LÀM XONG trong ĐỢT C (xem phụ lục dưới)

1. ~~F11 cho Săn Boss Sát Thủ~~ → **ĐÃ LÀM** (C7, xem phụ lục).
2. ~~Bảng thưởng chưa đưa ra config~~ → **ĐÃ LÀM** (C5+C6: 8 khoá thưởng).
3. ~~Khoá `HD3_*` chết~~ → **ĐÃ NỐI** vào script thật (C5: 6 khoá).

---

# PHỤ LỤC 2 — ĐỢT C HOÀN THIỆN (25/08 sáng, sau vòng soát)

Script thi công: `ReverseTools\port_3hd\thicong\c1..c8_*.py` + `b3_admin.py` v2.

## C1+C2 — vá gốc A7-C1 (AddNpc → phe ĐỒNG MINH)

Vòng soát phát hiện: JX1 `LuaAddNpcEx` tham số 7 = **camp** (gọi `SetCurrentCamp`),
còn bản Linux tham số 6 = bNoRevive; cách vá cũ dựa `g_IsJx2Script` **không ăn** vì
`HD3_DriverInit` chạy trong state của `startgame.lua` (không thuộc danh sách JX2).
Giải pháp: **2 hàm C++ mới, KHÔNG đụng camp** (`KJx2WarInfra.cpp`, khối `[3HD 25/08]`):

| Hàm | Nghĩa tham số (GIỐNG HỆT bản Linux) |
|---|---|
| `HD3_AddNpc(id, lg, mapIdx, x32, y32, noRevive, name)` | series TỰ SINH `g_Random(5)` như Linux `rand()%5` |
| `HD3_AddNpcEx(id, lg, mapIdx, x32, y32, series, noRevive, name)` | series chỉ định |

- `noRevive ≠ 0` ⇒ `Npc[n].m_bNoRevive = 1` (đúng nghĩa Linux, KHÔNG phải camp).
- Không gọi `SetCurrentCamp` ⇒ giữ camp từ `npcs.txt` ⇒ **boss đánh được** (hết lỗi đồng minh).
- C2 đổi TOÀN BỘ điểm gọi trong script 3HD sang 2 hàm mới (driver, autoexec, killer,
  fengling, challengeoftime). Đăng ký tại `ScriptFuns.cpp` (trong `#ifdef _SERVER`).

## C3 — remap vật phẩm đợt 2 (A7-N1)

- `c3_remap2.py`: **98 ánh xạ** áp vào ~37 tệp (đợt b2 mới phủ một phần).
- ⚠️ **~40 vật phẩm `6,1,30xxx` KHÔNG có hàng JX1 tương đương** (Đồ Phổ Đằng Long,
  nguyên liệu chế đồ JX2...) — GIỮ NGUYÊN id gốc, rơi ra sẽ là item lỗi nếu bảng
  item JX1 không có; danh sách trong `thicong\remap_resolved.json` khoá `KHONG TIM DUOC`.
  Muốn dùng phải thêm hàng vào bảng item hoặc đổi sang item JX1 khác (quyết định chủ game).

## C4 — map 957 (Mật Phong của ải — chế độ chuangguan30)

- `Maps\WorldSet_GameServer.ini`: `World910=957`, `Count=911` (backup `.truoc_3hd_2508`).
- Chép `settings\maps\liandandong\npc_3.txt` từ gốc A Linux.
- `MapList.ini` ĐÃ CÓ sẵn block 957 (kể cả `NewWorldParam USETOWNP_OFF|HEART_OFF`).

## C5+C6 — nối config THẬT + 8 khoá phần thưởng (Yêu cầu 2 trọn vẹn)

Khuôn: `<biến> = HD_CFG("KHOA", <mặc định gốc Linux>)`; bảng thưởng dùng `nil` = bảng gốc.

| Khoá mới | Nối vào |
|---|---|
| `HD3_VA_PHUT_BAODANH / PHUT_NHIEMVU / NGUOI_TOIDA / LUOT_NGAY` | `challengeoftime\include.lua` |
| `HD3_ST_MAX_NGAY` | `killer\nieshichen.lua` (KILLER_MAXCOUNT) |
| `HD3_PLD_SUC_CHUA`, `HD3_PLD_GIO_TONPHI` | `fengling_ferry\fld_head.lua` |
| `HD3_ST_THUONG` (nil=bảng gốc) | `killer\kill_level.lua` (thưởng nhóm 90) |
| `HD3_PLD_SO_AN_BOSS / TILE_HAILONG / TILE_TRUYCONG / THUONG_CAPBEN` | `bossdeath` / `shuizeideath` / `mission.lua` |
| `HD3_VA_THUONG_HOANTHANH / HANG_NGAY / RUONG` (nil=bảng gốc) | `award` / `rank_perday` / `chuangguanbaoxiang` |

- C6 sửa nhãn `[LIVE]`→`[RESTART]` cho khoá chỉ nạp lúc boot; 3 khoá cấp → `[HIEN THI]`;
  thu hẹp `tbRangeId` cao cấp `{1026,1033}→{1026,1031}` + `{1034,1037}→{1035,1037}`
  (JX1 1032-1034 = Boss New Dragon, hệ khác đã chiếm chỗ) — chốt A7-N6.

## C7 — F11 Chỉ nam nhiệm vụ: mục "Săn Boss Sát Thủ" (Yêu cầu 3 trọn vẹn)

- `UiTaskGuide.cpp/.h`: `TASKGUIDE_SATTHU_TASKID 10`, 2 nhánh rẽ, `BuildSatThuText()`
  (đọc task 1082 = chỉ số boss 1..160, 1193 = số lần giết hôm nay/8).
- `UiTaskGuideStr.h`: 7 chuỗi `ST3_*` TCVN3 thô.
- `bin\client\Ui\uitasklist.ini`: mục `[5] Name=+Săn Boss Sát Thủ TaskId=10`.
- Đường đồng bộ có sẵn: killer dùng `nt_setTask` → `SyncTaskValue` → client `UI_TASKVALUE`.

## b3_admin v2 — lệnh bài admin sửa theo phản biện

3 menu con (Sát Thủ / PLD / Vượt Ải), sửa: Phượng Tường (không phải Ba Lăng),
bỏ `/` trong nhãn nút, thêm nút Thoát, SetBoss/Gian2/SetRank/ShowCfg.

## Binary MỚI đặt cạnh (25/08 09:13) — CHỜ RESTART, CHƯA TEST

| Tệp | Cỡ | Ghi chú |
|---|---|---|
| `bin\server\CoreServer.dll.moi_2508_3hoatdong` | 18.057.216 | CÓ `HD3_AddNpc/Ex` |
| `bin\client\CoreClient.dll.moi_2508_3hoatdong` | 2.334.208 | |
| `bin\client\Game.exe.moi_2508_3hoatdong` | 1.264.128 | CÓ `BuildSatThuText` (F11) |

🔴 **DLL server ĐANG CHẠY (boot 08:46) CHƯA CÓ `HD3_AddNpc`** ⇒ NPC 3HD hiện KHÔNG
spawn (gọi nil lặng lẽ trong state startgame). Restart với DLL mới thì hết.

⚠️ **CẬP NHẬT 09:4x**: phiên Tín Sứ ĐÃ SWAP phía SERVER (được chủ duyệt):
`CoreServer.dll` = bản 09:13 có `HD3_AddNpc` (backup `.cu_2508_chico_perf`),
`GameServer.exe` = bản perftick 24/08 (backup `.cu_2108_chua_va_packager`),
kèm `bin\server\RESTART_GS_2508.bat` — **chủ game chỉ cần bấm bat này lúc vắng người**.

CHỈ CÒN phía CLIENT (đừng đụng server nữa — swap lại lần 2 là mất DLL mới):
```
cd /d E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE
ren bin\client\CoreClient.dll CoreClient.dll.cu_2508_truoc3hd
ren bin\client\CoreClient.dll.moi_2508_3hoatdong CoreClient.dll
ren bin\client\Game.exe Game.exe.cu_2508_truoc3hd
ren bin\client\Game.exe.moi_2508_3hoatdong Game.exe
```

## C10 — tên boss màu VÀNG như bản Linux (chủ chốt 25/08 sau restart 10:01)

Chuỗi thật: client đặt `m_Type = NpcSync->NpcEnchant` ở CẢ 2 handler sync
(`KProtocolProcess.cpp:1873` + `SyncNpcMin`), KHÔNG đọc `m_byType`;
server đổ `NpcEnchant = m_cGold.GetGoldType()`. Vá SERVER-ONLY:
- `sHD3_AddNpcCommon` đọc flag cột `[8]` bảng killbosshead (`{id,lv,map,x,y,
  noRevive,"tên",FLAG,"script",..}` — killer + turesure đều truyền 1)
  → `Npc.m_Type = boss_gold` (khớp dịch ngược Linux +0x181C=3).
- 2 chỗ đổ `NpcEnchant` trong `KNpc.cpp`: nếu `GetGoldType()==0` và
  `m_Type==boss_gold` thì đẩy `boss_gold`(3). Hệ quái vàng golding giữ ưu
  tiên; NPC tĩnh `bSpecialNpc` chỉ 0/1 — không đụng chạm. **Client CŨ không
  cần build lại vẫn thấy tên vàng.**
- ⚠️ WAuto có ô "bỏ qua boss vàng" (`bSkipGoldboss`) — đi giết boss thì ĐỪNG tick.

**DLL: `bin\server\CoreServer.dll.moi_2508_bossgold` (10:23).** Trước khi bấm
`RESTART_GS_2508.bat`: copy đè `CoreServer.dll.moi_2508_bossgold` → `CoreServer.dll`
(bản 09:24 đang nằm đó vẫn chạy được, chỉ thiếu màu vàng).
`CoreClient.dll.moi_2508_3hoatdong` đã làm tươi (10:23) — vẫn chờ swap client.

## C12 — boss đánh "tàng hình" vẫn mất máu (chủ báo 10:3x, commit `b1f7c2bb`)

Bằng chứng log 2 phía: server cast skill **328** (Tam Nga Tự Tuyết — boss Tín Sứ
849 nhận skill theo ngũ hành) `E3_CANCAST_OK relation=0x8`; client **38/41 lượt**
`relation=0x1 (none)` → `E4_SKILL_ABORT` (2 lượt RÚT VŨ KHÍ thì 0x8 → vẽ được).
Gốc: `GetRelation` client ép `relation_none` cho (quái → người chơi CHƯA rút vũ
khí, `KNpcSet.cpp:1554`); skill 53 không có cờ TargetEnemy nên quái thường không
dính gate, skill 328 có cờ ⇒ hủy vẽ. Vá `KSkills.cpp CanCastSkill`: **client bỏ
qua gate quan hệ khi launcher là NPC** (chiêu đến từ `s2c_skillcast` — server đã
thẩm định; client chỉ vẽ lại). Server không đổi hành vi (guard `#ifndef _SERVER`).
⇒ `CoreClient.dll.moi_2508_3hoatdong` đã làm tươi (10:43) — hiệu lực sau swap client.

## C14 — dọn NPC "Nhiếp Thí Trần" CŨ của bản Việt (chủ báo, commit `bb6ddeaa`)

NPC cũ (script `global
pcchucnang
hieptran.lua`, 2021) nằm trong **dữ liệu map
tĩnh** — không script nào spawn (đã grep hết cây; `addnpcbosssatthu` comment từ
trước). Vá: hàm mới `HD3_DelNpcByName(tên)` (quét `Npc[]` với `m_dwID != 0`, bỏ
player, gỡ đúng khuôn `LuaDelNpc`), gọi **đầu `HD3_DriverInit`** trước khi sinh
NPC 769 Linux (guard nil cho DLL cũ). Console boot in `[3HD] Da xoa N NPC ...` —
N=7 là đúng (7 thành); N=0 nghĩa là map nạp NPC SAU startgame → báo lại để chuyển
sang HD3_Tick lần đầu.

**DLL server MỚI NHẤT: `CoreServer.dll.moi_2508_xoanpccu` (12:08, 18.058.240 B)**
— gộp đủ: HD3_AddNpc/Ex + boss vàng + DelNpcByName. Copy đè `CoreServer.dll`
trước khi bấm `RESTART_GS_2508.bat` (bản 10:23 đang nằm sẵn thiếu C14).

## C15 — PLĐ đúng 100% Linux (3 báo cáo của chủ, commit `14e561ed`)

1. **Xoá 6 NPC thuyền phu CŨ** (đứng TRÙNG toạ độ NPC mới ⇒ chủ click trúng bản
   cũ). `HD3_DelNpcByName` thêm tham số 2 = **map** — chỉ xoá map 336 (template
   "Thuyền phu *" còn dùng cho bến đò thôn, cấm xoá toàn cục).
2. **Gốc "không đánh được quái"**: lên thuyền bằng NPC cũ ⇒ NGOÀI mission ⇒
   không được `mission.lua` `SetFightState(1)` khi thuyền rời bến (fld_head:135
   `SetFightState(0)` = THU VŨ KHÍ theo nghĩa JX1) ⇒ client mask
   `relation_none` (KNpcSet.cpp:1558) chặn CHỌN mục tiêu. Đi đúng luồng Linux
   (NPC mới) là hết — mission tự kéo fight=1 cho mọi thành viên.
3. **Thông báo so bản Linux** (soát toàn bộ relay `TaskList.ini`):
   - Task_86 PLĐ = bản interval **60 = MỖI GIỜ** (các tệp `_NN00` giờ chẵn KHÔNG
     được đăng ký) ⇒ `HD3_PLD_GIO` mặc định đổi thành **đủ 24 giờ**.
   - Task_52 Vượt Ải: chuỗi báo danh thay bằng ĐÚNG chuỗi gốc (có dấu, "10 phút").
   - Task_84 DailyRank: đúng chuỗi gốc + `AddGlobalNews` (JX1 có sẵn) như Linux.
   - PLĐ không có thông báo tầng relay (announce nằm trong mission script chép 1:1).
   - Sát Thủ không có relay task — thông báo đều trong script chép.
   - Tên NPC thuyền phu mới: "Thuyền phu" TCVN3 (trước là ASCII).

**DLL server CHỐT: `CoreServer.dll.moi_2508_c15` (12:31)** — gộp AddNpc + boss
vàng + DelNpcByName(tên, map). Copy đè `CoreServer.dll` (bản 12:08 đang nằm đó
thiếu lọc-map + C15) trước khi bấm `RESTART_GS_2508.bat`. Console boot phải in:
`Da xoa 7 NPC Nhiep Thi Tran cu` + `Da xoa 6 NPC thuyen phu cu (map 336)`.

## C16-C18 — 3 báo cáo có ảnh của chủ (13:2x), commit `fe5b8bec`

- **C16 — vẫn còn 2 Nhiếp Thí Trần**: xoá theo TÊN lúc boot không đủ (NPC cũ
  có thể sinh SAU DriverInit/hồi sinh). Thêm `HD3_DelNpcByScript` (match
  `Npc[].ActionScript` — nhieptran.lua/thuyenphu*.lua cũ, KHÔNG BAO GIỜ đụng
  NPC mới dù trùng tên) gọi ở boot **và MỖI PHÚT** trong `HD3_Tick` (tự lành).
- **C17 — thoại NPC 769**: gói `Describe` ~729B vượt **trần 511B** ⇒ menu đứt
  "<#>M"; "<co lor>" do byte TCVN3 nuốt `<`. Rút gọn còn 425/459B + space
  trước mọi `<color>`. Chỉ đổi chuỗi hiển thị, giữ nguyên function.
- **C18 — F11 Sát Thủ đầy đủ**: (a) hiện **Mục tiêu: tên boss + nơi ở (toạ độ)**
  — bảng `UiTaskGuideSatThuBoss.h` SINH TỰ ĐỘNG từ `killer.txt` (160 boss,
  generator `ReverseTools\gen_satthu_boss_table.py`); (b) 3 nút hoạt động:
  Bỏ nhiệm vụ → gửi `st3_quit` (case 6 uicmdscript) → server chạy `cancel()`
  của nieshichen; Theo dõi/Hủy theo dõi dùng khung chung, nhớ tab đang theo.
- ⏳ **CHƯA làm: kích → tự chạy tới xa phu → qua map → tới toạ độ boss** — đây
  là tính năng AUTO liên map mới (cần chốt tuyến xe/điểm dừng như các
  AUTO_*_SPEC trước nay) — đề xuất làm đợt riêng sau khi chủ duyệt luồng.

**Binary CHỐT (13:35): `CoreServer.dll.moi_2508_c18` + `Game.exe.moi_2508_c18`
+ `CoreClient.dll.moi_2508_c18`** (gộp đủ mọi vá trước + vá của phiên Tín Sứ).
Server: copy đè `CoreServer.dll` (bản 12:31 thiếu C16+st3_quit) rồi restart;
client: copy đè `Game.exe` + `CoreClient.dll` rồi vào lại.

## C19-C20 — AUTO LIÊN MAP tới boss + dọn NPC cũ triệt để (commit `f7761f92`)

**Kích dòng nhiệm vụ Sát Thủ trên F11** → tự chạy tới Xa Phu → thuê xe
(`HD3_ST_TIEN_XE`=1000, khuôn cũ `denchobossST` bản Việt) → server `NewWorld`
tới gần boss + `SetFightState(1)` → client đi bộ nốt tới ô boss (bảng
`KSatThuBossPos.h` SINH TỰ ĐỘNG từ killbosshead — 160 vị trí khớp tên 160/160
với killer.txt). Đang ở đúng map boss thì đi bộ thẳng (miễn phí). Bấm lại = hủy.

Dọn NPC cũ lớp cuối: `HD3_DelNpcByNameEx(tên, map, EXCLUDE-script)` — xoá NPC
trùng tên KHÔNG mang script của mình (bắt được NPC cũ bất kể bind gì), chạy mỗi
phút; mỗi nạn nhân ghi `DebugLog` (idx/setting/map/script) để truy nguồn gốc.

Thoại [1] phục hồi câu 'thách thức thời gian' — gói 482B thường / 499B khi event
(<511). ⚠️ nguyên văn Linux 292B KHÔNG nhét vừa trần 511B của JX1 cùng menu —
bản nén giữ đủ 3 ý (160 giản = đệ nhất; 8 nhiệm vụ/ngày; thách thức thời gian);
muốn nguyên văn từng chữ phải tách thoại 2 trang (đổi luồng — chờ chủ quyết).
Lỗi '8nhiệm vụ<color>là' hoá ra CÓ SẴN trong bản Linux gốc (dòng 21, 292B).

**Bộ binary CHỐT (13:55): `.moi_2508_c20` × 3** (thay c18).

## C21-C23 — nhân bản NPC (gốc thật!) + Xa Phu đủ 39 map + hạ cánh waypoint (`fac440a2`)

- 🔴 **GỐC "NPC ĐÔI"**: nút admin `HD3_ADM_ST_Boot` gọi lặp `HD3_DriverInit` ⇒
  x2 toàn bộ (2 Nhiếp Thí Trần, 2 boss cùng toạ độ). KHÔNG phải NPC dự án cũ —
  quét toàn cây xác nhận mọi spawn/lịch cũ (PLĐ/VA/ST) đã comment/trơ từ trước
  (othermap :16-18, sukien_*, addnpc*, mission03/04, task01 nhánh cũ; task01
  DÙNG CHUNG Tống Kim/CTC nên không đụng). Vá: **DriverInit idempotent** — dọn
  NPC của chính hệ (script `	ollgate\killer\` + `hd3_thuyenphu`) trước khi
  sinh; nút admin giờ gọi lặp an toàn.
- **C21 Xa Phu**: menu luyện công đủ **39 map** (nguồn `BOT_BAI` = đã gộp từ Thần
  Hành Phù 20/08), chia TRANG THEO MỐC (20-30/40-50/60-70/80/90) né trần 511B;
  39 tên CÓ DẤU (`XP_TEN`); `BOT_BAI` giữ nguyên (khớp `s_bai` KPlayerBot).
- **C22 F11**: "số hiệu 147" → "con thứ %d/20 của nhóm cấp %d (ngươi tự chọn
  khi nhận)" — 147 là SỐ DÒNG BẢNG do người chơi tự chọn, không phải thứ tự nhận.
- **C23 hạ cánh**: `st3_goboss` đáp xuống **waypoint mặc định** của map
  (`GetWayPointPos`+`AddTermini` — chuẩn Thần Hành Phù) rồi client TỰ ĐI tới boss
  (timeout pha đi bộ 6 phút).

**Binary mới cần swap: `Game.exe.moi_2508_c23` + `CoreClient.dll.moi_2508_c23`
(14:11). Server KHÔNG đổi C++ — chỉ script (ăn cùng lần restart).**

## C24 — GỐC THẬT của "2 boss 1 toạ độ" + auto đi đúng luồng Xa Phu (`00a0fc5b`)

🔴🔴 **GỐC**: `startgame.lua:202` gọi `addnpcsatthu()` →
`script\startgame\khac\satthu.lua` spawn **20 boss sát thủ bản Việt đang SỐNG**
(không comment như 2 hàm anh em `addnpcbosssatthu`/`addnpcphonglangdo`). Ví dụ
"Dương Phong Dật" map 225 mps (46399,108248) **trùng chỗ** boss Linux 147 cùng
map cell (1452,3377). ⇒ đã TẮT lời gọi.
**Đính chính C23**: chẩn đoán "do nút admin" là SAI — nút admin chỉ là nguồn thứ
hai (vẫn nên giữ idempotent), nguồn CHÍNH là spawner bản Việt này.

**Bài học đưa vào quy trình**: trước khi port một hệ, phải quét `startgame.lua` +
`timerserver.lua` theo TỪNG LỜI GỌI HÀM (không chỉ grep tên hệ) — hàm cũ có thể
nằm ở thư mục tên khác (`startgame\khac\satthu.lua` chứ không phải
`tinhnangoss_satthu\`).

**Auto giờ đi ĐÚNG luồng người chơi thật** (6 pha): chạy tới Xa Phu → chọn
"luyện công" → chọn "Mốc XX" → chọn TÊN MAP → lên map ở **toạ độ mặc định** →
đi bộ tới ô boss. Bảng tên menu từng boss sinh tự động (`s_szST3BossMenu/Moc`
trong `KSatThuBossPos.h`, lấy từ `XP_TEN`/`BOT_BAI` nên khớp từng byte).
**44/160 boss** đi được bằng menu — **gồm đủ 20 con nhóm cấp 90** (nhóm duy nhất
còn phát thưởng); 116 con nhóm 20-80 ở map không thuộc menu luyện công → giữ
đường thuê xe `st3_goboss` làm dự phòng (cũng đáp xuống waypoint).

⚠️ `CoreShell.cpp` bị `ExcludedFromBuild` ở MỌI config Server ⇒ sửa nó **không**
cần build lại server. Binary cần swap: **CLIENT `.moi_2508_c24` (14:27)**.

## C25 — QUÉT SẠCH tàn dư dự án cũ theo TỪNG LỜI GỌI (công cụ `c25_quet_tandu.py`)

Cách quét (bài học C24 — grep theo tên hệ KHÔNG bắt được):
lấy mọi lời gọi hàm còn SỐNG trong `startgame.lua` + `timerserver.lua` → truy
định nghĩa hàm trong toàn cây → soi thân hàm, đọc **đúng vị trí tham số map**
(`AddNpcNew` tham số 3, `AddNpcEx1/3` tham số 4, `NewWorld` tham số 1) → đối
chiếu với 3 dải map: PLĐ 336-339, Vượt Ải 464-495+957, 40 map boss sát thủ.

**Kết quả cuối:**

| Spawner bản Việt | Trạng thái |
|---|---|
| `addnpcsatthu()` (20 boss sát thủ) | 🔴 CÒN SỐNG → **đã tắt** (C24) |
| `addnpcbosssatthu()` | đã comment từ trước |
| `addnpcphonglangdo()` | đã comment từ trước |
| `sukien_phonglangdo()` (chứa `addthuytacdaulinhpld` map 337) | định nghĩa còn nhưng **KHÔNG ai gọi** (call site :83 đã comment) ⇒ code chết |
| `sukien_vuotai()` | call site :82 đã comment ⇒ code chết |
| `othermap.lua` NPC thuyền phu PLĐ | đã comment |
| `task01/task02` (Vượt Ải cũ) | chỉ chạy qua TimerTask **row 1/2**; bản Linux dùng **row 28/29** (PLĐ) và **41/42/43** (Vượt Ải) ⇒ **không va chạm** |

⇒ Sau C24, **không còn lời gọi SỐNG nào** sinh NPC / dịch chuyển vào map của 3
hoạt động ngoài bản Linux. (Không xoá file cũ để giữ lịch sử; chúng đã trơ.)

## Kiểm sau đợt C

- Build: `Server Release|x64` + `Client Release|Win32` + `Game.exe` (Release|Win32,
  cần `/p:VcpkgEnableManifest=false`) — **0 error C / 0 error LNK**.
- `syncheck.exe` (Lua 4.0 engine): **89/89 tệp Lua** trong gương đạt cú pháp.
- Gương ↔ cây sống: **115/115 tệp giống byte** (114 dưới `bin\server` + `uitasklist.ini`
  dưới `bin\client`, đối chiếu md5 tay).
- `check_encoding.py`: FFFD=0 trên mọi tệp C++ đã sửa.
- ScriptError.log boot 08:46: **0 lỗi HD3** (162 lỗi `qianbaoku` là của hệ Tín Sứ cũ,
  có từ 23/08 — KHÔNG thuộc đợt này; 160 lỗi `kill_level OnRevive` chỉ ở boot 01:49,
  trước khi vá).

