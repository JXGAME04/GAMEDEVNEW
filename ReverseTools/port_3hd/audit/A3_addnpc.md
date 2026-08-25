# A3 — Soát chuyển đổi `AddNpc` → `AddNpcEx` (14 chỗ) — đợt 3HD 25/08

Phạm vi: 14 chỗ do `thicong\b2_patch.py` đổi, cộng phần rà "còn sót / sửa nhầm / đơn vị toạ độ".
Mọi kết luận dưới đây đều có `tệp:dòng` hoặc địa chỉ dịch ngược. Không sửa gì — chỉ đọc.

---

## 0. TÓM TẮT — 1 lỗi CHẶN nuốt trọn cả 3 hoạt động

> 🔴🔴 **CHẶN-1**: `LuaAddNpcEx` (ScriptFuns.cpp:6982-6995) ánh xạ **tham số 7 → `SetCurrentCamp()`**.
> Tham số 7 của bản Linux **KHÔNG PHẢI phe** — nó là `不重生 / bNoRevive`. Bản gốc đặt phe bằng
> hàm RIÊNG `SetNpcCurCamp`. Sau khi đổi 13 lời gọi quái/boss sang `AddNpcEx`, **mọi con quái và
> boss của cả 3 hoạt động bị ép về phe 0 (`camp_begin`) hoặc phe 1 (`camp_justice`)**, mà bảng quan
> hệ `KNpcSet::GenOneRelation` trả **`relation_ally`** cho cả hai trường hợp ⇒ **người chơi KHÔNG
> ĐÁNH ĐƯỢC, quái cũng KHÔNG ĐÁNH LẠI**. Chi tiết ở mục 4.

> 🔴 **CHẶN-1b (LAN SANG TÍNH NĂNG ĐANG CHẠY)**: chỗ vá #1 (`killbosshead.lua:189`) **không chỉ**
> phục vụ hoạt động mới. `startgame.lua:117` → `tinsu_addnpc()` (`tinsu_addnpc.lua:44-47`) →
> `add_alltollgatenpc()` (`addtollgatenpc.lua:16-17`) **cũng gọi chính hàm đó** cho 9 Bảo rương
> (id 844) và **9 "Bảo Khố Thủ Hộ Giả" (id 849, `Kind=0 / Camp=5` — quái thật)** của ải Thiên Bảo Khố
> đợt Tín Sứ 21/08. ⇒ **Vá 25/08 làm hỏng luôn một tính năng đã bàn giao trước đó.**

> 🔴🔴 **CHẶN-2 (BONUS, ngoài hướng — mục 9b)**: `settings\TimerTask.txt` của cây sống **thiếu 5 dòng
> id 28/29/41/42/43** ⇒ `StartMissionTimer` của Phong Lăng Độ (`mission.lua:15-16`) và Vượt Ải
> (`challengeoftime\include.lua:7-9`) không giải ra được đường dẫn script ⇒ **hai hoạt động này
> không chạy nổi một dòng nào**. Tệp không có trong `b1_manifest.txt`.

Ngoài ra: 7 chỗ `AddNpc(` **còn sót** (mục 5), 3 chỗ ghi chú sai/lệch quy ước (mục 6, 7).
Phần **đơn vị toạ độ (câu hỏi 7) là ĐÚNG, không phải chặn** — mục 8.

---

## 1. Chữ ký `AddNpcEx` của JX1 (đọc thân hàm)

`D:\GAMEDEVNEW\Sources\Core\Src\ScriptFuns.cpp:6937-7009` (`LuaAddNpcEx`, đăng ký `:14495`):

| # | Đọc ở dòng | Kiểu | Ý nghĩa trong JX1 | Ghi chú |
|---|---|---|---|---|
| 1 | 6946-6960 | số **hoặc chuỗi** | `nId` (chuỗi ⇒ `g_NpcSetting.FindRow(pName) - 2`) | âm ⇒ ép 0 |
| 2 | 6964-6966 | số | `nLevel`, kẹp `[1,127]` (≥128 ⇒ 127; <0 ⇒ 1) | |
| 3 | 6968 | số | **`nSeries` (ngũ hành)** | |
| 4 | 6969 | số | `nSubWorldIdx` (chỉ số, KHÔNG phải map id) | |
| 5 | 6970 | số | `nX` — **đơn vị MPS = pixel** | |
| 6 | 6971 | số | `nY` — **đơn vị MPS = pixel** | |
| 7 | 6982-6995 | số | 🔴 **`SetCurrentCamp(nCamp)` nếu `0 ≤ v < camp_num`** *và* `m_bNoRevive = 1` nếu `g_IsJx2Script(L)` và `v != 0` | **ĐÂY LÀ CHỖ SAI** |
| 8 | 6998-7003 | chuỗi | tên hiển thị (chỉ đặt khi chuỗi khác rỗng) | |
| 9 | 7005 | — | **KHÔNG ĐỌC** (chỉ có 1 dòng chú thích "bỏ qua") | |

- Yêu cầu tối thiểu `nTop >= 6` (`:6943`).
- `nNpcIdxInfo = MAKELONG(nLevel, nId)` (`:6973`) → `NpcSet.AddNpcSet2(info, nSeries, nSubWorldIdx, nX, nY)`.
- `AddNpcSet2` (`KNpcSet.cpp:448-457`) gọi `SubWorld[..].Mps2Map(nMpsX, nMpsY, ...)` ⇒ **X/Y phải là pixel**.

## 2. Chữ ký `AddNpc` bản Linux — TỰ DỊCH NGƯỢC LẠI `0x0811BB10`

Lệnh: `python D:\GAMEDEVNEW\ReverseTools\re_disasm.py D:/ServerLinux/server1/jx_linux_y 0x0811BB10 150`

| Địa chỉ | Mã | Kết luận |
|---|---|---|
| `0x0811BB2D` | `cmp eax,4 / jg` | cần `nTop >= 5`, nếu không → `xor eax,eax; ret` |
| `0x0811BB53` | `lua_type(L,1)`; `cmp eax,2 → je 0x811bd08` | tham số 1 nhận **SỐ** (nhánh 0x811bd08) |
| `0x0811BB6C/80/98` | `lua_isstring(L,1)` → `lua_tostring(L,1)` → `FindRow(0x830aec0)` → `sub eax,2` | tham số 1 cũng nhận **CHUỖI** |
| `0x0811BBAE` | `lua_tonumber(L,2)` → `movzx edx,ax`, `cmovs ←1` | `nLevel` (16 bit thấp; âm ⇒ 1) |
| `0x0811BBDF` | `call 0x804b28c` = `rand()` | |
| `0x0811BC72-8C` | `imul 0x66666667 … lea [edx+edx*4]; sub edi,eax` | **`edi = rand() % 5` = ngũ hành, engine TỰ SINH** |
| `0x0811BBF7 / BC07 / BC1A` | `lua_tonumber(L,5)` / `(L,4)` / `(L,3)` | |
| `0x0811BC3A-BC95` | xếp đối số | `[esp+4]=rand()%5`, `[esp+8]=MAKELONG`, `[esp+0xc]=(int)p3`, `[esp+0x10]=(int)p4`, `[esp+0x14]=(int)p5`, `[esp+0x18]=-1|p9` |
| `0x0811BD70-BDB2` | `lua_tonumber(L,6)` → `setne byte [KNpc+0x1824]` | **tham số 6 = cờ BYTE `bNoRevive`** (chỉ khác-0/bằng-0) |
| `0x0811BDC2-BDD9` | `lua_tostring(L,7)`, khác NULL & khác rỗng | **tham số 7 = TÊN** |
| `0x0811BDF0-BE25` | `lua_tonumber(L,8)`; `==1 → 0x811bef2`, `==2 → reset [KNpc+0x88]` | **tham số 8 = cờ boss** |
| `0x0811BD40` | chỉ khi `nTop > 8`: `lua_tonumber(L,9)` → đối số 7 của hàm tạo | tham số 9 = phụ, mặc định `-1` |

⇒ **Linux `AddNpc(id, level, subworldIdx, X, Y, bNoRevive, szName, bIsBoss [, extra])`**, ngũ hành sinh trong engine.
⇒ **Linux `AddNpcEx` (`0x0811BF40`, `cmp eax,5/jg` ⇒ `nTop>=6`) chỉ chèn thêm `nSeries` ở vị trí 3**, phần còn lại dời 1 ô.

### 2b. Ba bằng chứng ĐỘC LẬP (không phải dịch ngược) rằng tham số 7 của `AddNpcEx` = `bNoRevive`, KHÔNG phải phe

1. `src_utf8\phonglangdo\missions\basemission\lib.lua:33-45` — chính bản Linux chú thích từng tham số:
   `1 npc Id · 2 等级 · 3 五行 · 4 地图 · 5 X坐标 · 6 Y坐标 · 7 不重生 · 8 名字 · 9 是否BOSS`
   và **phe đặt riêng** ở `:52` `SetNpcCurCamp(nNpcIndex, tbNpc.nCurCamp)`.
2. `src_utf8\phonglangdo\battles\battlehead.lua:437` — tham số đó tên biến là `l_removedeath`
   (`AddNpc(tnpcid, level, SubWorld, x, y, l_removedeath, l_name, l_boss)`), phe đặt bằng
   `SetNpcCurCamp(npcidx, camp)` ở `:438`.
3. Quy ước **của chính JX1**: `script\lib\lib_map.lua:91-109` (`AddNpcEx1`) và `:134-156` (`AddNpcEx3`)
   đều gọi `AddNpc(id, lvl, mapidx, X, Y, nSeries)` rồi đặt phe bằng `SetNpcCurCamp(nNpcId, nCurCamp)`.
   **Trong JX1 phe CHƯA BAO GIỜ là tham số của `AddNpc`.**

---

## 3. BẢNG 14 CHỖ — đối chiếu CỘT-THEO-CỘT

Ký hiệu: `L#` = vị trí ở bản Linux, `J#` = vị trí sau khi đổi sang `AddNpcEx` của JX1.
Cột "SAI?" chỉ nói về **lệch ý nghĩa**, không nói về giá trị.

Ánh xạ chung (đúng cho cả 14 chỗ):

| Linux `AddNpc` | Ý nghĩa Linux | JX1 `AddNpcEx` | Ý nghĩa JX1 | SAI? |
|---|---|---|---|---|
| L1 | npc id | J1 | npc id | OK |
| L2 | level | J2 | level (kẹp ≤127) | OK |
| — | (engine `rand()%5`) | **J3 = `random(0,4)`** | nSeries | **OK — đúng ý đồ** |
| L3 | subworld **index** | J4 | subworld index | OK |
| L4 | X (pixel) | J5 | X (pixel) | OK |
| L5 | Y (pixel) | J6 | Y (pixel) | OK |
| L6 | **bNoRevive** (`KNpc+0x1824`) | J7 | 🔴 **`SetCurrentCamp()`** + (nếu jx2) `m_bNoRevive` | **SAI Ý NGHĨA** |
| L7 | tên | J8 | tên | OK |
| L8 | cờ boss (`0x8085250`, `KNpc+0x181C = 2/3`) | J9 | **bị bỏ qua** | mất, xem 3b |

### Bảng chi tiết 14 chỗ (cây sống `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server`)

| # | Tệp:dòng (sau vá) | NPC | Kind/Camp bảng `NpcS.txt` | Giá trị J7 | Phe SAU khi vá | Quan hệ với người chơi | Mức |
|---|---|---|---|---|---|---|---|
| 1 | `script\task\tollgate\killbosshead.lua:189` (`add_killertasknpc`) | 160 boss sát thủ (761…820) **+ 9 Bảo rương 844 + 9 Bảo Khố Thủ Hộ Giả 849 của Tín Sứ** | 0 / 5 `camp_animal` (849, 761-820); 3 / 6 (844) | `Tab3[i][6]` = **0** (mọi dòng, xem `killbosshead.lua:6-179`, `:3072-3389`, `:2609-2931`) | **0 `camp_begin`** | 🔴 `relation_ally` với MỌI phe | **CHẶN** |
| 2 | `killbosshead.lua:3399` (`add_bossnpc`) | `AddNpc_flyboss` | — | `Tab6[i][6]` = 0 | 0 | ally | NHẸ (chết: bảng 0 dòng active + `addtollgatenpc.lua:18` đã chú thích) |
| 3 | `killbosshead.lua:3410` (`add_messengernpc`) | `AddNpc_allbugbear` | — | `Tab4[i][6]` = 0 | 0 | ally | NHẸ (bảng 0 dòng active, dòng 366-2605 chú thích hết) |
| 4 | `script\missions\fengling_ferry\boss.lua:28` | 513 Diệu Như | 0 / 5 | **1** | **1 `camp_justice`** | 🔴 ally (người chơi trên thuyền có phe 1, xem `fld_head.lua:127`) | **NẶNG** (mã chết: `boss.lua:19-24` chú thích hết) |
| 5 | `boss.lua:35` | 511 Trương Tông Chính | 0 / 5 | 1 | 1 | như trên | NẶNG (mã chết) |
| 6 | `boss.lua:44` | 513 | 0 / 5 | 1 | 1 | như trên | NẶNG (mã chết) |
| 7 | `boss.lua:51` | 511 | 0 / 5 | 1 | 1 | như trên | NẶNG (mã chết) |
| 8 | `boss.lua:60` | 513 | 0 / 5 | 1 | 1 | như trên | NẶNG (mã chết) |
| 9 | `boss.lua:67` | 511 | 0 / 5 | 1 | 1 | như trên | NẶNG (mã chết) |
| 10 | `fld_smalltimer.lua:37` (UPBOSS_TIME) | 725 Boss Thuỷ tặc đầu lĩnh | 0 / 5 | **1** | **1** | 🔴 ally với 100% người chơi trên thuyền | **CHẶN** |
| 11 | `fld_smalltimer.lua:44` (UPBOSS_TIME2) | 725 | 0 / 5 | 1 | 1 | 🔴 ally | **CHẶN** |
| 12 | `fld_smalltimer.lua:51` (đại đầu lĩnh) | 1692 Thuỷ Tặc Đại Đầu Lĩnh | 0 / 5 | 1 | 1 | 🔴 ally | **CHẶN** |
| 13 | `fld_smalltimer.lua:60` (UPBOSS_TIME3) | 725 | 0 / 5 | 1 | 1 | 🔴 ally | **CHẶN** |
| 14 | `mission.lua:22` (`RunMission`, 30 con) | 724 Thuỷ tặc | 0 / 5 | **0** | **0 `camp_begin`** | 🔴 ally với MỌI phe | **CHẶN** |

Giá trị `Kind/Camp` lấy trực tiếp từ `E:\...\bin\server\settings\NpcS.txt` (cột `Kind` = 1, `Camp` = 2,
hàng = id + 2). Tên trong bảng khớp đúng tên tiếng Việt trong script Linux ⇒ id không lệch.

### 3b. Trả lời riêng ba câu hỏi trong đề bài

* **Tham số 7 (`noRevive`) nhận giá trị gì?** — `killbosshead` truyền **0** (cột 6 của cả 160 dòng),
  `fengling_ferry` truyền **1**. Cả hai đều đi thẳng vào `SetCurrentCamp` (dòng 6986) vì điều kiện
  chỉ là `nCamp >= 0 && nCamp < camp_num` — **giá trị 0 KHÔNG bị lọc**.
  Riêng `m_bNoRevive` chỉ được đặt khi `g_IsJx2Script(L)` **và** giá trị ≠ 0 ⇒
  – `fld_smalltimer.lua` (state riêng, đường dẫn `\script\missions\fengling_ferry\` có trong
    `KSortScript.cpp:126`) ⇒ có đặt `m_bNoRevive`, khớp Linux ✔
  – `killbosshead.lua` chạy trong **state của `hd3_driver.lua`** (Include dùng state tệp gọi,
    `KSortScript.cpp:111-112`), mà `\script\tinhnang\3hoatdong\` **KHÔNG có** trong danh sách
    `g_IsJx2Script` ⇒ `m_bNoRevive` không đặt — nhưng giá trị vốn là 0 nên **không lệch** ở đây.
    (Vẫn là bẫy cho các hàm JX2 khác gọi trong cùng state — ngoài phạm vi hướng này.)

* **JX1 có đọc tham số 9 (`isboss`) không?** — **KHÔNG** (`ScriptFuns.cpp:7005` chỉ là chú thích).
  Bản Linux khi `isboss == 1` gọi `0x08085250` (dịch ngược: tra bảng `0x836eb00` theo
  `[KNpc+0x1530]`, nạp danh sách kỹ năng/thuộc tính vào `KNpc+0x248` qua `0x80e4310`) rồi đặt
  `KNpc+0x181C = 3` (nếu `bNoRevive`≠0) hoặc `= 2`.
  – Phần "nạp thuộc tính từ mẫu" thì `AddNpcSet1 → KNpc::Load(...)` (`KNpcSet.cpp` dòng
    `Npc[i].Load(nNpcSettingIdx, nLevel, nSeries)`) của JX1 **đã tự làm** cho MỌI npc
    (`KNpc.cpp:7997-8003` chép `m_Camp/m_Series/m_SkillList/m_AiMode` từ `pNpcTemp`) ⇒ không mất.
  – Phần `KNpc+0x181C = 2/3` thì JX1 **không có trường tương ứng** ⇒ **mất hẳn**.
    **CHƯA XÁC MINH** trường này làm gì (ứng viên: lớp vòng đời/huỷ xác NPC). Rủi ro thấp vì
    "boss" trong JX1 đến từ mẫu NPC + `SetNpcExp/SetNpcLife`, nhưng nên ghi nhận.

* **`Tab3[i][6..8]` là gì?** — đọc bảng `addkillertasknpc` (`killbosshead.lua:4-180`, 160 dòng active),
  mẫu dòng: `{804,85,181,1598,3111,0,"Viên Niệm Tịch",1,"\script\...\kill_level.lua",133}`
  ⇒ `[1]` npcId · `[2]` level · `[3]` **mapID** (đổi qua `SubWorldID2Idx` ở `:184`) · `[4]` X ô ·
  `[5]` Y ô (×32 ở `:186-187`) · **`[6]` = `bNoRevive` = 0** · **`[7]` = tên** · **`[8]` = cờ boss = 1** ·
  `[9]` script · `[10]` tham số 1 của NPC.

---

## 4. 🔴🔴 CHẶN-1 — chứng minh đầy đủ vì sao quái thành "đồng minh"

**Bảng quan hệ** `KNpcSet::GenOneRelation` (`KNpcSet.cpp:128-165`), theo đúng thứ tự:

```
130  Kind == kind_dialoger                                -> relation_dialog
136  Camp == camp_event                                   -> relation_none
139  (camp_begin & camp_animal)                           -> relation_enemy   <-- luật giữ cho quái đánh được
143  Camp1 == camp_begin || Camp2 == camp_begin           -> relation_ALLY
155  Camp1 == Camp2                                       -> relation_ALLY
158  kind_normal (một trong hai)                          -> relation_enemy
```

`GetRelation` (`KNpcSet.cpp:1415-1446`) tra bảng bằng **`m_CurrentCamp`**, đúng ô mà
`SetCurrentCamp` ghi (`KNpc.cpp:451-453`).

**Phe của người chơi:**
* mặc định = phe MÔN PHÁI (`KPlayer.cpp:4144/4167` `SetCamp(m_cFaction.GetGurFactionCamp())`;
  `settings\faction\FactionInfo.ini`: 正派→`camp_justice`=1 (Thiếu Lâm/Nga My/Cái Bang/Võ Đang),
  中立→`camp_balance`=3, 邪派→`camp_evil`=2).
  `KPlayerFaction.cpp:142-150`: **chưa từng vào phái ⇒ `camp_begin` = 0**;
  đã phản môn (`m_nAddTimes != 0`) ⇒ `camp_free` = 4.
* **Phong Lăng Độ ép phe 1 cho mọi người lên thuyền**: `fld_head.lua:127` `SetCurCamp(1);`
  (`LuaSetCurCamp` → `ScriptFuns.cpp:8188` `SetCurrentCamp(nValue)`).
* Vượt Ải: `chuangguang30.lua:138` / `mission_match.lua:42` `SetCurCamp(GetCamp())` = phe môn phái.

**Suy ra:**

| Hoạt động | Phe quái sau vá | Phe người chơi | Dòng luật trúng | Kết quả |
|---|---|---|---|---|
| Săn boss sát thủ (chỗ #1, 160 boss) | 0 | bất kỳ | `:143` | **ally** — không ai đánh được |
| Tín Sứ / Thiên Bảo Khố — 9 Hộ Thủ Giả 849 (cùng chỗ #1) | 0 | bất kỳ | `:143` | **ally** — hỏng tính năng cũ |
| Phong Lăng Độ — 30 thuỷ tặc (#14) | 0 | 1 | `:143` | **ally** |
| Phong Lăng Độ — 4 boss (#10-13) | 1 | 1 (ép ở `fld_head.lua:127`) | `:155` | **ally** |
| Vượt Ải — quái ải (`npc.lua:431`, xem mục 6) | 1 | 1 (chính phái) | `:155` | **ally** |
| Vượt Ải — quái ải | 1 | 0 (chưa vào phái) | `:143` | **ally** |
| Vượt Ải — quái ải | 1 | 2 tà / 3 trung lập / 4 phản môn | `:158` | enemy — **vẫn đánh được** ⇒ hỏng KHÔNG ĐỀU giữa các phái |

`relation_ally` chặn ở **cả hai chiều**:
* đạn/kỹ năng: `KMissle.cpp:682/716/1849` lọc mục tiêu bằng `FindNpc(..., m_eRelation)`;
* AI quái: `KNpcAI.cpp:801` `nRelation = NpcSet.GetRelation(...)`;
* đánh tay/auto: `KPlayer.cpp:9426`, `CoreShell.cpp:6586/7528/8747`.
⇒ Quái đứng im, người chơi không bấm đánh được. **Hoạt động rỗng hoàn toàn.**

**Trước khi vá thì KHÔNG có lỗi này**: `LuaAddNpc` (`ScriptFuns.cpp:6877-6907`) **không đụng phe**
— quái giữ `m_Camp = 5 camp_animal` từ mẫu ⇒ trúng luật `:139` ⇒ `relation_enemy`.
Nói cách khác **chính đợt vá 25/08 tạo ra lỗi này**, đổi một lỗi nhẹ (ngũ hành Kim) thành lỗi chặn.

### 4b. Hướng vá đề nghị (KHÔNG tự sửa — để phiên chính quyết)

Vá tại **một điểm nghẽn** `ScriptFuns.cpp:6982-6995`: bỏ hẳn nhánh `SetCurrentCamp(nCamp)`,
giữ lại đúng `m_bNoRevive`. Lý do an toàn:

* 3 điểm gọi `AddNpcEx` cũ của JX1 đều truyền **1** ở vị trí 7 và đều là **quái thù địch**:
  `script\item\bosscharm.lua:99` (boss hoàng kim 562-583), `script\item\seasonnpc_item.lua:57`
  (1237/1238), `script\skill\special\spider_web.lua:42` (1632) — cả ba template đều `Camp=5`
  trong `NpcS.txt`. Bỏ `SetCurrentCamp` **trả chúng về `camp_animal`, tức là SỬA luôn cho chúng**
  (hiện tại chúng cũng đang bị ép về `camp_justice` ⇒ chính phái/chưa vào phái không đánh được).
* Nếu vẫn muốn giữ đường ép phe cho script nào đó, dùng đúng hàm sẵn có
  `SetNpcCurCamp` (`ScriptFuns.cpp:8199`) — đúng khuôn cả bản Linux lẫn `lib_map.lua`.

Ghi chú kèm: nếu **không** vá C++ mà vá script, phải sửa **toàn bộ 16 điểm gọi** (14 chỗ ở bảng
mục 3 + `challengeoftime\npc.lua:431` + `battles\battlehead.lua:77`) cộng 2 tệp mới ở mục 6 —
dễ sót hơn nhiều.

---

## 5. CÒN SÓT `AddNpc(` — quét lại 101 tệp manifest + 4 tệp mới + 4 tệp bị sửa

Quét `\bAddNpc\(` trên toàn bộ `b1_manifest.txt` (phần `.lua`) + `hd3_driver.lua`,
`hd3_admin.lua`, `autoexec_npc_hd3.lua`, `hd3_thuyenphu.lua`, `startgame.lua`, `timerserver.lua`,
`cauhinh_hoatdong.lua`, `lenhbaiadmin.lua` ⇒ **còn 7 chỗ**:

| # | Tệp:dòng | Lời gọi | Tác động trên JX1 | Có chạy không? | Mức |
|---|---|---|---|---|---|
| S1 | `script\activitysys\config\32\talkdailytask.lua:27` | `AddNpc(tbPos.nNpcRes, 1, SubWorldID2Idx(nMapId), tbPos.nX*32, tbPos.nY*32, 0, tbPos.szNpcName)` | tham số 6 = 0 ⇒ **ngũ hành ép Kim**; tên (tham số 7) vẫn đúng; **phe KHÔNG bị đụng** | 🔴 **CÓ** — `fld_smalltimer.lua:13` gọi `%tbTalkDailyTask:AddTalkNpc(...)` mỗi khi thuyền rời bến | **NHẸ** (NPC đối thoại, ngũ hành vô nghĩa) |
| S2 | `script\battles\battlehead.lua:384` | `AddNpc(tnpcid, 1, SubWorld, x, y, 1, name)` | tham số 6 = 1 ⇒ **ngũ hành ép Mộc** cho mọi NPC đối thoại chiến trường | không, chỉ gọi trong luồng `battles` (chưa nối) | NHẸ |
| S3 | `battlehead.lua:386` | `AddNpc(tnpcid, 1, SubWorld, x, y)` | 5 tham số — Kim | như trên | NHẸ |
| S4 | `battlehead.lua:400` | `AddNpc(..., 1 , name)` | Mộc | như trên | NHẸ |
| S5 | `battlehead.lua:402` | `AddNpc(tnpcid, 1, SubWorld, x, y )` | Kim | như trên | NHẸ |
| S6 | `battlehead.lua:437` (`bt_addfightnpc`) | `AddNpc(tnpcid, level, SubWorld, x, y, l_removedeath, l_name, l_boss)` | `l_removedeath` (0/1) rơi vào **ô ngũ hành**; `l_name` rơi vào ô tham số 7 = tên ⇒ tên vẫn đúng; `l_boss` bị bỏ | không (gọi từ `battlehead.lua:586/602/648/652` của hệ chiến trường) | NHẸ→NẶNG nếu nối hệ `battles` |
| S7 | `battlehead.lua:482` (`bt_addrandfightnpc`) | như S6 | như S6 | như S6 | như S6 |

> Nhận xét: `battlehead.lua` là tệp **MỚI chép** (có trong `b1_manifest.txt`, và `b1_copy.py` chỉ chép
> tệp JX1 chưa có) — nên nó **không đè** hệ chiến trường cũ của JX1, nhưng nó bị bỏ quên khỏi
> danh sách `NPC_FILES` của `b2_patch.py:59-64` dù `boss.lua:2` có `Include` nó.
> `battlehead.lua:77` lại **đã** dùng `AddNpcEx(..., 1, name, 1)` sẵn ⇒ dính đúng CHẶN-1 (phe 1).

> Ghi thêm (ngoài bộ tệp của đợt này, để phiên chính khỏi bỏ sót):
> `script\task\tollgate\tinsu_addnpc.lua:38` `AddNpc(itemlist[1], 1, SId, x*32, y*32, 0, itemlist[6])`
> — đúng lỗi "mọi NPC hệ Kim" đã ghi ở `00_ghichu_dieuphoi.md` mục 9, **vẫn chưa vá**.
> Cả 18 NPC ở đây là `kind_dialoger` nên chỉ ở mức NHẸ; **đừng vá bằng cách đổi sang `AddNpcEx`
> khi CHẶN-1 chưa xử lý**, nếu không 18 Dịch quan / Tiêu Trấn / Cánh trắng sẽ bị ép phe 0 luôn.

---

## 6. Hai điểm gọi `AddNpcEx` MỚI TỰ VIẾT — cũng dính tham số 7

| Tệp:dòng | Lời gọi | Vấn đề | Mức |
|---|---|---|---|
| `script\tinhnang\3hoatdong\hd3_driver.lua:60` | `AddNpcEx(240, 1, random(0,4), nIdx, t[1]*32, t[2]*32, **1**, "Thuyen phu")` | NPC 240 `Kind=3 (dialoger) / Camp=6 (camp_event)` → bị ép về **phe 1**. Quan hệ vẫn `relation_dialog` (luật `:130` xét trước) ⇒ **KHÔNG chặn đối thoại**, nhưng **lệch quy ước JX1**: `lib_phonglangdo.lua:150-156` sinh đúng NPC 240 này với `nCurCamp = 6`. Ảnh hưởng: màu tên / phe hiển thị phía client. | **NHẸ** |
| `script\global\autoexec_npc_hd3.lua:22` | `AddNpcEx(it[1], 1, random(0,4), SId, it[3]*32, it[4]*32, **0**, it[6])` | NPC 769 `Kind=3 / Camp=6` → ép về **phe 0**. Cùng lý do trên ⇒ không chặn đối thoại. | **NHẸ** |
| `script\missions\challengeoftime\npc.lua:431-439` (chép nguyên từ Linux, **không** thuộc 14 chỗ) | `AddNpcEx(id, level, series, SubWorld, px*32, py*32, **1**, name, isboss)` — chú thích gốc ghi rõ `1 -- 不重生` | 🔴 quái **Vượt Ải** bị ép **phe 1** ⇒ ally với người chơi chính phái + chưa vào phái | **CHẶN (một phần)** |
| `script\battles\battlehead.lua:77` (`ExhibitBoss`) | `AddNpcEx(..., 1, BOSSINFO[..][2], 1)` | boss trưng bày phe 1 | NHẸ (chưa nối) |
| `script\global\thanh\npc\add_npc.lua:225` (chép mới từ Linux) | `AddNpcEx(nID, nLevel, nSeries, nSubWorld, nX, nY, nRevive, nName, nIsBoss)` với `nRevive = TabIndex[i][6] = 0` | 160 boss sát thủ bản Việt cũng ép **phe 0**. Hiện **chưa ai gọi** `add_npc_thanh()` (grep toàn cây chỉ thấy định nghĩa) | NHẸ (mã chết) — nhưng là mìn |

> `add_npc.lua:225` cũng là **bằng chứng thứ tư** cho chữ ký `AddNpcEx` bản Linux:
> tệp này do chính bản Linux viết và đặt tên biến `nSeries` ở vị trí 3, `nRevive` ở vị trí 7.

---

## 7. Có chỗ nào BỊ SỬA NHẦM không?

**Không.** Đã diff từng tệp (giải mã UTF-8 giữ số dòng, so với `src_utf8`):

* `boss.lua` — chỉ 6 dòng `AddNpc→AddNpcEx` + 1 dòng bỏ `Include("\script\global\路人_礼官.lua")`.
* `mission.lua` — 1 dòng `AddNpc→AddNpcEx` + 1 dòng đổi id vật phẩm `6,1,2743 → 6,1,3361`
  (thuộc hướng khác) + thêm 1 dòng trống cuối.
* `fld_smalltimer.lua` — đúng 4 dòng, thêm newline cuối tệp.
* `killbosshead.lua` (3421 dòng) — **đúng 3 dòng** thay đổi, không có gì khác.

Biểu thức `RE_ADDNPC = r"\bAddNpc\(([^,]+),([^,]+),"` (`b2_patch.py:57`) **không** khớp `AddNpcEx(`
(sau `AddNpc` là chữ `E`), không khớp `AddNpc_flyboss=` (không có dấu ngoặc), không khớp
`SetAddNpc(` (`\b` chặn). Không có lời gọi `AddNpcEx` sẵn có nào bị chèn thêm `random(0,4)`.

**Hai ghi chú sai trong `b2_patch.py` (mức NHẸ, chỉ là tài liệu):**
* `:56` viết *"Chỉ khớp lời gọi có >= 6 tham số"* — biểu thức **không đếm tham số** gì cả, chỉ cần
  có 2 dấu phẩy. May là 4 tệp được vá không có lời gọi 5 tham số; nếu chạy lại trên
  `battlehead.lua:386/402` (5 tham số) thì kết quả tình cờ vẫn đúng, nhưng đừng dựa vào.
* `:23` mô tả `AddNpcEx` của JX1 là `(..., noRevive, name, isboss)` — **đúng với bản Linux nhưng
  sai với cái mà `LuaAddNpcEx` của JX1 thật sự làm** (ô đó là phe). Chính chỗ hiểu nhầm này đẻ ra CHẶN-1.

---

## 8. Ngũ hành `random(0,4)` — KIỂM XONG, ĐÚNG

`D:\GAMEDEVNEW\Sources\Library\LuaLib\src\baselib\lmathlib.c:168-192` (Lua 4.0):

```c
double r = (double)(rand()%RAND_MAX) / (double)RAND_MAX;   // r thuoc [0,1)
case 2: lua_pushnumber(L, (int)(r*(u-l+1))+l);             // random(0,4) -> (int)(r*5)+0
```

⇒ trả **số nguyên chính xác 0…4** (đã ép `(int)` bên trong C, không phải số thực cần làm tròn).
`LuaAddNpcEx:6968` `(int)lua_tonumber(L,3)` không làm mất gì. **Đúng dải 0..4, đủ 5 hệ.**

* Đăng ký toàn cục: `lua_mathlibopen` → `luaL_openl(L, mathlib)` (`lmathlib.c:231-232`), gọi ở
  `Sources\Engine\Src\KLuaScript.cpp:517`. Không có hàm `random` nào khác đè lên trong
  `ScriptFuns.cpp` (chỉ có `RandomNew` `:14500` và `Random` của WLLS `:14986`).
* Có gieo hạt: `KCore.cpp:191` và `:218` `srand((unsigned)time(NULL))` ⇒ không bị lặp dãy mỗi lần khởi động.
* Đối chiếu Linux: `0x0811BC8C` cho `rand()%5` — **cùng dải 0..4**. ✔

---

## 9. Đơn vị toạ độ `mission.lua:22` (câu hỏi 7) — ĐÚNG, KHÔNG PHẢI CHẶN

* `mission.lua:21` `posx, posy = fld_getadata(npcthiefpos)`.
* `fld_head.lua:18` `npcthiefpos = "\settings\maps\中原北区\渡船\渡船刷怪点.txt"`.
* `fld_head.lua:135-137` dùng **cùng hàm đó** rồi `posx = floor(posx/32)` trước khi gọi `NewWorld`
  ⇒ **`fld_getadata` trả PIXEL**.
* Bảng thật (`00_ghichu_dieuphoi.md` mục 8): `XPOS 50400 / YPOS 102240` ⇒ ô (1575, 3195), nằm
  quanh `boatMAP_POS = {1646, 3233}` (`fld_head.lua:13`) ⇒ đúng là pixel.
* JX1 `AddNpcEx:6970-6974` → `AddNpcSet2` → `SubWorld[..].Mps2Map(nMpsX, nMpsY, ...)`
  (`KNpcSet.cpp:453`) ⇒ **mong đợi MPS = pixel**.

⇒ **Khớp đơn vị. Quái KHÔNG sinh ngoài bản đồ.** Tương tự cho `fld_smalltimer.lua:37/44/51/60`
(cùng `fld_getadata`), `boss.lua` (`1636*32`), `killbosshead.lua:186-187` (`Tab3[i][4]*32`),
`hd3_driver.lua:60` (`t[1]*32`), `autoexec_npc_hd3.lua:22` (`it[3]*32`).

> Chỉ có **một** chỗ không nhân 32 là `killbosshead.lua:3410` (`add_messengernpc` dùng
> `Tab4[i][4]` thô) — nhưng bảng nguồn `AddNpc_allbugbear` vốn ghi sẵn pixel
> (`{823,70,387,45664,81888,...}`) nên vẫn nhất quán; hơn nữa bảng đó **0 dòng active**.

---

## 9b. 🔴🔴 BONUS — CHẶN-2: thiếu 5 dòng trong `settings\TimerTask.txt` ⇒ **Phong Lăng Độ và Vượt Ải KHÔNG BAO GIỜ CHẠY**

Tìm ra khi đang kiểm "13 chỗ vá có thật sự chạy không". **Ngoài phạm vi hướng A3 nhưng nặng hơn A3-1**, ghi lại để phiên chính không bỏ sót.

Cơ chế: `StartMissionTimer(missionId, timerId, time)` (`ScriptFuns.cpp:11426-11454`) **chỉ lưu số
`timerId`**. Khi hết giờ, `KTimerTaskFun::Activate` (`KTaskFuns.cpp:127`) gọi
`g_TimerTask.GetTimerTaskScript(szTimerScript, m_dwTimerTaskId, MAX_PATH)` →
`KTaskFuns.cpp:177-186` chỉ làm **một việc**: `m_TimerTaskTab.GetString("<timerId>", "SCRIPT", "", ...)`
trên bảng `settings\TimerTask.txt`. **Không có đường dự phòng** — không tìm thấy thì trả chuỗi rỗng
và `g_MissionTimerCallBackFun` (`KMission.cpp:338`) chạy `ExecuteScript("", ...)` = không làm gì.

| timerId | Script bản Linux (`D:\ServerLinux\server1\settings\TimerTask.txt`) | Ai dùng | Có trong `TimerTask.txt` của cây sống? |
|---|---|---|---|
| **28** | `\script\missions\fengling_ferry\fld_landingtimer.lua` | `mission.lua:16` | ❌ **KHÔNG** |
| **29** | `\script\missions\fengling_ferry\fld_smalltimer.lua` | `mission.lua:15` | ❌ **KHÔNG** |
| **41** | `\script\missions\challengeoftime\timer_match.lua` | `include.lua:7` `TIMER_MATCH`, `mission_match.lua:12` | ❌ **KHÔNG** |
| **42** | `\script\missions\challengeoftime\timer_board.lua` | `include.lua:8` `TIMER_BOARD`, `include.lua:209` | ❌ **KHÔNG** |
| **43** | `\script\missions\challengeoftime\timer_close.lua` | `include.lua:9` `TIMER_CLOSE`, `include.lua:220` | ❌ **KHÔNG** |

Bảng của cây sống (`E:\...\bin\server\settings\TimerTask.txt`, 37 dòng dữ liệu) chỉ có id
`1-10, 12-18, 20, 21, 50-55, 61, 62, 65-70, 75-77`. **Tệp này KHÔNG có trong `b1_manifest.txt`**,
và `b1_copy.py:38-41` có luật "không bao giờ đè tệp đã có ở đích" ⇒ **bảng chưa từng được hợp nhất**.

Hệ quả dây chuyền: `fld_smalltimer.lua:9` mới là chỗ gọi `RunMission(MISSIONID)` ⇒ **30 con thuỷ tặc
của `mission.lua:22` cũng không sinh**, 4 boss `fld_smalltimer.lua:37/44/51/60` không sinh,
`Landing()` không chạy. Nghĩa là chỗ #10-14 của bảng mục 3 hiện **chưa chạy được để lộ CHẶN-1** —
nhưng vá CHẶN-2 xong thì CHẶN-1 hiện ra ngay.

**Vá đề nghị**: thêm đúng 5 dòng `28/29/41/42/43` vào `settings\TimerTask.txt` của cây sống
(id không đụng dải đang dùng — kiểm lại 28/29/41/42/43 đều trống). **Không** chép đè cả tệp Linux
(116 dòng) vì sẽ nuốt dải 50-77 mà các đợt port trước đã thêm.

---

## 10. Danh mục phát hiện, xếp theo mức

| Mã | Mức | Vị trí | Tóm tắt |
|---|---|---|---|
| **A3-1** | **CHẶN** | `Sources\Core\Src\ScriptFuns.cpp:6982-6995` (gốc) → hiện thực ở `killbosshead.lua:189`, `fld_smalltimer.lua:37/44/51/60`, `mission.lua:22`, `challengeoftime\npc.lua:431` | Tham số 7 bị ánh xạ sang `SetCurrentCamp` ⇒ **toàn bộ quái/boss của 3 hoạt động thành `relation_ally`, không đánh được** |
| **A3-1b** | **CHẶN (hồi quy)** | `killbosshead.lua:189` ← `startgame.lua:117` → `tinsu_addnpc.lua:45` → `addtollgatenpc.lua:17` | 9 "Bảo Khố Thủ Hộ Giả" (849) của ải Thiên Bảo Khố **đợt Tín Sứ 21/08 đang chạy** cũng bị ép phe 0 ⇒ **hỏng tính năng đã bàn giao** |
| **A3-2** | NẶNG | `boss.lua:28/35/44/51/60/67` | Cùng lỗi A3-1 (6 boss bến thuyền). Hiện là mã chết vì `boss.lua:19-24` chú thích hết — nếu phiên chính bật lại thì thành CHẶN |
| **A3-3** | NẶNG | `script\battles\battlehead.lua:384/386/400/402/437/482` | **6 chỗ `AddNpc(` bỏ sót** trong tệp vừa chép; tham số 6 (`l_removedeath`) rơi vào ô ngũ hành |
| **A3-4** | NHẸ | `script\activitysys\config\32\talkdailytask.lua:27` | Chỗ `AddNpc(` bỏ sót **có chạy thật** (gọi từ `fld_smalltimer.lua:13`); NPC đối thoại bị ép hệ Kim |
| **A3-5** | NHẸ | `hd3_driver.lua:60`, `autoexec_npc_hd3.lua:22` | Ép phe 0/1 cho NPC `kind_dialoger`; không chặn đối thoại nhưng lệch quy ước (`lib_phonglangdo.lua:150` dùng phe 6) |
| **A3-6** | NHẸ | `script\global\thanh\npc\add_npc.lua:225` | Tệp mới chép, `nRevive = 0` ⇒ cùng lỗi phe; hiện **không ai gọi** `add_npc_thanh()` — mìn chờ |
| **A3-7** | NHẸ | `thicong\b2_patch.py:23` và `:56` | Hai chú thích sai: mô tả tham số 7 là `noRevive` (thực tế JX1 dùng làm phe) và "chỉ khớp ≥6 tham số" (biểu thức không đếm) |
| **A3-8** | CHƯA XÁC MINH | `ScriptFuns.cpp:7005` vs Linux `0x0811BEF2` | JX1 bỏ tham số 9; phần nạp thuộc tính đã được `KNpc::Load` làm, nhưng trường `KNpc+0x181C = 2/3` **không có tương ứng** — chưa rõ nó điều khiển gì |
| **A3-9** | **CHẶN** (ngoài hướng, xem mục 9b) | `E:\...\bin\server\settings\TimerTask.txt` (thiếu id **28, 29, 41, 42, 43**) | Không có bảng ⇒ `StartMissionTimer` của `mission.lua:15-16` và `challengeoftime\include.lua:7-9` **không giải được ra script** ⇒ **Phong Lăng Độ + Vượt Ải không chạy dòng nào**. Tệp không nằm trong `b1_manifest.txt` |

---

## 11. Cách tái lập nhanh (cho phiên chính)

```
# doc than ham JX1
sed -n '6937,7009p'  D:\GAMEDEVNEW\Sources\Core\Src\ScriptFuns.cpp
sed -n '128,165p'    D:\GAMEDEVNEW\Sources\Core\Src\KNpcSet.cpp      # GenOneRelation
sed -n '484,495p'    D:\GAMEDEVNEW\Sources\Core\Src\GameDataDef.h    # enum NPCCAMP

# dich nguoc ban Linux
python D:\GAMEDEVNEW\ReverseTools\re_disasm.py D:/ServerLinux/server1/jx_linux_y 0x0811BB10 150
python D:\GAMEDEVNEW\ReverseTools\re_disasm.py D:/ServerLinux/server1/jx_linux_y 0x0811BD40 100

# kiem Kind/Camp cua npc (id N nam o dong N+2 cua NpcS.txt, cot Kind=1, Camp=2)
python -c "p=r'E:\SourceTuanLe\...\settings\NpcS.txt'; d=open(p,'rb').read().decode('latin-1').split('\n'); print(d[725+1].split('\t')[:4])"

# quet lai AddNpc( con sot
rg -n "\bAddNpc\(" "E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
```

---

## ĐỐI CHẤT (tác tử độc lập)

> Vòng soát thứ hai, **không** do người viết A3 thực hiện. Nguyên tắc: mặc định coi mọi phát hiện
> của A3 là SAI cho đến khi tệp gốc / mã C / binary chứng minh ngược lại. Đã kiểm chứng lại **20
> mục**, mở lại từ đầu 4 đường: (1) đọc thân hàm `LuaAddNpcEx`, (2) bảng quan hệ + enum phe,
> (3) đọc lại bảng dữ liệu và `NpcS.txt` của cây sống, (4) **tự dịch ngược lại** `jx_linux_y`.
> Không sửa một dòng mã nguồn nào; chỉ ghi vào tệp báo cáo này.

### Bảng đối chất

| # | Phát hiện của A3 | Bằng chứng gốc (tự kiểm lại) | KẾT LUẬN | Sửa lại thành |
|---|---|---|---|---|
| 1 | **CHẶN-1**: `LuaAddNpcEx` tham số 7 → `SetCurrentCamp` | `ScriptFuns.cpp:6982-6995` đọc nguyên văn: `int nCamp = (int)Lua_ValueToNumber(L, 7); if (nCamp >= 0 && nCamp < camp_num) Npc[nNpcIdx].SetCurrentCamp(nCamp);`. Chú thích ngay trên đó (`:6929-6931`) tự thú "LECH CO CHU DICH … tham so 7 anh xa sang SetCurrentCamp" | **ĐÚNG** | giữ nguyên |
| 2 | Phe 0/1 ⇒ `relation_ally` | `KNpcSet.cpp:139-141` `(camp_begin & camp_animal)→enemy`; `:143-144` `Camp==camp_begin→ALLY`; `:155-156` `Camp1==Camp2→ALLY`; `:158` `kind_normal→enemy`. `GameDataDef.h:486-495` `camp_begin=0 … camp_num=9`. `GetRelation` (`KNpcSet.cpp:1440-1446`) tra bảng bằng **`m_CurrentCamp`** — đúng ô `SetCurrentCamp` ghi (`KNpc.cpp:451-453`) | **ĐÚNG** | giữ nguyên |
| 3 | (A3 không nêu) `SetCurrentCamp` có ĐÈ được phe mẫu không? | `KNpc::Load` đặt `m_CurrentCamp = m_Camp` ở `KNpc.cpp:5357`, mà `Load` chạy **bên trong** `AddNpcSet2` — tức TRƯỚC `SetCurrentCamp` ở `:6986`. `RestoreCurrentCamp` (`:524`) / `RestoreNpcBaseInfo` (`:9659`) chỉ chạy khi hồi sinh | **ĐÚNG (thiếu mắt xích)** | thêm 1 dòng: thứ tự `Load → SetCurrentCamp` là lý do bản vá thắng phe mẫu |
| 4 | Chuỗi chặn đánh: `KPlayer.cpp:9426`, `KMissle.cpp:682/716/1849` | `KPlayer.cpp:9416-9430` là vòng **tìm mục tiêu của WAuto** (`m_mAutoExcludeNpcID`, `AUTOLOG_EVERY("[FT-SKIP-REL]")`) — **không phải** đường đánh tay. Đường thật: `KSkills.cpp:2081-2089` dựng `m_eRelation` = bitmask theo cờ mục tiêu của chiêu → `KSkills.cpp:1912` gán cho đạn → `KMissle.cpp:682/716/1849` `FindNpc(..., m_eRelation)` | **ĐÚNG kết luận, SAI chứng cứ** | đổi dẫn chứng chính thành `KSkills.cpp:2081-2089` + `:1912`; giữ `KPlayer.cpp:9426` nhưng ghi rõ "đường WAuto" |
| 5 | `Tab3[i][6] = 0` cho toàn bộ 160 dòng | Đếm lại dòng active: `killbosshead.lua:6-165` = **160 dòng**, mọi dòng cột 6 = `0`, cột 7 = tên, cột 8 = `1`. Mẫu `{761,25,73,1545,2947,0,"Trác Lãnh Cầm",1,"…kill_level.lua",1}` | **ĐÚNG** | giữ nguyên |
| 6 | CHẶN-1b: 9 Bảo rương 844 + 9 Bảo Khố Thủ Hộ Giả 849 của Tín Sứ dùng chung `killbosshead.lua:189` | `startgame.lua:117 tinsu_addnpc()` → `tinsu_addnpc.lua:45 add_alltollgatenpc()` → `addtollgatenpc.lua:16-17` (KHÔNG bị chú thích) → `killbosshead.lua:183`. Bảng: `:3380-3388` 9 dòng `{844,…,0,"Bảo rương N",1,…}`; `:2922-2930` 9 dòng `{849,…,0,"Bảo Khố Thủ Hộ Giả N",1,…}`. `NpcS.txt`: 849 = `Kind 0 / Camp 5`, 844 = `Kind 3 / Camp 6` | **ĐÚNG một nửa** — 849 hỏng thật; 844 là `kind_dialoger` nên trúng `KNpcSet.cpp:130` → `relation_dialog`, **không** hỏng | tách câu: "9 Hộ Thủ Giả 849 hỏng; 9 Bảo rương 844 KHÔNG hỏng (dialoger)" |
| 7 | Kind/Camp lấy từ `NpcS.txt` | Kiểm lại theo tiêu đề cột (`Name Kind Camp Series …`): 724 `0/5` "Thủy tặc", 725 `0/5` "Boss Thủy tặc đầu lĩnh", 1692 `0/5` "Thủy Tặc Đại Đầu Lĩnh.", 513 `0/5`, 511 `0/5`, 240 `3/6` "Thuyền phu Giáp", 769 `3/6` "Nhiếp Thị Trấn", 1632 `0/5`, 1237/1238 `0/5`, 562 `0/5` — tên khớp 100% với script | **ĐÚNG** | giữ nguyên |
| 8 | Người chơi Phong Lăng Độ bị ép phe 1 | `fld_head.lua:127` `SetCurCamp(1);`; `ScriptFuns.cpp:14690` `{"SetCurCamp", LuaSetPlayerCurrentCamp}` → `:8188 SetCurrentCamp(nValue)` | **ĐÚNG** | giữ nguyên |
| 9 | **CHẶN-2**: `TimerTask.txt` thiếu 28/29/41/42/43 | Đọc lại tệp sống: 36 id = `1-10,12-18,20,21,50-55,61,62,65-70,75-77` — **thiếu đủ 5 id**. `KTaskFuns.cpp:177-186` chỉ tra `m_TimerTaskTab` (`GameDataDef.h:65 = \settings\TimerTask.txt`), không có nhánh dự phòng; `KMission.cpp:347` `if (szScriptFile && szScriptFile[0])` ⇒ chuỗi rỗng là không làm gì. `mission.lua:15-16` dùng 29/28; `challengeoftime\include.lua:7-9` dùng 41/42/43 | **ĐÚNG, mức CHẶN đúng** | giữ nguyên nội dung |
| 9b | Câu chữ "**không chạy nổi một dòng nào**" | `settings\task\missions.txt` **ĐÃ được vá** (còn bản `missions.txt.truoc_3hd_2508`): `15 → fengling_ferry\mission.lua`, `22 → challengeoftime\mission_match.lua`. Nên `OpenMission` vẫn nạp và `InitMission()` **vẫn chạy** (xoá NPC/trap, đặt biến); chỉ **đồng hồ** là chết. Cũng đã kiểm `g_MissionTimerCallBackFun` (`KMission.cpp:356-358`) tự dò `OnMissionTimer`/`OnTimer` nên **không** có blocker thứ hai ở đây | **THỔI PHỒNG (câu chữ)** | "InitMission vẫn chạy; **RunMission / Landing / boss / mọi mốc giờ** không bao giờ chạy ⇒ hoạt động rỗng" |
| 10 | Ngũ hành `random(0,4)` đúng dải | `lmathlib.c:186-190` case 2: `(int)(r*(u-l+1))+l` với `r ∈ [0,1)` ⇒ **0..4** nguyên | **ĐÚNG** | giữ nguyên |
| 11 | Toạ độ là PIXEL, không phải chặn | `fld_head.lua:135-137` gọi cùng `fld_getadata` rồi `floor(posx/32)`; tệp dữ liệu thật `settings\maps\中原北区\渡船\渡船刷怪点.txt` dòng đầu `50400  102240` (= ô 1575/3195). `AddNpcSet2 → Mps2Map` | **ĐÚNG** | giữ nguyên |
| 12 | Chỗ #2/#3 (`:3399`, `:3410`) là mã chết | `AddNpc_allbugbear` (`:365`→`:2606`): đếm lại = **0 dòng active** ⇒ `add_messengernpc` (được gọi thật ở `addtollgatenpc.lua:19`) chạy 0 vòng. `add_bossnpc` bị chú thích ở `:18` | **ĐÚNG** | giữ nguyên |
| 13 | `boss.lua` là mã chết | `boss.lua:18-25` `function thuytacdaulinh()` có **6 dòng gọi đều bị `--`** | **ĐÚNG** | giữ nguyên |
| 14 | `add_npc.lua:225` chưa ai gọi | Lời gọi nằm trong `add_boss(TabIndex)` (`:211`), chỉ được gọi từ `add_npc_thanh()` (`:203`); grep toàn cây: `add_npc_thanh` **chỉ xuất hiện 1 lần = định nghĩa** | **ĐÚNG** (A3 ghi nhầm tên hàm chứa lời gọi) | ghi rõ "trong `add_boss()`, gọi từ `add_npc_thanh()` — hàm không ai gọi" |
| 15 | Chú thích sai trong `b2_patch.py` `:23` và `:56` | `RE_ADDNPC = r"\bAddNpc\(([^,]+),([^,]+),"` (`b2_patch.py:47`) **không đếm tham số** — đúng; `\b` cũng chặn `AddNpcEx(` và `AddNpc_flyboss=`. Nhưng câu "AddNpcEx cua JX1 nhan (id, lv, series, map, x, y, noRevive, name, isboss)" nằm ở **dòng 9**, không phải `:23` | **ĐÚNG nội dung, SAI số dòng** | đổi `:23` → `:9`, `:56` → `:45-47` |
| 16 | Chữ ký `AddNpc` bản Linux | **Tự dịch ngược lại** `0x0811BD40..0x0811BE25`: `0x0811BD70` nạp đối số **6** → `0x0811BDB2 setne byte ptr [edx+0x1824]` (cờ BYTE, chỉ khác-0/bằng-0); `0x0811BDC2` nạp đối số **7** qua `call 0x8233850` (`lua_tostring`) + kiểm `[eax] != 0` ⇒ **tên**; `0x0811BDF0` nạp đối số **8** rồi `cmp eax,1 / cmp eax,2` ⇒ **cờ boss**; `0x0811BD40` đối số **9** | **ĐÚNG** — khớp 100% chú thích gốc `basemission\lib.lua:34-42` (`不重生 / 名字 / 是否BOSS`) | giữ nguyên |
| 17 | "3 điểm gọi `AddNpcEx` cũ của JX1 … đều truyền 1" (mục 4b) | Grep lại `AddNpcEx(` **thô** (không phải `AddNpcEx1..5` của `lib_map.lua`) trên cây sống: ngoài `bosscharm.lua:99`, `seasonnpc_item.lua:57`, `spider_web.lua:42` còn **`missions\basemission\lib.lua:33`**, **`missions\bairenleitai\hundred_arena.lua:635`**, **`missions\tongcastle\tongcastle.lua:937`** — cả ba đều truyền `tbNpc.bNoRevive or 1` ở ô 7 | **THIẾU (sót 3 điểm gọi đang sống)** | xem "Bỏ sót" mục BS-B |
| 18 | "**chính đợt vá 25/08 tạo ra lỗi này**" (mục 4) | Ba điểm ở #17 thuộc đợt port **23/08** (Bách Nhân / Thành Bảo / `basemission`), đã dính đúng CHẶN-1 từ trước 25/08. Riêng `bosscharm.lua:99` (Boss Hoàng Kim — tính năng JX1 **đang chạy thật**) dính từ **21/08** | **SAI một phần / HẠ THẤP** | "đợt 25/08 **mở rộng** một lỗi đã có từ 21-23/08 sang cả 3 hoạt động mới" |
| 19 | Mức của `bosscharm.lua:99` (chỉ nhắc thoáng trong 4b) | Mẫu 562-583 `Camp=5`; ép về **1** ⇒ người chơi **chính phái (camp 1)** trúng `KNpcSet.cpp:155` → ally; người **chưa vào phái (camp 0)** trúng `:143` → ally ⇒ **không đánh được Boss Hoàng Kim**; chỉ tà / trung lập / phản môn đánh được | **HẠ THẤP** | nâng thành mục riêng **A3-1c — CHẶN (đang chạy thật, có từ 21/08)** |
| 20 | Đề xuất vá 4b (bỏ nhánh `SetCurrentCamp`) | Kiểm "tác dụng phụ" rộng hơn A3: `basemission\lib.lua:58-60` và `tongcastle.lua:962-964` **đã tự đặt phe** bằng `SetNpcCurCamp` khi cần ⇒ không mất gì. `hundred_arena.lua:490/518` ép **người chơi** về `SetCurCamp(4)` `camp_free` ⇒ quái trả về `camp_animal(5)` vẫn ra `relation_enemy` qua `KNpcSet.cpp:158`. Ba mẫu JX1 cũ (562-583 / 1237-1238 / 1632) đều `Camp=5` ⇒ trả về `camp_animal` là **đúng** với mọi phe người chơi | **ĐÚNG hướng, và AN TOÀN HƠN A3 tưởng** | giữ đề xuất; bổ sung 3 điểm gọi ở #17 vào bảng "ảnh hưởng khi vá" |

**Thống kê đối chất**: 20 mục kiểm — **14 ĐÚNG nguyên vẹn**; **1 SAI chứng cứ (giữ kết luận)** (#4);
**1 ĐÚNG một nửa** (#6); **1 SAI một phần / HẠ THẤP** (#18); **1 THỔI PHỒNG câu chữ** (#9b);
**1 HẠ THẤP mức** (#19); **2 sai số dòng trích dẫn** (#14, #15); **1 THIẾU danh sách** (#17).
**Không phát hiện nào của A3 bị bác bỏ hoàn toàn** — CHẶN-1, CHẶN-1b và CHẶN-2 đều là lỗi thật, đúng mức.

---

### Bỏ sót của chính vòng soát

Ba vùng nằm **đúng trong hướng "addnpc"** mà A3 không mở ra.

#### BS-A (**NẶNG**) — NPC 769 "Nhiếp Thị Trấn" bị **NHÂN ĐÔI ở 7 thành**, 5/7 **trùng đúng ô**

`hd3_driver.lua:38` nạp `autoexec_npc_hd3.lua`, `:42` gọi `add_dialognpc_hd3(adddialognpc_hd3)` ⇒
`autoexec_npc_hd3.lua:22` sinh **7 NPC 769** ở 7 thành. Nhưng cây sống **đã có sẵn 7 NPC 769**,
sinh trong `addfullnpc()` (`startgame.lua:124`) qua `addnpc<thành>()`:

| Thành | NPC 769 CŨ (`\script\startgame\thanh\…`) | NPC 769 MỚI (`autoexec_npc_hd3.lua:8-14`) | Lệch |
|---|---|---|---|
| Thành Đô (11) | `thanhdo.lua:27` `3210*32, 4974*32` + `SetNpcValue(,7)` | `{769,11,3210,4974}` | **0 ô — TRÙNG KHÍT** |
| Phượng Tường (1) | `phuongtuong.lua:22` `1506*32, 3198*32` + `SetNpcValue(,1)` | `{769,1,1506,3198}` | **0 ô — TRÙNG KHÍT** |
| Biện Kinh (37) | `bienkinh.lua:24` `1647*32, 3050*32` + `SetNpcValue(,4)` | `{769,37,1647,3050}` | **0 ô — TRÙNG KHÍT** |
| Lâm An (176) | `laman.lua:30` `1372*32, 3010*32` + `SetNpcValue(,5)` | `{769,176,1372,3010}` | **0 ô — TRÙNG KHÍT** |
| Dương Châu (80) | `duongchau.lua:31` `1700*32, 2963*32` + `SetNpcValue(,3)` | `{769,80,1700,2963}` | **0 ô — TRÙNG KHÍT** |
| Tương Dương (78) | `tuongduong.lua:31` `1511*32, 3204*32` + `SetNpcValue(,2)` | `{769,78,1512,3206}` | 1-2 ô |
| Đại Lý (162) | `daily.lua:18` `1573*32, 3228*32` + `SetNpcValue(,6)` | `{769,162,1573,3227}` | 1 ô |

Hai NPC **cùng id 769, cùng tên "Nhiếp Thị Trấn"**, khác script và khác dữ liệu:

* bản CŨ chạy `OTHER_SGSTHU` = `\script\tinhnang\vuot_ai\sugiasatthu.lua` (`lib_map.lua:8`) và
  **có `SetNpcValue(nNpcIdx, 1..7)`** = chỉ số thành;
* bản MỚI chạy `\script\task\tollgate\killer\nieshichen.lua` và **không gọi `SetNpcValue`** ⇒
  `GetNpcValue` trả 0.

Hệ quả: (a) người chơi bấm trúng NPC nào là ngẫu nhiên theo thứ tự vẽ; (b) memory dự án đã ghi
đúng bẫy này ở đợt Liên Đấu — `DT_FindNpcName` lấy **khớp đầu tiên** nên auto/bot sẽ bấm nhầm;
(c) `sukien_vuotai` cũ đã bị tắt (`timerserver.lua:82`) nên nhánh cũ thành ngõ cụt, nhưng NPC vẫn còn.

**Sửa đề nghị** (chọn MỘT, không làm cả hai): hoặc bỏ 7 dòng `AddNpcEx1({769},…)` trong
`\script\startgame\thanh\*.lua` (7 tệp), hoặc bỏ `add_dialognpc_hd3(...)` ở `hd3_driver.lua:42`.
**Không** để cả hai cùng sinh.

#### BS-B (**NẶNG**) — 3 điểm gọi `AddNpcEx` **thô, đang sống** nằm ngoài mọi bảng của A3

A3 chỉ liệt kê 3 điểm gọi cũ (`bosscharm` / `seasonnpc_item` / `spider_web`). Grep lại toàn cây sống
cho `AddNpcEx(` **không phải** `AddNpcEx1..5`:

| Tệp:dòng | Ô thứ 7 | Ai gọi | Hệ quả |
|---|---|---|---|
| `script\missions\basemission\lib.lua:33` | `tbNpc.bNoRevive or 1` | `hundred_arena.lua:79/90/101/347`, `tongcastle.lua:730/781`, `guideperson.lua:143/152`, `game.lua:83`, `challengeoftime\chuangguang30.lua:218`, `challengeoftime\npc.lua:500`, `activitysys\config\41\extend.lua:21`, `bairen_boot.lua:17` | mọi NPC không khai `nCurCamp` bị ép **phe 1**; có `SetNpcCurCamp` bù ở `:58-60` nên chỉ hỏng khi bảng không khai phe |
| `script\missions\bairenleitai\hundred_arena.lua:635` | `tb_npc.bNoRevive or 1` | Bách Nhân (đấu sĩ trên đài) | **may mà không hỏng**: `:490/:518` ép người chơi về `SetCurCamp(4)` `camp_free` ⇒ vẫn `relation_enemy` qua `KNpcSet.cpp:158` |
| `script\missions\tongcastle\tongcastle.lua:937` | `tbNpc.bNoRevive or 1` | Thành Bảo | NPC nào không khai `nCurCamp` (`:962`) bị ép phe 1 — **CHƯA XÁC MINH** vì Thành Bảo còn bị chặn bởi thiếu pak map 984 |

⇒ Hai hệ quả A3 bỏ mất: (1) CHẶN-1 **không phải do đợt 25/08 sinh ra** — nó có từ 21-23/08 và đang
sống trên `bosscharm.lua:99` (Boss Hoàng Kim); (2) bảng "ảnh hưởng khi vá C++" của mục 4b **thiếu 3
dòng** — may là cả ba đều an toàn khi bỏ `SetCurrentCamp` (đã kiểm ở mục #20 bảng trên).

#### BS-C (**NẶNG**) — 3/6 "thuyền phu" mới đặt **đúng vào ĐIỂM ĐỔ BỘ bờ Bắc**, lại gắn script LÊN THUYỀN

`hd3_driver.lua:28-31` chép nguyên 6 toạ độ của `lib_phonglangdo.lua:79-86` `TAB_NPCCHUCNANG`, rồi
`:60-63` gắn **cùng một** script `hd3_thuyenphu.lua` (→ `fld_wanttakeboat`) cho **cả 6**, BOATID 1,2,3,1,2,3.

Nhưng bản JX1 phân đôi bảng đó: `lib_phonglangdo.lua:150-152` dùng phần tử **1-3** với
`TAB_NPCCHUCNANG_SCRIPT[1] = thuyenphu.lua` (**bờ Nam, lên thuyền**), còn `:154-156` dùng phần tử
**4-6** với `TAB_NPCCHUCNANG_SCRIPT[2] = thuyenphubac.lua`, tên NPC "Thuyền phu **Bắc** ất/bính/giáp"
— chức năng là `vebonam` (về bờ Nam, `thuyenphubac.lua:34-40`) và `TraNvPLD` (trả nhiệm vụ).

Bản Linux xác nhận đúng phân đôi này: `src_utf8\phonglangdo\missions\fengling_ferry\fld_head.lua:15`
`northMAP_POS = {{1158,2964},{1343,2868},{1482,2796}}` — trùng (sai số 11-19 ô) với
`TAB_NPCCHUCNANG[6] = {1173,2981}`, `[4] = {1324,2886}`, `[5] = {1493,2809}`.

Hệ quả:
1. Ba NPC ở **điểm đổ bộ bờ Bắc** mời người chơi… lên thuyền đi bờ Bắc (trong khi đang đứng bờ Bắc).
2. `startgame.lua:102` đã tắt `addnpcphonglangdo()` ⇒ **mất hẳn** NPC về bờ Nam và NPC trả nhiệm vụ
   "Tiêu diệt thuỷ tặc" của bản JX1.
3. BOATID gán cho 3 điểm bờ Bắc là gán bừa: theo `northMAP_POS` thì `[4]` ứng với **thuyền 2**
   (map 338, 中游) chứ không phải 1.

**Sửa đề nghị**: cắt `HD3_PLD_BOAT` còn 3 phần tử đầu (bờ Nam), giữ BOATID 1/2/3; nếu muốn giữ NPC
bờ Bắc thì phải gắn script khác, **không** dùng `hd3_thuyenphu.lua`.

> Đã kiểm và **KHÔNG** phải lỗi (ghi lại để khỏi soát lại): `Include` lười trong thân hàm
> (`hd3_driver.lua:38-39`) chạy được — `LuaIncludeFile` (`ScriptFuns.cpp:1969-2026`) chỉ là
> `lua_dofile` trên state hiện tại; wiring `timerserver.lua:32` + `:74` `HD3_Tick` có đủ;
> lịch cũ `sukien_phonglangdo`/`sukien_vuotai` đã tắt (`timerserver.lua:82-83`); tiền tố mới
> `\script\vng_feature\` thêm vào `g_IsJx2Script` (`KSortScript.cpp:129`) chỉ phủ 4 tệp, trong đó
> duy nhất `challengeoftime\npcNhiepThiTran.lua` dùng nhóm hàm lệch nghĩa ⇒ không gây hồi quy.

---

### Danh sách CHỐT sau đối chất (đã lọc trùng, xếp theo mức)

| Mã | Mức | Vị trí | Cách sửa |
|---|---|---|---|
| **A3-1** | **CHẶN** | `Sources\Core\Src\ScriptFuns.cpp:6982-6995` | Bỏ nhánh `SetCurrentCamp(nCamp)`, giữ `m_bNoRevive`. Đã kiểm: an toàn cho **toàn bộ 9 điểm gọi thô** trên cây sống |
| **A3-1b** | **CHẶN (hồi quy)** | `killbosshead.lua:189` ← `startgame.lua:117` | Tự khỏi khi vá A3-1. **9 Hộ Thủ Giả 849** hỏng; 9 Bảo rương 844 KHÔNG hỏng (dialoger) |
| **A3-1c** | **CHẶN (đang chạy thật, có từ 21/08)** | `script\item\bosscharm.lua:99` | Boss Hoàng Kim: chính phái + chưa-vào-phái không đánh được. Tự khỏi khi vá A3-1 |
| **A3-9** | **CHẶN** | `E:\…\bin\server\settings\TimerTask.txt` | Thêm đúng 5 dòng `28/29/41/42/43`. **Không** chép đè cả tệp Linux (nuốt dải 50-77) |
| **BS-A** | **NẶNG** | `autoexec_npc_hd3.lua:22` + `\script\startgame\thanh\*.lua` (7 tệp) | Bỏ MỘT trong hai nguồn sinh NPC 769 |
| **BS-C** | **NẶNG** | `hd3_driver.lua:28-31` / `:60` | Cắt `HD3_PLD_BOAT` còn 3 điểm bờ Nam, hoặc gắn script bờ Bắc riêng |
| **A3-2** | NẶNG (mã chết) | `boss.lua:28/35/44/51/60/67` | Tự khỏi khi vá A3-1 |
| **A3-3** | NẶNG (chưa nối) | `battles\battlehead.lua:384/386/400/402/437/482` | Đổi sang `AddNpcEx` **sau khi** A3-1 đã vá |
| **BS-B** | NẶNG / CHƯA XÁC MINH | `basemission\lib.lua:33`, `hundred_arena.lua:635`, `tongcastle.lua:937` | Tự khỏi khi vá A3-1; ghi vào bảng ảnh hưởng |
| **A3-4** | NHẸ | `activitysys\config\32\talkdailytask.lua:27` | NPC đối thoại bị ép hệ Kim — vá sau A3-1 |
| **A3-5** | NHẸ | `hd3_driver.lua:60`, `autoexec_npc_hd3.lua:22` | Ép phe cho `kind_dialoger`, không chặn đối thoại |
| **A3-6** | NHẸ (mã chết) | `global\thanh\npc\add_npc.lua:225` (trong `add_boss`) | Không ai gọi `add_npc_thanh()` |
| **A3-7** | NHẸ (tài liệu) | `thicong\b2_patch.py:9` và `:45-47` | Sửa chú thích + sửa số dòng A3 trích sai |
| **A3-8** | CHƯA XÁC MINH | `ScriptFuns.cpp:7005` vs `KNpc+0x181C` | Giữ nguyên ghi nhận |
