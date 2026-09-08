# BÀN GIAO — PHA 1: BIÊN DỊCH BẰNG CLANG, DANH SÁCH PORT THẬT (08/09/2026, 14:5x)

Nhánh `mobile-0809`. Công cụ: `ReverseTools/mobile_x64/clang_survey.py` (clang-cl của NDK r25 = LLVM 14, `-fsyntax-only`, header MSVC 14.44,
vcpkg/DXSDK dò tự động) và `survey_winapi.py` (khảo sát tĩnh Windows API). Kết quả nguyên văn: `ReverseTools/mobile_x64/ketqua/`.

## 0. Kết luận ngắn

Mã client **gần như đã sạch MSVC-ism**: sau khi bỏ nhiễu do STL MSVC 14.44 không hợp clang 14 (`<tuple>` `static_assert` ×4 mỗi tệp,
biến mất với clang ≥ 16 hoặc libc++ của Android), toàn bộ Engine + Core client + Represent3 + S3Client chỉ còn **~10 loại lỗi thật, ~30 dòng
sửa**, không có gì cần thiết kế lại. Phần khó của mobile không nằm ở "biên dịch được" mà ở lớp nền (pha 2) và bộ vẽ (pha 3).

| dự án | tệp | tệp có lỗi | lỗi (kể nhiễu STL) | lỗi THẬT |
|---|---|---|---|---|
| Engine | 137 | 7 | 13 | 13 (1 loại) |
| Core (client) | 103 | 101 | 541 | 11 (4 loại) + 2 include |
| Represent3 | 19 | 5 | 20 | 0 |
| S3Client | 186 | 148 | 1362 | ~70 (6 loại, 5 tệp) |

## 1. Danh sách sửa cho clang (đề nghị làm ngay trên nhánh mobile, đều trung tính với MSVC)

| # | Ở đâu | Lỗi clang | Gốc | Sửa đề nghị |
|---|---|---|---|---|
| 1 | Engine 7 tệp (KBitmap.cpp:122 …) 13 chỗ | `no matching function for call to 'g_MemComp'` | `g_MemComp(LPVOID, LPVOID, DWORD)` gọi với chuỗi hằng `"BM08"` (MSVC cho phép const→non-const) | `KMemBase.h`: tham số `LPCVOID` (không đổi hành vi) |
| 2 | Core `KPlayerChat.cpp:2584` (7), `KItemGenerator.CPP:71` (1) | `cannot initialize LPVOID/LPSTR with const char` | như trên | ép kiểu tại chỗ hoặc `const` hoá tham số |
| 3 | Core `KNpcRes.cpp:309` | `left operand to ? is void` | `a ? SetCurFrame() : GetNextFrame()` một vế `void` | viết `if/else` |
| 4 | Core `BauCua.cpp:8` | `nlohmann/json.hpp` not found | include vcpkg của S3Client, khảo sát chưa thêm | thêm `/I` (không phải lỗi mã) |
| 5 | Core `MultiServer/Common/Utils.h:36` | `atlbase.h` not found | Core client kéo ATL qua Utils.h | tách phần ATL ra `#ifdef _WIN32`; Android không có ATL |
| 6 | S3Client `UiMsgCentrePad.h:95` (→ 361×2 lỗi ở 101 tệp) | `'static' member function 'CloseWindow' overrides a virtual` | `static void CloseWindow(bool)` che `virtual int KWndWindow::CloseWindow(bool)` — MSVC lỏng | đổi tên hàm static (ví dụ `CloseWnd`) + các chỗ gọi |
| 7 | S3Client `WndObjContainer.cpp:826–879` (56+) | `expected expression` | biến tên **`or`** — từ khoá thay thế của C++ (MSVC không bật) | đổi tên `or` → `rc` |
| 8 | S3Client `UiToaDo.cpp:455` | `\U used with no following hex digits` | `"…UserData\UiToaDo.ini"` — `\U` là escape | `\\U` |
| 9 | S3Client `MouseHover.cpp` dòng 1 | `unknown type name 'ï»¿'` | tệp có **hai BOM** (BOM + BOM bọc UTF-8) | bỏ BOM thừa (xem luật BOM/cp1258 trong MEMORY) |
| 10 | S3Client `AntiHack/*` 3 tệp | `ordered comparison between pointer and zero` | `hwnd > 0` | `!= NULL` (AntiHack chỉ Windows, có thể loại khỏi mobile) |

Nhiễu không phải mã: `[HEADER HE THONG] static_assert get<T>(tuple…)` ×4/tệp, `redefinition`/`expected identifier` trong header hệ thống
(6 tệp Core) — do LLVM 14 quá cũ so với STL 14.44 (`_ALLOW_COMPILER_AND_STL_VERSION_MISMATCH` chỉ tắt kiểm tra phiên bản).

## 2. Bộ mã TCVN3/GBK và clang

2/3 số tệp mang byte > 127 (Engine 168/291, Core 183/300, S3Client 301/408). clang giữ nguyên byte trong chuỗi hằng (chỉ cảnh báo
`-Winvalid-source-encoding`), nên **không cần chuyển mã** để biên dịch; chỉ lỗi khi byte cao nằm ngoài chuỗi/chú thích (trường hợp BOM kép ở #9).
Với Android, `/source-charset` không tồn tại nhưng cũng không cần: byte vào = byte ra, đúng như MSVC hiện nay.

## 3. Khảo sát tĩnh phụ thuộc Windows (đầu vào pha 2)

`ketqua/WINAPI_TONGHOP.md`. Tóm tắt (số lần xuất hiện, không kể chú thích):

| phân hệ | cửa sổ/thông điệp | tệp | luồng/đồng bộ | thời gian | Winsock | DirectX cũ | hộp thoại/IME | registry/ini | CRT MSVC |
|---|---|---|---|---|---|---|---|---|---|
| Engine | 90 | 47 | 51 | 26 | 25 | 38 | 19 | 15 | 38 |
| Core | 20 | 24 | 92 | 268 | 8 | 0 | 1 | 36 | 149 |
| S3Client | 246 | 44 | 60 | 69 | 0 | 0 | 42 | 66 | 265 |
| Represent3 | 23 | 5 | 16 | 24 | 0 | 264 | 0 | 5 | 3 |

`__asm` còn 18 tệp (Engine 12 — bộ vẽ mềm DDraw cũ `KDraw*`, KColors, KPalette; iRepresent 2; Represent3 1; JpgLib 3), phần lớn đã có nhánh C
nhờ pha 0 (x64 không có hợp ngữ nội tuyến). Lớp thay thế theo đúng ba nhóm của phương án: **Platform4** (cửa sổ/tệp/luồng/thời gian/IME — SDL3),
**Net4** (Winsock → BSD socket, `KNetClient`), **Sound4** (DirectSound/mp3 → miniaudio). Lát 2a (SDL3 cửa sổ + đầu vào) đã làm: `BANGIAO_SDL_PHA2_0809.md`.

## 4. Bước tiếp của pha 1

1. Sửa 10 mục ở §1 (trung tính MSVC), chạy lại `clang_survey.py` → mục tiêu 0 lỗi thật.
2. Chạy clang với `--target=aarch64-linux-android24` + sysroot NDK để ra danh sách thiếu header/ký hiệu Windows theo tệp (chỉ biên dịch, chưa link);
   cần một `windows.h` giả tối thiểu (kiểu `DWORD/BOOL/HWND…` + `KWin32.h`) — chính là bộ khung của Platform4.
3. Cài LLVM ≥ 16 trên Windows (hoặc dùng clang-cl của VS khi có) để bỏ nhiễu STL và bật `-Wall` xem cảnh báo thật.
