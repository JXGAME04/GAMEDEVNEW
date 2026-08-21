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

---

## 8. 🟢 PHIÊN 20/08 CHIỀU-TỐI — ĐÃ TÌM RA GỐC "KHÔNG XEM ĐƯỢC" VÀ DỰNG TRÌNH XEM

### 8.1 ĐO THẬT: khâu GHI ĐÚNG, DLL PHÁT ĐÚNG — file KHÔNG hỏng

Trên máy thật đã có **2 tệp `.jxr` ghi ra hợp lệ**: `bin\client\JxRep\2D_26-08-20_20-01.jxr`
(167 KB) và `2D_26-08-20_20-05.jxr` (38 KB). Cả hai giải nén zlib OK, magic `JXR\0`, ver 14,
name `CaiBang`.

Đã build **harness 32-bit** (`scratchpad\test_jxr.cpp`, adapter 38 ô giả) đặt cạnh `Game.exe`
trong `bin\client`, gọi thẳng `Play()` vào `jxreplay.dll`:
- `Play()` trả 1, trạng thái 0→3→4, `PlayTick` chạy 16 tick rồi báo HẾT đúng.
- Callback ra **DỮ LIỆU THẬT**: `LookAt(47710,108493)` toạ độ bản đồ thật;
  `CreateImage '*PlaceMap_0..7*'` 128×128; `DrawPrimitives` 22.185 lần; `OutputText` 1.823;
  `OutputRichText` 1.343.

⇒ **Ánh xạ vtable adapter (ô 8=CreateImage, 18=DrawPrimitives, 4=OutputText, 5=OutputRichText,
21=LookAt) nay đã XÁC NHẬN bằng phát lại thật** — hết "suy luận". Mục 2 và 3.1 của
`PHANTICH_QUAY_MANHINH_DICHNGUOC.md` đã được kiểm chứng.

### 8.2 GỐC LỖI: KHÔNG có đường UI nào để BẮT ĐẦU phát lại

Quét toàn bộ `autoexec.lua` + `kethop.lua` đã deploy: Ctrl+R/S/P đều gọi `Replay([[rec]])`
để **GHI**; **`Open([[rec]])` (mở thanh phát lại, case 24) KHÔNG gắn với phím hay nút nào**.
Nút toolbar `[Rec]` = `Player_Recorder` cũng chỉ để GHI (`UiShell.cpp:989`).
Bản tham chiếu xem replay từ **nút `[OpenRep]` màn đăng nhập** — thứ ta CHƯA dựng (mục 7.6).
⇒ Người chơi ghi được nhưng **không có đường nào để xem**. Đúng triệu chứng "in ra file không xem được".

Đã dịch ngược đường xem của bản tham chiếu để làm cho khớp:
- Handler `[OpenRep]` `0x00476C30` → mở hộp thoại `*.jxr` (`0x00476B70`) → `g_pReplay->vtbl+0x10`
  = **`Play(szFile)`** → `Sleep(10)` → mở bar (`0x0052DC10`) → **`EnterReplayMode(1)` `0x004760E0`
  ĐẶT CỜ STYLE `0x2000000` LÊN UI LOGIN** (ẩn login để replay không bị che).
- `PlayTick` khi hết khung tự gọi `PlayCleanup 0x10008190` → `SetStatus(0)` ⇒ **tự về IDLE**, không kẹt vòng.

### 8.3 ĐÃ THI CÔNG — nút `[OpenRep]` ở màn đăng nhập (build + deploy xong, CHƯA chạy thử trong game)

Vì `KMyApp::GameLoop` **chạy cả ở màn đăng nhập**, nhánh phát lại `.jxr` sẵn có
(`S3Client.cpp:1252-1259`) tự kích hoạt khi state==4 — nên đặt trình xem ở login là **sạch nhất**
(kho ảnh trống, không đụng `*PlaceMap*` của map đang chơi như khi xem trong game).

| Tệp | Sửa |
|---|---|
| `Ui/UiCase/UiLogin.h` | thêm `KWndButton m_OpenRep`; 2 static `EnterReplayHide()`/`RestoreAfterReplay()`; cờ `m_bHiddenForReplay` |
| `Ui/UiCase/UiLogin.cpp` | `AddChild(&m_OpenRep)`, `m_OpenRep.Init(&Ini,"OpenRep")`, xử lý click → `JxReplay_OpenFileAndPlay()` rồi `EnterReplayHide()`; cài `EnterReplayHide`/`RestoreAfterReplay` (ẩn/hiện form+nền login, giống `EnterReplayMode`) |
| `Ui/UiCase/UiLoginBg.{h,cpp}` | thêm `Restore()` (hiện lại nền login; `SetConfig` bỏ qua vì config không đổi) |
| `S3Client.cpp` | sau nhánh phát lại trong `GameLoop`: `KUiLogin::RestoreAfterReplay()` (khôi phục login khi phát xong) |
| `bin\client\Ui\Ui3\UiLogin.ini` | thêm mục `[OpenRep]` (Left=20 Top=560 28×28, tái dùng SPR `录像按钮`, Tip="Xem lai ban dien (.jxr)") |

Build `Release\|Win32` (S3Client → `Game.exe` **1.254.912 B**, 20/08 21:13), deploy sang
`E:\...\TESTLOFFF_ONLINE\bin\client\` (bản cũ đổi tên `Game.exe.dangchay_old` vì client đang chạy).
🔴 **Phải thoát và mở lại client mới có nút.**

**Cơ chế xem:** ở màn đăng nhập, bấm nút camera góc dưới-trái → chọn tệp `.jxr` trong `JxRep\`
→ login tự ẩn, replay phát toàn màn hình, thanh điều khiển (Tạm dừng/Tiếp/Dừng) tự hiện →
bấm **Dừng** hoặc hết phim → tự về màn đăng nhập. Nút [Open] trên thanh phát vẫn để chọn tệp khác.

### 8.4 CHƯA KIỂM ĐƯỢC (thành thật)
- 🔴 **Chưa chạy thử trong game** — chưa thấy pixel thật. Đường ghi + phát của DLL đã đo thật;
  render tại login còn suy luận (kho ảnh trống, sprite nạp theo-yêu-cầu từ pak như login vẫn làm).
  Có đường thoát an toàn: nút **Dừng** trên thanh phát luôn đưa về login (không kẹt).
- Nút camera hơi kín đáo (giống bản tham chiếu). Nếu muốn dễ thấy hơn: đổi `Left/Top` trong
  `[OpenRep]` hoặc dùng SPR to hơn.
- Giới hạn DLL: xem **mục 9** — con số "≈20 giây" tôi viết ở đây lúc đầu là **SAI**, đã đo lại và đính chính.

---

## 9. 🔬 ĐO THẬT `jxreplay.dll` — ĐÍNH CHÍNH MỤC 7.5 VÀ VÁ 1 BYTE

> Bối cảnh: câu "DLL đóng nên **không sửa được**" ở mục 7.5 là **nói quá**. DLL đóng chỉ có nghĩa
> là không có mã nguồn — đã dịch ngược được thì **vá thẳng byte cũng được**. Mục này thay thế mục 7.5.

### 9.1 🔴 ĐÍNH CHÍNH LỚN — "quay > 20 giây thì hỏng" là SAI

Bộ đếm khung là BYTE thật (`0x1000A4F8 add byte ptr [esi+0x35], 1`), bảng offset ở header+0x132
trong header 0x450 byte ⇒ **chỉ chứa 199 mục**, tràn từ khung 200. Lỗi có thật. **Nhưng điều kiện
kích bộ đếm thì hoàn toàn khác điều tôi tưởng.**

Dịch ngược `0x10003660`: đó là **`std::map<string, WORD>` tra TÊN tài nguyên → id**; gặp tên cũ thì
tái dùng id (`al=0`), chỉ **tên MỚI** mới cấp id mới (`lea ebx,[eax+1]`). Bộ đếm khung chỉ tăng khi
id chạm `0xFFFE` (`0x10005BDF cmp word ptr [esp+0x54], 0xfffe` → `NextFrame` + `Signal(1)`).

**Đo thật** (harness `scratchpad\test_frames.cpp`, ghi N lệnh `CreateImage`):

| Thí nghiệm | Kết quả `nFrameCount` |
|---|---|
| 70.000 tên **giống nhau** | **1** |
| 70.000 tên **khác nhau** | **1** |
| 200.000 tên **khác nhau** (có nghỉ cho luồng nền) | **4** — offset khung cách đều ~65.534 tên |
| 2 tệp **thật** của người chơi (14 giây, 1,34 MB) | **1** |

⇒ **1 khung = 65.534 TÊN TÀI NGUYÊN KHÁC NHAU**, không liên quan tới thời lượng hay dung lượng.
Muốn tràn 199 khung cần **≈13 triệu tên tài nguyên khác nhau** trong một phiên quay.
Trong game số sprite khác nhau chỉ vài nghìn và **được tái dùng** ⇒ **KHÔNG BAO GIỜ chạm tới.**

**Kết luận: quay bao lâu cũng được. Không cần vá, không cần cảnh báo người chơi.**

### 9.2 🟢 ĐÃ VÁ 1 BYTE — lỗi `WriteHeader` ghi rác ngăn xếp

```
0x100097F4   8D 4C 24 10   lea ecx,[esp+0x10]   ; SAI: lay DIA CHI cua o chua con tro
             8B 4C 24 10   mov ecx,[esp+0x10]   ; DUNG: lay CHINH con tro header
```
Cùng độ dài 4 byte ⇒ vá đúng **1 byte** `8D`→`8B`, không xê dịch gì trong ảnh PE.

**Chứng minh lỗi có thật** (ghi 200.000 lệnh rồi `TerminateProcess`, soi 0x450 byte đầu tệp tạm):

| | 16 byte đầu header | byte khác 0 / 1104 |
|---|---|---|
| DLL **gốc** | `f8 7f 8f 01 ec 7f 8f 01 e8 7f 8f 01 40 0d 03 00` | **430** |
| DLL **đã vá** | `00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00` | **0** |

Bản gốc ghi thẳng **địa chỉ ngăn xếp** (`0x018F7FF8`) và cả **tên tệp tạm** vào header, không có magic
`JXR\0`. Bản vá cho đúng 1104 byte 0 như `memset` ở `KJXRFile::Open` dự định.

**Kiểm tương thích ngược** — phát lại tệp THẬT `2D_26-08-20_20-05.jxr` bằng DLL đã vá cho kết quả
**giống hệt** bản gốc: 16 tick, `OutputText` 1.823, `OutputRichText` 1.343, `CreateImage` 74,
`DrawPrimitives` 22.185, `LookAt` 16. Ghi tệp mới cũng bình thường (`JXR\0`, ver 14).
⚠️ Tệp do harness sinh (chỉ toàn `CreateImage` tên giả) thì decoder dừng sớm — nhưng **DLL gốc cũng
dừng y hệt**, nên đó là do dữ liệu giả, không phải do bản vá.

**Đã triển khai:** `bin\client\JXReplay.dll` (90.112 B, byte `0x97F4 = 8B`).
Sao lưu: `JXReplay.dll.goc` và `JXReplay.dll.dangchay_old` (đều là bản gốc 31/03/2020).
Công cụ vá: `scratchpad\patch_jxr.py` (mặc định dry-run, phải `--apply`; từ chối vá nếu byte không khớp).

**Lợi ích thực tế của bản vá này là VỪA PHẢI**, nói thẳng: phiên quay kết thúc bình thường bằng
`EndRec` vốn đã ghi lại header đúng, nên tệp `.jxr` xuất ra **trước hay sau khi vá đều đọc được**.
Vá giải quyết: (a) không còn rò rỉ địa chỉ ngăn xếp + tên tệp vào tệp, (b) tệp tạm dang dở có header
hợp lệ thay vì rác. **Không** sửa được việc phiên quay bị ngắt vẫn mất bản diễn (vì `EndRec` mới là
nơi ghép tệp cuối).

### 9.4 🔴🟢 CHẠY THỬ THẬT: "xem lại bị ĐEN NỀN, chỉ thấy tên và thanh máu" — ĐÃ TÌM RA VÀ SỬA

**Tin tốt kèm theo:** người chơi bấm được nút xem ở màn đăng nhập và **replay có chạy** (thấy tên +
thanh máu chuyển động) ⇒ toàn bộ đường UI/nạp/phát của mục 8 là ĐÚNG. Chỉ thiếu phần hình nền.

**Chẩn đoán bằng harness** (`scratchpad\test_img.cpp` — adapter đọc thẳng `KRUImage` khi phát lại),
chạy trên bản ghi thật `2D_26-08-20_22-15.jxr`:
```
CreateImage '*PlaceMap_0..13*' 128x128        <- CO tao anh, nhung RONG
Draw#1 genre=3 szImage='_*PlaceGround*_#~6~#_' uImage=0 nISPos=0
Draw#2 genre=3 szImage='_*PlaceGround*_#~21~#_'
...
DrawPrimitives=41171  CreateImage=74  DrawPrimOnImage=11
```
⇒ Lệnh vẽ tham chiếu tới ảnh **`_*PlaceGround*_#~N~#_`** (ảnh nền đất, tạo lúc chạy), mà **trong cả
bản ghi KHÔNG có `CreateImage` nào tạo ra chúng** — chúng đã được tạo TRƯỚC khi người chơi bấm quay.
Lúc phát lại `GetImage` trả NULL ⇒ không vẽ gì ⇒ **nền đen**, chỉ còn chữ (`OutputText`) và hình học.

**Gốc rễ — bỏ sót `SetParam`.** `jxreplay.dll` có sẵn bộ "chụp trạng thái ban đầu": REC vtable ô 6
(`0x10007700`) chạy **mỗi lần vẽ, KỂ CẢ khi chưa bấm quay** (nó được gọi TRƯỚC phép kiểm `status==1`
trong `Rec` dispatch), ghi nhớ ảnh đã tạo / nội dung đã vẽ, rồi `StartRec` xả hết vào đầu tệp.
Nhưng mỗi nhánh bị kẹp bởi một cờ riêng — giải mã bảng nhảy tại `0x100077CC`/`0x100077E4`:

| nType | nhánh | cờ kiểm | bật bởi |
|---|---|---|---|
| 1 `CreateImage` | `0x10007798` | `[K2DREC+0x5F4]` | `SetParam(1, x)` |
| 12 `DrawPrimitivesOnImage` | `0x10007750` | `[K2DREC+0x5F0]` | `SetParam(12, x)` |
| 17 / 20 / 21 | `0x1000776E` / `0x10007783` / `0x100077B6` | — | |

Cả hai cờ **mặc định 0** ⇒ bảng chụp rỗng ⇒ nền đen. `SetParam` giải mã (`0x10004A80`):
`id 1 → +0x5F4`, `id 12 → +0x5F0` (kèm init `0x10003FD0`), `id 11 → +0x5F8`; id 1 và 12 **bắt buộc
`status == 0`**.

**Đã sửa** (`JxReplay.cpp`, trong `JxReplay_Init()` ngay sau `SetDrawInterface`, lúc status vẫn 0):
```cpp
l_pJxReplay->SetParam(JXR_PARAM_TRACE_CREATEIMAGE,     1);   // id 1
l_pJxReplay->SetParam(JXR_PARAM_TRACE_DRAWPRIMONIMAGE, 1);   // id 12
```

🔴 **ĐÂY LÀ CHỖ CỐ Ý KHÁC BẢN THAM CHIẾU (thêm vào mục 6).** Đã đối chiếu byte: bản gốc **KHÔNG gọi
`SetParam` ở bất kỳ đâu** — cả `KMyApp::InitRepresent` (`0x0056D520`, chỉ có `LoadLibrary` →
`SetJxReplay` → `SetDrawInterface` → `SetReplayTimeAndStatus(1,0)`) lẫn nhánh `rec` (`0x0040FFAD`,
chỉ gọi `StartRec`). **Và đó nhiều khả năng chính là lý do CẢ HAI lối vào UI của bản tham chiếu đều
bị chú thích: tính năng của họ vốn đang hỏng đúng kiểu này.** Giống-100%-theo-nghĩa-đen ở đây =
tiếp tục đen nền, nên ta cố ý làm khác.

**Triển khai:** `Game.exe` 1.254.912 B (20/08 22:26), bản trước đổi tên `Game.exe.truoc_setparam`
(đã đối chiếu byte: hai bản khác nhau thật). **Phải thoát và mở lại client.**

**⚠️ Chưa kiểm được:** chưa chạy thật sau bản vá này. Rủi ro còn lại đã lường: bảng chụp tích luỹ
theo thời gian chơi (mỗi ảnh nền mới) ⇒ nếu chơi rất lâu rồi mới bấm quay thì phần đầu tệp `.jxr`
sẽ phình ra. Ảnh nền map hữu hạn và được tái dùng nên dự kiến chấp nhận được, nhưng **nếu thấy tệp
`.jxr` phình bất thường hoặc khựng lúc bấm quay thì báo — khi đó chỉ cần bỏ `SetParam(12,1)`,
chấp nhận nền tĩnh kém chi tiết hơn.**

### 9.5 🔴 ĐÍNH CHÍNH 9.4 + vòng sửa thứ hai: "chỗ thấy nền chỗ không"

**Sau bản vá 9.4, người chơi báo: hết đen hoàn toàn, nhưng "chỗ thấy nền chỗ không".**

🔴 **Đính chính chẩn đoán ở 9.4:** tôi đã kết luận vội "bản ghi thiếu `CreateImage` cho `_*PlaceGround*_`"
— **SAI**. Lúc đó tôi chỉ in 10 dòng `CreateImage` đầu (toàn `*PlaceMap_N*`) rồi suy ra là thiếu.
Đo lại đầy đủ (đối chiếu tập ảnh ĐƯỢC TẠO với tập ảnh ĐƯỢC VẼ) thì **cả hai tệp, trước và sau vá,
đều có đủ 49 `_*PlaceGround*_` + 25 `*PlaceMap_*` và 0 ảnh bị thiếu.**

**Khác biệt thật nằm ở `DrawPrimitivesOnImage` (nội dung vẽ VÀO ảnh):**

| tệp | DrawPrimitives | CreateImage | **DrawPrimOnImage** |
|---|---|---|---|
| `22-15` (trước vá 9.4) | 41.171 | 74 | **11** |
| `22-30` (sau vá 9.4) | 111.288 | 74 | **76** |

⇒ `SetParam(12,1)` **có tác dụng thật** (11 → 76), nên nền mới hiện lên được một phần.

**Nguyên nhân phần còn thiếu:** mỗi ảnh nền được **GHÉP TỪ NHIỀU LỆNH**
`DrawPrimitivesOnImage` — `KScenePlaceRegionC::PrerenderGround` (`:215-292`) chạy 2 vòng (từng ô đất
`pGrunodes`, rồi từng vật thể `pObjects`), mỗi lô tối đa `LOCAL_MAX_IMG_NUM` ảnh mới xả một lệnh.
Bảng chụp của DLL chỉ giữ được **khoảng MỘT lệnh cho mỗi ảnh** (76 lệnh cho 74 ảnh) ⇒ mỗi vùng chỉ
dựng lại được một mảnh ⇒ **chỗ có nền chỗ không**. Đây là giới hạn thiết kế của bảng chụp, không sửa
được từ phía ta bằng `SetParam`.

**🟢 Cách sửa (vòng 2) — ép vẽ lại toàn bộ nền NGAY SAU `StartRec`, dùng API có sẵn:**
```cpp
l_pJxReplay->StartRec(szName, (unsigned int)time(NULL));
if (g_pCoreShell && g_pRepresentShell)
    g_pCoreShell->SetRepresentShell(g_pRepresentShell);   // -> RepresentShellReset()
```
`KCoreShell::SetRepresentShell` (`CoreShell.cpp:12542`) gọi thẳng `KScenePlaceC::RepresentShellReset`
(`KScenePlaceC.cpp:1753`), hàm này đặt `m_bRenderGround = true` và hạ `GROUND_IMG_OK_FLAG` của **mọi**
ảnh nền ⇒ khung kế tiếp engine dựng lại toàn bộ nền, và lần này **mọi lệnh đều rơi vào trong phiên
ghi** nên được lưu đầy đủ.
**Không sửa một dòng nào trong Core**, chỉ dùng lại API client vốn đã gọi ở `UiPaint`. Tác dụng phụ
duy nhất: một lần vẽ lại nền ngay lúc bấm quay (có thể khựng nhẹ đúng một khung) và `SetAdjustColorList`
được nạp lại (vô hại).

**Triển khai:** `Game.exe` 1.254.912 B (20/08 22:40), bản trước → `Game.exe.truoc_rerender`
(đối chiếu byte: khác 135.144 byte). **Phải thoát và mở lại client, rồi quay một đoạn MỚI.**

**⚠️ Chưa kiểm được:** chưa chạy thật sau vòng 2. Nếu vẫn còn mảng thiếu thì bước tiếp theo là gọi
thẳng `KScenePlaceC::PrerenderGround(true)` cho từng region trong vùng xử lý (đã có sẵn tham số
`bForce`, khai ở `KScenePlaceC.h:322`) — nhưng khi đó phải mở thêm API từ Core.

### 9.3 Còn lại: `strcpy` tên phiên (`0x10009790`) — CHƯA vá, có chủ ý
Trường tên trong header chỉ 32 byte và DLL dùng `strcpy` không giới hạn. Phía ta **đã tự kẹp 31 ký tự**
trong `JxrVerb_Rec` (`JxReplay.cpp`), nên đường tràn đã bị chặn từ gốc. Vá thêm trong DLL là thừa
và làm lệch thêm khỏi bản tham chiếu.
