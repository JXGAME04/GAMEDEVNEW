# PORT SIMCITY (BOT NGƯỜI CHƠI GIẢ LẬP) JX2 → CÂY NGUỒN JX1 `D:\GAMEDEVNEW`

Ngày: 16/08/2026 · Giai đoạn 1 (khảo sát ánh xạ) XONG — **chỉ đọc, chưa sửa mã**.
Nguồn: 11 tác nhân khảo sát + 2 tổ phản biện độc lập. Yêu cầu chủ game: **"dev đầy đủ, phân tích cho phù hợp mã nguồn, không copy, không đoán mò"**.

Kết quả phản biện: **0/48 khẳng định bịa** (mọi tên hàm/trường/hằng đều tồn tại thật, mọi kết luận "JX1 không có X" đều grep lại đúng). Chỉ có 9 lỗi trích sai **số dòng** (đã ghi nhận), không lỗi bản chất.

---

## 0. KẾT LUẬN LỚN NHẤT — JX1 DỄ HƠN JX2 Ở CHỖ NỀN TẢNG

| | JX2 (bản gốc) | JX1 (`D:\GAMEDEVNEW`) | Hệ quả |
|---|---|---|---|
| Trần NPC template | 1999, phải dùng `vdk.so` vá lên 4999 + mmap lại 14,4 MB | **`MAX_NPCSTYLE = 3000`** (`KNpc.h:25`), đang dùng tới id 2030, **còn 969 khe** | **Toàn bộ tầng native vá-trần của `vdk.so` là THỪA với JX1** |
| Thêm mẫu bot | patch nhị phân | thêm dòng vào `npcs.txt` | chỉ sửa dữ liệu |
| `NpcSettingIdx` qua mạng | int | `int` (`KProtocol.h:474`) | **không đổi giao thức** |
| Timer toàn cục gọi Lua | `KTimerList` multimap | **có** `StartGlbMSTimer` (`KJx2League.cpp:936`), production từ đợt công thành | dựa vào hạ tầng sẵn có |

⇒ **Không có tầng nào bắt buộc phải patch nhị phân.** Đây là khác biệt căn bản: bản JX2 sống nhờ `vdk.so` bơm 51 hàm + nới trần; JX1 chỉ cần **viết các hàm C++ tương ứng vào `Core/Src` và thêm dữ liệu**.

---

## 1. BA TẦNG — ÁNH XẠ SANG JX1

### 1.1 Tầng dữ liệu — DÙNG NGUYÊN ĐƯỢC (đã tự xác minh 2 lần)

Tổ phản biện #2 **tự parse `MapList.ini` + `.wor` + `Region_C.dat`** (đọc bố cục từ `KRegion::LoadLittleMapData`, `KRegion.cpp:556-627`), không tin bảng có sẵn:

```
map kiểm được       : 143
tổng node           : 15.102
node vào ĐÚNG region: 15.102  (100,000 %)   <- dữ liệu JX2 khớp bản đồ JX1
  -> ô đi được      : 15.027  ( 99,503 %)
đối chứng (gán node map A sang map B): chỉ 30–42 %  <- nền ngẫu nhiên
```

100% không thể do trùng hợp ⇒ **đồ thị đường đi của SimCity dùng thẳng trên JX1**. Kế hoạch di chuyển **không sụp đổ**.

**Nhưng phát hiện mới (phản biện tự raycast 16.618 cạnh):** **1,47% cạnh (244 cạnh) đâm xuyên ô chặn** của bản đồ JX1, mà **server JX1 không có A\*** (`KJXPathFinder.h:12` là `#ifndef _SERVER`, chỉ client mới có). ⇒ Cứ ~68 chặng thì 1 chặng bot kẹt tường đứng im. **Bắt buộc thêm cơ chế chống kẹt** (vị trí không đổi sau N nhịp → đổi node), điều bản JX2 không cần vì `vdk.so`/engine JX2 khác.

Dữ liệu nhân vật: tên (738) + chat (2.619 câu) là văn bản TCVN3 dùng lại được; skillId/phái/pet phải đối chiếu bảng JX1 (tổ `data:npc-skill-pet` — tỷ lệ khớp còn phải chốt từng id).

### 1.2 Tầng Lua — GIỮ ĐƯỢC PHẦN LỚN, nhưng KHÔNG có pack/namespace

JX2 mỗi tệp script có pack riêng (`lua_usepack`); **JX1 không có** — mọi biến toàn cục của 40 tệp SimCity sẽ dùng chung không gian tên với 861 hàm + hàng trăm biến toàn cục của JX1. ⇒ **Bắt buộc thêm tiền tố** (vd `SC_`) cho mọi biến toàn cục SimCity. Đã kiểm: `"SC_"`/`"Sim"` hiện **không đụng** tên nào trong 861 tên đăng ký.

Cơ chế nạp: JX1 quét cả thư mục (`KSortScript.cpp:223`, nạp cả `.TXT`), gốc phụ `\scriptjx2\tong_vn` (`KSortScript.cpp:65`) — **đây là tiền lệ**: đợt bang hội JX2→JX1 đã gắn script vào đúng đường này. Thư viện chuẩn Lua (tinsert/getn/strfind/format/mod/floor/random/sqrt) đều có; `random = math_random`. **`objCopy` phải kiểm** (SimCity dùng để tạo lớp con).

### 1.3 Tầng native — VIẾT MỚI 51 hàm bằng C++ (không bê `vdk.so`)

`vdk.so` vá địa chỉ tuyệt đối của một bản Linux 32-bit → **không dùng được**. 51 hàm phải viết lại vào `Core/Src`, theo khuôn `KJx2WarInfra.h` (header cầu nối JX2→JX1 của đợt công thành, bọc trọn `#ifdef _SERVER`).

**46 API game** SimCity gọi: 29 dùng nguyên/đổi tên, 17 phải viết mới hoặc sửa. Các điểm đã chốt bằng mã:

| API JX2 | JX1 | Ghi chú then chốt |
|---|---|---|
| `AddNpcEx` | viết mới bọc `AddNpcSet2` | JX1 có `AddNpc` nhưng **thứ tự tham số khác** (series ở vị trí 6, JX2 ở vị trí 3) |
| `NpcWalk` | `NpcWalk` (`ScriptFuns.cpp:6069`) | 🔴 **JX1 nhận MPS, SimCity truyền Ô LƯỚI** → phải **nhân 32**. Bê thẳng = lệch 32 lần |
| `SetNpcParam`/`GetNpcParam` | có tên | 🔴 **`MAX_NPCPARAM = 4`** (ô 0..3), SimCity dùng ô **1,2,3,4** → ô 4 **tràn sang `m_bNpcFollowFindPath`**. Phải nới mảng hoặc dồn cờ |
| `SetNpcAI` | `SetNpcAIType` | đổi tên |
| `ChangeNpcFeature` | **stub rỗng** (`ScriptFuns.cpp:12734`) | phải viết thật nếu cần đổi ngoại hình |
| `AddTimer`/`DelTimer` | không có theo tên | dùng `StartGlbMSTimer` + **tầng điều phối Lua** (xem 2.2) |
| `NpcCastSkill` | `CastNpcSkill` | đổi tên |
| `NpcDropMoney` | `DropNpcMoney` | đổi tên |

**51 hàm vdk.so**: 0/51 trùng tên → tất cả viết mới. Phần lớn là bọc mỏng quanh hàm C++ sẵn có của JX1 (`NpcRun`→`KNpc::RunTo`, `EnforceBotHp`→ghi `m_CurrentLife`…); một số phụ thuộc gói tin đồng bộ (`SetNpcCombat/SetNpcPeace/SetNpcStall` phát `SendDataToNearRegion`) — **dùng gói CÓ SẴN của JX1, không đổi giao thức**.

---

## 2. BA ĐIỀU PHẢI CHỐT TRƯỚC KHI VIẾT (phản biện nhấn mạnh)

### 2.1 🔴 `m_Kind` của bot — QUYẾT ĐỊNH LỚN NHẤT, chênh 100× khối lượng

Hai báo cáo mâu thuẫn vì ngầm giả định khác nhau. Phản biện #2 giải quyết:

**Hướng A — `kind_normal` (an toàn):**
- Sinh bằng `AddNpc` nguyên bản → `GetNpcCopyFromTemplate` chạy đủ → **có stat thật, không kế thừa rác**.
- Né toàn bộ 76 điểm `IsPlayer()` + 198 điểm `Player[m_nPlayerIdx]`.
- **0 dòng C++ đụng `KNpc`/gói tin** cho GĐ1.
- **Đánh đổi**: không đổi trang bị động (`ChangeNpcFeature` là stub), không mang tên bang/danh hiệu người chơi, ngoại hình = 1 trong 24 bộ đồ cố định theo dòng `npcs.txt`.

**Hướng B — `kind_player` (đẹp, nhưng chạm luật):** phản biện tìm **4 chặn + 4 tác dụng phụ đổi cân bằng**, tất cả có bằng chứng dòng:
- Template −1/−2 trên server **bỏ qua `LoadDataFromTemplate`** → bot không có stat, kế thừa rác NPC cũ (`KNpc.cpp:5045-5075`). Phải viết **bộ khởi tạo thuộc tính bot mới ~300-500 dòng** đụng mã dùng chung.
- `Player[0]` là **ô nháp dùng chung** mọi bot ghi vào (`KPlayerSet.cpp:59`, `KNpc.cpp:1420/3588/3680/3746`) — trạng thái toàn cục ẩn.
- Bot `kind_player` tắt FightMode **không bao giờ chết** (`KNpc.cpp:1414`).
- 🔴 **Đổi cân bằng gameplay (vi phạm Gate 4)**: người chơi đánh bot nhận exp skill 90/120 theo hệ số "đánh người" (`KNpc.cpp:3672-3700`); giết bot **lên điểm PK/đỏ tên** (`:8397`); bot đánh người chơi làm script `OnDamage` **câm** (`:3751`).

⇒ **Đề xuất: GĐ1 dùng hướng A.** Hướng B (nếu chủ game thật sự cần ngoại hình người chơi đầy đủ + tên bang) là **một đợt riêng** có ngân sách thật và phải kèm gói vá 4 chặn + kẹp 4 tác dụng phụ.

### 2.2 🔴 Timer — 1 timer duy nhất, tự điều phối trong Lua

JX1 có `StartGlbMSTimer` (`KJx2League.cpp:936`) chạy mỗi tick, không cần người chơi — **nhưng**: gọi tên cứng `"OnTimer"` 0 tham số (`:1021`), kẹp sàn **≥18 frame = 1 giây** (`:947`), trần **16 timer nổ/tick** (`:996`). SimCity gọi `AddTimer(18,"mainLoop",obj)` = tên hàm + đối tượng.
⇒ **Không đăng ký 1 timer/bot** (149 map sẽ trôi timer). Phải: **1 timer toàn cục** → Lua đọc bảng callback tự phân phối. Đây là tầng phải tự viết, không báo cáo native nào thay được.

### 2.3 🔴 Trần 256 NPC phía CLIENT + không chốt số bot

Client `MAX_NPC = 256` (`KNpc.h:23`), dùng 1..255 **tính cả người chơi thật**. `KRegion::SendSyncData` gửi **toàn bộ** NPC region cho client không trần. Cấu hình gốc `THANHTHI_SIZE=300` → phản biện đo mật độ: cửa sổ 9-region đặc nhất có **26–45 bot** → thành thị sống được, **Tống Kim gần chắc tràn**. `SyncNpc`/`SyncPlayer` không kiểm `nIdx==0` (`KProtocolProcess.cpp:1795/2230`).
⇒ **Bắt buộc kẹp số bot theo region**, không để `STARTUP_AUTOADD_THANHTHI=1` mặc định.

---

## 3. KẾ HOẠCH THEO GIAI ĐOẠN (đã sửa theo phản biện — mỗi GĐ chạy + kiểm chứng độc lập)

### 🎯 GĐ1 — "MỘT BOT ĐI BỘ" — 0 dòng C++ đụng KNpc/gói tin

| Bước | Việc | Dựa vào |
|---|---|---|
| 1 | Thêm 1 dòng `npcs.txt` (server **và** client) tại **id 2031**, `NpcResType=MainMan` | id trống thật đầu tiên là 2031 (client đã dùng tới 2030) |
| 2 | Sinh bot bằng `AddNpc` nguyên bản, `m_Kind` = `kind_normal` | né 4 chặn + đổi cân bằng |
| 3 | Nhịp: 1 timer qua `StartGlbMSTimer` + 1 dòng `TimerTask.txt` | production từ đợt công thành |
| 4 | Di chuyển: đọc `*_nodes.txt` bằng `TabFile_*` sẵn có, gọi `NpcWalk` **× 32** | né lỗi lệch đơn vị |
| 5 | Chống kẹt: `GetNpcPos` không đổi sau 5 nhịp → đổi node | trả lời 1,47% cạnh xuyên tường |

**Nghiệm thu:** 1 bot mang hình người đi vòng quanh Biện Kinh theo node, không kẹt >30s, **0 tệp `.cpp` bị sửa**. Chứng minh trọn đường dây dữ liệu → Lua → NPC hiện trong game.

Trước GĐ1: bổ sung 4 module vào bảng `IncludeLib` (`ScriptFuns.cpp:2322`) — `ITEM/NPCINFO/TASKSYS/TIMER` — nếu không, hàm của 4 module này thành `nil`, hỏng câm lúc chạy.

### GĐ2 — nới hàm C++ tối thiểu
`GetNpcKind`, `SetNpcLevel`, `NpcRun` (bọc `KNpc::RunTo`), sửa `SetNpcParam` để có ≥4 ô an toàn. ~4-6 hàm, tất cả trong `#ifdef _SERVER` của `ScriptFuns.cpp`. Kiểm: nhiều bot đi + đổi cấp.

### GĐ3 — chat, ngồi, cưỡi ngựa, trạng thái hoà bình
Viết nhóm `SetNpcPeace/SetNpcStall/NpcSit/SetNpcRideHorse` bọc gói đồng bộ CÓ SẴN. Kiểm: bot chào hỏi, ngồi, cưỡi ngựa.

### GĐ4 — chiến đấu (bot ⟷ bot)
`SetNpcCombat/BotDoSkill/EnforceBotHp/SetNpcFightTarget`. **Chưa** cho bot đánh người chơi. Kiểm: 2 bot khác phe đánh nhau, không đụng người chơi.

### GĐ5 — plugin phụ + (tuỳ chọn) ngoại hình người chơi đầy đủ
Tống Kim, kéo xe, shop pet. Nếu chủ game cần ngoại hình `kind_player` → đợt riêng kèm gói vá 4 chặn.

**Ước lượng (phản biện chỉnh lên):** ≥ đợt bang hội (28 commit) + đợt công thành (12 commit) gộp lại = **40-70 commit**. Ngân sách bộ nhớ: 300 bot × 149 map × 6.008 B ≈ 268 MB nếu bật hết — **phải chốt trần bot toàn cục**.

---

## 4. NHỮNG GÌ NÊN BỎ / KHÔNG PORT
- Tầng vá-trần của `vdk.so` (JX1 không cần).
- `funSys:Update` (chat ngẫu nhiên, rớt tiền, hồi máu) — bản JX2 **không bao giờ gọi**, là mã chết.
- Nhánh `group_fighter*` — engine cũ đã chết trong chính bản JX2.
- Ngoại hình `kind_player` đổi trang bị động — chỉ làm nếu chủ game yêu cầu, là đợt riêng.

---

## 5. RÀNG BUỘC GIỮ SUỐT
- Sửa tệp TCVN3 qua `safe_edit.py`, kiểm `check_encoding.py` trước+sau (high-byte không đổi, FFFD=0).
- Không đổi giao thức/gói tin/bố cục struct/DB. Không đổi cân bằng như tác dụng phụ.
- Ký hiệu chỉ-server bọc `#ifdef _SERVER` (nhiều tệp `Core/Src` biên dịch vào cả client Win32).
- Mỗi giai đoạn: viết → build compile PASS → **vòng phản biện độc lập đọc lại diff** → sửa → mới sang GĐ sau.
- Thêm dòng `npcs.txt` phải vào **cả server lẫn client** cùng id, đóng gói lại pak client, giữ đủ 87 cột.
