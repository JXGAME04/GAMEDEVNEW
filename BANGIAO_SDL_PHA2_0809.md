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

## 9. Lát 2b-2 (16:0x): mạng `Rainbow.dll` qua BSD socket + SDL3 — cùng bộ thử `bin\client64sdl`

**Nơi mã thật:** `Rainbow.vcxproj` chỉ biên dịch 3 tệp (`ClientStage.cpp` = `CGameClient : IClient + CSocketClient`, `IClient.cpp`, `Rainbow.cpp`)
và link `Lib\release64\common.lib` với include `..\Common` ⇒ lớp mạng nằm ở **`Sources\MultiServer\Common`** (SocketClient/EventSelect/Thread/Event/
CriticalSection/IOBuffer…), KHÔNG phải `Sources\Network\Rainbow\ESClient` (bản sao cũ, khác 400+ dòng, không có bắt tay khoá). Common cũng là lib của
máy chủ (Heaven/Goddess/…) ⇒ mọi thay đổi nằm trong `#ifdef JX_PLATFORM_SDL`, cấu hình `Release|x64` không đổi một byte.

**Luồng gốc (Win32):** `ConnectTo` → socket chặn `connect` → `WaitAndVerifyCipher` (select+recv 34 byte `ACCOUNT_BEGIN`, khoá XOR cố định) →
`WSAEventSelect(FD_CONNECT|FD_CLOSE|FD_READ)` → luồng I/O `Run()`: `WaitForMultipleObjects(shutdown, đã-kết-nối)` rồi `WaitForEnumEvent 1 s` →
`OnRead` = `WSARecv` một phát vào `CIOBuffer` 10 KB → `ReadCompleted` đẩy vào `m_pRecvBuffer` (khoá `m_csReadAction`), luồng chính `GetPackFromServer`
mỗi khung tách gói `[WORD len][payload]`. Gửi: `Write()` = `WSASend` + vòng `select` khi WOULDBLOCK.

**Bản SDL (một mã nguồn; chạy được trên Windows vì Winsock 2 tương thích BSD):**

| Lớp | Win32 | SDL | Tệp |
|---|---|---|---|
| `CEventSelect` | `WSAEventSelect/WSAWaitForMultipleEvents/WSAEnumNetworkEvents` | socket non-blocking + `select(rd,ex)`; `recv(MSG_PEEK)`: >0 = FD_READ, 0 = FD_CLOSE (FIN), lỗi = FD_CLOSE+mã; **FD_CONNECT báo 1 lần sau `AssociateEvent`** (WSAEventSelect ghi nhận cả khi đã nối → callback `enumServerConnectCreate` như bản Win32) | `EventSelect.h` (lớp SDL inline), `EventSelect.cpp` chắn |
| `CThread` | `_beginthreadex/WaitForSingleObject/TerminateThread` | `SDL_CreateThread` (friend `JxThreadProcSdl`), `Wait(ms)` thăm `m_nDone`, `SDL_WaitThread`; `Terminate` = TRACE | `Thread.h` (+2 member SDL), `Thread.cpp` |
| `CEvent` | `CreateEvent/SetEvent/WaitForSingleObject` | `SDL_Mutex + SDL_Condition` + trạng thái manual/auto, `Pulse` bằng bộ đếm thế hệ | `Event.cpp` |
| `CCriticalSection` | `CRITICAL_SECTION` | `SDL_Mutex` (đệ quy) trong `void *m_crit` | `CriticalSection.h` |
| `Run()` | `WaitForMultipleObjects` 2 handle | thăm `shutdown.Wait(0)` + `đã-kết-nối.Wait(50)` (Cleanup trễ ≤ 50 ms) | `SocketClient.cpp` |
| `OnRead` | `WSARecv` | `recv()`; 0 → `StopConnections`; WOULDBLOCK bỏ qua; RESET/ABORTED/NOTCONN → `StopConnections` | `SocketClient.cpp` |
| `Write` | `WSASend` + `select(fd_count/fd_array)` | `send()`; **lỗi ⇒ `dwSendNumBytes = 0`** (bản Win32 giữ số byte cũ → có thể trừ hai lần khi WOULDBLOCK); `FD_ZERO/FD_SET` nạp lại mỗi vòng, `timeval` bản sao; **quá 5 s không gửi được → đóng kết nối** (bản Win32 cũng kết thúc ở đó vì `select` đã xoá fd_set → lỗi) | `SocketClient.cpp` |
| tạo socket / cipher | `WSASocket`, `select(0,…)` | `JxNetCreateTcpSocket()` (Windows vẫn `WSASocket`), `select(s+1,…)` | `SocketClient.cpp`, `JxNetShim.h` |
| khác | `::Sleep(1)` trong `ReadCompleted`, `DllMain` | `SDL_Delay(1)`, `DllMain` chỉ `_WIN32` | `ClientStage.cpp`, `Rainbow.cpp` |

`Sources/MultiServer/Common/JxNetShim.h`: Windows = vài inline (`JxNetLastError/SetNonBlocking/SockError/CreateTcpSocket`, mã lỗi `JX_NET_*`);
khối POSIX (SOCKET/WSABUF/OVERLAPPED/closesocket/FD_*/Interlocked…) **chưa biên dịch** — hoàn thiện ở pha Android cùng lớp đệm kiểu Win32 chung.
Chưa đổi: `Utils.cpp` (`FormatMessage`, `OutputDebugString`, `atlbase.h`), `CUsesWinsock` (WSAStartup), `IOBuffer` (`OVERLAPPED`/`WSABUF`/Interlocked) —
trên Windows chạy nguyên; POSIX qua shim. Các tệp máy chủ trong Common (IOCompletionPort, SocketServer, KSocketClient2…) vẫn biên dịch trong cấu hình
SDL trên Windows nhưng Android sẽ chỉ lấy tập con Rainbow cần.

**Build:** `them_cfg_sdl_net.py` thêm `ReleaseSDL|x64` cho `Common.vcxproj` (→ `Lib\release64sdl\common.lib`) và `Rainbow.vcxproj` (link common.lib SDL +
SDL3.lib → `bin\client64sdl\Rainbow.dll`, import SDL3.dll). `build_chuoi_sdl.ps1` nay: common → rainbow → engine → core → s3client. `tao_client64sdl.ps1`
không chép đè Rainbow.dll nữa. BẪY: post-build gốc của Rainbow dùng `..\..\..\..\bin` (4 cấp = `D:\bin`, đường của cây dự án gốc); bản SDL dùng 3 cấp.

**Kiểm tự động (không cần máy chủ game):** `ReverseTools/mobile_x64/test_rainbow/` — `may_chu_gia.py` (TCP 127.0.0.1, gửi bắt tay `ACCOUNT_BEGIN`, giải mã XOR
khoá client, trả `ECHO:` mã hoá khoá server, `BYE` → đóng; tuỳ chọn `--gop` gộp gói, `--cham` trễ) + `test_rainbow.cpp` (nạp `Rainbow.dll` qua `CreateInterface` →
`IClientFactory` → `IClient` y như `NetConnectAgent`: cổng sai → E_FAIL, kết nối, 5 gói + gói 9.000 B + 1 B, máy chủ đóng → callback Close, `Shutdown/Cleanup/Release`,
kết nối lần 2) + `chay_test.ps1` (dịch cl x64, chạy cho từng thư mục). Kết quả 16:0x: **PASS cả `bin\client64` (Win32) lẫn `bin\client64sdl` (SDL)** — cùng
hành vi: cổng sai thất bại sau ~2 s, trễ hồi đáp 0–16 ms, gói 9.005 B nguyên vẹn, callback Close ~15 ms sau khi máy chủ đóng, kết nối lại OK. Khác biệt duy nhất
lúc đầu: bản SDL thiếu callback `enumServerConnectCreate` (Win32 có vì WSAEventSelect ghi nhận FD_CONNECT sau khi đã nối) → tái hiện ở 2b-2b. Game
(`ClientCallBack` trong `NetConnectAgent.cpp`) chỉ dùng `enumServerConnectClose`.

## 10. Lỗi chủ báo 16:2x với bộ `client64sdl` và cách sửa (2b-2c/2b-2d)

**Chủ báo:** đăng nhập đứng ~5 s mới vào; di chuyển lag; phù về/qua map lag "như mạng yếu".

**Cách tìm:** rà 3 lát (2b-1 thời gian/pak, 2b-3 âm thanh, 2b-2 mạng) — `KThread/KEvent` Engine chỉ dùng ở máy chủ, pak đọc cùng số lần
`ReadFile`; harness echo cơ bản PASS nhưng **đo áp lực** (máy chủ giả đẩy 40–80 gói/s, ping 20/s, `chay_apluc.ps1`) thấy bản SDL đuôi RTT 40–100 ms
(Win32 ≤ 17 ms); rồi **đo trên máy chủ thật** (`test_rainbow.exe <dir> 5622 5 -1 127.0.0.1 3`: chỉ nối + bắt tay khoá rồi ngắt, vô hại): `ConnectTo`
2–10 ms cả hai bản, nhưng **`Shutdown+Cleanup` bản SDL 1.750 ms** (Win32 0,2 ms). Dấu vết `jx_net_sdl.log` (bật bằng tệp `jx_net_trace.on`) cho thấy
`Shutdown` gọi lúc 302 ms, `setsockopt` xong lúc 602, `shutdown()` xong lúc 803 — **mỗi lệnh Winsock từ luồng chính đợi đúng đến lúc `select()` của luồng
I/O hết hạn**.

**Gốc:** `JxNetCreateTcpSocket()` tạo socket bằng `WSASocket(..., 0)` (bắt chước bản Win32) = handle **không overlapped** → Windows xếp hàng MỌI I/O trên
handle đó: `send()`/`setsockopt`/`shutdown`/`closesocket` từ luồng chính phải chờ `select()` (1 s) của luồng I/O trả về. Bản Win32 không dính vì
`WSAEventSelect` chờ trên EVENT (không giữ I/O trên socket). Hệ quả: mỗi gói client gửi trễ tới 1 s (trung bình ~0,5 s khi máy chủ im) → đăng nhập
vài lượt hỏi-đáp = ~5 s; lệnh di chuyển/phù về trễ như mạng yếu.

**Sửa (chỉ `JX_PLATFORM_SDL`):** (1) `WSASocket(..., WSA_FLAG_OVERLAPPED)` (như `socket()` mặc định; POSIX không có vấn đề này); (2) `StopConnections`
gọi `::shutdown(SD_BOTH)` trước `closesocket` để `select` thức ngay; (3) `WaitForEnumEvent` 1000 → 200 ms làm lưới an toàn; (4) dấu vết mạng
`JxNetTraceOn()/JxNetTrace()` (`JxNetShim.h` khai báo, `SocketClient.cpp` định nghĩa) ghi `jx_net_sdl.log` `[ms][luồng] ...` cho conn/cipher/AssociateEvent/
select/peek/recv/send/StopConnections/Cleanup/backpressure; bật khi có tệp `jx_net_trace.on` trong thư mục làm việc hoặc `JX_NET_TRACE=1`.

**Đo sau sửa (16:4x):** `Shutdown 0,2 ms / Cleanup 0,1 ms`; áp lực 80 gói/s 10 s: SDL RTT tb 1,7 ms, p99 2,5, max 2,6 (Win32 1,9 / 6,7 / 10,3);
khoảng cách gói đẩy max 14,6 ms (Win32 19,5). Harness cơ bản PASS, callback create/close như Win32.

**Bài học:** khi thay `WSAEventSelect` bằng `select` trên Windows, socket PHẢI overlapped; và phải đo áp lực + đo trên máy chủ thật, harness echo
đơn lẻ không lộ lỗi xếp hàng I/O. Ghi nhớ `[[jx1-sdl-socket-overlapped-0809]]`.

**Chủ xác nhận 16:5x:** *"đã vào game và mượt rồi hãy tiếp tục"* (bộ client64sdl dựng 16:50: SDL fix + main tới NAP a; Represent3 NAP b chép sau khi game tắt).

## 11. Pha 3 (17:2x): bộ vẽ mobile = lớp **D3D9 trên SDL_GPU** (`Rep3Api=100`) — màn hình chính đã vẽ đúng trên Vulkan

**Quyết định kiến trúc:** không viết `Represent4` mới. Phiên D3D11 đã tách Represent3 thành "logic" (`KRepresentShell3`, `TextureRes*`) gọi vtable D3D9,
và lớp `D3D9on11` cài vtable đó trên D3D11 (`Rep3Api=11`). Mobile đi đúng đường ấy: lớp **`D3D9onGPU`** cài cùng vtable trên **SDL_GPU** (Vulkan
trên Android/PC, Metal trên iOS). Represent3 giữ nguyên; điểm chạm với mã của phiên D3D11 chỉ là nhánh `Rep3Api == 100` trong `D3D_Shell.cpp`
(trong `#ifdef JX_PLATFORM_SDL`) và 3 dòng `ClCompile` trong `Represent3.vcxproj`.

**Tệp (Sources/Represent/Represent3, chỉ biên dịch khi `JX_PLATFORM_SDL`):**
- `D3D9onGPU.h` (API: `Rep3_CreateD3D9onGPU`, `Rep3_GpuVramInfo`, `Rep3_Gpu*Palette`), `D3D9onGPUi.h` (lớp nội bộ),
- `D3D9onGPU.cpp` (IDirect3D9: chế độ màn hình từ SDL, caps như D3D11, bảng định dạng, đổi hàng BGRA8 hai chiều),
- `D3D9onGPURes.cpp` (texture/surface/vertex buffer/state block), `D3D9onGPUDev.cpp` (thiết bị),
- `Rep3ShadersGPU.vert/.frag` (GLSL 450, chuyển từ `Rep3Shaders11.hlsl`) → `Rep3ShadersGPU_spv.h` sinh bởi `ReverseTools/mobile_x64/dich_shader_gpu.py`
  (glslc + spirv-val trong Android NDK r25 `shader-tools`; một định dạng SPIR-V dùng cho cả PC lẫn Android).
- Build: `them_cfg_sdl_rep3.py` thêm `ReleaseSDL|x64` cho Represent3 (→ `bin\client64sdl\Represent3.dll`, link `Lib\release64sdl\engine.lib` + SDL3.lib);
  `build_chuoi_sdl.ps1` nay: common → rainbow → engine → **rep3** → core → s3client. Bật: `Rep3Api=100` trong `bin\client64sdl\config.ini` (lùi: 9).

**Mô hình (khác D3D11 vì SDL_GPU ghi lệnh theo command buffer / render pass, tải dữ liệu phải đi TRƯỚC render pass):**
- Mọi lệnh D3D9 chỉ ghi trạng thái; `Draw*` ghi một `RgCmd` (trạng thái đã tính `RgDrawState`: pipeline, sampler, texture, viewport, scissor, uniform VS/PS)
  + đỉnh chép vào ring CPU (thêm 4 byte PALROW mỗi đỉnh). Quad (strip 4 đỉnh) → 6 đỉnh triangle list, **gộp** vào lệnh trước nếu cùng trạng thái và liền kề.
- `Present` = `SubmitFrame`: 1 command buffer = [copy pass: hàng bảng màu + mọi vùng texture bẩn (transfer buffer) + ring đỉnh] → [render pass theo từng
  target: `Clear` → load-op CLEAR, `SetRenderTarget` → đổi pass; mỗi lệnh chỉ bind phần khác lệnh trước] → [copy khung vừa vẽ sang `m_pLastFrame` để
  `GetFrontBufferData` chụp màn hình] → submit. Cửa sổ thu nhỏ (không có swapchain) → bỏ lệnh khung.
- Texture: **luôn giữ bản CPU** (định dạng D3D9); GPU là "phiên bản" `SDL_GPUTexture`. `LockRect/UnlockRect` chỉ đánh dấu vùng bẩn; lúc bind: nếu phiên bản
  hiện tại đã bị lệnh vẽ trong khung tham chiếu mà CPU lại đổi → tạo phiên bản mới (bản cũ trả sau submit) — nhờ vậy mọi tải lên gom vào một copy pass
  mà lệnh cũ vẫn thấy nội dung cũ. Render target = phiên bản có COLOR_TARGET (BGRA8); `Lock` một RT đã vẽ → đọc lại đồng bộ.
- Định dạng: A8R8G8B8/X8R8G8B8 → BGRA8 trực tiếp; **16 bit (4444/1555/565) và 24 bit → đổi sang BGRA8 khi tải** (thứ tự kênh 4444/1555 khác nhau giữa
  Vulkan PACK16 và D3D nên chưa dùng native; VRAM gấp đôi so với D3D9 4444 — bước sau: đường bảng màu A8L8 2 B/px như D3D11); A8L8 → R8G8; L8 → R8; A8 → A8.
- Shader: uniform VS set 1 (viewport, W·V·P hàng-chính của D3D → GLSL `M * v`), sampler FS set 2 (t0, t1, bảng màu), uniform FS set 3 (2 stage
  D3DTOP/D3DTA + alpha test như HLSL cũ); màu đỉnh D3DCOLOR → `UBYTE4_NORM` rồi `.bgra`; XYZRHW cộng 0,5 texel như D3D11; SDL_GPU chuẩn hoá NDC y-lên,
  gốc viewport trên-trái trên mọi backend nên KHÔNG lật y. `gl_PointSize` cho POINTLIST. Đỉnh thiếu màu/uv lấy từ buffer "đỉnh giả" theo instance.
- Pipeline cache theo (FVF, topology, blend src/dst/op/enable, mặt nạ ghi màu, cull, fill, định dạng target); sampler cache theo lọc/địa chỉ.

**Chạy thử 17:27 (`GameSDL.exe`, `Rep3Api=100`):** `[GPU] thiết bị: driver vulkan, backbuffer 1024x768, swapchain BGRA8`; 22 s và 45 s không sập;
**ảnh chụp cửa sổ: màn hình chính vẽ đúng** (nền, 4 nút, chữ Việt, alpha) — lần đầu client JX1 chạy trên Vulkan. `gpu_test` trước đó: máy này
Vulkan OK, mọi định dạng 16 bit lấy mẫu được (4444 không làm target), vsync/immediate/mailbox đều có.

**Chưa làm / cần chủ test:** vào game thật (sprite nhân vật, hiệu ứng blend INVDESTCOLOR/ONE và ONE/ONE, chữ, bản đồ, `DrawPrimitivesOnImage` → render
target, đổi map/Reset); toàn màn hình (hiện chạy cửa sổ); `StretchRect`/`DrawIndexedPrimitive` chưa cài (log `CHUA CAI` nếu game gọi); bảng màu chỉ bật khi
`g_nRep3ApiOn == 11` trong `TextureRes.cpp:699` → cần thêm `|| == 100` (một token, bàn với phiên D3D11); native 16 bit; Android: `d3d9mini.h` thay
`<d3d9.h>` (chỉ enum/struct/vtable, mọi thứ đã có trong `D3D9onGPUi.h`).

### 11.1 Khung ảo (17:3x): toàn màn hình / màn hình khác backbuffer

- `CDevGpu::ApplyWindowMode()` (gọi ở Init và Reset): `SDL_SetWindowFullscreen(pp.Windowed == FALSE)` (toàn màn hình kiểu desktop, không đổi chế độ);
  cửa sổ thì `SDL_SetWindowSize` theo backbuffer. `Letterbox(swW, swH)`: khi swapchain khác backbuffer → scale = min(W/w, H/h), căn giữa; viewport và
  scissor của lệnh vẽ lên swapchain được nhân scale + cộng offset (shader vẫn nhận viewport logic → NDC; phần cứng đặt viewport đã phóng).
- `KSdlApp.cpp` `SdlToLogical()`: toạ độ chuột (motion/button/wheel) từ cửa sổ → khung logic `SCREEN_WIDTH × SCREEN_HEIGHT` cùng công thức letterbox, kẹp biên.
- Test 17:39 (`FullScreen=1`, desktop 2560×1440, khung 1024×768 → scale 1,875, dải đen 320 px hai bên): ảnh chụp cả màn hình đúng; bấm chuột tại
  (320 + 512·1,875 ; 275·1,875) mở đúng hộp "Tùy chọn" → ánh xạ chuột đúng. Kịch bản: `ReverseTools/mobile_x64/test_gpu/test_fullscreen.ps1`
  (đặt `FullScreen=1` tạm, chụp `CopyFromScreen`, `mouse_event`, trả lại config). Cửa sổ SDL không có `SDL_WINDOW_RESIZABLE` nên không phóng
  được từ ngoài (SetWindowPos bị SDL chặn) — muốn thử cửa sổ khác cỡ thì đổi `Width/Height` trong config.
- Đây chính là cơ chế "khung ảo + scale" trong phương án mobile (điện thoại render 1024×768 hoặc 1138×640 rồi phóng lên màn), đã chạy trên PC.
