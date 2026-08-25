# 15 — BỔ SUNG VÒNG 2: SOÁT LẠI BẢNG KHE HỞ API (`04_api_gap.md`) VỚI TINH THẦN PHẢN BÁC

| Mục | Nội dung |
|---|---|
| Ngày | 24/08/2026 |
| Vai trò | Hướng quét 6 — soát độc lập `04_api_gap.md` / `04_api_gap.json` / `api_gap_raw.json` |
| Nguyên tắc | Mọi khẳng định của vòng 1 bị coi là SAI cho tới khi tệp gốc / mã máy chứng minh ngược lại. Không sửa tệp `00_`..`09_`. |
| Công cụ tự dựng (độc lập, không dùng lại `api_gap_3hd.py`) | `scan2.py` (quét lại bao đóng, khử byte GBK trước khi bắt định danh ASCII), `defs.py` (chỉ mục 8.756 tên toàn cục do 5.230 tệp Lua Linux định nghĩa), `dumpreg.py` (1.026 tên Lua JX1 đăng ký, quét CẢ `Core\Src\ScriptFuns.cpp`, `Core\Src\LuaFuns.cpp` **và** `Engine\Src\LuaFuns.cpp`), `holes.py` / `holes2.py` / `tabs.py` / `guards.py` |
| Đối chiếu số nền | 5.230 tệp `.lua` (khớp vòng 1) · 1.560 tên luamap (khớp) · **855** tên gọi trần trong bao đóng (vòng 1 phản biện: 856 — lệch 1, không ảnh hưởng kết luận) |

---

## 0. KẾT QUẢ MỘT DÒNG

**Nhóm B giảm từ 42 xuống 40 hàm** (−3 hàm JX1 **đã có sẵn dưới tên khác**, +1 hàm `JoinMission` được đưa chính thức vào bảng). Trong 40 hàm đó chỉ **38 hàm thật sự phải viết mã C mới**; 1 hàm là **mở rộng hàm JX1 có sẵn**, 1 hàm **không chặn** vì chỗ gọi có `if … then`.

```
Vòng 1 : (B) 42 hàm  [+ JoinMission chỉ nhắc trong lời văn, không có dòng bảng]
Vòng 2 :
   − ITEM_DropRateItem   → JX1 ĐÃ CÓ, tên khác: DropRateItem        (chỉ cần 1 dòng đăng ký)
   − NPCINFO_GetSeries   → JX1 ĐÃ CÓ, tên khác: GetNpcSeries        (chỉ cần 1 dòng đăng ký)
   − ST_IsTransLife      → suy ra từ ST_GetTransLifeCount JX1 ĐÃ CÓ (cùng byte player+0x86B8)
   + JoinMission         → khe hở THẬT của CẢ satthu LẪN vuotai (vòng 1 chỉ nói satthu)
  ⇒ (B) = 42 − 3 + 1 = 40
      trong đó  NpcDropMoney     = mở rộng DropNpcMoney của JX1 (thêm tham số 3)
                NpcName2Replace  = KHÔNG CHẶN (chỗ gọi có guard)
  ⇒ số hàm PHẢI VIẾT MỚI HOÀN TOÀN = 38
```

Phân bố lại theo tính năng:

| Tính năng | Vòng 1 | Bỏ vì JX1 đã có | Thêm `JoinMission` | **Vòng 2** |
|---|---|---|---|---|
| Săn boss sát thủ | 28 (lời văn nói 29) | −3 (`ITEM_DropRateItem`, `NPCINFO_GetSeries`, `ST_IsTransLife`) | +1 | **26** |
| Phong Lăng Độ | 28 | −1 (`ST_IsTransLife`) | 0 | **27** |
| Vượt ải | 39 | −1 (`ST_IsTransLife`) | +1 | **39** |

Ngoài API còn **1 lỗi script mới phát hiện** mà vòng 1 bỏ sót: `tbTimerLog:weiMing(...)` ở **tệp lõi của Vượt Ải** — xem mục 7.

---

## 1. BA HÀM ĐỔI PHÂN LOẠI B → A (JX1 ĐÃ CÓ, CHỈ KHÁC TÊN)

Vòng 1 (và cả tác tử phản biện của vòng 1) chỉ chạy `grep '"<tên>"'` trên `Sources` rồi kết luận "42/42 vắng mặt". Tôi **xác nhận lại** kết quả grep đó là đúng về mặt CHUỖI (tôi chạy lại `grep -i` toàn bộ `Sources`, 42/42 = 0 kết quả thật; 5 kết quả của `File_Create` đều là `__blob_file_create` / `my_tmp_file_created` trong thư viện BerkeleyDB/MySQL, không liên quan).

**Nhưng vòng 1 KHÔNG kiểm ALIAS.** Tôi trích 1.026 tên Lua JX1 đăng ký rồi dò tên đồng nghĩa cho từng hàm nhóm B. Ba hàm sau JX1 **đã hiện thực đầy đủ**, chỉ mang tên khác.

### 1.1 `ITEM_DropRateItem` ≡ JX1 `DropRateItem` — TRÙNG KHÍT

| | Bản Linux (JX2) | JX1 |
|---|---|---|
| Đăng ký | luamap `0x08154DE0` | `Core\Src\ScriptFuns.cpp:14410 -> LuaDropRateItem` |
| Thân hàm | `0x08154DE0` | `Core\Src\ScriptFuns.cpp:4315` |
| Kiểm tham số | `call 0x8107910` (GetGlobalPlayerIndex) rồi `call 0x8232490` (`lua_gettop`), `cmp eax, 5` ⇒ **> 5 tham số** | `int nPlayerIndex = GetPlayerIndex(L);` + `if (nParamNum < 6) return 0;` (dòng 4318, 4321) |
| p1 | `lua_tonumber` idx 1 | `nNpcIndex = Lua_ValueToNumber(L,1)` (4325) |
| p2 | `lua_tonumber` idx 2 | `nCount` (4335) |
| p3 | `lua_tostring` idx 3 + `cmp byte [eax],0` (chặn chuỗi rỗng) | `pFileName = Lua_ValueToString(L,3)` (4336) |
| p4 / p5 / p6 | `lua_tonumber` idx 4, 5, 6 | `nUnknow`, `nItemLevel`, `nItemSeries` (4337-4339) |
| Trả về | `0` giá trị | `return 0;` (4344) |

Thân thi hành JX1 **không phải stub**: `KNpc::DropRateItem` (`Core\Src\KNpc.cpp:8988`) nạp bảng tỉ lệ rơi bằng `KNpcTemplate::UpdateDropRate(pszFileName)` (dòng 8993) rồi sinh vật phẩm thật (vòng `while (j < nCount)` dòng 9008, lấy may mắn từ `Player[nBelongIdx].m_nCurLucky` dòng 9021).

> **Việc phải làm khi port**: thêm **một dòng** `{"ITEM_DropRateItem", LuaDropRateItem},` vào `GameScriptFuns[]`.
> **Lệch duy nhất**: JX1 nhận `nUnknow` rồi **không dùng** (`KNpc.cpp:8988` có tham số `nUnknown` nhưng thân hàm không đọc). Chỗ gọi duy nhất trong bao đóng truyền `0` (`task/tollgate/killer/kill_level.lua:69`) nên **vô hại**.

### 1.2 `NPCINFO_GetSeries` ≡ JX1 `GetNpcSeries` — TRÙNG KHÍT

| | Linux | JX1 |
|---|---|---|
| Địa chỉ / dòng | `0x081C08E0` | `ScriptFuns.cpp:14504 -> LuaGetNpcSeries`, thân `ScriptFuns.cpp:7195` |
| Chữ ký | `(nNpcIndex) → 1 số` | `if (Lua_GetTopIndex(L)==1) { … Lua_PushNumber(L, Npc[nNpcIndex].m_Series); return 1; }` (7197-7207) |
| Thất bại | đẩy 0 | `Lua_PushNumber(L, 0); return 1;` (7206) |

Cùng ý nghĩa (ngũ hành NPC), cùng số trả về, cùng giá trị mặc định 0.
> **Việc phải làm**: một dòng `{"NPCINFO_GetSeries", LuaGetNpcSeries},`.
> Lưu ý JX1 **đã** có sẵn cả bộ `SetNpcSeries` (`:14503`), `GetSeries`/`SetSeries` cho người chơi (`:14631/:14632`).

### 1.3 `ST_IsTransLife` — suy trực tiếp từ `ST_GetTransLifeCount` mà JX1 ĐÃ CÓ

Dịch ngược `0x081C1160` (29 lệnh, đọc trọn):

```
0x081C116D  call 0x8107910                       ; GetGlobalPlayerIndex
0x081C1179  cmp ecx, 0x4AE / ja                  ; chặn 1..1199
0x081C1186  imul edx, edx, 0x8788                ; bước bản ghi người chơi
0x081C118C  cmp byte ptr [edx+eax+0x86b8], 0     ; <<< ĐÚNG BYTE player+0x86B8
0x081C1197  setne al                             ; → 0 / 1
0x081C11A7  call 0x8232d40                       ; lua_pushnumber
```

Mục 3.1 của `04_api_gap.md` đã tự chứng minh `ST_GetTransLifeCount` (`0x081C1100`) đọc **cùng byte `player+0x86B8`**, và JX1 ánh xạ nó sang `LuaGetPlayerReBornValue` (`ScriptFuns.cpp:5528`, thân gọi `m_cReBorn.GetReBornValue()` dòng 5538) — đã đăng ký ở `ScriptFuns.cpp:14336`.

⇒ **`ST_IsTransLife()` ≡ `ST_GetTransLifeCount() ~= 0`**. Không cần đọc trường mới, không cần cấu trúc mới. Có thể làm bằng **3 dòng C** (`Lua_PushNumber(L, Player[i].m_cReBorn.GetReBornValue() ? 1 : 0)`) hoặc thậm chí bằng **một shim Lua thuần**.

Đây là dòng nhóm B có **số chỗ gọi cao nhất** của vòng 1 (ghi 13 = "tính năng × chỗ gọi"; **thật sự 5 chỗ gọi**, xem mục 6) nên việc gỡ nó khỏi nhóm B là mức giảm công lớn nhất của vòng soát này.

---

## 2. HAI HÀM Ở LẠI NHÓM B NHƯNG **HẠ MỨC**

### 2.1 `NpcDropMoney` — JX1 có `DropNpcMoney`, **thiếu đúng 1 tham số**

| | Linux `NpcDropMoney` `0x0811D9C0` | JX1 `DropNpcMoney` (`ScriptFuns.cpp:14411 -> LuaDropNpcMoney`, thân `:4267`) |
|---|---|---|
| Tham số | 3 số: `(nNpcIndex, nMoney, nBelongerIndex)` | **2** số: `(nNpcIndex, nMoneyNum)` (`:4283-4284`) |
| Người sở hữu đồ rơi | lấy từ **tham số 3** | lấy từ **biến toàn cục `PlayerIndex`**: `GetPlayerIndex(L)` (`:4269`) rồi `Object[nObjIdx].SetItemBelong(nPlayerIndex)` (`:4308`) |
| Trả về | 0 giá trị | 1 số (0 / 1) |

Chỗ gọi duy nhất trong bao đóng: `lib/droptemplet.lua:159` → `NpcDropMoney(nNpcIndex, nAddJxb, nBelongerIndex)` — **truyền belonger tường minh**.

> **Hậu quả nếu bê nguyên `DropNpcMoney` của JX1**: khi boss chết, script chạy trong ngữ cảnh không có `PlayerIndex` hợp lệ (hoặc có nhưng là người khác) thì JX1 thoát sớm ở nhánh `if (nPlayerIndex <= 0)` **`:4270`**, `Lua_PushNumber(L, 0); return 1;` ở **`:4272-4273`** (trả 1 giá trị 0, không phải `return 0` trắng) ⇒ **KHÔNG rơi tiền chút nào**, im lặng. *[đã sửa theo đối chất]* Đây là đường tiền của cả ba hoạt động.
> **Việc phải làm**: mở rộng `LuaDropNpcMoney` nhận tham số 3 tuỳ chọn (`nBelongerIndex`, mặc định = `GetPlayerIndex(L)`), rồi đăng ký thêm tên `NpcDropMoney`. **Không phải viết mới từ đầu.**

### 2.2 `NpcName2Replace` — **KHÔNG CHẶN** (chỗ gọi tự bảo vệ)

Chỗ gọi **duy nhất** trong bao đóng, đọc nguyên văn:

```
missions/challengeoftime/npc/transfer.lua:10
    if NpcName2Replace then szNpcName = NpcName2Replace(szNpcName) end
```

Nếu JX1 không đăng ký tên này, biến toàn cục là `nil` ⇒ nhánh bị bỏ qua, `szNpcName` giữ nguyên tên gốc. **Không có lỗi runtime**, chỉ mất bước đổi tên hiển thị.
> Xếp lại: **B — mức THẤP, có thể hoãn**. Vòng 1 liệt kê nó ngang hàng với các hàm chặn.

Tôi đã quét guard cho **cả 42 hàm** (`guards.py`, đọc thẳng dòng gọi): **chỉ `NpcName2Replace`** có guard. 41 hàm còn lại gọi trần ⇒ thiếu là `attempt to call global`.

---

## 3. `JoinMission` — NÂNG THÀNH KHE HỞ NHÓM B CỦA **CẢ HAI** TÍNH NĂNG

Vòng 1 để `JoinMission` ở nhóm C ("cảnh báo trùng tên") + mục 5.2 "CHƯA XÁC MINH", phản biện vòng 1 kéo nó thành khe hở của **riêng `satthu`**. Tôi giải quyết dứt điểm được cả hai điểm.

### 3.1 Chỗ gọi truyền **mission id**, không phải RoleIndex — ĐÃ XÁC MINH

`missions/challengeoftime/include.lua:6` → `MISSION_MATCH = 22`.
Toàn bộ chỗ dùng khác của hằng này đều đưa nó làm **tham số 1 của API mission**:
`AddMSPlayer(MISSION_MATCH, camp)` (`mission_match.lua:66`), `Msg2MSAll(MISSION_MATCH, …)` (`:76`), `StartMissionTimer(MISSION_MATCH, …)` (`include.lua:209`), `GetMSPlayerCount(MISSION_MATCH, 1)` (`include.lua:176`).
⇒ `JoinMission(MISSION_MATCH, 1)` ở `npc/dragonboat_main.lua:163` = `JoinMission(nMissionId=22, nCamp=1)` — **hàm ENGINE**.

### 3.2 Chữ ký hàm engine — dịch ngược `0x08137E40` (85 lệnh)

```
0x08137E52  call 0x8232490            ; lua_gettop
0x08137E57  cmp eax, 1 / jle → thoát  ; BẮT BUỘC ≥ 2 tham số
0x08137E6B  call 0x82339b0 (idx 1)    ; p1 = số nguyên
0x08137E70  test eax,eax / js → thoát ; p1 phải ≥ 0   (= mission id)
0x08137E89  call 0x82338b0 (idx 2)    ; p2 = số        (= camp)
0x08137E97  call 0x8106a40            ; chỉ số SubWorld, phải ≥ 0
0x08137EAD  call 0x8107860            ; GetGlobalPlayerIndex, phải > 0
…           khung ngăn xếp 0x751C byte, hằng 0x4AF (=1199) → duyệt danh sách người chơi
```

⇒ `JoinMission(nMissionId ≥ 0, nCamp) → 0 giá trị`, chạy trên **người chơi hiện tại** + **subworld hiện tại**. Khớp hoàn toàn cách gọi ở `dragonboat_main.lua:163`, **không** khớp `function JoinMission(RoleIndex, camp)` của `mission_match.lua:93`.

### 3.3 Vì sao là khe hở của **vuotai** nữa (điểm vòng 1 bỏ sót)

`closure3.json`: `satthu` có `dragonboat_main.lua` mà **không** có `mission_match.lua`; `vuotai` có **cả hai**; `phonglangdo` **không có tệp nào**.

Vòng 1 lập luận "với `vuotai` thì hàm script đè lên hàm engine nên không sao". **Lập luận đó SAI trên JX1**, vì JX1 cấp **một `Lua_State` riêng cho mỗi tệp `.lua`**:
- `Core\Src\KSortScript.cpp:151-152` — mỗi tệp nạp vào một `g_ScriptSet[n]` riêng rồi `RegisterFunctions(GameScriptFuns, …)` lại từ đầu;
- ghi chú trong nguồn nói thẳng: *"JX1: mỗi file .lua một Lua_State riêng nên 'pack hiện tại' luôn là 0"* (`ScriptFuns.cpp:2182-2185`, ngay trên `LuaCurPack`).

⇒ Trên JX1, `function JoinMission` của `mission_match.lua` **không thể** che lời gọi nằm trong `dragonboat_main.lua` (khác state). Hàm engine **luôn thắng**. Vậy JX1 **bắt buộc** phải đăng ký `JoinMission`, nếu không `dragonboat_main.lua:163` sẽ `attempt to call global` ở **cả `satthu` lẫn `vuotai`**.

### 3.4 Tiền lệ có sẵn trong cây JX1

Cây máy chủ JX1 đang chạy **đã có** một `JoinMission` mức script:
`E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\missions\tongwar\match\mission.lua:163`
```lua
function JoinMission(RoleIndex, Camp)
    PlayerIndex = RoleIndex;
    if (Camp ~= 1 and Camp ~= 2) then return end
    sf_join(Camp)
end;
```
(bản sao ở `D:\GAMEDEVNEW\serverscript_jx2\tongwar\script\missions\tongwar\match\mission.lua`).
Do một-state-một-tệp, bản script này chỉ có hiệu lực **trong chính tệp đó** ⇒ đăng ký hàm engine `JoinMission` **không phá** Tống chiến. Cần ghi chú lại kẻo về sau tưởng trùng.

---

## 4. KIỂM 39 HÀM NHÓM B CÒN LẠI — KHÔNG BÁC ĐƯỢC

Với 39 tên còn lại tôi làm đủ ba bước độc lập:

**(a) grep tên thật** — `grep -ril` toàn `D:\GAMEDEVNEW\Sources` (`*.cpp`, `*.h`): **0 kết quả thật** cho cả 39.

**(b) dò alias** — đối chiếu từng tên với 1.026 tên JX1 đăng ký theo từ khoá ngữ nghĩa. Các ứng viên đáng ngờ nhất đều đã đọc thân hàm và **bác bỏ**:

| Hàm nhóm B | Ứng viên alias JX1 | Kết luận sau khi đọc thân hàm |
|---|---|---|
| `GetFirstPlayerAtServer` / `GetNextPlayerAtServer` | `GetNextPlayer` (`:14761 -> LuaGetNextPlayer`, thân `:11647`) | **KHÔNG PHẢI.** JX1 `GetNextPlayer(mission, idx, group)` duyệt **người chơi trong MỘT mission** (`pMission->GetNextPlayerC`), còn bản Linux duyệt **toàn máy chủ**. Giữ nhóm B. |
| `Tm2Time` | `MakeDateTime` (`:15265`, thân `:3922`), `Time2Tm` (`:14891`) | **KHÔNG PHẢI — ngược chiều.** Cả hai của JX1 đều **phân rã** `time_t` → (Y,M,D,h,m,s,wday,yday); `Tm2Time` là chiều **ghép** (Y,M,D,h,m,s) → `time_t`. Giữ nhóm B (nhưng chỉ là bọc `mktime`). |
| `GetItemAllParams` | `GetItemParam` (`:14439`, thân `:5683`) | **KHÔNG PHẢI vỏ, NHƯNG DỮ LIỆU ĐÃ CÓ.** JX1 `GetItemParam(idx, k)` với `k=1..6` đọc đúng `Item[idx].m_GeneratorParam.nGeneratorLevel[k-1]` — chính là `KItem+0x1E0` mà Linux đọc (ghi chú `ScriptFuns.cpp:5709-5710` nói đúng như vậy). Chỉ thiếu **vỏ trả về BẢNG**. Công viết ≈ 10 dòng. |
| `GetNpcAroundPlayerList` | `GetNpcAroundNpcList` (`:15266`, thân `KJx2WarInfra.cpp:349`), `GetAroundNpcList` (`:15195`, thân `:532`) | **KHÔNG PHẢI (trả NPC, không trả người chơi), NHƯNG LÀ KHUÔN SẴN.** JX1 đã có hàm cùng hình dạng trả `(bảng, số lượng)` và đã xử lý đúng bẫy toạ độ cục bộ của region (`KJx2WarInfra.cpp:550-559`). Chỉ cần đảo bộ lọc: giữ `Npc[i].IsPlayer()` rồi đẩy `Npc[i].GetPlayerIdx()`. |
| `IniFile_Save`, `IniFile_SetData`, `File_Create` | `IniFile_Load` / `IniFile_GetData` / `IniFile_UnLoad` (`:15288-15290`, thân `ScriptFuns.cpp:3190/3214/3232`) | **BỘ MÁY ĐÃ CÓ.** JX1 đã có `struct KTongIniEntry { KIniFile Ini; … }` + bảng băm `s_mapTongIniFiles` (`ScriptFuns.cpp:3172-3173`), và lớp `KIniFile` đã có **`Save(LPCSTR)`** (`Engine\Src\KIniFile.h:49`) + **`WriteString` / `WriteInteger`** (`:119` / `:124`). Ba hàm này là vỏ Lua ≈ 10 dòng/hàm. |
| `OpenProgressBar` | `PaceBar` (`:14730 -> LuaOpenTimeBox`, thân `:9675`) | **GẦN — cùng cơ chế, khác gói tin.** JX1 gửi `S2C_TIME_BOX` và cũng có nhánh "tên tệp script tuỳ chọn → `g_FileName2Id`" (`:9691-9692`) đúng như tham số 7 của bản Linux (xem 5.2). Nhưng số/kiểu tham số khác ⇒ vẫn phải viết mới; có thể dùng lại đường gói tin của `LuaOpenTimeBox`. |
| `SetItemBindState` | — | **KHÔNG CÓ.** `Core\Src\KItem.h` không có trường bind nào (`grep Bind` = 0). Ứng viên gần nhất là `InsuranceCourse` "trạng thái khoá" (`KItem.h:137`). **CHƯA XÁC MINH** có dùng lại được không. |
| `ITEM_GetExpiredTime` / `ITEM_SetExpiredTime` / `ITEM_SetLeftUsageTime` | — | **KHÔNG CÓ vỏ, NHƯNG TRƯỜNG ĐÃ CÓ**: `KItem.h:101 nExpireTime` (đang được dùng thật: `KItem.cpp:2627` so `m_CommonAttrib.nExpireTime > KSG_GetCurSec()`), `KItem.h:87 nParam` = "số lần sử dụng item", `KItem.h:67 nExpirePoint`. Chú ý `KItem.cpp:2629` cộng mốc `1451581200` — phải xem lại kẻo lệch gốc thời gian. |
| `AddStatData` | `AddStation` / `Get*State` | **KHÔNG PHẢI** — chỉ trùng ký tự. |
| `Add120SkillExp` | `IncSkillExp` (`:14475`, thân `:12581`) | **KHÔNG PHẢI.** JX1 `IncSkillExp(nSkillId, nAddExp)` cần **id kỹ năng**; bản Linux `Add120SkillExp(nExp)` **không** nhận id (cộng thẳng vào độ tu luyện kỹ năng 120). Giữ nhóm B. |
| `ST_DoTransLife` / `ST_LevelUp` | `AddTranslife`/`GetTranslife` (`:14444/:14445`), `AddReBorn`/`SetReBorn`/`GetReBornNum` (`:14335/:14331/:14337`) | **KHÔNG PHẢI.** Nhóm JX1 chỉ **cộng/đọc giá trị**; `ST_DoTransLife` gọi cả thủ tục chuyển sinh (`0x080B2180` trên đối tượng người chơi), `ST_LevelUp` gọi `0x080AFE10(pPlayer, nDelta)`. Giữ nhóm B. |
| `TrimString` | `PushString`/`PopString`/`AppendString`/`ReplaceString` (đăng ký ở `ScriptFuns.cpp:15217-15220`; `:13778-13781` chỉ là 4 dòng khai báo `extern`) *[đã sửa theo đối chất]* | **KHÔNG PHẢI, NHƯNG BỘ ĐỆM CHUỖI DÙNG CHUNG ĐÃ CÓ Ở JX1** (4 hàm kia đều nằm trong nhóm A). `TrimString()` chỉ là thao tác thứ 5 trên chính bộ đệm đó ⇒ ≈ 8 dòng. |

**(c) kiểm "có phải hàm do script Linux định nghĩa không" (nhóm D)** — đối chiếu 42 tên với chỉ mục 8.756 tên toàn cục của **toàn bộ** cây Linux (kể cả kiểu gán bí danh `X = Y` mà vòng 1 từng bỏ sót ở `ContriValueEntryLogic`): **0/42 trùng**. Không tên nào trong nhóm B thật ra là hàm script.

**Không hàm nào trong 42 là "HÀM RỖNG"** (`xor eax,eax; ret`): số lệnh nhỏ nhất là 11 (`GetGlodEqIndex` / `GetPlatinaEquipIndex` — thunk gọi helper chung `0x080FEEB0`, xem 5.5) và 15 (`GetFirst/GetNextPlayerAtServer` — thunk gọi bộ lặp `0x080C3EE0` / `0x080C3F10`, xem 5.4). Cả bốn đều có thân thật ở hàm được gọi.

---

## 5. GIẢI QUYẾT MỤC "5.2 CÁC ĐIỂM CHƯA XÁC MINH" CỦA `04_api_gap.md`

Vòng 1 để lại **4 điểm** chưa xác minh. Tôi giải quyết được **cả 4**.

### 5.1 `JoinMission` — gọi hàm engine hay hàm script cùng tên?
**ĐÃ GIẢI QUYẾT — hàm ENGINE.** Xem mục 3 (`MISSION_MATCH = 22` là mission id; chữ ký engine khớp; JX1 một-state-một-tệp nên không thể bị che).

### 5.2 `GetNpcAroundPlayerList` — tham số thứ 3 là gì?
**ĐÃ GIẢI QUYẾT — tham số 3 TUỲ CHỌN, mặc định 31.** Dịch ngược `0x08104870`:

```
0x081048F2  cmp esi, 2                 ; esi = lua_gettop
0x081048F5  mov edx, 0x1f              ; <<< MẶC ĐỊNH = 31
0x081048FA  jg  0x81049e8              ; top > 2 → đọc tham số 3
…
0x081049E8: mov [esp+4], 3 / call lua_tonumber   ; nhánh 3 tham số
0x08104A17  jmp 0x8104900                        ; nhập lại luồng chính
```
`edx` (31 hoặc tham số 3) được đặt vào `[esp+0x14]` của lời gọi khởi tạo bộ lặp `0x080EF9F0`, cạnh hai đối số `-1, -1`. Hình dạng `0x1F = 11111b` + hai `-1` cho thấy đây là **mặt nạ lọc 5 bit** (nhiều khả năng là mặt nạ phe/quan hệ), **không phải bán kính** (bán kính là tham số 2).
Chỗ gọi duy nhất trong bao đóng chỉ truyền 2 tham số (`missions/boss/bigboss.lua:218` — `GetNpcAroundPlayerList(nNpcIndex, 20)`) ⇒ **port chỉ cần hiện thực dạng 2 tham số với mặc định 31**.

Bổ sung 2 chi tiết vòng 1 không có:
- mỗi phần tử được đẩy là `[đối tượng + 0xC0]` (`fild dword ptr [eax+0xc0]` ở `0x08104968`) rồi `lua_rawseti(L, -2, ++n)` (`0x0823 3360` với `[esp+4] = 0xFFFFFFFE = −2`) ⇒ đúng là **PlayerIndex**, không phải NpcIndex;
- **nhánh thất bại đẩy `nil` + `0`** (`lua_pushnil 0x8232E70` ở `0x081049C3`), **KHÔNG** đẩy bảng rỗng. JX1 `LuaGetNpcAroundNpcList` lại đẩy **bảng rỗng** (`KJx2WarInfra.cpp:353` gọi `Lua_NewTable` TRƯỚC mọi kiểm tra) ⇒ nếu chép khuôn JX1 y nguyên sẽ **lệch hành vi**; script Linux có thể đang dựa vào `nil`.

### 5.3 `OpenProgressBar` — tham số thứ 7 là gì?
**ĐÃ GIẢI QUYẾT — tham số 7 TUỲ CHỌN, kiểu CHUỖI = tên tệp script.** Dịch ngược `0x081082D0`:

```
0x081082EA  cmp eax, 5 / jg            ; bắt buộc > 5 tham số  (tức ≥ 6)
p1 lua_tostring, p2..p4 lua_tonumber, p5 lua_tostring, p6 lua_tostring
0x08108383  cmp esi, 6 / jne 0x81084e8 ; nếu top ≠ 6 → nhánh 7 tham số
--- top == 6 ---
0x08108392  call 0x8220780             ; lấy ID SCRIPT HIỆN TẠI
--- top >= 7 (0x081084E8) ---
0x081084E8  mov [esp+4], 7 / call 0x8233850  ; lua_tostring idx 7
0x081084F8  test eax,eax / je → nhánh 6 tham số ; [đã sửa theo đối chất] tham số 7 NIL/không phải chuỗi cũng bỏ qua
0x08108500  cmp byte [eax], 0 / je → quay về nhánh 6 tham số  ; chuỗi rỗng = bỏ qua
0x0810850C  call 0x821DE70             ; đổi TÊN TỆP → ID
```
⇒ `OpenProgressBar(szTiêuĐề, n2, n3, n4, szHàmOnTime, szHàmOnBreak [, szTệpScript])`; **thiếu tham số 7 ⇒ dùng id script đang chạy**.
Chỗ gọi duy nhất truyền 6 tham số (`lib/progressbar.lua:95`) ⇒ **port chỉ cần dạng 6 tham số**.
Khuôn có sẵn ở JX1: `LuaOpenTimeBox` (`ScriptFuns.cpp:9675`) làm y hệt — `nParamNum > 4` thì `Player[].m_dwTimeBoxId = g_FileName2Id(szScript)` (`:9691-9692`), ngược lại lấy `Npc[…].m_ActionScriptID` (`:9699`).

### 5.4 `PET_*` — độ lệch trường trong bản ghi người chơi
**ĐÃ GIẢI QUYẾT — đối chiếu chéo GETTER ↔ SETTER, cả 3 cặp đều khớp.** (Vòng 1 chỉ đoán được `+0x873C` và nghi `+0x8738`; **`+0x8738` là SAI**, không hàm nào đụng tới nó.)

| Cặp hàm | Getter đọc | Setter ghi | Kết luận |
|---|---|---|---|
| `PET_GetGrownPoint 0x081D5FE0` / `PET_SetGrownPoint 0x081D6230` | `fild [edx+eax+0x873c]` (`0x081D600C`) | `mov [esi+0x873c], eax` (`0x081D628C`) | **`player + 0x873C`** ✔ |
| `PET_GetTamePoint 0x081D5F90` / `PET_SetTamePoint 0x081D5F10` | `fild [edx+eax+0x8740]` (`0x081D5FBC`) | `mov [esi+0x8740], eax` (`0x081D5F6C`) | **`player + 0x8740`** ✔ |
| `PET_GetUpgradePoint 0x081D6150` / `PET_SetUpgradePoint 0x081D70E0` | `fild [eax+edx+0x8730]` (`0x081D6194`) | `mov [esi+0x8730], eax` (`0x081D7155`) | **`player + 0x8730`** ✔ |

Hai chi tiết mới, **bắt buộc chép khi port**:
1. **Cả 3 setter đều gọi đồng bộ**: sau khi ghi trường, chúng làm `add esi, 0x8704` rồi `call 0x081D4D10` (`0x081D6292-98`, `0x081D5F72-78`, `0x081D715B-64`). ⇒ khối dữ liệu thú cưng bắt đầu ở **`player + 0x8704`** và **phải phát gói đồng bộ**, không được chỉ ghi trường.
2. **`PET_GetUpgradePoint` trả `−1`** (không phải 0) khi chỉ số người chơi sai: `fld1 / fchs` ở `0x081D616D-6F`. Hai getter kia trả 0. Chỗ gọi `vng_event/thapnienlenhbai/lenhbai_def.lua:390` là `PET_SetUpgradePoint(PET_GetUpgradePoint() + nUpgrade)` ⇒ nếu JX1 trả 0 thay vì −1 sẽ **lệch 1 điểm** trong tình huống lỗi.

---

## 5.5 Bổ sung dịch ngược cho 3 hàm vòng 1 mô tả thiếu

**`GetGlodEqIndex` / `GetPlatinaEquipIndex`** — vòng 1 (mục B-6 của phản biện) đã chỉ ra chúng là **một** helper `0x080FEEB0(L, nPhẩmChất)`. Tôi đọc trọn helper và bổ sung **giá trị trả về thật**:
```
0x080FEEBD  call lua_gettop / test/jle → 0        ; cần ≥ 1 tham số
0x080FEED1  call lua_tonumber(L,1)                ; p1 = chỉ số vật phẩm
0x080FEEF3  cmp eax, [0x830ca5c] / jge → 0        ; chặn theo SỐ LƯỢNG vật phẩm
0x080FEEFB  imul eax, eax, 0x368 + [0x830d300]    ; bước bản ghi vật phẩm
0x080FEF0A  cmp edx, [eax+4] / jne → đẩy 0        ; [+4] = phẩm chất
0x080FEF48  mov eax, [eax+0x80] / add eax, 1      ; <<< TRẢ VỀ  KItem[+0x80] + 1
```
⇒ **`GetGlodEqIndex(idx)` = `Item[idx].[+0x80] + 1`** nếu `Item[idx].[+4] == 1`, ngược lại 0.
⇒ **`GetPlatinaEquipIndex(idx)`** y hệt với hằng `4`.
Mọi nhánh đều `return 1` (một giá trị). Chỗ gọi `lib/log.lua:39` / `:44` viết `… - 1` ⇒ **xác nhận độc lập rằng engine đã cộng 1**. Khi port JX1 **phải giữ đúng quy ước +1**, nếu không toàn bộ log ghi lệch một dòng bảng.

**`GetFirstPlayerAtServer` / `GetNextPlayerAtServer`** — vòng 1 chỉ ghi "PlayerIndex, 0 = hết". Thân thật:
```
GetFirst 0x08101CF0 → call 0x080C3EE0(0x08BAF320)
GetNext  0x08101D20 → call 0x080C3F10(0x08BAF320)
```
Đối tượng `0x08BAF320`: mảng ở `[obj+0x3C]`, **số lượng** ở `[obj+0x44]`, **con trỏ duyệt** ở `[obj+0x48]`, phần tử **rộng 8 byte**, giá trị ở `[phần tử+4]`.
- `GetFirst`: nếu số lượng ≠ 0 và mảng ≠ NULL → `kq = [mảng+4]`; ghi `[obj+0x48] = kq`; ngược lại `kq = 0`.
- `GetNext`: `cur = [obj+0x48]`; nếu `cur == 0` → 0; nếu `cur >= sốLượng` → đặt cursor 0, trả 0; ngược lại `kq = [mảng + cur*8 + 4]`, ghi cursor.

> **Hai hệ quả bắt buộc ghi vào bản port JX1:**
> 1. **Con trỏ duyệt là TOÀN CỤC, chỉ có MỘT.** Hai vòng lặp lồng nhau (hoặc hai script chạy xen kẽ trong cùng tick) sẽ **phá nhau**. Chỗ gọi trong bao đóng — `missions/boss/bigboss.lua:289` (`GetFirstPlayerAtServer`) và `:293` (`GetNextPlayerAtServer`) — nằm trong **cùng một vòng lặp**, an toàn; nhưng JX1 nếu hiện thực bằng biến `static` cũng phải chấp nhận đúng ràng buộc này (hoặc làm sạch hơn: quét `Player[1..MAX_PLAYER]`).
> 2. Đây **không** phải quét mảng người chơi thẳng: `0x08BAF320` khác gốc mảng người chơi `0x08BAEE60` (lệch `+0x4C0`) ⇒ là **danh sách người chơi ĐANG ONLINE** riêng, nên số lần lặp ≪ 1199.

**`AddStatData`** — vòng 1 ghi `(szTên [, nSốLượng]) → 0 giá trị`, đúng; bổ sung ràng buộc thật:
```
0x080FF562  call lua_gettop
0x080FF569  lea eax,[eax-1] / cmp eax,1 / jbe   ; CHỈ chấp nhận top == 1 HOẶC 2
0x080FF574  (ngược lại) xor eax,eax  … 0x080FF57C ret   ; 3 tham số trở lên = KHÔNG LÀM GÌ, im lặng
                                                 ; [đã sửa theo đối chất] 0x080FF571 là `mov ebx,[ebp-8]`
0x080FF58B  lua_tostring(L,1) / test → thoát    ; p1 bắt buộc là chuỗi
0x080FF596  cmp esi,2 / je → nhánh đọc p2       ; ngược lại nDelta = 1
0x080FF5B3  call 0x081D0420(0x0978C0A0, sz, nDelta, 0)
```
Bộ đếm thống kê nằm ở đối tượng toàn cục `0x0978C0A0`.

---

## 6. BẢNG 40 HÀM NHÓM B SAU KHI SOÁT — ĐỦ CỘT "PHỤC VỤ AI / GỌI Ở ĐÂU / THIẾU THÌ HỎNG GÌ"

Cột **"Chỗ gọi"** dưới đây là **chỗ gọi THẬT, đã khử trùng lặp giữa 3 bao đóng** (khác cột cuối của `04_api_gap.md` vốn là "tính năng × chỗ gọi"). Chữ ký lấy theo `04_api_gap.md` trừ khi mục 5 của tệp này sửa lại.

| # | Hàm | Tính năng | Chỗ gọi thật (tệp:dòng) | THIẾU THÌ HỎNG GÌ | Mức |
|---|---|---|---|---|---|
| 1 | `Add120SkillExp` | satthu, vuotai | `task/task_award_extend.lua:6` | Phần thưởng nhiệm vụ **không cộng tu luyện kỹ năng 120**; lỗi `call global` làm hỏng nốt phần thưởng phía sau | CAO |
| 2 | `AddStatData` | cả 3 | 11 chỗ: `activitysys/config/32/talkdailytask.lua:159`; `missions/challengeoftime/item/chuangguanbaoxiang.lua:169`; `missions/fengling_ferry/fld_death.lua:30/32/34`; `missions/fengling_ferry/fld_head.lua:291`; `missions/fengling_ferry/mission.lua:48/50/52`; `task/tollgate/killer/lib_killlevel.lua:145`; `task/tollgate/killer/shashou_mibao.lua:23` | Lỗi `call global` **ngay giữa** đường tính người chết / người tham gia của Phong Lăng Độ và đường mở rương của Sát thủ ⇒ cắt đứt hàm. (Bản thân số liệu chỉ là thống kê, nhưng lỗi thì chặn.) Có thể vá rẻ bằng hàm rỗng. | CAO (dễ) |
| 3 | `BT_GetBattleParam` | phonglangdo, vuotai | `battles/battlehead.lua:631`, `:639` | Không đọc được cấu hình 12 dòng bảng xếp hạng trận ⇒ `SetMissionV(MS_TRANK1_S…)` sai | TB |
| 4 | `DropItemEx` | cả 3 | `lib/droptemplet.lua:74` (**MỘT** chỗ gọi duy nhất cho cả 3) | **Toàn bộ hệ rơi đồ theo mẫu** của cả ba hoạt động chết | RẤT CAO |
| 5 | `File_Create` | cả 3 | `lib/file.lua:8` | Không tạo được tệp ini lưu trạng thái | TB |
| 6 | `FormatTime2Date` | phonglangdo, vuotai | `event/…/qianqiu_yinglie/head.lua:200`, `:217` | Không tính được ngày hết hạn YYYYMMDD của vật phẩm sự kiện | TB |
| 7 | `GetFirstPlayerAtServer` | cả 3 | `missions/boss/bigboss.lua:289` | Vòng duyệt người chơi toàn máy chủ của **bigboss** chết ⇒ hỏng thông báo/thưởng boss lớn | CAO |
| 8 | `GetNextPlayerAtServer` | cả 3 | `missions/boss/bigboss.lua:293` | như trên | CAO |
| 9 | `GetGlodEqIndex` | cả 3 | `event/…/qianqiu_yinglie/head.lua:67`; `lib/composeex.lua:142`; `lib/log.lua:39` | Ghép đồ hoàng kim + log rơi đồ sai/chết | CAO |
| 10 | `GetPlatinaEquipIndex` | cả 3 | `lib/composeex.lua:144`; `lib/log.lua:44` | như trên, phần bạch kim | CAO |
| 11 | `GetPlatinaLevel` | cả 3 | `lib/log.lua:43` | Log rơi đồ chết | TB |
| 12 | `GetItemQuality` | cả 3 | `event/…/qianqiu_yinglie/head.lua:65`; `lib/composeex.lua:138`; `lib/log.lua:37` | Ghép đồ + log chết | CAO |
| 13 | `GetItemAllParams` | cả 3 | `lib/log.lua:46` | Log rơi đồ chết (dữ liệu đã có ở JX1, chỉ thiếu vỏ) | TB |
| 14 | `GetItemGenTime` | cả 3 | `lib/log.lua:51` | Log rơi đồ chết | THẤP |
| 15 | `ITEM_GetItemRandSeed` | cả 3 | `lib/log.lua:48` | Log rơi đồ chết | THẤP |
| 16 | `GetNpcAroundPlayerList` | cả 3 | `missions/boss/bigboss.lua:218` | Không lấy được danh sách người chơi quanh boss ⇒ **không chia thưởng/không thông báo** | CAO |
| 17 | `GetRoomItems` | cả 3 | `lib/composeex.lua:191` | Hệ ghép đồ không quét được rương | CAO |
| 18 | `ITEM_GetExpiredTime` | cả 3 | `activitysys/activity.lua:314` | Không đọc được hạn dùng vật phẩm hoạt động | TB |
| 19 | `ITEM_SetExpiredTime` | cả 3 | `event/birthday_jieri/200905/class.lua:95`; `event/…/qianqiu_yinglie/head.lua:201`; `lib/droptemplet.lua:130`; `missions/fengling_ferry/bossdeath.lua:35`; `missions/fengling_ferry/shuizeideath.lua:17` | Vật phẩm rơi từ boss Phong Lăng Độ **không có hạn** ⇒ lỗi kinh tế. Phải chép ngưỡng 20 000 000 (vòng 1 đã xác minh) | RẤT CAO |
| 20 | `ITEM_SetLeftUsageTime` | cả 3 | `lib/droptemplet.lua:135`, `:140` | Vật phẩm rơi không giới hạn số lần dùng | CAO |
| 21 | `IniFile_Save` | cả 3 | `lib/file.lua:24` | Không ghi được tệp lưu | TB |
| 22 | `IniFile_SetData` | cả 3 | `lib/file.lua:19` | như trên | TB |
| 23 | `IsDisabledUseHeart` | vuotai | `item/heart_head.lua:116` | Không chặn được "Tâm Tâm Tương Ánh Phù" ⇒ dịch chuyển lậu vào bản đồ Vượt ải | TB |
| 24 | `NpcDropMoney` **(mở rộng)** | cả 3 | `lib/droptemplet.lua:159` | **Không rơi tiền** (JX1 có `DropNpcMoney` nhưng thiếu tham số belonger — mục 2.1) | CAO |
| 25 | `NpcName2Replace` **(có guard)** | vuotai | `missions/challengeoftime/npc/transfer.lua:10` | **Không hỏng gì** — chỉ giữ nguyên tên NPC | THẤP |
| 26 | `OpenProgressBar` | phonglangdo | `lib/progressbar.lua:95` | Mất thanh tiến trình (đò/vượt sông) | CAO |
| 27-32 | `PET_GetGrownPoint`, `PET_SetGrownPoint`, `PET_GetTamePoint`, `PET_SetTamePoint`, `PET_GetUpgradePoint`, `PET_SetUpgradePoint` | vuotai | `vng_event/thapnienlenhbai/lenhbai_def.lua:390`, `:391`, `:392` (3 dòng, mỗi dòng dùng 1 cặp get+set) | Lệnh bài thập niên **không cộng được điểm thú cưng** | TB |
| 33 | `QueryWiseManForSB` | vuotai | `item/heart_head.lua:129`, `:132` | Không hỏi được vị trí bạn đời ⇒ Tâm Tâm Tương Ánh vô dụng | TB |
| 34 | `ST_DoTransLife` | vuotai | `task/metempsychosis/translife_6.lua:102` | **Không chuyển sinh được** ở cuối chuỗi nhiệm vụ | CAO |
| 35 | `ST_LevelUp` | vuotai | `vng_event/thapnienlenhbai/lenhbai_def.lua:378` (`ST_LevelUp(nLevel - GetLevel())` ⇒ tham số là **SỐ CẤP CHÊNH**, đã xác minh) | Lệnh bài không nâng cấp được | TB |
| 36 | `SendScriptData` | cả 3 | `script_protocol/protocol_def_gs.lua:193` | **Toàn bộ giao thức script → client chết** (bảng điểm, UI hoạt động) | RẤT CAO |
| 37 | `SetItemBindState` | cả 3 | `battles/battlehead.lua:1317`; `lib/droptemplet.lua:144` | Vật phẩm rơi **không bị khoá** ⇒ bán/giao dịch được ⇒ lỗi kinh tế | RẤT CAO |
| 38 | `Tm2Time` | cả 3 | `activitysys/functionlib.lua:364`, `:380` | Lịch hoạt động không tính được mốc thời gian ⇒ **hoạt động không mở** | RẤT CAO |
| 39 | `TrimString` | cả 3 | `lib/string.lua:163` | Hàm cắt khoảng trắng dùng chung chết | TB |
| 40 | **`JoinMission`** (mới vào bảng) | **satthu + vuotai** | `missions/challengeoftime/npc/dragonboat_main.lua:163` | **Không báo danh được vào mission 22** ⇒ NPC báo danh Vượt ải/Sát thủ vô dụng | RẤT CAO |

> Ba dòng đã rời bảng: `ITEM_DropRateItem` (→ `DropRateItem`), `NPCINFO_GetSeries` (→ `GetNpcSeries`), `ST_IsTransLife` (→ suy từ `ST_GetTransLifeCount`).
> Chỗ gọi của chúng để tiện tra: `task/tollgate/killer/kill_level.lua:69`, `task/tollgate/killer/kill_level.lua:37`, và 5 chỗ của `ST_IsTransLife` (`activitysys/playerfunlib.lua:146`, `:157`; `event/birthday_jieri/200905/class.lua:59`, `:63`; `event/change_destiny/head.lua:114`).

---

## 7. QUÉT NGƯỢC — CÓ TÊN NÀO BỊ `04_api_gap.md` BỎ QUA KHÔNG?

Tôi quét lại bao đóng bằng bộ quét riêng, bắt **3 dạng lời gọi**: gọi trần `Ten(`, gọi phương thức `a:b(` / `a.b(`, và **upvalue Lua 4 `%ten(`**.

Kết quả: **855** tên gọi trần, **592** tên gọi phương thức, **3** upvalue.

### 7.1 Nhóm A/B — không có tên engine nào bị bỏ sót
Lọc: tên **có trong luamap**, **JX1 chưa đăng ký**, **không bị hàm script trong bao đóng che** ⇒ đúng **42 tên của nhóm B + `JoinMission`**, **không thừa một tên nào**. Đây là kiểm chứng độc lập mạnh nhất cho kết luận của vòng 1.

Ngược lại, có **14 tên engine bị `04_api_gap.md` bỏ QUA HOÀN TOÀN** (không nằm cả trong 235 của nhóm A lẫn 42 của nhóm B): `AddGoldItem`, `AddItem`, `GetAccount`, `GetExp`, `GetItemName`, `GetLife`, `GetName`, `GetSeries`, `GetTask`, `Include`, `RunMission`, `Say`, `SetTask`, `Talk`.
Lý do: bộ lọc 3 của vòng 1 loại chúng vì cây Linux **cũng** định nghĩa hàm cùng tên ở đâu đó. **Không phải khe hở** — tôi kiểm: **14/14 JX1 đều đã đăng ký** (`AddItem` `:14413`, `GetTask` `:14361`, `SetTask` `:14362`, `Say` `:14259`, `Talk` `:14266`, `Include` `:14275`, `GetName` `:14633`, `GetAccount` `:14634`, `GetExp` `:14612`, `GetLife` `:14622`, `GetSeries` `:14631`, `GetItemName` `:14426`, `RunMission` `:14747`, `AddGoldItem` `:14420`). **Nhưng con số "280 hàm engine bị gọi" của vòng 1 là con số ĐÃ TRỪ, không phải tổng thật** — nên ghi lại để lần sau không hiểu nhầm.

Điểm đáng lưu ý riêng: **`Include`** bị `lib/include.lua:4` định nghĩa lại bằng Lua thuần (`function Include(path) … dofile(realpath) end`), nhưng tệp đó **KHÔNG nằm trong bao đóng nào** ⇒ ba tính năng dùng **`Include` của engine**. JX1 có `LuaIncludeFile` (`:14275`). ⇒ **Khi port, TUYỆT ĐỐI không kéo `lib/include.lua` vào**, kẻo hàm Lua đè lên và `dofile` sai đường dẫn.

### 7.2 Gọi qua `%tên` (upvalue Lua 4) — 3 chỗ, không có khe hở
| Chỗ gọi | Tên | Kết luận |
|---|---|---|
| `activitysys/activitydetail.lua:106`, `:108` | `%_pack_` | biến cục bộ của chính tệp — không phải hàm engine |
| `lib/file.lua:57` | `%GetTabFileHeight` | đúng như vòng 1 đã biết: hàm **script** định nghĩa tại `lib/file.lua:38/46` |
| `task/metempsychosis/translife_6.lua:264` | `%CreateNewSayEx` | hàm script, có trong bao đóng |

### 7.3 Gọi qua bảng `a:b(` — kiểm "bảng gốc có nằm trong bao đóng không"
Đây là lớp rủi ro **vòng 1 chưa hề xét**. Tôi lọc 592 lời gọi phương thức, tìm bảng nhận mà **nơi tạo bảng không nằm trong bao đóng của tính năng gọi nó**. Sau khi loại các dương tính giả (biến cục bộ `self`/`v`/`tb`/`pData`, tham số hàm như `tbPattan` ở `lib/pfunction.lua:56`, lệch **HOA/thường tên tệp** như `vng_lib/VngTransLog.lua` ↔ `vng_lib/vngtranslog.lua`, và bảng khai báo xuống dòng như `ChuangGuan30 =` ở `missions/challengeoftime/chuangguang30.lua:12`), còn lại **đúng MỘT trường hợp thật** — mục 7.4.

### 7.4 🔴 BỎ SÓT MỚI: `tbTimerLog:weiMing(...)` — bảng KHÔNG TỒN TẠI ở bất kỳ cây nào

```
missions/challengeoftime/award.lua:164-167
    function award_to_player_success()
        tbAwardTemplet:GiveAwardByList(tbExtItem, "finish challengeoftime")
        tbTimerLog:weiMing(tbExtItem[1].nPrestige)          <<< dòng 166
    end
```

- `tbTimerLog` **chỉ xuất hiện đúng 1 lần** trong `D:\ServerLinux\server1\script` (5.059 tệp `.lua` — đối chất đếm lại: khớp), **0 lần** trong `vng_script` (171 tệp `.lua` — khớp) và **0 lần** trong `D:\ServerLinux\Patch\script` (634 tệp — khớp). Không có `tbTimerLog = …` ở đâu cả. *[đối chất bổ sung]* Quét lại bằng `grep -rI` trên **toàn bộ cây `D:\ServerLinux`**, gồm cả hai gốc mà bài chưa kê — `gateway\s3relay\script` (104 tệp `.lua`) và `Patch\vng_script` (1 tệp) — vẫn **đúng 1 kết quả duy nhất**, chính dòng này.
- Phương thức `weiMing` cũng **chỉ xuất hiện ở đúng dòng này**.
- **Không có guard** (`if tbTimerLog then`).
- `missions/challengeoftime/award.lua` **nằm trong bao đóng của `vuotai`** (kiểm bằng `closure3.json`) và là **tệp lõi** của tính năng.

> **Hậu quả**: `award_to_player_success()` phát thưởng ở dòng 165 rồi **văng lỗi** ở dòng 166 (`attempt to index global 'tbTimerLog' (a nil value)`). Người chơi **vẫn nhận thưởng** nhưng mọi việc sau lời gọi này (và trong cùng chuỗi gọi) **không chạy**.
> **Cùng loại với `JudgePLAddTitle`** mà vòng 1 đã tìm ra ở `battles/battlehead.lua:781/783` — nhưng nguy hiểm hơn vì nằm ở **tệp lõi Vượt Ải**, không phải thư viện dùng chung.
> **Việc phải làm khi port**: bỏ dòng 166, hoặc bọc `if tbTimerLog then … end`, hoặc viết bảng ghi log uy danh thật.
> ⇒ **Nhóm D nay có 4 mục**, không phải 3: `ContriValueEntryLogic`, `JudgePLAddTitle`, `Require`, **`tbTimerLog:weiMing`**.

### 7.5 Tên bị che bởi hàm script định nghĩa NGOÀI bao đóng (mục 5.1 của vòng 1)
Vòng 1 báo **35** tên, phản biện vòng 1 báo **17**; tôi đếm độc lập được **16** tên (bảng dưới). Chênh lệch là do quy tắc đếm khác nhau (vòng 1 đếm cả tên được che nhưng không được gọi). **Kết luận cuối cùng thì trùng**: trong 16 tên đó, chỉ **`JoinMission`** vừa có trong luamap vừa **JX1 chưa đăng ký**.

16 tên: `AddGoldItem`, `AddItem`, `GetAccount`, `GetExp`, `GetItemName`, `GetLife`, `GetName`, `GetSeries`, `GetTask`, `Include`, `JoinMission`, `OutputMsg`, `Say`, `SetTask`, `Talk`, `login_add`.
- 14 tên đầu (trừ `JoinMission` và `login_add`): **JX1 đã có** — xem 7.1. `OutputMsg` cũng đã có (`:14879 -> LuaJX2_OutputMsg`).
- `login_add` (`global/login_head.lua:7`) — **không** trong luamap, **không** ở JX1, nhưng chỗ gọi được bọc `if login_add then … end` ⇒ **không phải khe hở** (xác nhận lại kết luận của vòng 1).

### 7.6 Tên gọi trần không thuộc nhóm nào và không được định nghĩa ở đâu
Lọc: gọi trần + không phải từ khoá/thư viện chuẩn Lua 4 + không có trong `04_api_gap.json` + không có định nghĩa toàn cục nào trong cây Linux ⇒ **7 tên**, và **cả 7 đều là biến cục bộ / tham số**, tức **dương tính giả** đúng như bộ lọc 2 của vòng 1 dự đoán:
`fnCallback` (`task/tollgate/killer/lib_killlevel.lua:40`, `:59`), `id` / `name` / `proceed` / `series` (`missions/challengeoftime/npc.lua:313/330/367/349`), `org` (`lib/basic.lua:117`, `:145`), `value` (`lib/composeex.lua:172`).
⇒ **Không có tên engine nào bị bỏ sót ở dạng này.**

---

## 8. NHỮNG GÌ TÔI CỐ BÁC NHƯNG KHÔNG BÁC ĐƯỢC (giữ nguyên kết luận vòng 1)

- **Bảng đăng ký JX1 đủ chưa?** Tôi tìm **mọi** mảng `TLua_Funcs` trong `Sources`: `Core\Src\ScriptFuns.cpp:14257` (`GameScriptFuns`), `:15325` (`WorldScriptFuns`), `Core\Src\LuaFuns.cpp:43` (`SysFuns`), `Engine\Src\LuaFuns.cpp:54` (`SysFuns` bản engine), `MultiServer\S3Relay\DoScript.cpp:272` (chỉ 4 hàm: `GlobalExecute`/`GameExecute`/`Msg2IP`/`GetIP`), `S3Client\Ui\ShortcutKey.cpp:2273` (client). Vòng 1 chỉ nêu 3 bảng đầu — **hai bảng còn lại không chứa tên nào của nhóm B**, nên kết luận vòng 1 vẫn đúng. Không có đường đăng ký động nào khác. *[đã sửa theo đối chất]* `Lua_Register` có **HAI** chỗ gọi trong `Sources`: `Engine\Src\KLuaScript.cpp:502` (trong `KLuaScript::RegisterFunctions`) **và `:476`** (trong `KLuaScript::RegisterFunction` số ít, khai báo ở `KLuaScript.h:55`). Kết luận không đổi vì `RegisterFunction` số ít **không có nơi nào gọi** (grep toàn `Sources`: chỉ có định nghĩa/khai báo ở `KLuaScript.cpp:472`, `KEicScript.cpp:133`, `KScript.cpp:87` và 3 dòng `.h`).
- **39/42 tên còn lại**: đúng là JX1 không có, kể cả sau khi dò alias theo từ khoá ngữ nghĩa.
- **`IniFile_GetData` có phải nhóm C không?** Tôi nghi vì JX1 dùng **tên định danh (handle)** làm tham số 1 (`sGetTongIniByName`, `ScriptFuns.cpp:3175`) còn bản Linux có vẻ dùng **đường dẫn tệp**. Đọc chỗ gọi thật: `lib/file.lua:7` viết `IniFile_Load(filename, filename)` và `:19` viết `IniFile_SetData(filename, …)` — **truyền cùng một chuỗi làm cả tệp lẫn handle** ⇒ quy ước JX1 tương thích. **Không phải nhóm C.**
- **Không có hàm nhóm B nào là stub rỗng** — 42/42 đều có thân thật (mục 4).
- **`GetItemQuality` mã phẩm chất `1 = hoàng kim, 4 = bạch kim`**: xác nhận lại độc lập qua hằng số của hai thunk `0x080FEF90` (`1`) và `0x080FEF70` (`4`).

---

## 9. VIỆC CÒN LẠI / CHƯA XÁC MINH SAU VÒNG 2

| Vấn đề | Trạng thái |
|---|---|
| `SetItemBindState` — JX1 lưu trạng thái khoá vào đâu | **CHƯA XÁC MINH.** `Core\Src\KItem.h` không có trường bind (`grep Bind` = 0 kết quả). Ứng viên: `KItem.h:137 InsuranceCourse` ("trạng thái khoá") + `:138 InsuranceHourCourse`. Cần đọc đường ghi/đọc của hai trường này trước khi quyết định thêm trường mới (nhớ ràng buộc **cấm nới `TRoleData`**). |
| `GetNpcAroundPlayerList` — ý nghĩa chính xác của mặc định `31` | **CHƯA XÁC MINH** (biết chắc là mặc định 31 và là mặt nạ 5 bit; chưa đọc `0x080EF9F0` để biết mặt nạ gì). Không chặn việc port vì chỗ gọi duy nhất dùng mặc định. |
| `ITEM_SetExpiredTime` — gốc thời gian JX1 | **CHƯA XÁC MINH.** `KItem.cpp:2629` cộng hằng `1451581200` vào `nExpireTime`; phải đối chiếu trước khi chép ngưỡng 20 000 000 của bản Linux. |
| `BT_GetBattleParam` `0x081C69B0` — đọc kho dữ liệu nào | **CHƯA XÁC MINH** (mới biết: `→ 1 chuỗi`, không gọi `lua_gettop`, đọc tham số 1 là số). Liên quan mật thiết tới cảnh báo nhóm C về `BT_GetGameData` (JX1 là stub trả 0). |
| `QueryWiseManForSB` `0x0811A270` — cơ chế hỏi máy chủ khác | **CHƯA XÁC MINH** (131 lệnh; chữ ký `(szCB_OK, szCB_Fail, nTimeout, szTênNhânVật) → 0 giá trị` khớp chỗ gọi `item/heart_head.lua:129/132`). |

---

## 10. TỆP KẾT QUẢ

Tệp này: `D:\GAMEDEVNEW\ReverseTools\port_3hd\15_bosung_soat_api.md`.
Kịch bản soát (thư mục nháp của phiên, không nằm trong `port_3hd`): `scan2.py`, `defs.py`, `dumpreg.py`, `holes.py`, `holes2.py`, `tabs.py`, `guards.py`, cùng kết quả `scan2.json`, `defs.json`, `jx1_reg.json`, `guards.json`.
Không sửa bất kỳ tệp nào trong `D:\GAMEDEVNEW\Sources`, `D:\GAMEDEVNEW\bin`, `E:\SourceTuanLe`, và không sửa `00_`..`09_`.

---

## ĐỐI CHẤT (tác tử độc lập)

| Mục | Nội dung |
|---|---|
| Ngày | 24/08/2026 |
| Vai trò | Kiểm chứng viên độc lập — **không** phải người viết mục 0–10 ở trên |
| Nguyên tắc | Mọi khẳng định của vòng 2 bị coi là **SAI** cho tới khi tệp gốc / mã máy chứng minh ngược lại. Chỉ đọc, không sửa `Sources`, `bin`, `E:\SourceTuanLe`, không sửa `00_`..`09_`. |
| Đã kiểm | **31 khẳng định cụ thể** (con số đếm được, đường dẫn, số dòng C++, số dòng Lua, địa chỉ ELF, và mọi chỗ kết luận “thiếu / không có / chặn tiến độ”) |
| Kết quả | **27 ĐÚNG · 4 SAI (đều là sai chi tiết dẫn nguồn, không lật kết luận nào) · 0 không xác minh được** — cộng **4 chỗ vòng 2 CÒN SÓT** |

### Bảng đối chất

| # | Khẳng định (mục trong bài) | Bằng chứng gốc tôi tự dựng lại | KẾT LUẬN | Sửa lại thành |
|---|---|---|---|---|
| 1 | §1.1 `DropRateItem` đăng ký `ScriptFuns.cpp:14410`, thân `:4315`, kiểm `nParamNum<6` ở `:4318`, `GetPlayerIndex` `:4321`, p1 `:4325`, p2..p6 `:4335-4339`, `return 0` `:4344` | Đọc thẳng `ScriptFuns.cpp`. `grep -n "^int LuaDropRateItem"` → **4315**. Dòng 14410 = `{"DropRateItem",	LuaDropRateItem},`. 8/8 số dòng khớp | **ĐÚNG** | — |
| 2 | §1.1 Thân thi hành `KNpc.cpp:8988` không phải stub; `UpdateDropRate` `:8993`; vòng `while (j < nCount)` `:9008`; `Player[nBelongIdx].m_nCurLucky` `:9021`; `nUnknow` **không được dùng** | Đọc `KNpc.cpp:8985-9030`: 4/4 số dòng khớp. `awk NR 8988..9120 | grep nUnknown` → **chỉ 1 hit, ở chữ ký hàm** | **ĐÚNG** | — |
| 3 | §1.2 `GetNpcSeries` `:14504`, thân `:7195`, thân trải `7197-7207`, thất bại `Lua_PushNumber(L,0)` `:7206`; `SetNpcSeries` `:14503` | Đọc `ScriptFuns.cpp:14500-14508` + `7190-7212`; `grep -n "^int LuaGetNpcSeries"` → **7195**. Khớp từng dòng | **ĐÚNG** | — |
| 4 | §1.3 `ST_IsTransLife 0x081C1160` đọc `byte player+0x86B8` + `setne`, chặn 1..1199, `imul 0x8788` | `re_disasm 0x081C1160`: `0x081C116D call 0x8107910` · `0x081C1179 cmp ecx,0x4ae / ja` · `0x081C1186 imul edx,edx,0x8788` · `0x081C118C cmp byte [edx+eax+0x86b8],0` · `0x081C1197 setne al` · `0x081C11A7 call 0x8232d40`. **6/6 địa chỉ lệnh khớp từng byte** | **ĐÚNG** | — |
| 5 | §1.3 `ST_GetTransLifeCount 0x081C1100` đọc **cùng** byte; JX1 ánh xạ `LuaGetPlayerReBornValue`, đăng ký `:14336`, thân `:5528`, `m_cReBorn.GetReBornValue()` `:5538` | `re_disasm 0x081C1100` → `0x081C112C movzx eax, byte [edx+eax+0x86b8]` (cùng byte). `ScriptFuns.cpp:14336` = `{"ST_GetTransLifeCount",LuaGetPlayerReBornValue},`; `grep -n "^int LuaGetPlayerReBornValue"` → **5528**; dòng 5538 đúng là `nReBornValue = Player[...].m_cReBorn.GetReBornValue();` | **ĐÚNG** | — |
| 6 | §2.1 JX1 `DropNpcMoney` `:14411`, thân `:4267`, **chỉ 2 tham số** `:4283-4284`, belonger lấy từ `GetPlayerIndex(L)` `:4269`, `SetItemBelong` `:4308`, **`return` sớm ở `:4270`** | 6/7 khớp. Riêng: `:4270` là dòng `if (nPlayerIndex <= 0)`; câu thoát thật là `Lua_PushNumber(L, 0); return 1;` ở **`:4272-4273`** (trả **1 giá trị 0**, không phải `return 0` trắng) | **SAI (chi tiết)** — kết luận “không rơi tiền” vẫn đúng | “thoát sớm ở nhánh `:4270`, `Lua_PushNumber(L,0); return 1;` `:4272-4273`” |
| 7 | §2.2 `NpcName2Replace` là **tên duy nhất có guard**; chỗ gọi `challengeoftime/npc/transfer.lua:10` | `src_utf8/vuotai/.../transfer.lua:10` = `if NpcName2Replace then szNpcName = NpcName2Replace(szNpcName) end` — đúng nguyên văn, đúng số dòng | **ĐÚNG** | — |
| 8 | §3.1 `MISSION_MATCH = 22` ở `challengeoftime/include.lua:6`; chỗ gọi `npc/dragonboat_main.lua:163` | `include.lua:6` = `MISSION_MATCH			= 22		-- 比赛mission`; `dragonboat_main.lua:163` = `JoinMission(MISSION_MATCH, 1)`. Cả hai đúng số dòng | **ĐÚNG** | — |
| 9 | §3.2 Chữ ký `JoinMission 0x08137E40`: `gettop>1`, p1 int ≥0, p2 số, SubWorld ≥0, PlayerIndex >0, khung ngăn xếp `0x751C` | `re_disasm 0x08137E40`: `sub esp,0x751c` · `0x08137E52 call 0x8232490` · `0x08137E57 cmp eax,1 / jle` · `0x08137E6B call 0x82339b0` + `js` · `0x08137E89 call 0x82338b0` · `0x08137E97 call 0x8106a40` + `js` · `0x08137EAD call 0x8107860` + `jle`. **7/7 khớp** | **ĐÚNG** | — |
| 10 | §1.3 vs §3.2 gán **cùng nhãn “GetGlobalPlayerIndex”** cho hai địa chỉ khác nhau `0x8107910` và `0x8107860` — nghi mâu thuẫn | `re_disasm 0x08107910` → `push ebp; mov ebp,esp; sub esp,8; leave; **jmp 0x8107860**` ⇒ **là thunk của chính `0x8107860`**. `0x08107860` gọi `lua_getglobal` (`0x8233620`) với chuỗi `0x8251227` | **ĐÚNG** (không mâu thuẫn) | Nên ghi rõ “`0x8107910` = thunk `jmp` tới `0x8107860`” để người sau khỏi nghi |
| 11 | §3.3 Bao đóng: `satthu` có `dragonboat_main.lua` mà **không** có `mission_match.lua`; `vuotai` có cả hai; `phonglangdo` không có tệp nào | `find src_utf8 -iname mission_match.lua` → **chỉ** `vuotai/missions/challengeoftime/mission_match.lua`. `dragonboat_main.lua` có ở `satthu` **và** `vuotai` | **ĐÚNG** | — |
| 12 | §3.3 JX1 cấp **một `Lua_State` mỗi tệp** ⇒ hàm script không che được hàm engine khác tệp — `KSortScript.cpp:151-152`, ghi chú `ScriptFuns.cpp:2182-2185` | `KSortScript.cpp:151` = `g_ScriptSet[nCurrentScriptNum].Init();`, `:152` = `…RegisterFunctions(GameScriptFuns, g_GetGameScriptFunNum());` — đúng số dòng, và **đăng ký TRƯỚC `Load()` (`:161`)**. `ScriptFuns.cpp:2182-2185` đúng là khối ghi chú ngay trên `LuaCurPack` (`:2186`). **Kiểm bổ sung tôi tự làm**: `Include` của JX1 (`LuaIncludeFile`, thân `:1969`) nạp vào **CÙNG** `L`, nên lập luận chỉ đứng vững nếu không tệp bao đóng nào `Include` `mission_match.lua` — `grep -rn "mission_match" src_utf8/vuotai` → **0 kết quả** ⇒ lập luận đứng vững | **ĐÚNG** (và mạnh hơn vòng 2 tưởng) | Nên thêm câu “không tệp nào `Include` `mission_match.lua`” làm bằng chứng chốt |
| 13 | §3.4 Cây JX1 đang chạy đã có `function JoinMission(RoleIndex, Camp)` ở `…\bin\server\script\missions\tongwar\match\mission.lua:163` | Đọc trực tiếp (chỉ đọc) `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\missions\tongwar\match\mission.lua` — dòng **163** đúng là `function JoinMission(RoleIndex, Camp)` | **ĐÚNG** | Thân hàm ở bài viết gộp `if … then return end` một dòng, tệp thật tách 3 dòng (`164-168`) — chỉ là diễn giải |
| 14 | §4(a) 39 tên còn lại (và cả 42+`JoinMission`) **0 kết quả thật** trong `D:\GAMEDEVNEW\Sources` | Tôi chạy lại độc lập `grep -rIl -w` trên `*.cpp *.h` cho **cả 43 tên** → **0 hit / 43 tên** | **ĐÚNG** | — |
| 15 | §4 `TrimString`: bộ đệm chuỗi dùng chung của JX1 ở `ScriptFuns.cpp:13778-13781` | `sed -n 13778,13781p` → 4 dòng **`extern int LuaPushString(Lua_State* L);`** … tức **khai báo**, không phải bảng đăng ký. Đăng ký thật: `grep -n '"PushString"'` → **`:15217-15220`** | **SAI (dẫn nhầm dòng)** | `ScriptFuns.cpp:15217-15220` (13778-13781 chỉ là khai báo `extern`) |
| 16 | §4/§9 `ITEM_*ExpiredTime`: “KHÔNG CÓ vỏ, nhưng **trường** đã có `KItem.h:101 nExpireTime`, `:87 nParam`, `:67 nExpirePoint`, `:137 InsuranceCourse`, `:138`; `KItem.cpp:2627` so `> KSG_GetCurSec()`, `:2629` cộng `1451581200`; `grep Bind` = 0” | Đọc `KItem.h` 65-69/85-89/99-103/135-140 + `KItem.cpp:2620-2635`: **8/8 số dòng khớp**, `grep -i bind KItem.h` = **0**. Kiểm thêm: dòng 101 nằm **trong** `typedef struct … } KItemCommonAttrib;` (`:45-107`) nên `m_CommonAttrib.nExpireTime` ở `:2627` đúng là trường đó | **ĐÚNG** | (nhưng xem “Bỏ sót #3”: JX1 **đã có sẵn accessor**, không chỉ có trường) |
| 17 | §4 `IniFile_*`: `KTongIniEntry` + `s_mapTongIniFiles` `ScriptFuns.cpp:3172-3173`, `sGetTongIniByName` `:3175`, thân `:3190/3214/3232`, đăng ký `:15288-15290`; `KIniFile::Save` `Engine\Src\KIniFile.h:49`, `WriteString :119`, `WriteInteger :124` | Đọc cả 3 vùng: **10/10 số dòng khớp** (`grep -n "^int LuaIniFile_Load"` → 3190) | **ĐÚNG** | — |
| 18 | §4 `GetNpcAroundPlayerList` ≠ `GetNpcAroundNpcList` nhưng JX1 “đẩy **bảng rỗng** trước mọi kiểm tra” (`KJx2WarInfra.cpp:353`), thân `:349`, xử lý toạ độ region `:550-559` | `KJx2WarInfra.cpp:349` = `int LuaGetNpcAroundNpcList(Lua_State* L)`, `:353` = `Lua_NewTable(L);` **đứng trước** mọi `if` ⇒ đúng. `:550-559` là phần quy đổi toạ độ **của `LuaGetAroundNpcList` (thân `:532`)** — hàm thứ hai cùng ô bảng; bản thân `LuaGetNpcAroundNpcList` cũng có đoạn tương đương ở **`:367-374`** (ghi chú + guard `m_RegionIndex` + `nMX`/`nMY`) và **`:387-388`** (`dx`/`dy` quy về toàn cục) | **ĐÚNG** (số dòng thuộc hàm ứng viên thứ 2, không sai) | Ghi thêm “tương đương trong `LuaGetNpcAroundNpcList` = `:367-374`, `:387-388`” |
| 19 | §5.2 Tham số 3 **tuỳ chọn, mặc định 31**; phần tử đẩy là `[obj+0xC0]`; **nhánh lỗi đẩy `nil` + `0`** | `re_disasm`: `0x081048F2 cmp esi,2` · `0x081048F5 mov edx,0x1f` · `0x081048FA jg 0x81049e8` · `0x08104921 mov [esp+0x14],edx` cạnh `[esp+0x18]=[esp+0x1c]=0xffffffff` · `0x0810494F call 0x80ef9f0` · `0x08104968 fild [eax+0xc0]` · `0x0810498C call 0x8233360` với `[esp+4]=0xfffffffe` (=−2) · `0x081049C3 call 0x8232e70` (pushnil) rồi `fldz` + push ⇒ trả `nil,0`. **8/8 khớp** | **ĐÚNG** | — |
| 20 | §5.2 Chỗ gọi duy nhất chỉ truyền 2 tham số: `missions/boss/bigboss.lua:218` | `bigboss.lua:218` = `local tbRoundPlayer, nCount = GetNpcAroundPlayerList(nNpcIndex, 20);` | **ĐÚNG** | — |
| 21 | §5.3 `OpenProgressBar`: bắt buộc >5 tham số (`0x081082EA`), `cmp esi,6 / jne 0x81084e8` (`0x08108383`), nhánh 6 lấy id script hiện tại (`0x08108392 call 0x8220780`), nhánh 7 `lua_tostring idx 7` (`0x081084E8`), chuỗi rỗng bỏ qua (`0x08108500`), đổi tên→id (`0x0810850C call 0x821de70`); khuôn JX1 `LuaOpenTimeBox` thân `:9675`, `:9691-9692`, `:9699` | `re_disasm` 4 vùng: **6/6 địa chỉ khớp**. `grep -n "^int LuaOpenTimeBox"` → **9675**; `:9691-9692` đúng là `char* szScript = …(L,4); Player[…].m_dwTimeBoxId = g_FileName2Id(szScript);`; `:9699` đúng là nhánh `m_ActionScriptID` | **ĐÚNG** | Thiếu 1 nhánh: `0x081084F8 test eax,eax / je` ⇒ tham số 7 **nil** cũng rơi về nhánh 6 (đã bổ sung vào thân bài) |
| 22 | §5.3 Chỗ gọi duy nhất truyền **6** tham số: `lib/progressbar.lua:95` | `progressbar.lua:95` = `OpenProgressBar(szTitle, nFrame, nEvent, bDesc, "tbProgressBar_OnTime", "tbProgressBar_OnBreak")` — đếm được **6** | **ĐÚNG** | — |
| 23 | §5.4 `PET_*`: Grown `+0x873C`, Tame `+0x8740`, Upgrade `+0x8730`; cả 3 setter `add esi,0x8704` rồi `call 0x081D4D10`; `PET_GetUpgradePoint` trả **−1** (`fld1/fchs`) | `re_disasm` 6 hàm: `0x081D600C fild [edx+eax+0x873c]` · `0x081D628C mov [esi+0x873c],eax` + `0x081D6292 add esi,0x8704` · `0x081D5FBC …+0x8740` · `0x081D5F6C mov [esi+0x8740]` + `0x081D5F72 add esi,0x8704` · `0x081D6194 …+0x8730` · `0x081D7155 mov [esi+0x8730],eax` + `0x081D715B add esi,0x8704` + `0x081D7164 call 0x81d4d10` · `0x081D616D fld1 / 0x081D616F fchs`. **11/11 địa chỉ khớp** | **ĐÚNG** (kể cả kết luận “`+0x8738` của vòng 1 là SAI”) | — |
| 24 | §5.5 `GetGlodEqIndex`/`GetPlatinaEquipIndex` = một helper `0x080FEEB0`, trả **`KItem[+0x80] + 1`**; hằng phẩm chất `1` / `4`; `lib/log.lua:39/:44` viết `… - 1` | `re_disasm 0x080FEEB0`: `0x080FEEBD gettop/jle` · `0x080FEED1 tonumber(1)` · `0x080FEEF3 cmp eax,[0x830ca5c]/jge` · `0x080FEEFB imul 0x368 + [0x830d300]` · `0x080FEF0A cmp edx,[eax+4]` · **`0x080FEF48 mov eax,[eax+0x80]` / `0x080FEF51 add eax,1`**. Thunk `0x080FEF70` đẩy hằng **4**, `0x080FEF90` đẩy hằng **1** — khớp luamap. `log.lua:39` = `GetGlodEqIndex(nItemIdx) - 1;`, `:44` = `GetPlatinaEquipIndex(nItemIdx) - 1;` | **ĐÚNG** | — |
| 25 | §5.5 `GetFirst/GetNextPlayerAtServer` = thunk gọi `0x080C3EE0`/`0x080C3F10` trên đối tượng `0x08BAF320`; mảng `+0x3C`, đếm `+0x44`, con trỏ duyệt `+0x48`, bước **8 B**, giá trị `[phần tử+4]`; `0x08BAF320` lệch `+0x4C0` so với gốc mảng người chơi `0x08BAEE60` | `re_disasm 0x08101CF0` → `mov [esp],0x8baf320 ; call 0x80c3ee0`; `0x08101D20` → `call 0x80c3f10`. `0x080C3EE0`: `[edx+0x44]` đếm, `[edx+0x3c]` mảng, `[eax+4]` giá trị, ghi `[edx+0x48]`. `0x080C3F10`: `[edx+0x48]` → `cmp` với `[edx+0x44]` → `mov eax,[eax+ecx*8+4]`. **Khớp 100%.** `0x08BAEE60` xác nhận độc lập từ `ST_IsTransLife` (`mov eax,[0x8baee60]`); hiệu = **0x4C0** | **ĐÚNG** (cảnh báo “con trỏ duyệt TOÀN CỤC” là thật) | Ghi chú kỹ thuật: đây là **danh sách móc xích** (giá trị vừa trả **chính là chỉ số** cho bước sau), không phải con trỏ chạy trên mảng đóng gói |
| 26 | §5.5 `AddStatData` chỉ nhận **đúng 1 hoặc 2** tham số, `≥3` là **no-op im lặng**; `0x080FF571 xor eax,eax; ret` | Ràng buộc **ĐÚNG**: `0x080FF562 call gettop` · `0x080FF569 lea eax,[eax-1]` · `0x080FF56C cmp eax,1` · `0x080FF56F jbe`. Nhưng **`0x080FF571` là `mov ebx,[ebp-8]`**; `xor eax,eax` ở **`0x080FF574`**, `ret` ở **`0x080FF57C`** | **SAI (địa chỉ lệch 3 byte)** — ràng buộc vẫn đúng | `0x080FF574 xor eax,eax` … `0x080FF57C ret` |
| 27 | §6 Cột “chỗ gọi THẬT, đã khử trùng lặp” của **40 dòng** bảng | Tôi dựng lại **hoàn toàn độc lập** (kịch bản riêng, quét 3 cây `src_utf8`, bỏ phần sau `--`, regex biên từ): **43/43 tên khớp từng `tệp:dòng` và từng nhãn tính năng**, kể cả 11 chỗ `AddStatData`, 5 chỗ `ITEM_SetExpiredTime`, 2 chỗ `BT_GetBattleParam`, 3 dòng `PET_*` (390/391/392), 5 chỗ `ST_IsTransLife` ở cước chú | **ĐÚNG** | Riêng `GetRoomItems`: tôi ra 4 hit `lib/composeex.lua:76/180/191/252` nhưng đọc lại thì `:76`/`:252` là `self:GetRoomItems`, `:180` là **định nghĩa phương thức** ⇒ chỗ gọi engine đúng là **`:191`** như bài viết. **Không phải lỗi của bài.** |
| 28 | §7.1 14 tên bị bộ lọc 3 của vòng 1 bỏ qua đều **đã có ở JX1**, kèm 14 số dòng đăng ký | In từng dòng: `14413 AddItem` · `14361 GetTask` · `14362 SetTask` · `14259 Say` · `14266 Talk` · `14275 Include` · `14633 GetName` · `14634 GetAccount` · `14612 GetExp` · `14622 GetLife` · `14631 GetSeries` · `14426 GetItemName` · `14747 RunMission` · `14420 AddGoldItem` · (+ `14879 OutputMsg` của §7.5). **15/15 khớp** | **ĐÚNG** | — |
| 29 | §7.1 `lib/include.lua:4` định nghĩa lại `Include`, và tệp đó **không nằm trong bao đóng nào** | `gbktool read lib/include.lua` → dòng **4** = `function Include(path)`. `ls src_utf8/*/lib/include.lua` → **không có tệp** ⇒ đúng là ngoài bao đóng | **ĐÚNG** | — |
| 30 | §7.4 `tbTimerLog:weiMing` ở `challengeoftime/award.lua:166`, **không guard**, `tbTimerLog` không được định nghĩa ở đâu, và `Patch` **không** đè `award.lua` | `award.lua:164-166` đúng nguyên văn. `grep -rI "tbTimerLog"` **toàn bộ `D:\ServerLinux`** (cả `server1`, `Patch`, `gateway\s3relay`) → **đúng 1 kết quả**; `grep -o "weiMing"` → **1**. `find Patch/script -ipath "*challengeoftime*"` → **rỗng** ⇒ không bị đè | **ĐÚNG** — đây là phát hiện thật của vòng 2 | Ba con số đếm của bài **đều khớp** khi đếm `.lua`: `server1\script` = **5.059**, `vng_script` = **171**, `Patch\script` = **634**. Chỉ thiếu: chưa kê `gateway\s3relay\script` (104) và `Patch\vng_script` (1) — tôi đã quét, vẫn 1/1 |
| 31 | §8 “Không có đường đăng ký động nào khác — `Lua_Register` **chỉ** được gọi từ `KLuaScript::RegisterFunctions` `KLuaScript.cpp:502`”; 6 bảng `TLua_Funcs` | 6 bảng **ĐÚNG 6/6** (`ScriptFuns.cpp:14257`, `:15325`, `Core\LuaFuns.cpp:43`, `Engine\LuaFuns.cpp:54`, `S3Relay\DoScript.cpp:272`, `ShortcutKey.cpp:2273`). Nhưng `grep -rn "Lua_Register"` → **HAI** chỗ gọi: `:502` **và `:476`** (trong `KLuaScript::RegisterFunction` số ít) | **SAI (chữ “chỉ”)** — kết luận vẫn đúng | “`Lua_Register` có 2 chỗ gọi (`:502`, `:476`); `RegisterFunction` số ít **không có nơi nào gọi** nên không mở thêm đường đăng ký” |

### Tổng kết đối chất

- **31 khẳng định kiểm — 27 ĐÚNG, 4 SAI (#6, #15, #26, #31), 0 không xác minh được.**
- **Cả 4 lỗi đều là lỗi DẪN NGUỒN (số dòng / địa chỉ / chữ “chỉ”), không lỗi nào lật kết luận.** Con số **B = 40, viết mới = 38** đứng vững; phân bố `satthu 26 / phonglangdo 27 / vuotai 39` cộng đúng (`28−3+1=26`, `28−1=27`, `39−1+1=39`), và bảng §6 đếm được **đúng 40 dòng**.
- Phần **dịch ngược ELF của vòng 2 rất chắc**: hơn **55 địa chỉ lệnh** tôi kiểm lại khớp từng byte, chỉ **một** mục lệch (`0x080FF571`). Cả **22 địa chỉ hàm** bài trích dẫn đều khớp `jx_linux_y.luamap.full.txt`.
- Phần **liệt kê chỗ gọi (§6) chính xác tuyệt đối**: dựng lại độc lập 43 tên trên 3 bao đóng → khớp 100%.

---

## Bỏ sót của chính vòng 2

### BS-1 🔴 `Include` trỏ tới **tệp KHÔNG TỒN TẠI** ngay trong tệp lõi Phong Lăng Độ — nhóm D phải là **5 mục**

```
missions/fengling_ferry/boss.lua:11
    Include("\script\global\路人_礼官.lua")
```

- Tệp `script\global\路人_礼官.lua` (byte GBK `C2 B7 C8 CB 5F C0 F1 B9 D9`) **không tồn tại ở bất kỳ gốc nào**: `server1\script\global` (136 tệp, **0 tên ngoài ASCII**), `Patch\script\global` (2 tệp), `gateway\s3relay\script` (không có thư mục `global`). Quét toàn `D:\ServerLinux` theo **byte** (6.366 tệp `.lua`, so byte thô để tránh bẫy mojibake CP1258): **0 tệp** chứa byte `C2 B7 C8 CB`.
- `closure3.json` của vòng 1 **đã ghi nhận** mục này là `{"rel": "global/Â·ÈË_Àñ¹Ù.lua", "linux": false, "size": 0}` — nhưng **§7 của vòng 2 không hề nhắc lại**, dù §7 tự nhận là “quét ngược — không bỏ sót”.
- Đây **cùng loại** với `tbTimerLog` (§7.4) và **nguy hiểm tương đương**: `boss.lua` là tệp lõi của Phong Lăng Độ, `Include` chạy ở **thời điểm nạp tệp**, không có guard.
- ⇒ **Nhóm D = 5 mục**: `ContriValueEntryLogic`, `JudgePLAddTitle`, `Require`, `tbTimerLog:weiMing`, **`Include global\路人_礼官.lua`**.
- Việc phải làm khi port: bỏ dòng 11 của `boss.lua`, hoặc dựng tệp `路人_礼官.lua` rỗng/hợp lệ trước khi nạp.

### BS-2 🔴 Vòng 2 **không hề xét lớp `Patch`** — 5 tệp bao đóng có bản đè, 2 tệp khác nội dung

Bối cảnh “HAI GỐC DỮ LIỆU” là điều vòng 1 chưa biết; **vòng 2 cũng không dùng**. Đối chiếu 137 tệp bao đóng (khử trùng 3 tính năng) với `D:\ServerLinux\Patch\script`:

| Tệp bao đóng | `server1` | `Patch` | Khác nội dung? |
|---|---|---|---|
| `tong/log.lua` | 1.651 B | 1.419 B | **CÓ** |
| `tong/tong_header.lua` | 6.722 B | 7.009 B | **CÓ** |
| `tong/contribution_entry.lua` | 7.488 B | 7.488 B | không |
| `tong/tong_award_head.lua` | 4.595 B | 4.595 B | không |
| `tong/tong_setting.lua` | 5.610 B | 5.610 B | không |

Hệ quả cụ thể đã xác minh: bản `Patch` của `tong/log.lua` **xoá hẳn** `function cTongLog:WriteTongMoneyChangeLog` (có ở `server1\script\tong\log.lua:32`), trong khi `tong/tong_mix.lua` (**không** bị đè) vẫn gọi ở `:65`, `:77`, `:157` ⇒ nếu lớp `Patch` thắng thì 3 chỗ đó là `attempt to call method 'WriteTongMoneyChangeLog' (a nil value)`.

**Tôi đã tự chạy kiểm để bảo vệ kết luận của vòng 2**: trích toàn bộ tên hàm được gọi trong cả 5 tệp bản `Patch` và so với bản `server1` ⇒ **không tệp `Patch` nào thêm tên hàm engine mới** (chỉ **bớt** đi `WriteTongMoneyChangeLog`). ⇒ **Con số 40 / 38 KHÔNG đổi.** Nhưng phương pháp của §7 (“quét ngược không bỏ sót”) chưa hoàn chỉnh, và **số dòng của 2 tệp trên có thể lệch** nếu bản port lấy theo lớp `Patch`.

### BS-3 🟡 JX1 **đã có sẵn accessor hạn dùng vật phẩm** — `ITEM_*ExpiredTime` bị đánh giá thừa công

§4 chỉ nói “trường đã có”, §6 vẫn xếp `ITEM_SetExpiredTime` là **RẤT CAO / viết mới** và đếm nó vào 38 hàm viết mới. Thực tế JX1 đã có cả **đường đọc/ghi lẫn đường đồng bộ client**:

- `Core\Src\KItem.h:344` — `void SetExpireTime(int nSec) { m_CommonAttrib.nExpireTime = nSec; };`
- `Core\Src\KItem.h:345-350` — `int GetExpireTime()` — **trả `0` nếu đã hết hạn** (`if (m_CommonAttrib.nExpireTime > KSG_GetCurSec()) return …; else return 0;`)
- `Core\Src\KItemList.cpp:4088` — `sItem.m_ExpireTime = Item[nIdx].GetExpireTime();`
- `Core\Src\KProtocolProcess.cpp:1470` — `Item[nIndex].SetExpireTime(pItemSync->m_ExpireTime);`

⇒ `ITEM_SetExpiredTime` / `ITEM_GetExpiredTime` là **vỏ Lua ~5 dòng**, không phải hàm viết mới, và đã có sẵn đường đẩy về client.
⚠️ **Bẫy phải ghi vào bản port**: **không** bọc thẳng `GetExpireTime()` cho `ITEM_GetExpiredTime`, vì nó trả `0` khi vật phẩm đã hết hạn — script Linux có thể cần giá trị thô. Phải đọc `m_CommonAttrib.nExpireTime` trực tiếp.

### BS-4 🟡 Còn **gốc script thứ BA** chưa ai kê: `D:\ServerLinux\gateway\s3relay\script` (104 tệp)

Cả vòng 1 lẫn vòng 2 chỉ kê `server1\script`, `vng_script`, `Patch\script`. Gốc thứ ba tồn tại và **chứa bản sao của tệp bao đóng** (`tong/log.lua`, `tong/tong_mix.lua`, …) ⇒ một phần script chạy ở **tiến trình relay**, không phải GameServer.

Điều này ăn thẳng vào chính §8 của vòng 2: bảng Lua của relay ở JX1 là `MultiServer\S3Relay\DoScript.cpp:272` và **chỉ có 4 hàm** (`GlobalExecute`/`GameExecute`/`Msg2IP`/`GetIP`). Nếu bất kỳ tệp bao đóng nào thật sự được nạp ở phía relay thì khe hở API **lớn hơn** bảng 40 hàm rất nhiều. **CHƯA XÁC MINH**: tệp nào trong 137 tệp bao đóng được nạp ở phía relay của bản Linux (cần đọc danh sách nạp của `s3relay`, chưa làm).

### Ghi chú phụ (không tính là bỏ sót)

- §5.3 thiếu nhánh `0x081084F8 test eax,eax / je` ⇒ tham số 7 **nil** cũng rơi về nhánh 6 tham số (đã bổ sung vào thân bài).
- §5.5 nên nói rõ bộ lặp `0x08BAF320` là **danh sách móc xích** (giá trị trả về của bước trước **chính là chỉ số** của bước sau), không phải con trỏ chạy tuần tự trên mảng đóng gói — cách hiện thực trên JX1 phải chép đúng ngữ nghĩa này chứ không chỉ chép “biến `static`”.
- §4 dòng `GetNpcAroundPlayerList`: JX1 `LuaGetNpcAroundNpcList` còn **bỏ qua NPC `do_death`/`do_revive`** (`KJx2WarInfra.cpp:385-386`), bản Linux thì bộ lọc nằm trong bộ lặp `0x080EF9F0` (chưa đọc) ⇒ khi đảo bộ lọc sang người chơi phải cân nhắc có giữ điều kiện này không. **CHƯA XÁC MINH.**
- `Patch\script\skill\` chứa **314 tệp `.lua` tên GBK** — không nằm trong bao đóng nào của 3 hoạt động, nhưng là bẫy đường dẫn cho mọi kịch bản quét sau này (phải so **byte**, không so chuỗi Unicode).
