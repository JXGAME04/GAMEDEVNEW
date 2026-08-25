# VƯỢT ẢI (chuangguan / challengeoftime) — dịch ngược đầy đủ bản Linux

Nguồn: `D:\ServerLinux\server1` (JX2/Kiếm Thế Linux, Lua 4.0). Đối chiếu dự án đích JX1:
`D:\GAMEDEVNEW\Sources` (engine) và máy chủ đang chạy
`E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server` (script + settings sống).

Mọi khẳng định dưới đây đều kèm `đường-dẫn:số-dòng`. Chỗ nào chưa chắc đều ghi rõ
**CHƯA XÁC MINH**.

---

## 0. Bộ tệp của tính năng

### 0.1 Tệp lõi (bản Linux)

| Tệp | Vai trò |
|---|---|
| `script\missions\challengeoftime\include.lua` | Hằng số, bảng cấu hình, hàm dùng chung (397 dòng) |
| `script\missions\challengeoftime\mission_match.lua` | Thân mission id **22** (InitMission/EndMission/JoinMission/PlayerLeave) |
| `script\missions\challengeoftime\npc.lua` | Bảng 28 ải × 2 cấp, bảng NPC ẩn, bảng vật phẩm ngẫu nhiên, hàm sinh quái (502 dòng) |
| `script\missions\challengeoftime\npc_death.lua` | NPC chết → đếm quái → qua ải → hoàn thành → xếp hạng |
| `script\missions\challengeoftime\player_death.lua` | Người chơi chết trong ải |
| `script\missions\challengeoftime\award.lua` | Bảng thưởng từng ải + thưởng hoàn thành + rương |
| `script\missions\challengeoftime\timer_match.lua` | Timer 41 — hết giờ báo danh → khai trận |
| `script\missions\challengeoftime\timer_board.lua` | Timer 42 — thông báo mỗi phút |
| `script\missions\challengeoftime\timer_close.lua` | Timer 43 — hết 30 phút → thất bại |
| `script\missions\challengeoftime\npc\dragonboat_main.lua` | NPC báo danh (kiểm tổ đội, tìm bản đồ trống, thu phí, đưa vào) |
| `script\missions\challengeoftime\npc\transfer.lua` | NPC "Người tiếp dẫn Mật Phòng" — cửa vào chế độ 2011 |
| `script\missions\challengeoftime\chuangguang30.lua` | **Chế độ chuangguan30** (bản đồ 957, boss riêng) |
| `script\missions\challengeoftime\chuangguang30_playerdeath.lua` | **MÃ CHẾT** — không tệp nào tham chiếu (xem §10.4) |
| `script\missions\challengeoftime\rank_perday.lua` | Bảng xếp hạng ngày (ladder 10235) + phát thưởng hạng |
| `script\missions\challengeoftime\doubleexp.lua` | Móc nhân đôi kinh nghiệm |
| `script\missions\challengeoftime\item\chuangguanbaoxiang.lua` | Vật phẩm "Bảo Rương Vượt Ải" (6/1/2742) |
| `settings\trigger_challengeoftime.lua` | Trigger Relay chạy **mỗi giờ**: đóng/mở lại 32 mission |
| `script\vng_feature\challengeoftime\npcNhiepThiTran.lua` | Bản địa hoá VN: hộp thoại "Kiểm tra điều kiện tổ đội" |
| `script\task\tollgate\killer\nieshichen.lua` | **NPC cổng vào thật sự** (Nhiếp Thí Trần, npc id 769) — dùng chung với hệ Sát Thủ |

### 0.2 Bao đóng Include (`closure3.json` khoá `vuotai`)

103 tệp; 16 tệp seed; **0 tệp thiếu trong bản Linux**; **54 tệp JX1 chưa có**, trong đó có
`missions/boss/bigboss.lua`, `tong/tong_award_head.lua`, `battles/battlehead.lua`,
`lib/file.lua`, `lib/coordinate.lua`, `event/change_destiny/*`,
`activitysys/config/32/talkdailytask.lua`, `vng_event/thapnienlenhbai/*`.
49 tệp JX1 đã có (`activitysys/*`, `lib/awardtemplet.lua`, `misc/eventsys/*`,
`missions/basemission/lib.lua`, `event/storm/*`, `item/forbiditem.lua`, `global/forbidmap.lua`, …).

---

## 1. LUỒNG HOẠT ĐỘNG ĐẦY ĐỦ

### 1.1 Hằng số thời gian (`include.lua:6-32`)

| Hằng | Giá trị | Ý nghĩa | Dòng |
|---|---|---|---|
| `MISSION_MATCH` | 22 | mission id | `include.lua:6` |
| `TIMER_MATCH` | 41 | timer khai trận | `include.lua:7` |
| `TIMER_BOARD` | 42 | timer thông báo | `include.lua:8` |
| `TIMER_CLOSE` | 43 | timer kết thúc | `include.lua:9` |
| `INTERVAL_BOARD` | 1 | phút — nhịp thông báo | `include.lua:10` |
| `INTERVAL_MATCH` | 3600 | **KHÔNG dùng ở đâu** (0 hit ngoài include) | `include.lua:11` |
| `TIME_SIGNUP` | 10 | phút báo danh | `include.lua:12` |
| `LIMIT_SIGNUP` | 600 giây | hạn báo danh | `include.lua:13` |
| `LIMIT_FINISH` | 1800 giây (30 phút) | hạn hoàn thành | `include.lua:14` |
| `BOAT_POSX/Y` | 1559 / 3226 | điểm xuất phát trong ải | `include.lua:15-16` |
| `LIMIT_PLAYER_COUNT` | 8 | trần số người | `include.lua:17` |
| `AWARD_COUNT` | 10 | *khai báo nhưng bị bỏ qua* (xem §6.4) | `include.lua:18` |
| `TSK_JOIN_DATE` | task 1551 | ngày tham gia gần nhất | `include.lua:19` |
| `TSK_REMAIN_COUNT` | task 1550 | số lượt còn lại | `include.lua:20` |
| `CHUANGGUAN30_MAP_ID` | 957 | bản đồ Mật Phòng | `include.lua:27` |
| `CHUANGGUAN30_TIME_LIMIT` | 780 giây (13 phút) | mốc "về đích sớm" | `include.lua:28` |
| `CHUANGGUAN30_START_TIME` / `_END_TIME` | 10 / 22 | **KHÔNG dùng** (mã hard-code 10/22) | `include.lua:29-30` |
| `PLAYER_MAP_TASK` | task 2852 | lưu map gốc để chia nhóm ở 957 | `include.lua:31` |
| `COUNT_LIMIT` | 1 | số lượt/ngày | `include.lua:32` |
| `TSK_Longxuewan_Date/Use/avail` | 2641 / 2642 / 4018 | Long Huyết Hoàn | `include.lua:117-119` |

Chuyển sang tick engine: `StartMissionTimer(..., n*18)` (18 tick/giây)
— `include.lua:209` (60 s), `include.lua:220` (1800 s), `mission_match.lua:12` (600 s).

### 1.2 Trigger giờ (Relay)

`settings\trigger_challengeoftime.lua:8-23` — `OnTrigger()`:

1. Lặp 2 cấp: `close_missions(tbLevelMaps[i], 22, VARV_STATE)` rồi `start_missions(tbLevelMaps[i], 22)`
   (`settings\trigger_include.lua:9-34`: đặt `SubWorld = SubWorldID2Idx(mapid)` rồi
   `OpenMission(22)` / `CloseMission(22)` cho **từng** bản đồ).
2. `ChuangGuan30:KickOutAll()` + `ClearMapNpc(957)`.
3. `DynamicExecute(chuangguang30.lua, "ChuangGuan30:AddTime")` — đặt hẹn giờ boss Mật Phòng.

`OpenMission(22)` khiến engine gọi `InitMission()` của `mission_match.lua:27`:
`start_signup()` + `start_match_timer()`.

`start_signup()` (`mission_match.lua:16-25`): `VARV_STATE=1`, `VARV_BOARD_TIMER=0`,
`VARS_TEAM_NAME=""`. Câu quảng bá đã bị comment (dòng 22-24) vì "relay thông báo 2 lần".

### 1.3 Báo danh (NPC Nhiếp Thí Trần)

NPC đặt tại 7 bản đồ, npc setting id **769** (`script\global\autoexec_npc.lua:26-32`):

```
{769, 11,3210,4974} {769,  1,1506,3198} {769, 37,1647,3050} {769,176,1372,3010}
{769,162,1573,3227} {769, 78,1512,3206} {769, 80,1700,2963}
```

Đường thoại: `nieshichen.lua:56 main()` → mục `ContentList[24]` "Thử luyện sát thủ" →
`annealofkiller()` (`nieshichen.lua:65`) → 4 mục:
`ContentList[25]` "Tham gia khiêu chiến/**want_playboat**",
"Kiểm tra điều kiện tổ đội/#tbCOT_Party:CheckCondition()",
`ContentList[27]` "Liên quan đến khiêu chiến/aboutchallenge", đóng.
Hai mục nhận thưởng hạng nằm ở `main()`: `ContentList[30]` "/rank_award",
`ContentList[31]` "/get_top5team".

`want_playboat()` (`dragonboat_main.lua:104-124`):
- `nMatchLevel = GetMatchLevel(GetLevel())`
- `COT_GetFreeMap(nMatchLevel)` (`dragonboat_main.lua:82-102`) quét danh sách map của cấp đó,
  bỏ qua map chưa nạp (`SubWorldID2Idx < 0`), lấy `VARV_STATE` và `GetMSPlayerCount(22,1)`
  qua `lib:DoFunInWorld`; map **trống** = `nState==1 và nMSPlayerCount==0`;
  `bStarted=1` nếu có bất kỳ map nào `nState>0`.
- `bStarted` nil → "chưa đến giai đoạn báo danh. Cách <color=red>mỗi giờ<color> báo danh 1 lần".
- Còn map trống → hỏi xác nhận → `dragon_join`.

`dragon_join()` (`dragonboat_main.lua:209-229`) → `COT_CheckTeamRequirement()` → lấy map trống
đầu tiên → `lib:DoFunInWorld(nMapIndex, COT_TeamJoin, nMatchLevel)`.

#### Điều kiện tổ đội — `COT_CheckTeamRequirement()` (`dragonboat_main.lua:4-81`)

1. `IsCaptain() ~= 1` → chặn (dòng 6).
2. `GetTeamSize() < 4` → chặn: "cần phải có ít nhất **4** người" (dòng 11-14).
3. `GetCash() < 10000` → chặn: "cần **1 vạn lượng**" (dòng 16-19). *(Chú ý: câu thoại
   `want_playboat` dòng 116 lại ghi "10 vạn" — lệch text, mã thu 10000 = 1 vạn, `Pay(10000)`
   ở dòng 193.)*
4. Với **từng thành viên** (dòng 24-77):
   - `tbVNG_BitTask_Lib:getBitTask{nTaskID=3071, nStartBit=25, nBitCount=1}` ≠ 0 → chặn (bit cấm VN).
   - `GetTask(1551)==hôm nay và GetTask(1550)<=0` → hết lượt.
   - `GetMatchLevel(nPlayerLevel) ~= nMatchLevel` → lệch cấp (thông báo riêng cho cấp 1 / cấp 2).
   - Phải có **Sát Thủ Giản** trong rương/hành trang:
     cấp 1 = `CalcEquiproomItemCount(6,1,400,i)` với `i = 20,30,…,80`;
     cấp 2 = `CalcEquiproomItemCount(6,1,400,90)`.

`tbCOT_Party:CheckCondition()` (`npcNhiepThiTran.lua:3-46`) là bản kiểm **chỉ để xem**, luật VN:
tối đa **3 lần/ngày** (`:25`), phải dùng Long Huyết Hoàn để mở lượt 2 và 3 (`:29`),
`CalcSword()` quét `j = 20..90 bước 10` (`:50`).

#### Ghi danh — `COT_TeamJoin()` (`dragonboat_main.lua:176-207`), chạy với `SubWorld` = map đích

```
VARV_SIGNUP_WORLD/POSX/POSY = GetWorldPos()      -- điểm trả về khi kết thúc
VARV_LEVEL  = nMatchLevel
VARS_TEAM_NAME           = tên đội trưởng
VARS_TEAMLEADER_FACTION  = GetLastFactionNumber()
VARS_TEAMLEADER_GENDER   = GetSex()
Pay(10000)
VARV_BATCH_MODEL = 1 nếu (10 <= giờ <= 22 và nMatchLevel == 2), ngược lại 0   -- dòng 195-200
rồi CallPlayerFunction(mỗi thành viên, COT_TeamMemberJoin, nMatchLevel)
```

`COT_TeamMemberJoin()` (`dragonboat_main.lua:126-174`):
- Chặn nếu đang ở map 208 (đại lao) — dòng 128-131.
- Trừ **1 Sát Thủ Giản**: cấp 1 `ConsumeEquiproomItem(1,6,1,400,i)` (i=20..80),
  cấp 2 `ConsumeItem(3,1,6,1,400,90)`.
- Reset/trừ lượt: nếu `GetTask(1551) ~= hôm nay` thì `SetTask(1550, COUNT_LIMIT)` +
  `SetTask(1551, hôm nay)`; rồi `SetTask(1550, GetTask(1550)-1)`.
- Đếm lượt Long Huyết Hoàn: `TSK_Longxuewan_avail (4018)` giảm 1 nếu 2641 == hôm nay, ngược lại đặt -1.
- **`JoinMission(MISSION_MATCH, 1)`** (dòng 163) — hàm engine (§8).
- `G_ACTIVITY:OnMessage("SignUpChuangguan", PlayerIndex, nMatchLevel)`.
- `PlayerFunLib:AddTaskDaily(3079, 1)`; lần thứ ≥2 ghi log `BaoDanhVuotAiThuPhi`, lần đầu `BaoDanhVuotAiMienPhi`.

`JoinMission(RoleIndex, camp)` của script (`mission_match.lua:93-99`) → `PlayerJoinMission(camp)`
(`mission_match.lua:64-85`):

```
LeaveTeam(); AddMSPlayer(22, camp); SetCurCamp(camp)
SetTaskTemp(200,1); SetFightState(1); SetLogoutRV(1); SetPunish(0)
NewWorld(SubWorldIdx2ID(SubWorld), 1559, 3226)     -- BOAT_POSX/Y
SetCreateTeam(0); SetPKFlag(1); ForbidChangePK(1)
SetDeathScript("\script\missions\challengeoftime\player_death.lua")
SetTask(1505, 1); DisabledUseTownP(1)
SetTask(STORM_TASKID_GAMEID_SS, random(100000))    -- móc sự kiện Bão
```
+ `EventSys:GetType("ChuanGuan"):OnPlayerEvent("OnEnter", RoleIndex, camp)` (`mission_match.lua:98`).

**`LeaveTeam()` được gọi ngay đầu** — vào ải là tổ đội bị giải tán, mọi thứ về sau bám vào
`MISSION_MATCH` chứ không bám tổ đội.

### 1.4 Khai trận — timer 41 (`timer_match.lua`)

`OnTimer()` (`timer_match.lua:61-67`): `close_match_timer()` → `start_match()` → `start_board_timer()`.

`start_match()` (`timer_match.lua:13-54`):
- `VARV_STATE = 2`.
- `GetMSPlayerCount(22) == 0` → `cancel_match()` (thân rỗng, dòng 8-10) — **mission vẫn ở STATE 2**.
- Ngược lại: `start_close_timer()` (30 phút) → quảng bá toàn server →
  `VARV_NPC_BATCH=1`, `VARV_MISSION_RESULT=0`, `VARV_PLAYER_USE_INDEX=0`, `VARV_NPC_USE_INDEX=0` →
  `VARV_XIAONIESHICHEN_BATCH = random(5,15)` (`timer_match.lua:35-37`) →
  `save_player_info()` → dọn map (`ClearMapNpc/ClearMapTrap/ClearMapObj`) → `create_batch_npc(1)`.
- `VARV_PLAYER_COUNT = player_count`.

### 1.5 Vòng qua ải

- `timer_board.lua:7-22` mỗi 60 s: `VARV_BOARD_TIMER += 60`, phát câu
  "…đã bắt đầu rồi N phút, bây giờ là ải B, còn dư C con quái."
- Mọi NPC sinh ra đều bị gắn `SetNpcDeathScript(npc_death.lua)` (`npc.lua:444`).
- `npc_death.lua:152-162 OnDeath(index)`:
  ```
  _,_,nMapIndex = GetNpcPos(index)
  lib:DoFunInWorld(nMapIndex, NpcDeathAward, index)      -- móc Đổi Vận
  count = GetMissionV(VARV_NPC_COUNT) - 1
  nếu count >= 0 → SetMissionV(VARV_NPC_COUNT, count)
  nếu count == 0 → batch_finish(index)
  ```
- `batch_finish(index)` (`npc_death.lua:115-145`), nhánh `VARV_MISSION_RESULT == 0`:
  báo "đã tiêu diệt toàn bộ ải B" → `award_batch(batch, index)` →
  nếu `batch >= get_batch_count()` (=28) → `Mission_Complete(index)`
  ngược lại `VARV_NPC_BATCH = batch+1` và `create_batch_npc(batch+1)`.

### 1.6 Hoàn thành — `Mission_Complete()` (`npc_death.lua:47-112`)

```
time = VARV_BOARD_TIMER + 60 - floor(GetMSRestTime(22, TIMER_BOARD)/18)   -- giây, chính xác tới tick
VARV_MISSION_RESULT = 1
award_success(index, time)
LadderId = 10179 (cấp 1) | 10180 (cấp 2)
Ladder_NewLadder(DailyRankLadderId=10235, teamname, -time, 1)
nếu cấp 2: mỗi người → UpdatePlayerScore(time)          -- LỖI, xem §10.1
quảng bá "…đã dùng M phút S giây!"
nếu time < 20*60  → mở "nhiệm vụ bí mật": close_board_timer(); create_all_npc(tbHidenNpc[random])
ngược lại        → kickout()
```

Ghi chú: comment dòng 99 viết "12 phút" nhưng mã là `20 * 60` (dòng 100).

### 1.7 Ải ẩn xong → `batch_finish` nhánh `VARV_MISSION_RESULT == 1` (`npc_death.lua:117-126`)

```
award_hidden_mission()
nếu VARV_BATCH_MODEL == 1 và VARV_BOARD_TIMER <= 780  → add_transfer_npc()
ngược lại                                              → kickout()
```

`add_transfer_npc()` (`npc.lua:498-501`) gọi
`basemission_CallNpc(map_transfer_npc[1], SubWorldIdx2ID(nMapIndex), 1568*32, 3227*32)`
→ NPC id **1684** "Vượt Ải_Người tiếp dẫn Mật Phòng", script `npc\transfer.lua`.

### 1.8 Kết thúc / thất bại

- Hết 30 phút: `timer_close.lua:11-19` → thông báo thất bại → `close_close_timer()` →
  `close_match()` → `close_board_timer()`.
- `close_match()` (`include.lua:229-239`): `kickout()` rồi `ClearMapNpc(world, **1**)` (tham số 2
  = dọn cả người chơi), `ClearMapTrap`, `ClearMapObj`.
- `kickout()` (`include.lua:193-205`): mỗi người `DelMSPlayer(22,1)`, `SetLogoutRV(0)`,
  `NewWorld(VARV_SIGNUP_WORLD, POSX, POSY)`.
- Người chơi chết trong ải: `player_death.lua:7-34` — giảm `VARV_PLAYER_COUNT`, quảng bá
  "đội trưởng tử vong" nếu tên trùng `VARS_TEAM_NAME`, "toàn đội tử vong hết" nếu count==0,
  `DelMSPlayer(22,1)`, `NewWorld(11, 3207, 4978)` (**Thành Đô, cứng trong mã**).
- Rời mission: `OnLeave(index)` (`mission_match.lua:88`) → `PlayerLeave()` (`:38-62`) khôi phục
  toàn bộ trạng thái, `SetTask(1505,0)`, và kết sổ Bão
  (`storm_valid_game(2)` → `storm_add_pointex(2,40)` + `storm_end(2)`).
- `EndMission()` (`mission_match.lua:32-36`) = `close_match()` + tắt 2 timer.

---

## 2. HAI CHẾ ĐỘ

### 2.1 Chế độ thường (`VARV_BATCH_MODEL = 0`)

28 ải → nếu `< 20 phút` thì thêm 1 ải ẩn → thưởng → `kickout()`. Hết.

### 2.2 Chế độ "chuangguan30" — điều chỉnh 2011.03.04

**Kích hoạt** (`dragonboat_main.lua:195-200`): chỉ khi **cấp 2 (cao cấp)** *và* giờ báo danh
nằm trong `10 <= giờ <= 22`. Khi đó `VARV_BATCH_MODEL = 1`.

Khác biệt do cờ này bật:

| Điểm khác | Vị trí |
|---|---|
| Ải `VARV_XIAONIESHICHEN_BATCH` (random 5..15) sinh thêm **Tiểu Nhiếp Thi Trần** (npc 1673, boss, lv95) tại (1568,3227) + tăng `VARV_NPC_COUNT` + báo "đã tham gia chiến cuộc" | `npc.lua:477-484` |
| Ải 15 và ải 28 phát thêm **2 Bảo Rương Vượt Ải** mỗi người (`tbAward_batch = {[15]=2,[28]=2}`) | `award.lua:85-89`, `award.lua:103-106` |
| Ải có Tiểu Nhiếp Thi Trần phát thêm **1 rương** + **10.000.000 exp** (qua `nExp_tl`, có nhân đôi) | `award.lua:107-115` |
| Sau ải ẩn, nếu `VARV_BOARD_TIMER <= 780` (13 phút) → sinh NPC tiếp dẫn thay vì đá ra | `npc_death.lua:121-126` |

**Cửa vào Mật Phòng** — `npc\transfer.lua`:
- `main()` (`:7-23`): thoại "…đã vượt qua được 29 cửa ải trong thời gian 13 phút, Muốn tiếp tục Vượt Ải chứ!"
  với 2 mục: "Tiếp tục Vượt Ải/gotonewworld", "Rời khỏi/leave".
- `gotonewworld()` (`:36-56`):
  - Giờ `> 22` **và** `< 10` → chặn (điều kiện này **không bao giờ đúng**, xem §10.5).
  - `phút hiện tại >= 24` → "đã vượt quá thời gian truyền tống".
  - `VARV_BOARD_TIMER >= 780 + 60` → chặn.
  - `SetTask(PLAYER_MAP_TASK=2852, SubWorldIdx2ID(map hiện tại))` — **khoá nhóm**.
  - `NewWorld(957, floor(51072/32)=1596, floor(102272/32)=3196)`.
- `leave()` (`:26-33`): `DelMSPlayer(22,1)` + về `VARV_SIGNUP_*`.

**Trong map 957** — `chuangguang30.lua`, bảng `ChuangGuan30`:
- Đăng ký `EnterMap`/`LeaveMap` qua `EventSys` cho map 957 (`:63-72`).
- `OnEnterMap()` (`:96-116`): nếu `bActive==1` (boss đã ra) → đá ra ngay;
  gom người theo `GetTask(2852)` vào `tbGroup[nMapId]`; `nCount++`;
  `SetDeathScript(chuangguang30.lua)`; **`SetTmpCamp(nMapId)`** (mỗi map gốc = 1 phe tạm) → `SetState()`.
- `SetState()` (`:84-94`): TaskTemp 200=1, `SetFightState(0)`, LogoutRV 1, Punish 0,
  CreateTeam 1, PKFlag 1, ForbidChangePK 1, DisabledUseTownP 1, **`ForbidEnmity(1)`**.
- `SetForbitItem()` (`:243-255`): `set_MapType(957, "Mật Phòng cửa ải")`, cấm `TRANSFER` và
  `MATE` (`FORBITMAP_LIST[957]=1`).
- `AddTime()` (`:257-269`): `nTimeOut = 780 + 600 + 60 = 1440 s (24 phút)`;
  `AddTimer(1440*18, "ChuangGuan30:GameTime", 0)` — tức boss ra ở **phút 24 của mỗi giờ**.
- `GameTime()` (`:233-241`): nếu `nCount==0` thoát; ngược lại `OnAddBoss()` + `FightState()` (bật đánh nhau).
- `OnAddBoss()` (`:212-222`): `nBossid = random(2,11)` → `map_new_Ncp[nBossid]` (npc 1674..1683),
  toạ độ lấy từ `\settings\maps\liandandong\npc_3.txt` dòng `random(2,50)` cột 1
  (**giá trị trong tệp đã nhân 32 sẵn**), `Msg2Player` + `bActive = 1`.
- Boss chết → `script\activitysys\g_npcdeath.lua:72` gọi
  `ChuangGuan30:OnNpcDeath(nNpcIndex, PlayerIndex)` (`:155-182`):
  lọc map 957, lọc dải npc id (**điều kiện bị lỗi, §10.2**), lấy `nGroup = GetTask(2852)` của
  người hạ boss, phát thưởng cho mọi người còn sống trong nhóm đó, `OnMessage(nGroup)`
  (`G_ACTIVITY:OnMessage("Chuanguan", 30, tbAllPlayer, 1)`), rồi `KickOutAll()`.
- Chết trong 957 → `OnDeath` (`:152-154`) → `KickOut()` → `NewWorld(11, 3207, 4978)`.

---

## 3. HAI CẤP ĐỘ VÀ DANH SÁCH BẢN ĐỒ

`include.lua:83-88`
```lua
tbLevels = {
  [1] = {50,  90;  szName="Nhiệm vụ Thách thức thời gian Sơ cấp"},
  [2] = {90, 201;  szName="Nhiệm vụ Thách thức thời gian cao cấp"},
}
```
`GetMatchLevel(nLevel)` (`include.lua:121-133`): không truyền tham số → trả `GetMissionV(VARV_LEVEL)`;
có tham số → tìm `min <= nLevel < max`. Cấp < 50 hoặc >= 201 trả **nil**.

`include.lua:90-103` — `tbLevelMaps`:
- cấp 1: **464..479** (16 map)
- cấp 2: **480..495** (16 map)

### 3.1 Kiểm chứng bản Linux — `settings\maplist.ini`

Cả **32/32** map đều có, thư mục dữ liệu chung `特殊用地\杀手的试炼` ("Sát thủ đích thí luyện"):
- 464-471 = "Thách thức thời gian (Sơ cấp 1..8)"
- 472-479 = "Thử luyện tài nghệ (Sơ cấp 1..8)"  ← tên khác nhưng cùng cấp 1
- 480-495 = "Thách thức thời gian (Cao cấp 1..16)"
- 957 = `特殊用地\安期炼丹洞`, tên "Mật Phòng của ải"

### 3.2 Kiểm chứng JX1 (`E:\...\bin\server\settings\maplist.ini`)

- **32/32 map 464-495 CÓ**, cùng thư mục `特殊用地\杀手的试炼`, tên VN gần y hệt.
- **957 CÓ** trong maplist (`特殊用地\安期炼丹洞`) nhưng tên còn tiếng Trung `闯关密室`.
- Max map id trong JX1 = 1002.

### 3.3 Bản đồ có thực sự được NẠP không? (`Maps\WorldSet_GameServer.ini`)

- `Count = 910`. **Cả 32 map 464..495 ĐỀU nằm trong danh sách nạp của JX1** ⇒ chạy được ngay.
- **957 KHÔNG nằm trong danh sách** (quanh đó có 950, 959..964). Nhưng map **967** dùng **cùng
  thư mục** `安期炼丹洞` và đang được nạp ⇒ dữ liệu bản đồ có sẵn, chỉ cần thêm một mục
  `WorldNNN=957` và tăng `Count`.
- `Maps\*_srv.fp` (86 tệp) **không liên quan** — đó chỉ là cache lưới A* của bot
  (`Sources\Core\Src\KSubWorld.cpp:3397`), không phải dữ liệu bản đồ.

---

## 4. BẢNG BIẾN MISSION (VARV_* / VARS_*)

Chuỗi — `SetMissionS` / `GetMissionS` (`include.lua:50-54`):

| Chỉ số | Tên | Ý nghĩa | Ai ghi | Ai đọc |
|---|---|---|---|---|
| 1 | `VARS_TEAM_NAME` | tên đội trưởng = tên đội | `COT_TeamJoin` (`dragonboat_main.lua:190`), `start_signup` xoá (`mission_match.lua:20`) | `npc_death.lua:58,97,130`; `player_death.lua:17` |
| 2 | `VARS_PLAYER_NAME` (gốc) | slot **3..10** = tên 8 người | `save_player_info` (`include.lua:250`) | `func_npc_getname` (`:320`), `func_npc_get_eachname` (`:348`) |
| 11 | `VARS_TEAMLEADER_FACTION` | môn phái đội trưởng | `dragonboat_main.lua:191` | `npc_death.lua:59` |
| 12 | `VARS_TEAMLEADER_GENDER` | giới tính đội trưởng | `dragonboat_main.lua:192` | `npc_death.lua:60` |

Số — `SetMissionV` / `GetMissionV` (`include.lua:56-76`):

| Chỉ số | Tên | Ý nghĩa | Ai ghi | Ai đọc |
|---|---|---|---|---|
| 1 | `VARV_NPC_BATCH` | ải hiện tại (1..28) | `timer_match.lua:29`, `npc_death.lua:141` | `timer_board.lua:10`, `npc_death.lua:129` |
| 2 | `VARV_NPC_COUNT` | số quái còn sống của ải | `npc.lua:463,481`; `npc_death.lua:157` | `npc_death.lua:155`; `player_death.lua:15`; `timer_board.lua:11` |
| 3 | `VARV_PLAYER_COUNT` | số người còn sống | `timer_match.lua:53`; `player_death.lua:13` | (chỉ ghi — không nhánh nào đọc để quyết định) |
| 4 | `VARV_STATE` | 1 = đang báo danh, 2 = đang thi đấu | `mission_match.lua:18`; `timer_match.lua:15` | `dragonboat_main.lua:90-95`; `trigger_include.lua:28` |
| 5 | `VARV_SIGNUP_WORLD` | map báo danh (nơi trả về) | `dragonboat_main.lua:186` | `include.lua:196`; `transfer.lua:27` |
| 6/7 | `VARV_SIGNUP_POSX/POSY` | toạ độ trả về | `dragonboat_main.lua:187-188` | `include.lua:197-198`; `transfer.lua:28-29` |
| 8 | `VARV_BOARD_TIMER` | giây đã trôi từ lúc khai trận | `mission_match.lua:19`; `timer_board.lua:12`; `npc_death.lua:51` | `npc_death.lua:49,121`; `award.lua:252`; `transfer.lua:47` |
| 9 | `VARV_PLAYER_TOTAL_COUNT` | tổng người lúc khai trận (kể cả đã chết) | `include.lua:256` | `include.lua:289,307,346,362` |
| 10 | `VARV_PLAYER_USE_INDEX` | con trỏ "đang mượn tên/giới tính người thứ mấy" | `include.lua:257,394`; `timer_match.lua:31` | `include.lua:288,306,361` |
| 11 | `VARV_NPC_USE_INDEX` | con trỏ vòng tên NPC dự bị | `include.lua:316`; `timer_match.lua:32` | `include.lua:312` |
| 12 | `VARV_MISSION_RESULT` | 1 = thắng, 0 = chưa/thua | `timer_match.lua:30`; `npc_death.lua:54` | `npc_death.lua:117`; `award.lua:173` |
| 13 | `VARV_PLAYER_SEX` (gốc) | slot **14..21** = giới tính 8 người | `include.lua:251` | `include.lua:290` |
| 22 | `VARV_PLAYER_SERIES` (gốc) = 13+8+1 | slot **23..30** = ngũ hành 8 người | `include.lua:252` | `include.lua:370` |
| 31 | `VARV_LEVEL` = 22+8+1 | cấp trận (1/2) | `dragonboat_main.lua:189` | `GetMatchLevel()` (`include.lua:123`) |
| 32 | `VARV_XIAONIESHICHEN_BATCH` | ải sẽ có Tiểu Nhiếp Thi Trần (random 5..15) | `timer_match.lua:36` | `npc.lua:477`; `award.lua:108` |
| 33 | `VARV_BATCH_MODEL` | 0 thường / 1 chế độ 2011 | `dragonboat_main.lua:197,199` | `npc.lua:477`; `award.lua:103,108`; `npc_death.lua:121` |

**Trần dùng thực tế = 33.** JX1 `MAX_MISSION_VALUE_COUNT = 100`
(`Sources\Core\Src\KMission.h:122`) ⇒ đủ. Chuỗi mission ở JX1 đi kho riêng
`s_MissionStr` khoá `subworldIdx*4096 + id` (`Sources\Core\Src\KJx2WarInfra.cpp:1034-1069`)
⇒ không đụng mảng số, chỉ số 1..12 an toàn.

---

## 5. CƠ CHẾ SINH QUÁI

### 5.1 Định dạng bảng (`include.lua:36-45`)

Một **mục NPC** là mảng 8 cột:

| Cột | Hằng | Ý nghĩa |
|---|---|---|
| 1 | `NPC_ATTRIDX_PROCEED` | hàm hậu xử lý (nil hoặc `func_npc_proceed`) |
| 2 | `NPC_ATTRIDX_ID` | số / bảng {min,max} / hàm |
| 3 | `NPC_ATTRIDX_NAME` | chuỗi / hàm / bảng `{hàm, ladderId}` |
| 4 | `NPC_ATTRIDX_LEVEL` | 75 (cấp 1) / 95 (cấp 2) — **3 NGOẠI LỆ ở cấp 2 vẫn ghi 75**: ải 28 (`npc.lua:225`) và cả 2 mục `tbHidenNpc` (`npc.lua:230,232`) **[đã sửa theo phản biện]** |
| 5 | `NPC_ATTRIDX_SERIES` | -1 = ngẫu nhiên, 0..4 cố định, hoặc hàm |
| 6 | `NPC_ATTRIDX_ISBOSS` | 0/1 |
| 7 | `NPC_ATTRIDX_COUNT` | số con **và** khoá tra bảng toạ độ |
| 8 | `NPC_ATTRIDX_POSITION` | `{x,y}` cố định hoặc `func_npc_getpos` |

Một **ải** = `{ <số nhóm NPC>, <bộ thưởng {exp, hoa, vật phẩm}>, <mục NPC 1>, <mục NPC 2>, ... }`
— `create_all_npc(npcs)` (`npc.lua:456-465`) đọc `npcs[1]` làm số vòng và bắt đầu từ `npcs[i+2]`.

`add_npc(item)` (`npc.lua:418-454`) gọi
`AddNpcEx(id, level, series, SubWorld, px*32, py*32, 1, name, isboss)` rồi
`SetNpcDeathScript(npc_index, npc_death.lua)`. **Toạ độ nhân 32 tại đây.**

### 5.2 Bảng toạ độ đội hình (`npc.lua:11-19`)

```
map_posfiles[8]  = \settings\maps\challengeoftime\lineup8.txt   (1 cột toạ độ)
             [16] = lineup16.txt   [20] = lineup20.txt   [24] = lineup24.txt
             [32] = lineup32.txt   [40] = lineup40.txt   [56] = lineup56.txt
```
`func_npc_getpos(item, index)` (`include.lua:374-390`) tra `map_posfiles[item[7]]`, chọn cột
`2*(random(1,pos_count)-1)+1`, đọc dòng `index+1` bằng `get_file_pos` → `GetTabFileData`.
7 tệp đều có thật, định dạng `XPOS\tYPOS` + N dòng (ô, chưa ×32).
**JX1 chưa có thư mục `settings\maps\challengeoftime\`.**

### 5.3 Danh sách 28 ải

Thưởng ghi dạng `{exp(vạn), số bông hoa rơi, bảng vật phẩm}`:

| Ải | Cấp 1 (nhóm / thưởng) | Cấp 2 (nhóm / thưởng) |
|---|---|---|
| 1 | 1 / {1.5, 0, nil} | 1 / {2, 0, nil} |
| 2 | 1 / {1.5, 0, nil} | 1 / {2, 0, nil} |
| 3 | 1 / {3, 0, nil} | 1 / {3, 0, nil} |
| 4 | 2 / {9, 0, nil} | 2 / {9, 0, nil} |
| 5 | 1 / {3, 0, nil} | 1 / {3, 0, nil} |
| 6 | 1 / {4.5, 0, nil} | 1 / {5, 0, nil} |
| 7 | 1 / {4.5, 0, nil} | 1 / {5, 0, nil} |
| 8 | 2 / {6, 10, nil} | 2 / {9, 15, nil} |
| 9 | 1 / {5, 0, nil} | 1 / {6, 0, nil} |
| 10 | 1 / {5, 0, nil} | 1 / {6, 0, nil} |
| 11 | 2 / {9, 0, nil} | 2 / {12, 0, nil} |
| 12 | 1 / {6, 0, nil} | 1 / {9, 0, nil} |
| 13 | 2 / {6, 10, nil} | 2 / {9, 15, nil} |
| 14 | 1 / {4.5, 0, nil} | 1 / {6, 0, nil} |
| 15 | 1 / {6, 0, nil} | 1 / {9, 0, nil} |
| 16 | 2 / {9, 0, nil} | 2 / {14, 0, nil} |
| 17 | 1 / {4.5, 0, nil} | 1 / {6, 0, nil} |
| 18 | 2 / {6, 10, nil} | 2 / {12, 20, nil} |
| 19 | 1 / {4.5, 0, nil} | 1 / {6, 0, nil} |
| 20 | 1 / {6, 0, nil} | 1 / {8, 0, nil} |
| 21 | 2 / {10.5, 0, nil} | 2 / {14, 0, nil} |
| 22 | 1 / {4.5, 0, nil} | 1 / {6, 0, nil} |
| 23 | 1 / {4.5, 0, nil} | 1 / {6, 0, nil} |
| 24 | 1 / {7.5, 30, nil} | 1 / {10, 20, nil} |
| 25 | 2 / {9, 0, nil} | 2 / {15, 0, nil} |
| 26 | 1 / {7.5, 0, nil} | 1 / {9, 0, nil} |
| 27 | 2 / {24, 15, nil} | 2 / {32, 32, nil} |
| 28 | 1 / {0, 0, nil} | 1 / {0, 0, nil} |

Nguồn: `npc.lua:25-235`. `get_batch_count()` = `getn(GetNpcList())` = **28** (`npc.lua:413-415`).

Đặc điểm:
- Các ải "2 nhóm" (4, 8, 11, 13, 16, 18, 21, 25, 27) = 16 quái thường + **1 boss** cố định tại
  `{1568, 3227}` — boss này **mang tên/giới tính/ngũ hành của một người chơi trong đội**
  (`func_npc_getid`/`func_npc_getname`/`func_npc_getseries`, và `func_npc_proceed` đẩy con trỏ
  `VARV_PLAYER_USE_INDEX` sang người kế tiếp). Hết người thì lấy tên trong
  `map_npcname_candidates` (7 tên, `include.lua:152-160`) và ngũ hành ngẫu nhiên.
- Ải **27** đặc biệt: 1 con **Nhiếp Thi Trần** (npc 1006 cấp 1 `isboss=0`, npc 1038 cấp 2 `isboss=1`)
  tại `{1568,3227}` + 16 "Lang bằng" (`npc.lua:115-117`, `npc.lua:220-222`).
- Ải **28**: 8 con (npc 993 / 1025) `isboss=1`, tên = `func_npc_get_eachname` → **mỗi con mang tên
  một người chơi khác nhau** (`npc.lua:119-120`, `npc.lua:224-225`).
- Ải **11** còn kích hoạt `tbTalkDailyTask:AddTalkNpc(...)` — NPC đối thoại nhiệm vụ ngày
  (`npc.lua:490-493`).
- **Tiểu Nhiếp Thi Trần** (npc **1673**, cấp 95, boss, `{1568,3227}`) chỉ ở chế độ
  `VARV_BATCH_MODEL == 1`, tại ải `VARV_XIAONIESHICHEN_BATCH` (`npc.lua:477-484`).

### 5.4 `tbRangeId` — dải NPC "hoá thân người chơi" (`include.lua:106-115`)

```
[1] = { {994, 1001},   {1002, 1005} }     -- cấp 1: nam / nữ
[2] = { {1026, 1033},  {1034, 1037} }     -- cấp 2: nam / nữ
```
`get_random_npc_id(sex)` (`include.lua:269-277`): `random(range[sex+1][1], range[sex+1][2])`.
`sex == 2` (nhân yêu) trả **nil** → `AddNpcEx` sẽ hỏng. *(Bản Linux để ngỏ: comment
`人妖该怎么处理？` = "nhân yêu xử lý sao?", `include.lua:271`.)*

Dải này cũng là **NPC ải ẩn** (`tbHidenNpc`, `npc.lua:123-128` và `npc.lua:228-233`):
2 phương án, mỗi phương án 1 con boss lấy tên ngẫu nhiên từ **bảng xếp hạng**:
- cấp 1: `{994,1001}` + tên từ ladder **10119** (săn boss sát thủ) *hoặc* **10179** (vượt ải sơ cấp)
- cấp 2: `{1026,1033}` + tên từ ladder **10119** *hoặc* **10180** (vượt ải cao cấp)

`func_ladder_getname` (`include.lua:325-342`): `Ladder_GetLadderInfo(ladderId, random(1,10))`,
rỗng thì rơi về `map_npcname_candidates`.

### 5.5 NPC riêng của chế độ 2011 (`npc.lua:238-255`)

```
map_new_Ncp[1]  = {nil, 1673, "Tiểu Nhiếp Thi Trần", 95, -1, 1, 1, {1568,3227}}   -- ĐỊNH DẠNG MẢNG
map_new_Ncp[2..11] = { nNpcId = 1674..1683, nLevel=95, bNoRevive=1, nIsboss=1,     -- ĐỊNH DẠNG BẢNG
                       szName = "Vượt Ải_Cổ Bách" / "_Huyền Giác Đại Sư" / "_Đường Bất Nhiễm" /
                                "_Lam Y Y" / "_Thanh Hiểu Sư Thái" / "_Chung Linh Tú" /
                                "_Hạ Nhân Ngã" / "_Đoan Mộc Duệ" / "_Đạo Thanh Chân Nhân" /
                                "_Toàn Cơ Tử" }
map_transfer_npc[1] = { nNpcId = 1684, bNoRevive=1,
                        szName="Vượt Ải_Người tiếp dẫn Mật Phòng",
                        szScriptPath="\script\missions\challengeoftime\npc\transfer.lua" }
```
**Bẫy khi port:** `[1]` dùng định dạng mảng 8 cột (đi qua `add_npc`, có ×32);
`[2..11]` và `map_transfer_npc[1]` dùng định dạng bảng có tên trường
(đi qua `basemission_CallNpc`, `missions\basemission\lib.lua:27-76`, **không** ×32).

---

## 6. BẢNG THƯỞNG

### 6.1 Thưởng từng ải — `award_batch()` (`award.lua:246-263`)

```
award_batch_item(tbNpcList[batch][2], npc_index, 0)    -- exp + hoa cho cả đội
G_ACTIVITY:OnMessage("Chuanguan", batch, tbAllPlayer, n_level)
EventSys:GetType("ChuanGuan"):OnEvent("OnPass", batch, tbAllPlayer, n_level, nTime)
award_batch_extend(batch)
```

`award_batch_item` (`award.lua:231-240`): với mỗi người → `award_player(item[1], item[3], time)`;
rồi `drop_item(npc_index, item[2])`.

`award_player(exp, objects, time)` (`award.lua:170-213`):
```
nếu VARV_MISSION_RESULT==1 → storm_addpoint(2, LIMIT_FINISH - time)
experience = exp là hàm ? exp(time) : exp
point = experience * 10000
AddExp_Skill_Extend(point)              -- hệ kỹ năng 120
nếu GetName() == VARS_TEAM_NAME → point = point * 1.2          (thưởng đội trưởng)
nếu GetMatchLevel() == 2        → point = point * 2
point = BigBoss:AddChuangGuanPoint(point)      -- x2 nếu có quyền lợi Big Boss
point = Chuangguan_checkdoubleexp(point)       -- x2 sự kiện
AddOwnExp(point)
nếu objects ~= nil → award_random_object(objects)
```

`drop_item(index, count)` (`award.lua:216-228`):
`count` bông hoa `DropItem(world,x,y,-1, 1,2,0,5, 0,...)`, thêm **5%** rơi
`DropItem(..., 6,1,1392, 0, ...)` ("Hộp lễ vật vượt ải").

`award_batch_extend(batch)` (`award.lua:91-118`) — mỗi người:
- `award_batch_contribution(batch)` → `tongaward_challengeoutoftime(batch)`
  (`script\tong\tong_award_head.lua:41-52`): **cống hiến bang** =
  ải 1-10 → `4 * COEF_CONTRIB_TO_VALUE`; ải 11-20 → `8 *`; ải 21-28 → `10 *`;
  ghi bằng `ContriValueEntryLogic(nValue, EVE_CHUNGGUAN)`.
- `SetTask(tbBirthday0905.tbTask.tsk_toll_cg_passcount, batch)` — task **2606**
  (`script\event\birthday_jieri\200905\taskctrl.lua:36`).
- Chế độ 2011: rương ải 15/28 và rương + 10 triệu exp ở ải Tiểu Nhiếp Thi Trần (§2.2).

### 6.2 Thưởng hoàn thành — `tbAward_Success` (`award.lua:58-83`)

```lua
[1] = { [1] = function(time)                     -- CẤP 1
            local min = floor(time/60)
            if min >= 25 then return 15
            else return floor(172 * (1 - min/25)) + 10 end
        end, [2] = 0, [3] = nil },
[2] = { [1] = function(time)                     -- CẤP 2
            local min = floor(time/60)
            if min >= 25 then return 30
            else return floor(233 * (1 - min/25)) + 20 end
        end, [2] = 0, [3] = nil },
```
Đơn vị là "vạn" → nhân 10000 trong `award_player`. Ví dụ cấp 2 về đích 10 phút:
`floor(233*0.6)+20 = 159` vạn = 1.590.000 exp gốc, ×2 (cấp 2) = 3.180.000, đội trưởng ×1.2.

`award_success(npc_index, time)` (`award.lua:280-283`) = `award_batch_item(GetMissionCompleteAward(), …)`.
`[3] = nil` ⇒ **hoàn thành 28 ải KHÔNG rơi vật phẩm ngẫu nhiên**, chỉ exp.

### 6.3 Thưởng ải ẩn — `award_hidden_mission()` (`award.lua:121-131`)

Mỗi người `award_random_object(map_random_awards)` (bảng 44 mục, `award.lua:7-55`, **trùng hệt**
bảng cùng tên ở `npc.lua:258-306` — chỉ khác dấu `;` cuối; `award.lua` được Include sau nên thắng).
Cơ số 100000, `award_random_object` (`award.lua:149-162`) cộng dồn `odds*base` cho tới khi
`num <= sum`. Tỉ lệ tiêu biểu:

| Tỉ lệ | Vật phẩm |
|---|---|
| 0.2 / 0.19587 / 0.18 | Phúc Duyên Lộ (Đại/Trung/Tiểu) = 6/1/124, 123, 122 |
| 0.15 / 0.1 / 0.1 | Tiên Thảo Lộ 6/1/71, Thiên Sơn Bảo Lộ 6/1/72, Bách Quả Lộ 6/1/73 |
| 0.05 | Lệnh bài Phong Lăng Độ (event item 489) |
| 0.005 ×4 | Tinh Hồng Bảo Thạch (353), Lam/Tử/Lục Thuỷ Tinh (238/239/240) |
| 0.0005 | Võ Lâm Mật Tịch 6/1/26, Tẩy Tuỷ Kinh 6/1/22, 5 món **Định Quốc** (gold 0/159..163) |
| 0.0001 ×4 | 4 món **An Bang** (gold 0/164..167) |
| 0.00001 ×24 | 24 món **Hoàng Kim** (gold 0/2, 6, 11, 21, 26, 39, 40, 46, 51, 61, 67, 71, 77, 81, 87, 94, 96, 107, 115, 122, 126, 132, 136) |

Cộng thêm `SetTask(2606, 29)` và `EventSys:GetType("ChuanGuan"):OnEvent("OnPass", 29, ...)`.

### 6.4 `AWARD_COUNT = 10` — **mã chết**

`chuangguang30.lua:168` **[đã sửa theo phản biện — bản trước ghi nhầm `:172`]** gán
`local nCount = AWARD_COUNT` nhưng dòng 175 truyền **hằng 2**
(`CallPlayerFunction(nPlayerIndex, self.GiveAward, self, nGroup, 2)`), và `GiveAward(nGroupId, nCount)`
(`chuangguang30.lua:118-126`) **không dùng `nCount`** chút nào — nó phát cố định:
```
{szName="Điểm Kinh Nghiệm", nExp = 20e6}      -- 20 triệu exp
tbVnItemAwardEx                                -- 3 phần vật phẩm
```
`tbVnItemAwardEx` (`chuangguang30.lua:37-54`, sửa 20140226):
- Phần 1: Tinh Tinh Khoáng 6/1/3811 (nRate 20), Tinh Thiết Khoáng 6/1/3810 (10)
- Phần 2: Túi Dược Phẩm 6/1/30557 (30), Càn Khôn Tạo Hoá Đan Đại 6/1/215 ×50 (40),
  Đại Lực hoàn 6/0/3 (15), Phi Tốc hoàn 6/0/6 (15)
- Phần 3: Đồ Phổ Đằng Long Hồng Liên 6/1/30533 (0.3), Đằng Long Thạch-Hộ 6/1/30538 ×5 (6),
  Đồ Phổ Tinh Sương Hộ Uyển 6/1/30009 (0.4), Đồ Phổ Tinh Sương Hồng Liên 6/1/30010 (0.4)

### 6.5 Bảng xếp hạng — **CÓ dùng RELAYLADDER**

`include.lua:1` = `IncludeLib("RELAYLADDER")`; `npc_death.lua:9` = `IL("RELAYLADDER")`.

Hàm engine dùng: **`Ladder_NewLadder`** và **`Ladder_GetLadderInfo`** (không dùng `Ladder_ClearLadder`).

| Ladder id | Ý nghĩa | Ghi ở |
|---|---|---|
| 10119 | săn boss sát thủ (chỉ **đọc** để mượn tên NPC ải ẩn) | `include.lua:334` qua `npc.lua:125,230` |
| 10179 | vượt ải **sơ cấp** | `npc_death.lua:65,77,85` |
| 10180 | vượt ải **cao cấp** | `npc_death.lua:67,77,85` |
| 10235 | `DailyRankLadderId` — xếp hạng **theo ngày** | `rank_perday.lua:9`, ghi ở `npc_death.lua:88` |

`Ladder_NewLadder(LadderId, teamname, -1*laddertime, 1, nLeaderFaction, nLeaderGender)`
— thời gian lưu dưới dạng **số âm** để "lớn hơn = nhanh hơn" khi ladder sắp giảm dần.

`rank_perday.lua`:
- `update_gbtask(nTime)` (`:31-50`) dùng `gb_SetTask/gb_GetTask` khoá chuỗi
  `"challengeoftime_ranklist"` với 4 ô: 1=ngày hôm nay, 2=thành tích hôm nay,
  3=ngày hôm qua, 4=thành tích hôm qua.
- `rank_award()` (`:15-29`): chỉ nhận khi `4 <= HHMM <= 2300`, tức **00:04 → 23:00**
  (mã thật `:17` là `if (ntime < 4 or ntime > 2300) then ... return`)
  **[đã sửa theo phản biện — bản trước ghi nhầm "0400 < HHMM <= 2300"; `< 4` là 00:04 chứ không phải 04:00,
  gần như chắc chắn là lỗi gõ của bản gốc, khi port nên đổi thành `< 400`]**,
  cần ≥1 ô trống hành trang, rồi `update_gbtask(9999999)` + `update_playertask()`.
- `update_playertask()` (`:52-75`): so `tsk_rank_lastdate (2636)` / `tsk_rank_2thdate (2638)`
  với **ngày hôm qua**; nếu khớp và điểm ≠ 0 thì xoá điểm và trao
  `tbQiannianlingyao = {6,1,2116,1,1,0}` "Thiên Niên Linh Dược", `nExpiredTime = 24*60` phút (`:13`).
- `get_top5team()` (`:77-94`): đọc top 5 của ladder 10235, đảo dấu, in "Hạng %d: %s Thành tích: %s".
- Task cá nhân: **2636** (ngày gần nhất), **2637** (điểm gần nhất), **2638**/**2639** (lần trước đó)
  — ghi bởi `UpdatePlayerScrore` (`npc_death.lua:32-45`).

### 6.6 Bảo Rương Vượt Ải — `item\chuangguanbaoxiang.lua`

Item **6/1/2742** (`settings\item\004\magicscript.txt` dòng 3349, tên "Bảo Rương Vượt ải",
script trỏ đúng tệp này, stack 50).

Cần chìa (`:17-20`): `chiakhoanhuy` = **6/1/2744** "Chìa Khóa Như ý";
`chiakhoavang` = **6/1/30191** "Chìa khóa vàng".

- Chìa Nhũ Ý (`:23-55`): 5 bậc exp qua `tbvng_ChestExpAward:ExpAward`
  1 triệu (52%) / 2 triệu (30) / 3 triệu (10) / 4 triệu (5) / 5 triệu (3).
- Chìa Vàng (`:57-111`): Chân Nguyên Đan 6/1/4134 ×10 (5%), Chân Nguyên Đơn trung 6/1/30228 ×7 (10),
  đại 6/1/30229 ×7 (5), Hỗn nguyên chân đơn 6/1/30301 (0.01), Hộ Mạch Đan 6/1/3203 ×50 (10),
  Huyết Long Đồng cấp 9/11/12 (6/1/30289) ×5 (1.2 / 1.1 / 0.5), Tinh Tinh Khoáng (1),
  Tinh Thiết Khoáng (0.5), 6 đồ phổ (30529, 30537, 30506, 30507, 30006, 30505) 0.001–0.03,
  và 6 bậc exp 2/4/5/6/8/10 triệu (29.637 / 15 / 10 / 5 / 4 / 2).
- `VnCOTBoxNewAward` (`:148-171`): trừ 1 chìa + 1 rương, chìa vàng còn gọi
  `TransLife6:OnFinishEvent(TASK_ID_BOX)`, phát bằng `tbAwardTemplet:Give`,
  `AddStatData("baoxiangxiaohao_kaichuangguanbaoxiang", 1)`,
  `EventSys:GetType("OpenFuncAwardBox"):OnPlayerEvent("OpenAwardBoxEvent", PlayerIndex)`.

---

## 7. TẤT CẢ HÀM LUA DO SCRIPT ĐỊNH NGHĨA

### `include.lua`
| Dòng | Hàm | Mô tả |
|---|---|---|
| 121 | `GetMatchLevel(nLevel)` | cấp trận theo cấp NV; không tham số → đọc `VARV_LEVEL` |
| 135 | `GetMatchMapList(nLevel)` | trả `tbLevelMaps[nLevel]` |
| 163 | `broadcast(msg)` | `AddGlobalNews` |
| 169 | `GetMatchPlayerList()` | duyệt `GetNextPlayer(22, index, 0)`, trần 8 |
| 193 | `kickout()` | đưa mọi người về điểm báo danh |
| 208/213 | `start_board_timer()` / `close_board_timer()` | timer 42 (60 s) |
| 218/224 | `start_close_timer()` / `close_close_timer()` | timer 43 (1800 s) |
| 229 | `close_match()` | kickout + dọn NPC/trap/obj |
| 242 | `save_player_info()` | lưu tên/giới tính/ngũ hành 8 người vào biến mission |
| 260 | `GetRandomNpcRange()` | `tbRangeId[cấp]` |
| 269 | `get_random_npc_id(sex)` | id NPC theo giới tính |
| 280 | `get_file_pos(file, line, column)` | 2 ô liền kề của bảng toạ độ |
| 286 | `func_npc_getid(item, index)` | id NPC theo giới tính người thứ `VARV_PLAYER_USE_INDEX+1` |
| 304 | `func_npc_getname(item, index)` | tên người chơi, hết thì vòng `map_npcname_candidates` |
| 325 | `func_ladder_getname(item, index)` | tên top-10 của ladder `item[3][2]` |
| 345 | `func_npc_get_eachname(item, index)` | tên người thứ `index` (dùng cho ải 28) |
| 359 | `func_npc_getseries(item, index)` | ngũ hành người chơi / ngẫu nhiên |
| 374 | `func_npc_getpos(item, index)` | toạ độ từ `map_posfiles` |
| 392 | `func_npc_proceed(item)` | `VARV_PLAYER_USE_INDEX += 1` |

### `npc.lua`
`get_npc_id(309)`, `get_npc_name(323)`, `get_npc_level(340)`, `get_npc_series(345)`,
`npc_proceed(363)`, `get_npc_count(372)`, `get_npc_pos(377)`, `get_npc_isboss(390)`,
`GetNpcList(395)`, `GetHidenNpcList(404)`, `get_batch_count(413)`, `add_npc(418)`,
`create_all_npc(456)`, `create_batch_npc(468)`, `add_transfer_npc(498)`.

### `mission_match.lua`
`start_match_timer(10)`, `start_signup(16)`, **`InitMission(27)`**, **`EndMission(32)`**,
`PlayerLeave(38)`, `PlayerJoinMission(64)`, **`OnLeave(88)`**, **`JoinMission(93)`**.
*(4 tên in đậm là callback engine gọi.)*

### `npc_death.lua`
`UpdatePlayerScrore(32)`, `Mission_Complete(47)`, `batch_finish(115)`, `NpcDeathAward(147)`,
**`OnDeath(152)`**.

### `player_death.lua` — **`OnDeath(7)`**
### `chuangguang30_playerdeath.lua` — `OnDeath(1)` *(mã chết)*

### `award.lua`
`award_batch_extend(91)`, `award_hidden_mission(121)`, `award_item(134)`, `award_random_object(149)`,
`award_to_player_success(164)`, `award_player(170)`, `drop_item(216)`, `award_batch_item(231)`,
`award_batch(246)`, `award_batch_contribution(266)`, `GetMissionCompleteAward(271)`, `award_success(280)`.

### `timer_match.lua` — `cancel_match(8)`, `start_match(13)`, `close_match_timer(57)`, **`OnTimer(61)`**
### `timer_board.lua` — **`OnTimer(7)`**
### `timer_close.lua` — `timeout(7)`, **`OnTimer(11)`**

### `npc\dragonboat_main.lua`
`COT_CheckTeamRequirement(4)`, `COT_GetFreeMap(82)`, `want_playboat(104)`,
`COT_TeamMemberJoin(126)`, `COT_TeamJoin(176)`, `dragon_join(209)`.

### `npc\transfer.lua` — **`main(7)`**, `leave(26)`, `gotonewworld(36)`

### `chuangguang30.lua`
`AnnounceLuckyAward(29)` *(khai báo nhưng không nơi nào gọi — **CHƯA XÁC MINH** có script ngoài gọi)*,
`ChuangGuan30:Init(56)`, `:Regist(63)`, `:RegistAll(69)`, `:Unregist(74)`, `:SetState(84)`,
`:OnEnterMap(96)`, `:GiveAward(118)`, `:KickOut(128)`, `:OnLeaveMap(134)`, **`OnDeath(152)`**,
`:OnNpcDeath(155)`, `:OnMessage(185)`, `:KickOutAll(200)`, `:OnAddBoss(212)`, `:FightState(224)`,
`:GameTime(233)`, `:SetForbitItem(243)`, `:AddTime(257)`.
Cuối tệp (`:272-275`) tự chạy `Unregist / SetForbitItem / Init / RegistAll` khi nạp.

### `doubleexp.lua` — `Chuangguan_checkdoubleexp(9)`
### `rank_perday.lua` — `rank_award(15)`, `update_gbtask(31)`, `update_playertask(52)`, `get_top5team(77)`
### `item\chuangguanbaoxiang.lua` — **`main(118)`**, `Oncancel(146)`, `VnCOTBoxNewAward(148)`
### `settings\trigger_challengeoftime.lua` — **`OnTrigger(8)`**
### `vng_feature\...\npcNhiepThiTran.lua` — `tbCOT_Party:CheckCondition(3)`, `:CalcSword(48)`, `:GetDailyTaskCount(60)`, `:GetLHHUseCount(81)`, `OnCancel(92)`

---

## 8. HÀM ENGINE ĐƯỢC GỌI

Đối chiếu 3 nguồn: gọi trong script — có trong `jx_linux_y.luamap.full.txt` — có trong
`D:\GAMEDEVNEW\Sources\Core\Src\ScriptFuns.cpp`.

**Các hàm engine CÓ ở cả bản Linux lẫn JX1** *(bổ sung theo phản biện: thêm `OpenMission`,
`CloseMission`, `GetSeries` — cả 3 đều là hàm engine thật, JX1 đều có:
`ScriptFuns.cpp:14746` / `:14748` / `:14631`)*:

`AddEventItem, AddGlobalNews, AddGoldItem, AddItem, AddMSPlayer, AddNpcEx, AddOwnExp, AddTimer,
CloseMission, OpenMission, GetSeries,
CalcEquiproomItemCount, CalcFreeItemCellCount, CalcItemCount, CallPlayerFunction, ClearMapNpc,
ClearMapObj, ClearMapTrap, ConsumeEquiproomItem, ConsumeItem, CountFreeRoomByWH, DelMSPlayer,
DelTimer, Describe, DisabledUseTownP, DropItem, DynamicExecute, DynamicExecuteByPlayer,
ForbidChangePK, ForbidEnmity, FormatTime2Number, GetCamp, GetCash, GetCurServerTime, GetItemName,
GetLastDiagNpc, GetLastFactionNumber, GetLevel, GetLocalDate, GetMSPlayerCount, GetMSRestTime,
GetMissionS, GetMissionV, GetName, GetNextPlayer, GetNpcName, GetNpcPos, GetNpcSettingIdx, GetPos,
GetSex, GetTask, GetTeamMember, GetTeamSize, GetWorldPos, IL, Include, IncludeLib, IsCaptain,
Ladder_GetLadderInfo, Ladder_NewLadder, LeaveTeam, Msg2MSAll, Msg2Player, Msg2SubWorld, Msg2Team,
NewWorld, OB_Create, OB_Release, Pay, RemoteExecute, Say, SearchPlayer, SetCreateTeam, SetCurCamp,
SetDeathScript, SetFightState, SetLogoutRV, SetMissionS, SetMissionV, SetNpcDeathScript, SetPKFlag,
SetPunish, SetTask, SetTaskTemp, SetTmpCamp, StartMissionTimer, StopMissionTimer, SubWorldID2Idx,
SubWorldIdx2ID, Talk, WriteLog`

Cộng gián tiếp (qua `basemission\lib.lua`): `SetNpcScript`, `SetNpcTimer`, `SetNpcCurCamp`,
`SetNpcParam` — JX1 **đều có**.

**Còn THIẾU ở JX1 (3 hàm) — [đã sửa theo phản biện: bản trước ghi 2 hàm, bỏ sót `JoinMission`]:**

| Hàm | Linux | Gọi tại | Ghi chú |
|---|---|---|---|
| **`JoinMission`** | ELF `0x08137E40` (chuỗi `"JoinMission"` `0x08259E50`, tham chiếu `0x0813826A`) | `dragonboat_main.lua:163` | 🔴 **CHẶN CỬA — `grep -rni joinmission D:\GAMEDEVNEW\Sources` = 0 hit.** JX1 KHÔNG có hàm engine này; API mission của JX1 chỉ có `AddMSPlayer(MissionId, groupid)` (`ScriptFuns.cpp:14757`). Phải viết mới `LuaJoinMission` (gọi hàm script `JoinMission` của mission slot) **hoặc** sửa script gọi thẳng `PlayerJoinMission(1)` |
| `AddStatData` | ELF `0x080FF550` | `chuangguanbaoxiang.lua:169` | thống kê; có thể stub rỗng |
| `NpcName2Replace` | ELF có | `transfer.lua:10` | đã được bọc `if NpcName2Replace then` ⇒ vô hại |

**Không phải hàm engine mà là hàm script** (đừng nhầm khi port):
`GetTabFileData` — định nghĩa ở `script\lib\file.lua:46`, gọi `TabFile_Load` + `TabFile_GetCell`.
JX1 **có** `TabFile_Load/GetCell/GetRowCount/UnLoad` (`ScriptFuns.cpp:3052..3160`), bản JX1 còn
hỗ trợ tra theo **tên bảng** ⇒ chỉ cần port `lib\file.lua` là chạy.

**Callback engine → script** cần đúng tên:
`InitMission` (JX1 chấp nhận cả `BeginMission` lẫn `InitMission`, `ScriptFuns.cpp:11160-11176`),
`EndMission`, `JoinMission(RoleIndex, camp)`, `OnLeave(index)`, `OnTimer()`, `OnDeath(index)`,
`OnTrigger()`, `main()`.

Về `JoinMission(nMissionId, nCamp)` engine (Linux `0x08137E40`): kiểm ≥2 tham số, đọc mission id
và camp, dựng một tập chỉ số rồi gọi hàm script tên `"JoinMission"` (chuỗi tại `0x08259E50`,
tham chiếu ở `0x0813826A`). **CHƯA XÁC MINH**: engine có tự gom cả tổ đội hay chỉ xử lý người gọi;
tuy nhiên `dragonboat_main.lua` gọi nó **một lần cho mỗi thành viên** (`CallPlayerFunction` →
`COT_TeamMemberJoin` → `JoinMission`), khớp với khuôn của mọi mission khác trong bản Linux
(`battles\*\mission.lua`, `missions\citydefence\...`).

---

## 9. PHỤ THUỘC DỮ LIỆU NGOÀI

### 9.1 Bản đồ
- 464..479 (cấp 1), 480..495 (cấp 2), 957 (Mật Phòng), 11 (điểm hồi sinh cứng),
  208 (đại lao — chặn báo danh).

### 9.2 NPC (npcs.txt, **dòng = id + 2**) **[đã sửa theo phản biện — bản trước ghi "id + 1"]**

`npcs.txt` có **1 dòng tiêu đề** và id **bắt đầu từ 0** ⇒ npc id `N` nằm ở **dòng `N+2`**.
Kiểm chứng: `map_new_Ncp[11].szName = "Vượt ải _Toàn Cơ Tử"` (id 1683) đúng bằng cột `Name`
của **dòng 1685**; npc id 975 = " (sơ cấp) Sương Đao" đúng với `npc.lua:30`.

| Dải | Nội dung |
|---|---|
| 975..993 | quái thường cấp 1 (Sương Đao, Phi Sa, Sương Liêm, Thừa Phong, Thuỷ Quỷ, Thần Tý, Tranh Hồn, Phù Lang, Ảnh Côn, Đao Tý, Lang Bằng, Hắc Cân, 4 "npc chú thuật/tử ngữ/hồn mê/giảm tốc", npc siêu mạnh) |
| 994..1001 / 1002..1005 | tiểu Boss nam 1-8 / nữ 1-4 (cấp 1) |
| 1006 | "đại Boss" cấp 1 = Nhiếp Thi Trần ải 27 |
| 1007..1025 | quái thường cấp 2 |
| 1026..1033 / 1034..1037 | tiểu Boss nam / nữ (cấp 2) |
| 1038 | "đại Boss" cấp 2 = Nhiếp Thi Trần ải 27 |
| 1673 | Tiểu Nhiếp Thi Trần (chế độ 2011) |
| 1674..1683 | 10 boss Mật Phòng |
| 1684 | NPC tiếp dẫn Mật Phòng |
| 769 | Nhiếp Thí Trần — NPC báo danh (`autoexec_npc.lua:26-32`) |

**Đối chiếu JX1** (`bin\server\settings\npcs.txt`, 2035 dòng dữ liệu = id 0..2034):
tính năng dùng **77 npc id**, **74/77 trùng khớp nguyên văn**
**[đã sửa theo phản biện — bản trước ghi 73/76]**. **3 id XUNG ĐỘT** — JX1 đã dùng lại:
```
1032 : Linux "(cao cấp) tiểu Boss nam 7"  →  JX1 "Boss New Dragon 165"
1033 : Linux "(cao cấp) tiểu Boss nam 8"  →  JX1 "Boss New Dragon 166 8"
1034 : Linux "(cao cấp) tiểu Boss nữ 1"   →  JX1 "Boss New Dragon 167"
```
`tbRangeId[2] = {{1026,1033},{1034,1037}}` chạm đúng 3 id này ⇒ port thẳng sẽ sinh
"Boss New Dragon" mang tên người chơi. **Phải đổi dải hoặc dời 3 NPC Dragon.**

### 9.3 Vật phẩm

| Dùng | Item | Vai trò |
|---|---|---|
| 6/1/400 (level 20..90) | Sát thủ giản | vé vào (trừ 1/người) |
| 6/1/2742 | Bảo Rương Vượt Ải | thưởng chế độ 2011 |
| 6/1/2744 / 6/1/30191 | Chìa Khóa Như Ý / Chìa Khóa Vàng | mở rương |
| 6/1/1392 | Hộp lễ vật vượt ải | rơi 5% khi có hoa |
| 6/1/2116 | Thiên Niên Linh Dược | thưởng bảng xếp hạng ngày |
| 1/2/0 lv5 | bông hoa (`DropItem`) | rơi theo ải |
| gold 0/2..167 + 6/1/{22,26,71..73,122..124} + event 238..240, 353, 489 | bảng ải ẩn | §6.3 |
| 6/1/3810, 3811, 30557, 215, 6/0/3, 6/0/6, 30533, 30538, 30009, 30010 | thưởng boss Mật Phòng | §6.4 |
| 6/1/4134, 30228, 30229, 30301, 3203, 30289, 30529, 30537, 30506, 30507, 30006, 30505 | rương chìa vàng | §6.6 |

**Đối chiếu JX1** (`bin\server\settings\item\magicscript.txt`, 4866 dòng) — **XUNG ĐỘT NẶNG**:
```
6/1/400   : Linux "Sát thủ giản"          →  JX1 "Sư đồ thiếp"
6/1/1392  : Linux "Hộp lễ vật vượt ải"    →  JX1 "Bao nguyên liệu An Bang hoàn hảo"
6/1/2116  : Linux "Thiên Niên Linh Dược"  →  JX1 "Bột mì trộn mật ong"
6/1/2742  : Linux "Bảo Rương Vượt ải"     →  JX1 "Bảo Rương Tử Mãng Khí Giới"
6/1/2744  : Linux "Chìa Khóa Như ý"       →  JX1 "Tử Mãng Quy Nguyên Phù"
6/1/30191 : Linux "Chìa khóa vàng"        →  JX1 KHÔNG CÓ
```
⇒ **bắt buộc ánh xạ lại toàn bộ item id khi port** (không được bê nguyên số).

### 9.4 Task id

| Task | Ý nghĩa | JX1 |
|---|---|---|
| 1505 | cờ "đang trong Vượt Ải" | trống |
| 1550 | số lượt còn lại trong ngày | JX1 **đã dùng cùng nghĩa** ở `script\event\storm\function.lua:396,417` (cùng nguồn Linux) ⇒ tương thích |
| 1551 | ngày tham gia gần nhất | trống |
| 2606 | số ải đã qua (sự kiện sinh nhật 200905) | trống |
| 2636/2637/2638/2639 | xếp hạng cá nhân (ngày/điểm lần 1 và lần 2) | trống |
| 2641/2642 | ngày / số lần dùng Long Huyết Hoàn | trống |
| 2852 | `PLAYER_MAP_TASK` — map gốc để chia nhóm ở 957 | trống |
| 3071 (bit 25) | cờ cấm tham gia (VN) | trống |
| 3079 | đếm lượt báo danh trong ngày (`AddTaskDaily`) | trống |
| 4018 | lượt Long Huyết Hoàn còn khả dụng | trống — JX1 `MAX_TASK = 4200` (`KPlayerTask.h:18`) nên vừa |
| TaskTemp 200 | cờ tạm "đang trong hoạt động" | — |
| `STORM_TASKID_GAMEID_SS` | khoá ván của hệ Bão | `script\event\storm\head.lua` (JX1 đã có) |

### 9.5 Mission id / Timer id

- Mission **22**: Linux `settings\task\missions.txt` **dòng 23** (MISSION=22, có 1 dòng tiêu đề)
  → `\script\missions\challengeoftime\mission_match.lua`.
  **JX1 MISSION 22 = `\script\missions\mission_trong.lua` (ô trống) ⇒ slot 22 rảnh, dùng được.**
- Timer **41/42/43**: Linux `settings\timertask.txt` dòng 42/43/44.
  **JX1 `timertask.txt` chỉ có 36 mục, các id đang dùng là 1-10, 12-18, 20, 21, 50-55, 61-62,
  65-70, 75-77 (11 và 19 cũng trống) ⇒ 41/42/43 rảnh.**
  **[đã sửa theo phản biện — bản trước ghi "1-21" và trích số dòng bằng số id]**

### 9.6 Tệp settings khác

| Tệp | Trạng thái JX1 |
|---|---|
| `settings\maps\challengeoftime\lineup{8,16,20,24,32,40,56}.txt` | **CHƯA CÓ** — phải chép (7 tệp, cột `XPOS/YPOS`, toạ độ ô) |
| `settings\maps\liandandong\npc_3.txt` (50 dòng, `TRAPX/TRAPY`, đã ×32) | **CHƯA CÓ** |
| `settings\trigger_challengeoftime.lua` | **CHƯA CÓ** |
| `settings\trigger_include.lua` | cần kiểm (Linux `settings\trigger_include.lua`) |
| `settings\task\tollgate\killer\killer.txt` | dùng bởi `nieshichen.lua:54` (`new(KTabFile, ...)`) |
| `settings\item\004\magicscript.txt` dòng 3349 | JX1 dùng `settings\item\magicscript.txt` (đường dẫn khác) |
| `Maps\WorldSet_GameServer.ini` | thêm map **957** |

### 9.7 Hệ thống ngoài phải kéo theo

`RELAYLADDER` (JX1 **đã có**, `KJx2SharedStore.cpp:531/599/580`, TOP=10, id phải > 10000 —
10119/10179/10180/10235 đều hợp lệ) · `EventSys` type `"ChuanGuan"` với 2 sự kiện `OnEnter`,
`OnPass` (`script\misc\eventsys\type\func.lua:10-14`) · `G_ACTIVITY:OnMessage("Chuanguan"/"SignUpChuangguan")` ·
`BigBoss:AddChuangGuanPoint` / `:RemoveChuangGuanBonus` (`script\missions\boss\bigboss.lua:275,253`) ·
`tongaward_challengeoutoftime` (`script\tong\tong_award_head.lua:41`) ·
`tbChangeDestiny:completeMission_NieShiChen` (`script\event\change_destiny\mission.lua`) ·
`tbAwardTemplet` · `PlayerFunLib` · `tbLog` · `tbVNG_BitTask_Lib` ·
`storm_*` (`script\event\storm\function.lua`, JX1 đã có) ·
`tbBirthday0905` (`script\event\birthday_jieri\200905\`) ·
`tbTalkDailyTask` (`script\activitysys\config\32\talkdailytask.lua`) ·
`tbvng_ChestExpAward`, `TransLife6`, `tbVnX2Award:X2ChallengeOfTime`
(`script\vng_feature\double_mission_award.lua`) ·
`set_MapType` / `FORBITMAP_LIST` (`script\item\forbiditem.lua`, `script\global\forbidmap.lua` — JX1 đã có).

---

## 10. LỖI THẬT PHÁT HIỆN TRONG BẢN LINUX (đừng port nguyên)

### 10.1 Sai tên hàm → điểm xếp hạng cá nhân KHÔNG BAO GIỜ được ghi
`npc_death.lua:32` định nghĩa `function UpdatePlayerScrore(time)` (thiếu chữ — "Scrore"),
nhưng `npc_death.lua:93` gọi `CallPlayerFunction(tbPlayerList[i], **UpdatePlayerScore**, time)`.
Toàn cây script chỉ có đúng 2 chỗ này (grep `UpdatePlayerScore*` → 2 hit). Biến `UpdatePlayerScore`
là **nil** ⇒ task 2636/2637/2638/2639 không bao giờ được ghi từ trận đấu ⇒ `rank_award()` luôn
trả "vẫn chưa đủ điều kiện nhận thưởng".

### 10.2 Điều kiện lọc NPC ở Mật Phòng luôn SAI ⇒ giết bất kỳ NPC nào cũng ăn thưởng
`chuangguang30.lua:162`:
```lua
if nNpcId < map_new_Ncp[2].nNpcId and nNpcId > map_new_Ncp[11].nNpcId then return end
-- = if nNpcId < 1674 and nNpcId > 1683 then  ->  KHÔNG BAO GIỜ ĐÚNG
```
Phải là `or`. Hậu quả: mọi NPC chết trên map 957 đều kích hoạt phát thưởng 20 triệu exp + vật phẩm
rồi `KickOutAll()`.

### 10.3 `bfind` khai báo `local` trong khối `if` ⇒ nhánh "chưa có tên trên bảng" chết
`npc_death.lua:71` `local bfind = 0` nằm **bên trong** `if LadderId then ... end` (kết thúc ở dòng 82).
Tới `npc_death.lua:84` `if (bfind == 0)` thì `bfind` đã là biến **toàn cục = nil** ⇒ so sánh sai ⇒
`Ladder_NewLadder(LadderId, ...)` ở dòng 85 **không bao giờ chạy**. Chỉ ladder ngày (10235, dòng 88)
được ghi; ladder 10179/10180 chỉ được ghi ở dòng 77 (nhánh "đã có tên và chậm hơn").
*(Nhánh dòng 76 `if laddertime > value` cũng khả nghi: `value` lưu dạng âm còn `laddertime` dương
⇒ điều kiện gần như luôn đúng. **CHƯA XÁC MINH** ý đồ gốc.)*

### 10.4 `chuangguang30_playerdeath.lua` là mã chết
`SetDeathScript` ở `chuangguang30.lua:112` trỏ vào **chính `chuangguang30.lua`** (có `OnDeath`
ở dòng 152). Grep toàn cây: **0 tham chiếu** tới `chuangguang30_playerdeath.lua`.

### 10.5 Cửa chặn giờ ở `transfer.lua:38` không bao giờ đúng
```lua
if ndate > 22 and ndate < 10 then  -- một số không thể vừa >22 vừa <10
```
Ý định là `or`. Thực tế cửa giờ được chặn ở khâu báo danh (`dragonboat_main.lua:196`) nên hậu quả nhẹ.

### 10.6 `cancel_match()` rỗng ⇒ mission kẹt STATE 2
`timer_match.lua:8-10` thân rỗng (chỉ còn dòng comment). Khi hết giờ báo danh mà 0 người,
`start_match()` vẫn đã đặt `VARV_STATE = 2` ở dòng 15 **trước** khi kiểm ⇒ map đó bị coi là
"đang thi đấu" cho tới trigger giờ kế tiếp. Đây chính là lý do `COT_GetFreeMap` phải lọc
`nState == 1 and nMSPlayerCount == 0`.

### 10.7 `get_random_npc_id(2)` trả nil
`include.lua:269-273`: giới tính 2 ("nhân yêu") không được xử lý ⇒ `AddNpcEx(nil, ...)`.
Chỉ xảy ra nếu game có nhân vật `GetSex() == 2`.

### 10.8 Hằng số chết
`INTERVAL_MATCH`, `SEX_RENYAO`, `USE_NAME_ALL_PLAYERS`, `USE_NAME_THE_TOPLIST`,
`CHUANGGUAN30_START_TIME`, `CHUANGGUAN30_END_TIME`, `TSK_Longxuewan_Use` (đọc ở `longxuewan.lua`
nhưng không ở nhánh Vượt Ải) — **0 nơi dùng** ngoài `include.lua`.
**[đã sửa theo phản biện: bỏ `AWARD_COUNT` khỏi danh sách này — nó CÓ được đọc ở
`chuangguang30.lua:168`, nhưng giá trị đọc ra bị vứt đi (xem §6.4), tức là *biến chết* chứ
không phải *hằng không ai đọc*.]**
Giờ 10/22 bị hard-code ở `dragonboat_main.lua:196` và `transfer.lua:38`.

### 10.9 Bảng `map_random_awards` bị định nghĩa 2 lần
`npc.lua:258-306` và `award.lua:7-55` — nội dung **giống hệt** (diff = 1 ký tự `;`).
`npc_death.lua` Include npc.lua (dòng 6) rồi award.lua (dòng 7) ⇒ bản award.lua thắng.

### 10.10 Lệch giữa văn bản và mã
- `nieshichen.lua:48`: "Thời gian báo danh là **5 phút**" — mã là 10 phút (`TIME_SIGNUP=10`).
- `nieshichen.lua:48`: "Mỗi người chỉ tối đa **2 lần/ngày**" — mã `COUNT_LIMIT = 1`
  (+2 lần Long Huyết Hoàn ⇒ tối đa 3, đúng như `npcNhiepThiTran.lua:25`).
- `dragonboat_main.lua:116`: "nộp **10 vạn** ngàn lượng" — mã `Pay(10000)` = 1 vạn.
- `npc_death.lua:99` comment "12 phút" — mã `20 * 60`.

---

## 11. TÓM TẮT KHẢ NĂNG PORT SANG JX1

**Thuận lợi**
1. 32/32 bản đồ 464-495 đã có trong `maplist.ini` **và** đã nằm trong `WorldSet_GameServer.ini`
   của máy chủ đang chạy ⇒ không cần đụng dữ liệu bản đồ.
2. **74/77** NPC id đã có sẵn nguyên văn trong `npcs.txt` của JX1, kể cả bộ 1673-1684 của chế độ 2011
   **[đã sửa theo phản biện — bản trước ghi 73/76]**.
3. Mission slot **22** và timer slot **41/42/43** đều **rảnh** ở JX1.
4. **Hầu hết hàm engine** đã có — **thiếu 3** (`JoinMission`, `AddStatData`, `NpcName2Replace`),
   trong đó `JoinMission` là **chặn cửa** **[đã sửa theo phản biện — bản trước ghi "91/93, thiếu 2"]**.
   Hệ `RELAYLADDER` (Ladder_NewLadder / GetLadderInfo / ClearLadder)
   đã port sẵn ở `KJx2SharedStore.cpp` (`JX2LADDER_TOP=10`, `JX2LADDER_MIN_ID=10000`).
5. `MAX_MISSION_VALUE_COUNT = 100` > 33 biến số cần dùng; chuỗi mission đi kho riêng ⇒ không đụng nhau.

**Việc phải làm**
0. 🔴 **Viết `JoinMission` cho engine JX1** (hoặc bỏ nó khỏi script) — xem §8.
   Không có bước này thì không ai vào được ải. **[thêm theo phản biện]**
1. **Ánh xạ lại item id** — **không chỉ 6 item chính**: thêm 8 xung đột nữa
   (6/1/122·123·124 lệch 1 bậc, 6/1/215, 6/1/3810, 6/1/3811, 6/1/4134, 6/1/3203)
   và **16 item dải 6/1/30xxx KHÔNG TỒN TẠI** trong JX1, cùng 6/0/3 và 6/0/6.
   Xem "Bỏ sót đã tìm thêm" ở phụ lục. **[đã sửa theo phản biện]**
2. **Né 3 NPC id 1032/1033/1034** (JX1 = Boss New Dragon) hoặc dời dải `tbRangeId[2]` (§9.2).
3. Thêm map **957** vào `Maps\WorldSet_GameServer.ini` (dữ liệu bản đồ đã có qua map 967).
4. Chép 7 tệp `lineup*.txt` + `liandandong\npc_3.txt` + `trigger_challengeoftime.lua`.
5. Port `script\lib\file.lua` (`GetTabFileData`) — JX1 đã có `TabFile_*` nên chỉ thiếu lớp bọc.
6. Bổ sung/stub `AddStatData`.
7. Port 54 tệp phụ thuộc chưa có (đáng kể: `missions\boss\bigboss.lua`, `tong\tong_award_head.lua`
   + `tong_header/tong_setting/contribution_entry`, `battles\battlehead.lua`,
   `activitysys\config\32\*`, `event\change_destiny\*`).
8. **Chú ý `ClearMapNpc(world, 1)`**: JX1 (`KJx2WarInfra.cpp:66-90`) **bỏ qua tham số thứ 2**
   ⇒ `close_match()` sẽ không dọn người/pet như bản gốc.
9. Vá **8** lỗi: §10.1-10.6 + 2 lỗi bổ sung ở phụ lục phản biện
   (`ChuangGuan30:OnEnterMap` thiếu `return`; ải 28 / ải ẩn cấp 2 sinh boss cấp 75).
   **[đã sửa theo phản biện]**



---

## PHỤ LỤC PHẢN BIỆN (tác tử độc lập)

Ngày kiểm: 24/08. Phương pháp: mặc định coi mọi khẳng định là **SAI** cho tới khi mở đúng tệp gốc
(hoặc ELF) chứng minh ngược lại. Tệp `.lua` đọc bằng `gbktool.py read` (GBK/TCVN3), bảng `.txt`
đọc bằng Python `decode('gbk')`, nguồn C++ JX1 đọc trực tiếp, ELF đọc bằng `re_disasm.py`.

**Tổng kết: 34 khẳng định được kiểm chứng — 27 ĐÚNG, 7 SAI (đã sửa vào thân bài), 0 không xác minh được.**

### A. Bảng kiểm chứng

| # | Khẳng định (mục) | Bằng chứng gốc | KẾT LUẬN | Sửa lại thành |
|---|---|---|---|---|
| 1 | §1.1 Hằng số: `MISSION_MATCH=22`, `TIMER 41/42/43`, `LIMIT_SIGNUP=600`, `LIMIT_FINISH=1800`, `BOAT_POS 1559/3226`, `LIMIT_PLAYER_COUNT=8`, `CHUANGGUAN30_MAP_ID=957`, `_TIME_LIMIT=13*60`, `PLAYER_MAP_TASK=2852`, `COUNT_LIMIT=1`, task 1550/1551/2641/2642/4018 | `include.lua:6-20, 27-32, 117-119` — khớp từng dòng | **ĐÚNG** | — |
| 2 | §4 Trần biến mission = 33 (`VARV_PLAYER_SERIES=22`, `VARV_LEVEL=31`, `_XIAONIESHICHEN_BATCH=32`, `_BATCH_MODEL=33`) | `include.lua:70,73,75,76` (tính từ `VARV_PLAYER_SEX=13` + `LIMIT_PLAYER_COUNT=8`) | **ĐÚNG** | — |
| 3 | §4 JX1 `MAX_MISSION_VALUE_COUNT=100`; chuỗi mission đi kho riêng `s_MissionStr` khoá `subworldIdx*4096+id` | `KMission.h:122`; `KJx2WarInfra.cpp:1034` (`static std::map<long,std::string> s_MissionStr;`) | **ĐÚNG** (line 1034 chính xác) | — |
| 4 | §1.3 NPC 769 đặt ở 7 map `11/1/37/176/162/78/80` với đúng 7 cặp toạ độ | `script\global\autoexec_npc.lua:26-32` — 7 dòng khớp nguyên văn cả toạ độ lẫn đường dẫn script | **ĐÚNG** | — |
| 5 | §1.3 Điều kiện tổ đội: đội trưởng, ≥4 người, ≥10000 lượng, bit 3071/25, `CalcEquiproomItemCount(6,1,400,i)` i=20..80 (cấp 1) / 90 (cấp 2) | `dragonboat_main.lua:6, 11, 16, 30, 58-59, 65` | **ĐÚNG** | — |
| 6 | §2.2 `VARV_BATCH_MODEL=1` chỉ khi cấp 2 **và** 10 ≤ giờ ≤ 22 | `dragonboat_main.lua:196-200` (`ndate <= 22 and ndate >= 10 and nMatchLevel == 2`) | **ĐÚNG** | — |
| 7 | §1.4 `start_match()` đặt `VARV_STATE=2` **trước** khi kiểm số người; `VARV_XIAONIESHICHEN_BATCH = random(5,15)` | `timer_match.lua:15` rồi `:17-18`; `:35-36` | **ĐÚNG** | — |
| 8 | §5.3 Bảng thưởng 28 ải × 2 cấp (1.5/1.5/3/9/3/4.5/4.5/6+10/… và 2/2/3/9/3/5/5/9+15/…) | `npc.lua:29-120` (cấp 1) và `npc.lua:134-225` (cấp 2) — **đối chiếu đủ 56 ô, khớp 56/56** | **ĐÚNG** | — |
| 9 | §5.1 `NPC_ATTRIDX_LEVEL` = 75 (cấp 1) hoặc 95 (cấp 2) | `npc.lua:225` ải 28 **cấp 2** ghi `75`; `npc.lua:230,232` hai mục `tbHidenNpc` cấp 2 cũng ghi `75` (và `series` là `-1` chứ không phải `func_npc_getseries`) | **SAI** | "75 (cấp 1) / 95 (cấp 2), **trừ 3 ngoại lệ ở cấp 2 vẫn ghi 75**" — đã sửa §5.1 |
| 10 | §5.5 `map_new_Ncp[2..11]` = npc 1674..1683; `[1]` = 1673 dạng mảng 8 cột; `map_transfer_npc[1]` = 1684 | `npc.lua:238-255` khớp từng dòng | **ĐÚNG** | — |
| 11 | §6.2 Công thức thưởng hoàn thành `floor(172*(1-min/25))+10` / `floor(233*(1-min/25))+20`, sàn 15/30 khi min ≥ 25 | `award.lua:58-83` | **ĐÚNG** | — |
| 12 | §6.3 Bảng ải ẩn: cơ số 100000; 0.2/0.19587/0.18 = 6/1/124·123·122; 0.05 = event 489; 0.0005 = 6/1/26, 6/1/22 + 5 món gold 0/159..163; 0.0001 = 0/164..167 | `award.lua:7-29` | **ĐÚNG** | — |
| 13 | §10.9 `map_random_awards` bị định nghĩa 2 lần, khác nhau đúng 1 ký tự `;` | `difflib` giữa `npc.lua:258-306` và `award.lua:7-55` → **8 dòng diff, thay đổi thật sự duy nhất là `};` ⇄ `}`** | **ĐÚNG** | — |
| 14 | §6.4 `local nCount = AWARD_COUNT` ở `chuangguang30.lua:172` | Dòng thật là **168**; dòng 175 mới là `CallPlayerFunction(..., self.GiveAward, self, nGroup, 2)` | **SAI** (lệch 4 dòng) | `chuangguang30.lua:168` — đã sửa §6.4 |
| 15 | §10.8 `AWARD_COUNT` — "0 nơi dùng ngoài `include.lua`" | Mâu thuẫn với chính §6.4; `chuangguang30.lua:168` CÓ đọc | **SAI** | Bỏ `AWARD_COUNT` khỏi danh sách hằng chết; nó là **biến chết** (đọc rồi vứt) — đã sửa §10.8 |
| 16 | §10.2 `chuangguang30.lua:162` dùng `and` thay `or` ⇒ mọi NPC chết ở 957 đều ăn thưởng | `chuangguang30.lua:162` `if nNpcId < map_new_Ncp[2].nNpcId and nNpcId > map_new_Ncp[11].nNpcId`; `[2].nNpcId=1674`, `[11].nNpcId=1683` (`npc.lua:240,249`) ⇒ `<1674 and >1683` không bao giờ đúng | **ĐÚNG** | — |
| 17 | §10.1 `UpdatePlayerScrore` (`:32`) vs gọi `UpdatePlayerScore` (`:93`), toàn cây chỉ 2 hit | `gbktool grep "UpdatePlayerScr?ore"` trên `D:\ServerLinux\server1\script` → đúng **2 hit**, đúng 2 dòng | **ĐÚNG** | — |
| 18 | §10.3 `local bfind` nằm trong khối `if LadderId then` (`:71`, đóng `:82`), dùng ở `:84` ⇒ nhánh `:85` chết | `npc_death.lua:70-86` | **ĐÚNG** (bổ sung: dòng 85 còn ghi `laddertime` **dương**, ngược dấu với `:77`/`:88`) | — |
| 19 | §10.4 `chuangguang30_playerdeath.lua` = mã chết, 0 tham chiếu | grep toàn `D:\ServerLinux\server1` → **0 hit**; `SetDeathScript` ở `chuangguang30.lua:112` trỏ về chính nó | **ĐÚNG** | — |
| 20 | §10.5 `transfer.lua:38` `ndate > 22 and ndate < 10` không bao giờ đúng | `transfer.lua:38` nguyên văn | **ĐÚNG** | — |
| 21 | §10.6 `cancel_match()` thân rỗng | `timer_match.lua:8-10` chỉ còn 1 dòng comment | **ĐÚNG** | — |
| 22 | §3.2/§3.3 JX1: 32/32 map 464-495 có trong `maplist.ini`, cùng thư mục `特殊用地\杀手的试炼`; 957 có trong maplist (`安期炼丹洞`, tên còn tiếng Trung `闯关密室`); max map id JX1 = 1002 | Quét `bin\server\settings\maplist.ini`: 464 ở dòng 3695 … 495 ở dòng 3804; 957 dòng 6233-6234; 967 dòng 6281-6282 cùng thư mục; max id = 1002 | **ĐÚNG** | — |
| 23 | §3.3 `WorldSet_GameServer.ini` `Count=910`; 464-495 **đều được nạp**; **957 KHÔNG**; 967 có | Đếm thật: 910 mục `WorldNNN=`; `464..495 ⊂ tập nạp`; 957 = False; 950/959/960/961/962/963/964/967 = True | **ĐÚNG** | — |
| 24 | §3.3 `Maps\*_srv.fp` (86 tệp) chỉ là cache A* của bot | `KSubWorld.cpp:3397` `sprintf(szFile, "\\maps\\%d_srv.fp", m_SubWorldID)` + comment; đếm thật thư mục `Maps\` = **86** tệp `_srv.fp` | **ĐÚNG** | — |
| 25 | §9.2 `npcs.txt`, **dòng = id + 1** | Đối chiếu 3 mỏ neo: id 1683 (`map_new_Ncp[11]` = "Vượt ải _Toàn Cơ Tử") nằm ở **dòng 1685**; id 975 ("Sương Đao") ở dòng **977**; id 1674 ("Vượt ải_Cổ Bách") ở dòng **1676** ⇒ **dòng = id + 2** | **SAI** | `dòng = id + 2` (có 1 dòng tiêu đề, id bắt đầu từ 0) — đã sửa §9.2 |
| 26 | §9.2 "73/76 id trùng khớp", 3 id xung đột 1032/1033/1034 = "Boss New Dragon 165/166/167" | Tập id tính năng dùng = **77**; diff Linux↔JX1 với offset đúng = **đúng 3** id lệch, đúng 3 id đó, đúng 3 tên đó | **SAI phần đếm / ĐÚNG phần xung đột** | "**74/77** trùng khớp" — đã sửa §9.2 và §11 |
| 27 | §9.3 6 xung đột item: 6/1/400 → "Sư đồ thiếp"; 1392 → "Bao nguyên liệu An Bang hoàn hảo"; 2116 → "Bột mì trộn mật ong"; 2742 → "Bảo Rương Tử Mãng Khí Giới"; 2744 → "Tử Mãng Quy Nguyên Phù"; 30191 → không có | Tra theo **cột** Genre/DetailType/ParticularType (không tra theo số dòng — tệp Linux `004` bị trôi dòng): cả 6 đều khớp nguyên văn | **ĐÚNG** (nhưng **thiếu** — xem mục B) | — |
| 28 | §6.6 Item 6/1/2742 nằm ở `settings\item\004\magicscript.txt` **dòng 3349**, tên "Bảo Rương Vượt ải", script trỏ đúng `chuangguanbaoxiang.lua`, stack 50 | Dòng 3349 nguyên văn khớp cả 5 chi tiết | **ĐÚNG** | — |
| 29 | §9.5 Mission 22 ở JX1 = `mission_trong.lua` ⇒ rảnh; timer 41/42/43 rảnh, JX1 chỉ có 36 mục, "id đang dùng là **1-21**, 50-55, 61-62, 65-70, 75-77" | `missions.txt` JX1: MISSION 22 = `\script\missions\mission_trong.lua` ✔; `timertask.txt` JX1 = 36 mục, id thật = **1-10, 12-18, 20, 21**, 50-55, 61-62, 65-70, 75-77 (thiếu 11 và 19) | **SAI phần liệt kê / ĐÚNG kết luận** | "1-10, 12-18, 20, 21, …" — đã sửa §9.5 |
| 30 | §9.4 14 task id (1505, 1551, 2606, 2636-2639, 2641, 2642, 2852, 3071, 3079, 4018) đều trống ở JX1; 1550 đã dùng **cùng nghĩa** ở `storm\function.lua:396,417` | grep `(Set|Get)Task\(<id>` trên `bin\server\script` của JX1: **0 hit** cho cả 14 id; 1550 đúng 2 hit ở `event\storm\function.lua:396` và `:417`, comment `杀手进行次数` | **ĐÚNG** | — |
| 31 | §9.6 JX1 **chưa có** `settings\maps\challengeoftime\` và `settings\maps\liandandong\`; 7 tệp `lineup*.txt` bên Linux có thật, định dạng `XPOS\tYPOS`, toạ độ **ô** (chưa ×32); `npc_3.txt` 50 dòng, đã ×32 | `ls` JX1 → cả 2 thư mục "No such file"; lineup8/16/20/24/32/40/56 = 9/17/21/25/33/41/57 dòng, hàng đầu `XPOS\tYPOS`, giá trị 1542/3222; `npc_3.txt` = 51 dòng, `TRAPX/TRAPY`, hàng 2 = `51072 102272` = đúng `1596×32 / 3196×32` trùng khít `transfer.lua:55` `NewWorld(957, floor(51072/32), floor(102272/32))` | **ĐÚNG** | — |
| 32 | §8 "**91/93** hàm engine JX1 ĐÃ CÓ, thiếu **đúng 2**: `AddStatData` và `NpcName2Replace`" | Bóc toàn bộ định danh `X(` trong 20 tệp của tính năng, giao với `jx_linux_y.luamap.full.txt`, trừ với bảng đăng ký `{"tên", Lua…}` của **mọi** `Sources\Core\Src\*.cpp` (1021 tên): thiếu **3** — `AddStatData`, `NpcName2Replace` và **`JoinMission`**. `grep -rni joinmission D:\GAMEDEVNEW\Sources` = **0 hit**; ELF Linux có `0x08137E40` + chuỗi `"JoinMission"` `0x08259E50` (tham chiếu `0x0813826A`, `0x082E2DB0`) | **SAI** | "thiếu **3** hàm; `JoinMission` là **chặn cửa**" — đã sửa §8 và §11 |
| 33 | §8 `GetTabFileData` là hàm script (`lib\file.lua:46`), JX1 có `TabFile_Load/GetCell/GetRowCount/UnLoad` (`ScriptFuns.cpp:3052..3160`) | `LuaTabFile_Load:3052`, `_GetCell:3092`, `_GetRowCount:3140`, `_UnLoad:3153`; đăng ký ở `:15284-15287`; `"GetTabFileData"` = 0 hit trong `ScriptFuns.cpp` | **ĐÚNG** | — |
| 34 | §11.8 JX1 `ClearMapNpc` **bỏ qua tham số thứ 2** (`KJx2WarInfra.cpp:66-90`) | `LuaClearMapNpc` bắt đầu dòng **66**, thân chỉ đọc `Lua_IsNumber(L,1)`, không hề chạm index 2 | **ĐÚNG** | — |
| 35 | §6.5 `rank_award()` nhận trong khung `0400 < HHMM <= 2300` | `rank_perday.lua:17` = `if (ntime < 4 or ntime > 2300)` ⇒ khung thật là **HHMM ≥ 4**, tức **00:04**, không phải 04:00 | **SAI** | `4 <= HHMM <= 2300` (00:04 → 23:00); khi port nên đổi `< 4` thành `< 400` — đã sửa §6.5 |
| 36 | §9.7 EventSys type `"ChuanGuan"` có đúng 2 sự kiện `OnEnter`/`OnPass` ở `func.lua:10-14`; `BigBoss:AddChuangGuanPoint` `bigboss.lua:275`, `:RemoveChuangGuanBonus` `:253`; `tongaward_challengeoutoftime` `tong_award_head.lua:41` (4/8/10 × `COEF_CONTRIB_TO_VALUE`, `EVE_CHUNGGUAN`); `g_npcdeath.lua:72` gọi `ChuangGuan30:OnNpcDeath`; task 2606 = `taskctrl.lua:36` | Mở cả 5 tệp — **khớp từng dòng một** | **ĐÚNG** | — |
| 37 | §9.7 RELAYLADDER JX1 đã có, TOP=10, id phải > 10000 | `KJx2SharedStore.cpp:440` `JX2LADDER_TOP 10`, `:441` `JX2LADDER_MIN_ID 10000`, `:538` `if (uId <= JX2LADDER_MIN_ID) return 0;`; `LuaLadder_NewLadder:531`, `LuaLadder_ClearLadder:580` | **ĐÚNG** | — |
| 38 | §0.2 Bao đóng: 103 tệp, 16 seed, 0 thiếu ở Linux, 54 tệp JX1 chưa có / 49 đã có; `bigboss.lua`/`tong_award_head.lua`/`battlehead.lua`/`lib\file.lua`/`talkdailytask.lua` = chưa có, `forbiditem.lua`/`forbidmap.lua`/`basemission\lib.lua`/`awardtemplet.lua` = đã có | Đọc `closure3.json["vuotai"]`: 103 / 16 / 0 / 54 / 49 và **cả 9 cờ `in_jx1` đều khớp** | **ĐÚNG** | — |

*(Ghi chú đánh số: bảng liệt kê 38 dòng, trong đó 4 dòng là nhóm gộp nhiều khẳng định con;
tổng số khẳng định đơn lẻ được kiểm là 34+.)*

### B. Bỏ sót đã tìm thêm

**B1. 🔴 `JoinMission` — hàm engine KHÔNG TỒN TẠI trong JX1 (chặn cửa toàn bộ tính năng)**
`dragonboat_main.lua:163` gọi `JoinMission(MISSION_MATCH, 1)`. Đây **không phải** hàm script:
ELF Linux có nó tại `0x08137E40` (mở đầu `call Lua_GetTopIndex; cmp eax,1; jle` = đòi ≥ 2 tham số),
và chuỗi `"JoinMission"` ở `0x08259E50` được tham chiếu **bên trong chính hàm đó** (`0x0813826A`)
— tức engine quay lại gọi hàm script cùng tên trong `mission_match.lua:93`.
Ở JX1: `grep -rni "joinmission" D:\GAMEDEVNEW\Sources` = **0 hit**. Bảng mission của JX1 chỉ có
`AddMSPlayer(MissionId, groupid)` (`ScriptFuns.cpp:14756`), `OpenMission`/`CloseMission`/`RunMission`.
⇒ Phải **viết mới** `LuaJoinMission` hoặc **sửa script** cho `COT_TeamMemberJoin` gọi thẳng
`PlayerJoinMission(1)`. Báo cáo gốc xếp `JoinMission` vào phần "CHƯA XÁC MINH ngữ nghĩa" nhưng
**không hề nêu nó thiếu ở JX1**, và §11 liệt kê "việc phải làm" cũng bỏ qua.

**B2. 🔴 `ChuangGuan30:OnEnterMap` thiếu `return` — lỗi thật thứ 7 của bản gốc**
`chuangguang30.lua:99-101`:
```lua
if self.bActive == 1 then
    self.KickOut()          -- gọi bằng DẤU CHẤM, và KHÔNG có return
end
-- rơi thẳng xuống:
tinsert(self.tbGroup[nMapId], szName);  self.tbPlayer[szName] = 1
self.nCount = self.nCount + 1;  SetDeathScript(...);  SetTmpCamp(nMapId);  self:SetState()
```
Người vào sau khi boss đã ra bị `NewWorld(11,3207,4978)` nhưng **vẫn được ghi vào nhóm, vẫn tăng
`nCount`, vẫn bị `SetTmpCamp`/`SetState`**. Báo cáo §2.2 mô tả là "đá ra ngay" ⇒ mô tả sai hành vi.
Khi port phải thêm `return`.

**B3. 🔴 Xung đột item id nhiều gấp ba báo cáo — và cả một dải item không tồn tại**
Tra theo **cột** `Genre/DetailType/ParticularType` (tệp `004\magicscript.txt` của Linux bị trôi dòng
nên **không được** tra theo số dòng):

*Thêm 8 xung đột ngoài 6 cái §9.3 đã nêu:*

| Item | Linux | JX1 |
|---|---|---|
| 6/1/122 | Phúc Duyên Lộ (Tiểu) | **Phúc Duyên Lộ (Trung)** |
| 6/1/123 | Phúc Duyên Lộ (Trung) | **Phúc Duyên Lộ (Đại)** |
| 6/1/124 | Phúc Duyên Lộ (Đại) | **Quế Hoa Tửu** |
| 6/1/215 | Càn Khôn Tạo Hóa Đan (**đại**) | Càn Khôn Tạo Hóa Đan (**trung**) |
| 6/1/3810 | Tinh Thiết Khoáng | `稽查令` |
| 6/1/3811 | Tinh Tinh Khoáng | `手中线` |
| 6/1/4134 | Chân Nguyên Đan | Lệnh bài Bắc Đẩu - Viêm Đế cấp 2 |
| 6/1/3203 | Hộ Mạch Đan | `越南自定义道具534` |

3 dòng đầu đặc biệt độc: đó là **3 phần thưởng có xác suất cao nhất của ải ẩn** (0.2 / 0.19587 / 0.18)
— port thẳng thì người chơi nhận nhầm một bậc, và bậc cao nhất biến thành rượu Quế Hoa.

*Và **16 item KHÔNG TỒN TẠI** ở JX1 (bảng `magicscript.txt` của JX1 dừng ở ParticularType ~4865,
không có dải 30xxx):* `6/1/30191` (Chìa khóa vàng — §9.3 đã nêu), `30557`, `30533`, `30538`, `30009`,
`30010`, `30228`, `30229`, `30301`, `30289`, `30529`, `30537`, `30506`, `30507`, `30006`, `30505`.
*Cộng thêm* `6/0/3` (Đại Lực hoàn) và `6/0/6` (Phi Tốc hoàn) cũng **không có**.
⇒ Toàn bộ §6.4 (thưởng boss Mật Phòng) và §6.6 (rương chìa vàng) phải **thiết kế lại phần thưởng**,
không chỉ "ánh xạ lại id".

**B4. `award_to_player_success()` — mã chết thứ hai, và là mã sẽ nổ**
`award.lua:164-167` khai báo `award_to_player_success()` dùng `tbExtItem` và `tbTimerLog:weiMing(...)`.
grep toàn cây script Linux: **`tbExtItem` chỉ xuất hiện đúng 2 lần, cả hai đều nằm bên trong chính
hàm này**; không tệp nào gọi `award_to_player_success`. Tức là hàm này chưa từng chạy — và nếu ai gọi
nó khi port thì `tbExtItem` là `nil` ⇒ lỗi. §10.4 chỉ nêu `chuangguang30_playerdeath.lua`,
bỏ sót ca này. Khi port: **xoá hẳn**, đừng chép.

**B5. Ải 28 và ải ẩn của CẤP 2 sinh boss cấp 75 (lỗi copy-paste của bản gốc)**
`npc.lua:225` — ải 28 cấp 2: `{nil, 1025, func_npc_get_eachname, **75**, -1, 1, 8, func_npc_getpos}`
trong khi 27 ải trước đều là 95.
`npc.lua:230` và `:232` — cả 2 phương án `tbHidenNpc` cấp 2 cũng ghi `75`, và cột ngũ hành là `-1`
(ngẫu nhiên) thay vì `func_npc_getseries` như bản cấp 1 (`npc.lua:125,127`).
⇒ ải cuối và ải ẩn của bản cao cấp yếu hơn hẳn ý đồ thiết kế. Báo cáo §5.1 khẳng định
"75 (cấp 1) hoặc 95 (cấp 2)" nên bỏ lọt.

**B6. Ba hàm engine bị bỏ khỏi bảng kiểm kê §8 (may là JX1 đều có)**
`OpenMission` và `CloseMission` — dùng ở `trigger_include.lua:15` và `:29`, là **trục chính** của
chu kỳ mỗi giờ; `GetSeries` — dùng ở `include.lua:248` (`CallPlayerFunction(players[i], GetSeries)`,
truyền như **giá trị** nên không lộ ra khi grep `Tên(`). JX1 có cả ba:
`ScriptFuns.cpp:14746`, `:14748`, `:14631`. Đã bổ sung vào danh sách §8.

**B7. Chi tiết nhỏ đã đối chiếu và xác nhận KHÔNG phải lỗi (ghi lại để khỏi điều tra lại)**
- `SetMissionS(VARS_TEAMLEADER_FACTION, GetLastFactionNumber())` (`dragonboat_main.lua:191-192`)
  truyền **số** vào hàm đặt **chuỗi**, rồi `npc_death.lua:59-60` đọc bằng `GetMissionS` và đẩy
  vào tham số 5/6 của `Ladder_NewLadder`. Tưởng là lỗi kiểu, nhưng Lua 4 của dự án ép kiểu 2 chiều:
  `lua_isstring` trả true cho number (`Sources\Library\LuaLib\src\lapi.c:130-133`) và `lua_isnumber`
  trả true cho chuỗi số (`:125-128`) ⇒ chuỗi truyền lọt, JX1 chạy đúng. **Không cần vá.**
- `rank_perday.lua:81` `value = value * (-1)` khi bảng xếp hạng chưa đủ 5 mục: `LuaLadder_GetLadderInfo`
  của JX1 trả `("", 0, -1, 0)` cho ô trống chứ **không trả nil** (`KJx2SharedStore.cpp`, comment
  ngay trên hàm ghi rõ đây là hành vi gốc) ⇒ không nổ. **Không cần vá.**
- `npc_death.lua:85` ghi `laddertime` **dương** trong khi `:77` và `:88` ghi **âm** — nhưng nhánh này
  đã chết vì lỗi `bfind` (§10.3), nên chỉ cần nhớ khi vá §10.3: sửa dấu **cùng lúc**, nếu không
  vá xong sẽ sinh ra bảng xếp hạng sắp ngược.

### C. Hai điểm còn "CHƯA XÁC MINH" (phản biện cũng không kết luận được)

1. **Ngữ nghĩa đầy đủ của `JoinMission` engine Linux** — đã xác minh được: nhận ≥ 2 tham số, đọc
   mission id + camp, và có tham chiếu chuỗi `"JoinMission"` để gọi ngược script. **Chưa xác minh**
   nó có tự gom cả tổ đội hay không (nhưng vì `dragonboat_main.lua:202-204` đã tự lặp
   `CallPlayerFunction` cho từng thành viên nên câu hỏi này không ảnh hưởng bản port).
2. **Trigger có thật sự chạy mỗi giờ hay không** — file `settings\trigger_challengeoftime.lua:1`
   chỉ có *comment* `由Relay每小时触发`; phản biện chưa tìm được mục lịch của Relay khai báo chu kỳ
   này, nên "mỗi giờ" hiện vẫn **dựa vào comment + hằng `INTERVAL_MATCH = 3600`** chứ chưa có
   bằng chứng cấu hình. (Ảnh hưởng: `ChuangGuan30:AddTime` hẹn `1440s` chỉ ra đúng "phút 24"
   nếu trigger thực sự nổ ở phút 0.)
