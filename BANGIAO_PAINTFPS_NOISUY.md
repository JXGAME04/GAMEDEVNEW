# BÀN GIAO — TÁCH NHỊP VẼ KHỎI NHỊP LOGIC (PaintFps) + NỘI SUY VỊ TRÍ

Ngày: 15/08/2026 · Trạng thái: đang phản biện + build, sẽ cập nhật mục 6 trước khi chốt

---

## 1. Bối cảnh — vì sao game "18 FPS"

Điều tra 15/08 (17 tác nhân, dump byte trực tiếp) chốt:

- **Nhịp LOGIC** của cả 3 phía đều là **18 tick/giây**, không phải 20, không phải 60:
  - Server Linux gốc `jx_linux_y`: bộ điều nhịp ở offset `0x5310/0x51e5/0x5332` — thời gian ×144, hạn khung `loop×8` ⇒ 144/8 = **18**. Chuỗi `[GameFPS = %d]` là FPS **đo được**, không phải cấu hình.
  - JX1 cây này: `Core\Src\KNpc.h:26`, `MultiServer\GameServer\KSOServer.cpp:91`, `S3Client\S3Client.cpp:62` đều `GAME_FPS 18`. Số 20 trong `S3Server\SwordOnLineServer.cpp:11` là **mã chết** (S3Server không có trong `JXAll.sln`, bản build cuối fail C1004).
- **Nhịp VẼ** mới là chỗ khác nhau:
  - Client JX2 gốc (`game_y.exe`): đã **tách nhịp vẽ** — `config.ini [Client] PaintFPS` mặc định **30**, sàn 1, không trần (VA `0x0056D7D5`), bộ đếm vẽ riêng, nhịp logic vẫn `imul ...,18` (VA `0x0056E1C5`).
  - Client JX1 cây này (trước bản vá): `UiPaint()` nằm **bên trong** cổng `GAME_FPS` ⇒ trần vẽ = 18 hình/giây, và vẽ không nội suy.
- `GAME_FPS` là **đơn vị thời gian toàn game** (buff, cooldown, timer Lua `*18`, tốc chạy) — **cấm** vặn lên 60. Muốn mượt phải tách nhịp vẽ + nội suy, đúng như bản vá này.

## 2. Bản vá gồm gì (6 file, client-only)

### 2.1 `Sources\S3Client\S3Client.cpp` — tách nhịp vẽ
- Biến mới (sau `#define GAME_FPS 18`, ~dòng 64):
  - `g_nPaintFps` (mặc định **30**) — số khung VẼ mỗi giây. `0` = trả về hành vi cũ nguyên vẹn (vẽ dính tick).
  - `g_nPaintInterp` (mặc định **1**) — bật nội suy vị trí.
- `GameInit` (~dòng 495): đọc `config.ini`:
  ```ini
  [Client]
  PaintFps=60      ; 0 = như cũ; kẹp 0..60 (vòng bơm ngoài trần ~60 nên đặt cao hơn là ảo)
  PaintInterp=1    ; 0 = tắt nội suy (vẽ nhanh nhưng vị trí vẫn nhảy theo tick)
  ```
  Kèm `timeBeginPeriod(1)`/`timeEndPeriod(1)` **chỉ khi PaintFps>30** — ai bật 60 mới trả giá độ phân giải timer, mặc định 30 và dàn multibox không đổi gì.
- `GameLoop` (~dòng 1205):
  - Tick logic 18/s giữ nguyên; trong tick **không vẽ nữa** mà gọi `GOI_PROCFRAME_BREATHE` (chụp vị trí tick cho nội suy; uParam=1 khi nội suy bật). `PaintFps=0` thì vẫn `UiPaint` trong tick như cũ.
  - **Cổng vẽ riêng, miễn nhiễm tràn số**: `nPaintDelta = (int)(elapse*PaintFps − s_PaintCounter*1000) ≥ 0` (hiệu có dấu — hai tích cùng wrap 2^32 nên hiệu luôn nhỏ) → tính `alpha = elapse*18 − (m_GameCounter−1)*1000` (kẹp 0..1000) → `GOI_PROCFRAME_POSSHIFT(alpha)` → `UiPaint`. Tụt sau **>2 khung vẽ** (modal loop, nạp map) thì resync bộ đếm, không vẽ bù dồn.
  - Pass vừa vẽ xong thì bỏ `Sleep(1)` (cờ `bPainted`); hai nhánh Sleep còn lại giữ nguyên.

### 2.2 `Sources\Core\Src\CoreShell.cpp` — nội suy vị trí (hiện thực 2 case chết sẵn có)
- Enum `GOI_PROCFRAME_BREATHE` / `GOI_PROCFRAME_POSSHIFT` **đã tồn tại** trong `CoreShell.h:804-806` từ trước → không đổi header, không đổi thứ tự enum.
- Trạng thái (trước `OperationRequest`, ~dòng 2492): `s_InterpFrom/To[MAX_NPC]`, `s_InterpNpcID[MAX_NPC]`, `s_InterpValid[MAX_NPC]`, `PAINT_INTERP_SNAP_DIST 64`.
- `GOI_PROCFRAME_BREATHE` (mỗi tick, thoát sớm nếu uParam=0): `from ← to`, `to ← Map2Mps(vị trí tick mới)`; cờ hợp lệ riêng nên **NPC ClientOnly có `m_dwID=0` (gà/chó/chim) vẫn được nội suy**; slot đổi chủ hoặc nhảy >64 px → snap (chống smear khi teleport).
- `GOI_PROCFRAME_POSSHIFT` (mỗi khung vẽ): `lerp(from→to, alpha/1000)` rồi `KNpcRes::SetPos(...)` — dịch **duy nhất vị trí vẽ**; NPC đứng yên (trừ nhân vật chính) được bỏ qua; **camera chỉ được ghi khi region(lerp) == region(to)** (biên region 512×1024 luôn được vượt tại tick — nơi cây vẽ được dựng lại ngay — không bao giờ tại paint). **Toạ độ logic `m_MapX/m_MapY/m_OffX/m_OffY` không bị đụng** → không ảnh hưởng combat/sync/WAuto.
- Vẽ trễ đúng **1 tick (~55 ms)** so với logic — cái giá chuẩn của nội suy fixed-timestep.

### 2.3 Overlay bám theo thân đã nội suy — `KNpc.h/.cpp`, `KNpcRes.h`, `CoreDrawGameObj.cpp`
- `KNpcRes::GetPos` (accessor mới, inline — `m_nXpos/m_nYpos` vốn private) + `KNpc::GetDrawPos` (client-only) trả **vị trí vẽ đã nội suy**.
- Đổi `GetMpsPos` → `GetDrawPos` tại đúng **11 điểm vẽ** trong `KNpc.cpp` (PaintSeriesNpc / PaintInfo ×5 / PaintChat / PaintLife / PaintMana / PaintTeamMNG / PaintBlood) + 1 điểm `CoreGetGameObjLightInfo` (`CoreDrawGameObj.cpp:242`) — hết cảnh tên/thanh máu/bong bóng chat/số damage/đèn động chạy trước thân người 10-14px rung 18Hz. `GetMpsPos` giữ nguyên cho mọi đường logic/mạng (24 điểm còn lại không đụng).

## 3. Vì sao KHÔNG dùng đường `OnRunByFPS/OnWalkByFPS`

Cây này có sẵn hạ tầng sub-step bị comment (`CoreShell.cpp` khối cũ + `KNpc.cpp:10901-10935`): chia tốc độ mỗi tick thành N bước nhỏ gọi `ServeMove` **dịch vị trí logic thật** giữa 2 tick. Không dùng vì:
1. `ServeMove` đổi `m_MapX/m_OffX` thật → mọi kiểm tra tầm đánh/va chạm/đồng bộ thấy vị trí lệch pha so với server.
2. Phải sửa `OnRun/OnWalk` để khỏi di chuyển đôi (tick vẫn `ServeMove` đủ tốc) — xâm lấn logic.
3. `m_bProcPosShift` chưa từng được gán ở đâu — hạ tầng dở dang.
Nội suy thuần vẽ đạt cùng độ mượt, rủi ro bằng 0 với logic.

## 4. Giới hạn đã biết (không phải lỗi)

| Thứ | Hiện trạng |
|---|---|
| Hoạt ảnh sprite (bước chân, chiêu) | Vẫn đổi khung theo tick 18/s (`m_Frames.nCurrentFrame`). Vị trí mượt, khung hình sprite không tăng — giống cách client JX2/mobile xử lý. |
| Tên lửa/ám khí (`KMissle`) | Chưa nội suy — bay nhanh, sống ngắn, khó thấy giật. Làm sau nếu cần. |
| `PaintFps>60` | Vô nghĩa: vòng bơm `KWin32App::Run` trần ~60 lần gọi `GameLoop`/giây (`nInterval=1000/60`). Muốn >60 thật phải sửa cả vòng bơm — không khuyến nghị. |
| Represent3 (D3D) | Không đặt `PresentationInterval` → vsync mặc định theo màn hình; PaintFps hiệu dụng ≤ tần số quét. Represent2 (DDraw, mặc định) không vướng. |
| Ô "FPS" trong bảng auto (WAuto GUI) | Vẫn vô tác dụng như trước (`UiShell.cpp:331` bị comment từ xưa) — không liên quan cơ chế mới. |
| CPU | Vẽ 30 fps ≈ +67% chi phí vẽ so với 18; 60 fps ≈ +233%. Máy yếu/treo nhiều cửa sổ: hạ `PaintFps` hoặc để `0`. |

## 5. Cách bật trên client đang chạy

1. Thay `Game.exe` (S3Client) + `CoreClient.dll` (Core) bản build mới — **phải thay cả hai**.
2. Mở `config.ini` cạnh Game.exe, mục `[Client]` thêm:
   ```ini
   PaintFps=60
   PaintInterp=1
   ```
   Không thêm gì thì mặc định 30 (đã mượt hơn 18 rõ rệt).
3. Muốn quay về hành vi cũ 100%: `PaintFps=0`.
4. Đo nhanh: bật hiển thị debug FPS (nếu build có `SWORDONLINE_SHOW_DBUG_INFO`) — `FPS=` giờ là **nhịp vẽ** (30/60), `LOOP=` vẫn là loop logic.

## 6. Kết quả build + phản biện

### Vòng phản biện 4 lăng kính (15/08) — 7 lỗi thật, đã vá toàn bộ

| # | Lỗi | Mức | Cách vá |
|---|---|---|---|
| 1 | Cổng vẽ `counter*1000 <= elapse*fps` **tràn DWORD** → màn hình đóng băng vĩnh viễn sau ~20h (60fps) / ~40h (30fps) — resync nằm trong `if` nên không tự thoát | **NẶNG** (client auto chạy 24/7) | So sánh qua **hiệu có dấu** `nPaintDelta` — hai tích cùng wrap nên hiệu luôn nhỏ, miễn nhiễm tràn vĩnh viễn |
| 2 | Tick đẩy camera tới `pos(N)`, POSSHIFT kéo về `lerp` — nếu 2 điểm nằm 2 bên **biên region** (512×1024 mps) thì `ClearPreprocess→Fell()` đốn cây vẽ ngay giữa lúc paint, `Preprocess()` chỉ dựng lại ở tick → **khung hình trắng cảnh vật mỗi ~2,4s khi chạy ngang** | **NẶNG** | POSSHIFT chỉ ghi camera khi `region(lerp) == region(to)` — biên luôn được vượt tại tick (rebuild ngay trong cùng Breathe như nguyên bản); camera khựng ~12px một nhịp mỗi lần vượt biên, không còn khung trắng |
| 3 | Tên/thanh máu/mana/bong bóng chat/số damage/icon hệ/khung tổ đội/đèn động lấy toạ độ qua `GetMpsPos` (logic) → **overlay chạy trước thân người 10-14px, rung răng cưa 18Hz** | **VỪA** | Thêm `KNpc::GetDrawPos` (đọc `KNpcRes::GetPos` — accessor mới, vị trí đã nội suy) và thay đúng **11 điểm** trong các hàm `Paint*` của `KNpc.cpp` + 1 điểm `CoreGetGameObjLightInfo` (`CoreDrawGameObj.cpp:242`). `GetMpsPos` giữ nguyên cho logic/mạng |
| 4 | NPC ClientOnly (gà/chó/chim sinh từ file region qua `AddClientNpc`) có `m_dwID == 0` → điều kiện `ID != 0` **tắt nội suy vĩnh viễn** với chúng | VỪA | Tách cờ hợp lệ riêng `s_InterpValid[]` — ID 0 vẫn được nội suy; chốt 64px đỡ trường hợp 2 NPC ID-0 tái dùng slot |
| 5 | Trần `PaintFps=200` là ảo (vòng bơm + `Sleep(1)` không có `timeBeginPeriod` chỉ cho ~60-64Hz) và làm lỗi #1 tới sớm 3× | NHỎ | Kẹp về **60**; thêm `timeBeginPeriod(1)`/`timeEndPeriod(1)` **chỉ khi PaintFps>30** (mặc định 30 và dàn multibox không đổi gì) |
| 6 | NPC đứng yên vẫn bị `SetPos`→scene mỗi khung vẽ (~7.680 lệnh/s vô ích ở 30fps) | NHỎ | `continue` khi `from==to` (trừ nhân vật chính — camera phải luôn được ghi) |
| 7 | Pass vừa vẽ xong vẫn `Sleep(1)` → jitter ở 60fps | NHỎ | Cờ `bPainted` — pass đã vẽ thì bỏ `Sleep(1)` |

**Các nghi vấn đã kiểm và KHÔNG phải lỗi:** animation không chạy nhanh lên (`IR_NextFrame` + `KSprControl` đều theo đồng hồ thật/tick — 2 reviewer độc lập xác nhận); login/NpcSet rỗng an toàn (`KLinkArray::GetNext` trả 0); không đường vẽ nào bypass cổng nội suy (`RepresentBegin` chỉ có 1 chỗ); `MoveObject` không tạo trùng/rò rỉ khi `m_SceneID==0`; không có chuyện "hồi sinh" NPC đã gỡ; alpha được kẹp 2 lớp; chữ ký `GetInteger` đúng; hit-test chuột lệch ≤14px trên dung sai 40px — vẫn trúng; `m_bMultiGame` luôn TRUE nên minimize không làm dừng GameLoop.

**Hai cảnh báo sai của reviewer đã bác:** (a) "chèn enum làm lệch GDCNI_*" — sai, `CoreShell.h` không nằm trong diff, enum có sẵn từ trước; (b) "Game.exe mới + CoreClient.dll cũ sẽ đứng hình" — sai, cổng vẽ gọi `UiPaint` vô điều kiện, thiếu Core mới chỉ mất nội suy (suy giảm êm), vẫn vẽ bình thường.

### Giới hạn chấp nhận (ghi nhận, không sửa đợt này)
- Hình vẽ trễ đúng **1 tick (~55ms)** so với logic — giá cố hữu của nội suy fixed-timestep; bot/WAuto không ảnh hưởng (đọc toạ độ logic).
- Tràn DWORD **có sẵn** của nhịp logic (`m_GameCounter*1000` @ ~66h) — lỗi cũ, ngoài phạm vi, không đụng.
- Vệt mờ blur tan nhanh hơn ~1,7-3,3× (đếm theo khung vẽ) — thẩm mỹ.
- Trục **z không nội suy** (`m_Height` truyền theo tick) — lúc nhảy/rơi phương đứng vẫn nhịp 18Hz trong khi x/y mượt — thẩm mỹ, để đợt sau nếu cần.
- Ô "LOOP/FPS" debug: FPS = nhịp vẽ thật, LOOP vẫn là fps logic (~18) — chỉ hiển thị.
- Heap churn `KLightBase` trong `MoveObject` (LIGHT_PROP destroy/recreate) chạy dày hơn theo nhịp vẽ — chấp nhận ở 256 NPC trần client; muốn tối ưu thì sửa `KScenePlaceC::MoveObject` cập nhật tại chỗ (việc riêng).

### Vá đợt 3+4 (15/08 chiều) — hết "giựt lùi 1 cái" khi chạy ~1 đoạn

Người dùng test thực tế báo: *"mượt hơn trước nhưng di chuyển 1 đoạn thì bị giựt lùi 1 cái"* — chu kỳ đúng bằng biên region 512×1024. Nguyên nhân gốc: camera có **hai người ghi** — tick (`KNpc::Activate` bFocus=TRUE) đẩy focus tới `pos(N)`, và bất ngờ hơn: `KSubWorld::LoadMap:1717` ghi focus về **góc region đích** mỗi lần đổi region (LoadMap chạy cả khi cùng map, từ `NpcChangeRegion` trong `ServeMove`); POSSHIFT giữ camera chờ lerp qua biên rồi ghi giá trị lùi ~10px → giật lùi.

Sửa **đơn-người-ghi** (`CoreShell.cpp`, `KNpc.cpp`, `KSubWorld.cpp`):
- `g_bPaintInterpFocus` (gán từ uParam của BREATHE): khi nội suy bật, tick + `LoadMap` (chỉ nhánh cùng-map, `bLoadNew=false`) **không ghi camera nữa** — camera do duy nhất POSSHIFT điều khiển, bám lerp mượt tuyệt đối.
- Vượt biên region giờ xảy ra tại paint → đo focus **trước/sau** `SetPos`, nếu region đổi thật thì gọi `g_ScenePlace.Breathe()` ngay trong khung đó (Fell→Preprocess trong cùng khung — không khung trắng). Đo *sau* nên miễn nhiễm mọi return-sớm của `SetFocusPosition` (kéo bản đồ Ctrl+rê, place chưa mở) — không thể Breathe lặp vô hạn.
- Phản biện độc lập xác nhận: `m_bPreprocessEvent` được `ChangeProcessArea` set (42≥25) nên Breathe-tại-paint có Preprocess thật; không có đường treo 30s (`m_bLoading` chỉ chờ ~1 region từ đĩa — hành vi có sẵn của game); đổi map an toàn (`m_RegionIndex=-1` làm POSSHIFT bỏ qua player tới khi snapshot hợp lệ); Game.exe cũ + Core mới → cờ FALSE → nguyên hành vi cũ.
- Đánh đổi còn lại (có sẵn của engine, không phải regression): mỗi lần qua biên vẫn có cú chờ nạp ~1 region từ đĩa như nguyên bản.

### Build cuối
- `Sources\Core\ClientRelease\CoreClient.dll` + `Sources\S3Client\Release\Game.exe` — **Client Release|x86**, compile + link PASS (post-build copy vào `bin\` fail vì thư mục bị gitignore — không ảnh hưởng).
- Repo trước đây **thiếu 10 cặp file nguồn UI** của S3Client (UiTeamManager2, UiTargetInfo, TrayMode, UiSkillsNew, UiGamble, UiMeridian, UiFlashMessage, SpringGame, GourdCrabFishTigerLogic, WndLine…) — đã chép từ cây gốc `E:\...\SOURCESUPDATE_KINHMACH_ONLTEST0608` vào repo để `Game.exe` build được từ D:. Cũng phải chép `Lib\debug64`, `Lib\release64`, `Lib\x64` (bị gitignore, checkout mới không có) từ cây gốc.
- Lệnh build chuẩn: solution config **`Client Release|x86`** trên `JXAll.sln`, kèm `/p:VcpkgEnableManifest=false`; **không** mở lẻ project để chọn config tay.
