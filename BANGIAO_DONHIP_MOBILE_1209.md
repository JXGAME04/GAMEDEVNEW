# BÀN GIAO: BẢN ĐO NHỊP VẼ TRÊN ĐIỆN THOẠI THẬT — `[DONHIP 12/09]`

> Worktree `D:\GAMEDEVNEW\.claude\worktrees\mobile-144hz-optimization-429e2b`, nhánh `claude/mobile-144hz-optimization-429e2b`.
> Đọc kèm `BANGIAO_NHIP_VE_144HZ_0809.md` (tối ưu nhịp vẽ của bản PC) — bản đo này kiểm các khác biệt mobile ↔ PC trên máy thật.

## 0. Trạng thái (cập nhật 22:05)

> **00:30 11/09 — BẢN `-c` HỎNG TRÊN FOLD 7, ĐÃ THAY BẰNG `android/apk/jx1mobile-1209-donhip-d.apk`** (versionCode 109110027, md5
> `9510ae82…`, máy chủ 8765 PID 277384). Lỗi `-c`: `SDLActivity.onCreate` so `nativeGetVersion()` (3.2.30) với hằng `SDL_*_VERSION` trong
> `SDLActivity.java` của gói (bản gốc 3.2.14) → `mBrokenLibraries`, hộp thoại lỗi, app đóng sau 1–4 s không chạy native (5 phiên 00:23 chỉ có
> `jx_thietbi.log`). Sửa (commit `f278ad79` [DONHIP 12/09 c]): chép `SDLActivity.java` + `SDLSurface.java` của 3.2.30 vào gói, CMake chốt
> `FATAL_ERROR` khi Java ≠ `JX_SDL3_VER`. Đã thử máy ảo: native chạy `(SDL 3.2.30)`, Vulkan + Represent3 lên, vào game. Trong lúc chủ kẹt,
> phiên giao diện đẩy tạm `-p` (3.2.14 vá, 109110015) lúc 00:27; `-d` cao hơn nên điện thoại tự lên. Bẫy thứ hai cùng đêm: chép tệp vào
> dt_v4 mà không khởi động lại máy chủ → manifest cũ → "2 tệp lỗi" (00:21, xem §8 cuối).

> **00:15 11/09 — APK `android/apk/jx1mobile-1209-donhip-c.apk` = SDL 3.2.30 + bảng pha mới ĐÃ LÊN dt_v4** (versionCode 109110010,
> md5 `206638db…`, 20,0 MB; máy chủ 8765 khởi động lại PID 284056 với đúng dòng lệnh cũ của phiên giao diện; config dt_v4 giữ nguyên:
> `[DoNhip] Bat=1 GiayMoiPha=60 Pha=0,1,2,3,4,5 LanLap=2` → 12 phút, pha 5 = "SDL cũ" đối chứng). Nhánh: `6838634e` [DONHIP 12/09 b]
> + gộp `origin/mobile-0809 = 838fdf36` → `b9d85851`. Chưa chạy thử trên máy ảo (đang có người chạy bản `-o`). Lần mở app kế tiếp trên
> Fold 7 tự cập nhật rồi đo; kiểm nhanh: `jx_android.log` ghi `(SDL 3.2.30)`, `[NHIP-BAT]` không còn "chua co hint", pha 0 phải ≈ 118–120
> khung/s **mà không cần hint**, pha 5 ≈ 45. Cách dựng + bẫy: §8 cuối.

> **Mới nhất 10/09 23:45 — LOG FOLD 7 ĐÃ VỀ ĐỦ 12 BƯỚC, M1 XÁC NHẬN, ĐÃ CHỌN CÁCH SỬA (xem §8).** Phiên 23:23 (APK `-b`): pha 0 = 35–61 khung/s
> vì SDL dựng lại swapchain **mỗi khung**; chỉ bỏ qua SUBOPTIMAL (pha 1) → **118–120 khung/s khoá cứng, cùng mức điện ~2,4 W**; nhịp PC (pha 2)
> đều thêm chút; 1 khung bay (pha 3–5) không ổn định → giữ 2; 60 Hz phải đo lại. Hai phiên 22:05/22:53 và đoạn 23:24–23:30 là chủ **ra nền**,
> không sập. **Cảnh báo:** APK `-m` (247beeea, phiên giao diện dựng ở `D:\GAMEDEVNEW_wt_mobile` 23:29, đã lên dt_v4 23:33) **không có bản vá SDL**
> — phiên Fold 7 23:41 ghi `chua co hint`, `suboptimal +0` → pha 1 vô hiệu, máy lại 28–45 khung/s. Quyết định: nâng SDL Android lên **3.2.30**
> (chỉ Android; gói VC Windows giữ 3.2.14), bước vá đếm chạy tự động lúc CMake để mọi cây giống nhau. **23:50: phiên giao diện đã thay
> APK bộ tải bằng `jx1mobile-1309-suagd-n`** (= 7702bab0 + SDL 3.2.14 đã vá DONHIP, md5 `f49c9386…`, versionCode 109102349, đã kiểm khớp
> apk.txt; `origin/mobile-0809 = bc18c53d`) — lần thử Fold 7 kế tiếp có cả vá SDL lẫn vùng an toàn theo camera. Worktree này = `b42f8110`
> + tệp này, đang sau `mobile-0809` 6 commit — **trước khi dựng phải gộp mobile-0809** (luật chủ 10/09).

> **Mới nhất (22:05):** phiên giao diện đã gộp bản này và FF `mobile-0809 = d9a72b72` ([SUAGD 13/09] a/b/c: khung "hai họ", HUD theo vùng an toàn,
> trình chỉnh giao diện cho người chơi, nút "Chỉnh giao diện" trong Cài đặt cạnh thanh FPS). Worktree này đã FF lên `d9a72b72`;
> **APK điện thoại = `android/apk/jx1mobile-1209-donhip-b.apk`** dựng từ đúng `d9a72b72` + vá SDL (md5 `be0f3dae…`), đã thay vào bộ tải
> và khởi động lại máy chủ 8765. Bộ tải cũng có `ui/uitoado_danhsach.ini`, `spr/ui3/uisuagd/nut_chinh_gd.spr`, `uioptions.ini` mới và các khoá
> config [KHUNG/SUAGD/ANTOAN 13/09] (giữ `[DoNhip] Bat=1`, `PaintLog=1`). APK `-a` (6edcb896) chỉ còn để đối chiếu.
> Khung vẽ Fold 7 nay là **1437x616** (hai họ) — đọc từ `[NHIP-BAT]`.

- Đã vá, đã dựng APK `android/apk/jx1mobile-1209-donhip-a.apk` (arm64 + x86_64, đã soi chuỗi mã mới trong `libmain.so`,
  `libRepresent3.so`, `libSDL3.so`), commit `e56a6760` (DONHIP) + commit FPS/THONGTIN (xem git log).
- Gộp `origin/main` (BHXS / QUAICHAN / TRANGTRI f) = `bd7bd40e`, không xung đột.
- **Thêm theo yêu cầu chủ 12/09 tối** ("có thanh chỉnh FPS ở cài đặt và hiện FPS – CPU – GPU – pin ở góc phải như game mobile khác,
  không phải PerfHud"): `android/va_nguon_android_donhip2.py` + `android/sinh_uioptions_fps.py` —
  - **Thanh FPS trong Cài đặt** (`UiOptions.h/.cpp`, chỉ `JX_ANDROID`): 6 nấc *Tự động (theo màn hình) / 30 / 45 / 60 / 90 / 120*,
    đặt vào hàng "Bộ phím tắt" (ẩn trên điện thoại). Áp ngay: `PaintFps` + xin tần số màn (`ANativeWindow_setFrameRate`);
    lưu `UserData\UiCommon.ini [Options] FpsMuc`, áp lại lúc mở game (`LoadSetting`). Ini: lớp ghi đè `ui/ui3/uioptions.ini` mục `[Fps]`.
  - **Thông tin góc phải-trên**: `60 FPS | CPU 23% | GPU 41% | Pin 87% 34C | 25 ms` (`JxPerfHudAndroid.cpp`, không phụ thuộc PerfHud),
    hiện khi đã vào thế giới, `[Client] ThongTinGoc=0` để tắt, kéo được trong Sửa giao diện (khoá `ThongTinGoc`). GPU % đọc sysfs
    (Adreno `kgsl`, Samsung `/sys/kernel/gpu`, Mali) — máy không cho đọc thì hiện `-`. **Mặc định đè lên bản đồ nhỏ** (góc phải-trên
    của JX1) — chủ kéo chỗ khác hoặc phiên sau đổi mặc định sang trái bản đồ nhỏ (`KUiMiniMap::GetSelf()->GetAbsolutePos`).
- Đã cài máy ảo (kiểm md5 bản đã cài), đã đưa lên bộ tải `D:\jx1_android_data_dt_v4` + bật máy chủ 8765 `--nhat-ky D:\jx1_android_log`.
- Chuỗi Windows: **0 lỗi cả hai** (x64: Engine/Core/Represent3/S3Client; SDL: đủ 6 bước, ra `GameSDL.exe`) sau khi chép thêm các thư mục
  git bỏ qua từ worktree trước: `Lib\release64`, `Sources\packages\Microsoft.DXSDK.D3DX.9.29.952.8\build\native\{debug,release}` (d3dx9.lib),
  `Sources\Core|Engine\vcpkg_installed` (nlohmann/json.hpp), `ThirdParty\SDL3-src`, `android\gradle-project\local.properties`.
- **Đã FF + push 21:38: `origin/mobile-0809 = 6edcb896`** (gan_ff, cây `D:\GAMEDEVNEW_wt_mobile` sạch). Phiên giao diện
  (`claude/mobile-ui-customization-analysis-202ae4`, đợt [SUAGD 13/09]/[KHUNG 13/09 HAIHO]/[ANTOAN 13/09], chưa commit lúc đó) sẽ merge
  `mobile-0809` rồi thêm nút "Chỉnh giao diện" vào cửa sổ Cài đặt trên bản `UiOptions.cpp` này — **đừng đổi UiOptions/uioptions.ini song song**.
  Lưu ý từ phiên đó: khung vẽ sắp theo "hai họ" (Fold 7 = 1437x616, máy ảo 1060x616; tắt `[Resolution] KhungHaiHo=0`) → đọc log đo nhịp
  theo `SCREEN_WIDTH/HEIGHT` trong `[NHIP-BAT]`, không giả định 1440x616.
- Máy ảo: phiên giao diện đang dùng (chạy kịch bản adb, bật Sửa giao diện) — **không chạm máy ảo khi họ đang chạy**; ảnh chụp 21:33 cho thấy dòng
  góc phải chạy: `57 FPS | CPU 16% | GPU - | Pin 100% | 26 ms` (GPU `-` vì LDPlayer không có sysfs), chưa chụp được cửa sổ Cài đặt.
- Thay đổi trong worktree (đã commit):

| Tệp | Việc |
|---|---|
| `android/va_sdl3_donhip.py` (mới) | vá bản SDL 3.2.14: đếm `VK_SUBOPTIMAL_KHR` + số lần dựng lại swapchain qua hint, hint `JX_BO_QUA_SUBOPTIMAL=1` = bản sửa `bca30aa` của SDL, bật/tắt lúc chạy. Mặc định giữ nguyên hành vi 3.2.14 |
| `android/va_nguon_android_donhip1.py` (mới) | vá 3 tệp C++ (TCVN3, latin-1, kiểm số byte cao, mỗi mốc khớp đúng 1 chỗ — đã soát bằng Grep): `S3Client.cpp`, `D3D9onGPUDev.cpp`, `JxPerfHudAndroid.cpp`. Tất cả trong `#ifdef JX_ANDROID` |
| `android/gradle-project/.../JxDoNhip.java` (mới) | log thiết bị + gửi log về máy chủ tải |
| `android/gradle-project/.../JxActivity.java` | móc `onCreate` / `onPause` / `onDestroy` (SDLActivity khai `protected`, khớp) |
| `android/may_chu_tai_du_lieu.py` | thêm `POST /nhatky` + tham số `--nhat-ky` (mặc định `D:\jx1_android_log`) |
| `android/du_lieu_ghi_de/config.ini` | mục `[DoNhip]`, **`Bat=0`** (tắt = không đổi gì) |

- Đã chép vào worktree (git bỏ qua, lấy từ worktree `full-wauto-mobile-analysis-f781df`): `android/gradle-project/local.properties`,
  `ThirdParty/SDL3-src/SDL3-3.2.14`, `Lib/release64sdl`.

## 1. Chủ quyết 12/09

1. **FPS không có mặc định cố định** — tự chọn theo cấu hình từng máy.
2. **Có số đo rồi mới chọn cách sửa**, ưu tiên cách tiện cho phát triển về sau (ví dụ nâng SDL thay vì vá tay).
3. **Log lấy từ Fold 7 qua mạng LAN**: dựng APK, đưa lên máy chủ tải 8765, chủ tải về test, log tự gửi về máy tính.

Bốn nghi vấn cần số đo trên máy thật (LDPlayer không lộ được):
- **M1** swapchain dựng lại mỗi khung: SDL 3.2.14 đặt `preTransform = IDENTITY` trên Android (`SDL_gpu_vulkan.c:4669`), màn ngoài Fold 7
  hướng gốc dọc + game khoá ngang → `VK_SUBOPTIMAL_KHR` mỗi lần trình khung → dựng lại + chờ GPU rỗi (`:10589`, `:9890`, `:9819`).
  SDL issue #12950, sửa ở commit `bca30aa` (09/10/2025, nhánh release-3.2.x).
- **M2** chép nguyên swapchain sang `m_pLastFrame` mỗi khung chỉ để chụp màn hình (`D3D9onGPUDev.cpp:1052-1069`); swapchain SDL không có `TRANSFER_SRC`.
- **Nhịp vẽ**: config Android không đặt `PaintVsync` / `PaintSmooth` → chạy `PaintSmooth=1` và (ở 60 Hz) đường lập lịch cũ, không phải `[NHIP a–e]` của PC.
- **Tần số màn** là ảnh chụp lúc mở app (`SDLSurface.java:142`), game không xin nhịp màn (`setFrameRate` không có).

## 2. Các pha đo (tự chạy khi `[DoNhip] Bat=1`)

Vào thế giới 10 s thì bắt đầu; mỗi pha `GiayMoiPha` giây (60), `LanLap` vòng (2) → ~12 phút; dòng chữ vàng giữa màn hình ghi pha đang chạy.
Mỗi pha **thêm** một thay đổi so với pha trước; hết vòng tự trả về cấu hình lúc mở app.

| Pha | Thay đổi | Trả lời câu hỏi |
|---|---|---|
| 0 | hiện tại | mốc so sánh; `dung lai` ≈ số khung ⇒ M1 có thật |
| 1 | + sửa SDL (bỏ qua SUBOPTIMAL) | M1 ăn bao nhiêu FPS / độ đều |
| 2 | + nhịp PC: `PaintVsync=1`, `PaintSmooth=2` | nhịp `[NHIP]` của PC có làm đều khung trên mobile không |
| 3 | + bỏ chép khung mỗi khung, 1 khung bay | M2 + độ trễ; có tụt FPS không |
| 4 | + xin màn 120 Hz (`ANativeWindow_setFrameRate`), `PaintFps=120` | máy có chạy thật 120 không, tốn bao nhiêu W / nhiệt |
| 5 | + xin màn 60 Hz, `PaintFps=60` | mốc 60 khoá nhịp để so với 4 |

## 3. Log: ra đâu, đọc thế nào

- Điện thoại ghi trong thư mục dữ liệu app: `jx_nhip.log` (C++), `jx_thietbi.log` (Java), cùng `jx_paint.log`, `jx_rep3.log`, `jx_android.log`.
- `JxDoNhip.java` gửi **phần mới** mỗi 10 s (và khi app ra nền) → máy chủ ghi `D:\jx1_android_log\<model>_<yyyyMMdd_HHmmss>\<tệp>`.
- `jx_nhip.log`: `[NHIP-BAT]` (cửa sổ, khung vẽ, tần số SDL báo, hướng gốc/hiện tại, cấu hình lúc mở app), `[NHIP]` mỗi 10 s,
  `[NHIP-PHA]` cả pha, `[NHIP-XIN]` kết quả xin tần số, `[NHIP-XONG]`. Mỗi dòng: cách khung vẽ p50/p95/p99/max + số khung trễ > 1,5 chu kỳ,
  cắt ngang nội suy, chờ swapchain TB/max, cách giữa hai lần trả swapchain (≈ nhịp màn), SUBOPTIMAL / dựng lại swapchain, cấu hình.
- `jx_thietbi.log`: `[THIETBI]` máy/SoC/Android, `[MANHINH]` mọi chế độ màn, `[MAU]` mỗi 5 s (tần số màn đang chạy, nhiệt, headroom,
  pin %, nhiệt độ pin, dòng, áp, công suất W), `[DOI]` khi màn đổi chế độ.
- Quyết định: M1 xác nhận → sửa bằng **nâng SDL trong nhánh 3.2.x** (gồm cả `7875654` dựng lại swapchain khi quay lại app và `e0050c3`
  sập khi trở về từ nền) — đúng ý chủ "tiện phát triển về sau"; pha 2 đều hơn pha 1 → bật nhịp PC cho mobile; pha 4/5 → chính sách FPS theo máy.

## 4. Việc đã làm (21:00–21:40) và còn lại

Đã: vá SDL + 2 bản vá C++ (kiểm mã hoá: byte cao giữ nguyên), dựng APK 2 ABI, cài máy ảo (md5 khớp, versionCode 109102125), bộ tải
`D:\jx1_android_data_dt_v4` có APK + `config.ini` (`ThongTinGoc=1`, `[DoNhip] Bat=1 GiayMoiPha=60 Pha=0,1,2,3,4,5 LanLap=2`) +
`ui/ui3/uioptions.ini`; máy chủ 8765 chạy (pid xem `Get-NetTCPConnection -LocalPort 8765`) với `--nhat-ky D:\jx1_android_log`
(`POST /nhatky` đã thử: 200, ghi đúng chỗ — thư mục thử `THU_thu` để lại); hai chuỗi Windows 0 lỗi; FF + push `mobile-0809`.

Còn lại:
1. Chủ mở app trên Fold 7 → tự cập nhật (apk.txt 109102125) → chơi ~12 phút chỗ đông (§5). Theo dõi `D:\jx1_android_log\<model>_<phiên>\`.
2. Đọc `jx_nhip.log` `[NHIP-PHA]` theo §3, chọn cách sửa (M1 → nâng SDL nhánh 3.2.x; nhịp PC; chính sách FPS theo máy). Ghi kết luận vào tệp này.
3. Sau test: `D:\jx1_android_data_dt_v4\config.ini` đặt `[DoNhip] Bat=0` (máy chủ tự sinh lại manifest khi khởi động lại).
4. Mặc định dòng góc phải đang đè bản đồ nhỏ → cân nhắc đổi `ThongTin_MacDinh` sang bên trái bản đồ nhỏ (`KUiMiniMap::GetSelf()->GetAbsolutePos`).
5. Phiên giao diện sẽ merge `mobile-0809` và thêm nút "Chỉnh giao diện" vào `UiOptions.cpp` — chờ họ FF, không sửa UiOptions song song.

## 5. Chủ test trên Fold 7

- Điện thoại cùng Wi-Fi với máy `10.0.0.140`.
- Mở app → cài bản mới → mở lại → vào game → ở chỗ đông (thành / Tống Kim), đi bằng cần và đánh bình thường khoảng 12 phút.
- Dòng chữ vàng giữa màn hình ghi pha đang chạy; muốn nhận xét cảm giác thì ghi lại "pha nào mượt / rung".
- Đừng thoát app giữa chừng. Xong khi dòng vàng báo `DO NHIP: xong`.

## 7. Phân tích log MÁY ẢO 12/09 tối (chủ test thanh FPS trên LDPlayer) — đọc từ `D:\jx1_android_data\*.log`

Nguồn: `jx_paint.log` (`[SUM]` mỗi 10 s: khung vẽ, cách khung min/TB/max, `spikes` = lượt vòng bơm ≥ 25 ms, `ve` = thời gian UiPaint
**gồm cả chờ vsync trong Present**, `passes` = số vòng bơm), `jx_rep3.log`, `jx_nhip.log` (`[NHIP-XIN]`), `UserData\UiCommon.ini`.
Không lấy được logcat (phiên bị chặn lệnh shell) → không có dòng `[FPS]`/`[THONGTIN]`. Máy ảo: LDPlayer 1040x604, **màn 60 Hz, present mode vsync**,
Android 9 → `ANativeWindow_setFrameRate` không có (mọi `[NHIP-XIN]` trả −9999) — mức FPS chỉ đổi `PaintFps`. Chủ kéo thanh FPS lúc 21:36, 21:42,
22:27 và **lưu mức 5 = 120** (`FpsMuc=5`). App khởi động lại ~10 lần trong buổi (phần lớn do kịch bản adb của phiên giao diện cài lại APK).

| Đoạn (10 s/mẫu) | Khung/10 s | Cách khung min/TB/max (ms) | spikes ≥25 ms/10 s | `ve` TB (ms) | Vòng bơm/10 s | Kẹp alpha |
|---|---|---|---|---|---|---|
| **Mức 60**, chủ chơi, trước khi chuyển (21:32→21:36, giây 11–212) | 573–600 | 2–7 / 16–17 / 30–56 (cú nạp 117) | 6–15 | **7–13** | **770–1170** (có ngủ) | 0–4 |
| **Mức 120**, cảnh đông (giây 92–402 một lần chạy 21:4x) | 578–597 | 2–6 / 16–17 / 24–52 | 0–33 | **16** | 597–656 (≈ số khung) | 0–5 |
| **Mức 120**, chủ chơi cảnh yên (22:10, giây 202–642) | 595–597 | 7–12 / 16 / 22–35 | 0–6 | 16 | 597–613 | 0–3 |
| Mức 120, lần chạy tự động 22:34 (giây 92–682) | 588–601 | 5–11 / 16 / 24–37; hai cú 528 và 268 ms (nạp map, tick 479 ms) | 0–18 | 16 | 590–630 | 0–5 |

Đọc số:
1. **Trên màn 60 Hz, mức 120 không cho thêm khung nào** (59,5–60 khung/s ở mọi mức; `jx_rep3.log` `fps TB 60–61`). Vsync trong `SDL_WaitAndAcquireGPUSwapchainTexture` chặn.
2. **Cái giá của mức 120 trên màn 60 Hz**: luồng chính bị khoá trong Present ≈ 16 ms mỗi khung (`ve` 16 vs 7–13; CPU vẽ thật chỉ 2–4 ms theo `[PDET] render`),
   vòng bơm không còn khoảng ngủ (passes ≈ số khung, lưới 1 ms). Trên điện thoại đây là **M4** (alpha nội suy tính trước khi chờ 16 ms) + tốn pin.
   → Máy 60 Hz nên để **Tự động** (= 60); mức 120 chỉ có nghĩa trên màn 120 Hz thật (Fold 7) — cần log điện thoại để kết luận.
3. **Cách khung không đều ở cả hai mức** (min 2–7 ms, max 25–50 ms quanh trung bình 16,7): cùng hiện tượng "rung" 11/09. Mức 60 do lưới 8 ms + lead 4 ms
   thỉnh thoảng vẽ hai khung sát nhau; mức 120 do vẽ ngay khi swapchain thả rồi chờ. Đó chính là thứ **pha 2** (PaintVsync=1 + PaintSmooth=2) đo trên
   điện thoại; máy ảo không thử được vì LDPlayer chỉ có vsync.
4. Tick logic TB 0–3 ms, `cross` 0–10 → không nghẽn logic. Cú giật lớn chỉ khi nạp map (528–697 ms) và thi thoảng 100–270 ms (nạp sprite mới) — việc
   "30 s đầu vào map" đã biết, không liên quan mức FPS.
5. `spikes` tăng ở mức 120 (0–33 so với 6–15) một phần là **giả tạo**: mỗi vòng bơm đã gồm 16 ms chờ vsync nên chỉ cần thêm 9 ms là vượt ngưỡng 25.
6. `jx_rep3.log`: RAM riêng 560 MB, cache texture 282–288/393 MB, GPU tex 303–310 MB, giải mã sprite 5–12 ms/30 s trên luồng vẽ, `anh_null` 72–120 nghìn/30 s
   (bảng NpcRes thiếu ảnh — đã biết). GPU % trên máy ảo = `-` (không có sysfs) — đúng như thiết kế.
7. Việc nhỏ nên làm sau: kéo thanh FPS sinh **hàng chục lần áp mức** trong 2 s (mỗi bước = một lần `JxNhip_DatMuc`, `g_SetLoopInterval`, ghi log);
   nên áp khi **nhả ngón** (WND_N_SCORLLBAR_POS_CHANGED cuối) — vô hại nhưng thừa. `LoadSetting` áp mức 2 lần lúc mở game (UiInit + UiShell:447) — vô hại.

## 8. KẾT QUẢ FOLD 7 (10/09 23:23–23:41, APK `-b` = d9a72b72 + vá SDL) — M1 XÁC NHẬN, ĐÃ CHỌN CÁCH SỬA

Nguồn: `D:\jx1_android_log\SM-F966U1_20260910_232309\jx_nhip.log` (12 bước × 60 s, 2 vòng; `[NHIP-XONG]` không về vì chủ mở lại app
lúc 23:41:30, 6 s trước khi xong) và `jx_thietbi.log` (điện, tần số màn theo bước). Máy: SM-F966U1 (SM8750, Android 16), màn ngoài
2520×1080 120 Hz, hướng gốc **dọc** (`huong goc 3 hien tai 1`), khung vẽ 1436×616, `luc mo app: PaintFps 120 vsync 0 smooth 1`.

Tám phiên 22:05–23:21 trước đó đều ngắn, **không phiên nào sập**: hai phiên 22:05/22:53 chủ ra nền sau ~60 s (Java vẫn ghi `[MAU]` 5 s một,
C++ ngừng vẽ vì SDL chặn vòng lặp khi app ẩn, POST trong nền bị chặn nhưng bộ gửi giữ vị trí và gửi bù khi quay lại — không mất dòng nào);
các phiên còn lại là mở/đóng ở màn đăng nhập (`JxPosixMain tra ve 0` ghi vào phiên kế vì tiến trình cũ thoát sau khi phiên mới đã mở).
Phiên 23:23 cũng ra nền 23:24:34–23:30:35 (`xoay=0` rồi `xoay=1`) nên bước 1 kéo dài 412 s; 11 bước sau đủ 60 s.

| Bước | Pha (cộng dồn) | khung/s | cách khung p50 / p95 / p99 (ms) | trễ >1,5T | cắt ngang | chờ swapchain TB (ms) | SDL dựng lại | màn | W |
|---|---|---|---|---|---|---|---|---|---|
| 1 | 0 hiện tại (50 s đầu, trước khi ra nền) | 46–61 | 18–22 / 22–25 / 23–27 | ≈ mọi khung | 0–2 | 13–17 | **mỗi khung** (2683/2683) | 120 | — |
| 2 | 1 +sửa SDL (bỏ qua SUBOPTIMAL) | **118,5** | 8,12 / 9,62 / 10,1 | 33 | 30 | 0,04 | +1 | 120 | 2,38 |
| 3 | 2 +nhịp PC (`PaintVsync=1 PaintSmooth=2`) | **119,4** | 8,12 / 9,38 / 9,88 | **9** | **0** | 0,04 | 0 | 120 | 2,35 |
| 4 | 3 +bỏ chép khung, 1 khung bay | 82,3 | 11,6 / 15,4 / 20,1 | 1413 | 2 | 6,0 | 0 | 120 | 1,93 |
| 5 | 4 +xin 120 Hz | 86,1 | 11,1 / 15,4 / 16,9 | 936 | 0 | 5,7 | 0 | 120 | 1,75 |
| 6 | 5 +xin 60 Hz, `PaintFps=60` | 36,1 | 23,9 / 50,1 / 73,9 | 987 | 16 | 9,2 | 0 | **60** | 1,69 |
| 7 | 0 (vòng 2) | 35,3 | 27,1 / 47,6 / 55,6 | 2090 | 8 | 13,1 | mỗi khung (2117/2118) | 120 | 2,37 |
| 8 | 1 | 107,1 | 8,38 / 16,9 / 22,1 | 684 | 21 | 0,50 | +1 | 120 | 4,94 (?) |
| 9 | 2 | 110,7 | 8,38 / 14,1 / 20,1 | 406 | 11 | 0,47 | 0 | 120 | 2,83 |
| 10 | 3 | **119,9** | 8,38 / 9,62 / 9,88 | **3** | 0 | 0,12 | 0 | 120 | 2,02 |
| 11 | 4 | 55,3 | 15,6 / 37,1 / 53,4 | 1917 | 12 | 6,7 | 0 | 120 | 1,76 |
| 12 | 5 (50 s) | 33–46 | 21–28 / 34–50 / 43–55 | ~190 /10 s | 1 | 8,7–9,7 | 0 | 60 | 1,90 |

(T = 8,33 ms ở `PaintFps=120`, 16,7 ms ở 60; "trễ >1,5T" = số khung cách nhau quá 1,5 chu kỳ trong 60 s; W = trung bình `[MAU]` p= của bước, không sạc.)

Đọc số:
1. **M1 là nút thắt duy nhất đáng kể.** Pha 0: SDL 3.2.14 dựng lại swapchain **mỗi khung** (2117 lần / 2118 khung) → luồng chính chờ 13–17 ms
   mỗi khung dù CPU vẽ 2–3 ms (`[PDET] render`) → 35–61 khung/s. Chỉ bỏ qua `VK_SUBOPTIMAL_KHR` sau `vkQueuePresentKHR` (đúng nội dung
   commit SDL `bca30aa`: bọc chỗ đặt `needsSwapchainRecreate` trong `#ifndef SDL_PLATFORM_ANDROID`) → **118–120 khung/s, chờ swapchain 0,04 ms,
   cùng mức điện (2,35–2,38 W so với 2,37 W)**. SUBOPTIMAL vẫn trả về mỗi khung (+7112) vì preTransform vẫn lệch hướng màn, nhưng không dựng
   lại nên vô hại (compositor Android tự xoay).
2. **Nhịp PC có ích thêm một chút**: vòng 1 trễ 9 so với 33, cắt ngang 0 so với 30; vòng 2: 406 so với 684. → bật `PaintVsync=1`,
   `PaintSmooth=2` trong lớp ghi đè Android sau khi sửa SDL (pha 2 = đúng đường `[NHIP a–e]` của PC).
3. **1 khung bay không ổn định**: cảnh nhẹ (bước 10) cho kết quả tốt nhất cả bài (119,9; trễ 3; 2,0 W) nhưng khi CPU nặng (bước 4, 11:
   `[SEC] world` 100–130 ms/s) tụt còn 82 → 55 khung/s, chờ swapchain 6–7 ms **mỗi khung** = CPU đứng chờ GPU. Giữ **2 khung bay** như PC.
   "Bỏ chép khung" (M2) chưa tách được khỏi "1 khung bay"; điện thấp hơn ~0,4 W ở các bước 4/5/10/11 gợi ý M2 đáng làm — cần pha riêng
   (chép 0, bay 2).
4. Xin 120 Hz: không thêm gì (màn đã 120 khi game vẽ 120; Samsung tự hạ 60 Hz khi game chậm — thấy `[DOI]` ở pha 0). Xin 60 Hz: màn
   **xuống 60 Hz thật** (`che_do id=10`, `setFrameRate` trả 0), điện 1,7–1,9 W, nhưng chỉ 33–46 khung/s vì dính "1 khung bay" + vsync
   (lỡ 16,7 ms là rơi 33 ms). Phải đo lại 60 Hz với 2 khung bay trước khi làm nấc "tiết kiệm pin" của thanh FPS.
5. Nhiệt: `nhiet=0` suốt 18 phút, headroom ≤ 0,85, pin 75 → 72 %. Bước 8 đo 4,94 W là ngoại lệ chưa giải thích (các bước cùng cấu hình ~2,4 W).
6. Bài đo tự chạy ổn: chuyển pha, đổi khung bay, xin tần số đều không sập. Thiếu sót: đồng hồ pha tính cả lúc ra nền (bước 1 = 412 s)
   → phiên sau dừng đồng hồ khi không có khung vẽ (hoặc bỏ qua khoảng > 2 s giữa hai khung).
7. **APK `-m` (247beeea, phiên giao diện dựng 23:29 ở `D:\GAMEDEVNEW_wt_mobile`, lên dt_v4 23:33) không có bản vá SDL** (`grep "DONHIP 12/09"`
   trong `SDL_gpu_vulkan.c` của cây đó = 0): phiên Fold 7 23:41 ghi `SDL dem: suboptimal 0 dung lai 0 (chua co hint)`, `SDL suboptimal +0`
   → pha 1 vô hiệu, máy lại 28–45 khung/s (rủi ro §6 đã thành thật). Đã nhắn phiên giao diện; họ trả lời 23:50: đã chạy
   `va_sdl3_donhip.py` trong cây của họ (grep = 3), dựng `jx1mobile-1309-suagd-n` (23:49) và **đã thay vào dt_v4 23:50** (md5 `f49c9386…`,
   versionCode 109102349, `libSDL3.so` arm64 có chuỗi `JX_BO_QUA_SUBOPTIMAL`; máy chủ 8765 PID 279172; cũng cài lên máy ảo); commit
   `bc18c53d` [SUAGD 13/09 i] và `BANGIAO_GIAODIEN_MOBILE_SUAGD_1309.md` §7.9 ghi quy tắc "cây mới phải vá trước khi dựng".
   `[DoNhip] Bat=1` vẫn giữ → lần mở app kế tiếp trên Fold 7 sẽ đo lại đủ 12 bước với bản `-n` (chủ đừng ra nền giữa chừng).

Đối chứng thêm (chủ chạy bản `-n`/`-o` = SDL 3.2.14 đã vá, 23:50–00:08, ba phiên `SM-F966U1_20260910_235058`, `_235554`,
`SM-F966U1_20260911_000602`): pha 0 = 38–61 khung/s (dựng lại mỗi khung), **pha 1 = 109–115 khung/s (p50 8,38)**, pha 2 = 115–118
(trễ 107–262 so với 208–485 của pha 1), pha 3 "1 khung bay" = 43–72 khung/s, pha 5 = 43 khung/s. Phiên 23:41 (bản `-m` chưa vá) mọi pha
đều 34–53 khung/s, `suboptimal +0` — đúng như cảnh báo mục 7. Kết luận §8 giữ nguyên.

Quyết định (theo chủ quyết #2 "tiện phát triển về sau"):
- **Sửa M1 bằng nâng SDL Android lên 3.2.30** — bản 3.2.x cuối (01/01/2026; nhánh `release-3.2.x` đang 3.2.31-dev), có `bca30aa` (vào từ
  3.2.26, 30/10/2025), "Fixed Android applications losing vsync when being resumed" (3.2.24) và "Fixed a crash on Android upon returning
  from the background when using the GPU API" (3.2.30). **Chỉ Android**: `android/CMakeLists.txt` `JX_SDL3_SRC` → `SDL3-3.2.30`,
  `android/tai_sdl3_src.ps1` nhận phiên bản riêng cho Android; gói VC `ThirdParty\SDL3` 3.2.14 của Windows (GameSDL.exe) giữ nguyên → PC không đổi.
  Giữ bộ đếm suboptimal / dựng lại để bài đo còn dùng được: `va_sdl3_donhip.py` sửa neo cho 3.2.30 và **chạy tự động lúc CMake configure**
  (idempotent) để mọi worktree dựng ra cùng một SDL; hint đảo chiều `JX_DUNG_LAI_SUBOPTIMAL=1` = hành vi 3.2.14 (chỉ để pha 0 đối chứng).
- Sau khi nâng: lớp ghi đè Android `PaintVsync=1`, `PaintSmooth=2`, 2 khung bay; bảng pha mới cho `[DoNhip]`: 0 = SDL mới, 1 = +nhịp PC,
  2 = +bỏ chép (bay 2), 3 = xin 60 Hz + `PaintFps=60` (bay 2), 4 = 1 khung bay (đối chứng); đồng hồ pha dừng khi ra nền.
- Trước mắt: APK `-m` trên dt_v4 phải dựng lại với SDL đã vá (`python android\va_sdl3_donhip.py <wt_mobile>\ThirdParty\SDL3-src\SDL3-3.2.14`
  rồi gradle) hoặc đặt `[DoNhip] Bat=0` để chủ khỏi đo vô ích; mọi lần dựng phải gộp `origin/mobile-0809` trước (luật chủ 10/09).

**Đã làm 00:15 11/09 (chủ chốt "nâng SDL Android lên 3.2.30 theo §8, dựng lại và đo với bảng pha mới")** — commit `6838634e`
[DONHIP 12/09 b] (chỉ Android, không tệp nào của chuỗi Windows đổi: `JxPerfHudAndroid.cpp` không nằm trong vcxproj nào):
- `android/CMakeLists.txt`: `JX_SDL3_VER 3.2.30` (biến thường, không CACHE để cây cũ không giữ đường 3.2.14), báo lỗi rõ khi chưa tải nguồn,
  **tự chạy `va_sdl3_donhip.py` lúc configure** (`find_package(Python3)` + `execute_process`, FATAL_ERROR nếu lỗi) → mọi cây dựng ra
  cùng một `libSDL3.so`. `android/tai_sdl3_src.ps1`: `-Ver` mặc định 3.2.30, không còn lấy phiên bản từ gói VC Windows.
- `android/va_sdl3_donhip.py` viết lại cho 3.2.30: đếm SUBOPTIMAL / dựng lại như cũ, hint **đảo chiều** `JX_DUNG_LAI_SUBOPTIMAL=1` = hành vi
  3.2.14 (chỉ để pha đối chứng), từ chối SDL < 3.2.26, ghi tệp nguyên tử (CMake hai ABI có thể gọi cùng lúc). Đường acquire của 3.2.30
  đã coi SUBOPTIMAL là thành công, không cần vá.
- `JxPerfHudAndroid.cpp` (qua `android/va_nguon_android_donhip3.py`, latin-1 + CRLF, byte cao không đổi): bảng pha 0 "SDL mới, nhịp cũ"
  (vsync 0, smooth 1) | 1 +nhịp PC | 2 +bỏ chép khung, 2 bay | 3 xin 60 Hz + PaintFps 60 (2 bay) | 4 "1 khung bay" đối chứng | 5 "SDL cũ"
  đối chứng (hint = 1; mặc định `Pha=0,1,2,3,4`, dt_v4 ghi rõ `0,1,2,3,4,5`); **đồng hồ pha dừng khi app ra nền** (hai vòng bơm cách
  > 2 s → dời mốc, ghi `[NHIP-NGHI]`). Lớp ghi đè `config.ini`: chú thích bảng mới, `Pha=0,1,2,3,4`, `Bat=0` giữ nguyên.
- Dựng: worktree này thiếu mọi thứ git bỏ qua → chép `Sources\Core|Engine\vcpkg_installed` từ wt_mobile (robocopy, mã 1 = OK), viết
  `local.properties`, junction `D:\GAMEDEVNEW_wt_sdl30`, tải `SDL3-3.2.30.tar.gz` (15,9 MB, GitHub release) vào `ThirdParty\SDL3-src`.
  **Bẫy:** gọi `gradlew.bat` tương đối từ công cụ shell của phiên báo "not recognized" dù cwd đúng — phải gọi đường tuyệt đối
  `D:\GAMEDEVNEW_wt_sdl30\android\gradle-project\gradlew.bat assembleDebug -p <thư mục đó>`; dựng hết 2 phút 37 giây (38 task).
  Kiểm APK bằng zipfile + regex: `libSDL3.so` hai ABI có `release-3.2.30-0-gf5e5f6588`, ba hint mới, không còn `JX_BO_QUA_SUBOPTIMAL`;
  `libmain.so` có "SDL moi, nhip cu", "SDL cu (doi chung)", `[NHIP-NGHI]`.
- Đọc log lần đo tới: pha 0 (3.2.30, không hint) phải ≈ pha 1 cũ (118–120 khung/s, chờ swapchain ~0,04 ms, `dung lai +0`); pha 5 phải ≈ 45
  (chứng minh hint đối chứng hoạt động); pha 3 (60 Hz, 2 bay) kỳ vọng ~60 khung/s đều ở ~1,7 W → nếu đúng, đó là nấc "tiết kiệm pin";
  pha 4 (1 bay) kỳ vọng tụt khi CPU nặng → chốt 2 bay; pha 1 so pha 0 → có bật `PaintVsync=1 PaintSmooth=2` mặc định cho mobile không;
  pha 2 so pha 1 → có bỏ chép khung (M2) không. Sau đó đặt `[DoNhip] Bat=0` trên dt_v4.

**Sự cố 00:21–00:30 và cách xử lý (ghi để khỏi lặp):**
1. *"Báo lỗi 2 tệp không cho update"* (00:21): phiên giao diện chép `ui/uitoado_macdinh*.ini` mới vào dt_v4 sau khi manifest sinh (00:15) →
   md5 lệch → launcher lặp tải hai tệp mỗi giây, không vào bước cài APK. Xử lý: khởi động lại `may_chu_tai_du_lieu.py` (manifest chỉ sinh lúc
   khởi động). **Quy tắc:** chép bất cứ gì vào dt_v4 xong phải khởi động lại máy chủ ngay. Gợi ý sau này: máy chủ tự băm lại khi mtime đổi.
2. *Bản `-c` (SDL 3.2.30) mở rồi tự đóng sau 1–4 s* (00:23): thứ tự launcher là kiểm APK **trước** đồng bộ dữ liệu, nên điện thoại đã cài
   109110010 rồi mới gặp lỗi 2 tệp; sau khi manifest sửa, JxActivity chạy nhưng `SDLActivity.onCreate` thấy `nativeGetVersion()` = 3.2.30 ≠
   hằng Java 3.2.14 → `mBrokenLibraries` → hộp thoại "An error occurred… reinstall", bấm OK là `onDestroy`; native main không chạy nên không có
   `jx_android.log`. Bảng JNI `RegisterNatives` 3.2.14 ↔ 3.2.30 giống nhau, chỉ khác Java (`getPreferredLocales`, clipboard, `onResolvePointerIcon`).
   Sửa: gói dùng nguyên `org/libsdl/app/*.java` của SDL3-3.2.30 (`diff -rq` = 0), CMake đọc hằng trong `SDLActivity.java` và chặn khi khác
   `JX_SDL3_VER`. **Quy tắc nâng SDL:** nguồn C + Java `android-project` phải cùng phiên bản; `JxActivity` chỉ dùng `createSDLSurface(Context)`
   (còn trong 3.2.30). Launcher **không tự hạ cấp** (`maMoi <= maDangCai` thì bỏ qua) → bản hỏng đã cài chỉ gỡ được bằng bản có versionCode cao hơn.
3. Thử máy ảo bằng adb (00:28): `am force-stop` → `adb install -r D:/…apk` (MSYS_NO_PATHCONV=1) → `am start -n vn.jx1.mobile/.TaiDuLieuActivity`
   → logcat có `Running main function SDL_main`, `[ANDROID] … (SDL 3.2.30)`, `goldfish_vulkan`, `[LOGIN] da nho du dang nhap -> vao thang game`.
   Máy ảo **tự đăng nhập tài khoản của chủ** → thử xong phải `am force-stop` ngay kẻo đá phiên trên điện thoại. `screencap`/`pull` với đường
   `/sdcard/...` cần `MSYS_NO_PATHCONV=1`, đích ghi bằng đường Windows `C:/...`.

## 9. KẾT QUẢ SDL 3.2.30 TRÊN FOLD 7 (00:31–00:43 11/09, APK `-d`) + VÌ SAO "ĐÔNG LÀ TỤT FPS"

Nguồn: `D:\jx1_android_log\SM-F966U1_20260911_003034` (12 bước × 60 s, không ra nền, `[NHIP-XONG]` 00:43:12; pin 62 → 58 %, headroom ≤ 0,92,
`nhiet=0`). `[NHIP-BAT]`: `suboptimal 3434, dung lai 2` trong 37 s đầu — bản 3.2.30 tự hết dựng lại swapchain, **không cần hint**.

| Bước | Pha | khung/s | p50 / p95 / p99 (ms) | trễ >1,5T | chờ sc (ms) | dựng lại | W | màn |
|---|---|---|---|---|---|---|---|---|
| 1 / 7 | 0 SDL mới, nhịp cũ (vsync 0, smooth 1) | 111,0 / 96,9 | 8,4 / 10,9 / 26,4 — 8,6 / 20,9 / 27,6 | 238 / 992 | 0,02 / 0,04 | 0 / 1 | 3,18 / 3,49 | 120 |
| 2 / 8 | 1 +nhịp PC (vsync 1, smooth 2) | 115,5 / 115,7 | 8,4 / 11,4 / 16,1 — 8,4 / 10,4 / 19,1 | 218 / 185 | 0,03 | 0 | 3,56 / 4,41 | 120 |
| 3 / 9 | 2 +bỏ chép khung, 2 bay | 114,7 / 110,9 | 8,4 / 11,4 / 17,1 — 8,4 / 13,4 / 20,9 | 243 / 463 | 0,03 | 0 | 2,73 / 4,33 | 120 |
| 4 / 10 | 3 +xin 60 Hz, PaintFps 60 | **59,0 / 59,3** | 16,6 / 20,1 / 26,9 — 16,6 / 20,9 / 27,6 | 57 / 61 | 0,05 | 0 | **2,35 / 2,46** | **60** |
| 5 / 11 | 4 "1 khung bay" (đối chứng) | 47,7 / 80,4 | 18,1 / 41,1 / 53,6 — 11,9 / 18,1 / 24,4 | 2332 / 1886 | 8,5 / 6,0 | 0 | 2,34 / 2,39 | 120 |
| 6 / 12 | 5 "SDL cũ" (hint, đối chứng) | 34,0 / 32,6 | 29,1 / 49,4 / 62,6 — 30,6 / 44,1 / 57,6 | 2034 / 1954 | 16,0 / 16,8 | 2041 / 1955 | 2,43 / 2,25 | 120 |

Kết luận (thay cho §8 "Đọc log lần tới"):
1. **Nâng SDL 3.2.30 giải quyết M1 ngay ở cấu hình mặc định**: 97–116 khung/s, chờ swapchain 0,02–0,04 ms; pha 5 (đối chứng bật hành vi cũ
   bằng hint) rơi về 33–34 khung/s, dựng lại mỗi khung — chứng minh khác biệt đúng là chỗ đó.
2. **Nhịp PC (PaintVsync=1, PaintSmooth=2)**: không thua ở bất cứ bước nào, p99 16–19 so với 26–28 ms, trễ 185–218 so với 238–992 →
   **đặt mặc định cho mobile** (lớp ghi đè). Điện đo cao hơn 0,4–0,9 W nhưng cảnh vòng 2 nặng hơn hẳn (tất cả các pha vòng 2 đều cao) nên chưa
   tách được; đo lại khi cần bằng cảnh tĩnh.
3. **Bỏ chép khung mỗi khung (M2)**: khung/s như nhau, điện thấp hơn 0,1–0,8 W → chỉ chép khi có yêu cầu chụp màn hình (chỉ Android).
4. **Nấc 60 Hz của thanh FPS** = chế độ tiết kiệm pin thật: 59 khung/s đều (p95 20 ms, 57–61 khung trễ/phút), 2,35–2,46 W so với 3,2–4,4 W ở 120,
   màn ở 60 Hz suốt. Không cần sửa mã.
5. **2 khung bay là bắt buộc**: 1 khung bay = CPU đứng chờ GPU 6–8,5 ms mỗi khung → 48–80 khung/s.
6. Đồng hồ pha mới đúng: không có `[NHIP-NGHI]`, 12 bước đúng 60 s.

**Vì sao "FPS giảm nhiều khi đông → lag"** (chủ hỏi 00:47; dòng thời gian 10 s một, ghép `jx_nhip.log` + `[SEC]/[SPIKE]/[WORLD b]` + `[REP3-NAP]`):
- **4 trong 12 phút là pha đối chứng cố ý xấu** (bước 5–6 lúc 00:35–00:37, bước 11–12 lúc 00:41–00:43: 28–80 khung/s, 140–312 cú giật/10 s).
  Ai chơi đúng lúc đó sẽ thấy lag dù đông hay không. Từ 00:46 `Bat=0`, không còn.
- Trong các pha thật, các cửa sổ tụt (t=67 s 95 khung/s; t=447–457 s 76 → 53; t=577 s 91; t=417/547 s 104–107) **trùng với tick logic
  thế giới nặng trên luồng chính**: `[SEC] world=110–151 ms mỗi giây, tickmax 10–15 ms` đúng lúc NPC/tick nhảy 70 → 134 (đám đông tới).
  Mỗi tick 10–15 ms chặn vòng vẽ 1–2 khung (chu kỳ 8,3 ms) × 18 tick/s → còn 55–75 khung/s. Chờ swapchain vẫn 0,03 ms → **không phải vẽ, không phải GPU**.
- Trong tick, NPC chỉ tốn 0,7–0,8 ms/tick (134 NPC, 5–6 µs/NPC). Phần nặng là **"đạn"** (`g_uKhacSo[1]/g_dKhacMs[1]` = kỹ năng/đạn bay):
  **687–787 viên/tick tốn 4,3–5,3 ms/tick** (≈ 80–95 ms mỗi giây), kéo theo truy vấn không gian: `cay: duyet2 2,4–3,7 triệu / 10 s`
  (≈ 13–20 nghìn lần duyệt cây mỗi tick), `vung: hoi 425–441 nghìn / 10 s`, `quet_vung max 9–15 ms`. Cảnh yên (t=589 s): đạn 22/tick 0,5 ms,
  duyệt 155 nghìn/10 s → 119 khung/s. Tức **lag lúc đông = số đạn/kỹ năng đang bay × cách mỗi viên tìm mục tiêu trên luồng chính**.
- Phút đầu vào map còn thêm **nạp sprite trên luồng vẽ**: `[REP3-NAP]` 30 s đầu 415 tệp spr 254 ms, 24.500 lần giải mã, 7 khung nạp > 16 ms,
  nặng nhất 106 ms/khung; `NAPNPC` "trễ 437" (ảnh NPC chưa kịp) — các cú giật `paint=55–71 ms` ở t=60–62 s là chỗ này (việc "30 s đầu" đã biết).
- So với ghi chú 11/09 (LDPlayer, Tống Kim, mục tiêu 60 khung/s, "tick < 1,1 ms"): trên Fold 7 mục tiêu 120 khung/s (8,3 ms) nên tick 10–15 ms
  giờ lộ thành tụt khung; trước đây ở 16,7 ms/khung nó "vừa lọt".

Đề xuất bước tiếp (chưa làm, chờ chủ chốt; mọi thứ chỉ `JX_ANDROID`):
- **Đo sâu "đạn"**: thêm `[DAN]` mỗi 10 s trong `KSubWorld`/`KMissile*`: số viên, số lần tìm mục tiêu, số node duyệt, thời gian từng phần
  (di chuyển / va chạm / hiệu ứng) để biết phần nào của 5 ms/tick; rồi mới chọn: giới hạn số đạn xử lý mỗi tick, cache vùng, hay bỏ tìm mục tiêu
  cho đạn của người khác (client chỉ cần vẽ). Không cắt hiệu ứng (luật chủ: không giảm cảm giác).
- Nạp trước sprite NPC khi vào map (`NAPNPC` "trễ 437") — việc đã ghi trong [[mobile-tongkim-lag-goc]].
- Sau khi bật nhịp PC mặc định: khi tick 10–15 ms xảy ra, `PaintSmooth=2` nội suy giúp mượt hơn (`cat ngang` thấp), nhưng không bù được khung mất.

## 6. Rủi ro

- Bản vá SDL chỉ nằm trong bản SDL của worktree này (git bỏ qua) — dựng APK ở worktree khác sẽ thiếu bộ đếm, pha 1 mất tác dụng.
  **Đã xảy ra 10/09 23:29 với APK `-m` của phiên giao diện (§8 mục 7).**
- Pha 1–5 đổi hành vi thật khi đang chơi (đó là mục đích); hết vòng tự trả về như cũ.
- Đổi số khung bay (pha 3, lúc kết thúc) làm SDL chờ hết hàng lệnh + dựng lại swapchain một lần.
- `POST /nhatky` không xác thực (chỉ trong LAN); chỉ ghi vào thư mục nhật ký, tên tệp đã làm sạch (không thể thành `..`).
