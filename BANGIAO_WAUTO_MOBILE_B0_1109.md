# BÀN GIAO — WAuto MOBILE, BƯỚC B0: ĐƯỜNG ỐNG TRONG TIẾN TRÌNH (11/09)

> Lộ trình: `LOTRINH_WAUTO_MOBILE_1109.md` (đọc §0 luật trước). Bước này **không có giao diện**; nó làm cho bộ não WAuto
> đã nằm sẵn trong bản Android **chạy được khi có ai bảo nó chạy**, để nút BẬT ở B1 có cái để bật.
>
> Nhánh: `claude/full-wauto-mobile-analysis-f781df` (worktree `D:\GAMEDEVNEW\.claude\worktrees\full-wauto-mobile-analysis-f781df`),
> đã gộp `mobile-0809` tới `cc1a41f8` (11/09, "Mục 14.8"). Chủ quyết cách đưa vào `mobile-0809` (merge hoặc cherry-pick).

---

## 1. Kết quả đo — LDPlayer, tài khoản `hinod1`, nhân vật id `3758889385`

Hai lần thử (APK `android/apk/jx1mobile-1109-wauto-b0.apk` rồi `...-b0b.apk` sau khi gộp và vá 84). Lần hai:

| Việc | Bằng chứng (logcat `SDL/APP` và `jx_auto.log` trong thư mục dữ liệu) |
|---|---|
| Đọc công tắc | `[WAUTO] config.ini [WAuto] Bat=1; sizeof(autoData)=7644` |
| Lấy mã nhân vật + máu từ CoreShell | `[WAUTO] chua co \APdata\3758889385.dat -> mac dinh lan dau (danh + nhat + uong thuoc), mau 7742 / noi luc 834` |
| Ghi cấu hình mặc định lần đầu | `[WAUTO] ghi \APdata\3758889385.dat: 7644 byte`; tệp thật `D:\jx1_android_data\APdata\3758889385.dat` 7.644 byte |
| Báo bộ não bật | `[WAUTO] BAT (PRT_TICKSTART -> ATYPE_CLEAR)` |
| Bên gửi chạy đều | `[WAUTO] nhip: id=3758889385 fight=1 vis=1000 pick=1 life=1/5160 TK=0 DT=0` mỗi 5 giây (gói gửi mỗi 54 ms) |
| **Bên nhận đã tiêu thụ gói** | `jx_auto.log`: **48 dòng `[AUTO-PASS]` trong 50 giây** (`fight=1 pick=1 fpick=1 vis=1000 near=75 pvis=400` đúng như đã gửi), `[HD-GATE] nCT=0 nTK=0 nLD=0 nHD=0 nST=0 ...` mỗi 5 giây |
| Máy Chiến đấu thật sự chạy | `[FIGHT-IN] ... hp=10262/14157`, `[FIGHT-RET] fightret=0`, `[FIGHT-SKIP] fight tra 0 -> RESETMOVE`, `[AUTO-END] ret=0` — trả 0 vì **đứng trong thành, không có quái**; đúng hành vi WAuto trên PC |
| Đọc lại tệp `.dat` bằng công cụ | `python android/wauto_dat.py xem D:\jx1_android_data\APdata\3758889385.dat` → `bFight=1 nVision=1000 nIlifeCell1=5160 nIlifeCell2=2580 bPickUp=1 nPickVision=400 uFKey=32 ...` |
| Sập | `jx_crash.log` rỗng |

Những dòng `[AUTO-PASS]`, `[HD-GATE]`, `[FIGHT-*]` do **chính `ExtAutoLoop` và CoreShell** ghi, không phải mã mới — tức là gói
`IPCGameLoop` đã đi qua `ProcIpcCommand` (bên nhận **không sửa một dòng**) y như khi WAuto.exe gửi trên PC.

**Lần thử ba** (APK `...-b0c.apk`, sau vá 85): tệp `.dat` sinh mới **sạch** — mọi mảng chuỗi (`szIJPtName`, `szNOPName`, `szLDPtName`...) rỗng;
62 dòng `[AUTO-PASS]`; `jx_crash.log` rỗng. **APK cuối của B0: `android/apk/jx1mobile-1109-wauto-b0c.apk`.**

**Chưa đo:** nhân vật tự đánh quái ngoài thành (tài khoản thử đứng trong Thành Long, cấu hình mặc định không có "về map luyện").
Muốn thấy, chủ đưa nhân vật ra bãi quái rồi mở app với `Bat=1`, hoặc chờ B1 có nút BẬT để tự thử.

**Hai chuỗi Windows** dựng lại từ worktree riêng trên nguồn đã gộp: `build_chuoi_x64` 4/4 bước **0 lỗi**, `build_chuoi_sdl` 6/6 bước
**0 lỗi**, đủ 6 tệp `bin\client64sdl`. Mọi dòng thêm vào tệp dùng chung đều nằm trong `#ifdef JX_ANDROID`.

---

## 2. Đã làm gì, ở đâu

| Tệp | Gì | Rào |
|---|---|---|
| `Sources/S3Client/Platform/JxWAutoNoiBo.h` / `.cpp` (**mới**, 331 dòng) | Bên gửi trong tiến trình: cứ 54 ms nạp `IPCGameLoop` + `IPCHienThi` vào `g_pState` rồi `SetEvent(g_hEventRecv)` — y hệt `AppLoop` của WAuto.exe. Đổi trạng thái bật/tắt → `PRT_TICKSTART` (`ATYPE_CLEAR`) + một dòng ở khung chat (TCVN3). Nạp/ghi `APdata\<id>.dat` qua `KFile`; chưa có tệp → **mặc định lần đầu chép nguyên** từ `LoadRoleData` của WAuto.exe rồi ghi ra tệp. Tệp cũ ngắn hơn struct → phần đuôi giữ mặc định lần đầu (= các nhánh nâng cấp của WAuto) | Chỉ Android: **không nằm trong vcxproj nào**, thêm thẳng vào `add_library(main ...)` của `android/CMakeLists.txt` |
| `Sources/S3Client/S3Client.cpp` | `#include "Platform/JxWAutoNoiBo.h"` + gọi `JxWAuto_NhipVongLap()` **ngay trước** `ProcIpcCommand()` để gói vừa nạp được tiêu thụ cùng khung | `#ifdef JX_ANDROID` |
| `Sources/Core/Src/CoreShell.cpp` | `JxCore_WAutoNhanVat(&id, &lifemax, &manamax)` — id = `Player.m_dwID` (đúng trường `IPCMainSync.dwPID` gửi cho WAuto.exe, nên **tên tệp `.dat` trùng bản PC**) | trong khối `#ifdef JX_ANDROID` sẵn có cạnh `JxCore_GotoHuong` |
| `Sources/Core/Src/KCore.cpp` | `g_AutoLogOn()`: nhánh không-`WIN32` trước đây ép 0 → Android **không bao giờ** có `[AUTO-PASS]`/`[HD-GATE]`. Nay Android đọc `[Client] AutoLog` như PC, ghi `jx_auto.log` ở thư mục dữ liệu | `#ifdef JX_ANDROID` bên trong nhánh `#else`; nhánh `WIN32` và máy chủ Linux không đổi |
| `android/CMakeLists.txt` | thêm `JxWAutoNoiBo.cpp` vào target `main` | |
| `android/du_lieu_ghi_de/config.ini` | mục `[WAuto] Bat=0` (công tắc tạm cho B0) | lớp ghi đè |
| `android/wauto_dat.py` (**mới**) | đọc / xem / sửa / so tệp `.dat` theo bố cục struct **đọc thẳng từ `ipc_shared.h`** (không chép tay). `sizeof`, `truong`, `xem`, `dat ten=giá_trị`, `so` | |
| `android/va_nguon_android_83.py`, `84.py`, `85.py` | ba bản vá (tạo tệp + vá; sửa sau lần thử 1; sửa sau lần thử 2). Chạy lại vô hại | |
| `LOTRINH_WAUTO_MOBILE_1109.md` | §0.1 thêm cách "chỉ Android, không vcxproj" (ưu tiên); dòng tiến độ B0 | |

Số kịch bản vá: ban đầu là 81/82, **đổi thành 83/84** vì phiên kia cũng vừa tạo 81/82 (vá ICON d/e) — commit gộp `017a522a`.

---

## 3. Cách dùng (cho người thử)

```ini
; D:\jx1_android_data\config.ini  (hoặc lớp ghi đè android\du_lieu_ghi_de\config.ini)
[WAuto]
Bat=1            ; 1 = bật máy auto ngay khi vào game; 0 = tắt (mặc định)
[Client]
AutoLog=1        ; muốn xem bên nhận làm gì: jx_auto.log ở thư mục dữ liệu (rất nhiều dòng, ~360 KB / 50 giây - xong thì tắt)
```

- Cấu hình: `APdata\<mã nhân vật>.dat` ở thư mục dữ liệu, **cùng định dạng và tên tệp với WAuto.exe** — chép tệp `APdata` của
  bản PC sang là dùng được. Chưa có → mặc định lần đầu (đánh, tầm 1000, nhặt đồ, uống thuốc ở 2/3 và 1/3 máu, sửa đồ...).
- Sửa cấu hình không cần giao diện:

```bash
python android/wauto_dat.py xem  "D:/jx1_android_data/APdata/3758889385.dat"
python android/wauto_dat.py dat  "D:/jx1_android_data/APdata/3758889385.dat" bTongKim=1 nTKPhe=2 bDaTau=1
python android/wauto_dat.py truong TK          # liệt kê trường có chữ "TK" kèm offset
```

- Kịch bản thử tự động (scratchpad, không trong kho): cài APK, `Bat=1`, khởi động lại thật (`force-stop` + `sleep 4`), chờ 50 giây,
  gom logcat + `jx_auto.log` + `APdata` + ảnh.

---

## 4. Ba lỗi đã gặp và cách sửa (đều là bẫy sẽ gặp lại ở B1+)

| # | Bẫy | Sửa |
|---|---|---|
| 1 | **`g_CreatePath("\APdata")` trên Android không tạo thư mục**: nó ghép gốc + `\APdata` rồi đưa thẳng cho `SDL_CreateDirectory`, dấu `\` không được đổi. Tệp `.dat` ghi thất bại lần thử 1 | Dùng `CreateDirectory(...)` của lớp giả lập (`KPosixWin32.cpp`): `JxPathPosix` đổi `\`→`/`, ghép thư mục dữ liệu, hạ chữ thường — đúng đường `KFile::Create` dùng (vá 84) |
| 2 | **`g_AutoLogOn()` = 0 trên mọi nền không `WIN32`** → không có nhật ký nào của bên nhận để chứng minh | Mở cho `JX_ANDROID` (vá 84). Từ nay `jx_auto.log` là chỗ soi mọi máy auto trên điện thoại |
| 3 | **Bản mặc định lấy từ biến tạm trên ngăn xếp**: constructor `autoData` chỉ gán số, không xoá trắng mảng chuỗi → rác (`'r8v'`) trong `szIJPtName` ghi ra tệp | Lấy từ đối tượng **tĩnh** (bộ nhớ tĩnh được xoá trắng trước khi constructor chạy) (vá 85) |

---

## 5. Việc bên lề đã phải làm để dựng được từ worktree mới (ghi lại cho người sau)

Worktree mới thiếu các **đầu vào không theo dõi git** mà cây `D:\GAMEDEVNEW_wt_mobile` đã có sẵn. Đã chép sang (chỉ đọc bên nguồn):

| Thiếu | Ai cần | Lấy ở |
|---|---|---|
| `ThirdParty/SDL3-src/` (77 MB) | Android CMake | cây mobile (hoặc `android/tai_sdl3_src.ps1`) |
| `android/gradle-project/local.properties` (`sdk.dir=...`) | gradle | tự tạo |
| `Sources/Core/vcpkg_installed/.../include/nlohmann/` | `BauCua.cpp` (Android) | cây mobile |
| `Lib/release64/*.lib` (JpgLib, common, lualibdll, FilterText...) và `Lib/release64sdl/*` | hai chuỗi Windows | cây mobile |
| `Sources/packages/Microsoft.DXSDK.D3DX.9.29.952.8/build/native/{release,debug}/{lib,bin}` | Represent3 | cây mobile (bị `.gitignore` `Release/`) |

Chuỗi x64 có bước hậu dựng chép `engine.dll` ra `..\..\..\bin\Multiserver` (ngoài kho) → ở worktree lạ phải chạy với
`-Props PostBuildEventUseInBuild=false` (`build_thap.ps1` có sẵn tham số này). Chuỗi SDL không cần.

Hai kịch bản chuỗi dựng trỏ vào worktree này nằm ở scratchpad phiên (`build_chuoi_x64_wt.ps1`, `build_chuoi_sdl_wt.ps1`), chỉ khác
bản gốc ở biến `$W` và tham số `-SolutionDir`.

---

## 6. Máy ảo dùng chung với phiên khác

Trong lúc thử, một phiên khác đang chạm vào cùng LDPlayer (thấy `[UITOADO] BAT che do sua giao dien` giữa bài thử của tôi và
pid app đổi liên tục). Đã làm để không dẫm lên nhau:

- Không sửa mã trong `D:\GAMEDEVNEW_wt_mobile`; làm ở worktree riêng, gộp `mobile-0809` **trước mỗi lần dựng** (chủ yêu cầu).
- Trước khi cài APK của tôi đã kéo APK đang cài về (`scratchpad\ld\apk_phien_khac_backup.apk`, 23,8 MB); thử xong **cài lại**
  bản đó, đặt lại `Bat=0`, mở lại app. Chỉ còn khác một mục `[WAuto] Bat=0` trong `D:\jx1_android_data\config.ini` (vô hại) và
  tệp `APdata\3758889385.dat` mới sinh.
- Phiên kia còn ba tệp sửa dở chưa commit (`CoreShell.cpp`, `KNpc.cpp`, `KPlayer.cpp`) — chưa gộp được, sẽ gộp khi họ commit.

---

## 7. Còn treo

- Nhân vật tự đánh quái ngoài thành: chưa đo (xem §1).
- `AutoLog=1` trong `D:\jx1_android_data\config.ini` do phiên khác đặt — `jx_auto.log` lớn rất nhanh, xong việc nên đặt 0.
- Tiếp theo: **B1** — khung Auto từ kho VNKU + icon Auto trên thanh công cụ + nút BẬT/TẮT nối vào `JxWAuto_Bat()` + dòng trạng thái
  `WA_HoatDong()`. Mọi API cần cho B1 đã có trong `JxWAutoNoiBo.h`: `JxWAuto_Bat`, `JxWAuto_DangBat`, `JxWAuto_CauHinh`,
  `JxWAuto_LuuCauHinh`, `JxWAuto_IdNhanVat`.
