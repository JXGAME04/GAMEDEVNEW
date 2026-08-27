# Đặc tả hệ XÚC XẮC chia đồ (DICEITEM) — dịch ngược từ bản Linux

> Nguồn: `D:\ServerLinux\server1\jx_linux_y` (ELF 32-bit, 8.931.808 B, bảng
> section đã bị gỡ ⇒ ánh xạ địa chỉ qua program header).
> Công cụ: `ReverseTools/viemde/pak_id.py`, scratchpad `elf.py` + `xdis.py` (capstone 5.0.7).
>
> **Cách đọc tài liệu này**: mọi số hiệu offset trong struct của bản Linux chỉ để
> *chứng minh* ý nghĩa trường. Khi cài sang JX1 phải ánh xạ **theo NGHĨA** sang
> `KItem`/`KPlayer` của JX1, tuyệt đối không bê offset.

---

## 1. Vì sao bắt buộc phải có

`YDBZ_award` (`npc_death.lua:20`) rải thưởng theo bảng `YDBZ_tbaward_item`.
Cột 4 của mỗi dòng quyết định cách rơi:

| Cột 4 | Cách rơi |
|---|---|
| `0` | `DropItem(...)` — rơi xuống đất, ai nhặt trước được |
| `1` | **`YDBZ_DiceDice(...)`** — mở cửa sổ xúc xắc cho cả tổ đội |

Bảng thưởng (`head.lua:133`):

- loại 2 (boss ải nhỏ): `{50,1,{6,1,1605,...},1,"Hình nhân",1}` → **50% mỗi lần**
- loại 3 (boss tranh đoạt): `{100,1,{6,1,1605,...},1,"Hình nhân",1}` → **100%**

⇒ Xúc xắc chạy ở **mọi** boss của Viêm Đế. Không phải nhánh phụ.

---

## 2. Bảng đăng ký trong ELF

Một khối liền 8 mục ở offset tệp `0x882a40`–`0x882a78` (mục 8 byte
`{const char* ten, lua_CFunction ham}`):

| Tên Lua | Địa chỉ hàm | Viêm Đế dùng |
|---|---|---|
| `ApplyItemDice` | `0x81c1c40` | ✔ |
| `AddDiceItemInfo` | `0x81c1a10` | ✔ |
| `RollItem` | `0x81c16d0` | ✔ |
| `GetItemDiceState` | `0x81c05b0` | ✘ (vẫn nên có) |
| `DiceLootItem` | `0x81c14f0` | ✘ (vẫn nên có) |
| `GetItemDiceRollInfo` | `0x81c0b80` | ✔ |
| `GetItemDiceItemInfo` | `0x81c04a0` | ✔ |
| `GetItemDicePlayerList` | `0x81c0380` | ✔ |

Các hàm Lua C API đã nhận diện chắc chắn (đọc thẳng thân hàm):

| Địa chỉ | Là gì | Bằng chứng |
|---|---|---|
| `0x8232490` | `lua_gettop` | trả số, mọi hàm gọi đầu tiên |
| `0x82338b0` | `lua_getnumber(L,i)` → st0 | trả double |
| `0x8233850` | `lua_getstring(L,i)` | trả con trỏ |
| `0x8232be0` | `lua_newtable` | đặt thẻ **4** (LUA_T_ARRAY) vào ô đỉnh |
| `0x8232d40` | `lua_pushnumber(double)` | đặt thẻ **2** (LUA_T_NUMBER) |
| `0x8232e40` | `lua_pushnumber((double)(int))` | `fild` rồi gọi `0x8232d40` |
| `0x82337a0` | `lua_pushstring` | |
| `0x8233430` | `lua_settable(L, idx)` | luôn gọi với `-3` |

---

## 3. Hợp đồng từng hàm

### 3.1 `ApplyItemDice(nType, nMaxRoll, nTime, szFile, szDoneFunc, szCallbackFunc, nPlayerCount)`

**Trả về**: `nId` (số) — mã phiên xúc xắc. Trả **0 giá trị** nếu `lua_gettop() <= 5`.

Đọc tham số (đúng thứ tự trong mã):

| # | Kiểu đọc bằng | Ý nghĩa |
|---|---|---|
| 1 | `lua_getnumber` | `nType` — Viêm Đế và Cổ Tháp đều truyền **1** |
| 2 | `lua_getnumber` | `nMaxRoll` — đều truyền **100** |
| 3 | `lua_getnumber` | `nTime` — giây; Viêm Đế **20** |
| 4 | `lua_getstring` | `szFile` — tệp kịch bản chứa 2 hàm gọi lại |
| 5 | `lua_getstring` | `szDoneFunc` — gọi khi chốt xong |
| 6 | `lua_getstring` | `szCallbackFunc` — gọi mỗi lần một người gieo; Viêm Đế truyền `""` |
| 7 | `lua_getnumber` | `nPlayerCount` |

Thân hàm:

```
nId    = DiceSet_Alloc(g_DiceSet, 2)        ; 0x81ce730, tham so 2 la HANG SO trong ma
pDice  = DiceSet_Find(g_DiceSet, nId)       ; 0x81ce540 ; null -> tra 0 gia tri
pDice->Setup((nType << 16) | nMaxRoll, nPlayerCount)   ; 0x81ceef0
pDice->vtbl[0x1c](nTime)                    ; ham ao: dat dong ho dem nguoc
strncpy(pDice + 0x7c, szFile,          79)  ; 80 byte
strncpy(pDice + 0x3c, szDoneFunc,      31)  ; 32 byte
strncpy(pDice + 0x5c, szCallbackFunc,  31)  ; 32 byte
lua_pushnumber(nId); return 1
```

`Setup` (`0x81ceef0`) — đã đọc kỹ, đây là chỗ chốt nghĩa hai trường:

```
if ((WORD)packed == 0)      return   ; nMaxRoll = 0  -> bo
if ((packed >> 16) == 0)    return   ; nType    = 0  -> bo
pDice->[0x30] = packed                              ; (nType<<16)|nMaxRoll
pDice->[0x28] = (nPlayerCount > 0) ? nPlayerCount : 0x7fffffff
if (nPlayerCount > 0x1fffffff) throw                ; chan tran
reserve(vector nguoi choi, nPlayerCount * 8 byte)   ; +0x1c .. +0x24
```

> 🔑 **`pDice->[0x28]` = số người dự kiến** — chính là `nSize` mà `saizi.lua` dùng.
> Đây là bằng chứng dùng để bắt lỗi ở mục 3.5.

### 3.2 `AddDiceItemInfo(nId, nQuality, nGenre, nDetail, nParticular, nLevel, nSeries, nRandomSeed, nLucky)`

Đòi `lua_gettop() >= 9`; **đọc đúng 9 tham số đầu**, thừa thì bỏ qua
(Viêm Đế truyền 12 — 3 tham số cuối là rác, không ảnh hưởng).

Thân hàm: dựng vật phẩm thật (`0x806e110`), gắn vào phiên
(`0x81ce460(pDice, nItemIndex)`), `lua_pushnumber(nItemIndex)`, trả 1.

Bên gọi phải kiểm `nItemIndex <= 0` là thất bại (`roll_item.lua:40` làm đúng vậy).

### 3.3 `RollItem(nId)`

Chạy với `PlayerIndex` = người sẽ nhận cửa sổ.

```
nPlayer = PlayerIndex                      ; 0x8107910
pDice   = DiceSet_Find(g_DiceSet, nId)
pDice->AddPlayer(nPlayer)                  ; 0x81cf430
nItemIndex = pDice->GetItem()              ; 0x81ce490 ; <=0 -> khong gui
pItem   = g_ItemArray + nItemIndex * 0x368
<dung goi 208 byte roi gui>                ; 0x80a8400
```

**Gói gửi client** (dựng tại `ebp-0x3ad`, gửi dài `size + 1` = **208** byte):

| Lệch | Cỡ | Giá trị | Nghĩa |
|---|---|---|---|
| 0 | 1 | `0x63` | mã giao thức |
| 1 | 2 | `0xBF + 0x10` = `0xCF` | độ dài phần sau |
| 3 | 1 | `0` | |
| 4 | 1 | `0x1D` | **nhánh con** (29) |
| 5 | 1 | `0` | |
| 6 | 1 | `0` | |
| 7 | 1 | `1` | |
| 13 | 4 | `0xBF` (191) | cỡ khối vật phẩm |
| 17.. | 191 | 0 rồi ghi đè | khối mô tả vật phẩm |
| 18 | | `0x81f9430(dst, pItem)` | tuần tự hoá vật phẩm |
| 202 | 2 | `pPlayer->vtbl[0x20]()` | |
| 204 | 4 | (biến cục bộ) | |

> ⚠️ Khối 191 byte là **định dạng vật phẩm của JX2**. JX1 có định dạng riêng
> (`KItem::GetDataToBuffer`). Khi port **phải dùng định dạng JX1**, không bê
> 191 byte này — nếu không client JX1 đọc ra rác.

### 3.4 `GetItemDiceItemInfo(nId)` → **8 giá trị**

Thứ tự đẩy (đọc thẳng mã, `esi = g_ItemArray + nItemIndex * 0x368`):

| # | Nguồn | Tên trong `saizi.lua` |
|---|---|---|
| 1 | `nItemIndex` | `nItemIndex` |
| 2 | chuỗi tại `pItem+0x2c` | `szItem` |
| 3 | `pItem+0x04` | `quality` |
| 4 | `pItem+0x00` | `nGenre` |
| 5 | `pItem+0x08` | `nDetial` |
| 6 | `pItem+0x0c` | `nPart` |
| 7 | `pItem+0x24` | `nLevel` |
| 8 | `pItem+0x28` | `nSeries` |

Trả **0 giá trị** nếu: `gettop()<=0`, không tìm thấy phiên, hoặc `nItemIndex==0`.

### 3.5 `GetItemDiceRollInfo(nId)` → **(bảng, nSize, nWinner)** — ⚠️ bản Linux SAI

Thân hàm đẩy lên ngăn xếp, theo đúng thứ tự:

1. `lua_newtable()` → bảng kết quả `t`
2. duyệt **hai** danh sách (`0x81ce270` = người đã gieo, `0x81ce450` = phần còn lại),
   mỗi phần tử dựng một bảng con rồi `t[i] = bảng con`:

   | Khoá | Giá trị | Nguồn |
   |---|---|---|
   | 1 | tên nhân vật | `pRole + 0x1505` |
   | 2 | `nNumber` | `pNode + 0x14` |
   | 3 | trạng thái | hằng `1` ở nhánh này, `0` ở nhánh kia |
   | 4 | có phải người thắng | `pNode->[0x10] == pDice->[0xec] ? 1 : 0` |
   | 5 | `nPlayerIndex` | tra từ id người chơi |

3. `lua_pushnumber(pDice->[0x28])`  ← **nSize** (số người dự kiến, xem 3.1)
4. `lua_pushnumber(pDice->[0xec])`  ← **nWinner**
5. `return 2`   ← **`b8 02 00 00 00` tại `0x81c0c65`, đã kiểm byte thô**

**Lỗi**: Lua 4.0 lấy **n giá trị trên đỉnh**. Ngăn xếp là `[t, nSize, nWinner]`,
trả 2 ⇒ script nhận `(nSize, nWinner)`, **bảng `t` bị rơi mất**.

Mà `saizi.lua:29` viết `local t, nSize = GetItemDiceRollInfo(dwID)` rồi
`for index, value in t do` ⇒ `t` là **số** ⇒ Lua báo lỗi và cả hàm
`YDBZ_show_roll_info` dừng giữa chừng: người chơi **không bao giờ thấy bảng
điểm gieo**.

Đã loại trừ khả năng tôi đọc nhầm:

- `[ebp-0x34]` giữ `pDice` suốt hàm, không bị ghi đè (kiểm mọi lần ghi).
- `0x8232be0` đúng là `lua_newtable` (đặt thẻ 4) — cùng hàm mà
  `GetItemDicePlayerList` dùng rồi `return 1`, và script nhận được **bảng**.
- Byte thô tại `0x81c0c62`: `83 c4 4c b8 02 00 00 00` = `add esp,0x4c; mov eax,2`.

> **Quyết định khi port**: cài **trả 3 giá trị** `(t, nSize, nWinner)`.
> Cách này vừa khớp ý đồ tác giả, vừa **tương thích ngược** với chính
> `saizi.lua` (`local t, nSize = ...` lấy đúng 2 giá trị đầu). Đây là chỗ
> **cố ý lệch bản Linux** và là chỗ duy nhất; ghi rõ ở đây để chủ game quyết.

### 3.6 `GetItemDicePlayerList(nId)` → **1 bảng**

```
count = (pDice->[0x20] - pDice->[0x1c]) / 8
lua_newtable()
for i = 0 .. count-1:
    id  = pDice->PlayerAt(i)               ; 0x81ce2f0
    idx = PlayerSet_IdToIndex(id)          ; 0x80c61a0
    if (unsigned)(idx - 1) > 0x4ae: bo qua ; 0x4ae = 1198 -> chi nhan 1..1199
    t[++n] = idx
return 1
```

⇒ **chỉ liệt kê người còn online**; `getn(t)` có thể nhỏ hơn `nSize` — đúng
như `saizi.lua` kiểm `nSize ~= getn(t)` để in "Người chơi khác về quá hạn".

### 3.7 `GetItemDiceState(nId)` → 1 số  ·  `DiceLootItem(...)`

Cả hai Viêm Đế không dùng. Vẫn nên cài để giữ đủ API cho các tính năng khác
(Cổ Tháp bang hội dùng cùng hệ này).

---

## 4. Vòng đời (từ `roll_item.lua` — bản tự tài liệu hoá rõ nhất)

```
1. nId = ApplyItemDice(1, 100, nTime, szFile, szDone, szCallback, nPlayerCount)
2. nItemIndex = AddDiceItemInfo(nId, ...)        -- tao vat pham that
   (tuy chon) ITEM_SetExpiredTime / SetItemBindState tren nItemIndex
3. voi tung nguoi: PlayerIndex = X; RollItem(nId)   -- mo cua so tren may X
4. nguoi choi bam "Cần" hoac "Hủy bỏ"
   -> may chu gieo 1..nMaxRoll
   -> goi szCallbackFunc(nId, nNumber) trong szFile, PlayerIndex = nguoi gieo
5. het gio HOAC moi nguoi da chon
   -> chon nguoi diem cao nhat
   -> goi szDoneFunc(nId, nWinner, nNumber) trong szFile
```

`nWinner <= 0` nghĩa là **mọi người đều hủy bỏ** (`roll_item.lua:70` thoát sớm).

---

## 5. Việc phải làm ở JX1

| Phần | Nội dung |
|---|---|
| Engine máy chủ | lớp `KItemDice` + `KItemDiceSet`, đồng hồ đếm ngược, chọn người thắng |
| ScriptFuns | đăng ký 8 hàm Lua trên |
| Giao thức | 1 gói s2c (mở cửa sổ) + 1 gói c2s (gieo / hủy) |
| Client | lớp cửa sổ gắn `投色子.ini` + `投色子-公用.ini` (đã chép nguyên văn) |

**Ảnh giao diện**: cả 3 đường dẫn ảnh của 2 tệp `.ini` đều **có sẵn** trong
`data\updatejx06.pak` của client đích — không phải làm gì thêm.
Xem `BANGIAO_VIEMDE_2608.md` mục 10.3.

**Quy trình thêm giao thức**: bắt buộc theo memory `protocol-add-order` —
cây LIVE `J:\CayChay\NangLen64bit\SwordOnline0_03_05\SwordOnline` là chuẩn,
chỉ đọc, và **cả 4 tệp** phải khớp vị trí với LIVE.

**Chưa làm**: `BuildATeam` (`readymap/include.lua:357`) — chỉ nằm trong
nhánh báo danh cũ, mà nhánh đó **chết cả ở bản Linux** (không nơi nào gọi
`OpenMission(51)`), nên không cài.
## 6. Vòng đời THẬT trong máy — đọc từ mã máy

### 6.1 Bố cục đối tượng phiên (chỉ để chứng minh nghĩa, KHÔNG bê sang JX1)

| Lệch | Nghĩa | Bằng chứng |
|---|---|---|
| `+0x18` | số người **đã chọn xong** | so với `+0x28` ở nhịp đếm |
| `+0x1c`/`+0x20`/`+0x24` | vector người chơi (mỗi mục 8 byte) | `(0x20−0x1c)/8` = số người |
| `+0x28` | **nSize** — số người dự kiến | `Setup` ghi vào đây |
| `+0x2c` | **mã phiên** | `Alloc` chép từ bộ đếm của bộ quản lý |
| `+0x30` | `(nType << 16) \| nMaxRoll` | `Setup` |
| `+0x34` | **trạng thái** | `DiceLootItem` so `== 2`; `GetItemDiceState` đọc |
| `+0x38` | **giây còn lại** | nhịp đếm trừ 1 mỗi lần |
| `+0x3c` | tên hàm **chốt** (32 B) | `strncpy` trong `ApplyItemDice` |
| `+0x5c` | tên hàm **gọi lại** (32 B) | như trên |
| `+0x7c` | tên **tệp kịch bản** (80 B) | như trên |
| `+0xec` | **người thắng** | `GetItemDiceRollInfo` so với id người chơi |

Lớp phiên **kế thừa một lớp hẹn giờ kịch bản** của engine (chuỗi RTTI quanh
`0x8268c48` đọc ra `…KScriptTim…`) — lớp cha giữ 3 chuỗi tên và bộ đếm giây.

### 6.2 Nhịp đếm — `0x81cee60`

```
nGiayConLai = pDice->[0x38] - 1;   pDice->[0x38] = nGiayConLai;
if (pDice->[0x28] == pDice->[0x18]   // moi nguoi da chon xong
    || nGiayConLai == 0)             // hoac het gio
{
    if (pDice->[0x7c][0] && pDice->[0x3c][0])   // co ten tep VA ten ham chot
    {
        snprintf(buf, 0x27, "%u", pDice->[0x2c]);        // 0x804b5ac, chuoi tai 0x824fb8f
        pScript = LoadScript(&pDice[0x7c]);              // 0x821de70
        CallScriptFunc(pScript, &pDice[0x3c], buf);      // 0x80f9020
    }
}
pDice->vtbl[0x10]();   // dong bo trang thai xuong client
```

⇒ **chốt sớm khi mọi người đã chọn**, không phải chờ hết giờ.

### 6.3 🔴 Hàm chốt chỉ nhận MỘT tham số — lỗi thứ hai của bản Linux

Đường chốt dựng chuỗi bằng `"%u"` (một giá trị: mã phiên) rồi gọi qua `0x80f9020`
— helper nhận **danh sách tham số dạng chuỗi**.

Đối chiếu với đường **gọi lại** (`0x81cf390`), engine dùng helper khác:

```
0x80aced0(pPlayer, &pDice[0x7c], &pDice[0x5c], 0, "dd", nId, nNumber)
                                                    ^^^^
                            chuoi KIEU tham so: hai so nguyen
```

`"dd"` (tại `0x825597c`) chứng minh engine **thừa sức truyền nhiều tham số** —
nên việc hàm chốt chỉ truyền một là **lỗi**, không phải giới hạn kỹ thuật.

Hệ quả trên bản Linux:

| Kịch bản | Hậu quả |
|---|---|
| `roll_item.lua` — `Done(nWinner, nNumber)` làm `if nWinner <= 0` | `nil <= 0` ⇒ **Lua 4 báo lỗi**, hàm chốt của Cổ Tháp đứt |
| `saizi.lua` — `nWinner == value[5]` | luôn **false** ⇒ nhánh thưởng "Viêm Đế Lệnh hiệu lực" **không bao giờ chạy** |

Cộng với lỗi `return 2` ở mục 3.5, phần hiển thị kết quả gieo của bản Linux **hỏng**.

### 6.4 Ai trao vật phẩm cho người thắng

**Không kịch bản nào trao.** `roll_item.lua:Done` chỉ đặt trạng thái khoá, đọc thuộc
tính, nhắn tin và ghi nhật ký; `saizi.lua` chỉ in tin. `DiceLootItem` là hàm Lua để
**kịch bản tự đòi**, nhưng cả Viêm Đế lẫn Cổ Tháp đều không gọi.

⇒ **engine phải là bên trao**, lúc chốt phiên.

---

## 7. Quyết định khi port sang JX1

Ba chỗ **cố ý lệch bản Linux**, đều là sửa lỗi, đều tương thích ngược với chính
kịch bản gốc:

| # | Bản Linux | Bản JX1 | Vì sao |
|---|---|---|---|
| 1 | `GetItemDiceRollInfo` trả **2** (mất bảng) | trả **3**: `(t, nSize, nWinner)` | `local t, nSize = …` lấy đúng 2 giá trị đầu ⇒ chạy đúng như tác giả viết |
| 2 | `szDoneFunc(nId)` | `szDoneFunc(nId, nWinner, nNumber)` | đúng chữ ký cả hai kịch bản; engine vốn truyền được nhiều tham số (`"dd"`) |
| 3 | — | engine **trao vật phẩm** cho người thắng lúc chốt | không kịch bản nào trao; đây là hành vi duy nhất hợp lý |

Giữ **nguyên** bản Linux ở mọi chỗ khác, đặc biệt:

- `szCallbackFunc(nId, nNumber)` — 2 tham số, chạy với `PlayerIndex` = người vừa gieo.
- Chốt **sớm** khi mọi người đã chọn, không chờ hết giờ.
- `GetItemDicePlayerList` chỉ liệt kê người **còn online**, nên `getn(t)` có thể nhỏ
  hơn `nSize` — kịch bản dựa vào đúng điều này để in "Người chơi khác về quá hạn".
- `AddDiceItemInfo` đọc **đúng 9 tham số đầu**, thừa thì bỏ (Viêm Đế truyền 12).
- `ApplyItemDice` đòi `lua_gettop() >= 6`; `AddDiceItemInfo` đòi `>= 9`.

### 7.1 Trao vật phẩm — theo đúng lối JX1

Bắt chước `LuaAddItem` (`ScriptFuns.cpp:4833`):

```cpp
int nIndex = ItemSet.AddItemSet2(nItemClass, nSeries, nLevel, nLuck,
                nDetailType, nParticularType, nItemLevel,
                g_SubWorldSet.GetGameVersion(), 0, nStackNum, nEnChance,
                nPoint, nYear, nMonth, nDay, nHour, bLock, sLock, xOpt);
// xOpt = ItemSet.genXOpt(nLuck)
```

Lúc chốt, trao cho người thắng đúng cách JX1 vẫn làm: thử
`m_ItemList.CheckCanPlaceInEquipment(...)` → `AddKIL(nIndex, pos_equiproom, x, y)`;
đầy thì đẩy thứ đang cầm xuống đất (`ObjSet.Add` + `SetItemBelong`) rồi
`AddKIL(nIndex, pos_hand, 0, 0)`.

> ⚠️ Vật phẩm được tạo ở `AddDiceItemInfo` và **chưa thuộc về ai** trong suốt phiên.
> Nếu phiên bị huỷ mà không có người thắng thì **phải giải phóng** để khỏi rò chỉ số
> vật phẩm. Đây là chỗ dễ rò nhất của cả tính năng.
