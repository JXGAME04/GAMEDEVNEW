# BÀN GIAO — DỰNG LẠI HỆ GHI/PHÁT LẠI BẢN DIỄN `.jxr` (bỏ hẳn nhánh BandiCam)

Ngày: 20/08/2026 · **ĐÃ THI CÔNG, ĐÃ BUILD, ĐÃ TRIỂN KHAI — CHƯA CHẠY THỬ TRONG GAME.**
Hồ sơ dịch ngược nền: [`PHANTICH_QUAY_MANHINH_DICHNGUOC.md`](PHANTICH_QUAY_MANHINH_DICHNGUOC.md)

---

## 0. TÓM TẮT

Đã **xoá toàn bộ** nhánh quay video BandiCam và **dựng lại từ đầu** hệ replay `.jxr` của bản Linux tham chiếu:
ghi lại chính các **lời gọi phương thức của `iRepresentShell`** ra tệp `.jxr` nén zlib, phát lại bằng chính engine game.

**Đã kiểm chứng tới byte** (không phải suy đoán):
- 7 chuỗi định dạng `Rec()` (`pnwh`, `pun`, `xyz`, `npub`, `npusim`, `npcxyolzb`, `npscl`) và 7 hằng `nType` — đọc thẳng bằng capstone từ `represent2.dll` tham chiếu.
- 38 ô `ret imm` của vtable tham chiếu ⇒ chốt được ánh xạ sang 37 hàm ảo của ta.
- Quy ước gọi khi phát lại là `__thiscall` (this trong `ecx`) — nên adapter chỉ cần là lớp C++ thường.
- `KUiPlayerBaseInfo.Name` nằm ở **+0x20**, đúng bằng `info+0x20` mà bản tham chiếu truyền cho `StartRec`.
- **Vtable adapter trong `Game.exe` đã build**: 9 phương thức thật nằm **chính xác** tại 9 offset `jxreplay.dll` chốt cứng, 29 ô còn lại đều rỗng.

---

## 1. SƠ ĐỒ HỆ THỐNG SAU KHI DỰNG

```
 ┌── LỐI VÀO ───────────────────────────────────────────────────────────────┐
 │ Ctrl+R  Ui\autoexec.lua + kethop.lua → Replay([[rec]])                    │
 │ Ctrl+S                               → Replay([[endrec]])                 │
 │ Ctrl+P                               → Replay([[pauserec]])               │
 │ Nút [Rec] thanh công cụ  ClassType=Player_Recorder                        │
 │ Open([[rec]]) (mục 24 l_WindowList)  → bật/tắt thanh điều khiển phát lại   │
 └───────────────────────────┬──────────────────────────────────────────────┘
                             ▼
   ShortcutKey.cpp   {"Replay", LuaReplay}  →  JxReplay_DoVerb(szVerb)
                             ▼
   JxReplay.cpp   ── 8 động từ: rec endrec play stop pause speedup slowdown pauserec
                             ▼
   IJXReplay* (jxreplay.dll)          ◄── nạp trong InitRepresentShell
                             ▲
 ┌───────────────────────────┴──────────────────────────────────────────────┐
 │ ĐƯỜNG GHI                                                                 │
 │   KRepresentShell2 tự gọi Rec() trong ĐÚNG 7 phương thức:                 │
 │     CreateImage           → nType 1  "pnwh"                               │
 │     ClearImageData        → nType 4  "pun"        (bộ ghi DLL vứt bỏ)     │
 │     DrawPrimitives        → nType 11 "npub"       ← dòng chính            │
 │     DrawPrimitivesOnImage → nType 12 "npusim"                             │
 │     OutputRichText        → nType 13 "npscl"                              │
 │     OutputText            → nType 14 "npcxyolzb"                          │
 │     LookAt                → nType 17 "xyz"                                │
 │   Cổng bật/tắt = m_nReplayStatus, do game đẩy xuống MỖI KHUNG qua         │
 │   SetReplayTimeAndStatus(nTime = bộ đếm khung 30Hz, nStatus)              │
 └──────────────────────────────────────────────────────────────────────────┘
 ┌──────────────────────────────────────────────────────────────────────────┐
 │ ĐƯỜNG PHÁT                                                                │
 │   jxreplay.dll gọi ngược vào KJxrDrawAdapter (38 ô, thứ tự bản tham chiếu)│
 │   → chuyển tiếp sang g_pRepresentShell                                    │
 │   Máy bơm: KUiJxrPlayer::PlayPump()  (96 ms/khung = 0x60, đúng bản gốc)   │
 │     RepresentBegin → PlayTick → Wnd_RenderWindows → RepresentEnd          │
 └──────────────────────────────────────────────────────────────────────────┘
```

---

## 2. TỆP ĐÃ XOÁ (nhánh BandiCam)

| Tệp | Ghi chú |
|---|---|
| `Sources/S3Client/Ui/UiCase/UiCapture.cpp` | 243 dòng, lớp `KUiCapture` |
| `Sources/S3Client/Ui/UiCase/UiCapture.h` | |
| `Sources/S3Client/bandicap.h` | SDK BandiCam 2.5 — **kèm theo là bỏ luôn khoá bản quyền `MAIET-GUNZ-20080916` của game khác** |
| `UiShell.h:149-156` | lớp `Player_Rec` |
| `UiShell.cpp:986-1019` | cài đặt `Player_Rec` |
| `ShortcutKey.h:37` | `SCK_SHORTCUT_REC "Switch([[rec]])"` (động từ sai xưa nay) |
| `S3Client.cpp` | khối đọc `RecPath` + khối bơm BandiCam trong `GameLoop` |
| client: `Ui\Ui3\UiCapture.ini`, `Spr\Ui3\UiCapture\` (6 spr), `JxRelay\`, `config.ini` khoá `RecPath` | |

`grep` toàn cây `Sources` cho `KUiCapture|bandicap|Player_Rec|m_bandiCaptureLibrary` = **0 kết quả**.

---

## 3. TỆP MỚI

| Tệp | Vai trò |
|---|---|
| `Sources/Represent/iRepresent/iJxReplay.h` | giao diện `IJXReplay` 19 ô, khớp từng ô với `jxreplay.dll` |
| `Sources/S3Client/JxrDrawAdapter.h/.cpp` | **lớp chuyển tiếp 38 ô** cho đường phát lại |
| `Sources/S3Client/JxReplay.h/.cpp` | tầng điều phối: nạp DLL, trạng thái, nhịp, 8 động từ, hộp thoại mở tệp |
| `Sources/S3Client/Ui/UiCase/UiJxrPlayer.h/.cpp` | thanh điều khiển phát lại 168×26, 6 nút |

## 4. TỆP ĐÃ SỬA

| Tệp | Sửa gì |
|---|---|
| `iRepresentShell.h` | **nối** 3 hàm ảo vào CUỐI: `SetJxReplay`, `SetReplayTimeAndStatus`, `SetReplayTime` |
| `KRepresentShell2.h/.cpp` | 3 setter + **7 điểm gọi `Rec()`** + 3 thành viên |
| `KRepresentShell3.h/.cpp` | 3 hàm lưu trữ (định dạng `.jxr` chỉ hỗ trợ 2D) |
| `S3Client.cpp` | `JxReplay_Init()` sau `Create()`, `JxReplay_Exit()` trước `Release()`, `JxReplay_Breathe()` + nhánh phát lại đầu `GameLoop`, `JxReplay_OnGameFrame()` mỗi khung logic |
| `ShortcutKey.cpp` | `LuaReplay`, đăng ký `{"Replay", LuaReplay}`, `case 24` |
| `UiShell.h/.cpp` | lớp `Player_Recorder` (đúng tên lớp bản tham chiếu) |
| `S3Client.vcxproj` | bỏ 3 mục cũ, thêm 5 mục mới, thêm `comdlg32.lib` |

> 🟢 **Vì chỉ NỐI hàm ảo vào cuối `iRepresentShell`, mọi chỉ số ô sẵn có giữ nguyên** ⇒ `CoreClient.dll` và Core **không phải build lại**.

---

## 5. TRIỂN KHAI

Build tại `D:\GAMEDEVNEW` rồi chép tay sang `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\`:

| Thứ | Trạng thái |
|---|---|
| `Game.exe` (Release\|Win32) | ✅ 1.253.888 B (bản có nhật ký chẩn đoán) — bản cũ đổi tên `Game.exe.truoc_jxr` |
| `Represent2.dll` (**Debug\|Win32**, đúng luật trộn CRT) | ✅ 130.048 B — bản cũ `Represent2.dll.truoc_jxr` |
| `jxreplay.dll` + `msvcr80.dll` + `msvcp80.dll` + manifest | ✅ chép từ `D:\ServerLinux\Patch` |
| `zlib.dll` | ⚠️ **KHÔNG dùng bản của `Patch`** (đóng gói, `DllMain` hỏng) — dùng bản sạch 69.632 B, xem mục 6b |
| `UserData\Temp\` và `JxRep\` | ✅ đã tạo (**thiếu là hỏng IM LẶNG cả ghi lẫn phát**) |
| 2 tệp ini giao diện replay (GBK) → `Ui\Ui3\` | ✅ 858 B + 1.729 B |
| 8 ảnh SPR | ✅ **đã có sẵn trong `update01.pak`** — không phải vẽ gì |
| `Ui\Setting.ini` khoá `[InfoString]` 51-54 | ✅ byte TCVN3 lấy nguyên từ `stringtable_client.txt` bản tham chiếu |
| `UiToolsControlBar.ini` | ✅ thêm `Button5=Rec` + mục `[Rec] ClassType=Player_Recorder` |
| `autoexec.lua` / `kethop.lua` | ✅ `Open([[rec]])` → `Replay([[rec]])` |

Sao lưu ở `bin\client\_backup_truoc_jxr\`.
🔴 **Client đang chạy lúc triển khai** nên phải đổi tên binary — **cần thoát và mở lại client mới có hiệu lực.**

---

## 6. NHỮNG CHỖ CỐ Ý KHÁC BẢN THAM CHIẾU (nói rõ để khỏi tưởng là lỗi)

| # | Bản tham chiếu | Ở đây | Lý do |
|---|---|---|---|
| 1 | Hai lối vào UI **đều bị chú thích** (nút `[Rec]`, nút `[OpenRep]`) | **Bật cả hai** | Giống 100% theo nghĩa đen = tính năng vô hình |
| 2 | Nút `[Open]` trên thanh phát lại bị `Enable(0)` | **Để sống**, mở hộp thoại chọn tệp | Client ta không có nút `[OpenRep]` ở màn đăng nhập ⇒ nếu khoá thì **không còn đường nào xem lại bản diễn** |
| 3 | Truyền thẳng con trỏ shell cho `SetDrawInterface` | Dùng **lớp adapter** | Vtable ta lệch 8/9 ô (do có thêm `OutputVNText`); truyền thẳng sẽ gọi nhầm hàm và sập ngay khung đầu |
| 4 | Nút trái = chỉ BẮT ĐẦU ghi, dừng qua menu chuột phải | Nút trái = **công tắc** bật/tắt | `KWndButton` của ta không có menu chuột phải đó |
| 5 | Thoát khi đang phát chạy `Replay([[rec]])` | Dùng `RequestStopPlay()` | Bản gốc gần như chắc chắn **lỗi sao chép** (`rec` chỉ chạy ở state 0/5 nên không làm gì) |
| 6 | `KWndWindow` có 16 ô ảo (`Paint`/`GetToolTipInfo`/`SetToolTipInfo` là ảo) | Giữ nguyên 13 ô của ta | Đổi sẽ lan ra **toàn bộ cây UI client**, mà `jxreplay.dll` **không hề gọi vào lớp này** ⇒ không ảnh hưởng tính năng |

**Giữ nguyên có chủ đích (trung thành):** hai nút **"Nhanh"/"Chậm" KHÔNG làm gì** — ô vtable tương ứng trong `jxreplay.dll` là hàm rỗng (1 byte `0xC3`). Động từ `play` là **mã chết**. `ClearImageData` vẫn phát `Rec()` dù bộ ghi vứt bỏ.

---

## 6b. 🔴 LẦN CHẠY THỬ ĐẦU TIÊN — TÍNH NĂNG CHẾT IM LẶNG, ĐÃ TÌM RA VÀ SỬA

**Triệu chứng:** bấm nút quay phim không ra gì, không thông báo, không log.

**Nguyên nhân gốc:** `zlib.dll` chép từ `D:\ServerLinux\Patch` (1.114.172 B) **bị đóng gói bảo vệ**
(có section `.vng0`/`.vng1`). `DllMain` của nó hỏng với **err=1114**, kéo theo
`LoadLibrary("JXReplay.dll")` trượt ⇒ `l_pJxReplay = NULL` ⇒ mọi động từ bị bỏ qua im lặng.

**Đã sửa:** thay bằng `zlib.dll` **sạch 69.632 B** lấy từ
`C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky 2.0\` — đã kiểm **khớp đủ 6 ordinal**
(4=deflate, 6=deflateEnd, 8=deflateInit_, 19=inflate, 20=inflateEnd, 22=inflateInit_).
Bản đóng gói được đổi tên thành `zlib.dll.dongoi_hong`.

**Kết quả dò sau khi sửa** (EXE 32-bit chạy từ chính `bin\client`):

```
zlib.dll         NAP OK   (handle 10000000)
jxreplay.dll     NAP OK   CreateJxReplayInterface = CO   goi thu -> con tro 037C1058
```

**Hai bẫy phụ đã sửa cùng lúc:**
- Nút `[Rec]` đặt sai chỗ: chép từ `UiToolsControlBar0.ini` nên có `Left=295 Top=4 W=20 H=20`,
  trong khi thanh công cụ đang dùng có hàng nút ở `Top=68`, bước 30, cỡ 28×28.
  Đã đặt lại **`Left=242 Top=68 W=28 H=28`**.
- Thêm nhật ký chẩn đoán **`bin\client\jx_replay.log`** (hàm `JxrLog`): ghi lúc nạp DLL
  (kèm `GetLastError`) và mỗi lần bấm động từ. Hệ này thất bại im lặng theo đúng bản gốc
  nên không có log thì không lần ra được gì.

**Đừng nhầm:** `msvcr80.dll`/`msvcp80.dll` báo `err=1114` khi `LoadLibrary` **thẳng** là bình thường
(DLL kiểu SxS từ chối nạp trực tiếp khi không có activation context). `jxreplay.dll` tự nạp
chúng qua manifest của nó. Muốn dò phải dùng EXE **32-bit** đặt trong **chính `bin\client`**
(SxS dò theo thư mục chứa EXE, không theo thư mục làm việc) và nhớ `SetErrorMode(...)`
nếu không sẽ treo ở hộp thoại lỗi.

---

## 7. CHƯA KIỂM ĐƯỢC — VIỆC PHẢI LÀM TIẾP

1. 🔴 **Chưa chạy thử trong game.** Toàn bộ kết luận là từ dịch ngược + biên dịch + kiểm bố cục vtable trên binary đã build.
2. Chưa có tệp `.jxr` mẫu nào để đối chiếu ⇒ chưa biết tệp ta ghi ra có đọc được bằng client tham chiếu không.
3. Nhịp: `.jxr` có lưới thời gian **3 tick** ở 30 tick/giây ⇒ hình học chỉ được ghi **10 lần/giây**. Client ta tách `PaintFps` (30-60) khỏi `GAME_FPS` (18) — **chưa rõ có lệch thời gian khi phát lại không**.
4. Nhánh phát lại trong `GameLoop` `return` sớm, bỏ qua `g_pCoreShell->Breathe()`. Chưa kiểm mạng có bị ngắt khi xem lâu không.
5. Ba khiếm khuyết **không sửa được** trong `jxreplay.dll` (DLL đóng): `WriteHeader` dùng `lea` ⇒ phiên ghi bị ngắt giữa chừng là **mất trắng**; bộ đếm khung là **BYTE** ⇒ tràn header từ khung 199, đè bảng tài nguyên từ khung 214; `strcpy` tên phiên không giới hạn (đã tự kẹp 31 ký tự ở phía ta).
6. Chưa dựng nút `[OpenRep]` ở màn hình đăng nhập (bản tham chiếu có). Hiện mở tệp qua nút `[Open]` trên thanh phát lại.

### Kịch bản nghiệm thu đề nghị
1. Thoát hẳn client, mở lại → vào game.
2. Bấm **Ctrl+R** → phải hiện *"Hiện đang quay phim"*.
3. Đi lại, đánh vài chiêu ~30 giây.
4. Bấm **Ctrl+S** → phải hiện *"File ảnh đã được lưu tại …"* và có tệp trong `bin\client\JxRep\2D_YY-MM-DD_HH-MM.jxr` (**> 0 byte**).
5. `Ctrl+R` mở thanh điều khiển (hoặc `Open([[rec]])`) → nút **Mở ra** → chọn tệp → phải phát lại được.
6. Nếu tệp `.jxr` 0 byte: kiểm `bin\client\UserData\Temp\` có tồn tại không — đây là điểm chết số 1.
