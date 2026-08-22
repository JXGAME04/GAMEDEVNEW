# ĐẶC TẢ THI CÔNG — BÁCH NHÂN LÔI ĐÀI "Lôi Đài Hoàng Thành Tư" (tag `bairenleitai`)

> Phiên **22/08/2026**. **CHỈ PHÂN TÍCH — chưa sửa tệp nào.** Tài liệu này đủ để người thi công làm mà không cần mở lại cây Linux.
> Nguồn Linux: `D:\ServerLinux\server1\script\missions\bairenleitai` (10 tệp / 1.368 dòng) + vỏ khởi động `activitysys\config\9` (3 tệp) + lối vào `missions\zhaojingling\enternpc.lua`.
> Đích: mã nguồn `D:\GAMEDEVNEW\Sources`, cây chạy thật `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server` (gọi tắt `bin\server`).
> Công cụ/kết quả thô: `D:\GAMEDEVNEW\ReverseTools\port_bairenleitai\` (`brlt_closure.py` → `closure_result.txt`, `brlt_gap.py` → `api_gap_tree2.txt` / `api_gap_libs.txt`, `find_off.py`, **`dac_ta_ham_engine_bairenleitai.json`** = đặc tả 7 mục engine theo khuôn `dac_ta_17_ham_hoatdong_phuong.json`).
> Mọi khẳng định đều kèm `tệp:dòng` hoặc địa chỉ ELF đã đọc thật; chỗ chưa chắc ghi **CHƯA RÕ**.

---

## 0. KẾT LUẬN 1 PHÚT

| | |
|---|---|
| **Làm được?** | **LÀM ĐƯỢC CÓ ĐIỀU KIỆN.** Script chép gần nguyên byte (9/10 tệp của cây + 2 tệp lib), map 960 đủ dữ liệu hai cây, 0 item, 0 ảnh, 6 NPC template trùng id+tên. Điều kiện: **4 hàm engine mới** (`SetTmpCamp`/`GetTmpCamp`, `AddTimer`+`DelTimer`(+`Suspend/Resume`), `RemoteExecute`, `Msg2Map` = alias) + **3 chỗ sửa hàm đã có** (`AddMapTrap` phải lưu tham số 5, `CallPlayerFunction` phải trả kết quả, `DynamicExecute*` không được cắt chuỗi 64 byte) + **4 tệp Lua mới** (driver thay `activitysys/config/9` vì engine ta mỗi tệp một `lua_State`). |
| Bản Linux có đang chạy không? | **KHÔNG.** NPC lối vào "Quan nhắc nhở Hoàng Thành Tư" bị `global\autoexec.lua:214 ClearNPCNewVersion()` xoá khỏi 28 map (trong đó có 176 Lâm An) ngay lúc boot (`autoexec.lua:275-287`, chuỗi tên ở `:280`). Script và map thì còn nguyên. Chủ game muốn làm → **dự án đặt lại NPC** (mục F). |
| Đổi theo chính sách | **1 chỗ**: `hundred_arena.lua:411` `GetLevel() >= 120` → `>= 90` (+ câu thoại `:414`). **Không có** điều kiện trùng sinh nào trong cây. |
| Thứ tốn công nhất | `AddTimer` (hệ hẹn giờ script JX2 — `scriptjx2\lib\timerlist.lua` đã có sẵn trong dự án nhưng chưa bao giờ chạy được vì thiếu hàm C) và mở rộng `AddMapTrap` lưu tham số theo ô. Cả hai **dùng lại được** cho Cảnh Kỹ Trường / Cổ Tháp / Thành Bảo / Bang Chiến sau này. |

---

## 1. CƠ CHẾ HOẠT ĐỘNG (đọc từ mã, không suy đoán)

### 1.1 Luồng chính (`hundred_arena.lua`)

```
BOOT  G_ACTIVITY ServerStart (config/9/config.lua:5-17)
        → lib:Include(hundred_arena.lua) → HundredArena.IniEnvironment(HundredArena)   [hundred_arena.lua:131-150]
             · 5 ArenaField:New(i)                                                       [:133-136, :428-439]
             · LoadMap(): LoadTrap() = 5 bảng arena%d.txt → AddMapTrap(960,x,y,trap_arena.lua, i)   [:50-66]
                          AddNpc()   = Xa phu 393 (56000,101088) · Rương chứa đồ 625 (55232,100512) · Chủ dược điếm 389 (55936,99744)  [:68-101]
                          AddObstacle() = obstacle.txt 33 ô → AddObstacleObj(359,960,x,y)          [:104-112]
             · EventSys Reg EnterMap/LeaveMap map 960 → OnEnterMap/OnLeaveMap               [:160-172]
             · TimerList:AddTimer(self, 1 phút, 1) → HundredArena:OnTime mỗi phút            [:146]

NPC "Quan nhắc nhở Hoàng Thành Tư" (Lâm An 176)  → "Lôi Đài Hoàng Thành Tư" → bairen_dialog()  [npc_enter.lua:13-20]
   "Ta muốn vào…" → HundredArena:NpcEnter()   [npc_enter.lua:22-26 → hundred_arena.lua:207-221]
        IsOpen(): giờ ≥ 12:00 (GetLocalDate %H%M ≥ 1200) và GetLevel() ≥ 120   [:406-421]
        lưu map/x/y hiện tại vào task 2323/2324/2325; NewWorld(960, điểm ngẫu nhiên inmap.txt)

VÀO MAP 960 → OnEnterMap [:223-234]: NewPlayer (tbPlayerList[tên]), SetDeathScript(player_death.lua),
        DisabledUseTownP(1), SetLogoutRV(1), DisabledStall(1), SetFightState(0), nhắc "dùng khinh công nhảy lên đài"
        (SetAForbitSkill(210,1) cấm khinh công ĐÃ BỊ COMMENT :228 — giữ nguyên)

NHẢY LÊN Ô TRAP CỦA ĐÀI i → trap_arena.lua main(i) → HundredArena:InTrap(i)   [:465-535]
   nFightState 0 (đài trống)  → người này = LÔI CHỦ: nGrade=1, SetTempRevPos(960, RevivalPos), SetTmpCamp(1), SetCurCamp(4),
                                  hẹn 30 s (tbTimerInfo.nApply) chờ khiêu chiến, nFightState=1
   nFightState 1 (có lôi chủ)  → chính lôi chủ nhảy lại = rời đài (Leave+Destroy) ; người khác = KHIÊU CHIẾN: SetTmpCamp(2), SetCurCamp(4),
                                  huỷ timer 30 s, hẹn 3 s (nPrepare), nFightState=2
   nFightState ≥2 (đang đấu)  → "đợi lượt sau", bị Leave() ra 1 trong 4 góc OutPos
   → ArenaField:Enter(): SetPos(InPos), hồi đầy HP/MP/thể lực

ArenaField:OnTime(nFightState) [:571-616]
   1: hết 30 s không ai khiêu chiến → CallFightNpc(): AddNpcEx 1 trong 10 "Cao thủ <phái> Bách Nhân Lôi Đài" (1786-1795),
      cấp 90 + floor((nGrade-1)/10) (tối đa 99), ngũ hành theo phái, script npc_death.lua, NpcParam[1]=đài; hẹn 3 s → 2
   2: hết 3 s chuẩn bị → cả hai SetFightState(1), SetPKFlag(2), SetTmpCamp(1|2), ST_StartDamageCounter; hẹn 3 phút (nFight) → 3
   3: hết 3 phút → TimeClose() [:705-801]: so ST_GetDamageCounter — lôi chủ chịu ít hơn hoặc bằng → lôi chủ thắng (nGrade+1),
      khiêu chiến ra đài, hẹn 30 s chờ người mới; ngược lại khiêu chiến thành lôi chủ mới (nGrade=1); nếu đối thủ là NPC mà hết giờ → lôi chủ THUA, Destroy
   NPC chết → npc_death.lua OnDeath → ArenaField:NpcDeath() [:803-826]: nGrade+1, lôi chủ giữ đài
   Người chết → player_death.lua → ArenaField:PlayerDeath(1|2|3) [:828-915]
IsMaxGrade() [:669-703]: mỗi 10 trận thắng liên tiếp ở ĐÀI 1 → RemoteExc(msg2allworld) thông báo TOÀN SERVER; 100 trận → "truyền thuyết", đuổi ra, Destroy

HundredArena:OnTime() mỗi phút [:265-352]
   giờ 0 → Close() (Destroy 5 đài, trả mọi người về map đã lưu) ; giờ 0..11 → ngủ
   mỗi 5 phút: người trong map +1.000.000 exp (×2 nếu có buff Cổ Thủ), lôi chủ thêm +2.000.000; trần 50 lượt/ngày qua task daily 2709
              ai quá 90 phút không hoạt động (HA_MAXSTAYTIME) → đuổi về map đã lưu
   mỗi 30 phút: thả NPC "Cổ Thủ" 1571 (tên hiển thị trong bảng NPC: "Người đánh trống") tại 1 trong 15 điểm drummer.txt, sống 15 phút (npc_beauty.lua),
              cho 20 % số người đang ở map nhận buff ×2 exp 30 phút (AddDoubleState :362-385)
```

### 1.2 `G_ACTIVITY` nạp `config/9` như thế nào (trả lời câu hỏi trong đề bài)

Trên Linux **một `lua_State` cho cả cây** nên mọi tệp `.lua` chạy phần top-level lúc boot:

1. `activitysys/config/9/registe.lua:1-7` chạy lúc nạp: Include `head.lua` (`pActivity = ActivityClass:new(); nId=9; szName="Lôi Đài Hoàng Thành Tư"` — `head.lua:2-4`), Include `config.lua` (bảng `tbConfig[1..21]`), `pActivity.tbConfig = tbConfig; pActivity:InitTaskGroup(); G_ACTIVITY:AddActivity(pActivity)` (`g_activity.lua:114-118` đẩy vào `tbSet`).
2. `global/autoexec.lua:204` `G_ACTIVITY:LoadActivitys()` → `ActivityClass:LoadConfig` (`activity.lua:66-86`): mỗi `tbConfig[i]` → `ActivityDetailClass:Create(szMessageType)` lấy **template theo loại** đã đăng ký bởi `activitysys/detailtype/*.lua` (`serverstart.lua:3 NewType("ServerStart")`, `clicknpc.lua:4 NewType("ClickNpc")`) → `LoadConfig` biến `tbCondition/tbActition` thành `FunctionClass` (`lib/pfunction.lua`) → `RegisteMessage` treo vào `G_ACTIVITY.FunSet[loại]`.
3. `autoexec.lua:206` `G_ACTIVITY:OnMessage("ServerStart")` → detail 1 (`config.lua:5-17`): điều kiện `lib:Include(hundred_arena.lua)` (`functionlib.lua:324-327` = `Include` rồi trả 1) → hành động `HundredArena.IniEnvironment(HundredArena)`.
4. NPC bấm → `activitysys/npcdailog.lua:23` (hoặc `zhaojingling/enternpc.lua:28`) `G_ACTIVITY:OnMessage("ClickNpc", tbDailog, nNpcIndex)` → `clicknpc.lua:7-31`: tên NPC khớp `tbMessageParam[1] = "Quan nhắc nhở Hoàng Thành Tư"` (`config.lua:26`) → `AddDialogOpt("Lôi Đài Hoàng Thành Tư", 3)` = thêm mục, khi chọn → `pActivity:GotoDetail(3)` → detail 3 (`config.lua:38-50`, `szMessageType="nil"` = không đăng ký sự kiện, chỉ gọi tay): `lib:Include(npc_enter.lua)` rồi `bairen_dialog()`.
5. Detail 4..21 rỗng (`szName="nil"`), không làm gì.

**Dự án**: `activitysys\` chỉ có 6 tệp (`activity.lua`, `activitydetail.lua`, `functionlib.lua`, `g_activity.lua`, `ladderfunlib.lua`, `playerfunlib.lua` — **trùng byte Linux**), **KHÔNG có `config\`, `detailtype\`, `npcdailog.lua`, `answer.lua`, `npcfunlib.lua`**; `G_ACTIVITY.FunSet` rỗng ⇒ `G_ACTIVITY:OnMessage` là no-op (đang dùng làm "stub" cho Tín Sứ `messenger_turenpc.lua:29`, `global\map_helper.lua:130`). Vì **mỗi tệp một `lua_State`** (`ScriptFuns.cpp:2178-2190` ghi chú `curpack/usepack` stub), cơ chế đăng ký toàn cục kiểu Linux **không thể** dùng ⇒ thay bằng **driver gọi thẳng** (mục E) đúng như đã làm cho Liên Đấu (`gsdriver.lua`) và 3 Hoạt động Phường (`tong_driver.lua`). **Không chép `activitysys/config/9`** (3 tệp chỉ có giá trị tài liệu).

### 1.3 Hằng số (head.lua)

| Hằng | Giá trị | Nguồn |
|---|---|---|
| Map | 960 `特殊用地\leitai` | `hundred_arena.lua:23`; `maplist.ini` dự án `:6248-6251` (`960_name=Lôi Đài Hoàng Thành Tư`, `960_NewWorldParam=PUNISH_OFF\|USETOWNP_OFF\|HEART_OFF\|CD_Forbid_OFF`) |
| 5 đài: InPos / OutPos×4 / NpcPos / RevivalPos(1749,3135) | `head.lua:10-79` |
| Chờ khiêu chiến / chuẩn bị / đánh / nhịp điều phối | 30 s / 3 s / 3 phút / 1 phút (×18 khung) | `head.lua:82-88` |
| 5 bảng toạ độ + bẫy + vật cản + điểm vào | `\settings\maps\missions\bairenleitai\{arena%d,drummer,obstacle,inmap}.txt` | `head.lua:90-97` |
| 10 Cao thủ 1786-1795 (cấp 90, ngũ hành 0..4 theo phái), Cổ Thủ 1571 sống 15 phút | `head.lua:108-186` |
| Task lưu vị trí ra: 2323 / 2324 / 2325 | `head.lua:188-190` |
| Ở lì tối đa 90 phút; buff ×2 exp 30 phút | `head.lua:192-193` |
| Task đếm ngày trần exp: 2709 (50 lượt) | `hundred_arena.lua:32, :298-315` |
| exp mỗi 5 phút: 1e6 (×2 buff) + lôi chủ 2e6 | `hundred_arena.lua:293, :310` |
| Mở cửa: 12:00 → hết ngày (giờ 0 đóng) | `:276-279, :409` |

---

## A. DANH SÁCH TỆP CHÉP (bao đóng Include — `port_bairenleitai\closure_result.txt`)

Bao đóng đệ quy từ 10 tệp cây + `event/msg2allworld.lua` + `missions/basemission/lib.lua`: **31 tệp**, trong đó **15 dự án ĐÃ CÓ và trùng byte**, **12 phải chép**, 3 `config/9` không chép, 1 bị thay (`npcdailog.lua`).

### A.1 Chép từ Linux vào `bin\server\script\` (12 tệp)

| # | Tệp (tương đối `script\`) | Dòng | Cách chép | Ghi chú |
|---|---|---|---|---|
| 1 | `missions\bairenleitai\head.lua` | 193 | nguyên byte | |
| 2 | `missions\bairenleitai\hundred_arena.lua` | 928 | nguyên byte **+ sửa 2 dòng (H) + nối 14 dòng cầu nối cuối tệp (A.4)** | |
| 3 | `missions\bairenleitai\npc_beauty.lua` | 35 | nguyên byte | Cổ Thủ: `G_ACTIVITY` có nhờ `playerfunlib.lua:5` Include `g_activity.lua` |
| 4 | `missions\bairenleitai\npc_chuwuxiang.lua` | 2 | nguyên byte | `OpenBox()` có (`ScriptFuns.cpp:14370`) |
| 5 | `missions\bairenleitai\npc_death.lua` | 30 | **DIFF** (A.4) | truy cập `HundredArena.tbArenaList` xuyên state |
| 6 | `missions\bairenleitai\npc_enter.lua` | 38 | **DIFF 1 dòng**: `:9 Include("\\script\\activitysys\\npcdailog.lua")` → `Include("\\script\\dailogsys\\dailogsay.lua")` | `npcdailog.lua` Linux kéo `bonusvlmc\*` (6 tệp), `vng_event\traogiai\npah\vng_toolaward.lua` → `event\tongwar\head.lua` → `task\random\treasure_head.lua`… (18 tệp, xem `closure_result.txt` bản đầu) — **không port**. `npc_enter.lua` chỉ cần `CreateNewSayEx` (`dailogsay.lua:22`), `oncancel` (nil → `g_DailogBack` nhánh 1 tham số `dailogsay.lua:49-50` = đóng), `tbLog` (`lib\log.lua` đã Include `:11`). |
| 7 | `missions\bairenleitai\npc_store.lua` | 17 | nguyên byte | `Sale(12)` (D.4) |
| 8 | `missions\bairenleitai\npc_trans.lua` | 56 | nguyên byte | Xa phu 7 thành, `NewWorld` + `SetLogoutRV(0)` |
| 9 | `missions\bairenleitai\player_death.lua` | 54 | **DIFF** (A.4) | tên hàm chết + xuyên state |
| 10 | `missions\bairenleitai\trap_arena.lua` | 15 | nguyên byte | `main(index)` — cần `AddMapTrap` tham số 5 (C.5) |
| 11 | `missions\basemission\lib.lua` | 76 | nguyên byte | `basemission_CallNpc` (AddNpcEx + SetNpcScript/Timer/Param); Include `lib\common.lua` (có) + `lib\coordinate.lua` (→ `scriptjx2\lib`, trùng byte) + `IncludeLib("FILESYS")` (→ `scriptjx2\lib\file.lua`) |
| 12 | `event\msg2allworld.lua` | 13 | nguyên byte | `battle_msg2allworld` → `GlobalExecute("dw Msg2SubWorld([[%s]])")` — `dw` = hoãn 1 tick chạy trong state `\script\gmscript.lua` (`KJx2League.cpp:1054-1090`); `Msg2SubWorld` dự án = phát **toàn server** (`ScriptFuns.cpp:3572-3580` `SendSystemInfo(0,0,…)`) ✓ đúng ý "all world" |

### A.2 Dự án ĐÃ CÓ, trùng byte (không chép) — 15 tệp

`activitysys\activity.lua`, `activitysys\g_activity.lua`, `activitysys\playerfunlib.lua`, `dailogsys\dailogsay.lua`, `dailogsys\g_dialog.lua`, `lib\common.lua`, `lib\log.lua`, `lib\objbuffer_head.lua`, `misc\eventsys\type\map.lua`, `misc\taskmanager.lua` (trong `script\`);
`lib\coordinate.lua`, `lib\file.lua`, `lib\player.lua`, `lib\remoteexc.lua`, `lib\timerlist.lua` (**trong `scriptjx2\lib\`** — `Include("\\script\\lib\\X.lua")` thiếu ở `script\lib` thì engine tự ánh xạ sang `scriptjx2\lib` — `ScriptFuns.cpp:1933-1960 sJX2RemapScriptPath`, bảng `{"script\\tong\\","script\\lib\\"}`). Tầng sâu hơn (`lib\pay.lua`, `task\task_addplayerexp.lua`, `lib\string.lua`→scriptjx2, `item\class\virtualitem.lua`, `global\itemset.lua`, `activitysys\functionlib.lua`, `task\system\task_string.lua`, `script_protocol\protocol_def_gs.lua`) đều đã có và đang phục vụ Tín Sứ. `lib\awardtemplet.lua` DIFF (bản dự án mở rộng) — không ảnh hưởng (cây này không phát thưởng qua awardtemplet).

⚠ `IncludeLib("LEAGUE")`/`("SETTING")` trong `lib\player.lua:4-5` → `scriptjx2\lib\noop.lua` (`ScriptFuns.cpp` bảng `LuaIncludeLib`) — OK.

### A.3 Tệp MỚI (4) — driver thay `config/9` + `newworldscript` (nội dung đầy đủ ở Phụ lục P1)

| Tệp | Thay cho | Vai trò |
|---|---|---|
| `script\missions\bairenleitai\bairen_boot.lua` | `config/9/config.lua:5-17` (ServerStart) + `zhaojingling\enternpc.lua:7-15,44-53` (đặt NPC) | `BairenLeitai_Init()`: `DynamicExecute(hundred_arena.lua, "HundredArena:IniEnvironment")` + đặt NPC 1747 ở Lâm An từ `chrismas\enternpc.txt` |
| `script\missions\bairenleitai\npc_quan_jx1.lua` | `config/9/config.lua:22-50` (ClickNpc → AddDialogOpt → detail 3) + thoại `enternpc.lua:25` | `main()` của NPC "Quan nhắc nhở Hoàng Thành Tư": tiêu đề + 1 mục "Lôi Đài Hoàng Thành Tư" → `bairen_dialog()` (Include `npc_enter.lua`) |
| `script\missions\bairenleitai\newworld.lua` | `EventSys EnterMap/LeaveMap` (`hundred_arena.lua:160-172` đăng ký trong state của nó — không bao giờ được `newworldscript` state gọi tới) | `OnNewWorld/OnLeaveWorld` → `DynamicExecuteByPlayer(PlayerIndex, hundred_arena.lua, "HundredArena:OnEnterMap"/"OnLeaveMap")` — khuôn `missions\leaguematch\combat\newworld.lua:1-16` |
| (sửa) `settings\maplist.ini:6250` | `960_NewWorldScript=\script\maps\newworldscript.lua` (tệp **không tồn tại** trong dự án — `KSubWorld_FireMapScript` bỏ qua im lặng, `ScriptFuns.cpp:10923-10945`) | → `960_NewWorldScript=\script\missions\bairenleitai\newworld.lua` |

### A.4 Ba tệp DIFF — nội dung sửa (Phụ lục P2 có bản đầy đủ)

**Vì sao phải sửa**: Linux 1 state nên `npc_death.lua:10,26-27` và `player_death.lua:13,19-35` `Include("hundred_arena.lua")` rồi đọc **trực tiếp** `HundredArena.tbPlayerList/tbArenaList`. Ở dự án, `Include` = `lua_dofile` vào state **của tệp đang gọi** (`ScriptFuns.cpp:2015`) ⇒ mỗi tệp có **bản sao rỗng** của `HundredArena` ⇒ phải gọi sang state của `hundred_arena.lua` bằng `DynamicExecuteByPlayer` (chính cách `trap_arena.lua:14`, `npc_enter.lua:24`, `npc_beauty.lua:30` của Linux đã dùng).

1. **`hundred_arena.lua`** — nối **cuối tệp** (sau dòng 928), không đụng dòng nào khác ngoài (H):
```lua
-- ===== [JX1 22/08] cầu nối 1 state/tệp: npc_death.lua & player_death.lua gọi sang state này qua DynamicExecuteByPlayer =====
function HundredArena:JX1_NpcDeath(nNpcIndex)                 -- = npc_death.lua:25-28 gốc
	local nArenaID = ArenaNpc:GetNpcArena(nNpcIndex);
	if (nArenaID and self.tbArenaList[nArenaID]) then self.tbArenaList[nArenaID]:NpcDeath(); end
end
function HundredArena:JX1_PlayerDeath(nType)                  -- = player_death.lua:19-36 gốc; nType 3 = bị NPC giết, 0 = bị người giết (tự suy theo nRight)
	local tb = self.tbPlayerList[GetName()];
	if (not tb or not tb.nArenaId or tb.nArenaId == 0 or not self.tbArenaList[tb.nArenaId]) then return end
	if (nType == 0) then if (tb.nRight == 1) then nType = 2 else nType = 1 end end
	self.tbArenaList[tb.nArenaId]:PlayerDeath(nType);
end
```
2. **`npc_death.lua:25-28`** → `DynamicExecuteByPlayer(PlayerIndex, "\\script\\missions\\bairenleitai\\hundred_arena.lua", "HundredArena:JX1_NpcDeath", nNpcIndex)`. (`PlayerIndex` = người giết, engine đặt ở `KNpc.cpp:11178` trước khi gọi `OnDeath(m_Index, m_nLastDamageIdx)` `KNpc.cpp:1552`.)
3. **`player_death.lua`**: (a) engine dự án gọi **`OnPlayerDeath(nVictimPIdx, nLauncherNpcIdx)`** (`KNpc.cpp:1542`) chứ không phải `OnDeath(Launcher)` của Linux (`KNpc::OnDeath` Linux `0x08083805` gọi "OnDeath" `0x8256d94` với `m_nLastDamageIdx`) — quy ước Đợt E đã dùng cho `missions\citywar_city\playerdeath.lua:2-4` ⇒ thêm `function OnPlayerDeath(nVictimPIdx, Launcher) OnDeath(Launcher) end`; (b) dòng 19-35 → 2 lệnh `DynamicExecuteByPlayer(nKilledIndex, hundred_arena.lua, "HundredArena:JX1_PlayerDeath", 3 hoặc 0)`; (c) **dòng 49 `self:DelPlayer(PlayerIndex)`**: `self` là biến toàn cục **chưa bao giờ được gán** trong tệp (hàm `OnDeath` không phải method) ⇒ trên Linux dòng này **ném lỗi runtime** và dòng 50-53 (`NewWorld` về map đã lưu) **không bao giờ chạy**; hành vi thực tế trên Linux = người chết hồi sinh tại điểm hồi sinh đăng nhập (`SetRevPos(GetPlayerRev())` dòng 47 — `KPlayer::SetRevivalPos` đồng thời đặt lại `m_sDeathRevivalPos` `KPlayer.cpp:1405`), rời map 960 → `OnLeaveMap` dọn. **Bản dự án giữ đúng hành vi thực tế**: bỏ dòng 49-53 (ghi chú tại chỗ). Nếu gọi `HundredArena:DelPlayer` thay `self` thì `OnLeaveMap:240` sẽ lỗi `index nil` — **không làm**.

---

## B. SETTINGS / MAP

| Gì | Nguồn Linux | Dự án | Việc |
|---|---|---|---|
| `settings\maps\missions\bairenleitai\` **11 tệp** (`arena1..5.txt` 127-135 dòng, `chefu.txt`, `chuwuxiang.txt`, `drugstore.txt`, `drummer.txt` 15 điểm, `inmap.txt` 10 điểm, `obstacle.txt` 33 ô; cột `TRAPX\tTRAPY`, đơn vị **pixel**) | `D:\ServerLinux\server1\settings\maps\missions\bairenleitai\` | **chưa có** (thư mục `settings\maps\missions\` không tồn tại) | chép nguyên thư mục. Script chỉ dùng 9 tệp (3 tệp `chefu/chuwuxiang/drugstore.txt` không tệp nào đọc — toạ độ NPC viết cứng ở `hundred_arena.lua:74-75,85-86,96-97` trùng giá trị) — chép cả 11 cho giống |
| `settings\maps\chrismas\enternpc.txt` (`POSX 46848 POSY 103136` = ô 1464,3223 map 176) | Linux | **đã có, trùng byte** (đợt 3 Hoạt động Phường chép cả thư mục) | không |
| Map 960 dữ liệu server | `\maps\特殊用地\leitai` | **222 tệp `_Region_S` + 32 `_Region_C`** trong `bin\server\Pak` (đo lại `regionscan.py` 22/08); `Maps\WorldSet_GameServer.ini:891 World886=960` | không |
| Map 960 client | | có (`PHANTICH…` mục 4.1; client `npcs.txt` có 1571/1747/1786… đúng dòng) | không |
| `settings\maplist.ini:6250` | | `960_NewWorldScript=\script\maps\newworldscript.lua` (tệp không có) | đổi sang `\script\missions\bairenleitai\newworld.lua` (A.3). `960_NewWorldParam` giữ nguyên — bản compat `maps\newworldscript_default.lua:12-23` của dự án bỏ qua tham số lạ (`PraseParam` chỉ nhận `CreateTeam_OFF/PARTNER_OFF/TISHENZHIREN`) |
| `settings\task\missions.txt` / `settings\TimerTask.txt` | | không cần — tính năng **không dùng mission, không dùng TimerTask** (hẹn giờ bằng `AddTimer` nội bộ) | không |
| `settings\maptraffic.ini [176] 38_Content=Quan nhắc nhở Hoàng Thành Tư` (điểm 1195,968) | Linux `:4081-4083` | dự án `:3888` đã có | không |

---

## C. HÀM ENGINE THIẾU — đặc tả từ disasm (chi tiết đầy đủ: `port_bairenleitai\dac_ta_ham_engine_bairenleitai.json`, 7 mục)

`brlt_gap.py` trên 14 tệp (cây + config/9 + msg2allworld): **50 hàm engine đã có**, thiếu **2** (`Msg2Map` ×19, `SetTmpCamp` ×2); trên 8 tệp lib/basemission: thiếu `AddTimer/DelTimer/SuspendTimer/ResumeTimer` (`timerlist.lua:11,26,34,41`), `RemoteExecute` (`remoteexc.lua:23,25`); `PIdx2NpcIdx`, `GetNpcRelation`, `File_Create`, `IniFile_SetData/Save` chỉ nằm trong hàm **không được gọi** (`player.lua:512-516`, `file.lua:6-25`) — bỏ qua.

| # | Hàm | Địa chỉ Linux | Việc ở dự án | Độ khó |
|---|---|---|---|---|
| C.1 | `Msg2Map(nMapId, szMsg)` | `0x08105080` (SearchWorld `0x080f68a0` → duyệt người chơi trong map → `SendSystemInfo(1, idx, …)` `0x081c9220`) | **Alias**: thêm `{"Msg2Map", LuaMsgToAroundRegion}` cạnh `ScriptFuns.cpp:14077` — `LuaMsgToAroundRegion` (`:3582-3604`) nhận đúng `(nMapId, szMsg)` và làm y hệt | 1 dòng |
| C.2 | `SetTmpCamp(nCamp[, nNpcIdx])` / `GetTmpCamp([nNpcIdx])` | `0x0810BA50` / `0x0810BB70`; `KNpc::SetTmpCamp 0x0807B2D0` ghi `KNpc+0x1900`, đồng bộ client gói 9 byte proto `0xD1` (`0x0807B260`); **tác dụng** trong `KNpcSet::GetRelation` Linux `0x0809EE50` đoạn `0x0809EF72-0x0809EFB3`: cả hai `m_nTmpCamp ≠ 0` và (không phải người **hoặc** `m_FightMode ≠ 0`) ⇒ khác = **enemy(8)**, cùng = **ally(4)**, xét **trước** bảng camp và trước hàm PK người-người; chỉ reset khi `KNpc::Init` (`0x0807DCB1`) | Thêm `int m_nTmpCamp` vào `KNpc.h:279` (`#ifdef _SERVER`), reset ở `KNpc.cpp:228`, chèn luật vào `KNpcSet.cpp` ngay sau khối SimCity `:1687-1690` trước `:1692`; 2 hàm Lua cạnh `LuaSetPlayerCurrentCamp` `ScriptFuns.cpp:7847`, đăng ký cạnh `:14299`. **Không** đồng bộ client (JX1 không có gói `0xD1`; client tự tính quan hệ bằng `camp_free(4)`+`FightMode`+`PKFlag` mà script đã đặt `hundred_arena.lua:485,591-593` — `KNpcSet::GenOneRelation` `KNpcSet.cpp:146-152`) | ~40 dòng |
| C.3 | `AddTimer(nFrames, "Ham", nParam) → id`, `DelTimer(id)`, `SuspendTimer(id)`, `ResumeTimer(id)` | `0x08100D40 / 0x08100CA0 / 0x08100C00 / 0x08100B60`; timer obj: `+4` tên hàm (259), `+0x108` ActionScriptID tệp gọi, `+0x10c` param, `+0x110` id; `KScriptTimer::OnTimer 0x081CC300`: gọi `Ham(nParam, nTimerId)` trong state tệp đó; **0 kết quả → huỷ; 1 kết quả t → t==0 huỷ / t≠0 hẹn lại t; ≥2 kết quả (t, p) → hẹn lại t với param p** (đúng `timerlist.lua:61-66`) | Viết `s_ScriptTimers` + `KJx2ScriptTimer_Breathe()` theo khuôn `s_GlbTimers/KJx2GlbMission_Breathe` (`KJx2League.cpp:924-1000, 1193-1227`), gọi ở `CoreServerShell.cpp:1172`; tìm tệp gọi bằng quét `g_ScriptSet[i].m_LuaState == L` (`KSortScript.h:18`); gọi bằng `lua_dostring(L, "return TimerList:OnTime(p,id)")` (Lua 4.0.1 `lua_dobuffer` để kết quả trên stack `ldo.c:310-320`) | ~150 dòng |
| C.4 | `RemoteExecute(szScript, szFun, hObj[, szCb, nCbId, dwGS])` | `0x08100740`: đóng gói proto `0x3D` ≤ 0x8000 byte gửi relay `g_Server+0x20030` | **1 GS → thực thi tại chỗ**: `hRes=OB_Create`; gọi `szFun(hObj, hRes, 0)` trong state `szScript` (có remap `\script\lib` → `scriptjx2\lib`); nếu có `szCb` gọi `szCb(nCbId, hRes)` trong state đang gọi; `OB_Release`. Đăng ký cạnh `GlobalExecute` `ScriptFuns.cpp:14588`. ObjBuffer dự án đủ 14 hàm `OB_*` | ~60 dòng; **CHƯA RÕ** tên hàm C tạo/huỷ handle (phải tách từ `LuaOB_Create/Release`) |
| C.5 | **`AddMapTrap` tham số 5 `nParam`** (đã có, lệch) | `KNpc::CheckTrap` Linux `0x0807DA78`: `FindTrap 0x080E0990` trả `{scriptId, nParam}` → người chơi: `ExecuteScript(scriptId, "main"@0x826dbc2, "d", nParam)` ⇒ **`main(nParam)`** | Dự án `KNpc.cpp:10186` gọi `main(m_nPlayerIdx)`; `KRegion.h:54` chỉ có `m_dwTrap[16][32]`. Thêm `m_nTrapParam[16][32]` (mặc định `TRAP_PARAM_NONE`), `KRegion/KSubWorld::SetTrapParam`, `LuaAddMapTrap` ghi param (thiếu → 0 như Linux), `CheckTrap` truyền param nếu ≠ NONE, ngược lại giữ `m_nPlayerIdx` cho trap JX1 cũ | ~40 dòng. **Không làm = không lên được đài** (`InTrap(nPlayerIdx)` → `tbArenaList[nPlayerIdx]` nil) |
| C.6 | **`CallPlayerFunction` không trả kết quả** (đã có, lỗi) | Linux trả mọi kết quả của hàm | `ScriptFuns.cpp:2196-2232` `lua_rawcall(L,nArgs,0); return 0;` ⇒ `hundred_arena.lua:177 szPlayerName = CallPlayerFunction(idx, GetName)` = **nil** → `tbPlayerList[nil]` lỗi ngay khi vào map; `:708-709 ST_GetDamageCounter` so sánh nil. Sửa `LUA_MULTRET` + trả `nRes` | 5 dòng |
| C.7 | **`DynamicExecute/ByPlayer` cắt chuỗi 64 byte** (đã có, lệch) | Linux tham số chuỗi dài tuỳ ý | `ScriptFuns.cpp:2360, :2408` `"\"%.64s\""`, `szCall[512]` ⇒ thông báo toàn server của `RemoteExc` (~150 byte, có `<color=yellow>`) bị cắt. Đổi sang `[[…]]` (Lua 4.0.1 chỉ hỗ trợ `[[ ]]` — `llex.c:186-210`), `szCall[4096]`, thay `]]` trong chuỗi bằng `] ]` | 10 dòng |

Tổng: **C++ sửa 7 tệp** (`ScriptFuns.cpp`, `KJx2League.cpp`, `KNpc.h/.cpp`, `KNpcSet.cpp`, `KRegion.h/.cpp`, `KSubWorld.h/.cpp`, `CoreServerShell.cpp`), **chỉ CoreServer** (mọi thứ trong `#ifdef _SERVER`), **client không đổi, không đổi wire-format**.

---

## D. BẢNG REMAP

### D.1 Item — `item_remap.py missions/bairenleitai`: **0 item** (cây không dùng vật phẩm nào). Không làm thêm item, không vẽ ảnh.

### D.2 NPC template (`settings\npcs.txt`, id = dòng−2, đối chiếu theo TÊN)

| Linux id | Tên (cột Name) | Dự án | Ghi chú |
|---|---|---|---|
| 1786-1795 | Cao thủ Thiên Vương / Thiếu Lâm / Ngũ Độc / Đường Môn / Nga Mi / Thúy Yên / Thiên Nhẫn / Cái Bang / Võ Đang / Côn Lôn Bách Nhân Lôi Đài | **TRÙNG id + tên** (Kind 0, Camp 5) | chỉ lệch 5 cột `*ResistMax` (Linux 95 / dự án 25 — lệch chung toàn bảng, không riêng NPC này) và bảng dự án 87 cột vs Linux 103 |
| 1571 | Người đánh trống (script đặt tên "Cổ Thủ") | TRÙNG | |
| 393 | Xa phu ất 4 | TRÙNG | tên có dấu cách cuối ở Linux |
| 625 | Rương chứa đồ | TRÙNG | |
| 389 | Chủ dược điếm 1 | TRÙNG | |
| 1747 | Quan nhắc nhở Hoàng Thành Tư (Kind 3 dialoger) | TRÙNG | client `settings\npcs.txt` cũng có đúng dòng |

### D.3 Skill — **0** (không `AddSkillState/CastSkill` nào; `SetAForbitSkill(210,1)` đã comment). Goldequip — **0**.

### D.4 Shop `Sale(12)` (`npc_store.lua:15`) — `LuaSale` mở dòng `id+1` của `buysell.txt` (`ScriptFuns.cpp:2610-2623` `nShopId-1`): Linux dòng 13 = thuốc 1,2,3,4,6,7,8,9,11,12,16,22,23,24,638,639,640; dự án dòng 13 "Hieu thuoc Thanh Thi" = 1,2,3,4,6,7,8,9,11,12,16,22,23,24,5,10,15 ⇒ **cùng tiệm thuốc thành thị**, giữ `Sale(12)`.

### D.5 Vật cản `AddObstacleObj(359,…)` — dự án `LuaAddObstacleObj` (`KJx2WarInfra.cpp:113-154`) chỉ đặt **ô cản phía server** theo map ID, bỏ qua template 359, trần 2048 ô (`JX2OBST_MAX`) — 33 ô đủ. Lệch: không có vật thể nhìn thấy ở client (**CHƯA RÕ** Linux có vẽ obj 359 hay không).

### D.6 Task id 2323/2324/2325/2709 — `grep` cây dự án: không script nào dùng (chỉ trùng số trong `codenew.lua` bảng mã). `MAX_TASK 4200` (`KPlayerTask.h:18`) đủ.

---

## E. LỊCH & KÍCH HOẠT

- **Không có relay task** (grep `relaysetting\` = 0), **không TimerTask, không mission**. Lịch nằm trong script: `IsOpen()` `hundred_arena.lua:406-421` cho vào từ **12:00** (`GetLocalDate("%H%M") ≥ 1200`, giờ **local của máy chạy GameServer**), `OnTime()` `:265-279` **giờ 0 → Close()**, giờ 0-11 ngủ. Nhịp 1 phút bằng `TimerList:AddTimer(self, 60×18, 1)` `:146` → cần **C.3**.
- **Kích hoạt lúc boot** (thay `G_ACTIVITY:OnMessage("ServerStart")` `autoexec.lua:206`): `script\startgame.lua` — thêm `Include("\\script\\missions\\bairenleitai\\bairen_boot.lua")` cạnh dòng 18 và `DynamicExecute("\\script\\missions\\bairenleitai\\bairen_boot.lua", "BairenLeitai_Init")` cạnh dòng 101 (sau `TONG_DriverInit`, trước `addnpccongthanh()`), **sau khi map đã nạp** (`SubWorldID2Idx(960) ≥ 0` là điều kiện của `LoadMap` `:116-118`).
- Vào/ra map 960 → `newworld.lua` (A.3) → `OnEnterMap/OnLeaveMap`. Engine dự án bắn `OnNewWorld` ở `KNpc::ChangeWorld` và `OnLeaveWorld` ở map cũ + lúc thoát (`ScriptFuns.cpp:10923-10945`, ghi chú WLLS 20/08).
- Lệnh bài Admin (giống `TONG_Adm_MoNgay`): thêm `BairenLeitai_Adm_MoNgay()` trong `bairen_boot.lua` = `DynamicExecute(hundred_arena.lua, "HundredArena:IniEnvironment")` lần nữa (hàm tự `UnReg`+`bTrapFlag` chống đặt trap 2 lần `:138-141`) — **lưu ý** gọi lại sẽ tạo timer 1 phút thứ hai (`:146` không huỷ timer cũ) ⇒ chỉ dùng khi boot lỗi; test ngoài giờ đúng cách là đổi tạm `ntime >= 1200` bằng GM `dostring` trong state (`DynamicExecute(HA, "dostring", "[[HundredArena.IsOpen = function() return 1 end]]")`) — ghi ở I.3.

---

## F. LỐI VÀO

| | Linux | Dự án |
|---|---|---|
| NPC | "Quan nhắc nhở Hoàng Thành Tư" tpl **1747**, cấp 95, **map 176 Lâm An**, toạ độ `settings\maps\chrismas\enternpc.txt` (46848,103136 pixel = ô 1464,3223); tạo bởi `missions\zhaojingling\enternpc.lua:7-15` `basemission_CallNpc` qua `AutoFunctions:Add(initialize)` `:55-60`; script NPC = chính `enternpc.lua` (`main()` `:17-30`: tiêu đề + mục "Hoàng Thành Tư huấn luyện cung" (Truy tìm Mộc Nhân — **không thuộc phạm vi**) + `G_ACTIVITY ClickNpc` thêm "Lôi Đài Hoàng Thành Tư"). **Bị xoá lúc boot** bởi `autoexec.lua:214` (không phải NPC map-data). | `bairen_boot.lua` đặt NPC cùng tpl/cấp/map/toạ độ, script `npc_quan_jx1.lua` (A.3): tiêu đề nguyên văn `enternpc.lua:25`, **1 mục** "Lôi Đài Hoàng Thành Tư" → `bairen_dialog()` (`npc_enter.lua:13-20`: "Ta muốn vào…" / "Về Lôi Đài Hoàng Thành Tư" (hướng dẫn `:28-38`) / "Kết thúc đối thoại"). Mục Mộc Nhân **không** đưa vào (chưa port `zhaojingling`). |
| Trong map 960 | Xa phu (393) đưa về 7 thành (`npc_trans.lua:34-56`), Rương chứa đồ (625) `OpenBox()`, Chủ dược điếm (389) `Sale(12)`, Cổ Thủ (1571) mỗi 30 phút | script tự `AddNpcEx` — giữ nguyên |
| Ra | Xa phu / chết / hết giờ / ở lì 90 phút / `Close()` giờ 0 → `NewWorld(task 2323/2324/2325)` | giữ nguyên |

---

## G. XUNG ĐỘT VỚI DỰ ÁN + CÁCH GIẢI QUYẾT

| # | Xung đột | Giải quyết |
|---|---|---|
| 1 | `960_NewWorldScript` đang trỏ tệp không tồn tại (như **495** map khác trong `maplist.ini`) | chỉ đổi dòng của 960 (A.3). **Không** tạo `script\maps\newworldscript.lua` chung (sẽ bật `OnNewWorld` cho 495 map với `PraseParam` compat → thay đổi hành vi toàn server). |
| 2 | `AddMapTrap` tham số 5: 3 Hoạt động Phường gọi 4 tham số (và hàm `addtrap()` của chúng không ai gọi — `dac_ta_17…json` AddMapTrap.rui_ro) | C.5 giữ `main(nPlayerIdx)` cho `AddTrap` JX1 / trap map-data; chỉ trap đặt qua `AddMapTrap` mới nhận `main(nParam)` (4 tham số → 0 như Linux). |
| 3 | `CallPlayerFunction` đổi từ 0 → N kết quả (C.6) | grep các caller hiện có (`messenger`, `tong`, `leaguematch`) — thêm giá trị trả về là vô hại; ghi nhận để phản biện. |
| 4 | `KNpcSet::GetRelation` thêm luật TmpCamp trước mọi luật khác (C.2) | chỉ tác dụng khi **cả hai** `m_nTmpCamp ≠ 0` — mọi NPC/người khác = 0 ⇒ không đổi hành vi hiện tại. Bot SimCity đã bị chặn ở dòng 1687 trước đó. |
| 5 | `m_nTmpCamp` không tự xoá khi rời map (Linux cũng vậy) | script tự `SetTmpCamp(0)` ở `ArenaField:Leave` `:548`; **người thoát game giữa trận**: `OnLeaveMap` → `PlayerDeath(1|2)` → `Leave(player)` → `SetTmpCamp(0)` ✓; KNpc::Init khi đăng nhập lại reset ✓. |
| 6 | Map 960 / NPC 1747 / task 2323-2325,2709 / timer / mission | grep cây dự án: **không ai dùng** (mục D.6; `lib_trap.lua:426` "960" là toạ độ). |
| 7 | `DisabledUseTownP` dự án là **stub no-op** (`KJx2WarInfra.cpp:241-244`, ghi chú "LO GOC… chưa cưỡng chế") | cấm phù hồi thành trong map 960 **không có hiệu lực** (lệch có sẵn từ Đợt E, không thuộc phạm vi này — ghi nhận). `DisabledStall` thật (`LuaWllsDisabledStall`). |
| 8 | `SetPKFlag` dự án = alias `SetFightState` (`KJx2WarInfra.cpp:227-230`) | `SetPKFlag(2)/(1)/(0)` của script ⇒ bật/tắt trạng thái chiến đấu — đủ cho bảng quan hệ `camp_free+fight_active` (C.2). |
| 9 | Lịch dự án: 16h/22h Lôi Đài Hỗn Chiến map 209, Tống Kim, Liên Đấu… | Bách Nhân chạy 12:00-24:00 mỗi ngày trên map riêng 960, không đụng mission/timer nào; chỉ cạnh tranh người chơi (vận hành). |
| 10 | Ghi chú phát hiện ngoài phạm vi: `missions\leaguematch\combat\playerdeath.lua:4` vẫn là `OnDeath(Launcher)` (engine gọi `OnPlayerDeath`) ⇒ **có thể** Liên Đấu chưa xử lý chết đúng. **CHƯA RÕ**, không sửa trong đợt này. | báo chủ game. |

---

## H. ĐỔI THEO CHÍNH SÁCH "cấp ≥ 90, bỏ trùng sinh"

| Tệp : dòng | Chuỗi cũ | Chuỗi mới |
|---|---|---|
| `script\missions\bairenleitai\hundred_arena.lua:411` | `if (GetLevel() >= 120) then` | `if (GetLevel() >= 90) then` |
| `hundred_arena.lua:414` | `Talk(1, "", "<color=yellow>Cấp 120 và 120 trở lên<color>mới có thể tham gia.")` | `Talk(1, "", "<color=yellow>Cấp 90 và 90 trở lên<color>mới có thể tham gia.")` (byte TCVN3, chỉ đổi 2 số) |

Không có điều kiện trùng sinh / thời gian vào bang / bang hội nào trong 10 tệp (grep `TransLife`, `GetJoinTongTime`, `GetTong` = 0). Cao thủ NPC cấp `90+floor((nGrade-1)/10)` `:620-625` là **luật chơi**, không phải cổng — giữ.

---

## I. RỦI RO + CÁCH TEST SAU RESTART

### I.1 Rủi ro

1. **Múi giờ**: `GetLocalDate` = `localtime()` máy chạy GameServer (`ScriptFuns.cpp:3212-3229`); server ở máy khác (206.82.7.181) — **CHƯA RÕ** múi giờ ⇒ cửa 12:00-24:00 có thể lệch với giờ VN (memory: máy chủ Pacific UTC−8). Quyết định vận hành, không phải mã.
2. **Lỗi gốc Linux giữ nguyên** (ghi để khỏi "sửa nhầm"): (a) `OnTime :320-325` và `Close :395-399` đọc `GetTask(2323..)` với `PlayerIndex` **hiện hành của state** (không đổi sang `tb.nPlayerIndex`) ⇒ có thể đuổi người về toạ độ của người khác; (b) `player_death.lua:49 self` nil (A.4); (c) `TimeClose :790` format thiếu `%s` tên lôi chủ; (d) `IsMaxGrade :674` chỉ thông báo khi `nArenaId == 1`.
3. **`OnLeaveMap :240`** giả định `tbPlayerList[tên]` tồn tại: người có mặt trong 960 mà **không** qua `OnEnterMap` (đăng nhập lại ngay trong map — `SetLogoutRV(1)` `:230` khiến thoát trong map → vào lại ở điểm hồi sinh, nên hiếm) → 1 dòng ScriptError, không sập.
4. **Hai lôi đài gần nhau**: lôi chủ đài A (tmp 1) vs khiêu chiến đài B (tmp 2) là **địch** (C.2 rủi ro 2) — giống Linux.
5. **`AddNpcEx` cho NPC thoại** (`basemission_CallNpc` dùng cho Xa phu/Rương/Dược điếm/Quan nhắc nhở): **CHƯA RÕ** `LuaAddNpcEx` dự án (`ScriptFuns.cpp:6640`, port boss 21/08) có lấy `Kind=3` từ template để NPC bấm được không — nếu không, đổi `bairen_boot.lua` sang `AddNpc(1747,95,SubWorldID2Idx(176),x,y,1,tên)+SetNpcScript` (khuôn `autoexec.lua:233-242`) và 3 NPC trong map (`:68-101`) sẽ cần cùng cách — khi đó `hundred_arena.lua` phải sửa thêm 3 chỗ (ghi nhận, kiểm lúc thi công).
6. **`RemoteExecute` tại chỗ** gọi `ReceiveExc` → `call(DynamicExecute,…)` → phụ thuộc C.7; nếu chưa sửa C.7, thông báo toàn server bị cắt 64 byte nhưng trận vẫn chạy.
7. `AddOwnExp` 1e6/5 phút + 2e6 lôi chủ (trần 50 lượt/ngày): giá trị Linux, chủ game đã chọn "giống 100 %".
8. `AddTimer` dùng `GetTickCount` (ms) thay khung 18/s — lệch < 1 khung.

### I.2 Thứ tự thi công đề nghị

1. C++: C.6 → C.7 → C.5 → C.1 → C.2 → C.3 → C.4 (mỗi bước build CoreServer, 21 tên hàm xác minh trong DLL như đợt trước).
2. Script: chép 12 tệp (A.1) + vá (A.4, H) + 3 tệp mới + `startgame.lua` 2 dòng + `maplist.ini` 1 dòng + 11 tệp settings (B). Mirror vào `serverscript_jx2\bairenleitai\` như các đợt trước. Script python vá nên đặt ở `ReverseTools\port_bairenleitai\brlt_patch.py` với assert số lần thay thế (khuôn `tinsu_patch.py`).
3. Đặt `CoreServer.dll.moi_bairen` cạnh bản đang chạy, báo chủ game restart.

### I.3 Kiểm sau restart

| Bước | Mong đợi |
|---|---|
| Log boot | `[BAIREN] Bach Nhan Loi Dai khoi dong`, `HundredArena:IniEnvironment Done!!` (`:148`), không dòng `bairenleitai` trong `ScriptError.log`, không `[script] Include HONG … bairenleitai` |
| Lâm An (1464,3223) | NPC "Quan nhắc nhở Hoàng Thành Tư" bấm được, 1 mục → 3 mục của `bairen_dialog` |
| Ngoài giờ (<12:00) | "Thời gian mở Lôi Đài mỗi ngày vào lúc 12:00 ~ 24:00…" (`:418`); cấp <90: "Cấp 90 và 90 trở lên…" |
| GM ép mở ngoài giờ | `DynamicExecute("\\script\\missions\\bairenleitai\\hundred_arena.lua","dostring","[[HundredArena.IsOpen=function() return 1 end]]")` (hoặc chờ 12:00) |
| Vào map 960 | nhắn "Xin hãy sử dụng khinh công để nhảy lên lôi đài tỷ võ."; thấy Xa phu (1750,3159), Rương (1726,3141), Dược điếm (1748,3117); `GetTask(2323)` = map cũ |
| Nhảy lên đài 1 (ô 1784,3099) | bị kéo vào InPos, thông báo map "Lôi Đài1: <tên> trở thành Đài Chủ…"; 30 s sau không ai khiêu chiến → NPC "Cao thủ … Bách Nhân Lôi Đài" cấp 90 xuất hiện, 3 s → "Lượt đấu thứ 1 bắt đầu!" — **đánh được NPC** (kiểm C.2/C.5) |
| Người thứ hai nhảy lên | "Sau 3 giây bắt đầu chiến đấu!", 2 người **đánh được nhau** (camp 4 + fight + tmp 1/2); hết 3 phút so sát thương đúng (kiểm C.6) |
| Giết NPC | "Lôi Chủ … chiến thắng lượt thứ 1", lôi chủ giữ đài, chờ 30 s (kiểm `npc_death.lua` DIFF) |
| Chết trên đài | hồi sinh ở thành (điểm đăng nhập), thông báo "… rời khỏi Lôi Đài / trở thành Lôi Chủ mới" (kiểm `player_death.lua` DIFF + `OnLeaveMap`) |
| Thắng 10 trận liên tiếp đài 1 (GM giảm `nApply`/`nFight` trong `head.lua` để test) | thông báo **toàn server** đầy đủ, không cắt (kiểm C.4 + C.7) |
| Mỗi 5 phút | +1.000.000 exp (lôi chủ +2.000.000), `GetTask(2709)` tăng; 30 phút: Cổ Thủ xuất hiện + buff ×2 |
| 0:00 | "Lôi Đài Hoàng Thành Tư hôm nay kết thúc !" và bị đưa về map đã lưu |

---

## PHỤ LỤC P1 — 4 tệp MỚI (nội dung đề nghị, ASCII có dấu viết theo byte TCVN3 khi tạo tệp thật)

**`script\missions\bairenleitai\bairen_boot.lua`**
```lua
-- [BAIREN 22/08] Thay activitysys\config\9 (ServerStart) + zhaojingling\enternpc.lua (dat NPC) - engine ta 1 state/tep.
Include("\\script\\missions\\basemission\\lib.lua")	-- basemission_CallNpc
BAIREN_HA = "\\script\\missions\\bairenleitai\\hundred_arena.lua"
function BairenLeitai_Init()
	-- = config\9\config.lua:12-16: Include hundred_arena.lua roi HundredArena.IniEnvironment(HundredArena)
	DynamicExecute(BAIREN_HA, "HundredArena:IniEnvironment")
	-- = zhaojingling\enternpc.lua:7-15,44-53 (Linux xoa NPC nay luc boot: autoexec.lua:214 - chu game quyet dat lai)
	local szPos = "\\settings\\maps\\chrismas\\enternpc.txt"
	if (TabFile_Load(szPos, szPos) == 0) then OutputMsg("[BAIREN] thieu "..szPos) return 0 end
	basemission_CallNpc({
		szName = "Quan nhắc nhở Hoàng Thành Tư", nNpcId = 1747, nLevel = 95, nMapId = 176,
		nPosX = tonumber(TabFile_GetCell(szPos, 2, "POSX")), nPosY = tonumber(TabFile_GetCell(szPos, 2, "POSY")),
		szScriptPath = "\\script\\missions\\bairenleitai\\npc_quan_jx1.lua",
	})
	OutputMsg("[BAIREN] Bach Nhan Loi Dai khoi dong")
	return 1
end
```

**`script\missions\bairenleitai\npc_quan_jx1.lua`**
```lua
-- [BAIREN 22/08] main() NPC "Quan nhắc nhở Hoàng Thành Tư" = zhaojingling\enternpc.lua:17-30 (thoai) + config\9 ClickNpc -> bairen_dialog.
-- Khong dua muc "Hoàng Thành Tư huấn luyện cung" (Truy tim Moc Nhan - zhaojingling chua port).
Include("\\script\\dailogsys\\g_dialog.lua")
Include("\\script\\missions\\bairenleitai\\npc_enter.lua")
function main()
	local nNpcIndex = GetLastDiagNpc()
	local tbDailog = DailogClass:new(GetNpcName(nNpcIndex))
	tbDailog.szTitleMsg = "<npc>Để huấn luyện những đai nội cao thủ mới , để nâng cao chí sỹ giang hồ, Hoàng Thành Tư ta đây chiêu mộ Hòa Kiệt khắp nơi. Nếu như ngươi có ý định báo hiếu cho triều đình, hãy tham gia báo danh hoạt động. Học thành văn võ nghệ, giúp cho Đế Vương Gia. Các hạ võ nghệ đầy mình, bảo kiếm mông trần."
	tbDailog:AddOptEntry("Lôi Đài Hoàng Thành Tư", bairen_dialog)	-- = config\9\config.lua:32 AddDialogOpt -> detail 3 -> bairen_dialog
	tbDailog:Show()
end
```

**`script\missions\bairenleitai\newworld.lua`**
```lua
-- [BAIREN 22/08] map 960: thay EventSys EnterMap/LeaveMap (hundred_arena.lua:160-172 dang ky trong state rieng, newworldscript khong thay)
-- khuon missions\leaguematch\combat\newworld.lua
Include("\\script\\lib\\common.lua")	-- split() cho PraseParam
Include("\\script\\maps\\newworldscript_default.lua")
BAIREN_HA = "\\script\\missions\\bairenleitai\\hundred_arena.lua"
function OnNewWorld(szParam)
	DynamicExecuteByPlayer(PlayerIndex, BAIREN_HA, "HundredArena:OnEnterMap")
	OnNewWorldDefault(szParam)
end
function OnLeaveWorld(szParam)
	DynamicExecuteByPlayer(PlayerIndex, BAIREN_HA, "HundredArena:OnLeaveMap")
	OnLeaveWorldDefault(szParam)
end
```

**`settings\maplist.ini:6250`** → `960_NewWorldScript=\script\missions\bairenleitai\newworld.lua`
**`script\startgame.lua`**: sau dòng 18 thêm `Include("\\script\\missions\\bairenleitai\\bairen_boot.lua")	-- [BAIREN 22/08]`; sau dòng 101 thêm `DynamicExecute("\\script\\missions\\bairenleitai\\bairen_boot.lua", "BairenLeitai_Init")	-- [BAIREN 22/08] Bach Nhan Loi Dai (thay activitysys/config/9)`.

## PHỤ LỤC P2 — 2 tệp DIFF (bản dự án đầy đủ)

**`npc_death.lua`** (dòng 1-16 giữ nguyên; thay 17-30):
```lua
function OnDeath(nNpcIndex)
	if PlayerIndex <= 0 then
		-- 擂主输
		print("HundredArenaNPC Killed By NoOne!!");
		return
	end
	-- [JX1 22/08] HundredArena.tbArenaList song trong state cua hundred_arena.lua (engine ta 1 state/tep) -> goi sang do
	DynamicExecuteByPlayer(PlayerIndex, "\\script\\missions\\bairenleitai\\hundred_arena.lua", "HundredArena:JX1_NpcDeath", nNpcIndex)
end
```

**`player_death.lua`** (dòng 1-13 giữ nguyên; thay từ dòng 15):
```lua
-- [JX1 22/08] engine du an goi OnPlayerDeath(nVictimPIdx, nLauncherNpcIdx) (KNpc.cpp:1542), Linux goi OnDeath(Launcher) (0x08083805)
function OnPlayerDeath(nVictimPIdx, Launcher)
	OnDeath(Launcher)
end

function OnDeath(Launcher)
	local nKilledIndex = PlayerIndex;
	SetCurCamp(GetCamp());
	local nKillerIndex = NpcIdx2PIdx(Launcher);
	local HA = "\\script\\missions\\bairenleitai\\hundred_arena.lua"
	if (nKillerIndex <= 0) then
		print(Launcher, GetNpcName(Launcher));
		DelNpc(Launcher);
		DynamicExecuteByPlayer(nKilledIndex, HA, "HundredArena:JX1_PlayerDeath", 3);		-- 被NPC杀死
	else
		DynamicExecuteByPlayer(nKilledIndex, HA, "HundredArena:JX1_PlayerDeath", 0);		-- 0 = tu suy theo nRight (goc :31-35)
	end

	--Modified by DinhHQ - 20110714
	ST_StopDamageCounter();
	SetDeathScript("");
	DisabledUseTownP(0)	-- 可以回城符
	-- SetLogoutRV(0);	-- 只有在主动离开时 才设置为0
	DisabledStall(0)	-- 可以摆摊
	SetFightState(0)	-- 非战斗
	SetCurCamp(GetCamp());

	SetRevPos(GetPlayerRev())
	SetPKFlag(0);
	-- [JX1 22/08] goc :49-53 `self:DelPlayer(PlayerIndex)` + NewWorld: `self` la global chua gan -> Linux NEM LOI tai day,
	-- NewWorld khong bao gio chay; hanh vi thuc te = hoi sinh o diem dang nhap, roi map -> OnLeaveMap don. Giu dung hanh vi do.
end
```
(Các dòng chú thích Trung/Việt giữ nguyên byte GBK/TCVN3 của bản gốc khi vá; chỉ dòng có `[JX1 22/08]` là ASCII mới.)

---

## PHỤ LỤC P3 — LỆNH TÁI LẬP

```bash
cd D:\GAMEDEVNEW\ReverseTools && set PYTHONIOENCODING=utf-8
python port_bairenleitai\brlt_closure.py --list-missing        # bao dong Include (31 tep)
python port_bairenleitai\brlt_gap.py TREE <14 tep>              # ham engine thieu cua cay
python api_gap2.py BAIRENLEITAI missions/bairenleitai activitysys/config/9
python item_remap.py missions/bairenleitai                      # 0 item
python npc_skill_remap.py missions/bairenleitai                 # (bang nNpcId = N nen doi chieu tay, xem D.2)
python regionscan.py "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/server/Pak" "特殊用地\leitai"   # S=222 C=32
python re_disasm.py D:/ServerLinux/server1/jx_linux_y 0x0810BA50 75      # SetTmpCamp (va cac dia chi khac trong JSON)
python port_bairenleitai\find_off.py D:/ServerLinux/server1/jx_linux_y 0x1900   # moi lenh doc/ghi KNpc+0x1900
```
