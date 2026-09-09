# BÀN GIAO — PHA 4 ANDROID: client JX1 chạy được trong game trên Android (08/09)

> Nhánh `mobile-0809` (worktree `D:\GAMEDEVNEW_wt_mobile`), đã rebase theo `origin/main` `0eabb123` và đẩy lên.
> Đọc file này TRƯỚC khi làm tiếp pha Android. Các pha trước: `BANGIAO_SDL_PHA2_0809.md` (§12, §13).

---

## 1. Trạng thái hiện tại — ĐÃ CHẠY ĐƯỢC TRONG GAME

Trên LDPlayer 9 (Android 9, x86_64, `E:\LDPlayer\LDPlayer9`, máy ảo `leidian0`), APK
`android/apk/jx1mobile-0809-chu-va-pak.apk` chạy **hết đường**: khởi động → màn hình chính →
chọn vùng → chọn máy chủ → đăng nhập → chọn nhân vật → **vào bản đồ, thấy người chơi khác,
đánh được, chữ hiện rõ**. Ảnh chụp còn trong `scratchpad/ld/z1.png`, `s3_native.png`.

| Việc | Trạng thái |
|---|---|
| Dựng 7 `.so` + libSDL3 cho arm64-v8a và x86_64 | Xong |
| Đọc dữ liệu (pak + tệp rời), script Lua, bản đồ | Xong |
| Vẽ bằng SDL_GPU / Vulkan (`Rep3Api=100`) | Xong, 57–58 FPS trên LDPlayer |
| Chữ (font bitmap trong `font.pak`) | Xong (xem §2.1 — lỗi nặng nhất hôm nay) |
| Bàn phím mềm, gõ vào ô tài khoản/mật mã | Xong |
| Chuột/chạm (đổi toạ độ theo khung letterbox) | Xong |
| Mạng: nối máy chủ tài khoản + máy chủ game | Xong (xem §2.3) |
| Âm thanh miniaudio | Chạy, thiếu thư mục `Music` nên không có nhạc nền |
| Độ phân giải theo màn hình thiết bị | Xong một phần (xem §3.1 — còn co 6 %) |

---

## 2. Bốn lỗi đã sửa hôm nay (đọc kỹ, đều là bẫy sẽ gặp lại)

### 2.1. MẤT HẾT CHỮ — tên tệp tra trong pak phải kiểu Windows (dấu `\`)

**Triệu chứng:** danh sách máy chủ trống trơn, ô đăng nhập không hiện chữ, `LoadImage FAIL` 200 lần.

**Gốc:** `KPakList::FindElemFile(tên)` (`Sources/Engine/Src/KPakList.cpp:101`) có

```c
#ifdef WIN32
    szPackName[0] = '\\';
#else
    szPackName[0] = '/';      // <-- bản Android đi nhánh này
#endif
```

Bản Android định nghĩa `_WIN64` nhưng **không** định nghĩa `WIN32`, nên ghép dấu `/`. Id băm
(`FileNameToId`) ra khác hoàn toàn id trong chỉ mục pak (chỉ mục sinh trên Windows, luôn dấu `\`)
→ **không tra được một tệp nào trong pak**. Máy test có sẵn `spr/`, `ui/`, `script/` dạng tệp rời
nên game vẫn chạy, riêng font `\Font\gbk_fs*.fnt` **chỉ nằm trong `font.pak`** → 5/5 font nạp hỏng
→ `KRepresentShell3::OutputText` bỏ mọi chuỗi.

**Đã sửa:** `#if defined(WIN32) || defined(JX_POSIX)` + đổi hết `/` thành `\` trước khi băm.
Bản Windows không đổi một dòng nào (đã có `WIN32`).

**Cùng họ:** `KPakList::FileNameToId` và `ZSPRPackFile.cpp:FileNameHash` khai báo `unsigned long id`
— trên LP64 là 8 byte nên phép `% 0x8000000b * 0xffffffef` không tràn 32 bit như Windows. Đã đổi
sang `JX_ULONG`.

> **LUẬT:** mọi thứ liên quan tới **định dạng trên đĩa / trên dây** phải đi nhánh Windows, kể cả khi
> macro là `WIN32` chứ không phải `_WIN64`. Khi thấy `#ifdef WIN32` trong đường dữ liệu, phải xét
> thêm `|| defined(JX_POSIX)`.

### 2.2. Tên tệp rời có chữ Trung/Việt (byte ≥ 0x80)

Windows lưu tên thư mục GBK dạng byte ANSI; chép sang Android thành UTF-8. `JxPathPosix`
(`KPosixWin32.cpp`) nay thử tên byte thô trước, không có thì đổi cp1252 → UTF-8 (kèm hạ chữ thường
Latin-1 cho khớp `chuan_bi_du_lieu.ps1`). Nhờ vậy `Settings\ServerList=<GBK>.ini` mở được.

### 2.3. VÀO GAME KHÔNG ĐƯỢC — máy chủ trả địa chỉ `127.0.0.1`

**Triệu chứng:** chọn nhân vật xong, kết nối vào game thất bại.

**Gốc:** máy chủ tài khoản gửi địa chỉ máy chủ game lấy từ `GameServer_cfg.ini [FixIp] InternetIp`
= `127.0.0.1`. Trên PC đó là cùng máy nên chạy; trên điện thoại/giả lập `127.0.0.1` là **chính thiết bị**.

**Đã sửa (chỉ `JX_POSIX`):** `KNetConnectAgent::ConnectToGameSvr` — nếu địa chỉ trả về là vòng lặp
hoặc rỗng thì dùng lại địa chỉ máy chủ tài khoản vừa đăng nhập. Nhật ký:
`[Gateway] may chu game tra 127.0.0.1 (vong lap) -> dung dia chi may chu tai khoan 10.0.0.140`.
Cổng Rainbow 6670 đã nghe `0.0.0.0` nên thiết bị ngoài vào được. **Bản Windows không đổi.**

### 2.4. Nhớ bộ nhớ chia sẻ (`InitMapping`)

`CreateFileMappingA`/`MapViewOfFile` giả lập bằng `calloc` trong tiến trình (WAuto ngoài không dùng
được trên Android). Trước đó `GameInit` trả FALSE lặng lẽ ở bước này.

---

## 3. ĐANG LỖI / CÒN LÀM

### 3.1. Khung vẽ còn bị co 6 % (chữ chưa sắc tuyệt đối) — **việc tiếp theo quan trọng nhất**

Đã làm: `JxSdl_ChotDoPhanGiaiTheoManHinh()` (`KSdlApp.cpp`, gọi trong `KMyApp::GameInit` ngay trước
`InitRepresentShell`) lấy cỡ cửa sổ thật làm `SCREEN_WIDTH/HEIGHT` + `SetEngineResolution`, và
`g_nDoPhanGiaiTheoManHinh` chặn `LoadResolutionFromConfig()` đọc đè. Nhờ vậy 1024×768 cố định
(co xuống 74 %) đã thành ~1040×604.

**Còn lại:** LDPlayer báo cửa sổ `1040x604` suốt lúc khởi tạo, nhưng **sau khung hình đầu tiên**
Android trả lại thanh trạng thái 36 px nên cửa sổ thật là `1040x568` → backbuffer 604 vẫn bị co
xuống 568 (94 %). Nhật ký chứng minh:

```
[SDL] man hinh 1040x604, cua so ve 1040x604      <- lúc chốt
[GPU] cua so: cua so, 1040x568 px (backbuffer 1040x604)   <- lúc chạy
```

Hướng đi cho phiên sau (chưa làm):
1. Bắt `SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED` lúc chạy rồi **dựng lại thiết bị vẽ** theo cỡ mới
   (Represent3 đã có đường `Reset`), hoặc
2. Ẩn hẳn thanh trạng thái từ phía Java (theme đã là `Theme.NoTitleBar.Fullscreen` mà LDPlayer
   vẫn hiện; thử `WindowInsetsController` / `setSystemUiVisibility` trong `SDLActivity`), hoặc
3. Ghi cỡ đo được vào `config.ini` để lần chạy sau đúng ngay.

> **BẪY ĐÃ TRẢ GIÁ:** **KHÔNG được gọi `SDL_SetWindowFullscreen(pWin, false)` sau khi bề mặt đã có** —
> Android tạo lại Activity, game **khởi động vòng lặp, chủ không mở được game**. Đã thử, đã gỡ.
> Gọi `SDL_SetWindowFullscreen(..., true)` một lần trong `KSdlApp::Init` thì an toàn.

Lùi về như cũ: đặt `[Resolution] TheoManHinh=0` trong `config.ini`.

### 3.2. Giao diện cho màn hình điện thoại thật

Chủ yêu cầu: *“phải để đúng độ phân giải của mobile để cỡ nào cũng nhìn rõ được hết”*. Trên điện
thoại thật (ví dụ 2400×1080) vẽ 1:1 sẽ **sắc nhưng chữ/nút rất nhỏ**. Cần một hệ **tỉ lệ giao diện**
(vẽ khung ở độ phân giải thật, phóng to phần giao diện + chữ theo mật độ điểm ảnh). Đây là việc lớn,
nên tách thành pha 5.

### 3.3. Chưa thử trên điện thoại thật

Mới chỉ chạy LDPlayer x86_64. Bản arm64-v8a đã dựng và nằm trong APK nhưng **chưa ai chạy**.
Trên bản arm64 dịch qua houdini sẽ không có tombstone ký hiệu, khó soi lỗi.

### 3.4. Việc nhỏ còn treo

- Không có nhạc nền: chưa chép thư mục `Music` sang thư mục dữ liệu (`\Music\Music003.mp3`).
- `LoadImage FAIL` còn ~26 lần cho vài tên kết thúc bằng `\` và `MiniMap.spr` — **giống hệt bản PC**,
  không phải lỗi riêng Android.
- Nhật ký chẩn đoán còn bật (`#ifdef JX_POSIX`): `[FONT] CreateAFont/BO chu` trong `jx_rep3.log`,
  `[GO] su kien SDL / chu vao / chuot xuong` (có chặn số lần) trong logcat. Khi ổn định thì gỡ.
- WAuto ngoài **không dùng được trên Android** (không có bộ nhớ chia sẻ liên tiến trình).

---

## 4. Cách dựng và test

```bash
# 1. Nguồn SDL3 (nếu ThirdParty/SDL3-src chưa có)
powershell -File android/tai_sdl3_src.ps1

# 2. Dựng APK  (JAVA_HOME = JDK 17 Temurin)
cd android/gradle-project && ./gradlew.bat assembleDebug
cp app/build/outputs/apk/debug/app-debug.apk ../apk/jx1mobile-0809.apk

# 3. Cài + chạy + lấy log trên LDPlayer
bash android/ldplayer_chay_thu.sh D:/GAMEDEVNEW_wt_mobile/android/apk/jx1mobile-0809.apk 30
```

- Thư mục dữ liệu trên máy ảo: thư mục chia sẻ **Misc** trỏ tới `D:\jx1_android_data`
  (khoá `statusSettings.sharedMisc` trong `E:\LDPlayer\LDPlayer9\vms\config\leidian0.config`;
  bản sao lưu `leidian0.config.bak-jx1-*`). Trong máy ảo là `/mnt/shared/Misc`.
- Chuẩn bị dữ liệu: `powershell -File android/chuan_bi_du_lieu.ps1` (robocopy + hạ chữ thường).
- **Địa chỉ máy chủ:** sửa `settings/serverlist=<GBK>.ini` (tệp có tên mã hoá) — hiện đang trỏ
  `10.0.0.140` là IP LAN của máy này. Bản gốc giữ ở `...ini.goc`.
- Nhật ký: `jx_android.log`, `jx_rep3.log`, `jx_net_sdl.log` (bật bằng tệp `jx_net_trace.on`),
  `jx_crash.log` — tất cả trong `D:\jx1_android_data`.
- Cỡ màn hình máy ảo: `adb -s emulator-5554 shell wm size 1024x768` (đang để `1040x604`).

---

## 5. Luật vẫn phải giữ

1. Mỗi lần phiên D3D11 đẩy `origin/main` → **rebase `mobile-0809` + dựng lại hai chuỗi Windows**
   (`ReverseTools/mobile_x64/build_chuoi_x64.ps1` và `build_chuoi_sdl.ps1`, chạy tách rời).
   Hôm nay đã rebase 4 lần, xung đột duy nhất là `CoreDrawGameObj.cpp` (main gỡ bộ đo VETRUNG,
   giữ chữ ký `KNPARAM nParam` của x64).
2. Sửa xong **đẩy `origin/main`/`mobile-0809` ngay**.
3. Mọi thay đổi trong nguồn game phải có rào (`JX_POSIX` / `JX_ANDROID` / `JX_PLATFORM_SDL`) để
   bản Windows **không đổi hành vi**; sửa nguồn TCVN3 **chỉ bằng python latin-1**, không dùng
   Edit/Write.
4. Không dựng/đo khi đang có trận Tống Kim.
5. Không đụng cây chạy thật `E:\SourceTuanLe\...\bin`.
