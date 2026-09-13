# MỔ NHỊ PHÂN VNKU — CÁCH GIẢM CPU/GPU MÀ KHÔNG TỤT FPS, TỐNG KIM ĐÔNG KHÔNG GIẬT

> **Ngày:** 13/09/2026 · **Loại:** CHỈ PHÂN TÍCH — không sửa một dòng mã nào (theo yêu cầu chủ).
> **Đối tượng mổ:** `C:\Users\nguye\Downloads\NHACTAI\VNKU-27-07.apk` (chỉ đọc, mọi thứ trích ra nằm trong scratchpad).
> **Đối chiếu với:** GAMEDEVNEW mobile, nhánh `mobile-0809` (đầu `26ed36ef`), đường SDL3 + D3D9onGPU.
> VNKU **không phải** USVOLAM: đường dẫn biên dịch trong DWARF là
> `/Users/administrator/Developer/JxMobiProject/KHTDMobile/`, một port khác trên cocos2d-x 4 (OpenGL ES 2).

---

## 0. Tóm tắt 6 dòng

1. **Mẹo lớn nhất:** VNKU chạy logic **và vẽ thế giới** chỉ **18 lần/giây** vào một *render texture*, còn màn hình vẫn 60 FPS
   (khung không có tick chỉ vẽ 1 hình chữ nhật + nút giao diện). Không nội suy vị trí. GAMEDEVNEW thì vẽ lại cả thế giới **mỗi khung vẽ**
   (tới 120 Hz) kèm nội suy.
2. **Chữ trên đầu** (tên, thông báo) được vẽ sẵn thành ảnh RGBA một lần rồi dùng lại như sprite; cache tự dọn. GAMEDEVNEW vẽ từng chuỗi mỗi khung,
   mỗi chuỗi đổi trạng thái vẽ riêng.
3. **Nền đất** được ghép bằng CPU trên **luồng nền** (ưu tiên SCHED_BATCH, có semaphore); luồng chính chỉ tải 1 ảnh 512×512 lên GPU.
   GAMEDEVNEW prerender nền trên luồng chính (ngân sách 8 ms, vùng dưới chân luôn vẽ ngay).
4. **Nạp/giải mã/tải texture co giãn theo FPS thật và độ dài hàng đợi**: FPS thấp thì nạp ít, luồng nền ngủ lâu; áp lực cao thì *giảm*
   thời gian tải lên mỗi khung (ưu tiên mượt hơn đủ hình). GAMEDEVNEW dùng ngưỡng cố định 3 ms.
5. **Đám đông:** tuỳ chọn "chỉ vẽ N người chơi khác" (làm mới mỗi 18 tick), chống vẽ trùng thân NPC trong cùng tick, cờ ẩn NPC thường.
6. Logic (`KSubWorldSet::MainLoop`) của VNKU **giữ nguyên bản**, không cắt NPC/đạn. Toàn bộ chỗ lợi nằm ở **vẽ + nạp tài nguyên**.

---

## 1. Nguồn và cách mổ

| Thứ | Giá trị |
|---|---|
| Gói | `com.vennguyenkyuc.jxmobi` versionCode 59, minSdk 21, target 36, `glEsVersion 0x20000`, `largeHeap`, hướng ngang |
| Thư viện | `lib/arm64-v8a/libMyGame.so` 110 MB (+ bản v7a 96 MB) |
| Trình biên dịch | NDK r27 clang 18.0.1 (`DW_AT_producer`) |
| Engine | cocos2d-x 4 (có `renderer/backend/opengl/*`), không phải 3.17 |
| **Thông tin gỡ lỗi** | **Còn nguyên DWARF** (`.debug_info` 31 MB, `.debug_line` 8,6 MB) + `.symtab` → ra được tên hàm, tên biến tĩnh, **tên tệp nguồn + số dòng** cho từng lệnh máy |
| Số tệp nguồn | 670 CU (Classes: Engine 45, GameCore 66, GameScene 12, GameUi/UiCase 119, Represent3 2, Manager 11…) |

Công cụ: `llvm-readelf / llvm-nm / llvm-objdump -l / llvm-dwarfdump` của NDK 25.2, `aapt2`, `dexdump` (build-tools 34);
3 script tự viết trong scratchpad: `vdis.py` (dịch ngược kèm dòng nguồn + chú thích chuỗi/GOT), `xref.py` (tìm nơi gọi hàm, nơi dùng chuỗi),
`scanimm.py` (tìm hàm truy cập một offset trường). Các số dòng `Tên.cpp:N` bên dưới là số dòng **trong mã nguồn VNKU** đọc từ DWARF.

---

## 2. Kiến trúc khung hình của VNKU (quan trọng nhất)

### 2.1. Ai chạy ở nhịp nào

| Việc | Nhịp | Bằng chứng |
|---|---|---|
| Màn hình (`Director`) | **60 FPS cố định** | `AppDelegate.cpp:285` `setAnimationInterval(0x3C888889 = 1/60)`; Java `Cocos2dxRenderer.onDrawFrame` ngủ bù rồi `nativeRender` (bản gốc cocos) |
| Mạng `GameScene::NetworkTick` → `KLoginLogic::Update` | 50 Hz | `GameScene.cpp:47` `schedule(..., 0.02)` |
| **`S3Client::UiPaintFrame`** = `KCoreShell::Breathe()` (MessageLoop + `KSubWorldSet::MainLoop` + `KScenePlaceC::Breathe`) → `UiPaint(0)` → `compositeSceneRenderTexture()` | **18 Hz** | `S3ClientScene.cpp:330` `schedule(UiPaintFrame, 0x3D638E39 = 1/18)`; gọi ảo slot 39 của vtable `KCoreShell` = `Breathe` (đã giải vtable) |
| `S3Client::UiPaintFrameUI` (dọn cache chữ, HUD) | 18 Hz | `S3ClientScene.cpp:331` |
| `S3Client::DrawScene` (đặt vị trí/tỉ lệ sprite RT) | 18 Hz | `S3ClientScene.cpp:332` |
| `S3Client::update` (chữ bay/thông báo động) + nút cocos, cần điều khiển | mỗi khung (60) | `scheduleUpdate` |
| Nội suy vị trí giữa hai tick | **Không có** | không có ký hiệu/chuỗi `POSSHIFT`, `PROCFRAME`, `interpolat…` nào ngoài cocos |

`UiPaint(int)` chỉ có **một** nơi gọi là `UiPaintFrame`; `KScenePlaceC::Paint` chỉ được gọi từ `UiPaint` và `KCoreShell::DrawGameSpace`.
⇒ Toàn bộ thế giới (nền, nhà, NPC, đạn, tên, máu) chỉ được dựng **18 lần/giây**. Ở 60 FPS, 2/3 số khung chỉ vẽ lại ảnh cũ.

### 2.2. Thế giới vẽ vào render texture

- `S3ClientScene.cpp:358-378`: `RenderTexture::create(visibleW, visibleH, RGBA8888)` + `Sprite::createWithTexture`, `setFlippedY`, z = −1000;
  tạo lại khi đổi cỡ màn (`onScreenResize`, dòng 667).
- Độ phân giải thiết kế `1920×1080`, chính sách `FIXED_WIDTH` (`_GLOBAL__sub_I_AppDelegate` + nhánh policy 4 trong `CCGLView.cpp:147`).
  Ví dụ màn 2520×1080 → RT ≈ 1920×823. **Không thấp hơn** khung logic của mình (D1: 1040×616 / 1436×616).
- `compositeSceneRenderTexture` (`S3ClientScene.cpp:861-936`): `beginWithClear(0,0,0,0)` → vẽ nút nền → trộn hàng đợi sprite theo thứ tự
  → hàng bóng → `end()` → `Renderer::render()` ngay. **Không có gì trong hàng đợi thì bỏ qua** (log `[RENDER-DBG] composite SKIP no sprites/shadows`).
- Sprite lấy từ **bể 55.000 `cocos2d::Sprite` dùng vòng** (chỉ số `[this+1336]` quay về 0 ở 55000), không tạo/huỷ nút mỗi khung.
- `DrawScene` (`S3ClientScene.cpp:1019-1051`) chỉ gọi `setScale` khi zoom lệch > 1e-4 và `setPosition` khi lệch > 0,5 px → không làm bẩn ma trận.

### 2.3. Giao diện KWnd

`S3ClientUi` giữ một **bề mặt CPU** (`memset` đầu khung, `blitFrameData` blit phần mềm 6 KB mã máy, `flushLocked` memcpy) rồi
`Texture2D::updateWithData` lên **một** texture → 1 lệnh vẽ. Chạy trong nhịp 18 Hz như thế giới.

---

## 3. Đám đông: NPC và người chơi

### 3.1. Chỉ vẽ N người chơi khác (tuỳ chọn người chơi)

- `KNpc.cpp:397-413` (trong `KNpc::Activate` của **chính nhân vật mình**), khi `m_nLoop % 18 == 0` (1 lần/giây):
  nếu `N = Player+7948` khác 0 và 255 → xoá danh sách, duyệt **danh sách người chơi của vùng hiện tại**, chép tối đa N chỉ số (short) vào `Player+7908`.
- `CoreDrawGameObj.cpp:94-105`: NPC loại người chơi (không phải mình) chỉ được vẽ khi `N == 255` (tất cả) hoặc có trong danh sách; `N == 0` → không vẽ ai.
- Bật/tắt qua `KCoreShell::OperationRequest`; giao diện có chuỗi `showplayernumber` (`Player_Life/Mana/Stamina/Exp::OnButtonClick` → `Switch([[showplayernumber]])`).
- Điểm yếu của VNKU: vòng kiểm gọi `cocos2d::log("show Uid: %d %d")` **trong đường vẽ nóng**.

### 3.2. Chống vẽ trùng thân NPC trong cùng tick

`CoreDrawGameObj.cpp:86-92`: hai mảng tĩnh `s_NpcBodyDrawSeen[320]`, `s_NpcBodyDrawTick[320]`; nếu NPC (chỉ số < 320) đã vẽ thân trong tick
hiện tại (`SubWorld[0].m_nLoop`) thì bỏ. Dấu hiệu cây IPOT từng làm một NPC xuất hiện ở hai lá.

### 3.3. Ẩn NPC thường

`KNpcSet::LockDrawNpcFlag` (`KNpcSet.cpp:928`, gọi từ `OperationRequest`) đặt 2 bit ở `NpcSet+61548`; `KNpc::Paint` (`KNpc.cpp:3952`) bỏ thân NPC
không phải người chơi khi `CheckDraw()` đúng. Cờ tên/máu/nội lực ở `NpcSet+61544` giống bản mình.

### 3.4. Cờ hiển thị từ máy chủ — chưa rõ nghĩa

`s2cSyncByte` loại 4 ghi `PlayerSet+2024` (`KProtocolProcess.cpp:4227`); `KNpc::Paint` truyền cờ này vào `KNpcRes::Draw` và bỏ `DrawMenuState`,
`PaintInfo` đọc 3 lần. Là giao thức riêng của máy chủ VNKU, **không chép**.

---

## 4. Chữ trên đầu: cache chuỗi thay vì vẽ từng chữ mỗi khung

- `S3Client::OutputText` → `GenerateTextCacheKey(font, chuỗi, màu, kiểu)` → `unordered_map<string, TextCacheEntry>`;
  lần đầu `ConvertRGBAtoFrameData` vẽ **cả chuỗi** thành ảnh RGBA rồi dùng lại như một sprite (font `fonts/JXFont/gbk_fs12.fnt`, cờ `text_fast`).
- Dọn: `UiPaintFrameUI` gọi `CleanupUnusedTextCache` mỗi ≥ 1 s (`S3ClientScene.cpp:1072-1076`); chỉ dọn khi **> 300 mục**, bỏ mục
  **không dùng ≥ 1,5 s** (`0x59682F00` ns), nhả texture và gỡ khỏi các sprite đang giữ (`S3ClientScene.cpp:3043-3063`).
- `OutputNotify::s_textStabilizeMap` giữ vị trí chữ thông báo khỏi rung.
- Shader nhãn cocos sửa thành **vẽ chữ + viền trong một lượt** (chú thích trong mã shader: "composite text + stroke trong MOT pass").

---

## 5. Nền đất: ghép trên luồng nền

`KScenePlaceRegionC::PaintGround` (`KScenePlaceRegionC.cpp:974-1081`):

1. Trạng thái nguyên tử `+136`: 0 rảnh, 1 đang dựng, 2 đã dựng xong (chờ tải lên).
2. Trạng thái 2 → `_UploadStagedToTexture()` (`Texture2D::initWithData` một lần), rồi vẽ cả vùng bằng **1 hình**:
   `S3Client::DrawGroundCompositeTexture(tex, x, y, 512, 512)`.
3. Chưa có texture và đang rảnh → gom tên tile (`"%s|%d"`), `SprManager::LoadGroundSprOnly`, đặt trạng thái 1, tạo **`std::thread` tách rời**:
   `pthread_setschedparam(self, SCHED_BATCH)` (dòng 1068) → chờ **semaphore đếm** `s_groundBuildSem` (dòng 44-49) → `_DoBuildGroundBuffer()`
   (ghép tile bằng CPU vào bộ đệm) → nhả semaphore.
4. Có kiểm phiên bản tile (`GetGroundSprVersion`), thử lại tối đa 28 lần.
5. Khi một vùng nạp xong, `KScenePlaceC::ARegionLoaded` gọi `PreBuildGroundSyncInLoadThread` để dựng sẵn **ngay trên luồng tải**.

⇒ Luồng chính không bao giờ ghép tile; chỉ tải lên một ảnh đã xong.

---

## 6. Nạp — giải mã — tải texture co giãn

### 6.1. Chính sách giải mã theo FPS (`SprManager::refreshThermalDecodePolicy`, `SprManager.cpp:594-676`)

Tên có chữ *Thermal* nhưng **đầu vào là FPS trung bình** (`getRealFPS_Avg`); `NotifySoCTemperatureC` là hàm rỗng.
Gọi mỗi khung từ `SprManager::update`. Kết quả ghi vào 4 biến toàn cục:

| FPS trung bình | `s_pendingDrainPerFrame` (yêu cầu đẩy vào luồng giải mã/khung) | `s_workerSleepIdleMs` | `s_schedulerAtlasBase` | `s_workerSprBurstPerLoop` |
|---|---|---|---|---|
| chưa đo (≤ 0) | 16 | 11 | 2 | 2 |
| < 28 | **6** | **20** | 1 | 1 |
| 28–38 | 10 | 16 | 2 | 1 |
| 38–48 | 14 | 12 | 2 | 2 |
| 48–56 | 20 | 8 | 3 | 2 |
| ≥ 56 | **28** | **5** | 6 | 3 |

Thưởng theo độ dài hàng đợi (chỉ khi FPS ≥ 40 hoặc chưa đo): ≥ 201 → +10; ≥ 121 → +7; ≥ 71 → min(x, 27) + 5; ≥ 36 → min(x, 25) + 3
(+1 burst ở dải 50–56 FPS). Hàng đợi yêu cầu là `MutexUniqueQueue<string, 2000>`.

### 6.2. Ngân sách tải lên GPU mỗi khung (`SprManager.cpp:808-837`)

- Áp lực = `s_uploadPressure` (đọc rồi xoá mỗi khung) > 22 **hoặc** hàng chờ > 40.
- **Áp lực nặng** (> 45 hoặc hàng chờ ≥ 76): `s_atlasLimitThisFrame = min(base, 2)`, `s_uploadBudgetThisFrameUs = 1800` (1,8 ms).
- Áp lực thường: atlas/khung = min(base + (hàng chờ > 20), 5), ngân sách 2,6 ms.
- Không áp lực: atlas/khung = base + (hàng chờ > 20), ngân sách **3,2 ms nếu FPS < 40, 4,0 ms nếu FPS ≥ 40**.
- Kẹp atlas/khung trong [2, 12]. `UploadSingleSprite` đo thời gian từng lần `initWithData` cộng vào `s_uploadUsedThisFrameUs`.

**Ý chính:** càng quá tải càng **bớt** việc trong khung; hình thiếu vài khung còn hơn khung bị giật.

### 6.3. Dọn cache

| Cơ chế | Điều kiện | Mã |
|---|---|---|
| Dọn theo tuổi | mỗi 60 khung, FPS ≥ 30: > 850 mục → bỏ mục cũ hơn **5 s**; ≥ 701 mục → **10 s** | `SprManager.cpp:734-752` (`KFastSprCache::cleanByAge`) |
| Làm mới LRU | mỗi 60 khung, gom texture đang được 4096 ô sprite dùng → cập nhật thời điểm dùng | `SprManager.cpp:774-791` |
| Theo ngân sách GPU | `KFastSprCache::s_totalGpuBytes` > **350 MiB** → đuổi theo LRU về **280 MiB** | `SprManager.cpp:983-988` (`evictByBudget`) |
| Xoá thật | gom rồi `Ref::release` một lượt, tránh trùng | `SprManager.cpp:1006-1018` |

### 6.4. Định dạng

- Trang atlas: luỹ thừa 2 của khối lớn nhất, **tối đa 2048×2048** (`SprManager.cpp:2472-2476`, log `[SPR PACK 300]`).
- Texture sprite: **RGBA4444** (`initWithData(..., PixelFormat 8, ...)`; đã giải bảng tên `getStringForFormat`: 8 = RGBA4444, 2 = RGBA8888).

---

## 7. Những thứ khác (ít quan trọng)

- HUD: mỗi 10 lần `UiPaintFrameUI` (~0,55 s) đọc `thermal_zone0/1/3` bằng `fopen/fscanf` trên luồng chính, hiện `HH:MM:SS - FPS - CPU °C`.
- Java: **không** `Surface.setFrameRate`, **không** `SurfaceHolder.setFixedSize` cho mặt GL (2 lần gọi `setFixedSize` là của VideoView).
  `AppActivity.getCpuUsage/getAppCpuUsage` có `Thread.sleep(360/500 ms)` bên trong (chỉ an toàn nếu không gọi từ luồng GL).
- Bộ dựng cocos (`Renderer::drawBatchedTriangles`, `TriangleCommandBufferManager`) là bản gốc v4, không tự sửa; gộp lệnh nhờ atlas + thứ tự.
- `KSubWorldSet::MainLoop` → `KSubWorld::Activate` → vòng `KRegion::Activate`: **y nguyên bản**, không có cắt NPC xa.
- `KProtocolProcess::NpcSleepSync` chỉ là biểu tượng "đang ngủ/treo máy" (menu state 4), không phải tối ưu.

---

## 8. Đối chiếu với GAMEDEVNEW mobile

| Hạng mục | VNKU | GAMEDEVNEW mobile hiện tại | Hệ quả |
|---|---|---|---|
| Nhịp vẽ thế giới | 18 Hz vào RT, màn 60 Hz | Tick 18 Hz (`S3Client.cpp:1525-1537`) nhưng **`UiPaint` mỗi khung vẽ** (`S3Client.cpp:1724`), PaintFps tới 120 | Cảnh đông: ở 120 Hz mình dựng thế giới ~6,7 lần nhiều hơn VNKU mỗi giây; ở 60 Hz ~3,3 lần |
| Nội suy vị trí | Không | Có: `GOI_PROCFRAME_POSSHIFT` mỗi khung (`S3Client.cpp:1717`, `CoreShell.cpp:24260`), `PaintSmooth=2`; `[Client] NhipTheGioi` (mặc định 0) chỉ giảm nhịp *cập nhật vị trí*, vẫn vẽ lại mỗi khung | Mình mượt hơn VNKU khi di chuyển, trả bằng CPU/GPU |
| Khung giống hệt | Không cần (không dựng lại) | [BKG] `JxBoKhungGiong` bỏ **GPU** khi giống, **CPU vẫn dựng cả khung** rồi mới so | Tiết kiệm của BKG không chạm vào CPU |
| Chữ tên/máu | Cache chuỗi → 1 sprite | `KFont3::OutputText` (`KFont3.cpp:271-373`): mỗi chuỗi `Capture` + `Apply` state, `SetFVF`, `SetTexture`, `CommitText`, `ms_RenderText`, rồi `Apply` lại; chỉ cache từng glyph | Mỗi tên ≥ 1 lệnh vẽ + đổi trạng thái → cắt lô; càng đông càng tốn |
| Nền đất | Ghép CPU trên luồng nền, luồng chính chỉ tải lên | Prerender trên luồng chính qua GPU render-to-texture (`KScenePlaceC.cpp:1346-1414`), ngân sách 8 ms, vùng dưới chân luôn vẽ ngay; [NENDAT 11/09] đo = 61,7 % thời gian vẽ thế giới lúc có vùng mới, trung vị 17 ms/vùng | Nguồn khung giật khi đổi vùng / chạy trong Tống Kim |
| Nạp/giải mã | Co giãn theo FPS + hàng đợi (bảng §6) | Cố định `NapKhungMs=3`, `NapKhungApMs=3`, `NapKhungTruoc=2` ([VE 11/09]) | Không tự nhường khi FPS tụt |
| Bộ nhớ texture | Mốc 350 → 280 MiB, đuổi theo lô, LRU làm mới từ thứ đang vẽ | RAM/8 kẹp 128–512 MB; `CheckBalance` chỉ chạy khi vượt, bỏ mục rảnh ≥ 10 s, **mỗi lần gọi nhả 1 món** (theo lượt rà mã 13/09) | Khi đầy dễ dồn nhả chậm |
| Giới hạn người chơi vẽ | Tuỳ chọn N / tất cả / không | `LowEstPlayer` ẩn **tất cả kể cả mình**; không có "N người" | Thiếu nấc giữa |
| Chống vẽ trùng NPC | Có (theo tick) | Chưa thấy | Cần đo xem cây IPOT của mình có vẽ trùng không |
| Lọc ngoài khung nhìn | (không soi sâu) | Theo **điểm neo** của đối tượng (`KIpotLeaf.cpp:76-80, 122-123`) | — |
| Định dạng sprite | RGBA4444 | Chỉ số bảng màu R8G8 + bảng màu trong shader | Của mình **đã tốt hơn** (cùng 2 byte/điểm, đủ màu) |
| Độ phân giải vẽ | 1920×(theo tỉ lệ) | D1: khung logic 1040×616 / 1436×616 | Của mình **đã thấp hơn** |
| FPS mặc định | Khoá 60 | "Tự động" = Hz màn (Fold 7 = 120) | Trùng đề xuất A trong `PHUONGAN_FPS_NHIET_PIN_MOBILE_1109.md` |

Số đo tham chiếu đã có (ghi chép 11/09, Fold 7, Tống Kim): bản D1 115 FPS nhưng **CPU 96/85 %** khi ~800 đạn/tick (CPU là nút thắt);
ghi + nộp lệnh SDL-GPU chiếm 55–60 % thời gian bận của luồng chính, cảnh đông 4,5–5,8 ms/khung, 2500–2900 lệnh/khung.
Đây đúng là phần việc VNKU chỉ làm 18 lần/giây.

---

## 9. Phương án rút ra (CHƯA LÀM — chờ chủ chọn)

Mọi phương án đều phải theo luật *mobile riêng, PC không đổi*: rào `JX_ANDROID`/`JX_MOBILE`, soi chéo trước khi đẩy mã dùng chung
(`KRepresentShell3`, `KFont3`, `CoreShell` là mã dùng chung với PC).

### P1 — Tách nhịp vẽ thế giới khỏi nhịp màn hình *(lợi lớn nhất, đổi cảm giác → chủ quyết)*

| Mức | Làm gì | Được | Mất |
|---|---|---|---|
| **P1-1 thích nghi (rẻ)** | Khi đông (đếm NPC + đạn trong khung nhìn, hoặc ms vẽ vượt ngân sách) tự hạ `PaintFps` hoặc nhịp vẽ thế giới 120 → 60 → 40; vắng thì trả lại | Dùng lại hạ tầng FPS/NhipTheGioi sẵn có | Giảm độ mượt đúng lúc đông |
| **P1-2 kiểu VNKU** | Lớp thế giới (`DrawGameSpace`) vẽ vào render target riêng theo tick (18 Hz) hoặc 30/36 Hz; giao diện KWnd + cần điều khiển vẽ mỗi khung, ghép RT lên | CPU/GPU phần thế giới giảm theo tỉ lệ nhịp (ước lượng thô ~3–6 lần cho phần vẽ thế giới); nút, cần điều khiển vẫn 60–120 | Mất nội suy vị trí nếu ở 18 Hz (chuyển động thế giới "bước" 55 ms như JX PC gốc và VNKU); phải tách pass trong D3D9onGPU |
| P1-2b giữa | Như P1-2 nhưng thế giới 36 Hz, **giữ nội suy** ở nhịp đó | Vẫn khá mượt, vẫn cắt được ~3 lần so với 120 | Công sức như P1-2 |

Đo nghiệm thu: log `[DONHIP]` / `[VE]` / `[WORLD]` / `[DAN]`, Fold 7 Tống Kim ≥ 10 phút: FPS trung vị + p1, CPU %, W, nhiệt, ms dựng thế giới/giây.

### P2 — Cache chuỗi chữ *(không đổi cảm giác)*

Đường mobile của `KRepresentShell3::OutputText`: khoá = (font, byte chuỗi TCVN3, màu, viền) → vẽ cả chuỗi vào atlas **một lần** → vẽ 1 quad;
dọn mỗi 1 s khi > 300 mục, bỏ mục không dùng ≥ 1,5 s (số VNKU làm điểm xuất phát). Lợi: tên/máu hàng trăm NPC không cắt lô, bỏ đổi trạng thái mỗi chuỗi.
Đo: lệnh/khung và "lý do vỡ lô" trong `[VE-GOP]` trước/sau.

### P3 — Nền đất dựng trên luồng nền *(không đổi cảm giác)*

Ghép tile SPR bằng CPU vào bộ đệm 16 bit trên luồng tải (đã có `Rep3NapNen`), semaphore giới hạn số vùng dựng song song, ưu tiên thấp;
luồng chính chỉ tải lên (1 vùng/khung, trong ngân sách tải lên). Chưa xong thì giữ ảnh cũ / vẽ trực tiếp như hiện nay. Lợi: bỏ gai `[PGND]` / [NENDAT].

### P4 — Ngân sách nạp/tải lên co giãn *(không đổi cảm giác)*

Thay `NapKhungMs` / `NapKhungApMs` / `NapKhungTruoc` cố định bằng hàm của FPS trung bình + độ dài hàng chờ (bảng §6.1–6.2 làm giá trị khởi đầu);
**giảm** ngân sách khi áp lực cao. Đo: `[VE-NAP]`, `[VE-GIAT]`.

### P5 — Tuỳ chọn "số người chơi hiển thị" *(đổi nội dung nhìn thấy → chủ quyết)*

Nấc 0 / N / tất cả, làm mới 1 lần/giây từ danh sách vùng hiện tại (rẻ), luôn vẽ mình và tổ đội. Cần chủ quyết có áp dụng cho phe địch
trong Tống Kim không (ẩn địch ảnh hưởng lối chơi). Bổ sung cho `LowNpc` / `LowEstPlayer` hiện có.

### P6 — Chống vẽ trùng thân NPC *(đo trước)*

Đếm số lần `CoreDrawGameObj` vẽ thân mỗi NPC mỗi khung trong Tống Kim; nếu > 1 thì thêm dấu tick như VNKU.

### P7 — Ngân sách bộ nhớ texture theo mốc *(không đổi cảm giác)*

Mốc cao/thấp (vượt cao → đuổi theo lô về thấp), làm mới "lần dùng cuối" từ những gì đang vẽ, dọn theo tuổi khi số mục lớn; chỉ khi FPS ≥ 30.

### P8 — Mặc định 60 Hz

VNKU khoá 60 và vẫn trơn. Củng cố đề xuất A trong `PHUONGAN_FPS_NHIET_PIN_MOBILE_1109.md` (60 Hz: 1,6–2,0 W so với 3,6–4,8 W khi đông ở 120 Hz).

### Không nên chép

- RGBA4444 (của mình R8G8 + bảng màu đã tốt hơn).
- Độ phân giải RT (D1 của mình đã thấp hơn).
- Shader nhãn cocos, bề mặt UI bằng CPU (UI của mình chiếm ít — `[VE]` 11/09: 341/2500 đơn vị).
- `cocos2d::log` trong đường vẽ nóng, đọc tệp nhiệt trên luồng chính (điểm yếu của VNKU).

**Thứ tự gợi ý** nếu chỉ xét lợi/rủi ro: P2 → P3 → P4 (không đổi cảm giác, chạm ít) → đo lại → P1 (chủ chọn mức) → P5/P6/P7.

---

## 10. Phát hiện phụ (ngoài phạm vi, chưa sửa)

`KIpotLeaf.cpp:116-119` (`KIpotLeaf_PaintNoneObjectLayer`): nhánh vẽ bản thân lá nằm sau `else if`, nên **lá có con trái không vẽ lớp phi-đối-tượng**
(tên, máu…); `KIpotLeaf_PaintObjectLayer` (dòng 70-73) thì không có `else`. Cần kiểm có phải cố ý (mã gốc) không trước khi đụng.

---

## Phụ lục A — Bảng địa chỉ (libMyGame.so arm64, VNKU-27-07)

| Hàm / biến | VA | Cỡ | Vai trò |
|---|---|---|---|
| `AppDelegate::applicationDidFinishLaunching` | 0x6e5e38 | 620 | 60 FPS, design 1920×1080 FIXED_WIDTH |
| `GameScene::onEnter` | 0x6e6684 | 376 | lịch 1/18 s, 0,02 s |
| `S3Client::init` | 0x6e7628 | 4748 | lịch UiPaintFrame/UI/DrawScene 1/18 s, tạo RT |
| `S3Client::UiPaintFrame` | 0x6e88b4 | 256 | Breathe → UiPaint → composite |
| `S3Client::compositeSceneRenderTexture` | 0x6e995c | 1412 | vẽ hàng đợi vào RT |
| `S3Client::DrawScene` | 0x6e8e50 | 412 | đặt sprite RT khi đổi |
| `S3Client::UiPaintFrameUI` | 0x6e89b4 | 1180 | dọn cache chữ 1 s, HUD |
| `S3Client::CleanupUnusedTextCache` | 0x6ea3b4 | 808 | > 300 mục, rảnh ≥ 1,5 s |
| `CoreDrawGameObj` | 0x859160 | 2148 | chống trùng, danh sách N người |
| `KNpc::Activate` | 0x83f2b4 | 2056 | dựng danh sách N người mỗi 18 tick |
| `KNpc::Paint` | 0x8489ec | 820 | `CheckDraw`, cờ `PlayerSet+2024` |
| `KScenePlaceRegionC::PaintGround` | 0x7ccc4c | 1556 | nền: tải lên / sinh luồng dựng |
| `PaintGround::$_0` thread proxy | 0x7cedac | 372 | SCHED_BATCH + semaphore + `_DoBuildGroundBuffer` |
| `KScenePlaceRegionC::PreBuildGroundSyncInLoadThread` | 0x7cd260 | 916 | dựng từ luồng tải |
| `SprManager::update` | 0x706fe8 | 1720 | ngân sách mỗi khung, dọn theo tuổi, LRU |
| `SprManager::refreshThermalDecodePolicy` | 0x706dc8 | 544 | bảng theo FPS |
| `SprManager::RemoveTextureDontUse` | 0x70675c | 1640 | 350 → 280 MiB |
| `SprManager::UploadSingleSprite` | 0x70b428 | 2156 | RGBA4444, đo µs tải lên |
| `s_atlasLimitThisFrame … s_uploadBudgetThisFrameUs` | 0xeb7248–0xeb7260 | — | ngân sách toàn cục |
| `KFastSprCache::s_totalGpuBytes` | 0xec69a0 | 8 | đếm byte GPU |

## Phụ lục B — Cách tái lập nhanh

```
llvm-readelf -S libMyGame.so            # thấy .debug_info/.debug_line/.symtab
llvm-nm -C -S --defined-only -n libMyGame.so > nm.txt
llvm-objdump -d -C -l --no-show-raw-insn --start-address=0x6e88b4 --stop-address=0x6e89b4 libMyGame.so
llvm-dwarfdump --debug-info --recurse-depth=0 libMyGame.so | findstr DW_AT_name
aapt2 dump xmltree --file AndroidManifest.xml VNKU-27-07.apk
dexdump -d classes.dex
```
Hằng số đáng nhớ: `0x3D638E39` = 1/18 s, `0x3C888889` = 1/60 s, `0x59682F00` ns = 1,5 s, `0x15E00000` = 350 MiB, `0x11800000` = 280 MiB.
