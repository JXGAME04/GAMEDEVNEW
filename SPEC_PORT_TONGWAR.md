# ĐẶC TẢ THI CÔNG — BANG CHIẾN / VÕ LÂM ĐỆ NHẤT BANG (tag `tongwar`)

> Phiên **22/08/2026**. **CHỈ PHÂN TÍCH — chưa sửa tệp nào.**
> Nguồn: `D:\ServerLinux\server1\script\missions\tongwar` (16 tệp) + `script\event\tongwar` (7 tệp) + lịch relay `D:\ServerLinux\gateway\s3relay\relaysetting\task\tongwar.lua` (`tasklist.ini:247-248 [Task_64]`).
> Đích: mã `D:\GAMEDEVNEW\Sources`, cây chạy thật `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server`.
> Mọi số liệu đều đọc thật (tệp:dòng / địa chỉ ELF). Chỗ nào chưa chắc ghi **CHƯA RÕ**.
> Kết quả thô của công cụ: `ReverseTools\port_tongwar\` (`api_gap_tongwar.txt`, `closure.txt`, `ok_where.txt`, `check_ids.txt`, `npc_skill_remap.txt`, `item_remap.txt`, `dac_ta_ham_tongwar.json`).

---

## 0. KẾT LUẬN 1 PHÚT

| | |
|---|---|
| **Làm được?** | **LÀM ĐƯỢC CÓ ĐIỀU KIỆN.** Script + map + League + BT + Title + Ladder đều đã có nền. Nhưng phải (1) sửa **3 lỗi đang có sẵn trong engine** (timer mission gọi sai tên, `BT_LeaveBattle` quét task, `FormatTime2Number` sai định dạng) — cả 3 đang âm thầm phá luôn Lôi Đài Bang Hội / Hoạt động Phường / Tống Kim; (2) **tự dựng lối vào** vì bản Linux không có; (3) **chủ game chốt lịch mùa** (bản Linux hard-code 29/03→05/04/2014 ở 4 chỗ). |
| Hàm engine thiếu thật | **6** (`SetDeathType`, `GetMapInfoFile`, `ClearMapTrap`, `GetAllEquipment`, `ST_SyncMiniMapObj` (stub), `Tm2Time` (không cần nếu xoá mốc 2014)) + **4 stub** kênh chat. `api_gap2` đo 10 tên ENG (`port_tongwar\api_gap_tongwar.txt`), không phải 16 như PHANTICH — vì `AddMapTrap/ClearMapObj/IniFile_*/FileName2Id/ForbidEnmity` đã được thêm ở đợt Hoạt động Phường (`CoreServer.dll.moi_*` **chưa restart**). |
| Hàm đã có nhưng **sai ngữ nghĩa** so với Linux (phải sửa) | `BT_LeaveBattle` / `BT_ClearPlayerData` (KJx2Battle.cpp:145/:132), `FormatTime2Number` (KTongJX2.cpp:3945), timer mission `"OnMissionTimer"` (KMission.cpp:342), `GetTeamMember` lệch gốc chỉ số (ScriptFuns.cpp:7235), `AddSkillState` tham số 3 (ScriptFuns.cpp:12514), `GetGameTime` đơn vị (ScriptFuns.cpp:277), `GetItemProp` trả thêm `lv` (KJx2WarInfra.cpp:621). |
| Tệp chép | **25 tệp chép nguyên byte** (23 tệp của cây + `vng_event\give_support_item.lua` + `relay\tongwar.lua`) + **6 tệp MỚI** (driver, newworld hook, NPC lối vào, 2 script item thưởng, head Bắc Đẩu) — mục A. `tongpkmessage\tong_pk_message_gs.lua` **KHÔNG thuộc Bang Chiến** (handler giao thức `emSCRIPT_PROTOCOL_TONG_PKMESSAGE`, kéo cả hệ trùng sinh `global\translife.lua`) → bỏ ngoài phạm vi. |
| Settings | `missions.txt` dòng 34 (id 33), `TimerTask.txt` +61/+62/+54, `MapList.ini` +`605..607_NewWorldScript`, **15 tệp `settings\battles\maps\woods\`** (tên GBK → đặt theo quy ước cp1252), `settings\maps\tongwar\signup_trap.txt`, `gmscript.lua` +1 Include, `startgame.lua` +2 dòng. |
| Item | 1 item **làm thêm** (Quả Đại Hoàng Kim → id **4864**), 2 gold-equip tra theo tên (`goldequip.txt` record **4491** Cuồng Lan 5%, **3476** Phi Phong Phù Quang), Hoàng Chân Đơn **2273** (có sẵn nhưng `Script=0` → phải gắn script), Hỗn Nguyên Chân Đơn **4857** (đã làm ở đợt Tín Sứ). 6 mặt nạ cấm → bảng (particular, level) của dự án. |
| Map | 605-607 (chiến trường) + 608-613 (báo danh) **đủ cả hai pak** (PHANTICH 4.1: 336 / 105 tệp region). ⚠ PHANTICH ghi ngược: **605-607 là CHIẾN TRƯỜNG, 608-613 là BÁO DANH** (`head.lua:34-38 tbTONGWARMAP = {605,608,609}…`, `tongwar_getmatchmap` `:257-265`, `tongwar_autoexec.lua:1 tbTONGWAR_MAP = {608..613}`). |

---

## 1. CÁCH HOẠT ĐỘNG (tóm tắt đúng theo mã, để người thi công biết mình đang nối gì)

```
[RELAY tongwar.lua — tick 30′]                              [GS]
 TaskContent():
  mùa = tongwar_GetCurSeason(yymmdd)  (TONGWAR_SEASON_TB :12-43)
  ngày [1] & <18h  → tongwar_initmatch(): xoá league type 10 mùa cũ, tạo 7 league
                     tên = GetCityAreaName(i), member job=1 = GetCityOwner(i) (bang chiếm thành),
                     Ladder_ClearLadder(10225..10234), gb_SetTask("TONGWAR_STATION",2,1) pha 1, (…,3,mùa)
  ngày [2]..[3]     → pha 2 (thi đấu);  ngày [4]..[5] → pha 3 (nhận thưởng)
  ngày trong SCHEDULE_TABLE (:44-87) & 20:00±5 → GlobalExecute("dw tongwar_start(c1,c2,g1,c3,c4,g2,c5,c6,g3)")
                                        21:30±5 → GlobalExecute("dw tongwar_start(0)")
                                                            ├─ gmscript.lua → tongwar_gmscript.lua → tongwar_main.lua
                                                            │   tongwar_start: 3 cặp thành → map 604+g (605/606/607):
                                                            │   LG task CAMP=1/2, MAP=map, OpenMission(33), MissionV 21/22, MissionS 3/4,
                                                            │   SetGlbValue(850,1)   (:9-55)
                                                            └─ mission 33 (match\mission.lua InitMission):
                                                                đọc mapinfo (GetMapInfoFile→IniFile_*), chọn 2 khu (bt_autoselectmaparea),
                                                                đặt trap hậu doanh/cổng (AddMapTrap), NPC Quân Nhu/cờ/rương,
                                                                timer 61 (20 s) + 62 (90′), MS_STATE=1, MS_KEYNUMBER ngẫu nhiên
 Người chơi: NPC lối vào (MỤC F) → tongWar_Start (event\tongwar\head.lua:169)
   pha 1: bang chủ bang chiếm thành chọn thành (tongWar_SelectCity) / kết nạp liên minh (tongwar_want2join → tongwar_check_team
          → LG_ApplyDoScript(0,…,event.lua,"tongwar_add") → LGM_ApplyAddMember)
   pha 2: "Ta muốn tham gia thi đấu" → tongwar_want2signup (tongwar_signup.lua:3) → kiểm bang/liên minh/mặt nạ/GlbValue 850
          → NewWorld(map báo danh 608..613, 1 trong 3 toạ độ :70-74)
          → đạp trap (signup_trap.txt, 36 ô) → trap\tongwar_trap.lua main → tongwar_checkjoin → tongwar_signup:
            AddMSPlayer(33,camp), NewWorld(map 605..607, hậu doanh), SetDeathScript(playerdeath.lua), ForbidEnmity(1), PK…
   smalltimer (20 s): t==90 → RunMission (pha đánh, MS_STATE=2); t≥105: phe <5 người thua; mỗi 60 s công bố điểm;
          but_pop2home: ai ở hậu doanh >120 s bị ném ra đại doanh (npcpos1/2 trong mapinfo)
   playerdeath OnDeath: +75×hệ số quân hàm (RANK_PKBONUS), liên trảm 3 lần +150, 5 kill → +1 lượt chết,
          NDEATH≥MAXDEATH(10) → tongwar_check_outmatch đẩy về map báo danh
   totaltimer (90′) → CloseMission → EndMission → GameOver: so MS_TOTALPOINT_S/J → tongwar_dorelay
          → LG_ApplyDoScript(1,…,score.lua,"tongwar_score") → LG task WIN/LOSE/TIE/RESULT(+3/+1)/POINT(±net) + Ladder 10225/10226
   pha 3: tongwar_cityrank (xếp 7 liên minh theo RESULT rồi POINT) → danh hiệu 105 (bang chủ) / 106 (bang chúng) 30 ngày,
          thưởng TAB_AWORD_GOOD theo hạng (headinfo.lua:145-160) — chỉ bang chủ bang chiếm thành nhận (tongwar_aword_talk)
```

Hằng số gameplay (giữ nguyên): `missions\tongwar\head.lua:4-14` (20 s / 90′ / 30′ báo danh / 150 / 5 / 120 s / 10 lần / +75 / +150), quân hàm `:41-86` (mốc 0/10 000/20 000/40 000/60 000/80 000 ở `:74-79`; **`TAB_RANKBONUS = {0,1000,3000,6000,10000}` `:62` mới là bảng thật dùng ở `match\head.lua:409-417`** — TITLE_BONUSRANK* không được dùng ở đâu).

---

## A. DANH SÁCH TỆP CHÉP (bao đóng Include: `ReverseTools\port_tongwar\tongwar_closure.py` → `closure.txt`)

### A.1 Chép nguyên byte (25 tệp) — đích `bin\server\script\...` cùng đường dẫn tương đối

| # | Tệp Linux | Ghi chú |
|---|---|---|
| 1-16 | `missions\tongwar\head.lua`, `tongwar_main.lua`, `tongwar_autoexec.lua`, `match\{head,mission,playerdeath,smalltimer,totaltimer,hometrap1,hometrap2,homeouttrap1,homeouttrap2}.lua`, `npc\{chefu,doctor,openbox}.lua`, `trap\tongwar_trap.lua` | 16 tệp; sau khi chép **vá có chủ đích** theo mục H + mục G |
| 17-23 | `event\tongwar\{head,headinfo,tongwar_signup,score,event,tongwar_gmscript,npc_shizhe}.lua` | 7 tệp |
| 24 | `vng_event\give_support_item.lua` (295 dòng, tự chứa, Include `vng_lib\bittask_lib.lua` **IDENT**) | `npc_shizhe.lua:8` Include + menu `:34`. Sự kiện VNG hết hạn theo ngày (`:27-28` 2014) → **chép để giống gốc, chết như gốc** |
| 25 | `gateway\s3relay\relaysetting\task\tongwar.lua` → `script\missions\tongwar\relay\tongwar.lua` | khuôn `missions\tong\relay\` của đợt Hoạt động Phường; **phải sửa lịch** (mục E/H) |

**Không chép:** `tongpkmessage\tong_pk_message_gs.lua` (ngoài phạm vi, xem mục 0); `task\random\treasure_head.lua` (`event\tongwar\head.lua:3` Include nhưng **0/21 ký hiệu** của nó được dùng trong cây — kiểm bằng `port_tongwar` script; đợt Tín Sứ đã quyết không port `task\random`) → **chú thích dòng Include** (lệch có chủ đích).

### A.2 Tệp phụ thuộc dự án ĐÃ CÓ (dùng bản dự án, không chép)

| Tệp Include | Trạng thái (`closure.txt`) | Cần gì từ nó |
|---|---|---|
| `lib\file.lua` | IDENT(jx2lib) — `\script\lib\*` thiếu tự rơi về `scriptjx2\lib\*` (ScriptFuns.cpp `sJX2RemapScriptPath`) | `GetTabFileData/Height` |
| `lib\common.lua` | IDENT | `split`, `safestr`, `join` (`:56/:127/:32`) |
| `gb_taskfuncs.lua` + `lib\gb_taskfuncs.lua` | IDENT | `gb_GetTask/gb_SetTask` = League type **500** persist `jx2league.txt` (đã có 2 dòng `G 500`) |
| `task\system\task_string.lua` | IDENT | `CreateTaskSay` (71 lần gọi) |
| `dailogsys\dailogsay.lua` | IDENT | `CreateNewSayEx` (`head.lua:156`) |
| `global\titlefuncs.lua` | **DIFF** — dùng bản dự án (có `TASK_ACTIVE_TITLE = 1122` `:5` + cầu nối danh hiệu JX1) | `npc_shizhe.lua:354` |
| `tong\tong_header.lua` | DIFF(tong_vn) — engine tự ánh xạ `\script\tong\` → `scriptjx2\tong_vn\` | `TONG_MASTER = 0` (`tong_vn\tong_header.lua:134`) |
| `lib\awardtemplet.lua` | **DIFF** — bản dự án thêm `Include awardtype\item_jx1.lua` | `tbAwardTemplet:GiveAwardByList` (`head.lua:751`) — **phải mở rộng `item_jx1.lua`**, xem mục D.4 |
| `IL("FILESYS"/"ITEM"/"LEAGUE"/"SETTING"/"TITLE"/"RELAYLADDER"/"BATTLE"/"TONG")` | bảng module ScriptFuns.cpp:2450-2454 có đủ 8 tên (→ `noop.lua` hoặc `tong_header.lua`) | — |

### A.3 Tệp MỚI (6)

| Tệp | Nội dung | Khuôn |
|---|---|---|
| `script\missions\tongwar\tongwar_driver.lua` | thay task-centre relay: `TONGWAR_DriverInit` (DynamicExecute `relay\tongwar.lua` `TaskShedule` rồi `StartGlbMSTimer(29, 54, 30*18)`), `OnTimer` mỗi quý 15′ gọi `TaskContent` | `missions\tong\tong_driver.lua` (MSKEY 28/timer 53 đã dùng → dùng **29 / 54**) |
| `script\missions\tongwar\match\newworld.lua` | `OnLeaveWorld(szParam)`: `DelMSPlayer(33, 0)` + `OnLeaveWorldDefault(szParam)`; `OnNewWorld`: chỉ gọi default | `missions\leaguematch\combat\newworld.lua:13-16`. Lý do: `KNpc::ChangeWorld` của ta **không** gỡ mission khi đổi map (KNpc.cpp:9841-9870 dòng `m_MissionArray.RemovePlayer` bị comment), chỉ gỡ lúc logout (KPlayerSet.cpp:375) ⇒ `OnLeave` (mission.lua:171) không bao giờ chạy khi bị đẩy ra / dùng Thổ Địa Phù |
| `script\event\tongwar\npc_tongwar.lua` | NPC lối vào: `Include head.lua` + `main()` → `tongWar_Start()` (+ mục con `onetong_*` khi pha 3, mục F) | — |
| `script\item\huangzhendan.lua` + `script\event\BeiDouChuanGong\head.lua` | Hoàng Chân Đơn (2273) — chép Linux, **bỏ 3 Include không dùng** (`lib\pay.lua`, `task\task_addplayerexp.lua`, `BeiDouChuanGong\lib\addaward.lua`) | mục D.4 |
| `script\vng_event\item\biggoldenseed.lua` | Quả Đại Hoàng Kim (item mới 4864) | mục D.4 |

---

## B. SETTINGS / MAP PHẢI CHÉP HOẶC TẠO

| Việc | Chi tiết |
|---|---|
| `settings\task\missions.txt` **dòng 34** | hiện `33\t\script\missions\mission_trong.lua` → `33\t\script\missions\tongwar\match\mission.lua` (tra theo SỐ DÒNG = id+1; Linux `missions.txt:34` y hệt) |
| `settings\TimerTask.txt` | thêm `61\t\script\missions\tongwar\match\smalltimer.lua`, `62\t\script\missions\tongwar\match\totaltimer.lua` (Linux `:62-63`), `54\t\script\missions\tongwar\tongwar_driver.lua` (khoá 54 còn trống; 50-53 đã dùng) |
| `settings\maps\tongwar\signup_trap.txt` | chép nguyên (517 byte, 36 toạ độ pixel, header `TRAPX TRAPY`); dự án **chưa có thư mục** |
| `settings\battles\maps\woods\` (**15 tệp**) | dự án **không có `settings\battles`**. `mapinfo.txt` (3 073 byte, GBK) + 14 tệp toạ độ. Tên tệp gốc là **GBK** (`后营2.txt`…); bản dump Linux có 3 bản mojibake mỗi tệp, **nội dung trùng md5** (`port_tongwar\woods_names2.py`): lấy bản tên cp437 (`┤≤╙¬2.txt`…). **Tên trên đĩa dự án = byte GBK giải mã cp1252** (ACP máy = 1252; quy ước đang dùng: `settings\maps\great_night\³¤°×É½±±Â´.txt`): `ºóÓª2.txt` `´óÓª2.txt` `´óÓª2Ò°Íâ.txt` `ºóÓª2trap.txt` `´óÓª2toºóÓª2trap.txt` `´óÓª2toÒ°Íâtrap.txt` `Ë§Æì2.txt` và bộ `4` tương ứng (`woods_names2.py` in sẵn 14 tên). `mapinfo.txt:60` `symbolnpc=\\settings\\battles\\maps\\woods\帅旗2.txt` **chỉ một `\`** (lỗi gốc) — giữ nguyên, KIniFile chấp nhận. |
| `settings\MapList.ini` | dự án đã có 605-613 (`:4312-4364`) giống Linux **trừ** thiếu `NOTONGCLAIMWAR` trong `_NewWorldParam` (không ảnh hưởng). **Thêm** `605_NewWorldScript=\script\missions\tongwar\match\newworld.lua` (và 606, 607) thay `newworldscript.lua` (tệp không tồn tại). `605_MapInfo` (`:4314`) đã đúng đường `\\settings\\battles\\maps\\woods\\mapinfo.txt` |
| `settings\citywar.ini` `[CityArea]` | **trùng byte** với relay Linux; 7 `AreaName` = đúng 7 chuỗi TCVN3 của `TONGWAR_CITY` (`headinfo.lua:122`, kiểm từng byte) — không sửa |
| `settings\playertitle_jx2.txt` | dòng 101-107 (id 100-106) + 200 (id 199) **trùng Linux `playertitle.txt`** — không sửa |
| `settings\skills.txt` 661 | "trạng thái trong chiến trường Tống Kim", MaxLevel 20, SkillStyle 2 — **trùng** |
| `settings\npcs.txt` | 11 template (393 Xa phu, 625 Rương, 389 Chủ dược điếm, 55/49 Quân nhu, 629/630 cờ soái, 626-628 cờ, 53 Sứ giả) **trùng cả id lẫn tên** (`check_ids.txt`) |
| `script\gmscript.lua` | +`Include("\\script\\event\\tongwar\\tongwar_gmscript.lua")` (Linux `gmscript.lua:9`) — để `dw tongwar_start(...)` / `dw tongwar_gw_say(...)` hạ cánh đúng state (KJx2League.cpp:1041-1092) |
| `script\startgame.lua` | +`Include("\\script\\missions\\tongwar\\tongwar_autoexec.lua")` và gọi `tongwar_addsignnpc()` cạnh `:99`, +`DynamicExecute("\\script\\missions\\tongwar\\tongwar_driver.lua","TONGWAR_DriverInit")` cạnh `:101` (Linux `global\autoexec.lua:14` + `:170`) |
| Map pak | 605-607 `特殊用地\新密林战场`, 608-613 `特殊用地\卫国战争\烽火连城报名点` — PHANTICH 4.1 đã đo: **có ở server pak + WorldSet**; client có sẵn (cùng MapList). Không phải trích. |

---

## C. HÀM ENGINE THIẾU — đặc tả từ disasm (`port_tongwar\dac_ta_ham_tongwar.json` có mã mẫu đầy đủ)

| # | Hàm | ELF | Chữ ký / ngữ nghĩa (từ disasm) | Cài đặt dự án |
|---|---|---|---|---|
| 1 | **`SetDeathType(n)`** | `0x08110580` | `GetPlayerIndex` → ghi `Player[idx]+0xE4 = (int)n` (`0x081105DF`). `KNpc::OnDeath` (`0x080892E0`, nhánh người chơi `0x08089528`) đọc `+0xE4`: 0 = thường; ≠0 → nhánh `0x8089560`, nếu >0 thì **giảm 1** mỗi lần chết (`0x08089593-96`) rồi vẫn gọi death script (`0x8083720`). Cây gọi: `SetDeathType(-1)` lúc vào trận (`trap:113`), `SetDeathType(0)` lúc ra (`head.lua:140`, `:505`) — **luôn đi kèm `SetPunish(0/1)`** | thêm `int m_nJX2DeathType` vào `KPlayer` (init cạnh `m_dwDeathScriptId` KPlayer.cpp:295), Lua chỉ lưu; tuỳ chọn: KNpc.cpp:1540 khi ≠0 bỏ hình phạt như `m_nCurPKPunishState=0` (LuaSetDeathPunish ScriptFuns.cpp:12797) |
| 2 | **`GetMapInfoFile(mapid)`** | `0x081024E0` | gettop **== 1**; `SearchWorld` (`0x80f68a0`) <0 → `""`; else push chuỗi `SubWorld[idx]+0x4EFC4` = giá trị khoá `<id>_MapInfo` MapList.ini, **nguyên văn** (`\\` kép) | `KSubWorld.h:118` thêm `char m_szMapInfoFile[128]`, `KSubWorld.cpp:1777` đọc `%d_MapInfo` (khuôn `_NewWorldScript`), rút `\\`→`\` cho chắc; Lua push |
| 3 | **`ClearMapTrap(mapid)`** | `0x08102AC0` → `KSubWorld::ClearTrap 0x80efdf0` → `KRegion::ClearTrap 0x80e11a0` | lặp `m_nTotalRegion` (+0x20), region stride 0xCC4, **xoá sạch danh sách trap** của region (KList +0x8A0, đặt count +0x8A8 = 0) — kể cả trap map-data | KJx2WarInfra.cpp cạnh `LuaClearMapObj :261`: lặp region `memset(m_dwTrap,0)` (`KRegion.h:54`) |
| 4 | **`GetAllEquipment()`** | `0x0810D0F0` | `lua_newtable`; i=0..14 push `Player[idx]+0x408+i*8` (chỉ số Item ô trang bị, kể cả 0) `rawseti i+1` | `m_ItemList.GetEquipment(i)` (`KItemList.h:81`) i=0..`itempart_num`-1 (17 ô, `GameDataDef.h:202-219`) |
| 5 | `ST_SyncMiniMapObj(x,y)` | `0x081C1930` | gói S2C 15 byte `B4 0E00 00000000 x y` → `Player::Send` (`0x80a8400`): ký hiệu bản đồ nhỏ client JX2. Cây chỉ gọi `(-1,-1)` (gỡ) ở `match\head.lua:57` | **stub `return 0`** (client JX1 không có proto 0xB4, không có điểm nào đặt ký hiệu) |
| 6 | `Tm2Time(y,m,d,h,mi,s)` | `0x08103AC0` | `struct tm` zero, `tm_year=y-1900`, `tm_mon=m-1` (mặc định 1 → 0), `tm_mday=d` (mặc định 1), h/mi/s mặc định 0, `mktime` | **không cần** nếu xoá khối 2014 (mục H #18/#20 — 2 call site duy nhất). Mã mẫu trong JSON nếu muốn có sẵn |
| 7-10 | `CreateChannel/EnterChannel/LeaveChannel/DeleteChannel` | `0x081045A0/0x081044C0/0x08104420/0x08104560` | hệ kênh chat tạm JX2 (`KChannelMgr::Create 0x813afe0`, join `0x813b160`); không ghi dữ liệu khác | **4 stub `return 0`** — PHANTICH đã chốt bỏ |

### C.2 Hàm ĐÃ CÓ nhưng phải SỬA (engine) — đây mới là phần nặng

| # | Hàm | Vấn đề (đọc thật) | Sửa |
|---|---|---|---|
| S1 | **Timer mission gọi `"OnMissionTimer"`** — `KMission.cpp:338-345 g_MissionTimerCallBackFun` | `smalltimer.lua:3`/`totaltimer.lua:3` định nghĩa `OnTimer()` (chuẩn Linux). Ta gọi tên không tồn tại → `Lua_Call` lỗi (`KLuaScript.cpp:295-299`) mỗi 20 s, **trận không bao giờ chuyển pha / kết thúc**. ⚠ `citywar_arena\timer.lua:3`, `missions\tong\*\schedule\*.lua:3` **cũng `OnTimer`** — hai đợt trước chưa test nên chưa lộ (Liên Đấu thoát vì dùng GlbTimer `KJx2League.cpp:1225` gọi đúng `OnTimer`). | `g_MissionTimerCallBackFun`: thử `OnMissionTimer`, nếu không phải function thì gọi `OnTimer` — y khuôn `LuaInitMission` ScriptFuns.cpp:10888-10905. **Sửa một chỗ, cứu 3 tính năng.** |
| S2 | **`BT_LeaveBattle` quét 0 mọi task** — `KJx2Battle.cpp:145-161` (+ `BT_ClearPlayerData :132` quét cả type 40-49) | Linux `BT_LeaveBattle 0x081C62E0 → KBattle::LeaveBattle 0x08148F30` **chỉ gửi gói 7 byte (0xB0 sub 5) đóng bảng xếp hạng client, không đụng task**; `BT_ClearPlayerData 0x081C6050` quét type 0..49 **trừ 40..49** (`lea eax,[ebx-0x28]; cmp eax,9; jbe`). Cây gọi `BT_LeaveBattle()` **ngay trước khi vào trận** (`trap:92`) ⇒ bản ta quét `2369-2378` = `NDEATH/MAXDEATH/KEYNUMBER/TOTALPOINT` về 0 ⇒ `tongwar_check_outmatch` (`NDEATH>=MAXDEATH` = `0>=0`) **đẩy người chơi ra ngay lần đạp hometrap đầu**, điểm cá nhân cả mùa mất. | `LuaBT_LeaveBattle`: chỉ xoá member khỏi `s_Members`; `LuaBT_ClearPlayerData`: bỏ qua type 40-49. Kiểm citywar_arena/citywar_city: họ gọi `BT_ClearPlayerData` trước JoinCamp nên không dựa vào reset của LeaveBattle. Dài hạn: kho BT theo SubWorld (Linux `KBattle` nằm trong `KSubWorld +0x4D468`) vì tongwar mở **3 map cùng lúc** + citywar cùng giờ 20h dùng chung `s_Type2Task` (KJx2Battle.cpp:31). |
| S3 | **`FormatTime2Number`** `KTongJX2.cpp:3945-3954` trả `YYYYMMDD` | Linux `0x081023B0`: `sprintf("%02d%02d%02d%02d%02d", yy, mm, dd, HH, MM)` (chuỗi `0x825963a`) → **`YYMMDDHHMM`**. `head.lua:263` lấy `mod(…,1e8)` = `MMDDHHMM` cho `Title_AddTitle(n,2,t)` (KJx2Title.cpp:142-152 so `MMDDHHMM`). Bản ta → danh hiệu 30 ngày **không bao giờ hết hạn**. **6 caller đang có của dự án** (`songjin_shophead.lua:249-252`, `leaguematch\npc\officer.lua:174-177`, `ws_tiangong.lua:81-84`, `seasonnpc.lua:932-934`) đều giải mã theo dạng Linux ⇒ **đang tính sai**. | sửa theo Linux (10 chữ số → push double). |
| S4 | **`GetTeamMember(i)`** `ScriptFuns.cpp:7235-7262` | Linux `0x08115530`: `i==1` → **đội trưởng** (`0x08115618`), `i>1` → thành viên thứ i-1 (bỏ ô -1). Ta: `0` = đội trưởng, `1..n` = `m_nMember[i-1]`. `head.lua:439-440 for i = 2, teamsize do PlayerIndex = GetTeamMember(i)` ⇒ trên ta bỏ sót thành viên 1, `GetTeamMember(teamsize)` = 0 → `GetName()` lỗi. | **vá script** (không đổi engine vì Dã Tẩu dùng quy ước 0..n): `GetTeamMember(i - 1)` tại `head.lua:440`, `npc_shizhe.lua:238`. |
| S5 | **`AddSkillState(id, lv, p3, t)`** `ScriptFuns.cpp:12514-12556` | Linux `0x08126240 → 0x08125D70`: p3 = **kiểu thời gian 0/1/2** (≤2, `0x08125DB9`), luôn lấy skill thật từ `g_SkillManager` (`0x08076CE0`) → **có thuộc tính**. Ta: p3 = `nIfMagic`; **0 ⇒ `SetStateSkillEffect` với thuộc tính rỗng = không buff**; 1 ⇒ `CastStateSkill` thật. `head.lua:432 AddSkillState(661, rank-1, 0, 999999)` ⇒ quân hàm không tăng HP/thủ. `rank-1 = 0` ⇒ `GetSkill(661,0)` NULL ⇒ `pSkill->CastStateSkill` **deref NULL** nếu đổi p3=1. | vá script: `head.lua:432` → `if (rank > 1) then AddSkillState(RANK_SKILL, rank - 1, 1, 999999) end`; `mission.lua:188 AddSkillState(661,5,0,0)` → `RemoveSkillState(661)` (Đợt E đã làm y vậy, BANGIAO_CONGTHANH_DOTE.md:66). Thêm guard NULL vào `LuaAddSkillState`. |
| S6 | **`GetGameTime()`** `ScriptFuns.cpp:277` = `g_SubWorldSet.GetGameTime()` = `m_nLoopRate` (**frame**, +1/tick KSubWorldSet.cpp:90) | Linux `0x0810F3A0`: `Player[idx]+0x8C * 20 / 18` — giây online theo người. Cây dùng làm **giây** (`TIME_PLAYER_STAY = 120` `head.lua:10`, so ở `match\head.lua:197,233`, `hometrap*:11`). | vá script `head.lua:10` → `TIME_PLAYER_STAY = 120 * 18`; `match\head.lua:233` `remain_time = floor((...)/18)`. (Không đổi engine: `fuyuan.lua:43`/`storm\custom.lua:114` đang dùng.) |
| S7 | `GetItemProp(idx)` KJx2WarInfra.cpp:621-634 trả `g,d,p,lv,se,lk` | Linux trả `g,d,p`. Bảng cấm mặt nạ `tongwar_signup.lua:28-35` khoá theo `p` Linux (482,447,450,446,647,806 trong `item\004\mask.txt`) — dự án tách `(p,lv)` | vá bảng → mục D.3 |
| S8 | Death script: ta gọi `OnPlayerDeath(nVictimPIdx, nKillerNpcIdx)` (`KNpc.cpp:1540-1542`), Linux `OnDeath(Launcher)` | `playerdeath.lua:5` | thêm cầu nối **cuối tệp** `function OnPlayerDeath(nVictim, Launcher) OnDeath(Launcher) end` (khuôn `citywar_city\playerdeath.lua:2-4`). ⚠ `citywar_arena\death.lua` hiện **chưa có cầu nối** — lỗi tiềm ẩn đợt 1. |
| S9 | `DisabledUseTownP` / `ForbidChangePK` là **no-op** (KJx2WarInfra.cpp:234-244) | `trap:114 DisabledUseTownP(1)`: người chơi dùng Thổ Địa Phù thoát trận được | chấp nhận (đợt E đã chấp nhận); `newworld.lua OnLeaveWorld` (A.3) đảm bảo vẫn gỡ khỏi mission |
| S10 | `Title_*` chỉ sống trong RAM (`KJx2Title.cpp:58-60 s_PlayerTitles`, không có Save/Load) | danh hiệu 105/106 (30 ngày) & quân hàm mất khi restart; `tongwar_title_talk` chặn nhận lại bằng `GetByte(task 1737,1) == mùa` (`head.lua:213`) ⇒ **restart = mất danh hiệu không lấy lại được** | CHƯA RÕ mức chấp nhận. Gợi ý: persist `s_PlayerTitles` ra `settings\jx2title.txt` (khuôn `jx2league.txt`), hoặc ghi nhận là rủi ro. Mặt khác `titlefuncs.lua` dự án chỉ nối JX1-title cho 153-159 (Thái thú) ⇒ chữ trên đầu **không hiện** cho 100-106/199 — cần mở rộng `title_jx1_id` nếu muốn thấy (CHƯA RÕ bảng `PlayerTitle.txt` JX1 có dòng tương ứng không). |

Kiểm trước khi viết: `AddSkillState` guard NULL, `LuaAddSkillState:12542 pSkill->CastStateSkill` không kiểm NULL.

---

## D. BẢNG REMAP ITEM / NPC / SKILL / GOLDEQUIP

### D.1 NPC template — 11/11 **TRÙNG id + tên** (`check_ids.txt`): 393, 625, 389, 55, 49, 629, 630, 626, 627, 628, 53. Không sửa script.

### D.2 Skill

| Linux | Tên | Dự án | Chỗ dùng |
|---|---|---|---|
| 661 | trạng thái trong chiến trường Tống Kim (MaxLevel 20) | **661 TRÙNG** | `head.lua:63/432`, `mission.lua:188` |
| 1500 | 加全技能状态vn (hào quang VLMC) | **→ 1485** (1500 của ta là "Bích Mộc Chi Nộ") | `head.lua:269-270`, `npc_shizhe.lua:350-351` — **đổi 1500 → 1485** |
| 990/991/976/977 (ExtSkill của title 105/106) | trùng tên | trùng id | trong `playertitle_jx2.txt`, engine tự áp (KJx2Title.cpp:185-211) |
| 509 (Hoàng Chân Đơn `huangzhendan.lua:83`) | trùng (đợt Tín Sứ đã kiểm) | trùng | — |

### D.3 Mặt nạ cấm (`tongwar_signup.lua:28-35`) — `item\004\mask.txt` Linux (g=0 d=11) → `settings\item\mask.txt` dự án theo **TÊN**, khoá `(particular, level)`:

| Linux p | Tên | Dự án (p, lv) |
|---|---|---|
| 482 | Mặt nạ - Anh hùng chiến trường | (48, 3) |
| 447 | Mặt nạ Nguyên soái | (44, 8) |
| 450 / 446 | Mặt nạ Đại Tướng quân (2 dòng cùng tên) | (44, 7) **và** (45, 1) |
| 647 | Mặt nạ chiến trường Vương Giả | (64, 8) |
| 806 | Mặt nạ chiến trường Thánh Giả | (80, 7) |

Vá: bảng thành `["48_3"]=1, ["44_8"]=1, ["44_7"]=1, ["45_1"]=1, ["64_8"]=1, ["80_7"]=1`; dòng `:38-39` → `local nG,nD,nP,nLv = GetItemProp(tbEquip[i]); if nG == 0 and nD == 11 and tb[nP.."_"..nLv] then`. (`itempart_mask` = 11 GameDataDef.h:213 — **CHƯA RÕ** `GetDetailType()` của mặt nạ dự án có = 11 không; kiểm bằng 1 item thật trước khi chốt.)

### D.4 Item thưởng (`headinfo.lua:145-160 TAB_AWORD_GOOD`, `item_remap.txt`)

| Linux | Tên | Dự án | Cách trao trên dự án |
|---|---|---|---|
| `{6,1,30438}` nExpiredTime 10080′ | **Quả Đại Hoàng Kim** (ảnh `\spr\item\questkey\huangjinzhiguo.spr` — **có** trong `bin\client\data\update01.pak`) | **THIẾU → làm thêm id 4864** (`magicscript.txt` 4 865 dòng, id = dòng-2). Dòng gốc Linux (`item\004\magicscript.txt`): `Quả Đại Hoàng Kim\t6\t1\t30438\t\spr\item\questkey\huangjinzhiguo.spr\t377\t1\t1\tSử dụng sẽ nhận được 200 triệu kinh nghiệm\t\t0\t1\t0\t\script\vng_event\item\biggoldenseed.lua\t…` — **bố cục cột khác** (Linux cột 14 = script, dự án cột 10): chép theo cột dự án như 7 item 4857-4863 đợt Tín Sứ | `AddItem(6,1,4864,1,0,0,0)`; script `biggoldenseed.lua` chép Linux: `GetLevel() < 120` (`:9`) → **90**; `TASKIDDAY/TASKEATCOUNT` không định nghĩa trong tệp (Linux dùng global của `item\goldenseed.lua:5/7` = 2321/2322) → thêm 2 dòng; `huihuang_checkdate` gọi `ITEM_GetExpiredTime` (**không có** trong dự án) → vì ta không đặt hạn, vá `if nItemdate == 0 then return 1 end` (item không hết hạn — cùng lệch có chủ đích như Đợt E "GetItemLife"). |
| `{6,1,2264}` tbParam={0,20} | **Hoàng Chân Đơn** | **→ 2273** (trùng tên+ảnh `huangzhendan.spr` có trong `updatejx03.pak`), nhưng `Script = 0` (đồ chết) | gắn `\script\item\huangzhendan.lua` vào cột 10 của 2273; script chép Linux (cho 2 tỷ exp `:81`, 30 ngày/lần task 2668 `BeiDouChuanGong\head.lua:8-10`); thêm đường dẫn vào danh sách JX2 `KItemList.cpp sIsJx2ItemScript` vì script tự `RemoveItemByIndex` + `return` (bẫy #7). `tbParam={0,20}` Linux ghi tham số 2 = 20 (2 tỷ/1e8) nhưng **script không đọc** (cố định 2 000 000 000 `:81`) → bỏ qua. |
| `{6,1,30301}` | Hỗn Nguyên Chân Đơn | **→ 4857** (đợt Tín Sứ đã tạo, script `honnguyenchandon.lua`) | `AddItem(6,1,4857,…)` |
| `{0,4862, nQuality=1}` nExpiredTime 30 ngày | **Cuồng Lan 5%** = `goldequip.txt` Linux hàng 4862 (`0 14 0`, attr `249 5 5`) | dự án `goldequip.txt` **record 4491** (dòng 4493; 7 hàng "Cuồng Lan" chỉ 4491 khớp attr 5 5) | `AddItem2(2, 0, 4491, 0, 0, 0)` (`ScriptFuns.cpp:4662 LuaAddItem2`, nature 2 = NATURE_GOLD đọc `goldequip.txt` record; dự án đang dùng y vậy `event\30thang4\EventLib.lua:176`) |
| `{0,3477, nQuality=1}` nExpiredTime 90 ngày | Phi Phong Cấp Phù Quang (hoà giải sát thương) = Linux hàng 3477 | dự án **record 3476** "Phi Phong Phù Quang (hòa giải sát thương)" — cùng 18 thuộc tính, khác tên (thiếu "Cấp") và thiếu yêu cầu cột 34-35 (47=54000) | `AddItem2(2, 0, 3476, 0, 0, 0)` |

**Mở rộng `lib\awardtype\item_jx1.lua`** (tệp dự án, 39 dòng): `ItemJX1Type:Give` hiện chỉ `AddItem(tbProp[1..6])`. Thêm: nếu `tbItem.nQuality == 1` và `tbProp[1] == 0` → `AddItem2(2, 0, tbProp[2], 0, 0, 0)`; `tbProp[2]` khi đó phải là **record dự án** ⇒ sửa `headinfo.lua:151/154` `{0,4862}`→`{0,4491}`, `{0,3477}`→`{0,3476}`; `nExpiredTime` **bỏ qua** (không hạn — ghi lệch có chủ đích). Hoặc viết `awardtype\goldequip_jx1.lua` đăng ký khoá riêng — tuỳ người thi công, nhưng phải ghi chú.

Thưởng theo hạng (`headinfo.lua:147-154`, cột = hạng 1..7): Quả Đại HK 30/15/10/10/5/0/0 · Hoàng Chân Đơn 20/15/10/5/5/5/0 · Cuồng Lan 10/5/0… · Hỗn Nguyên 30/20/10/5/5/5/5 · Phi Phong 1/0… — mỗi lần nhận tối đa 60 (10 với loại 2/3) `head.lua:727-734`, đếm đã nhận trong LG task 11/12/13/14/21 (`headinfo.lua:55`).

---

## E. LỊCH & CƠ CHẾ KÍCH HOẠT (relay → driver)

**Relay Linux**: `tasklist.ini [Task_64] TaskFile=tongwar.lua` (`tongwar_once.lua` **không** nằm trong tasklist → bỏ). `TaskShedule :89-97`: chạy từ giờ tròn kế tiếp, **mỗi 30′**, vô hạn. `TaskContent :99-185` như sơ đồ mục 1. `GlobalExecute("dw …")` → mọi GS chạy trong state `gmscript.lua`.

**Dự án**: không có relay ⇒ `tongwar_driver.lua` (A.3) giống `tong_driver.lua`: `DynamicExecute(relay\tongwar.lua, "TaskShedule")` lúc boot, `OnTimer` mỗi quý 15′ gọi `TaskContent` (cửa sổ ±5′ của `:159/:175` chỉ khớp tick :00 và :30 ⇒ tương đương 30′). `TaskName/TaskTime/TaskInterval/TaskCountLimit` = stub C có sẵn (ScriptFuns.cpp:14598-14601). `GlobalExecute` của ta xếp hàng "dw" hoãn 1 tick vào state `gmscript.lua` (KJx2League.cpp:1041-1092) ⇒ `gmscript.lua` **phải Include `tongwar_gmscript.lua`** (mục B). `LG_ApplyDoScript(1, …, score.lua, "tongwar_score")` và `(0, …, event.lua, "tongwar_add")` chạy **cục bộ đồng bộ** trong state tệp đích (KJx2League.cpp:856-907) — `score.lua`/`event.lua` nằm trong `script\` nên được `LoadAllScript` nạp sẵn; `event.lua:96 tongwar_say` lại `GlobalExecute("dw tongwar_gw_say…")` → về `gmscript` state ✓ (hoãn 1 tick — vừa đúng để không `Say` lồng hộp thoại).

**Lịch mùa — PHẢI CHỐT (CHƯA RÕ, chủ game quyết):** 4 chỗ hard-code cùng phải khớp nhau:
1. `relay\tongwar.lua:12-43 TONGWAR_SEASON_TB` — thêm mùa `[9] = {yymmdd báo danh, yymmdd đánh đầu, yymmdd đánh cuối, yymmdd thưởng đầu, yymmdd thưởng cuối}`; ngày báo danh phải **< 18h** tick thì `tongwar_initmatch` mới chạy (`:129`).
2. `relay\tongwar.lua:44-87 SCHEDULE_TABLE` — 7 mục theo **ngày trong tháng** (`"29","30","31","1","2","3","5"`), mỗi mục 3 cặp thành (id 1..7 theo `TONGWAR_CITY`), thành còn lại nghỉ.
3. `event\tongwar\head.lua:516-517` (danh sách ngày có đấu) + `:542-551 TAB_CALENDAR[ngày] = {đối thủ của thành thứ i theo TAB_CITYNAME}` (thứ tự `TAB_CITYNAME :542` **khác** `TONGWAR_CITY`!) — dùng cho mục "Hôm nay đối thủ là ai".
4. `headinfo.lua:125-133 TONGWAR_CALENDAR` (chuỗi hiển thị) + `head.lua:492` `tongwar_detail` (chuỗi "29/03/2014 … 05/04/2014, trừ 04/04").
Gợi ý: giữ cấu trúc, ghi 1 bảng tháng mới; nếu mùa lặp hằng tháng cần viết lại `tongwar_GetCurSeason` — ngoài phạm vi "chép y".

Giờ: báo danh 20:00-20:30, đánh 20:30-21:30 (`TONGWAR_STATETIME=2000 :5`, `RUNGAME_TIME=90 tick×20 s`). **Trùng giờ Công Thành/Lôi đài CN 20h** (timerserver_ctc.lua) — xem G.

---

## F. LỐI VÀO (bản Linux KHÔNG có — DIEUKIEN đã chứng minh `tongWar_Start` 0 call site)

Đề xuất (tái dùng 100% `tongWar_Start`/`tongwar_want2signup`, không viết logic mới):

1. **NPC "Võ Lâm Truyền Nhân"** (tên theo chuỗi `WULINMENG_NPC headinfo.lua:120`, template **53** như Sứ giả gốc `tongwar_autoexec.lua:37`), đặt **Ba Lăng Huyện** cạnh "Sứ Giả Công Thành" (1625,3170 — cùng nơi đăng ký Lôi đài bang hội đợt 1) — toạ độ cụ thể do chủ game chọn; gốc đặt ở Hoa Sơn (map 2, 2608×3496) **CHỈ 07-18/04/2014** (`tongwar_autoexec.lua:31-42`).
   Script `event\tongwar\npc_tongwar.lua`: `Include head.lua; Include npc_shizhe.lua; function main() tongWar_Start() end`. Mọi mục con đã có sẵn trong `tongWar_Start` theo pha 1/2/3 (`head.lua:180-207`).
2. Thêm NPC này vào `tongwar_addsignnpc()` (thay khối ngày 2014 `:31-42` bằng spawn vô điều kiện) — **hoặc** gọi `AddNpc` trong `startgame.lua` như Tín Sứ.
3. Đăng ký danh sách 54 người (`npc_shizhe.lua`, cửa sổ `20140407-17` `:16-17`, danh hiệu 199 `20100118-0214` `:19-20`): muốn dùng thì thay điều kiện ngày bằng `get_tongwar_phase() == 3` và thêm mục "Đăng ký chiến đội Thiên Hạ Đệ Nhất Bang/onetong_signupmem" vào menu pha 3 — **CHƯA RÕ** chủ game có cần (là sự kiện hậu mùa của VNG).
4. Xa phu / Rương / Chủ dược điếm trên 6 map báo danh + 36 ô trap: `tongwar_addsignnpc()` lo (`:12-30`) — NPC map-data bị engine bỏ (bẫy #6) nên bắt buộc chạy hàm này lúc boot. `doctor.lua:8 Sale(131,1)`: `buysell.txt` dự án **102 dòng** (Linux 166, shop 131 = thuốc 1-25) → đổi thành **`Sale(12, 1)`** ("Hieu thuoc Thanh Thi", dòng 13) — lệch có chủ đích.

---

## G. XUNG ĐỘT VỚI TÍNH NĂNG ĐANG CHẠY + CÁCH GIẢI

| # | Xung đột | Chứng cứ | Giải |
|---|---|---|---|
| G1 | **Timer mission `OnMissionTimer`** | C.2 S1 | sửa engine fallback |
| G2 | **Kho BT toàn cục + `BT_LeaveBattle` quét task** | C.2 S2; citywar/citywar_arena 20h cùng giờ (timerserver_ctc.lua) dùng chung `s_Type2Task` | sửa 2 hàm theo Linux; cân nhắc kho BT theo SubWorld |
| G3 | `FormatTime2Number` sai định dạng — đang ảnh hưởng Tống Kim/Liên Đấu/Thiên Công | C.2 S3 | sửa theo Linux |
| G4 | Mission 33 / timer 61-62 / League type 10 / Ladder 10225-10234 / task 2369-2378, 1737, 2305-2308 / GlbValue 850 / temp 193 / title 100-106,199 | đã grep cây dự án: **trống** (`jx2league.txt` dùng 2,4,500,508,509,538; `codenew.lua` trùng số chỉ là khoá giftcode) | — |
| G5 | Temp task 200 = "đang trong trận" dùng chung với citywar (`citywar_arena\head.lua:94`, `citywar_city\camper.lua:136`) | cùng ngữ nghĩa | — |
| G6 | `GetGlbValue(850)` do `tongwar_start` đặt; `tongwar_start(0)` lúc 21:30 reset — nếu GS restart giữa trận thì 850 mất (kho RAM `KJx2SharedStore.cpp:469-492`) | — | chấp nhận; `tongwar_want2signup:53` chỉ chặn vào thêm |
| G7 | Tiền lệ bản tự viết trùng chức năng? | grep `tinhnang\` không có bang chiến/liên minh bang | không phải gỡ gì |
| G8 | `AddGoldItem(0,N)` đợt Tín Sứ (`posthouse.lua:966/977/988`) — `LuaAddGoldItem ScriptFuns.cpp:4401-4423` đọc **đối 1** (=0 → hàng 2 `GoldItem.txt`) | phát hiện ngoài lề khi tra gold-equip; THICONG 7.1 ghi "hoán đổi 205/206" nhưng không đảo thứ tự đối | báo riêng — không thuộc tongwar |
| G9 | `citywar_arena\death.lua:3 OnDeath(Launcher)` chưa có cầu `OnPlayerDeath` | C.2 S8 | báo riêng |

---

## H. DANH SÁCH ĐÍCH DANH TỪNG CHỖ SỬA TRONG TỆP CHÉP

### H.1 Chính sách chủ game (cấp ≥ 90, bỏ trùng sinh, bỏ mốc 2014)

| # | Tệp : dòng | Cũ | Mới |
|---|---|---|---|
| 1 | `event\tongwar\tongwar_signup.lua:14-21` | khối `nTongJoinTime … Tm2Time(2014,3,29,0,0) … Say("…trước 0h ngày 29/03/2014…")` | **xoá cả khối** (DIEUKIEN #18) |
| 2 | `tongwar_signup.lua:22-26` | `if ST_GetTransLifeCount() < 4 then Talk(1,"","Chỉ những nhân vật Trùng sinh 4…") return end` | `if GetLevel() < 90 then Talk(1, "", "Phải đạt cấp 90 trở lên mới được vào đấu trường.") return end` (DIEUKIEN #19) |
| 3 | `missions\tongwar\trap\tongwar_trap.lua:64-71` | bản sao khối 2014 | **xoá** (DIEUKIEN #20) |
| 4 | `tongwar_trap.lua:72-76` | `ST_GetTransLifeCount() < 4` | `GetLevel() < 90` + câu thoại như #2 |
| 5 | `event\tongwar\npc_shizhe.lua:341` | `… or GetJoinTongTime() < 10080) then` | bỏ vế `or GetJoinTongTime() < 10080` (DIEUKIEN #21) |
| 6 | `npc_shizhe.lua:16-20`, `tongwar_autoexec.lua:31-42` | cửa sổ ngày 2014/2010 | theo quyết định mục F.3 |
| 7 | `vng_event\item\biggoldenseed.lua:9` | `GetLevel() < 120` | `GetLevel() < 90` (item thưởng) |

### H.2 Vá kỹ thuật bắt buộc (lệch có chủ đích, ghi chú `-- [TONGWAR 22/08]` tại chỗ)

| # | Tệp : dòng | Cũ → Mới | Lý do |
|---|---|---|---|
| 8 | `event\tongwar\head.lua:3` | `Include("\\script\\task\\random\\treasure_head.lua")` → chú thích | không dùng ký hiệu nào; tệp không port |
| 9 | `head.lua:440`, `npc_shizhe.lua:238` | `GetTeamMember(i)` → `GetTeamMember(i - 1)` | S4 |
| 10 | `head.lua:269-270`, `npc_shizhe.lua:350-351` | `1500` → `1485` | D.2 |
| 11 | `match\head.lua:432` | `AddSkillState(RANK_SKILL, rank - 1, 0, 999999)` → `if (rank > 1) then AddSkillState(RANK_SKILL, rank - 1, 1, 999999) end` | S5 |
| 12 | `match\mission.lua:188` | `AddSkillState(661,5,0,0)` → `RemoveSkillState(661)` | S5 (Đợt E tiền lệ) |
| 13 | `missions\tongwar\head.lua:10` | `TIME_PLAYER_STAY = 120` → `120 * 18`; `match\head.lua:233` chia 18 khi hiển thị | S6 |
| 14 | `match\playerdeath.lua` cuối tệp | + `function OnPlayerDeath(nVictim, Launcher) OnDeath(Launcher) end` | S8 |
| 15 | `tongwar_signup.lua:28-43` | bảng mặt nạ → khoá `"p_lv"` + nhận 4 giá trị `GetItemProp` | D.3 |
| 16 | `headinfo.lua:151/154` | `{0,4862}`→`{0,4491}`, `{0,3477}`→`{0,3476}`; `:149` 2264→**2273**; `:153` 30301→**4857**; `:147` 30438→**4864** | D.4 |
| 17 | `npc\doctor.lua:8` | `Sale(131, 1)` → `Sale(12, 1)` | F.4 |
| 18 | `relay\tongwar.lua` | mùa/lịch mới (E) | — |
| 19 | `lib\awardtype\item_jx1.lua` (tệp dự án) | + nhánh `nQuality==1` → `AddItem2(2,0,…)` | D.4 |
| 20 | `settings\item\magicscript.txt` | +dòng 4864; 2273 cột 10 = `\script\item\huangzhendan.lua` | D.4 |
| 21 | `KItemList.cpp sIsJx2ItemScript` | + `item\huangzhendan.lua`, `vng_event\item\biggoldenseed.lua` | bẫy #7 (script `return 0/1`) |

**Giữ nguyên (luật chơi, không phải cổng):** phải có bang; bang thuộc liên minh; bang chủ mới lập liên minh/nhận thưởng; tối đa 150/phe, tối thiểu 5; 10 lần chết; 6 mặt nạ; giờ 20:00-21:30.

---

## I. RỦI RO + CÁCH TEST SAU RESTART

### I.1 Rủi ro
1. **3 sửa engine (S1-S3) đụng tính năng khác** — S1/S3 sửa về đúng; S2 phải kiểm lại citywar_city/citywar_arena (`BT_ClearPlayerData` trước JoinCamp).
2. **`CloseMission` gọi từ trong timer callback** (`smalltimer.lua:38`, `totaltimer.lua:8`) → `KMission::StopMission → Init()` xoá timer đang được `KTimerFunArray::Activate` lặp (KMissionArray.h:113-133). Citywar làm y vậy và chưa sập — theo dõi.
3. **Tên tệp GBK trong `mapinfo.txt`** + `\\` kép: nếu `KIniFile::Load`/`TabFile_Load` không mở được → `InitMission` in `"Load IniFile Error!"`, `areacount` nil → `tongwar error`. Test riêng bằng `IniFile_Load` + `GetTabFileHeight` từ lệnh bài admin trước giờ.
4. Thiếu `OnLeave` khi đổi map (S9/A.3) nếu quên `_NewWorldScript` → `GetMSPlayerCount` đếm người đã rời → luật "<5 người thua" và trần 150 sai.
5. `Title_*` RAM (S10): restart giữa mùa mất danh hiệu/quân hàm.
6. `LG_GetLeagueTask(lid, task)` khi `lid` = 0 (bang không liên minh): `tongwar_signup.lua:46` gọi sau khi đã kiểm `FALSE(n_lid)` ✓; `tongwar_querycityrecord :576-578` gọi với `LeagueID` có thể 0 → trả 0 (KJx2League.cpp:555 kiểm) ✓.
7. `tongwar_start` chạy trong `gmscript` state đặt `SubWorld = mapidx` rồi `OpenMission(33)` 3 lần (3 map) — `LuaInitMission` `_ASSERT(0)` nếu mission đã mở (gọi 2 lần) → chỉ gọi qua driver; GM test dùng `tongwar_redo_start(...)` (`tongwar_gmscript.lua:18-42`, kiểm tham số ≤7/≤3).
8. Ladder 10225-10234 client có hiển thị không (UI xếp hạng JX2) — **CHƯA RÕ**, không chặn gameplay.

### I.2 Test (GM, sau restart với DLL mới)
1. `ScriptError.log` không thêm dòng `tongwar`; log boot có `[TONGWAR] Driver … khoi dong`; map 608 có Xa phu/Rương/Chủ dược điếm (tpl 393/625/389 tại 1597,3139 / 1590,3132 / 1580,3151) và 36 trap.
2. Ép pha 1: `gb_SetTask("TONGWAR_STATION",2,1)`, `(…,3,<mùa>)` rồi `DynamicExecute(relay\tongwar.lua,"tongwar_initmatch",<mùa>)` → `jx2league.txt` có 7 dòng `G 10 <tên thành>` + member job 1 = bang chiếm thành (cần `jx2citywar.txt` có chủ thành — hiện **7 thành vô chủ** ⇒ phải có ít nhất 2 bang chiếm thành trước, hoặc GM `AppointViceroy`).
3. Bang chủ A (chiếm thành) tổ đội bang chủ B → NPC → "Đồng minh gia nhập liên minh" → B nhận thông báo (qua `dw tongwar_gw_say`, hoãn 1 tick).
4. Ép pha 2 + `dw tongwar_start(1,2,1, 3,4,2, 5,6,3)` từ `gmscript` (hoặc `tongwar_redo_start(1,2,1,3,4,2,5,6,3)`) → map 605/606/607 mở mission 33, tin `AddGlobalNews`; người chơi cấp ≥90 có bang trong liên minh → "Ta muốn tham gia thi đấu" → tới map 608/609 → đạp trap → vào hậu doanh 605; `GetTask(2377) == GetMissionV(25)`.
5. 30′ (hoặc đặt `RUNGAME_TIME` nhỏ khi test) → pha đánh; giết 1 người → +75 điểm, `Msg2MSAll`; chết 10 lần → bị đẩy về 608; ở hậu doanh >120 s → bị ném ra đại doanh.
6. 90′ → `GameOver` tin thắng/thua + `jx2league.txt` task 2/3/5/6 cập nhật + `jx2ladder.txt` 10225/10226; người chơi được `NewWorld` về map báo danh với `SetPunish(1)`.
7. Ép pha 3 → NPC: "Nhận danh hiệu" (105 bang chủ / 106 bang chúng, hết hạn sau 30 ngày — kiểm `Title_GetTitleInfo`), "Nhận phần thưởng" theo hạng: Quả Đại HK 4864, Hoàng Chân Đơn 2273 dùng được, Cuồng Lan/Phi Phong là gold equip đúng tên.
8. Sau S3: kiểm lại danh hiệu 7 ngày của shop Tống Kim (`songjin_shophead.lua:247-255`) hết hạn đúng ngày.

---

## J. PHÁT HIỆN NGOÀI LỀ (không thuộc tongwar, nên xử lý)
- **S1** timer mission: `citywar_arena\timer.lua`, `missions\tong\*\schedule\*.lua` (đợt 1) sẽ không chạy cho tới khi sửa `g_MissionTimerCallBackFun`.
- **S3** `FormatTime2Number`: Tống Kim / Liên Đấu / Thiên Công đang tính sai ngày hết hạn danh hiệu.
- **G8** `AddGoldItem(0,N)` đợt Tín Sứ đọc đối 1 = 0.
- **G9** `citywar_arena\death.lua` thiếu `OnPlayerDeath`.
