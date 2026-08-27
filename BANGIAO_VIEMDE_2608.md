# BÀN GIAO — THI CÔNG VIÊM ĐẾ BẢO TÀNG (JX1)

> Bắt đầu 26/08/2026. Yêu cầu chủ game: **giống bản Linux 100%**, làm trọn rồi bàn giao một lần.
> Tài liệu đo đạc gốc: `PHANTICH_VIEMDE_EPDOTIM_HKMP.md`.
> **CHƯA KHỞI ĐỘNG LẠI MÁY CHỦ. CHƯA TEST.**

---

## 0. TRẠNG THÁI

| # | Việc | Trạng thái |
|---|---|---|
| 1 | Chép dữ liệu bản đồ 851–862 | ✅ xong, đã kiểm |
| 2 | Khai bể 15 bản đồ phòng chờ nhân bản | ✅ xong, đã kiểm |
| 3 | Khung phó bản `dungeon.lua` bản JX1 | ✅ xong, syncheck sạch |
| 4 | Vật phẩm: soát 37 mã + tạo 5 mã mới | ✅ xong, đã kiểm |
| 5 | 32 NPC template | ✅ xong, đã kiểm |
| 6a | Chép 26 tệp + đổi mã vật phẩm | ✅ xong, **0 lỗi cú pháp**, FFFD=0 |
| 6b-1 | Lớp dùng chung `ydbz_share.lua` | ✅ xong, syncheck sạch |
| 6b-2 | Đấu nối 3 tệp vào lớp dùng chung | ✅ xong |
| 6c | Vá bẫy #3/#5, `composeclass`, `GetItemBindState` | ✅ xong |
| 8a | Khối cấu hình `[7]` trong `cauhinh_hoatdong.lua` | ✅ xong |
| 8b | Trình điều khiển lịch `ydbz_driver.lua` | ✅ xong (**chưa móc**) |
| 9 | **Phản biện** | ✅ **0 lỗi, 0 cảnh báo, 10 mục đạt** |
| 7 | Hệ xúc xắc chia đồ (engine + client) | ⬜ chưa |
| 8 | Nối lịch + cấu hình + kiểm giao | ⬜ chưa |

Bộ công cụ: `D:\GAMEDEVNEW\ReverseTools\viemde\` — mọi bộ vá **chạy lại được nhiều lần**, có sao lưu `.truoc_viemde_2608`.

---

## 1. ĐÃ ĐỘNG VÀO NHỮNG TỆP NÀO

Cây máy chủ `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\`:

| Tệp | Thay đổi | Sao lưu |
|---|---|---|
| `settings\MapList.ini` | +62 dòng: khai 15 bản đồ 1003–1017 | `.truoc_viemde_2608` |
| `maps\WorldSet_GameServer.ini` | `Count 911→926`, +16 dòng World911–925 | `.truoc_viemde_2608` |
| `settings\npcs.txt` | 32 dòng NPC (1289–1319, 1338) | `.truoc_nan_eol_2608` |
| `settings\item\magicscript.txt` | +5 vật phẩm (4865–4869) | `.truoc_viemde_2608` |
| `maps\特殊用地\炎帝宝藏[准备场]\` | **mới**: 525 tệp `_Region_S.dat` + `.wor` + `.jpg` | — (thêm mới) |
| `script\missions\basemission\dungeon.lua` | **mới**, 296 dòng | — |

Cây client `...\bin\client\`:

| Tệp | Thay đổi |
|---|---|
| `maps\特殊用地\炎帝宝藏[准备场]\` | **mới**: 525 tệp `_Region_C.dat` + `.wor` + `.jpg` |
| `settings\item\magicscript.txt` | +5 vật phẩm (giống hệt máy chủ) | `.truoc_viemde_2608` |
| `spr\vng\item\longha.spr`, `dophodanglong.spr` | **mới**, lấy từ `D:\ServerLinux\Patch\spr\vng\item\` |

> ⚠️ **PHẢI KHỞI ĐỘNG LẠI MÁY CHỦ** — đã đổi `MapList.ini`, `WorldSet`, `npcs.txt`, `settings\item\*.txt`.
> Client cũng phải phát lại cho người chơi (bảng item + 2 ảnh + dữ liệu bản đồ).

---

## 2. BẢN ĐỒ — CÁCH GIẢI QUYẾT

Bản đồ **đã có sẵn** trong `bin\client\maps2\特殊用地\` (thư mục dựng sẵn, không phải thứ client đọc).

Bằng chứng dùng được (4 phép đo, xem `PHANTICH_...md` §3):
- Đoạn `OBSTACLE` dài **đúng 2048 byte** = cỡ `int[16][32]` mà `KRegion::LoadServerObstacle` (`KRegion.cpp:407`) đòi khớp tuyệt đối; **204/500** + **10/25** tệp có vật cản thật.
- `TRAP`/`NPC` rỗng ở cả 525 tệp — **đúng thiết kế**: tính năng tự sinh quái bằng `AddNpc` theo `settings\maps\yandibaozang\way*.txt`.
- Toạ độ kịch bản nằm trong vùng bản đồ (điểm rơi ô 1876,3276; vùng phủ X 1584–1984, Y 3200–3936).
- 3 ảnh nền dung nham dùng chung với `banghuichengbao` (đã chạy ở JX1).

**Máy chủ đọc `_Region_S.dat`, client đọc `_Region_C.dat`** — hai bản chung một định dạng ở đoạn OBSTACLE, nên bộ vá chép rồi **đổi tên** thay vì viết bộ chuyển đổi. `KRegion::LoadObject` (`KRegion.cpp:157`) **không có nhánh lui về `_C`** (nhánh đó chỉ có ở bộ nạp lưới A* `KSubWorld.cpp:236-340`).

`<id>.fp` / `<id>_srv.fp` **tự sinh** lần chạy đầu (`KSubWorld.cpp:2189`, `:3397`) — không phải chuẩn bị.

---

## 3. BỂ PHÓ BẢN — VÌ SAO KHÔNG PHẢI BẢN RÚT GỌN

Bản Linux nhân bản phòng chờ cho **từng tổ đội** (`readymap.lua:77`), nhưng **tự nó chặn cứng ở 15 phòng**
(`head.lua:47` `YDBZ_LIMIT_TEAMS_COUNT = 15`; `ready.lua:74` `if tbReady.nMapCount >= 15 then return 0 end`).

JX1 nạp bản đồ **một lần lúc khởi động** (`KSubWorldSet::Load`), không nhân bản lúc chạy. Nhưng **nhiều id trỏ cùng một thư mục bản đồ là hợp lệ** — JX1 đã làm sẵn: 379 và 380 dùng chung y hệt thư mục (`KSubWorld.cpp:1771`).

⇒ Khai trước **bể 15 bản đồ tĩnh** (1003–1017, cùng trỏ `特殊用地\炎帝宝藏准备场`), `ApplyDungeonMap()` phát ra một id rảnh, `ReturnDungenonMap()` thu về. **Người chơi thấy y hệt bản gốc**: mỗi tổ đội một phòng riêng, hết phòng thì phải chờ.

---

## 4. VẬT PHẨM — BẢNG TRA (Linux → JX1)

**Tra theo TÊN**, không dùng lại id (bẫy #7). Bộ soát: `v04_soat_vatpham.py`, phân giải trùng tên: `v05_phangiai_trungten.py`.

### 4.1 Đổi số (đã chắc)

| Linux | JX1 | Tên |
|---|---|---|
| `6,0,3` | `6,1,3` | Đại Lực hoàn |
| `6,0,6` | `6,1,6` | Phi Tốc hoàn |
| `6,1,147` | `6,1,146` | Huyền Tinh Khoáng Thạch |
| `6,1,215` | `6,1,214` | Càn Khôn Tạo Hóa Đan (đại) ← **JX1 215 là "trung"** |
| `6,1,398` | `6,1,397` | Thần bí khoáng thạch |
| `6,1,1309` | `6,1,1314` | Mảnh Bổ Thiên Thạch (trung) |
| `6,1,1604` | `6,1,1613` | Anh Hùng Thiếp |
| `6,1,1605` | `6,1,1614` | Hình nhân |
| `6,1,1606` | `6,1,1615` | Viêm Đế Đồ Đằng |
| `6,1,1617` | `6,1,1626` | Viêm Đế Lệnh |
| `6,1,1746` | `6,1,1755` | Phượng Minh Chùy |
| `6,1,2351` | `6,1,2360` | Huyền Viên Lệnh |
| `6,1,2532` | `6,1,2541` | Viêm Đế Lệnh Kỳ |
| `6,1,2744` | `6,1,3362` | Chìa Khóa Như ý |
| `6,1,2805` | `6,1,3423` | Viêm Đế Bí Bảo |
| `6,1,3810` | `6,1,4428` | Tinh Thiết Khoáng |
| `6,1,3811` | `6,1,4429` | Tinh Tinh Khoáng |
| `6,1,30006` | `6,1,4862` | Đồ Phổ Tinh Sương Y |
| `6,1,30191` | `6,1,2953` | Chìa khóa vàng |
| `6,1,30228` | `6,1,4846` | Chân Nguyên Đơn (trung) |
| `6,1,30301` | `6,1,4857` | Hỗn nguyên chân đơn |
| `6,1,30505` | `6,1,4863` | Đồ Phổ Tinh Sương Khí Giới |
| `6,1,30506` | `6,1,4860` | Tinh Sương Lệnh |
| `6,1,30507` | `6,1,4861` | Huyền Thiết |
| `6,1,30529` | `6,1,4858` | Đồ Phổ Đằng Long Y |
| `6,1,30537` | `6,1,4859` | Đồ Phổ Đằng Long Khí Giới |
| `6,1,30557` | `6,1,4813` | Túi Dược Phẩm |
| `4,417,1` | `4,417,1` | Tiền đồng — **khớp sẵn, giữ nguyên** |

### 4.2 Trùng tên — phân giải bằng thuộc tính vật phẩm

| Linux | JX1 chọn | Vì sao |
|---|---|---|
| `6,1,3203` Hộ Mạch Đơn | **`6,1,3821`** | trùng đường ảnh `xuelingzhuhun.spr` (ứng viên 4844 dùng ảnh khác); trùng cả đợt Bảo Rương |
| `6,1,30229` Chân Nguyên Đơn (đại) | **`6,1,4847`** | 4845/4846/4847 = tiểu/trung/đại; 30228→4846 đã chắc; trùng đợt Bảo Rương |
| `6,1,30289` Huyết Long Đằng | **`6,1,3051`** | 3051 và 3946 hoà nhau về thuộc tính → theo đợt Bảo Rương đã phát hành |
| `6,1,4134` Chân Nguyên Đan | **`6,1,3926`** | ⚠️ theo đợt Bảo Rương. **Nhưng `6,1,4752` mới là mã dùng ĐÚNG đường ảnh `wusetang.spr` của bản Linux** — chủ game muốn đúng ảnh thì đổi sang 4752 |

### 4.3 Năm vật phẩm tạo mới (giống bản Linux 100%, kể cả ảnh)

| Linux | JX1 mới | Tên | Ảnh |
|---|---|---|---|
| `6,1,30350` | `6,1,4865` | Hộp Mặt Nạ Chiến Trường | có sẵn `spr.pak` |
| `6,1,30538` | `6,1,4866` | Đằng Long Thạch - Hạ | **chép mới** `\spr\vng\item\longha.spr` |
| `6,1,30008` | `6,1,4867` | Đồ Phổ Tinh Sương Yêu Đái | có sẵn `update01.pak` |
| `6,1,30011` | `6,1,4868` | Đồ Phổ Tinh Sương Ngọc Bội | có sẵn `update01.pak` |
| `6,1,30534` | `6,1,4869` | Đồ Phổ Đằng Long Bội | **chép mới** `\spr\vng\item\dophodanglong.spr` |

---

## 5. NPC — 32 TEMPLATE

Nguồn: `D:\ServerLinux\Patch\settings\npcs.txt` (npcs.txt của **client bản VNG**, không phải cây `server1`).

**Lề id: `id = số dòng dữ liệu − 1`.** Chốt bằng hai mốc độc lập:
- id 769 = "Nhiếp Thí Trần" nằm ở dòng 770 (mốc đã biết từ đợt Sát Thủ).
- Ngũ hành của 10 boss đọc từ dòng (N+1) **trùng khớp từng cái** với bảng `YDBZ_map_npc` trong `npc.lua`.

Hai bảng **thẳng hàng từng dòng** (dòng 1338 cả hai đều là "Công Thành Chiến Xa"). JX1 có 87 cột, Linux 103; **87 cột đầu trùng tên hoàn toàn** nên chỉ cắt bớt.

32 dòng đích ở JX1 **đều đang `<<chưa định nghĩa>>`** — không đụng ai. **22/22 bộ hình NPC đã có** trong `client\settings\NpcRes\npcres_v4`.

NPC 389 "Chủ dược điếm 1" (2 NPC bán thuốc ở phòng chờ) **JX1 đã có sẵn**.

---

## 6. HAI LỖI ĐÃ MẮC — VÀ CÁCH CHẶN

| Lỗi | Hậu quả | Đã chặn bằng |
|---|---|---|
| Ghi thẳng `open(p,"w")`, nổ giữa chừng | `magicscript.txt` máy chủ về **0 byte** | `bangtxt.Bang.ghi()`: mã hoá **trước**, ghi tệp tạm, `os.replace` |
| Đọc/ghi bằng `"\n".join(...)` | **2036 dòng CRLF → LF** trong `npcs.txt` | `Bang` tự dò và giữ nguyên kiểu xuống dòng |
| Chỉ mã hoá cột `Name` sang TCVN3 | Ô mô tả còn ký tự Unicode ⇒ nổ khi ghi | `uni2tcvn()` áp cho **mọi ô** |

> 🔴 **LUẬT**: `check_encoding.py` in cả dòng `EOL` — **phải đọc dòng đó**, không chỉ nhìn `FFFD`.


---

## 6b. SỰ CỐ 26/08: 5 VẬT PHẨM MỚI LÀM **DUMP GAMESERVER** — ĐÃ SỬA

**Triệu chứng**: chủ game báo GameServer dump sau khi thêm 5 dòng vào `magicscript.txt`.

**Gốc**: tôi cắt 30 cột đầu của hàng bản Linux dán thẳng vào JX1. Hai bảng **khác nghĩa cột từ cột 9**:

| Cột | JX1 (`KBASICPROP_MAGICSCRIPT`, `KBasPropTbl.cpp:1004`) | Bản Linux |
|---|---|---|
| 9 | `m_szScript` — **CHUỖI** (đường dẫn `.lua` hoặc `"0"`) | 五行属性 (ngũ hành) |
| 10 | `m_nPrice` | 价格 |
| 11 | `m_bShortKey` | 等级 |
| 12 | `m_nMaxStack` — **số lượng** xếp chồng | 是否叠放 — chỉ là cờ 0/1 |
| **13** | **`m_nPickExecute` — SỐ** | **脚本名 — TÊN SCRIPT (chuỗi)** |
| 14 | `m_nParam` | 技能ID |

`LoadRecord` gọi `pTF->GetInteger()` cho cột 13; gặp chuỗi `\script\item
oscript.lua` thì
thất bại và `{ _ASSERT(FALSE); bEC = FALSE; break; }` — **bỏ dở việc nạp bản ghi**.

**Cách sửa (đã làm)** — `v08b_them_vatpham.py` thay cho `v08` (bản cũ đã đổi tên
`.SAI_DUNG_CHAY` + chèn `raise SystemExit`):

1. Lấy **hàng anh em có sẵn ở JX1** làm mẫu (`6,1,4862`): **15 cột**, `Script="0"`,
   `ShortKey=1`, `PickExecute=0`, `nParam` rỗng.
2. Điền **từng ô theo nghĩa cột JX1**; `nMaxStack` lấy từ **cột 20** của bản Linux
   (物品最大叠放值 = 8/250), không phải cột 12.
3. Kiểm lại bằng cách **mô phỏng đúng vòng đọc của `LoadRecord`** (kiểu char/int từng ô).

Kết quả: 5 hàng, đều 15 cột, **0 ô sai kiểu**, CRLF giữ nguyên, FFFD=0.

### Hộp Mặt Nạ Chiến Trường — đã dịch ngược đầy đủ

Bản Linux có script thật `\scriptng_event\itemoxmask\mask_param.lua`.
Đã soát từng mắt xích trước khi viết bản JX1 (`v16_gen_maskbox.py` →
`\script\itemiemde\mask_param.lua`):

- Mặt nạ `{0,11,647}` / `{0,11,828}`: `mask.txt` của JX1 **thẳng hàng với bản Linux**
  (id = số dòng − 1) — 647 "Vương Giả", 828 "Bá Giả", **có sẵn**.
- `PlayerFunLib:VnCheckInCity`: có (`vng_feature\checkinmap.lua:2`).
- `tbAwardTemplet:Give` của JX1 **bỏ qua** `nExpiredTime`/`nUsageTime`/`nBindState`
  (`libwardtype\item_jx1.lua` ghi rõ "nExpiredTime bỏ qua (lệch có chủ đích)")
  ⇒ **không dùng Give**; tự `AddItem` rồi áp bằng API thật:
  `ITEM_SetExpiredTime` (`KJx2WarInfra.cpp:1399` — giá trị < 20.000.000 là **SỐ PHÚT**),
  `ITEM_SetLeftUsageTime` (`:1481`), `SetItemBindState(idx,-2)` (`:1504`).

### 🔴 Bẫy mới ghi nhận: `AddItem` và 6 ô tham số

`LuaAddItem` chỉ chép riêng 6 ô `nItemLevel[0..5]` **khi truyền ≥ 15 đối số**;
ít hơn thì nó **nhân bản tham số thứ 7 ra cả 6 ô**. `GetItemParam(idx,k)` k=1..6 đọc
chính 6 ô đó (`m_GeneratorParam.nGeneratorLevel[k-1]`).

⇒ Muốn tạo hộp có `tbParam={1,10080,60,0,0,0}` như bản Linux thì phải gọi
`AddItem(6,1,4865, 1,0,0, 1,10080,60,0,0,0, nStack, nEnChance, nPoint)` — **15 đối số**.
**Chưa nối**: khâu rơi đồ của boss cuối đi qua `tbAwardTemplet:Give` vốn bỏ qua `tbParam`,
nên hộp rơi ra sẽ có param = 0 và script trả về "chưa thiết lập loại mặt nạ".
Phải xử ở bước nối phần thưởng (mục 7.3).


---

## 9. PHẢN BIỆN — `v20_phanbien.py`

Soát 8 mặt trên 33 tệp, mỗi mặt đối chiếu **nguồn thật** chứ không tin trí nhớ:

| Mặt | Kết quả |
|---|---|
| A. `Include` | mọi đường dẫn đều tồn tại |
| B. Hàm gọi ra | đều có ở engine hoặc trong cây script (trừ 8 mục đã soát, xem dưới) |
| C. Vật phẩm | **45 chỗ** dùng mã, tất cả có trong bảng item JX1 |
| D. NPC | **34 template**, tất cả đã khai |
| E. Bản đồ | **26 bản đồ**, đều đã khai + đã nạp + có thư mục dữ liệu trên đĩa |
| F. Task id | không đụng tính năng khác |
| G. Cú pháp + mã hoá | **0 lỗi cú pháp**, FFFD=0, không dòng LF lẻ |
| H. Bể phó bản | cấu hình 15 phòng = 15 bản đồ trong bể (khớp) |

**Ba lỗi thật mà phản biện bắt được và đã sửa:**

1. `yandituteng.lua` Include `\script\lib\composeclass.lua` — JX1 không có.
   → `v21_port_composeclass.py` port từ bản Linux. **Bỏ `Include string.lua`** vì tệp đó
   định nghĩa `replace`/`split`/`join`/`trim` mà **JX1 đã có trong `lib\common.lua`** —
   chép cả tệp sẽ che mất bản của JX1. Chỉ mang theo đúng 1 hàm nó cần (`transferDigit2CnNum`).
2. `yandiduihuan.lua:128` gọi `GetItemBindState` — JX1 không có.
   → `v22_compat.py`: JX1 có `GetPlayerItemIsLock`, đã đọc mã engine
   (`LuaGetPlayerItemIsLock` → `Item[idx].GetPlayerItemLock()`) — **đọc đúng trường mà
   `SetItemBindState` ghi**, ánh xạ 1:1 chứ không xấp xỉ.
3. `mission.lua:94` `GetMSPlayerCount(MATCH, 0)` — bẫy #3 (xem §7).

**Tám mục đã soát tận nơi và có ý chấp nhận:**

- 6 hàm xúc xắc (`ApplyItemDice`…) — bước 7 chưa làm.
- `BuildATeam` (`readymap/include.lua:357`) — thuộc luồng báo danh **cũ**, đã chứng minh
  chết ở cả bản Linux (không ai gọi `OpenMission(51)`; `YDBZ_ready_missions()` ở
  `trigger_include.lua:10` không có người gọi).
- `YDBZ_proceed` (`npc.lua:213`) — **giống hệt bản Linux**: `local proceed = item[...]`
  nhưng lại gọi `YDBZ_proceed(item)`. Hàm này **không định nghĩa ở cả hai bên**, và cả
  26 mục NPC đều để `nil` ở ô PROCEED nên `type(nil) ~= "function"` ⇒ **nhánh chết**.

**Một mục ngoài phạm vi**: `basemission\lib.lua:7` Include `\script\lib\coordinate.lua`
(không có). Tệp `lib.lua` có sẵn từ đợt 23/08 và **không ai Include nó**; ngoài ra JX1 đã
có sẵn toàn bộ hàm của `coordinate.lua` ở `missions	ong\collectgoods
pcpoint.lua` nên
port thêm sẽ chỉ gây trùng tên. Để nguyên.

---

## 7. CÒN LẠI PHẢI LÀM

### 7.0 Bước 6a ĐÃ XONG

`v09_port_kichban.py` đã chép 26 tệp (3.619 dòng) sang cả cây máy chủ lẫn gương git,
đổi **47 chỗ mã vật phẩm** theo bảng §4. Kiểm: `syncheck` **26/26 không lỗi cú pháp**
(7 tệp báo lỗi *lúc chạy* vì thiếu hàm engine khi giả lập — đã đối chứng: lỗi cú pháp
thật in `LOI CU PHAP`, 7 tệp này không có); mã hoá 3.202 byte TCVN3, **FFFD=0, LF-đơn=0**.

Bộ vá in ra **danh sách chính xác chỗ còn phải sửa tay**:

| Số chỗ | Việc |
|---|---|
| **71** | `tbReady` — lớp điều phối dùng chung state (xem 7.1) |
| 8 | `IncludeLib` — kiểm từng thư viện JX1 có chưa |
| 7 | bẫy #3 `GetMSPlayerCount(id, nhóm)` — phải bỏ tham số nhóm |
| 6 | hệ xúc xắc (`saizi.lua`) — bước 7 |
| 3 | `curpack`/`usepack` — JX1 không cần |
| 2 | `unpack()` — Lua 4.0 không có |
| 2 | bẫy #5 `AddNpc` — đổi sang `AddNpcEx` |
| 1 | bẫy #4 `GetMissionV()==0` → `IsMission()` |
| 1 | `SubWorldName` — đã bù trong `dungeon.lua` |

### 7.1 Lớp điều phối — vướng mắc cốt lõi

JX1 cho **mỗi tệp Lua một state riêng** (`Include` = `lua_dofile` vào state của tệp gọi, `ScriptFuns.cpp:1969`).
Viêm Đế viết theo giả định **một state chung**:

| Biến dùng chung | Định nghĩa ở | Nhưng cũng bị đọc/ghi ở |
|---|---|---|
| `tbReady.tbPlayer` (hàng đợi báo danh) | `readymap\ready.lua` | `npc\yandibaozang_main.lua`, `readymap\readymap.lua` |
| `tbReady.nMapCount` / `nState` | `ready.lua` | `readymap.lua:68`, `yandibaozang_main.lua:24` |
| `DungeonList` | `basemission\dungeon.lua` | mọi tệp Include nó |

⇒ **Lớp điều phối phải viết lại**, không chép được. Hướng đã chọn (đã áp trong `dungeon.lua`):
mọi trạng thái dùng chung nằm ở **biến mission của chính bản đồ** (`SubWorld[i].m_MissionArray`,
chỉ số 1–99, `KMission.h:122`), đọc/ghi qua `DG_GetVar`/`DG_SetVar`.

Riêng `tbReady.tbPlayer` (tên người chơi → id tổ đội) nên **suy ra từ chính người chơi**:
`YDBZ_TEAMS_TASKID = 1851` vốn đã lưu id tổ đội trên từng người, và người báo danh đều đứng
trong bể phòng chờ ⇒ quét người trong bể là dựng lại được danh sách, khỏi cần bảng Lua chung.

### 7.1b Lớp dùng chung ĐÃ CÓ — còn phải đấu nối

`readymap\ydbz_share.lua` (175 dòng, syncheck sạch) đã cung cấp sẵn:

| Bản Linux | Hàm thay thế ở JX1 |
|---|---|
| `tbReady.nState` | `YDBZ_GetRV(YDBZ_RV_STATE)` / `YDBZ_SetRV(...)` |
| `tbReady.nMaxPartyId` · `:NewPartyId()` | `YDBZ_IdToDoiMoi()` |
| `tbReady.nCurTime` | `YDBZ_GetRV(YDBZ_RV_CURTIME)` |
| `tbReady.tbPlayer` · `:AddPartyMember` | `YDBZ_ThemNguoi(nPlayerIndex, nPartyId)` |
| `tbReady:DelPartyMember` | `YDBZ_BoNguoi(nPlayerIndex)` |
| `for szName,nPartyId in self.tbPlayer` | `YDBZ_LayDanhSach()` → `{[id]={chỉ số người chơi,...}}` |
| `tbReady.nMapCount` | `YDBZ_DemPhong()` |
| `tbReady:BroadCast` | `YDBZ_MoiPhong(szMsg)` |

Kho chứa: biến mission 90/91/92 trên **bản đồ neo 852** (khuôn, không ai vào) +
mission `YDBZ_MISSION_READY = 51` giữ danh sách người báo danh qua
`AddMSPlayer`/`GetNextPlayer` — **đúng cơ chế mà chính bản gốc dùng cho pha đánh trận**
(`ready.lua:214`).

**Việc kế tiếp (6b-2)**: sửa 3 tệp cho gọi lớp này thay vì `tbReady`:
`readymap
eady.lua` · `readymap
eadymap.lua` · `npc\yandibaozang_main.lua`.
Kèm theo: 7 chỗ bẫy #3, 2 chỗ bẫy #5, 2 chỗ `unpack()`, 1 chỗ bẫy #4, 8 chỗ `IncludeLib`.

### 7.2 Hệ xúc xắc chia đồ

6 hàm `ApplyItemDice` / `AddDiceItemInfo` / `RollItem` / `GetItemDicePlayerList` /
`GetItemDiceRollInfo` / `GetItemDiceItemInfo` + cửa sổ đổ xúc xắc phía client + gói tin.
JX1 **không có gì tái dùng được** (`BauCua.cpp` là trò bầu cua, khác hẳn).
Chỉ được gọi ở **một chỗ**: `npc_death.lua:34`.

### 7.3 Việc còn lại khác

- Task **200 đã bị dùng** (`battlehead.lua`, `hoatdong_admin.lua`, `bwhead.lua`) → phải đổi. Task **2618 trống**.
- Kiểm chéo task 1851–1855 (`head.lua:36-40`) xem JX1 có ai dùng chưa.
- Nối lịch chạy + khoá cấu hình vào `script\header\cauhinh_hoatdong.lua` (mặc định = **đúng số bản Linux**).
- Thêm mọi tệp mới vào `gen_hd3_reload.py`.
- `check_encoding.py` + `syncheck.exe` trên mọi tệp đã đụng.


---

## 10. GIAO DIỆN CLIENT — ĐÃ TÌM RA (đính chính 26/08)

**Tôi đã kết luận sai một lần**: báo rằng "không nguồn nào có bố cục cửa sổ xúc xắc".
Sai vì tôi tìm theo từ khoá `dice / roll / touzi / saizi`, mà bản CN gọi xúc xắc là
**色子 (sắc tử)** chứ không phải 骰子. Chủ game bảo soi lại `D:\ServerLinux` thì thấy ngay.

### 10.1 Cửa sổ đổ xúc xắc — CÓ ĐỦ

`D:\ServerLinux\Patch\ui\ui3_1024\` (và bản `ui3_800`):

| Tệp | Nội dung |
|---|---|
| `投色子.ini` | một khung 312×83: `[Main]` · `[TimeInfo]` "Thời gian còn lại %d giây" · `[BtnNeed]` **"Cần"** · `[BtnGreed]` **"Tham dự nhận"** · `[BtnClose]` **"Hủy bỏ nhận"** · `[objItem]` ô vật phẩm 38×38 |
| `投色子-公用.ini` | khung chứa **4 ô** (Img0..Img3) xếp dọc, mỗi ô 312×83 |

Chữ trong ini **đã dịch sẵn tiếng Việt** (bản VNG) — dùng thẳng được.

⚠️ **Còn thiếu đúng 2 tệp ảnh** mà ini trỏ tới, không có trên đĩa ở bất kỳ cây nào
(chúng nằm trong pak chính của client CN mà ta chưa có):

- `\Spr\Ui3\DICEITEM\NPC对话框_中.spr` — khung hộp thoại
- `\Spr\Ui3\DICEITEM\3字按钮_a.spr` — nút 3 chữ, 63×19

Ứng viên thay thế cùng họ có sẵn: `Patch\spr\ui3\NPC对话条\滚动选择对话条不透明.spr`
(105 KB) và `client JX1\Spr\Ui3\NPC对话条\滚动选择对话条-小3.spr` (132 KB).

### 10.2 Giao diện LÒ RÈN — CÓ ĐỦ (cho 2 tính năng ép đồ làm sau)

Cùng thư mục đó có **6 tệp bố cục** của hệ lò rèn, đúng 6 thẻ mà
`UiCompoundItem.cpp` của JX1 đang thiếu:

| Tệp bản Linux | Thẻ |
|---|---|
| `铸造装备界面.ini` | khung chính |
| `铸造_矿石合成.ini` | tinh luyện Huyền Tinh |
| `铸造_矿石提取.ini` | chiết xuất |
| `铸造_装备打造.ini` | **chế tạo trang bị TÍM** |
| `铸造_装备镶嵌.ini` | khảm nạm |
| `铸造_黄金图谱.ini` | **Đồ phổ HOÀNG KIM** |

Ngoài ra `smeltsystem\smelt.ini`, `smeltsystem.ini`, `distill.ini` **đã có sẵn trong
pak client JX1**.

> 🔴 **BÀI HỌC**: khi tìm tài nguyên bản CN/VNG, phải tra **cả tên tiếng Trung**
> (色子 = xúc xắc, 铸造 = đúc/rèn, 图谱 = đồ phổ), không chỉ phiên âm tiếng Anh.
> `D:\ServerLinux\Patch\ui\` có **431 tệp .ini** — liệt kê bằng cách giải mã tên GBK.

### 10.3 Hai tệp ảnh xúc xắc — **ĐÃ TÌM RA, có sẵn trong client** (26/08)

> **Đính chính.** Bản trước của mục này kết luận hai tệp ảnh "không tồn tại ở
> cây nào". **Kết luận đó sai** — do công cụ dò của tôi hỏng, không phải do
> thiếu dữ liệu. Chủ game bảo tìm kỹ hơn vì từng thấy rồi; đúng vậy.

#### Lỗi của công cụ

`KPakList::FileNameToId` (`Sources/Engine/Src/KPakList.cpp:73`) duyệt tên tệp
qua `const char *ptr`. Trên MSVC **`char` là kiểu CÓ DẤU**, nên byte GBK
`0xB2` vào phép nhân là **−78**, không phải 178:

```c
id = (id + (++index) * (*ptr)) % 0x8000000b * 0xffffffef;
```

Công cụ của tôi dùng số **không dấu**. Với tên thuần ASCII thì không khác gì
nhau nên phép kiểm chứng 300/300 lúc đầu vẫn đạt — **nhưng phép kiểm đó chỉ
gồm đường dẫn ASCII**, còn mọi đường dẫn có chữ Trung đều băm ra sai.

#### Phép đối chứng lẽ ra phải làm từ đầu

Lấy các tệp **có thật trên đĩa** rồi hỏi ngược: uid của chúng có trong pak không?

| Nhóm tên tệp | Băm không dấu (cũ) | Băm có dấu (đúng) |
|---|---|---|
| ASCII | 1744/2319 | 1744/2319 |
| **GBK (chữ Trung)** | **0/714** | **714/714** |

0/714 là điều không thể xảy ra ngẫu nhiên — đó là dấu hiệu công cụ hỏng, và
tôi đã bỏ qua nó ở các lần dò trước. Lần dò đầu tiên tôi có chạy đối chứng
nhưng nó "đạt" **nhờ phép kiểm tra trên đĩa**, che mất việc phần pak trả về sai.

#### Kết quả sau khi sửa

Cả hai ảnh nằm ngay trong **`data\updatejx06.pak` của chính client đích**:

| Tệp | Kích thước | Nén |
|---|---|---|
| `\Spr\Ui3\DICEITEM\NPC对话框_中.spr` | 27.360 B | `0x20000000` (UCL, nhánh "Load Pak VNG") |
| `\Spr\Ui3\DICEITEM\3字按钮_a.spr` | 4.613 B | `0x20000000` |

Ngoài client đích, hai ảnh còn có trong **20 pak khác** trên các ổ E: và J:
(mọi bản `PATCHFULL_*`, `SrcKinox`, `jx1m_cdn`…). Game đọc thẳng từ pak lúc
chạy nên **không cần rút ảnh ra**, không cần chép gì thêm.

#### Đã sửa lại phần đã làm

`v23_port_uidice.py` từng nắn 5 chỗ `Image=` sang một ảnh khác để "chữa cháy".
Việc đó **không còn cần** và đã hoàn tác bằng **`v23b_uidice_nguyenvan.py`**:

- Chép **nguyên văn từng byte** `投色子.ini` (1.454 B) và `投色子-公用.ini`
  (855 B) từ `D:\ServerLinux\Patch\ui\ui3_1024` sang `client\Ui\Ui3`.
- Bản đã bị nắn giữ lại ở `*.ini.v23_da_nan` phòng khi cần đối chiếu.
- Kiểm lại: 2 tệp khớp nguyên văn bản Linux; **cả 3 đường dẫn ảnh đều phân
  giải được** trong pak client.

Client chỉ có một thư mục `Ui\Ui3` nên bản `ui3_1024` là bản đúng để chép.

#### Ảnh hưởng sang tính năng 2 và 3 (ép đồ tím / ép hoàng kim môn phái)

Đo lại toàn bộ `ui3_1024` bằng hàm băm đã sửa: **981 lượt khai ảnh, chỉ 90
đường dẫn riêng là thiếu** (con số "508/617" ở bản trước cũng sai).

Sáu giao diện lò rèn cần cho hai tính năng ép đồ **thiếu đúng một ảnh**:

| Giao diện | Ảnh riêng | Thiếu |
|---|---|---|
| `铸造装备界面.ini` | 7 | `\Spr\ctc_ui\chetao_frame.spr` |
| `铸造_矿石合成.ini` | 5 | 0 |
| `铸造_矿石提取.ini` | 5 | 0 |
| `铸造_装备打造.ini` | 5 | 0 |
| `铸造_装备镶嵌.ini` | 4 | 0 |
| `铸造_黄金图谱.ini` | 4 | 0 |

Công cụ dò đã dùng lại được: **`ReverseTools/viemde/pak_id.py`** (bản dịch
đúng từng dòng của `FileNameToId`, kèm ghi chú về bẫy char có dấu).


## 11. Lưới giao thức — kiểm bằng số trước khi chèn (26/08)

Chủ game dặn: *"chèn giao thức phải đồng bộ theo thứ tự tránh việc lỗi"*. Đây là
loại lỗi **không lộ ra lúc biên dịch** mà làm rớt người chơi lúc chạy, nên tôi
kiểm bằng công cụ tất định chứ không đọc mắt.

### 11.1 Vì sao lệch một ô là hỏng

`g_nProtocolSize[MAX_PROTOCOL_NUM]` (`Sources/Core/Src/KProtocol.cpp:9`) là **khối
khởi tạo phẳng**, tra cứu bằng:

| Bản | Công thức |
|---|---|
| client | `g_nProtocolSize[s2c_id - s2c_clientbegin - 1]` |
| server | `g_nProtocolSize[c2s_id - c2s_gameserverbegin - 1]` |

Thứ tự phần tử phải khớp **1:1** với thứ tự thành viên enum. Lệch một ô là hai đầu
hiểu sai độ dài gói.

### 11.2 Công cụ kiểm

`ReverseTools/viemde/kiem_luoi_giaothuc.py` — phân tích enum (tính giá trị thật,
**bỏ thành viên bị chú thích** vì chúng không chiếm giá trị), phân tích khối khởi
tạo (một dòng như `-1, //sizeof(X), // s2c_y` chỉ là **một** phần tử), rồi so
**tên suy ra từ vị trí** với **tên trong chú thích cuối dòng**.

```bash
python D:\GAMEDEVNEW\ReverseTools\viemde\kiem_luoi_giaothuc.py
```

### 11.3 Kết quả: lưới ĐANG ĐÚNG

```
enum c2s: 139 thành viên | enum s2c: 181 thành viên
khối khởi tạo: nửa client 150 phần tử, nửa server 107 phần tử
nửa client: khớp 145, lệch 5     nửa server: khớp 101, lệch 3
```

**8 chỗ "lệch" đều là chú thích cũ / gõ sai, KHÔNG phải lệch lưới.** Bằng chứng:

1. Mọi tên xuất hiện trong chú thích lệch đều **không tồn tại trong enum**:
   `s2c_playershowui`, `s2c_chatfriendinline`, `s2c_gameblechangestate`,
   `s2c_opendataubox1`, `c2s_sellitem`, `c2s_buyitem`, `c2s_logicLogin`.
2. Nếu lệch thật thì **mọi** ô phía sau cũng lệch theo. Ở đây hai ô kề bên vẫn
   khớp — ví dụ quanh ô 57:

   | ô | giá trị | suy từ vị trí | chú thích |
   |---|---|---|---|
   | 56 | 121 | `s2c_playerswitchequip` | `s2c_playerswitchequip` ✔ |
   | 57 | 122 | `s2c_scriptaction` | `s2c_playershowui` ✘ (tên không có thật) |
   | 58 | 123 | `s2c_chatapplyaddfriend` | `s2c_chatapplyaddfriend` ✔ |

### 11.4 Một ô THỪA ở đuôi nửa client — phải xử lý

| ô | giá trị | nội dung | ghi chú |
|---|---|---|---|
| 148 | 213 | `sizeof(ITEM_REMOVE_SYNC)` | `s2c_removeallitem` ✔ |
| **149** | **214** | **`-1`** | ghi `s2c_dynamic_structure` — **enum bị chú thích** (`KProtocolDef.h:205`) |

Hôm nay vô hại (không giao thức nào mang giá trị 214). Nhưng giao thức s2c mới sẽ
nhận đúng giá trị **214**, nên phần tử của nó phải chèn **vào ô 149**, tức **trước**
dòng `-1` đó; dòng `-1` trôi xuống ô 150 (giá trị 215, vẫn bỏ không).

### 11.5 Số chốt

| | giá trị cuối hiện có | giao thức mới | ô trong bảng |
|---|---|---|---|
| c2s | `c2s_baucua = 171` (`KProtocolDef.h:363`) | `c2s_diceitem = 172` | thêm ô 107, sau `sizeof(BAUCUA_DATA)` (`KProtocol.cpp:282`), trước `#endif` |
| s2c | `s2c_removeallitem = 213` (`KProtocolDef.h:204`) | `s2c_diceitem = 214` | chèn ô 149, sau `KProtocol.cpp:167`, **trước** dòng `-1` ở 168 |

Sức chứa: `MAX_PROTOCOL_NUM = 200`; chỉ số cao nhất thực dùng là **188**
(`c2s_extendtong/s2c_extendtong = 253` → `253-64-1`). Còn thừa — **không phải nới**.

`ProcessFunc` đánh chỉ số **thẳng bằng mã giao thức**, khai báo cỡ `[s2c_end]` /
`[c2s_end]` (=254) nên giá trị 172 và 214 nằm trong tầm — **không phải nới**, chỉ
cần thêm dòng đăng ký.

### 11.6 🔴 BẪY: có HAI tệp `KProtocol.h`

| Tệp | Cỡ | Sửa lần cuối |
|---|---|---|
| `Sources/Core/Src/KProtocol.h` | 78.779 B | 17/08 |
| `Headers/KProtocol.h` | 58.367 B | 14/08 |

Hai bản **khác nhau** (bản `Headers` thiếu `OPEN_TREMBLEITEM`…). Dự án nào ăn bản nào
phụ thuộc **thứ tự** `AdditionalIncludeDirectories`:

| Dự án | Thứ tự | Ăn bản |
|---|---|---|
| `Core` | `..\engine\src; ..\..\Headers; …; src` — nhưng `.cpp` nằm ngay trong `Src` nên **thư mục tệp gọi thắng** | `Core/Src` |
| `S3Client` | `../Engine/src; **../Core/src**; ../Engine/include; ../../Headers` | `Core/Src` |
| `Bishop` | `…; **..\..\..\Headers**; …; ..\..\Core\src` | `Headers` |
| `GameServer` | `…; ..\..\..\Headers\; …` (không có `Core\src`) | `Headers` |

⇒ **Cấu trúc gói xúc xắc phải đặt vào `Sources/Core/Src/KProtocol.h`** (bản mà
CoreServer.dll và Game.exe cùng biên dịch). Bản `Headers/KProtocol.h` là **rác
tồn đọng** với hai dự án đó.

`KProtocolDef.h` chỉ có **một** bản (`Headers/`) — nên phần nhạy giá trị (enum) tự
động đồng nhất cho mọi dự án. Đó là điều may.

> ⚠️ Ghi nhận riêng, ngoài phạm vi Viêm Đế: Bishop và GameServer đang biên dịch một
> `KProtocol.h` khác với Core/client. Nếu hai dự án đó có đọc cấu trúc gói trong game
> thì đây là lệch tiềm ẩn sẵn có. Chưa kiểm sâu, chưa đụng vào.

### 11.7 Danh sách kiểm khi chèn

1. `Headers/KProtocolDef.h` — thêm `c2s_diceitem` ngay sau `c2s_baucua`;
   thêm `s2c_diceitem` ngay sau `s2c_removeallitem`, **trước** dòng chú thích
   `// s2c_dynamic_structure`.
2. `Sources/Core/Src/KProtocol.h` — thêm 2 cấu trúc gói.
3. `Sources/Core/Src/KProtocol.cpp` — thêm 2 ô kích thước **đúng vị trí thứ tự**
   (mục 11.5), rồi **chạy lại `kiem_luoi_giaothuc.py`** để xác nhận vẫn khớp.
4. `Sources/Core/Src/KProtocolProcess.h` — khai báo hàm xử lý.
5. `Sources/Core/Src/KProtocolProcess.cpp` — đăng ký `ProcessFunc[c2s_diceitem]`
   ngay sau `ProcessFunc[c2s_baucua]` (`:355`) + viết thân hàm; phía client thêm
   `ProcessFunc[s2c_diceitem]`.
6. Build **cả hai**: `Server Release - US|x64` và `Client Release - US|Win32`.
   Thả **cùng lúc** `CoreServer.dll` + `CoreClient.dll` + `Game.exe` — hai đầu phải
   khớp giao thức.
### 11.8 ĐÃ CHÈN — `v24_chen_giaothuc_xucxac.py`

Bản vá giữ **CRLF** và **tab** nguyên vẹn, sao lưu `.truoc_xucxac_2608`, và
**khẳng định từng mỏ neo là duy nhất trước khi sửa bất cứ gì**.

| Tệp | Thay đổi |
|---|---|
| `Headers/KProtocolDef.h` | `s2c_diceitem` sau `s2c_removeallitem` · `c2s_diceitem` sau `c2s_baucua` |
| `Sources/Core/Src/KProtocol.h` | `DICE_ITEM_SYNC` + `DICE_CHOICE_DATA`, đặt sau `VIEW_ITEM_SYNC` |
| `Sources/Core/Src/KProtocol.cpp` | **thay** dòng chết `-1 //s2c_dynamic_structure` bằng ô `s2c_diceitem`; thêm ô `c2s_diceitem` sau `c2s_baucua` |
| `Sources/Core/Src/KProtocolProcess.h` | khai báo `s2cDiceItem`, `c2sDiceItem` |
| `Sources/Core/Src/KProtocolProcess.cpp` | đăng ký `ProcessFunc[s2c_diceitem]`, `ProcessFunc[c2s_diceitem]` |

**Vì sao THAY chứ không nối thêm dòng `-1`**: nó đang chiếm đúng ô 149 — ô mà
`s2c_diceitem` (=214) sẽ tra tới. Nối thêm vào cuối thì
`g_nProtocolSize[149]` vẫn là `-1` (bị hiểu là **gói độ dài thay đổi**) còn ô mới
rơi vào 150 và **không bao giờ được đọc**. Dòng đó chết cả ba nơi nên xoá là an toàn:

- enum `s2c_dynamic_structure` bị chú thích — `KProtocolDef.h:205`
- trình xử lý bị chú thích — `KProtocolProcess.cpp:234`
- không mã nào mang giá trị 214 trước thay đổi này

### 11.9 Bằng chứng sau khi chèn

```
nửa client 150 phần tử, nửa server 108 phần tử
[149] gt=214  sizeof(DICE_ITEM_SYNC)     // s2c_diceitem
[107] gt=172  sizeof(DICE_CHOICE_DATA)   // c2s_diceitem
enum s2c_diceitem = 214 -> chỉ số bảng 149   ✔
enum c2s_diceitem = 172 -> chỉ số bảng 107   ✔
```

Số chỗ "lệch" nửa client giảm **5 → 4**: đúng bằng việc ô chết biến mất. Bốn chỗ
còn lại (và 3 chỗ bên server) là chú thích cũ đã chứng minh vô hại ở mục 11.3.

### 11.10 Nguồn gốc nhị phân — đã truy đến cùng

Câu hỏi sống còn: cây `D:\GAMEDEVNEW` có đúng là cây sinh ra bản đang chạy không?
(Bộ nhớ cảnh báo hai cây `D:` và `E:` lệch nhau, và **có** cây nguồn nằm ngay dưới
`E:\...\TESTLOFFF_ONLINE`.)

| Tệp đang chạy | Trùng byte với |
|---|---|
| `bin\server\CoreServer.dll` (18.062.336 B) | `D:\GAMEDEVNEW\Sources\Core\x64\ServerRelease\CoreServer.dll` |
| `bin\client\CoreClient.dll` (2.381.824 B) | `D:\GAMEDEVNEW\Sources\Core\ClientRelease\CoreClient.dll` |
| `bin\client\Game.exe` (1.275.904 B) | `D:\GAMEDEVNEW\Sources\S3Client\Release\Game.exe` |

Hai cây nguồn dưới `E:` là **bản cũ** (`SOURCESUPDATE_KINHMACH_ONLTEST0608` build
18/08; `..._OPENTEST` build 2025). Đường dẫn PDB nhúng trong cả ba nhị phân đều
trỏ về `D:\GAMEDEVNEW`.

Kiểm sâu thêm: đọc **thẳng bảng `g_nProtocolSize` ra khỏi `CoreServer.dll` đang
chạy** (ký hiệu `?g_nProtocolSize@@3PAHA`, RVA `0x69a550` theo
`x64\ServerRelease\CoreServer.map`): đúng **107 ô khác 0**, ô cuối cùng khác 0 là
chỉ số **106 = 14** = `sizeof(BAUCUA_DATA)` với `#pragma pack(1)`. Trùng khớp
hoàn toàn với mã nguồn trước khi sửa.

> ⚠️ Máy chủ **đang chạy** (`GameServer.exe`) với đúng bản DLL đó. Giao thức mới chỉ
> có hiệu lực sau khi build lại và thả **đồng thời** `CoreServer.dll` +
> `CoreClient.dll` + `Game.exe`, rồi khởi động lại — hai đầu phải cùng lưới.

### 11.11 Còn lại của bước 7

Trình xử lý mới **mới chỉ khai báo, chưa viết thân** — build bây giờ sẽ báo thiếu
ký hiệu. Còn phải:

1. Lớp `KItemDice` + `KItemDiceSet` (phiên, đồng hồ đếm ngược, chọn người thắng).
2. 8 hàm Lua (đặc tả `DACTA_HE_XUCXAC_DICEITEM.md`).
3. Thân `c2sDiceItem` (nhận lựa chọn, gieo điểm) và `s2cDiceItem` (client mở ô).
4. Lớp cửa sổ client gắn `投色子-公用.ini` — khung 312×340 chứa **4 ô** xếp dọc
   (`Img3` y=0, `Img2` y=86, `Img1` y=172, `Img0` y=259), mỗi ô 312×83 gồm ô vật
   phẩm 38×38 tại (16,16), đồng hồ `Thời gian còn lại %d giây`, và 3 nút 63×19:
   `Cần` / `Tham dự nhận` / `Hủy bỏ nhận`.
### 11.12 Hai lỗi phải sửa sau khi chèn — và bài học

#### (a) Thứ tự khai báo trong `.h` không soi gương `.cpp` — chủ game bắt

`v24` neo `s2cDiceItem` vào `s2cSyncBauCuaResult` (chỗ vừa grep ra) thay vì soi
theo thứ tự đăng ký:

| `KProtocolProcess.h` (SAI) | `KProtocolProcess.cpp` (đăng ký = chuẩn) |
|---|---|
| 168 `s2cSyncBauCuaResult` | 230 `s2c_syncbaucuaresult` |
| **169 `s2cDiceItem`** ← sai | 231 `s2c_syncbaucuainfo` |
| 170 `s2cPlayerSync` | 232 `s2c_playersync` |
| 171 `s2cRemoveAllItem` | 233 `s2c_removeallitem` |
| | **234 `s2c_diceitem`** ← phải ở cuối |

Đã sửa bằng `v26_sua_thutu_khaibao.py`. Phía c2s thì `v24` làm đúng sẵn.

> 🔑 **Bài học**: tìm trình xử lý được **đăng ký ngay trước** cái của mình trong
> `.cpp`, rồi khai báo cái của mình **ngay sau khai báo của chính nó** trong `.h`.
> Đừng neo vào hàm mình vừa tình cờ grep ra.

**Công cụ**: `ReverseTools/viemde/kiem_thutu_handler.py` — đối chiếu toàn bộ.

```
s2c: 145 mục đăng ký | 69 sai thứ tự | 0 thiếu khai báo
c2s:  98 mục đăng ký |  0 sai thứ tự | 0 thiếu khai báo
.cpp:234 s2c_diceitem -> s2cDiceItem  .h:171   ✔
.cpp:357 c2s_diceitem -> c2sDiceItem  .h:281   ✔
```

69 chỗ sai bên s2c là **nợ cũ** (khối chat/friend/trade khai báo quanh `.h:80-97`
nhưng đăng ký muộn hơn). Chứng minh bản vá vô can bằng cách chạy bộ kiểm trên
**bản sao lưu**:

| | trước bản vá | sau bản vá |
|---|---|---|
| s2c | 144 mục / 69 sai | 145 mục / **69** sai |
| c2s | 97 mục / 0 sai | 98 mục / **0** sai |

#### (b) Nhúng `SViewSellItemInfo` vào gói — đúng cái bẫy lệch độ dài

`v24` cho `DICE_ITEM_SYNC` nhúng `SViewSellItemInfo`. Phản biện phát hiện điều đó
**không an toàn**, và tôi đã tự kiểm lại:

- Hai tệp `KProtocol.h` **dùng chung include guard `KPROTOCOL_H`** ⇒ trong một đơn
  vị biên dịch, bản nào vào trước thì bản kia bị **nuốt im lặng**.
- Ba tệp client nạp bản `Headers` bằng **đường dẫn tường minh** (nên thứ tự
  `AdditionalIncludeDirectories` không cứu được):
  `S3Client\NetConnect\NetConnectAgent.h:8`, `S3Client\Ui\UiCase\UiItem.cpp:29`,
  `S3Client\Ui\UiCase\UiPlayerBar.cpp:39`.
- `SViewSellItemInfo` **lệch thật 44 byte**: bản `Headers` ghi cứng
  `m_btMagicLevel[6]` và **thiếu** `m_nNature`; bản `Core\Src` dùng
  `[MAX_ITEM_MAGICLEVEL]` = `MAX_ITEM_MAGICATTRIB` × 2 = **16** và có `m_nNature`.

⇒ `sizeof(DICE_ITEM_SYNC)` sẽ **khác nhau giữa các đơn vị biên dịch** — đúng kiểu
lỗi lệch độ dài gói mà chủ game dặn phải tránh.

**Đã sửa** (`v25_tach_header_xucxac.py`): tách sang **`Headers/KDiceProtocol.h`**,
guard riêng `KDICEPROTOCOL_H`, bản mô tả vật phẩm **tự chứa** (`DICE_ITEM_DESC`,
không tham chiếu `SViewSellItemInfo`), rồi `#include` từ **cả hai** bản
`KProtocol.h` — nạp hai lần vẫn an toàn nhờ guard riêng. Chỉ dùng
`MAX_ITEM_MAGICLEVEL`, an toàn vì cả hai bản đều nạp cùng một
`Sources\Core\src\GameDataDef.h`.

> ⚠️ Ghi nhận ngoài phạm vi: `PLAYER_SYNC` ở bản `Headers` là **struct rỗng**
> (`sizeof` = 1) trong khi bản `Core\Src` có 35 dòng. Lệch có sẵn từ trước, chưa đụng.

### 11.13 Thả hàng — hai điều phải nhớ

1. **Chép sau build chỉ tới `D:\GAMEDEVNEW\bin\{client,server}`**, KHÔNG tới
   `E:\...\TESTLOFFF_ONLINE\bin`. Thả sang máy chủ thật là **bước tay**.
2. `KPROTOCOL_VERSION` (`Headers/KProtocolDef.h:8` = `20240430`) được Bishop /
   StartGameSV so lúc **đăng nhập**. Client cũ vẫn đăng nhập được, nhưng khi nhận
   `s2c_diceitem` (=214) nó tra `g_nProtocolSize[149]` = `-1` cũ ⇒ hiểu là gói
   **biến độ dài** ⇒ đọc rác. Muốn chặn triệt để thì **tăng số này** để client cũ
   bị từ chối ngay từ đăng nhập — **chờ chủ game quyết**, vì làm vậy là bắt mọi
   người chơi phải cập nhật client.
## 12. Hệ xúc xắc — phía máy chủ ĐÃ XONG (26/08)

Đặc tả: [`DACTA_HE_XUCXAC_DICEITEM.md`](DACTA_HE_XUCXAC_DICEITEM.md).
**Chưa build, chưa gắn vào.**

### 12.1 Tệp mới

| Tệp | Nội dung |
|---|---|
| `Headers/KDiceProtocol.h` | `DICE_ITEM_SYNC`, `DICE_CHOICE_DATA`, `DICE_ACTION`, `DICE_CHOICE` |
| `Sources/Core/Src/KItemDice.h` | lớp `KItemDice` + `KItemDiceSet`, khai báo 8 hàm Lua |
| `Sources/Core/Src/KItemDice.cpp` | 700 dòng: vòng đời phiên + 8 hàm Lua |

### 12.2 Tệp đã sửa

| Tệp | Sửa gì | Bản vá |
|---|---|---|
| `Headers/KProtocolDef.h` | `c2s_diceitem`=172, `s2c_diceitem`=214 | v24 |
| `Sources/Core/Src/KProtocol.cpp` | 2 ô kích thước đúng vị trí | v24 |
| `Sources/Core/Src/KProtocol.h` + `Headers/KProtocol.h` | nạp `KDiceProtocol.h` | v25 |
| `Sources/Core/Src/KProtocolProcess.h` | 2 khai báo, **đúng thứ tự `.cpp`** | v24 + v26 |
| `Sources/Core/Src/KProtocolProcess.cpp` | 2 đăng ký + **2 thân hàm** + include | v24 + v28 + v29 |
| `Sources/Core/Src/ScriptFuns.cpp` | 8 extern + 8 dòng đăng ký (khối `#ifdef _SERVER`) | v27 |
| `Sources/Core/Src/CoreServerShell.cpp` | `#include` + `KItemDice_Breathe()` | v27 |
| `Sources/Core/Src/CoreShell.h` | `GDCNI_DICE_ITEM` **ở cuối enum** | v29 + v30 |
| `Sources/Core/Core.vcxproj` | thêm `KItemDice.cpp/.h`, loại khỏi cấu hình Client | v27 |

Tất cả đều có bản lưu `.truoc_xucxac_2608`.

### 12.3 Ba quyết định kỹ thuật đáng chú ý

**(a) `ChatItem` làm bản mô tả vật phẩm.** Ban đầu tôi định nhúng
`SViewSellItemInfo` — sai, vì hai bản `KProtocol.h` định nghĩa nó **lệch 44 byte**
(`[6]` vs `[MAX_ITEM_MAGICLEVEL]`=16, và thiếu `m_nNature`). `ChatItem` nằm ở
`GameDataDef.h:613` — tệp mà **cả hai** bản đều nạp — nên bố cục đồng nhất mọi nơi.
Thêm nữa client **đã có sẵn** đường dựng lại vật phẩm từ nó: `CoreShell.cpp:2226`
(`GDI_ITEM_CHAT`) gọi `ItemSet.Add`/`AddGoldItem`/`AddItemSet2` rồi vẽ như vật phẩm
thường, chú giải tự động.

**(b) `GDCNI_DICE_ITEM` đặt CUỐI enum.** Chèn giữa sẽ **dịch giá trị** của
`GDCNI_EXIT_GAME`, `GDCNI_UI_ACT`, `GDCNI_TONG_JX2`, `GDCNI_TASK_VALUE_UPDATE`.
Enum này do **cả** `CoreClient.dll` lẫn `Game.exe` biên dịch — build lệch một bên là
gọi nhầm callback. Đặt cuối thì không giá trị cũ nào đổi.

**(c) Vòng đời vật phẩm — chỗ dễ rò nhất.** Vật phẩm sinh ở `AddDiceItemInfo` và
**chưa thuộc về ai** suốt phiên. `Finish()` xử đủ ba nhánh: có người thắng → vào
trang bị/tay; túi đầy **và** tay đang cầm → `ItemSet.Remove` + ghi nhật ký (khác
`LuaAddItem`: ở đây người chơi không chủ động xin nên làm rơi đồ của họ là sai);
không ai nhận → `ItemSet.Remove`.

### 12.4 Bẫy gặp phải khi thi công

| Bẫy | Chi tiết |
|---|---|
| **Trộn kiểu xuống dòng** | `ScriptFuns.cpp` là **LF** (15.516 dòng) trong khi `CoreServerShell.cpp`, `KProtocolProcess.cpp`, `CoreShell.h`, `Core.vcxproj` là **CRLF**. Bộ vá phải tự nhận kiểu từng tệp. |
| **Bộ bảo vệ quá cứng** | Phép kiểm "không được có dòng LF lẻ" báo động giả trên tệp LF. Sửa thành "kiểu **thiểu số** không được tăng". |
| **LF lẻ có sẵn** | `Headers/KProtocol.h` sẵn 3 dòng, `CoreShell.h` sẵn 1 dòng — phải cho phép giữ nguyên. |
| **PCH** | Core build với PCH "Use" qua `KCore.h` ⇒ `KCore.h` **phải là include đầu tiên**. |
| **BOT không có kết nối** | `m_nNetConnectIdx = -1` — chặn trước khi gửi gói. |

### 12.5 Còn lại — phía client

| Việc | Mẫu có sẵn để bắt chước |
|---|---|
| Lớp cửa sổ `KUiDice` (4 ô, mỗi ô 1 ô vật phẩm + đồng hồ + 3 nút) | `KUiTrembleItem` (mảng `KWndObjectBox m_ItemBox[8]` + bảng ánh xạ mục `.ini`) |
| Đếm ngược | `KUiTimeBox` — `Breathe()` đếm khung, **1 giây = 18 khung** |
| Dựng lại vật phẩm để vẽ | `KSellItem::GetData()` → `ItemSet.AddItemSet2` → `HoldObject(CGOG_PLAYERSELLITEM, ...)`; **bắt buộc `ItemSet.Remove` khi đóng**, không thì rò mảng `Item[]` |
| Chú giải | **tự động** — chỉ cần `HoldObject` đúng genre + id |
| `case GDCNI_DICE_ITEM` | `GameSpaceChangedNotify.cpp` (mẫu: `GDCNI_OPEN_TREMBLE_ITEM` :575) |
| `GOI_DICEITEM` + `SendClientDiceItem` | `SendClientBaucua` (`KProtocol.cpp:781`) + `case GOI_*` trong `CoreShell.cpp:18205` |
| Đăng ký cửa sổ | `UiShell.cpp` |
| `S3Client.vcxproj` | thêm tệp mới |

> 🔑 Phát hiện khi khảo sát: khoá `._WndType=` trong `.ini` **không hề được mã C++
> đọc** (grep toàn `Sources\S3Client` = 0 kết quả) — nó chỉ là siêu dữ liệu của công
> cụ vẽ. Ánh xạ mục `.ini` → đối tượng C++ là do lập trình viên **gõ tay tên mục**.

### 12.6 Trước khi build

Build sẽ **chưa chạy được** cho tới khi xong phía client (thiếu `case GDCNI_DICE_ITEM`
thì vẫn biên dịch được, nhưng cửa sổ không mở). Thứ tự đề nghị: xong client → build
`Server Release|x64` + `Client Release|Win32` → chạy phản biện → mới gắn vào.
### 12.7 Phản biện bắt được 5 lỗi thật — đã vá (`v31`)

Chạy một đợt khảo sát + phản biện độc lập trên chính mã vừa viết. Tôi **tự kiểm lại
từng điểm** trước khi sửa; cả 5 đều đúng.

**(1) `PlayerIndex` treo — nặng nhất.** `Finish()` truyền `nPlayerIndex = m_nWinner`,
mà khi **không ai nhận** thì `m_nWinner == 0` → `CallScript` bỏ qua cả khối đặt
`PlayerIndex` → kịch bản chạy với `PlayerIndex` **còn sót** từ lời gọi trước.
`saizi.lua:35` đọc `YDBZ_sdl_getTaskByte(YDBZ_ITEM_YANDILING,1)` **ngay trước** vòng
lặp, tức đọc nhầm dữ liệu người khác.

Vá: `Finish()` tự chọn người còn sống đầu tiên làm ngữ cảnh khi không có người thắng;
và `CallScript` **luôn** đặt `PlayerIndex` (đặt 0 khi thật sự không có ai) chứ không
bao giờ để treo.

**(2) Thiếu `SCRIPT_SUBWORLDINDEX`.** Mọi lối gọi kịch bản khác trong cây đều đặt
biến này (`KPlayer.cpp:6975`, `:7397`; `KMission.cpp:89`; `ScriptFuns.cpp:11287`).

**(3) Vứt giá trị trả của `CallFunction`.** Kịch bản nổ là hỏng **im lặng**, không
dấu vết trong nhật ký. Đã thêm cảnh báo `g_DebugLog`.

**(4) `_NAME_LEN` trong `KItemDice.h`.** Hằng đó chỉ có ở `KProtocol.h`; header chỉ
biên dịch được **nhờ PCH** kéo `KCore.h` vào trước. Đổi sang `MAX_DICE_NAME_LEN` của
chính `KDiceProtocol.h`.

**(5) `g_ItemDiceSet.Init()` không ai gọi.** Mã vẫn chạy vì biến toàn cục được
zero-init và `Alloc()` có chốt `if (m_nNextId <= 0) m_nNextId = 1;` — nhưng để nguyên
là dựa vào may mắn. Đã gọi tường minh cạnh `g_TeamSet.Init()` (`KCore.cpp`), kèm
`#include "KItemDice.h"`.

Thêm `#include "KNpc.h"` vào `KItemDice.cpp` vì `CallScript` giờ đọc
`Npc[...].m_SubWorldIndex`.

#### Một khẳng định của phản biện thì SAI — và sai đúng kiểu tôi từng sai

Nó báo *"`\Spr\Ui3\DICEITEM\*.spr` KHÔNG TỒN TẠI Ở ĐÂU TRONG CÂY"*, dẫn chứng
`find . -iname "*.spr" -ipath "*Ui3*"` = 0 kết quả.

`find` **chỉ thấy tệp rời**. Hai ảnh đó nằm **bên trong `data\updatejx06.pak`** của
chính client đích — đã chứng minh ở mục 10.3 bằng bảng chỉ mục pak (27.360 B và
4.613 B, nén `0x20000000`). Đây đúng là cái bẫy tôi mắc sáng nay và đã sửa.

> 🔑 Bài học chung: **công cụ chỉ thấy được cái nó biết nhìn.** Kết luận "không tồn
> tại" chỉ có giá trị khi công cụ đã được kiểm bằng một **đối chứng dương tính** đi
> đúng đường mã đang nghi.

#### Đã kiểm và bác bỏ thêm

- Kịch bản Viêm Đế **đã nằm sẵn** ở `E:\...\bin\server\script\missions\yandibaozang`
  (18 mục, có `saizi.lua`) — không phải việc còn phải làm.
### 12.8 ĐÃ BUILD THẬT — cả hai cấu hình, exit 0

MSBuild có sẵn: `C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe`.
**Build thử được thì đừng đoán.**

```bash
MSBuild.exe Core.vcxproj -p:Configuration="Server Release" -p:Platform=x64 -m -v:minimal
MSBuild.exe Core.vcxproj -p:Configuration="Client Release" -p:Platform=Win32 -m -v:minimal
```

Cả hai **exit 0**, không lỗi nào; chỉ còn các cảnh báo vốn có của cây.

**Nghiệm thu bằng nhị phân, không bằng suy luận:**

- `KItemDice.obj` 77.328 B; **97 ký hiệu** xúc xắc trong `CoreServer.map`; đủ 8 hàm Lua
  + `c2sDiceItem` + mọi phương thức lớp.
- Đọc thẳng bảng `g_nProtocolSize` ra khỏi DLL vừa build (ký hiệu
  `?g_nProtocolSize@@3PAHA`, địa chỉ tuyệt đối `0x18069c550`, image base `0x180000000`
  ⇒ RVA `0x69c550`):

| ô | giá trị giao thức | DLL **cũ** | DLL **mới** | |
|---|---|---|---|---|
| 105 | 170 `c2s_setmeridian` | 17 | **17** | không đổi ✔ |
| 106 | 171 `c2s_baucua` | 14 | **14** | không đổi ✔ |
| 107 | 172 `c2s_diceitem` | — | **6** | ô mới ✔ |
| | tổng ô khác 0 | 107 | **108** | đúng thêm một ✔ |

> 🔑 Ô 107 = **6** (`BYTE + int + BYTE`) chứng minh `#pragma pack(1)` **không bị rò**
> ra khỏi `KDiceProtocol.h` — nếu rò thì phải là 12. Và ô 105/106 giữ nguyên chứng
> minh **không cấu trúc cũ nào đổi cỡ, không ô nào bị dịch**.

### 12.9 ⚠️ `CoreClient.dll` ở máy chủ thật ĐÃ BỊ THAY lúc 15:44 — không phải tôi

Khi kiểm mốc thời gian tôi phát hiện:

| Tệp đang triển khai | Sửa lúc | Tình trạng |
|---|---|---|
| `bin\client\CoreClient.dll` | **26/08 15:44:32** | **ĐÃ ĐỔI** (trước 13:41 là md5 `ff7814f8`) |
| `bin\client\Game.exe` | 26/08 12:45:48 | bản **cũ** |
| `bin\server\CoreServer.dll` | 26/08 13:51:54 | bản **cũ** |

Tôi **không** build gì trước 15:58, nên bản 15:44 là do người khác build và thả.

**Đã kiểm bản đó có nguy hiểm không** — so byte với bản tôi vừa build:
**9 byte khác nhau / 2.382.848**, nằm ở 5 vùng bé (dấu thời gian PE, vùng gỡ lỗi)
⇒ **cùng một mã**, chỉ khác dấu build. Tức nó chứa đúng các thay đổi giao thức của tôi.

Bộ đang chạy hiện **lệch nhau** (client mới + `Game.exe` cũ + máy chủ cũ), nhưng
**vẫn an toàn** vì mọi thay đổi đều **thêm vào, không dịch chỗ cũ**:

- `GDCNI_DICE_ITEM` nằm **cuối** enum ⇒ `Game.exe` cũ vẫn gọi đúng callback.
- Mã giao thức mới là 172/214 — máy chủ cũ không bao giờ gửi 214, client chưa có
  giao diện nên không bao giờ gửi 172.
- Không cấu trúc cũ nào đổi cỡ (đã chứng minh ở 12.8).

> 🔴 Nhưng **đừng mong xúc xắc chạy** với bộ này: máy chủ chưa có `CoreServer.dll` mới
> và **giao diện client chưa viết**. Khi gắn thật thì phải thả **đồng thời cả ba**
> `CoreServer.dll` + `CoreClient.dll` + `Game.exe` rồi khởi động lại.
## 13. Hệ xúc xắc — phía client (26/08)

### 13.1 Tệp mới

`Sources/S3Client/Ui/UiCase/UiDiceItem.h` + `.cpp` — lớp `KUiDiceItem : public KWndImage`.

Khung 312×340 chứa **4 ô** xếp dọc. Toạ độ trong `投色子.ini` là **tương đối so với
một ô**, nên mỗi ô được làm thành một `KWndImage` riêng và các điều khiển là **con**
của nó — khỏi phải tự cộng offset theo hàng:

```
KWndImage         m_Row[4]          <- [Img3] y=0 · [Img2] y=86 · [Img1] y=172 · [Img0] y=259
  KWndObjectBox   m_ItemBox[4]      <- [objItem] 38x38 @(16,16)
  KWndText        m_Time[4]         <- [TimeInfo] @(80,28)
  KWndLabeledButton m_BtnNeed[4]    <- [BtnGreed] "Tham dự nhận" @(100,52)
  KWndLabeledButton m_BtnGiveUp[4]  <- [BtnClose] "Hủy bỏ nhận"  @(200,52)
```

### 13.2 Bốn thứ dùng lại được nguyên, không phải viết mới

| Việc | Đường có sẵn |
|---|---|
| Dựng vật phẩm **chưa sở hữu** để vẽ | `GetGameData(GDI_ITEM_CHAT, true, &ChatItem)` → chỉ số vật phẩm (`CoreShell.cpp:2226`) |
| **Nhả** vật phẩm đó | `GetGameData(GDI_ITEM_CHAT, false, nIdx)` → `ItemSet.Remove` (`:2296`) |
| **Chú giải** khi rê chuột | tự động — chỉ cần `HoldObject` đúng genre + id |
| Đếm ngược | `Breathe()` là hàm ảo của `KWndWindow`, gọi mỗi khung hình; **18 khung = 1 giây** |

> 🔑 Đây là lý do gói mang `ChatItem`: client đã biết dựng lại vật phẩm từ nó.
> **Bắt buộc nhả** khi đóng ô, không thì rò mảng `Item[]` của client
> (`KSellItem.cpp:57` làm đúng vậy).

### 13.3 Đường đi hai chiều

```
may chu -> client:
  KItemDice::SendTo -> s2c_diceitem -> KProtocolProcess::s2cDiceItem
    -> CoreDataChanged(GDCNI_DICE_ITEM, &goi, 0)
    -> GameSpaceChangedNotify.cpp case GDCNI_DICE_ITEM
    -> KUiDiceItem::OnDiceMsg()

client -> may chu:
  nut bam -> KUiDiceItem::WndProc(WND_N_BUTTON_CLICK)
    -> OperationRequest(GOI_DICE_CHOICE, nDiceId, nChoice)
    -> CoreShell.cpp case GOI_DICE_CHOICE
    -> SendClientDiceItem() -> c2s_diceitem
```

Truyền **hai số** (`nDiceId`, `nChoice`) qua `OperationRequest` thay vì con trỏ như
Bầu Cua — khỏi lo vòng đời bộ nhớ.

### 13.4 Ba cái bẫy đã né

| Bẫy | Cách xử |
|---|---|
| Tên tệp `.ini` **chữ Trung** | phải là **byte GBK thô** trong mã nguồn. Viết bằng chuỗi thoát `"\xCD\xB6\xC9\xAB\xD7\xD3-\xB9\xAB\xD3\xC3.ini"` — đã kiểm không ký tự hex nào theo sau nên không bị nuốt chữ số. Mẫu: `UiTeamApply.cpp:19` |
| `Text=Thời gian còn lại %d giây` | `KWndText::Init` đặt nguyên chuỗi **có `%d`**. Phải `Ini.GetString` giữ lại làm **định dạng** rồi tự `sprintf` mỗi giây |
| `HoldObject` cỡ ô | `DataW/DataH` là **số ô lưới** (`WndObjContainer.cpp:512` nhân với `m_nUnitWidth`). Ô `[objItem]` chỉ 38×38 = **một** ô ⇒ truyền `1,1`; để cỡ thật thì vật phẩm 2×3 vẽ tràn ra ngoài |

Thêm: `EnablePickPut(false)` cho ô vật phẩm — người chơi **chưa sở hữu** nó nên cấm
nhấc/thả.

### 13.5 Tệp đã sửa

| Tệp | Sửa gì |
|---|---|
| `Core/Src/CoreShell.h` | `GOI_DICE_CHOICE` **cuối** enum `GAMEOPERATION_INDEX` |
| `Core/Src/KProtocol.h` / `.cpp` | nguyên mẫu + thân `SendClientDiceItem` |
| `Core/Src/CoreShell.cpp` | `case GOI_DICE_CHOICE` |
| `S3Client/Ui/GameSpaceChangedNotify.cpp` | `#include` + `case GDCNI_DICE_ITEM` |
| `S3Client/Ui/UiShell.cpp` | `#include` + 3 chỗ đăng ký cửa sổ |
| `S3Client/S3Client.vcxproj` + `.filters` | 4 mục |

`GOI_DICE_CHOICE` đặt **cuối enum** vì cùng lý do với `GDCNI_DICE_ITEM`: enum này do
**cả** `CoreClient.dll` lẫn `Game.exe` biên dịch.
## 14. ĐÃ BUILD + ĐÃ CHÉP VÀO MÁY CHỦ THẬT (26/08 16:23)

### 14.1 Build

| Đích | Cấu hình | Kết quả |
|---|---|---|
| `Core` | `Server Release\|x64` | ✔ `CoreServer.dll` |
| `Core` | `Client Release\|Win32` | ✔ `CoreClient.dll` |
| `S3Client` | `Release\|Win32` | ✔ `Game.exe` |

Một lỗi duy nhất phải sửa: `KWndWindow::Breathe` là thành viên **private**
(`WndWindow.h:119`, thân rỗng) nên không gọi được từ lớp dẫn xuất —
`KUiTrembleItem` cũng không gọi. Bỏ lời gọi đó là xong.

Các lỗi `MSB3073` ở bước chép sau build là do `D:\GAMEDEVNEW\bin\client` chưa
tồn tại — **không liên quan mã**. Đã tạo thư mục để lần sau sạch.

### 14.2 Đã chép — `v34_chep_vao_may_chu.py`

| Tệp | md5 | Bản cũ giữ lại |
|---|---|---|
| `bin\server\CoreServer.dll` | `fa6bfb46d15a` | `.cu_162306` |
| `bin\client\CoreClient.dll` | `dde18f1b4c67` | (đã trùng sẵn, bỏ qua) |
| `bin\client\Game.exe` | `450eafa515df` | `.cu_162306` |

Máy chủ **đang chạy** nên bản cũ được **đổi tên** chứ không ghi đè — đổi tên vẫn
thành công khi tệp đang bị mở, và muốn quay lui thì đổi tên ngược lại.

> Bộ chép có chốt an toàn: nếu đích **mới hơn nguồn mà khác nội dung** thì dừng.
> Lúc chạy nó bắt được `CoreClient.dll` ở đích mới hơn — so byte thì **trùng 0 byte**
> với bản vừa build (có người đã chép sang lúc 16:18), nên bỏ qua đúng đắn.

### 14.3 Đã nối LỊCH CHẠY — `v35_noi_lich_viemde.py` (bước 8, trước đó còn dang dở)

`ydbz_driver.lua` nằm sẵn ở cây máy chủ nhưng **không ai gọi** ⇒ Viêm Đế không bao
giờ khởi động. Đã nhân bản đúng khuôn đợt 3HD:

| Tệp | Dòng | Thêm |
|---|---|---|
| `script/startgame.lua` | 24 | `Include("\script\tinhnang\viemde\ydbz_driver.lua")` |
| `script/startgame.lua` | 105 | `if (YDBZ_DriverInit ~= nil) then YDBZ_DriverInit() end` |
| `script/timerserver.lua` | 33 | `Include(...)` |
| `script/timerserver.lua` | 76 | `if (YDBZ_Tick ~= nil) then YDBZ_Tick(nHr, nMi) end` |

Giữ nguyên **chốt `~= nil`** của khuôn cũ: nếu tệp driver chưa nạp thì máy chủ không
nổ. Bản lưu `.truoc_lich_viemde_2608`; bản sao lưu vào
`serverscript_jx2\viemde\script\`.

### 14.4 Soát cú pháp Lua

Dựng lại `syncheck.exe` (Lua 4.0, từ `ReverseTools\lua_syncheck\syncheck.c` — bản
`.bat` cũ trỏ vào scratchpad của phiên đã chết, và thiếu 6 tệp `baselib\*.c`).

**32 tệp** (`startgame` · `timerserver` · `ydbz_driver` · toàn bộ
`missions\yandibaozang`): **0 LỖI CÚ PHÁP**.

### 14.5 Còn phải làm khi test

1. **Khởi động lại máy chủ và trò chơi** — giao thức mới ở cả hai đầu.
2. Lịch Viêm Đế: **8h25 · 10h25 · 14h25 · 16h25 · 18h25 · 20h25 · 22h25**
   (giờ máy chủ). NPC báo danh: **Bình Bình cô nương** (template 1320) ở Biện Kinh.
3. Cửa sổ xúc xắc **chưa từng chạy lần nào** — lần đầu nên soi kỹ: ô vật phẩm có
   hiện biểu tượng không, chú giải có ra không, đồng hồ có đếm không, hai nút có
   gửi được lệnh không.
4. Nếu có gì lạ, xem nhật ký máy chủ tìm chuỗi `KItemDice:` — đã cắm sẵn cảnh báo
   cho: không tìm thấy kịch bản · kịch bản chạy hỏng · hết khe phiên · túi đầy.
## 15. Bộ test toàn bộ hoạt động trên Lệnh Bài Admin (26/08)

Tệp mới: `script\item\test_hoatdong_admin.lua` (17.858 B, 441 dòng, TCVN3 + LF),
sinh bằng `ReverseTools/viemde/v36_gen_bo_test_admin.py`.
Đã nối vào `lenhbaiadmin.lua`: một dòng `Include` + một mục ở **menu gốc**.

> `lenhbaiadmin.lua` có `main()` **dofile lại chính nó mỗi lần dùng** ⇒ sửa bộ test
> **không cần khởi động lại GameServer**. Chỉ sinh lại tệp rồi bấm lệnh bài.

### 15.1 Cây menu

```
Bộ test toàn bộ hoạt động (26.08)
 ├─ 1. Chẩn đoán nhanh tất cả hoạt động (một lượt quét)
 ├─ 2. Viêm Đế Bảo Tàng (mới - chưa chạy lần nào)
 │    ├─ 1. Xem trạng thái đầy đủ
 │    ├─ 2. Đặt lại 4 NPC báo danh (gọi lặp an toàn)
 │    ├─ 3. Bắt đầu báo danh ngay (không chờ tới giờ)
 │    ├─ 4. Dịch chuyển tới NPC báo danh (Biện Kinh)
 │    ├─ 5. Kiểm 15 bản đồ phòng đấu đã nạp chưa
 │    ├─ 6. Nhận vật phẩm liên quan
 │    ├─ 7. Test hệ xúc xắc ngay (không cần đánh boss)   <= quan trọng nhất
 │    └─ 8. Xem cấu hình khối 7 đang hiệu lực
 ├─ 3. Tống Kim  (dịch chuyển 324 · vé Chiêu Thư · bật/tắt BOT · kiểm bản đồ)
 ├─ 4. Ba hoạt động bản Linux        -> HD3_AdminMenu   (đã có sẵn)
 ├─ 5. Bốn hoạt động PORT5           -> HD_AdminMenu    (đã có sẵn)
 ├─ 6. Liên Đấu                      -> LD_AdminMenu    (đã có sẵn)
 ├─ 7. Dã Tẩu                        -> DT_AdminMenu    (đã có sẵn)
 ├─ 8. Tín Sứ                        -> HD_TS_Menu      (đã có sẵn)
 ├─ 9. Tiện ích chung (giờ máy chủ · vị trí · tổ đội · về Biện Kinh)
 └─ 10. Kiểm tra động cơ: các hàm engine mới có trong DLL chưa
```

### 15.2 Hai mục đáng dùng nhất

**Mục 1 - chẩn đoán một lượt.** In ra khung chat: giờ máy chủ; với Viêm Đế thì
bật/tắt, driver + lịch đã nạp chưa, giờ chạy, bản đồ NPC, **đếm 15 bản đồ phòng đấu
1003-1017**; với hệ xúc xắc thì **kiểm đủ 8 hàm engine**; rồi ba hoạt động Linux,
bốn hoạt động PORT5, và các bộ test khác. Mỗi dòng là `Có` xanh hoặc `Không` đỏ.

**Mục 2.7 - test hệ xúc xắc ngay.** Đây là đường test **end-to-end** của cả hệ vừa
dựng, **không phải chờ đánh boss**:

```lua
nId   = ApplyItemDice(1, 100, 30, TTHD_XX_TEP, "TTHD_XX_Chot", "TTHD_XX_Goi", 1)
nItem = AddDiceItemInfo(nId, 0, 6, 1, 1605, 1, 1, 1, 1)   -- Hình nhân
RollItem(nId)
```

Chạy hết một vòng: engine tạo phiên → tạo vật phẩm thật → gửi `s2c_diceitem` →
client mở cửa sổ → bấm nút gửi `c2s_diceitem` → engine gieo điểm → gọi lại
`TTHD_XX_Goi(nId, nSo)` → chốt → `TTHD_XX_Chot(nId, nThắng, nSố)`. Hai hàm gọi lại
in kết quả ra chat, và hàm chốt còn gọi `GetItemDiceItemInfo` để kiểm luôn nhóm hàm
truy vấn.

> 🔑 `TTHD_XX_TEP` trỏ tới **`lenhbaiadmin.lua`** chứ không phải chính tệp test.
> Lý do: `Include` của JX1 = `lua_dofile` vào trạng thái của tệp **GỌI**
> (`ScriptFuns.cpp:1969`), nên hai hàm gọi lại nằm trong trạng thái Lua của
> `lenhbaiadmin.lua` — tệp chắc chắn đã được engine nạp.

### 15.3 🔴 Luật viết tiếng Việt trong kịch bản JX1 (đo được, không phải phỏng đoán)

Kịch bản dùng **TCVN3 một byte**. Đo bằng `bangtxt.py`:

| nhóm | mã hoá được |
|---|---|
| nguyên âm **thường** có dấu | **67/67** ✔ |
| nguyên âm **HOA** có dấu | **7/67** ✘ |

Bảy chữ hoa sống sót: **Â Ê Ô Ă Đ Ơ Ư** — chỉ chữ mang *dấu phụ nền*, **không** mang
thanh điệu. Mọi chữ hoa có thanh (À Á Ả Ã Ạ Ầ Ế Ộ Ứ…) bị **bỏ lặng lẽ**.

Lỗi thật đã gặp: `"BỘ TEST TOÀN BỘ HOẠT ĐỘNG"` ghi ra thành `"B TEST TON B HOT ĐNG"`.

⇒ **Viết câu thường / Title case, đừng viết HOA toàn bộ.** Nhấn mạnh thì dùng
`<color=yellow>…<color>`.

⇒ Bộ sinh có **chốt vòng tròn** `uni → tcvn → uni` (`kiem_mat_dau()`), báo đúng ký tự
nào rơi ở dòng nào rồi **dừng hẳn**. Nó đã bắt được 2 chỗ mắt thường bỏ qua:
`É` trong "Ép" và `Ấ` trong "Ấn Đồng".

⛔ Còn một luật nữa: **cấm dấu `/` trong NHÃN menu** — `ScriptFuns.cpp:717` cắt ở dấu
`/` đầu tiên, phần sau là **tên hàm**.

### 15.4 Đã kiểm

- `syncheck.exe` (Lua 4.0): `test_hoatdong_admin.lua`, `lenhbaiadmin.lua`,
  `hoatdong_admin.lua`, `hd3_admin.lua` — **0 lỗi cú pháp**.
- Giải mã ngược TCVN3 → Unicode toàn tệp: chữ Việt hiện đúng, **794 byte có dấu**.
- Bản sao lưu vào `serverscript_jx2\viemde\script\item\`.
