# BÀN GIAO PHA 0 MOBILE — GAME.EXE 64-BIT (08/09/2026, 10:30–12:10)

Chủ chốt 08/09: *"ok hãy thực hiện — lưu ý hạn chế ảnh hưởng bản pc hiện tại"* và *"có phiên khác đang nâng lên d3d11, tránh xung đột"*.
Phương án gốc: `PHUONGAN_NANG_MOBILE_0809.md` (pha 0 = Game.exe 64-bit). Chia vùng với phiên D3D11: `PHOICHOP_X64_MOBILE_VS_D3D11_0809.md`.

## 0. Kết quả ngắn

- **Game.exe 64-bit đã link xong lúc 12:05** cùng CoreClient.dll, Engine.dll, Represent3.dll (D3D9 x64), Rainbow.dll, Lua54Dll.dll, FilterText.dll,
  JpgLib (giải mã bằng WIC) — tất cả x64, kiểm bằng `dumpbin`, không thiếu DLL nào (`ReverseTools/mobile_x64/kiem_client64.ps1`).
- **Chưa chạy thử trong game** (tôi không tự mở Game.exe để không chen cửa sổ/trận của chủ). Bộ test nằm riêng ở
  `D:\GAMEDEVNEW_wt_mobile\bin\client64\` — chạy `ChoiGame64.bat`. `data\ Spr\ maps\ maps2\` là junction chỉ-đọc tới cây live;
  `settings\ script\ Ui\ UserData\ Filter\ Move\ tools\ APdata\ Chat\ NOEND config.ini package.ini` là bản sao riêng ⇒ **không đụng cây live,
  không dùng khe `.moi`**.
- **Bản PC Win32 không đổi**: mọi kiểu mới (`KUPARAM/KNPARAM/KSCENEID`) trên Win32 đúng bằng `unsigned int/int` (xáo tên y hệt); mọi mã
  mới nằm sau `#ifdef _WIN64`/`_M_X64`; cấu hình Win32 của các vcxproj không sửa (chỉ **thêm** cấu hình x64); máy chủ (`_SERVER`) giữ
  `KSCENEID = unsigned int`, `KPlayerBot.cpp` không chạm. **Chưa build lại Win32 để chứng minh** — việc đầu của phiên sau (mục 5).
- Nhánh `mobile-0809` đã push, **đã rebase lên `origin/main` có D3D9on11** (5 commit b8df8b89 → 8cc5b0e3 → 4248a7bf → 973f5ec7 → bd782326
  + commit bàn giao); công cụ ở `ReverseTools/mobile_x64/`.

## 1. Cách thử (chủ)

1. Mở `D:\GAMEDEVNEW_wt_mobile\bin\client64\ChoiGame64.bat` (hoặc `Game.exe` trong đó). Máy chủ live vẫn nhận client này như client thường
   (cùng giao thức; `CheckProtocolSize` có dung sai +4 cho 32/64-bit, bộ đo `do_sizeof_goi.py`: 263 struct gói, chỉ `SHOW_MSG_SYNC` 9→13 do
   `LPVOID m_lpBuf`, đúng bằng máy chủ x64).
2. Kỳ vọng: đăng nhập, vào thế giới, giao diện 134 cửa sổ hoạt động như bản 32-bit, RAM có thể vượt 2 GB không sập, `Rep3CacheMB=1500` an toàn.
3. Biết trước KHÔNG có trên x64 (không phải lỗi): nhạc MP3 (mp3lib.lib chỉ x86, stub im lặng), video mở đầu (KLVideo.dll x86, rad.h chỉ để biên dịch),
   JXReplay/UpdateDLL (không có bản x64, nạp động thất bại êm), Represent2 (không build x64; chỉ Represent3).
4. Nếu sập: `bin\client64\DumpInfo\` + `CrashLog` đã ghi thanh ghi x64 (RIP/RSP/RBP, StackWalk64 máy 0x8664).
5. **Chạy thử 12:2x (chủ): vào game OK.** Lần đầu vướng hộp thoại "KMp3Music: mp3 decode head fail" → vá `Mp3Init` thất bại êm trên x64
   (commit 33f631eb). Sau đó chủ báo **"di chuyển màn hình bị gợn sóng"**. Log `client64\jx_rep3.log` cho thấy bản x64 đang chạy
   **`Rep3Api=11` (lớp D3D9on11 của phiên kia, vsync=0, atlas BẬT)** vì `config.ini` chép từ cây live có `Rep3Api=11` — và **cây live Win32 cũng
   đang chạy D3D9on11** (`Represent3.dll` live swap 12:08, log live cùng dòng API D3D11). Để tách x64 khỏi D3D11: đã đặt `Rep3Api=9` trong
   `client64\config.ini`, chờ chủ chạy lại; đồng thời hỏi chủ bản PC 32-bit (cũng D3D11) có gợn sóng không. Nếu D3D9 x64 hết gợn ⇒ lỗi thuộc
   lớp D3D9on11/atlas (phiên D3D11); nếu vẫn gợn ⇒ lỗi riêng x64, điều tra `KRepresentShell3`/`TextureRes` đường D3D9.

## 2. Cơ chế đã đổi (đọc trước khi sửa vùng này)

| Cơ chế | Ở đâu | Ghi chú |
|---|---|---|
| `KUPARAM`/`KNPARAM` (`uintptr_t`/`intptr_t`) thay `unsigned int uParam, int nParam` | `Engine/Src/KWin32.h`; 318 tệp S3Client + Core (`sua_x64_nguon.py`) | hệ KWnd `WndProc`, `CoreDataChanged`, `OperationRequest`, `GetGameData`, `TongOperation`, `TeamOperation`, `SceneMapOperation`, `AutoPlayOperation`, `Debug`, `ProcessInput`, `DrawGameObj(... KNPARAM nParam)`, `SetMoveMap(..., KNPARAM nValue)`, `SetSortItem/FkAutoSetFillterMagic(KNPARAM nType, ...)`, `KUi{Auction,Mail,ChienLenh}_OnCoreCmd(..., KNPARAM)`, `KUiChatCentre::UpdateData(KUPARAM)`, `KUiShop::FkAutoOnBuyItem(KUPARAM)`, `WND_GAMESPACE = ((KUPARAM)-1)`, tay cầm `KWndMessageListBox::{SplitData,BindData,FreeData}` |
| `KSCENEID` = ID vật thể cảnh (thực chất `KIpotRuntimeObj*`) | `KWin32.h` (client `KUPARAM`, máy chủ `unsigned int`); `Scene/KScenePlaceC.h/.cpp`, `KMissle.h`, `KNpcRes.h`, `KObj.h` `m_SceneID` | |
| JPEG trên x64 | `JpgLib/JpgLib.vcxproj` (mới) + `JpgLib/Src/KJpegLib_wic.cpp` (WIC, cùng 4 hàm API); `Engine/Src/KJpgFile.cpp:14`, `KPakFile.cpp:121` `#ifndef _WIN64` → `#if 1`; macro `KSG_IMAGE_CONTENT_SIZE` qua `KUPARAM` | Win32 vẫn dùng `Lib\release\JpgLib.lib` cũ |
| Hợp ngữ | `KFontRes::Update` nhánh C `#ifdef _WIN64`; `TextureRes.cpp` `RenderToA4R4G4B4` bọc `#ifndef _WIN64` (không còn được gọi); `KLVideo/rad.h` bản C cho `_M_X64` (12 helper + `Locked*` → `Interlocked*`) | Engine KDraw* đã có nhánh C sẵn |
| Con trỏ cất trong số | `KCodecLzo.cpp` (so sánh/trừ con trỏ), `KMemManager.cpp` (`bh->next` chứa chỉ số), `ucl/ucl_ptr.h` (`ucl_ptr_t` 8 byte), `LuaFuns.cpp`/`KStepLuaScript.cpp` (địa chỉ `lua_State` qua số Lua: 'n' thay 'd' trên x64), `KNetServerNode.cpp`, `OpaqueUserData.h` (`ULONG_PTR`), `CrashLog.cpp` (thanh ghi theo kiến trúc), `NetConnectAgent.cpp` (`size_t nSize`), `UiCursor.cpp` (`IDC_ARROW` không bọc lại `MAKEINTRESOURCE`), `ShortcutKey.cpp` (tay cầm từ Lua `KNPARAM`), `DumpMemory.cpp` | |
| MP3 trên x64 | `KMp3Music.cpp` mở lại lớp (S3Client `KMyApp` cần) + stub 4 hàm `mp3_decode_*` trả thất bại | mp3lib không có nguồn |
| Cấu hình x64 | `them_x64.py` nhân bản `…|Win32` → `…|x64` (S3Client, Represent3, Core `Client Release`, Engine `Release`, FilterText_StaticLib, FilterText.dll); đầu ra `x64\…`, lib `Lib\release64`, chép `bin\client64` (worktree); `/we4311 /we4312 /we4302` + `/MP`; vcpkg tắt cho S3Client x64; mọi `<Library>` x86 loại khỏi x64 | `Lib\lua54\x64\Lua54Dll.*` **phải build lại từ `Library/Lua54`** (bản trong git thiếu `lua4_pushboolean/pushinteger/toboolean`) |

## 3. Số đo

| Việc | Số |
|---|---|
| Tệp nguồn đổi kiểu tham số | 318 (S3Client 280, Core 38) — không đổi byte nào > 0x7F (kiểm bằng đếm byte cao từng tệp) |
| Ép con trỏ sửa theo compiler | Core 10 + 5 tệp ngữ cảnh; S3Client 48 tự động + 11 nhóm tay |
| Vòng build | 14 (Engine 7 vòng, Core 4, S3Client 4); mỗi vòng ≤ 50 s nhờ `/MP3`, ưu tiên BelowNormal, tránh giờ trận (build 11:41–12:06 sau trận 11:10–11:40) |
| Nhị phân x64 | Game.exe 1.790.464 B, CoreClient.dll 3.397.120, Engine.dll 363.008, Represent3.dll 150.528, Lua54Dll.dll 587.776, Rainbow.dll 1.084.928, FilterText.dll 41.472, D3DX9_43.dll x64 2.404.248 |

## 4. Phối hợp với phiên D3D11 (wauto-fe, `D:\GAMEDEVNEW_wt_delta`, nhánh `delta-0709`) — ĐÃ GỘP

Họ đẩy 3 commit lên `origin/main` trong lúc tôi build (e44becd4 D3D9on11 `Rep3Api=11`, 7cbe4bac, 8be73705 atlas). **12:10 đã rebase
`mobile-0809` lên `origin/main` (8be73705): git tự gộp sạch**, chỉ hai tệp giao nhau (`Represent3.vcxproj`, `TextureRes.cpp`), không xung đột;
kiểm sau gộp: vcxproj còn cấu hình x64 + 4 tệp `D3D9on11*.cpp` mới, `TextureRes.cpp` còn chắn `_WIN64` và số byte cao bằng bản main (874).
**Build lại 12:12: Represent3 x64 kèm mã D3D9on11 biên dịch sạch qua cả `/we4311 /we4312 /we4302`** (0 lỗi, 12 cảnh báo), Game.exe link lại
12:13. Nhánh đã push lại bằng `--force-with-lease` (đã đổi lịch sử); bản trước rebase giữ ở nhánh cục bộ `mobile-0809-truoc-rebase`.
Lần sau họ đẩy tiếp: cứ `git rebase origin/main`; nếu xung đột thì lấy bản họ rồi chạy lại `them_x64.py` / `sua_postbuild_x64.py` /
`va_x64_nguon2.py` (đều tái áp được).

## 5. Việc tiếp theo (theo thứ tự)

1. **Chứng minh Win32 không đổi**: build `Client Release|Win32` (Core) + `Release|Win32` (S3Client) từ `mobile-0809` ở worktree này, so `sizeof`
   struct và chạy thử; nếu muốn chắc tuyệt đối, so mã máy `.obj` hàm `WndProc` một cửa sổ với bản main.
2. Chủ chạy `bin\client64\ChoiGame64.bat`; đọc `jx_auto.log` trong `client64`; sập thì đọc `DumpInfo`.
3. Gộp `origin/main` (có D3D11) → rebase, chạy lại 2 script như mục 4, build lại chuỗi (`build_chuoi_x64.ps1`).
4. Việc x64 còn lại nhỏ: `RADsqrt` C làm tròn xuống (hợp ngữ làm tròn gần nhất) — chỉ ảnh hưởng video; `CL_DumpStack` in địa chỉ 32-bit bằng
   `(unsigned)` ở nhánh không có StackWalk64; cảnh báo C4244 nhiều (uintptr → int) chưa rà.
5. Pha 1 của phương án: biên dịch bằng clang/NDK để ra danh sách port thật; bắt đầu từ chính bộ cờ `/we4311…` đã bật.

## 6. Bẫy đã dính trong phiên (đừng lặp)

- Git Bash đổi tham số `/we4311` thành `C:/Program Files/Git/we4311` khi truyền cho python → dùng `MSYS_NO_PATHCONV=1` hoặc viết script ra tệp.
- Heredoc Bash cắt `\\` thành `\` → script python inline có backslash hỏng âm thầm; **mọi script vá đều viết ra tệp** (`ReverseTools/mobile_x64/*.py`).
- `.gitignore` nuốt thư mục tên `x64` và `bin` → công cụ để ở `ReverseTools/mobile_x64`, bat mẫu `ChoiGame64.bat.mau`.
- PowerShell 5.1 không có `Start-Process -PriorityClass` → hạ ưu tiên chính tiến trình PowerShell rồi gọi MSBuild (con kế thừa).
- Post-build của dự án dùng `..\..\..\bin` = **`D:\bin`** (ngoài cây), không phải cây live; cấu hình x64 đã đổi về `..\..\bin\client64`.
- Tệp có cả LF lẫn CRLF → tách theo `\n`, giữ `\r` từng dòng (`va_x64_nguon2.py`).
- Regex `(unsigned)(ip[3])` suýt bị đổi (giá trị byte, không phải con trỏ) — script tự dừng nhờ đếm số khớp.
