# BÀN GIAO 23/08/2026 — THI CÔNG 4 TÍNH NĂNG PORT + ENGINE PORT5

> Phiên 23/08. Lệnh chủ game: *"làm mọi thứ xong báo test"* + *"phải làm đúng 100% bản linux"*.
> Tiếp nối 4 SPEC ngày 22/08 (`SPEC_PORT_{TONGWAR,BAIRENLEITAI,BW,ARENA}.md`) và lớp JX2COMPAT (`b97735d0`).
> Commit: **`4789700b`** (engine 15 tệp + script vá + mirror). Đã push GitHub.
> **DLL chờ restart: `E:\...\bin\server\CoreServer.dll.moi_2308_tongkim6fix` (md5 `18caaf45`)** — build từ HEAD bởi phiên song song (bot Tống Kim), GỘP đủ: PORT5 + Tín Sứ/Lôi Đài CN + JX2COMPAT + 6 fix bot Tống Kim. Chỉ còn MỘT bản .moi này.
> **CHƯA TEST** — server chưa restart. Mục 7 là kịch bản test từng tính năng.

---

## 0. TÓM TẮT 1 PHÚT

| Tính năng | Trạng thái | Lối vào | Ghi chú lớn nhất |
|---|---|---|---|
| **Bang Chiến / Võ Lâm Đệ Nhất Bang** (tongwar, map 605-613) | ✅ đủ server + client | NPC **"Võ Lâm Truyền Nhân"** Ba Lăng Huyện (map 53, ô 1628,3173, cạnh Sứ Giả Công Thành) | Mùa 9 = **29/10 → 05/11/2026** (nghỉ 04/11) — trùng khoá ngày mùa 8 gốc nên toàn bộ lịch giữ nguyên byte |
| **Bách Nhân Lôi Đài** (bairenleitai, map 960) | ✅ đủ server + client | NPC **"Quan nhắc nhở Hoàng Thành Tư"** Lâm An (176, ô 1464,3223) | Mở 12:00–24:00 hằng ngày; cấp ≥ 90 |
| **Lôi đài Tỷ võ** (bw, map 209) | ✅ đủ server + client | 12 NPC **"Công Bình Tử"** ở Dương Châu/Tương Dương/Thành Đô | Hỗn Chiến dời sang **map 210** (cùng dữ liệu map) |
| **Bang Hội Thành Bảo** (tongcastle, map 984) | ✅ server đầy đủ; 🔴 **client THIẾU dữ liệu map 984** | NPC **"Người chỉ dẫn bang hội thành bảo"** Lâm An (176, ô 1663,3262) | Chủ nhật 17:00–19:00; cần thành Lâm An CÓ CHỦ (citywar). Người chơi **chưa vào được map** cho tới khi có pak client (mục 6.1) |
| **Cảnh Kỹ Trường** (arena) | 🚫 **HOÃN** | — | Đúng luật "Linux có sẵn ĐẦY ĐỦ mới làm": ~30% (toàn bộ nửa GS của protocol, lớp tbMember, công thức ELO, lối vào, UI client) đã **mất trên chính bản Linux** — phải tự thiết kế (SPEC_PORT_ARENA mục 0 liệt kê 12 bằng chứng). Nếu chủ game chấp nhận 5 khác biệt ở SPEC mục 0 thì làm được ~1–1,5 ngày. |

---

## 1. ENGINE PORT5 (15 tệp C++, script vá `ReverseTools\port5_engine_patch.py`)

Tất cả trong `#ifdef _SERVER`, **không đổi wire-format, không đụng client**. Mỗi hàm bám disasm trong `port_*/dac_ta_*.json`.

| Nhóm | Hàm | Ghi chú |
|---|---|---|
| tongwar | `SetDeathType` (lưu `KPlayer::m_nJX2DeathType` — cây luôn gọi kèm SetPunish) · `GetMapInfoFile` (KSubWorld đọc `<id>_MapInfo`, rút `\\`→`\`) · `ClearMapTrap` (KRegion::ClearAllTraps) · `GetAllEquipment` (17 ô) · `ST_SyncMiniMapObj` stub · `CreateChannel/EnterChannel/LeaveChannel/DeleteChannel` stub | |
| tongwar S2 | **Kho BT tách 2 nhóm theo cây script** (`g_GetScriptNameByState`): nhóm 1 = `missions\tongwar` + `event\tongwar`, nhóm 0 = còn lại (citywar…). Lý do: cả hai đăng ký `BT_SetType2Task` cùng khoá PL_* → một map chung là ghi đè chéo (điểm citywar rơi vào task mùa tongwar và ngược lại). **`BT_LeaveBattle` = no-op** (Linux chỉ gửi gói client — bản cũ quét 0 task làm tongwar bị đẩy ra ngay hometrap đầu, mất điểm cả mùa). **`BT_ClearPlayerData` chừa type 40-49** (giữ KEYNUMBER/LASTDEATHTIME/BATTLEPOINT như Linux). | KJx2Battle.cpp |
| bairenleitai | `Msg2Map` = alias Msg2Region · `SetTmpCamp/GetTmpCamp` (KNpc::m_nTmpCamp) + luật **GetRelation**: cả hai TmpCamp≠0 và (không phải người ∨ FightMode) ⇒ khác trại = địch, cùng = đồng minh — đặt sau khối SimCity, trước bảng kind · hệ **AddTimer/DelTimer/SuspendTimer/ResumeTimer** (`KJx2ScriptTimer_Breathe` mỗi tick, gọi `Hàm(nParam,nTimerId)` dạng `Bảng:Hàm`, kết quả (t,p) hẹn lại) · **RemoteExecute cục bộ** (1 GS: fn(hParam,hRes,0) trong state đích, callback trong state gọi, tự cấp/hủy hRes) · **AddMapTrap tham số 5** (KRegion `m_nTrapParam`, trap AddMapTrap → `main(nParam)`, trap JX1 cũ param NONE → `main(nPlayerIdx)` như cũ; so (script,param) để 2 vùng kề cùng script vẫn kích) · **CallPlayerFunction trả kết quả** (MULTRET — bản cũ trả nil làm `tbPlayerList[nil]`) · **DynamicExecute/ByPlayer**: chuỗi `[[...]]` + buffer 4096 (bỏ cắt 64 byte) | |
| tongcastle | `GetNpcId`/`NpcCastSkill` alias · `GetItemStackCount/SetItemStackCount` (+SyncItem) · `MakeDateTime` · `GetNpcAroundNpcList` (tâm NPC bất kỳ) · `NPCINFO_GetNpcCurrentLife/Set` (không đụng max — SetNpcLife cũ ghi cả 2 = sai) · `OB_SaveShareData/OB_LoadShareData` → `\settings\jx2sharedata\<key>_<p1>_<p2>.bin` (tmp + MoveFileEx) · **AddNpcEx tham số 7 = bNoRevive** (nghĩa gốc Linux; NPC chết biến mất — gỡ tại OnRevive theo khuôn DelNpc-trong-OnRevive của daihoangkim.lua đã chạy thật; xác nán ~1s). GIỮ nhánh SetCurrentCamp cũ của tham số 7 (không đổi hành vi boss đã chạy). | |
| Ghi chú | `GetAroundNpcList "< 2"` **không sửa** — JSON tongcastle đoán sai: `Lua_NewTable` push trước khi đếm top nên gọi 1 tham số vẫn qua. `Tm2Time` không cần (khối 2014 đã xoá). | |

Build: `MSBuild Sources\Core\Core.vcxproj /p:Configuration="Server Release" /p:Platform=x64`. 15 tên hàm mới xác minh có trong DLL.

---

## 2. BANG CHIẾN (tongwar) — `port_tongwar\tongwar_port.py`

- **28 tệp chép** (16 cây missions + 7 event + give_support_item + relay + huangzhendan + BeiDouChuanGong\head + biggoldenseed) + **3 tệp MỚI**: `tongwar_driver.lua` (MSKEY 29, TimerTask 54, tick 15′ gọi relay TaskShedule/TaskContent — khuôn tong_driver; GM: `TONGWAR_Adm_TaskContent()`, `TONGWAR_Adm_InitMatch(9)`), `match\newworld.lua` (OnLeaveWorld → `DelMSPlayer(33,0)` — engine không gỡ mission khi đổi map), `event\tongwar\npc_tongwar.lua` (main → `tongWar_Start()`).
- **Vá đã áp** (mục H SPEC, có `-- [TONGWAR 23/08]` tại chỗ): bỏ khối 2014 + trùng sinh→cấp 90 (signup + trap); bảng mặt nạ → khoá `(particular,level)` `48_3/44_8/44_7/45_1/64_8/80_7` + `GetItemProp` 4 giá trị; skill 1500→**1485** (hào quang VLMC); item `30438→4864` (Quả Đại Hoàng Kim — **item mới**, dòng 4866 magicscript, script biggoldenseed), `2264→2273` (Hoàng Chân Đơn — gắn script cột 10), `4862→4491` (Cuồng Lan record goldequip), `30301→4857`, `3477→3476`; `mission.lua:188` `AddSkillState(661,5,0,0)`→`RemoveSkillState(661)` (bản ta sẽ thành buff vô hạn); doctor `Sale(131,1)`→`Sale(12,1)`; comment Include `treasure_head`; `npc_shizhe` bỏ vế `GetJoinTongTime<10080`; autoexec thay khối NPC-2014 bằng NPC lối vào; `item_jx1.lua` + nhánh goldequip `AddItem2(2,0,record,0,0,0)`; huangzhendan bỏ 3 Include chết + `AddSkillState(509,1,**1**,180)` (tệp KHÔNG thuộc danh sách JX2 nên phải tự đặt p3=1); biggoldenseed cấp 90 + `TASKIDDAY=2321/TASKEATCOUNT=2322` + bỏ hạn sử dụng.
- **KHÔNG vá** (JX2COMPAT lo): GetTeamMember(i), AddSkillState(661,rank-1,0,…) lúc vào trận, TIME_PLAYER_STAY, OnPlayerDeath bridge, OnTimer.
- **Settings**: `missions.txt` dòng 34 → `tongwar\match\mission.lua`; TimerTask +61/62/54; `signup_trap.txt`; **woods 15 tệp** (tên trên đĩa = byte GBK giải cp1252, vd `ºóÓª2.txt`; nguồn = biến thể cp437 trong dump, lọc theo bộ chữ 大营后野外帅旗); MapList 605-607 `_NewWorldScript`; gmscript +Include tongwar_gmscript; startgame +3 chỗ.
- **Lịch mùa 9**: `[9]={261029,261029,261105,261106,261111}` — cùng khoá ngày 29,30,31,1,2,3,5 của SCHEDULE_TABLE (mùa 8 gốc 29/03-05/04/2014 cũng vậy) ⇒ TAB_CALENDAR/`head.lua:516` giữ nguyên; chỉ đổi chuỗi hiển thị "29/03"→"29/10"… và "04/04/2014"→"04/11/2026". **Muốn mùa khác: sửa 1 dòng [9] + 7 chuỗi hiển thị.**

## 3. BÁCH NHÂN LÔI ĐÀI (bairenleitai) — `port_bairenleitai\brlt_port.py`

- 12 tệp (9 nguyên byte; DIFF: `npc_death`/`player_death` → `DynamicExecuteByPlayer` sang state chủ `hundred_arena.lua` (cầu `JX1_NpcDeath`/`JX1_PlayerDeath` append cuối tệp), `npc_enter` đổi Include npcdailog→dailogsay) + 3 MỚI theo Phụ lục P1: `bairen_boot.lua` (đặt NPC 1747 từ `chrismas\enternpc.txt`), `npc_quan_jx1.lua` (chuỗi TCVN3 trích nguyên từ enternpc.lua/config\9), `newworld.lua` (map 960 → OnEnterMap/OnLeaveMap).
- Vá chính sách: `hundred_arena.lua:411/414` cấp 120→90. Giữ đúng "lỗi gốc Linux" `self:DelPlayer` (không bao giờ chạy — hồi sinh điểm đăng nhập).
- Settings: 11 tệp `settings\maps\missions\bairenleitai`; MapList `960_NewWorldScript`; startgame +2.

## 4. TỶ VÕ (bw) — `port_bw\bw_port.py`

- 10 tệp + `bw_addnpc.lua` (12 Công Bình Tử — engine bỏ NPC thoại map-data). Vá: mission **4→11** (missions.txt dòng 12), timer **10/11→20/21**, task **300-302→2340-2342** (tránh đè Tống Kim), mở khoá dòng "Chức năng đã đóng", cấp 90 ở OnRegister/SignUpFinal (cả 2 đội trưởng)/OnJoin (khán giả miễn), `LeaveGame()` trước NewWorld trong GameOver, prompt nhập số 30 byte, tên "Lý Tư", cắt Include battlehead. **KHÔNG vá SetPunish/GetTeamMember** (JX2COMPAT).
- Hỗn Chiến: `mainloidai.lua:46` → **map 210**.

## 5. THÀNH BẢO (tongcastle) — `port_tongcastle\tc_port.py` (không có SPEC .md — thiết kế ghi trong script vá + JSON đặc tả)

- **Kiến trúc 1-state/tệp**: STATE CHỦ = `missions\tongcastle\tongcastle.lua` — mọi bảng sống ở đây. Append 5 cầu nối: `JX1_OnEnterMap/JX1_OnLeaveMap` (= CastlePlayer, gọi từ `newworld.lua` của map 984), `JX1_Trap` (trap.lua bridge), `JX1_CallGuard` (guard.lua bridge — cả luồng triệu hồi chạy trong state chủ để `GuardIsLimit/RegANpc` dùng kho thật), `JX1_TreeDeath` (treedeath adapter `OnDeath(npc, lastDamage)` → bối cảnh kẻ giết). Chuỗi tiếng Việt trích nguyên byte từ nguồn Linux.
- **Bản relay chạy cục bộ**: `script\mission\tongcastle\tongcastle.lua` (chép nguyên) — RemoteExecute 2 chiều + OB_SaveShareData persist điểm bang/người + cây (`settings\jx2sharedata\`). Driver `tongcastle_driver.lua` (MSKEY 30, TimerTask 55): boot = `AutoFunctions:Run` Ở STATE CHỦ + gọi thẳng `CreateGuideNpc/CreateGuideNpcInside` (KHÔNG Run ở guideperson — sẽ chạy TongCastle.Start bản sao → nhân đôi cây); tick 30s, đổi phút thì gọi `CheckAndReviveTree/CheckAndDeleteTree`. GM: `TONGCASTLE_Adm_ReviveTree(n)`.
- Vá khác: `game.lua` +`FORBITMAP_LIST = FORBITMAP_LIST or {}` (heart_head JX1 không có); `tongcastle.lua` bỏ `self:RegAll()`; `castleplayer` guard tbKillCount; `guideperson` 3 Include thay npcdailog + **cấp 90** (bỏ trùng sinh 4 + cấp 150; GIỮ "vào bang > 1 ngày").
- **NPC template**: cây 1912-1914 dự án CÓ SẴN cùng id (tên GBK 神木 — tên hiển thị ghi đè lúc spawn). Thủ Vệ 1908-1911 dự án đã bị dùng cho boss khác ⇒ **4 template MỚI 2031-2034** (map cột theo TÊN từ Linux; `npcs.txt` server ĐÃ ĐỒNG BỘ đuôi với client (nhận id 2021-2030 của client) rồi cả hai cùng thêm 4 dòng — client\settings\npcs.txt đã sửa kèm).
- Items: **3204-3207 CÓ SẴN đúng id** trên dự án — chỉ gắn script cột 10 (3204→guard.lua, 3205-3207→shenmuling.lua). Awardtype mới `zhenyuan_jx1.lua` (nZhenYuanPoint → `PlayerFunLib:AddTask(TASK_CHANGNGUYENDAN,n)`) + Include vào awardtemplet.lua.
- Map 984: pak server `maps_banghuichengbao.pak` (553 Region_S + .wor, đã verify trùng Linux) + `package.ini 6=` (đợt trước); **WorldSet +`World909=984`, Count=910**; MapList `984_NewWorldScript`; `thodiaphuvh.lua` chặn 984.

---

## 6. LỆCH CÓ CHỦ ĐÍCH / RỦI RO / VIỆC CHỦ GAME CẦN QUYẾT

1. 🔴🔴 **Map 984 thiếu dữ liệu CLIENT**: quét TOÀN BỘ pak client (cả cây vận hành J:\) — **0 tệp `_Region_C`** cho `特殊用地\banghuichengbao`; pak Linux cũng chỉ có Region_S (server). ⇒ client hiện tại **không vẽ được map 984** — đừng để người chơi NewWorld vào (sẽ đen màn/kẹt). Phần server + NPC lối vào + điểm + lịch vẫn chạy; NPC lối vào sẽ chặn tự nhiên khi thành vô chủ, nhưng **nếu Lâm An có chủ và đúng giờ mở thì EnterCastle sẽ dịch chuyển người chơi vào 984** → cần chủ game hoặc (a) tìm pak client VNG có map này, hoặc (b) tạm khoá mục "Ta muốn vào" (1 dòng), hoặc (c) đổi map đích. **Cần quyết trước khi mở.**
2. **Bang Chiến cần bang chiếm thành**: 7 thành đang vô chủ ⇒ pha 1 initmatch tạo 7 liên minh với member rỗng. Test cần ≥2 bang chiếm thành (citywar hoặc GM AppointViceroy). Tương tự Thành Bảo cần Lâm An có chủ.
3. **Danh hiệu JX2 sống trong RAM** (S10 SPEC): restart mất 105/106/quân hàm; chữ trên đầu chỉ hiện nếu mở rộng titlefuncs (chưa làm — ngoài phạm vi).
4. Thủ Vệ Hỏa Tường (template mới 2034) mang cột `Skill1=1212` của Linux — **skill 1212 dự án là "Linh Điệp"** (khác nghĩa); 1210/1208 (hào quang Từ Hàng/Băng Đông) trùng tên. Guard vẫn đánh, chỉ hiệu ứng hào quang lửa có thể sai — chỉnh sau nếu thấy.
5. `AddNpcEx` tham số 7 giờ đặt cả `bNoRevive` ⇒ **boss triệu hồi qua AddNpcEx (bosscharm…) sẽ BIẾN MẤT sau chết thay vì hồi sinh** — đúng bản gốc Linux, và sửa đúng lỗi boss hồi sinh vô hạn; để ý phản hồi người chơi.
6. Vật cản động (`AddObstacleObj 469`) chỉ chặn phía server, không có hình phía client (lệch sẵn có từ Đợt E).
7. Kênh chat phe (CreateChannel…) là stub — chỉ mất chat riêng phe trong tongwar/bw.
8. `s2rExchangeTreePoint` dùng biến `nNpcIndex` nil (LỖI GỐC Linux) — vô hại vì relay không dùng mapId trong nhánh trả điểm; GIỮ nguyên để trùng byte.
9. Điểm Thần Mộc/điểm bang persist ra `settings\jx2sharedata\*.bin` — backup cùng settings.
10. `Sources/S3Client/S3Client.cpp` có sửa CHƯA COMMIT từ phiên trước (static TickCountTMG — fix khựng UI client). Không thuộc đợt này, không đụng.
11. 🔴 **MÚI GIỜ (F21 phản biện xác nhận)**: mọi lịch của 4 tính năng dùng `GetLocalDate`/`date` = giờ LOCAL máy GameServer (**Pacific UTC−8** theo ghi nhận trước). "Chủ nhật 17:00–19:00" của Thành Bảo = **~08:00–10:00 sáng THỨ HAI giờ VN**; Bách Nhân 12:00–24:00, Bang Chiến 20:00–21:30 cũng lệch tương tự. Chủ game cần chọn: (a) đổi TZ máy chủ, hoặc (b) báo tôi đổi các hằng giờ trong script (Thành Bảo phải đổi ĐỒNG BỘ 2 tệp: state chủ `tbRoomCondition` + relay `tbTreeLifeTime`).

---

## 7. TEST SAU RESTART (GM)

**Chung**: `ScriptError.log` không có dòng `tongwar|bairenleitai|missions\bw|tongcastle`; console boot có `[TONGWAR] Driver Bang Chien khoi dong`, `[BAIREN] Bach Nhan Loi Dai khoi dong`, `[TONGCASTLE] Bang Hoi Thanh Bao khoi dong`.

**Tỷ võ (nhanh nhất)**: 3 nhân vật ≥90. Công Bình Tử Dương Châu (1659,3020) → đội 2 người, đội trưởng "Được thôi!" → "2 vs 2" → nhận số thứ tự; "Ta là tuyển thủ" → vào 209 (1620,3202)/(1612,3187); người thứ 3 nhập số qua HỘP SỐ client (AskClientForNumber mới); 2 phút sau đánh nhau CÓ damage, chết KHÔNG mất exp/đồ, bị loại; một phe hết người → về chỗ cũ, giao dịch/bày bán dùng lại được. Hỗn Chiến 16h phải vào **map 210**.

**Bách Nhân**: sau 12:00, NPC Lâm An (1464,3223) → "Ta muốn vào…" (cấp ≥90) → map 960 thấy Xa phu/Rương/Dược điếm; khinh công nhảy lên đài 1 (~1784,3099) → thành Lôi Chủ; 30 s không ai → Cao thủ NPC xuất hiện đánh được; người 2 nhảy lên → 3 s đấu, so sát thương 3 phút; thắng 10 trận đài 1 → thông báo TOÀN server không cắt chữ; +1 triệu exp mỗi 5 phút, 0h bị trả về.

**Bang Chiến (ép mùa)**: `TONGWAR_Adm_InitMatch(9)` sau khi có ≥2 bang chiếm thành → `jx2league.txt` 7 dòng `G 10`; pha 2: `tongwar_redo_start(1,2,1,3,4,2,5,6,3)` (từ gmscript) → map 605-607 mở mission 33; nhân vật ≥90 có bang trong liên minh → NPC Ba Lăng Huyện "Ta muốn tham gia thi đấu" → map báo danh 608+ → đạp trap → hậu doanh; kiểm +75 điểm/kill, 10 chết bị đẩy ra, 90′ kết thúc ghi league/ladder; pha 3 nhận danh hiệu + thưởng (Quả Đại HK 4864 ăn được 200tr exp, Hoàng Chân Đơn 2273 2 tỷ, Cuồng Lan/Phi Phong là gold-equip đúng record).

**Thành Bảo** (⚠ mục 6.1 trước): cần Lâm An có chủ + Chủ nhật 17:00-19:00 (GM đổi giờ hoặc `TONGCASTLE_Adm_ReviveTree(1)` để ép cây). Kiểm: cây 青銅/白銀/黃金 spawn theo `bronzetree/silvertree/goldtree.txt` với TÊN tiếng Việt; bang khác đánh cây → điểm cộng qua relay (xem `settings\jx2sharedata\`); 19:00-24:00 "Ta muốn nhận điểm Thần Mộc" (≥45 phút trong map); đổi Thần Mộc Lệnh (3205-3207) → dùng được (10tr exp / 20 chân nguyên / 60tr+120); bùa 3204 triệu Thủ Vệ gần cây, trừ stack.

---

## 8. PHẢN BIỆN (workflow `phan-bien-port5`: 29 tác nhân / 989 lượt công cụ / 43 phút — 6 miền tìm lỗi + kiểm chứng đối kháng từng phát hiện)

**Kết quả chính thức: 15 phát hiện THẬT (13 đã vá + 2 quyết định vận hành R14/R15 bên dưới), 8 bị bác.** Các bản vá ở commit `2a4d0372`, `367caa17`, `161170cc`, `4df8e433`; DLL build lại → `.moi_2308_tongkim6fix` **md5 `0edd1e40`**:

| # | Mức | Lỗi | Vá |
|---|---|---|---|
| F1 | CAO | bw: **đệ quy vô hạn** OnLeave↔LeaveGame↔DelMSPlayer (KMission gọi OnLeave TRƯỚC khi xoá entry; mỗi vòng ~150KB stack do `KMission Mission;` cục bộ) → sập ngay trận đầu | guard `GetTaskTemp(200) ~= 1` đầu OnLeave (bwmission.lua) |
| F2 | CAO | bw: cổng cấp 90 SignUpFinal bị chèn SAU OpenMission (regex nuốt tới `end;` khối SetMissionS) → mission mở nửa chừng, khoá đài 2 phút | dời lên trước OldSubWorld/OpenMission (sửa tại gốc bw_port.py) |
| F3 | TRUNG | bairen: **RemoteExc thông báo toàn server chết** — engine không nạp `scriptjx2\lib` thành state | chép `script\lib\remoteexc.lua` (tự nạp lúc boot) |
| F4 | THẤP | bairen newworld.lua thiếu Include common.lua (split nil) | thêm Include (đúng SPEC P1) |
| F6 | CAO | engine: `GetNpcAroundNpcList`/`GetAroundNpcList` so **toạ độ cục bộ region** (m_MapX 0-15) giữa các region → guard đâu cũng "gần cây", đứng cạnh cây lại bị từ chối | quy toạ độ toàn cục theo công thức `GetMapDisX/Y` (LOWORD/HIWORD m_RegionID × RegionWidth/Height) |
| F7 | TRUNG | engine: AddNpcEx bNoRevive VÔ ĐIỀU KIỆN — 3 script JX1 SỐNG (bosscharm boss bang hội, seasonnpc_item, spider_web) truyền 1 → boss hết hồi sinh | gate `g_IsJx2Script(L)` (JX1 giữ hồi sinh như cũ) |
| F8 | THẤP | engine: tham số chuỗi `[[...]]` hỏng khi tên chứa `[[` hoặc kết thúc `]` (vd "Bao[GM]") | khử `[[`→`[ [` + đuôi `]` chèn 1 dấu cách |
| F9 | CAO | tongwar: NPC lối vào chết ngay click đầu — state npc_tongwar thiếu `gb_taskfuncs` (Linux 1 state có sẵn) | thêm Include gb_taskfuncs.lua |
| F10/F14 | CAO | **awardtype nExp/nExp_tl chưa đăng ký** → Hoàng Chân Đơn (2 tỷ), Quả Đại HK (200tr), Thần Mộc Lệnh (10tr/60tr) TRỪ ĐỒ mà không trao exp | mới `lib\awardtype\exp_jx1.lua` (nExp→AddOwnExp, nExp_tl→tl_addPlayerExp) + Include vào awardtemplet |
| F11 | TRUNG | tongwar match\newworld thiếu common.lua (split nil mỗi lượt vào/ra 605-607) | thêm Include |
| F12 | THẤP | head.lua:492 hai tham số format còn 29/03/2014, 05/04/2014 | → 29/10/2026, 05/11/2026 |
| F13 | CAO | tongcastle: **Thần Mộc Lệnh dùng không mất** — sIsJx2ItemScript thiếu shenmuling.lua | thêm vào danh sách (engine, đã build) |
| F15 | TRUNG | item dịch chuyển (Thổ Địa Phù thường, Thần Hành Phù…) thoát được map 984 | chặn 1 điểm tại `CheckAllMaps` (forbidmap.lua) — phủ mọi item |
| F16 | TRUNG | Adm_ReviveTree nhân bản record cây ở relay (append) → ×2 điểm thủ | xoá record loại n trước khi spawn |
| F18 | THẤP | cây 1912-1914 ResistMax 25 (Linux 95) | → 95 (server + client npcs.txt); NpcResType giữ enemy197 (boss099 chưa chắc có res client) — lệch ghi nhận |
| F22 | THẤP | nhánh huoyuedu (điểm hoạt bát) spam log mỗi phút Chủ nhật | comment (JX1 không có hệ này) |
| F23 | THẤP | log BHTBKillTree ghi sai người giết (thiếu bối cảnh) | adapter đặt PlayerIndex = kẻ giết |

**Bị bác sau kiểm chứng đối kháng** (không sửa): F5 timerserver quét 209 (hàm LoiDaiHonChien đã TẮT từ trước 20/08 — vẫn đồng bộ 4 chỗ 209→210 trong mã chết để phòng bật lại); F17 skill Thủ Vệ 1208/1210/1212 "khác nghĩa" (finder đếm lệch 1 dòng — thực tế ĐÚNG là hào quang Thủ Vệ).

**Chấp nhận + ghi nhận (không vá đợt này)**: F19 cây chết bởi nguồn không-phải-người (DOT sau khi kẻ đánh thoát) không gọi death script — giới hạn trong 1 phiên, tự phục hồi 19:00; F21 xem mục 6.11; nợ cũ phát hiện kèm: Hỗn Chiến không có cơ chế kết thúc/trao giải (timer tắt từ ≥20/08 mà NPC vẫn nhận báo danh).
