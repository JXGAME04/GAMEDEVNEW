# BÀN GIAO — PORT SIMCITY (BOT NGƯỜI CHƠI GIẢ LẬP) JX2 → JX1

Ngày: 16/08/2026 · Cây nguồn: `D:\GAMEDEVNEW` (nhánh main) · Cây chạy: `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server`

Mỗi bản vá đều **build Server Release|x64 PASS** và **qua vòng phản biện độc lập 3 lăng kính**. Chi tiết phân tích: `PHANTICH_SIMCITY_JX2.md`, kế hoạch: `PORT_SIMCITY_JX1_KEHOACH.md`.

---

## 1. ĐÃ LÀM (4 bản vá)

| Bản vá | Nội dung | Tệp |
|---|---|---|
| **Fix1** | Lớp giữ cân bằng: cờ `KNpc::m_btSimCityBot` + **6 cổng** (bot kind_player miễn nhiễm: exp skill 90/120, điểm PK, OnDamage, task map 209/397, bẫy CheckTrap) | `KNpc.h`, `KNpc.cpp`, `KSimCity.cpp/.h` |
| **Fix2** | Sinh/xoá bot kind_player mang ngoại hình MainMan qua **sentinel −1/−2** (0 dòng `npcs.txt`): `SC_AddBot`, `SC_DelBot`, `SC_ClearBots` | `KSimCity.cpp` |
| **GĐ2** | Driver di chuyển: sổ bot C++ + `SC_Breathe()` hook `CoreServerShell::Breathe` + `SC_MoveOn/Off`, `SC_Goto`, `SC_PatrolBox` | `KSimCity.cpp`, `CoreServerShell.cpp` |
| **GĐ2b** | Nạp lộ trình bản đồ thật từ file: `SC_LoadPreset`, `SC_SetBotRoute` (bảng route chung `s_routes[]`) | `KSimCity.cpp` + 271 tệp dữ liệu |

**Nguyên tắc port** (khác hẳn bản JX2):
- **Không dùng `vdk.so`** (JX2 patch nhị phân). 51 hàm native viết lại bằng C++ trong `Core/Src` (mới dùng ~14 hàm cho GĐ này).
- Bot = **KNpc `kind_player`** để có ngoại hình đầy đủ, **nhưng 6 cổng giữ cân bằng** — người chơi & quái thật **giữ hành vi y hệt**.
- **Không đổi giao thức/struct/DB** (dùng gói `s2c_npcwalk`/`PLAYER_NORMAL_SYNC` sẵn có). Cờ `m_btSimCityBot` là byte server-only, không lên wire/DB.

**Tệp mã đã sửa** (chỉ commit các tệp này, KHÔNG lẫn thay đổi Dã Tẩu):
```
Sources/Core/Src/KSimCity.cpp     (mới)
Sources/Core/Src/KSimCity.h       (mới)
Sources/Core/Src/KNpc.cpp         (+7 cổng/khởi tạo cờ)
Sources/Core/Src/KNpc.h           (+field m_btSimCityBot)
Sources/Core/Src/ScriptFuns.cpp   (+đăng ký 11 hàm SC_*)
Sources/Core/Src/CoreServerShell.cpp (+include + SC_Breathe() hook)
Sources/Core/Core.vcxproj         (+KSimCity.cpp/.h, loại khỏi 4 config Client)
serverdata_jx2/settings/simcity/maps/thanhthi/  (271 tệp dữ liệu lộ trình)
```

---

## 2. DEPLOY (chép tay sang cây chạy E: — như tiền lệ công thành)

> `D:\GAMEDEVNEW` là cây **NGUỒN**; server chạy từ `E:\...\bin\server` (cwd của `GameServer.exe`). Build không tự deploy.

**Bước:**
1. **Build** (đã xong): `CoreServer.dll` ở `D:\GAMEDEVNEW\Sources\Core\x64\ServerRelease\CoreServer.dll`.
2. **Tắt** `GameServer.exe`.
3. **Chép binary:**
   ```
   copy /y "D:\GAMEDEVNEW\Sources\Core\x64\ServerRelease\CoreServer.dll" "E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\Coreserver.dll"
   ```
   ⚠️ **Cần bạn xác nhận đường này** — `CAPNHAT_SERVER_JX2.bat` hiện trỏ nguồn `SOURCESUPDATE_KINHMACH_ONL...`, KHÔNG phải `D:\GAMEDEVNEW`. Nếu bạn build/deploy từ cây khác, chép từ đó.
4. **Chép dữ liệu lộ trình** (271 tệp) — đặt dưới `settings\` (KHÔNG `script\`, vì boot nạp cả `.TXT` như Lua):
   ```
   robocopy "D:\GAMEDEVNEW\serverdata_jx2\settings\simcity\maps\thanhthi" "E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\simcity\maps\thanhthi" *.txt
   ```
5. **Khởi động lại** `GameServer.exe`.

---

## 3. TEST TRONG GAME (lệnh GM, tiền tố `?gm ds`)

> `?gm ds <mã Lua>` — lệnh DoScript gọi thẳng hàm C đã đăng ký. Cần GM/người chơi **đứng gần** vì region phải "nóng" (có người chơi) bot mới bước.

### 3.1 Smoke test — tuần tra hình vuông (chạy ở BẤT KỲ ĐÂU, không cần dữ liệu)
```
?gm ds x = SC_AddBot(0, 100, 0, 1625, 3170)
?gm ds SC_MoveOn()
?gm ds SC_PatrolBox(x)
```
→ 1 bot mang hình người chơi đi vòng 4 góc quanh chỗ tạo. Dọn: `?gm ds SC_ClearBots()`.
*(tham số SC_AddBot: `nSex(0 nam/1 nữ), nLevel, nSubWorldIdx, nMpsX, nMpsY, nSeries`; toạ độ MPS)*

### 3.2 Test lộ trình bản đồ thật (sau khi deploy 271 tệp dữ liệu)
🔴 **DÙNG GẠCH XUÔI `/` trong đường dẫn** — Lua 4.0 nuốt escape `\` (biến `\t`→TAB, `\1`→ký tự lạ) làm đường dẫn hỏng:
```
?gm ds rid = SC_LoadPreset("/settings/simcity/maps/thanhthi/1_phuongtuong_preset.txt", "phuongtuong1")
?gm ds x = SC_AddBot(0, 100, SubWorldID2Idx(1), 1361*32, 2758*32)
?gm ds SC_SetBotRoute(x, rid, 1)
?gm ds SC_MoveOn()
```
→ bot đi vòng theo tuyến `phuongtuong1` của Phượng Tường (map 1).
*(spawn gần node đầu tuyến để bot bắt nhịp nhanh; toạ độ node `X_Y` × 32 = MPS)*

**Bảng hàm GM:**
| Hàm | Việc |
|---|---|
| `SC_AddBot(sex,lv,swIdx,x,y,series[,tên][,phe][,máu])` → idx | sinh bot |
| `SC_DelBot(idx)` / `SC_ClearBots()` | xoá 1 / xoá hết |
| `SC_MoveOn()` / `SC_MoveOff()` | bật/tắt driver di chuyển |
| `SC_Goto(idx, mpsX, mpsY)` | đi 1 chặng (test tay) |
| `SC_PatrolBox(idx[, halfMps])` | tuần tra hình vuông quanh vị trí |
| `SC_LoadPreset("/…/…_preset.txt", "PathName")` → routeId | nạp lộ trình từ file |
| `SC_SetBotRoute(idx, routeId[, bLoop=1])` | gán lộ trình cho bot |
| `SC_SetBotFlag(idx,0/1)` / `SC_GetBotFlag(idx)` | cờ bot (debug) |

---

## 4. CƠ CHẾ ĐÃ CHỐT BẰNG MÃ (đừng đoán lại)
- `SendCommand(do_walk,x,y)` = **bắn-rồi-quên**: phát 1 lần/chặng, engine tự bước mỗi tick (`ServeMove`), né vật cản cục bộ (men-tường, **không A***), tự dừng khi tới.
- **Bot đứng giữ `m_ProcessAI=1`** (`OnStand` không hạ) → lệnh đi tác dụng ngay. (`SC_AddBot` set `SetProcessAI(1)` cho chắc.)
- Ngưỡng tới chặng **48 MPS** > bước/tick (engine kẹp ≤31) → driver bẻ lái trước khi bot tự dừng.
- Chỉ phát lệnh khi **đổi chặng / kẹt** (không mỗi tick) — vì `DoWalk` broadcast trước early-return.
- **Bot chỉ đi khi region "nóng"** (có người chơi gần) — `KRegion` nóng qua `AddPlayer`, không qua `AddNpc`.
- `Player[0]` (bot có `m_nPlayerIdx=0`) là ô trơ, mọi ghi vô hại (không vào UseIdx/DB); khi bot băng region `PlayerChangeRegion(0)` no-op + `Abrade` gate `>0`.

---

## 5. GIỚI HẠN / CÒN LẠI
- **Chưa deploy** CoreServer.dll + 271 tệp sang E: (bước 2 — chờ bạn xác nhận đường build→E:).
- Bảng route dùng chung **64 slot** (LoadPreset + PatrolBox); tuyến ≤ **320 node**. Test lặp nhiều lần → `SC_ClearBots()` để reset.
- **GĐ2b chỉ nạp `thanhthi` (123 tuyến preset)**; chưa nạp attractions/haudoanh/pet/chat. Bot đi lộ trình nhưng **chưa** chat/đánh nhau/Tống Kim (các GĐ sau).
- HỞ hút HP/MP từ bot (`KNpc.cpp:3704-3730`) — hoãn tới khi làm bot chiến đấu.
- `_nodes.txt` (148 tệp, đồ thị kề) đã chép sẵn nhưng **chưa dùng** — để dành GĐ pathfinding thật (đi giữa 2 node bất kỳ) sau này.
