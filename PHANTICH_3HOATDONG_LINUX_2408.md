# PHÂN TÍCH DỊCH NGƯỢC — SĂN BOSS SÁT THỦ · PHONG LĂNG ĐỘ · VƯỢT ẢI
### 100% từ bản Linux `D:\ServerLinux` · 24/08 · **CHỈ PHÂN TÍCH, KHÔNG SỬA MỘT DÒNG MÃ NGUỒN NÀO**

Tài liệu này là **bản tổng**. Chi tiết nằm trong `D:\GAMEDEVNEW\ReverseTools\port_3hd\`.
Quy trình: 1 vòng dịch ngược (6 mảng) → 1 vòng phản biện độc lập → 1 vòng quét tìm sót (8 hướng)
→ 1 vòng đối chất chéo. Tổng **28 tác tử**, ~7,6 triệu token, ~2.900 lượt gọi công cụ.
Mọi khẳng định đều kèm `tệp:dòng` hoặc địa chỉ ELF.

---

## 0. ĐỌC THEO THỨ TỰ NÀY

| # | Tệp | Nội dung | Dòng |
|---|---|---|---|
| — | **`ReverseTools\port_3hd\src_utf8\`** | 🟢 **261 tệp Lua đã giải mã UTF-8, GIỮ NGUYÊN SỐ DÒNG** + 106 bảng dữ liệu. Đọc thẳng bằng `Read`. Bảng kê: `src_utf8\INDEX.md` | — |
| 00 | `00_ghichu_dieuphoi.md` | Ghi chú tôi tự kiểm chứng (không qua tác tử) — đọc trước | 260 |
| 01 | `01_satthu.md` | Săn boss sát thủ, đầy đủ + phụ lục phản biện | 951+ |
| 02 | `02_phonglangdo.md` | Phong Lăng Độ, đầy đủ + phản biện | 745 |
| 03 | `03_vuotai.md` | Vượt ải, đầy đủ + phản biện | 1177 |
| 04 | `04_api_gap.md` + `.json` | Khe hở API vòng 1 (**đã bị 15 soát lại — dùng 15**) | 969 |
| 05 | `05_dulieu.md` | NPC/bản đồ/vật phẩm/task id | 761 |
| 06 | `06_phia_jx1.md` | Phía JX1 đã có gì | 880 |
| 07 | `07_doi_chieu_tep.md` | So từng tệp Linux ↔ JX1 | — |
| 08 | `08_hai_goc_dulieu.md` | Đo lệch các gốc dữ liệu | — |
| 09 | `09_bang_toado_patch.md` | 106 bảng trích từ gốc B (34 chỉ-có-ở-B) | — |
| 10 | `10_bosung_gocB_patch.md` | 🔴 Lật "chặn cứng" của Phong Lăng Độ | 597 |
| 11 | `11_bosung_phuthuoc_luc_chay.md` | 🔴 Phụ thuộc gọi lúc chạy + **gốc dữ liệu thứ 3** | — |
| 12 | `12_bosung_khoidong_lich.md` | 🔴 Dây khởi động & lịch chạy thật | — |
| 13 | `13_bosung_phia_client.md` | Phía client | — |
| 14 | `14_bosung_dungdo_id.md` + `id_dungdo.csv` | 🔴 **48 đụng độ định danh nặng** | 340 |
| 15 | `15_bosung_soat_api.md` | ✅ **Bảng hàm CHỐT: 40 nhóm B** | — |
| 16 | `16_bosung_phebinh.md` | Chỗ sót của 01/02/03 | — |
| 17 | `17_bosung_phebinh_dulieu.md` | Chỗ sót của 05/06 | — |

Kèm 10 script Python chạy lại được (`closure3.py`, `dump_utf8.py`, `dec2.py`, `cmp_jx1.py`,
`scan_patch_root.py`, `dump_patch_tables.py`, …) và các bảng `.csv` / `.json`.

---

## 1. BA TÍNH NĂNG NẰM Ở ĐÂU

| Tên tiếng Việt | Tên gốc | Thư mục bản Linux |
|---|---|---|
| Săn boss sát thủ | 杀手任务 Boss | `script\task\tollgate\killer\` + `killbosshead.lua` |
| Phong Lăng Độ | 风陵渡 | `script\missions\fengling_ferry\` |
| Vượt ải | 闯关 | `script\missions\challengeoftime\` |

Bằng chứng khớp tên tiếng Việt — `script\activitysys\config\1008\config.lua`:

```lua
:189  tbConfig[11] = --boss sát thủ
:193      szName = "Nhiệm vụ sát thủ cấp 90",
:200      {"NpcFunLib:CheckKillerdBoss", {90} },
:80   szMessageType = "Chuanguan",   :81  szName = "Vượt qua ải 17",
```

**Quy mô** (bao đóng `Include` → bao đóng đầy đủ kể cả gọi lúc chạy):

| Tính năng | Include | Đầy đủ | Tệp lõi bắt buộc port |
|---|---|---|---|
| Săn boss sát thủ | 67 | 220 | 28 + 9 |
| Phong Lăng Độ | 82 | 202 | 43 + 5 |
| Vượt ải | 103 | 214 | 54 + 8 |

---

## 2. 🔴 NĂM CÁI BẪY — ĐỌC TRƯỚC KHI VIẾT BẤT KỲ DÒNG NÀO

### BẪY 1 — **JX1 ĐÃ CÓ SẴN cả ba tính năng, bản viết lại của người Việt, đang bị TẮT**

Đây là phát hiện làm thay đổi toàn bộ bài toán. Máy chủ JX1 đang chạy đã có:

```
E:\SourceTuanLe\...\bin\server\script\tinhnang\boss_satthu\   (3 tệp,  2024)
                                              \phonglangdo\   (8 tệp,  2021→2025)
                                              \vuot_ai\       (6 tệp,  2023→2025)
```

Và dây bật/tắt (tôi đã tự đọc):

```lua
timerserver.lua:79   -- sukien_vuotai(nHr,nMi)          <- ĐANG COMMENT
timerserver.lua:80   -- sukien_phonglangdo(nHr,nMi)     <- ĐANG COMMENT
startgame.lua:99     -- addnpcbosssatthu()              <- ĐANG COMMENT
startgame.lua:100    addnpcphonglangdo()                <- ĐANG BẬT
```

⇒ Câu hỏi thật của dự án **không phải** "port từ đầu" mà là **"giữ bản Việt hay thay bằng bản gốc?"**
Bản Việt đơn giản hơn nhiều (`lib_phonglangdo.lua` 9.995 B so với cả thư mục `fengling_ferry`),
và bảng toạ độ của nó là **số soạn tay**: `NPC_PHONGLANG` 44 điểm chỉ trùng bảng gốc **2/63 ô**,
`TAB_THULINH` trùng **1/63**. Nhánh sinh quái thuyền 2 (`:168`) và thuyền 3 (`:173`) **đang bị comment**
⇒ hai thuyền trống.

> **Đây là quyết định của chủ game, tôi không tự chọn.** Ba phương án ở mục 7.

### BẪY 2 — bản Linux có **BA gốc dữ liệu**, không phải một

| Gốc | Đường dẫn | Vai trò |
|---|---|---|
| **A** | `D:\ServerLinux\server1` | GameServer: `script\` 5145 tệp, `settings\` 2388 |
| **B** | `D:\ServerLinux\Patch` | **Cây CLIENT tiếng Việt** — nhưng `settings\` của nó chứa **bảng dữ liệu máy chủ mà A không có** |
| **C** | `D:\ServerLinux\gateway\s3relay` | **S3Relay — TOÀN BỘ LỊCH CHẠY** (`relaysetting\task\`, 128 tệp `.lua` + `TaskList.ini`) |

- **34 bảng chỉ có ở B**, trong đó có dữ liệu lõi Phong Lăng Độ (mục 3).
- **Gốc C chưa từng được nhắc trong bất kỳ tài liệu nào của dự án.** Điểm vào thật của
  Phong Lăng Độ và Vượt ải nằm ở đó, không nằm trong `script\`.
- ⚠️ Gốc B **không nhất quán mới hơn**: `tong\tong_header.lua` bản B mới hơn, nhưng `tong\log.lua`
  bản B **thiếu** `WriteTongMoneyChangeLog` mà `tong_mix.lua:65` đang gọi. **Cấm chép đè cả thư mục.**

### BẪY 3 — `AddNpc` lệch ngữ nghĩa tham số 6 (tôi tự dịch ngược, xác nhận)

Bản Linux `0x0811BB10` **tự sinh ngũ hành**:
```asm
0x0811BBDF  call 0x804b28c        ; rand()
0x0811BC8C  sub  edi, eax         ; edi = rand() % 5   <- NGŨ HÀNH
0x0811BC8E  mov  [esp+4], edi     ; -> đối số 2 của hàm tạo NPC
```
Bản JX1 `ScriptFuns.cpp:6834` lấy đúng ô đó từ **tham số Lua thứ 6**:
```c
int nSeries = (int)lua_tonumber(L, 6);
```
`add_killertasknpc` truyền `0` ở vị trí 6 ⇒ **cả 160 boss đều hệ Kim** ⇒ hỏng hợp thành
5 Sát Thủ lệnh cùng ngũ hành và luật tương khắc.

**Lối ra**: dùng `AddNpcEx` (`ScriptFuns.cpp:6874`, đăng ký `:14495`) — tham số 3 là ngũ hành,
khớp đúng bản Linux, và nhận tên dạng chuỗi ở tham số 1 (nhánh này ở `AddNpc` đã bị chú thích `:6823-6827`).

> 🔴 **Lỗi này ĐÃ có sẵn trong cây JX1**: `tinsu_addnpc.lua:38` (đợt Tín Sứ 21/08) gọi
> `AddNpc(..., 0, name)` ⇒ mọi NPC Tín Sứ hiện cũng đang hệ Kim. Nên kiểm lại đợt đó.

### BẪY 4 — **48 đụng độ định danh nặng** với JX1 (`id_dungdo.csv`, 340 dòng)

| Nhóm | Số | Hậu quả nếu port thẳng |
|---|---|---|
| **Vật phẩm** `(genre,detail,particular)` | **42** | Phát **nhầm vật phẩm**, không có thông báo lỗi |
| NPC template 1032/1033/1034 | 3 | Ải cao cấp sinh **Boss New Dragon** thay vì tiểu boss |
| Map 337-339, 480-489 | 13 map | Hai hệ cùng tên giẫm lên nhau trên cùng map |
| Task id 88 | 1 | Chuyển sinh đọc bộ đếm **Dã Tẩu** ⇒ hỏng chỉ số nhân vật |

Cặp nguy hiểm nhất — **tên gần giống nên duyệt tay rất dễ bỏ qua**:

| Bộ số | Bản Linux muốn | JX1 cùng số đang là | JX1 có cùng tên ở |
|---|---|---|---|
| `6,1,399` | **Sát Thủ lệnh** | **Sát thủ giản** | `6,1,398` |
| `6,1,400` | **Sát thủ giản** | Sư đồ thiếp | `6,1,399` |
| `6,1,906` | Quả Huy Hoàng (cao) | Quả Huy Hoàng (**trung**) | `6,1,907` |
| `6,1,215` | Càn Khôn Tạo Hóa Đan (**đại**) | Càn Khôn Tạo Hóa Đan (**trung**) | `6,1,214` |
| `6,1,2742` | **Bảo Rương Vượt ải** | Bảo Rương Tử Mãng Khí Giới | `6,1,3360` |

`lib_killlevel.lua` phát `AddItem(6,1,399,...)` ở **8 dòng** (`:73,77,81,85,89,93,97,103`).
⇒ **Bắt buộc có bảng ánh xạ ID trước khi chép bất kỳ dòng Lua nào.**

### BẪY 5 — mã hoá trộn **hai bảng mã trên cùng một dòng**

Tệp `.lua` trộn **GBK** (chú thích Hán) và **TCVN3** (chuỗi Việt), có dòng chứa cả hai:

```lua
{szName = "Cẩm nang thay đổi trời đất", tbProp = {6,1,1781,...}},	-- 锦囊包含60个--ok
     ^^^^^^^^^^^^ TCVN3                                              ^^^^^^^^ GBK
```

Công cụ `ReverseTools\gbktool.py` của dự án đoán theo **dòng** nên hỏng ở các dòng như vậy,
**và** hỏng ở dòng TCVN3 có lẫn dấu nháy thông minh CP1252 (`0x93 0x94 0x85`).
Đã viết bản vá `port_3hd\dec2.py` giải mã **theo ĐOẠN** (cắt tại `"` và `-`, hai byte không bao giờ
là byte dẫn/theo của GBK hay TCVN3 nên cắt là vô hại).

Đo trên toàn cây 594.696 dòng: số dòng mất ký tự ASCII **1136 → 229** (giảm 80%).
Ba dòng từng hỏng nay đúng, trong đó có `nieshichen.lua:48` — **toàn bộ luật Vượt Ải**.

> Bộ `src_utf8\` đã được sinh lại bằng `dec2.py`. Dùng bộ này, đừng dùng `gbktool` trực tiếp.

---

## 3. TÌNH TRẠNG "ĐIỂM CHẶN TIẾN ĐỘ"

| Điểm chặn vòng 1 nêu | Kết luận cuối |
|---|---|
| **#1 (CỨNG)** thiếu bảng toạ độ spawn Phong Lăng Độ | ❌ **KHÔNG TỒN TẠI** — xem dưới |
| **#2** thiếu 7 tệp `lineup*.txt` ở JX1 | ✅ vẫn đúng (bản Linux có đủ, chép sang là xong) |
| **#3** 16 dòng vật phẩm phải soạn thêm | ✅ vẫn đúng |
| ~~thiếu `GetTabFileData/Height`~~ | ❌ SAI — có ở `bin\server\scriptjx2\lib\file.lua:38,46` (vòng 1 quét sót 189 tệp `scriptjx2\`) |

**Chi tiết #1** — vòng 1 quét đệ quy gốc A, băm `KPakList::FileNameToId`, dò **44 pak / 514.459 mục**
rồi kết luận phải soạn lại bảng. Nhưng họ chỉ tìm ở gốc A. Tệp có thật ở gốc B:

| Tệp | Byte | Dòng dữ liệu |
|---|---|---|
| `Patch\settings\maps\中原北区\渡船\渡船刷怪点.txt` | 893 | **63** |
| `…\风陵渡北岸\baiyingyingboss.txt` / `yanxiaoqianboss.txt` | 99 | 8 / 8 |
| `…\风陵渡南岸\herenwoboss.txt` | 143 | 12 |
| `…\maps\great_night\风陵渡.txt` | 1520 | 100 |

`渡船刷怪点.txt` — 2 cột `XPOS/YPOS`, **đơn vị pixel**, X 50400…53152 = ô **1575…1661**,
Y 101344…104096 = ô **3167…3253**. Bao quanh `boatMAP_POS = {1646, 3233}` (`fld_head.lua:13`).
**126/126 giá trị chia hết 32** ⇒ bảng máy sinh, canh tâm ô. **63 điểm rơi trọn vào 15/15 region
có thật** của bản đồ bến thuyền JX1. Chép nguyên, không phải đoán.

> ⚠️ **Bẫy đơn vị**: cùng cặp (x,y) dùng **hai đơn vị khác nhau** — `NewWorld` chia 32
> (`fld_head.lua:136-137`), `AddNpc` dùng nguyên (`mission.lua:22`). Và các bảng `*boss.txt`
> lưu **Ô** trong khi `渡船刷怪点.txt` lưu **pixel** — cùng thư mục cha, khác đơn vị.

---

## 4. HÀM: JX1 ĐÃ CÓ GÌ, PHẢI THÊM GÌ

### 4.1 Con số chốt

```
Tên hàm ENGINE mà 3 tính năng gọi tới      : 281
   (A) JX1 ĐÃ CÓ                           : 238   (vòng 1: 235, +3 sau khi soát alias)
   (B) JX1 CHƯA CÓ                         :  40   (vòng 1: 42, −3 +1)
        ├─ phải viết mã C mới hoàn toàn    :  38
        ├─ mở rộng hàm sẵn có (NpcDropMoney):  1
        └─ không chặn (NpcName2Replace)     :  1
   (D) KHÔNG phải hàm engine (script tự định nghĩa, xếp nhầm) : 3 → 5
```

Vòng 1 đếm 280 = 235 (A) + 42 (B) + 3 (D). Vòng 2 soát lại: **−3** khỏi B (JX1 đã có dưới
tên khác ⇒ sang A) và **+1** vào B (`JoinMission` vòng 1 chỉ nhắc trong lời văn, không có dòng bảng)
⇒ **238 + 40 + 3 = 281**. Nhóm D còn được nâng lên 5 mục (`15` mục BS-1).
Bảng nhóm A đã được một tác tử độc lập kiểm tự động: **235/235 khớp cả tên Lua lẫn tên hàm C**.

| Hàm bản Linux | JX1 đã có, tên khác | Việc phải làm |
|---|---|---|
| `ITEM_DropRateItem` | `DropRateItem` | thêm 1 dòng đăng ký |
| `NPCINFO_GetSeries` | `GetNpcSeries` (`:7195`/`:14504`) | thêm 1 dòng đăng ký |
| `ST_IsTransLife` | suy từ `ST_GetTransLifeCount` (cùng byte `player+0x86B8`) | vỏ ~5 dòng |

⚠️ **Đừng đếm nhầm**: `GetTabFileHeight` / `GetTabFileData` **không phải hàm engine** —
không tồn tại một byte nào trong `jx_linux_y`; chúng do `script\lib\file.lua:38,46` định nghĩa,
bọc quanh `TabFile_Load` / `TabFile_GetRowCount` / `TabFile_GetCell` (JX1 đã có cả ba).

### 4.2 Bốn mươi hàm phải thêm — xếp theo mức nghiêm trọng

**RẤT CAO (5)** — thiếu là hỏng cả hệ:

| Hàm | Chỗ gọi | Thiếu thì hỏng gì |
|---|---|---|
| `DropItemEx` | `lib/droptemplet.lua:74` | **Toàn bộ hệ rơi đồ theo mẫu** của cả ba hoạt động chết |
| `SendScriptData` | `script_protocol/protocol_def_gs.lua:193` | **Toàn bộ giao thức script → client chết** |
| `Tm2Time` | `activitysys/functionlib.lua:364,380` | Lịch hoạt động không tính được mốc ⇒ **hoạt động không mở** |
| `SetItemBindState` | `lib/droptemplet.lua:144`, `battles/battlehead.lua:1317` | Đồ rơi **không bị khoá** ⇒ bán được ⇒ lỗi kinh tế |
| `ITEM_SetExpiredTime` | 5 chỗ, gồm `fengling_ferry/bossdeath.lua:35` | Đồ boss Phong Lăng Độ **không có hạn** ⇒ lỗi kinh tế |
| `JoinMission` | `challengeoftime/npc/dragonboat_main.lua:163` | **Không báo danh được** vào mission 22 |

**CAO (13)**: `Add120SkillExp`, `AddStatData`, `GetFirstPlayerAtServer`, `GetNextPlayerAtServer`,
`GetGlodEqIndex`, `GetPlatinaEquipIndex`, `GetItemQuality`, `GetNpcAroundPlayerList`, `GetRoomItems`,
`ITEM_SetLeftUsageTime`, `NpcDropMoney`, `OpenProgressBar`, `ST_DoTransLife`.

**TRUNG BÌNH (17)**: `BT_GetBattleParam`, `File_Create`, `FormatTime2Date`, `GetPlatinaLevel`,
`GetItemAllParams`, `ITEM_GetExpiredTime`, `IniFile_Save`, `IniFile_SetData`, `IsDisabledUseHeart`,
`QueryWiseManForSB`, `ST_LevelUp`, `TrimString`, `PET_{Get,Set}{GrownPoint,TamePoint,UpgradePoint}` (6).

**THẤP (4)**: `GetItemGenTime`, `ITEM_GetItemRandSeed`, `NpcName2Replace`, (+`GetPlatinaLevel` log).

Bảng đầy đủ có chữ ký, địa chỉ ELF, mức tin cậy: **`15_bosung_soat_api.md` mục 6**.

### 4.3 Ba hàm JX1 "có tên nhưng là vỏ rỗng" — nguy hiểm hơn thiếu hẳn

| Hàm | Bằng chứng | Hậu quả |
|---|---|---|
| `DisabledUseTownP` | **stub `return 0;`** — `KJx2WarInfra.cpp:258-261` | 7 lời gọi của Phong Lăng Độ + Vượt Ải **không làm gì** ⇒ người chơi **thoát thuyền/mật phòng bằng Hồi thành phù** |
| `Msg2Region` (Linux) | `0x080FB820` = `xor eax,eax; ret` | Có trong bảng đăng ký nhưng không có logic |
| `BT_GetGameData` (JX1) | stub trả 0 | Xem cảnh báo nhóm C, `15` mục 8 |

> Đây là lý do phải **đọc thân hàm**, không tin bảng đăng ký.

### 4.4 Tên hàm hẹn giờ — dễ tra nhầm ra 0 kết quả

| Tên hay bị đoán | Thực tế trong mã |
|---|---|
| `AddMissionTimer`, `SetMissionTimer` | ⛔ **không tồn tại** — tên thật là **`StartMissionTimer`** (`0x08138840`) / **`StopMissionTimer`** (`0x08134720`) |
| `AddGlobalMissionTimer` | ⛔ không tồn tại — tên thật **`StartGlbMSTimer`** |
| `DynamicExecuteFile`, `ExeScript`, `SetTaskScript` | ⛔ không tồn tại trong mã nguồn này |

JX1 đã đăng ký sẵn cả `StartMissionTimer`:14750 · `StopMissionTimer`:14751 · `StartGlbMSTimer`:15120 ·
`OpenMission`:14746 · `CloseMission`:14748 · `RemoteExecute`:15258 · `GlobalExecute`:14882.

---

## 5. LỊCH CHẠY — TÌM RA CHỖ MÀ VÒNG 1 GHI LÀ "NẰM NGOÀI CÂY SCRIPT"

Bản Linux có **hai tầng khởi động**:

| Tầng | Tiến trình | Tệp gốc | Vai trò |
|---|---|---|---|
| 1 | `jx_linux_y` (GameServer) | `script\global\autoexec.lua` → `main()` | **Chỉ dựng NPC/bảng lúc boot.** Không có đồng hồ |
| 2 | `s3relay_y` (S3Relay) | `gateway\s3relay\relaysetting\task\TaskList.ini` + 128 tệp `.lua` | **TOÀN BỘ LỊCH.** Bắn lệnh `dwf`/`dw` sang GameServer |

| Tính năng | Lịch |
|---|---|
| Săn boss sát thủ | **KHÔNG CÓ LỊCH** — nhiệm vụ thường trực; chỉ có điểm boot (NPC 769 + boss) |
| Phong Lăng Độ | `relaysetting\task\fengling_ferry.lua` (**Task_86**), mỗi 60 phút, đúng phút :00 |
| Vượt ải | `relaysetting\task\challengeoftime.lua` (**Task_52**), mỗi 60 phút phút :00 + bảng xếp hạng ngày Task_84 lúc 00:00 |

Điểm boot trong `autoexec.lua` (chỉ Sát thủ):
`:3` `Include(killbosshead.lua)` · `:151` `add_dialognpc()` (NPC 769) · `:158` `add_killertasknpc()` · `:159` `add_alltollgatenpc()`.

- ❌ `settings\systemtimetask.txt` là **TỆP RỖNG 24 byte** ở cả A lẫn B ⇒ **không phải nguồn lịch, đừng port**.
- ✅ JX1 **đã có `GlobalExecute` hiểu tiền tố `dw`/`dwf`** (`KTongJX2.cpp:4056-4090`) ⇒ thân hàm
  `TaskContent()` của relay port sang chạy **nguyên văn**.

---

## 6. CÁC LỖI THẬT CỦA CHÍNH BẢN GỐC (đừng chép sang)

| # | Lỗi | Bằng chứng |
|---|---|---|
| 1 | `killbosshead.lua` chỉ còn **243/3421 dòng sống**; `AddNpc_allbugbear` là **bảng RỖNG** (2.239 dòng comment) ⇒ `add_messengernpc` sinh **0 NPC**. Tổng thực sinh = **18**, không phải "3 bảng NPC ải" | `killbosshead.lua:365-2606`, mốc `--只保留9个` |
| 2 | **Chỉ nhóm cấp 90 còn chơi được** — `killer20..killer80` bị comment trong khi **140 boss cấp 20-80 vẫn spawn** | `nieshichen.lua:73-120` |
| 3 | `SetMemberTask` khai 4 mã trả về trong chú thích nhưng **không có một lệnh `return` nào** | `lib_killlevel.lua:8-13` vs `:21-68` |
| 4 | Mục "Quá quan tầm bảo" trong thoại NPC **không bao giờ hiện** — `tinsert(tbDialog,12,…)` trên bảng 8 phần tử | `nieshichen.lua` |
| 5 | `AddExp_Skill_Extend(140000)` thực tế chỉ cộng **70.000** (`floor(nExp/2)`) | `task_award_extend.lua` |
| 6 | `SoLuongBossSatThuTrongNgay = 100` mâu thuẫn `KILLER_MAXCOUNT = 8` | `configserver/configall.lua:36` |
| 7 | `EventSys:OnPlayerEvent` **không tồn tại** trong bản Linux (0 định nghĩa, không có chuỗi trong ELF) | `kill_level.lua:72,74` |
| 8 | Thưởng "OnFinish" vốn dành **CẢ ĐỘI** (`OnTeamEvent`, `:73` đã comment), bị đổi thành **chỉ người ra đòn cuối** | `kill_level.lua:73` vs `:74` |
| 9 | `tbTimerLog:weiMing(...)` — bảng **không tồn tại ở bất kỳ cây nào** | tệp lõi Vượt Ải |
| 10 | Cú pháp upvalue Lua 4 `%tên` — giá trị **đóng băng lúc nạp tệp**, gán lại sau **không có tác dụng** ⇒ **thứ tự `Include` quyết định giá trị** | `shashou_mibao.lua:13,17,18,21`; `lparser.c:229-242,295-300` |

---

## 7. VIỆC CHO PHIÊN SAU — CẦN CHỦ GAME CHỌN TRƯỚC

Toàn bộ phân tích đã xong. Nhưng **BẪY 1** đặt ra một quyết định kiến trúc mà tôi không tự quyết:

| Phương án | Nội dung | Khối lượng | Rủi ro |
|---|---|---|---|
| **A — Bật bản Việt sẵn có** | Gỡ comment `timerserver.lua:79-80` + `startgame.lua:99`, thay bảng toạ độ soạn tay bằng bảng gốc, mở nhánh thuyền 2/3 | **Nhỏ nhất** | Giữ nguyên thiết kế đơn giản hơn bản gốc; không có ải ẩn / bảng xếp hạng ngày |
| **B — Port nguyên bản gốc** | Chép ~125 tệp Lua + 40 hàm C mới + bảng ánh xạ ID | **Lớn nhất** | Đụng độ ID (48 chỗ), phải viết 38 hàm C |
| **C — Lai** | Giữ khung bản Việt, lấy từ bản gốc: bảng toạ độ, bảng thưởng, cơ chế ải ẩn, lịch relay | Trung bình | Phải quyết từng phần |

**Tôi đề xuất C**, vì bản Việt đã chạy được trên chính engine JX1 (đã qua vấn đề ABI/encoding),
còn thứ nó thiếu lại đúng là thứ bản gốc có sẵn dữ liệu chuẩn (63 điểm spawn, bảng ải, bảng thưởng).

Dù chọn gì, ba việc dưới đây **bắt buộc làm trước** và không phụ thuộc phương án:

1. **Lập bảng ánh xạ ID vật phẩm** (42 chỗ trùng nặng) — `id_dungdo.csv`.
2. **Vá `DisabledUseTownP`** (`KJx2WarInfra.cpp:258-261`) — hiện là vỏ rỗng, người chơi thoát bản đồ bằng phù.
3. **Sửa `AddNpc` → `AddNpcEx`** ở mọi chỗ port, và **kiểm lại `tinsu_addnpc.lua:38`** của đợt Tín Sứ 21/08.

---

## 8. GHI CHÚ CHẤT LƯỢNG — NHỮNG GÌ ĐÃ BỊ SỬA

Quy trình phản biện đã bắt được lỗi ở **cả tài liệu của tác tử lẫn của tôi**:

| Vòng | Kiểm | Sai | Ví dụ nặng nhất |
|---|---|---|---|
| Phản biện vòng 1 (6 mảng) | ~150 khẳng định | ~40 | "task 1192/1193 không xung đột" → **có xung đột** (`lenhbaitanthu.lua`, `baivip.lua`) |
| Đối chất vòng 2 (8 hướng) | ~100 khẳng định | ~35 | "CHẶN #2 bị lật" → **vẫn đứng vững** (nhầm với hệ quả kèm) |
| **Tài liệu của chính tôi** | — | **2** | `09_bang_toado_patch.md` ghi 106 bảng "chỉ có ở B" → thật ra **34**; bộ `src_utf8` giải mã sai **3 dòng** (đã sửa bằng `dec2.py`) |

Những chỗ còn ghi **CHƯA XÁC MINH** (không đoán bừa): ý nghĩa mặt nạ `31` của
`GetNpcAroundPlayerList`; gốc thời gian `ITEM_SetExpiredTime` (`KItem.cpp:2629` cộng `1451581200`);
JX1 lưu trạng thái khoá vật phẩm vào đâu (`KItem.h` không có trường bind);
`BT_GetBattleParam` `0x081C69B0` đọc kho dữ liệu nào.
