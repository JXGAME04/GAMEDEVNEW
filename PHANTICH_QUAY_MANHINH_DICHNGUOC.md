# PHÂN TÍCH — TÍNH NĂNG QUAY MÀN HÌNH / GHI HÌNH GAME (dịch ngược bản Linux tham chiếu + client dự án)

Ngày: 20/08/2026 · **CHỈ PHÂN TÍCH — chưa sửa gì, chưa build, chưa chép tệp, chưa chạy game.**
Phạm vi hỏi: dịch ngược bản Linux, tìm tính năng quay màn hình game; phân tích những chỗ cần thay đổi + sơ đồ.
Phương pháp: 6 mũi dịch ngược song song + 6 mũi phản biện đối kháng + 1 lượt soát cuối (13 tác nhân). Mọi địa chỉ dưới đây đã được ít nhất hai lượt độc lập đối chiếu byte; những chỗ còn suy đoán được ghi rõ ở mục 10.

> **Lưu ý cách gọi tên** (giống ghi chú của hồ sơ F12): `D:\ServerLinux\Patch` **không phải bản gốc Trung Quốc** mà là **bản Việt hoá 2021-2022 CÓ SẴN tính năng**. Gọi là "bản tham chiếu".

---

## 0. KẾT LUẬN NGẮN (đọc mục này là đủ để quyết)

1. **Có tính năng thật, nhưng là HAI công nghệ hoàn toàn khác nhau trùng tên — đây là bẫy lớn nhất của cả hồ sơ.**
   - **Bản tham chiếu** dùng `jxreplay.dll` → ghi **lời gọi vẽ của Represent** ra tệp `.jxr` rồi **phát lại bằng chính engine game**. Không ra video, không chia sẻ ra ngoài được.
   - **Dự án ta** dùng lớp `KUiCapture` bọc **SDK thương mại BandiCam Capture Library 2.5** → ra **video thật** (`.mp4`/`.avi`).
   - Tên tệp `jxreplay32.dll` trong mã dự án **KHÔNG PHẢI** `jxreplay.dll` của bản tham chiếu — nó là `bdcap32.dll` của BandiCam **bị đổi tên để nguỵ trang**. Chép nhầm = ăn hộp thoại assert mà không hiểu vì sao (`GetProcAddress("CreateBandiCapture")` trả NULL).
2. **Phía máy chủ SẠCH 100%.** Quét 73.072 chuỗi trong `jx_linux_y` + 2 module `.so`: không một dấu vết replay/ghi hình. Mọi việc là **100% phía client** — không cần đụng GameServer, không cần thêm gói tin, không cần restart.
3. **Tính năng của dự án CÓ ĐỦ MÃ nhưng CHẾT**, đứt ở 3 mắt xích độc lập: (a) thiếu DLL trong `bin\client`; (b) mục `[Rec]` không có trong tệp `.ini` thanh công cụ **thực sự được nạp**; (c) `case 24` của `Open([[rec]])` bị chú thích. `JxRelay\` rỗng — **tính năng chưa từng chạy thành công một lần nào**.
4. **🔴 NÚT THẮT THẬT SỰ không nằm ở DLL mà ở KHÂU LẤY KHUNG HÌNH.** 52/52 ảnh trong `bin\client\JxCap` **giống nhau từng byte**, md5 `8ed1a736…`, giải mã ra **một màu xanh đặc RGB(0,56,247)** 1024×768. Đường chụp ảnh đang đọc **primary surface** của DirectDraw — thứ Windows 10/11 không còn trả nội dung. **Không sửa chỗ này thì mọi đường đều cho ra video xanh đặc.**
5. **Nguồn khung hình ĐÚNG vẫn còn sống và đọc được 100%**: `m_Canvas` là surface DirectDraw nằm trong **BỘ NHỚ HỆ THỐNG** (`DDSCAPS_SYSTEMMEMORY`), truy cập qua `KCanvas::LockCanvas()` — engine.dll đã xuất khẩu sẵn (ordinal 846/1121). Đây là phát hiện quan trọng nhất của cả đợt.
6. **Bộ SDK BandiCam khớp phiên bản ĐANG NẰM SẴN TRÊN MÁY** — `bdcap32.dll` 2.5.0.250 (11.926.504 B, 02/02/2017) ở **6 cây nguồn** dưới `E:\SourceTuanLe\Source_G7VN\...\Sources\S3Client\`, ngay cạnh `bandicap.h` bản gốc. Đã dịch ngược `CreateBandiCapture` và chứng minh nó chỉ đòi `(ver & 0xFFFFFF00) == 0x02030000` — **khớp đúng**, không hề lệch phiên bản.
7. **Tài nguyên giao diện của CẢ HAI đường đều đã có sẵn, công vẽ ảnh = 0.** 6 SPR của `UiCapture` nằm rời; còn 7 SPR thanh phát lại `.jxr` + 2 tệp `.ini` giao diện replay **nằm ngay trong `update01.pak`/`update03.pak` của chính dự án ta**.
8. Tìm ra **10 lỗi lập trình thật** trong lớp `KUiCapture` của dự án (một cái tràn stack) và **3 khiếm khuyết không sửa được** trong `jxreplay.dll` vì DLL đóng (mục 9).

---

## 1. HAI CÔNG NGHỆ — BẢNG PHÂN BIỆT BẮT BUỘC ĐỌC TRƯỚC

| | Bản tham chiếu (`D:\ServerLinux\Patch`) | Dự án ta (`E:\…\TESTLOFFF_ONLINE\bin\client`) |
|---|---|---|
| Thư viện | `jxreplay.dll` 90.112 B (31/03/2020) | `bandicap.h` = BandiCam Capture Library 2.5 → `bdcap32.dll` |
| Bản chất | Ghi **lời gọi phương thức của `iRepresentShell`** + sự kiện âm thanh, nén zlib | Quay **video thật** bằng SDK ngoài |
| RTTI / lớp | `IJXReplay`, `KJxReplay`, `IREC/K2DREC`, `IPlay/K2DPlay`, `IFile/KJXRFile/KRepFile`, `KJxrLog` | `IBandiCapture`, `CBandiCaptureLibrary`, `KUiCapture`, `Player_Rec` |
| Export | `CreateJxReplayInterface` (rva `0x1380`) | `CreateBandiCapture` (rva `0x00676A10`) |
| Tệp ra | `<thư mục game>\JxRep\2D_%y-%m-%d_%H-%M.jxr` | `<RecPath>\Capture <ngày giờ>.mp4` |
| Điều khiển | Lua `Replay([[rec]])` / `endrec` / `pauserec` + 5 động từ nữa | Lớp `KUiCapture` (nút Rec / Pause) |
| Giao diện | `KUiJxrPlayer` + `录像操作界面.ini` | `UiCapture.ini` + 6 SPR |
| Phím tắt | Ctrl+R / Ctrl+S / Ctrl+P | không có phím nào sống |
| Trạng thái | Đủ trong client tham chiếu, **nhưng cả 2 lối vào UI đã bị chú thích** | **CHẾT** — thiếu DLL trong `bin\client` |

> 🔴 **Đính chính lời tôi nói lúc khảo sát mở đường:** tôi đã nói "DLL bandicam không có trên toàn bộ ổ C/D/E/J". **Sai** — lúc đó tôi chỉ tìm nông. `bdcap32.dll` 2.5.0.250 có thật ở **6 nơi** trên ổ E (danh sách ở mục 11). Đây là tin tốt, nó bỏ hẳn một hạng mục "phải đi xin".

---

## 2. DỊCH NGƯỢC `jxreplay.dll` — KIẾN TRÚC

`PE32`, imagebase `0x10000000`. Import **chỉ có** KERNEL32 (23 hàm luồng/CS/module) + MSVCP80 (3 hàm `std::string`) + MSVCR80 (stdio + `strftime`/`_time32`) + `zlib.dll` (6 ordinal).
**Tuyệt đối không có GDI32, DDRAW, D3D, DSOUND, WINMM, AVIFIL32, MSVFW32** — không có `BitBlt`/`GetDC`/`CreateDIBSection`.
⇒ **Kết luận dứt khoát: đây KHÔNG phải công cụ quay video màn hình.** Nó không thể xuất mp4/avi.

### 2.1 Cấu trúc lớp

```
CreateJxReplayInterface()          rva 0x1380 → jmp 0x10001300
   IJXReplay* __cdecl (void)       singleton, biến toàn cục [0x100131C0]
        │  operator new(0x28)
        ▼
   KJxReplay  (0x28 byte)          vtable 0x10010174, 19 ô
     +0x00 vtable
     +0x04 K2DREC*  m_pRec         singleton 0x573C byte, [0x100131CC], vtable 7 ô @0x10010264
     +0x08 K2DPlay* m_pPlay        singleton 0x0AD4 byte, [0x100131D8], vtable 10 ô @0x10010284
     +0x0C DWORD    m_dwStatus     0..5 (SetStatus chặn cứng >5)
     +0x10 CRITICAL_SECTION (24 B)
```

Gần như toàn bộ 19 ô của `KJxReplay` chỉ là **thunk chuyển tiếp** sang `K2DREC` (ghi) hoặc `K2DPlay` (phát).

### 2.2 Bảng vtable `IJXReplay` — dùng để viết lại header C++

| Ô | Offset | Tên suy ra | Đích | Ghi chú |
|---|---|---|---|---|
| 0 | +0x00 | `Rec(int nTime,int nType,const char* fmt, ...)` | REC vt[1] | **`__cdecl` biến tham số** |
| 1 | +0x04 | `StartRec(const char* szName, DWORD dwParam)` | REC vt[0] | |
| 2 | +0x08 | `PauseRec()` | REC vt[2] | |
| 3 | +0x0C | `EndRec(const char* szDestPath)` | REC vt[3] | |
| 4 | +0x10 | `Play(const char* szFile)` | Play vt[1] | |
| 5 | +0x14 | `PlayTick()` / Stop | Play vt[0] | gọi mỗi khung khi phát |
| 6 | +0x18 | `GetTotalSize()` | Play vt[2] | mặc định trả 1 |
| 7 | +0x1C | `GetLoadedSize()` | Play vt[3] | mặc định trả 0 |
| 8 | +0x20 | `RequestStopPlay()` | Play vt[4] | đặt `+0xAD0 = 2` |
| 9 | +0x24 | `Pause()` | Play vt[5] | đặt `+0xAD0 = 1` |
| 10 | +0x28 | `Resume()` | Play vt[6] | đặt `+0xAD0 = 0` |
| 11 | +0x2C | `SpeedUp()` | Play vt[7] = `0x1000B450` | **🔴 HÀM RỖNG, 1 byte `C3`** |
| 12 | +0x30 | `SlowDown()` | Play vt[8] = `0x1000B450` | **🔴 HÀM RỖNG** |
| 13 | +0x34 | `GetStatus()` | nội bộ | có khoá CS |
| 14 | +0x38 | `SetParam(int nID,int nValue)` | REC vt[4] | |
| 15 | +0x3C | `SetDrawInterface(void*)` | ghi `[0x100131B8]` | **= Represent shell** |
| 16 | +0x40 | `SetSoundInterface(void*)` | ghi `[0x100131BC]` | bỏ qua giá trị NULL |
| 17 | +0x44 | `Release()` | | |
| 18 | +0x48 | `SetStatus(int)` | | chặn `>5` |

**🔴 Hai nút "Nhanh"/"Chậm" trên thanh phát lại của bản tham chiếu KHÔNG LÀM GÌ.** Trong DLL không hề có biến tốc độ; `+0xAD0` chỉ là cờ điều khiển `{0=chạy, 1=tạm dừng, 2=xin dừng}`. Nếu port đường A thì phải báo trước hoặc tự viết lại phần tua.

### 2.3 Máy trạng thái

`m_dwStatus` ∈ `{0=RẢNH, 1=ĐANG GHI, 2=YÊU CẦU KẾT THÚC GHI, 3=ĐANG NẠP để phát, 4=ĐANG PHÁT, 5=TẠM DỪNG GHI}`.
`Rec()` chỉ mã hoá khi `status==1`; `StartRec`/`Play` chỉ chạy khi `status==0`; `SetParam(1,…)`/`SetParam(12,…)` bắt buộc `status==0`.

### 2.4 Định dạng tệp `.jxr`

```
TỆP .jxr TRÊN ĐĨA  =  MỘT LUỒNG zlib deflate DUY NHẤT, không có tiêu đề thô
   ├ byte đầu tệp = 0x78 0x9C  (KHÔNG phải chữ "JXR")
   ├ nén theo khối: đọc 0x8000 B từ tệp tạm → deflate(flush = Z_SYNC_FLUSH) → ghi khối 0x1000 B
   └ 🔴 KHÔNG BAO GIỜ gọi Z_FINISH ⇒ luồng không đạt Z_STREAM_END
        → zlib.decompress() trong Python sẽ ném "Error -5 incomplete or truncated stream"
        → phải dùng zlib.decompressobj().decompress(data)

SAU KHI GIẢI NÉN, offset 0 là JXRHEADER (version 14 = 0x450 B; version ≤13 = 0x332 B):
   +0x00  char  szMagic[4] = "JXR\0"
   +0x04  BYTE  byVersion  = 0x0E
   +0x05  char  szName[0x20]        ← 🔴 strcpy KHÔNG giới hạn, tên ≥32 ký tự đè các trường sau
   +0x25  DWORD dwParam             (tham số 2 của StartRec — chưa xác định ý nghĩa)
   +0x29  BYTE  nFrameCount         ← 🔴 chỉ 1 byte
   +0x2A  DWORD dwOffsetTableA
   +0x12E DWORD dwOffsetTableB
   +0x132 DWORD adwFrameOffset[]    ← 🔴 chỉ đủ ~198 mục; từ khung 199 TRÀN header,
                                       từ khung 214 đè lên BẢNG TRA TÀI NGUYÊN

THÂN TỆP (sau header), mỗi KHUNG:
   DWORD  số nhịp trì hoãn
   rồi các BẢN GHI:  WORD | BYTE | WORD(độ dài) | <dữ liệu>
```

**Tệp tạm bắt buộc:** `<thư mục chứa EXE>\UserData\Temp\<YYYYMMDDHHMMSS>.jxr`, mở `fopen(...,"w+b")`.
🔴 **CẢ chiều ghi LẪN chiều phát đều tạo tệp tạm này.** Thiếu thư mục ⇒ `fopen` hỏng ⇒ **cả hai chiều thất bại IM LẶNG** (không báo lỗi). Đây là điểm chết đầu tiên phải kiểm khi khôi phục.

**Chuỗi `"1.1.4"`** không phải phiên bản định dạng — nó là `ZLIB_VERSION` truyền cho `deflateInit_`/`inflateInit_`.
⚠️ Đính chính một hiểu nhầm dễ mắc: thay zlib 1.2.x **KHÔNG** gây `Z_VERSION_ERROR` (zlib chỉ so `version[0]` và `stream_size=0x38`, cả hai đều khớp). **Rủi ro thật là LỆCH ORDINAL** — DLL này import theo số hiệu `#4 deflate, #6 deflateEnd, #8 deflateInit_, #19 inflate, #20 inflateEnd, #22 inflateInit_`, tức phải chép đúng `zlib.dll` đi kèm (1.114.172 B), không thay bằng `zlib1.dll` chuẩn.

### 2.5 Giao thức ghi — `Rec(nTime, nType, fmt, ...)`

`nType` nhận 1..25 nhưng **chỉ 12 loại có bộ mã hoá thật** trên đường ghi: `1, 11, 12, 13, 14, 17, 20, 21, 22, 23, 24, 25`.
13 loại còn lại (`2,3,4,5,6,7,8,9,10,15,16,18,19`) **bị bỏ qua im lặng**. Loại `4` chỉ tồn tại ở đường phát (trạng thái camera).

Chuỗi định dạng dùng **chữ cái đơn** để chọn trường; chữ lạ bị bỏ qua và **không tiêu thụ tham số**:

| nType | Bộ mã hoá | Chữ cái nhận |
|---|---|---|
| 1 | `0x10006D90` | `h n p w` |
| 11 | `0x10006BF0` | `b n p u` |
| 12 | `0x10006A30` | `i m n p s u` |
| 13 | `0x10006860` | `c l n p s` |
| 14 | `0x10006650` | `b c l n o p x y z` |
| 21 | `0x10007080` | `l p x y` |

Ký tự là mnemonic: `x,y`=toạ độ · `w,h`=rộng/cao · `c`=màu · `z`=lớp · `n`=số/ID · `p`=con trỏ tài nguyên (được nội suy thành WORD id qua bảng tại `K2DREC+0x498`).
🔴 `nTime` **bị lượng tử hoá về bội số 3** ngay trong bộ mã hoá — độ phân giải thời gian của định dạng là 3 đơn vị, không phải 1.

### 2.6 Mô hình luồng

Mỗi chiều một luồng nền: **ghi** = `0x10007910` (handle `[0x100131C4]`), **phát** = `0x10009020` (handle `[0x100131D0]`). Nén/giải nén chạy trên luồng nền, không chặn luồng game. Đồng bộ bằng hai bộ đệm đảo + `CRITICAL_SECTION`; bảng tài nguyên xả mỗi **3 khung**.
🔴 **Phải gọi `EndRec`/`Release` trước khi thoát game**, nếu không luồng nền bị giết giữa lúc nén → `.jxr` hỏng.

---

## 3. BẢN THAM CHIẾU MÓC TÍNH NĂNG VÀO GAME NHƯ THẾ NÀO — SƠ ĐỒ

VA = `0x401000` + file-offset của `game_y_unpacked.bin`.

```
 ┌── LỐI VÀO ────────────────────────────────────────────────────────────────┐
 │  Ctrl+R / Ctrl+S / Ctrl+P                                                 │
 │      ui\autoexec.lua:32-34  AddCommand("Ctrl+R","","Replay([[rec]])")     │
 │                                                                           │
 │  Nút toolbar [Rec]  ClassType=Player_Recorder   (vftable 0x0069A0BC)      │
 │      → 0x00428DB0 ExecuteScript("Replay([[rec]])")                        │
 │      → 0x00428E30 lParam==0 → "Replay([[pauserec]])" ; ==1 → "…endrec…"   │
 │      🔴 ĐÃ BỊ CHÚ THÍCH: ui3_1024\工具控制条.ini:205-217                    │
 │                                                                           │
 │  Nút [OpenRep] màn hình đăng nhập → hộp thoại chọn tệp *.jxr             │
 │      🔴 ĐÃ BỊ CHÚ THÍCH: ui3_1024\UiNewLogin\开始.ini:41-52                │
 └───────────────────────────┬───────────────────────────────────────────────┘
                             ▼
   GameScriptFuns[19]  @0x006E45C8/0x006E45CC   {"Replay", LuaReplay}
   (bảng 150 mục @0x006E4530, đăng ký ở 0x004186C0 và 0x004EE0B4)
                             ▼
   LuaReplay  0x0040FF30
      ├ tra 8 động từ @0x006E4450 bằng strcmp (helper 0x0040FE90)
      │    rec  endrec  play  stop  pause  speedup  slowdown  pauserec
      ├ cmp idx,7 / ja  →  bảng nhảy 8 mục @0x004103C8
      └ kiểm trạng thái g_nReplayState [0x0075B350] trước mỗi động từ
                             ▼
   ┌──────────────────────────────────────────────────────────────────────┐
   │ rec       → vtbl+0x04   chỉ khi state 0 hoặc 5                       │
   │             (trước đó kiểm đĩa ≥100 MB tại 0x0040FEE0;               │
   │              thiếu chỗ → Setting.ini [InfoString] 23=tiêu đề,        │
   │              43="Ổ đĩa đã đầy không thể lưu thêm!")                   │
   │ endrec    → vtbl+0x0C   chỉ khi state 1  ← nơi ghép tên tệp .jxr     │
   │ pauserec  → vtbl+0x08   chỉ khi state 1                              │
   │ stop      → vtbl+0x20 ┐                                              │
   │ pause     → vtbl+0x24 │ chỉ khi state 4                              │
   │ speedup   → vtbl+0x2C │ (hai cái này là hàm rỗng trong DLL)          │
   │ slowdown  → vtbl+0x30 ┘                                              │
   │ play      → 0x004103B7  🔴 MÃ CHẾT, KHÔNG LÀM GÌ                     │
   └──────────────────────────────────────────────────────────────────────┘
                             ▼
   g_pReplay  [0x0075F334]  ← IJXReplay*
                             ▲
   ┌─────────────────────────┴────────────────────────────────────────────┐
   │ KHỞI TẠO LÚC BẬT GAME (không lazy):                                  │
   │   KMyApp::GameInit 0x0056D6A0 → InitRepresent 0x0056D520             │
   │        → 0x0056D615 gọi 0x0056D4A0                                   │
   │             LoadLibraryA("JXReplay.dll")   → HMODULE [0x0075B354]    │
   │             GetProcAddress("CreateJxReplayInterface")                │
   │             → g_pReplay [0x0075F334]                                 │
   │        → 0x0056D630  shell->vtbl+0x88(g_pReplay)      (gắn 2 chiều)  │
   │        → 0x0056D653  g_pReplay->vtbl+0x3C(shell)      SetDrawInterface│
   │        → 0x0056D66C  shell->vtbl+0x80(1, 0)                          │
   │ GỠ: 0x0056DF5E-0x0056DFA3 (vtbl+0x3C(0), +0x40(0), +0x44, g_pReplay=0)│
   │ 🔴 LỖI GỐC: FreeLibrary tại 0x0056E0B4 xong lại ghi 0 vào 0x0075B348 │
   │    (handle của Represent) thay vì 0x0075B354 → rò handle             │
   └──────────────────────────────────────────────────────────────────────┘
                             ▼
   jxreplay.dll  —  K2DREC ghi ra .jxr  /  K2DPlay đọc .jxr rồi
                    GỌI NGƯỢC vào Represent shell qua [0x100131B8]

   NHỊP PHÁT LẠI:
     mỗi khung  0x0056E3BD  g_nReplayState = g_pReplay->vtbl+0x34()
        state==4 → vòng lặp phát 0x0056E370 (thay cho vòng game 0x0056E180)
        KUiJxrPlayer (vftable 0x006A3C3C, thực thể toàn cục 0x00758C10)
          tự bật ở 0x0052E110/0x0052E190 khi state==4
          nạp ini "录像操作界面.ini"; nhịp ở vtable slot 15 (0x0052D590),
          dùng timeGetTime, chu kỳ lưu ở +0x1670, gọi vtbl+0x14 mỗi khung
```

### 3.1 Điểm quan trọng nhất — dữ liệu ghi vào `.jxr` là gì

Soát cuối đã ghép được mảnh còn thiếu bằng byte tại `VA 0x0056D630`:

```
mov eax,[edx+0x88] ; call eax          ← shell->vtbl+0x88(g_pReplay)
mov ecx,[0x0075B33C] ; push ecx         ← Represent shell
mov edx,[0x0075F334] ; mov eax,[edx]    ← g_pReplay->vtable
mov edx,[eax+0x3C]  ; call edx          ← SetDrawInterface(shell)
```

⇒ **"Giao diện vẽ" mà `jxreplay.dll` ghi lại CHÍNH LÀ `iRepresentShell`.** DLL không ghi ảnh, cũng không ghi "lệnh vẽ 2D" trừu tượng — nó ghi **chính xác các lời gọi phương thức của Represent shell**. Nghĩa là ta biết đúng 9 điểm phải chèn `Rec()` nếu đi đường A:

| Offset callback | nType | Suy ra là phương thức | Độ chắc |
|---|---|---|---|
| +0x10 | 14 | `OutputText` | suy luận |
| +0x14 | 13 | `OutputRichText` | suy luận |
| +0x20 | 1 | `CreateImage` (`w h n p`) | suy luận |
| +0x48 | 11 | `DrawPrimitives` — **dòng chính** | suy luận |
| +0x4C | 12 | `DrawPrimitivesOnImage` | suy luận |
| +0x50 | 17 | `ClearImageData` | suy luận |
| +0x54 | 4 | `LookAt` (chỉ đường phát) | suy luận |
| +0x68 | 21 | `SetLightInfo(nX,nY,pLighting)` | **gần như chắc** |
| +0x6C | 20 | `SetOption` | suy luận |
| ÂM THANH +0x08/+0x0C/+0x1C/+0x20 | 22/23/24/25 | — | |

Neo quyết định cho `+0x68`: ô đó trong `represent2.dll` tham chiếu là `0x10004670`, byte tại đó là `C2 0C 00` = `ret 0xC` ⇒ **hàm rỗng nhận đúng 3 tham số**; phía ta `iRepresentShell.h:359-366` có `SetLightInfo(int nX, int nY, unsigned int* pLighting)` — Represent2 là 2D nên thân hàm rỗng; và bảng chữ cái của nType 21 đúng là `{l, p, x, y}` khớp 1-1 với 3 tham số.

### 3.2 🔴 Rào cản ABI của đường A

- Represent shell **bản tham chiếu có 38 ô vtable**; **của dự án ta có 37** (`iRepresentShell.h` dòng 65..393). **Không tương thích ABI.**
- Bản tham chiếu có 3 ô setter riêng cho replay: `+0x80` (2 tham số, ghi cả `+0x11C` và `+0x120`), `+0x84` (ghi `+0x11C`), `+0x88` (ghi `+0x120`). Phía ta `+0x80` = `SetGamma`, `+0x84` = `SetAdjustColorList`, `+0x88` = `setZoomFactor(float)`.
- Chênh lệch 1 ô do phía ta có thêm `OutputVNText` (`iRepresentShell.h:127`) — **mọi offset từ đó trở đi lệch −4**.

⇒ **Đường A KHÔNG PHẢI "chỉ chép DLL vào".** Bắt buộc phải viết một **lớp adapter** có đúng thứ tự vtable của bản tham chiếu rồi truyền cho `SetDrawInterface`. Truyền thẳng `KRepresentShell2` sẽ gọi nhầm hàm.

### 3.3 Bẫy kỹ thuật khi đọc `game_y_unpacked.bin`

Mọi lệnh `CALL`/`JMP` trực tiếp trong bản dump đã bị **mã hoá**: `E8/E9 46 b1 b2 b3`, rel32 đọc thô là **rác**.
Công thức giải: **`đích_VA = 0x401004 + (b1<<16 | b2<<8 | b3)`** — ba byte là file-offset của đích trừ 4, ghi **big-endian**, marker là `0x46`.
Thống kê: 40.084/53.389 lệnh `E8` và 10.264/11.902 lệnh `E9` mang marker; 98,7% giải ra byte mở đầu lệnh hợp lệ.
⚠️ Có **đúng 10 lệnh `E8` dùng rel32 THƯỜNG** (tại `0x0056C02F, 0x0056C1EB, 0x0056D594, 0x0056D5DC, 0x0056D687, 0x0056D77A, 0x0056D968, 0x0056DB31, 0x0056DC22, 0x0056DCC1`) — hai trong số đó nằm ngay trong `InitRepresent`. **Tìm xref chỉ bằng mẫu `E8 46` sẽ bỏ sót người gọi.**

### 3.4 Tài nguyên & văn bản của bản tham chiếu

- `ui\ui3_1024\录像操作界面.ini` (858 B) — thanh phát lại: `[Main]` + 6 nút `Pause/GoOn/SpeedUp/SlowDown/Stop/Open`, tooltip Việt: Dừng / Phát / Nhanh / Chậm / Thoát ra / Mở ra. Bản `ui3_800` = 966 B (chênh 108 B do 2 dòng chú thích đầu tệp — **không "y hệt"**, đừng so checksum).
- `ui\ui3_1024\录像回放系统.ini` (1.729 B) — **tài liệu hướng dẫn tiếng Việt do chính nhà phát triển viết**, 19 chuỗi đánh số 0..18. Đáng đọc trước khi port. Dòng khoá `18`: *"Hiện tại chưa thể phát dưới dạng 3D nhưng có thể thu dưới dạng 3D"* — xác nhận độc lập việc chặn 3D ở nút `[OpenRep]`.
- 11 khoá chuỗi trong `gateway\lang\vn\stringtable_client.txt` (dòng 340-343, 376-382): `G_STR_RECORDER="Bắt đầu ghi (CTRL+R)"`, `G_STR_PAUSE_REC="Tạm ngưng (CTRL+P)"`, `G_STR_SAVE_REC="Lưu (CTRL+S)"`, `G_STR_JXREPLAY_FILE="Ghi hình ảnh!"`, `G_STR_REC_START="Hiện đang quay phim"`, `G_STR_REC_PAUSE="Tạm dừng ghi hình"`, `G_STR_SAVE_REPFILE="File ảnh đã được lưu tại"`…
  ⚠️ `G_STR_SAVE_SCREENFILE` **KHÔNG thuộc hệ replay** — nó chỉ có 1 nơi dùng (`0x0042B905`) và thuộc chức năng **chụp màn hình** (`PrintScreen`). Đừng gộp nhầm.
- 🔴 **Không có tệp `.spr` nào của thanh phát lại trong `D:\ServerLinux`** — chỉ có 4 tệp `.ini`. Nhưng **pak của chính dự án ta lại có đủ** (mục 6).

---

## 4. DỰ ÁN TA — ĐƯỜNG BANDICAM (`KUiCapture`)

### 4.1 Sơ đồ hiện trạng

```
 config.ini [Client] RecPath=.\JxRelay   CapPath=.\JxCap
        │
        ▼  S3Client.cpp:527
 KUiCapture::SetRecPath()  → m_zRecPath   (UiCapture.cpp:57-68)   🔴 2 lỗi, mục 9

 ── LỐI VÀO ────────────────────────────────────────────────────────────────
 (a) Nút toolbar [Rec]  ClassType=Player_Rec   ✅ LỚP & MÃ C++ CÒN SỐNG
        UiShell.cpp:201  Player_Rec::RegisterSelfClass()
        UiShell.cpp:1002-1005  OnButtonClick → KUiCapture::OpenWindow/CloseWindow
        UiShell.cpp:1017  CheckButton(GetIfVisible()!=NULL)
        🔴 NHƯNG: Ui\Ui3\UiToolsControlBar.ini (tệp THỰC SỰ nạp) KHÔNG có [Rec];
           [Main] chỉ liệt kê Button0..Button4 = Run/Sit/Horse/Exchange/PK.
           Mục [Rec] chỉ nằm ở UiToolsControlBar0.ini:39-50 và _D.ini:200-211
           — HAI TỆP KHÔNG ĐƯỢC NẠP.
 (b) Phím Ctrl+R  → autoexec.lua:47  Open([[rec]])
        ShortcutKey.cpp:167  l_WindowList[24] = "rec"
        🔴 ShortcutKey.cpp:329-336  case 24 BỊ CHÚ THÍCH TOÀN BỘ
        🔴 ShortcutKey.cpp:46  //#include "UiCase/UiCapture.h"
        🔴 ShortcutKey.h:37  SCK_SHORTCUT_REC = "Switch([[rec]])"  ← SAI ĐỘNG TỪ
           (bảng l_StatusList 11 mục không có "rec" ⇒ tooltip không hiện phím)
 ── VÒNG LẶP CHÍNH ────────────────────────────────────────────────────────
 S3Client.cpp:1253  kct = KUiCapture::GetUiCapture()    ← tạo đối tượng, KHÔNG Show()
 S3Client.cpp:1254-1261  nếu IsCapturing() → Work(NULL) + GetCaptureTime()
                          → SetRecTimmer("[ mm:ss:mmm ]")
 ── KHI BẤM Rec_btn ───────────────────────────────────────────────────────
 UiCapture.cpp:125 StartCapture()
   ├ đang quay?  → Stop() + Destroy() + CloseWindow() + báo "Video đã lưu %s"
   └ chưa quay:
        :150  Create(BANDICAP_RELEASE_DLL_FILE_NAME)
                 bandicap.h:298  LoadLibrary("bdcap32.dll")      ← thử TRƯỚC
                 bandicap.h:301  LoadLibrary("jxreplay32.dll")   ← rồi mới thử
                 bandicap.h:304  ASSERT(0) → BCERR_LOAD_LIBRARY_FAIL
        :156  Verify("MAIET-GUNZ-20080916","d25f910a")   ← khoá của GunZ/MAIET
        :165  BCapConfigPreset(BCAP_PRESET_DEFAULT)  → F_AVI + MPEG-1 + MP2 48kHz
        :169  SetMinMaxFPS(30,60)
        :173  MakePathnameByDate(m_zRecPath,"Capture","mp4",…)
        :176  Start(pathName, NULL, BCAP_MODE_GDI, (LONG_PTR)hWnd)
 ── KẾT QUẢ ───────────────────────────────────────────────────────────────
 🔴 bin\client KHÔNG có bdcap32.dll lẫn jxreplay32.dll  → Create() hỏng
 🔴 Release KHÔNG định nghĩa NDEBUG (S3Client.vcxproj:139), Game.exe THẬT SỰ
    import _wassert  →  hộp thoại "Assertion failed" lộ đường dẫn D:\GAMEDEVNEW
    rồi abort — KHÔNG phải thất bại im lặng
 🔴 JxRelay\ RỖNG — chưa từng sinh ra tệp video nào
```

### 4.2 Những điều đã chốt được

- **Lớp đã được biên dịch vào `Game.exe` đang phát hành.** RTTI: `.?AUIBandiCapture@@` @`0x0011DBDC`, `.?AVCBandiCaptureLibrary@@` @`0x0011DBF8`, `.?AVKUiCapture@@` @`0x0011DC1C`, `.?AVPlayer_Rec@@` @`0x0011EC50`. Chuỗi `bdcap32.dll` @`0x000F6670`, `jxreplay32.dll` @`0x000F667C`, `CreateBandiCapture` @`0x000F668C`, `UiCapture.ini` @`0x000F6740`. **Chỉ có lớp BỌC được biên dịch vào exe** — toàn bộ bộ mã hoá video vẫn nằm trong DLL đang thiếu.
- **Cửa sổ `KUiCapture` KHÔNG hiển thị.** `KUiCapture : protected KWndShowAnimate`; `WndShowAnimate.cpp:43` có `m_Style &= ~WND_S_VISIBLE` trong `Init`; cờ chỉ bật lại bởi `Show()`, mà `GetUiCapture()` (đường duy nhất đang chạy) có `//m_pSelf->Show();` bị chú thích (`UiCapture.cpp:86`). ⇒ `GetIfVisible()` **luôn trả NULL**, `Player_Rec::UpdateData` luôn `CheckButton(FALSE)`.
- **`CreateBandiCapture` chỉ đòi `(SDKVersion & 0xFFFFFF00) == 0x02030000`.** Byte tại file-offset `0x00675E10` của `bdcap32.dll`: `55 8B EC 8B 45 08 25 00 FF FF FF 3D 00 00 03 02 74 09 B8 30 10 77 87 5D C2 08 00`. `bandicap.h:42` khai `BCAP_VERSION = 2.3.0.230` = `0x020300E6` ⇒ **KHỚP**. Chú thích lo lắng ở `UiCapture.cpp:149` là báo động giả.
- **Toàn bộ vtable `IBandiCapture` trong DLL khớp 20/20 ô đúng thứ tự khai báo trong header** (bảng tại file-offset `0x00ABC528`). Bằng chứng tương thích mạnh hơn nhiều so với một phép kiểm phiên bản đơn lẻ.
- **`Create(szDllPathName)` BỎ QUA hoàn toàn tham số của chính nó** — luôn nạp cứng `"bdcap32.dll"` rồi `"jxreplay32.dll"`. Muốn đổi tên DLL phải sửa `bandicap.h`, không sửa chỗ gọi. Vì dùng tên trần nên **DLL phải nằm cạnh `Game.exe`** (không phải thư mục làm việc).
- **`bdcap32.dll` 2.5 có CẢ HAI muxer**: chuỗi `CBandiMuxerAvi` @`0x00AA6F60` và `CBandiMuxerMp4` @`0x00AA6F98` (kèm bảng box ISOBMFF `ftyp/isom/mp41/mdat/moov/…`). ⇒ **Chưa kết luận được** tệp `.mp4` sinh ra thực chất là AVI hay MP4 — phải chạy thử rồi đọc 4 byte đầu.
- **Tài nguyên đủ**: `Ui\Ui3\UiCapture.ini` (831 B, 6 mục) + 6 SPR `Spr\Ui3\UiCapture\{main, main1, rec_btn, pause_btn, stop_btn, close_btn}.spr`. Bố cục có lỗi nhỏ: `TimeRec_txt` tràn 60px ra ngoài khung 170px; `Rec_btn`/`Pause_btn` chồng 3px; `Stop_btn` trùng vị trí `Pause_btn`.
- **`Game.pdb` cạnh `Game.exe` là PDB CŨ, không khớp.** Debug Directory của `Game.exe` trỏ về `D:\GAMEDEVNEW\Sources\S3Client\Release\S3Client.pdb`, GUID canonical `f731ac0c-d02f-4542-bcfe-d456908c8add`, age 27, TimeDateStamp `0x6A86DBA8` = 2026-08-20 10:49:12 UTC. **Mọi đối chiếu phải dùng PDB của cây D:.**
- ⚠️ Post-build chép `Game.exe` vào **cả** `bin\client\` lẫn `bin\client\release\`. Trên máy này bản ở `release\` cũ hơn 1 ngày (1.236.992 B, 19/08 19:11) còn bản đang chạy là 1.252.352 B (20/08 03:49). **Nói rõ dùng bản nào khi dịch ngược.**

---

## 5. 🔴 NÚT THẮT THẬT SỰ: KHÂU LẤY KHUNG HÌNH ĐANG HỎNG

### 5.1 Hiện tượng đo được

52/52 tệp `.jpg` trong `bin\client\JxCap`: cùng **12.921 byte**, cùng **md5 `8ed1a7366371abac3db620687dbcfa95`**, trải từ `2026-01-05_22-27-39.jpg` đến `2026-08-19_17-48-50.jpg` (8 tháng, gồm cả ngày làm việc gần nhất). Giải mã: JFIF, SOF0 `W=1024 H=768`, 3 thành phần; `getcolors` trả về **đúng 1 màu** phủ toàn bộ 786.432 điểm ảnh: **RGB(0, 56, 247)**.

### 5.2 Chốt nguyên nhân ở mức số học

`KRepresentShell2.cpp:1305-1315` chuyển RGB565 → BGR:
```
pDes[2] = (v & 0xF800) >> 8 ;  pDes[1] = (v & 0x07E0) >> 3 ;  pDes[0] = (v & 0x001F) << 3
```
Với `v = 0x01DF`: R = 0, G = 14<<2 = **56**, B = 31<<3 = **248**.
Ảnh đo được RGB(0,56,**247**) — lệch 1 ở kênh B đúng bằng sai số làm tròn JPEG (B luôn là bội số của 8 nên 247 không thể sinh trực tiếp).
⇒ **Giá trị điểm ảnh là hằng số `0x01DF` trên toàn khung.** Đường đọc: `KRepresentShell2.cpp:1290` `m_DirectDraw.LockPrimaryBuffer()`.

**Giả thuyết nguyên nhân gốc** (chưa gỡ lỗi trên máy thật): trên Win10/11 primary surface của DirectDraw bị DWM chuyển hướng, `Lock` đọc ra bộ đệm mô phỏng không chứa nội dung desktop.

### 5.3 🟢 Nguồn khung hình ĐÚNG — vẫn còn sống

```
KRepresentShell2::RepresentBegin  :1151-1157  →  m_Canvas.FillCanvas()
   … mọi thứ được vẽ qua shell vào m_Canvas …
KRepresentShell2::RepresentEnd    :1161-1165  →  m_Canvas.Changed(true); m_Canvas.UpdateScreen()

m_Canvas.m_pSurface  =  g_pDirectDraw->CreateSurface(w,h)      KCanvas.cpp:77
     KDDraw.cpp:370-382  ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY   ← 🟢 BỘ NHỚ HỆ THỐNG
     ⇒ Lock đọc được 100%, không phụ thuộc DWM

Truy cập:  void* KCanvas::LockCanvas(int& nPitch)   KCanvas.h:91 / KCanvas.cpp:1347-1361
           void  KCanvas::UnlockCanvas()             KCanvas.cpp:1363
Đã xuất khẩu sẵn từ engine.dll:
           ordinal  846  ?LockCanvas@KCanvas@@QAEPAXAAH@Z    rva 0x00016170
           ordinal 1121  ?UnlockCanvas@KCanvas@@QAEXXZ       rva 0x00016240
⇒ CÓ THỂ GỌI THẲNG TỪ S3Client, tránh được việc phá ABI của iRepresentShell.
```

🔴 **Cạm bẫy:** `KRepresentShell2.cpp:1173-1231` có một bản `SaveScreenToFile` **thứ hai đã bị chú thích**, đọc thẳng `m_Canvas.m_pCanvas` (dòng 1198). **KHÔNG được bỏ chú thích nó** — biến `m_pCanvas` đã bị xoá (`KCanvas.h:59-60` cả hai dòng đều bị chú thích, thay bằng `:54 LPDIRECTDRAWSURFACE m_pSurface`). Bỏ chú thích = **không biên dịch được**. Phải viết lại bằng `LockCanvas()`.

Nhưng chính khối chú thích đó là bằng chứng: **người ta đã ĐỔI từ canvas sang primary surface** — và đó là lúc ảnh chụp hỏng.

---

## 6. KIỂM KÊ TÀI NGUYÊN — BẢNG DỨT KHOÁT

Đã dựng lại độc lập bộ đọc chỉ mục pak (header 32 B `PACK`, mục 16 B) + hàm băm `FileNameToId` (mô phỏng `char` **có dấu** của MSVC), quét cả 37 pak. Hai lượt độc lập cho ra **cùng một** id/offset/size.

| Hạng mục | Bản tham chiếu | Dự án ta | Trạng thái |
|---|---|---|---|
| `jxreplay.dll` (hệ .jxr) | ✅ 90.112 B | ❌ | Chép từ `Patch\` hoặc `C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky 2.0\` (95.608 B, 2025 — **cùng định dạng**, chỉ khác build) |
| `zlib.dll` (import theo ordinal) | ✅ 1.114.172 B | ❌ | **Bắt buộc chép kèm**, không thay `zlib1.dll` |
| `msvcp80.dll` / `msvcr80.dll` | ✅ | ❌ (chỉ có `MSVCP60D`/`MSVCRTD`) | jxreplay.dll cần CRT của VC++2005 |
| `bin\client\UserData\Temp\` | — | ❌ | **Bắt buộc tồn tại cho CẢ ghi lẫn phát** |
| `bdcap32.dll` 2.5.0.250 (hệ video) | — | ✅ **có 6 bản trên ổ E** | Chỉ cần chép, **giữ nguyên tên** |
| `.ini` thanh phát lại `.jxr` | ✅ rời `ui3_1024`/`ui3_800` | ✅ **`\ui\ui3\录像操作界面.ini` id=`763616A0` trong `update01.pak` off=147995608, size=706** | 🟢 đúng thư mục chủ đề `ui3` của ta |
| `.ini` tài liệu hướng dẫn | ✅ 1.729 B | ✅ **`\ui\ui3\录像回放系统.ini` id=`9FB26F2D`** (`update01`/`update03`/`vltkcache`) | 🟢 |
| 7 SPR thanh phát lại | ❌ **không có tệp .spr nào** | ✅ **`\spr\ui3\录像\播放操作\*.spr` trong `update01.pak`** (`154F878C`, `53DAB5C2`, `39C9F928`, `524B6257`, `B8AFF470`, `4BBEFE6C`, `32DE19A1`) | 🟢 pak ta ĐỦ, bản tham chiếu THIẾU |
| SPR nút Rec toolbar | ✅ khai trong ini | ✅ `\spr\ui3\主界面\按钮条按钮\录像按钮.spr` id=`74DBE071`, `update01.pak` off=117664156 size=2168 | 🟢 (không có bản rời, nhưng pak có) |
| `UiCapture.ini` + 6 SPR | — | ✅ rời + trong pak | 🟢 |
| Chuỗi tiếng Việt hệ `.jxr` | ✅ 11 khoá `stringtable_client.txt` | ✅ **`Ui\Setting.ini` `[InfoString]` dòng 41/42 đã Việt hoá sẵn** | dấu vết cho thấy dòng client này TỪNG chạy hệ replay |

**Nợ cấu hình phát hiện kèm** (không chặn tính năng, nhưng ghi nhận): `package.ini` khai **4** pak không có trên đĩa (`sprvlngaothe2.pak`, `settings.pak`, `ui.pak`, `script.pak`) và có **5** pak trên đĩa không được khai (`sprgame.pak`, `vlngaothe1.pak`, `vltkcache.pak`, `serverlistfree.pak`, `update05.pak`). ⚠️ Nếu định lấy `.ini` replay từ `vltkcache.pak` thì **pak đó đang không được nạp**.

**Luật ưu tiên đọc tệp**: `KPakFile.cpp:18` `m_nPakFileMode = 0` = **đọc đĩa TRƯỚC, pak sau** (`KPakFile.cpp:241-269`), và `g_SetPakFileMode` **không có nơi gọi nào** trong toàn cây. ⇒ Thả tệp rời là đè được pak, không cần đóng lại pak.

---

## 7. MÁY CHỦ & ENGINE

### 7.1 Máy chủ — sạch 100%

Quét 73.072 chuỗi trong `D:\ServerLinux\server1\jx_linux_y` (8.931.808 B): **0 kết quả** cho `replay` / `jxreplay` / `.jxr` / `bandicam` / `screenrecord`. 16 chuỗi khớp regex rộng đều là CSDL/nhật ký (`UpdateSDBRecord`, `TONG_ApplyAddEventRecord`, `KTongRecordList`, `LoadRecord ItemFile`, các thông điệp pattern-matching của Lua…).
Hai module `.so` (`GameExtConnect.so` và `KG_Angel.so` **là cùng một tệp** — trùng md5 `2326fbc1…`; `GameExtContent.so` 3.746.592 B) cũng sạch; `RecordSpan` là ký hiệu runtime của Go.
`ScriptFuns.cpp` (13.890 dòng) **không đăng ký** bất kỳ hàm Lua record/replay/capture nào; **không tồn tại** ký hiệu `OPENWINDOW` nào trong Core.
Ở bản tham chiếu, `Replay()` cũng **thuần client** — gắn phím trong `Patch\ui\autoexec.lua`, không script máy chủ nào gọi.

### 7.2 Engine — có gì / thiếu gì nếu tự viết

| Thành phần | Khả năng | Kết luận |
|---|---|---|
| `KAviFile` (940 dòng) | **Chỉ ĐỌC** — 0 lệnh ghi, 0 phương thức Write; lưu trữ là `KPakFile` vốn không có `Write` | ❌ không dùng ghi hình được |
| `KMp4Video` | Chỉ giải mã DivX để phát; **thiếu `mp4lib.dll`** trong `bin\client` | ❌ |
| `KGifFile` | Khai báo `Save`/`write_*`/`lzw_encode` (`KGifFile.h:192-197,215) nhưng **KHÔNG CÀI ĐẶT dòng nào**; engine.dll cũng không xuất khẩu chúng | ❌ **bẫy**: nhìn header tưởng ghi được GIF |
| `JpgLib` | Chỉ `jpeg_decode_*` | ❌ |
| `KLVideo.dll` (278.615 B, có sẵn) | Bộ **PHÁT** video KLV cho phim mở đầu, 10 hàm export | ❌ (hệ video thứ ba, cũng chỉ phát) |
| **Lấy khung hình** | `KCanvas::LockCanvas` / `KDirectDraw::LockPrimary/BackBuffer` | 🟢 **CÓ ĐỦ** |
| **Nén ảnh tĩnh** | `RepresentUtility.cpp:185 SaveBufferToJpgFile24` (GDI+); `engine.dll` ord 998 `KBmpFile24::SaveBuffer24` | 🟢 CÓ |
| **Điểm móc mỗi khung** | `UiShell.cpp:341` ngay trước `g_pRepresentShell->RepresentEnd()`; nhịp `PaintFps` 30-60 đã tách khỏi `GAME_FPS 18` | 🟢 CÓ |
| **Đóng gói video** | Không module nào import `avifil32`/`vfw32`/`msacm32`/`mfplat` | ❌ **PHẢI TỰ VIẾT** (~200 dòng RIFF/AVI) |
| **Thu âm** | `engine.dll` import `DSOUND.dll` **chỉ ordinal #1** (`DirectSoundCreate`); `Game.exe` import WINMM chỉ có `timeBeginPeriod/timeEndPeriod/timeGetTime` — **không `waveIn*` nào** | ❌ không có đường thu |

🔴 **Đính chính đề bài:** `?SaveBuffer24@KBmpFile24@@SAHPADPAXHHH@Z` **KHÔNG phải export của `Represent2.dll`**. `Represent2.dll` chỉ export **duy nhất** `CreateRepresentShell` (rva `0x000174C0`). Hàm đó là export của **`engine.dll`** ordinal 998 rva `0x00014950`. **Hook sai module = hook trượt.**

🔴 **Bẫy `nPitch`:** nhánh 24-bit của `KBmpFile24::SaveBuffer` **BỎ QUA tham số `nPitch`** (`KBmpFile24.cpp:287` chỉ dùng ở nhánh 32-bit tại `:319`). Pitch của surface đã khoá thường **lớn hơn** `nWidth*3` ⇒ **phải nén hàng cho khít trước**, nếu không ảnh xiên/rác. Với bộ ghi 30-60 fps, bước này là chi phí phải tính.

---

## 8. NHỮNG CHỖ CẦN THAY ĐỔI

### BƯỚC 0 — BẮT BUỘC TRƯỚC MỌI ĐƯỜNG (~15 dòng, đồng thời là phép kiểm rẻ nhất)

**Sửa đường lấy khung hình.** Không làm thì mọi đường đều cho ra video xanh đặc giống 52 ảnh trong `JxCap`.

| # | Tệp:dòng | Việc |
|---|---|---|
| 0.1 | `Represent\Represent2\KRepresentShell2.cpp:1290` | `m_DirectDraw.LockPrimaryBuffer()` → `m_Canvas.LockCanvas(nPitch)` |
| 0.2 | `…:1296` | bỏ `GetScreenPitch()`, dùng `nPitch` trả về |
| 0.3 | `…:1323` | `UnLockPrimaryBuffer()` → `m_Canvas.UnlockCanvas()` |
| 0.4 | `…:1241-1272` | **Khối `WINDOWMODE` (`GetClientRect`/`ClientToScreen`/`nPicOffX/Y`) trở nên THỪA và SAI** — canvas là toạ độ cửa sổ, không phải toạ độ desktop. Dùng nhánh `else` (`:1276-1281`, `m_Canvas.GetWidth/GetHeight`) |
| 0.5 | `…:1173-1231` | **KHÔNG bỏ chú thích khối cũ** — nó dùng `m_pCanvas` đã bị xoá, sẽ không biên dịch được |

**Kiểm chứng:** bấm `PrintScreen` (`Ui\autoexec.lua:61`) → tệp mới trong `bin\client\JxCap` phải có nội dung game thay vì `RGB(0,56,247)`.
**Rủi ro:** phải build lại `Represent2.dll` → **nhớ luật trộn CRT** (`engine.dll` + `Represent2.dll` = `Debug|Win32` với `MSVCP140D`/`ucrtbased`; `Game.exe` = `Release`). Sai là sập `0xC0000374`.

Đây cũng là **sửa luôn một lỗi người chơi đang gặp**: bấm PrintScreen ra ảnh xanh trơn.

---

### ĐƯỜNG A — Khôi phục hệ replay `.jxr`

**Bản chất:** ghi lời gọi `iRepresentShell` + âm thanh, phát lại bằng chính engine. **Không ra video, không chia sẻ ra ngoài được.**

| # | Chỗ phải động | Ghi chú |
|---|---|---|
| A.1 | `bin\client\` — chép `jxreplay.dll`, `zlib.dll`, `msvcr80.dll`, `msvcp80.dll`, `microsoft.vc80.crt.manifest` | zlib import **theo ordinal**, không thay được |
| A.2 | `bin\client\UserData\Temp\` — **phải tồn tại** | thiếu = hỏng im lặng **cả hai chiều** |
| A.3 | `iRepresent\iRepresentShell.h` — thêm 1-2 hàm ảo (`SetReplay`/`GetReplay`) | **PHÁ ABI** → build lại `Represent2` + `Represent3` + `Game.exe` cùng lúc |
| A.4 | `Represent2\KRepresentShell2.{h,cpp}` — chèn `IJXReplay::Rec(...)` vào **9 hàm** (bảng mục 3.1) | |
| A.5 | **LỚP ADAPTER BẮT BUỘC** | vtable ta lệch 1 ô so với bản tham chiếu (do `OutputVNText`, `iRepresentShell.h:127`). **Tuyệt đối không truyền thẳng `KRepresentShell2` cho `SetDrawInterface`** |
| A.6 | `S3Client.cpp` — thêm khối nạp DLL trong luồng khởi tạo Represent | tương đương `KMyApp::InitRepresent 0x0056D520`; nhớ `SetParam(1,1)` và `SetParam(12,1)` **TRƯỚC** `StartRec` (bắt buộc `status==0`), nếu không thiếu ảnh chụp trạng thái ban đầu |
| A.7 | `Ui\ShortcutKey.cpp:2275-2340` — thêm `{"Replay", LuaReplay}` + hiện thực 8 động từ | ánh xạ vtable `+0x04/+0x0C/+0x08/–/+0x20/+0x24/+0x2C/+0x30` |
| A.8 | `Ui\autoexec.lua:47` đổi `Open([[rec]])` → `Replay([[rec]])`; dòng 48-54 đã có sẵn 7 dòng `Replay(...)` (hiện là lệnh chết). Bản sao `Ui\kethop.lua:27,30-36` sửa giống | |
| A.9 | Port lớp `KUiJxrPlayer` (`KWndPage`) | **tài nguyên đã có sẵn trong pak CỦA TA** (mục 6) |
| A.10 | Nút toolbar `Player_Recorder` + hộp thoại `GetOpenFileNameA` lọc `*.jxr`, thư mục `JxRep\`, tên `2D_%y-%m-%d_%H-%M.jxr` | |
| A.11 | Kiểm đĩa ≥100 MB trước khi ghi | thông báo lấy từ `[InfoString]` khoá **23 = tiêu đề**, **43 = nội dung** |
| A.12 | Giao diện âm thanh `SetSoundInterface` (+0x40) cho 4 loại 22/23/24/25 | 🔴 bộ giải mã kiểm con trỏ **VẼ** trước tiên (`0x1000C6A2`) ⇒ **không cài giao diện vẽ thì âm thanh cũng câm** |

**🔴 Rủi ro lớn nhất — ABI đóng cứng.** DLL 2020 không có mã nguồn; mọi offset vtable hai chiều đều cứng. Sai 1 ô = crash hoặc ghi rác không báo lỗi. Cộng thêm **3 khiếm khuyết KHÔNG SỬA ĐƯỢC**:
- `WriteHeader` dùng `lea` thay `mov` (`0x100097F4`, byte `8d 4c 24 10`) ⇒ header ghi lần đầu là **1104 byte RÁC** từ ngăn xếp. Phiên ghi bị ngắt giữa chừng = **mất trắng**, chỉ tệp đã `EndRec` hoàn tất mới đọc được.
- Bộ đếm chỉ số khung là **BYTE**: tràn header từ khung **199**, đè bảng tra tài nguyên từ khung **214**, cuộn vòng ở 256.
- `strcpy` tên phiên không giới hạn (`0x10009790`), trường chỉ 32 byte.

**Rủi ro thứ hai:** không có tệp `.jxr` mẫu nào trên máy (cả 3 mũi quét độc lập) ⇒ **không thể kiểm thử từng bước**, chỉ biết đúng/sai sau khi ghi thành công lần đầu.
**Rủi ro thứ ba:** `nTime` bị lượng tử hoá về bội số 3, `NextFrame` xả mỗi 3 khung; client ta đã đổi sang `PaintFps` 30-60 tách khỏi `GAME_FPS 18` — chưa rõ có lệch thời gian khi phát lại không.
**Phải báo trước với chủ dự án:** hai nút **"Nhanh"/"Chậm" sẽ không hoạt động** (hàm rỗng trong DLL).

---

### ĐƯỜNG B — Hoàn thiện hệ video BandiCam sẵn có

**Bản chất:** ra tệp video thật, chia sẻ được. Phụ thuộc thư viện thương mại bên thứ ba.

| # | Chỗ phải động | Ghi chú |
|---|---|---|
| B.1 | Chép `E:\SourceTuanLe\Source_G7VN\JX\swrod3\SwordOnline\Sources\S3Client\bdcap32.dll` (2.5.0.250) vào `bin\client\` | **GIỮ NGUYÊN TÊN** — `bandicap.h:298` thử `bdcap32.dll` trước. ⚠️ dùng tên trần nên phải nằm **cạnh `Game.exe`**; nếu máy người chơi đã cài Bandicam có thể nạp nhầm bản hệ thống |
| B.2 | `Ui\Ui3\UiToolsControlBar.ini` — thêm mục `[Rec]` (chép từ `UiToolsControlBar0.ini:39-50`) + `Button5=Rec` vào `[Main]` | 🟢 **KHÔNG cần build lại** (`KWndToolBar::Init` lặp `Button%d` → `CreateComObject(ClassType)`). ⚠️ **KHÔNG chép cả mục `[Main]` của tệp `0.ini`** — ảnh nền `\SPR\Ui3\1188.spr` MISS ở cả đĩa lẫn 37 pak |
| B.3 | `S3Client.vcxproj:139` — thêm `NDEBUG` vào Release, **hoặc** thay `ASSERT(0)` ở `bandicap.h:304` / `UiCapture.cpp:151,157` bằng xử lý lỗi thật | không làm thì người chơi thấy hộp thoại "Assertion failed" lộ đường dẫn máy dev |
| B.4 | `UiCapture.cpp:176` — `BCAP_MODE_GDI` + `hWnd`: **ĐIỂM PHẢI THỬ ĐẦU TIÊN** | nếu ra khung đen thì ứng viên là `BCAP_MODE_DDRAW7` (`bandicap.h:73`) và tham số 4 phải là con trỏ surface. Nhưng engine ta giữ `LPDIRECTDRAWSURFACE` **phiên bản 1** (`KDDraw.h:27`) ⇒ phải `QueryInterface(IID_IDirectDrawSurface7)` trước |
| B.5 | `Ui\ShortcutKey.h:37` — `"Switch([[rec]])"` → `"Open([[rec]])"` | **chỉ có một hướng sửa đúng**: `l_StatusList` (11 mục) không có `"rec"`, `FindStatus` trả −1, `switch` không có `case -1` ⇒ `Switch([[rec]])` là lệnh chết tuyệt đối |
| B.6 | `Ui\ShortcutKey.cpp:46` bỏ chú thích `#include`; `:329-336` bỏ chú thích `case 24` | **tuỳ chọn** — nút toolbar đã đủ để bấm thử |
| B.7 | 10 lỗi lập trình trong `KUiCapture` | xem mục 9 |
| B.8 | `Ui\autoexec.lua:48-54` + `Ui\kethop.lua:30-36` — xoá 7 dòng `Replay(...)` | lệnh chết: `Game.exe` không đăng ký hàm Lua `Replay` ⇒ bấm sẽ ném `attempt to call a nil value` vào `ScriptError.log` |
| B.9 | `Ui\Ui3\UiCapture.ini` — chỉnh toạ độ `[TimeRec_txt]`, `[Rec_btn]`/`[Pause_btn]`; cân nhắc bỏ `CheckBox=1` | `CheckBox=1` khiến nút **tự chốt xuống** dù chưa quay được gì → **che giấu thất bại** |

**🔴 Rủi ro lớn nhất:** **chưa ai thử `BCAP_MODE_GDI` trên client DirectDraw + Win11.** Đây là phép thử rẻ nhất (chép 1 tệp DLL + thêm 12 dòng `.ini`, **không cần build**) và nó **quyết định cả nhánh**.
**Rủi ro thứ hai — PHÁP LÝ:** đang nhúng khoá bản quyền của game khác — `Verify("MAIET-GUNZ-20080916","d25f910a")` (`UiCapture.cpp:156`, khoá của MAIET Entertainment cho GunZ: The Duel, cấp 16/09/2008) — và **cố ý đổi tên DLL thành `jxreplay32.dll` để nguỵ trang** (`bandicap.h:275`). Nếu phát hành thương mại thì phải mua giấy phép BCL hoặc bỏ nhánh này.
**Rủi ro thứ ba:** nếu ai đó bật lại `ANTI_DETECT_WINDOWS_TEXT` (`S3Config.h:25`, hiện `= 0`) thì danh sách đen **245 mục** sẽ đá người chơi ra khỏi game khi họ mở bất kỳ phần mềm nào có chữ `Record` trên tiêu đề — trong khi `Bandicam` và `OBS` **lại không có** trong danh sách.

---

### ĐƯỜNG C — Tự viết bộ ghi hình

**Bản chất:** ít phụ thuộc nhất, không dính bản quyền, kiểm soát hoàn toàn. **Sau Bước 0 thì đã có sẵn ~80% nguyên liệu.**

```
 ┌─ NGUỒN KHUNG HÌNH ────────────────────────────────────────────────────┐
 │ KCanvas::LockCanvas(int& nPitch)   Engine\Src\KCanvas.h:91            │
 │   surface = DDSCAPS_SYSTEMMEMORY (KDDraw.cpp:381) ⇒ đọc được 100%     │
 │   đã xuất khẩu: engine.dll ord 846 / 1121                             │
 │   ⇒ gọi THẲNG từ S3Client, TRÁNH được việc phá ABI iRepresentShell    │
 └───────────────────────────┬───────────────────────────────────────────┘
                             ▼
 ┌─ ĐIỂM MÓC ────────────────────────────────────────────────────────────┐
 │ UiShell.cpp:341 — chèn NGAY TRƯỚC g_pRepresentShell->RepresentEnd()   │
 │   (canvas đã vẽ xong, chưa Blt)                                       │
 │ Nhịp đã đồng bộ sẵn với PaintFps 30-60 (S3Client.cpp:500-511)         │
 └───────────────────────────┬───────────────────────────────────────────┘
                             ▼
 ┌─ CHUYỂN MÀU ──────────────────────────────────────────────────────────┐
 │ tái dùng vòng lặp KRepresentShell2.cpp:1305-1315                      │
 │ nhớ kiểm GetRGBBitMask16() == RGB_565 hay 555; thứ tự ghi là BGR      │
 └───────────────────────────┬───────────────────────────────────────────┘
                             ▼
 ┌─ NÉN ẢNH ─────────────────────────────────────────────────────────────┐
 │ RepresentUtility.cpp:185 SaveBufferToJpgFile24 (GDI+, đã có gdiplus)  │
 │   → MJPEG        ← con đường ÍT PHỤ THUỘC NHẤT                        │
 │ hoặc engine.dll ord 998 KBmpFile24::SaveBuffer24 (BMP thô)            │
 │ 🔴 BẪY: nhánh 24-bit BỎ QUA nPitch → phải nén hàng cho khít trước     │
 └───────────────────────────┬───────────────────────────────────────────┘
                             ▼
 ┌─ ĐÓNG GÓI ────────────────────────────────────────────────────────────┐
 │ PHẢI TỰ VIẾT bộ ghi RIFF/AVI (~200 dòng), FourCC 'MJPG'               │
 │   KAviFile = demuxer thuần (0 lệnh ghi)                               │
 │   KGifFile Save/lzw_encode = KHÔNG CÀI ĐẶT                            │
 │   KMp4Video = decode + thiếu mp4lib.dll ;  JpgLib = chỉ giải mã       │
 └───────────────────────────┬───────────────────────────────────────────┘
                             ▼
 ┌─ ÂM THANH ────────────────────────────────────────────────────────────┐
 │ KHÔNG CÓ đường thu. DSOUND chỉ ordinal #1 (DirectSoundCreate).        │
 │ ⇒ hoặc bỏ tiếng, hoặc thêm WASAPI loopback / DirectSoundCapture       │
 │   (khảo sát KDSound.cpp / KMusic.cpp trước)                           │
 └───────────────────────────────────────────────────────────────────────┘
```

**🔴 Rủi ro lớn nhất — hiệu năng.** Khoá surface + chuyển màu + nén JPEG chạy **trên luồng game** (không có luồng nén sẵn như jxreplay/bandicam). Ở 1024×768×30fps đó là **~23 MB/giây** dữ liệu thô phải xử lý. Chưa ai đo tốc độ `GdiplusStartup`/`Save`. Nếu chậm thì phải thêm hàng đợi + luồng nén riêng — lúc đó lại vướng luật trộn CRT Debug/Release giữa các module.
**Rủi ro thứ hai:** chưa xác minh `m_Canvas` có chứa **đầy đủ** cả lớp UI và hiệu ứng hay chỉ lớp bản đồ. Chuỗi `RepresentBegin FillCanvas → mọi thứ vẽ qua shell → RepresentEnd UpdateScreen` nói là **có**, nhưng vẫn là suy luận từ mã.

---

### THỨ TỰ THI CÔNG KHUYẾN NGHỊ

1. **Làm BƯỚC 0** (`LockCanvas`, ~15 dòng) rồi bấm PrintScreen — chứng minh/bác bỏ nguồn khung hình, **và sửa luôn lỗi ảnh xanh mà người chơi đang gặp**.
2. **Song song:** chép `bdcap32.dll` 2.5 + thêm mục `[Rec]` vào `.ini` rồi bấm thử (**không cần build**) — trả lời câu hỏi `BCAP_MODE_GDI`. Nhớ tạm chấp nhận hộp thoại assert hoặc thêm `NDEBUG` trước.
3. Nếu (2) **ra hình** → **đường B rẻ nhất**, chỉ còn dọn 10 lỗi lập trình + vấn đề bản quyền. Nếu (2) **ra khung đen** → đi **đường C** (bước 0 đã làm xong một nửa).
4. **Đường A chỉ nên chọn nếu yêu cầu THỰC SỰ là "xem lại trận đấu trong game"** chứ không phải "xuất file video".

---

## 9. LỖI THẬT PHÁT HIỆN KÈM (chưa sửa)

### 9.1 Trong `KUiCapture` của dự án

| # | Vị trí | Lỗi | Hậu quả |
|---|---|---|---|
| 1 | `UiCapture.cpp:141` | `sprintf` vào `KSystemMessage::szMessage[128]` (`GameDataDef.h:939`) | **TRÀN STACK** khi `strlen(RecPath)` từ 79-127. Cấu hình hiện tại (9 ký tự) may mắn an toàn. Đổi sang `_snprintf` |
| 2 | `UiCapture.cpp:59-60` | `memcpy(m_zRecPath, path, strlen(path))` **không ghi NUL** và không xoá đuôi cũ | `RecPath` ngắn hơn 9 ký tự dính đuôi `.\Jxrelay`. VD `D:\R` → `D:\Rrelay` |
| 3 | `UiCapture.cpp:64` | `CreateDirectory(path,…)` dùng **tham số `path`** chứ không phải `m_zRecPath` | đúng nhánh `else` (RecPath rỗng → về mặc định) thì thư mục mặc định **không bao giờ được tạo** |
| 4 | `S3Client.cpp:522-529` | nếu `[Client] RecPath` vắng thì `SetRecPath` **không bao giờ được gọi** | trên máy người chơi không có sẵn thư mục ⇒ `Start()` hỏng ngay |
| 5 | `S3Client.vcxproj:139` | Release **không định nghĩa `NDEBUG`**; `Game.exe` thật sự import `_wassert` từ `api-ms-win-crt-runtime-l1-1-0.dll` | assert **còn sống trong bản phát hành** → hộp thoại lộ đường dẫn `D:\GAMEDEVNEW` rồi abort |
| 6 | `UiShell.cpp:1002-1005` | đóng cửa sổ khi đang quay **không kiểm `IsCapturing()`** | kết thúc phiên ghi **ngầm**, mất thông báo "Video đã lưu". Tệp vẫn đóng đúng nhờ `~KWndWindow` là **virtual** (`WndWindow.h:54`) → `~CBandiCaptureLibrary` → `Destroy()` → `Stop()` |
| 7 | `UiCapture.cpp:228-235` | cờ `doPause` **không được đặt lại** giữa các phiên | lần quay sau nút Pause hiểu ngược |
| 8 | `UiCapture.cpp:70-73` | `SetRecTimmer` thao tác `m_pSelf` **không kiểm NULL**; `S3Client.cpp:1253-1261` cũng không kiểm giá trị trả về | |
| 9 | `UiCapture.cpp:103` | `m_pSelf->LoadScheme(...)` thay vì `this->LoadScheme(...)` trong khi 2 dòng liền kề dùng `this` | hiện vô hại, nhưng là cùng một cái bẫy đang chờ |
| 10 | `UiCapture.cpp:17` | `UiCapture.h:28` khai `void StartRecord();` **không có định nghĩa** ở bất kỳ đâu; `m_StopButton` đã bị gỡ khỏi header nhưng `AddChild` vẫn còn dạng chú thích; `m_CloseButton` khai báo mà không bao giờ `AddChild`; `CheckWinXP()` có thân đủ nhưng **cả hai nơi gọi đều bị chú thích** | bỏ chú thích `:97-98`/`:117-118` sẽ **không biên dịch được** |

⚠️ **Số dòng dễ nhầm:** `Verify(...)` ở **dòng 156**; dòng **155** là bản `//Verify("BCL-SDK-TRIAL",...)` **đã bị chú thích**. Ai theo dòng 155 để vá khoá bản quyền sẽ **sửa nhầm mã chết**.

### 9.2 Trong `jxreplay.dll` (không sửa được — DLL đóng)

Đã liệt kê ở mục 8/Đường A: `WriteHeader` dùng `lea`; bộ đếm khung là BYTE (tràn từ khung 199/214); `strcpy` tên phiên không giới hạn. Thêm: thunk ô vtable 9 kiểm `[ecx+4]` nhưng dùng `[ecx+8]` (vô hại vì ctor tạo cả hai singleton); `StartRec` tạo luồng và `SetStatus(1)` **TRƯỚC** khi khởi tạo header (`0x10007D61`/`0x10007D70` vs `0x10007D81`) — luồng ghi đã sống và `Rec()` đã được phép mã hoá trong lúc bộ đệm header còn chưa có magic/version.

### 9.3 Trong client tham chiếu

`FreeLibrary` tại `0x0056E0B4` xong ghi 0 vào `0x0075B348` (handle Represent) thay vì `0x0075B354` → **rò handle**. Nếu chép nguyên xi mã tham chiếu sẽ chép luôn lỗi này.

---

## 10. BẪY DƯƠNG TÍNH GIẢ — ĐỌC KỸ ĐỂ KHÔNG MẤT THỜI GIAN

| Manh mối | Thực chất | Bằng chứng |
|---|---|---|
| `Rc_RecordList_List`, `BtnRecordEvent` | **Nhật ký sự kiện BANG HỘI** | `UiTongJX2.ini`, `UiTongJX2.cpp` |
| `battlerecord`, `.?AVKBattleRecord@@` | **Biên bản trận Tống Kim** | `game_y_unpacked.bin` `0x00297D6C` nằm trong bảng tên cửa sổ, kẹp giữa `battlerank`/`battlestart`; RTTI `0x002E77C8` kẹp giữa `KBattleReport`/`KBattleStart`; `protocol_gs.lua` `select_camp` = "Phe Tống"/"Phe Kim". **Dự án ta không có lớp này ở cả Core lẫn S3Client** |
| `"Record"`, `"Recorder"`, `"ScreenRecorder"` trong `Game.exe` | **Danh sách đen chống hack**, không phải `l_WindowList` | `.rdata` `0x000F04C8/D0/0x000F0504` xếp alphabet kẹp giữa `"Project"` và `"Simplifier"`, cùng `HACK`/`Ollydbg`/`WinHex`/`XTrap`. Nguồn: `AntiHack\DetectWindowsText\DetectWindowsText.cpp:133/134/139`. **Cờ `ANTI_DETECT_WINDOWS_TEXT = 0`** (`S3Config.h:25`) ⇒ mã chết. Danh sách 245 mục còn chứa `Windows`, `Manager`, `Option`, `Team`, `explorer`… — **cực kỳ dễ dính dương tính giả, đừng bật lại** |
| `klvideo.dll` / `KLVideo.dll` | Bộ **PHÁT** video KLV cho phim mở đầu | export `KLVideoOpen/DoFrame/NextFrame/CopyToBuffer/SetSoundSystem…` — chỉ giải mã |
| `KAviFile`, `KMp4Video` trong `engine.dll` | Bộ **ĐỌC**, không ghi | mục 7.2 |
| `KGifFile::Save` / `lzw_encode` trong header | **Khai báo nhưng không cài đặt** | `KGifFile.cpp` chỉ có 15 định nghĩa, không có `Save`/`write_*`; engine.dll không xuất khẩu |
| Tên `"jxreplay"` | **HAI thứ khác nhau** | `jxreplay.dll` (bản tham chiếu) export `CreateJxReplayInterface`; `jxreplay32.dll` (dự án) = `bdcap32.dll` BandiCam đổi tên, export `CreateBandiCapture`. **Chép nhầm ⇒ `GetProcAddress` trả NULL ⇒ assert** |
| `G_STR_SAVE_SCREENFILE` | Thuộc **chụp màn hình**, không thuộc replay | chỉ 1 nơi dùng `0x0042B905`, không nằm trên nhánh `Replay()` nào |

---

## 11. ĐỘ TIN CẬY & NHỮNG GÌ CHƯA KIỂM ĐƯỢC

**Đã kiểm tới byte:** toàn bộ vtable/RTTI/bảng nhảy/địa chỉ nêu trên (mỗi con số qua ít nhất 2 lượt độc lập); chỉ mục 37 pak (2 bộ công cụ viết độc lập cho ra cùng kết quả); md5 + giải mã 52 ảnh JPEG; bảng import/export của `engine.dll`, `Represent2.dll`, `CoreClient.dll`, `Game.exe`, `bdcap32.dll`, `jxreplay.dll`, `zlib.dll`; mọi trích dẫn `tệp:dòng` trong `D:\GAMEDEVNEW`.

**Chưa kiểm được / còn bỏ ngỏ:**
- 🔴 **Chưa chạy game lần nào** — mọi kết luận về hành vi đều từ đọc mã/byte.
- 🔴 **`BCAP_MODE_GDI` có lấy được khung hình của client DirectDraw này không** — chưa ai thử. Hai lập luận đối lập đều là suy diễn: một bên nói "gần như chắc chắn khung đen", một bên chỉ ra `UiCapture.cpp` của ta **giống từng byte** bản gốc G7VN vốn được đóng gói **kèm** BCL 2.5 (⇒ GDI là lựa chọn có chủ đích của tác giả gốc) và `FullScreen=0` là kịch bản GDI thường chụp được. **Bằng chứng "ảnh xanh" KHÔNG áp dụng được cho GDI** — hai đường đọc độc lập (một bên `Lock` primary surface, một bên `BitBlt` từ DC cửa sổ).
- **Ánh xạ 8/9 offset callback → tên phương thức `iRepresentShell`** (mục 3.1) là **suy luận theo ngữ nghĩa**; chỉ `+0x68 = SetLightInfo` là gần như chắc. Cách chốt 100%: dịch ngược `0x10005860` / `0x10004710` / `0x10005510` trong `represent2.dll` tham chiếu rồi so với cài đặt của ta.
- **VÌ SAO `LockPrimaryBuffer` trả về hằng số `0x01DF`** — con số đã verified bằng toán học từ mã nguồn, nhưng nguyên nhân (DWM chuyển hướng) là **giả thuyết**, chưa gỡ lỗi trên máy thật.
- **Container thật của tệp `.mp4`** do BCL sinh ra — DLL có cả hai muxer, phải chạy thử rồi đọc 4 byte đầu (`RIFF` hay `ftyp`).
- **Khoá `Verify("MAIET-GUNZ-20080916")` còn hiệu lực với BCL 2.5 không** — kiểm bằng băm, DLL chỉ chứa chuỗi `BCL-SDK-TRIAL`.
- **Toàn bộ định dạng `.jxr` suy ra từ mã DLL** — **không có tệp `.jxr` mẫu nào trên máy** (3 mũi quét độc lập xác nhận). Riêng cấu trúc thân tệp (`WORD+BYTE+WORD(len)+data`) tự đánh giá là **medium**.
- **Header `.jxr` `+0x25` (tham số 2 của `StartRec`) và vùng `0x2E..0x12D`, `0x332..0x44F`** — **không xác định**; không mã nào trong DLL đọc/ghi các vùng đó.
- **`m_Canvas` có chứa đầy đủ lớp UI và hiệu ứng không** — suy luận từ chuỗi `RepresentBegin`/`RepresentEnd`, chưa chạy thật.
- **Bộ ghi hình tự viết có chịu nổi 30fps trên luồng game không** — chưa đo tốc độ `SaveBufferToJpgFile24`.
- **Nhánh "thoát khi đang phát" của bản tham chiếu gọi `Replay([[rec]])` thay vì `stop`** (`0x0056E45F`) — **không giải thích được**, có thể là lỗi của bản gốc.
- **Chưa giải nén được UCL** nên chưa so byte nội dung 2 tệp `.ini` replay trong `update01.pak` (706 B / 1.597 B) với bản rời của bản tham chiếu (858 B / 1.729 B).
- 3 tệp `.so` của máy chủ mới chỉ quét chuỗi, chưa dịch ngược (xác suất liên quan cực thấp).

---

## PHỤ LỤC — ĐƯỜNG DẪN & ĐỊA CHỈ HAY DÙNG

| Thứ | Nơi |
|---|---|
| Bản tham chiếu (client) | `D:\ServerLinux\Patch\` — `game_y_unpacked.bin` (VA = `0x401000` + offset), `jxreplay.dll`, `zlib.dll`, `msvcp80/msvcr80.dll`, `ui\ui3_1024\`, `ui\autoexec.lua` |
| Bản tham chiếu (máy chủ) | `D:\ServerLinux\server1\jx_linux_y` — **sạch, không cần đụng** |
| Client đang phát hành | `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\` |
| **`bdcap32.dll` 2.5.0.250** | `E:\SourceTuanLe\Source_G7VN\JX\swrod3\SwordOnline\Sources\S3Client\bdcap32.dll` (11.926.504 B, 02/02/2017) + `bdcap64.dll`; **6 bản sao** dưới `SRC_UPDATE\{SRCUPDATA,SwordOnline,SwordOnline_8_7,SwordOnline_loi,SwordOnline_loi2}\SwordOnline\Sources\S3Client\`. Cạnh đó có `bandicap.h` **bản gốc** (19.595 B) mà `D:\GAMEDEVNEW` chép lại (19.571 B, khác 2 chỗ: bỏ `#include <tchar.h>`, và dòng 301 từ chú thích chuyển thành sống) |
| Cây VLTK chính chủ 2025 (nguồn nhị phân thay thế) | `C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky 2.0\` — `jxreplay.dll` 95.608 B (27/05/2025, **cùng định dạng**), `zlib.dll` 69.632 B, `klvideo.dll` |
| Mã nguồn client | `D:\GAMEDEVNEW\Sources\S3Client\` — `Ui\UiCase\UiCapture.{cpp,h}`, `bandicap.h`, `Ui\ShortcutKey.{cpp,h}`, `Ui\UiShell.cpp`, `S3Client.cpp`, `S3Config.h`, `AntiHack\DetectWindowsText\DetectWindowsText.cpp` |
| Mã nguồn Represent/Engine | `D:\GAMEDEVNEW\Sources\Represent\Represent2\KRepresentShell2.cpp`, `Represent\iRepresent\{iRepresentShell.h,RepresentUtility.cpp}`, `Engine\Src\{KCanvas.cpp,KCanvas.h,KDDraw.cpp,KDDraw.h,KBmpFile24.cpp,KAviFile.*,KGifFile.*,KPakFile.cpp,KPakList.cpp,XPackFile.cpp}` |
| PDB đúng của `Game.exe` | `D:\GAMEDEVNEW\Sources\S3Client\Release\S3Client.pdb` (GUID `f731ac0c-d02f-4542-bcfe-d456908c8add`, age 27) — **KHÔNG dùng `Game.pdb` cạnh exe** |

### Địa chỉ — `jxreplay.dll` (imagebase `0x10000000`)

| VA | Là gì |
|---|---|
| `0x10001380` → `0x10001300` | `CreateJxReplayInterface` (thunk → thân thật) |
| `0x10001270` | ctor `KJxReplay`; `0x10010174` = vtable 19 ô |
| `0x10007DC0` / `0x10009110` | ctor `K2DREC` (`0x573C` B) / `K2DPlay` (`0xAD4` B) |
| `0x10010264` (7 ô) / `0x10010284` (10 ô) | vtable `K2DREC` / `K2DPlay` |
| `0x10007930` | bộ điều phối `Rec()`; bảng chỉ số 25 byte @`0x10007AB8`, bảng nhảy @`0x10007A80` |
| `0x10007700` | bộ theo dõi trạng thái (chụp 5 loại: 1, 12, 17, 20, 21) |
| `0x10007CD0` / `0x10001A20` | `StartRec` / `EndRec` |
| `0x10009A10` | `KJXRFile::Open` — nơi dựng `UserData\Temp\<timestamp>.jxr` |
| `0x100093D0` / `0x10009660` | nén (deflate) / `ReadHeader` |
| `0x100097D0` | `WriteHeader` — **lỗi `lea` tại `0x100097F4`** |
| `0x1000C6A0` | bộ giải mã khi phát; kiểm `[0x100131B8]` tại `0x1000C6A2` |
| `0x100131B8` / `0x100131BC` | con trỏ giao diện VẼ / ÂM THANH |

### Địa chỉ — client tham chiếu (VA)

| VA | Là gì |
|---|---|
| `0x006E4530` (150 mục) | `GameScriptFuns`; `Replay` = mục 19 @`0x006E45C8/CC` → `0x0040FF30` |
| `0x006E4450` (8 mục) | bảng động từ `rec/endrec/play/stop/pause/speedup/slowdown/pauserec` |
| `0x0040FE90` / `0x004103C8` | tra chuỗi → chỉ số / bảng nhảy 8 mục |
| `0x0056D4A0` ← `0x0056D615` | nạp `JXReplay.dll`; gọi từ `KMyApp::InitRepresent 0x0056D520` |
| `0x0075F334` / `0x0075B350` / `0x0075B354` | `g_pReplay` / `g_nReplayState` / HMODULE |
| `0x0056D630/653/66C` | gắn 2 chiều shell ↔ replay |
| `0x0056E3BD` | làm mới state mỗi khung (vtbl+0x34) |
| `0x006A3C3C` / `0x00758C10` | vftable / thực thể toàn cục `KUiJxrPlayer` |
| `0x0052D210` | `KUiJxrPlayer::Init` — nạp `录像操作界面.ini`, 6 nút ở +0x244/+0x5A0/+0x8FC/+0xC58/+0xFB4/+0x1310 |
| `0x0069A0BC` / `0x00428DB0` | vftable `Player_Recorder` / xử lý bấm |
| `0x00476B70` / `0x00476C30` | hộp thoại `GetOpenFileNameA` lọc `*.jxr` / hàm bao (ép 2D, 800×600) |
| `0x006E4470` (48 mục) | bảng tên cửa sổ `Open()` — **KHÔNG có mục replay nào** |

### Địa chỉ — `Game.exe` dự án (imagebase `0x400000`; `.rdata` VA−RAW = `0x400C00`, `.data` = `0x401200`)

| File-offset | VA | Chuỗi |
|---|---|---|
| `0x000F2E58` | `0x004F3A58` | `rec` (mục 24 của `l_WindowList`) |
| `0x000F6670` / `0x000F667C` / `0x000F668C` | `0x004F7270` / `0x004F727C` / `0x004F728C` | `bdcap32.dll` / `jxreplay32.dll` / `CreateBandiCapture` |
| `0x000F6734` / `0x0011C488` | `0x004F7334` / `0x0051D688` | `.\Jxrelay` hằng / biến tĩnh `m_zRecPath[260]` |
| `0x000F6740` | `0x004F7340` | `UiCapture.ini` |
| `0x000F6800` / `0x000F680C` | `0x004F7400` / `0x004F740C` | `d25f910a` / `MAIET-GUNZ-20080916` |
| `0x000F6618` / `0x000F6790` | | (UTF-16) `D:\GAMEDEVNEW\…\bandicap.h` / `…\UiCapture.cpp` — **chuỗi của assert** |
| `0x0011DC1C` / `0x0011EC50` | `0x0051EE1C` / `0x0051FE50` | RTTI `KUiCapture` / `Player_Rec` |
