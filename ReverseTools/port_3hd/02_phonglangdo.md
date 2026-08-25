# PHONG LĂNG ĐỘ (fengling_ferry) — DỊCH NGƯỢC ĐẦY ĐỦ TỪ BẢN LINUX

Nguồn: `D:\ServerLinux\server1` (JX2/Kiếm Thế Linux, Lua 4.0, script trộn GBK + TCVN3).
Mọi khẳng định dưới đây đều kèm `đường-dẫn:dòng` hoặc địa chỉ ELF.
Dấu 🔴 = bẫy / lỗi thật cần chú ý khi port. "CHƯA XÁC MINH" = chưa có bằng chứng trực tiếp.

---

## 0. TÓM TẮT KIẾN TRÚC

Phong Lăng Độ là **Mission id 15** (`settings\task\missions.txt:16` → `\script\missions\fengling_ferry\mission.lua`),
chạy **đồng thời trên 3 bản đồ thuyền 337/338/339**, mỗi bản đồ là **một thể hiện (instance) riêng** của mission 15.

Ba giai đoạn:
1. **Báo danh 10 phút** trên bờ Nam — người chơi nói chuyện NPC Thuyền phu, nộp lệnh bài → bị `NewWorld` sang bản đồ thuyền.
2. **Hải trình 29 phút** trên bản đồ thuyền — 30 Thủy tặc + 3 đợt Boss, PK bật.
3. **Cập bến** — toàn bộ người còn sống bị `NewWorld` sang bản đồ 336 (bờ Bắc Phong Lăng Độ = bản đồ đánh bảo).

🔴 **"Thuyền" KHÔNG di chuyển.** Không có bất kỳ lệnh dịch chuyển bản đồ/vật thể nào. "Đi thuyền" hoàn toàn là
**hiệu ứng kể chuyện**: người chơi đứng yên trên bản đồ 337/338/339 trong 39 phút, hết giờ thì bị teleport sang 336.

---

## 1. LUỒNG HOẠT ĐỘNG THEO THỜI GIAN

### 1.1 Hằng số thời gian (`fld_head.lua:10-30`)

| Hằng số | Biểu thức | Giá trị (frame) | Quy ra giây/phút | Ý nghĩa (chú thích gốc tiếng Trung) |
|---|---|---|---|---|
| `MISSIONID` | `15` | — | — | `--未定` (chưa định) |
| `FRAME2TIME` | `18` | — | 18 frame = 1 giây | `--18帧游戏时间相当于1秒钟` |
| `FLD_TIMER_1` | `20 * 18` | **360** | **20 giây** | `--每20秒公布一下战况` (20 s báo tình hình 1 lần) |
| `FLD_TIMER_2` | `39 * 60 * 18` | **42120** | **39 phút** | `--从报名到进入打宝地图40分钟` (ghi 40 phút nhưng số thật là 39) |
| `ENDSIGN_TIME` | `10*60*18 / 360` | **30 nhịp** | **10 phút** | `--报名时间结束` (hết giờ báo danh) |
| `UPBOSS_TIME` | `25*60*18 / 360` | **75 nhịp** | **25 phút** | `--开打15分钟时产生第1个BOSS` |
| `UPBOSS_TIME2` | `30*60*18 / 360` | **90 nhịp** | **30 phút** | `--开打20分钟时产生第2个BOSS` |
| `UPBOSS_TIME3` | `35*60*18 / 360` | **105 nhịp** | **35 phút** | `--开打25分钟时产生第3个BOSS` |
| `REPORT_TIME` | `38*60*18 / 360` | **114 nhịp** | **38 phút** | (báo giờ lần chót) |
| `HUOYUEDU_TIME` | `3*60*18 / 360` | **9 nhịp** | **3 phút** | `--活跃度获得时间，船开启后的3分钟` |

🔴 Chú thích tiếng Trung của 3 mốc BOSS đếm **từ lúc thuyền rời bến (T+10)**, còn biến `t` trong `fld_smalltimer.lua`
đếm **từ lúc mở mission (T+0)**. Vì vậy BOSS ra ở **T+25 / T+30 / T+35 phút** = 15/20/25 phút sau khi rời bến. Không mâu thuẫn.

### 1.2 Dòng thời gian đầy đủ

| Mốc | Nhịp `t` | Nơi thi hành | Việc xảy ra |
|---|---|---|---|
| **T+0:00** | — | `fldmap_boat1.lua:4-30 fenglingdu_main()` | Lặp qua `boatMAPS={337,338,339}`: đặt `SubWorld`, `CloseMission(15)` rồi `OpenMission(15)`, `SetMissionV(MS_STATE,1)`. Sau vòng lặp: `AddGlobalCountNews(str,3)` (rao 3 lần) — "Cuộc đua thuyền ở Phong Lăng Độ chuẩn bị khai cuộc, hãy mau đến Bờ Nam Phong Lăng Độ gặp Thuyền phu nộp Phong Lăng Độ lệnh bài hoặc 200 quyển Mật đồ thần bí để đăng ký thuyền!" |
| T+0:00 | — | `mission.lua:7-17 InitMission()` (do `OpenMission` gọi) | `SetMissionV(i,0)` với i=1..100; `ClearMapNpc/ClearMapTrap/ClearMapObj(worldid)`; `StartMissionTimer(15, 29, FLD_TIMER_1)` (20 s); `StartMissionTimer(15, 28, FLD_TIMER_2)` (39 phút) |
| T+0:00 → T+9:59 | 1..29 | `fld_head.lua:38-81 / 85-150` | **Cửa báo danh mở.** `fld_TakeBoat` chặn khi `t = 10 - GetMissionV(MS_TIMEACC_1MIN) <= 0` (`fld_head.lua:108-111`) và khi thuyền đầy 100 người (`fld_head.lua:153`). |
| mỗi 3 phút | `mod(t,9)==0` và `t<30` | `fld_smalltimer.lua:20-23` | `Msg2MSAll` "còn N phút rời bến" (N = `10 - phút`) |
| **T+10:00** | `t == 30` | `fld_smalltimer.lua:8-14` | Gọi `RunMission(MISSIONID)`; `Msg2MSAll` "Thuyền đi rồi! 30 phút sau sẽ đến bờ Bắc Phong Lăng Độ."; `tbTalkDailyTask:AddTalkNpc(...)` sinh NPC đối thoại nhiệm vụ ngày **Tiểu Chiêu** |
| T+10:00 | — | `mission.lua:19-41 RunMission()` | Sinh **30 NPC 724** ("Thủy Tặc ", cấp 95) tại toạ độ ngẫu nhiên từ bảng `npcthiefpos`, gắn `shuizeideath.lua`; duyệt toàn bộ người chơi trong mission: `SetFightState(1)` + `PutMessage(...)`; `JiluAttendCount()` ghi thống kê |
| **T+13:00** | `t == 30+9 = 39` | `fld_smalltimer.lua:70-88 fld_addhuoyuedu()` | Cộng **điểm năng động (hoạt dược độ)** cho mọi người trong mission qua `DynamicExecuteByPlayer(..., "tbHuoYueDu:AddHuoYueDu", "fenglingdu")`. 🔴 **Loại trừ thuyền 3 (339)**: `if nMapId == boatMAPS[3] ... then return end` |
| mỗi 5 phút | `mod(t,15)==0` và `t>=30` | `fld_smalltimer.lua:25-28` | `Msg2MSAll` "còn N phút sẽ đến bờ Bắc Phong Lăng Độ" (N = `39 - phút`) |
| **T+25:00** | `t == 75` | `fld_smalltimer.lua:35-40` | Sinh **1 × NPC 725** "Thủy tặc đầu lĩnh" (cấp 85, cờ boss=1), gắn `bossdeath.lua`; `Msg2MSAll` "Thủy tặc đầu lĩnh đã xuất hiện." |
| **T+30:00** | `t == 90` | `fld_smalltimer.lua:42-56` | Sinh **1 × NPC 725** như trên; **nếu là khung giờ tốn phí** (`check_new_shuizeitask()==1`) sinh thêm **2 × NPC 1692** "Thủy Tặc Đại Đầu Lĩnh." gắn `bigbossdeath.lua` + rao "Thủy Tặc Đại Đầu Lĩnh xuất hiện rồi" |
| **T+35:00** | `t == 105` | `fld_smalltimer.lua:58-63` | Sinh **1 × NPC 725** (đợt 3) |
| **T+38:00** | `t == 114` | `fld_smalltimer.lua:30-33` | Báo giờ lần cuối |
| **T+39:00** | timer 28 | `fld_landingtimer.lua:2-4 OnTimer()` | `CloseMission(MISSIONID)` |
| T+39:00 | — | `mission.lua:56-63 EndMission()` | `StopMissionTimer(15,28)`, `StopMissionTimer(15,29)`, `Landing()`, `SetMissionV(i,0)` i=1..100 |
| T+39:00 | — | `mission.lua:79-151 Landing()` | Xem mục 1.3 |

### 1.3 `Landing()` — cập bến (`mission.lua:79-151`)

1. `Msg2MSAll(15, "đã đến bờ Bắc Phong Lăng Độ.")` (`:80`)
2. Nếu `20090428 <= ngày < 20090601` (sự kiện Giải Phóng 2009) → `AddGlobalNews` mời giao **[Truy Công Lệnh]** (`:82-86`). 🔴 **Nhánh này chết vĩnh viễn** vì `jf0904_act_dateE = 20090601` (`script\event\jiefang_jieri\200904\taskctrl.lua:24`).
3. Gom danh sách `tbPlayer` bằng `GetNextPlayer(15, idx, 0)` (`:88-102`).
4. `bShuizeiTask = check_new_shuizeitask()` (`:105`).
5. `G_ACTIVITY:OnMessage("FinishFengLingDu", tbPlayer)` (`:107`) và `EventSys:GetType("FengLingDu"):OnEvent("OnLanding", tbPlayer, bShuizeiTask)` (`:108`).
6. Với **từng người**: `SetCurCamp(GetCamp())`, `SetCreateTeam(1)`, `SetDeathScript("")`, `ForbidEnmity(0)` (`:111-117`).
7. Nếu `bShuizeiTask == 1` → phát **2 × vật phẩm `6,1,2743` "Bảo Rương Thủy Tặc"** (`:120`).
8. Theo bản đồ thuyền: `SetLogoutRV(0)` → `NewWorld(fld_landingpos(n))` → `SetFightState(1)` → `DisabledUseTownP(1)` → `SetRevPos(175,1)` (`:123-144`).
9. `ClearMapNpc/Trap/Obj(worldid)` dọn sạch bản đồ thuyền (`:148-150`).

**Điểm cập bến** (`fld_head.lua:15,167-173`, bản đồ 336, toạ độ **ô**):

| Thuyền | Bản đồ | Toạ độ đổ bộ (ô) |
|---|---|---|
| 337 (thượng du) | 336 | 1158, 2964 |
| 338 (trung du) | 336 | 1343, 2868 |
| 339 (hạ du) | 336 | 1482, 2796 |

### 1.4 Lịch mở — nằm NGOÀI cây script

- Engine **có** bảng lịch `\settings\systemtimetask.txt` (chuỗi tại ELF `0x082518E2`, cột `HOUR MIN SCRIPT d DESC`) nhưng **file này RỖNG, chỉ có dòng tiêu đề** (`settings\systemtimetask.txt:1-2`).
- Trong toàn cây, `fldmap_boat*.lua` chỉ được gọi từ **lệnh GM**: `script\gmscript\gmcommand_3.lua:854-857 tbCommand3:FLD_Start()`.
- Log chạy thật xác nhận mission có hoạt động: `Logs\KSG_ScriptOutputLog_20260806.txt:139826-139828` (`ExecuteCode \script\missions\fengling_ferry\fldmap_boat1.lua` → `CallFun ... mission.lua InitMission` ×3 → `fld_smalltimer.lua OnTimer` lặp 3 lần/nhịp).
- ⇒ **Kết luận: lịch thực tế do công cụ bên ngoài (GM tool / cron của vận hành) phát lệnh `ExecuteScript`.** CHƯA XÁC MINH được danh sách giờ mở đầy đủ.
- Giờ **"tốn phí"** thì XÁC ĐỊNH: **10:00, 14:00, 16:00, 18:00, 20:00** (`fld_head.lua:302-317 check_new_shuizeitask()`, lặp lại y hệt ở `script\activitysys\config\1004\extend.lua:37-51 pActivity:IsPaidBoat()`).
  Chú thích lịch sử tại `fld_head.lua:114` cho biết trước 2011-05-04 khung giờ này là **13/15/17/19 h**.

🔴 **Lỗi thật trong lệnh GM**: `FLD_Start()` `Include` file `fldmap_boat1.lua` (file này định nghĩa **`fenglingdu_main()`**) rồi gọi **`main()`** — mà `main()` chỉ do `fldmap_boat2.lua:2` và `fldmap_boat3.lua:2` định nghĩa. Kết quả là lệnh GM **không mở cả 3 thuyền**, chỉ mở đúng 1 thuyền (thuyền nào tuỳ file nào nạp sau cùng). Thứ tự nạp trong log là boat3 → boat1 → boat2 (`Logs\KSG_ScriptOutputLog_20260806.txt:3994,3996,3999`) ⇒ `main` = của boat2 (bản đồ 338).

🔴 **[đã sửa theo phản biện] — KẾT LUẬN TRÊN CHỈ ĐÚNG CHO ĐƯỜNG LỆNH GM.** Log chạy thật chứng minh ngược lại ở đường vận hành: `Logs\KSG_ScriptOutputLog_20260806.txt:139826-139830` cho thấy **2 dòng `ExecuteCode ...fldmap_boat1.lua` rồi **3 dòng liên tiếp** `CallFun /script/missions/fengling_ferry/mission.lua InitMission`**, và sau đó `fld_smalltimer.lua OnTimer` luôn đi **thành cụm 3** (tổng 150 lần = 50 nhịp × 3 thuyền). ⇒ trên máy chủ thật **cả 3 thuyền ĐỀU MỞ**, tức công cụ vận hành gọi `fenglingdu_main()` chứ không đi qua `FLD_Start`.

---

## 2. CƠ CHẾ "THUYỀN"

### 2.1 Bản đồ

| Map ID | Thư mục tài nguyên | Tên hiển thị | Nguồn |
|---|---|---|---|
| **336** | `中原北区\风陵渡` | Phong Lăng độ (bờ Bắc, bản đồ đánh bảo) | `settings\maplist.ini:2908-2923`; `settings\cavelist.ini:1218-1220` (MapPos 304,169) |
| **337** | `中原北区\渡船` | Bến thuyền 1 | `settings\maplist.ini:2925-2932` |
| **338** | `中原北区\渡船` | Bến thuyền 2 | `settings\maplist.ini:2934-2941` |
| **339** | `中原北区\渡船` | Bến thuyền 3 | `settings\maplist.ini:2943-2950` |

🔴 **337/338/339 dùng CHUNG một thư mục tài nguyên bản đồ `中原北区\渡船`** — tức 3 "chiếc thuyền" là **3 bản sao (subworld) của cùng một file bản đồ**, chỉ khác map id. Đây là mấu chốt để port: JX1 cần 3 subworld trỏ cùng 1 `.map`.

Phân loại bản đồ (`settings\map_type.txt`):
- dòng 14: `FENGLING  336,341,342  MATE  风陵渡，打宝地图` → bờ Bắc cấm vật phẩm loại **MATE** (tổ đội).
- dòng 15: `FERRY  337,338,339  TRANSFER,MATE  风陵渡渡船` → trên thuyền cấm **TRANSFER** (hồi thành phù) và **MATE**.

Chú thích thứ tự thuyền (`fld_head.lua:12`): `--南岸渡船地图依顺序为，337南岸上游、338中游、339下游`
= 337 thượng du, 338 trung du, 339 hạ du (tính theo bờ Nam).

### 2.2 Dịch chuyển người chơi

Toàn bộ chỉ là **2 lần `NewWorld`**:

```
fld_head.lua:139   NewWorld(boatmapid, posx, posy)          -- lên thuyền (bờ Nam → 337/338/339)
mission.lua:126/132/138  NewWorld(fld_landingpos(1|2|3))    -- cập bến (thuyền → 336)
fld_death.lua:17/19/21   NewWorld(fld_landingpos(1|2|3))    -- chết trên thuyền → cũng sang 336 luôn
```

Vị trí lên thuyền lấy **ngẫu nhiên từ chính bảng điểm sinh quái** (`fld_head.lua:135-137`):
```lua
posx, posy = fld_getadata(npcthiefpos)   -- trả về toạ độ PIXEL
posx = floor(posx/32); posy = floor(posy/32)   -- NewWorld nhận toạ độ Ô
```
🔴 **Khác đơn vị**: `AddNpc` nhận **pixel** (mission.lua:22 dùng thẳng `posx,posy`), `NewWorld` nhận **ô**. Bảng `npcthiefpos` lưu **pixel**.

### 2.3 Khác biệt giữa 3 thuyền

| Khía cạnh | Thuyền 1 (337) | Thuyền 2 (338) | Thuyền 3 (339) |
|---|---|---|---|
| Script mở riêng | `fldmap_boat1.lua` (`fenglingdu_main`, mở CẢ 3) | `fldmap_boat2.lua` (`main`) | `fldmap_boat3.lua` (`main`) |
| `MS_STATE` gán | 1 (cho cả 3 map) | 2 | 3 |
| Rao tên NPC | "gặp **Thuyền phu**" | "gặp **Thuyền phu Ất**" | "gặp **Thuyền phu Bính Ất**" |
| PK / cừu sát ở giờ tốn phí | **VẪN PK được** | `SetTaskTemp(200,1)` + `ForbidEnmity(1)` → cấm cừu sát | như thuyền 2 |
| Điểm năng động (huoyuedu) | Có | Có | **KHÔNG** (`fld_smalltimer.lua:72`) |
| Điểm đổ bộ trên 336 | 1158,2964 | 1343,2868 | 1482,2796 |
| Thống kê `AddStatData` | `fld_chuan1canjiarenshu` / `fld_chuan1siwangrenshu` | `...chuan2...` | `...chuan3...` |

Ngoài các điểm trên, **3 thuyền chạy y hệt nhau**: cùng mission id 15, cùng 30 quái, cùng 3 đợt boss, cùng sức chứa 100.

### 2.4 Trạng thái áp lên người chơi khi lên thuyền (`fld_head.lua:112-146`)

| Lệnh | Giá trị | Ý nghĩa |
|---|---|---|
| `LeaveTeam()` | — | Buộc rời tổ đội |
| `SetTaskTemp(200,1)` | chỉ khi giờ tốn phí **và** BOATID≠1 | Biến tạm 200 = "cho phép đổi phe, dùng cho chiến trường PK" (`settings\task\player_tmp_task_def.txt:90`) |
| `ForbidEnmity(1)` | như trên | Cấm cừu sát |
| `SetCurCamp(1)` | luôn | Ép về phe 1 |
| `SetFightState(0)` | luôn | Tắt trạng thái chiến đấu lúc mới lên (bật lại ở `RunMission`) |
| `AddMSPlayer(15, 1)` | luôn | Ghi danh vào **nhóm 1** của mission |
| `DisabledUseTownP(1)` | luôn | Cấm hồi thành phù (`--限制其在渡船内使用回城符`) |
| `SetRevPos(175,1)` | luôn | Điểm hồi sinh = **map 175 Tây Sơn thôn** (`settings\maplist.ini:1927-1928`), region 1 → pixel `53440,101440` (`settings\revivepos.ini:218-220`) |
| `SetLogoutRV(1)` | luôn | Thoát game = bị đưa về điểm hồi sinh |
| `SetCreateTeam(0)` | luôn | Cấm lập tổ đội |
| `SetDeathScript(".../fld_death.lua")` | luôn | Script tử vong riêng |

🔴 `AddMSPlayer(15,**1**)` ghi vào **nhóm 1**, còn `fld_haveroom()` đếm `GetMSPlayerCount(15,**1**)` (đúng), nhưng
`RunMission`/`Landing`/`JiluAttendCount` lại duyệt **nhóm 0** (`GetNextPlayer(15,idx,0)`). Theo quy ước dùng khắp cây
(`script\battles\*\head.lua` dùng nhóm 1/2 cho 2 phe, nhóm 0 để duyệt tất cả) thì **nhóm 0 = toàn bộ**. Đây là hành vi đúng.

🔴 **Dữ liệu mission là PER-SUBWORLD, không phải per-mission-id.** Bằng chứng dứt khoát: `fld_TakeBoat` phải
**đổi `SubWorld` sang subworld của thuyền** (`fld_head.lua:104`) rồi mới gọi `fld_haveroom()` / `GetMissionV(...)`, xong
mới khôi phục `SubWorld` (`fld_head.lua:147`). Nếu không thì 3 thuyền sẽ dùng chung biến và mọi mốc giờ sẽ sai gấp 3.

---

## 3. BẢNG MISSION VARIABLE

Khai báo tại `fld_head.lua:28-30`. Chỉ dùng `SetMissionV/GetMissionV`; **không dùng `SetMissionS/GetMissionS`** (đã grep toàn thư mục).

| Hằng | Chỉ số | Ghi ở | Đọc ở | Ý nghĩa |
|---|---|---|---|---|
| `MS_STATE` | **1** | `fldmap_boat1.lua:19` (=1), `fldmap_boat2.lua:15` (=2), `fldmap_boat3.lua:15` (=3) | **KHÔNG NƠI NÀO** | 🔴 Biến chết — ghi mà không bao giờ đọc |
| `MS_TIMEACC_1MIN` | **2** | `fld_smalltimer.lua:17` (+1 mỗi 3 nhịp) | `fld_head.lua:108`, `fld_smalltimer.lua:21,26,31` | Số **phút** đã trôi từ khi mở mission |
| `MS_TIMEACC_20SEC` | **3** | `fld_smalltimer.lua:6` | `fld_smalltimer.lua:5` | Số **nhịp 20 giây** đã trôi (biến `t`) |
| (4..100) | 4..100 | `mission.lua:9`, `mission.lua:61` (đặt 0) | — | Không dùng; chỉ bị xoá hàng loạt ở `InitMission`/`EndMission` |

---

## 4. BẢNG QUÁI / BOSS

Chỉ số NPC = **số dòng `settings\npcs.txt` − 2** (file có 1 dòng tiêu đề, dòng dữ liệu đầu là id 0).

| NPC id | Tên (npcs.txt) | Cấp sinh | Số lượng / lượt | Đợt sinh | Script tử vong | LevelScript | DropRateFile |
|---|---|---|---|---|---|---|---|
| **724** | `Thủy tặc` (dòng 726) | 95 | **30** | T+10:00 (`mission.lua:20-24`) | `shuizeideath.lua` | `\script\npclevelscript\npc_fenglingdunan.lua` | `\settings\droprate\npcdroprate_boatthief.ini` (Count=54, RandRange=1000000) |
| **725** | `Boss Thủy tặc đầu lĩnh` (dòng 727) | 85, cờ boss=1 | **3** (1 mỗi đợt) | T+25 / T+30 / T+35 (`fld_smalltimer.lua:37,44,60`) | `bossdeath.lua` | `\script\npclevelscript\makeboss.lua` | `\settings\droprate\goldennpc\boatboss_droprate.ini` (Count=9) |
| **1692** | `Thủy Tặc Đại Đầu Lĩnh.` (dòng 1694) | 85, cờ boss=1 | **2** | T+30, **chỉ khung giờ tốn phí** (`fld_smalltimer.lua:48-55`) | `bigbossdeath.lua` | `\script\npclevelscript\makeboss.lua` | **RỖNG** (không rơi theo bảng) |

Chữ ký `AddNpc` (suy ra từ `script\battles\battlehead.lua:437`):
`AddNpc(nNpcId, nLevel, nSubWorldIdx, nPixelX, nPixelY, nRemoveOnDeath, szName, nIsBoss)`
- Quái thường: `AddNpc(724, 95, SubWorld, posx, posy, **0**, "Thủy Tặc ")` — `nRemoveOnDeath=0`
- Boss: `AddNpc(725, 85, SubWorld, posx, posy, **1**, "Thủy tặc đầu lĩnh", **1**)`

### 4.1 Rơi đồ

**NPC 724 → `shuizeideath.lua:5-22`**
- `random(1,100) < 50` (≈ **49 %**) **và** `jf0904_shuizei_IsActtime()==1` → rơi **1 × `6,1,2015` "Truy công lệnh"**, hạn dùng = số phút còn lại tới nửa đêm (`ITEM_SetExpiredTime(nItemIdx, nRestMin)`).
- 🔴 `jf0904_shuizei_IsActtime()` = `nDate >= 20090428` (`script\event\jiefang_jieri\200904\shuizei\shuizei.lua:103-106`) ⇒ **LUÔN ĐÚNG kể từ 28/04/2009**. Lua 4 trả `1`/`nil` cho phép so sánh nên `== 1` khớp.

**NPC 725 → `bossdeath.lua:8-49`**
- `SIGNET_DROPCOUNT = 2` (`bossdeath.lua:6`) → rơi **2 × `6,1,1094` "Thí Giả Chi Ấn"**, mỗi món ghi 4 tham số: thời điểm hết hạn = `GetCurServerTime() + 7*24*3600`, năm/tháng/ngày (`bossdeath.lua:17-27`).
  - Nếu bang hội đang có **Đào Chu tiền đãi** (`IsDoubledMoneyBox(nTongID)`, `script\global\signet_head.lua:16-21`) → số lượng = `getSignetDropRate(2)` = `random(14,18)*2/10` làm tròn ⇒ **3 hoặc 4 ấn**.
- Rơi thêm **1 × `6,1,2015`** (hạn tới nửa đêm) — `bossdeath.lua:31-36`.
- **0,5 %** rơi thêm **1 × `6,1,2115` "Hải long châu"** — `bossdeath.lua:38-43`.
- Gọi `tbChangeDestiny:completeMission_WaterThief()` (`script\event\change_destiny\mission.lua:62-77`) — hoàn thành nhiệm vụ "đánh bại Thủy Tặc Đầu Lĩnh của Thần Nông Lão Gia".
- Phát sự kiện `FengLingDu / OnShuiZeiDeath`.

**NPC 1692 → `bigbossdeath.lua:4-6`** — chỉ phát sự kiện `OnShuiZeiDeath`, **không rơi gì trực tiếp**.
Phần thưởng của nó nằm ở activitysys (`script\activitysys\config\40\config.lua:126-142`, `config\45\config.lua:236-250`)
và ở **phần thưởng cập bến** 2 × `6,1,2743` "Bảo Rương Thủy Tặc" (mô tả vật phẩm ghi "Boss Thủy Tặc Đại Đầu Lĩnh rơi ra").

**Bảng rơi `boatboss_droprate.ini`** (9 mục đầu được dùng, `RandRange=10000`):
`6,1,196` (5398), `6,1,122` (1000), `6,1,123` (500), `6,1,124` (300), `6,1,26` (1), `6,1,22` (1), `6,1,71` (100), `6,1,72` (200), `6,1,73` (500).
🔴 File có 19 section `[n]` nhưng `[Main] Count=9` ⇒ engine chỉ đọc 9 mục đầu.

### 4.2 NPC "chết" khác (không được dùng)

`boss.lua` (`---Script -- By-NguyenKhai-------`) định nghĩa `bossben1A/1B/2A/2B/3A/3B` sinh **NPC 513 "Diệu Như"** và
**NPC 511 "Trương Tông Chính"** tại `1632*32, 3221*32` (**[đã sửa theo phản biện]** — chỉ NPC 513 ở `1636*32`) với `SetNpcTimer(nNpcIndex, 900*18)` (15 phút thì `DelNpc`).
🔴 **Toàn bộ là code chết**: hàm gọi duy nhất `thuytacdaulinh()` (`boss.lua:18-25`) có thân **bị comment hết**, và không script nào gọi `thuytacdaulinh`.
🔴 `boss.lua:11` `Include("\\script\\global\\路人_礼官.lua")` — **file này KHÔNG tồn tại** trong cây; `SetNpcDeathScript(...,"\\script\\global\\tamhiep\\callbossdeathmini.lua")` cũng trỏ vào thư mục **không tồn tại**.

### 4.3 Bảng điểm sinh quái — THIẾU TRONG BẢN TRÍCH XUẤT

`fld_head.lua:18`:
```lua
npcthiefpos = "\\settings\\maps\\中原北区\\渡船\\渡船刷怪点.txt"
```
🔴 **Thư mục `settings\maps\中原北区\` KHÔNG có trong `D:\ServerLinux\server1`** (đã liệt kê `settings\maps` bằng `os.listdir`, toàn tên ASCII).
Đây là **phụ thuộc BẮT BUỘC còn thiếu** khi port: cần bảng 2 cột (X, Y **pixel**) làm điểm sinh quái **và** điểm xuất hiện của người chơi lên thuyền.
Định dạng đọc: `GetTabFileHeight` = `TabFile_GetRowCount - 1` (bỏ dòng tiêu đề) — `script\lib\file.lua:38-52`.

**Gợi ý bù dữ liệu:** `settings\task\dailytask\talk_pos.txt:2-13` cho 4 điểm hợp lệ trên cả 3 map 337/338/339 (ô):
`(1607,3163) (1571,3198) (1630,3254) (1665,3220)`; `fld_head.lua:13 boatMAP_POS = {1646, 3233}` (hằng **chết**, không ai đọc);
`boss.lua:28` dùng `1636*32, 3221*32`. ⇒ vùng chơi trên thuyền quanh ô **(1570..1670, 3160..3260)**.

---

## 5. BẢNG BIẾN NHIỆM VỤ NGƯỜI CHƠI (task id) & GIỚI HẠN LƯỢT

| Task id | Loại | Giới hạn | Nơi dùng | Ý nghĩa |
|---|---|---|---|---|
| **3070** | daily | **≤ 5 lần/ngày** | `fld_head.lua:121-125` | Mỗi lần **báo danh giờ tốn phí** tặng 2 × `6,1,30228` "Chân Nguyên Đơn (trung)" (khoá, `nBindState=-2`). Điều kiện: còn ≥1 ô trống túi. Log `{"KinhMach","DangKyPLDTonPhiThanhCong"}` |
| **2863** | daily | **≤ 2 lần/ngày**, 5 điểm/lần | `settings\huoyuedu\huoyuedu.txt:3` (ActivityId 2 = `风陵渡船`) | Bộ đếm **điểm năng động** của Phong Lăng Độ |
| **2880** | daily | trần **100** điểm/ngày | `script\huoyuedu\huoyuedu.lua:12` | Tổng điểm năng động toàn hệ trong ngày |
| **2817** | daily | `== 0` mới nhận | `script\activitysys\config\23\config.lua:92-101` (`TaskVarIdx_FengLingDu`, khai báo `config\23\variables.lua:9`) | "Trong ngày lần đầu hoàn thành Phong Lăng Độ" → 2 × `6,1,2566`; cộng 2 vào 2817 và 2823 |
| **3105** | weekly | cần **≥ 14** để đổi thưởng | `script\activitysys\config\1004\variables.lua:4` (`nTSK_WEEKLY_FENGLINGDU`), dùng ở `config\1004\config.lua:55,248,250` | Bộ đếm tuần; đủ 14 lượt → trừ 14, nhận 1 × `6,1,30134` "Tục Linh Đơn" |
| **1764** | task | 1 lần | `script\event\jiefang_jieri\200904\taskctrl.lua:7` | Đã nhận nhiệm vụ diệt Thủy tặc (sự kiện 2009) |
| **1765** | task | đặt về **1** mỗi ngày mới | `taskctrl.lua:8,49` | 🔴 [đã sửa theo phản biện] Chú thích gốc `taskctrl.lua:8` ghi “上次交水贺任务的时间” = **thời điểm giao nhiệm vụ Thủy tặc lần trước**, không phải “số lần còn lại”; `taskctrl.lua:49` đặt lại `= 1` khi sang ngày mới |
| **200** | **temp** | — | `fld_head.lua:117` | `--记录当前是否可以变玩家阵营，用于PK战之类的地方` (`settings\task\player_tmp_task_def.txt:90`) |

**Giới hạn theo lượt chạy:**
- **Sức chứa mỗi thuyền = 100** (`fld_head.lua:153`) ⇒ tối đa **300 người/lượt** trên cả 3 thuyền.
- **Cửa báo danh = 10 phút đầu** (`fld_head.lua:108-111`).
- **Điều kiện vào**: đã gia nhập môn phái — `GetLastFactionNumber() == -1` thì bị chặn với thông điệp `MSG_NOFACTION_LIMIT` (`fld_head.lua:40-43`, `lang.lua:1`).

**Vé vào cửa:**

| Khung giờ | Vật phẩm chấp nhận | Kiểm tra |
|---|---|---|
| Giờ **thường** | `4,489` **"Lệnh bài Phong Lăng Độ"** (questkey — `settings\item\004\questkey.txt`) | `fld_head.lua:269` |
| Giờ **thường** | **200 cuốn** `6,1,196` **"Mật đồ thần bí"** (đúng 200, không hơn không kém) | `fld_head.lua:189-225` |
| **10/14/16/18/20 h** | `6,1,2745` **"Lệnh Bài Thủy Tặc"** (chỉ đúng vật phẩm này) | `fld_head.lua:274`; yêu cầu thêm ≥1 ô túi trống (`fld_head.lua:280-283`, thông điệp `MSG_NOTENOUGH_ITEM2745`) |

Log hành vi khi báo danh (`fld_head.lua:291-299`):
`AddStatData("shuizeilingpai_shiyongshuliang",1)` (chỉ với 2745);
`tbLog:PlayerActionLog("EventChienThang042011","BaoDanhPhongLangDo")`;
`tbLog:PlayerActionLog("TinhNangKey","BaoDanhPLD_SDLenhBaiThuyTac" | "BaoDanhPLD_SDLenhBaiPLD")`.

---

## 6. TẤT CẢ HÀM LUA DO SCRIPT ĐỊNH NGHĨA

### 6.1 `script\missions\fengling_ferry\fld_head.lua` (bộ não, 321 dòng, GBK)

| Dòng | Hàm | Tham số | Mô tả |
|---|---|---|---|
| 35 | `fld_cancel()` | — | Rỗng (nút "để ta suy nghĩ lại") |
| 38 | `fld_wanttakeboat(addr)` | `addr` = 1..3 | Cửa hội thoại chính của NPC Thuyền phu. Chặn nếu chưa có môn phái; chặn nếu subworld chưa nạp (`SubWorldID2Idx < 0`); chặn nếu thuyền đầy. Dựng menu `Say` khác nhau cho giờ thường / giờ tốn phí. 🔴 **[đã sửa theo phản biện]** `addr` **CÓ** được dùng: `:46 local MapId = boatMAPS[addr]` rồi `:51 SubWorldID2Idx(MapId)` để kiểm tra thuyền tương ứng đã nạp chưa. Nhưng 3 nhánh `if addr==1/2/3` gọi **`Say` giống hệt nhau** (`:74-80`), và thuyền thực sự lên lại do biến toàn cục `BOATID` quyết định (`fld_TakeBoat`, `:90-101`) — hai nguồn tách rời nhau |
| 85 | `fld_TakeBoat(plindex)` | chỉ số người chơi | Toàn bộ thủ tục lên thuyền. Đổi `PlayerIndex`/`SubWorld` tạm, kiểm tra chỗ + giờ, đặt trạng thái, `AddMSPlayer`, `NewWorld`, phát sự kiện `OnBoard`. Trả `1` nếu thành công, `0` nếu hỏng |
| 152 | `fld_haveroom()` | — | Trả `1` khi `GetMSPlayerCount(15,1) >= 100`, kèm `Say` "Thuyền đã đầy" |
| 167 | `fld_landingpos(posation)` | 1..3 | Trả `northMAP, x, y`. 🔴 Điều kiện chặn viết sai: `if (posation <= 0 and posation > 3)` — dùng `and` thay vì `or` nên **không bao giờ đúng** |
| 175 | `fld_getadata(file)` | đường dẫn bảng | Đọc ngẫu nhiên 1 dòng (cột 1,2) từ bảng tab. Bản sao của `GetRandomAData` (`script\lib\file.lua:55-64`) nhưng **không kiểm tra `totalcount == 0`** 🔴 |
| 184 | `use_juanzhou()` | — | Mở `GiveItemUI` nhận Mật đồ thần bí |
| 189 | `exchange_juanzhou(ncount)` | số ô | Cộng dồn `GetItemStackCount`; phải **đúng 200**; gọi `fld_TakeBoat`; xoá vật phẩm |
| 229 | `use_lingpai()` | — | `GiveItemUI` nhận Lệnh bài Phong Lăng Độ |
| 233 | `use_suizeilingpai()` | — | `GiveItemUI` nhận Lệnh Bài Thủy Tặc (tham số thứ 5 = `1`) |
| 238 | `exchange_lingpai_1(ncount)` | | → `exchange_lingpai(ncount, 1)` |
| 242 | `exchange_lingpai_2(ncount)` | | → `exchange_lingpai(ncount, 2)` |
| 247 | `exchange_lingpai(ncount, ntype)` | | Xác thực đúng 1 món, `nStackCount == 1`. 🔴 **[đã sửa theo phản biện]** nhánh `ntype==1` kiểm `itemgenre==4 and detailtype==489` (`:269`); nhánh `ntype==2` **chỉ** kiểm `itemgenre==6 and particular==2745` (`:274`) — **không kiểm `detailtype`**; kiểm ô túi (2745); `fld_TakeBoat`; `RemoveItemByIndex`; ghi log |
| 302 | `check_new_shuizeitask()` | — | Trả `1` nếu giờ hiện tại ∈ {10,14,16,18,20}, ngược lại `0` |
| 319 | `no()` | — | Rỗng |

### 6.2 `mission.lua` (**153 dòng** [đã sửa theo phản biện], GBK) — bản **ĐANG CHẠY**

| Dòng | Hàm | Vai trò |
|---|---|---|
| 7 | `InitMission()` | Engine gọi khi `OpenMission` |
| 19 | `RunMission()` | Được `fld_smalltimer.lua:9` gọi tay ở T+10 |
| 43 | `JiluAttendCount()` | Ghi `AddStatData("fld_chuanNcanjiarenshu", nCount)` |
| 56 | `EndMission()` | Engine gọi khi `CloseMission` |
| 65 | `OnLeave(RoleIndex)` | Engine gọi khi 1 người rời mission (kể cả do chết). Trả lại `SetCreateTeam(1)`, `ForbidEnmity(0)`, `SetDeathScript("")`, `SetFightState(0)`, `ForbidChangePK(0)`, `SetPKFlag(0)` |
| 79 | `Landing()` | Cập bến (mục 1.3) |

### 6.3 Còn lại

| Tệp | Dòng | Hàm | Vai trò |
|---|---|---|---|
| `fld_smalltimer.lua` | 4 | `OnTimer()` | Nhịp 20 giây (timer 29) — nhịp tim của hoạt động |
| `fld_smalltimer.lua` | 70 | `fld_addhuoyuedu(nPassedTime)` | Cộng điểm năng động ở T+13, bỏ qua map 339 |
| `fld_landingtimer.lua` | 2 | `OnTimer()` | Timer 28 → `CloseMission(15)` |
| `fld_death.lua` | 3 | `OnDeath(Launcher)` | Chết trên thuyền: `Msg2Player`, `SetCurCamp(GetCamp())`, `JiluDeathCount(1)`, `DelMSPlayer(15,0)`, `SetLogoutRV(0)`, `NewWorld(fld_landingpos(n))` |
| `fld_death.lua` | 27 | `JiluDeathCount(nCount)` | `AddStatData("fld_chuanNsiwangrenshu", ...)` |
| `bossdeath.lua` | 8 | `OnDeath(nNpcIndex)` | Rơi đồ Boss 725 (mục 4.1) |
| `bigbossdeath.lua` | 4 | `OnDeath(nNpcIndex)` | Chỉ phát sự kiện |
| `shuizeideath.lua` | 5 | `OnDeath(nNpcIndex)` | Rơi Truy công lệnh 49 % |
| `fldmap_boat1.lua` | 4 | `fenglingdu_main()` | Mở **cả 3** thuyền + rao tin |
| `fldmap_boat2.lua` | 2 | `main()` | Mở riêng thuyền 338 |
| `fldmap_boat3.lua` | 2 | `main()` | Mở riêng thuyền 339 |
| `boss.lua` | 18,27,34,43,50,59,66,75 | `thuytacdaulinh`, `bossben1A/1B/2A/2B/3A/3B`, `OnTimer(nNpcIndex,nTimeOut)` | **Code chết** (mục 4.2) |
| `lang.lua` | 1,2 | (hằng) `MSG_NOFACTION_LIMIT`, `MSG_NOTENOUGH_ITEM2745` | Chuỗi tiếng Việt |

### 6.4 Hàm ngoài thư mục nhưng thuộc luồng

| Hàm | Tệp:dòng | Vai trò |
|---|---|---|
| `GetTabFileHeight(mapfile)` | `script\lib\file.lua:38` | = `TabFile_GetRowCount - 1` |
| `GetTabFileData(mapfile,row,col)` | `script\lib\file.lua:46` [đã sửa theo phản biện] | = `tonumber(TabFile_GetCell(...))` |
| `tbAwardTemplet:Give(tbItem,nAwardCount,tbLogTitle)` | `script\lib\awardtemplet.lua:32` | Phát thưởng |
| `tbAwardTemplet:GiveAwardByList(tbItem,szLogTitle,nAwardCount)` | `script\lib\awardtemplet.lua:59` | Bọc `Give` |
| `getSignetDropRate(nCnt)` / `IsDoubledMoneyBox(nTongID)` | `script\global\signet_head.lua:6 / 16` | Nhân đôi ấn boss |
| `jf0904_shuizei_IsActtime()` | `script\event\jiefang_jieri\200904\shuizei\shuizei.lua:103` | 🔴 luôn đúng từ 2009 |
| `tbChangeDestiny:completeMission_WaterThief()` | `script\event\change_destiny\mission.lua:62` | Nhiệm vụ Thần Nông |
| `tbTalkDailyTask:AddTalkNpc(nMapTemplate,nMapId)` | `script\activitysys\config\32\talkdailytask.lua:22` | Sinh NPC "Tiểu Chiêu" |
| `tbHuoYueDu:AddHuoYueDu(szActivity)` | `script\huoyuedu\huoyuedu.lua:68` | Cộng điểm năng động |
| `NpcFunLib:CheckBoatBoss(nNpcIndex)` | `script\activitysys\npcfunlib.lua:160` | **Điều kiện đếm điểm activitysys**: NPC setting idx phải là **725** và script NPC không phải `bossscript.lua`/`callbossdeath.lua` |
| `pActivity:IsPaidBoat()` | `script\activitysys\config\1004\extend.lua:37` | Bản sao `check_new_shuizeitask` |
| `_Detail:OnMessage(tbParam)` (type `FinishFengLingDu`) | `script\activitysys\detailtype\finishifenglingdu.lua:8` | Rẽ nhánh thông điệp ra từng người chơi |

---

## 7. TẤT CẢ HÀM ENGINE ĐƯỢC GỌI

Đã đối chiếu với `D:\GAMEDEVNEW\ReverseTools\jx_linux_y.luamap.full.txt` (1560 mục).

| Hàm engine | Địa chỉ ELF | Gọi tại (tệp:dòng) |
|---|---|---|
| `OpenMission` | `0x081332F0` | `fldmap_boat1.lua:18`, `fldmap_boat2.lua:12`, `fldmap_boat3.lua:12` |
| `CloseMission` | `0x081327E0` | `fldmap_boat1.lua:17`, `fld_landingtimer.lua:3`, `gmcommand_3.lua:864` |
| `StartMissionTimer` | `0x08138840` | `mission.lua:15,16` |
| `StopMissionTimer` | `0x08134720` | `mission.lua:57,58` |
| `SetMissionV` | `0x08107390` | `mission.lua:9,61`; `fld_smalltimer.lua:6,17`; `fldmap_boat1/2/3` |
| `GetMissionV` | `0x081072F0` | `fld_head.lua:108`; `fld_smalltimer.lua:5,17,21,26,31` |
| `AddMSPlayer` | `0x081366A0` | `fld_head.lua:138` |
| `DelMSPlayer` | `0x081372A0` | `fld_death.lua:12` |
| `GetMSPlayerCount` | `0x081351F0` | `fld_head.lua:153`; `mission.lua:26,45,90`; `fld_smalltimer.lua:78` |
| `GetNextPlayer` | `0x08135760` | `mission.lua:28,92`; `fld_smalltimer.lua:80` |
| `Msg2MSAll` | `0x08134280` | `mission.lua:68,80`; `fld_smalltimer.lua:10,22,27,32,39,46,53,62` |
| `SubWorldID2Idx` | `0x08102580` | `fld_head.lua:51,92,95,98,104`; `boss.lua:28…`; `fldmap_boat*` |
| `SubWorldIdx2ID` | `0x081077D0` | `mission.lua:11,46,104,123`; `fld_death.lua:15,28`; `fld_smalltimer.lua:13,71` |
| `ClearMapNpc` | `0x08102E90` | `mission.lua:12,148` |
| `ClearMapTrap` | `0x08102AC0` | `mission.lua:13,149` |
| `ClearMapObj` | `0x08102B40` | `mission.lua:14,150` |
| `AddNpc` | `0x0811BB10` | `mission.lua:22`; `fld_smalltimer.lua:37,44,51,60`; `boss.lua:28,35,44,51,60,67` |
| `SetNpcDeathScript` | `0x08101500` | `mission.lua:23`; `fld_smalltimer.lua:38,45,52,61`; `boss.lua:30,…` |
| `SetNpcTimer` | `0x080FC180` | `boss.lua:31,38,47,54,63,70` |
| `DelNpc` | `0x08107460` | `boss.lua:76` |
| `GetNpcPos` | `0x081293F0` | `bossdeath.lua:9,41`; `shuizeideath.lua:11` |
| `DropItem` | `0x081200B0` | `bossdeath.lua:22,31,42`; `shuizeideath.lua:13` |
| `SetSpecItemParam` | `0x080FF360` | `bossdeath.lua:23,24,25,26` |
| `SyncItem` | `0x08114EF0` | `bossdeath.lua:27,36`; `shuizeideath.lua:18` |
| `ITEM_SetExpiredTime` | `0x08154A30` | `bossdeath.lua:35`; `shuizeideath.lua:17` |
| `GiveItemUI` | `0x0812BBA0` | `fld_head.lua:185,230,235` |
| `GetGiveItemUnit` | `0x08114E60` | `fld_head.lua:195,258` |
| `GetItemProp` | `0x080FF260` | `fld_head.lua:196,266` |
| `GetItemStackCount` | `0x080FD250` | `fld_head.lua:200,259` |
| `RemoveItemByIndex` | `0x08114F80` | `fld_head.lua:222,289` |
| `CalcFreeItemCellCount` | `0x0810CA20` | `fld_head.lua:121,280` |
| `NewWorld` | `0x0811B690` | `fld_head.lua:139`; `mission.lua:126,132,138`; `fld_death.lua:17,19,21` |
| `SetRevPos` | `0x0811B370` | `fld_head.lua:143`; `mission.lua:129,135,141` |
| `SetLogoutRV` | `0x08110500` | `fld_head.lua:144`; `mission.lua:125,131,137`; `fld_death.lua:13` |
| `DisabledUseTownP` | `0x08130A80` | `fld_head.lua:142`; `mission.lua:128,134,140` |
| `SetCreateTeam` | `0x08120FC0` | `fld_head.lua:145`; `mission.lua:69,114` |
| `LeaveTeam` | `0x08121060` | `fld_head.lua:112` |
| `SetDeathScript` | `0x08110700` | `fld_head.lua:146`; `mission.lua:72,115` |
| `SetFightState` | `0x08117A10` | `fld_head.lua:134`; `mission.lua:34,73,127,133,139` |
| `SetCurCamp` | `0x0811B1D0` | `fld_head.lua:127`; `mission.lua:113`; `fld_death.lua:7` |
| `GetCamp` | `0x08114650` | `mission.lua:112`; `fld_death.lua:6` |
| `ForbidEnmity` | `0x0810B0F0` | `fld_head.lua:118`; `mission.lua:70,117` |
| `ForbidChangePK` | `0x0810F590` | `mission.lua:74` |
| `SetPKFlag` | `0x0810F610` | `mission.lua:75` |
| `SetTaskTemp` | `0x08123950` | `fld_head.lua:117` |
| `GetLastFactionNumber` | `0x0810E6D0` | `fld_head.lua:40` |
| `Say` | `0x08123C90` | `fld_head.lua:53,75,77,79,155,158,161,205,209,213,218,249,254,262,270,275,281,285` |
| `Talk` | `0x08116930` | `fld_head.lua:41` |
| `Msg2Player` | `0x081171B0` | `fld_head.lua:141`; `fld_death.lua:4` |
| `PutMessage` | `0x08116F80` | `mission.lua:35` |
| `GetName` | `0x08111E70` | `mission.lua:68` |
| `AddGlobalNews` | `0x08125A90` | `mission.lua:85`; `fldmap_boat1.lua:28` |
| `AddGlobalCountNews` | `0x081258E0` | `fldmap_boat1.lua:23`; `fldmap_boat2.lua:14`; `fldmap_boat3.lua:14` |
| `AddStatData` | `0x080FF550` | `fld_head.lua:291`; `mission.lua:48,50,52`; `fld_death.lua:30,32,34` |
| `GetLocalDate` | `0x0812A140` | `fld_head.lua:303`; `mission.lua:82`; `bossdeath.lua:33`; `shuizeideath.lua:15` |
| `GetCurServerTime` | `0x08103800` | `bossdeath.lua:17` |
| `FormatTime2String` | `0x08106720` | `bossdeath.lua:18,19,20` |
| `GetTongName` | `0x0811AB30` | `bossdeath.lua:11` |
| `DynamicExecuteByPlayer` | `0x0812FE80` | `fld_smalltimer.lua:82` |
| `AddEventItem` | `0x0811DF90` | `fld_head.lua:1` (comment); `gmcommand_3.lua:860` |
| `LoadMap` | `0x081013D0` | `gmcommand_3.lua:849` |
| `TabFile_Load` | `0x0814AEF0` | qua `script\lib\file.lua:39,47` [đã sửa theo phản biện] |
| `TabFile_GetRowCount` | `0x0814A690` | qua `script\lib\file.lua:43` [đã sửa theo phản biện] |
| `TabFile_GetCell` | `0x0814A740` | qua `script\lib\file.lua:51` [đã sửa theo phản biện] |

🔴 **`GetTabFileHeight` / `GetTabFileData` KHÔNG phải hàm engine** — không có trong luamap và `re_disasm.py --find` báo "khong thay chuoi". Chúng là hàm Lua trong `script\lib\file.lua`.

---

## 8. PHỤ THUỘC DỮ LIỆU NGOÀI

### 8.1 Bảng cấu hình

| Tệp | Dòng | Nội dung dùng đến |
|---|---|---|
| `settings\task\missions.txt` | 16 | `15  \script\missions\fengling_ferry\mission.lua` |
| `settings\timertask.txt` | 29, 30 | `28 → fld_landingtimer.lua`, `29 → fld_smalltimer.lua` |
| `settings\systemtimetask.txt` | (rỗng) | Bảng lịch của engine — **không cấu hình gì** |
| `settings\maplist.ini` | 2908-2950 | Map 336/337/338/339 |
| `settings\cavelist.ini` | 1218-1220 | Map 336 (MapPos 304,169) |
| `settings\map_type.txt` | 14, 15 | `FENGLING 336,341,342 / MATE`; `FERRY 337,338,339 / TRANSFER,MATE` |
| `settings\revivepos.ini` | 218-220 | `[175] region=1,1 → 53440,101440` |
| `settings\npcs.txt` | 726, 727, 1694 | NPC 724 / 725 / 1692 |
| `settings\droprate\npcdroprate_boatthief.ini` | — | Bảng rơi quái 724 (Count=54) |
| `settings\droprate\goldennpc\boatboss_droprate.ini` | — | Bảng rơi boss 725 (Count=9) |
| `settings\item\004\questkey.txt` | — | `Lệnh bài Phong Lăng Độ  4  489` |
| `settings\item\004\magicscript.txt` | 1094/2015/2115/2743/2745/196/30228/30134 | Xem bảng 8.2 |
| `settings\huoyuedu\huoyuedu.txt` | 3 | `2  风陵渡船  2863  2  5 5` |
| `settings\task\dailytask\talk.txt` | 4 | `3  Thuyền Phong Lăng Độ  Tiểu Chiêu  1` |
| `settings\task\dailytask\talk_pos.txt` | 2-13 | 12 vị trí NPC Tiểu Chiêu trên 337/338/339 (NpcRes 1601) |
| `settings\achievement\achievements.txt` | 117-119 | Thành tựu 116/117/118 (1 / 10 / 50 lần diệt Thủy Tặc) |
| `settings\achievement\script_data.txt` | 118-120 | Loại `FinishShuiZeiTask` |
| `settings\task\player_tmp_task_def.txt` | 90 | Biến tạm 200 |
| `settings\waypoint.txt` | **dòng 225** (id 224) [đã sửa theo phản biện] | `Phong Lăng độ  336, 1112, 3189` |
| **`settings\maps\中原北区\渡船\渡船刷怪点.txt`** | — | 🔴 **THIẾU** — bảng điểm sinh (pixel) |

### 8.2 Vật phẩm

| Genre,Detail,Particular | Tên | Vai trò |
|---|---|---|
| `4, 489` | Lệnh bài Phong Lăng Độ | Vé vào giờ thường (`questkey.txt`) |
| `6,1,196` | Mật đồ thần bí | 200 cuốn = vé thay thế; cũng là món rơi chính của boss (5398/10000) |
| `6,1,2745` | Lệnh Bài Thủy Tặc | Vé vào **giờ tốn phí** |
| `6,1,1094` | Thí Giả Chi Ấn | 2 (hoặc 3-4) món/boss 725, có hạn 7 ngày |
| `6,1,2015` | Truy công lệnh | 49 % từ quái 724, 100 % từ boss 725; hết hạn lúc nửa đêm |
| `6,1,2115` | Hải long châu | 0,5 % từ boss 725 |
| `6,1,2743` | Bảo Rương Thủy Tặc | 2 món khi **cập bến** trong giờ tốn phí |
| `6,1,30228` | Chân Nguyên Đơn (trung) | 2 món khi **báo danh** giờ tốn phí, ≤5 lần/ngày (task 3070) |
| `6,1,2566` | (thưởng activitysys 23) | 2 món lần đầu hoàn thành trong ngày |
| `6,1,30134` | Tục Linh Đơn | Thưởng tuần khi 3105 ≥ 14 |

### 8.3 Hệ thống liên đới

| Hệ | Đăng ký | Sự kiện nghe |
|---|---|---|
| `EventSys` type **`FengLingDu`** | `script\misc\eventsys\type\func.lua:22-28` — 4 sự kiện: `OnBoard`, `OnLanding`, `OnFinishShuiZeiTask`, `OnShuiZeiDeath` | |
| Thành tựu | `script\global\achievementsys\type\fenglingdu.lua:10` | `OnFinishShuiZeiTask` |
| Chuyển sinh 6 | `script\task\metempsychosis\translife_6.lua:215` | `OnLanding` |
| Chuyển sinh 7 | `script\task\metempsychosis\translife_7.lua:338` | `OnLanding` |
| Bắc Đẩu Lệnh Bài | `script\event\beidoulingpai\beidouactivity.lua:478` | `OnLanding` |
| Prize GS | `script\event\prize\func_prize_gs.lua:152` | `OnBoard` |
| Hệ Phi Phong | `script\global\mantlesystem\other_func_outputs.lua:174` | `OnShuiZeiDeath` |
| `G_ACTIVITY` | `script\activitysys\detailtype\finishifenglingdu.lua:5` (`"FinishFengLingDu"`) | Dùng ở **7 thư mục**: 1001:108, 1002:71, 1004:43+366, 18:112, 23:85, 31:507, 45:200. 🔴 [đã sửa theo phản biện] config **4** và **40** KHÔNG dùng `FinishFengLingDu` mà dùng `szMessageType="NpcOnDeath"` (`config\4\config.lua:95,113`); 1003 và 1016 đã bị comment |
| Điểm năng động | `script\huoyuedu\huoyuedu.lua:22` (`fenglingdu` = ActivityId 2) | Gọi từ `fld_smalltimer.lua:82` |
| Nhiệm vụ đối thoại ngày | `script\activitysys\config\32\talkdailytask.lua:22` | Gọi từ `fld_smalltimer.lua:13` |

---

## 9. SO SÁNH BẢN `.bak` — BẢN NÀO ĐANG CHẠY

Engine nạp `mission.lua` / `fld_smalltimer.lua`; đuôi `.bak` **không được nạp**.
Thời gian sửa file: `mission.lua` & `fld_smalltimer.lua` = **2021-10-30**, hai file `.bak` = **2022-04-03** (mới hơn).
Kiểu xuống dòng: bản đang chạy = **CRLF**, bản `.bak` = **LF**.

### 9.1 `mission.lua.bak` → `mission.lua`

```diff
 function InitMission()
-	do return end                                     <-- .bak: TẮT toàn bộ hoạt động
 	for i = 1, 100 do
@@ RunMission()
 	idx = 0
-	for i = 1 , 100 do
+	local nCount = GetMSPlayerCount(MISSIONID, 0)
+	for i = 1 , nCount do
@@ Landing()
 	idx = 0
-	for i = 1 , 100 do
+	local nCount = GetMSPlayerCount(MISSIONID, 0)
+	for i = 1 , nCount do
```

### 9.2 `fld_smalltimer.lua.bak` → `fld_smalltimer.lua`

```diff
 function OnTimer()
-	do return en                                      <-- .bak: LỖI CÚ PHÁP (thiếu chữ 'd')
 	t = GetMissionV(MS_TIMEACC_20SEC) + 1
@@ UPBOSS_TIME2, nhánh giờ tốn phí
-			AddNpc(1692, 85, SubWorld, posx, posy, 1, "Thủy Tặc Đại Đầu Lĩnh.", 1)
+			npcindex = AddNpc(1692, 85, SubWorld, posx, posy, 1, "Thủy Tặc Đại Đầu Lĩnh.", 1)
+			SetNpcDeathScript(npcindex, "\\script\\missions\\fengling_ferry\\bigbossdeath.lua");
```

**Kết luận:** hai file `.bak` là bản **đã bị vô hiệu hoá** (`do return end`), trong đó `fld_smalltimer.lua.bak` còn
**không biên dịch được** vì viết thiếu chữ `d` (`do return en`). Bản đang chạy là bản không có `do return`, đã sửa
2 vòng lặp cứng `1..100` thành `1..GetMSPlayerCount` và đã bổ sung death-script cho Đại Đầu Lĩnh 1692.
Log chạy thật (`Logs\KSG_ScriptOutputLog_20260806.txt:139828-139850`) cho thấy `InitMission` và `OnTimer` **có chạy**
⇒ xác nhận hoạt động đang **BẬT**.

---

## 10. DANH SÁCH BẪY & LỖI THẬT KHI PORT SANG JX1

1. 🔴 **`settings\maps\中原北区\渡船\渡船刷怪点.txt` không có trong bản trích xuất** — không có nó thì `fld_getadata` gọi `random(0)` (`fld_head.lua:177`) và cả việc lên thuyền lẫn sinh quái đều hỏng. Phải tự dựng bảng điểm (pixel), vùng ~ô (1570..1670, 3160..3260).
2. 🔴 **Script NPC Thuyền phu bờ Nam KHÔNG có trong cây** — grep `fld_wanttakeboat` và `BOATID` toàn bộ `D:\ServerLinux\server1` chỉ ra **duy nhất `fld_head.lua`**. NPC phải đặt biến toàn cục `BOATID` rồi gọi `fld_wanttakeboat(addr)`. Đây là **thiết kế xấu** (2 nguồn chọn thuyền: tham số `addr` **chỉ dùng để kiểm tra map ở `fld_head.lua:46`** + biến toàn cục `BOATID` dùng để lên thuyền ở `:90-101`) **[đã sửa theo phản biện]**.
3. 🔴 **Mission variable là per-subworld** — nếu JX1 lưu theo mission-id thì 3 thuyền dùng chung `t` và mọi mốc giờ sai gấp 3.
4. 🔴 `fld_landingpos` có điều kiện chặn sai (`and` thay vì `or`, `fld_head.lua:168`) — không bao giờ chặn.
5. 🔴 `fld_getadata` không kiểm tra `totalcount == 0`, khác với bản gốc `GetRandomAData` (`script\lib\file.lua:56-58`).
6. 🔴 **Chết trên thuyền = được cập bến sớm** (`fld_death.lua:15-24` dùng chính `fld_landingpos`) — người chơi có thể tự sát để lên bờ Bắc trước 39 phút. Kiểm lại xem đây là chủ ý hay lỗ hổng.
7. 🔴 `jf0904_shuizei_IsActtime()` chỉ so `nDate >= 20090428` ⇒ **luôn đúng**; nhánh "Truy công lệnh" của quái/boss vẫn chạy đến ngày nay dù sự kiện 2009 đã kết thúc (`taskctrl.lua:23-24` `dateE = 20090601`).
8. 🔴 `boss.lua` là **code chết** và `Include` 2 đường dẫn không tồn tại (`\script\global\路人_礼官.lua`, `\script\global\tamhiep\callbossdeathmini.lua`). Không port.
9. 🔴 `MS_STATE` (chỉ số 1) ghi mà không đọc; `boatMAP_POS`, `TNPC_THIEF`, `TNPC_THIEF_COUNT` là **hằng chết** (`fld_head.lua:13,16,17`) — số 30 quái viết cứng ở `mission.lua:20`.
10. 🔴 Lệnh GM `FLD_Start` gọi `main()` chứ không phải `fenglingdu_main()` (`gmcommand_3.lua:854-857`) ⇒ **riêng đường lệnh GM** không mở đủ 3 thuyền. **[đã sửa theo phản biện]** Đường vận hành thật thì MỞ ĐỦ 3 — log `KSG_ScriptOutputLog_20260806.txt:139826-139830` có **3 `InitMission` liên tiếp**.
11. 🔴 `AddNpc` dùng **pixel**, `NewWorld` dùng **ô** — sai đơn vị là quái/người rơi ra ngoài bản đồ.
12. 🔴 Thông điệp "30 phút sau sẽ đến" (`mission.lua:35`, `fld_smalltimer.lua:10`) nhưng thời lượng thật là **29 phút** (39 − 10). Chú thích `--40分钟` ở `fld_head.lua:20` cũng lệch với giá trị `39`.
13. **JX1 đã có sẵn một bản Phong Lăng Độ khác** (không liên quan bản Linux này) — 🔴 **[đã sửa theo phản biện]: xem mục B5/B6 của Phụ lục phản biện, bản JX1 ĐANG SỐNG không phải `header\phonglangdo.lua` mà là 8 tệp `script\tinhnang\phonglangdo\`, mission id `MS_PLANGDO = 4`, `MAP_DUATHUYEN_PLD = {337,338,339}`**:
    `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\header\phonglangdo.lua` (44 điểm quái + 2 điểm boss viết cứng, `AddNpcNew(724,...)` / `AddNpcNew(725,...)`, mission id 1, **chỉ 1 map 337**) và
    `...\bin\server\script\missions\mission04.lua` (`MS_PLANGDO`, `MAP_DUATHUYEN_PLD`, gọi `\script\tinhnang\phonglangdo\lib_phonglangdo.lua`).
    Khi port cần quyết định: thay thế hay chạy song song. **Bảo đóng gói `closure3.json` cho biết 44/82 tệp phụ thuộc CHƯA có trong JX1**, và **toàn bộ 13 tệp `missions/fengling_ferry/*` đều chưa có**.

---

## PHỤ LỤC PHẢN BIỆN (tác tử độc lập)

**Ngày:** 2026-08-24 · **Phạm vi:** kiểm chứng ngược 40 khẳng định của thân bài đối chiếu trực tiếp tệp gốc
(`D:\ServerLinux\server1`, `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server`) và `jx_linux_y.luamap.full.txt`.
Nguyên tắc: **mặc định coi khẳng định là SAI cho đến khi tệp gốc chứng minh ngược lại**.
Bản sao trước phản biện: `02_phonglangdo.md.truoc_phanbien`.

**Kết quả:** 40 khẳng định được kiểm chứng — **29 ĐÚNG**, **11 SAI / SAI MỘT PHẦN** (đã sửa vào thân bài,
đánh dấu `[đã sửa theo phản biện]`), **0 không xác minh được**. Tìm thêm **15 chỗ bỏ sót**, trong đó
**2 lỗi thật chưa ai nêu** và **2 nguồn dữ liệu bù được phần "THIẾU" mà thân bài coi là bế tắc**.

### A. Bảng kiểm chứng

| # | Khẳng định (thân bài) | Bằng chứng gốc đã đọc lại | KẾT LUẬN | Sửa lại thành |
|---|---|---|---|---|
| 1 | Mission id **15** → `\script\missions\fengling_ferry\mission.lua` | `settings\task\missions.txt:16` = `15 \script\missions\fengling_ferry\mission.lua` | **ĐÚNG** | — |
| 2 | 337/338/339 dùng **chung** thư mục tài nguyên `中原北区\渡船` | `settings\maplist.ini:2925,2934,2943` — cả 3 dòng đều `=中原北区\渡船`; 336 là `中原北区\风陵渡` (`:2908`) | **ĐÚNG** | — |
| 3 | Hằng thời gian: `FLD_TIMER_2=39*60*18`, `ENDSIGN=30`, `UPBOSS=75/90/105`, `REPORT=114`, `HUOYUEDU=9` | `fld_head.lua:19-26` đọc nguyên văn; `fld_smalltimer.lua:72` dùng `ENDSIGN_TIME + HUOYUEDU_TIME` = 39 nhịp = T+13 | **ĐÚNG** | — |
| 4 | Timer 28 = `fld_landingtimer.lua`, timer 29 = `fld_smalltimer.lua` | `settings\timertask.txt:29` (`28 …fld_landingtimer.lua`), `:30` (`29 …fld_smalltimer.lua`) | **ĐÚNG** | — |
| 5 | Sức chứa **100**/thuyền; cửa báo danh **10 phút** | `fld_head.lua:153` `GetMSPlayerCount(MISSIONID,1) >= 100`; `fld_head.lua:108-111` `t = 10 - GetMissionV(MS_TIMEACC_1MIN); if (t <= 0) return 0` | **ĐÚNG** | — |
| 6 | Vé: `4,489` / **đúng 200** cuốn `6,1,196` / giờ tốn phí `6,1,2745`; giờ tốn phí = {10,14,16,18,20} | `fld_head.lua:269` (`genre~=4 or detail~=489`), `:204-216` (3 chốt ~=/>/< 200), `:274` (`genre~=6 or particular~=2745`), `:302-317` bảng `tb_sptime` | **ĐÚNG** | — |
| 7 | NPC 724/725/1692; id = dòng `npcs.txt` − 2; 1692 **DropRateFile rỗng** | `npcs.txt:726` = "Thủy tặc", `:727` = "Boss Thủy tặc đầu lĩnh", `:1694` = "Thủy Tặc Đại Đầu Lĩnh."; cột `LevelScript`/`DropRateFile` khớp 100 %; hàng 1694 **rỗng** cột DropRateFile | **ĐÚNG** | — |
| 8 | Quái 724 rơi `6,1,2015` với xác suất ≈ **49 %** | `shuizeideath.lua:9-10` `random(1,100)` + `if nCurRate < 50` ⇒ 49/100 | **ĐÚNG** | — |
| 9 | Boss 725: 2×`1094` (3–4 nếu Đào Chu), 1×`2015`, **0,5 %** `2115` | `bossdeath.lua:6,21-28,31-36,38-43`; tự tính `getSignetDropRate(2)` (`signet_head.lua:6-14`) cho tập {28,30,32,34,36} → **{3,3,3,3,4}**; `random(10000000) <= 50000` = 0,5 % | **ĐÚNG** | — |
| 10 | `boatboss_droprate.ini` có **19** section nhưng `[Main] Count=9`; 9 mục = 196/122/123/124/26/22/71/72/73 | Đếm `^\[` = 20 (kể `[Main]`) ⇒ 19 mục số; `Count=9`, `RandRange=10000`; đọc tay `[1]`…`[9]` khớp từng `Particular`/`RandRate` | **ĐÚNG** | — |
| 11 | Mission var: 1 `MS_STATE` (ghi không đọc), 2 = phút, 3 = nhịp; không dùng `SetMissionS` | `fld_head.lua:28-30`; grep `MS_STATE` chỉ có ở `fldmap_boat1/2/3`; grep `MissionS` = 0 hit trong thư mục | **ĐÚNG** | — |
| 12 | Dữ liệu mission là **per-subworld** — `fld_TakeBoat` đổi `SubWorld` rồi mới đọc `GetMissionV` | `fld_head.lua:103-104` (`oldsubworldindex = SubWorld; SubWorld = SubWorldID2Idx(boatmapid)`) → `:105,108` → khôi phục `:147` | **ĐÚNG** (nhưng thiếu 1 lỗi, xem **B1**) | — |
| 13 | `fld_landingpos` chặn sai: `and` thay vì `or` | `fld_head.lua:168` `if (posation <= 0 and posation >3) then` | **ĐÚNG** | — |
| 14 | `fld_getadata` thiếu `totalcount == 0` so với `GetRandomAData` | `fld_head.lua:175-181` không có chốt; `script\lib\file.lua:56-60` **có** `if (totalcount == 0) then return 0 end` | **ĐÚNG** | — |
| 15 | `GetTabFileHeight`/`GetTabFileData` **không phải** hàm engine | `grep -iw` trên `jx_linux_y.luamap.full.txt` (1560 mục) = **0 hit** cho cả 2; chúng ở `script\lib\file.lua:38,46` | **ĐÚNG** | — |
| 16 | Toàn bộ địa chỉ ELF ở mục 7 | Kiểm 29/29 tên hàm ngẫu nhiên trong luamap: `OpenMission 0x081332F0`, `CloseMission 0x081327E0`, `GetMSPlayerCount 0x081351F0`, `AddNpc 0x0811BB10`, `NewWorld 0x0811B690`, `TabFile_GetCell 0x0814A740`… **khớp tuyệt đối** | **ĐÚNG** | — |
| 17 | 2 tệp `.bak` là bản **bị tắt**; `fld_smalltimer.lua.bak:5` `do return en` là **lỗi cú pháp** | `mission.lua.bak:8` = `do return end`; `fld_smalltimer.lua.bak:5` = `do return en` (thiếu `d`, `return` không phải câu lệnh cuối khối ⇒ lỗi biên dịch); bản chạy `mission.lua:27,91` đã đổi sang `for i = 1, nCount` và `fld_smalltimer.lua:51-52` đã thêm `SetNpcDeathScript` cho 1692 | **ĐÚNG** | — |
| 18 | `jf0904_shuizei_IsActtime()` **luôn đúng** từ 28/04/2009 | `shuizei.lua:103-106` `return (nDate >= jf0904_act_dateS)`; `taskctrl.lua:23-24` `dateS=20090428`, `dateE=20090601` | **ĐÚNG** | — |
| 19 | Chết trên thuyền ⇒ **cập bến sớm** | `fld_death.lua:15-24` gọi `NewWorld(fld_landingpos(1|2|3))` — chính hàm dùng cho cập bến | **ĐÚNG** | — |
| 20 | `boss.lua` là code chết, `Include` 2 đường dẫn không tồn tại | grep phân biệt hoa/thường `thuytacdaulinh` toàn cây = **1 tệp duy nhất** (`boss.lua`, thân hàm `:19-24` comment hết); `script\global\` có **0 tên tệp phi-ASCII** ⇒ `路人_礼官.lua` không tồn tại; `script\global\tamhiep` **không tồn tại** | **ĐÚNG** | — |
| 21 | `settings\maps\中原北区\渡船\渡船刷怪点.txt` **THIẾU** | `os.walk` toàn `D:\ServerLinux\server1`: **0** tên tệp/thư mục chứa `渡船`, `刷怪点`, hoặc `中原北区` (tuy cây có 698 tên phi-ASCII khác) | **ĐÚNG** (nhưng bù được, xem **B4**) | — |
| 22 | Script NPC Thuyền phu **không có** trong cây | grep phân biệt hoa/thường `BOATID` = **1 tệp** (`fld_head.lua`); `fld_wanttakeboat` = **1 tệp** (`fld_head.lua`) | **ĐÚNG** (nhưng vị trí NPC tìm được, xem **B3**) | — |
| 23 | `settings\systemtimetask.txt` rỗng; `FLD_Start` ở `gmcommand_3.lua:854-857` | Tệp chỉ có 2 dòng: tiêu đề `HOUR MIN SCRIPT d DESC` + 1 dòng trắng; `gmcommand_3.lua:854-857` đúng nguyên văn (`Include` boat1 rồi `main()`) | **ĐÚNG** | — |
| 24 | 2 × `6,1,2743` khi cập bến, 2 × `6,1,30228` khi báo danh | `mission.lua:120` `GiveAwardByList({tbProp={6,1,2743,...}}, ..., 2)`; `script\lib\awardtype\item.lua:75` `nAmount = (nAmount or 1) * (tbItem.nCount or 1)` ⇒ 2×1 = **2**; `fld_head.lua:123-124` `nCount=2` với `Give(..., 1, ...)` ⇒ **2** | **ĐÚNG** | — |
| 25 | Task 3070 (≤5/ngày), 2817, 3105, 2863, temp 200 | `fld_head.lua:121-125`; `config\23\variables.lua:9` = 2817 + `config\23\config.lua:92-101` (2×`6,1,2566`); `config\1004\variables.lua:4` = 3105; `huoyuedu.txt:3` = `2 风陵渡船 2863 2 5 5`; `player_tmp_task_def.txt:90` = `200 记录当前是否可以变玩家阵营…` | **ĐÚNG** | — |
| 26 | `EventSys` type `FengLingDu` có đúng 4 sự kiện | `script\misc\eventsys\type\func.lua:22-28` — `OnBoard/OnLanding/OnFinishShuiZeiTask/OnShuiZeiDeath` | **ĐÚNG** | — |
| 27 | `NpcFunLib:CheckBoatBoss` chỉ nhận setting idx **725** | `script\activitysys\npcfunlib.lua:160-171` — `szIdList = "725"` | **ĐÚNG** | — |
| 28 | `closure3.json`: **44/82** tệp chưa có trong JX1; **13** tệp `fengling_ferry` đều chưa có | Đọc lại JSON: `len=82`, `in_jx1=False` đếm được **44**, nhánh `fengling_ferry` = **13** tệp, tất cả `in_jx1=False` | **ĐÚNG** (nhưng gây hiểu nhầm, xem **B5**) | — |
| 29 | Thành tựu 116/117/118 = 1/10/50 lần diệt Thủy Tặc | `settings\achievement\achievements.txt:117,118,119` khớp từng chữ | **ĐÚNG** (thiếu 1 dòng, xem **B8**) | — |
| **30** | 🔴 «`addr` **không được dùng** để chọn thuyền» | `fld_head.lua:46` `local MapId = boatMAPS[addr]`; `:47` `if (MapId <= 0)`; `:51` `SubWorldID2Idx(MapId)` | **SAI** | `addr` **có** dùng — để tra `boatMAPS[addr]` và kiểm tra subworld đã nạp (`:46-55`). Chỉ có **menu `Say`** (`:74-80`) là giống hệt nhau; việc lên thuyền mới do `BOATID` quyết (`:90-101`) |
| **31** | `mission.lua` «181 dòng» | Tệp thật kết thúc ở **dòng 153** (4191 byte) | **SAI** | **153 dòng** |
| **32** | `GetTabFileData` ở `file.lua:45`; `TabFile_Load` ở `:39,46`; `TabFile_GetRowCount` `:42`; `TabFile_GetCell` `:50` | `grep -n` trên `script\lib\file.lua`: **38** (GetTabFileHeight), **39** + **47** (TabFile_Load), **43** (GetRowCount), **46** (GetTabFileData), **51** (GetCell) | **SAI** (4 số dòng lệch 1) | `file.lua:46` / `:39,47` / `:43` / `:51` |
| **33** | `settings\waypoint.txt` **dòng 224** | Dòng 224 là `223 Thiên Long Tự`; `224 Phong Lăng độ 336, 1112, 3189` nằm ở **dòng 225** | **SAI** | **dòng 225** (id 224) |
| **34** | «7 nơi đăng ký nghe» sự kiện `FengLingDu` | `grep -c 'EventSys:GetType("FengLingDu"):Reg'` = **6** (achievementsys/fenglingdu.lua:10, translife_6.lua:215, translife_7.lua:338, beidouactivity.lua:478, func_prize_gs.lua:152, other_func_outputs.lua:174) — đúng bằng số dòng trong bảng 8.3 | **SAI** (số đếm) | **6 nơi** `Reg` |
| **35** | `G_ACTIVITY "FinishFengLingDu"` dùng ở config **1001,1002,1004,18,23,31,4,45** | `config\4\config.lua:95` = `szMessageType = "NpcOnDeath"`, `:96/:114` chỉ đặt `szName = "FengLingDu_Boss"/"FengLingDu_BigBoss"` — **không phải** message type | **SAI** | 7 thư mục: **1001,1002,1004(×2),18,23,31,45**; config 4 và 40 hook `NpcOnDeath`; 1003/1016 đã comment |
| **36** | `boss.lua` sinh 513 **và** 511 «tại `1636*32, 3221*32`» | `boss.lua:28` (513) = `1636*32`; `boss.lua:35` (511) = **`1632*32`** | **SAI một phần** | NPC 511 ở `1632*32, 3221*32` |
| **37** | `exchange_lingpai` «xác thực đúng genre/detail/particular» | `fld_head.lua:269` nhánh 1 kiểm `genre`+`detailtype`; `:274` nhánh 2 kiểm `genre`+`particular` — **không** kiểm `detailtype` | **SAI một phần** | Nhánh `2745` chỉ kiểm `itemgenre==6 and particular==2745` |
| **38** | Task **1765** = «số lần còn được nhận nhiệm vụ Thủy tặc trong ngày» | `taskctrl.lua:8` chú thích `上次交水贼任务的时间` = **thời điểm giao nhiệm vụ lần trước**; `taskctrl.lua:49` đặt lại `= 1` khi sang ngày mới | **SAI** (mô tả) | «thời điểm/lần giao nhiệm vụ gần nhất, đặt lại =1 mỗi ngày» |
| **39** | 🔴 «Lệnh GM `FLD_Start` ⇒ **không mở đủ 3 thuyền**» (kết luận vận hành) | Mã nguồn ĐÚNG như mô tả, nhưng log chạy thật bác bỏ kết luận: `Logs\KSG_ScriptOutputLog_20260806.txt:139826-139830` = 2 dòng `ExecuteCode …fldmap_boat1.lua` rồi **3 dòng liên tiếp** `CallFun …mission.lua InitMission`; sau đó `fld_smalltimer.lua OnTimer` luôn đi **cụm 3** (tổng **150** lần = 50 nhịp × 3 thuyền) | **SAI** (ở mức vận hành) | Lỗi `main()` là thật nhưng **chỉ ảnh hưởng đường lệnh GM**; đường vận hành thật gọi `fenglingdu_main()` và **mở đủ 3 thuyền** |
| **40** | Phía JX1: «`header\phonglangdo.lua` … **mission id 1, chỉ 1 map 337**» + «13 tệp `fengling_ferry` đều chưa có» ⇒ ngụ ý JX1 gần như trắng | `header\phonglangdo.lua` **là tệp mồ côi**: không tệp nào `Include` nó (grep toàn `bin\server\script`), hàm nó định nghĩa là `release_npcphonglang` còn `timertask\task04.lua:54` gọi `release_npcphonglang**do**` (định nghĩa ở `tinhnang\phonglangdo\lib_phonglangdo.lua:159`), và `DROP_PHONGLANG`/`DIE_PHONGLANG` (`:51-52`) trỏ `\script\feature\phonglangdo\` — **thư mục không tồn tại**. Bản JX1 đang sống là **8 tệp** `script\tinhnang\phonglangdo\`, `MS_PLANGDO = 4` (`lib\lib_task.lua:285`), `MAP_DUATHUYEN_PLD = {337,338,339}` (`lib_phonglangdo.lua:108-112`) | **SAI / lệch trọng tâm** | Xem **B5**, **B6** |

### B. Bỏ sót đã tìm thêm

**B1. 🔴🔴 LỖI THẬT CHƯA AI NÊU — `fld_TakeBoat` không khôi phục `SubWorld`/`PlayerIndex` ở 2 nhánh thoát sớm.**
`fld_head.lua:103-111`:

```lua
103  oldsubworldindex = SubWorld
104  SubWorld = SubWorldID2Idx(boatmapid)     -- đã nhảy sang subworld thuyền
105  if (fld_haveroom() == 1) then
106      return 0                              -- <-- KHÔNG khôi phục SubWorld, KHÔNG khôi phục PlayerIndex
107  end
108  t = 10 - GetMissionV(MS_TIMEACC_1MIN)
109  if (t <= 0) then
110      return 0                              -- <-- y hệt
111  end
```
Chỉ nhánh thành công mới khôi phục (`:147-148`). Ngoài ra `orgplayerindex` (`:86`) và `oldsubworldindex` (`:103`)
đều là **biến toàn cục** (không `local`), nên hai lần gọi lồng nhau sẽ đè lên nhau.
⇒ Khi thuyền đầy hoặc quá giờ báo danh, biến toàn cục `SubWorld`/`PlayerIndex` của máy ảo Lua **vẫn trỏ vào thuyền**
sau khi trở về `exchange_lingpai`/`exchange_juanzhou` — mọi lệnh sau đó (`Say`, `RemoveItemByIndex`, log) chạy sai ngữ cảnh.
**Khi port sang JX1 phải khôi phục ở cả 3 đường ra.**

**B2. 🔴 LỖI THẬT CHƯA AI NÊU — `fld_wanttakeboat` gọi `fld_haveroom()` TRƯỚC khi đổi `SubWorld`.**
`fld_head.lua:45` khai `local orgworld = SubWorld` rồi **không dùng lần nào** (biến chết); `:56` gọi thẳng
`fld_haveroom()` trong khi `SubWorld` vẫn là subworld người chơi đang đứng (bờ báo danh).
Mà `fld_haveroom()` (`:152-165`) đọc `GetMSPlayerCount(MISSIONID, 1)` — dữ liệu mission là **per-subworld** (xem #12).
⇒ Chốt "thuyền đã đầy" **trong hộp thoại** đếm nhầm subworld; chỉ chốt thứ hai trong `fld_TakeBoat` (`:105`, sau khi đã
đổi `SubWorld` ở `:104`) mới đếm đúng. Đây là lý do người chơi thấy menu mở bình thường rồi mới bị đá ra.

**B3. ✅ TÌM ĐƯỢC vị trí 3 NPC Thuyền phu — thân bài coi là "không có trong cây".**
`script\gmscript\gmcommand_3.lua:829-834`:
```lua
function tbCommand3:FLD_BaoMingNpc(nChoice)
    local tbPos = {
        {"船夫一", 336,1154,3026},
        {"船夫二", 336,1282,2906},
        {"船夫三", 336,1542,2811};
        };
```
(`:849-850` `LoadMap(nMapId); NewWorld(nMapId, nX, nY)`, hộp thoại `:836` = "chọn muốn dịch chuyển đến bên thuyền phu nào").
⇒ **Ba Thuyền phu báo danh đứng trên chính bản đồ 336**, tại ô (1154,3026) / (1282,2906) / (1542,2811),
ngay cạnh 3 điểm đổ bộ (1158,2964) / (1343,2868) / (1482,2796).
🔴 **Hệ quả sửa kiến trúc:** "bờ Nam" và "bờ Bắc" **cùng nằm trên bản đồ 336** (một bản đồ, hai bờ sông) —
không phải hai bản đồ khác nhau như mục 0/2.1 gợi ý. `settings\waypoint.txt:225` (336, 1112, 3189) và
`map_type.txt:14` (`FENGLING 336,341,342`) đều nhất quán với điều này. **Port JX1 chỉ cần 1 bản đồ bờ + 3 subworld thuyền.**

**B4. ✅ TÌM ĐƯỢC dữ liệu bù cho `渡船刷怪点.txt` — không cần đoán nữa.**
Bảng điểm sinh của bản JX1 chứa đúng loại dữ liệu đang thiếu:
- `E:\…\bin\server\script\header\phonglangdo.lua:2-45` — `NPC_PHONGLANG` **44 điểm (pixel)** + `:47-50` `BOSS_PHONGLANG` **2 điểm**.
  Đo lại: pixel X 50310..53021, Y 101372..104080 ⇒ **ô X 1572..1656, ô Y 3167..3252**.
- `E:\…\bin\server\script\tinhnang\phonglangdo\lib_phonglangdo.lua:28+` — `TAB_QUAIVUOTAIPLD`, cùng vùng, lưu bằng **ô**.
⇒ Ước lượng của mục 4.3 ("ô 1570..1670, 3160..3260") **được xác nhận là bao trùm đúng vùng thật**, nhưng nay có
**số liệu chính xác** để dựng lại bảng. Xác nhận đơn vị độc lập: `script\activitysys\config\32\talkdailytask.lua:27`
gọi `AddNpc(..., tbPos.nX * 32, tbPos.nY * 32, ...)` với dữ liệu `talk_pos.txt` (ô 1571..1665, 3163..3254) — cùng vùng.

**B5. 🔴 JX1 đã có Phong Lăng Độ ĐANG CHẠY, chỉ khác đường dẫn — "13/13 tệp chưa có" là đúng-mà-gây-hiểu-nhầm.**
`E:\…\bin\server\script\tinhnang\phonglangdo\` có **8 tệp**: `lib_phonglangdo.lua`, `thuyenphu.lua`, `thuyenphubac.lua`,
`quaipld.lua`, `bossthuytacdaulinhpld.lua`, `dietrenthuyen.lua`, `drop.lua`, `logout.lua`;
được `Include` từ `startgame.lua:11`, `timerserver.lua:16`, `timertask\task04.lua:6`, `task05.lua:6`, `task10.lua:9`,
`missions\mission04.lua:7`, `item\lenhbaiadmin.lua:17`.
**Bảng đối chiếu số hiệu (bắt buộc đọc trước khi port — nếu bê thẳng số của bản Linux là hỏng):**

| Khái niệm | Bản Linux (JX2) | Bản JX1 đang chạy | Nguồn JX1 |
|---|---|---|---|
| Mission id | 15 | **4** (`MS_PLANGDO`) | `lib\lib_task.lua:285` |
| Danh sách map thuyền | `{337,338,339}` | `{337,338,339}` **đã khai đủ 3** | `lib_phonglangdo.lua:108-112` |
| Số thuyền thật sự sinh quái | 3 | **1** (nhánh 338/339 bị comment) | `lib_phonglangdo.lua:168-176` |
| NPC quái thường | **724** | **725** | `lib_phonglangdo.lua:163` |
| Mật đồ thần bí | `6,1,**196**` | `6,1,**195**` | `lib_phonglangdo.lua:19` |
| Lệnh Bài Thủy Tặc | `6,1,**2745**` | `6,1,**3363**` | `lib_phonglangdo.lua:18` |
| Truy công lệnh | `6,1,**2015**` | `6,1,**2024**` | `lib_phonglangdo.lua:20` |
| Lệnh bài Phong Lăng Độ | `4,489` | `4,489` (**giống nhau**) | `lib_phonglangdo.lua:17` |
| Số cuốn mật đồ | 200 | 200 (**giống nhau**) | `lib_phonglangdo.lua:21` |
| Sức chứa | 100 | 100 (**giống nhau**) | `lib_phonglangdo.lua:24` |
| Giờ tốn phí | {10,14,16,18,20} | {10,14,16,18,20,**22**} | `lib_phonglangdo.lua:130-139` |
| Báo danh / toàn trận | 10 ph / 39 ph | **9 ph** / **30 ph** | `lib_phonglangdo.lua:7-8` |
| Mốc boss | T+25/30/35 | **{20,25,30}** phút | `lib_phonglangdo.lua:114` |
| NPC báo danh | *(thiếu trong cây Linux)* | **6 NPC** res 240: 3 "Thuyền phu Nam ất/bính/giáp" + 3 "Thuyền phu Bắc …" | `lib_phonglangdo.lua:147-157` |

⇒ Quyết định port **không phải** "thêm mới vào chỗ trống" mà là **"thay thế / hợp nhất một tính năng đang chạy"**.

**B6. 🔴 `header\phonglangdo.lua` của JX1 là tệp mồ côi + 2 đường dẫn chết.**
Không tệp nào `Include` nó; hàm nó khai là `release_npcphonglang` trong khi `timertask\task04.lua:54` gọi
`release_npcphonglangdo` (khác tên, định nghĩa ở `lib_phonglangdo.lua:159`); `:51-52` trỏ
`\script\feature\phonglangdo\drop.lua` và `\…\die.lua` — thư mục `script\feature\phonglangdo` **không tồn tại**
(thư mục thật là `script\tinhnang\phonglangdo`). Dùng tệp này làm mốc so sánh JX1 là sai mốc.

**B7. Thiếu bên PHÁT của sự kiện `OnFinishShuiZeiTask`.**
Bảng 8.3 chỉ liệt kê bên NGHE. Bên phát là `script\event\jiefang_jieri\200904\shuizei\shuizei.lua:79`
`EventSys:GetType("FengLingDu"):OnPlayerEvent("OnFinishShuiZeiTask", PlayerIndex)`.
⇒ **3 thành tựu 116/117/118 phụ thuộc script sự kiện 2009**; nếu port mà bỏ `shuizei.lua` thì thành tựu chết theo.
(Ghi chú: `fld_head.lua:140` và `mission.lua:108` dùng **hai API khác nhau** — `OnPlayerEvent` cho `OnBoard`,
`OnEvent` cho `OnLanding` — cần giữ đúng khi port.)

**B8. Thiếu thành tựu thứ 4.** `settings\achievement\achievements.txt:120` = id **119 "Vì Dân Diệt Hại"**,
"Lần đầu tiên tiêu diệt Thủy Tặc Đầu Lĩnh" (nhóm 16, 5 điểm). Bảng 8.1 chỉ ghi 117-119.

**B9. Quy mô liên đới NPC 1692 lớn hơn báo cáo nhiều.**
Thân bài chỉ nêu `config\40` và `config\45`. Thực tế grep `1692` trong `script\` = **75 dòng đang bật / 81 dòng / 51 tệp**,
gần như toàn bộ qua `{"NpcFunLib:CheckId", {"1692"}}` với `szMessageType = "NpcOnDeath"`.
Kết hợp với #27: **hai đường đếm tách biệt** — 725 đi qua `CheckBoatBoss`, 1692 đi qua `CheckId`. Port thiếu một trong hai là mất thưởng.

**B10. Bản đồ 336 tự sinh Golden NPC — mục 2.1 bỏ qua.**
`settings\maplist.ini:2912-2923`: `336_NpcSeriesAuto=1`, 5 dòng `NpcSeries*=20`, `336_NpcAutoLevelFlag=1`,
`NpcAutoLevelMin/Max = 95`, `336_AutoGoldenNpc=0`, `336_GoldenType=16`,
`336_GoldenDropRate=\settings\droprate\goldennpc\90_01234droprate.ini`.
⇒ "bản đồ đánh bảo" có **bảng rơi + cấu hình sinh quái riêng của engine**, không nằm trong script — phải port kèm.
(337/338/339 chỉ có `NpcSeriesAuto` + 5 dòng `NpcSeries*`, **không** `MapType`, **không** `NewWorldScript`.)

**B11. `revivepos.ini` còn mục [336] mà thân bài không nêu.**
`settings\revivepos.ini:214-216` `[336] region=1,1 → 35584, 102048` (điểm hồi sinh của chính bản đồ bờ);
mục [175] Tây Sơn thôn ở `:218-220` như báo cáo. Cả hai đều cần khi port.

**B12. Tên 5 vật phẩm còn bỏ trống trong bảng 8.2 / 4.1** (`settings\item\004\magicscript.txt`):
`6,1,2566` = **Hành Hiệp Lệnh** (dòng 2578); `6,1,122/123/124` = **Phúc Duyên Lễ (Tiểu/Trung/Đại)** (dòng 124-126) —
đây là 3 trong 9 mục của bảng rơi boss; `6,1,196` = Mật đồ thần bí (dòng 198); `6,1,2743` = Bảo Rương Thủy Tặc (dòng 3350);
`6,1,2745` = Lệnh Bài Thủy Tặc (dòng 3352). (Đã đối chiếu cả 12 particular trong bảng 8.2 — không sai mục nào.)

**B13. `pActivity:IsPaidBoat()` KHÔNG phải bản sao y hệt `check_new_shuizeitask()`.**
`config\1004\extend.lua:49` trả **`nil`** khi ngoài giờ, còn `fld_head.lua:315` trả **`0`**.
Nơi gọi so `== 1` thì tương đương, nhưng nơi nào so `== 0` sẽ lệch. Bảng giờ thì giống hệt (10/14/16/18/20).

**B14. Lệch dữ liệu (vô hại) ở NPC nhiệm vụ ngày.**
`settings\task\dailytask\talk_pos.txt:2-13` ghi cột `TaskId = 4` cho 12 dòng của 337/338/339, trong khi
`talk.txt:4` cho biết TaskId **3** = "Thuyền Phong Lăng Độ / Tiểu Chiêu" còn TaskId 4 = "Thiên Bảo Khố / Tiêu Trấn".
Vô hại vì `talkdailytask.lua:30` `--SetNpcParam(nNpcIdx, 1, tbPos.nTaskId)` **đã bị comment** — nhưng nếu port mà bật lại
dòng đó thì NPC Tiểu Chiêu sẽ mang nhiệm vụ sai.

**B15. Chứng cứ độc lập cho đơn vị "ô" trên bản đồ thuyền.**
`script\activitysys\config\32\talkdailytask.lua:27` `AddNpc(tbPos.nNpcRes, 1, SubWorldID2Idx(nMapId), tbPos.nX * 32, tbPos.nY * 32, ...)`
⇒ `talk_pos.txt` lưu **ô**, `AddNpc` nhận **pixel** — khẳng định #11 của mục 10 (AddNpc = pixel) được xác nhận từ một đường mã hoàn toàn khác.

### C. Ghi chú về hướng suy diễn của mục 9

Mục 9 kết luận đúng "bản `.bak` không được nạp". Nhưng cách trình bày diff `.bak → bản chạy` ngụ ý bản chạy sinh ra **sau**,
trong khi **thời gian sửa tệp thì ngược lại**: `mission.lua`/`fld_smalltimer.lua` = **2021-10-30**, hai tệp `.bak` = **2022-04-03**.
Giải thích khớp cả hai dữ kiện: `.bak` được tạo **năm 2022 từ một bản CŨ HƠN** (chưa có `GetMSPlayerCount`, chưa có
`SetNpcDeathScript` cho 1692) rồi chèn `do return end` để tắt — tức `.bak` **không phải** ảnh chụp của bản đang chạy.
Không ảnh hưởng kết luận, nhưng khi port **không được** coi `.bak` là "phiên bản kế tiếp".
