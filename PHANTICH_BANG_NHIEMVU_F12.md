# PHÂN TÍCH — BẢNG NHIỆM VỤ F12 ("Chỉ nam nhiệm vụ") & BẢNG THÔNG BÁO NHIỆM VỤ DÃ TẨU

Ngày: 19/08/2026 · **CHỈ PHÂN TÍCH — chưa sửa gì, chưa build, chưa commit.**
Phạm vi hỏi: (1) điều tra UI/SPR "bản nhiệm vụ" khi nhận nhiệm vụ Dã Tẩu, phím **F12**; (2) dịch ngược bản Linux tìm "bảng thông báo nhiệm vụ".
Tài liệu nền: `AUTO_DATAU_SPEC.md`, `BANGIAO_DATAU_TASKLINK.md`, `PHANTICH_DATAU_LINUX_GOC.md`.

> **Lưu ý cách gọi tên.** `D:\ServerLinux\Patch` **không phải bản gốc Trung Quốc**: `uitasklist.ini:3-4` ghi rõ *"Ui designed and modified by sonkt — volamtranhba - 20211023"*, `taskguide.ini:2` là chú thích tiếng Việt. Đây là **bản Việt hoá 2021-2022 CÓ SẴN tính năng** — vẫn là nguồn tham chiếu tốt nhất, nhưng đừng gọi là "bản gốc".

---

## 0. KẾT LUẬN NGẮN (đọc mục này là đủ để quyết)

1. **Tính năng có thật, tên là cửa sổ `NewTask` — "Chỉ nam nhiệm vụ"**, mở bằng F12. Lớp C++ là `KUiTaskGuide` (+ `KUiTaskTrace`), RTTI nằm nguyên trong `game_y_unpacked.bin`.
2. **Ở dự án ta F12 là phím chết.** `Ui\autoexec.lua` vẫn còn dòng `AddCommand("F12","","Open([[NewTask]])")`, nhưng `l_WindowList` trong `ShortcutKey.cpp` **không có mục `"NewTask"`** ⇒ `FindWindow()` trả −1, bấm không ra gì. Kiểm byte trên `Game.exe` đang phát hành: chuỗi `NewTask` = **0 lần**.
3. **Dã Tẩu CÓ trong bảng đó**: `settings\task\taskguide.txt` dòng 7 → `TaskId=6`, kịch bản `\UI\taskui_random.lua`, hàm `showrandraskdesc`.
4. **⚠️ ĐÍNH CHÍNH KỲ VỌNG:** ở bản tham chiếu, **nhận nhiệm vụ Dã Tẩu KHÔNG làm bảng tự bật lên và cũng KHÔNG tự làm mới**. Bảng lọc `tbTaskVariable` trong `taskguide.lua` chỉ liệt kê TaskId **5, 14, 15, 16** — không có 6. Người chơi **phải tự bấm F12** rồi tự chọn dòng "+Nhiệm vụ Dã Tẩu".
5. **Công vẽ ảnh = 0.** Toàn bộ 10 ảnh SPR của bảng F12 (và cả 8 ảnh của sổ tay F11) **đã nằm sẵn trong pak của client đang phát hành**. Không phải vẽ, không phải chép.
6. **Chỉ thiếu đúng 5 thứ dữ liệu + 2 lớp C++.** Bốn trong năm tệp thiếu có sẵn ở bản tham chiếu; tệp điều phối `taskguide.lua` nằm trong `D:\ServerLinux\Patch\data\slistcache.pak` (3.835 B) và đã rút ra được.
7. **🔴 NÚT THẮT THẬT SỰ nằm ở GIAO THỨC, không ở giao diện.** `TASK_VALUE_SYNC.nTaskId` khai **`BYTE`** ⇒ mọi biến Dã Tẩu (1021/1025/1027/1028/1030/1031/1032) **bị cắt còn 8 bit** khi đẩy xuống client. Làm giao diện trước mà không sửa cái này thì được một cái bảng rỗng — **và tệ hơn: hiện tại server đang ghi đè nhầm ô task của client** (xem mục 7).
8. Nhân tiện, tìm ra **3 lỗi thật** trong đường `AddNote` của dự án, một trong đó **phá ngăn xếp GameServer** (mục 8).

---

## 1. TRẢ LỜI THẲNG HAI CÂU HỎI

### Câu 1 — "khi nhận nhiệm vụ Dã Tẩu thì hiện bảng nhiệm vụ, bấm F12"

| Vế | Sự thật | Chứng cứ |
|---|---|---|
| Có bảng nhiệm vụ mở bằng F12 | **ĐÚNG** | `taskguide.ini:2` ghi thẳng *"Giao diện theo dõi nhiệm vụ - F12"*; `autoexec.lua:25` |
| Bảng đó hiển thị nội dung nhiệm vụ Dã Tẩu | **ĐÚNG** | `taskguide.txt` dòng 7 → `taskui_random.lua::showrandraskdesc` |
| Nhận nhiệm vụ thì bảng **tự hiện lên** | **SAI** | `taskguide.lua` — `tbTaskVariable[1] = {[5],[14],[15],[16]}`; Dã Tẩu là ID 6, không có trong bảng ⇒ `playertaskchange` trả −1, không gọi `NewTask_ShowTask` |
| Bảng **tự làm mới** khi biến đổi | **SAI** | như trên; `tbTaskVariable[2] = {[5],[14]}` cho khung theo dõi |
| Dã Tẩu theo dõi được ở khung phụ | **SAI** | `uitasklist.ini` mục `[7]`: `Trace=2` (không theo dõi được); `taskguide.txt` dòng 7 chỉ điền 5/9 cột nên `tasktrace_*` đều return sớm |

**Nếu chủ game vẫn muốn "nhận nhiệm vụ là bảng tự bật"** thì đó là **thêm mới**, không phải khôi phục — và đã có sẵn đường rẻ nhất: kênh `emSCRIPT_PROTOCOL_OPENWINDOW` (mục 4.4).

### Câu 2 — "bảng thông báo nhiệm vụ" ở bản Linux

Có **ba** kênh khác nhau, đừng lẫn:

| Kênh | Là gì | Ở bản tham chiếu | Ở dự án ta |
|---|---|---|---|
| **`AddNote`** (ELF `0x08124DC0`) | Ghi 1 dòng vào **sổ tay nhiệm vụ (F11)** — lưu ra `MissionMemory.dat` | dùng **231 lần** (nhiều nhất ở `huashan2013` 100, `pgaming\npc\chuongmoncacphai` 91) | **ĐÃ CÓ** (`ScriptFuns.cpp:13277`), đường ống chạy đủ: `UI_NOTEINFO` → `KPlayer.cpp:7793` → `GameSpaceChangedNotify.cpp:495` → `KUiTaskNote::WakeUp` |
| **`TaskTip`** (ELF `0x08122730`) | Dòng nhắc nổi "bạn nhận được nhiệm vụ…" | dùng **đúng 1 lần** (`task/random/task_head.lua:95`, là hệ nhiệm vụ đồng hành, **không phải** Dã Tẩu) | **KHÔNG CÓ** |
| **`NewTask_TaskTextOut`** (client) | In dòng vào **bảng F12** | là ruột của mọi `taskui_*.lua` | **KHÔNG CÓ** |

**Hệ Dã Tẩu (tasklink) ở bản tham chiếu không hề dùng `AddNote` lẫn `TaskTip`.** Nó chỉ `SetTask` + nói chuyện qua `Say`/`Msg2Player`/`AddGlobalCountNews`. Nghĩa là "bảng thông báo nhiệm vụ" của Dã Tẩu **chính là bảng F12**, kéo dữ liệu theo kiểu *pull*, không phải *push*.

---

## 2. KIẾN TRÚC BẢNG F12 (dịch ngược từ `game_y_unpacked.bin`, VA = `0x401000` + offset tệp)

```
F12  ──▶ autoexec.lua:25  Open("NewTask")
          │
          ▼  bảng 48 tên cửa sổ @ 0x006E4470, "NewTask" = chỉ số 28
        dispatcher 0x004104B0   (cmp 0x2F / ja / jmp [eax*4 + 0x00410A58])
          │  case 28 → 0x00410821
          ▼
        KUiTaskGuide   IsOpen 0x005438B0 · Close 0x00543850 · Open 0x00543770   (nút BẬT/TẮT)
          │
          ├─ Init @~0x005441E1 :  _snprintf("%s\\%s", <thư mục scheme>, "uitaskguide\taskguide.ini")
          │                       ⇒ ĐƯỜNG DẪN KHÔNG GẮN CỨNG ui3_1024 — khe phẳng Ui\Ui3 của ta dùng được
          ├─ đọc khoá TaskIni  ⇒ \UI\uitasklist.ini   (cây danh mục 20 mục, 2 nhóm)
          └─ nạp Lua \script\ui\task\taskguide.lua
                 │  load_data() đọc settings\task\taskguide.txt (9 cột)
                 │  taskguide_showtask(nId) → DynamicExecute(guidescript, fullfunc, nId)
                 ▼
             \UI\taskui_random.lua :: showrandraskdesc(nTaskGenre)      ← DÃ TẨU (TaskId 6)
                 │  GetTask(1021/1028/1030/1025/1027/1026) + TabFile_Load(tasklink_*.txt)
                 ▼
             NewTask_TaskTextOut(nTaskGenre, "…")   → in vào [TaskContent]
```

**Sự kiện làm mới** (chuỗi @`0x006A54C8`): `taskguide_showtask`, `playertaskchange`, `onplayerinfochange`, `onridestatechange`.
`playertaskchange` được gọi từ C++ `0x00543C80`, và hàm đó chỉ có **một** người gọi: nhánh `0x004091EF` = **entry[83]** của bảng nhảy `0x0040A1C8`. Vì dispatcher có `sub ecx,1` trước khi tra bảng ⇒ **notify id = 84**. Trần `cmp 0xA8 / ja` ⇒ bản tham chiếu có **169** notify id (dự án ta có 92).

**Các hàm Lua giao diện mà bản tham chiếu đăng ký thêm** (bảng @`0x006E4800`, chuỗi @`0x00698650`):
`NewTask_TaskTextOut` (number,string,string) · `NewTask_DetailTextOut` (number,string) · `NewTask_ShowTask` (number) · `NewTask_AddNpcFindPath` · `NewTask_AddMapCoordinate` · `NewTask_RemoveMapCoordinate`, cộng `TraceTask`, `TraceTask_Update`, `TraceTask_TextOut`, `SwitchTaskTrace`, `AutoCrossMapFindPath`.
Bảng Lua giao diện của **ta** (`ShortcutKey.cpp:2248-2329`) **kết thúc đúng ở `SetScreenShotFolder`** — tức bản dựng S3Client của ta là thế hệ cũ hơn, thiếu toàn bộ khối trên.

### 2.1 Nội dung Dã Tẩu mà bảng in ra (`taskui_random.lua`)

| Loại (task 1021) | Bảng tra | Câu hiện ra |
|---|---|---|
| 0 | — | "Bạn chưa tiếp nhận nhiệm vụ của Dã Tẩu…" |
| 1 | `tasklink_buygoods.txt` | "Đến `<tiệm>` mua `<món>` cho Dã Tẩu." |
| 2 | `tasklink_findgoods.txt` | "Đi tìm `<thuộc tính>` ít nhất `<Min>` lớn nhất `<Max>` `<loại>`…" |
| 3 | `tasklink_showgoods.txt` | "Đi tìm bộ trang bị `<thuộc tính>`…" |
| 4 | `tasklink_findmaps.txt` | "Đến `<map>` tìm `<Num>` quyển Địa Đồ/Mật Chỉ… Bạn đã thu thập `GetTask(1025)` tấm" |
| 5 | `tasklink_upground.txt` | "Đi nâng cấp `<N>` điểm kinh nghiệm / danh vọng / phúc duyên / PK / Tống Kim" (có tính hiệu số qua `GetTask(1026)`) |
| 6 | `tasklink_worldmaps.txt` | "Đưa Dã Tẩu `<Num>` mảnh Sơn Hà Xã Tắc… đã có `GetTask(1027)`" |

Cuối hàm: `course==1` → tiền tố `AEXP_TASKDESCWORKING`; `course==2/3` → `AEXP_TASKDESCCOMPLETE`.
**Đúng khớp 100% bộ biến trong `AUTO_DATAU_SPEC.md` §3** — nghĩa là nếu dựng lại bảng này, auto Dã Tẩu cũng có thêm một nguồn đọc trạng thái hợp lệ.

---

## 3. KIỂM KÊ TÀI NGUYÊN — "LÀM UI SPR" THỰC RA KHÔNG PHẢI LÀM

`taskguide.ini` tham chiếu **10 tệp SPR** (`tasktrace.ini` thêm 1 nhưng cả tệp bị chú thích; `opentracebtn.spr` không ini nào dùng).
Tôi đã đọc chỉ mục **37 pak** của client đang phát hành bằng đúng thuật toán `KPakList::FileNameToId` (`Engine\Src\KPakList.cpp:72`) + chuẩn hoá `g_GetPackPath` (`KFilePath.cpp:259`); tự kiểm: 297/400 tệp `.spr` rời cũng trùng id trong pak ⇒ thuật toán đúng.

| Tệp SPR | Cỡ | Nằm ở pak nào của **client đang chạy** |
|---|---|---|
| `\spr\Ui4\主界面\任务指南资源\renwuzhinanjiemian.spr` (khung chính 506×355) | 173.922 B | `updatejx14.pak` |
| `\spr\Ui4\主界面\任务指南资源\任务追踪底板.spr` (nền khung theo dõi) | 43.008 B | `updatejx14.pak` |
| `\Spr\Ui3\主界面\关闭_vn.spr` (nút đóng) | 10.268 B | `updatejx08.pak` |
| `\spr\Ui4\common\拖动条.spr` (thanh kéo, ×2 chỗ) | 2.732 B | `updatejx14.pak` |
| `\spr\Ui4\common\小按钮四字.spr` (nút 4 chữ, ×3 chỗ) | 4.961 B | `updatejx14.pak` |
| `\spr\UI3\新任务系统\按钮\勾12 / 12ok / 14 / 14ok / 16 / 16ok.spr` | 984–1.104 B | `update01.pak` |

Và **8/8 ảnh của sổ tay F11** (`任务记事*.spr`, 136.129 B + 6 nút + thanh kéo) đều có trong `spr.pak` / `update01.pak` / `update03.pak`.

> ⚠️ **Đính chính giữa phiên:** quét tệp **rời** thì các ảnh này đều báo "THIẾU" — kết luận đó **sai**. Chỉ khi tra chỉ mục pak mới thấy đủ. `KPakFile::Open` (`KPakFile.cpp:241-268`) đọc **đĩa trước, pak sau** ⇒ muốn đè bản mới cứ thả tệp rời, không cần đóng lại pak.

**Kết luận mục này: không cần vẽ mới bất kỳ ảnh nào; cũng không cần chép ảnh từ bản tham chiếu.**

---

## 4. HIỆN TRẠNG DỰ ÁN — CÓ GÌ / THIẾU GÌ

### 4.1 Bảng dứt khoát (đã tra hash trên toàn bộ 37 pak + tệp rời)

| | Hạng mục | Trạng thái |
|---|---|---|
| ✅ | 10 + 1 ảnh SPR bảng F12, 8 ảnh sổ tay F11 | **CÓ** trong pak |
| ✅ | `\ui\taskui_*.lua` — **7/9 tệp** (random, master, world, killer, messenger, besttong, wlls) | **CÓ** trong `update01/update03/slistcache.pak`, **đã dịch tiếng Việt** |
| ✅ | 6 bảng `\settings\task\tasklink_*.txt` (buygoods/findgoods/showgoods/findmaps/upground/worldmaps) | **CÓ** trong `update01.pak` |
| ✅ | `Ui\autoexec.lua` đã gán sẵn F12 | **CÓ** (dòng 40) |
| ✅ | Tệp ini "hệ thống nhiệm vụ mới" thế hệ JX1 (13 section) | **CÓ** trong 3 pak |
| ❌ | `\UI\uitasklist.ini` | **THIẾU** (id `0x2B39313E`, vắng ở cả 37 pak) |
| ❌ | `\settings\task\taskguide.txt` | **THIẾU** (id `0x302AED19`) |
| ❌ | `\script\ui\task\taskguide.lua` | **THIẾU** (id `0x77CF2192`) |
| ❌ | `<scheme>\uitaskguide\taskguide.ini` | **THIẾU** |
| ❌ | `<scheme>\uitaskguide\tasktrace.ini` | **THIẾU** |
| ❌ | Lớp C++ `KUiTaskGuide` / `KUiTaskTrace` | **THIẾU** (grep `TaskGuide` trên `S3Client` = 0) |
| ❌ | 6 hàm Lua `NewTask_*` + mục `"NewTask"` trong `l_WindowList` | **THIẾU** |
| ❌ | Client Lua thiếu `GetTask`, `TabFile_Load`, `TabFile_GetCell` | **THIẾU** (CoreClient.dll chỉ có `GetTaskTemp`/`SetTaskTemp`/`IncludeLib`) |

### 4.2 Bốn trong năm tệp thiếu đã có sẵn để tham chiếu

| Tệp | Ở đâu |
|---|---|
| `uitasklist.ini` | rời: `D:\ServerLinux\Patch\ui\` (1.795 B, 20 mục — bản **mới hơn**) · pak: `slistcache.pak` (1.469 B, 19 mục) |
| `taskguide.txt` | rời: `Patch\settings\task\` (1.609 B) — **trùng byte 100%** với bản trong `slistcache.pak` |
| `taskguide.ini` + `tasktrace.ini` | rời: `Patch\ui\ui3_1024\uitaskguide\` (3.228 + 932 B) và `ui3_800\` (3.336 + 1.040 B) |
| **`taskguide.lua`** | **`D:\ServerLinux\Patch\data\slistcache.pak`, 3.835 B** — cùng `taskguide_head.lua` (1.103 B), `protocol.lua`, `protocol_def_c.lua`, `clientcmd.lua`, `taskui_translife7.lua` |

> Đính chính giữa phiên: tôi từng nói "`taskguide.lua` không tồn tại ở đâu, phải viết lại". **Sai** — nó nằm trong pak của bản tham chiếu, đã rút ra được và tôi đã xác nhận độc lập bằng tra hash.

Chỉ còn **2 tệp thật sự không tìm thấy ở bất kỳ đâu**: `\script\ui\task\tasktrace.lua` và `\UI\UnChangeShortKey.lua`.

### 4.3 Sổ tay nhiệm vụ F11 (`KUiTaskNote`) — đã có nhưng thiếu 2 trang

`KUiTaskNote` **là của bản tham chiếu**, không phải đội Việt tự thêm: RTTI liền khối trong `game_y_unpacked.bin`:
`.?AVKTaskEdit@@` `0x006E7514` · `.?AVKUiTaskNote@@` `0x006E752C` · `_Item` `0x006E7548` · `_System` `0x006E7568` · `_Personal` `0x006E758C` · `.?AVKUiUpdate_Bulletin@@` `0x006E75B0`, kèm chuỗi `MissionMemory.dat` `0x0069FD60`.
Bản tham chiếu chạy **đồng thời cả hai** hệ: F11 sổ tay + F12 chỉ nam. Bản tham chiếu có **5 tệp ini** (khung + 4 trang: công báo / nhiệm vụ hệ thống / ghi chép cá nhân / trang bị khoá); dự án ta port **3 tệp** (khung + 2 trang) — trang công báo vốn thuộc lớp riêng `KUiUpdate_Bulletin`.

### 4.4 Có sẵn đường để máy chủ ÉP client mở bảng

`protocol_def_c.lua` (trong `slistcache.pak` bản tham chiếu) đăng ký mục cuối:
```lua
{ "emSCRIPT_PROTOCOL_OPENWINDOW", "script\\lib\\clientcmd.lua", "ClientCmd:OnOpenWindow", {OBJTYPE_STRING} }
```
và `clientcmd.lua`: `function ClientCmd:OnOpenWindow(szWindow) Open(szWindow); end`.
Bản tham chiếu **không dùng kênh này cho Dã Tẩu** (cả cây `server1` chỉ 1 chỗ dùng: `gmscript\gmcommand_4.lua:6-10` với chuỗi `"reloadfilelist"`). Nhưng nếu chủ game muốn "nhận nhiệm vụ là bảng tự bật", đây là đường rẻ nhất — chỉ đẩy chuỗi `"NewTask"`.
⚠️ Ở client **ta**, `protocol_def_c.lua` chỉ còn **234 B** (bản tham chiếu 3.463 B) ⇒ tầng script-protocol của ta đã bị rút ruột, phải dựng lại nếu muốn dùng kênh này.

---

## 5. PHÍA MÁY CHỦ — KHÔNG CẦN ĐỘNG GÌ (cho phần giao diện)

Quét ELF `D:\ServerLinux\server1\jx_linux_y`: `taskguide_showtask` / `playertaskchange` / `NewTask` / `taskui_` / `TaskTextOut` = **0 lần**.
⇒ **Bảng F12 là 100% phía client.** Máy chủ chỉ đẩy **giá trị task** xuống; client tự bắn sự kiện `playertaskchange` nội bộ.

API "thông báo" phía máy chủ và tình trạng ở dự án ta:

| Hàm Lua | ELF bản tham chiếu | `Sources\Core\Src\ScriptFuns.cpp` |
|---|---|---|
| `AddNote` | `0x08124DC0` | ✅ có (`:13277`) |
| `Prise` | `0x08124950` | ✅ có |
| `TaskTip` | `0x08122730` | ❌ không |
| `SendTaskOrder` | `0x08117100` | ❌ không |
| `SetStringTask` / `GetStringTask` | `0x081165F0` / `0x081166E0` | ❌ không |
| `SyncTaskValue` / `SyncTaskValueMore` | `0x0810E350` / `0x0810E240` | ⚠️ có tên nhưng là **hàm rỗng** (mục 7) |

---

## 6. HAI THẾ HỆ `taskui_*.lua` — BẪY PORT NẶNG NHẤT

Client **đang phát hành** của ta vẫn ship 7 tệp `taskui_*.lua`, **dịch tiếng Việt tốt hơn** bản tham chiếu. Nhưng chữ ký hàm **lệch một tham số**:

```lua
-- bản tham chiếu (Patch\data\slistcache.pak) — thế hệ mới, MỘT tham số
function showrandraskdesc(nTaskGenre)

-- client ta đang ship (update01.pak)          — thế hệ JX1, HAI tham số
function showrandraskdesc(nTaskIdx, nTaskGenre)
```
Lệch ở **cả 7 tệp** (`showpasstaskdesc_*`, `showkillertaskdesc`, `showmessengerdesc`, `showbwtaskdesc`, `showbwtaskbesttong`, `showworldtaskdesc`).
Trong khi `taskguide.lua` bản tham chiếu gọi `DynamicExecute(guidescript, fullfunc, nId)` — **một** tham số.

**Hệ quả:** bê nguyên bộ `taskui_*.lua` của client ta vào hệ `taskguide.lua` thì `nTaskGenre` = `nil` ⇒ mọi lời gọi `NewTask_TaskTextOut(nTaskGenre, …)` hỏng. Bê ngược lại thì `nTaskGenre` nhận nhầm `nTaskIdx`.
Bằng chứng phụ: `taskui_world.lua` **cả hai bản** đều còn `storm_task(nTaskIdx, nTaskGenre)` — tức bản tham chiếu là bản **đã bị sửa hàng loạt** bỏ tham số đầu và sót mấy hàm này.

⇒ **Phải chọn một thế hệ và đi trọn**, không trộn.

### Các bẫy nhỏ hơn
- **`tasktrace.ini` bị chú thích 100%.** `Ini.Load` vẫn trả về thành công ⇒ không bail, cửa sổ theo dõi dựng với kích thước 0 → **vô hình, không treo**. Bản tham chiếu đã cố ý tắt khung theo dõi.
- **Nút "Bỏ nhiệm vụ" là mã chết ở cả hai đầu.** `taskguide.lua` gửi `ScriptProtocol:SendData("emSCRIPT_PROTOCAL_TaskGuide_QuitTask", handle)` nhưng bảng `KE_SCRIPT_PROTOCOL` (33 mục) **không có** enum đó ⇒ `SendData` im lặng bỏ qua. Phía máy chủ cũng không có người nhận (grep `TaskGuide_QuitTask` toàn cây `server1` = 0). (Chú ý chính tả `PROTOCAL`, không phải `PROTOCOL` — bản gốc gõ sai.)
- **6 mục trong bảng 48 tên cửa sổ trỏ thẳng về nhánh vô hiệu**: `commandline`, `statustool`, `normaltool`, `chatroom`, `giveitem`, `battlerank`.
- **Khe giao diện**: bản tham chiếu dùng `ui3_800` / `ui3_1024`, ta dùng `ui3` phẳng. Không sao — đường dẫn ini được ghép bằng `"%s\\%s"` với thư mục scheme truyền vào, **không gắn cứng**. Đặt tại `Ui\Ui3\uitaskguide\taskguide.ini` là chạy.

---

## 7. 🔴 NÚT THẮT THẬT SỰ: GIAO THỨC ĐỒNG BỘ TASK BỊ CẮT CÒN 1 BYTE

```c
// D:\GAMEDEVNEW\Sources\Core\Src\KProtocol.h:1866-1870
typedef struct // protolcol task
{
    BYTE   ProtocolType;
    BYTE   nTaskId;        //  ⬅️  CHỈ 8 BIT
    DWORD  nTaskValue;
} TASK_VALUE_SYNC;
```
```c
// KPlayerTask.cpp:77-89  — SetSaveVal(int nNo, DWORD bFlag)
sValue.nTaskId = nNo;      // nNo = 1021  ⇒  1021 & 0xFF = 253
g_pServer->PackDataToClient(...);
```
```c
// KProtocolProcess.cpp:3792-3796
Player[CLIENT_PLAYER_INDEX].m_cTask.SetSaveVal(pValue->nTaskId, pValue->nTaskValue);
```

**Ba hệ quả:**

1. **Client không bao giờ nhận đúng biến Dã Tẩu.** Ánh xạ bị cắt: 1021→253, 1025→1, 1027→3, 1028→4, 1030→6, 1031→7, 1032→8.
2. **🔴 Tệ hơn: đang GHI ĐÈ NHẦM ô task của client ngay lúc này.** Mỗi `SetTask(1025,…)` của Dã Tẩu ghi đè ô task **1** phía client, `SetTask(1027,…)` ghi đè ô **3**… Đây là lỗi đang sống, không phụ thuộc việc có làm bảng F12 hay không. (`MAX_TASK = 3000`, `int nSave[3000]` — mảng đủ chỗ; **cắt xảy ra ở dây, không ở mảng**.)
3. **Không có tín hiệu làm mới UI.** `s2cTaskValueSync` chỉ gán giá trị, **không gọi `CoreDataChanged`** ⇒ dù có bảng F12 cũng không có sự kiện `playertaskchange` tương đương.

Cộng thêm: `SyncTaskValue` phía Lua là **hàm rỗng**
```c
// KTongJX2.cpp:4036-4041 — đăng ký tại ScriptFuns.cpp:13423
int LuaJX2_SyncTaskValue(Lua_State* L) { Lua_PushNumber(L, 1); return 1; }   // "JX1 chua co kenh nay"
```

> **📌 Đính chính cho `AUTO_DATAU_SPEC.md` §7.** Dòng *"Đọc trạng thái | s2c_taskvalue (mỗi SetTask server-side) — client giữ bản sao task | KPlayerTask.cpp:83-88"* **chỉ đúng với task id ≤ 255**. Với Dã Tẩu (1021+) thì sai. Ghi chú cũ "client KHÔNG đọc task ≥ 256" là **đúng kết luận nhưng sai lý do**: không phải trần 256 nào cả — thủ phạm là trường `BYTE nTaskId`.

**Thứ tự thi công bắt buộc:** sửa `TASK_VALUE_SYNC` sang `int nTaskId` (đồng bộ **cả hai đầu**, đây là thay đổi phá vỡ giao thức → client và server phải lên cùng lúc) → thêm `CoreDataChanged` khi nhận → rồi mới đến giao diện.

---

## 8. BA LỖI THẬT PHÁT HIỆN KÈM (ngoài phạm vi hỏi, chưa sửa)

**8.1 🔴 Tràn ngăn xếp GameServer trong `LuaAddNote`** — `ScriptFuns.cpp:1736-1743`
```c
g_StrCpyLen(UiInfo.m_pContent, strMain, MAX_SCIRPTACTION_BUFFERNUM);   // kẹp ở 512
int nLen = strlen(strMain);                                            // ⬅️ độ dài CHƯA cắt
*(int*)(UiInfo.m_pContent + nLen) = nParam2;                           // ⬅️ ghi 4 byte tại nLen
```
`m_pContent` là `char[512]` và là **thành viên cuối** của struct (`KProtocol.h:1173`, `MAX_SCIRPTACTION_BUFFERNUM = 512` tại `GameDataDef.h:59`); `UiInfo` là **biến cục bộ trên ngăn xếp**. Bất kỳ `AddNote` nào có chuỗi ≥ 509 ký tự sẽ ghi ra ngoài struct → **phá ngăn xếp GameServer**. Kịch bản Lua điều khiển được độ dài này.

**8.2 Mọi dòng `AddNote` bị cụt 2 ký tự cuối** — `KPlayer.cpp:7800` + `Engine\Src\KStrBase.cpp:255-271`
`g_StrCpyLen` = `strncpy(dest, src, nMaxLen); dest[nMaxLen-1] = 0;` ⇒ giữ tối đa **nMaxLen−1** ký tự. Nơi gọi truyền `nMaxLen = m_nBufferLen − sizeof(int) − 1 = nLen − 1` ⇒ chỉ còn **nLen − 2** ký tự.

**8.3 Ghi 4 byte bộ nhớ chưa khởi tạo vào tệp lưu** — `Ui\UiCase\UiTaskDataFile.cpp:123-125`
`SaveData` ghi `24 + nContentLen − 4` byte tính từ `&pCurrent->Record`, vượt quá phần nội dung 4 byte → đọc phần dư của khối `malloc` (heap slack) rồi ghi ra `MissionMemory.dat`. **Không phải tràn heap** (vẫn nằm trong khối cấp phát), nhưng là rò dữ liệu + nội dung không ổn định. Khối ghi chép **cá nhân** không bị ảnh hưởng.

---

## 9. BA PHƯƠNG ÁN (chưa phải kế hoạch thi công)

| | Phương án | Nội dung | Ước lượng | Rủi ro |
|---|---|---|---|---|
| **A** | **Port hệ `KUiTaskGuide`** (thế hệ bản tham chiếu) | 2 lớp C++ + 6 hàm Lua `NewTask_*` + `GetTask`/`TabFile_*` phía client + mục `"NewTask"` + 5 tệp dữ liệu (4 chép từ bản tham chiếu, `taskguide.lua` rút từ pak) | Lớn nhất | Phải **sửa 7 tệp `taskui_*.lua` bỏ tham số đầu** (mục 6); phá vỡ giao thức ở mục 7 |
| **B** | **Dựng lại hệ thế hệ JX1** (ini 13 section đã ship) | Dùng đúng bộ `taskui_*.lua` 2 tham số đang có sẵn + tệp ini đã ship; viết lớp đọc ini đó | Vừa | Chưa có tệp điều phối tương ứng; phải tự thiết kế phần tra `taskguide.txt` |
| **C** | **Không làm bảng, chỉ vá thông báo** | Sửa giao thức task (mục 7) + thêm `TaskTip`, dùng `AddNote` sẵn có để ghi tiến độ Dã Tẩu vào **sổ tay F11** | Nhỏ nhất | Không phải bảng F12; F12 vẫn là phím chết |

**Việc bắt buộc cho cả ba:** sửa `TASK_VALUE_SYNC` (mục 7) — riêng nó đã là một đợt cần đồng bộ client+server. Với **C** thì đây gần như là toàn bộ khối lượng.

---

## 10. ĐỘ TIN CẬY & NHỮNG GÌ CHƯA KIỂM ĐƯỢC

**Đã kiểm tới byte:** mọi địa chỉ VA/RTTI/bảng nhảy nêu trên; chỉ mục 37 pak (tự kiểm thuật toán băm 74,2%); kích thước và nơi chứa từng tệp; toàn bộ trích dẫn mã nguồn `D:\GAMEDEVNEW`.

**Chưa kiểm được / còn bỏ ngỏ:**
- Chưa **chạy thử trong game** — mọi kết luận về hành vi đều từ đọc mã/byte.
- Chưa giải nén để **so byte** bản `taskui_random.lua` trong pak của ta (5.161 B) với bản tham chiếu (5.154 B rời / 5.141 B trong pak); mới đối chiếu chữ ký hàm và kích thước. Giải nén cần `ucl_nrv2b_decompress_8` (`Engine\Src\XPackFile.cpp:232`).
- `\script\ui\task\tasktrace.lua` và `\UI\UnChangeShortKey.lua`: **không tồn tại ở bất kỳ đâu**.
- Chưa xác định lớp nào đọc tệp ini "hệ thống nhiệm vụ mới" 13 section của thế hệ JX1 (phương án B) — chỉ biết `Game.exe` hiện **không** tham chiếu tên tệp đó.

---

## PHỤ LỤC — ĐƯỜNG DẪN & ĐỊA CHỈ HAY DÙNG

| Thứ | Nơi |
|---|---|
| Bản tham chiếu (client, đã bung) | `D:\ServerLinux\Patch\` — `ui\`, `spr\`, `settings\`, `data\slistcache.pak` |
| Bản tham chiếu (client, nhị phân) | `D:\ServerLinux\Patch\game_y_unpacked.bin` — VA = `0x401000` + offset |
| Bản tham chiếu (máy chủ) | `D:\ServerLinux\server1\jx_linux_y` (ELF32, 5 đoạn PT_LOAD) |
| Client đang phát hành | `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\` |
| Mã nguồn client | `D:\GAMEDEVNEW\Sources\S3Client\Ui\` — `ShortcutKey.cpp`, `UiCase\UiTaskNote.cpp`, `UiCase\UiTaskDataFile.cpp` |
| Mã nguồn máy chủ | `D:\GAMEDEVNEW\Sources\Core\Src\` — `ScriptFuns.cpp`, `KPlayerTask.cpp`, `KProtocol.h`, `KProtocolProcess.cpp` |
| Đọc pak | `Engine\Src\XPackFile.cpp` (header 32 B, mục lục 16 B/mục), `KPakList.cpp:72` (băm tên) |

| Địa chỉ (VA) | Là gì |
|---|---|
| `0x006E9290` / `0x006E92B0` | RTTI `KUiTaskGuide` / `KUiTaskTrace` |
| `0x006E8A0C` / `0x006E8A34` | RTTI `KUiNewTaskMessageList` / `UiNewTaskMapCoordinate` |
| `0x006E7514`–`0x006E75B0` | Khối RTTI họ `KUiTaskNote` + `KUiUpdate_Bulletin` |
| `0x006E4470` (48 mục) | Bảng tên cửa sổ của `Open()`; `NewTask` = chỉ số 28, `tasknote` = 8 |
| `0x004104B0` | Bộ điều phối `Open()`; bảng nhảy `0x00410A58` |
| `0x00543770` / `0x00543850` / `0x005438B0` | `KUiTaskGuide` Open / Close / IsOpen |
| `0x00543C80` | Nơi bắn sự kiện Lua `playertaskchange` |
| `0x0040A1C8` entry[83] → `0x004091EF` | Notify id **84**; trần 169 id |
| `0x00698650`, `0x006E4800` | Chuỗi + bảng đăng ký 6 hàm `NewTask_*` |
| `0x006A54C8`–`0x006A56C0` | Toàn bộ khoá cấu hình của `KUiTaskGuide` / `KUiTaskTrace` |
