# 17 — PHÊ BÌNH ĐỘC LẬP `05_dulieu.md` + `06_phia_jx1.md` (hướng quét 8)

Ngày: 24/08/2026. **Chỉ phân tích** — không sửa một byte nào trong `D:\GAMEDEVNEW\Sources`,
`D:\GAMEDEVNEW\bin`, `E:\SourceTuanLe\...`, và không sửa `00_`…`09_`.

Phương pháp: mở lại tệp gốc / nhị phân / bảng dữ liệu và **đếm lại bằng script**, mặc định coi mọi
con số của vòng 1 là SAI cho tới khi tệp gốc chứng minh ngược lại. Script kiểm chứng để ở
`…\scratchpad\v1.py … v20.py` (đường dẫn đầy đủ ở §5).

---

## 0. TÓM TẮT

| Chỉ số | Kết quả |
|---|---|
| Khẳng định kiểm lại | **36** (20 của `05_dulieu.md` · 16 của `06_phia_jx1.md`) |
| **SAI** | **8** (3 nghiêm trọng: lật hẳn 2/3 “điểm chặn tiến độ”) |
| Lệch nhỏ / thiếu bằng chứng | 5 |
| **Hệ thống KHÔNG báo cáo nào kể tên** | **7 hệ** (1 hệ **hỏng hẳn trên JX1**) |
| Hệ đã kiểm và **loại trừ được** (bằng chứng phủ định) | 2 (TRAP, ACHIEVEMENT) |
| 🔴 **Đã bị ĐỐI CHẤT bác lại** (tác tử độc lập, xem mục cuối tài liệu) | **7 SAI + 1 lệch nhỏ** trên **24** khẳng định được kiểm; **5 chỗ chính tài liệu này bỏ sót** |

**Ba lỗi nghiêm trọng nhất:**

1. 🔴🔴 **CHẶN #1 của `05` (bảng toạ độ spawn Phong Lăng Độ) KHÔNG TỒN TẠI.** Tệp có thật ở
   gốc B: `D:\ServerLinux\Patch\settings\maps\中原北区\渡船\渡船刷怪点.txt` — **893 byte, tiêu đề
   `XPOS<TAB>YPOS`, 63 dòng toạ độ**. Không phải “soạn lại bảng”, chỉ là chép tệp.
2. 🔴🔴 ~~**CHẶN #2 của `05` (thiếu `GetTabFileData`/`GetTabFileHeight`) SAI.**~~
   **[đã sửa theo đối chất]** Cái bị lật là **BS-2**, KHÔNG phải CHẶN #2. `05_dulieu.md:546` định
   nghĩa **CHẶN #2 = thiếu 7 tệp `settings\maps\challengeoftime\lineup{8,16,20,24,32,40,56}.txt`**;
   đối chất đã kiểm cây JX1: `bin\server\settings\maps\challengeoftime\` **không tồn tại**, glob
   `bin\server\**\lineup*` = **0 kết quả** ⇒ **CHẶN #2 VẪN ĐỨNG VỮNG**.
   Phần đúng là: hai hàm `GetTabFileData`/`GetTabFileHeight` **có sẵn** ở
   `E:\...\bin\server\scriptjx2\lib\file.lua:38` và `:46` (vòng 1 chỉ quét 2.811 tệp trong
   `script\`, bỏ 189 tệp `scriptjx2\`) ⇒ **BS-2 SAI**, nhưng nó chỉ là *hệ quả kèm* mà `05` gắn
   vào CHẶN #2 (`05:551-566`), không phải bản thân điểm chặn.
3. 🔴🔴 **Hệ cấm Hồi thành phù của JX1 là một cái vỏ rỗng** — `DisabledUseTownP` là **stub
   `return 0;`** (`KJx2WarInfra.cpp:258-261`). 7 lời gọi `DisabledUseTownP(1)` của Phong Lăng Độ +
   Vượt Ải sẽ **không làm gì**, người chơi thoát khỏi thuyền / Mật Phòng bằng phù (§3.1).
   **Không báo cáo nào trong 00–09 nhắc tới.**

---

## 1. PHÊ BÌNH `05_dulieu.md`

### 1.1 Bảng kiểm chứng (20 khẳng định)

| # | Khẳng định của `05` | Đo lại độc lập | Kết luận |
|---|---|---|---|
| K1 | §2.1 + §8: **“78 map”** (`40 + 7 thành + 5 + 32 + 1, đã trừ trùng`) | Hợp nhất 5 tập: `{40 satthu} ∪ {1,11,37,78,80,162,176} ∪ {336,337,338,339,175} ∪ {464..495} ∪ {957}` = **85**, **KHÔNG có một cặp trùng nào** giữa 5 tập. `map_can_them.csv` cũng cho **85 map id duy nhất** trên 92 dòng (7 dòng lặp là 7 thành ghi 2 lần cho satthu + vuotai) | 🔴 **SAI** — phải là **85**; phép “trừ trùng” đã trừ 7 thành **hai lần** |
| K2 | `map_can_them.csv` **92 dòng** | 93 dòng vật lý = 1 tiêu đề + **92** dòng dữ liệu | ✅ ĐÚNG |
| K3 | §3.2 **“`phonglangdo` — 8 mục”** | `item_can_them.csv` chỉ có **6** dòng `phonglangdo`. Hai bộ ba thân bài trích dẫn — **`6,1,2745`** (Lệnh Bài Thủy Tặc, `fld_head.lua:274`) và **`4,489`** (Lệnh bài Phong Lăng Độ, `fld_head.lua:269`) — **không hề có trong CSV** (`4,489,1` chỉ xuất hiện dưới nhãn `vuotai`) | 🔴 **SAI** (bảng kèm không khớp thân bài) |
| K4 | §8: “Bộ ba vật phẩm 3 tính năng dùng — **83 dòng** trong `item_can_them.csv`” | Đúng **83 dòng dữ liệu**, nhưng chỉ **73 bộ ba DUY NHẤT** (10 dòng lặp giữa các tính năng). Cột `KetLuan`: 41 `TRUNG ID` · **25 `THIEU`** · 17 `ANH XA` | ⚠️ Đúng chữ, **dễ đọc nhầm** thành 83 vật phẩm |
| K5 | §1.3 “**141/141** NPC id đã tồn tại ở JX1, trùng tên trừ 3 ô” | Đọc thẳng 2 bảng `npcs.txt`: LINUX **2.353** dòng dữ liệu/103 cột, JX1 **2.035**/87 cột. Cả **141** id đều nằm trong bảng JX1, **lệch tên đúng 3 ô**: 1032/1033/1034 | ✅ ĐÚNG |
| K6 | `npc_can_them.csv` **140 dòng** | Đúng 140 — **thiếu chính id 769**. Thân bài nói “141/141 đã kiểm”, bảng kèm chỉ chứng minh 140 | ⚠️ lệch nhỏ (lỗ tài liệu) |
| K7 | §1.3 “`npcs.txt` JX1 **không có dòng trống nào** (0/2035)” | Đếm cột `Name` rỗng: **0/2035** ở JX1, **0/2353** ở LINUX | ✅ ĐÚNG |
| K8 | §1.2 bảng `addkillertasknpc` = 160 dòng, **59 npc id `761…768, 770…820`**, **40 map**, 8 cấp | Bóc lại `killbosshead.lua` dòng 4…180: **160 hàng khớp**, **59 id**, id khuyết duy nhất trong dải là **769**, **40 map id**, cấp `{25,35,45,55,65,75,85,95}` | ✅ ĐÚNG |
| K9 | §5.5 + §6 **CHẶN #1**: `渡船刷怪点.txt` “KHÔNG CÓ ở cả hai cây và mọi pak” | **CÓ** ở `D:\ServerLinux\Patch\settings\maps\中原北区\渡船\渡船刷怪点.txt` — 893 byte, header `XPOS\tYPOS`, **63 dòng toạ độ**, giá trị **pixel** (50400 = 1575×32 … 53152 = 1661×32; Y 101184…103168 = ô 3162…3224) | 🔴🔴 **SAI** — xem §1.2 |
| K10 | **BS-2** (KHÔNG phải CHẶN #2 — xem §0 mục 2 đã sửa): “`GetTabFileData`/`GetTabFileHeight` **0 kết quả** trong 2.811 tệp `.lua` của JX1” | `scriptjx2\lib\file.lua:38` `function GetTabFileHeight(mapfile)`, `:46` `function GetTabFileData(mapfile, row, col)` — **CÓ**. **[đã sửa theo đối chất]** Nhưng “engine nạp cả cây `scriptjx2\`” là **dẫn nhầm dòng**: `KSortScript.cpp:152-161` nằm trong `LoadScriptToSortListA` (nạp *một* tệp bất kỳ); chỗ **liệt kê thư mục lúc boot** là `g_IniScriptEngine` `:56` `LoadAllScript("\script")` + `:65` `LoadAllScript("\scriptjx2\tong_vn")` — **`scriptjx2\lib\` KHÔNG được liệt kê lúc boot**, nó chỉ tới được qua `Include`/`IncludeLib` | 🔴🔴 **SAI** (BS-2) |
| K11 | §6 **CHẶN #3** = “**16 dòng**” vật phẩm | Mâu thuẫn nội bộ: §3.2 liệt kê **11** mục genre 6 + **3** ngựa = **14**; §8 ghi **14**; §6 ghi **16** vì cộng thêm 2 lời gọi *đổi DetailType* (không phải dòng bảng mới) | 🔴 **SAI** — đúng là **14 dòng mới** |
| K12 | §3.2 “**thiếu hẳn** 11 mục genre 6: 30009, 30010, 30528, 30530–30536, 30538” | Tra `magicscript.txt` JX1 theo **cả id lẫn TÊN**: cả 11 mục **không có id, cũng không có tên** ở JX1 | ✅ ĐÚNG (đã kiểm 2 chiều) |
| K13 | §3.1 “LINUX `horse.txt` 331 dòng, `magicscript.txt` (004) …” — dùng thư mục `settings\item\004\` | LINUX có **5 thế hệ** `settings\item\{000,001,002,003,004}` + một bộ **phẳng** ở `settings\item\`. Bảng phẳng `horse.txt` chỉ tới `ParticularType` **7**; bảng `004` tới **32**. Engine chọn thư mục qua chuỗi `"/settings/item/%03d"` (`jx_linux_y` offset **`0x20B379`**, VA `0x08253379`, nạp tại VA `0x08070E3B`, số hiệu thế hệ là **tham số hàm** `[ebp+0xc]`). Thế hệ 004 là bản duy nhất có `magicscript.txt` 1,15 MB ⇒ chắc chắn là bản đang chạy | ⚠️ **Kết luận đúng, thiếu bằng chứng** — báo cáo không nói vì sao chọn `004`; nếu chọn nhầm thế hệ, mục “thiếu 3 ngựa 19/20/21” đổi hoàn toàn |
| K14 | BS-4: “`magicscript.txt` LINUX **30 cột** ↔ JX1 **14 cột**” | Đếm thật: **cả hai đều 30 cột** ở dòng tiêu đề — JX1 chỉ **đặt tên** cho 14 cột đầu, 16 cột sau để trống tên. Dòng dữ liệu JX1 **răng cưa**: gặp cả 14/15/17/30 cột. Khác biệt THẬT là **lệch NGỮ NGHĨA từ cột 10**: LINUX `…, 说明文字(9), 五行属性(10), 价格(11), 等级(12), 是否叠放(13), 脚本名(14)` ↔ JX1 `…, Intro(9), Script(10), Price(11), ShortKey(12), nMaxStack(13), PickExecute(14)`. Ví dụ thật `(6,1,22) Tẩy Tuỷ Kinh`: LINUX cột 14 = `\script\item\xisui-jing.lua`; JX1 **cột 10** = chính chuỗi đó. **JX1 không có cột 五行属性** | 🔴 **SAI** (số cột) — **kết luận “không copy-paste được dòng” vẫn đúng nhưng vì lý do khác** |
| K15 | BS-5: “**4.352** bộ ba `(6,d,p)` chung, **188 (4,3 %)** trùng tên” | Đo lại: giao = **4.352** ✅; trùng tên **chính xác từng ký tự** = **185**. Chênh 3 là do khoảng trắng thừa (vd `6,1,72` LINUX `"Thiên sơn  Bảo Lộ"` ↔ JX1 `"Thiên sơn Bảo Lộ"` — cùng vật phẩm) | ✅ ĐÚNG (sai số 3) |
| K16 | §3.2 “42 bộ ba trước ghi ‘trùng id’: chỉ **5 trùng tên**, **37 lệch tên**” | Kiểm độc lập trên **61 bộ ba genre-6** mà 3 tính năng dùng: **24 thiếu hẳn ở JX1**, **3 trùng tên tuyệt đối** (+1 nếu bỏ qua khoảng trắng), **34 lệch tên**. Bảng lệch đầy đủ ở §5 (`v20.py`). Kết luận “genre 6 BẮT BUỘC tra theo TÊN” **đứng vững** | ✅ ĐÚNG về bản chất |
| K17 | §4.1 `MAX_TASK = MAX_TEMP_TASK = 4200` (`KPlayerTask.h:18-19`); id lớn nhất dùng 4018 | Khớp | ✅ ĐÚNG |
| K18 | §4.2 + BS-2 “Đã quét **2.811** tệp `.lua` của JX1” | `find script -iname "*.lua"` = **2.811** ✅ **nhưng** `scriptjx2\` còn **189** tệp nữa và **cũng được nạp**. Tổng thật = **3.000** | 🔴 **SAI phạm vi** — chính là gốc của K10 |
| K19 | §7 `killbosshead.lua` “giống hệt từng byte, MD5 `159b2ace…`” | `md5sum` hai cây: **`159b2acec0344bf965785cc346460f8e`**, 384.819 B cả hai | ✅ ĐÚNG |
| K20 | §5.6 “`ITEM_DropRateItem`→`DropRateItem`, `NPCINFO_GetSeries`→`GetNpcSeries`, `AddExp_Skill_Extend` **không có**” | Bóc lại toàn bộ **1.005** dòng đăng ký của `GameScriptFuns[]`: `DropRateItem` **CÓ**, `GetNpcSeries` **CÓ**; `AddStatData`, `ITEM_SetExpiredTime`, `NpcName2Replace`, `AddExp_Skill_Extend`, `DropItemEx`, `NpcDropMoney` **đều KHÔNG** | ✅ ĐÚNG |

**05: 20 khẳng định — 13 ĐÚNG, 5 SAI, 2 lệch nhỏ/thiếu bằng chứng.**

### 1.2 🔴🔴 LẬT CHẶN #1 — bảng toạ độ Phong Lăng Độ CÓ THẬT

Vòng 1 (và cả phụ lục phản biện của nó) kết luận sai vì **chỉ biết gốc A** (`D:\ServerLinux\server1`).
Gốc B (`D:\ServerLinux\Patch`) có **50 tệp bảng tên chữ Hán** dưới `settings\maps\` mà gốc A **hoàn
toàn không có** (gốc A: 29 thư mục con, tất cả tên ASCII — điều này thì `05` ghi đúng).

```
D:\ServerLinux\Patch\settings\maps\中原北区\渡船\渡船刷怪点.txt      893 B, 63 dòng toạ độ
D:\ServerLinux\Patch\settings\maps\中原北区\风陵渡北岸\baiyingyingboss.txt   99 B
D:\ServerLinux\Patch\settings\maps\中原北区\风陵渡北岸\yanxiaoqianboss.txt   99 B
D:\ServerLinux\Patch\settings\maps\中原北区\风陵渡南岸\herenwoboss.txt      143 B
```

Nội dung thật (đã giải mã, `v8.py`):

```
XPOS<TAB>YPOS
50400   102240
50560   102048
50752   101856
...
53152   103072          ← 63 dòng dữ liệu
```

**Hệ quả trực tiếp:**

* Không phải “soạn lại bảng”, chỉ **chép 1 tệp**. Bỏ hẳn hạng mục “điểm chặn tiến độ cứng” của `05`.
* Toàn bộ đoạn hướng dẫn dựng lại toạ độ ở §6 CHẶN #1 (“vùng chơi quanh ô 1630…1650 × 3220…3235”)
  **SAI vùng**: bảng thật trải ô **X ≈ 1575…1661**, **Y ≈ 3162…3253**
  **[đã sửa theo đối chất — bản trước ghi “Y ≈ 3162…3224”, SAI]**. Đo lại bằng `awk … | sort -n`:
  X `50400…53152` (÷32 = **1575…1661**), Y `101184…**104096**` (÷32 = **3162…3253**). Giá trị Y lớn
  nhất là **104096** ở dòng 57, không phải 103168. Nếu ai đó đã theo hướng dẫn cũ để tự đặt 30 thuỷ
  tặc thì quái sẽ sinh **lệch hẳn khỏi vùng thuyền**.
* Số dòng thật là **63**, không phải “≥ 30” như suy đoán.

**Chỉ dẫn kỹ thuật khi chép** (đây là bẫy đã làm hỏng bằng chứng của cả vòng 1): tên thư mục trên
đĩa là **byte GBK thô**; Windows/Python trả về chuỗi mojibake latin-1. Muốn tìm phải duyệt cây rồi
`os.fsencode(tên).decode('gbk')` hoặc `tên.encode('latin-1').decode('gbk')` — `os.path.join` với
chuỗi Unicode tiếng Trung luôn báo “không tìm thấy tệp”.

### 1.3 🔴🔴 LẬT CHẶN #2 — hàm đọc bảng đã có sẵn ở `scriptjx2\`

```
E:\...\bin\server\scriptjx2\lib\file.lua:38   function GetTabFileHeight(mapfile)
E:\...\bin\server\scriptjx2\lib\file.lua:46   function GetTabFileData(mapfile, row, col)
```

~~Việc thật rút xuống còn: **sửa đường dẫn `Include`** (`\script\lib\file.lua` → `\scriptjx2\lib\file.lua`)
**hoặc** chép 1 tệp giữa hai cây của chính JX1 — không phải “port từ Linux”.~~

🔴 **[đã sửa theo đối chất] — KHÔNG PHẢI LÀM GÌ CẢ.** Engine JX1 đã có sẵn đường tự chuyển tiếp:

```c
// D:\GAMEDEVNEW\Sources\Core\Src\ScriptFuns.cpp:1938-1967
static void sJX2RemapScriptPath(char* szFull)
{
	FILE* f = fopen(szFull, "rb");
	if (f) { fclose(f); return; }             // ban goc CO -> giu nguyen
	static const char* szFrom[2] = { "script\\tong\\", "script\\lib\\" };
	static const char* szTo[2]   = { "scriptjx2\\tong_vn\\", "scriptjx2\\lib\\" };
	...                                        // doi tien to roi fopen lai
}
```

Hàm này được `LuaIncludeFile` gọi ở **`ScriptFuns.cpp:2016`**, ngay trước `lua_dofile` (`:2020`), và
đường dẫn lúc đó đã là **TUYỆT ĐỐI** (`g_GetRootPath`, `:1998-2004`) nên `fopen` phán đoán đúng.
Đã kiểm: `E:\...\bin\server\script\lib\file.lua` **KHÔNG tồn tại** (thư mục `script\lib\` có 27 tệp,
không có `file.lua`) ⇒ mọi `Include("\\script\\lib\\file.lua")` **tự động** rơi sang
`scriptjx2\lib\file.lua`. **Không phải sửa Include, không phải chép tệp.**

Hệ quả kèm: “chứng cứ phụ” của `05:565-566` (*`missions\tong\collectgoods\npcpoint.lua:8,14,15` của
chính cây JX1 đang gọi `GetTabFileData` ⇒ đoạn đó hiện cũng hỏng/chết*) **cũng SAI** — nó chạy được
nhờ đúng cơ chế remap này. **Cả `05` lẫn bản vòng 2 này đều bỏ sót `sJX2RemapScriptPath`.**

Đo lại `closure3.json` cũng ra **đúng con số của `06`**: **85** tệp `.lua` duy nhất mà JX1 thiếu
(satthu 28 / phonglangdo 44 / vuotai 54), trong đó **15 tệp nằm sẵn ở `scriptjx2\`**:

```
lib\basic.lua        lib\composeclass.lua   lib\composelistclass.lua  lib\coordinate.lua
lib\droptemplet.lua  lib\file.lua           lib\mem.lua               lib\progressbar.lua
lib\sharedata.lua    lib\string.lua         tong\contribution_entry.lua
tong\log.lua         tong\tong_award_head.lua  tong\tong_header.lua   tong\tong_setting.lua
```

và đúng **1** tệp không tồn tại ở bất kỳ đâu: `global\路人_礼官.lua` (`fengling_ferry\boss.lua:11`).
⇒ **`06_phia_jx1.md` đúng, `05_dulieu.md` sai** ở điểm này.

### 1.4 Gốc B **KHÔNG** lật phần còn lại của `05` (đã kiểm để chốt)

| Bảng | server1 | Patch | Kết quả |
|---|---|---|---|
| `settings\item\004\magicscript.txt` | 1.151.203 B | 1.151.203 B | **MD5 TRÙNG** |
| `settings\item\004\horse.txt` | 69.928 B | 69.928 B | **MD5 TRÙNG** |
| `settings\item\004\questkey.txt` | 236.152 B | 236.152 B | **MD5 TRÙNG** |
| `settings\item\004\goldequip.txt` | 1.411.890 B | 1.411.890 B | **MD5 TRÙNG** |
| `settings\npcs.txt` | 739.550 B | 739.550 B | **MD5 TRÙNG** |
| `settings\maplist.ini` | 193.907 B | 193.907 B | **MD5 TRÙNG** |
| `settings\maps\challengeoftime\lineup*.txt` | 7 tệp | 7 tệp | có ở cả hai |
| `settings\task\tollgate\killer\killer.txt` | 14.033 B | 14.033 B | có ở cả hai |
| `settings\droprate\boss\bosstask_lev20..90.ini` | 8 tệp | 8 tệp | có ở cả hai |
| `settings\maps\liandandong\` | 5 tệp | 5 tệp | có ở cả hai |
| `script\huoyuedu\huoyuedu.lua` | **7.712 B — CÓ** | không có | chép từ A |
| `script\task\tollgate\killer\`, `missions\fengling_ferry\`, `missions\challengeoftime\` | có | **Patch KHÔNG đè** | phần script không bị ảnh hưởng |

`Patch\script\` chỉ có 634 tệp trong 9 thư mục (`activitysys 4 · event 1 · global 3 · item 1 ·
skill 542 · task 2 · tong 66 · ui 2 · vng_event 13`) và **không đụng** bất kỳ tệp lõi nào của 3
tính năng. ⇒ **Mọi kết luận phía script của `05` và `06` không bị gốc B lật.** Chỉ **một** kết luận
bị lật, đúng như §1.2.

---

## 2. PHÊ BÌNH `06_phia_jx1.md`

### 2.1 “Hệ mission JX1 có đủ ô không?” — đếm lại số ô biên thật

| Hằng số | Giá trị đo lại | Tệp:dòng |
|---|---|---|
| Mission / 1 map | `MAX_TIMER_PERMISSION = 10` → **thực dùng 9** | `KMission.h:23`; `KLinkArrayTemplate.h:82` (`m_ulTotalSize = ulSize - 1`) |
| Mission toàn cục | `MAX_GLOBAL_MISSIONCOUNT = 50` | `KSubWorld.h:23,25` |
| Mission / subworld | `MAX_SUBWORLD_MISSIONCOUNT = 10` | `KSubWorld.h:22` |
| **Ô biến số** | `int m_MissionValueC[100]` | `KMissionArray.h:13` |
| **Ô biến chuỗi** | `char m_MissionValue[100][16]` | `KMissionArray.h:14` |
| NPC / mission | `MAX_NPC_MISSION = 5000` | `KMission.h:25,120` |
| Người / mission | `MAX_PLAYER = 1500` (server) | `KMission.h:119`; `KPlayerDef.h:19` |
| Tham số / người | `MAX_MISSION_PARAM = 18` | `GameDataDef.h:87` |
| Ladder nội bộ | `MISSION_STATNUM = 10` | `GameDataDef.h:91` |
| Trần map | `MAX_SUBWORLD = 1000` | `KSubWorld.h:5` |
| Trần NPC | `MAX_NPC = 98000` | `KNpc.h:21` |

**Ô cao nhất 3 tính năng cần**: Vượt ải `VARV_BATCH_MODEL = 33` (tính lại từ `include.lua:17,50-76`:
`VARV_PLAYER_SEX=13` → `+8+1 = VARV_PLAYER_SERIES=22` → `+8+1 = VARV_LEVEL=31` → `+2 = 33`);
Phong Lăng Độ `MS_TIMEACC_20SEC = 3` (`fld_head.lua:28-30`). **33 ≪ 100 ⇒ ĐỦ, xác nhận `06` đúng.**

**Bổ sung `06` chưa nói:** JX1 có **HAI kho tách rời** — `m_MissionValueC[100]` (số nguyên, chỉ dùng
qua `SetGlbMissionV`/`GetGlbMissionVC`, `ScriptFuns.cpp:11067`/`11113`) và `m_MissionValue[100][16]`
(chuỗi, là kho mà `SetMissionV`/`GetMissionV`/`GetMissionS` dùng — số được **lưu dưới dạng chuỗi**
rồi `atoi` khi đọc, `KMissionArray.h:40,48-49`).

### 2.2 “Ba khiếm khuyết của hệ mission JX1” — **cả 3 đều CÓ THẬT**

| # | Khiếm khuyết | Bằng chứng đo lại |
|---|---|---|
| 1 | `strcpy` không kiểm độ dài vào `char[16]` | `KMissionArray.h:36-41` → dòng **40** `strcpy(m_MissionValue[ulValueId], szValue);`, chỉ chặn `ulValueId >= 100`. `KMission.h:334-339` y hệt (dòng **338**). Và `LuaSetMission` lấy tham số 2 bằng **`Lua_ValueToString`** (`ScriptFuns.cpp:11002`) ⇒ mọi chuỗi > 15 ký tự **tràn sang ô kế**. `SetMissionV` **dùng chung đúng hàm này** (`ScriptFuns.cpp:15231` `{ "SetMissionV", LuaSetMission }`) ✅ |
| 2 | Phạm vi biến là **MAP**, không phải mission | `ScriptFuns.cpp:11007` `SubWorld[nSubWorldIndex].m_MissionArray.SetMission(...)`; `:11049` `…GetMissionValue(...)` ✅ (đúng cả 2 số dòng) |
| 3 | `GetMissionV` bỏ ô 0 | `ScriptFuns.cpp:11048` `if (nValueId > 0)` ↔ `:11004` `if (nValueId < 0) return 0;` ✅ |

**Hai điểm `06` bỏ sót ngay trong cùng vùng mã:**

* **KĐ-a.** ~~`GetMissionS(0)` đọc được ô 0 trong khi `GetMissionV(0)` luôn trả 0.~~
  🔴 **[đã sửa theo đối chất — kết luận này TỰ MÂU THUẪN với §2.6 của chính tài liệu này]**
  Đúng là `LuaGetMissionString` (`ScriptFuns.cpp:11023`) dùng `if (nValueId >= 0)` trong khi
  `LuaGetMissionValue` (`:11048`) dùng `> 0` và `LuaSetMission` (`:11004`) dùng `< 0`. **Nhưng tên
  `GetMissionS` KHÔNG trỏ tới `LuaGetMissionString` lúc chạy**: `ScriptFuns.cpp:14740`
  `{"GetMissionS", LuaGetMissionString}` bị **`:15233` `{ "GetMissionS", LuaJx2GetMissionString }`
  đè** (mục sau thắng — chính §2.6 dưới đây chứng minh). ⇒ nhánh `>= 0` ở `:11023` **không tới được
  qua tên `GetMissionS`**; “ba luật biên khác nhau” chỉ đúng trên giấy. Điều còn giá trị: **nếu ai
  đảo thứ tự hai cụm đăng ký** thì luật biên đổi im lặng — ghi lại như một cái bẫy thứ tự, **không
  phải một khiếm khuyết đang sống**.
* **KĐ-b.** `memset(m_MissionValueC, …)` bị **chú thích ở CẢ HAI chỗ khởi tạo** — `KMission.h:144`
  (constructor) và `KMission.h:167` (`Init()`).
  🔴 **[đã sửa theo đối chất — BẰNG CHỨNG LỆCH LỚP]**: hai dòng ấy nằm trong lớp **`KMission`**, mà
  `KMission` **không hề có trường `m_MissionValueC`** (grep `KMission.h`: chỉ có `:126`
  `char m_MissionValue[MAX_MISSIONARRAY_VALUE_COUNT][16];`). Đó là **hai dòng chết**, bị chú thích
  vì nếu bỏ chú thích sẽ **không biên dịch được** — không phải một lỗ khởi tạo.
  Trường thật tên `m_MissionValueC` nằm ở lớp **khác**: `KMissionArray.h:13`
  `int m_MissionValueC[MAX_MISSIONARRAY_VALUE_COUNT];`. Nhận xét *vẫn còn giá trị* là phần sau:
  `KMissionArray` **không có constructor riêng**, nên kho ấy chỉ sạch nhờ `g_GlobalMissionArray` là
  biến **toàn cục** (`KSubWorld.cpp:47`, nằm BSS ⇒ nạp bằng 0); đặt một `KMissionArray` trên
  stack/heap thì `GetGlbMissionVC` trả rác. **Số dòng phải trích là `KMissionArray.h:13`, không phải
  `KMission.h:144,167`.**

### 2.3 “Hệ timer — 2 cái bẫy” — **cả 2 đều CÓ THẬT**, và có **bẫy thứ 3**

| Bẫy | Kiểm chứng |
|---|---|
| **1 — rò khe timer** | `LuaStartMissionTimer` `ScriptFuns.cpp:11363-11391`; dòng **11382** gọi thẳng `m_cTimerTaskSet.Add()`, **không có vòng dò `(missionId,timerId)` trùng**. Đối chứng: `LuaStartGlbMSTimer` (`KJx2League.cpp:974-982`) **có** vòng dò và ghi đè ✅ |
| **2 — `OnTimer` không nhận timer id, script tra qua bảng ngoài** | `KTaskFuns.cpp:117-132` (`Activate`) → `:184-185` tra `TimerTask.txt`; `KMission.cpp:338-366` gọi `OnMissionTimer`, không có thì `OnTimer` với tham số **0** (`:362` → `:97`) ✅. `TimerTask.txt` hiện **36 dòng dữ liệu**, id đang dùng `1-10,12-18,20,21,50-55,61,62,65-70,75-77` ⇒ **28, 29, 41, 42, 43 đều còn trống** ✅ |
| **3 🔴 MỚI — glb timer chỉ bắn TỐI ĐA 16 cái/tick** | `KJx2League.cpp:1426` `for (size_t i = 0; i < s_GlbTimers.size() && nFireCount < 16; i++)`. Vòng **thoát ngay** khi đủ 16, nên các timer đứng sau trong vector **không được xét** tick đó (không mất hạn, nhưng bị dồn). `06` §2(b) giới thiệu `StartGlbMSTimer` như lựa chọn thay thế mà không nêu trần này |

**Kiểm thêm và loại trừ:** `g_GlobalMissionArray.Activate()` **bị chú thích** ở
`KSubWorldSet.cpp:102`. Đã truy: glb timer **không** dùng container đó mà dùng vector riêng
`s_GlbTimers` (`KJx2League.cpp:925`) do `KJx2GlbMission_Breathe()` (`:1419`) đập nhịp từ
`CoreServerShell::Breathe`. ⇒ dòng bị chú thích **vô hại**, `g_GlobalMissionArray` chỉ còn là kho biến.

### 2.4 “Điều kiện bật tổ đội: danh sách tiền tố HARDCODE” — CÓ THẬT, chép nguyên văn

`D:\GAMEDEVNEW\Sources\Core\Src\KSortScript.cpp:113-133`:

```c
int g_IsJx2Script(Lua_State* L)
{
	static const char* szJx2[] = {
		"\\script\\missions\\citywar_", "\\script\\missions\\leaguematch\\", "\\script\\leaguematch\\",
		"\\script\\missions\\tong\\", "\\script\\task\\tollgate\\", "\\script\\item\\messenger\\",
		"\\script\\item\\xinshirenwu\\", "\\script\\missions\\tongwar\\", "\\script\\event\\tongwar\\",
		"\\script\\missions\\bw\\", "\\script\\missions\\bairenleitai\\", "\\script\\missions\\tongcastle\\",
		"\\script\\missions\\arena\\", "\\script\\activitysys\\", "\\script\\tong\\", "\\scriptjx2\\",
		"\\script\\global\\npcchucnang\\dichquan.lua",	// Dich Quan 7 thanh: Include posthouse.lua (Tin Su)
		"\\script\\missions\\basemission\\",	// [PORT5 23/08 phan bien F7] lib JX2 (CallNpc bNoRevive)
	};
	const char* szName = g_GetScriptNameByState(L);
	if (!szName || !szName[0])
		return 0;
	for (int i = 0; i < (int)(sizeof(szJx2) / sizeof(szJx2[0])); i++)
	{
		if (strstr(szName, szJx2[i]) != NULL)
			return 1;
	}
	return 0;
}
```

**18 mẫu, so bằng `strstr` (chuỗi con).** Ba tính năng lọt qua thế nào:

| Tính năng | Đường dẫn script | Lọt? |
|---|---|---|
| Săn boss sát thủ | `\script\task\tollgate\killer\*.lua` | ✅ **lọt sẵn** nhờ mẫu `"\\script\\task\\tollgate\\"` |
| Phong Lăng Độ | `\script\missions\fengling_ferry\*.lua` | ❌ **KHÔNG lọt** — phải thêm mẫu |
| Vượt ải | `\script\missions\challengeoftime\*.lua` | ❌ **KHÔNG lọt** — phải thêm mẫu |

⚠️ **Bổ sung `06` chưa nói rõ hệ quả**: cờ này lấy theo **tệp SỞ HỮU Lua state**, không phải tệp
đang chạy (chú thích ngay trên hàm, `KSortScript.cpp:110-112`: *“`Include()` dùng state của tệp gọi
nên NPC JX1 gọi luồng JX2 … phải liệt kê”*). Vì vậy nếu móc 3 tính năng vào bằng `Include` từ
`startgame.lua` hay `timerserver.lua` (đều là script JX1), **thân hàm chạy dưới state của tệp JX1
⇒ vẫn ăn ngữ nghĩa JX1** dù đã thêm mẫu. Chỉ `DynamicExecute` / script trỏ từ `TimerTask.txt` /
`missions.txt` / `SetNpcScript` mới tạo state riêng theo đúng đường dẫn. Đây là **cách móc dây bắt
buộc**, không phải tuỳ chọn.

### 2.5 “`activitysys` thiếu 803/809 tệp và CHƯA KHỞI ĐỘNG” — ĐÚNG, đếm lại khớp

| Đo | Kết quả |
|---|---|
| `D:\ServerLinux\server1\script\activitysys\**\*.lua` | **809** |
| `E:\...\bin\server\script\activitysys\*.lua` | **6** (`activity`, `activitydetail`, `functionlib`, `g_activity`, `ladderfunlib`, `playerfunlib`) |
| Thiếu | **803** ✅ |
| `detailtype\` bản Linux | **32** tệp ✅ |
| `config\` trong **bao đóng** 3 tính năng | đúng **6** tệp: `config/32/{dailytask,head,talkdailytask,variables}.lua` + `config/1005/{check_func,partysupport}.lua` ✅ (thư mục đầy đủ thì `config/32` có 11 tệp, `config/1005` có 22 — `06` nói “4 tệp / 2 tệp” là **theo bao đóng**, không sai) |
| Điểm gọi `G_ACTIVITY:LoadActivitys()` ngoài định nghĩa | **0** ✅ |
| `Patch\script\activitysys\` | chỉ **4** tệp `config/1005/ruong_*.lua` + `config/33/tumangchibao.lua` — **không có** `npcfunlib.lua` hay `detailtype\` |

### 2.6 “`ScriptFuns.cpp` — số dòng thật hôm nay” — ĐỌC LẠI, **không đổi**

```
D:\GAMEDEVNEW\Sources\Core\Src\ScriptFuns.cpp
427.894 byte · 15.348 dòng · sửa đổi 2026-08-23 16:03:05
```

Mốc chèn kiểm lại từng dòng: **14255** `#endif // _SERVER (khoi ham WLLS)` · **14257**
`TLua_Funcs GameScriptFuns[] =` · **15315** `#else` · **15318** `#endif` · **15323** `};` ·
**15325** `TLua_Funcs WorldScriptFuns[] =` · **15340-15343** `g_GetGameScriptFunNum()`. **Tất cả khớp.**

Đếm lại bảng đăng ký (bỏ `//` và `/*…*/`): **1.005 dòng đăng ký / 1.002 tên duy nhất**, trùng đúng
3 tên: `BT_GetData`, `GetLeadLevel`, `GetMissionS` ✅ (khớp bản đã sửa của `06`).

⚠️ **Điểm `06` khẳng định mà chưa chứng minh**: “mục sau đè mục trước”. Đã truy: đăng ký chạy qua
`KLuaScript::RegisterFunctions(GameScriptFuns, g_GetGameScriptFunNum())` (`KSortScript.cpp:152`,
`KPlayer.cpp:7405`) — duyệt tuần tự mảng, nên **mục sau thắng**. Cụ thể `GetMissionS` ở **14740**
(`LuaGetMissionString`, đọc `m_MissionValue[100][16]`) bị **15233** (`LuaJx2GetMissionString`, kho
chuỗi riêng) đè. **Nếu ai đảo thứ tự hai cụm, va chạm `VARS_*` ↔ `VARV_*` quay lại ngay.**

### 2.7 Các khẳng định còn lại của `06` — kiểm nhanh

| # | Khẳng định | Kết luận |
|---|---|---|
| K30 | `settings\task\missions.txt` 45 dòng, **id 22 trống** ở dòng 23 | ✅ dòng 23 = `22 \script\missions\mission_trong.lua` |
| K31 | `WorldSet_GameServer.ini` `Count=910`, **thiếu 957** | ✅ 910 dòng `World*`, grep `957` = 0; các map 175/336/337/338/339/340/464/480/495 **đều có** |
| K32 | `closure3.json` 85 duy nhất / 15 ở `scriptjx2` / 1 không có ở đâu | ✅ đo lại **y hệt** |
| K33 | §7.3 “**EVENTSYS đã đầy đủ và đang chạy**” | 🔴 **SAI một phần** — xem dưới |
| K34 | `startgame.lua:100` `addnpcphonglangdo()` **không chú thích**, `:99` `-- addnpcbosssatthu()` **có chú thích** | ✅ đọc thật, khớp |
| K35 | Ladder: TOP 10 cứng, id > 10000, upsert theo TÊN | ✅ `KJx2SharedStore.cpp:538` `if (uId <= JX2LADDER_MIN_ID) return 0;` (**id 10000 bị từ chối**, phải ≥ 10001), `:552-573` upsert + sắp giảm dần, `:574-575` cắt top 10 |
| K36 | 5 hàm engine lõi còn thiếu (`AddStatData`, `ITEM_DropRateItem`, `NPCINFO_GetSeries`, `ITEM_SetExpiredTime`, `NpcName2Replace`) | ✅ đối chiếu với 1.005 tên đăng ký: đúng **không có** `AddStatData` / `ITEM_SetExpiredTime` / `NpcName2Replace`; `GetNpcSeries` và `DropRateItem` **có** |

🔴 **K33 — `EVENTSYS` KHÔNG giống bản Linux (`06` so byte cho `activitysys` nhưng bỏ qua `eventsys`):**

```
md5  E:\...\bin\server\script\misc\eventsys\eventsys.lua  = 1a399f24cf7f3f7e0ded2de780a985bd  (2.650 B, 20/08/2026)
md5  D:\ServerLinux\server1\script\misc\eventsys\eventsys.lua = 7c692df49fdfa6dcf588790c3d88536c  (1.634 B, 2021)
```

Khác biệt: bản JX1 **thêm hẳn `EventSys:OnPlayerEvent`** (34 dòng, dấu `[WLLS port 20/08/2026]`),
vì bản Linux **không định nghĩa hàm này trong Lua — engine Linux bơm vào**. Đây là bản **tự viết
lại**, chưa đối chiếu với hành vi engine gốc. Cả 3 tính năng đều gọi `OnPlayerEvent`
(`kill_level.lua:72,74`, `fld_head.lua:140`) ⇒ **phải phản biện riêng hàm này**, không được coi là
“đã đầy đủ”. (Phần `type\` thì **đúng là đầy đủ**: cả hai cây có đúng **15** tệp cùng tên.)

**06: 16 khẳng định — 14 ĐÚNG, 1 SAI (K33), 1 thiếu chứng minh (thứ tự đăng ký, đã bù ở §2.6);
kèm 4 điểm bổ sung mới (KĐ-a, KĐ-b, bẫy timer #3, hệ quả `strstr` theo state).**

---

## 3. 🔴 CÁC HỆ THỐNG **KHÔNG BÁO CÁO NÀO (00–09) KỂ TÊN**

Cách tìm: quét toàn bộ **34 tệp lõi** của 3 tính năng (bản UTF-8 giữ nguyên số dòng ở
`src_utf8\`) theo 23 mẫu hệ thống, rồi đối chiếu từng hệ với cây JX1 (`v11.py`, `v12.py`).

### 3.1 🔴🔴 F1 — HỆ CẤM HỒI THÀNH PHÙ: **JX1 KHÔNG CÓ, và cái vỏ thì rỗng**

Ba tính năng gọi `DisabledUseTownP` **7 lần**:

```
phonglangdo/fld_head.lua:142   DisabledUseTownP(1)   -- 限制其在渡船内使用回城符
phonglangdo/mission.lua:128    DisabledUseTownP(1)
phonglangdo/mission.lua:134    DisabledUseTownP(1)
phonglangdo/mission.lua:140    DisabledUseTownP(1)
vuotai/chuangguang30.lua:92    DisabledUseTownP(1)
vuotai/mission_match.lua:52    DisabledUseTownP(0)
vuotai/mission_match.lua:81    DisabledUseTownP(1)
```

Phía JX1, hàm **có đăng ký** (`ScriptFuns.cpp:15203`) nhưng **thân là stub**:

```c
// D:\GAMEDEVNEW\Sources\Core\Src\KJx2WarInfra.cpp:256-261
// LO GOC nhu tren; map 221 la map mission - viec chan Tho Dia Phu de E5 xu
// bang thuoc tinh map neu can
int LuaDisabledUseTownP(Lua_State* L)
{
	return 0;
}
```

Đường thứ hai của bản Linux cũng **đứt** trên JX1: bản Linux bắn sự kiện ngay sau khi dùng phù —

```
D:\ServerLinux\server1\script\item\townportal_l.lua:30-31
    UseTownPortal();
    UseTownPortalEvent:OnEvent();
```

còn JX1 **chỉ gọi vế đầu**:

```
E:\...\bin\server\script\item\townportal_l.lua:73     UseTownPortal()
```

⇒ `misc\eventsys\type\usetownportal.lua` của JX1 **tồn tại nhưng không bao giờ được kích hoạt**
(grep toàn `script\` + `scriptjx2\`: 0 điểm gọi `UseTownPortalEvent:OnEvent`).

**Cơ chế duy nhất còn chạy trên JX1 là `script\header\forbidmap.lua`** — chưa báo cáo nào kể tên.
Đó là danh sách map cứng, **67 tệp item `Include`** nó, **9 tệp** gọi `CheckAllMaps(mapid)`. Chính
dự án đã dùng nó làm giải pháp thay thế cho Thành Bảo:

```
E:\...\bin\server\script\header\forbidmap.lua:95-99
	-- [TONGCASTLE 23/08 phan bien F15] cam moi item dich chuyen trong Thanh Bao 984
	-- (DisabledUseTownP la no-op tren JX1; Linux cam qua forbiditem TRANSFER)
	if mapid == 984 then
		return 1
	end
```

Bảng của nó hiện có `SJMAPS · BWMAPS · ZQMAPS · FHMAPS · YANDIBAOZANG · TONGMAPS ·
NEWPRACTICEMAPS · TRAINMAPS`. Đối chiếu với 3 tính năng:

* **337 / 338 / 339** (3 thuyền) — **KHÔNG có trong bất kỳ danh sách nào**
* **464–495** (32 map Vượt ải) — **KHÔNG có**
* **957** (Mật Phòng) — **KHÔNG có**
* 336 và 340 thì **có** (đều nằm trong `TRAINMAPS`)

**Hệ quả vận hành**: người chơi lên thuyền rồi bấm Hồi thành phù là **thoát ngay giữa chuyến**, mang
theo đồ vừa nhặt; trong Mật Phòng 957 cũng vậy. Đây là kiểu lỗi **im lặng, không crash**, đúng loại
mà `06` §⑧ cảnh báo nhưng lại không phát hiện ra. **Việc phải làm**: thêm 3 dải map vào
`forbidmap.lua` (hoặc hiện thực thật `LuaDisabledUseTownP`) — **không** phải chỉ port lời gọi Lua.

### 3.2 F2 — HỆ CAMP / `ForbidEnmity` (15 điểm gọi, không báo cáo nào kể tên)

```
phonglangdo/fld_head.lua:118   ForbidEnmity(1)
phonglangdo/fld_head.lua:127   SetCurCamp(1)
phonglangdo/fld_death.lua:6-7  camp = GetCamp() ; SetCurCamp(camp)
phonglangdo/mission.lua:70     ForbidEnmity(0)
vuotai/chuangguang30.lua:93    ForbidEnmity(1)
vuotai/chuangguang30.lua:114   SetTmpCamp(nMapId)     -- 设置player的阵营
vuotai/chuangguang30.lua:138-139  SetCurCamp(GetCamp()) ; SetTmpCamp(0)
vuotai/chuangguang30.lua:148   ForbidEnmity(0)
vuotai/mission_match.lua:42    SetCurCamp(GetCamp())
```

JX1 **có đủ 4 hàm** (đối chiếu bảng đăng ký): `ForbidEnmity → LuaTongForbidEnmity`,
`SetTmpCamp → LuaSetTmpCamp`, `SetCurCamp → LuaSetPlayerCurrentCamp`, `GetCamp → LuaGetPlayerCamp`,
`SetCamp → LuaSetPlayerCamp`. ⇒ **không phải viết mới**, nhưng **ngữ nghĩa `SetTmpCamp(nMapId)`
(Vượt ải dùng CHÍNH MAP ID LÀM SỐ HIỆU PHE, giá trị tới 957) CHƯA XÁC MINH** — JX1 có thể kẹp phe
trong dải nhỏ. Phải đọc `LuaSetTmpCamp` (`ScriptFuns.cpp:3859`) trước khi thi công.

### 3.3 F3 — HỆ `SetRevPos` / `RevivePos.ini` (4 điểm gọi) — **có 2 vấn đề**

```
phonglangdo/fld_head.lua:143       SetRevPos(175,1)   -- 设置重生点在西山村
phonglangdo/mission.lua:129/135/141 SetRevPos(175,1)
```

* JX1 **có** hàm (`SetRevPos → LuaSetPlayerRevivalPos`, thân `ScriptFuns.cpp:8019-8047`), nhận
  đúng 2 tham số `(subworldId, revId)` ⇒ **tương thích chữ ký**.
* `RevivePos.ini` của JX1 **có** mục cần: `[175] region=1,1 / 1=53440, 101440` ⇒ `SetRevPos(175,1)`
  chạy được.
* 🔴 **Nhưng `RevivePos.ini` (142 section) KHÔNG có mục cho map 337 / 338 / 339 / 957 / 464–495.**
  Mọi đường hồi sinh mặc định trên các map này rơi vào **fallback cứng `(53, 19, 52032, 101696)`**
  (`KPlayer.cpp:1401-1406`). Chưa báo cáo nào liệt kê `RevivePos.ini` vào danh sách bảng phải bổ sung.
* 🔴 **`GetRevivalPosFromId` MỞ VÀ ĐỌC LẠI `RevivePos.ini` TỪ ĐĨA MỖI LẦN GỌI** —
  `KSubWorldSet.cpp:196-199` `KIniFile IniFile; … IniFile.Load("RevivePos.ini")`, **không cache**.
  Phong Lăng Độ gọi `SetRevPos` cho **từng người chơi lên thuyền** và **từng người cập bến**
  (`mission.lua:126-141` trong vòng duyệt người chơi) ⇒ I/O tệp trên **luồng logic duy nhất**
  (xem §4).

### 3.4 F4 — HỆ LOG `tbLog` (11 điểm gọi, không báo cáo nào kể tên)

```
satthu/nieshichen.lua:127         tbLog:PlayerActionLog("TinhNangKey","NhanNhiemVuBossSatThu")
phonglangdo/fld_head.lua:293      tbLog:PlayerActionLog("EventChienThang042011","BaoDanhPhongLangDo")
phonglangdo/fld_head.lua:296,298  tbLog:PlayerActionLog("TinhNangKey", …)
vuotai/npc_death.lua:107          tbLog:TabFormatLog("challengeoftime","Mission_Complete","HiddenNpc",index,time)
vuotai/npc/dragonboat_main.lua:168,170  tbLog:PlayerActionLog("TinhNangKey", …)
vuotai/chuangguang30.lua:113 · npc.lua:482 · timer_match.lua:37   WriteLog(…)
```

JX1 **CÓ ĐỦ**: `script\lib\log.lua:56` `function tbLog:TabFormatLog(...)`, `:87`
`function tbLog:PlayerActionLog(szKeyLog, ...)` (bản `scriptjx2\lib\log.lua` giống hệt, cùng 4.318 B);
`WriteLog` đăng ký ở `GameScriptFuns[]` (`LuaJX2_WriteLog`, thân `KTongJX2.cpp:3999`).
⇒ **không phải làm gì**, nhưng cần ghi vào bảng phụ thuộc để không ai xoá nhầm.

### 3.5 F5 — HỆ CẤM VẬT PHẨM THEO MAP: `set_MapType` + `FORBITMAP_LIST` (**có lỗ**)

```
vuotai/chuangguang30.lua:243-256   ChuangGuan30:SetForbitItem()
    set_MapType(CHUANGGUAN30_MAP_ID, szMapType)
    if self.tbForbitItemType[i] == "MATE" then  FORBITMAP_LIST[CHUANGGUAN30_MAP_ID] = 1  end
    tb_MapType[szMapType] = tb_MapType[szMapType] or {}
```

* `set_MapType` **CÓ** ở JX1: `script\item\forbiditem.lua:22`, cùng `get_MapType`, `del_MapType`,
  `add_Item2Map`, bảng `tb_MapType` / `tb_MapDetail`. `06` §9.4 có ghi một dòng “`item\forbiditem.lua`
  (`set_MapType`) CÓ” — đúng.
* 🔴 **`FORBITMAP_LIST` thì KHÔNG được khai ở JX1.** Điểm duy nhất đụng tới nó là
  `script\missions\tongcastle\game.lua:163-164`, và ở đó đợt Thành Bảo đã phải **tự khởi tạo**:
  ```
  -- [TONGCASTLE 23/08] heart_head JX1 khong co FORBITMAP_LIST (ban Linux :27) - tu khoi tao
  FORBITMAP_LIST = FORBITMAP_LIST or {}
  ```
  ⇒ `chuangguang30.lua:249` sẽ **ghi vào một bảng nil**. ~~hoặc vô tình **dùng chung bảng của Thành
  Bảo** nếu chạy sau.~~ 🔴 **[đã sửa theo đối chất]** Vế “dùng chung bảng của Thành Bảo” **SAI**:
  engine cấp cho **mỗi tệp script một Lua state riêng** — `KLuaScript.h:90` `Lua_State * m_LuaState;`
  (trường của từng đối tượng) và `KLuaScript.cpp:23` `m_LuaState = lua_open(100);` trong `Init()`,
  được gọi cho từng `g_ScriptSet[i]` ở `KSortScript.cpp:151`. Biến toàn cục Lua **không chia sẻ giữa
  hai tệp**, nên `FORBITMAP_LIST` của `tongcastle\game.lua` **không bao giờ** nhìn thấy được từ
  `chuangguang30.lua`. Kết luận đúng gọn hơn: **luôn luôn là bảng nil**, thứ tự chạy không đổi được
  điều đó ⇒ bắt buộc tự khởi tạo y hệt đợt Thành Bảo. (Điều này cũng khớp với chính §2.4 ở trên:
  “cờ `g_IsJx2Script` lấy theo **tệp sở hữu Lua state**”.) Chưa báo cáo nào kể tên `FORBITMAP_LIST`.

### 3.6 F6 — BẢNG `settings\skills.txt` (`AddSkillState`) — **đã kiểm, KHÔNG thiếu**

`satthu/kill_level.lua:82` gọi `AddSkillState(541, 1, 0, 54)`. Không báo cáo nào kiểm bảng kỹ năng.
Đo lại: cả hai bảng đều **114 cột** (LINUX 1.629 dòng / JX1 1.602 dòng), và **`SkillId = 541` có ở
CẢ HAI** với đúng tên `"Hoàn thành nhiệm vụ Sát thủ"`. ⇒ **không phải bổ sung dữ liệu.**
(Ngữ nghĩa `AddSkillState` giữa JX1/JX2 vẫn khác — nằm trong 4 hàm mà `g_IsJx2Script` rẽ nhánh.)

### 3.7 F7 — `Pay()` (thu phí báo danh Vượt ải)

`vuotai/npc/dragonboat_main.lua:193` `Pay(10000)`. JX1 **CÓ** (`Pay → LuaPlayerPayMoney`,
`ScriptFuns.cpp:9511`). Chưa báo cáo nào kể — nhỏ nhưng là **đường tiền**, phải rà khi cân bằng.

### 3.8 Hai hệ đã kiểm và **LOẠI TRỪ** (bằng chứng phủ định, tiết kiệm công thi công)

| Hệ | Kết quả quét 34 tệp lõi |
|---|---|
| **TRAP** (`lib_trap.lua`, `AddTrap`, `SetTrapParam`) | **0 điểm gọi**. Bảng `settings\maps\liandandong\trap_1.txt` có tồn tại nhưng 3 tính năng **chỉ đọc `npc_3.txt`** (`chuangguang30.lua:216`, `npc\transfer.lua:54`) ⇒ **không cần port hệ trap** |
| **ACHIEVEMENT** | **0 điểm gọi**. (JX1 vẫn có sẵn `misc\eventsys\type\achievement.lua`) |

### 3.9 Hai điểm ngữ nghĩa còn treo (CHƯA XÁC MINH — cần vòng sau)

* **`SetFightState`** — 11 điểm gọi (`fld_head.lua:134`, `mission.lua:34,73`, …). JX1 có
  (`LuaSetFightState`, `ScriptFuns.cpp:9770`) nhưng theo ghi chú của dự án, `GetFightState` bên JX1
  là **cờ RÚT VŨ KHÍ**, có thể khác hẳn ngữ nghĩa JX2. **Chưa đọc mã hai bên.**
* **`Ladder_NewLadder` với giá trị ÂM** — `vuotai/npc_death.lua:77,88` gọi
  `Ladder_NewLadder(LadderId, teamname, -1 * laddertime, 1, nLeaderFaction, nLeaderGender)`
  (thời gian càng nhỏ càng tốt ⇒ đảo dấu). JX1 sắp **giảm dần** theo `i64Value`
  (`KJx2SharedStore.cpp:562-573`) rồi **cắt còn 10** (`:574-575`, `JX2LADDER_TOP = 10` ở `:440`)
  ⇒ hành vi khớp về hướng, **nhưng** `strncpy(e.szName, szName, 31)` (**`:543`** —
  **[đã sửa theo đối chất: bản trước ghi `:542`, đó là dòng `memset`]**) **cắt tên đội còn 31 byte**
  và upsert so **đúng 31 byte đó** (`:554` `strncmp(tb[i].szName, e.szName, 31)`)
  ⇒ hai tên đội tiếng Việt dài khác nhau có thể **gộp làm một dòng bảng hạng**. Chưa báo cáo
  nào nêu.

---

## 4. RỦI RO VẬN HÀNH KHI BẬT 3 TÍNH NĂNG TRÊN MÁY CHỦ ĐANG CHẠY

Bối cảnh bắt buộc nhớ: **100 % CPU của GameServer nằm trên MỘT luồng** — mọi thứ dưới đây cộng
thẳng vào cùng một vòng tick 18 Hz.

### 4.1 Tải NPC

| Nguồn | Số NPC | Kiểu |
|---|---|---|
| `satthu` — `add_killertasknpc(addkillertasknpc)` (`killbosshead.lua:183-194`) | **160** boss trên 40 map (đúng 4/map) | **THƯỜNG TRỰC**, hồi sinh theo `ReviveFrame` của `npcs.txt` |
| `satthu` — NPC dẫn nhập 769 “Nhiếp Thí Trần” | **7** (map 11/1/37/176/162/78/80) | thường trực |
| `phonglangdo` — `mission.lua:19-24` | **30 thuỷ tặc/thuyền × 3 thuyền = 90** mỗi chuyến | tạm, ~40 phút/chuyến |
| `phonglangdo` — `fld_smalltimer.lua:35,42,58` (3 đợt boss 725) + `:50-56` (2 con 1692 nếu bật nhiệm vụ mới) | **9 … 15**/chuyến | tạm |
| `vuotai` — `npc.lua` bảng `tbFightNpc` | **~610 lượt sinh/1 lượt chơi hoàn chỉnh** mỗi map (sơ cấp 610 / cao cấp 609); mỗi ải sống đồng thời **16–20** con | tạm, 30 phút/lượt |
| `vuotai` — **32 map chạy song song mỗi giờ** (`tbLevelMaps` 464-479 + 480-495) | đỉnh **~512–640** NPC sống; **~19.500 lượt `AddNpc`/`DelNpc` mỗi chu kỳ giờ** | tạm |

**Kết luận tải NPC**: đỉnh thêm khoảng **800–900 NPC sống** (167 thường trực + ~640 Vượt ải + ~105
Phong Lăng Độ) — so với `MAX_NPC = 98000` là **không đụng trần**. Cái đáng lo **không phải số NPC
sống mà là NHỊP SINH/XOÁ**: ~19.500 lượt `AddNpc`/`DelNpc` mỗi giờ dồn vào các mốc chuyển ải, tức
**bùng theo cụm** chứ không rải đều — mỗi lần chuyển ải là 32 map × ~20 con **cùng một tick**.

### 4.2 Tải mission + timer

| Hạng mục | Số lượng thêm |
|---|---|
| Mission mới đồng thời | **32** (Vượt ải, mỗi map 1 `MISSION_MATCH`) + **3** (3 thuyền) = **35** |
| Trần mission/map | **9** — Vượt ải chỉ dùng **1**/map ⇒ dư thoải mái |
| Mission timer mới | Vượt ải **3**/map (`41 MATCH`, `42 BOARD`, `43 CLOSE`) × 32 = **96**; Phong Lăng Độ **2**/thuyền (`28`, `29`) × 3 = **6** ⇒ **~102 timer** |
| Nhịp quét | `KSubWorld::Activate` gọi `m_MissionArray.Activate()` **mỗi tick** (`KSubWorld.cpp:1168`) → `KMission::Activate` → `m_cTimerTaskSet.Activate()` (`KMission.cpp:67-71`), duyệt **9 khe/mission** ⇒ **35 × 9 ≈ 315 lượt kiểm/tick ≈ 5.670 lượt/giây**. Chi phí số học thuần, **không đáng kể** |
| 🔴 Rủi ro thật | **BẪY rò khe timer** (`ScriptFuns.cpp:11382`): `StartMissionTimer` **không dò trùng id**. `trigger_challengeoftime.lua` mỗi giờ gọi `close_missions` rồi `start_missions` cho **cả 32 map**. Nếu một lần `CloseMission` thất bại (script lỗi, map chưa nạp) thì khe timer **không được trả lại**; **9 lần là map đó chết câm** — timer im lặng không nổ, không log, không crash |
| Trần map | `WorldSet_GameServer.ini` `Count=910` + map **957** = 911, `MAX_SUBWORLD = 1000` ⇒ còn dư **89 khe** |
| Trần script | `MAX_SCRIPT_IN_SET = 5000`; hiện có 2.811 + 189 = **3.000** tệp, port thêm **85** ⇒ **3.085**. Nạp lười nên số khe thật thấp hơn ⇒ an toàn |

### 4.3 🔴 Ba điểm I/O trên luồng logic (đây mới là rủi ro CPU thật)

| Điểm | Bằng chứng | Tần suất khi bật 3 tính năng |
|---|---|---|
| **`Ladder_NewLadder` ghi TỆP mỗi lần gọi** | `KJx2SharedStore.cpp:531-577` kết thúc bằng `sLadderSave()`; `sLadderSave` `fopen`+`fprintf` toàn bộ bảng → `MoveFileEx` → `KGameKV::PutFile(…, true)` | `satthu/kill_level.lua:44` **mỗi lần giết boss** (trần 8 lần/người/ngày × số người chơi); `vuotai/npc_death.lua:77,85,88` **3 lần mỗi đội hoàn thành** × 32 map/giờ. Ước lượng **hàng trăm lần ghi tệp/giờ** |
| **`SetRevPos` nạp lại `RevivePos.ini` mỗi lần gọi** | `KSubWorldSet.cpp:196-199` | mỗi người **lên thuyền** + mỗi người **cập bến** (`mission.lua:126-141` trong vòng duyệt). Một chuyến đông 3 thuyền × N người ⇒ **N×2 lần đọc INI dồn vào vài tick** |
| **`GetTabFileData` cho từng con quái** | `fld_head.lua:176-179` gọi `GetTabFileHeight` + 2 lần `GetTabFileData` **cho mỗi NPC sinh ra**; `mission.lua:19-24` sinh 30 con liên tiếp | Nếu wrapper `scriptjx2\lib\file.lua` nạp lại tệp mỗi lần (**CHƯA XÁC MINH** — phải đọc `TabFile_Load` `ScriptFuns.cpp` xem có cache không) thì 30 con = 90 lượt tra bảng dồn 1 tick |

### 4.4 Va chạm với thứ ĐANG CHẠY (bổ sung cho mục ⑥ của `06`)

* **Phong Lăng Độ bản VN đang spawn NPC thật ngay lúc này**: `startgame.lua:100`
  `addnpcphonglangdo()` **không bị chú thích** (đối chứng: `:99` `-- addnpcbosssatthu()` và `:101`
  `--addnpctongkimtrungcap()` thì có). ⇒ map 336/337 **đang có NPC sống**; bản JX2 lên cùng map sẽ
  **chồng NPC từ giây khởi động**, không chờ tới giờ sự kiện.
* **Săn boss sát thủ bản VN dùng trùng dải npcId**: `script\tinhnang\boss_satthu\lib_boss_st.lua`
  bảng `BOSS_SATTHU` (20 boss, npcId **768, 812–820**) đè đúng dải cấp 90 của `killbosshead.lua`
  (dòng 177-179). Hiện `addnpcbosssatthu()` **đang tắt**, nhưng lib vẫn được nạp và
  `tinhnang\vuot_ai\sugiasatthu.lua:5` **dùng lại chính lib này** ⇒ bật bản JX2 mà quên gỡ dây VN
  sẽ ra **hai bộ boss trùng tên trùng id khác máu khác drop**.
* **Ladder id**: `kill_level.lua:44` dùng **10119**. Đã đối chiếu danh sách id đang dùng thật của
  `06` §6.4 (10118, 10182-10186, 10196/10197, 10225-10228+, 10250/10251, 10261) — **10119 còn trống**
  ⇒ không xung đột. Nhưng ladder là **TOP 10 cứng, 1 máy chủ, lưu tệp cục bộ**.

### 4.5 Thứ tự bật đề nghị (rủi ro tăng dần)

1. **Săn boss sát thủ** — 167 NPC thường trực, **0 mission, 0 timer mới**, đường script đã lọt
   `g_IsJx2Script` sẵn. Rủi ro thấp nhất; chỉ cần gỡ dây bản VN `boss_satthu` trước.
2. **Phong Lăng Độ** — 3 mission + 6 timer + ~105 NPC/chuyến; **phải gỡ `addnpcphonglangdo()` ở
   `startgame.lua:100` trước**, và **phải vá lỗ Hồi thành phù** (§3.1) nếu không muốn người chơi
   thoát thuyền tuỳ ý.
3. **Vượt ải** — nặng nhất: 32 mission + 96 timer + ~19.500 lượt sinh/xoá NPC mỗi giờ, cộng
   `FORBITMAP_LIST` chưa khai và bảng hạng ghi tệp liên tục. **Nên bật thử trên ít map trước**
   (thu hẹp `tbLevelMaps` xuống 2–4 map) rồi mới mở đủ 32.

---

## 5. PHỤ LỤC — script kiểm chứng (dùng lại được)

Thư mục: `C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\0064b491-1b33-4139-acb8-6928fda45bcd\scratchpad\`

| Tệp | Việc |
|---|---|
| `v8.py` | **Duyệt cây gốc B và giải mã tên thư mục GBK** → tìm ra `渡船刷怪点.txt` (mẫu để tìm 49 bảng chữ Hán còn lại) |
| `v10.py` / `v12.py` | Bóc **1.005** dòng đăng ký `GameScriptFuns[]`, tra tên hàm có/không |
| `v11.py` | Quét **23 mẫu hệ thống** trên 34 tệp lõi của 3 tính năng (nguồn của §3) |
| `v15.py` | Đếm lại `closure3.json`, phân loại 85 tệp thiếu thành *có ở `scriptjx2`* / *không có ở đâu* |
| `v16.py` | Đối chiếu 141 NPC id giữa hai `npcs.txt` |
| `v19.py` / `v20.py` | Đối chiếu vật phẩm genre 6 **theo TÊN** giữa hai `magicscript.txt` |
| `v5.py` | Phát hiện **5 thế hệ `settings\item\{000..004}`** của bản Linux |
| `v18.py` | Đếm số quái mỗi lượt Vượt ải từ `tbFightNpc` |

Bẫy đã gặp và cách tránh: **heredoc của Bash tool rút `\\` thành `\`** — mọi script có dấu `\`
phải ghi qua tệp bằng công cụ `Write` rồi mới chạy, không viết inline.

---

## 6. VIỆC PHẢI SỬA TRONG `05_dulieu.md` / `06_phia_jx1.md` (để tác tử khác áp)

| Tệp | Chỗ | Sửa thành |
|---|---|---|
| `05` §2.1, §2.2, §8 | “78 map” | **85 map** (không có cặp nào trùng giữa 5 tập) |
| `05` §5.5, §6 CHẶN #1, §8 | “thiếu bảng toạ độ — chặn cứng” | **BỎ HẲN**: tệp có ở `D:\ServerLinux\Patch\settings\maps\中原北区\渡船\渡船刷怪点.txt` (893 B, 63 dòng, `XPOS/YPOS` pixel) |
| `05` **BS-2** (KHÔNG phải CHẶN #2) | “`GetTabFileData` không có ở đâu” | **CÓ** ở `scriptjx2\lib\file.lua:38,46`; **[đã sửa theo đối chất]** và **KHÔNG phải sửa `Include`** — `sJX2RemapScriptPath` (`ScriptFuns.cpp:1938-1967`, gọi ở `:2016`) tự đổi `script\lib\` → `scriptjx2\lib\` khi bản gốc không tồn tại ⇒ **không phải làm gì**. Kèm theo: “chứng cứ phụ” `05:565-566` (`npcpoint.lua` đang hỏng) **cũng SAI** |
| `05` §6 **CHẶN #2** (7 tệp `lineup*.txt`) | — | **[đã sửa theo đối chất] GIỮ NGUYÊN — VẪN ĐỨNG VỮNG**: `bin\server\settings\maps\challengeoftime\` không tồn tại, glob `bin\server\**\lineup*` = 0 kết quả |
| `05` §4.2, BS-2 | “quét 2.811 tệp `.lua`” | **3.000** tệp (2.811 `script\` + 189 `scriptjx2\`) |
| `05` §6 CHẶN #3 | “16 dòng” | **14 dòng mới** (11 genre 6 + 3 ngựa); 2 lời gọi đổi DetailType **không** là dòng bảng |
| `05` BS-4 | “`magicscript.txt` JX1 14 cột” | **30 cột** cả hai; dòng dữ liệu JX1 răng cưa (14/15/17/30); **lệch ngữ nghĩa từ cột 10**, JX1 **không có** cột `五行属性` |
| `05` §3.1 | dùng `settings\item\004\` | thêm bằng chứng: engine Linux chọn thế hệ qua `"/settings/item/%03d"` (ELF `0x20B379`), 004 là bản duy nhất đủ dữ liệu |
| `05` §3.2 | “`phonglangdo` — 8 mục” | `item_can_them.csv` **thiếu 2 bộ ba** `6,1,2745` và `4,489` (nhánh phonglangdo) — bổ sung CSV hoặc sửa con số |
| `06` §7.3 | “EVENTSYS đã đầy đủ” | `eventsys.lua` **KHÁC BYTE** bản Linux; `OnPlayerEvent` là bản **Lua tự viết 20/08/2026** thay hàm engine ⇒ phải phản biện riêng |
| `06` §2 | 2 bẫy timer | thêm **bẫy 3**: glb timer bắn tối đa **16/tick** (`KJx2League.cpp:1426`) |
| `06` §1.5 | 3 khiếm khuyết | thêm **KĐ-a** (`GetMissionS` dùng `>= 0`, lệch luật biên) và **KĐ-b** (`memset(m_MissionValueC)` bị chú thích ở `KMission.h:144,167`) |
| `06` §4 | tiền tố `g_IsJx2Script` | thêm hệ quả: cờ theo **tệp sở hữu state**, nên **không được** móc 3 tính năng bằng `Include` từ `startgame.lua`/`timerserver.lua` |
| `06` §5, §9.4, §10 | danh sách hệ phụ thuộc | bổ sung **7 hệ ở §3**: town-portal (**hỏng**), CAMP, `SetRevPos`/`RevivePos.ini`, `tbLog`, `FORBITMAP_LIST`, `skills.txt`, `Pay` |

*Mọi con số trong tài liệu này đều kèm tệp:dòng, MD5, hoặc offset/địa chỉ ELF. Chỗ chưa dò tới
được ghi rõ **CHƯA XÁC MINH**.*

---

## ĐỐI CHẤT (tác tử độc lập)

Ngày: 24/08/2026. Người kiểm **không phải** người viết tài liệu này. Phương pháp: mặc định coi mọi
khẳng định của `17_…` là **SAI cho tới khi tệp gốc / nhị phân chứng minh ngược lại**; ưu tiên kiểm
con số đếm được, đường dẫn, số dòng, địa chỉ ELF, và **mọi chỗ kết luận “thiếu / không có / chặn
tiến độ”** (dễ sai nhất vì có HAI gốc dữ liệu A = `server1`, B = `Patch`).

**Kết quả: kiểm 24 khẳng định — 7 SAI, 1 lệch nhỏ, 16 ĐÚNG. Tìm thêm 5 chỗ chính tài liệu này bỏ sót.**

### Bảng đối chất

| # | Khẳng định của `17_…` | Bằng chứng gốc | KẾT LUẬN | Sửa lại thành |
|---|---|---|---|---|
| Đ1 | §0 mục 1 + §1.2 · Bảng toạ độ Phong Lăng Độ **CÓ THẬT** ở gốc B, **893 B**, header `XPOS<TAB>YPOS`, **63 dòng** | Đường thật trên đĩa (byte GBK thô, hiện ra dạng CP1252): `D:\ServerLinux\Patch\settings\maps\ÖÐÔ­±±Çø\¶É´¬\¶É´¬Ë¢¹Öµã.txt`. `wc -c` = **893**; dòng 1 = `XPOS\tYPOS`; dòng 2…64 = **63** hàng. Khớp đúng chuỗi mà script yêu cầu: `fld_head.lua:18` `npcthiefpos = "\\settings\\maps\\中原北区\\渡船\\渡船刷怪点.txt"` | ✅ **ĐÚNG** | — (giữ nguyên; đây là phát hiện đúng và quan trọng nhất của vòng 2) |
| Đ2 | §1.2 · “bảng thật trải ô **X ≈ 1575…1661**, **Y ≈ 3162…3224**” | `awk 'NR>1{print $2}' … \| sort -n \| sed -n '1p;$p'` ⇒ Y = **101184 … 104096**. 104096 ÷ 32 = **3253**, không phải 3224. (103168 chỉ là giá trị ở dòng 27 và 63, không phải max.) X = 50400…53152 ⇒ 1575…1661 ✔ | 🔴 **SAI** (nửa Y) | **Y ≈ 3162…3253**. *Trớ trêu: đây đúng là loại lỗi “dựng lại vùng toạ độ sai” mà §1.2 dùng để bác `05`.* → đã sửa trong thân bài |
| Đ3 | §0 mục 2 + K10 · “**CHẶN #2 của `05`** (thiếu `GetTabFileData`) **SAI**” | `05_dulieu.md:546` ghi nguyên văn: **“CHẶN #2 (MỀM) — thiếu 7 tệp `lineup*.txt` của Vượt ải ở cây JX1”**; phần `GetTabFileData` là **hệ quả kèm** ở `05:551-566` (và là **BS-2**). Kiểm cây JX1: `bin\server\settings\maps\challengeoftime\` **không tồn tại**; glob `bin\server\**\lineup*` = **0 kết quả** | 🔴 **SAI** (bác nhầm đối tượng) | Cái bị lật là **BS-2**. **CHẶN #2 (7 tệp `lineup{8,16,20,24,32,40,56}.txt`) VẪN ĐỨNG VỮNG** — vòng 2 chưa hề kiểm nó ở cây JX1 → đã sửa trong thân bài |
| Đ4 | K10/§1.3 · `GetTabFileHeight` `:38`, `GetTabFileData` `:46` ở `scriptjx2\lib\file.lua` | Đọc thật `E:\...\bin\server\scriptjx2\lib\file.lua`: `:38 function GetTabFileHeight(mapfile)`, `:46 function GetTabFileData(mapfile, row, col)`. Và `E:\...\bin\server\script\lib\file.lua` **thật sự không tồn tại** (thư mục `script\lib\` có 27 tệp, không có `file.lua`) | ✅ **ĐÚNG** | — |
| Đ5 | §1.3 · “Việc thật rút xuống còn **sửa đường dẫn `Include`** … hoặc chép 1 tệp” | `ScriptFuns.cpp:1938-1967` `sJX2RemapScriptPath()`: `fopen` đường gốc, **trượt thì đổi tiền tố** `script\lib\` → `scriptjx2\lib\`. Được `LuaIncludeFile` gọi ở **`:2016`**, ngay trước `lua_dofile` (`:2020`); đường dẫn lúc đó là **tuyệt đối** (`g_GetRootPath`, `:1998-2004`) nên `fopen` phán đoán đúng | 🔴 **SAI** (kê thừa việc) | **KHÔNG phải làm gì cả** — `Include("\script\lib\file.lua")` **tự động** rơi sang `scriptjx2\lib\file.lua`. Kéo theo: “chứng cứ phụ” `05:565-566` (`npcpoint.lua` *“hiện cũng hỏng/chết”*) **cũng SAI** → đã sửa trong thân bài |
| Đ6 | K10 · “Cây `scriptjx2\` **được engine nạp** (`KSortScript.cpp:152`, `:156-161`)” | `:152-161` nằm **trong `LoadScriptToSortListA`** — hàm nạp *một* tệp bất kỳ, không chứng minh cây nào được liệt kê. Chỗ liệt kê thư mục lúc boot là `g_IniScriptEngine`: `:56` `LoadAllScript("\script")` + `:65` `LoadAllScript("\scriptjx2\tong_vn")`. **`scriptjx2\lib\` KHÔNG có trong danh sách boot** | 🔴 **SAI** (dẫn nhầm dòng) | Lúc boot chỉ liệt kê `\script` và `\scriptjx2\tong_vn`; `scriptjx2\lib\` **chỉ tới được qua `Include`/`IncludeLib`** (bảng `IncludeLib` ở `ScriptFuns.cpp:2492-2502` trỏ thẳng `scriptjx2\lib\file.lua`) → đã sửa trong thân bài |
| Đ7 | K1 · “**85 map**, không cặp nào trùng giữa 5 tập; `map_can_them.csv` 85 id duy nhất / 92 dòng” | Bóc lại `map_can_them.csv`: 93 dòng vật lý = 1 tiêu đề + **92** dữ liệu. Nhóm: `satthu-NPC dan nhap` 7 · `satthu-map co boss` **40** · `phonglangdo` 5 · `vuotai-so cap` 16 · `vuotai-cao cap` 16 · `vuotai-mat phong` 1 · `vuotai-NPC dan nhap` 7. 40+7+5+32+1 = **85** duy nhất; 7 dòng lặp = đúng 7 thành ghi 2 lần. 40 map boss **không giao** với 7 thành | ✅ **ĐÚNG** | — (`05` “78 map” đúng là sai; phép trừ 7 thành là trừ thừa) |
| Đ8 | K3 · `item_can_them.csv` chỉ **6** dòng `phonglangdo`; thiếu `6,1,2745` (`fld_head.lua:274`) và `4,489` (`fld_head.lua:269`) | `grep -n phonglangdo item_can_them.csv` = **6** dòng (2745 **không** có; chỉ có `6,1,2743`). `fld_head.lua:269` `if (itemgenre ~= 4 or detailtype ~= 489)`; `:274` `if (itemgenre ~= 6 or particular ~= 2745)` — **cả hai số dòng đúng từng chữ** | ✅ **ĐÚNG** | — |
| Đ9 | K11 · CHẶN #3 “đúng là **14 dòng mới**, `05` §6 ghi 16 vì cộng 2 lời gọi đổi DetailType” | `05:568-571` nguyên văn: *“CHẶN #3 (MỀM) — 16 dòng … 11 mục genre 6 (30009, 30010, 30528, 30530–30536, 30538) + 3 ngựa + **2 mục cần đổi DetailType**”* ⇒ 11+3 = **14**, giải thích của vòng 2 **chính xác** | ✅ **ĐÚNG** | Bổ sung: `05:294` còn ghi **“13 mục hàng + 3 ngựa = 16”** trong khi danh sách liệt kê ngay dưới (`:296`) chỉ có **11** id ⇒ `05` sai ở **hai** chỗ, vòng 2 mới bắt một |
| Đ10 | K14 · `magicscript.txt` **cả hai đều 30 cột**; lệch **ngữ nghĩa từ cột 10**; JX1 không có cột `五行属性` | `head -1 … \| tr '\t' '\n' \| wc -l` = **30** cho cả `D:\ServerLinux\server1\settings\item\004\magicscript.txt` và `E:\...\bin\server\settings\item\magicscript.txt`. Tiêu đề Linux: `…说明文字(9), 五行属性(10), 价格(11), 等级(12), 是否叠放(13), 脚本名(14)…`; JX1: `…Intro(9), Script(10), Price(11), ShortKey(12), nMaxStack(13), PickExecute(14)`, 16 cột cuối **không có tên** | ✅ **ĐÚNG** | — (`05` “JX1 14 cột” sai) |
| Đ11 | K18 · “2.811 `script\` + **189** `scriptjx2\` = **3.000**” | `find script -iname "*.lua" \| wc -l` = **2811**; `find scriptjx2 -iname "*.lua" \| wc -l` = **189** | ✅ **ĐÚNG** | Nhưng xem Đ6: 189 tệp ấy **không phải tất cả đều nạp lúc boot**, nên phép tính trần `MAX_SCRIPT_IN_SET` ở §4.2 là **ước lượng trên**, không phải số khe thật |
| Đ12 | K8 · `addkillertasknpc` 160 hàng, **59** npc id `761…768, 770…820` (khuyết 769), **40** map, 8 cấp | Bóc lại `killbosshead.lua` bằng regex `\{…\}`: **160** hàng · npc id duy nhất **59**, dải 761-820, khuyết **đúng 769** · map duy nhất **40** · cấp `{25,35,45,55,65,75,85,95}` = **8** | ✅ **ĐÚNG** | — |
| Đ13 | K5/K7 · `npcs.txt` LINUX **2.353**/103 cột, JX1 **2.035**/87 cột, 0 ô `Name` rỗng | `wc -l` = 2354 / 2036 (⇒ 2353 / 2035 dòng dữ liệu); `head -1 \| tr '\t' '\n' \| wc -l` = **103** / **87** | ✅ **ĐÚNG** | — |
| Đ14 | K19 · `killbosshead.lua` md5 `159b2ace…`, 384.819 B | `md5sum` = **159b2acec0344bf965785cc346460f8e**, `ls -l` = **384819** B | ✅ **ĐÚNG** (số liệu) | ⚠️ Chữ **“md5sum hai cây”** dễ gây nhầm: `D:\ServerLinux\Patch\script\task\tollgate\killbosshead.lua` **KHÔNG tồn tại** (Patch chỉ có 634 tệp `.lua`, khớp §1.4). Nên ghi rõ “hai cây” là **cây nào** |
| Đ15 | K13 · ELF: engine Linux chọn thế hệ item qua `"/settings/item/%03d"`, offset **`0x20B379`** | Dò byte trong `D:\ServerLinux\server1\jx_linux_y`: `settings/item/%03d` ở **0x20B379**, còn chuỗi **đầy đủ** `/settings/item/%03d` bắt đầu ở **0x20B378** | ✅ **ĐÚNG** (lệch 1 byte) | Ghi `0x20B378` nếu muốn trỏ đầu chuỗi có dấu `/` |
| Đ16 | §2.1 · các hằng số biên (`MAX_TIMER_PERMISSION=10`, `MAX_GLOBAL_MISSIONCOUNT=50`, `MAX_SUBWORLD_MISSIONCOUNT=10`, `MAX_NPC_MISSION=5000`, `MAX_SUBWORLD=1000`, `MAX_NPC=98000`, `m_MissionValueC[100]`, `m_MissionValue[100][16]`) | Đọc thẳng: `KMission.h:23,25` · `KSubWorld.h:5,22,23,25` · `KNpc.h:21` · `KMissionArray.h:13,14` — **khớp từng dòng** | ✅ **ĐÚNG** | — |
| Đ17 | K17 · `MAX_TASK = MAX_TEMP_TASK = 4200` (`KPlayerTask.h:18-19`) | `sed -n '18,19p'` ⇒ `#define MAX_TASK 4200` / `#define MAX_TEMP_TASK 4200` | ✅ **ĐÚNG** | — |
| Đ18 | §2.2 · 3 khiếm khuyết mission (strcpy `char[16]`; phạm vi là MAP; `GetMissionV` bỏ ô 0) | `KMissionArray.h:36-41` — dòng **40** `strcpy(m_MissionValue[ulValueId], szValue);`, chỉ chặn `>= 100`. `ScriptFuns.cpp:11002` `Lua_ValueToString(L, 2)` · `:11004` `< 0` · `:11007` `SubWorld[…].m_MissionArray.SetMission` · `:11048` `> 0` · `:11049` `GetMissionValue`. `:15231` `{ "SetMissionV", LuaSetMission }` | ✅ **ĐÚNG** (cả 3, đúng cả số dòng) | — |
| Đ19 | §2.2 **KĐ-a** · “`GetMissionS` dùng `>= 0` ⇒ ba luật biên khác nhau” | `:11023` đúng là `if (nValueId >= 0)`. **Nhưng** `:14740 {"GetMissionS", LuaGetMissionString}` bị **`:15233 { "GetMissionS", LuaJx2GetMissionString }` đè** — chính §2.6 của tài liệu này chứng minh “mục sau thắng” | 🔴 **SAI** (tự mâu thuẫn với §2.6) | Nhánh `>= 0` **không tới được qua tên `GetMissionS`**. Hạ xuống thành **bẫy thứ tự đăng ký**, không phải khiếm khuyết đang sống → đã sửa trong thân bài |
| Đ20 | §2.2 **KĐ-b** · “`memset(m_MissionValueC…)` bị chú thích ở `KMission.h:144,167`” | Hai dòng đó có thật. **Nhưng** `grep m_MissionValueC KMission.h` ⇒ chỉ khớp **hai dòng chú thích ấy**; lớp `KMission` **không khai trường đó** (chỉ có `:126 char m_MissionValue[100][16]`). Trường thật ở lớp khác: `KMissionArray.h:13` | 🔴 **SAI** (bằng chứng lệch lớp) | Đó là **hai dòng chết** (bỏ chú thích sẽ không biên dịch), không phải lỗ khởi tạo. Phần còn giá trị (`KMissionArray` không có constructor) phải trích **`KMissionArray.h:13`** → đã sửa trong thân bài |
| Đ21 | §2.3 · bẫy 1 (`StartMissionTimer` không dò trùng) và bẫy 3 (glb timer ≤ **16**/tick) | `ScriptFuns.cpp:11363-11391`, dòng **11382** `KTimerTaskFun* pTimer = pMission->m_cTimerTaskSet.Add();` — **không có vòng dò**. Đối chứng `KJx2League.cpp:974-983` **có** vòng dò + ghi đè. `:1426` `for (size_t i = 0; i < s_GlbTimers.size() && nFireCount < 16; i++)` | ✅ **ĐÚNG** (cả hai) | Bổ sung: **bẫy thứ 4** — `KJx2League.cpp:1163-1164` `for (… s_ScriptTimers.size() && nFireCount < 64; …)`, trần **64/tick** cho *script* timer, cùng kiểu dồn |
| Đ22 | §2.4 · `g_IsJx2Script` **18 mẫu**, trích nguyên văn; Phong Lăng Độ + Vượt ải **không lọt**, satthu **lọt sẵn** | `KSortScript.cpp:113-133` — đếm mảng `szJx2[]` = **18** phần tử, khối trích **khớp từng ký tự**. Có `"\\script\\task\\tollgate\\"` (satthu lọt), **không có** `fengling_ferry` hay `challengeoftime` | ✅ **ĐÚNG** | — |
| Đ23 | §3.1 · `LuaDisabledUseTownP` là **stub `return 0;`**; JX1 `townportal_l.lua` chỉ gọi `UseTownPortal()`; `forbidmap.lua` **không có** 337/338/339, 464-495, 957 (có 336 và 340) | `KJx2WarInfra.cpp:258-261` thân đúng là `{ return 0; }`; đăng ký `ScriptFuns.cpp:15203`. `script\item\townportal_l.lua`: `:7` Include forbidmap · `:48` `CheckAllMaps(nSubWorldID)` · **`:73 UseTownPortal()`** và **không có** `UseTownPortalEvent:OnEvent`. `forbidmap.lua:3-11` 8 bảng — 336 và 340 nằm trong `TRAINMAPS`; **không bảng nào chứa** 337/338/339/464-495/957; `:94-99` `CheckAllMaps` + nhánh `mapid == 984` | ✅ **ĐÚNG** (toàn bộ, kể cả số dòng) | — (đây là phát hiện thứ hai có giá trị nhất của vòng 2) |
| Đ24 | §3.3 · `RevivePos.ini` **142 section**, có `[175]`, **thiếu** 337/338/339/957/464-495; `GetRevivalPosFromId` **nạp lại INI mỗi lần gọi** | `grep -c "^\["` = **142**; `[175]` = 1; `[337] [338] [339] [957] [464] [480] [495]` = **0** hết. `KSubWorldSet.cpp:196-199` `KIniFile IniFile; g_SetFilePath(SETTING_PATH); if(!IniFile.Load("RevivePos.ini"))` — đối tượng **cục bộ**, **không cache** | ✅ **ĐÚNG** | — |

**Các khẳng định phụ cũng đã kiểm và ĐÚNG** (không lập dòng riêng): §2.5 `activitysys` 809 / 6 / thiếu 803 ·
§2.6 `ScriptFuns.cpp` = **15.348** dòng · K30 `missions.txt` dòng 23 = `22 \script\missions\mission_trong.lua` ·
K31 `Maps\WorldSet_GameServer.ini` `Count=910`, grep `957` = 0 · K33 md5 `eventsys.lua` **1a399f24…** (JX1) ≠
**7c692df4…** (Linux), `OnPlayerEvent` tự viết ở `:74` (nhãn `[WLLS port 20/08/2026]`) và `:79` ·
K34/§4.4 `startgame.lua:99 -- addnpcbosssatthu()` / `:100 addnpcphonglangdo()` **không chú thích** / `:101 --addnpctongkimtrungcap()` ·
K35 `KJx2SharedStore.cpp:440` `JX2LADDER_TOP 10`, `:441` `JX2LADDER_MIN_ID 10000`, `:538` `uId <= …` ·
§3.2 cả 4 hàm CAMP đăng ký thật (`SetRevPos:14589` · `GetCamp:14592` · `SetCurCamp:14593` · `ForbidEnmity:14922` · `SetTmpCamp:15252`; thân `LuaSetTmpCamp` ở `:3859`) ·
§3.4 `script\lib\log.lua` **có tồn tại** ·
§3.5 `FORBITMAP_LIST` chỉ xuất hiện ở `tongcastle\game.lua:163,164,175` ·
§3.6 `skills.txt` **114 cột** cả hai, `SkillId 541` có ở **cả hai** cùng tên “Hoàn thành nhiệm vụ Sát thủ” ·
§3.7 `Pay` đăng ký ở `:14656` · `TimerTask.txt` **36** dòng dữ liệu · `KSubWorld.cpp:1168` `m_MissionArray.Activate();`.

**Một lệch nhỏ đã sửa trong thân bài:** §3.9 ghi `strncpy(e.szName, szName, 31)` ở `:542` — thật là **`:543`**
(`:542` là dòng `memset`).

---

### Bỏ sót của chính vòng 2

**BS-a. 🔴🔴 Chưa hề kiểm CHẶN #2 THẬT ở cây JX1 — và nó vẫn đứng.**
Vòng 2 đọc nhầm “CHẶN #2” thành BS-2 rồi tuyên bố lật, nên **7 tệp `lineup*.txt` không ai kiểm**.
Đối chất đã kiểm: `E:\...\bin\server\settings\maps\challengeoftime\` **không tồn tại**, và glob
`E:\...\bin\server\**\lineup*` = **0 kết quả**. §1.4 của vòng 2 chỉ chứng minh chúng có ở **cả hai gốc
Linux** (A và B) — đó là câu hỏi khác hẳn. ⇒ **Vẫn phải chép 7 tệp sang cây JX1.**

**BS-b. 🔴🔴 Bỏ sót `sJX2RemapScriptPath` — cơ chế làm cho cả `05` lẫn `17` đều kê thừa việc.**
`ScriptFuns.cpp:1938-1967`, gọi ở `:2016`. Engine **tự** đổi `script\lib\` → `scriptjx2\lib\` (và
`script\tong\` → `scriptjx2\tong_vn\`) khi đường gốc `fopen` trượt. Vì `script\lib\file.lua` thật sự
không tồn tại, `Include("\script\lib\file.lua")` **đã chạy đúng từ trước**. Hai hệ quả:
(1) không phải sửa `Include`, không phải chép tệp;
(2) “chứng cứ phụ” của `05:565-566` — *`npcpoint.lua` của chính cây JX1 gọi `GetTabFileData` nên đoạn
đó hiện cũng hỏng/chết* — **SAI**, đoạn đó vẫn chạy.
Đây cũng là lời cảnh báo cho **10 tệp `lib\*` + 5 tệp `tong\*`** trong danh sách “15 tệp nằm sẵn ở
`scriptjx2`” ở §1.3: **cả 15 đều rơi đúng vào hai tiền tố được remap** ⇒ không tệp nào cần đụng tới.

**BS-c. 🔴 Ô “CHƯA XÁC MINH” ở §4.3 trả lời được ngay — và câu trả lời là trường hợp XẤU NHẤT.**
§4.3 để ngỏ *“Nếu wrapper `scriptjx2\lib\file.lua` nạp lại tệp mỗi lần (**CHƯA XÁC MINH**)…”*. Đọc
`LuaTabFile_Load` (`ScriptFuns.cpp:3052-3089`): với tên bảng đã biết thì
`pTab = it->second; pTab->Clear();` rồi **`v4 = pTab->Load(szFileName);`** — **không có nhánh “đã nạp
thì thôi”**, tức **đọc lại tệp từ đĩa MỖI LẦN GỌI**. Mà `scriptjx2\lib\file.lua:39` và `:47` gọi
`TabFile_Load` ở **cả** `GetTabFileHeight` **lẫn** `GetTabFileData`. Ghép với `fld_head.lua:176-179`
(1× Height + 2× Data **cho mỗi con quái**) và `mission.lua:19-24` (30 thuỷ tặc liên tiếp):
⇒ **90 lần mở-đọc-đóng `渡船刷怪点.txt` dồn trong một tick**, trên **luồng logic duy nhất**.
Điểm I/O thứ 3 của §4.3 phải chuyển từ “chưa xác minh” sang **ĐÃ XÁC NHẬN, mức nặng nhất**.
Kèm rò rỉ có chủ ý: `s_mapTabFiles[szName] = new KTabFile;` — *“song suot doi tien trinh”* (`:3074`),
mỗi tên bảng mới giữ vĩnh viễn một `KTabFile`.

**BS-d. 🔴 `ForbidChangePK` — 7 điểm gọi trong 3 tính năng, KHÔNG báo cáo nào (kể cả `17_`) kể tên.**
Nó nằm **ngay cạnh** `DisabledUseTownP` trong cùng tệp, cùng lớp lỗi, mà §3.1 chỉ soi một cái.

```
phonglangdo/missions/fengling_ferry/mission.lua:74   ForbidChangePK(0)
phonglangdo/battles/battlehead.lua:966               ForbidChangePK(0)
vuotai/missions/challengeoftime/chuangguang30.lua:91  ForbidChangePK(1)
vuotai/missions/challengeoftime/chuangguang30.lua:145 ForbidChangePK(0)
vuotai/missions/challengeoftime/mission_match.lua:48  ForbidChangePK(0)
vuotai/battles/battlehead.lua:966                     ForbidChangePK(0)
```

**Tin tốt, nhưng phải ghi lại vì tài liệu nội bộ đang nói ngược:** `KJx2WarInfra.h:24` chú thích
`int LuaForbidChangePK(Lua_State* L);  // no-op nhan doi (lo goc; chua co co cam doi PK tay)` —
**CHÚ THÍCH ĐÃ LỖI THỜI**. Thân hàm `KJx2WarInfra.cpp:243-251` **đã hiện thực thật**:
`Player[nPlayerIndex].m_cPK.SetNormalPKState((BYTE)…GetNormalPKState(), bLock);`.
⇒ `ForbidChangePK` **chạy được**, không cần port. Nhưng ai đọc header rồi kết luận “lại một stub nữa”
sẽ đi vá nhầm — và ngược lại, ai thấy `DisabledUseTownP` ở dòng kế mà tin chú thích thì bỏ sót lỗ thật.
**Phải đọc thân hàm, đừng tin chú thích header.**

**BS-e. 🔴 Bẫy timer còn cái thứ 4, cùng tệp cùng kiểu.**
§2.3 nêu “bẫy 3 = glb timer ≤ 16/tick” (`KJx2League.cpp:1426`) nhưng bỏ qua anh em của nó ở
**`KJx2League.cpp:1163-1164`**: `int nFireCount = 0; for (size_t i = 0; i < s_ScriptTimers.size() && nFireCount < 64; i++)`
— trần **64/tick** cho *script* timer. Cùng cơ chế dồn, và với 32 map Vượt ải chuyển ải đồng loạt
mỗi giờ thì đây mới là trần dễ chạm hơn.

---

### Ba điều nên nhớ từ vòng đối chất này

1. **Bác một kết luận thì phải bác đúng câu chữ của nó.** BS-2 ≠ CHẶN #2. Vòng 2 lật đúng một mệnh đề
   phụ rồi ghi lên tiêu đề là đã lật cả điểm chặn — hệ quả là **7 tệp `lineup*.txt` suýt bị bỏ quên**.
2. **Số dòng đúng không có nghĩa là lớp đúng.** KĐ-b trích đúng `KMission.h:144,167` nhưng trường
   `m_MissionValueC` lại ở `KMissionArray.h:13` — hai lớp khác nhau, kết luận đổi hẳn.
3. **Trước khi kết luận “thiếu / phải sửa”, tìm xem engine có đường tự chữa không.**
   `sJX2RemapScriptPath` xoá sạch một hạng mục việc mà hai vòng liên tiếp đều ghi vào danh sách phải làm.

*Mọi dòng trong mục ĐỐI CHẤT này đều kèm tệp:dòng, md5, offset ELF, hoặc lệnh đếm đã chạy.*
