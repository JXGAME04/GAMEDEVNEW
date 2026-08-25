# BÀN GIAO 24/08 — Đo nhịp tick máy chủ + dọn luồng rác + bỏ I/O trên luồng chính

Phiên này làm 4 việc đã đề xuất sau khi đọc bản Linux và đo thật luồng của máy chủ đang chạy.
**Cây thi công: `D:\GAMEDEVNEW`** (đã xác minh: `GameServer.exe`, `CoreServer.dll`,
`CoreClient.dll`, `Game.exe` đang chạy đều mang CodeView trỏ về cây này; chỉ `heaven.dll`
là bản cũ 22/04 build từ cây E — không đụng tới).

---

## 0. Số liệu đo được TRƯỚC khi sửa (để so sánh sau buổi test)

Đo trực tiếp trên tiến trình đang chạy, máy 24 lõi logic:

```
GameServer.exe pid 25620 — 73 luồng:
    52 × heaven.dll+0x17F9B0     worker IOCP (Heaven.pdb KHỚP GUID+age; RVA = thread thunk CRT;
                                  công thức SocketServer.cpp:42-52 = cores*2+2 = 50)
    13 × CoreServer.dll+0x4C1AD0 (khớp THREAD_COUNT = 24/2 = 12, +1)
     5 × rainbow.dll
     1 × GameServer.exe          luồng chính

CPU trong 12 giây:  tổng 2,42s = 20% của MỘT lõi
    luồng chính  2,42s  = 100% lượng CPU của tiến trình
    72 luồng kia 0,00s  = 0%
```

Ghi log đang chạy trên luồng chính (đo 10 giây, máy chủ gần trống):

```
jx_auto_server.log   26,0 KB/giây      (~260 dòng/giây)
bot.log               1,8 KB/giây      — nhưng đã tích 128 MB sau ~1 giờ (~360 dòng/giây)
```

---

## 1. Bộ đo nhịp tick — `KPerfTick.h` / `KPerfTick.cpp` (MỚI)

Chia một tick (18 khung/giây = 55 ms) thành các giai đoạn và báo cáo định kỳ.

**Bật/tắt bằng `bin\server\config.ini`** (đã thêm sẵn, mặc định `On=1`):

```ini
[PerfLog]
On=1
IntervalSec=60
TreMs=55
Console=0
```

Khi `On=0`: chi phí bằng một phép so sánh biến static — không gọi đồng hồ.

**Kết quả ghi ra `bin\server\jx_perf_server.log`**, mỗi `IntervalSec` giây một khối:

```
[PERF] 20:15:03 tick=1080 tre=12 (1.1%) online=873 khoang=60.0s
   TICK         n=1080  tb=  18.40ms max=  142.70ms p95=  41ms chiem= 33.1%
   SW_ACTIVATE  n=1080  tb=  15.10ms max=  131.20ms p95=  36ms chiem= 27.2%
   SW_MSGLOOP   n=1080  tb=   1.20ms max=   14.00ms p95=   3ms chiem=  2.2%
   AUTOSAVE     n=1080  tb=   0.30ms max=    9.10ms p95=   1ms chiem=  0.5%
   KHOILUONG    region tb=412 max=980 | NPC tb=5130 max=11002  (khoi luong moi tick)
```

- `tre` = số tick vượt `TreMs` — **đây là con số quan trọng nhất**: nếu nó lớn thì máy chủ
  đang tụt nhịp, người chơi thấy giật.
- `KHOILUONG` để phân biệt **"chậm vì đông"** (thời gian tăng, khối lượng cũng tăng) với
  **"chậm vì lỗi thuật toán"** (thời gian tăng mà khối lượng không tăng). Không có nó thì chỉ đoán.

**Các điểm đã gắn** (`PERF_SCOPE`, RAII, không đổi luồng chạy):

| Giai đoạn | Vị trí |
|---|---|
| `TICK` | `CoreServerShell.cpp` — cả hàm `Breathe()` |
| `SCRIPT_TIME` | `CoreServerShell.cpp` — khối `if (pTimeScript)` |
| `GLBMISSION` | `CoreServerShell.cpp` — 4 nhịp `KJx2*_Breathe()` |
| `SW_MSGLOOP` / `SW_MAINLOOP` | `CoreServerShell.cpp` — `g_SubWorldSet.MessageLoop/MainLoop` |
| `SW_ACTIVATE` | `KSubWorldSet.cpp` — vòng `SubWorld[i].Activate()` |
| `AUTOSAVE` | `KSubWorldSet.cpp` — `PlayerSet.AutoSave()` |
| `BAUCUA` | `CoreServerShell.cpp` — `g_BauCua.run()` |
| `KHOILUONG` | `KSubWorldSet.cpp` + `KRegion.cpp` (cộng `npcCount` đã tính sẵn) |

Bộ đo `[SvPerf]` cũ (in mỗi 10 giây qua `PB_LogNgoai`) **giữ nguyên**, không đụng.

---

## 2. Bỏ ThreadPool chết trong `KCore.h`

`class ThreadPool` + `THREAD_COUNT` + `static ThreadPool pool(THREAD_COUNT)` đã bị **xoá**,
cùng `extern ThreadPool pool;` và biến chết `std::atomic<int> activeSubworlds` trong
`KSubWorldSet.cpp`.

Căn cứ:
- grep toàn repo: **không một lệnh `pool.enqueue()` / `pool.wait()` nào tồn tại** — chỉ có
  định nghĩa trong chính `KCore.h`.
- Đo thật: 13 luồng trong `CoreServer.dll`, **0,00s CPU trên 12 giây** — luồng rác.
- `static` đặt trong **header**: mỗi `.cpp` include `KCore.h` (**84 tệp**) về nguyên tắc sinh
  một pool riêng. Hiện chỉ 1 bản tồn tại, nhưng ai đó chỉ cần bắt đầu dùng `pool` ở vài tệp
  là số luồng nhân lên theo số tệp. Đây là mìn, bỏ hẳn cho chắc.

Các `#include <thread> <mutex> ...` **giữ nguyên** (không cần thiết phải gỡ, gỡ chỉ thêm rủi ro vỡ build).

Lợi ích: máy chủ bớt 12 luồng; **client cũng bớt 12 luồng** (`KCore.h` là tệp dùng chung).

---

## 3. Bỏ I/O đĩa nặng khỏi luồng chính

### 3.1 `pb_Log` (bot.log) — bỏ cặp `fopen`/`fclose` MỖI DÒNG

`KPlayerBot.cpp` — trước đây mỗi dòng log mở và đóng tệp một lần. Chính tác giả `g_AutoLog`
đã đo và ghi trong `KCore.cpp:806`: *"fopen/fclose mỗi dòng tốn 30-200us"*. Với ~360 dòng/giây
đo được hôm nay, đó là 10-70 ms mỗi giây tiêu trên luồng chính — và Tống Kim đông thì nhân lên.

Nay: **giữ `FILE*` mở sẵn, flush theo lô** (≥50 dòng hoặc ≥500 ms) — đúng cách `g_AutoLog`
đã làm và đã được chủ game chấp nhận. Xoay tệp 256 MB vẫn giữ, nhưng đóng tệp trước khi
`rename` (Windows khoá tệp đang mở) và dùng kích thước cộng dồn thay cho `ftell`/`fopen("rb")`.

Muốn tuyệt đối không mất dòng nào khi máy chủ tắt đột ngột: đặt `[BotLog] Flush=1` trong
`config.ini` → flush ngay mỗi dòng (vẫn rẻ hơn nhiều so với mở/đóng tệp mỗi dòng).

### 3.2 `g_AutoLog` (jx_auto_server.log) — bỏ `ftell()` mỗi dòng

`KCore.cpp` — hàm này đã giữ tệp mở sẵn, nhưng vẫn gọi `ftell(pLog)` mỗi dòng chỉ để biết
khi nào xoay tệp. `ftell` lấy khoá CRT của stream và đồng bộ vị trí với hệ điều hành.
Nay dùng **kích thước cộng dồn** từ giá trị trả về của `fprintf`; `ftell` chỉ gọi một lần lúc mở.

Hành vi (định dạng dòng, ngưỡng xoay 64 MB, bảng lọc nhãn) **giữ nguyên từng chữ**.

---

## 4. Tối ưu thuật toán trong luồng chính

Mọi mục dưới đây đã qua một vòng phản biện độc lập (mỗi phát hiện được một tác tử khác
đọc lại mã và ra phán quyết GIỮ / BỎ / SỬA LẠI). **4 đề xuất bị chặn** vì nguy hiểm —
xem mục 6.2.

### 4.1 Bỏ vòng quét region của ~900 bản đồ đang trống — khoản lớn nhất

`KSubWorld::Activate()` quét tuyến tính **toàn bộ** mảng region của **mọi** bản đồ mỗi khung,
kể cả bản đồ không có một ai. Đo thật: `WorldSet_GameServer.ini` có **Count=909** bản đồ nạp sẵn,
trung bình ~370-500 region/bản đồ → **~334.000 lần đọc `m_nActive` mỗi khung ≈ 6 triệu lần/giây**,
mỗi lần nhảy qua một `KRegion` **lớn hơn 6 KB** nên gần như chắc chắn trượt cache và trượt TLB.

Nay: bản đồ nào vừa quét mà không có region nào hoạt động thì **"ngủ" tối đa 9 khung (0,5 giây)**
rồi tự quét lại; và được **đánh thức ngay** khi có người vào qua `KSubWorld::AddPlayer`.

Hai lớp lưới an toàn:
- `AddPlayer` là điểm **duy nhất** làm `m_nActive` tăng — đã kiểm: đoạn tăng trong
  `PlayerChangeRegion` nằm gọn trong khối chú thích `/* */` (KSubWorld.cpp:2423-2458) nên
  chưa từng chạy, và `PlayerChangeRegion` gọi thẳng `AddPlayer`.
- Kể cả nếu sót một đường đánh thức nào đó, bản đồ tự quét lại sau 0,5 giây — **không thể kẹt vĩnh viễn**.

**Bọc `#ifdef _SERVER`, nhánh client giữ nguyên từng chữ.** Đây là điều phản biện chặn được:
trên client `KRegion::IsActive()` **luôn trả TRUE** (KRegion.h:116-123) và `m_nActive` vĩnh viễn = 0
(cả hai nơi gọi `AddPlayer` đều trong `#ifdef _SERVER`) — bản vá gốc không bọc `#ifdef` sẽ làm
client **ngừng Activate toàn bộ region**: NPC, vật thể, đạn, người chơi đứng im.

Và **không thêm trường vào lớp `KSubWorld`** (đổi `sizeof` là đổi ABI qua ranh giới
CoreClient.dll ↔ Game.exe) — dùng một mảng `static` ở phạm vi tệp, chỉ số lấy bằng `this - SubWorld`.

### 4.2 Xoá nhánh đồng bộ chết trong `KRegion::Activate`

```c
if ((nCounter == m_nNpcSyncCounter / 2) && (m_nNpcSyncCounter & 1))   // LUÔN SAI
```
`m_nNpcSyncCounter` xuất hiện **đúng 3 lần** trong toàn bộ mã nguồn: khai báo (KRegion.h:60),
gán 0 trong ctor (KRegion.cpp:35), và đọc ở điều kiện này. **Không nơi nào tăng nó** → `(0 & 1)` = 0
→ khối `NormalSync` bên trong **chưa từng chạy một lần nào**. Đối chiếu: `m_nObjSyncCounter` thì
*có* được tăng — chứng tỏ đây là mã bỏ quên. Bỏ đi = bớt một phép chia + một so sánh cho
**mỗi NPC mỗi khung**. Việc đồng bộ thật sự vẫn do vòng "chunk" (5 NPC/khung) đảm nhiệm.

### 4.3 Bỏ phép chia dư khỏi vòng duyệt từng NPC

`int syncIndex = (m_nNpcSyncCursor + synced) % npcCount;` chạy cho **mỗi NPC mỗi khung**
(idiv ~20-40 chu kỳ, không pipeline được) dù cả vòng chỉ dùng tối đa **5** giá trị — `m_nNpcSyncCursor`
không đổi trong vòng (chỉ cập nhật sau vòng) và `npcCount` lấy một lần. Nay tính sẵn `aSyncIdx[5]`
trước vòng.

### 4.4 Các mục nhỏ, kết quả tính ra y hệt

| Sửa | Vị trí | Vì sao |
|---|---|---|
| `POINT POff[8]` → `static const` (**18 chỗ**) | KNpc.cpp | Mảng hằng 8 phần tử dựng lại trên stack mỗi lần gọi; có chỗ nằm trong `NormalSync` (5 NPC/region/khung). Đã kiểm mọi cách dùng đều chỉ đọc `POff[i].x/.y` |
| `const char* FactionName[]` → `static const char* const` | `KNpc::UpdateGameTitle` | 10 con trỏ ghi vào stack mỗi lần đồng bộ một người chơi |
| Thu hẹp `int Map = SubWorld[...].m_SubWorldID` | KNpc::Activate | Mọi NPC (kể cả quái — đa số) đều đọc một ô trong mảng `KSubWorld` rất lớn rồi vứt; `Map` chỉ dùng trong nhánh người-chơi + `_SERVER` + map PUBG |
| `pOrdinSkill1 = pOrdinSkill` | KNpc::ProcessState | `GetSkill(m_ActiveAuraID, nCurLevel)` gọi hai lần cùng tham số; hàm chỉ là tra bảng |
| Nâng `nRangeX*nRangeX` và `i*i` ra ngoài vòng | `KNpcAI::GetNearestNpc`, `GetNpcNumber` | Tính lại ở **từng ô** của vùng quét vuông |

### 4.5 Bỏ `ftell()` mỗi dòng trong `g_AutoLog` — xem mục 3.2.

---

## 5. Build

| Cấu hình | Kết quả |
|---|---|
| `Server Release \| x64` (Core → CoreServer.dll) | COMPILE PASS 0 lỗi, LINK PASS |
| `Client Release \| Win32` (Core → CoreClient.dll) | COMPILE PASS 0 lỗi, LINK PASS |

(`KCore.h`, `KSubWorldSet.cpp`, `KRegion.cpp` là tệp dùng chung client+server nên bắt buộc
build cả hai — đã làm.)

---

## 6. Cảnh báo trước khi triển khai

### 6.1 🔴🔴 DLL đã đặt sẵn CHỨA CẢ công việc đang dở của một phiên song song

`bin\server\CoreServer.dll.moi_2408_perftick` (19:35) build từ **working tree** của
`D:\GAMEDEVNEW`, mà lúc build cây này đang có thay đổi **chưa commit của một phiên khác**
đang chạy đồng thời:

| Tệp | Giờ sửa | Của ai |
|---|---|---|
| `Sources/Core/Src/ipc_shared.h` | 19:24 | phiên khác (Liên đấu / WAuto) |
| `Sources/Core/Src/KLienDauTables.h` | 19:25 | phiên khác |
| `Sources/Core/Src/KPlayer.h` | 19:26 | phiên khác |
| `Sources/Core/Src/KProtocolProcess.cpp` | 19:29 | phiên khác |
| `Sources/Core/Src/CoreShell.cpp` | 19:30 | phiên khác |
| `WAutoUI/*` | 19:36 | phiên khác |
| `Sources/Core/Src/KMissle.cpp`, `KMissleSet.cpp`, `KRegion.h` | 18:00-18:40 | đợt vá đạn 24/08 |

Ngoài ra `bin\server\CoreServer.dll` hiện tại trên đĩa (18:47) **không phải bản server đang
chạy**: nó do phiên vá đạn swap vào từ worktree `D:\GAMEDEVNEW\_wt_missle2408` (worktree này
đã bị xoá, commit `faeee99d` đã vào `main`). Máy chủ vẫn đang chạy bản nạp lúc 17:47.

**Vì vậy KHÔNG tự swap.** Trước buổi test Tống Kim, người vận hành cần chọn một trong hai:
- **An toàn nhất**: đợi phiên song song chốt xong, rồi build lại một bản sạch từ `main` đã commit.
- **Nhanh**: dùng bản `.moi_2408_perftick` và chấp nhận nó mang theo cả phần Liên đấu/WAuto đang dở.

### 6.2 Bốn đề xuất đã bị phản biện CHẶN — đừng làm lại

| Đề xuất | Vì sao bị chặn |
|---|---|
| Thêm `m_nTongActive` vào lớp `KSubWorld` | Đổi `sizeof` struct qua ranh giới CoreClient.dll ↔ Game.exe (Gate: cấm đụng bố cục struct). Bản đã làm dùng mảng `static` ở phạm vi tệp thay thế |
| Chặn vòng quét region mà không bọc `#ifdef _SERVER` | Client luôn `IsActive()==TRUE` và `m_nActive` vĩnh viễn 0 → client **ngừng Activate toàn bộ region**, NPC/vật thể/đạn/người chơi đứng im |
| Thêm `m_nNpcCount`/`m_nObjCount` vào `KRegion` thay cho `GetNodeCount()` | Vừa đổi ABI vừa **chắc chắn trôi bộ đếm**: `KRegion::Close` gỡ hàng loạt phần tử không qua `RemoveObj` |
| Bộ nhớ đệm danh hiệu trong `KNpc` | Đổi ABI **và** có hai đường ghi `m_szGameTitle` vượt mặt bộ đệm → mất danh hiệu trên đầu nhân vật |

### 6.3 Ba lỗi thật đã tìm ra nhưng CỐ Ý KHÔNG sửa (cần chủ game quyết)

1. **`m_cDeathCalcExp.Active()` bị gọi HAI LẦN trong một lần `KNpc::Activate`**
   (KNpc.cpp:605-607 và :647-657). Hàm này **giảm bộ đếm** `m_nTime--`, nên cửa sổ quy công
   sát thương đang bị rút **còn một nửa** (1200 tick → 600). Bỏ một lời gọi là **đổi cân bằng**:
   ai được tính là người hạ quái → đổi chia kinh nghiệm và quyền rơi đồ. Phải làm có chủ đích
   và đo lại, không gộp vào đợt "không đổi hành vi".

2. **`BYTE nTaskCount` tràn** — `Lib/S3DBInterface.h:158` khai báo `BYTE`, trong khi
   `MAX_TASK = 4200` và `KPlayerDBFuns.cpp` gán `pRoleData->nTaskCount = nTaskCount;` với
   `nTaskCount` là `int`. Nhân vật có ≥ 256 nhiệm vụ đã lưu sẽ bị cắt còn `n & 0xFF`; đúng 256
   thì `nTaskCount == 0` và đường nạp `return 1` ngay — **mất toàn bộ nhiệm vụ**. Sửa là đụng
   **định dạng lưu / struct DB** (Gate 2) nên chỉ báo cáo. Nên đếm thử số task thật của vài
   nhân vật lâu năm để biết đã chạm ngưỡng chưa.

3. **Blob lưu nhân vật có thể vượt bộ đệm `CPackager` 128 KB** → `CBuffer::AddData` ném
   `CException` mà `MainLoop()` không có `try` bao → sập GameServer. Dòng nâng lên 655360
   đang bị chú thích ở `Buffer.h:179`.

### 6.4 Bản vá đã làm nhưng CHƯA triển khai được

`Sources/Engine/Src/KLuaScript.cpp` — vá `fopen` không kiểm NULL (sập khi ổ đầy / tệp bị khoá).
Bản vá nằm trong nguồn nhưng **không đi vào đợt này**: `bin\server\engine.dll` đang chạy là bản
**22/04 build từ cây E, cấu hình Debug**, không phải từ cây D. Muốn có bản vá này phải build
engine từ đúng cây đã sinh ra nó — nên làm thành một đợt riêng, không gộp vào buổi test.

### 6.5 Cách đọc kết quả sau buổi test

```bash
python D:\GAMEDEVNEW\ReverseTools\perf_read.py "E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\jx_perf_server.log"
```

Công cụ tự trả lời ba câu: máy chủ **có tụt nhịp không** (% tick trễ), **giai đoạn nào ăn thời gian**,
và **chậm vì đông hay vì lỗi thuật toán** (so tỉ lệ tăng thời gian với tỉ lệ tăng khối lượng).
