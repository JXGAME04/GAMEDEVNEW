# 13 — BỔ SUNG VÒNG 2: PHÍA CLIENT của 3 hoạt động

> Hướng quét 4. Vòng 1 (báo cáo `00_`…`09_`) **không đụng tới phía client** — grep
> `client|ui3_1024|ui3_800|spr|\.pak|package\.ini|taskui` trên 3 tệp ghi chú điều phối chỉ
> ra **5 dòng**, và không dòng nào nói về `ui\`, `spr\`, hay `.pak` của client.
> Tài liệu này lấp chỗ đó. **Chỉ phân tích — không sửa gì.**

Ngày: 24/08. Mọi khẳng định kèm `tệp:dòng` hoặc địa chỉ ELF/offset PE.

---

## 0. TÓM TẮT ĐIỀU HÀNH (đọc trước)

**Trả lời câu hỏi 5 (“có phải đụng client không?”):**

| Tính năng | Có cần sửa client? | Mức độ |
|---|---|---|
| **Săn boss sát thủ** | **CÓ** — nhưng chỉ 1 mục trên bảng Chỉ nam nhiệm vụ (F11) | **NHỎ** (~1 hàm C++ + 1 dòng ini) |
| **Phong Lăng Độ** | **KHÔNG** (bắt buộc) — chỉ cần nếu muốn có thanh tiến trình | **KHÔNG / hoặc TRUNG BÌNH nếu làm progress bar** |
| **Vượt ải** | ~~ĐÃ LÀM RỒI~~ → **KHÔNG** (bắt buộc). Bản Linux **không có mục F11 nào** cho `missions\challengeoftime` (nó dùng task **1505**, không phải 1201-1204 của Tín Sứ) | **KHÔNG** **[đã sửa theo đối chất]** |

**Ba phát hiện quan trọng nhất vòng này:**

1. ~~🔴 HAI ảnh NPC bị thiếu hẳn trong client JX1 — `boss020` và `passerby043`~~
   → **BỊ BÁC BỎ.** Cả **54/54** `NpcResType` đều **CÓ ĐỦ** ảnh trong `spr.pak`.
   Tên tệp thật không theo khuôn `<res>_<hậu tố>.spr` mà lấy từ
   `bin\client\settings\NpcRes\npc_normal_res_file.txt` (tên tệp) +
   `npc_res_kind_file_name.txt` (thư mục):
   `passerby043` → `\spr\npcres\passerby\passerby043\passerby043z.spr` (`spr.pak` 294.126 B);
   `boss020` → `\spr\npcres\boss\boss020\enemy216_*.spr` (`spr.pak`, đủ 6 tệp).
   **KHÔNG có chặn tiến độ nào ở phía ảnh NPC** (mục 7). **[đã sửa theo đối chất]**
2. 🟢 **Toàn bộ dữ liệu bản đồ phía CLIENT đã đủ 100 %** — kiểm bằng băm
   `KPakList::FileNameToId` trên **37 pak (32 được `package.ini` khai + có thật)** của `bin\client\data`: `.wor` + `Region_C.dat`
   + ảnh minimap `<tên>24.jpg` đều có cho cả 5 cây thư mục bản đồ (mục 6).
   Vòng 1 chỉ kiểm `Region_S.dat` (phía máy chủ) — đây là phần bổ sung.
3. 🟢 **`Patch\ui\taskui_killer.lua` + `Patch\settings\task\taskguide.txt` là mã CLIENT
   của Săn boss sát thủ** mà vòng 1 chưa biết; và **JX1 đã thay toàn bộ cơ chế đó bằng
   C++ `KUiTaskGuide`** ⇒ port không phải chép Lua, mà thêm 1 hàm `BuildSatThuText()`
   theo đúng khuôn `BuildTinSuText()` đã có (mục 5.1).

**Điều KHÔNG phải làm (tránh đi lạc):** 3 cửa sổ `通缉*.ini` (“truy nã”) + 3 lớp C++
`KUiKillerAllTask` / `KUiKillerCreateTask` / `KUiKillerOwnTask` + `settings\killer.ini`
**KHÔNG thuộc “săn boss sát thủ”** — chúng là hệ **treo thưởng truy sát NGƯỜI CHƠI**
(mục 3.4). Vòng 1 đã kết luận đúng về `killer.ini` (`05_dulieu.md` §5.1); tài liệu này
bổ sung phần giao diện đi kèm để không ai chép nhầm.

---

## 1. `D:\ServerLinux\Patch` là **thư mục cập nhật CLIENT**, không phải “lớp vá máy chủ”

`00_ghichu_dieuphoi.md:40-41` viết Patch “đúng một nửa là client”. Bằng chứng cứng hơn:
thư mục gốc `Patch\` chứa **binary client**:

| Tệp | Bằng chứng |
|---|---|
| `game_y.exe` (1.083.450 B, 08/07/2022) | tên tiến trình client bản Linux/CN |
| `game_y_unpacked.bin` (22.551.940 B) | bản đã giải nén của chính exe đó (do phiên trước tạo) |
| `engine.dll`, `represent2.dll`, `represent3.dll`, `lualibdll.dll` | 4 DLL đồ hoạ/kịch bản của client |
| `config.ini` | `[Client] Represent=2 / Resolution=1024x768 / PaintFPS=30` |
| `package.ini` | khai 31 pak: `spr.pak`, `ui.pak`, `script.pak`, `maps.pak`, `settings.pak`… |
| `vauto.exe`, `vaook.dll` | auto ngoài của bản Việt |

⇒ ~~`Patch\settings` là bản settings mà CLIENT đọc, không phải bảng riêng của máy chủ~~
→ **SỬA: `Patch\` là lớp cập nhật TRỘN CẢ HAI PHÍA.** `Patch\settings\droprate\` là bảng
**MÁY CHỦ**: chuỗi `droprate` xuất hiện **8 lần trong `server1\jx_linux_y`** (ELF máy chủ) và
**0 lần trong `Patch\game_y_unpacked.bin`** (exe client). Chính `Patch\config.ini` cũng có khối
`[Server] DenialPort=5623`. Vẫn đúng rằng 467 tệp “chỉ có ở B” **không** hàm ý “thiếu” —
nhưng **phải xét từng bảng**, không suy ra cả thư mục là của client. **[đã sửa theo đối chất]**

### 1.1 ⚠️ Đính chính một ví dụ trong đề bài

Đề bài (và phần đầu `08_hai_goc_dulieu.md`) nêu `Patch\settings\droprate\boss\bosstask_lev20..90.ini`
là “chỉ có ở B”. **Kiểm lại bằng máy: SAI** — cả 8 tệp đều **có ở cả hai gốc**, cùng kích thước:

```
D:\ServerLinux\server1\settings\droprate\boss\bosstask_lev20.ini   6233 B  (CÓ)
D:\ServerLinux\server1\settings\killer.ini                          492 B  (CÓ)
D:\ServerLinux\server1\settings\trigger_challengeoftime.lua         704 B  (CÓ)
D:\ServerLinux\server1\settings\maps\challengeoftime\lineup8.txt     99 B  (CÓ)
```
Danh sách `chi_co_o_patch.txt` **không** liệt kê `bosstask_*`; 5 tệp liên quan 3 tính năng
thật sự chỉ-có-ở-B là (grep `chi_co_o_patch.txt`):
`settings\maps\great_night\风陵渡.txt:246`, `settings\maps\中原北区\渡船\渡船刷怪点.txt:287`,
`…\风陵渡北岸\baiyingyingboss.txt:288`, `…\yanxiaoqianboss.txt:289`,
`…\风陵渡南岸\herenwoboss.txt:290`.

ℹ️ Trong đó `settings\maps\great_night\风陵渡.txt` **KHÔNG thuộc Phong Lăng Độ** — `great_night`
là sự kiện Trung Thu (`server1\script\event\great_night\event.lua:2-15`), tệp này chỉ là toạ độ
đặt đèn lồng trên map 336. Đừng port nhầm. (Nó **đã có sẵn** ở JX1: `vlngaothe.pak`, 303 B,
và **loose** ở `bin\client\settings\maps\great_night\`.)

### 1.2 Bẫy tên tệp tiếng Trung (khác cách đề bài mô tả)

Trên đĩa Windows này, tên thư mục tiếng Trung được lưu ở NTFS **dưới dạng UTF-16 của chuỗi
đã hỏng** (byte GBK từng bị đọc theo latin-1/CP1252). Muốn khôi phục chữ Hán phải làm:

```python
ten_that = ten_tu_os_listdir.encode('latin-1').decode('gbk')
```

Nếu `os.fsencode` rồi `decode('gbk')` (cách trực giác) sẽ ra **mojibake lần hai**
(`帮会` → `掳茂禄谩`). Script dùng lại: `scratchpad\lsgbk.py`.

---

## 2. KIỂM KÊ TÀI NGUYÊN CLIENT TRONG `Patch\`

| Thư mục | Số tệp | Ghi chú |
|---|---|---|
| `ui\` | **443** | 26 tệp gốc + `ui3_1024\` **209** + `ui3_800\` **208** |
| `spr\` | **519** | `vng` 331, `ui3` 72, `ui4` 36, `ctc_ui` 31, `npcres` 20, `icon` 16, `item` 9, `vongsang` 3, `skill` 1 |
| `script\` | **634** | script CLIENT (phần lớn là `skill\`) |
| `settings\` | **1754** | bảng dữ liệu client |
| `maps\` | **52** | `*.map` / `*.dat` (bảng chướng ngại nhỏ, không phải đồ hoạ) |
| `data\` | **1** | **chỉ có `slistcache.pak`** — 30 pak còn lại khai trong `package.ini` **KHÔNG có trong bản dump** |
| `music\` 36 · `users\` 49 | | |

🔴 **Hệ quả quan trọng:** vì `Patch\data` chỉ có 1 pak, **không thể đối chiếu trực tiếp ảnh
`.spr` gốc của bản Linux**. Mọi kết luận về ảnh dưới đây đều dựa trên **pak của client JX1**
(`E:\…\bin\client\data`, 37 pak (32 được `package.ini` khai + có thật)) — đó mới là nơi phải có ảnh khi port.

### 2.1 `ui\ui3_1024` — ~~175 cửa sổ + 16 thư mục con~~ → **157 tệp `.ini` + 15 thư mục con** **[đã sửa theo đối chất]** *(đếm lại: 157 tệp ở mức 1, 15 thư mục con, tổng cây 209 tệp)*

Không có cửa sổ nào mang tên Phong Lăng Độ / Vượt ải / phà / thuỷ tặc.
Chỉ **4 mục** dính tới 3 tính năng (và 3 trong đó là **nhầm lẫn**, xem §3.4):

```
ui3_1024\progressbar.ini            485 B   <- Phong Lăng Độ (gián tiếp)
ui3_1024\uitaskguide\taskguide.ini  3228 B  <- khung F11/F12 (dùng chung)
ui3_1024\uitaskguide\tasktrace.ini   932 B
ui3_1024\通缉接收任务主界面.ini      1627 B  <- KHÔNG PHẢI (hệ truy nã người chơi)
ui3_1024\通缉创建任务编辑框.ini      1664 B  <- KHÔNG PHẢI
ui3_1024\通缉查看任务编辑框.ini      1573 B  <- KHÔNG PHẢI
ui3_1024\uiactivityguide\*.ini      5 tệp   <- bảng “Lịch trình hoạt động” (activitysys)
```

### 2.2 `settings\` nào là dữ liệu client — danh sách **rút từ chính exe client**

Quét chuỗi trong `Patch\game_y_unpacked.bin` (script `scratchpad\settpaths.py`) cho ra
**đúng** danh sách đường dẫn `\settings\…` mà client mở. Trích các mục liên quan:

| Đường dẫn client đọc | Liên quan 3 tính năng |
|---|---|
| `\settings\NpcS.txt` | **CÓ** — client đọc `npcs.txt` để biết `NpcResType` của mọi NPC |
| `\settings\npcres\`, `\settings\npcres_simple\` | **CÓ** — bảng hoạt ảnh |
| `\settings\maplist.ini` | **CÓ** — tên + đường dẫn 78 map |
| `\settings\portrait.ini` | không (3 tính năng không NPC nào có `HeadImage`) |
| `\settings\task\task_id.txt`, `task_type.txt`, `player_task_def.txt` | phụ (sổ tay nhiệm vụ) |
| `\settings\killer.ini` | **KHÔNG** (hệ truy nã người chơi) |
| `\settings\magicdesc.ini`, `\settings\Skills.txt`, `\settings\Missles.txt` | phụ (mô tả chiêu boss) |
| `\settings\item\…` (10 bảng) | phụ (icon/độ bền vật phẩm rơi) |

🔴 **Luật cần nhớ (vòng 1 chưa nêu):** `npcs.txt` phải **cập nhật cả hai bên**.
Ở JX1 hiện có **hai bản riêng biệt**, cùng 2036 dòng, chỉ lệch 11 dòng do dấu nháy CSV:

```
E:\…\bin\server\settings\npcs.txt   514.004 B   2036 dòng
E:\…\bin\client\settings\npcs.txt   514.002 B   2036 dòng
so cột NpcResType + Kind trên 2035 dòng: LỆCH 0
```
Nguồn hằng số: `Sources\Core\Src\CoreUseNameDef.h:119`
`#define NPC_SETTING_FILE "\\settings\\NpcS.txt"`.

---

## 3. TÀI NGUYÊN CLIENT CỦA TỪNG TÍNH NĂNG

### 3.1 Săn boss sát thủ — **có mã client thật**

Vòng 1 nói tính năng nằm ở `script\task\tollgate\killer\`. Còn thiếu **3 mảnh phía client**:

**(a) `Patch\settings\task\taskguide.txt:9`** — bảng điều phối bảng Chỉ nam nhiệm vụ:

```
ID  TaskName              IsQuitable  TaskGuideScriptFile      ShowFullInfoFunc
8   Nhiệm vụ Sát thủ      0           \UI\taskui_killer.lua    showkillertaskdesc
9   Thách thức thời gian  0           \UI\taskui_messenger.lua showmessengerdesc
```

**(b) `Patch\ui\taskui_killer.lua`** (1027 B, TCVN3) — toàn văn 28 dòng, logic:

```lua
:4   KILLER = "\\settings\\task\\tollgate\\killer\\killer.txt"
:8   local Uworld1082 = GetTask(1082)
:18  if (TabFile_Load(KILLER, KILLER) == 0) then ... end
:22  if (Uworld1082 == 0)  -> in danh sách 7 thành có Nhiếp Thí Trần
:25  if (Uworld1082 >  0)  -> in TabFile_GetCell(KILLER, Uworld1082+1, "BossName")
                                .. TabFile_GetCell(KILLER, Uworld1082+1, "BossInfo")
```

⇒ Khẳng định (mới): **client đọc trực tiếp `killer.txt` bằng `TabFile_*`**, và
**chỉ số dòng = `GetTask(1082) + 1`** (bù dòng tiêu đề). Đây là đặc tả chính xác cho
`BuildSatThuText()` sắp viết ở JX1.

**(c) `settings\task\tollgate\killer\killer.txt`** — phải có ở **cả** client lẫn máy chủ.
Cả hai gốc Linux đều có, cùng **14.033 B**.

### 3.2 Vượt ải — client của nó **đã được port rồi**, dưới tên “Tín Sứ”

`Patch\ui\taskui_messenger.lua:21` mở đầu bằng chú thích chữ Hán:

```lua
function showmessengerdesc(nTaskGenre) --闯关任务之信使任务
```
= “**nhiệm vụ Tín Sứ THUỘC nhiệm vụ Vượt ải (闯关)**”. Thân hàm đọc `GetTask(1201..1204)`
(`:22-25`) và nhánh `Uworld1203 == 10 / 20 / 21 / 25 / 30` (`:79-104`) — đúng bằng 5 nhánh
`case 10/20/21/25/30` trong `Sources\S3Client\Ui\UiCase\UiTaskGuide.cpp:531-552`
(`KUiTaskGuide::BuildTinSuText()`, `:513`).

⇒ ~~**Kết luận:** mục “Nhiệm vụ Tín Sứ” (TaskId 7) … chính là giao diện client của Vượt ải~~
→ **SAI, ĐÃ BÁC BỎ.** `taskui_messenger.lua` là giao diện của **nhiệm vụ Tín Sứ / Thiên Bảo Khố**
nằm ở `script\task\tollgate\messenger\` — task 1201-1204 **chỉ** được ghi ở cây đó
(`messenger_turerukou.lua:155`, `qianbaoku\messenger_turenpc.lua:99`, `posthouse.lua:514`…;
grep `SetTask(1203` trên toàn `server1\script` = **0 hit ngoài `task\tollgate\messenger`**).
Còn **Vượt ải theo đề bài = `script\missions\challengeoftime\`** dùng task **1505**
(`missions\challengeoftime\mission_match.lua:51` `SetTask(1505, 0)` và `:80` `SetTask(1505, 1)`),
đúng như vòng 1 đã ghi ở `03_vuotai.md:865`. **Không có dòng nào của 1505 trong `Patch\ui\*.lua`**
⇒ **Vượt ải KHÔNG có mục F11 nào ở bản Linux**, giống hệt Phong Lăng Độ.
Chú thích `--闯关任务之信使任务` nói “nhiệm vụ Tín Sứ **thuộc họ** 闯关 (= `task\tollgate`)”,
chứ không phải `missions\challengeoftime`. **Tên “Tín Sứ” ở JX1 đang ĐÚNG — đừng đổi nhãn.** **[đã sửa theo đối chất]**

### 3.3 Phong Lăng Độ — **không có mục nào trên bảng nhiệm vụ**

`taskguide.txt` 18 dòng, **không có** dòng nào cho phà / Phong Lăng Độ.
Toàn bộ giao tiếp với người chơi đi qua `Msg2Player` / `Msg2Team` (đếm trong bao đóng
đã giải mã `src_utf8\`: **180 lần `Msg2Player`, 13 lần `Msg2Team`**, dùng kênh chat sẵn có).

Vết client duy nhất: `OpenProgressBar` — xem §4.2.

### 3.4 🔴 CẢNH BÁO: `通缉*` / `KUiKiller*` / `killer.ini` **KHÔNG PHẢI** tính năng của ta

Chuỗi trong `Patch\game_y_unpacked.bin`:

```
0x002E76DC  .?AVKUiKillerAllTask@@
0x002E7700  .?AVKUiKillerCreateTask@@
0x002E7744  .?AVKUiKillerOwnTask@@
0x002A1118  通缉接收任务主界面.ini      (cùng khối RTTI/khởi tạo với 3 lớp trên)
0x002A1214  通缉创建任务编辑框.ini
0x002A1288  通缉查看任务编辑框.ini
0x002A9DC4  \settings\killer.ini
0x002B446C  WorldKillerAppear
0x002A8498  G_UI_KILLER_NO_CANCEL
0x002A84B0  G_UI_KILLER_NO_TASK
```
Và `Patch\settings\killer.ini:1` ghi rõ: `;通缉追杀系统专用ini` (“ini riêng của hệ **truy nã
truy sát**”), các khoá `MoneyPerHour / MinTargetLevel=50 / MinReward=100000` và thông báo
`"Đối tượng dưới cấp 50 không thể truy nã!"`, `"Nhân vật mục tiêu không trực tuyến…"`.
⇒ Đây là **treo thưởng giết NGƯỜI CHƠI khác**, trùng tên tiếng Anh “killer” là ngẫu nhiên
(đúng như `01_satthu.md:817` đã cảnh báo). **Không chép 3 cửa sổ này.**

---

## 4. GÓI TIN — 3 tính năng có cần giao thức riêng không?

### 4.1 Kết luận: **KHÔNG** cần thêm gói tin mới cho cả 3

Bằng chứng quét chuỗi `D:\ServerLinux\server1\jx_linux_y` (ELF32 i386, không làm rối):

```
0x00209D3D  \settings\killer.ini            <- hệ truy nã
0x0020B349  C_KillerTask / 0x0020B356 C_Killer
0x00213C74  AskRelayForKillerStatus         <- hệ truy nã (đi qua S3Relay)
0x002185D5  WorldKillerAppear               <- hệ truy nã
0x0021CE8D  G_STR_FENGLINGQI / 0x00222AD8 ITEM_FENGLINGQI
```
🔴 Đừng nhầm: `FENGLINGQI` = **风铃泣** (một vật phẩm), **không phải** `FENGLINGDU` (风陵渡).
Quét theo byte GBK trong exe client: `闯关`(B4B3B9D8) **0 hit**, `风陵渡`(B7E7C1EAB6C9) **0 hit**,
`渡船`(B6C9B4AC) **0 hit**, `杀手`(C9B1CAD6) **0 hit**, `通缉`(CDA8BCA9) **3 hit** (đúng 3 tên
tệp .ini nêu ở §3.4). ⇒ **Không có mã lệnh/tên gói tin riêng cho 3 hoạt động.**

Ba tính năng chỉ dùng các đường sẵn có:
* thoại NPC / hộp thoại → đường `SCRIPTACTION` chuẩn;
* thông báo → `Msg2Player` / `Msg2Team`;
* trạng thái nhiệm vụ hiện trên F11 → **đồng bộ task value**, JX1 **đã có sẵn**:
  `Sources\Core\Src\KPlayerTask.cpp:76-85` — `KPlayerTask::SetSaveVal()` gọi
  `SyncTaskValueToClient(nNo, bFlag)` cho **MỌI** task id, không lọc;
  `Core\Src\KPlayerTask.cpp:97-125` chọn kênh: id `0..255` → gói cũ `s2c_taskvalue`;
  id `>= 256` → `PLAYER_SCRIPTACTION_SYNC` + `UI_TASKVALUE`;
  client nhận ở `Core\Src\KPlayer.cpp:8216-8233`, đọc lại ở `Core\Src\CoreShell.cpp:836`
  (`GDI_TASK_SAVE_VALUE`).
  ⇒ **task 1082 / 1192 / 1193 / 1217 và 1201-1218 tự động xuống client, không cần code gói tin.**

### 4.2 Ngoại lệ duy nhất: `OpenProgressBar` (thanh tiến trình) — của Phong Lăng Độ

| | |
|---|---|
| Hàm Lua máy chủ | `OpenProgressBar` @ **`0x081082D0`** (`04_api_gap.md` mục 28 đã ghi) |
| **3 hàm anh em vòng 1 BỎ SÓT** (đúng: `04_api_gap.md` chỉ có `OpenProgressBar`) | ⚠️ các số `0x00214C20 / 0x00214C34 / 0x00214C4A` là **offset CHUỖI trong .rodata**, KHÔNG phải địa chỉ hàm. Địa chỉ **MÃ** đã có sẵn trong `jx_linux_y.luamap.full.txt`: `0x080FBAF0 GetProgressBarMaskEvent`, `0x080FC410 ClearProgressBarEvent`, `0x080FC4D0 SetProgressBarEvent`, `0x081082D0 OpenProgressBar` **[đã sửa theo đối chất]** |
| Lớp hẹn giờ máy chủ | chuỗi ELF `0x00220EE0` = `17KProgressBarTimer` (mangling GCC: lớp `KProgressBarTimer`) |
| Lớp cửa sổ client | `game_y_unpacked.bin` `0x002E7F00` = `.?AVKUiProgressBar@@` |
| Tệp giao diện client | `game_y_unpacked.bin` `0x002A2D64` = `%s\progressbar.ini` |
| Bảng cấu hình | `settings\progressconfig.txt` (1336 B, 17 cột `EVENT_*`, 16 dòng dữ liệu) |

**Nhưng: 3 tính năng KHÔNG BAO GIỜ GỌI nó.** Grep toàn bộ bao đóng đã giải mã:

```
src_utf8\phonglangdo\lib\progressbar.lua:95   OpenProgressBar(...)   <- chỉ định nghĩa
src_utf8\phonglangdo\missions\fengling_ferry\boss.lua:7   Include("\\script\\lib\\progressbar.lua")
```
Không có lời gọi `tbProgressBar:Open` / `:Start` / `:OpenByConfig` nào trong `satthu`,
`phonglangdo`, `vuotai`.
🔴 **Nhưng vẫn có bẫy:** `progressbar.lua:123` chạy `tbProgressBar:InitEventType()` **ngay lúc
nạp tệp**, hàm này gọi `self:LoadConfig("\\settings\\progressconfig.txt")` (`:9`, `:68`).
Thiếu bảng đó ⇒ lỗi im lặng lúc `Include`.

**Trạng thái ở JX1:**
| Thành phần | JX1 có? | Bằng chứng |
|---|---|---|
| `settings\progressconfig.txt` (máy chủ) | ✅ **CÓ trong `bin\server\Pak\namcung.pak`** (1681 B nén) | băm `FileNameToId`, `package.ini:4 namcung.pak` — máy chủ **có** nạp pak này |
| `settings\progressconfig.txt` (client) | ✅ `data\slistcache.pak` (1201 B nén) — đã rút ra kiểm: header `Id/Title/Time/EVENT_MOVE/…` khớp bản Linux | |
| `\Ui\Ui3\progressbar.ini` | ✅ `data\slistcache.pak` (336 B nén) — rút ra đọc được `[Main] Width=165 Height=39 Image=\Spr\Ui3\progressbar\progressbar_back.spr Button0=Progress` | |
| Ảnh `\Spr\Ui3\progressbar\progressbar_back.spr` / `_bit.spr` | ✅ `data\updatejx06.pak` (7772 B / 2433 B) | |
| **Lớp C++ `KUiProgressBar`** | ❌ **KHÔNG CÓ** | `grep -ri progressbar Sources\**` chỉ trúng `Sword3PaySys\Toolkits\CppUnit15\…` (thư viện test, không liên quan) |
| **4 hàm Lua + `KProgressBarTimer`** | ❌ **KHÔNG CÓ** | `grep -i progress Core\Src\ScriptFuns.cpp` = 0 hit |
| `script\lib\progressbar.lua` | ⚠️ **CÓ SẴN, chỉ sai chỗ** | máy chủ: `spak.py` 0 hit, không loose — **nhưng bản y hệt đã nằm ở `bin\client\script\lib\progressbar.lua`** (3.436 B, md5 `954f2755…`, **trùng md5 với `D:\ServerLinux\server1\script\lib\progressbar.lua`**) ⇒ chỉ cần **chép sang `bin\server\script\lib\`**, không phải viết mới **[đã sửa theo đối chất]** |

⇒ **Khuyến nghị:** khi port Phong Lăng Độ, **stub 4 hàm** (`OpenProgressBar`,
`SetProgressBarEvent`, `ClearProgressBarEvent`, `GetProgressBarMaskEvent`) trả `0` là đủ
chạy — **art và .ini đã sẵn trong pak**, chỉ thiếu code. Làm đầy đủ là việc **tuỳ chọn**,
khối lượng trung bình (1 lớp cửa sổ + 1 lớp timer máy chủ + 1 gói tin mới).

---

## 5. ĐỐI CHIẾU PHÍA JX1 — CÁI GÌ ĐÃ CÓ, CÁI GÌ PHẢI THÊM

### 5.1 Bảng Chỉ nam nhiệm vụ (F11): JX1 dùng **C++**, không dùng Lua

| | Bản Linux | JX1 |
|---|---|---|
| Khung cửa sổ | C++ `KUiTaskGuide` (`game_y_unpacked.bin:0x002E8290`) | C++ `KUiTaskGuide` (`Sources\S3Client\Ui\UiCase\UiTaskGuide.cpp`, 23.538 B) |
| Danh mục | `ui\uitasklist.ini` (~~119+ mục~~ → **20 mục**: 18 TaskId + 2 tiêu đề nhóm; tệp 1.795 B) **[đã sửa theo đối chất]** | `bin\client\Ui\uitasklist.ini` (**4 mục**: TaskId 6/7/8/9) |
| Điều phối nội dung | bảng `settings\task\taskguide.txt` → tệp `\UI\taskui_*.lua` | **hardcode trong C++** (`UiTaskGuide.cpp:379-394`) |
| Tệp `taskguide.txt` | có (18 dòng) | ❌ **KHÔNG CÓ** — kiểm băm cả 37 pak (32 được `package.ini` khai + có thật) client + loose: `\settings\task\taskguide.txt` = *không thấy* |
| `\ui\taskui_killer.lua` | có | ✅ **CÓ trong pak** (`update01.pak` 1051 B, `update03.pak` 1051 B) — **nhưng vô dụng** vì không có `taskguide.txt` gọi nó |
| `\ui\taskui_messenger.lua` | có | ✅ có trong pak (`slistcache.pak` 5462 B, `update01.pak` 4150 B) — cũng vô dụng, đã được C++ thay |

Nội dung hiện tại `bin\client\Ui\uitasklist.ini:7-31`:
```
[0] Nhiệm vụ hàng ngày   TaskId=-1
[1] +Nhiệm vụ Dã Tẩu     TaskId=6
[2] +Nhiệm vụ Tín Sứ     TaskId=7      <- CHÍNH LÀ Vượt ải
[3] +Bang Chiến          TaskId=8
[4] +Bách Nhân Lôi đài   TaskId=9
```

**Việc phải làm cho Săn boss sát thủ (phía client) — đúng 4 chỗ:**

| # | Tệp | Việc |
|---|---|---|
| 1 | `bin\client\Ui\uitasklist.ini` | thêm `[5] Name=+Nhiệm vụ Sát thủ / TaskId=10` |
| 2 | `Sources\S3Client\Ui\UiCase\UiTaskGuide.h` | khai `void BuildSatThuText();` + `#define TASKGUIDE_SATTHU_TASKID 10` |
| 3 | `…\UiTaskGuide.cpp:379-394` | thêm nhánh `else if (pEntry->nTaskId == TASKGUIDE_SATTHU_TASKID) BuildSatThuText();`; và ở `:163-190` thêm `if (nTaskId == 1082 \|\| nTaskId == 1193) BuildSatThuText();` |
| 4 | `…\UiTaskGuideStr.h` | thêm chuỗi TCVN3 (khuôn có sẵn: `TS_NOTASK`, `TS_ROUTE_FMT`…) |

Thân `BuildSatThuText()` = dịch nguyên `taskui_killer.lua:22-27` sang C++:
đọc `DTG_TaskVal(1082)` (`UiTaskGuide.cpp:88-91`), nếu `> 0` thì `KTabFile` mở
`\settings\task\tollgate\killer\killer.txt`, lấy dòng `giá trị + 1`, cột `BossName` + `BossInfo`.

✅ **Bảng `killer.txt` đã sẵn ở CẢ HAI phía JX1** (vòng 1 đã ghi ở `05_dulieu.md:485`, ở đây
xác nhận lại bằng băm):
```
máy chủ : bin\server\Pak\namcung.pak          14042 B   (package.ini:4 -> có nạp)
client  : data\update01.pak 14042 B ; data\update03.pak 14033 B (= đúng cỡ bản Linux)
```

### 5.2 Bảng “Lịch trình hoạt động” (`uiactivityguide`) — **không cần đụng**

`Patch\settings\activitysys\activity.txt` chỉ có **22 dòng**, toàn sự kiện lễ tết
(Trung Thu, Giáng Sinh, WorldCup…), **không dòng nào** của 3 tính năng.
3 tính năng chỉ gọi `G_ACTIVITY:OnMessage(...)` (đường thành tựu/thống kê phía máy chủ).
JX1 **đã có** `\ui\ui3\uiactivityguide\activityguide.ini` trong `slistcache.pak` (1482 B)
và ảnh `\spr\ui3\activityguide\activityguideui_vn.spr` trong `updatejx10.pak` (72.797 B),
nhưng **không có** lớp C++ `KUiActivityGuide` (grep Sources = 0). Không phải việc của đợt này.

### 5.3 Script client (`script\`) — không thiếu gì thuộc 3 tính năng

So `Patch\script` (634 tệp) với `bin\client\script` (436 tệp): 305 trùng, **329 chỉ có ở Linux**.
Lọc theo tên liên quan boss/nhiệm vụ, chỉ còn:
`skill\biggoldboss.lua`, `skill\special\boss_libaiskill.lua`, `skill\special\qianbaoku.lua`,
`skill\special\spider_web_single.lua`.
**Kiểm chỗ gọi: tất cả đều nằm trong dòng ĐÃ COMMENT** — ví dụ 20 dòng
`src_utf8\satthu\task\tollgate\killbosshead.lua:2611-2630` đều mở đầu bằng `--`.
Còn `skill\npc\killerbossmianyi.lua` (chiêu miễn dịch của boss sát thủ) **JX1 client ĐÃ CÓ**
(`bin\client\script\skill\npc\killerbossmianyi.lua`). ⇒ **Không chặn.**

---

## 6. BẢN ĐỒ PHÍA CLIENT — ĐỦ 100 %

Vòng 1 (`05_dulieu.md` §2.2) đã kiểm `.wor` và `Region_S.dat` (định dạng **máy chủ**).
Vòng này kiểm **định dạng CLIENT**: `Region_C.dat` (`Sources\Core\Src\Scene\SceneDataDef.h:13`
`REGION_COMBIN_FILE_NAME_CLIENT "Region_C.dat"`), đường dẫn dựng ở
`Core\Src\Scene\KScenePlaceRegionC.cpp:79`
`sprintf(RegionPathPrefix, "%s\\v_%03d\\%03d_", pszBaseFolderName, y, x)`,
gốc lấy từ `Core\Src\Scene\KScenePlaceC.cpp:368` (`\maps\<MapList value>.wor`).

Băm `FileNameToId` trên **37 pak (32 được `package.ini` khai + có thật)** `bin\client\data` + đối chiếu bản loose `bin\client\maps2\`:

| Map id | Thư mục (GBK) | `.wor` | `Region_C.dat` (mẫu 8/8) | minimap `…24.jpg` |
|---|---|---|---|---|
| 336 | `中原北区\风陵渡` | `update01.pak` 111 B | ✅ `update01.pak` (857 khối loose) | ✅ 243.896 B |
| 337/338/339 | `中原北区\渡船` | `update01.pak` 113 B | ✅ `update01.pak` (21 khối) | ✅ 2.837 B |
| 340 | `西北北区\莫高窟` | `update01.pak` 113 B | ✅ `update01.pak` (1131 khối) | ✅ `update01`+`updatejx03` |
| 464–495 | `特殊用地\杀手的试炼` | `update01`+`update03` 113 B | ✅ `update03.pak` (52 khối) | ✅ 6.929 B |
| 957 | `特殊用地\安期炼丹洞` | `updatejx08.pak` 92 B | ✅ `updatejx08.pak` (28 khối) | ✅ 18.940 B |
| *(đối chứng)* 1 | `西北南区\凤翔` | `maps.pak` 284 B | ✅ `maps.pak` (1066 khối) | ✅ 519.552 B |

`bin\client\settings\MapList.ini` khai **1000 map**; kiểm 38 id (336-340, 464-495, 957):
**thiếu 0**. Ba id 337/338/339 dùng chung một cây thư mục; 32 id 464-495 cũng vậy — giống
hệt bản Linux.

ℹ️ `bin\client\maps2\` là **bản loose (đã rút khỏi pak)** của toàn bộ đồ hoạ bản đồ —
`grep maps2` trong `Game.exe` / `CoreClient.dll` / `engine.dll` = **0 hit**, và `package.ini`
không khai ⇒ **thư mục dàn dựng, game không đọc**. Dùng để đối chiếu thì tốt, đừng tưởng
là đường nạp thật.

⚠️ **Wart cấu hình đã tồn tại** (không do 3 tính năng, nhưng nên biết):
`bin\client\package.ini` khai 37 pak (32 được `package.ini` khai + có thật), trong đó **4 pak không có trên đĩa**:
`sprvlngaothe2.pak`, `settings.pak`, `ui.pak`, `script.pak`. Client bỏ qua im lặng và
rơi về thư mục loose `settings\`, `Ui\`, `script\` — đó là lý do các bản vá gần đây đặt
tệp loose. **Khi port, đặt tệp mới ở dạng loose là an toàn nhất.**

---

## 7. ~~🔴 CHẶN TIẾN ĐỘ THẬT: THIẾU 2 ẢNH NPC~~ → **KHÔNG THIẾU ẢNH NÀO** **[đã sửa theo đối chất]**

🔴 **TOÀN BỘ MỤC 7 ĐÃ BỊ BÁC BỎ — xem “ĐỐI CHẤT” ở cuối tệp.** **[đã sửa theo đối chất]**

~~Client vẽ NPC theo cột `NpcResType` của `npcs.txt` → `\spr\npcres\<loại>\<res>\<res>_*.spr`
(khuôn xác nhận ở `bin\client\settings\ImageNpcList.ini:32`)~~ — **SAI NGUỒN**: `ImageNpcList.ini`
(1.343 B, 53 dòng) chỉ được `S3Client\Ui\UiCase\UiMsgSel4.cpp:13` và `UiSayNew.cpp:14` dùng cho
**ảnh NPC trong hộp thoại**, liệt kê vỏn vẹn vài NPC (181/182/183…) — nó **không** là khuôn đường
dẫn sprite thế giới. Khuôn ĐÚNG: `Core\Src\CoreUseNameDef.h:11` `RES_INI_FILE_PATH "\settings\npcres"`
+ hai bảng `bin\client\settings\NpcRes\npc_res_kind_file_name.txt` (cột `ResFilePath` = thư mục)
và `npc_normal_res_file.txt` (14 cột = **tên tệp .spr thật**). Ví dụ:
`npc_normal_res_file.txt:263` `passerby043 → passerby043z.spr` (không có hậu tố `_st`);
`:523` `boss020 → enemy216_st/_wlk/_bat/_die/_at1/_at2.spr` (tên tệp **khác hẳn** tên res).

~~Lấy **54 `NpcResType`** của 140 NPC trong `npc_can_them.csv`, thử **11 hậu tố**~~
(⚠️ công cụ `chk_npcspr.py:23` thật ra chỉ thử **5**: `_wlk _st _stand _atk _die` — và không hậu tố nào trong số đó là bộ hậu tố thật) trên **37 pak (32 được `package.ini` khai + có thật) client**:

```
(kết quả CŨ, sai)  TỔNG: 54 res type — THIẾU 2: boss020 , passerby043
(kết quả ĐÚNG)     TỔNG: 54 res type — THIẾU 0        <- quét lại bằng 2 bảng NpcRes
``` **[đã sửa theo đối chất]**
Kiểm chéo: `boss019_wlk.spr` ✅ `spr.pak(426186)`, `passerby181_st.spr` ✅ `spr.pak(333231)`
⇒ hàm băm và đường dẫn đúng; hai res kia **thật sự không có trong bất kỳ pak nào**, và cũng
không có trong `bin\client\Spr\npcres\` loose (thư mục này chỉ có `boss186`, `boss188`,
`passerby440..443`).

**NPC bị ảnh hưởng** (đọc thẳng `bin\server\settings\npcs.txt`, dòng = id + 1):

| NpcId | Tên trong JX1 | Kind | Res | Hậu quả |
|---|---|---|---|---|
| **1684** | `Vượt ải_Người tiếp dẫn Mật Phòng` | **3 (đối thoại)** | `passerby043` | 🔴 **NPC vô hình** — đây là NPC **đối thoại DUY NHẤT** của Vượt ải (lọc `Kind=3` trên cả 141 dòng `npc_can_them.csv` chỉ ra 1 kết quả). Không thấy ⇒ **không vào được Mật Phòng (map 957)** |
| **1001** | ` (sơ cấp) tiểu Boss nam 8` | 0 | `boss020` | 🟡 boss vô hình ở nhánh sơ cấp Vượt ải |
| 1033 | `Boss New Dragon 166 8` | 0 | `boss166` | *(không ảnh hưởng — JX1 đã đổi sang `boss166`, khác bản Linux `boss020`)* |

⇒ **Ba lựa chọn** (theo thứ tự ưu tiên):
1. Rút 2 bộ `.spr` từ pak client **bản Linux gốc** — nhưng `D:\ServerLinux\Patch\data` **chỉ có
   `slistcache.pak`**, 30 pak kia không có trong dump ⇒ **phải xin lại từ chủ game**.
2. Đổi `NpcResType` của NPC 1684 sang một `passerby*` đã có (rẻ nhất, đổi 1 ô trong **cả hai**
   `npcs.txt`); tương tự NPC 1001 → `boss019`/`boss023`.
3. Bỏ hẳn nhánh Mật Phòng 957 (mất tính năng).

*(NPC 769 “Nhiếp Thí Trần” — người giao nhiệm vụ Săn boss sát thủ, `global\autoexec_npc.lua:26-32`
— dùng `enemy154`, ảnh **CÓ**: `\spr\npcres\enemy\enemy154\enemy154_st.spr` trong `spr.pak`, 197.226 B.
Toàn bộ 2035 NPC của JX1 có cột `HeadImage` **rỗng** ⇒ không cần chân dung.)*

---

## 8. KIỂM CHÉO LẠI 3 KHẲNG ĐỊNH CỦA VÒNG 1 (dùng pak, cả 2 phía)

| Khẳng định vòng 1 | Kết quả kiểm lại | Kết luận |
|---|---|---|
| `05_dulieu.md:483` — `lineup8..56.txt` “thiếu, kể cả trong pak” | quét **37 pak (32 được `package.ini` khai + có thật) client + 9 pak máy chủ**: `\settings\maps\challengeoftime\lineup8/16/56.txt` = **không thấy ở đâu** | ✅ **ĐÚNG** |
| `05_dulieu.md:487` — `渡船刷怪点.txt` không có ở cả hai cây | quét 45 pak: **không thấy** | ✅ **ĐÚNG** (vẫn là chặn cứng #1) |
| `05_dulieu.md:471` — `huoyuedu.txt` ở `slistcache.pak` là pak **client**, máy chủ không đọc được | `slistcache.pak(1340)` ✅ đúng là pak client; **pak máy chủ 9 tệp không có** | ✅ **ĐÚNG** — vẫn phải rút ra đặt lên đĩa máy chủ |
| `05_dulieu.md:485` — `killer.txt` “✅ có trong `Pak\namcung.pak` (⚠️ chưa đối chiếu nội dung)” | ✅ `namcung.pak` 14042 B; client `update03.pak` **14033 B = đúng cỡ tệp Linux** | ✅ xác nhận thêm; nội dung vẫn **chưa giải nén so từng dòng** — CHƯA XÁC MINH |
| `05_dulieu.md:513` — `bosstask_lev90.ini` phải chép sang | quét 45 pak: **không thấy** ⇒ đúng là phải chép | ✅ **ĐÚNG** |

---

## 9. TỔNG KẾT — DANH MỤC VIỆC PHÍA CLIENT

### 9.1 ĐÃ CÓ, không phải làm

| Thứ | Nơi |
|---|---|
| Đồ hoạ 5 cây bản đồ (`.wor` + `Region_C.dat` + minimap) | `update01`, `update03`, `updatejx08`, `maps.pak` |
| **54/54** ảnh NPC (đã sửa: không thiếu cái nào) | `spr.pak`, `update01.pak`, `updatejx08.pak` **[đã sửa theo đối chất]** |
| `MapList.ini` 38/38 map id | `bin\client\settings\MapList.ini` |
| `npcs.txt` client ↔ máy chủ **đồng bộ** (2036 dòng, lệch 0 cột `NpcResType`/`Kind`) | `bin\{client,server}\settings\npcs.txt` |
| `killer.txt` (bảng tên/mô tả 160 boss) | client `update01/03.pak`, máy chủ `namcung.pak` |
| `progressconfig.txt` | client `slistcache.pak`, máy chủ `namcung.pak` |
| `progressbar.ini` + 2 ảnh thanh tiến trình | `slistcache.pak`, `updatejx06.pak` |
| Khung F11 `KUiTaskGuide` + `uitaskguide\taskguide.ini` + `tasktrace.ini` | `Sources\S3Client\…`, `bin\client\Ui\Ui3\uitaskguide\` |
| ~~Giao diện Vượt ải (= mục “Nhiệm vụ Tín Sứ”, TaskId 7)~~ → thật ra là **giao diện Tín Sứ/Thiên Bảo Khố** (`task\tollgate\messenger`), **không** phải Vượt ải | `UiTaskGuide.cpp:513-560` **[đã sửa theo đối chất]** |
| Đường đồng bộ task value xuống client (mọi id) | `KPlayerTask.cpp:76-125` |
| `killerbossmianyi.lua` (chiêu boss) | `bin\client\script\skill\npc\` |

### 9.2 PHẢI THÊM

| # | Việc | Khối lượng | Chặn? |
|---|---|---|---|
| 1 | ~~2 ảnh `.spr`: `boss020`, `passerby043`~~ → **KHÔNG PHẢI LÀM**, cả 54/54 res type đã đủ ảnh trong `spr.pak` | 0 | **KHÔNG** **[đã sửa theo đối chất]** |
| 2 | Mục “Nhiệm vụ Sát thủ” trên F11: 1 dòng `uitasklist.ini` + `BuildSatThuText()` + chuỗi TCVN3 | ~120 dòng C++ | không (tính năng vẫn chạy, chỉ không có bảng tra) |
| 3 | *(tuỳ chọn)* Thanh tiến trình: `KUiProgressBar` + `KProgressBarTimer` + 4 hàm Lua + 1 gói tin | trung bình | **không** — 3 tính năng không gọi; **stub trả 0 là đủ** |
| 4 | *(nhắc)* Mọi NPC mới thêm phải ghi vào **cả hai** `npcs.txt` (client + máy chủ) | quy trình | có, nếu quên → NPC vẽ sai |
| 5 | ~~Đổi nhãn `uitasklist.ini:19` “Nhiệm vụ Tín Sứ” → Vượt ải~~ → **ĐỪNG ĐỔI**: “Tín Sứ” đang đúng (task 1201-1204 = `task\tollgate\messenger`). Nếu muốn Vượt ải có mục F11 thì phải **thêm mục MỚI** đọc task **1505** | 1 mục mới | không **[đã sửa theo đối chất]** |

### 9.3 CHƯA XÁC MINH

* Nội dung `killer.txt` trong `namcung.pak` / `update03.pak` **chưa giải nén so từng dòng**
  với bản Linux (chỉ mới so kích thước 14033/14042 B). Bộ giải nén trong
  `scratchpad\pget.py` chưa xử lý đúng thuật toán nén của pak (không phải zlib thuần).
* **Mã lệnh gói tin** mà `OpenProgressBar` phát ra: đã dịch ngược `0x081082D0`→`0x08108500`
  (ghi struct rồi `call 0x081d0ec0`, stride người chơi `0x8788`), **chưa lần ra số hiệu gói**.
* Chưa kiểm ảnh icon của **16 vật phẩm “THIẾU”** trong `item_can_them.csv` (nếu phải tạo
  vật phẩm mới ở JX1 thì mới cần icon; nếu ánh xạ sang id JX1 sẵn có thì không).

---

## PHỤ LỤC — công cụ dùng lại được (ở `…\scratchpad\`)

| Tệp | Việc |
|---|---|
| `lsgbk.py` | liệt kê cây thư mục, **khôi phục tên GBK** (`s.encode('latin-1').decode('gbk')`) |
| `cpak.py` | tra 1 đường dẫn ảo trong **37 pak (32 được `package.ini` khai + có thật) client JX1** + kiểm tệp loose |
| `spak.py` | như trên nhưng cho **9 pak máy chủ JX1** |
| `pget.py` | rút 1 tệp ra khỏi pak (⚠️ giải nén chưa hoàn chỉnh) |
| `chk_region2.py` | đối chiếu khối `Region_C.dat` loose (`maps2\`) ↔ pak |
| `chk_npcspr.py` | quét 54 `NpcResType` × 11 hậu tố trên pak client |
| `cmp_clientscript.py` | so `Patch\script` ↔ `bin\client\script` |
| `strscan.py`, `cnscan.py`, `settpaths.py` | quét chuỗi ASCII / chuỗi GBK / đường dẫn `\settings\` trong ELF & PE |


---

## ĐỐI CHẤT (tác tử độc lập)

> Vòng 2 — kiểm chứng ngược lại tệp gốc / binary. Ngày 24/08.
> Nguyên tắc: **mặc định coi khẳng định là SAI cho đến khi nguồn gốc chứng minh ngược lại**.
> Đã kiểm **30 khẳng định** → **22 ĐÚNG**, **7 SAI**, **1 ĐÚNG NHƯNG SAI BẰNG CHỨNG**.
> Các dòng sai trong thân bài đã được đánh dấu `**[đã sửa theo đối chất]**`.

### A. Bảng đối chất

| # | Khẳng định | Bằng chứng gốc (đã tự chạy lại) | KẾT LUẬN | Sửa lại thành |
|---|---|---|---|---|
| 1 | §0/§7 — **thiếu 2 ảnh NPC `boss020` + `passerby043`**, `passerby043` là NPC đối thoại duy nhất của Vượt ải ⇒ NPC vô hình ⇒ **chặn tiến độ thật** | `bin\client\settings\NpcRes\npc_normal_res_file.txt:263` `passerby043 → passerby043z.spr`; `:523` `boss020 → enemy216_*.spr`; `npc_res_kind_file_name.txt:265` `spr\npcres\passerby\passerby043`, `:526` `spr\npcres\boss\boss020`. Băm `FileNameToId`: `\spr\npcres\passerby\passerby043\passerby043z.spr` = **`spr.pak`(294.126 B)**; `\spr\npcres\boss\boss020\enemy216_st.spr` = **`spr.pak`(443.223 B)** + `_wlk`(881.553) `_die`(592.248) `_at1`(663.206) `_bat`(443.198). Quét lại **cả 54 res type** bằng 2 bảng NpcRes ⇒ **THIẾU 0** | 🔴 **SAI** | **Không thiếu ảnh NPC nào. Không có chặn tiến độ ở phía ảnh.** Lỗi gốc: `chk_npcspr.py` **tự bịa** khuôn `<res>_<hậu tố>.spr` thay vì đọc 2 bảng `settings\NpcRes\` |
| 2 | §0/§3.2/§9.1 — **“Vượt ải: client ĐÃ PORT RỒI”**, mục “Nhiệm vụ Tín Sứ” (TaskId 7) chính là giao diện Vượt ải | `grep SetTask(1203` trên `D:\ServerLinux\server1\script`: **16/16 hit đều ở `script\task\tollgate\messenger\`** (`messenger_turerukou.lua:155`, `qianbaoku\messenger_turenpc.lua:99`, `posthouse.lua:514`, `messenger_losetask.lua:16`…), **0 hit ở `missions\challengeoftime`**. Ngược lại `missions\challengeoftime\mission_match.lua:51` `SetTask(1505, 0)` và `:80` `SetTask(1505, 1)` — trùng đúng `03_vuotai.md:865` (“1505 = cờ đang trong Vượt Ải”). `grep 1505` trên `Patch\ui\*.lua` = **0 hit** | 🔴 **SAI** | `taskui_messenger.lua` là UI của **Tín Sứ / Thiên Bảo Khố** (`task\tollgate\messenger`, 闯关 ở đây = **tollgate**). **Vượt ải (`missions\challengeoftime`) KHÔNG có mục F11 nào ở bản Linux** — cùng tình trạng Phong Lăng Độ. Tên “Tín Sứ” ở JX1 đang **đúng**, đừng đổi |
| 3 | §2.1 — `ui\ui3_1024` có **175 cửa sổ + 16 thư mục con** | `os.listdir` trên `D:\ServerLinux\Patch\ui\ui3_1024`: **157 tệp (toàn `.ini`) + 15 thư mục con**; cả cây 209 tệp (khớp §2) | 🔴 **SAI** | **157 tệp `.ini` + 15 thư mục con** |
| 4 | §5.1 — `ui\uitasklist.ini` bản Linux có **119+ mục** | `D:\ServerLinux\Patch\ui\uitasklist.ini` = **1.795 B**, `grep -c '^\['` = **20**; danh sách TaskId: `-1,1,2,3,4,5,14,16,17,-1,6,7,8,9,10,11,12,13,15,18` | 🔴 **SAI** | **20 mục** (18 TaskId + 2 tiêu đề nhóm) |
| 5 | §0/§2/§5.1/§6/§7/§8 (8 lần) — quét **“36 pak”** của `bin\client\data` | `ls *.pak` = **37** tệp; chính `cpak.py` nạp **37**. `package.ini` khai **36** mục, trong đó **4** không có trên đĩa ⇒ **chỉ 32 pak thật sự được nạp**. **5 pak trên đĩa KHÔNG được khai**: `serverlistfree.pak`, `sprgame.pak`, `update05.pak`, `vlngaothe1.pak`, `vltkcache.pak` | 🔴 **SAI** | **37 trên đĩa / 36 khai / 32 thật sự nạp.** Kết luận “✅ CÓ trong pak” chỉ có giá trị khi pak trúng nằm trong 32 pak đó |
| 6 | §1 — “⇒ `Patch\settings` là bản settings mà **CLIENT** đọc, không phải bảng riêng của máy chủ” | chuỗi `droprate`: **8 hit** trong `server1\jx_linux_y` (ELF máy chủ), **0 hit** trong `Patch\game_y_unpacked.bin` (exe client). `Patch\config.ini` có khối `[Server] DenialPort=5623` | 🔴 **SAI (quá đà)** | `Patch\` là lớp cập nhật **TRỘN cả client lẫn máy chủ**. `Patch\settings\droprate\` là bảng máy chủ. Phải xét **từng bảng**, không suy ra cả thư mục |
| 7 | §7 — khuôn đường dẫn sprite “xác nhận ở `bin\client\settings\ImageNpcList.ini:32`” | `ImageNpcList.ini` = **1.343 B / 53 dòng**, chỉ được `S3Client\Ui\UiCase\UiMsgSel4.cpp:13` và `UiSayNew.cpp:14` dùng (`#define Image_NPC_List`), liệt kê vài NPC hộp thoại. Đường dẫn thật: `Core\Src\CoreUseNameDef.h:11` `RES_INI_FILE_PATH "\settings\npcres"` + 2 bảng `settings\NpcRes\` | 🔴 **SAI NGUỒN** | Nguồn đúng = `CoreUseNameDef.h:11` + `npc_res_kind_file_name.txt` (cột `ResFilePath`) + `npc_normal_res_file.txt` (14 cột tên `.spr`) |
| 8 | §4.2 — “3 hàm anh em vòng 1 BỎ SÓT”, kèm `0x00214C20 SetProgressBarEvent`, `0x00214C34`, `0x00214C4A` | Phần “vòng 1 bỏ sót” **ĐÚNG** (`grep` `04_api_gap.md` + `04_api_gap.json` + `api_gap_raw.json`: chỉ có `OpenProgressBar`). Nhưng 3 số đó là **offset chuỗi trong `.rodata`** (kiểm bằng `re.finditer` trên ELF). `jx_linux_y.luamap.full.txt` **đã sẵn có địa chỉ MÃ**: `0x080FBAF0 GetProgressBarMaskEvent`, `0x080FC410 ClearProgressBarEvent`, `0x080FC4D0 SetProgressBarEvent` | 🟡 **ĐÚNG nhưng SAI BẰNG CHỨNG** | Giữ kết luận, thay địa chỉ bằng **địa chỉ hàm** ở trên (dùng để `re_disasm.py`) |
| 9 | §1.1 — 8 tệp `bosstask_lev20..90.ini`, `killer.ini`, `trigger_challengeoftime.lua`, `lineup8.txt` **có ở cả hai gốc**, cùng cỡ | `ls -la` cả hai cây: `bosstask_lev20.ini` 6233 B ×2, `lev90` 6214 B ×2 (đủ 8 tệp), `killer.ini` 492 ×2, `trigger_challengeoftime.lua` 704 ×2, `lineup8.txt` 99 ×2. `grep -c bosstask chi_co_o_patch.txt` = **0** | ✅ **ĐÚNG** | — (đính chính đề bài là chính xác) |
| 10 | §1.1 — 5 tệp chỉ-có-ở-B ở dòng 246/287/288/289/290 của `chi_co_o_patch.txt` | Đọc thẳng 5 dòng: `great_night\风陵渡.txt 1520`, `中原北区\渡船\渡船刷怪点.txt 893`, `风陵渡北岸\baiyingyingboss.txt 99`, `yanxiaoqianboss.txt 99`, `风陵渡南岸\herenwoboss.txt 143` (tệp 845 dòng) | ✅ **ĐÚNG** | — |
| 11 | §2 — kiểm kê `Patch\`: ui 443 · spr 519 · script 634 · settings 1754 · maps 52 · **data 1** · music 36 · users 49 | `find -type f | wc -l` từng thư mục: **khớp cả 8 con số**; `Patch\data` chỉ có `slistcache.pak` (10.855.818 B) | ✅ **ĐÚNG** | — |
| 12 | §3.1 — `taskui_killer.lua` 1027 B / 28 dòng; `:4` KILLER, `:8` `GetTask(1082)`, `:18` `TabFile_Load`, `:22` nhánh `==0` (7 thành), `:25` nhánh `>0` dùng `Uworld1082+1` | `gbktool read`: khớp **từng dòng một** (1027 B, 28 dòng, `killernews` có đúng 7 thành) | ✅ **ĐÚNG** | — |
| 13 | §3.1 — `taskguide.txt:9` = `8 / Nhiệm vụ Sát thủ / 0 / \UI\taskui_killer.lua / showkillertaskdesc`; tệp 18 dòng | Dòng 9 khớp nguyên văn; dòng 10 = ID 9 “Thách thức thời gian”; tệp 1.609 B, 18 dòng có nội dung; **có ở cả hai gốc, cùng cỡ** | ✅ **ĐÚNG** | — |
| 14 | §3.2 — `taskui_messenger.lua:21` chú thích `--闯关任务之信使任务`; `:22-25` `GetTask(1201..1204)`; `:79-104` nhánh `Uworld1203 == 10/20/21/25/30` | Khớp chính xác (dòng 79/82/88/96/101, `end` ở 104). *(Nhưng suy luận rút ra từ đó là SAI — xem #2.)* | ✅ **ĐÚNG (dữ kiện)** | Dữ kiện đúng, **kết luận sai** |
| 15 | §3.2/§5.1 — `UiTaskGuide.cpp:513` `BuildTinSuText()`, `:531-552` `case 10/20/21/25/30`, `:379-394` phân nhánh, `:163-190` cập nhật theo task id, tệp 23.538 B | `sed -n '505,565p'` + `grep -n`: **khớp từng số dòng** (513, 531/534/546/549/552, 379/383/387/391, 170/175/177/180/185/187) | ✅ **ĐÚNG** | — |
| 16 | §2.2 — `npcs.txt` hai bản: 514.004 / 514.002 B, 2036 dòng, lệch 11 dòng thô, **lệch 0** cột `NpcResType`+`Kind` | Đọc nhị phân so từng dòng: raw diff = **11** (dòng 137, 543, 626, 1593-1597, 1973, 2010, 2011); `NpcResType`/`Kind` mismatch = **0**. `CoreUseNameDef.h:119` `NPC_SETTING_FILE "\settings\NpcS.txt"` khớp | ✅ **ĐÚNG** | — |
| 17 | §7 — NPC 1684 `Kind=3` `passerby043`; NPC 1001 `boss020`; NPC 1033 `boss166`; NPC 769 `enemy154`; `HeadImage` rỗng toàn bộ | Đọc `bin\server\settings\npcs.txt`: khớp cả 4 NPC; **2035 dòng, HeadImage non-empty = 0** | ✅ **ĐÚNG** | — |
| 18 | §7 — `npc_can_them.csv` 140 dòng, **54** `NpcResType`, lọc `Kind=3` ra **đúng 1** (NPC 1684) | `csv.DictReader`: 140 / 54 / Kind==3 = 1 | ✅ **ĐÚNG** | — |
| 19 | §4.1 — không cần gói tin mới; `KPlayerTask::SetSaveVal` (`:76-85`) đồng bộ **mọi** task id; `:97-125` id 0-255 → `s2c_taskvalue`, id ≥256 → `PLAYER_SCRIPTACTION_SYNC`+`UI_TASKVALUE` | `sed -n '70,130p' Sources/Core/Src/KPlayerTask.cpp`: `SetSaveVal` ở **:77**, gọi `SyncTaskValueToClient` ở **:83** không lọc id; `SyncTaskValueToClient` ở **:98**, nhánh `nTaskId < 256` / `else` đúng như mô tả | ✅ **ĐÚNG** | — |
| 20 | §4.1 — quét byte GBK trong `game_y_unpacked.bin`: `闯关` 0 · `风陵渡` 0 · `渡船` 0 · `杀手` 0 · `通缉` **3** | `re.finditer` theo byte `B4B3B9D8 / B7E7C1EAB6C9 / B6C9B4AC / C9B1CAD6 / CDA8BCA9`: **0/0/0/0/3** | ✅ **ĐÚNG** | — |
| 21 | §3.4/§4.1/§4.2 — 10 offset PE/ELF (`0x2E76DC`, `0x2E7700`, `0x2E7744`, `0x2E7F00`, `0x2E8290`, `0x2A2D64`, `0x2A84B0`; ELF `0x2185D5`, `0x213C74`, `0x21CE8D`, `0x222AD8`, `0x209D3D`, `0x20B349`, `0x220EE0`) | `re.finditer` trên cả hai binary: **khớp 100%, không sai một offset nào** | ✅ **ĐÚNG** | — |
| 22 | §3.4 — `Patch\settings\killer.ini:1` = `;通缉追杀系统专用ini`, `MinTargetLevel=50`, `MinReward=100000` ⇒ hệ **truy nã người chơi**, không phải săn boss sát thủ | `gbktool read`: dòng 1 khớp nguyên văn; `MoneyPerHour=10000` `:4`, `MinTargetLevel=50` `:6`, `MinReward=100000` `:10` | ✅ **ĐÚNG** | — |
| 23 | §3.3 — 3 tính năng chỉ dùng `Msg2Player` **180 lần** / `Msg2Team` **13 lần** | `grep -ro` trên `src_utf8\{satthu,phonglangdo,vuotai}`: 33+52+95 = **180**; 5+0+8 = **13** | ✅ **ĐÚNG** | — |
| 24 | §4.2 — không tính năng nào gọi `tbProgressBar:Open/:Start/:OpenByConfig`; `progressbar.lua:123` chạy `InitEventType()` lúc nạp | `grep -rn tbProgressBar` ngoài `lib/progressbar.lua` = **0 hit**; tệp 123 dòng, dòng cuối = `tbProgressBar:InitEventType()`; `OpenProgressBar` được gọi ở `:95` bên trong `tbProgressBar:Start` (`:92`) | ✅ **ĐÚNG** | — |
| 25 | §6 — bảng bản đồ phía client đủ 100 % (`.wor` + minimap, 6 hàng, kèm cỡ byte và tên pak) | Băm `FileNameToId` từng đường dẫn GBK: `风陵渡.wor` `update01(111)` + minimap `update01(243.896)`; `渡船.wor` `update01(113)` + `update01(2.837)`; `莫高窟.wor` `update01(113)` + `update01(403.105)`/`updatejx03(311.772)`; `杀手的试炼.wor` `update01`+`update03(113)` + `(6.929)`; `安期炼丹洞.wor` `updatejx08(92)` + `(18.940)`; `凤翔.wor` `maps.pak(284)` + `(519.552)` — **khớp toàn bộ**. `MapList.ini` `Count=1000`, 38/38 id có mặt | ✅ **ĐÚNG** | — |
| 26 | §6 — nguồn C++: `SceneDataDef.h:13` `Region_C.dat`, `KScenePlaceRegionC.cpp:79` `sprintf(...v_%03d\%03d_...)`, `KScenePlaceC.cpp:368` `\%s.wor` | `sed -n` từng tệp: khớp **đúng số dòng cả ba** | ✅ **ĐÚNG** | — |
| 27 | §6 — `maps2\` là thư mục dàn dựng, `grep maps2` trong `Game.exe`/`CoreClient.dll`/`engine.dll` = 0 hit | `grep -a -c -i maps2`: **0 / 0 / 0** (thư mục `maps2` có tồn tại trên đĩa) | ✅ **ĐÚNG** | — |
| 28 | §5.3 — `Patch\script` 634 ↔ `bin\client\script` 436; **305 trùng, 329 chỉ có ở Linux**; `killbosshead.lua:2611-2630` toàn dòng comment | `os.walk` so tương đối (hạ chữ thường): 634 / 436 / **chung 305 / chỉ Linux 329 / chỉ JX1 131**; dòng 2611-2630 của `src_utf8\satthu\...\killbosshead.lua` đều mở đầu `--` | ✅ **ĐÚNG** | — |
| 29 | §5.2 — `activity.txt` **22 dòng**, `uiactivityguide` **5 tệp**, không dòng nào của 3 tính năng | `wc -l` = 22 (1.303 B); `ls uiactivityguide | wc -l` = 5 | ✅ **ĐÚNG** | — |
| 30 | §8 — 5 khẳng định vòng 1 kiểm lại: `lineup8/16/56.txt` không có ở đâu · `渡船刷怪点.txt` không có · `huoyuedu.txt` chỉ ở pak client · `killer.txt` `namcung.pak(14042)` / `update03.pak(14033)` · `bosstask_lev90.ini` phải chép | `spak.py`+`cpak.py`: `lineup8.txt` = KHÔNG THẤY cả 2 phía; `killer.txt` = `namcung.pak(14042)`, client `update01(14042)`+`update03(14033)`; `bosstask_lev90.ini` = KHÔNG THẤY; `huoyuedu.txt` = không có ở pak máy chủ. Bản Linux `killer.txt` = **14.033 B ở cả hai gốc** | ✅ **ĐÚNG** | — |

### B. Bỏ sót của chính vòng 2

| # | Chỗ bỏ sót | Bằng chứng | Tại sao quan trọng |
|---|---|---|---|
| **B1** | §4.2 kết luận `script\lib\progressbar.lua` “❌ không có” và §7 kết luận “**phải xin lại từ chủ game**” — nhưng **cả hai vật tư đều đã có sẵn trên đĩa** | `bin\client\script\lib\progressbar.lua` = **3.436 B**, `md5 = 954f27556d038e9016632273b5120107` — **trùng md5** với `D:\ServerLinux\server1\script\lib\progressbar.lua`. Ảnh NPC: xem đối chất #1 | Báo cáo đẩy 2 việc sang “cần vật tư ngoài / chặn tiến độ”, trong khi việc thật chỉ là **`copy` một tệp** sang `bin\server\script\lib\`. Đây là loại sai đắt nhất: nó dừng cả một nhánh port |
| **B2** | **Rủi ro phương pháp chưa nêu**: `cpak.py` quét **mọi** `.pak` trong `data\`, kể cả pak **không được `package.ini` khai** ⇒ “✅ CÓ trong pak” có thể là **dương tính giả** | 5 pak trên đĩa không được khai: `serverlistfree.pak`, `sprgame.pak`, `update05.pak`, `vlngaothe1.pak`, `vltkcache.pak`. **Ví dụ thật đã dính**: `\settings\progressconfig.txt` trúng `serverlistfree.pak(1461)` — pak này **client không nạp**. Phía máy chủ tương tự: `spak.py` quét 9 tệp nhưng `bin\server\package.ini` chỉ khai 7 (`maps_error.pak`, `maps_tieu_bang_chien.pak` không nạp) | Mọi câu “ĐÃ CÓ, không phải làm” ở §9.1 cần kiểm lại pak trúng có nằm trong 32 pak được nạp không |
| **B3** | Báo cáo ghi “thử **11 hậu tố** (`_wlk _st _stand _atk _atk1 _die _cst _hit _run _sit _mgc`)” nhưng công cụ chỉ thử **5** — và **cả 11 hậu tố đó đều không phải bộ hậu tố thật** | `chk_npcspr.py:23` `SUF = ["_wlk", "_st", "_stand", "_atk", "_die"]`. Bộ hậu tố THẬT (đọc `npc_normal_res_file.txt` dòng 1 = 14 cột): `_st`, `_pst`, `_wlk`, `_bat`, `_die`, `_at` / `_at1` / `_at2` / `_at01` / `_at02`, và cả tên **không có hậu tố** (`passerby043z.spr`, `passerby043s.spr`). Ví dụ `:515` `boss019 → boss019_at01.spr`, `boss019_bat.spr` | Con số trong báo cáo **không khớp công cụ đã chạy** — và đó chính là gốc của sai #1. Nguyên tắc: **đừng đoán tên tệp khi đã có bảng khai báo** |
| **B4** | §5.1 “đúng 4 chỗ sửa” liệt kê `uitasklist.ini` chỉ 2 khoá `Name` + `TaskId`, **bỏ 3 khoá điều khiển** | Bản Linux `Patch\ui\uitasklist.ini` mục `[9]` (Sát thủ): `Type=2` (bỏ được) · `Class=2` (nhiệm vụ hàng ngày) · `Trace=1` (theo dõi được). Chú thích `:6-9` của chính tệp giải nghĩa 3 khoá này | Thiếu 3 khoá ⇒ mục F11 mới sẽ mặc định sai loại/không bỏ được, phải sửa lại lần hai |
| **B5** | **Mâu thuẫn trong chính dữ liệu Linux, chưa ai nêu**: `uitasklist.ini` đặt `Trace=1` cho Sát thủ nhưng `taskguide.txt` **để trống 4 cột theo dõi** của đúng dòng đó | `taskguide.txt:1` khai **9 cột** (`…TaskTraceScriptFile / ShowReducedInfoFunc / ClickTraceItemFunc / TraceTaskFunc`). Dòng `:9` (ID 8) chỉ có **5 trường**; so với dòng `:6` (ID 5) và `:15` (ID 14) điền đủ | Nếu port nguyên `Trace=1` sang JX1 mà không cài đường `tasktrace`, bấm “theo dõi” sẽ ra mục rỗng. Nên đặt `Trace=0`/`2` cho mục Sát thủ ở JX1 |
| **B6** | §5.1 nói `taskui_messenger.lua` ở JX1 “cũng vô dụng, đã được C++ thay” nhưng **bỏ sót bản `update03.pak`** | `cpak.py \ui\taskui_messenger.lua` → `slistcache.pak(5462)`, `update01.pak(4150)`, **`update03.pak(4123)`**. Bản Linux gốc = **5.441 B** | Ba bản khác cỡ nhau ⇒ nội dung khác nhau; nếu sau này cần đối chiếu thì phải biết có 3 bản, không phải 2 |

### C. Kết luận đối chất

1. **Hai kết luận LỚN NHẤT của báo cáo đều sai**: (a) “chặn tiến độ vì thiếu 2 ảnh NPC” — không thiếu ảnh nào; (b) “Vượt ải đã port client rồi” — chưa port, và bản Linux vốn **không có** giao diện F11 cho Vượt ải.
2. Phần **bằng chứng nhị phân** (offset ELF/PE, băm pak bản đồ, đếm chuỗi GBK, số dòng C++) của báo cáo **rất chắc — 100 % khớp khi kiểm lại**. Điểm yếu nằm ở chỗ **suy diễn từ tên** (`闯关` → Vượt ải; `<res>_<hậu tố>.spr`) thay vì đọc **bảng khai báo** (`npc_normal_res_file.txt`, `SetTask(id)`).
3. **Luật rút ra cho vòng 3**: mỗi khi định kết luận “THIẾU / KHÔNG CÓ”, phải trả lời trước hai câu — *(i)* tên tệp lấy từ **bảng khai báo** nào? *(ii)* pak trúng có nằm trong danh sách `package.ini` **thật sự được nạp** không?
