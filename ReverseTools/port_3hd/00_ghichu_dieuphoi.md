# 00 — Ghi chú điều phối (do phiên chính tự kiểm chứng, KHÔNG qua tác tử)

Mọi mục dưới đây **tôi đã tự chạy lệnh và đọc tận mắt** trong phiên 24/08.
Chúng là căn cứ để đối chiếu lại báo cáo của các tác tử.

---

## 1. Ba tính năng nằm ở đâu trong bản Linux

| Tên tiếng Việt | Tên gốc (Hán) | Thư mục bản Linux |
|---|---|---|
| Săn boss sát thủ | 杀手任务 Boss | `script\task\tollgate\killer\` + `script\task\tollgate\killbosshead.lua` |
| Phong Lăng Độ | 风陵渡 (fengling ferry) | `script\missions\fengling_ferry\` |
| Vượt ải | 闯关 (chuangguan) | `script\missions\challengeoftime\` |

**Bằng chứng "săn boss sát thủ" đúng là hệ này** — `script\activitysys\config\1008\config.lua:189-207`:

```lua
tbConfig[11] = --boss sát thủ
    szName = "Nhiệm vụ sát thủ cấp 90",
    tbCondition = { {"NpcFunLib:CheckKillerdBoss", {90} }, ... }
```

`killbosshead.lua:4` mở đầu bằng `addkillertasknpc = {` với chú thích GBK `--Boss杀手任务npc`,
mỗi dòng trỏ tới `\script\task\tollgate\killer\kill_level.lua`.

**Bằng chứng "vượt ải" = chuangguan** — `activitysys\config\1008\config.lua:77-94`:
`szMessageType = "Chuanguan"`, `szName = "Vượt qua ải 17"`.

---

## 2. 🔴🔴 BẪY LỚN NHẤT: bản Linux có **HAI gốc dữ liệu**, không phải một

| Gốc | Đường dẫn | `settings\` | `script\` | `maps\` |
|---|---|---|---|---|
| A — thư mục chạy | `D:\ServerLinux\server1` | 2388 tệp | 5145 tệp | 747 tệp |
| B — lớp cập nhật | `D:\ServerLinux\Patch` | 1754 tệp | 634 tệp (client: 542 là `skill\`) | 52 tệp |

- **467 tệp `settings\` CHỈ CÓ Ở B**, 55 tệp `script\` lệch kích thước giữa hai gốc.
- Ký ức cũ của dự án ghi `Patch` là "lớp cập nhật CLIENT" — **đúng một nửa**: `Patch\script`
  đúng là client (542/634 tệp là `skill\`), nhưng **`Patch\settings` chứa bảng dữ liệu MÁY CHỦ
  mà `server1` không có**.

**Chứng minh cụ thể, đây là dữ liệu lõi của Phong Lăng Độ:**

`fld_head.lua:18` khai báo
`npcthiefpos = "\settings\maps\中原北区\渡船\渡船刷怪点.txt"`
rồi `fld_head.lua:135` gọi `fld_getadata(npcthiefpos)` để lấy toạ độ thả người chơi lên thuyền.

Tệp đó **KHÔNG có** trong `server1\settings\maps\` (thư mục này không có một thư mục khu vực
tên tiếng Trung nào). Nó nằm ở:

```
D:\ServerLinux\Patch\settings\maps\中原北区\渡船\渡船刷怪点.txt          893 B
D:\ServerLinux\Patch\settings\maps\中原北区\风陵渡北岸\baiyingyingboss.txt  99 B
D:\ServerLinux\Patch\settings\maps\中原北区\风陵渡北岸\yanxiaoqianboss.txt  99 B
D:\ServerLinux\Patch\settings\maps\中原北区\风陵渡南岸\herenwoboss.txt     143 B
D:\ServerLinux\Patch\settings\maps\great_night\风陵渡.txt               1520 B
```

Cùng với **8 bảng rơi đồ của boss sát thủ** `Patch\settings\droprate\boss\bosstask_lev20..90.ini`
(≈6,2 KB mỗi tệp, 645-653 dòng) và các bảng rơi đồ boss Phong Lăng Độ
(`herenwodroprate.ini`, `baiyingyingdroprate.ini`, `heiyishashoudroprate.ini`).

> ⚠️ **Nếu phiên sau chỉ đọc `server1` thì sẽ kết luận nhầm là "thiếu dữ liệu gốc, không port được".**
> Đã trích 106 bảng liên quan ra `src_utf8\_patch_settings\`, bảng kê ở `09_bang_toado_patch.md`,
> danh sách đầy đủ 846 tệp chỉ-có-ở-B ở `chi_co_o_patch.txt`, số liệu ở `08_hai_goc_dulieu.md`.

Tên thư mục tiếng Trung trên đĩa là **byte GBK thô**; Python/`ls` đọc theo mã trang ANSI (CP1258)
nên ra mojibake ⇒ **so khớp phải so BYTE hoặc giải mã trước**, `os.path.join` với chuỗi Unicode
tiếng Trung sẽ báo "không tìm thấy tệp" dù tệp có thật.

---

## 3. Mã hoá tệp nguồn

Các tệp `.lua` trộn **GBK** (chú thích chữ Hán) và **TCVN3** (chuỗi tiếng Việt) — có khi trong cùng
một tệp, phân biệt theo TỪNG DÒNG. Ví dụ `challengeoftime/include.lua`:

```
b'Nhi\xd6m v\xf4 Th\xb8ch th\xf8c th\xeai gian S\xac c\xcap'   ->  TCVN3  ->  "Nhiệm vụ Thách thức thời gian Sơ cấp"
```

Công cụ `D:\GAMEDEVNEW\ReverseTools\gbktool.py` đã nhận dạng đúng theo từng dòng.
Toàn bộ **261 tệp** trong bao đóng đã được xuất bản UTF-8 **giữ nguyên số dòng** ra
`src_utf8\` (bảng kê `src_utf8\INDEX.md`) ⇒ phiên sau đọc thẳng bằng `Read`, không cần công cụ.

---

## 4. Bao đóng `Include()` của ba tính năng (`closure3.json`)

| Tính năng | Tệp trong bao đóng | JX1 **chưa có** | Ghi chú |
|---|---|---|---|
| satthu | 67 | 28 | `killbosshead.lua` (384 KB) **đã được chép sang JX1 từ đợt Tín Sứ 21/08** |
| phonglangdo | 82 | 43 | 1 tệp `global\路人_礼官.lua` không có cả ở bản Linux |
| vuotai | 103 | 54 | nặng nhất |

## 5. "JX1 đã có tệp" ≠ "giống nhau" (`07_doi_chieu_tep.md`)

Đo thật bằng `difflib` trên 252 lượt tệp:

| Trạng thái | Số lượt |
|---|---|
| MISSING (JX1 chưa có) | 125 |
| IDENTICAL (y hệt byte) | 118 |
| DIVERGED (lệch nhiều) | 5 |
| NEAR (≥90% trùng) | 3 |

**Chỉ 3 tệp thật sự phải hợp nhất bằng tay** (bản JX1 đã bị các đợt port trước sửa, DÀI HƠN bản Linux):

| Tệp | Dòng (Linux) | Dòng (JX1) | % trùng |
|---|---|---|---|
| `misc/eventsys/eventsys.lua` | 73 | 107 | 81,1% |
| `task/newtask/newtask_head.lua` | 54 | 89 | 75,5% |
| `item/heart_head.lua` | 290 | 131 | 27,6% |
| `lib/awardtemplet.lua` (NEAR) | 61 | 68 | 94,6% |

> 🔴 **CẤM `copy /Y` đè cả cây** — sẽ nuốt mất phần các đợt trước đã thêm vào 3 tệp này.

---

## 6. Vài điểm API đã tự kiểm chứng (đối chiếu lại báo cáo tác tử `04_api_gap.md`)

### 6.1 `GetTabFileHeight` / `GetTabFileData` **KHÔNG PHẢI hàm engine**

Tìm chuỗi trong `jx_linux_y`: cả `GetTabFileData`, `GetTabFileHeight`, `GetTabFileWidth`
đều **không tồn tại một byte nào** trong binary. Chúng do **script định nghĩa**:
`script\lib\file.lua:38` và `:46`, bọc quanh engine `TabFile_Load` / `TabFile_GetRowCount`
/ `TabFile_GetCell` (nạp qua `IncludeLib("FILESYS")`, `file.lua:1`).

⇒ Không cần viết C++ mới, chỉ cần chép `lib\file.lua`. **Đây là bẫy đếm nhầm khối lượng.**

### 6.2 JX1 **đã có sẵn** phần lớn API mà Phong Lăng Độ cần

Đã grep `Sources\Core\Src\ScriptFuns.cpp` — có đăng ký sẵn:

`Say`:14259 · `Talk`:14266 · `SetTaskTemp`:14269 · `GetTeamMember`:14292 · `GetLastFactionNumber`:14302 ·
`GetTask`:14361 · `SetTask`:14362 · `GetTeamSize`:14365 · `LeaveTeam`:14366 · `Msg2Player`:14368 ·
`Msg2SubWorld`:14370 · `Msg2Region`:14371 · `NewWorld`:14402 · `AddItem`:14413 · `CalcFreeItemCellCount`:14431 ·
`SubWorldID2Idx`:14477 · `SetFightState`:14483 · `AddNpc`:14494 · `SetRevPos`:14589 · `SetCurCamp`:14593 ·
`AddOwnExp`:14617 · `GetMissionV`:14739 · `GetMissionS`:14740 · `AddMSPlayer`:14756 · `GetMSPlayerCount`:14765 ·
`SetDeathScript`:14776 · `SetLogoutRV`:14779 · `SetCreateTeam`:14780 · `CalcItemCount`:14911 ·
`ForbidEnmity`:14922 · `SetNpcDeathScript`:15190 · `DisabledUseTownP`:15203 · `GetItemProp`:15213 ·
`GiveItemUI`:15214 · `GetGiveItemUnit`:15215 · `RemoveItemByIndex`:15216 · `SetMissionV`:15231 ·
`SetMissionS`:15232 · `GetMissionS`:15233 · `AddTimer`:15254 · `GetItemStackCount`:15263 ·
`TabFile_Load`:15284 · `TabFile_GetCell`:15285 · `TabFile_GetRowCount`:15286 · `IniFile_Load`:15288 ·
`GetLocalDate`:15282 · `IncludeLib`:14276 (+ alias `IL`:14282)

⇒ Công của các đợt port JX2 trước (Tín Sứ / Lôi Đài / Bang Chiến). Khối lượng còn lại **nhỏ hơn nhiều** so với dự đoán ban đầu.

### 6.3 `IncludeLib("RELAYLADDER")` — **KHÔNG phải chặn tiến độ** (đã kiểm lại)

`missions\challengeoftime\include.lua:1` mở đầu bằng `IncludeLib("RELAYLADDER")`.
Đọc `ScriptFuns.cpp:2485-2503`: JX1 có nhận tên module này, nhưng ánh xạ nó về
`scriptjx2\lib\noop.lua` — tức **nạp một tệp rỗng, trả về 1, không lỗi**. Lý do ghi ở `:2482-2483`:
*"hàm thật đã nằm ở C, trỏ noop.lua chỉ để dofile không lỗi"*.

Và các hàm ladder thật **đã có sẵn trong JX1**:
`Ladder_NewLadder`:15085 · `Ladder_ClearLadder`:15086 · `Ladder_GetLadderInfo`:15087.

Đếm thật trên toàn bộ cây script Linux: `Ladder_NewLadder` 105 lượt · `Ladder_GetLadderInfo` 50 ·
`Ladder_ClearLadder` 23 · `Ladder_List` 14 · `Ladder__AddOne` 4.
Riêng **Vượt Ải chỉ dùng 2 hàm đầu** (`npc_death.lua:73,77,85,88` · `rank_perday.lua:80` ·
`include.lua:334`) ⇒ **đủ, không phải viết mới**.

> Cũng đã kiểm: `GetCurServerTime`:14873 và `FormatTime2Number`:14877 (mà `rank_perday.lua:33,53` cần)
> đều có sẵn.

**Bẫy còn lại của `IncludeLib`**: nó là *require-once theo cờ global* `__INCLIB_<MOD>`
(`ScriptFuns.cpp:2520-2530`). Bản gốc JX2 là require-once thật; JX1 `dofile` mỗi lần gọi nên
từng gây **đệ quy vô hạn lúc khởi động** (sập 14/08, vì `lib\file.lua:1` chính nó gọi
`IncludeLib("FILESYS")`). Đã vá bằng cách đặt cờ TRƯỚC khi `dofile`. Khi chép thêm lib mới của
3 tính năng phải giữ đúng khuôn này.

### 6.4 Cảnh báo cho người đọc `luamap`: có mục là **hàm rỗng**

`jx_linux_y.luamap.full.txt` ghi `0x080FB820 Msg2Region`, nhưng dịch ngược ra:

```
0x080FB820  push ebp ; xor eax,eax ; mov ebp,esp ; pop ebp ; ret
```

= **stub trả về 0**. ⇒ Có mặt trong bảng đăng ký **không** đồng nghĩa có logic.
Khi dịch ngược một hàm phải nhìn thân hàm, thấy `xor eax,eax; ret` thì kết luận "hàm rỗng phía máy chủ".

### 6.5 `SubWorldID2Idx` được đăng ký **HAI LẦN** trong JX1

`ScriptFuns.cpp:14477` và `:15333` (cùng trỏ `LuaSubWorldIDToIndex`). Không gây sai kết quả
(bảng tra lấy mục khớp đầu tiên) nhưng là mã thừa — ghi lại để đợt dọn dẹp sau xử lý.

---

## 7. Tệp sinh ra trong đợt này

| Tệp | Nội dung |
|---|---|
| `closure3.py` / `closure3.json` | tính bao đóng `Include()` của 3 tính năng |
| `dump_utf8.py` → `src_utf8/` (261 tệp) + `src_utf8/INDEX.md` | **bản mã nguồn Lua đã giải mã UTF-8, giữ nguyên số dòng** |
| `cmp_jx1.py` → `07_doi_chieu_tep.md` | so bản Linux với máy chủ JX1 đang chạy |
| `scan_patch_root.py` → `08_hai_goc_dulieu.md`, `chi_co_o_patch.txt` | đo lệch hai gốc dữ liệu |
| `dump_patch_tables.py` → `09_bang_toado_patch.md`, `src_utf8/_patch_settings/` (106 bảng) | bảng toạ độ/rơi đồ chỉ có ở `Patch` |
| `01_satthu.md` … `06_phia_jx1.md` | báo cáo từng mảng (tác tử) + phụ lục phản biện |

---

## 8. 🔴 LẬT NGƯỢC "CHẶN #1 (CỨNG)" của `05_dulieu.md` — dữ liệu KHÔNG mất

`05_dulieu.md` mục 6 kết luận Phong Lăng Độ **chặn cứng** vì thiếu bảng toạ độ spawn, sau khi
đã quét đệ quy `D:\ServerLinux\server1`, băm `KPakList::FileNameToId` và dò **44 pak /
514.459 mục**. Công phu, nhưng cả vòng 1 **chỉ tìm trong gốc A** và không biết gốc B tồn tại.

**Tệp có thật, tôi đã đọc tận nơi:**

| Tệp (gốc B `D:\ServerLinux\Patch\settings\`) | Byte | Dòng dữ liệu |
|---|---|---|
| `maps\中原北区\渡船\渡船刷怪点.txt` | 893 | **63** |
| `maps\中原北区\风陵渡北岸aiyingyingboss.txt` | 99 | 8 |
| `maps\中原北区\风陵渡北岸\yanxiaoqianboss.txt` | 99 | 8 |
| `maps\中原北区\风陵渡南岸\herenwoboss.txt` | 143 | 12 |
| `maps\great_night\风陵渡.txt` | 1520 | 100 (3 cột: `map_ID nPosX nPosY`, toàn bộ map 336) |

`渡船刷怪点.txt` — 2 cột `XPOS`/`YPOS`, **đơn vị pixel**, khớp đúng suy đoán của vòng 1:

```
XPOS	YPOS
50400	102240      -> ô (1575, 3195)
...
53152	103072      -> ô (1661, 3221)
```

Dải: X 50400…53152 = ô **1575…1661**; Y 101344…104096 = ô **3167…3253**.
Bao quanh `boatMAP_POS = {1646, 3233}` (`fld_head.lua:13`) và vùng boss `1636*32, 3221*32`
(`boss.lua:28`). ⇒ `fld_head.lua:136-137` chia 32 ra đúng toạ độ ô.

> ✅ **CHẶN #1 KHÔNG CÒN.** Không phải soạn lại bảng — chép nguyên 63 dòng.
> Bản đã giải mã UTF-8 nằm sẵn ở `src_utf8\_patch_settings\maps\中原北区\...`.

Cùng lý do đó, mọi kết luận "thiếu / không tìm thấy / chặn tiến độ" của vòng 1 đều **phải
kiểm lại ở gốc B trước khi tin**. Vòng 2 đã giao riêng một hướng quét cho việc này.

---

## 9. Tự kiểm chứng RỦI RO SỐ 1: `AddNpc` lệch ngữ nghĩa tham số 6 — **CÓ THẬT**

Tác tử phản biện mảng sát thủ báo lệch. Tôi đã tự dịch ngược lại và tự đọc mã JX1, **xác nhận đúng**.

### Bản Linux — `AddNpc` tại `0x0811BB10` tự sinh ngũ hành ngẫu nhiên

```asm
0x0811BBDF  call 0x804b28c      ; PLT -> rand()      ; edi = rand()
...
0x0811BC72  mov  eax, edi
0x0811BC74  imul edx            ; nhân số kỳ diệu (chia 5)
0x0811BC82  sar  eax, 0x1f
0x0811BC85  sar  edx, 1
0x0811BC87  sub  edx, eax       ; edx = rand()/5
0x0811BC89  lea  eax, [edx+edx*4]
0x0811BC8C  sub  edi, eax       ; edi = rand() % 5      <-- NGŨ HÀNH
0x0811BC8E  mov  [esp+4], edi   ; -> ĐỐI SỐ THỨ 2 của hàm tạo NPC
0x0811BC9C  call 0x809fb10
```

Đây là khuôn chia dư có dấu chuẩn của GCC. Ngoài ra `imul edx, eax, 0x1a4c` (`0x0811BCB0`)
cho `sizeof(KNpc) = 6732`, khớp với việc tham số 6 đi vào cờ `[KNpc+0x1824]`.

### Bản JX1 — cùng ô đó lại lấy từ **tham số Lua thứ 6**

`ScriptFuns.cpp:6814` `LuaAddNpc`:
```c
6834:  int nSeries = (int)lua_tonumber(L, 6);
6835:  int nNpcIdx = NpcSet.AddNpcSet2(nNpcIdxInfo, nSeries, /*subworld*/3, /*x*/4, /*y*/5);
6836:  if (Lua_GetTopIndex(L) >= 7 && Lua_IsString(L, 7))   // tham số 7 = TÊN
```
Và `:6823-6827` cho thấy **tham số 1 dạng CHUỖI đã bị chú thích**, JX1 chỉ nhận id số.

### Hệ quả cụ thể

`add_killertasknpc` gọi `AddNpc(npcId, level, mapIdx, x, y, 0, "Tên", 1)`.
Chạy trên JX1 ⇒ `nSeries = 0` ⇒ **cả 160 boss đều hệ Kim**.
Hỏng: hợp thành 5 Sát Thủ lệnh cùng ngũ hành (`lib_killlevel.lua:70-107` truyền `series` vào `AddItem`),
và luật tương khắc ngũ hành khi đánh.

### Lối ra đã có sẵn — dùng `AddNpcEx`

`ScriptFuns.cpp:6874` `LuaAddNpcEx` (đăng ký `:14495`):
```c
6905:  int nSeries      = (int)lua_tonumber(L, 3);
6906:  int nSubWorldIdx = (int)lua_tonumber(L, 4);
6907:  int nX = ...5 ;  6908:  int nY = ...6 ;
6887-6893: tham số 1 CHẤP NHẬN CHUỖI (tra g_NpcSetting.FindRow)
```
Thứ tự này **khớp bản Linux** (`AddNpcEx` Linux đẩy tham số 3 vào đúng ô ngũ hành).

⇒ Khi port: **đổi lời gọi sang `AddNpcEx` và tự truyền `random(0,4)`** cho ngũ hành, đừng chép nguyên
`AddNpc(...)` của bản Linux.

> 🔴 Rủi ro này **đã hiện thực trong chính cây JX1**: `tinsu_addnpc.lua:38` (đợt Tín Sứ 21/08)
> đang gọi `AddNpc(..., 0, name)` — tức mọi NPC Tín Sứ hiện cũng đang hệ Kim hết.
> Cần kiểm lại đợt đó luôn, không chỉ đợt này.
