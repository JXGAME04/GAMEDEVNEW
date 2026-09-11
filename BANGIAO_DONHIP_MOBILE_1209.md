# BÀN GIAO: BẢN ĐO NHỊP VẼ TRÊN ĐIỆN THOẠI THẬT — `[DONHIP 12/09]`

> Worktree `D:\GAMEDEVNEW\.claude\worktrees\mobile-144hz-optimization-429e2b`, nhánh `claude/mobile-144hz-optimization-429e2b`.
> Đọc kèm `BANGIAO_NHIP_VE_144HZ_0809.md` (tối ưu nhịp vẽ của bản PC) — bản đo này kiểm các khác biệt mobile ↔ PC trên máy thật.

## 0. Trạng thái (cập nhật 21:30)

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

## 6. Rủi ro

- Bản vá SDL chỉ nằm trong bản SDL của worktree này (git bỏ qua) — dựng APK ở worktree khác sẽ thiếu bộ đếm, pha 1 mất tác dụng.
- Pha 1–5 đổi hành vi thật khi đang chơi (đó là mục đích); hết vòng tự trả về như cũ.
- Đổi số khung bay (pha 3, lúc kết thúc) làm SDL chờ hết hàng lệnh + dựng lại swapchain một lần.
- `POST /nhatky` không xác thực (chỉ trong LAN); chỉ ghi vào thư mục nhật ký, tên tệp đã làm sạch (không thể thành `..`).
