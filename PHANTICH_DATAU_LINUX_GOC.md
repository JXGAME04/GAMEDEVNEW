# PHÂN TÍCH — TÍNH NĂNG DÃ TẨU BẢN LINUX GỐC & KHẢ NĂNG DÙNG 100% SCRIPT GỐC CHO MÁY CHỦ JX1

Ngày: 15/08/2026 · Trạng thái: **CHỈ PHÂN TÍCH, KHÔNG SỬA MỘT DÒNG MÃ NÀO**
Nguồn Linux gốc: `D:\ServerLinux\server1` (JX2 / Kiếm Thế, binary `jx_linux_y`)
Nguồn JX1 (đích): `D:\GAMEDEVNEW\Sources` + `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server`

---

## 0. KẾT LUẬN NGẮN (đọc mục này là đủ để quyết)

| Câu hỏi | Trả lời |
|---|---|
| Dã Tẩu bản Linux gốc là gì? | Hệ **`tasklink`** (chuỗi nhiệm vụ), 6 loại nhiệm vụ, 4 file lõi + 21 bảng dữ liệu |
| Script gốc có chạy được trên máy ảo Lua của JX1 không? | **CÓ — 100%, không phải sửa một dòng cú pháp nào.** Cả hai đều là Lua 4.0 |
| Có dùng được "full script bản gốc" không? | **CÓ, với đúng nghĩa "toàn bộ script của tính năng Dã Tẩu"** (≈56 file + 21 bảng). **KHÔNG** với nghĩa "cả cây script 5.059 file của bản Linux" — vượt trần `MAX_SCRIPT_IN_SET 5000` |
| Phải viết thêm bao nhiêu hàm C++? | **≈15 hàm.** Trong đó 1 chỉ là đổi tên, 3 là stub vô hại, 4 là hàm 10–30 dòng, 7 là việc thật |
| Trở ngại lớn nhất? | **Không phải Lua, không phải task ID.** Là (a) `Prise` — cửa sổ 3 lựa chọn phần thưởng, (b) `GetItemMagicAttrib` — đọc thuộc tính ma pháp trang bị, (c) hệ thưởng `tbAwardTemplet` bên JX1 đang **rỗng ruột** |
| Có xung đột với Dã Tẩu JX1 đang chạy không? | **Không xung đột số hiệu task** (Linux dùng dải 1020–1046, JX1 hoàn toàn trống). Xung đột ở **3 điểm hardcode trong C++** và **2 file item trùng tên** |
| Bất ngờ lớn nhất | **Trên chính bản Linux gốc của anh, Dã Tẩu ĐÃ BỊ VNG TẮT từ 30/10/2014.** Mã và dữ liệu còn nguyên vẹn, chỉ bị chặn ở đúng 1 dòng |

---

## 1. DÃ TẨU BẢN LINUX GỐC LÀ CÁI GÌ

### 1.1 Có HAI hệ mang tên Dã Tẩu — phải chọn đúng

| Hệ | Vị trí | Thế hệ | Trạng thái trên `D:\ServerLinux\server1` |
|---|---|---|---|
| **A. `tasklink`** (chuỗi nhiệm vụ) | `script/task/newtask/tasklink/` + `script/global/seasonnpc.lua` | KingSoft 2004, VNG Việt hoá | **Được nạp mỗi lần boot, nhưng lối vào bị chặn** |
| **B. `quest_daily`** (VNG viết lại) | `vng_script/features/quest_daily/` | VNG ~2014+ | **Chưa bao giờ chạy** trên bản này |

**Bằng chứng hệ B chưa từng chạy** (tôi tự kiểm chứng, không tin báo cáo):
`Logs/KSG_ScriptOutputLog_20260806.txt` — 140.403 dòng của một lần khởi động server thật:
```
grep -c "vng_script"  → 0
grep -c "quest_daily" → 0
grep -c "vng_lib"     → 30      (đây là script/vng_lib/, nằm TRONG cây script)
```
Máy chủ **chỉ quét cây `script/`**. Cả thư mục `vng_script/` là mã chết trên đĩa.

**Bằng chứng hệ A bị chặn** — `script/global/seasonnpc.lua`, dòng 118–119:
```lua
function main()
	do return end -- close and remove npc Da tau, by vng 20141030
```
Chính VNG đóng NPC Dã Tẩu ngày 30/10/2014. Toàn bộ hội thoại (`tasklink_entence`, `Task_MainDialog`, `Task_TaskProcess`, `Task_AwardRecord`) nằm **sau** dòng `return` này.

**Nhưng phần ruột còn nguyên vẹn 100%:**
- 11 file lua của hệ đều có dòng `ExecuteCode` trong log boot ⇒ **biên dịch sạch, không lỗi**.
- **21/21 bảng dữ liệu tồn tại thật** trong `settings/task/` (~130 KB).
- Chỉ 2 file item bị mất: `seasonnpc_box.lua`, `item_yesoumishu.lua` (magicscript vẫn trỏ tới).

> **Kết luận mục 1:** thứ anh muốn port là **hệ A (`tasklink`)**. Nó nguyên vẹn, chỉ bị bịt lối vào bằng 1 dòng. Bỏ dòng đó = khôi phục đúng bản gốc, **không phải chế thêm**.

### 1.2 Còn một cái bẫy: có HAI bản `tasklink_head.lua`

| File | Cỡ | Đời | Tên hàm | Dùng? |
|---|---|---|---|---|
| `script/task/newtask/tasklink/tasklink_head.lua` | 35.866 B | Bản mới | tiền tố `tl_*` | ✅ **BẢN THẬT** |
| `script/global/tasklink_head.lua` | 31.424 B | 2004 | CamelCase `DealTask/GetTaskState` | ❌ Mồ côi, không file nào Include |

Khác biệt thực chất (không chỉ đổi tên):
- Bản mới: `TL_MAXLOOPS=20` (cũ 10); thêm **hệ thoại ngẫu nhiên** 6 bảng `talk_*.txt`; thêm chuyển sinh `ST_GetTransLifeCount`; **tách exp thành 2 biến** (2574 = exp/1e5, 1034 = phần dư) vì 1 biến task không chứa nổi exp lớn — bản cũ **tràn số**.
- **Bản cũ có LỖI THẬT ở loại 5**: so `GetRepute() >= GetTask(1026)` (gần như luôn đúng ngay lập tức). Bản mới sửa thành phép trừ.

> **Nếu port nhầm bản `/global/` là lấy phải bản có lỗi tràn số và lỗi logic.**

---

## 2. CƠ CHẾ ĐẦY ĐỦ CỦA HỆ `tasklink`

### 2.1 Cấu trúc vòng đời

```
20 nhiệm vụ  =  1 "đơn vị link"
20 đơn vị    =  1 vòng (loop)  =  400 nhiệm vụ
20 vòng      =  hết chu kỳ (TL_MAXLOOPS)
```
`levellink.txt` chia cấp nhân vật thành 5 khoảng, mỗi khoảng có số link riêng:

| Cấp | Số link | Chỉ tiêu mỗi link |
|---|---|---|
| 1–19 | 2 (link 1–2) | 10 + 10 |
| 20–49 | 3 (link 3–5) | 6 + 6 + 8 |
| 50–79 | 5 (link 6–10) | 4 mỗi link |
| 80–99 | 5 (link 11–15) | 4 mỗi link |
| 100+ | 5 (link 16–20) | 4 mỗi link |

Link hiện tại = **cột tra** của mọi bảng (`TaskRate1..TaskRate20`). Link càng cao ⇒ nhiệm vụ càng khó, thưởng càng lớn.

### 2.2 SÁU loại nhiệm vụ (không có loại "đánh quái")

| # | Loại | Bảng dữ liệu | Số dòng | Cách hoàn thành |
|---|---|---|---|---|
| 1 | **Mua vật phẩm** | `tasklink_buygoods.txt` | 35 | Giao đúng 1 món, khớp tuyệt đối 5 trường (Genre/Detail/Particular/GoodsFive/Level). Món bị **xoá** |
| 2 | **Tìm vật phẩm có thuộc tính** | `tasklink_findgoods.txt` | 526 (79 KB) | Giao 1 món; lặp 6 ô ma pháp, cần 1 ô khớp `MagicEnName` và `MinValue ≤ giá trị ≤ MaxValue`. Món bị **xoá** |
| 3 | **Khoe vật phẩm** | `tasklink_showgoods.txt` | 45 | Chỉ **cho xem**, KHÔNG mất đồ. Loại rẻ nhất cho người chơi |
| 4 | **Thu thập Địa Đồ Chỉ / Mật Chỉ** | `tasklink_findmaps.txt` | 208 (104 map × 2 loại) | **Tỉ lệ ra cao nhất — xương sống của hệ.** Nhặt cuộn đúng map, đúng loại |
| 5 | **Nâng chỉ số** | `tasklink_upground.txt` | 45 | Chụp mốc lúc nhận, so hiệu số. NumericType: 2=KN, 3=Danh vọng, 4=Phúc duyên, 5=PK, 6=Tống Kim |
| 6 | **Thu thập Sơn Hà Xã Tắc** | `tasklink_worldmaps.txt` | 11 | Nộp N mảnh (task 1027). Tỉ lệ **cố định 1000** ở cả 20 link |

Trọng số chọn loại nằm ở `tasklink_mainlink.txt` (6 dòng × 20 cột). Ví dụ loại 1 (mua đồ): link 1 = 8000, link 13 = 500, **từ link 14 trở đi = 0** (biến mất hẳn ở cấp cao).

### 2.3 Biến trạng thái — chỉ 1 biến chính, cắt thành 4 byte

**Task 1020** (qua `tl_gettaskstate/tl_settaskstate`, dùng `GetByte/SetByte`):
```
byte 1 = TIMES   số nhiệm vụ đã làm trong link hiện tại (0..19)
byte 2 = LINKS   link hiện tại (1..20)  ← chính là "TaskLevel", cột tra mọi bảng
byte 3 = LOOPS   số vòng đã xong (0..19)
byte 4 = CANCEL  số lượt huỷ còn lại (0..254)
```

**Task 1028 = tiến trình (course)** — bản lề chống mất thưởng:
| Giá trị | Nghĩa | NPC làm gì |
|---|---|---|
| 0 | Chưa nhập môn | Hỏi 3 lựa chọn |
| 1 | Đang làm | Mở cửa sổ nhiệm vụ |
| 2 | **Đã nộp đúng, chưa chọn thưởng** | **Nhảy thẳng tới trao thưởng** (chống thoát game giữa chừng) |
| 3 | Đã lãnh thưởng | Hỏi có làm tiếp không |

**Toàn bộ dải task Dã Tẩu Linux:** 1020, 1021, 1025, 1026, 1027, 1028, 1029, 1030, 1031, 1032, 1033, 1034, 1035, 1036, 1037, 1038–1043 (6 ô thoại), 1044, 1045, 1046, cộng 1825, 2570, 2571, 2574, 2575, 2690, 2797, 2419, 2420.

### 2.4 Cơ chế chống gian lận có sẵn trong bản gốc (rất đáng giữ)

1. **Task 1046** = bản sao độc lập của byte4 (số lượt huỷ). `_CancelTaskDebug()` so hai giá trị, lệch thì `WriteLog` + chặn.
2. **Task 1045** = cờ tái nhập; `Task_Cancel` chỉ chạy khi = 1 ⇒ chặn double-click.
3. **Task 1036** = đếm huỷ lậu; > 2 lần thì NPC **từ chối tiếp chuyện 605 giây**.
4. **Task 1037 = seed ngẫu nhiên khoá cứng.** `SetRandSeed(nt_getTask(1037))` trước khi bốc thưởng, xong khôi phục seed cũ ⇒ **cùng người chơi + cùng lần nộp luôn ra cùng bộ 3 phần thưởng**, thoát/vào lại không quay lại được.

> Điểm 4 là lý do **`SetRandSeed` bắt buộc phải có** — thiếu nó là mất cả cơ chế chống hack, không phải "hàm phụ".

### 2.5 Công thức phần thưởng (đo từ `tasklink_award.lua`)

Bốc **3 ô KHÁC LOẠI** từ `myMainAwardRate = {20, 33, 34, 8, 5}`:
Tiền 20% · Kinh nghiệm 33% · Vật phẩm 34% · "Đổi lần nữa" 8% · Lượt huỷ NV 5%.
Người chơi chọn **1 trong 3** (hàm `Prise`).

```
V_chung = TaskValue2 * (1 + (soLink + soLan) * 0.1) + soVong * 0.2

[1] TIỀN   = floor( V_chung * 0.05 * 1.15 * (C_Random(60,100)*0.01) ) + TaskValue1
             (1.15 = "+15% cho bản Việt Nam, 2006/8/23")

[2] KN     = floor( (TaskValue1 + V_chung) * 0.36 * (C_Random(80,120)*0.01) )

[3] VẬT PHẨM: V = TaskValue1 + (TaskValue2*(1 + (cnt+times)*0.1 + loops*0.2))   ← ngoặc KHÁC 2 công thức trên
             V = V * (GetLucky(0)*0.001 + 1)
             Đổi V thành trọng số trên 98 dòng award_basic.txt:
                món có giá ≤ V  → trọng số 1/98 (đều nhau)
                món có giá > V  → trọng số V/(98*giá)   (tỉ lệ nghịch)
             ⇒ V càng lớn, đồ đắt càng dễ ra, TUYẾN TÍNH

[4] ĐỔI LẦN NỮA: hệ số may mắn 0.03 (GẤP 30 LẦN nhánh [3]), túi tiền +20%,
             bảng 98 dòng + 2 hàng ảo (exp, tiền). Kết quả bị GIẤU khỏi người chơi
```

**Hai hàm hệ số có lỗi sẵn trong bản gốc — phải biết trước khi port:**
- `TireReduce(v)`: lấy `GetTiredDegree()` rồi **gán đè `TireDegree = 0` ngay dòng sau** ⇒ hệ mệt mỏi **bị vô hiệu hoá**, hai nhánh ÷2 và =0 không bao giờ chạy.
- `CountDoubleMode(v)`: điều kiện `if d ~= 0 or d ~= nil` **luôn đúng** ⇒ luôn nhân. Nếu sự kiện trả 0 thì **mọi phần thưởng = 0**.

> Đây là hành vi **thật** của bản gốc. "100% bản gốc" nghĩa là **giữ nguyên cả hai lỗi này**. Nếu anh muốn sửa thì đó là lệch khỏi bản gốc — phải là quyết định có ý thức của chủ game.

**Cống hiến bang đi kèm:** mọi loại thưởng đều `SetTaskTemp(154, giaTri)` rồi `tongaward_tasklink()` → `floor(nMainValue/8)` → `ContriValueEntryLogic(nValue, EVE_YESHOU)`.

---

## 3. BÊN JX1 CỦA ANH ĐANG CÓ GÌ

### 3.1 Ba hệ Dã Tẩu chồng nhau, chỉ MỘT hệ sống

| Hệ | Vị trí | Trạng thái |
|---|---|---|
| **A. "Fong Kiều 2021"** | `script/tinhnang/datau/` + `script/global/npcchucnang/datau.lua` | ✅ **ĐANG CHẠY** (xác nhận bằng `ScripNpcDialog.log` 15/08/2026) |
| B. "dammejx 2013" | `script/datau/dataunew.lua` | ❌ Mã chết — Include `script/source/sourcejx49.lua`, thư mục không tồn tại |
| C. taskid cũ | `script/header/taskid.lua` | ❌ 6 hằng số định nghĩa, 0 lần dùng |

**Bẫy: có BẢN SONG SINH.** `script/tinhnang/datau/datau.lua` (39.275 B) và `script/global/npcchucnang/datau.lua` (40.685 B) gần giống nhau (khác 90 dòng), **chỉ bản thứ hai được gắn vào NPC**. Bản thứ nhất bị chặn bằng `Talk("Đang trong thời gian đua Top...")`. Sửa nhầm file = không có tác dụng gì.

### 3.2 Hệ JX1 đang chạy khác hệ Linux ra sao

| | JX1 (Fong Kiều 2021) | Linux gốc (tasklink) |
|---|---|---|
| Số loại NV | **8** (có đánh quái, tìm mật chỉ, Tống Kim) | **6** (không có đánh quái) |
| Cấu trúc | Phẳng — 40 NV/ngày, không có link/loop | **Phân tầng** — 20 NV/link, 20 link/vòng, 20 vòng |
| Chọn NV | `random(1,9)` theo cấp | **Roulette có trọng số theo link**, tinh vi hơn nhiều |
| Chọn thưởng | **UI cứng trong client C++** (`UiFinishQuest.cpp`, 8 kiểu nút) | **Hộp thoại `Prise`** 3 lựa chọn, thuần script |
| Chống hack | không | 4 lớp (1036/1037/1045/1046) |
| Task ID | 12,13,14,75,87–99,317,318,395,397 | 1020–1046, 1825, 2419/2420, 2570–2575, 2690, 2797 |

### 3.3 NPC Dã Tẩu của JX1

- Tên **"Dã Tẩu"**, **template 108, NameID 59**
- Đặt tại **10 điểm**: Phượng Tường(1), Thành Đô(11), Giang Tân Thôn(20), Biện Kinh(37), Ba Lăng Huyện(53), Tương Dương(78), Dương Châu(80), Long Môn Trấn(121), Đại Lý(162), Lâm An(176)
- Gắn qua `AddNpcNew(108, 1, map, X*32, Y*32, "\\script\\global\\npcchucnang\\datau.lua", 6, 59)`
- Minimap đã có marker trong `settings/maps/maptraffic.ini`

> **Chuyển sang hệ Linux mà vẫn giữ NPC: chỉ cần đổi đường dẫn script trong 10 dòng `AddNpcNew`.** Không phải động vào template/NameID/minimap.

### 3.4 Ba điểm HARDCODE trong C++ của JX1 (quan trọng nhất)

| Vị trí | Nội dung |
|---|---|
| `Core/Src/GameDataDef.h:75` | `#define TASKVALUE_DANHQUAIDATAU 89` — task 89 khoá cứng trong header |
| `Core/Src/KNpc.cpp:1525` | Mỗi lần người chơi giết NPC đều gọi cứng `danhquai.lua::OnDeathMonsterDaTau` |
| `Core/Src/KPlayer.cpp:4748, 4758` | Gọi cứng `matchi.lua::PickItem` / `diadochi.lua::PickItem` |

**Phát hiện phụ (lỗi sẵn có của JX1, ngoài phạm vi):** hàm `PickItem` **không được định nghĩa** trong `matchi.lua` và `diadochi.lua` ⇒ 2 hook nhặt-vật-trên-đất ở `KPlayer.cpp:4748/4758` hiện là **NO-OP**. Đường sống thật là người chơi click phải item 205/212.

---

## 4. ĐỐI CHIẾU — CÁI GÌ CHẠY ĐƯỢC NGUYÊN XI

### 4.1 Máy ảo Lua: TRÙNG KHỚP (tin tốt nhất)

| | Phiên bản | Bằng chứng |
|---|---|---|
| Linux `jx_linux_y` | **Lua 4.0** | Chuỗi `$Lua: Lua 4.0 Copyright (C) 1994-2000 TeCGraf, PUC-Rio $` dump trực tiếp từ byte |
| JX1 | **Lua 4.0.1** | `Sources/Library/LuaLib/src/lua.h:50` → `#define LUA_VERSION "Lua 4.0.1"` |

Quét toàn bộ **5.230 file .lua** của máy chủ Linux:
```
local function 0 | pairs( 0 | ipairs( 0 | setmetatable 0 | __index 0 | pcall 0 | coroutine 0
string.* 0 | table.* 0 | math.* 0 | require 0 | toán tử # 0 | true/false 0
```
Bằng chứng mạnh nhất: **4.149 chỗ dùng `%tên` (upvalue)** — cú pháp **CHỈ tồn tại ở Lua 4.0** (`lparser.c:465`), là lỗi biên dịch trên Lua 5. Cộng 741 chỗ `for k,v in <bảng> do` (Lua 4 lặp thẳng trên bảng).

Chính script JX1 đang chạy cũng nói cùng thứ tiếng: `getn(` 890 lần, `tinsert(` 208, `format(` 700, `%upvalue` 1.950, `local function` 0.

> **⇒ 0 dòng cú pháp phải sửa. Đây là điều kiện tiên quyết quan trọng nhất, và nó đã thoả.**

### 4.2 Hạ tầng nạp script JX2: ĐÃ CÓ SẴN (di sản đợt Bang Hội)

| Thứ | Trạng thái bên JX1 |
|---|---|
| `IncludeLib` + alias `IL` | ✅ `ScriptFuns.cpp:1974`, đăng ký ở `:12416`/`:12422` |
| `TabFile_Load / GetCell / GetRowCount / UnLoad` | ✅ `ScriptFuns.cpp:2543 / 2583 / 2631 / 2644` |
| Thư mục `scriptjx2/` với thư viện khung JX2 | ✅ 189 file, `scriptjx2/lib/` chính là `script/lib/` của bản Linux (ngày file `Oct 30 2021` trùng khít) |
| Tổng hàm Lua đăng ký | 459 mục trong bảng, ~849 tên |

> **Con đường "thả script gốc vào chạy" đã được khai thông một lần rồi.** Đây là lợi thế lớn: không phải dựng từ số 0 như đợt Bang Hội.

### 4.3 Bề mặt API — con số CHÍNH XÁC

Phải phân biệt hai phạm vi, vì chênh nhau rất xa:

**(a) Toàn bộ bao đóng 113 file** (gồm cả khung `activitysys` và hệ `quest_daily`):

| Nhóm | Số hàm | % |
|---|---|---|
| CÓ SẴN | 226 | 74,1% |
| ĐỔI TÊN (wrapper mỏng) | 35 | 11,5% |
| THIẾU (viết được từ API JX1) | 42 | 13,8% |
| KHÔNG THỂ CÓ | 2 | 0,7% |
| **Tổng** | **305** | |

**(b) RIÊNG lõi Dã Tẩu `tasklink`** — con số này mới là con số phải nhìn.
Tôi tự trích và tự đối chiếu với `ScriptFuns.cpp`:

| File | Ứng viên | Thiếu thật |
|---|---|---|
| `tasklink_head.lua` + `tasklink_award.lua` | 30 hàm C++ | **8** |
| `seasonnpc.lua` (1.129 dòng, file điều phối) | 115 ứng viên | **8** |
| **Hợp lại (đã loại trùng)** | | **≈15 hàm** |

### 4.4 Danh sách ĐẦY ĐỦ 15 hàm C++ còn thiếu

| # | Hàm | Dùng ở đâu | Mức khó | Ghi chú |
|---|---|---|---|---|
| 1 | `GetTeamMember(i)` | chia đồ cho tổ đội | **Đổi tên** | JX1 có `GetTeamMem` — shim 1 dòng |
| 2 | `GetTiredDegree()` | `TireReduce` | **Stub vô hại** | Bản gốc tự gán đè `= 0` ⇒ trả 0 là **giống hệt hành vi thật** |
| 3 | `SyncTaskValue` | mọi `nt_setTask` | **Đã có, là stub** | `KTongJX2.cpp:3977` trả 1. JX1 tự đồng bộ trong `SetSaveVal` ⇒ vô hại |
| 4 | `C_Random(min,max)` | bốc thưởng | **Dễ** | JX1 có `random` + `RandomNew` — shim Lua |
| 5 | `GetBitTask / SetBitTask` | lệnh bài, đếm ngày | **Dễ** | ~30 dòng C, hoặc Lua thuần bằng `floor/mod` |
| 6 | `CreateNewSayEx` | hội thoại | **Dễ** | JX1 có `Say`/`Describe` |
| 7 | `SetRandSeed(seed)` | **chống quay lại thưởng** | **Vừa** | ~10 dòng C bọc `srand`. **Bắt buộc** — mất nó là mất lớp chống hack |
| 8 | `SetItemMagicLevel` | item "Tích luỹ Dã Tẩu" | **Vừa** | Ghi ô ma pháp của item |
| 9 | `SyncItem` | sau khi sửa item | **Vừa** | `m_ItemList.SyncItem` đã có trong Core |
| 10 | `PlayerActionLog` | ghi log | **Vừa** | JX1 có `WriteLog` |
| 11 | **`GetItemMagicAttrib(idx, i)`** | **loại NV 2 và 3** | **KHÓ** | JX1 có `GetOTItem(idx)` (`ScriptFuns.cpp:4482`) trả 7 cặp type/value — phải viết lớp ánh xạ. **Thiếu nó là 2/6 loại nhiệm vụ chết** |
| 12 | **`Prise(msg, t1, t2, t3)`** | **cửa sổ 3 lựa chọn thưởng** | **KHÓ** | Trái tim UI của hệ. Phải dựng lại bằng `Say` + callback `/#Hàm(n)` |
| 13 | `CalcEquiproomItemCount` | đếm đồ trong rương | **Vừa** | `KItemList::CountCommonItem` có sẵn nhưng **không có wildcard −1** |
| 14 | `ConsumeEquiproomItem` | trừ đồ trong rương | **Vừa** | cùng gốc với #13 |
| 15 | `DynamicExecuteByPlayer` | chạy hàm trên người chơi khác | **Vừa** | `g_GetScript` + `KLuaScript::CallFunction`. **Bẫy: `g_GetScript` không tự nạp** |

**Toàn bộ 21 hàm còn lại của lõi đều CÓ SẴN**, đã tự đếm trong `ScriptFuns.cpp`:
`TabFile_Load`, `TabFile_GetCell`, `TabFile_GetRowCount`, `SetByte`, `GetByte`, `GetLevel`, `GetExp`, `GetRepute`, `GetPK`, `ST_GetTransLifeCount`, `Msg2Player`, `Msg2SubWorld`, `SetTask`, `GetTask`, `SetTaskTemp`, `GetTaskTemp`, `GetLucky`, `Include`, `IncludeLib`, `AddItem`, `AddGoldItem`, `AddGlobalNews`, `GetGiveItemUnit`, `RemoveItemByIndex`, `GiveItemUI`, `Earn`, `AddOwnExp`, `GetGameTime`, `GetLocalDate`, `WriteLog`, `Say`, `Talk`, `GetName`, `GetAccount`, `SubWorldIdx2ID`, `GetTeamSize`.

### 4.5 Task ID: KHÔNG XUNG ĐỘT

- `MAX_TASK = 3000` (`Core/Src/KPlayerTask.h:13`), mảng `int nSave[3000]` + `int nClear[3000]`.
- Kẹp biên: `GetSaveVal/SetSaveVal` kiểm `nNo >= MAX_TASK` rồi **bỏ qua IM LẶNG** (`KPlayerTask.cpp:67-81`) — id ≥ 3000 không báo lỗi gì cả, rất khó phát hiện.
- Script JX1 hiện chỉ dùng **84 task id rời rạc, cao nhất 2350**.
- **Dải 1020–1046 mà Dã Tẩu Linux dùng: HOÀN TOÀN TRỐNG bên JX1** (grep chỉ thấy 1017, 1122, 1564, 1565, 1757 được dùng).
- Các số 1825, 2419, 2420, 2570–2575, 2690, 2797 cũng trống.

> **⇒ Giữ nguyên số task của bản gốc được, không phải đánh số lại. Đây là điều kiện quan trọng thứ hai của "100% bản gốc", và nó cũng thoả.**

**Ngoại lệ duy nhất:** `script/global/mel/lenhbaidatau.lua` dùng `DTL_TASK = 8000` — **vượt trần 3000**, sẽ bị bỏ qua im lặng. Nhưng file này **mồ côi ngay trên bản Linux** (không item nào trỏ tới), nên có thể bỏ hẳn mà không mất tính năng.

---

## 5. BẢY NÚT THẮT THẬT SỰ

### Nút 1 — `Prise` (nặng nhất về UI)
Cửa sổ 3 lựa chọn phần thưởng là trải nghiệm cốt lõi của Dã Tẩu. JX1 không có hàm này. Phải dựng lại bằng hộp thoại chuẩn của JX1, và hộp thoại JX1 **chật hơn JX2**:

| Giới hạn (đo từ mã nguồn, không ước lượng) | Giá trị |
|---|---|
| Gói tin `PLAYER_SCRIPTACTION_SYNC.m_pContent` | **512 byte** (`KProtocol.h:1173`), dùng được 511 — **tổng** của mô tả + `|` + mọi nhãn nút |
| Tràn thì sao | **CẮT ÂM THẦM**, không báo lỗi (`ScriptFuns.cpp:538`) |
| Số nút tối đa (server) | 50 (`KPlayer.h:26`) |
| Chuỗi callback | ≤ 31 ký tự (`m_szTaskAnswerFun[50][32]`) |
| Tham số callback | tối đa 5, **CHỈ SỐ NGUYÊN** (parse bằng `atoi`) |
| Ký tự `\|` | **CẤM TUYỆT ĐỐI** — là dấu tách câu hỏi/đáp án |
| Ô mô tả (client) | **6 dòng × 53 nửa-ký-tự, KHÔNG CUỘN** — chữ vượt là biến mất |

**Tin tốt:** menu Dã Tẩu sinh nhiều nhất **4 nút + "Kết thúc đối thoại" = 5 nút**, mô tả dài nhất ~92 byte, tổng ~300 byte < 511, ~5 dòng < 6 dòng ⇒ **vừa khít, không cần phân trang**. Nhưng mọi mở rộng về sau phải đo lại.

### Nút 2 — `GetItemMagicAttrib` (chặn 2/6 loại nhiệm vụ)
Loại 2 (tìm đồ có thuộc tính) và loại 3 (khoe đồ) đều lặp 6 ô ma pháp. JX1 có `GetOTItem` trả 7 cặp type/value — cấu trúc khác, phải viết lớp ánh xạ. **Không có nó thì `tasklink_findgoods.txt` (526 dòng — bảng lớn nhất) vô dụng.**

### Nút 3 — `tbAwardTemplet` bên JX1 đang RỖNG RUỘT ⚠️ NGUY HIỂM NHẤT
`scriptjx2/lib/awardtemplet.lua` đã có nhưng **không Include awardtype nào**, và 11 file `awardtype/*.lua` lại cần `NewItemEx` / `CallPlayerFunction` / `curpack` / `usepack` — **đều 0 hit** bên JX1.

> Gọi `GiveAwardByList(...)` sẽ **chạy hết mà KHÔNG TRAO GÌ và KHÔNG BÁO LỖI**. Người chơi mất trắng phần thưởng, log vẫn sạch. Đây là kiểu hỏng tệ nhất — im lặng.

### Nút 4 — `IncludeLib` bên JX1 chỉ biết 20 module
Bảng tại `ScriptFuns.cpp:1978`:
```
TONG FILE LOG STRING BASIC COMMON SAY PLAYER AWARD TIMERLIST
TOPLIST MAPDB GB_TASK FILESYS SETTING BATTLE RELAYLADDER TITLE LEAGUE PARTNER
```
Bản Linux còn gọi các module **KHÔNG có tên trong bảng này**: `ITEM`, `TASKSYS`, `TIMER`, `NPCINFO`, `SPREADER`, `STATINFO`, `ACHIEVEMENT`, `DICEITEM`, `PET`, `WATCH`, `CONTROLTONG`.
Một số module **có tên nhưng trỏ vào `scriptjx2/lib/noop.lua` RỖNG**: `SETTING`, `BATTLE`, `LEAGUE`, `PARTNER`, `RELAYLADDER`, `TITLE`.

**Riêng lõi Dã Tẩu chỉ gọi `FILESYS`** — module này **có** trong bảng. Nhưng nếu kéo theo `seasonnpc.lua` với `Include` của nó (`event/storm/function.lua`, `activitysys/g_activity.lua`, `tong/tong_award_head.lua`) thì phải rà lại từng module.

### Nút 5 — `usepack` / `curpack` (chỉ vướng nếu kéo theo khung activitysys)
Máy ảo Linux có 3 hàm KingSoft thêm: `usepack`, `curpack`, `setbreakpointhook`. JX1 **không có**.
- Bản chất: Linux = **1 Lua_State + N bảng global chuyển đổi được ("pack")**. JX1 = **N Lua_State, mỗi file 1 state, không đổi được**.
- 247 điểm gọi / 144 file trên toàn cây Linux; chỉ 8 điểm có bảo vệ `if usepack then`.
- **Lõi `tasklink` không dùng `usepack`.** Nó chỉ vướng nếu anh kéo theo cả `script/activitysys/`.
- Nếu phải làm: stub `curpack()→0` và `usepack(n)→0` **chạy được** vì JX1 vốn đã mỗi file một state — nhưng chỗ nào dùng `usepack` để **đọc global của file khác** thì stub sẽ trả nil.

### Nút 6 — Trần số lượng script
`MAX_SCRIPT_IN_SET = 5000` (`Core/Src/KSortScript.h:13`), mảng `KLuaScript g_ScriptSet[5000]` — **mỗi slot mở một `lua_open(100)`**.

| Cây | Số file .lua |
|---|---|
| JX1 `script/` hiện tại | 2.533 |
| JX1 `scriptjx2/` | 189 |
| **Đang dùng** | **2.722** — còn 2.278 slot |
| Linux `script/` toàn bộ | **5.059** |
| Nếu chép cả cây | 7.781 ⇒ **VƯỢT TRẦN 55%** |
| Nếu chỉ chép bao đóng Dã Tẩu | **≈56 file** ⇒ thừa sức |

> Đây chính là chỗ phải làm rõ ý "**full script bản gốc**". Xem mục 6.

### Nút 7 — Mã hoá ký tự
Script Linux là hỗn hợp **GBK** (chú thích chữ Hán) và **TCVN3** (chuỗi tiếng Việt hiển thị). Máy chủ JX1 đọc script theo ANSI/TCVN3.
- Chuỗi tiếng Việt đã là TCVN3 ⇒ **hiển thị đúng, không phải làm gì**.
- Chú thích chữ Hán ⇒ hiện thành ký tự lạ trong editor nhưng **vô hại** (là comment).
- 🔴 **Quy tắc cứng:** tuyệt đối không dùng Edit/Write thường lên các file này — sẽ phá sạch byte cao. Phải dùng `safe_edit.py`.

---

## 6. TRẢ LỜI THẲNG: "100% BẢN GỐC" CÓ KHẢ THI KHÔNG?

Câu trả lời phụ thuộc anh hiểu "100%" theo nghĩa nào. Có ba nghĩa, khả thi rất khác nhau:

### Nghĩa 1 — "Chép cả cây script 5.059 file của bản Linux vào JX1"
❌ **KHÔNG KHẢ THI.** Vượt trần `MAX_SCRIPT_IN_SET 5000`; nâng trần thì phải build lại Core và đo lại RAM (mỗi slot 1 `lua_State`). Ngoài ra JX1 **chạy top-level mọi file .lua lúc boot** (`KSortScript.cpp:56`), mà cây Linux có rất nhiều file header/config chỉ để `Include` — chạy độc lập sẽ nổ. Đây cũng chính là lý do đợt Bang Hội trước phải giấu script JX2 ra ngoài thư mục `script/`.

### Nghĩa 2 — "Dùng nguyên văn, không sửa một chữ, toàn bộ script của TÍNH NĂNG Dã Tẩu"
✅ **KHẢ THI** — và đây gần như chắc chắn là điều anh muốn.
- Bao đóng ≈ **56 file .lua + 21 bảng .txt** (~130 KB dữ liệu).
- **0 dòng cú pháp phải sửa** (Lua 4.0 cả hai bên).
- **Số task giữ nguyên** (dải 1020–1046 trống bên JX1).
- Toàn bộ công việc dồn vào **C++**: viết ≈15 hàm để script gốc gọi được.
- **Đúng tinh thần yêu cầu của anh**: "tận dụng hàm sẵn có của mã nguồn tôi nhưng hệ thống script của Linux gốc".

**Hai chỗ buộc phải chạm vào script, không tránh được:**
1. `seasonnpc.lua` dòng 119: bỏ `do return end`. — Đây là **khôi phục bản gốc**, không phải sửa đổi: đó là dòng VNG **thêm vào** năm 2014 để tắt tính năng.
2. Đường dẫn `Include` nếu anh đặt script ở `scriptjx2/` thay vì `script/`. Tránh được bằng cách giữ đúng cấu trúc thư mục gốc.

### Nghĩa 3 — "Hành vi trong game giống hệt bản Linux 100%"
⚠️ **KHẢ THI CÓ ĐIỀU KIỆN.** Ba chỗ sẽ lệch nếu không xử lý:
1. **Cửa sổ chọn thưởng** (`Prise`) — dựng lại bằng hộp thoại JX1 sẽ **nhìn khác**, dù logic giống. Muốn giống hình thức phải sửa client.
2. **Hai lỗi sẵn có** (`TireReduce` gán đè 0, `CountDoubleMode` điều kiện luôn đúng) — giữ nguyên thì giống 100%; sửa thì lệch. **Phải là quyết định có ý thức.**
3. **UI theo dõi nhiệm vụ phía client**: `SyncTaskValue` bên JX1 là stub (`KTongJX2.cpp:3977` trả 1). Tin tốt: JX1 tự bắn `s2c_taskvalue` trong `SetSaveVal` ⇒ **có thể vô hại**, nhưng phải test thật.

---

## 7. PHẠM VI CÔNG VIỆC NẾU LÀM (ước lượng, chưa phải kế hoạch)

| Giai đoạn | Nội dung | Mức |
|---|---|---|
| **G1** | Chép bao đóng 56 file + 21 bảng vào JX1, giữ nguyên cấu trúc thư mục gốc. Boot thử, đọc `ScriptError.log` để đếm hàm nil thật (con số thực tế có thể khác 15) | Nhẹ |
| **G2** | Viết 15 hàm C++ vào `ScriptFuns.cpp`. Nhóm dễ (1–6) trước, `Prise` và `GetItemMagicAttrib` sau | **Nặng nhất** |
| **G3** | Viết lại lớp trao thưởng bằng `AddItem`/`AddItem2`/`AddTimeItem`/`AddExp`/`AddOwnExp` của JX1 — **không dùng `tbAwardTemplet` rỗng ruột** | Vừa |
| **G4** | Nối lối vào: đổi đường dẫn script trong 10 dòng `AddNpcNew` (template 108 giữ nguyên). Nối 2 item 205/212 vào `tasklink_goods.lua` bản Linux | Nhẹ |
| **G5** | Đối chiếu dữ liệu: 104 map trong `tasklink_findmaps.txt`, ID vật phẩm trong `award_basic.txt` (98 dòng) — cái nào JX1 không có phải thay | Vừa |
| **G6** | Quyết định số phận hệ Dã Tẩu JX1 cũ: tắt hẳn, hay để song song ở NPC khác | Quyết định của chủ game |
| **G7** | Test theo 6 loại nhiệm vụ × 5 khoảng cấp × 4 trạng thái course | Nặng |

---

## 8. NHỮNG BẪY PHẢI BIẾT TRƯỚC KHI GÕ MÃ

1. 🔴 **`tbAwardTemplet` rỗng ruột** — trao thưởng im lặng không trao gì. Xem Nút 3.
2. 🔴 **Task ID ≥ 3000 bị bỏ qua IM LẶNG** — không báo lỗi. (`scriptjx2` hiện đã có 1 chỗ dùng 5877 = đã chết im.)
3. 🔴 **Port nhầm `script/global/tasklink_head.lua`** (bản 2004) = lấy phải bản tràn số + lỗi logic loại 5.
4. 🔴 **`tl_countuplevelexp` được định nghĩa HAI LẦN** — một ở `task_addplayerexp.lua` (3 tham số), một ở chính `tasklink_head.lua:733` (2 tham số). Bản trong head thắng. `tl_checktask` gọi 3 tham số ⇒ tham số 3 bị nuốt (Lua 4 không báo lỗi). Vẫn đúng vì bản head tự gọi `ST_GetTransLifeCount()` bên trong — **nhưng rất dễ sửa nhầm file**.
5. 🔴 **`g_GetScript` không tự nạp** (`KSortScript.cpp:60`) — file .lua mới phải nằm trong cây được quét lúc boot, nếu không trả NULL im lặng.
6. 🔴 **`dataulenhbai.lua` có vòng lặp `AddItem`/`RemoveItemByIndex` tới 1000 lần** để ép option — rất nặng nếu bật lại.
7. ⚠️ **Trần 40 lần/ngày nằm ở `seasonnpc.lua::checkTask_Limit`**, KHÔNG nằm trong `tasklink_head`. Tìm nhầm chỗ sẽ không thấy.
8. ⚠️ **`HaveMagic` nhánh tham số CHUỖI không push giá trị mà vẫn `return 1`** (`ScriptFuns.cpp:5490-5499`) — script Dã Tẩu gọi dạng chuỗi.
9. ⚠️ **`SetBit` chỉ OR, không xoá được bit khi truyền 0** (`ScriptFuns.cpp:110-115`).
10. ⚠️ **`Talk` đòi ≥ 3 tham số** (`ScriptFuns.cpp:1767`) — dạng `Talk(szMsg)` 1 tham số bị nuốt im lặng.
11. ⚠️ **`KItemList::CountCommonItem` không có wildcard −1** — mọi hàm đếm/trừ vật phẩm phải truyền `nature` chính xác.
12. ⚠️ **`GetLeadLevel` và `GetServerName` bị đăng ký 2 lần**; `GetLeadLevel` trỏ tới hai hàm C khác nhau (`ScriptFuns.cpp:11203` và `:8127`) — bản sau ghi đè bản trước.
13. ⚠️ **`LuaDropItem` có lỗi sẵn**: `ScriptFuns.cpp:3516` gán `nMagicLevel` = tham số 9 **sau khi** đã tính xong mảng ⇒ tham số 9 vô tác dụng.
14. ⚠️ **`LuaSetItemParam` (`:4747`) không gọi `SyncItem`** trong khi `LuaSetParamItem` (`:5231`) có gọi.
15. 🔴 **Mã hoá**: dùng `safe_edit.py`, không dùng Edit/Write thường lên file gốc.

---

## 9. GHI CHÚ VỀ ĐỘ TIN CẬY CỦA BÁO CÁO NÀY

- Chạy 15 tác nhân phân tích, **11 hoàn thành**, 4 hỏng: 3 tác nhân phản biện chết vì **hết hạn mức phiên**, 1 chết vì lỗi schema (khoá JSON có dấu tiếng Việt).
- Vì vòng phản biện tự động không chạy, **tôi tự kiểm chứng thủ công 8 khẳng định quan trọng nhất**, mỗi cái đều có lệnh cụ thể:
  1. ✅ Lua 4.0 hai bên — dump byte binary + đọc `lua.h:50`
  2. ✅ `do return end` ở `seasonnpc.lua:119` — `sed -n '110,130p'` + `cat -A`
  3. ✅ `vng_script` chưa từng nạp — grep log boot 140.403 dòng
  4. ✅ `MAX_TASK = 3000` — đọc `KPlayerTask.h:13`
  5. ✅ Bảng `IncludeLib` 20 module — đọc `ScriptFuns.cpp:1974-1994`
  6. ✅ `MAX_SCRIPT_IN_SET 5000` vs 5.059 file — đọc `KSortScript.h:13` + đếm file
  7. ✅ 21/29 hàm lõi có sẵn — grep từng tên trong `ScriptFuns.cpp`
  8. ✅ `SyncTaskValue` là stub — đọc `KTongJX2.cpp:3977`
- **Hai khẳng định chưa được kiểm chứng độc lập** (nên coi là cần xác minh khi bắt tay làm):
  - "Toàn bộ 21 bảng `settings/task/*.txt` tồn tại thật" — do 1 tác nhân báo, tôi chưa tự đếm từng file.
  - Con số "15 hàm C++ thiếu" dựa trên quét tĩnh. **Con số thật sẽ lộ ra ở G1** khi boot thử và đọc `ScriptError.log` — đó mới là phép đo đáng tin nhất.

---

## PHỤ LỤC A — ĐƯỜNG DẪN TUYỆT ĐỐI

**Lõi (bắt buộc):**
```
D:\ServerLinux\server1\script\task\newtask\tasklink\tasklink_head.lua     35.866 B  BẢN THẬT
D:\ServerLinux\server1\script\task\newtask\tasklink\tasklink_award.lua    23.269 B
D:\ServerLinux\server1\script\global\seasonnpc.lua                        37.199 B  điều phối
D:\ServerLinux\server1\script\task\newtask\newtask_head.lua                1.851 B  nt_getTask/nt_setTask
D:\ServerLinux\server1\script\task\task_addplayerexp.lua                   3.175 B
D:\ServerLinux\server1\script\item\tasklink_goods.lua                      2.131 B  nhặt Địa Đồ Chỉ
D:\ServerLinux\server1\script\item\tasklink_goods_secret.lua               2.145 B  nhặt Mật Chỉ
D:\ServerLinux\server1\script\task\newtask\map_index.lua                  10.700 B
D:\ServerLinux\server1\script\task\newtask\lib_setmembertask.lua           1.451 B
D:\ServerLinux\server1\script\global\fuyuan.lua                            6.588 B
```

**KHÔNG lấy:**
```
D:\ServerLinux\server1\script\global\tasklink_head.lua      31.424 B  ← BẢN 2004 CHẾT, có lỗi
D:\ServerLinux\server1\vng_script\features\quest_daily\*    ← hệ khác, chưa từng chạy
D:\ServerLinux\server1\script\global\mel\lenhbaidatau.lua   ← mồ côi, dùng task 8000 vượt trần
```

**Bảng dữ liệu — `D:\ServerLinux\server1\settings\task\`:**
```
levellink.txt · tasklink_mainlink.txt
tasklink_buygoods.txt (35)   tasklink_findgoods.txt (526)  tasklink_showgoods.txt (45)
tasklink_findmaps.txt (208)  tasklink_upground.txt (45)    tasklink_worldmaps.txt (11)
award_basic.txt (98)         award_link.txt                award_loop.txt
talk\talk_{buygoods,findgoods,showgoods,findmaps,upground,worldmap}.txt
```

**Phía JX1:**
```
D:\GAMEDEVNEW\Sources\Core\Src\ScriptFuns.cpp                 bảng đăng ký hàm Lua (362 KB)
D:\GAMEDEVNEW\Sources\Core\Src\KPlayerTask.h:13               MAX_TASK 3000
D:\GAMEDEVNEW\Sources\Core\Src\KSortScript.h:13               MAX_SCRIPT_IN_SET 5000
D:\GAMEDEVNEW\Sources\Core\Src\GameDataDef.h:75               TASKVALUE_DANHQUAIDATAU 89
D:\GAMEDEVNEW\Sources\Core\Src\KNpc.cpp:1525                  hook giết quái Dã Tẩu (hardcode)
D:\GAMEDEVNEW\Sources\Core\Src\KPlayer.cpp:4748,4758          hook nhặt đồ (hardcode, đang NO-OP)
E:\...\TESTLOFFF_ONLINE\bin\server\script\global\npcchucnang\datau.lua   Dã Tẩu JX1 ĐANG CHẠY
E:\...\TESTLOFFF_ONLINE\bin\server\script\lib\lib_task.lua              bảng task ID SỐNG của JX1
E:\...\TESTLOFFF_ONLINE\bin\server\scriptjx2\lib\                       thư viện khung JX2 đã port
```

## PHỤ LỤC B — SO SÁNH NHANH HAI HỆ

| Tiêu chí | JX1 hiện tại | Linux gốc (tasklink) | Nhận xét |
|---|---|---|---|
| Số loại NV | 8 | 6 | JX1 nhiều hơn, có đánh quái |
| Chiều sâu | Phẳng, 40 NV/ngày | 20/link × 20 link × 20 vòng = 8.000 NV | **Linux sâu hơn hẳn** |
| Chọn NV | `random(1,9)` | Roulette trọng số theo link | **Linux tinh vi hơn** |
| Thoại NPC | Cố định | 6 bảng thoại ngẫu nhiên | **Linux phong phú hơn** |
| Chọn thưởng | UI cứng trong client (8 kiểu) | 3 ô ngẫu nhiên khác loại | Khác hẳn về chất |
| Chống hack | Không | 4 lớp | **Linux hơn hẳn** |
| Cống hiến bang | Không | Có (`/8`) | Linux có |
| Phụ thuộc client | **Cao** (`UiFinishQuest.cpp`) | **Thấp** (thuần script) | **Linux dễ bảo trì hơn** |

---

*Tài liệu này chỉ phân tích. Không có file nguồn nào bị sửa đổi trong quá trình khảo sát.*
