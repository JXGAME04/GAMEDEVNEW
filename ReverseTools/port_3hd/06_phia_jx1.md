# 06 — KIỂM KÊ PHÍA DỰ ÁN JX1: CÁI GÌ ĐÃ CÓ, CÁI GÌ PHẢI XÂY

Ngày lập: 24/08/2026. Chỉ **phân tích**, không sửa một dòng nào trong `D:\GAMEDEVNEW\Sources`,
`D:\GAMEDEVNEW\bin`, `E:\SourceTuanLe\...`.

Hai cây được kiểm:

| Ký hiệu trong tài liệu | Đường dẫn thật |
|---|---|
| **[SRC]** | `D:\GAMEDEVNEW\Sources\` (mã C++ engine JX1) |
| **[LIVE]** | `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\` (script + settings đang chạy) |
| **[REPO-JX2]** | `D:\GAMEDEVNEW\serverscript_jx2\` (kho script JX2 đã port các đợt trước) |
| **[LINUX]** | `D:\ServerLinux\server1\` (bản JX2/Kiếm Thế Linux — nguồn gốc) |

Tệp `ScriptFuns.cpp` được đọc thật hôm nay: **427.894 byte, 15.348 dòng, sửa đổi 23/08/2026 16:03:05**.  `[da sua theo phan bien]`

---

## 0. TÓM TẮT ĐIỀU HÀNH (đọc trước khi đi tiếp)

Ba phát hiện làm **thay đổi hoàn toàn ước lượng khối lượng**:

1. **JX1 ĐÃ CÓ SẴN 2 bản "made in VN" của Phong Lăng Độ và Vượt Ải**, viết bằng hệ mission JX1,
   dùng map/mission-id thật, và **đang bị TẮT** (chú thích ở `timerserver.lua:79-80`).
   Đây **không** phải bản JX2 gốc — thiết kế khác hẳn — nhưng nó chứng minh hạ tầng JX1 **đủ sức chạy**
   loại hoạt động này.
2. **Khung `activitysys` đã được port và GIỐNG BYTE-BY-BYTE với bản Linux** (6/6 tệp lõi), kể cả
   `playerfunlib.lua` và `ladderfunlib.lua`. Nhưng **chưa ai khởi động nó** và **thiếu toàn bộ
   `detailtype\` + `config\`** — tức là thiếu đúng phần chứa 3 móc nối ta cần.
3. **`RELAYLADDER` KHÔNG phải xây mới.** Nó đã được cài trong CoreServer (không phải S3Relay) và
   `IncludeLib("RELAYLADDER")` đã hợp lệ. Nhưng nó là ladder **1 máy chủ, TOP 10, lưu file cục bộ**.

Ba tính năng cũng **đã có sẵn khung nhận sự kiện**: `[LIVE] script\misc\eventsys\type\func.lua`
khai báo đủ `"KillerBoss"` (dòng 33-37), `"ChuanGuan"` (10-14), `"FengLingDu"` (22-28).

---

## 1. HỆ MISSION CỦA JX1

### 1.1 Kiến trúc lớp

| Thành phần | Giá trị / kiểu | Bằng chứng |
|---|---|---|
| Lớp mission | `class KMission` | `[SRC] Core\Src\KMission.h:124` |
| Container theo **map** | `KSubWorld::m_MissionArray`, kiểu `KMissionArray<KMission, MAX_TIMER_PERMISSION>` | `KSubWorld.h:24`, `KSubWorld.h:75` |
| **Trần mission / 1 map** | `MAX_TIMER_PERMISSION = 10` → **thực dùng 9** (chỉ số 0 vô hiệu) | `KMission.h:23`; `KLinkArrayTemplate.h:82` (`m_ulTotalSize = ulSize - 1`) |
| Container toàn cục | `g_GlobalMissionArray = KMissionArray<KMission, 50>` | `KSubWorld.h:23,25`; `KSubWorld.cpp:47` |
| **Số ô biến / mission-array** | `MAX_MISSIONARRAY_VALUE_COUNT = 100`, mỗi ô `char[16]` | `KMissionArray.h:7,13-14`; `KMission.h:126` |
| Số người / mission | `MAX_PLAYER = 1500` | `KMission.h:119`; `KPlayerDef.h:19` |
| Số NPC / mission | `MAX_NPC_MISSION = 5000` | `KMission.h:25,120` |
| Tham số / mỗi người | `MAX_MISSION_PARAM = 18` (ô 0 = `MISSION_PARAM_AVAILABLE`) | `GameDataDef.h:87-89` |
| Nhóm (group) | `unsigned char m_ucPlayerGroup` | `KMission.h:31` |
| Ladder nội bộ mission | `MISSION_STATNUM = 10` (top-10) | `GameDataDef.h:91`; `KMission.h:434` |
| Timer / 1 mission | `KMissionTimerArray` = 10 ô → **thực dùng 9** | `KMission.h:121,134` |

### 1.2 Tạo / huỷ / đăng ký script

* **`OpenMission(missionId)`** → `LuaInitMission` (`ScriptFuns.cpp:11120-11184`):
  tra bảng `g_MissionTabFile` (nạp từ `TASK_MISSION_SETTING_TABFILE`, `KCore.cpp:461`;
  tệp thật = `[LIVE] settings\task\missions.txt`) tại **dòng `missionId + 1`, cột 2** để lấy đường dẫn script;
  gọi hàm **`InitMission`** nếu script có hàm đó, **ngược lại `BeginMission`** (11160-11179).
  ⇒ script kiểu Linux (`InitMission`) chạy được **không cần sửa**.
* **`CloseMission(missionId)`** → `LuaCloseMission` (`11302-11330`): gọi `EndMission` trong script,
  rồi `StopMission()` + `m_MissionArray.Remove()`.
* **`RunMission`** → `11220-11264`; **`IsMission`**, **`GetMissionName`** → `11266`, đăng ký `14754`, `14749`.
* Bảng đăng ký Lua nằm ở **`ScriptFuns.cpp:14738-14782`** (khối `//-----------------Mission Script-----------------`).

### 1.3 API Lua của hệ mission (đã có đủ)

`GetMissionV` `GetMissionS` `SetMission` `SetMissionV` `SetMissionS`
`GetGlbMissionV` `GetGlbMissionVC` `SetGlbMissionV` `SetGlbMission`
`OpenMission` `RunMission` `CloseMission` `GetMissionName` `IsMission`
`StartMissionTimer` `StopMissionTimer` `GetMSRestTime` `GetMSIdxGroup` `GetMSLadder`
`AddMSPlayer` `DelMSPlayer` `AddMSNpc` `DelMSNpc` `SetMSGroup` `GetNextPlayer`
`PIdx2MSDIdx` `MSDIdx2PIdx` `NpcIdx2PIdx` `GetMSPlayerCount` `GetMSNpcCount`
`SetPMParam` `GetPMParam` `Msg2MSGroup` `Msg2MSAll` `Msg2MSPlayer`
`RevivalAllNpc` `DelAllNpc` `DelAllNpcName` `UpdateBattleBox`
(`ScriptFuns.cpp:14739-14775`), cộng `OpenGlbMission` / `CloseGlbMission` /
`StartGlbMSTimer` / `StopGlbMSTimer` (`15119-15121`, `14896`).

### 1.4 So với cách bản Linux dùng — **CÓ ĐỦ Ô KHÔNG?**

| Tính năng Linux | Ô số cao nhất dùng | Bằng chứng | JX1 (trần 100) |
|---|---|---|---|
| `challengeoftime` (Vượt ải) | `VARV_BATCH_MODEL` = `VARV_LEVEL + 2` = **33** (do `LIMIT_PLAYER_COUNT = 8`) | `[LINUX] missions\challengeoftime\include.lua:17,56-76` | **ĐỦ** |
| `fengling_ferry` (Phong Lăng Độ) | `MS_TIMEACC_20SEC` = **3** | `[LINUX] missions\fengling_ferry\fld_head.lua:28-30` | **ĐỦ** |
| Chuỗi `VARS_*` | 1, 2, 11, 12 | `challengeoftime\include.lua:50-54` | **ĐỦ** (xem dưới) |

**Va chạm số ↔ chuỗi đã được xử lý rồi.** JX1 gốc dùng **chung một mảng** `m_MissionValue[100][16]`
cho cả số lẫn chuỗi ⇒ `SetMissionV(1, ...)` sẽ xoá mất `SetMissionS(1, ...)`. Đợt cổng thành JX2 (E4)
đã tách: `SetMissionS`/`GetMissionS` đi **kho chuỗi riêng** `s_MissionStr`, khoá = `subworldIdx*4096 + id`
(`[SRC] Core\Src\KJx2WarInfra.cpp:1034-1070`), đăng ký **đè lên** bản cũ ở `ScriptFuns.cpp:15231-15233`.
⇒ `VARS_TEAM_NAME=1` và `VARV_NPC_BATCH=1` **không còn đụng nhau**.

### 1.5 🔴 BA KHIẾM KHUYẾT CỦA HỆ MISSION JX1 (phải biết trước khi port)

1. **Tràn bộ đệm ở `SetMissionV`.** `KMission::SetMission` (`KMission.h:334-339`) và
   `KMissionArray::SetMission` (`KMissionArray.h:36-41`) dùng **`strcpy` thẳng vào `char[16]`,
   KHÔNG kiểm độ dài**. Chuỗi > 15 ký tự **ghi đè sang ô kế tiếp**. Số thì an toàn, nhưng bất kỳ
   script nào lỡ truyền chuỗi dài vào `SetMissionV` sẽ hỏng âm thầm.
2. **Phạm vi biến là MAP, không phải MISSION.** `LuaSetMission` / `LuaGetMissionValue` thao tác trên
   `SubWorld[idx].m_MissionArray` — tức **kho biến của cả map**, dùng chung cho mọi mission trên map đó
   (`ScriptFuns.cpp:11007`, `11049`). Bản Linux cũng lấy theo map
   (`lib:DoFunInWorld(nMapIndex, GetMissionV, ...)` — `[LINUX] challengeoftime\npc_death.lua:155`)
   ⇒ **trùng ngữ nghĩa**, nhưng nếu **hai mission cùng chạy trên MỘT map** thì dải id biến phải không đụng nhau.
3. **`GetMissionV` bỏ ô 0.** `if (nValueId > 0)` (`ScriptFuns.cpp:11048`) — id 0 luôn trả 0,
   trong khi `SetMissionV` lại cho phép ghi id 0 (`11004`: `if (nValueId < 0) return`). Không dùng ô 0.

---

## 2. HỆ TIMER — **JX1 ĐÁP ỨNG ĐƯỢC**, nhưng có 2 cái bẫy

JX1 có **4 tầng timer** đang sống:

### (a) Mission timer (thứ Phong Lăng Độ / Vượt Ải cần)

* `StartMissionTimer(missionId, timerId, nTicks)` — `ScriptFuns.cpp:11363-11391`, đăng ký `14750`
* `StopMissionTimer(missionId, timerId)` — `11332-11361`, đăng ký `14751`
* `GetMSRestTime(missionId, timerId)` — đăng ký `14752`
* **Đơn vị = tick game (18/giây)** — `KTaskFuns.h:42` (`g_SubWorldSet.GetGameTime() + ulTime`).
  Đúng đơn vị bản Linux dùng (`StartMissionTimer(MISSION_MATCH, TIMER_CLOSE, LIMIT_FINISH * 18)`,
  `[LINUX] challengeoftime\include.lua:220`).
* **Timer tự lặp lại** sau mỗi lần nổ (`KTaskFuns.cpp:122`).

**🔴 BẪY 1 — rò khe timer.** `LuaStartMissionTimer` chỉ gọi `m_cTimerTaskSet.Add()` mà
**không tìm timer trùng id trước** (`ScriptFuns.cpp:11382`). Gọi `StartMissionTimer` cùng một id
**9 lần là hết sạch 9 khe** và mọi timer sau đó im lặng thất bại. (So sánh: `LuaStartGlbMSTimer`
`KJx2League.cpp:974-982` **có** xử lý trùng.)

**🔴 BẪY 2 — timer id KHÔNG được truyền vào script, và script được tra theo BẢNG NGOÀI.**
Khi timer nổ, `KTimerTaskFun::Activate` (`[SRC] KTaskFuns.cpp:117-132`) tra
**`\settings\TimerTask.txt`** (`GameDataDef.h:65`) theo **timerId** để lấy tên script, rồi
`g_MissionTimerCallBackFun` (`KMission.cpp:338-366`) gọi **`OnMissionTimer`**, nếu script không có hàm đó
thì gọi **`OnTimer`** — **với tham số bằng 0** (`KMission.cpp:362` → `ExecuteScript(..., 0)` →
`CallFunction(szFunName, 0, "d", nParam)` `KMission.cpp:97`).
⇒ Mỗi timer id mới phải **thêm một dòng vào `[LIVE] settings\TimerTask.txt`**.

Bảng hiện tại có **36 dòng**, các id đã dùng: `1-10, 12-18, 20, 21, 50-55, 61, 62, 65-70, 75-77`.
Các id bản Linux cần **đều còn trống**:

| Tính năng Linux | Timer id | Bằng chứng |
|---|---|---|
| `fengling_ferry` | **28, 29** | `[LINUX] missions\fengling_ferry\mission.lua:15-16,57-58` |
| `challengeoftime` | **41 (MATCH), 42 (BOARD), 43 (CLOSE)** | `[LINUX] missions\challengeoftime\include.lua:7-9,209,220`, `mission_match.lua:12` |

### (b) Global mission timer (không gắn map)

`OpenGlbMission` / `CloseGlbMission` / `StartGlbMSTimer` / `StopGlbMSTimer` —
`[SRC] KJx2League.cpp:956-1005`, đăng ký `ScriptFuns.cpp:15119-15121`, `14896`.
Đơn vị **frame**, kẹp `[18 … 31.104.000]` (`KJx2League.cpp:967-971`).

### (c) Timer script kiểu JX2 (`AddTimer`)

`AddTimer(nFrames, "Ham", nParam)` → id; `DelTimer` / `SuspendTimer` / `ResumeTimer` —
`[SRC] KJx2League.cpp:1051-1120+`, đăng ký `ScriptFuns.cpp:15254-15257`.
Chạy trong **state của tệp đang gọi**, hỗ trợ dạng `"Bảng:Hàm"` (dùng cho `timerlist.lua`).

### (d) Timer theo người chơi

`SetTimer(Time, TimerTaskId)` — `ScriptFuns.cpp:2701-2709`, đăng ký `14357`. Cũng đi qua `TimerTask.txt`.

### (e) Đồng hồ toàn server

`[LIVE] script\timerserver.lua` — hàm `RunTime()` (dòng 33) chạy theo phút, đã có sẵn **đúng 2 dòng**
cho hai tính năng của ta nhưng **đang bị chú thích**:

```
timerserver.lua:15   Include("\script\tinhnang\vuot_ai\lib_vuotai.lua")
timerserver.lua:16   Include("\script\tinhnang\phonglangdo\lib_phonglangdo.lua")
timerserver.lua:79   -- sukien_vuotai(nHr,nMi)
timerserver.lua:80   -- sukien_phonglangdo(nHr,nMi)
```

---

## 3. SINH NPC ĐỘNG

| Hàm | Chữ ký | Bằng chứng |
|---|---|---|
| `AddNpc` | `(nId, nLevel, nSubWorldIdx, nX, nY, nSeries [, szName])` → npcIdx | thân `ScriptFuns.cpp:6814-6844`, đăng ký `14494` |
| `AddNpcEx` | `(nId, nLevel, nSeries, nSubWorldIdx, nX32, nY32, nCamp, szName, nFlag)` — **thứ tự kiểu Linux** | thân `6874`+, đăng ký `14495`, chú giải `6846-6873` |
| `DelNpc` | `(npcIdx)` | `14497` |
| `DelAllNpc` / `DelAllNpcName` | xoá NPC trong map | `14768-14769` |
| `ClearMapNpc` / `ClearMapNpcWithName` / `GetMapNpcWithName` | | `15191, 15193, 15194` |
| `GetAroundNpcList` / `GetNpcAroundNpcList` | | `15195`, `15266` |

**Gán script tử vong: CÓ.** `SetNpcDeathScript` (`ScriptFuns.cpp:15190`), cùng với `SetNpcScript`
(script hội thoại) và `SetNpcDropScript` (script rơi đồ).

**73 hàm tinh chỉnh NPC** đã có, gồm: `SetNpcLife` `SetNpcExp` `SetNpcDamage` `SetNpcDefense`
`SetNpcResist` `SetNpcReplenish` `SetNpcHitRecover` `SetNpcRevTime` `SetNpcTimer` `SetNpcValue`
`SetNpcParam` `SetNpcBoss` `SetNpcBoss2` `SetNpcCamp` `SetNpcCurCamp` `SetNpcSeries` `SetNpcAIType`
`SetNpcActiveRange` `SetNpcSkill` `SetNpcSpeed` `SetNpcName` `SetNpcPos` `SetNpcOwner` `SetNpcKind`
`SetNpcHonorId` `SetNpcDmgEx` `SetNpcDamageReduction` `SetNpcWalkOriginPos` `SetNpcFindPathTime`
`SetNpcTimeIdle` `SetNpcFightState` + bộ `GetNpc*` tương ứng.

**Trần**: `MAX_NPC = 98000` (`[SRC] KNpc.h:21`), mission giữ tối đa 5000 NPC.

**Bọc sẵn ở Lua**: `AddNpcEx1/2/3/4` — `[LIVE] script\lib\lib_map.lua:91, 119, 134, 193`
(`AddNpcEx3` cho phép đặt luôn exp/máu/sát thương/thời gian phục sinh).

**Chú ý toạ độ**: `AddNpc` nhận toạ độ **pixel** (ô × 32) — xem `lib_map.lua:102` và
`killbosshead.lua:187-189` (`Tab3[i][4] * 32`).

**Thiếu (so với bản Linux)**: `DropItemEx` (ELF `0x0811FD70`), `NpcDropMoney` (`0x0811D9C0`),
`NpcName2Replace` (`0x081006D0`), `NPCINFO_GetSeries` (`0x081C08E0` — JX1 có `GetNpcSeries`,
chỉ cần **alias**), `ITEM_DropRateItem` (`0x08154DE0`).

---

## 4. HỆ TỔ ĐỘI

| Bản Linux | Bên JX1 | Bằng chứng |
|---|---|---|
| `GetTeamSize()` | `GetTeamSize` | thân `ScriptFuns.cpp:2809-2835`, đăng ký `14365` |
| `GetTeamMember(n)` | `GetTeamMem` + **alias `GetTeamMember`** | thân `7500`+, đăng ký `14367` và `14292` |
| — | `GetTeam` (tên Lua thật, **KHÔNG** phải `GetTeamId`), `LeaveTeam`, `SetCreateTeam` | đăng ký `14364`, `14366`, `14780` |  `[da sua theo phan bien]`

Ngữ nghĩa `GetTeamMember` **đã được dựng riêng cho JX2**: `sJx2TeamMember`
(`ScriptFuns.cpp:7480-7498`) — `n == 1` trả **đội trưởng**, `n >= 2` trả **thành viên hợp lệ thứ n-1**
(bỏ ô trống). Khớp `[LINUX] 0x08115530`. Đúng khuôn vòng lặp JX2
`for i = 1, GetTeamSize() do GetTeamMember(i)`.

### 🔴 ĐIỀU KIỆN BẬT: danh sách tiền tố đường dẫn HARDCODE

Nhánh JX2 chỉ chạy khi `g_IsJx2Script(L)` trả 1, và hàm này dò **CHUỖI CON** (`strstr`, `KSortScript.cpp:130`)  `[da sua theo phan bien]`
của đường dẫn script trong một mảng cứng — **không phải so tiền tố** — `[SRC] Core\Src\KSortScript.cpp:113-133`:

```
KSortScript.cpp:116-122  "\script\missions\citywar_", "\script\missions\leaguematch\",
                         "\script\leaguematch\", "\script\missions\tong\",
                         "\script\task\tollgate\",  "\script\item\messenger\",
                         "\script\item\xinshirenwu\", "\script\missions\tongwar\",
                         "\script\event\tongwar\", "\script\missions\bw\",
                         "\script\missions\bairenleitai\", "\script\missions\tongcastle\",
                         "\script\missions\arena\", "\script\activitysys\",
                         "\script\tong\", "\scriptjx2\",
                         "\script\global\npcchucnang\dichquan.lua",
                         "\script\missions\basemission\"
```

* `\script\task\tollgate\` **đã có** ⇒ nhánh **săn boss sát thủ** tự hưởng.
* `\script\activitysys\` **đã có**.
* **PHẢI THÊM**: `"\script\missions\fengling_ferry\"` và `"\script\missions\challengeoftime\"`,
  nếu không thì `GetTeamMember`, `SetPunish`, `AddSkillState`, `GetGameTime` sẽ chạy theo quy ước JX1
  (`GetTeamMember` sẽ **bỏ sót đội trưởng** và đọc ô trống).
* ⚠ `m_szScriptName` chỉ giữ **100 ký tự** (`KSortScript.cpp:161` `g_StrCpyLen(..., 100)`) —
  đường dẫn dài hơn bị cắt; vì là `strstr` nên mẫu nằm ở ĐẦU đường dẫn vẫn khớp — chỉ mẫu nằm sau ký tự 100 mới mất.  `[da sua theo phan bien]`

---

## 5. HỆ THƯỞNG / DROP

### 5.1 `tbAwardTemplet` — **ĐÃ CÓ**

`[LIVE] script\lib\awardtemplet.lua` (67 dòng) — đúng lớp `tbAwardTemplet` của bản Linux:  `[da sua theo phan bien]`
`RegType`, `Give`, `GivByRandom`, `GiveAwardByList`. Dòng 63-67 tự kéo 3 kiểu thưởng:

```
awardtemplet.lua:65  Include("\script\lib\awardtype\item_jx1.lua")
awardtemplet.lua:66  Include("\script\lib\awardtype\zhenyuan_jx1.lua")
awardtemplet.lua:67  Include("\script\lib\awardtype\exp_jx1.lua")
```

Thư mục `[LIVE] script\lib\awardtype\` có: `simple.lua`, `exp.lua`, `item_jx1.lua`,
`zhenyuan_jx1.lua`, `exp_jx1.lua` (**5**). Bản JX2 gốc có 13 kiểu (`[REPO-JX2] lib\awardtype\`:
thêm `fun`, `honour`, `item`, `jxb`, `repute`, `title`, `vng_contribute`, `vng_energy`, `zhenyuanpoint`).

**Ba tính năng chỉ dùng `tbProp` (vật phẩm) và `nExp` / `nExp_tl` (kinh nghiệm)** ⇒ **ĐỦ**:

* `[LINUX] challengeoftime\award.lua:105,110,114`, `chuangguang30.lua:123-124`,
  `item\chuangguanbaoxiang.lua:168`, `rank_perday.lua:65,72`
* `[LINUX] fengling_ferry\mission.lua:120`, `fld_head.lua:124`

### 5.2 `droptemplet` — 🔴 SAI ĐƯỜNG DẪN

`[LINUX] missions\fengling_ferry\boss.lua:15` gọi `Include("\\script\\lib\\droptemplet.lua")`.

* `[LIVE] script\lib\droptemplet.lua` → **KHÔNG CÓ**
* `[LIVE] scriptjx2\lib\droptemplet.lua` → **CÓ**

⇒ phải **chép sang `script\lib\`** hoặc sửa đường dẫn Include trong script port.

### 5.3 Đường drop kiểu JX1 (đang chạy)

`SetNpcDropScript(npcIdx, "\script\...\drop.lua")` — ví dụ thật:
`[LIVE] script\tinhnang\phonglangdo\drop.lua`, `script\tinhnang\vuot_ai\drop.lua`.
Có `DropItem`, `DropItemPUBG`, `GetItemLife`, `GetItemProp`, `GetItemCountEx`, `DelItemEx`.

### 5.4 Bảng dữ liệu drop còn thiếu

`[LINUX] settings\droprate\boss\` có **57 tệp**, `[LIVE] settings\droprate\boss\` chỉ có **1**.
`killer\kill_level.lua` cần `bosstask_lev20.ini … bosstask_lev90.ini` (**8 tệp**).

---

## 6. BẢNG XẾP HẠNG / `IncludeLib("RELAYLADDER")`

### 6.1 S3Relay **KHÔNG** có ladder

`[SRC] MultiServer\S3Relay\DoScript.cpp:272-278` — toàn bộ bảng hàm Lua của S3Relay chỉ có **4 mục**:

```
{"GlobalExecute", ...}, {"GameExecute", ...}, {"Msg2IP", ...}, {"GetIP", ...}
```

Tìm chuỗi `Ladder` / `LADDER` trong **toàn bộ** mã nguồn `MultiServer\S3Relay\*.{cpp,h}`
(**109 tệp** `.cpp/.h`; con số 197 là TỔNG mọi tệp trong thư mục, kể cả `.pdb/.obj/.vcxproj`): **0 kết quả**.  `[da sua theo phan bien]`

### 6.2 Nhưng ladder **ĐÃ ĐƯỢC CÀI trong CoreServer**

| Hàm Lua | Thân | Đăng ký |
|---|---|---|
| `Ladder_NewLadder(id, szName, nValue [,nType][,nSect][,nGender])` | `[SRC] Core\Src\KJx2SharedStore.cpp:531-578` | `ScriptFuns.cpp:15085` |
| `Ladder_ClearLadder(id)` | `KJx2SharedStore.cpp:580-595` | `15086` |
| `Ladder_GetLadderInfo(id, nRank)` → `szName, nValue, nSect, nGender` | `KJx2SharedStore.cpp:599-627` | `15087` |

`IncludeLib("RELAYLADDER")` **hợp lệ**: bảng module `ScriptFuns.cpp:2485-2503` có `"RELAYLADDER"`
trỏ `scriptjx2\lib\noop.lua` (hàm thật nằm ở C, file chỉ để `dofile` không lỗi).

### 6.3 Giới hạn phải biết

* **TOP 10 cứng** — `JX2LADDER_TOP = 10` (`KJx2SharedStore.cpp:440`); mọi mục thứ 11 trở đi **bị cắt** (`:574-575`).
* **id phải > 10000** — `JX2LADDER_MIN_ID = 10000` (`:441`, kiểm ở `:538`, `:585`).
* **Lưu file cục bộ + KGameKV** namespace `"jx2.ladder"` (`:443`, `:526`), ghi qua tệp tạm + `MoveFileEx` (`:524`).
* **Upsert theo TÊN**, sắp giảm dần theo giá trị (`:552-573`).
* Ô trống trả `("", 0, -1, 0)` — **không phải `nil`** (`:622-626`), đúng như `ladderfunlib.lua:20` trông đợi.

### 6.4 🔴 Đây là ladder **MỘT máy chủ**

Không đi qua relay, không đồng bộ liên server. Nếu 3 hoạt động cần bảng hạng **liên server** hoặc
**giữ hơn 10 mục** (ví dụ `rank_perday` của Vượt Ải muốn lưu điểm cá nhân ngoài top-10) thì **phải mở rộng**.

Các ladder id đang dùng thật trong `[LIVE]` (quét lại TOÀN BỘ điểm gọi `Ladder_*`):  `[da sua theo phan bien]`
**10118** (`global\seasonnpc.lua:1108`); **10182-10185** (`STORM_LADDERID_WEEK1 + weekid`, hằng ở
`event\storm\head.lua:55-58`, gọi ở `event\storm\function.lua:240,428`) và **10186** (`head.lua:59`, `function.lua:242,430`);
**10196 / 10197** (`item\liendau_admin.lua:147,157`); **10225 / 10226** (`event\tongwar\score.lua:72-76`);
**10227** (`PHB_WULIN`) và **10228+** (`PHB_CHENGSHI`) — `event\tongwar\headinfo.lua:136-137,180-181`;
**10250 / 10251** (`startgame\tinhnang\tongkim\songjin_shophead.lua:70,97,190`);
**10261** (`missions\citywar_global\ladder.lua:9`).
🔴 Bản cũ ghi **10119** — id này **KHÔNG tồn tại** ở đâu cả. **10187** chỉ nằm trong dòng ĐÃ CHÚ THÍCH
(`task\tollgate\messenger\posthouse.lua:111,415,447,549` đều mở đầu `--`). **10999** chỉ có trong
`script\test\citywar_e3.lua:204` (script thử), không phải đường chạy thật.

---

## 7. HỆ `ACTIVITYSYS`

### 7.1 JX1 **ĐÃ CÓ** — và giống byte-by-byte với bản Linux

`[LIVE] script\activitysys\` có **6 tệp**, so byte với `[LINUX] script\activitysys\`:

| Tệp | Cỡ | Kết quả `cmp` |
|---|---|---|
| `activity.lua` | 8.685 B | **GIỐNG HỆT** |
| `activitydetail.lua` | 4.276 B | **GIỐNG HỆT** |
| `functionlib.lua` | 10.164 B | **GIỐNG HỆT** |
| `ladderfunlib.lua` | 1.940 B | **GIỐNG HỆT** |
| `playerfunlib.lua` | 18.746 B | **GIỐNG HỆT** |
| `g_activity.lua` | 2.613 B | **GIỐNG HỆT** |

⇒ **`PlayerFunLib` CÓ**, **`LadderFunLib` CÓ**, **`lib:DoFunInWorld` CÓ**
(`[LIVE] script\activitysys\functionlib.lua:316`).
`[REPO-JX2] serverscript_jx2\` **không có** thư mục `activitysys` — nhưng kho này có **25 mục cấp 1**  `[da sua theo phan bien]`
(`bairenleitai boot bw citywar_e datau_tasklink global_securitylock jx1_edits lib libfix loidai_cn nationalwar`
`port5_admin settings_tong settings_tong_vn startgame test tinsu tong tong_hoatdong tong_vn tongcastle tongwar`
`trap_fix` + `jx2compat.lua.DISABLED`, `timerserver_reference.lua`), chứ KHÔNG phải "chỉ lib/tong/tong_vn".

### 7.2 Nhưng **THIẾU 803/809 tệp** và **CHƯA KHỞI ĐỘNG**

`[LINUX] script\activitysys\` có **809 tệp `.lua`**. JX1 thiếu:

* **`npcfunlib.lua`** ← 🔴 chính là nơi định nghĩa **`NpcFunLib:CheckKillerdBoss`** và
  **`NpcFunLib:CheckBoatBoss`** (điểm gọi: `[LINUX] activitysys\config\1002\config.lua:94`,
  `config\1000\config.lua:68`, `config\1007\config.lua:102,189`, `config\1008\config.lua:123,200`, …)
* **`detailtype\` (32 tệp)** ← 🔴 chứa đúng các móc nối ta cần:
  * `finishkillerboss.lua` → **săn boss sát thủ** (`ActivityDetailClass:NewType("FinishKillerBoss")`, dòng 3)
  * `chuanguan.lua` + `signup_chuangguan.lua` → **Vượt ải** (`NewType("Chuanguan")`, dòng 3)
  * `finishifenglingdu.lua` + `vngfldcollectitem.lua` → **Phong Lăng Độ**
* **`config\` (~770 tệp)** — bao đóng chỉ cần `config\32\` (4 tệp) và `config\1005\` (2 tệp)
* `ambience.lua`, `answer.lua`, `answersys.lua`, `g_itemuse.lua`, `g_npcdeath.lua`, `itemuse.lua`,
  `npcdailog.lua`, `npctimer.lua`, `autotest\`

**Chưa khởi động**: `G_ACTIVITY:LoadActivitys()` / `:AddActivity()` **chỉ có định nghĩa**
(`g_activity.lua:24`, `:114`) — **không nơi nào trong `[LIVE]` gọi chúng**.
`startgame.lua:107` còn ghi rõ đã **thay** `activitysys\config\9 ServerStart` bằng gọi trực tiếp:
`DynamicExecute("\script\missions\bairenleitai\bairen_boot.lua", "BairenLeitai_Init")`.

Tìm `CheckKillerdBoss` / `CheckBoatBoss` / `Chuanguan` trong toàn bộ `[LIVE] script`: **0 kết quả**
(một lần duy nhất "ChuanGuan" xuất hiện là ở `misc\eventsys\type\func.lua` — xem 7.3).

### 7.3 Bù lại: **EVENTSYS đã đầy đủ và đang chạy**

`[LIVE] script\misc\eventsys\` — 18 tệp, `eventsys.lua` đầy đủ
(`NewType` `GetType` `Reg` `UnReg` `GetProcParam` `OnEvent` `OnPlayerEvent`).
`type\func.lua` **đã khai báo sẵn cả 3 loại sự kiện ta cần**:

```
func.lua:33-37  ["KillerBoss"]  = { "OnFinish", "OnKillBoss" }
func.lua:10-14  ["ChuanGuan"]   = { "OnEnter", "OnPass" }
func.lua:22-28  ["FengLingDu"]  = { "OnBoard", "OnLanding",
                                    "OnFinishShuiZeiTask", "OnShuiZeiDeath" }
```

Đang có nơi phát/nghe thật: `maps\newworldscript_default.lua:51-62`,
`global\map_helper.lua:129`, `item\xinshirenwu\xinshibaoxiang.lua:156`.
⇒ **Khung nhận đã sẵn; chỉ thiếu bên PHÁT (script tính năng) và bên NGHE (activitysys detailtype).**

---

## 8. ĐIỂM CHÈN HÀM LUA MỚI TRONG `ScriptFuns.cpp` — SỐ DÒNG THẬT HÔM NAY

Tệp: `D:\GAMEDEVNEW\Sources\Core\Src\ScriptFuns.cpp` — **427.894 byte, 15.348 dòng**,  `[da sua theo phan bien]`
sửa đổi **23/08/2026 16:03:05**. Đếm thật, không lấy từ trí nhớ.

### 8.1 Bản đồ tiền xử lý

| Dòng | Nội dung |
|---|---|
| **14255** | `#endif // _SERVER (khoi ham WLLS)` ← **kết thúc khối thân hàm server-only cuối cùng** |
| **14257** | `TLua_Funcs GameScriptFuns[] =` ← **bảng bắt đầu** |
| 14258 | `{` |
| 14288 → 14293 | `#ifdef _SERVER` … `#endif` (alias `GetTeamMember`) |
| 14323 → **15315** `#else` → **15318** `#endif` | khối `_SERVER` LỚN của bảng |
| 14798 → **15269** | `#ifdef _SERVER` … `#endif` (lồng — **khối port JX2**) |
| 14884 → 14931 | `#ifdef _SERVER` … `#endif` (lồng) |
| **15323** | `};` ← **bảng kết thúc** |
| 15325-15338 | `TLua_Funcs WorldScriptFuns[] = { … };` |
| 15340-15343 | `int g_GetGameScriptFunNum() { return sizeof(GameScriptFuns) / sizeof(GameScriptFuns[0]); }` — **không phải sửa tay** |

### 8.2 Khuyến nghị chèn

* **Thân hàm mới (server-only)**: chèn **ngay TRƯỚC dòng 14255**, bên trong khối `#ifdef _SERVER`
  của các hàm WLLS. (Hoặc mở khối `#ifdef _SERVER … #endif` mới giữa dòng 14255 và 14257.)
* **Dòng đăng ký (server-only)**: chèn **ngay TRƯỚC dòng 15269**. Đây đúng chỗ các đợt port JX2
  gần nhất đã dùng — cụm `[PORT5 23/08]` nằm ở **15241-15268**, cụm cổng thành E4/E7 ở 15189-15240.
* **Dòng đăng ký dùng chung client+server**: chèn trước dòng 15323 (sau `#endif` 15318) —
  hiếm khi cần cho việc này.
* **Khai báo `extern`** cho hàm nằm ở tệp `.cpp` khác: đặt cùng chỗ với các dòng
  `extern int LuaXxx(Lua_State* L);` hiện có, ví dụ 13564-13566 (ladder), 13598-13600 (glb mission),
  13675 (`LuaSetNpcDeathScript`), 13814 (`LuaCloseGlbMission`).

### 8.3 Số hàm đang đăng ký

**1.005** dòng đăng ký trong `GameScriptFuns[]` (**1.002** tên duy nhất — `BT_GetData`, `GetLeadLevel`,
`GetMissionS` mỗi cái đăng ký HAI lần, mục sau đè mục trước).  `[da sua theo phan bien]`
Danh sách đầy đủ đã lưu ở
`C:\Users\nguye\AppData\Local\Temp\claude\…\scratchpad\jx1_funcs.txt` (tệp tạm của phiên).

### 8.4 Nơi khác cũng có bảng cùng tên (đừng nhầm)

* `[SRC] S3Client\Ui\ShortcutKey.cpp:2273` — bảng của **client UI**, khác hoàn toàn.
* `[SRC] MultiServer\S3Relay\DoScript.cpp:272` — bảng của **S3Relay**, chỉ 4 hàm.
* Bảng của engine game nằm ở `Core\Src\ScriptFuns.cpp` — được nạp bởi
  `KSortScript.cpp:152`, `KPlayer.cpp:7405`.

---

## 9. CÁC ĐỢT PORT TRƯỚC ĐÃ LÀM GÌ — VÀ CÒN THIẾU GÌ

### 9.1 Tín Sứ (21/08) đã kéo theo **một nửa** hạ tầng Săn boss sát thủ

`[REPO-JX2] serverscript_jx2\tinsu\script\task\tollgate\` đã chép sang `[LIVE] script\task\tollgate\`:

* `killbosshead.lua` — **375,8 KB**, đã có ở `[LIVE]`
* `addtollgatenpc.lua` — 21 dòng, đã có ở `[LIVE]`
* toàn bộ `messenger\` (fengzhiqi / shanshenmiao / qianbaoku / trap) — đã có
* `tinsu_addnpc.lua` — bộ nối vào `startgame.lua`

**Đã có sẵn trong `killbosshead.lua` (rất giá trị):**

* Bảng **`addkillertasknpc`** — dòng **4-180**, **160 boss sát thủ**, **TÊN ĐÃ DỊCH TIẾNG VIỆT**
  (`"Trác Lãnh Cầm"`, `"Tiêu Thiên Ngạo"`, …), mỗi dòng gồm
  `{npcId, level, mapId, x, y, ?, tên, ?, "\script\task\tollgate\killer\kill_level.lua", mã 1..160}`
* Hàm nạp `add_killertasknpc(Tab3)` — dòng **183-194** (đã có `SetNpcScript` + `SetNpcParam(...,1,mã)`)
* Hàm `add_bossnpc` (3393), `add_messengernpc` (3406)
* Các chuỗi mô tả `DescLink_*` — dòng 3417-3421

### 9.2 🔴 CÒN THIẾU cho Săn boss sát thủ

1. **Không ai gọi `add_killertasknpc(addkillertasknpc)`.**
   `[LIVE] script\task\tollgate\addtollgatenpc.lua:10-21` chỉ gọi cho `AddNpc_turesureboss`,
   `AddNpc_turesurebug`, `AddNpc_allbugbear`; các dòng Killer bị **chú thích** (12-15, 18).
   Bản Linux gọi ở `[LINUX] script\global\autoexec.lua:158`.
2. **Cả thư mục `script\task\tollgate\killer\` KHÔNG CÓ ở `[LIVE]`** — 5 tệp bên Linux:
   `kill_level.lua`, `lib_killlevel.lua`, `mibao_head.lua`, `nieshichen.lua`, `shashou_mibao.lua`.
3. **Bảng `settings\task\tollgate\killer\killer.txt` KHÔNG CÓ** (`[LINUX]` có).
4. `activitysys\detailtype\finishkillerboss.lua` + `npcfunlib.lua` (`CheckKillerdBoss`) — không có.

### 9.3 Phong Lăng Độ & Vượt Ải — JX1 đã có **bản Việt Nam riêng**, ĐANG TẮT

| | Phong Lăng Độ | Vượt Ải |
|---|---|---|
| Thư mục | `[LIVE] script\tinhnang\phonglangdo\` (**8 tệp**) | `[LIVE] script\tinhnang\vuot_ai\` (**6 tệp**) |
| Tệp | `lib_phonglangdo.lua`, `thuyenphu.lua`, `thuyenphubac.lua`, `quaipld.lua`, `bossthuytacdaulinhpld.lua`, `dietrenthuyen.lua`, `logout.lua`, `drop.lua` | `lib_vuotai.lua`, `sugiasatthu.lua`, `ondeath.lua`, `die_normal.lua`, `die_boss.lua`, `drop.lua` |
| Mission id | `MS_PLANGDO = 4` | `MS_VUOTAI = 3` |
| Nguồn id | `[LIVE] script\lib\lib_task.lua:285` | `lib_task.lua:284` |
| Map | 336 (Phong Lăng Độ), 337/338/339 (3 bến thuyền) | 480-489 |
| Timer mission | id 4, 5 (`lib_phonglangdo.lua:267,274`) | id 1, 2, 6 (`sugiasatthu.lua:219`, `die_boss.lua:36`) |
| Trạng thái | **CHỈ TẮT LỊCH** — `timerserver.lua:80` chú thích, NHƯNG `addnpcphonglangdo()` **vẫn được gọi thật** ở `startgame.lua:100` ⇒ 6 NPC "Thuyền phu" (`lib_phonglangdo.lua:147-157`) spawn mỗi lần khởi động  `[da sua theo phan bien]` | **TẮT** — `timerserver.lua:79`; `addnpcbosssatthu()` ở `startgame.lua:99` cũng đang chú thích |
| Tác giả | "Fong Kieu", 28/11/2020 | "Fong Kieu", 28/11/2016 |

Bản này **dùng đúng hệ mission JX1**: `OpenMission`, `StartMissionTimer`, `AddMSNpc`, `AddMSPlayer`,
`SetPMParam`, `GetPMParam`, `PIdx2MSDIdx`, `MSDIdx2PIdx`, `Msg2MSAll`, `GetMSRestTime`, `GetMSPlayerCount`.
⇒ **Bằng chứng thực nghiệm rằng hạ tầng mission JX1 chạy được loại hoạt động này.**

⚠ **Nhưng thiết kế KHÁC bản JX2 gốc**: bản VN không có phà thật sự chạy (`fldmap_boat1/2/3`,
`MS_STATE` 1/2/3), không có nhiệm vụ thuỷ tặc kiểu Linux, không có bảng `activitysys`.
⇒ Cần một **quyết định kiến trúc** (mục 10.6).

### 9.4 Tài nguyên **đã sẵn**, không phải làm

| Thứ | Trạng thái | Bằng chứng |
|---|---|---|
| Map 336 "Phong Lăng Độ" | CÓ | `[LIVE] settings\maplist.ini:2915-2930` |
| Map 337/338/339 "Bến thuyền 1/2/3" | CÓ | `maplist.ini:2932-2935`; đúng map bản Linux dùng (`SubWorldID2Idx(337/338/339)`) |
| Map 480-489 (sát thủ) | CÓ | `maplist.ini:3759-3786` |
| Map **957** | CÓ | `maplist.ini:6233` — **đúng `CHUANGGUAN30_MAP_ID = 957`** của `[LINUX] challengeoftime\include.lua:27` |
| `settings\task\missions.txt` | 45 dòng; **id 13-23, 27-32, 34-36, 41-43 trỏ `mission_trong.lua` = TRỐNG** | đọc thật |
| Mission id 22 (`MISSION_MATCH` của Vượt ải bản Linux) | **TRỐNG** | `missions.txt:23` |
| `settings\TimerTask.txt` | 36 dòng; id 28, 29, 41, 42, 43 **TRỐNG** | đọc thật |
| `misc\eventsys\` + `misc\taskmanager.lua` + `misc\boss\callbossdeath.lua` | CÓ | Glob `[LIVE] script\misc\**` |
| `lib\gb_taskfuncs.lua` (`gb_GetTask`/`gb_SetTask`) | CÓ | so tên hàm |
| `missions\basemission\lib.lua` (`basemission_CallNpc`) | CÓ | so tên hàm |
| `event\storm\` (`storm_addpoint`, `storm_end`, `storm_valid_game`, `storm_add_pointex`) | CÓ | so tên hàm |
| `task\task_award_extend.lua` (`AddExp_Skill_Extend`) | CÓ | so tên hàm |
| `item\forbiditem.lua` (`set_MapType`) | CÓ | so tên hàm |
| `missions\tongwar\match\head.lua` (`nt_getTask`/`nt_setTask`) | CÓ | so tên hàm |
| `scriptjx2\tong\tong_award_head.lua` (`tongaward_killer`, `tongaward_challengeoutoftime`) | CÓ (khác đường dẫn) | so tên hàm |
| `scriptjx2\lib\file.lua` (`GetTabFileData`, `GetTabFileHeight`) | CÓ (khác đường dẫn) | so tên hàm |

---

## 10. MƯỜI KHỐI PHẢI XÂY MỚI — XẾP THEO KHỐI LƯỢNG GIẢM DẦN

> Cơ sở định lượng: `closure3.json` — bao đóng Include của 3 tính năng có **85 tệp `.lua` duy nhất**
> mà JX1 **không có cùng đường dẫn** (satthu 28 / phonglangdo **44** / vuotai 54, giao nhau nhiều).  `[da sua theo phan bien]`
> Trong 85 tệp đó, **15 tệp thực ra đã nằm ở `[LIVE] scriptjx2\`** — chỉ sai đường dẫn Include.

---

### ① `activitysys` đầy đủ: `detailtype\` + `npcfunlib.lua` + `config\` + khởi động hệ
**Khối lượng: LỚN NHẤT.** Bản Linux 809 tệp `.lua`, JX1 mới có 6.
* **Bắt buộc tối thiểu**: `npcfunlib.lua` (chứa `CheckKillerdBoss`, `CheckBoatBoss`) +
  `detailtype\` **32 tệp** (trong đó `finishkillerboss.lua`, `chuanguan.lua`, `signup_chuangguan.lua`,
  `finishifenglingdu.lua`, `vngfldcollectitem.lua` là 5 móc nối trực tiếp) +
  `config\32\` (4 tệp) + `config\1005\` (2 tệp).
* **Phải viết mới bộ khởi động**: hiện **không có ai gọi** `G_ACTIVITY:LoadActivitys()`
  (`g_activity.lua:24`). Bản Linux khởi động qua `config\<n>\registe.lua`.
* Kèm theo: `g_npcdeath.lua`, `g_itemuse.lua`, `itemuse.lua`, `npcdailog.lua`, `npctimer.lua` nếu
  muốn đủ đường phát sự kiện.
* ⚠ Nếu quyết **bỏ activitysys** và nối thẳng vào `eventsys` (như đợt Bách Nhân đã làm ở
  `startgame.lua:107`) thì khối này rút xuống rất nhỏ — **đây là quyết định kiến trúc số 1**.

---

### ② Bộ hàm engine Lua còn thiếu trong `ScriptFuns.cpp`
**44 hàm** có địa chỉ ELF xác thực trong `jx_linux_y.luamap.full.txt` (3 hàm còn lại
`ContriValueEntryLogic`, `JudgePLAddTitle`, `Require` **không có** trong luamap ⇒ CHƯA XÁC MINH).

**Nếu chỉ port phần LÕI của 3 tính năng thì chỉ cần 5 hàm** (đo bằng cách quét riêng
`task\tollgate\killer\`, `missions\fengling_ferry\`, `missions\challengeoftime\`):

| Hàm | ELF | Dùng ở | Ghi chú |
|---|---|---|---|
| `AddStatData` | `0x080FF550` | cả 3 | thống kê VNG — **có thể làm stub rỗng** |
| `ITEM_DropRateItem` | `0x08154DE0` | satthu (`kill_level.lua`) | rơi đồ theo bảng tỉ lệ |
| `NPCINFO_GetSeries` | `0x081C08E0` | satthu (`kill_level.lua`) | JX1 **đã có `GetNpcSeries`** ⇒ chỉ cần alias |
| `ITEM_SetExpiredTime` | `0x08154A30` | phonglangdo (`bossdeath.lua`, `shuizeideath.lua`) | vật phẩm có hạn |
| `NpcName2Replace` | `0x081006D0` | vuotai (`npc\transfer.lua`) | |

**Nếu port trọn bao đóng** thì thêm 39 hàm nữa, nhóm theo chủ đề:
`ITEM_GetExpiredTime` `ITEM_GetItemRandSeed` `ITEM_SetLeftUsageTime` `SetItemBindState`
`GetItemAllParams` `GetItemGenTime` `GetItemQuality` `GetGlodEqIndex` `GetPlatinaEquipIndex`
`GetPlatinaLevel` `GetRoomItems` `DropItemEx` `NpcDropMoney` `GetNpcAroundPlayerList`
`GetFirstPlayerAtServer` `GetNextPlayerAtServer` `Add120SkillExp` `BT_GetBattleParam`
`IsDisabledUseHeart` `OpenProgressBar` `QueryWiseManForSB` `SendScriptData`
`ST_DoTransLife` `ST_IsTransLife` `ST_LevelUp` `PET_GetGrownPoint` `PET_GetTamePoint`
`PET_GetUpgradePoint` `PET_SetGrownPoint` `PET_SetTamePoint` `PET_SetUpgradePoint`
`File_Create` `IniFile_Save` `IniFile_SetData` `FormatTime2Date` `Tm2Time` `TrimString`
`_ERRORMESSAGE` `execute`.

Điểm chèn: mục **8.2** ở trên.

---

### ③ 85 tệp `.lua` bao đóng còn thiếu  `[da sua theo phan bien]`
* **69 tệp phải chép/dịch thật** từ `[LINUX]`.
* **15 tệp chỉ sai đường dẫn** — đã nằm ở `[LIVE] scriptjx2\`, cần chép sang `script\` hoặc sửa Include:
  `lib\basic.lua` `lib\string.lua` `lib\file.lua` `lib\droptemplet.lua` `lib\sharedata.lua`
  `lib\coordinate.lua` `lib\mem.lua` `lib\composeclass.lua` `lib\composelistclass.lua`
  `lib\progressbar.lua` `tong\tong_header.lua` `tong\tong_setting.lua` `tong\log.lua`
  `tong\tong_award_head.lua` `tong\contribution_entry.lua`
* **1 tệp không có ở đâu cả**: `global\路人_礼官.lua` — `[LINUX] missions\fengling_ferry\boss.lua:11` Include nó,
  nhưng `[LINUX] script\global\` **không có tệp nào tên phi-ASCII** ⇒ Include treo ngay trong BẢN GỐC, phải bỏ hoặc viết lại.  `[da sua theo phan bien]`
  (Bản cũ ghi `class\ktabfile.lua` + `class\lerror.lua` là **SAI** — cả hai **CÓ** ở `[LINUX] script\class\`;
  `killer\nieshichen.lua:54` `new(KTabFile,"/settings/task/tollgate/killer/killer.txt","KILLER")` chính là chỗ cần `ktabfile.lua`.)
* Các cụm lớn còn thiếu: `battles\` (6 tệp), `event\change_destiny\` (2), `task\metempsychosis\` (4),
  `missions\boss\bigboss.lua`, `global\signet_head.lua`, `global\judgeoffline_limit.lua`,
  `vng_event\thapnienlenhbai\` (2), `misc\vngpromotion\ipbonus\ipbonus_2_head.lua`,
  `bonusvlmc\head.lua`, `item\battles\songjinskill.lua`, `missions\yandibaozang\head.lua`.

---

### ④ Script LÕI của 3 tính năng — **34 tệp**
| Tính năng | Thư mục Linux | Số tệp |
|---|---|---|
| Săn boss sát thủ | `task\tollgate\killer\` | **5** |
| Phong Lăng Độ | `missions\fengling_ferry\` | **13** `.lua` (+2 `.bak` ⇒ 15 tệp trong thư mục) |  `[da sua theo phan bien]`
| Vượt ải | `missions\challengeoftime\` | **16** |

Đây là phần **phải dịch nghiệp vụ**, không chép máy móc được: mã hoá GBK/TCVN3, tên hàm engine lệch,
`AddNpcEx` khác thứ tự tham số, `GetTeamMember` khác quy ước.

---

### ⑤ Bảng dữ liệu `settings\` còn thiếu
| Thư mục | Linux | JX1 |
|---|---|---|
| `settings\maps\challengeoftime\` (lineup8/16/20/24/32/40/56.txt) | **7 tệp** | **KHÔNG CÓ** |
| `settings\task\tollgate\killer\killer.txt` | **1 tệp** | **KHÔNG CÓ** |
| `settings\droprate\boss\` (`bosstask_lev20..90.ini` + phụ trợ) | **57 tệp** | **1 tệp** |
| `settings\maps\liandandong\` (`npc_1.txt` `npc_2.txt` `npc_3.txt` `player.txt` `trap_1.txt`) | **5 tệp** | **KHÔNG CÓ CẢ THƯ MỤC** |  `[da sua theo phan bien]`
| `settings\maps\中原北区\渡船\渡船刷怪点.txt` — điểm spawn thuỷ tặc, `fengling_ferry\fld_head.lua:18` | 🔴 **KHÔNG CÓ Ở CẢ HAI CÂY** (`[LINUX] settings\maps\` chỉ có 29 thư mục con, đều tên ASCII) ⇒ **phải dựng lại bảng** | **KHÔNG CÓ** |  `[da sua theo phan bien]`

---

### ⑥ Quyết định kiến trúc: **giữ bản VN hay thay bằng bản JX2** (Phong Lăng Độ + Vượt Ải)
Không phải viết code nhiều nhưng là **rủi ro cao nhất**:
* Đụng **mission id 3 và 4** (`lib_task.lua:284-285`) — bản Linux muốn `MISSION_MATCH = 22`.
* Đụng **map 336-339 và 480-489** — hai bản dùng chung map, spawn NPC chồng nhau.
* Đụng **timer id 1, 2, 4, 5, 6** — bản VN đã chiếm.
* Đụng **`timerserver.lua`** — phải chọn gọi `sukien_*` (VN) hay `OpenGlbMission`/`AddTimer` (JX2).
* Đụng **`\script\lib\lib_task.lua`** (task id lưu tiến độ ngày).
Ba lựa chọn: (a) bật lại bản VN, bỏ port; (b) thay hẳn bằng bản JX2; (c) chạy song song trên
mission/map/timer id **khác nhau**. Phải chốt trước khi viết dòng đầu tiên.

---

### ⑦ Nối dây bảng cấu hình (nhỏ nhưng **quên là chết im lặng**)
1. `[LIVE] settings\task\missions.txt`: thêm dòng cho mission id của 3 tính năng
   (Vượt ải cần **22**; hiện dòng 23 trỏ `mission_trong.lua`). Định dạng: `<id>\t<đường dẫn script>`.
2. `[LIVE] settings\TimerTask.txt`: thêm **28, 29** (Phong Lăng Độ) và **41, 42, 43** (Vượt ải),
   trỏ tới `\script\missions\...\timer_*.lua` / `fld_smalltimer.lua` / `fld_landingtimer.lua`.
   🔴 Không có dòng này thì `KTimerTaskFun::Activate` lấy tên script **rỗng** ⇒ timer nổ mà **không gọi gì cả**.
3. `timerserver.lua`: bỏ chú thích dòng 79/80 (nếu giữ bản VN) hoặc thêm điểm gọi mới.
4. `startgame.lua`: thêm điểm gọi đặt NPC (như `tinsu_addnpc()` đã làm).
5. `addtollgatenpc.lua`: thêm `add_killertasknpc(addkillertasknpc)` cho 160 boss sát thủ.

---

### ⑧ Mở rộng `g_IsJx2Script` + rà ngữ nghĩa lệch
`[SRC] Core\Src\KSortScript.cpp:115-123` — thêm 2 tiền tố:
`"\\script\\missions\\fengling_ferry\\"`, `"\\script\\missions\\challengeoftime\\"`.
Ảnh hưởng tới 4 hàm có **ngữ nghĩa khác nhau giữa JX1 và JX2**:
`GetTeamMember` (`ScriptFuns.cpp:7505`), `SetPunish` (`12811`), `AddSkillState` (`6929`), `GetGameTime` (`13087`).
Không thêm ⇒ lỗi **âm thầm, sai kết quả**, không crash.

---

### ⑨ Vá 3 khiếm khuyết của hệ mission JX1 (mục 1.5 + 2)
1. `SetMissionV` tràn `char[16]` — `KMission.h:334-339`, `KMissionArray.h:36-41` (`strcpy` không kiểm).
2. `LuaStartMissionTimer` rò khe timer khi gọi trùng id — `ScriptFuns.cpp:11363-11391`
   (mẫu đúng: `LuaStartGlbMSTimer` `KJx2League.cpp:974-982`).
3. `OnTimer` **không nhận timer id** — `KMission.cpp:362`. Script Linux dựa vào bảng
   `TimerTask.txt` để tách script theo id nên **tạm chấp nhận được**, nhưng nếu muốn một
   `mission.lua` xử lý nhiều timer thì phải truyền id.
> ⚠ Cả 3 đều là **thay đổi hành vi engine đang chạy** — phải phản biện riêng, không gộp vào đợt port.

---

### ⑩ Bảng xếp hạng: mở rộng ladder + đường phát sự kiện
* Ladder hiện **TOP 10 cứng**, id > 10000, **file cục bộ 1 máy chủ**
  (`KJx2SharedStore.cpp:440-441, 574-575`). Nếu `rank_perday` (Vượt ải) hay bảng tuần
  (`battles\weeklyrank.lua`) cần > 10 mục / điểm cá nhân ngoài top-10 / liên server
  ⇒ **phải mở rộng `KJx2SharedStore`** (hoặc làm thật trong S3Relay — hiện S3Relay **không có gì**).
* `activitysys\ladderfunlib.lua` đã dùng đúng API (`Ladder_GetLadderInfo`, `Ladder_NewLadder`) ⇒ không phải sửa.
* Đường **phát sự kiện** `EventSys:GetType("KillerBoss"):OnPlayerEvent("OnFinish", …)` v.v.
  phải viết trong script tính năng — khung `NewType` đã có sẵn (`misc\eventsys\type\func.lua`).

---

## PHỤ LỤC A — Tệp tạm của phiên (dùng lại được)

| Tệp | Nội dung |
|---|---|
| `…\scratchpad\jx1_funcs.txt` | tên hàm Lua đang đăng ký trong `GameScriptFuns[]` (số thật: **1.005** dòng / **1.002** tên duy nhất) |
| `…\scratchpad\diff_funcs.py` | so tên hàm bao đóng Linux ↔ JX1 |
| `…\scratchpad\core_diff.py` | so riêng 3 thư mục LÕI |
| `…\scratchpad\check_defs.py` | phân biệt hàm Lua-định-nghĩa ↔ hàm engine |
| `…\scratchpad\luamap_check.py` | tra địa chỉ ELF của hàm engine |

(`…` = `C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\0064b491-1b33-4139-acb8-6928fda45bcd`)

## PHỤ LỤC B — Những điều **CHƯA XÁC MINH**

1. Ba tên `ContriValueEntryLogic`, `JudgePLAddTitle`, `Require` **không có** trong
   `jx_linux_y.luamap.full.txt` và cũng không phải hàm Lua — chưa rõ là hàm engine không xuất hiện
   trong bảng, hay là tàn dư mã chết.
2. Số script thực sự được nạp lúc chạy so với trần `MAX_SCRIPT_IN_SET = 5000`
   (`[SRC] KSortScript.h:13`) — cây `[LIVE]` có **2.811** `.lua` trong `script\` + **189** trong
   `scriptjx2\`, nhưng nạp là **lười (lazy)** nên số chiếm khe thật chưa đo được.
3. Chưa so nội dung từng byte của 39 tệp mà `closure3.json` đánh dấu `in_jx1 = true`
   (trùng đường dẫn **không** bảo đảm trùng nội dung) — mới so 6 tệp `activitysys` (đều giống hệt).
4. `settings\maps\liandandong\npc_3.txt` và tệp bảng tên tiếng Trung
   `settings\maps\中原地图\…` mà `killer\` tham chiếu — chưa đối chiếu với `[LIVE]`.
5. Đơn vị và ngữ nghĩa `SetNpcTimer` khi dùng cho boss hồi sinh của Phong Lăng Độ
   (`lib_phonglangdo.lua:213`) — chưa đọc mã C.

---

## PHU LUC PHAN BIEN (tac tu doc lap)

Ngày phản biện: **24/08/2026**. Người phản biện **không** viết bản gốc; mọi dòng dưới đây đều
mở tệp gốc / nhị phân ra đọc lại, mặc định coi khẳng định là SAI cho tới khi tệp gốc chứng minh ngược lại.
Đã kiểm chứng **31 khẳng định đánh số** (cộng ~20 khẳng định phụ gộp ở cuối mục A):
**10 dòng kết luận SAI**, chứa tổng cộng **12 lỗi riêng biệt** — tất cả đã sửa trong thân bài
và đánh dấu `[da sua theo phan bien]` (18 dấu). Thêm **6 chỗ bỏ sót** ở mục B.

### A. Bảng kiểm chứng

| # | Khẳng định (bản gốc) | Bằng chứng gốc | KẾT LUẬN | Sửa lại thành |
|---|---|---|---|---|
| 1 | `ScriptFuns.cpp` = 427.894 byte, **15.349 dòng**, sửa 23/08 16:03:05 | `stat` → `427894  2026-08-23 16:03:05`; số ký tự `\n` = **15348**, tệp KẾT THÚC bằng newline ⇒ `splitlines()` = 15348 | **SAI** (byte + giờ ĐÚNG) | **15.348 dòng** |
| 2 | `GameScriptFuns[]` đang đăng ký **1.007** hàm | Quét khối dòng 14257-15323 bằng mẫu `{"Ten", Lua…}` (đã bỏ `//` và `/*…*/`, dung sai xuống dòng) = **1005**; không mục nào có con trỏ hàm mà thiếu tên | **SAI** | **1.005** dòng đăng ký, **1.002** tên duy nhất (`BT_GetData`, `GetLeadLevel`, `GetMissionS` mỗi cái 2 lần) |
| 3 | Mốc chèn: 14255 `#endif // _SERVER (khoi ham WLLS)`; 14257 mở bảng; 15315 `#else`; 15318 `#endif`; 15323 `};`; 15325-15338 `WorldScriptFuns[]`; 15340-15343 `g_GetGameScriptFunNum()` | `sed -n '14250,14262p'` và `'15315,15345p'` — **khớp từng dòng một** | **ĐÚNG** | — |
| 4 | Cụm `[PORT5 23/08]` ở **15241-15268**, `#endif` ở **15269** | `sed -n '15226,15275p'`: 15241 = dòng chú thích `[PORT5 23/08]`, 15268 = `NPCINFO_SetNpcCurrentLife`, 15269 = `#endif` | **ĐÚNG** | — |
| 5 | Ladder đăng ký ở `ScriptFuns.cpp:15085/15086/15087`; thân `KJx2SharedStore.cpp:531/580/599` | `sed -n '15080,15090p'` khớp; `grep -n "^int LuaLadder_"` → 531 / 580 / 599 | **ĐÚNG** | — |
| 6 | `JX2LADDER_TOP=10` (:440), `JX2LADDER_MIN_ID=10000` (:441, kiểm :538/:585), NS `"jx2.ladder"` (:443), `MoveFileEx` (:524), cắt top ở :574-575, ô trống trả `("",0,-1,0)` (:622-626) | `grep -n` trên `KJx2SharedStore.cpp` + `sed -n '615,630p'` — **khớp toàn bộ** | **ĐÚNG** | — |
| 7 | S3Relay chỉ có 4 hàm Lua (`DoScript.cpp:272-278`); 0 kết quả `Ladder` trong **197 tệp** `*.{cpp,h}` | Bảng 4 mục đúng (271-277). Nhưng đếm thật: `.cpp/.h` = **109** tệp, tổng mọi tệp = **197**; grep `-il ladder` trên 109 tệp nguồn = **0** | **SAI (số tệp)** — kết luận "0 hit" **ĐÚNG** | **109 tệp** `.cpp/.h` (197 = tổng mọi tệp kể cả `.pdb/.obj`) |
| 8 | `IncludeLib("RELAYLADDER")` hợp lệ, bảng module `ScriptFuns.cpp:2485-2503`, trỏ `scriptjx2\lib\noop.lua` | `sed -n '2483,2505p'`: `szMod[21]` mở ở 2485, `szFile[21]` đóng ở 2503; RELAYLADDER là phần tử thứ 17 (chỉ số 16) ↔ `noop.lua` | **ĐÚNG** | — |
| 9 | Ladder id đang dùng thật: 10118, **10119**, 10187, 10196, 10197, 10250, 10251, 10261, 10999 | Quét mọi điểm gọi `Ladder_*` trong `[LIVE] script\` + `scriptjx2\`: **không có 10119** (chỉ khớp `110119` trong `npcpos.lua`); **10187** cả 4 lần đều là dòng `--` (`posthouse.lua:111,415,447,549`); **10999** và một phần **10261** chỉ có ở `script\test\`; **thiếu 10182-10186** (`event\storm\head.lua:55-59`, `function.lua:240,242,428,430`), **10225/10226** (`event\tongwar\score.lua:72-76`), **10227/10228+** (`headinfo.lua:136-137`) | **SAI** | Danh sách mới ở mục 6.4 |
| 10 | `MAX_TIMER_PERMISSION=10` (`KMission.h:23`) ⇒ thực dùng **9** vì `m_ulTotalSize = ulSize - 1` (`KLinkArrayTemplate.h:82`) | `grep -n` + `sed -n '78,88p'` — khớp chính xác cả hai | **ĐÚNG** | — |
| 11 | `MAX_MISSIONARRAY_VALUE_COUNT=100`, ô `char[16]` (`KMissionArray.h:7,13-14`); `MAX_NPC_MISSION=5000`; `MAX_PLAYER=1500` (`KPlayerDef.h:19`); `MISSION_STATNUM=10` (`GameDataDef.h:91`); `MAX_MISSION_PARAM=18` (`:87`) | Đọc thẳng 4 header — **khớp hết**. `MAX_PLAYER 1500` nằm trong `#ifdef _SERVER` (client = 2) | **ĐÚNG** | — |
| 12 | Tràn `strcpy` không kiểm độ dài ở `KMission.h:334-339` và `KMissionArray.h:36-41` | `KMissionArray.h:40` = `strcpy(m_MissionValue[ulValueId], szValue);`, chỉ chặn `ulValueId >= 100`; `KMission.h:338` y hệt | **ĐÚNG** | — |
| 13 | `LuaInitMission` tra `g_MissionTabFile` tại **dòng missionId+1, cột 2**, gọi `InitMission` nếu có, không thì `BeginMission` | `ScriptFuns.cpp:11156` = `g_MissionTabFile.GetString(nMissionId + 1, 2, "", szScript, MAX_PATH);`; khối chọn tên hàm ở 11160-11178 | **ĐÚNG** (thân thật 11120-11182, bản gốc ghi 11184 — lệch 2 dòng trắng) | — |
| 14 | BẪY 1: `LuaStartMissionTimer` (11363-11391) chỉ `Add()` ở **11382**, không dò timer trùng id; `LuaStartGlbMSTimer` (`KJx2League.cpp:974-982`) thì CÓ dò | `sed -n '11360,11395p'` — 11382 đúng là `m_cTimerTaskSet.Add()`, không có vòng tìm trùng. `KJx2League.cpp:974-982` đúng là vòng `for` so `(nMissionId,nTimerId)` rồi ghi đè | **ĐÚNG** | — |
| 15 | BẪY 2: `KTimerTaskFun::Activate` (117-132) tra `TimerTask.txt` theo timerId; `g_MissionTimerCallBackFun` (`KMission.cpp:338-366`) gọi `OnMissionTimer`, không có thì `OnTimer`, tham số 0 (`:362` → `CallFunction(...,0,"d",nParam)` `:97`) | Đọc `KTaskFuns.cpp:117-132` + `KMission.cpp:338-366` + `:97` — **khớp từng dòng**. **Bổ sung**: tra bảng là **theo KHOÁ**, không theo chỉ số dòng (`KTaskFuns.cpp:184-185`) ⇒ thứ tự dòng trong `TimerTask.txt` KHÔNG quan trọng | **ĐÚNG** | — |
| 16 | Timer mission **tự lặp lại** sau mỗi lần nổ (`KTaskFuns.cpp:122`) | `KTaskFuns.cpp:122` = `m_dwTimeTaskTime = g_SubWorldSet.GetGameTime() + m_dwIntervalTime;` | **ĐÚNG** | — |
| 17 | Glb timer kẹp `[18 … 31.104.000]` (`KJx2League.cpp:967-971`) | `sed -n '963,985p'`: 967 `if (!(fFrames >= 18))`, 969 `if (fFrames > 31104000.0)` | **ĐÚNG** | — |
| 18 | `AddTimer/DelTimer/SuspendTimer/ResumeTimer` đăng ký `ScriptFuns.cpp:15254-15257` | Đếm từ 15226: AddTimer=15254, DelTimer=15255, SuspendTimer=15256, ResumeTimer=15257 | **ĐÚNG** | — |
| 19 | `settings\TimerTask.txt` **36 dòng**; id đã dùng `1-10,12-18,20,21,50-55,61,62,65-70,75-77`; **28,29,41,42,43 TRỐNG** | Đọc thật: 37 dòng vật lý = 1 header `TASK/SCRIPT` + **36 dòng dữ liệu**; tập id khớp **chính xác**; 28/29/41/42/43 không có | **ĐÚNG** | — |
| 20 | `settings\task\missions.txt` **45 dòng**; id 13-23, 27-32, 34-36, 41-43 = `mission_trong.lua`; **id 22 TRỐNG ở dòng 23** | Đọc thật: 46 dòng vật lý = 1 header + **45 dòng mission**; dòng 23 = `22` → `mission_trong.lua`; các dải trống khớp hết | **ĐÚNG** | — |
| 21 | Linux: `MISSION_MATCH=22`, `TIMER_MATCH/BOARD/CLOSE=41/42/43` (`challengeoftime\include.lua:6-9`); `CHUANGGUAN30_MAP_ID=957` (:27); `VARV_BATCH_MODEL=33`; `VARS_*` = 1,2,11,12 (:50-54) | Đọc `include.lua` — 6/7/8/9/17/27/50-54/68/70/73/75-76 khớp; tính lại: `VARV_PLAYER_SEX=13` → `VARV_PLAYER_SERIES=22` → `VARV_LEVEL=31` → `VARV_BATCH_MODEL=33` | **ĐÚNG** | — |
| 22 | Linux `fengling_ferry` dùng timer id **28, 29** (`mission.lua:15-16,57-58`); ô mission cao nhất = `MS_TIMEACC_20SEC = 3` (`fld_head.lua:28-30`) | Đọc thật — khớp cả 4 vị trí dòng | **ĐÚNG** | — |
| 23 | Số tệp lõi: killer **5**, fengling **13 (gồm 2 .bak)**, challengeoftime **16** ⇒ tổng 34 | `ls`: killer = 5 `.lua`; challengeoftime = **16** `.lua` (13 gốc + `item\` + `npc\`); fengling = **15 tệp** = **13 `.lua` + 2 `.bak`** ⇒ ".bak nằm TRONG 13" là sai | **SAI (diễn đạt)** — tổng 34 vẫn đúng | 13 `.lua` **cộng thêm** 2 `.bak` (15 tệp trong thư mục) |
| 24 | `maplist.ini`: 336 @2915, 337 @2932, 480 @3759, 489 @3786, **957 @6233** | Đọc `maplist.ini` (6471 dòng): 2915 = `336=…风陵渡`, 2932 = `337=…渡船`, 3759 = `480=…杀手的试炼`, 3786 = `489=…`, **6233 = `957=特殊用地\安期炼丹洞`** | **ĐÚNG** | — |
| 25 | `activitysys` 6/6 tệp **GIỐNG BYTE-BY-BYTE** Linux; Linux có **809** `.lua`; `detailtype\` **32 tệp**; JX1 thiếu 803/809; `LoadActivitys` @`g_activity.lua:24`, `AddActivity` @`:114`, **không ai gọi** | So nhị phân 6/6 = IDENTICAL; đếm đệ quy Linux = **809**; `detailtype` = **32**; `g_activity.lua` có 118 dòng, `:24` và `:114` khớp; grep toàn `[LIVE]` chỉ ra định nghĩa, **0 điểm gọi ngoài** | **ĐÚNG** | — |
| 26 | `[REPO-JX2] serverscript_jx2\` không có `activitysys` (**chỉ `lib`, `tong`, `tong_vn`**) | `ls D:\GAMEDEVNEW\serverscript_jx2` = **25 mục cấp 1** (bairenleitai, boot, bw, citywar_e, datau_tasklink, … tongwar, trap_fix). Không có `activitysys` — đúng | **SAI (vế sau)** | Kho có 25 mục cấp 1 |
| 27 | `eventsys\type\func.lua` khai báo `ChuanGuan` 10-14, `FengLingDu` 22-28, `KillerBoss` 33-37; `CheckKillerdBoss`/`CheckBoatBoss`/`Chuanguan` = 0 kết quả trong `[LIVE]` | Đọc `func.lua` — **khớp chính xác 3 khối**. `grep -rni` toàn `[LIVE] script\` + `scriptjx2\` chỉ trả về đúng 1 dòng `func.lua:10` | **ĐÚNG** | — |
| 28 | `g_IsJx2Script` so **tiền tố** đường dẫn với mảng cứng `KSortScript.cpp:116-122`; `m_szScriptName` cắt 100 ký tự (`:161`) | `KSortScript.cpp:113-133`: mảng ở 115-123, 18 mẫu ở 116-122; `:161` `g_StrCpyLen(...,100)` — **nhưng** `:130` là `strstr(szName, szJx2[i])`, tức khớp **chuỗi con ở bất kỳ vị trí**, không phải so tiền tố | **SAI (mô tả cơ chế)** — số dòng ĐÚNG | `strstr` = khớp chuỗi con |
| 29 | Tổ đội: `GetTeamMem` @14367 + alias `GetTeamMember` @14292, `sJx2TeamMember` 7480-7498 (n=1 → đội trưởng); `GetTeamId`, `LeaveTeam` @14366 | 7480 `static int sJx2TeamMember(...)`, `nPos==1` trả `m_nCaptain`; 14292 alias; 14367 `GetTeamMem`. **Nhưng** 14364 = `{"GetTeam", LuaGetTeamId}` — tên Lua là **`GetTeam`**, còn 14366 = `LeaveTeam` | **SAI (tên + số dòng)** | `GetTeam` @14364; `LeaveTeam` @14366 |
| 30 | Bao đóng `closure3.json`: **84** tệp duy nhất không có ở JX1 (satthu 28 / phonglangdo **43** / vuotai 54); **15** đã ở `scriptjx2\`; **2** không có ở đâu (`class\ktabfile.lua`, `class\lerror.lua`) | Đọc lại `closure3.json`: satthu **28** đúng, phonglangdo **44** sai, vuotai **54** đúng, hợp nhất = **85** sai. 15 tệp ở `scriptjx2\` — **danh sách khớp 15/15**. `class\ktabfile.lua` và `class\lerror.lua` **ĐỀU CÓ** ở `D:\ServerLinux\server1\script\class\`; tệp duy nhất không tồn tại ở đâu là `global\路人_礼官.lua` (`fengling_ferry\boss.lua:11`) | **SAI (3 con số)** | 85 duy nhất / phonglangdo 44 / 1 tệp "không có ở đâu" |
| 31 | Phong Lăng Độ + Vượt Ải bản VN **đang TẮT** (`timerserver.lua:79-80`); `MS_VUOTAI=3`, `MS_PLANGDO=4` (`lib_task.lua:284-285`); timer PLĐ 4/5, VA 1/2/6 | `timerserver.lua:79` `-- sukien_vuotai(nHr,nMi)`, `:80` `-- sukien_phonglangdo(nHr,nMi)`; `lib_task.lua:284/285`; `lib_phonglangdo.lua:267,274` (id 4, 5); `sugiasatthu.lua:219` (id 1), `die_boss.lua:36` (id 6) — đều đúng. **NHƯNG** `startgame.lua:100` gọi `addnpcphonglangdo()` **KHÔNG chú thích** ⇒ 6 NPC Thuyền phu (`lib_phonglangdo.lua:147-157`) vẫn spawn mỗi lần khởi động | **SAI một phần** | "TẮT" chỉ đúng với **lịch**, không đúng với **NPC** |

**Các khẳng định khác cũng đã mở tệp kiểm và ĐÚNG** (không lập dòng riêng để bảng khỏi dài):
`killbosshead.lua` = 384.819 B (= 375,8 KiB), bảng `addkillertasknpc` dòng **4-180** đúng **160 dòng**,
`add_killertasknpc(Tab3)` ở **183-194** có `SetNpcScript` + `SetNpcParam(...,1,mã)` và nhân toạ độ ×32 ở 187-188;
`addtollgatenpc.lua` chỉ gọi `AddNpc_turesureboss` / `turesurebug` / `allbugbear`, dòng 12-15 và 18 bị chú thích;
`script\lib\droptemplet.lua` KHÔNG có mà `scriptjx2\lib\droptemplet.lua` CÓ (`boss.lua:15` Include);
`awardtype\` có đúng 5 kiểu và 3 tính năng **chỉ** dùng `tbProp` / `nExp` / `nExp_tl` (quét toàn bộ 34 tệp lõi);
`settings\droprate\boss\` 57 so với 1; `settings\maps\challengeoftime\` 7 so với không có; `killer.txt` có so với không có;
`MAX_NPC = 98000` (`KNpc.h:21`); `AddNpc` thân 6814, `AddNpcEx` chú giải 6846-6873 / thân 6874;
5 hàm engine lõi (`AddStatData` `ITEM_DropRateItem` `NPCINFO_GetSeries` `ITEM_SetExpiredTime` `NpcName2Replace`)
**đều 0 lần xuất hiện** trong `ScriptFuns.cpp` và **đều có địa chỉ ELF** trong `jx_linux_y.luamap.full.txt`,
còn `GetNpcSeries` thì CÓ; `ContriValueEntryLogic` / `JudgePLAddTitle` / `Require` **không** có trong luamap;
khối đăng ký mission 14738+ khớp `GetMissionName`@14749, `StartMissionTimer`@14750, `StopMissionTimer`@14751,
`GetMSRestTime`@14752, `IsMission`@14754; `startgame.lua:107` đúng là `BairenLeitai_Init`;
`functionlib.lua:316` đúng là `lib:DoFunInWorld`.

---

### B. Bỏ sót đã tìm thêm

#### B1. JX1 **ĐÃ CÓ MỘT HỆ "BOSS SÁT THỦ" BẢN VIỆT NAM NẰM NGAY TRONG CÂY** — báo cáo không hề nhắc

Mục 9.2 kết luận "Săn boss sát thủ" chỉ có `killbosshead.lua` và thiếu tất cả phần còn lại. Sai bối cảnh:

* `[LIVE] script\tinhnang\boss_satthu\` — **3 tệp**: `lib_boss_st.lua` (2.080 B), `death.lua` (2.738 B), `drop.lua` (1.316 B).
* `lib_boss_st.lua:1-3` ghi tác giả "Fong Kiều — 07/2021 — Lib Boss sát thủ"; **`BOSS_SATTHU` dòng 7-29 = 20 boss**
  (`{stt, tên, mapId, x, y, npcId}`), hàm `addnpcbosssatthu()` dòng **34-42** dùng `AddNpcEx3` + `SetNpcValue`.
* Đã được nối dây ở **4 chỗ**: `startgame.lua:9` (Include), `global\npcchucnang\nhieptran.lua:5`,
  `item\lenhbaiadmin.lua:15`, và **`tinhnang\vuot_ai\sugiasatthu.lua:5`** (Vượt Ải bản VN dùng lại chính lib này).
* Còn có `[LIVE] script\startgame\khac\satthu.lua` — bản đặt NPC cũ hơn (`AddNpcNew`), **toàn bộ đã chú thích**.
* Trạng thái: `addnpcbosssatthu()` ở **`startgame.lua:99` đang bị chú thích** ⇒ NPC không spawn, nhưng
  lib vẫn được nạp và 3 tệp còn lại vẫn sống.

**Hệ quả cho kế hoạch port**: `BOSS_SATTHU` dùng **npcId 768, 812-820** — trùng đúng dải npcId mà
`killbosshead.lua` dùng cho boss cấp 90 (`{818,…}`, `{819,…}`, `{820,…}` ở dòng 177-179), và trùng cả
tên tiếng Việt ("Cổ Thủ Đằng", "Tăng Chỉ Oán", "Vệ Biên Thành", "Trang Minh Trung", "Gia Cát Kinh Hồng",
"Đoạn Lăng Nguyệt", "Tả Dật Minh", "Nhậm Thương Khung"…). Bật `add_killertasknpc(addkillertasknpc)` (160 boss)
**cùng lúc** với `addnpcbosssatthu()` (20 boss) sẽ ra **hai bộ boss trùng tên, trùng id, khác máu, khác drop**
trên cùng bản đồ. Mục ⑥ "Quyết định kiến trúc" mới chỉ nêu va chạm của Phong Lăng Độ / Vượt Ải —
**phải bổ sung Săn boss sát thủ vào đúng mục đó**.

#### B2. Phong Lăng Độ bản VN **KHÔNG tắt hoàn toàn** — NPC vẫn spawn mỗi lần khởi động

`[LIVE] script\startgame.lua:100` gọi **`addnpcphonglangdo()` không chú thích** (đối chiếu: dòng 99
`-- addnpcbosssatthu()` và dòng 101 `--addnpctongkimtrungcap()` thì CÓ chú thích).
`lib_phonglangdo.lua:147-157` đặt **6 NPC "Thuyền phu Nam/Bắc ất-bính-giáp"** (npcId 240) theo bảng
`TAB_NPCCHUCNANG`. Nghĩa là map 336/337 **đang có NPC sống**; port bản JX2 lên cùng map sẽ chồng NPC
ngay từ giây đầu, không cần chờ tới lúc mở sự kiện. Bảng 9.3 đã được sửa lại.

#### B3. Bảng dữ liệu Phong Lăng Độ **không tồn tại ở CẢ HAI cây** — không thể "chép sang"

`[LINUX] missions\fengling_ferry\fld_head.lua:18`:

```
npcthiefpos = "\\settings\\maps\\中原北区\\渡船\\渡船刷怪点.txt"
```

Đây là bảng điểm spawn thuỷ tặc. Kiểm thật:
`D:\ServerLinux\server1\settings\maps\` có **29 thư mục con và tất cả đều tên ASCII**
(`bianjingdigong challengeoftime championship … yuegedao`) — **không có `中原北区`**.
`[LIVE] settings\maps\` chỉ có 9 mục. Vậy bảng này **phải dựng lại từ đầu**, không phải việc "chép tệp".
Mục ⑤ đã được bổ sung dòng này.

Cùng lúc, mục ⑤ ghi `settings\maps\liandandong\npc_3.txt` là "CHƯA XÁC MINH" — nay đã xác minh:
`[LINUX]` có **5 tệp** (`npc_1.txt` `npc_2.txt` `npc_3.txt` `player.txt` `trap_1.txt`),
`[LIVE]` **không có cả thư mục**. (`challengeoftime\npc\` dùng `npc_3.txt` để chọn điểm rơi cho
`NewWorld(CHUANGGUAN30_MAP_ID, …)`.)

#### B4. Hai `IncludeLib` của script lõi **KHÔNG có trong bảng module của engine** — và engine **bỏ qua IM LẶNG**

Mục 6.2 chỉ kiểm `RELAYLADDER`. Quét toàn bộ 34 tệp lõi thì có **5** tên `IncludeLib` khác nhau:

| Tên | Điểm gọi (Linux) | Có trong `szMod[21]`? |
|---|---|---|
| `ITEM` | `killer\kill_level.lua:8`, `fengling_ferry\boss.lua:5` | CÓ |
| `LEAGUE` | `fengling_ferry\boss.lua:6` | CÓ |
| `RELAYLADDER` | `killer\kill_level.lua:7`, `challengeoftime\include.lua:1` | CÓ |
| **`NPCINFO`** | **`killer\kill_level.lua:9`** | **KHÔNG** |
| **`TASKSYS`** | **`fengling_ferry\boss.lua:9`** | **KHÔNG** |

`LuaIncludeLib` (`ScriptFuns.cpp:2480`) — chú thích ngay trên hàm ở **`:2479`**:
"module không có trong bảng thì bỏ qua im lặng"; vòng lặp là `for (int k = 0; k < 21; k++)` (**`:2511`**).
Vậy `IncludeLib("NPCINFO")` và `IncludeLib("TASKSYS")` sẽ **no-op**, script chạy tiếp rồi **chết ở lời gọi
hàm đầu tiên** (`NPCINFO_GetSeries`, các hàm `TASKSYS`) — đúng kiểu lỗi âm thầm mà mục ⑧ cảnh báo.

**Phải sửa 3 chỗ cùng lúc** (không sửa đủ là lệch chỉ số mảng): `szMod[21]` (`:2485`),
`szFile[21]` (`:2494`), và trần vòng lặp `k < 21` (`:2511`). Mục ② và ⑩ nên bổ sung việc này.

#### B5. Tra bảng `TimerTask.txt` là **theo KHOÁ**, không theo chỉ số dòng (làm nhẹ mục ⑦)

`KTaskFuns.cpp:184-185`:

```
sprintf(szTaskId, "%d", usTimerTaskId);
m_TimerTaskTab.GetString(szTaskId, "SCRIPT", "", szScriptFileName, nScriptFileLen);
```

Khác hẳn `missions.txt` (tra theo **dòng** `missionId + 1`, `ScriptFuns.cpp:11156`).
Vậy thêm id 28/29/41/42/43 vào `TimerTask.txt` **có thể nối vào cuối tệp**, không cần chèn đúng thứ tự —
đúng như tệp hiện tại đang làm (id 50,51,52 nằm ở dòng 19-21, id 53 ở dòng 31, id 20/21 ở dòng 35-36).

#### B6. `class\ktabfile.lua` không phải "không có ở đâu" — nó là bắt buộc, và đường dẫn dùng dấu `/`

`[LINUX] task\tollgate\killer\nieshichen.lua:54`:

```
killertabfile = new(KTabFile,"/settings/task/tollgate/killer/killer.txt","KILLER")
```

Tệp lớp `KTabFile` **có sẵn** ở `[LINUX] script\class\ktabfile.lua` (cùng `lerror.lua`, `ktabex.lua`,
`ltabfile.lua`, `kbonus.lua`, `kbook.lua`, `worthanalyse.lua`, `writelog.lua`) — chỉ **thiếu ở `[LIVE]`**.
Lưu ý phụ: đường dẫn ở đây dùng **dấu `/`** chứ không phải `\\` như phần còn lại của cây —
khi port cần kiểm hàm nạp bảng của JX1 có chấp nhận `/` hay không (**CHƯA XÁC MINH**).

---

### C. Ba điều bản gốc ghi "CHƯA XÁC MINH" — nay đã xác minh

1. `ContriValueEntryLogic`, `JudgePLAddTitle`, `Require` **thật sự không có** trong
   `jx_linux_y.luamap.full.txt` (grep `-w` trả rỗng cho cả ba). Giữ nguyên kết luận của bản gốc.
2. `settings\maps\liandandong\` — đã xác minh, xem **B3**.
3. Bảng tên tiếng Trung mà `fengling_ferry` tham chiếu — đã xác minh **không tồn tại**, xem **B3**.

Còn lại **vẫn CHƯA XÁC MINH**: so byte 39 tệp `in_jx1 = true` của `closure3.json` (mới so 6 tệp
`activitysys`); số khe `MAX_SCRIPT_IN_SET` thật lúc chạy; ngữ nghĩa `SetNpcTimer` cho boss hồi sinh;
hàm nạp bảng của JX1 với đường dẫn dấu `/` (B6).
