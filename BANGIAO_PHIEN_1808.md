# BÀN GIAO PHIÊN 18/08/2026 — client JX1: FPS/nội suy, crash map đông, dịch ngược bản gốc

> **Đọc tệp này TRƯỚC KHI GÕ bất cứ dòng nào trong phiên sau.**
> Phiên này mất phần lớn thời gian vì **mắc đi mắc lại cùng một loại lỗi**. Mục 2 là để
> không lặp lại. Mục 5 là bộ công cụ dịch ngược đã dựng sẵn, chạy được ngay.

---

## 1 · Trạng thái hiện tại

### 1.1 Hai cây mã nguồn — KHÔNG chép đè lẫn nhau

| | đường dẫn | vai trò |
|---|---|---|
| **D** | `D:\GAMEDEVNEW` | kho git chính, **có** `KPlayerBot` |
| **E** | `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\SOURCESUPDATE_KINHMACH_ONLTEST0608` | cây vận hành, **không có** `KPlayerBot` |
| bin | `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client` | thư mục game chạy thật |

Sửa ở **D** → commit → **áp riêng từng thay đổi** sang **E**. `cp` nguyên tệp giữa hai cây
đã làm hỏng build một lần (`KNpc.cpp` của D `#include "KPlayerBot.h"` mà E không có).

### 1.2 Máy chủ nằm ở MÁY KHÁC

`206.82.7.181:6667`. **Mọi thay đổi phía server (`CoreServer.dll`) trong phiên này chưa từng chạy.**
Trước khi đổ lỗi cho thay đổi server, kiểm bằng `Get-NetTCPConnection` của `Game.exe`.
Hệ quả: mọi cải thiện phải làm được **chỉ bằng client**.

### 1.3 Cấu hình đang chạy (`[Client]` trong ini)

```
PaintFps   = 60      ; nhịp VẼ (nhịp logic vẫn cố định 18, không được nâng)
PaintInterp= 1       ; bật nội suy vị trí khi vẽ
PaintLog   = 0
PerfHud    = 1       ; hiện FPS/CPU/GPU/RAM trong game
```

### 1.4 Module đã triển khai — **bảng CRT bắt buộc**

| module | cấu hình PHẢI dùng | CRT |
|---|---|---|
| `engine.dll` | `Debug\|Win32` | UCRT-DEBUG |
| `Represent2.dll` | `Debug\|Win32` | UCRT-DEBUG |
| `LuaLibDll.dll` | (không đụng tới) | UCRT-DEBUG |
| `Game.exe` (S3Client) | `Release\|Win32` | UCRT-RELEASE |
| `CoreClient.dll` | `ClientRelease` | CRT tĩnh |

Kiểm trước mỗi lần triển khai:

```bash
python D:/GAMEDEVNEW/ReverseTools/re_pe_crt.py E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/client
```

### 1.5 Commit mốc

| sha | nội dung |
|---|---|
| `eba30b04` | **revert** `c8981653` — "cờ xin nắn vị trí", đã gây "chân chạy tại chỗ" |
| `042ee210` | vá treo `KRegion::BroadCast` (vòng lặp không tiến node) |
| `54779c0c` | vá 2 lỗi tôi tự gây ra làm crash map đông |
| `4ecfea49` | bộ bắt lỗi sập game bản đầu |
| `b481c665` | bộ bắt lỗi **nhiều lớp** + tra tên hàm/dòng mã từ PDB |
| `ab7b5b4c` | **sửa tận gốc crash map đông** (`free()` → `release_image()`) |

---

## 2 · TOÀN BỘ LỖI ĐÃ MẮC PHẢI

### 2.1 ⚠️ Mắc **HAI LẦN** cùng một lỗi: trộn CRT Debug/Release

Đây là lỗi tốn thời gian nhất phiên này.

UCRT bản gỡ lỗi bọc thêm `_CrtMemBlockHeader` quanh mỗi khối. Con trỏ người dùng vì thế
**lệch** so với khối thật. Module Release gọi `free()` lên khối do Debug cấp phát →
`RtlFreeHeap` nhận con trỏ giữa khối → **`0xC0000374` heap corruption**. Chiều ngược lại →
**`_CrtIsValidHeapPointer` Debug Assertion**.

| lần | tôi làm gì | hậu quả |
|---|---|---|
| 1 | dựng `engine.dll` bằng `Release` | sập ngay lúc **đăng nhập** |
| 2 | dựng `Represent2.dll` bằng `Release` | sập ở **map đông người** |

**Cách nhận biết cấu hình đã phát hành** (không phải đoán): bước sau-khi-dựng của
`Debug|Win32` chép ra **hai** nơi — `bin\client\` *và* `bin\client\debug\`. Có tệp trong
`bin\client\debug\` nghĩa là bản phát hành được dựng bằng Debug.
Cỡ lớn cũng phân biệt được: `Represent2.dll` Debug = **130.048 byte**, Release = 47–73 KB.

### 2.2 🔴 `build = deploy` — bước sau-khi-dựng ghi đè binary đang chạy

`Core.vcxproj` / `Engine.vcxproj` / `S3Client.vcxproj` / `Represent2.vcxproj` đều **tự chép**
sang `bin\client`, `bin\client\debug`, `bin\server`, `bin\Multiserver`. Đã **ghi đè mất
`engine.dll` gốc** một lần (cứu lại từ `bin\client\debug`).
Lưu ý client 32-bit vs server 64-bit — suýt ghi đè nhầm.

### 2.3 🔴 Bước sau-khi-dựng **im lặng thất bại** khi game đang mở

Tệp bị khoá → `copy` lỗi → MSBuild vẫn báo dựng xong → **binary cũ vẫn nằm đó**.
Bắt buộc kiểm dấu thời gian tệp trong `bin\client` sau mỗi lần dựng.
Thay tệp đang chạy bằng **đổi tên**:

```bash
mv bin/client/X.dll bin/client/X_cu.dll && cp <ban moi> bin/client/X.dll
```

### 2.4 🔴 Vá xong nhưng **chưa ghi vào tệp** — commit mô tả việc không có trong diff

Script python vá nhiều chỗ, `assert` giữa chừng → in "OK" nhưng chưa `write` → commit
`ce8c4d49` mô tả việc **không có trong diff** (phần cắt cành khi vẽ nền đất **chưa từng
được viết**, vòng lặp vẫn duyệt đủ 49 vùng — **việc này vẫn còn nợ**).

> **Luật:** vá xong **phải `grep` lại chính tệp đó** rồi mới commit.

### 2.5 🔴 Bẫy chuỗi có dấu `\` trong Bash

Bash tool (kể cả heredoc trích dẫn) **rút `\\` thành `\`**. Đã hỏng hook Dã Tẩu 2 ngày,
và trong phiên này làm hỏng đường dẫn khi sửa `.vcxproj`.
→ Chuỗi có backslash **phải đi qua tệp** (dùng Write tool, hoặc python với dấu `/`).

### 2.6 🔴 Bẫy CRLF

Nguồn dùng CRLF. Mẫu tìm-thay trong python dùng `\n` sẽ **không khớp** → phải dựng mẫu
bằng `\r\n`. `sed` của MSYS **nuốt CR**. So sánh tệp phải dùng `diff --strip-trailing-cr`.
Đọc/ghi nhị phân an toàn: `read_bytes().decode('latin-1')` → sửa → `encode('latin-1')`.

### 2.7 🔴 `.sln` ánh xạ `x64` → `Win32`

Dựng qua `.sln` sẽ ra sai nền tảng. **Dựng thẳng `.vcxproj`.**
`S3Client` bắt buộc `/p:VcpkgEnableManifest=false`; `Core` thì **cấm** cờ này.
`S3Client` trong D dùng tên cấu hình `Release|Win32` (không phải `Client Release|x86`).

### 2.8 🔴 Cấy mã từ bản gốc mà **bỏ mất điều kiện** của nó

`c8981653` port "cờ xin nắn vị trí" nhưng bỏ mất cổng chặn của bản gốc → sinh ra
"chân chạy tại chỗ" + "giựt lùi" → phải revert (`eba30b04`).

### 2.9 🔴 Port **nửa cơ chế**

`KImageStore2`: bản gốc đuổi ảnh theo **thời gian nhàn rỗi VÀ ngân sách byte**
(160/192/256/512 MB tuỳ RAM). Tôi chỉ port phần thời gian → mất cận trên → kho ảnh phình.
Đã vá tạm bằng trần cứng `KIS_HARD_IMAGE_CEIL 2048`. **Ngân sách byte vẫn chưa port.**

### 2.10 🔴 Lỗi ghi tràn mảng do chính tôi

`for (int nMsl = 1; nMsl <= MAX_MISSLE; ...)` — phải là `<`. Gây ghi tràn.

### 2.11 🔴 Hai giả thuyết của tôi đã bị **chính tôi bác bỏ** (đừng đi lại)

| giả thuyết | kết quả |
|---|---|
| ngưỡng `PAINT_INTERP_SNAP_DIST 64` giết nội suy | **SAI.** Mỗi tick dịch **dưới 32 px** — `KNpc.cpp:4450` chặn ±1 ô/tick |
| crash do cạn không gian địa chỉ 32-bit | **SAI.** Lúc sập mới dùng **713 MB / 2048 MB** |
| `KSubWorld.cpp:1542` (client) khác `1548` (server) về NPC-là-tường | **SAI.** Hai bên **giống nhau** |

### 2.12 Workflow đa tác nhân chết vì hết hạn mức phiên

10/10 tác nhân lỗi `session limit`. Việc điều tra phải làm tay. Nếu chạy workflow, **chia nhỏ**.

---

## 3 · QUY TRÌNH LÀM VIỆC BẮT BUỘC

> Đây là các luật do chính người dùng đặt ra sau khi tôi làm sai. Tuân thủ nguyên văn.

1. **Phân tích bản gốc CHO KỸ TRƯỚC KHI GÕ.** Không "làm rồi lỗi rồi gỡ rồi phản biện".
   Nguyên văn: *"lỗi do bạn không phân tích kỹ thì đã làm chứ không phải làm lỗi rồi gỡ rồi phản biện"*.
2. **Lỗi thì phải hỏi: vì sao bản gốc KHÔNG lỗi?** Rồi làm giống bản gốc.
3. **Không đoán mò.** Mọi khẳng định phải có số dòng mã hoặc byte thật.
4. **Vá xong `grep` lại tệp** rồi mới commit.
5. **Kiểm binary đã triển khai**, không tin log dựng:
   - `grep -q -F "<chuoi moi>" bin/client/Game.exe`
   - `python ReverseTools/re_pe_crt.py bin/client`
   - so dấu thời gian tệp trong `bin\client`
6. **Commit + push chính tệp đã sửa, ngay trong phiên.**
7. **Trả lời bằng tiếng Việt.**
8. Prompt vừa "fix" vừa "phân tích" → **mặc định chỉ phân tích**.

### 3.1 Quy trình dựng + triển khai chuẩn

```bash
# 1. sua o D, kiem lai bang grep
cd /d/GAMEDEVNEW && grep -n "<thay doi>" <tep>

# 2. dung o D de bat loi bien dich som
"/c/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe" \
  Sources/<du an>/<x>.vcxproj -p:Configuration=<cau hinh> -p:Platform=Win32 \
  -p:VcpkgEnableManifest=false -m -v:m -nologo

# 3. ap RIENG thay doi sang E (khong cp nguyen tep neu tep do khac nhau giua 2 cay)

# 4. dung o E, dung DUNG cau hinh trong bang muc 1.4

# 5. kiem tep trong bin/client da doi that chua (dau thoi gian + grep chuoi)

# 6. kiem CRT
python /d/GAMEDEVNEW/ReverseTools/re_pe_crt.py \
  E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/client

# 7. commit + push o D
```

### 3.2 Bộ bắt lỗi sập game

`Sources/S3Client/CrashLog.{h,cpp}`, gọi `CrashLog_Install()` đầu `WinMain`.
Ghi ra `bin\client\jx_crash.log` (ghi thêm, không đè).

Bắt **4 lớp** — vì lần đầu chỉ có `SetUnhandledExceptionFilter` nên log **chỉ có dòng
khởi động, không có bản ghi sập**:

| lớp | bắt gì |
|---|---|
| `AddVectoredExceptionHandler` | chạy **trước** mọi SEH khác |
| `SetUnhandledExceptionFilter` | lưới cuối |
| `signal` SIGABRT/SEGV/ILL/FPE + `_set_abort_behavior(0,3)` | đường chết của CRT (Debug Assertion) |
| `_set_invalid_parameter_handler` | CRT nhận tham số hỏng |

Bản ghi có: mã lỗi + nghĩa tiếng Việt, module+offset, **tên hàm + tệp + dòng** (PDB),
đọc/ghi, tình trạng bộ nhớ (**dòng này phân biệt "hết địa chỉ ảo" với "lỗi con trỏ"**),
thanh ghi, ngăn xếp, danh sách module.
`dbghelp` / `AddVectoredExceptionHandler` / `RtlCaptureContext` **nạp động** → không thêm thư viện liên kết.
Gọi `SymRefreshModuleList` trước khi đọc ngăn xếp vì `Represent2.dll` nạp bằng `LoadLibrary`
**sau** `WinMain` (`CoreClient.dll` và `Engine.dll` thì nhập tĩnh — đã đọc bảng nhập để xác nhận).

Muốn biết game đang làm gì lúc chết: `CrashLog_Mark("ten moc", giatri)`.

---

## 4 · Công việc đã làm & còn nợ

### 4.1 Đã xong

- **Tách nhịp vẽ khỏi nhịp logic** (`PaintFps`) + **nội suy vị trí** (`GOI_PROCFRAME_BREATHE`
  chụp ảnh mỗi tick, `GOI_PROCFRAME_POSSHIFT` nội suy mỗi khung vẽ, alpha 0..1000).
  Bản gốc vẽ 30 hình/giây và **không** nội suy → phần này ta đã vượt bản gốc.
- **Nhịp khung hình bản gốc là 18 FPS**, cả JX1 lẫn JX2 (đã dump byte xác nhận).
  `[GameFPS = %d]` là FPS **đo được**, không phải cấu hình. **Không được nâng nhịp logic.**
- Bản gốc **không có luồng vẽ riêng** (chỉ 4 `CreateThread`: dò độ trễ máy chủ, 2 cho hoạt
  ảnh bingo, 1 nạp cảnh).
- HUD trong game: FPS vẽ/logic/ping, ms khung + tệ nhất, CPU%, GPU%/VRAM, RAM.
- Bộ bắt lỗi sập game nhiều lớp (mục 3.2).
- **Vá crash map đông tận gốc** (mục 4.2).
- Vá tràn `szPackName`, vá treo `KRegion::BroadCast`.

### 4.2 Crash map đông — nguyên nhân & bản vá (đã xong, chờ người dùng test)

Log chỉ thẳng:

```
0xC0000374 heap corruption
  free()                          <- ucrtbase.dll (RELEASE)
  KImageStore2::FreeImageObject   KImageStore2.cpp:767
  KImageStore2::CheckBalance      KImageStore2.cpp:600
  KImageStore2::GetImage          KImageStore2.cpp:340
  KRepresentShell2::DrawPrimitives -> KNpcRes::Draw -> KNpc::Paint -> KScenePlaceC::Paint
```

Ảnh JPG do `get_jpg_image()` của **engine.dll** cấp phát bằng `malloc`
(`Sources/Engine/Src/KPakFile.cpp:171`) nhưng `KImageStore2` giải phóng bằng `free()` của
**Represent2.dll** → hai vùng nhớ động khác nhau.

Engine **đã xuất sẵn** `release_image()` (`Sources/Engine/Src/KPakFile.h:58`). Mọi chỗ khác
trong chính tệp đó đều trả về engine (`SprReleaseHeader` / `SprReleaseFrame`) — **chỉ riêng
dòng 767 bỏ quên**. Đã sửa (`ab7b5b4c`) + trả `Represent2.dll` về `Debug|Win32`.

Vì sao **chỉ** lộ ở map đông: `CheckBalance` chỉ bắt đầu đuổi ảnh khi kho vượt **1024 ảnh**.
Map vắng không bao giờ chạm ngưỡng nên `free()` không bao giờ chạy.

### 4.3 Còn nợ — **việc của phiên sau**

| # | việc | ghi chú |
|---|---|---|
| 1 | **"Nhìn người khác di chuyển giật như robot khi đông"** | phân tích ở mục 6, **chưa sửa** |
| 2 | Cắt cành khi vẽ nền đất | `ce8c4d49` **khai mà chưa làm** — vẫn duyệt đủ 49 vùng |
| 3 | Port **ngân sách byte** của kho ảnh | hiện chỉ có trần cứng 2048 ảnh |
| 4 | "Qua map không có ai cũng nhảy lên 100 ms rồi về 19" | chưa truy |

---

## 5 · CÁCH DỊCH NGƯỢC BẢN GỐC

### 5.1 Ba bản gốc và cách quy đổi địa chỉ

| bản | đường dẫn | quy đổi địa chỉ ảo ⇄ tệp |
|---|---|---|
| client Windows gốc, **đã giải nén** | `D:\ServerLinux\Patch\game_y_unpacked.bin` (22.551.940 B) | **VA = 0x401000 + độ lệch tệp** |
| client Windows gốc, còn nén | `D:\ServerLinux\Patch\game_y.exe` | UPX biến thể `NRV2E_LE32`, bộ lọc call `C = -0x45BFEFFC` |
| máy chủ Linux gốc | `D:\ServerLinux\server1\jx_linux_y` (ELF32) | qua bảng `PT_LOAD`, xem dưới |

Bảng `PT_LOAD` của `jx_linux_y` (**5 đoạn, không phải một công thức duy nhất**):

```
  offset 0x00000000 -> vaddr 0x08048000  filesz 2.695.992   (ma may, PF_X)
  offset 0x00877000 -> vaddr 0x082DB000  filesz    55.776
  offset 0x00293190 -> vaddr 0x09B6C190  filesz 5.746.814
  offset 0x0080F210 -> vaddr 0x0A0E8210  filesz   421.464
```

> ⚠️ **`jx_linux_y` đã bị lược bảng mục** (`e_shoff = 0`) và `.dynsym` chỉ còn **25 ký hiệu**
> → **không có tên hàm game**. Ghi chú cũ nói "trích được 1.421 hàm" là của **JX2**
> (`D:\ServerLinux` — binary khác), **không áp dụng cho bản này.**

### 5.2 Bộ công cụ đã dựng sẵn — `D:\GAMEDEVNEW\ReverseTools\`

Máy này **không có** `objdump` / `readelf` / `nm` / `strings`. Chỉ có **python + capstone 5.0.7**.
Ba công cụ dưới đây thuần python, chạy được ngay.

#### `re_elf_luamap.py` — lấy lại **tên hàm** của máy chủ Linux

Máy chủ đăng ký hàm cho Lua bằng bảng gồm các cặp *(con trỏ tên, con trỏ mã)*. Quét bảng đó
là lấy lại được tên hàm dù binary đã bị lược ký hiệu.

```bash
python D:/GAMEDEVNEW/ReverseTools/re_elf_luamap.py D:/ServerLinux/server1/jx_linux_y
```

**Kết quả đã trích sẵn: `ReverseTools/jx_linux_y.luamap.txt` — 42 bảng, 1.344 tên hàm.**
Ví dụ: `TONG_GetTongCount@0x818A6C0`, `LoadMap@0x81013D0`, `GetItemBelong@0x80FEC30`.

#### `re_disasm.py` — dịch ngược theo địa chỉ ảo, cho **cả hai** định dạng

```bash
# may chu Linux
python ReverseTools/re_disasm.py D:/ServerLinux/server1/jx_linux_y 0x080FB820 12
# client Windows goc
python ReverseTools/re_disasm.py D:/ServerLinux/Patch/game_y_unpacked.bin 0x006429A0 22
# tim ai tham chieu mot chuoi
python ReverseTools/re_disasm.py D:/ServerLinux/Patch/game_y_unpacked.bin --find "GameFPS"
```

#### `re_pe_crt.py` — **kiểm thử bắt buộc trước khi triển khai**

```bash
python ReverseTools/re_pe_crt.py E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/client
```

Trả mã thoát khác 0 nếu có module sai cấu hình. Chạy cái này là tránh được cả mục 2.1.

### 5.3 Quy trình dịch ngược một câu hỏi cụ thể

1. **Tìm điểm neo.** Chuỗi (`--find`), hoặc hằng số đặc trưng, hoặc tên hàm từ `luamap.txt`.
2. **Dịch ngược quanh điểm neo** bằng `re_disasm.py`.
3. **Nhận dạng hàm bằng chữ ký**, không bằng cảm giác:
   - `ret 0x1C` = 7 tham số trên ngăn xếp (`__thiscall`, `this` trong `ecx`)
   - `shl edx, 4` = nhân 16 → khớp `REGION_GRID_WIDTH = 16`
   - giá trị trả `mov al, 4` = `Obstacle_JumpFly`, `xor al, al` = `Obstacle_NULL`
4. **Đối chiếu với nguồn của ta** — tìm đúng hàm tương ứng trong `Sources/`.
5. **Chỉ port phần KHÁC NHAU, kèm nguyên điều kiện của nó** (xem lỗi 2.8).

### 5.4 Đọc dữ liệu tĩnh (giá trị mặc định của biến toàn cục)

```python
d = open(r"D:\ServerLinux\Patch\game_y_unpacked.bin", 'rb').read()
BASE = 0x401000
def rd(va, n=4): return d[va-BASE : va-BASE+n]
# tim moi cho tham chieu toi mot dia chi:
import struct
pat = struct.pack('<I', 0x0140F3B4)
```

---

## 6 · Việc còn nợ #1: "nhìn người khác di chuyển giật như robot khi đông"

### 6.1 Triệu chứng

Map đông → **người khác và NPC** di chuyển giật từng nấc, cứng đờ. **FPS vẫn cao.**
**Nhân vật của mình vẫn mượt.**

### 6.2 Cơ chế đã truy được trong nguồn của ta

Client **tự mô phỏng** bước đi của người khác giữa các gói tin:

```
KNpc.cpp:4397         nRet = m_PathFinder.GetDir(...)
KNpcFindPath.cpp:74     nCheckBarrier = CheckBarrier(x, y)
KNpcFindPath.cpp:264      -> SubWorld[0].TestBarrierMin(...)
KSubWorld.cpp:1544          -> GetBarrierMin(nMapX, nMapY, nDx, nDy, TRUE)
KRegion.cpp:1018-1021         if (bCheckNpc && m_pNpcRef)
                                  if (m_pNpcRef[y*w+x] > 0) return Obstacle_JumpFly;
KNpcFindPath.cpp:106  m_nFindTimes++;
KNpcFindPath.cpp:107  if (m_nFindTimes > 1) return 0;      <-- CHI NE 1 LAN
KNpc.cpp:4405         nRet == 0 -> DoStand();              <-- dung phat giua buoc
```

Ô có người ⇒ tường ⇒ né hụt lần thứ hai ⇒ **`DoStand()`** ⇒ đứng khựng ⇒ gói tin kế tiếp
(`KNpc::AutoFixXY`, `KNpc.cpp:10140`) **giật** họ tới. Đó là "đi như robot".

Giả thuyết này qua được **cả ba phép thử**:

| phép thử | vì sao qua |
|---|---|
| chỉ khi đông | `m_pNpcRef > 0` chỉ xảy ra khi ô bên cạnh có người |
| FPS vẫn cao | đây là dừng/chạy phía **logic**, không tốn sức vẽ |
| nhân vật mình vẫn mượt | mình liên tục ra lệnh đích mới; `KNpcFindPath.cpp:60-67` **đổi đích thì reset `m_nFindTimes = 0`** → hạn mức né của mình được nạp lại liên tục. Người khác có đích **cố định** từ máy chủ nên cạn hạn mức rồi đứng |

### 6.3 ✅ BẢN GỐC LÀM KHÁC — đã kiểm bằng byte thật

Dịch ngược `game_y_unpacked.bin` tại `0x006429A0`:

```asm
006429A0  cmp dword [esp+0x18], 0     ; bCheckNpc
006429A5  je  6429DB                  ; khong kiem NPC -> Obstacle_NULL
006429A7  lea eax, [ecx+0xAC]         ; mang m_pNpcRef nam NGAY TRONG doi tuong, tai this+0xAC
006429AD  test eax, eax
006429AF  je  6429DB
006429B1  cmp dword [esp+4], 0        ; <== THAM SO 1
006429B6  jne 6429C1                  ;     khac 0 -> kiem NPC binh thuong
006429B8  cmp dword [0x0140F3B4], 1   ; <== CO TOAN CUC
006429BF  jne 6429DB                  ;     co != 1 -> KHONG coi NPC la tuong
006429C1  mov edx, [esp+0xC]
006429C5  shl edx, 4                  ; *16 = REGION_GRID_WIDTH
006429C8  add edx, [esp+8]
006429CC  cmp byte [edx+ecx+0xAC], 0
006429D4  jbe 6429DB
006429D6  mov al, 4                   ; Obstacle_JumpFly
006429D8  ret 0x1C                    ; 7 tham so
006429DB  xor al, al                  ; Obstacle_NULL
006429DD  ret 0x1C
```

**Nguồn của ta KHÔNG có cổng `if (thamso1 != 0 || g_co == 1)`.** Ta coi NPC là tường
**vô điều kiện** khi `bCheckNpc`.

Cờ toàn cục `0x0140F3B4`:
- giá trị trong ảnh tệp = **0** (tức mặc định **KHÔNG** coi NPC là tường)
- được **ghi** tại `0x005A3047` (`mov [0x0140F3B4], edx`) → chỉnh được lúc chạy
- được **đọc** tại **hai** chỗ: `0x00642973` và `0x006429BA` (hai hàm gần giống nhau)

### 6.4 Bước tiếp theo (làm theo đúng thứ tự, **đừng nhảy cóc sang sửa**)

1. Dịch ngược quanh `0x005A3047` xem **ai** ghi cờ và ghi giá trị gì
   (`python ReverseTools/re_disasm.py D:/ServerLinux/Patch/game_y_unpacked.bin 0x005A3000 60`).
2. Xác định **tham số 1** của hàm `0x006429A0` là gì — dịch ngược nơi gọi nó.
   Nghi vấn: cờ "đang giao chiến". Cần xác nhận, **không đoán**.
3. Xác định hàm `0x006429A0` tương ứng với hàm nào trong nguồn của ta.
   `ret 0x1C` = **7** tham số, còn `KRegion::GetBarrierMin` của ta chỉ có **5**
   (`ret 0x14`) → **chưa chắc là cùng một hàm.** Phải khớp cho chắc trước khi port.
4. Chỉ khi 1–3 xong mới port cổng đó vào `KRegion.cpp:1018`.
   **Đánh đổi thị giác thật:** nhân vật có thể chồng lên nhau khi đứng đông.
   → **phải hỏi người dùng trước.**

### 6.5 Phương án dự phòng nếu bản gốc không cho câu trả lời rõ

Chỉ sửa cho **NPC không phải nhân vật của mình** (mô phỏng của họ chỉ mang tính thị giác,
máy chủ mới là bên có thẩm quyền):

- **A** — bị chặn thì **đừng `DoStand()`**, giữ nguyên hành động và bỏ qua tick đó, tick sau thử lại.
- **B** — nới hạn mức né (`m_nFindTimes > 1` → `> N`) cho nhân vật ở xa.
- **C** — không tính NPC là tường khi mô phỏng nhân vật ở xa.

Cả ba đều **chỉ động vào client**, hợp với việc máy chủ nằm ở máy khác.

---

## 7 · Tra cứu nhanh

```bash
# doc log sap game
cat /e/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/client/jx_crash.log

# kiem CRT truoc khi trien khai  (BAT BUOC)
python /d/GAMEDEVNEW/ReverseTools/re_pe_crt.py \
  E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/client

# ten ham may chu Linux
grep -i "<ten can tim>" /d/GAMEDEVNEW/ReverseTools/jx_linux_y.luamap.txt

# dich nguoc client goc
python /d/GAMEDEVNEW/ReverseTools/re_disasm.py \
  D:/ServerLinux/Patch/game_y_unpacked.bin 0x006429A0 22

# kiem binary da trien khai co chua thay doi khong
grep -q -F "<chuoi moi them>" /e/.../bin/client/Game.exe && echo DAT
```
