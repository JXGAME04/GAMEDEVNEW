# BÀN GIAO 02/09/2026 — (1) BOT CHƯA VÀO BANG: GỐC LỖI · (2) CÔNG THÀNH CHIẾN BẢN JX2 HOẠT ĐỘNG THẾ NÀO · (3) SƠ ĐỒ BOT THAM GIA CÔNG THÀNH

> Chủ game giao 3 việc (phiên wauto, cwd WAuto):
> 1. *"bot chưa tự vào bang hội khi tôi chat mật cho bot — cần kiểm tra phần xin vào bang hội"*
> 2. *"đọc và phân tích tính năng công thành chiến cách hoạt động từng bước"*
> 3. *"lên sơ đồ để viết bot tham gia công thành chiến"*
>
> "Bot" ở đây = **bot KPlayer phía máy chủ** (`Sources\Core\Src\KPlayerBot.cpp`, cùng hệ với bot Tống Kim / Dã Tẩu / xin bang),
> KHÔNG phải WAuto của người chơi. Mọi dòng dưới đều trỏ tới tệp:dòng đã đọc thật; **chưa sửa gì** (chỉ phân tích + đề xuất, chờ chủ duyệt).
> Cây chạy thật: `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server`; mã: `D:\GAMEDEVNEW`.

---

## 1. VIỆC 1 — BOT NHẬN PM NHƯNG KHÔNG VÀO BANG: GỐC = `TASK_DUNGCHUNG2` LÀ `nil` TRONG SCRIPT NPC MÔN PHÁI

### 1.1 Bằng chứng trong `bot.log` (bản đang chạy `CoreServer.dll` md5 `6c6ad9b5`, khởi động 14:10:50 hôm nay, ĐÃ chứa mã đợt f — đếm chuỗi `BO QUA loi ru vao bang` / `da XUAT SU` / `nop don` trong DLL đều > 0)

```
[01/09 19:23:27] [BotBang] PhanVu856   nhan PM ru vao bang TESTGAME (id 441122550) tu CaiBang
[01/09 19:23:27] [BotBang] PhanVu856   dich chuyen ve map 53 canh NPC Thien Vuong de xin vao bang (ret=1)
[01/09 19:23:27] [BotBang] PhanVu856   goi 'xuatsu' cua Thien Vuong KHONG an (task 4134 = 0) -> HUY
[01/09 19:25:24] [BotBang] HoangMinh903 ... goi 'xuatsu' cua Nga Mi KHONG an (task 4134 = 0) -> HUY
[02/09 10:51:28] [BotBang] DinhNhat610 nhan PM ru vao bang TESTGAME (id 441122550) tu CaiBang
[02/09 10:51:28] [BotBang] DinhNhat610 dich chuyen ve map 53 canh NPC Con Lon de xin vao bang (ret=1)
[02/09 10:51:28] [BotBang] DinhNhat610 goi 'xuatsu' cua Con Lon KHONG an (task 4134 = 0) -> HUY
```

Ba lần thử (3 phái khác nhau) đều chết **đúng một chỗ**: bước 4 XUẤT SƯ. Các bước trước (nhận PM đúng bang chủ, cấp ≥ 90, rời nhóm, dịch chuyển map 53, tới NPC) đều chạy. Sau 14:10 (restart) chưa có lần thử nào. Không có dòng nào trong `ScriptError.log` / `GameServer.log` lúc đó — lỗi này **im lặng** theo thiết kế Lua 4.0.

### 1.2 Đường mã (lần ngược từng bước, không đoán)

| # | Ở đâu | Gì xảy ra |
|---|---|---|
| 1 | `KPlayerBot.cpp:7670` | `Player[nIdx].ExecuteScript(s_facNpc[nFac].szScript, "xuatsu", 0, false)` — gọi hàm `xuatsu` trong script NPC môn phái (vd `\script\npcthon\npcmonphai\conlon.lua`) |
| 2 | `KPlayer.cpp:7172-7262` | Lấy `KLuaScript` của tệp đó (`g_GetScript`), đặt `SCRIPT_PLAYERINDEX` = bot, `CallFunction("xuatsu")`. **Mỗi tệp `.lua` là MỘT Lua state riêng** (`Engine\Src\KLuaScript.cpp:445 Lua_Create(0)`); `Include(...)` = `lua_dofile` vào đúng state đó (`ScriptFuns.cpp:2054`), không có "header nạp sẵn" nào cho mọi state |
| 3 | `conlon.lua:5-7` | Include `header\factionhead.lua`, `lib\lib_task.lua`, `npcmonphai\FactionHelper.lua` — **không tệp nào Include `header\taskid.lua`** (3 tệp đó cũng không Include gì thêm) |
| 4 | `factionhead.lua:34-40` | `xuatsu()`: `SetCamp(4); SetCurCamp(4); LeaveTeam(); SetTask(TASK_DUNGCHUNG2, 1)` |
| 5 | `script\header\taskid.lua:26` | `TASK_DUNGCHUNG2 = 4134` — **định nghĩa DUY NHẤT trong toàn cây**; chỉ 14 tệp Include `taskid.lua`, không có tệp nào trong 13 script môn phái + factionhead.lua |
| 6 | Lua 4.0 `lapi.c:157-160` | `lua_tonumber(nil)` trả **0** |
| 7 | `ScriptFuns.cpp:2670-2684` `LuaSetTaskValue` | `SetSaveVal(0, 1)` → **ghi vào task 0**, task 4134 vẫn 0 |
| 8 | `KPlayerBot.cpp:7671` | `GetSaveVal(4134) != 1` → log "KHONG an" → HUỶ toàn bộ (không tới bước 5 nộp đơn) |

Vì sao người chơi thật **không thấy lỗi**: `conlon.lua:18` `GetTask(TASK_DUNGCHUNG2)` cũng đọc `nil` → task 0, và `trungphansumon` (`factionhead.lua:88`) cũng ghi task 0 — đọc/ghi **cùng lệch về ô 0** nên menu "Xuất sư xuống núi" ↔ "Trùng phản môn phái" vẫn đổi đúng. Bot là **người đầu tiên đọc ô 4134 thật**. Lỗi có từ script gốc (bản sao `lib_task.lua.truoc_taskfix2_2408` cũng không định nghĩa hằng này), không phải do đợt sửa trùng task 24/08.

Đối chiếu thêm: `bot.log` không có dòng `nop don` nào từ trước tới nay → **bước 5 (`DoClientOpBody APPLY_JOIN`) chưa từng chạy thật** — sau khi sửa phải nghiệm thu tiếp bước này (điều kiện phía server `KTongJX2.cpp:575 sJX2_DoApplyJoin`: chưa có bang, cấp ≥ ngưỡng từ chối field 66; tự nhận nếu cấp ≥ field 65, không thì vào hàng đợi cho bang chủ duyệt; server KHÔNG kiểm camp 4 — chỉ client kiểm `KPlayerTong.cpp:190`).

### 1.3 Đề xuất sửa (CHỜ CHỦ DUYỆT — đụng script người chơi thật dùng)

**Phương án A (khuyến nghị):** thêm **dòng 1** vào `script\header\factionhead.lua`:

```lua
Include("\\script\\header\\taskid.lua")   -- [BOTBANG 02/09] TASK_DUNGCHUNG2 (4134) truoc day nil -> xuatsu ghi nham task 0
Include("\\script\\lib\\lib_task.lua")    -- (dong cu, giu nguyen)
```

- Đặt **TRƯỚC** `lib_task.lua` vì 3 hằng trùng tên khác giá trị (`TASK_NVST` 21/355, `TASK_TANTHU` 22/320, `TASK_THOIGIAN9` 52/321): Include sau ghi đè Include trước → giá trị của `lib_task.lua` **vẫn thắng như hiện nay**, hành vi cũ không đổi; chỉ có `TASK_DUNGCHUNG2` (chỉ `taskid.lua` có) thành 4134. `taskid.lua` = 73 dòng hằng số, không hàm, không Include → không tác dụng phụ khác. Cả 13 script môn phái đều Include `factionhead.lua` nên sửa **một chỗ** là đủ.
- Hệ quả với người chơi thật đã "xuất sư" từ trước (ô 0 = 1, ô 4134 = 0): menu **"Xuất sư xuống núi" hiện lại một lần**; bấm lại vô hại (SetCamp(4) + SetTask(4134,1)). Ai đã "trùng phản" thì không đổi gì. Ô 0 bị bẩn cũ để nguyên (không script nào dùng task 0 có chủ đích; `station_ctc.lua:9 TV_VALUE = 0` là quirk gốc đã ghi).
- Cần **restart** (script NPC nạp lúc boot). Restart này gộp với đợt swap `.moi` đang chờ (`CoreServer.dll.moi` `e0e57d73` 15:31 của phiên Vũ Hồn/Tiêu Dao).
- Cách áp: `safe_edit.py` (ASCII, không đụng TCVN3) trên cây `bin\server\script\header\factionhead.lua` + chép mirror `D:\GAMEDEVNEW\serverscript_jx2\jx1_edits\factionhead.lua` + commit. Tôi làm khi chủ gật.

**Phương án B (không khuyến nghị):** bot chấp nhận "đã xuất sư" khi `m_CurrentCamp == 4` thay vì đọc task — che lỗi script, người chơi vẫn ghi ô 0 mãi.

### 1.4 Nghiệm thu sau restart (bang chủ TESTGAME nhắn mật `vao bang` cho bot ≥ 90)

```
grep -a "BotBang" bot.log | tail -20
```
Kỳ vọng: `nhan PM` → `dich chuyen ve map 53` → **`da XUAT SU tai NPC ... (camp -> 4)`** → 2 giây → **`nop don vao bang TESTGAME ... ket qua=0|7`** (7 = vào thẳng, 0 = đơn chờ bang chủ duyệt ở trang Chiêu mộ; 11 = dưới ngưỡng field 66; 12 = đã nộp; 5 = lỗi/đã có bang). Vào bang xong ~10 giây bot đổi màu + tên bang (`doi mau theo bang`).

⚠️ Liên quan việc 3: vào bang hôm nay thì `GetJoinTongTime()` (KJx2WarInfra.cpp:638, đọc field 2 = `time(NULL)` lúc được nhận, KTongJX2.cpp:598/2456/3087) tính từ hôm nay → xem mục 3.1 Q2.

---

## 2. CÔNG THÀNH CHIẾN (bản JX2, `BAT_CTC_JX2 = 1`) — HOẠT ĐỘNG TỪNG BƯỚC

Hệ cũ Việt (`script\tinhnang\congthanhchien\`, `script\maps\congthanhchien\`) đã **gỡ 30/08** (không còn ai trỏ tới, timerserver.lua:139). Bản đang chạy = port từ `D:\ServerLinux` (đợt E 14/08 + lôi đài CN 21/08 + cấu hình 30/08).

### 2.1 Bản đồ linh kiện

| Tầng | Tệp | Vai trò |
|---|---|---|
| Cấu hình | `settings\citywar.ini` | 7 thành: 1 Phượng Tường (map 1) · 2 Thành Đô (11) · 3 Đại Lý (162) · 4 Biện Kinh (37) · 5 Tương Dương (78) · 6 Dương Châu (80) · 7 Lâm An (176); `SignUpFee=1000000`, `MinTongLevel=18`, `MinTongCrowNumber=37`, `MaxExchangeTax=20`, thuế 22h-23h |
| Trạng thái | `settings\jx2citywar.txt` | mirror 7 thành (`C id state tax pp ngày`, `O/M/H` chủ/Thái Thú/khiêu chiến; `A/F/B/R` lôi đài). **Hiện tại: cả 7 thành state 0, KHÔNG CÓ CHỦ** (city 4 còn ngày 20260814 của lần test cũ) |
| Engine | `KJx2CityWar.cpp` (1834 dòng) | state 7 thành + lôi đài 16 bang + 4 pha (`StartSignUp/EndSignUp/StartArena/StartCityWar`), `NotifyWarResult`, đổi chủ + Title Thái Thú, thuế |
| | `KJx2WarInfra.cpp` (2755) | hạ tầng trận: `SetNpcDeathScript`, vật cản động `AddObstacleObj/Clear`, `SetPKFlag/ForbidChangePK`, `GetJoinTongTime`, lệnh bài `GetItemCountEx/DelItemEx/GetItemLife`, `GiveItemUI`, `SetSiegeVoitureParam/SetMangonelParam` |
| | `KJx2Battle.cpp` (368) | `BT_*` điểm/bảng xếp hạng (task 2895 + 702..750) |
| | `KJx2League.cpp` (1458) | league 508/509 (báo danh thành), 538 (`tiaozhanling` — kho Khiêu chiến lệnh của bang) |
| Lịch | `script\timerserver_ctc.lua` (Include từ `timerserver.lua:41`, gọi `CTC_JX2_Tick` mỗi phút tại `:136`) | `TB_CTC6 = {{3,4},{1,2},{2,3},{5,6},{4,5},{0,1},{6,0}}` (thứ báo danh, thứ đánh; `%w` 0=CN) |
| Poll | `missions\citywar_global\mission.lua` + `timer.lua` (global mission 8, timer 18, 5 phút) | `HaveBeginWar(i)` → `OpenMission(7)` map 221; `IsArenaBegin(i)` → `OpenMission(9)` map 213+i |
| NPC | `startgame\citywar_boot.lua` (gọi từ `startgame.lua:61`) | spawn **Sứ Giả Công Thành** (tpl 373) map 53 ô (1625,3170) script `sugia_congthanh.lua` → `ArenaMain` (`infocenter_head.lua:145`); 2 NPC camper trên 8 map lôi đài 213-220. Log boot 14:11:58 hôm nay: `league 4/508/509 + GlbMission 8 OK, NPC infocenter=90167` |
| Lối vào | `global\npcchucnang\xaphu.lua:23` "Đi Chiến trường công thành/GoCityWar" → `global\station.lua:13` Include `missions\citywar_global\station_ctc.lua` | `GoCityWarAttack/Defend` → `NewWorld(223/222, ...)` |
| Trận | `missions\citywar_city\*.lua` (map 221 + hậu phương 222 thủ / 223 công) | xem 2.4 |
| Lôi đài | `missions\citywar_arena\*.lua` (map 213-220, mission 9) | xem 2.3 |
| Map | `Maps\WorldSet_GameServer.ini` World195-205 = 213…223; `MapList.ini` 221 `特殊用地\剑门关vn`, 222/223 `特殊用地\剑门关中转` | đã nạp (boot ghi "NPC loi dai tren 8 map") |
| GM | `script\test\citywar_e3.lua` (qua lệnh bài admin → `bangthanh_f.lua`) | `E3Appoint` (bổ nhiệm chủ = bang mình, đứng trong thành) · `E3Challenge` · `E6Force18/19/20/0` · `E5GiveLenh` (10 Khiêu chiến lệnh) · `E5Goto221` · `E3WinAttack/WinDefend` |

### 2.2 Lịch tuần + đồng hồ 4 pha (giờ máy GameServer)

| Thành | 18h mở báo danh | 20h cùng ngày | 20h hôm sau |
|---|---|---|---|
| 1 Phượng Tường | **T4** | lôi đài (nếu ≥ 2 bang) | **T5** công thành |
| 2 Thành Đô | T2 | | T3 |
| 3 Đại Lý | T3 | | T4 |
| 4 Biện Kinh | T6 | | T7 |
| 5 Tương Dương | T5 | | T6 |
| 6 Dương Châu | CN | | T2 |
| 7 Lâm An | T7 | | CN |

Hôm nay **T4 02/09** → 18h mở báo danh **Phượng Tường**, đánh T5 20h. (Đại Lý "đánh" hôm nay nhưng không có khiêu chiến giả → không có gì.)

1. **0h-6h** (`ctc6_daily0h`, 1 lần/ngày): thành còn treo state 5 → `NotifyWarResult(i,0)` (thủ thắng mặc định); xoá + tạo lại league 508/509.
2. **18h ngày báo danh** (`ctc6_startsignup`): xoá ladder 10261, league 508 task1 = 1 (cờ mở menu NPC), C++ `StartSignUp(i)`: xoá dữ liệu lôi đài cũ, **state 1 SIGNUP**, tin toàn server. **18h00-18h59 tại Sứ Giả Công Thành** (menu 8 mục `infocenter_head.lua:148`):
   - **"Báo danh đấu thầu Lôi đài bang hội/SignUpTheOne"** → nhập phí 1.000.000..99.999.999 → `SignUpCityWarArena(city, fee)` (C++ `sSignUpArena` :1032, 10 cửa: có bang · **là bang chủ** · state SIGNUP · chưa báo danh thành khác · không đang là khiêu chiến giả · không là chủ thành nào · bang **≥ 37 người** · **cấp bang ≥ 18** (field 6) · phí ≥ 1.000.000 và quỹ bang đủ → trừ quỹ) → vào danh sách đấu thầu. **Đây là đường DUY NHẤT quyết định khiêu chiến giả.**
   - "Báo danh công thành chiến/SignUpCityWar" + "Ta đến giao lệnh bài/GiveTiaoZhanLing" = đường **Khiêu chiến lệnh** (item 6,1,1508; +50.000 exp/lệnh, trần 300/ngày/người, kho bang league 538) — nộp vẫn được nhưng **không còn quyết định ai được khiêu chiến** (`timerserver_ctc.lua:117-124`: 19h gọi `EndSignUp` C++ rồi `return`, nhánh `ctc6_randomchallenger` bên dưới là mã chết). Nguồn phát 1508 chỉ thấy lệnh bài GM (`E5GiveLenh`) + bảng `codenew.lua:6029` (mã code).
3. **19h** (`ctc6_endsignup` cho cả 7 thành; thành không tới lịch chỉ hạ cờ): C++ `EndSignUp(i)` (:919): **0 bang** → state 0 "vạn sự thái bình"; **1 bang** → **state 4 WARDECIDED**, khiêu chiến giả = bang đó (field 11 = 3); **≥ 2 bang** → cắt còn ≤ 16 theo phí giảm dần (hoàn phí bang bị cắt), bốc thăm bảng đấu (`sBuildBracket`), **state 2 BRACKET**.
4. **20h ngày báo danh** (`ctc6_startarena` → `StartArena` :958): cần state 2 → **state 3 ARENA**, ghép cặp vòng 1 (ArenaID 0..7 = map 213..220) → lôi đài (2.3) → vô địch = khiêu chiến giả, state 4.
5. **20h ngày đánh** (`ctc6_startwar` → `StartCityWar` :988): cần có khiêu chiến giả. **Thành vô chủ → khiêu chiến giả CHIẾM NGAY không đánh** (tin WAR_RESULT3, Title Thái Thú `152+city` cho bang chủ, field 48 ngày chiếm, state 0). Có chủ → **state 5 ATWAR** (`HaveBeginWar = 1`), field 11 = 4 cho 2 bang → timer 18 (≤ 5 phút) `OpenMission(7)` + `RunMission(7)` map 221 → trận (2.4).

Hệ quả hiện trạng: cả 7 thành vô chủ → bang đầu tiên đấu thầu một mình sẽ **nhận thành ngày hôm sau mà không có trận** → muốn có trận thật cần 1 bang đã là chủ + 1 bang khiêu chiến tuần sau, hoặc ép bằng GM (`E3Appoint` + `E3Challenge` + `E6Force20`).

### 2.3 Lôi đài bang hội (20h ngày báo danh, chỉ khi ≥ 2 bang đấu thầu)

- Timer 18 thấy `IsArenaBegin(ArenaID)` (cặp chưa có kết quả) → `OpenMission(9)` map 213+ID → `citywar_arena\mission.lua InitMission`: MissionS(1)/(2) = 2 bang, timer 16 (20 s) + 17 (25 phút `CTLD_DODAI_TRAN_PHUT`).
- Vào: Sứ Giả "Tham gia Lôi đài bang hội/PreEnterGame" → bang mình đang có cặp → `EnterBattle(i)` → `NewWorld(213+i, 1633,3292)` → gặp NPC camper (spawn boot) → `OnJoin`: bang == MissionS(1|2) **và cấp ≥ 90** (chính sách chủ 21/08) → `JoinCamp` (≤ 16 người/phe `CTLD_MAX_NGUOI_MOI_PHE`), PK bật+khoá, death script `death.lua`, SetPos CampPos1 (1536,3223)/CampPos2 (1563,3195).
- **8 phút** vào (`GO_TIME` 480 s) → `RunMission`: `SetFightState(1)` cho mọi người → đánh. Chết → `DelMSPlayer` + về (1633,3292) ngoài sàn.
- Thắng: phe kia hết người (`timer.lua ReportBattle`) hoặc hết 25 phút phe còn **nhiều người hơn**; hoà → phe có **tổng cấp thấp hơn** thắng (`GetTotalLevel` quirk gốc) → `WinBonus` → `NotifyArenaResult` + `AddTongExp` 1200 → C++ `sAddArenaResultCore` đánh thua → `sBuildArenaPairs` vòng kế (timer 18 lại mở mission 9 **cùng đêm**) → hết cây → khiêu chiến giả.

### 2.4 Trận công thành (20h ngày đánh, map 221, 90 phút)

**Dựng trận** (`citywar_city\mission.lua InitMission`, cấu hình `head.lua`): `ClearMapNpc(221)`; 3 **Long trụ** NPC 528 cấp 10 tên `<bang thủ> + "Định Xuyên Trụ / Lập Dương Trụ / Bình Giang Trụ"` tại StonePos (55232,104928)/(53152,107040)/(50976,108992), camp 1, MissionV 15..17 = 1 (thủ giữ), death script `symboldeath1-3.lua`; 3 **cổng** NPC 532 cấp 60 "Định Xuyên môn / Lập Dương môn / Bình Giang môn" tại DoorPos (57472,106944)/(55296,109088)/(53184,111136), camp 1, `doordeath.lua`, MissionV MS_DOORBEGIN+i = idx; **vật cản động** 14 ô chéo sau mỗi cổng (`CreateObstacle` ObstaclePos (1793,3340)/(1727,3407)/(1660,3469)); 8 Tuỳ Quân dược Y (`yaoshang.lua`, 4 thuốc CTC 1307-1310); Địa đạo vệ thủ (48) ở (1597,3470) → mật đạo ra (1739,3513)/(1836,3405); 2 Rương chứa đồ (625); MissionS(1) = thủ, MissionS(2) = công; bảng điểm `BT_*`; timer 12 (loa mỗi 60 s) + 13 (90 phút); MS_STATE 1 → `RunMission` → **MS_STATE 2** + tin toàn server.

**Vào trận** (người chơi): Xa Phu thành → "Đi Chiến trường công thành" (`station_ctc.lua GoCityWar`): thành đang đánh + 2 bang → chọn **Bên công / Bên thủ**:
- `GoCityWarAttack/Defend`: là thành viên bang đúng phe, HOẶC có **Thành chiến lệnh bài** phe đó (`CardTab[city*2-1]` công / `[city*2]` thủ, event item 354-367), HOẶC task `TV_CITYID/TV_VALUE/TV_TASKID` khớp (đã từng vào bằng lệnh bài) → `NewWorld(223 công | 222 thủ, (1614,3172) hoặc (1629,3193))`.
- Trên hậu phương 222/223 đạp **trap** `zhongzhuan_map\trap.lua`: MS_STATE 0 → "đang tập hợp"; `CheckAndJoin(camp)`: bang == MissionS(camp) **và `GetJoinTongTime() ≥ 7200 phút (= 5 ngày trong bang)`** → reset điểm nếu key trận khác → **`JoinCamp(camp, 1)`** (trần **200/phe**); vào bang < 5 ngày → `JoinWithCard(camp,0)` → không lệnh bài → "Thời gian bạn gia nhập bang hội quá ngắn"; không phải bang → cần lệnh bài phe → trừ lệnh bài, ghi task TV_*, `JoinCamp(camp, 2)` (trần **50 liên minh/phe**). Lệnh bài: chỉ **bang chủ** phe đó mua ở Sứ Giả (`BuyCard`, 200.000 lượng/lệnh, ≤ 30/lần, "hạn 5 ngày" nhưng cây ta `GetItemLife` trả 0 → không hết hạn), trả lại 10.000/lệnh.
- `JoinCamp` (`camper.lua:97`): `LeaveTeam`; `AddMSPlayer(7, camp)`; `SetCurCamp(camp)`; `SetPKFlag(1)+ForbidChangePK(1)`; `SetPunish(0)`; `SetCreateTeam(0)`; `SetLogoutRV(1)`; death script `playerdeath.lua`; `SetTempRevPos` doanh; **`NewWorld(221, CS_CampPos1 (1533,3211) thủ | CS_CampPos2 (1903,3608) công)`**; `SetFightState(0)`; nạp bảng điểm.
- **Ra trận**: đạp trap cửa doanh `chengzhan_map\ctrap1(b,c).lua` (thủ) / `ctrap2(b,c).lua` (công): FightState 0 → `SetPos` (1571,3263) | (1866,3547) + **`SetFightState(1)`** + `bt_RankEffect`; đi ngược khi FightState 1 → về doanh (1565,3246) | (1875,3571), FightState 0; phe khác đạp → "phục kích", đẩy về. `trap1/trap2.lua` = ra khỏi trận về 222/223.
- **Chết** (`playerdeath.lua`, MS_STATE 2): kẻ giết khác camp → +1 PK, liên trảm (+150 điểm mỗi 3 mạng liên tiếp), điểm giết = 75 × hệ số rank (`RANK_PKBONUS`), tin trận; nạn nhân +1 bị giết, liên trảm về 0; hồi sinh tại doanh (`SetTempRevPos`) với FightState 0 → phải đạp trap ra lại.

**Luật thắng**:
- **Cổng** (camp 1, kind_normal → phe công là địch theo `KNpcSet::GenOneRelation`: khác camp + có kind_normal = enemy; phe thủ = đồng minh không đánh được) chết → `doordeath.lua`: MissionV cổng = 0, **`ClearObstacle` dải vật cản tương ứng** (mở lối), xoá xe công thành gắn cổng.
- **Trụ** chết → `symboldeath.lua`: đổi chủ 1↔2, tin toàn server, tạo lại trụ NPC 528 (thủ, cấp 10) | 530 (công, cấp 20) với camp mới. `CheckWin` = số trụ công giữ; **3/3 → công thắng ngay** (`GameWin(2)`).
- Hết 90 phút (`totaltimer.lua`): công giữ **> 1,5 (tức ≥ 2)** → công thắng, ngược lại thủ thắng.
- `GameWin` → `NotifyWarResult(city, côngThắng)` (C++ :1507): công thắng → chủ = khiêu chiến giả, Thái Thú = bang chủ (Title 152+city, thu của cựu), field 48; **mọi nhánh**: khiêu chiến giả = "", state 0, đồng bộ tên chủ + thuế vào KSubWorld → `CloseMission(7)` → `EndMission`: xoá trụ, **`GameOver`**: top-10 `PL_TOTALPOINT` +3.000.000 exp, 5 người ngẫu nhiên Lễ hộp (6,1,1076), `LeaveGame` mọi người (PK tắt, camp gốc, `SetPunish(1)`, `NewWorld` OuterPos 222/223 (1613,3185)); dọn vật cản; `ClearMapNpc(221)`; `BT_ClearBattle`.

**Sau trận**: thuế — Thái Thú thoại **bia Quản Thành** (`citybulletin.lua`, mới có ở Tương Dương) 22h-23h, 1 lần/ngày, 0-20%; thưởng tuần — T2 9h-18h thành viên bang chiếm thành nhận 1 huyền tinh cấp 5 (6,1,146) ở NPC Kim Sơn (`citywar_function.lua take_tong_award`), 18h-20h bang chủ nhận phần dư (300 viên Biện Kinh/Lâm An, 200 thành khác). Dụng cụ công thành: NPC Quân Nhu trong trận (`junxuguan.lua AskDeal`): Kim Nguyên Bảo (event 343) → Nghiệt Long Xung Xa / Vân Kỳ Binh phù / Ngoan Cổ Binh phù (6,1,30-32), mỗi người 1 lần.

### 2.5 Điểm lệch / bẫy nhìn thấy khi đọc (chưa sửa, ghi để chủ biết)

1. `citywar_city\camper.lua` dùng `JOINSTATE` nhưng hằng này chỉ định nghĩa ở `citywar_arena\head.lua` (= 242) → `SetTaskTemp(nil→0, ...)` vô hại nhưng "cờ đã vào" của công thành ghi nhầm ô temp 0 (quirk gốc).
2. Vào trận với tư cách thành viên đòi **5 ngày trong bang**; lôi đài đã đổi thành **cấp ≥ 90** (chủ 21/08) — hai cửa lệch nhau; bot vừa vào bang hôm nay sẽ bị chặn (mục 3.1 Q2).
3. Khiêu chiến lệnh còn nhận nhưng không quyết định gì (2.2 bước 2) — giữ (cho exp) hay gỡ menu là quyết định của chủ.
4. Lưới A* phía server (`KSubWorld.cpp:1752 IsBotPathMap`) **không có 213-223** → bot hiện không đi bộ được trên mọi map công thành (PB_WalkTo trả −2) — bắt buộc thêm khi làm bot (3.4).
5. Vật cản động (rào sau cổng) ghi thẳng `m_Obstacle` lúc trận mở; lưới `_srv.fp` là cache tính lúc nạp map → A* bot **không biết rào** → có thể dẫn bot vào rào rồi kẹt "đường cụt im lặng" như TKKET4. Bot công phải phá cổng trước (đúng luật) hoặc PB_WalkTo kiểm `CellObsSrv` từng bước.
6. Thành 37/176 (Biện Kinh/Lâm An) lần đầu mở có thể trống NPC (báo từ E1); bia Quản Thành mới có Tương Dương.

---

## 3. SƠ ĐỒ BOT THAM GIA CÔNG THÀNH (thiết kế, chưa gõ mã)

Khuôn mẫu = **bot Tống Kim** (`pb_TkNhip` 1 s/lần trong `PB_Breathe`, `pb_TkGoiQuan`, máy trạng thái `pb_TkLai` 5 pha, `PB_TrapLog` cho bot đạp trap, `bot_tongkim.lua` trả trạng thái, menu `PB_TkMenu` trong `simcity_admin.lua`). Bot đi **đúng đường người chơi**: Xa Phu → hậu phương → trap → doanh → trap ra trận.

### 3.1 Năm câu chờ chủ quyết TRƯỚC khi thi công

| # | Câu | Đề xuất |
|---|---|---|
| Q1 | Bot đứng phe nào? | Theo **bang của bot** (`m_cTong` = bang thủ → phe 1, = khiêu chiến giả → phe 2). Muốn cả 2 phe có bot → bot ở 2 bang (2 bang chủ nhắn mật xin bang), hoặc bang chủ mua lệnh bài phát cho bot ngoài bang (trần 50/phe, tốn 200.000/lệnh). Cần việc 1 sửa xong trước. |
| Q2 | Luật **5 ngày trong bang** (`GetJoinTongTime ≥ 7200 phút`) | (a) giữ nguyên → bot vào bang hôm nay, 5 ngày sau mới đánh được; (b) đổi `trap.lua:44` sang **cấp ≥ 90** như lôi đài (chủ đã chọn cho lôi đài 21/08) — áp cho cả người thật; (c) ngoại lệ riêng bot (không khuyến nghị: bot khác luật người). |
| Q3 | Trần bot mỗi phe + cấp tối thiểu | mặc định **100 bot/phe** (chừa 100 chỗ trong trần 200 cho người thật), cấp ≥ 90 (cùng luật lôi đài), lệnh bài admin đổi được (`PB_SetCongThanhTran`). |
| Q4 | Bot có đánh **lôi đài** (20h ngày báo danh, ≤ 16/phe) không? | Đợt 2 — cùng khuôn, khác map (213-220) + NPC camper. |
| Q5 | Bot công có **mục tiêu** (cổng → trụ) hay chỉ "đánh địch gần nhất" như TK? | Có mục tiêu — không phá cổng thì rào không mở, không hạ trụ thì công **không bao giờ thắng**. Bot thủ canh trụ. Bot **không** mua dụng cụ/xe (cần Kim Nguyên Bảo). |

### 3.2 Máy trạng thái `b.nCtc` (trường mới trong `PB_Bot`, server-local, không đụng giao thức)

```mermaid
stateDiagram-v2
    [*] --> 0_Khong
    0_Khong --> 1_VeThanh : pb_CtcNhip thấy state 5 + mission 7 map 221 MS_STATE>=1\n+ bot đủ tư cách (pb_CtcGoiQuan, xáo trộn, trần/phe)
    1_VeThanh --> 2_ToiXaPhu : ChangeWorld về thành nhà (khuôn Dã Tẩu s_dtNpc, so le 3-5s)
    2_ToiXaPhu --> 3_HauPhuong : PB_WalkTo tới Xa Phu → ExecuteScript(script Xa Phu, GoCityWarAttack|Defend)\n→ sang 223 (công) / 222 (thủ)
    3_HauPhuong --> 4_Doanh : quét ô trap zhongzhuan_map\trap.lua (KRegion::GetTrap) → đi tới\n→ PB_TrapLog chạy kịch bản → CheckAndJoin → JoinCamp → map 221 doanh
    4_Doanh --> 5_RaTran : (chờ so le 5-10s) quét trap ctrap2|ctrap1 (theo phe) → đạp\n→ SetPos ra trận + SetFightState(1)
    5_RaTran --> 4_Doanh : chết → tự hồi sinh tại doanh (SetTempRevPos), FightState 0
    5_RaTran --> 5_RaTran : chọn mục tiêu theo phe (3.3), đánh, uống thuốc
    5_RaTran --> 6_HetTran : mission 7 đóng / bot thấy mình ở 222/223 (GameOver đã LeaveGame)
    4_Doanh --> 6_HetTran : mission đóng
    3_HauPhuong --> 6_HetTran : quá hạn pha (PB_CTC_PHA_HAN 3 phút) / trận đóng
    6_HetTran --> 0_Khong : bot_ctc_thoat (nếu rời sớm) → Thần Hành Phù về bãi (khuôn pb_TkDungPhu)
```

| Pha | Việc | Điều kiện sang pha / thoát | Khuôn TK tương ứng |
|---|---|---|---|
| 1 | Rời nhóm người thật (có log, không PM), `ChangeWorld` về **thành nhà** của bot cạnh Xa Phu (bảng `s_dtNpc` Dã Tẩu đã có 10 thành + NPC Xa Phu `PB_NPC_XP`), so le 3-5 s theo chỉ số bot | tới map thành | `pb_TkLai` pha 1 (:8938+133), `pb_DtVeThanh` |
| 2 | `PB_WalkTo` tới Xa Phu (`pb_TimNpcNho(PB_NPC_XP)`), tới nơi → `Player.ExecuteScript(Npc[xp].ActionScript, "GoCityWarAttack" hoặc "GoCityWarDefend", 0, false)` (state của `xaphu.lua` đã Include `station.lua` → `station_ctc.lua`); mỗi phe **1 bot bấm/nhịp** (khoá `s_uCtcBamTick[phe]` như TK); thành công = bot đã sang map 223/222; 10 lần không sang → bỏ cuộc | map = 222/223 | pha 2 (:9123-9262) |
| 3 | Quét ô trap có `m_dwTrap == id(zhongzhuan_map\trap.lua)` trong mọi region của subworld (`KRegion::GetTrap`, vòng `m_nTotalRegion` như `pb_TimNpc`) — cache theo map; `PB_WalkTo` tới ô đó; `PB_TrapLog` (KNpc.cpp:11112) thêm nhánh `nCtc == 3` cho phép chạy `ExecuteScript(id,"main")` như TK; script tự `JoinCamp` → map 221 | map = 221 | `pb_TkRaTrai` + `PB_TrapLog` [TKKET4] |
| 4 | Trong doanh: bơm máu/nội lực như bot luyện công (`pb_ApplyAuraBuff`, `pb_DungVatPham`), chờ `nCtcChoRa` (5 s + lệch); quét ô trap **`ctrap2/ctrap2b/ctrap2c`** (công) hoặc **`ctrap1/1b/1c`** (thủ), đi tới, `PB_TrapLog` nhánh `nCtc == 4` chạy kịch bản; kịch bản `SetPos` ra ngoài + FightState 1 → sang 5. Mọi trap khác (trap1/trap2 ra khỏi trận) **vẫn miễn** để bot không tự rời | vị trí đổi + `m_FightMode != none` | pha 3 (:9261-9457) |
| 5 | Đánh (3.3). Chết → khối tự hồi sinh sẵn có (`pb_DriveBot` "TU HOI SINH", 1 s trong trận) → về doanh → pha 4 | `pb_CtcMission()` NULL / `MS_STATE != 2` / map ≠ 221 | pha 4 (:9457-9740), `pb_TkTimDichGanNhat` |
| 6 | Nếu còn trong mission (rời sớm: tắt công tắc) → `ExecuteScript("\\script\\global\\bot_congthanh.lua", "bot_ctc_thoat")` (chép `LeaveGame` camper.lua:5-19 bỏ `NewWorld` + `DelMSPlayer(7)` + `RemoveSkillState(661)` + `Title_ActiveTitle(0)` + `SetTaskTemp(200,0)`); rồi Thần Hành Phù về bãi (`pb_TkDungPhu` 3 lần thua → `ChangeWorld` thẳng) | về bãi | pha 5 + `bot_tk_thoat` |

### 3.3 Chọn mục tiêu trong trận (`pb_CtcMucTieu`, gọi từ pha 5 thay cho `pb_TkTimDichGanNhat`)

Quan hệ đã đúng sẵn nhờ engine: `GenOneRelation` (KNpcSet.cpp) → cổng/trụ là `kind_normal` khác camp = **địch**; cùng camp = đồng minh; người chơi khác camp + cả hai FightMode ≠ none + cờ PK (JoinCamp đã bật) = địch. Mở gác `kind_player` trong `pb_FindTarget` (KPlayerBot.cpp:4862) thêm điều kiện `(b.nCtc == 5 && map == 221)` như TK.

| Phe | Ưu tiên 1 | Ưu tiên 2 | Ưu tiên 3 |
|---|---|---|---|
| **Công (camp 2)** | Địch (người/bot/NPC phe 1) trong tầm nhìn — đánh trước như TK | **Cổng còn sống gần nhất** (idx = `GetMissionValue(MS_DOORBEGIN+i)` ≠ 0 ở DoorPos[i]) → `PB_WalkTo` tới + đánh NPC 532 | Sau khi có cổng vỡ (rào đã `ClearObstacle`): **trụ thủ đang giữ** (`MissionV(15+i) == 1`) gần nhất → tìm NPC tại StonePos[i] (`pb_TimNpc` theo template 528) → đánh; đủ 3 trụ = thắng |
| **Thủ (camp 1)** | Địch trong tầm nhìn | **Đứng canh trụ** còn giữ (chia đều 3 trụ theo chỉ số bot, đứng trong 6-10 ô) | Trụ bị chiếm (`MissionV == 2`) → tới đánh NPC 530 để chiếm lại |

Chi tiết cần chép từ script vào C++ (bảng tĩnh, đơn vị MPS đã có trong `head.lua`): DoorPos ×3, StonePos ×3, ObstaclePos ×3, CS_CampPos ×2, điểm ra trận (1571,3263)/(1866,3547). Rào: bot công **không** được nhắm trụ khi cổng tương ứng chưa vỡ (rào chắn, A* không biết) — nếu PB_WalkTo trả −1 liên tiếp 5 lần → đổi mục tiêu về cổng.

### 3.4 Danh sách chỗ phải chạm (ước lượng)

| Tệp | Thay đổi | Cỡ |
|---|---|---|
| `KPlayerBot.cpp` | trường `nCtc, nCtcPhe, nCtcCity, nCtcTick, nCtcGoiThu, nCtcChoRa, nCtcMucTieu, nCtcTrapX/Y, uCtcKetTick...`; `pb_CtcMission()` (mission 7 trên map 221, khuôn `pb_TkMission`); `pb_CtcDuTuCach` (AI FIGHT, không TK/sạp/Dã Tẩu đang đi/nhóm người thật/`nBangPha`, cấp ≥ min, **có bang và tên bang == công hoặc thủ**); `pb_CtcGoiQuan` (xáo trộn, trần/phe, đếm `GetGroupPlayerCount(1|2)` để chừa chỗ người thật); `pb_CtcNhip` (1 s, census 10 s `[CtcCensus]`, công tắc tắt → trả trạng thái); `pb_CtcLai` 6 pha; `pb_CtcMucTieu`; `pb_TimTrap(nSub, dwScriptId)` quét `KRegion::GetTrap`; hook trong `pb_DriveBot` **ngay sau khối Tống Kim** (:10256) trước xin bang/sạp/về thành; gác thêm ở `pb_TkDuTuCach`, `PB_MoiVaoNhom`, `PB_SetDaTau`, `PB_SetBanSap`, **`pb_DongBoBang` (không đặt lại camp khi `nCtc`, như TK)**, `pb_RaBai`; `PB_TrapLog` thêm 2 nhánh; `pb_FindTarget` mở gác người chơi; `PB_SetCongThanh/PB_SetCongThanhTran/PB_CongThanhGoi` + `LuaPB_*` | ~700-900 dòng (TK là ~2.000, tái dùng nhiều) |
| `KJx2CityWar.cpp/.h` | helper C mới `int KJx2CityWar_GetWar(char* szCong, char* szThu)` trả city đang state 5 (đọc `s_Cities` static) — bot cần tên 2 bang để chọn phe | +30 |
| `KSubWorld.cpp:1752` | thêm `221,222,223` (+ `213..220` nếu Q4) vào `IsBotPathMap` — lần nạp đầu tự tính lưới, ghi cache `Maps\221_srv.fp` | +1 |
| `ScriptFuns.cpp` | khai + đăng ký 3 hàm `PB_SetCongThanh`, `PB_SetCongThanhTran`, `PB_CongThanhGoi` (khuôn :14035-14037, :15031+) | +6 |
| `KNpc.cpp:11110` | không đổi (đã gọi `PB_TrapLog` cho bot) | 0 |
| `script\global\bot_congthanh.lua` (mới, mirror `serverscript_jx2\jx1_edits`) | `bot_ctc_thoat()` | ~40 |
| `script\item\simcity_admin.lua` | `PB_CtcMenu` (Bật/Tắt theo giờ · Gọi ngay · Trần 50/100/200) + dòng trong `PB_Menu` | ~60 |
| `script\cauhinh\ch_lich.lua` (tuỳ) | `BOT_CTC_CAP_MIN`, `BOT_CTC_TRAN_PHE` nếu muốn chỉnh không cần build | +2 |

Không đổi giao thức, không đổi cấu trúc save, không đụng client. Log: `[BotCTC]` mọi bước (gọi quân, tới Xa Phu, sang 222/223, JoinCamp, ra trận, mục tiêu cổng/trụ, chết/hồi sinh, hết trận), `[CtcCensus]` 10 s.

### 3.5 Thứ tự thi công đề xuất

1. **Đợt 0 (điều kiện):** sửa việc 1 (1.3) → bot vào bang; chủ quyết Q1-Q3; thêm 3 map vào `IsBotPathMap`.
2. **Đợt 1:** pha 1-6 + đánh "địch gần nhất" (như TK) + về bãi + menu admin. Nghiệm thu bằng GM: đứng trong Phượng Tường → `E3Appoint` (bang A làm chủ) → bang B `E3Challenge` → `E6Force20` → ≤ 5 phút mission 7 mở → `PB_CongThanhGoi` → đọc `bot.log [BotCTC]`; kết bằng `E3WinAttack/WinDefend` hoặc chờ 90 phút.
3. **Đợt 2:** mục tiêu cổng → trụ (công), canh trụ (thủ); đo bằng `[citywar]` log của script (cổng vỡ, trụ đổi chủ).
4. **Đợt 3 (Q4):** lôi đài 213-220.

### 3.6 Rủi ro đã thấy trước

- 100-200 bot `NewWorld` cùng khung → nghẽn region sync (bài học 18/08) → so le như TK (mỗi phe 1 bot bấm/nhịp).
- Rào động không có trong lưới A* (2.5 #5) → bot công kẹt trước rào nếu nhắm trụ sớm → luật "cổng chưa vỡ thì không nhắm trụ".
- `pb_DongBoBang` 10 s/con sẽ **ghi đè camp** về camp bang nếu quên gác `nCtc` → bot thành "đồng minh" giữa trận.
- Bot tự hồi sinh hạ `m_FightMode = fight_none` (pb_DriveBot) — đúng với luật trận (về doanh FightState 0), nhưng phải đạp trap ra lại → pha 4 phải xử lý vòng lặp chết-hồi sinh không đếm là "kẹt".
- Bot chưa bao giờ đứng ở map 222/223/221 → lưới `_srv.fp` tính lần đầu lúc nạp map (boot) ~1-2 s/map, chấp nhận được.

---

## 4. TÓM TẮT CHO CHỦ

1. **Việc 1**: bot làm đúng tới bước xuất sư, nhưng script `xuatsu()` ghi nhầm task **0** thay vì **4134** vì `TASK_DUNGCHUNG2` không được Include vào script NPC môn phái (mỗi `.lua` một state riêng). Sửa = 1 dòng Include ở đầu `factionhead.lua` (đặt trước `lib_task.lua`), cần restart. **Chờ chủ gật** vì đụng script người thật (menu "Xuất sư" hiện lại 1 lần cho ai đã xuất sư).
2. **Việc 2**: CTC bản JX2 chạy bằng lịch tuần (18h đấu thầu lôi đài → 19h chốt → 20h lôi đài → 20h hôm sau công thành 90 phút map 221: phá cổng → hạ 3 Long trụ). Hiện **7 thành vô chủ** → bang đầu tiên đấu thầu sẽ chiếm không cần đánh. Khiêu chiến lệnh không còn quyết định gì.
3. **Việc 3**: sơ đồ 6 pha theo khuôn bot Tống Kim, ~900 dòng, 3 đợt; **chặn đường** hiện tại: bot phải ở trong bang **≥ 5 ngày** (hoặc chủ đổi luật sang cấp ≥ 90 như lôi đài) và lưới A* server chưa có map 221-223.
