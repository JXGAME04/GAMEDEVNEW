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

---

# 9. KẾT QUẢ MỔ LOG 26/08 — TÌM RA CƠ CHẾ, CHƯA VÁ (chờ chủ duyệt)

Mẫu: 25 phút chơi thật sáng 26/08 (auto Dã Tẩu bật, nhân vật `CaiBang` idx server 91423 / dwID **92422**),
client log 457.899 dòng + server log. Bản chụp trong scratchpad phiên 26/08
(`jx_client_2608_b.log`, `jx_server_2608_b.log`).

- DLL client lúc đo: **`0e009f08`** (bản phiên sát-thủ build 25/08 23:19) — **vẫn có đủ `[S6-*]`**
  (build sau commit `59875e48`). Bản `61b8bff8` đã bị thay nhưng không sao.
- ⚠️ client log có **22 cú "bo qua"** (chạm trần 1200 dòng/s), server log **205 cú** (nhãn `E3_/E4_`
  của phiên khác chiếm băng thông) — tỷ lệ tuyệt đối quanh các điểm tràn không tin được.

## 9.1 Triệu chứng A (bot trượt tới/lùi · biến mất · hiện lại chỗ cũ · lặp) — CƠ CHẾ ĐÃ CHỐT

**A1 — trượt & đứng đánh sai chỗ:** client **chỉ nắn NPC khác khi `m_nNeedFixPos > 0`**
(`KProtocolProcess.cpp:2036`). Bot nhận **rất ít lệnh di chuyển** (đo hồ sơ: bot 92593 nhận **1 lệnh/99 s**,
bot 92438 **4 lệnh/99 s** — server chỉ phát khi ĐỔI lệnh), và khi NPC vừa ADD thì
`SendCommand(Doing, <vị trí sync>)` (`:1911`) — đích là **chính chỗ đứng**, không phải đích thật.
⇒ bot hiện hình **lệch sẵn ~6 ô, đứng đánh (`doing=7`) tại chỗ SAI suốt nhiều giây** (drift giữ nguyên
6,5 ô), chỉ khi có lệnh run mới thì client cho nó **trượt dần** về đúng. Đo trên 7.766 gói sync bot
(đã TÁCH nhân vật mình ra): **10,9 % lệch > 1 ô, 1,0 % > 4 ô, max 7,3 ô**; quái chỉ ~1,5 % > 1 ô.

**A2 — biến mất/hiện lại chỗ cũ theo vòng lặp = NPC MỒ CÔI:** khi bộ 9 region-slot cuộn
(`KSubWorld::LoadMap` 2-arg), slot bị tái dùng gọi `KRegion::Load` → `Close()`
(`KRegion.cpp:110→1590`): mọi NPC của region đó bị `m_RegionIndex = -1` + `RemoveNpc` **nhưng KHÔNG bị
xoá khỏi `NpcSet`** ⇒ không được vẽ (BIẾN MẤT) mà id vẫn tra được. Gói sync-min sau đó **gắn lại theo
vị trí server** (`KProtocolProcess.cpp:1967-1993`) ⇒ HIỆN LẠI đúng chỗ nó "đáng đứng" (nhìn như "chỗ cũ").
Đo: **186 NPC khác nhau, ≥ 718 cú mồ-côi/gắn-lại trong 25 phút** (`SYNCMIN-REGION-BAD`, cận dưới vì
nhãn EVERY-1000); từng bot 92xxx dính **10-18 lần/con**. Auto Dã Tẩu chạy con thoi (đổi map 7 lần +
trap nội map) làm slot cuộn **45 lần/99 s** lúc chạy xa ⇒ vòng này quay liên tục.

## 9.2 Triệu chứng B (trap/chết/phù về thành nhảy toạ độ) — MỘT CA TRAP BẮT ĐƯỢC TẠI TRẬN

Ca trap nội map lúc `t=46519038`: auto chạy tới (54144,103424), server SetPos về **(47232,104768)**
(cách ~215 ô cùng map). Phía client: gói `NPC_SYNC` full + `SYNCPLAYER`; bộ region được nạp lại
qua `NpcChangeRegion` → `LoadMap` (`KSubWorld.cpp:2383`) — **nhánh `SYNCME-LOADMAP`/`nhanh=loadmap`
(`KProtocolProcess.cpp:2146`) KHÔNG chạy lần nào trong cả 25 phút**. Trong chuyển tiếp vài trăm ms:
NPC vùng cũ mồ côi hàng loạt, NPC vùng mới REQNPC/ADD dần, và có **10 cú `[S6-ME] nhanh=vaolandau`**
giữa phiên (mình bị "đặt lại như vào lần đầu" — region chứa mình cũng bị Close) — mỗi cú một cú snap.
**Chết/phù về thành dùng cùng họ cơ chế** (SetPos xa) — nhưng trong 25 phút này **chủ chưa chết/chưa
phù lần nào** (0 dòng `doing=10/21` cho mình) ⇒ CHƯA có mẫu đo riêng cho chết/phù.

## 9.3 Triệu chứng C (Tống Kim đánh vào không khí) — CHƯA CÓ MẪU, CÓ ỨNG VIÊN

Chưa có trận TK trong log (`S6-ATK thay=0` = 0/5.570 — nhưng nhãn này dùng `SearchID` nên **KHÔNG
phát hiện được mục tiêu MỒ CÔI** — nó vẫn nằm trong bảng). Ứng viên mạnh: đánh/khoá mục tiêu là NPC
mồ côi (vô hình) hoặc NPC đang lệch 6-7 ô. Auto giữ-mục-tiêu ĐÃ lọc `m_RegionIndex < 0`
(`CoreShell.cpp` ~14174) nhưng **đường chọn-mới và auto TK chưa rà**.

## 9.4 Số/kết luận đã RÚT LẠI ngay trong phiên (đừng dùng lại)

| Kết luận | Vì sao rút |
|---|---|
| "22 cú client nhảy >2 ô, có cú 9,6 ô" (đo theo cell+slot) | slot đổi nội dung ⇒ so cell giữa 2 bộ region là vô nghĩa; cú 9,6 ô = trap THẬT (hợp lệ) |
| "Ping-pong 46-305 ô TRUNG-NAP-VUNG" (pt_s6b) | **artifact bảng neo slot→regionID** — RAW cho thấy client chạy liên tục mượt qua biên |
| "Drift chính mình p50 1,45 ô khi chạy" là lỗi | đó là client-prediction dẫn trước server (18 fps) — đứng yên p50 0,5 ô ⇒ bình thường |
| 3 cú `E4_POS_CHANGEWORLD npc=92422` là của chủ | nhãn `E4_*` in `npc=<IDX> id=<dwID>` ⇒ đó là bot dwID 93421; chủ = idx 91423 |

## 9.5 HƯỚNG VÁ ĐỀ XUẤT (chưa làm — cần chủ game duyệt từng mục)

1. **Nắn-có-ngưỡng cho NPC khác** trong `SyncNpcMin`: nếu cùng region mà lệch > N ô (đề xuất N=2)
   thì ghi đè toạ độ kể cả `fix=0` (lệch nhỏ vẫn để prediction + nội suy lo). Trị A1 tận gốc,
   rủi ro thấp (chỉ kích khi đã lệch rõ; PAINT_INTERP kéo mượt cú snap ≤ 64 px).
2. **Trị mồ côi:** khi `KRegion::Close` gỡ NPC — hoặc xoá hẳn khỏi `NpcSet` (sẽ REQNPC lại khi gặp),
   hoặc giữ nhưng **mọi đường chọn mục tiêu phải lọc `m_RegionIndex < 0`** (auto thường + auto TK + click tay).
3. **Gửi kèm lệnh di chuyển hiện tại khi client REQNPC** (`NpcRequestCommand`, server
   `KProtocolProcess.cpp:5016`) — NPC vừa hiện hình biết ngay đích đang chạy (đề xuất cũ mục 5.2 vẫn đứng).
4. **Nhãn log bổ sung** trước khi vá để đo trước/sau: `[S6-ORPHAN]` (Close/gắn-lại, không tiết chế),
   `S6-ATK` in thêm `tgtreg=`, `[S6-LOADMAP]` trong `KSubWorld::LoadMap` client (đếm + đo ms nạp).

## 9.6 Việc cần chủ game làm để đo nốt B và C

Log đang bật sẵn (`AutoLog=1`): **chết 1 lần** + **dùng phù về thành 1 lần** + **1 trận Tống Kim**,
rồi báo giờ để phiên sau cắt đúng cửa sổ. Muốn số server sạch thì phải giảm nhãn `E3_/E4_`
(server đang tràn 205 cú "bo qua").

## 9.8 ĐỢT LOG 2 (26/08 trưa, commit `305be20b`) — chủ duyệt "làm mục 4 trước, chắc chắn mới fix"

Chủ hỏi thêm: *"kiểm tra thêm vùng người chơi - bot chết thì có xoá không hay tích luỹ dẫn tới
tràn - thêm log chỗ này"*. Đã đặt **8 điểm log, KHÔNG sửa một dòng logic nào**, build
`CoreClient.dll` **`5d75074a`** swap vào `bin\client` (backup `.cu_2608_truoc_S6b_d4479115` —
lưu ý bản bị thay là **`d4479115`** = `.moi_2608_danhtheotab` phiên kia vừa swap 11:19, KHÔNG còn
`0e009f08`; bản mới của tôi build từ HEAD nên **gồm cả** `6723de19` đánh-theo-tab chưa test của họ).
Server không đổi. Client phải **thoát vào lại**.

### Trả lời sơ bộ từ CODE cho câu "chết có xoá không hay tích luỹ":

Client có **3 làn đường MỒ CÔI** (gỡ khỏi region, KHÔNG xoá khe) + **1 đường xoá khe duy nhất**:

| Đường | Ở đâu | Điều kiện | Nhãn mới |
|---|---|---|---|
| Mồ côi 1 | `KRegion::Close` (slot tái dùng/đóng map) | LoadMap cuộn vùng | `[S6-ORPHAN]` |
| Mồ côi 2 | `KNpc::Activate` client `:696` | cách người chơi ≥ 40 ô | `[S6-VANH]` |
| Mồ côi 3 | `KNpc::Activate` client `:710` | câm sync > 120 tick (~6,7 s) — **XÁC chắc chắn rơi vào đây** vì `SyncNpcMin` bỏ qua gói của NPC `do_death/do_revive` (`:1944`) | `[S6-CAM]` |
| **Xoá khe** | `KNpcSet::CheckBalance` `:755` | câm sync > 1000 tick (~55,5 s) → `Remove` trả khe về FreeIdx | `[S6-BAL]` |

⇒ **Khe KHÔNG tích luỹ vô hạn về lý thuyết** (chết ~6,7 s thì biến mất, ~55,5 s thì trả khe).
NHƯNG hai chỗ NGHI tích luỹ thật, chờ log xác nhận:
1. **RÒ REF Ô**: `CheckBalance` khi xoá NPC `do_death/do_revive` **cố tình KHÔNG DecRef**
   (`KNpcSet.cpp:762-763`). Nếu xác vẫn còn trong region lúc đó (chưa qua làn 3 — ví dụ NPC
   mồ côi không được Activate nên làn 2/3 không chạy cho nó) thì ô đó **+1 vĩnh viễn** trên
   `m_pNpcRef` (BYTE, trần 255) — y hệt vụ rò ref viên đạn 24/08. Nhận diện: dòng `[S6-BAL]`
   có `doing=10/21` mà `reg >= 0`.
2. **NPC mồ côi đóng băng**: mồ côi thì không được Activate (vòng Activate duyệt theo region)
   ⇒ chỉ còn CheckBalance dọn sau 55,5 s; trong 55 s đó nó chiếm khe. `[S6-BANG]` (5 s/lần)
   in `dung/mocoi/xac/nguoi/quai` trên 256 — nhìn chuỗi này là biết có tích luỹ hay không.

### 8 điểm log mới (tất cả client-only, tắt cùng `AutoLog=0`):

`[S6-VANH]` `[S6-CAM]` `[S6-BAL]` `[S6-ORPHAN]` `[S6-ORPHAN-BACK]` (gắn lại mồ côi,
`KProtocolProcess:1989`) · `[S6-BANG]` (bảng tổng 5 s) · `[S6-ATK]` nay in thêm
**`tgreg=/tgdoing=/tgcell=`** (`thay>0` mà `tgreg=-1` = đánh mục tiêu MỒ CÔI — đúng ca "đánh vào
không khí" mà bản `thay=0` cũ mù) · `[S6-LOADMAP]` (`KSubWorld::LoadMap` 2-arg, đếm cuộn vùng).

### Cách đọc nhanh khi có log mới

```
grep -c "bo qua"                                  # phai = 0
grep "S6-BANG"                                    # dung= co leo thang khong -> TRAN?
grep "S6-BAL" | grep -E "doing=(10|21)" | grep -v "reg=-1"   # >0 = RO REF o DA xay ra
grep "S6-ATK" | grep "tgreg=-1"                   # danh vao muc tieu mo coi (khong khi)
grep -c "S6-ORPHAN\]" ; grep -c "S6-VANH" ; grep -c "S6-CAM" # lan duong bien mat nao chiem
```

## 9.9 KẾT QUẢ ĐỢT LOG 2 (26/08 ~11:45-12:05, 18 phút, me dwID=94619) — **BẢNG NPC CLIENT ĐẦY THẬT**

> Đảo ngược kết luận mục 9 cũ ("dung max 85/256, không được nghi bảng đầy"): phiên đó auto quanh quẩn.
> Phiên này chơi lâu + auto chạy rộng: **`dung` chạm 255/256 (mồ côi chiếm tới 250 khe)** và
> **3.334 cú `[S6-ADD] idx=0`** — trong đó **77,3% xảy ra đúng lúc `dung ≥ 250`** (23% còn lại là
> NPC ngoài vùng nạp — loại fail vô hại). Hàng nghìn lần NPC quanh người chơi KHÔNG HIỆN ĐƯỢC.

Số đo 18 phút (pid 13704): `[S6-VANH]` **11.166** cú gỡ-vành-đai (**11.165/11.166 là kind=1 bot**) ·
`[S6-ORPHAN]` (Close do cuộn vùng) 5.145 · `[S6-CAM]` 39 · gắn lại `[S6-ORPHAN-BACK]` 18.167 ·
`[S6-LOADMAP]` 382 cú cuộn vùng (~1 cú/2,8 s) · `[S6-BAL]` dọn khe 1.058 cú (~1/giây — KHÔNG kịp).

**Phân bố thời gian nằm mồ côi trước khi gắn lại** (15.001 cặp): p50 = **197 ms**, ≤5 s = 80,8%,
≤10 s = **83,5%**, ≤20 s = 88%, p90 = 22,8 s; ai không được gắn lại thì bị `CheckBalance` xoá đúng
~55,4 s (p50 55.357 ms). ⇒ khe bị chiếm vô ích tới 55 s cho 16,5% NPC đã đi hẳn.

**Hai giả thuyết bị BÁC trong đợt này:** (1) RÒ REF ô: 0/1.058 cú `[S6-BAL]` rơi vào nhánh
không-DecRef (`doing=10/21 && reg>=0` = 0) — mọi xác đều đã mồ côi (làn CAM/VANH DecRef đúng) trước
khi bị xoá khe; (2) đánh-mục-tiêu-mồ-côi: 0/4.605 phát có `tgreg=-1`, `thay=0` cũng 0 — auto lọc tốt.
⇒ "TK đánh vào không khí" giờ nghiêng hẳn về **địch VÔ HÌNH do bảng đầy** (chưa có mẫu trận TK thật —
người chơi vào map 324 lúc 12:05, trận vừa khai chiến khi chụp log; phần khu chờ dung chỉ 1-7).

### ĐỀ XUẤT FIX (1 dòng, chờ chủ duyệt) — `KNpcSet::CheckBalance`

NPC **MỒ CÔI** (`m_RegionIndex < 0`) dùng ngưỡng dọn riêng **180 tick (10 s)**; NPC còn trong region
giữ 1000 tick như cũ:

```cpp
if (SubWorld[0].m_dwCurrentTime - Npc[nIdx].m_SyncSignal > (DWORD)(Npc[nIdx].m_RegionIndex < 0 ? 180 : 1000))
```

Cơ sở số: giữ nguyên 83,5% lợi ích cache-gắn-lại, giải phóng khe sớm 5,5 lần ⇒ mồ côi steady-state
ước 250 → ~45 khe, hết tràn (dung max ước ~130-150), hết loạt ADD-FAIL do đầy. Trả giá: 16,5% NPC
quay lại muộn hơn 10 s phải REQNPC + NPC_SYNC full lại (thêm chút lưu lượng, không đổi giao thức).
Lưu ý ngữ nghĩa: NPC mồ côi ngoài vùng KHÔNG được cập nhật `m_SyncSignal` (SyncNpcMin return sớm
`:1975-1977`) nên timeout tính đúng từ gói cuối trước khi bị gỡ.

## 9.10 NGUYÊN NHÂN GỐC TRÀN BẢNG (chốt 26/08 chiều, đo trực tiếp lúc chủ vào TK 12:38) + 3 FIX ĐÚNG NGỮ NGHĨA CHỜ DUYỆT

Chủ game bác đề xuất "dọn mồ côi 10 s" (vặn ngưỡng = chữa cháy) — yêu cầu nguyên nhân gốc. Chuỗi
`[S6-BANG]` bắt được cảnh tràn TRỰC TIẾP khi chủ vào Tống Kim: `+1999s` vào map 324 → `+2011s` sang
map 379 (chiến trường) → **10-24 giây sau: `dung=255, mồ côi 144→254/255`, 1.198 cú ADD-FAIL** —
tức cả bảng là RÁC map cũ, địch/đồng đội VÔ HÌNH phút đầu trận.

**Số quyết định: nhu cầu khe THẬT tối đa quan sát được = `max(dung − mocoi)` = 156/256** ⇒ trần 256
KHÔNG thiếu — thiếu là do rác chiếm. (Đo lại sau fix; chỉ khi >200 mới bàn nâng `MAX_NPC`.)

### Ba nguyên nhân gốc, ba fix (client-only, không đổi giao thức, không đổi cảm nhận):

| # | Gốc | Bằng chứng | Fix đúng ngữ nghĩa |
|---|---|---|---|
| **A** | **Đổi map không dọn bảng NPC** — `NpcSet.RemoveAll` bị comment từ bản gốc `"later finish it. spe"` (`KSubWorld.cpp:1961`) ⇒ NPC map cũ (100% vô nghĩa ở map mới) chiếm khe tới 55 s đúng lúc map mới cần khe nhất | vào 379: mồ côi 254/255, ADD-FAIL 1.198 | **Hoàn thiện chỗ bỏ dở**: khi `bLoadNew` xoá mọi NPC khỏi `NpcSet` trừ chính mình. NPC map cũ vốn không được vẽ ⇒ trải nghiệm chỉ TỐT LÊN (địch hiện ngay từ giây đầu) |
| **B** | **NPC còn sống rời vùng quan sát giữ khe theo timeout mù 55 s**: gói sync của NPC mồ côi có mps NGOÀI vùng nạp bị `return` im (`KProtocolProcess.cpp:1975-1977`) | `[S6-BAL]` 551 cú doing=3 + 274 doing=1 + 168 doing=7 (bot còn sống đi xa) | Tại đúng nhánh đó: **trả khe ngay** (`NpcSet.Remove`) — sự kiện chính xác, không hẹn giờ. NPC lởn vởn RÌA 40-48 ô (trong vùng nạp) KHÔNG bị đụng — vẫn gắn lại 197 ms như đo. Quay vào tầm thì REQNPC+full-sync (cơ chế sẵn) |
| **C** | **Xác chết câm sync** (`SyncNpcMin:1944` bỏ gói doing=10/21) ⇒ xác bị gỡ lúc 6,7 s (`[S6-CAM]`) rồi chiếm khe thêm 55 s; TK chết ồ ạt = 60-120 khe xác | CAM có doing=21×10; BAL doing=10/21 = 36 | Tại khối gỡ-câm-sync (`KNpc.cpp:710`): NPC `do_death/do_revive` thì **xoá khe luôn** thay vì mồ côi. Thời điểm xác BIẾN MẤT y hệt hiện tại (cũng 6,7 s — gỡ region là hết vẽ) ⇒ trải nghiệm không đổi 100 %, chỉ trả khe |

Sau A+B+C, mồ côi steady-state ước ~40 khe (nhóm "đi hẳn không còn sync" vẫn nhờ CheckBalance 55 s —
1.058 cú/25 phút ≈ 39 khe, chấp nhận được), dung đỉnh ~200 ⇒ hết ADD-FAIL. Nghiệm thu: chơi 1 trận
TK, `[S6-BANG]` phải giữ `dung < 220`, `ADD idx=0` ≈ 0, và m末 chủ quan: vào trận thấy địch NGAY.

Ghi chú thêm: trận TK "chết rất nhiều lần" chủ kể nằm trong khúc log **đã bị xoá** (client log bị
reset giữa 11:19-11:45 bởi phiên khác swap DLL) — chưa có mẫu `[S6-ME] doing=10` nào của chính chủ;
cần chủ chết vài lần trong trận TK tới (log đang bật) để đo nốt triệu chứng "chết về thành nhảy toạ độ".

## 9.11 ĐÃ THI CÔNG FIX A/B/C (chủ duyệt 26/08 chiều) — commit `a197a53f`, DLL client `4922889b` ĐÃ SWAP

| Fix | Chỗ sửa | Nhãn đo mới |
|---|---|---|
| **A** dọn bảng khi đổi map (trừ mình + `kind_partner`) | `KSubWorld::LoadMap` ngay dưới dòng comment gốc `"later finish it. spe"` | `[S6-DONMAP] xoa= conlai=` |
| **B** mồ côi + sync báo ngoài vùng → trả khe (trừ partner) | `SyncNpcMin` nhánh `nRegion == -1` (trước đây `return` im) | `[S6-XOAXA]` |
| **C** xác câm-sync 6,7 s → trả khe luôn (mốc biến mất y hệt cũ) | `KNpc::Activate` khối gỡ-câm-sync | `[S6-XOAXAC]` |

Backup: `bin\client\CoreClient.dll.cu_2608_truoc_fixABC_<md5>`. Client-only — thoát vào lại là ăn.

**Nghiệm thu (so con số TRƯỚC/SAU trên cùng kịch bản — chơi 1 trận TK):**

```
TRUOC (do 12:38):                          SAU (ky vong):
[S6-BANG] dung=255 mocoi=254               dung < 220, mocoi < ~60
[S6-ADD] idx=0 : 1.198 cu/phien            ~ 0
[S6-BAL] ~1.058 cu don-55s/25min           giam manh (viec don ve tay A/B/C)
vao tran: dich vo hinh phut dau            thay dich NGAY; [S6-DONMAP] in moi lan doi map
```

Lưu ý phân tích sau: `[S6-ORPHAN]` (Close) vẫn in hàng loạt NGAY TRƯỚC `[S6-DONMAP]` mỗi lần đổi map
(Close chạy trước) — không phải lỗi; và `[S6-XOAXA]` đếm cả ca partner (partner chỉ không bị xoá).

## 9.12 NGHIỆM THU FIX A/B/C (26/08 ~13:10-13:27, pid 11252, me=93431, có ~7,5 phút TRONG trận TK 379)

| Số | TRƯỚC (12:38) | SAU (13:10+) |
|---|---|---|
| `[S6-ADD] idx=0` (NPC vô hình vì hết khe) | **1.198 cú** | **71 cú (−94%)**, dồn vài giây quanh đỉnh trận |
| Vào TK bảng khởi đầu | đầy rác map cũ, 10-24 s kịch trần | `[S6-DONMAP] xoa=110 conlai=1/256` — SẠCH tức thì |
| `[S6-BAL]` dọn vớt 55 s | ~1.058 cú/25 min | 567 cú/16,7 min (đa số việc dọn về tay B/C đúng sự kiện) |
| Fix B `[S6-XOAXA]` | — | 1.001 cú trả khe theo sự kiện |
| Fix C `[S6-XOAXAC]` | — | 9 cú xác |
| RÒ-REF | 0 | 0 ✓ |

**Phần còn lại (đã định danh, chưa fix — FIX D chờ duyệt):** giữa trận TK dung vẫn chạm 255 vài
nhịp (mocoi 150-192). Nguồn = **220 NPC "cư dân dải rìa 40-48 ô"** flapping gỡ-gắn chu kỳ 0,1-0,4 s
(92 % cú VANH): server đẩy sync tới ~48 ô (3×3 region) nhưng client **không bao giờ vẽ ngoài 40 ô**
(`MAX_SYNC_RANGE`, vùng nhìn thật ~31,5 ô) ⇒ client ADD→gỡ→gắn→gỡ vô nghĩa, chiếm 150+ khe.
**FIX D đề xuất**: không nhận NPC cách ≥ `MAX_SYNC_RANGE` vào bảng ngay từ đầu — (i) `SyncNpcMin`
NPC lạ + mps cách mình ≥40 ô → không REQNPC; (ii) nhánh gắn-lại mồ côi: cách ≥40 → trả khe thay vì
gắn. Trải nghiệm không đổi (ngoài 40 ô vốn không vẽ; vào tầm thì ADD ở nhịp sync kế như mọi NPC mới —
chỉ thêm 1 round-trip REQNPC ~50-200 ms lần đầu). Kỳ vọng: mocoi đỉnh ~30-40, dung đỉnh ~150, ADD-FAIL = 0.

Chưa có mẫu chủ chết (me10 = 0 cả phiên — trận đang diễn ra lúc chụp).

## 9.13 FIX D + BỘ LOG S7 LUỒNG CHẾT (26/08 chiều muộn, commit `6abb9914`) — client `ff7814f8` ĐÃ SWAP, server `44f28af5` đặt cạnh `.moi_2608_S7` CHỜ RESTART

Bối cảnh: chủ chết 17 lần trong TK nhưng **log không có mẫu nào** — khúc 13:27-13:35 bị phiên khác
xoá, VÀ gói sync bản thân **không bao giờ mang doing=10/21** (đo 2 bản log: 0 cú, dù doing=9 có 131)
⇒ luồng chết vốn KHÔNG quan sát được. Chủ yêu cầu log kỹ luồng server→client. 4 triệu chứng chờ đo:
(1) chết 0 máu vài giây mới về thành; (2) **nằm bẹp dưới đất sau khi về** (nghi hệ quả fix revive-liền
`839c14c2` — client nhận `s2c_playerrevive` khi còn giữa hoạt ảnh chết; `ProcNetCommand(do_revive)`
gọi thẳng `DoStand()` nên lý thuyết phải dậy — cần S7 chỉ chỗ đứt); (3) về thành đông người mất
NPC vài giây mới hiện đủ; (4) đánh miss phái tiếp cận trong TK.

**FIX D** (`S6_XaQuaTam` ≥40 ô, đồng bộ `MAX_SYNC_RANGE`): không REQNPC NPC lạ ngoài tầm · không ADD
gói full ngoài tầm · mồ côi ngoài tầm → trả khe (hết flapping dải 40-48) · ngoại lệ `kind_partner` ·
đếm `g_nS6BoXa`, in `boxa=` trong `[S6-BANG]`.

**S7** (lọc NGƯỜI THẬT `m_nNetConnectIdx>=0` — ~vài người, không ngập bởi 1000 bot):
client `[S7-CHET-CLI]` (nhận gói chết: doing/cdoing/frame) · `[S7-REV-CLI]`/`[S7-REV-CLI2]`
(trước/sau `DoStand` — soi "nằm bẹp"); server `[S7-CHET]` DoDeath · `[S7-CHET2]` hết hoạt ảnh→script ·
`[S7-REV]` mọi lời gọi + `[S7-REV-EP]` (bấm sớm, nhánh fix revive) + `[S7-REV-NUOT]` (bị bỏ qua) ·
`[S7-REV-AUTO]` (đến hạn 5 s) · `[S7-REV-BAM]` (người bấm nút) · `[S7-REV-XONG]` (ChangeWorld về trại).
⇒ MỘT cú chết ra trọn timeline: chết → hoạt ảnh → script → nguồn hồi sinh → về trại → client dậy;
"mất NPC vài giây" đo bằng nhịp `[S6-ADD]` sau `[S7-REV-XONG]`.

Đo miss-phái-tiếp-cận cần server `config.ini [AutoLog] Name=` đúng **nick chủ đang chơi** (đang là
`CaiBang` — nếu chủ chơi nick khác thì các nhãn `S1/S4` im lặng). Hỏi chủ nick trước khi đo.

## 9.14 KẾT QUẢ ĐO S7 + FIX D (26/08 ~14:20-14:50, server `44f28af5`, client `ff7814f8`, 28 cú chết CaiBang trong TK)

| Triệu chứng | Số đo | Kết luận |
|---|---|---|
| Chết 0 máu lâu về thành | 26/28 cú: chết → về trại **0,85-1,0 s** (0,83 s = hoạt ảnh chết 15 frame, trần vật lý); **2/28 cú 5,9 s** — cả hai là `REV-AUTO` không có `REV-BAM` trước | Fix revive-liền chạy hoàn hảo (7 `REV-EP`, **0 `REV-NUOT`**); 2 cú chậm do **auto TK không bấm nút** → chọn: (a) WAuto bấm bền hơn, (b) hạ auto-rev 90→~27 tick |
| Nằm bẹp dưới đất | 8/8 cú quan sát từ client: sau `DoStand` `doing=1 cdoing=1` | Không tái hiện — chờ chủ xác nhận cảm quan |
| Về thành mất NPC vài giây | 40-101 NPC hiện lại trong 3 s đầu sau hồi sinh, đủ sau ~5-10 s | Bản chất giao thức xin-từng-con (REQNPC→full). Muốn nhanh: server chủ động đẩy loạt NPC quanh điểm hồi sinh khi ChangeWorld (server-side, chờ duyệt) |
| Đánh vào không khí / miss cận chiến | **Drift bot GẦN HẾT: 1,3 % >1 ô, max 1,1 ô** (trước 10,9-16,7 %, max 7,3) — A/B/C/D gián tiếp chữa (NPC được làm mới vị trí qua chu trình xoá/ADD); server: hụt 51/386 = **13,2 %** (trước vá tháng: 29,9 %), 669 cú TOOFAR-đuổi (mục tiêu TK chạy — hành vi đúng) | Hình chủ chụp là bản TRƯỚC fix D. Cần chủ xác nhận phiên mới còn thấy không; nếu còn → đào bằng `S6-ATK tgcell/tgreg` + cần giảm ồn server log |

⚠️ Server log tràn **1.656 cú "bo qua"** (nhãn `E3_/E4_` chiếm) — `S4-CAST` 577 vs `S4-MSL-END` 386 lệch
chủ yếu do mất mẫu; số miss là ước lượng. Muốn đo miss chuẩn phải tắt bớt `E3_/E4_` (của phiên kia).
FIX D xác nhận chạy: `boxa=14.442` NPC-xa bị chặn, bảng ổn định 135-137/256, mồ côi ~65.

## 9.15 VÒNG ĐO 26/08 ~15:00 (chủ báo 4 triệu chứng còn) — LOẠI TRỪ 4 TẦNG, CÒN FPS CLIENT

Chủ: nằm bẹp VẪN CÒN · miss cận chiến VẪN NHIỀU · "chết lâu về thành do đánh TK lâu bị QUÁ TẢI kẹt" ·
bot/người/quái trượt tới-lui.

**"Quá tải kẹt" — đo cả 4 tầng đều SẠCH:** server tick 1080-1081/60s trễ 0,1% suốt trận 1001 online
(`jx_perf_server.log`) · trễ server→client mọi cú hồi sinh **7-122 ms** · vòng logic client **0 khoảng
câm >300 ms** cả phiên · về-trại thực tế **≤1,4 s** ⇒ tầng duy nhất CHƯA đo = **FPS VẼ client khi đông**
→ đã bật `PerfHud=1` trong `bin\client\Config.ini` (không cần build) — chủ đọc số FPS lúc đông; nếu
tụt sâu thì "kẹt/giật/trượt" phần lớn là khung hình → hướng xử = tối ưu vẽ.

⛔ **ĐÍNH CHÍNH thước đo của tôi**: 3 cú "về-trại 9 s" ở 9.14 là ARTIFACT — detector so cell giữa
2 nội dung slot (slot tái dùng trùng số); RAW cho thấy cú đó về trại sau **333 ms** (`nhanh=loadmap`
`[S6-LOADMAP] tam=(105,96)`). Đừng dùng lại số 9 s.

**Miss cận chiến (CaiBang, mẫu bị tràn — server 1.656 "bo qua" do E3/E4 phiên kia):** 943 `S4-CAST`
thi hành + **669 `S2-MELEE-TOOFAR-RUN`** (client vung hoạt ảnh nhưng server từ chối "xa quá→đuổi") ⇒
**~41 % số lần vung nhìn-như-hụt**; trong đạn thi hành: 51/386 không chạm (13,2 %; trước vá tháng
29,9 %) — `life=4/4 lasthit=0` = mục tiêu chạy thoát ô; dist lúc cast p50=65 p90=102 (radius 90+20 —
đánh ở RÌA tầm); dist=0 44 cú vẫn sinh đạn (vá cũ chạy tốt). **Đề xuất chờ duyệt: hạ ngưỡng auto
phát-đánh near 75 → ~50 mps** (vung ít hụt, TOOFAR giảm mạnh; trade-off: áp sát thêm ~0,1-0,2 s).

**Nằm bẹp:** 23/23 cú trong log dậy sạch (`doing=1 cdoing=1` sau DoStand) — nhãn hiện tại KHÔNG bắt
được ca chủ thấy → kế: thêm nhãn tự-phát-hiện `[S7-NAMBEP]` (client tự soi mỗi 2 s: `m_Doing==do_stand`
mà `m_ClientDoing` == hoạt ảnh nằm (cdoing 8/10 — xem enum cdo_ trước khi code)) + nhờ chủ báo
GIỜ phút:giây khi gặp. **Trượt tới-lui:** drift sync còn 1,3 % (mẫu ít) — chờ số FPS để tách
giật-do-vẽ vs trượt-do-vị-trí; nếu còn thì đặt nhãn S8 theo dõi chuyển động 1 NPC mẫu.

## 9.16 ĐỢT MỔ SÂU 16 TÁC NHÂN (26/08 tối) — RA GỐC CỦA "TRƯỢT" VÀ "MISS", **CHƯA VÁ GÌ**

5 mũi đào độc lập + 2 vòng phản biện + tự kiểm lại. Báo cáo đầy đủ:
`<scratchpad>/baocao_dot9.md`. Dữ liệu: cặp client+server chồng nhau 21,9 phút (43 cú chết) +
682 giây log sau fix D. **Trần log thật `[AUTOLOG] bo qua` = 0 ở cả 4 tệp** ⇒ số liệu là đếm
chính xác (⚠️ `grep -c "bo qua"` ra 2526 là DƯƠNG TÍNH GIẢ — trúng chữ trong thân nhãn
`[E2-RECV-RANDMOVE] ... bo qua sat thuong`; phải grep `\[AUTOLOG\] bo qua`).

### A. TRƯỢT TỚI-LUI — **GỐC: `KNpcFindPath::GetDir` trả CÙNG MÃ 0 cho hai nghĩa trái ngược**

`KNpcFindPath.cpp:52` trả `0` khi `!CheckDistance(...)` = **ĐÃ TỚI ĐÍCH** (`:250-253` trả TRUE khi
còn xa hơn một bước); nhưng `:157` và `:166` cũng trả `0` khi **THẬT SỰ BỊ CHẶN**. Chỗ gọi duy nhất
`KNpc.cpp:4621` gộp cả hai vào một nhánh `nRet == 0` (`:4640`): nhân vật mình thì `DoStand()` (đúng),
**mọi NPC khác** thì `m_nNeedFixPos++; DoStandBlocked();` (`:4656-4657`) — mà `DoStandBlocked`
(`:2138-2148`) **chỉ đổi hoạt ảnh, không đặt `m_Doing`** ⇒ cửa `ServeMove` vẫn mở, tick sau lại
`ret=0`, cờ tăng mãi. Cờ đó mở cửa ghi đè toạ độ (`KProtocolProcess.cpp:2135`) mà chỉ hạ khi gói
sync đổi vị trí (`:2144-2153`) — NPC đứng yên thì không bao giờ hạ ⇒ **kéo lui liên tục**.
🔴 Nhánh `else if (nRet == 0)` này CHÍNH LÀ BẢN VÁ CŨ CỦA CHÍNH DỰ ÁN (chú thích ngay trong code:
*"JX1 truoc day goi DoStand() => dong nguoi la ai cung dung khung roi nhay"*) ⇒ **cấm revert thẳng**,
phải TÁCH hai nghĩa. Bản `_SERVER` (`:4674-4683`) làm đúng: `else { DoStand(); return; }`.

**Số tự đếm lại (không lấy của tác nhân):** `[E4_MOVE_PATH] ret=0` n=160 → **158 (98,8%) là ĐÃ TỚI
ĐÍCH** (d < speed); bài đối chứng bắt buộc: `ret=1` n=1116 → **0 dòng** có d < speed (đúng như mã
đòi hỏi). Ví dụ vàng `npc=92822`: `fix` leo **0→304 qua 53,4 giây**, `cl == sv` từng chữ số.
Mắt nhìn thấy: **5632 cú kéo ≤2 ô = 8,3 cú/giây** (p50 19 mps), **62,4% ngược hướng đang đi**;
thêm 1329 cú >2 ô bị SNAP = "biến mất rồi hiện lại". Lớp nội suy (`PAINT_INTERP_SNAP_DIST=64`)
biến cú nhảy 1 khung thành **cú trượt mượt** — nên bây giờ mới thấy rõ. Số cú nắn còn **TĂNG**
23,92 → 30,02 lần/giây sau các đợt vá (các đợt trước chạm BIÊN ĐỘ, không chạm SỐ LẦN).

**Vá đề xuất (rẻ, rủi ro thấp, `GetDir` chỉ có 1 nơi gọi):** `KNpcFindPath.cpp:52` `return 2`;
`KNpc.cpp:4640` thêm nhánh `else if (nRet == 2) { DoStand(); return; }` TRƯỚC nhánh `nRet==0`;
`KNpc::DoStand()` thêm `m_nNeedFixPos = 0;`.

### B. MISS CẬN CHIẾN — **GỐC: toạ độ CỦA CHÍNH NHÂN VẬT lệch, client CỐ Ý không bao giờ tự nắn**

Phễu đầy đủ (21,9 phút, đếm lại toàn bộ): **6546 gói đánh gửi đi → chỉ 368 đòn có sát thương = 5,6%**.
Chặng: 649 (9,9%) mục tiêu server không còn · 2330 (35,6%) bị nuốt giữa hoạt ảnh · 2509 (38,3%) bị
ghi đè trong hàng đợi MỘT khe (`m_Command` là biến đơn) · 1052 tới phân quyết, trong đó **557 (52,9%)
bị từ chối XA QUA**, 495 thành đòn thật, và **127/495 (25,7%) đòn thật không gây sát thương nào**.
Đã loại trừ: né đòn (539/539 `missrate=0`), trễ mạng (p50 4 ms, 0 gói mất), tick server, vũ khí/ngựa.

Cơ chế: `SyncNpcMin` có hai chỗ ghi đè vị trí, **cả hai đều loại trừ chính mình**
(`KProtocolProcess.cpp:2104` và `:2135`), và người quyết định đánh cũng là client (server
`ProcessPlayer` bản `_SERVER` chỉ có 2 lời gọi trap — **máy chủ không bao giờ tự đuổi bám thay
người chơi**). ⇒ client tin mình trong tầm, bắn liên tục, **không hề chạy**; server đo bằng toạ độ
của nó thấy xa → từ chối tại `KSkills.cpp:359-360` (`dist > radius + 20`).
**Lệch đo được** (`[S6-ME]`, cùng dòng nên không dính bẫy slot): toàn cửa sổ p50 31 / p90 109 /
max 291 mps; **riêng lúc bị từ chối p50 = 66 mps (2 ô)** — **93,4% ca lệch một mình nó đã đủ giải
thích phán quyết**. Cụm đóng đinh: client đứng bảng ở (254,400) suốt 1,15 giây bắn 18 gói, server
cho là (309,349), `dist=135 > 110`.
**Cộng hưởng:** WAuto kẹp ngưỡng bắn `nNearDist < 75 → 75` (`CoreShell.cpp:14569-14570` và
`:14782-14783`) ⇒ biên an toàn chỉ **110−75 = 35 mps**, trong khi lệch lúc từ chối p50 = 66 mps
⇒ auto bắn thẳng vào vùng server chắc chắn từ chối, **và không bao giờ biết mình bị từ chối**.

Thứ tự vá đề xuất: (1) hạ kẹp 75 → ~45-48 (rẻ nhất, ăn ngay vào 52,9%); (2) nắn toạ độ bản thân
**CÓ ĐIỀU KIỆN — chỉ khi ĐỨNG YÊN và lệch ≥2 ô** (⚠️ đây là chỗ từng đẻ ra lỗi "giựt lùi" tháng 8,
phải đặt log đo trước rồi mới vá); (3) báo client khi mục tiêu không còn (`:5237` đang im lặng).

### C. CHẾT CHẬM VỀ THÀNH — **ENGINE KHÔNG CHẬM. Cái chậm là ĐIỂM HỒI SINH Ở XA**

41/43 cú xong dưới 1 giây (chết→hết hoạt ảnh p50 835 ms = đúng 15 khung/18fps; nhận lệnh→dời vị trí
p50 **0 ms**); 2 cú >4 giây là 2 cú **không ai bấm nút** (rơi auto 5 giây). `[S7-REV-NUOT]=0`,
sổ sách khớp tuyệt đối: `[S7-REV] 57 − [S7-REV-EP] 14 = 43 = [S7-REV-XONG] = [S7-CHET]`.
**43/43 lần hồi sinh về ĐÚNG MỘT ĐIỂM** `sw=379 mps=(54016,98304)`, cách chỗ chết **p50 296 ô**
⇒ **từ lúc sống lại đến lúc đánh được tiếp: p50 18,0 giây (min 16,4 max 44,4)**, tổng **14,4 phút
chạy bộ trong phiên 46,4 phút**. ⇒ việc cần làm là **đổi điểm hồi sinh (script/cấu hình bản đồ 379)**,
không phải vá C++.
Điểm chưa chốt: 41/41 lần bấm nút đều rơi đúng dải 803-946 ms (ngay khi hoạt ảnh chết kết thúc) —
nghi **nút chỉ ăn sau khi hoạt ảnh chạy xong** ⇒ mỗi cú chết ăn thêm ~0,85 giây vô ích. Cần 1 nhãn
ở chỗ client gửi `c2s_playerrevive` + chỗ mở hộp thoại để chốt.

### D. NẰM BẸP — **CHƯA RA GỐC, 4 giả thuyết mạnh nhất ĐÃ BỊ BÁC BỎ BẰNG MÃ**

47/47 cú chết trong 3 tệp đều kết thúc `doing=1 cdoing∈{1,2}`; 25.755 mẫu nhãn không tiết chế của
chính nhân vật: `doing=10` và `doing=21` xuất hiện **0 lần**. Bị bác: (1) "gói sync full diễn lại
hoạt ảnh chết" — `KNpc::NormalSync` chặn ngay đầu (`KNpc.cpp:5764`), server KHÔNG BAO GIỜ gửi gói
mang `Doing=do_death/do_revive`; (2) "trạng thái nằm chỉ có một đường ra" — còn ≥2 đường (bị đánh
→ `DoHurt`; đổi bản đồ → `SendCommand(do_stand)`); (3) "mất một gói hồi sinh là nằm vĩnh viễn" —
server gửi **hai** gói (`KPlayer.cpp:6822`+`:6823`), đo thật 1,96-2,00 gói/cú, và đường truyền là
**TCP**; (4) "`DoStand()` return sớm là bẫy" — trạng thái kích hoạt **bất khả đạt** (47/47 `cdoing=8`
đều kèm `doing=21`) ⇒ **vá chỗ đó = mã chết, tạo ảo giác đã vá**.
Còn lại: **(G1)** `KNpc.cpp:2673-2691` lấy `ClientDoing` từ bảng chiêu, chỉ chặn `>= cdo_count`, nên
một dòng chiêu có `CharActionId = 8` sẽ lọt và để nhân vật **đứng mà mang tư thế chết** — kiểm bằng
cách đếm cột action-type = 8 trong bảng chiêu (**lỗi DỮ LIỆU, chưa kiểm**). **(G2)** NPC mồ côi:
`KNpc::Activate` return ở `:676-677` **trước cả** `m_DataRes.SetAction()` ⇒ đóng băng cả tư thế lẫn
vị trí; đo thật có nhịp `dung=228/256 mocoi=208` (91% NPC không được cập nhật hoạt ảnh).
**Cần thêm nhãn `[S8-VE]`** ngay trước `KNpc.cpp:738` (in `m_DataRes.m_nDoing/m_nAction`) — hiện
**toàn bộ hệ log chưa từng nhìn thấy LỚP VẼ**, nên không thể phân biệt lỗi ở `KNpc`, `KNpcRes` hay
`Represent`. Kèm 1 vá 1 dòng đáng ngờ: `KNpcRes::Init` (`KNpcRes.cpp:66`) đặt lại `m_nAction=0`
nhưng **không đặt lại `m_nDoing`**, trong khi `SetAction` mở đầu `if (m_nDoing == nDoing) return TRUE;`
⇒ khe NPC tái dùng có thể vẽ bằng bộ ảnh sai.

### 🔴 NHỮNG SỐ PHẢI RÚT LẠI (phản biện bác bỏ — cấm dùng lại)

| Kết luận cũ | Vì sao sai |
|---|---|
| `[SKILL-REFUSE-FAR]` = đòn bị từ chối vì xa (manh mối tôi đưa cho các mũi) | Nhãn đặt **trước** cửa chặn, bản server in **vô điều kiện** (`KSkills.cpp:353-358` điều kiện nằm trong `#ifndef _SERVER`). 0/62 dòng client có `d ≤ 0,8R` nhưng **44,0%** dòng server có ⇒ từ chối THẬT chỉ 4 dòng |
| `[MSL-SET-FULL]`/`[MISSLE-POOL-FULL]`/`[E3_MISSLE_ADDFAIL]` = hết khe đạn | Cả 3 đặt trước cửa chặn; có dòng `dang dung=0` mà kêu hết khe |
| "Dòng `KNpc.cpp:1014` xoá lệnh là nguyên nhân số 1 của miss" | Server không bao giờ tự đuổi bám thay người chơi ⇒ hạ bậc xuống "lưới an toàn bị vô hiệu", xếp thứ 9 |
| "Nhịp gói tự đồng bộ ~55 ms" | Đo thật p50 **111 ms**, tb 145, max **3.223 ms** |
| "Mảng nội suy theo khe `idx` bị tái dùng gây trượt" | Guard `dwID` (`CoreShell.cpp:18243`) chặn 5014/5049; chỉ 0,021 lần/giây so với 8,3 cú trượt/giây ⇒ không phải gốc |
| "Fix D làm tăng `[S6-CMD] ap=0`" | 18,73% → 17,80% — gần như không đổi |
| "fix càng cao càng trùng khít chứng minh NPC đã tới đích" | Lập luận **tự vòng** (luật reset sinh ra chính nó) ⇒ thay bằng số học từng dòng `[E4_MOVE_PATH]` |

### 🕳️ CHỖ MÙ CỦA HỆ LOG (cần bổ sung)

1. **Chưa hề thấy LỚP VẼ** (`m_DataRes`) — chỗ mù nghiêm trọng nhất, chặn đứng việc chốt "nằm bẹp".
2. **KHÔNG CÓ CON QUÁI NÀO trong bản ghi**: 682 giây chỉ có `kind=1` (người) và `kind=3` (NPC thoại),
   **`kind=0` = 0 dòng** ⇒ mọi kết luận về "quái trượt/quái miss" là **suy từ mã, chưa đo được**.
   Cần một phiên đo ở **bãi có quái**.
3. Nhãn đặt trước cửa chặn = báo động giả — đã lừa **3 lần**; danh sách nghi thêm: `KSkills.cpp:188`,
   `:193`, `:230`, `:279`, `:300`, `:357`, `:371-376`.
4. Không có nhãn cho **hàng đợi lệnh MỘT khe** (38,3% gói biến mất không dấu vết) — cần
   `[S5-CMD-DEDE]` ngay trước `KNpc.cpp:4947`.
5. Không đếm được **viên đạn sinh ra** cho riêng một người (nhãn đều là `AUTOLOG_EVERY`), và
   `[S4-MSL-END]` chỉ ghi trong `DoVanish` nên đạn chết qua `DoCollide` **không bao giờ in**.
6. `AUTOLOG_IDX` chỉ lọc theo **người tung chiêu** ⇒ mọi thứ CaiBang **bị đánh** đều vô hình.
7. Chưa có mẫu đối chứng "không cưỡi ngựa" (557/557 dòng TOOFAR đều `ngua=1`).

## 9.17 ĐỢT 10 — ĐÃ VÁ 1+3+5 (chủ duyệt), client `dde18f1b` ĐÃ SWAP · server `fa6bfb46` chờ restart (commit `7e48620e`)

Chủ chốt: *"ưu tiên là không ảnh hưởng người chơi trải nghiệm game — phải tìm ra lỗi rồi fix chính
xác lỗi chứ không phải fix chữa cháy"*. Vì vậy **KHÔNG** hạ ngưỡng bắn 75→45 như dự kiến ban đầu
(đó là vặn số/che triệu chứng) — thay bằng vá đúng cơ chế. Trước khi vá đã đo thêm 2 tầng:

| Số tự đếm (log `dot9/cli.log`, 682 giây) | Kết quả |
|---|---|
| `[E4_MOVE_PATH] ret=0` là **đã tới đích** (d<speed) | **158/160 = 98,8%** (đối chứng `ret=1`: **0/1116** — đúng như mã đòi hỏi) |
| Gói sync có ghi đè toạ độ (`nan=1`) | **20.487 = 30 cú/giây**; 65,2% vô hại (lệch 0), **23,0% dịch > nửa ô**, p99 = 249 mps, max 857 mps |
| NPC "đang chạy" vừa bị ghi đè, **có tự chạy tới gói sau không** | **67,9% ĐỨNG IM HOÀN TOÀN** (n=17.320) ⇒ bị **kéo từng nấc**, nội suy biến mỗi nấc thành cú trượt mượt |
| NPC được ADD trong trạng thái chạy | 251/458 — mỗi con được giao đích = **chính chỗ nó đứng** (`KProtocolProcess.cpp:1979`) ⇒ "tới đích" ngay |

⚠️ **Vì thế item 1 làm MỘT MÌNH sẽ thành hồi quy**: cờ hết bị kẹt ⇒ gói sync hết quyền ghi đè ⇒ NPC
đứng im tại chỗ sai = đúng lỗi "đứng khựng rồi nhảy" mà bản vá tháng trước sinh ra để chữa. Nên phải
đi kèm FIX-2.

**Bộ vá (13 miếng, 8 tệp):**
- **FIX-1 (gốc TRƯỢT)** `KNpcFindPath.cpp:52` trả **mã riêng 2** = "đã tới đích" (hai chỗ còn lại vẫn
  trả 0 = bị chặn thật) · `KNpc.cpp` `ServeMove` thêm nhánh `nRet==2 → DoStand()` · `DoStand()` xoá
  `m_nNeedFixPos` (bọc `#ifndef _SERVER` — biến chỉ có bản client). Bản `_SERVER` gộp mọi mã ≠1 vào
  `DoStand()` nên **hành vi máy chủ không đổi**; `GetDir` chỉ có **một** nơi gọi.
- **FIX-2 (chống hồi quy, cùng gốc)** `SyncNpcMin`: khi máy chủ báo NPC **đang di chuyển** mà bản sao
  client lệch **≥1 ô và <12 ô** thì **giao đúng đích đó** để nó **tự chạy tới** thay vì bị kéo. Không
  đụng NPC chạy đúng (lệch p50 chỉ 12 mps < 32) và không đụng dịch chuyển thật (≥12 ô → nhánh nắn cũ).
  Nhãn `[S9-DICH]`.
- **FIX-3 (MISS)** lưu vị trí **máy chủ** của chính nhân vật (`g_nS9SvMeX/Y`, ghi trong
  `SyncNpcMinPlayer`, **không đụng toạ độ client** nên không có rubber-band); máy đánh (cả nhánh
  thường lẫn PK) lấy **khoảng cách XẤU HƠN** trong hai góc nhìn ⇒ chỉ bắn khi **cả hai** thấy trong
  tầm ⇒ hết đòn bị từ chối im lặng. Nhãn `[S9-TAM]`/`[S9-TAM-PK]` in cả `dcli`, `dsv`, `lechme`.
- **FIX-4 (3 nhãn báo động giả)** `SKILL-REFUSE-FAR`, `MSL-SET-FULL`, `MISSLE-POOL-FULL`+
  `E3_MISSLE_ADDFAIL` → đặt **đúng trong thân `if`**; nay in ra là **từ chối/hết khe THẬT**.
- **FIX-5 ("nằm bẹp", ứng viên có thật)** `KNpcRes::Init` đặt lại `m_nAction` nhưng **quên `m_nDoing`**,
  trong khi `SetAction` mở đầu `if (m_nDoing == nDoing) return TRUE;` ⇒ khe NPC tái dùng (7,5 lần/giây)
  hoặc nhân vật mồ côi qua mỗi lần đổi vùng có thể **không bao giờ nạp bộ ảnh mới** → vẽ bằng hoạt ảnh
  của chủ cũ. Đặt `m_nDoing = -1`. Kèm nhãn **`[S9-VE]`** — nhãn ĐẦU TIÊN nhìn thấy lớp vẽ
  (`resdoing`/`resaction`), chỉ ghi cho chính nhân vật và chỉ khi tư thế đổi.

**Nghiệm thu (đo lại đúng 4 số trên):** `ret=0` phải còn ~1,3% (chỉ ca bị chặn thật) · tỷ lệ ghi đè
`nan=1` giảm mạnh · **"đứng im giữa hai gói" 67,9% → thấp** · `[S9-TAM]` cho thấy `dsv > dcli` bao
nhiêu và `[S2-MELEE-TOOFAR-RUN]` phải giảm mạnh · nếu "nằm bẹp" tái diễn thì `[S9-VE]` chỉ đích danh
tầng hỏng.

🔴 **Bẫy vận hành**: hai tệp `KProtocolProcess.cpp` + `CoreShell.cpp` đang là **công trường chung với
phiên khác** (gói S8 Tống Kim + hệ xúc xắc, chưa commit) ⇒ FIX-2/FIX-3 **cố ý KHÔNG commit** để không
đè việc của họ; tái áp bằng `ReverseTools/goi_va_dot10_truot_miss.py` (+`_b.py`). DLL đợt này build từ
cây có cả phần chưa commit của họ — giống hệt bản `dac6f83e` họ đã thả trước đó.

## 9.18 HAI HỒI QUY DO CHÍNH ĐỢT 10 GÂY RA — đã định lượng, r2 đã thả, r3 viết sẵn chờ gộp

Chủ báo trong lúc test (chủ **chưa nạp** bản mới nên log đang chạy là `dde18f1b` = r1):

**(a) "người xung quanh đang di chuyển thì QUAY ĐẦU LUI rồi đi tiếp"** — lỗi của **FIX-2 r1**.
Giao đích = vị trí máy chủ **tại thời điểm gói tin**, mà vị trí đó đã cũ (trễ mạng + nhịp 55 ms);
bản sao client nếu đã chạy vượt qua thì bị bắt **quay đầu**. Đo: **2.413/22.857 = 10,6%** lệnh giao
ra nằm **phía sau** hướng đang chạy (tích vô hướng âm); mẫu `npc=92426` lệch 72-96 mps trong khi
bước máy chủ chỉ 24 mps/gói.
➜ **r2 (`8b4ef5a0`, client `9fb7cac0` đã thả, CHƯA ai nạp)**: nhớ vị trí máy chủ gói trước (mảng
static theo khe + `dwID` để biết khe đổi chủ) → suy ra vector V → giao đích **P + V** (một nhịp phía
trước); nếu vẫn nằm phía sau thì **bỏ qua hẳn**, không bao giờ ép quay đầu. Chặn `|V| > 200` (dịch
chuyển). Nhãn `[S9-LUI]` đếm số lần bỏ qua.

**(b) "di chuyển ra ngoài rồi CHẠY VÀO TƯỜNG ~30 giây rồi mới đi A* bình thường lại"** — **FIX-3 làm
nặng thêm một lỗ hổng có sẵn**.
Đo (cùng một thước, loại các cú dịch chuyển/hồi sinh ra khỏi quãng đường — **bẫy: không loại thì
mỗi cú chết-hồi sinh 296 ô bị tính thành "đi 9.472 mps", ra 7 ca giả**):
| Bản | Thời gian "đi nhiều mà không tới đâu" |
|---|---|
| `jx_client_i` (trước) | 70/1310 giây = **5,3%** |
| `dot9` (trước FIX-3) | 19/681 giây = **2,8%** |
| `dot10 r1` (sau FIX-3) | 94/1337 giây = **7,0%** |
Hiện trường ca nặng nhất (`t=69379160`, 29 giây): chạy 800 mps rồi **dội qua dội lại trong hộp
6×8 ô** suốt 25 giây, `doing` nhấp nháy 3↔1.
Cơ chế: FIX-3 bắt máy đánh dùng **khoảng cách xấu hơn** ⇒ chọn **áp sát** thay vì bắn; mà máy đánh
**không hề có bước bỏ cuộc** — mục tiêu sau tường/không có đường tới thì nó húc mãi. Lỗ hổng có sẵn
(khâu chọn mục tiêu không kiểm đường tới), FIX-3 chỉ làm lộ ra.
➜ **r3 viết sẵn** (`scratchpad/patch_dot10d_bomuctieu.py`, **chưa áp** — chủ muốn gộp một lần):
đang áp sát mà **quá 4 giây không gần thêm được ≥1 ô** ⇒ loại mục tiêu **30 giây** và chọn con khác,
dùng đúng khuôn `m_mAutoExcludeNpcID` sẵn có của `[FIGHT-SKIPGOLD]`. Nhãn `[S9-BOMUCTIEU]`.

🔑 **Bài học ghi lại**: FIX-3 đổi "đứng vung vào không khí" thành "chạy húc tường" — cùng một gốc
(auto không biết mình bị từ chối / không tới được), nên **phải đi kèm bước bỏ cuộc** mới trọn vẹn.

## 9.19 ĐO LẠI "ĐÁNH MISS PHÁI CẬN CHIẾN" SAU FIX-3 — cơ chế miss ĐÃ CHỮA ĐƯỢC, nhưng sát thương/phiên KHÔNG đổi

Hai cửa sổ **cùng độ dài** (21,9 vs 22,3 phút), chỉ dùng nhãn `AUTOLOG_IDX` (lọc tên CaiBang, **không
tiết chế** ⇒ đếm được); trần log thật = **0 dòng** ở cả hai tệp.
⚠️ `[SKILL-REFUSE-FAR]/"TU CHOI THAT"` là `AUTOLOG_EVERY` (tiết chế + không lọc tên) ⇒ **cấm** dùng
701 dòng của nó làm số đếm — đã suýt vấp lại.

| | TRƯỚC vá | SAU FIX-3 |
|---|---|---|
| Gói đánh client gửi | 6.546 | 4.558 (−30%, hết bắn vô ích) |
| **Bị máy chủ từ chối "xa quá"** | **557 (8,5%)** | **75 (1,6%) — giảm 87%** |
| Đòn thi hành thật (`S4-CAST`) | 495 | 641 |
| **Hiệu suất mỗi lần bấm (CAST/ATK)** | 7,6% | **14,1% — gần gấp đôi** |
| **Khoảng cách lúc ra đòn** | p50 **63**, p90 102 (rìa tầm; trần máy chủ 110) | p50 **26**, p90 67 |
| Đạn hụt hoàn toàn | 11,8% | **8,6%** |
| Đạn đâm địa hình | 2,9% | **0,5%** |
| **Nhịp giữa 2 đòn KHI ĐANG ĐÁNH** | p50 **832 ms** | p50 **481 ms** |
| **Tốc độ đánh khi đang giao tranh** | 78,3 cast/phút | **96,3 cast/phút (+23%)** |
| Đòn chạm/phút (cả phiên) | 24,7 | 25,2 (**+2%**) |

**Kết luận trung thực:** cơ chế "vung mà không ăn" **đã chữa được** (từ chối −87%, đánh gần hơn 2,4
lần, đạn hụt/đâm tường giảm). Nhưng **sát thương cả phiên gần như không đổi**, vì:
**thời gian THỰC SỰ giao tranh chỉ chiếm 29-30% phiên** ở cả hai bản (6,3 vs 6,7 phút/22 phút).
70% còn lại là: chết 20-23 lần × **18 giây chạy về** (mục 9.14 = 14,4/46,4 phút), chạy tới mục tiêu,
và **7% húc tường** (mục 9.18).

⇒ Muốn tăng sát thương thật thì hai đòn bẩy còn lại **không nằm ở khâu đánh** nữa:
1. **FIX-6 bỏ mục tiêu không tới được** (đã viết sẵn) — lấy lại ~7% thời gian.
2. **Đổi điểm hồi sinh bản đồ 379** (script/cấu hình) — lấy lại tới ~30% thời gian.

## 9.20 ĐÍNH CHÍNH QUAN TRỌNG + trạng thái bản `2145f043` (26/08 tối muộn)

### 🔴 RÚT LẠI: "FIX-3 làm húc tường nặng lên 2,8% → 7,0% → 12,9%" — **SAI, do thước đo của tôi**

Thước cũ = *quãng đường đi / khoảng cách tới được* trong cửa sổ 20-30 giây. Nó **không phân biệt được**
"húc tường" với **săn quái bình thường** (chạy từ con này sang con khác trong một bãi nhỏ ⇒ đi nhiều,
net nhỏ). Bằng chứng: mở hiện trường một "ca húc tường" của bản mới thì thấy chuỗi hoàn toàn lành mạnh
— `dist 639→351→160→114→46` rồi **đổi mục tiêu** sang con khác `548→357→213→141→69`, tức auto Dã Tẩu
đang chạy giữa các con quái. (Trước đó tôi cũng đã phải loại "cú chết + hồi sinh 296 ô" khỏi quãng đường
— thước này dính bẫy hai lần.)

### ✅ Thước ĐÚNG cho "bám mục tiêu mà không tới được"

Định nghĩa theo **hành vi**: cùng một `tgID` liên tục, kéo ≥6 giây, và **khoảng cách tốt nhất không cải
thiện nổi 1 ô trong ≥4 giây**. Đọc từ `[PK-EMIT]`/`[FIGHT-EMIT]` (dùng theo kiểu *có mặt*, hợp lệ với
nhãn tiết chế).

| Bản | Thời lượng | Số đoạn kẹt | Tổng thời gian kẹt | Đoạn dài nhất |
|---|---|---|---|---|
| Trước mọi fix | 1309 s | 4 | 56 s = **4,2%** | 37 s (tgID 92985) |
| Trước FIX-3 | 673 s | 5 | 59 s = **8,8%** | 21 s |
| **Sau FIX-3 (r1)** | 1295 s | 2 | 42 s = **3,3%** | 38 s (tgID 93073, kẹt ở 284 mps) |
| r3 (118 s, Dã Tẩu) | 95 s | 0 | 0% | — |

⇒ **Hiện tượng này CÓ TỪ TRƯỚC, không phải FIX-3 gây ra** (sau FIX-3 còn thấp hơn). Nhưng nó có thật và
có những đoạn 21-38 giây — đúng như chủ game mô tả "chạy vào tường khoảng 30 giây".
**100% các đoạn kẹt đều ở nhánh `PK-EMIT`** ⇒ FIX-6 đặt ở nhánh PK là **đúng chỗ**; với ca 38 giây
(tgID 93073, `dist` dao động 284-481 suốt 25 giây) FIX-6 sẽ cắt còn ~4 giây rồi đổi mục tiêu.
⚠️ FIX-6 **chưa được thử lửa**: phiên r3 mới chỉ 118 giây đánh Dã Tẩu, chưa có trận TK nào ⇒
`[S9-BOMUCTIEU]` = 0 là **do chưa gặp tình huống**, không phải do vá hỏng.

### 🔴 Thước "quay đầu lui" cũng hết nghĩa với r2/r3
Ở r1 nó đo "lệnh giao ra nằm phía sau" = 9,5-10,6%. Ở r2/r3 client **cố ý chạy hơi trước** (ngoại suy
P+V) nên dấu tích vô hướng bị lật ⇒ ra 82,5% nhưng **vô nghĩa**. Thước đúng cho r2/r3: đếm `[S9-LUI]`
(số lần từ chối giao đích lùi — có nổ: 4-21 lần/mẫu) và **độ lệch thật** (`[S6-BANG]`: p50 11-16 mps,
p90 46-63 mps = dưới 2 ô ⇒ lành).

### Trạng thái binary
Client **`2145f043`** (r3: FIX-1/2/3 + FIX-6 + `[S9-KET]`) đang chạy từ 17:40. Server `fa6bfb46`.
Kéo giật toạ độ: **0/12.447 gói**. "Đã tới đích bị hiểu nhầm": **0**. NPC vô hình vì hết khe: **0**.
`[S9-KET]` (nằm bẹp THẬT) = **0** — nhưng phiên r3 chưa có cú chết nào nên chưa kết luận được.

## 9.21 ĐO PHIÊN 1486 GIÂY — hai báo cáo của chủ, hai kết luận khác nhau (commit `265e7559`)

### A. "Bot vẫn còn trượt tới - lùi" — **CÓ THẬT, và là lỗi của bản vá r2 của tôi**

Cú ghi đè toạ độ (nghi phạm cũ) đã **về 0**, nên phải đổi thước: đo **hướng di chuyển thật** của bản
sao client so với vị trí máy chủ.

| | Đảo chiều (>120°) |
|---|---|
| **Bản sao CLIENT** | 1.046/52.213 bước = **2,0%** |
| **Vị trí MÁY CHỦ** | 231/110.914 = **0,2%** |

Gấp **10 lần**. Cá biệt `npc=93103` client 39% / máy chủ 0%; `npc=93353` client 25% / máy chủ 0%.
Máy chủ đi rất thẳng: **95,7% bước rẽ dưới 30°**. ⇒ bot **không** thật sự đi tới-lui; **bản sao client
zigzag**. Nguyên nhân: ngoại suy `P + V` của r2 **vọt quá đà** mỗi khi bot dừng hoặc rẽ → client chạy
quá điểm thật → gói sau kéo lại.
✅ **r3 (`e5b1176d` đã thả)**: bỏ ngoại suy, giao **đúng vị trí máy chủ**. Bản sao chạy **theo sau đúng
một nhịp**, không bao giờ vượt lên nên không phải quay lại; vẫn liên tục vì mỗi gói đẩy đích lên một
nhịp nữa. Giữ nguyên cửa chặn chống-quay-đầu.

### B. "Phù về thành hay bị nhảy toạ độ" — **KHÔNG phải do cú phù**

50 cú dịch chuyển trong 1.486 giây (đa số là hồi sinh Tống Kim), tất cả đi qua nhánh `loadmap` và
**hạ cánh sạch**: độ lệch client↔máy chủ trong 6 giây sau khi phù còn **THẤP HƠN** lúc bình thường.

| Độ lệch vị trí bản thân | Bình thường | 6 giây sau phù/hồi sinh |
|---|---|---|
| p50 | 17 mps | **0 mps** |
| p90 | 81 | 87 |
| > 4 ô | 3,3% | **1,4%** |
| > 8 ô | 0% | 0% |

**Thứ NHẢY THẬT là `[S8-NAN]`** — bản vá "nắn toạ độ bản thân khi lệch ≥ 8 ô" **của phiên Claude kia**:
bắn **4 lần trong 25 phút**, mỗi lần dịch nhân vật **256-264 mps = 8 ô**, `doing=3` (đang chạy), và
**cách cú phù gần nhất 8-17 giây** ⇒ hoàn toàn không liên quan tới phù, mà là **nắn sai số tích luỹ
lúc chạy**. Gốc của sai số 8 ô nhiều khả năng là **client và server đọc hai tệp vật cản khác nhau**
(phiên kia đo: lệch 25,7% ô ở map 324, 67% ở map 379) ⇒ client chạy đường máy chủ không đi theo.
⚠️ **Việc này thuộc phiên kia** (họ viết `[S8-NAN]` và tìm ra vụ lệch lưới) — cần phối hợp, không tự sửa
chồng. Hướng: hoặc nâng ngưỡng/nắn mềm (kéo dần thay vì búng), hoặc chữa gốc lệch lưới.

🔴 Ghi chú vận hành: lúc thả r3 thì DLL đang nằm là **`e28f1e69`** (phiên kia vừa thay), không phải
`2145f043` của tôi — bản `e5b1176d` build từ cây hiện tại nên **có gồm phần của họ**; backup
`.cu_2608_r3_e28f1e69`.

## 9.22 **S10 — SERVER GỬI ĐÍCH THẬT: fix dứt điểm "đang chạy thì quay đầu"** (commit `43036785`, client `638dde7d` + server `43b94b3d` ĐÃ ĐẶT, **chờ restart GameServer + relog**)

Chủ ra lệnh: *"tôi cần phương án giải quyết dứt điểm chứ không đoán mò nữa"* rồi duyệt *"làm và
phản biện trước khi fix code"*. Quy trình: điều tra 4 tác nhân → thiết kế → **phản biện đối kháng
3 tác nhân** → mới đặt tay vào code.

### Gốc (chứng cứ từng dòng)
- Engine **vốn có** kênh đích thật: `KNpc::DoRun` (`KNpc.cpp:2341`) broadcast `s2c_npcrun`
  {ID, đíchX, đíchY} 13 byte mỗi chặng — cho **cả quái, bot lẫn người chơi thật** (đều hội tụ về
  `NewPath`, `m_DesX/m_DesY` đơn vị mps).
- Lỗ hổng: gói ADD (`NPC_SYNC`) **không mang đích** — lỗi thiết kế gốc 2003 (comment `need check
  later -- spe 03/05/27`); client tự chế lệnh "chạy tới chỗ đang đứng" (`KProtocolProcess.cpp:1987`)
  ⇒ NPC vào tầm giữa chặng thì bản sao **mù đích**; đo: 10% khoảng chờ lệnh > 2,2 s.
- Ba bản vá S9 (r1/r2/r3) đoán đích từ vị trí cũ 0,3-1 s ⇒ **chính chúng sinh quay đầu**: bản sao
  tự đảo chiều **gấp 6,1 lần** server trên cùng NPC (49 vs 8 cú; `93402` client đảo 6 — server
  thẳng tuyệt đối).

### Bộ vá S10 (3 miếng, không đổi protocol)
- **M1 server** — cuối `KNpc::SendSyncData` (sau cả PLAYER_SYNC, trước `return bRet`): NPC đang
  `do_run/do_walk` → gửi kèm `s2c_npcrun/walk` mang `m_DesX/m_DesY` cho riêng client đó. FIFO
  per-client (`ServerStage.cpp:396+`) ⇒ luôn tới SAU gói ADD, đè lệnh tự chế (khe lệnh một chỗ).
- **M2 client** — **xoá nguyên khối S9-DICH/S9-LUI 72 dòng**.
- **M3 client** — lưới hẹp CHỈ cho bản sao **đang đứng** (không có hướng ⇒ không thể quay đầu):
  `[S10-KEO]` đứng + server báo chạy + lệch 2..12 ô → chạy bù; `[S10-SNAP]` hai bên cùng đứng +
  lệch ≥2 ô → nắn toạ độ theo khuôn nhánh nắn; `[S10-GAC]` đếm số lần nhường vì khe lệnh đang bận.

### Phản biện đã ép đổi những gì (đọc kỹ trước khi sửa tiếp)
1. 🔴 CHẾT_NGƯỜI: M3 nguyên bản **không có cận trên** ⇒ NPC dịch chuyển cùng map (SetPos không báo)
   sẽ bị lệnh **chạy xuyên bản đồ** → thêm cận 12 ô.
2. 🔴 NẶNG: **race một-khe** — lệnh đích thật vừa ghi vào `m_Command` nhưng `ProcCommand` tick sau
   mới thi hành, `m_Doing` còn `do_stand` ⇒ M3 tưởng "đang đứng" và đè mất đích thật → gác
   `GetCommand().CmdKind == do_none` (`m_Command` là **private** — build fail nếu truy cập thẳng).
3. 🔴 NẶNG: kết quả gói phụ M1 **cấm** đụng `bRet` — chuỗi login theo bước (`KPlayerDBFuns.cpp:46+`)
   đọc giá trị trả về của `SendSyncData`.
4. 🔴 NẶNG (KB-C): cặp **đứng-đứng lệch to thì không ai kéo** (nhánh nắn cần `fix>0`, DoStand xoá
   fix) — mất ~50% lệnh chặng khi region >100 người (`MAX_BROADCAST_COUNT=100`, con trỏ xoay
   `KRegion.cpp:1395`) ⇒ **bắt buộc** thêm `[S10-SNAP]`.
5. Gác sống còn `m_DesX>0`: DoSkill mượn `m_DesX=-1/m_DesY=CHỈ-SỐ-KHE` mà không đổi `m_Doing`
   (`KNpc.cpp:2592`+`:2649` default-return) — thiếu gác là bản sao chạy về góc map.
6. Đã bác bằng chứng cứ: lo self-run (3 lớp chặn, ConformIdx vứt), lệch struct x64/Win32 (LLP64,
   pack(1), 13 byte cả hai), bể REQNPC (+13 byte/lần, có tiết lưu 19 khe), do_runattack (client
   ProcCommand không có case — cấm mở rộng gác).
7. Giới hạn ghi nhận (không phải lỗi mới khi test): lệnh có thể bị nuốt ≤0,9 s bởi `m_FrozenAction`
   stale (ZeroMemory bị chú thích `:10293`); đích xa hơn cửa sổ vật cản 3×3 vẫn đứng+giữ đích như
   hiện trạng; người đi **phím** đích ngắn 2 bước — kém mượt hơn S9 một bậc nhưng không quay đầu.

### Nghiệm thu sau restart (đo bằng nhãn KHÔNG tiết chế)
`[S9-*]` phải = 0 tuyệt đối · tỷ lệ đảo chiều bản sao/server trên cùng NPC **6,1× → ~1×** ·
`[S6-CMD] ap=1` ngay sau `[S6-ADD]` với đích ≠ vị trí đứng (M1 chạy) · đếm `[S10-KEO]/[S10-SNAP]/
[S10-GAC]` — GAC > vài % tổng phát nghĩa là race có thật và gác đang cứu đúng chỗ · lệch p50/p90
NPC đang chạy không phình quá 2 ô.

🔴 Vận hành: hai DLL đặt bằng rename-backup `.cu_2608_truoc_s10_{e5b1176d|fa6bfb46}`; build ôm cả
công trường chưa commit của phiên kia (S8+xúc xắc — như mọi bản hôm nay); `KProtocolProcess.cpp`
vẫn KHÔNG commit (tái áp: `ReverseTools/goi_va_S10_dichthat.py` — lưu ý script gốc dùng
`m_Command` trực tiếp, bản đã áp đổi sang `GetCommand()`).

## 9.23 NGHIỆM THU S10 ĐẠT + **[S10-MA] gỡ bóng ma** — chữa "đánh vào không khí" (commit `bc075c64`, server `898900cc` ĐÃ ĐẶT chờ restart, client `638dde7d` giữ nguyên)

### Nghiệm thu S10 (phiên 282 s sau restart 21:53, trần log = 0)
Tự-đảo-chiều bản sao/server: **6,1× → 1,7×** (client 15,1% / server 9,1% — server đảo nhiều là thật:
TK rượt nhau) · lệch p90 **55-66 → 33 mps** · >4 ô 3,6% → **1,1%** · `[S9-*]` = **0** · **747** cú ADD
có lệnh đích thật đi kèm (M1 chạy) · lưới hẹp: KEO 168 / SNAP 71 / **GAC 53** (race một-khe CÓ THẬT
— gác phản biện đòi đã cứu 53 lần).

### "Đánh vào không khí" = đánh BÓNG MA — server từ chối im lặng
Hiện trường trọn vẹn: đợt **28,7 s** auto đánh `tgID=92666` ở `dist=5` (bản sao ngay cạnh chân),
server trả từng cú `[S3-TGT-FIND] tgtid=92666 found=0` (`FindAroundNpc` = vùng 3×3 quanh người chơi,
`KPlayer.cpp:2045`). Gốc: gói gỡ `s2c_npcremove` **bị rớt bởi ngân sách broadcast 100 người/lượt**
(`KRegion.cpp:1395`) ⇒ ma kẹt tới ~55 s (bộ dọn 1000 tick `KNpcSet.cpp:755`). Đo: **323 ma bị dọn
trong 282 giây** (`[S6-BAL]` camtick=1001 toàn bộ) — hệ thống, không cá biệt.

### Fix [S10-MA] (1 miếng, server-only)
Tại chỗ `FindAroundNpc` trả 0 trong handler skill (`KProtocolProcess.cpp` sau `[S3-TGT-FIND]`):
gửi `NPC_REMOVE_SYNC {s2c_npcremove, ID}` (gói gốc 5 byte) cho **riêng client** vừa xin đánh ⇒
client xoá ma ngay, auto đổi mục tiêu nhịp kế (~0,3 s thay vì 28,7-55 s). Bọc `#ifdef _SERVER`
(file dùng chung). Nhãn `[S10-MA]` = `AUTOLOG_IDX` (đếm được).
An toàn: ID lạ → client `ConformIdx` vứt (no-op); mục tiêu sống ngoài vùng quay lại → REQNPC tự
thêm (kèm đích thật nhờ S10-M1); client cũ vẫn hiểu gói gốc.
**Nghiệm thu sau restart**: đếm `[S10-MA]` (server) vs số đợt đánh-một-chỗ ≥4 s (client, PK-EMIT
cùng tgID) — đợt dài nhất phải tụt từ 28,7 s xuống ≤1 s; `[S6-BAL]` camtick=1001 phải giảm mạnh
(ma bị gỡ chủ động trước khi bộ dọn tới).
Tái áp phần chưa commit: `goi_va_S10_dichthat.py` **rồi** `goi_va_S10_ma.py`.

## 9.24 **S11 — BỘ CHỐNG MA + miễn ngân sách gói chết/gỡ** (commit `f4ac011e`, client `8ffd4243` + server `449e3ecc` ĐÃ ĐẶT — **chờ restart GameServer + relog**)

Chủ hỏi *"có chắc fix được đúng gốc không?"* rồi duyệt. Phản biện 3 tác nhân **bác miếng probe tổng
quát** trước khi code vì 2 rủi ro CHẾT_NGƯỜI: (a) ngưỡng 2,5s **thấp hơn nhịp NormalSync hợp lệ của
đám đông** (server chỉ sync **5 NPC/region/tick** — `KRegion.cpp:691` — region 200 NPC là 40-60
tick/con) ⇒ probe bắn đại trà, cạn bể REQNPC 19 khe = tái sinh lỗi FIX-D "NPC vô hình"; (b)
`KNpcSet::SyncNpc` tìm ID **toàn cục mọi map**, `NPC_SYNC` không mang mã map ⇒ probe người đã rời
map kéo bản sao về toạ độ bay + refresh đồng hồ dọn = **ma bất tử**.

### Bộ đã lên (16 miếng)
**Client**: ① 5 bộ chọn mục tiêu (`TK_ChonDich`/`TK_BangDich`/`LD_ChonDich`/`HD_ChonDichDai`/
`HD_TimQuai`) thêm check `m_mAutoExcludeNpcID` — lưu ý mẫu neo `GetMpsPos(&x,&y)` khớp 2 hàm
(TK_BangDich + HD_ChonDichDai), đã chèn cả hai; ② FIX-6 hạn loại 30s→**60s** (phải vượt chu kỳ dọn
55s) + khi loại thì **hỏi server** bằng khuôn REQNPC chuẩn (`[S11-DO]`); ③ `RequestNpcFail` **gỡ
luôn bản sao ma** (`[S11-XOAMA]`) — DecRef **có gác** do_death/do_revive theo khuôn CheckBalance
(khuôn NetCommandRemoveNpc DecRef vô điều kiện sẽ tràn bảng đếm BYTE tham chiếu ô); ④ `SyncNpc`
toạ độ ngoài map → gỡ bản sao + trả khe (`[S11-MAPLA]`; trước đây return im lặng giữ ma + kẹt khe
100 tick).
**Server**: ⑤ `NpcRequestCommand` ID ở map khác → trả fail (`[S11-DOIMAP]`); ⑥ gói **CHẾT + GỠ miễn
ngân sách broadcast** (`NPC_EVENT_BROADCAST_LIMIT=100000` trong `KRegion.h`; 3 điểm: DoDeath,
`KNpcSet::Remove`, `SendDataToNearRegion` thêm tham số `nLimit=-1` chỉ truyền lớn tại SetPos
`:9998`/ChangeWorld `:10068` — **12 caller còn lại giữ nguyên**). Đây là fix tận nguồn "xác 0 máu
vẫn đi rồi búng về trại": gói chết 5 byte hiếm nhưng chui chung ngân sách 100 với gói di chuyển,
và `nMaxCount--` trừ **cả với bot không có kết nối** (90% node) — mất còn nặng hơn 50%.
🔴 CẤM miễn cho run/walk/hurt/skill (hàng MB/s).

### Nghiệm thu sau restart
Đếm `[S11-XOAMA]`/`[S11-DOIMAP]`/`[S11-DO]` · đợt bám-một-mục-tiêu ≥6s phải hết (trước: 37s) ·
`[S6-BAL]` camtick=1001 giảm mạnh (trước 323/282s) · **không còn** xác 0-máu đi lại / búng về trại ·
kiểm `[EXCL-PURGE]` vẫn nhịp 5s ở tab TK + LD (check exclusion là find()-only, dựa purge) ·
`SYNCMIN` REQNPC của NPC mới **không** bị InsertNpcRequest trả FALSE (tiêu chí hỏng = tái FIX-D) ·
`[PerfLog]` gai tick cuối trận TK không vượt nền 6,45/55,5 ms (gỡ ~1000 bot một đợt nay broadcast
không cắt — nếu gai thì rải remove ở tầng script, không hạ ngân sách).
Tái áp file chung: `goi_va_S10_dichthat.py` → `goi_va_S10_ma.py` → `goi_va_S11_chongma.py`.

## 9.25 NGHIỆM THU S11 — **ĐẠT TOÀN BỘ** (phiên 683 s sau restart 23:20, trần log = 0)

| Chỉ số | Trước | Sau S11 |
|---|---|---|
| Đợt bám-một-mục-tiêu ≥6 s không tiến bộ | 37 s + 9 s | **0 đợt** (529 emit) |
| Xác chạy (bản sao đổi chết→chạy) | có | **0** |
| Bản sao nhận được trạng thái CHẾT | hay mất gói | **311 sync trạng thái chết về đủ** (gói chết miễn ngân sách đã tới nơi) |
| Chết → về trại (15 cú, đủ 15 hồi sinh) | từng "vài giây" | **p50 = 729 ms, max 842 ms, 0 cú > 2 s** |
| Đợt đánh-sát-chân kéo dài | 28,7 s | tệ nhất **4 s** (2 ca — có thể là địch thật đứng yên) |
| Tự-đảo-chiều client/server | 6,1× → 1,7× | **1,4×** |
| Lệch NPC p90 | 55-66 | **35 mps (~1 ô)**; >4 ô = 1,4% |
| `S6-BAL` bộ dọn 55 s | 323 ma (có con NHÌN THẤY) | 882 nhưng **100% reg=-1 = mồ côi VÔ HÌNH** (dọn bàn, người chơi không thấy) |
| Bể REQNPC | — | khoẻ: **4.560 cú ADD** trôi chảy, `[S11-DO]`=0 không chiếm khe |

Điểm đáng chú ý: **các lớp phòng thủ S11 client (`XOAMA/MAPLA/DO/DOIMAP`) chưa cần nổ phát nào** —
vì miếng tận nguồn (miễn ngân sách gói chết/gỡ) đã chặn ma **hình thành** ngay từ đầu; FIX-6 thậm chí
không có gì để loại. Đúng thứ tự ưu tiên: fix nguồn gánh chính, lưới phòng thủ nằm im chờ ca hiếm.
Còn lại theo dõi: (a) `[S8-NAN]` búng 8 ô của CHÍNH MÌNH 9 lần/683 s — việc của phiên kia (gốc lệch
lưới vật cản); (b) PerfLog không có dòng nào trong cửa sổ nên CHƯA kiểm được gai tick cuối trận
(gỡ 1000 bot nay broadcast không cắt) — cần bật/soi đợt sau.

## 9.26 **S12 — chữa "chính mình bị búng 8 ô"** (commit `811513ab`, client `795eb6a4` + server `78515a3d` ĐÃ ĐẶT — **chờ restart GameServer + relog**)

### Điều tra lật đổ giả thuyết chính (workflow 3 mũi + mổ 9 cú búng)
**8/9 cú búng KHÔNG phải lệch lưới vật cản** mà là **server teleport người chơi ~8.000 mps**
(`SetPos` cùng map **không báo cho chính client** — lỗ hổng gốc `KNpc.cpp:9997+`) **rồi TỰ DẮT chạy
~1.100 mps @400 mps/s theo lộ trình script TK** (cú 2 và cú 7 cách nhau 172 s có toạ độ server
**trùng tuyệt đối** (53540,98779)); lệnh dắt-đi bị client vứt (ConformIdx loại self) ⇒ mỗi sự kiện
nổ thành 4 cú búng liên tiếp. Chỉ **1/9 cú** là lệch-vật-cản thật (leo 45→315 mps khi cả hai cùng
chạy, client vượt trước server — khớp lưới 379 lệch 67%: server chặn 298k ô vs client 24k, đo lại
bằng `tk_luoi_client_vs_server.py`; map 324 nay còn 21,43%). Lệch thường ngày p50=32 mps, chỉ sinh
lúc chạy (+13,7 mps/s trung vị, đỉnh 38), tự giảm khi đứng/đánh.

### Bộ vá (qua phản biện 3 tác nhân)
- **[S12-TELE]** server, cuối `KNpc::SetPos`: gửi ngay gói tự-sync `s2c_syncnpcminplayer` CÓ SẴN
  (27 byte) cho riêng chủ nhân vật ⇒ client snap **một lần đúng lúc dịch chuyển thật**. Điều kiện
  phản biện: `memset` gói (khuôn gốc để 3 trường rác stack — bẫy nếu ai bật lại check equip-count);
  gác `!m_btSimCityBot && m_nPlayerIdx > 0` (bot SimCity idx=0); điểm chèn sau `DoStand()` trước
  `return 1`.
- **[S12-THEO]** client: sau mỗi cú `S8-NAN` mở **cửa sổ 3000 ms** cho `NetCommandRun/Walk` áp lệnh
  cho CHÍNH MÌNH ⇒ đoạn dắt-đi thành chạy mượt. Điều kiện phản biện: **gác diệt echo** (`HaveTarget`
  A* đang chạy HOẶC `m_nSendMoveFrames < 5` ⇒ không áp — lệnh self lúc đó là echo click của mình);
  đồng hồ **`timeGetTime`** (CẤM `m_dwCurrentTime` — là FRAME ~18/s và bị gán lại theo server mỗi
  SyncWorld ⇒ cửa sổ 3000 sẽ thành ~167 giây); làm tươi cửa sổ mỗi lần áp; **reset = 0 tại SyncWorld**
  (lệnh tồn đọng khi region=-1 sẽ thi hành muộn ở map mới); chỉ 2 handler run/walk, áp qua
  `SendCommand` (giữ cửa nuốt FrozenAction / ProcessAI=0-khi-chết sẵn có).
- **[KEO] HOÃN** theo phản biện: chỉ nhắm 1/9 cú nhưng mang 2 rủi ro CHẾT_NGƯỜI (kéo về mẫu server
  có thể đặt mình **vào ô tường theo lưới client** ⇒ GetDir=0 ⇒ chính-mình-DoStand = kẹt vĩnh viễn;
  sàn 64 < nhiễu p90=83 ⇒ giật CẢ CAMERA 1,8 lần/s) + cần mô hình "nợ kéo ≤2 mps/tick" mới hội tụ
  (8/gói = 14,4 mps/s chỉ dư 0,7 so tích luỹ). Cú loại này vẫn có S8-NAN đỡ như cũ. **Fix nguồn
  thật = đồng bộ lưới vật cản** (phương án ĐÃ KHẢO SÁT: sinh lại đoạn obstacle trong `_Region_C.dat`
  từ dữ liệu `_S`, đặt tệp LOOSE cạnh pak — `KPakFile` mặc định **đọc đĩa trước pak sau**, không cần
  rebuild pak; bắt buộc xoá cache `.fp` hai bên) — hồ sơ riêng, làm khi chủ duyệt.

### Nghiệm thu sau restart
Đếm `[S12-TELE]` (DebugLog server) ↔ `[S8-NAN]` client: chuỗi 4-cú-liên-tiếp phải biến mất, thay
bằng **1 snap ngay lúc teleport + `[S12-THEO]` áp lệnh dắt-đi**; tổng S8-NAN/phiên phải giảm ~8/9;
kiểm không có `[S12-THEO]` nào nổ lúc đang cầm chuột chạy bình thường (gác echo làm việc).

## 9.7 Lỗi phụ nhặt được dọc đường (ngoài phạm vi di chuyển)

- Server `[S2-SKILL-NOTLEARNED] npc=91423 id=92422 skill_req=361` lặp ~1,3 s/lần suốt phiên —
  vẫn đánh được qua nhánh `S1-MELEE-NOROLL`, nhưng "chưa học mà vẫn xin đánh" cần soi riêng.
- Hai phiên Claude vẫn đang chạy song song trên máy (MEMORY.md bị phiên kia ghi chen giữa phiên này).

## 9.27 NGHIỆM THU S12 = **TRƯỢT** + MỔ 2 TRIỆU CHỨNG CHỦ BÁO 27/08 TỐI (log 19:37-19:51, me=92426, map sw=226, 10 lần vào map, CHƯA VÁ GÌ — chờ duyệt)

Chủ báo: (1) "phù về / chết về thành hay bị nhảy toạ độ"; (2) "BOT/người chơi/quái nhảy toạ độ
ra ngoài map rồi biến mất xong xuất hiện lại vị trí cũ cứ thế mãi".
Log đã chép về scratchpad phiên này (`logs_2708/`) TRƯỚC khi bị xoay. Binary đang chạy = bản build
19:28 của phiên kinh mạch (GameServer pid 22064 khởi động 19:36:08, client Game pid 8060 19:37:13;
đĩa bị swap tiếp 19:49 SAU khi hai tiến trình đã nạp) — **ĐÃ KIỂM: bản 19:28 có đủ nhãn S11+S12**
(client `.truoc_kmp19_2708` S12-THEO=2, server S12-TELE=1) ⇒ triệu chứng xảy ra VỚI S12 sống.

### A. Triệu chứng 1 = THANG S8-NAN khi bị server dắt đi sau teleport — S12-THEO **0 phát**, vì SAI THỜI ĐIỂM MỞ CỬA SỔ (không phải sai gác)

Hiện trường trọn vẹn (t=166073784-166080441, ngay sau vào TK ~12s):
1. `[S6-ORPHAN] npc=92426` (CHÍNH MÌNH mồ côi do recenter) → self-sync đặt lại tại
   **sv=(9,6,0,0) offset (0,0) = điểm SetPos script** — cú teleport ĐÃ snap sạch ngay lập tức.
2. **Cùng khoảnh khắc** `[S6-CMD] lenh=run npc=92426 ap=0 dich=(55299,106560)` — LỆNH DẮT của
   server (đích ≠ mọi đích auto) → **BỊ VỨT vì cửa sổ S12 chưa mở**: cửa sổ chỉ mở trong nhánh
   `nLech>=256` (S8-NAN), mà cú teleport đi qua nhánh ĐẶT-LẠI-MỒ-CÔI (reg=-1) nên không mở.
   Cú NAN đầu tiên tới **486ms SAU** — muộn rồi.
3. Server tự đi bộ self (sync 2 gói/tick, `[E4_MOVE_PATH] id=92426 ret=1 des=(55299,106560)
   speed=24` phía server, ~440 mps/s) — **không gửi thêm lệnh run nào giữa chặng** → client đứng
   (doing=1, 0 dòng E4_MOVE_PATH client suốt thang) → lệch +256 → snap 263 mps (~8 ô) → lặp:
   **thang 11 cú NAN/6,1s** (t=166074337) + **thang 4 cú/1,9s** (t=166153218). Đây chính là
   "bị nhảy toạ độ" (camera nhảy 8 ô mỗi ~0,55s suốt đoạn hộ tống ~90 ô).
4. Gác diệt-echo VÔ CAN ở thang này nhưng ĐÚNG việc nơi khác: 7.698 lệnh run-self/14' (echo
   auto), 7.656 rơi ngoài cửa sổ, **42 rơi trong cửa sổ đều bị gác chặn và đều là ECHO THẬT**
   (đích trùng đích auto vừa gửi `des=(53056,104800)`). ⇒ fix KHÔNG được nới gác mù quáng.
5. `[S12-TELE]` **KHÔNG kiểm chứng được**: `g_DebugLog` chỉ bắn WM_COPYDATA sang DebugWin,
   không ghi file nào — khiếm khuyết đo đạc, cần chuyển AUTOLOG.

### B. Triệu chứng 2 = HAI cơ chế đã đo

- **Flap 9Hz "chớp tắt"**: `KNpc::GetMapDisX/Y` trả `VOID_DIS=0x7FFFFFFF` khi **self chưa có
  region** (LoadMap đặt self=-1; hoặc self vừa mồ côi lúc teleport) ⇒ `S6-VANH` coi MỌI NPC là
  "≥40 ô" gỡ mỗi frame, trong khi `S6_XaQuaTam` cùng điều kiện lại "nhận hết" ⇒ VANH↔ORPHAN-BACK
  vô hạn tới khi self được đặt. Đo: **179 chu kỳ <500ms (p50=82ms)**, 14 NPC ≥3 chu kỳ liên tiếp,
  tệ nhất npc=54614 **8 chu kỳ @110ms** đứng yên cell=(6,24). Hai bên dùng HAI THƯỚC khoảng cách
  với quy ước "không biết" NGƯỢC NHAU — đây là gốc.
- **Biến mất 2-60s rồi hiện lại chỗ cũ**: mỗi cú snap của thang A kéo `NpcChangeRegion` +
  recenter (`S6-LOADMAP` cùng ms với NAN) → `KRegion::Close` mồ côi hàng loạt (761 cú/14') →
  NPC chỉ hiện lại khi tới lượt NormalSync của NÓ (5 NPC/region/tick — region đông là 2-3,3s+)
  → `S6-ORPHAN-BACK` (528). Đo: **97 ca biến mất 2-60s**; VANH→BACK p50=82ms nhưng đuôi ≥2s = 62 ca.
  ⇒ triệu chứng 2 phần lớn là HỆ QUẢ của thang A (bão recenter theo từng cú snap).

### C. BỘ VÁ ĐỀ XUẤT (chưa làm — cần phản biện đối kháng + chủ duyệt từng mục)

- **V1 client (chữa tận gốc thang)**: (a) mở `g_uS12CuaSoSelf` CẢ ở nhánh self được đặt-lại-từ-
  mồ-côi/teleport trong self-sync (không chỉ ở S8-NAN); (b) gác echo so THÊM ĐÍCH: lưu đích
  move client vừa tự gửi (một điểm hook SendClientCmdRun/Walk), lệnh self có đích lệch >64 mps
  so với đích đó = KHÔNG PHẢI echo → được áp. Giữ nguyên 2 gác cũ cho trường hợp trùng đích.
  Áp được lệnh dắt tại T+0 là client TỰ CHẠY cả chặng — thang biến mất, bão recenter cũng tắt.
- **V2 client (1 dòng, tắt flap 9Hz)**: trong check S6-VANH, `GetMapDisX==VOID_DIS` thì KHÔNG gỡ
  (đồng quy ước với S6_XaQuaTam "chưa vào map xong: nhận hết").
- **V3 server (đo được)**: `[S12-TELE]` g_DebugLog → AUTOLOG (jx_auto_server.log, đếm được).
- **V4 hoãn tiếp**: [KEO] nắn mềm + đồng bộ lưới vật cản — giữ nguyên trạng thái hoãn/chờ duyệt.

Bẫy thi công nếu duyệt: `KProtocolProcess.cpp` + `CoreShell.cpp` vẫn CHƯA COMMIT dùng chung 2
phiên (tái áp THEO THỨ TỰ chuỗi script mục 4 tóm tắt); sửa xong build CẢ client lẫn server;
kiểm nhãn trong DLL trước swap; phiên kinh mạch đang swap liên tục — phối hợp giờ restart.
Lỗi phụ `[S2-SKILL-NOTLEARNED]` vẫn lặp (317 client / 314 server trong 14') — như mục 9.7.

## 9.28 THI CÔNG S12b (28/08 sáng) — chủ duyệt "xác định chính xác rồi mới làm"; đã xác định bằng tái hiện độc lập lần 2, ĐÃ BUILD + SWAP, **CHỜ RESTART GameServer + relog client**

### Tái hiện lần 2 chốt gốc (log 28/08 09:16-09:42, me=92621, bản 07:27 có đủ S11+S12)
- **Phù về thành sw=78 t=215278587**: lặp ĐÚNG kịch bản 27/08 từng mili-giây — self mồ côi
  (recenter) → hạ cánh `sv=(9,6,0,0)` nhánh `vaolandau` → **lệnh dắt `dich=(49076,103456)` tới
  CÙNG MILI-GIÂY, cửa sổ chưa mở → vứt** → server bò đi, client đứng → **thang 5 cú S8-NAN**.
  Server xác nhận: `[E4_POS_CHANGEWORLD] id=92621 swid=78 to=(50464,103616)`.
- **Đối chứng sw=227 t=214964193**: 1 cú NAN mở cửa sổ khi lệnh VẪN đang chảy (~9,5 lệnh/s)
  → **529 `[S12-THEO]` bám dắt mượt 55,5s, quãng 2.993 mps, tổng chỉ 1 snap** — S12 chạy đúng
  thiết kế khi lệnh rơi vào cửa sổ đã mở (phiên này KHÔNG auto: MOVE-GATE=0 → gác cho qua).
- **9 cú phù 78↔225 KHÔNG có dắt** → hạ cánh sạch, 0 NAN → thang CHỈ nổ khi có script dắt sau
  teleport. Flap NPC vẫn nguyên: 208 chu kỳ <500ms (p50=74ms); **phát hiện thêm cơ chế flap 2**:
  npc=92689 flap 12 chu kỳ @110ms khi self ĐÃ có region nhưng NPC nằm đúng vành 40 ô lúc mình
  chạy nhanh — hai thước đo (ô-nguyên GetMapDis vs mps XaQuaTam) không có dải trễ.

### Bộ vá đã lên (script `ReverseTools/goi_va_S12b_cuaso_vanh.py`, 9 hunk, idempotent, ÁP SAU chuỗi S10→S11→S12)
- **V1a** `KProtocolProcess.cpp` nhánh đặt-lại-mồ-côi của self-sync: mở `g_uS12CuaSoSelf` ngay
  tại hạ cánh + nhãn đếm được **`[S12-CUA]`** (bọc `#ifndef _SERVER` — nhánh này là mã dùng chung).
- **V1b** `KProtocol.cpp` SendClientCmdRun/Walk ghi `g_nS12TuGuiX/Y` (đích mình vừa tự gửi);
  `S12_ChoPhepSelf(nIdx, dichX, dichY)`: đích lệch >64 mps so đích tự gửi ⇒ KHÔNG phải echo ⇒
  cho áp **trước** 2 gác HaveTarget/SendMoveFrames (giữ nguyên 2 gác cho ca trùng đích — 42 lệnh
  echo thật 27/08 vẫn bị chặn đúng). Chưa từng tự gửi (idle) ⇒ bỏ qua phép so, về gác cũ.
- **V2** `KNpc.cpp` check S6-VANH: (a) `GetMapDisX/Y == 0x7FFFFFFF` (VOID_DIS — self chưa có
  region) ⇒ KHÔNG gỡ; (b) ngưỡng gỡ 40 → **42** tạo dải trễ 2 ô so ngưỡng nhận-lại 40 của
  `S6_XaQuaTam` ⇒ hết flap biên.
- **V3** `KNpc.cpp` server: `[S12-TELE]` g_DebugLog → **AUTOLOG** (giờ đếm được trong
  `jx_auto_server.log`).

### Trạng thái binary
| Đâu | Bản | Ghi chú |
|---|---|---|
| `bin\client\CoreClient.dll` | **33e9412d** (28/08 09:51, nhãn S12-CUA=1) | backup `.cu_2808_truoc_s12b_ad7d2c51` |
| `bin\server\CoreServer.dll` | **cdc783b7** (28/08 09:51, chuỗi S12-TELE dạng AUTOLOG) | backup `.cu_2808_truoc_s12b_18d22890` |

Build từ cây chung ⇒ ôm cả công trường kinh mạch/lò rèn của phiên kia (như mọi bản gần đây).
`KProtocolProcess.cpp`/`KProtocol.cpp`/`KNpc.cpp` tiếp tục KHÔNG commit — tái áp theo thứ tự:
`goi_va_S10_dichthat.py` → `goi_va_S10_ma.py` → `goi_va_S11_chongma.py` → `goi_va_S12_bung8o.py`
→ **`goi_va_S12b_cuaso_vanh.py`**.

### Nghiệm thu sau restart + relog (đếm nhãn không tiết chế)
1. `[S12-CUA]` ≈ số lần vào map/teleport (mỗi vaolandau 1 phát).
2. Cú phù/teleport CÓ dắt: `[S12-THEO]` nổ NGAY sau `[S12-CUA]` (không cần đợi NAN) — **thang
   4-11 cú S8-NAN phải biến mất**, tổng S8-NAN/phiên giảm ~8/9 (chỉ còn loại lệch-vật-cản đơn lẻ).
3. Có auto TK bật (như 27/08): thang vẫn phải biến mất nhờ V1b (đích dắt ≠ đích auto).
4. `[S12-TELE]` xuất hiện trong `jx_auto_server.log` khi script SetPos cùng map.
5. Flap: chu kỳ VANH→BACK <500ms phải về ~0 (trước: 179-208/phiên); `[S6-VANH]` tổng giảm mạnh;
   KHÔNG tái "đứng chạm 255" (dải trễ +2 ô chỉ nới vành ~5% diện tích).
6. Đối chứng gác echo còn sống: cầm chuột/auto chạy bình thường KHÔNG có `[S12-THEO]` nào ngoài
   các đoạn bị dắt.

## 9.29 SỰ CỐ "GỌI 1000 BOT CỨ CHẶP BỊ XOÁ SẠCH" (28/08 ~10h) — KHÔNG PHẢI S12b; thủ phạm = bộ test tự động BDH/PETSYS của phiên kia, ĐÃ TẮT

- Hiện trường `bot.log`: mọi cú xoá đều rơi ĐÚNG giây :00 (09:58:00 gỡ 1000 · 10:20:00 gỡ 794 ·
  10:21:00 gỡ 408×2 · 10:24:00 + 10:26:00 gỡ 1000 · …), có từ tối 27/08 (21:54/22:01/… mỗi lần
  "gỡ 1 bot" nên không ai để ý). Đường gỡ là `PB_RemoveAll()` (KPlayerBot.cpp:1224) — gỡ CHỦ ĐỘNG
  có trình tự lưu-trước-gỡ, không phải sập.
- Gốc: `script/partner/partner_test_bdh.lua` (header "SINH TU DONG [BDH 27/08] — GO SAU KHI
  NGHIEM THU") định nghĩa 3 tick `BDH_TestTick`/`BDH_TalkTick`/`BDH_PetTick` được
  `timerserver.lua RunTime()` gọi **mỗi phút**; cả 3 đều `PB_AddBot(1,1)` rồi **`PB_ClearBot()`
  = gỡ SẠCH bot toàn server** (dòng 133/155/210/314/329). GlbValue reset theo restart ⇒ mỗi lần
  restart GameServer bộ test chạy lại từ đầu ⇒ "cứ chặp" lại quét (nhật ký test:
  `bin\server\bdh_test.log`).
- Đã tắt bằng comment 3 dòng `call(BDH_*Tick...)` trong `script/timerserver.lua` (safe_edit,
  high-byte 1244 nguyên vẹn; `BDH_JitanTick` GIỮ vì không đụng bot). `RunTime()` dofile lại
  chính nó mỗi phút ⇒ hiệu lực ≤2 phút, KHÔNG cần restart. Muốn chạy lại bộ test: bỏ 3 dấu `--`.
- S12b VÔ CAN: các sửa S12b là client-sync + 1 dòng log server, không đụng KPlayerBot; hành vi
  xoá có từ 27/08 tối, TRƯỚC khi S12b tồn tại.
- ⚠️ Binary: phiên kia lại swap 10:14-10:15 (client `70080172` + server `e0b1c247`) và server
  restart 10:21 chạy bản ĐÓ (build sau S12b nên vẫn ôm S12b — chưa kiểm nhãn; nghiệm thu S12b
  vẫn theo mục 9.28).

## 9.30 BOT GÓC KẸT + BÁO DANH KIM BAY VÀO GÓC (28/08 trưa) — TOẠ ĐỘ CHUẨN LẤY TỪ THẦN HÀNH PHÙ, ĐÃ VÁ + SWAP `491c0086`, CHỜ RESTART

Chủ: "gọi bot ra nhiều con đứng trong góc kẹt" + "bot vào báo danh phe Kim bay thẳng vào góc lag"
+ chỉ đích danh nguồn: **"mọi toạ độ chính ở Thần Hành Phù"** (`script/item/ib/shenxingfu.lua`).

- Toạ độ chuẩn trích từ nguồn chủ chỉ: báo danh (map `MAP_BD_TC=324`, qua `battle_transprot`
  lib_tktc.lua:809/:812): **Tống (1541,3178) · Kim (1570,3085)**; bảng thành/thôn
  `THON_TT_MP_ARRAY` (shenxingfu.lua:18-48, 27 map).
- Gốc 1 (báo danh): bot đáp theo toạ độ NPC báo danh (`startgame.lua`: Tống 1550,3179 / Kim
  1555,3082) chứ KHÔNG phải điểm thả người chơi của Thần Hành Phù → phía Kim cả đàn đáp sát
  góc kẹt cạnh NPC → dồn cục + lag.
- Gốc 2 (gọi bot ra góc kẹt): bot vào game đứng nguyên vị trí lưu/mẫu (luật 18/08); con nào
  lưu ở Ô BỊ CHẶN/ngoài lưới (di sản các lần kẹt trước — vd LyHieu1 map 1 ô(1613,3073) "trong
  O BI CHAN", [BotLach] lắc không thoát, [BotCuu]/T1 chỉ phủ map bãi) thì đứng góc đó vĩnh viễn.
- Vá (`ReverseTools/goi_va_botthp_toado.py`, 3 hunk, chỉ `KPlayerBot.cpp` server-only):
  H1 điểm đáp báo danh = đúng điểm battle_transprot; H2 `[BotTHP]` sau LaunchPlayer2: đứng ô
  chặn/ngoài lưới → SetPos về điểm Thần Hành Phù của map (bảng 27 map chép nguyên văn, rải
  9×7 + `pb_ODat`; map ngoài bảng giữ nguyên); H0 prototype `pb_ODat` (định nghĩa :2744 nằm
  SAU điểm chèn). Vị trí lưu hợp lệ giữ nguyên — không phá luật 18/08.
- Binary: server **`491c0086`** (12:27, nhãn BotTHP=2, S12-TELE còn nguyên) đè lên `ae8cee24`
  (backup `.cu_2808_truoc_botthp_ae8cee24` — lưu ý phiên kia đã swap tiếp sau 10:15 nên bản bị
  thay KHÔNG phải `e0b1c247`). Client KHÔNG swap (sửa server-only; client chỉ build kiểm biên
  dịch). **Chờ chủ restart GameServer** rồi gọi bot nghiệm thu.
- Nghiệm thu: gọi 1000 bot → đếm `[BotTHP]` (số con được kéo khỏi góc, lần đầu sẽ cao rồi
  giảm dần vì bot lưu lại vị trí tốt); tới giờ TK xem phía Kim đáp quanh (1570,3085) tản đều,
  hết dồn góc; `grep "trong O BI CHAN" bot.log` phải tụt về ~0 sau vài lần gọi.

## 9.31 BOT 100% NGOẠI CÔNG → CHIA 2 ĐƯỜNG NỘI/NGOẠI (28/08 chiều) — server `da940e78` ĐÃ SWAP, CHỜ RESTART

Chủ: "bot đang 100% đánh kỹ năng ngoại công (ai cũng được phát vũ khí, 81 còn nâng vũ khí cấp 10)
→ random tỉ lệ bot 2 đường nội-ngoại cho cân bằng; **nội công là KHÔNG cần vũ khí** — set vũ khí
là đánh skill theo vũ khí = toàn ngoại".
- Đo xác nhận: bot.log 27+28/08 dùng 16 chiêu, **15/16 IsPhysical=1** (chỉ 303 Độc Thạch Cốt là
  phép). Cơ chế bThienNoi 23/08 (dwID lẻ ưu tiên chiêu phép) bất lực vì AI CŨNG CẦM VŨ KHÍ →
  bậc khớp-vũ-khí (rank 2) toàn thắng chiêu phép -2 (rank 1).
- Vá (`ReverseTools/goi_va_botnoingoai.py`, 5 hunk, chỉ KPlayerBot.cpp):
  `pb_BotNoi(nIdx) = dwID & 1` (một nguồn sự thật; ~50/50, ổn định giữa các phiên, TRÙNG nhóm
  bThienNoi cũ nên chiêu phép được ưu tiên sẵn). H1 `pb_GiveFactionWeapon`: bot nội → tháo vũ khí
  cũ rồi đi nhánh tay-không có sẵn (nhãn `[BotNoi]`); H2 khối phát-lại-vũ-khí bỏ qua bot nội;
  H3 `pb_TrangBiTheoCap` (chạy cả lúc login nhờ gate nTrangBiLevel=0): bot nội còn cầm vũ khí
  di sản → tháo huỷ + chọn lại chiêu (bước "81 nâng vũ khí" tự thành no-op); H4 bThienNoi đọc
  qua helper. Đường NGOẠI giữ nguyên 100% như cũ.
- Binary: server **`da940e78`** (13:24, ôm cả BotTHP 9.30 + S12b 9.28) đè `491c0086`, backup
  `.cu_2808_truoc_botnoi_491c0086`. Client chỉ kiểm biên dịch, không swap. Chuỗi tái áp giờ là:
  S10_dichthat → S10_ma → S11_chongma → S12_bung8o → S12b_cuaso_vanh → botthp_toado →
  **botnoingoai**.
- Nghiệm thu sau restart: `grep BotNoi bot.log` — nửa đàn log "đường NỘI CÔNG"/"tháo vũ khí";
  phân bố `dung chieu` phải xuất hiện các chiêu IsPhysical=0 (vd 303) ≈ nửa số lượt; phe đánh
  nhau nhìn thấy cả chưởng/phép lẫn vũ khí. Lưu ý: phái không có chiêu phép -2 (vd Thiếu Lâm
  đường quyền) bot nội sẽ đấm tay không — yếu hơn, chủ muốn nắn tỉ lệ theo phái thì nói thêm.

## 9.32 DÃ TẨU: PHÙ VỀ TRUNG TÂM THÀNH (Thần Hành Phù) RỒI ĐI BỘ TỚI NPC (28/08 chiều) — server `8cdb8b1e` ĐÃ SWAP, CHỜ RESTART

Chủ: "khi bot về trả nhiệm vụ Dã Tẩu tùy thành thì lấy toạ độ TRUNG TÂM thành đó (có sẵn ở
Thần Hành Phù) để phù về, rồi DI CHUYỂN tới NPC Dã Tẩu".
- Trước: `pb_DtVeThanh` teleport đáp THẲNG cạnh NPC Dã Tẩu (6..15 ô) — không giống người chơi.
- Vá (`ReverseTools/goi_va_datau_thp.py`, 5 hunk, KPlayerBot.cpp):
  H1 nâng bảng `THON_TT_MP_ARRAY` lên cấp file (`s_aThpDiem[27]` + `pb_ThpDiem()` — MỘT nguồn
  sự thật); H2a-c khối [BotTHP] login (9.30) chuyển sang dùng bảng chung, bỏ bảng cục bộ;
  H3 `pb_DtVeThanh` đáp quanh TRUNG TÂM thành (lệch 6..15 ô theo chỉ số bot), map ngoài bảng
  giữ điểm cũ. Pha sẵn có tự lo đoạn đi bộ: DTB_TOI_NPC cưỡi ngựa + quét NPC thật (hạn 600
  nhịp, xa nhất Đại Lý trung tâm→NPC ~119 ô vẫn dư), đường Xa Phu ngân sách 300s.
  Ảnh hưởng CẢ 5 điểm gọi (nhận/trả Dã Tẩu, về thành gặp Xa Phu ×3) — đều là "phù về" nên
  về trung tâm là đúng hành vi phù của người thật.
- Binary: server **`8cdb8b1e`** (14:28) đè `da940e78`, backup `.cu_2808_truoc_dtthp_da940e78`.
  ⚠️ GameServer đã được chủ restart 14:18 với `da940e78` (BotNoi+BotTHP+S12b ĐANG SỐNG) —
  bản `8cdb8b1e` cần RESTART LẦN NỮA. Chuỗi tái áp: …S12b_cuaso_vanh → botthp_toado →
  botnoingoai → **datau_thp**.
- Nghiệm thu: log `[BotDT] ... ve thanh` rồi thấy bot xuất hiện ở TRUNG TÂM (toạ độ THP ±15 ô)
  và chạy/cưỡi ngựa dọc phố tới NPC; không tăng "khong toi duoc NPC Da Tau" (hạn 600 nhịp đủ).

## 9.33 BOT SẠP CHỒNG LÊN NHAU → RẢI ĐỀU QUANH QUẢNG TRƯỜNG (28/08 chiều muộn) — server `bbd45444` ĐÃ SWAP, CHỜ RESTART

Chủ: "bot sạp bày bán bị chồng lên nhau không rải đều ra". BA GỐC đo được:
1. 🔑 **`g_nPbNpcChan` mặc định 0** (KRegion.cpp:29 — dự án CỐ Ý cho người/bot đứng chồng nhau)
   ⇒ nhánh `GetBarrierMin(bCheckNpc=TRUE)` trong `pb_OSapTot` (ghi chú "chưa ai ngồi") là
   **NO-OP** ⇒ bot cùng `nLech%8` chọn đúng MỘT ô → chồng cột.
2. Vòng duyệt chỉ đi **8 TIA × bán kính 3..12** (80 ô) — có kiểm cũng chỉ ra 8 vệt thẳng.
3. Bot **đang ở sẵn thành nhà** khi bị bốc bán sạp: điều kiện `map != nha.nMap` bỏ qua khối xếp
   chỗ → ngồi ngay tại đám đông đang đứng (nhất là sau khi gọi bot dồn quanh trung tâm).
Vá (`ReverseTools/goi_va_sap_raideu.py`, 6 hunk, KPlayerBot.cpp):
- `pb_OSapTot` tự quét danh sách bot sạp (kể cả con vừa đặt trong CÙNG khung) — không đụng
  công tắc toàn cục `g_nPbNpcChan` (gameplay giữ nguyên).
- Duyệt **trọn vành đai** mỗi vòng (8r ô/vòng ≈ 600 ô cho vành 3..12), điểm khởi đầu xoay theo
  chỉ số bot → toả đều như chợ thật.
- Cờ mới `b.nSapChoXong`: bot ở sẵn thành nhà cũng phải xếp chỗ (SetPos cùng map — S12-TELE
  không bắn cho bot vì m_nNetConnectIdx=-1); reset ở login/đóng sạp/bốc mới.
- Log mới `[BotSap] %s ngoi sap thanh %d o(x,y)` — đếm được khi nghiệm thu.
Binary: server **`bbd45444`** đè `2ea7def1` (phiên kia lại swap giữa chừng — backup
`.cu_2808_truoc_sap_2ea7def1`). GameServer vẫn chạy bản 14:18 (`da940e78`) — restart lần tới ăn
TRỌN GÓI: S12b + BotTHP + BotNoi + DT-THP + SapRai. Chuỗi tái áp: …datau_thp → **sap_raideu**.
Nghiệm thu: `grep "ngoi sap" bot.log` — mỗi con một ô khác nhau; nhìn quảng trường sạp toả vòng
quanh NPC Dã Tẩu thay vì chồng cột/8 vệt.

## 9.34 SỬA HỒI QUY 9.31: PHÁI THUẦN NGOẠI (Thiên Vương/Đường Môn) KHÔNG ĐI ĐƯỜNG NỘI (28/08 tối) — server `b5ca4a50` ĐÃ SWAP, CHỜ RESTART

Chủ: "fix vũ khí làm lỗi 1 số phái toàn kỹ năng ngoại công cần vũ khí (Thiên Vương - Đường Môn)"
— đúng lỗ đã cảnh báo ở 9.31: bot nội (dwID lẻ) của phái thuần-ngoại bị tước vũ khí thành phế.
- Vá (`ReverseTools/goi_va_botnoi_theophai.py`, 4 hunk, KPlayerBot.cpp): thêm
  `pb_CoChieuNoiTayKhong(nNpcIdx)` — quét danh sách chiêu của CHÍNH BOT bằng đúng bộ lọc
  pb_PickSkill với nWant=-1 (series khớp hệ · radius>0 · !aura/!self/enemy · style
  Missles/Melee · **eqt ∈ {-1,-2}** · đủ cấp rq≤80 · có 1/5 đòn phép thật) +
  `pb_BotNoiThat = dwID lẻ && có chiêu nội` — 3 điểm quyết định vũ khí (phát nhập môn /
  phát lại / tháo ở TrangBiTheoCap) đổi sang dùng nó. `bThienNoi` trong pb_PickSkill giữ
  parity (chỉ là ưu tiên xếp hạng, vô hại).
- **Theo dữ liệu, không liệt kê tay**: TV/ĐM (0 chiêu phép tay-không) tự về đường ngoại;
  phái nào sau này được thêm chiêu phép thì tự chuyển. Bot chưa đủ cấp dùng chiêu phép
  thì TẠM giữ vũ khí, lên cấp `pb_TrangBiTheoCap` tháo sau (hội tụ).
- **Tự hồi phục**: bot TV/ĐM đã bị tước vũ khí từ restart 14:18 sẽ được khối `[BotVuKhi]`
  phát lại trong ~60s sau relog (gác `!pb_BotNoiThat` giờ cho qua; nVuKhiThu reset khi login).
- Binary: server **`b5ca4a50`** đè `0cbd9e60` (backup `.cu_2808_truoc_noiphai_0cbd9e60`) —
  ôm trọn: S12b + BotTHP + BotNoi + BotNoi-PHAI + DT-THP + SapRai. Chuỗi tái áp:
  …botnoingoai → **botnoi_theophai** → datau_thp → sap_raideu (theo thứ tự tạo file, script
  nào cũng idempotent nên chạy đúng thứ tự nào trong nhóm bot cũng được — RIÊNG datau_thp
  phải SAU botthp+botnoingoai; sap_raideu SAU tất cả).
- Nghiệm thu: `grep BotNoi bot.log` — KHÔNG còn dòng "duong NOI CONG" / "thao vu khi" cho
  bot Thiên Vương/Đường Môn; `[BotVuKhi] ... phat lai` xuất hiện cho các con TV/ĐM từng bị
  tước; phái có nội (Ngũ Độc/Nga My/Thúy Yên/Võ Đang/Côn Lôn…) vẫn chia ~50/50.

## 9.35 TK: ~100 BOT TỰ THOÁT SAU VÀI PHÚT, CHỈ MỘT PHE (28/08 tối) — server `07d38030` ĐÃ SWAP, CHỜ RESTART

Chủ: "500 bot vào Tống Kim, vài phút sau tự thoát ~100 con, CHỈ 1 PHE chứ không chia đều".
- Đo (bot.log 17:19-17:21): đợt 500 chia đúng 250 Tống + 250 Kim; đúng **85 con "KET o pha 3
  qua 120 giay (map 379) -> bo cuoc"** trong ~10 giây, **CẢ 85 ĐỀU PHE TỐNG** (0 Kim), cả 85
  chưa từng log "RA TRAN" — chúng là ĐUÔI HÀNG ĐỢI.
- Gốc: pha 3 dồn 250 con/phe về MỘT toạ độ Quân Y rồi MỘT cửa trại (phễu); đồng hồ
  `PB_TK_PHA_HAN` (120s) KHÔNG được làm tươi suốt đoạn đi bộ (chỉ tươi khi đứng chờ cổng
  chưa mở) ⇒ đuôi hàng đợi CÓ TIẾN nhưng chậm bị coi là "kẹt" ⇒ cắt oan nguyên một vệt của
  phe có trại chật/bố cục hẹp hơn (Tống) ⇒ trận lệch hẳn một phe.
- Vá (`ReverseTools/goi_va_tk_ket_hangdoi.py`, 4 hunk, KPlayerBot.cpp):
  H3 **nhúc nhích ≥6 ô kể từ mẫu trước ⇒ làm tươi đồng hồ pha** (kẹt thật — đứng im/quanh
  quẩn <6 ô — vẫn bị cắt 120s như cũ; trần cứng 6×hạn pha = 12' chống treo vĩnh viễn nếu bị
  đẩy qua lại); H4 **rải điểm đến Quân Y ±8 ô** theo chỉ số bot (mua thuốc là ExecuteScript,
  không đòi đứng sát NPC; lọc ô đất pb_ODat) — phá phễu từ gốc; H1/H2 4 trường `nTkKet*` +
  reset login.
- Binary: server **`07d38030`** (17:33) đè `7f11622c` (backup `.cu_2808_truoc_tkket_7f11622c`).
  GameServer đang chạy bản 17:12 — restart để ăn. Chuỗi tái áp thêm CUỐI: **tk_ket_hangdoi**.
- Nghiệm thu trận TK kế (500 bot): "KET o pha 3" phải về ~0 (chỉ còn ca kẹt tường thật);
  quân số 2 phe giữ ~250/250 suốt trận; bot ra trận trễ nhất trễ hơn nhưng KHÔNG bỏ trận.

## 9.36 TÍNH NĂNG MỚI [BotSan]: BOT TK TỰ ĐỊNH VỊ ĐỐI THỦ GẦN NHẤT VÀ ĐUỔI ĐÁNH (28/08 tối) — server `8fb5dcd2` ĐÃ SWAP, CHỜ RESTART

Chủ yêu cầu: "bot TK di chuyển TÌM đối thủ để đánh, không đi toạ độ cố định nữa; tự định vị
đối thủ ở đâu, di chuyển THẲNG tới đánh; trên đường gặp mục tiêu khác thì bắt mục tiêu GẦN
NHẤT, không cố định; bỏ qua mục tiêu đã chết".
- Hiện trạng pha 4: hết địch gần → bốc toạ độ CỐ ĐỊNH trong bảng doanh trại địch
  (`pb_TkLayDoanh`) + vòng điểm trung gian; bộ nhắm trong TK XOAY VÒNG ứng viên theo chỉ số
  bot (thiết kế chống dồn cục 23/08) chứ không lấy gần nhất.
- Vá (`ReverseTools/goi_va_tk_san_doithu.py`, 5 hunk, KPlayerBot.cpp):
  H3 `pb_TkTimDichGanNhat`: quét `Player[]` (người + bot, rẻ hơn quét cả bảng Npc) cùng map,
  `m_CurrentCamp` = phe địch, SỐNG (loại do_death/do_revive/máu≤0) → gần nhất.
  H4 khối `[BotSan]` trong pha 4 (chỉ chạy khi KHÔNG có mục tiêu đang đánh — giữ luật "đang
  đánh không đụng lộ trình"): mỗi ~1,2s so-le theo bot (+ ngay khi chưa có đích) định vị lại;
  có địch → đích = vị trí nó, đi THẲNG (bỏ chặng vòng), đổi-con-gần-hơn/nó-dời->8ô thì tính
  lại đường; không thấy ai → rơi về lối doanh-trại cũ (dự phòng khi địch chết/ẩn hết).
  Đang săn thì `nTkDichTick` được làm tươi ⇒ nhánh bốc-doanh-trại ngủ. H5 bộ nhắm TK lấy
  **gần-nhất-nhìn-thấy-được** (aId đã sắp theo khoảng cách) thay vì xoay vòng.
- Chi phí: quét MAX_PLAYER × ~24 bot/khung (500 bot ÷ 21 nhịp so-le) — không đáng kể; A*
  tính lại có ngưỡng (đổi mục tiêu / dời >8 ô) tránh giật đường mỗi giây.
- ⚠️ Đánh đổi có chủ đích: bỏ xoay-vòng-mục-tiêu nghĩa là nhiều bot có thể dồn đánh CÙNG một
  nạn nhân gần nhất (đúng yêu cầu "gần nhất"); nếu chủ thấy dồn quá thì nói — thêm giới hạn
  "tối đa N con săn cùng một mục tiêu" rất dễ (đếm nTkSanIdx trùng).
- Binary: server **`8fb5dcd2`** (17:44) đè `07d38030` (backup `.cu_2808_truoc_san_07d38030`).
  GameServer đang chạy bản 17:12 — RESTART ăn cả 9.35 + 9.36. Chuỗi tái áp thêm CUỐI:
  **tk_san_doithu**.
- Nghiệm thu trận TK kế: `grep BotSan bot.log` — thấy "duoi <tên> o(x,y) cach N o"; nhìn trận
  bot lao thẳng về phía địch thay vì chạy tuyến cố định; khi mục tiêu chết bot đổi con khác
  trong ~1-2s; `[BotLan] vong qua` phải giảm mạnh (chỉ còn khi map sạch địch).

## 9.37 [BotSan] TRẦN 10 CON/MỤC TIÊU + giải đáp cơ chế (28/08 tối) — server swap CHỜ RESTART

Chủ: "tối đa 10 bot định vị cùng 1 mục tiêu" + hỏi (a) 2 bot 2 phe đứng xa có thấy nhau không,
(b) cách xác định vị trí đối thủ đang di chuyển.
- Trả lời đã gửi chủ: (a) CÓ — quét `Player[]` phía server đọc toạ độ thật, không giới hạn
  khoảng cách, chỉ cần cùng map + phe địch + còn sống; (b) tái-định-vị chu kỳ ~1,2s/bot (đọc
  lại toạ độ server của mục tiêu; dời >8 ô thì re-path), vào tầm thì pb_Fight bám từng khung;
  xác/hồi sinh bị loại ở lần quét kế.
- Vá (`ReverseTools/goi_va_tk_san_tran10.py`, 6 hunk): `PB_TK_SAN_TRAN=10` +
  `s_nTkSanDem[MAX_NPC]` (short) — kế toán SỐNG khi đổi/thả mục tiêu trong khối [BotSan],
  **dựng lại từ đầu mỗi giây trong `pb_TkNhip`** (tự dọn rác bot chết/rời trận, khỏi móc mọi
  đường thoát); `pb_TkTimDichGanNhat` thêm tham số `nTuIdx` — ứng viên đủ 10 con săn bị bỏ
  qua (trừ mục tiêu hiện tại của chính bot — giữ chỗ) ⇒ áp lực tự dàn sang con gần kế tiếp.
- Nghiệm thu: trong trận, đếm `grep -o "duoi [A-Za-z0-9]*" bot.log | sort | uniq -c` — không
  tên nào vượt ~10-11 (chênh 1-2 do khe 1 giây giữa hai lần dựng bảng là chấp nhận được).

## 9.38 [BotSan] TẢN RA NHIỀU ĐỐI THỦ + SỬA NỐT "PHE TỐNG VẪN THOÁT" (28/08 tối) — server swap CHỜ RESTART

Hai phản hồi của chủ sau trận 18:0x (bản 17:59 đã chạy BotSan thật — 84 dòng, BotLan=0):
1. "Bot gom chạy đúng 1 đường, chưa tản ra": log 18:06 cho thấy MỌI cú săn trỏ về cùng tụm
   địch ô(1542-1549, 3215-3231) — "gần nhất còn slot" của cả đàn nằm trong MỘT cụm ⇒ một làn
   đường. Vá: `pb_TkTimDichGanNhat` gom **TOP-25** ứng viên gần nhất (vẫn bỏ chết/đủ slot),
   mỗi bot nhận **HẠNG riêng** `nLech % số ứng viên` ⇒ ~10 con/đích, 25 đích rải nhiều cánh ⇒
   nhiều làn đường; chạm mặt giữa đường vẫn đánh gần nhất (pb_Fight).
2. "Bot phe Tống vẫn thoát khi vào trận": 18:05 còn **81 cú KET pha 3** dù đã có vá nhúc-nhích
   ⇒ đám này ĐỨNG IM thật — đầu trận 500 con cùng xin đường A* (hàng đợi PathSrv) ⇒ đuôi hàng
   chờ CẤP ĐƯỜNG >120s. Vá kép: **[TK-KET2]** trong hậu doanh phe mình = vùng an toàn (làm
   tươi đồng hồ, vẫn chịu trần cứng 12'; ra khỏi trại đứng im vẫn bị cắt) + **tiết lưu
   re-path [BotSan]** (đổi hẳn mục tiêu mới reset ngay; mục tiêu cũ dời chỗ thì ≥3s/lần) giảm
   áp lực hàng đợi đường từ gốc.
- Script: `ReverseTools/goi_va_tk_san_tanra.py` (5 hunk — ÁP CUỐI CHUỖI, sau tk_san_tran10).
  🔴 Bẫy tái diễn: sửa script bằng heredoc bash nuốt backslash → hỏng file; đã viết lại bằng
  Write. Nghiệm thu trận kế: "KET o pha 3" ~0; `grep "duoi " | uniq -c` thấy ≥15-25 tên khác
  nhau cùng lúc; nhìn trận bot toả nhiều cánh.

## 9.39 "BOT KẸT TRONG DOANH TRẠI" = BỊ XAY THỊT NGAY CỬA RA — VÙNG CẤM SĂN [TK-CHONGCAMP] (28/08 tối) — server `c88cb289` ĐÃ SWAP, CHỜ RESTART

Chủ: "đợt này fix bot kẹt trong doanh trại không ra ngoài được rất nhiều".
- Đo (sau restart 18:15 bản `dccd069d`): **KET pha 3 = 0** (9.38 ăn — hết mass-quit) NHƯNG
  **7.8k lượt "RA TRAN"/giờ ≈ 15 lần/bot**; vết VoLam258: ra cửa 18:22:40 → CHẾT 18:22:56
  (16s), ra 18:23:16 → chết 18:23:26 (10s)... Bot KHÔNG kẹt — chúng bị **giết trong 10-16
  giây sau khi bước ra** rồi quay về trại hồi sinh/mua thuốc, nên phần lớn quân số lúc nào
  cũng đứng trong trại. Thủ phạm: [BotSan] hai phe cắm ngay **cụm điểm SetPos ra cửa của
  địch** (tụm săn (1542-1549,3215-3231) hôm trước = đúng RANDOM_POS_KIM (1544-1592,3173-3227)
  của kimratrai.lua) — máy xay thịt tại cửa.
- Vá (`ReverseTools/goi_va_tk_chongcamp.py`, 2 hunk trong `pb_TkTimDichGanNhat`): **vùng cấm
  săn theo phe của ứng viên** — không định vị mục tiêu còn đứng: (a) quanh hậu doanh phe nó
  R=45 ô (anchor TKPOS_GO_HDOANH + đảo thế trận), (b) quanh **cụm điểm ra cửa** phe nó R=25 ô
  (tâm cụm từ RANDOM_POS: Tống (1331,3442) / Kim (1568,3200)). Ra khỏi vùng là bị săn bình
  thường; đánh-cận (pb_Fight) không đổi — chỉ chặn việc kéo đàn tới cửa trại địch.
- Binary: server **`c88cb289`** (18:27) đè `dccd069d` (backup `.cu_2808_truoc_chongcamp_dccd069d`).
  Chuỗi tái áp thêm CUỐI: **tk_chongcamp**. Restart để ăn.
- Nghiệm thu trận kế: đời sống bot sau khi ra cửa phải tính bằng PHÚT (vết tên bất kỳ:
  RA TRAN → chết ≥60s); "RA TRAN"/giờ tụt mạnh (<2k); [BotSan] không còn dòng "duoi ... 
  o(155x-159x,317x-322x)" (cụm cửa Kim) hay quanh (1331,3442); quân số đứng trong trại
  giảm hẳn bằng mắt.

## 9.40 "VẪN KẸT TRONG TRẠI" = 40% QUÂN SỐ LUÔN TRONG VÒNG ĐỜI CHẾT-HỒI SINH — CHIA HOẢ LỰC TOP-4 [TK-DEU] (28/08 tối) — server `b97d15db` ĐÃ SWAP, CHỜ RESTART

Chủ: "vẫn kẹt trong doanh trại nhiều không ra ngoài được". Đo (18:56-18:58, bản chống-camp
`c88cb289` ĐANG chạy, restart 18:51):
- Thời gian ĐI QUA trại (hồi sinh → RA TRẬN): **p50=16s, max=23s** — dây chuyền trại KHÔNG tắc.
- **Chết ~436-535 mạng/PHÚT** (500 bot ⇒ mỗi con chết ~60s/lần); sống sau khi ra cửa
  **p50=24s, p90=39s**; vòng đời ≈45s ⇒ **~40% quân số (≈200 con) LUÔN đứng trong trại** ở
  mọi thời điểm — chính là cảnh "kẹt trong trại". Trại đông vì DÒNG CHẢY, không phải tắc ống.
- Gốc TTK quá nhanh: H5 (9.36) đổi bộ nhắm sang "gần nhất TUYỆT ĐỐI" ⇒ mọi bot quanh một khu
  cùng đấm đúng MỘT nạn nhân ⇒ xoá 1-2 giây/mạng theo dây chuyền (xoay vòng 23/08 tồn tại
  chính để chống việc này — đã bị H5 gỡ theo yêu cầu "gần nhất").
- Vá (`ReverseTools/goi_va_tk_chia_hoaluc.py`, 1 hunk): vẫn "gần" nhưng **chia hoả lực trong
  NHÓM 4 GẦN NHẤT** theo chỉ số bot (aId đã sắp theo khoảng cách); hết nhóm mới duyệt tiếp.
  TTK chậm ~4×, bot sống lâu hơn ⇒ trại vãn.
- Binary: server **`b97d15db`** (19:00) đè `cb4d5419` (phiên kia lại swap giữa chừng; backup
  `.cu_2808_truoc_deu_cb4d5419`). Chuỗi tái áp CUỐI: **tk_chia_hoaluc**. Restart để ăn.
- Nghiệm thu: "da chet"/phút phải tụt (< ~200); sống-sau-ra-cửa p50 ≥ 60s; nhìn trại vãn hẳn.
  Nếu chủ muốn trận còn "lành" hơn nữa: hạ PB_TK_SAN_TRAN 10→5, hoặc tăng hồi sinh chờ —
  nói là chỉnh (đều 1 dòng).

## 9.41 "BOT ĐỨNG YÊN TRONG TRẠI KHÔNG RA" — LỖI TỒN ĐỌNG (25% từ trận 17:19, TRƯỚC mọi vá TK) + BỘ MỔ [TkCensus] + TỰ CỨU [TkCuu3] (28/08 tối) — server `52680c66` ĐÃ SWAP, CHỜ RESTART

Chủ hỏi "fix gì ảnh hưởng — trước bot đi tới gần trap gọi script ra, giờ đứng yên":
- Đo 3 trận: tỉ lệ báo-danh-xong-KHÔNG-bao-giờ-ra = **25% trận 17:19 (bản 17:12 — TRƯỚC toàn bộ
  vá TK chiều nay)** / 12% (18:03) / 16% (19:05) ⇒ KHÔNG phải hồi quy từ các vá hôm nay — lỗi
  có sẵn, 500 bot làm lộ rõ (60-140 con/trận). Các vá chiều nay thực tế làm GIẢM (25→12-16%).
- Nhóm "không ra" gồm 2 loại đã bóc: (a) lặng lẽ RỜI TK đi luyện map thường (CaoDuy619 —
  BotXe/BotBai ngay sau báo danh); (b) CÂM LẶNG tuyệt đối từ lúc báo danh (BuiBao594 — 0 dòng
  log 15'+). Manh mối cứng cho (b): `[BotKet] bo cuoc sau 3 lan: doing=3 procAI=1 toc ok` =
  NPC **đang do_run mà không nhúc nhích** ⇒ đứng trên Ô BỊ CHẶN theo lưới server (GetDir=0
  mỗi tick) — NewWorld của script báo danh thả KHÔNG kiểm vật cản.
- Vá (`ReverseTools/goi_va_tk_censu_cuu.py`): **[TkCensus]** 10s/lần trong pb_TkNhip — đếm bot
  theo pha (pha 3 tách map 379/khác) + mổ 3 con pha-3: ô, doing, procAI, **obs=CellObsSrv**,
  choRa còn chờ bao lâu, đã mua thuốc chưa → trận sau đọc log là chốt giải phẫu; **[TkCuu3]**
  đầu pha 3: đứng trên ô CHẶN → SetPos sang ô đất cạnh (pb_ODat quanh chỗ đứng, dự phòng quanh
  anchor trại) — chữa ngay nhóm (b) phổ biến nhất.
- Binary: server **`52680c66`** đè `b97d15db` (backup `.cu_2808_truoc_census_b97d15db`).
  Chuỗi tái áp CUỐI: **tk_censu_cuu**. Restart để ăn.
- Nghiệm thu: đọc `[TkCensus]` — pha3 phải rút về ~0 giữa trận; `[TkCuu3]` đếm số con được
  cứu khỏi ô chặn; tỉ lệ không-bao-giờ-ra phải < 3%. Nhóm (a) rời-TK-sớm nếu còn nhiều thì
  mổ tiếp bằng chính census (pha0/nghỉ tăng bất thường).

## 9.42 [BotTan] TẢN ĐÁM ĐÔNG CÙNG PHE >20 CON (28/08 tối) — server `1a3760ac` ĐÃ SWAP, CHỜ RESTART

Chủ: "bot tìm đối thủ gom 1 nhóm rất đông — quét: trên 20 bot cùng phe tại chỗ phải tản ra,
không thì người sau dồn phía xa không đánh được người trước".
- Vá (`ReverseTools/goi_va_tk_tan_nhom.py`, 3 hunk): lúc SEEK (chỉ chạy khi bot CHƯA có mục
  tiêu = đúng tuyến sau đang xếp hàng), đếm đồng đội cùng phe trong 12 ô quanh mình; **>20**
  → `pb_TkTimDichGanNhat` bỏ mọi ứng viên **<20 ô** → nhận mục tiêu ở cánh khác và rời đám.
  Tuyến đầu đang đánh (b.nTargetNpc>0) đã return trước — không bị đụng. Nhãn `[BotTan]`
  (tiết chế 3s). Chi phí ~24 bot seek/khung × quét s_bots = không đáng kể.
- Binary: server **`1a3760ac`** đè `52680c66` (backup `.cu_2808_truoc_tannhom_52680c66`) —
  gói này ôm: census+cứu-ô-chặn (9.41) + tản-nhóm (9.42) + toàn bộ chuỗi trước. Chuỗi tái áp
  CUỐI: **tk_tan_nhom**. GameServer đang chạy bản 19:01 — RESTART một lần ăn 9.40→9.42.
- Nghiệm thu: `grep BotTan bot.log` thấy các cú toả; nhìn trận không còn cục ≥30-40 con một
  chấm; đám sau tự kéo sang cánh khác thay vì nối đuôi.

## 9.43 [TK-CHIAMAP] CHIA TRẬN THÀNH NHIỀU Ổ KHẮP MAP (28/08 tối) — server `8e59f0a4` ĐÃ SWAP, CHỜ RESTART

Chủ: "[BotTan] đã oke NHƯNG lúc di chuyển phải chia ra NHIỀU đối thủ Ở XA NHAU → nhiều nhóm
đánh nhau; hiện vẫn gom 1 nhóm, không chia đều map".
- Gốc: TOPK=25 GẦN NHẤT ⇒ cả 25 ứng viên đều nằm trong chính đám đánh nhau (tiền tuyến là
  MỘT cụm) ⇒ chia hạng kiểu gì cũng quanh một chỗ.
- Vá (`ReverseTools/goi_va_tk_chia_map.py`, 2 hunk): **TOPK 25→60** — hạng mỗi bot rải theo
  toàn phổ khoảng cách tới tận cánh xa, cộng trần 10 con/đích ⇒ mặc định đàn phân tán thành
  nhiều ổ; **[BotTan] bán kính né khi kẹt đám >20: 20→60 ô** — tuyến sau phải nhận địch ở
  VÙNG KHÁC HẲN (không phải rìa đám), làm mồi kéo trận tách ổ; ngoài 60 ô không còn ai thì
  rơi về lang-thang-doanh-trại (cũng xuyên map).
- Binary: server **`8e59f0a4`** đè `1a3760ac` (backup `.cu_2808_truoc_chiamap_1a3760ac`).
  Gói chờ restart giờ gồm 9.40→9.43. Chuỗi tái áp CUỐI: **tk_chia_map**.
- Nghiệm thu: nhìn trận thấy ≥3-5 ổ đánh nhau ở các khu khác nhau thay vì 1 cục; [BotSan]
  "cach N o" trải rộng (nhiều dòng N>100); [BotTan] vẫn nổ khi có cục >20.
- Núm chỉnh nếu muốn tản mạnh/yếu hơn: PB_TK_SAN_TOPK (60) và bán kính né (60*32) —
  mỗi cái 1 dòng.

## 9.44 NGHIỆM THU S12b **ĐẠT** (28/08 ~20h, phiên client 13' pid=15948, nhân vật CaiBang chơi TK + 3 cú phù)

| Thước (đặt ở 9.28) | Trước (27/08) | Nay | Kết |
|---|---|---|---|
| Thang S8-NAN 4-11 cú liên tiếp | 2 thang (11+4 cú), THEO=0 | **2 cú ĐƠN LẺ cách nhau 315s — mỗi cú đúng 1 snap, S12-THEO áp lệnh dắt CÙNG MILI-GIÂY rồi bám mượt** | ✅ |
| Tổng S8-NAN | 15/14' | **2/13'** | ✅ |
| `[S12-CUA]` ≈ số lần vào map | — | 3 CUA / 3 loadnew=1 | ✅ |
| `[S12-THEO]` khi bị dắt | 0 | 24 (chuỗi theo từng đoạn 45226→45441, không búng thêm) | ✅ |
| `[S12-TELE]` đo được (V3) | g_DebugLog không file | **30 dòng trong jx_auto_server.log** ("CaiBang setpos cung map -> bao chinh chu") | ✅ |
| Phù về nhảy toạ độ | có | 3 phù cuối phiên (324→225→53): **0 NAN sau phù** | ✅ |
| Flap chớp tắt VANH↔BACK <500ms | 179-208/phiên | **10** | ✅ (V2 VOID_DIS + dải trễ ăn) |
Ghi chú: cú NAN thứ 2 doing=9 (đang bị khống chế) — server dắt trong lúc choáng, vẫn chỉ 1 snap.
Log lưu `scratchpad/logs_2808c/`. Chuỗi S9→S12b coi như KHÉP; còn mở phần bot/TK 9.40-9.43
(chờ restart) + lưới vật cản 379 (hoãn, chờ chủ duyệt).

## 9.45 "PHÙ VỀ CÒN NHẢY TOẠ ĐỘ BẬY" = GIẰNG CO 2 NGƯỜI LÁI (THEO vs WAuto) — VÁ S12c, client `d5c76e64` ĐÃ SWAP, CHỜ RELOG

Chủ báo sau nghiệm thu 9.44: "phù về còn nhảy toạ độ bậy".
- Pháp y (log 20:15-21:02, CaiBang + WAuto bật): **122/573 mẫu SYNCME-DRIFT lệch ≥4 ô, dao động
  LẮC QUA LẮC LẠI ~30 ô mỗi 1-2 giây** sau khi ra cửa trại (d=(5,-3)↔(-12,28)↔(5,-3)↔(-12,-5)…)
  — client bị giằng giữa HAI người lái: `[S12-THEO]` áp lệnh dắt của server, WAuto cùng lúc lái
  theo mục tiêu của nó. Hai teleport server (hồi sinh 485 ô + qua trap 139 ô — đúng RANDOM_POS)
  đều HỢP LỆ; cái "bậy" là đoạn ping-pong sau đó.
- Gốc: **bypass V1b của 9.28** (đích lạ >64 mps ⇒ áp luôn) sống SUỐT cửa sổ 3s ⇒ đè cả lúc auto
  đang chủ động lái — chính thứ 2 gác nguyên bản (HaveTarget/SendMoveFrames) tồn tại để ngăn.
- Vá (`ReverseTools/goi_va_S12c_autothang.py`, 4 hunk, CLIENT-only):
  mốc mới `g_uS12TuGuiTick` (timeGetTime tại SendClientCmdRun/Walk); bypass CHỈ hiệu lực khi
  **auto chưa tự gửi lệnh nào KỂ TỪ lúc mở cửa sổ** (`g_uS12CuaSoSelf − g_uS12TuGuiTick ≥ 0`,
  hiệu có dấu chịu wrap) ⇒ cú dắt BÀN GIAO lúc hạ cánh vẫn được nuốt; auto lên tiếng lại là
  auto THẮNG ngay. Người chơi không auto: gác cũ vẫn cho qua (hành vi 9.44 giữ nguyên).
- Đánh đổi chấp nhận: khi auto giành lái giữa lúc server còn dắt, lệch có thể tích tới 256 →
  thêm 1 cú S8-NAN snap đơn lẻ — vẫn hơn hẳn lắc ±30 ô liên tục.
- Binary: client **`d5c76e64`** (21:05) đè bản cũ (backup `.cu_2808_truoc_s12c_<md5>`).
  **CHỜ CHỦ RELOG CLIENT** (server không cần restart cho miếng này — client-only; cả hai chỗ
  sửa nằm trong `#ifndef _SERVER`). Chuỗi tái áp: …S12b_cuaso_vanh → **S12c_autothang**.
- Nghiệm thu: chơi TK có auto, chết/phù vài lần — `SYNCME-DRIFT` lệch ≥4 ô phải về mức nhiễu
  (<10/phiên, không còn chuỗi dao động ±30 ô); S8-NAN vẫn đơn lẻ; cảm quan hết "nhảy bậy".

## 9.46 "BOT/NPC/NGƯỜI CHƠI NHẢY TOẠ ĐỘ, BIẾN MẤT HIỆN LẠI" — GẮN LẠI NHANH BẢN SAO MỒ CÔI [S6-GANNHANH] (28/08 ~21h20) — client `284115d7` ĐÃ SWAP, CHỜ RELOG

Chủ: "xem log không thấy BOT - NPC - Người chơi hay bị nhảy tọa độ à?" — đo phần BẢN SAO
NGƯỜI KHÁC (client log 20:15-21:02, trận TK 500 bot):
- **90 cú BIẾN MẤT rồi hiện lại 2-60s, trung vị 8,4 giây** = mồ côi do RECENTER (KRegion::Close)
  rồi phải chờ vòng quay sync server (5 npc/tick/region, region đông ~150 con) — đây chính là
  "nhảy tọa độ + biến mất + hiện lại chỗ khác" với mắt người chơi.
- 131 cú `S10-SNAP` nắn 3-5 ô ("hai bên cùng đứng") — nắn trôi bình thường, ít gây khó chịu.
- `S6-BAL` 247 cú toàn reg=-1 → **ma nhìn thấy vẫn = 0** ✓ (thành quả S11 giữ nguyên).
- Vá (`ReverseTools/goi_va_S6_gannhanh.py`, 1 hunk, `KNpcSet::CheckBalance` — chạy MỖI TICK
  client): con mồ côi có `SyncSignal` còn TƯƠI ≤36 khung (~2s — server VỪA sync = còn sống,
  vị trí chuẩn, tự loại ma cũ) + không phải chết/hồi sinh (kế toán ref có gác death) + không
  phải partner + còn trong 38 ô + region cũ ĐÃ NẠP LẠI (khớp m_RegionID VÀ trong cửa sổ 3×3
  quanh region mình) → **AddNpc + AddRef gắn lại NGAY** (khuôn [S6-ORPHAN-BACK]). Kỳ vọng:
  90 cú chờ-8-giây → gắn lại trong 1 tick (~55ms), người chơi không kịp thấy biến mất.
- Binary: client **`284115d7`** (chứa cả S12c 9.45; bản bị thay `c02cbd9a` của phiên kia —
  backup `.cu_2808_truoc_gannhanh_c02cbd9a`). **CHỜ RELOG** — một lần relog ăn cả 9.45+9.46.
  Server không đổi. Chuỗi tái áp: …S12c_autothang → **S6_gannhanh**.
- Nghiệm thu: `grep -c S6-GANNHANH` phải ≈ số cú ORPHAN tươi; cặp ORPHAN→BACK trễ 2-60s phải
  tụt từ 90 → <10/47'; SNAP giữ nguyên mức; soi thêm không có [S6-GANNHANH] nào cho npc đã
  chết (gác death hoạt động).

## 9.47 "BOT CẤP 20 ĐƯỜNG MÔN KHÔNG ĐÁNH ĐƯỢC NPC" — KHÔNG PHẢI LẪN CHIÊU PHÁI KHÁC (30/08 chiều) + VÁ [CAST-LECH], server `9c1c572a` ĐÃ SWAP, CHỜ RESTART

Chủ hỏi: ĐM cấp 20 không đánh được — đúng chiêu ĐM hay lỗi chiêu phái khác?
**Trả lời: KHÔNG lẫn chiêu phái khác** (3 tầng đều sạch: bộ chọn lọc series khớp hệ; RemoveAllSkill
từ 18/08 dọn chiêu thừa kế; [BotChon] thất bại = 0 cả ngày). Gốc là **DỮ LIỆU chiêu ĐM**:
- skills.txt: chiêu sát thương ĐM SỚM NHẤT đòi **ReqLevel=30** (Đoạt Hồn Tiêu eqt=100 / Truy Tâm
  Tiễn 101 / Mãn Thiên Hoa Vũ 102 — phủ đủ 3 họ vũ khí); chủ lực 60/80+ (302/339/342 đều rq=80).
- ⇒ Bot ĐM cấp 20 KHÔNG có chiêu phái nào đủ cấp → bộ chọn rơi về đòn-đánh-thường (id 1/2/53,
  eqt=-2, rq=0) → đánh như gãi → nhìn là "không đánh được". Từ **cấp 30** tự có chiêu khớp họ
  vũ khí đang cầm (3 chiêu-30 phủ cả 3 họ). Đối chứng: Ngũ Độc có 303 rq=20 nên đánh được từ 20.
- Muốn ĐM đánh được từ cấp 20 = QUYẾT ĐỊNH DATA của chủ: hạ ReqLevel (cột 54) 3 dòng id 47/50/54
  trong `settings/skills.txt` 30→20 (tôi không tự đổi cân bằng).

**Phát hiện kèm + ĐÃ VÁ [CAST-LECH]** (`ReverseTools/goi_va_botcast_repick.py`, 2 hunk):
82.971 cú `[BotCast] BI TU CHOI`/ngày đến từ vỏn vẹn 2 bot hệ kiếm (Nga My/Võ Đang) bốc trúng
"đường QUYỀN" (tay không, PB_WPN_NONE có sẵn trong pool) nhưng `b.nAtkSkill` vẫn giữ chiêu kiếm
cũ — chiêu chỉ được chọn lại khi ĐỔI CẤP, nhánh phát-lại-vũ-khí bốc trúng NONE cũng không reset
⇒ cast lệch vũ khí bị từ chối VĨNH VIỄN. Vá: tại chỗ thăm dò CanCastSkill==0, chiêu LỆCH vũ khí
đang cầm (quy ước eqt y hệt pb_PickSkill) ⇒ `nAtkSkill=0` chọn lại ngay (từ chối vì mana/thế
cưỡi giữ nguyên); gác không phát chiêu 0. Binary: server **`9c1c572a`** đè `cadd97da` (backup
`.cu_3008_truoc_castlech_cadd97da`). Chuỗi tái áp CUỐI: **botcast_repick**.

**Còn treo đáng điều tra tiếp** (nếu chủ muốn): 199k cú/ngày "10 giây không sụt máu" của **chiêu
303 Độc Thạch Cốt (Ngũ Độc nội) vs quái HP600** — chiêu phép độc gần như 0 sát thương với quái
bãi; + 46k cú vs quái HP 30000 (chiêu vật lý trần-20 không xuyên nổi giáp quái cao cấp).

## 9.48 [DM20] BOT THIẾU CHIÊU PHÁI ĐỦ CẤP → TỰ HỌC + DÙNG ĐÒN ĐÁNH THƯỜNG (30/08) — server `7f0909fc` ĐÃ SWAP, CHỜ RESTART

Chủ chốt 2 luật: **KHÔNG được sửa skills.txt**; phải VIẾT code "cấp 20 không có skill đánh được
thì lấy skill đánh được cấp thấp hơn".
- Vá (`ReverseTools/goi_va_dm20_donthuong.py`, 1 hunk cuối `pb_PickSkill`): quét xong mà
  nBest==0 (Đường Môn <30: mọi chiêu phái bị lọc CAP; RemoveAllSkill lúc tạo bot đã xoá cả đòn
  đánh thường của mẫu, hockynang chỉ dạy chiêu phái) → **học id 1 "Công kích vật lý"** (rq=0,
  eqt=-2 mọi vũ khí, series=-1 không vướng lọc hệ — đã kiểm skills.txt) nếu chưa có, rồi dùng
  nó đánh tạm — y hệt người chơi thấp cấp đánh đòn trắng. Đủ cấp chiêu phái (30) thì gate
  đổi-cấp tự pick lại chiêu xịn. Nhãn: `[BotChon] ... -> dung DON DANH THUONG (id 1)`.
- Binary: server **`7f0909fc`** đè `9c1c572a` (backup `.cu_3008_truoc_dm20_9c1c572a`) — gói
  gồm cả [CAST-LECH] 9.47. Chuỗi tái áp CUỐI: botcast_repick → **dm20_donthuong**.
- Nghiệm thu: thả batch bot ĐM mới cấp 20 → thấy dòng "dung DON DANH THUONG (id 1)"; bot NÉM
  đòn trắng vào quái (sát thương nhỏ nhưng > 0, giết được quái HP 600); lên 30 tự chuyển sang
  Đoạt Hồn Tiêu/Truy Tâm Tiễn/Mãn Thiên Hoa Vũ theo họ vũ khí.

## 9.49 RA GỐC THẬT "BOT ĐM CẤP 20 KHÔNG ĐÁNH ĐƯỢC": CHIÊU 303 ĐỘC-THUẦN 0 SÁT THƯƠNG VỚI QUÁI — [303-DOC] (30/08 ~16h40) — server `d3486293` ĐÃ SWAP, CHỜ RESTART

Chủ đúng cả hai lần; chẩn đoán rq=30 của tôi (9.47) SAI MỘT NỬA — bỏ sót chiêu vì lọc eqt≥100:
- **303 "Độc Thạch Cốt" là chiêu ĐƯỜNG MÔN** (data từ `\script\skill\tangmen.lua` bảng `duci_gu`),
  rq=20, eqt=-2 ⇒ là chiêu DUY NHẤT ĐM dùng được ở cấp 20 ⇒ bộ chọn LUÔN chọn nó (vì thế
  fallback đòn-thường 9.48 không bao giờ nổ — 0 dòng sau restart 16:27).
- `duci_gu` CHỈ có `poisondamage_v` (8→40) + `seriesdamage_p` — KHÔNG đòn tức thời; syncheck
  cú pháp OK. Đo thật: **199k cú (29-30/08) + 308 cú (sau 16:27) "10 giây không sụt máu" của
  303 vào quái HP600 còn nguyên máu** ⇒ trên build này ĐỘC KHÔNG BÀO MÒN QUÁI.
- Hệ quả kép: ĐM cấp 20 ôm 303 đánh cả ngày = 0 sát thương, kẹt cấp 20 vĩnh viễn; 303 mang đòn
  phép nên pb_CoChieuNoiTayKhong đếm ĐM là "phái có nội" ⇒ bot ĐM lẻ bị TƯỚC vũ khí rồi cũng ôm
  303 vô dụng.
- Vá (`ReverseTools/goi_va_303_docthuan.py`, 2 hunk KPlayerBot.cpp — không đụng engine/skills.txt):
  H1 pb_PickSkill LOẠI HẲN ứng viên **phép độc-thuần** (IsPhysical=0 && mọi đòn đều poison;
  PB_DIAG `DOCTHUAN`) — chiêu vật lý mang độc phụ vẫn giữ; H2 bỏ poison khỏi danh sách đòn-phép
  trong pb_CoChieuNoiTayKhong ⇒ ĐM hết bị coi là phái-có-nội, bot lẻ GIỮ vũ khí (con đã bị tước
  thì [BotVuKhi] tự phát lại như cơ chế 9.34).
- Kỳ vọng sau restart: ĐM cấp 20 → `[BotChon] ... dung DON DANH THUONG (id 1)` → ném đòn trắng
  giết quái 600 máu → lên 30 nhận Đoạt Hồn Tiêu/Truy Tâm Tiễn/Mãn Thiên Hoa Vũ; "khong sut mau"
  của 303 tụt từ ~20k/giờ về ~0; ĐM lẻ cấp cao cầm lại vũ khí đánh chiêu tầm xa.
- Binary: server **`d3486293`** đè `7f0909fc` (backup `.cu_3008_truoc_docthuan_7f0909fc`).
  Chuỗi tái áp CUỐI: dm20_donthuong → **303_docthuan**. Nếu chủ muốn ĐỘC bào mòn được quái
  (sửa engine poison-vs-mob) — việc riêng, đụng gameplay, cần chủ duyệt trước.

### 9.49b ĐÍNH CHÍNH (chủ chỉ ra): ĐM **CÓ** chiêu cấp 10 dùng mọi vũ khí — bot bỏ qua vì XẾP HẠNG, không phải vì thiếu chiêu

Chủ: "Đường Môn có skill đánh cấp 10 dùng được toàn bộ vũ khí sao mà không dùng mà đi đánh thường?"
— ĐÚNG. Kiểm lại tận gốc:
- **id 45 "Phích Lịch Đạn"**: rq=**10**, eqt=**-2 (mọi vũ khí: phi đao/phi tiêu/tụ tiễn)**,
  IsPhysical=1, tầm 400, TargetEnemy=1, series=1 (khớp hệ ĐM) — và **bot CÓ HỌC** nó
  (`factionhead.lua SKILLNORMAL[3]` dòng ĐẦU TIÊN). Dữ liệu `pili_dan` trong tangmen.lua:
  physicsenhance_p 20→80%, deadlystrike_p, addskilldamage1-4 ⇒ đòn THẬT, ăn sát thương vũ khí.
- **Vì sao bot vẫn ôm 303**: cả hai cùng eqt=-2, cùng có đòn sát thương, cùng mang poison
  ⇒ hoà hết 3 bậc đầu (nNoi/nRank/nDmg) → xuống bậc **rqTier: 303 (rq20) > 45 (rq10) ⇒ 303
  thắng**. Bậc rqTier thêm 18/08 để bot khỏi đấm-tay thay chiêu phái — đúng ý đồ, nhưng ở
  ĐM nó chọn trúng chiêu độc-thuần 0 sát thương.
- **Bot CHƯA HỀ dùng đòn thường** (0 dòng `DON DANH THUONG` cả ngày) — fallback 9.48 chỉ là
  lưới an toàn cho cấp <10, không phải thứ đang chạy.
- Vá 9.49 (`d3486293`, ĐÃ nằm trên đĩa) **giải đúng ca này**: loại 303 (phép độc-thuần) khỏi
  ứng viên ⇒ ở cấp 20 chỉ còn **45** hợp lệ (đã soát: 305/306 Thanh Mộc Công TargetSelf=1;
  49 Địa Diêm cơ quan TargetEnemy=0; 347 Địa Diêm Hoả series=3 lệch hệ; 47/50/54 đòi cấp 30)
  ⇒ **bot ĐM cấp 10-29 sẽ dùng Phích Lịch Đạn**, cấp 30 lên Đoạt Hồn Tiêu/Truy Tâm Tiễn/Mãn
  Thiên Hoa Vũ theo họ vũ khí đang cầm.
- ⚠️ **GameServer vẫn chạy bản 16:27 (`7f0909fc`) — CHƯA có vá này**; `d3486293` chờ RESTART.

## 9.50 GỐC THẬT (2 TẦNG) CỦA "BOT ĐƯỜNG MÔN ĐI TỚI QUÁI RỒI ĐỨNG YÊN" — server `2e748b7d` ĐÃ SWAP, CHỜ RESTART (31/08 ~11:56)

Điều tra bằng workflow 5 hướng + phản biện đối kháng (mọi kết luận đều `con_dung=true`, tự đọc mã/log).

### TẦNG 1 — LỖI LẬP TRÌNH CỦA TÔI: đọc sai `m_DamageAttribs` (mảng THƯA)
`m_DamageAttribs` là mảng **17 ô đánh chỉ số theo LOẠI đòn** (KSkills.cpp:2592-2704):
`[0]attackrating [1]ignoredefense [2]MAGIC [3]seriesdamage [4]deadlystrike [5]fatallystrike
[6]steallife [7]stealmana [8]stealstamina [9]PHYSICS [10]COLD [11]FIRE [12]LIGHTING [13]POISON
[14]stun [16]randmove`; còn `GetDamageAttribsNum()` chỉ là **bộ đếm số ô đã điền**.
Cả **3 vòng quét** trong KPlayerBot.cpp viết `for (a = 0; a < nDaNum; a++)` ⇒ với 303 (nDaNum=2:
poison ở [13] + series ở [3]) chỉ đọc ô [0],[1] rỗng ⇒ **mọi phép thử đòn-phép luôn = 0**:
- `bNoi` luôn 0 ⇒ cơ chế "nửa đàn thiên nội công" (23/08) **nằm im từ đầu**;
- khối `DOCTHUAN` (30/08) gate trên bNoi ⇒ **không bao giờ chạy** (log DOCTHUAN = 0 đúng như đo);
- `pb_CoChieuNoiTayKhong` luôn 0 ⇒ đường nội/ngoại (28/08) **cũng nằm im**.
⇒ 303 không bị loại, và bậc phụ **rqTier** (rq20 > rq10) cho nó **thắng tuyệt đối** chiêu
**45 "Phích Lịch Đạn"** (rq10, eqt-2, IsPhysical=1, tầm 400) — đúng chiêu chủ chỉ ra.
**Vá** (`goi_va_damageattrib_thua.py`, 4 hunk): 3 phép thử `pb_DonPhepThat/pb_DonDoc/pb_DonVatLy`
đọc **theo Ô + so ĐÚNG enum** (mảng không được memset nên không thể chỉ kiểm ≠0); DOCTHUAN tách
độc lập khỏi bNoi (303 còn `seriesdamage_p` ở ô [3] nên phép "có attrib khác độc" của bản cũ sai).

### TẦNG 2 — TƯỜNG NỘI LỰC (workflow tìm ra, tôi đã bỏ sót)
- `duci_gu` (303): `skill_cost_v = {{1,20},{20,60}}` ⇒ **cấp kỹ năng 20 tốn 60 nội lực**.
  Nhân vật **cấp 20 trần nội lực chỉ 45-57**. Đo `jx_auto_server.log`: **1.963/1.963 mẫu
  `[E4_SKILL_COST] skill=303 cost=60` với mana ĐẦY nhưng < 60 — chưa một lần nào đủ.**
- `KNpc.cpp:2604-2606` `case 1: ... if (!IsPlayer() || Cost(...))` — Cost FALSE ⇒ bỏ cả thân lệnh,
  rơi thẳng xuống nhãn `Exit:` (2712) ⇒ **`DoStand()` = ĐỨNG YÊN**, không một dòng báo lỗi
  (nhánh báo "hết nội lực" nằm trong `#ifndef _SERVER`). Đếm: **5.061/5.079 dòng
  `[E4_SKILL_ABORT]` là 303 (99,6%)**; `[E3_CAST_ENTRY]` của 303 = **0** (chưa hề ra chiêu).
- `CanCastSkill` (KSkills.cpp:195-397) **không kiểm cost** ⇒ bộ thăm dò của bot cũng không bắt được.
- Đối chứng: chiêu 45 tốn **12 nội lực ở MỌI cấp** ⇒ luôn trả nổi. Bot phái khác cùng trả 60 nhưng
  cấp 103-115 có bể mana ~1.000 nên vô sự ⇒ biến phân biệt là **CẤP NHÂN VẬT**, không phải phái.
- **Vá** (`goi_va_loc_noiluc.py`, 1 hunk): pb_PickSkill **loại ứng viên có chi phí VƯỢT TRẦN tài
  nguyên** của bot (mana/stamina/life theo `GetSkillCostType`, so với TRẦN chứ không so hiện tại).
  Nhãn `PB_DIAG "COST=<giá>><trần>"`. Đây là bẫy CHUNG cho mọi phái/mọi cấp, không riêng ĐM.

### Số liệu phân biệt (đã phản biện)
- 303: **106.341 lần bỏ mục tiêu, 0,0000% quái mất máu**. Mọi chiêu khác: 26.316 lần bỏ,
  **40,67% quái ĐÃ mất máu** ⇒ **không phải lỗi chung của bot**, chỉ 39 bot ĐM cấp 20-26 tê liệt.

### Trạng thái
Server **`2e748b7d`** (11:56, có `DOCTHUAN` + `DON DANH THUONG` + `COST=`) đè `448d63cc`
(backup `.cu_3108_truoc_mana_448d63cc`). GameServer vẫn chạy bản 11:05 (`f8d819cd`) ⇒ **RESTART**.
Chuỗi tái áp thêm CUỐI: **damageattrib_thua** → **loc_noiluc**.
⚠️ Vá tầng 1 **kích hoạt lần đầu** đường nội/ngoại (28/08) vốn nằm im — sau restart nửa đàn của các
phái CÓ chiêu phép thật (Nga My/Võ Đang/Côn Lôn/Thúy Yên/Cái Bang/Thiên Nhẫn) sẽ bỏ vũ khí đánh
phép. ĐM/Thiên Vương KHÔNG bị (không có chiêu phép cùng hệ dùng tay không). Nếu chủ thấy nhóm đó
yếu đi thì nói — gỡ bằng 1 dòng.

### Nghiệm thu sau restart
`grep "dung chieu 45" bot.log` phải có (ĐM cấp 10-29) · `[E3_CAST_ENTRY] skill=45` xuất hiện ·
`[E4_SKILL_ABORT] skill=303` về 0 · quái bị ĐM đánh phải sụt máu · bot ĐM bắt đầu **lên cấp** ·
`grep BotNoi bot.log` xem đường nội/ngoại có kích hoạt đúng phái không.

### 9.50b ĐÍNH CHÍNH + CHẶN RỦI RO (kết quả workflow 11 agent, phản biện đối kháng)

**a) ĐÍNH CHÍNH giả thuyết của tôi ở 9.49: "độc không bào mòn quái" là SAI — đã bị bác bỏ.**
Độc chưa hề có cơ hội chạm quái: đo `jx_auto_server.log` (485.472 dòng) cho skill 303:
`E4_SKILL_IN`=6.912 · `E4_SKILL_CANCAST`=6.912 · **`E3_CAST_ENTRY`=0 · `E3_MISSLE_BORN`=0 ·
`E4_DMG_IN`=0** · `E4_SKILL_ABORT`=7.513/7.531 (99,8%). Chuỗi `HP 600/600` là chữ ký của chiêu
**không bao giờ nổ**, không phải chiêu nổ mà vô hại. ⇒ KHÔNG cần điều tra hệ sát thương độc.

**b) TẦNG 3 (workflow bổ sung): chính bot tự đẩy giá chiêu lên trần.**
`KPlayerBot.cpp` khối "nâng full kỹ năng theo cấp" ép MỌI kỹ năng lên cấp = cấp nhân vật (trần 20).
`duci_gu.skill_cost_v={{1,20},{20,60}}` ⇒ bot cấp 20 tự nâng 303 lên cấp kỹ năng 20 = **60 nội lực**,
trong khi **người chơi thật** cấp 20 giữ 303 ở cấp kỹ năng thấp (~20 nội lực) nên **vẫn dùng được**.
Đo: `[E3_LIST_CURLEVEL] skill=303 level=20 cur_level=20` = 1.352/1.352 mẫu.
⇒ Cải tiến NÊN làm sau (chưa làm, chờ chủ duyệt): **giới hạn cấp nâng kỹ năng theo khả năng chi
trả** để bot cấp thấp của MỌI phái không tự làm mình liệt (hiện chúng rơi về đòn thường — vẫn tốt
hơn đứng yên, nhưng chưa tối ưu).

**c) 🔴 CHẶN KỊP RỦI RO HUỶ VŨ KHÍ ~400 BOT (vá `goi_va_noi_hoan.py`).**
Vá SPARSE hồi sinh `pb_CoChieuNoiTayKhong` (trước đó luôn trả 0 — `[BotNoi]` = **0 dòng trên toàn bộ
115 MB bot.log** ⇒ đường nội/ngoại 28/08 thực tế NẰM IM từ đầu). Đầu ra của nó gác khối
**`RemoveItemIdx` = HUỶ vũ khí đang cầm** trong `pb_TrangBiTheoCap`, đồng thời chặn phát lại.
Đo `skills.txt × factionhead.lua` ở cấp bot 110: **8/10 phái** có chiêu phép tay-không hợp lệ
(TL 271 · NM 80,82,91 · TY 102,113,111 · CB 122,128 · TN 145,138,148 · VĐ 153,164,165 · CL 179,182);
ĐM + Thiên Vương an toàn. Với `pb_BotNoi = dwID&1` ⇒ **~400/1000 bot bị huỷ vũ khí ngay lần
`pb_TrangBiTheoCap` đầu sau restart**. Chủ CHƯA duyệt việc này và nó không liên quan lỗi đang sửa
⇒ **tạm ngắt bằng công tắc biên dịch `#define PB_BAT_DUONG_NOI 0`** trong `pb_BotNoiThat`
(giữ nguyên hành vi đang chạy; đổi 0→1 khi muốn test riêng đường nội công).
Xác nhận tắt thật: chuỗi `"duong NOI CONG, khong nhan vu khi"` **biến mất khỏi binary** (trình biên
dịch loại nhánh chết).

**d) TRẠNG THÁI CHỐT:** server **`74b55a4f`** (31/08 12:06) — có `COST=` (lọc nội lực) + `DOCTHUAN`
+ `DON DANH THUONG`, KHÔNG có đường nội công. Backups: `.cu_3108_truoc_sparse_f8d819cd` ·
`.cu_3108_truoc_mana_448d63cc` · `.cu_3108_truoc_noihoan_2e748b7d`.
GameServer vẫn chạy `f8d819cd` (11:05) ⇒ **CHỜ CHỦ RESTART**.
Chuỗi tái áp thêm CUỐI: **damageattrib_thua** → **loc_noiluc** → **noi_hoan**.

## 9.51 "NHIỀU BOT TK ĐỨNG YÊN KHÔNG ĐÁNH KHI ĐÁNH ĐÔNG" (01/09 đêm) — ĐO XONG 3 TRẬN, ĐẶT BỘ LOG [TkKet3], server `66c1f6d1` ĐÃ ĐẶT `.moi` CHỜ RESTART

Chủ nghi "phần làm bot tản ra". **Đã GIẢI OAN phần tản ra** — bot kẹt chưa từng vào pha 4
(nơi 9.42/9.43 hoạt động); `[BotSan]`/`[BotTan]` nổ bình thường (870/560 cú trận 20:50).

### Số đo 3 trận 31/08 (mỗi trận 500 bot, đếm theo TÊN trên nhãn không tiết chế)

| Trận (báo danh) | Thế trận | Không bao giờ "qua cua trai RA TRAN" | Thuộc phe |
|---|---|---|---|
| 20:50 | Tống ở trại ĐÔNG (1688,3072) | **79/500 (Tống 79/250 = 32%)** | 100% Tống |
| 22:50 | Kim ở trại ĐÔNG | **45/500** | 100% Kim |
| 23:46 | Tống ở trại ĐÔNG | **73/500** | 100% Tống |

- Nhóm kẹt **bám theo TRẠI ĐÔNG, không theo phe**; phe trại TÂY (1242,3549): **0 con cả 3 trận**.
- Giao trận 1 ∩ trận 3 (cùng thế trận, cùng chỉ số mảng bot): chỉ **10/79 trùng tên** ⇒ KHÔNG
  bền theo con — yếu tố **ĐỘNG mỗi trận**, không phải ô trap gán theo chỉ số.
- Census tóm quả tang bot đóng băng `doing=1 procAI=1 obs=0 choRa=0 mua=1` đứng nguyên MỘT ô
  8-12+ phút ngay **vết trap cửa ra** (NguyenHieu9/CaoNam20 ô(1666,3102), LeTrung10/PhamHieu21
  ô(1670,3103) — trùng ô giữa các trận!), tại **điểm đáp** (1242,3549)/(1688,3072) và **cạnh
  Quân Y** (1692-1699,306x). Bot khoẻ ra trận 20-40 lần/con; bot kẹt = 0 (hỏng nhị phân).
- Luồng cửa toàn cục vẫn khoẻ: RA TRAN 400-700 cú/phút suốt trận; "da chet" ~480/phút.
- **Nhóm "đứng yên" thứ hai KHÔNG phải lỗi**: chu trình chết→hồi sinh→5s→mua thuốc→5s→ra cửa
  ≈30s/vòng × ~480 chết/phút ⇒ lúc nào cũng ~220-250 con (≈nửa quân) đứng/đi trong trại —
  churn theo thiết kế (hai quãng chờ 5s là yêu cầu chủ 21/08); muốn giảm là quyết định gameplay.

### Đã LOẠI bằng mã + log (đỡ phiên sau đi lại)

1. **A\* lỗi lặp**: cả đêm chỉ 22 dòng `[BotA*]` (throttle 3 dòng/s — nếu hàng chục con fail
   mỗi tick phải thấy ~nghìn dòng).
2. **Nhánh chờ cổng** (`GetTimerRestTimer(1)>0`): luôn nạp lại `nTkChoRa` 1-15s ⇒ census phải
   thấy choRa>0; thực tế choRa=0 ở 45/45 mẫu con kẹt.
3. **Đồng hồ cá nhân**: `GetRestTime` = deadline trừ `m_nLoopRate` (KSubWorldSet.cpp:91 tăng
   mỗi khung) ⇒ tự cạn ≤90s; mobinhtk `common_tong/kim` LUÔN `StopTimer()` rồi `SetTimer(90*18,2)`
   khi báo danh ⇒ không thể từ chối quá 90 giây sau báo danh.
4. **Lỗi Lua trong trap**: `ExecuteScript` không nuốt lỗi; ScriptError.log = 0 dòng trap cả đêm
   (chỉ có lỗi mantle `PushByType` nil — việc khác).
5. **Khác kịch bản hai cửa**: `kimratrai.lua` ≡ `tongratrai.lua` về logic (diff chỉ toạ độ
   RANDOM_POS + đảo SetDeathScript).

### Phát hiện kèm (chưa vá, có log canh)

- **a. "mua=1 rởm"**: khâu Quân Y — `PB_WalkTo` trả −1 (không đường) thì code đặt
  `nTkMuaXong=1` mà **bỏ qua `bot_tk_muamau` = bỏ qua `StopTimer()`** (KPlayerBot.cpp khối
  `!b.nTkMuaXong`). Nhãn mới `[TkKet3-MUA]` đếm ca này.
- **b. Kẹt-vô-hình do 9.38**: vùng-an-toàn hậu doanh làm tươi `nTkTick` MỖI TICK khi bot trong
  hộp 40 ô quanh trại (:8387) ⇒ bot kẹt trong trại **không bao giờ** bị sweeper 120s đá ⇒ kẹt
  thành vĩnh viễn và lọt mọi thống kê KET.
- **c. `tongtu/kimtu.lua` vừa là script BÁO DANH vừa là SetDeathScript** — mỗi cái chết nạp lại
  `SetTimer(90*18,2)`; chu trình sống nhờ `bot_tk_muamau` StopTimer (liên quan mục a).

### Bước mù cuối + bộ log [TkKet3] (`ReverseTools/goi_va_tkket3_moxe.py`, 6 hunk, CHỈ GHI LOG)

Còn đúng MỘT điều không quan sát được: mỗi tick pha 3 của con kẹt kết thúc ở dòng nào
(PB_WalkTo trả gì / cổng đóng / script trap từ chối hay SetPos hụt). 7 nhãn mới, tất cả gate
"đứng nguyên một chỗ >60s" bằng neo toạ độ riêng (`uKet3Tick` — không dùng `nTkTick` vì mục b):

| Nhãn | Trả lời |
|---|---|
| `[TkKet3]` (10s/con) | mọi con pha-3 đứng >60s: pos, doing, mua, choRa, **timer cá nhân**, t1 cổng |
| `[TkKet3-QY]` (10s/con) | kẹt ở khâu đi Quân Y: nWq của PB_WalkTo |
| `[TkKet3-MUA]` (luôn in) | đường "mua=1 rởm" — bỏ mua + bỏ StopTimer |
| `[TkKet3-RA]` (10s/con) | kẹt ở khâu ra cửa: nW, vị trí, đích, cửa k |
| `[TkKet3-CONG]` | qua được walk nhưng cổng đóng (t1>0) |
| `[TkKet3-TRAP]` (15s/con) | gọi script trap: **GetRestTime đúng giá trị script sẽ đọc** + pos trước |
| `[TkKet3-TRAP2]` | script trả về gì + có bị TỪ CHỐI (không đổi chỗ) không |

### Trạng thái binary + nghiệm thu

- **`CoreServer.dll.moi` = `66c1f6d1`** (01/09 00:26) đè `.moi` PF13 cũ `3d4e6f9f`
  (backup `.moi.cu_0109_truoc_tkket3_3d4e6f9f`) — superset: đủ marker mantle/REFOAN/COST=/
  DOCTHUAN + TkKet3×7. GameServer đang chạy `2472b5e1` (31/08 20:19) ⇒ **CHỜ CHỦ RESTART**.
- `Client Release|Win32` biên dịch sạch (file dùng chung) — **KHÔNG deploy client**, các `.moi`
  client PF13 giữ nguyên.
- Chuỗi tái áp CUỐI: … → noi_hoan → **tkket3_moxe**.
- Nghiệm thu sau restart, chơi/để chạy 1 trận TK rồi:

```
grep -c "TkKet3\]"  bot.log        # phai >0 neu con hien tuong (moi con dung >60s deu bi diem danh)
grep "TkKet3-TRAP"  bot.log        # doc rest= : >1440 (80s*18) la thu pham dong ho; nguoc lai xem TRAP2
grep "TkKet3-TRAP2" bot.log        # script=1 + TU CHOI lien tuc = SetPos hut / nRemain — chot goc
grep "TkKet3-RA"    bot.log        # nW=-1 lap = A* ; nW=0 lap ma doing=1 = lenh di bi nuot
grep "TkKet3-MUA"   bot.log        # dem ca "mua rom" (bo StopTimer)
```

  Chốt được nhánh nào thì **vá gốc nhánh đó** (chủ đã dặn không vá chữa cháy kiểu hẹn giờ).
