# BÀN GIAO — HỆ LIÊN ĐẤU (leaguematch / WLLS)

Phiên 20/08/2026 · **Trạng thái: MỚI PHÂN TÍCH XONG, CHƯA SỬA MỘT DÒNG MÃ NÀO.**
Mục đích tệp này: phiên sau đọc vào là **bắt tay làm được ngay**, không phải điều tra lại từ đầu.

---

## 0. ĐỌC GÌ TRƯỚC — 60 GIÂY

| Thứ tự | Tệp | Để làm gì |
|---|---|---|
| 1 | **tệp này** | biết đang ở đâu, làm gì tiếp, tránh bẫy nào |
| 2 | `PHANTICH_LIENDAU_LINUX_VS_DUAN.md` | phân tích đầy đủ + sơ đồ hoạt động 2 bản (47 KB) |
| 3 | `DIEUTRA_LEAGUE_SPEC.md` | đặc tả 28 hàm `LG_*`/`LGM_*` (viết 14/08, vẫn đúng) |
| 4 | `ReverseTools/README.md` | công cụ dịch ngược + lý do bản cũ bỏ sót 216 tên |

**Một câu tóm tắt:** "Liên Đấu" bản gốc tên là **`leaguematch`, viết tắt `WLLS`** (武林联赛), là hệ **2 tầng** (relay + GameServer), 45 tệp / 5.902 dòng Lua. Dự án đang chạy một hệ **tự viết 1v1** khác hẳn (10 tệp / 1.635 dòng). Muốn port: **thiếu 37 hàm engine**, nhưng hạ tầng đắt nhất (League / Ladder / Mission / bản đồ) **đã có sẵn**.

---

## 1. ĐƯỜNG DẪN CHUẨN (nhớ kỹ, rất dễ mở nhầm)

| Vai trò | Đường dẫn |
|---|---|
| **Mã nguồn C++** (sửa ở đây) | `D:\GAMEDEVNEW\Sources` |
| **Máy chủ chạy thật** (script + settings + Maps + Pak + binary) | `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server` |
| Bản tham chiếu — GameServer Linux | `D:\ServerLinux\server1` (ELF `jx_linux_y`) |
| Bản tham chiếu — Relay | `D:\ServerLinux\gateway\s3relay` (ELF `s3relay_y`) |
| Bản tham chiếu — client đã bung | `D:\ServerLinux\Patch` (`game_y_unpacked.bin`) |

🔴 **BẪY:** `E:\SourceTuanLe\dulieugame\server` là **cây cũ, KHÔNG chuẩn**. Cùng bố cục, cùng số tệp, chỉ lệch vài byte ⇒ `find` đệ quy trả về nó trước và nhìn qua thì "có vẻ đúng".
Ví dụ thật: `timer_liendau.lua` cây chuẩn **450 dòng**, cây cũ **364 dòng** — cây cũ **thiếu hẳn 3 hàm** `QuyDinhThangThuaTheoDame` / `ThangTheoDame` / `ThuaTheoDame`, tức thiếu luôn luật thắng-thua.
**Cách kiểm nhanh:** cây chạy thật phải thấy `GameServer.exe` + `CoreServer.dll` + `Pak\` + `scriptjx2\`.

---

## 2. CÔNG CỤ — CHẠY ĐƯỢC NGAY

Máy này **không có** `strings` / `objdump` / `readelf` / `nm`. Chỉ có python:
`C:\Users\nguye\AppData\Local\Programs\Python\Python312\python`

### 2.1 Dịch ngược ELF Linux

```bash
cd D:/GAMEDEVNEW/ReverseTools
PYTHONIOENCODING=utf-8 python re_elf_luamap_full.py D:/ServerLinux/server1/jx_linux_y        > jx_linux_y.luamap.full.txt
PYTHONIOENCODING=utf-8 python re_elf_luamap_full.py D:/ServerLinux/gateway/s3relay/s3relay_y > s3relay_y.luamap.full.txt
```

Kết quả đã commit sẵn trong repo: **1.560 tên** (GS) và **473 tên** (relay).

🔴 **Dùng `re_elf_luamap_full.py`, KHÔNG dùng `re_elf_luamap.py` cũ** — bản cũ **bỏ sót 216 tên** vì GCC gộp hậu tố chuỗi (`SHF_MERGE|SHF_STRINGS`): `Msg2Player` không tồn tại riêng, nó là `GMMsg2Player + 2`; `GetTask` nằm trong `NW_GetTask`; `Talk` trong `ForbitTalk`. Chi tiết trong `ReverseTools/README.md`.

### 2.2 Đối chiếu chênh lệch API (bản Linux gọi gì ↔ dự án có gì)

```bash
cd D:/GAMEDEVNEW/ReverseTools
PYTHONIOENCODING=utf-8 python re_lua_api_gap.py > liendau_api_gap.txt
```

Công cụ này tự: đọc bảng `{ "Tên", LuaHàm }` trong `ScriptFuns.cpp` (+ mọi `.cpp` khác trong `Core\Src`) → quét cây `leaguematch` lấy mọi tên được gọi → loại hàm Lua tự định nghĩa → **in ra danh sách hàm engine còn thiếu**.
Muốn dùng cho tính năng khác: sửa biến `ROOTS` ở đầu tệp.

### 2.3 Đọc tệp script (BẮT BUỘC đúng cách)

Script bản Linux là **GBK (chữ Hán) trộn TCVN3 (tiếng Việt)**; script dự án là **ANSI/TCVN3**.
🔴 **Tuyệt đối không dùng Read/Edit/Write thẳng lên tệp script game — sẽ hỏng byte.**

```bash
# đọc chữ Hán bản Linux
PYTHONIOENCODING=utf-8 python -c "print(open(r'D:\ServerLinux\server1\script\leaguematch\head.lua','rb').read().decode('gbk','replace'))"

# đọc tệp dự án (giữ nguyên cấu trúc ASCII)
PYTHONIOENCODING=utf-8 python -c "print(open(p,'rb').read().decode('latin-1'))"
```

Khi **sửa** script game: dùng skill `swordonline-dev` (có `safe_edit.py`), pattern phải CRLF.

### 2.4 Đọc chỉ mục pak (kiểm bản đồ/tài nguyên có thật không)

Hàm băm: `KPakList::FileNameToId` (`Engine\Src\KPakList.cpp:72`), chuẩn hoá đường dẫn `g_GetPackPath` (`Engine\Src\KFilePath.cpp:259-275`).
🔴 **Bẫy:** `g_GetPackPath` **cắt dấu `\` đầu tiên** và **hạ chữ thường TOÀN BỘ** trước khi băm; `char` dùng để băm là **signed**. Băm nguyên chuỗi hoa sẽ ra 0 kết quả thật và chỉ còn nhiễu va chạm hash ⇒ dễ kết luận nhầm "thiếu bản đồ".

---

## 3. SỰ THẬT ĐÃ CHỐT (đã kiểm tới byte — không cần kiểm lại)

| # | Sự thật | Chứng cứ |
|---|---|---|
| 1 | "Liên Đấu" bản Linux = `leaguematch` = `WLLS` | `leaguematch\head.lua:43` có chuỗi `"Vũ lâm liên đấu"`; cụm tệp `wlls_*.lua` |
| 2 | 7 tệp `server1\script\leaguematch\*.lua` **chạy trên RELAY, không phải GS** | md5 khớp 7/7 với `gateway\s3relay\script\leaguematch\`; điểm nạp thật `s3relay\relaysetting\task\leaguematch.lua:1` |
| 3 | Quy mô: Linux **45 tệp / 5.902 dòng** (38 GS = 4.872 + 7 relay = 1.030); dự án **10 tệp / 1.635 dòng** | `wc -l` trực tiếp |
| 4 | **158** hàm engine WLLS dùng · dự án **đã có 121** · **thiếu 37** | `re_lua_api_gap.py`; `ScriptFuns.cpp` đăng ký 923 tên qua 3 bảng |
| 5 | **28/28 hàm `LG_*`/`LGM_*` ĐÃ CÓ** và **có persist thật** | `KJx2League.cpp` 1.035 dòng; `sLeagueSave():203` ghi `settings\jx2league.txt` (fopen + MoveFileEx REPLACE + dự phòng `KGameKV`) |
| 6 | 3/3 hàm `Ladder_*` đã có, persist `settings\jx2ladder.txt` | `KJx2SharedStore.cpp:262-461` |
| 7 | Hệ Mission đủ dùng: có camp (`m_ucPlayerGroup`), 9 timer/mission, 24 hàm đăng ký | `KMission.h:27-34,121,138`; `ScriptFuns.cpp:13370-13405` |
| 8 | **Bản đồ trùng ID 100%** và **đã có dữ liệu thật** trong `Pak\maps.pak` | so `MapList.ini` hai bên (trùng cả đường dẫn lẫn tên Việt); giải mã chỉ mục pak 87.245 mục |
| 9 | `MapList.ini` dự án **đã trỏ 46 map** vào `\script\missions\leaguematch\...` — thư mục chưa tồn tại ⇒ **đặt tệp vào là 46 mục tự sống lại**, không phải sửa ini | `settings\MapList.ini` |
| 10 | **Bỏ tầng bất đồng bộ AN TOÀN** | 10/10 call site `LG_ApplyDoScript` đều kết thúc `, "", ""` = không dùng callback |
| 11 | **0 thay đổi giao thức** client↔server | bản gốc cũng không có gói tin riêng: chỉ 2 kênh chung (`SyncTaskValue` và `Ladder_*`) |
| 12 | Bản dự án là hệ tự viết 2021, nhưng **`WLLS_FORBID_ITEM` trùng 78/78 mục** với bản gốc | `lib_liendau.lua:361-442` vs `missions\leaguematch\head.lua:147-230`, trùng cả chú thích `--tinhpn 20100720` |
| 13 | Bản tham chiếu **KHÔNG có cửa sổ UI riêng** cho liên đấu | quét 370 lớp RTTI trong `game_y_unpacked.bin`: không có `KUiLeague*`/`KUiWlls*` |

---

## 4. 🔴 BỐN THỨ CHẶN CỨNG — LÀM TRƯỚC DÒNG CODE ĐẦU TIÊN

### 4.1 `LuaRunMission` sẽ SẬP GameServer

`Sources\Core\Src\ScriptFuns.cpp:10394-10400`

```c
sprintf(szScript, "\\script\\misions\\mision%02d.lua", nMissionId);   // thiếu chữ 's' ở CẢ HAI TỪ
...
pScript->m_LuaState                                                    // deref không kiểm NULL
```

`g_GetScript` trả NULL được (`KSortScript.cpp:70-84`). Bản Linux gọi `RunMission(25)` ở `combat\mission.lua:19` ⇒ **sập ngay trận đầu tiên**.
Đối chiếu: `OpenMission` (`:10365`) và `CloseMission` (`:10463`) làm đúng — chúng dùng `g_MissionTabFile.GetString`.
**Việc phải làm:** sửa đường dẫn dùng `g_MissionTabFile` như 2 hàm kia, và kiểm NULL.

### 4.2 Lịch mùa giải đã chết từ 28/12/2016

`D:\ServerLinux\server1\script\leaguematch\timetable.lua:32-137` — bảng `WLLS_SEASON_TB` kết thúc ở mùa `[123] = 161228`, sau đó là 2 mùa giả `999998`/`999999`.
⇒ `wlls_calc_phase()` (`task.lua:22-50`) **luôn trả pha 1 = nghỉ vĩnh viễn**.
Port nguyên xi = **tính năng không bao giờ chạy**, và sẽ mất nhiều ngày điều tra "sao không thấy gì".
**Việc phải làm:** viết lại bảng mùa giải (~150 dòng) hoặc đổi sang công thức tính mùa theo tháng.

### 4.3 Bỏ async ⇒ đệ quy trong cùng Lua state

```
npc\signup.lua:98        LG_ApplyDoScript(...)
  └─▶ KJx2League.cpp:844   thực thi CỤC BỘ, ĐỒNG BỘ (bản gốc đi qua relay)
       └─▶ joinmatch.lua:227  GlobalExecute("dw wlls_player_join(...)")
            └─▶ wlls_gmscript.lua:147  NewWorld()   ← ĐỔI MAP GIỮA LÚC HỘP THOẠI NPC ĐANG MỞ
```

Bản gốc có độ trễ mạng chen vào nên `NewWorld` rơi vào tick sau.
**Việc phải làm:** chèn **hàng đợi hoãn 1 tick** (~120 dòng C++), nếu không sẽ treo hộp thoại hoặc lệch `PlayerIndex`.

### 4.4 Hai hệ sẽ giành nhau tài nguyên — CẦN NGƯỜI QUYẾT

| Tài nguyên | Bản tự viết đang chiếm | Bản gốc cần |
|---|---|---|
| Bản đồ | 396 / 397 / 399 | 396-415 + 540-579 + 991-993 |
| Mission | 11 | 24 / 25 / 26 |
| Timer | 11 | 50 / 51 |
| Task bền | 37 (`T_LIENDAU`) | 1715-1735, 2500, 2501, 4125 |

🔴 Task 37 = `TASKVALUE_STATTASK_HONOR` = **đơn vị tiền của shop 93** ⇒ chạy song song sẽ **làm lệch điểm vinh dự**.
**Hai lựa chọn:** (a) thay thế hẳn bản tự viết; (b) chạy song song trên bộ map khác (540-579 còn trống) + tách task.
**Chưa quyết — phải hỏi chủ game.**

---

## 5. CHECKLIST THI CÔNG — ĐỢT A (5-8 ngày công)

Phạm vi: 1 thể loại (Song đấu 2 người), 1 hạng cấp (≥120), giữ mùa giải + xếp hạng + thưởng cuối mùa.
Bỏ: 6 thể loại còn lại, bảng tìm đồng đội, 7 vật phẩm, UI client.

- [ ] **A0.** Chốt quyết định mục 4.4 (thay thế hay song song) — hỏi chủ game.
- [ ] **A1.** Sửa `LuaRunMission` (mục 4.1). ⚠️ Đây là sập chắc chắn, làm trước tiên.
- [ ] **A2.** Sửa `LuaStartMissionTimer` chống trùng slot (`ScriptFuns.cpp:10505-10533`; chỉ 9 slot dùng được, index 0 bị bỏ).
- [ ] **A3.** Sửa `GlobalExecute` bóc tiền tố `dw ` / `dwf <đường dẫn> ` (`KTongJX2.cpp:4023-4034`).
- [ ] **A4.** Thêm **12 hàm** Lua-API (danh sách mục 6, nhóm "Bắt buộc, rẻ") + đăng ký trong `ScriptFuns.cpp`.
- [ ] **A5.** Thêm hàng đợi hoãn 1 tick (mục 4.3).
- [ ] **A6.** Chép `D:\ServerLinux\server1\settings\maps\championship\champion_gmpos.txt` (1.127 B, 101 dòng) sang cây chạy. **Đây là tệp bảng DUY NHẤT cả hệ leaguematch nạp** — thiếu nó thì `TabFile_GetCell` trả rỗng ⇒ `NewWorld(map, nil, nil)`.
- [ ] **A7.** Port ~14 tệp Lua (~1.850 dòng), đặt vào `script\missions\leaguematch\`. Đổi tên thư mục cho 7 tệp relay (ví dụ `...\leaguematch\relay\`) để **tránh va chạm tên toàn cục** — hai tệp `head.lua` cùng định nghĩa `FALSE` và `_M`.
- [ ] **A8.** Viết lại `WLLS_SEASON_TB` (mục 4.2).
- [ ] **A9.** Nối `settings\task\missions.txt` (đang 12 dòng, cần tới 26) và `settings\TimerTask.txt` (đang 18 dòng, cần tới 51) — **hoặc** đánh số lại Mission/Timer về khe trống cho bảng ngắn.
- [ ] **A10.** Thêm 2 cờ `PKEX`, `LEAGUE` vào dòng `LEAGUEMATCH` trong `settings\map_type.txt` (dự án có dòng đó nhưng thiếu 2 cờ).
- [ ] **A11.** Ánh xạ lại ID vật phẩm/danh hiệu (mục 7, bẫy #4).

**Nghiệm thu đợt A:** 1 trận Song đấu chạy trọn vẹn từ lập đội → báo danh → ghép cặp → đánh → cộng điểm → xếp hạng lên → thưởng cuối mùa trả đúng.

## 6. ĐỢT B (+7-10 ngày công)

- [ ] Mở 6 thể loại còn lại (8 tệp `macthtype/`, 1.113 dòng).
- [ ] Mở hạng Kiệt Xuất (cấp 80-119, map 396-415).
- [ ] **4 hàm `ST_*DamageCounter`** — thứ DUY NHẤT phải dựng hạ tầng C++ từ số 0: thêm `m_nDamageCounter` + `m_bDamageCounterOn` vào `KPlayer/KNpc`, hook vào đường tính sát thương trong `KNpc`. ⚠️ hook sai chỗ là tụt hiệu năng.
- [ ] 7 tệp `item/` (171 dòng) + ~13 dòng trỏ script trong `magicscript.txt`.
  ⚠️ `honour.lua` bản gốc chỉ khai 4/7 vật phẩm (thiếu 1294, 1334, 1335) — người chơi sẽ mất đồ nếu port nguyên.
- [ ] 14 hàm nhóm "Chiều sâu" (mục 7).

## 7. BỎ HẲN Ở CẢ HAI ĐỢT

| Bỏ | Vì sao |
|---|---|
| `npc\helper.lua` (bảng tìm đồng đội, 467 dòng) | kéo theo `GetStringTask`/`SetStringTask` ⇒ **đổi định dạng lưu nhân vật** ⇒ **xung đột trực tiếp với đợt di trú `roledb` sang MySQL đang làm** |
| `LoadLadder`, `SyncAllLadder` | thừa hoàn toàn với 1 GameServer |
| `TaskName`/`TaskTime`/`TaskInterval`/`TaskCountLimit` | task centre của relay; thay bằng `StartGlbMSTimer(26, id, 15*60*18)` đã có |
| `GetGlbMSRestTime` | **không tồn tại ở cả bản Linux**; chính script đã bọc `if GetGlbMSRestTime then` |
| Cửa sổ xếp hạng phía client | `Game.exe` có 0 hit cho `KUiTreeRank`/`battlerank`/`ExportLadder`; dùng menu NPC với `Ladder_GetLadderInfo` là đủ |

---

## 8. DANH SÁCH 37 HÀM THIẾU

### Nhóm 1 — bắt buộc, rẻ (12 hàm, làm ở đợt A)

| Hàm | Ghi chú |
|---|---|
| `GetGblInt`, `SetGblInt` | ánh xạ thẳng vào `LuaGetGlbValue`/`LuaSetGlbValue` (`KJx2SharedStore.cpp:463-492`). Dải RLGLB 121..145 **không va chạm** dải `GLB_WLLS` 820..826 |
| `Random` | 🔴 **BẪY:** dự án chỉ có `RANDOM` **VIẾT HOA** (`ScriptFuns.cpp:13835`). Bảng băm tên hàm Lua **phân biệt hoa-thường** — `Select-String`/`-match` mặc định KHÔNG phân biệt nên sẽ báo nhầm "đã có". Luôn dùng `-CaseSensitive` |
| `CloseGlbMission` | có `Open`/`Start`/`Stop` rồi, khuyết đúng cái này. 2 call site |
| `SyncTaskValueMore` | dự án chỉ có `SyncTaskValue` 1 id; bản gốc đồng bộ dải 1720..1732 + 2500..2501 |
| `AskClientForString` | ⚡ **KHÔNG cần đổi giao thức** — chỉ là lớp bọc ~25 dòng quanh `OpenGetString` + `GetStringFromUI` (`ScriptFuns.cpp:13322-13325`); đường gói tin `S2C_INPUT_BOX` đã chạy sẵn |
| `Number2Int` | 1 call site, có thể thay bằng `floor()` |
| `CalcItemCount`, `ITEM_GetImmediaItemIndex`, `CountFreeRoomByWH` | bọc quanh `pos_immediacy` / `FindRoom` / `GetItemCount` đã có |
| `DisabledStall`, `ForbitTrade` | phải thêm 2 cờ mới vào `KPlayer` + 2 điểm chặn |

### Nhóm 2 — chiều sâu (14 hàm, đợt B)

`CalcEquiproomItemCount` · `ForbitStamina` · `GetSkillState` · `GetLastAddFaction` · `GetBoxLockState` · `CheckGlobalTradeFlag` · `ST_CheckTextFilter` · `Time2Tm` · `LoadScript` · `GetGateWayClientID` · `GetRespect` · `AddRespect` · `SetRespect` · **4 hàm `ST_*DamageCounter`** (gộp thành 1 hạ tầng)

> Hạ tầng `Respect` **đã có** trong engine (16 hit) — chỉ thiếu lớp đăng ký Lua.
> `GetLastAddFaction` có thể thay bằng `GetFaction`/`GetLastFactionNumber` đã có.
> `LoadScript` — dự án có `ReLoadScript` (`ScriptFuns.cpp:13364`); 17 call site đều nằm trong lệnh GM nạp lại script.

### Nhóm 3 — bỏ (9 hàm)

`GetStringTask` · `SetStringTask` · `LoadLadder` · `SyncAllLadder` · `TaskName` · `TaskTime` · `TaskInterval` · `TaskCountLimit` · `GetGlbMSRestTime`

### Hàm đã có nhưng PHẢI SỬA (6)

| Hàm | Vấn đề | Vị trí |
|---|---|---|
| `LuaRunMission` | sai chính tả đường dẫn + deref NULL | `ScriptFuns.cpp:10394` |
| `LuaStartMissionTimer` | không chống trùng slot | `ScriptFuns.cpp:10505` |
| `GlobalExecute` | chưa bóc tiền tố `dw `/`dwf ` | `KTongJX2.cpp:4023` |
| `OpenMission` | gọi hàm Lua `"BeginMission"`, script Linux đặt tên `"InitMission"` | `ScriptFuns.cpp:10365` |
| callback timer mission | engine gọi `"OnMissionTimer"`, script Linux đặt `"OnTimer"` | `KMission.cpp:336` |
| nhịp `Breathe` GlbTimer | không set `SCRIPT_SUBWORLDINDEX` ⇒ mọi hàm Mission gọi từ `OnTimer` **no-op im lặng** | `KJx2League.cpp:996-1033` |

---

## 9. BẢY BẪY ĐÃ BIẾT — ĐỪNG MẮC LẠI

1. **Mở nhầm cây dữ liệu** — xem mục 1.
2. **Đếm hàm không phân biệt hoa-thường** — `Random` vs `RANDOM`. Bảng băm tên hàm Lua phân biệt hoa-thường (bài học `g_FileName2Id` 19/08).
3. **Dùng `re_elf_luamap.py` cũ** — bỏ sót 216 tên.
4. **Chép bảng ID vật phẩm/danh hiệu** — cả **19 mã vật phẩm** WLLS (1254-1257, 2126, 2212-2220…) và **danh hiệu 81-88 / 100-103 / 200-207** đều **đã bị chiếm** trong bảng của dự án. Phải ánh xạ lại **trong script**, không sửa bảng. May là 12/19 món tương đương đã tồn tại dưới ID khác (Lệnh bài vinh dự 1259-1262, Chân Kinh 2222-2226, Hồng Anh Bảo Rương 2227, Càn Khôn Song Tuyệt Bội 2228).
5. **Va chạm tên toàn cục khi gộp relay vào GS** — hai tệp `head.lua` cùng định nghĩa `FALSE` và `_M`, bản nạp sau thắng. GS bản gốc **đã nạp cả 7 tệp relay** (log khởi động xác nhận) nhưng ở đó chúng là **mã chết**; gộp 1-GS thì chúng thành mã **sống**.
6. **Tưởng `script\header\liendau.lua` là điểm khởi đầu** — nó là **MÃ CHẾT hoàn toàn**: không tệp nào Include, và nó gọi `GetDataInt`/`GetDataGr` vốn **không tồn tại ở bất kỳ `.lua`, `.dll` hay `.exe` nào**.
7. **Băm tên tệp pak sai** — `g_GetPackPath` cắt `\` đầu + hạ chữ thường toàn bộ; `char` là signed. Băm sai ⇒ kết luận nhầm "thiếu bản đồ".

### Bẫy phụ

- `Ladder` dự án chỉ giữ **TOP 10** (`JX2LADDER_TOP = 10`), bản Linux xếp tới hạng 512.
- `GetGlbValue`/`SetGlbValue` **chỉ RAM** (`KJx2SharedStore.cpp:463-492`, không có `fopen` nào) ⇒ khởi động lại giữa mùa là mất pha thi đấu. Cần cờ cứu hộ lúc boot.
- `nLid` **không ổn định qua khởi động lại** (`sLeagueLoad` cấp lại bằng `++s_nNextLid` theo thứ tự dòng) ⇒ **luôn tra lại bằng `LG_GetLeagueObj(type, tên)`**, đừng lưu lid.
- `LGM_ApplyRemoveMember` **bỏ qua đối số thứ 6 `removelg`** (`KJx2League.cpp:758-788`) ⇒ league rỗng tồn tại vĩnh viễn trong `jx2league.txt`. `league.lua:104,128,132` đặt `removelg=1` để engine tự xoá.
- Hai stub `LG_GetMemberObj` / `LG_GetMemberJoinTime` — chú thích trong mã ghi "0 call site Lua" là **sai**: có 2 call site (`shitu\shitu.lua:287`, `missions\statinfo\timer_goodssale.lua:92`). Không ảnh hưởng leaguematch nhưng đang hỏng logic khoá 24 giờ sư đồ.
- `sLadderSave` **không kiểm `ferror`** trước `MoveFileEx` (`KJx2SharedStore.cpp:357-358`) — đĩa đầy sẽ để bản ladder cụt đè bản tốt.
- `SetMissionV` `strcpy` vào `char[100][16]` **không kiểm độ dài** (`KMissionArray.h:36-41`).
- `GetMissionV`/`SetMissionV` **lệch điều kiện chỉ số**: `Set` chặn `< 0` (id 0 ghi được), `Get` đòi `> 0` (id 0 đọc ra 0) ⇒ ô 0 là hố đen.
- `g_GlobalMissionArray` 50 khe **chưa bao giờ được dùng** — `Activate()` bị chú thích ở `KSubWorldSet.cpp:101`; nó chỉ là kho key-value cho `GetGlbMissionV`/`SetGlbMissionV`.
- `faction.lua` khai **TRÙNG** bộ map Hoa Sơn `{991,993,992}` cho cả mtype 11 lẫn 22 (`:74-79` và `:140-145`) ⇒ `map_index` bị ghi đè.
- Ladder id **10225-10226** ("Đấu ba người") **trùng** với mục "Xếp hạng bang hội" trong theme xếp hạng client bản VN — chỉ quan trọng nếu port cả UI xếp hạng.

---

## 10. 10 LỖI TỒN ĐỌNG TRONG BẢN LIÊN ĐẤU ĐANG CHẠY

Ngoài phạm vi port, nhưng đang ảnh hưởng người chơi thật. Chưa sửa gì.

| # | Lỗi | Vị trí |
|---|---|---|
| 1 | **Hạng cao cấp không bao giờ thắng được bằng sát thương.** `TaskTemp[50]` chỉ được engine cộng trên map **209** và **397**; map **399 không nằm trong điều kiện** ⇒ mọi trận cao cấp hết giờ đều `myDame == enemyDame == 0` ⇒ luôn hoà | `KNpc.cpp:3586-3592` — call site **DUY NHẤT** của `SetClearVal(50,...)` |
| 2 | **Luật cấm vật phẩm hiện KHÔNG chạy** — `CheckForbidItem()` gọi 3 hàm không tồn tại, cả 3 dòng đang bị `--` | `lib_liendau.lua:326, 337, 342` |
| 3 | **Bảng xếp hạng có thể không bao giờ được sắp** — `CapNhatHang()` chỉ gọi từ `CheckFull()`, tức chỉ khi người chơi bấm menu. Cả tháng không ai bấm ⇒ thưởng phát theo **thứ tự đăng ký**. Dữ liệu thật xác nhận: `data\11\danhsachtrung.lua` dòng [1] có 0 trận thắng, [2] có 4 | `lib_liendau.lua:124` |
| 4 | `return` sớm trong vòng lặp đếm ngược ⇒ chỉ người chơi ĐẦU TIÊN nhận thông báo, `PlayerIndex` không phục hồi | `timer_liendau.lua:64-78` |
| 5 | Người chết **kẹt lại map đấu trường** — khối người chết không có `NewWorld()` | `playerdie.lua:21-34` vs `:42-58` |
| 6 | `MAX_PLAYER` định nghĩa **2 lần khác giá trị** — `lib_ham.lua:6` = 400, `lib_map.lua:58` = 280; nạp sau thắng. Riêng `QuyDinhThangThuaTheoDame()` lại dùng `GetCountPlayerMax()` ⇒ không nhất quán | |
| 7 | `Include` tệp không tồn tại — `data\<tháng>\danhsachthamgia.lua` không có ở bất kỳ tháng nào | `quanvienhoitruong.lua:8` |
| 8 | `sukien_liendau()` **đang bị tắt** ⇒ mất thông báo toàn server, và đoạn **xoá dữ liệu tháng trước không chạy** — sau 1 năm dữ liệu cũ sẽ "sống lại" chồng lên mùa mới cùng tháng | `timerserver.lua:68` |
| 9 | **Chiến lược tối ưu là KHÔNG ĐÁNH** — thua vì ít sát thương vẫn được 15 điểm + 10 triệu exp; chỉ CHẾT mới 0 điểm. Đứng yên hết giờ ⇒ 48 × 15 = 720 điểm/mùa | `timer_liendau.lua:292, 434` |
| 10 | `ReLoader()` ghi đè **toàn bộ** tệp bảng sau **mỗi** kết quả trận của **mỗi** người chơi — tối đa 752 lần mở/ghi/đóng tệp mỗi giờ, đồng bộ trong luồng game chính | `lib_liendau.lua:206-233` |

---

## 11. CÒN BỎ NGỎ (chưa kiểm được)

- **Chưa chạy thử trong game** — mọi kết luận về hành vi đều từ đọc mã.
- Pak bản đồ phía **client thật** (`maps_client.pak` trong `bin\server` thiếu 2 thư mục liên đấu, nhưng đó chỉ là bản sao).
- Ý nghĩa/tính bắt buộc của các tệp `Maps\*_srv.fp` (thiếu toàn bộ cho map liên đấu).
- Engine dự án có nhận diện 2 cờ `FORBIT_ITEM_TYPE` `PKEX` và `LEAGUE` hay không.
- Dự án có hệ Sư đồ (League loại 1 + TASK 1403) để chạy thể loại 3 hay không.
- `KLinkArrayTemplate.h` chưa đọc ⇒ chưa chắc 10 khe mission là 10 dùng được hay 9 (với timer thì đã xác định là **9**, vì constructor `KMission` lặp từ `i = 1` — `KMission.h:138`).
- Chưa đối chiếu **chữ ký từng hàm** `LG_*` của dự án với bản Linux (mới xác nhận `LG_GetLeagueInfo` trả 3 giá trị, đúng như `head.lua:110` mong đợi).

---

## 12. NHẬT KÝ PHIÊN 20/08

| | |
|---|---|
| Yêu cầu | dịch ngược bản Linux tìm tính năng liên đấu, so với bản dự án, ước lượng số hàm phải viết, vẽ sơ đồ — **chỉ phân tích** |
| Cách làm | 2 workflow đa tác tử (13 + 3 tác tử), ~3,4 triệu token, 895 lượt gọi công cụ; kèm 3 tác tử phản biện độc lập |
| Đã sinh ra | `PHANTICH_LIENDAU_LINUX_VS_DUAN.md`, tệp này, 2 công cụ mới + 3 tệp dữ liệu trong `ReverseTools/` |
| **Chưa động vào** | **không sửa một dòng mã nguồn nào, không build, không deploy** |

Hai đính chính người dùng đưa ra trong phiên (đã ghi vào bộ nhớ):
1. Cây máy chủ chạy thật là `TESTLOFFF_ONLINE\bin\server`, **không phải** `dulieugame\server`.
2. Có mã nguồn thì **đọc mã nguồn**, chỉ dịch ngược bản Linux — không soi `CoreServer.dll` của chính dự án.
