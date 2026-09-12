# PHÂN TÍCH: LÀM BẢN iOS TỪ DỰ ÁN HIỆN TẠI — CHỈ PHÂN TÍCH, KHÔNG SỬA MỘT DÒNG MÃ

> Đọc trên nhánh `mobile-0809`, HEAD `ee3c3cdd` (11/09 16:11). Mọi con số dưới đây tôi tự đếm bằng lệnh
> trên chính cây này; chỗ nào là ước thì ghi rõ "ước".
>
> Tài liệu đã đọc: `PHUONGAN_NANG_MOBILE_0809.md`, `BANGIAO_X64_PHA0_0809.md`, `BANGIAO_PHA1_CLANG_0809.md`,
> `BANGIAO_SDL_PHA2_0809.md`, `BANGIAO_ANDROID_PHA4_0809.md`, `LOTRINH_MOBILE_0909.md`,
> `KEHOACH_GIAODIEN_MOBILE_1209.md`, `PHANTICH_GIAODIEN_MOBILE_KEHOACH1209.md`,
> `BANGIAO_GIAODIEN_MOBILE_SUAGD_1309.md`, `BANGIAO_ANDROID_DULIEU_1109.md`,
> `PHUONGAN_FPS_NHIET_PIN_MOBILE_1109.md`, `GHICHU_PHIEN_1209.md`, `BAOCAO_LOHONG_2908.md`.
>
> Ràng buộc của chủ được giữ nguyên trong toàn bộ tài liệu này: **bản PC không đổi một byte hành vi,
> bản Android không đổi một byte hành vi.**

---

## 0. Kết luận ngắn

1. **Phần nặng nhất của iOS không phải kỹ thuật vẽ.** Metal/shader chỉ là 3–5 ngày công. Thứ thật sự
   chặn là **đường phát hành**: iOS không có APK, không tự cài bản mới, mọi bản tới tay người chơi đều
   phải qua Apple hoặc TestFlight. Việc này phải chốt **trước** khi viết dòng mã đầu tiên, vì nó quyết
   định cả cách cập nhật, cách tải dữ liệu và cả việc có nên làm hay không.
2. **Phần mã dùng lại được rất lớn.** Lớp tương thích đã POSIX hoá sẵn: `JX_POSIX` xuất hiện ở
   **162 tệp**, còn `JX_ANDROID` chỉ **17 tệp / 78 chỗ**. Nghĩa là phần đã "dính Android thật" rất mỏng —
   đây là tin tốt nhất của bản khảo sát này.
3. **Bốn lớp phải viết mới cho iOS** (ước tổng **3.500–5.000 dòng**): vỏ ứng dụng thay lớp Java
   (4 lớp trong `vn.jx1.mobile`), bảng ký hiệu tĩnh thay `dlopen`, shader MSL thay SPIR-V, đo hiệu năng
   qua `mach`/ThermalState thay `/proc`.
4. **Ước công: 5–8 tuần** kỹ thuật, tính từ lúc đã có máy Mac + tài khoản nhà phát triển + một iPhone
   thật. Tài liệu `PHUONGAN_NANG_MOBILE_0809.md` ghi "+1 tháng iOS" — đúng cho phần vẽ, **thiếu** phần
   vỏ ứng dụng, bộ tải dữ liệu, chính sách lưu trữ và rà soát chính sách của Apple.
5. **Sáu việc bảo mật phải làm ngay ở bước đầu**, và nên làm **trên bản Android trước** để iOS thừa
   hưởng: HTTPS cho bộ tải, **ký bản kê tệp**, kho lưu mật mã, thư mục dữ liệu + cờ loại trừ sao lưu,
   dọn nhật ký, cổng phiên bản. Mọi việc này nếu lùi lại sau sẽ phải **sửa cả hai nền** — đúng thứ chủ
   muốn tránh.
6. **Điểm phải sửa mà chưa ai nhắc**: mã mạng đang dùng `inet_addr` + `gethostbyname` (chỉ IPv4).
   Apple bắt buộc ứng dụng chạy được trên mạng **chỉ có IPv6** (NAT64/DNS64). Đây là lỗi bị trả hồ sơ
   kinh điển và nó nằm ở `Sources/MultiServer/Common/Socket.cpp:165`.

---

## 1. Bản Android đang hoạt động ra sao (đo trên HEAD)

### 1.1 Chồng tầng

```
   Java: TaiDuLieuActivity (màn tải dữ liệu)  ->  JxActivity : SDLActivity  ( + JxDoNhip, JxTepProvider )
                                                        │
   SDL3 3.2.30 (nguồn, tự vá bằng android/va_sdl3_*.py) ─┘
                                                        │  nạp libSDL3.so + libmain.so, gọi SDL_main
   C++:  JxAndroidMain.cpp  ->  JxPosixMain (S3Client.cpp)  ->  KMyApp = KSdlApp : KWin32App
                                                        │
   .so:  libmain.so (S3Client)  ──link──> libCoreClient.so, libEngine.so, libFilterText, libLua54Dll.so
                                 ──dlopen──> libRainbow.so (mạng), libRepresent3.so (vẽ)
                                 ──chung──> libJxPosix.so (lớp tương thích Win32)
```

Cấu trúc `.so` cố ý **giống cấu trúc DLL trên Windows**, kể cả việc nạp động Rainbow và Represent3
(`android/CMakeLists.txt`). Đó là lý do một mã nguồn chạy được cả hai nền.

### 1.2 Số đo

| Thứ | Số | Chỗ đo |
|---|---|---|
| Tệp có `JX_POSIX` | 162 tệp / 187 chỗ | `Sources` |
| Tệp có `JX_ANDROID` | **17 tệp / 78 chỗ** | `Sources` |
| Tệp có `JX_PLATFORM_SDL` | 22 tệp | `Sources` |
| Tệp có `JX_MOBILE` hoặc `JX_IOS` | **0** | chưa ai định nghĩa |
| Lớp tương thích Win32→POSIX | 2.460 dòng | `KPosixCompat.h` 1.343 + `KPosixWin32.cpp` 958 + `KPosixCrypto.cpp` 159 |
| Header Windows giả lập | ~150 tên, sinh tự động | `android/gen_winshim.py` → `Engine/Src/Platform/winshim/` |
| Trình chỉnh giao diện mobile | 2.236 dòng | `Ui/Elem/UiToaDoMobile.inc`, rào `JX_ANDROID` |
| Mã SPIR-V nhúng | 430 KB | `Rep3ShadersGPU_spv.h`, sinh bởi `ReverseTools/mobile_x64/dich_shader_gpu.py` (glslc) |
| Dữ liệu | 10,7 GB → **≈ 9,0 GB** sau dọn | `BANGIAO_ANDROID_DULIEU_1109.md` §4 |
| Java riêng của dự án | 4 lớp | `vn.jx1.mobile` |
| Bộ dựng | NDK 25.2.9519653, minSdk 24, abi arm64-v8a + x86_64 | `app/build.gradle` |

### 1.3 Những cơ chế Android đang dùng mà iOS **không có tương đương thẳng**

| Cơ chế Android | Ở đâu | iOS |
|---|---|---|
| `dlopen("libRainbow.so")`, `dlopen("libRepresent3.so")` | `KPosixWin32.cpp:406-428` | Phải link tĩnh + bảng ký hiệu tra tay |
| Tự tải APK mới rồi tự cài | `REQUEST_INSTALL_PACKAGES` + `JxTepProvider` + `TaiDuLieuActivity.kiemApk` | **Cấm tuyệt đối.** Chỉ còn cổng phiên bản + mở App Store |
| HTTP thường (`usesCleartextTraffic="true"`), máy chủ tải cổng 8765 | `AndroidManifest.xml`, `android/may_chu_tai_du_lieu.py` | ATS bắt HTTPS |
| `/proc/self/stat`, `/proc/self/statm` | `KPosixWin32.cpp:351,359,903` | Không có; dùng `task_info` / `os_proc_available_memory` |
| `SDL_GetAndroidExternalStoragePath` | `JxAndroidMain.cpp:44` | `SDL_GetPrefPath` / thư mục Application Support |
| Nhịp vẽ qua `Choreographer` (lớp `JxDoNhip`) | Java | `CADisplayLink` |
| Bàn phím mềm, vùng an toàn qua `SDLSurface` con | `JxActivity.JxSurface` | SDL đã có đường iOS (`SDL_uikitview.m`), nhưng phần ghi đè riêng của dự án phải viết lại |
| Shader SPIR-V | `Rep3ShadersGPU_spv.h` | Metal chỉ nhận MSL / metallib |

---

## 2. Cái gì dùng lại nguyên vẹn

**Dùng lại nguyên, không sửa một dòng** (đây là phần lớn nhất của dự án):

- `Sources/Core/Src` — toàn bộ logic và giao thức.
- `Sources/S3Client/Ui` — 134 cửa sổ, hệ `KWnd`, kể cả trình chỉnh giao diện mobile 2.236 dòng.
- `Sources/Engine/Src` phần thuần + lớp `KPosix*` (POSIX chứ không phải Linux, trừ 4 chỗ `/proc`).
- `Sources/Represent/Represent3` đường `Rep3Api=100` (D3D9 trên SDL_GPU) — chỉ đổi **định dạng shader**,
  không đổi mã vẽ.
- `Sources/MultiServer/Common` + `Rainbow` — mạng BSD socket (trừ chỗ phân giải địa chỉ, mục 3.6).
- `Library/Lua54` — C thuần.
- Toàn bộ dữ liệu, pak, ini, bố cục, `du_lieu_ghi_de` — **định dạng không đổi**.
- Bộ sinh danh sách nguồn `android/gen_lists.py` — sinh từ chính vcxproj, iOS dùng lại được y hệt.

**Phải viết mới** (ước 3.500–5.000 dòng):

| Phần | Ước dòng | Ghi chú |
|---|---|---|
| Vỏ ứng dụng iOS (AppDelegate/ViewController, nhịp vẽ, toàn màn hình, vòng đời) | 600–900 | thay `JxActivity` + `JxDoNhip` |
| Bộ tải dữ liệu iOS (giao diện + tải song song + tiếp tục khi đứt + kiểm md5/chữ ký) | 1.200–1.800 | thay `TaiDuLieuActivity` 1:1 |
| Bảng ký hiệu tĩnh thay `dlopen` | 150–250 | `LoadLibraryA`/`GetProcAddress` trả từ bảng |
| Sinh shader MSL + nhánh `SDL_GPU_SHADERFORMAT_MSL` | 300–500 | gồm cả 3 biến thể `JX_TEX_ARRAY`, `JX_PAL_BUFFER`, `JX_PS_BUFFER` |
| Đo hiệu năng/nhiệt/pin bằng API Apple | 400–700 | thay `JxPerfHudAndroid.cpp` phần `/proc` |
| Kho mật mã (Keychain) + lớp trừu tượng dùng chung | 200–300 | Android dùng Keystore cùng giao diện này |
| CMake/Xcode, ký, gói, PrivacyInfo | 300–500 | |

---

## 3. Tám thứ chặn thật, xếp theo mức nặng

### 3.1 Đường phát hành — nặng nhất, và **không phải việc kỹ thuật**

Android: chủ tự dựng APK, máy chủ tự phát, app tự cập nhật. iOS **không có đường nào như vậy**:

| Đường | Được gì | Mất gì |
|---|---|---|
| App Store | phát hành công khai | phải qua duyệt; một game VLTK không có giấy phép của chủ sở hữu gần như chắc chắn bị trả hồ sơ theo mục sở hữu trí tuệ |
| TestFlight | không lên chợ, vẫn cài được qua liên kết | vẫn phải qua một lượt duyệt cho bản ngoài nhóm nội bộ; trần số người thử; bản thử có hạn dùng và phải nộp lại |
| Chợ thay thế (chỉ trong EU) | không qua App Store | chỉ áp dụng cho thiết bị ở EU |
| Ký bằng tài khoản cá nhân, tự cài | không qua ai | mỗi máy phải gắn cáp/chạy công cụ; hồ sơ hết hạn 7 ngày (tài khoản miễn phí) hoặc 1 năm |

**Đây là câu hỏi phải hỏi chủ trước tiên**, vì nếu câu trả lời là "chỉ phát cho một nhóm nhỏ" thì
toàn bộ phần bộ tải dữ liệu, cổng phiên bản và chính sách lưu trữ có thể làm gọn hơn nhiều.

> Lưu ý: mọi điều khoản của Apple phải **kiểm lại tại thời điểm làm**, tôi ghi theo hiểu biết chung
> chứ không mở trang của Apple trong phiên này.

### 3.2 Không có `dlopen` kiểu Android

`LoadLibraryA` hiện dò `libRainbow.so`, `libRepresent3.so` rồi `dlopen`. Trên iOS mọi thứ nên nằm
trong **một nhị phân tĩnh**. Cách sửa sạch và **không đụng Android**: giữ nguyên `LoadLibraryA`, thêm
nhánh `#ifdef JX_IOS` trả về một **bảng tra tĩnh** `{ "Rainbow.dll" → bảng hàm, "Represent3.dll" → bảng hàm }`.
Chỉ có hai chỗ gọi thật cần bảng này: `Rep3_JxEpTrinhChieu` (`KSdlApp.cpp:1230`) và
`CreateTextFilter` (`Ui/FilterTextLib.cpp:40`), cộng điểm vào của Rainbow.

### 3.3 Shader: chỉ có SPIR-V

`D3D9onGPUDev.cpp` xin `SDL_GPU_SHADERFORMAT_SPIRV`; nguồn có `Rep3ShadersGPU.vert/.frag` và bản
sinh `Rep3ShadersGPU_spv.h`. Metal cần **MSL**. Cách rẻ nhất: thêm một bước trong
`ReverseTools/mobile_x64/dich_shader_gpu.py` chạy SPIRV-Cross ra `.metal`, sinh
`Rep3ShadersGPU_msl.h` **song song, không đụng tệp cũ**, rồi chọn định dạng theo nền lúc tạo thiết bị.

**Bẫy đã thấy trong nguồn:** bản Android bật ba biến thể dùng *storage buffer* trong fragment
(`JX_PAL_BUFFER`, `JX_PS_BUFFER`) và *texture mảng 2D* (`JX_TEX_ARRAY`). Ba thứ này Metal có, nhưng
**số lượng và thứ tự binding của SDL_GPU khác nhau giữa các nền**, và bài học "giữ nguyên byte mảng
shader PC bằng macro" đã ghi ở commit `24e25cae` cho thấy vùng này rất dễ vỡ. Đề nghị: **bản iOS đầu
tiên tắt cả ba biến thể** (đi đúng đường đã chạy 08/09), bật lại từng cái sau khi có số đo.

### 3.4 Lớp Java phải viết lại

4 lớp: `JxActivity` (toàn màn hình + vùng an toàn), `JxDoNhip` (nhịp vẽ), `TaiDuLieuActivity`
(bộ tải), `JxTepProvider` (đưa APK cho trình cài — **bỏ hẳn trên iOS**). Phần logic C++ bên dưới
không đổi vì nó nói chuyện qua SDL và qua tệp, không qua JNI: cả dự án chỉ có **2 tệp** chạm tới JNI.

### 3.5 `/proc` không tồn tại

4 chỗ: `KPosixWin32.cpp:351, 359, 447, 903`. Thay bằng `task_info`, `proc_pidinfo`,
`os_proc_available_memory`, `ProcessInfo.thermalState`. Bảng đo hiệu năng mobile
(`JxPerfHudAndroid.cpp`, 35 KB) phải có bản iOS — **nhưng chỉ bản gỡ lỗi**, không nên nộp kèm bản phát hành.

### 3.6 Mạng chỉ IPv4 — rủi ro bị trả hồ sơ

`Sources/MultiServer/Common/Socket.cpp:165` dùng `inet_addr(...)` trên chuỗi địa chỉ,
`Utils.cpp:636,700` dùng `gethostbyname`. Máy chủ tài khoản lại **trả về địa chỉ IP dạng số** cho máy
chủ game (chính là gốc lỗi `127.0.0.1` đã sửa ở `BANGIAO_ANDROID_PHA4_0809.md` §2.3). Trên mạng chỉ
có IPv6, nối tới một địa chỉ IPv4 dạng số **thất bại**, trong khi nối qua **tên miền** thì hệ thống tự
tổng hợp địa chỉ. Cách sửa đúng, làm chung cho cả ba nền: đổi sang `getaddrinfo` và **để máy chủ trả
tên miền thay vì số**. Đây là việc chạm cả máy chủ nên phải xếp sớm.

### 3.7 Bộ nhớ

iOS giết ứng dụng vượt ngưỡng **không báo trước** và ngưỡng thấp hơn Android nhiều. `Rep3CacheMB` đang
để 1500 (`LOTRINH_MOBILE_0909.md` §3.1 đã cảnh báo). Trên iOS phải tính theo `os_proc_available_memory`,
không theo tổng RAM máy.

### 3.8 Dữ liệu 9 GB

Ba việc bắt buộc, làm sai là bị trả hồ sơ: (a) đặt đúng thư mục dữ liệu tải về, (b) **bật cờ loại trừ
sao lưu** cho toàn bộ cây dữ liệu, (c) khai `PrivacyInfo.xcprivacy`. Mục (c) là thật và cụ thể: mã đang
gọi `statvfs` (`KPosixWin32.cpp:555`, hàm `GetDiskFreeSpaceExA`) và đọc **thời gian sửa tệp** — cả hai
đều thuộc nhóm API phải khai lý do sử dụng.

---

## 4. Phản biện tài liệu hiện có

**4.1. "Chặn thật sự của iOS là shader MSL"** (`KEHOACH_GIAODIEN_MOBILE_1209.md` §5).
Đúng nhưng **không đủ**. Shader là việc nhỏ nhất trong bốn việc chặn. Xếp lại theo công thật:
phát hành > vỏ ứng dụng + bộ tải > bảng ký hiệu tĩnh > shader.

**4.2. "+1 tháng cho iOS"** (`PHUONGAN_NANG_MOBILE_0809.md` §4).
Lạc quan. Con số đó chỉ tính phần vẽ. Cộng vỏ ứng dụng, bộ tải, chính sách lưu trữ, rà chính sách và
một vòng duyệt, **ước 5–8 tuần** kỹ thuật, chưa tính thời gian chờ Apple.

**4.3. "Mã mới rào `JX_MOBILE` để iOS dùng lại"** (`GHICHU_PHIEN_1209.md` §1.1).
Đúng hướng nhưng **chưa ai làm**: đếm được **0 chỗ** có `JX_MOBILE` hoặc `JX_IOS` trong toàn bộ nguồn.
Trong khi đó `JX_ANDROID` đã 78 chỗ và đang tăng mỗi ngày. **Mỗi ngày trôi qua giá chuyển đổi lại đắt thêm.**
Đề nghị: làm bước tách macro **ngay**, như một bước riêng, thuần cơ học, chứng minh không đổi nhị phân.

**4.4. "Bản PC không đổi"** — hiện **chưa đúng tuyệt đối**.
Chính `PHANTICH_GIAODIEN_MOBILE_KEHOACH1209.md` §7 ghi: khoảng **467 dòng** thêm vào `UiToaDo.cpp`
nằm **ngoài** rào `JX_ANDROID`; chúng trơ về hành vi nhưng làm **nhị phân PC dựng từ `mobile-0809`
khác `main`**. Thêm nền thứ ba sẽ làm kỷ luật rào khó giữ hơn. Đề nghị đưa việc chứng minh thành
**máy móc**: mỗi lần giao, dựng Win32 từ nhánh và so mã băm với bản dựng từ `main`; lệch thì chỉ ra
đúng tệp.

**4.5. Bản Android hiện **không có lớp chống gian lận nào** (`LOTRINH_MOBILE_0909.md` §8.6: `AntiHack`
là mã Windows, đã bị loại). Trên iOS còn ít cửa hơn: không có cách nào chạy mã kiểm tra ngoài tiến
trình. Kết luận thẳng: **chống gian lận cho mobile chỉ có một chỗ làm được, là máy chủ.** Mọi thứ làm
ở client chỉ làm chậm chân người gian lận. Việc này nên vào kế hoạch máy chủ, không nên treo vào iOS.

---

## 5. Bảo mật phải làm từ đầu — sáu việc, làm trên Android trước

Chủ yêu cầu "bảo mật ngay từ đầu, tránh quay lại làm tốn thời gian". Đây là sáu việc mà **nếu lùi lại
sau sẽ phải sửa cả hai nền**, xếp theo mức thiệt hại nếu bỏ qua:

| # | Việc | Hiện trạng | Vì sao không lùi được |
|---|---|---|---|
| 1 | **Ký bản kê tệp tải về** | `manifest.txt` chỉ có md5, **không có chữ ký**; tải qua HTTP thường | Người đứng giữa đổi được một tệp pak hoặc một tệp Lua là **điều khiển được client**. Lua client là mã chạy thật, không phải dữ liệu chết. Sửa sau = phải đổi cả máy chủ tải, cả bộ đóng gói, cả hai client |
| 2 | **HTTPS cho mọi thứ tải về** | `usesCleartextTraffic="true"`, máy chủ HTTP cổng 8765 | iOS bắt buộc; làm sớm thì Android hưởng luôn, làm muộn thì phải đổi hạ tầng lúc đang chạy thật |
| 3 | **Kho lưu mật mã** | mật mã lưu ở `UserData\UiCommon.ini` dạng băm MD5 rồi mã hoá bằng `EDOneTimePad`; tệp nằm trong thư mục dữ liệu **người dùng đọc được** | iOS phải là Keychain. Phải có **một lớp trừu tượng dùng chung** ngay bây giờ, nếu không sau này phải sửa `UiLogin.cpp` cho từng nền |
| 4 | **Thư mục dữ liệu + cờ loại trừ sao lưu** | Android chọn thư mục theo danh sách 9 ứng viên, có cả `/sdcard/...` | Đặt sai trên iOS là bị trả hồ sơ; sửa sau nghĩa là người chơi phải tải lại 9 GB |
| 5 | **Dọn nhật ký chẩn đoán** | `[DPG]`, `[LOGIN]`, `[GO]`, `[FONT]`, `[NGOAIHINH]`, `jx_net_sdl.log`… còn bật; `BANGIAO_ANDROID_DULIEU_1109.md` đo **212 MB nhật ký** nằm trong cây dữ liệu | Nhật ký lộ tên tài khoản, địa chỉ máy chủ, khung gói. Cần một công tắc **một chỗ** cho cả ba nền, không phải gỡ tay từng chỗ |
| 6 | **Cổng phiên bản + ép cập nhật** | Android tự tải và tự cài APK | iOS không tự cài được; phải có màn hình "bản cũ, mở App Store" **và** máy chủ phải từ chối client quá cũ. Đây là thay đổi giao thức bắt tay, làm sớm thì rẻ |

Ba việc **không nên** làm vì tốn công mà lợi ít trên mobile: che giấu mã máy, tự kiểm tra toàn vẹn
nhị phân, phát hiện máy đã bẻ khoá. Cả ba đều bị vượt trong vài giờ và không thay được xác thực phía
máy chủ.

---

## 6. Lộ trình — tám bước, **bước nào xong hẳn bước đó**

Mỗi bước có: mục tiêu một câu, việc, **mốc nghiệm thu đo được**, và rào để PC/Android không đổi.
Không bước nào được bắt đầu khi bước trước chưa nghiệm thu.

### Bước 0 — Chốt đường phát hành và dựng bàn làm việc *(chủ quyết, 0 dòng mã)*
- Trả lời 5 câu ở mục 8.
- Chuẩn bị: máy Mac (Apple silicon), Xcode, tài khoản nhà phát triển, **một iPhone thật** còn ≥ 20 GB trống.
- **Nghiệm thu:** chạy được một ứng dụng trống lên chính chiếc iPhone đó. Chưa qua được mốc này thì
  mọi bước sau đều là đoán.

### Bước 1 — Tách macro `JX_MOBILE` / `JX_IOS` *(thuần cơ học, không đổi hành vi)*
- Định nghĩa `JX_MOBILE` cho cả Android và iOS trong `android/CMakeLists.txt`.
- Duyệt **78 chỗ** `JX_ANDROID`, chỗ nào là "vì là điện thoại" thì đổi thành `JX_MOBILE`, chỗ nào là
  "vì là Android" thì giữ. Sửa bằng kịch bản python latin-1 theo đúng luật hiện hành.
- **Nghiệm thu:** APK dựng lại có **cùng mã băm hành vi** (chạy thử 5 phút, ảnh chụp trùng); hai chuỗi
  Windows dựng **0 lỗi**; bản Win32 so với `main` không lệch thêm tệp nào.
- **Vì sao đứng ở đây:** làm sau thì phải sửa tay từng chỗ giữa lúc hai nền đang chạy.

### Bước 2 — Sáu việc bảo hộ ở mục 5, làm **trên Android**
- Làm từng việc, mỗi việc một bản APK riêng để chủ thử.
- **Nghiệm thu:** bộ tải từ chối một tệp bị sửa ruột (thử bằng cách đổi 1 byte trên máy chủ); mật mã
  không còn nằm dạng đọc được trong cây dữ liệu; bật một công tắc là tắt sạch nhật ký chẩn đoán; máy
  chủ từ chối client giả cũ.
- **Vì sao đứng ở đây:** iOS thừa hưởng toàn bộ, và Android được lợi ngay hôm nay.

### Bước 3 — Dựng được nhị phân iOS *(chưa cần lên hình)*
- Thêm `ios/CMakeLists.txt` **song song** với `android/CMakeLists.txt`, dùng lại `android/lists/*.cmake`.
- Link **tĩnh** mọi thư viện; `LoadLibraryA`/`GetProcAddress` đi bảng tra tĩnh.
- Thay 4 chỗ `/proc`; thay chọn thư mục dữ liệu; điểm vào `JxIosMain.mm` đối xứng với `JxAndroidMain.cpp`.
- **Nghiệm thu:** ứng dụng chạy trên máy thật, hiện đúng hộp thoại *"Không thấy config.ini"* rồi thoát
  sạch. Đây là mốc chứng minh **toàn bộ 400 nghìn dòng đã biên dịch, link và khởi động được trên iOS**.

### Bước 4 — Lên hình: shader MSL + SDL_GPU Metal
- Sinh `Rep3ShadersGPU_msl.h` bằng SPIRV-Cross, thêm **song song** bản `_spv.h` (không đụng tệp cũ).
- Tắt ba biến thể `JX_TEX_ARRAY` / `JX_PAL_BUFFER` / `JX_PS_BUFFER` ở bản iOS đầu.
- **Nghiệm thu:** chép tay dữ liệu vào máy, vào tới **màn hình đăng nhập**, chữ hiện đúng, danh sách
  máy chủ đủ. So ảnh chụp với bản Android cùng độ phân giải.

### Bước 5 — Chơi được bằng ngón tay
- Vòng đời (ra nền / vào lại / cuộc gọi đến), toàn màn hình, vùng an toàn, bàn phím mềm, nhịp vẽ.
- **Nghiệm thu:** đăng nhập → vào bản đồ → đánh nhau → mở 10 cửa sổ hay dùng → ra nền 30 giây rồi vào
  lại không sập. Chạy đúng bảng kiểm §8.8 của `BANGIAO_ANDROID_PHA4_0809.md`.

### Bước 6 — Bộ tải dữ liệu iOS
- Viết lại `TaiDuLieuActivity` bằng Swift/Obj-C, **cùng bản kê, cùng chữ ký** như bước 2.
- Đặt thư mục đúng chỗ + cờ loại trừ sao lưu + kiểm chỗ trống trước khi tải.
- **Nghiệm thu:** máy sạch, cài xong, tải hết ≈ 9 GB, rút mạng giữa chừng rồi nối lại vẫn tải tiếp,
  vào thẳng game.

### Bước 7 — Hiệu năng, nhiệt, pin
- Trần bộ nhớ theo `os_proc_available_memory`; nhịp vẽ theo `ProcessInfo.thermalState`;
  áp lại các bài học đã đo ở `PHUONGAN_FPS_NHIET_PIN_MOBILE_1109.md`.
- **Nghiệm thu:** một trận Tống Kim đầy đủ, ghi lại khung hình/giây, nhiệt, phần trăm pin tụt — so với
  bảng số của Fold 7.

### Bước 8 — Đóng gói và nộp
- Biểu tượng, tên, ảnh giới thiệu, `PrivacyInfo.xcprivacy`, khai quyền riêng tư, gỡ mọi bảng đo.
- **Nghiệm thu:** một bản TestFlight người ngoài cài được và chơi được.

---

## 7. Luật giữ cho PC và Android không đổi

1. **Ba rào, ba nghĩa khác nhau** — dùng sai là gốc của mọi lỗi sau này:
   `JX_POSIX` = không phải Windows · `JX_MOBILE` = điện thoại (cả hai) · `JX_ANDROID` / `JX_IOS` = đúng một nền.
2. **Không bao giờ sửa mã chung mà không có rào.** Việc 467 dòng ngoài rào trong `UiToaDo.cpp` là bài
   học đã có giá.
3. **Thêm tệp mới thay vì sửa tệp cũ** ở những chỗ có thể: `ios/`, `JxIosMain.mm`, `Rep3ShadersGPU_msl.h`,
   `UiToaDoIos.inc` — Android và PC không bao giờ thấy chúng.
4. **Nguồn có byte cao (TCVN3/GBK) chỉ sửa bằng kịch bản python latin-1**, đếm số byte ≥ 0x80 trước và
   sau, phải bằng nhau.
5. **Mỗi lần giao: dựng đủ hai chuỗi Windows 0 lỗi + một APK chạy thử**, rồi mới tính là xong bước.
6. **Không dựng/đo khi đang có trận Tống Kim.** Không đụng cây chạy thật.
7. `android/gen_lists.py` và `gen_winshim.py` là bộ sinh — **iOS phải dùng chung**, không chép tay danh
   sách tệp, nếu không hai nền sẽ trôi khỏi nhau sau vài tuần.

---

## 8. Chủ cần quyết trước khi bắt đầu

1. **Phát hành iOS bằng đường nào** — App Store, TestFlight, hay chỉ cài cho một nhóm nhỏ? Câu trả lời
   đổi hẳn khối lượng của bước 6 và bước 8.
2. **Có Mac + iPhone thật + tài khoản nhà phát triển chưa?** Chưa có thì bước 0 chưa qua được.
3. **Có làm bước 2 (sáu việc bảo hộ) trên Android trước không**, hay để iOS tự lo? Tôi đề nghị làm
   trước — rẻ hơn hẳn.
4. **Có đổi máy chủ sang trả tên miền thay vì địa chỉ số không?** Cần cho mục 3.6, và cũng làm bản
   Android bớt phụ thuộc IP LAN.
5. **Bản iOS đầu tiên nhắm máy nào?** Nếu chấp nhận iPhone từ 2018 trở lên thì Metal và bộ nhớ dễ thở;
   nếu muốn cả máy cũ hơn thì phải đo lại trần bộ nhớ.

---

## 9. Độ chắc của số liệu

- Mọi con số đếm mã là đếm thật bằng `grep`/`wc` trên HEAD `ee3c3cdd`, **không phải ước**.
- Số liệu dung lượng dữ liệu lấy từ `BANGIAO_ANDROID_DULIEU_1109.md` (đo 11/09), tôi không đo lại.
- Ước dòng cho phần viết mới là **ước**, dựa trên cỡ các tệp tương ứng bên Android
  (`TaiDuLieuActivity` + `JxActivity` + `JxDoNhip` + `JxPerfHudAndroid.cpp`).
- Điều khoản của Apple ghi theo hiểu biết chung, **phải kiểm lại tại thời điểm làm** — tôi không mở
  trang của Apple trong phiên này.
- **Không biên dịch, không chạy, không sửa một dòng mã nào trong phiên này.**
