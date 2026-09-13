# PHÂN TÍCH SÂU — PHƯƠNG ÁN TỐT NHẤT CHO CẢ iOS VÀ ANDROID (tiếp `PHANTICH_NHIPHAN_VNKU_CPU_GPU_1309.md`)

> **Ngày:** 13/09/2026 · **Loại:** CHỈ PHÂN TÍCH — không sửa một dòng mã, không dựng, không đẩy gì (theo yêu cầu chủ).
> **Đọc trên:** nhánh `claude/ios-android-optimization-analysis-9e9677` = `mobile-0809` + báo cáo VNKU (`72818dc9`); mọi số dòng dưới đây là
> số dòng **trong cây này**. Số đo Fold 7 lấy từ `BANGIAO_DONHIP_MOBILE_1209.md`, `PHUONGAN_FPS_NHIET_PIN_MOBILE_1109.md`; số đo iPhone lấy từ
> các commit `[IOS-GOP 12/09]` và `[DONHIP 11/09 CPU]` (trong kho **không có** tệp log iPhone nào — xem §1.2).
> Nguồn SDL đọc tại `ThirdParty/SDL3-src/SDL3-3.2.30/src` (Metal + UIKit).
> Luật giữ nguyên: bản PC không đổi một byte hành vi; mọi thứ đề xuất đều rào `JX_MOBILE` (hoặc `JX_ANDROID` / `JX_APPLE` khi chỉ một nền), có công tắc
> `config.ini`, và chứng minh bằng `ios/kiem_android_tuongduong.py` (+ `--pc`) trước khi đẩy.

---

## 0. Kết luận trong 12 dòng

1. **Hai nền đang ở hai chỗ khác nhau trên cùng một con đường.** Android (Fold 7) đã qua đợt B/C/D1/BKG/GOMNEN: cảnh yên 36 % CPU, 2,0 W; Tống Kim với
   atlas khối: 1,2–1,65 ms khoảng vẽ, sàn fps 113. iOS (iPhone 17 Pro Max) **chưa hưởng phần lớn các tối ưu đó**: 119 chỗ `JX_ANDROID` trong 12 tệp của
   `Represent3` + `S3Client/Platform` là thứ iOS không dịch (bảng §1.3), Tống Kim đo 4 331 lệnh vẽ/khung ≈ 1 lệnh/quad, luồng chính ăn ~100 % một nhân.
2. **Lỗi thật đã tìm thấy khi đọc mã (chưa ai ghi):** khối đọc **mọi** khoá `Rep3*`/`NapKhung*` từ `config.ini` (`KRepresentShell3.cpp:739–774`) nằm trong
   `#ifdef JX_ANDROID`; dòng `#ifdef JX_APPLE` mà commit `50d0ffc1` sửa ở 750–752 nằm **trong** khối đó nên là mã chết trên iOS. Hệ quả: iOS đang chạy
   `PalBuffer=1, PsBuffer=0, BindRing=0` theo giá trị khởi tạo ở dòng 226–227, **không đọc được** `Rep3PalBuffer=0` (nút tắt khẩn cấp ghi trong commit
   không có tác dụng trên iOS) và shader `PalPs` trên Metal chưa từng được chọn.
3. **Trên iOS còn ba thứ riêng nền chưa làm:** (a) `ios/Info.plist` không có `CADisableMinimumFrameDurationOnPhone` → khả năng cao trình chiếu bị Apple
   khoá 60 Hz dù `PaintFps=120` (cần kiểm bằng `[DO] fps`); (b) drawable Metal = **2868×1320 = 3,8 Mpx** (chưa có D1 — Android đã ÷3–4,4 điểm GPU);
   (c) mỗi khung vẫn **chép nguyên swapchain 15 MB** để chụp màn hình (Android đã tắt, `D3D9onGPUDev.cpp:1617–1621`).
4. **Chỗ lợi lớn nhất còn lại cho CẢ HAI nền là P1 của VNKU (tách nhịp vẽ thế giới khỏi nhịp màn hình)**, nhưng không chép nguyên 18 Hz: làm
   **thế giới vẽ vào render target + trượt theo camera nội suy + nhịp thích nghi 120 → 60 → 40 chỉ khi khung vượt ngân sách** (§3.1). Ước ở Tống Kim
   120 Hz: CPU vẽ −40…−55 %, GPU tô thế giới −50…−67 %, cần điều khiển và giao diện vẫn 120 Hz. Cảnh yên/đi bộ ít người: giữ y hệt hôm nay.
5. **P2 (cache chuỗi chữ) phải đo trước.** Trên mobile chữ **đã** gộp lô với sprite (CHUATLAS + CULLCPU), nên phần còn lại chỉ là CPU: mỗi `OutputText`
   chụp/áp state block **616 mục** hai lần (`KFont3.cpp` + `CSBGpu::Capture/Apply`) — ước 3–5 µs/lần × 150–450 lần/khung lúc đông = 0,5–2 ms/khung.
   Sửa rẻ (bỏ chụp/áp khi trạng thái chữ đã đặt) trước, cache cả chuỗi kiểu VNKU chỉ khi đo còn > 0,5 ms.
6. **P3 (nền đất ở luồng nền):** không chép cách VNKU (ghép bằng CPU) ngay. GOMNEN đã cắt 7 lượt đổi đích vẽ → 1; **chưa có số đo `[PGND]` sau GOMNEN**.
   Việc kế phải là đo lại; nếu còn gai thì bước rẻ = nạp trước tile của vùng ở luồng nạp rồi mới dựng (không còn `Rep3NapDongBo` trên luồng vẽ),
   và **hạ ngân sách 8 ms xuống theo Hz** (8 ms = gần trọn khung 120 Hz — `KScenePlaceC.cpp:1357`).
7. **P4 (ngân sách nạp co giãn):** đúng hướng, nhưng trên iOS **chưa có bộ nạp khung nền** để co giãn (toàn bộ `[VE 11/09]` là `JX_ANDROID`). Port trước, co giãn sau.
8. **P8 (60 Hz):** Android đo rõ (2,35 W so với 3,2–4,4 W). iOS có thể **đã** ở 60 (mục 3a). Chủ đã chốt "Tự động = Hz màn" → giữ, thay bằng lưới an
   toàn nhiệt/tải (A2/A3) cho cả hai nền: Android có `getThermalHeadroom`, iOS chỉ có `thermalState` 0..3 (không có headroom).
9. **Thứ tự tốt nhất cho cả hai:** đo iOS (½–1 ngày) → **đưa iOS ngang Android** bằng cách chuyển `JX_ANDROID` → `JX_MOBILE` có kiểm máy + ba việc riêng iOS
   (2–4 ngày) → **P1 thích nghi** dùng chung (4–6 ngày) → P4/P3/P2 → P5/P6/P7. Không việc nào đụng PC.
10. **Không nên chép từ VNKU:** RGBA4444, độ phân giải RT 1920, bề mặt UI bằng CPU, `log` trong đường vẽ nóng, đọc tệp nhiệt trên luồng chính, và
    **18 Hz không nội suy** (mất đi thứ bản mình hơn VNKU).
11. **Ba công tắc trong lớp ghi đè git đang TẮT dù đã đo tốt:** `Rep3AtlasKhoi=0`, `Rep3AtlasMangGpu=0` (`android/du_lieu_ghi_de/config.ini`). Bản KHOI3
    (`0ee16758`) đo 11 phút nhiệt 0, khoảng vẽ 4,16 → 1,21 ms — cần chủ xác nhận config trên `dt_v4` đang bật hay tắt trước khi đo tiếp.
12. Mọi ước lượng trong tài liệu này ghi rõ chữ **ước**; số nào không có chữ đó là số đã đo trong log/commit.

---

## 1. Hiện trạng thật của hai nền

### 1.1 Android — Fold 7 (SM8750, màn trong 2184×1968 / ngoài 2520×1080, 120 Hz), đã đo

| Cảnh | Trước đợt 11/09 | Sau BKG + PALBUF (14:50 11/09) | Sau D1 (15:25) | Sau KHOI3 (20:16–20:27, `Rep3AtlasKhoi=1`) |
|---|---|---|---|---|
| Yên trong thành, 120 Hz | CPU 53/44 %, 2,39 W, khựng bảng màu 10,8/phút | **36/31 %, 1,99 W**, bỏ trình chiếu **71 %** khung, khựng 1/28 phút | — | — |
| Tống Kim | 2 500–2 900 lệnh/khung, ghi 3,4–3,5 + nộp 1–2,3 + vẽ CPU 2,7–3 = 7,3–9 ms → 97–114 fps, 5–6 W, SEVERE sau 8–17 phút | fps trung vị 109, GPU 82 % @652 MHz, 4,7 W | fps 109 → 115, nộp 1,6 → 0,8, GPU 73 % @336 MHz, 3,35 W, **CPU thành nút thắt** (96/85 %) | đổi texture 954–1 127 → **22–23**/khung, ghi 1,7–1,8 → **0,5 ms**, nộp 1,6–1,9 → **0,4**, khoảng vẽ **1,2–1,65 ms**, sàn fps 108,8 → 113,2, khung > 8 ms 0,159 → 0,021 %, 11 phút **nhiệt 0**, 2,59 W |
| Giật còn lại | — | — | — | 70 % nằm **ngoài** lớp vẽ: `PrerenderGround` 61,7 % thời gian vẽ thế giới lúc có vùng mới, trung vị **17 ms/vùng** → GOMNEN (7 lượt → 1) **chưa có số đo sau sửa** |

Tick logic đã hết là gốc: 677 viên đạn/tick = 1,6 ms (`[DAN 11/09]`).

### 1.2 iOS — iPhone 17 Pro Max (A19 Pro, 2868×1320 điểm ảnh, 120 Hz ProMotion), đo được rất ít

| Thứ | Số | Nguồn |
|---|---|---|
| Tống Kim, trước bảng màu buffer | **khung 7200: 4 331 lệnh vẽ / 4 282 quad** = 1,01 lệnh/quad | commit `bc6158e4` |
| Cảnh thường, sau bảng màu buffer | 211 lệnh / 189 quad → 183 lệnh / 361 quad (0,51 lệnh/quad) | commit `50d0ffc1` |
| CPU tiến trình | 100–132 % **của một nhân** = 17–22 % của 6 nhân → **luồng chính ~100 % + luồng nạp** | commit `a5669ca4` (đơn vị cũ) |
| fps, ms/khung, GPU, W | **không có** trong kho | `ios/JxIosStubs.cpp` chỉ ghi `[DO] fps/cpu/ram/nhiet/pin` mỗi 10 s vào `jx_nhip.log` trên máy; chưa kéo về |
| Khung vẽ logic | 1338×616 (họ 616), `SDL_WINDOW_HIGH_PIXEL_DENSITY` | `KSdlApp.cpp:544–545` |
| Drawable Metal | = cửa sổ × `nativeScale` = **2868×1320** (3,8 Mpx; khung logic 0,82 Mpx → GPU tô gấp 4,6) | `SDL_uikitmetalview.m` `updateDrawableSize`, `SDL_gpu_metal.m:3678` |
| Nhịp | `PaintFps=-1` → theo màn = 120; `PaintVsync=1`, `PaintSmooth=2`, lưới vòng bơm 1 ms | `S3Client.cpp:586–598`, config ghi đè dùng chung |
| Khung bay | 2 (mặc định SDL Metal) | `SDL_gpu_metal.m:4513` |
| Trình chiếu | chỉ VSYNC (Metal iOS không có IMMEDIATE/MAILBOX) | `SDL_gpu_metal.m:3685–3695` |
| Chép khung mỗi khung để chụp màn hình | **CÓ** (Android đã tắt) — mỗi khung một `SDL_CopyGPUTextureToTexture` 2868×1320×4 = 15 MB | `D3D9onGPUDev.cpp:1617–1621` (nhánh `#else`) |

**Vì sao "Tống Kim fps giảm mạnh" trên iPhone** (chủ hỏi 12/09): 4 300 lệnh/khung qua `SDL_gpu_metal` (mỗi lệnh = bind vertex buffer + set texture +
draw ≈ 1–2 µs CPU) ≈ 5–8 ms **chỉ để ghi lệnh**, cộng vẽ CPU 3 ms và tick → luồng chính bão hoà. Đó đúng là trạng thái Android trước đợt C/KHOI
(2 900 lệnh → 97 fps). Bảng màu buffer chỉ giúp gộp quad khác bảng màu; ps theo đỉnh (chưa bật thật, §2), bind ring, cull CPU và atlas khối là phần
đã chứng minh trên Android và **chưa có trên iOS**.

### 1.3 Kiểm rào: những gì Android có mà iOS không dịch (đếm `grep -a`: `JX_ANDROID` 119 chỗ / 12 tệp; `JX_MOBILE` 336 / 83; `JX_APPLE` 49 / 13)

| Tính năng | Ở đâu (`JX_ANDROID`) | iOS hiện | Có chạy được trên Metal không? | Rủi ro khi đổi sang `JX_MOBILE` |
|---|---|---|---|---|
| **Đọc khoá ini** `NapKhung*`, `Rep3AtlasKe/Trang`, `Rep3PalBuffer`, `Rep3BoKhungGiong*`, `Rep3PsBuffer`, `Rep3AtlasMangGpu`, `Rep3AtlasKhoi*`, `Rep3CullCpu`, `Rep3AtlasManaged`, `Rep3BindRing`, `Rep3SwapchainLogic`, `VeGiatMs`, `NapHoiKhongDe` | `KRepresentShell3.cpp:739–774` | **không đọc gì**; dùng khởi tạo dòng 224–232: Pal 1, Ps 0, BindRing 0, Mang 0, Khoi 0 | có (chỉ là đọc ini) | không; nhưng phải khai các biến còn thiếu (`g_nJxNapKhung*`, `g_nJxBoKhungGiong*`, `g_nJxAtlasKe/Trang`, `g_nJxCullCpu`, `g_nJxAtlasManaged`, `g_nJxSwapchainLogic`…) cho Apple |
| Ps theo đỉnh (`[GOP]`) | mã đã `JX_MOBILE` (`D3D9onGPUDev.cpp:1095, 1139, 1232, 1579`) | **tắt** vì `g_nJxPsBuffer = 0` (dòng 227) | có — shader `g_Rep3GpuFSPalPsMsl` đã sinh, số buffer Metal đã đánh lại (`ios/sinh_shader_msl.py`) | thấp; bật = đổi 1 số |
| Bind ring một lần + `first_vertex` | `D3D9onGPUDev.cpp:1109–1115, 1590–1593` | không | có — Metal `drawPrimitives:vertexStart:` (`SDL_gpu_metal.m:2843`) | thấp |
| Cull tam giác trên CPU (chữ chung lô với sprite) | `D3D9onGPUDev.cpp:993–1004, 1046–1058, 1125, 1161, 1175–1189` | không → chữ (`KFont3` đặt `CULLMODE=CCW`) vỡ lô như PC trước (e) | có (thuần CPU) | thấp; đo trên Android: 1,5 tỷ tam giác, bỏ 0 |
| Atlas xếp kệ, trang 2048 | `D3D9onGPURes.cpp:58–60, 383–405, 438–476`, `D3D9onGPUi.h:54–72` | trang **1024** theo bin cao (bố cục cũ, 86 % quad vỡ lô vì texture0 trên Fold 7 09:31) | có | thấp |
| Chữ MANAGED vào atlas (CHUATLAS) | `D3D9onGPURes.cpp:115–120` | không → mỗi nhãn tên/số sát thương cắt lô | có | thấp |
| **Atlas khối** (12 texture mảng gắn chết khe 2..13) | `D3D9onGPURes.cpp:552–600`, `D3D9onGPUDev.cpp:374–380, 1020, 1556–1567`, shader `g_Rep3GpuFSPalPsKhoi` | không | **có**: Metal `texture2d_array` là gốc, 12 + 2 khe < 16 (SDL) < 31 (Apple); spirv-cross dịch được switch + `textureLod` (bài học KHOI3 áp cả hai nền) | vừa: phải sinh biến thể MSL thứ 5 và đo lại `nộp` như đã làm với Adreno |
| Bỏ khung giống (BKG) | `D3D9onGPUDev.cpp:1678–1712, 1717–1718`, `Reset` 608–610, ép trình chiếu `KSdlApp.cpp:1237–1268` (gọi `Rep3_JxEpTrinhChieu` qua `GetProcAddress` — iOS chưa đăng ký ký hiệu → không ép được) | không → cảnh yên vẫn trình chiếu 100 % khung | có: không acquire drawable = không present; Metal không có gì phải "giữ" | vừa: phải đăng ký `Rep3_JxEpTrinhChieu` trong `ios/JxIosMain.cpp` như `Rep3_NapTruoc2`; kiểm xoay màn / vào lại app |
| Nạp KHUNG ở luồng nền + ngân sách 3 ms + nạp trước 2 khung + `NapHoiKhongDe` | `TextureRes.cpp:637–655, 1497+`, `TextureResMgr.cpp:21–60, 480–520, 832–860, 1000–1060`, `TextureResMgr.h:22, 65`, `TextureRes.h:26, 163` | không → giải mã khung đồng bộ trên luồng vẽ khi đám đông tới (Android trước [VE]: paint 56–157 ms) | có (thuần CPU + tải texture chung) | vừa: nhiều mã; đã chạy ổn trên Android từ 11/09 |
| Vùng 0 từ bộ đệm cố định | `D3D9onGPUDev.cpp:950–956, 1295–1329` | dùng đường cũ (memset vào staging, staging phình 2–4 MB/trang) | có | thấp |
| D1 swapchain = khung logic | `D3D9onGPUDev.cpp:108–126, 335, 617` + vá SDL Vulkan | không (Metal cần cách khác — §3.9) | khác cơ chế | vừa |
| Không chép swapchain mỗi khung | `D3D9onGPUDev.cpp:1617–1621` | **chép mỗi khung 15 MB** | có | thấp; chụp màn hình tự bật lại như Android (`GetFrontBufferData` 781–783) |
| Mailbox, số khung bay, đo `[VE]`/`[VE-GOP]`/`[VE-GIAT]`/`[VE-BKG]`, HUD số GPU | `D3D9onGPUDev.cpp` nhiều chỗ, `KRepresentShell3.cpp:116–222, 3067, 3104–3108, 3196` | iOS **không có một dòng `[VE]` nào** trong `jx_rep3.log` | có | thấp (chỉ là đếm) |
| Nấc FPS + xin Hz màn, 30 fps ngoài thế giới, bản đồ nhịp, lưới nhiệt | `JxPerfHudAndroid.cpp:557–576, 719–747`, Java `JxDoNhip` | `ios/JxIosStubs.cpp`: **hàm rỗng** | phải viết bản Apple (§3.10) | vừa |

Ba thứ ở tầng Core (`GOMNEN`, `PHACANH`, `NENDAT` đo, `DAN`) đều đã là `JX_MOBILE` → iOS có.

### 1.4 Hai nền giống và khác nhau ở đâu (đọc từ SDL 3.2.30)

| Việc | Android (Vulkan / Adreno 830) | iOS (Metal / Apple A19 Pro) | Ý nghĩa cho phương án |
|---|---|---|---|
| Kích thước swapchain | hint `JX_SWAPCHAIN_W/H` vá vào `SDL_gpu_vulkan.c` (D1, HWC phóng) | `CAMetalLayer.drawableSize` do `SDL_uikitmetalview.m:updateDrawableSize` đặt = bounds × contentsScale, đặt lại ở `layoutSubviews` | D1 cho iOS = vá `updateDrawableSize` đọc cùng hint (tệp `ios/va_sdl3_d1_metal.py`), Core Animation phóng lên màn, **không thêm pass GPU** |
| Chờ khung | `vkWaitForFences` + acquire | chờ fence khung N−2 rồi `[layer nextDrawable]` (`SDL_gpu_metal.m:3842–3871`) | giống nhau: CPU ngủ khi GPU/màn chưa xong; 2 khung bay là mặc định cả hai |
| Tần số màn | `ANativeWindow_setFrameRate` (Java `DnXinHz`), Samsung tự hạ 120 → 60 khi game chậm | ProMotion **tự** hạ theo nhịp present; muốn > 60 Hz cần khoá plist; không có API "xin Hz" cho vòng lặp tay (chỉ `CADisplayLink.preferredFrameRateRange` — SDL không dùng display link khi ta tự bơm) | nấc "60" trên iOS = chỉ cần `PaintFps=60`; nấc 120 = phải có khoá plist |
| Nhiệt | `THERMAL_STATUS` 0..6 + `getThermalHeadroom` (0..1, 1 = sắp giảm xung) | `NSProcessInfo.thermalState` 0..3 (nominal/fair/serious/critical) + thông báo đổi trạng thái; **không có headroom** | bậc thang A2 dùng chung mã C++, đầu vào khác: Android (status, headroom), iOS (state) |
| Số GPU bận | sysfs `kgsl` (Fold 7 đọc được) | không có API công khai; **có** `MTLCommandBuffer.GPUStartTime/GPUEndTime` — SDL đã có `addCompletedHandler` ở `SDL_gpu_metal.m:4031` → vá thêm cộng dồn "GPU ms/khung" | iOS đo GPU bằng thời gian lệnh, không bằng % |
| Khe sampler mỗi tầng | 16 (SDL `SDL_sysgpu.h:29`) | 16 (SDL) — Apple cho 31 | atlas khối 12 khe dùng được cả hai |
| Đổi đích vẽ | GPU xếp ô (binning) — mỗi pass = xả ô | TBDR — như trên, thậm chí nhạy hơn | GOMNEN đúng cho cả hai; P1 thêm đúng **1** pass mỗi lần vẽ thế giới (không phải mỗi khung) |
| Đọc storage buffer mỗi điểm ảnh (bảng màu, bảng ps) | đã chạy | bộ nhớ thống nhất, rẻ | giữ |
| Sự kiện | `SDL_WaitEventTimeout` → `Android_PumpEvents(delay)` (ALooper chờ có hạn, `SDL_events.c:1665`) | không có chờ của backend → vòng lặp bơm `CFRunLoopRunInMode` 2 µs (`SDL_uikitevents.m:150–175`) rồi `SDL_DelayNS(≤ 1 ms)` (`SDL_events.c:1686–1704`) | cả hai đều ngủ thật ≤ 1 ms giữa hai lần bơm; không đáng kể ở khung 8 ms |
| Ngân sách texture | `TextureResMgr.cpp:113–160`: RAM/8 kẹp 128–512 MB, trần RAM/3 | cùng đường `JX_POSIX` → 512 MB trên 12 GB | iOS 4 GB (iPhone 11) = 512 MB vẫn qua; P7 thêm `os_proc_available_memory` |
| Chép khung để chụp | tắt (`s_nJxChepKhung=0`) | bật | tắt cho iOS (rẻ, 1 dòng) |

---

## 2. Mô hình chi phí một khung — vì sao P1 là chỗ lợi lớn nhất còn lại

Mỗi khung vẽ của bản mình (cả hai nền) = **tick logic** (18 lần/giây, 0,2–1,8 ms) + **vẽ CPU** (`RepresentBegin → End`: duyệt cây cảnh, `DrawPrimitives`,
chữ, giao diện; 1,6–3,0 ms) + **ghi lệnh** SDL-GPU (0,5–3,5 ms tuỳ gộp) + **chép** lên GPU + **nộp** + **chờ**. Chi phí mỗi giây = Hz × (vẽ CPU + ghi + GPU tô).

Đợt 11/09 trên Android đã **giảm giá mỗi khung** (D1: GPU tô ÷4,4; KHOI: ghi ÷3,5; PALBUF: hết khựng) và **không trả giá khi khung không đổi** (BKG: cảnh yên
bỏ 71 %). Cái còn lại là: **lúc đông, khung nào cũng đổi** (NPC hoạt hình 18 Hz, đạn bay, nội suy) nên BKG không cắt được gì, và cả ba số hạng đều ×120.

VNKU cắt số hạng đó bằng cách chỉ dựng thế giới 18 lần/giây (§2 báo cáo VNKU). Bản mình có thứ VNKU không có: nội suy vị trí (`GOI_PROCFRAME_POSSHIFT`,
`CoreShell.cpp:24260`) và `[Client] NhipTheGioi` (24278–24310: POSSHIFT chỉ chạy mỗi K khung) — nhưng **`UiPaint` vẫn vẽ lại thế giới mỗi khung**
(`UiGame.cpp:184 → KCoreShell::DrawGameSpace → KScenePlaceC::Paint`), nên `NhipTheGioi` chỉ giảm nhịp *đổi vị trí*, không giảm nhịp *vẽ*.

Phân tích ước trên số Fold 7 sau KHOI3 (khoảng vẽ 1,2–1,65 ms, vẽ CPU ~3 ms, thế giới ≈ 88 % đơn vị vẽ, giao diện ≈ 12 %):

| Nhịp thế giới | CPU vẽ + ghi mỗi giây (ước) | So hôm nay | GPU tô thế giới |
|---|---|---|---|
| 120 (hôm nay) | 120 × 3,9 = 468 ms | — | 120 lần |
| 60 (K=2) | 60 × 3,4 + 120 × (0,45 + 0,3 RT) = 294 ms | −37 % | −50 % |
| 40 (K=3) | 40 × 3,4 + 120 × 0,75 = 226 ms | −52 % | −67 % |
| 36 (VNKU-ish) | 36 × 3,4 + 120 × 0,75 = 212 ms | −55 % | −70 % |

Trên iOS con số còn lớn hơn vì phần "ghi" đang là 5–8 ms (chưa gộp). Nhưng **thứ tự đúng vẫn là gộp lệnh trước** (giá mỗi khung), rồi mới giảm số lần
(P1): làm P1 trước trên iOS sẽ che mất lỗi gộp và làm khó đo.

---

## 3. Phân tích sâu từng phương án — thiết kế trong mã của mình, cho cả hai nền

### 3.1 P1 — Tách nhịp vẽ thế giới khỏi nhịp màn hình *(lợi lớn nhất; chủ quyết mức)*

**Thiết kế đề nghị (P1-2c, khác VNKU ở ba chỗ):**

1. **Thế giới vẽ vào render target** `RT_thegioi` cỡ khung logic + lề (ví dụ 1436+128 × 616+128, BGRA8 ≈ 4,6 MB). Chỗ móc duy nhất: `Wnd_RenderWindows`
   (`Wnds.cpp:129–160`) — trước `pGameSpaceWnd->Paint()` gọi `SetRenderTarget(RT)` + `Clear`, sau đó trả về backbuffer và vẽ **một quad** RT lên màn;
   giao diện ba lớp + cần điều khiển + `JxKyNang_Ve`/`JxVongChon_Ve` (`UiShell.cpp:371–376`) vẽ như cũ mỗi khung. Lớp `D3D9onGPU` đã có sẵn
   `RGCMD_TARGET` (`SetRenderTarget` 797–822, `PrepareAsTarget` `D3D9onGPURes.cpp:167`) và `DrawPrimitivesOnImage` (`KRepresentShell3.cpp:2411`) đang
   dùng đúng cơ chế này cho nền đất → không phải thêm đường vẽ mới, chỉ thêm một đích vẽ.
2. **Khung không vẽ thế giới thì trượt ảnh RT theo camera nội suy** (pan): `POSSHIFT` vẫn chạy mỗi khung (rẻ: chỉ NPC đang đi + camera), lấy
   `GetFocusPosition` hiện tại trừ vị trí lúc vẽ RT lần cuối → độ lệch (dx, dy) ≤ lề → vẽ quad RT lệch (dx, dy). Nền, nhà, cây, và **tên/máu (vẽ trong
   `KNpc::Paint`, nằm trong RT)** trượt mượt 120 Hz như hôm nay; chỉ hoạt ảnh và vị trí NPC/đạn khác cập nhật ở nhịp thế giới. Vượt lề hoặc vượt biên
   vùng (`Breathe()` trong POSSHIFT, `CoreShell.cpp:24341–24350`) → ép vẽ thế giới ngay khung đó. Đây là điểm VNKU **không** làm (nó bước 55 ms cả cảnh).
3. **Nhịp thích nghi, không cố định 18:** K = 1 (mỗi khung) khi "vẽ CPU + ghi" của khung thế giới < 0,6 × chu kỳ; K = 2 khi > 0,8 × chu kỳ trong 2 s;
   K = 3 khi vẫn > 0,8 × chu kỳ; nâng lại từng nấc khi < 0,5 × chu kỳ trong 5 s. Luôn vẽ thế giới khi có tick logic mới (18 Hz: hoạt ảnh đổi khung) nếu
   K ≤ 3 → ở 120 Hz nhịp thế giới thấp nhất = 40 Hz > 18 Hz, **không bao giờ thấp hơn VNKU**. Công tắc `[Client] TheGioiRT=1`, `TheGioiNhipToiThieu=40`.

**Tương tác với thứ đã có:** BKG so danh sách lệnh — khung "chỉ UI + quad RT" giống nhau sẽ được bỏ trình chiếu nốt → đứng yên trong thành gần như
chỉ trình chiếu khi tick; `NhipTheGioi` trở nên thừa (giữ = 0). `Rep3BoKhungGiong` cần biết RT là "texture chờ tải" hay không → `bCoTai` bỏ qua RT.
Chụp màn hình (`GetFrontBufferData`) không đổi. Mã hoàn toàn phía client, rào `JX_MOBILE`, PC không thấy.

**Chi phí thêm:** mỗi lần vẽ thế giới +1 render pass (xả ô một lần: 1,6 Mpx BGRA8 store ≈ 6 MB) và mỗi khung +1 quad toàn màn (0,8–1 Mpx sample) —
nhỏ so với 1 300–2 900 quad sprite chồng nhau. Trên TBDR (Apple, Adreno) đây là dạng pass "vẽ xong rồi dùng làm texture" chuẩn; **không** đổi đích vẽ
giữa chừng như lỗi 7 lượt của nền đất.

**Mất gì (để chủ quyết):** lúc đông, vị trí NPC/đạn/hiệu ứng cập nhật 40–60 lần/giây thay vì 120; nền và giao diện vẫn 120. Cảnh yên/đi bộ không đổi
gì. Nếu chủ không chấp nhận, P1-1 (chỉ hạ `PaintFps` khi đông) là bậc lùi — nhưng đó là hạ cả UI/cần điều khiển.

**Đo nghiệm thu:** cùng bài 15 phút yên + 15 phút Tống Kim; `[VE]` thêm "khung thế giới/khung màn", K trung bình, số lần ép vẽ vì vượt lề/biên; kỳ vọng
ước Tống Kim 120 Hz: CPU luồng chính −25…−35 điểm, W −0,8…−1,2, fps p10 ≥ 115 ở cả hai nền.

**Công:** 4–6 ngày (RT + pan 2, nhịp thích nghi 1, BKG/chụp/xoay màn 1, đo 1–2). Phương án dự phòng nếu RT gặp lỗi driver: "phát lại đoạn lệnh thế giới"
(giữ lại `m_cmds`/`m_ring` phần thế giới như BKG đang giữ cả khung, `D3D9onGPUDev.cpp:1706–1708`) — chỉ tiết kiệm vẽ CPU, không tiết kiệm GPU/ghi.

### 3.2 P2 — Chữ: đo trước, sửa rẻ trước, cache chuỗi sau

Đường đi một nhãn tên trên mobile: `KNpc::PaintInfo` (`KNpc.cpp:8138, 8220, 8243, 8415, 8499…`, 1–3 `OutputText` mỗi NPC) → `KRepresentShell3::OutputText`
(2714: tra bảng phông tuyến tính, `Rep3ChuGiu`) → `KFont3::OutputText` (`KFont3.cpp:~271–276`): **`m_pStateBlockSaved->Capture()` + `m_pStateBlockDrawText->Apply()`**
→ sinh 6 đỉnh/ký tự → `CommitText` (tra cache glyph) → `ms_RenderText` (`RenderTextCmp`: 2 `SetRenderState` + `DrawPrimitiveUP`) → `Apply()` lại.
`CreateStateBlock` (`D3D9onGPUDev.cpp:892–900`) ghi **256 RS + 8 × (32 TSS + 13 SS) = 616 mục**; `CSBGpu::Capture/Apply` (`D3D9onGPURes.cpp:759–768`) duyệt
tuyến tính → mỗi nhãn ≈ 1 232 phép đặt trạng thái + mảng đỉnh trên ngăn xếp `KCS_CHAR_NUM_LIMIT × 6`. Ước 3–5 µs/nhãn; Tống Kim 150–450 nhãn/khung → **0,5–2 ms/khung**
(≈ 15–60 % của "vẽ CPU" 3 ms). Chưa ai đo riêng → bước 1 là đo (một đồng hồ quanh `OutputText`, in vào `[VE]`).

Trên mobile chữ **đã gộp lô**: bảng chữ 512×512 MANAGED vào atlas (CHUATLAS), cull ép NONE (CULLCPU), alphatest đi theo bảng ps → lợi của cache chuỗi kiểu
VNKU chỉ còn là CPU, không còn là "cắt lô". Vì thế thứ tự: (1) đo; (2) sửa rẻ: rào `JX_MOBILE` bỏ `Capture/Apply` khi trạng thái chữ đã được đặt ở lần gọi
trước trong cùng khung (cờ "đang ở trạng thái chữ", đặt lại ở `RepresentBegin` và sau mọi `DrawPrimitives`), ước cắt 60–70 % chi phí nhãn; (3) chỉ khi đo
còn > 0,5 ms/khung mới làm cache chuỗi (khoá = phông + byte TCVN3 + màu + viền → ô atlas, dọn > 300 mục / rảnh 1,5 s như VNKU). Cả hai nền dùng chung.

### 3.3 P3 — Nền đất: đo lại sau GOMNEN, rồi "nạp trước rồi mới dựng", CPU ghép chỉ khi còn gai

Hiện trạng (cả hai nền, `JX_MOBILE`): `KScenePlaceC::PrerenderGround` (1346) duyệt 9 vùng: vùng tiêu điểm **không chịu ngân sách**, 8 vùng kề chịu
`dwPgBudgetMs = 8` (1357), 1 vùng xa/khung; mỗi vùng → `KScenePlaceRegionC::PrerenderGround` (222) gom cả vùng (GOMNEN) → `DrawPrimitivesOnImage` (2411)
= **đổi đích vẽ sang ảnh 512×512 + `Rep3NapDongBo` ép nạp đồng bộ** mọi tile chưa có. Trước GOMNEN: trung vị 17 ms/vùng, 115 lần/phiên.

Ba điều rút ra:
1. **Ngân sách 8 ms là số của 60 Hz** (16,7 ms/khung). Ở 120 Hz nó bằng cả khung. Nên tính theo Hz: `ngân sách = 0,35 × chu kỳ` (2,9 ms ở 120, 5,8 ở 60), và
   vùng tiêu điểm cũng nên đi qua ngân sách **trừ khi** nó chưa từng được dựng (lần đầu vào map).
2. Phần "nạp đồng bộ" bên trong `DrawPrimitivesOnImage` chưa tách ra được khỏi phần "đổi đích vẽ" (ghi chú `[NENDAT 12/09]`). Cách chữa không cần luồng nền
   cho việc vẽ: khi vùng nạp xong (`m_bLoading` → sẵn), **đẩy danh sách tile SPR của vùng vào luồng nạp** (đã có `Rep3_NapTruoc2` / hàng `NAPNPC`) và chỉ gọi
   `PrerenderGround` khi mọi tile đã có trong cache; chưa đủ thì vẽ trực tiếp (`PaintGroundDirect`, 627) như hôm nay. Không còn `Rep3NapDongBo` trên luồng vẽ.
3. Ghép bằng CPU trên luồng nền kiểu VNKU **chỉ đáng làm nếu sau (1)+(2) `[PGND]` vẫn > 1/3 khung**: sprite của mình là chỉ số bảng màu (R8G8 trong atlas) nên
   ghép CPU phải giải bảng màu ra RGB565/BGRA rồi tải 512×512×2–4 B mỗi vùng (0,5–1 MB, rẻ); thay đổi cấu trúc lớn hơn (2), và mất tính "vẽ y hệt" của GPU.

Số phải có trước: `[PGND]` phân bố sau GOMNEN (chưa có trong kho), tỉ lệ khung có `PrerenderGround` > 3 ms ở 120 Hz.

### 3.4 P4 — Ngân sách nạp/tải co giãn theo tải thật

Của mình: `NapKhungMs=3`, `NapKhungApMs=3`, `NapKhungTruoc=2` cố định (`KRepresentShell3.cpp:740–743`), ngân sách nền 8 ms (3.3). VNKU: hàm của FPS trung
bình + độ dài hàng đợi, **giảm** ngân sách khi áp lực cao (§6 báo cáo VNKU).

Đề nghị (dùng chung hai nền, chỉ `KRepresentShell3.cpp` + `KScenePlaceC.cpp`): ngân sách khung = `max(0,5, min(4, chu kỳ − (vẽ CPU + ghi + nộp) trung bình
trượt − 1))` ms — tức là "phần dư của khung", tự về 0,5 ms khi Tống Kim và về 4 ms khi cảnh yên; nạp trước 2 → 4 khung khi hàng chờ ngắn; luồng nạp ngủ
5 → 20 ms khi fps < 40. Bảng §6.1–6.2 của VNKU là giá trị khởi đầu tốt cho ngưỡng hàng đợi (36/71/121/201).

**Điều kiện tiên quyết trên iOS:** toàn bộ bộ nạp khung nền là `JX_ANDROID` (§1.3) → port trước (giai đoạn 1), co giãn sau.

### 3.5 P5 — "Số người chơi hiển thị" N

Của mình chỉ có `LowEstPlayer` ẩn **tất cả** người chơi kể cả mình (`KNpc.cpp:9172–9180`) và `LowNpc`. Thêm nấc N (0 / 10 / 20 / 40 / tất cả), làm mới
1 lần/giây từ danh sách người chơi vùng hiện tại như VNKU (`KNpc::Activate` khi `m_nLoop % 18 == 0`), luôn giữ mình + tổ đội + mục tiêu đang chọn.
Việc này **đổi thứ nhìn thấy** → chủ quyết, đặc biệt Tống Kim (ẩn địch ảnh hưởng lối chơi; VNKU không phân biệt phe). Công 1 ngày, client-only.

### 3.6 P6 — Chống vẽ trùng thân NPC: chỉ đo

Thêm bộ đếm "số lần vẽ thân mỗi NPC mỗi khung" trong `CoreDrawGameObj` (rào `JX_MOBILE`, chỉ khi `PaintLog>0`), chạy một phiên Tống Kim. > 1 → thêm dấu
tick như VNKU (`s_NpcBodyDrawTick`). ½ ngày. Ghi chú thêm: `KIpotLeaf.cpp:116–119` (lá có con trái không vẽ lớp phi-đối-tượng) — kiểm cùng lúc.

### 3.7 P7 — Bộ nhớ texture theo mốc, và iOS jetsam

`TextureResMgr::CheckBalance` (60–112): mỗi lần gọi bỏ **một** khung/tài nguyên rảnh > 10 s, chỉ khi vượt ngân sách và fps ≥ 25 (`KRepresentShell3.cpp:3122`).
Ở 120 Hz = 120 mục/giây → thực ra không chậm; vấn đề là **không có mốc thấp** (đuổi về 80 %) nên cứ lượn quanh trần. Đề nghị: mốc cao/thấp (100 % / 80 %),
làm mới "lần dùng cuối" từ texture chạm trong khung (`m_touched`, đã có), dọn theo tuổi khi > N mục. iOS: `SetBudget` đi đường `JX_POSIX` (RAM/8 kẹp
128–512) → thêm kẹp theo `os_proc_available_memory` (đã dùng trong `ios/JxIosDoNhip.mm`) để máy 4 GB không bị giết. Ưu tiên thấp, 1 ngày.

### 3.8 P8 — 60 Hz và lưới an toàn nhiệt/tải cho cả hai nền

Android: nấc 60 = 2,35–2,46 W so với 3,2–4,4 W ở 120 (Tống Kim), 59 khung/s đều. Chủ đã chốt "Tự động = Hz màn" (`PHUONGAN … §6`) → giữ; làm **A2 + A3**:
bậc thang 120 → 90 → 60 → 45, hạ khi (Android) `status ≥ 2` hoặc `headroom ≥ 0,9`, (iOS) `thermalState ≥ 2` (serious), hoặc khi "vẽ CPU + ghi + nộp" > 0,9 × chu
kỳ trong 3 s; nâng lại sau 60–120 s. Mã C++ dùng chung (`JxNhip_*`), đầu vào từ hai lớp vỏ. iOS thêm: `NSProcessInfoThermalStateDidChangeNotification` để
không phải hỏi mỗi 0,5 s. Cần quyết riêng cho iOS: có bật 120 Hz không (3.10).

### 3.9 (mới) D1 cho Metal — GPU tô đúng khung logic trên iOS

Hiện drawable 2868×1320 (3,8 Mpx) trong khi khung logic 1338×616 (0,82 Mpx): GPU tô gấp **4,6** lần cần thiết, y như Fold 7 màn trong trước D1 (nghẽn
GPU lúc đông, 4,61 → 3,35 W sau D1). Cách làm: `ios/va_sdl3_d1_metal.py` vá `SDL_uikitmetalview.m:updateDrawableSize` đọc hint `JX_SWAPCHAIN_W/H`
(đúng tên hint Android đang dùng, `JxDatHintSwapchain` `D3D9onGPUDev.cpp:112–126` mở rào sang `JX_MOBILE`); `layer.drawableSize` nhỏ hơn bounds → Core Animation
phóng khi ghép (không thêm pass); `SDL_EVENT_WINDOW_METAL_VIEW_RESIZED` vẫn bắn. Kiểm: `[D1]` trong `jx_rep3.log` báo swapchain 1338×616; so W/nhiệt hai
bản (`Rep3SwapchainLogic=100` / `0`). Rủi ro: chữ 1× có thể mềm hơn — trên Android chủ đã chấp nhận. 1 ngày.

### 3.10 (mới) Lớp vỏ iOS: khoá 120 Hz, nấc FPS, nhiệt, đo GPU, tắt chép khung

| Việc | Cách | Vì sao |
|---|---|---|
| Xác nhận trần 60 Hz | đọc `[DO] fps` trong `jx_nhip.log` (iPhone) khi `PaintFps=120`: nếu ≤ 60 ở cảnh yên → đúng là bị khoá | `ios/Info.plist` không có `CADisableMinimumFrameDurationOnPhone`; Apple khoá ứng dụng iPhone ở 60 Hz nếu thiếu khoá này (áp cho cả `CAMetalLayer`) |
| Quyết 60 hay 120 | 60: pin/nhiệt như nấc "tiết kiệm" của Android, không cần làm gì; 120: thêm khoá plist + bậc thang 3.8 | không có khoá thì mọi tối ưu iOS đều "vô hình" về fps, chỉ thấy ở CPU %/W |
| Nấc FPS + 30 fps ngoài thế giới | viết `ios/JxIosNhip.mm` thay các stub trong `ios/JxIosStubs.cpp` (`JxNhip_DatMuc`, `JxDoNhip_KhungVe`…), dùng lại `JxDoNhip_DatNhip` của `S3Client.cpp:90`; không cần "xin Hz" (ProMotion tự theo nhịp present) | `[FPSNGOAI]` Android: −0,5–1 W lúc chờ; iOS hiện vẽ 120 ở màn đăng nhập |
| Đo GPU | vá `SDL_gpu_metal.m:4031` (`addCompletedHandler`): cộng `GPUEndTime − GPUStartTime` vào bộ đếm, xuất qua `SDL_SetHint`/hàm C → in `[GPU-MS]` mỗi 30 s | iOS không cho đọc % GPU; thời gian lệnh là số duy nhất có |
| Tắt chép swapchain | mở `s_nJxChepKhung = 0` cho `JX_MOBILE` (`D3D9onGPUDev.cpp:21–25, 1617`) | 15 MB copy mỗi khung; Android đo −0,1…−0,8 W |
| `[VE]`/`[VE-GOP]`/`[VE-GIAT]`/`[VE-BKG]` | mở các khối đếm/in sang `JX_MOBILE` (`KRepresentShell3.cpp:116–222, 3067, 3104–3108, 3196`; `D3D9onGPUDev.cpp` các `jxK.*`) | không có số thì không đo được gì ở các bước sau |

---

## 4. So sánh và xếp hạng (lợi / rủi ro / công; "đổi cảm giác?" là thứ chủ phải gật)

| # | Việc | Lợi Android | Lợi iOS | Đổi cảm giác? | Chạm mã chung? | Công | Xếp |
|---|---|---|---|---|---|---|---|
| G1 | Sửa khối đọc ini `739–774` → `JX_MOBILE` + khai biến Apple; bật `PsBuffer` thật trên iOS | 0 (không đổi) | gộp quad khác ps (Android: đổi ps 1 105 → 0/khung, −22 % µs/lệnh) | không | `KRepresentShell3.cpp` — kiểm máy `kiem_android_tuongduong.py` | ½ ngày | **1** |
| G2 | Tắt chép swapchain + mở `[VE]` cho iOS | 0 | −15 MB copy/khung; có số đo | không | `D3D9onGPUDev.cpp`, `KRepresentShell3.cpp` | ½ ngày | **1** |
| G3 | Port CULLCPU + bind ring + atlas kệ 2048 + CHUATLAS | 0 | ghi lệnh −20…−30 %, pipeline vỡ lô → 0 | không | `D3D9onGPUDev/Res.cpp` | 1 ngày + đo | **2** |
| G4 | Port atlas KHỐI (+ biến thể MSL thứ 5, `textureLod`) | 0 | đổi texture ~1 100 → ~23/khung; ghi −70 % (số Android) | không | `D3D9onGPUDev/Res.cpp`, `sinh_shader_msl.py` | 1–2 ngày + đo `nộp` | **2** |
| G5 | Port BKG (+ đăng ký `Rep3_JxEpTrinhChieu` iOS) | 0 | cảnh yên bỏ ~70 % khung (Android: −17 điểm CPU, −0,4 W) | không (pixel y hệt) | như trên + `ios/JxIosMain.cpp` | 1 ngày | **2** |
| G6 | Port nạp KHUNG nền + `NapHoiKhongDe` | 0 | hết giật 56–157 ms khi đám đông tới | không | `TextureRes*.cpp/.h` | 1–2 ngày | **2** |
| G7 | D1 Metal (3.9) | 0 | GPU tô ÷4,6; ước −0,5…−1 W lúc đông | rất nhỏ (như Android) | vá SDL iOS + 1 rào | 1 ngày | **2** |
| G8 | Vỏ iOS: nấc FPS/30 fps ngoài/nhiệt/đo GPU + quyết 120 Hz (3.10) | 0 | pin lúc chờ, lưới an toàn | không | chỉ `ios/` | 1–2 ngày | **2** |
| P1 | Thế giới vào RT + trượt theo camera + nhịp thích nghi (3.1) | Tống Kim ước CPU −25…−35 điểm, W −0,8…−1,2, giữ 115+ fps | như Android, cộng thêm vì ghi lệnh iOS đắt hơn | **có, chỉ lúc đông** (NPC/đạn 40–60 Hz) | `Wnds.cpp`, `UiShell.cpp`, `CoreShell.cpp` (POSSHIFT), `D3D9onGPUDev.cpp` | 4–6 ngày | **3** |
| P4 | Ngân sách nạp + nền đất theo phần dư của khung (3.4, 3.3.1) | bớt giật đầu map/đám đông | như Android (sau G6) | không | `KRepresentShell3.cpp`, `KScenePlaceC.cpp` | 1–2 ngày | **4** |
| P3 | Nền: nạp trước tile rồi mới dựng (3.3.2) | hết `Rep3NapDongBo` trên luồng vẽ | như Android | không | `KScenePlaceC/RegionC.cpp` | 1–2 ngày (sau khi đo `[PGND]`) | **4** |
| P2 | Chữ: đo → bỏ Capture/Apply thừa → cache chuỗi nếu cần (3.2) | ước −0,3…−1,4 ms/khung lúc đông | như Android | không | `KFont3.cpp` (chung với PC! → rào `JX_MOBILE`) | ½ + 1 (+2 nếu cache) | **4** |
| A2/A3 | Lưới nhiệt/tải hai nền (3.8) | không SEVERE sau 8–17 phút | như Android | có khi máy nóng (đúng ý) | vỏ + `JxNhip_*` | 1–2 ngày | **4** |
| P5 | N người chơi hiển thị | Tống Kim rất đông | như Android | **có** | `KNpc.cpp` client | 1 ngày | 5 (chủ quyết) |
| P6/P7 | Vẽ trùng (đo), mốc bộ nhớ | nhỏ | nhỏ + an toàn jetsam | không | Core / `TextureResMgr.cpp` | ½–1 ngày | 5 |

Công cụ chứng minh "không đổi Android/Windows" cho G1–G6: `python3 ios/kiem_android_tuongduong.py` và `--pc` (tiền xử lý rút gọn theo bộ macro từng nền,
so từng dòng). Đổi `JX_ANDROID` → `JX_MOBILE` đúng nghĩa là **0 dòng khác** ở Android và Windows theo cấu trúc, nên rủi ro chỉ nằm ở phía iOS và đo được.

---

## 5. Lộ trình "tốt nhất cho cả hai" — mỗi bước có mốc đo, bước sau chỉ bắt đầu khi bước trước có số

### Giai đoạn 0 — Đo iOS cho ra số (½–1 ngày, không đổi hành vi)
- G2 phần đếm: mở `[VE]`/`[VE-GOP]`/`[VE-GIAT]` cho `JX_MOBILE`; thêm `[GPU-MS]` (3.10); kéo `jx_nhip.log` + `jx_rep3.log` iPhone về máy Mac/PC (đường
  máy chủ tải đã có). Android: `[PGND]` sau GOMNEN (đã có bộ đếm, chỉ cần một phiên chạy `PaintLog=1`), đồng hồ `OutputText` (P2), bộ đếm vẽ trùng (P6).
- **Mốc:** bảng §1.2 điền đủ fps/ms/GPU-ms/W cho iPhone ở "yên" và "Tống Kim"; biết iPhone đang 60 hay 120.

### Giai đoạn 1 — Đưa iOS ngang Android (2–4 ngày, từng cái một bản, có công tắc)
- Thứ tự: G1 → G2 → G3 → G4 → G5 → G6 → G7 → G8. Mỗi bước: `kiem_android_tuongduong.py` (+`--pc`) đạt → dựng iOS → chơi 10 phút → đọc `[VE-GOP]`.
- **Mốc:** Tống Kim iPhone: lệnh/quad ≤ 0,5, đổi texture ≤ 50/khung, `[VE]` ghi ≤ 1 ms; cảnh yên bỏ ≥ 50 % khung; không lỗi hình (chữ, sprite, xoay màn,
  vào lại app). Config `dt_v4`: quyết `Rep3AtlasKhoi` bật/tắt cho cả hai nền cùng lúc (mục 11 §0).

### Giai đoạn 2 — P1 dùng chung (4–6 ngày)
- RT + pan + nhịp thích nghi, công tắc `TheGioiRT`; đo trên Fold 7 trước (đã có bài đo chuẩn), rồi iPhone.
- **Mốc:** Tống Kim 120 Hz cả hai máy: fps p10 ≥ 115, W giảm ≥ 0,8, không SEVERE trong 20 phút; cảnh yên/đi bộ: `[VE]` không đổi so với giai đoạn 1
  (K = 1 suốt); chủ nhìn mắt: nền trượt mượt, không "bước".

### Giai đoạn 3 — Nạp, nền, chữ (3–5 ngày)
- P4 → P3 (theo số `[PGND]`) → P2 (theo số đồng hồ chữ). **Mốc:** `[REP3-NAP]` khung nạp > 16 ms ≈ 0 ngoài 10 s đầu map; `[PGND]` > 1/3 khung ≈ 0; chữ < 0,3 ms/khung.

### Giai đoạn 4 — Lưới an toàn và tuỳ chọn (2–3 ngày)
- A2/A3 hai nền; P5 nếu chủ gật; P7. **Mốc:** 30 phút Tống Kim không SEVERE, fps không rung khi hạ nấc.

Tổng ước 12–19 ngày công, trong đó phần **dùng chung hai nền** (P1, P2, P3, P4, A2 lõi) chiếm hơn nửa — làm một lần được cả hai.

---

## 6. Quyết định (chủ uỷ quyền 13/09: "tự quyết nhưng không ảnh hưởng trải nghiệm"; sau đó: "fix một lần cho cả iOS và Android")

| # | Câu hỏi | Quyết | Vì sao không ảnh hưởng trải nghiệm | Lùi được bằng gì |
|---|---|---|---|---|
| 1 | iPhone 60 hay 120 Hz | **Giữ nguyên** (không thêm khoá plist) cho tới khi đo được `[DO] fps`; nếu đúng là đang khoá 60 thì iPhone đã ở nấc tiết kiệm pin, mọi tối ưu vẫn có ích vì luồng chính bão hoà | không đổi gì người chơi đang thấy | — |
| 2 | P1 nhịp thế giới | **Làm, nhưng chỉ khi quá tải và không bao giờ dưới 60 Hz:** thế giới vẽ vào render target cách khung (K = 2) chỉ khi màn ≥ ~105 Hz, việc/khung > 70 % chu kỳ trong 2 s **và** khung đang rơi; hết quá tải về K = 1 = đường cũ. **Bỏ ý "trượt ảnh theo camera"**: nhân vật chính bám camera nên ảnh RT trượt sẽ làm chính nhân vật rung ±4 px ở 60 Hz | khi không quá tải: đường cũ, 0 lệnh thêm; khi quá tải: thế giới 60 Hz đều (đúng bằng nấc 60 của thanh FPS mà chủ đã cho người chơi chọn) thay vì 97–114 fps rung | `[Client] TheGioiRT=0` trong `config.ini` dt_v4 + khởi động lại 8765, không cần APK |
| 3 | `Rep3AtlasKhoi` | **Bật** — config dt_v4 đang chạy đã là 1 (đọc 13/09), KHOI3 đo 11 phút nhiệt 0; đồng bộ lớp ghi đè git = 1; iOS tự tắt tới khi Mac sinh biến thể MSL | pixel y hệt (chỉ đổi cách đóng gói texture) | `Rep3AtlasKhoi=0` |
| 4 | P5 ẩn N người chơi | **Không làm** (ẩn người chơi = đổi thứ nhìn thấy) | — | — |
| 5 | Vá SDL phía iOS | **Có**, cùng cơ chế `va_sdl3_*.py`: `ios/va_sdl3_d1_metal.py` (D1: drawable = khung logic, Core Animation phóng) chạy lúc CMake configure | cùng hình ảnh đã chấp nhận trên Android (D1); lùi bằng `Rep3SwapchainLogic=0` | `Rep3SwapchainLogic=0` |
| 6 | P2 chữ | **Làm bản rẻ, pixel y hệt:** state block "trọn" chụp/áp bằng memcpy ba mảng (`CSBGpu::m_bJxDay`) thay vì 616 mục × 2; **không** làm "bỏ Capture/Apply khi đã đặt" (rủi ro rò trạng thái); cache chuỗi để sau khi đo | cùng tập giá trị trạng thái | — |
| 7 | P3 nền đất | **Đo lại `[PGND]` sau GOMNEN trước**; khi làm thì "nạp trước tile ở luồng nạp khi vùng nạp xong, dựng khi đủ tile" — không bao giờ vẽ lỗ | vùng chưa dựng vẫn vẽ trực tiếp như hôm nay | — |
| 8 | P4 ngân sách co giãn | **Sau G6**, với luật: sprite đang trên màn không bao giờ bị bỏ khung, chỉ sprite mới xuất hiện được chờ | không nháy sprite đang thấy | — |
| 9 | A2/A3 lưới nhiệt/tải | **A2 (nhiệt) giữ, A3 (tải) không cần** vì P1 đã xử lý quá tải theo cách không hạ giao diện | chỉ chạy khi máy đã nóng, lúc OS sắp giảm xung | `FpsTheoNhiet=0` |

**Đã làm 13/09 theo các quyết định trên** (chi tiết, cách thử và cách lùi: `BANGIAO_MOBILE_TOIUU_1309.md`): G1–G7 bằng `android/va_nguon_mobile_1309_a.py`
(113 dòng rào `JX_ANDROID` → `JX_MOBILE` trong 9 tệp Represent3, bỏ khối `JX_APPLE` trùng, chọn shader KHOI trên Metal khi có biến thể, P2),
P1 + móc + đăng ký iOS + config bằng `android/va_nguon_mobile_1309_b.py`, D1 Metal bằng `ios/va_sdl3_d1_metal.py`.

---

## 7. Những gì đã kiểm và độ chắc

- Mọi số dòng, tên biến, rào macro ở §1.3–§3 đọc trực tiếp từ cây này (`grep -a`, `sed`), không suy từ tài liệu cũ. Riêng "commit d sửa dòng chết" kiểm bằng
  `git show 50d0ffc1 -- Sources/Represent/Represent3/KRepresentShell3.cpp` (chỉ đổi dòng 48–49 của diff, nằm trong khối `739–774`).
- Cơ chế Metal/UIKit đọc từ nguồn SDL 3.2.30 trong `ThirdParty/SDL3-src` (drawableSize, nextDrawable, frames in flight, present mode, sampler cap,
  `vertexStart`, `addCompletedHandler`).
- Khoá plist `CADisableMinimumFrameDurationOnPhone` là hiểu biết chung về ProMotion, **chưa kiểm trên máy**; cách kiểm ghi ở 3.10 (một dòng log).
- Số iPhone chỉ có ba mẩu (lệnh/quad, CPU %); các ước lượng cho iOS đều ghi "ước" và đều **dựa trên tỉ lệ đã đo trên Android** cho cùng đoạn mã.
- Chi phí chữ (P2) là ước theo số mục state block (616) và số nhãn; chưa có đồng hồ → giai đoạn 0 đo trước.
- Không biên dịch, không chạy, không sửa một dòng mã nào trong phiên này.
