# PHỐI HỢP HAI PHIÊN 08/09: "Game.exe 64-bit (pha 0 mobile)" ↔ "Represent3 lên D3D11 (D3D9on11)"

Phiên **wauto-45** (mobile, nhánh `mobile-0809`, worktree `D:\GAMEDEVNEW_wt_mobile`) và phiên đang làm **D3D9on11** trong
`D:\GAMEDEVNEW_wt_delta` (nhánh `delta-0709`) cùng chạm `Sources/Represent/Represent3`. `SendMessage` giữa phiên bị tắt nên ghi ở đây;
chủ đọc và chuyển lời. Cả hai phiên: **commit theo pathspec, `git fetch` + rebase `origin/main` trước khi build/push, không đặt `.moi`
của nhau**.

## Vùng của wauto-45 (phiên D3D11 xin đừng chạm)

| Tệp | Đổi gì |
|---|---|
| `Sources/S3Client/**` (~280 tệp UI) | `unsigned int uParam, int nParam` → `KUPARAM uParam, KNPARAM nParam` ở `WndProc`/`ProcessInput`/`CoreDataChanged`; ép con trỏ `(unsigned int)&x` → `(KUPARAM)&x`. Win32: `KUPARAM == unsigned int`, `KNPARAM == int` ⇒ nhị phân Win32 **không đổi**. |
| `Sources/Core/Src` (CoreShell.h/.cpp, `*Client.cpp`, KPlayer.cpp, KProtocolProcess.cpp… chỉ vùng `#ifndef _SERVER`) | cùng loại đổi trên; **không chạm `KPlayerBot.cpp`** (chỉ máy chủ) |
| `Sources/Engine/Src/KWin32.h` | thêm `typedef uintptr_t KUPARAM; typedef intptr_t KNPARAM;` |
| `Core.vcxproj`, `Engine.vcxproj`, `S3Client.vcxproj`, `FilterText_StaticLib.vcxproj`, `JpgLib/JpgLib.vcxproj` (mới) | **thêm** cấu hình `…|x64` (đầu ra `x64\…`, `Lib\release64`, `bin\client64`); cấu hình Win32 giữ nguyên |

## Vùng của phiên D3D11 (wauto-45 không chạm), trừ đúng hai chỗ nhỏ

- `Sources/Represent/Represent3/*.cpp/.h` (D3D9on11*.cpp/.h, D3D_Device/D3D_Shell/BaseInclude, KRepresentShell3, TextureRes*…).
- Hai chỗ wauto-45 **đã** sửa (2–30 dòng, để x64 biên dịch được), phiên D3D11 nếu đụng cùng chỗ thì báo:
  1. `Represent3/TextureRes.cpp`: bọc hàm hợp ngữ `inline void RenderToA4R4G4B4(...)` (KHÔNG còn được gọi, mã dùng
     `RenderToA4R4G4B4Safe`) trong `#ifndef _WIN64 … #endif`.
  2. `iRepresent/Font/KFontRes.cpp`: `KFontRes::Update()` thêm nhánh C dưới `#ifdef _WIN64`, giữ nguyên `__asm` cho Win32.
- `Represent3.vcxproj`: wauto-45 thêm `Release|x64` **bằng script** (`ReverseTools/mobile_x64/them_x64.py`, nhân bản từ `Release|Win32`).
  Phiên D3D11 cứ thêm tệp `.cpp` mới vào vcxproj như thường; wauto-45 sẽ rebase rồi chạy lại script trên bản đã gộp, không sửa tay.

## Yêu cầu cho mã D3D9on11 để sau này chạy x64 (wauto-45 không làm hộ)

- Không dùng `__asm`; không ép con trỏ vào `int`/`DWORD`/`unsigned int` (dùng `uintptr_t`/`KUPARAM`); không giả định `sizeof(void*) == 4`.
- Tệp mới phải nằm trong `Represent3.vcxproj` (để nhân bản cấu hình x64).
- Cờ x64 wauto-45 bật để bắt lỗi cắt cụt con trỏ: `/we4311 /we4312 /we4302`.

## Lịch build của wauto-45

Chỉ build ở worktree riêng, ưu tiên thấp (BelowNormal), `-m:1`, `CL_MPCount` ≤ 3, tránh giờ trận Tống Kim (bot vào ~x:10, hết ~x:40).
Không đụng `E:\SourceTuanLe\...\TESTLOFFF_ONLINE\bin`.

## Ghi nhận 08/09 12:3x–12:5x (wauto-45, từ bản test Game.exe x64 trong `D:\GAMEDEVNEW_wt_mobilein\client64`)

- **Gợn sóng khi cuộn màn hình** xuất hiện khi chạy lớp D3D9on11 (`Rep3Api=11`, vsync=0, atlas bật — cùng cấu hình `config.ini` với cây live,
  và cây live Win32 cũng đang chạy D3D9on11 từ 12:08). Đổi riêng bản x64 sang `Rep3Api=9` (D3D9 thuần) thì **hết gợn** ⇒ hiện tượng thuộc lớp
  D3D9on11/atlas, không phải x64. Bản x64 lúc thử là mã main tới 8be73705 (chưa có g+h+i 56e7ecf0); bản g+h+i đã được gộp vào nhánh mobile
  12:37 và dựng lại, chưa thử lại với `Rep3Api=11`.
- Mã D3D9on11 (kể cả g+h+i) biên dịch sạch trên x64 với `/we4311 /we4312 /we4302` — không có chỗ ép con trỏ vào int.
- Việc x64 đã sửa cùng lúc (không đụng Represent3): sập khi đăng nhập lại do `sNotify(int nCmd, int nParam)` trong KMailClient/KAuctionClient/
  KChienLenhClient và `sSendOp` ở UiAuction/UiChienLenh/UiMail nhận con trỏ qua `int` (dump 12:42 `KUiMailList::AddHeader`, địa chỉ
  `FFFFFFFFFCB9D3F0` = con trỏ 64-bit bị cắt) → đổi sang `KNPARAM`.
