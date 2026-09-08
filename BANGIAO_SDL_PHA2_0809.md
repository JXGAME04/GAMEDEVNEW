# BÀN GIAO — PHA 2 (lát 2a): LỚP NỀN SDL3 CHO CLIENT, `GameSDL.exe` TRÊN WINDOWS (08/09/2026)

Nhánh `mobile-0809` (worktree `D:\GAMEDEVNEW_wt_mobile`). Tiếp theo pha 0 (`BANGIAO_X64_PHA0_0809.md`) theo lộ trình
`D:\GAMEDEVNEW\PHUONGAN_NANG_MOBILE_0809.md` §4. Chủ chốt 08/09 14:2x: *"làm tiếp cho đến khi có thể test được"*; dữ liệu 9,6 GB
chủ giao phiên khác, làm sau.

## 0. Mốc test cho chủ

`D:\GAMEDEVNEW_wt_mobile\bin\client64\ChoiGameSDL64.bat` → `GameSDL.exe` (cùng dữ liệu, cùng `config.ini` với `Game.exe` x64).
**Yêu cầu: hành vi phải giống hệt `Game.exe` x64** — vì lát này chỉ đổi cửa sổ + vòng lặp + đầu vào, hình vẫn Represent3 (D3D9 qua HWND của SDL).
Kiểm: vào game; chuột trái/phải/giữa, kích đúp, kéo cửa sổ, lăn chuột; phím tắt, gõ chat (Telex tiếng Việt của game), Enter/Backspace/Esc/Tab,
Ctrl+V; Alt+Tab rồi quay lại; Alt+F4 / nút X hỏi thoát; WAuto gửi lệnh (WM_COPYDATA) vẫn vào; thoát rồi vào lại.
Khác biệt chấp nhận ở lát này: biểu tượng cửa sổ mặc định của SDL; (có thể) con trỏ chuột riêng của game đôi lúc về mũi tên.

## 1. Vì sao lớp nền SDL3 là bước bắt buộc trước Android

Android không có Win32: cửa sổ, vòng lặp thông điệp, chuột/phím/chạm, vòng đời ứng dụng đều qua SDL. Làm lớp này trên Windows trước để
chủ kiểm được hành vi không đổi, rồi cùng mã đó chạy trên Android (SDL_main + Represent4).

## 2. Thiết kế (đọc trước khi sửa)

| Thành phần | Ở đâu | Ghi chú |
|---|---|---|
| `KSdlApp : public KWin32App` | `Sources/S3Client/Platform/KSdlApp.{h,cpp}` | chỉ biên dịch khi `JX_PLATFORM_SDL`; đè `Init/Run/ShowMouse` |
| Cửa sổ | `SDL_CreateWindow(WND_INIT_WIDTH x WND_INIT_HEIGHT)` → `SDL_PROP_WINDOW_WIN32_HWND_POINTER` → `g_SetMainHWnd/g_SetDrawHWnd` | Represent3, `SetWindowText`, `MessageBox` cũ vẫn dùng HWND này |
| Vòng lặp | `SDL_PollEvent` → dịch → `SDL_WaitEventTimeout` tối đa hết khe 8 ms → `GenerateMsgHoverMsg` + `GameLoop` | y hệt `KWin32App::Run` (lưới 8 ms chia hết 55,56 ms) |
| Đầu vào | sự kiện SDL → **WM_\*** (MOUSEMOVE/L·R·M BUTTON DOWN/UP/DBLCLK, MOUSEWHEEL, KEYDOWN/UP, SYSKEY\*, CHAR, ACTIVATEAPP) → `KWin32App::MsgProc` cũ | giữ bookkeeping hover, `HandleInput` → `UiProcessInput` → `Wnd_ProcessInput`; bảng `SdlKeyToVk` |
| WM_CHAR | `SDL_EVENT_TEXT_INPUT` (UTF-8 → bảng mã hệ thống) + tự sinh cho Enter/Backspace/Tab/Esc/Ctrl+chữ | Windows `TranslateMessage` vốn sinh các ký tự này |
| Thoát | `SDL_EVENT_QUIT`/`CLOSE_REQUESTED` → `HandleInput(WM_CLOSE)` (hộp hỏi thoát) | KHÔNG đưa WM_CLOSE vào `MsgProc` (nó `DestroyWindow` cửa sổ của SDL) |
| Thông điệp Windows đặc thù | `SDL_SetWindowsMessageHook`: `WM_COPYDATA` (WAuto/script), `WMAPP_TRAY`, `TaskbarCreated` → `MsgProc`; `WM_SETCURSOR` bị chặn để `UiCursor::SetCursor` không bị SDL đè | chỉ có trên Windows |
| `KMyApp` | `S3Client.h`: `class KMyApp : public KPlatformApp` (`typedef KSdlApp`/`KWin32App` theo define) | bản PC thường = typedef thuần, mã máy không đổi |
| Tên exe | `KMyApp::GameInit` chấp nhận thêm `GameSDL.exe` khi `JX_PLATFORM_SDL` | kiểm tra chống đổi tên cũ |
| `KWin32App.h` | `private:` → `protected:` (hover/timer) | bố cục lớp không đổi |
| Cấu hình | `S3Client.vcxproj` `ReleaseSDL|x64` (`ReverseTools/mobile_x64/them_sdl_cfg.py`): `JX_PLATFORM_SDL;SDL_MAIN_HANDLED`, include/lib `$(JX_SDL3_DIR)` = `ThirdParty\SDL3`, ra `x64\ReleaseSDL\GameSDL.exe`, post-build chép `GameSDL.exe` + `SDL3.dll` vào `bin\client64` | Engine/Core/Represent3 dùng lại nguyên DLL x64 |
| SDL3 | `ThirdParty/SDL3` = gói `SDL3-devel-3.2.14-VC` (chủ duyệt cách 1, 14:30): `include`, `lib\x64\SDL3.lib/.dll`, LICENSE zlib | trong git (lib/x64 add -f vì `.gitignore` nuốt `x64/`) |

Không đụng ở lát này (vẫn Win32): tệp/pak (`CreateFile`), luồng/`CRITICAL_SECTION`, `timeGetTime`, Winsock (`KNetClient`), âm thanh (DirectSound/mp3),
IME (`KIme`, chỉ trong `WndEdit`; SDL text input đang bật thường trực), `GetKeyState` cho phím tổ hợp trong `Wnd_ProcessInput`.

## 3. Build

```
powershell -File ReverseTools\mobile_x64\build_thap.ps1 -Proj Sources\S3Client\S3Client.vcxproj -Cfg ReleaseSDL -Plat x64 -Tag s3client_sdl
```
(cần `Lib\release64\CoreClient.lib/engine.lib` từ chuỗi x64 đã build). Post-build tự chép vào `bin\client64`. Ngoài giờ trận (x:10–x:40).

## 4. Việc tiếp theo (thứ tự đề nghị)

1. Chủ test mốc §0 → sửa lệch (con trỏ, phím, IME) cho tới khi giống Game.exe.
2. Lát 2b: tệp/đường dẫn/luồng/timer → SDL3 (`SDL_IOStream`, `SDL_Thread`, `SDL_Mutex`, `SDL_GetTicks`) sau `#ifdef JX_PLATFORM_SDL` trong Engine;
   Winsock → BSD socket (`IClient`); âm thanh → miniaudio. Mỗi lát vẫn ra `GameSDL.exe` test trên PC.
3. Pha 1 (clang) song song: `ReverseTools/mobile_x64/clang_survey.py` + `survey_winapi.py` (kết quả `clang_survey/TONGHOP.md`, `WINAPI_TONGHOP.md`).
4. Pha 3 Represent4 (SDL_GPU) sau khi chốt phép thử 2.000 sprite; pha 4 Android.

## 5. Bẫy đã dính

- Heredoc Bash cắt `\\` → mọi vá phải viết ra tệp `.py`; `.gitignore` nuốt thư mục `x64/` (ThirdParty/SDL3/lib/x64 phải `add -f`).
- S3Client dùng PCH `/Yu"KWin32.h"`: `KSdlApp.cpp` phải có `#include "KWin32.h"` là dòng include đầu tiên (mọi thứ trước nó bị bỏ qua).
- Công cụ PowerShell nền có trần 10 phút → build/chờ dài chạy tách rời bằng `Start-Process`; log MSBuild qua `*>>` ra UTF-16, phải `| Out-File -Encoding utf8`.

## 6. Kết quả test lát 2a (chủ, 15:0x): *"oke hết, chỉ có không kết nối được với WAuto ngoài"*

- Toàn bộ bảng kiểm §0 đạt (vào game, chuột/phím/chat, Alt+Tab, thoát, vào lại) trên `GameSDL.exe` 14:47/15:00.
- **WAuto ngoài không thấy game**: `WAuto.cpp EnumWindowsProc` dò cửa sổ bằng `GetClassNameA(hWnd) == "JXWC Class"` (lớp do `KWin32App::InitClass`
  đăng ký) → lấy PID → mở `Local\Auto_Name_MMFSV_<pid>`; cửa sổ SDL mang lớp `SDL_app` nên bị bỏ qua. Sửa (cdee4558): `KSdlApp::Init` gọi
  `SDL_RegisterApp(m_szClass /* "JXWC Class" */, CS_DBLCLKS|CS_BYTEALIGNCLIENT|CS_OWNDC, hInstance)` TRƯỚC `SDL_Init(VIDEO)` (SDL chỉ tự đăng ký khi
  chưa có) + `SDL_UnregisterApp()` sau `SDL_Quit()`; cần `#include <SDL3/SDL_main.h>` (SDL.h không tự include). Bản GameSDL mới dựng sau trận 15:10.
- Bài học: mọi thứ dựa vào **lớp/tiêu đề cửa sổ Win32** (WAuto, chống multibox `ThreadWindowsClassName`, `FindWindow` khác) đều phải được cho
  lớp SDL mang tên cũ; trên Android không có, WAuto ngoài không tồn tại (auto trong game ở Core vẫn chạy).

**15:1x — chủ xác nhận sau bản 15:11:33 (SDL_RegisterApp): *"đã nhận Auto và mọi thứ đã oke hết"* → LÁT 2a NGHIỆM THU.** Quy tắc build mới của
chủ 15:1x: mặc định được build, chỉ dừng khi chủ báo đang có trận Tống Kim.

## 7. Lát 2b-1 (15:2x): thời gian / luồng / khóa / sự kiện / log / tệp pak qua SDL3 — bộ thử riêng `bin\client64sdl`

**Mốc test:** `D:\GAMEDEVNEW_wt_mobile\bin\client64sdl\ChoiGameSDL64.bat`. Khác `bin\client64`: ở đây **Engine.dll và CoreClient.dll là bản SDL**
(cấu hình `ReleaseSDL|x64` / `Client ReleaseSDL|x64`, define `JX_PLATFORM_SDL`, import lib `Lib\release64sdl`), GameSDL.exe cùng loại. Dữ liệu: junction
tới cây live như client64; `config.ini`/settings/script/Ui/UserData là bản sao riêng (`tao_client64sdl.ps1`). Hành vi phải giống hệt bản x64 thường.

Những gì đã đổi khi `JX_PLATFORM_SDL` (Win32 thường: không đổi một byte, mọi khối trong `#ifdef`; chuỗi Win32 chứng minh vẫn build):
| Phân hệ | Cũ (Win32) | Mới (SDL3) | Tệp |
|---|---|---|---|
| thời gian | `timeGetTime/GetTickCount/Sleep`, `QueryPerformanceCounter` | shim macro trong `KWin32.h` → `SDL_GetTicks`, `SDL_Delay`; `KTimer` → `SDL_GetPerformanceCounter` | KWin32.h, KTimer.cpp |
| khóa | `CRITICAL_SECTION` | `SDL_Mutex` (reentrant) | KCriticalSection.h, KMutex.cpp |
| sự kiện | `CreateEvent` auto-reset | `SDL_Semaphore` (Signal chỉ khi đếm 0) | KEvent.cpp |
| luồng | `_beginthreadex/TerminateThread/Suspend` | `SDL_CreateThread/SDL_WaitThread`; Destroy/Suspend/Resume = ghi log (SDL không có) | KThread.cpp |
| log/hộp thoại | DebugWin `WM_COPYDATA`, `MessageBox` | `SDL_Log` (+ DebugWin nếu có), `SDL_ShowSimpleMessageBox` | KDebug.cpp |
| pak | `CreateFile/ReadFile/SetFilePointer`, `CRITICAL_SECTION m_ReadCritical` | `SDL_IOFromFile/SDL_ReadIO/SDL_SeekIO`, `SDL_Mutex` (HANDLE/CRITICAL_SECTION giữ nguyên trong header: chứa con trỏ SDL) | XPackFile.cpp, ZSPRPackFile.h/.cpp |
| thư mục | `CreateDirectory`, `GetFileAttributes` | `SDL_CreateDirectory`, `SDL_GetPathInfo` | KFilePath.cpp |

Chưa đổi (lát sau): `KFile` (đã là stdio), `ZPackFile` (mmap), `KScanDir` (client không dùng), mạng (Rainbow ESClient), âm thanh (DirectSound/mp3),
IME (`KIme`), `S3Client.cpp` (`GetPrivateProfileInt`, `timeBeginPeriod`, CrashLog, AntiHack, con trỏ chuột).

Chạy thử 15:28 (25 s, màn đăng nhập): không sập, tiêu đề cửa sổ "Vo Lam Truyen Ky", Represent3 nạp ảnh bình thường. Build: `build_chuoi_sdl.ps1`.
Bẫy: post-build Core ReleaseSDL không có .pdb → `copy` lỗi chặn chuỗi (đã `if exist`); overload inline `LPCVOID` làm MSVC C2666 (đã bỏ, ép kiểu tại chỗ gọi).

## 8. Lát 2b-3 (15:3x): âm thanh qua miniaudio — cùng bộ thử `bin\client64sdl`

`Sources/Engine/Src/KSoundMa.cpp` (chỉ khi `JX_PLATFORM_SDL`; 4 tệp DirectSound `KDSound/KWavSound/KMusic/KMp3Music.cpp` chắn `#ifndef`) cài lại
đúng các lớp đang có, **giữ nguyên header và bố cục lớp** (Core/S3Client không đổi): `KDirectSound` = `ma_engine`; `KWavSound` = PCM trong bộ nhớ +
`BUFFER_COUNT` (3) `ma_sound` phát chồng, pan/volume đổi từ đơn vị DirectSound (1/100 dB → 10^(v/2000)); `KMusic/KMp3Music` = `ma_decoder` từ bộ nhớ
(đọc qua `KPakFile`, mp3/wav tự nhận) + `ma_sound` stream, `Seek(%)` theo độ dài. Kết quả phụ: **mp3 chạy lại trên 64-bit** (bản x64 thường vẫn stub
vì mp3lib chỉ có x86). `ThirdParty/miniaudio` = `miniaudio.h` master 15:30 08/09 (MIT-0, tải từ github.com/mackron/miniaudio, chủ cho phép tự tải).
Backend: WASAPI (Windows), AAudio/OpenSL (Android) — `MA_ENABLE_ONLY_SPECIFIC_BACKENDS`. Engine.dll bản SDL không còn phụ thuộc dsound.dll.
Chạy thử 15:37 (25 s): không sập, chỉ nạp Engine.dll + SDL3.dll. Chờ chủ nghe thử (hiệu ứng, nhạc, âm lượng).
Chưa làm: log SDL ra tệp (`SDL_SetLogOutputFunction` → `jx_sdl.log`), mạng (2b-2), IME/WndEdit, S3Client.cpp (GetPrivateProfileInt/timeBeginPeriod/CrashLog/AntiHack).
