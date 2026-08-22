# ĐẶC TẢ THI CÔNG — CẢNH KỸ TRƯỜNG (`missions/arena`, tag **arena**)

> Lập 22/08/2026 (chỉ đọc, chưa sửa gì). Nguồn: `D:\ServerLinux\server1` (script/settings/ELF `jx_linux_y`), relay `D:\ServerLinux\gateway\s3relay`.
> Đích: `D:\GAMEDEVNEW\Sources` + cây chạy thật `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server`.
> Công cụ sinh kèm: `D:\GAMEDEVNEW\ReverseTools\port_arena\` (`arena_closure.py`, `arena_remap.py`, `check_map975.py`, `scan_offset.py`, `dac_ta_ham_engine_arena.json`).
> Mọi khẳng định đều có `tệp:dòng` hoặc địa chỉ ELF đã đọc thật; chỗ nào không chứng minh được ghi **CHƯA RÕ**.

---

## 0. KẾT LUẬN ĐIỀU HÀNH (đọc 2 phút)

**Cảnh Kỹ Trường trên chính bản Linux là PHẾ TÍCH — không phải "có sẵn và đầy đủ".** Bằng chứng đo thật:

| # | Mảnh chết | Bằng chứng |
|---|---|---|
| 1 | Giao thức báo danh từ bản đồ nhỏ bị chú thích tắt ở **cả hai** nơi | `script/protocol.lua:22` `--"emSCRIPT_PROTOCOL_SIGNUP_AREAN"`; `script_protocol/protocol_def_gs.lua:60-65` (khối `apply_signup` comment) |
| 2 | Hàm GS `apply_signup` của arena **không tồn tại** (chỉ có của `miniencounter/protocol.lua:8`) | grep toàn cây: 0 định nghĩa trong `missions/arena` |
| 3 | 3 hàm GS `on_player_enter_map` / `on_player_leave_map` / `on_begin_battle` **không tồn tại ở đâu** | chỉ có 3 điểm gọi `rule.lua:100`, `:132`, `:250` |
| 4 | 5 hàm GS nhận lệnh từ relay **không tồn tại**: `finded_oppoent`, `wait_map`, `notify_oppoent_cancel`, `player_enter_map` (bản GS), `allocate_map` | chỉ có điểm gọi `player.lua:227,236,245,255,319`, `map.lua:51` |
| 5 | Lớp **`tbMember` không tồn tại** (giữ điểm, phe, vị trí về) | gọi tại `rule.lua:101` `tbMember:new(nCamp)`; không tệp nào định nghĩa |
| 6 | `common.lua` (task rank 3172-3177, 9 danh hiệu) **không ai Include**; `officer.lua` dùng `tbPlayer:GetTitleFlag/SetTitleFlag/PER_WEEK_COUNT` (`officer.lua:33,37,40`) — **3 thứ này không có cả trong `common.lua`** | grep toàn cây |
| 7 | NPC lối vào "Quan Viên Cảnh Kỹ Trường" được thêm rồi **bị xoá ngay lúc boot** | thêm: `global/autoexec_npc.lua:221` (tpl 108, map 176, ô 1451,3233); xoá: `global/autoexec.lua:214` → `ClearNPCNewVersion()` `:275-287` (tên ở `:279`) |
| 8 | `rule.lua` **không ai Include** ⇒ `PreApplyDungeonMap(975)` (`rule.lua:317`) chưa bao giờ chạy trên Linux | grep toàn cây: 0 Include `missions\arena\rule.lua` |
| 9 | Client VNG (`game_y_unpacked.bin`, 22,5 MB) **không có chuỗi `arena`/`Arena`/`SIGNUP_AREAN`** ⇒ không có UI báo danh từ bản đồ nhỏ trong bản client này | quét byte toàn tệp |
| 10 | Shop huy chương `Sale(175,16)` (`officer.lua:70`): `settings/buysell.txt` Linux chỉ **166 dòng** ⇒ shop 175 **không tồn tại cả trên Linux**; 4 hàm engine `Get/Add/Reduce/SetArenaCredits` (ELF 0x08108040/0x08107F40/0x08107E40/0x0812B6D0) không có ai dùng ngoài `exchangeshop/cost/costcurrency.lua:133` | `arena_remap.py` |
| 11 | Công thức ELO, cách chia 30 bậc ghép cặp (`player.lua:357` `Init(30)`), điểm khởi đầu: **mất theo mã GS** | không còn ở đâu (grep `ELO`, `/400`: 0) |
| 12 | 4 tệp `missions\arena\{protocol,player,map,cmd}.lua` trong `server1\script` **trùng byte 100 %** với bản relay `gateway\s3relay\script\missions\arena\` (`cmp` 22/08: 60/359/69/13 dòng) ⇒ bản "GS" chỉ là bản **relay chép sang**, nửa GS thật của `protocol.lua` đã mất; cây relay không có tệp nào khác nhắc `arena` (grep `*.lua` toàn `s3relay\script`: chỉ 4 tệp này trích dẫn nhau; `relaysetting\task\*startarena*` là lôi đài **citywar**, không liên quan) | `cmp`/grep 22/08 |

**Khối lượng thật nếu làm:** chép nguyên byte **15 tệp Linux (9 tệp arena 1.096 dòng + 6 tệp thư viện, bảng A)**; **VIẾT MỚI ≈ 330 dòng Lua** (phần GS của `protocol.lua` ≈ 230 dòng, `tbMember` + ELO ≈ 60, `common_jx1.lua` 20, `death.lua` 8, nối dây 6 dòng) + **DIFF 1 tệp Linux** (`npc/officer.lua` +2 Include, +2 mục menu, +cổng cấp 90); **engine 10 hàm Lua nhỏ** (AddTimer/DelTimer/Msg2Map/RemoteExecute/SetTmpCamp/GetTmpCamp/SetDeathType/PreApplyDungeonMap/ApplyDungeonMap/ReturnDungenonMap, ≈ 250 dòng C++, đặc tả từ disasm ở mục C) — **không đụng client, không cần restart client**.

### KẾT LUẬN: **NÊN HOÃN theo đúng luật chủ game** ("Linux có sẵn và ĐẦY ĐỦ thì làm luôn") — Cảnh Kỹ Trường không đầy đủ: ~30 % logic (toàn bộ phần GS + lớp điểm + lối vào + công thức điểm) phải **tự thiết kế**, không chép được. Về kỹ thuật thì **LÀM ĐƯỢC CÓ ĐIỀU KIỆN** trong một đợt (≈ 1–1,5 ngày, không cần client) **nếu chủ game chấp nhận 5 khác biệt không tránh được**:
1. Báo danh qua **menu NPC Quan Viên** (Lâm An) thay vì chuột phải bản đồ nhỏ (client không có UI).
2. **Công thức ELO + chia bậc + điểm khởi đầu** do ta chọn (đề xuất ở C.5, ghi rõ là tự chọn).
3. **Bỏ shop huy chương** (mục menu giữ nguyên chữ nhưng mở shop không tồn tại → giữ đúng hiện trạng Linux = không mở được).
4. Đợt 1 chỉ **1 sân** (map 975): cặp thứ hai phải chờ (ghép cặp vẫn chạy, hệ thống báo "đang chờ sân"). Muốn nhiều sân phải vá `MapList.ini` cả server lẫn **client** (mục B.3).
5. Danh hiệu 9 bậc chỉ là **chuỗi tính từ điểm** (`common.lua:63-74`), không gắn lên đầu nhân vật (Linux cũng đã chú thích tắt `officer.lua:44-55`).

Nếu chủ game đồng ý 5 điểm trên thì theo mục A→I dưới đây là làm ngay được, không cần mở lại cây Linux.

---

## A. DANH SÁCH TỆP CHÉP (bao đóng Include, `arena_closure.py`)

Bao đóng Include đệ quy của `missions/arena` (dừng ở tệp dự án đã có): **22 tệp** — 16 THIẾU, 3 DIFF, 3 IDENT. Sau khi trừ 2 tệp đã có ở `scriptjx2\lib` (engine tự ánh xạ `\script\lib\` → `scriptjx2\lib\`, `ScriptFuns.cpp:1941-1942`), còn:

| # | Tệp Linux (`script\…`) | Dòng | Trạng thái dự án | Việc | Ghi chú |
|---|---|---|---|---|---|
| 1 | `missions\arena\rule.lua` | 321 | THIẾU | **chép nguyên byte** | luật sân đấu; KHÔNG sửa (xem G.4 về `SetPKFlag`) |
| 2 | `missions\arena\player.lua` | 360 | THIẾU | chép nguyên byte | logic ghép cặp (phía relay) |
| 3 | `missions\arena\map.lua` | 69 | THIẾU | chép nguyên byte | quản lý sân (phía relay) |
| 4 | `missions\arena\protocol.lua` | 61 | THIẾU | **chép nguyên byte NHƯNG ĐỔI TÊN → `missions\arena\protocol_relay.lua`** | đây là NỬA RELAY của giao thức; nửa GS (cùng tên `protocol.lua` trên Linux) đã mất → ta viết mới ở đúng tên `protocol.lua` (mục C.4). Đổi tên để `rule.lua:100,132,250` (`DynamicExecute("…\arena\protocol.lua", "on_player_*")`) và `player.lua:227…319` (`RemoteExecute("…\arena\protocol.lua", …)`) trỏ vào nửa GS mà **không sửa 2 tệp đó** |
| 5 | `missions\arena\cmd.lua` | 14 | THIẾU | chép nguyên byte | lệnh GM relay `get_status`; chết vô hại (state riêng) |
| 6 | `missions\arena\common.lua` | 87 | THIẾU | chép nguyên byte | task 3172-3177, 9 danh hiệu; **thiếu 3 hàm** → bổ sung ở tệp mới `common_jx1.lua` (C.4) |
| 7 | `missions\arena\vnforbidstate.lua` | 101 | THIẾU | chép nguyên byte | 92 trạng thái buff gỡ khi vào sân — **toàn bộ id TRÙNG** với `skills.txt` dự án (bảng D.2) |
| 8 | `missions\arena\npc\officer.lua` | 88 | THIẾU | **chép + DIFF** (mục C.4 / F) | +Include `common_jx1.lua`, +Include `lib\awardtype\exp.lua`, +2 mục menu Báo danh / Huỷ, +cổng cấp 90 |
| 9 | `missions\arena\npc\yaodian.lua` | 3 | THIẾU | chép nguyên byte | `Sale(53)` = shop thuốc; dòng 54 `buysell.txt` hai cây đều là shop thuốc 1..N (`arena_remap.py`) |
| 10 | `missions\basemission\dungeon.lua` | 141 | THIẾU | chép nguyên byte | khung phó bản (`Dungeon:new_type/new/free/close/AddTimer/OnTime`) |
| 11 | `missions\basemission\lib.lua` | 76 | THIẾU | chép nguyên byte | `basemission_CallNpc` (AddNpcEx, `lib.lua:33`) ; Include `lib\common.lua` (IDENT) + `lib\coordinate.lua` (có ở `scriptjx2\lib`, IDENT) |
| 12 | `global\autoexec_head.lua` | 23 | THIẾU | chép nguyên byte | `AutoFunctions` (`rule.lua:319` đăng ký `init_rule`) |
| 13 | `item\forbiditem.lua` | 165 | THIẾU | chép nguyên byte | `set_MapType`/`tb_MapType` (`rule.lua:35-43`); nạp `settings\item_type.txt`, `item_detail.txt` (B.1) |
| 14 | `maps\checkmap.lua` | 88 | THIẾU | chép nguyên byte | `IsCityMap/IsFreshmanMap` (`officer.lua:69`) |
| 15 | `battles\openbox.lua` | 3 | THIẾU | chép nguyên byte | NPC Rương chứa đồ (`rule.lua:25`); `OpenBox` đã đăng ký `ScriptFuns.cpp:14370` |
| — | `missions\basemission\mapscript.lua` | 33 | THIẾU | **KHÔNG chép** | trên JX1 mỗi tệp một `lua_State`: `DungeonList` của mapscript.lua rỗng ⇒ `mapscript.lua:8-10` đá mọi người về Thành Đô (`NewWorld(11,3143,5065)`). Logic `OnNewWorld/OnLeaveWorld` (`mapscript.lua:5-33`) được đưa vào `protocol.lua` mới (C.4) và `MapList.ini` trỏ `975_NewWorldScript` sang đó (B.2) |
| — | `lib\file.lua`, `lib\coordinate.lua`, `lib\objbuffer_head.lua` | 65/251/119 | **ĐÃ CÓ, IDENT** (`scriptjx2\lib\` hoặc `script\lib\`) | không chép | `cmp` trùng byte |
| — | `lib\common.lua`, `dailogsys\dailogsay.lua` | 171/180 | ĐÃ CÓ, IDENT | không chép | |
| — | `lib\awardtemplet.lua`, `global\titlefuncs.lua`, `item\heart_head.lua` | 61/53/289 | ĐÃ CÓ, **DIFF** | **dùng bản dự án** | bản dự án là tập mở rộng (awardtemplet +Include item_jx1; titlefuncs +nối danh hiệu JX1; heart_head bản JX1 thiếu `add_forbit_templatemap` → `rule.lua:321` DynamicExecute chỉ ghi 1 dòng log `[WLLS] DynamicExecute: script chua nap/LOI`, vô hại) |

**Tệp MỚI (viết tay, mục C.4):** `missions\arena\protocol.lua` (nửa GS + `tbMember` + ELO + OnNewWorld/OnLeaveWorld + báo danh), `missions\arena\common_jx1.lua`, `missions\arena\death.lua`.

**Tệp dự án phải sửa (mỗi chỗ 1 dòng, mục E/F):** `settings\MapList.ini` (server), `startgame.lua`, `startgame\thanh\laman.lua`, `player\playerlogout.lua`.

> Luật chép: dùng `copy /b` hoặc `shutil.copyfile` (KHÔNG qua Edit/Write tool — tệp GBK/TCVN3 bị phá byte). Sau khi chép: `fc /b` hoặc `filecmp.cmp` từng tệp = phải trùng (trừ `officer.lua`).

---

## B. SETTINGS / MAP

### B.1 Settings chép
| Tệp | Linux | Dự án | Việc |
|---|---|---|---|
| `settings\missions\arena\readypos.txt` | 32 điểm (`TRAPX TRAPY` pixel, dòng 2-33) | THIẾU | chép nguyên |
| `settings\missions\arena\battlepos.txt` | 15 điểm | THIẾU | chép nguyên |
| `settings\item_type.txt` | 13 dòng | THIẾU | chép nguyên (để `forbiditem.lua:47-64` không in lỗi lúc boot; cần cho hook tuỳ chọn C.6) |
| `settings\item_detail.txt` | 215 dòng | THIẾU | chép nguyên (id Linux — chỉ hook C.6 mới dùng, khi đó phải remap theo tên) |
| `settings\map_type.txt` | 34 dòng | CÓ (36 dòng, DIFF) | **giữ bản dự án** |
| `settings\dungeonmap.ini` | 3 dòng `[IgnoredTempMap] Count=1 Map1=948` | THIẾU | chép + **nối thêm** `[Copies]` `975=975` (đọc bởi `PreApplyDungeonMap` JX1, C.3) |

Kiểm chứng toạ độ (`check_map975.py`, đọc `.wor` trong `bin\server\pak\jingjichang.mps` và `maps.pak` Linux — trùng nhau): map 975 `rect=96,97,105,102` (region tuyệt đối) = pixel x[49152..53760] y[99328..104448]; **29 tệp `_Region_S.dat`** có trong pak; **32/32 readypos, 15/15 battlepos, 2/2 NPC (`rule.lua:25-26`: ô 1566,3214 và 1578,3204) đều nằm trong rect**.

### B.2 MapList / WorldSet
- Map 975 **đã nạp**: `Maps\WorldSet_GameServer.ini:906` `World901=975`; `settings\MapList.ini:6330-6333` (`975=特殊用地\jingjichang`, `975_NewWorldScript=\script\missions\basemission\mapscript.lua`, `975_NewWorldParam=PARTNER_OFF|TISHENZHIREN|NOTONGCLAIMWAR|STALL_OFF|FIGHTSTATE_ON`). Client cũng có dòng 975 (`bin\client\settings\MapList.ini:6330`). Pak client `updatejx10.pak` có map (PHANTICH 4.1).
- **SỬA 1 DÒNG** (server): `settings\MapList.ini:6332` `975_NewWorldScript=\script\missions\basemission\mapscript.lua` → `975_NewWorldScript=\script\missions\arena\protocol.lua`. Cơ chế: `KSubWorld_FireMapScript` (`ScriptFuns.cpp:10923-10947`) gọi `OnNewWorld`/`OnLeaveWorld` trong **state riêng** của tệp đó với `SubWorld`/`PlayerIndex` đã bơm; gọi từ `KNpc::ChangeWorld` (`KNpc.cpp:9913-9914`) và `KPlayerSet::PrepareRemove` (đăng xuất = rời map, `KPlayerSet.cpp:374`). `NewWorldParam` của dự án chỉ hiểu `CreateTeam_OFF|PARTNER_OFF|TISHENZHIREN` (`maps\newworldscript_default.lua:13-23`) ⇒ `FIGHTSTATE_ON/STALL_OFF` bị bỏ qua, vô hại.
- Không cần dòng nào trong `settings\task\missions.txt` (arena dùng khung Dungeon, không dùng mission) và `settings\TimerTask.txt` (timer qua `AddTimer` C++ mới).

### B.3 Nhiều sân (tuỳ chọn, đợt sau)
Pool bản sao tĩnh = các id trỏ **cùng thư mục map** (cách JX1 đang làm: `MapList.ini:2281/2285/2289` 209/210/211 = `中原南区\演武场一`; 213..220 cùng map lôi đài). Muốn 2-3 sân: chọn id **chưa nạp trong WorldSet** (MapList dự án đã dùng đủ 1000 id 0..999 — `grep -c` = 1000; ví dụ 978-983 đang là `banghuichengbao` chưa nạp, nhưng có thể đợt Thành Bảo cần), đổi dòng `<id>=特殊用地\jingjichang` + `<id>_NewWorldScript` ở **cả** `bin\server\settings\MapList.ini` lẫn **`bin\client\settings\MapList.ini`** (client tra id→thư mục khi vào map), thêm `World9xx=<id>` vào `WorldSet_GameServer.ini`, và `[Copies] 975=975,<id>…` trong `dungeonmap.ini`. **CHƯA RÕ** client có nhận id ≥ 1000 không (`KSubWorldSet::SearchWorld` server tìm tuyến tính theo DWORD, `KSubWorldSet.cpp:29-37`, không giới hạn).

---

## C. HÀM ENGINE THIẾU — đặc tả từ disasm + gợi ý cài đặt

`api_gap2.py arena missions/arena` ⇒ **[ENG] 6**: `Msg2Map`(×13), `RemoteExecute`(×6), `SetTmpCamp`(×2), `SetDeathType`(×2), `AddTimer`(×1), `PreApplyDungeonMap`(×1); **[LIB] 6** (đều nằm trong bao đóng A). Khung `dungeon.lua` kéo thêm `ApplyDungeonMap` (`:59`), `ReturnDungenonMap` (`:79`), `DelTimer` (`:73`). Từ 21/08 dự án **đã có**: `ForbidEnmity` (`ScriptFuns.cpp:13833`), `ClearMapNpcWithName`, `ST_Start/Stop/GetDamageCounter` (`:13899-13922`, đếm sát thương **hứng chịu** — đúng nghĩa `rule.lua:170`), `SetLogoutRV`, `SetPunish`, `ForbidChangePK` (stub no-op `KJx2WarInfra.cpp:234`), `ForbitStamina`, `SetCreateTeam`, `DisabledStall`, `SetPKFlag` (= alias `SetFightState`, `KJx2WarInfra.cpp:227-230`), `RemoveSkillState`, `GetLocalDate` (strftime, `:3212`), `GetSysCurrentTime`, `WriteLog`, `AddNpcEx`, `SetNpcScript`, `OB_*`/ObjBuffer (`KJx2SharedStore.cpp`), `DynamicExecute`/`DynamicExecuteByPlayer` (`:2333-2429`), `curpack/usepack` stub (`:2181-2191`), `IncludeLib` (21 module, `:2449`).

Đặc tả đầy đủ từng hàm (chữ ký, ngữ nghĩa từ disasm, offset struct, hàm JX1 tương đương, mã gợi ý, bẫy) ở **`ReverseTools\port_arena\dac_ta_ham_engine_arena.json`** (11 mục). Tóm tắt:

### C.1 Bắt buộc (10 hàm Lua, ≈ 250 dòng C++, đặt ở `KJx2WarInfra.cpp` hoặc tệp mới `KJx2Arena.cpp`; đăng ký trong bảng `ScriptFuns.cpp` cạnh dòng 14826)

| Hàm | ELF | Chữ ký / ngữ nghĩa (đã đọc disasm) | JX1 tương đương | Cài |
|---|---|---|---|---|
| `AddTimer(nFrame, szFun, nParam) → id` | `0x08100D40` | gettop>2; lấy script của state gọi (`0x08220780`); tạo object timer (`0x081CDA80` → `0x081CC520`: +4 tên hàm[0x103], +0x108 script, +0x10c param) đưa vào `g_TimerSet[0x82e8cac]` (`0x0804E470`). Khi đến hạn (`0x081CC300`): gọi **theo tên** qua `0x08221ED0` với `(nParam, nTimerId)`, 2 kết quả; `0x08221ED0` tách `Obj:Fn` (`0x08221EFC cmp 0x3a`) / `Obj.Fn` (`0x08221FD8 cmp 0x2e`); kết quả[-2] = frame kế tiếp (0 ⇒ huỷ), [-1] = param mới | KHÔNG có (StartGlbMSTimer chỉ gọi `OnTimer()` theo khoá TimerTask, `KJx2League.cpp:955`) | vector `{id, nScriptIdx, szFun, nParam, dwNextMs}`; tìm script: quét `g_ScriptSet[i].m_LuaState == L` (`KSortScript.h:18`); tick trong `CoreServerShell::Breathe` cạnh `KJx2GlbMission_Breathe()` (`KJx2League.cpp:1192`); frame→ms ×1000/18 |
| `DelTimer(id) → 1/0` | `0x08100CA0` | `0x0804E4B0` Del theo id, `setne` | KHÔNG | xoá theo id |
| `Msg2Map(nMapId, szMsg)` | `0x08105080` | SearchWorld (`0x080F68A0`) → lặp player trong SubWorld (`0x080EF5C0/0x080EF640`, stride 0x63FC8) → `0x081C9220(1, idx, …)` = kênh Msg2Player | **`LuaMsgToAroundRegion`** (`ScriptFuns.cpp:3582-3608`) y hệt | 1 dòng: `{"Msg2Map", LuaMsgToAroundRegion}` |
| `RemoteExecute(szFile, szFun, hParam[, szCb, nCbParam, dwGS])` | `0x08100740` | gettop>2; đóng gói protocol `0x3D` (`0x0810091C`) gửi tiến trình kia, bên kia gọi `szFun(ParamHandle, ResultHandle, dwGSID)`; có callback ⇒ gọi `szCb(nCbParam, ResultHandle)` trong script **đã gọi** (mẫu `lib/remoteexc.lua:23,30`) | KHÔNG (1 GS). `DynamicExecute` (`:2380`) không trả kết quả, không callback | **đồng bộ cùng tiến trình**: `hRes=OB_Create` → `lua_dostring(target, "fn(hParam,hRes,1)")` → nếu có callback `lua_dostring(L, "cb(nCbParam,hRes)")` → `OB_Release(hRes)`. Tách `LuaOB_Create` (`KJx2SharedStore.cpp:66`) thành helper C |
| `SetTmpCamp(nCamp[, nNpcIdx]) → 1/0` | `0x0810BA50` → `KNpc::SetTmpCamp 0x0807B2D0` | ghi **Npc+0x1900** (phe tạm), đồng bộ (`0x0807B260`); nCamp<0 ⇒ 0 | `KNpc::SetCurrentCamp` (`KNpc.cpp:442`), `SetCurCamp` (`:7847`); camp 1/2 = justice/evil (`GameDataDef.h:487-488`) — đúng cách `citywar_arena/head.lua:91` | `nCamp>0 ⇒ SetCurrentCamp(nCamp)`; `0 ⇒ SetCurrentCamp(m_Camp)` |
| `GetTmpCamp([nNpcIdx])` | `0x0810BB70` | đọc Npc+0x1900 | `m_CurrentCamp` | trả 0 nếu = m_Camp |
| `SetDeathType(n)` | `0x08110580` | ghi **Player+0xE4**; `KNpc::DoDeath` (`0x08089534-0x08089596`) đọc: ≠0 ⇒ **bỏ phạt khi chết** (>0 giảm dần, −1 vô hạn), 0 ⇒ phạt | `m_nCurPKPunishState` (`KNpc.h:515`); =3 ⇒ `DoDeath` return ngay (`KNpc.cpp:8091`) | `n≠0 ⇒ state=3; n=0 ⇒ state=0` |
| `PreApplyDungeonMap(tpl[,p2[,p3]])` | `0x08126FE0` → `0x081CBCD0` | tạo mục `std::map<tpl, DungeonTemplate>` (`0x081CB5A0`), `Init(p2,p3)` (`0x0820F560`) — ý nghĩa p2/p3 **CHƯA RÕ** (cả cây gọi (0,0) trừ maze (0,1), dungeonmanager (1,1)) | KHÔNG có map động; JX1 nhân bản bằng nhiều id cùng thư mục | kho tĩnh: đọc `settings\dungeonmap.ini [Copies] <tpl>=id,…` (mặc định `{tpl}`), chỉ nhận id `SearchWorld>=0` |
| `ApplyDungeonMap(tpl) → id/0` | `0x08126F50` | gettop **==1**; tìm mục (`0x081CB510`), Apply | KHÔNG | id rảnh đầu tiên trong kho, đánh dấu bận |
| `ReturnDungenonMap(tpl, id) → 1/0` | `0x08126E10` (tên gốc sai chính tả — giữ) | gettop ==2; `0x081CB400` → `0x0820F760` | KHÔNG | trả về kho |

Tuỳ chọn (cây khác sẽ cần, arena không gọi): `ApplyDelDungeonMap` (`0x08126EC0`, 2 tham số, no-op), `IsGSHaveFreeMap()` (`0x081270D0`, trả số slot SubWorld trống).

### C.2 Bẫy engine phải nhớ khi cài
1. **`AddTimer` là điều kiện sống còn**: `player.lua:360` gọi `AddTimer` ở **top-level**; nếu nil ⇒ `KLuaScript::Load` trả FALSE ⇒ `KSortScript.cpp` (FIX 14/08) **bỏ hẳn slot** ⇒ `protocol_relay.lua`, `player.lua`, `officer.lua`, `rule.lua` không được nạp. Mỗi state standalone của 4 tệp đó đều đăng ký 1 timer `tbPlayerManage:OnTime` 30 frame trên bảng rỗng — vô hại, chấp nhận để giữ byte-identical.
2. **`RemoteExecute` đồng bộ ⇒ đệ quy**: relay `SearchOpponents` (`player.lua:155`) → `NotifyEnterMap` → GS `player_enter_map` → `NewWorld` là **tức thời** (`ScriptFuns.cpp:3827 ChangeWorld`) → `OnNewWorld` → `rule.lua:87 OnEnterMap` → `:100 on_player_enter_map` → relay `player_enter_map` **trong khi** relay đang lặp `for szName,pPlayerInfo in self.tbMap` (`player.lua:271`). ⇒ nửa GS **xếp hàng** yêu cầu vào map và thực hiện ở tick sau (`AddTimer(1, "ARENA_ProcEnterQueue", 0)`). Mã ở C.4 đã làm vậy.
3. `Dungeon:new` (`dungeon.lua:44`) gọi `unpack(tbInitParam)` — `unpack(nil)` lỗi Lua 4 ⇒ luôn truyền `{}`.
4. `map.lua:51` gọi `RemoteExecute(…, "allocate_map", 0, nil, 0, nil)` — handle = 0 ⇒ handler GS không được đụng ParamHandle.
5. `DynamicExecute` dự án ghép chuỗi gọi bằng `lua_dostring` (`ScriptFuns.cpp:2400-2413`): **không truyền được table**, chuỗi cắt 64 ký tự ⇒ `rule.lua:250` `on_begin_battle(self.nMapId, self.tbMemberMap)` nhận `tbMemberMap = nil` ⇒ nửa GS tự đọc `DungeonList[nMapId].tbMemberMap` (cùng state vì `protocol.lua` Include `rule.lua`).
6. `SetPunish` của JX1 **NGƯỢC** Linux (chi tiết G.1). Với arena không sao vì `SetDeathType` ghi sau (`rule.lua:107→109`, `:123→124`).

### C.3 Luồng dữ liệu sau khi ghép (1 GS, thay relay)
```
[officer.lua] Báo danh → DynamicExecuteByPlayer(protocol.lua,"apply_signup")
  → GS: set_last_state{name,{rank,fightstate,map,x,y}} → relay (protocol_relay.lua:14)
  → GS: apply_search_opponents{name,nGroup} → relay (protocol_relay.lua:6) → cb ARENA_CbSignup (−1 đã trong hàng / 1 ghép xong / nil chờ)
relay timer 30 frame (player.lua:360) → Proc/Active (player.lua:283,270)
  → ghép được: NotifyMatched → GS finded_oppoent (thông báo) ; ApplyMap → chưa có sân → allocate_map → GS ApplyDungeonMap(975) → reg_map → relay RegMap (protocol_relay.lua:52)
  → tick sau: NotifyEnterMap → GS player_enter_map{name,map,timeout} → GS xếp hàng → tick sau: Dungeon:new(map,975,{}) nếu chưa có → SetDeathScript → NewWorld(map, readypos)
  → OnNewWorld (protocol.lua = NewWorldScript 975) → rule OnEnterMap (rule.lua:87-118) → tbMember:new → on_player_enter_map → relay player_enter_map → trả tbLastState → cb → pDungeon:SetLastState (rule.lua:147) → đủ 2 bReady → StartBattle (rule.lua:247)
  → on_begin_battle → relay begin_battle (protocol_relay.lua:43) xoá hồ sơ relay
  → Dungeon:AddTimer 55 s → OnTime (rule.lua:284): 5 s đếm ngược → ReadyFight (dịch tới battlepos) → 5 s → StartFight (SetFightState 1, ST_StartDamageCounter) → 300 s → ProcResult
  → chết (death.lua → ARENA_OnPlayerDeath) / rời map / đăng xuất (OnLeaveWorld, KPlayerSet.cpp:374) ⇒ bDeath → ProcResult (rule.lua:163) → SetResult → tbMember:SetResult → ELO vào task 3172-3177 → WriteLog → close → OnClose → GoToLastPos → ReturnDungenonMap
```

### C.4 MÃ VIẾT MỚI (Lua 4.0 — chép nguyên vào cây; mọi tệp ghi bằng script python `open(...,'wb')` hoặc `Write` tool **chỉ cho tệp ASCII thuần**; chuỗi tiếng Việt có dấu trong 3 tệp dưới phải để TCVN3 — viết bằng `vn_to_octal` của skill `swordonline-dev`, hoặc tạm để không dấu)

#### C.4.1 `script\missions\arena\common_jx1.lua` (MỚI)
```lua
-- Bo sung 3 thu officer.lua:33,37,40 goi ma common.lua Linux khong co (mat theo ma GS)
Include("\\script\\missions\\arena\\common.lua")
tbPlayer.PER_WEEK_COUNT = 20            -- officer.lua:22 "moi tuan ... 20 tran"
tbPlayer.TSK_TITLE_FLAG = 3178          -- task MOI (3172-3177 la cua Linux; 3100-3300 du an chua dung)
function tbPlayer:GetTitleFlag()        -- ~= 0 khi tuan nay da nhan
	local nYW = tonumber(GetLocalDate("%Y%W"))
	if GetTask(self.TSK_TITLE_FLAG) == nYW then return 1 end
	return 0
end
function tbPlayer:SetTitleFlag()
	SetTask(self.TSK_TITLE_FLAG, tonumber(GetLocalDate("%Y%W")))
end
```

#### C.4.2 `script\missions\arena\death.lua` (MỚI) — engine JX1 gọi `OnPlayerDeath(nPlayerIdx, nKillerNpcIdx)` (`KNpc.cpp:1542`), KHÔNG phải `OnDeath`
```lua
function OnPlayerDeath(nPlayerIdx, nKiller)
	DynamicExecuteByPlayer(nPlayerIdx, "\\script\\missions\\arena\\protocol.lua", "ARENA_OnPlayerDeath")
end
```

#### C.4.3 `script\missions\arena\protocol.lua` (MỚI — nửa GS; cũng là `NewWorldScript` của map 975)
```lua
-- CANH KY TRUONG - nua GameServer cua giao thuc (ban Linux da mat; viet lai tu call site).
-- Nua relay (byte Linux) = protocol_relay.lua (player.lua/map.lua). Cung 1 tien trinh, goi qua RemoteExecute C++.
Include("\\script\\lib\\objbuffer_head.lua")
Include("\\script\\missions\\arena\\rule.lua")        -- keo Dungeon/DungeonList/DungeonType["arena"] vao state nay
Include("\\script\\missions\\arena\\common_jx1.lua")  -- tbPlayer (task 3172-3178)

ARENA_RELAY   = "\\script\\missions\\arena\\protocol_relay.lua"
ARENA_SELF    = "\\script\\missions\\arena\\protocol.lua"
ARENA_TPL     = 975
ARENA_ELO_K   = 32        -- TU CHON (Linux mat ma): ELO chuan K=32
ARENA_ELO_INIT= 1000      -- TU CHON: < 1200 => "Nhap Mon De Tu" (common.lua:23)
ARENA_GROUPS  = 30        -- player.lua:357 tbPlayerManage:Init(30)
ARENA_MIN_LEVEL = 90      -- chinh sach chu game
tbEnterQueue = {}

-- ============ tbMember (rule.lua:101 goi; lop nay KHONG ton tai tren Linux) ============
tbMember = {}
function tbMember:new(nCamp)             -- boi canh nguoi choi (rule.lua:87 OnEnterMap qua CallPlayerFunction)
	local tb = {}
	for k, v in self do tb[k] = v end
	tb.szName = GetName()
	tb.nCamp = nCamp
	tb.bDeath = 0
	tb.bReady = 0
	tb.nReceiveDamage = 0
	tb.nRank = tbPlayer:GetRank()
	if tb.nRank <= 0 then tb.nRank = ARENA_ELO_INIT end
	tb.nLastFightState = 0
	tb.tbLastPos = nil
	return tb
end
function tbMember:SyncLastState(tbLast)  -- rule.lua:150 ; tbLast = {nRank, nFightState, nMapId, nX, nY} (tu apply_signup)
	if type(tbLast) == "table" then
		if tbLast[1] and tbLast[1] > 0 then self.nRank = tbLast[1] end
		self.nLastFightState = tbLast[2] or 0
		if tbLast[3] then self.tbLastPos = {tbLast[3], tbLast[4], tbLast[5]} end
	end
	self.bReady = 1
end
function tbMember:SetResult(nOppRank, szResult)   -- rule.lua:213-222
	local nIdx = SearchPlayer(self.szName)
	if nIdx > 0 then
		CallPlayerFunction(nIdx, ARENA_ApplyResult, self.nRank, nOppRank, szResult)
	end
end
function tbMember:GoToLastPos()          -- rule.lua:230
	local nIdx = SearchPlayer(self.szName)
	if nIdx <= 0 then return end
	if self.tbLastPos and self.tbLastPos[1] and self.tbLastPos[1] ~= ARENA_TPL then
		CallPlayerFunction(nIdx, NewWorld, self.tbLastPos[1], self.tbLastPos[2], self.tbLastPos[3])
	else
		CallPlayerFunction(nIdx, NewWorld, 20, 3546, 6226)   -- rule.lua:66 KickOut
	end
end

-- ============ ELO (TU CHON) ============
function ARENA_EloNew(nSelf, nOpp, szResult)
	if nSelf <= 0 then nSelf = ARENA_ELO_INIT end
	if nOpp <= 0 then nOpp = ARENA_ELO_INIT end
	local nScore = 0.5
	if szResult == "VICTORY" then nScore = 1 elseif szResult == "LOSE" then nScore = 0 end
	local fExpect = 1 / (1 + 10 ^ ((nOpp - nSelf) / 400))
	local nNew = floor(nSelf + ARENA_ELO_K * (nScore - fExpect) + 0.5)
	if nNew < 1 then nNew = 1 end
	return nNew
end
function ARENA_Group(nRank)              -- 30 bac: CHUA RO cach Linux chia; tu chon rank/100
	local n = floor(nRank / 100)
	if n < 1 then n = 1 end
	if n > ARENA_GROUPS then n = ARENA_GROUPS end
	return n
end
function ARENA_ApplyResult(nSelfRank, nOppRank, szResult)   -- boi canh nguoi choi
	local nNew = ARENA_EloNew(nSelfRank, nOppRank, szResult)
	SetTask(tbPlayer.TSK_RANK, nNew)
	if szResult == "VICTORY" then SetTask(tbPlayer.TSK_VICTORY_COUNT, GetTask(tbPlayer.TSK_VICTORY_COUNT) + 1)
	elseif szResult == "LOSE" then SetTask(tbPlayer.TSK_LOSE_COUNT, GetTask(tbPlayer.TSK_LOSE_COUNT) + 1)
	else SetTask(tbPlayer.TSK_DRAW_COUNT, GetTask(tbPlayer.TSK_DRAW_COUNT) + 1) end
	local nYW = tonumber(GetLocalDate("%Y%W"))           -- common.lua:55 cung khoa tuan
	if GetTask(tbPlayer.TSK_CUR_WEEK) ~= nYW then
		SetTask(tbPlayer.TSK_CUR_WEEK, nYW)
		SetTask(tbPlayer.TSK_CUR_COUNT, 0)
	end
	SetTask(tbPlayer.TSK_CUR_COUNT, GetTask(tbPlayer.TSK_CUR_COUNT) + 1)
	Msg2Player(format("Canh Ky Truong: %s. Diem %d -> %d (%s). Tuan nay %d tran.", szResult, nSelfRank, nNew, tbPlayer:GetTitle(), GetTask(tbPlayer.TSK_CUR_COUNT)))
end

-- ============ NewWorldScript cua map 975 (= mapscript.lua:5-33 dua vao dung state) ============
Include("\\script\\maps\\newworldscript_default.lua")
function OnNewWorld(szParam)
	local nMapId = SubWorldIdx2ID(SubWorld)
	local pDungeon = DungeonList[nMapId]
	OnNewWorldDefault(szParam)
	if not pDungeon then
		NewWorld(11, 3143, 5065)                          -- mapscript.lua:10: vao san khong qua ghep cap => ve Thanh Do
		return
	end
	CallPlayerFunction(PlayerIndex, pDungeon.OnEnterMap, pDungeon)
	SetFightState(0)  -- JX1: SetPKFlag == SetFightState (KJx2WarInfra.cpp:227) nen rule.lua:110 vua bat chien dau; tra ve 0 cho 60 s chuan bi (rule.lua:276 se bat lai)
end
function OnLeaveWorld(szParam)
	local nMapId = SubWorldIdx2ID(SubWorld)
	local pDungeon = DungeonList[nMapId]
	OnLeaveWorldDefault(szParam)
	if pDungeon then
		CallPlayerFunction(PlayerIndex, pDungeon.OnLeaveMap, pDungeon)
	end
end

-- ============ relay -> GS (player.lua:227,236,245,255,319 ; map.lua:51) ============
function ARENA_MsgByName(szName, szMsg)
	local nIdx = SearchPlayer(szName)
	if nIdx > 0 then CallPlayerFunction(nIdx, Msg2Player, szMsg) end
end
function finded_oppoent(ParamHandle, ResultHandle)
	ARENA_MsgByName(ObjBuffer:PopObject(ParamHandle), "Canh Ky Truong: da tim duoc doi thu, dang sap xep san dau.")
end
function wait_map(ParamHandle, ResultHandle)
	ARENA_MsgByName(ObjBuffer:PopObject(ParamHandle), "Canh Ky Truong: san dau dang ban, vui long cho.")
end
function notify_oppoent_cancel(ParamHandle, ResultHandle)
	ARENA_MsgByName(ObjBuffer:PopObject(ParamHandle), "Canh Ky Truong: doi thu da huy, tiep tuc tim doi thu khac.")
end
function allocate_map(ParamHandle, ResultHandle)      -- map.lua:51 (handle = 0, khong doc)
	local nMapId = ApplyDungeonMap(ARENA_TPL)
	if nMapId > 0 then
		local h = OB_Create()
		ObjBuffer:PushObject(h, nMapId)
		RemoteExecute(ARENA_RELAY, "reg_map", h, nil, 0, nil)   -- protocol_relay.lua:52
		OB_Release(h)
	end
end
function player_enter_map(ParamHandle, ResultHandle)  -- player.lua:316-318: name, mapid, timeout
	local szName = ObjBuffer:PopObject(ParamHandle)
	local nMapId = ObjBuffer:PopObject(ParamHandle)
	local nTimeOut = ObjBuffer:PopObject(ParamHandle)
	tinsert(tbEnterQueue, {szName, nMapId})
	AddTimer(1, "ARENA_ProcEnterQueue", 0)              -- hoan sang tick sau (C.2 bay 2)
end
function ARENA_ProcEnterQueue(nParam, nTimerId)
	local tb = tbEnterQueue
	tbEnterQueue = {}
	for i = 1, getn(tb) do
		local szName, nMapId = tb[i][1], tb[i][2]
		local nIdx = SearchPlayer(szName)
		if nIdx > 0 then
			local pDungeon = DungeonList[nMapId]
			if not pDungeon then
				pDungeon = DungeonType["arena"]:new(nMapId, ARENA_TPL, {})   -- dungeon.lua:24 ; {} vi unpack(nil) loi
			end
			if pDungeon then
				local nX, nY = pDungeon:GetReadyPos()                      -- rule.lua:236
				CallPlayerFunction(nIdx, SetDeathScript, "\\script\\missions\\arena\\death.lua")
				CallPlayerFunction(nIdx, NewWorld, nMapId, nX, nY)          -- -> OnNewWorld -> rule OnEnterMap
			end
		end
	end
	return 0
end

-- ============ GS -> relay (rule.lua:100,132,250 goi vao tep nay) ============
function on_player_enter_map(szName, nMapId)
	local h = OB_Create()
	ObjBuffer:PushObject(h, szName)
	ObjBuffer:PushObject(h, nMapId)
	RemoteExecute(ARENA_RELAY, "player_enter_map", h, "ARENA_CbEnterMap", 0, nil)   -- protocol_relay.lua:25-34
	OB_Release(h)
end
function ARENA_CbEnterMap(nParam, ResultHandle)
	local szName = ObjBuffer:PopObject(ResultHandle)
	local nMapId = ObjBuffer:PopObject(ResultHandle)
	local tbLast = ObjBuffer:PopObject(ResultHandle)
	local pDungeon = DungeonList[nMapId]
	if pDungeon then pDungeon:SetLastState(szName, tbLast) end     -- rule.lua:147 -> du 2 -> StartBattle
end
function on_player_leave_map(szName, nMapId)
	local h = OB_Create()
	ObjBuffer:PushObject(h, szName)
	ObjBuffer:PushObject(h, nMapId)
	RemoteExecute(ARENA_RELAY, "player_leave_map", h, nil, 0, nil)   -- protocol_relay.lua:36
	OB_Release(h)
end
function on_begin_battle(nMapId, tbMemberMap)        -- tbMemberMap = nil qua DynamicExecute -> tu doc
	local pDungeon = DungeonList[nMapId]
	if not pDungeon then return end
	local tbNames = {}
	for szName, pMember in pDungeon.tbMemberMap do
		if pMember then tinsert(tbNames, szName) end
	end
	local h = OB_Create()
	ObjBuffer:PushObject(h, nMapId)
	ObjBuffer:PushObject(h, tbNames[1] or "")
	ObjBuffer:PushObject(h, tbNames[2] or "")
	RemoteExecute(ARENA_RELAY, "begin_battle", h, nil, 0, nil)       -- protocol_relay.lua:43-50
	OB_Release(h)
end

-- ============ bao danh / huy / dang xuat / chet (thay emSCRIPT_PROTOCOL_SIGNUP_AREAN) ============
function apply_signup()                   -- boi canh nguoi choi (officer.lua goi qua DynamicExecuteByPlayer)
	if GetLevel() < ARENA_MIN_LEVEL then
		Talk(1, "", format("Phai dat cap %d tro len moi duoc tham gia Canh Ky Truong.", ARENA_MIN_LEVEL))
		return
	end
	local nMapId, nX, nY = GetWorldPos()
	if DungeonList[nMapId] then
		Talk(1, "", "Nguoi dang o trong san dau.")
		return
	end
	local szName = GetName()
	local nRank = tbPlayer:GetRank()
	if nRank <= 0 then nRank = ARENA_ELO_INIT end
	local h = OB_Create()
	ObjBuffer:PushObject(h, szName)
	ObjBuffer:PushObject(h, {nRank, GetFightState(), nMapId, nX, nY})
	RemoteExecute(ARENA_RELAY, "set_last_state", h, nil, 0, nil)                 -- protocol_relay.lua:14
	OB_Release(h)
	h = OB_Create()
	ObjBuffer:PushObject(h, szName)
	ObjBuffer:PushObject(h, ARENA_Group(nRank))
	RemoteExecute(ARENA_RELAY, "apply_search_opponents", h, "ARENA_CbSignup", 0, nil)   -- protocol_relay.lua:6-12
	OB_Release(h)
end
function ARENA_CbSignup(nParam, ResultHandle)
	local szName = ObjBuffer:PopObject(ResultHandle)
	local bFind = ObjBuffer:PopObject(ResultHandle)
	if bFind == -1 then ARENA_MsgByName(szName, "Nguoi da bao danh, dang cho ghep cap.")
	elseif bFind == 1 then ARENA_MsgByName(szName, "Da ghep duoc doi thu.")
	else ARENA_MsgByName(szName, "Da vao hang cho Canh Ky Truong; qua 30 giay se noi bac ghep cap.") end
end
function cancel_signup()
	local h = OB_Create()
	ObjBuffer:PushObject(h, GetName())
	RemoteExecute(ARENA_RELAY, "player_cancel", h, nil, 0, nil)        -- protocol_relay.lua:58
	OB_Release(h)
	Msg2Player("Da huy bao danh Canh Ky Truong.")
end
function ARENA_OnLogout(szName)           -- player\playerlogout.lua goi (E.3)
	local h = OB_Create()
	ObjBuffer:PushObject(h, szName)
	RemoteExecute(ARENA_RELAY, "player_logout", h, nil, 0, nil)        -- protocol_relay.lua:20
	OB_Release(h)
end
function ARENA_OnPlayerDeath()            -- death.lua, boi canh nguoi choi
	local nMapId = GetWorldPos()
	local pDungeon = DungeonList[nMapId]
	if not pDungeon then return end
	local pMember = pDungeon.tbMemberMap[GetName()]
	if pMember and pDungeon.nState == "fight" then
		pMember.bDeath = 1
		pDungeon:ProcResult()                                          -- rule.lua:163
	end
end

-- ============ khoi dong (thay autoexec.lua:209 AutoFunctions:Run) ============
function ARENA_Init()
	AutoFunctions:Run()                   -- rule.lua:316-319 -> PreApplyDungeonMap(975,0,0)
	WriteLog("[arena] ARENA_Init xong, pool 975")
end
-- GM: DynamicExecuteByPlayer(idx, ARENA_SELF, "apply_signup") cho 2 nhan vat; ARENA_Adm_Status in DungeonList
function ARENA_Adm_Status()
	for nMapId, pD in DungeonList do
		if pD then WriteLog(format("[arena] map %d state %s", nMapId, pD.nState or "?")) end
	end
end
```

#### C.4.4 DIFF `npc\officer.lua` (chép Linux rồi vá đúng các chỗ sau; ghi bằng python thay thế có `assert count`)
| Dòng gốc | Sửa |
|---|---|
| `:1` `Include("\\script\\missions\\arena\\player.lua")` | giữ; **thêm** `Include("\\script\\missions\\arena\\common_jx1.lua")` và `Include("\\script\\lib\\awardtype\\exp.lua")` (dự án: `awardtemplet.lua:65` chỉ tự kéo `item_jx1.lua`; `nExp` đăng ký ở `lib\awardtype\exp.lua:4-8` ⇒ thiếu là `tbAwardTemplet:Give({nExp=…})` `officer.lua:41-42` im lặng không trao) |
| `:80-86` bảng `tbOpt` của `main()` | **chèn 2 mục đầu**: `{"Bao danh Canh Ky Truong (cap >= 90)", signup_jx1, {}}`, `{"Huy bao danh", cancel_jx1, {}}`; thêm 2 hàm: `function signup_jx1() DynamicExecuteByPlayer(PlayerIndex, "\\script\\missions\\arena\\protocol.lua", "apply_signup") end` / `cancel_signup` tương tự |
| `:33,37,40` | không sửa — đã có nhờ `common_jx1.lua` |
| `:70` `Sale(175, 16)` | không sửa (shop không tồn tại cả hai cây — giữ hiện trạng; `LuaSale` `ScriptFuns.cpp:2610` mở shop 174 rỗng, vô hại) |

### C.5 Quyết định thiết kế tự chọn (Linux mất) — ghi rõ để chủ game duyệt
| Mục | Đề xuất | Căn cứ |
|---|---|---|
| Điểm khởi đầu | 1000 | `common.lua:23` bậc thấp nhất "Nhập Môn Đệ Tử" < 1200; 1000 ⇒ người mới ở bậc thấp nhất |
| Công thức | ELO chuẩn, K = 32, E = 1/(1+10^((Rđ−Rt)/400)), thắng 1 / hoà 0,5 / thua 0, làm tròn | không có mã Linux; 9 mốc 1200→2600 cách nhau 200 = quy mô ELO |
| 30 bậc ghép cặp | bậc = ⌊rank/100⌋ kẹp [1,30]; cùng bậc ghép ngay, quá 30 s nới bậc ±1, ±2… (`player.lua:176-196` đã có) | `player.lua:357` `Init(30)`; 1000→bậc 10, 2600→26 |
| Thưởng tuần | giữ nguyên Linux: đủ 20 trận/tuần nhận 100.000.000 exp (`officer.lua:41-42`), 1 lần/tuần qua task 3178 | danh hiệu bị Linux tắt `officer.lua:44-55` |
| Xếp hạng | theo **task value** 3172 của từng người (đúng Linux). Không dùng League (Linux không dùng). Bảng xếp hạng toàn server: **không có trên Linux** — không làm | |

### C.6 Tuỳ chọn (không chặn): hook cấm vật phẩm trong sân
Linux: đường dùng vật phẩm `0x082047DA-0x0820480D` gọi `CallScript("\script\item\forbiditem.lua" [0x0826B6DD], "Check_ItemUsable" [0x0826B6CC], "uuuu", nSubWorldID, Genre, Detail, Particular)` ⇒ 0 là cấm. JX1 **không có hook** ⇒ `rule.lua:29-33` (CALLNPC, TRANSFER) và 76 thuốc `vng_feature/forbiditem/vngforbidspecialitem.lua` **không có hiệu lực**. Muốn có: chèn ở `KItemList::UseItem` (`KItemList.cpp:1866`, trước `switch(Item[nIdx].GetGenre())`), gọi state standalone của `forbiditem.lua`, đồng thời thêm dòng map 975 vào `settings\map_type.txt` (state đó chỉ biết bảng settings, không thấy `set_MapType` của `rule.lua:37`). Và phải port + remap theo tên `vngforbidspecialitem.lua` (id Linux).

---

## D. BẢNG REMAP (`arena_remap.py`)

### D.1 Item
Cây arena **không dùng vật phẩm nào** (`item_remap.py missions/arena`: 0 mẫu `(6,1,N)`; chỉ có exp ở `officer.lua:41`). **Không phải làm thêm item, không ảnh.**

### D.2 NPC template (`settings\npcs.txt`, id = dòng−2 cả hai cây)
| Linux id | Tên Linux | Dự án | Dùng ở |
|---|---|---|---|
| 108 | Bạch Bào Tẩu | **TRÙNG ID** (108 Bạch Bào Tẩu) | NPC Quan Viên (`autoexec_npc.lua:221`, tên hiển thị đặt khi AddNpc) |
| 625 | Rương chứa đồ | **TRÙNG ID** | `rule.lua:25` |
| 203 | Ông chủ dược điếm | dự án 203 = "Chủ dược điếm" (khác 1 chữ, cùng khe) → **dùng 203** | `rule.lua:26` (tên hiển thị do script đặt "Chủ dược điếm") |

### D.3 Skill-state id (`vnforbidstate.lua:2-94`, 92 id → `RemoveSkillState`)
**92/92 TRÙNG ID** với `settings\skills.txt` dự án (cột `SkillId`/`SkillName` cùng bố cục). 1 id (521) tên Linux "kháng tấn công khí mệnh (kỹ năng)" ≠ tên dự án "Giảm Băng hoàn (kỹ năng)" nhưng nhãn trong script là "Giảm Băng hoàn" = khớp dự án ⇒ giữ 521. 4 nhãn lệch nhau vô hại (ví dụ 462 Linux "Tăng Sinh lực (Tống Kim)" vs nhãn "Hoàn Hồn Đơn"). Kết luận: **không remap**.

### D.4 Shop / goldequip / task
- `Sale(53)` (`yaodian.lua:2`) → `BuySell.OpenSale(52)` = dòng 54 `buysell.txt`: Linux `1,2,3,…` ; dự án `ShopName rỗng,1,2,3,…` ⇒ cùng shop thuốc. Giữ.
- `Sale(175,16)`: không tồn tại cả hai (Linux 166 dòng, dự án 102). Giữ nguyên = chết như Linux.
- goldequip: không dùng.
- Task id 3172-3177 (Linux) + **3178 (mới)**: quét toàn cây dự án `GetTask/SetTask` và `TASK_* =`: **không có id nào trong 3100-3300** ⇒ không va chạm; `MAX_TASK` 4200 (`KPlayerTask.h:18`).
- Danh hiệu 237-245 (`officer.lua:8-19`): khối dùng đã bị Linux chú thích ⇒ không tra.

---

## E. LỊCH & CƠ CHẾ KÍCH HOẠT

Cảnh Kỹ Trường **không có lịch** (mở 24/24, không có TaskShedule nào trong `relaysetting\task\` nhắc `arena` — `find` ra 0). Kích hoạt:

| Khi | Gì | Ở đâu |
|---|---|---|
| Boot GS | `LoadAllScript("\script")` (`KSortScript.cpp:56`) nạp + chạy top-level mọi tệp: `protocol_relay.lua` state = **relay sống** (`player.lua:357-360` Init(30) + `AddTimer(30,"tbPlayerManage:OnTime",0)`), `protocol.lua` state = GS sống (Include rule.lua → `Dungeon:new_type("arena")`, `AutoFunctions:Add(init_rule)`) | tự động |
| Boot, sau WLLS/TONG | **`startgame.lua:101`** thêm ngay dưới: `DynamicExecute("\\script\\missions\\arena\\protocol.lua", "ARENA_Init")` → `AutoFunctions:Run()` → `PreApplyDungeonMap(975,0,0)` | E.1 |
| Boot | NPC Quan Viên: `startgame\thanh\laman.lua` `addnpclaman()` (`:2`) thêm 1 dòng theo mẫu `:30`: `AddNpcEx1({108},1,nil,176,1451*32,3233*32,"","\\script\\missions\\arena\\npc\\officer.lua","Quan Viên Cảnh Kỹ Trường",6)` (toạ độ `autoexec_npc.lua:221`; `AddNpcEx1` ở `lib\lib_map.lua:91`) | E.2 |
| Đăng xuất | `player\playerlogout.lua:8` (`main()`) thêm: `DynamicExecute("\\script\\missions\\arena\\protocol.lua", "ARENA_OnLogout", GetName())` — cho người đang **xếp hàng** (người trong sân đã được `OnLeaveWorld` lo, `KPlayerSet.cpp:374`) | E.3 |
| Mỗi 30 frame | relay tick `tbPlayerManage:OnTime` (`player.lua:328`) — Linux relay dùng **giây**, GS ta dùng **frame** ⇒ ghép cặp quét 1,7 s/lần thay vì 30 s (các `nTimeOut` so với `GetSysCurrentTime()` giây vẫn đúng) — lệch có chủ đích, có lợi | |

---

## F. LỐI VÀO

- **NPC "Quan Viên Cảnh Kỹ Trường"** (tpl 108) — **Lâm An (map 176) ô (1451, 3233)** (`autoexec_npc.lua:221`). Linux xoá NPC này lúc boot (`autoexec.lua:214,279`) — ta **không** chép `ClearNPCNewVersion`.
- Menu (`officer.lua:77-88` + 2 mục DIFF): *Báo danh Cảnh Kỹ Trường (cấp ≥ 90)* · *Huỷ báo danh* · Nhận thưởng chiến đội tuần này (`want_get_title`, đủ 20 trận → 100.000.000 exp) · Mở cửa hàng Vinh Dự (chết như Linux) · Giới thiệu · Huỷ bỏ.
- Trong sân: NPC **Rương chứa đồ** (625, ô 1566,3214 → `battles\openbox.lua`) và **Chủ dược điếm** (203, ô 1578,3204 → shop thuốc 53) do `rule.lua:53-56` tự `AddNpcEx` mỗi trận (engine dự án bỏ NPC map-data, `KRegion.cpp:474-484`; `ClearMapNpcWithName` trước nên không nhân đôi).
- Chỗ chuẩn bị: 32 điểm `readypos.txt`; chỗ đánh: 15 điểm `battlepos.txt` (`rule.lua:236-245`).

---

## G. XUNG ĐỘT VỚI CÁI ĐANG CHẠY + CÁCH GIẢI

| # | Xung đột | Bằng chứng | Giải |
|---|---|---|---|
| G.1 | **`SetPunish` của JX1 NGƯỢC Linux** — phát hiện khi disasm cho arena, ảnh hưởng **Lôi Đài Bang Hội đã port** và **3 Hoạt động Phường** | Linux `0x0810F470`: `SetPunish(0)` ⇒ Npc+0x1818 = **3** (không phạt, `0x0810F52B`), `SetPunish(≠0)` ⇒ 0. JX1 `LuaSetDeathPunish` (`ScriptFuns.cpp:12797-12812`): 0 ⇒ state 0 (**phạt**), ≠0 ⇒ `enumDEATH_MODE_PKBATTLE_PUNISH`=3 (không phạt, `KNpc.cpp:8091`). Script gốc JX1 dùng nghĩa JX1 (`missions\mission02.lua:48`, `player\dichuyenmap.lua:63`). Script gốc JX2 dùng nghĩa Linux: `citywar_arena\head.lua:100 SetPunish(0)` (vào sân = **bị phạt khi chết**), `missions\tong\collectgoods\head.lua:230/266/304`, `tong_disciple\head.lua:193/226/262`, `tong_springfestival\head.lua:224/261/298` | **Arena không bị** (C.2 bẫy 6). Cho các cây JX2 khác: ĐỀ NGHỊ thêm hàm C `SetPunishJX2` nghĩa Linux và thay tên trong 10 điểm gọi JX2 (hoặc sửa 3 tệp head.lua) — việc riêng, ghi nợ |
| G.2 | `death.lua` của `citywar_arena` khai `OnDeath(Launcher)` nhưng JX1 gọi **`OnPlayerDeath(nPlayerIdx, nKiller)`** (`KNpc.cpp:1542`) | `citywar_arena\death.lua:3` | Arena dùng `death.lua` mới đúng tên (C.4.2). Ghi nợ cho Lôi Đài Bang Hội |
| G.3 | Map 975: **không tính năng nào của dự án dùng** | grep `\b975\b` script dự án: chỉ `codenew.lua:3897` (bảng giftcode) + `misc\taskmanager.lua:84` (dải task id, không phải map) | không xung đột |
| G.4 | `SetPKFlag` JX1 = `SetFightState` (`KJx2WarInfra.cpp:227-230`) ⇒ `rule.lua:110 SetPKFlag(1)` bật chiến đấu ngay khi vào (Linux `0x0810F610` chỉ đặt cờ PK `m_cPK+…` qua `0x080C3740`, chiến đấu vẫn 0 theo `rule.lua:105`) | | `OnNewWorld` ở `protocol.lua` gọi `SetFightState(0)` **sau** `OnEnterMap` (C.4.3) — không sửa `rule.lua`. Người chơi vẫn tự bật chiến đấu tay được vì `ForbidChangePK` là stub (`KJx2WarInfra.cpp:234`) — chấp nhận (đối thủ ở điểm chuẩn bị ngẫu nhiên, 60 s) |
| G.5 | Hồi sinh tại chỗ (`KPlayer::Revive LOCAL_REVIVE_TYPE`, `KPlayer.cpp:6811-6813`) giữ người chết trong sân ⇒ Linux chờ người chết **rời map** mới kết trận (`rule.lua:137-139`) | | `death.lua` + `ARENA_OnPlayerDeath` kết trận ngay lúc chết (C.4.3) — lệch có chủ đích, có lợi |
| G.6 | `heart_head.lua` dự án thiếu `add_forbit_templatemap` ⇒ `rule.lua:321` ghi 1 dòng log mỗi state nạp rule.lua (2 state) | `item\heart_head.lua` dự án 130 dòng vs Linux 289 | bỏ qua, hoặc thêm 2 hàm no-op `add_forbit_templatemap/del_forbit_templatemap` vào bản dự án |
| G.7 | `forbiditem.lua` in "Khởi động tệp loại hình vật phẩm thất bại!" nếu thiếu `item_type.txt`/`item_detail.txt` | `forbiditem.lua:50,71` | chép 2 tệp (B.1) |
| G.8 | 5 state standalone (`player.lua`, `cmd.lua`, `rule.lua`, `officer.lua`, + `protocol.lua` qua Include) mỗi state 1 timer rỗng 30 frame | `player.lua:360` | chấp nhận (giữ byte-identical); CPU không đáng kể |
| G.9 | Bản tự viết trùng chức năng trong dự án: **không có** (grep "Cảnh Kỹ"/"canhky": 0) | | không phải gỡ gì |

---

## H. ĐỔI CẤP 90 / BỎ TRÙNG SINH — đích danh

| # | Tệp : dòng | Chuỗi cũ | Chuỗi mới |
|---|---|---|---|
| 1 | `missions\arena\*` (9 tệp Linux) | **không có** `GetLevel` nào (đã quét) | không sửa tệp Linux nào |
| 2 | `missions\arena\protocol.lua` (MỚI) `apply_signup` | — | `if GetLevel() < ARENA_MIN_LEVEL then Talk(...) return end` với `ARENA_MIN_LEVEL = 90` (C.4.3) |
| 3 | `missions\arena\npc\officer.lua:80-86` (DIFF) | — | mục menu ghi rõ "(cấp >= 90)" |
| 4 | Trùng sinh | **không có** điều kiện trùng sinh nào trong cây (`ST_IsTransLife`/`TransLife`: 0 kết quả trong `missions/arena`) | — |

---

## I. RỦI RO + CÁCH TEST SAU RESTART

**Rủi ro**
1. `AddTimer` thiếu/không đăng ký ⇒ 4 tệp không nạp (C.2-1) — kiểm `ScriptError.log` + `g_DebugLog` sau boot: không được có `arena` trong danh sách lỗi nạp.
2. Đệ quy `RemoteExecute` đồng bộ (C.2-2): đã xếp hàng `player_enter_map`; nếu vẫn thấy lỗi "attempt to index" trong `protocol_relay` lúc ghép ⇒ xếp hàng thêm `reg_map` (gọi qua `AddTimer(1,…)`).
3. `DynamicExecute` cắt chuỗi 64 ký tự — tên nhân vật ≤ 32 byte, an toàn.
4. `NewWorld` người đang chết (GoToLastPos khi bị xử thua do chết): `KNpc::ChangeWorld` với `m_Doing = do_death` — **CHƯA RÕ** có dịch được không; nếu không, người chết ở lại 975 → hồi sinh (REMOTE) về thành → `OnLeaveWorld` → `DungeonList[975]` đã nil → chỉ `OnLeaveWorldDefault` — vẫn ổn.
5. Hai người cùng phe gốc (m_Camp giống) vẫn đánh được nhờ `SetCurrentCamp(1/2)` + `FightMode` — cùng cơ chế `citywar_arena` (chưa test thật trên dự án! Lôi đài bang hội mới port 21/08 "chưa test chạy thật").
6. Người chơi đứng sẵn trong 975 lúc restart: `OnNewWorld` lần sau đá về Thành Đô (`mapscript.lua:10`) — đúng Linux.
7. Không có giới hạn số trận/ngày, không thu phí — đúng Linux (`DIEUKIEN_THAMGIA_9_HOATDONG.md` mục 4).

**Test (GM, 2 tài khoản cấp ≥ 90)**
1. Boot: log có `[arena] ARENA_Init xong`; `ScriptError.log` không thêm dòng `arena|basemission|forbiditem`.
2. Lâm An (1451,3233) có NPC "Quan Viên Cảnh Kỹ Trường"; menu 6 mục; nhân vật cấp < 90 bấm Báo danh → bị chặn.
3. A báo danh → "Đã vào hàng chờ"; B báo danh cùng bậc → ≤ 2 s cả hai nhận "đã tìm được đối thủ", ≤ 4 s bị dịch vào 975 tại readypos; chat hệ thống "Sau 60 giây chính thức bắt đầu" (`rule.lua:251`).
4. 55 s: "Thời gian chuẩn bị còn lại 5…0 giây"; dịch tới battlepos; đếm 5…0; "Phân tranh"; chiến đấu bật; đánh nhau được (khác phe tạm).
5. Một bên chết ⇒ ngay lập tức "X đánh bại Y" (`rule.lua:187`), cả hai về chỗ cũ, task 3172 đổi (xem bằng `GetTask(3172)` qua lệnh GM), 3173/3175 +1, 3177 +1.
6. Trận 2 để hết 5 phút ⇒ so sát thương hứng chịu (`rule.lua:193-205`).
7. Một bên đăng xuất giữa trận ⇒ bên kia thắng (`OnLeaveWorld` từ `PrepareRemove`).
8. A báo danh rồi Huỷ ⇒ không bị ghép; A đăng xuất khi đang chờ ⇒ B sau đó vẫn ghép được với C (relay `Release`, `player.lua:336`).
9. 3-4 người báo danh cùng lúc: cặp 2 nhận "sân đang bận" cho tới khi cặp 1 xong (pool 1 sân).
10. Đủ 20 trận trong tuần ⇒ NPC "Nhận thưởng" +100.000.000 exp, lần 2 báo "Mỗi tuần chỉ nhận một lần" (`officer.lua:34`).

---

## PHỤ LỤC — CHƯA RÕ (không suy diễn)
- Ý nghĩa tham số 2, 3 của `PreApplyDungeonMap` (Linux luôn (0,0) cho arena).
- Công thức ELO / cách chia 30 bậc / điểm khởi đầu của bản gốc (mã GS mất).
- Bản client có UI báo danh ở phiên bản nào (bản VNG dump này: không).
- Đơn vị `AddTimer` của relay (giây?) — ta chạy trong GS theo frame, chấp nhận lệch.
- Client có nhận map id ≥ 1000 không (chỉ cần khi mở nhiều sân).
- `KNpc::ChangeWorld` khi nhân vật đang chết (I.4).
