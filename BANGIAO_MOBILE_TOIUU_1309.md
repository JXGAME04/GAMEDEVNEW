# BÀN GIAO 13/09 — TỐI ƯU MỘT LẦN CHO CẢ iOS VÀ ANDROID (rào `JX_MOBILE`), KHÔNG ĐỔI TRẢI NGHIỆM

> Chủ 13/09: *"cho bạn tự quyết nhưng phải không ảnh hưởng đến trải nghiệm game của người chơi"* rồi *"hãy fix lại cho iOS và Android tối ưu
> một lần không tốn time viết riêng từng cái"*. Phân tích và quyết định: `PHANTICH_SAU_IOS_ANDROID_1309.md` (§6 = bảng quyết định).
> Nhánh `claude/ios-android-optimization-analysis-9e9677` (= `mobile-0809` + việc này). Bản PC không đổi một byte hành vi (chứng minh bằng máy, §4).

## 1. Đã làm gì (ba kịch bản, chạy lại vô hại)

| Kịch bản | Việc | Tệp đổi |
|---|---|---|
| `android/va_nguon_mobile_1309_a.py` | **113 dòng rào `JX_ANDROID` → `JX_MOBILE`** trong 9 tệp Represent3 → iOS có: BKG (bỏ khung giống), cull CPU, bind ring một lần, atlas kệ 2048 + chữ MANAGED vào atlas + atlas KHỐI, nạp KHUNG ở luồng nền (+ `NapHoiKhongDe`), vùng 0 từ bộ đệm cố định, D1 (khi có vá SDL), **tắt chép swapchain mỗi khung** (15 MB/khung trên iPhone), `[VE]`/`[VE-GOP]`/`[VE-GIAT]`/`[VE-BKG]`, và **đọc được các khoá `Rep3*`/`NapKhung*` từ `config.ini`** (trước đây khối đọc ini nằm trong `JX_ANDROID` → iOS chạy `PsBuffer=0`, không tắt khẩn được). Bỏ khối `JX_APPLE` khai trùng. CreateShaders nhánh Apple chọn biến thể MSL KHỐI **khi header có** (`g_nJxMslCoKhoi`), chưa có thì tự tắt + ghi log. **P2**: state block "trọn" (`CreateStateBlock(D3DSBT_ALL)`) chụp/áp bằng `memcpy` ba mảng thay vì 616 mục × 2 lần mỗi nhãn chữ | `D3D9onGPUDev.cpp`, `D3D9onGPURes.cpp`, `D3D9onGPUi.h`, `KRepresentShell3.cpp`, `TextureRes.cpp/.h`, `TextureResMgr.cpp/.h`, `Rep3ShadersGPU_spv.h`, `Rep3ShadersGPU_msl.h` (stub), `ios/sinh_shader_msl.py`, `ReverseTools/mobile_x64/dich_shader_gpu.py` |
| `android/va_nguon_mobile_1309_b.py` | **`[TG 13/09]` thế giới vẽ vào render target khi quá tải** (P1 đã chốt): Represent3 quyết K mỗi 2 s; Wnds.cpp hỏi qua `Rep3_JxTheGioi` (GetProcAddress, không đổi vtable); `Rep3_JxEpTrinhChieu` ép vẽ thật khi bề mặt đổi; iOS đăng ký hai ký hiệu; config ghi đè thêm khoá + `Rep3AtlasKhoi=1` (khớp dt_v4 đang chạy); `ios/CMakeLists.txt` chạy vá SDL Metal | `KRepresentShell3.h/.cpp`, `D3D9onGPUDev.cpp`, `Sources/S3Client/Ui/Elem/Wnds.cpp`, `ios/JxIosMain.cpp`, `android/du_lieu_ghi_de/config.ini`, `ios/CMakeLists.txt` |
| `ios/va_sdl3_d1_metal.py` | **D1 cho Metal**: `SDL_uikitmetalview.m updateDrawableSize` và `SDL_gpu_metal.m METAL_SetSwapchainParameters` đọc hint `JX_SWAPCHAIN_W/H` → drawable = khung logic (Core Animation phóng, không thêm pass GPU); ghi `JX_SWAPCHAIN_THAT` cho `[D1]`. Vá vào `ThirdParty/SDL3-src` (ngoài git) lúc CMake configure của iOS, idempotent | 2 tệp SDL (không nằm trong git) |

### 1.1 `[TG 13/09]` — cách chạy (KRepresentShell3.cpp, khối trước `Rep3VeDem`)

- Mỗi khung: `Wnd_RenderWindows` hỏi `Rep3_JxTheGioi(0)` → **0** vẽ như cũ · **1** vẽ thế giới vào RT rồi blit · **2** chỉ blit ảnh RT của khung trước.
- Sau `Present` (`JxTheGioiCapNhat`): **chu kỳ = max(1000/PaintFps mục tiêu, 1000/Hz màn thật)** — PaintFps do Wnds.cpp đưa sang mỗi khung
  (`JxDoNhip_LayNhip`), Hz màn từ `SDL_GetCurrentDisplayMode` (`Rep3_JxManHinhMs`, D3D9onGPUDev.cpp). Việc/khung = vẽ CPU + (chép + ghi + nộp) của các
  khung **có** vẽ thế giới. Vào K = 2 khi: 2·chu kỳ ≤ 1000/`TheGioiToiThieuHz` (tức PaintFps **và** màn đều ≥ ~117 Hz với mặc định 60), việc/khung > 70 % chu kỳ,
  **và** khoảng cách present thực > 1,12 chu kỳ (khung đang rơi). Về K = 1 khi việc/khung < 45 % chu kỳ trong 2 cửa sổ liền (4 s) hoặc màn/nấc không đủ nhanh.
  Nấc 60 trên màn 120 → chu kỳ 16,7 → không bao giờ K = 2 (thế giới không dưới 60). Bản đầu 16:00 ước chu kỳ = **min** khoảng cách present → máy ảo 60 Hz có hai
  khung cách nhau 3,5–5 ms (BKG bỏ trình chiếu) → tưởng màn 120 Hz → K = 2 = thế giới 30 Hz → chủ thấy **nháy màn hình lúc vào map và di chuyển** → đã sửa như trên.
- K = 1 và không ép thử = **đường cũ**, không tạo RT, 0 lệnh thêm. K = 2: khung chẵn vẽ thế giới vào RT (`SetRenderTarget` + `Clear`) rồi blit quad 1:1
  point-sampling (cùng phép chiếu +0,5 của `DrawBitmap16`), khung lẻ chỉ blit; khung lẻ đặt lại `m_nLeft/m_nTop` = gốc lúc vẽ RT để lớp phủ
  (vòng chọn, nút kỹ năng, chữ neo thế giới) trùng ảnh. Bề mặt đổi (`Rep3_JxEpTrinhChieu`), đổi cỡ khung, RT chưa có → luôn vẽ thật.
- Bắt đầu vẽ vào RT thất bại (hiếm) → Wnds.cpp vẽ đường cũ ngay, không bao giờ mất thế giới một khung.
- Không bao giờ dưới 60 Hz thế giới; ở màn 60 Hz (LDPlayer, iPhone nếu bị khoá 60) K luôn 1.
- Log `[TG]` mỗi kỳ 30 s trong `jx_rep3.log`: `bat / ep / K / khung: duong cu N, ve vao RT N, chi blit N / doi K N lan / chu ky man / viec/khung / cach present`; và
  `[TG] qua tai: ...` / `[TG] het qua tai: ...` mỗi lần đổi K.

### 1.2 Khoá config mới (`[Client]`, đọc lúc khởi động Represent3)

| Khoá | Mặc định | Ý nghĩa |
|---|---|---|
| `TheGioiRT` | 1 | 0 = tắt hẳn (không bao giờ dùng RT) — **nút lùi** |
| `TheGioiRTEp` | 0 | 1 = luôn vẽ qua RT mỗi khung (thử pixel y hệt), 2 = luôn cách khung (thử K=2) — chỉ để đo |
| `TheGioiToiThieuHz` | 60 | thế giới không bao giờ dưới nhịp này |

Các khoá cũ nay **iOS cũng đọc được**: `Rep3PalBuffer`, `Rep3PsBuffer`, `Rep3BindRing`, `Rep3CullCpu`, `Rep3AtlasKe`, `Rep3AtlasTrang`, `Rep3AtlasManaged`,
`Rep3AtlasKhoi(Lop)`, `Rep3BoKhungGiong(Ms)`, `Rep3SwapchainLogic`, `NapKhungNen/Ms/Truoc/ApMs`, `NapHoiKhongDe`, `VeGiatMs`, `Rep3DoVeChiTiet`.

## 2. Bản dựng

| Thứ | Giá trị |
|---|---|
| APK **phát hành** | `android/apk/jx1mobile-1309-tg-c.apk` (thư mục bị gitignore, giữ tại chỗ) — versionCode **109131615**, md5 `a6f91a5f7d5fffcd6f1316a2f20e2197`, 20 471 151 B = commit `cdff796e` + gộp `origin/main` (`1087d2d3`: NET-* của phiên mạng + [TK-NPC 12/09]); lên `D:\jx1_android_data_dt_v4\jx1mobile.apk` **16:17 13/09**, máy chủ 8765 khởi động lại (dt_v4 `config.ini` thêm `TheGioiRT/TheGioiRTEp/TheGioiToiThieuHz`, `[DoNhip] GuiLog=1` đã có sẵn). Trước đó: `-tg-b` 109131610 (cùng mã mobile, chưa gộp main, lên dt_v4 16:14), `-tg-a` 109131553 (ước chu kỳ sai, chỉ máy ảo), 109131606 (máy ảo pha B) |
| Dựng từ | worktree này qua junction `D:\GAMEDEVNEW_wt_bg`, SDL 3.2.30 đã vá `[DONHIP 12/09]` + `[D1 11/09]` (grep = 3) + `[D1 13/09 Metal]` (2 tệp Metal, Android không dịch) |
| Windows | `Represent3.vcxproj` ReleaseSDL\|x64 và Release\|Win32: **0 lỗi**; `S3Client.vcxproj` (chỉ `Ui\Elem\Wnds.cpp`) hai cấu hình: **0 lỗi** (MSBuild qua PowerShell — gọi từ Git Bash thì `/t:` bị đổi thành đường dẫn) |
| iOS | **chưa dựng được ở máy này** (cần Mac). Đã kiểm cú pháp bằng clang NDK với bộ macro iOS (`JX_MOBILE JX_APPLE JX_IOS`, không `JX_ANDROID`): 6 tệp Represent3 + Wnds.cpp = 0 lỗi |

## 3. Kết quả thử máy ảo LDPlayer (60 Hz, 1040×604, tài khoản test, nhân vật TestBot) — 15:59–16:12 13/09

| Pha | Config máy ảo | Kết quả |
|---|---|---|
| A: `TheGioiRTEp=1` (ép vẽ qua RT **mọi** khung, K = 1), `Rep3AtlasKhoi=1` | bản 109131553 | Vào thế giới bình thường, không sập; `[TG] ve vao RT 743 → 1819 khung, chi blit 0`; `[VE] 2 pass/khung` (pass RT + pass swapchain), ghi 0,07 ms, nộp 0,38 ms; `[VE-BKG] bỏ 203/1616 khung` (BKG vẫn chạy với RT); `[KHOI] khối #0/#1` cấp đúng. Hình: thế giới, chữ, thanh máu, giao diện đúng (ảnh `A4.png`). **Lỗi lộ ra:** `[TG] qua tai → K=2` rồi `het qua tai` sau 4 s dù màn 60 Hz — bộ ước chu kỳ sai (xem §1.1) → chủ thấy nháy lúc vào map / di chuyển |
| B: mặc định (`TheGioiRT=1`, `Ep=0`) | bản 109131606 (đã sửa ước chu kỳ) | 95 s trong thế giới, di chuyển tới điểm báo danh Tống Kim: `[TG] K=1`, `duong cu 1142/1785/1762`, `chu ky 16.67 ms (PaintFps 60, man 16.67)`, `min 3.5–4.4 ms` (chứng minh bộ ước cũ sai), **không** đổi K lần nào; 58 fps; hình đúng (ảnh `B4.png`) |
| C: `TheGioiRTEp=2` (ép cách khung) | **chưa chạy** — chủ đang dùng máy ảo (đăng ký Tống Kim), không khởi động lại thêm lần nữa | đường "chỉ blit" chỉ khác pha A ở ba phép gán (`m_nLeft/m_nTop`, đếm) — kiểm trên Fold 7 qua `[TG] qua tai` khi đông |

Config máy ảo (`D:\jx1_android_data\config.ini`) để lại: `Rep3AtlasKhoi=1`, `TheGioiRT=1`, `TheGioiRTEp=0`, `TheGioiToiThieuHz=60` (bản sao trước khi sửa: `%TEMP%\jxbuild\config_mayao_truoc.ini`).

## 4. Chứng minh không đổi bản PC / Android

- `python ios/kiem_android_tuongduong.py` (tiền xử lý rút gọn theo bộ macro, so từng dòng với HEAD; chạy với `PYTHONIOENCODING=latin-1` vì console cp1252 sập khi in byte cao):
  **Android**: khác đúng ở chỗ cố ý (P2: `D3D9onGPUi.h/Res.cpp/Dev.cpp`; TG: `KRepresentShell3.h/.cpp`, `Dev.cpp`, `Wnds.cpp`); 5 tệp chỉ đổi rào = **giống hệt**.
  **Windows (`--pc`)**: **mọi tệp mã giống hệt HEAD**; chỉ `Rep3ShadersGPU_msl.h` báo khác (tệp dữ liệu chỉ Apple include — báo nhầm đã ghi ở commit `4b9a75d9`).
- `check_encoding.py` từng tệp: số byte cao trước = sau, FFFD = 0 (kịch bản tự chặn khi lệch).

## 5. Việc phiên Mac (iOS) phải làm — theo thứ tự

1. `git pull` `mobile-0809`; `python3 ios/sinh_shader_msl.py` (cần `spirv-cross`) → `Rep3ShadersGPU_msl.h` có thêm `g_Rep3GpuFSPalPsKhoiMsl` + `g_nJxMslCoKhoi = 1`
   (stub hiện tại = 0 → KHỐI tự tắt, có log `[KHOI] ... -> TU TAT`). Đây là biến thể 14 sampler (t0, t1 + 12 khối) + 2 đệm lưu trữ; `SO_DEM` đã khai (0, 2).
2. CMake configure → `va_sdl3_d1_metal.py` tự chạy (in "da va ..."); dựng; cài.
3. Kiểm log trên máy: `[VE] bang mau kieu storage buffer (Rep3PalBuffer=1); ... ps theo dinh=1, bind ring mot lan=1`, `[D1] swapchain 1338x616`, `[VE-BKG] ... bo vi giong khung truoc N`,
   `[TG] the gioi RT: bat=1 ep=0 K=1`. Nhìn: chữ, sprite, xoay màn, vào lại app.
4. Kiểm trần 60 Hz: `[DO] fps` trong `jx_nhip.log` khi `PaintFps=-1` (120) — nếu ≤ 60 ở cảnh yên thì iPhone đang bị khoá 60 (thiếu `CADisableMinimumFrameDurationOnPhone`);
   quyết bật 120 chỉ khi có bậc thang nhiệt (`PHANTICH_SAU_IOS_ANDROID_1309.md` §3.10).
5. Tống Kim 10 phút: `[VE-GOP]` lệnh/quad phải ≤ 0,5, `[VE]` ghi ≤ 1 ms; nếu màn 120 Hz thật và đông, `[TG] qua tai` sẽ xuất hiện.

## 6. Cách thử trên Fold 7 (khi APK lên dt_v4) và cách lùi

1. Mở lại app để nhận bản. Vào thành: mọi thứ phải **y hệt** bản trước (đây là đường cũ, K = 1). Nhìn chữ, sprite, thanh máu.
2. Tống Kim 10–15 phút chỗ đông: `[TG] qua tai` sẽ chuyển K = 2 khi khung rơi; cảm giác cần đạt: nền/giao diện 120 Hz, NPC/đạn 60 Hz đều, **fps ổn định hơn, máy mát hơn**.
   Nếu thấy khó chịu: `TheGioiRT=0` trong `[Client]` của `config.ini` dt_v4 + khởi động lại 8765 → về đường cũ ngay, không cần APK.
3. Đọc log: `[TG]` (số khung vẽ vào RT / chỉ blit / đổi K), `[VE]` (ghi, nộp), `[MAU]` (W, nhiệt), fps trong `[DO]`/`jx_nhip.log`.

## 7. Còn lại (chưa làm, theo bảng §4 của phân tích)

P3 nền đất (đo `[PGND]` sau GOMNEN trước) · P4 ngân sách nạp co giãn (sau khi iOS có bộ nạp nền — đã có từ bản này) · A2 lưới nhiệt hai nền · iOS: nấc FPS + 30 fps ngoài
thế giới + đo GPU ms · P2 bước 2 (cache chuỗi) chỉ khi đo còn > 0,5 ms/khung.

## 8. Bẫy đã gặp trong phiên

- Khối đọc ini `KRepresentShell3.cpp:739–774` là `JX_ANDROID` → dòng `JX_APPLE` bên trong là mã chết; commit `50d0ffc1` "bật cả hai công tắc" thực ra không bật gì trên iOS.
- `ios/kiem_android_tuongduong.py` sập với `UnicodeEncodeError` khi in diff có byte cao → chạy với `PYTHONIOENCODING=latin-1`.
- Công cụ đó giữ nguyên các `#if` macro lạ (`JX_MSL_CO_KHOI`) và dòng chú thích ngoài rào → thay bằng kiểm lúc chạy (`g_nJxMslCoKhoi`) và không để dòng nào ngoài rào.
- MSBuild gọi từ Git Bash: `/t:ClCompile` bị MSYS đổi thành đường dẫn → dùng PowerShell.
- `mklink /J` từ Git Bash cũng bị đổi tham số → `New-Item -ItemType Junction` trong PowerShell.
- `KFont3` chụp/áp state block do `CreateStateBlock`/`BeginStateBlock` ở `RestoreDeviceObjects` — P2 chỉ tăng tốc khối **trọn**; khối ghi từng phần vẫn đi đường cũ (đúng).
