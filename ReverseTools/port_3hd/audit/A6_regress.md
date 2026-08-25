# A6 — HỒI QUY: đợt port 3 hoạt động 25/08 có làm hỏng tính năng đang chạy không?

> Phạm vi: chỉ ĐỌC. Không sửa gì. Mọi phát hiện đều kèm `tệp:dòng` hoặc bằng chứng log/tiến trình.
> Cây sống: `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server`
> Repo: `D:\GAMEDEVNEW` (3 tệp C++ của đợt này còn **chưa commit** ⇒ `git diff` = đúng phần thêm).

## TỔNG KẾT

| Mức | Số | Mã |
|---|---|---|
| **CHẶN** | 5 | R1 R2 R3 R4 R5 |
| **NẶNG** | 6 | R6 R7 R8 R9 R10 R11 |
| **NHẸ** | 3 | R12 R13 R14 |
| Kiểm sạch (không hồi quy) | 12 mục | mục "ĐÃ KIỂM — SẠCH" ở cuối |

---

# CHẶN

## R1. Đợt này **ĐÃ LÊN MÁY CHỦ THẬT VÀ ĐANG CHẠY VỚI NGƯỜI CHƠI** — trái với bàn giao

`BANGIAO_3HOATDONG_2508.md:5` và mục 8.1 viết "DLL đã đặt cạnh bản chạy, **CHƯA SWAP — CHƯA TEST**".
Thực tế đo được:

```
E:\...\bin\server\CoreServer.dll                     18.056.192 B  25/08 01:02
E:\...\bin\server\CoreServer.dll.moi_2508_3hoatdong  18.056.192 B  25/08 01:26
md5 CoreServer.dll                     = ffcf88fa33e38c0edd160c3d59aa4ea5
md5 CoreServer.dll.moi_2508_3hoatdong  = ffcf88fa33e38c0edd160c3d59aa4ea5   <-- TRÙNG
```

`GameServer.exe` PID **34480** khởi động **25/08 01:48:51** — tức là **sau** khi `CoreServer.dll` bị ghi đè
(01:02) và **sau** khi toàn bộ script bị sửa (01:03 → 01:23, xem `find -newermt`). Máy chủ đang có người
chơi thật (`logs/script_jx2.log` 25/08 02:15:20, các nhân vật `DuongLong185`, `MaiMinh276`, `LeToan249`…).

Cơ chế nhiều khả năng: PostBuild của `.vcxproj` tự chép đè binary vận hành (bẫy "build = deploy" đã ghi
trong ký ức dự án), rồi bản `.moi_...` được sao ra **sau** đó — nên không ai nhận ra đã swap.

**Hệ quả:** mọi rủi ro R2…R12 dưới đây **không còn là giả định** — chúng đang xảy ra trên máy chủ vận hành.

---

## R2. `chuangguang30.lua` **không nạp được**, kéo chết cả `challengeoftime/npc/transfer.lua`

`script/missions/challengeoftime/chuangguang30.lua:249`
```lua
FORBITMAP_LIST[CHUANGGUAN30_MAP_ID] = 1
```
`FORBITMAP_LIST` **không tồn tại trên JX1**. Lời gọi nằm trong `ChuangGuan30:SetForbitItem()` (dòng 243) và
được chạy ngay ở thân tệp (dòng 273) ⇒ lỗi lúc **nạp script**.

Bằng chứng thật (do chính máy chủ ghi ra sau khi khởi động 01:48):
- `script/missions/challengeoftime/ScriptError.log` — `2026/08/25 01:48:55.655` ·
  `error: attempt to index global 'FORBITMAP_LIST' (a nil value)` · `main of string "?" at line 273`
- `script/missions/challengeoftime/npc/ScriptError.log` — cùng lỗi, nhưng qua đường
  `3: function 'Include' [C]` ⇒ tệp **`script/missions/challengeoftime/npc/transfer.lua:3`**
  (`Include("\\script\\missions\\challengeoftime\\chuangguang30.lua")`) cũng chết theo.

**Đã có sẵn cách vá đúng trong dự án mà đợt này không dùng:** đợt Thành Bảo 23/08 gặp y hệt và vá tại
`script/missions/tongcastle/game.lua:163-164`
```lua
-- [TONGCASTLE 23/08] heart_head JX1 khong co FORBITMAP_LIST (ban Linux :27) - tu khoi tao
FORBITMAP_LIST = FORBITMAP_LIST or {}
```

---

## R3. Vượt Ải **không bao giờ khởi động** — điểm vào trỏ vào tệp mà engine không hề nạp

`script/tinhnang/3hoatdong/hd3_driver.lua:23`
```lua
HD3_VA_TRIGGER = "\\settings\\trigger_challengeoftime.lua"
```
gọi ở `hd3_driver.lua:92` và `:123` bằng `DynamicExecute(HD3_VA_TRIGGER, "OnTrigger")`.

- `ScriptFuns.cpp:2415` — `LuaDynamicExecute` chỉ tra `g_GetScript(szLow)`; **không tự nạp**, không thấy thì
  ghi `"[WLLS] DynamicExecute: script chua nap, bo qua"` rồi `return 0` (dòng 2416-2420).
- `KSortScript.cpp:56` và `:65` — `g_IniScriptEngine` chỉ quét `"\\script"` và `"\\scriptjx2\\tong_vn"`.
  **Mọi tệp trong `\settings\` không bao giờ vào `g_ScriptSet`.**

⇒ Toàn bộ nhánh (C) Vượt Ải chết ở bước đầu tiên; `hd3_admin.lua` "báo danh NGAY" cũng vô tác dụng.

**Kèm theo:** tiền tố `"\\settings\\trigger_"` mới thêm vào `KSortScript.cpp:130` là **mã chết** — tên script
trong `g_ScriptSet` không bao giờ có chuỗi đó.

---

## R4. Thiếu dòng trong `settings/TimerTask.txt` cho **cả** PLD lẫn Vượt Ải ⇒ hai hoạt động đứng im

Id timer mà bản Linux dùng:

| Tệp:dòng | Timer id |
|---|---|
| `script/missions/fengling_ferry/mission.lua:15` | **29** |
| `script/missions/fengling_ferry/mission.lua:16` | **28** |
| `script/missions/challengeoftime/include.lua:7` `TIMER_MATCH` | **41** |
| `script/missions/challengeoftime/include.lua:8` `TIMER_BOARD` | **42** |
| `script/missions/challengeoftime/include.lua:9` `TIMER_CLOSE` | **43** |

`settings/TimerTask.txt` hiện chỉ có id `1-10, 12-18, 20, 21, 50-55, 61, 62, 65-70, 75-77`.
Đường chạy: `ScriptFuns.cpp:11448` `pTimer->SetTimer(nTimeInterval, nTimerId)` →
`KTaskFuns.cpp:127` `g_TimerTask.GetTimerTaskScript(...)` → `KTaskFuns.cpp:185` `m_TimerTaskTab.GetString(...)`
trả **chuỗi rỗng** → `KMission.cpp:75` `if (!ScriptFileName[0]) return FALSE`.

Không sập, nhưng: **đua thuyền không bao giờ công bố/cập bến, trận Vượt Ải không bao giờ chuyển từ báo danh
sang thi đấu.** Bàn giao không nhắc gì tới `TimerTask.txt`.

---

## R5. Mục Lệnh Bài Admin của đợt này **bấm vào là lỗi** (yêu cầu #5 của chủ game không dùng được)

`script/item/lenhbaiadmin.lua:107`
```lua
"Hoạt động Linux (Săn Boss Sát Thủ / Phong Lăng Độ / Vượt ải): test/HD3_AdminMenu",
```
`SayEx` tách nhãn và tên hàm bằng dấu `/` — nhãn ở đây chứa **hai dấu `/`** ⇒ tên hàm bị cắt sai.

Bằng chứng thật trong `ScriptError.log`:
```
2026/08/25 01:51:58.615  ScriptError 4:[1] Script Name: (\script\item\tasklink_goods.lua)
                          cFuncName:( Phong Lăng Độ / Vượt ải): test)   error: attempt to call a nil value
2026/08/25 01:52:05.575  ScriptError 4:[1] Script Name: (\script\item\lenhbaiadmin.lua)
                          cFuncName:( Phong Lăng Độ / Vượt ải): test)
```
(Các mục cũ dùng nhãn không có `/` — ví dụ `lenhbaiadmin.lua:100` `"Hoạt động 23-24.08 (BC-BN-TV-TB): bộ test/HD_AdminMenu"`.)

---

# NẶNG

## R6. 42 hàm engine mới **đổi hành vi của khoảng 20 đường script ĐANG CHẠY**, không phải chỉ 3 hoạt động

`git diff Sources/Core/Src/ScriptFuns.cpp` cho thấy khối `[3HD 25/08]` (`ScriptFuns.cpp:15388-15434`) đăng ký
**42 tên toàn cục mới**. Trước đợt này các tên đó là `nil`; Lua 4.0 `lua_call` là lời gọi **có bảo vệ** ⇒ gọi
hàm nil làm **hỏng nốt phần còn lại của hàm đang chạy**. Nay chúng chạy thật ⇒ hành vi đổi.

Quét toàn cây sống (loại trừ thư mục của 3 hoạt động mới) — các đường **đang chạy** bị ảnh hưởng:

**a) `SetItemBindState` → khoá đồ vĩnh viễn.**
`KJx2WarInfra.cpp:1490` `Item[nItemIdx].SetPlayerItemLock(n)`; `KItem.h:313-316` ghi `LockItem.nState`;
`GameDataDef.h:290` `LOCK_STATE_FOREVER = -2`; đường cấm bán/giao dịch `KPlayer.cpp:3755, 5155, 5237`.
Các bảng thưởng **đang chạy** đặt `nBindState = -2` (được gọi qua `scriptjx2/lib/droptemplet.lua:144`
và `scriptjx2/lib/awardtype/item.lua:114`):

| Tệp:dòng | Hệ đang chạy |
|---|---|
| `script/missions/leaguematch/item/hongyinbaoxiang.lua:4-7` | Liên Đấu (WLLS) |
| `script/missions/leaguematch/item/jindanbaoxiang.lua:5` | Liên Đấu |
| `script/missions/tongcastle/guideperson.lua:312` | Thành Bảo |
| `script/startgame/tinhnang/tongkim/songjin_shophead.lua:121` | Tống Kim |
| `script/activitysys/config/32/variables.lua:11-16` | nhiệm vụ ngày |
| `script/item/xinshirenwu/xinshibaoxiang.lua:52-53` | rương tân thủ |

⇒ **Từ 01:48 hôm nay, đồ thưởng của các hệ trên bị KHOÁ VĨNH VIỄN** (không bán / không giao dịch / không
vứt) trong khi trước đó không bị. Đây là thay đổi kinh tế, chưa ai test.

**b) `ITEM_SetExpiredTime` → đồ thưởng nay CÓ HẠN** (`KJx2WarInfra.cpp:1385`, gọi qua
`scriptjx2/lib/droptemplet.lua:130`, `scriptjx2/lib/awardtype/item.lua:101`):
`script/missions/leaguematch/item/hongyinbaoxiang.lua:4-7` (`nExpiredTime = 86400`),
`script/event/tongwar/headinfo.lua:148,152,155` (Bang Chiến, 10080 / 30 ngày / 90 ngày),
`script/missions/tongcastle/guideperson.lua:312` (10080).

**c) `ITEM_SetLeftUsageTime` → ghi đè `KItem.nParam`** ("số lần sử dụng item", `KItem.h:87`;
`KJx2WarInfra.cpp:1467`): `scriptjx2/lib/droptemplet.lua:135,140`,
`scriptjx2/lib/awardtype/item.lua:109`, `scriptjx2/lib/randomitem.lua:44`.

**d) `NpcDropMoney` → nay trả tiền thật** (`scriptjx2/lib/droptemplet.lua:159`). Trước đây lỗi ở dòng này
làm cả nhánh `nJxb` không chạy. Bảng có `nJxb` tới **10.000.000**:
`script/vng_event/thapnienlenhbai/lenhbai_def.lua:465, 475, 483, 491, 502, 509, 517, 527, 538`.

**e) Các tên còn lại vốn nil, nay chạy thật trên tệp có sẵn:**

| Tên mới | Điểm gọi CÓ SẴN trên cây sống |
|---|---|
| `SendScriptData` (**gửi gói ra client**) | `script/script_protocol/protocol_def_gs.lua:193`, `scriptjx2/lib/player.lua:206,417` |
| `OpenProgressBar` | `scriptjx2/lib/progressbar.lua:95`, `scriptjx2/lib/player.lua:626` |
| `AddStatData` | `script/startgame/tinhnang/tongkim/songjin_shophead.lua:183` (Tống Kim), `script/activitysys/config/32/talkdailytask.lua:159` |
| `Add120SkillExp` | `script/task/task_award_extend.lua:6` |
| `ST_IsTransLife` | `script/activitysys/playerfunlib.lua:146,157` |
| `Tm2Time` | `script/activitysys/functionlib.lua:364,380` |
| `GetItemQuality` / `GetGlodEqIndex` / `GetPlatinaEquipIndex` / `GetPlatinaLevel` / `GetItemGenTime` | `script/lib/log.lua:37-51`, `script/lib/composeex.lua:138-144`, `scriptjx2/lib/equipex.lua:50`, `scriptjx2/tong_vn/workshop/ws_tiangong.lua:353,378,402` |
| `GetRoomItems` | `script/lib/composeex.lua:191` |
| `ITEM_GetExpiredTime` | `script/activitysys/activity.lua:314` |
| `DropItemEx` | `script/task/tollgate/messenger_prize.lua:78` (**Tín Sứ**) |
| `BT_GetBattleParam` | (chỉ trong tệp mới chép) |

Không tệp nào ở trên thuộc 3 hoạt động mới — tất cả là đường **đang chạy**. Bàn giao mục 1 không nêu rủi ro này.

---

## R7. **Bản VIỆT Săn Boss Sát Thủ VẪN ĐANG CHẠY** — bàn giao khẳng định sai

`BANGIAO_3HOATDONG_2508.md:97`: *"Bản Việt Vượt Ải + Sát Thủ vốn đã tắt sẵn (`timerserver.lua:79-80`,
`startgame.lua:99`)"*.

Sát Thủ bản VN có **hai** điểm vào, đợt này chỉ tắt một:
- `script/startgame.lua:100` `-- addnpcbosssatthu()` — đã tắt (từ trước).
- `script/startgame.lua:202` **`addnpcsatthu()` — VẪN GỌI**, định nghĩa ở
  `script/startgame/khac/satthu.lua:5`.

`script/startgame/khac/satthu.lua` còn **20 lời gọi `AddNpcNew` chưa comment** (dòng 48, 52, 56, 60, 64, 68,
72, 76, 80, 84, 88, 92, 144, 148, 152, 156, 160, 164, 168, 172), dùng template **768, 812-820** trên map
**321, 225, 93, 75, 340**, gắn `DEATHFILEST = "\script\tinhnang\boss_satthu\death.lua"` và
`DROPFILEST = "\script\tinhnang\boss_satthu\drop.lua"`.

Song song, `hd3_driver.lua:45` `add_killertasknpc(addkillertasknpc)` sinh **160 boss** bản Linux trong cùng
dải template 761-820, gắn script chết bản Linux. ⇒ **Hai hệ boss sát thủ cùng tồn tại, hai bảng rơi đồ
khác nhau, hai bảng nhiệm vụ khác nhau.**

*CHƯA XÁC MINH:* mức trùng **bản đồ** giữa 20 boss VN và 160 boss Linux (bảng `killbosshead.lua` 384 KB
không bóc chắc chắn bằng regex; chỉ chắc map 75 xuất hiện ở cả hai).

---

## R8. 7 NPC "Nhiếp Thí Trần" và 6 thuyền phu bị đặt **SAI PHE** — tham số 7 của `AddNpcEx` trên JX1 là **camp**

`ScriptFuns.cpp:6981-6994` (chú thích ngay trên đó ở `:6910` và `:6929-6935`):
```cpp
// tham so 7: phe NPC (ban goc la mot co byte rieng - xem chu thich tren)
if (nTop >= 7 && Lua_IsNumber(L, 7)) { int nCamp = ...; Npc[nNpcIdx].SetCurrentCamp(nCamp); ... }
```

Hai chỗ gọi mới:
- `script/global/autoexec_npc_hd3.lua:22` → tham số 7 = **0** ⇒ `camp_begin` (`GameDataDef.h:486`, "chỗ trống").
- `script/tinhnang/3hoatdong/hd3_driver.lua:60` → tham số 7 = **1** ⇒ `camp_justice` (**chính phái**).

Trong khi `settings/npcs.txt` cho cả hai template này `Camp = 6` (`camp_event`):
dòng **770** = "Nhiếp Thí Trần" `Kind=3 Camp=6`; dòng **241** = "Thuyền phu Giáp" `Kind=3 Camp=6`
(quy ước: id template N ↔ dòng N+1; kiểm chéo `startgame.lua:82` `AddNpcEx1({95},…,"Thán Tức Lão Nhân")`
↔ npcs.txt dòng 96).

Khuôn JX1 luôn truyền 6: `startgame.lua:60, 69, 71, 74, 76, 81, 82` (`AddNpcEx1(…,6)`),
`startgame.lua:85-97` (`AddNpcNew(…,6,…)`).

⇒ 6 thuyền phu trên map 336 (map sống) mang phe **chính phái**: người chơi **tà phái có thể đánh NPC**.
7 NPC ở 7 thành lớn mang phe 0.

**Kèm:** ý định "chết là biến mất" (`m_bNoRevive`) cũng không thành — nhánh `ScriptFuns.cpp:6992` yêu cầu
`g_IsJx2Script(L)`, mà `hd3_driver.lua` và `autoexec_npc_hd3.lua` được `Include` vào **state của
`\script\startgame.lua`** (không khớp tiền tố nào ở `KSortScript.cpp:115-131`).

---

## R9. 160 lỗi script mỗi lượt hồi sinh boss — `kill_level.lua` không có `OnRevive`

`script/task/tollgate/killer/kill_level.lua` chỉ định nghĩa `OnDeath` (dòng 22) và `kill_level20` (dòng 34).
Engine gọi `OnRevive` trên script của NPC ⇒ nil.

Bằng chứng: `ScriptError.log` — **160 dòng** liên tiếp lúc `2026/08/25 01:49:50`:
```
ScriptError 4:[1] Script Name: (\script\task\tollgate\killer\kill_level.lua) cFuncName:(OnRevive)
error: attempt to call a nil value
```
Đúng bằng số boss mà `hd3_driver.lua:45` sinh ra (bàn giao mục 3 ghi "160 boss sát thủ").
Thống kê cả tệp log: **0 dòng trước 25/08**, 160 dòng từ 01:49:50 ⇒ hoàn toàn do đợt này.

---

## R10. `script/activitysys/config/41/extend.lua` **chết lúc nạp** — chép tệp lẻ vào hệ activitysys đang chạy

`script/activitysys/config/41/ScriptError.log`:
```
error: attempt to index global `pActivity' (a nil value)
stack traceback: 1: main of string "?" at line 11
2026/08/25 01:48:53.418  ScriptError 4:[1] (\script\activitysys\config\41\extend.lua)
```
Tệp này nằm trong `thicong/b1_manifest.txt` (dòng cuối) — tức là **b1 chép nó vào thư mục
`script/activitysys/config/41/` VỐN ĐÃ CÓ của JX1**. `activitysys` là khung hoạt động đang chạy
(`\script\activitysys\` đã nằm trong `g_IsJx2Script` từ đợt 22/08). Chép một mảnh cấu hình Linux vào
cấu hình 41 của JX1 làm tệp không nạp được.

---

## R11. Thông báo toàn server + đóng/mở mission mỗi 2 giờ, trong khi tính năng không thể chạy

`hd3_driver.lua:85-88` gọi `fenglingdu_main` theo `HD3_PLD_GIO` = `{0, 200, 400, …, 2200}`
(`script/header/cauhinh_hoatdong.lua:272` — **12 lượt/ngày**).

`script/missions/fengling_ferry/fldmap_boat1.lua:17-23`:
```lua
CloseMission(MISSIONID); OpenMission(MISSIONID); SetMissionV(MS_STATE, 1)   -- trên 3 map thuyền
...
AddGlobalCountNews(str, 3)   -- "Cuộc đua thuyền ở Phong Lăng Độ chuẩn bị khai cuộc..."
```

Vì R4 (thiếu timer 28/29) cuộc đua **không bao giờ diễn tiến** ⇒ người chơi thật nhận thông báo rỗng
12 lần/ngày, và mission 15 bị đóng/mở lại trên map 337/338/339 mỗi 2 giờ.

Tương tự Vượt Ải: `hd3_driver.lua:91-95` chạy mỗi giờ đúng phút :00 theo `HD3_VA_GIO` (24 mốc,
`cauhinh_hoatdong.lua:289-290`), phát `AddLocalCountNews("Nhiệm vụ 'Thách thức thời gian' bắt đầu báo danh…")`
trong khi R3 chặn hoàn toàn phần chạy thật.
*CHƯA XÁC MINH:* `AddLocalCountNews` gọi từ `RunTime` (không có ngữ cảnh người chơi) có thực sự phát tới
người chơi hay không.

---

# NHẸ

## R12. `DisabledUseTownP` "chuyển sang bản thật" nhưng **cờ chỉ ghi, không ai đọc**

`KJx2WarInfra.cpp:260-263` trỏ stub sang `LuaHD3_DisabledUseTownP_Real` (`:1658`).
Cờ nằm ở `s_byHD3NoTownP[MAX_PLAYER]` (`:1656`) và **chỉ được đọc bởi** `LuaHD3_GetDisabledUseTownP`
(`:1678`) / `LuaHD3_IsDisabledUseHeart` (`:1694`). Quét toàn `Sources/`: không nơi nào khác đụng tới
`s_byHD3NoTownP` — **đường dùng Hồi thành phù của engine không kiểm cờ này**.
Quét toàn cây script sống: **0 lời gọi** `GetDisabledUseTownP()`, **0 lời gọi** `IsDisabledUseHeart()`.

**Tin tốt cho hồi quy:** 12 điểm `DisabledUseTownP(1)` của các hệ đang chạy KHÔNG đổi hành vi —
`missions/bairenleitai/hundred_arena.lua:230`, `missions/bw/bwhead.lua:166`,
`missions/leaguematch/head.lua:524`, `missions/tong/collectgoods/head.lua:247,319`,
`missions/tong/tong_disciple/head.lua:210,277`, `missions/tong/tong_springfestival/head.lua:241,313`,
`missions/tongcastle/castleplayer.lua:21`, `missions/tongcastle/tongcastle.lua:996`,
`missions/tongwar/trap/tongwar_trap.lua:110`.

**Nhưng khẳng định ở `BANGIAO_3HOATDONG_2508.md:41-45`** ("nay chuyển sang bản thật … 7 lời gọi của PLD/Vượt Ải
trước đây vô tác dụng") **là SAI**: chúng vẫn vô tác dụng, chỉ khác là nay có ghi một cờ không ai đọc.

Hai điểm phụ:
- Script sống `script/missions/clearskill/head.lua:144` gọi **`IsDisabledUseTownP()`** — tên này **vẫn chưa
  được đăng ký** (bảng chỉ có `GetDisabledUseTownP`, `ScriptFuns.cpp:15415`). Đợt này động đúng chỗ mà không vá.
- `s_byHD3NoTownP` / `s_nHD3NoTownPWorld` **không được xoá khi người chơi thoát hoặc vào lại**. `PlayerIndex`
  bị dùng lại ⇒ khi nào có script đọc cờ, người chơi mới sẽ thừa cờ của người cũ.

## R13. `hd3_admin.lua` kéo 384 KB `killbosshead.lua` vào state của Lệnh Bài Admin

`script/item/lenhbaiadmin.lua:38` `Include(".../hd3_admin.lua")`; bao đóng Include của `hd3_admin.lua` gồm
`hd3_driver.lua` → `killbosshead.lua` (384.861 B) + `autoexec_npc_hd3.lua`. Mà `lenhbaiadmin.lua:46`
`dofile("script/item/lenhbaiadmin.lua")` chạy **mỗi lần dùng vật phẩm**.
*CHƯA XÁC MINH:* chi phí thực tế (Include có nạp lười trong hàm hay không tuỳ đường chạy).

## R14. `timerserver.lua` nạp thêm 2 tệp mỗi phút

`script/timerserver.lua:36` `dofile("script/timerserver.lua")` chạy mỗi phút; dòng `:32` nay thêm
`Include(".../hd3_driver.lua")` → kéo theo `cauhinh_hoatdong.lua`. Trước đợt này không có 2 tệp đó.

---

# ĐÃ KIỂM — SẠCH (không hồi quy)

1. **`g_IsJx2Script` — 5 tiền tố mới** (`KSortScript.cpp:126-130`). Quét toàn cây sống: khớp đúng **43 đường
   dẫn**, trong đó **chỉ 1 tệp đã có từ trước** là `script/vng_feature/checkinmap.lua` (không nằm trong
   `b1_manifest.txt`). Tệp này chỉ có `Include` + `GetFightState` + `CheckInMap` — **không** dùng
   `GetGameTime` / `GetTeamMember` / `AddSkillState` / `SetPunish` / `AddNpcEx(tham số 7)` ⇒ không đổi hành vi.
   43 đường còn lại đều là tệp mới chép. `\script\missions\boss\` chỉ khớp đúng `bigboss.lua` (tệp mới).
   Tiền tố `\settings\trigger_` là mã chết (xem R3).
2. **`settings/task/missions.txt`**: diff với `missions.txt.truoc_3hd_2508` = **đúng 2 dòng**
   (15 → `fengling_ferry\mission.lua`, 22 → `challengeoftime\mission_match.lua`), cả hai trước là
   `mission_trong.lua`. Không đụng slot nào khác (7/8/9 citywar, 11 bw, 12 pubg, 24-26 leaguematch,
   33 tongwar, 37-45 tong đều nguyên).
3. **`script/header/cauhinh_hoatdong.lua`**: diff với `.truoc_vongsang_2508` = **0 dòng bị xoá/sửa**, thuần
   thêm. 73 khoá, **không khoá nào trùng** ⇒ 4 hoạt động cũ (TW/BR/BW/TC) nguyên vẹn.
4. **Va chạm tên toàn cục**: dựng bao đóng `Include` của `startgame.lua` (109 tệp), `timerserver.lua`
   (33 tệp), `lenhbaiadmin.lua` (29 tệp) rồi so với bao đóng mới (`hd3_driver`, `autoexec_npc_hd3`,
   `killbosshead`, `hd3_admin`, `cauhinh_hoatdong`): **0 trùng tên hàm, 0 trùng biến toàn cục**.
5. **Băm ID script** (`g_FileName2Id`, `Engine/Src/KFilePath.cpp:442`): tính cho toàn bộ **2.959** tệp
   `.lua`/`.txt` được nạp lúc boot — **không tệp mới nào đụng ID tệp cũ**. (Còn 1 cụm trùng cũ ở
   `script\skill\special\*.lua`, đã có từ trước đợt này.)
6. **Trần bảng script**: `MAX_SCRIPT_IN_SET = 5000` (`KSortScript.h:13`); đang nạp 2.959 ⇒ còn dư ~2.000.
7. **Bảng đăng ký `GameScriptFuns[]`**: 1.053 mục, có 4 tên trùng nhưng **cả 4 đều có từ trước** đợt này
   (`BT_GetData` 14403/15270, `GetLeadLevel` 14579/14734, `GetMissionS` 14837/15330, `GetTeamMem`
   14387/14464). Khối 3HD (15388-15434) **không tạo trùng mới**.
8. **Cú pháp Lua**: chạy `syncheck.exe` (Lua 4.0 của chính engine) trên **89 tệp .lua** (toàn bộ manifest +
   các tệp bị sửa) — **tất cả "cú pháp OK"**. Các lỗi in ra chỉ là lỗi giả lập lúc chạy (thiếu hàm C).
   `IncludeLib` có đăng ký thật (`ScriptFuns.cpp:14373`, alias `IL` ở `:14379`) nên không phải vấn đề.
9. **Template NPC**: quy ước `settings/npcs.txt` là **id N ↔ dòng N+1** (kiểm chéo bằng
   `startgame.lua:82 AddNpcEx1({95},…,"Thán Tức Lão Nhân")` ↔ dòng 96 "Côn Luân Hộ pháp Thán Tức Lão Nhân").
   Theo đó: 769↔dòng 770 "Nhiếp Thí Trần" `Kind=3` ✅ · 240↔241 "Thuyền phu Giáp" `Kind=3` ✅ ·
   511↔512, 513↔514, 724↔725, 725↔726, 1692↔1693 đều là boss/quái đúng loại ·
   761-820 ↔ 762-821 "Nhiệm vụ sát thủ cấp …" ✅. **Không có nhầm ID template.**
   (Vấn đề duy nhất là **phe** khi sinh — xem R8.)
10. **Bản đồ**: 336, 337, 338, 339, 464-495 có đủ trong **cả** `Maps/WorldSet_GameServer.ini` và
    `settings/MapList.ini`. Riêng **957 có trong `MapList.ini` nhưng KHÔNG có trong
    `WorldSet_GameServer.ini`** — chế độ `chuangguan30` (`cauhinh_hoatdong.lua:302`) không có map;
    dù sao R2 đã chặn trước.
11. **Bản VN Phong Lăng Độ + Vượt Ải đã tắt thật**: `startgame.lua:102` và `timerserver.lua:82,83` đã
    comment; `sukien_vuotai` (`timerserver.lua:877`) và `sukien_phonglangdo` (`:914`) không còn ai gọi;
    NPC VN Nhiếp Thí Trần (`script/global/npcchucnang/nhieptran.lua`) không được gán cho NPC nào.
    Còn sót một lối thủ công: `script/tinhnang/phonglangdo/lib_phonglangdo.lua:256`
    `"Open Mission PLD/OpenMissionPLD"` → `:262` mở `MS_PLANGDO`(=4) trên map 337 — nhưng nằm trong thoại
    của thuyền phu VN mà thuyền phu VN không còn được sinh ⇒ không với tới được.
    (Sát Thủ VN thì **không** sạch — xem R7.)
12. **`b2_patch.py` chỉ vá tệp trong `b1_manifest.txt`** (tệp mới chép). Kiểm mã: vòng lặp remap item duyệt
    đúng manifest; 4 tệp `AddNpc→AddNpcEx` đều là tệp mới. **Không đụng tệp JX1 cũ nào.**
    Menu Lệnh Bài Admin dùng `SayEx` dạng `"nhãn/hàm"` (điều phối theo **TÊN**, không theo chỉ số) ⇒ chèn
    thêm 1 mục không làm lệch các mục cũ.

---

# ƯU TIÊN XỬ LÝ (đề xuất, không tự sửa)

1. **R1 trước hết** — xác nhận với chủ game là đợt này đã lên máy chủ thật lúc 01:48. Nếu chưa muốn phát
   hành: đổi `CoreServer.dll` về `CoreServer.dll.cu_2508_truoc_findnpc_605ec36f`, tắt
   `startgame.lua:103 HD3_DriverInit()` và `timerserver.lua:74 HD3_Tick(...)`, khôi phục
   `settings/task/missions.txt.truoc_3hd_2508`, rồi restart.
   Song song rà **R6** trên dữ liệu người chơi: đồ thưởng Liên Đấu / Bang Chiến / Thành Bảo / Tống Kim
   nhận sau 01:48 hôm nay có thể đã bị khoá vĩnh viễn hoặc gắn hạn.
2. R2 (một dòng `FORBITMAP_LIST = FORBITMAP_LIST or {}` theo khuôn `tongcastle/game.lua:164`),
   R5 (bỏ dấu `/` trong nhãn `lenhbaiadmin.lua:107`), R3, R4 — bốn thứ này chặn toàn bộ tính năng.
3. R7, R8, R9, R10, R11 trước khi mở lại cho người chơi.

---

# ĐỐI CHẤT (tác tử độc lập)

> Người viết mục này **không** phải người viết A6. Nguyên tắc: mặc định coi mọi phát hiện của A6 là **SAI**
> cho đến khi tệp gốc / binary / mã C chứng minh ngược lại. Đã kiểm chứng **15 mục** (R1–R12 + 3 mục
> "ĐÃ KIỂM — SẠCH" số 2, 9, 10). Chỉ ĐỌC, không sửa mã nguồn.
>
> Kết quả: **8 ĐÚNG · 4 THỔI PHỒNG · 1 ĐÚNG-nhưng-hạ-mức · 1 ĐÚNG-nhưng-bằng-chứng-sai · 1 SAI phần lớn**,
> và **5 chỗ A6 bỏ sót**, trong đó **1 chỗ là CHẶN gây MẤT DỮ LIỆU của hệ đang chạy**.

## Bảng đối chất

| Phát hiện | Bằng chứng gốc (kiểm lại độc lập) | KẾT LUẬN | Sửa lại thành |
|---|---|---|---|
| **R1** đã lên máy chủ thật | `md5(bin\server\CoreServer.dll)` = `md5(...moi_2508_3hoatdong)` = `ffcf88fa33e38c0edd160c3d59aa4ea5`; `CoreServer.dll` mtime **25/08 01:02:56**; `Get-Process` PID 34480 `StartTime = 25/08 01:48:51`; `$p.Modules` xác nhận nạp `E:\...\bin\server\CoreServer.dll` (đúng đường dẫn sống). Nạp sau khi ghi đè ⇒ tiến trình đang chạy **đúng** DLL mới. | **ĐÚNG** (CHẶN đúng mức) | Giữ nguyên. **Bổ sung**: `bin\client\CoreClient.dll` **cũng đã bị đè** (md5 `35fd1f5864b4764101cbe96c39f0cfde` = `.moi_2508_3hoatdong`, mtime 01:03:19) — A6 chỉ nói phía server. |
| **R2** `FORBITMAP_LIST` nil | `chuangguang30.lua:249` trong `SetForbitItem`, gọi ở thân tệp dòng **273**; `script/missions/challengeoftime/ScriptError.log` `01:48:55.655` và `.../npc/ScriptError.log` `01:48:55.949` (`3: function 'Include' [C]` → `transfer.lua:3`). Cả cây sống chỉ 2 tệp có `FORBITMAP_LIST`: chính nó và `tongcastle/game.lua`. Khuôn vá `tongcastle/game.lua:164` `FORBITMAP_LIST = FORBITMAP_LIST or {}` **tồn tại đúng như A6 mô tả**. | **ĐÚNG** (CHẶN) | Giữ nguyên. Chỉ chỉnh mô tả: tệp **không** chết hoàn toàn — các `function` khai trước dòng 273 vẫn được định nghĩa; cái chết là `Unregist/SetForbitItem/Init/RegistAll` ⇒ Vượt Ải không đăng ký gì, và `transfer.lua` đứt từ dòng 3 nên **mọi hàm khai sau dòng 3 của transfer.lua không tồn tại**. |
| **R3** trigger ở `\settings\` không bao giờ được nạp | `hd3_driver.lua:23` + `:92,:123`; `KSortScript.cpp:52-66` `g_IniScriptEngine` chỉ `LoadAllScript("\\script")` và `("\\scriptjx2\\tong_vn")`; `ScriptFuns.cpp` `LuaDynamicExecute` chỉ `g_GetScript(szLow)`, không thấy thì log `"[WLLS] DynamicExecute: script chua nap, bo qua"` rồi `return 0`. Tệp `settings/trigger_challengeoftime.lua` **có tồn tại trên đĩa** nhưng không nằm trong `g_ScriptSet`. | **ĐÚNG** (CHẶN) | Giữ nguyên. Cách vá đúng nhất: **chuyển tệp trigger vào `\script\tinhnang\3hoatdong\`** và sửa `HD3_VA_TRIGGER`; **không** nên nới `LoadAllScript` sang `\settings` (thư mục đó có `.txt`/`.ini` và 2.037 dòng npcs.txt — sẽ ăn slot `MAX_SCRIPT_IN_SET` và đổi ID băm). Hệ quả kèm ("`\\settings\\trigger_` trong `KSortScript.cpp:130` là mã chết") **ĐÚNG**. |
| **R4** thiếu timer 28/29/41/42/43 | `settings/TimerTask.txt` có đúng 36 dòng dữ liệu, danh sách id: `1-10,12-18,20,21,50-55,61,62,65-70,75-77`. **Không có 28, 29, 41, 42, 43.** | **ĐÚNG** (CHẶN) | Giữ nguyên. |
| **R5** nhãn Lệnh Bài Admin chứa `/` | Cơ chế đã truy tới tận mã C: `ScriptFuns.cpp:716` `char* pFunName = strstr(pAnswer, "/")` — lấy dấu `/` **ĐẦU TIÊN**; `KPlayer.h:636` `char m_szTaskAnswerFun[MAX_ANSWERNUM][32]` ⇒ tên hàm bị **cắt còn 31 byte**, khớp chính xác chuỗi trong log `" Phong Lăng Độ / Vượt ải): test"` (31 ký tự). `lenhbaiadmin.lua:107` đúng như A6 trích. Kèm hệ quả A6 không nêu: `*pFunName = 0` ⇒ **nhãn hiển thị cũng bị cắt** còn `"Hoạt động Linux (Săn Boss Sát Thủ "`. | **ĐÚNG** nhưng **HẠ MỨC: CHẶN → NẶNG** | Lỗi thật, sửa đúng (bỏ mọi dấu `/` trong nhãn). Nhưng chỉ ảnh hưởng **menu admin**, 0 tác động lên người chơi ⇒ không phải "CHẶN". |
| **R6** 42 hàm mới đổi hành vi ~20 đường đang chạy | Xem mục "Phản bác R6" bên dưới. Tóm tắt: **(a) khoá đồ vĩnh viễn, (b) đồ có hạn, (d) trả tiền 10 triệu — đều KHÔNG XẢY RA**; `SendScriptData` **là stub rỗng** chứ không gửi gói; `config/32/variables.lua` **là tệp MỚI của chính đợt này**, không phải "hệ đang chạy"; `script/lib/log.lua getItemInfo` **không ai gọi**. | **THỔI PHỒNG NẶNG** | Xoá (a)(b)(d) và các mục dead-code. Giữ lại — và **nâng lên NẶNG** — đúng 2 thứ: **`ST_IsTransLife`** (đổi **cổng cấp độ** đang chạy) và **`DropItemEx`** (Tín Sứ nay rơi đồ thật). |
| **R7** Sát Thủ bản VN vẫn chạy | `startgame.lua:202 addnpcsatthu()` **không comment** (dòng 100 `-- addnpcbosssatthu()` và 102 `-- addnpcphonglangdo()` mới là các dòng đã tắt). `startgame/khac/satthu.lua:5` định nghĩa hàm; 20 `AddNpcNew` còn sống (dòng 48…174), template **768, 812-820**, map **321, 225, 75, 340**, `DEATHFILEST/DROPFILEST = \script\tinhnang\boss_satthu\`. Đối chiếu `settings/npcs.txt`: id 761-820 = "Nhiệm vụ sát thủ cấp …" ⇒ **trùng đúng dải template** với 160 boss Linux. | **ĐÚNG** (NẶNG) | Giữ nguyên. Bổ sung sắc thái: đây **không phải hồi quy do đợt này gây ra** (dòng 202 vốn đã bật từ trước) mà là **trùng lặp hệ thống** do đợt này thêm hệ Linux vào — vẫn phải xử lý trước khi mở. |
| **R8** 7 NPC + 6 thuyền phu **sai phe**, tà phái đánh được | Sự thật một nửa. **ĐÚNG**: tham số 7 của `LuaAddNpcEx` (`ScriptFuns.cpp:6981-6994`) là `SetCurrentCamp`, `autoexec_npc_hd3.lua:22` truyền 0, `hd3_driver.lua:60` truyền 1, đè lên `Camp=6` của mẫu. **SAI hệ quả**: `KNpcSet::GenOneRelation` (`KNpcSet.cpp:128-130`) — **dòng KIỂM ĐẦU TIÊN** là `if (Kind1 == kind_dialoger \|\| Kind2 == kind_dialoger) return relation_dialog;` — **trước mọi phép so camp**. `npcs.txt` cho cả template 240 và 769 `Kind = 3` = `kind_dialoger` (`GameDataDef.h:1372`). ⇒ quan hệ người chơi↔NPC **luôn là `relation_dialog` bất kể camp 0/1/6**; **không ai đánh được thuyền phu**. | **THỔI PHỒNG (NẶNG → NHẸ)** | Đổi thành: "camp bị ghi đè 6→0/1 — **vô hại** vì NPC là `kind_dialoger`; nên sửa về 6 cho sạch, **không gấp**". Ý phụ về `m_bNoRevive` không áp dụng: **đó là điều ĐÚNG**, thuyền phu/NPC chức năng phải hồi sinh — A6 mô tả ngược. **Bằng chứng dòng npcs.txt của A6 lệch 1** (xem mục "sạch #9"). |
| **R9** 160 lỗi `kill_level.lua OnRevive` | Số liệu **ĐÚNG**: thống kê `bin\server\ScriptError.log` — `kill_level.lua/OnRevive` = **160 dòng, 100% ngày 25/08**. Nhưng cùng tệp log có **`turebug90.lua/OnRevive` 117 dòng ngày 24/08 + 9 dòng ngày 23/08** — tức mẫu lỗi này **đã tồn tại từ trước, ở hệ Tín Sứ**, và không ai xếp nó là NẶNG. Đường gọi `KNpc.cpp:8676 ExecuteScript(ActionScript,"OnRevive",m_Index)` là **fire-and-forget sau khi đã hồi sinh** ⇒ hỏng script không cản hồi sinh. | **THỔI PHỒNG (NẶNG → NHẸ)** | Đổi thành "nhiễu log, cùng loại với `turebug90.lua` có sẵn". Vá 1 dòng `function OnRevive() end` là đủ (và nên vá luôn cho `turebug90.lua`). |
| **R10** `activitysys/config/41/extend.lua` chết, "chép tệp lẻ vào hệ đang chạy" | Lỗi log **ĐÚNG** (`config/41/ScriptError.log` `01:48:53.418`, `pActivity` nil dòng 11). Nhưng tiền đề **SAI**: `script/activitysys/config/` **tự nó là thư mục MỚI** — mtime `25/08 00:52:09`, và chỉ chứa **3 thư mục con 32, 41, 1005 — cả ba đều mtime 00:52/01:03 của chính đợt này**. JX1 **trước đây không có `config/` nào cả**. `activity.lua` nạp cấu hình qua `TabFile_GetCell(szPath,nRow,"Id")`; quét cả cây: **0 tham chiếu tới 41/32/1005** ⇒ ba bộ này là **mồ côi**, chỉ tốn 1 dòng lỗi lúc boot. | **THỔI PHỒNG (NẶNG → NHẸ)**, tiền đề SAI | Đổi thành: "`b1_copy` kéo **3 bộ cấu hình activitysys không liên quan (32/41/1005)** vào `\script` — nơi engine tự chạy mọi `.lua` lúc boot. Không hỏng gì, nhưng nên **xoá** vì là rác + 1 lỗi boot." |
| **R11** thông báo + đóng/mở mission mỗi 2 giờ | `fldmap_boat1.lua:4-23` đúng như trích (`CloseMission/OpenMission/SetMissionV(MS_STATE,1)` trên `boatMAPS`, `AddGlobalCountNews(str,3)`); `hd3_driver.lua:85-88` + `HD3_PLD_GIO` 12 mốc; `timerserver.lua:74 HD3_Tick(nHr,nMi)` chạy mỗi phút. | **ĐÚNG** (NẶNG) | Giữ nguyên, chỉ sửa chữ: thông báo **không "rỗng"** mà **sai sự thật** — nó bảo người chơi ra Bờ Nam gặp thuyền phu (thuyền phu **có thật**, đã sinh), nhưng cuộc đua không bao giờ diễn tiến vì R4. |
| **R12** `DisabledUseTownP` ghi cờ không ai đọc | Quét `D:\GAMEDEVNEW\Sources`: `s_byHD3NoTownP` chỉ xuất hiện ở `KJx2WarInfra.cpp:1656,1668,1674,1682` — **không đường engine nào khác đọc**. Quét cây script sống: **0** lời gọi `GetDisabledUseTownP()` / `IsDisabledUseHeart()`; đúng **1** lời gọi `IsDisabledUseTownP()` ở `missions/clearskill/head.lua:144` và tên đó **không có trong `GameScriptFuns[]`**. | **ĐÚNG** (NHẸ đúng mức) | Giữ nguyên. Bổ sung: rủi ro "cờ thừa khi relog" **đã được giảm sẵn** bởi phép so `m_SubWorldIndex == s_nHD3NoTownPWorld[...]` ở `KJx2WarInfra.cpp:1682-1684` ⇒ chỉ dính khi người mới vào **đúng cùng subworld**. |
| **Sạch #2** `missions.txt` chỉ đổi 2 dòng | Đọc lại `settings/task/missions.txt`: slot **15** → `fengling_ferry\mission.lua`, slot **22** → `challengeoftime\mission_match.lua`, mọi slot khác nguyên (7/8/9 citywar, 11 bw, 12 pubg, 24-26 leaguematch, 33 tongwar, 37+ tong). Kiểm thêm phần A6 **không** kiểm: quét cả cây `OpenMission(15|22)` / `CloseMission(15|22)` = **0 hit**; các hằng `MS_ARENAID=15`, `MS_TONGWAR_J=22` là **chỉ số biến mission**, không phải id mission ⇒ không đụng. | **ĐÚNG** | Giữ nguyên (đã bổ sung phép kiểm còn thiếu). |
| **Sạch #9** "template NPC đúng hết" | **Kết luận đúng, bằng chứng SAI**. Quy ước thật là **id N ↔ dòng N+2**, không phải N+1 — vì `LuaAddNpcEx`/`LuaAddNpc` dùng `g_NpcSetting.FindRow(pName) - 2` (`ScriptFuns.cpp:6957`). Kiểm thật: dòng **771** = "Nhiếp Thí Trần" `Kind 3 Camp 6` (id 769 ✅), dòng **242** = "Thuyền phu Giáp" `Kind 3 Camp 6` (id 240 ✅), dòng **763-822** = "Nhiệm vụ sát thủ cấp …" ⇒ id **761-820** ✅, dòng 97 = "Côn Luân Hộ pháp Thán Tức Lão Nhân" (id 95 ✅). **Dòng 770 mà A6 trích thực ra là "Nhiệm vụ sát thủ cấp 90" `Kind 0 Camp 5`, dòng 241 là "Xa phu Đinh"** — A6 đọc nhầm dòng nhưng may là kết luận vẫn trùng. | **ĐÚNG (kết luận) / SAI (bằng chứng)** | Sửa câu quy ước thành **"id N ↔ dòng N+2 (`FindRow()-2`)"** và sửa 2 số dòng 770→771, 241→242. |
| **Sạch #10** map 957 thiếu trong WorldSet | `grep -c 957 Maps/WorldSet_GameServer.ini` = **0**; `settings/MapList.ini:6233-6236` có `957=…` + `957_NewWorldParam=PARTNER_OFF\|…\|USETOWNP_OFF\|…`. | **ĐÚNG** | Giữ nguyên. |

---

## Phản bác chi tiết R6 — vì sao "khoá đồ vĩnh viễn" KHÔNG xảy ra

A6 nối chuỗi *"bảng thưởng có `nBindState=-2`"* → *"`scriptjx2/lib/awardtype/item.lua:114` gọi `SetItemBindState`"* mà
**không kiểm tệp `awardtemplet` nào thực sự được nạp trên cây JX1**. Kiểm lại:

1. **Cả 5 bảng thưởng A6 nêu đều `Include("\\script\\lib\\awardtemplet.lua")`** (bản **JX1**), không phải bản `scriptjx2`:
   `leaguematch/item/hongyinbaoxiang.lua:1` · `leaguematch/item/jindanbaoxiang.lua:1` ·
   `tongcastle/guideperson.lua:9` · `startgame/tinhnang/tongkim/songjin_shophead.lua:9` ·
   `item/xinshirenwu/xinshibaoxiang.lua:1`.
2. **`script/lib/awardtemplet.lua:65-67`** tự kéo `awardtype\item_jx1.lua` + `zhenyuan_jx1.lua` + `exp_jx1.lua`.
   Thư mục `script/lib/awardtype/` **không hề có `item.lua`** — chỉ có `exp.lua, exp_jx1.lua, item_jx1.lua,
   simple.lua, zhenyuan_jx1.lua`.
3. **`script/lib/awardtype/item_jx1.lua:12-39` (`ItemJX1Type:Give`) trao đồ bằng `AddItem`/`AddItem2` và
   KHÔNG hề gọi `SetItemBindState` / `ITEM_SetExpiredTime` / `ITEM_SetLeftUsageTime`** — chú thích dòng 26 ghi
   thẳng "nExpiredTime **bỏ qua**". `tbAwardTemplet:Give` (`awardtemplet.lua:49-56`) điều phối theo **KHOÁ**
   (`tbProp`) nên luôn rơi vào `ItemJX1Type`.
   ⇒ **`nBindState = -2` và `nExpiredTime` của Liên Đấu / Thành Bảo / Tống Kim / rương tân thủ bị bỏ qua
   HÔM NAY y như hôm qua. Không có đồ nào bị khoá vĩnh viễn, không có đồ nào bị gắn hạn.**
4. **Không tệp nào trong cây sống `Include` `scriptjx2\lib\awardtemplet.lua` hay `scriptjx2\lib\droptemplet.lua`**
   (quét toàn bộ `Include*("...")` của 2 cây `script` + `scriptjx2`). Chỉ có **2** điểm `Include` `droptemplet`
   và **cả hai là tệp MỚI của đợt này**: `script/missions/boss/bigboss.lua` và
   `script/missions/fengling_ferry/boss.lua`.
   Cơ chế remap `sJX2RemapScriptPath` (`ScriptFuns.cpp:1938-1966`) **chỉ đổi `\script\lib\` → `\scriptjx2\lib\`
   KHI tệp ở `\script\lib\` KHÔNG tồn tại** — mà `script/lib/awardtemplet.lua` **có tồn tại**.
   ⇒ `droptemplet.lua:144` (`SetItemBindState`) và `:159` (`NpcDropMoney`) **chỉ chạy cho 2 boss mới**, không
   đụng `vng_event/thapnienlenhbai/lenhbai_def.lua` (tệp này `Include awardtemplet`, không `droptemplet`;
   khoá `nJxb` **không có type nào đăng ký** trên JX1 nên trước-sau đều không trả tiền).
   ⇒ **R6(d) "trả tiền tới 10.000.000" SAI.**
5. `scriptjx2/lib/randomitem.lua` — **0 điểm `Include`** trên cả cây ⇒ R6(c) dẫn `randomitem.lua:44` là mã chết.
6. **`SendScriptData` KHÔNG gửi gói**: `KJx2WarInfra.cpp:1785-1790` là **stub** `sHD3_LogOnce(...); Lua_PushNumber(L,0);`
   (cùng nhóm stub có chủ đích với `QueryWiseManForSB`, `BT_GetBattleParam`, `ST_DoTransLife`, `ST_LevelUp`, `PET_*`).
7. `script/activitysys/config/32/variables.lua` mà A6 xếp vào "nhiệm vụ ngày **đang chạy**" thực ra là **tệp MỚI
   chép ngày 25/08 01:03**, nằm trong thư mục `config/32` **mới tạo 00:52** (xem R10).
8. `script/lib/log.lua:35 getItemInfo` — quét cả cây: **chỉ có 2 nơi định nghĩa, 0 nơi gọi** ⇒ nhóm
   `GetItemQuality/GetGlodEqIndex/GetPlatinaLevel/GetPlatinaEquipIndex/GetItemGenTime` ở đó **là mã chết**.

**Phần R6 còn ĐỨNG VỮNG (và A6 lại xếp nhẹ):**

- **`ST_IsTransLife` — đổi CỔNG CẤP ĐỘ của hệ đang chạy.** `KJx2WarInfra.cpp:1762-1773` map thẳng sang
  `LuaGetPlayerReBornValue` ⇒ trả **1 thật** cho người đã chuyển sinh. Điểm gọi sống:
  `activitysys/playerfunlib.lua:146` (`PlayerFunLib:CheckTotalLevel` — `if ST_IsTransLife() == 1 or <đủ cấp>`),
  `:157` (`CheckIsTransLife`), `event/change_destiny/head.lua:114`,
  `event/birthday_jieri/200905/class.lua:59,63`. Trước đây hàm nil ⇒ **đứt hàm kiểm**; nay
  **người chuyển sinh vượt qua yêu cầu cấp độ** ở mọi hoạt động dùng `CheckTotalLevel`. → **NẶNG, cần test.**
- **`DropItemEx` — Tín Sứ nay rơi đồ thật.** `script/task/tollgate/messenger_prize.lua:77-78`: nhánh
  `prize_Quality == 1` gọi `DropItemEx(...)`. Trước: nil ⇒ đứt hàm, **không rơi gì**; nay rơi item thật
  (`LuaHD3_DropItemEx`, `KJx2WarInfra.cpp:1505+`). → **NẶNG, cần test bảng `Tollgateprize`.**
- `AddStatData` (`songjin_shophead.lua:183` trong `Jilu_jinglixiaohao`) nay ghi `log_game\hd3_statdata.log` — **NHẸ**.

---

## Bỏ sót của chính vòng soát

### M1 — 🔴 **CHẶN / MẤT DỮ LIỆU: `hd3_driver.lua` XOÁ SẠCH bảng xếp hạng Liên Đấu mỗi 00:00** (A6 không hề soát)

`hd3_driver.lua:98-100` → `if (nHHMM == HD_CFG("HD3_VA_GIO_XEPHANG", 0))` — `HD3_VA_GIO_XEPHANG = 0`
(`cauhinh_hoatdong.lua`), `nHHMM = 0` đúng **00:00**, và `HD3_Tick` chạy **mỗi phút** (`timerserver.lua:74`).
Hàm chạy tới:

```lua
-- hd3_driver.lua:104-114
function HD3_VA_DailyRank()
    local name, value = Ladder_GetLadderInfo(10235, 1)   -- :105
    ... AddGlobalCountNews(szMsg, 10); Msg2SubWorld(szMsg)
    Ladder_ClearLadder(10235)                            -- :113
end
```

**Ladder `10235` KHÔNG trống — đó là bảng "Đơn đấu tự do" của LIÊN ĐẤU (WLLS) đang chạy:**
- `script/leaguematch/head.lua:75-81` → `{ name = "Đơn đấu tự do", max_member = 1, ladder = 10235, ... }`
- `script/missions/leaguematch/macthtype/single.lua:15` → `ladder = 10235`
- (`logs/script_jx2.log 25/08 02:15:20` xác nhận Liên Đấu đang chạy: `League Match: Phase=2 SeasonID=131`)

`LuaLadder_ClearLadder` (`KJx2SharedStore.cpp:580-595`) **`s_LadderMap.erase(it); sLadderSave();`** —
**xoá hẳn và ghi đè xuống đĩa**, không hoàn tác được.

Hệ quả mỗi đêm 00:00: (1) phát **thông báo toàn server sai sự thật** — lấy quán quân Đơn đấu Liên Đấu rồi
công bố là người "hoàn thành *Thách thức thời gian*" với thời gian = `điểm × (−1)` quy ra phút/giây;
(2) **xoá sạch bảng xếp hạng Đơn đấu Liên Đấu**.

*Chưa xảy ra*: máy chủ khởi động **01:48 ngày 25/08**, mốc 00:00 kế tiếp là **26/08** ⇒ **còn kịp chặn**.

**Sửa**: Vượt Ải bản Linux dùng ladder riêng (`challengeoftime/rank_perday.lua:9 DailyRankLadderId = 10235`
là **giá trị của bản Linux**, trùng ngẫu nhiên với JX1). Phải **đổi id ladder của Vượt Ải sang một số chưa
dùng** (các id đang dùng trên JX1: 10118, 10119, 10182, 10186, 10187, 10196, 10197, 10225, 10226, 10235,
10250, 10251, 10261, 10999) và sửa **cả** `rank_perday.lua:9` lẫn `hd3_driver.lua:105,113`.
Trước mắt, cách chặn 1 dòng: bỏ `HD3_VA_DailyRank()` khỏi `HD3_Tick`.

### M2 — 🟠 A6 chỉ soát DLL **server**; `CoreClient.dll` cũng đã bị đè, `Game.exe` thì không

`md5(bin\client\CoreClient.dll)` = `md5(bin\client\CoreClient.dll.moi_2508_3hoatdong)` =
`35fd1f5864b4764101cbe96c39f0cfde`, mtime **25/08 01:03:19**; trong khi `bin\client\Game.exe` vẫn là bản
**24/08 22:27**. Đã kiểm: khối `[3HD 25/08]` trong `GameScriptFuns[]` nằm **trong `#ifdef _SERVER`**
(mở ở `ScriptFuns.cpp:14420`) ⇒ client **không** đăng ký 42 hàm mới, thay đổi thực chất chỉ là biên dịch lại.
Rủi ro thấp, **nhưng R1 phải nêu**: bản client phát cho người chơi đã đổi mà không ai ghi nhận, và dự án đã
từng dính đúng bẫy "CoreClient mới + Game.exe cũ" ngày 24/08 (Tín Sứ không hiện ở F11).

### M3 — 🟠 A6 **không soát TRÙNG TASK ID** — mà bàn giao lại khẳng định "23 id đều TRỐNG"

Quét literal `SetTask/GetTask/AddTask/SetTaskTemp/GetTaskTemp` trên tệp mới vs tệp cũ:
tệp mới dùng `200, 1082, 1192, 1193, 1217, 1505, 1550, 1551, 2641, 2642`.

- **`1550` TRÙNG**: `script/vng_feature/challengeoftime/npcNhiepThiTran.lua:67,71` dùng `GetTask(1550)` =
  *"số lần vượt ải có thể tham gia"*, trong khi `script/event/storm/function.lua:396` và `:417` (tệp JX1 cũ,
  15/08) đặt `SetTask(1550,0)` với chú thích gốc `杀手进行次数` (số lần sát thủ). Mức độ hiện tại **NHẸ**
  (chỉ công cụ GM `storm_clear` mới ghi), nhưng **bác bỏ khẳng định "tất cả TRỐNG"** của bàn giao mục 0/4.
- `200` là `SetTaskTemp` (biến tạm) và ở tệp mới đang bị comment ⇒ vô hại.
- Quét này **chỉ bắt id dạng số nguyên viết thẳng**; các id đặt qua hằng (`TIMER_*`, `TSK_*`, bảng
  `TB_STORM_TASKID`) **chưa được soát** ⇒ **cần một vòng audit task-id riêng** (dự án đã có tiền lệ
  "trùng task id = lỗi kinh tế" ngày 24/08).

### M4 — 🟡 Lỗi **phương pháp** khiến R6 sai: A6 nêu chuỗi gọi mà không truy luật remap `Include`

A6 dẫn `scriptjx2/lib/*` như thể đó là thư viện đang chạy. Thực tế `sJX2RemapScriptPath`
(`ScriptFuns.cpp:1938-1966`) **chỉ remap khi tệp `\script\lib\...` VẮNG MẶT**, và `KSortScript.cpp:52-66`
chỉ nạp sẵn `\script` + `\scriptjx2\tong_vn` — nên `scriptjx2\lib\` gần như toàn bộ là **mã chết** trừ khi
được `Include` đích danh. Mọi kết luận "hàm X nay chạy trên đường Y" **phải kèm bước xác minh Y có thực sự
được nạp/gọi**, bằng chứng tối thiểu: đếm điểm `Include` + đếm điểm gọi (như đã làm cho `getItemInfo` = 0 lời gọi).

### M5 — 🟡 3 tệp C++ của đợt này **chưa commit**

`git status` tại `D:\GAMEDEVNEW`: `M Sources/Core/Src/KJx2WarInfra.cpp`, `M .../ScriptFuns.cpp`,
`M .../KSortScript.cpp` (HEAD = `550b7d0b`). DLL dựng từ chúng **đang chạy trên máy chủ thật** (R1) mà nguồn
chưa vào git ⇒ không có mốc để lùi/đối chiếu. Trái luật dự án "sửa file nào commit + push ngay trong phiên".

---

## DANH SÁCH CHỐT sau đối chất

| # | Lỗi THẬT | Mức (sau đối chất) | Sửa |
|---|---|---|---|
| 1 | **`Ladder_ClearLadder(10235)` xoá bảng xếp hạng Đơn đấu Liên Đấu lúc 00:00 + phát thông báo sai** (M1) | **CHẶN — mất dữ liệu** | Đổi ladder id của Vượt Ải (`rank_perday.lua:9` + `hd3_driver.lua:105,113`) sang id chưa dùng; tạm thời bỏ `HD3_VA_DailyRank()` khỏi `HD3_Tick`. **Phải xong trước 00:00 ngày 26/08.** |
| 2 | **Đợt này đã lên máy chủ thật lúc 01:48, cả server lẫn client DLL** (R1 + M2) | **CHẶN (trạng thái)** | Xác nhận với chủ game. Nếu chưa muốn phát hành: lùi `CoreServer.dll` ← `.cu_2508_truoc_findnpc_605ec36f`, `CoreClient.dll` ← `.cu_2508_truoc_findnpc_8f8c001a`, tắt `startgame.lua:103` + `timerserver.lua:74`, khôi phục `missions.txt.truoc_3hd_2508`, restart. |
| 3 | **`chuangguang30.lua:249 FORBITMAP_LIST` nil** ⇒ Vượt Ải không đăng ký + `transfer.lua` đứt (R2) | **CHẶN** | Thêm `FORBITMAP_LIST = FORBITMAP_LIST or {}` đầu `ChuangGuan30:SetForbitItem()` (khuôn `tongcastle/game.lua:164`). |
| 4 | **`HD3_VA_TRIGGER` trỏ `\settings\` — engine không nạp** (R3) | **CHẶN** | Chuyển `trigger_challengeoftime.lua` vào `\script\tinhnang\3hoatdong\`, sửa `hd3_driver.lua:23`; gỡ tiền tố chết `"\\settings\\trigger_"` ở `KSortScript.cpp:130`. |
| 5 | **`TimerTask.txt` thiếu 28, 29, 41, 42, 43** (R4) | **CHẶN** | Thêm 5 dòng trỏ `fengling_ferry\mission.lua` (28/29) và `challengeoftime\include.lua` (41/42/43) theo đúng script bản Linux. |
| 6 | **`hd3_thuyenphu.lua` gọi `GetNpcValue()` không tham số** (phiên chính đã tự tìm) | **CHẶN** | `GetNpcValue(NpcIndex)`. |
| 7 | **`ST_IsTransLife` nay trả 1 thật ⇒ người chuyển sinh vượt cổng cấp độ** (R6 phần còn đúng) | **NẶNG** | Test `PlayerFunLib:CheckTotalLevel` + `change_destiny` + `birthday_jieri` trước khi mở. |
| 8 | **`DropItemEx` nay rơi đồ thật ở Tín Sứ** (`messenger_prize.lua:78`) | **NẶNG** | Kiểm bảng `Tollgateprize` cột `prize_Quality`; xác nhận vật phẩm rơi ra là thứ mong muốn. |
| 9 | **Sát Thủ bản VN (`startgame.lua:202 addnpcsatthu()`) chạy song song 160 boss Linux** (R7) | **NẶNG** | Comment `startgame.lua:202` (hoặc bỏ hệ Linux) — chọn **một** hệ. |
| 10 | **Thông báo PLD 12 lần/ngày + đóng/mở mission 15 trên map 337-339 trong khi cuộc đua không diễn tiến** (R11) | **NẶNG** | Tắt `HD3_PLD_GIO` cho tới khi lỗi 5 (timer) được vá. |
| 11 | **Nhãn Lệnh Bài Admin chứa `/`** ⇒ menu 3 hoạt động bấm vào là nil (R5) | **NẶNG** (không phải CHẶN) | Bỏ hết dấu `/` trong nhãn `lenhbaiadmin.lua:107`. |
| 12 | **Trùng task id `1550`** giữa `npcNhiepThiTran.lua` và `event/storm/function.lua` + chưa có vòng audit task-id (M3) | **NẶNG** (rủi ro) | Chạy audit task-id đầy đủ (kể cả id qua hằng số) trước khi mở. |
| 13 | **160 lỗi `kill_level.lua OnRevive`** (R9) | **NHẸ** (nhiễu log) | Thêm `function OnRevive() end`; vá luôn `turebug90.lua` (đã lỗi từ 23/08). |
| 14 | **3 bộ `activitysys/config/{32,41,1005}` mồ côi, `41/extend.lua` lỗi lúc boot** (R10) | **NHẸ** | Xoá 3 thư mục (không hệ nào tham chiếu). |
| 15 | **Camp NPC bị ghi đè 6 → 0/1** (R8) | **NHẸ** (vô hại: `kind_dialoger`) | Truyền `6` cho gọn ở `autoexec_npc_hd3.lua:22` và `hd3_driver.lua:60`. |
| 16 | **`DisabledUseTownP` ghi cờ không ai đọc; `IsDisabledUseTownP` chưa đăng ký** (R12) | **NHẸ** | Đăng ký alias `IsDisabledUseTownP` → `LuaHD3_GetDisabledUseTownP`; sửa lại khẳng định ở `BANGIAO_3HOATDONG_2508.md:41-45`. |
| 17 | **3 tệp C++ chưa commit** (M5) | **NHẸ** | Commit + push. |

**Bị loại sau đối chất (không phải lỗi):** R6(a) khoá đồ vĩnh viễn · R6(b) đồ thưởng có hạn ·
R6(c) `randomitem.lua` · R6(d) `NpcDropMoney` 10 triệu · R6(e) `SendScriptData` "gửi gói ra client" ·
R6 `script/lib/log.lua getItemInfo` · R6 `activitysys/config/32` "hệ đang chạy" ·
R8 "tà phái đánh được thuyền phu" · R8 "`m_bNoRevive` không thành" (đó là hành vi ĐÚNG).
