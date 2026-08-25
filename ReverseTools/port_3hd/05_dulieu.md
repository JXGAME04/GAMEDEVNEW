# 05 — KIỂM KÊ DỮ LIỆU NGOÀI (NPC / BẢN ĐỒ / VẬT PHẨM / TASK ID / BẢNG RIÊNG)

Phạm vi: 3 hoạt động cần port từ bản Linux (JX2/Kiếm Thế) sang dự án JX1 C++.

| Mã | Tên | Cây script gốc (bản Linux) |
|---|---|---|
| `satthu` | Săn boss sát thủ (杀手任务) | `script\task\tollgate\killbosshead.lua` + `script\task\tollgate\killer\*` |
| `phonglangdo` | Phong Lăng Độ (风陵渡船) | `script\missions\fengling_ferry\*` |
| `vuotai` | Vượt ải / Thách thức thời gian (时间的挑战 · 闯关) | `script\missions\challengeoftime\*` + `script\vng_feature\challengeoftime\npcNhiepThiTran.lua` |

Hai cây đối chiếu:
- **LINUX** = `D:\ServerLinux\server1`
- **JX1** = `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server` (và `...\bin\client`)

Tệp bảng kèm theo (cùng thư mục `D:\GAMEDEVNEW\ReverseTools\port_3hd\`):
`npc_can_them.csv`, `map_can_them.csv`, `map_pak_check.csv`, `map_client_pak_check.csv`,
`item_can_them.csv`, `task_id_doi_chieu.csv`, `bang_dulieu_phu.csv`, `satthu_bossboard.csv`.

---

## 0. TÓM TẮT KẾT LUẬN

| Hạng mục | Cần thêm vào JX1 | Ghi chú |
|---|---|---|
| **NPC** | **0 dòng mới** (141/141 id đã có sẵn trong `settings\npcs.txt` của JX1, đúng tên) | **3 ô bị chiếm dụng**: id **1032 / 1033 / 1034** đã bị JX1 đổi tên thành `Boss New Dragon 165/166/167` |
| **Bản đồ** | **0 bản đồ mới**; chỉ **1 dòng cấu hình**: thêm map **957** vào `Maps\WorldSet_GameServer.ini` | Dữ liệu `.wor` + `Region_S.dat` của **cả 78 map** đã có đủ trong pak máy chủ **và** pak client JX1 |
| **Vật phẩm** | **~13 mục** phải thêm/ánh xạ (xem §3) + **3 ngựa** `(0,10,19/20/21)` | Phần lớn dùng lại được; nhưng **DetailType lệch chuẩn** giữa 2 bản (JX2 `6,0,x` = JX1 `6,1,x`) |
| **Task ID** | **0 xung đột thật** | 19 id kiểm tra: 17 id **hoàn toàn trống** trong JX1; `200` và `1550` trùng nhưng **cùng ngữ nghĩa** |
| **Bảng dữ liệu phụ** | **10 tệp** phải chép/soạn (xem §5) | |
| **CHẶN TIẾN ĐỘ** | **1 hạng mục cứng** — xem §6 | Thiếu bảng toạ độ spawn Phong Lăng Độ ở **cả hai** cây và **mọi** pak |

---

## 1. NPC

### 1.1 Cách đánh chỉ số

`settings\npcs.txt` **không có cột Id**: NPC id = số thứ tự dòng dữ liệu − 1 (dòng 1 là tiêu đề).
Đã kiểm chứng: `AddNpc(724, …, "Thủy Tặc")` (`missions\fengling_ferry\mission.lua:22`) ↔ dòng 726 của
`D:\ServerLinux\server1\settings\npcs.txt` = `Thủy tặc`; `AddNpc(725, …)` (`fld_smalltimer.lua:37`) ↔
dòng 727 = `Boss Thủy tặc đầu lĩnh`.

- LINUX `npcs.txt`: **2353** dòng dữ liệu (id 0…2352), **103 cột**
- JX1 `npcs.txt`: **2035** dòng dữ liệu (id 0…2034), **87 cột**
- 16 cột chỉ có ở bản Linux (không có ở JX1): `DropRateFile, AIMaxTime, PhysicalDamageBase,
  PhysicalMagicBase, PoisonDamageBase, PoisonMagicBase, ColdDamageBase, ColdMagicBase,
  FireDamageBase, FireMagicBase, LightingDamageBase, LightingMagicBase, AuraSkillId,
  AuraSkillLevel, PasstSkillId, PasstSkillLevel`
  → khi so hàng chỉ so được 87 cột chung.

### 1.2 Danh sách NPC theo tính năng

**`satthu` — 59 id: 761…768, 770…820** (thiếu 769 trong bảng boss vì 769 là NPC dẫn nhập).
Nguồn: bảng `addkillertasknpc` trong `script\task\tollgate\killbosshead.lua` **dòng 5…180** —
**160 dòng**, mỗi dòng `{npcId, level, mapId, x, y, camp, tênBoss, isBoss, deathScript, giá-trị-task-1082}`.
Cấp 25/35/45/55/65/75/85/95, mỗi cấp 20 boss, mỗi bản đồ đúng 4 boss.
Xuất đầy đủ ra `satthu_bossboard.csv`.

NPC dẫn nhập: **769 “Nhiếp Thí Trần”**, đặt ở 7 thành trong
`script\global\autoexec_npc.lua:26–32` (map 11, 1, 37, 176, 162, 78, 80), script
`\script\task\tollgate\killer\nieshichen.lua`.

**`phonglangdo` — 5 id:**

| Id | Tên (bảng Linux) | Dùng ở |
|---|---|---|
| 724 | Thủy tặc | `mission.lua:22` — spawn 30 con mỗi chuyến (`fld_head.lua:16-17` `TNPC_THIEF`, `TNPC_THIEF_COUNT=30`) |
| 725 | Boss Thủy tặc đầu lĩnh | `fld_smalltimer.lua:37,44,60` — 3 đợt (phút 25/30/35) |
| 1692 | Thủy Tặc Đại Đầu Lĩnh. | `fld_smalltimer.lua:51` — chỉ khi `check_new_shuizeitask()==1`, 2 con |
| 511 | Trương Tông Chính | `boss.lua:35,51,67` — **TOÀN BỘ ĐANG BỊ COMMENT** (`boss.lua:18-25`) |
| 513 | Diệu Như | `boss.lua:28,44,60` — **TOÀN BỘ ĐANG BỊ COMMENT** |

**`vuotai` — 76 id:**

| Dải | Ý nghĩa | Nguồn |
|---|---|---|
| 975–993 | quái thường sơ cấp (28 ải) | `missions\challengeoftime\npc.lua:30–120` |
| 994–1001 | tiểu Boss **nam** sơ cấp | `include.lua:107-110` `tbRangeId[1]`, `npc.lua:125,127` |
| 1002–1005 | tiểu Boss **nữ** sơ cấp | `include.lua:107-110` |
| 1006 | đại Boss sơ cấp (“Nhiếp Thí Trần”) | `npc.lua:116` |
| 1007–1025 | quái thường cao cấp | `npc.lua:135–225` |
| 1026–1033 | tiểu Boss **nam** cao cấp | `include.lua:111-114` `tbRangeId[2]`, `npc.lua:230,232` |
| 1034–1037 | tiểu Boss **nữ** cao cấp | `include.lua:111-114` |
| 1038 | đại Boss cao cấp | `npc.lua:221` |
| 1673 | Tiểu Nhiếp Thí Trần | `npc.lua:239` `map_new_Ncp[1]` |
| 1674–1683 | 10 boss mới “Vượt ải_…” (Cổ Bách, Huyền Giác Đại Sư, Đường Bất Nhiễm, Lam Y Y, Thanh Hiểu Sư Thái, Chung Linh Tú, Hà Nhân Ngả, Đoan Mộc Duệ, Đạo Thanh Chân Nhân, Toàn Cơ Tử) | `npc.lua:240–249` |
| 1684 | Vượt ải_Người tiếp dẫn Mật Phòng | `npc.lua:254` `map_transfer_npc` |

NPC dẫn nhập cho `vuotai` **cũng là 769** — `nieshichen.lua:5` include
`\script\missions\challengeoftime\npc\dragonboat_main.lua`, mục thoại
“Tham gia khiêu chiến/want_playboat” (`nieshichen.lua:45`).

### 1.3 Đối chiếu với JX1 — kết quả

**Số NPC cần thêm mới: 0.** Toàn bộ 141 id (59 + 5 + 76 + id 769) **đã tồn tại** trong
`E:\...\bin\server\settings\npcs.txt` và **trùng tên** với bản Linux, trừ 3 ô dưới đây.

🔴 **XUNG ĐỘT Ô NPC — phải quyết định trước khi thi công:**

| Id | Tên ở LINUX | Tên ở JX1 | Cột lệch |
|---|---|---|---|
| 1032 | `(cao cấp) tiểu Boss nam 7` | `Boss New Dragon 165` | `Name`, `NpcResType` |
| 1033 | `(cao cấp) tiểu Boss nam 8` | `Boss New Dragon 166 8` | `Name`, `NpcResType` |
| 1034 | `(cao cấp) tiểu Boss nữ 1` | `Boss New Dragon 167` | `Name`, `NpcResType` |

Hệ quả: `get_random_npc_id()` (`include.lua:269-277`) bốc ngẫu nhiên trong `tbRangeId[2]`
= `{1026..1033}` (nam) và `{1034..1037}` (nữ) ⇒ **3/12 lần bốc sẽ ra NPC “Boss New Dragon”**
với hình ảnh (`NpcResType`) sai.
Đã quét toàn bộ 2811 tệp `.lua` của JX1: **không tệp nào gọi AddNpc 1032/1033/1034**
(3 kết quả grep bắt được là *item* `tbProp={4,1032,…}` trong `global\itemset.lua:455-457`, không phải NPC).
⇒ Rủi ro thấp; hai hướng: (a) khôi phục tên/`NpcResType` gốc cho 3 ô này, hoặc
(b) thu hẹp `tbRangeId[2]` để tránh 1032–1034.

Các ô còn lại chỉ lệch ở nhóm cột **`FireResistMax / ColdResistMax / LightResistMax /
PoisonResistMax / PhysicsResistMax`** (và đôi khi `ReviveFrame`, `LifeParam3`, `Level1..4`) —
đây là chênh lệch **cân bằng số** giữa hai bản, **không phải thiếu dữ liệu**. Chi tiết từng id ở
`npc_can_them.csv` (140 dòng).

**Vùng ID trống của JX1:** `npcs.txt` của JX1 **không có dòng trống nào** (0/2035).
Nếu về sau cần thêm NPC mới thì phải **nối vào cuối bảng, bắt đầu từ id 2035**.

---

## 2. BẢN ĐỒ

### 2.1 Danh sách map id theo tính năng (tổng 78 map riêng biệt)

| Tính năng | Map id | Nguồn |
|---|---|---|
| `satthu` (boss) | 4, 5, 10, 12, 14, 22, 23, 24, 42, 43, 56, 66, 71, 72, 73, 75, 76, 77, 83, 91, 93, 94, 117, 123, 135, 141, 143, 148, 164, 168, 181, 193, 194, 196, 201, 202, 225, 319, 321, 340 (**40 map**) | cột 3 của bảng `addkillertasknpc`, `killbosshead.lua:5-180` |
| `satthu` + `vuotai` (NPC 769) | 1, 11, 37, 78, 80, 162, 176 (**7 thành**) | `global\autoexec_npc.lua:26-32` |
| `phonglangdo` | 336 (bờ Bắc Phong Lăng Độ), 337/338/339 (3 thuyền), 175 (Tây Sơn thôn — điểm hồi sinh) | `fld_head.lua:12,14` (`boatMAPS`, `northMAP`), `fld_head.lua:143` `SetRevPos(175,1)` |
| `vuotai` sơ cấp | 464–479 (**16 map**) | `include.lua:91-96` `tbLevelMaps[1]` |
| `vuotai` cao cấp | 480–495 (**16 map**) | `include.lua:97-102` `tbLevelMaps[2]` |
| `vuotai` Mật Phòng | 957 | `include.lua:27` `CHUANGGUAN30_MAP_ID = 957` |

### 2.2 Đối chiếu JX1

Định dạng `MapList.ini` của hai bản **giống hệt nhau** (`<id>=<đường dẫn GBK>`, `<id>_name`,
`<id>_MapType`, …). LINUX `settings\maplist.ini` khai báo 984 map; JX1
`settings\MapList.ini` (server) và `bin\client\settings\MapList.ini` đều khai báo **1000 map**.

**Kết quả: 78/78 map đã có sẵn ở JX1, đường dẫn GBK TRÙNG KHỚP 100 %.**
Chỉ khác vài chỗ ở cột `_name` tiếng Việt (mojibake trong bản JX1, ví dụ map 464
`Thách thức thời gian (S?cấp 1)` thay vì `(Sơ cấp 1)`; map 957 còn để nguyên chữ Hán `闯关密室`).

**Dữ liệu bản đồ thật (kiểm tra bằng băm `KPakList::FileNameToId`):**

| Nhóm | `.wor` trong pak máy chủ JX1 | `.wor` trong pak client JX1 | `Region_S.dat` |
|---|---|---|---|
| 40 map `satthu` | ✅ `Pak\maps.pak` | ✅ `maps.pak` (map 340: `update01.pak`) | ✅ |
| 336/337/338/339/175 | ✅ `Pak\maps.pak` | ✅ `update01.pak` | ✅ |
| 464–495 | ✅ `Pak\maps.pak` | ✅ `update01.pak` + `update03.pak` | ✅ |
| 957 | ✅ `Pak\maps.pak` | ✅ `updatejx08.pak` | ✅ |

Đối chứng số lượng region (`\maps\<map>\v_%03d\%03d_Region_S.dat`) — **LINUX và JX1 bằng nhau tuyệt đối**:

```
特殊用地\杀手的试炼      JX1srv=4    Linux=4      (Vượt ải, 32 map dùng chung 1 cây thư mục)
特殊用地\安期炼丹洞      JX1srv=5    Linux=5      (Mật Phòng 957)
中原北区\渡船            JX1srv=4    Linux=4      (3 thuyền 337/338/339)
中原北区\风陵渡          JX1srv=757  Linux=757    (bờ Bắc 336)
西北北区\莫高窟          JX1srv=356  Linux=356    (map 340)
两湖区\武陵山\伏流洞迷宫   JX1srv=113  Linux=113    (map 73)
```

🟡 **Việc phải làm (1 dòng cấu hình):**
map **957 CHƯA được khai báo** trong `E:\...\bin\server\Maps\WorldSet_GameServer.ini`
(tệp này khai 910 map, id 1…995 nhưng thiếu 957). 77 map còn lại **đều đã có**.
⇒ Thêm 1 dòng `World9xx=957 --- Mật Phòng cửa ải` và tăng `Count`.

ℹ️ **Không phải chặn:** thư mục `bin\server\maps\` của JX1 chỉ có 86 tệp `<id>_srv.fp`
và **thiếu** cho phần lớn map trong danh sách. Đây **KHÔNG** phải dữ liệu bản đồ mà là
**cache lưới A\* cho bot**, engine tự sinh lần nạp đầu:
`Sources\Core\Src\KSubWorld.cpp:1928` — *“Lần đầu tính xong sẽ ghi cache \maps\<id>_srv.fp,
boot sau nạp thẳng từ cache”*, và chỉ chạy khi `IsBotPathMap(m_SubWorldID)`
(`KSubWorld.cpp:1929` *[đã sửa theo phản biện — trước ghi 1927]*). Tên tệp cố tình khác client
để không nuốt nhầm `%d.fp` trong `maps.pak` (`KSubWorld.cpp:3393-3397`, `sprintf` ở dòng 3397).

Chi tiết từng map: `map_can_them.csv` (92 dòng), `map_pak_check.csv`, `map_client_pak_check.csv`.

---

## 3. VẬT PHẨM

### 3.1 🔴 KHÁC BIỆT CẤU TRÚC PHẢI BIẾT TRƯỚC

Vật phẩm được tra bằng bộ ba **(Genre, DetailType, ParticularType)** trong các bảng
`settings\item\*.txt`. **Hai bản đánh số `DetailType` KHÁC NHAU cho genre 6:**

*[đã sửa theo phản biện — số dòng phía JX1 lệch 1 vì bản Linux có thêm dòng 2 `Vật phẩm kỹ năng 6 0 0`
mà JX1 không có]*

| Vật phẩm | LINUX `settings\item\004\magicscript.txt` | JX1 `settings\item\magicscript.txt` |
|---|---|---|
| Bảo rương | dòng **3** — `6  1  0` | dòng **2** — `6  1  0` |
| Trường Mệnh hoàn | dòng **4** — `6  **0**  1` | dòng **3** — `6  **1**  1` |
| Gia Bào hoàn | dòng **5** — `6  **0**  2` | dòng **4** — `6  **1**  2` |

⇒ Mọi lời gọi `AddItem(6, **0**, N, …)` trong script Linux phải đổi thành
`AddItem(6, **1**, N, …)` khi port. Trong 3 tính năng có 2 chỗ:
`(6,0,3)` Đại Lực hoàn và `(6,0,6)` Phi Tốc hoàn
(`killer\mibao_head.lua:16-17`, `kill_level.lua:105-106`, `challengeoftime\chuangguang30.lua:45-46`).

Kích thước bảng: LINUX `magicscript.txt` 4996 dòng / JX1 4866 dòng;
LINUX `questkey.txt` (004) 1327 dòng / JX1 1343 dòng;
LINUX `horse.txt` 331 dòng / JX1 136 dòng; LINUX `goldequip.txt` 5939 / JX1 7399.

🔴 *[bổ sung theo phản biện]* **LƯỢC ĐỒ CỘT CỦA HAI BÊN KHÁC NHAU — KHÔNG chép nguyên dòng được:**

| Bảng | Số cột LINUX | Số cột JX1 | Ghi chú |
|---|---|---|---|
| `magicscript.txt` | **30** (tiêu đề chữ Hán `名称/ItemGenre/…`) | **14** (`Name/Genre/DetailType/ParticularType/ImageName/ObjIdx/Width/Height/Intro/Script/Price/ShortKey/nMaxStack/PickExecute`) | LINUX có thêm 16 cột kỹ năng/xếp chồng (`技能ID`, `参数2…5`, `Target`, `要求*相同`) |
| `horse.txt` | **46** | **89** | ngược chiều — JX1 nhiều cột hơn |
| `questkey.txt` | **10** (có cột `ParticularType` ở vị trí 9) | **11** (**KHÔNG có** cột `ParticularType`) | |

⇒ 14 dòng vật phẩm mới phải **soạn theo lược đồ JX1**, không copy-paste từ bảng Linux.

🔴 **Mức trôi của bảng genre 6 (đo thật):** trong **4.352** bộ ba `(6,d,p)` có ở **cả hai** bảng,
chỉ **188 mục (4,3 %)** trùng tên; **4.164 mục lệch tên**. Nói cách khác `magicscript.txt` của
hai bản gần như **đánh số lại hoàn toàn** ⇒ **mặc định phải tra theo TÊN**, cấm giả định trùng id.
Ngược lại `questkey.txt` (genre 4) **ổn định**: 5/5 mục 3 tính năng dùng đều trùng tên.

### 3.2 Kết quả đối chiếu (đầy đủ ở `item_can_them.csv`, 83 dòng)

**`phonglangdo` — 8 mục.** 🔴 *[đã sửa theo phản biện — bản trước ghi “5 dùng lại nguyên id”, SAI]*

Bản trước chỉ kiểm **id có tồn tại trong bảng JX1 hay không** rồi đánh ✅ “trùng id”, **không so TÊN**.
So lại tên thì **7/8 mục là VẬT PHẨM KHÁC HẲN** — dùng nguyên id sẽ phát nhầm đồ:

| Bộ ba (Linux) | Tên ở LINUX | Tên ở **JX1 cùng id** | Kết luận |
|---|---|---|---|
| `6,1,196` | Mật đồ thần bí — `fld_head.lua:197` | **Thượng Thiên lệnh** | 🔴 KHÁC ĐỒ — phải ánh xạ lại |
| `4,489` | Lệnh bài Phong Lăng Độ — `fld_head.lua:269` | Lệnh bài Phong Lăng Độ | ✅ **thật sự trùng** (`questkey.txt`) |
| `6,1,2745` | Lệnh Bài Thủy Tặc — `fld_head.lua:274` | **Thỏng gừ** | 🔴 KHÁC ĐỒ |
| `6,1,1094` | Thí Giả Chi ấn — `bossdeath.lua:22` | **Thiệp Hoàng Kim Đại Lực Thần** | 🔴 KHÁC ĐỒ |
| `6,1,2015` | Truy công lệnh — `bossdeath.lua:31`, `shuizeideath.lua:13` | **Khiêu chiến Lễ bao** | 🔴 KHÁC ĐỒ |
| `6,1,2115` | Hải long châu — `bossdeath.lua:42` (0,5 %) | **Bách Hoa Lộ** | 🔴 KHÁC ĐỒ |
| `6,1,2743` | Bảo Rương Thủy Tặc — `mission.lua:120` | **Tứ Mãng Giáp Định Phù** | 🔴 KHÁC ĐỒ |
| `6,1,30228` | Chân Nguyên Đơn (trung) — `fld_head.lua:123` | — | ⚠️ ánh xạ **`6,1,4846`** (JX1 = “Chân Nguyên Đơn (Trung)”, ✅ đúng đồ) |

**`satthu` — 37 mục:**

- 🔴 *[đã sửa theo phản biện]* ~~Trùng id, dùng ngay~~ — **KHÔNG mục nào dùng lại được nguyên id.**
  Đã so tên 18/18 bộ ba: **18/18 LỆCH TÊN**. Cụ thể (LINUX → JX1 cùng id):
  `6,1,399` Sát Thủ lệnh → **Sát thủ giản** · `6,1,400` Sát thủ giản → **Sơ đồ thiết…**
  (bảng JX1 **lệch đúng 1 ô** ở cặp này) · `6,1,2347` Sát Thủ Bí Bảo → **Khấp Địa Quần lễ hộp** ·
  `6,1,2348` Huyền Thiên Chùy → **Sát Thủ Giản lễ hộp** · `6,1,71` Tiên Thảo Lộ → **Tiên Thảo Lộ Thượng** ·
  `6,1,906` Quế Huy Hoàng (cao) → **Quế Huy Hoàng (trung)** · `6,1,907` Quế Hoàng Kim → **Quế Huy Hoàng (cao)** ·
  `6,1,1781` Cẩm nang thay đổi trời đất → **Kim Trử** · `6,1,2006` Khiêu chiến lễ bao → **`<<未翻译>>`** ·
  `6,1,2350–2353` Tứ Mãng/Huyền Viên/Thương Lang/Vân Lộc Lệnh → **Hộp bánh chưng / Hồng Bao Đoan Ngọ /
  Tiểu Hồng Hoa Đoan Ngọ / Lễ Hộp Đoan Ngọ** · `6,1,2823/2825/2826` → **ô trống chưa dịch** ·
  `6,1,3810/3811` Tinh Thiết/Tinh Tinh Khoáng → **稽查令 / 手中线**.
  ⇒ Toàn bộ 18 mục phải **tra lại theo TÊN** trong `magicscript.txt` của JX1 rồi ánh xạ id mới.
- Phải đổi DetailType: `6,0,3` → `6,1,3`; `6,0,6` → `6,1,6`.
- ⚠️ Ánh xạ id khác: `6,1,30557` Túi Dược Phẩm → JX1 **`6,1,4813`**.
- 🔴 **THIẾU hẳn trong bảng JX1 (9 mục)** — bộ Đồ Phổ Đằng Long, `kill_level.lua:107-115`:
  `30528` Đồ Phổ Đằng Long Khôi · `30530` Hài · `30531` Yêu Đái · `30532` Hộ Uyển ·
  `30533` Hạng Liên · `30534` Bội · `30535` Thượng Giới Chỉ · `30536` Hạ Giới Chỉ ·
  `30538` Đằng Long Thạch - Hạ.
- 🔴 **Ngựa thiếu 3 con** (`kill_level.lua:101-103`): `0,10,19` Phong Vân Bạch Mã ·
  `0,10,20` Phong Vân Chiến Mã · `0,10,21` Phong Vân Thần Mã — `horse.txt` của JX1
  **chỉ tới ParticularType 13** *[đã sửa theo phản biện — bản trước ghi nhầm “tới id 18”]*;
  bản Linux có 0…32. Tức JX1 thiếu cả dải **14…32**, không riêng 19/20/21.
  (`0,10,5/6/7/8` Ô Vân Đạp Tuyết / Bôn Tiêu / Phiên Vũ / Phi Vân **đã có**, tên trùng khớp.)
  ⚠️ Ba con này có `nExpiredTime=43200` (ngựa **có hạn**) — JX1 có hỗ trợ khoá này
  (18 chỗ dùng, ví dụ `event\tongwar\headinfo.lua:148`), không phải viết thêm.

**`vuotai` — 40 mục:**

- 🔴 *[đã sửa theo phản biện]* ~~Trùng id~~ — đã so tên 18 bộ ba: **chỉ 4 mục thật sự trùng tên**, 14 mục lệch.
  ✅ **Trùng tên, dùng ngay (4)**: `6,1,22` Tẩy Tuỷ Kinh · `6,1,26` Võ Lâm Mật Tịch ·
  `6,1,72` Thiên sơn Bảo Lộ · `6,1,73` Bách Quả Lộ · và `1,2,0` Thổ Tiên Mật (tiểu) (`award.lua:221`).
  🔴 **Lệch tên, phải ánh xạ lại (14)**: `6,1,71` Tiên Thảo Lộ → **Tiên Thảo Lộ Thượng** ·
  `6,1,122/123/124` Phúc Duyên Lộ (Tiểu/Trung/Đại) → **(Trung)/(Đại)/Quế Hoa Tửu**
  (bảng JX1 **lệch đúng 1 ô**) · `6,1,215` Càn Khôn Tạo Hoá Đan (đại) → **(trung)** ·
  `6,1,400` Sát thủ giản → **Sơ đồ thiết…** · `6,1,1392` Hộp lễ vật vượt ải →
  **Bao nguyên liệu An Bang hoàn hảo** · `6,1,2116` Thiên Niên Linh Dược → **Bột mì trộn mật ong** ·
  `6,1,2742` Bảo rương vượt ải → **Bảo Rương Tứ Mãng Khố Giới** · `6,1,3203` Hộ mạch đơn → **ô chưa dịch** ·
  `6,1,3810/3811` → **稽查令 / 手中线** · `6,1,4134` Chân Nguyên Đơn →
  **Lệnh bài Bắc Đẩu - Viêm Đế cấp 2**.
- Vật phẩm quest genre 4 (dùng qua `AddEventItem(N)` = vật phẩm `{4, N}`):
  `4,238` Lam Thủy Tinh · `4,239` Tử Thủy Tinh · `4,240` Lục Thủy Tinh ·
  `4,353` Tinh Hồng Bảo Thạch · `4,489` Lệnh bài Phong Lăng Độ — **cả 5 đều đã có** trong
  `questkey.txt` của JX1, **đúng id**. (Kiểm chứng ngữ nghĩa: `settings\activitysys\awardtable\11.txt:17`
  ghi `{szName="猩红宝石", tbProp={4,353,1,1,0,0}}`.)
- Ánh xạ id khác: `30006`→`4862`, `30228`→`4846`, `30229`→`4847`, `30289`→`3051/3946/4848`,
  `30301`→`4857`, `30505`→`4863`, `30506`→`4860`, `30507`→`4861`, `30529`→`4858`,
  `30537`→`4859`, `30557`→`4813`.
- 🔴 **THIẾU trong bảng JX1 (4 mục)**: `30009` Đồ Phổ Tinh Sương Hộ Uyển ·
  `30010` Đồ Phổ Tinh Sương Hạng Liên (`chuangguang30.lua:51-52`) ·
  `30533` Đồ Phổ Đằng Long Hạng Liên · `30538` Đằng Long Thạch - Hạ (`chuangguang30.lua:49-50`).

**Tổng vật phẩm phải thêm dòng mới vào bảng JX1: 13 mục hàng + 3 ngựa = 16 dòng**
(9 Đồ Phổ Đằng Long trùng nhau giữa 2 tính năng nên đếm 1 lần; danh sách gộp:
30009, 30010, 30528, 30530, 30531, 30532, 30533, 30534, 30535, 30536, 30538 và ngựa 19/20/21).

### 3.3 🔴 Khác chữ ký API: `AddGoldItem`

- LINUX: `award.lua:140` gọi **2 tham số** `AddGoldItem(item[2], item[3])`, ví dụ `AddGoldItem(0, 159)`.
- JX1: `Sources\Core\Src\ScriptFuns.cpp:4635-4665` `LuaAddGoldItem` chỉ đọc **1 tham số**
  (`nEventId`), tra `g_GoldItemTab` cột 10 lấy `Series`.

Bảng `goldequip.txt` của hai bản có **chỉ số dòng trùng nhau** cho toàn bộ 30 chỉ số mà
`map_random_awards` dùng (2, 6, 11, 21, 26, 39, 40, 46, 51, 61, 67, 71, 77, 81, 87, 94, 96,
107, 115, 122, 126, 132, 136, 159–167) — tên ở JX1 giống bản Linux, chỉ khác chỗ bản Linux
thêm tiền tố “Đồ phổ Hoàng Kim - ” cho dải 159–166.
⇒ Khi port phải **bỏ tham số đầu** (`AddGoldItem(0,159)` → `AddGoldItem(159)`), và
**đối chiếu lại tên** vì bảng ở bản Linux đã trôi so với chú thích trong script
(chú thích ghi `定国之青纱长衫` nhưng dòng 159 của bảng lại là “Định Quốc Ô Sa Phát Quán”).
*CHƯA XÁC MINH*: ý nghĩa tham số thứ nhất trong bản Linux (hàm ELF `0x0811F210` là lớp bọc
gọi `0x0811EE10` với hằng 1; chưa dò tiếp).

---

## 4. TASK ID

### 4.1 Trần task của JX1

`Sources\Core\Src\KPlayerTask.h:18-19`:
```
#define MAX_TASK       4200
#define MAX_TEMP_TASK  4200
```
Id lớn nhất mà 3 tính năng dùng là **4018** ⇒ **nằm trong trần, không phải nới**.

### 4.2 Bảng đối chiếu (đầy đủ ở `task_id_doi_chieu.csv`)

Đã quét **2811 tệp `.lua`** trong `E:\...\bin\server\script`.

| Task | Dùng làm gì ở bản Linux | Nguồn | Trạng thái ở JX1 |
|---|---|---|---|
| **1082** | `TSKID_KILLTASKID` — boss sát thủ đang nhận (1…160) | `task\newtask\newtask_head.lua:14`; đọc/ghi ở `nieshichen.lua:57,131,152`, `kill_level.lua:23` | **TRỐNG** (không có Set/GetTask nào). JX1 **đã có sẵn hằng số**: `newtask_head.lua:14` và `task\metempsychosis\task_head.lua:84` (`TSK_KILLER_ID = 1082`) |
| **1192** | `TSKID_KILLERDATE` — ngày nhận nhiệm vụ | `newtask_head.lua:15` | **TRỐNG**; hằng số đã có ở JX1 `newtask_head.lua:15` |
| **1193** | `TSKID_KILLERMAXCOUNT` — số lần giết trong ngày (trần `KILLER_MAXCOUNT=8`, `newtask_head.lua:20`) | `nieshichen.lua:224-229`, `lib_killlevel.lua:35,54` | **TRỐNG**; hằng số đã có ở JX1 `newtask_head.lua:16` |
| **1217** | tổng số nhiệm vụ sát thủ đã hoàn thành (đẩy lên ladder 10119) | `kill_level.lua:38,41,44` | **TRỐNG** |
| **2871** | `huoyuedu` — CountTask của hoạt động “杀手任务” | `settings\huoyuedu\huoyuedu.txt` dòng 12 | **TRỐNG** |
| **200** | `SetTaskTemp(200,1)` — cờ TẠM “đang trong phó bản” | `fld_head.lua:117`, `chuangguang30.lua:85` | ⚠️ **JX1 đã dùng 22 chỗ trong 13 tệp `.lua`** *[đã sửa theo phản biện — bản trước ghi 23; con số 23/26 là do đếm lẫn tệp sao lưu `hoatdong_admin.lua.truoc_phanbien_2408`]* (`missions\bw\bwhead.lua:62,137`, `missions\citywar_arena\head.lua:96`, …) nhưng **cùng ngữ nghĩa** (cờ tạm theo phó bản, tự xoá khi rời) ⇒ **không phải xung đột**, chỉ cần nhớ xoá cờ khi thoát |
| **3070** | TaskDaily — số lần nhận Chân Nguyên Đan khi lên thuyền (trần 5) | `fld_head.lua:121-122` | **TRỐNG** |
| **2863** | `huoyuedu` CountTask của “风陵渡船” | `huoyuedu.txt` dòng 3 | **TRỐNG** |
| **1505** | `SetTask` trong `mission_match.lua:51` | | **TRỐNG** |
| **1550** | `TSK_REMAIN_COUNT` — số lượt vượt ải còn lại | `include.lua:20` | ⚠️ JX1 dùng ở `event\storm\function.lua:396,417` `SetTask(1550,0)` — **chính là cùng đoạn mã đã port từ bản Linux** (`D:\ServerLinux\server1\script\event\storm\function.lua:396,417`, cùng chú thích `杀手进行次数`) ⇒ **không xung đột** |
| **1551** | `TSK_JOIN_DATE` — ngày tham gia gần nhất | `include.lua:19` | **TRỐNG** |
| **2606** | `tsk_toll_cg_passcount` — số ải đã qua (event sinh nhật 200905) | `event\birthday_jieri\200905\taskctrl.lua:36`; dùng ở `award.lua:101,127` | **TRỐNG** |
| **2641** | `TSK_Longxuewan_Date` — ngày dùng Long Huyết Hoàn | `include.lua:117` | **TRỐNG** |
| **2642** | `TSK_Longxuewan_Use` — số lần dùng Long Huyết Hoàn | `include.lua:118` | **TRỐNG** |
| **2852** | `PLAYER_MAP_TASK` — lưu map gốc để chia phe trong Mật Phòng | `include.lua:31`; `chuangguang30.lua:105,119` | **TRỐNG** |
| **3071** | bit-task, **bit 25** — cờ cấm tham gia | `dragonboat_main.lua:30`, `npcNhiepThiTran.lua:14` | **TRỐNG** |
| **3079** | TaskDaily trong `dragonboat_main.lua:166` | | **TRỐNG** |
| **4018** | `TSK_Longxuewan_avail` | `include.lua:119` | **TRỐNG** |
| **2864** | `huoyuedu` CountTask của “时间的挑战” | `huoyuedu.txt` dòng 4 | **TRỐNG** |

**Kết luận: 17/19 id hoàn toàn trống, 2 id trùng nhưng cùng ngữ nghĩa ⇒ KHÔNG có xung đột task id.**

### 4.3 Bảng xếp hạng (ladder) đi kèm

- `10119` — “thưởng kim thập đại liệp nhân”, ghi ở `kill_level.lua:44`
  `Ladder_NewLadder(10119, GetName(), Uworld1217+1, 1)`; đọc ở `npc.lua:125,230`.
- `10179` / `10180` — bảng xếp hạng Vượt ải sơ cấp / cao cấp, đọc ở `npc.lua:127,232`.

JX1 **đã có** engine ladder: `Sources\Core\Src\KJx2SharedStore.cpp:530` `LuaLadder_NewLadder`,
`:599` `LuaLadder_GetLadderInfo`, đăng ký ở `ScriptFuns.cpp:15085`; module `RELAYLADDER` được
liệt kê ở `ScriptFuns.cpp:2488`. Kho hiện tại `settings\jx2ladder.txt` mới có ladder `10118`
và `10999` ⇒ ba ladder trên sẽ **được tạo lúc chạy**, không phải chuẩn bị dữ liệu trước.

---

## 5. CÁC BẢNG RIÊNG

### 5.1 `settings\killer.ini` — **KHÔNG thuộc “săn boss sát thủ”**

🔴 **Đính chính quan trọng:** dòng chú thích đầu tệp là
`;通缉追杀系统专用ini` = *ini riêng của hệ **TRUY NÃ / TRUY SÁT giữa người chơi***, không phải
hệ 杀手任务 (boss sát thủ). Ba tính năng đang port **không đọc tệp này**.

| Khoá | Ý nghĩa |
|---|---|
| `[Main] MoneyPerHour=10000` | tiền thuê 1 giờ treo lệnh truy nã (phải khớp cấu hình Relay) |
| `MinTargetLevel=50` | cấp tối thiểu của mục tiêu mới được truy nã |
| `MaxActiveTaskTime=10` | thời gian kích hoạt tối đa của lệnh, đơn vị **giờ** |
| `MinReward=100000` | tiền thưởng tối thiểu |
| `[Messages] TargetLevelTooLow` | câu báo khi mục tiêu dưới cấp 50 |
| `[Messages] TargetAbsent` | câu báo khi mục tiêu không trực tuyến |

Ai đọc: **engine bản Linux** — chuỗi `MinTargetLevel` nằm ở `jx_linux_y` offset `0x20B2BF`
(địa chỉ ảo `0x082532BF`), được tham chiếu tại `0x0806F8FD`.
JX1 **đã có tệp** `E:\...\bin\server\settings\killer.ini` (bản chữ Hán, chưa dịch) nhưng
**mã C++ của dự án KHÔNG hề đọc** (grep `killer.ini` / `MinTargetLevel` trong `D:\GAMEDEVNEW\Sources`
= 0 kết quả) ⇒ tệp nằm chết. Không ảnh hưởng 3 tính năng.

**Bảng dữ liệu THẬT của “săn boss sát thủ” là `settings\task\tollgate\killer\killer.txt`**
(`nieshichen.lua:54` — `new(KTabFile, "/settings/task/tollgate/killer/killer.txt", "KILLER")`),
160 dòng, 3 cột: `Id | BossName | BossInfo` (mô tả vị trí, ví dụ dòng 2:
`1  Trác Lãnh Cầm  trong Phục Lưu động (193,184)…`). Dùng ở `nieshichen.lua:144,148,153`.

### 5.2 `settings\goldboss.txt` — bảng mẫu BOSS Hoàng Kim, **hiện là tệp chết ở bản Linux**

15 cột, khoá theo **TÊN NPC** (không phải id):
`Name | PhysicalDamageBase | PhysicalMagic | PoisonDamageBase | PoisonMagic | ColdDamageBase |
ColdMagic | FireDamageBase | FireMagic | LightingDamageBase | LightingMagic | AuraSkillName |
AuraSkillLevel | PasstSkillName | PasstSkillLevel`

Các cột `*DamageBase/*Magic` ghi dạng `a|b` (ngưỡng | giá trị), tương ứng 1-1 với 16 cột thừa
của `npcs.txt` bản Linux (§1.1) — tức đây là **bảng ghi đè thuộc tính đánh nguyên tố + hào quang +
kỹ năng bị động cho boss Hoàng Kim**. Tệp mã hoá **TCVN3**, 33 dòng dữ liệu.

Liên quan tới 3 tính năng: chỉ gián tiếp — có dòng `Trương Tông Chính` (dòng 2) và
`Diệu Như` (dòng 4), đúng hai NPC 511/513 mà `fengling_ferry\boss.lua` gọi… nhưng
**boss.lua đang bị comment toàn bộ** (`boss.lua:18-25`) nên thực tế **không dùng**.

🔴 **Bằng chứng tệp không được nạp ở bản Linux:** quét byte thô `jx_linux_y` (8.931.808 byte)
và 5 tệp `.so` — **không tồn tại** chuỗi `goldboss`, `AuraSkillName`, `PasstSkillName`.
(Chuỗi `GoldBoss` duy nhất ở offset `0x223B4D` nằm trong danh sách **tên bảng log/DB**:
`…LuckyStar\0GoldBoss\0…Lottery\0LotteryData…`, không phải tên tệp.)
Ngược lại `PhysicalDamageBase` (`0x20DD34`) và `AuraSkillId` (`0x20DDD9`) **có** — đó là tên cột
đọc từ `npcs.txt`.
⇒ Ở bản Linux này, thuộc tính boss Hoàng Kim lấy thẳng từ 16 cột thừa của `npcs.txt`,
`goldboss.txt` là **tàn dư của bản cũ**.

JX1 vẫn giữ tệp trong `Pak\namcung.pak` (và `client\data\update01.pak`); ghi chú trong mã dự án
`Sources\Core\Src\ScriptFuns.cpp:6871` xác nhận: *“Đánh Boss Hoàng Kim ở bản gốc đến từ MẪU NPC
(goldboss.txt) chứ không từ cờ này”*.
**Kết luận: KHÔNG cần port `goldboss.txt` cho 3 tính năng.**

### 5.3 `settings\trigger_challengeoftime.lua` — bộ hẹn giờ của Vượt ải

Đây là **script trigger do Relay gọi mỗi giờ** (chú thích dòng 1-3: *“本地活动，由本地Relay每小时触发”*,
tác giả wangbin, 2005-07-14). Nội dung `OnTrigger()`:

1. `Include("\\settings\\trigger_include.lua")` + `Include("\\script\\missions\\challengeoftime\\include.lua")`
2. Vòng qua `tbLevelMaps` (2 cấp × 16 map): `close_missions(map, MISSION_MATCH, VARV_STATE)`
   rồi `start_missions(map, MISSION_MATCH)` → mở lại vòng báo danh
3. `ChuangGuan30:KickOutAll()` + `ClearMapNpc(CHUANGGUAN30_MAP_ID)` → dọn Mật Phòng 957
4. `DynamicExecute("\\script\\missions\\challengeoftime\\chuangguang30.lua", "ChuangGuan30:AddTime")`

Hằng số nhịp lấy từ `include.lua`: `INTERVAL_MATCH = 3600` (1 giờ/lượt),
`TIME_SIGNUP = 10` phút báo danh (`LIMIT_SIGNUP`), `LIMIT_FINISH = 30*60` giây,
`LIMIT_PLAYER_COUNT = 8`, `AWARD_COUNT = 10`,
`CHUANGGUAN30_START_TIME = 10` / `CHUANGGUAN30_END_TIME = 22` (khung giờ mở Mật Phòng).

Trạng thái ở JX1: **không có tệp trên đĩa**; có bản cũ trong `client\data\update01.pak`.
`trigger_include.lua` cũng vậy. **Cả hai phải chép sang.**

### 5.4 `settings\huoyuedu\huoyuedu.txt` — bảng “Độ hoạt bát” (活跃度), dùng chung cả 3 tính năng

**15** cột *[đã sửa theo phản biện — bản trước ghi “16 cột” dù liệt kê đúng 15 tên]*:
`ActivityId | ActivityName | CountTask | MaxCount | Param1…Param10 | WeekResetFlag`
— `CountTask` là **task id lưu số lần**, `MaxCount`/`Param*` là các mốc điểm.
41 dòng dữ liệu. Ba dòng liên quan:

| ActivityId | ActivityName | CountTask | MaxCount | Param1 | Param2 | Thuộc tính năng |
|---|---|---|---|---|---|---|
| 2 | 风陵渡船 | **2863** | 2 | 5 | 5 | `phonglangdo` |
| 3 | 时间的挑战 | **2864** | 2 | 5 | 5 | `vuotai` |
| 11 | 杀手任务 | **2871** | 2 | 3 | 3 | `satthu` |

Điểm gọi:
- `phonglangdo`: `fld_smalltimer.lua:82` `DynamicExecuteByPlayer(…, "\\script\\huoyuedu\\huoyuedu.lua",
  "tbHuoYueDu:AddHuoYueDu", "fenglingdu")` — cộng cho mọi người trên thuyền sau
  `ENDSIGN_TIME + HUOYUEDU_TIME` (`fld_head.lua:26`, 3 phút sau khi thuyền rời bến)
- `satthu`: `lib_killlevel.lua:29` `… "tbHuoYueDu:AddHuoYueDu", "shashourenwu"`
- `vuotai`: qua `G_ACTIVITY:OnMessage("Chuanguan", …)` / `EventSys "ChuanGuan"` (`award.lua:254-255`)

🟡 **JX1 KHÔNG có hệ huoyuedu** (trên đĩa máy chủ): thiếu cả `settings\huoyuedu\huoyuedu.txt` lẫn
`script\huoyuedu\huoyuedu.lua`.
*[bổ sung theo phản biện]* Nhưng **`huoyuedu.txt` LẤY LẠI ĐƯỢC**: nó nằm trong
`E:\...\bin\client\data\slistcache.pak` (hash `0xAF5B6124`, 1.340 byte gốc, đã bản địa hoá
tiếng Việt — đọc được chuỗi “Thuyền Phong Lăng…”). Không phải soạn tay. Còn
`script\huoyuedu\huoyuedu.lua` thì **không có trong bất kỳ pak nào trong 44 pak** ⇒ vẫn phải chép từ Linux.
⚠️ Lưu ý: `slistcache.pak` là pak **client**; thư mục pak của máy chủ (`bin\server\Pak\`) chỉ có
`maps*.pak` + `namcung.pak`, nên phải **rút tệp ra và đặt lên đĩa máy chủ**, máy chủ không tự đọc được.

Trong cây JX1 vẫn còn **lời gọi mồ côi**
(`global\seasonnpc.lua:105`, `missions\leaguematch\head.lua:678`) và một ghi chú của phiên trước:
`missions\tongcastle\tongcastle.lua:517` — *“hệ huoyuedu (điểm hoạt bát) KHÔNG có trên JX1”*, dòng
gọi đã bị comment ở `:519`.
⇒ Khi port có 2 lựa chọn: (a) bỏ các lời gọi `AddHuoYueDu` (an toàn, `DynamicExecuteByPlayer`
vào tệp không tồn tại chỉ thất bại im lặng), hoặc (b) port luôn cả hệ huoyuedu.

### 5.5 Bảng toạ độ (`GetTabFileData`)

| Tệp | Dùng ở | LINUX | JX1 |
|---|---|---|---|
| `\settings\maps\challengeoftime\lineup8.txt` … `lineup56.txt` (7 tệp: 8/16/20/24/32/40/56) | `npc.lua:11-19` `map_posfiles`, đọc qua `func_npc_getpos`→`get_file_pos` (`include.lua:280-284,374-390`) | ✅ có (99–627 byte) | ❌ **thiếu, kể cả trong pak** |
| `\settings\task\tollgate\killer\killer.txt` | `nieshichen.lua:54` | ✅ có (14.033 byte, 160 dòng) | ✅ có trong `Pak\namcung.pak` (⚠️ **chưa đối chiếu nội dung**) |
| `\settings\maps\中原北区\渡船\渡船刷怪点.txt` *[đã sửa theo phản biện — trước ghi nhầm `中原地图`]* | `fld_head.lua:18` `npcthiefpos`; đọc ở `fld_head.lua:135,175-181`, `mission.lua:21`, `fld_smalltimer.lua:36,43,50,59` | ❌ **KHÔNG CÓ** | ❌ **KHÔNG CÓ** |

### 5.6 Bảng tỉ lệ rơi

`kill_level.lua:69`: `ITEM_DropRateItem(nNpcIndex, 8, "\\settings\\droprate\\boss\\bosstask_lev90.ini", 0, 10, nseries)`
(chỉ nhánh cấp 90 còn hoạt động; các nhánh lev20…lev80 ở `kill_level.lua:46-66` **đã bị comment**).

🔴 *[bổ sung theo phản biện]* **Tên hàm này KHÔNG tồn tại ở JX1.** Bản Linux đăng ký
`ITEM_DropRateItem` (`jx_linux_y.luamap.full.txt` dòng 961, `0x08154DE0`); JX1 đăng ký cùng chức năng
nhưng tên là **`DropRateItem`** (`ScriptFuns.cpp:14410` → `LuaDropRateItem` ở `:4315`, cùng 6 tham số
`nNpcIndex, nCount, pFileName, nUnknow, nItemLevel, nItemSeries`).
⇒ Khi port phải đổi `ITEM_DropRateItem(...)` → `DropRateItem(...)`.
Tương tự, `NPCINFO_GetSeries` (Linux `0x081C08E0`, dùng ở `kill_level.lua:37`) ở JX1 là
**`GetNpcSeries`** (`ScriptFuns.cpp:14504`). Và `AddExp_Skill_Extend` (`award.lua:185`)
**không có ở JX1** dưới bất kỳ tên nào — phải thay hoặc bỏ.

`bosstask_lev90.ini` (LINUX, ASCII): `[Main] Count=100, RandRange=1000000, MagicRate=1, MoneyRate=0,
MoneyScale=0, MinItemLevel=10, MinItemLevelScale=100, MaxItemLevel=10, MaxItemLevelScale=1`,
sau đó 100 khối `[n]` gồm `Genre / Detail / Particular / RandRate`.

JX1: thư mục `settings\droprate\boss\` **chỉ có `helianpiaodroprate.ini`** ⇒ phải chép
`bosstask_lev90.ini` sang (và `bosstask_lev20…80.ini` nếu định bật lại 7 cấp còn lại).
ℹ️ JX1 **đã có sẵn** `settings\droprate\npcdroprate_fenglindubei.ini` và
`npcdroprate_fenglindunan.ini` (droprate Phong Lăng Độ bờ Bắc / bờ Nam).

---

## 6. 🔴 CÁC ĐIỂM CHẶN TIẾN ĐỘ

### CHẶN #1 (CỨNG) — thiếu bảng toạ độ spawn Thủy tặc của Phong Lăng Độ

`fld_head.lua:18` *[đã sửa theo phản biện — bản trước ghi nhầm thư mục `中原地图`, đúng phải là `中原北区`]*
```lua
npcthiefpos = "\\settings\\maps\\中原北区\\渡船\\渡船刷怪点.txt"
```
Tệp này quyết định vị trí spawn của **toàn bộ** NPC trên thuyền:
- 30 Thủy tặc (724) mỗi chuyến — `mission.lua:20-24`
- 3 lượt Thủy tặc đầu lĩnh (725) — `fld_smalltimer.lua:36,43,59`
- 2 Thủy Tặc Đại Đầu Lĩnh (1692) — `fld_smalltimer.lua:50`
- vị trí xuất phát của người chơi khi lên thuyền — `fld_head.lua:135-139`

**Đã tìm hết và KHÔNG thấy ở đâu:**
- quét đệ quy toàn bộ `D:\ServerLinux\server1` theo tên chứa `渡船` / `刷怪点` / `中原北区` → **0 kết quả**
- băm `KPakList::FileNameToId` đường dẫn trên rồi dò trong `D:\ServerLinux\server1\pak\maps.pak`,
  `E:\...\bin\server\Pak\*.pak` và `E:\...\bin\client\data\*.pak` → **không có trong pak nào**
- `settings\maps\` của bản Linux **không có thư mục tên chữ Hán nào** (chỉ 33 thư mục ASCII)

*[đã sửa theo phản biện]* Lần băm đầu dùng **đường dẫn sai** (`中原地图`). Tác tử phản biện đã băm lại
bằng đường dẫn đúng `\settings\maps\中原北区\渡船\渡船刷怪点.txt`
(hash `0x741B4168`, hàm băm được kiểm chứng trước bằng `\settings\task\tollgate\killer\killer.txt`
→ `0xA2D3A99B` → trúng `namcung.pak`) và dò lại **44 pak** (5 server + 38 client + 1 Linux,
514.459 mục) → vẫn **KHÔNG CÓ**. ⇒ **Kết luận CHẶN #1 vẫn đứng vững**, chỉ có bằng chứng cũ là hỏng.

⇒ Không thể chép nguyên bản. Phải **soạn lại bảng toạ độ**. Tham chiếu để dựng lại:
`fld_head.lua:13` `boatMAP_POS = {1646, 3233}`; `boss.lua:28,35` dùng `1636*32, 3221*32` và
`1632*32, 3221*32` trên map 337 ⇒ vùng chơi nằm quanh ô **(1630…1650, 3220…3235)**.
Định dạng: bảng TAB, cột 1 = X (đơn vị **pixel**, vì `fld_head.lua:136-137` chia 32), cột 2 = Y;
số dòng ≥ 30 (`GetTabFileHeight` → `random(totalcount)` ở `fld_head.lua:176-179`).
Map 337/338/339 chỉ có **4 region** ⇒ vùng hợp lệ rất nhỏ, phải kiểm bằng `.wor`/`Region_S.dat` thật.

### CHẶN #2 (MỀM) — thiếu 7 tệp `lineup*.txt` của Vượt ải ở cây JX1

`npc.lua:11-19` cần `\settings\maps\challengeoftime\lineup{8,16,20,24,32,40,56}.txt`.
Có đủ ở bản Linux (99–627 byte), **không có** ở JX1 (đĩa lẫn pak — đã băm lại đủ 7 tệp, MISS cả 44 pak).

🔴 *[sửa theo phản biện — KHÔNG chỉ là “chép 7 tệp”]* Chép tệp thôi **vẫn chạy sai**, vì hàm đọc bảng
**không tồn tại ở JX1**:

- `GetTabFileHeight()` / `GetTabFileData()` **không phải hàm engine** — chúng là **wrapper Lua**
  định nghĩa ở `D:\ServerLinux\server1\script\lib\file.lua:38` và `:46`, bọc quanh
  `TabFile_Load` / `TabFile_GetRowCount` / `TabFile_GetCell`.
- Cây JX1 **không có `script\lib\file.lua`** và **không có định nghĩa hai hàm này ở bất kỳ đâu**
  trong 2.811 tệp `.lua` (grep `function GetTabFileData|function GetTabFileHeight` = 0 kết quả).
  Tệp `file.lua` chỉ còn trong pak **client** `update03.pak`, mà pak máy chủ không nạp.
- Các hàm engine nền **thì có đủ**: `ScriptFuns.cpp:15284-15286` đăng ký
  `TabFile_Load` / `TabFile_GetCell` / `TabFile_GetRowCount` ⇒ **chỉ cần port phần script**, không phải sửa C++.

Ảnh hưởng: `fld_getadata()` (`fld_head.lua:176-179`) và `get_file_pos()` (`include.lua:280-284`)
sẽ gọi hàm `nil`. ⇒ Phải chép kèm `script\lib\file.lua` (hoặc tối thiểu 2 hàm trên).
(Chứng cứ phụ: `missions\tong\collectgoods\npcpoint.lua:8,14,15,…` của **chính cây JX1** đang gọi
`GetTabFileData` — tức đoạn đó hiện cũng hỏng/chết.)

### CHẶN #3 (MỀM) — 16 dòng vật phẩm phải soạn thêm

11 mục genre 6 (30009, 30010, 30528, 30530–30536, 30538) + 3 ngựa (`0,10,19/20/21`)
+ 2 mục cần đổi DetailType. Kèm theo là **tài nguyên ảnh**:
`\spr\vng\item\dophodanglong.spr`, `\spr\vng\item\longha.spr`,
`\spr\item\citydefence\fragment.spr` — *CHƯA XÁC MINH* các `.spr` này có trong pak client JX1 hay chưa.

### KHÔNG chặn (đã kiểm và loại trừ)

- ❌ ~~Thiếu NPC~~ — 141/141 đã có
- ❌ ~~Thiếu dữ liệu bản đồ~~ — `.wor` + `Region_S.dat` đủ ở cả pak server lẫn pak client
- ❌ ~~Thiếu `_srv.fp`~~ — chỉ là cache A\* cho bot, engine tự sinh (`KSubWorld.cpp:1928`)
- ❌ ~~Trùng task id~~ — 17/19 trống, 2 id trùng cùng ngữ nghĩa
- ❌ ~~Trần task 4200~~ — id lớn nhất dùng là 4018

---

## 7. PHỤ LỤC — phần “săn boss sát thủ” JX1 ĐÃ CÓ SẴN

Kiểm tra bất ngờ: `E:\...\bin\server\script\task\tollgate\killbosshead.lua`
**giống hệt từng byte** bản Linux — 384.819 byte, MD5 `159b2acec0344bf965785cc346460f8e`,
diff = 0 dòng. (Nhiều khả năng đã theo đợt port Tín Sứ / messenger.)

| Thành phần | JX1 |
|---|---|
| `script\task\tollgate\killbosshead.lua` (bảng 160 boss) | ✅ có, **giống hệt** |
| `script\task\tollgate\addtollgatenpc.lua` | ✅ có, **giống hệt** — nhưng **KHÔNG** gọi `add_killertasknpc(addkillertasknpc)` |
| `script\task\tollgate\killer\` (5 tệp) | ❌ **thiếu toàn bộ** |
| hằng số `TSKID_KILLTASKID/…DATE/…MAXCOUNT`, `DescLink_NieShiChen` | ✅ có ở `task\newtask\newtask_head.lua:13-16` |
| `settings\task\tollgate\killer\killer.txt` | ✅ có trong `Pak\namcung.pak` |
| `settings\droprate\boss\bosstask_lev90.ini` | ❌ thiếu |
| Đặt NPC 769 ở 7 thành (`global\autoexec_npc.lua`) | 🔴 **cả TỆP `global\autoexec_npc.lua` KHÔNG TỒN TẠI ở JX1** *[đã sửa theo phản biện]* |
| Gọi `add_killertasknpc(addkillertasknpc)` (bản Linux ở `global\autoexec.lua:158`) | 🔴 **cả TỆP `global\autoexec.lua` KHÔNG TỒN TẠI ở JX1** *[đã sửa theo phản biện]* |

🔴 *[sửa theo phản biện]* Bản trước ghi “thiếu 7 dòng / 1 lời gọi”, ngụ ý chỉ cần **sửa** tệp có sẵn.
Thực tế **cả hai tệp chủ nhà đều không có** trong cây JX1 (`ls script\global\` không có
`autoexec.lua` lẫn `autoexec_npc.lua`; `autoexec.lua` chỉ còn bản JX2 cũ trong pak **client**
`update01.pak`, `autoexec_npc.lua` không có trong pak nào). Điểm vào lúc khởi động của JX1 là
**`script\startgame.lua`**.

✅ **Khuôn mẫu đã có sẵn trong cây, cứ theo:** `script\task\tollgate\tinsu_addnpc.lua` — đợt port
Tín Sứ đã gặp đúng vấn đề này và giải quyết bằng cách gom bảng NPC + hàm `add_dialognpc()` của
Linux vào một tệp riêng, rồi móc vào `startgame.lua:18` (`Include`) và `startgame.lua:114` (lời gọi).
Chú thích ngay trong tệp đó (`tinsu_addnpc.lua:3-7`) ghi rõ *“Du an khong co autoexec.lua -> gom vao day”*.

⇒ Việc phải làm cho `satthu`: chép 5 tệp `killer\*.lua`, 1 tệp droprate, **tạo mới**
`killer_addnpc.lua` (bảng 7 dòng NPC 769 + hàm nạp, theo khuôn `tinsu_addnpc.lua`) và **móc 2 dòng
vào `startgame.lua`**.

---

## 8. THỐNG KÊ CHỐT

| Chỉ số | Số lượng |
|---|---|
| NPC id 3 tính năng dùng | **141** (satthu 59 + phonglangdo 5 + vuotai 76 + NPC dẫn nhập 769) |
| NPC phải **thêm dòng mới** vào `npcs.txt` JX1 | **0** |
| NPC ô bị **chiếm dụng, phải xử lý** | **3** (1032, 1033, 1034) |
| Vùng NPC id trống của JX1 | **không có ô trống**; phải nối từ **2035** trở đi |
| Map id 3 tính năng dùng | **78** (40 + 7 thành + 5 + 32 + 1 Mật Phòng, đã trừ trùng) |
| Map phải **thêm dữ liệu** | **0** |
| Map phải **thêm dòng cấu hình** (`WorldSet_GameServer.ini`) | **1** (map 957) |
| Bộ ba vật phẩm 3 tính năng dùng | **83** dòng trong `item_can_them.csv` |
| Vật phẩm phải **thêm dòng mới** | **11** (genre 6) + **3** ngựa = **14** |
| Vật phẩm phải **ánh xạ lại id** | ~~13~~ → **≥ 48** *[đã sửa theo phản biện]* (13 mục dải `30xxx` đã biết **+ 35 mục** trước bị đánh nhầm “trùng id”: 7 phonglangdo + 18 satthu + 14 vuotai, trừ trùng lặp giữa các tính năng) |
| Bộ ba trước ghi “trùng id, dùng ngay” — **kiểm lại theo TÊN** | **42** bộ ba: chỉ **5 trùng tên**, **37 lệch tên** *[phản biện]* |
| Lời gọi phải **đổi DetailType** `6,0,x → 6,1,x` | **2** loại (`6,0,3`, `6,0,6`) — 4 chỗ gọi |
| Task id 3 tính năng dùng | **19** |
| Task id **xung đột thật** ở JX1 | **0** |
| Tệp bảng dữ liệu phụ phải chép/soạn | ~~10~~ → **12** *[đã sửa theo phản biện]* (7 `lineup*.txt` + `trigger_challengeoftime.lua` + `trigger_include.lua` + `bosstask_lev90.ini` + **`script\lib\file.lua`** + **`settings\huoyuedu\huoyuedu.txt`** rút từ `slistcache.pak`) |
| Tệp script phải **tạo mới** (không có tệp chủ nhà ở JX1) | **1** — `killer_addnpc.lua` theo khuôn `tinsu_addnpc.lua`, móc vào `startgame.lua` *[phản biện]* |
| Hàm phải **đổi tên** khi port | **2** (`ITEM_DropRateItem`→`DropRateItem`, `NPCINFO_GetSeries`→`GetNpcSeries`) + **1** không có (`AddExp_Skill_Extend`) *[phản biện]* |
| Điểm **chặn tiến độ cứng** | **1** (`渡船刷怪点.txt`) |

*Mọi khẳng định trong tài liệu này đều kèm đường dẫn tệp + số dòng, hoặc địa chỉ/offset ELF.
Các chỗ chưa dò tới được ghi rõ “CHƯA XÁC MINH”.*

---

## PHỤ LỤC PHẢN BIỆN (tác tử độc lập)

*Người kiểm chứng: tác tử phản biện, không phải người viết báo cáo. Phương pháp: mặc định coi mọi
khẳng định là SAI cho tới khi tệp gốc / ELF chứng minh ngược lại. Đã kiểm **35 khẳng định**, tìm được
**9 chỗ SAI** (2 nghiêm trọng) và **6 chỗ bỏ sót**. Mọi dòng SAI đã được sửa trong thân bài và đánh dấu
`[đã sửa theo phản biện]`.*

### A. Bảng kiểm chứng

| # | Khẳng định (bản gốc) | Bằng chứng gốc | KẾT LUẬN | Sửa lại thành |
|---|---|---|---|---|
| 1 | LINUX `npcs.txt` 2353 dòng dữ liệu / 103 cột; JX1 2035 / 87 | đếm thật: LINUX 2354 dòng (1 tiêu đề + 2353), 103 cột; JX1 2036 (1+2035), 87 cột | **ĐÚNG** | — |
| 2 | NPC id = số thứ tự dòng dữ liệu − 1; `AddNpc(724…)` ↔ dòng 726 “Thủy tặc” | dòng 726 = `Thủy tặc`, dòng 727 = `Boss Thủy tặc đầu lĩnh` (cả 2 cây) | **ĐÚNG** | — |
| 3 | Bảng `addkillertasknpc` 160 dòng; 59 npc id `761…768, 770…820`; 40 bản đồ; 8 cấp × 20 boss; mỗi map đúng 4 boss | phân tích `killbosshead.lua`: 160 dòng khớp regex trong 5–180; id/map/cấp khớp **từng con số** | **ĐÚNG** | (nhỏ) thân bảng thật là dòng **6–179**; dòng 4 = `addkillertasknpc={`, dòng 5 = chú thích, dòng 180 = `}` |
| 4 | NPC 769 đặt 7 thành ở `autoexec_npc.lua:26–32`, map 11/1/37/176/162/78/80 | đọc thật dòng 26–32 — 7 dòng, đúng thứ tự map | **ĐÚNG** | — |
| 5 | `add_killertasknpc(addkillertasknpc)` ở `global\autoexec.lua:158` (Linux) | dòng 158 đúng nguyên văn | **ĐÚNG** | — |
| 6 | JX1 id 1032/1033/1034 = `Boss New Dragon 165/166/167` | `npcs.txt` JX1 dòng 1034/1035/1036 ↔ LINUX `(cao cấp) tiểu Boss nam 7/nam 8/nữ 1` | **ĐÚNG** | — |
| 7 | `tbRangeId[2]` = `{1026,1033}` + `{1034,1037}` ⇒ 3/12 lần bốc sai | `include.lua:111-114`; nam 2/8 = 25 %, nữ 1/4 = 25 % | **ĐÚNG** | (làm rõ) xác suất thật là **25 % mỗi lần bốc** cho cả hai giới, không phải “3 trong 12 lượt” |
| 8 | `CHUANGGUAN30_MAP_ID = 957` (`include.lua:27`); `tbLevelMaps` 464–479 / 480–495 (`:91-96`, `:97-102`); task 1550/1551/2852/2641/2642/4018 ở `:20/:19/:31/:117/:118/:119` | đọc thật `include.lua` — **khớp từng dòng** | **ĐÚNG** | — |
| 9 | Map 957 chưa khai trong `Maps\WorldSet_GameServer.ini` | tệp có `Count=910`, 910 dòng `World*`, grep `957` = 0 kết quả (có 950 và 959) | **ĐÚNG** | — |
| 10 | JX2 dùng `DetailType 0`, JX1 dùng `1` cho genre 6 | LINUX dòng 4 `Trường Mệnh hoàn 6 0 1`; JX1 `6 1 1` | **ĐÚNG (nội dung)** / **SAI (số dòng JX1)** | số dòng JX1 lệch 1 (2/3/4 chứ không phải 3/4/5) vì LINUX có thêm dòng 2 `Vật phẩm kỹ năng 6 0 0` |
| 11 | `MAX_TASK = MAX_TEMP_TASK = 4200` ở `KPlayerTask.h:18-19` | đúng dòng 18 và 19 | **ĐÚNG** | — |
| 12 | `LuaAddGoldItem` (`ScriptFuns.cpp:4635-4665`) chỉ nhận 1 tham số; Linux `award.lua:140` gọi 2 | đọc thân hàm: chỉ `Lua_ValueToNumber(L,1)`; `award.lua:140` = `AddGoldItem(item[2], item[3])`, dữ liệu `{"…",0,159}` | **ĐÚNG** | — |
| 13 | `killer.ini` là hệ truy nã PvP, JX1 không đọc | dòng 1 = `;通缉追杀系统专用ini`; grep `killer.ini`/`MinTargetLevel` trong `Sources` = **0** | **ĐÚNG** | — |
| 14 | `goldboss.txt` là tệp chết ở Linux: 0 chuỗi `goldboss`/`AuraSkillName`/`PasstSkillName`; `GoldBoss`@`0x223B4D` nằm trong danh sách tên bảng log; `PhysicalDamageBase`@`0x20DD34`; `AuraSkillId`@`0x20DDD9` | quét byte thô `jx_linux_y` (8.931.808 B): **khớp CHÍNH XÁC cả 5 offset**, ngữ cảnh đúng `…LuckyStar\0GoldBoss\0…Lottery\0LotteryDa…` | **ĐÚNG** (mục vững nhất báo cáo) | — |
| 15 | `ScriptFuns.cpp:6871` ghi chú boss Hoàng Kim đến từ `goldboss.txt` | đúng dòng 6871 | **ĐÚNG** | — |
| 16 | `killbosshead.lua` giống hệt từng byte: 384.819 B, MD5 `159b2acec0344bf965785cc346460f8e` | `md5sum` cả hai cây: **trùng khít** | **ĐÚNG** | — |
| 17 | `addtollgatenpc.lua` giống hệt nhưng KHÔNG gọi `add_killertasknpc(addkillertasknpc)` | MD5 `8b6c3e7386b4e21d9f4570cc3fbd244c` cả hai; JX1 chỉ gọi với `AddNpc_turesureboss/bug` (`:16,17`) | **ĐÚNG** | — |
| 18 | 5 tệp `script\task\tollgate\killer\*.lua` thiếu toàn bộ ở JX1 | thư mục không tồn tại; băm cả 5 tệp → MISS toàn bộ 44 pak | **ĐÚNG** | — |
| 19 | 🔴 Bảng spawn ở `\settings\maps\`**`中原地图`**`\渡船\渡船刷怪点.txt` | byte thật `fld_head.lua:18` = `d6 d0 d4 ad **b1 b1 c7 f8**` = **中原北区**, KHÔNG phải 中原地图 | **SAI (đường dẫn ⇒ hỏng cả bằng chứng băm pak)** | đổi thành `中原北区`; đã **băm lại đúng** (`0x741B4168`) trên 44 pak / 514.459 mục → vẫn MISS ⇒ **kết luận CHẶN #1 vẫn đúng** |
| 20 | 7 tệp `lineup*.txt` có ở Linux (99–627 B), không có ở JX1 kể cả pak | `ls` Linux đủ 7 tệp; thư mục JX1 không tồn tại; băm cả 7 → MISS 44 pak | **ĐÚNG** | — |
| 21 | `trigger_challengeoftime.lua` + `trigger_include.lua` chỉ còn trong `client\data\update01.pak` | băm → trúng **đúng `update01.pak`**; giải nén thấy `-- description : 闯关活动的触发器…wangbin…2005-07-14` | **ĐÚNG** | — |
| 22 | 🔴 `horse.txt` JX1 “chỉ tới id 18” | liệt kê thật: JX1 genre0/detail10 particular = **0…13**; LINUX = 0…32 | **SAI** | JX1 **chỉ tới 13**; thiếu cả dải 14–32 (kết luận “thiếu 3 ngựa 19/20/21” vẫn đúng) |
| 23 | 🔴🔴 42 bộ ba vật phẩm “trùng id, dùng ngay / dùng lại nguyên id” | so **TÊN** trong bảng thật hai bên: **chỉ 5/42 trùng tên, 37/42 là VẬT PHẨM KHÁC HẲN** (vd `6,1,196` Mật đồ thần bí → JX1 “Thượng Thiên lệnh”; `6,1,2743` Bảo Rương Thủy Tặc → “Tứ Mãng Giáp Định Phù”; `6,1,122/123/124` lệch đúng 1 ô) | **SAI NGHIÊM TRỌNG** | §3.2 đã viết lại toàn bộ; số “phải ánh xạ lại id” 13 → **≥ 48**. Gốc lỗi: bản trước chỉ kiểm *id có tồn tại*, không so tên |
| 24 | 5 vật phẩm genre 4 (`4,238/239/240/353/489`) đã có, đúng id | so tên `questkey.txt` hai bên: **5/5 trùng tên** | **ĐÚNG** | — |
| 25 | `activitysys\awardtable\11.txt:17` ghi `{szName="猩红宝石", tbProp={4,353,1,1,0,0}}` | đọc thật dòng 17 — **nguyên văn** | **ĐÚNG** | — |
| 26 | Ánh xạ `30228`→`4846`, `30557`→`4813` | JX1 `6,1,4846` = “Chân Nguyên Đơn (Trung)”, `6,1,4813` = “Túi dược phẩm” — khớp `szName` trong script Linux | **ĐÚNG** | — |
| 27 | 9 mục Đồ Phổ Đằng Long (`30528,30530–30536,30538`) thiếu ở JX1; ngựa ở `kill_level.lua:101-103`; `(6,0,3)`/`(6,0,6)` ở `:105-106` | đọc thật `kill_level.lua:101-115` — **khớp từng dòng, đúng 9 mục** | **ĐÚNG** | — |
| 28 | Task 1550 dùng ở JX1 `event\storm\function.lua:396,417`, cùng đoạn mã port từ Linux | cả hai cây: **cùng dòng 396 và 417, cùng chú thích** `杀手进行次数` | **ĐÚNG** | — |
| 29 | Hằng số 1082/1192/1193 đã có ở JX1 `newtask_head.lua:14-16`; `KILLER_MAXCOUNT=8` ở `:20` | hai tệp **giống nhau từng dòng** | **ĐÚNG** | — |
| 30 | `SetTaskTemp(200,…)` JX1 “đã dùng 23 chỗ” | grep `--include=*.lua`: **22 chỗ / 13 tệp** (26 nếu đếm cả tệp sao lưu `.truoc_phanbien_2408`) | **SAI (nhỏ)** | **22 chỗ trong 13 tệp `.lua`** |
| 31 | `huoyuedu.txt` 41 dòng, “16 cột”; hàng 2863/2864/2871 ở dòng 3/4/12 | tệp có **15 cột** (`ActivityId…WeekResetFlag`), 41 dòng dữ liệu; ba hàng khớp **chính xác** (`2 风陵渡船 2863 2 5 5`, `3 时间的挑战 2864 2 5 5`, `11 杀手任务 2871 2 3 3`) | **SAI (nhỏ)** phần cột / **ĐÚNG** phần còn lại | **15 cột** (chính báo cáo liệt kê đúng 15 tên) |
| 32 | JX1 thiếu cả `settings\huoyuedu\huoyuedu.txt` lẫn `script\huoyuedu\huoyuedu.lua` | `huoyuedu.txt` **CÓ** trong `client\data\slistcache.pak` (hash `0xAF5B6124`, 1.340 B, đã dịch tiếng Việt — đọc được “Thuyền Phong Lăng…”); `huoyuedu.lua` MISS cả 44 pak | **SAI MỘT NỬA** | `huoyuedu.txt` **rút được từ pak**, không phải soạn tay; chỉ `huoyuedu.lua` mới phải chép từ Linux |
| 33 | `tongcastle.lua:517` có ghi chú “hệ huoyuedu KHÔNG có trên JX1”, lời gọi bị comment ở `:519` | đọc thật dòng 514–520 — **đúng cả hai số dòng** | **ĐÚNG** | — |
| 34 | `killer.txt` 160 dòng, 3 cột `Id\|BossName\|BossInfo`, dòng 2 = `1 Trác Lãnh Cầm … (193,184)`; có trong `namcung.pak` | 14.033 B, 160 dòng dữ liệu, header đúng 3 cột, dòng đầu khớp; băm `0xA2D3A99B` → trúng `namcung.pak` | **ĐÚNG** | — |
| 35 | `_srv.fp` chỉ là cache A* cho bot (`KSubWorld.cpp:1928`), chỉ chạy khi `IsBotPathMap` (`:1927`) | chú thích đúng ở dòng 1928; nhưng `IsBotPathMap` ở **dòng 1929** | **ĐÚNG (kết luận)** / **SAI (1 số dòng)** | `KSubWorld.cpp:**1929**` |
| 36 | `droprate\boss\` của JX1 chỉ có `helianpiaodroprate.ini` | `ls` xác nhận; Linux có đủ `bosstask_lev20…lev90.ini` | **ĐÚNG** | — |
| 37 | Đã quét 2.811 tệp `.lua` của JX1 | `find -iname "*.lua" \| wc -l` = **2811** | **ĐÚNG** | — |
| 38 | LINUX `maplist.ini` 984 map, JX1 `MapList.ini` 1000 map | đếm `_name`: 984 / 1000; JX1 có `Count=1000` | **ĐÚNG** | — |
| 39 | `npc.lua`: 1673 ở `:239`, 1674–1683 (10 boss) ở `:240-249`, 1684 ở `:254`; `map_posfiles` ở `:11-19` | đọc thật — **khớp cả 10 tên boss và cả 4 số dòng** | **ĐÚNG** | — |
| 40 | `boss.lua:18-25` comment toàn bộ; NPC 511/513 ở `:35,51,67` và `:28,44,60`; toạ độ `1636*32/1632*32, 3221*32` | đọc thật — **khớp cả 6 số dòng và toạ độ** | **ĐÚNG** | — |

**Tổng: 40 mục kiểm — 31 ĐÚNG, 9 SAI/lệch (2 nghiêm trọng: #19 đường dẫn spawn, #23 vật phẩm “trùng id”).**

### B. Bỏ sót đã tìm thêm

**BS-1 🔴🔴 — JX1 KHÔNG CÓ tệp chủ nhà `global\autoexec.lua` lẫn `global\autoexec_npc.lua`.**
Báo cáo (§7) viết “thiếu 7 dòng NPC 769” + “thiếu 1 lời gọi”, ngụ ý chỉ cần **sửa** tệp có sẵn.
Thực tế `ls E:\...\bin\server\script\global\` **không có cả hai tệp**; `autoexec_npc.lua` cũng
không có trong bất kỳ pak nào (44 pak), `autoexec.lua` chỉ còn bản JX2 cũ trong pak **client**
`update01.pak` (pak máy chủ không nạp). Điểm vào khởi động của JX1 là **`script\startgame.lua`**.
Khuôn mẫu đã có sẵn: `script\task\tollgate\tinsu_addnpc.lua` (đợt port Tín Sứ), tự ghi ở dòng 3–7
*“Du an khong co autoexec.lua -> gom vao day, startgame.lua goi tinsu_addnpc()”*, móc vào
`startgame.lua:18` + `:114`. ⇒ Việc thật là **tạo tệp mới** `killer_addnpc.lua`, không phải sửa 7 dòng.

**BS-2 🔴🔴 — `GetTabFileData()` / `GetTabFileHeight()` KHÔNG tồn tại ở JX1 ⇒ CHẶN #2 không “mềm”.**
Đây là **wrapper Lua** (không phải hàm engine), định nghĩa ở `D:\ServerLinux\server1\script\lib\file.lua:38`
và `:46`. Cây JX1 **không có `script\lib\file.lua`** và grep `function GetTabFileData|function GetTabFileHeight`
trong 2.811 tệp `.lua` = **0 kết quả**; `file.lua` chỉ còn trong pak client `update03.pak`.
Hệ quả: chép đủ 7 `lineup*.txt` **vẫn vô dụng** vì `fld_getadata()` (`fld_head.lua:176-179`) và
`get_file_pos()` (`include.lua:280-284`) gọi hàm `nil`. Tin tốt: hàm engine nền **có đủ** —
`ScriptFuns.cpp:15284-15286` đăng ký `TabFile_Load`/`TabFile_GetCell`/`TabFile_GetRowCount`
⇒ chỉ cần port phần script, không phải sửa C++.
(Dấu hiệu phụ: `missions\tong\collectgoods\npcpoint.lua:8,14,15,…` của **chính cây JX1** đang gọi
`GetTabFileData` — đoạn đó hiện cũng đang hỏng.)

**BS-3 🔴 — Ba hàm script phải đổi tên / không có khi port.**
Báo cáo §5.6 trích nguyên `ITEM_DropRateItem(...)` mà không nói tên này không tồn tại ở JX1:

| Bản Linux | JX1 | Bằng chứng |
|---|---|---|
| `ITEM_DropRateItem` (`0x08154DE0`, luamap dòng 961) | **`DropRateItem`** (cùng 6 tham số) | `ScriptFuns.cpp:14410` → `LuaDropRateItem` ở `:4315` |
| `NPCINFO_GetSeries` (`0x081C08E0`, dùng ở `kill_level.lua:37`) | **`GetNpcSeries`** | `ScriptFuns.cpp:14504` |
| `AddExp_Skill_Extend` (`award.lua:185`) | **KHÔNG CÓ dưới bất kỳ tên nào** | grep toàn `Sources` = 0 |

**BS-4 🔴 — Lược đồ cột của các bảng vật phẩm KHÁC NHAU ⇒ không copy-paste được dòng.**
Báo cáo chỉ so **số dòng** (4996/4866, 331/136…) mà không so **số cột**:
`magicscript.txt` LINUX **30 cột** ↔ JX1 **14 cột** (Linux thừa `技能ID`, `参数2…5`, `Target`,
8 cột `要求*相同`); `horse.txt` LINUX **46** ↔ JX1 **89** (ngược chiều); `questkey.txt`
LINUX **10** (có `ParticularType`) ↔ JX1 **11** (**không có** `ParticularType`).
⇒ 14 dòng vật phẩm mới phải **soạn theo lược đồ JX1**.

**BS-5 🔴 — Đo mức trôi của bảng genre 6 (giải thích gốc rễ lỗi #23).**
Trong **4.352** bộ ba `(6,d,p)` có mặt ở **cả hai** bảng `magicscript.txt`, chỉ **188 mục (4,3 %)**
trùng tên — **4.164 mục lệch**. Tức bảng genre 6 gần như **đánh số lại hoàn toàn** giữa hai bản.
Ngược lại `questkey.txt` (genre 4) **ổn định** (5/5 mục trùng). ⇒ Quy tắc thi công: **genre 4 tra
theo id, genre 6 BẮT BUỘC tra theo TÊN.**

**BS-6 ℹ️ — Ba ngựa `0,10,19/20/21` là ngựa CÓ HẠN.**
`kill_level.lua:101-103` kèm `nExpiredTime=43200` (phút ⇒ 30 ngày) mà báo cáo không nhắc.
JX1 **có hỗ trợ** khoá này (18 chỗ dùng, vd `event\tongwar\headinfo.lua:148,152`) ⇒ không phải
viết thêm, nhưng khi soạn 3 dòng `horse.txt` phải giữ thuộc tính hạn dùng.

### C. Đánh giá chung

Phần **dịch ngược nhị phân** (§5.2 — offset ELF `0x223B4D` / `0x20DD34` / `0x20DDD9`, kết luận
`goldboss.txt` là tệp chết) và phần **đếm bảng / số dòng script** là **chính xác gần như tuyệt đối**
— kiểm 5 offset và ~25 số dòng đều khớp. Điểm yếu tập trung ở **phép đối chiếu vật phẩm**: quy trình
so sánh chỉ hỏi *“id này có tồn tại ở JX1 không?”* thay vì *“id này ở JX1 có phải cùng vật phẩm không?”*,
dẫn tới 37/42 kết luận sai — đây là loại lỗi sẽ chỉ lộ ra khi người chơi nhận nhầm đồ trên máy chủ thật.
Hai điểm chặn mà báo cáo xếp “mềm” (CHẶN #2) thực ra **cứng hơn** vì thiếu `script\lib\file.lua`.

*Công cụ kiểm chứng đã dựng lại độc lập: bộ băm `KPakList::FileNameToId` (`KPakList.cpp:72-85`) +
đọc chỉ mục `XPackFileHeader`/`XPackIndexInfo` (`XPackFile.cpp:13-34`), quét **44 pak / 514.459 mục**;
hàm băm được kiểm chuẩn trước bằng 3 tệp đã biết trước kết quả.*
