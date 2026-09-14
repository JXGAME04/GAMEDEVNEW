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

## 9. Đợt 17:00 13/09 — gộp WAuto hoàn thiện + sửa sập KHỐI + phân tích log Fold 7 (bản 109131648)

Chủ báo *"có một phiên đã fix WAuto nhưng tôi chưa thấy cập nhập lên bản mới hiện tại trên điện thoại và giả lập, hình như đã mất một việc đã làm"*.

| Việc | Kết quả |
|---|---|
| Tìm việc "mất" | Không mất: toàn bộ hoàn thiện WAuto 12–13/09 (`BANGIAO_WAUTO_MOBILE_HOANTHIEN_1209.md`) nằm **chưa commit** ở worktree `D:\GAMEDEVNEW\.claude\worktrees\kind-bassi-b0f319` (`D:\wt9`, nhánh `claude/wauto-mobile-analysis-b7df3f` @ `bc6158e4`, 33 sửa + 29 mới, sửa cuối 14:07 13/09). Bản 109131615 dựng từ `mobile-0809` nên không có; máy ảo còn thấy "mất" vì tôi đã cài đè bản của tôi lên bản của phiên đó |
| Gộp | Chép 61 tệp (patch `git diff` + 28 tệp mới qua `git diff --no-index`), so `cmp` từng byte với `D:\wt9` = giống hệt (trừ `CoreShell.cpp` có thêm hunk của [TK-NPC 12/09] và origin/main); **worktree kind-bassi không bị đụng**. Commit `28a100a0` |
| Lỗi tìm thêm | Phiên WAuto ghi §6.2: `Rep3AtlasKhoi=1` + thẻ Cơ bản (chữ đổi 500 ms) → SIGSEGV tại `SDL_BindGPUFragmentSamplers(pass, 2, tk, 12)`. Gốc: `CAtlasMgrGpu::Free/JxFreeKe` trang rỗng thứ hai → `DeferRelease(pPage->m_pTex)` = huỷ cả **texture khối dùng chung** (KHỐI bắt buộc AtlasMang=0 nên không đi nhánh `JxTraLop`); `ReleaseAll` cũng huỷ khối hai lần. Sửa: `JxTraLopKhoi` trả lớp về khối, giữ texture; `ReleaseAll` bỏ trang trong khối. Chỉ `JX_MOBILE`. Kịch bản `android/va_nguon_mobile_1309_c.py`, commit `0ac371b2` |
| Chứng minh PC | `kiem_android_tuongduong.py --pc deb88a95`: Windows chỉ đổi ở `S3Client.cpp` + `KDebug.h/.cpp` (vá dựng Windows bị [BAOMAT 12/09] làm gãy — của phiên WAuto, cố ý cho cả hai bản), `KSOServer.cpp` (origin/main), `UiWAutoDsach.cpp/.h` (không trong vcxproj nào). MSBuild `S3Client.cpp` + `KDebug.cpp` ở Release\|x64 và ReleaseSDL\|x64: **4/4 exit 0, 0 lỗi** |
| Dựng | gộp `origin/main` `f1dfce43` (`91c7c20b`) → `gradlew assembleDebug` 38 s, `android/apk/jx1mobile-1309-wauto-khoi.apk` = **109131648**, md5 `23da03208348d3864fd3f0a633dfae84`, 20 603 311 B; `libmain.so` cả hai ABI có `KUiWAutoDsach`, `libRepresent3.so` có `JxTraLopKhoi` |
| Máy ảo | cài 16:51 (md5 base.apk khớp), `Rep3AtlasKhoi=1`, nhân vật vào **Tống Kim đông** với auto bật ngay từ đầu; khung Auto mở được, nút Đóng/? có ảnh, hộp chọn/ô tích vẽ đúng; **8 khối atlas (512 MB) cấp ra, không sập** suốt trận (kiểm `logcat` + `jx_crash.log` nhiều lần tới 17:06) |
| Log Fold 7 | `PHANTICH_LOG_FOLD7_1309.md` (chủ: *"bạn kéo log của điện thoại về phân tích"*): 60 fps giữ (TB 58–59), [TG] K=2 chạy thật 3 lần lúc màn 120 Hz (2 164 RT + 2 161 blit, không lỗi), GPU 61–63 %, 2,1–2,3 W, không sập; nút thắt = CPU game (chữ vẽ mới 600 dòng/s, `[PGND]` 18 ms/lần); ảnh null = hiệu ứng nổ chiêu Thiên Nhẫn thiếu 5 khung (việc pak) |
| Bẫy bố cục (máy ảo 16:9) | hàng nút NHÓM của khung WAuto (y 57–97) bị thanh icon trên của game (Nhân vật / băng Tống-Kim / Hảo hữu / Cài đặt / Chiến Lệnh) đè và ăn chạm trước: chạm "Hoạt động" của WAuto → mở cửa sổ Chiến Lệnh. Fold 7 (1040×936) khung Auto nằm giữa nên không đè. Chưa sửa — việc của phiên giao diện (giấu thanh icon khi khung Auto mở, như đã giấu cụm kỹ năng / icon NPC) |
| Thẻ Cơ bản (máy ảo) | 17:20–17:22 mở thẻ Cơ bản (7 ô số sống đổi 500 ms) với `Rep3AtlasKhoi=1` — đúng tình huống sập của phiên WAuto: **không sập**, khối cấp thêm 2 (tổng 10 dòng [KHOI] trong phiên), `jx_crash.log` không có |
| Giao bản | 17:23 13/09: `jx1mobile.apk` = 109131648 + 24 tệp WAuto (16 `uiwauto_tab*.ini` + `uiwauto.ini` sinh lại, `uiwauto_dsach.ini`, `uiwauto_ghichu.ini`, 6 `.spr`) lên `D:\jx1_android_data_dt_v4`, khởi động lại 8765 (manifest + apk.txt sinh lại). Lùi: chép `android/apk/jx1mobile-1309-tg-c.apk` (109131615) đè lại + khởi động lại 8765 (dữ liệu WAuto mới không hại bản cũ: bản cũ không đọc `uiwauto_dsach/ghichu`, các tab ini cũ ở git `deb88a95`) |
| Cập nhật PC → mobile (17:50, chủ: *"có thêm cập nhập hệ thống của bản PC bạn xem cập nhập vào mã nguồn mobile"*) | **Mã:** gộp `origin/main` `7b3d744e` (3 commit NET-CACHE/NET-XA/NET-NHAN 16:54, chỉ Bishop/GameServer/Heaven) → `5f844328`, không đụng mã client nên **không cần APK mới**. **Dữ liệu:** quét cây PC `E:\SourceTuanLe\...in\client` (chỉ đọc) tìm tệp mới hơn lúc đóng pak điện thoại (10/09 07:54): 15 `maps/*.fp` (trang trí 10–13/09), 4 bảng `settings` (goods, buysell, item/magicscript, NpcRes/npc_res_kind_file_name), 1 `spr/item/haohuazhanl.spr`; so từng tệp với bản rời trên dt_v4 và mục trong 13 pak theo uid (`pakdump.entries/read_entry` + `dong_goi.uid_cua`) → **7 tệp** (11,7 MB) điện thoại chưa có/đã cũ → chép RỜI (chữ thường) vào dt_v4 + máy ảo (`KFile::Open`/`KImageRes` mở tệp rời trước, pak sau), khởi động lại 8765 → manifest có 7 dòng mới. Không phải đóng lại pak |

## 10. Đợt 19:20 13/09 — P3 nạp trước nền đất + ngân sách cache (bản 109131908)

Chủ: *"oke tiếp tục và tôi sẽ test trên iOS luôn"*. Làm theo thứ tự đề nghị: P3 trước.

| Việc | Kết quả |
|---|---|
| Đo trước (`android/va_nguon_mobile_1309_d.py`, `[PGND-V]` chỉ log) | một vùng ~66–76 ô ghép mất 11–32 ms, trong đó **GetImage 95–99 %** (PrepareFrameData đồng bộ: rút khung + giải mã + tạo texture từng ô; chỉ 1–4 ô phải nạp tệp), RIO ghi lệnh 0–0,5 ms, đổi đích vẽ 0,2–0,7 ms. Điện thoại 18:42–18:54: **706 lần `[PGND]` / 12 phút = 59 lần/phút, TB 16,9 ms** (mỗi lần một khung rơi) |
| Sửa `[NENTRUOC 13/09]` (`android/va_nguon_mobile_1309_e.py`, commit `7d6dfc50`) | vùng KỀ BÊN / XA: trước khi ghép, Core gọi `Rep3_NenTruocKhung(ảnh, khung)` (qua GetProcAddress, như `Rep3NapTruocAnh`) cho từng ô → Represent3 giao luồng nền chuẩn bị khung (bộ `[VE-NAP]` sẵn có, nguồn 3); còn ô "đang chuẩn bị" thì hoãn sang khung sau; tất cả sẵn (hoặc quá 1,5 s) thì ghép — lúc đó GetImage chỉ tìm. Vùng CHỨA người chơi giữ nguyên (ghép ngay). Tệp chờ tệp: `TextureResMgr::m_jxNenCho` xử lý đầu khung (`JxNenTruocXuLy`), quá 3 s bỏ |
| Sửa `[CACHE 13/09]` | Fold 7: cache 508 MB nhưng ngân sách **393 MB** vì `CapBudgetByVram` kẹp theo `GetAvailableTextureMem` của SDL_GPU = **1024 MB giả** → `CheckBalance` mỗi 24 ms bỏ một khung ("bỏ 101 632"/50 phút), giải mã lại 20 khung/s, tải GPU 4,6 MB/s. Nay SDL_GPU không kẹp theo VRAM, `PressureByVram` bỏ qua, trần RAM/8 512 → 768 (máy 4–5 GB vẫn 512; RAM/3 vẫn kẹp) |
| Đo sau (máy ảo, auto Dã Tẩu đi 3 phút, `[VE-NAP]` giao 134–700 khung/30 s xong 100 %) | `[PGND-V]` ≥ 4 ms: **0 dòng** ngoài vùng tiêu điểm lúc vào map (25 ms, theo thiết kế); `[PGND]` ≥ 15 ms: 1 (vào map). Nền vẽ bình thường khi đi, không sập |
| Chứng minh | `kiem_android_tuongduong.py --pc HEAD`: **WINDOWS biên dịch y hệt**; iOS: đăng ký `Rep3_NenTruocKhung` trong `ios/JxIosMain.cpp` |
| Giao | 19:20: `jx1mobile.apk` = **109131908** (md5 `eec40e37…`), 8765 khởi động lại; lùi = chép `android/apk/jx1mobile-1309-wauto-khoi.apk` (109131648) đè + khởi động lại 8765 |
| Bẫy | (1) biến toàn cục của Represent3 KHÔNG dùng được từ Core (`libCoreClient.so` không liên kết `libRepresent3.so`: undefined symbol lúc link) → log trong Represent3 hoặc gọi qua GetProcAddress; (2) `kiem_android_tuongduong.py` chỉ hiểu `#ifdef X`, không hiểu `#if defined(X) && !defined(Y)` — rào mới dùng dạng đơn; mọi dấu ngoặc/dòng trống thêm phải nằm trong rào |

Còn lại theo thứ tự: **P2 cache chữ** (556 000 dòng chữ vẽ mới / 16 phút Tống Kim), đo lại khối atlas sau [KHOI]+[CACHE] trên Fold 7, P4.

### 10.1. Log Fold 7 19:33–20:06 (bản 109131908) và sửa tiếp — bản 109132016 (20:24)

Chủ: *"Lấy log test"*. Gói `SM-F966U1_20260913_193302` (34 phút, Tống Kim đông NPC 100–140/tick):

| Điều thấy | Số |
|---|---|
| `[PGND]` vẫn nhiều | **1156 lần, TB 17 ms** (34 lần/phút; trước P3 59 lần/phút) — nhưng `ve len anh` chỉ **0,6 ms** và **1002/1156** lần nằm ở nhánh XA: 17 ms KHÔNG còn ở ghép |
| Gốc thật | `ClearImageData` → `LockData`+memset+`UnLockData` → `UpdateTexture` → `UpdateSurface` **khoá ảnh đích** → `CTexGpu::LockRect` thấy render target đã vẽ (`m_bGpuNewer`) → `ReadbackTexture` = `SubmitFrame` + đọc GPU về CPU **đồng bộ**. Máy ảo đọc ngược rẻ nên không lộ; Fold 7 ~17 ms/lần |
| CPU tăng | lõi 0 1,9–2,1 GHz, 3,2–3,6 W trong Tống Kim (phiên 16:41: ~1 GHz, 2,1 W): vòng hỏi `JxNenTruoc` gọi 66 ô × mỗi khung × mỗi vùng đang chờ |
| Cache | 495–512 MB / ngân sách 768, hết kẹp giả; `nạp 5886, bỏ 58230` = nhịp dọn LRU >10 s của bản 2.0 (1 mục/khung), không phải vượt ngân sách |
| Khác | `[TG]` K=1 suốt (60 Hz); 10 khối atlas = 640 MB; `cho lenh+swapchain` TB 4–11 ms (chờ GPU/vsync) — theo dõi tiếp; fps TB 57,5, p10 55; không sập |

Sửa (`android/va_nguon_mobile_1309_f.py`, commit `b22bf5eb`): (1) `[XOANEN 13/09]` ảnh nền vùng `_*PlaceGround*_` xoá **trên GPU** (SetRenderTarget + Clear + trả lại), không khoá CPU; đo `[PGND-X]` ≥ 4 ms cho cả hai đường; (2) lớp giả lập: Clear rồi đổi đích vẽ mà không có lệnh vẽ trước đây **bị bỏ** (`bPendingClear = false` ở `RGCMD_TARGET`) → nay mở một pass rỗng `load_op CLEAR`; (3) vòng hỏi chỉ hỏi lại ô còn chờ, tối đa mỗi 32 ms; (4) tệp ô nền vào hàng TRƯỚC của luồng nền. Máy ảo đi qua nhiều vùng 6 phút: `[PGND]` ≥ 15 ms = 1 (vào map), `[PGND-X]` 0, không sập. Windows y hệt (bộ kiểm ĐẠT). Bản **109132016** lên dt_v4 20:24; lùi = `android/apk/jx1mobile-1309-nentruoc.apk` (109131908) hoặc `-wauto-khoi.apk` (109131648).

### 10.2. 21:55–22:03 — "Tải tệp cập nhật bị lỗi" (không phải do bản dựng)

Phiên khác (`game-3d-data-analysis`, việc LIA/ZOOM/CAMERA) ghi `dt_v4\config.ini` 21:55 và thêm `settings\camera_mobile.ini` 21:56 **sau** khi
manifest sinh lúc 21:53 → md5 config.ini lệch → app báo lỗi. Phiên đó cũng chạy máy chủ 8765 riêng (PID 46564 từ 21:08) song song máy chủ của
tôi (Windows cho hai tiến trình cùng listen vì `allow_reuse_address`). Xử lý 22:03: sinh lại manifest tại chỗ (`--chi-manifest`, không restart),
tắt máy chủ trùng của tôi, giữ 46564, nhắn phiên kia. Mã LIA/ZOOM/CAMERA (`066d0939`, `c83048d9`) chưa vào `mobile-0809` → các khoá config
đó hiện chưa có tác dụng trong bản 109132150. Bản đo `[PGND-R]` 109132150 vẫn là bản trên dt_v4.

### 10.3. 22:13 "quay một lần có màn đen" (bản 109132150) → sửa, bản 109132221 (22:24)

Gốc: P3 (`[NENTRUOC 13/09]`) hoãn ghép cả 8 vùng **kề bên** (đang trên màn hình) cho tới khi luồng nền chuẩn bị xong khung; lúc vào/quay lại
map luồng nền đang ngập (`[VE-GIAT]` nạp 322 tệp spr) nên nền quanh nhân vật đen tới 1,5 s (`[PGND]` lúc vào map: "ke ben 0.0/0"). Trước P3
8 vùng đó ghép ngay (17 ms/vùng, vài khung). Sửa (`android/va_nguon_mobile_1309_h.py`): vùng kề bên ghép ngay như cũ, chỉ xin nạp trước;
chỉ vùng XA mới hoãn (đúng chỗ hưởng lợi của P3). Kèm `[XOANEN 13/09 b]`: `ClearImageData` báo "đích sắp Clear" → `CTexGpu` không giữ bản CPU
→ `PrepareAsTarget/LockRect` không đọc ngược GPU (cú 156 ms xoá #38 lúc vào map = SubmitFrame chờ GPU đang ngập tải lên).
Bản 109132221 = merge `mobile-0809` b18d9965 của phiên LIA/ZOOM/CAMERA + sửa này; máy ảo: vào map nền đủ, `ke ben` ghép ngay, không sập.
Giao 22:24 bằng `--chi-manifest` trên máy chủ 52072 của phiên kia (không khởi động lại). Windows y hệt trừ 1 dòng `#include JxLiaCanh.h`
chưa rào của phiên kia trong `GameSpaceChangedNotify.cpp` (đã báo họ).

### 10.4. 00:33 14/09 — P4 tải lên GPU `[TAI 14/09]` (bản 109140033, sẽ gộp camera lắc của phiên kia)

Chủ 23:40 13/09: *"Bây giờ bạn làm xong 40 % còn lại rồi tôi test luôn một lần - ưu tiên không mất trải nghiệm game thủ - mà mọi hiệu năng đều tốt"*.

**Số đo gốc** (log Fold 7 13/09, `[VE-GIAT]`): `chep: bang mau 0/0.0, tex map+chep 3.0, lenh tai 14.7, zero 1/116.8, ring 0.2, xfer 8192` → một trang
atlas 8 MB tô 0 mất **116,8 ms ngay trong 4 lệnh `SDL_UploadToGPUTexture`** (memcpy 8 MB vào transfer buffer chỉ 3,0 ms); tải ảnh 2–3 MB = 42–68 ms.
SDL Vulkan trong lệnh đó chỉ ghi `vkCmdCopyBufferToImage` + 2 barrier (không có việc theo byte) → việc theo byte nằm trong **driver Adreno**
(~20 ms/MB, nhiều khả năng xếp ô ảnh trên CPU). Đây là phần lớn 6 khung giật/phút còn lại sau P3: 117–172 ms mỗi lần cấp trang/lớp atlas mới,
40 × 20 ms tải 1 MB ảnh nền vùng lúc vào map, 42–68 ms khi vẽ khung chiêu to lần đầu.

**Sửa** (`android/va_nguon_mobile_1409_i.py`, commit `dbebc13f`, chỉ `JX_MOBILE`, `kiem_android_tuongduong --pc cbc4b5cb`: ĐẠT):

| Việc | Cách | Đo |
|---|---|---|
| a. Trang atlas mới = 0 | chép GPU→GPU (`SDL_CopyGPUTextureToTexture`) từ **dải nguồn 0** (w×256 mỗi (định dạng, loại texture), tải 0 một lần qua `m_jxZeroUploads`) thay vì tải 8 MB từ bộ đệm; `QueueZeroUpload` thêm `fmt`+`eLoai` (từ `NewPage`), `m_jxZeroCopy` chép trong cùng copy pass **sau** khối vùng 0 và **trước** nội dung texture; `bCoTai` (BKG) tính cả chép 0 | `[VE]`/`[VE-GIAT]` mục `zero n/ms` (giờ = số lần chép), `[VE-TAI] … trang atlas moi to 0 bang chep GPU: n trang x ms`, dòng `[TAI] dai nguon 0 …` lúc tạo dải |
| b. Khung nạp trước tải dần | khung luồng nền giao với `nNguon ≠ 0` (chiêu / NPC / nền đất; `JxKhungXong` thêm `nNguon`) → `Rep3Gpu_TaiTruoc` đưa vào hàng `CDevGpu::m_jxTaiTruoc`; `RepresentBegin` gọi `Rep3Gpu_TaiTruocChay` tải lên GPU **từng dải** theo `[Client] NapKhungKB` (mặc định 128 KB/khung, 0 = tắt) **trước** khi khung được vẽ (`CTexGpu::JxTaiTruoc`: texture ảo = xin ô + tải cả ô; texture riêng = `NewVersion` + tải dải từ đầu `m_rcDirty`); lúc vẽ `PrepareForBind` chỉ tải nốt phần dư (đường cũ). Khung đang vẽ cần (`nNguon 0`) y như cũ. `~CTexGpu` rút khỏi hàng | `[VE-TAI] vao hang / xong / KB / luot / ms / hang cho max` |
| c. Ảnh nền vùng sắp Clear | `PrepareAsTarget` (đích có `m_bJxKhongGiuCpu` từ XOANEN b) **không** tải bản CPU 1 MB, `free` luôn | `[VE-GIAT]` lúc vào map: `tai N tex KB` giảm ~40 MB |

Giới hạn thật thà: (b) chỉ giúp khung ≥ 1 của chiêu (khung 0 cần ngay vẫn tải lúc vẽ); tổng CPU tải không đổi (20 ms/MB), chỉ dàn ra
128 KB ≈ 2,5 ms/khung. Nếu log Fold 7 cho thấy chép GPU→GPU cũng chậm (không nên — hai ảnh đều ở bộ nhớ thiết bị): dự phòng = tô 0 bằng render
pass `LOAD_OP_CLEAR` (cần `COLOR_TARGET` trên khối) hoặc tải qua storage buffer + shader như `[PALBUF 11/09]` (tải buffer→buffer đã chứng minh
nhanh). P2 cache chữ vẫn để lại: `[PDET]` 13/09 cho thấy chữ chỉ vài phần mười ms/khung, không có số đo biện minh.

**Máy ảo 00:40–00:46 (bản gộp 109140039 = `mobile-0809` 474fe775 camera lắc của phiên kia + TAI, merge `ac515d75`, `kiem --pc 474fe775` ĐẠT):** vào Tương Dương → đi bộ → Sa Mạc 1 (auto Dã Tẩu của chủ tự chạy lại), 6 phút không sập, hình đúng (không rác viền ô atlas). `[TAI] dai nguon 0 fmt 12/3 loai 1 2048x256` tạo một lần mỗi định dạng; `[VE-TAI]`: 8 + 6 + 2 trang atlas tô 0 bằng chép GPU (0,0 ms ghi lệnh), tải dần 778 → 1413 → 3684 khung/30 s (18–28 MB, 3–5 ms/30 s, hàng chờ max 142); `[PGND]` ghép nền `xoa 0.0` (P4c: không còn tải 1 MB/khe). Còn thấy trên máy ảo (không do đợt này): khung vào map 109–121 ms = `tep spr 92–101 tệp / 180–215 ms` (đọc tệp spr đồng bộ lúc vào map) + `tai 72–84 tex 21–30 MB` một khung (khung cần ngay lúc vào map; trên Fold 7 ≈ 0,5 s trong màn nạp) — việc tiếp theo nếu chủ thấy vào map lâu. Giao 00:46: `jx1mobile.apk` 109140039 md5 `4c24cf6a…` + `--chi-manifest` (máy chủ 52072 giữ), config.ini md5 khớp manifest; phiên camera đồng ý giao gộp một lần. Lùi: chép `android/apk/jx1mobile-1309-xoanen-b.apk`-tương-đương = bản 109132248 (md5 `6da7855b…`) đè lại + `--chi-manifest`.

**01:38:** phiên camera giao **109140121** (`mobile-0809` 56d21988 = TAI + camera lắc + `[VATROI 14/09]` vật rơi mức 1, chỉ `KObj.cpp` JX_MOBILE) đè lên 109140039, `--chi-manifest` xong, đã soát: apk.txt/config.ini khớp manifest, không tệp mới hơn manifest; `libRepresent3.so` arm64 trong APK đó vẫn có `[VE-TAI]` → **log Fold 7 cần đọc là của 109140121**.

**Còn lại sau đợt này:** chờ log Fold 7 của 109140039 để xác nhận `zero n/ms` < 5 ms/trang và `[VE-TAI]` ms/khung; iOS chủ dựng trên Mac (mã chung JX_MOBILE, `SDL_CopyGPUTextureToTexture` Metal cùng loại texture đã lo); P5/P6 cần chủ quyết; P2 cache chữ chỉ khi đo còn > 0,5 ms/khung; vào map: đọc tệp spr đồng bộ 100 tệp/200 ms + tải 25 MB một khung (ẩn sau màn nạp, chưa đo trên Fold 7).
