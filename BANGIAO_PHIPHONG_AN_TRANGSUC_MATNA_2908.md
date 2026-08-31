# BÀN GIAO — PORT HỆ PHI PHONG / ẤN / TRANG SỨC / MẶT NẠ TỪ BẢN LINUX SANG JX1

> Lập 29/08/2026. Đọc hết mục 1–4 trước khi gõ dòng mã đầu tiên.
> Trạng thái: **GĐ1 xong · thiết kế đã khoá bằng số đo · chưa viết mã engine.**

---

## 1. QUYẾT ĐỊNH CỦA CHỦ GAME (29/08)

| Việc | Chốt |
|---|---|
| Đồ cũ ở 4 ô | **Xoá thẳng, không đền bù** |
| Phạm vi phi phong | **Port đủ 2 nhánh, bật hết** (tăng sao · đột phá thường · Vô Cực · đột phá lần 2 · khảm đá) |
| Sửa engine | **Duyệt** — nhưng phải sao lưu trước, và **báo trước khi swap** |
| Giao diện | **Phải port như bản Linux** |

Sao lưu đã làm: `D:\_BACKUP_TRUOC_PHIPHONG_20260829\role.bin`
— 1003 nhân vật, 12,6 MB, `sha256 = 8a60ac98a2469a98248db12cdae02f9a648d2d99fcc66e308761ac192cc950bc`.

---

## 2. GĐ1 — ĐO KHO NHÂN VẬT THẬT (xong)

Kho: MySQL `jx1_role.role` — **1003 nhân vật**, ~50.000 bản ghi vật phẩm.
(`role_history` 5,7 GB là bảng ảnh chụp, không phải dữ liệu sống.)

### 2.1 Bản ghi vật phẩm = 233 byte, khép kín bằng số học

`TDBItemData` ở `D:\GAMEDEVNEW\lib\S3DBInterface.h:177`, `pack(1)`,
`MAX_ITEM_MAGICLEVEL = MAX_ITEM_MAGICATTRIB * 2 = 16` (`GameDataDef.h:37-38`):

```
0   iid              4   iequipclasscode = GENRE   8   ilocal    12 ix   16 iy
20  iequipcode       24  idetailtype               28 iparticulartype   32 ilevel
36  iseries          40  iequipversion             44 irandseed
48  iparam[16] ...................................................... 111
112 ilucky  116 iidentify  120 idurability  124 igoldid  128 istacknum
132 ienchance  136 ipoint  140 iyear  144 imonth  148 iday  152 ihour
156 ilockbh  160 igiomokhoa
164 iiduphong1 = phi phong (GetMantle)     <- DA DUNG
168 iiduphong2 = so lan dung item          <- DA DUNG
172 iiduphong3 = GetItemGlowLight          <- DA DUNG
176 iiduphong4 = GetMaxOptMultiply         <- DA DUNG
180 iiduphong5   184 iiduphong6   188 iiduphong7   192 iiduphong8   <- 4 O TRONG
196 iiduphong9 = uPrice (gia bay ban)      <- DA DUNG
200 iBaiTanPrice  204 ilocksell  208 ilocktrade  212 ilockdrop
216 imantle(1B)  217 ifortune  221 iowner  225 irow  229 iequipnaturecode
                                                              = 233 byte
```

> ⚠️ Đính chính so với báo cáo miệng lúc đầu: **KHÔNG phải 6 ô trống mà chỉ 4**
> (`iiduphong4` và `iiduphong9` đã có người dùng — `KPlayerDBFuns.cpp:660,711,1071,1072`).

### 2.2 Kết quả đếm

```
Nhân vật đang giữ đồ DetailType 11/12/13/14 : 0
Tổng vật phẩm dính                          : 0
```

⇒ **Xoá bảng cũ an toàn tuyệt đối**, không sinh item ma, không cần bộ dọn roledb.
Công cụ: `…\scratchpad\ktkm\g1_dem.py` (chỉ đọc).

---

## 3. DỊCH NGƯỢC ENGINE LINUX — SỐ LIỆU GỐC

`jx_linux_y` (x86-32, đã cắt bảng section). Bảng đăng ký hàm Lua tại `0x082e36b0`.
Sải bước một vật phẩm trong mảng `Item` = **0x368 = 872 byte**.

### 3.1 Bản đồ trường trên vật phẩm (Linux)

| Offset | Trường | Đọc bởi |
|---|---|---|
| `+0x2A8` | **MaxStoneNum** — số lỗ khảm | `GetEquipMaxStoneNum` @`0x080fc800` |
| `+0x2AC` | **MaxWishValue** — trần điểm chúc phúc | `GetMaxEquipWishValue` @`0x080fcd80` |
| `+0x2B4` | **CurWishValue** — chúc phúc hiện có | `GetCurEquipWishValue` @`0x080fce00` |
| `+0x2B8` | **StarLevel** (= phần tử 0 của mảng) | `GetStarLevel` @`0x080fce80` |
| `+0x2B8 + i*4` | **mã Tinh Thần Thạch** lỗ i | `GetStarStoneOnEquip` @`0x080fca40` |
| `+0x2CC + i*4` | **cấp sao của lỗ** i | `GetStoneLevelOnEquip` @`0x080fc980` |

Tất cả đều là **trường lưu trên vật phẩm**, không phải tra bảng lúc chạy.

### 3.2 Nguồn của MaxStoneNum / MaxWishValue

`goldequip.txt` có **62 cột ở CẢ Linux lẫn JX1** (giống hệt). Ba cột cuối:

- cột 60 `是否可升星装备` — có tăng sao được không
- cột 61 `可镶嵌星辰石数量` — số lỗ khảm → nạp vào `+0x2A8`
- cột 62 `装备突破祝福值` — trần chúc phúc → nạp vào `+0x2AC`

**Đúng 5 dòng trong toàn bảng 5.940 dòng có giá trị:**

| idx | Phi phong | c60 | c61 (lỗ) | c62 (trần chúc phúc) |
|---|---|---|---|---|
| 5374 | Tuyệt Thế | 1 | 1 | 4 |
| 5375 | Phá Quân | 1 | 2 | 6 |
| 5376 | Ngạo Tuyết | 1 | 3 | 8 |
| 5377 | Kinh Lôi | 1 | 4 | 14 |
| 5378 | Ngự Phong | 1 | 5 | *(trống)* |

Khớp chéo với `mantleupgrade_head.lua`: tỉ lệ đột phá 15 / 10 / 8 / 5 %, và Ngự Phong
`eBreakType = "AdvBreak"` nên không có trần chúc phúc. **Không còn tham số nào phải đoán.**

### 3.3 JX1 hiện KHÔNG đọc 3 cột này

Grep `KItemGenerator.cpp` / `KItem.cpp` / `KItem.h`: không có `StoneNum`, `WishValue`,
`StarLevel`. Cột có sẵn trong .txt nhưng engine bỏ qua ⇒ phải thêm phần đọc.

---

## 4. NỀN JX1 ĐÃ TƯƠNG THÍCH SẴN (đo, không đoán)

| Hạng mục | Kết quả |
|---|---|
| Enum ô trang bị | `GameDataDef.h:683-701` — `equip_mask=11, equip_mantle=12, equip_signet=13, equip_shipin=14` **trùng 100% Linux** (JX1 dư thêm 15 `hoods`, 16 `cloak`) |
| Bảng thuộc tính | `magicattrib.txt` **332/332 dòng trùng khít** → copy chỉ số thuộc tính là đúng nghĩa |
| API đã có | `AddGoldItem`, `GetGlodEqIndex`, `GetPlatinaEquipIndex`, `GetPlatinaLevel`, `UpgradePlatinaItem`, `SetItemBindState`, `ITEM_SetExpiredTime`, `GetItemQuality` |
| API thiếu (17) | 12 hàm sao/đá + `AddPlatinaItem`, `UpgradePlatinaFromGoldItem`, `DegradePlatinaItem`, `DisguiseMask`, `SaveMaskFeature`/`GetMaskFeature` |
| `KItem` | đã có `GetMantle/SetMantle`, `m_MaxOptMultiply` (tiền lệ cho biến rời) |
| Giao diện | client JX1 dựng UI bằng **C++**, không có `.ini`. Có sẵn `UiCompoundItem.cpp` (hộp lò rèn 7 khung, 27/08) làm khuôn |

---

## 5. THIẾT KẾ ĐÃ KHOÁ

### 5.1 Lưu trữ — KHÔNG đổi layout (tuân Gate 2)

Đóng gói toàn bộ dữ liệu phi phong vào **4 ô dự phòng có sẵn** `iiduphong5..8`:

```
pack[0] (iiduphong5): bit 0-3   cap sao        0..10
                      bit 4-11  chuc phuc      0..255
                      bit 12-14 so lo kham     0..5
                      bit 15-22 tran chuc phuc 0..255
pack[1] (iiduphong6): 5 x 6 bit  ma Tinh Than Thach tung lo (0 = trong)
pack[2] (iiduphong7): 5 x 4 bit  cap sao tung lo (0..10)
pack[3] (iiduphong8): time_t 32 bit — lan dot pha gan nhat
```

Tổng 97/128 bit. ⇒ `TDBItemData` **giữ nguyên 233 byte**, schema MySQL **không đổi**,
nhân vật cũ đọc bình thường, không phải di trú.

### 5.2 Việc còn lại

| GĐ | Nội dung | Tệp |
|---|---|---|
| 2 | `m_nPfPack[4]` + 14 accessor (bit-pack) | `KItem.h` |
| 3a | Nạp/ghi `iiduphong5..8` | `KPlayerDBFuns.cpp` (~660, ~1071) |
| 3b | Đọc cột 60/61/62 lúc sinh trang bị vàng/bạch kim | `KItemGenerator.cpp` |
| 3c | 17 hàm Lua | `ScriptFuns.cpp` |
| 3d | Áp thuộc tính Tinh Thần Thạch lên nhân vật | `KItem::ApplyMagicAttribToNPC` |
| 4 | Panel tăng sao / đột phá / khảm 6 khung + protocol | `S3Client\Ui\UiCase\` (khuôn `UiCompoundItem.cpp`) |
| 5 | Bộ sinh dữ liệu: xoá D11–14 cũ, bơm bản Linux | 554 gold + 526 platina + 903 mask + 34 tinh thần thạch |
| 6 | Port `mantlesystem\` (1.437 dòng) + `wuxingyin.lua` + `platina_*` | Lua |

### 5.3 Luật thi công bắt buộc

- Nguồn là **ANSI/TCVN3** → chỉ sửa bằng `safe_edit.py` / `vn_edit.py`, **cấm** Edit/Write thường.
  Mốc trước khi sửa: `KItem.h` 97 high-byte · `KPlayerDBFuns.cpp` 88 high-byte · FFFD = 0.
- `Core\Src` biên dịch vào **cả** client Win32 lẫn server x64 → build **cả hai** cấu hình.
- Không đổi `TDBItemData`, không đổi gói tin, không đổi cân bằng ngoài phạm vi.
- Build ra `.moi_2908_phiphong` đặt cạnh, **báo chủ game trước khi swap**.

---

## 6. CẦN LÀM RÕ TRƯỚC KHI BUILD

1. `D:\GAMEDEVNEW\Sources\Core\x64\ServerRelease\CoreServer.dll` có bản build **12:06 ngày 29/08**,
   mới hơn bản đang chạy (11:37) và **khác md5** — chưa rõ của việc gì. Build đè là mất.
2. Lúc kiểm tra, `Goddess` / `Bishop` / `S3Relay` **vẫn đang chạy** (khởi động 27/08 17:47).
   GameServer đã tắt nên build không sao, nhưng **swap DLL thì phải dừng hẳn cụm**.


---

## 7. TIẾN ĐỘ 29/08 (cập nhật)

### 7.1 Engine — XONG, build sạch cả hai cấu hình
`Server Release|x64` và `Client Release|Win32`: **COMPILE PASS (0 error C) + LINK PASS**.

| Tệp | Việc |
|---|---|
| `KItem.h` | `m_nPfPack[4]` + 16 accessor bit-pack |
| `KItem.cpp` | xoá pack trong `Reset()` |
| `KPlayerDBFuns.cpp` | nạp/ghi `iiduphong5..8` |
| `KItemGenerator.cpp` | bộ đọc cột 60/61/62 (cô lập) + gọi ở 2 bộ sinh |
| `KJx2WarInfra.cpp` | 12 hàm Lua phi phong |
| `ScriptFuns.cpp` | khai báo + đăng ký, giữ nguyên tên Linux |

### 7.2 Dữ liệu — XONG (server + client, đều có `.truoc_phiphong_2908`)
`goldequip` 286 dòng · `platinaequip` 37 dòng · `mask.txt` thay nguyên bản Linux 904 dòng
(JX1 đang để trống cột ParticularType) · `mantle/signet/shipin` thay · `starstone.txt` **thêm mới**.
Giữ nguyên số dòng + số cột nên không lệch chỉ số đồ Hoàng Kim đang mặc.

### 7.3 🔴 BẢNG ÁNH XẠ MÃ VẬT PHẨM — nguồn sự thật cho port Lua

Mã Linux **đã bị JX1 dùng cho món khác**, cấm bê thẳng. Đã cấp mã mới (4881–4889) và
thêm vào `magicscript.txt` cả hai phía:

| Linux | JX1 mới | Tên |
|---|---|---|
| `6,1,4546` | **`6,1,4881`** | Tinh Ngọc |
| `6,1,4547` | **`6,1,4882`** | Thiên Tinh Ngọc |
| `6,1,4548` | **`6,1,4883`** | Mảnh Thiên Tinh Ngọc |
| `6,1,4549` | **`6,1,4884`** | Bách Luyện Thành Cương |
| `6,1,4550` | **`6,1,4885`** | Tinh Ngọc Nguyên Thạch |
| `6,1,4551` | **`6,1,4886`** | Tinh Hỏa Than |
| `6,1,4552` | **`6,1,4887`** | Tinh Thần Khoáng |
| `6,1,4553` | **`6,1,4888`** | Tinh Thần Thạch |
| `6,1,2839` | **`6,1,4889`** | Vương Thiết Tượng Lệnh Phù |

### 7.4 Còn thiếu, đã khoanh chính xác

**5 hàm engine** bộ script phi phong cần mà JX1 chưa có:
`AddStackItem` · `AddPlatinaItem` · `GetItemIndexBydwID` · `SetUiGiveItemMsg` · `SetUiGiveItemMoreConfirmMsg`

**3 hàm giao diện** (thuộc GĐ4): `g_GiveItemUI` · `g_GiveMantleInlayUI` · `g_ReturnInlayResult`

**2 include** Linux có mà JX1 không: `\script\global\login_head.lua` · `\script\misc	imeline	imelinemanager.lua`

**Còn lại**: GĐ3d (áp thuộc tính Tinh Thần Thạch) · GĐ4 (panel + protocol) · GĐ6 (port 1.437 dòng Lua, dùng bảng 7.3).


---

## 8. ĐỢT 2 NGÀY 29/08 — GĐ3d + GĐ6 XONG

### 8.1 GĐ3d — thuộc tính Tinh Thần Thạch (engine)
`KItem.cpp`: `PF_GetStoneInfo` (nạp 1 lần, cache) + `KItem::PF_ModifyStoneAttrib(pNPC, bAdd)`
gọi ở **cả hai chiều** — cộng trong `ApplyMagicAttribToNPC`, trừ trong `RemoveMagicAttribFromNPC`.
Quy ước đã đo: `starstone.txt` cột 14 = **chỉ số 0-based** → dòng KTabFile = `ge + 1`.
34/34 viên có dòng `magicattrib_ge` trùng khít Linux ⇒ không nắn mã thuộc tính.

### 8.2 Ba hàm engine mới (`KJx2WarInfra.cpp` + `ScriptFuns.cpp`)
| Hàm | Lý do |
|---|---|
| `AddGoldEquipByRow(nRow)` | **KHÔNG** dùng lại `AddGoldItem`: JX1 đọc `GoldItem.txt`, Linux đọc `goldequip.txt` — khác nghĩa hoàn toàn |
| `AddPlatinaItem(0, nRow [,nLevel])` | JX1 chưa có → giữ đúng tên Linux |
| `GetItemIndexBydwID(dwID)` | đổi dwID client gửi lên → chỉ số vật phẩm |

Đường tạo: `KItemSet::Add(NATURE_GOLD/PLATINA, item_equip, nSeries, nLevel, nLuck, nRow)`
→ `Gen_Equipment` (`KItemGenerator.cpp:1406-1421`) đưa `nRow` thẳng vào bộ sinh rồi `SetRow`.

### 8.3 Hai hàm KHÔNG thêm vào engine — có lý do
`SetUiGiveItemMsg` / `SetUiGiveItemMoreConfirmMsg`: hộp `GiveItemUI` của JX1 gửi
`S2C_GIVE_BOX` chỉ có **3 ô chuỗi** (tiêu đề / nội dung / tên callback) — không còn chỗ cho
chuỗi gửi SAU khi hộp đã mở. Đã định nghĩa **shim Lua** ở cuối `mantleupgrade_head.lua`,
đẩy chữ ra `Msg2Player`. **Đây là lệch duy nhất so với bản Linux ở phần giao diện tăng sao.**

### 8.4 GĐ6 — port 7 tệp Lua (đều có `.truoc_phiphong_2908`)
`mantleupgrade_head/npc` · `item_starore/starstone/tianxingyu` · `other_func_outputs` · `wuxingyin`

- nắn **19 mã vật phẩm** (2 dạng: `{6,1,N}` và `nP == N`)
- nắn **2 lời gọi** `AddGoldItem(0,N)` → `AddGoldEquipByRow(N)`
- bỏ **2 Include** JX1 không có (`login_head`, `timelinemanager` — phần dùng tới chúng vốn đã bị chú thích bên Linux)
- **soát sót: 0 mã cũ còn lại**; `syncheck.exe`: cú pháp Lua 4.0 sạch cả 7 tệp
- byte TCVN3/GBK **không đổi một byte nào** ở mọi tệp

### 8.5 Nối NPC
`script\global\npcchucnang\thoren.lua` (Thợ Rèn): thêm `Include` `mantleupgrade_npc.lua`
+ mục menu **"Tăng cấp Phi Phong/onMantleSystem"**. TCVN3 1766 → 1768 (đúng +2 cho `ă`,`ấ`).

### 8.6 Sự cố trong lúc thi công (đã xử lý)
1. **Tệp khối lẫn chữ Hán** → ghi rỗng → `safe_edit` xoá mất dòng khai báo
   `ApplyMagicAttribToNPC`. Phát hiện ngay, khôi phục có kiểm chứng high-byte.
   Đã thêm `assert` ASCII vào mọi bộ sinh khối.
2. **`error C1853` PCH** kèm `KPlayerPet.cpp` = hai phiên build giẫm chung thư mục obj.
   Xoá `Core.pch` (34 MB, sinh lại được) → build sạch.
3. **Heredoc bash rút `\`** làm biểu thức nắn mã trượt im lặng (`head.lua` báo 0 mã).
   Viết lại bộ sinh ra tệp → 16 → **19 mã**. Bài học cũ, đã dính lại.

### 8.7 CÒN LẠI
**Chỉ còn panel khảm Tinh Thần Thạch** (client C++ + 3 protocol).
4/5 thao tác — tăng sao · đột phá thường · nâng Vô Cực · đột phá lần 2 — **dùng `GiveItemUI`
sẵn có nên đã chạy được**. Mục "Phi Phong khảm Tinh Thần Thạch" hiện gọi
`g_GiveMantleInlayUI()` → giao thức chưa đăng ký → **bấm vào không mở gì** (vô hại).


---

## 9. ĐỢT 3 — GENRE 9 + PHẢN BIỆN

### 9.1 Kết quả phản biện trên phần đã làm

| # | Điểm | Kết luận |
|---|---|---|
| 1 | Ô vật phẩm tái dùng có mang sao/đá cũ? | ✅ Không — `KItemSet::Remove` → `KItem::Remove()` → `Reset()` (đã `memset` pack) |
| 2 | 6 bit đủ chứa mã đá? | ✅ 34 viên, P = 1…34 ≤ 63 |
| 3 | `GetGlodEqIndex` trả 0? | ⚠️ Dương tính giả — bản rỗng `LuaHD3_*` **không được đăng ký**; bản dùng là `LuaCmp_GetGlodEqIndex` (`KItemCompound.cpp:1782`) |
| 4 | Quy ước chỉ số dòng | 🔴 **LỖI THẬT — đã vá.** Bản thật trả `GetRow()+1` ⇒ `AddGoldEquipByRow`/`AddPlatinaItem` phải truyền `nRow-1`, nếu không **tạo nhầm món kế bên trong im lặng** |
| 5 | Số cột `starstone.txt` | 🔴 Bỏ sót cột 25 (`物品最大叠放值`) — đã bổ sung |

### 9.2 Genre 9 — thêm đúng chuẩn Linux (9 tệp, 13 chỗ)

Linux: `7 brokenequip · 8 fusion · 9 starstone`. JX1 trước đó chỉ có 0–6.

Đo trước khi đụng:
- `item_number` chỉ xuất hiện **2 chỗ** toàn nguồn; mảng `szColor[item_number][32]`
  vốn chỉ 5 dòng khởi tạo trên 7 ⇒ nới lên 10 vô hại.
- `KLibOfBPT::Init()` phân nhánh **theo vị trí mảng** (`i<17`, `i==17/18/19/27..31`)
  ⇒ **bắt buộc nối bảng mới vào CUỐI** (thành `i==32`), cấm chèn giữa.

| Tệp | Việc |
|---|---|
| `GameDataDef.h` | `item_brokenequip(7) · item_fusion(8) · item_starstone(9)` |
| `CoreUseNameDef.h` | `TABFILE_STARSTONE` |
| `KBasPropTbl.h` | `KBASICPROP_STARSTONE` (25 cột) + lớp `KBPT_StarStone` + thành viên + accessor |
| `KBasPropTbl.CPP` | ctor/LoadRecord/GetRecord/FindRecord(theo ParticularType) + nối cuối mảng |
| `KItem.h/.cpp` | `operator=(const KBASICPROP_STARSTONE&)` |
| `KItemGenerator.h/.cpp` | `Gen_StarStone(nParticular, pItem, nStackNum)` |
| `KItemSet.cpp` | `case item_starstone` |

Chuỗi khép kín đã kiểm: `item_starore.lua` trao `{9,1,P,1,0,0}` → `AddItemSet2` →
`case item_starstone` → `Gen_StarStone(P)` → `FindRecord(P)`.

**Build: Server x64 + Client Win32 đều COMPILE PASS + LINK PASS. 12/12 tệp FFFD = 0.**

### 9.3 Còn lại duy nhất
Panel khảm phía client. Tài nguyên **đã trích từ client VLTK thật của chủ game**
(`C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky`) và **đã chép vào** `bin/client`:
`Ui/Ui3/星辰石镶嵌.ini` (6.515 B, 14 ô, toạ độ đầy đủ) · `星辰石镶嵌界面.ini` ·
7 tệp `.spr` (nền 100 KB, khung ô, nút đóng, thanh kéo, nút 2 chữ, đế đúc).
Client JX1 đọc **tệp rời** nên **không phải ghi pak**.

Còn phải viết: lớp `KUiMantleInlay` (khuôn `UiCompoundItem.cpp`) + mở panel bằng
`S2C_GIVE_BOX` **case 3** (dùng lại gói sẵn có, **không thêm gói tin mới** — tuân Gate 2).
