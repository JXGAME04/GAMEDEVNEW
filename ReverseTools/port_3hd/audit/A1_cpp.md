# A1 — SOÁT MÃ C++ CỦA ĐỢT PORT 3 HOẠT ĐỘNG (25/08)

| Mục | Nội dung |
|---|---|
| Ngày | 25/08/2026 |
| Vai trò | Hướng soát 1 — mã C++: khối `[3HD 25/08]` trong `KJx2WarInfra.cpp`, `ScriptFuns.cpp`, `KSortScript.cpp` |
| Nguyên tắc | Mọi thứ phiên trước làm bị coi là **SAI** cho tới khi tệp gốc / mã máy / cây sống chứng minh ngược lại |
| Đã làm | Đọc trọn khối `[3HD]` (KJx2WarInfra.cpp:1074-1844), khối đăng ký (ScriptFuns.cpp:15388-15434), 3 hàm IniFile (ScriptFuns.cpp:3247-3308), `g_IsJx2Script` (KSortScript.cpp:113-141); đối chiếu **6 hàm ELF** bằng `re_disasm`; đối chiếu **mọi chỗ gọi thật** trong `src_utf8` + cây sống `E:\SourceTuanLe\...\bin\server`; quét trùng tên toàn bảng `GameScriptFuns[]`; **biên dịch lại** cả 2 cấu hình |
| Kết quả | **0 CHẶN · 10 NẶNG · 15 NHẸ** — và **3 điểm nghi ngờ của đề bài hoá ra ÂM TÍNH** (mục 3, 4, 8 dưới đây) |
| KHÔNG sửa | Không sửa `Sources`, `bin`, `E:\SourceTuanLe`, script, settings. Chỉ đọc + biên dịch `/t:ClCompile` (không link, không PostBuild, không deploy). |

---

## 0. TÓM TẮT MỘT TRANG

**NẶNG (sai hành vi):**

| # | Chỗ | Một dòng |
|---|---|---|
| N1 | `KJx2WarInfra.cpp:1429-1450` | `ITEM_GetExpiredTime` trả **số phút còn lại**; bản gốc trả **giá trị THÔ** của trường hạn dùng → `activity.lua:314` cho ra ngày **19700101** |
| N2 | `KJx2WarInfra.cpp:1344` | `GetNpcAroundPlayerList` **bỏ mất `Npc[nMe].m_Index <= 0`** có trong khuôn gốc → đọc `m_Region[]` ngoài mảng khi NPC đã bị giải phóng |
| N3 | `KJx2WarInfra.cpp:1653-1688` + `forbidmap.lua` | "Vá stub thật `DisabledUseTownP`" **không có tác dụng**: 0 script đọc cờ, và `forbidmap.lua` **chưa hề** được thêm map 3 hoạt động (ngược với chú thích trong mã) |
| N4 | `KJx2WarInfra.cpp:1166-1181` | `AddStatData` ghi vào `bin\server\log_game\` — **thư mục không tồn tại** → thống kê chết im lặng + `fopen` hỏng lặp lại mỗi lời gọi |
| N5 | `ScriptFuns.cpp:3274-3284` | `IniFile_Save` dùng `KIniFile::Save` mà **chính dự án đã cấm** (`KJx2SharedStore.cpp:2-3`); thêm bẫy `MAX_CHUNK=10` → NULL-deref |
| N6 | (nhiều) | **6 hàm mới không có lời gọi thật nào** trong 3 bao đóng (IniFile_SetData/Save, File_Create, GetRoomItems, OpenProgressBar, cụm log vật phẩm) — bảng "mức độ CAO" của `15_bosung_soat_api.md` mục 6 bị thổi phồng |
| N7 | `KJx2WarInfra.cpp:1268-1295` | `GetRoomItems` **vứt bỏ tham số `nRoomType`**, luôn trả túi (+ hành trang mở rộng) |
| N8 | `KJx2WarInfra.cpp:1796-1801` | `BT_GetBattleParam` đẩy **số 0**, bản gốc trả **CHUỖI** → `battlehead.lua:631/639` tách sai |
| N9 | `KJx2WarInfra.cpp:1400,1419` | `ITEM_SetExpiredTime(idx, 0)`: gốc = "không đặt hạn", port = "**hết hạn ngay**" |
| N10 | `ScriptFuns.cpp:15416` | `IsDisabledUseHeart` đăng ký nhưng **0 lời gọi** trong cây sống → "chặn Tâm Tâm Tương Ánh" của Vượt Ải **không tồn tại** |

**Ba nghi ngờ của đề bài — kiểm thật, ÂM TÍNH:**
- **Mục 3** (đăng ký nằm ngoài `#ifdef _SERVER` → client lỗi link): **KHÔNG**, khối đăng ký nằm gọn trong `#ifdef _SERVER` — xem §3.
- **Mục 4** (trùng tên trong `GameScriptFuns[]`): **KHÔNG có tên `[3HD]` nào trùng** — xem §4.
- **Mục 8** (`\script\missions\boss\` đè hệ boss JX1): **KHÔNG**, thư mục đó trên cây sống chỉ có đúng 1 tệp và là tệp Linux vừa chép — xem §8. Nhưng **`\script\vng_feature\` thì CÓ vấn đề** (H1).

---

## 1. AN TOÀN BỘ NHỚ (mục 1 của đề bài)

### 1.1 🔴 N2 — `LuaHD3_GetNpcAroundPlayerList` bỏ mất kiểm "NPC còn sống"

```
KJx2WarInfra.cpp:1344-1345   (bản MỚI)
    if (nMe <= 0 || nMe >= MAX_NPC || nDist <= 0 || Npc[nMe].m_SubWorldIndex < 0
        || Npc[nMe].m_RegionIndex < 0)

KJx2WarInfra.cpp:363         (KHUÔN GỐC LuaGetNpcAroundNpcList, PORT5 23/08)
    if (nMe <= 0 || nMe >= MAX_NPC || Npc[nMe].m_Index <= 0 || nDist <= 0 || Npc[nMe].m_SubWorldIndex < 0)
```

Khuôn gốc có `Npc[nMe].m_Index <= 0` — bản mới **bỏ**. Hệ quả:

- Chỗ gọi duy nhất là `script\missions\boss\bigboss.lua:218`, chạy trong **script CHẾT của NPC boss**. Đây đúng là lúc slot NPC có thể đã/đang được giải phóng.
- Khi slot đã giải phóng, `m_SubWorldIndex` / `m_RegionIndex` là **giá trị cũ hoặc rác**. Hàm chỉ chặn `< 0`, **không chặn `>= pWorld->m_nTotalRegion`**, rồi dùng ngay ở dòng 1351-1352:
  `pWorld->m_Region[Npc[nMe].m_RegionIndex].m_RegionID` ⇒ **đọc ngoài mảng `m_Region[]`**.
- Vòng lặp thân hàm thì vẫn giữ đúng `Npc[i].m_Index <= 0` (dòng 1361) — chỉ thiếu ở chỗ kiểm NPC gốc.

**Việc phải làm:** thêm `Npc[nMe].m_Index <= 0` và (khuyến nghị cho cả khuôn gốc) `Npc[nMe].m_RegionIndex >= pWorld->m_nTotalRegion`.
**Mức: NẶNG.**

### 1.2 Các chỗ còn lại — kiểm biên ĐẠT

| Hàm | Kiểm biên | Kết luận |
|---|---|---|
| `LuaHD3_DropItemEx` :1505 | `nSubWorldIdx` 0..`MAX_SUBWORLD` (:1535), `nIdx<=0` (:1543), `nObj==-1` (:1577), `nBelonger>0 && <MAX_PLAYER` (:1583) | ĐẠT. Không gọi `SetItemBelong(-1)` khi không có chủ **không phải lỗi**: `KObjSet::AddData` đã đặt `Object[].m_nBelong = -1` (`KObjSet.cpp:402`) |
| `LuaHD3_GetRoomItems` :1268 | `nPlayerIndex <= 0` (:1272), `nItemIdx>0 && <MAX_ITEM` (:1285); `KInventory::FindItem` **tự kiểm biên** trả -1 (`KInventory.cpp:153-154`) | ĐẠT. Chú thích ":1265-1266" nói `room_equipmentex` cũng 6×10 — **ĐÚNG** (`KItemList.cpp:879` dùng `REPOSITORY_ROOM_WIDTH/HEIGHT` = 6/10, `GameDataDef.h:374-375`) |
| `LuaHD3_GetFirstPlayerAtServer` / `GetNext` :1301-1327 | `sHD3_NextOnlinePlayer` quét `i < MAX_PLAYER`; `MAX_PLAYER = 1500` (`KPlayerDef.h:19`), `KPlayer Player[MAX_PLAYER]` (`KPlayer.cpp:111`) | ĐẠT |
| `LuaHD3_DisabledUseTownP_Real` :1658 | mảng `static BYTE s_byHD3NoTownP[MAX_PLAYER]` + `s_nHD3NoTownPWorld[MAX_PLAYER]`, chỉ số chặn `>0 && <MAX_PLAYER` (:1661, :1682) | ĐẠT. Hai mảng nằm trong `#ifdef _SERVER` nên **không** dính `MAX_PLAYER = 2` của client (`KPlayerDef.h:21`) |
| `LuaHD3_GetItemAllParams` :1189 | `nItemIdx>0 && <MAX_ITEM`; vòng `i < MAX_ITEM_MAGICLEVEL && i < 6`, mà `MAX_ITEM_MAGICLEVEL = 8*2 = 16` (`GameDataDef.h:37-38`), `nGeneratorLevel[16]` (`KItem.h:112`) | ĐẠT |
| `LuaHD3_ITEM_*` / `SetItemBindState` | đều `nItemIdx>0 && <MAX_ITEM` | ĐẠT |
| `LuaHD3_NpcDropMoney` :1592 | `nIndex>0 && <MAX_NPC`, `nObjIdx>0 && <MAX_OBJECT` | ĐẠT (chặt hơn khuôn gốc `KNpc.cpp:8335`) |
| `LuaHD3_TrimString` :1824 | thao tác trên `s_szStrBuf` — **đúng là biến static CÙNG TỆP** (`KJx2WarInfra.cpp:913`), chung với `LuaPushString/AppendString/ReplaceString/PopString` (:915/924/941/972) | ĐẠT — chú thích ":1822" chính xác, không phải bộ đệm khác |

### 1.3 🟡 H12 — tràn ngăn xếp tiềm ẩn qua `IniFile_SetData` (đường MỚI)

`Engine\Src\KIniFile.cpp`, `KIniFile::SetKeyValue`:
```c
char szSection[32] = "[";          // <<< 32 byte
if (pSection[0] != '[') { g_StrCat(szSection, pSection); g_StrCat(szSection, "]"); }
```
`KIniFile::GetKeyValue` cùng tệp dùng `char szSection[64]`. `LuaIniFile_SetData` (`ScriptFuns.cpp:3255`) truyền **thẳng chuỗi Lua**, không giới hạn độ dài ⇒ tên section ≥ 30 ký tự **ghi đè ngăn xếp**. Trước đợt này Lua chỉ vào được nhánh `[64]` (`IniFile_GetData`); đợt này mở nhánh `[32]`.
**Mức: NHẸ** (chưa có lời gọi thật — xem N6) nhưng nên chặn `strncpy` ≤ 28 ký tự tại `ScriptFuns.cpp:3255`.

---

## 2. CHỮ KÝ SO VỚI BẢN LINUX (mục 2 của đề bài)

Đã đối chiếu **6 hàm bằng mã máy** (`re_disasm` trên `D:\ServerLinux\server1\jx_linux_y`), phần còn lại bằng chỗ gọi thật.

### 2.1 🔴 N1 — `ITEM_GetExpiredTime` SAI ĐƠN VỊ **và** SAI GỐC THỜI GIAN **và** SAI KHI HẾT HẠN

Dịch ngược `ITEM_GetExpiredTime` (`0x08154540`, luamap khớp):
```
0x08154552  cmp eax, 1 / je 0x8154578      ; đúng 1 tham số
0x081545A1  test eax,eax / jle → đẩy 0     ; chỉ số phải > 0
0x081545A5  cmp eax,[0x830ca5c] / jge → 0  ; chặn theo số vật phẩm
0x081545B3  imul eax, eax, 0x368
0x081545B9  mov eax, [eax+edx+0x34c]       ; <<< ĐỌC THÔ trường hạn dùng
0x081545D2  call lua_pushnumber            ; KHÔNG có phép tính nào
```
⇒ **Bản gốc trả nguyên giá trị trường**, không trừ, không đổi ra phút, không zero-hoá khi hết hạn.

Chỗ gọi duy nhất — `script\activitysys\activity.lua:310-323` (bản sống 8.685 B, trùng khít bản Linux `src_utf8/*/activitysys/activity.lua:310-323`):
```lua
local nTime = ITEM_GetExpiredTime(nItemIndex);
if (nTime == nil or nTime <= 0) then return 0; end
local nTm = Time2Tm(nTime);          -- <<< coi nTime là MỐC THỜI GIAN
return nTm[1]*10000 + nTm[2]*100 + nTm[3];
```
`Time2Tm` của JX1 (`KJx2League.cpp:1366-1369`) là `localtime((time_t)tham_số)` — **epoch Unix thật**.

Bản thi công (`KJx2WarInfra.cpp:1442-1449`):
```c
int nExpire = Item[nItemIdx].GetExpireTime();   // KItem.h:345 → 0 KHI ĐÃ HẾT HẠN
if (nExpire <= 0) { Lua_PushNumber(L, 0); return 1; }
Lua_PushNumber(L, (nExpire - KSG_GetCurSec()) / 60);   // <<< SỐ PHÚT
```

Ba lỗi chồng lên nhau:
1. **Đơn vị**: trả phút (vd 1440) thay vì mốc thời gian ⇒ `Time2Tm(1440)` → 01/01/1970 ⇒ `GetItemExpiredTime` trả **19700101**.
2. **Gốc thời gian**: dù có sửa thành "trả mốc" cũng phải **cộng lại 1451581200**, vì `nExpireTime` của JX1 tính theo gốc 2016 (`KSG_StringProcess.cpp:18-23`: `return curtime - 1451581200;`, và `KItem.cpp:2629` cộng lại khi hiển thị).
3. **Dùng `GetExpireTime()`** — hàm này trả 0 khi vật phẩm ĐÃ hết hạn (`KItem.h:345-350`), trùng với giá trị của vật phẩm **không có hạn** ⇒ mất phân biệt. Đúng phải đọc thẳng `m_CommonAttrib.nExpireTime`.
   👉 **`15_bosung_soat_api.md` mục "BS-3" đã cảnh báo đúng chỗ này** ("không bọc thẳng `GetExpireTime()`… phải đọc `m_CommonAttrib.nExpireTime` trực tiếp") — bản thi công **vẫn mắc**.

**Sửa đúng:** `Lua_PushNumber(L, (double)Item[nItemIdx].m_CommonAttrib.nExpireTime + 1451581200.0);` (0 vẫn trả 0).
**Mức: NẶNG.**

### 2.2 🟡 H7 / 🔴 N9 — `ITEM_SetExpiredTime`: ngưỡng ĐÚNG, nhưng thiếu 2 nhánh của bản gốc

Dịch ngược `ITEM_SetExpiredTime` (`0x08154A30`) → nó **không tự đổi đơn vị**, mà gọi helper `0x081F2760(giá_trị, cờ)`; đọc helper:
```
0x081F2771  test esi,esi / je → TRẢ 0            ; <<< giá trị 0  = KHÔNG ĐẶT HẠN
0x081F2775  cmp esi, 0x1312d00                    ; 0x1312D00 = 20 000 000  ✔ ngưỡng ĐÚNG
0x081F277B  jbe 0x81f27c8                         ;  <= 20e6  → nhánh PHÚT
0x081F27E6  imul ebx, esi, 0x3c ; lea ebx,[eax+ebx]; = now + phút*60
--- nhánh ngày (> 20e6) ---
0x081F2787  call 0x820ba80(giá_trị, cờ)          ; YYYYMMDD → mốc
0x081F27AC  cmp ebx, eax / cmovbe ebx, -1        ; <<< mốc <= HIỆN TẠI → TRẢ -1 (không đặt)
```

| Điểm | Bản thi công | Bản gốc | Kết luận |
|---|---|---|---|
| Ngưỡng 20 000 000 | `>= 20000000.0` là ngày (`:1400`) | `> 20 000 000` là ngày (`jbe`) | lệch **đúng 1 giá trị biên** — vô hại (NHẸ) |
| Gốc thời gian | phút: `KSG_GetCurSec() + n*60` (:1419); ngày: `mktime - 1451581200` (:1415) | tương đương | **ĐÚNG** ✔ |
| **Giá trị 0** | rơi vào nhánh phút ⇒ `nExpireTime = now` ⇒ **hết hạn NGAY** | trả 0 ⇒ **không đặt hạn** | 🔴 **N9 — NẶNG** |
| Ngày đã qua | vẫn đặt | trả -1, **không đặt** | NHẸ |
| Tham số 3 | bỏ hẳn | có nhận (cờ cho `0x820ba80`) | NHẸ (mọi chỗ gọi chỉ truyền 2) |

**N9 cụ thể:** `lib\droptemplet.lua:129` chỉ kiểm `if tbItem.nExpiredTime then` — trong Lua **0 là true** ⇒ một dòng cấu hình `nExpiredTime = 0` sẽ khiến đồ rơi biến mất ngay. Hiện chưa thấy cấu hình nào dùng 0 trong 3 bao đóng ⇒ **bẫy tiềm ẩn, không phải lỗi đang chạy**.

Đơn vị 2 nhánh xác nhận từ chỗ gọi thật: phút → `fengling_ferry\bossdeath.lua:35` (`nRestMin = 24*60 - ...`), `shuizeideath.lua:17`; YYYYMMDD → `qianqiu_yinglie\head.lua:200-201` (`FormatTime2Date(7*24*60*60 + GetCurServerTime())`).

### 2.3 🟡 H5 — `ITEM_SetLeftUsageTime` ánh xạ hợp lý nhưng **không ai trừ**

Dịch ngược `0x08154FD0`: đòi **đúng 2 tham số** (`cmp eax,2 / je`), rồi `mov [eax+esi+0x348], edx` — trường **+0x348**, **khác** trường hạn dùng **+0x34C**. Vậy chọn một trường riêng của JX1 (`nParam`) là đúng hướng.

Nhưng trên JX1:
- `nParam` = "số lần sử dụng item" (`KItem.h:87`), có `SetParam/GetParam` (`KItem.h:413-414`), **được lưu** (`KPlayerDBFuns.cpp:1069` `iiduphong2`) và **được đồng bộ** (`KItemList.cpp:4069`, `KPlayer.cpp:10560`).
- **Không có đoạn engine nào TRỪ `nParam`**: grep `SetParam(` trong `Core\Src` chỉ ra nạp/đồng bộ/`LuaSetParamItem` (`ScriptFuns.cpp:6249`). `KItem.cpp:1361-1367` chỉ **hiển thị** "Còn lại %d lần sử dụng" cho `item_magicscript` particular 1083/1084 (Hồi thành phù).
⇒ "giới hạn số lần dùng" chỉ có hiệu lực nếu script tự trừ. **Nên ghi vào BÀN GIAO là LỆCH CÓ CHỦ ĐÍCH.**
- Phụ: `KInventory.cpp:519` yêu cầu `GetParam()` **bằng nhau** mới xếp chồng ⇒ vật phẩm rơi có `nParam ≠ 0` sẽ **không stack** với vật phẩm cùng loại thường. Lệch so với bản gốc (gốc dùng trường riêng, không nằm trong điều kiện stack).

### 2.4 🔴 N8 — `BT_GetBattleParam` trả SỐ, gốc trả CHUỖI

`15_bosung_soat_api.md` mục 9 tự ghi: `BT_GetBattleParam 0x081C69B0 → 1 chuỗi`. Bản thi công `KJx2WarInfra.cpp:1796-1801` đẩy `Lua_PushNumber(L, 0)`.

Chỗ gọi (`battles\battlehead.lua:625-644`):
```lua
str = BT_GetBattleParam(i);
tnpcid, level, count = getNpcInfo(str);
SetMissionV(MS_TRANK1_S + i - 1, tnpcid);
```
Tách chuỗi trên số 0 ⇒ `tnpcid/level/count` = nil/rác ⇒ `SetMissionV` ghi giá trị sai (hoặc lỗi `attempt to ...`). **Nên đẩy chuỗi rỗng `""`** — đúng hình dạng, và `getNpcInfo("")` thất bại "sạch" hơn.
**Mức: NẶNG** (theo tiêu chí "sai kiểu trả về so với gốc"), *nhưng* xem thêm N6: chưa xác minh `sf_buildfightnpcdata` có được 3 hoạt động gọi hay không.

### 2.5 Các chữ ký còn lại — ĐẠT

| Hàm | Đối chiếu | Kết luận |
|---|---|---|
| `GetNpcAroundPlayerList` | gốc trả `(bảng, số)`, phần tử là **PlayerIndex** (`[obj+0xC0]`); port đẩy `Npc[i].GetPlayerIdx()` (:1371) và trả 2 giá trị | **ĐẠT**. Khác biệt "nhánh lỗi gốc đẩy `nil`, port đẩy bảng rỗng" (`15_bosung` mục 5.2 cảnh báo): chỗ gọi duy nhất `bigboss.lua:218-222` chỉ `for i=1,nCount` ⇒ **vô hại** |
| `GetItemAllParams` | gốc đọc 6 số ở `+0x1E0`; port trả bảng `{1..6}` = `nGeneratorLevel[0..5]` | ĐẠT (`GetItemParam` của JX1 `ScriptFuns.cpp:5683` đọc đúng trường này) |
| `GetPlatinaLevel` | "trả nil nhưng VẪN 1 giá trị" | **ĐẠT** — `Lua_PushNil` + `return 1` (:1256-1257); `lib\log.lua:43` `tostring(...)` chịu được |
| `ITEM_DropRateItem` → `LuaDropRateItem` | alias | ĐẠT (`15_bosung` §1.1) |
| `NPCINFO_GetSeries` → `LuaGetNpcSeries` | alias | ĐẠT (`15_bosung` §1.2) |
| `NpcDropMoney` | gốc 3 tham số `(npc, tiền, belonger)` → 0 giá trị | ĐẠT (:1592-1616), có nhánh mặc định `GetPlayerIndex(L)` khi thiếu tham số 3 |
| `JoinMission` | gốc `(missionId, camp)` → 0 giá trị, chạy trên người chơi + subworld hiện tại | ĐẠT — khớp khuôn `LuaAddMissionPlayer` (`ScriptFuns.cpp:11498-11536`) |
| `Tm2Time` / `FormatTime2Date` | `FormatTime2Date(7*24*3600 + GetCurServerTime())`; `GetCurServerTime` của JX1 trả **epoch Unix thật** (`KTongJX2.cpp:3922-3926`) | ĐẠT |
| `DropItemEx` | 19 tham số so `lib\droptemplet.lua:59-93`: 1=subworld, 2/3=MPS, 4=belonger, 6=seed(chuỗi), 8..13 = genre/detail/particular/level/series/luck, 14..19 = magic — **khớp từng vị trí**; `GetNpcPos` của JX1 (`ScriptFuns.cpp:7845-7863`) đúng là `GetMpsPos` + `m_SubWorldIndex` | ĐẠT — xem H8 cho 2 điểm nhỏ |
| `TrimString` | gốc 0 giá trị; `lib\string.lua:158-163` `PushString(str); TrimString(); return PopString()` | ĐẠT |
| `Add120SkillExp` | `KPlayer::AddSkillExp120` tồn tại (`KPlayer.h:959`, `KPlayer.cpp:4439`), đang dùng thật ở `KNpc.cpp:3857` | ĐẠT |
| `ST_IsTransLife` | suy từ `LuaGetPlayerReBornValue` | ĐẠT về giá trị — xem H13 về ngăn xếp |
| `AddStatData` | gốc nhận **đúng 1 hoặc 2** tham số, ≥3 là no-op im lặng | Port chấp nhận ≥3 (bỏ qua tham số thừa). Lệch **NHẸ**, vô hại |

---

## 3. ĐĂNG KÝ NẰM TRONG HAY NGOÀI `#ifdef _SERVER` — **ÂM TÍNH, GIẢI THÍCH ĐẦY ĐỦ**

Quét toàn bộ chỉ thị tiền xử lý trong vùng bảng `GameScriptFuns[]` (`ScriptFuns.cpp:14354-15468`):

```
14420: #ifdef _SERVER        <<< MỞ nhánh máy chủ
14895:   #ifdef _SERVER
14981:     #ifdef _SERVER
15028:     #endif
15366:   #endif
        ... 15388-15434 = KHỐI [3HD 25/08] nằm Ở ĐÂY ...
15460: #else                 <<< nhánh client (chỉ PlaySound / PlaySprMovie)
15463: #endif
```

⇒ Khối đăng ký `[3HD]` (15388-15434) **nằm TRONG `#ifdef _SERVER` mở tại 14420**. Ba lớp đều nhất quán:
- thân hàm: `KJx2WarInfra.cpp` — cả tệp bọc `#ifdef _SERVER`, khối `[3HD]` kết thúc ngay trước `#endif // _SERVER` (`:1844`);
- khai báo `extern`: `ScriptFuns.cpp:13850-13883`, đóng bằng `#endif` ở `:13884`;
- 3 hàm IniFile/File: `ScriptFuns.cpp:3247-3308`, nằm trong `#ifdef _SERVER` mở tại `:2643`.

**Đó là lý do bản client LINK PASS.** Đã kiểm lại bằng biên dịch (§9). **Không có việc phải làm.**

---

## 4. TRÙNG TÊN TRONG `GameScriptFuns[]` — **ÂM TÍNH**

Quét bằng kịch bản riêng (phân biệt HOA/thường; bỏ dòng chú thích), vùng `14354..15468`:

- **1048 mục**, **1045 tên duy nhất**.
- **3 cặp trùng — TẤT CẢ đều CŨ, không dính `[3HD]`:**

| Tên | Dòng | Ghi chú |
|---|---|---|
| `BT_GetData` | 14403, **15270** | cả hai đều là mã cũ (`[TONG 21/08]` / tongwar) |
| `GetLeadLevel` | 14579, 14734 | mã cũ |
| `GetMissionS` | 14837, **15330** | mã cũ |

- **41 tên `[3HD]` + 2 alias (`ITEM_DropRateItem`, `NPCINFO_GetSeries`) đều DUY NHẤT.**
- Không có bảng đăng ký thứ hai nào vào cùng `Lua_State`: `KSortScript.cpp:160` và `KPlayer.cpp:7405` chỉ nạp `GameScriptFuns`; `g_GetGameScriptFunNum()` = `sizeof/sizeof` (`ScriptFuns.cpp:15484-15487`) nên **không có trần cứng** để tràn.
- `NpcName2Replace` **cố ý không đăng ký** (chỗ gọi có guard `if NpcName2Replace then`, `challengeoftime\npc\transfer.lua:10`) — đúng với `15_bosung` §2.2.

**Không có việc phải làm.** (Ba cặp trùng cũ nên ghi lại cho đợt khác; ngoài phạm vi đợt này.)

---

## 5. `ITEM_SetExpiredTime` — xem §2.2 (ngưỡng ĐÚNG, gốc thời gian ĐÚNG, thiếu 2 nhánh biên)

Bổ sung bằng chứng gốc thời gian, để phiên sau khỏi phải kiểm lại:
- `Engine\Src\KSG_StringProcess.cpp:18-23` → `int KSG_GetCurSec() { time(&curtime); return curtime - 1451581200; }`
- `Core\Src\KItem.h:344-350` → `SetExpireTime(int nSec)` ghi thẳng `nExpireTime`; `GetExpireTime()` so với `KSG_GetCurSec()`
- `Core\Src\KItem.cpp:2627-2629` → hiển thị: `nowtime = nExpireTime + 1451581200`
⇒ **`nExpireTime` tính theo gốc 01/01/2016.** Cả 2 nhánh của `LuaHD3_ITEM_SetExpiredTime` đều quy về đúng gốc này. ✔

---

## 6. `SetItemBindState` → `SetPlayerItemLock` — **ĐẠT**, một lệch nhỏ

`KItem.h:313-316`:
```c
void SetPlayerItemLock(int nLock) { InsuranceCourse = nLock; m_CommonAttrib.LockItem.nState = nLock; }
```
- `LOCK_STATE_FOREVER = -2` (`GameDataDef.h:291`); `KLockItem::nState` là **`short`** (`GameDataDef.h:1688`) ⇒ `-2` **không tràn**.
- `InsuranceCourse` là `int` (`KItem.h:137`), được lưu (`KPlayerDBFuns.cpp:1069` `ilockbh`) và đồng bộ (`KItemList.cpp:4076`, `KProtocolProcess.cpp:708`) ⇒ **bền qua save** như BÀN GIAO nói. ✔
- **Đường cấm có THẬT ở cả 3 nhóm** (đề bài chỉ nêu 3 dòng KPlayer — thực tế nhiều hơn):

| Hành vi | Chỗ chặn |
|---|---|
| **Vứt đồ** | `KPlayer.cpp:3755`, `:5155`, `:5237` — cả 3 đều `InsuranceCourse > 0 \|\| == -2` |
| **Bán cho NPC** | `KBuySell.cpp:366-368`, `:407-409` |
| **Giao dịch / kho / các thao tác UI** | `CoreShell.cpp:3140-3141`, `:3346-3347`, `:4267-4268`, `:9942`, `:10144-10146`, `:12792-12794`, `:12901-12903`, `:13362-13364` |

- **Lệch duy nhất:** `lib\droptemplet.lua:25` ghi `-2 = vĩnh viễn, -1 = khoá thường`. JX1 kiểm `> 0 || == -2` ⇒ **`-1` không khoá gì cả**. Cấu hình hiện tại chỉ dùng `-2` (`activitysys\config\32\variables.lua:11-16`, `fengling_ferry\fld_head.lua:123`, `battles\battlehead.lua:1317`) ⇒ **chưa lộ**. Nên ghi chú, hoặc quy `-1` về `-2`.
**Mức: NHẸ.**

---

## 7. `AddStatData` GHI TỆP — 🔴 N4

```c
KJx2WarInfra.cpp:1166-1173
    static FILE* s_pStat = NULL;
    if (!s_pStat) { g_GetRootPath(szPath); strcat(szPath, "\\log_game\\hd3_statdata.log"); s_pStat = fopen(szPath, "a"); }
```

1. **Đường dẫn gốc ĐÚNG**: `g_GetRootPath` (`Engine\Src\KFilePath.cpp:125-128`) trả chuỗi **đã cắt dấu `\` cuối** (`:110-116`), nên `strcat("\\log_game\\...")` ghép đúng — cùng khuôn `KJx2League.cpp:125-130`.
2. **🔴 Nhưng thư mục KHÔNG TỒN TẠI**: trên cây sống `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server` **không có** thư mục `log_game`. `log_game` ở JX1 là thư mục **SCRIPT** (`script\log_game\log_giaodich.lua` … — `KPlayer.cpp:5825`, `KBuySell.cpp:338`). Thư mục nhật ký thật của máy chủ là `bin\server\logs\`.
   `fopen(..., "a")` **không tự tạo thư mục** ⇒ luôn `NULL`.
3. **Hệ quả kép**:
   - tính năng thống kê **chết im lặng** (BÀN GIAO §1 nói "ghi `log_game\hd3_statdata.log`" — không đúng);
   - vì `s_pStat` vẫn `NULL`, **mỗi lời gọi lại thử `fopen`**. `AddStatData` có **11 chỗ gọi**, trong đó `fengling_ferry\fld_death.lua:30/32/34` chạy **mỗi lần có người chết ở Phong Lăng Độ** và `mission.lua:48/50/52` chạy cho **mỗi người tham gia** ⇒ chuỗi syscall hỏng lặp lại trên **luồng logic đơn** (đã đo: 100% CPU game nằm trên 1 luồng).
4. **Rò rỉ / đua luồng**: `FILE*` static không bao giờ `fclose` — chấp nhận được vì GameServer đơn luồng logic (đã đo) và tiến trình đóng sẽ flush. **Không phải lỗi.**
5. **Thiếu kiểm NULL**: `localtime(&nNow)` (`:1177`) có thể trả `NULL`; `:1179` dùng ngay `pTm->tm_year`. `LuaHD3_FormatTime2Date` (`:1140-1145`) thì **có** kiểm — không nhất quán.

**Việc phải làm:** đổi sang `bin\server\logs\` (đã tồn tại) hoặc `CreateDirectory` trước; thêm kiểm `pTm`; nếu `fopen` hỏng thì đặt cờ để **thôi thử lại**.
**Mức: NẶNG.**

---

## 8. `KSortScript.cpp` — 5 TIỀN TỐ MỚI (mục 8 của đề bài)

`KSortScript.cpp:126-130`:
```c
"\\script\\missions\\fengling_ferry\\",
"\\script\\missions\\challengeoftime\\",
"\\script\\missions\\boss\\",
"\\script\\vng_feature\\",
"\\settings\\trigger_",
```
Cú pháp C **đúng** (escape `\\`, mảng `const char*`, vòng lặp `sizeof/sizeof` `:135`, so bằng `strstr` `:137`).

`g_IsJx2Script` đổi hành vi **5 hàm**: `GetGameTime` (`ScriptFuns.cpp:281` — trả **giây** thay vì frame), `AddNpc` (`:6992` — tham số 7 thành `bNoRevive`), `GetTeamMember` (`:7568` — quy ước vị trí khác), `AddSkillState` (`:12874` — ép `nIfMagic = 1`), và một nhánh ở `:13150`.

### 8.1 `\script\missions\boss\` — **ÂM TÍNH** (rủi ro đề bài lo nhất)

Trên cây sống, thư mục `script\missions\boss\` chứa **đúng 1 tệp**: `bigboss.lua` — chính tệp Linux vừa chép (có trong `b1_manifest.txt`). **Không có script boss nào của JX1 nằm ở đó** (boss JX1 ở `script\item\bosscharm.lua`, `script\global\thanh\npc\bosssatthu*.lua`, `script\item\seasonnpc_item.lua`). ⇒ **Không đổi hành vi hệ boss JX1.**

### 8.2 `\settings\trigger_` — **ÂM TÍNH**

`bin\server\settings\` chỉ có **2 tệp `.lua`**: `trigger_challengeoftime.lua`, `trigger_include.lua` — **cả hai vừa được chép trong đợt này** (có trong manifest). Không tệp JX1 nào bị kéo vào.

### 8.3 🟡 H1 — `\script\vng_feature\` **CÓ tệp JX1 bị kéo vào**

`script\vng_feature\` có 4 tệp; **3 tệp nằm trong manifest** (`challengeoftime\npcNhiepThiTran.lua`, `forbiditem\vngforbidspecialitem.lua`, `double_mission_award.lua`), nhưng:

```
-rw-r--r--  488  Aug 21 22:36  script/vng_feature/checkinmap.lua      <<< TỆP JX1 CÓ SẴN (đợt Tín Sứ 21/08)
```
`checkinmap.lua` **không** có trong `b1_manifest.txt` ⇒ là tệp JX1. Nay bị `g_IsJx2Script` nhận là script JX2.

Đọc trọn `checkinmap.lua` (14 dòng): chỉ dùng `Include`, `GetFightState()`, `lib:ShowMessage`, `self:CheckInMap`, `PlayerFunLib` — **không chạm** vào 5 hàm bị đổi hành vi ⇒ **hiện tại KHÔNG sai**. (Nó `Include` `script\activitysys\playerfunlib.lua`, mà `\script\activitysys\` **vốn đã** nằm trong danh sách JX2 từ trước — `KSortScript.cpp:120`.)

**Vẫn là bẫy**: bất kỳ ai sửa `checkinmap.lua` (hoặc thêm tệp JX1 vào `vng_feature\`) sau này sẽ bị đổi ngữ nghĩa `GetGameTime`/`AddNpc` mà không hiểu vì sao.
**Việc nên làm:** thay 1 tiền tố rộng bằng 2 tiền tố hẹp:
```
"\\script\\vng_feature\\challengeoftime\\",
"\\script\\vng_feature\\forbiditem\\",
```
và liệt kê riêng `"\\script\\vng_feature\\double_mission_award.lua"`.
**Mức: NHẸ.**

---

## 9. BIÊN DỊCH LẠI (mục 9 của đề bài)

Chỉ chạy target `ClCompile` (**không link, không PostBuildEvent, không deploy**).

| Cấu hình | Lệnh | Kết quả |
|---|---|---|
| `Server Release \| x64` | `MSBuild Core.vcxproj /t:ClCompile` | **0 error C / 0 error LNK**, 619 warning. `KJx2WarInfra.cpp` **có** được biên dịch lại (13 dòng chẩn đoán) |
| `Client Release \| Win32` | `/t:ClCompile "/p:SelectedFiles=Src\KJx2WarInfra.cpp"` (và `ScriptFuns.cpp`, `KSortScript.cpp`, từng tệp) | **0 error** cả 3 tệp |

Warning ở khối `[3HD]` — **toàn bộ đều là C4996 (hàm CRT "không an toàn")**, cùng loại với 619 warning sẵn có của dự án, **không có warning mới về kiểu/tràn**:
`KJx2WarInfra.cpp(1140)` `localtime` · `(1171)` `strcat` · `(1172)` `fopen` · `(1177)` `localtime` · `(1566)` `sprintf` · `(1568)` `strcpy` · `(1725)(1728)(1730)` `strncpy`.

Nhật ký đầy đủ: `…\scratchpad\core_srv_x64.log`, `…\scratchpad\cli_*.log`.

---

## 10. 🔴 N3 — "VÁ STUB THẬT `DisabledUseTownP`" KHÔNG CÓ TÁC DỤNG

BÀN GIAO §1 khẳng định:
> "🔴 **Vá stub thật — `DisabledUseTownP`** … nay chuyển sang bản thật … 7 lời gọi của PLD/Vượt Ải trước đây vô tác dụng ⇒ người chơi thoát thuyền/mật phòng bằng Hồi thành phù. Kèm map 984/337-339 đã chặn item dịch chuyển ở `header\forbidmap.lua` (đợt trước)."

Kiểm thật — **cả hai lớp đều KHÔNG có**:

**(a) Không script nào ĐỌC cờ.** `LuaHD3_DisabledUseTownP_Real` (`KJx2WarInfra.cpp:1658-1677`) đặt `s_byHD3NoTownP[]`, và hàm đọc là **tên MỚI** `GetDisabledUseTownP` (đăng ký `ScriptFuns.cpp:15415`). Grep toàn `E:\SourceTuanLe\...\bin\server`:
```
GetDisabledUseTownP  →  0 tệp .lua  (chỉ khớp trong CoreServer.dll và CoreServer.dll.moi_2508_3hoatdong)
```
Trong khi **script JX1 có sẵn lại gọi một tên KHÁC**:
```
script\missions\clearskill\head.lua:143-148
    function CSP_CheckValid()
        if (IsDisabledUseTownP() ~= 1) then return 0; end
        return 1;
    end
```
`IsDisabledUseTownP` **KHÔNG được đăng ký** ở JX1 (`grep '"IsDisabledUseTownP"'` trong `Sources` = 0 kết quả) — tức lời gọi này vẫn `attempt to call global` như trước. Nếu đặt tên hàm đọc là **`IsDisabledUseTownP`** thì vừa khớp bản Linux vừa **vá luôn** lỗi sẵn có này.

**(b) `forbidmap.lua` KHÔNG hề được thêm map 3 hoạt động.** Chú thích trong mã (`KJx2WarInfra.cpp:1653-1655`) nói "đã thêm map 3 hoạt động - cùng khuôn Thành Bảo 984". Đọc tệp thật `script\header\forbidmap.lua`:
```lua
function CheckAllMaps(mapid)          -- dòng 94-119
    if mapid == 984 then return 1 end               -- [TONGCASTLE 23/08] — CHỈ CÓ 984
    if checkSJMaps(mapid) == 1 then return 1 end
    ... checkBWMaps / checkZQMaps / checkXSMaps / checkHFMaps / checkActMaps ...
    return 0
end
```
Không có 336/337/338/339, không có map Vượt Ải. (`TRAINMAPS` dòng 11 **có** 336/340 nhưng `TRAINMAPS` **không được `CheckAllMaps` dùng**.) `CheckAllMaps` chính là cửa chặn của `script\item\townportal_l.lua` (Hồi thành phù) và các phù dịch chuyển khác.

⇒ **Kết luận: hành vi trước và sau bản vá GIỐNG HỆT NHAU** — người chơi vẫn dùng Hồi thành phù thoát thuyền Phong Lăng Độ / mật phòng Vượt Ải. Bản vá chỉ tốn 6 KB bộ nhớ static.
**Mức: NẶNG** (khẳng định trong BÀN GIAO là sai sự thật; và tính năng gốc vẫn thiếu).

**Việc phải làm (một trong hai, tốt nhất cả hai):**
1. Đổi tên đăng ký `GetDisabledUseTownP` → **`IsDisabledUseTownP`** (khớp `clearskill\head.lua:144`), rồi cho `townportal_l.lua` kiểm thêm nó; **hoặc**
2. Thêm map 3 hoạt động vào `CheckAllMaps` của `script\header\forbidmap.lua` đúng khuôn 984 (đây mới là đường JX1 dùng thật).

### 10.1 🔴 N10 — `IsDisabledUseHeart` cũng chết theo

`ScriptFuns.cpp:15416` đăng ký `IsDisabledUseHeart` → `LuaHD3_IsDisabledUseHeart` (trả cùng cờ). Grep cây sống: **0 tệp `.lua` gọi** (chỉ khớp trong 2 DLL). Lý do: chỗ gọi gốc là `item\heart_head.lua:116` của **bản Linux**, mà `script\item\heart_head.lua` **đã có sẵn ở JX1** nên `b1_copy.py` (chính sách "không đè tệp JX1") **không chép**, và bản JX1 không có lời gọi đó.
⇒ Dòng 23 của bảng `15_bosung_soat_api.md` mục 6 ("Không chặn được Tâm Tâm Tương Ánh Phù ⇒ dịch chuyển lậu vào bản đồ Vượt ải") **vẫn đúng nguyên như trước khi port**. Hàm C++ là mã chết.
**Mức: NẶNG (thiếu tính năng, không phải lỗi mã).**

---

## 11. 🔴 N6 — SÁU HÀM MỚI **KHÔNG CÓ LỜI GỌI THẬT** TRONG 3 BAO ĐÓNG

Quét `D:\GAMEDEVNEW\ReverseTools\port_3hd\src_utf8\{satthu,phonglangdo,vuotai}` (đây là bản GIẢI MÃ của chính 3 bao đóng):

| Hàm mới | "Chỗ gọi" mà `15_bosung` mục 6 ghi | Sự thật |
|---|---|---|
| `IniFile_SetData` | `lib\file.lua:19` | chỉ nằm **bên trong hàm bọc** `ini_setdata`; grep `ini_setdata` toàn 3 bao đóng ⇒ **0 lời gọi** ngoài chính `lib\file.lua` |
| `IniFile_Save` | `lib\file.lua:24` | như trên (`ini_save` ⇒ 0 lời gọi) |
| `File_Create` | `lib\file.lua:8` | như trên (`ini_loadfile` ⇒ 0 lời gọi) |
| `GetRoomItems` | `lib\composeex.lua:191` | nằm trong phương thức `tbActivityCompose:GetRoomItems`; grep `tbActivityCompose` toàn 3 bao đóng (trừ chính `lib\composeex.lua`) ⇒ **0 kết quả** — không tệp nào khởi tạo lớp này |
| `OpenProgressBar` | `lib\progressbar.lua:95` | nằm trong `tbProgressBar:Start`; grep `tbProgressBar` (trừ `lib\progressbar.lua`) ⇒ **0 kết quả** |
| `GetItemAllParams`, `ITEM_GetItemRandSeed`, `GetItemGenTime`, `GetPlatinaLevel` | `lib\log.lua:46/48/51/43` | tất cả nằm trong `function getItemInfo` (`lib\log.lua:35-53`); grep `getItemInfo` toàn 3 bao đóng ⇒ **chỉ 1 kết quả = chính dòng định nghĩa** |

Hệ quả:
- Bảng "THIẾU THÌ HỎNG GÌ / Mức" của `15_bosung_soat_api.md` mục 6 (và mục "Nhóm hàm thật" của BÀN GIAO) **thổi phồng**: 4 dòng ghi CAO/RẤT CAO cho hàm không ai gọi.
- Rủi ro N5 (`KIniFile::Save`) lẽ ra **không cần gánh**.
- Ngược lại, các hàm **THẬT SỰ** nằm trên đường chạy chính là: `DropItemEx`, `NpcDropMoney`, `ITEM_SetExpiredTime`, `SetItemBindState`, `AddStatData`, `Tm2Time`, `JoinMission`, `GetNpcAroundPlayerList`, `GetFirst/GetNextPlayerAtServer`, `Add120SkillExp`, `ST_IsTransLife`, `TrimString`, `ITEM_DropRateItem`, `NPCINFO_GetSeries` — **nên tập trung test vào nhóm này**.
- `GetItemQuality` / `GetGlodEqIndex` **có** chỗ gọi thật ngoài `getItemInfo`: `event\jiefang_jieri\200904\qianqiu_yinglie\head.lua:65-70` — xem H10.

**Mức: NẶNG (sai đánh giá ưu tiên, dẫn tới rủi ro không cần thiết).** CHƯA XÁC MINH: `script_protocol\protocol_def_gs.lua:193` (`SendScriptData`) và `battles\battlehead.lua:625` (`BT_GetBattleParam`) có được nạp/chạy trên cây sống không — cần Hướng soát script trả lời.

---

## 12. 🔴 N5 — `IniFile_Save` DÙNG ĐÚNG THỨ MÀ DỰ ÁN ĐÃ CẤM

`ScriptFuns.cpp:3274-3284`:
```c
int LuaIniFile_Save(Lua_State* L) { ... Lua_PushNumber(L, p->Ini.Save(szTo) ? 1 : 0); return 1; }
```

**(a) Vi phạm luật đã ghi trong chính cây nguồn** — ba tệp port trước đều **cố ý tránh** `KIniFile::Save`:
```
Core\Src\KJx2SharedStore.cpp:2-3
    // Persist Ladder theo khuon tmp + MoveFileEx REPLACE (KIniFile::Save cat trang
    // file truoc khi ghi nen KHONG dung cho du lieu song - phan bien E ky thuat F6).
Core\Src\KJx2League.cpp:2   ... (tmp + MoveFileEx REPLACE - khong dung KIniFile::Save).
Core\Src\KJx2CityWar.cpp:2  ... (tmp + MoveFileEx REPLACE - khong dung KIniFile::Save).
```

**(b) Bẫy cạn bộ nhớ → NULL-deref.** `Engine\Src\KIniFile.cpp:306-325`:
```c
dwLen   = m_MemStack.GetStackSize();
DataBuf = (LPSTR)m_MemStack.Push(dwLen);       // luôn phải cấp CHUNK MỚI
pBuffer = DataBuf;
... sprintf(pBuffer, ...)                      // KHÔNG kiểm NULL
```
- `KMemStack::Free` là **no-op** (`Engine\Src\KMemStack.cpp:76-79: void Free(PVOID) { return; }`) ⇒ không bao giờ thu hồi.
- `MAX_CHUNK = 10` (`Engine\Src\KMemStack.h:12`); `AllocNewChunk` thất bại thì gọi **`g_MessageBox("KMemStack : Chunk over flow")`** — hộp thoại **chặn** trên tiến trình máy chủ — rồi `Push` trả `NULL` ⇒ `sprintf(NULL, …)` ⇒ **sập**.
- Mỗi lần `ini_save` tiêu **một chunk** ⇒ sau ~9 lần lưu cùng một tệp ini là hết.

**(c)** `KIniFile::Load` dùng `KPakFile`, `Save` dùng `KFile` — hai đường phân giải đường dẫn khác nhau; `LuaFile_Create` (`ScriptFuns.cpp:3286-3308`) lại tự ghép `g_GetRootPath` + đường dẫn. **CHƯA XÁC MINH** ba đường này có trỏ về cùng một tệp không.

**Việc phải làm:** vì hiện **không ai gọi** (N6), phương án rẻ nhất là **gỡ 3 dòng đăng ký** `IniFile_SetData`/`IniFile_Save`/`File_Create`; nếu vẫn muốn giữ thì viết đường ghi riêng theo khuôn `tmp + MoveFileEx REPLACE` của `KJx2League.cpp:125-130`.
**Mức: NẶNG (rủi ro sập, hiện chưa kích hoạt).**

---

## 13. 🔴 N7 — `GetRoomItems` VỨT BỎ THAM SỐ KHOANG

`KJx2WarInfra.cpp:1268-1295` **không hề đọc tham số 1**; luôn quét `{ room_equipment, room_equipmentex }`.

Bản Linux nhận loại khoang; bảng hằng ở `lib\composeex.lua:22-39`:
```
room_equipment = 0   room_repositor = 1   room_trade = 2 ... room_immediacy = 5
room_ext1..3 = 7,8,9   room_giveitem = 10   room_distill = 11 ...
```
`tbActivityCompose:_init(tbFormula, szLogTitle, nRoomType)` (`:50`) cho phép chỉ định khoang; mặc định 0 (túi).

Hệ quả nếu về sau có công thức dùng khoang khác (rương = 1, phím tắt = 5…):
`tbActivityCompose:CheckMaterial` / `ConsumeItem` (`composeex.lua:70/210`) sẽ **đếm và TIÊU HUỶ vật phẩm trong TÚI** thay vì khoang được hỏi.

Lệch thứ hai: bản Linux khoang 0 = **một** khoang; port gộp thêm `room_equipmentex` (hành trang mở rộng) ⇒ đếm/tiêu thụ **nhiều hơn gốc**.

Hiện chưa kích hoạt (N6: không tệp nào khởi tạo `tbActivityCompose`).
**Việc phải làm:** đọc tham số 1, ánh xạ `0→room_equipment(+ex)`, `1→room_repository`, `5→room_immediacy`, `7/8/9→room_exbox1/2/3`; khoang không hỗ trợ thì trả **bảng rỗng**, đừng trả túi.
**Mức: NẶNG (sai hành vi, chưa kích hoạt).**

---

## 14. CÁC ĐIỂM NHẸ CÒN LẠI

| # | Chỗ | Nội dung |
|---|---|---|
| H6 | `KJx2WarInfra.cpp:1301-1327` | JX1 **đã có sẵn** bộ lặp đúng ngữ nghĩa Linux (danh sách người chơi đang dùng + **một** con trỏ toàn cục): `KPlayerSet::GetFirstPlayer()/GetNextPlayer()`, đang dùng thật ở `ScriptFuns.cpp:9675/9684`. Bản thi công tự viết quét tuyến tính 1500 slot với điều kiện `Player[i].m_nIndex > 0`. Kết quả không sai, nhưng: (a) tốn công vô ích; (b) **lệch tập** — bỏ qua người vừa đăng nhập chưa sinh NPC (`m_nIndex == 0`) mà `m_UseIdx` vẫn tính. Nên dùng lại hàm có sẵn. |
| H8 | `KJx2WarInfra.cpp:1548-1549` | Ghi đè `Item[nIdx].GetGeneratorParam()->uRandomSeed` **SAU** khi `AddItemSet2` đã sinh thuộc tính ⇒ hạt giống lưu lại không còn tái tạo đúng vật phẩm (chỉ ảnh hưởng log/kiểm tra). Ngoài ra không kiểm `SubWorld[nSubWorldIdx]` đã nạp (`m_nTotalRegion > 0`) trước khi `Mps2Map`. |
| H9 | `KJx2WarInfra.cpp:1709, 1720-1722` | `OpenProgressBar` đòi `top < 5` trả về; bản gốc đòi `> 5` (tức ≥ 6, `0x081082EA`). Chỗ gọi duy nhất truyền 6 nên chưa lộ. **Quan trọng hơn**: khi `g_GetScriptNameByState` trả rỗng, port **giữ nguyên `m_dwTimeBoxId` CŨ** ⇒ callback có thể chạy trong tệp SAI; khuôn `LuaOpenTimeBox` (`ScriptFuns.cpp:9755/9762`) **luôn** đặt lại. Quy đổi `nFrame / 18` là **đúng** (`progressbar.lua:78` `pTemp.nTime * 18`). |
| H10 | `KJx2WarInfra.cpp:1238-1258` | `GetItemQuality`/`GetGlodEqIndex` trả 0 làm `event\jiefang_jieri\200904\qianqiu_yinglie\head.lua:65-70` **luôn** đi nhánh "mặt nạ thường" ⇒ mất bậc thưởng của mặt nạ Hoàng Kim (`4493 <= nRecordIndex <= 4630`). Lệch có chủ đích (JX1 không có trường phẩm chất) nhưng **BÀN GIAO chỉ ghi "chỉ để log + ghép đồ"**, bỏ sót chỗ gọi này. |
| H11 | `KJx2WarInfra.cpp:1814-1819` | 6 hàm `PET_*` gộp một stub trả 0. `15_bosung` §5.4 nói `PET_GetUpgradePoint` phải trả **-1** khi lỗi. Vì cả get lẫn set đều stub nên `PET_SetUpgradePoint(PET_GetUpgradePoint()+n)` (`lenhbai_def.lua:390`) vô hại. |
| H13 | `KJx2WarInfra.cpp:1762-1773` | `LuaHD3_ST_IsTransLife` gọi `LuaGetPlayerReBornValue(L)` rồi đẩy thêm 1 giá trị ⇒ để lại giá trị thừa dưới đỉnh ngăn xếp Lua. `return 1` chỉ lấy đỉnh nên **vô hại**, nhưng nên đọc-rồi-pop cho sạch. |
| H14 | `KJx2WarInfra.cpp:1114` | `Tm2Time` mặc định `{1970,1,1,0,0,0}` khi thiếu tham số. Chỗ gọi `activitysys\functionlib.lua:364/380` truyền đủ 6. **CHƯA XÁC MINH** mặc định của bản gốc `0x08103AC0`. |
| H15 | `KJx2WarInfra.cpp:1264-1267` | `std::map<int,int> mapSeen` để khử trùng là **thừa**: `KInventory::FindItem` đã trả `-1` cho ô nối tiếp của vật phẩm nhiều ô (`KInventory.cpp:162-163`), nên không thể lặp. Vô hại, chỉ tốn cấp phát. |
| H16 | `KJx2WarInfra.cpp:1095-1103` | `sHD3_LogOnce` giữ `std::map<std::string,int>` static (rò rỉ có chặn, tối đa 6 khoá) — chấp nhận được. |
| H17 | `ScriptFuns.cpp:3280-3281` | `LuaIniFile_Save` khi thiếu tham số 2 dùng `p->strFile` — hợp lý; nhưng bản gốc `IniFile_Save(szNguồn, szĐích)` có ngữ nghĩa "ghi sang tệp khác", còn ở đây tham số 1 là **handle** chứ không phải tệp. Vì `lib\file.lua:24` truyền cùng chuỗi nên trùng khớp — ghi chú cho phiên sau. |

---

## 15. VIỆC PHẢI LÀM — XẾP THEO ƯU TIÊN

| Ưu tiên | Mã | Việc |
|---|---|---|
| 1 | **N2** | Thêm `Npc[nMe].m_Index <= 0` (và chặn `m_RegionIndex >= m_nTotalRegion`) vào `KJx2WarInfra.cpp:1344` |
| 2 | **N1** | `ITEM_GetExpiredTime` → trả `m_CommonAttrib.nExpireTime + 1451581200` (0 vẫn 0), bỏ `GetExpireTime()` |
| 3 | **N3 + N10** | Đổi tên đăng ký thành `IsDisabledUseTownP` **và/hoặc** thêm map 3 hoạt động vào `CheckAllMaps` của `script\header\forbidmap.lua`; sửa chú thích sai ở `KJx2WarInfra.cpp:1653-1655` và BÀN GIAO §1 |
| 4 | **N4** | Đổi `log_game\` → `logs\` (hoặc `CreateDirectory`), kiểm `pTm` NULL, thôi thử lại khi `fopen` hỏng |
| 5 | **N5 + N6** | Gỡ đăng ký `IniFile_SetData`/`IniFile_Save`/`File_Create` (không ai gọi, mà `KIniFile::Save` bị dự án cấm); nếu giữ thì viết đường ghi `tmp + MoveFileEx` |
| 6 | **N8** | `BT_GetBattleParam` → `Lua_PushString(L, "")` thay vì số 0 |
| 7 | **N9** | `ITEM_SetExpiredTime`: `dVal == 0` ⇒ không đặt hạn (trả 0), và ngày đã qua ⇒ trả -1 |
| 8 | **N7** | `GetRoomItems`: đọc và tôn trọng tham số `nRoomType` |
| 9 | **H1** | Thu hẹp tiền tố `"\\script\\vng_feature\\"` trong `KSortScript.cpp:129` |
| 10 | **H12** | Chặn độ dài `szSect` ≤ 28 trước khi vào `KIniFile::SetKeyValue` (nếu giữ `IniFile_SetData`) |
| 11 | **H6, H8, H9, H10, H11, H13** | Dọn theo bảng §14; **H10 phải ghi vào BÀN GIAO** vì đây là lệch thưởng nhìn thấy được |

---

## 16. ĐÃ CỐ BÁC NHƯNG KHÔNG BÁC ĐƯỢC (giữ nguyên kết luận của phiên trước)

- Gốc thời gian `1451581200` và ngưỡng `20 000 000` của `ITEM_SetExpiredTime` — **đúng**, đã kiểm bằng cả mã máy lẫn mã nguồn JX1.
- Ánh xạ `SetItemBindState(-2)` → `SetPlayerItemLock` → khoá bán/vứt/giao dịch — **đúng và bền qua save**.
- Thứ tự 19 tham số của `DropItemEx` và thứ tự `AddItemSet2` — **khớp từng vị trí** với `lib\droptemplet.lua` và khuôn `KNpc::DropItemFromLuaScript`.
- `TrimString` dùng **đúng** bộ đệm chung `s_szStrBuf` (cùng tệp, cùng nhóm hàm) — không phải bộ đệm khác như tôi nghi ban đầu.
- Đăng ký nằm trong `#ifdef _SERVER`; không trùng tên; `\script\missions\boss\` và `\settings\trigger_` **không** đụng script JX1.
- `room_equipmentex` thật sự cũng 6×10 — chú thích ở `KJx2WarInfra.cpp:1265` chính xác.
- `JoinMission` khớp khuôn `LuaAddMissionPlayer`; đăng ký hàm engine **không** phá `missions\tongwar\match\mission.lua:163` (mỗi tệp một `Lua_State`).
- Cả hai cấu hình **biên dịch sạch, 0 error**.

---

## 17. TỆP ĐÃ ĐỌC / CÔNG CỤ

Đọc (không sửa): `Sources\Core\Src\{KJx2WarInfra.cpp, ScriptFuns.cpp, KSortScript.cpp, KItem.h, KItem.cpp, KItemSet.h, KNpc.cpp, KPlayer.cpp, KObj.cpp, KObjSet.cpp, KInventory.h/.cpp, KItemList.cpp, KPlayerSet.cpp, KPlayerDef.h, GameDataDef.h, KJx2League.cpp, KJx2SharedStore.cpp, KTongJX2.cpp, KBuySell.cpp, CoreShell.cpp}`, `Sources\Engine\Src\{KIniFile.cpp, KMemStack.h/.cpp, KSG_StringProcess.cpp, KFilePath.cpp}`; cây sống `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\{script,scriptjx2,settings}`; `ReverseTools\port_3hd\{src_utf8, thicong\b1_manifest.txt, 15_bosung_soat_api.md}`; `D:\GAMEDEVNEW\BANGIAO_3HOATDONG_2508.md`.

Dịch ngược (`re_disasm.py` trên `D:\ServerLinux\server1\jx_linux_y`): `0x08154540` (`ITEM_GetExpiredTime`), `0x08154A30` (`ITEM_SetExpiredTime`), `0x081F2760` (helper đổi đơn vị hạn dùng), `0x08154FD0` (`ITEM_SetLeftUsageTime`).

Biên dịch: `MSBuild Core.vcxproj /t:ClCompile` — `Server Release|x64` và `Client Release|Win32`. **Không link, không PostBuild, không deploy, không đụng `bin\`.**

---

## ĐỐI CHẤT (tác tử độc lập)

| Mục | Nội dung |
|---|---|
| Vai trò | Kiểm chứng ngược **báo cáo A1** — không phải người viết. Mặc định coi mọi phát hiện của A1 là SAI cho tới khi tệp gốc / mã máy / cây sống chứng minh ngược lại |
| Đã kiểm | **22 phát hiện** (10 NẶNG + 5 âm tính + 7 NHẸ/ĐẠT tiêu biểu) |
| Kết quả | **13 ĐÚNG nguyên vẹn · 4 THỔI PHỒNG · 1 HẠ THẤP · 2 bằng chứng SAI ở chi tiết · 2 cách sửa đề xuất sai/vô ích** |
| Bỏ sót | **5 lỗi thật A1 không thấy — trong đó 2 CHẶN**, do A1 **chưa từng mở nhật ký chạy thật** của cây sống |
| Không sửa | Chỉ đọc `Sources`, `E:\SourceTuanLe\...\bin\server`, ELF Linux. Chỉ ghi thêm vào chính tệp báo cáo này. |

### DC.1 — Bảng đối chất

| Phát hiện | Bằng chứng gốc (tôi tự kiểm) | KẾT LUẬN | Sửa lại thành |
|---|---|---|---|
| **N1** `ITEM_GetExpiredTime` trả phút, gốc trả thô | Dịch ngược lại `0x081545B9 mov eax,[eax+edx+0x34c]` → `0x081545D2 call 0x8232d40` (`lua_pushnumber`), **không phép tính** — ĐÚNG. `KItem.h:344-350` `GetExpireTime()` trả 0 khi hết hạn — ĐÚNG. `KSG_StringProcess.cpp:17-22` `return curtime - 1451581200` — ĐÚNG. **NHƯNG**: chỗ gọi `activity.lua:314` nằm trong `ActivityClass:GetItemExpiredTime`, mà grep `GetItemExpiredTime` toàn 3 bao đóng + toàn cây sống ⇒ **chỉ 1 kết quả = chính dòng định nghĩa, 0 lời gọi**. Và nhánh lỗi của bản gốc đẩy hằng `[0x825eed4]` = **-2.0** (đọc byte `000000c0`), **không phải 0** như A1 viết | **ĐÚNG (lỗi thật) + THỔI PHỒNG ưu tiên + 1 bằng chứng SAI** — A1 xếp ưu tiên **2/11** cho một hàm **không ai gọi**, đúng thứ mà chính A1 dùng để hạ mức N6 | Giữ nội dung sửa (`m_CommonAttrib.nExpireTime + 1451581200`), nhưng hạ xuống **NẶNG-chưa kích hoạt**, ưu tiên sau N3/N7. Nhánh lỗi trả **-2** cho khớp gốc |
| **N2** `GetNpcAroundPlayerList` bỏ `Npc[nMe].m_Index<=0` ⇒ **đọc `m_Region[]` ngoài mảng** | Khuôn gốc `:363` có kiểm — ĐÚNG, bản mới `:1344-1345` thiếu — ĐÚNG. **Nhưng hệ quả thì SAI**: `KNpc::Remove()` (`KNpc.cpp:7315`) gọi `Init()`, và `KNpc::Init()` đặt **`m_Index=0` (`:139`) VÀ `m_RegionIndex=-1` (`:156`) cùng lúc**; `KNpc::KNpc()` (`:116-122`) cũng gọi `Init()` ⇒ **mọi khe chưa dùng / đã giải phóng đều có `m_RegionIndex = -1`**, đúng thứ mà bản mới **đã chặn** ở `:1345`. Không có đường nào đặt `m_Index=0` mà bỏ qua `m_RegionIndex` (grep `m_Index = 0` trong `KNpc.cpp`/`KNpcSet.cpp` = 2 kết quả, 1 là khối đã comment). Thêm: ở chỗ gọi duy nhất `bigboss.lua:218` NPC **đang trong script chết nên `m_Index > 0`** ⇒ kiểm thêm cũng **không đổi gì** | **THỔI PHỒNG** — lệch khuôn có thật, "đọc ngoài mảng" **không chứng minh được**; A1 xếp **ưu tiên 1/11** | Hạ xuống **NHẸ** (nhất quán khuôn). Vẫn nên thêm `m_Index<=0`, nhưng **đừng** xếp trước N3/N7 |
| **N3** vá `DisabledUseTownP` vô tác dụng + `forbidmap.lua` chưa hề thêm map | `grep GetDisabledUseTownP` toàn `bin\server\{script,settings,scriptjx2}` = **0 tệp .lua** — ĐÚNG. `IsDisabledUseTownP` chỉ có ở `clearskill\head.lua:144`, `grep '"IsDisabledUseTownP"'` trong `Sources` = 0 — ĐÚNG. Đọc `script\header\forbidmap.lua:94-119`: `CheckAllMaps` **chỉ 984** + 6 nhóm cũ, `TRAINMAPS` (có 336) **không được dùng** — ĐÚNG. `CheckAllMaps` đúng là cửa chặn của `item\townportal_l.lua` (9 tệp gọi) — ĐÚNG. Map PLD = **336/337/338/339** (`fld_head.lua:12-14`) — ĐÚNG | **ĐÚNG**, nhưng **cách sửa 1 VÔ ÍCH** | **Bỏ phương án 1**: đổi tên thành `IsDisabledUseTownP` **không vá được gì** — hàm duy nhất gọi nó là `CSP_CheckValid` (`clearskill\head.lua:143`), mà **cả 2 chỗ gọi `CSP_CheckValid` đều đang bị comment** (`global\npcchucnang\phantang.lua:12`, `global\ÌØÊâÓÃµØ\ÃÎ¾³\npc\Â·ÈË_ÅÑÉ®.lua:12`). **Chỉ phương án 2 là thật**: thêm 336/337/338/339 + map Vượt Ải vào `CheckAllMaps` |
| **N4** `AddStatData` ghi `log_game\` — thư mục không tồn tại | `ls bin\server\log_game` = **không có**; `bin\server\logs\` **có** — ĐÚNG. `g_GetRootPath` cắt `\` cuối (`KFilePath.cpp:110-116`) — ĐÚNG. Thiếu kiểm `localtime` NULL ở `:1177` — ĐÚNG | **ĐÚNG lõi + THỔI PHỒNG mức** — lập luận "chuỗi syscall hỏng lặp lại trên luồng logic đơn" không đứng: `fopen` hỏng vài chục µs, tần suất là **mỗi người chết ở PLD**, không phải mỗi tick; và **thống kê là log vận hành, không đụng luật chơi** | Hạ xuống **NHẸ**. Nội dung sửa (`logs\` + kiểm `pTm` + cờ thôi thử lại) giữ nguyên |
| **N5** `IniFile_Save` dùng `KIniFile::Save` bị dự án cấm; `MAX_CHUNK=10` ⇒ `sprintf(NULL,…)` | `KJx2SharedStore.cpp:2-3` / `KJx2League.cpp:2` / `KJx2CityWar.cpp:2` đúng là ghi luật cấm — ĐÚNG. `KMemStack::Free` no-op (`KMemStack.cpp:76-79`), `MAX_CHUNK 10` (`KMemStack.h:12`), `AllocNewChunk` gọi `g_MessageBox` rồi `Push` trả NULL, `KIniFile.cpp:319-325` **không kiểm NULL** trước `sprintf` — ĐÚNG. **Nhưng cơ chế cạn thì A1 mô tả SAI**: `Push` chỉ cấp chunk mới khi `m_nStackTop + nSize > m_nStackEnd`; `GetStackSize()` = `m_nStackTop + (m_nChunkTop-1)*m_nChunkSize` ⇒ mỗi lần `Save` **nhân đôi** `m_nStackTop`, và `if (m_nChunkSize < nSize) m_nChunkSize = nSize` còn **phình cỡ chunk** ⇒ cạn theo **cấp số nhân**, không phải "1 chunk / lần lưu" | **ĐÚNG (rủi ro thật) + 1 bằng chứng SAI** | Giữ mức **NẶNG-chưa kích hoạt** và giữ cách sửa (gỡ đăng ký, hoặc `tmp + MoveFileEx`). Sửa mô tả: **rò theo cấp số nhân**, sập sớm hơn "9 lần" nếu ini lớn, muộn hơn nếu ini nhỏ |
| **N6** 6 hàm mới không có lời gọi thật | `grep {tbActivityCompose, tbProgressBar, getItemInfo, ini_setdata, ini_save, ini_loadfile}` trong `src_utf8` (trừ chính tệp lib) = **0** — ĐÚNG. **Phạm vi A1 tuyên bố hẹp hơn thực tế**: trên **cây sống** thì `tbActivityCompose` CÓ ở `scriptjx2\lib\composelistex.lua:21` và `OpenProgressBar` CÓ ở `scriptjx2\lib\player.lua` — tôi kiểm tiếp: **không tệp nào `Include` hai tệp đó** ⇒ kết luận vẫn đứng | **ĐÚNG** (kết luận đúng, phạm vi quét mô tả thiếu) | Ghi rõ phạm vi: "0 lời gọi **được nạp**", kèm 2 tệp `scriptjx2\lib\` là mã treo |
| **N7** `GetRoomItems` vứt tham số khoang | Đọc `:1268-1295`: hàm **không hề đọc `Lua_ValueToNumber(L,1)`**, luôn quét `{room_equipment, room_equipmentex}` — ĐÚNG. `composeex.lua` cho phép `nRoomType` — ĐÚNG | **ĐÚNG** | Giữ. Bổ sung: `room_equipmentex` chỉ hợp lệ khi `Player[].m_dwEquipExpandTime - KSG_GetCurSec() > 0` (`KItemList.cpp:155`) ⇒ bản port đếm cả **hành trang đã hết hạn** |
| **N8** `BT_GetBattleParam` đẩy số, gốc trả chuỗi | Tự dịch ngược `0x081C69B0`: `0x081C6A2B call 0x804aeec` (dựng `std::string` từ phần tử vector) rồi `0x081C6A3A call 0x82337a0` — **khác** `0x8232d40` (`lua_pushnumber` đã thấy ở `ITEM_GetExpiredTime`) ⇒ đúng là **đẩy CHUỖI**. **Nhưng**: chỗ gọi `battlehead.lua:631/639` nằm trong `function sf_buildfightnpcdata()` (`:625`), mà `grep sf_buildfightnpcdata` toàn cây sống = **1 kết quả = chính dòng định nghĩa** ⇒ **mã chết** | **ĐÚNG kiểu trả về + THỔI PHỒNG mức** (A1 tự ghi "CHƯA XÁC MINH" rồi vẫn xếp NẶNG, ưu tiên 6/11) | Hạ xuống **NHẸ**. Vẫn đổi `Lua_PushString(L,"")` (1 dòng, rẻ) |
| **N9** `ITEM_SetExpiredTime(idx,0)` ⇒ hết hạn ngay | Tự dịch ngược helper `0x081F2760`: `0x081F2771 test esi,esi / je 0x81f27b6` → `ebx=0` → **trả 0 = không đặt hạn**; `0x081F2775 cmp esi,0x1312d00 / jbe` → ≤20e6 là **phút**; nhánh ngày `0x081F27AC cmp ebx,eax / mov eax,-1 / cmovbe ebx,eax` → **-1 khi mốc đã qua**. Bản port `:1419` `nExpireSec = KSG_GetCurSec() + 0` = **now** ⇒ `GetExpireTime()` trả 0 ngay — ĐÚNG. `droptemplet.lua:129` chỉ `if … then` (Lua coi 0 là true) — ĐÚNG | **ĐÚNG** | Giữ nguyên |
| **N10** `IsDisabledUseHeart` 0 lời gọi | `grep IsDisabledUseHeart` toàn cây sống = **0 tệp .lua** — ĐÚNG | **ĐÚNG** | Giữ. **Nhưng A1 dừng quá sớm**: cùng một nguyên nhân (`item\heart_head.lua` bản Linux không được chép) còn gây một lỗi **CHẶN** mà A1 không thấy — xem DC.2 / M1 |
| **§3** đăng ký nằm trong `#ifdef _SERVER` (âm tính) | Tự quét ngăn xếp tiền xử lý toàn tệp: dòng **3247/3274/3286** → trong `#ifdef _SERVER@2643`; **15388/15416/15434** → trong `#ifdef _SERVER@14420`; **13850** → trong `#ifdef _SERVER@**13392**` (A1 ghi khối extern "13850-13883, `#endif` ở 13884" — hàm ý `#ifdef` mở ở 13850, **sai**; nó mở ở 13392) | **ĐÚNG (kết luận) + 1 bằng chứng SAI ở chi tiết dòng** | Sửa `13850` → "nằm trong `#ifdef _SERVER` mở tại **13392**, đóng tại 13884" |
| **§4** không trùng tên (âm tính) | Tự quét `ScriptFuns.cpp:14354-15468`: **1048 mục / 1045 tên duy nhất**, 3 cặp trùng `BT_GetData(14403,15270)`, `GetLeadLevel(14579,14734)`, `GetMissionS(14837,15330)` — **khớp từng con số** với A1 | **ĐÚNG** | Giữ |
| **§8.1/8.2** `missions\boss\` + `settings\trigger_` không đụng JX1 (âm tính) | `find script/missions/boss -type f` = **1 tệp `bigboss.lua`** (có trong manifest); `ls settings/*.lua` = **2 tệp `trigger_challengeoftime.lua`, `trigger_include.lua`** (đều trong manifest) — ĐÚNG. Tôi kiểm thêm 2 tiền tố A1 **bỏ qua**: `missions\fengling_ferry\` (14 tệp) và `missions\challengeoftime\` (16 .lua) — **không tệp JX1 nào bị kéo vào** ⇒ kết luận âm tính mở rộng vẫn đúng. Tiền tố `\settings\trigger_` **có tác dụng thật** (không phải mã chết): `hd3_driver.lua:92,123` gọi `DynamicExecute("\\settings\\trigger_challengeoftime.lua","OnTrigger")` ⇒ tệp có state riêng | **ĐÚNG** | Giữ; ghi thêm 2 tiền tố đã kiểm bổ sung |
| **H1** `\script\vng_feature\` kéo nhầm `checkinmap.lua` | `find script/vng_feature` = 4 tệp, `checkinmap.lua` **không có trong `b1_manifest.txt`** — ĐÚNG | **ĐÚNG** | Giữ (NHẸ) |
| **H6** JX1 đã có `KPlayerSet::GetFirstPlayer/GetNextPlayer` | `KPlayerSet.h:155-156`, `KPlayerSet.cpp:178/184`; dùng thật ở `ScriptFuns.cpp:3695/3701/3740/3746/3784` **và 9675/9684** (đọc `:9670-9690`, đúng là vòng `PlayerSet.GetFirstPlayer()`) — ĐÚNG | **ĐÚNG** | Giữ |
| **H9** `OpenProgressBar` giữ `m_dwTimeBoxId` cũ khi tên rỗng | Đọc `LuaOpenTimeBox` `:9750-9762`: **luôn** đặt lại (`g_FileName2Id(szScript)` hoặc `m_ActionScriptID`); bản port `:1722-1723` chỉ đặt khi `szSelf[0]` — ĐÚNG. Quy đổi `/18` đúng | **ĐÚNG** | Giữ |
| **H12** tràn `szSection[32]` qua `IniFile_SetData` | `KIniFile::SetKeyValue` `:761` `char szSection[32]` + `g_StrCat` không chặn; `GetKeyValue` `:858` dùng `[64]` — ĐÚNG. `LuaIniFile_SetData:3255` truyền thẳng chuỗi Lua qua `WriteString → SetKeyValue` (`KIniFile.cpp:1133`) — ĐÚNG | **HẠ THẤP** — đây là **ghi đè ngăn xếp từ dữ liệu script**, cùng lớp rủi ro với N5 mà A1 xếp NẶNG; xếp NHẸ là không nhất quán | Nâng lên **NẶNG-chưa kích hoạt**. Nếu theo cách sửa của N5 (gỡ 3 đăng ký) thì H12 **tự hết** — nên gộp H12 vào việc N5 |
| **H15** `mapSeen` là thừa | `KInventory::FindItem` `:162-163` trả **-1** cho ô nối tiếp; port lọc `nItemIdx > 0` ⇒ không thể lặp — ĐÚNG | **ĐÚNG** | Giữ |
| **§6** `SetItemBindState(-2)` → khoá thật | `KItem.h:313-316`; `KPlayer.cpp:3755/5155/5237` đều `InsuranceCourse > 0 \|\| == -2` (thêm `InsuranceHourCourse > 0`) — ĐÚNG | **ĐÚNG** | Giữ |
| **§2.2** ngưỡng 20 000 000 + gốc 1451581200 | Đã tự dịch ngược (xem N9) + `KItem.cpp:2632` `nowtime = nExpireTime + 1451581200` — ĐÚNG | **ĐÚNG** | Giữ |
| **§1.2** `room_equipmentex` cũng 6×10 | `GameDataDef.h:371-376`: `EQUIPMENT_ROOM_ 6×10`, `REPOSITORY_ROOM_ 6×10`; `KItemList.cpp:879` `m_Room[room_equipmentex].Init(REPOSITORY_…)` — ĐÚNG | **ĐÚNG** | Giữ |
| **§1.2** `DropItemEx` kiểm biên ĐẠT | `KObjSet::Add` `:242` `if (nAddNo < 0) return -1` ⇒ kiểm `nObj == -1` của port là **đúng quy ước** — ĐÚNG. Thứ tự 19 tham số `AddItemSet2` khớp chữ ký `KItemSet.cpp:197-200` — ĐÚNG. `SetExpTime(0,0,0,0)` vô hại vì `KItem.cpp:3086 if (bYear)` — ĐÚNG | **ĐÚNG** | Giữ |

**Tổng đối chất: 22 kiểm → 13 ĐÚNG nguyên vẹn · 4 THỔI PHỒNG (N1 ưu tiên, N2, N4, N8) · 1 HẠ THẤP (H12) · 2 bằng chứng sai chi tiết (N1 "-2" không phải 0; N5 "1 chunk/lần") · 1 bằng chứng sai dòng (§3) · 1 cách sửa vô ích (N3 phương án 1).**

### DC.2 — Bỏ sót của chính vòng soát

> **Lỗ hổng phương pháp gốc:** A1 tuyên bố đã đối chiếu "**mọi chỗ gọi thật** … cây sống `E:\SourceTuanLe\…\bin\server`" và kết luận "**0 CHẶN**", nhưng **chưa từng mở nhật ký chạy thật** của chính cây đó. Máy chủ đã nạp bộ script mới lúc **2026/08/25 01:48-01:52** và ghi lỗi vào `bin\server\ScriptError.log` (56 KB) cùng 3 tệp `ScriptError.log` theo thư mục. Chỉ cần `find . -name ScriptError.log -newermt 2026-08-24` là ra. Đây là bằng chứng **mạnh hơn mọi suy luận tĩnh** trong báo cáo, và nó **bác thẳng dòng "0 CHẶN"**.
> *(Lưu ý phản biện tự thân: `CoreServer.dll.moi_2508_3hoatdong` **chưa swap**, nên lỗi kiểu "hàm engine nil" có thể do DLL cũ. Tôi đã loại trừ: cả 5 lỗi dưới đây đều **thuần Lua**, không phụ thuộc 41 hàm mới. Với M2 tôi còn tách theo NGÀY để loại lỗi có sẵn.)*

| # | Mức | Chỗ | Nội dung + bằng chứng |
|---|---|---|---|
| **M1** | 🔴 **CHẶN** | `script\missions\challengeoftime\chuangguang30.lua:249` | `FORBITMAP_LIST[CHUANGGUAN30_MAP_ID] = 1` — biến này do **`item\heart_head.lua:27` bản Linux** khai báo (`FORBITMAP_LIST = {}`), mà tệp đó **KHÔNG được chép** (JX1 đã có `heart_head.lua` riêng, chính sách "không đè"). ⇒ `attempt to index global 'FORBITMAP_LIST' (a nil value)`, **chunk chính đứt tại dòng 273** ⇒ **`ChuangGuan30:Init()` (274) và `ChuangGuan30:RegistAll()` (275) KHÔNG BAO GIỜ CHẠY** ⇒ **Vượt Ải chết hoàn toàn**. Bằng chứng: `script\missions\challengeoftime\ScriptError.log` + `…\npc\ScriptError.log`, **2026/08/25 01:48:55**. **Đợt port trước đã gặp đúng lỗi này và đã vá** — `script\missions\tongcastle\game.lua:163-164` `FORBITMAP_LIST = FORBITMAP_LIST or {}` — đợt này **không áp lại**. **Sửa:** thêm `FORBITMAP_LIST = FORBITMAP_LIST or {}` ngay trước vòng lặp ở `ChuangGuan30:SetForbitItem()` |
| **M2** | 🔴 **CHẶN** | `script\activitysys\config\41\extend.lua` | Thư mục `script\activitysys\config\41\` trên cây sống **chỉ có mỗi `extend.lua`** — 3 tệp mà chính nó `Include` ở dòng 1-3 (`head.lua`, `variables.lua`, `data.lua`) **không được chép** (`b1_manifest.txt` chỉ liệt kê `extend.lua`; thư mục `config\41` **không tồn tại** trong bản giải mã `src_utf8`) ⇒ `pActivity` nil ở dòng 11 ⇒ `attempt to index global 'pActivity' (a nil value)`, chunk đứt. Bằng chứng: `script\activitysys\config\41\ScriptError.log`, **2026/08/25 01:48:53**. **Sửa:** chép đủ bộ `config\41\{head,variables,data}.lua`, **hoặc GỠ `extend.lua` khỏi cây sống** (hoạt động 41 = `2012april_zhushuai`, **không thuộc 3 hoạt động** — nhiều khả năng là tệp chép lây) |
| **M3** | 🟠 **NẶNG** | `script\task\tollgate\killer\kill_level.lua` | Tệp này là **ActionScript của 160 boss Sát Thủ** (`script\task\tollgate\killbosshead.lua:6-…`). JX1 gọi `ExecuteScript(ActionScript, "OnRevive", m_Index)` khi NPC hồi sinh (`KNpc.cpp:8676`, `ScriptFuns.cpp:7198`), nhưng tệp Linux **chỉ định nghĩa `OnDeath` (dòng 22)**, **không có `OnRevive`** ⇒ `attempt to call a nil value`, **160 lần** (đúng bằng số boss) trong 4 phút. **Đã loại trừ lỗi có sẵn**: tách theo ngày trong `ScriptError.log` cho thấy `kill_level.lua/OnRevive` **chỉ xuất hiện ngày 25/08** (160), trong khi `turebug90.lua/OnRevive` có từ **23/08 (9) và 24/08 (117)** ⇒ M3 **do đợt này gây ra**, `turebug90` thì không. Hệ quả: móc hồi sinh không chạy + rác nhật ký ghi đĩa trên **luồng logic đơn** (đây mới là "chuỗi syscall hỏng lặp lại" thật, không phải N4). **Sửa:** thêm `function OnRevive(nNpcIndex) end` (hoặc nội dung tương ứng bản JX1 `bosssatthuhead.lua`) |
| **M4** | 🟠 **NẶNG** | `script\item\lenhbaiadmin.lua:107` | Mục menu mới `"Hoạt động Linux (Săn Boss Sát Thủ / Phong Lăng Độ / Vượt ải): test/HD3_AdminMenu"` — **nhãn chứa ký tự `/`**, mà quy ước `Say`/`SayEx` là `"nhãn/tên_hàm"`. Bộ tách lấy nhầm đoạn giữa ⇒ engine gọi `Lua_GetGlobal(L, " Phong Lăng Độ / Vượt ải): test")` (`KLuaScript.cpp:218`) ⇒ `attempt to call a nil value`. Bằng chứng: `bin\server\ScriptError.log` **2026/08/25 01:52:05** (`lenhbaiadmin.lua`) và **01:51:58** (`tasklink_goods.lua`), cFuncName in đúng chuỗi rác đó. **Mọi mục kề bên đều không có `/` trong nhãn** (vd `:100` `"Hoạt động 23-24.08 (BC-BN-TV-TB): bộ test/HD_AdminMenu"`). ⇒ **menu test của cả 3 hoạt động không mở được**. **Sửa:** bỏ hết `/` trong nhãn, vd `"Hoạt động Linux (Sát Thủ - Phong Lăng Độ - Vượt ải): test/HD3_AdminMenu"` |
| **M5** | 🟡 NHẸ (tiềm ẩn NẶNG) | `KJx2WarInfra.cpp:1656-1677` | A1 (N3) chỉ hỏi "**ai ĐỌC** cờ" mà **không hề liệt kê ai GHI**. Sau khi stub `LuaDisabledUseTownP` (`:258-263`) được chuyển sang bản thật, số chỗ ghi cờ **không phải 7 như BÀN GIAO** mà là **42 lời gọi trong 24 tệp** — trong đó **20 tệp là script JX1/đợt port cũ** (Bách Nhân, Bạch Ải, Công Thành, Liên Đấu, Tống Kim `collectgoods`/`tong_disciple`/`tong_springfestival`, Tống Chiến, Tín Sứ `messenger_*`…), chỉ 4 tệp thuộc 3 hoạt động mới. Thêm nữa: `s_byHD3NoTownP[]` **không được xoá khi thoát game hay khi khe PlayerIndex được tái dùng** — chỉ xoá khi script gọi `DisabledUseTownP(0)`. Hôm nay vô hại vì cờ là **chỉ-ghi**, nhưng **ngay khi nối đường đọc** (đúng việc mà mục "Việc phải làm" số 3 của A1 đề xuất) thì 20 tệp kia đổi hành vi cùng lúc, và người chơi mới vào khe cũ **thừa kế cờ**. **Sửa:** xoá `s_byHD3NoTownP[i]/s_nHD3NoTownPWorld[i]` tại điểm nạp/huỷ người chơi; và **trước khi nối đường đọc, phải rà đủ 42 chỗ ghi** |

### DC.3 — DANH SÁCH CHỐT (đã qua đối chất) — 15 lỗi thật

| Ưu tiên | Mã | Mức | Chỗ | Sửa |
|---|---|---|---|---|
| 1 | **M1** | **CHẶN** | `challengeoftime\chuangguang30.lua:249` | `FORBITMAP_LIST = FORBITMAP_LIST or {}` (khuôn `tongcastle\game.lua:164`) |
| 2 | **M2** | **CHẶN** | `activitysys\config\41\extend.lua` | Chép đủ `head/variables/data.lua`, hoặc **gỡ tệp** |
| 3 | **M4** | NẶNG | `item\lenhbaiadmin.lua:107` | Bỏ `/` trong nhãn menu |
| 4 | **M3** | NẶNG | `task\tollgate\killer\kill_level.lua` | Thêm `OnRevive` |
| 5 | **N3** (ph.án 2) | NẶNG | `script\header\forbidmap.lua:94-119` | Thêm **336/337/338/339** + map Vượt Ải vào `CheckAllMaps` (**bỏ** phương án đổi tên `IsDisabledUseTownP`) |
| 6 | **N9** | NẶNG | `KJx2WarInfra.cpp:1400,1419` | `dVal == 0` ⇒ **không đặt hạn** (trả 0); ngày đã qua ⇒ trả **-1** |
| 7 | **N7** | NẶNG (chưa kích hoạt) | `KJx2WarInfra.cpp:1268-1295` | Đọc & tôn trọng `nRoomType`; khoang không hỗ trợ ⇒ **bảng rỗng**; bỏ `room_equipmentex` khi đã hết hạn mở rộng |
| 8 | **N5 + H12** | NẶNG (chưa kích hoạt) | `ScriptFuns.cpp:3274-3284` + `KIniFile.cpp:761` | **Gỡ 3 đăng ký** `IniFile_SetData`/`IniFile_Save`/`File_Create` (hết luôn H12); nếu giữ thì `tmp + MoveFileEx` **và** chặn `szSect ≤ 28` |
| 9 | **N1** | NẶNG (chưa kích hoạt) | `KJx2WarInfra.cpp:1429-1450` | Trả `m_CommonAttrib.nExpireTime + 1451581200` (0 vẫn 0); nhánh lỗi trả **-2** |
| 10 | **M5** | NHẸ (tiềm ẩn NẶNG) | `KJx2WarInfra.cpp:1656-1677` | Xoá cờ khi thoát/tái dùng khe; rà đủ **42** chỗ ghi trước khi nối đường đọc |
| 11 | **N4** | NHẸ | `KJx2WarInfra.cpp:1166-1181` | `log_game\` → `logs\`; kiểm `pTm` NULL; cờ thôi thử lại |
| 12 | **N2** | NHẸ | `KJx2WarInfra.cpp:1344` | Thêm `Npc[nMe].m_Index <= 0` (nhất quán khuôn — **không** phải chống tràn mảng) |
| 13 | **N8** | NHẸ (mã chết) | `KJx2WarInfra.cpp:1796-1801` | `Lua_PushString(L, "")` |
| 14 | **N10** | NHẸ (thiếu tính năng) | `ScriptFuns.cpp:15416` | Gộp vào việc số 5 (chặn theo map là đường JX1 dùng thật) |
| 15 | **H1, H6, H9, H15, N6** | NHẸ | — | Giữ nguyên đề xuất của A1 |

**Không tìm thấy lỗi mới trong phần mã C++ thuần** ngoài M5: 22 điểm kiểm ở DC.1 cho thấy phần chữ ký/kiểm biên của khối `[3HD]` **đã được soát khá chắc**. Rủi ro thật của đợt này **nằm ở phía script và ở khâu chép tệp**, đúng chỗ mà báo cáo A1 không soi tới.
