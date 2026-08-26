# DI CHUYỂN: GIỰT LÙI / TRƯỢT / TỐC BIẾN — BÀN GIAO CHO PHIÊN SAU (25/08/2026)

> **Trạng thái: MỚI ĐẶT LOG, CHƯA SỬA MỘT DÒNG LOGIC NÀO.**
> Đã thả `CoreClient.dll` **`61b8bff8`** (lùi: `.cu_2508_truoc_S6_0a62cb0d`). `CoreServer.dll` giữ nguyên
> `d44b1233`. Commit `59875e48`.

---

## 1. Ba triệu chứng chủ game báo

| # | Mô tả nguyên văn | Ghi chú |
|---|---|---|
| **A** | *"bot lúc mới di chuyển tới thì thấy nó giựt nhảy từng toạ độ rồi biến mất rồi xuất hiện lại tại toạ độ cũ"* | và *"quái (kind_normal) hay chạy giựt lùi"* |
| **B** | *"nick tôi đang chơi chết về lại vị trí hồi sinh hoặc phù về thành thì bị nhảy vài toạ độ bậy"* | như setpos nhảy lung tung |
| **C** | *"Tống Kim hay xảy ra nick tôi đang chơi đứng đánh vào không khí, không có ai cả"* | không có bot hay NPC gì hết |

---

## 2. 🔴 ĐỌC TRƯỚC: những số tôi đã RÚT LẠI

Phiên này đưa ra một loạt số rồi **tự bác bỏ**. Đừng dùng lại chúng.

Mọi nhãn di chuyển hiện có đều là **`AUTOLOG_EVERY`** — tiết chế theo đồng hồ, **một biến `static` cho MỖI
ĐIỂM GỌI**, dùng chung cho hàng trăm NPC, **không lọc tên**:

```
NET-RUN / NET-WALK                              AUTOLOG_EVERY(300)
NET-RMNPC / SYNCNPC-ADDFAIL / SYNCMIN-DRIFT     AUTOLOG_EVERY(1000)
```

| Kết luận đã đưa ra | Trạng thái |
|---|---|
| "**73,5%** NPC mới không nhận lệnh di chuyển trong 6 giây đầu" | ❌ **SAI** — suy từ **sự vắng mặt** của nhãn tiết chế |
| "bảng NPC client **luôn đầy**, 6.215 lần khe đổi chủ, 0,41 thêm/giây" | ❌ **Không đứng vững** — số đếm của nhãn tiết chế |
| "124 lần `AddNpcSet2` trả 0 = hết khe" | ⚠️ Có thật (mỗi dòng tự chứa `idx=0`) nhưng là **cận dưới**, không phải tần suất |

**Luật rút ra (đã ghi ở `BANGIAO_DANH_MISS_TOANTAP.md` mục 6, phiên này vẫn vấp lại):**
> Chỉ được suy luận từ **sự vắng mặt** với nhãn `AUTOLOG_IDX` (lọc tên, không tiết chế).
> Nhãn `AUTOLOG_EVERY` **chỉ dùng khi nó CÓ MẶT**, và chỉ dùng để so **tỷ lệ giữa các nhóm**
> (lấy mẫu theo thời gian không thiên vị nhóm nào), **không bao giờ** làm tần suất tuyệt đối.

---

## 3. ✅ Những gì ĐO ĐƯỢC và VẪN ĐỨNG VỮNG

Đều là **tỷ lệ giữa các nhóm** hoặc **số học từng dòng** — hai dạng hợp lệ với nhãn tiết chế.

**3.1 Lệch vị trí (client so với server), tách theo loại NPC** — 6.588 dòng `SYNCMIN-DRIFT`:

| Loại | dòng | lệch > 1 ô | lệch > 4 ô | p90 | max |
|---|---|---|---|---|---|
| **người chơi / bot** | 2.987 | **27,0%** | **8,4%** | **106 mps** | **1.014 mps (32 ô)** |
| dialoger | 2.314 | 0% | 0% | 0 | 0 |
| **quái** | 1.224 | **1,0%** | 0,1% | 0 | 155 |
| bird | 63 | 11,1% | 0% | 8 | 50 |

**3.2 Xoá NPC khỏi bảng client gần như CHỈ xảy ra với `kind_player`**: `NET-RMNPC` 1.333/1.334 là kind=1,
**quái 0 lần**. ⇒ phần *"biến mất rồi hiện lại"* chắc chắn không phải quái.

**3.3 Nhân vật của mình (`SYNCME-DRIFT`)**: p50 = 11, p90 = **83**, p99 = 181, **max = 522 mps (16 ô)**;
**28,8%** số dòng lệch > 1 ô.

**3.4 Số NPC khác nhau trong cửa sổ 10 giây**: p50 = 66, p90 = 127, max = 163 — **trần là 256**.
⚠️ Đây là **cận dưới** (NPC không sinh dòng log thì không đếm được), nhưng đủ để **không được phép**
vá `MAX_NPC` dựa trên giả thuyết "bảng đầy" khi chưa có `[S6-ADD]/[S6-DEL]`.

**3.5 Nhịp gói đồng bộ vị trí của một NPC**: p50 = **1,10 giây**. ⇒ NPC nào không có lệnh di chuyển thì
**chỉ có thể nhảy mỗi 1,1 giây một lần** — khớp mô tả "giựt nhảy từng toạ độ", nhưng **chưa chứng minh
được có bao nhiêu NPC rơi vào cảnh đó** (đó chính là việc của `[S6-CMD]`).

---

## 4. Bộ log `[S6-*]` vừa đặt — mỗi nhãn trả lời một câu hỏi

Tất cả đều `#ifndef _SERVER`, đều `AUTOLOG` (**không tiết chế**), tắt cùng `Config.ini [Client] AutoLog=0`.

| Nhãn | Ở đâu | Trả lời câu hỏi |
|---|---|---|
| `[S6-ADD]` | `KProtocolProcess.cpp` ~1859 | Thêm NPC. **`idx=0` = KHÔNG thêm được.** Kèm `dung=<n>/256` |
| `[S6-DEL]` | ~652 | Xoá NPC, kèm `dung=<n>/256` |
| `[S6-CMD]` | `NetCommandRun` / `NetCommandWalk`, đặt **TRƯỚC** cửa chặn `ConformIdx` | **`ap=0` = client chưa có NPC đó nên lệnh di chuyển BỊ VỨT** |
| `[S6-SYNC]` | cạnh `SYNCMIN-DRIFT` | Bản không tiết chế, thêm **`nan=1/0`** = client có ghi đè toạ độ hay không |
| `[S6-ME]` | `SyncNpcMinPlayer` | Ghi rõ rẽ **nhánh nào**: `vaolandau` / `loadmap` / **`GIUNGUYEN`** |
| `[S6-ATK]` | `KProtocol.cpp` `SendClientCmdSkill` | Đánh theo id mục tiêu thì **`thay=0` = client KHÔNG có mục tiêu đó** |

Kèm `S6_UsedSlots()` (đặt ngay sau khối `#include`, **trước** mọi điểm gọi) quét 256 ô đếm khe đang dùng.

> ⚠️ `[S6-CMD]` và `[S6-SYNC]` là hai nhãn **tải cao** (ước lượng 250–450 dòng/giây ở bãi đông).
> Trần cứng là **1.200 dòng/giây** (`KCore.cpp:791`). **Việc đầu tiên khi lấy log: `grep -c "bo qua .* dong"`.**
> Nếu > 0 thì số liệu đã mất mẫu — phải giảm tải (ví dụ chỉ ghi `[S6-SYNC]` khi `nan=1` hoặc khi lệch > 1 ô)
> rồi đo lại.

---

## 5. Quy trình cho phiên sau

### 5.1 Lấy mẫu

1. Kiểm `bin\client\Config.ini` → `AutoLog=1`; **xoá `jx_auto.log`** trước khi vào game.
2. Xác nhận client đang chạy `CoreClient.dll` md5 **`61b8bff8`** (nếu phiên khác đã thay thì thả lại từ
   `D:\GAMEDEVNEW\Sources\Core\ClientRelease\CoreClient.dll`).
3. Chơi ~5 phút ở **bãi đông bot**, trong đó:
   - đứng yên nhìn bot chạy tới (triệu chứng **A**),
   - **chết một lần** và **dùng phù về thành một lần** (triệu chứng **B**),
   - nếu vào được **Tống Kim** thì để log chạy nguyên trận (triệu chứng **C**).

### 5.2 Ba phép đo, ba nhánh kết luận

**A — bot giựt / biến mất**

```
grep -c "bo qua .* dong"                     # PHAI = 0
grep "\[S6-ADD\]" | grep -c "idx=0"          # bao nhieu lan KHONG them duoc
grep -o "dung=[0-9]*" | sort -n | tail -1    # khe dung CAO NHAT tren 256
grep "\[S6-CMD\]" | grep -c "ap=0"           # lenh di chuyen bi vut
```
- `dung=` chạm gần **256** hoặc `idx=0` nhiều ⇒ **bảng NPC client hết khe**. Hướng: nâng `MAX_NPC`
  (xem mục 6 để biết những chỗ phải kiểm trước) hoặc giảm mật độ bot.
- `dung=` còn dư nhiều mà **`ap=0` chiếm tỷ lệ lớn** ⇒ **NPC mới không nhận được lệnh di chuyển**.
  Hướng vá (KHÔNG đụng giao thức): trong `KProtocolProcess::NpcRequestCommand`
  (`KProtocolProcess.cpp:5016`) sau `NpcSet.SyncNpc(...)`, **gửi thêm lệnh di chuyển hiện tại của NPC đó
  cho riêng client vừa xin** bằng chính gói `do_run`/`do_walk` đã có (`g_pServer->PackDataToClient`).
  Lý do: `NPC_NORMAL_SYNC` (`KProtocol.h:497-524`) **chỉ có `Doing`, KHÔNG có đích đến** — client biết
  NPC "đang chạy" nhưng không biết chạy đi đâu nên chỉ nhảy theo từng gói.
- Cả hai đều thấp ⇒ nguyên nhân nằm ở **lớp nắn vị trí**, xem `[S6-SYNC] nan=1` và mục 6.2.

**B — chết/phù về thành nhảy toạ độ bậy**

```
grep "\[S6-ME\]" | grep -c "nhanh=GIUNGUYEN"
grep "\[S6-ME\]" | grep "nhanh=GIUNGUYEN"   # xem cl= va sv= cach nhau bao xa
```
Nếu quanh mốc chết/dùng phù thấy `nhanh=GIUNGUYEN` với `cl=` và `sv=` cách nhau vài ô ⇒ **đã chốt**:
`SyncNpcMinPlayer` (`KProtocolProcess.cpp:2068`) chỉ nắn vị trí nhân vật mình ở **hai** nhánh
(`m_RegionIndex == -1` và `nRegion == -1` → `LoadMap`); điểm đến nằm trong region **đã nạp** thì rơi
xuống nhánh ba — **chỉ kiểm vật cản, không sửa toạ độ**.
⚠️ **Đừng vội bỏ nhánh đó**: memory dự án ghi rõ việc **không nắn là CÓ Ý** (client-side prediction), và
đề xuất "nắn vị trí nhân vật trong `SyncNpcMinPlayer`" **đã từng bị phản biện bác bỏ** vì gây giật
rubber-band 4 ô mỗi lần sync. Hướng đúng có lẽ là **chỉ nắn khi lệch lớn** (ví dụ > 2 ô) hoặc **chỉ nắn
ngay sau sự kiện hồi sinh / dịch chuyển**, không nắn liên tục.

**C — Tống Kim đánh vào không khí**

```
grep "\[S6-ATK\]" | grep -c "thay=0"
grep "\[S6-ATK\]" | awk '{...}'   # ty le thay=0 tren tong
```
`thay=0` nhiều ⇒ **client không có mục tiêu trong bảng NPC** nhưng vẫn gửi lệnh đánh theo id ⇒ đúng
triệu chứng. Khi đó ghép tiếp với `[S6-ADD]/[S6-DEL]` để biết mục tiêu **bị xoá** hay **chưa từng thêm được**.

---

## 6. Những chỗ mã đã đọc (đỡ mất công đọc lại)

### 6.1 Đường đồng bộ NPC phía client
- `KProtocolProcess.cpp:1817-1890` — `SyncNpc` (gói đầy đủ): `Mps2Map` → `if (nRegion == -1) return;`
  → `SearchID` → `AddNpcSet2` → đặt vị trí. **`SYNCNPC-ADDFAIL` là tên SAI** — nhãn in **ngay sau**
  `AddNpcSet2`, thực chất là "NPC mới được thêm".
- `KProtocolProcess.cpp:1930-2030` — `SyncNpcMin` (gói định kỳ). Client **chỉ** ghi đè toạ độ khi
  `Npc[].m_nNeedFixPos > 0` **và** cùng region (`:2000`). Chú thích `:1993-1998` nói rõ đây là **cố ý**
  theo bản gốc JX2.
- `KNpc.cpp:4566` `KNpc::ServeMove`, nhánh `#ifndef _SERVER` mở tại `:4600`:
  `nRet == 1` → `m_nNeedFixPos = 0`; `nRet == 0` → `m_nNeedFixPos++` + `DoStandBlocked()`.
  ⇒ **đang chạy trơn thì client không bao giờ được nắn**; sai số chỉ xả khi bị chặn hoặc đổi region.

### 6.2 Cần kiểm nếu nghi lớp vẽ/nội suy
- `CoreShell.cpp:2539-2542`: `s_InterpFrom/s_InterpTo/s_InterpNpcID/s_InterpValid[MAX_NPC]`.
- Tìm `PAINT_INTERP_SNAP_DIST` — ngưỡng bỏ nội suy khi lệch lớn.

### 6.3 Nếu định nâng `MAX_NPC` (256 → 512/1024) thì PHẢI kiểm trước
- `KNpc.h:21-23`: server 98000 / client **256**.
- `KNpc.h:892` `extern CORE_API KNpc Npc[MAX_NPC];` — **mảng dữ liệu XUẤT KHẨU** của CoreClient.dll.
- **Mảng song song cùng cỡ**: `CoreShell.cpp:2539-2542` (4 mảng), `KPlayer.cpp:9464/9465/9535/9582`
  — **`int aryNpc[MAX_NPC][2]` là mảng TRÊN NGĂN XẾP**; 256 → 2 KB, 1024 → 8 KB mỗi cái, có 4 cái.
  Kiểm xem chúng có nằm trong `#ifndef _SERVER` không (phía server `MAX_NPC = 98000` thì 784 KB/mảng).
- **`Engine/Src/LuaFuns.cpp:243/249/263/269/284/290/325/331` dùng `MAX_NPC`** ⇒ `engine.dll` biên dịch
  **bản sao riêng**. Nâng ở `KNpc.h` mà chỉ build lại CoreClient thì engine.dll vẫn quét 256
  (không sập, nhưng không thấy phần dôi ra) ⇒ **phải build lại cả `engine.dll`**, mà theo memory dự án
  `engine.dll` đang chạy được build ở cấu hình **`Debug|Win32`**, build nhầm `Release` gây
  `_CrtIsValidHeapPointer` lúc đăng nhập.

---

## 7. Bẫy vận hành (đã va phải trong phiên này)

- **Hai phiên Claude cùng làm trên `D:\GAMEDEVNEW` và cùng thả DLL vào một `bin\`.** Trong phiên này
  `bin\client\CoreClient.dll` bị thay 3 lần bởi phiên khác. **Luôn `md5` trước và sau khi thay**, và
  `git add` **chỉ đích danh tệp mình sửa**.
- **So mốc thời gian `.obj`/`.dll` với mốc sửa nguồn** trước khi thả — đã suýt thả bản client dịch từ
  trạng thái nguồn trung gian.
- Cây vận hành `E:\SourceTuanLe\...\SOURCESUPDATE_KINHMACH_ONLTEST0608` **không áp được** bộ `[S6-*]`
  (cây đó không có các nhãn log gốc). Đây là log tạm — **gỡ sau khi tìm ra gốc**.
- Nguồn là **ANSI/TCVN3 + GBK** ⇒ mọi sửa đổi qua `safe_edit.py`, kiểm `high bytes` không đổi và `FFFD = 0`.

---

## 8. Trạng thái binary

| | md5 | Ghi chú |
|---|---|---|
| `bin\client\CoreClient.dll` | **`61b8bff8`** | có `[S6-*]`; lùi: `.cu_2508_truoc_S6_0a62cb0d` |
| `bin\server\CoreServer.dll` | `d44b1233` | **giữ nguyên** — mọi mã S6 đều `#ifndef _SERVER`; bản này đã có vá `dist=0` và **đang chạy** |

Client phải **thoát vào lại** thì mới nạp DLL mới. Máy chủ **không cần** restart cho bộ log này.
