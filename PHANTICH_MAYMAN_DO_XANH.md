# PHÂN TÍCH — MAY MẮN (LUCKY) QUYẾT ĐỊNH ĐỒ XANH NHƯ THẾ NÀO: BẢN LINUX GỐC ĐỐI CHIẾU BẢN DỰ ÁN

Ngày: 20/08/2026 · **CHỈ PHÂN TÍCH — chưa sửa gì, chưa build, chưa commit.**

Câu hỏi: (1) dịch ngược cách tính tỉ lệ may mắn rớt trang bị xanh ở bản Linux; (2) so với hiện trạng dự án; (3) mang bao nhiêu may mắn thì rớt bao nhiêu **dòng**, dòng đó **mạnh cỡ nào**.

> **Đường dẫn dùng trong tài liệu này**
> | Vai trò | Đường dẫn |
> |---|---|
> | Mã nguồn C++ của dự án | `D:\GAMEDEVNEW\Sources` |
> | **Máy chủ đang chạy thật** (script + settings) | `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server` |
> | Bản tham chiếu — GameServer Linux | `D:\ServerLinux\server1` (ELF 32-bit `jx_linux_y`, **không có section header**) |
> | Công cụ dịch ngược | `D:\GAMEDEVNEW\ReverseTools\re_disasm.py` (capstone 5.0.7) |

---

## 0. KẾT LUẬN NGẮN (đọc mục này là đủ để quyết)

1. **Bản gốc chỉ có MỘT cơ chế may mắn duy nhất, và nó nằm trong `Gen_MagicAttrib`.** Không có bảng dữ liệu nào ánh xạ "may mắn → số dòng". Engine quay một số `nDecide`, rồi **chỉ giữ những thuộc tính có `DropRate > nDecide`**. May mắn làm **mẫu số** của `nDecide` to lên ⇒ ngưỡng tụt xuống ⇒ nhiều thuộc tính lọt hơn ⇒ (a) dòng hiếm/mạnh có cơ hội, (b) xác suất "hết ứng viên → dừng sinh dòng" giảm. **Số dòng là hệ quả, không phải tham số.**

2. **Bản Linux có BA công thức `nDecide`, chọn theo `nGameVersion`** — đã dịch ngược tới từng byte tại `Gen_MagicAttrib` **VA `0x0806AF70`**:

   | `nGameVersion` | Công thức `nDecide` | VA nhánh |
   |---|---|---|
   | ≤ 1 | `g_Random(100) / (1 + mayman/5)` | `0x0806B33C` |
   | 2 – 3 | `g_Random(1000000) / (1 + mayman/5)` | `0x0806B075` |
   | > 3 | `(100 × g_Random(1000000)) / (10×mayman + 100)` | `0x0806B31D` |

3. 🔴 **Đây là lỗi gốc rễ của dự án: chạy BẢNG của phiên bản 4 bằng SỐ HỌC của phiên bản 1.**
   `magicattrib.txt` của dự án **chính là** `D:\ServerLinux\server1\settings\item\004\magicattrib.txt` (330/330 bản ghi trùng 11 cột đầu, chỉ bỏ cột 24 "mặt nạ" và tắt 4 dòng) — tức bảng thang **phần-triệu**. Nhưng `ITEM_VERSION = 1` (`GameDataDef.h:15`) và mã đang chạy là nhánh ver ≤ 1 (`GetRandomNumber(0,99) / (1 + nLucky*20/100)`), tức thang **phần-trăm**. `nDecide` tối đa **99**, còn `DropRate` nhỏ nhất trong bảng dự án là **100**.
   ⇒ Điều kiện `m_DropRate[nType] <= nDecide` **không bao giờ đúng**. **Toàn bộ cơ chế may mắn gốc bị vô hiệu 100 %.** Cột `DropRate` chỉ còn là cờ bật/tắt theo ô trang bị.

4. **Bù lại, dự án tự viết cơ chế mới**: hai thang bậc cứng trong `KNpc::DropRateItem` (may mắn → **số dòng**, may mắn → **cấp ma thuật**) cộng bốn nhóm "cổng chỉ số" trong `Gen_MagicAttrib`. Đây mới là thứ đang chạy — xem Bảng 1/2/3.

5. 🔴 **Đường rớt đồ cũ đã chết.** `KNpc::LoseSingleItem` (`KNpc.cpp:8216`) nằm sau điều kiện `!g_NotAddNpcNormal` (`KNpc.cpp:7987`), mà máy chủ chạy thật đặt `NotAddNpcNormal=1` (`settings/gamesetting.ini:259`). **Không bao giờ chạy.** Ai đọc nhầm hàm này sẽ ra bảng số hoàn toàn khác (xem mục 9 để đối chiếu).

6. **Trần số dòng: bản Linux 6, bản dự án 6 (thực tế) / 8 (mảng) / 16 (biến đếm vòng lặp)** — ba con số lệch nhau, là mầm tràn mảng. Xem mục 8, lỗi #4.

---

## 1. ĐƯỜNG ĐI THẬT CỦA MỘT MÓN ĐỒ XANH (bản dự án, máy chủ đang chạy)

```
Quái chết
   └─ script/global/LuaNpcMonsters/Droprate_normal.lua
        ├─ boss xanh (nGoldNpc==1)  :100  DropRateItem(npc, 8, "...goldennpc/npcdroprateXX.ini", 1, nLevel, nSeries)
        └─ quái thường              :117  nSelT = random(0,30)
                                          nSelT==2  -> DropNpcMoney
                                          nSelT< 4  -> DropRateItem(npc, 1, "...npcdroprateXX.ini", 1, nLevel, nSeries)
                                          => chỉ 3/31 ≈ 9,7 % lượt mới gọi sinh đồ
   └─ ScriptFuns.cpp:3762  LuaDropRateItem   (tên Lua "DropRateItem", đăng ký :13052)
   └─ KNpc.cpp:8739        KNpc::DropRateItem
        ├─ nLuck        = Player[nBelongIdx].m_nCurLucky              :8772
        ├─ nLevel       (cấp món 1..10, từ MinItemLevel/MaxItemLevel) :8779-8784
        ├─ nLuckySoDong / nTotalLucky                                  :8786-8810   ← 🔴 có lỗi, xem mục 8
        ├─ nSlMagicTT   = SỐ DÒNG        (BẢNG 1)                      :8812-8947
        ├─ vòng j: nLevelMagic = CẤP của dòng j (BẢNG 2)               :8961-9193
        │           kẹp: nLevelMagic > nLevel && nLevel < 6 -> lấy nLevel :9194-9197
        └─ ItemSet.AddItemSet2(..., pnMagicLevel, ...)                 :9214
   └─ KItemSet.cpp:213     Gen_Equipment
   └─ KItemGenerator.CPP:575  Gen_MagicAttrib   ← chọn ra DÒNG CỤ THỂ (mục 5)
```

Ghi chú: tham số thứ 5 `nItemLevel` mà Lua truyền vào **không được dùng** trong thân hàm; cấp món tính lại từ file `.ini`. Hai biến `nMaxLevelMagic`/`nMinLevelMagic` (`:8776-8777`) được tính rồi bỏ.

---

## 2. MAY MẮN CỦA NGƯỜI CHƠI ĐẾN TỪ ĐÂU

`nLuck` dùng khi rớt đồ = `Player[i].m_nCurLucky`, **không có trần trên lẫn dưới**.

| Nguồn | Giá trị | Vị trí |
|---|---|---|
| Nền `m_nLucky` | tạo nhân vật lấy từ `m_cNewPlayerAttribute.m_nLucky[Series]`; lưu DB | `KPlayer.cpp:2047` |
| Thuộc tính trang bị (PropKind **135** = `magic_lucky_v`) | mỗi món **+1 … +10** | `KNpcAttribModify.cpp:684-690`, `KMagicAttrib.h:142` |
| Cấp Bang | **+2 điểm mỗi cấp**, cấp 1-10 ⇒ tối đa **+20** | `KPlayer.cpp:2824-2834` |
| Buff `lucky_v` | xem bảng dưới | `settings/skills.txt` + script cấp kỹ năng |

**Năm kỹ năng `lucky_v` trong bảng của dự án** (giá trị thật lấy từ `GetSkillLevelData` trong script, **ghi đè** cột `Param1` của `skills.txt`):

| ID | Tên | Cộng may mắn | Thời gian | Script cấp |
|---|---|---|---|---|
| 410 | Bình chủng May mắn | **+30** | 32400 tick = 30 phút | `script\skill\special\404.lua:14` |
| 450 | Quế Hoa Tửu | **+20** (cả tổ đội) | 32400 = 30 phút | `script\skill\zhongqiu\<Quế Hoa Tửu>.lua:21` |
| 441 | Thiên Sơn Bảo Lộ | **+10** | 64800 = 1 giờ | `script\skill\special\tianshanyulu.lua:4` |
| 444 | Phúc Tinh Cao Chiếu | **+5** | 32400 = 30 phút | `script\skill\special\fuxinggaozhao.lua:4` |
| 460 | Hoa Đăng | 🔴 **+0** | 64800 | `script\skill\special\star1.lua:24` — **buff chết** |

*(18 tick = 1 giây.)*

**Phía Lua chỉ có duy nhất `GetLucky`** (`ScriptFuns.cpp:13276` → `LuaGetPlayerLucky`, đọc `m_nCurLucky`). Không có `SetLucky`/`AddLucky`. Hàm C++ `KPlayer::AddBaseLucky` (`KPlayer.cpp:2270`) **không có nơi nào gọi** — mã chết.

⚠️ Bẫy tên: `T_MayMan = 99` trong script Dã Tẩu là **biến nhiệm vụ**, không dính gì tới Lucky.

---

## 3. BẢNG 1 — MAY MẮN → **SỐ DÒNG** (`nLuckySoDong` → `nSlMagicTT`)

`mm = g_Random(100)` ∈ [0,99], quay **một lần cho cả món**. Nguồn: `KNpc.cpp:8812-8947`.

| May mắn | 0 dòng | 1 dòng | 2 dòng | 3 dòng | 4 dòng | 5 dòng | 6 dòng | TB dòng |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 0 – 1 | **25 %** | 30 % | 30 % | 15 % | – | – | – | 1,35 |
| 2 – 5 | 22 % | 25 % | 33 % | 15 % | 5 % | – | – | 1,56 |
| 6 – 9 | – | 13 % | **62 %** ⚑ | 15 % | 10 % | – | – | 2,22 |
| 10 – 13 | – | 25 % | 40 % | 15 % | 15 % | 5 % | – | 2,35 |
| 14 – 17 | – | 10 % | 40 % | 25 % | 15 % | 10 % | – | 2,75 |
| 18 – 21 | – | 10 % | 35 % | 25 % | 20 % | 10 % | – | 2,85 |
| 22 – 25 | – | 10 % | 30 % | 25 % | 20 % | 10 % | 5 % | 3,05 |
| 26 – 29 | – | – | 20 % | 35 % | 25 % | 15 % | 5 % | 3,50 |
| 30 – 33 | – | – | 13 % | 35 % | 25 % | 20 % | 7 % | 3,73 |
| **34 – 99** | – | – | – | – | **70 %** ⚑ | 15 % | 15 % | 4,45 |
| **≥ 100** | – | – | – | – | – | 30 % | **70 %** | 5,70 |

⚑ = nhánh trùng lặp trong mã (hai điều kiện liền nhau trả cùng giá trị), xem lỗi #2.

**Đọc bảng này thế nào**
- Từ **may mắn ≥ 6** trở đi **không bao giờ ra món trắng** (0 dòng) nữa.
- **May mắn 34 → 99 cho kết quả y hệt nhau.** 65 điểm may mắn ở khoảng này hoàn toàn vô ích cho số dòng.
- Muốn 6 dòng ổn định phải đạt **may mắn ≥ 100**.
- Số dòng **tối đa tuyệt đối là 6**, dù mảng có 8/16 ô.

---

## 4. BẢNG 2 — MAY MẮN → **CẤP MA THUẬT CỦA TỪNG DÒNG** (`nTotalLucky` → `nLevelMagic`)

`mmopt = g_Random(100)` được quay **lại cho MỖI dòng** (`KNpc.cpp:8961`, nằm trong vòng `j`) ⇒ mỗi dòng có cấp độc lập. Nguồn: `KNpc.cpp:8962-9193`. Cấp chạy 1…10, khớp cột "đẳng cấp yêu cầu" của `magicattrib.txt`.

| May mắn | c1 | c2 | c3 | c4 | c5 | c6 | c7 | c8 | c9 | c10 | TB cấp |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 0 – 3 | 10 % | 20 % | 40 % | 20 % | 10 % | – | – | – | – | – | 3,00 |
| 4 – 7 | 5 % | 20 % | 40 % | 20 % | 10 % | 5 % | – | – | – | – | 3,25 |
| 8 – 11 | – | 10 % | 30 % | 35 % | 15 % | 10 % | – | – | – | – | 3,85 |
| 12 – 15 | – | 5 % | 20 % | 35 % | 20 % | **20 %** ⚑ | – | – | – | – | 4,30 |
| 16 – 19 | – | – | 10 % | 20 % | 20 % | **50 %** ⚑ | – | – | – | – | 5,10 |
| 20 – 23 | – | – | 5 % | 15 % | 20 % | 20 % | 20 % | 20 % | – | – | 5,95 |
| 24 – 27 | – | – | – | 15 % | 25 % | 30 % | 15 % | 10 % | 5 % | – | 5,95 |
| 28 – 31 | – | – | – | 5 % | 15 % | 20 % | 20 % | 20 % | **20 %** ⚑ | – | 6,95 |
| 32 – 35 | – | – | – | – | 5 % | 20 % | 30 % | 20 % | **25 %** ⚑ | – | 7,40 |
| 36 – 39 | – | – | – | – | 5 % | 25 % | 20 % | 20 % | 25 % | 5 % | 8,01 |
| 40 – 43 | – | – | – | – | – | 15 % | 20 % | 20 % | 39 % | 6 % | 8,01 |
| 44 – 47 | – | – | – | – | – | 10 % | 20 % | 20 % | 40 % | 10 % | 8,20 |
| 48 – 51 | – | – | – | – | – | 5 % | 20 % | 20 % | 43 % | 12 % | 8,37 |
| 52 – 55 | – | – | – | – | – | – | 20 % | 20 % | 45 % | 15 % | 8,55 |
| 56 – 59 | – | – | – | – | – | – | 15 % | 20 % | 45 % | 20 % | 8,70 |
| **60 – 119** | – | – | – | – | – | – | – | – | 40 % | **60 %** | 9,60 |
| **120 – 149** 🔴 | – | – | – | – | – | – | 10 % | 20 % | 40 % | 30 % | **8,90** |
| **≥ 150** | – | – | – | – | – | – | – | – | 20 % | 80 % | 9,80 |

⚑ = nhánh trùng lặp. 🔴 = **thụt lùi**: may mắn 120-149 cho kết quả **kém hơn** khoảng 60-119 (xem lỗi #3).

**Kẹp theo cấp món** (`KNpc.cpp:9194-9197`):
```cpp
if (nLevelMagic > nLevel && nLevel < 6)
    pnMagicLevel[j] = nLevel;        // món cấp thấp không đeo nổi dòng cấp cao
else
    pnMagicLevel[j] = nLevelMagic;
```
⇒ Món **cấp 1-5** bị kẹp cấp dòng ≤ cấp món. Món **cấp ≥ 6** không bị kẹp — cấp 6 vẫn có thể ra dòng cấp 10.

---

## 5. TỪ "CẤP MA THUẬT" RA DÒNG CỤ THỂ — `Gen_MagicAttrib` (`KItemGenerator.CPP:575-855`)

Với mỗi ô `i` (tối đa vòng lặp chạy `MAX_ITEM_MAGICLEVEL` = 16 lần):

1. `GetCMIT(1 - (i&1), nType, nSeriesReq, pnaryMALevel[i])` — lấy danh sách ứng viên theo **tiền tố/hậu tố xen kẽ**, loại trang bị, hệ, và **cấp**. Cấp ngoài 1..10 ⇒ trả `NULL` ⇒ **`break`, dừng sinh dòng**.
2. `nDecide = GetRandomNumber(0, 99) / (1 + nLucky*20/100)`
   *(trình biên dịch rút gọn `nLucky*20/100` = `nLucky/5`; đây đúng là nhánh **ver ≤ 1** của bản Linux)*
3. Bỏ ứng viên nếu `m_DropRate[nType] <= nDecide` — 🔴 **không bao giờ xảy ra** (xem mục 0.3).
4. **Cửa hiếm cố định** (dự án tự thêm), độc lập với may mắn:

   | PropKind | Ý nghĩa | Cửa |
   |---|---|---|
   | 135 | Tăng May mắn | `GetRandomNumber(0,79) != 0 → bỏ` ⇒ **1/80** |
   | 136 | Hút Sinh lực | 1/20 |
   | 137 | Hút Nội lực | 1/15 |
   | 106 | Giảm/miễn đóng băng | 1/15 |
   | 104 | Bỏ qua sát thương vật lý | 1/15 |
   | 114 | Tăng phòng thủ | 1/16 |
   | 115 | Tăng tốc độ công kích | 1/13 |

5. **Cổng theo may mắn** — bốn nhóm, chặn theo **chỉ số bản ghi** `nMAIndex` (Bảng 3).
6. Loại trùng `nPropKind` với các ô đã chọn.
7. Bốc ngẫu nhiên **đều** trong danh sách còn lại; giá trị `nValue[0..2]` = `GetRandomNumber(min, max)` — 🔴 **may mắn KHÔNG ảnh hưởng giá trị**, chỉ ảnh hưởng *chọn dòng nào*.

> **Ngoại lệ duy nhất may mắn ăn thẳng vào giá trị**: nhánh `nPoint != 0` của `Gen_Equipment` (`KItemGenerator.CPP:376-419`), dùng cho đồ tạo sẵn (shop/bot/script), nội suy tuyến tính
> `giá trị = min + (max - min) × nLucky / 10`, với `nLucky` kẹp 0…10. Đường rớt đồ từ quái **không** đi qua nhánh này.

**Ánh xạ chỉ số ↔ dòng tệp** (đã kiểm): `LoadRecord` cộng 2 (`KBasPropTbl.CPP:87`) ⇒ **bản ghi `i` = dòng `i+2`** của `magicattrib.txt`.

---

## 6. BẢNG 3 — MAY MẮN **MỞ KHOÁ ĐƯỢC DÒNG NÀO**

### 6.1 PropKind **135 — Tăng May mắn** (bản ghi 223…232 = May mắn +1…+10), cửa 1/80

| May mắn đang mang | Cổng trong mã (`KItemGenerator.CPP:756-784`) | Mở khoá tối đa |
|---|---|---|
| < 4 | `nMAIndex > 224 → bỏ` | **May mắn +2** (rec 224, cấp 2) |
| 4 – 10 | `> 225` | +3 |
| 11 – 20 | `> 226` | +4 |
| 21 – 30 | `> 227` | +5 |
| 31 – 50 | `> 228` | +6 |
| 51 – 60 | `> 229` | +7 |
| 61 – 70 | `> 230` | +8 |
| 71 – 80 | `> 231` | +9 |
| **≥ 81** | `> 232` | **+10 (đủ thang)** |
| ≥ 91 | `> 233` | *(vô tác dụng — rec 233 là "Hỏa Tinh", PropKind 134)* |

Tên đầy đủ: 223 Vận khí(+1) · 224 Số may(+2) · 225 Ngẫu nhiên(+3) · 226 Cơ hội(+4) · 227 Vận mệnh(+5) · 228 Gặp may(+6) · 229 Khéo léo(+7) · 230 Hạnh phúc(+8) · 231 Ngạo mạn(+9) · 232 Vua Thuận(+10).
Cấp yêu cầu của mỗi bản ghi **bằng đúng** giá trị cộng ⇒ muốn "May mắn +10" phải **đồng thời** có dòng cấp 10 (Bảng 2) **và** may mắn ≥ 81 **và** trúng cửa 1/80. Chỉ roll được trên **Nhẫn / Dây chuyền / Ngọc bội**.

### 6.2 PropKind **114 — Tăng phòng thủ** (rec 53…62, giá trị 1-2 → 18-20), cửa 1/16

| May mắn | Cổng | Cấp dòng tối đa |
|---|---|---|
| < 4 | `> 54` | 2 |
| 4 – 10 | `> 55` | 3 |
| 11 – 20 | `> 56` | 4 |
| 21 – 30 | `> 57` | 5 |
| 31 – 50 | `> 58` | 6 |
| 51 – 60 | `> 59` | 7 |
| 61 – 70 | `> 60` | 8 |
| 71 – 80 | `> 61` | 9 |
| **≥ 81** | `> 62` | **10 (18-20 phòng thủ)** |

### 6.3 PropKind **104 — Bỏ qua sát thương vật lý** (rec 179…183), cửa 1/15

| May mắn | Cổng | Mở khoá tối đa |
|---|---|---|
| < 11 | `> 180` | Huyền nhã, cấp 4 (5-10 %) |
| 11 – 30 | `> 181` | Thấp thoáng, cấp 6 (10-15 %) |
| 31 – 50 | `> 182` | Hư ảo, cấp 8 (15-20 %) |
| **≥ 51** | `> 183` | **Linh hồn, cấp 10 (20-25 %)** |

### 6.4 PropKind **106 — Giảm / miễn đóng băng** (rec 236, 237, 238), cửa 1/15

| May mắn | Cổng | Mở khoá tối đa |
|---|---|---|
| < 11 | `> 237` | Nhiệt tình, cấp 6 (giảm 30) |
| **≥ 11** | `> 238` | **Hỏa nhiệt, cấp 10 — "không thể đóng băng"** |

> ⚠️ **Nghi vấn lệch 1 chỉ số:** các hằng số trong mã (`224`, `54`, `180`, `237`…) trông như được viết theo **số thứ tự dòng** của tệp, trong khi `GetMARecord` dùng **chỉ số bản ghi 0-based** (lệch 2 dòng). Bảng trên là **hành vi thực tế đã đối chiếu tệp**, không phải ý đồ thiết kế. Nếu tác giả định "khoá tới dòng 224" thì thang thực đang mở sớm hơn ý đồ. Chưa có bằng chứng văn bản về ý đồ ⇒ ghi nhận là **suy đoán**.

---

## 7. BẢN LINUX GỐC — KẾT QUẢ DỊCH NGƯỢC

### 7.1 Định vị

Bằng chuỗi log gỡ lỗi `[GenMagicAttrib] ...` tại VA `0x8252F10` / `0x8252F6C`, xref `0x0806B367` / `0x0806B2F9` ⇒ hàm `Gen_MagicAttrib` bắt đầu tại **`0x0806AF70`** (prologue `55 89 E5`).

Điểm mấu chốt — tính `nDecide`:

```
0806AFA0  lea  eax, [ebx + ebx*4]       ; ebx = nLucky      -> 5L
0806AFA3  mov  edx, 0x66666667          ; magic chia 5
0806AFAB  lea  eax, [eax + eax + 0x64]  ; -> 10L + 100      (lưu [ebp-0x74])
0806AFB4  imul edx
0806AFC9  sar  edx, 1
0806AFCB  sub  edx, ebx
0806AFCD  add  edx, 1                   ; -> 1 + L/5        (lưu [ebp-0x6C])
...
0806B05B  cmp  dword [ebp+0x20], 3      ; nGameVersion
0806B065  jg   0806B31D                 ; ver > 3
0806B06B  cmp  dword [ebp+0x20], 1
0806B06F  jle  0806B33C                 ; ver <= 1
0806B075  mov  dword [esp], 0xF4240     ; 1 000 000         <- ver 2..3
0806B07C  call 08226AD0                 ; g_Random
0806B086  idiv dword [ebp-0x6C]         ; / (1 + L/5)
---
0806B31D  mov  dword [esp], 0xF4240     ; ver > 3
0806B324  call 08226AD0
0806B329  imul edx, eax, 0x64           ; × 100
0806B331  idiv dword [ebp-0x74]         ; / (10L + 100)
---
0806B33C  mov  dword [esp], 0x64        ; ver <= 1 : g_Random(100)
0806B343  call 08226AD0
0806B348  jmp  0806B081                 ; rồi cũng / (1 + L/5)
```

Trần vòng lặp: `0806B30E  cmp dword [ebp-0x54], 6` ⇒ **tối đa 6 dòng**.

### 7.2 May mắn nhân xác suất bao nhiêu lần

Mẫu số `1 + L/5` (hoặc `(10L+100)/100`) ⇒ hệ số khuếch đại xấp xỉ **1 + may mắn/5** (ver ≤3) hoặc **1 + may mắn/10** (ver >3).

Xác suất **một** thuộc tính vượt bộ lọc `DropRate`:

| DropRate | Công thức | MM 0 | MM 5 | MM 10 | MM 20 | MM 30 | MM 50 | MM 90 |
|---|---|---|---|---|---|---|---|---|
| 20 (`tỉnh táo`) | ver ≤1 | 20 % | 40 % | 60 % | 100 % | 100 % | 100 % | 100 % |
| 50 (`phục sinh`) | ver ≤1 | 50 % | 100 % | 100 % | 100 % | 100 % | 100 % | 100 % |
| 80 (`Con dơi`) | ver ≤1 | 80 % | 100 % | 100 % | 100 % | 100 % | 100 % | 100 % |
| 10 000 (phổ thông) | ver 2-3 | 1 % | 2 % | 3 % | 5 % | 7 % | 11 % | 19 % |
| 100 000 (cao nhất) | ver 2-3 | 10 % | 20 % | 30 % | 50 % | 70 % | 100 % | 100 % |
| 100 000 | ver >3 | 10 % | 15 % | 20 % | 30 % | 40 % | 60 % | 100 % |

**Bảng `settings/item/magicattrib.txt` của Linux có đúng 6 bản ghi (21 ô) với DropRate ≤ 99** — chính là các dòng mà cơ chế may mắn ver ≤1 điều khiển:

| rec | Tên | Cấp | PropKind | DropRate | Trên ô nào |
|---|---|---|---|---|---|
| 54 | tỉnh táo | 4 | 110 | 20 | giới chỉ, dây chuyền, ngọc bội |
| 69 | Con dơi | 2 | 137 | 80 | 2 loại vũ khí |
| 70 | Quỷ hồn | 5 | 137 | 70 | 2 loại vũ khí |
| 71 | Châu chấu | 7 | 137 | 60 | 2 loại vũ khí |
| 72 | Châu chấu | 10 | 137 | 60 | 2 loại vũ khí |
| 117 | phục sinh | 10 | 88 | 50 | cả 10 ô |

### 7.3 Bảng dữ liệu theo phiên bản

Bản Linux giữ **5 bộ bảng** `settings/item/000…004/`, khớp đúng ba nhánh công thức:

| Thư mục | `magicattrib.txt` | Nhóm công thức |
|---|---|---|
| `000`, `001` | 299 dòng, 23 cột | ver ≤ 1 — `g_Random(100)` |
| `002` | 299 dòng, 23 cột | ver 2-3 — `g_Random(1e6)` |
| `003` | 350 dòng, 23 cột | ver 2-3 |
| **`004`** | **331 dòng, 24 cột** | **ver > 3 — `100 × g_Random(1e6)`** |
| *(gốc)* `settings/item/` | 299 dòng, 23 cột | — |

**Bản dự án: 331 dòng, 23 cột.** Đối chiếu từng dòng với `004`: **330/330 bản ghi trùng khít 11 cột đầu**, chỉ khác:
- bỏ cột thứ 24 「面具」 = **mặt nạ** (toàn 0 ở bản Linux, nên bỏ đi vô hại);
- **tắt (đưa DropRate về 0) đúng 4 bản ghi**:

| rec | dòng | Tên | Cấp | Kind | DropRate ở Linux/004 |
|---|---|---|---|---|---|
| 23 | 25 | Đâm xuyên | 10 | 58 | 10000 trên 2 loại vũ khí |
| 24 | 26 | Vĩnh hằng | 10 | 43 | 100 trên 7 ô |
| 51 | 53 | Thái Sư | 5 | 139 | 2 / 1 |
| 52 | 54 | Tông sư | 10 | 139 | 2 / 1 |

DropRate = 0 ⇒ bị loại khỏi `m_CMAIT` ngay lúc nạp (`KBasPropTbl.CPP:249`) ⇒ **bốn dòng này không bao giờ rơi ra nữa**.

### 7.4 Kích thước bản ghi — bằng chứng cho MATF_CBDR

| | Bản dự án | Bản Linux (đo từ ELF) |
|---|---|---|
| stride bản ghi | 296 = `0x128` | **428 = `0x1AC`** (`imul edi, eax, 0x1ac` @ `0x0806B0C4`) |
| `m_nUseFlag` | +`0x124` | +**`0x1A8`** (`mov ecx,[edi+0x1a8]` @ `0x0806B0CE`) |
| `m_szIntro` | `char[128]` | `char[256]` (suy ra) |
| `m_DropRate[]` | `MATF_CBDR = 11` | **12** (suy ra) |

296 + 128 + 4 = 428 — khớp chính xác. ⇒ Bản Linux có **12 ô trang bị** (thêm mặt nạ), dự án khai **11** (`KBasPropTbl.h:170`).

---

## 8. BẢNG ĐỐI CHIẾU TRỰC DIỆN

| Trục | **Bản Linux gốc** | **Bản dự án (đang chạy)** |
|---|---|---|
| Cơ chế may mắn | **Một** — nhân xác suất qua bộ lọc `DropRate` | **Ba** — 2 thang bậc cứng + 4 nhóm cổng chỉ số; bộ lọc gốc **chết** |
| Công thức `nDecide` | 3 nhánh theo `nGameVersion` | 1 nhánh cứng = nhánh ver ≤ 1 |
| Thang `DropRate` | phần-triệu (ver ≥2) / phần-trăm (ver ≤1), **có 21 ô ≤ 99** | phần-triệu, **0 ô ≤ 99** ⇒ bộ lọc vô hiệu |
| Quyết định số dòng | *hệ quả*: hết ứng viên → `break` | *tham số*: bảng 11 nhánh (Bảng 1) |
| Quyết định cấp dòng | do người gọi truyền vào mảng `pnMagicLevel` | bảng 18 nhánh (Bảng 2), quay lại từng dòng |
| Trần số dòng | **6** (`cmp …,6` @ `0x0806B30E`) | 6 (thực tế) / 8 (`sMA[]`) / 16 (biến vòng lặp) |
| Bảng thuộc tính | 5 bộ theo phiên bản, bộ `004` 24 cột | 1 bộ = `004` bỏ cột mặt nạ, tắt 4 dòng |
| Số ô trang bị (`MATF_CBDR`) | 12 | 11 |
| May mắn ăn vào **giá trị** dòng? | Không (chỉ chọn dòng) | Không — trừ nhánh `nPoint != 0` |
| Nguồn may mắn | nền + trang bị (135) + buff | nền + trang bị (135) + **Bang ×2/cấp** + 5 buff |
| API Lua | `GetLucky` / `SetLucky` | **chỉ `GetLucky`** |
| Đường rớt đồ | `LoseSingleItem` phía C++ | **Lua `DropRateItem`**; `LoseSingleItem` bị tắt |

---

## 9. DANH SÁCH LỖI ĐÃ XÁC NHẬN (xếp theo mức nghiêm trọng)

**#1 🔴🔴 Bảng phiên bản 4 chạy bằng số học phiên bản 1 ⇒ cơ chế may mắn gốc chết hoàn toàn.**
`ITEM_VERSION = 1` (`GameDataDef.h:15`) + `magicattrib.txt` là bản `004`. `nDecide` ∈ [0,99], `DropRate` ≥ 100. Điều kiện lọc không bao giờ đúng ⇒ cột `DropRate` mất hết ý nghĩa xác suất, may mắn không tác động vào việc chọn dòng ngoài 4 nhóm cổng tự thêm.
*Cách kiểm nhanh*: `min(DropRate khác 0)` trong tệp dự án = **100**; `max(nDecide)` = **99**.

**#2 🔴🔴 Thưởng may mắn của boss bị ghi đè, và đọc nhầm người chơi** — `KNpc.cpp:8789-8808`
```cpp
if (m_cGold.GetGoldType() == 1)              { nLuckySoDong = nLuck+5;  nTotalLucky = nLuck+10; }  // boss xanh
else if (m_cGold.m_nGoldType >= defNPC_GOLD_TYE){ nLuckySoDong = nLuck+10; nTotalLucky = nLuck+20; } // boss vàng
else                                          nLuckySoDong = nTotalLucky = nLuck;

if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentExpSkillsVip == 2)
     { nLuckySoDong = nLuck+150; nTotalLucky = nLuck+200; }
else   nLuckySoDong = nTotalLucky = nLuck;      // <-- XOÁ SẠCH thưởng boss ở trên
```
- Nhánh `else` cuối chạy **mọi lúc VIP≠2** ⇒ thưởng boss xanh/boss vàng là **mã chết 100 %**.
- `CLIENT_PLAYER_INDEX = 1` (`KPlayerDef.h:24`) là hằng **phía client**. Trên máy chủ nó trỏ tới **người chơi ở khe 1**, không phải người giết quái ⇒ khi người đó bật x2, **toàn server** được +150/+200 may mắn; khi họ thoát, không ai được gì.
- Còn `printf("Debug: Lucky: %d - Dong: %d\n", ...)` (`:8810`) in ra stdout mỗi lần rớt đồ.

**#3 🔴 Ba vùng chết / thụt lùi trong hai thang bậc**
- Số dòng: **may mắn 34-99 hoàn toàn giống nhau** (Bảng 1).
- Cấp dòng: **may mắn 120-149 kém hơn 60-119** (TB 8,90 so với 9,60) — do nhánh `else` cuối đặt sau `< 120`.
- Bốn nhánh trùng lặp vô nghĩa: `mm<62 → 2 / mm<87 → 2` (`:8843-8846`), `mm<65 → 4 / else → 4` (`:8943-8946`), `mmopt<95 → 6 / else → 6` (`:9011-9014`), `mmopt<97 → 9 / else → 9`.

**#4 🔴 Đọc lố mảng ngăn xếp ở đường `LoseSingleItem`** — `KNpc.cpp:8241` khai `int pnMagicLevel[6]`, nhưng:
- `Gen_Equipment` `memcpy(..., pnaryMALevel, sizeof(int) * MAX_ITEM_MAGICATTRIB)` = **8 int** (`KItemGenerator.CPP:295`);
- `Gen_MagicAttrib` đọc `pnaryMALevel[MAX_ITEM_MAGICATTRIB]` = **phần tử thứ 9** (`:590`); nếu rác khác 0 thì rẽ nhầm sang nhánh "thuộc tính chỉ định sẵn" và sinh ra món hỏng.
Hiện **không phát tác** vì `LoseSingleItem` đang bị tắt (lỗi #5), nhưng sẽ nổ ngay nếu ai đó đặt `NotAddNpcNormal=0`.
*(Đường sống `DropRateItem` khai đúng `pnMagicLevel[MAX_ITEM_MAGICLEVEL]` = 16 ⇒ an toàn.)*

**#5 🟡 Hai bản chép, một bản chết** — `KNpc::GetMagicLevel` (`:8553`) và `KNpc::GetLevelMagic` (`:8626`) là bản tách hàm của đúng hai thang bậc trên, nhưng **không có nơi nào gọi** (đã grep toàn `Sources`). Ai sửa cân bằng ở đây sẽ không thấy game đổi gì. Phải sửa bản **inline** trong `DropRateItem`.

**#6 🟡 `nMagicRate` đọc rồi bỏ** — `KNpcTemplate.cpp:20` nạp khoá `MagicRate` từ `droprate/*.ini` (mọi tệp đều đặt `1`, bản Linux có 20 tệp đặt `0`) nhưng **không nơi nào dùng**. Ở bản Linux đây là công tắc "bàn rớt này có sinh đồ xanh hay không"; ở dự án nó vô hiệu.

**#7 🟡 `genXOpt` vô hiệu** — `KItemSet.h:52-98`. Máy chủ đặt `MaxOptMultiply=1` (`gamesetting.ini:260`) ⇒ mọi nhánh đều trả **1**. Toàn bộ thang `nLuck ≥ 11/21/31/41 → 10/30/50/80 %` không có tác dụng.

**#8 🟡 Lỗi Lua trong đường rớt đồ** — `Droprate_normal.lua:115` gọi `DropNpcMoney(NpcIndex, ...)` trong khi biến đúng là `nNpcIndex` (dùng ở mọi dòng khác) ⇒ biến toàn cục `NpcIndex` chưa gán.

**#9 🟡 Buff Hoa Đăng (skill 460) cộng 0 may mắn** — `script\skill\special\star1.lua:24` trả `Param2String(0, 64800, 0)`.

**#10 ⚪ Rủi ro lệch client/server** — client **không** nhận thuộc tính đã sinh; nó nhận `uRandomSeed` + mảng cấp + `nLuck` + `nVersion` rồi **chạy lại chính `Gen_MagicAttrib`**. Mọi thay đổi ở hàm này, ở `magicattrib.txt`, hay ở `GetCMIT` mà client không có bản tương ứng sẽ khiến thuộc tính hiển thị **lệch hoàn toàn** so với thuộc tính thật. Chú thích tại `KItemGenerator.CPP:301` (*"fix khong get dc nluck tai client dan den sai option do xanh chatitem"*) chính là bản vá cho đúng lỗi này ở đường khoe đồ lên chat.

---

## 10. ĐƯỜNG ĐÃ CHẾT — `KNpc::LoseSingleItem` (giữ lại để đối chiếu)

Chỉ sống lại nếu đặt `NotAddNpcNormal=0`. Cơ chế **khác hẳn** và gần với bản gốc JX1 hơn:

- **Số dòng** = phân phối hình học: mỗi ô trong 6 ô đi tiếp với xác suất `(1 + nLuck)/(2 + nLuck)`, gặp thất bại thì dừng (`:8322`).
- **Cấp dòng**: mọi dòng **cùng một cấp**, lấy từ thang `:8277-8311`, rồi **kẹp cứng bởi biến cục bộ `int MAX_MAGIC_LEVEL = 6;`** (`:8218`).

| May mắn | 0 dòng | 1 | 2 | 3 | 4 | 5 | 6 | TB | Cấp |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---|
| 0 | 50,0 % | 25,0 % | 12,5 % | 6,2 % | 3,1 % | 1,6 % | 1,6 % | 0,66 | ngẫu nhiên 0/1/2 |
| 5 | 14,3 % | 12,2 % | 10,5 % | 9,0 % | 7,7 % | 6,6 % | 39,7 % | 2,41 | ngẫu nhiên 0/1/2 |
| 10 | 8,3 % | 7,6 % | 7,0 % | 6,4 % | 5,9 % | 5,4 % | 59,3 % | 2,98 | ngẫu nhiên 0/1/2 |
| 11 | 7,7 % | 7,1 % | 6,6 % | 6,1 % | 5,6 % | 5,2 % | 61,9 % | 4,58 | 3 |
| 21 | 4,3 % | 4,2 % | 4,0 % | 3,8 % | 3,6 % | 3,5 % | 76,6 % | 5,15 | 4 |
| 31 | 3,0 % | 2,9 % | 2,8 % | 2,8 % | 2,7 % | 2,6 % | 83,1 % | 5,39 | 5 |
| 41 | 2,3 % | 2,3 % | 2,2 % | 2,2 % | 2,1 % | 2,1 % | 86,8 % | 5,53 | 6 |
| ≥ 41 | … | | | | | | | | **luôn 6 — trần cứng** |

Hai đặc điểm đáng chú ý:
- Ở **may mắn 0-10**, `nMagicLevel = g_Random(3)` có thể ra **0** ⇒ `GetCMIT(...,0)` trả `NULL` ⇒ **1/3 số món là đồ trắng** bất kể bốc được mấy ô.
- Các mệnh đề kẹp `if (nMagicLevel < X) nMagicLevel = X;` khiến mọi khoảng từ 11 trở lên trở nên **tất định** (11-20 luôn cấp 3, 21-30 luôn 4, …), triệt tiêu tính ngẫu nhiên mà `g_Random` định tạo ra.

---

## 11. ĐỘ TIN CẬY & NHỮNG GÌ CHƯA KIỂM ĐƯỢC

**Đã kiểm tới byte / tới dòng:**
- Toàn bộ `KNpc.cpp:8216-9263`, `KItemGenerator.CPP:277-855`, `KBasPropTbl.CPP:84-120, 233-360, 1899-1960`, `KItemSet.cpp:150-240`, `KItemSet.h:52-98`.
- Dịch ngược `jx_linux_y`: `Gen_MagicAttrib` @ `0x0806AF70` (prologue, ba nhánh `nDecide`, trần vòng lặp 6, stride bản ghi `0x1AC`), `Gen_Equipment` @ `0x0806B3A0`, hàm nạp `droprate/*.ini` @ `0x080A3B80`.
- Đối chiếu **331 dòng × 23 cột** của `magicattrib.txt` dự án với 5 bộ bảng Linux — xác định nguồn gốc là `004`.
- Khảo sát 139 tệp `droprate/*.ini` bản Linux và 40 tệp bản dự án.
- Tất cả giá trị `Param1`/`Param2` của 5 kỹ năng `lucky_v`, đọc cả `skills.txt` lẫn script cấp kỹ năng.
- Grep toàn cây xác nhận `AddBaseLucky`, `GetMagicLevel`, `GetLevelMagic` không có nơi gọi.

**Chưa kiểm được / cần thận trọng:**
- **Chưa chạy thử trong game** — mọi con số đều từ đọc mã và bảng, chưa có mẫu thực nghiệm. Nên bật lại `printf` ở `KNpc.cpp:8810` (hoặc ghi log riêng) rồi giết 1.000 quái để đối chiếu Bảng 1/2.
- **`nGameVersion` thật của máy chủ Linux**: mới xác định được nó nằm ở một trường của đối tượng ItemGenerator (`[this+0x5A4C]`, đọc tại `0x0806B41D`), **chưa truy được giá trị khởi tạo**. Suy đoán mạnh là **4** vì bộ bảng `004` là bộ duy nhất có 24 cột và trùng khít với bảng dự án — nhưng **chưa chứng minh**.
- **Đường rớt đồ (`LoseSingleItem` tương đương) trong ELF Linux chưa định vị được.** Đã thử: đi ngược đồ thị lời gọi từ `Gen_MagicAttrib` 4 mức; quét toàn `.text` tìm mẫu `g_Random(2 + nLuck)` (**không thấy**); quét `idiv [reg+0x14]`/`[reg+0x18]` cho `MinItemLevelScale`/`MaxItemLevelScale` (**không thấy**). Kết luận **tạm thời**: bản Linux 2022 đã viết lại phần này. Chưa đủ bằng chứng để khẳng định.
- **Nghi vấn lệch 1 chỉ số ở Bảng 3** (mục 6, khung cảnh báo) — hành vi thực tế đã đối chiếu tệp, nhưng ý đồ thiết kế thì không có bằng chứng.
- Hướng khảo sát *client* và toàn bộ khâu **phản biện đối kháng tự động** của workflow bị huỷ giữa chừng do hết hạn mức phiên; các khẳng định trong tài liệu này đều đã được **kiểm lại thủ công**, nhưng chưa qua vòng phản biện độc lập.

---

## PHỤ LỤC — ĐỊA CHỈ & VỊ TRÍ HAY DÙNG

| Thứ | Nơi |
|---|---|
| Thang may mắn → số dòng | `Sources\Core\Src\KNpc.cpp:8812-8947` |
| Thang may mắn → cấp dòng | `Sources\Core\Src\KNpc.cpp:8961-9197` |
| Bốn nhóm cổng may mắn | `Sources\Core\Src\KItemGenerator.CPP:686-786` |
| Công thức `nDecide` (dự án) | `Sources\Core\Src\KItemGenerator.CPP:631` |
| Bản gốc bị chú thích (thang phần-triệu) | `Sources\Core\Src\KItemGenerator.CPP:427-573` |
| Nạp bảng thuộc tính | `Sources\Core\Src\KBasPropTbl.CPP:1911-1960`; ánh xạ dòng `:87` (`nRow += 2`) |
| Hằng số | `GameDataDef.h:15,37,38` · `KBasPropTbl.h:170-173` · `KPlayerDef.h:24` |
| Bảng thuộc tính (chạy thật) | `<bin\server>\settings\item\magicattrib.txt` |
| Bảng rớt đồ (chạy thật) | `<bin\server>\settings\droprate\*.ini` |
| Script rớt đồ (chạy thật) | `<bin\server>\script\global\LuaNpcMonsters\Droprate_normal.lua` |
| `Gen_MagicAttrib` bản Linux | `jx_linux_y` VA `0x0806AF70` |
| Bảng thuộc tính bản Linux theo phiên bản | `D:\ServerLinux\server1\settings\item\000..004\magicattrib.txt` |
