# BÀN GIAO — iOS BƯỚC A VÀ B (11/09): CLIENT ĐÃ BIÊN DỊCH VÀ LINK ĐƯỢC CHO iOS

> Nhánh `mobile-0809`, làm trên máy Mac (10.0.0.34). Đọc kèm `PHANTICH_IOS_LOTRINH_1109.md` (lộ trình 6 bước).
> **Chưa commit gì.** Cây làm việc còn sửa; máy Windows đang test bản Android không nhận gì cả.

## 0. TOÀN CẢNH PHIÊN 11/09 — 12 commit

> Chủ xác nhận cuối ngày: **"đã chạy oke nhận đủ tính năng trong game"** trên iPhone 17 Pro Max.

| Commit | Việc | Chạm mã dùng chung? |
|---|---|---|
| `46a3b52e` | Dọn 7 ký hiệu toàn cục trùng tên (3 nền) | **có** |
| `513d62f8` | Bước A + B: biên dịch và link được cho iOS | có, rào `JX_IOS` |
| `a5e3da10` | **Sửa 2 lỗi làm hỏng bản Windows** do 2 commit hôm nay gây ra | **có** |
| `4faaab2e` | Bước C: chạy trên iPhone thật, cài qua LAN | có, rào `JX_IOS` |
| `e52d46ec` | Shader Metal + bảng tra ký hiệu tĩnh thay `dlopen` | có, rào `JX_IOS` |
| `f6587a7c` | Ghi mã lỗi khởi tạo → tìm ra gốc màn hình đen | có, rào `JX_IOS` |
| `56a691f4` | Bàn giao bước E | không, tài liệu |
| `77e29e0b` | Sửa cú cuối: game chạy, có nhạc, nhận chạm, nối máy chủ | có, rào `JX_IOS` |
| `c3a70658` | **Đổi rào `JX_ANDROID` → `JX_MOBILE`, 230 chỗ / 78 tệp** | **có** |
| `73b24f77` | Bật mật độ điểm ảnh cao → khung vẽ 1338x616 | có, rào `JX_IOS` |
| `50dd9d06` | Lưu đường dẫn đã phân giải → icon tự căn đúng | không, chỉ `ios/` |
| `ce425896` | Bàn giao mục 11 | không, tài liệu |

### VIỆC CHẶN, DÀNH CHO PHIÊN PC VÀ PHIÊN ANDROID

Ba commit chạm mã dùng chung (`46a3b52e`, `a5e3da10`, `c3a70658`) **chưa được dựng trên Windows hay
Android** vì phiên này làm trên máy Mac. Hai phiên đó phải dựng lại và chạy thử trước khi phát hành.
Trong đó `a5e3da10` **sửa lỗi đang làm bản Windows không dựng được** do commit `c848184f` gây ra.

### CÔNG CỤ KIỂM ĐÃ CÓ — chạy trước mỗi lần giao

| Công cụ | Kiểm gì |
|---|---|
| `python3 ios/kiem_rao.py` | Lọc bỏ nhánh `JX_IOS` ở cả hai bản rồi so **từng dòng** với git. Kiểm `android/CMakeLists.txt` phải có `JX_MOBILE`, không được có `JX_IOS` |
| `python3 ios/kiem_doi_ten.py` | Đổi ngược `JX_MOBILE` → `JX_ANDROID` rồi so từng byte. Chứng minh đợt đổi rào là thuần đổi tên |

### KỊCH BẢN VÁ — chạy lại vô hại, đọc/ghi latin-1, giữ số byte cao

`ios/va_nguon_ios_1.py` · `va_nguon_ios_metal.py` · `va_nguon_ios_kyhieu.py` · `va_nguon_ios_baoloi.py` ·
`va_nguon_ios_dem_dinh.py` · `va_nguon_ios_mat_do.py` · `va_chuyen_jx_mobile.py` · `va_dontrung_kyhieu.py` ·
`va_sua_cullcpu_1109.py` · `sinh_shader_msl.py`

---

## 0b. Kết quả ngắn

Vượt mốc của bước B. Không chỉ có bảng lỗi, mà **toàn bộ client đã biên dịch và link thành một tệp thực thi
Mach-O arm64 cho máy ảo iOS**: 7,2 MB, 0 lỗi biên dịch, 0 ký hiệu trùng tên, 0 ký hiệu thiếu.

| Thư viện tĩnh | Cỡ | Số tệp .o |
|---|---|---|
| libS3Client.a | 3,5 MB | 172 |
| libCoreClient.a | 2,6 MB | 103 |
| libEngine.a | 1,1 MB | 124 |
| libLua54.a | 0,4 MB | 34 |
| libRepresent3.a | 0,3 MB | 18 |
| libRainbow / Common / FilterText / JpgLib / JxPosix | 0,1 MB | 22 |
| libSDL3.a | 3,0 MB | — |

Chưa chạy được: còn thiếu điểm vào iOS, vỏ ứng dụng, shader Metal và dữ liệu. Đó là bước C, D, E.

## 1. Bàn làm việc (bước A — xong)

| Thứ | Trạng thái |
|---|---|
| Xcode | 26.6, SDK iOS 26.5, máy ảo iPhone 17 / 17 Pro / Air |
| cmake | 4.4.2, sinh Makefile (không cần ninja) |
| spirv-cross | cài bằng brew, cho bước E |
| SDL3 | 3.2.30 tải về `ThirdParty/SDL3-src/`, dựng riêng cho iOS đạt: `libSDL3.a` 2,98 MB arm64 |
| Mạng LAN | cổng **8765** (máy chủ tải dữ liệu) và **6670** (cổng game) của máy PC 10.0.0.140 đều mở từ Mac. Cổng 445 đóng |
| Bản kê dữ liệu | 665 mục, **7,81 GB**, gói `mobile_NN.pak` 512 MB — lấy qua LAN được, không cần chép tay |

> **Máy ảo `jx1m-test` là của dự án USVOLAM — không đụng vào.** Bản này phải tạo máy ảo riêng.

## 2. Cây dựng iOS (bước B — xong)

Tệp mới, không đụng gì của Android hay PC:

| Tệp | Việc |
|---|---|
| `ios/CMakeLists.txt` | cây dựng, **include thẳng `android/lists/*.cmake`** nên hai nền không thể trôi khỏi nhau |
| `ios/shim/malloc.h` | header chỉ-iOS, Android không bao giờ thấy |
| `ios/JxIosProbeLink.cpp` | phép thử link, ép nạp mọi tệp .o |
| `ios/va_nguon_ios_1.py` | vá nguồn đợt 1 (hai chỗ) |
| `ios/va_nguon_ios_2.py` | vá nguồn đợt 2 (ký hiệu trùng tên, ba tệp) |
| `ios/kiem_rao.py` | **bộ kiểm rào — chạy sau mỗi lần vá** |
| `ThirdParty/nlohmann/nlohmann/json.hpp` | 3.11.3, vì `vcpkg_installed/` chỉ có trên máy Windows |

Dựng:

```
cmake -S ios -B build/ios-sim -G "Unix Makefiles" -DCMAKE_SYSTEM_NAME=iOS \
      -DCMAKE_OSX_SYSROOT=iphonesimulator -DCMAKE_OSX_ARCHITECTURES=arm64 \
      -DCMAKE_OSX_DEPLOYMENT_TARGET=15.0 -DCMAKE_BUILD_TYPE=Release
cmake --build build/ios-sim -j 8
```

Khác bản Android đúng bốn chỗ, đều ghi lý do trong tệp: link tĩnh thay vì `.so`; không chạy hai bản vá
SDL của bản Android (chúng **chỉ sửa `src/gpu/vulkan/SDL_gpu_vulkan.c`**, mà iOS tắt hẳn Vulkan); không kiểm
phiên bản Java; định nghĩa `JX_MOBILE` + `JX_IOS` thay `JX_ANDROID`.

## 3. Sáu thứ chặn vòng biên dịch

| Vấn đề | Xử lý | Chạm mã chung? |
|---|---|---|
| `sys/sysinfo.h` không có trên Apple | shim `sysinfo()` ngay chỗ include, thân hai hàm `GlobalMemoryStatus*` không đổi một dòng | có, rào `JX_IOS` |
| `malloc.h` không có trên Apple | `ios/shim/malloc.h` | không |
| `std::auto_ptr` bị libc++ mới bỏ | macro `_LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR` | không |
| Lua gọi `system()`, iOS cấm | bật `LUA_USE_IOS` — Lua 5.4 vốn có sẵn nhánh này | không |
| `fopen` khai báo ngầm sau `#undef` (clang Apple coi là lỗi, NDK chỉ cảnh báo) | khai báo lại | có, rào `JX_IOS` |
| `nlohmann/json.hpp` chỉ có qua vcpkg trên Windows | nhúng bản 3.11.3 vào `ThirdParty` | không |

## 4. BẢY KÝ HIỆU TOÀN CỤC TRÙNG TÊN — phát hiện quan trọng, liên quan cả bản Android

Link tĩnh của iOS lôi ra **7 ký hiệu, 4 nhóm**, mỗi ký hiệu được định nghĩa ở hai thư viện khác nhau:

| Ký hiệu | Bản A | Bản B |
|---|---|---|
| `l_Time`, `IR_GetCurrentTime()`, `IR_UpdateTime()` | `S3Client/Ui/Elem/UiImage.cpp` | `Core/Src/ImgRef.cpp` |
| `g_pMusic` | `S3Client/S3Client.cpp:111` | `Core/Src/KCore.cpp:218` |
| `g_pIInlinePicSink` | `S3Client/S3Client.cpp:109` | `Engine/Src/Text.cpp:21` |
| `IID_IESClient`, `IID_IClientFactory` | `S3Client/NetConnect/NetConnectAgent.cpp` (qua `<initguid.h>` + `DEFINE_GUID`) | `MultiServer/Rainbow/IClient.cpp:4,8` |

**Đây đúng là họ hàng của lỗi `GameScriptFuns`** đã giết Thư, Đấu giá và Chiến Lệnh trên Android
(`BANGIAO_ANDROID_PHA4_0809.md` §9.1). Trên Windows mỗi DLL giữ bản riêng nên không ai va chạm. Trên Android
bộ nạp động chỉ giữ **một** định nghĩa cho cả tiến trình và **không báo gì** — sai lặng lẽ. Trên iOS link tĩnh
thì trình liên kết báo to ngay lúc dựng.

Nhóm 1 đáng chú ý nhất: `UiImage.cpp` và `ImgRef.cpp` **mỗi bên có một đồng hồ hoạt ảnh riêng cùng tên**,
một bên lấy `GetTickCount()`, bên kia `timeGetTime()`. Trên Android hai bên đang dùng chung một đồng hồ.
Hiện chưa thấy triệu chứng vì lớp POSIX cho hai hàm đó trả cùng một giá trị, nhưng đây là bom hẹn giờ.

**Cách xử lý đã dùng cho iOS:** bỏ bản sao **phía client** (S3Client và Rainbow) khi `JX_IOS`, dùng chung một
bản của Core / Engine — đúng bằng thứ bản Android đang chạy. Không đụng Core hay Engine vì máy chủ cũng dịch
những tệp đó.

### 4.1 ĐÃ DỌN cho cả ba nền (chủ duyệt 11/09) — `ios/va_dontrung_kyhieu.py`

Cách chữa: **đổi tên bản phía giao diện (S3Client)** và bỏ hai hằng số thừa của Rainbow.
**Không đụng Core, Engine, Represent** vì máy chủ cũng dịch những tệp đó.
Theo đúng tiền lệ đã có sẵn trong dự án: Represent gặp y hệt va chạm này và đã đổi tên bản của nó thành
`g_pIInlinePicSinkRP` (`Represent2/KRepresentShell2.cpp:31`, `Represent3/KRepresentShell3.cpp:577`).

| Tên cũ | Tên mới phía S3Client | Số chỗ |
|---|---|---|
| `l_Time` | `l_UiTime` | |
| `IR_GetCurrentTime` | `UiIR_GetCurrentTime` | 84 chỗ / 24 tệp |
| `IR_UpdateTime` | `UiIR_UpdateTime` | |
| `g_pMusic` | `g_pMusicUI` | 10 chỗ / 2 tệp |
| `g_pIInlinePicSink` | `g_pIInlinePicSinkUI` | 36 chỗ / 5 tệp |
| `IID_IESClient`, `IID_IClientFactory` | bỏ bản thừa trong `Rainbow/IClient.cpp` | 2 chỗ |

Tổng **138 chỗ / 31 tệp**, số byte ≥ 0x80 của từng tệp giữ nguyên.

**Vì sao trung tính về hành vi:**
- **PC**: mỗi mô đun vốn đã có biến riêng. Đổi tên không đổi chỗ lưu, không đổi ngữ nghĩa.
- **Android**: hai bản đang dùng chung một ô nhớ nhưng **đều trỏ tới cùng một đối tượng** —
  `g_pMusic` cả hai đều là `&m_Music` (Core nhận qua `CoreShell.cpp:25325`, S3Client gán ở `S3Client.cpp:664`);
  `g_pIInlinePicSink` cả hai đều là `m_pInlinePicSink`; `l_Time` cả hai đều được cập nhật mỗi khung
  (Core ở `KScenePlaceC.cpp:1096`, S3Client ở `UiShell.cpp:233` và `:405`). Tách ra cho cùng giá trị.
- Hai GUID chỉ là hằng số, hai bản trùng nhau từng byte.

**Đã kiểm được gì trên máy Mac này:** bản iOS dựng lại **0 lỗi, 0 ký hiệu trùng**; các tệp S3Client
**chỉ-Windows** mà bản iOS không dịch (`CrashLog`, `JxReplay`, `PerfHud`, `TrayMode`, `UiPlayVideo`, `AntiHack`)
**không hề nhắc tới** bốn nhóm ký hiệu này, nên không có vùng mù.

**Việc của phiên PC và phiên Android:** máy này là Mac, **không dựng được Windows hay APK**. Hai phiên đó phải
dựng lại và chạy thử trước khi phát hành. Đổi tên là cơ học và trình biên dịch bắt hết chỗ sót, nhưng luật cũ
vẫn là hai chuỗi Windows phải 0 lỗi mới tính là xong.

## 5. Hai quy tắc mới của chủ, đã thành công cụ

1. **Sửa xong việc gì là kiểm lại rào ngay.** `python3 ios/kiem_rao.py` — lọc bỏ mọi nhánh `JX_IOS` / `JX_MOBILE`
   ra khỏi tệp đã vá rồi so **từng dòng** với bản trong git, và kiểm `android/CMakeLists.txt` không định nghĩa
   hai macro đó. Kết quả hiện tại: 5/5 tệp **giống hệt**, số byte ≥ 0x80 không đổi.
2. **Trước mỗi lần dựng phải kiểm git.** Quy tắc này ăn ngay lần đầu: phiên khác đẩy **2 commit** lên
   `origin/mobile-0809` giữa lúc tôi dựng (`58b41d02`, `47bfa742` — tắt atlas texture mảng), trong đó có
   `KRepresentShell3.cpp` mà bản iOS cũng dịch. Đã gộp tiến thẳng, không xung đột, hai bản vá iOS còn nguyên.
   `origin/main` còn 2 commit mobile chưa có (`997a3183`, `9184b5f1`) nhưng **không đụng mã C++** (một tài liệu,
   một script Lua máy chủ) nên không ảnh hưởng bản dựng.

## 6. Việc tiếp theo — bước C

| # | Việc | Ghi chú đã khảo sát |
|---|---|---|
| C1 | Điểm vào `JxIosMain.mm` | đối xứng `JxAndroidMain.cpp`; `JxPosixMain` là ký hiệu **C++**, không `extern "C"` |
| C2 | Thư mục dữ liệu | thay `SDL_GetAndroidExternalStoragePath` bằng thư mục của ứng dụng, kèm cờ loại trừ sao lưu |
| C3 | `LoadLibraryA` → bảng ký hiệu tĩnh | `dlopen` vẫn còn trong nhị phân; `dlopen("libRainbow.so")` sẽ hỏng lúc chạy. Hai chỗ cần: `Rep3_JxEpTrinhChieu`, `CreateTextFilter`, cộng điểm vào Rainbow |
| C4 | miniaudio | **đã đo: bản iOS hiện chỉ có bộ phát rỗng** (`ma_context_init__null`). `KSoundMa.cpp` mới bật AAudio và OpenSL. Cần thêm Core Audio |
| C5 | Bốn chỗ đọc `/proc` | biên dịch qua nhưng lúc chạy trả rỗng, không chặn mốc C |
| — | **Mốc nghiệm thu C** | chạy trên máy ảo, hiện đúng hộp thoại báo không thấy `config.ini` rồi thoát sạch |

## 7. Trạng thái cây làm việc

Sửa nhưng **chưa commit**: `.gitignore`, `Sources/Engine/Src/Platform/KPosixWin32.cpp`,
`Sources/Library/Lua54/jx_lua_fopen.c`, `Sources/S3Client/S3Client.cpp`,
`Sources/S3Client/Ui/Elem/UiImage.cpp`, `Sources/MultiServer/Rainbow/IClient.cpp`.
Tệp mới: `ios/`, `ThirdParty/nlohmann/`, `PHANTICH_IOS_LOTRINH_1109.md`, tệp này.

Năm tệp nguồn đều đã qua `ios/kiem_rao.py`. **Chưa dựng lại hai chuỗi Windows và chưa dựng lại APK** — máy này
là Mac, không dựng được. Trước khi đưa lên `origin`, máy Windows phải dựng lại cả hai chuỗi và một APK để
nghiệm thu theo luật cũ.

---

## 8. BƯỚC C (11/09 chiều): chạy được trên iPhone thật

`ios/JxIosMain.cpp` + `ios/JxIosDuongDan.mm` (điểm vào) và `ios/Info.plist` + đích `jx1ios` (gói ứng dụng).

| Mốc | Kết quả |
|---|---|
| Chạy trên máy ảo iOS | hiện đúng hộp thoại thiếu `config.ini` |
| Dựng bản ký cho máy thật | `BUILD SUCCEEDED`, ký bằng `Apple Development: dainguyen0401@gmail.com` |
| Cài lên iPhone 17 Pro Max **qua LAN**, không cáp | `App installed: vn.jx1.mobile` |
| Đẩy 14 MB dữ liệu nhẹ vào Documents qua LAN | 52 tệp |
| Tìm thấy thư mục dữ liệu trên máy thật | `[IOS] thu muc du lieu: /var/mobile/.../Documents (SDL 3.2.30)` |
| Mở cửa sổ, vào vòng lặp game | có (chủ thấy màn hình đen + bàn phím) |

**Còn lại của bước C, đã truy ra gốc, chưa sửa:**

1. **Màn hình đen**: `D3D9onGPUDev.cpp:306` xin `SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, ...)`. iOS chạy Metal nên lệnh này thất bại, tầng vẽ không lên. Việc của bước E.
2. **Bàn phím ảo bật suốt**: bản sửa của Android nằm trong `#ifdef JX_ANDROID` tại `KSdlApp.cpp:572`; bản iOS đi nhánh `#else` gọi `SDL_StartTextInput` rồi giữ luôn. Cả cơ chế bật theo ô nhập (dòng 741-808) cũng sau rào đó. Cách chữa là đổi những rào "vì là điện thoại" sang `JX_MOBILE`.

## 9. BẢY BẪY ĐÃ VẤP TRONG BƯỚC C — đọc để khỏi mất giờ lại

| # | Bẫy | Triệu chứng | Cách qua |
|---|---|---|---|
| 1 | `CFBundleExecutable` để `$(EXECUTABLE_NAME)` | `App installation failed ... is missing its bundle executable` | CMake kiểu Makefile **không** thay biến của Xcode. Ghi thẳng tên thật vào `Info.plist` |
| 2 | Trộn lớp giả lập Win32 với header Objective-C | `typedef redefinition ('bool' vs 'int')` rồi `unexpected '@' in program` | Lớp giả lập đặt `BOOL = int`, `<objc/objc.h>` đặt `BOOL = bool`. **Tách hai tệp**: `.mm` chỉ dùng Foundation, không include `KWin32.h` |
| 3 | `access()` bị macro đổi thành `jx_access` | app báo thiếu `config.ini` **dù tệp có thật** | `KPosixWin32.h:147` `#define access jx_access` → đi qua `JxPathPosix`. Danh sách đường hệ thống chỉ có gốc Android. Đã thêm `/var/ /private/ /Users/ /Library/ /Applications/` cho `JX_IOS` |
| 4 | Mã đội ký nhầm | `No Account for Team "362WH8ZLF2"` | Phần trong ngoặc của tên chứng chỉ là **mã chứng chỉ**, không phải mã đội. Mã đội nằm ở trường **OU**: `PK9QTZYMSL` |
| 5 | `developer disk image could not be mounted` | không cài được, tưởng do Developer Mode tắt | Thực ra chỉ **tạm thời**. Developer Mode vẫn bật (`developerModeStatus: enabled`). Thử lại là được |
| 6 | Mã hóa URL tên tệp GBK | 404 khi tải, trong đó có `settings/serverlist=<GBK>.ini` | Phải `quote()` trên **byte gốc** (`duong.encode("latin-1")`), không phải trên chuỗi đã giải mã |
| 7 | Vá làm **tách một dòng cũ thành hai** | `ios/kiem_rao.py` báo hỏng | Bộ kiểm so **từng dòng**. Bản vá phải **chèn nguyên khối mới**, không đụng dòng cũ nào |

**Bẫy thứ tám, nặng nhất, do bản dọn `[DONTRUNG 11/09]` của phiên này gây ra:** bỏ hai GUID trong
`Rainbow/IClient.cpp` làm **hỏng link bản Android** (libRainbow.so là `.so` riêng, không chứa
`NetConnectAgent.cpp` nên không còn định nghĩa nào). Phiên `[CULLCPU 11/09]` phát hiện và vá cho Android.
**Bản Windows cũng dính y hệt** vì Rainbow.dll cũng là DLL riêng và `ClientStage.cpp` dùng hai GUID đó ở
bốn chỗ. Đã sửa thành **ba nền ba đường** (`[DONTRUNG-SUA 11/09]`): iOS không định nghĩa, Android định nghĩa
`EXTERN_C`, Windows giữ **y hệt hai dòng `static const GUID` nguyên bản**.

> **Bài học vào quy tắc:** đừng bao giờ bỏ một định nghĩa toàn cục chỉ vì một nền thấy nó trùng. Phải xét
> **cách đóng gói của từng nền** trước: thứ trùng trên nhị phân gộp lại là thứ **bắt buộc** trên nhị phân tách rời.

---

## 10. BƯỚC E (11/09 tối): METAL CHẠY ĐƯỢC TRÊN iPHONE THẬT

### 10.1 Shader Metal — `ios/sinh_shader_msl.py`

Không cần cài `glslc`: lấy **thẳng mã SPIR-V đang dùng** trong `Rep3ShadersGPU_spv.h` (đúng byte với
bản Android), dịch chéo sang MSL bằng `spirv-cross --msl --msl-decoration-binding`.

Đã đọc `SDL_gpu_metal.m` 3.2.30 trước để biết quy ước gắn kết, và chỉ số sinh ra **khớp chính xác**:

| Tài nguyên | Sinh ra | SDL đặt ở |
|---|---|---|
| Đệm hằng số đỉnh và điểm ảnh | `buffer(0)` | `atIndex:i` từ 0 |
| Ba texture và ba bộ lấy mẫu | 0, 1, 2 | `NSMakeRange(0, num_samplers)` |
| Đệm đỉnh | khe 14 trở lên | `METAL_FIRST_VERTEX_BUFFER_SLOT = 14` |

### 10.2 Bảng tra ký hiệu tĩnh — thay `dlopen`

`S3Client.cpp:389` gọi `LoadLibrary("Represent3.dll")`. Trên iOS mọi thứ link tĩnh vào **một** nhị phân
nên không có gì để nạp. Thêm bảng tra trong `KPosixWin32.cpp` (rào `JX_IOS`); ứng dụng đăng ký trước
trong `ios/JxIosMain.cpp`. Đã đăng ký ba hàm: `CreateRepresentShell`, `CreateInterface` của Rainbow,
`CreateTextFilter`.

### 10.3 Ba chặn liên tiếp, mỗi cái lộ ra cái sau

1. **Khởi tạo hỏng, không biết vì sao.** Thêm `JxIos_GhiLoiKhoiTao()` ghi mã lỗi mà `GameInit()` vốn đã
   đặt sẵn. Ngay lần chạy đầu: `ma loi = 1, chuoi loi = "\settings\chatsent.flt"` — **thiếu dữ liệu**,
   không phải lỗi mã. Tệp đó nằm trong gói pak chứ không phải tệp rời.
2. **Máy ảo iOS không chạy được SDL_GPU Metal.** `SDL_CreateGPUDevice(MSL)` trả
   `Device does not meet the hardware requirements`. Tra mã: `SDL_gpu_metal.m:4489` đòi
   `MTLGPUFamilyApple3`, máy ảo không đáp ứng. **Từ nay muốn thấy hình phải chạy máy thật.**
3. **Trên iPhone thật thì chạy.** Nhật ký máy chủ (iPhone 17 Pro Max, iOS 26.6.2):

```
[GPU] thiet bi: driver metal, backbuffer 800x600, swapchain fmt 12, trinh chieu vsync
[REP3] card: HAL | MaxTex 8192x8192 | vsync=1
[FONT] CreateAFont id=10..16 -> OK  (du 5 bo phong)
[GPU] man hinh 956x440@120 | 13 che do
```

### 10.4 Cách chuyển dữ liệu — số đo thật

| Việc | Số đo |
|---|---|
| Tải 14 gói pak từ máy chủ LAN của máy PC | 6,50 GB trong 115 s, **58 MB/s** |
| Tải 599 tệp rời (spr, maps) | 1,30 GB trong 71 s |
| Chép vào máy ảo | 6,5 GB trong **7,8 s** (sao chép nhanh của APFS) |
| Đẩy sang iPhone bằng `devicectl` | 512 MB trong 14,8 s, **35 MB/s** |

Tra gói nào chứa tệp nào bằng bộ đọc pak sẵn có `ReverseTools/pak_vltk/pakdump.py`:
`chatsent.flt`, năm bộ phông và `UiLogin.ini` đều nằm trong `mobile_01.pak`.

### 10.5 Ba bẫy nữa (tiếp mục 9)

| # | Bẫy | Cách qua |
|---|---|---|
| 9 | Bản vá biến một hàm viết trên **một dòng** thành nhiều dòng | `ios/kiem_rao.py` so **từng dòng**. Phải bọc bằng rào có nhánh `#else` giữ nguyên văn dòng cũ |
| 10 | `Rep3_JxEpTrinhChieu` nằm trong `#ifdef JX_ANDROID` nên iOS không có; `CreateTextFilter` khai báo kiểu C++ nhưng xuất ký hiệu liên kết C | Xem `nm` trước khi khai báo |
| 11 | `extern "C"` đặt trong **thân hàm** | C++ đòi đặc tả liên kết ở phạm vi tệp. Clang báo `expected unqualified-id` |

---

## 11. (11/09 tối) CHẠY ĐẦY ĐỦ TRÊN iPHONE — chủ xác nhận "đã chạy oke nhận đủ tính năng trong game"

### 11.1 Ba việc của đợt cuối

| # | Việc | Chỗ sửa | Ảnh hưởng PC / Android |
|---|---|---|---|
| 1 | Dùng chung **toàn bộ** phần giao diện mobile của Android | 230 chỗ / 78 tệp, `JX_ANDROID` → `JX_MOBILE` | **Không**, đã chứng minh bằng máy |
| 2 | Bật mật độ điểm ảnh cao | `KSdlApp.cpp`, rào `JX_IOS` | Không |
| 3 | Lưu đường dẫn đã phân giải | `ios/JxIosMain.cpp` (chỉ iOS dịch) | Không |

### 11.2 Vì sao đợt đổi rào là an toàn — có máy kiểm (`ios/kiem_doi_ten.py`)

Sau đợt này `android/CMakeLists.txt` định nghĩa **cả hai** macro, nên mọi rào đổi tên vẫn thoả y hệt.
Windows và máy chủ không định nghĩa macro nào trong hai nên cũng đi đúng nhánh cũ. Rủi ro dồn hết về
phía iOS, và trình biên dịch chỉ ra từng chỗ.

Kiểm bằng máy: đổi **ngược** `JX_MOBILE` → `JX_ANDROID` ở từng tệp rồi so với bản trong git.
Kết quả **78/78 tệp giống hệt từng byte** ⇒ thay đổi chỉ là đổi tên macro, không thêm bớt một dòng logic.

**Không** đổi: `Represent/Represent3/**` (thử nghiệm atlas và texture mảng riêng của Android, chưa đo
trên Metal bao giờ), `JxAndroidStubs.cpp`, `JxPerfHudAndroid.cpp` (đọc `/proc`).

### 11.3 Mật độ điểm ảnh — gốc của "chưa căn đúng kích thước màn hình"

`SDL_CreateWindow(..., 0)` thiếu cờ `SDL_WINDOW_HIGH_PIXEL_DENSITY`. Android không dính vì cửa sổ
Android vốn tính bằng điểm ảnh; iOS thì làm việc ở 1x.

```
trước:  [GPU] man hinh 956x440 | cua so 956x440 px    -> khung ve 1124x516 (sai họ)
sau :   [DPG] man hinh 956x440 | cua so 2868x1320 px | ho DIEN THOAI, he so 2,142 -> khung ve 1338x616
```

1338x616 đúng **họ chiều cao 616** mà Fold 7 dùng, nên tệp bố cục mặc định của bản Android áp đúng tỉ lệ.

### 11.4 `/var` và `/private/var` — bẫy nặng nhất, do chính bản vá iOS gây ra

Bản vá `[IOS 11/09]` thêm `/var/` và `/private/` vào danh sách "đường hệ thống giữ nguyên" của
`JxPathPosix`. Hệ quả không lường: **mọi** đường tuyệt đối của iOS bỏ qua bước hạ chữ thường.

Chuỗi nhân quả đầy đủ:
1. `g_GetFullPath` ghép `szRootPath` lấy từ thư mục hiện hành; iOS phân giải `/var` → `/private/var`.
2. `JxIosMain` lưu thư mục dữ liệu ở dạng `/var/...` (từ `NSSearchPathForDirectoriesInDomains`).
3. Hai chuỗi khác nhau ⇒ `strncmp` với `s_szDataDir` thất bại ⇒ không nhận ra là đường **dưới** thư mục dữ liệu.
4. Rơi xuống danh sách đường hệ thống ⇒ trả về nguyên si, **bỏ qua hạ chữ thường**.
5. Hệ tệp iOS phân biệt hoa thường ⇒ tìm `Ui/UiToaDo_DanhSach.ini` trong khi tệp thật là `ui/uitoado_danhsach.ini`.

Một gốc giải thích bốn triệu chứng: không nạp bố cục, không nạp danh sách chỉnh, thiếu
`\settings\npcres\Man*Weapon*_effect.txt`, không ghi được `\APdata\<id>.dat`.
Tệp nằm **trong pak** không dính vì tra theo id băm, không qua đường dẫn.

**Chữa:** `realpath()` trước khi `JxPosix_SetDataDir`. Một dòng, trong tệp chỉ iOS dịch.

Đo lại trên máy thật:
```
[UITOADO] [UITOADO 12/09 NEO]: tep thiet ke 1040x604, khung ve 1338x616
[UITOADO] nap xong ...\Ui\UiToaDo_MacDinh_Rong.ini -> bang co 162 muc
[SUAGD] danh sach trang: 102 muc tu ...\Ui\UiToaDo_DanhSach.ini
```

### 11.5 Hai bẫy nữa (tiếp mục 9 và 10.5)

| # | Bẫy | Cách qua |
|---|---|---|
| 12 | Phép kiểm "đã vá từ trước" trong kịch bản vá dùng một chuỗi là **tiền tố** của dòng cũ | Báo trùng nhầm, bản vá **không được áp mà không báo lỗi**. Phải dùng dấu nhận dạng riêng |
| 13 | Trên iOS `/var` và `/private/var` là **cùng một chỗ nhưng khác chuỗi** | Mọi phép so sánh tiền tố đường dẫn phải làm trên bản **đã phân giải** |

> Và một bẫy đã gặp hai lần trong phiên: `grep` coi tệp có byte cao (TCVN3/GBK) là **nhị phân** và bỏ qua.
> Đếm thiếu 51 tệp có `JX_ANDROID` vì quên cờ `-a`. Mọi lần đếm trên cây nguồn này phải dùng `grep -a`.

### 11.6 Trạng thái cuối ngày 11/09

Chủ xác nhận trên iPhone 17 Pro Max: **"đã chạy oke nhận đủ tính năng trong game"**.

| Việc | Trạng thái |
|---|---|
| Dựng, link, ký, cài **qua LAN** không cần cáp | xong |
| Thiết bị Metal, shader MSL | xong |
| Phông chữ, ảnh, nhạc nền, âm thanh | xong |
| Nối máy chủ, đồng bộ nhân vật, vào thế giới | xong |
| Cần điều khiển ảo, nút kỹ năng, HUD, khung Auto | xong |
| Khung vẽ theo màn hình, tự căn icon theo neo | xong |
| Trình chỉnh vị trí icon, vùng an toàn | dùng chung với Android |

**Còn lại:** bảng đo hiệu năng iOS đang để rỗng (`ios/JxIosStubs.cpp`) vì bản Android đọc `/proc`;
chưa đo khung hình / nhiệt / pin trên iOS; chưa làm bộ tải dữ liệu trong app (bước F);
Represent3 chưa bật ba biến thể storage buffer / texture mảng trên Metal.

---

## 12. Bảo mật iOS — trạng thái sáu việc (12/09)

| # | Việc | Trạng thái | Ở đâu |
|---|---|---|---|
| 1 | Ký bản kê tệp tải về | **Xong** | `android/ky_manifest.py`, `ios/JxTaiDuLieu.mm` |
| 2 | HTTPS cho mọi thứ tải về | **Xong** | `ios/Info.plist` chỉ mở `NSAllowsLocalNetworking`; ATS vẫn chặn HTTP thường ra Internet |
| 3 | Kho lưu mật mã | **Xong** | `ios/JxIosKhoaMat.mm` (Keychain) + dọn bản cũ trong `Login.cpp` |
| 4 | Thư mục dữ liệu + cờ loại trừ sao lưu | **Xong** | `NSURLIsExcludedFromBackupKey` trong `ios/JxTaiDuLieu.mm` |
| 5 | Dọn nhật ký chẩn đoán | **Xong** | `[Client] NhatKyChanDoan=0` → `g_DebugLog` câm |
| 6 | Cổng phiên bản | **Nửa client xong**, nửa máy chủ còn lại | `phienban.txt` trong kho dữ liệu |

### Việc bắt buộc mỗi lần đổi dữ liệu

Bộ tải **từ chối** kho không có chữ ký hợp lệ. Sau mỗi lần đổi dữ liệu:

```
python3 android/may_chu_tai_du_lieu.py --thu-muc <thư mục> --chi-manifest
python3 android/ky_manifest.py --ky <thư mục> --khoa ~/.jx1_khoa/jx1_manifest_ec.key
```

Khoá riêng ở `~/.jx1_khoa/jx1_manifest_ec.key`, quyền 600, **cố ý để ngoài kho mã nguồn**.
Mất khoá = phải đổi khoá công khai trong `ios/JxTaiDuLieu.mm` rồi phát hành lại.

### Trước khi phát hành

- Đặt `[Client] NhatKyChanDoan=0` trong `config.ini` của gói phát hành.
- Kho dữ liệu dùng HTTPS, không dùng HTTP.
- Tăng `JX_PHIEN_BAN_APP` trong `ios/JxIosMain.cpp`, và cập nhật `phienban.txt` trong kho.

### Còn lại cho phiên máy chủ

`Login.cpp:1078` có sẵn trường `ProtocolVersion` nhưng nằm trong `#ifdef USE_KPROTOCOL_VERSION`,
mà macro đó chưa được định nghĩa ở đâu cả nên trường này **hiện không được gửi**. Bật lên là đổi
giao thức bắt tay, phải sửa máy chủ cùng lúc. Khi máy chủ sẵn sàng đọc và từ chối client quá cũ
thì phía client chỉ cần định nghĩa `USE_KPROTOCOL_VERSION`.

Cổng phiên bản hiện tại chỉ chặn người chơi ngay tình. Client bị sửa ruột thì bỏ qua được.
Cổng thật phải nằm ở máy chủ.
