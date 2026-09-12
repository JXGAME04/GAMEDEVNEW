# BÀN GIAO — iOS BƯỚC A VÀ B (11/09): CLIENT ĐÃ BIÊN DỊCH VÀ LINK ĐƯỢC CHO iOS

> Nhánh `mobile-0809`, làm trên máy Mac (10.0.0.34). Đọc kèm `PHANTICH_IOS_LOTRINH_1109.md` (lộ trình 6 bước).
> **Chưa commit gì.** Cây làm việc còn sửa; máy Windows đang test bản Android không nhận gì cả.

## 0. Kết quả ngắn

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
