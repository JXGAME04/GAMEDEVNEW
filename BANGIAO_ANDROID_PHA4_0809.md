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

### 12.10. (10/09) "Xoá cái main này, để lại các icon, thay vào cái main của VNKU" + "thanh đó nhỏ chứ không to như vậy" — **đã làm, chưa xem trong game**

Chỉ đổi **dữ liệu Android** (`ui/ui3/uiplayerbar.ini`, `uitoolscontrolbar.ini` + một ảnh mới), **không
đổi mã C++, bản PC không đổi**. Công cụ sinh: `android\thanh_duoi_mobile.py [k]`.

- Khung Main của `KUiPlayerBar` = `\Spr\UiNew\UiPlayerBar\khung_chat_new.spr` của VNKU (1343×211, hai
  con rồng + dải giấy + hai ô vuông) **thu nhỏ k = 0,5 → 672×106**, đặt **góc trái dưới** trên nền trong
  suốt 1040×604 (`spr/uinew/uiplayerbar/khung_chat_mobile.spr`, 60 KB). Bên phải để trống cho cụm kỹ năng.
  Lần dựng đầu tôi kéo khung **đầy bề ngang** (1040×191) — chủ: *"thanh đó nhỏ chứ không to như vậy"* →
  thu về 0,5 (cao ngang thanh cũ). `k = 0,6` (806×127) cũng đã dựng thử, chỉ là tham số.
- Icon **giữ nguyên ảnh cũ**, đặt lại chỗ (toạ độ màn hình, ini = màn hình − (3,1) vì chủ đặt
  `KUiPlayerBar|Main=3,1`):

  | Ô | Chỗ mới |
  |---|---|
  | Vật phẩm 1–9 (`Item_0..8`, 36 px) | trên **dải giấy**, x 162.. bước 37, y 549 |
  | Ký năng trái/phải T/P (`ImediaLeftSkill/RightSkill`) | **hai ô vuông phải** (513 / 561, 549) |
  | Chọn kênh chat (`ChannelBtn`) | **ô vuông trái** (99, 557) |
  | Ô gõ chat (`InputEdit`) 305×18, biểu cảm, gửi | **dải hoa văn trên** (162,527 / 479,525 / 509,525) |
  | 10 icon chức năng (Nhân vật…Ẩn chat) + 6 nút công cụ (Chạy…Ghi hình) | **một hàng ngay trên khung**, y 468, x 16.. bước 30 (28 px) |

- `KUiToolsControlBar` Main về (0,0) 1040×604 (trước là 450,440 200×150) — sáu nút có toạ độ tuyệt đối.
- Ảnh dựng thử (ghép icon thật lên khung, chưa chạy game): `mock_hai_co.png` đã gửi chủ.
- **Chưa xem trong game** vì đổi ini phải **khởi động lại** app mà chủ đang dùng máy ảo suốt buổi. Khi
  chủ thoát: `adb shell am force-stop vn.jx1.mobile` rồi mở lại (dữ liệu đọc thẳng từ `/mnt/shared/Misc`),
  chụp màn hình so với `mock_k50.png`. Chỗ nào lệch thì **kéo bằng chế độ sửa giao diện**, không cần dựng.
- Muốn quay về thanh cũ: xoá 3 tệp trong `android\du_lieu_ghi_de\ui\ui3\` và chạy lại
  `chuan_bi_du_lieu.ps1`, hoặc chép lại `uiplayerbar.ini`/`uitoolscontrolbar.ini` từ cây client PC.

### 12.11. (10/09) Dữ liệu Android là SINH RA — lớp ghi đè `android\du_lieu_ghi_de\`

`chuan_bi_du_lieu.ps1` sinh `D:\jx1_android_data` từ cây client PC và **không có** bước nào giữ tệp
chỉ-Android → chạy lại là mất: ảnh VNKU cho nút kỹ năng (`spr/ui3/uiskillcontrol`, `attack_direction`,
`direction_arrow`, `attack_radius`), khung thanh dưới, hai ini thanh dưới, `ui/uitoado_macdinh.ini`,
`config.ini` có `[Resolution] TheoManHinh` / `[Login]`. Nay tất cả nằm trong `android\du_lieu_ghi_de\`
(cùng cây thư mục, tên **chữ thường** sẵn) và kịch bản chép đè lên **sau cùng** (`them_lop_ghi_de.py`).
**Luật:** thêm/sửa tệp chỉ-Android → sửa ở lớp ghi đè rồi chép sang thư mục dữ liệu (hoặc ngược lại,
nhưng phải có cả hai).

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
17. (10/09) Thanh dưới: khung rồng của VNKU (nhỏ, góc trái dưới), vật phẩm 1–9 trên dải giấy, T/P ở hai
    ô vuông phải, gõ chat ở dải trên, hàng icon chức năng + công cụ ngay trên khung. Chỗ nào chưa ưng →
    kéo trong chế độ sửa giao diện rồi bấm "Mặc định".

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
