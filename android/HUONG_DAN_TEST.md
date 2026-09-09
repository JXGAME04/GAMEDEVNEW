# Hướng dẫn test bản Android (JX1 Mobile, bản thử đầu tiên — 08/09)

Bản này là **cùng mã nguồn với `GameSDL.exe`** (bộ `bin\client64sdl`): lớp nền SDL3, bộ vẽ D3D9-trên-SDL_GPU (Vulkan), âm thanh miniaudio,
mạng BSD socket. Mục tiêu bản thử: **mở app → tới màn hình chính/đăng nhập → vào game bằng chạm**. Bàn phím/IME, WAuto, video mở đầu, chụp ảnh JPG
chưa có trên Android.

## 1. Cần gì
- Điện thoại/máy tính bảng **Android 7.0+ (API 24), chip 64-bit (arm64-v8a), có Vulkan 1.0+** (hầu hết máy từ 2017). Bật *Cài đặt → Nhà phát triển → Gỡ lỗi USB*
  nếu chép bằng `adb`.
- Dung lượng trống ≈ 10 GB cho dữ liệu game (bản thử dùng nguyên dữ liệu PC, chưa nén/đóng gói).
- Điện thoại **cùng mạng LAN với máy chủ** (địa chỉ máy chủ lấy như bản PC — cùng tệp cấu hình; `127.0.0.1` sẽ không chạy trên điện thoại).

## 2. Cài APK
- Tệp: `android\gradle-project\app\build\outputs\apk\debug\app-debug.apk` (ký khoá debug; Android sẽ hỏi "cài từ nguồn không rõ" → đồng ý).
- Bằng dây: `adb install -r app-debug.apk`. Hoặc chép tệp APK sang máy rồi bấm cài.
- **Mở app một lần** (sẽ báo "Không thấy config.ini…") để Android tạo thư mục dữ liệu riêng của app:
  `/storage/emulated/0/Android/data/vn.jx1.mobile/files/`

## 3. Chép dữ liệu game (tên tệp phải là chữ thường)
Trên máy Windows chạy (mất vài chục phút, ~9,6 GB):
```bat
powershell -File D:\GAMEDEVNEW_wt_mobile\android\chuan_bi_du_lieu.ps1 -Nguon E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client -Dich D:\jx1_android_data
```
Kịch bản chép `spr data maps maps2 script settings ui userdata (sound music nếu có)` + `config.ini` (bản SDL, `Rep3Api=100`) + `package.ini`,
bỏ mọi .exe/.dll/.log, rồi **hạ chữ thường** toàn bộ tên. Trước khi hạ chữ thường, kịch bản chép đè **lớp ghi đè Android** `android\du_lieu_ghi_de\` (ảnh VNKU cho nút kỹ năng, khung thanh dưới, bố cục mặc định, `config.ini` Android) — tệp chỉ-Android **phải** nằm trong đó, không thì chạy lại là mất. Sau đó chép lên máy:
```bat
adb push D:\jx1_android_data\. /storage/emulated/0/Android/data/vn.jx1.mobile/files/
```
(Chép bằng cáp/Trình quản lý tệp cũng được, miễn đúng thư mục trên. Android 11+ có thể không cho trình quản lý tệp ghi vào `Android/data` — dùng `adb push`.)
Kiểm nhanh: `adb shell ls /storage/emulated/0/Android/data/vn.jx1.mobile/files/` phải thấy `config.ini`, `spr`, `data`, `maps`, `script`, `settings`, `ui`.

## 4. Chạy và ghi log
- Mở app. Màn hình ngang, khung logic 1024×768 phóng vừa màn (dải đen hai bên nếu tỉ lệ khác 4:3). Chạm = chuột trái.
- Log khi chạy (cắm dây): `adb logcat -s JX1:* SDL:* DEBUG:* AndroidRuntime:*`
- Tệp log trong thư mục dữ liệu: `jx_android.log` (điểm vào), `jx_rep3.log` (bộ vẽ: phải có dòng `[GPU] thiet bi: driver vulkan`), `jx_auto.log`, `jx_crash.log`.
  Lấy về: `adb pull /storage/emulated/0/Android/data/vn.jx1.mobile/files/jx_rep3.log`
- Sập ngay khi mở: `adb logcat -b crash` (tombstone) — gửi kèm 3 tệp log trên.

## 5. Điều chỉnh
- `config.ini` trong thư mục dữ liệu: `Rep3Api=100` (bắt buộc — không có D3D9/D3D11 trên Android), `Width/Height` = khung logic (1024×768 mặc định;
  thử `1138×640` cho màn 16:9), `PaintFps=60` (điện thoại 60 Hz; 120 nếu máy 120 Hz), `FullScreen` không có tác dụng (luôn toàn màn).
- Biến môi trường `JX_DATA_DIR` (đặt qua `<meta-data android:name="SDL_ENV.JX_DATA_DIR">` trong AndroidManifest) nếu muốn thư mục khác.

## 6. Dựng lại
- Native: `cmake --build android/build/arm64` (cấu hình ở đầu `android/CMakeLists.txt`); APK: `gradlew.bat assembleDebug` trong `android\gradle-project`
  (PowerShell, `JAVA_HOME` = JDK 17). Nguồn SDL3: `powershell -File android\tai_sdl3_src.ps1` nếu thư mục `ThirdParty\SDL3-src` chưa có.
