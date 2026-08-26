# HƯỚNG DẪN DỊCH NGƯỢC & PORT MỘT TÍNH NĂNG TỪ BẢN LINUX SANG JX1

> Viết 25/08/2026, sau khi port xong 3 hoạt động (Săn boss sát thủ · Phong Lăng Độ ·
> Vượt ải) và hệ **Bắc Đẩu lệnh bài**.
> **Mục tiêu phiên sau: VIÊM ĐẾ BẢO TÀNG và THIÊN LỘC PHÚC** — số liệu đo sẵn ở mục 6.
>
> Đọc hết mục 1→4 trước khi gõ dòng code đầu tiên. Mục 5 là 13 cái bẫy **đã cắn thật**,
> mỗi cái từng tốn nửa buổi.

---

## 1. BA CÂY NGUỒN — ĐỪNG MỞ NHẦM

| Vai trò | Đường dẫn | Ghi chú |
|---|---|---|
| **Bản Linux (nguồn dịch ngược)** | `D:\ServerLinux\server1\` | JX2/Kiếm Thế bản VNG. Script ở `script\`, bảng ở `settings\` |
| **Máy chủ JX1 CHẠY THẬT (đích)** | `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\` | Sửa ở đây là ăn ngay sau khi nạp lại script |
| Client JX1 | `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\` | Có bản **riêng** của `settings\item\magicscript.txt` — sửa bảng item phải sửa **cả hai** |
| Gương git (để commit) | `D:\GAMEDEVNEW\serverscript_jx2\3hoatdong\` | Mọi tệp sửa ở cây E phải chép sang đây rồi mới commit |
| Mã nguồn engine JX1 | `D:\GAMEDEVNEW\Sources\Core\Src\` | Chỉ đụng khi **bắt buộc** (xem bước 3) |

**Luật gương**: mỗi bộ sinh (`gen_*.py`) / bộ vá (`cNN_*.py`) đều ghi vào cây E **và**
`shutil.copyfile` sang gương. Đừng sửa tay ở gương.

`D:\ServerLinux` có **ba** gốc con (`server1`, `gateway`, `Patch`). Kịch bản tính năng
nằm ở `server1`; lịch chạy nằm ở `gateway\...\relaysetting`; 34 bảng máy chủ mà
`server1` không có thì nằm ở `Patch\settings`.

---

## 2. BỘ CÔNG CỤ (đã có sẵn, không phải viết lại)

### 2.1 `dec2.py` — đọc tệp Linux ra chữ đọc được  ⭐ BẮT BUỘC
`D:\GAMEDEVNEW\ReverseTools\port_3hd\dec2.py`

Tệp Linux **trộn hai bảng mã trong cùng một dòng** (tiếng Việt VNI/TCVN + tiếng Trung
GBK). `iconv`/`gbktool.py` giải mã sai. `dec2.decline2(bytes)` cắt từng đoạn rồi chọn
bảng mã theo đoạn.

```python
import importlib.util
spec = importlib.util.spec_from_file_location("dec2", r"D:\GAMEDEVNEW\ReverseTools\port_3hd\dec2.py")
dec2 = importlib.util.module_from_spec(spec); spec.loader.exec_module(dec2)
out = [dec2.decline2(l.rstrip(b"\r")) for l in open(p, "rb").read().split(b"\n")]
```

### 2.2 `recon_tinhnang.py` — ĐO MẶT PHỤ THUỘC  ⭐ CHẠY ĐẦU TIÊN
`D:\GAMEDEVNEW\ReverseTools\port_3hd\recon_tinhnang.py`

```bash
python recon_tinhnang.py "missions\yandibaozang" "event\tianlu_tree"
```

In ra 5 mục, đúng 5 câu hỏi phải trả lời trước khi port:

1. **Tệp + số dòng còn SỐNG** — dòng bị `--` không tính. Nếu 0 dòng sống thì **bản
   Linux cũng không chạy tính năng đó** (Vi Minh Lệnh dính đúng cái này).
2. **Hàm Lua mà JX1 chưa có** — đối chiếu bảng đăng ký `{"Ten", LuaTen}` trong
   `Sources\Core\Src\*.cpp` **và** mọi `function ...` trong cây script JX1.
3. **Include ra ngoài thư mục** — tệp nào JX1 thiếu.
4. **Item dùng trong tính năng** — tra theo `magicscript.txt` **hai bên**, báo
   `KHÔNG CÓ MÃ NÀY` / `KHÁC NGHĨA`.
5. **Task id** — JX1 đã ai dùng chưa.

### 2.3 `syncheck.exe` — kiểm cú pháp Lua 4.0 đúng bản engine
`C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\0064b491-1b33-4139-acb8-6928fda45bcd\scratchpad\luac\syncheck.exe`

Build từ chính `Sources\Library\LuaLib` nên bắt đúng cú pháp Lua **4.0** (khác Lua 5!).
Chạy trên **mọi** tệp .lua vừa sinh. Dòng `(co loi chay - binh thuong khi gia lap)`
là bình thường — nó chỉ báo hàm engine không tồn tại khi chạy ngoài server.

### 2.4 Bộ chữ Việt (kỹ năng `swordonline-dev`)
`C:\Users\nguye\.claude\skills\swordonline-dev\scripts\`

| Việc | Lệnh |
|---|---|
| Đọc tệp JX1 ra chữ Việt | `vn_edit.py <tep> --read [--grep "tu khoa"]` |
| Sửa dòng **có** chữ Việt | `vn_edit.py <tep> --old "..." --new "..."` |
| Sửa dòng **chỉ ASCII** | `safe_edit.py <tep> --old "..." --new "..."` |
| Kiểm tra tệp còn nguyên vẹn | `check_encoding.py <tep>` → **high bytes phải giữ nguyên, FFFD phải = 0** |

Trong bộ sinh Python: `from vn_to_octal import unicode_to_tcvn3_bytes` rồi
`V = lambda s: unicode_to_tcvn3_bytes(s).decode("latin-1")`, ghi tệp bằng
`io.open(p, "w", encoding="latin-1", newline="")`.

Muốn **đọc ngược** (byte TCVN3 → chữ Việt) thì dựng bảng nghịch từ chính hàm trên —
xem `recon_tinhnang.py` hàm `tcvn3()`. **Đừng bao giờ gõ tay một byte `\xNN`.**

### 2.5 Nạp lại script không cần khởi động lại
Lệnh bài admin → mục **6. Nạp lại toàn bộ script**
(`script\tinhnang\3hoatdong\hd3_reload.lua`, sinh bởi `ReverseTools\gen_hd3_reload.py`).

> ⛔ **`?gm RLAS` KHÔNG hoạt động** trên bản build này: `TextGMFilter` chỉ được gọi
> trong `#ifdef _CHAT_SCRIPT_OPEN` mà macro đó không được định nghĩa ở đâu cả. Bảng
> `GM_Command` vẫn nằm trong DLL (dữ liệu tĩnh) nên grep thấy chuỗi nhưng lệnh vô tác dụng.

Thêm tệp mới thì **chạy lại `gen_hd3_reload.py`** để nó vào danh sách nạp.

**Phải khởi động lại máy chủ** khi: đổi `MapList.ini`, đổi `settings\item\*.txt`,
thêm NPC sinh lúc boot, đổi `.dll/.exe`.

### 2.6 Khuôn bộ sinh `gen_*.py` / bộ vá `cNN_*.py`
- **Bộ sinh** (`ReverseTools\gen_*.py`): sinh trọn một tệp .lua, chạy lại được, tệp
  sinh ra có băng-rôn `SINH TU DONG ... DUNG SUA TAY`. Ví dụ đúng khuôn:
  `gen_beidou.py`, `gen_baoruong_thuytac.py`, `gen_satthu_boss_table.py`.
- **Bộ vá** (`ReverseTools\port_3hd\thicong\cNN_*.py`): vá tệp có sẵn. Luật:
  - `assert d.count(neo) == 1` trước khi thay — **không khớp duy nhất thì dừng**.
  - Idempotent: kiểm `if "[NHAN 25/08]" in d: return` để chạy lại không nhân bản.
  - Đọc/ghi `encoding="latin-1", newline=""` để giữ nguyên byte.
  - Sao lưu tệp bảng lớn: `shutil.copyfile(p, p + ".truoc_<viec>_<ngay>")`.

---

## 3. QUY TRÌNH 8 BƯỚC

1. **Chạy `recon_tinhnang.py`** trên thư mục Linux của tính năng. Đọc kỹ 5 mục.
2. **Nếu mục 1 báo `TAT HET`** → bản Linux cũng không chạy tính năng đó. Dừng lại,
   hỏi chủ game có muốn hồi sinh không (chi phí và rủi ro khác hẳn).
3. **Xử hàm thiếu (mục 2)** — theo thứ tự ưu tiên:
   1. Có hàm JX1 **cùng nghĩa** không? (`GetServerDate` → `GetLocalDate`)
   2. Có hàm JX1 trả **nhiều giá trị hơn** không? (`GetItemLevel`/`GetItemSeries` →
      giá trị thứ 4 và 5 của `GetItemProp`, `KJx2WarInfra.cpp:771-793`)
   3. Viết bằng Lua từ hàm sẵn có.
   4. **Cuối cùng mới** thêm hàm C++ → phải build lại `Core.vcxproj`
      `"Server Release"|x64` **và** `"Client Release"|Win32` (tệp trong `Core\Src`
      biên dịch vào **cả hai**), rồi swap DLL + khởi động lại.

   > Bắc Đẩu thiếu đúng 3 hàm và **cả 3 đều xử được bằng Lua** ⇒ port xong mà **không
   > phải build engine**. Luôn cố đạt trạng thái này.
4. **Xử item (mục 4)** — tra **THEO TÊN**, tuyệt đối không dùng lại id của Linux:
   ```python
   # gom bang item JX1 theo ten (nho giai ma TCVN3 truoc khi so)
   # roi doi tbProp cua Linux sang id JX1 cung ten
   ```
   Nếu JX1 **đã có sẵn** item cùng tên (Bắc Đẩu có sẵn cả 16 mã ở 4126–4141!) thì
   **dùng luôn** — khỏi phải đụng bảng item lẫn pak sprite.
   Nếu buộc phải thêm item mới: cấp id sau mã `6,1,*` lớn nhất đang dùng (25/08 là
   **4867**), thêm dòng vào **cả server lẫn client**, và **mượn đường sprite của một
   dòng item đã có** (chắc chắn có trong pak).
5. **Xử task id (mục 5)** — mã nào JX1 đã dùng thì đổi sang mã trống. Đổi rồi phải
   quét lại toàn bộ tính năng.
6. **Viết bộ sinh**, sinh script, chạy `syncheck.exe` **từng tệp**.
7. **Nối vào máy chủ** bằng bộ vá `cNN_*.py`: móc sự kiện, NPC, khoá cấu hình trong
   `script\header\cauhinh_hoatdong.lua`, thêm vào `gen_hd3_reload.py`.
8. **Kiểm & giao**: `check_encoding.py` mọi tệp đã đụng (high bytes không đổi, FFFD=0)
   → chép gương → commit **kèm bằng chứng** (`tệp:dòng` của bản Linux) → nói rõ cái
   gì cần khởi động lại.

---

## 4. QUY ƯỚC CẤU HÌNH

Mọi số liệu người vận hành có thể muốn chỉnh đều phải đi qua
`script\header\cauhinh_hoatdong.lua` và đọc bằng `HD_CFG("TEN_KHOA", <mặc định>)`.
Mặc định **phải bằng đúng giá trị bản Linux**, để không đổi cân bằng khi chưa được yêu cầu.

```lua
local nTiLe = HD_CFG("HD3_PLD_TILE_TRUYCONG", 50)   -- 50 = số của bản Linux
```

---

## 5. MƯỜI BA CÁI BẪY ĐÃ CẮN THẬT

| # | Bẫy | Triệu chứng | Cách đúng |
|---|---|---|---|
| 1 | **`DropItem` khác chữ ký** — Linux `(world,x,y,player,genre,…)`, JX1 `(nNpcIndex,genre,detail,particular,level,series,luck)` (`ScriptFuns.cpp` `LuaDropItem`) | `KItemSet::AddItemSet2 khong tim thay nItemGenre=51584` (= `x*32`), **không rơi đồ** | Đổi sang chữ ký JX1. Cần thả theo toạ độ tự do thì dùng `DropItemEx` (đã thêm, nhận MPS = ô × 32) |
| 2 | **`AddItem` đòi TỐI THIỂU 7 tham số** ở JX1 (Linux 6) | **Im lặng không tạo item** | Luôn truyền đủ 7 |
| 3 | **`GetMSPlayerCount(id, 0)` đếm NHÓM 0**, còn `GetNextPlayer(id,i,0)` coi 0 là TẤT CẢ (`KMission.h`) | Cả trận không ai được `SetFightState(1)` ⇒ **không đánh được** | Bỏ tham số nhóm: `GetMSPlayerCount(MISSIONID)` |
| 4 | **`OpenMission` = TẠO, `CloseMission` = XOÁ** (không phải bật/tắt) | `GetMissionV` trả 0 khi mission chưa tồn tại ⇒ mọi cổng chặn theo giờ vô hiệu | Chặn bằng `IsMission(MISSIONID)` |
| 5 | **`AddNpc` lệch nghĩa tham số 6** (JX1 = ngũ hành, Linux = cờ bNoRevive) | NPC sai hệ / không hồi sinh | Dùng `AddNpcEx` / `HD3_AddNpc` |
| 6 | **Engine JX1 gọi `OnRevive` cho MỌI NPC có script chết**; Linux không định nghĩa | `ScriptError ... cFuncName:(OnRevive)` lặp mỗi nhịp | Thêm `function OnRevive() end` rỗng |
| 7 | **Trùng id item khác nghĩa** — 42 mã đã dính ở đợt 3 hoạt động | Trao nhầm vật phẩm, im lặng | Luôn tra **theo TÊN** (bước 4) |
| 8 | **TCVN3 không có nguyên âm hoa có dấu** (Á, Ú, Ả…) | Bộ mã hoá báo lỗi / chữ hỏng trong game | Chỉ có `Ă Â Đ Ê Ô Ơ Ư`. Viết `Ác` → `ác` hoặc bỏ dấu ở chữ hoa |
| 9 | **Phải có 1 dấu cách trước `<color…>`** (`Text.cpp:468-475` coi byte >0x80 là nửa đầu cặp GBK, nuốt mất `<`) | Người chơi thấy chữ `<color>` trên màn hình | `.." <color=yellow>"` |
| 10 | **Gói hội thoại tối đa 511 byte** (`Describe`/`Say`) | Mất chữ cuối câu | Tách trang |
| 11 | **Bash heredoc rút `\\` thành `\`** | Chuỗi đường dẫn trong script Python sinh ra sai | Nội dung có backslash thì viết tệp bằng công cụ Write rồi mới chạy |
| 12 | **`ReLoadAllScript` chỉ quét `\script` và `\scriptjx2\tong_vn`** (`KSortScript.cpp`), KHÔNG quét `\settings` | Trigger nằm ở `\settings` biến mất sau khi nạp lại | Nạp bù bằng `ReLoadScript("\\settings\\...")` — `hd3_driver` đã làm |
| 13 | **`HD3_DelNpcByScript` dùng `strstr`** (khớp chuỗi con) | Xoá `"hd3_thuyenphu"` xoá luôn `hd3_thuyenphubac.lua` — có khi đúng ý, có khi không | Đặt tên script cho **không phải tiền tố của nhau** khi muốn tách bạch |

---

## 6. HAI TÍNH NĂNG CHO PHIÊN SAU — SỐ LIỆU ĐÃ ĐO (25/08)

### 6.1 THIÊN LỘC PHÚC — **làm cái này trước, nhỏ và sạch**

- **Nguồn**: `D:\ServerLinux\server1\script\event\tianlu_tree\`
  (`awardlist.lua` 54 · `head.lua` 1 · `tianlufudai.lua` 13 · `tianlushuzhizhong.lua` 48 ·
  `tree.lua` 133) — **5 tệp, 249 dòng còn sống**. Đang CHẠY THẬT bên Linux.
- **Hàm JX1 chưa có: 4** → `ChangTreeState`, `GetItemBindState`, `NpcName2Replace`,
  `OpenByConfig`. Kiểm lần lượt theo bước 3 (`GetItemBindState` gần như chắc chắn map
  được sang khoá vật phẩm `InsuranceCourse` mà `HD3_SetItemBindState` đã dùng).
- **Include thiếu: 2** → `\script\lib\progressbar.lua`, `IncludeLib("ITEM")`.
- **Item: 10 mã** — 4 mã KHÁC NGHĨA (`3203`, `3269`, `4134`, `4208`), 6 mã JX1 KHÔNG CÓ
  (`30227/30228/30229` Chân Nguyên Đơn tiểu/trung/đại, `30289` Huyết Long Đằng,
  `30301` Hỗn nguyên chân đơn, `30526` Lệnh Bài Tiến Cử).
  → 5/6 mã này **đã tra được tên sang JX1 ở đợt Bảo Rương Thủy Tặc**, chép lại từ
  `ReverseTools\gen_baoruong_thuytac.py` (30228→4846, 30229→**4847**, 30301→4857,
  30289→3051, 4134 Chân Nguyên Đan→3926).
- **Task id: 4015, 4016 — cả hai TRỐNG.**
- **Móc sẵn có**: `HD3_BD_Co()` + `tbBeidou:PhatLenhBai("thienloc")` đã viết sẵn trong
  `bd_activity.lua`, chỉ việc gọi khi người chơi hái được quả.
  Lệnh bài Bắc Đẩu – Thiên Lộc Phúc = `6,1,4139` (đã có trong bảng item JX1).

### 6.2 VIÊM ĐẾ BẢO TÀNG — **to hơn nhiều, chuẩn bị kỹ**

- **Nguồn**: `D:\ServerLinux\server1\script\missions\yandibaozang\`
  — **26 tệp, ~2.900 dòng còn sống** (`npc.lua` 404, `include.lua` 394+318 hai tệp,
  `ready.lua` 291, `npc_death.lua` 261…). Đang CHẠY THẬT bên Linux.
- **Hàm JX1 chưa có: 18**, trong đó nhóm **đổ xúc xắc chia đồ theo tổ đội** là nhóm
  thật sự khó (`ApplyItemDice`, `AddDiceItemInfo`, `RollItem`, `GetItemDicePlayerList`,
  `GetItemDiceRollInfo`, `GetItemDiceItemInfo`) + nhóm **hầm động động** (`new_dungeon`,
  `new_type`, `PreApplyDungeonMap`, `BuildATeam`, `SubWorldName`).
  → Nhóm xúc xắc **không có tương đương ở JX1**, phải quyết: (a) viết vào engine, hoặc
  (b) bỏ chia-xúc-xắc, rơi đồ thường như các hoạt động khác. **Hỏi chủ game trước.**
  (7 tên còn lại — `id`, `name`, `func`, `series`, `pos`, `new_type`, `YDBZ_proceed` —
  là **nhiễu của công cụ** do bảng Lua đặt tên trùng, bỏ qua.)
- **Include thiếu: `\script\lib\composeclass.lua`, `\script\missions\basemission\dungeon.lua`**
  + 6 `IncludeLib` (`FILESYS`, `ITEM`, `RELAYLADDER`, `SETTING`, `TASKSYS`, `TITLE`).
  `dungeon.lua` là **khung bản đồ phó bản động** — đây mới là khối lượng thật.
- **Item: 34 mã, 36 dòng cảnh báo** → hầu như phải tra tên lại toàn bộ.
- **Task id**: `2618` TRỐNG; **`200` ĐÃ BỊ DÙNG** ở `battlehead.lua`,
  `hoatdong_admin.lua`, `bwhead.lua` → **bắt buộc đổi**.
- **Móc sẵn có**: `tbBeidou:PhatLenhBai("viemde1"/"viemde2")` đã viết sẵn
  (Linux `OnPassYanDiBaoZang`, lệnh bài `6,1,4133`/`4134`).

### 6.3 Thứ tự đề nghị
1. Thiên Lộc Phúc (nhỏ, 4 hàm, task trống) → nối luôn vào Bắc Đẩu.
2. Hỏi chủ game về **xúc xắc chia đồ** của Viêm Đế.
3. Viêm Đế: `dungeon.lua` + `composeclass.lua` trước, rồi mới tới 26 tệp tính năng.

---

## 7. TRẠNG THÁI 2 TÍNH NĂNG ĐỢT 25/08

| Tính năng | Trạng thái | Ghi chú |
|---|---|---|
| **Bắc Đẩu lệnh bài** | ✅ **ĐÃ MỞ** | `gen_beidou.py` + `c49_noi_beidou.py`. Không phải build engine. 4 móc: Phong Lăng Độ · Vượt ải · Tín Sứ · Boss sát thủ. `HD3_BD_TongKim(nDiem)` viết sẵn nhưng **chưa nối** (chưa tìm ra chỗ kết thúc trận Tống Kim của JX1). 141 nhiệm vụ "thu thập item" của Linux **chưa port** (cần tra 141 mã item). |
| **Vi Minh Lệnh** | ⛔ **CHƯA MỞ — cần chủ game quyết** | Xem 7.1 |

### 7.1 Vì sao Vi Minh Lệnh chưa mở

1. **Bản Linux đã gỡ hẳn tính năng này.** `main_function.lua` (380 dòng),
   `viminhlenh.lua` (42), `viminhlenh_task.lua` (614) — **cả 3 bị chú thích 100%,
   0 dòng còn sống**. Bộ điều khiển `activitysys\config\1046\` (6 tệp, **1.450 dòng**)
   cũng **chú thích 100%**. Tổng cộng phải hồi sinh **2.414 dòng chưa từng chạy ở
   trạng thái đó** — không có bản tham chiếu nào để đối chiếu là đúng hay sai.
2. **JX1 không có vật phẩm "Vi Minh Lệnh"** (Linux `6,1,30522`) — phải tạo item mới.
3. **Chức năng chính là phát KỸ NĂNG**: `script\vng_feature\getskills.lua` (91 dòng
   sống) cho người chơi **toàn bộ kỹ năng cấp 10→70 của môn phái + xưng hiệu + hoàn
   điểm tiềm năng + đá ra khỏi game để nạp lại**. Đây là thay đổi cân bằng lớn, phải
   do chủ game quyết chứ không phải tự bật.
4. Hai chuỗi nhiệm vụ (Tụ Nghĩa Vi Minh 107 mốc · Bang Hội Tương Phùng) cần ~20 móc
   rải khắp game qua config 1046 — mà config đó đang chết.

**Đã kiểm sẵn cho phiên sau**: hàm JX1 thiếu = **0**; task `2968/2969/2970/2971` TRỐNG,
riêng **`4000` đã bị `lenhbai_def.lua` dùng** → phải đổi; `skills_table.lua` của JX1
**đã có đủ** `add_em/add_tm/add_sl/...` nên `getskills.lua` port thẳng được;
JX1 có trùng sinh tới **7** (`NTRANSLIFE_MAX`) nên cổng "trùng sinh 4–6, cấp 150–199"
của Vi Minh là thoả được.

Bản đã bỏ chú thích để đọc nằm ở
`…\scratchpad\viminh\` (sinh lại bằng đoạn bỏ chú thích trong phiên 25/08).

---

## 8. DANH SÁCH KIỂM TRA TRƯỚC KHI GIAO

- [ ] `syncheck.exe` sạch trên **mọi** tệp .lua đã sinh/sửa
- [ ] `check_encoding.py`: high bytes không đổi, FFFD = 0, trên **mọi** tệp đã đụng
- [ ] Chạy lại `recon_tinhnang.py` trên **tệp mới sinh** → mục "hàm chưa có" phải rỗng
- [ ] Đã chép sang gương `D:\GAMEDEVNEW\serverscript_jx2\3hoatdong\`
- [ ] Đã chạy lại `gen_hd3_reload.py` nếu có tệp mới
- [ ] Khoá cấu hình mới đã vào `cauhinh_hoatdong.lua`, mặc định = số của bản Linux
- [ ] Thông điệp commit có **tệp:dòng của bản Linux** làm bằng chứng
- [ ] Nói rõ cái gì cần **khởi động lại máy chủ**, cái gì chỉ cần nạp lại script
