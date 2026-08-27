# PHÂN TÍCH & DỊCH NGƯỢC 3 TÍNH NĂNG: VIÊM ĐẾ · ÉP ĐỒ TÍM · ÉP ĐỒ HOÀNG KIM MÔN PHÁI

> Ngày 26/08/2026. Làm theo `HUONGDAN_DICHNGUOC_TINHNANG_LINUX.md` (bước 1→2 của quy trình 8 bước).
> **CHƯA SỬA MỘT DÒNG NÀO** — đây là báo cáo đo đạc. Mọi con số dưới đây đều có `tệp:dòng` làm bằng chứng.

---

## 0. TÓM TẮT MỘT TRANG

| | Viêm Đế Bảo Tàng | Ép đồ TÍM | Ép đồ HOÀNG KIM MÔN PHÁI |
|---|---|---|---|
| Nguồn Linux | `script\missions\yandibaozang\` — 26 tệp, **2.906 dòng sống** | `script\item\compound\` — 9 tệp, **1.133 dòng sống** | *cùng thư mục trên*, tệp `atlas.lua` (272 dòng sống) |
| Bản Linux có chạy không | ✅ Có | ✅ Có | ✅ Có |
| JX1 đã có gì | Bản đồ 851–862 **đã khai trong MapList**, `head.lua` (214 dòng), 100% hàm engine cần thiết | **Giao diện client 6 thẻ đã vẽ xong** (`UiCompoundItem.cpp` 3.768 dòng), enum `NATURE_VIOLET` trong engine | Bảng `atlas_compound.txt` **1.297 dòng**, 260 Đồ phổ, 9 Mảnh thiên thạch, `goldequip.txt` 7.398 dòng |
| JX1 thiếu gì (cốt lõi) | 31 NPC template, 26 tệp script (**bản đồ đã tìm ra — xem §3**) | **Toàn bộ backend máy chủ** (bị chú thích trong `KItemList.cpp`), + 10 hàm Lua | Như cột giữa **+** phải nắn lại id nguyên liệu |
| Có phải build lại engine không | ❌ **Không** | ⚠️ **Có** (chủ game đã chốt làm đúng bản gốc) | ⚠️ **Có** (chung nền với cột giữa) |
| Khối lượng ước lượng | Lớn | Vừa (backend) + rất lớn (giao diện) | Nhỏ *nếu* dùng lại `ghephkmp` sẵn có |

**Điều bất ngờ lớn nhất**: “ép đồ tím” và “ép đồ hoàng kim môn phái” **không phải hai tính năng** — chúng là **hai thẻ của cùng MỘT hệ thống** trong bản gốc. Bằng chứng: `script\global\equip_system.lua:1` ghi nguyên văn *“Chức năng: Hệ thống chế tạo trang bị đồ tím và hkmp”*, và `settings\item\compoundscript.txt` liệt kê cả 7 thẻ trong một bảng.

---

## 1. HAI TÍNH NĂNG ÉP ĐỒ — THỰC RA LÀ MỘT HỆ “LÒ RÈN” 7 THẺ

### 1.1 Bảng điều phối của bản Linux

`D:\ServerLinux\server1\settings\item\compoundscript.txt`:

| Thẻ | Kiểu | Kịch bản | Việc |
|---|---|---|---|
| 0 | `xuanjing_compound.lua` | Tinh luyện | Trang sức xanh/trắng → **Huyền Tinh Khoáng Thạch** cấp 1–10 |
| 1 | `ore_upgrade.lua` | Nâng cấp | 3 Huyền Tinh cùng cấp → 1 cấp cao hơn |
| 2 | `ore_upgrade.lua` | Nâng cấp | 3 khoáng thuộc tính cùng cấp → 1 cấp cao hơn |
| 3 | `magic_distill.lua` | Chiết xuất | Rút 1 dòng thuộc tính của **trang bị xanh** sang **Nguyên Khoáng** |
| 4 | `equip_compound.lua` | Chế tạo | **← ÉP ĐỒ TÍM**: trang bị xanh/trắng + Huyền Tinh → **trang bị tím có 1–5 lỗ trống** |
| 5 | `equip_enchase.lua` | Khảm nạm | Khoáng thuộc tính → khảm vào lỗ trống của đồ tím |
| 6 | `atlas.lua` | Đồ phổ | **← ÉP ĐỒ HOÀNG KIM MÔN PHÁI**: Đồ phổ + Huyền Tinh + 6 nguyên liệu → **trang bị Hoàng Kim** hoặc **mảnh Hoàng Kim** |
| 7, 8 | `fantasygold*_upgrade.lua` | Huyền Kim | Nâng cấp Huyền Kim / Huyền Kim Chi Tinh |

Cửa vào: `script\global\equip_system.lua:17` gọi `FoundryItem()` → engine mở cửa sổ lò rèn ở client.
NPC gọi nó: `script\global\pgaming\npc\thoren\{daohoanguyen,macbac,visondao}.lua`.
Công trường bang hội cũng dùng: `script\tong\workshop\ws_bingjia.lua:239` gọi `FoundryItem(4)`.

### 1.2 “Ép đồ tím” hoạt động thế nào (thẻ 4)

`equip_compound.lua:1` — *“紫色装备系统 - dùng trang bị xanh/trắng + khoáng thạch để chế tạo trang bị TÍM có lỗ trống”*.

1. Nguyên liệu **bắt buộc**: 1 trang bị (genre 0) + Huyền Tinh Khoáng Thạch. Phí 10.000 lượng (`COMPOUND_COST`).
2. `genDesItemsInfo()` (dòng 48–58) dựng **5 phương án đích**: cùng chủng loại/cấp/ngũ hành với trang bị nguồn, nhưng `nQuality = 2` (đồ tím) và có **1, 2, 3, 4 hoặc 5 lỗ trống** (đánh dấu bằng `arynMagLvl[j] = -1`).
3. Chọn phương án nào là do **giá trị vật phẩm**: `compound_header.lua:120` gọi `TransItemVal(tổng giá trị nguyên liệu, giá trị 5 phương án)` — càng đổ nhiều/tốt nguyên liệu thì càng dễ ra món nhiều lỗ. Bỏ tiền ra rồi **có thể trượt hết, mất sạch nguyên liệu** (`defFinalCompound` nhánh `else`, dòng 154–159).
4. Có **trần chống lạm phát theo ngày** toàn máy chủ: `compound_header.lua:45–53, 237–252` — biến toàn cục 32/33/34 cộng dồn tổng giá trị đã ép trong ngày; quá ngưỡng thì thợ rèn “mệt”, khoá tới hôm sau.

Engine JX1 **đã hiểu đồ tím**: `Sources\Core\Src\GameDataDef.h:655-660` có `enum EQUIPNATURE { NATURE_NORMAL, NATURE_VIOLET, NATURE_GOLD, NATURE_PLATINA }`, và `KItem.cpp:2447` vẽ chữ “Chưa khảm nạm” cho lỗ trống, `KItem.cpp:2482` tô màu `<color=170,30,255>` (tím) cho dòng thuộc tính. **Chỉ thiếu đường sinh ra nó.**

### 1.3 “Ép đồ Hoàng Kim môn phái” hoạt động thế nào (thẻ 6)

`atlas.lua:1` — *“紫色装备系统 - dùng Đồ Phổ Hoàng Kim và nguyên liệu ghép ra trang bị Hoàng Kim”*.

Đặt vào lò rèn:
- **1 Đồ phổ Hoàng Kim** (`6,1,238…377` ở JX1) — quyết định món sẽ ra.
- **1 Huyền Tinh Khoáng Thạch** — bắt buộc (`atlas.lua:186`, thiếu là `RESULT_RULE_ERROR`).
- **Tối đa 6 nguyên liệu** phải khớp *đúng* dòng công thức: đúng mã, **cấp ≥ yêu cầu**, đúng ngũ hành, **đúng mã thuộc tính của khoáng** (`verifyAtlasMaterials`, dòng 215–257).
- **Ô phụ**: 1 “Mảnh thiên thạch” (`4,1317…1325`). Đây là **công tắc chia nhánh**:
  - Có mảnh thiên thạch số N → ra **mảnh Hoàng Kim thứ N** của món đó.
  - `atlas.lua:160-164` ghi rõ: *“bỏ đoạn này đi là có thể ghép thẳng ra Hoàng Kim, đặt phân giải thạch thì ra mảnh, không đặt thì ra trang bị Hoàng Kim”* — hiện bản Linux **bắt buộc phải có** mảnh thiên thạch.

Tỉ lệ thành công (`transItemVal_Atlas`, dòng 286–304) — **rất đáng chú ý vì nó khác hẳn bản `ghephkmp` 20% cứng đang có ở JX1**:

```
tỉ lệ = tổng giá trị nguyên liệu đặt vào / giá trị món Hoàng Kim   (chặn trần ở 100%)
```

Nghĩa là người chơi **mua được tỉ lệ** bằng cách đổ thêm khoáng/bí kíp. Giá trị từng thứ nằm ở `settings\item\itemvaluescript.txt` (ví dụ Võ Lâm Mật Tịch = 350 triệu, Thần bí khoáng thạch = 100 triệu).

Thợ rèn còn có mục **“Tìm hiểu mảnh Hoàng Kim”** (`atlas.lua:87-124` `QueryPiece`) cho xem một đồ phổ chia được bao nhiêu mảnh.

### 1.4 JX1 đang có gì — và tại sao nó **không chạy**

**Client: giao diện đã vẽ xong, nhưng bị cắt dây thần kinh.**

`Sources\S3Client\Ui\UiCase\UiCompoundItem.h:1` — *“Hộp thoại chế tạo trang bị tím — Author: Fong Kiều, 2021”*. 3.768 dòng, đủ 6 lớp `KUiCompoundOne/Two/Three/Distill/Forge/Enchase` + nút `m_AtlasPadBtn`, enum `WINDOWS_COMP…WINDOWS_ATLAS` **khớp một-một với 7 thẻ của `compoundscript.txt`**.

Nhưng **cả 6 nút “Hợp thành” đều bị chú thích mất lệnh gửi lên máy chủ**:

```
UiCompoundItem.cpp:843    //g_pCoreShell->OperationRequest(GOI_LOAD_BUTTON_SCRIPT, ... "CompOneItem");
UiCompoundItem.cpp:1319   // ... "CompTwoItem"
UiCompoundItem.cpp:1871   // ... "CompThreeItem"
UiCompoundItem.cpp:2455   // ... "DistillItem"
UiCompoundItem.cpp:2840   // ... "ForgeItem"
UiCompoundItem.cpp:3629   // ... "EnchaseItem"
```

Bấm nút hiện giờ chỉ đọc một câu từ file ini rồi thôi. Ngoài ra `GOI_LOAD_BUTTON_SCRIPT` **không tồn tại chỗ nào khác trong toàn bộ mã nguồn** — nó là hằng số chết.

**Máy chủ: các ô chứa nguyên liệu bị chú thích nguyên khối.**

`Sources\Core\Src\KItemList.h:33` chỉ khai `int m_TrembleItem[compoundpart_num];`. **Không có** `m_CompOneItem`, `m_CompTwoItem`, `m_CompThreeItem`, `m_DistillItem`, `m_ForgeItem`, `m_EnchaseItem`. Toàn bộ đoạn xử lý chúng nằm trong khối `/* … */` ở `KItemList.cpp:5416-5535` và `CoreShell.cpp:12546`.

Hệ quả: kéo vật phẩm vào ô của cửa sổ đó **máy chủ không biết gì cả**.

**Bản mẫu đang chạy tốt để bắt chước: “Khảm nạm” (Tremble).** Đây là cùng một cơ chế nhưng đã nối dây đầy đủ:
- Lua: `ScriptFuns.cpp:14793` `{"OpenTrembleItem", …}` → đặt `m_szTaskExcuteFun = "TrembleItem"` (dòng 10232).
- Client bấm nút: `UiTrembleItem.cpp:290` `OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 5, "TrembleItem")` ← **dòng này KHÔNG bị chú thích**.
- Máy chủ nhận: `KProtocolProcess.cpp:6111` `ExecuteScript(m_dwTrembleItemId, m_szTaskExcuteFun, "")`.
- Kịch bản: `script\global\npcchucnang\thoren.lua:1515` `function TrembleItem()`.

**Cửa mở đã có sẵn**: `ScriptFuns.cpp:14795` `{"OpenCompoundItem", LuaOpenCompoundItem}` và `GameSpaceChangedNotify.cpp:585-586` `case GDCNI_OPEN_COMPOUND_ITEM: KUiCompoundItem::OpenWindow();`. Menu thợ rèn cũng đã có dòng `-- "Khảm nạm trang bị Tím/OpenCompoundItem"` (bị chú thích, `thoren.lua:63`).

**Thiếu tệp giao diện**: `UiCompoundItem.cpp:30` đọc `Ui\Ui3\khamnam\Tinhluyen.ini` — **thư mục `Ui\Ui3\khamnam\` không tồn tại trong client**. Phải tự viết file bố cục (vị trí từng ô, ảnh nền, hiệu ứng).

### 1.5 JX1 **đã có sẵn một bản “ép Hoàng Kim” tự chế** — đang bị tắt

`script\global\npcchucnang\thoren.lua:331-455`, `function ghephkmp()` / `batdaunangcaphkmp()`:
- Đặt 1 Đồ phổ (`6,1,238…377`) vào `OpenGiveBox`.
- Yêu cầu cứng: 500 vạn lượng + 50 Tiền Đồng + 200 Điểm sự kiện + 1 Tử/Lục/Lam Thủy Tinh + 6 Tinh Hồng Bảo Thạch.
- `random(1,7) + task_FANCUNG_5 >= 30` → **tỉ lệ ~20% cố định, không phụ thuộc nguyên liệu**.
- Thành công: `AddItem2(2, 0, nDoPho - 238, 0, 0, 0)` = `NATURE_GOLD`, dòng thứ `(id đồ phổ − 238)` của `goldequip.txt`.

Menu tắt nó ở `thoren.lua:56-57` (`-- "Ghép Trang Bị HKMP/ghephkmp"`, `-- "Ghép mảnh đổ phổ hkmp/ghepmanhdopho"`).

> **Đây là lựa chọn rẻ nhất**: bật lại 2 dòng menu này là có ngay tính năng ép HKMP, không cần đụng engine. Nhưng nó **không có công thức riêng cho từng món**, không có hệ mảnh, và tỉ lệ cứng.

### 1.6 KIỂM ĐỊNH BẢNG `atlas_compound.txt` CỦA JX1 — có 1 quả mìn

JX1 đã có `settings\item\atlas_compound.txt`, **1.297 dòng**. Tôi so từng ô số với bản Linux `settings\item\004\atlas_compound.txt`:

> **Kết quả: 0 ô số nào khác nhau. Bảng của JX1 là bản sao y nguyên của bản Linux, chỉ dịch phần tên.**

Nghĩa là **mọi mã vật phẩm trong bảng đang là mã của bản Linux**. Tôi tra ngược từng mã sang bảng item JX1 (theo TÊN, đúng luật bẫy #7 của hướng dẫn):

**a) Cột nguyên liệu — 30 loại sách bí kíp KHỚP HOÀN TOÀN, 6 loại khoáng LỆCH ĐÚNG 1:**

| Tên | Linux | **JX1 đúng** | Số ô bị ảnh hưởng |
|---|---|---|---|
| Huyền Thiết khoáng | 6,1,200 | **6,1,199** | 789 |
| Khổng Tước Thạch | 6,1,201 | **6,1,200** | 438 |
| Mật Ngân khoáng | 6,1,202 | **6,1,201** | 333 |
| Phù Dung Thạch | 6,1,203 | **6,1,202** | 845 |
| Chu Sa khoáng | 6,1,204 | **6,1,203** | 557 |
| Chung Nhũ thạch | 6,1,205 | **6,1,204** | 176 |
| | | **Tổng** | **3.138 ô** |

Nếu bật tính năng mà không nắn: người chơi đặt Huyền Thiết khoáng vào thì hệ thống lại đi tìm Khổng Tước Thạch → **không công thức nào ghép được**, im lặng báo thiếu nguyên liệu.

Cùng lệch −1 này áp cho: **Huyền Tinh Khoáng Thạch** (Linux `6,1,147` → JX1 `6,1,146`), **Thần bí khoáng thạch** (398 → 397), **6 Nguyên Khoáng** dùng trong chiết xuất (Linux 149–154 → JX1 148–153).

**b) Cột Đồ phổ (`ATLAS_*`) — lệch nặng hơn:**

Trong 153 dòng sinh ra trang bị Hoàng Kim:
- **132 dòng tra được tên sang JX1**: 124 dòng lệch **+1**, 4 dòng lệch **+6**, 4 dòng lệch **−4**. → không thể trừ đi một hằng số, **phải tra từng dòng theo tên**.
- **21 dòng KHÔNG tra được tên** — bảng item JX1 không có đồ phổ đó (vd `Mộng Long Đạt Ma Tăng Hài`, `Phục Ma Huyền Hoàng Cà Sa`, `Tứ Không Tử Kim Cà Sa`, `Ngự Long Tấn Phong Hộ Uyển`…). **Cần chủ game quyết**: bỏ 21 món này, hay thêm item mới.

**c) Tin tốt — 3 thứ khớp sẵn, khỏi đụng:**
- **Cột món sinh ra (`DES_*`): 152/153 khớp `goldequip.txt` của JX1.** Chỉ 1 dòng lệch.
- **595 dòng mảnh Hoàng Kim khớp `questkey.txt` của JX1 100%** (`4,576…` = “Mảnh <tên món> (n/9)”), và **9 Mảnh thiên thạch `4,1317…1325` đã có sẵn**.
- **`magicattrib.txt` của JX1 giống hệt bản Linux** (331 dòng, nội dung trùng) → cột `_MAGIC_ID` trong công thức **dùng được ngay**.

**d) Độ phủ thật của bảng:**

| | Số dòng |
|---|---|
| Có công thức nguyên liệu | **727** |
| Trống (không ghép được) | 570 |
| Sinh ra **trang bị Hoàng Kim** (`DES_QUALITY=1`) | 153 — trong đó **96 có công thức** |
| Sinh ra **mảnh Hoàng Kim** | 1.144 — trong đó **595 có công thức** |

→ Nếu bật hệ Đồ phổ nguyên bản, chỉ **96 món Hoàng Kim** ghép thẳng được; phần còn lại phải đi đường **gom mảnh** (mỗi món 4, 6 hoặc 9 mảnh).

### 1.7 Hàm engine còn thiếu cho hệ ép đồ

Đã đối chiếu 1.083 hàm Lua mà JX1 đăng ký trong `Sources\Core\Src\*.cpp`:

| Hàm | Bản Linux làm gì | Hướng xử ở JX1 |
|---|---|---|
| `FoundryItem(scheme)` | Mở cửa sổ lò rèn | **JX1 đã có `OpenCompoundItem()`** — dùng thay, chỉ thiếu tham số scheme |
| `AddItemEx(...16 tham số)` | Tạo vật phẩm đủ: version, randseed, **quality**, 6 mức thuộc tính | ⚠️ **Phải thêm vào engine.** `AddItem2` của JX1 chỉ nhận `nItemLevel[0..5]`, **không nhận quality** (nó suy từ `nItemNature`) và không nhận randseed |
| `ITEM_CalcItemValue(...)` | Tính giá trị vật phẩm | ✅ **Không phải viết C++**: engine Linux chỉ là bộ điều phối, đọc `settings\item\itemvaluescript.txt` rồi gọi hàm `CalcItemValue()` trong **kịch bản Lua** (`itemvalue\ore.lua`, `equip_gold.lua`, `equip_normal.lua`, `equip_enchasable.lua`). Viết lại bằng Lua thuần được |
| `ITEM_GetItemVersion` / `ITEM_GetLatestItemVersion` | Hệ **phiên bản vật phẩm** (`settings\item\000…004\`) | ✅ JX1 **không có hệ phiên bản** — thư mục `settings\item\` phẳng ⇒ trả cứng `0` |
| `WriteCompoundLog` | Ghi log riêng | ✅ Ánh xạ sang `WriteLog` (JX1 đã có) |
| `GetItemBindState` | Đọc trạng thái khoá | ✅ JX1 có `GetPlayerItemIsLock` (dùng ở `thoren.lua:359`) và `SetItemBindState` |
| `GetMaxDurability` / `GetCurDurability` / `SetMaxDurability` / `SetCurDurability` | Độ bền (dùng ở chiết xuất) | ⚠️ Thiếu — nhưng chỉ thẻ 3 cần |
| `GetItemMagicDesc` | Chữ mô tả thuộc tính | ⚠️ Thiếu — chỉ dùng để hiển thị |
| `makeItemFilePath`, `loadItemTabFiles`, `TransItemVal`, `inssort`, `pack`, `isMagicMatch*`, `getMagAttrLvl*`, `CalcFantasyGold*Value*` | | ✅ **Đều là Lua thuần**, chép sang là chạy |

**Bảng dữ liệu phải chép từ Linux** (`settings\item\004\itemvalue\`, JX1 chưa có thư mục này): `ore.txt`, `equip_gold.txt`, `equip_normal_magic.txt`, `equip_enchasable_{level,socket,type,magic_pos}.txt`, `magicattrib_combine.txt`, `fantasygold.txt`, `fantasygoldessence.txt` — **10 tệp**. Chúng khoá theo **cấp** và **mã thuộc tính** chứ không theo mã vật phẩm, nên nhìn chung không phải nắn id.

**Việc phía engine nếu làm đúng bản gốc** (thẻ 0–6):
1. Thêm 6 mảng `m_CompOneItem[]`… vào `KItemList.h` + bỏ chú thích 6 khối ở `KItemList.cpp:5416-5535` và `CoreShell.cpp:12546`.
2. Thêm nhánh `pos_compone…pos_enchase` vào đường đặt/nhấc vật phẩm (`KItemList::AddKIL`, `KProtocolProcess`) + `GetGameData(GDI_COMPONE_ITEM…)`.
3. Bỏ chú thích 6 dòng gửi lệnh ở `UiCompoundItem.cpp`, đổi sang `GOI_ADD_UI_CMD_SCRIPT` như `UiTrembleItem.cpp:290`.
4. Thêm `AddItemEx` + 4 hàm độ bền.
5. **Viết mới `Ui\Ui3\khamnam\Tinhluyen.ini`** cho client.
6. Build lại `Core.vcxproj` cả `"Server Release"|x64` **và** `"Client Release"|Win32`, + `Game.exe`.

---

## 2. VIÊM ĐẾ BẢO TÀNG

### 2.1 Cách chơi (rút từ 2.906 dòng)

Là một **phó bản vượt ải theo tổ đội, 3 đường đua song song**.

**Báo danh** — NPC `yandibaozang_main.lua`, điều kiện (`YDBZ_PlayerCheck`, dòng 35–69):
- Đã vào môn phái, **cấp ≥ 120** (`YDBZ_LIMIT_PLAYER_LEVEL`).
- Tổ đội đủ số người, tối đa 15 đội/đợt.
- Trần **4 lượt/ngày**, **10 lượt/tuần** (`head.lua:49-50`) — mang **Viêm Đế Lệnh** thì miễn trần.
- Cần **Anh Hùng Thiếp** (`6,1,1604` bản Linux) làm tín vật.

**Phòng chờ** — bản đồ 851/852, mở 5 phút (`ready.lua:38`), NPC bán thuốc, cấm PK/giao dịch/hồi thành.

**Vào trận** — chia ngẫu nhiên tối đa 3 đội vào 1 trong 10 bản đồ 853–862, mỗi đội một **trận doanh** (camp 1/2/3) và một **đường** A/B/C.

**Vượt ải** — mỗi đường 10 ải (`npc.lua:13-55`, toạ độ ở `settings\maps\yandibaozang\way{a,b,c}_01…10.txt`):
- Mỗi ải: ~60 quái thường + 1 boss (Cổ Thiên Khuyết, Chứng Ngôn, Chiếu Nhật Miện, Đường Hạo, Diệu Nhất, Lệ Chiết Mai, Đoan Mộc Li, Bạch Lang, Tần Hiểu Thư, Lăng Tiêu Tử).
- Ải 3 và 6 có thêm boss “(Nộ)” riêng cho từng đường.
- Qua mỗi ải: **cả đội +600.000 kinh nghiệm** (`YDBZ_AWARD_EXP`) + 1 **Viêm Đế Bí Bảo**.

**Tranh đoạt** — sau khi 3 đường xong, tất cả dồn về khu giữa, **PK lẫn nhau**, giết người +200.000 kinh nghiệm, chết thì hồi sinh tại chỗ với 3 giây bất tử.

**Boss cuối** — **Lương Mi Nhi**, +1.000.000 kinh nghiệm. Rơi đồ chia làm **4 phần độc lập** (`head.lua:149-175`):
- Phần 1: Tinh Tinh Khoáng 6,43% · Tinh Thiết Khoáng 3,21%
- Phần 2: Hộp Mặt Nạ Chiến Trường 11,15% + 2,57% · 4 loại Phi Phong (1–3%)
- Phần 3: Túi Dược Phẩm 40% · Càn Khôn Tạo Hóa Đan ×50 30% · Đại Lực hoàn 15% · Phi Tốc hoàn 15%
- Phần 4 (hàng hiếm): Đồ Phổ Đằng Long Bội **0,1%** · Đằng Long Thạch ×5 2% · Đồ Phổ Tinh Sương Yêu Đái **0,11%** · Đồ Phổ Tinh Sương Bội **0,11%**

**Giới hạn thời gian** 30 phút, xong thì 30 giây sau bị đưa ra.

**Móc sẵn có ở JX1**: `script\tinhnang\3hoatdong\beidou\bd_head.lua` đã viết sẵn `tbBeidou:PhatLenhBai("viemde1"/"viemde2")` — chỉ chờ gọi.

### 2.2 JX1 thiếu gì — và **một tin rất tốt về bản đồ**

**a) Bản đồ đã được khai sẵn.** `settings\MapList.ini:5529-5563` **đã có đủ 851–862** với tên tiếng Việt do dự án tự đặt:
```
851/852 = "Cửa vào bảo tang Viêm Đế"      (đường: 特殊用地\炎帝宝藏准备场)
853..862 = "Bảo tàng Viêm Đế (1)".."(10)" (đường: 特殊用地\炎帝宝藏)
```
**Nhưng tệp dữ liệu bản đồ chưa có**: `bin\server\maps\` chỉ có **94 tệp `_srv.fp`**, không có `851_srv.fp`…`862_srv.fp`. Client cũng không có `85x.fp` rời.

**b) Hệ phó bản động — HOÁ RA KHÔNG CẦN.** Đây là chỗ hướng dẫn cũ đánh giá là “khối lượng thật”, tôi đo lại thì nhẹ hơn nhiều:
- `readymap.lua:5,77,87` dùng `Dungeon:new_type` / `new_dungeon` / `PreApplyDungeonMap(852,0,0)` — 4 hàm engine JX1 không có (`ApplyDungeonMap`, `ReturnDungenonMap`, `PreApplyDungeonMap`, `ApplyDelDungeonMap`).
- **Nhưng nó chỉ nhân bản đúng PHÒNG CHỜ (852)**. Còn **10 bản đồ đánh nhau 853–862 là bản đồ TĨNH** (`head.lua:86-97` `YDBZ_MAP_MAP = {853…862}`).
- ⇒ **Dùng 851 làm phòng chờ tĩnh là bỏ được toàn bộ `dungeon.lua` và 4 hàm engine kia.** Đổi lại: mọi người chờ chung một phòng thay vì mỗi đội một phòng riêng.

**c) Xúc xắc chia đồ — cũng chỉ dùng đúng một chỗ.** 6 hàm `ApplyItemDice`/`AddDiceItemInfo`/`RollItem`/`GetItemDice*` là thư viện engine + cửa sổ client mà JX1 không có. Nhưng cả tính năng **chỉ gọi ở `npc_death.lua:34`**, cho các phần thưởng đánh cờ `y1[4]==1` (Hình nhân, Ngũ Hoa Ngọc Lộ Hoàn) ⇒ thay bằng “trao thẳng cho một thành viên ngẫu nhiên” là xong.

**d) 100% hàm engine của phần logic trận đã có ở JX1.** Tôi kiểm 23 hàm chính (`SubWorldID2Idx`, `OpenMission`, `CloseMission`, `IsMission`, `GetMSPlayerCount`, `GetNextPlayer`, `SetMissionV/S`, `AddNpc`, `SetNpcScript`, `AddTimer`, `SetTaskTemp`, `CallPlayerFunction`, `GetTeamSize/Member`, `Msg2Map/Team/Tong`, `AddGlobalNews`, `RemoteExecute`, `SetTmpCamp`, `SetPKFlag`, `ForbidChangePK`, `DisabledUseTownP`, `ForbitTrade`, `ForbidEnmity`, `SetCreateTeam`, `SetLogoutRV`, `SetDeathScript`, `SetDeathType`, `AddSkillState`, `ConsumeItem`, `CalcItemCount`, `CalcFreeItemCellCount`) → **tất cả đều có**.
> ⚠️ Nhắc lại bẫy #3 và #4 của hướng dẫn: `GetMSPlayerCount(id, nhóm)` và `IsMission` — bản Linux dùng `GetMissionV(...)==0` để đoán mission chưa mở (`readymap\include.lua:309`), sang JX1 phải đổi sang `IsMission()`.

**e) 31 NPC template còn trống.** `settings\npcs.txt` của JX1 (2.036 dòng, id = số dòng dữ liệu): **các dòng 1289–1319 đều là `<<未定义>>`**, và **không dòng nào trong bảng mang tên 13 boss Viêm Đế**. ⇒ Không đụng ai, nhưng phải tự khai 31 template (chỉ số + hình ảnh).

**f) Vật phẩm.** 36 mã dùng trong tính năng: **14 mã KHÁC NGHĨA** (phải tra lại theo tên) và **17 mã JX1 KHÔNG CÓ** — trong đó nhóm Đồ Phổ Tinh Sương / Đằng Long (`30006/30008/30011/30505/30506/30507/30529/30534/30537/30538`) là **rơi từ boss cuối phần 4**, phải quyết định giữ hay thay.
> 5/6 mã đã có sẵn bảng tra từ đợt Bảo Rương Thủy Tặc (xem `ReverseTools\gen_baoruong_thuytac.py`): 30228→4846, 30229→4847, 30301→4857, 30289→3051, 4134 Chân Nguyên Đan→3926.

**g) Task id.** `2618` **TRỐNG, dùng được**. `200` **ĐÃ BỊ DÙNG** ở `battles\battlehead.lua`, `item\hoatdong_admin.lua`, `missions\bw\bwhead.lua` → **bắt buộc đổi**. Ngoài ra `head.lua` còn dùng 1851–1855 (chưa kiểm chéo, sẽ làm khi thi công).

**h) Đã có sẵn ở JX1**: `script\missions\yandibaozang\head.lua` (214 dòng — đúng là tệp hằng số của bản Linux, đã dịch), và `script\huoyuedu\huoyuedu.lua:24` đăng ký `["yandibaozang"] = 5` cho hệ độ hoạt nhảy.
> `script\task\metempsychosis\translife_5.lua:225` có móc `EventSys:GetType("YanDiBaoZang"):Reg("OnPass", …)` (qua 6 lần Viêm Đế được thưởng Trùng Sinh 5) — **nhưng `TRANSLIFE5_init()` đã bị tắt ở dòng 230** (“Make change translife for free”), nên đây **không phải** thứ đang chặn ai cả.

---

## 3. BẢN ĐỒ VIÊM ĐẾ — ĐÃ TÌM RA, KHÔNG CÒN CHẶN ✅

*(Bổ sung 26/08 sau khi chủ game chốt làm Viêm Đế trước)*

**Bản đồ nằm sẵn trong client JX1**, ở `bin\client\maps2\特殊用地\`:

| | Thư mục | `.wor` | Ảnh nhỏ | Số tệp region |
|---|---|---|---|---|
| Bảo tàng (853–862) | `炎帝宝藏` | `rect=99,100,123,122` (25×23 region) | `炎帝宝藏24.jpg` 742 KB | **500** |
| Phòng chờ (851–852) | `炎帝宝藏准备场` | `rect=97,97,101,101` (5×5) | 34 KB | **25** |

**Bốn kiểm định đã chạy:**

1. **Toạ độ trong kịch bản khớp bản đồ.** `head.lua:60` điểm rơi `{60032,104832}` = ô (1876, 3276); vùng bảo tàng phủ ô X 1584–1984, Y 3200–3936 → **nằm trong**. `readymap.lua:13` đặt NPC tại ô (1577, 3184); phòng chờ phủ ô X 1552–1632, Y 3104–3264 → **nằm trong**. Đây đúng là bản đồ CN mà bộ kịch bản nhắm tới.

2. **Dữ liệu vật cản dùng được ngay.** Đoạn `OBSTACLE` trong `_Region_C.dat` dài **đúng 2048 byte** — chính xác cỡ `int[16][32]` mà `KRegion::LoadServerObstacle` (`KRegion.cpp:407`) đòi khớp tuyệt đối. **204/500** tệp bảo tàng và **10/25** tệp phòng chờ có vật cản thật (khác 0); phần còn lại là đất trống. Đối chứng bản đồ JX1 đang chạy (Kiếm Môn Quan vn): cũng 2048 byte.

3. **Không cần đoạn TRAP/NPC trong tệp bản đồ.** Cả 525 tệp đều có `TRAP=0, NPC=0` — đúng như thiết kế, vì tính năng **tự sinh** quái bằng `AddNpc` theo `settings\maps\yandibaozang\way{a,b,c}_01…10.txt` và tự đặt trap theo `head.lua:210-215`. `LoadServerTrap`/`LoadServerNpc` gặp độ dài 0 thì thoát an toàn (đã đọc mã).

4. **Ảnh nền có sẵn.** Bản đồ chỉ dùng **3 ảnh địa hình** (`熔地.spr`, `熔浆.spr`, `熔地_熔浆.spr` — nền dung nham). Quét 39.477 tệp region của cả 132 bản đồ: **bản đồ duy nhất khác cũng dùng đúng 3 ảnh này là `banghuichengbao` (Bang Hội Thành Bảo)** — đã port và đang chạy ở JX1 (có `Pak\maps_banghuichengbao.pak`). Vậy ảnh chắc chắn đã nằm trong pak client.

**Việc phải làm (thuần chép tệp, KHÔNG đụng engine):**

```
1) client:  maps2\特殊用地\{炎帝宝藏, 炎帝宝藏准备场}  ->  maps\特殊用地\
            (kèm 2 tệp .wor và 2 tệp 24.jpg)
2) server:  cùng cây trên  ->  server\maps\特殊用地\
            rồi ĐỔI TÊN mọi  *_Region_C.dat  ->  *_Region_S.dat
```

Lý do đổi tên: `KRegion::LoadObject` (`KRegion.cpp:157`) chỉ mở `Region_S.dat`, **không có nhánh lui về `_C`** (nhánh lui chỉ có ở bộ nạp lưới A* `KSubWorld.cpp:236-340`). Vì đoạn `OBSTACLE` của hai bản là **một định dạng**, đổi tên là đủ — không phải viết bộ chuyển đổi, không phải sửa/biên dịch lại engine.

> `<id>.fp` (client) và `<id>_srv.fp` (server) **tự sinh lần chạy đầu** rồi ghi cache (`KSubWorld.cpp:2189-2213` và `:3397`) — **không phải chuẩn bị**.

**Cách đo lại** (kịch bản trong scratchpad): `hashtest.py` kiểm định hàm băm tên tệp pak — **trúng 300/300** trên đường dẫn `.spr` lấy từ `magicscript.txt`; `findmap2.py` quét 46 pak; `region2.py`/`obs.py` thống kê 6 đoạn dữ liệu; `tex.py` rút tên ảnh nền.

> ⚠️ Bẫy đã vấp trong lúc đo: hàm băm `KPakList::FileNameToId` là số học **unsigned long 32 bit** — phép cộng **wrap trước** rồi mới `% 0x8000000b`. Viết bằng Python phải `((i + idx*c) & 0xFFFFFFFF) % 0x8000000B`, thiếu mặt nạ là sai toàn bộ.

---

## 4. NHỮNG CHỖ CÒN CẦN CHỦ GAME QUYẾT

*(mục 1 và 2 đã chốt 26/08: **làm Viêm Đế trước**; hai tính năng ép đồ làm **đủ như bản gốc**, chấp nhận build lại engine + client; ép Hoàng Kim dùng **hệ Đồ phổ đúng bản gốc**)*

1. **Hệ “mảnh Hoàng Kim”** (595 công thức, gom 4/6/9 mảnh): bật hay bỏ? Bỏ thì chỉ còn 96 món ghép thẳng được.
2. **21 Đồ phổ** bảng có mà bảng item JX1 không có: bỏ hay thêm item mới?
3. **Xúc xắc chia đồ Viêm Đế**: thay bằng trao ngẫu nhiên cho 1 thành viên — có chấp nhận không?
4. **Phòng chờ Viêm Đế**: dùng 1 phòng tĩnh chung (bỏ phó bản động) — có chấp nhận không?
5. **Cân bằng**: trần chống lạm phát theo ngày của lò rèn (`compound_header.lua`) có giữ không?
6. **Viêm Đế**: 17 vật phẩm JX1 không có (nhóm Đồ Phổ Tinh Sương / Đằng Long rơi từ boss cuối) — thay bằng gì?

---

## 5. THỨ TỰ LÀM (đã chốt)

1. **Viêm Đế Bảo Tàng** ← đang làm. Bản đồ đã thông; việc còn lại: chép bản đồ, khai 31 NPC template, đổi task 200, tra 36 mã vật phẩm, port 26 tệp kịch bản.
2. **Ép đồ Hoàng Kim môn phái** (hệ Đồ phổ đúng bản gốc).
3. **Ép đồ tím** (dùng chung nền với bước 2).

---

## 5. PHỤ LỤC — LỆNH ĐO ĐÃ CHẠY

```
python D:\GAMEDEVNEW\ReverseTools\port_3hd\recon_tinhnang.py "item\compound"
python D:\GAMEDEVNEW\ReverseTools\port_3hd\recon_tinhnang.py "missions\yandibaozang"
```

Kịch bản kiểm định riêng viết trong phiên này (thư mục scratchpad):
`d.py` (giải mã tệp Linux) · `it.py` (tra bảng item JX1 theo tên) · `atl.py` (soát nguyên liệu `atlas_compound.txt`) · `audit_atlas.py` + `audit2.py` (soát cột Đồ phổ / món sinh ra / mảnh) · `str.py` (rút chuỗi từ `jx_linux_y`).
