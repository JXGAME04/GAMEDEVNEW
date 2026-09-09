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
| Kéo **dọc** trên giao diện | Cuộn danh sách | chưa kiểm tay |
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

APK: `android/apk/jx1mobile-0909-nutto.apk` (đã cài sẵn trên LDPlayer).

1. Mở app → phải **vào thẳng bản đồ** (nhớ mật mã + tự đăng nhập).
2. **Kéo ngón nửa trái màn hình** → hiện vòng cần điều khiển, nhân vật đi theo hướng.
3. **Chạm giữ** một đối tượng → **vòng tròn hiện dưới chân nó** và thanh thông tin hiện ở trên;
   **chạm vào thanh đó** → ra danh sách
   *Tán gẫu / Hào hữu / Oẳn tù tì / Tổ đội / Theo sau / Cứu sát / Tin tức / Sổ đen*.
   → Xem chữ đã đúng chưa, dòng đã vừa tay chưa.
4. Mở túi đồ → **chạm giữ** một món → phải **dùng / mặc** được nó.
5. Bấm **Back** của máy → bảng hệ thống; chạm *Trở lại* để đóng.
6. Bấm Enter (hoặc chạm ô chat) → **bàn phím ảo phải bật**; bấm ESC → phải tắt.
7. Mở một cửa sổ có danh sách (thoại NPC, danh sách máy chủ) → **vuốt dọc** để cuộn.

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

### 8.10. Còn lại (theo lời chủ, chưa làm)

- **Tới gần NPC hiện icon để chạm chọn đối thoại** (`UiNpcBar` bên USVOLAM, 350 dòng).
- **Nút chọn kỹ năng** riêng cho mobile (`UiMiniSkill`, 1141 dòng + `UiAssignSkill.ini` +
  `\spr\Ui3\UiSkillControl\assign_skill_*.spr`).
- **Gửi / lấy vật phẩm** (`UiGive`, 1068 dòng).
- **Bàn phím ảo trong game** (`UiVirtualKeyboard`, 353 dòng) — hiện đang dùng bàn phím hệ thống.
- Kéo–thả vật phẩm và bấm đúp: **chưa kiểm bằng tay**.
