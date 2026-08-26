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

## 9.7 Lỗi phụ nhặt được dọc đường (ngoài phạm vi di chuyển)

- Server `[S2-SKILL-NOTLEARNED] npc=91423 id=92422 skill_req=361` lặp ~1,3 s/lần suốt phiên —
  vẫn đánh được qua nhánh `S1-MELEE-NOROLL`, nhưng "chưa học mà vẫn xin đánh" cần soi riêng.
- Hai phiên Claude vẫn đang chạy song song trên máy (MEMORY.md bị phiên kia ghi chen giữa phiên này).
