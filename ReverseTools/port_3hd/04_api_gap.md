# 04 — BẢNG KHE HỞ API ENGINE cho 3 hoạt động port từ bản Linux (JX2) sang JX1

| Mục | Nội dung |
|---|---|
| Ngày lập | 24/08/2026 |
| Nguồn danh sách tệp | `D:\GAMEDEVNEW\ReverseTools\port_3hd\closure3.json` (bao đóng Include) |
| Cây Lua tham chiếu | `D:\ServerLinux\server1\script` + `\vng_script` — 5.230 tệp `.lua` |
| Bảng đăng ký JX1 | `D:\GAMEDEVNEW\Sources\Core\Src\ScriptFuns.cpp` (`GameScriptFuns[]` dòng 14257, `WorldScriptFuns[]` dòng 15325) + `LuaFuns.cpp` (`SysFuns[]` dòng 43) + các `.cpp` khác trong `Core\Src` |
| Ảnh nhị phân | `D:\ServerLinux\server1\jx_linux_y` (ELF32 i386, đã xoá section header) |
| Bản đồ tên → địa chỉ | `D:\GAMEDEVNEW\ReverseTools\jx_linux_y.luamap.full.txt` (1.560 tên) |

## 0. Kết quả một dòng

**280** tên hàm ENGINE bị 3 hoạt động gọi tới. **235 JX1 đã có**, **42 phải viết mới**, **8 cảnh báo tên-giống-nghĩa-khác**, **3 tên bị xếp nhầm** (thực ra không phải hàm engine).

```
  Tổng tên được gọi trong bao đóng   : 521
   − trừ từ khoá + thư viện chuẩn Lua 4.0
   − trừ hàm do CHÍNH cây script Linux định nghĩa (8.044 tên toàn cục)
  ⇒ hàm ENGINE                       : 280
       (A) JX1 ĐÃ CÓ                 : 235
       (B) CHƯA CÓ — CẦN VIẾT MỚI    :  42
       (D) không phải hàm engine     :   3   ContriValueEntryLogic, JudgePLAddTitle, Require
       (C) cảnh báo nghĩa khác       :   9 tên / 8 dòng bảng   [đã sửa theo phản biện]
                                     (8 nằm trong 235 của nhóm A, riêng JoinMission thì không;
                                      dòng "SetNpcScript / SetNpcDeathScript" gộp HAI tên)
```

## 1. Bảng tóm tắt theo tính năng (bước 4)

| Tính năng | Tệp trong bao đóng | Hàm engine dùng | (A) đã có | (B) phải viết | Trong đó nằm ở TỆP LÕI của tính năng |
|---|---|---|---|---|---|
| **Săn boss sát thủ (Killer Boss)** | 67 | 208 | 180 | **28** | `AddStatData`, `ITEM_DropRateItem`, `NPCINFO_GetSeries` |
| **Phong Lăng Độ (fengling_ferry)** | 82 (1 tệp thiếu — xem phụ lục phản biện) | 215 | 187 | **28** | `AddStatData`, `ITEM_SetExpiredTime` |
| **Vượt ải (challengeoftime)** | 103 | 268 | 229 | **39** | `AddStatData`, `NpcName2Replace` |

### 1.1 Phân bố 42 hàm nhóm B

**Dùng chung cả 3 tính năng — 25 hàm** (viết một lần, cả ba cùng hưởng):

> `AddStatData`, `DropItemEx`, `File_Create`, `GetFirstPlayerAtServer`, `GetGlodEqIndex`, `GetItemAllParams`, `GetItemGenTime`, `GetItemQuality`, `GetNextPlayerAtServer`, `GetNpcAroundPlayerList`, `GetPlatinaEquipIndex`, `GetPlatinaLevel`, `GetRoomItems`, `ITEM_GetExpiredTime`, `ITEM_GetItemRandSeed`, `ITEM_SetExpiredTime`, `ITEM_SetLeftUsageTime`, `IniFile_Save`, `IniFile_SetData`, `NpcDropMoney`, `ST_IsTransLife`, `SendScriptData`, `SetItemBindState`, `Tm2Time`, `TrimString`

**Chỉ riêng Săn boss sát thủ (Killer Boss) — 2 hàm:** `ITEM_DropRateItem`, `NPCINFO_GetSeries`

**Chỉ riêng Phong Lăng Độ (fengling_ferry) — 1 hàm:** `OpenProgressBar`

**Chỉ riêng Vượt ải (challengeoftime) — 11 hàm:** `IsDisabledUseHeart`, `NpcName2Replace`, `PET_GetGrownPoint`, `PET_GetTamePoint`, `PET_GetUpgradePoint`, `PET_SetGrownPoint`, `PET_SetTamePoint`, `PET_SetUpgradePoint`, `QueryWiseManForSB`, `ST_DoTransLife`, `ST_LevelUp`

**Dùng bởi 2 tính năng — 3 hàm:** `Add120SkillExp` (satthu + vuotai), `BT_GetBattleParam` (phonglangdo + vuotai), `FormatTime2Date` (phonglangdo + vuotai)

> **[đã sửa theo phản biện] Hàm thứ 43 — `JoinMission` là khe hở nhóm B THẬT của Săn boss sát thủ.** `missions/challengeoftime/npc/dragonboat_main.lua` **có** trong bao đóng `satthu`, còn tệp định nghĩa hàm script cùng tên (`missions/challengeoftime/mission_match.lua:93`) thì **KHÔNG** ⇒ trong bản port `satthu`, `JoinMission(MISSION_MATCH, 1)` (`dragonboat_main.lua:163`) **không có gì che**, buộc phải là hàm **engine** (`luamap 0x08137E40`) mà JX1 chưa đăng ký. Với `vuotai` thì cả hai tệp đều có nên hàm script đè lên. Vậy số hàm phải viết cho `satthu` là **29** chứ không phải 28 (hoặc phải kéo thêm `mission_match.lua` vào bao đóng `satthu`).

> **Nhận xét quan trọng.** 25/42 hàm nhóm B **không** nằm trong tệp lõi của bất kỳ tính năng nào, mà đến từ **thư viện dùng chung** do bao đóng Include kéo vào: `lib/droptemplet.lua`, `lib/log.lua`, `lib/file.lua`, `lib/composeex.lua`, `lib/string.lua`, `missions/boss/bigboss.lua`, `activitysys/*`, `battles/battlehead.lua`. Viết xong 25 hàm này là gỡ được nút thắt cho cả ba hoạt động.

## 2. Nhóm (B) — 42 hàm engine JX1 chưa có, phải viết mới

> **[đã sửa theo phản biện]** Cột cuối **không phải** "số lần gọi": nó là **tổng (số tính năng × số chỗ gọi)**.
> Ví dụ `DropItemEx` chỉ có **một** chỗ gọi duy nhất trong cả bao đóng (`lib/droptemplet.lua:74`) nhưng ghi 3 vì cả 3 tính năng đều kéo tệp đó.

| # | Tên hàm | Địa chỉ ELF | Chữ ký suy ra | Độ tin cậy | Dùng bởi | Tính năng × chỗ gọi |
|---|---|---|---|---|---|---|
| 1 | **`Add120SkillExp`** | `0x0811C710` | `Add120SkillExp(nExp) → 1 số` | KHÁ CHẮC | satthu + vuotai | 2 |
| 2 | **`AddStatData`** | `0x080FF550` | `AddStatData(szTên [, nSốLượng]) → 0 giá trị` | CHẮC | phonglangdo + satthu + vuotai | 12 |
| 3 | **`BT_GetBattleParam`** | `0x081C69B0` | `BT_GetBattleParam(nChỉSố) → 1 chuỗi` | KHÁ CHẮC | phonglangdo + vuotai | 4 |
| 4 | **`DropItemEx`** | `0x0811FD70` | `DropItemEx(nSubWorldIdx, nMpsX, nMpsY, nBelonger, nVersion, szRandSeed, nQuality, nItemGenre, nItemDetailType, nItemParticularType, nItemLevel, nItemSeries, nItemLuck, nMagicLevel1..nMagicLevel6) → 1 số (chỉ số vật phẩm rơi, 0 = thất bại)` | CHẮC | phonglangdo + satthu + vuotai | 3 |
| 5 | **`File_Create`** | `0x0814A120` | `File_Create(szTênTệp) → 1 số` | CHẮC | phonglangdo + satthu + vuotai | 3 |
| 6 | **`FormatTime2Date`** | `0x081022B0` | `FormatTime2Date(nThờiGianUnix) → 1 số dạng YYYYMMDD` | CHẮC | phonglangdo + vuotai | 4 |
| 7 | **`GetFirstPlayerAtServer`** | `0x08101CF0` | `GetFirstPlayerAtServer() → 1 số (PlayerIndex, 0 = hết)` | CHẮC | phonglangdo + satthu + vuotai | 3 |
| 8 | **`GetGlodEqIndex`** | `0x080FEF90` | `GetGlodEqIndex(nItemIndex) → 1 số (chỉ số hàng trong bảng trang bị HOÀNG KIM, 0 nếu không phải)` | CHẮC | phonglangdo + satthu + vuotai | 8 |
| 9 | **`GetItemAllParams`** | `0x08102D20` | `GetItemAllParams(nItemIndex) → 1 **bảng Lua** {nMagLvl1..nMagLvl6}` | KHÁ CHẮC | phonglangdo + satthu + vuotai | 3 |
| 10 | **`GetItemGenTime`** | `0x080FE820` | `GetItemGenTime(nItemIndex) → 1 số (mốc thời gian tạo vật phẩm)` | KHÁ CHẮC | phonglangdo + satthu + vuotai | 3 |
| 11 | **`GetItemQuality`** | `0x080FEFB0` | `GetItemQuality(nItemIndex) → 1 số (0 = thường, 1 = hoàng kim, 4 = bạch kim)` | CHẮC | phonglangdo + satthu + vuotai | 8 |
| 12 | **`GetNextPlayerAtServer`** | `0x08101D20` | `GetNextPlayerAtServer() → 1 số (PlayerIndex, 0 = hết)` | CHẮC | phonglangdo + satthu + vuotai | 3 |
| 13 | **`GetNpcAroundPlayerList`** | `0x08104870` | `GetNpcAroundPlayerList(nNpcIndex, nBánKính [, nThamSố3]) → **2 giá trị**: (bảng PlayerIndex, nSốLượng)` | KHÁ CHẮC | phonglangdo + satthu + vuotai | 3 |
| 14 | **`GetPlatinaEquipIndex`** | `0x080FEF70` | `GetPlatinaEquipIndex(nItemIndex) → 1 số (chỉ số hàng trong bảng trang bị BẠCH KIM, 0 nếu không phải)` | CHẮC | phonglangdo + satthu + vuotai | 6 |
| 15 | **`GetPlatinaLevel`** | `0x08100520` | `GetPlatinaLevel(nItemIndex) → 1 giá trị: số, hoặc **nil** nếu vật phẩm không phải bạch kim` | CHẮC | phonglangdo + satthu + vuotai | 3 |
| 16 | **`GetRoomItems`** | `0x0810D170` | `GetRoomItems(nRoomType) → 1 **bảng** chỉ số vật phẩm` | KHÁ CHẮC | phonglangdo + satthu + vuotai | 3 |
| 17 | **`ITEM_DropRateItem`** | `0x08154DE0` | `ITEM_DropRateItem(nNpcIndex, nSốLần, szĐườngDẫnIni, nThamSố4, nThamSố5, nNgũHành) → 0 giá trị` | KHÁ CHẮC | satthu | 1 |
| 18 | **`ITEM_GetExpiredTime`** | `0x08154540` | `ITEM_GetExpiredTime(nItemIndex) → 1 số (nil hoặc ≤ 0 = không có hạn)` | KHÁ CHẮC | phonglangdo + satthu + vuotai | 3 |
| 19 | **`ITEM_GetItemRandSeed`** | `0x081548E0` | `ITEM_GetItemRandSeed(nItemIndex) → 1 số (hạt giống ngẫu nhiên, không dấu)` | KHÁ CHẮC | phonglangdo + satthu + vuotai | 3 |
| 20 | **`ITEM_SetExpiredTime`** | `0x08154A30` | `ITEM_SetExpiredTime(nItemIndex, nGiáTrị [, nKiểu]) → 1 số` | KHÁ CHẮC (đơn vị của tham số 2: CHƯA XÁC MINH) | phonglangdo + satthu + vuotai | 9 |
| 21 | **`ITEM_SetLeftUsageTime`** | `0x08154FD0` | `ITEM_SetLeftUsageTime(nItemIndex, nThờiGian) → 1 số` | KHÁ CHẮC | phonglangdo + satthu + vuotai | 6 |
| 22 | **`IniFile_Save`** | `0x0814B160` | `IniFile_Save(szTệpNguồn, szTệpĐích) → 1 số` | CHẮC | phonglangdo + satthu + vuotai | 3 |
| 23 | **`IniFile_SetData`** | `0x0814AAF0` | `IniFile_SetData(szTệp, szSection, szKey, szValue) → 0 giá trị` | CHẮC | phonglangdo + satthu + vuotai | 3 |
| 24 | **`IsDisabledUseHeart`** | `0x0812ED70` | `IsDisabledUseHeart() → 1 số (0/1)  ·  **tham số bị bỏ qua**` | CHẮC | vuotai | 1 |
| 25 | **`NPCINFO_GetSeries`** | `0x081C08E0` | `NPCINFO_GetSeries(nNpcIndex) → 1 số (ngũ hành của NPC)` | CHẮC | satthu | 1 |
| 26 | **`NpcDropMoney`** | `0x0811D9C0` | `NpcDropMoney(nNpcIndex, nSốTiền, nBelongerIndex) → 0 giá trị` | CHẮC | phonglangdo + satthu + vuotai | 3 |
| 27 | **`NpcName2Replace`** | `0x081006D0` | `NpcName2Replace(szTênNpc) → 1 chuỗi` | KHÁ CHẮC | vuotai | 1 |
| 28 | **`OpenProgressBar`** | `0x081082D0` | `OpenProgressBar(szTiêuĐề, nSốKhung, nCờSựKiện, bDesc, szHàmOnTime, szHàmOnBreak) → 0 giá trị` | KHÁ CHẮC | phonglangdo | 1 |
| 29 | **`PET_GetGrownPoint`** | `0x081D5FE0` | `PET_GetGrownPoint() → 1 số` | KHÁ CHẮC | vuotai | 1 |
| 30 | **`PET_GetTamePoint`** | `0x081D5F90` | `PET_GetTamePoint() → 1 số` | KHÁ CHẮC | vuotai | 1 |
| 31 | **`PET_GetUpgradePoint`** | `0x081D6150` | `PET_GetUpgradePoint() → 1 số` | KHÁ CHẮC | vuotai | 1 |
| 32 | **`PET_SetGrownPoint`** | `0x081D6230` | `PET_SetGrownPoint(nGiáTrị) → 0 giá trị` | KHÁ CHẮC | vuotai | 1 |
| 33 | **`PET_SetTamePoint`** | `0x081D5F10` | `PET_SetTamePoint(nGiáTrị) → 0 giá trị` | KHÁ CHẮC | vuotai | 1 |
| 34 | **`PET_SetUpgradePoint`** | `0x081D70E0` | `PET_SetUpgradePoint(nGiáTrị) → 0 giá trị` | KHÁ CHẮC | vuotai | 1 |
| 35 | **`QueryWiseManForSB`** | `0x0811A270` | `QueryWiseManForSB(szCallbackOK, szCallbackFail, nThờiGianChờ, szTênNhânVật) → 0 giá trị` | KHÁ CHẮC | vuotai | 2 |
| 36 | **`ST_DoTransLife`** | `0x081C2990` | `ST_DoTransLife() → 1 số (0/1 thành công)` | KHÁ CHẮC | vuotai | 1 |
| 37 | **`ST_IsTransLife`** | `0x081C1160` | `ST_IsTransLife() → 1 số (0 / 1)` | CHẮC | phonglangdo + satthu + vuotai | 13 |
| 38 | **`ST_LevelUp`** | `0x081C2A60` | `ST_LevelUp(nSốCấp) → 0 giá trị` | KHÁ CHẮC | vuotai | 1 |
| 39 | **`SendScriptData`** | `0x0810B220` | `SendScriptData(nProtocolId, nHandle) → 1 số` | KHÁ CHẮC | phonglangdo + satthu + vuotai | 3 |
| 40 | **`SetItemBindState`** | `0x08127630` | `SetItemBindState(nItemIndex, nBindState) → 0 hoặc 1 giá trị` | CHẮC | phonglangdo + satthu + vuotai | 5 |
| 41 | **`Tm2Time`** | `0x08103AC0` | `Tm2Time(nNăm, nTháng, nNgày, nGiờ, nPhút, nGiây) → 1 số (thời gian Unix)` | CHẮC | phonglangdo + satthu + vuotai | 6 |
| 42 | **`TrimString`** | `0x080FF630` | `TrimString() → 0 giá trị` | CHẮC | phonglangdo + satthu + vuotai | 3 |

### 2.1 Cách suy chữ ký — các địa chỉ API Lua đã xác định

Chữ ký suy từ mã máy i386: đếm `lua_tonumber(L,i)` / `lua_tostring(L,i)` theo thứ tự chỉ số, và đọc trị trả về của hàm C (Lua 4.0: **trị trả về của hàm C = số giá trị đã đẩy lên ngăn xếp**). Các địa chỉ API Lua dưới đây được xác định bằng cách **đọc trực tiếp thân hàm**, không đoán:

| Địa chỉ | Là hàm | Bằng chứng |
|---|---|---|
| `0x08232490` | `lua_gettop(L)` | `(L->top − L->stack) / 12` — `TObject` của Lua 4.0 dài 12 byte |
| `0x08232D40` | `lua_pushnumber(L, double)` | ghi tag `2` (`LUA_TNUMBER`) rồi `fstp qword [eax+4]` |
| `0x082338B0` | `lua_tonumber(L, idx)` | kiểm tag `2`, `fld qword [ebx+4]` |
| `0x08233850` | `lua_tostring(L, idx)` | kiểm tag `3` (`LUA_TSTRING`), trả `str + 0x14` |
| `0x082337A0` | `lua_pushstring(L, char*)` | gọi `strlen` (PLT `0x0804AEDC`) rồi `pushlstring` |
| `0x08232E70` | `lua_pushnil(L)` | nhánh "không phải bạch kim" của `GetPlatinaLevel` |
| `0x08232BE0` | `lua_newtable(L)` | gọi ngay trước vòng đổ 6 phần tử của `GetItemAllParams` |
| `0x082339B0` | `lua_tonumber → int` | bọc `lua_tonumber` + `fistp` |
| `0x08107860` / `0x08107910` | `GetGlobalPlayerIndex(L)` | đọc **biến toàn cục Lua `PlayerIndex`** (chuỗi tại `0x08251227`), trả −1 nếu không phải số |

Cấu trúc dữ liệu suy ra kèm:

- **Bản ghi người chơi**: mảng gốc `[0x08BAEE60]`, bước **`0x8788`** byte, chỉ số hợp lệ **1…1199** (`lea ecx,[eax-1]` rồi `cmp ecx, 0x4AE`).
- **Bản ghi vật phẩm**: mảng gốc `[0x0830D300]`, bước **`0x368`** byte, số lượng `[0x0830CA5C]`, trường `[+4]` = **phẩm chất**, `[+0x1E0]` = 6 cấp thuộc tính ngẫu nhiên.

### 2.2 Chi tiết từng hàm

#### `Add120SkillExp`

- **Địa chỉ bản Linux**: `0x0811C710`
- **Chữ ký**: `Add120SkillExp(nExp) → 1 số`
- **Độ tin cậy**: KHÁ CHẮC
- **Mô tả**: Cộng `nExp` vào **độ tu luyện kỹ năng cấp 120** của nhân vật hiện tại (lấy qua biến toàn cục `PlayerIndex`). Thân hàm có `lua_gettop` + 1 tham số số, có 3 nhánh `lua_pushnumber` (0 hoặc giá trị mới).
- **Số liệu dịch ngược**: 65 lệnh · `lua_gettop` = có · tham số đọc được = {p1 : number} · đẩy về = {'lua_pushnumber': 3} · trị trả về của hàm C = [1]
- **Dùng bởi**: satthu + vuotai
- **Chỗ gọi tiêu biểu**: `task/task_award_extend.lua:6`

#### `AddStatData`

- **Địa chỉ bản Linux**: `0x080FF550`
- **Chữ ký**: `AddStatData(szTên [, nSốLượng]) → 0 giá trị`
- **Độ tin cậy**: CHẮC
- **Mô tả**: Cộng `nSốLượng` (mặc định 1) vào **bộ đếm thống kê** mang tên `szTên` của máy chủ. Thân hàm: `lua_gettop` phải bằng 1 hoặc 2 (`lea eax,[eax-1]; cmp eax,1; jbe`), `lua_tostring(L,1)`, nếu top==2 thì `lua_tonumber(L,2)` ngược lại dùng 1, rồi gọi `0x081D0420` trên đối tượng toàn cục `0x0978C0A0` với (tên, số, 0). Chuỗi `pcszName` nằm trong thân hàm. Phong Lăng Độ dùng để đếm số người chết từng thuyền (`fld_death.lua:30/32/34`).
- **Số liệu dịch ngược**: 63 lệnh · `lua_gettop` = có · tham số đọc được = {p1 : string, p2 : number} · đẩy về = {} · trị trả về của hàm C = [0]
- **Chuỗi trong thân hàm**: `pcszName`
- **Dùng bởi**: phonglangdo + satthu + vuotai  ·  nằm trong **tệp lõi** của: phonglangdo + satthu + vuotai
- **Chỗ gọi tiêu biểu**: `activitysys/config/32/talkdailytask.lua:159`, `missions/fengling_ferry/fld_death.lua:30`, `missions/fengling_ferry/fld_death.lua:32`, `task/tollgate/killer/lib_killlevel.lua:145`, `task/tollgate/killer/shashou_mibao.lua:23`

#### `BT_GetBattleParam`

- **Địa chỉ bản Linux**: `0x081C69B0`
- **Chữ ký**: `BT_GetBattleParam(nChỉSố) → 1 chuỗi`
- **Độ tin cậy**: KHÁ CHẮC
- **Mô tả**: Trả về tham số cấu hình thứ `nChỉSố` (đếm từ 1) của trận chiến hiện hành, dạng chuỗi `"npcid,level,count"` — `battlehead.lua:632` tách bằng `getNpcInfo(str)` ra 3 số. Thân hàm: lấy id trận qua `0x08106A40`, đọc vector chuỗi tại `[battle+0x4EF5C .. +0x4EF60]` bước 4 byte, rồi `lua_pushstring`.
- **Số liệu dịch ngược**: 90 lệnh · `lua_gettop` = không · tham số đọc được = {p1 : number} · đẩy về = {'lua_pushstring': 2} · trị trả về của hàm C = [1]
- **Dùng bởi**: phonglangdo + vuotai
- **Chỗ gọi tiêu biểu**: `battles/battlehead.lua:631`, `battles/battlehead.lua:639`

#### `DropItemEx`

- **Địa chỉ bản Linux**: `0x0811FD70`
- **Chữ ký**: `DropItemEx(nSubWorldIdx, nMpsX, nMpsY, nBelonger, nVersion, szRandSeed, nQuality, nItemGenre, nItemDetailType, nItemParticularType, nItemLevel, nItemSeries, nItemLuck, nMagicLevel1..nMagicLevel6) → 1 số (chỉ số vật phẩm rơi, 0 = thất bại)`
- **Độ tin cậy**: CHẮC
- **Mô tả**: Thả xuống đất một vật phẩm **đã xác định đầy đủ thuộc tính**. Chữ ký lấy từ **chính script**: `lib/droptemplet.lua:59` có dòng chú thích liệt kê đủ 19 tham số, và lời gọi thật ở dòng 74–93 khớp đúng thứ tự đó. Lưu ý `szRandSeed` truyền bằng **chuỗi** (`format("%u", ...)`) vì giá trị vượt phạm vi số nguyên có dấu.
- **Số liệu dịch ngược**: 185 lệnh · `lua_gettop` = có · tham số đọc được = {p1 : number/string, p2 : number, p3 : number, p4 : number} · đẩy về = {'lua_pushnumber': 2} · trị trả về của hàm C = [1]
- **Dùng bởi**: phonglangdo + satthu + vuotai
- **Chỗ gọi tiêu biểu**: `lib/droptemplet.lua:74`

#### `File_Create`

- **Địa chỉ bản Linux**: `0x0814A120`
- **Chữ ký**: `File_Create(szTênTệp) → 1 số`
- **Độ tin cậy**: CHẮC
- **Mô tả**: Tạo tệp rỗng (dùng cho tệp .ini chưa tồn tại). `lib/file.lua:8` gọi ngay sau khi `IniFile_Load` trả 0.
- **Số liệu dịch ngược**: 92 lệnh · `lua_gettop` = không · tham số đọc được = {p1 : string} · đẩy về = {'lua_pushnumber': 3} · trị trả về của hàm C = [1]
- **Dùng bởi**: phonglangdo + satthu + vuotai
- **Chỗ gọi tiêu biểu**: `lib/file.lua:8`

#### `FormatTime2Date`

- **Địa chỉ bản Linux**: `0x081022B0`
- **Chữ ký**: `FormatTime2Date(nThờiGianUnix) → 1 số dạng YYYYMMDD`
- **Độ tin cậy**: CHẮC
- **Mô tả**: Đổi mốc thời gian Unix sang số nguyên ngày. Thân hàm chứa chuỗi định dạng `%04d%02d%02d`. Kết quả đưa thẳng cho `ITEM_SetExpiredTime` (`.../qianqiu_yinglie/head.lua:200-201`).
- **Số liệu dịch ngược**: 70 lệnh · `lua_gettop` = có · tham số đọc được = {p1 : number} · đẩy về = {'lua_pushnumber': 2} · trị trả về của hàm C = [1]
- **Chuỗi trong thân hàm**: `%04d%02d%02d`
- **Dùng bởi**: phonglangdo + vuotai
- **Chỗ gọi tiêu biểu**: `event/jiefang_jieri/200904/qianqiu_yinglie/head.lua:200`, `event/jiefang_jieri/200904/qianqiu_yinglie/head.lua:217`

#### `GetFirstPlayerAtServer`

- **Địa chỉ bản Linux**: `0x08101CF0`
- **Chữ ký**: `GetFirstPlayerAtServer() → 1 số (PlayerIndex, 0 = hết)`
- **Độ tin cậy**: CHẮC
- **Mô tả**: Bắt đầu duyệt **toàn bộ người chơi đang online** trên GameServer này. Thân hàm gọi `0x080C3EE0` trên đối tượng danh sách `0x08BAF320` rồi `lua_pushnumber`. Không tham số.
- **Số liệu dịch ngược**: 15 lệnh · `lua_gettop` = không · tham số đọc được = {} (không đọc tham số Lua nào) · đẩy về = {'lua_pushnumber': 1} · trị trả về của hàm C = [1]
- **Dùng bởi**: phonglangdo + satthu + vuotai
- **Chỗ gọi tiêu biểu**: `missions/boss/bigboss.lua:289`

#### `GetGlodEqIndex`

- **Địa chỉ bản Linux**: `0x080FEF90`
- **Chữ ký**: `GetGlodEqIndex(nItemIndex) → 1 số (chỉ số hàng trong bảng trang bị HOÀNG KIM, 0 nếu không phải)`
- **Độ tin cậy**: CHẮC
- **Mô tả**: Là một **thunk**: nạp hằng số 1 rồi nhảy vào hàm chung `0x080FEEB0`. Hàm chung: `lua_tonumber(L,1)` = chỉ số vật phẩm, kiểm `0 < idx < g_nItemCount` (`[0x0830CA5C]`), lấy bản ghi kích thước `0x368` từ bảng `[0x0830D300]`, **so sánh trường `[+4]` (= phẩm chất vật phẩm) với hằng số**; khớp thì trả chỉ số hàng, không khớp trả `0.0`. Hằng số 1 = hoàng kim, khớp với script `if nQuality == 1 then`.
- **Số liệu dịch ngược**: 11 lệnh · `lua_gettop` = không · tham số đọc được = {} (không đọc tham số Lua nào) · đẩy về = {} · trị trả về của hàm C = []
- **Dùng bởi**: phonglangdo + satthu + vuotai
- **Chỗ gọi tiêu biểu**: `event/jiefang_jieri/200904/qianqiu_yinglie/head.lua:67`, `lib/composeex.lua:142`, `lib/log.lua:39`

#### `GetItemAllParams`

- **Địa chỉ bản Linux**: `0x08102D20`
- **Chữ ký**: `GetItemAllParams(nItemIndex) → 1 **bảng Lua** {nMagLvl1..nMagLvl6}`
- **Độ tin cậy**: KHÁ CHẮC
- **Mô tả**: Tạo bảng mới (`lua_newtable` = `0x08232BE0`) rồi đổ 6 giá trị liên tiếp từ bản ghi vật phẩm ở độ lệch `+0x1E0`, bước 4 byte. Trả 0 giá trị nếu chỉ số không hợp lệ. Script đọc `arynMagLvl[1..6]` (`lib/log.lua:46,50`).
- **Số liệu dịch ngược**: 60 lệnh · `lua_gettop` = có · tham số đọc được = {p1 : number} · đẩy về = {'lua_pushnumber': 1} · trị trả về của hàm C = [0, 1]
- **Dùng bởi**: phonglangdo + satthu + vuotai
- **Chỗ gọi tiêu biểu**: `lib/log.lua:46`

#### `GetItemGenTime`

- **Địa chỉ bản Linux**: `0x080FE820`
- **Chữ ký**: `GetItemGenTime(nItemIndex) → 1 số (mốc thời gian tạo vật phẩm)`
- **Độ tin cậy**: KHÁ CHẮC
- **Mô tả**: Đọc trường thời gian sinh của bản ghi vật phẩm. Script in bằng `%0.0f` vì giá trị vượt phạm vi 32-bit có dấu (`lib/log.lua:51`).
- **Số liệu dịch ngược**: 40 lệnh · `lua_gettop` = có · tham số đọc được = {p1 : number} · đẩy về = {'lua_pushnumber': 1} · trị trả về của hàm C = [1]
- **Dùng bởi**: phonglangdo + satthu + vuotai
- **Chỗ gọi tiêu biểu**: `lib/log.lua:51`

#### `GetItemQuality`

- **Địa chỉ bản Linux**: `0x080FEFB0`
- **Chữ ký**: `GetItemQuality(nItemIndex) → 1 số (0 = thường, 1 = hoàng kim, 4 = bạch kim)`
- **Độ tin cậy**: CHẮC
- **Mô tả**: Đọc trường `[+4]` của bản ghi vật phẩm (bảng `[0x0830D300]`, bước `0x368`). Trả 0 khi chỉ số không hợp lệ.
- **Số liệu dịch ngược**: 46 lệnh · `lua_gettop` = có · tham số đọc được = {p1 : number} · đẩy về = {'lua_pushnumber': 2} · trị trả về của hàm C = [1]
- **Dùng bởi**: phonglangdo + satthu + vuotai
- **Chỗ gọi tiêu biểu**: `event/jiefang_jieri/200904/qianqiu_yinglie/head.lua:65`, `lib/composeex.lua:138`, `lib/log.lua:37`

#### `GetNextPlayerAtServer`

- **Địa chỉ bản Linux**: `0x08101D20`
- **Chữ ký**: `GetNextPlayerAtServer() → 1 số (PlayerIndex, 0 = hết)`
- **Độ tin cậy**: CHẮC
- **Mô tả**: Bước kế tiếp của vòng duyệt ở trên (`0x080C3F10`, cùng đối tượng). Cặp đôi với `GetFirstPlayerAtServer`; xem `missions/boss/bigboss.lua:289-294`.
- **Số liệu dịch ngược**: 15 lệnh · `lua_gettop` = không · tham số đọc được = {} (không đọc tham số Lua nào) · đẩy về = {'lua_pushnumber': 1} · trị trả về của hàm C = [1]
- **Dùng bởi**: phonglangdo + satthu + vuotai
- **Chỗ gọi tiêu biểu**: `missions/boss/bigboss.lua:293`

#### `GetNpcAroundPlayerList`

- **Địa chỉ bản Linux**: `0x08104870`
- **Chữ ký**: `GetNpcAroundPlayerList(nNpcIndex, nBánKính [, nThamSố3]) → **2 giá trị**: (bảng PlayerIndex, nSốLượng)`
- **Độ tin cậy**: KHÁ CHẮC
- **Mô tả**: Liệt kê người chơi trong bán kính `nBánKính` quanh NPC. Thân hàm có `lua_gettop` + 3 tham số số và trả 2 giá trị (`mov eax,2; ret`). Chỗ gọi: `missions/boss/bigboss.lua:218` → `local tbRoundPlayer, nCount = GetNpcAroundPlayerList(nNpcIndex, 20)` (chỉ dùng 2 tham số ⇒ tham số 3 có mặc định).
- **Số liệu dịch ngược**: 119 lệnh · `lua_gettop` = có · tham số đọc được = {p1 : number, p2 : number, p3 : number} · đẩy về = {'lua_pushnumber': 3} · trị trả về của hàm C = [2]
- **Dùng bởi**: phonglangdo + satthu + vuotai
- **Chỗ gọi tiêu biểu**: `missions/boss/bigboss.lua:218`

#### `GetPlatinaEquipIndex`

- **Địa chỉ bản Linux**: `0x080FEF70`
- **Chữ ký**: `GetPlatinaEquipIndex(nItemIndex) → 1 số (chỉ số hàng trong bảng trang bị BẠCH KIM, 0 nếu không phải)`
- **Độ tin cậy**: CHẮC
- **Mô tả**: Cùng hàm chung `0x080FEEB0` như `GetGlodEqIndex` nhưng hằng số = 4 (bạch kim; script kiểm `if nQuality == 4`).
- **Số liệu dịch ngược**: 11 lệnh · `lua_gettop` = không · tham số đọc được = {} (không đọc tham số Lua nào) · đẩy về = {} · trị trả về của hàm C = []
- **Dùng bởi**: phonglangdo + satthu + vuotai
- **Chỗ gọi tiêu biểu**: `lib/composeex.lua:144`, `lib/log.lua:44`

#### `GetPlatinaLevel`

- **Địa chỉ bản Linux**: `0x08100520`
- **Chữ ký**: `GetPlatinaLevel(nItemIndex) → 1 giá trị: số, hoặc **nil** nếu vật phẩm không phải bạch kim`
- **Độ tin cậy**: CHẮC
- **Mô tả**: Kiểm `[+4] == 4`; nếu không thì gọi `lua_pushnil` (`0x08232E70`) mà **vẫn trả 1 giá trị**. Script bọc bằng `tostring(...)` nên nil vẫn an toàn (`lib/log.lua:43`).
- **Số liệu dịch ngược**: 67 lệnh · `lua_gettop` = có · tham số đọc được = {p1 : number} · đẩy về = {'lua_pushnumber': 1} · trị trả về của hàm C = [0, 1]
- **Dùng bởi**: phonglangdo + satthu + vuotai
- **Chỗ gọi tiêu biểu**: `lib/log.lua:43`

#### `GetRoomItems`

- **Địa chỉ bản Linux**: `0x0810D170`
- **Chữ ký**: `GetRoomItems(nRoomType) → 1 **bảng** chỉ số vật phẩm`
- **Độ tin cậy**: KHÁ CHẮC
- **Mô tả**: Trả danh sách chỉ số vật phẩm trong một "phòng" (túi / trang bị / rương) theo `nRoomType`. `lib/composeex.lua:191` — nhánh `else` của trường hợp ô giao dịch (nhánh kia dùng `GetGiveItemUnit` mà JX1 đã có).
- **Số liệu dịch ngược**: 77 lệnh · `lua_gettop` = không · tham số đọc được = {p1 : number} · đẩy về = {'lua_pushnumber': 1} · trị trả về của hàm C = [1]
- **[đã sửa theo phản biện]** Bộ đếm trên **bỏ sót `lua_newtable`**: thân hàm gọi `0x08232BE0` (`lua_newtable`) tại `0x0810D1B4` rồi `0x08232D40` + `0x08233360` (đổ phần tử) ⇒ **đúng là trả 1 BẢNG**, chữ ký ở trên chuẩn. Ngoài ra hàm **mở đầu bằng `GetGlobalPlayerIndex` (`0x08107860`)** ⇒ nó lấy vật phẩm của **nhân vật hiện tại** (biến toàn cục `PlayerIndex`), tham số duy nhất chỉ là `nRoomType` — mô tả cũ không nói rõ điều này.
- **Dùng bởi**: phonglangdo + satthu + vuotai
- **Chỗ gọi tiêu biểu**: `lib/composeex.lua:191`

#### `ITEM_DropRateItem`

- **Địa chỉ bản Linux**: `0x08154DE0`
- **Chữ ký**: `ITEM_DropRateItem(nNpcIndex, nSốLần, szĐườngDẫnIni, nThamSố4, nThamSố5, nNgũHành) → 0 giá trị`
- **Độ tin cậy**: KHÁ CHẮC
- **Mô tả**: Thả vật phẩm theo **bảng tỉ lệ trong tệp .ini** (ví dụ `\\settings\\droprate\\boss\\bosstask_lev90.ini`) tại vị trí NPC. Thân hàm: `lua_gettop` + 6 tham số (số, số, chuỗi, số, số, số), không đẩy giá trị trả về. Chỗ gọi duy nhất: `task/tollgate/killer/kill_level.lua:69` → `ITEM_DropRateItem(nNpcIndex, 8, ini, 0, 10, nseries)`.
- **Số liệu dịch ngược**: 145 lệnh · `lua_gettop` = có · tham số đọc được = {p1 : number, p2 : number, p3 : string, p4 : number, p5 : number, p6 : number} · đẩy về = {} · trị trả về của hàm C = [0]
- **Dùng bởi**: satthu  ·  nằm trong **tệp lõi** của: satthu
- **Chỗ gọi tiêu biểu**: `task/tollgate/killer/kill_level.lua:69`

#### `ITEM_GetExpiredTime`

- **Địa chỉ bản Linux**: `0x08154540`
- **Chữ ký**: `ITEM_GetExpiredTime(nItemIndex) → 1 số (nil hoặc ≤ 0 = không có hạn)`
- **Độ tin cậy**: KHÁ CHẮC
- **Mô tả**: Đọc hạn sử dụng còn lại của vật phẩm. `activitysys/activity.lua:314-317` coi nil hoặc ≤ 0 là "không có hạn".
- **Số liệu dịch ngược**: 51 lệnh · `lua_gettop` = có · tham số đọc được = {p1 : number} · đẩy về = {'lua_pushnumber': 2} · trị trả về của hàm C = [1]
- **Dùng bởi**: phonglangdo + satthu + vuotai
- **Chỗ gọi tiêu biểu**: `activitysys/activity.lua:314`

#### `ITEM_GetItemRandSeed`

- **Địa chỉ bản Linux**: `0x081548E0`
- **Chữ ký**: `ITEM_GetItemRandSeed(nItemIndex) → 1 số (hạt giống ngẫu nhiên, không dấu)`
- **Độ tin cậy**: KHÁ CHẮC
- **Mô tả**: Đọc RandSeed của vật phẩm; script in bằng `%0.0f`. Cặp đôi với tham số `szRandSeed` của `DropItemEx`.
- **Số liệu dịch ngược**: 52 lệnh · `lua_gettop` = có · tham số đọc được = {p1 : number} · đẩy về = {'lua_pushnumber': 2} · trị trả về của hàm C = [1]
- **Dùng bởi**: phonglangdo + satthu + vuotai
- **Chỗ gọi tiêu biểu**: `lib/log.lua:48`

#### `ITEM_SetExpiredTime`

- **Địa chỉ bản Linux**: `0x08154A30`
- **Chữ ký**: `ITEM_SetExpiredTime(nItemIndex, nGiáTrị [, nKiểu]) → 1 số`
- **Độ tin cậy**: KHÁ CHẮC (đơn vị của tham số 2: CHƯA XÁC MINH)
- **Mô tả**: Đặt hạn sử dụng cho vật phẩm. **CẢNH BÁO**: hai chỗ gọi trong bao đóng dùng **hai đơn vị khác nhau** — `event/.../qianqiu_yinglie/head.lua:201` truyền YYYYMMDD (kết quả `FormatTime2Date`), còn `missions/fengling_ferry/bossdeath.lua:35` truyền **số phút** còn lại trong ngày. Thân hàm đọc 3 tham số số và có 4 lần `lua_pushnumber`, nhiều khả năng tham số thứ 3 (có mặc định) chọn đơn vị. **Phải đối chiếu lại trước khi hiện thực.**
- **Số liệu dịch ngược**: 100 lệnh · `lua_gettop` = có · tham số đọc được = {p1 : number, p2 : number, p3 : number} · đẩy về = {'lua_pushnumber': 4} · trị trả về của hàm C = [1]
- **Dùng bởi**: phonglangdo + satthu + vuotai  ·  nằm trong **tệp lõi** của: phonglangdo
- **Chỗ gọi tiêu biểu**: `event/jiefang_jieri/200904/qianqiu_yinglie/head.lua:201`, `lib/droptemplet.lua:130`, `missions/fengling_ferry/bossdeath.lua:35`, `event/birthday_jieri/200905/class.lua:95`

#### `ITEM_SetLeftUsageTime`

- **Địa chỉ bản Linux**: `0x08154FD0`
- **Chữ ký**: `ITEM_SetLeftUsageTime(nItemIndex, nThờiGian) → 1 số`
- **Độ tin cậy**: KHÁ CHẮC
- **Mô tả**: Đặt thời gian sử dụng còn lại của vật phẩm. `lib/droptemplet.lua:135,140`.
- **Số liệu dịch ngược**: 77 lệnh · `lua_gettop` = có · tham số đọc được = {p1 : number, p2 : number} · đẩy về = {'lua_pushnumber': 2} · trị trả về của hàm C = [1]
- **Dùng bởi**: phonglangdo + satthu + vuotai
- **Chỗ gọi tiêu biểu**: `lib/droptemplet.lua:135`, `lib/droptemplet.lua:140`

#### `IniFile_Save`

- **Địa chỉ bản Linux**: `0x0814B160`
- **Chữ ký**: `IniFile_Save(szTệpNguồn, szTệpĐích) → 1 số`
- **Độ tin cậy**: CHẮC
- **Mô tả**: Ghi ảnh .ini trong bộ nhớ xuống đĩa. `lib/file.lua:24` gọi với cùng một tên cho cả hai tham số.
- **Số liệu dịch ngược**: 61 lệnh · `lua_gettop` = có · tham số đọc được = {p1 : string, p2 : string} · đẩy về = {'lua_pushnumber': 2} · trị trả về của hàm C = [1]
- **Dùng bởi**: phonglangdo + satthu + vuotai
- **Chỗ gọi tiêu biểu**: `lib/file.lua:24`

#### `IniFile_SetData`

- **Địa chỉ bản Linux**: `0x0814AAF0`
- **Chữ ký**: `IniFile_SetData(szTệp, szSection, szKey, szValue) → 0 giá trị`
- **Độ tin cậy**: CHẮC
- **Mô tả**: Ghi một khoá vào ảnh .ini **đã nạp trong bộ nhớ** (chưa ghi xuống đĩa). Cặp đôi với `IniFile_GetData` / `IniFile_Load` mà JX1 **đã có** (ScriptFuns.cpp:15288–15289).
- **Số liệu dịch ngược**: 62 lệnh · `lua_gettop` = có · tham số đọc được = {p1 : string, p2 : string, p3 : string, p4 : string} · đẩy về = {} · trị trả về của hàm C = [0]
- **Dùng bởi**: phonglangdo + satthu + vuotai
- **Chỗ gọi tiêu biểu**: `lib/file.lua:19`

#### `IsDisabledUseHeart`

- **Địa chỉ bản Linux**: `0x0812ED70`
- **Chữ ký**: `IsDisabledUseHeart() → 1 số (0/1)  ·  **tham số bị bỏ qua**`
- **Độ tin cậy**: CHẮC
- **Mô tả**: Trả 1 nếu nhân vật hiện tại **bị cấm** dùng "Tâm Tâm Tương Ánh Phù". Thân hàm **không đọc tham số Lua nào cả**: nó lấy biến toàn cục `PlayerIndex` (`0x08107860`), gọi `0x080A8C80` lấy cờ trạng thái rồi `test eax, 0x200000`. Script vẫn truyền `PlayerIndex` (`item/heart_head.lua:116`) nhưng engine bỏ qua.
- **Số liệu dịch ngược**: 49 lệnh · `lua_gettop` = không · tham số đọc được = {} (không đọc tham số Lua nào) · đẩy về = {'lua_pushnumber': 2} · trị trả về của hàm C = [1]
- **Dùng bởi**: vuotai
- **Chỗ gọi tiêu biểu**: `item/heart_head.lua:116`

#### `NPCINFO_GetSeries`

- **Địa chỉ bản Linux**: `0x081C08E0`
- **Chữ ký**: `NPCINFO_GetSeries(nNpcIndex) → 1 số (ngũ hành của NPC)`
- **Độ tin cậy**: CHẮC
- **Mô tả**: Trả hệ ngũ hành của NPC. `task/tollgate/killer/kill_level.lua:37` lấy rồi truyền xuống `ITEM_DropRateItem` làm tham số thứ 6.
- **Số liệu dịch ngược**: 44 lệnh · `lua_gettop` = có · tham số đọc được = {p1 : number} · đẩy về = {'lua_pushnumber': 2} · trị trả về của hàm C = [1]
- **Dùng bởi**: satthu  ·  nằm trong **tệp lõi** của: satthu
- **Chỗ gọi tiêu biểu**: `task/tollgate/killer/kill_level.lua:37`

#### `NpcDropMoney`

- **Địa chỉ bản Linux**: `0x0811D9C0`
- **Chữ ký**: `NpcDropMoney(nNpcIndex, nSốTiền, nBelongerIndex) → 0 giá trị`
- **Độ tin cậy**: CHẮC
- **Mô tả**: Thả tiền tại vị trí NPC, quyền sở hữu thuộc về `nBelongerIndex`. `lib/droptemplet.lua:159` dùng cho Kim Nguyên Bảo (biến `nAddJxb`).
- **Số liệu dịch ngược**: 68 lệnh · `lua_gettop` = không · tham số đọc được = {p1 : number, p2 : number, p3 : number} · đẩy về = {} · trị trả về của hàm C = [0]
- **Dùng bởi**: phonglangdo + satthu + vuotai
- **Chỗ gọi tiêu biểu**: `lib/droptemplet.lua:159`

#### `NpcName2Replace`

- **Địa chỉ bản Linux**: `0x081006D0`
- **Chữ ký**: `NpcName2Replace(szTênNpc) → 1 chuỗi`
- **Độ tin cậy**: KHÁ CHẮC
- **Mô tả**: Đổi tên NPC qua **bảng thay thế** (bản địa hoá / bí danh). Thân hàm: `lua_tostring(L,1)`, 2 nhánh `lua_pushstring`. Chỗ gọi có kiểm tồn tại trước: `missions/challengeoftime/npc/transfer.lua:10` → `if NpcName2Replace then ... end` ⇒ **có thể tạm bỏ qua**, không bắt buộc khai báo.
- **Số liệu dịch ngược**: 32 lệnh · `lua_gettop` = không · tham số đọc được = {p1 : string} · đẩy về = {'lua_pushstring': 2} · trị trả về của hàm C = [1]
- **Dùng bởi**: vuotai  ·  nằm trong **tệp lõi** của: vuotai
- **Chỗ gọi tiêu biểu**: `missions/challengeoftime/npc/transfer.lua:10`

#### `OpenProgressBar`

- **Địa chỉ bản Linux**: `0x081082D0`
- **Chữ ký**: `OpenProgressBar(szTiêuĐề, nSốKhung, nCờSựKiện, bDesc, szHàmOnTime, szHàmOnBreak) → 0 giá trị`
- **Độ tin cậy**: KHÁ CHẮC
- **Mô tả**: Mở **thanh tiến trình** trên client; chạy hết thì gọi hàm Lua `szHàmOnTime`, bị ngắt thì gọi `szHàmOnBreak`. Thân hàm đọc tới 7 tham số (chuỗi, số, số, số, chuỗi, chuỗi, chuỗi) nhưng chỗ gọi thật chỉ dùng 6 (`lib/progressbar.lua:95`) ⇒ tham số 7 có mặc định. `nCờSựKiện` là mặt nạ bit dựng bằng `SetBit` (`progressbar.lua:86`).
- **Số liệu dịch ngược**: 157 lệnh · `lua_gettop` = có · tham số đọc được = {p1 : string, p2 : number, p3 : number, p4 : number, p5 : string, p6 : string, p7 : string} · đẩy về = {'lua_pushnumber': 1} · trị trả về của hàm C = [0]
- **Dùng bởi**: phonglangdo
- **Chỗ gọi tiêu biểu**: `lib/progressbar.lua:95`

#### `PET_GetGrownPoint`

- **Địa chỉ bản Linux**: `0x081D5FE0`
- **Chữ ký**: `PET_GetGrownPoint() → 1 số`
- **Độ tin cậy**: KHÁ CHẮC
- **Mô tả**: Đọc **điểm tăng trưởng** thú cưng; trường tại `player+0x873C`.
- **Số liệu dịch ngược**: 24 lệnh · `lua_gettop` = không · tham số đọc được = {} (không đọc tham số Lua nào) · đẩy về = {'lua_pushnumber': 1} · trị trả về của hàm C = [1]
- **Dùng bởi**: vuotai
- **Chỗ gọi tiêu biểu**: `vng_event/thapnienlenhbai/lenhbai_def.lua:391`

#### `PET_GetTamePoint`

- **Địa chỉ bản Linux**: `0x081D5F90`
- **Chữ ký**: `PET_GetTamePoint() → 1 số`
- **Độ tin cậy**: KHÁ CHẮC
- **Mô tả**: Đọc **điểm tu luyện / thuần hoá** thú cưng (trường kề bên, `player+0x8738`).
- **Số liệu dịch ngược**: 24 lệnh · `lua_gettop` = không · tham số đọc được = {} (không đọc tham số Lua nào) · đẩy về = {'lua_pushnumber': 1} · trị trả về của hàm C = [1]
- **Dùng bởi**: vuotai
- **Chỗ gọi tiêu biểu**: `vng_event/thapnienlenhbai/lenhbai_def.lua:392`

#### `PET_GetUpgradePoint`

- **Địa chỉ bản Linux**: `0x081D6150`
- **Chữ ký**: `PET_GetUpgradePoint() → 1 số`
- **Độ tin cậy**: KHÁ CHẮC
- **Mô tả**: Đọc **điểm thăng cấp** thú cưng của nhân vật hiện tại (một trường trong bản ghi người chơi; mảng gốc `[0x08BAEE60]`, bước `0x8788`). Không tham số.
- **Số liệu dịch ngược**: 33 lệnh · `lua_gettop` = không · tham số đọc được = {} (không đọc tham số Lua nào) · đẩy về = {'lua_pushnumber': 2} · trị trả về của hàm C = [1]
- **Dùng bởi**: vuotai
- **Chỗ gọi tiêu biểu**: `vng_event/thapnienlenhbai/lenhbai_def.lua:390`

#### `PET_SetGrownPoint`

- **Địa chỉ bản Linux**: `0x081D6230`
- **Chữ ký**: `PET_SetGrownPoint(nGiáTrị) → 0 giá trị`
- **Độ tin cậy**: KHÁ CHẮC
- **Mô tả**: Ghi `player+0x873C` rồi đồng bộ (`0x081D4CE0`).
- **Số liệu dịch ngược**: 38 lệnh · `lua_gettop` = có · tham số đọc được = {} (không đọc tham số Lua nào) · đẩy về = {} · trị trả về của hàm C = [0]
- **Dùng bởi**: vuotai
- **Chỗ gọi tiêu biểu**: `vng_event/thapnienlenhbai/lenhbai_def.lua:391`

#### `PET_SetTamePoint`

- **Địa chỉ bản Linux**: `0x081D5F10`
- **Chữ ký**: `PET_SetTamePoint(nGiáTrị) → 0 giá trị`
- **Độ tin cậy**: KHÁ CHẮC
- **Mô tả**: Ghi điểm tu luyện thú cưng rồi đồng bộ.
- **Số liệu dịch ngược**: 38 lệnh · `lua_gettop` = có · tham số đọc được = {} (không đọc tham số Lua nào) · đẩy về = {} · trị trả về của hàm C = [0]
- **Dùng bởi**: vuotai
- **Chỗ gọi tiêu biểu**: `vng_event/thapnienlenhbai/lenhbai_def.lua:392`

#### `PET_SetUpgradePoint`

- **Địa chỉ bản Linux**: `0x081D70E0`
- **Chữ ký**: `PET_SetUpgradePoint(nGiáTrị) → 0 giá trị`
- **Độ tin cậy**: KHÁ CHẮC
- **Mô tả**: Đặt điểm thăng cấp thú cưng rồi đồng bộ (gọi hàm thông báo `0x081D4CE0`).
- **Số liệu dịch ngược**: 49 lệnh · `lua_gettop` = có · tham số đọc được = {p1 : number} · đẩy về = {} · trị trả về của hàm C = [0]
- **Dùng bởi**: vuotai
- **Chỗ gọi tiêu biểu**: `vng_event/thapnienlenhbai/lenhbai_def.lua:390`

#### `QueryWiseManForSB`

- **Địa chỉ bản Linux**: `0x0811A270`
- **Chữ ký**: `QueryWiseManForSB(szCallbackOK, szCallbackFail, nThờiGianChờ, szTênNhânVật) → 0 giá trị`
- **Độ tin cậy**: KHÁ CHẮC
- **Mô tả**: Hỏi máy chủ trung tâm (WiseMan / Bishop) về **một nhân vật khác** rồi gọi lại hàm Lua `szCallbackOK` / `szCallbackFail`. Thân hàm: `lua_gettop`, 4 tham số (chuỗi, chuỗi, số, chuỗi), chứa chuỗi định dạng `|-|%d` và `|-|%s` (ghép gói tin). `item/heart_head.lua:129,132`.
- **Số liệu dịch ngược**: 131 lệnh · `lua_gettop` = có · tham số đọc được = {p1 : string, p2 : string, p3 : number, p4 : string} · đẩy về = {} · trị trả về của hàm C = [0]
- **Chuỗi trong thân hàm**: `\|-\|%d`, `\|-\|%s`
- **Dùng bởi**: vuotai
- **Chỗ gọi tiêu biểu**: `item/heart_head.lua:129`, `item/heart_head.lua:132`

#### `ST_DoTransLife`

- **Địa chỉ bản Linux**: `0x081C2990`
- **Chữ ký**: `ST_DoTransLife() → 1 số (0/1 thành công)`
- **Độ tin cậy**: KHÁ CHẮC
- **Mô tả**: Thực hiện chuyển sinh cho nhân vật hiện tại (gọi `0x080B2180` trên bản ghi người chơi, `setne` → 0/1). Không tham số. `task/metempsychosis/translife_6.lua:102`.
- **Số liệu dịch ngược**: 30 lệnh · `lua_gettop` = không · tham số đọc được = {} (không đọc tham số Lua nào) · đẩy về = {'lua_pushnumber': 1} · trị trả về của hàm C = [1]
- **Dùng bởi**: vuotai
- **Chỗ gọi tiêu biểu**: `task/metempsychosis/translife_6.lua:102`

#### `ST_IsTransLife`

- **Địa chỉ bản Linux**: `0x081C1160`
- **Chữ ký**: `ST_IsTransLife() → 1 số (0 / 1)`
- **Độ tin cậy**: CHẮC
- **Mô tả**: Nhân vật hiện tại **đã chuyển sinh hay chưa**. Thân hàm đọc đúng một byte tại `player+0x86B8` rồi `setne`. **Chú ý**: `ST_GetTransLifeCount` (`0x081C1100`, JX1 đã có, ánh xạ tới `LuaGetPlayerReBornValue`) đọc **chính byte đó**. ⇒ Trong JX1 chỉ cần `Lua_PushNumber(L, Player[i].m_cReBorn.GetReBornValue() > 0 ? 1 : 0)`.
- **Số liệu dịch ngược**: 29 lệnh · `lua_gettop` = không · tham số đọc được = {} (không đọc tham số Lua nào) · đẩy về = {'lua_pushnumber': 1} · trị trả về của hàm C = [1]
- **Dùng bởi**: phonglangdo + satthu + vuotai
- **Chỗ gọi tiêu biểu**: `activitysys/playerfunlib.lua:146`, `activitysys/playerfunlib.lua:157`, `event/change_destiny/head.lua:114`, `event/birthday_jieri/200905/class.lua:59`

#### `ST_LevelUp`

- **Địa chỉ bản Linux**: `0x081C2A60`
- **Chữ ký**: `ST_LevelUp(nSốCấp) → 0 giá trị`
- **Độ tin cậy**: KHÁ CHẮC
- **Mô tả**: Nâng cấp độ nhân vật thêm `nSốCấp` (dùng sau chuyển sinh). `vng_event/thapnienlenhbai/lenhbai_def.lua:378` → `ST_LevelUp(nLevel - GetLevel())`.
- **Số liệu dịch ngược**: 34 lệnh · `lua_gettop` = không · tham số đọc được = {p1 : number} · đẩy về = {} · trị trả về của hàm C = [0]
- **Dùng bởi**: vuotai
- **Chỗ gọi tiêu biểu**: `vng_event/thapnienlenhbai/lenhbai_def.lua:378`

#### `SendScriptData`

- **Địa chỉ bản Linux**: `0x0810B220`
- **Chữ ký**: `SendScriptData(nProtocolId, nHandle) → 1 số`
- **Độ tin cậy**: KHÁ CHẮC
- **Mô tả**: Gửi gói dữ liệu script (handle của objbuffer) **xuống client** theo mã giao thức `nProtocolId`. `script_protocol/protocol_def_gs.lua:193` trong `ScriptProtocol:SendData`. Là mặt kia của hệ `OB_*` mà JX1 đã có (`OB_Create`, `OB_PushInt`…). Thân hàm có `lua_gettop` + 2 tham số số, 3 lần `lua_pushnumber`, trả 1 giá trị.
- **Số liệu dịch ngược**: 125 lệnh · `lua_gettop` = có · tham số đọc được = {p1 : number, p2 : number} · đẩy về = {'lua_pushnumber': 3} · trị trả về của hàm C = [1]
- **Dùng bởi**: phonglangdo + satthu + vuotai
- **Chỗ gọi tiêu biểu**: `script_protocol/protocol_def_gs.lua:193`

#### `SetItemBindState`

- **Địa chỉ bản Linux**: `0x08127630`
- **Chữ ký**: `SetItemBindState(nItemIndex, nBindState) → 0 hoặc 1 giá trị`
- **Độ tin cậy**: CHẮC
- **Mô tả**: Đặt trạng thái **khoá (binding)** của vật phẩm; `-2` = khoá (`battles/battlehead.lua:1317` → `SetItemBindState(ItemIdx, -2);` kèm chú thích *绑定*).
- **Số liệu dịch ngược**: 75 lệnh · `lua_gettop` = có · tham số đọc được = {p1 : number, p2 : number} · đẩy về = {'lua_pushnumber': 1} · trị trả về của hàm C = [0, 1]
- **Dùng bởi**: phonglangdo + satthu + vuotai
- **Chỗ gọi tiêu biểu**: `battles/battlehead.lua:1317`, `lib/droptemplet.lua:144`

#### `Tm2Time`

- **Địa chỉ bản Linux**: `0x08103AC0`
- **Chữ ký**: `Tm2Time(nNăm, nTháng, nNgày, nGiờ, nPhút, nGiây) → 1 số (thời gian Unix)`
- **Độ tin cậy**: CHẮC
- **Mô tả**: Hàm **ngược** của `Time2Tm` (JX1 đã có — ScriptFuns.cpp:14891). 6 tham số số, có `lua_gettop` để xử lý thiếu tham số. `activitysys/functionlib.lua:364,380`.
- **Số liệu dịch ngược**: 159 lệnh · `lua_gettop` = có · tham số đọc được = {p1 : number, p2 : number, p3 : number, p4 : number, p5 : number, p6 : number} · đẩy về = {'lua_pushnumber': 1} · trị trả về của hàm C = [0, 1]
- **Dùng bởi**: phonglangdo + satthu + vuotai
- **Chỗ gọi tiêu biểu**: `activitysys/functionlib.lua:364`, `activitysys/functionlib.lua:380`

#### `TrimString`

- **Địa chỉ bản Linux**: `0x080FF630`
- **Chữ ký**: `TrimString() → 0 giá trị`
- **Độ tin cậy**: CHẮC
- **Mô tả**: Cắt khoảng trắng và TAB ở **hai đầu** của chuỗi trong **bộ đệm chuỗi dùng chung** (biến toàn cục `0x09780D54` = con trỏ, `0x09780D58` = độ dài) — đúng bộ đệm mà `PushString` / `AppendString` / `PopString` dùng. Không tham số, không trả giá trị. Cách dùng: `PushString(str); TrimString(); return PopString()` (`lib/string.lua:162-164`). JX1 **đã có** `AppendString` (15218) và `ReplaceString` (15219) ⇒ viết `TrimString` rất rẻ.
- **Số liệu dịch ngược**: 74 lệnh · `lua_gettop` = không · tham số đọc được = {} (không đọc tham số Lua nào) · đẩy về = {} · trị trả về của hàm C = [0]
- **Dùng bởi**: phonglangdo + satthu + vuotai
- **Chỗ gọi tiêu biểu**: `lib/string.lua:163`

## 3. Nhóm (C) — tên giống nhưng nghĩa khác / cần kiểm chứng

Đây là những tên **đã có trong bảng đăng ký JX1** nên bước 2 xếp vào nhóm A, nhưng **hành vi không giống bản JX2**. Nguy hiểm hơn nhóm B: script sẽ chạy **im lặng**, không báo lỗi.

| Tên | Đăng ký trong JX1 | Vấn đề |
|---|---|---|
| **`BT_GetData`** | `ScriptFuns.cpp:14306 -> LuaBT_GetDataStub` | Như trên, **cùng một stub**. Ví dụ `battlehead.lua:769-771` so sánh `BT_GetData(PL_BATTLEPOINT)` ⇒ mọi người chơi đều ra 0. |
| **`BT_GetGameData`** | `ScriptFuns.cpp:14305 -> LuaBT_GetDataStub` | **Tên giống, nghĩa khác.** JX1 có đăng ký nhưng thân hàm là STUB: `Lua_PushNumber(L, 0); return 1;` — **luôn trả 0**. Bản JX2 (`0x081C6230`) đọc kho dữ liệu trận chiến thật. Phong Lăng Độ và Vượt Ải đều kéo `battles/battlehead.lua` vào bao đóng và đọc rất nhiều trường qua `BT_GetGameData` ⇒ mọi nhánh đó sẽ chạy sai **một cách im lặng**. |
| **`JoinMission`** | `KHÔNG có trong bảng đăng ký JX1 — luamap 0x08137E40` | **Cảnh báo trùng tên.** Engine JX2 có hàm `JoinMission` (luamap dòng 917), **nhưng** bao đóng của Vượt Ải lại tự định nghĩa một hàm Lua cùng tên ở `missions/challengeoftime/mission_match.lua:93` → `function JoinMission(RoleIndex, camp)`. Trong Lua, bản định nghĩa sau sẽ **đè lên** hàm engine. Chỗ gọi `missions/challengeoftime/npc/dragonboat_main.lua:163` → `JoinMission(MISSION_MATCH, 1)` truyền **mission id** chứ không phải RoleIndex ⇒ rất có thể dòng này muốn gọi hàm **engine**. Phải xác minh khi port, nếu không sẽ sai âm thầm. |
| **`SetNpcScript / SetNpcDeathScript`** | `ScriptFuns.cpp:14500 / :15190` | **ĐÃ XÁC MINH — luamap ĐÚNG [đã sửa theo phản biện].** Bảng đăng ký trong ELF: con trỏ chuỗi `"SetNpcScript"` (`0x0825AE9E`) nằm ở `0x082E27F8`, con trỏ chuỗi `"SetNpcDeathScript"` (`0x0825B7F7`) nằm ở `0x082E2E70`; **dword kế của CẢ HAI đều là `0x08101500`** ⇒ JX2 **dùng chung một hàm C**. Thân hàm `0x08101500`: `lua_gettop`≥2, `strncpy` 0x4F byte vào `npc+0x1538`, `npc+0x1587=0`, id script vào `npc+0x1588`, và **tham số thứ 3 (tuỳ chọn) ghi vào `npc+0x158C`**; trả 0 giá trị. JX1 tách hai hàm C nhưng **cả hai cũng ghi cùng trường** `Npc[].ActionScript` + `m_ActionScriptID` (`ScriptFuns.cpp:7128-7130` và `KJx2WarInfra.cpp:57-58`) ⇒ **KHÔNG lệch nghĩa**. Ba lệch THẬT phải xử lý khi port: (a) JX1 **bỏ tham số thứ 3**; (b) `LuaSetNpcActionScript` còn chạy thêm `ExecuteScript(szScript,"OnRevive",nNpcIndex)` khi `m_Kind==kind_normal` (`ScriptFuns.cpp:7134`) — JX2 **không có**; (c) `LuaSetNpcDeathScript` (`KJx2WarInfra.cpp:49`) **không** remap `\script\...`→`\scriptjx2\...` như `LuaSetNpcActionScript` làm ⇒ NPC gán script kiểu JX2 qua tên này sẽ trỏ sai tệp. |
| **`TM_GetRestCount`** | `ScriptFuns.cpp:14304 -> LuaTM_GetRestCount` | **Stub**: `Lua_PushNil(L); return 1;` — trả nil để hệ storm tự tắt (ghi chú ngay trên hàm, ScriptFuns.cpp:2247-2249). Bản JX2 tại `0x081C21A0`. |
| **`TM_SetTimer`** | `ScriptFuns.cpp:14303 -> LuaTM_SetTimer` | **Stub có chủ đích**: `Lua_PushNumber(L, 0); return 1;` — không tạo hẹn giờ. Bản JX2 (`0x081C23C0`) tạo timer thật. |
| **`curpack`** | `ScriptFuns.cpp:14299 -> LuaCurPack` | **Stub** trả 0. Ghi chú trong nguồn: JX1 mỗi tệp .lua một `Lua_State` riêng nên khái niệm "pack" của máy ảo bản Linux không tồn tại. |
| **`usepack`** | `ScriptFuns.cpp:14300 -> LuaUsePack` | **Stub** trả 0, cùng lý do với `curpack`. |

### 3.1 Những tên đáng ngờ nhưng ĐÃ KIỂM CHỨNG LÀ ĐÚNG (không thuộc nhóm C)

Ba nhóm dưới đây có tên Lua khác tên hàm C nên thoạt nhìn đáng ngờ; đã đọc thân hàm và **xác nhận đúng nghĩa**:

| Tên Lua | Hàm C trong JX1 | Kết luận |
|---|---|---|
| `AddDex` / `AddEng` / `AddStrg` / `AddVit` | `LuaSetPlayerDexterity`… (ScriptFuns.cpp:9412) | **Đúng.** Tên hàm C là "Set" nhưng `KPlayer::SetBaseDexterity` (KPlayer.cpp:13422) làm `m_nDexterity += nData` — tức là **cộng**, khớp với `Add*` của JX2. |
| `AddProp` / `GetProp` | `LuaAddPropPoint` (15276) / `LuaGetRestPropPoint` (15279) | **Đúng.** Bản JX2 (`0x0810FD60` / `0x0810F9F0`) cộng và đọc đúng một trường `player+0x5924` = điểm tiềm năng còn lại. |
| `ST_GetTransLifeCount` | `LuaGetPlayerReBornValue` (ScriptFuns.cpp:5528) | **Đúng.** Bản JX2 (`0x081C1100`) đọc byte `player+0x86B8`; JX1 đọc `m_cReBorn.GetReBornValue()`. Cùng vai trò — và đây chính là byte mà `ST_IsTransLife` (nhóm B) đọc. |

## 4. Nhóm (D) — 3 tên bị xếp nhầm vào "khe hở engine"

| Tên | Kết luận |
|---|---|
| **`ContriValueEntryLogic`** | **Hàm script, không phải engine.** `script/tong/contribution_entry.lua:190` gán `ContriValueEntryLogic = ContriValueEntryLogic_HEAD;` (thân hàm ở dòng 55). Bộ quét ban đầu chỉ bắt `function <tên>(` và `<tên> = function(` nên không thấy kiểu gán bí danh. Chuỗi `ContriValueEntryLogic` cũng không có trong ELF (`re_disasm --find` = *khong thay chuoi*). → Chỉ cần port tệp `tong/contribution_entry.lua`. |
| **`JudgePLAddTitle`** | **Lỗi của chính script JX2 gốc.** `battles/battlehead.lua:781` và `:783` gọi `JudgePLAddTitle()` nhưng cả cây Linux chỉ định nghĩa `bt_JudgePLAddTitle` (battlehead.lua:844). Không có trong ELF. → Khi port phải sửa thành `bt_JudgePLAddTitle()` hoặc bỏ đi, nếu không Lua báo *attempt to call global*. |
| **`Require`** | **Hàm của client, không có ở máy chủ.** `protocol.lua:109` chỉ gọi trong nhánh `MODEL_GAMECLIENT == 1`; nhánh máy chủ (dòng 106) dùng `DynamicExecuteByPlayer`. Không có trong ELF máy chủ Linux. → **Không cần viết** cho GameServer. |

## 5. Rủi ro khác phát hiện kèm (không phải khe hở API nhưng sẽ làm vỡ bản port)

### 5.1 Hàm toàn cục được định nghĩa NGOÀI bao đóng Include

Bộ quét loại một tên khỏi "khe hở engine" khi cây script Linux có định nghĩa hàm toàn cục cùng tên. Nhưng nếu **tệp định nghĩa không nằm trong bao đóng** của tính năng thì khi port sang JX1 mà không kéo tệp đó theo, Lua sẽ báo *attempt to call global*.

- Tổng tên bị loại vì script tự định nghĩa: **203**
- Trong đó định nghĩa nằm **ngoài** bao đóng: **35**
- Trong số đó, tên **cũng có trong luamap ELF và JX1 chưa có**: chỉ một tên — `JoinMission` (xem cảnh báo nhóm C ở trên).

### 5.2 Các điểm CHƯA XÁC MINH

| Vấn đề | Trạng thái |
|---|---|
| `SetNpcScript` và `SetNpcDeathScript` cùng trỏ tới `0x08101500` trong luamap | **ĐÃ XÁC MINH [đã sửa theo phản biện]** — bảng đăng ký ELF `0x082E27F8` và `0x082E2E70` đều trỏ `0x08101500`: JX2 dùng CHUNG một hàm. Xem mục 3. |
| `ITEM_SetExpiredTime` — tham số 2 là YYYYMMDD hay số phút | **ĐÃ XÁC MINH — CẢ HAI [đã sửa theo phản biện]**. Hàm quy đổi `0x081F2760`: `if (n==0) return 0; if (n <= 0x1312D00 /* 20 000 000 */) return now + n*60;  else return DateTime2Unix(n /*YYYYMMDD*/, thamso3 /*HHMMSS, mặc định 0*/);`. Ngưỡng **20 000 000** tự tách "số phút" khỏi "YYYYMMDD". JX1 phải chép đúng ngưỡng này. |
| `JoinMission` — gọi hàm engine hay hàm script cùng tên | **CHƯA XÁC MINH** — `dragonboat_main.lua:163` truyền mission id, không khớp chữ ký hàm script. |
| `GetNpcAroundPlayerList` — tham số thứ 3 | **CHƯA XÁC MINH** — mã máy đọc 3 số, script chỉ truyền 2. |
| `OpenProgressBar` — tham số thứ 7 | **CHƯA XÁC MINH** — mã máy đọc 7, script chỉ truyền 6. |
| `PET_*` — độ lệch trường trong bản ghi người chơi | Chỉ `+0x873C` (Grown) và `+0x8738` (kề bên) đọc trực tiếp được; ánh xạ Tame/Upgrade dựa vào luamap, **chưa đối chiếu chéo**. |

## 6. Nhóm (A) — 235 hàm JX1 đã có

| Tên Lua | Đăng ký trong JX1 | Địa chỉ bản Linux | Dùng bởi |
|---|---|---|---|
| `AddContribution` | ScriptFuns.cpp:14865 -> LuaJX2_AddContribution | `0x0811A850` | satthu + vuotai |
| `AddCumulateOffer` | ScriptFuns.cpp:14870 -> LuaJX2_AddCumulateOffer | `0x08113F20` | satthu + vuotai |
| `AddDex` | ScriptFuns.cpp:14649 -> LuaSetPlayerDexterity | `0x08119FC0` | phonglangdo + satthu + vuotai |
| `AddEng` | ScriptFuns.cpp:14647 -> LuaSetPlayerEngergy | `0x0811A040` | phonglangdo + satthu + vuotai |
| `AddEventItem` | ScriptFuns.cpp:14421 -> LuaAddEventItem | `0x0811DF90` | vuotai |
| `AddExtPoint` | ScriptFuns.cpp:14346 -> LuaAddExtPoint | `0x0810FBE0` | phonglangdo + satthu + vuotai |
| `AddGlobalCountNews` | ScriptFuns.cpp:14313 -> LuaAddGlobalCountNews | `0x081258E0` | phonglangdo |
| `AddGlobalNews` | ScriptFuns.cpp:14311 -> LuaAddGlobalNews | `0x08125A90` | phonglangdo + satthu + vuotai |
| `AddMSPlayer` | ScriptFuns.cpp:14756 -> LuaAddMissionPlayer | `0x081366A0` | phonglangdo + vuotai |
| `AddMagic` | ScriptFuns.cpp:14465 -> LuaAddMagic | `0x0812C430` | phonglangdo + satthu + vuotai |
| `AddMagicPoint` | ScriptFuns.cpp:14471 -> LuaAddMagicPoint | `0x0810FE50` | phonglangdo + satthu + vuotai |
| `AddMapTrap` | ScriptFuns.cpp:14405 -> LuaAddMapTrap | `0x08102700` | phonglangdo + satthu + vuotai |
| `AddNpc` | ScriptFuns.cpp:14494 -> LuaAddNpc | `0x0811BB10` | phonglangdo + satthu + vuotai |
| `AddNpcEx` | ScriptFuns.cpp:14495 -> LuaAddNpcEx | `0x0811BF40` | phonglangdo + satthu + vuotai |
| `AddOwnExp` | ScriptFuns.cpp:14617 -> LuaAddOwnExp | `0x081126C0` | phonglangdo + satthu + vuotai |
| `AddProp` | ScriptFuns.cpp:15276 -> LuaAddPropPoint | `0x0810FD60` | phonglangdo + satthu + vuotai |
| `AddRepute` | ScriptFuns.cpp:14328 -> LuaModifyRepute | `0x08117290` | phonglangdo + vuotai |
| `AddSkillState` | ScriptFuns.cpp:14349 -> LuaAddSkillState | `0x08126240` | phonglangdo + satthu + vuotai |
| `AddStrg` | ScriptFuns.cpp:14651 -> LuaSetPlayerStrength | `0x08119F40` | phonglangdo + satthu + vuotai |
| `AddTimer` | ScriptFuns.cpp:15254 -> LuaJX2_AddTimer | `0x08100D40` | phonglangdo + satthu + vuotai |
| `AddVit` | ScriptFuns.cpp:14653 -> LuaSetPlayerVitality | `0x08119EC0` | phonglangdo + satthu + vuotai |
| `AddWeekGoalOffer` | ScriptFuns.cpp:14872 -> LuaJX2_AddWeekGoalOffer | `0x0811A700` | satthu + vuotai |
| `AddWeeklyOffer` | ScriptFuns.cpp:14867 -> LuaJX2_AddWeeklyOffer | `0x08114280` | satthu + vuotai |
| `AppendString` | ScriptFuns.cpp:15218 -> LuaAppendString | `0x0812FCD0` | phonglangdo + satthu + vuotai |
| `AskClientForNumber` | ScriptFuns.cpp:14932 -> LuaJX2_AskClientForNumber | `0x08115CA0` | phonglangdo + satthu + vuotai |
| `AskClientForString` | ScriptFuns.cpp:14901 -> LuaWllsAskClientForString | `0x08115E90` | phonglangdo + satthu + vuotai |
| `BT_GetData` | ScriptFuns.cpp:14306 -> LuaBT_GetDataStub | `0x081C67D0` | phonglangdo + satthu + vuotai |
| `BT_GetGameData` | ScriptFuns.cpp:14305 -> LuaBT_GetDataStub | `0x081C6230` | phonglangdo + satthu + vuotai |
| `BT_GetTopTenInfo` | ScriptFuns.cpp:15182 -> LuaBT_GetTopTenInfo | `0x081C57C0` | phonglangdo + vuotai |
| `BT_SetData` | ScriptFuns.cpp:15174 -> LuaBT_SetData | `0x081C68E0` | phonglangdo + vuotai |
| `BT_SetType2Task` | ScriptFuns.cpp:15172 -> LuaBT_SetType2Task | `0x081C6340` | phonglangdo + vuotai |
| `BT_SetTypeBonus` | ScriptFuns.cpp:15175 -> LuaBT_SetTypeBonus | `0x081C5120` | phonglangdo + vuotai |
| `CalcEquiproomItemCount` | ScriptFuns.cpp:14912 -> LuaWllsCalcEquiproomItemCount | `0x0810D580` | phonglangdo + satthu + vuotai |
| `CalcFreeItemCellCount` | ScriptFuns.cpp:14431 -> LuaCalcFreeItemCellCount | `0x0810CA20` | phonglangdo + satthu + vuotai |
| `CalcItemCount` | ScriptFuns.cpp:14911 -> LuaWllsCalcItemCount | `0x0810D840` | phonglangdo + satthu + vuotai |
| `CallPlayerFunction` | ScriptFuns.cpp:14301 -> LuaCallPlayerFunction | `0x08129580` | phonglangdo + satthu + vuotai |
| `ClearMapNpc` | ScriptFuns.cpp:15191 -> LuaClearMapNpc | `0x08102E90` | phonglangdo + satthu + vuotai |
| `ClearMapObj` | ScriptFuns.cpp:15192 -> LuaClearMapObj | `0x08102B40` | phonglangdo + satthu + vuotai |
| `ClearMapTrap` | ScriptFuns.cpp:15244 -> LuaClearMapTrap | `0x08102AC0` | phonglangdo + satthu + vuotai |
| `CloseMission` | ScriptFuns.cpp:14748 -> LuaCloseMission | `0x081327E0` | phonglangdo |
| `ConsumeEquiproomItem` | ScriptFuns.cpp:14385 -> LuaConsumeEquiproomItem | `0x0810CEF0` | phonglangdo + satthu + vuotai |
| `ConsumeItem` | ScriptFuns.cpp:14384 -> LuaConsumeItem | `0x0810D6C0` | satthu + vuotai |
| `CountFreeRoomByWH` | ScriptFuns.cpp:14914 -> LuaWllsCountFreeRoomByWH | `0x0810C790` | phonglangdo + satthu + vuotai |
| `DelMSPlayer` | ScriptFuns.cpp:14757 -> LuaRemoveMissionPlayer | `0x081372A0` | phonglangdo + vuotai |
| `DelMagic` | ScriptFuns.cpp:14467 -> LuaDelMagic | `0x0811C7E0` | vuotai |
| `DelNpc` | ScriptFuns.cpp:14497 -> LuaDelNpc | `0x08107460` | phonglangdo + vuotai |
| `DelTimer` | ScriptFuns.cpp:15255 -> LuaJX2_DelTimer | `0x08100CA0` | vuotai |
| `Describe` | ScriptFuns.cpp:14260 -> LuaSelectUI | `0x081242A0` | phonglangdo + satthu + vuotai |
| `DisabledUseTownP` | ScriptFuns.cpp:15203 -> LuaDisabledUseTownP | `0x08130A80` | phonglangdo + vuotai |
| `DropItem` | ScriptFuns.cpp:14408 -> LuaDropItem | `0x081200B0` | phonglangdo + satthu + vuotai |
| `DynamicExecute` | ScriptFuns.cpp:14900 -> LuaDynamicExecute | `0x081300B0` | phonglangdo + satthu + vuotai |
| `DynamicExecuteByPlayer` | ScriptFuns.cpp:14308 -> LuaDynamicExecuteByPlayer | `0x0812FE80` | phonglangdo + satthu + vuotai |
| `FileName2Id` | ScriptFuns.cpp:14480 -> LuaFileName2Id | `0x08100E80` | phonglangdo + satthu + vuotai |
| `ForbidChangePK` | ScriptFuns.cpp:15202 -> LuaForbidChangePK | `0x0810F590` | phonglangdo + vuotai |
| `ForbidEnmity` | ScriptFuns.cpp:14922 -> LuaTongForbidEnmity | `0x0810B0F0` | phonglangdo + vuotai |
| `FormatTime2Number` | ScriptFuns.cpp:14877 -> LuaJX2_FormatTime2Number | `0x081023B0` | phonglangdo + satthu + vuotai |
| `FormatTime2String` | ScriptFuns.cpp:14876 -> LuaJX2_FormatTime2String | `0x08106720` | phonglangdo + satthu + vuotai |
| `GetAllEquipment` | ScriptFuns.cpp:15245 -> LuaGetAllEquipment | `0x0810D0F0` | phonglangdo + vuotai |
| `GetBit` | ScriptFuns.cpp:14271 -> LuaGetBit | `0x080FEBA0` | phonglangdo + satthu + vuotai |
| `GetBitTask` | ScriptFuns.cpp:14294 -> LuaGetBitTask | `0x081090A0` | phonglangdo + satthu + vuotai |
| `GetByte` | ScriptFuns.cpp:14272 -> LuaGetByte | `0x080FEA20` | phonglangdo + satthu + vuotai |
| `GetCamp` | ScriptFuns.cpp:14592 -> LuaGetPlayerCamp | `0x08114650` | phonglangdo + vuotai |
| `GetCash` | ScriptFuns.cpp:14654 -> LuaGetPlayerCashMoney | `0x081116D0` | phonglangdo + satthu + vuotai |
| `GetCityOwner` | ScriptFuns.cpp:15123 -> LuaGetCityOwner | `0x080FFF40` | satthu + vuotai |
| `GetCurCamp` | ScriptFuns.cpp:14591 -> LuaGetPlayerCurrentCamp | `0x081146C0` | phonglangdo + vuotai |
| `GetCurServerTime` | ScriptFuns.cpp:14873 -> LuaJX2_GetCurServerTime | `0x08103800` | phonglangdo + satthu + vuotai |
| `GetDex` | ScriptFuns.cpp:14648 -> LuaGetPlayerDexterity | `0x08111960` | phonglangdo + satthu + vuotai |
| `GetEng` | ScriptFuns.cpp:14646 -> LuaGetPlayerEngergy | `0x08111A30` | phonglangdo + satthu + vuotai |
| `GetExtPoint` | ScriptFuns.cpp:14344 -> LuaGetExtPoint | `0x0810FA90` | phonglangdo + satthu + vuotai |
| `GetFightState` | ScriptFuns.cpp:14484 -> LuaGetFightState | `0x081113C0` | phonglangdo + satthu + vuotai |
| `GetGameTime` | ScriptFuns.cpp:14343 -> LuaGetGameTime | `0x0810F3A0` | phonglangdo + vuotai |
| `GetGiveItemUnit` | ScriptFuns.cpp:15215 -> LuaGetGiveItemUnit | `0x08114E60` | phonglangdo + satthu + vuotai |
| `GetItemProp` | ScriptFuns.cpp:15213 -> LuaGetItemProp | `0x080FF260` | phonglangdo + satthu + vuotai |
| `GetItemStackCount` | ScriptFuns.cpp:15263 -> LuaGetItemStackCount | `0x080FD250` | phonglangdo + satthu + vuotai |
| `GetLastDiagNpc` | ScriptFuns.cpp:15200 -> LuaGetLastDiagNpc | `0x0810C5E0` | phonglangdo + satthu + vuotai |
| `GetLastFactionNumber` | ScriptFuns.cpp:14302 -> LuaGetLastFactionNumber | `0x0810E6D0` | phonglangdo + satthu + vuotai |
| `GetLevel` | ScriptFuns.cpp:14638 -> LuaGetLevel | `0x081111E0` | phonglangdo + satthu + vuotai |
| `GetLocalDate` | ScriptFuns.cpp:15282 -> LuaGetLocalDate | `0x0812A140` | phonglangdo + satthu + vuotai |
| `GetMSPlayerCount` | ScriptFuns.cpp:14765 -> LuaMissionPlayerCount | `0x081351F0` | phonglangdo + satthu + vuotai |
| `GetMSRestTime` | ScriptFuns.cpp:14752 -> LuaGetMissionRestTime | `0x081361C0` | vuotai |
| `GetMapInfoFile` | ScriptFuns.cpp:15243 -> LuaGetMapInfoFile | `0x081024E0` | phonglangdo + vuotai |
| `GetMateName` | ScriptFuns.cpp:14446 -> LuaGetMateName | `0x08110340` | vuotai |
| `GetMissionS` | ScriptFuns.cpp:14740 -> LuaGetMissionString | `0x08107160` | satthu + vuotai |
| `GetMissionV` | ScriptFuns.cpp:14739 -> LuaGetMissionValue | `0x081072F0` | phonglangdo + satthu + vuotai |
| `GetNextPlayer` | ScriptFuns.cpp:14761 -> LuaGetNextPlayer | `0x08135760` | phonglangdo + satthu + vuotai |
| `GetNpcName` | ScriptFuns.cpp:14506 -> LuaGetNpcName | `0x08100040` | phonglangdo + satthu + vuotai |
| `GetNpcParam` | ScriptFuns.cpp:14509 -> LuaGetNpcParam | `0x081C0A00` | satthu |
| `GetNpcPos` | ScriptFuns.cpp:14578 -> LuaGetNpcPos | `0x081293F0` | phonglangdo + satthu + vuotai |
| `GetNpcSettingIdx` | ScriptFuns.cpp:15199 -> LuaGetNpcSettingIdx | `0x080FDE50` | phonglangdo + satthu + vuotai |
| `GetPos` | ScriptFuns.cpp:14400 -> LuaGetPos | `0x08129140` | vuotai |
| `GetProp` | ScriptFuns.cpp:15279 -> LuaGetRestPropPoint | `0x0810F9F0` | phonglangdo + satthu + vuotai |
| `GetServerName` | ScriptFuns.cpp:14705 -> LuaGetServerName | `0x08106510` | phonglangdo + satthu + vuotai |
| `GetSex` | ScriptFuns.cpp:14629 -> LuaGetPlayerSex | `0x08112020` | phonglangdo + satthu + vuotai |
| `GetStrg` | ScriptFuns.cpp:14650 -> LuaGetPlayerStrength | `0x08111890` | phonglangdo + satthu + vuotai |
| `GetTeamMember` | ScriptFuns.cpp:14292 -> LuaGetTeamMem | `0x08115530` | phonglangdo + satthu + vuotai |
| `GetTeamSize` | ScriptFuns.cpp:14365 -> LuaGetTeamSize | `0x08115480` | phonglangdo + satthu + vuotai |
| `GetTong` | ScriptFuns.cpp:14933 -> LuaJX2_GetTong | `0x0811AAB0` | phonglangdo + satthu + vuotai |
| `GetTongFigure` | ScriptFuns.cpp:14936 -> LuaJX2_GetTongFigure | `0x081145B0` | satthu + vuotai |
| `GetTongMTask` | ScriptFuns.cpp:14945 -> LuaJX2_GetTongMTask | `0x0812DC60` | satthu + vuotai |
| `GetTongName` | ScriptFuns.cpp:14788 -> LuaGetTongName | `0x0811AB30` | phonglangdo + satthu + vuotai |
| `GetVit` | ScriptFuns.cpp:14652 -> LuaGetPlayerVitality | `0x081117B0` | phonglangdo + satthu + vuotai |
| `GetWeekGoalOffer` | ScriptFuns.cpp:14871 -> LuaJX2_GetWeekGoalOffer | `0x08114390` | satthu + vuotai |
| `GetWeeklyOffer` | ScriptFuns.cpp:14866 -> LuaJX2_GetWeeklyOffer | `0x08114320` | satthu + vuotai |
| `GetWorldPos` | ScriptFuns.cpp:14401 -> LuaGetNewWorldPos | `0x08128F30` | phonglangdo + satthu + vuotai |
| `GiveItemUI` | ScriptFuns.cpp:15214 -> LuaGiveItemUI | `0x0812BBA0` | phonglangdo + satthu + vuotai |
| `HaveMagic` | ScriptFuns.cpp:14468 -> LuaHaveMagic | `0x0811C930` | phonglangdo + satthu + vuotai |
| `IL` | ScriptFuns.cpp:14282 -> LuaIncludeLib | `—` | phonglangdo + vuotai |
| `IncludeLib` | ScriptFuns.cpp:14276 -> LuaIncludeLib | `0x0812E1D0` | phonglangdo + satthu + vuotai |
| `IniFile_GetData` | ScriptFuns.cpp:15289 -> LuaIniFile_GetData | `0x0814ABD0` | phonglangdo + satthu + vuotai |
| `IniFile_Load` | ScriptFuns.cpp:15288 -> LuaIniFile_Load | `0x0814AFB0` | phonglangdo + satthu + vuotai |
| `IsCaptain` | ScriptFuns.cpp:14363 -> LuaIsLeader | `0x08115690` | satthu + vuotai |
| `IsMyItem` | ScriptFuns.cpp:14348 -> LuaIsMyItem | `0x0811B4D0` | phonglangdo + satthu + vuotai |
| `LGM_CreateMemberObj` | ScriptFuns.cpp:15113 -> LuaLGM_CreateMemberObj | `0x0815D0F0` | phonglangdo + satthu + vuotai |
| `LGM_SetMemberInfo` | ScriptFuns.cpp:15114 -> LuaLGM_SetMemberInfo | `0x0815CCA0` | phonglangdo + satthu + vuotai |
| `LG_AddMemberToObj` | ScriptFuns.cpp:15094 -> LuaLG_AddMemberToObj | `0x0815DC80` | phonglangdo + satthu + vuotai |
| `LG_ApplyAddLeague` | ScriptFuns.cpp:15095 -> LuaLG_ApplyAddLeague | `0x0815D210` | phonglangdo + satthu + vuotai |
| `LG_ApplyAppendLeagueTask` | ScriptFuns.cpp:15109 -> LuaLG_ApplyAppendLeagueTask | `0x0815C3B0` | phonglangdo + satthu + vuotai |
| `LG_ApplyDoScript` | ScriptFuns.cpp:15118 -> LuaLG_ApplyDoScript | `0x0815B700` | phonglangdo + satthu + vuotai |
| `LG_ApplySetLeagueTask` | ScriptFuns.cpp:15108 -> LuaLG_ApplySetLeagueTask | `0x0815C3D0` | phonglangdo + satthu + vuotai |
| `LG_CreateLeagueObj` | ScriptFuns.cpp:15091 -> LuaLG_CreateLeagueObj | `0x0815D740` | phonglangdo + satthu + vuotai |
| `LG_FreeLeagueObj` | ScriptFuns.cpp:15092 -> LuaLG_FreeLeagueObj | `0x0815C670` | phonglangdo + satthu + vuotai |
| `LG_GetFirstLeague` | ScriptFuns.cpp:15099 -> LuaLG_GetFirstLeague | `0x0815B640` | phonglangdo + vuotai |
| `LG_GetLeagueInfo` | ScriptFuns.cpp:15101 -> LuaLG_GetLeagueInfo | `0x0815C500` | phonglangdo + vuotai |
| `LG_GetLeagueObj` | ScriptFuns.cpp:15097 -> LuaLG_GetLeagueObj | `0x0815E020` | phonglangdo + satthu + vuotai |
| `LG_GetLeagueObjByRole` | ScriptFuns.cpp:15098 -> LuaLG_GetLeagueObjByRole | `0x0815DE40` | phonglangdo + satthu + vuotai |
| `LG_GetLeagueTask` | ScriptFuns.cpp:15107 -> LuaLG_GetLeagueTask | `0x0815E330` | phonglangdo + satthu + vuotai |
| `LG_GetNextLeague` | ScriptFuns.cpp:15100 -> LuaLG_GetNextLeague | `0x0815D870` | phonglangdo + vuotai |
| `LG_SetLeagueInfo` | ScriptFuns.cpp:15093 -> LuaLG_SetLeagueInfo | `0x0815CB90` | phonglangdo + satthu + vuotai |
| `Ladder_ClearLadder` | ScriptFuns.cpp:15086 -> LuaLadder_ClearLadder | `0x08159A80` | phonglangdo + vuotai |
| `Ladder_GetLadderInfo` | ScriptFuns.cpp:15087 -> LuaLadder_GetLadderInfo | `0x08159BD0` | phonglangdo + satthu + vuotai |
| `Ladder_NewLadder` | ScriptFuns.cpp:15085 -> LuaLadder_NewLadder | `0x08159D40` | phonglangdo + satthu + vuotai |
| `LeaveTeam` | ScriptFuns.cpp:14366 -> LuaLeaveTeam | `0x08121060` | phonglangdo + vuotai |
| `Msg2MSAll` | ScriptFuns.cpp:14773 -> LuaMissionMsg2All | `0x08134280` | phonglangdo + vuotai |
| `Msg2Player` | ScriptFuns.cpp:14368 -> LuaMsgToPlayer | `0x081171B0` | phonglangdo + satthu + vuotai |
| `Msg2SubWorld` | ScriptFuns.cpp:14370 -> LuaMsgToSubWorld | `0x08105170` | vuotai |
| `Msg2Team` | ScriptFuns.cpp:14369 -> LuaMsgToTeam | `0x081152C0` | satthu + vuotai |
| `Msg2Tong` | ScriptFuns.cpp:14375 -> LuaMsgToTong | `0x08120E00` | vuotai |
| `NewWorld` | ScriptFuns.cpp:14402 -> LuaEnterNewWorld | `0x0811B690` | phonglangdo + vuotai |
| `NpcIdx2PIdx` | ScriptFuns.cpp:14764 -> LuaNpcIndexToPlayerIndex | `0x081058B0` | phonglangdo + vuotai |
| `OB_Clear` | ScriptFuns.cpp:15074 -> LuaOB_Clear | `0x080FC360` | phonglangdo + satthu + vuotai |
| `OB_Create` | ScriptFuns.cpp:15071 -> LuaOB_Create | `0x08130230` | phonglangdo + satthu + vuotai |
| `OB_IsEmpty` | ScriptFuns.cpp:15073 -> LuaOB_IsEmpty | `0x080FC280` | phonglangdo + satthu + vuotai |
| `OB_LoadShareData` | ScriptFuns.cpp:15260 -> LuaOB_LoadShareData | `—` | phonglangdo + satthu + vuotai |
| `OB_PopByte` | ScriptFuns.cpp:15078 -> LuaOB_PopByte | `0x08128E30` | phonglangdo + satthu + vuotai |
| `OB_PopDouble` | ScriptFuns.cpp:15082 -> LuaOB_PopDouble | `0x08100220` | phonglangdo + satthu + vuotai |
| `OB_PopInt` | ScriptFuns.cpp:15080 -> LuaOB_PopInt | `0x08128D30` | phonglangdo + satthu + vuotai |
| `OB_PopString` | ScriptFuns.cpp:15084 -> LuaOB_PopString | `0x08100320` | phonglangdo + satthu + vuotai |
| `OB_PushByte` | ScriptFuns.cpp:15077 -> LuaOB_PushByte | `0x0812F2D0` | phonglangdo + satthu + vuotai |
| `OB_PushDouble` | ScriptFuns.cpp:15081 -> LuaOB_PushDouble | `0x0812F150` | phonglangdo + satthu + vuotai |
| `OB_PushInt` | ScriptFuns.cpp:15079 -> LuaOB_PushInt | `0x0812EFC0` | phonglangdo + satthu + vuotai |
| `OB_PushString` | ScriptFuns.cpp:15083 -> LuaOB_PushString | `0x08130B90` | phonglangdo + satthu + vuotai |
| `OB_Release` | ScriptFuns.cpp:15072 -> LuaOB_Release | `0x08130440` | phonglangdo + satthu + vuotai |
| `OB_SaveShareData` | ScriptFuns.cpp:15259 -> LuaOB_SaveShareData | `—` | phonglangdo + satthu + vuotai |
| `OpenMission` | ScriptFuns.cpp:14746 -> LuaInitMission | `0x081332F0` | phonglangdo |
| `PARTNER_GetCurPartner` | ScriptFuns.cpp:15222 -> LuaPARTNER_GetCurPartner | `0x0816BFD0` | phonglangdo + satthu + vuotai |
| `PARTNER_GetSettingIdx` | ScriptFuns.cpp:15223 -> LuaPARTNER_GetSettingIdx | `0x0816C870` | phonglangdo + satthu + vuotai |
| `Pay` | ScriptFuns.cpp:14656 -> LuaPlayerPayMoney | `0x08118A90` | phonglangdo + satthu + vuotai |
| `PayExtPoint` | ScriptFuns.cpp:14347 -> LuaPayExtPoint | `0x0810FCA0` | phonglangdo + satthu + vuotai |
| `PopString` | ScriptFuns.cpp:15220 -> LuaPopString | `0x080FFB00` | phonglangdo + satthu + vuotai |
| `PushString` | ScriptFuns.cpp:15217 -> LuaPushString | `0x0812FDA0` | phonglangdo + satthu + vuotai |
| `PutMessage` | ScriptFuns.cpp:14310 -> LuaSendMessageInfo | `0x08116F80` | phonglangdo |
| `RemoteExecute` | ScriptFuns.cpp:15258 -> LuaJX2_RemoteExecute | `0x08100740` | phonglangdo + satthu + vuotai |
| `RemoveItemByIndex` | ScriptFuns.cpp:15216 -> LuaRemoveItemByIndex | `0x08114F80` | phonglangdo + satthu + vuotai |
| `ReplaceString` | ScriptFuns.cpp:15219 -> LuaReplaceString | `0x0812EB20` | phonglangdo + satthu + vuotai |
| `RollbackSkill` | ScriptFuns.cpp:15273 -> LuaRollBackSkills | `0x0811C640` | phonglangdo + satthu + vuotai |
| `ST_GetTransLifeCount` | ScriptFuns.cpp:14336 -> LuaGetPlayerReBornValue | `0x081C1100` | phonglangdo + satthu + vuotai |
| `SearchPlayer` | ScriptFuns.cpp:15206 -> LuaSearchPlayer | `0x081020A0` | phonglangdo + vuotai |
| `SetBit` | ScriptFuns.cpp:14273 -> LuaSetBit | `0x080FEAC0` | phonglangdo + satthu + vuotai |
| `SetBitTask` | ScriptFuns.cpp:14295 -> LuaSetBitTask | `0x08108F10` | phonglangdo + satthu + vuotai |
| `SetByte` | ScriptFuns.cpp:14274 -> LuaSetByte | `0x080FE950` | phonglangdo + satthu + vuotai |
| `SetCreateTeam` | ScriptFuns.cpp:14780 -> LuaSetCreateTeamOption | `0x08120FC0` | phonglangdo + vuotai |
| `SetCurCamp` | ScriptFuns.cpp:14593 -> LuaSetPlayerCurrentCamp | `0x0811B1D0` | phonglangdo + vuotai |
| `SetDeathScript` | ScriptFuns.cpp:14776 -> LuaSetPlayerDeathScript | `0x08110700` | phonglangdo + vuotai |
| `SetFightState` | ScriptFuns.cpp:14483 -> LuaSetFightState | `0x08117A10` | phonglangdo + vuotai |
| `SetItemStackCount` | ScriptFuns.cpp:15264 -> LuaSetItemStackCount | `0x0810D9A0` | phonglangdo + satthu + vuotai |
| `SetLogoutRV` | ScriptFuns.cpp:14779 -> LuaSetPlayerRevivalOptionWhenLogout | `0x08110500` | phonglangdo + vuotai |
| `SetMissionS` | ScriptFuns.cpp:15232 -> LuaJx2SetMissionString | `0x08107220` | satthu + vuotai |
| `SetMissionV` | ScriptFuns.cpp:15231 -> LuaSetMission | `0x08107390` | phonglangdo + satthu + vuotai |
| `SetNpcCurCamp` | ScriptFuns.cpp:14663 -> LuaSetNpcCurCamp | `0x0811B120` | phonglangdo + satthu + vuotai |
| `SetNpcDeathScript` | ScriptFuns.cpp:15190 -> LuaSetNpcDeathScript | `0x08101500` | phonglangdo + satthu + vuotai |
| `SetNpcParam` | ScriptFuns.cpp:14507 -> LuaSetNpcParam | `0x081C0260` | phonglangdo + satthu + vuotai |
| `SetNpcScript` | ScriptFuns.cpp:14500 -> LuaSetNpcActionScript | `0x08101500` | phonglangdo + satthu + vuotai |
| `SetNpcTimer` | ScriptFuns.cpp:14555 -> LuaSetNpcTimer | `0x080FC180` | phonglangdo + satthu + vuotai |
| `SetPKFlag` | ScriptFuns.cpp:15201 -> LuaSetPKFlag | `0x0810F610` | phonglangdo + vuotai |
| `SetPunish` | ScriptFuns.cpp:14793 -> LuaSetDeathPunish | `0x0810F470` | phonglangdo + vuotai |
| `SetRevPos` | ScriptFuns.cpp:14589 -> LuaSetPlayerRevivalPos | `0x0811B370` | phonglangdo + vuotai |
| `SetSpecItemParam` | ScriptFuns.cpp:14440 -> LuaSetSpecItemParam | `0x080FF360` | phonglangdo + satthu + vuotai |
| `SetTaskTemp` | ScriptFuns.cpp:14269 -> LuaSetTempTaskValue | `0x08123950` | phonglangdo + vuotai |
| `SetTmpCamp` | ScriptFuns.cpp:15252 -> LuaSetTmpCamp | `0x0810BA50` | vuotai |
| `SetTongMTask` | ScriptFuns.cpp:14946 -> LuaJX2_SetTongMTask | `0x08128A20` | satthu + vuotai |
| `SetWeeklyOffer` | ScriptFuns.cpp:14868 -> LuaJX2_SetWeeklyOffer | `0x081141E0` | satthu + vuotai |
| `StartMissionTimer` | ScriptFuns.cpp:14750 -> LuaStartMissionTimer | `0x08138840` | phonglangdo + satthu + vuotai |
| `StopMissionTimer` | ScriptFuns.cpp:14751 -> LuaStopMissionTimer | `0x08134720` | phonglangdo + satthu + vuotai |
| `SubWorldID2Idx` | ScriptFuns.cpp:14477 -> LuaSubWorldIDToIndex | `0x08102580` | phonglangdo + satthu + vuotai |
| `SubWorldIdx2ID` | ScriptFuns.cpp:14478 -> LuaSubWorldIndexToID | `0x081077D0` | phonglangdo + satthu + vuotai |
| `SubWorldIdx2MapCopy` | ScriptFuns.cpp:14479 -> LuaSubWorldIdx2MapCopy | `0x08107740` | vuotai |
| `SyncItem` | ScriptFuns.cpp:14298 -> LuaSyncItemJX2 | `0x08114EF0` | phonglangdo + satthu + vuotai |
| `SyncTaskValue` | ScriptFuns.cpp:14883 -> LuaJX2_SyncTaskValue | `0x0810E350` | phonglangdo + satthu + vuotai |
| `TM_GetRestCount` | ScriptFuns.cpp:14304 -> LuaTM_GetRestCount | `0x081C21A0` | phonglangdo + vuotai |
| `TM_SetTimer` | ScriptFuns.cpp:14303 -> LuaTM_SetTimer | `0x081C23C0` | phonglangdo + vuotai |
| `TONGM_GetFigure` | ScriptFuns.cpp:14848 -> LuaTONGM_GetFigure | `0x08199EC0` | satthu + vuotai |
| `TONG_ApplyAddBuildFund` | ScriptFuns.cpp:15000 -> LuaTONG_ApplyAddBuildFund | `0x08194650` | satthu + vuotai |
| `TONG_ApplyAddEventRecord` | ScriptFuns.cpp:15011 -> LuaTONG_ApplyAddEventRecord | `0x0818BFE0` | satthu + vuotai |
| `TONG_ApplyAddStoredBuildFund` | ScriptFuns.cpp:15005 -> LuaTONG_ApplyAddStoredBuildFund | `0x08195A90` | satthu + vuotai |
| `TONG_ApplyAddStoredOffer` | ScriptFuns.cpp:15006 -> LuaTONG_ApplyAddStoredOffer | `0x08195C10` | satthu + vuotai |
| `TONG_ApplyAddTaskValue` | ScriptFuns.cpp:14842 -> LuaTONG_ApplyAddTaskValue | `0x08196450` | phonglangdo + satthu + vuotai |
| `TONG_ApplyAddTotalBuildFund` | ScriptFuns.cpp:15007 -> LuaTONG_ApplyAddTotalBuildFund | `0x08194B90` | satthu + vuotai |
| `TONG_ApplyAddWarBuildFund` | ScriptFuns.cpp:15001 -> LuaTONG_ApplyAddWarBuildFund | `0x08194410` | satthu + vuotai |
| `TONG_ApplyAddWeekBuildFund` | ScriptFuns.cpp:15008 -> LuaTONG_ApplyAddWeekBuildFund | `0x08194D10` | satthu + vuotai |
| `TONG_ApplyAddWeekGoalValue` | ScriptFuns.cpp:15009 -> LuaTONG_ApplyAddWeekGoalValue | `0x08195550` | satthu + vuotai |
| `TONG_ApplySetTaskValue` | ScriptFuns.cpp:14841 -> LuaTONG_ApplySetTaskValue | `0x0818AE40` | phonglangdo + satthu + vuotai |
| `TONG_GetBuildLevel` | ScriptFuns.cpp:14823 -> LuaTONG_GetBuildLevel | `0x0818D600` | satthu + vuotai |
| `TONG_GetDay` | ScriptFuns.cpp:14829 -> LuaTONG_GetDay | `0x0818DEC0` | satthu + vuotai |
| `TONG_GetMemberCount` | ScriptFuns.cpp:14809 -> LuaTONG_GetMemberCount | `0x0818FF40` | satthu + vuotai |
| `TONG_GetName` | ScriptFuns.cpp:14801 -> LuaTONG_GetName | `0x0818AC80` | satthu + vuotai |
| `TONG_GetTaskTemp` | ScriptFuns.cpp:14816 -> LuaTONG_GetTaskTemp | `0x08190260` | satthu + vuotai |
| `TONG_GetTaskValue` | ScriptFuns.cpp:14814 -> LuaTONG_GetTaskValue | `0x081900A0` | phonglangdo + satthu + vuotai |
| `TONG_GetWeek` | ScriptFuns.cpp:14830 -> LuaTONG_GetWeek | `0x0818E000` | satthu + vuotai |
| `TONG_GetWeekBuildFund` | ScriptFuns.cpp:14831 -> LuaTONG_GetWeekBuildFund | `0x0818F400` | satthu + vuotai |
| `TONG_GetWeekGoalEvent` | ScriptFuns.cpp:14953 -> LuaTONG_GetWeekGoalEvent | `0x0818E140` | satthu + vuotai |
| `TONG_GetWeekGoalPlayer` | ScriptFuns.cpp:14956 -> LuaTONG_GetWeekGoalPlayer | `0x0818E500` | satthu + vuotai |
| `TONG_SetTaskTemp` | ScriptFuns.cpp:14817 -> LuaTONG_SetTaskTemp | `0x0818B350` | satthu + vuotai |
| `TONG_WriteLog` | ScriptFuns.cpp:15014 -> LuaTONG_WriteLog | `0x0818B5A0` | satthu + vuotai |
| `TabFile_GetCell` | ScriptFuns.cpp:15285 -> LuaTabFile_GetCell | `0x0814A740` | phonglangdo + satthu + vuotai |
| `TabFile_GetRowCount` | ScriptFuns.cpp:15286 -> LuaTabFile_GetRowCount | `0x0814A690` | phonglangdo + satthu + vuotai |
| `TabFile_Load` | ScriptFuns.cpp:15284 -> LuaTabFile_Load | `0x0814AEF0` | phonglangdo + satthu + vuotai |
| `TabFile_UnLoad` | ScriptFuns.cpp:15287 -> LuaTabFile_UnLoad | `0x0814B040` | satthu |
| `Time2Tm` | ScriptFuns.cpp:14891 -> LuaTime2Tm | `0x081021B0` | phonglangdo + satthu + vuotai |
| `Title_ActiveTitle` | ScriptFuns.cpp:15165 -> LuaTitle_ActiveTitle | `0x081596F0` | phonglangdo + vuotai |
| `Title_AddTitle` | ScriptFuns.cpp:15164 -> LuaTitle_AddTitle | `0x081597A0` | phonglangdo + vuotai |
| `Title_RemoveTitle` | ScriptFuns.cpp:15166 -> LuaTitle_RemoveTitle | `0x081595A0` | phonglangdo + vuotai |
| `WriteLog` | ScriptFuns.cpp:14880 -> LuaJX2_WriteLog | `0x081237D0` | phonglangdo + satthu + vuotai |
| `curpack` | ScriptFuns.cpp:14299 -> LuaCurPack | `0x0823D220` | phonglangdo + satthu + vuotai |
| `safeshow` | ScriptFuns.cpp:14910 -> LuaWllsSafeshow | `—` | phonglangdo + vuotai |
| `usepack` | ScriptFuns.cpp:14300 -> LuaUsePack | `0x0823D240` | phonglangdo + satthu + vuotai |

## 7. Phụ lục — công cụ và tệp kết quả

Tất cả nằm trong `D:\GAMEDEVNEW\ReverseTools\port_3hd\`:

| Tệp | Vai trò |
|---|---|
| `api_gap_3hd.py` | Quét bao đóng → tách tên hàm engine → đối chiếu bảng đăng ký JX1. Sinh `api_gap_raw.json`. |
| `bdump_3hd.py` | Dịch ngược toàn bộ hàm nhóm B từ ELF. Sinh `bdump.txt` (mã máy + chuỗi tham chiếu) và `callstat.txt` (tần suất đích `call`, dùng để nhận diện API Lua). |
| `bsig_3hd.py` | Suy chữ ký từ mã máy (đếm `lua_tonumber`/`lua_tostring` theo chỉ số, số giá trị trả về). Sinh `bsig.json`. |
| `bsites_3hd.py` | In chỗ gọi Lua kèm ngữ cảnh. Sinh `sites.txt`. |
| `shadow_3hd.py` | Tìm tên bị che khuất bởi hàm script định nghĩa ngoài bao đóng. Sinh `shadow.json`. |
| `finalize_3hd.py` | Gộp tất cả + chú thích tay → `04_api_gap.json`. |
| `mkmd_3hd.py` | Sinh chính tệp `04_api_gap.md` này. |

### Chạy lại toàn bộ

```
set PYTHONIOENCODING=utf-8
cd D:\GAMEDEVNEW\ReverseTools\port_3hd
python api_gap_3hd.py  &&  python bdump_3hd.py  &&  python bsig_3hd.py
python bsites_3hd.py > sites.txt  &&  python shadow_3hd.py
python finalize_3hd.py  &&  python mkmd_3hd.py
```

### Bốn bộ lọc đã áp dụng khi tách "hàm engine"

1. Bỏ 20 từ khoá Lua 4.0 và ~96 tên thư viện chuẩn Lua 4.0. Lưu ý Lua 4.0 **chưa có** bảng `string` / `table` / `math` — mọi thứ đều là hàm toàn cục: `strfind`, `getn`, `tinsert`, `floor`, `format`…
2. Bỏ tên bị che bởi **biến cục bộ / tham số / biến vòng lặp** trong chính tệp đó (`local f = ...; f()`, `function g(cb) cb() end`, `for k, value in ...`). Không lọc bước này sẽ lọt `id`, `name`, `func`, `pos`, `series`, `proceed`, `value`, `org`, `fnCallback`.
3. Bỏ tên do cây script Linux định nghĩa bằng `function <tên>(` hoặc `<tên> = function(`. **Chỉ tính hàm toàn cục trần**: `function Player:OpenProgressBar(...)` là **phương thức** của bảng `Player`, không định nghĩa global `OpenProgressBar` — nếu tính nhầm sẽ bỏ sót 3 hàm engine thật (`OpenProgressBar`, `SendScriptData`, `GetRoomItems`).
4. Xoá nội dung chuỗi và chú thích trước khi quét, nhưng **giữ nguyên số dòng** để mọi chỗ gọi báo cáo được `tệp:dòng` chính xác.



---

## PHỤ LỤC PHẢN BIỆN (tác tử độc lập)

| Mục | Nội dung |
|---|---|
| Ngày phản biện | 24/08/2026 |
| Vai trò | Kiểm chứng ngược, mặc định coi mọi khẳng định là SAI cho tới khi tệp gốc / nhị phân chứng minh ngược lại |
| Công cụ tự dựng | `_indep.json` (quét lại độc lập bao đóng: 856 tên được gọi, 7.951 tên toàn cục do cây Linux định nghĩa, 1.021 tên JX1 đăng ký, 1.560 tên luamap) |
| Kết quả | **34 khẳng định được kiểm** — **28 ĐÚNG**, **5 SAI (đã sửa trong thân bài)**, **1 giải quyết được điểm "CHƯA XÁC MINH"** ; **6 chỗ bỏ sót** |

### A. Bảng kiểm chứng

| # | Khẳng định | Bằng chứng gốc | KẾT LUẬN | Sửa lại thành |
|---|---|---|---|---|
| 1 | `GameScriptFuns[]` ở `ScriptFuns.cpp:14257`, `WorldScriptFuns[]` ở `:15325` | Đọc thẳng tệp: dòng 14257 = `TLua_Funcs GameScriptFuns[] =`, dòng 15325 = `TLua_Funcs WorldScriptFuns[] =` | **ĐÚNG** | — |
| 2 | `SysFuns[]` ở `LuaFuns.cpp:43` | dòng 43 = `CORE_API TLua_Funcs SysFuns[] = ` | **ĐÚNG** | — |
| 3 | 6 dòng đăng ký stub: `curpack:14299`, `usepack:14300`, `TM_SetTimer:14303`, `TM_GetRestCount:14304`, `BT_GetGameData:14305`, `BT_GetData:14306` | `ScriptFuns.cpp` dòng 14299–14306 khớp **từng dòng một** | **ĐÚNG (6/6)** | — |
| 4 | Thân stub: `LuaTM_SetTimer` đẩy 0, `LuaTM_GetRestCount` đẩy nil (ghi chú `2247-2249`), `LuaBT_GetDataStub` đẩy 0 (`:2265`) | `ScriptFuns.cpp:2247-2249` = đúng khối chú thích; `2250-2253` `LuaTM_SetTimer` `Lua_PushNumber(L,0)`; `2255-2259` `LuaTM_GetRestCount` `Lua_PushNil(L)`; `2263-2267` `LuaBT_GetDataStub`, câu đẩy 0 nằm ở dòng **2265** | **ĐÚNG** | — (nếu muốn chỉ *đầu hàm* thì là `:2263`, dòng 2265 là câu `Lua_PushNumber(L, 0);`) |
| 5 | `SetNpcScript` ở `:14500` → `LuaSetNpcActionScript`; `SetNpcDeathScript` ở `:15190` → `LuaSetNpcDeathScript` | đọc đúng hai dòng đó | **ĐÚNG** | — |
| 6 | luamap dòng 152–153 gán **cả hai** tên `SetNpcScript`/`SetNpcDeathScript` vào `0x08101500`; **CHƯA XÁC MINH** luamap có sai không | `regtab`: chuỗi `"SetNpcScript"`@`0x0825AE9E` được trỏ từ `0x082E27F8`, chuỗi `"SetNpcDeathScript"`@`0x0825B7F7` được trỏ từ `0x082E2E70`; **dword kế của cả hai = `0x08101500`** | **ĐÚNG về dữ kiện — nhưng "CHƯA XÁC MINH" là THỪA** | luamap **đúng**: JX2 đăng ký **một hàm C chung** cho hai tên. Đã sửa mục 3 và 5.2 |
| 7 | **235 dòng nhóm A** đều ghi đúng `tệp:dòng -> tên hàm C` | Script kiểm tự động đọc từng dòng được trích dẫn: **235/235 khớp cả tên Lua lẫn tên hàm C** | **ĐÚNG (235/235)** | — |
| 8 | **42 địa chỉ ELF** của nhóm B | So từng tên với `jx_linux_y.luamap.full.txt`: **42/42 trùng khít**, không tên nào thiếu địa chỉ | **ĐÚNG (42/42)** | — |
| 9 | 42 hàm nhóm B "JX1 CHƯA CÓ" | `grep -r '"<tên>"' D:\GAMEDEVNEW\Sources --include=*.cpp --include=*.h` ⇒ **0 kết quả cho cả 42 tên**; và không tệp `.lua` nào trong `serverscript_jx2` / cây máy chủ đang chạy định nghĩa chúng | **ĐÚNG (42/42)** | — |
| 10 | `KPlayer::SetBaseDexterity` (`KPlayer.cpp:13422`) làm `+=` | dòng 13422 = chữ ký hàm, dòng **13424** = `m_nDexterity += nData;` (và `m_nCurDexterity += nData;` ở 13426) | **ĐÚNG** | nếu muốn trỏ đúng câu lệnh thì là `KPlayer.cpp:13424` |
| 11 | `AddDex`→`:14649`, `AddEng`→`:14647`, `AddProp`→`:15276`, `GetProp`→`:15279` | 4 dòng đọc đúng như báo cáo | **ĐÚNG (4/4)** | — |
| 12 | `ST_GetTransLifeCount` → `LuaGetPlayerReBornValue` (`ScriptFuns.cpp:5528`) dùng `m_cReBorn.GetReBornValue()` | dòng 5528 = `int LuaGetPlayerReBornValue(Lua_State* L)`, thân hàm dòng 5538 gọi đúng `m_cReBorn.GetReBornValue()` | **ĐÚNG** | — |
| 13 | `ContriValueEntryLogic` chỉ là bí danh gán ở `tong/contribution_entry.lua:190`, thân ở `:55` | dòng 190 = `ContriValueEntryLogic = ContriValueEntryLogic_HEAD;`, dòng 55 = `function ContriValueEntryLogic_HEAD(nValue, nEntry)`. Quét **toàn bộ** cây Linux: chỉ **1** định nghĩa/bí danh duy nhất | **ĐÚNG** | — |
| 14 | `JudgePLAddTitle` là **lỗi của script JX2 gốc** — gọi ở `battlehead.lua:781/783`, chỉ có `bt_JudgePLAddTitle` ở `:844` | ba dòng đọc đúng nguyên văn; quét toàn cây (`script` + `vng_script`): **0 định nghĩa** `JudgePLAddTitle` | **ĐÚNG** | — |
| 15 | `Require` chỉ chạy nhánh client — `protocol.lua:109` trong `MODEL_GAMECLIENT == 1`, nhánh máy chủ ở `:106` | `:105` = `if MODEL_GAMESERVER == 1 then`, `:106` = `DynamicExecuteByPlayer(...)`, `:107` = `elseif MODEL_GAMECLIENT == 1 then`, `:109` = `Require(szFile);`. Toàn cây: **0 định nghĩa** `Require` | **ĐÚNG** | — |
| 16 | `JoinMission`: hàm script ở `mission_match.lua:93` = `function JoinMission(RoleIndex, camp)`, chỗ gọi `dragonboat_main.lua:163` = `JoinMission(MISSION_MATCH, 1)` | đọc đúng nguyên văn cả hai dòng | **ĐÚNG** | — nhưng **thiếu** hệ quả cho `satthu`, xem mục B-2 |
| 17 | `ITEM_SetExpiredTime` bị gọi bằng **hai đơn vị** — YYYYMMDD ở `.../qianqiu_yinglie/head.lua:201`, **số phút** ở `missions/fengling_ferry/bossdeath.lua:35` | `head.lua:200` `nExpiredDate = FormatTime2Date(7*24*60*60 + GetCurServerTime())` → `:201` truyền `nExpiredDate`; `bossdeath.lua:34` `nRestMin = 24*60 - (...)` → `:35` truyền `nRestMin`. Cả hai tệp **đều** nằm trong bao đóng `phonglangdo` | **ĐÚNG** | — |
| 18 | Đơn vị tham số 2 của `ITEM_SetExpiredTime` = **CHƯA XÁC MINH** | Dịch ngược `0x08154A30` → gọi hàm quy đổi `0x081F2760`; thân `0x081F2760`: `test esi,esi / je →0`, `cmp esi, 0x1312D00 / jbe` → nhánh **`imul ebx, esi, 0x3c`** (= `now + n*60`, đơn vị **phút**); nhánh còn lại gọi `0x0820BA80(n, thamso3)` (= **YYYYMMDD + HHMMSS**) | **KHẲNG ĐỊNH "CHƯA XÁC MINH" LÀ SAI — xác minh được** | **Cả hai đúng**: ngưỡng `0x1312D00` = **20 000 000**. `n ≤ 20 000 000` ⇒ số **phút**; `n > 20 000 000` ⇒ **YYYYMMDD**, tham số 3 = **HHMMSS** (mặc định 0). Đã sửa mục 5.2 |
| 19 | `IsDisabledUseHeart` **bỏ qua tham số Lua**, chỉ đọc biến toàn cục `PlayerIndex` | `0x0812ED70`: **không** gọi `lua_gettop`, **không** gọi `lua_tonumber`; gọi thẳng `0x08107860` (GetGlobalPlayerIndex), chặn 1..1199, `imul 0x8788`, `+[0x08BAEE60]`, gọi `0x080A8C80`, `test eax, 0x200000` rồi đẩy 0/1 | **ĐÚNG** | — |
| 20 | Bản ghi vật phẩm: gốc `[0x0830D300]`, bước `0x368`, số lượng `[0x0830CA5C]`, trường `[+4]` = phẩm chất | `GetItemQuality` `0x080FEFB0`: `cmp eax,[0x830ca5c]` → `imul eax,eax,0x368` → `+[0x830d300]` → `fild dword [eax+edx+4]` → đẩy về | **ĐÚNG** | — |
| 21 | Bản ghi người chơi: gốc `[0x08BAEE60]`, bước `0x8788`, chỉ số hợp lệ 1…1199 | thấy nguyên mẫu `lea eax,[edi-1]; cmp eax,0x4AE; ja` + `imul edi,edi,0x8788; a1 60eeba08` ở `0x081F2823` và ở `IsDisabledUseHeart` | **ĐÚNG** | — |
| 22 | `GetPlatinaLevel` trả **nil** nếu vật phẩm không phải bạch kim | `0x08100577`: `cmp dword [eax+4], 4 / je` — nhánh không-bằng gọi `0x08232E70` (`lua_pushnil`) rồi `ret 1`; nhánh bằng đọc `byte [eax+0x344]` | **ĐÚNG** | — |
| 23 | Mã phẩm chất `0 = thường, 1 = hoàng kim, 4 = bạch kim` | `GetPlatinaEquipIndex` `0x080FEF70` = `helper(L, 4)`, `GetGlodEqIndex` `0x080FEF90` = `helper(L, 1)` — **cùng gọi `0x080FEEB0`**, chỉ khác hằng số | **ĐÚNG** | bổ sung: hai hàm này là **một** hàm với hằng phẩm chất ⇒ JX1 chỉ cần viết **một** helper (xem B-6) |
| 24 | `TrimString()` không tham số, không trả giá trị, cắt trên **bộ đệm chuỗi dùng chung** `0x09780D54`/`0x09780D58` | `0x080FF630`: mở đầu `mov eax,[0x9780d58]`, `mov ebx,[0x9780d54]`, so `cmp dl,9` (TAB) và `cmp dl,0x20` (SPACE) ở cả hai đầu; không có `lua_gettop`/`lua_tonumber` | **ĐÚNG** | — |
| 25 | Cách dùng `PushString(str); TrimString(); return PopString()` ở `lib/string.lua:162-164` | đọc đúng ba dòng 162/163/164 | **ĐÚNG** | — |
| 26 | Cây Lua tham chiếu = **5.230 tệp** `.lua` | đếm lại: `script` = 5.059 + `vng_script` = 171 = **5.230** | **ĐÚNG (khớp tuyệt đối)** | — |
| 27 | luamap có **1.560 tên** | tệp 1.561 dòng, dòng 1 là tiêu đề ⇒ 1.560 | **ĐÚNG** | — |
| 28 | Bộ lọc 3 — `GetRoomItems`/`OpenProgressBar`/`SendScriptData` bị `function Player:<tên>` che nên suýt bị bỏ sót | quét toàn cây: cả ba **chỉ** xuất hiện dưới dạng **phương thức** (`tbActivityCompose:GetRoomItems` `lib/composeex.lua:180`, `Player:OpenProgressBar` `lib/player.lua:626`, `MockPlayer:OpenProgressBar` `testcase/lib/mockplayer.lua:444`, `Player:SendScriptData` `lib/player.lua:417`), **không có định nghĩa toàn cục trần nào** | **ĐÚNG** | — |
| 29 | Mục 5.1 — trong số tên bị che mà tệp định nghĩa nằm ngoài bao đóng, **chỉ `JoinMission`** vừa có trong luamap vừa chưa có ở JX1 | Quét lại độc lập: 17 tên có tệp định nghĩa ngoài bao đóng, 14 tên có trong luamap; **chỉ `JoinMission`** có `jx1_dangky = False` | **ĐÚNG** | — |
| 30 | Cột **"Lần gọi"** trong bảng nhóm B | Đếm lại: `DropItemEx` chỉ có **1** chỗ gọi duy nhất trong toàn bao đóng (`lib/droptemplet.lua:74`) nhưng bảng ghi **3**. Tính theo **tổng (tính năng × chỗ gọi)** thì **42/42 dòng khớp tuyệt đối**, và `BT_GetGameData` cũng ra đúng **51** | **SAI (nhãn cột)** | Đổi tiêu đề thành **"Tính năng × chỗ gọi"** + ghi chú. **[đã sửa]** Số chỗ gọi **thật** nhỏ hơn 2–3 lần (`ST_IsTransLife` 5 chỗ chứ không phải 13; `ITEM_SetExpiredTime` 5 chứ không phải 9; `BT_GetGameData` 25 chứ không phải 51) |
| 31 | Bảng mục 1: Phong Lăng Độ có **81** tệp trong bao đóng | `closure3.json` có **82** mục cho `phonglangdo`; mục thứ 82 là `global/路人_礼官.lua` với `linux=false` (**tệp không tồn tại**) và đã bị lặng lẽ trừ đi | **SAI (con số) + BỎ SÓT (không nêu tệp thiếu)** | **82**, trong đó **1 tệp KHÔNG TỒN TẠI** — xem B-1. **[đã sửa]** |
| 32 | Mục 0: "(C) cảnh báo nghĩa khác : **8** (7 nằm trong 235 của nhóm A)" | Bảng mục 3 có 8 **dòng** nhưng dòng `SetNpcScript / SetNpcDeathScript` gộp **hai** tên ⇒ **9 tên**; trong đó 8 tên (`BT_GetData`, `BT_GetGameData`, `SetNpcScript`, `SetNpcDeathScript`, `TM_GetRestCount`, `TM_SetTimer`, `curpack`, `usepack`) đều nằm trong nhóm A | **SAI (lệch 1)** | **9 tên / 8 dòng, 8 nằm trong nhóm A**. **[đã sửa]** |
| 33 | Bản tóm tắt: `04_api_gap.json` có **282 mục** | Đếm theo nhóm: A=235, B=42, D=3, C=2 ⇒ 282 — nhưng một trong hai khoá C là khoá **gộp giả** `"SetNpcScript / SetNpcDeathScript"` (không phải một tên Lua) | **SAI (đếm)** | Số **TÊN LUA** thật = **281**; 282 là do một khoá gộp giả |
| 34 | `GetRoomItems` — "Số liệu dịch ngược: … đẩy về = {'lua_pushnumber': 1}" mà chữ ký lại ghi "trả 1 **bảng**" | Dịch ngược `0x0810D170`: có `call 0x08232BE0` (**`lua_newtable`**) ở `0x0810D1B4`, rồi `0x08232D40` + `0x08233360`; **và** mở đầu bằng `call 0x08107860` (`GetGlobalPlayerIndex`) | **SAI (số liệu tự mâu thuẫn với chữ ký)** | Chữ ký **đúng** (trả bảng); dòng "Số liệu dịch ngược" **thiếu `lua_newtable`**; mô tả **thiếu** việc hàm chạy trên **nhân vật hiện tại**. **[đã sửa]** |

### B. Bỏ sót đã tìm thêm

**B-1. Bao đóng Phong Lăng Độ thiếu HẲN một tệp — `global/路人_礼官.lua` KHÔNG TỒN TẠI ở bất kỳ cây nào.**
`missions/fengling_ferry/boss.lua:11` viết `Include("\script\global\路人_礼官.lua")` (byte GBK `C2 B7 C8 CB 5F C0 F1 B9 D9`).
Quét toàn bộ `D:\ServerLinux\server1`, `D:\GAMEDEVNEW\serverscript_jx2` và `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script`: **không có tệp nào tên như vậy**.
Hai chỗ khác cũng nhắc tới nó và đều xác nhận đó là NPC "Lễ Quan": `global/pgaming/reloadscript.lua:23` (`{"\script\global\路人_礼官.lua", "L. Quan"}`) và `global/gm/lib_data_table_filelua.lua:103` (đã bị chú thích).
`closure3.json` ghi đúng `linux=false` nhưng báo cáo **lặng lẽ trừ khỏi số đếm** thay vì cảnh báo. Khi port Phong Lăng Độ, `boss.lua` sẽ `Include` một tệp không có ⇒ phải viết mới NPC Lễ Quan hoặc bỏ dòng 11.

**B-2. `JoinMission` là khe hở nhóm B THẬT của "săn boss sát thủ" (hàm thứ 43).**
Bao đóng `satthu` **có** `missions/challengeoftime/npc/dragonboat_main.lua` nhưng **không có** `missions/challengeoftime/mission_match.lua` (kiểm bằng `closure3.json`).
Vậy trong bản port `satthu`, lời gọi `JoinMission(MISSION_MATCH, 1)` (`dragonboat_main.lua:163`) **không bị hàm script nào che**, buộc phải là hàm **engine** `0x08137E40` mà JX1 chưa đăng ký ⇒ `attempt to call global 'JoinMission'`.
Báo cáo có nêu `JoinMission` ở nhóm C và mục 5.1 nhưng **không** đưa vào bảng đếm theo tính năng. Con số đúng cho `satthu` là **29** hàm phải viết (hoặc kéo thêm `mission_match.lua` vào bao đóng). **[đã sửa trong mục 1.1]**

**B-3. `unpack` KHÔNG tồn tại trong JX1 — cả thư viện Lua lẫn bảng đăng ký.**
`grep '"unpack"'` toàn bộ `D:\GAMEDEVNEW\Sources` ⇒ **0 kết quả**; `Library/LuaLib/src/baselib/lbaselib.c` có `getn`/`sort`/`tinsert` nhưng **không có `unpack`**.
Trong bản Linux thì `unpack` **là hàm engine** (luamap `0x080FF9E0`) *và* còn được script định nghĩa lại (`lib/basic.lua:24`, `lib/common.lua:16`).
Bao đóng dùng `unpack` ở nhiều nơi, đáng chú ý `protocol.lua:106` — `DynamicExecuteByPlayer(PlayerIndex, szFile, szFun, unpack(tbParam))` — mà `protocol.lua` nằm trong bao đóng của **cả ba** tính năng.
Cả ba bao đóng **đều có** `lib/common.lua` nên trên giấy tờ là đủ, **nhưng** JX1 cấp **một `Lua_State` riêng cho mỗi tệp `.lua`** (chính ghi chú của `LuaCurPack`/`LuaUsePack` trong `ScriptFuns.cpp` nói vậy) ⇒ phải bảo đảm `lib/common.lua` được `Include` **trong cùng state** với mọi tệp gọi `unpack`, nếu không sẽ vỡ âm thầm. Báo cáo không nhắc gì tới rủi ro này.

**B-4. Ngưỡng 20 000 000 của `ITEM_SetExpiredTime` — phải chép nguyên vào JX1.**
(chi tiết ở dòng 18 bảng A). Nếu JX1 hiện thực chỉ một đơn vị, một trong hai chỗ gọi của bao đóng sẽ sai: `fengling_ferry/bossdeath.lua:35` truyền **số phút còn lại tới nửa đêm** (tối đa 1440), còn `qianqiu_yinglie/head.lua:201` truyền **YYYYMMDD**. Đồng thời JX1 phải nhận **tham số thứ 3 = HHMMSS** (`event/change_destiny/npc.lua:348` truyền `235959`).

**B-5. `SetNpcScript` / `SetNpcDeathScript`: ba lệch hành vi mà báo cáo không nêu.**
Đã chứng minh JX2 dùng **một hàm chung** `0x08101500`. So với JX1:
1. JX2 nhận **tham số thứ 3** (số) và ghi vào `npc+0x158C` (`0x081015E0`–`0x0810160F`); **cả hai** hàm JX1 đều bỏ qua.
2. JX1 `LuaSetNpcActionScript` còn chạy thêm `Npc[n].ExecuteScript(szScript, "OnRevive", nNpcIndex)` khi `m_Kind == kind_normal` (`ScriptFuns.cpp:7134`) — JX2 **không có** bước này ⇒ script JX2 port sang sẽ bị gọi `OnRevive` ngoài ý muốn.
3. JX1 `LuaSetNpcDeathScript` (`KJx2WarInfra.cpp:49-61`) **không** làm bước đổi tiền tố `\script\tong\` → `\scriptjx2\tong_vn\` mà `LuaSetNpcActionScript` có (`ScriptFuns.cpp:7100-7101`) ⇒ hai tên cùng nghĩa ở JX2 lại **giải đường dẫn khác nhau** ở JX1.

**B-6. `GetGlodEqIndex` và `GetPlatinaEquipIndex` là MỘT hàm, khác mỗi hằng số.**
`0x080FEF90` = `sub_080FEEB0(L, 1)`, `0x080FEF70` = `sub_080FEEB0(L, 4)` — bảy lệnh y hệt nhau.
Báo cáo tách thành hai mục độc lập với hai mô tả riêng. Khi port chỉ cần **một** hàm C nhận thêm tham số phẩm chất, đăng ký hai tên Lua trỏ vào nó (đúng khuôn `LuaBT_GetDataStub` mà JX1 đã dùng cho `BT_GetData`/`BT_GetGameData`). Hằng `1`/`4` cũng **xác nhận độc lập** bảng mã phẩm chất `1 = hoàng kim`, `4 = bạch kim` mà mục `GetItemQuality` khẳng định.

### C. Những gì tôi cố bác nhưng KHÔNG bác được

- Toàn bộ **235 trích dẫn `tệp:dòng -> hàm C`** của nhóm A: kiểm tự động **235/235 khớp**. Không có mục nào bịa.
- Toàn bộ **42 địa chỉ ELF** nhóm B: **42/42 khớp luamap**; và **42/42 tên thật sự vắng mặt** trong JX1.
- Quét độc lập bao đóng cho ra **cùng bộ chỗ gọi**: mọi tên trong nhóm A lẫn nhóm B đều **thật sự** được gọi trong bao đóng (không có tên ma), và **tổng (tính năng × chỗ gọi)** của cả 42 dòng nhóm B trùng khít với cột cuối của báo cáo — tức là đường ống dữ liệu của báo cáo chạy đúng, chỉ đặt sai **tên cột**.
- Không tìm thấy **stub JX1 nào bị bỏ sót**: 13 stub khác trong `ScriptFuns.cpp` (`SetItemMagicLevel`, `GetPartnerBagLevel`, `SetPartnerBagLevel`, `ChangeNpcFeature`, `ST_SyncMiniMapObj`, `CreateChannel`/`EnterChannel`/`LeaveChannel`/`DeleteChannel`, `TaskName`/`TaskTime`/`TaskInterval`/`TaskCountLimit`) **không** nằm trong bao đóng của ba tính năng ⇒ nhóm C đủ 9 tên.
- Rủi ro "thư viện chuẩn Lua 4.0 bị loại nhầm": JX1 mở `Lua_OpenBaseLib` + `Lua_OpenIOLib` + `Lua_OpenStrLib` + `Lua_OpenMathLib` (`Engine/Src/KLuaScript.cpp:514-517`) ⇒ `openfile`/`read`/`write`/`closefile`/`execute`/`date`/`dostring`/`getglobal`/`setglobal`/`settagmethod`… mà bao đóng dùng đều **có sẵn**. Ngoại lệ duy nhất là `unpack` (xem B-3).
- `login_add` (`global/login_head.lua:7`, ngoài bao đóng) tuy bị gọi ở `task/metempsychosis/task_func.lua:218` nhưng **được bọc** `if login_add then … end` ⇒ **không** phải khe hở.
- `OutputMsg` (ngoài bao đóng) **JX1 đã có**: `ScriptFuns.cpp:14879 -> LuaJX2_OutputMsg`, và còn có bản script `serverscript_jx2/lib/gb_modulefuncs.lua:32` ⇒ **không** phải khe hở.
