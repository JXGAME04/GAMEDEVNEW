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
| Độ phân giải theo màn hình thiết bị | **Xong** (09/09 — xem §6.1; hết co 6 %, có hệ số giao diện) |
| Nhớ mật mã + tự đăng nhập trên điện thoại | **Xong** (09/09 — xem §6.2) |

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

### 3.1. ~~Khung vẽ còn bị co 6 %~~ — **ĐÃ XONG 09/09, xem §6.1** (giữ lại phần dưới để hiểu quá trình)

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

### 3.2. ~~Giao diện cho màn hình điện thoại thật~~ — **ĐÃ XONG 09/09, xem §6.1** (hệ số giao diện)

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

---

## 6. (09/09) ĐỘ PHÂN GIẢI CHUẨN CHO ĐIỆN THOẠI + NHỚ MẬT MÃ / TỰ ĐĂNG NHẬP

> Bản vá nguồn: `android/va_nguon_android_9.py`, `_10.py`, `_11.py` (chạy lại vô hại, đọc/ghi latin-1, giữ CRLF).
> APK đã kiểm: `android/apk/jx1mobile-0909-dpg-login-d.apk`.
> Hai chuỗi Windows dựng lại **0 lỗi** sau khi vá.

### 6.1. Độ phân giải: hết co 6 %, và có **hệ số giao diện**

**Thủ phạm của mục 3.1 không nằm ở phía Java mà ở Represent3.** `config.ini` để `FullScreen=0` nên
`CDevGpu::ApplyWindowMode()` gọi `SDL_SetWindowFullscreen(pWin, false)` ngay sau khung hình đầu tiên →
Android **trả lại thanh trạng thái** → cửa sổ tụt `1040x604` → `1040x568`, backbuffer 604 bị ép xuống 568
(co 6 %, chữ mờ). Nhật ký cũ chứng minh: `[GPU] cua so: cua so, 1040x568 px (backbuffer 1040x604)`.

Đã sửa (`D3D9onGPUDev.cpp`, rào `JX_ANDROID`): điện thoại **không có chế độ cửa sổ**, `ApplyWindowMode` chỉ
đọc lại cỡ thật để ghi nhật ký, **không bao giờ đổi chế độ toàn màn hình**. Kết quả:
`[GPU] cua so: toan man hinh (Android), 1040x604 px (backbuffer 1040x604)` — vẽ 1:1, chữ sắc.

Kèm theo (`JxActivity.java`, mới): `SDLActivity.onCreate()` của SDL gọi `setWindowStyle(false)` (tức là BẬT
thanh trạng thái, dù theme là `Theme.NoTitleBar.Fullscreen`) nên cửa sổ còn đổi cỡ giữa chừng. Lớp con
`vn.jx1.mobile.JxActivity` xin **immersive sticky ngay ở `onCreate`** (trước khi bề mặt được tạo) và xin lại
mỗi lần lấy lại tiêu điểm. `AndroidManifest.xml` và `ldplayer_chay_thu.sh` đã trỏ sang lớp này
(`am start -n vn.jx1.mobile/vn.jx1.mobile.JxActivity`).

**Hệ số giao diện** (`KSdlApp.cpp` `JxSdl_ChotDoPhanGiaiTheoManHinh`, viết lại): giao diện JX1 là giao diện
"điểm ảnh cố định" (chữ 12 px, nút ~100 px). Vẽ 1:1 trên 2400×1080 thì sắc nhưng chữ cao ~1,5 mm — không đọc
nổi; còn vẽ cố định 1024×768 rồi kéo căng thì to nhưng **mờ và méo**. Nên:

> **khung vẽ = cỡ màn hình thật ÷ hệ số giao diện, GIỮ ĐÚNG tỉ lệ khung hình của máy**
> (nên không viền đen, không méo), rồi Represent3 (`CDevGpu::Letterbox`) phóng lên màn hình và
> `KSdlApp::SdlToLogical` đổi toạ độ chạm ngược lại.

Hệ số chọn theo nấc 1,00 / 1,25 / 1,50 / 1,75 / 2,00 / 2,50 / 3,00 sao cho **chiều cao khung vẽ** gần
`ChieuCaoMucTieu` (mặc định 640 px) nhất; luôn hạ hệ số nếu khung vẽ hẹp hơn 800×480.

| Màn hình thiết bị | Hệ số | Khung vẽ | Đã đo |
|---|---|---|---|
| 1040×604 (LDPlayer) | 1,00 | 1040×604 (vẽ 1:1) | ✔ ảnh + nhật ký |
| 2080×1080 (điện thoại) | 1,75 | 1188×616 | ✔ ảnh + nhật ký |
| 2400×1080 | 1,75 | 1370×616 | (tính) |
| 2048×1536 (máy tính bảng) | 2,50 | 818×614 | (tính) |

`config.ini` (đã ghi sẵn vào `D:\jx1_android_data\config.ini`):

```ini
[Resolution]
TheoManHinh=1        ; 0 = tắt hẳn, dùng Width/Height như bản PC
ChieuCaoMucTieu=640  ; nhỏ hơn = giao diện TO hơn (mờ hơn); lớn hơn = nét hơn (nhỏ hơn)
HeSoGiaoDien=0       ; 0 = tự chọn; 100/125/150/175/200/250/300 = ép (hệ số ×100)
```

**Lỗi sập moi ra được nhờ phép thử này** (`va_nguon_android_11.py`): đặt màn hình máy ảo 2080×1080 → khung vẽ
1188×616 → **SIGSEGV, fault addr 0x0, trong `KRepresentShell3::Create`**. Gốc: `CD3D_Shell::PickDefaultMode()`
chỉ trả về chế độ có `Width/Height` **đúng bằng** `g_nScreenWidth/Height`; nhưng khi vẽ trong cửa sổ thì cỡ
backbuffer đâu bắt buộc phải là một chế độ màn hình → 1188×616 trả NULL → `Create` gọi `D3DTerm()`, mà
`D3DTerm()` lại làm `PD3DDEVICE->SetGammaRamp(...)` **trong khi thiết bị chưa hề được tạo** → con trỏ NULL.
Trước đây không lộ vì `Width/Height` trong `config.ini` luôn là độ phân giải chuẩn, có sẵn trong danh sách.

- `PickDefaultMode` (rào `JX_PLATFORM_SDL`): vẽ trong cửa sổ mà không có chế độ khớp → **tự đặt** một bản ghi
  `D3DModeInfo` đúng bằng cỡ khung vẽ, định dạng lấy của màn hình nền. **Không** lấy bừa một chế độ có sẵn:
  `CD3D_Device::SetPresentationParams` đặt `BackBufferWidth/Height = pMode->Width/Height` (đã thử, ra
  backbuffer 640×480 dù khung vẽ là 1188×616).
- `D3DTerm` (**không rào theo nền tảng** — đây là sửa sập thuần tuý, đường này hiện giờ LUÔN sập): không gọi
  `SetGammaRamp` khi `PD3DDEVICE` còn NULL. **Chủ soát lại giúp điểm này** nếu muốn mọi thứ đều có rào.

### 6.2. Nhớ mật mã + tự đăng nhập (để tự vào test được)

Máy đã có sẵn cơ chế nhưng bị khoá sau lối tắt của người làm game: ô "Ghi nhớ" chỉ nhớ **tài khoản**; nhớ cả
mật mã phải bấm **Alt+A** và đặt `Ui\Setting.ini [Main] AutoLogin=6323` — điện thoại không có phím Alt.

| Sửa | Ở đâu | Nội dung |
|---|---|---|
| Ô "Ghi nhớ" = nhớ cả mật mã | `UiLogin.cpp::CloseWindow` | Android: bấm "Ghi nhớ" thì `SetRememberAllFlag(true)`. Thứ ghi xuống đĩa **không phải mật mã chữ thường** mà là **bản băm MD5** (`OnLogin` băm trước khi gọi `AccountLogin`), lại được `EDOneTimePad_Encipher`. |
| Lưu ngay, không đợi lúc thoát | `Login.cpp::SelectRole` | Android hay bị hệ thống giết / người chơi vuốt tắt app nên `SaveLoginChoice` ở `UiExit` rất hay không chạy. Chọn nhân vật xong là đủ cả ba thứ → lưu ngay. |
| **Tên nhân vật phải nằm ở tệp CHUNG** | `Login.cpp::Load/SaveLoginChoice` | `IsAutoLoginEnable()` đòi cả tên nhân vật, mà tên đó vốn nằm trong `UserData\<mã>\UiConfig.ini` với `<mã> = băm(tài khoản)+băm(TÊN NHÂN VẬT)` → **muốn mở tệp thì phải biết trước thứ nằm trong tệp**. Trên PC không sao vì Alt+A chỉ dùng sau khi đã đăng nhập một lần trong cùng lần chạy. Nay chép thêm một bản vào `UserData\UiCommon.ini [Login] LastCharacter`. |
| Tự đăng nhập | `UiInit.cpp` `JxUi_TuDangNhapAndroid()`, gọi từ `UiShell.cpp UiHeartBeat` | Đã nhớ đủ tài khoản + mật mã + nhân vật thì vào thẳng game. Gọi từ **nhịp vòng lặp**, KHÔNG gọi từ `ShowCompleted`/`WndProc` của chính `KUiInit` (ở đó mới được phép đóng cửa sổ đang hiện) — giống hệt `KMyApp::ExtAutoLogin` của WAuto. **Chỉ thử một lần mỗi lần chạy** nên đăng xuất về màn hình chính sẽ không bị vòng lặp. Tắt: bỏ dấu "Ghi nhớ", hoặc `config.ini [Login] TuDongDangNhap=0`. |

Đã đo (LDPlayer, tài khoản `hinod1`): chủ gõ mật mã **một lần** với ô "Ghi nhớ" → `UserData\UiCommon.ini` có đủ
`LastAccount` / `LastPassword` / `LastCharacter` → các lần mở app sau đều
`[LOGIN] da nho du dang nhap -> vao thang game`, từ màn hình chính **vào thẳng bản đồ, không chạm cái nào**.

### 6.3. Cách tự lái máy ảo để test (không cần chủ ngồi cạnh)

`adb` bắn được cả chạm lẫn **chữ** vào cửa sổ SDL — đã kiểm:

```bash
ADB=C:/Users/nguye/AppData/Local/Android/Sdk/platform-tools/adb.exe
"$ADB" -s emulator-5554 shell am force-stop vn.jx1.mobile; sleep 4   # sleep 2 LÀ ÍT: am start bắt lại tiến trình đang chết
"$ADB" -s emulator-5554 shell am start -n vn.jx1.mobile/vn.jx1.mobile.JxActivity
"$ADB" -s emulator-5554 shell input tap 408 148        # toạ độ MÀN HÌNH; engine tự đổi về khung vẽ
"$ADB" -s emulator-5554 shell input text "matma"       # gõ vào ô đang có tiêu điểm
"$ADB" -s emulator-5554 exec-out screencap -p > shot.png
"$ADB" -s emulator-5554 logcat -d | grep -a "DPG]\|LOGIN]"
```

> **Bẫy đã mất 20 phút:** `am force-stop` rồi `sleep 2` rồi `am start` thì Android bắt lại **tiến trình cũ
> đang chết**, app không khởi động lại mà chỉ hiện lên — nhìn cứ như game "tự đăng nhập được từ đầu".
> Phải `sleep 4` và kiểm `pidof vn.jx1.mobile` trước/sau.

Đổi cỡ màn hình máy ảo để thử độ phân giải khác: `adb shell wm size 2400x1080` + `wm density 440`, xong nhớ
`wm size reset` / `wm density reset`. LDPlayer kẹp bề ngang: xin 2400 thì được 2080.

Nhật ký của hai phần này: `adb logcat` (`SDL/APP`), lọc `[DPG]` (độ phân giải) và `[LOGIN]` (tự đăng nhập);
cỡ backbuffer/cửa sổ nằm ở `D:\jx1_android_data\jx_rep3.log` (`[GPU] cua so:`).

### 6.4. Còn lại

- Ảnh nền màn hình chính / đăng nhập là ảnh **4:3**, trên khung 16:9–20:9 chỉ phủ hết chiều cao rồi neo trái,
  chừa một dải đen bên phải (thấy rõ ở ảnh chụp 1040×604). Không phải lỗi độ phân giải — là chuyện tranh nền;
  muốn kín thì kéo giãn hoặc làm ảnh nền rộng.
- Chưa thử trên **điện thoại thật** (mới LDPlayer x86_64); bản arm64-v8a có trong APK nhưng chưa ai chạy.
- `ChieuCaoMucTieu=640` là con số chọn theo lý (giữa 604 đã chạy tốt và 768 gốc), **chưa ai cầm điện thoại
  thật soi**. Đây là con số duy nhất cần chỉnh nếu chủ thấy chữ to/nhỏ quá.
- Chưa gỡ nhật ký chẩn đoán `[DPG]`/`[LOGIN]` (rẻ, mỗi lần chạy vài dòng) — gỡ khi phát hành.


---

## 9. (09/09) THƯ / ĐẤU GIÁ / CHIẾN LỆNH KHÔNG CHẠY — **đã sửa tận gốc**

Chủ báo: *"khi đưa lên mobile một số tính năng mới không hoạt động: Mail - đấu giá -
chiến lệnh"*.

### 9.1. Bệnh: hai mảng toàn cục **trùng tên** trong cùng một tiến trình

Có **hai** mảng toàn cục cùng mang tên `GameScriptFuns`:

| Nơi khai báo | Vào thư viện | Nội dung |
|---|---|---|
| `Sources/Core/Src/ScriptFuns.cpp:14954` | `libCoreClient.so` | bảng hàm kịch bản lớn — `Include`, `OB_Create`, `Say`… |
| `Sources/S3Client/Ui/ShortcutKey.cpp:2343` | `libmain.so` | 80 mục phím tắt — `Mouse_Action`, `Mouse_Menu`… |

Hàm đếm `g_GetGameScriptFunNum()` cũng bị nhân đôi y hệt.

Trên **Windows** mỗi DLL giữ bản riêng nên không ai va chạm — bản PC vẫn đúng suốt.
Trên **Android** mọi thứ là thư viện chia sẻ ELF: ký hiệu để ở mức thấy được mặc định,
bộ nạp động chỉ giữ **một** định nghĩa cho cả tiến trình, và `libmain.so` thắng.

Hậu quả dây chuyền: Core đăng ký nhầm 80 hàm phím tắt cho **mọi** kịch bản phía client
→ `Include` hoá nil → chunk chết ngay dòng đầu (`Include("\\script\\protocol.lua")`)
→ `ScriptProtocol` nil → **mọi** tính năng chạy bằng Lua phía client đều chết.
Thư, Đấu giá, Chiến Lệnh chỉ là ba cái nhìn thấy được; đo trên máy ảo: **0 chunk nạp
được / 85 lần lỗi**.

**Sửa** (`android/va_nguon_android_30.py`): cho bản của S3Client liên kết **nội bộ**
(`static`). Cả mảng lẫn hàm đếm chỉ được dùng ngay trong `ShortcutKey.cpp`, nên không
ảnh hưởng ai khác. Bản PC **không đổi hành vi**: trước giờ nó vẫn dùng đúng bảng của
chính nó.

> **Bài học để không lặp lại:** đây là loại lỗi *chỉ lộ ra khi chuyển từ DLL sang .so*.
> Mọi biến/hàm toàn cục trùng tên giữa hai thư viện đều là bom hẹn giờ trên Android,
> và nó **không** báo lỗi lúc dựng — chỉ sai lặng lẽ lúc chạy. Xem §9.4 để rà.

### 9.2. Vì sao mất bốn vòng dựng mới tìm ra: lỗi Lua bị **câm hoàn toàn**

Từ khi chuyển sang lõi Lua 5.4 (05/09), mọi câu lỗi Lua đều biến mất, chỉ còn lại
`ScriptError 4:[1] (<tên tệp>)` — đúng một mã số. Ba chỗ cùng góp phần:

| Chỗ | Vấn đề | Bản vá |
|---|---|---|
| `ScriptFuns.cpp` | ghi chú 21/08 định cho `_ALERT` bắt lỗi, nhưng `_ERRORMESSAGE` nội sinh **không hề gọi** `_ALERT` → dòng đăng ký ấy là **mã chết** từ 21/08 | 23 |
| `lua4compat.c: lua4_outerrmsg` | chỉ `fputs(stderr)` — Windows còn thấy ở console, Android **mất trắng** | 24 |
| `lua4compat.c: l4_report` | đi vòng qua `_ERRORMESSAGE`/`_ALERT`; đứt một mắt xích là mất sạch thông điệp | 26 |

Thêm `lua4_execute` (bản vá 25): trước đây `if (!lua_isfunction(...)) return L4_ERRRUN;`
— thoát **im lặng** với đúng mã lỗi của một lỗi chạy thật, nên từ ngoài không phân biệt
được "chunk chạy rồi nổ" với "chunk chưa từng chạy".

Bốn bản vá này **giữ lại** — chúng chính là thứ biến "lỗi câm" thành "lỗi đọc được", và
có ích cho cả PC lẫn máy chủ. Sau khi sửa, `ScriptError.log` ghi thẳng:

```
\script\script_protocol\protocol_def_c.lua:10: attempt to call a nil value (global 'Include')
stack traceback:
        \script\script_protocol\protocol_def_c.lua:10: in main chunk
```

`KScriptProtocol.cpp` giữ một dòng nhật ký đếm số hàm C đăng ký — chính nó bắt được bệnh
(**80** hàm bắt đầu bằng `Mouse_Action`, đáng lẽ phải là bảng của Core):

```
[SP] dang ky 133 ham C (Say...) cho \script\ui\uimail.lua     <- sau khi sua
[SP] nap \script\ui\uimail.lua vao bang rieng: ok
```

### 9.3. Đã đo tận mắt trên máy ảo

| Việc | Kết quả |
|---|---|
| Nạp kịch bản | cả `protocol_def_c.lua`, `uimail.lua`, `uichienlenh.lua` đều **ok**, `ScriptError.log` **rỗng** |
| Chạm icon thư | hộp thư mở, **2 thư thật** (2/100); chạm một thư → đọc được nội dung, người gửi, ngày, vật phẩm kèm + nút **Nhận** |
| Chạm icon đấu giá | mở đủ ba thẻ (Bang hội / Thế giới / Cá nhân), món đang đấu, giá khởi điểm/bước giá/mua ngay, đếm ngược, tiền nhân vật |
| Chạm icon chiến lệnh | mở đủ: đếm ngược hoạt động, dải mốc thưởng theo cấp, Nhiệm Vụ Ngày / Nhiệm Vụ Tuần kèm tiến độ và nút **Đến** |

APK: `android/apk/jx1mobile-0909-luafix.apk`.

### 9.4. Việc nên làm tiếp (chưa làm)

**Rà nốt các ký hiệu trùng tên khác.** Bệnh vừa rồi không báo gì lúc dựng, nên rất có thể
còn cái khác đang sai lặng lẽ. Cách rà:

```bash
for f in android/gradle-project/app/build/intermediates/cxx/Release/*/obj/x86_64/*.so; do
  nm -D --defined-only "$f" | awk '{print $3}' | sed "s|^|$(basename $f) |"
done | sort -k2 | awk '{if ($2==p) print; p=$2}' | head -50
```

Cách chặn tận gốc: dựng các thư viện với `-fvisibility=hidden` rồi chỉ mở những ký hiệu
thật sự cần xuất. Việc này đụng nhiều nên để chủ quyết.

### 9.5. Một bẫy dựng APK cần nhớ

Trong lúc truy lỗi có lúc APK **đóng gói thư viện cũ**: `stripDebugDebugSymbols` chạy
nhưng không cập nhật đầu ra. Nếu nghi bản vá không vào APK thì kiểm bằng cách tìm một
chuỗi mới ngay trong `.so` lấy từ APK:

```bash
python -c "
import zipfile
d = zipfile.ZipFile('android/gradle-project/app/build/outputs/apk/debug/app-debug.apk').read('lib/x86_64/libLua54Dll.so')
print('co dau moi:', b'chuoi-danh-dau' in d)"
```

Cần ép làm lại thì xoá `android/gradle-project/app/build/intermediates/stripped_native_libs`
rồi dựng lại. **Lưu ý:** `libmain.so` là thư viện của S3Client — sửa Core thì phải soi
`libCoreClient.so`, sửa `lua4compat.c` thì soi `libLua54Dll.so`.


---

## 10. (09/09) CHỈNH ICON TRONG GAME BẰNG NGÓN TAY — **cả PC lẫn mobile**

Chủ: *"làm thêm tính năng chỉnh toạ độ icon trong game ở bản mobile … chỉnh to nhỏ icon -
di chuyển toạ độ - xoá icon - làm cả 2 bản pc và mobile luôn"*.

### 10.1. Đã có sẵn những gì

`Sources/S3Client/Ui/Elem/UiToaDo.cpp` (chế độ Ctrl+U) **đã làm đủ cả ba việc** từ trước.
Vấn đề là nó bám hoàn toàn vào chuột và bàn phím:

| Việc | Đường cũ | Điện thoại |
|---|---|---|
| to / nhỏ | lăn chuột | **không có** |
| dời cả khối cửa sổ | kéo chuột phải | **không có** |
| giấu / hiện lại một ô | bấm chuột giữa | **không có** |
| bảng danh sách cửa sổ | bấm giữa lên chỗ trống | **không có** |
| lưu và thoát / xoá hết | Ctrl+U / Ctrl+K | **không có bàn phím** |
| dời một ô | kéo chuột trái | chạm kéo được |

Nên trên điện thoại chỉ còn mỗi việc dời một ô.

### 10.2. Đã thêm: thanh nút chạm (bản vá 32, 33)

Một hàng **8 nút** vẽ ngay trên đầu màn hình khi đang ở chế độ sửa:

```
[ Dời ô ] [ Dời khối ] [ To hơn ] [ Nhỏ lại ] [ Giấu/hiện ] [ Danh sách ] [ Lưu ] [ Xoá hết ]
```

Năm nút đầu là **công cụ** (nút đang chọn tô xanh); chạm vào ô cần sửa thì công cụ ấy
tác dụng. Ba nút sau là **lệnh**. Bề rộng nút tự chia theo `SCREEN_WIDTH`, chặn trong
khoảng 56–124 điểm ảnh nên màn hẹp vẫn bấm được, màn rộng không bị nút dài ngoẵng.

**Vào chế độ sửa trên điện thoại:** nút nhỏ **"Sửa giao diện"** ở mép phải, giữa màn hình,
chỉ hiện khi `config.ini [Ui] SuaToaDo=1` (vẫn như trước) và đang không sửa. Đổi chỗ được
bằng `[Ui] SuaToaDoNutX` / `SuaToaDoNutY` nếu nó che mất thứ gì.

> **Không đụng gì tới đường chuột cũ.** Lăn / chuột phải / chuột giữa / Ctrl+U / Ctrl+K của
> bản PC giữ nguyên từng dòng một; thanh nút chỉ là đường **thứ hai**, đi bằng bấm trái —
> nên bản PC dùng được luôn cả hai kiểu. Khi `SuaToaDo=0` (mặc định bản phát hành) thì
> `TrongNutMo` trả về false ngay, không vẽ gì, không bắt cú bấm nào.

### 10.3. Một lỗi Android tìm ra khi thử (bản vá 34)

Khoá lưu trong `UserData\UiToaDo.ini` là `<tên lớp C++>|<tên mục ini>`, lấy bằng
`typeid(*pWnd).name()`. MSVC trả `"class KUiMailManager"` nên cắt ở dấu cách là ra tên;
GCC/Itanium (Android) trả tên **mã hoá** dạng `<độ dài><tên>` — `"16KUiMailManager"` —
không có dấu cách nào để cắt.

Hậu quả thật: khoá khác nhau giữa hai bản → giao diện chủ tự đặt ở máy tính **không dùng
lại được** trên điện thoại và ngược lại. Sửa: bỏ qua các chữ số đầu tên. Giờ hai bản ghi
ra cùng một khoá, tệp `UiToaDo.ini` dùng chung được.

### 10.4. Đã đo tận mắt

Mở hộp thư → chạm **"Sửa giao diện"** → chọn **To hơn** → chạm hai lần vào khung nội dung
→ chọn **Giấu/hiện** → chạm → **Lưu**. Tệp ghi ra:

```
[Pos]
KUiMailManager|MailContentValue=0,130,1100,1
```

tên lớp sạch (giống PC), tỉ lệ 1100 = **110%** (đúng hai nấc 5%), cờ = 1 = **đã giấu**.
Thanh báo trên màn hình hiện đúng từng bước: `KUiMailManager|MailContentValue  105%`.

APK: `android/apk/jx1mobile-0909-toado3.apk`.

### 10.5. Còn có thể làm thêm

- Nút trên thanh hiện vẽ bằng khối màu + chữ (`KRUShadow` + `OutputText`) vì `UiToaDo.cpp`
  không có sẵn nguyên thuỷ vẽ hình nào khác. Muốn đẹp hơn thì thay bằng ảnh `.spr`.
- Chưa có nút **hoàn tác một bước**; hiện chỉ có **Xoá hết** (trả mọi thứ về gốc).


---

## 11. (09/09) NÚT CHỌN KỸ NĂNG ĐÁNH — chạm là đánh, kéo để ngắm hướng

Chủ: *"Thiếu phần các nút chọn kỹ năng đánh bản mobile - chạm là đánh, điều chỉnh hướng
đánh - mã nguồn mobile tham khảo đã có sẵn dựa vào viết lại cho phù hợp"*.

### 11.1. Nguồn tham khảo

`D:\USVOLAM\Jx1mClientMobile\Classes\gamescene\KSkillRocker.cpp` — đặt ngón lên nút rồi
kéo để ngắm hướng, thả ngón là đánh. Bản ấy viết cho Cocos (và phần đi lại trong nó đã bị
chú thích tắt), nên chỉ lấy **cách làm**; phần vẽ và phần chạm viết lại theo bộ của bản này.

> Ghi lại cho khỏi tìm lại: `UiMiniSkill` bên `D:\USVOLAM\Sources` **không phải** thứ cần —
> đó là bảng buff. Nút đánh nằm ở bản Cocos `Jx1mClientMobile`, không phải ở nhánh S3Client.

### 11.2. Đã làm

**Bảng nút** (bản vá 36): lấy danh sách từ `GDI_LEFT_ENABLE_SKILLS` — **đúng danh sách bản PC
dùng cho ô đánh chuột trái**. Vẽ bằng chính hàm vẽ biểu tượng của game
(`iCoreShell::DrawGameObj`) nên biểu tượng giống hệt bản PC và **không phải thêm ảnh nào**.
Mặc định 6 nút, lưới 2 cột, cạnh 56 điểm ảnh, nằm góc phải dưới phía trên thanh công cụ.

**Chạm một nút** = chọn kỹ năng đó làm kỹ năng đánh trái
(`GOI_SET_IMMDIA_SKILL`, y hệt `UiSkillTree.cpp:156`) rồi **đánh ngay con địch gần nhất**.

**Giữ rồi kéo** = ngắm: hướng kéo quyết định đánh con nào. Trong lúc kéo có **vạch chỉ hướng**
(trắng = chưa ngắm được con nào, đỏ = đã có) và **vòng tròn dưới chân** con đang ngắm. Thả
ngón là đánh.

**Core** (bản vá 35): dùng lại mã số `NPC_OI_TARGET_INFO` với `nParam == 2` — *"trả về con
địch hợp nhất"*. **Không thêm mã GDI mới** (thêm vào giữa enum sẽ đẩy mọi mã số phía sau lệch
đi — đã ghi ở §8.11). Vào: `nViTriVeX/nViTriVeY` = véc tơ hướng ngắm (0,0 = không ngắm).
Ra: tên, vị trí vẽ, `nChiSoNpc`. Lọc `relation_enemy`, bỏ con đã chết, trong tầm 700, và nếu
có ngắm thì chỉ giữ con nằm trong **nón ±60°** quanh hướng kéo, rồi lấy con gần nhất.

Đánh = `iCoreShell::LockSomeoneUseSkill(chỉ số NPC, mã kỹ năng)` — **đúng hàm bản PC dùng**
ở `ShortcutKey.cpp:1712`, không đặt thêm đường đánh riêng cho mobile.

**Bộ nhận chạm** (bản vá 37): thêm trạng thái `CHAM_KYNANG`, bật **ngay lúc đặt ngón** chứ
không đợi xê dịch như cần điều khiển — nút là một ô cụ thể nên đặt trúng nó là chắc chắn muốn
dùng nó. Nhờ vậy chạm vào nút cũng **không lọt một cú bấm chuột** xuống dưới game.

`config.ini [Cham]`: `KyNang` / `KyNangSo` / `KyNangCot` / `KyNangCo` / `KyNangX` / `KyNangY`.

### 11.3. Đã đo tận mắt — và phần **chưa** đo được

| Việc | Kết quả |
|---|---|
| Bảng 6 nút hiện ở góc phải dưới, biểu tượng thật của từng kỹ năng | **đạt** |
| Chạm nút → ô đánh trái trên thanh trạng thái đổi đúng biểu tượng vừa chạm (thử hai nút khác nhau, ô đổi theo đúng cả hai) | **đạt** |
| Giữ nút → nút sáng xanh; kéo → có vạch chỉ hướng chạy theo ngón | **đạt** |
| Không sập, không lỗi trong nhật ký sau khi thử | **đạt** |
| **Đòn đánh thật rơi trúng quái** | **CHƯA đo được** |

> **Nói thẳng chỗ chưa chắc:** nhân vật test đang đứng trong **Ba Lăng Huyện**, quanh đó không
> có con nào `relation_enemy` nên vạch ngắm luôn màu trắng và không có con nào để đánh. Phần
> tìm địch và lời gọi đánh **chưa được chạy thử với quái thật**. Lời gọi là đúng hàm bản PC
> dùng, nhưng chủ nên thử lại: ra khỏi thành, chạm một nút kỹ năng xem có đánh không, rồi giữ
> nút kéo về phía một con khác xem vạch có đỏ lên và có đánh đúng con đó không.

APK: `android/apk/jx1mobile-0909-kynang.apk`.

### 11.4. Còn có thể làm thêm

- Chưa có **thời gian hồi** (cooldown) vẽ trên nút; hiện nút nào cũng bấm được, Core tự chặn.
- Chưa cho **chủ tự chọn** kỹ năng nào vào nút nào — hiện lấy 6 kỹ năng đầu của danh sách.
- Nón ngắm cố định ±60°; muốn đổi thì sửa số trong `CoreShell.cpp` (chưa đưa ra config).


---

## 12. (09/09 chiều) NÚT KỸ NĂNG LÀM LẠI THEO ĐÚNG NGUỒN THAM KHẢO

Chủ chê bản đầu: *"chưa thấy đúng các nút chọn kỹ năng để đánh — tôi đã kêu có nút sẵn ở
vnku rồi. Vị trí nút — chức năng các nút. Bạn sắp xếp đều sai vị trí."* Chủ đúng: lần đầu
tôi **tự nghĩ ra** bố cục (ô vuông xếp lưới 2 cột ở mép phải) thay vì mở nguồn ra đọc.

### 12.1. Nguồn tham khảo nằm ở đâu — ghi lại để khỏi tìm lại

**Không phải** `D:\USVOLAM\Sources` (đó là nhánh PC; `UiMiniSkill` bên đó là **bảng buff**).
Bản mobile nằm ở **`D:\USVOLAM\Jx1mClientMobile`** (bản Cocos). Các chỗ đã dùng:

| Việc | Chỗ đọc |
|---|---|
| Bố cục cụm nút | `vn/gamescence/KgameWorldVN.cpp:1688` — *"phím kỹ năng phụ — vòng tròn skill quanh mainskill"* |
| Độ lệch từng ô | `gamescene/KgameWorld.cpp:13877` (ô 0–4) và `:13908` (ô 5–7) |
| **Cỡ thật của ô** | `gamescene/KgameWorld.cpp:13821` — nền tròn **40 px**, nhân **1.5** (ô 0–4) và **1.2** (ô 5–7) ⇒ **60** và **48** |
| Nút đánh chính | `KgameWorldVN.cpp:802` (vị trí) + `KgameWorld.cpp:3015` `mianSkillCallback` → `MainAttack` |
| Nút phụ | `KgameWorld.cpp:4364` `auxiliaryskillCallback` → `SetRightSkill` + `UseSkill(x,y,id)` |
| Kiểu dùng kỹ năng | `gameui/KuiMyMenu.cpp:251` — 0 = chạm là đánh, 1 = ngắm đánh **một** phát, 2 = ngắm đánh **liên tục**, 3 = hướng nhìn |
| Phép tính điểm ngắm | `KuiMyMenu.cpp:847–912` — điểm = chỗ nhân vật + độ lệch kéo × (tầm đánh / bán kính cần), **ép trục Y còn một nửa** |
| Đánh liên tục khi đè | `KuiMyMenu.cpp:1000` `updateMode` |
| Luật ngựa | **có sẵn trong bản này**: `Core/Src/KPlayer.cpp:12610` (hệ tự đánh) |

Ảnh nút: **`\spr\Ui3\UiSkillControl\`** của VNKU — `assign_skill_70x70/100x100/140x140/200x200`,
`effect_skill`, `switch_assign_mode`, `joystick_bg/ctrl`. Tên *"assign skill"* của cả bộ đã
nói rõ: các ô là để **người chơi gán**.

### 12.2. Đã làm

**Bố cục** — một **nút đánh chính** (92 px) ở góc phải dưới, **8 ô kỹ năng phụ xếp thành
cung** quanh nó; ô 0–4 (cung trong) **60 px**, ô 5–7 (cung ngoài) **48 px**. Độ lệch lấy
nguyên số của nguồn. Vẽ bằng `RU_T_IMAGE_STRETCH` nên đặt được **đúng cỡ** bất kể cỡ sẵn
của tệp ảnh — nhờ vậy chỉ cần một tệp khung cho cả ba cấp, và màn hình cỡ nào cũng đúng.

**Chức năng** —
- *Nút chính*: đánh thường bằng kỹ năng đánh **trái**, tự chọn mục tiêu.
- *Nút phụ*: đặt làm kỹ năng đánh **phải** rồi đánh **theo điểm**. Hai bên không giẫm chân nhau.
- **Đè là đánh liên tục** (kiểu 2 của nguồn), nhịp 200 ms.
- **Kéo để ngắm**: ô xanh chạy tới đâu thả kỹ năng ở đó; có **vòng tầm đánh** (`attack_radius`)
  và **mũi tên định hướng đánh** (`attack_direction`, chọn khung theo góc).
- **Nghiêng theo hướng**: bỏ vạch đỏ, thay bằng **núm** chạy trong lòng nút (đúng cách
  `KuiMyMenu jsSprite` của nguồn).
- **Kiểm tra ngựa** trước khi đánh — luật lấy đúng của hệ tự đánh: kỹ năng chỉ dùng được
  dưới ngựa mà đang cưỡi thì **tự xuống ngựa** rồi thôi phát đó; có chặn 5 giây `TIME_RIDE`.

**Hai ngón cùng lúc** — SDL chỉ giả lập chuột cho ngón **thứ nhất**, nên trước đây giữ cần
thì không bấm được nút kỹ năng. Mở thêm đường riêng đọc thẳng sự kiện ngón tay cho ngón
**thứ hai trở đi**: đặt vào nút kỹ năng thì ngón đó điều khiển nút, đặt vào vùng cần thì
ngón đó cầm cần. Ngón thứ nhất giữ nguyên đường cũ.

**Tự gán kỹ năng vào ô** — nút **đổi chế độ gán** (`switch_assign_mode.spr`): bật chế độ →
chạm ô (ô sáng vàng) → mở bảng kỹ năng → chạm kỹ năng ⇒ vào ô đó. Ghi ra
`UserData\KyNangMobile.ini` nên lần sau mở game vẫn còn. Ô chưa gán thì vẫn lấy theo danh
sách kỹ năng đánh, để dùng được ngay không phải gán tay.

**Mũi tên nhỏ dưới chân theo hướng di chuyển** (`direction_arrow.spr` — chính chủ chỉ rõ
đây là ảnh hướng *di chuyển*, khác `attack_direction` dùng cho hướng *đánh*).

### 12.3. Ba lỗi tìm ra khi đo

1. **Biểu tượng kỹ năng lệch lên góc trái**: `KSkill::DrawSkillIcon` (`KSkills.cpp:2861`)
   **bỏ qua** `Width/Height` — nó vẽ ảnh ở cỡ thật, lấy `(x,y)` làm góc trái trên. Phải tự
   canh giữa theo cỡ thật.
2. **Tên lớp C++ khác nhau giữa hai bản**: `typeid().name()` cho `"class KUiFoo"` trên MSVC
   nhưng `"16KUiFoo"` trên GCC ⇒ khoá lưu khác nhau ⇒ giao diện đặt ở máy tính không dùng
   lại được trên điện thoại. Đã bỏ chữ số đầu tên.
3. **Chỉnh toạ độ: nhiều icon không kéo riêng được, kéo thì đi cả cụm.** Khoá của một ô là
   `<lớp cửa sổ gốc>|<tên mục ini>`, mà tên mục ini chỉ có khi ô đó nạp từ ini; ô tạo thẳng
   bằng mã thì `TaoKhoa` trả false ⇒ `TimODuoiChuot` **leo lên cha**. Đã thêm tên thay thế
   theo vị trí trong cây cửa sổ (`<lớp>|#<thứ tự cha>.<thứ tự nó>`).
   Kèm theo: thêm **bảng ô vẽ tay** vào `UiToaDo` — bất cứ thứ gì vẽ tay (cụm nút kỹ năng,
   sau này là nút auto/nhặt đồ…) chỉ cần đăng ký **một dòng** là dời chỗ được và được lưu.

### 12.4. Đã đo tận mắt

| Việc | Kết quả |
|---|---|
| Ba cấp ô đúng cỡ, hết chồng nhau | **đạt** |
| Chạm nút đánh → chém trúng quái, hiện **−200** | **đạt** |
| Đè nút → đánh liên tục (thấy quỹ đạo chiêu + **−760**) | **đạt** |
| Kéo → vòng sáng quanh nút, ô xanh ngắm chạy theo hướng | **đạt** |
| Hai ngón: giữ cần + đè nút kỹ năng cùng lúc | **đạt** |
| Mũi tên hướng di chuyển nằm **phía trước** khi đi lên (đo được y=249 so với giữa màn 302) | **đạt** |
| Gán kỹ năng: bật chế độ → chọn ô → chạm kỹ năng ⇒ log `gan ky nang 1380 vao o 0`, tệp ghi `O0=262148,1380`, ô đổi biểu tượng | **đạt** |
| Nút HUD sẵn có (Chạy/Đi, Ngồi, Ngựa, Giao dịch, PK) có ăn cú chạm | **đạt** (thử nút Chạy/Đi, nhân vật đổi trạng thái) |

APK mới nhất: `android/apk/jx1mobile-0909-gan.apk`.

### 12.5. Làm nốt sau khi viết mục này

- **Kỹ năng trợ (aura / nội công)** — **đã xong**. Đường vốn có sẵn: `KPlayer::SetRightSkill`
  (`KPlayer.cpp:4444`) tự gọi `SetAuraSkill(id)` khi kỹ năng là aura, và
  `GOI_SET_IMMDIA_SKILL` với `nParam == 1` chính là gọi `SetRightSkill`. Trước đó tôi đặt
  nhầm vào ô đánh **trái** (`nParam 0`) nên không bật gì. Đổi một tham số là xong.
- **Không kéo được icon trong chế độ sửa** — **đã xong**, và nguyên nhân có **hai lớp**:
  (a) cách đặt tên ô (§12.3 mục 3), và (b) **vuốt dọc trên giao diện bị hiểu thành cuộn
  danh sách** (nhánh `CHAM_CUON` làm hôm nay cho khung thoại NPC). Đang ở chế độ sửa thì ý
  định luôn là **kéo** — đã bỏ qua cả nhánh cần điều khiển lẫn nhánh cuộn.
  Đo tận mắt: kéo riêng nút *Ngồi* → chỉ nó dịch, bốn nút kia đứng yên; tệp thêm đúng một
  dòng `KUiToolsControlBar|Sit=...`.

### 12.7. (10/09) Chủ báo "chưa đổi kỹ năng ô được, chưa chỉnh toạ độ nút được"

Thử lại đúng thao tác của chủ trên máy ảo → **ba lỗi thật** (bản vá `va_nguon_android_56.py`):

| # | Lỗi | Ở đâu | Sửa |
|---|---|---|---|
| 1 | Đang ở chế độ sửa, chạm vào nút kỹ năng bị **nút nuốt mất** cú chạm → hệ chỉnh toạ độ không bao giờ nhận được | `KSdlApp.cpp` bắt `CHAM_KYNANG` ngay lúc đặt ngón (cả đường ngón 1 lẫn ngón 2) | đang sửa thì không bắt nút kỹ năng |
| 2 | Kéo cụm sang **trái / lên** là vô hiệu | `s_nKNX/Y` dùng −1 = "chưa đặt" và mọi chỗ đều `(x >= 0 ? x : 0)` → độ dời âm bị vứt | độ dời thường, mặc định 0, âm dương đều được |
| 3 | Gán chỉ chạy với **bảng nhỏ**; **cửa sổ kỹ năng lớn** (mở từ thanh công cụ) bấm vào ô là *nhấc lên kéo* (`WND_N_ITEM_PICKDROP`), không phải bấm thường | `UiSkills.cpp` / `UiSkillsNew.cpp` `OnSkillPickDrop` | đang ở chế độ gán thì gán luôn, không nhấc lên |

Thêm **chữ hướng dẫn trên màn** khi đang ở chế độ gán (bước tiếp theo phải làm gì).

**Đã đo:** kéo cụm từ (913,433) về (791,370) → tệp ghi `CumKyNang=791,370` và sau Lưu cụm vẫn ở
chỗ mới; gán từ cửa sổ kỹ năng lớn → `KyNangMobile.ini` đổi thành `O0=131076,1358`.

> **Ghi lại một chuyện đáng nhớ:** lúc tôi thử bằng adb, nhật ký cho thấy chế độ sửa bật/tắt liên
> tiếp và số mục lưu tăng 7→14 — **chủ đang trực tiếp kéo icon trên máy ảo cùng lúc**. Hai bên giẫm
> lên nhau nên vài cú chạm của tôi "trượt". Lần sau trước khi lái máy ảo bằng adb, nhìn
> `logcat | grep UITOADO` xem có ai đang dùng không.

#### Hướng dẫn thao tác (cho chủ)

**Đổi kỹ năng trong ô** — nút **mũi tên vòng tròn** nằm bên **trái** cụm nút:
1. Chạm nút mũi tên vòng tròn → nút sáng vàng, hiện chữ *"Chế độ gán: chạm vào ô kỹ năng muốn đổi"*.
2. Chạm **ô** muốn đổi (ô chính hay ô phụ đều được, kể cả ô trống) → ô sáng vàng, chữ đổi thành
   *"Giờ mở bảng kỹ năng, chạm một kỹ năng để gán vào ô đang sáng"*.
3. Mở bảng kỹ năng: **sách xanh** trên thanh công cụ dưới (ô thứ 4), *hoặc* chạm ô kỹ năng đánh
   trên thanh trạng thái (bảng nhỏ) — cả hai đều được.
4. Chạm kỹ năng muốn dùng → vào ô ngay, tự thoát chế độ gán. Lần sau mở game vẫn còn
   (`UserData\KyNangMobile.ini`; xoá tệp = về mặc định).
Muốn thoát mà không gán: chạm lại nút mũi tên vòng tròn.

**Dời cụm nút kỹ năng** — trong chế độ sửa giao diện (Ctrl+U trên máy ảo, hoặc nút *"Sửa giao
diện"* dưới tiểu bản đồ): đặt ngón **lên bất kỳ ô nào của cụm** rồi kéo — **cả cụm** đi theo
(hình cung giữ nguyên). Kéo trái/phải/lên/xuống đều được. Bấm **Lưu** (hoặc Ctrl+U) → ghi
`CumKyNang=x,y` vào `UserData\UiToaDo.ini`. Muốn về chỗ mặc định: xoá dòng đó.
Lưu ý: khi đang sửa giao diện, nút kỹ năng **không đánh** — đó là cố ý.

### 12.8. (10/09) "Nút kỹ năng phải điều chỉnh từng nút được" — **đã làm** (`va_nguon_android_58.py`)

Trước: cả cụm chỉ có **một** độ dời (`CumKyNang`). Nay **mỗi nút** (`KyNang0`..`KyNang8` = nút chính +
8 ô phụ, `KyNangGan` = nút mũi tên vòng tròn) có độ dời riêng `s_nKNDoiX/Y[]`, đăng ký thành 10 "ô riêng"
với hệ chỉnh toạ độ. Trong chế độ sửa giao diện:

- **"Dời ô"** (công cụ mặc định): đặt ngón lên **một** nút → chỉ nút đó đi.
- **"Dời khối"**: đặt ngón lên nút nào cũng kéo **cả cụm** (như trước).
- Ghi vào `UserData\UiToaDo.ini` dạng `KyNang3=x,y` (toạ độ màn hình của nút); xoá dòng = nút về chỗ
  trong cung mặc định. Cỡ (`TiLe`) và ẩn (`Co`) cũng nhận từng nút.

### 12.9. (10/09) "Lưu config toạ độ hiện tại làm mặc định, có xoá cũng quay trở lại như này" — **đã làm** (`va_nguon_android_59.py`)

Hai lớp tệp, nạp chồng lên nhau lúc mở game:

| Lớp | Tệp | Ai ghi | Nạp |
|---|---|---|---|
| Mặc định của **game** | `\Ui\UiToaDo_MacDinh.ini` (trong dữ liệu Android: `ui/uitoado_macdinh.ini`) | chủ chốt bằng nút **"Mặc định"** trên thanh công cụ sửa giao diện | **trước** |
| Người chơi tự đặt | `\UserData\UiToaDo.ini` | nút **Lưu** / Ctrl+U | **sau**, khoá trùng thì đè |

- **"Xoá hết"** giờ chỉ xoá lớp người chơi rồi **nạp lại lớp mặc định và áp ngay** → về đúng bố cục chủ
  đã chốt, không phải về ini gốc của bản PC nữa.
- Tệp mặc định hiện tại = **bố cục chủ đang có trên máy ảo lúc 11:13 ngày 10/09** (chép nguyên
  `UserData\UiToaDo.ini`). Muốn chốt lại: sắp xong → bấm **"Mặc định"** (không cần dựng lại).
- Thanh công cụ sửa giờ có **9 nút**: Dời ô · Dời khối · To · Nhỏ · Giấu/hiện · Bảng · Lưu · Xoá hết ·
  **Mặc định**. Bề rộng nút tự chia theo màn hình (56..124 px).
- Bẫy đã gặp: chuỗi C `"Ui\UiToaDo_MacDinh.ini"` → clang đọc `\U` là mã Unicode → lỗi dịch; phải viết
  `\\U`. (Bash tool nuốt dấu `\`, sửa bằng script Python `va_nguon_android_59b.py`.)

### 12.10. (10/09) HUD ĐIỆN THOẠI THEO BỐ CỤC VNKU — **đã làm, chưa xem trong game**

Ba câu của chủ, theo thứ tự: *"xoá cái main này, để lại các icon, thay vào cái main của VNKU"* →
*"thanh đó nhỏ chứ không to như vậy"* → *"bạn thiết kế theo bố cục VNKU được không"*. Làm theo câu cuối.

**Mẫu để bám:** ảnh chụp JX1M thật `D:\USVOLAM\JX1M_VNKU_DOCS\anh_mau_bo_cuc_hud.jpeg` (1034×584). Đọc từ
ảnh đó: hàng **icon tròn có nhãn** ngay dưới thanh trạng thái (Nhân vật, Hành trang, Võ công, Bang hội,
Tổ đội, Ngồi, Hảo hữu, Cài đặt…), **cột icon bên phải** dưới bản đồ nhỏ (Trao đổi, Auto, Nhiệm vụ, PK…),
**khung chat nhỏ giữa đáy** (~41 % bề ngang, chỉ có nút kênh + ô gõ + biểu cảm + gửi), cụm kỹ năng góc
phải dưới, cần đi góc trái dưới (chỉ hiện khi chạm).

**Chỉ đổi dữ liệu Android** (`ui/ui3/uiplayerbar.ini`, `uitoolscontrolbar.ini` + 13 ảnh `.spr` sinh ra),
**không đổi C++, bản PC không đổi**. Công cụ sinh ra tất cả: `android\bo_cuc_vnku_mobile.py` (thêm `mock`
để ghép ảnh dựng thử lên ảnh chụp máy ảo). Chạy lại là ra y hệt; đổi số trong `bo_cuc()` rồi chạy lại.

Hai điều phải biết trước khi động vào ảnh VNKU (đã ghi ở `JX1M_VNKU_DOCS\anh_xa_cua_so.md`):
- Ảnh `.spr` của VNKU **to gấp 5/3** so với `Width/Height` trong `.ini` của họ (bộ vẽ của họ co giãn).
  Bộ vẽ của ta vẽ ảnh **đúng cỡ gốc** → phải **thu nhỏ sẵn**: công cụ đọc `.spr` (mọi khung), thu nhỏ,
  ghi lại `.spr` **nhiều khung, bảng màu chung 256** (`ghi_spr_nhieu_khung`) — bảng màu luôn ghi đủ
  768 byte (bẫy 06/09 ở `ghi_spr.py`).
- Icon tròn có nhãn cao 168 → **50 px** (tròn ≈ 40 px như ảnh mẫu); `pk.spr` 74 → 44 px, 3 khung.

| Vùng | Ô (ini) | Ảnh | Toạ độ màn hình 1040×604 |
|---|---|---|---|
| Hàng icon tròn (tâm y 72) | Status, Items, Skills, Faction, Team, **Sit**, Friend, Options | `spr/uinew/uitoolscontrolbar/{nhanvat,hanhtrang,vocong,banghoi,todoi,ngoixuong,haohuu,caidat}_m.spr` (2 khung: thường / bấm) | tâm x 400 + 56·i (i = 0..7), kết thúc trước bản đồ nhỏ (854) |
| Cột phải dưới bản đồ nhỏ (như ảnh mẫu) | hàng tâm y 173: Exchange (891), PK (941), AutoPlay (991); cột tâm x 891: Horse (223), Run (273) | `icon_traodoi_m`, `pk_m`, `lenngua_m`, `dichuyen_m`; Auto giữ ảnh cũ 28 px | **trên** cụm kỹ năng (~320) |
| **Vật phẩm nhanh chỉ 1–4** (chủ: *"phím số bỏ item để lại 1–4 thôi và để vào vị trí như VNKU"*) | Item_0..3 khối **2×2** (36 px, bước 40), nền ô `o_item.spr` VNKU vẽ sẵn vào ảnh Main | | (932/972, 205/245) — sát mép phải, cạnh Lên ngựa / Chạy |
| Khung chat giữa đáy | `[Main]` Image = `khung_chat_mobile.spr` (khung_chat_new thu 0,32 = 430×68 trên nền trong suốt 1040×604) | | (305,536) |
| Trên khung chat | ChannelBtn ô vuông trái; InputEdit 218×18 trên dải giấy; Face, SendBtn hai ô vuông phải | ảnh cũ | 365,570 / 406,571 / 634,569 / 664,569 |
| Ghi hình (Rec) | góc trái dưới, chủ đang ẩn | ảnh cũ | (8,566) |
| **BỎ** (chủ: *"bỏ luôn 4 cái trên hình"* = T/P, túi mở rộng, ẩn chat; và ô 5–9) | ImediaLeftSkill, ImediaRightSkill, ItemEx, HideChat, Item_4..8 | | đẩy ra ngoài màn `Left=-303` — muốn lấy lại thì đặt toạ độ trong ini hoặc kéo bằng chế độ sửa |
| Nút "Sửa giao diện" | `config.ini [Ui] SuaToaDoNutX=200 / SuaToaDoNutY=40` | | (200,40) — nhường chỗ (SCREEN_WIDTH−98, 172) cho cột phải |

`KUiToolsControlBar` Main về (0,0) 1040×604 (trước là 450,440 200×150), sáu nút toạ độ tuyệt đối.
Ini = màn hình − (3,1) vì chủ đặt `KUiPlayerBar|Main=3,1`. Các cửa sổ chủ **đã tự xếp** (bản đồ nhỏ, khung
chat trái, Chiến lệnh, Đấu giá, Thư, Nhiệm vụ, Kỳ trân các, "Sửa giao diện") **không đụng**.

- Ảnh dựng thử `mock_vnku.png` đã gửi chủ (ghép icon thật lên ảnh chụp, chưa chạy game).
- **Chưa xem trong game**: đổi ini phải **mở lại app** mà chủ dùng máy ảo suốt buổi (nhật ký chạm liên
  tục tới 11:32). Khi máy ảo rảnh: `adb shell am force-stop vn.jx1.mobile` → mở lại → chụp so với
  `mock_vnku.png`. Chỗ nào lệch: **kéo bằng chế độ sửa giao diện** rồi bấm **"Mặc định"** — không dựng lại.
- Cách vẽ nút của ta: `KWndButton` vẽ ảnh **đúng cỡ** từ góc trên-trái, `Width/Height` chỉ là vùng bấm →
  công cụ đặt `Width/Height` = cỡ ảnh đã thu. Nhãn "Hành trang" rộng 73 px nên bước 56 (ảnh mẫu bước 50
  thì đè chữ nhau).
- Muốn quay về thanh cũ: bỏ hai ini trong `android\du_lieu_ghi_de\ui\ui3\` rồi chạy lại
  `chuan_bi_du_lieu.ps1` (hoặc chép lại từ cây client PC). Muốn thanh dưới kiểu "khung VNKU to, giữ icon
  cũ" (bản dựng đầu, `mock_hai_co.png`): xem lịch sử git `20af1a01` (`thanh_duoi_mobile.py`, đã bỏ).

### 12.11. (10/09) Dữ liệu Android là SINH RA — lớp ghi đè `android\du_lieu_ghi_de\`

`chuan_bi_du_lieu.ps1` sinh `D:\jx1_android_data` từ cây client PC và **không có** bước nào giữ tệp
chỉ-Android → chạy lại là mất: ảnh VNKU cho nút kỹ năng (`spr/ui3/uiskillcontrol`, `attack_direction`,
`direction_arrow`, `attack_radius`), khung thanh dưới, hai ini thanh dưới, `ui/uitoado_macdinh.ini`,
`config.ini` có `[Resolution] TheoManHinh` / `[Login]`. Nay tất cả nằm trong `android\du_lieu_ghi_de\`
(cùng cây thư mục, tên **chữ thường** sẵn) và kịch bản chép đè lên **sau cùng** (`them_lop_ghi_de.py`).
**Luật:** thêm/sửa tệp chỉ-Android → sửa ở lớp ghi đè rồi chép sang thư mục dữ liệu (hoặc ngược lại,
nhưng phải có cả hai).

### 12.12. (10/09 trưa) Bốn yêu cầu nối tiếp của chủ — **đã làm, chưa xem trong game**

| Chủ nói | Nguyên nhân / cách làm | Bản vá |
|---|---|---|
| *"sửa lại vị trí đấu giá – mail về mặc định, mở box lên bị lỗi vị trí không điều chỉnh được"* | Ảnh chụp: khung hộp thư mở đúng chỗ chủ kéo (146,43) nhưng **ruột** (danh sách thư, trang đấu giá) bị dời thêm đúng (146,43)/(154,59) nữa. Khoá bảng toạ độ là `<lớp>\|<mục ini>`; cửa sổ gốc đọc `[Main]` của `mail_manager.ini`, còn **con bên trong đọc `[Main]` của `mail_list.ini`** → cùng khoá `KUiMailManager\|Main` → con bị `SetPosition` theo. Sửa: ô con mà mục ini tên `Main` thì dùng tên thay thế `#cha.con`; và `ApChoCay` cũng dùng tên thay thế (trước đây ô không tên ghi được mà **không áp lại lúc mở game** — lỗi ngầm). Cả PC lẫn Android. Đã bỏ hai dòng đó khỏi tệp mặc định (hộp mở ở chỗ ini gốc). | `va_nguon_android_60.py` |
| *"làm thêm nút ẩn các icon phía trên và nút ẩn kênh chat như VNKU"* | Nút mũi tên `an_icon.spr` của VNKU (thu 24×36, 2 khung `>` / `<`). **`[HideIcons]`** (mục mới, `KUiPlayerBar::AnHienIconTron`) ở **đầu hàng icon tròn** (344,54) — cuối hàng là chỗ chủ đặt Chiến lệnh: bấm → ẩn/hiện 7 icon tròn + nút Ngồi (nằm ở `KUiToolsControlBar`, tìm theo mục ini `Sit`). **`[HideChat]`** có sẵn trong mã (bật/tắt khung chat trái), chỉ đổi sang ảnh mũi tên, đặt (364,122) mép phải khung chat. Ini PC không có `[HideIcons]` → nút 0×0, không ảnh hưởng. | `va_nguon_android_61.py` (+`61b`: biến tĩnh phải khai báo trước `WndProc`) |
| *"bỏ nền các tab chức năng kênh chat"* | Nền tối sau `Tất cả / Mật / …` chính là ảnh của từng tab (`TabButton_i Image=…通用弹出字标签.spr`, 62×20, 2 khung). Đặt `Image=` rỗng trong `uimsgcentrepad_left*.ini` (chỉ dữ liệu Android) → còn chữ. | `bo_cuc_vnku_mobile.py` |
| *"làm kênh chat có thể vuốt lên xuống xem tin tức"* | Vuốt dọc đã thành `CHAM_CUON` → `WM_MOUSEWHEEL`, nhưng **WM_MOUSEWHEEL không mang toạ độ**: `Wnd_ProcessInput` lấy vị trí **con trỏ**, mà con trỏ còn nằm ở lần chạm trước (chỗ khác) → lăn rơi vào cửa sổ khác. Thoại NPC cuộn được chỉ vì chạm trước đó là chạm mở thoại. Sửa: vào `CHAM_CUON` thì gửi `WM_MOUSEMOVE` về chỗ đặt ngón trước, thêm nhật ký `[CHAM] cuon tai x,y`. **Lưu ý:** trong chế độ sửa giao diện vuốt dọc luôn là KÉO (cố ý) — thử cuộn phải tắt chế độ sửa. | `va_nguon_android_62.py` |

Thêm: T/P (`ImediaLeftSkill/RightSkill`) chủ thấy "sai chỗ" trong ảnh — chúng mang cờ **ẩn** trong bố cục của chủ nên
chỉ hiện ở **chế độ sửa**; tệp mặc định đã bỏ hai dòng đó → bấm "Xoá hết" là mất hẳn. Ba biểu tượng trên khung chat
(kênh / biểu cảm / gửi) đo lại đúng ô vuông (lệch ≤ 2 px), chủ đã tự kéo `ChannelBtn` 3 px.
APK: `android\apk\jx1mobile-1009-anicon.apk` (vá 56–62). PC x64 + SDL dựng sạch.

### 12.13. (10/09 13:1x) "Mấy nút ẩn phải nằm phía trong", "mất một số nút kỹ năng phụ", "lưu cấu hình trước khi làm"

- **Đã sao lưu nguyên vẹn** bố cục chủ lúc 13:11 (54 mục): `userdata\uitoado_sao_luu_1009_1311.ini` (thư mục dữ
  liệu) + `android\du_lieu_ghi_de\ui\uitoado_sao_luu_1009_1311.ini`, và đặt luôn làm **mặc định**
  (`ui\uitoado_macdinh.ini`). Từ đây "Xoá hết" về đúng bố cục này.
- **"Mất" nút kỹ năng phụ** (`va_nguon_android_63.py`): không mất — vòng vẽ **bỏ qua ô phụ chưa gán kỹ năng** khi
  không ở chế độ sửa (chủ xem 11:50 là đang ở chế độ sửa nên thấy đủ 8). VNKU luôn vẽ ô trống (vòng "+").
  Nay: ô trống vẫn vẽ khung tròn; **chạm ô trống = vào chế độ gán cho đúng ô đó và mở luôn bảng kỹ năng**
  (`SCK_SHORTCUT_SKILLSNEW`, như nút Võ công) — một chạm thay vì ba.
- **Nút ẩn "phía trong"** (chỉ dữ liệu, `bo_cuc_vnku_mobile.py`): mũi tên thu gọn hàng icon dời về **cuối hàng**
  ngay sau Cài đặt (782,54) như `HidenButton` của VNKU (hàng icon dời trái 40: tâm 360 + 56·i); mũi tên ẩn chat
  vào **góc trên‑phải bên trong khung chat** (340,121) — khung chủ đặt (2,119) rộng 370, chữ kết thúc ~306.
- APK: `android\apk\jx1mobile-1009-otrong.apk` (vá 56–63). PC x64 + SDL dựng sạch.
- **(13:3x) "icon chọn kênh – mặt cười – gửi đều bị lệch"** — lỗi của tôi: công cụ trừ (3,1) khỏi mọi toạ độ vì
  tưởng `KUiPlayerBar|Main=3,1` của chủ là gốc; nhưng **ảnh khung vẽ tại Main và nút con cũng tính từ Main** →
  toạ độ ini phải bằng toạ độ **trong ảnh**, không trừ gì. Ba biểu tượng lệch 3 px trái, 1 px lên. Đã đặt
  `DX, DY = 0, 0`, sinh lại; đo lại trên `.spr`: kênh 365..385 / mặt cười 634..655 / gửi 665..686 (y 569..590)
  — ini 365,570 / 634,569 / 664,569 (≤ 1 px). Bỏ dòng `KUiPlayerBar|ChannelBtn=362,569` (chủ kéo bù 3 px)
  khỏi tệp mặc định; trong `UserData\UiToaDo.ini` của chủ vẫn còn → "Xoá hết" hoặc xoá dòng đó.
- **(13:4x)** chủ: *"vẫn còn lệch, dịch phải 1 tí"* → ba biểu tượng +2 px (phần vẽ trong ảnh icon lệch trái);
  *"nút ẩn kênh chat cho vào bên trong"* → đặt ở **cuối hàng tab kênh** (344,251), ngay sau "Khác".
- **(13:5x) Menu chọn kênh chat "bị chèn chữ – mất 1/3 chữ"** (`va_nguon_android_64.py`): `[ANDROID 09/09 CHAM]`
  nới đệm trên/dưới dòng menu lên 7 cho dễ chạm nhưng chỉ nới **dòng** khi chiều cao dòng còn mặc định;
  menu kênh (`PopupChannelMenu`) đặt sẵn `nItemHeight` = cao ảnh kênh (~22) → chữ 12 + dấu tiếng Việt vẽ từ
  y+7 tràn xuống, dòng sau vẽ đè. Sửa trong `PopupMenu.cpp` (JX_ANDROID): dòng đặt sẵn mà thấp hơn
  `chữ × số dòng + 2 × đệm` thì nới lên. APK `android\apk\jx1mobile-1009-menukenh.apk` (vá 56–64).

### 12.14. (10/09 14:xx) Sáu yêu cầu về kỹ năng — chỉ mobile (`va_nguon_android_65.py`, APK `jx1mobile-1009-bangchon.apk`)

| Chủ nói | Làm |
|---|---|
| *"ô kỹ năng chính không đổi kỹ năng được"* | Chế độ gán giờ nhận cả **ô chính** (`s_nKNOChon = KYNANG_CHON_CHINH`): gán = đặt làm kỹ năng đánh **trái** của Core (`GOI_SET_IMMDIA_SKILL`, nParam 0) và ghi `Chinh=<loại>,<mã>` vào `UserData\KyNangMobile.ini`; mở game áp lại một lần khi đã có danh sách kỹ năng. |
| *"mở bảng kỹ năng, bấm kỹ năng nào thì hiện thông tin và kèm 3 nút gắn chính – gắn phụ – gỡ"* | **Bảng 3 nút vẽ tay** (`JxKyNang_MoBangChon/ChamBangChon/VeBangChon`), mở từ `OnSkillPickDrop` của cả `UiSkillsNew` lẫn `UiSkills`, đặt cạnh ô vừa chạm, tiêu đề = tên kỹ năng (Core trả `szTen`). *Gắn ô chính* → như trên. *Gắn ô phụ* → ô đã chọn trước (chạm ô trống) thì gắn luôn; không thì ô trống đầu tiên; hết ô thì nhớ kỹ năng, báo "chạm ô phụ muốn thay" (chạm ô nào gắn ô đó). *Gỡ khỏi ô* → xoá khỏi mọi ô phụ đang giữ nó; ô chính chỉ thay được. Thông tin kỹ năng vẫn hiện như cũ (con trỏ đứng trên ô khi chạm). Bảng tự đóng khi chạm ra ngoài hoặc cửa sổ kỹ năng đóng. `KSdlApp` hỏi bảng **trước** mọi thứ khi đặt ngón. |
| *"không cho lấy kỹ năng lên tay khi bấm vào xem thông tin"* | Trên Android `OnSkillPickDrop` (nhấc lên) **không bao giờ nhấc nữa** — chỉ gán (chế độ gán) hoặc mở bảng 3 nút. |
| *"ô phụ bỏ kỹ năng buff thì chỉ cần kích vào sẽ tự sử dụng"* | Core `GDI_KYNANG_MOBILE` trả thêm `nTuDung` = không nhắm kẻ địch mà lên mình/đồng đội (`!IsTargetEnemy && (IsTargetSelf ‖ IsTargetAlly ‖ style InitiativeNpcState)`). Chạm → `UseSkill` ngay tại chỗ nhân vật (giữa khung vẽ), không cần kéo ngắm, không cần có địch; một lần mỗi chạm. Trước đây buff không có mục tiêu địch nên **không bắn gì cả**. |
| *"các ô phím phụ nếu bỏ kỹ năng vòng sáng thì sẽ tự luân chuyển đối với phái Nga My"* | **Chưa làm — chưa rõ ý** ("vòng sáng" = kỹ năng hào quang/trạng thái? "tự luân chuyển" = tự đổi qua lại giữa các hào quang, hay tự quay về kỹ năng đánh sau khi bật?). Đã hỏi lại chủ. |
| *"chỉ làm cho bản mobile"* | UI kỹ năng guard `JX_ANDROID`; Core chỉ thêm trường/tính thêm, không đổi hành vi PC. PC x64 + SDL dựng sạch. |
| *(14:0x) "các nút khi kích vào kỹ năng có sẵn ở VNKU rồi, thêm vào thôi — chế thêm nhìn không rõ chữ"* | `va_nguon_android_66.py`: bảng chọn dùng đúng **4 nút ảnh VNKU** `\Spr\UiNew\UiSkills\nut_phim_chinh / nut_phim_phu / nut_go_phim_chinh / nut_go_phim_phu.spr` (236×86, 3 khung; vẽ thu nửa 118×43 bằng `VeAnhKhung`), bỏ chữ tự vẽ. *Gỡ chính* = trả ô chính về kỹ năng đầu danh sách đánh trái (cơ bản) và quên `Chinh=`. Ảnh chép vào `spr/uinew/uiskills/` (lớp ghi đè). |
| *(14:2x) "Ô phụ bỏ kỹ năng vòng sáng thì tự luân chuyển (Nga My): 3 kỹ năng vòng sáng ở ô phụ tự đổi qua luân phiên liên tục + hiệu ứng vòng tròn xoay tại các ô đó"* | `va_nguon_android_67.py`. **Luân chuyển:** mỗi `[Cham] LuanChuyenMs` (mặc định 3000) đổi sang vòng sáng kế tiếp trong các ô phụ (`GOI_SET_IMMDIA_SKILL` nParam 1 → `SetRightSkill` → `SetAuraSkill`); một vòng sáng thì chỉ giữ cho nó bật. **Phát hiện quan trọng:** `KPlayer::SetRightSkill(kỹ năng thường)` gọi `SetAuraSkill(0)` = **tắt vòng sáng mỗi lần đánh** → bỏ bước SetRightSkill khi đánh/buff từ ô phụ (`UseSkill`/`LockSomeoneUseSkill` nhận mã kỹ năng trực tiếp, không cần). Core `GDI_KYNANG_MOBILE` nhận `nParam 1` = *chỉ hỏi* (không xử lên/xuống ngựa) để hỏi "aura?" mỗi khung an toàn; hai chỗ hỏi tầm đánh để vẽ cũng chuyển sang chỉ hỏi. **Vòng xoay:** bộ vẽ không xoay được ảnh → `lam_vong_xoay.py` làm sẵn 16 khung quay 22,5° từ `effect_skill.spr` → `spr/ui3/uiskillcontrol/vong_xoay.spr`, vẽ khung `(tick/62) % 16` (~1 vòng/giây như `CCRotateBy(1s,360)` của bản tham khảo). Gỡ một vòng sáng đang bật → tắt (đặt kỹ năng phải = kỹ năng đánh chính). |
| *"các nút nhỏ lại và nằm bên cạnh thông tin — không cần hiển thị tên kỹ năng của bạn chế"* | 4 nút VNKU thu 84×31, bỏ tên + nền; `KMouseOver::JxLayKhung()` (thêm) trả khung thông tin đang hiện → 4 nút xếp dọc **ngay mép phải khung thông tin** (hết chỗ thì mép trái). APK `jx1mobile-1009-luanchuyen.apk`. |
| *(14:3x) chủ: "vòng sáng luân chuyển bản tham khảo có sẵn, tìm ra mà viết theo"; "khinh công vẫn chưa dùng được"; "đọc code để làm, không được đó mò"* | `va_nguon_android_68.py`. **Đọc lại bản tham khảo:** (a) luân chuyển = `KuiAutoPlay::DoSkillBuff()` (KuiAutoPlay.cpp:5240): mỗi `MAX_SKILLAURA_COUNT = 15` nhịp (≈0,5 s) "bấm" ô vòng sáng **kế tiếp theo thứ tự ô** (`auxiliaryskillCallback` → `SetAuraSkill`) → đổi mặc định `LuanChuyenMs` 3000 → **500**. (b) Khinh công: nhật ký của chủ cho thấy ta thả về **(520,662)** — ngoài màn hình (hướng cần điều khiển = 0, không ép dẹt) và lặp mỗi 200 ms cắt ngang cú nhảy. Bản tham khảo `KgameWorld.cpp:4387` (R158/R171): `bPointCast = !Enemy && !Ally && !Self && !Obj && !Other`; hướng = **`Npc.m_Dir`** (0 xuống, 16 trái, 32 lên, 48 phải), góc `(-90 − 5,625·dir)°`; tầm theo **cấp đã học** (≥ 60); điểm = `(W/2 + cos·0,9·r, H/2 − sin·0,45·r)`; đang kéo cần thì theo hướng cần. Core trả thêm `nHuong`, `nTamCap`; nhảy **một lần mỗi chạm**. Thêm: đang cưỡi ngựa thì Core cho xuống ngựa và client **giữ lệnh tới 6 s**, xuống xong tự thả (TIME_RIDE 5 s). APK `jx1mobile-1009-khinhcong.apk`. |
| *(14:4x) chủ: "khinh công được rồi nhưng chưa chọn hướng nhảy được, nó mặc định theo hướng nhìn"* | `va_nguon_android_69.py`: `JxKyNang_Nhip` bắn **phát đầu ngay khi đặt ngón** (hợp với đánh liên tục) nên khinh công nhảy luôn theo hướng nhìn rồi thôi. Nay kỹ năng **cần điểm** không bắn lúc đặt ngón; **nhả ngón mới nhảy** — có kéo ngắm (vạch/mũi tên vẫn hiện) thì nhảy theo điểm ngắm, không kéo thì theo hướng nhìn. Chờ xuống ngựa vẫn giữ lệnh như vá 68. APK `jx1mobile-1009-huongnhay.apk`. |
| *chủ: "Vòng sáng luân chuyển: KuiAutoPlay::DoSkillBuff() là trong AUTO của USVOLAM, còn đây là phím kỹ năng; bản này đã có Auto đấu"* | **Đã đọc hết**: ngoài `DoSkillBuff` (Auto), bản tham khảo **không có** vòng lặp đổi vòng sáng cho phím phụ: `auxiliaryskillCallback` (KgameWorld.cpp:4362) chỉ `SetAuraSkill` khi bấm; `KPlayer::SetDefaultImmedSkill` (KPlayer.cpp:3396) lúc vào game bấm lần lượt mọi phím phụ có vòng sáng (cái cuối cùng bật); `KgameWorldVN.cpp:8642` chỉ bật/tắt **vòng xoay** ở ô có vòng sáng **đang bật**; khối "nhiều vòng sáng" trong Core (`m_IsMoreAura`/`m_TmpAuraID`, KNpc.cpp:1143) **đã bị comment**, chỉ còn thuộc tính `skill_appendskil` của kỹ năng 120 nạp danh sách. Ghi chú R112f "game tự luân chuyển" của tác giả bản đó không có mã tương ứng. Vì thế bản mobile của ta giữ thuật toán 0,5 s/ô theo thứ tự ô (chính là `DoSkillBuff` đem ra khỏi Auto); **vòng xoay chỉ vẽ ở ô đang bật** cho đúng `KgameWorldVN.cpp:8642` — chờ chủ quyết. |
| *chủ: "PC chơi Tống Kim không lag, mobile vào Tống Kim lag không di chuyển được"* | Chưa vào được Tống Kim để đo; số liệu sẵn có trên máy ảo (`jx_paint.log`/`jx_rep3.log`): bình thường 53 khung/s nhưng có giây tụt còn **5–11 khung/s** đúng lúc nạp sprite (`[REP3-NAP] rút khung 4770 lần/30 s = 2,4 s CPU trên luồng vẽ`, `[PDET] render 40–55 ms, end tới 105 ms`). Tống Kim = hàng trăm người chơi × trang bị khác nhau → giải mã/tạo texture dồn lên luồng vẽ → khung rơi → "không di chuyển được". Máy ảo chạy **x86_64 native** (APK có cả arm64 lẫn x86_64), không phải dịch nhị phân. Việc đã làm: tắt `PaintLog`/`AutoLog` trong `config.ini` Android và xoá `jx_net_trace.on` (dấu vết mạng đã ghi **169 MB** `jx_net_sdl.log` — mỗi gói tin một dòng, Tống Kim nhiều gói càng nặng). **Cần đo tiếp khi vào Tống Kim**: bật lại `PaintLog=1` 1 phút, so `[SEC] painted` và `[REP3-NAP]` với lúc ở thành; kiểm tra LDPlayer cấp mấy nhân CPU / RAM (PC dùng cả máy, máy ảo mặc định 2 nhân). Hướng sửa nếu đúng nghi vấn: đẩy giải mã sprite người chơi khác sang luồng nạp nền ([NAP 08/09 b] đã có) triệt để hơn, hạ chi tiết người chơi xa. |
| *"kỹ năng khinh công bỏ phím phụ sử dụng không được"* (lần 1) | Khinh công (210): `TargetEnemy/Self/Ally` đều 0, `AttackRadius` 400 → kỹ năng **cần một điểm**; chạm không ngắm thì mã cũ đi tìm kẻ địch → không có → bỏ qua. Core trả thêm `nCanDiem`; chạm không ngắm → thả về **phía đang đi / quay mặt** (`s_nHuong` của cần điều khiển) cách nhân vật min(tầm, 360) — như bản tham khảo dùng hướng joystick. `HorseLimit=1`: đang cưỡi thì Core cho xuống ngựa, lần chạm sau mới thả (như mọi kỹ năng dưới ngựa). APK `jx1mobile-1009-nutvnku.apk`. |

### 12.6. Còn lại thật sự


- **Nút nhặt đồ nhanh** và **nút bật/tắt auto** trên HUD: bản tham khảo có (`nhatnhanh.png`,
  `autoplay.png`); bản này chưa có nút chạm riêng (vẫn nhặt được bằng cách chạm vào món đồ).
- **Thời gian hồi chiêu** chưa vẽ trên nút (Core vẫn tự chặn, chỉ là không nhìn thấy).
- **Sắp xếp lại thứ tự ô** trong cụm: hiện dời được **cả cụm**; muốn đổi chỗ từng ô với nhau
  thì phải thêm.
- Chủ hỏi *"mũi tên xanh dài"*: hiện dùng `attack_direction.spr` (nhiều khung theo hướng).
  Nếu ý chủ là một ảnh khác thì cho tôi đường dẫn, đổi bằng `config.ini [Cham] KyNangAnhTen`
  là xong, không phải dựng lại.


---

## 7. (09/09) ĐIỀU KHIỂN BẰNG NGÓN TAY

> Bản vá nguồn: `android/va_nguon_android_12.py`. APK đã kiểm: `android/apk/jx1mobile-0909-cham-c.apk`.
> Đây là mục 1 của `LOTRINH_MOBILE_0909.md`.

Trước đó game chỉ nhận chạm qua kiểu "giả lập chuột" của SDL, tức là **một ngón = chuột trái, hết**.
JX1 thì sống bằng chuột phải (đánh ép, menu chuột phải) và bằng rê chuột (thông tin vật phẩm, tên NPC).

### Bộ nhận cử chỉ (`KSdlApp::ChamSuKien` / `NhipCham`)

Chặn các sự kiện chuột **do ngón tay sinh ra** (`which == SDL_TOUCH_MOUSEID`) rồi dịch lại:

| Cử chỉ | Thành | Đã đo |
|---|---|---|
| Chạm nhanh rồi nhả | Chuột trái **tại chỗ đặt ngón** | ✔ bấm nút "TRỞ LẠI" đóng được bảng |
| Chạm hai lần liền (≤400 ms, ≤24 px) | Bấm đúp (dùng vật phẩm) | chưa kiểm tay |
| Chạm rồi kéo đi (>12 px) | Giữ chuột trái từ chỗ đặt ngón rồi rê → đi chuyển liên tục, kéo–thả | ✔ nhân vật đi |
| **Giữ tại chỗ ≥400 ms trên BẢN ĐỒ** | **Chuột phải** (đánh ép, chọn mục tiêu) | ✔ hiện bảng mục tiêu người chơi |
| Giữ tại chỗ trên **GIAO DIỆN** | **Không** bấm chuột phải; nhả ngón vẫn bấm chuột trái như thường | ✔ nhật ký `[CHAM]` + nút ấn 900 ms vẫn ăn |

**Vì sao chạm nhanh phải đợi đến lúc NHẢ mới bấm:** nếu bấm ngay lúc đặt ngón thì không phân biệt được
với "giữ để bấm chuột phải" — sẽ bấm trái rồi lại bấm phải, trên NPC là mở thoại rồi đánh. Đợi đến lúc
nhả (thường dưới 150 ms) là cách mọi giao diện cảm ứng đều làm.

**Vì sao giữ trên giao diện lại không sinh chuột phải:** chuột phải trong túi đồ là **dùng vật phẩm** —
bấm nhầm là mất đồ. `Wnds.cpp` thêm `JxUi_CoGiaoDienTaiDiem(x, y)` (dùng chính `Wnd_GetActive` mà game
vẫn dùng để dò trúng) để phân biệt. Nhưng **nhả ngón vẫn bấm chuột trái**: trên điện thoại người ta hay
ấn nút lâu hơn 400 ms, không thể vì thế mà nút chết.

### Bàn phím ảo

`KSdlApp::Init` **không** còn gọi `SDL_StartTextInput` sẵn — trên điện thoại lệnh đó **đẩy bàn phím ảo lên
ngay từ màn hình chính và không tắt cho tới lúc thoát** (trên LDPlayer có bàn phím cứng nên không lộ).
Nay `KWndEdit::WndProc` gọi `JxSdl_BanPhimAo(1)` ở `WND_M_SET_FOCUS` và `(0)` ở `WND_M_KILL_FOCUS`.
Đo bằng `adb shell dumpsys input_method`: mở dòng chat → `mInputShown=true`; bấm ESC → `mInputShown=false`.

### Nút Back của máy

`SdlKeyToVk` thêm `SDLK_AC_BACK → VK_ESCAPE` (manifest đã đặt `SDL_ANDROID_TRAP_BACK_BUTTON=1` nên SDL
đưa nút này vào game thay vì thoát app). Đo: bấm Back → mở bảng THOÁT / GIÚP ĐỠ / TÙY CHỌN / TRỞ LẠI.

### Số có thể chỉnh (đầu `KSdlApp.cpp`, phần `#ifdef JX_ANDROID`)

| Hằng | Đang là | Ý nghĩa |
|---|---|---|
| `CHAM_GIU_MS` | 400 ms | giữ bao lâu thì thành chuột phải |
| `CHAM_NGUONG` | 12 px | xê dịch quá bao nhiêu thì coi là kéo |
| `CHAM_HAI_MS` / `CHAM_HAI_XA` | 400 ms / 24 px | hai lần chạm thế nào thì là bấm đúp |

### Còn lại của mục này

- Kéo–thả vật phẩm giữa các ô túi và bấm đúp để dùng đồ: **chưa kiểm bằng tay** (chỉ mới suy ra từ thiết kế).
- Hai ngón: phóng to / kéo bản đồ (`SetZoom` đã có sẵn trong `autoexec.lua`).
- Nhật ký `[CHAM]` còn bật, mỗi lần giữ ngón ghi một dòng — gỡ khi phát hành.


---

## 8. (09/09 tối) THAO TÁC TRONG GAME BẰNG NGÓN TAY — ĐỢT HAI

> Bản vá: `android/va_nguon_android_13..19.py`. APK đã kiểm: `android/apk/jx1mobile-0909-nutto.apk`.
> Nguồn tham khảo (**chỉ đọc, không sửa**): `D:\USVOLAM` (mã) và
> `C:\Users\nguye\Downloads\NHACTAI\VNKU_ui` (ảnh `.spr`). Xem `KEHOACH_GIAODIEN_MOBILE_0909.md`.

### 8.1. Một lỗi thật và nặng: `GetKeyState` trên Android luôn trả 0

`KPosixWin32.cpp` có sẵn móc `g_pfnJxGetKeyState` nhưng **chưa ai nối vào**. Hệ quả: mọi chỗ trong
game hỏi "đang giữ Shift/Ctrl/Alt không" đều trả lời KHÔNG — tức là Ctrl+chuột phải (menu người
chơi), Shift+chuột trái (đánh ép), Alt+chuột (bạn đồng hành, biểu cảm) **đều chết, kể cả khi cắm bàn
phím rời**. Nay nối vào bảng phím thật của SDL, kèm một *mặt nạ phím dính* (`JxSdl_DatPhimDinh`) để
lớp chạm hoặc một nút ảo sau này giữ hộ phím bổ trợ.

### 8.2. Cần điều khiển ảo (di chuyển)

`Sources/S3Client/Platform/JxCanDieuKhien.cpp` — **kéo ngón ở nửa trái** màn hình (ngoài giao diện)
là cần điều khiển; mỗi vòng lặp gọi `iCoreShell::Goto(nDir, 0)` với `nDir` là 1 trong 8 hướng của
vòng 64 hướng — **đúng cách bản JX1 Mobile của chủ làm** (lớp `HRocker`). Lúc bắt đầu kéo thì
`LockSomeoneAction(0)`/`LockObjectAction(0)` để nhân vật chịu rời chỗ.

Vẽ bằng **ảnh thật của VNKU**: `\spr\Ui3\UiSkillControl\joystick_bg.spr` + `joystick_ctrl.spr`
(đã chép vào `D:\jx1_android_data\spr\ui3\uiskillcontrol\`). Thiếu ảnh thì tự lùi về vẽ ô màu.
Chỉnh trong `config.ini [Cham]`: `CanDieuKhien / CanVungRong / CanVungTren / CanVungDuoi /
CanBanKinh / CanNguong / CanAnhNen / CanAnhNum`.

**Chạm vẫn là chuột trái như thường** nên không mất thao tác nào của bản PC.

### 8.3. Chạm đối tượng → thanh thông tin → chạm thanh → danh sách tuỳ chọn

Đây là đường chủ chỉ theo bản mobile. Đọc kỹ thì **hai phần đầu đã có sẵn** trong bản này
(`KUiTargetInfo` + `PopUpContextPeopleMenu`), chỉ thiếu mắt xích thứ ba → **không phải động vào
Core** (bản USVOLAM thêm hẳn thông báo `GDCNI_SHOW_NPC_BAR` vào Core để làm việc này).

**Bẫy đã đo tận mắt:** mục `[Main]` của `kuitargetinfo.ini` chỉ là một ô **27×23**, còn thân thanh
(`BackGround0`, `Head0`…) là các ô **con nằm ngoài** ô đó. Nên chạm vào thân thanh **rơi ra ngoài
cửa sổ**, xuống bản đồ, bỏ chọn mục tiêu thay vì mở menu. Phải bật cờ `WND_S_SIZE_WITH_ALL_CHILD`
cho cửa sổ dò trúng theo cả đám con. Danh sách đặt **ngay dưới** thân thanh (lấy khung bao cả đám
con), không đè lên thanh.

### 8.4. Chữ hỏng trong menu — **lỗi nằm trong nguồn, không phải lỗi phông**

Bảng `g_ActionName` (`UiGame.cpp`) có byte **0xE6** (chữ *ổ*) bị thay bằng `?` **và mất luôn dấu
cách**:

| Trong nguồn (hỏng) | Đúng ra là |
|---|---|
| `"T?<đ><ộ>i"` | `"Tổ đội"` |
| `"S?<đ>en"` | `"Sổ đen"` |
| `"O<ẳ>n t?t?"` | `"Oẳn tù tì"` |

Hai dòng đầu đối chiếu **byte-for-byte** với bản tham khảo `D:\USVOLAM` thì nay đã khớp y hệt.
Dòng thứ ba bản tham khảo không có; **chủ xác nhận chữ gốc là "Oẳn tù tì"**, và nó khớp đúng với
byte còn sót (mỗi `?` ăn một chữ + một dấu cách).

> **Đây là chỗ DUY NHẤT trong đợt này có đổi cả bản PC** — vì chuỗi hỏng cho cả hai bản.
> Sửa bằng `unicode_to_tcvn3_bytes` của `skills/swordonline-dev/scripts`, không gõ tay byte nào.

### 8.5. Cho dễ chạm

- **Dòng trong danh sách tuỳ chọn**: nới hai cái **đệm** (`byItemTitleUpSpace` / `nItemTitleIndent`)
  chứ **không** nhân thẳng `nItemHeight/nItemWidth` — làm vậy chữ mới còn nằm giữa dòng.
  Đang để 7/9 (chữ cỡ 12 → dòng cao ~26 px). Bản đầu để 14/18, chủ bảo **quá to**.
- **Nút nhỏ**: `KWndButton::PtInWindow` nới **vùng bắt chạm** (không đổi cách vẽ, không đụng bố cục)
  cho nút nhỏ hơn 26 px, nới nhiều nhất 6 px mỗi bên để hai nút kề nhau không cướp chạm của nhau.
- **Vuốt dọc trên giao diện = cuộn danh sách** (dịch thành lăn chuột): `WndList`, `WndList2`,
  `WndMessageListBox` đều nhận `WM_MOUSEWHEEL` nên thoại NPC / chat / danh sách máy chủ đều vuốt
  được. Vuốt **ngang** vẫn là giữ chuột trái rồi rê (kéo cửa sổ đi chỗ khác).

### 8.6. Cử chỉ ngón tay — bảng đầy đủ hiện nay

| Cử chỉ | Thành | Đã đo |
|---|---|---|
| Chạm nhanh | Chuột trái tại chỗ đặt ngón | ✔ |
| Chạm hai lần liền | Bấm đúp (dùng vật phẩm) | chưa kiểm tay |
| Kéo ở **nửa trái**, ngoài giao diện | **Cần điều khiển** (đi chuyển) | ✔ ảnh |
| Kéo ở chỗ khác trên bản đồ | Giữ chuột trái rồi rê (đi liên tục) | ✔ |
| Kéo **dọc** trên giao diện | Cuộn danh sách | ✔ khung thoại NPC |
| Kéo **ngang** trên giao diện | Giữ chuột trái rồi rê | — |
| **Giữ tại chỗ ≥400 ms** (bản đồ hoặc giao diện) | **Chuột phải** — đánh ép / chọn mục tiêu; trong túi đồ là **mặc / tháo / dùng** vật phẩm | ✔ |
| Nút Back của máy | ESC (mở bảng hệ thống) | ✔ |
| Ô nhập có tiêu điểm | Bàn phím ảo bật; mất tiêu điểm thì tắt | ✔ `dumpsys` |

### 8.7. Hệ tự căn chỉnh giao diện (`FitFlags`) — có, nhưng **opt-in**

Xem `KEHOACH_GIAODIEN_MOBILE_0909.md`. Tóm tắt: mang từ USVOLAM, nhưng **mặc định tự suy ra neo**
(một phần ba đầu → bám lề trên/trái, một phần ba cuối → bám lề dưới/phải) thay vì "dịch cả khung
vào giữa" của họ, vì màn hình điện thoại **thấp hơn** khung chuẩn 768 chứ không cao hơn. Và **chỉ
neo cửa sổ nào tự đăng ký** — đã thử áp đại trà rồi **đo thấy hỏng** (bản này tự chỉnh sẵn nhiều
cửa sổ theo `SCREEN_WIDTH/HEIGHT` trong mã → neo lại là chỉnh hai lần).

### 8.8. Chủ test buổi sáng — làm theo thứ tự này

APK mới nhất: `android/apk/jx1mobile-0909-suakeo.apk` (đã cài sẵn trên LDPlayer).

1. Mở app → phải **vào thẳng bản đồ** (nhớ mật mã + tự đăng nhập).
2. **Kéo ngón nửa trái màn hình** → hiện vòng cần điều khiển, nhân vật đi theo hướng.
3. **Chạm giữ** một đối tượng → **vòng tròn hiện dưới chân nó** và thanh thông tin hiện ở trên;
   **chạm vào thanh đó** → ra danh sách
   *Tán gẫu / Hào hữu / Oẳn tù tì / Tổ đội / Theo sau / Cứu sát / Tin tức / Sổ đen*.
   → Xem chữ đã đúng chưa, dòng đã vừa tay chưa.
4. Mở túi đồ → **chạm giữ** một món → phải **dùng / mặc** được nó.
5. Bấm **Back** của máy → bảng hệ thống; chạm *Trở lại* để đóng.
6. Bấm Enter (hoặc chạm ô chat) → **bàn phím ảo phải bật**; bấm ESC → phải tắt.
7. **Chạm một NPC** (Dã Tẩu, NPC Chuyển Sinh…) → khung thoại mở, các dòng lựa chọn giãn rộng;
   **vuốt dọc trong khung** → danh sách cuộn.
8. Nhìn quanh: NPC đối thoại gần nhất có **chấm tròn vàng trên đầu**.
9. Chạm **biểu tượng thư** (mép phải, dưới búa đấu giá) → hộp thư mở, đọc được thư, nhận được
   vật phẩm đính kèm. Chạm **búa đấu giá** → cửa sổ đấu giá. Chạm **Chiến Lệnh** (trên đầu) →
   cửa sổ chiến lệnh. *(Trước đây cả ba đều không mở được — xem §9.)*
10. **Nút kỹ năng đánh** (6 ô góc phải dưới): chạm một ô → ô đánh trái trên thanh trạng thái
    đổi theo. **Ra khỏi thành**, chạm một ô → phải đánh con gần nhất; **giữ ô rồi kéo** về
    phía một con khác → vạch chỉ hướng chuyển **đỏ**, con đó có **vòng tròn dưới chân**, thả
    ngón là đánh đúng con đó. *(Phần đánh trúng quái tôi CHƯA thử được — quanh chỗ test không
    có quái. Xem §11.3.)*
11. Chỉnh giao diện: `config.ini [Ui] SuaToaDo=1` (đã bật sẵn) → nút **"Sửa giao diện"**
    (nay nằm **ngay dưới tiểu bản đồ**, không còn ở giữa mép phải vì chỗ đó giờ là cụm nút kỹ
    năng) → hàng nút *Dời ô / Dời khối / To hơn / Nhỏ lại / Giấu hiện / Danh sách / Lưu / Xoá
    hết*. Chọn công cụ rồi chạm vào thứ cần sửa, xong bấm **Lưu**.
    → Kéo **từng icon một** phải dịch riêng nó, không kéo cả cụm (§12.6).
    → **Cụm nút kỹ năng** cũng kéo được trong chế độ này.
    *(Muốn bản phát hành không có nút này thì để `SuaToaDo=0`.)*
12. **Cụm nút kỹ năng** (góc phải dưới): một nút đánh chính to + 8 ô phụ xếp thành cung
    (ô trong 60 px, ô ngoài 48 px).
    - **Chạm/đè** một ô → đánh, đè thì đánh liên tục.
    - **Đè rồi kéo** → ô xanh ngắm chạy theo hướng, có vòng tầm đánh và mũi tên; thả ngón là
      thả kỹ năng ở đó. Nút đang giữ có **núm nghiêng** theo hướng (không còn vạch đỏ).
    - Kỹ năng chỉ dùng được **dưới ngựa** mà đang cưỡi → **tự xuống ngựa** rồi thôi phát đó,
      bấm lại là đánh.
    - Kỹ năng **trợ** (nội công) → chạm là **bật**.
    - **Hai ngón cùng lúc**: một ngón giữ cần để chạy, ngón kia đè nút kỹ năng để đánh.
13. **Tự chọn kỹ năng cho từng ô**: chạm nút **mũi tên vòng tròn** (bên trái cụm) → chạm ô
    (ô sáng vàng) → mở bảng kỹ năng (chạm ô kỹ năng đánh trên thanh trạng thái) → chạm một
    kỹ năng ⇒ vào ô đó. Nhớ luôn cho lần sau (`UserData\KyNangMobile.ini`).
14. Đi bộ: dưới chân có **mũi tên nhỏ chỉ hướng đang đi**, luôn nằm phía trước.
15. (10/09) Chế độ sửa giao diện, công cụ **"Dời ô"**: kéo **một** ô kỹ năng phụ → chỉ nó dịch; công cụ
    **"Dời khối"**: kéo → cả cụm dịch. Lưu → `UserData\UiToaDo.ini` có dòng `KyNang<n>=x,y`.
16. (10/09) Bấm **"Xoá hết"** → giao diện về **đúng bố cục anh đã sắp** (không về bản PC). Sắp lại tuỳ ý
    rồi bấm **"Mặc định"** → từ đó "Xoá hết" về bố cục mới đó.
17. (10/09) HUD theo bố cục VNKU: hàng icon tròn có nhãn dưới thanh trạng thái (bấm Nhân vật → mở bảng
    nhân vật…; Ngồi là nút bật/tắt), cột phải dưới bản đồ nhỏ: Trao đổi / PK / Auto, Lên ngựa, Chạy và
    **4 ô vật phẩm 2×2** (phím số 1–4, chạm dùng được; ô 5–9, T/P, túi mở rộng, ẩn chat đã bỏ), khung
    chat nhỏ giữa đáy (gõ được, chọn kênh, biểu cảm, gửi), nút "Sửa giao diện" giờ ở (200,40) dưới thanh
    trạng thái bên trái. Chỗ nào chưa ưng → kéo trong chế độ sửa giao diện rồi bấm "Mặc định".

### 8.9. Vòng tròn dưới chân đối tượng — **đã làm**

Chủ: *"bên mobile kích vào đối tượng là có vòng tròn dưới chân đối tượng nhằm cố định lại để hiện
thông tin"*. Bản vá `android/va_nguon_android_20.py`.

**Không đi đường của bản USVOLAM** (`SetInstantSpr(enumINSTANT_STATE_SELECT_NPC)` trong Core), vì:

- `KNpcRes` chỉ có **một** khe ảnh phụ (`m_cSpecialSpr`) và đang dùng chung với hiệu ứng kỹ năng
  (`KNpc.cpp:3383`), lại tự tắt sau một lúc → vòng sẽ chớp tắt và cướp chỗ của hiệu ứng kỹ năng;
- bảng `\settings\NpcRes\player_instant_special_file.txt` của bộ dữ liệu này **đang hỏng** (đường
  dẫn cụt `\spr\skill\`, không có tên tệp) nên `SetInstantSpr` cũng không lấy được ảnh.

Đường đã đi, gọn hơn và không đụng vào hệ vẽ của Core:

1. **Core** — `NPC_OI_TARGET_INFO` trả thêm **vị trí vẽ** của mục tiêu (toạ độ thế giới, lấy từ
   `KNpc::GetDrawPos`) và cờ *đang khoá* (`m_nPeopleIdx == idx`). Ba trường thêm vào
   `KUiTargetDetailInfo` đều trong `#ifdef JX_ANDROID` nên **cỡ struct bên PC không đổi**.
2. **Client** — `JxVongChon_Ve()` vẽ ảnh vòng ngay tại vị trí đó bằng `DrawPrimitives` với
   `bSinglePlaneCoord = FALSE`, tức là **toạ độ thế giới**, nên Represent3 tự đặt đúng chỗ và vòng
   bám theo dù màn hình cuộn đi đâu. Ảnh lùi lại nửa khung để nằm giữa chân (bản dựng đầu chưa lùi
   thì vòng lệch xuống dưới bên phải — đã nhìn tận mắt rồi mới sửa).

Ảnh dùng của bộ VNKU (đã chép vào thư mục dữ liệu):
`\spr\npcres\focused_non_enemy_circle.spr` và `focused_enemy_circle.spr` (dùng khi mục tiêu đang
bị khoá đánh). Chỉnh trong `config.ini [Cham]`: `VongChon` / `VongChonAnh` / `VongChonAnhDich`.

### 8.10. Thoại NPC: chạm mở được, dòng cao hơn, vuốt cuộn được

Chủ: *"kích vào npc phải hiện các dòng chat với npc để dễ kích vào - vuốt lên xuống được"*.

**Đo trước khi sửa:** chạm **đúng** dòng thì thoại chạy tiếp, nhưng chạm lệch 8 điểm ảnh là trượt.
Dòng thoại **không phải** `KWndButton` (nên không hưởng phần nới vùng chạm ở §8.5) mà là **mục của
`KWndMessageListBox`** — khung thoại NPC là `KUiMsgSayNew` với `KScrollMessageListBox`.

Bước dòng của lớp đó tính bằng `m_nFontSize + 1` ở **10 chỗ** khác nhau (vẽ, dò trúng, cuộn). Gom hết
về **một** hàm `CaoDong()` rồi trên Android cộng thêm 5 → dòng cao **18** thay vì 13 điểm ảnh. Vì vẽ,
dò trúng và cuộn đều dùng chung một hàm nên **không bao giờ lệch nhau**. Bản PC: `CaoDong()` trả về
đúng `m_nFontSize + 1` → không đổi một điểm ảnh nào.

> Muốn dòng cao/thấp hơn: sửa số 5 trong `Sources/S3Client/Ui/Elem/WndMessageListBox.h` (`CaoDong`).

**Đã đo tận mắt:** chạm NPC *Dã Tẩu* → khung thoại mở với các dòng lựa chọn giãn ra rõ; **vuốt dọc**
trong khung → danh sách **cuộn**, dòng thứ ba (*"Ta bận rồi, không rảnh ngồi tán gẫu với ông"*) hiện ra.
Đây cũng là phép thử xác nhận cơ chế **vuốt = lăn chuột** ở §8.5 chạy đúng.

### 8.11. Tới gần NPC thì hiện icon trên đầu — **đã làm**

Chủ: *"tới gần npc nào phải hiện icon để kích vào chọn đối thoại hay không"*.
Bản vá `android/va_nguon_android_22.py`.

Chạm vào NPC thì thoại **đã mở được sẵn** (§8.10), nên icon này để người chơi **biết chỗ nào chạm
được** — và vì nó nằm ngay trên đầu NPC nên chạm vào icon cũng là chạm trúng NPC.

- **Core** — dùng lại mã số `NPC_OI_TARGET_INFO` với `nParam == 1` = *"trả về NPC đối thoại gần
  nhất"* thay vì mục tiêu đang chọn. **Không thêm mã số GDI mới**: thêm vào giữa enum sẽ đẩy mọi mã
  số phía sau lệch đi. Quét bằng `NpcSet.GetNextIdx`, lọc `kind_dialoger`, lấy cái gần nhất theo
  khoảng cách vẽ.
- **Client** — `JxIconNpc_Ve()` đổi toạ độ thế giới → màn hình bằng
  `iRepresentShell::CoordinateTransform` rồi vẽ icon cao hơn chân NPC `IconNpcCao` điểm ảnh.
  NPC nào ra ngoài khung vẽ thì không vẽ (tức là chỉ hiện cho NPC đang nhìn thấy).

`config.ini [Cham]`: `IconNpc` / `IconNpcAnh` / `IconNpcCao`.

> **Chọn ảnh mất ba lượt** — ghi lại để khỏi thử lại: `\spr\Ui3\UiGui\NpcTalk.spr` là **ảnh minh
> hoạ 711 KB** (che góc màn hình), `nut_theo_doi.spr` và `giao_tiep.spr` là **nút chữ dài**. Đang
> dùng `\spr\obj\box\YellowPoint.spr` (896 byte) — đúng một **chấm tròn nhỏ** như chủ tả.
> Đổi ảnh khác thì sửa `IconNpcAnh`, không phải dựng lại.

**Đã đo tận mắt:** chấm vàng hiện trên đầu NPC lính gần nhất.

**Còn có thể làm thêm:** hiện chỉ đánh dấu **một** NPC gần nhất. Muốn đánh dấu mọi NPC đối thoại
đang nhìn thấy thì cho Core trả về một **danh sách** thay vì một cái.

### 8.12. Còn lại (theo lời chủ, chưa làm)

- **Tới gần NPC hiện icon để chạm chọn đối thoại** (`UiNpcBar` bên USVOLAM, 350 dòng).
- **Nút chọn kỹ năng** riêng cho mobile (`UiMiniSkill`, 1141 dòng + `UiAssignSkill.ini` +
  `\spr\Ui3\UiSkillControl\assign_skill_*.spr`).
- **Gửi / lấy vật phẩm** (`UiGive`, 1068 dòng).
- **Bàn phím ảo trong game** (`UiVirtualKeyboard`, 353 dòng) — hiện đang dùng bàn phím hệ thống.
- Kéo–thả vật phẩm và bấm đúp: **chưa kiểm bằng tay**.

---

## 13. (11/09) VÒNG SÁNG CHƯA LUÂN CHUYỂN + LAG TỐNG KIM TRÊN MOBILE (bản vá 71–73)

> Nhánh: `mobile-0809` = commit cũ `cbe40fdb` (va 70) + **4 commit mới** (va 71, 72, 73, bàn giao). **Chưa rebase lên `origin/main`**
> mới (`7c13d0a8`, 12 commit phiên tối ưu 09/09: LOCTG b, CHUGIU, DON/SANGTAT, WORLD b/c, NAPCHIEU, NAPNPC, NPCRES) — chủ
> rebase theo quy trình mục 5 khi thấy tiện; đã thử rebase một lần: xung đột duy nhất ở `KRepresentShell3.cpp` (định nghĩa
> `g_uRep3LocKhung`: giữ khối biến của main, giữ định nghĩa ở tệp này cho Android), sau đó dựng lại hai chuỗi Windows.
> Bản vá: `android/va_nguon_android_71.py` (vòng sáng), `_72.py` (bộ đệm đọc pak), `_73.py` (atlas SDL_GPU + bỏ bản CPU + ngân sách cache).
> Tất cả chỉ trong tệp/rào `JX_ANDROID` / `JX_POSIX` / `JX_PLATFORM_SDL` (và trong `JX_PLATFORM_SDL` thì mặc định **tắt** trên Windows
> `GameSDL.exe`), nên **bản PC không đổi hành vi**.
> **Chưa dựng APK, chưa chạy trên máy ảo/điện thoại** (không có NDK/LDPlayer ở đây). Đã kiểm cú pháp bằng clang + header SDL3
> + libc++ (đúng bộ NDK dùng) cho 8 tệp đã sửa: 0 lỗi.

### 13.1. Vòng sáng vẫn chưa luân chuyển — gốc thật (va 71)

Toàn bộ đường luân chuyển (va 67–70) đúng, nhưng **không bao giờ chạy** vì một lỗi bộ đệm:

- `KyNang_OLaAura(i)` hỏi Core "kỹ năng ở ô i có phải vòng sáng?" qua `KyNang_HoiCore()` rồi **ghi kết quả vào bộ đệm theo
  mã kỹ năng** (`s_uKNAuraId[i]` / `s_nKNAuraLa[i]`). `KyNang_HoiCore()` **trả về kể cả khi Core chưa trả lời được**
  (`GetGameData(GDI_KYNANG_MOBILE)` trả 0 vì `Player[CLIENT_PLAYER_INDEX].m_nIndex == 0` lúc **chưa vào game**) → `nAura = 0`
  bị ghi vào bộ đệm **vĩnh viễn** cho mã đó.
- `JxKyNang_Nhip()` (và trong đó `KyNang_LuanChuyen()`) chạy từ vòng lặp `KSdlApp::Run` **ngay từ màn hình đăng nhập**, mà
  `UserData\KyNangMobile.ini` đã có sẵn 3 vòng sáng từ phiên trước → mỗi lần mở app, bộ đệm bị đọc sai **trước khi vào game**
  → cả phiên coi 3 ô đó là "không phải vòng sáng": không luân chuyển, không vòng xoay. Đó là lý do mọi APK từ va 67 tới va 70
  đều "vẫn chưa luân chuyển" — mỗi lần cài APK mới là mở app lại với ini đã có.

**Sửa:** (1) `KyNang_OLaAura` hỏi thẳng Core và **chỉ ghi bộ đệm khi Core trả lời được** (`GetGameData` trả 1); (2)
`KyNang_LuanChuyen` chỉ làm việc **mỗi `LuanChuyenMs`** (trước: quét 8 ô + hỏi Core mỗi vòng lặp ~1 ms), **không làm gì khi
chưa vào game** (chưa có kỹ năng đánh trái), cập nhật `s_KNPhai` ngay khi đổi để **vòng xoay** chuyển theo (trước lấy từ
`KyNang_DocBang` 2 s/lần nên vòng xoay chạy chậm hơn luân chuyển 0,5 s); (3) chạm ô vòng sáng / gỡ vòng sáng cũng cập nhật
`s_KNPhai`; (4) nhật ký `[KYNANG] luan chuyen vong sang -> <mã> (k/n) lan N`: 20 dòng đầu rồi mỗi 200 lần một dòng.

**Chủ kiểm:** gán 2–3 vòng sáng Nga My vào ô phụ → **tắt app, mở lại** → vào game → mỗi 0,5 s: ô kỹ năng **phải** trên thanh
trạng thái đổi biểu tượng, vòng sáng dưới chân đổi, vòng xoay chuyển ô; logcat có `[KYNANG] luan chuyen vong sang`. Không thấy
dòng đó = Core trả `nLaAura = 0` cho kỹ năng ấy (cột IsAura trong `settings\skills.txt`), báo tôi mã kỹ năng.
Lưu ý: trong **Tống Kim** máy chủ cấm vòng sáng (`m_bTongForbidAura`, `KProtocolProcess::ChangeAuraSkill`) nên không thử ở đó.

### 13.2. Lag Tống Kim: cùng một bản, PC 144 fps đều, mobile lag — phân tích

Đã đọc lớp vẽ SDL_GPU (`D3D9onGPU*.cpp`), đường nạp sprite (`TextureRes*.cpp`, `XPackFile.cpp`), mạng SDL (`SocketClient.cpp`),
chữ (`KFont3.cpp`) và số liệu cũ của máy ảo (§12.14). Bốn chỗ **mobile khác PC** và đều tăng theo số người trên màn hình:

| # | Chỗ | Mobile | PC |
|---|---|---|---|
| 1 | **Đọc pak** (`XPackFile::GetSprFrame` → `DirectRead`) | mỗi khung sprite = 1 seek + 1 read xuống hệ thống tệp; trên LDPlayer dữ liệu nằm ở **thư mục chia sẻ** `/mnt/shared/Misc` → mỗi lần đọc là một chuyến máy ảo ↔ máy thật (~0,3–0,5 ms). Số liệu cũ: `[REP3-NAP] rút khung 4770 lần/30 s = 2,4 s trên luồng vẽ` | bộ đệm trang Windows, ~20 µs |
| 2 | **Texture GPU** (`D3D9onGPUDev/Res.cpp`) | **không có atlas**: mỗi khung / mỗi ô cắt của sprite = một `SDL_GPUTexture` riêng (vkCreateImage + cấp bộ nhớ + upload + barrier) và một lệnh vẽ riêng; Tống Kim = hàng nghìn texture mới mỗi giây trên Vulkan, LDPlayer còn dịch Vulkan sang máy thật → `[PDET] end tới 105 ms` | D3D11 có atlas 1024×1024 từ 08/09 (`D3D9on11Atlas.cpp`), vài trăm texture GPU thay vì hàng vạn |
| 3 | **RAM** | lớp SDL_GPU giữ **bản CPU của mọi texture** → RAM = 2× cache; `config.ini` Android chép `Rep3CacheMB=1500` từ PC 32 GB → máy ảo 2–4 GB / điện thoại thiếu RAM (hệ thống giết app hoặc máy ảo tráo trang) | 32 GB RAM, VRAM riêng |
| 4 | Giải mã RLE khung + logic Core (tick NPC) trên **cùng luồng vẽ** | CPU máy ảo/điện thoại yếu hơn nhiều | — |

Đã **loại trừ**: mạng (vòng đọc socket SDL `select` + `recv` không giới hạn thông lượng; đã đo 80 gói/s RTT 1,7 ms) và chữ
(`KFont3` dùng **một** texture chung cho cả font, vẽ theo lô — không phải mỗi chữ một texture).

### 13.3. Đã sửa (chỉ mobile) — 3 công tắc, đều A/B được không cần dựng lại

| Bản vá | Việc | Tắt để A/B |
|---|---|---|
| **72** `XPackFile.cpp` (`JX_POSIX`) | **Bộ đệm khối đọc pak**: 512 khối × 64 KB (32 MB) LRU trong `DirectRead`; khung kề nhau của cùng sprite nằm cùng khối → lấy từ RAM. Yêu cầu > 64 KB (tệp nguyên, chỉ mục) đọc thẳng. Đóng pak thì bỏ khối của handle đó. Chạy trong khóa `ms_ReadCritical` sẵn có. Nhật ký logcat (`SDL/APP`) mỗi 30 s: `[PAK] 30s: doc N lan (trung khoi A, truot B, doc thang C) \| xuong dia D lan X MB Y ms \| khoi dang giu 512 x 64 KB` | tạo tệp `<thư mục dữ liệu>\jx_pak_khoi.off` |
| **73** `D3D9onGPU*.cpp/.h` | **Atlas SDL_GPU**: texture `POOL_DEFAULT` ≤ 512 px (sprite, không RT/DYNAMIC) gom vào trang 1024×1024 cùng định dạng (R8G8 bảng màu / BGRA8), xếp kệ theo lớp chiều cao **y hệt** `CAtlasMgr` của D3D11; uv nhân/dịch khi chép đỉnh vào ring (`RgAtlasUv`); quad kề nhau cùng trang được **gộp lệnh** như trước; trả chỗ **sau khung** (`m_atlasFrees`), trang rỗng thứ hai mỗi lớp trả GPU. Texture bị huỷ giữa khung tự rút khỏi `m_touched` (trước đây là con trỏ treo tiềm ẩn). | `[Client] Rep3AtlasGpu=0` |
| **73** | **Bỏ bản CPU** của texture DEFAULT sau khi lệnh tải đã submit (`FrameReset`) → RAM ≈ 1× cache. `LockRect` sau đó (hiếm: sprite chỉ ghi một lần) đọc lại từ GPU (`ReadbackRegion`, đồng bộ) — đếm `doc lai N` trong dòng `khung ...` của `jx_rep3.log`. | `[Client] Rep3GpuBoBanCpu=0` |
| **73** `TextureResMgr.cpp` (`JX_POSIX`) + `config.ini` | Ngân sách cache: `Rep3CacheMB=0` (tự tính **RAM/8 kẹp 128–512 MB**, rồi `CapBudgetByVram` kẹp theo "VRAM" ảo 1024 MB của SDL_GPU); số ép trong ini vẫn được tôn trọng nhưng **kẹp ≤ RAM/3** (có dòng log `[REP3] cache texture: ... qua lon ... -> kep`). Hệ số 1,3× cho trang atlas áp cả cho `Rep3Api=100`. | đặt lại `Rep3CacheMB=<số>` |

Thống kê để đối chiếu (`jx_rep3.log`): dòng `atlas: BAT ... \| bo ban CPU sau khi tai len: BAT` lúc tạo thiết bị; dòng
`khung N: lenh ve .., quad .., tai texture .., texture GPU X (Y MB) \| atlas P trang (Q MB) \| bo ban CPU S texture (T MB), doc lai R`
mỗi 1800 khung; dòng `[REP3] ... gpu tex X (Y MB, P trang Q MB)` mỗi `Rep3StatSec` (30 s).

### 13.4. Chủ đo — theo thứ tự này

1. Dựng APK (§4), vào thành: `jx_rep3.log` phải có `atlas: BAT`; **hình ảnh sprite / UI phải y như cũ**. Lệch uv, ô đen, viền lạ
   → `Rep3AtlasGpu=0` rồi so; vẫn lỗi → `Rep3GpuBoBanCpu=0`. Báo tôi công tắc nào gây lỗi.
2. Vào **Tống Kim 1 phút** với `PaintLog=1`, gửi `jx_paint.log` (`[SEC] painted`, `[PDET]`), `jx_rep3.log` (`[REP3-NAP]`,
   `khung ...`), logcat (`[PAK]`). Số cần xem: `rút khung` ms/30 s và `[PAK] trung khoi` (I/O), `lenh ve`/`tai texture` mỗi khung
   (GPU), `RAM rieng` (RAM), `khung >16 ms`.
3. **Thử nhanh không cần dựng** (chỉ máy ảo): chép dữ liệu vào bộ nhớ trong của máy ảo (`adb push D:\jx1_android_data /sdcard/jx1`
   + trỏ `jx_data_dir.txt`, xem 686c08cd) → nếu hết lag thì thủ phạm chính là I/O thư mục chia sẻ (điện thoại thật không bị).
4. LDPlayer: cài đặt máy ảo ≥ 4 nhân CPU, ≥ 4 GB RAM (mặc định 2 nhân).

### 13.5. Còn lại / rủi ro

- **Bước 3 chưa làm:** giải mã RLE khung sprite (`TextureResSpr::PrepareFrameData` → `RenderToIndexAlpha`) vẫn trên luồng vẽ;
  đưa sang luồng nền (`TextureResMgr::NapNenChay`) như đã làm với tệp SPR (NAP 08/09 b) — làm sau khi có số đo bước 2.
- Atlas: texture ảo gán ở **stage 1** hoặc sampler WRAP với uv ngoài [0,1] sẽ sai (D3D11 trên PC cũng vậy, chưa thấy xảy ra).
- Bỏ bản CPU: nếu `doc lai N` tăng đều mỗi khung → có chỗ ghi lại texture DEFAULT sau khi vẽ (không mong đợi), báo tôi.
- Bộ đệm pak: 32 MB RAM thêm; nếu `[PAK] trung khoi` thấp (< 50 %) thì đổi `XP_KHOI_CO`/`XP_KHOI_SO` trong `XPackFile.cpp`.
- Chưa có số đo runtime nào cho ba bản vá này — mọi kết luận "hết lag" phải chờ log của chủ.


## 14. (11/09 chiều) LAG TỐNG KIM MOBILE — GỐC ĐÃ TÌM ĐƯỢC, ĐÃ SỬA (gộp `main` + bản vá 74)

> Nhánh `mobile-0809` = `a0766ee6` (mục 13) + **merge `main` `e952d0e9`** (`9f0d4eae`, 16 commit tối ưu 09/09) + `va_nguon_android_74.py` (`032b4835`).
> Merge chứ không rebase (một xung đột `KRepresentShell3.cpp`, hai khối: biến toàn cục + đọc ini — giữ cả hai bên; `g_uRep3LocKhung` vẫn định nghĩa ở `KRepresentShell3.cpp` như mục 10 vì `D3D9on11Dev.cpp` không dịch trên Android).
> Hai APK trong `android/apk/` (đều arm64-v8a + x86_64, đã soi chuỗi đánh dấu trong `.so` của cả hai ABI):
> - `jx1mobile-1109-vongsang-va73.apk` (15:47) = chỉ va 71–73 — **để chủ test vòng sáng trước** (mục 13.1).
> - `jx1mobile-1109-tongkim-va74.apk` (16:09) = merge main + va 74.
> **Chưa cài lên máy ảo**: lúc dựng, game của chủ đang chạy trong Tống Kim trên máy ảo (pid 14429, ~1.100 NPC/khung) — không cài đè, không dựng chuỗi Windows trong lúc đó (luật 5.4; máy chủ chạy trên chính máy này).

### 14.1. Bốn khác biệt mobile ↔ PC, xác nhận bằng mã + log

| # | Chỗ | Bằng chứng | Sửa |
|---|---|---|---|
| 1 | **Cần điều khiển ảo gửi lệnh đi bộ MỖI VÒNG LẶP** — chỉ có ở mobile | `KSdlApp::Run` gọi `JxCan_Nhip()` mỗi vòng (8 ms; 1 ms khi PaintFps > 60) → `Goto` → `KPlayer::Walk` (`KPlayer.cpp:896`) → `SendClientCmdWalk` **không có cổng gác** → 125–1000 gói `c2s_npcwalk`/giây khi giữ cần. Máy chủ với mỗi gói: `KNpc::Goto/NewPath` + `DoWalk` phát `NPC_WALK_SYNC` cho **cả vùng + 8 vùng kề** (`KNpc.cpp:3733`). PC giữ chuột: 1 gói / 5 tick (`GotoWhere`, `defMAX_PLAYER_SEND_MOVE_FRAME`), `Goto(nDir)` trên PC chỉ có Lua `MoveTo` gọi. `jx_net_sdl.log` cũ: 43 giây 21–40 gói gửi/s, 11 giây > 40/s. Trong Tống Kim nhân theo số người → lag cho mình và cho trận. | va 74: `JxCore_GotoHuong(huong, mode, nBuoc, bEp)` trong `CoreShell.cpp` (chỉ `JX_ANDROID`, `extern "C"`, libmain gọi thẳng): cổng gác 5 tick như chuột PC, đích xa 8 bước (`[Cham] CanBuocXa`, 2 = như cũ); đổi hướng → gửi ngay (≤ 1 lần/110 ms); nhả cần / ngón về giữa → **một** lệnh đích gần 2 bước để dừng (đúng như trước). Cảm giác di chuyển giữ nguyên, số gói giảm ~30–300 lần. |
| 2 | Nhánh mobile **thiếu 16 commit tối ưu của `main`** | VUNG (`FindNpc` chỉ mục ô→NPC — PC đo vòng ĐẠN 20–35 ms/tick khi 500–1.100 đạn), NAPCHIEU/NAPNPC (nạp trước ảnh chiêu / thân NPC ở luồng nền → bớt nạp trên luồng vẽ), DON/SANGTAT (bỏ `RenderLightMap` 0,5 ms/khung), CHUGIU. Tick 20–35 ms trên PC = gấp nhiều lần trên máy ảo. | Đã merge. DOLUOT (`SuspendThread`/`GetThreadContext`/`dbghelp`/`_beginthreadex`, lớp `JX_POSIX` không có) rào `#ifndef JX_POSIX` trong `KSubWorld.cpp`, Android hai hàm rỗng. `GetModuleHandleA("Represent3.dll")` của NAPCHIEU chạy được trên Android (lớp tương thích đổi thành `dlopen("libRepresent3.so")`, `Rep3_NapTruoc2` có trong `libRepresent3.so`). |
| 3 | **Present mode**: LDPlayer/điện thoại không có IMMEDIATE | `[GPU] Reset: ... vsync=1` dù config `vsync=0` → `SDL_WaitAndAcquireGPUSwapchainTexture` đợi vblank + lớp dịch máy ảo: `[PDET] end=16–28 ms` mỗi khung. | va 74: MAILBOX khi máy có (`[Client] Rep3GpuMailbox=1`, chỉ `JX_ANDROID`; dòng `[GPU] thiet bi: ... trinh chieu mailbox`). Chưa biết LDPlayer có MAILBOX không. |
| 4 | I/O pak qua thư mục chia sẻ, texture không atlas (35.000 texture GPU / 184 MB), RAM 2× | **Đo sống 16:00–16:12 trong Tống Kim, APK cũ 14:41 (va 70)**, bảng 14.2 | va 72 + 73 (mục 13), có trong cả hai APK |

### 14.2. Số đo sống trong Tống Kim trên máy ảo (APK cũ va 70, `jx_rep3.log`, mỗi dòng = 30 s)

| fps | NPC vẽ/khung | khung > 16 ms | khung max | rút khung (I/O pak) | CPU vẽ ms/khung |
|---|---|---|---|---|---|
| 58,1 | 1.100 | 0 | 8 ms | 750 lần / 443 ms | 2,6 |
| 55,9 | 893 | 5 | 88 ms | 783 / 467 ms | 2,4 |
| **39,0** | 531 | **81** | **230 ms** | **5.311 / 3.253 ms** | 8,3 |
| 55,3 | 1.300 | 3 | 60 ms | 1.094 / 667 ms | 3,4 |
| **31,1** | 887 | **29** | **279 ms** | **3.362 / 2.326 ms** | 9,2 |
| 53,8 | 1.356 | 1 | 19 ms | 230 / 153 ms | 2,6 |
| 58,1 | 1.221 | 0 | 2,5 ms | 98 / 60 ms | 2,3 |

Đọc: khi **không** phải nạp sprite mới, 1.100–1.350 NPC vẽ/khung vẫn 54–58 fps, CPU vẽ 2–3 ms → **không nghẽn CPU vẽ, không nghẽn logic** (`top`: tiến trình game 28 % một nhân, 364 % nhàn). Lag = những cửa sổ có người/chiêu mới xuất hiện: mỗi khung sprite rút từ pak mất **0,6–0,7 ms** (đọc qua thư mục chia sẻ), 3–5 nghìn lần trong 30 s = **2,3–3,3 giây đứng luồng vẽ**, kèm tạo texture GPU rời (2.000–2.900 lệnh vẽ/khung, 35.000 texture GPU). Đúng chỗ va 72 (đọc theo khối) + va 73 (atlas) + NAPCHIEU/NAPNPC (nạp trước ở luồng nền) nhắm vào. `anh_null` 430–480 nghìn/30 s (~250/khung) = ảnh NPC thiếu trong bảng NpcRes (đã có bộ đệm âm, chỉ tra bảng băm — không đáng kể; `kiem_npcres_thieu` của main sửa dữ liệu).

Loại trừ (đã đọc mã): chữ (`KFont3` một texture), âm thanh (`KWavSound::Load` nạp một lần rồi giữ), mạng phía nhận. `jx_paint.log` cũ còn có khung **3–4 giây** và tick 3,1 s ở map vắng (npc 6/tick) → nghẽn ngoài game (máy thật đang dựng DLL, `Game.exe` PC chạy cùng lúc chiếm GPU host). **Đo trên LDPlayer khi máy thật đang dựng là vô nghĩa.**

### 14.3. Chủ đo — thứ tự

1. Cài `jx1mobile-1109-vongsang-va73.apk` → kiểm vòng sáng (13.1: tắt app, mở lại, `[KYNANG] luan chuyen vong sang`).
2. Cài `jx1mobile-1109-tongkim-va74.apk`. `jx_rep3.log` phải có `atlas: BAT` và `trinh chieu mailbox` (hoặc `vsync` nếu máy ảo không có MAILBOX). Đi bằng cần 30 giây: phải đi liền mạch, đổi hướng tức thì, nhả cần dừng trong ~0,1 s. Dừng giật giữa chừng → tăng `[Cham] CanBuocXa` (10–12); muốn y như cũ: `CanBuocXa=2`.
3. Tạo `jx_net_trace.on`, giữ cần 10 giây → `jx_net_sdl.log` `[send]` phải ~4 dòng/giây (trước: 40–125).
4. Vào Tống Kim 1 phút với `PaintLog=1` → gửi `jx_paint.log` (`[SEC]` tickmax/ticksum, `[WORLD-TICK]`, `[SPIKE]`), `jx_rep3.log` (`rút khung`, `khung >16 ms`, `[GPU] khung`), logcat `[PAK] trung khoi`. Không dựng gì trên máy thật trong lúc đo; tắt `Game.exe` PC.
5. Hình ảnh lệch (uv sai, ô đen) → `Rep3AtlasGpu=0`; xé/giật lạ → `Rep3GpuMailbox=0`; RAM → `Rep3GpuBoBanCpu=0`. Báo công tắc nào gây lỗi.

### 14.4. Còn lại / rủi ro

- Chưa chạy va 74 trên máy ảo (game chủ đang chạy). Chưa có số đo sau sửa.
- Đích xa 8 bước: máy chủ tìm đường tới điểm 8 bước như chuột PC; sát vật cản có thể lượn quanh như PC.
- `chuan_bi_du_lieu.ps1` chưa chạy lại: `D:\jx1_android_data\config.ini` đã được sửa tay đúng như lớp ghi đè (`Rep3CacheMB=0`, `Rep3AtlasGpu=1`, `Rep3GpuBoBanCpu=1`, `Rep3GpuMailbox=1`, `[Cham] CanBuocXa=8`, `PaintFps=-1`).
- Hai chuỗi Windows (`build_chuoi_x64.ps1`, `build_chuoi_sdl.ps1`): 0 lỗi sau va 74 (xem 14.5).

### 14.5. (11/09 chiều–tối) Đã cài, đã đo trên máy ảo — kết quả thật (va 74 → 77)

> Chủ: "build xong cài luôn", "bạn phải mở log để có thông số để biết mà fix chứ không dự đoán". Từ đây mọi kết luận đều có số đo kèm.
> Commit: `9f0d4eae` (merge main) → `032b4835` (va 74) → `d2e3d667` (va 75) → `945c04e0` (va 76) → `b3acd3cf` (va 77) → va 78 (HUD chi phí thật, nền mờ, chữ có dấu).
> APK cuối: **`android/apk/jx1mobile-1109-tongkim-va78.apk`** (= va 71–78 + main; các APK va73/74/75/76/77 chỉ giữ để đối chiếu). Đang cài trên máy ảo.
> Hai chuỗi Windows (`build_chuoi_x64.ps1`, `build_chuoi_sdl.ps1`) dựng lại sau va 74, 76 và 77: **0 lỗi** cả ba lần.

**Va 74 trong Tống Kim (cùng trận, cùng máy ảo, APK cũ va 70 → va 74), `jx_rep3.log`, mỗi dòng 30 s:**

| | APK cũ (va 70) | va 74 (bộ đệm khối pak + atlas + main) |
|---|---|---|
| rút khung (I/O pak) | 3.362–5.311 lần / **2,3–3,3 s** (0,65 ms/lần) | 5.836–9.523 lần / **0,2–1,0 s** (0,04–0,1 ms/lần) |
| khung > 16 ms mỗi 30 s | 29–81 | 0–21 (chỉ 30 s đầu khi vào map) |
| khung lâu nhất | 230–279 ms | 108–150 ms lúc vào map, sau đó 2–8 ms |
| fps TB | 31–58 | 55–61 (1.000–1.400 NPC vẽ/khung) |
| `[PAK]` | — | trúng khối 9.472 / 10.496 lần đọc (90 %) ở 30 s đầu, 97–99 % sau đó |
| tick logic (`[WORLD b]`, PaintLog=1) | (không đo) | 0,5–1,1 ms/tick, 3,6–13 µs/NPC, vòng đạn 0,05 ms → **không nghẽn logic** |

**"Rung cửa sổ game" (chủ báo sau va 74):** đo A/B với `PaintLog=1`:
- Lần A (va 74: MAILBOX bật, cần đích xa 8 bước): `[SUM]` cách khung 11/16/30 ms, 0 spike/10 s; `[PDET] end` đa số < 8 ms.
- Lần B (va 76: vsync, cần đích gần 2 bước + 1 tick): cách khung 3–7/16–17/29–70 ms, 2–11 "spike" (≥ 25 ms, phần lớn là **đợi vblank** chứ không phải giật); `[PDET] end` 14–29 ms.
- Số đo CPU của A "đẹp" hơn B, nhưng **chủ xác nhận hết rung ở B**: `[SUM]`/`[SPIKE]` đo thời gian CPU, không đo nhịp *trình chiếu*; MAILBOX đưa khung lên màn không khớp 60 Hz → khung hiện hai lần / bỏ một khung = rung khi cuộn. Kết luận: **`Rep3GpuMailbox=0` mặc định** (va 75), giữ công tắc để A/B. Cần điều khiển về đích gần 2 bước như cũ nhưng tối đa **1 gói/tick** (`CanBuocXa=2`, `CanGacTick=1`; ~18 gói/giây thay vì 125–1.000; muốn như chuột PC: `8` + `5`).

**Còn lại "vào map đông vẫn lag" (chủ báo sau va 76) — là cửa sổ 30 s đầu khi vào map** (`[REP3-NAP]`): tệp spr 370–660 lần / 0,3–0,5 s, rút khung 6–9 nghìn lần / 0,2–0,6 s, giải mã 13 nghìn khung / 35 ms, tạo GPU 28 ms, `nen: giao 620 xong 346 hong 99 bo_ve 252` → 4–12 khung > 16 ms, lâu nhất 108–150 ms, rồi hết. Đây là **nạp đồng bộ trên luồng chính** (SPR mới của người/chiêu vừa xuất hiện) — việc tiếp theo: nạp trước theo map (khi nhận gói vào Tống Kim, xếp hàng nền toàn bộ ảnh giáp/vũ khí/chiêu hay gặp) hoặc cắt nạp đồng bộ theo lát ≤ 4 ms/khung. Chưa làm.

**Khởi động lại ngay sau khi đóng app → "GameInit thất bại" 1–2 lần** (logcat 06:27:42, 06:27:45, 06:28:30, 06:28:33; Android tự mở lại): lần hỏng chỉ có `[REP3] Rep3Api=11 nhưng không tạo được D3D11 -> lùi về D3D9` = `Rep3Ini("Rep3Api", 11)` trả mặc định dù `config.ini` có `Rep3Api=100` → tệp không đọc được đúng lúc đó (chưa rõ vì sao — `config.ini` có mtime 16:28:45 nhưng không mã nào của client ghi nó). Va 76: Android **ép `Rep3Api=100`** (chỉ có một bộ vẽ) + lớp JX_POSIX ghi `[INI] khong mo duoc <tep>: errno ...` để lần sau có số. Sau va 76 chưa thấy lặp lại.

**Vòng sáng:** va 71 đã xác nhận chạy (`[KYNANG] luan chuyen vong sang -> 89/332/92/712 (1..4/4)` mỗi 0,5 s; giờ 300 ms `LuanChuyenMs`). Vòng xoay trên ô: chủ chê dày → dùng `VNKU_extract\circle.png` (vòng mỏng 40 px, chép vào lớp ghi đè `spr/ui3/uiskillcontrol/circle_vnku.png`), `lam_vong_xoay.py` mặc định lấy ảnh đó (vòng dày cũ: `python android\lam_vong_xoay.py effect_skill.spr`).

**Bảng đo trong game (va 77 → 78, chủ yêu cầu "FPS – pin – GPU như USVOLAM", "có thẩm mỹ để phát hành"):** `Sources/S3Client/Platform/JxPerfHudAndroid.cpp` (chỉ Android; `Ui/PerfHud.cpp` của Windows dùng psapi/pdh nên bị loại). Nền mờ (`spr/ui3/uiskillcontrol/hud_nen.spr` kéo căng) + ba dòng chữ có dấu, mặc định **giữa màn hình, ngay dưới hàng nút menu** (`[Client] PerfHud=1`, `PerfHudX/PerfHudY`, -1 = tự tính; góc trái-dưới đè lên nhật ký chat, giữa-trên đè lên thanh máu — đã thử cả hai): `FPS | vẽ x,x ms | logic x,x ms | nhịp 16,5 ms (tệ nhất N) | ping`, `CPU % (nhân) | RAM game, máy còn | pin % đang sạc`, `GPU driver | texture MB (số) | atlas trang | vsync/mailbox | lệnh vẽ, quad`. **`vẽ`/`logic` là chi phí thật** đo trong `S3Client.cpp` quanh `UiPaint` và `Breathe+UiHeartBeat` (chủ nhìn `khung 16,5 ms` của va 77 tưởng chi phí vẽ cao — đó là nhịp 60 Hz). Số GPU từ Represent3 qua `Rep3_ThongKeGpu`. Ảnh đã xem: `FPS 56  vẽ 3,1 ms  logic 0,0 ms  nhịp 16,5 ms (tệ nhất 30)  ping 25 ms` ở map vắng; lúc vừa vào map đông: `FPS 37  vẽ 7,6 ms  logic 0,5 ms  nhịp 17,4 ms (tệ nhất 243)`, `lệnh vẽ 2439, quad 1871`.

**Cấu hình sống `D:\jx1_android_data\config.ini` hiện tại** (đã đồng bộ với lớp ghi đè, trừ hai khoá đo): `PaintLog=1`, `AutoLog=1` (đang bật để đo — **tắt lại khi chơi thật**), `Rep3CacheMB=0`, `Rep3AtlasGpu=1`, `Rep3GpuBoBanCpu=1`, `Rep3GpuMailbox=0`, `PerfHud=1`, `[Cham] CanBuocXa=2 CanGacTick=1 LuanChuyenMs=300`.

### 14.6. Trả lời của phiên tối ưu PC (17:0x) — đã đối chiếu với máy ảo

- **Tick logic PC trước VUNG**: 35 tick ≥ 20 ms / 30 s, gốc `KRegion::FindNpc` duyệt tuyến tính cả `m_NpcList` cho mỗi ô hỏi (~1,2 triệu lần đọc `Npc[]`/tick khi 1.000 đạn). **Sau VUNG**: tick ≥ 20 ms = 0, đạn 0,03–2,6 ms/tick. Máy ảo sau merge đo được cùng kết luận (0,5–1,1 ms/tick).
- Phần còn lại của tick trên PC: `KIpotBranch::AddPointLeafToList` ≈ 45 % (O(n) mỗi lần NPC/đạn đổi vị trí), `PluckRto` 4–6 %, `KNpc::Activate` gọi 6 hàm `Set*` + `strcat` mỗi tick/NPC 2–3 %, heap 4–8 %. Trên máy ảo chưa lộ (tick < 1,1 ms) — ghi lại để khi tick tăng thì biết cắt ở đâu.
- **NAPCHIEU/NAPNPC chạy được trên Android** (phiên PC tưởng `GetModuleHandleA` trả NULL): lớp `JX_POSIX` đổi thành `dlopen("libRepresent3.so")`; `jx_rep3.log` máy ảo: `[NAPNPC] nap truoc (bat=1): goi 182 | da co 141, giao nen 41 | kip 6, tre 1`.
- **DOLUOT**: đã rào `#ifndef JX_POSIX` trên `mobile-0809`; đã nhắn phiên PC rào **đúng cùng cách** trên `main` để lần merge sau không xung đột. CHUGIU dịch bình thường (QPC có trong lớp tương thích).
- **Dữ liệu NpcRes** phiên PC sửa trên cây live (`bin/client/settings/NpcRes`: 40.361 ô trỏ SPR không tồn tại → đội mũ trọc đầu, nạp hỏng hàng vạn lần/30 s): đã chép **14 bảng khác** sang `D:/jx1_android_data/settings/npcres/` (17:05; `chuan_bi_du_lieu.ps1` chạy lại cũng ra như vậy). Kỳ vọng `anh_null` (430 nghìn/30 s) và `nen: hong` giảm mạnh ở lần vào map sau.
- Phiên PC xác nhận 144 fps đo với `Game.exe` live cũ + `CoreClient`/`Represent3` đã có VUNG/NAPCHIEU/NAPNPC, `PaintFps=144`, `PaintVsync=1` — tức bản PC cũng chỉ mượt khi có các commit này; mobile trước merge không có.
- **Luồng nạp nền chạy trên Android** (phiên PC nghi không chạy): cửa sổ vào Tống Kim `nen: giao 1291 xong 681 hong 191 bo_ve 434`, sau đó `giao 157 xong 155 hong 1`. Khác biệt 370–660 lần nạp đồng bộ (mobile) so với 5–11 (PC) là do **cache lạnh**: máy ảo bị cài lại rồi tự đăng nhập thẳng vào map Tống Kim (cửa sổ 30 s thứ 1–2 sau khi mở app), mọi giáp/vũ khí/chiêu đều là lần chạm đầu; PC vào Tống Kim sau khi đã chơi map khác. `hong 99–191` = NpcRes thiếu (bảng đã đồng bộ ở trên sẽ giảm). Luồng nền trễ lúc đó (`[NAPNPC] kip 0, tre 14`) vì 1 tệp ≈ 0,8 ms trên thư mục chia sẻ.
- PC cũng có cache lạnh, nhẹ hơn nhờ SSD (mở game 16:41 rồi vào map: `tep spr 256 lan 74,8 ms`, 2 khung > 16 ms, `nen: giao 614 xong 449 hong 13`, NAPNPC trễ 54 %). **Phân công đã thống nhất với phiên PC** cho 30 s đầu vào map: (b) đệm header SPR (tên → cỡ, số khung) và (c) cắt lát nạp đồng bộ ≤ 4 ms/khung là mã chung Represent3 → phiên PC làm trên `main`, mobile merge; (a) giữ màn nạp map tới khi hàng nạp nền rỗng (trần ~2 s) → phiên mobile làm khi chủ giao, qua một hàm xuất `"C"` của Represent3 (số việc nền còn lại) + chờ ở màn nạp, báo phiên PC trước khi chạm `TextureResMgr`. Chưa ai bắt đầu.
- Gợi ý của phiên PC cho việc còn lại (30 s đầu vào map): nạp trước toàn bộ NPC có mặt ngay trong màn nạp (móc `SetSprFile` đã có) và để ý `GetImageParam` trong `KNpcRes::Draw` (nạp đồng bộ trên luồng vẽ khi `m_bChange`).

### 14.7. (11/09 tối) Năm yêu cầu thao tác — chỉ mobile (va 79–83, APK `android/apk/jx1mobile-1109-tongkim-va83.apk`)

| Chủ nói | Làm | Đã kiểm |
|---|---|---|
| *"đợi phiên PC xử lý xong (gộp lệnh vẽ) rồi áp vào"* | Chưa làm gì; khi `main` có commit gộp lệnh vẽ thì merge sang `mobile-0809` (xem 14.6). | — |
| *"phím số 1–4 bỏ item vào: chạm vào là sử dụng luôn, đè vài giây là lấy ra trên tay để bỏ vào rương; PC để nguyên"* | `UiPlayerBar.cpp` (`JX_ANDROID`): `WND_N_ITEM_PICKDROP` dạng nhấc (tay không) trên `m_ImmediaItem[i]` → `OnUseItem(i)`; `WND_N_RIGHT_CLICK_ITEM` (chạm giữ) → `OnObjPickedDropped(&Pick, NULL)` = nhấc lên tay khi tay không. Thả đồ đang cầm xuống ô vẫn như cũ. PC: trái nhấc, phải dùng, không đổi. | Chưa thử tay (cần vật phẩm trong ô). Chủ thử: chạm ô có thuốc → dùng ngay; giữ ~0,5 s → thuốc lên tay, chạm rương để bỏ. |
| *"gỡ kỹ năng khỏi ô thì ô phải trống, hiện tại nó tự add kỹ năng khác vào"* | Gốc: ô phụ chưa tự gán thì `KyNang_CuaNut` lấy kỹ năng thứ i của danh sách đánh (`s_KNBang`) → gỡ xong lại hiện cái kế. Nay `s_nKNTrong[8]` (`Trong<i>=1` trong `UserData\KyNangMobile.ini`): "Gỡ khỏi ô" gỡ cả kỹ năng đang hiện theo danh sách và đặt cờ trống; ô có cờ = trống thật; gán lại thì xoá cờ. | Chủ thử: gỡ một kỹ năng ở ô phụ → ô trống (vòng "+"), tắt mở app vẫn trống. |
| *"kích vào người chơi hiện thông tin chưa cố định, dí chuột đi chỗ khác là đổi mục tiêu"* | `KPlayer.cpp` (`JX_ANDROID`, client): `g_nJxMucTieuKhoa = m_nPeapleIdx` lúc `OnButtonDown`; `OnMouseMove` (hover giả lập, kéo cần, kéo cửa sổ) giữ `m_nPeapleIdx` = mục tiêu đã khoá; bỏ khoá khi NPC biến mất hoặc chạm chỗ khác/chạm đất. | Chủ thử: chạm người chơi → thanh thông tin; kéo cần đi → thanh vẫn giữ người đó; chạm đất → tắt. |
| *"đứng gần NPC hiện icon giao tiếp ở kho VNKU, bấm vào tự đi tới NPC mở hội thoại"* → *"icon này thay (doi_thoai.spr)"* → *"chỉ hiện khi đứng gần, phía góc phải, đưa vào phần chỉnh toạ độ"* → *"kèm tên rút gọn NPC ở dưới"* → *"khoảng cách ngắn thôi vì nhiều NPC đứng gần nhau"* | `JxIconNpc_Ve`: icon `\spr\UiNew\UiToolsControlBar\doi_thoai.spr` (86×86, 2 khung nhấp nháy 0,5 s; đã chép vào lớp ghi đè `spr/uinew/uitoolscontrolbar/`), vẽ 44 px **cố định** (mặc định `W-200, H/2+40`), đăng ký `UiToaDo` khoá `IconNpc` → kéo được trong "Sửa giao diện", vị trí lưu `UserData\UiToaDo.ini`; chỉ hiện khi NPC đối thoại gần nhất (Core `NPC_OI_TARGET_INFO` nParam 1) cách giữa khung vẽ ≤ `[Cham] IconNpcGan` (mặc định 130 px); tên NPC cắt 12 ký tự + ".." ngay dưới. Chạm icon: `JxIconNpc_Cham` dò điểm bấm trúng thân NPC bằng `iCoreShell::FindSelectNPC` (chân−40/55/25/70/12/85, không bị giao diện che) → `KSdlApp` bấm chuột vào đó = đi tới + mở thoại. **Bẫy đã đo:** điểm "chân−28" rơi mép chân → chỉ đi tới không mở thoại; NPC nằm dưới bản đồ nhỏ → cú bấm rơi vào cửa sổ. | Máy ảo: icon + "Võ Lâm Truyề.." hiện góc phải khi đứng cạnh NPC; chạm icon → `[ICON] cham icon giao tiep -> bam NPC tai x,y` → đi tới, mở thoại. |

`config.ini [Cham]`: `IconNpc=1`, `IconNpcAnh`, `IconNpcRong=44`, `IconNpcGan=130` (đã đồng bộ lớp ghi đè + dữ liệu sống). Hai chuỗi Windows dựng lại sau va 79 và va 83: 0 lỗi.

### 14.8. (11/09 tối) Thanh thông tin mục tiêu — khoá theo người đã chạm, dời góc phải, thu 70 % (va 84, APK `jx1mobile-1109-tongkim-va84.apk`)

Chủ: *"kích vào người chơi hiện thông tin chưa cố định, lỡ dí chuột đi chỗ khác là đổi mục tiêu — đúng là kích vào sẽ tự bám theo người chơi đã kích"*; *"icon hiển thị thông tin người chơi nhỏ lại và di chuyển xuống góc trống bên phải màn hình, dưới mấy icon"*.

- **Vì sao va 79 chưa đủ:** thanh lấy mục tiêu ở `NPC_OI_TARGET_INFO` (nParam 0): `idx = m_nPeopleIdx`, nếu đang hover (`GetTargetNpc()` = `m_nPeapleIdx`) thì lấy hover. Va 79 chỉ ép `m_nPeapleIdx` trong `OnMouseMove`, nhưng **kéo ngón = `WM_MOUSEMOVE` kèm `MK_LBUTTON` → `OnButtonMove`**, không qua `OnMouseMove`; thanh lại chỉ làm mới mỗi 1 s.
- **Va 84:** khoá áp **tại chỗ hỏi** (`CoreShell.cpp`, `JX_ANDROID`): có `g_nJxMucTieuKhoa` hợp lệ thì `idx` = nó, bất kể hover; `OnButtonDown` chỉ đặt khoá khi chạm **trúng** NPC/người (chạm đất không bỏ khoá); NPC biến mất (`m_dwID == 0` / `m_RegionIndex < 0`) thì bỏ. Bỏ đoạn ép `m_nPeapleIdx` trong `OnMouseMove` để hover/`Walk` y như PC.
- **Dời + thu nhỏ:** `KUiTargetInfo` là cửa sổ ini (`kuitargetinfo.ini`, `[Main]` 370,0) nên `UiToaDo` dời/thu được, **nhưng thanh máu do Core vẽ ở toạ độ cứng** (`KNpc::PaintTargetInfo`: x 420, y 65) → dời cửa sổ thì thanh máu đứng yên. Nay UI đưa vị trí tuyệt đối + tỉ lệ cửa sổ qua `KUiPlayerPaintTeamMNG.nGocX/nGocY/nTiLe` (thêm cuối struct, `JX_ANDROID`; PC không đổi) và Core vẽ theo: `x = gốc + (420−370)·tỉ lệ`, `y = gốc + (65+33·i−10)·tỉ lệ`, rộng 120·tỉ lệ.
- **Bố cục mặc định** (`ui/uitoado_macdinh.ini`, lớp ghi đè + dữ liệu sống): `KUiTargetInfo|Main=868,300,700,0` = góc phải dưới hàng Trao đổi/Xưởng/Chạy, 70 %. Chủ kéo/thu thêm trong "Sửa giao diện" (khoá `KUiTargetInfo|Main`, lưu `UserData\UiToaDo.ini`).
- **Chưa kiểm tận mắt trên máy ảo**: lúc cài, chủ đang đánh Tống Kim (không chạm hộ). Chủ kiểm: chạm một người → thanh hiện góc phải, nhỏ; kéo cần đi chỗ khác / chạm đất → thanh vẫn giữ người đó; chạm người khác → đổi. Thanh máu phải nằm đúng trong khung; lệch → báo tôi kèm ảnh (có thể do tỉ lệ áp cho con khác gốc).
