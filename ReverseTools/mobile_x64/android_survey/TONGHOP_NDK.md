# KHẢO SÁT BIÊN DỊCH CLIENT BẰNG ANDROID NDK (clang++ 14, aarch64-linux-android24, CHỈ KIỂM CÚ PHÁP)

**08/09/2026 — CHỈ ĐỌC NGUỒN + CHẠY `-fsyntax-only`. KHÔNG SỬA MỘT TỆP NÀO TRONG `Sources/` (chỉ TẠO MỚI `Sources/Engine/Src/Platform/KPosixCompat.h`). KHÔNG BUILD, KHÔNG LINK, KHÔNG ĐỤNG `bin\`.**

Worktree `D:\GAMEDEVNEW_wt_mobile`, nhánh `mobile-0809`. Máy thử đang chạy GameServer/Goddess/Bishop + Game.exe nên mọi vòng chạy ở ưu tiên BELOW_NORMAL, 4 tiến trình clang song song.

Tệp đi kèm:
- `ReverseTools/mobile_x64/android_survey/ndk_survey.py` — kịch bản khảo sát, chạy lại được (cách dùng ở docstring đầu tệp).
- `Sources/Engine/Src/Platform/KPosixCompat.h` — BẢN NHÁP lớp tương thích Win32 → POSIX (ASCII, biên dịch được cả C lẫn C++).
- Log thô từng TU: `<scratch>\android_survey\it{0,1,2,3}\<dự án>_raw.log` + `_tomtat.txt` + `TONGHOP_it<N>.md` + `tonghop.json`; bảng so sánh các vòng `<scratch>\android_survey\SOSANH.md`
  (`<scratch>` = `C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\f7fe3690-d1f7-481d-b4f6-845330c7c14f\scratchpad`).

---

## 0. TRẢ LỜI NGẮN

1. **Nhánh `#ifndef __linux` có sẵn (port server Linux cũ) gần như KHÔNG giúp gì cho client**: vòng 0 (không lớp tương thích) chỉ 65/443 TU sạch; Core chết ngay tại `KWin32.h:148` (`#include <winsock2.h>`), Common chết tại `stdafx.h` (`windows.h`). Nhánh Linux của `KWin32.h` còn sai cho client 64 bit (`#define DWORD unsigned long` = 64 bit trên LP64, `HWND unsigned long`, `MAX_PATH 1200`, tự định nghĩa `RECT/POINT/GUID`) → phải thay cả khối bằng `#include "Platform/KPosixCompat.h"`.
2. **Sau 3 vòng với `KPosixCompat.h`: 214/443 TU sạch (48 %), và 341/443 (77 %) sạch nếu bỏ lỗi lan từ header.** 81 % số lỗi còn lại (4 481/5 526) phát sinh từ **9 header "độc"** (31 điểm lỗi, đứng đầu là `KDDraw.h` với 14 điểm → 2 142 lỗi trên 153 TU); 77 % (4 275) nằm trong **33 tệp Windows-only theo bản chất** (DirectDraw/DirectInput/DirectSound/IME/AVI-MP3-MP4/AntiHack/CrashLog/khay hệ thống/PerfHud/JxReplay).
3. **Backlog port thật (ngoài tệp Windows-only) chỉ còn 447 điểm gọi / 125 ký hiệu, tập trung ở 76 TU (511 lỗi riêng).** Nhóm lớn: đồng bộ luồng (`Enter/LeaveCriticalSection` 56 điểm ở một tệp `KScenePlaceC.cpp`, `WaitForSingleObject`, `CreateEvent/SetEvent`), phím/con trỏ/cửa sổ (`GetKeyState` 30, `PostQuitMessage` 9, `LoadCursor/SetCursor`, caret/clipboard), tệp/thư mục (`FindFirstFile` họ, `SetCurrentDirectory`, `DeleteFile`, `SetFileAttributes`), giờ (`GetLocalTime` 19), nạp DLL (`LoadLibrary/GetProcAddress/FreeLibrary` 20), mã hoá chuỗi (`MultiByteToWideChar` 10), và 42 khối `__asm` x86 — **tất cả `__asm` sống đều nằm dưới `#ifdef _WIN64 … #else` (60 guard/18 tệp): bản x64 đã có nhánh C, chỉ cần đổi điều kiện guard.**
4. **Ước lượng để 443 TU biên dịch sạch bằng NDK: ≈ 15 người-ngày (13–19)**: hoàn thiện lớp tương thích 2–3, loại/stub tệp Windows-only + hệ build NDK 3–4, thay API 8–12. Không gồm renderer (Represent4/SDL_GPU), âm thanh miniaudio, giao diện cảm ứng — đó là việc riêng, lớn hơn.
5. Hai bẫy nền tảng cần quyết ngay: **`char` KHÔNG DẤU trên aarch64** (`__CHAR_UNSIGNED__`; mọi phép `if (c < 0)` cho byte TCVN3 > 0x7F đổi nghĩa → dùng `-fsigned-char`), và **LP64** (`long` = 64 bit; `KPosixCompat.h` ép `LONG/DWORD/HRESULT` = 32 bit để bố cục gói/struct giữ nguyên, nhưng mã dùng `long` thô trong gói mạng phải rà).

---

## 1. (a) Dòng lệnh đã dùng

Trình dịch: `C:\Users\nguye\AppData\Local\Android\Sdk\ndk\25.2.9519653\toolchains\llvm\prebuilt\windows-x86_64\bin\clang++.exe` (Android clang 14.0.7, NDK r25c), sysroot `…\prebuilt\windows-x86_64\sysroot`. Mỗi TU một lời gọi, 4 song song, `creationflags=BELOW_NORMAL_PRIORITY_CLASS`, timeout 900 s.

```
clang++ --target=aarch64-linux-android24 --sysroot=<NDK sysroot> -fsyntax-only -x c++ -std=gnu++17 -fms-extensions
        -ferror-limit=200 -w -fno-caret-diagnostics -fno-color-diagnostics -fdiagnostics-absolute-paths
        -D_MBCS -DJX_PLATFORM_SDL -DNDEBUG -DUSE_STANDALONE_SPR -D_LIBCPP_ENABLE_CXX17_REMOVED_FEATURES
        <define của vcxproj, BỎ WIN32 _WINDOWS _USRDLL _LIB WINVER _WIN32_WINNT — GIỮ *_EXPORTS (xem ghi chú)>
        <-I của vcxproj với $(JX_SDL3_DIR)=ThirdParty\SDL3> -I ThirdParty\SDL3\include -I ThirdParty\miniaudio -I <vcpkg include của dự án>
        [vòng ≥1] -I <scratch>\android_survey\stub_include -I Sources\Engine\Src\Platform
                  -include Sources\Engine\Src\Platform\KPosixCompat.h
                  -include <scratch>\android_survey\shadow\KWin32.h      (engine, core, s3client)
                  -include <scratch>\android_survey\shadow\JxNetShim.h   (common, rainbow)
        <tệp .cpp>
.c (20 tệp ucl + CRC32.C): clang -x c -std=gnu11 -Werror=implicit-function-declaration (các cờ khác giống trên, không force-include shadow)
```

Ghi chú phương pháp (quan trọng khi đọc số):
- `__linux` (không gạch dưới cuối) **được clang định nghĩa sẵn** cho target này ở chế độ gnu++ (cùng `__linux__`, `__ANDROID__`, `linux`, `unix`) → không cần `-D__linux`. Mã dùng `#ifndef __linux` 17 chỗ, `#ifdef __linux` 2 chỗ, `__linux__` 5 tệp.
- **`*_EXPORTS` được giữ** (khác đề bài) vì mã dùng chúng như cờ "đang ở trong module", không chỉ để chọn `__declspec`: `CoreShell.h:1269` khai báo `CoreDataChanged` dưới `#ifdef CORE_EXPORTS` (bỏ đi → 415 lỗi giả ở vòng 1), `KWin32.h:148` include `winsock2.h` khi `!ENGINE_EXPORTS`. `__declspec(x)` đã rỗng trong `KPosixCompat.h` nên giữ `*_EXPORTS` vô hại. Vòng 0 được chạy lại với cùng bộ cờ để so sánh công bằng.
- **Stub header** (`windows.h`, `winsock2.h`, `mmsystem.h`, `ddraw.h`, `dinput.h`, `dsound.h`, `d3d*.h`, `atlbase.h`, `crtdbg.h`, `io.h`, `direct.h`, `process.h`, `tchar.h`, `commdlg.h`, `tlhelp32.h`, `dbghelp.h`, `pshpack*.h`/`poppack.h` (có `#pragma pack` thật), `initguid.h` (đặt `INITGUID`) … 150 tên) chỉ chứa `#include "KPosixCompat.h"`, **chỉ để khảo sát** (để TU đi qua dòng `#include` và lộ lỗi ký hiệu thật). Bản port thật không dùng chúng.
- **`KWin32.h` bóng**: bản sao tệp thật do kịch bản sinh, trong đó dòng 51–113 (khối `#else` không-Windows) thay bằng `#include KPosixCompat.h` + nhắc lại ánh xạ SDL sẵn có (`timeGetTime/GetTickCount → SDL_GetTicks`, `Sleep → SDL_Delay`) vốn chỉ nằm trong nhánh Windows. Force-include trước mọi TU nên `#include "KWin32.h"` thật thành no-op (guard `KWin32_H`). Đây là **1 hunk** bản port thật phải sửa.
- **`JxNetShim.h` bóng**: khối POSIX của shim định nghĩa lại `WSABUF/OVERLAPPED/WSADATA` bằng struct vô danh → trùng kiểu với lớp tương thích; bản bóng bỏ 3 dòng đó (**1 hunk**). Force-include cho Common/Rainbow đúng ý đồ của shim (`WSAGetLastError → errno`, `closesocket → close`).
- `-ferror-limit=200`: vòng 3 **không TU nào chạm trần**, không TU nào timeout.
- Trên máy chủ Windows hệ tệp không phân biệt hoa/thường nên `#include "Shlwapi.h"` vẫn tìm thấy stub; con số include sai chữ hoa ở mục (e) là để dành cho CI Linux/macOS.

## 2. (b) Kết quả theo dự án

Cấu hình: Engine `ReleaseSDL|x64`, Core `Client ReleaseSDL|x64` (118 tệp − 15 tệp bị loại trong cấu hình = 103), S3Client `ReleaseSDL|x64` (186, gồm `Platform\KSdlApp.cpp` — vcxproj dùng điều kiện `!=` nên bộ đọc phải xử lý riêng), Common 13/14 tệp yêu cầu (`CriticalSection.cpp` **không có trong vcxproj** — chỉ có `CriticalSection.h`), Rainbow 3.

| dự án | TU | vòng 0: TU sạch / lỗi | vòng 3: TU sạch / lỗi | vòng 3: sạch nếu bỏ lỗi header | lỗi từ header | lỗi trong tệp Windows-only | TU Windows-only |
|---|---|---|---|---|---|---|---|
| engine | 138 (20 .c) | 49 / 1 303 | **70** / 1 034 | 97 | 610 | 817 | 17 |
| core | 103 | 2 / 101 (fatal `winsock2.h`) | 2 / 3 263 | **88** | 3 135 | 2 525 | 0 |
| s3client | 186 | 13 / 1 703 | **131** / 1 181 | 145 | 736 | 933 | 16 |
| common | 13 | 0 / 13 (fatal `windows.h`) | **8** / 48 | 8 | 0 | 0 | 0 |
| rainbow | 3 | 1 / 2 | **3** / 0 | 3 | 0 | 0 | 0 |
| **tổng** | **443** | **65 / 3 122** | **214 / 5 526** | **341** | **4 481 (81 %)** | **4 275 (77 %)** | **33** |

Đọc bảng: "lỗi" tổng TĂNG từ vòng 0 sang vòng 3 là bình thường — ở vòng 0 phần lớn TU chết ngay ở dòng `#include` đầu tiên (1 lỗi fatal) nên không lộ gì; con số có ý nghĩa là **TU sạch** và **lỗi riêng trong chính tệp .cpp** (vòng 3: 511 lỗi riêng trong 76 TU ngoài Windows-only). Diễn biến từng vòng (xem `SOSANH.md`): vòng 1 (lớp tương thích v1: typedef/macro/CRT) 149 sạch; vòng 2 (thêm hằng `WM_/VK_/MK_/MB_`, "secure CRT" `sprintf_s/strcpy_s…` dạng mẫu C++, `IUnknown` tối thiểu, `WIN32_FIND_DATA`, hàm `RECT`, `DEFINE_GUID` theo `INITGUID`, giữ `*_EXPORTS`, bóng `JxNetShim.h`) 213 sạch; vòng 3 (hằng `EXCEPTION_*`, `THREAD_PRIORITY_*`, `FILE_MAP_*`, `IDC_*`, `MulDiv`) 214 sạch — đã bão hoà: phần còn lại không phải việc của header.

Core chỉ 2 TU sạch nhưng 88 TU "sạch nếu bỏ lỗi header": mọi TU của Core kéo `KCore.h → KEngine.h → KDDraw.h/KDSound.h/KDInput.h/KFileDialog.h` và `KMusic.h/KWavSound.h`, `KSubWorld.h`, `KJXPathFinder.h` — sửa 9 header là Core nhảy từ 2 lên ~88.

## 3. (c) Lỗi còn lại

### 3.1 Loại thông báo lỗi (vòng 3, gộp 5 dự án)

| số lần | TU | loại | ghi chú |
|---|---|---|---|
| 3 275 | 185 | `unknown type name 'X'` | ~90 % là kiểu DirectX trong 5 header của Engine |
| 1 766 | 194 | `use of undeclared identifier 'X'` | hàm Win32 + hằng DirectX |
| 105 | 105 | `no matching function for call to '__sift_down'` (libc++ `<algorithm>`) | `KJXPathFinder.h:88` `OpenNodePtr::operator<` **không `const`** → `std::pop_heap` lỗi trên libc++ (MSVC STL bỏ qua); 1 dòng sửa, lan 101 TU |
| 102 | 102 | `invalid operands to binary expression` | cùng gốc trên |
| 46 | 13 | `no member named 'X' in the global namespace` | `::GetLocalTime`, `::SetLastError` |
| 44 | 12 | `unsupported architecture 'aarch64' for MS-style inline assembly` | `__asm` — xem (e) |
| 29 | 6 | `cannot initialize object parameter…` | hệ quả của kiểu thiếu |
| 19 | 6 | `ISO C++17 does not allow 'register'` | 12 chỗ/7 tệp (`KNpcAI.cpp`, `KCodecLzo.cpp`…) — xoá từ khoá |
| 11 | 5 | `typedef redefinition with different types` | `JxNetShim.h` (đã bóng) + `KLVideo.h` |
| 11 | 11 | `ISO C++ forbids forward references to 'enum' types` | MSVC-ism nhỏ |
| 5 | 3 | `SEH '__try' is not supported on this target` | `KRegion.cpp`, `CrashLog.cpp`, `S3Client.cpp` |
| còn lại | | `expected …`, `incomplete type`, `argument may not have 'void' type`… | hệ quả / `KLVideo.h` |

### 3.2 Nhóm ký hiệu thiếu (vòng 3; "điểm gọi" = vị trí file:dòng:cột distinct, header đếm 1 lần)

| nhóm | số ký hiệu | điểm gọi | trong tệp Windows-only | **còn lại (backlog thật)** |
|---|---|---|---|---|
| Win32 API kernel32/advapi32 (luồng, tệp, giờ, DLL, ini) | 84 | 376 | 139 | **237 (60 ký hiệu)** |
| Cửa sổ / GDI / thông điệp (user32/gdi32) | 78 | 220 | 137 | **83 (30)** |
| DirectX (DDraw/DInput/DSound) | 112 | 209 | 190 | 19 (8) — thành viên `KCanvas`, `KSoundMa` |
| Hằng số / kiểu Win32 ALLCAPS khác | 59 | 122 | 103 | 19 (13) |
| MSVC-only (`__asm`, SEH) | 2 | 47 | 5 | **42** (40 `__asm` + 2 `__try`) |
| Dự án (thành viên lớp dưới `#ifdef WIN32`: `m_ThreadHandle`, `m_ThreadId`, `m_CriticalSection`, `m_nFrequency`; `HKLVIDEO`) | 8 | 37 | 19 | 18 (5) |
| IME (imm32) | 15 | 30 | 30 | 0 (`Kime.cpp` loại) |
| Kiểu Win32 khác (`GetLocalTime`, `SetLastError` gọi có `::`) | 3 | 28 | 5 | 23 |
| CRT MSVC (`sscanf_s`) | 3 | 4 | 2 | 2 |
| Winsock / IOCP (`WSAStartup`, `WSACleanup`) | 2 | 2 | 0 | 2 |
| **tổng** | **369** | **1 078** | **631** | **447 (125 ký hiệu)** |

### 3.3 40 ký hiệu Win32 / DirectX / CRT thiếu nhiều nhất — xếp theo **điểm gọi NGOÀI tệp Windows-only** (backlog thật)

| # | ký hiệu | điểm gọi ngoài W-only | TU bị ảnh hưởng | tổng điểm | trong header | thay bằng |
|---|---|---|---|---|---|---|
| 1 | `__asm` (MS inline asm, x86) | 40 | 12 | 42 | 2 | đổi guard `#ifdef _WIN64` → `#if !defined(_M_IX86) && !defined(__i386__)` (nhánh C đã có) |
| 2 | `GetKeyState` | 30 | 10 | 30 | 0 | bảng trạng thái phím trong `KSdlApp` (`SDL_GetKeyboardState`/`SDL_GetModState`) |
| 3 | `EnterCriticalSection` | 28 | 1 | 28 | 0 | `KScenePlaceC.cpp` → `KCriticalSection` (đã có nhánh SDL) |
| 4 | `LeaveCriticalSection` | 28 | 1 | 28 | 0 | như trên |
| 5 | `GetLocalTime` | 19 | 12 | 23 | 0 | `localtime_r` → `SYSTEMTIME` (hàm thật trong `Platform/KPosixWin32.cpp`) |
| 6 | `CloseHandle` | 13 | 9 | 19 | 0 | theo từng handle (thread/event/file) |
| 7 | `WaitForSingleObject` | 12 | 102 | 12 | 4 | `KSubWorld.h:205/222` poll handle luồng nạp lưới → cờ atomic / `SDL_WaitThread` |
| 8 | `GetProcAddress` | 11 | 13 | 44 | 1 | `dlsym` hoặc link tĩnh (`KSortScript` lấy `lua4_*` từ DLL Lua; `KEicScript`) |
| 9 | `GetCurrentProcessId` | 9 | 6 | 12 | 0 | `getpid` |
| 10 | `PostQuitMessage` | 9 | 3 | 11 | 0 | đẩy `SDL_EVENT_QUIT` |
| 11 | `LPDIRECTDRAWSURFACE` | 7 | 162 | 26 | 21 | `KCanvas.h:58/97` thành viên + `BltSurface` → `#ifdef` / handle Represent |
| 12 | `FindClose` | 7 | 4 | 7 | 0 | `closedir` |
| 13 | `FindFirstFile` | 7 | 4 | 7 | 0 | `opendir/readdir` (`KScanDir.cpp`, `KFileCopy.cpp`, `UiBase.cpp`) |
| 14 | `m_ThreadHandle` (`KThread.h` chỉ khai báo dưới `#ifdef WIN32`) | 7 | 1 | 7 | 0 | khai báo thành viên cho nhánh SDL |
| 15 | `DeleteCriticalSection` | 6 | 1 | 6 | 0 | `KCriticalSection` |
| 16 | `FreeLibrary` | 5 | 7 | 12 | 0 | `dlclose` |
| 17 | `FindNextFile` | 5 | 4 | 5 | 0 | `readdir` |
| 18 | `DeleteFile` | 5 | 4 | 5 | 0 | `unlink` |
| 19 | `GetCurrentDirectory` | 5 | 4 | 5 | 0 | `getcwd` |
| 20 | `MultiByteToWideChar` | 5 | 2 | 5 | 0 | `SDL_iconv` (`KStrBase.cpp`) |
| 21 | `WideCharToMultiByte` | 5 | 2 | 5 | 0 | `SDL_iconv` |
| 22 | `SetFileAttributes` | 5 | 2 | 5 | 0 | `chmod` / bỏ |
| 23 | `SetCurrentDirectory` | 5 | 1 | 5 | 0 | `chdir` (`KScanDir.cpp`) |
| 24 | `LPDIRECTSOUNDBUFFER` | 4 | 151 | 13 | 9 | `KMusic.h:20-22`, `KWavSound.h:17` thành viên → `#ifdef` (miniaudio `KSoundMa` đã có) |
| 25 | `LoadLibrary` | 4 | 7 | 8 | 0 | `dlopen` / link tĩnh |
| 26 | `MessageBox` | 4 | 5 | 5 | 0 | `SDL_ShowSimpleMessageBox` (đã dùng ở `KDebug.cpp`) |
| 27 | `SetEvent` | 4 | 4 | 7 | 0 | `KEvent` (nhánh SDL có sẵn) |
| 28 | `GetPrivateProfileIntA` | 4 | 4 | 4 | 0 | `KIniFile` |
| 29 | `ShellExecute` | 4 | 2 | 4 | 0 | `SDL_OpenURL` |
| 30 | `QueryPerformanceCounter` | 4 | 1 | 4 | 0 | `SDL_GetPerformanceCounter` (`KCanvas.cpp`; `KTimer` đã có) |
| 31 | `m_CriticalSection` (`KMutex.h` `#ifdef WIN32`) | 4 | 1 | 4 | 0 | khai báo cho nhánh SDL |
| 32 | `m_ThreadId` (`KThread.h`) | 4 | 1 | 4 | 0 | như trên |
| 33 | `SetLastError` | 4 | 1 | 4 | 0 | `errno` (`Common/Utils.cpp`) |
| 34 | `CreateEvent` | 3 | 3 | 4 | 0 | `KEvent` |
| 35 | `LoadCursor` | 3 | 3 | 5 | 0 | `SDL_CreateSystemCursor` (`UiCursor.cpp`) |
| 36 | `SystemTimeToTzSpecificLocalTime` | 3 | 3 | 3 | 0 | `localtime_r` |
| 37 | `SetCursor` | 3 | 2 | 4 | 0 | `SDL_SetCursor` |
| 38 | `GetPrivateProfileInt` | 3 | 2 | 3 | 0 | `KIniFile` |
| 39 | `InitializeCriticalSection` | 3 | 1 | 3 | 0 | `KCriticalSection` |
| 40 | `ResetEvent` | 3 | 1 | 3 | 0 | `KEvent` |

Ngay sau top 40 (mỗi cái 2–3 điểm): `SetCaretPos` (WndEdit — caret), `OpenClipboard/GetClipboardData` (clipboard → `SDL_GetClipboardText`), `ShowWindow`, `GetModuleHandleA`, `SEH __try` (2 ngoài W-only: `KRegion.cpp`), `DDBLTFX/DDBLT_COLORFILL/CreateSurface` (`KCanvas.cpp`), `CopyFile`, `CreateDirectory` (`mkdir`), `GetTempPath`, `ShowCursor`, `FileTimeToSystemTime/SystemTimeToFileTime`, `OutputDebugString` (`__android_log_print`), `GetCurrentThreadId`, `sscanf_s` (2, `KIniFile.cpp`), `WSAStartup/WSACleanup` (`UsesWinsock.cpp`, thành no-op), `USES_CONVERSION/T2A` (ATL, `Utils.cpp`), `GetFileVersionInfo/VerQueryValue/GetComputerName/FormatMessage/MessageBeep` (`Utils.cpp` — stub), `GetModuleFileName`, `MAX_COMPUTERNAME_LENGTH/UNLEN`.

Ngoài ra `Common\Socket.cpp` (9) và `SocketClient.cpp` (6) còn lỗi kiểu tham số BSD (`socklen_t*` vs `int*`, `u_long`), không phải ký hiệu thiếu.

### 3.4 Header "độc" — sửa 31 điểm là hết 96 % lỗi lan (vòng 3)

| header | điểm lỗi | lỗi sinh ra | TU bị ảnh hưởng | nội dung |
|---|---|---|---|---|
| `Engine\Src\KDDraw.h` | 14 | 2 142 | 153 | `LPDIRECTDRAW*`, `DDSURFACEDESC`, hàm inline dùng `DDLOCK_WAIT/DD_OK/DDBLT_WAIT` — được `KEngine.h` include |
| `Engine\Src\KDSound.h` | 3 | 432 | 144 | `LPDIRECTSOUND(BUFFER)` — `KEngine.h` |
| `Engine\Src\KDInput.h` | 3 | 429 | 143 | `LPDIRECTINPUT8`, `LPDIRECTINPUTDEVICE8` — `KEngine.h` |
| `Engine\Src\KMusic.h` | 3 | 342 | 114 | `LPDIRECTSOUNDBUFFER/NOTIFY`, `DSBPOSITIONNOTIFY` — `KCore.h`, `KNpcRes.h`, `KObj.h` |
| `Engine\Src\KCanvas.h` | 2 | 308 | 154 | thành viên `LPDIRECTDRAWSURFACE m_pSurface`, `BltSurface(...)` |
| `Core\Src\KSubWorld.h` | 2 | 202 | 101 | `WaitForSingleObject(m_hLoadPathGrid, 0)` trong 2 hàm inline |
| libc++ `<algorithm>` (từ `Core\Src\KJXPathFinder.h:88`) | 2 | 202 | 101 | `bool operator<(CONST OpenNodePtr&)` thiếu `const` |
| `Engine\Src\KFileDialog.h` | 1 | 141 | 141 | `OPENFILENAME` — `KEngine.h` |
| `Engine\Src\KWavSound.h` | 1 | 104 | 104 | `LPDIRECTSOUNDBUFFER` |
| (khác) `Headers\DbgHelp.h` (bản chép của dbghelp.h trong repo, 28 điểm, 2 TU), `KLVideo\KLVideo.h` (19 điểm, `UiInit.cpp` + `UiPlayVideo.cpp`) | | | | Windows-only |

`KEngine.h` include thẳng `KDDraw.h KDSound.h KDInput.h KFileDialog.h KWin32App.h` → cần macro `JX_NO_DIRECTX` (Android) bọc 5 include này + thành viên DirectX trong `KCanvas/KMusic/KWavSound`.

## 4. (d) Tệp nên LOẠI khỏi bản Android (Windows-only theo bản chất) — 33 TU + 8 header

Engine (17 TU trong `ReleaseSDL`; `KNetClient/KNetServer/KNetServerNode/KNetThread` không có trong cấu hình và chỉ được Core include dưới dạng chú thích/`#ifdef _SERVER` → không cần bận tâm):

| tệp | lý do |
|---|---|
| `KDDraw.cpp` (+`.h`) | bọc IDirectDraw7 / surface / clipper — thay bằng Represent4 (SDL_GPU) |
| `KDError.cpp` | bảng chuỗi `DDERR_*`/`DIERR_*` của DirectX (68 lỗi riêng, toàn hằng DirectX) |
| `KDInput.cpp` (+`.h`) | DirectInput8 bàn phím/chuột — `KSdlApp` đã dịch SDL_Event → `WM_*/VK_*` |
| `KDSound.cpp` (+`.h`) | DirectSound — `KSoundMa.cpp` (miniaudio) là bản thay thế đã có trong SDL flavor |
| `KKeyboard.cpp`, `KMouse.cpp` | lớp mỏng trên DirectInput (`DIK_*`, `LPDIRECTINPUTDEVICE8`) |
| `KWin32App.cpp` (+`.h`) | vòng lặp `MSG`/`PeekMessage`, `WinMain`-style; `KSdlApp` thay thế |
| `KWin32Wnd.cpp` | tạo cửa sổ Win32 (`CreateWindow`, `ScreenToClient`…). **Giữ `.h`** và làm bản SDL vì `g_GetMainHWnd()` được 26 điểm ở Core/S3Client gọi (`HWND` xuất hiện trong 15 tệp client) |
| `Kime.cpp` (+`.h`) | IME Windows (`Imm*`, 15 ký hiệu/30 điểm) — Android dùng `SDL_StartTextInput` |
| `KFileDialog.cpp` (+`.h`) | hộp thoại `GetOpenFileName`/`OPENFILENAME` (chỉ dùng cho công cụ) |
| `KAviFile.cpp` | AVI qua vfw (biên dịch sạch vì mã bị `#if` tắt, vẫn nên loại) |
| `KMp3Music.cpp`, `KMpgMusic.cpp` | giải mã MP3 qua DLL ngoài (`LoadLibrary`) + DirectSound buffer |
| `KMp4Audio.cpp`, `KMp4Movie.cpp`, `KMp4Video.cpp`, `KVideo.cpp` | phát video MP4 lên DirectDraw surface + `CreateEvent/WaitForMultipleObjects` |

S3Client (16 TU):

| tệp | lý do |
|---|---|
| `CrashLog.cpp` | `SetUnhandledExceptionFilter` + MiniDump (`Headers\DbgHelp.h`), `PIMAGE_*`, `CONTEXT` — Android dùng breakpad/tombstone |
| `AntiHack\CheckSumCRC\{CRC32,CheckSumCRC,ShowLogCRC}.cpp`, `DetectHide`, `DetectWindowsClassName`, `DetectWindowsText`, `DetectWindowsTitle`, `DumpMemory`, `ExistMainName`, `Splash\{Splash,splash2}.cpp` (11 TU) | dò cửa sổ/tiến trình Windows (`FindWindow`, `PROCESSENTRY32`, `TerminateProcess`, `MessageBoxA` ×32, `CreateThread` ×24), splash bằng GDI — vô nghĩa trên Android |
| `JxReplay.cpp` | nạp plugin ghi lại trận (`LoadLibrary(JXREPLAY_MODULE_NAME)`) |
| `Ui\PerfHud.cpp` | đếm hiệu năng qua `psapi.dll`/`pdh.dll` |
| `Ui\TrayMode.cpp` | biểu tượng khay hệ thống (`Shell_NotifyIcon`, `NOTIFYICONDATA`) |
| `Ui\UiCase\UiPlayVideo.cpp` | phát video qua `KLVideo.dll` (`HKLVIDEO`, `GetProcAddress` ×10, `__asm`) — `UiInit.cpp` cũng include `KLVideo.h` (chỉ cần bọc include) |

Header đi kèm cần bọc `#ifdef JX_NO_DIRECTX` (không loại được vì bị include rộng): `KDDraw.h`, `KDSound.h`, `KDInput.h`, `KFileDialog.h` (từ `KEngine.h`), `KMusic.h`, `KWavSound.h` (từ `KCore.h`), `KCanvas.h` (2 thành viên), `KLVideo\KLVideo.h`, `Headers\DbgHelp.h`.

KHÔNG loại mà phải viết lại (không phải Windows-only theo bản chất): `S3Client.cpp` (78 lỗi riêng: `WinMain` + SEH + minidump + `PostQuitMessage` — tách phần Windows sang `CrashLog`, vào game bằng `SDL_main`/`KSdlApp`), `KScenePlaceC.cpp` (83: `CRITICAL_SECTION` thô), `Common\Utils.cpp` (29), `KCanvas.cpp` (16), `KStrBase.cpp` (14), `KFileCopy.cpp` (17), `KScanDir.cpp` (9), `KThread.cpp`/`KTimer.cpp`/`KMutex.cpp` (23: thành viên khai báo dưới `#ifdef WIN32` nhưng nhánh SDL vẫn dùng), `KFile.cpp` (nhánh `#ifndef WIN32` include `zport.h` không tồn tại — tàn dư server Linux), `UiCursor.cpp`, `WndEdit.cpp`, `Wnds.cpp`, `UiBase.cpp`, `KSoundMa.cpp`.

## 5. (e) Cấu trúc chỉ MSVC hiểu (quét tĩnh 1 064 tệp, đã bỏ chú thích; số lần / số tệp)

| cấu trúc | số lần | tệp | clang NDK xử lý | việc phải làm |
|---|---|---|---|---|
| `__asm`/`_asm` x86 | 72 | 14 | **lỗi** (44 lần trong 12 TU) | mọi khối sống nằm dưới `#ifdef _WIN64 … #else __asm` (60 guard / 18 tệp: `KColors`, `KDrawBase`, `KDrawBitmap(16)`, `KDrawFade`, `KDrawFont`, `KDrawSprite(Alpha)`…; bản x64 đã chạy nhánh C) → đổi điều kiện guard, không phải viết NEON |
| `__try/__except/__finally` (SEH) | 10 | 3 (`KRegion.cpp`, `CrashLog.cpp`, `S3Client.cpp`) | **lỗi** | `KRegion.cpp` bỏ SEH; 2 tệp kia Windows-only |
| `register` | 12 | 7 | **lỗi** C++17 (19 lần / 6 TU) | xoá từ khoá |
| `auto_ptr` | 4 | 1 (`Utils.cpp`) | lỗi nếu thiếu `-D_LIBCPP_ENABLE_CXX17_REMOVED_FEATURES` | thêm cờ hoặc `unique_ptr` |
| `OpenNodePtr::operator<` không `const` (`KJXPathFinder.h`) | 1 | 1 | **lỗi lan 101 TU** | thêm `const` |
| `_alloca` | 21 | 8 | OK (macro → `alloca`) | — |
| `__declspec(dllexport/dllimport)` | 13 | 8 | OK (macro rỗng) | `.so` dùng visibility mặc định |
| `__int8/16/32/64` | 273 | 30 | OK (`-fms-extensions`; macro dự phòng cho gcc) | — |
| `__stdcall/__cdecl/WINAPI/CALLBACK/APIENTRY/PASCAL` | 78 | 31 | OK (macro rỗng; clang chỉ cảnh báo) | — |
| `#if _MSC_VER` | 88 | 55 | OK (= 0) | rà các nhánh `#else` lạ (nhiều chỗ là "không phải MSVC = Linux server") |
| `#pragma comment(lib,…)` | 11 | 9 | bỏ qua | thư viện phải khai ở hệ build |
| `#pragma pack` | 37 | 14 | OK | — |
| `#pragma warning` | 31 | 22 | bỏ qua | — |
| `#pragma once` | 201 | 201 | OK | — |
| `#pragma message/region/hdrstop` | 10 | 5 | bỏ qua | — |
| `#pragma optimize/intrinsic` | 2 | 1 (`KInventory.cpp`) | bỏ qua | — |
| `#pragma` MSVC khác | 2 | 1 (`DumpMemory.cpp`) | bỏ qua | Windows-only |
| `#include <windows.h>` trực tiếp | 49 | 46 | fatal nếu không stub | bọc bằng `KWin32.h`/`KPosixCompat.h` |
| `#include <winsock2.h>` trực tiếp | 16 | 14 | fatal | `JxNetShim.h` / `KPosixCompat.h` |
| `#include ddraw/dinput/dsound` | 3 | 1 (`KWin32.h`, nhánh Windows) | — | bọc `JX_NO_DIRECTX` |
| `_M_IX86/_M_X64` | 1 | 1 (`uclconf.h`) | OK | — |
| SSE `_mm_*`/`__m128` | 0 | 0 | — | không có |
| `__super`, `__uuidof`, `__based`, `__if_exists`, `__FUNCSIG__` | 0 trong nguồn | — | (chỉ xuất hiện trong `KPosixCompat.h` dưới dạng macro dự phòng) | — |

Khả năng chuyển host build sang Linux/macOS (không ảnh hưởng NDK trên Windows): `#include` sai chữ hoa/thường so với tên tệp thật: engine 14, core 61, s3client **565**, common 1 (ví dụ `../../../core/src/coreshell.h` ×81, `../elem/wnds.h` ×63, `../../../core/src/gamedatadef.h` ×37, `scene/KScenePlaceC.h`, `kwin32.h`, `Kengine.h`); `#include` dùng `\`: core 42, s3client 2, common 1, rainbow 2. Ngoài ra thư mục include trong vcxproj cũng sai chữ hoa (`..\engine\src`).

Bẫy nền tảng khác đã thấy: `char` không dấu trên aarch64 (`-fsigned-char` để giữ ngữ nghĩa TCVN3); `wchar_t` 32 bit (`WCHAR` trong compat = `wchar_t` để `L"…"` biên dịch được — không dùng cho dữ liệu UTF-16 trên đĩa/mạng); `long` 64 bit; `LONG`/`INT` cùng là `int` nên hàm quá tải `f(LONG)/f(INT)` sẽ trùng (chưa gặp).

## 6. (f) Ước lượng (người-ngày) và 5 bước tiếp theo

Mục tiêu của ước lượng: **443 TU biên dịch sạch bằng NDK** (cú pháp + ngữ nghĩa; chưa link, chưa chạy). Không gồm Represent4 (SDL_GPU/bgfx), giao diện cảm ứng, đóng gói APK, kiểm thử.

| hạng mục | việc | người-ngày |
|---|---|---|
| 1. Lớp tương thích | hoàn thiện `KPosixCompat.h` (1 200 dòng nháp đã có), hunk `KWin32.h` (nhánh `#else`), hunk `JxNetShim.h` (3 typedef), macro `JX_NO_DIRECTX` bọc 5 include trong `KEngine.h` + thành viên DirectX trong `KCanvas/KMusic/KWavSound`, `KSubWorld.h` (2 hàm inline), `KJXPathFinder.h` `const`, tệp `Platform/KPosixWin32.cpp` cho ~15 hàm thật nhỏ (`GetLocalTime`, `QueryPerformanceCounter`, `GetCurrentProcessId`, `OutputDebugString`, `GetPrivateProfileInt` → `KIniFile`…) | **2–3** |
| 2. Loại/stub tệp Windows-only + hệ build | CMake/ndk-build cho engine/core/common/rainbow/s3client với 33 TU loại, cờ (`-fms-extensions -fsigned-char -D_LIBCPP_ENABLE_CXX17_REMOVED_FEATURES -DJX_NO_DIRECTX`), bản SDL của `KWin32Wnd` (giữ `g_GetMainHWnd`), stub `KDInput`-facing (`KKeyboard/KMouse` API mà UI gọi), sửa chữ hoa include khi host là Linux (565+61+14, có thể chạy script) | **3–4** |
| 3. Thay API | 447 điểm / 125 API: đồng bộ luồng (`KScenePlaceC` → `KCriticalSection`, `KThread/KTimer/KMutex/KEvent` bỏ thành viên `#ifdef WIN32`, ~100 điểm) 2; tệp/thư mục (`KScanDir/KFileCopy/UiBase`, ~45) 1; phím/con trỏ/caret/clipboard/quit (`GetKeyState` 30, cursor 10, WndEdit) 2; `S3Client.cpp` → `SDL_main` + tách SEH/minidump 1; DLL (`KSortScript/KEicScript` link tĩnh Lua, `dlopen`) 1; `__asm` guard (60) + `register` (12) + `Utils.cpp` + iconv + lặt vặt 1–3; vòng chạy lại `ndk_survey.py` sau mỗi bước | **8–12** |
| **tổng** | | **≈ 15 (13–19)** |

5 bước tiếp theo, theo thứ tự (mỗi bước đo lại bằng `python ndk_survey.py --iter N`):

1. **Đưa `KPosixCompat.h` vào cây + 3 hunk header (`KWin32.h` nhánh `#else`, `JxNetShim.h` 3 typedef, `KEngine.h` 5 include dưới `JX_NO_DIRECTX`) + 4 sửa nhỏ (`KCanvas.h`, `KMusic.h`/`KWavSound.h` thành viên, `KSubWorld.h`, `KJXPathFinder.h` `const`)** — kỳ vọng nhảy từ 214 lên ≈ 340 TU sạch (đã đo: 341 "sạch nếu bỏ lỗi header"). Khi đó bỏ được cơ chế bóng/stub của kịch bản.
2. **Hệ build NDK thật (CMake) + danh sách loại 33 TU** ở mục (d), cờ `-fsigned-char`; chuyển kịch bản từ `-fsyntax-only` sang `-c` để bắt lỗi codegen (ví dụ `__asm` còn sót, `#pragma comment`). Quyết định luôn: `WCHAR`, `long` trong gói mạng (rà `sizeof` bằng `do_sizeof_goi.py` đã có ở pha 0).
3. **Dịch vụ nền tảng**: hoàn thiện nhánh SDL của `KThread/KTimer/KMutex/KEvent/KCriticalSection` (bỏ khai báo `#ifdef WIN32`), `KScenePlaceC.cpp` sang `KCriticalSection`, `KSubWorld` cờ atomic, `KScanDir/KFileCopy` dirent, `KStrBase` `SDL_iconv`, `Platform/KPosixWin32.cpp` cho `GetLocalTime`/`QueryPerformanceCounter`/… , `Common/Utils.cpp` stub, `Socket.cpp` kiểu `socklen_t`.
4. **Đầu vào/UI trên SDL**: `GetKeyState` → bảng phím `KSdlApp`, `LoadCursor/SetCursor/ShowCursor` → `SDL_Cursor`, caret + clipboard `WndEdit`, `ShellExecute` → `SDL_OpenURL`, `PostQuitMessage` → `SDL_EVENT_QUIT`, `S3Client.cpp` → `SDL_main`; `MessageBox` → `SDL_ShowSimpleMessageBox`.
5. **Đạt 443/443 sạch cú pháp → biên dịch thật + link `.so` (engine/core/common/rainbow) và `libmain.so` (S3Client) với Represent4 rỗng (stub vẽ)**, chạy tới màn đăng nhập trên máy ảo; song song mở nhánh renderer SDL_GPU (`PHUONGAN_NANG_MOBILE_0809.md`). Sau đó mới đến âm thanh (miniaudio đã có), cảm ứng, đóng gói.

---

## 7. Cách chạy lại

```
cd D:\GAMEDEVNEW_wt_mobile\ReverseTools\mobile_x64\android_survey
python ndk_survey.py --iter 0                # nền, không compat
python ndk_survey.py --iter 4                # sau khi sửa nguồn: dùng compat + stub + bóng, so với it3
python ndk_survey.py --iter 4 --no-run       # chỉ phân tích lại log (ví dụ sau khi đổi WINONLY_RE)
python ndk_survey.py --compare               # SOSANH.md
```
Kịch bản chỉ đọc `Sources/`; tệp bóng/stub sinh vào scratch. Khi `KWin32.h` thật đã include `KPosixCompat.h`, bỏ `-include` bóng bằng cách đặt `kwin32=False`/`netshim=False` trong bảng `PROJ`.
