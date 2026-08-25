# 12 — BỔ SUNG VÒNG 2: DÂY KHỞI ĐỘNG & LỊCH CHẠY của 3 hoạt động

> Phạm vi: **chỉ phân tích**, không sửa gì. Tệp này là tệp MỚI, không đụng `00_`..`09_`.
> Mọi khẳng định đều kèm bằng chứng `tệp:dòng` hoặc địa chỉ ELF. Chỗ chưa chắc ghi rõ **CHƯA XÁC MINH**.
> Ký hiệu: **A** = `D:\ServerLinux\server1` (thư mục chạy) · **B** = `D:\ServerLinux\Patch` — **[đã sửa theo đối chất]** B là **cây CLIENT**
> (`game_y.exe`, `engine.dll`, `represent2.dll`, `spr\`, `ui\`, `music\`, `vauto.exe`, `versionlist.ini`), **không phải lớp vá của máy chủ**:
> `Patch\script\` **không có** `missions\`, và **không có** `gateway\` / `relaysetting\` ⇒ B **không thể** ghi đè lịch S3Relay (đã trả lời §12.7)
> · **R** = `D:\ServerLinux\gateway\s3relay` (S3Relay) · **J** = `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server` (JX1 đang chạy)
> · **S** = `D:\GAMEDEVNEW\Sources` (mã nguồn JX1).

---

## 0. TÓM TẮT ĐIỀU HÀNH (đọc cái này là đủ)

Vòng 1 mô tả luồng chơi nhưng **thiếu hẳn tầng khởi động thứ hai**. Bản Linux có **HAI tầng**:

| Tầng | Tiến trình | Tệp gốc | Vai trò với 3 hoạt động |
|---|---|---|---|
| 1 | `jx_linux_y` (GameServer) | `\script\global\autoexec.lua` → `main()` | **Chỉ dựng NPC/bảng lúc boot**. Không có đồng hồ. |
| 2 | `s3relay_y` (S3Relay) | `\RelaySetting\Task\TaskList.ini` + 128 tệp `.lua` | **TOÀN BỘ LỊCH CHẠY**. Bắn lệnh `dwf`/`dw` sang GameServer. |

- **Sát thủ (Killer Boss)** — **KHÔNG CÓ LỊCH**. Là nhiệm vụ thường trực; chỉ có điểm khởi động boot-time (NPC 769 + ~160 boss).
- **Phong Lăng Độ** — lịch nằm ở **`R\relaysetting\task\fengling_ferry.lua`** (Task_86), **mỗi 60 phút, đúng phút :00**. ⇒ **ĐÃ TÌM RA chỗ mà `02_phonglangdo.md` mục 1.4 ghi là "nằm NGOÀI cây script"**.
- **Vượt ải (ChallengeOfTime)** — lịch ở **`R\relaysetting\task\challengeoftime.lua`** (Task_52), **mỗi 60 phút, phút :00**; cộng bảng xếp hạng ngày ở Task_84 lúc **00:00**.
- **`settings\systemtimetask.txt` là TỆP RỖNG (24 byte, chỉ có dòng tiêu đề)** ở cả A lẫn B ⇒ **không phải nguồn lịch**. Đừng port nó.
- Phía JX1: **đã có sẵn cả 3 tính năng ở dạng viết lại của VN** (`J\script\tinhnang\{boss_satthu, phonglangdo, vuot_ai}`), **đang bị TẮT** (2 dòng comment `timerserver.lua:79-80` + `startgame.lua:99`). JX1 cũng **đã có sẵn `GlobalExecute` hiểu tiền tố `dw`/`dwf`** (`KTongJX2.cpp:4056-4090`) ⇒ thân hàm `TaskContent()` của relay port sang chạy **nguyên văn**.

---

## 1. TẦNG 1 — GameServer boot (`autoexec.lua`)

### 1.1 Chứng minh ở mức ELF

Chuỗi `\script\global\autoexec.lua` nằm ở **độ lệch tệp `0x00221BAC`** = **VA `0x08269BAC`** trong `A\jx_linux_y`
(chênh lệch nạp = `0x08048000`).

```
0x081DF9C3  mov dword ptr [esp + 8], 0x8250931     ; tham so 3 = "" (chuoi rong)
0x081DF9CB  mov dword ptr [esp + 4], 0x826dbc2     ; ten ham = "main"
0x081DF9D3  mov dword ptr [esp],     0x8269bac     ; duong dan = "\script\global\autoexec.lua"
0x081DF9DA  call 0x80f90e0
```

`0x080F90E0` = `ExecuteScript(szPath, szFunc, szParam)`: kiểm null cả 3 tham số, gọi `0x0821DE70` (nạp/tra script)
rồi `jmp 0x080F9020` (gọi hàm). Ngay trước đó, `0x081DF9AF` nạp `\settings\citywar.ini` (VA `0x08269B96`).

Chuỗi `"main"` ở độ lệch `0x00225BC7`… thực tế `0x00225BC2` (`main` + `upvalues` liền kề).

> Đây chính là đối ứng 1-1 với JX1 `CoreServerShell::OnLunch()` → `startgame.lua` → `OnGame()`.

### 1.2 Điểm khởi động của từng tính năng trong `A\script\global\autoexec.lua`

| Dòng | Nội dung | Thuộc tính năng |
|---|---|---|
| 3 | `Include("\script\task\tollgate\killbosshead.lua")` | **Sát thủ** — nạp bảng `addkillertasknpc` (~160 boss) |
| 4 | `Include("\script\task\tollgate\addtollgatenpc.lua")` | Sát thủ (họ hàng: tollgate/messenger) |
| 28 | `Include("\script\global\autoexec_npc.lua")` | nạp `adddialognpc` (chứa NPC 769) |
| 151 | `add_dialognpc(adddialognpc)` | **Sát thủ** — dựng NPC 769 "Nhiếp Thí Trần" ở 7 thành |
| 158 | `add_killertasknpc(addkillertasknpc)` | **Sát thủ** — dựng toàn bộ boss sát thủ |
| 159 | `add_alltollgatenpc()` | Sát thủ/tollgate |
| 204-206 | `G_ACTIVITY:LoadActivitys()` / `G_TASK:LoadAllConfig()` / `G_ACTIVITY:OnMessage("ServerStart")` | hệ hoạt động (xem §6.4) |
| 208-209 | `tbTimeLineManager:LoadAllTimeLine(...)` / `AutoFunctions:Run()` | hai móc boot dùng chung |

**KHÔNG có dòng nào trong `autoexec.lua` khởi động Phong Lăng Độ hay Vượt ải.** Đã grep toàn cây A và B:
`fengling` / `challengeoftime` / `chuangguan` không xuất hiện trong bất kỳ tệp `autoexec*`.

### 1.3 NPC 769 — Nhiếp Thí Trần (dùng chung cho SÁT THỦ + VƯỢT ẢI)

`A\script\global\autoexec_npc.lua:26-32` — 7 dòng, cùng script `\script\task\tollgate\killer\nieshichen.lua`:

```
{769, 11,3210,4974,...}  Thành Đô
{769,  1,1506,3198,...}  Phượng Tường
{769, 37,1647,3050,...}  Biện Kinh
{769,176,1372,3010,...}  Lâm An
{769,162,1573,3227,...}  Đại Lý
{769, 78,1512,3206,...}  Tương Dương
{769, 80,1700,2963,...}  Dương Châu
```

`nieshichen.lua` **là NPC báo danh của CẢ HAI** tính năng:
- `nieshichen.lua:5` `Include("\script\missions\challengeoftime\npc\dragonboat_main.lua")`
- `nieshichen.lua:11` `Include("\script\missions\challengeoftime\rank_perday.lua")`
- `nieshichen.lua:19` `Include("\script\vng_feature\challengeoftime\npcNhiepThiTran.lua")` (lớp phủ VNG)
- `nieshichen.lua:54` `killertabfile = new(KTabFile,"/settings/task/tollgate/killer/killer.txt","KILLER")`
- Mục thoại: 22-29 = nhận nhiệm vụ sát thủ cấp 20..90; 24-25/27 = "Thử luyện sát thủ"/"Tham gia khiêu chiến" (vượt ải).

⇒ **Port JX1 phải giữ ràng buộc này**: một NPC, hai tính năng. (Đã có tiền lệ hỏng: memory ghi vụ NPC 87 vs 308 trùng tên ở Liên Đấu.)

### 1.4 Bảng dữ liệu boot của Sát thủ

| Tệp | A | B | Ghi chú |
|---|---|---|---|
| `script\task\tollgate\killbosshead.lua` | có (3421 dòng) | — | `add_killertasknpc()` ở **dòng 183-194**; `add_bossnpc()` 3393-3404; `add_messengernpc()` 3406-3414; hằng `SHOUHUZHE_OFFSET = 2` (dòng 1) dịch X của 9 "Bảo Khố Thủ Hộ Giả" (dòng 2922-2930) khỏi 9 "Bảo rương" (dòng 3380-3388) |
| `settings\task\tollgate\killer\killer.txt` | có (14 033 B) | có (14 033 B) | **giống hệt nhau** (`cmp` = 0) |
| `settings\droprate\boss\bosstask_lev20..90.ini` | có | có | **giống hệt nhau** (đã `cmp` lev20 và lev90) |

> ⚠️ **Đính chính brief vòng 2**: brief nói `bosstask_lev20..90.ini` "chỉ có ở B". **SAI** — cả 8 tệp có trong A và
> **byte-identical** với B; và chúng **không nằm** trong `chi_co_o_patch.txt` (grep = 0 hit). Kết luận về hai gốc dữ
> liệu vẫn đúng ở chỗ khác, nhưng riêng bảng của Sát thủ thì **không lệch A/B**.

---

## 2. TẦNG 2 — S3Relay TaskCentre (ĐÂY LÀ ĐỒNG HỒ THẬT)

### 2.1 Cơ chế (bằng chứng nhị phân)

Quét chuỗi trong `R\s3relay_y` (độ lệch tệp):

| Độ lệch | Chuỗi | Ý nghĩa |
|---|---|---|
| `0x0019EFD0` | `./swordonline/multiserver/S3Relay/TaskCentre.cpp` | tên tệp nguồn gốc |
| `0x0019EF82` | `\RelaySetting\Task\` | thư mục lịch |
| `0x0019EF96` | `TaskList.ini` | danh mục |
| `0x0019EFA9` / `0x0019EFA3` | `List` / `Count` | `[List] Count=N` |
| `0x0019EFAE` | `Task_%d` | `[Task_0]`… |
| `0x0019EFC4` | `TaskFile` | tên tệp `.lua` |
| `0x0019EFB6` | `ExcutedCount` | số lần đã chạy; **`-1` = ĐÃ TẮT** |
| `0x0019F045` | `TaskShedule` | callback cấu hình lịch (gõ sai chính tả trong gốc, giữ nguyên) |
| `0x0019F074` | `TaskContent` | callback thân việc |
| `0x0019F082` / `0x0019F093` | `GameSvrConnected` / `GameSvrReady` | callback sự kiện GS |
| `0x0019AB85`..`0x0019ABD3` | `TaskName` `TaskTime` `TaskInterval` `TaskCountLimit` `OutputMsg` `GlobalExecute` | API Lua cho script lịch |
| `0x0019EF6C` | `\RelayRunData\WarTask` | tệp nhật ký chạy |

Ngữ nghĩa `ExcutedCount` được chính bản mẫu chú thích (tiếng Trung) ở
`R\relaysetting\task\new\tasklist.ini:4-8`:

```
;[Task_n]中数据的说明
;TaskFile=任务设定文件名
;ExcutedCount=任务已经执行的次数 ... 如果填-1表示执行次数已经达到限制，此任务不会再被执行
```
(tệp mẫu này `Count=0` ⇒ vô hiệu, chỉ để tra cứu).

### 2.2 Ba mục đăng ký của 3 hoạt động

`R\relaysetting\task\tasklist.ini` (`[List] Count=128`, dòng 1-2):

| Dòng | Mục | TaskFile | ExcutedCount |
|---|---|---|---|
| 208-209 | `[Task_52]` | `challengeoftime.lua` | *(không có)* ⇒ **BẬT** |
| 311-312 | `[Task_84]` | `challegeoftime-dailyrank.lua` *(gốc gõ thiếu chữ `n`)* | *(không có)* ⇒ **BẬT** |
| 317-318 | `[Task_86]` | `fengling_ferry.lua` | *(không có)* ⇒ **BẬT** |

**Không có mục nào cho Sát thủ.** (Đã grep toàn bộ `gateway/` với `tollgate|kill_level|nieshichen|killboss|bosstask`
— chỉ trúng `relaysetting\task\pinganjijie.lua:69`, và tệp đó gọi
`dwf \script\missions\killbossmatch\ready.lua tbKillBossMatch_ready:StartGame()` — **`killbossmatch` là tính năng KHÁC**
("Đại hội săn boss"), và **pinganjijie.lua không có trong tasklist.ini** ⇒ cũng không chạy. Xem bẫy §6.3.)

### 2.3 Lịch Phong Lăng Độ — `R\relaysetting\task\fengling_ferry.lua` (toàn văn, 27 dòng)

```lua
function TaskShedule()
    TaskName("Phong Lăng độ")
    local nStartHour = tonumber(date("%H")) + 1;      -- dong 4
    if (nStartHour >= 24) then nStartHour = 0; end;   -- dong 6-8
    TaskTime(nStartHour, 0);                          -- dong 10  -> phut :00
    TaskInterval(60)                                  -- dong 13  -> 60 phut/lan
    TaskCountLimit(0)                                 -- dong 16  -> vo han
end
function TaskContent()
    GlobalExecute("dwf \\script\\missions\\fengling_ferry\\fldmap_boat1.lua fenglingdu_main()")  -- dong 21
end
```

### 2.4 Lịch Vượt ải — `R\relaysetting\task\challengeoftime.lua` (42 dòng)

```lua
INTERVAL_TIME = 60                                    -- dong 3
function GetNextTime() ... hour+1, tran 23->0 ...  end -- dong 6-14
function TaskShedule()
    TaskName("Thi đấu 'Thách thức thời gian'");        -- dong 17
    TaskInterval(INTERVAL_TIME);                       -- dong 20
    local h, m = GetNextTime(); TaskTime(h, m);        -- dong 22-23
    TaskCountLimit(0);                                 -- dong 26
end
function TaskContent()
    GlobalExecute("dwf \\settings\\trigger_challengeoftime.lua OnTrigger()");   -- dong 34
    szMsg = "Thời gian báo danh 'Thách thức thời gian' đã bắt đầu, ... Thời gian báo danh là 10 phút."
    GlobalExecute(format("dw AddLocalCountNews([[%s]], 2)", szMsg))             -- dong 36
end
```

### 2.5 Xếp hạng ngày Vượt ải — `challegeoftime-dailyrank.lua` (35 dòng)

`TaskTime(0, 0)` (dòng 7) + `TaskInterval(1440)` (dòng 10) ⇒ **00:00 mỗi ngày**.
Thân việc: `Ladder_GetLadderInfo(10235, 1)` → công bố đội nhanh nhất → `Ladder_ClearLadder(10235)` (dòng 18-26).
⇒ **Bảng xếp hạng Vượt ải dùng LADDER ID 10235 nằm trên S3Relay, không phải trên GameServer.**

### 2.6 `settings\trigger_challengeoftime.lua` — bên GameServer nhận lệnh

`A\settings\trigger_challengeoftime.lua` (704 B, **A ≡ B byte-identical**):

```lua
-- dong 1: "闯关活动触发器，由Relay每小时触发"  (bo kich hoat, do Relay kich moi gio)
Include("\\settings\\trigger_include.lua")           -- dong 5
Include("\\script\\missions\\challengeoftime\\include.lua")  -- dong 6
function OnTrigger()
    for i=1, getn(tbLevelMaps) do
        close_missions(tbLevelMaps[i], MISSION_MATCH, VARV_STATE);   -- dong 14
        start_missions(tbLevelMaps[i], MISSION_MATCH);               -- dong 15
    end
    ChuangGuan30:KickOutAll()                        -- dong 19
    ClearMapNpc(CHUANGGUAN30_MAP_ID)                 -- dong 20
    DynamicExecute("\\script\\missions\\challengeoftime\\chuangguang30.lua", "ChuangGuan30:AddTime")  -- dong 22
end
--OnTrigger();                                       -- dong 24  <-- BI COMMENT CO CHU DICH
```

> 🔑 **Điểm dễ bỏ sót**: dòng 24 bị comment **vì relay gọi `OnTrigger()` bằng tên** qua `dwf <file> <func>`.
> So sánh `trigger_dragonboat.lua:14` thì **KHÔNG** comment (`OnTrigger();`) — bản Long Thuyền dùng kiểu chạy-thân-tệp.
> Khi port sang JX1, nếu bê nguyên `trigger_challengeoftime.lua` mà quên gọi hàm ⇒ **không có gì xảy ra**.

`trigger_include.lua` (828 B, A ≡ B) chỉ chứa `start_missions(map, mission)` / `close_missions(map, mission, status)`
có bảo toàn `SubWorld` (dòng 10, 18, 23, 33) — bản `trigger.lua` (Long Thuyền, 759 B) **thiếu** bảo toàn đó.

### 2.7 Chuỗi lệnh xuống GameServer

`GlobalExecute` của relay nhận 2 tiền tố định tuyến:
- `dw <câu lệnh Lua>` → chạy trên state `\script\gmscript.lua` của GS.
- `dwf <đường dẫn> <câu lệnh>` → chạy trong state của `<đường dẫn>`.

(Đã xác nhận cách hiểu này khớp 100% với hiện thực JX1 — xem §7.2.)

---

## 3. BẢNG SỰ KIỆN — SÁT THỦ (Killer Boss)

| Sự kiện | Nguồn kích hoạt (tệp:dòng / địa chỉ) | Chu kỳ | Ghi chú |
|---|---|---|---|
| Nạp bảng boss | `A\script\global\autoexec.lua:3` → `killbosshead.lua` | 1 lần / boot | ~160 dòng `addkillertasknpc` |
| Dựng NPC 769 (7 thành) | `autoexec.lua:151` `add_dialognpc(adddialognpc)` ← `autoexec_npc.lua:26-32` | 1 lần / boot | script `killer\nieshichen.lua` |
| Dựng boss sát thủ | `autoexec.lua:158` `add_killertasknpc(addkillertasknpc)` → `killbosshead.lua:183` | 1 lần / boot | mỗi boss `SetNpcParam(idx,1,<số thứ tự>)` (dòng 191) |
| Dựng NPC tollgate/messenger | `autoexec.lua:159` `add_alltollgatenpc()` | 1 lần / boot | nhánh "Phong chi kỵ / Thần miếu / Thiên bảo khố" |
| Hồi sinh boss sau khi chết | **[đã sửa theo đối chất] CÓ HỒI SINH** — cờ cấm hồi sinh là **cột 6** (`=0`), không phải cột 8 | theo `ReviveFrame` của `npcs.txt` | ELF `0x0811BDB2`; xem §9.1 |
| **Lịch theo giờ** | **KHÔNG CÓ** | — | không có mục trong `timertask.txt`, `missions.txt`, `tasklist.ini` |

**Kết luận Sát thủ: là hoạt động THƯỜNG TRỰC 24/7.** Toàn bộ "khởi động" nằm ở boot GameServer.
Giới hạn chơi là **giới hạn theo NGƯỜI** (8 lượt/ngày theo thoại `nieshichen.lua:17` bị comment; bảng `killer.txt`),
**không phải theo giờ**.

---

## 4. BẢNG SỰ KIỆN — PHONG LĂNG ĐỘ

| Sự kiện | Nguồn kích hoạt | Chu kỳ | Ghi chú |
|---|---|---|---|
| Đăng ký việc định giờ | `R\relaysetting\task\tasklist.ini:317-318` `[Task_86]` | 1 lần / khởi động Relay | không có `ExcutedCount` ⇒ bật |
| Tính giờ chạy đầu | `fengling_ferry.lua:4-10` (`giờ hiện tại + 1`, phút 0) | 1 lần | restart giữa giờ ⇒ nổ ở đầu giờ kế |
| **Mở lượt** | `fengling_ferry.lua:21` `dwf …\fldmap_boat1.lua fenglingdu_main()` | **60 phút, phút :00** | |
| Mở mission trên 3 bến | `A\script\missions\fengling_ferry\fldmap_boat1.lua:16-19` `CloseMission(15)`→`OpenMission(15)`→`SetMissionV(MS_STATE,1)` | mỗi lượt | map 337/338/339 (`fld_head.lua:12`) |
| Thông báo toàn server | `fldmap_boat1.lua:22-23` `AddGlobalCountNews(str, 3)` | mỗi lượt | |
| Đặt 2 đồng hồ mission | `A\script\missions\fengling_ferry\mission.lua:15-16` `StartMissionTimer(15, 29, FLD_TIMER_1)` + `(15, 28, FLD_TIMER_2)` | mỗi lượt | id 29 = 20 giây; id 28 = 39 phút |
| Đóng báo danh, thuyền chạy | `fld_smalltimer.lua:8-14` khi `t == ENDSIGN_TIME` | **phút thứ 10** | `fld_head.lua:21` `ENDSIGN_TIME = 10*60*18/FLD_TIMER_1` |
| Boss Thủy tặc đầu lĩnh #1 | `fld_smalltimer.lua:35-40` `t == UPBOSS_TIME` | **phút 25** | `fld_head.lua:22` |
| Boss #2 (+ 2 **Đại** đầu lĩnh nếu giờ đặc biệt) | `fld_smalltimer.lua:42-56` | **phút 30** | npc 1692, xem §4.1 |
| Boss #3 | `fld_smalltimer.lua:58-63` | **phút 35** | `fld_head.lua:24` |
| Cập bờ Bắc + trả thưởng | `mission.lua:56-63 EndMission()` → `Landing()` (dòng 79-…) | **phút 39** (timer 28) | `G_ACTIVITY:OnMessage("FinishFengLingDu")` dòng 107 |
| Bảng ID | `A\settings\task\missions.txt:16` → MISSION **15** ; `A\settings\timertask.txt:29,30` → TIMER **28, 29** | — | A ≡ B (`cmp` = 0) |

### 4.1 "Giờ đặc biệt" của Phong Lăng Độ — `check_new_shuizeitask()`

`A\script\missions\fengling_ferry\fld_head.lua:302-317`:

```lua
function check_new_shuizeitask()
    local nHour = tonumber(GetLocalDate("%H"));
    local tb_sptime = { [10]=1, [14]=1, [16]=1, [18]=1, [20]=1 };   -- dong 305-311
    if (tb_sptime[nHour] and tb_sptime[nHour] == 1) then return 1 else return 0 end
end
```

Ảnh hưởng (3 chỗ dùng):
1. `fld_head.lua:66-72` — đổi điều kiện lên thuyền: giờ đặc biệt cần **"Lệnh Bài Thủy Tặc"** (item `particular 2745`),
   giờ thường thì Lệnh bài PLD (`AddEventItem(489)`) hoặc **200 Mật đồ thần bí**.
2. `fld_head.lua:115-119` — giờ đặc biệt: bến 2/3 bị `SetTaskTemp(200,1)` + `ForbidEnmity(1)` (chú thích
   `-- 20110405: Fix bug, ngoài thời gian 13h,15h,17h,19h bến 2 3 có thể pk cừu sát`).
3. `fld_smalltimer.lua:48-55` — giờ đặc biệt spawn thêm **2 × NPC 1692 "Thủy Tặc Đại Đầu Lĩnh"** ở phút 30.
4. `mission.lua:105,119-121` — giờ đặc biệt phát thêm `tbAwardTemplet:GiveAwardByList({tbProp={6,1,2743,1,0,0}}, "fenglingdu_shuizeicaibao", 2)`.

> ⚠️ **Lệch tài liệu**: chú thích thoại `fld_head.lua:67` liệt kê `10:00,14:00,16:00,18:00,20:00` (khớp mã),
> nhưng bản relay dự phòng `R\relaysetting\task\phonglangdo\fengling_ferry_0000.lua:12` in ra
> `10h 14h 16h 18h 20h **22h**`, và **bản JX1 đã có `[22]=1`** (`J\script\tinhnang\phonglangdo\lib_phonglangdo.lua:138`).
> ⇒ Khi port phải chọn: **5 giờ (bản Linux đang chạy)** hay **6 giờ (ý định vận hành VN)**.

---

## 5. BẢNG SỰ KIỆN — VƯỢT ẢI (ChallengeOfTime)

| Sự kiện | Nguồn kích hoạt | Chu kỳ | Ghi chú |
|---|---|---|---|
| Đăng ký việc định giờ | `tasklist.ini:208-209` `[Task_52]` | 1 lần / khởi động Relay | |
| Tính giờ chạy đầu | `challengeoftime.lua:6-14, 22-23` | 1 lần | giờ hiện tại + 1, phút 0 |
| **Mở báo danh** | `challengeoftime.lua:34` `dwf \settings\trigger_challengeoftime.lua OnTrigger()` | **60 phút, phút :00** | |
| Thông báo báo danh | `challengeoftime.lua:36` `dw AddLocalCountNews(...,2)` | mỗi lượt | text: "báo danh là 10 phút" |
| Đóng rồi mở lại mission trên 32 map | `trigger_challengeoftime.lua:13-16` qua `trigger_include.lua:9-19` | mỗi lượt | `tbLevelMaps` = `include.lua:90-103` |
| Đá sạch map ải 30 + hẹn giờ | `trigger_challengeoftime.lua:19-22` → `chuangguang30.lua:257-269` `AddTimer((13*60+600+60)*18, "ChuangGuan30:GameTime", 0)` | mỗi lượt | map **957** (`include.lua:27`) |
| Đặt trạng thái báo danh + đồng hồ trận | `A\script\missions\challengeoftime\mission_match.lua:16-30` `InitMission()` → `SetMissionV(VARV_STATE,1)` + `StartMissionTimer(22, 41, LIMIT_SIGNUP*18)` | mỗi lượt | **10 phút** (`include.lua:12-13`) |
| **Bắt đầu thi đấu** | `timer_match.lua:13-44` `start_match()` → `SetMissionV(VARV_STATE,2)` + `start_close_timer()` | phút thứ **10** | hủy nếu 0 người (dòng 17-20) |
| Bảng thông báo mỗi phút | `timer_board.lua:7-22` (TIMER **42**) | 60 giây | `INTERVAL_BOARD = 1` |
| **Hết giờ / thất bại** | `timer_close.lua:11-19` (TIMER **43**) | phút thứ **10 + 30** | `LIMIT_FINISH = 30*60` (`include.lua:14`) |
| Ải 30 sinh boss | `chuangguang30.lua:233-241 GameTime()` → `OnAddBoss()` | 1 lần/lượt, sau ~13+11 phút | chỉ khi map 957 có người (dòng 234) |
| Khung giờ mở ải 30 | `include.lua:29-30` `CHUANGGUAN30_START_TIME=10`, `..._END_TIME=22` | **KHÔNG có hiệu lực** | 🔴 **MÃ CHẾT** — grep toàn A+B: chỉ có 2 dòng khai báo, không nơi nào đọc ⇒ ải 30 mở 24/24 |
| Bảng xếp hạng ngày | `tasklist.ini:311-312` `[Task_84]` → `challegeoftime-dailyrank.lua:7,10` | **00:00 hằng ngày** | Ladder 10235 trên Relay |
| Bảng ID | `A\settings\task\missions.txt:23` → MISSION **22** ; `A\settings\timertask.txt:42,43,44` → TIMER **41, 42, 43** | — | A ≡ B |

### 5.1 Danh sách map thi đấu (`include.lua:90-103`) vs thực tế triển khai

```
tbLevels (include.lua:83-88):  [1] = {50, 90}  So cap   ·  [2] = {90, 201}  Cao cap
tbLevelMaps[1] = 464..479   (16 map)
tbLevelMaps[2] = 480..495   (16 map)
```

**PHÁT HIỆN MỚI (chưa có ở vòng 1):** `R\settings\worldset.txt` (bảng map → GameServer, 696 dòng map)
**chỉ có 464..471**; **472, 473, 474, 475, 476, 477, 478, 479 KHÔNG CÓ DÒNG NÀO** (`grep -c $'\t472\t'` = 0).
Cao cấp 480..495 thì đủ cả 16.

Hệ quả: `SubWorldID2Idx(472..479)` trả −1 ⇒ `trigger_include.lua:13` bỏ qua **im lặng**
⇒ **tầng Sơ cấp thực tế chỉ có 8 phòng đồng thời, không phải 16**.

Đối chiếu: các map 464..479 **có** đăng ký trong `A\settings\maplist.ini` (dòng 3690 `464=特殊用地\杀手的试炼`,
dòng 3714 `472=…`, dòng 3749 `479=…`) — nghĩa là **map tồn tại nhưng không được gán cho GameServer nào**.

> Tên thư mục map của Vượt ải trong `maplist.ini` là `特殊用地\杀手的试炼` = **"Sát thủ đích thí luyện"** —
> thêm một bằng chứng nữa rằng Vượt ải và Sát thủ là **một cụm tính năng** trong thiết kế gốc.

### 5.2 Số lượt/ngày — ba con số MÂU THUẪN

| Nguồn | Giá trị |
|---|---|
| `include.lua:32` | `COUNT_LIMIT = 1` |
| `npc\dragonboat_main.lua:147-151` | reset lười theo `TSK_JOIN_DATE(1551)`, gán `TSK_REMAIN_COUNT(1550) = COUNT_LIMIT` rồi trừ 1 |
| Thoại NPC `nieshichen.lua:48` | *"Mỗi người chỉ tối 2 lần/ngày"* + *"Thời gian báo danh là 5 phút"* |
| Lớp phủ VNG `vng_feature\challengeoftime\npcNhiepThiTran.lua:25-32` | chặn khi `nDailyTaskCount >= 3`; 1 lượt free + 2 lượt qua **Long Huyết Hoàn** |

⇒ Thật sự đang chạy: **1 lượt miễn phí + tối đa 2 lượt mua bằng Long Huyết Hoàn = 3**.
Thoại NPC ("2 lần", "5 phút") là **văn bản cũ chưa cập nhật** — đừng chép sang JX1 làm chuẩn.

---

## 6. NHỮNG THỨ **KHÔNG** PHẢI NGUỒN LỊCH (bẫy — đã kiểm chứng)

### 6.1 `settings\systemtimetask.txt` — TỆP RỖNG
- `A\settings\systemtimetask.txt` = **24 byte**, `B\...` = **24 byte**, `cmp` = giống hệt.
- Nội dung đầy đủ: dòng 1 = `HOUR<TAB>MIN<TAB>SCRIPT<TAB>d<TAB>DESC`, dòng 2 = rỗng. **Không có dòng dữ liệu nào.**
- Engine có đọc: chuỗi `\settings\systemtimetask.txt` ở độ lệch `0x002098D8` của `jx_linux_y`.
- ⇒ **Cơ chế tồn tại nhưng không dùng.** JX1 **không có** đối ứng (grep `systemtimetask` trong `S\Core\Src` + `S\MultiServer` = 0 hit) ⇒ **không cần port**.

### 6.2 `settings\killer.ini` — KHÔNG liên quan Sát thủ
`A\settings\killer.ini:1` ghi rõ `;通缉追杀系统专用ini` = **hệ TRUY NÃ người chơi (bounty PvP)**:
`MoneyPerHour=10000`, `MinTargetLevel=50`, `MaxActiveTaskTime=10`, `MinReward=100000`,
thông điệp `TargetLevelTooLow=Đối tượng dưới cấp 50 không thể truy nã!`.
Engine đọc nó (`\settings\killer.ini` ở `0x00209D3D`). **Đừng nhầm với `script\task\tollgate\killer\`.**
Bảng đúng của Sát thủ là `settings\task\tollgate\killer\killer.txt`.

### 6.3 `killbossmatch` ≠ Sát thủ
`A\settings\timertask.txt:113,114` (TIMER 112, 113) và `missions.txt:69,70` (MISSION 68, 69) trỏ tới
`\script\missions\killbossmatch\…`. Đó là **"đại hội săn boss"** riêng, kích hoạt bởi
`R\relaysetting\task\pinganjijie.lua:69` — mà tệp đó **không có trong `tasklist.ini`** ⇒ **không chạy**.

### 6.4 `activitysys` là MÓC THƯỞNG, không phải bộ lịch
- `autoexec.lua:206` `G_ACTIVITY:OnMessage("ServerStart")` chỉ đánh thức các activity có
  `szMessageType = "ServerStart"`; **không có** activity nào mở mission cho 3 tính năng của ta
  (đã đọc toàn bộ config có `ServerStart`: 1002, 1004, 1007, 1008, 1010, 1021, 1023, 1024, 1025, 1029, 1034…).
- Liên quan của ta chỉ là **hook nhận thưởng**, ví dụ `A\script\activitysys\config\45\config.lua`:
  `tbConfig[9] szMessageType="Chuanguan" szName="chuangguan_17"` (dòng 162-163),
  `tbConfig[10] "chuangguan_28"` (181-182), `tbConfig[11] "FinishFengLingDu"` (200-201),
  `tbConfig[12] "fenglingdu_boatboss"` (218-219), `tbConfig[13] "fenglingdu_bigboatboss"` + `NpcFunLib:CheckId {"1692"}` (238-247).
- Bên bắn sự kiện: `fengling_ferry\mission.lua:107` `G_ACTIVITY:OnMessage("FinishFengLingDu", tbPlayer)`;
  `challengeoftime\award.lua:254` và `chuangguang30.lua:197` `G_ACTIVITY:OnMessage("Chuanguan", batch, tbAllPlayer, n_level)`;
  `activitysys\g_npcdeath.lua:72` `DynamicExecute(...chuangguang30.lua, "ChuangGuan30:OnNpcDeath", ...)`.
- `A\script\huoyuedu\huoyuedu.lua:22` `["fenglingdu"] = 2` — điểm độ sôi nổi, cũng chỉ là hook.

### 6.5 Các biến thể `*_HHMM.lua` trong `relaysetting\task\` — CÓ TỆP NHƯNG KHÔNG CHẠY
Tồn tại: `fengling_ferry_0200/0400/…/2400.lua` (12 tệp, mỗi 2 giờ) và `challengeoftime_0100/0300/…/2300.lua`
(12 tệp, giờ lẻ) — thiết kế "PLD giờ chẵn / Vượt ải giờ lẻ".
**Không tệp nào được đăng ký trong `tasklist.ini`.** Ví dụ `fengling_ferry_0200.lua:20-26` là bản
`TaskInterval(1440); TaskTime(2,0)` (ngày 1 lần) với toàn bộ bản gốc bị comment ở dòng 1-18.
Các thư mục `phonglangdo/`, `vuotai/` (ghi `***Edit*By*Heart*Doldly***`, năm 2018) là **kho bản thay thế**, không chạy.

### 6.6 `script\timerserver*` / `timertask*` — **KHÔNG TỒN TẠI ở bản Linux**
`find server1/script -iname "timerserver*" -o -iname "timertask*"` = **0 kết quả**.
Bản Linux **không có đồng hồ mỗi-phút bên GameServer**; đó là điểm khác kiến trúc lớn nhất so với JX1.
`A\script\global\everyday_do.lua` (8 dòng) chỉ `SetTask(3455, 0)` — reset thời gian online, không liên quan.

### 6.7 `gateway` — không có lịch cho 3 tính năng ngoài S3Relay
`bishop_y`, `goddess_y`, `BishopConn`, `KG_SyncD`, `libjxinfo.so`, `GameExtContent.so`, `GameExtConnect.so`:
quét chuỗi `trigger|OnTrigger|timertask|systemtimetask|challengeoftime|fengling` ⇒ **0 hit** trên tất cả.

---

## 7. LỆNH GM (cả 3 tính năng)

Tất cả nằm trong **một tệp duy nhất**: `A\script\gmscript\gmcommand_3.lua` ("活动相关" = liên quan hoạt động).
Đã quét thêm `A\script\gm_tool\` (4 tệp: `dispose_item`, `exp_recall`, `laoanmay`, `libgm`),
`A\script\global\gm\`, `A\script\gmscript.lua`, và `gmcommand_0..9` ⇒ **không có gì khác** cho 3 tính năng.

### 7.1 Menu và các mục

`gmcommand_3.lua:1-24` — `Include` ở đầu tệp đã nạp sẵn hai thứ cần thiết:
- dòng 2: `Include("\\script\\task\\tollgate\\killbosshead.lua")` (để có bảng `addkillertasknpc`)
- dòng 4: `Include("\\settings\\trigger_challengeoftime.lua")` (để có `OnTrigger`)

| Mục menu | Dòng | Hàm | Nội dung |
|---|---|---|---|
| `时间挑战` (Vượt ải) | 15 | `GMC_ChallengeOfTime` (258-267) | 4 lệnh con, xem dưới |
| `Nhiệm vụ Sát thủ` | 17 | `GMC_KillBoss` (441-456) | 10 lệnh con |
| `Thuyền Phong Lăng Độ` | 21 | `GMC_FLD` (818-827) | 4 lệnh con |

**Vượt ải** (`gmcommand_3.lua:258-289`):
| Lệnh | Dòng | Hành động |
|---|---|---|
| `报名npc` | 269-272 | `LoadMap(11); NewWorld(11,3207,4978)` → dịch chuyển tới NPC 769 Thành Đô |
| `Sát thủ giản` | 274-276 | `AddItem(6,1,400,90,1,0)` × 2 (2 sát thủ giản cấp 90) |
| `直接开启活动` | 278-280 | **`OnTrigger()`** — mở lượt ngay, không đợi relay |
| `设置关卡数` | 282-289 | `SetMissionV(1, n)` — nhảy ải |

**Sát thủ** (`gmcommand_3.lua:441-488`):
| Lệnh | Dòng | Hành động |
|---|---|---|
| `领任务npc 聂弑尘` | 458-460 | `NewWorld(11,3207,4978)` (**thiếu `LoadMap` — xem §9.3**) |
| `任务道具 弑者腰牌` | 462-466 | `AddItem(6,1,2346,1,0,0)` — **item 2346 = Thí Giả Yêu Bài** |
| `20..90级Boss` | 446-453 | `KillBoss_Transfer(self, <cấp>)` |
| (bộ dịch chuyển) | 468-488 | lọc `addkillertasknpc[i][2] == nLevel`, dịch chuyển tới `[3],[4],[5]` |

**Phong Lăng Độ** (`gmcommand_3.lua:818-865`):
| Lệnh | Dòng | Hành động |
|---|---|---|
| `报名npc` | 829-852 | 3 thuyền phu ở map **336**: `(1154,3026) (1282,2906) (1542,2811)` |
| `开启风陵渡` (Start) | 854-857 | `Include(fldmap_boat1.lua); main()` ← **LỖI, xem §9.2** |
| `开启风陵渡` (End) | 863-865 | `CloseMission(15)` — **nhãn bị chép nhầm**, đây là lệnh ĐÓNG |
| `Lệnh bài Phong Lăng Độ` | 859-861 | `AddEventItem(489)` |

---

## 8. BẰNG CHỨNG RUNTIME (nhật ký thật của Relay)

`R\RelayRunData\WarTask_20260806.log` (78 683 B) — bản chạy thật ngày 2026-08-06.

**Lúc nạp (06:45:52):**
```
"载入定时任务设定文件：\RelaySetting\Task\TaskList.ini"
"定时任务数量：128"
定时任务0：announce.lua      → 定时任务1：game_module_settings.lua → 定时任务50：ladderload.lua
```
⇒ **[đã sửa theo đối chất]** Log chỉ nạp **65/128** việc. Bị bỏ qua **63** việc — **không phải chỉ Task_2..49**, mà là
`Task_2..49` **cộng thêm** `56, 58, 61, 66, 68, 82, 83, 108, 110, 113, 120, 121, 123, 124, 125`.
Tập 63 này **trùng khít 100%** với 63 mục có `ExcutedCount=-1` trong `tasklist.ini` ⇒ xác nhận `-1 = tắt`.

| Task | Tên đặt (`TaskName`) | `TaskInterval` | `TaskTime` | Lần chạy kế |
|---|---|---|---|---|
| 52 `challengeoftime.lua` | `Thi đấu 'Thách thức thời gian'` | **3600 giây** | **7 giờ 0 phút** | `7:0` |
| 84 `challegeoftime-dailyrank.lua` | `闯关活动每日排行榜` | **86400 giây** | **0 giờ 0 phút** | `0:0` (log ghi `今天不会运行：时间过了！`) |
| 86 `fengling_ferry.lua` | `Phong Lăng Độ` | **3600 giây** | **7 giờ 0 phút** | `7:0` |

**Lúc nổ (07:00:52):**
```
2026-08-06 07:00:52  执行定时任务：challengeoftime.lua
2026-08-06 07:00:52  执行定时任务：fengling_ferry.lua
```

⇒ **Xác nhận tuyệt đối**: cả Phong Lăng Độ lẫn Vượt ải chạy **cùng lúc, mỗi giờ, phút :00**
(độ trễ ~52 giây là chu kỳ quét của relay, không phải cấu hình).
**Sát thủ không xuất hiện một lần nào trong log** — đúng như kết luận "không có lịch".

---

## 9. LỖI / LỆCH PHÁT HIỆN THÊM (mang sang JX1 phải vá)

### 9.1 `AddNpc` cột 8 của boss sát thủ — **[đã sửa theo đối chất] ĐÃ XÁC MINH**
`killbosshead.lua:189` gọi `AddNpc(id, level, subworld, x*32, y*32, Tab3[i][6], Tab3[i][7], Tab3[i][8])`
với `[6]=0`, `[7]=tên`, `[8]=1`.

Đã dịch ngược `jx_linux_y` hàm `AddNpc` tại **`0x0811BB10`** (luamap dòng 667). Chữ ký thật:

| Tham số | Xử lý trong ELF | Ý nghĩa |
|---|---|---|
| 1..5 | `0x0811BB19`–`0x0811BC9C` | id / level / **subworld idx** / x / y (`argc` phải ≥ 5: `cmp eax,4; jg` tại `0x0811BB2D`) |
| **6** | `0x0811BDB0` `test eax,eax` → **`0x0811BDB2` `setne byte ptr [KNpc+0x1824]`** | **cờ `bNoRevive`** — chết là biến mất |
| 7 | `0x0811BDCD` `lua_tostring` → `0x0811BED8` | **tên NPC** (chỉ đặt khi chuỗi khác rỗng) |
| **8** | `0x0811BDF0`–`0x0811BE25`; `==1` → `0x0811BEF2` `call 0x08085250` rồi `[KNpc+0x181C] = 3` (nếu ts6≠0) hoặc **`= 2`**; `==2` → reset đối tượng con `+0x88` | **nạp lại thuộc tính NPC từ mẫu**, **KHÔNG dính hồi sinh** |
| 9 | chỉ đọc khi `argc > 8` (`0x0811BBE4 cmp esi,8`), mặc định **−1** | nSeries (ngũ hành) |

⇒ **Kết luận: boss sát thủ CÓ tự hồi sinh** — vì cột **6 = 0** nên `bNoRevive` **không** bật; hồi sinh chạy theo
`ReviveFrame` của mẫu trong `npcs.txt`. Cột 8 chỉ ép nạp lại thuộc tính từ mẫu.

🔴 **Lệch ngữ nghĩa khi port sang JX1**: `LuaAddNpc` của dự án (`S\Core\Src\ScriptFuns.cpp:6814-6844`) đọc
**tham số 6 là `nSeries`** (`:6834`) và **tham số 7 là tên** (`:6836-6841`), **bỏ hẳn tham số 8**.
Với `killbosshead.lua` thì vô hại (cột 6 = 0 ⇒ nSeries = 0), nhưng **bất kỳ bảng nào có cột 6 ≠ 0 sẽ bị JX1 hiểu
thành ngũ hành thay vì `bNoRevive`** — dự án đã gặp đúng chuyện này và phải viết hàm riêng `LuaAddNpcEx`
(`ScriptFuns.cpp:6846-6946`, chú thích 6846-6873 đã ghi sẵn nghĩa `+0x1824` / `+0x181C`).

### 9.2 🔴 GM "mở Phong Lăng Độ" **GỌI SAI TÊN HÀM**
```lua
-- gmcommand_3.lua:854-857
function tbCommand3:FLD_Start()
    Include("\\script\\missions\\fengling_ferry\\fldmap_boat1.lua");
    main();                       -- <-- SAI
end
```
Nhưng `fldmap_boat1.lua:4` định nghĩa **`function fenglingdu_main()`**, không có `main()`.
⇒ Lệnh GM này gọi vào **`main` toàn cục nào đang tồn tại trong state đó** (rất có thể là `main` của
`gmscript\head.lua` hoặc `autoexec.lua`) ⇒ **không mở được Phong Lăng Độ, và có thể chạy nhầm thứ khác**.
Đối chiếu: relay gọi đúng `fenglingdu_main()` (`relaysetting\task\fengling_ferry.lua:21`).
**Khi port sang JX1 phải sửa thành `fenglingdu_main()`.**

### 9.3 GM "tới NPC Sát thủ" thiếu `LoadMap`
`gmcommand_3.lua:458-460` chỉ `NewWorld(11,3207,4978)`, trong khi bản Vượt ải cùng toạ độ
(`gmcommand_3.lua:269-272`) có `LoadMap(11)` trước. Nếu map 11 chưa nạp ⇒ dịch chuyển hỏng.

### 9.4 GM Phong Lăng Độ: hai mục cùng nhãn `开启风陵渡`
`gmcommand_3.lua:822` và `:823` — mục thứ hai thật ra là `FLD_End` (`CloseMission(15)`). Nhãn phải là `关闭风陵渡`.

### 9.5 Bảng `worldset.txt` thiếu 8 map Vượt ải sơ cấp
Xem §5.1. Khi port sang JX1 nhớ kiểm `MapList.ini` + bảng phân map, nếu không sẽ lặng lẽ mất một nửa số phòng.

### 9.6 Thoại NPC lệch với mã (số lượt, thời gian báo danh)
Xem §5.2. Ba nguồn nói 3 con số khác nhau.

### 9.7 Bảng xếp hạng Vượt ải nằm trên Relay
`Ladder_GetLadderInfo(10235, 1)` / `Ladder_ClearLadder(10235)`
(`challegeoftime-dailyrank.lua:18, 26`) chạy **trong state của S3Relay**, dùng `IncludeLib("RELAYLADDER")`
(`A\script\missions\challengeoftime\include.lua:1`). Đây là chỗ vòng 1 chưa đề cập.

> **[đã sửa theo đối chất]** Không cần "quyết định đưa ladder về đâu" nữa: **JX1 ĐÃ CÓ SẴN hệ ladder trong tiến trình**.
> Đăng ký Lua: `S\Core\Src\ScriptFuns.cpp:15086` `Ladder_ClearLadder` và `:15087` `Ladder_GetLadderInfo`
> (khai báo `extern` ở `:13565-13566`); hiện thực thật (có lưu xuống đĩa) ở `S\Core\Src\KJx2SharedStore.cpp:580`
> và `:599`. Chốt chặn id là `JX2LADDER_MIN_ID = 10000` (`KJx2SharedStore.cpp:441`) và **10235 > 10000** ⇒
> `Ladder_GetLadderInfo(10235,1)` / `Ladder_ClearLadder(10235)` **chạy nguyên văn**, top giữ 10 mục
> (`JX2LADDER_TOP = 10`, `:440`) đúng như bản relay.
>
> Phụ thuộc còn thiếu ở bản Linux: ladder **được nạp bởi một việc relay KHÁC** — `Task_50 ladderload.lua`
> (đang BẬT, có trong log). `ladderload.lua:18-20` `for i=10001,10300 do LoadLadder(i) end` trong `TaskShedule()`,
> và `:3-5` `function GameSvrConnected(dwGameSvrIP) SyncAllLadder(dwGameSvrIP) end`.
> ⇒ Nếu port kiểu relay mà quên Task_50 thì ladder 10235 **không có kho chứa**.

---

## 10. ĐỐI CHIẾU JX1 — CƠ CHẾ TƯƠNG ĐƯƠNG & "CẮM ĐƯỜNG NÀO"

### 10.1 Bảng ánh xạ kiến trúc

| Bản Linux | JX1 (dự án) | Bằng chứng |
|---|---|---|
| `jx_linux_y` gọi `autoexec.lua : main()` (ELF `0x081DF9C3-0x081DF9DA`) | `CoreServerShell::OnLunch()` nạp `\script\startgame.lua` rồi `CallFunction("OnGame",0,"")` | `S\Core\Src\CoreServerShell.cpp:1127, **1134**` **[đã sửa theo đối chất]** (trước ghi 1133 — dòng 1133 chỉ là dấu `{`) |
| **(không có)** | `pTimeScript = \script\timerserver.lua`; `Breathe()` gọi `RunTime()` **1 lần/phút** (khi `GetGameTime()%GAME_FPS==0` và `wSecond==0`) | `CoreServerShell.cpp:1139, 1161-1173` |
| **S3Relay TaskCentre** (`TaskList.ini` + `TaskShedule/TaskContent`) | **không có tương đương 1-1** → thay bằng `timerserver.lua RunTime()` + các `*_DriverInit` | tiền lệ: `startgame.lua:104-108` (`WLLS_DriverInit`, `TONG_DriverInit`, `TONGWAR_DriverInit`, `BairenLeitai_Init`, `TONGCASTLE_DriverInit`) |
| `GlobalExecute("dw …")` / `("dwf <path> <func>")` | **ĐÃ CÓ SẴN, hành vi y hệt** | `S\Core\Src\KTongJX2.cpp:4056-4090` (`LuaJX2_GlobalExecute`) |
| — (relay ở tiến trình khác nên có độ trễ mạng) | JX1 hoãn **1 tick** để giữ đúng độ trễ gốc | `KJx2League.cpp:1280 KJx2DeferredExec_Push`, `:1298 …_Breathe`; gọi ở `CoreServerShell.cpp:1181` |
| `settings\timertask.txt` (TASK → SCRIPT) | `settings\TimerTask.txt`, cùng định dạng | `S\Core\Src\KCore.cpp:456 g_TimerTask.Init()`; `J\settings\TimerTask.txt` |
| `settings\task\missions.txt` (MISSION → SCRIPT) | `settings\task\missions.txt`, **tra theo SỐ DÒNG** | `J\script\missions\mission_trong.lua:1-4` (ghi chú `GetString(nMissionId+1,2) - KJx2League.cpp:936`) |
| `settings\systemtimetask.txt` | **không có** (và bản Linux cũng rỗng) | grep `S\Core\Src` = 0 hit |
| `OpenGlbMission` / nhịp global mission | `KJx2GlbMission_Breathe()` | `CoreServerShell.cpp:**1180**` **[đã sửa theo đối chất]** (trước ghi 1179 — dòng 1179 là `PERF_SCOPE(PERF_GLBMISSION)`) |

### 10.2 🔴 PHÁT HIỆN LỚN NHẤT — JX1 **ĐÃ CÓ SẴN CẢ 3 TÍNH NĂNG**, đang bị TẮT

Vòng 1 (`06_phia_jx1.md`) không nêu điều này. Cây `J\script\tinhnang\` có sẵn:

```
J\script\tinhnang\boss_satthu\   : lib_boss_st.lua (2080 B), death.lua, drop.lua
J\script\tinhnang\phonglangdo\   : lib_phonglangdo.lua (9995 B), thuyenphu.lua, thuyenphubac.lua,
                                   bossthuytacdaulinhpld.lua, quaipld.lua, dietrenthuyen.lua, drop.lua, logout.lua
J\script\tinhnang\vuot_ai\       : lib_vuotai.lua (8869 B), sugiasatthu.lua (13348 B),
                                   die_boss.lua, die_normal.lua, ondeath.lua, drop.lua
```

**Trạng thái bật/tắt hiện tại:**

| Tính năng | Điểm cắm | Trạng thái | Bằng chứng |
|---|---|---|---|
| Sát thủ | `OnGame()` | ❌ **TẮT** | `J\script\startgame.lua:99` `-- addnpcbosssatthu()` (bị comment); tệp `startgame\khac\satthu.lua` vẫn `Include` ở dòng 39 |
| Phong Lăng Độ | `OnGame()` | ✅ **NPC ĐANG BẬT** | `J\script\startgame.lua:100` `addnpcphonglangdo()` |
| Phong Lăng Độ | `RunTime()` (lịch) | ❌ **TẮT** | `J\script\timerserver.lua:80` `-- sukien_phonglangdo(nHr,nMi)` |
| Vượt ải | `RunTime()` (lịch) | ❌ **TẮT** | `J\script\timerserver.lua:79` `-- sukien_vuotai(nHr,nMi)` |

Hai hàm lịch **đã viết xong** và nằm ngay trong `timerserver.lua`:
- `sukien_vuotai(nHr,nMi)` — **dòng 874-901**
- `sukien_phonglangdo(nHr,nMi)` — **dòng 911-950**

**Hằng số lịch bản JX1:**

| Hằng | Giá trị | Vị trí |
|---|---|---|
| `TAB_TIME_VUOT_AI` | `{13,00} {15,00} {17,00} {19,00} {23,00}` (5 khung; `{9,10} {10,00} {12,00} {18,00}` bị comment) | `J\script\tinhnang\vuot_ai\lib_vuotai.lua:21-31` |
| `MAP_VUOTAI` | `480..489` (**10 map**, chỉ tầng cao cấp của bản Linux) | `lib_vuotai.lua:33-44` |
| `MSTIME_VUOT_AI_BD` / `_KT` / `_XOANPC` | 10 / 30 / 1 phút | `lib_vuotai.lua:12-14` |
| `TAB_TIME_PLD` | `0,2,4,6,8,10,12,14,16,18,20,22` giờ (**mỗi 2 giờ**; `1,17,19,21,23` bị comment) | `J\script\tinhnang\phonglangdo\lib_phonglangdo.lua:88-106` |
| `MAP_DUATHUYEN_PLD` | `337, 338, 339` (**trùng bản Linux**) | `lib_phonglangdo.lua:108-112` |
| `TIME_PLD_BD` | `9*60*18` khung = **9 phút báo danh** (Linux là 10) | `lib_phonglangdo.lua:7` |
| `MSTIME_PLD_KT` | `30*60*18` = 30 phút (Linux là 39) | `lib_phonglangdo.lua:8` |
| `TAB_TIME_TTDL_PLD` | `{20, 25, 30}` phút ra boss (Linux là 25/30/35) | `lib_phonglangdo.lua:114` |
| `check_new_shuizeitask()` | `[10] [14] [16] [18] [20] **[22]**` | `lib_phonglangdo.lua:130-144` |
| `MS_VUOTAI` / `MS_PLANGDO` | **3 / 4** (Linux: 22 / 15) | `J\script\lib\lib_task.lua:284-285` |
| `BOSS_SATTHU` | **20 boss** ở 5 map (321, 225, 93, 75, 340) — bản viết lại VN, **không phải** ~160 boss của Linux | `J\script\tinhnang\boss_satthu\lib_boss_st.lua:7-29`, hàm `addnpcbosssatthu()` dòng 34-42 |

**Đồng hồ mission JX1 hiện hành:** `J\settings\TimerTask.txt` dùng TASK id `1..10, 12..18, 20, 21, 50..55, 61, 62, 65..70, 75..77`.
Timer id `1` → `script\timertask\task01.lua`, có `OnMissionTimer()` (dòng 28-41) chuyển tiếp sang
`ontime_vuotai(i, MAP_VUOTAI[i])` (dòng 137-151) — dừng timer 1, mở timer 2 (30 phút), gọi `release_npcvuotai(1, nMap)`.

### 10.3 KHUYẾN NGHỊ CẮM (nếu quyết định port bản Linux, không dùng bản VN)

**Điểm cắm cho từng tính năng:**

1. **Sát thủ** → chỉ cần **boot**, không cần lịch:
   - Thêm 2 dòng vào `J\script\startgame.lua` phần `Include` + gọi trong `OnGame()`
     (đúng chỗ dòng 99-100 hiện có), theo mẫu `tinsu_addnpc()` ở dòng 114.
   - `nieshichen.lua` phải cắm sau khi Vượt ải sẵn sàng (NPC dùng chung).

2. **Phong Lăng Độ** → **lịch + boot**:
   - Boot: `addnpcphonglangdo()`-tương-đương trong `OnGame()`.
   - Lịch: **bỏ comment / viết mới `sukien_phonglangdo(nHr,nMi)`** gọi từ `RunTime()`
     (`timerserver.lua` dòng 80). Thân hàm chỉ cần:
     `GlobalExecute("dwf \\script\\missions\\fengling_ferry\\fldmap_boat1.lua fenglingdu_main()")`
     — **chạy được nguyên văn** nhờ `KTongJX2.cpp:4066-4083`.
   - Điều kiện: `RunTime()` chạy **mỗi phút**, nên chỉ cần `if nMi == 0 then …`.

3. **Vượt ải** → **lịch**:
   - `GlobalExecute("dwf \\settings\\trigger_challengeoftime.lua OnTrigger()")` + `dw AddLocalCountNews(...)`.
   - ⚠️ Nhớ **giữ dòng `--OnTrigger();` ở cuối `trigger_challengeoftime.lua` là comment** (§2.6).
   - Xếp hạng ngày: gọi lúc `nHr==0 and nMi==0` trong `RunTime()`; **quyết định nơi đặt ladder 10235** (§9.7).

**Bảng ID cần cấp phát trong JX1** (đã kiểm tra chỗ trống):

| Thứ | Id bản Linux | Tình trạng trong JX1 | Kết luận |
|---|---|---|---|
| MISSION Phong Lăng Độ | 15 | `J\settings\task\missions.txt:16` = `mission_trong.lua` (chỗ trống có dòng thật) | ✅ **dùng lại được id 15** |
| MISSION Vượt ải | 22 | `missions.txt:23` = `mission_trong.lua` | ✅ **dùng lại được id 22** |
| TIMER PLD | 28, 29 | `J\settings\TimerTask.txt` **không dùng** 28/29 | ✅ trống |
| TIMER Vượt ải | 41, 42, 43 | **không dùng** 41/42/43 | ✅ trống |
| ⚠️ Xung đột | — | JX1 **đang dùng** MISSION **3** (`mission03.lua` = vượt ải VN) và **4** (`mission04.lua` = PLD VN) | Nếu port bản Linux thì phải **gỡ hoặc chuyển** hai mission VN, nếu không **hai hệ cùng mở map 337/338/339 và 480..489** |

> 🔴 **Cảnh báo va chạm quan trọng nhất**: `mission03.lua:5` include `lib_vuotai.lua` và
> `mission04.lua:7` include `lib_phonglangdo.lua`, cả hai đều thao tác **đúng những map mà bản Linux
> cũng dùng** (337/338/339 và 480..489). Bật cả hai hệ cùng lúc ⇒ `OpenMission`/`CloseMission` đá nhau.
> **Phải chọn MỘT.**

---

## 11. BẢNG TỔNG HỢP — LỊCH CHẠY THẬT (bản Linux đang vận hành)

| Hoạt động | Nguồn kích hoạt | Chu kỳ thật | Bằng chứng runtime |
|---|---|---|---|
| **Sát thủ** | `autoexec.lua:151,158` (boot GameServer) | **không có lịch — thường trực 24/7** | không xuất hiện trong `WarTask_*.log` |
| **Phong Lăng Độ** — mở lượt | S3Relay `Task_86` → `fengling_ferry.lua:21` | **mỗi 60 phút, phút :00** (24 lượt/ngày) | log `07:00:52 执行定时任务：fengling_ferry.lua`; `TaskInterval=3600` |
| Phong Lăng Độ — giờ "Đại đầu lĩnh" | `fld_head.lua:305-311` | **10, 14, 16, 18, 20** giờ | (mã, không cần lịch riêng) |
| **Vượt ải** — mở báo danh | S3Relay `Task_52` → `challengeoftime.lua:34` | **mỗi 60 phút, phút :00** (24 lượt/ngày) | log `07:00:52 执行定时任务：challengeoftime.lua`; `TaskInterval=3600` |
| Vượt ải — vào trận | `mission_match.lua:12` timer 41 | **+10 phút** | |
| Vượt ải — hết giờ | `timer_close.lua` timer 43 | **+10+30 phút** | |
| Vượt ải — xếp hạng ngày | S3Relay `Task_84` | **00:00 hằng ngày** | log `TaskInterval=86400`, `TaskTime 0:0` |

---

## 12. CHƯA XÁC MINH ĐƯỢC (cần vòng sau)

1. ~~**Ngữ nghĩa tham số thứ 8 của `AddNpc`**~~ — **[đã sửa theo đối chất] ĐÃ TRA XONG**, xem §9.1 đã viết lại:
   cột **6** = `bNoRevive` (ELF `0x0811BDB2`), cột **7** = tên, cột **8** = nạp lại thuộc tính từ mẫu (`0x0811BEF2` → `call 0x08085250`),
   cột 9 = nSeries. Vì cột 6 = 0 nên **boss sát thủ CÓ hồi sinh** theo `ReviveFrame` của `npcs.txt`.
2. **`CHUANGGUAN30_START_TIME = 10` / `CHUANGGUAN30_END_TIME = 22`** (`challengeoftime\include.lua:29-30`):
   grep toàn cây A **và** B chỉ ra **đúng 2 dòng khai báo, KHÔNG có chỗ đọc nào**
   (`grep -rIn "CHUANGGUAN30_START_TIME\|CHUANGGUAN30_END_TIME" server1/script Patch/script` → chỉ include.lua:29,30).
   ⇒ **Ải 30 (map 957) hiện KHÔNG bị chặn theo giờ, mở 24/24.** Hai hằng là **mã chết**.
   Cần chủ game xác nhận: port sang JX1 thì **giữ mã chết** hay **hiện thực khung 10h-22h như ý định gốc**.
3. **Chu kỳ quét thật của S3Relay TaskCentre**: log cho thấy độ trễ ~52 giây so với phút :00. Chưa dịch ngược vòng
   `TaskCentre::Breathe` để biết là 1 phút hay 60 giây trôi từ lúc khởi động (ảnh hưởng nếu port muốn khớp chính xác).
4. **`GameSvrConnected` / `GameSvrReady`** (chuỗi ở `0x0019F082` / `0x0019F093`): cả 3 tệp lịch của ta đều để **thân rỗng**.
   **[đã sửa theo đối chất]** Đã tìm được **ví dụ có thân hàm thật** trong cùng thư mục relay:
   `relaysetting\task\ladderload.lua:3-5` `function GameSvrConnected(dwGameSvrIP) SyncAllLadder(dwGameSvrIP) end`.
   ⇒ Callback này **được gọi mỗi khi một GameServer nối lại**, và mục đích là **đẩy lại trạng thái** xuống GS đó.
   ⇒ Vì `challengeoftime.lua` / `challegeoftime-dailyrank.lua` / `fengling_ferry.lua` đều **bỏ trống** hai callback,
   **GameServer restart giữa giờ thì mission 15 / 22 KHÔNG được khôi phục** — người chơi phải đợi lượt kế.
   (Còn lại chưa xác minh: relay gọi `GameSvrReady` ở thời điểm nào so với `GameSvrConnected`.)
5. **`RelaySetting\Task\` viết ngược `ExcutedCount` vào `tasklist.ini`?** Chuỗi `ExcutedCount` có mặt, nhưng chưa xác
   minh relay có **ghi đè tệp ini** khi task đạt giới hạn. Nếu có, thì file cấu hình là **tệp trạng thái**, không phải
   thuần cấu hình — điều này đổi cách port.
6. ~~`SHOUHUZHE_OFFSET`~~ — **ĐÃ TRA XONG**: `killbosshead.lua:1` khai báo `= 2`, dùng ở
   `killbosshead.lua:2922-2930` (9 dòng) để dịch toạ độ X của 9 NPC **"Bảo Khố Thủ Hộ Giả"** (npc 849, map 395)
   ra **2 đơn vị** so với 9 "Bảo rương" (npc 844, `killbosshead.lua:3380-3388`) đứng cùng vị trí.
   ⇒ Đây là mẹo **tránh chồng NPC lên nhau**; port sang JX1 phải giữ nguyên độ lệch này.
7. ~~**Bản B (`Patch`) có ghi đè `relaysetting` không?**~~ — **[đã sửa theo đối chất] KHÔNG, và không thể**:
   `D:\ServerLinux\Patch` là **cây CLIENT** (`game_y.exe`, `engine.dll`, `represent2.dll`, `spr\`, `ui\`, `music\`,
   `vauto.exe`, `versionlist.ini`), **không có thư mục `gateway\` cũng không có `relaysetting\`**;
   `Patch\script\` chỉ có `activitysys, event, global, item, skill, task, tong, ui, vng_event` — **không có `missions\`**.
   Đã xác nhận `timertask.txt`, `systemtimetask.txt`, `trigger*.lua`, `killer.txt`, `bosstask_lev*.ini` là **A ≡ B**.
   Còn lại chưa quét: 845 dòng trong `chi_co_o_patch.txt` (xem §"Bỏ sót của chính vòng 2" mục B2 — đã tìm ra
   một tệp CHẶN của Phong Lăng Độ nằm trong nhóm này).
8. **Item `2745` (Lệnh Bài Thủy Tặc)** và `2743`, `2024` (Truy Công Lệnh), `489` (event item PLD), `2346` (Thí Giả Yêu Bài),
   `400` (Sát thủ giản), `195` (Mật đồ thần bí) — đã bắt được id nhưng chưa đối chiếu với `item_can_them.csv` của vòng 1.

---

## 13. DANH SÁCH TỆP CẦN ĐỌC KHI THI CÔNG (đường dẫn tuyệt đối)

**Lịch (bản Linux) — chưa có trong 6 báo cáo vòng 1:**
```
D:\ServerLinux\gateway\s3relay\relaysetting\task\tasklist.ini
D:\ServerLinux\gateway\s3relay\relaysetting\task\fengling_ferry.lua
D:\ServerLinux\gateway\s3relay\relaysetting\task\challengeoftime.lua
D:\ServerLinux\gateway\s3relay\relaysetting\task\challegeoftime-dailyrank.lua
D:\ServerLinux\gateway\s3relay\relaysetting\task\new\tasklist.ini          (bản mẫu, chú thích định dạng)
D:\ServerLinux\gateway\s3relay\settings\worldset.txt                       (map -> GameServer)
D:\ServerLinux\gateway\s3relay\RelayRunData\WarTask_20260806.log           (bằng chứng chạy thật)
D:\ServerLinux\server1\settings\trigger_challengeoftime.lua
D:\ServerLinux\server1\settings\trigger_include.lua
D:\ServerLinux\server1\settings\timertask.txt
D:\ServerLinux\server1\settings\task\missions.txt
```

**Boot (bản Linux):**
```
D:\ServerLinux\server1\script\global\autoexec.lua
D:\ServerLinux\server1\script\global\autoexec_npc.lua
D:\ServerLinux\server1\script\global\autoexec_head.lua
D:\ServerLinux\server1\script\task\tollgate\killbosshead.lua
D:\ServerLinux\server1\script\gmscript\gmcommand_3.lua
```

**Phía JX1 (đọc TRƯỚC khi cắm):**
```
D:\GAMEDEVNEW\Sources\Core\Src\CoreServerShell.cpp            (1122-1185)
D:\GAMEDEVNEW\Sources\Core\Src\KTongJX2.cpp                   (4044-4090  GlobalExecute dw/dwf)
D:\GAMEDEVNEW\Sources\Core\Src\KJx2League.cpp                 (1278-1310  DeferredExec)
E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\startgame.lua                (9,11,39,99,100,104-108)
E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\timerserver.lua              (15,16,79,80,874-950)
E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\timertask\task01.lua         (28-41,137-151)
E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\tinhnang\vuot_ai\lib_vuotai.lua
E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\tinhnang\phonglangdo\lib_phonglangdo.lua
E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\tinhnang\boss_satthu\lib_boss_st.lua
E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\TimerTask.txt
E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\task\missions.txt
E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\missions\mission_trong.lua   (luật tra theo SỐ DÒNG)
```

---

## ĐỐI CHẤT (tác tử độc lập)

> Người kiểm chứng: một tác tử KHÁC, **không** viết báo cáo này. Nguyên tắc: **mặc định coi mọi khẳng định là SAI
> cho tới khi mở đúng tệp gốc / dịch ngược đúng địa chỉ ELF chứng minh ngược lại.**
> Đã kiểm **34 khẳng định** → **6 SAI** (đã sửa trong thân bài, đánh dấu `[đã sửa theo đối chất]`), **28 ĐÚNG**.
> Thêm **8 chỗ chính báo cáo này còn sót**.

### A. Bảng đối chất

| # | Khẳng định | Bằng chứng gốc (đã tự mở/dịch ngược) | KẾT LUẬN | Sửa lại thành |
|---|---|---|---|---|
| 1 | ELF `0x081DF9C3-0x081DF9DA` push `0x8269bac` (`\script\global\autoexec.lua`) + `0x826dbc2` (`main`) rồi `call 0x80f90e0` | `re_disasm jx_linux_y 0x081DF9A0` cho **đúng từng byte** 4 lệnh; `--find` trả chuỗi ở độ lệch `0x221BAD` (bỏ dấu `\` đầu) ⇒ chuỗi đầy đủ ở `0x221BAC` = VA `0x08269BAC` | **ĐÚNG** | — |
| 2 | Ngay trước đó `0x081DF9AF` nạp `\settings\citywar.ini` VA `0x08269B96` | `--find "settings\citywar.ini"` → `0x221B97`, trừ dấu `\` = `0x221B96` = VA `0x08269B96`; lệnh tại `0x081DF9AF` push đúng `0x8269b96` | **ĐÚNG** | — |
| 3 | `autoexec.lua` dòng 3, 4, 28, 151, 158, 159, 204-206, 208-209 | grep + đọc `server1\script\global\autoexec.lua` — khớp **cả 9 dòng** (`:204 G_ACTIVITY:LoadActivitys()`, `:205 G_TASK:LoadAllConfig()`, `:206 OnMessage("ServerStart")`) | **ĐÚNG** | — |
| 4 | `autoexec_npc.lua:26-32` = 7 dòng NPC **769** ở 7 thành, cùng script `killer\nieshichen.lua` | grep `769` trả **đúng 7 dòng 26..32**, map `11, 1, 37, 176, 162, 78, 80` | **ĐÚNG** | — |
| 5 | `nieshichen.lua` dòng 5, 11, 19, 54; mục thoại 22-29 = sát thủ 20..90; 24/25 = "Thử luyện"/"Tham gia khiêu chiến"; `:17` giới hạn 8 lượt bị comment; `:48` thoại "2 lần/ngày, báo danh 5 phút" | đọc giải mã GBK toàn bộ 60 dòng đầu — **khớp từng dòng một** | **ĐÚNG** | — |
| 6 | `killbosshead.lua` 3421 dòng; `SHOUHUZHE_OFFSET = 2` dòng 1; `add_killertasknpc` 183-194 (`AddNpc` 189, `SetNpcParam` 191); `add_bossnpc` 3393-3404; `add_messengernpc` 3406-3414; 9 "Bảo Khố Thủ Hộ Giả" (npc 849, map 395) dòng 2922-2930 lệch X **+2** so với 9 "Bảo rương" (npc 844) dòng 3380-3388 | grep + đọc 2 vùng: X của 844 = `1415,1437,1409,1377,1409,1378,1420,1375,1419`, của 849 = **đúng các số đó `+SHOUHUZHE_OFFSET`**, Y trùng khít | **ĐÚNG** | — |
| 7 | §1.4 đính chính brief: 8 tệp `bosstask_lev20..90.ini` **và** `killer.txt` có ở A, **byte-identical** với B, và **không** nằm trong `chi_co_o_patch.txt` | `cmp` cả 8 cặp = A==B; `killer.txt` 14 033 B A==B; `grep -c bosstask_lev chi_co_o_patch.txt` = **0**, `grep -ci killer.txt` = **0** | **ĐÚNG** | (ghi chú: `chi_co_o_patch.txt` có **845** dòng, brief nói 846) |
| 8 | 12 chuỗi/độ lệch trong `s3relay_y` (§2.1) | `--find` từng chuỗi: `TaskCentre.cpp` `0x19EFF2` − 34 byte tiền tố `./swordonline/multiserver/S3Relay/` = **`0x19EFD0`** ✔; `TaskList.ini 0x19EF96` ✔; `Task_%d 0x19EFAE` ✔; `TaskFile 0x19EFC4` ✔; `ExcutedCount 0x19EFB6` ✔; `TaskShedule 0x19F045` ✔; `TaskContent 0x19F074` ✔; `GameSvrConnected 0x19F082` ✔; `GameSvrReady 0x19F093` ✔; `\RelaySetting\Task\ 0x19EF82` ✔; `\RelayRunData\WarTask 0x19EF6C` ✔; `TaskCountLimit 0x19ABA4` nằm trong dải `0x19AB85..0x19ABD3` ✔ | **ĐÚNG (12/12)** | — |
| 9 | `tasklist.ini`: `Count=128`, `[Task_52]` dòng 208-209, `[Task_84]` 311-312, `[Task_86]` 317-318, cả ba **không có** `ExcutedCount` | đọc trực tiếp; `grep -c "^\[Task_"` = **128** | **ĐÚNG** | — |
| 10 | Không có mục Sát thủ trong `tasklist.ini`; `pinganjijie.lua:69` gọi `killbossmatch` và tệp đó không đăng ký | grep `challeng\|fengling\|killer\|tollgate\|killboss` trong `tasklist.ini` chỉ ra **4 dòng** (209, 254 `citywar_clearchallenger`, 312, 318); `pinganjijie.lua:69` đúng nguyên văn | **ĐÚNG** | (bổ sung: `pinganjijie.lua:55` còn chặn `nDate < 091211 or nDate > 100124` ⇒ **chết hai lần**) |
| 11 | `fengling_ferry.lua` **27 dòng**; `TaskTime(nStartHour,0)` dòng 10; `TaskInterval(60)` dòng 13; `TaskCountLimit(0)` dòng 16; `GlobalExecute("dwf ... fenglingdu_main()")` dòng 21 | đọc toàn văn: tệp đúng 27 dòng, số dòng khớp **tuyệt đối** | **ĐÚNG** | — |
| 12 | `challengeoftime.lua`: `INTERVAL_TIME=60` dòng 3, `GetNextTime` 6-14, `TaskInterval` 20, `TaskTime(h,m)` 22-23, `TaskCountLimit(0)` 26, `dwf` 34, `dw AddLocalCountNews` 36 | đọc toàn văn — khớp **từng dòng** | **ĐÚNG** | (thân bài bỏ sót dòng 24 `OutputMsg(format(...))`) |
| 13 | `challegeoftime-dailyrank.lua`: `TaskTime(0,0)` dòng 7, `TaskInterval(1440)` dòng 10, `Ladder_GetLadderInfo(10235,1)` dòng 18, `Ladder_ClearLadder(10235)` dòng 26 | đọc toàn văn 35 dòng — khớp cả 4 | **ĐÚNG** | — |
| 14 | `trigger_challengeoftime.lua` 704 B, A≡B, dòng 24 `--OnTrigger();` **bị comment**; `trigger_dragonboat.lua:14` **không** comment; `trigger.lua` thiếu bảo toàn `SubWorld` | `stat` = 704; `cmp` A/B = 0; đọc cả 3 tệp — `trigger_dragonboat.lua` đúng 14 dòng, dòng 14 = `OnTrigger();`; `trigger.lua` (759 B, 38 dòng) **không có** biến `oldSubWorld` | **ĐÚNG** | — |
| 15 | `trigger_include.lua` 828 B, bảo toàn `SubWorld` ở dòng 10/18/23/33; bỏ qua **im lặng** khi `SubWorldID2Idx < 0` ở dòng 13 | đọc toàn văn 34 dòng — `local oldSubWorld = SubWorld;` ở 10 và 23, khôi phục ở 18 và 33, guard `if (index >= 0)` ở 13 và 26 | **ĐÚNG** | — |
| 16 | `settings\systemtimetask.txt` = **24 byte**, chỉ dòng tiêu đề, A≡B; engine có đọc (chuỗi `0x002098D8`) | `xxd` = `HOUR\tMIN\tSCRIPT\td\tDESC\r\n` đúng 24 byte; `cmp` A/B = 0; `--find "systemtimetask"` → `0x2098E2`, trừ 10 ký tự `\settings\` = **`0x2098D8`** | **ĐÚNG** | — |
| 17 | `settings\killer.ini` là hệ **truy nã PvP**, không dính Sát thủ; chuỗi ELF `0x00209D3D` | đọc: dòng 1 `;通缉追杀系统专用ini`, `MoneyPerHour=10000`, `MinTargetLevel=50`, `MaxActiveTaskTime=10`, `MinReward=100000`, thông điệp `TargetLevelTooLow=…`; `--find` → `0x209D3E` − dấu `\` = `0x209D3D` | **ĐÚNG** | — |
| 18 | `challengeoftime\include.lua`: MISSION 22, TIMER 41/42/43, báo danh 10', trận 30', ải 30 = map **957**, `CHUANGGUAN30_START/END_TIME` ở dòng **29-30**, `COUNT_LIMIT = 1` dòng **32**, `tbLevelMaps` dòng **90-103** = 464..479 và 480..495 | đọc giải mã — **khớp mọi số dòng và mọi giá trị** | **ĐÚNG** | — |
| 19 | 🔴 `worldset.txt` **thiếu hẳn map 472-479** ⇒ tầng Sơ cấp chỉ 8 phòng | tự phân tích bảng: bản relay có **696** dòng map, `464..471` có, **472..479 không có dòng nào**, `480..495` đủ 16 | **ĐÚNG — và MẠNH HƠN** | Bản của **GameServer** (`server1\settings\worldset.txt`, md5 `8d0f6034…`, **694** dòng, cột GAMESVR khác hẳn relay md5 `dcbd7d32…`) **cũng thiếu đúng 472..479**. Kết luận 8 phòng đứng vững trên **cả hai** tệp, không phải chỉ tệp relay |
| 20 | `maplist.ini:3690/3714/3749` = `464/472/479 = 特殊用地\杀手的试炼` | đọc byte GBK 3 dòng đó — đúng chuỗi đó, y hệt nhau | **ĐÚNG** | — |
| 21 | `fld_head.lua`: MISSION 15 (dòng 10), map 337/338/339 (dòng 12), `FLD_TIMER_1`=20s / `FLD_TIMER_2`=39' (19-20), báo danh 10' (21), boss 25/30/35' (22-24), `check_new_shuizeitask` 302-317 với `tb_sptime` 305-311 = **10,14,16,18,20** | đọc bản giải mã UTF-8 giữ số dòng — khớp **tuyệt đối** cả 8 mốc | **ĐÚNG** | — |
| 22 | Bản relay dự phòng `phonglangdo\fengling_ferry_0000.lua:12` in `10h 14h 16h 18h 20h **22h**` | đọc: dòng 12 = `"      10h:00 - 14h:00 - 16h:00 - 18h:00 - 20h:00 - 22h:00 "` | **ĐÚNG** | — |
| 23 | `timertask.txt:29,30` → TIMER 28, 29; `:42,43,44` → TIMER 41, 42, 43; `missions.txt:16` → MISSION 15, `:23` → MISSION 22; A≡B | in đúng các dòng: 29→`28 fld_landingtimer.lua`, 30→`29 fld_smalltimer.lua`, 42→`41 timer_match.lua`, 43→`42 timer_board.lua`, 44→`43 timer_close.lua`, missions 16→`15 fengling_ferry\mission.lua`, 23→`22 challengeoftime\mission_match.lua`; `cmp` A/B cả 2 tệp = 0 | **ĐÚNG** | — |
| 24 | Bản Linux **không có** `timerserver*` / `timertask*` phía GameServer | `find server1/script Patch/script -iname "timerserver*" -o -iname "timertask*"` = **0** | **ĐÚNG** | — |
| 25 | 5 binary gateway quét `trigger\|OnTrigger\|timertask\|systemtimetask\|challengeoftime\|fengling` = 0 hit | `grep -a -c` trên `bishop_y`, `goddess_y`, `BishopConn`, `KG_SyncD`, `libjxinfo.so` → **0 / 0 / 0 / 0 / 0** | **ĐÚNG** | — |
| 26 | 12 tệp `fengling_ferry_HHMM.lua` + 12 tệp `challengeoftime_HHMM.lua`, **không tệp nào** đăng ký trong `tasklist.ini` | `ls` = 12 + 12 đúng; grep `tasklist.ini` chỉ trúng bản gốc | **ĐÚNG** | (thiếu liệt kê **`challengeoftime_ok.lua`** — biến thể thứ 13, cũng không đăng ký) |
| 27 | GM: `GMC_FLD` 818-827, `FLD_BaoMingNpc` 829-852 (map 336, 3 toạ độ), `FLD_Start` 854-857 gọi `main()`, `FLD_AddItem` 859-861, `FLD_End` 863-865; dòng 822 và 823 **cùng nhãn** `开启风陵渡`; `fldmap_boat1.lua:4` là `fenglingdu_main()` chứ không phải `main()`; `KillBoss_TaskNpc` 458-460 thiếu `LoadMap(11)` trong khi `COT_SignUpNpc` 269-272 có | đọc giải mã `gmcommand_3.lua` (977 dòng) đúng 3 vùng; `grep "function main"` trong `fengling_ferry\` cho thấy **chỉ** `fldmap_boat2.lua:2` và `fldmap_boat3.lua:2` có `main()` | **ĐÚNG (cả 4 lỗi GM)** | Bổ sung ứng viên `main()` bị gọi nhầm — xem mục B7 |
| 28 | Log `WarTask_20260806.log`: 128 việc, nạp 06:45:52, Task 52/84/86 đúng tên, **nổ cùng 07:00:52**, Sát thủ **0 lần** | tự tách bản ghi (tệp **không có** ký tự xuống dòng — 0 CR, 0 LF; phải tách theo dấu thời gian): 736 bản ghi, `定时任务数量：128`, 3 việc đúng tên, 2 dòng `执行定时任务` lúc `07:00:52`; đếm `killer/tollgate/killboss/nieshichen/satthu/bosstask` = **0/0/0/0/0/0** | **ĐÚNG** | — |
| 29 | Log ⇒ "**Task_2..Task_49 bị BỎ QUA hoàn toàn** (đúng các mục có `ExcutedCount=-1`)" | tự liệt kê id trong log: **chỉ 65/128** việc được nạp. Bị bỏ **63**: `2..49` **cộng** `56,58,61,66,68,82,83,108,110,113,120,121,123,124,125` — trùng khít 63 mục `ExcutedCount=-1` (`grep -c` = 63) | 🔴 **SAI** | "**63** việc bị bỏ qua = Task_2..49 **+ 15 mục rải rác** (56,58,61,66,68,82,83,108,110,113,120,121,123,124,125); tập này trùng **100%** với `ExcutedCount=-1`" |
| 30 | `CoreServerShell.cpp:1127, **1133**` = `startgame.lua` → `OnGame` | `grep -n`: `:1127` `g_GetScript("\\script\\startgame.lua")` ✔; `CallFunction("OnGame",0,"")` ở **`:1134`** (dòng 1133 chỉ là `{`) | 🔴 **SAI (lệch 1 dòng)** | `CoreServerShell.cpp:1127, **1134**` |
| 31 | `KJx2GlbMission_Breathe()` ở `CoreServerShell.cpp:**1179**` | `grep -n` → **`:1180`** (1179 là `PERF_SCOPE(PERF_GLBMISSION)`) | 🔴 **SAI (lệch 1 dòng)** | `CoreServerShell.cpp:**1180**` |
| 32 | `:1139` timerserver.lua · `:1161-1173` `RunTime` 1 lần/phút · `KJx2League.cpp:1280` Push, `:1298` Breathe · `KTongJX2.cpp` GlobalExecute hiểu `dw`/`dwf`, `dw` hạ cánh ở `\script\gmscript.lua` | `grep -n` = 1139 ✔, 1170 nằm trong khối 1161-1176 ✔, 1280 ✔, 1298 ✔; đọc `KTongJX2.cpp:4055-4091`: `dw` ở 4064-4069, `dwf` ở 4070-4086; `KJx2League.cpp:1285` mặc định path = `"\\script\\gmscript.lua"` | **ĐÚNG** | — |
| 33 | §10.2 JX1 **đã có sẵn cả 3 tính năng, đang TẮT**: `startgame.lua:99` `-- addnpcbosssatthu()`, `:100` `addnpcphonglangdo()`, `:39` Include `khac\satthu.lua`; `timerserver.lua:79/80` comment, hàm `sukien_vuotai` **874-901**, `sukien_phonglangdo` **911-950**; `TAB_TIME_VUOT_AI` 13/15/17/19/23h (`lib_vuotai.lua:21-31`), `MAP_VUOTAI` 480..489 (`:33-44`), 10/30/1' (`:12-14`); `TAB_TIME_PLD` mỗi 2 giờ (`lib_phonglangdo.lua:88-106`), map 337/338/339 (`:108-112`), `TIME_PLD_BD` 9' (`:7`), `MSTIME_PLD_KT` 30' (`:8`), `TAB_TIME_TTDL_PLD {20,25,30}` (`:114`), `check_new_shuizeitask` có `[22]` (`:138`); `MS_VUOTAI=3` / `MS_PLANGDO=4` (`lib_task.lua:284-285`); `BOSS_SATTHU` **20 boss** 5 map 321/225/93/75/340 (`lib_boss_st.lua:7-29`, hàm 34-42); cỡ tệp 2080/9995/8869/13348 B | mở **từng tệp trên máy chủ JX1 đang chạy** và đọc đúng các dòng đó — **khớp 100%, không sai một số dòng nào** | **ĐÚNG (khối lớn nhất, 20+ mốc)** | — |
| 34 | §10.3: `missions.txt:16` và `:23` của JX1 = `mission_trong.lua` (trống); `TimerTask.txt` **không dùng** 28/29/41/42/43 (đang dùng 1..10, 12..18, 20, 21, 50..55, 61, 62, 65..70, 75..77); va chạm MISSION **3**/**4** với `mission03.lua:5` include `lib_vuotai.lua`, `mission04.lua:7` include `lib_phonglangdo.lua` | in toàn bộ 2 bảng của JX1 — id đang dùng **đúng y hệt** danh sách báo cáo; đọc `mission03.lua`/`mission04.lua` — 2 dòng Include đúng số dòng | **ĐÚNG** | — |
| 35 | §9.1 "**CHƯA XÁC MINH** ý nghĩa tham số thứ 8 của `AddNpc` ⇒ boss sát thủ có tự hồi sinh không" | Đã dịch ngược `AddNpc` = `0x0811BB10` (luamap dòng 667). Cột **6** → `0x0811BDB2 setne byte ptr [KNpc+0x1824]` = `bNoRevive`; cột **7** → tên; cột **8** `==1` → `0x0811BEF2 call 0x08085250` + `[KNpc+0x181C]=2`; cột 9 = nSeries (mặc định −1). Ngoài ra `ScriptFuns.cpp:6846-6873` của chính dự án **đã ghi sẵn** ngữ nghĩa `+0x1824`/`+0x181C` này | 🔴 **SAI (xác minh được, và hỏi nhầm cột)** | "Cột **6** là cờ hồi sinh, **= 0** ⇒ **boss CÓ hồi sinh** theo `ReviveFrame` của `npcs.txt`. Cột 8 chỉ nạp lại thuộc tính từ mẫu." |
| 36 | §9.7/§10.3 "JX1 chỉ có 1 GameServer ⇒ **phải quyết định đưa ladder 10235 về đâu**" | `ScriptFuns.cpp:15086-15087` **đã đăng ký** `Ladder_ClearLadder` / `Ladder_GetLadderInfo`; hiện thực thật (có ghi đĩa) ở `KJx2SharedStore.cpp:580` và `:599`; chốt id `JX2LADDER_MIN_ID = 10000` (`:441`) < **10235**, `JX2LADDER_TOP = 10` (`:440`) | 🔴 **SAI (kết luận thừa)** | "JX1 **đã có sẵn** ladder in-process; `Ladder_*(10235)` chạy nguyên văn. Việc cần làm là **port `ladderload.lua`** (nạp 10001..10300), xem B1." |
| 37 | Đầu tệp: "**B** = `D:\ServerLinux\Patch` (**lớp cập nhật**)"; §12.7 "chưa rõ B có ghi đè `relaysetting` không" | `ls Patch/` → `game_y.exe`, `engine.dll`, `represent2.dll`, `spr\`, `ui\`, `music\`, `vauto.exe`, `KAuto.dat`, `versionlist.ini`, `package.ini` = **cây CLIENT**. `Patch/script/` chỉ có `activitysys,event,global,item,skill,task,tong,ui,vng_event` — **không có `missions\`**; **không có `gateway\` / `relaysetting\`** | 🔴 **SAI (mô tả nguồn)** | "B là **cây CLIENT** (bản vá client + auto), **không thể** ghi đè lịch relay. §12.7 **đã có câu trả lời**." |

### B. Bỏ sót của chính vòng 2

**B1. 🔴 Thiếu hẳn `Task_50 ladderload.lua` — mắt xích BẮT BUỘC của bảng xếp hạng Vượt ải.**
Báo cáo tìm ra `Task_84` xài `Ladder_GetLadderInfo(10235,1)` nhưng **không tìm nơi ladder được tạo/nạp**.
`gateway\s3relay\relaysetting\task\ladderload.lua` (đang **BẬT**, `[Task_50]` dòng 202-203, có trong log 06:45:52):
```lua
function GameSvrConnected(dwGameSvrIP)   -- dong 3
    SyncAllLadder(dwGameSvrIP)           -- dong 4
end;
function TaskShedule()
    TaskName( "Relay Bảng xếp hạng" );   -- dong 11
    TaskInterval( 2440 );                -- dong 13
    TaskCountLimit( 0 );                 -- dong 15
    for i=10001, 10300 do LoadLadder(i) end   -- dong 18-20  <== NAP CA 10235
end
```
Hai hệ quả:
1. Port kiểu relay mà quên việc này ⇒ ladder 10235 **không có kho chứa**, `Task_84` chạy vào chỗ rỗng.
2. Đây là **ví dụ duy nhất có thân hàm** của `GameSvrConnected` trong cả thư mục ⇒ **trả lời được §12.4**:
   callback này chạy khi một GameServer nối lại, dùng để **đẩy lại trạng thái**. Vì 3 tệp lịch của ta để **rỗng**,
   **GameServer restart giữa giờ thì MISSION 15/22 KHÔNG được khôi phục**.

**B2. 🔴 Bảng sinh Thủy tặc của Phong Lăng Độ KHÔNG CÓ trong cây máy chủ — chặn tiến độ.**
`fld_head.lua:17-18` khai `TNPC_THIEF_COUNT = 30` và
`npcthiefpos = "\\settings\\maps\\中原北区\\渡船\\渡船刷怪点.txt"`.
Đối chiếu thật (duyệt cây rồi **giải mã tên GBK-đọc-nhầm-CP1258**, không dùng chuỗi Unicode):

| Cây | `settings\maps\` có bao nhiêu mục | 4 thư mục vùng tiếng Trung |
|---|---|---|
| `server1` (máy chủ chạy) | 29 | **KHÔNG CÓ mục nào** |
| `Patch` (client) | 33 | `东北区`, `江南区`, `西北北区`, `中原北区` |

`渡船刷怪点.txt` (893 B) chỉ tồn tại ở `Patch\settings\maps\中原北区\渡船\`.
⇒ Theo đúng cây đang có, script máy chủ **không đọc được bảng điểm sinh 30 thủy tặc**.
Báo cáo mô tả trọn chuỗi khởi động PLD (timer 28/29, boss phút 25/30/35) nhưng **không nêu** lỗ hổng dữ liệu này.
Cùng nhóm: `风陵渡北岸\baiyingyingboss.txt`, `yanxiaoqianboss.txt`, `风陵渡南岸\herenwoboss.txt`.

**B3. 🔴 Lệch ngữ nghĩa cột 6 của `AddNpc` khi port sang JX1 (không phải cột 8).**
Bản Linux: cột 6 = `bNoRevive` (ELF `0x0811BDB2`). JX1 `LuaAddNpc` (`ScriptFuns.cpp:6834`): **cột 6 = `nSeries`** (ngũ hành),
cột 7 = tên (`:6836-6841`), **cột 8 bị bỏ hẳn**. Với `killbosshead.lua` thì vô hại vì cột 6 = 0, nhưng đây là **bẫy hệ thống**:
mọi bảng NPC bản Linux có cột 6 ≠ 0 sẽ bị JX1 hiểu thành ngũ hành. Dự án đã vấp đúng chỗ này và phải viết
`LuaAddNpcEx` riêng (`ScriptFuns.cpp:6874-6946`). §10 của báo cáo không cảnh báo.

**B4. `challengeoftime_ok.lua` — biến thể thứ 13 bị bỏ khỏi §6.5.** Thư mục có 13 tệp `fengling_ferry*` (1 gốc + 12 HHMM)
và **14** tệp `challengeoftime*` (1 gốc + 12 HHMM + `_ok`). Kết luận "không tệp nào đăng ký" vẫn đúng, nhưng danh sách thiếu.
Ngoài ra thư mục `relaysetting\task\phonglangdo\` có **25** tệp (gốc + `_0000`..`_2300` từng giờ), `vuotai\` chỉ có **2**.

**B5. Chỉ kiểm 1 trong 9 tệp `worldset`.** Có `worldset.txt`, `.preset1`, `worldset0.txt`, `worldset1.txt`,
`worldset_ngan.txt`, `worldset_trip_server.txt` bên relay và `worldset.txt`, `.preset1`, `worldset_trip_server.txt`
bên `server1`. Hai bản `worldset.txt` **khác md5 và khác số dòng** (696 vs 694). Kết luận §5.1 may mắn đúng ở cả hai,
nhưng §13 chỉ liệt kê tệp relay ⇒ người thi công dễ kiểm nhầm cây.

**B6. `pinganjijie.lua` chết **hai** lần, báo cáo chỉ nêu một.** Ngoài "không có trong `tasklist.ini`" (§6.3),
`pinganjijie.lua:55` còn `if nDate < 091211 or nDate > 100124 then return end` ⇒ dù có đăng ký cũng **không bao giờ chạy**
(cửa sổ 11/12/2009 – 24/01/2010).

**B7. §9.2 đoán thiếu ứng viên `main()`.** Báo cáo đoán `main()` bị gọi nhầm "rất có thể là `gmscript\head.lua` hoặc
`autoexec.lua`". Thực tế trong **chính thư mục Phong Lăng Độ** có 2 định nghĩa: `fldmap_boat2.lua:2` và
`fldmap_boat3.lua:2` đều `function main()` (mở MISSION 15 cho bến 2 / bến 3, `SetMissionV(MS_STATE, 2)`).
Nếu state GM đã `Include` một trong hai tệp đó trước, `FLD_Start()` sẽ mở **sai bến** thay vì không làm gì.
Ứng viên tệ nhất vẫn là `autoexec.lua:126 function main()` = **chạy lại toàn bộ boot server**.

**B8. Đầu vào của Phong Lăng Độ ở bản Linux không có điểm dựng NPC báo danh.**
Grep toàn cây: **không script nào** tạo "船夫" (thuyền phu) cho map 336 — `gmcommand_3.lua:829-852` chỉ **dịch chuyển**
GM tới 3 toạ độ, không `AddNpc`. `autoexec_npc.lua` không có dòng nào trỏ tới `fengling_ferry`.
⇒ NPC báo danh PLD bản Linux là **NPC tĩnh của dữ liệu map**, không phải script boot.
Đây là điểm §1.2 ("không có dòng nào trong `autoexec.lua` khởi động Phong Lăng Độ") bỏ ngỏ mà **bên JX1 bắt buộc phải bù**
— và JX1 đã bù sẵn bằng `addnpcphonglangdo()` (`lib_phonglangdo.lua:147`).
