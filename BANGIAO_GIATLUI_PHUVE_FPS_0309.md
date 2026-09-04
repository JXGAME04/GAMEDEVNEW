# GIỰT TỚI / GIỰT LÙI · PHÙ VỀ NHẢY TOẠ ĐỘ · NỘI SUY FPS SO VỚI CLIENT VLTK

**03/09/2026 — CHỈ PHÂN TÍCH. KHÔNG SỬA MỘT DÒNG NÀO. KHÔNG BUILD. KHÔNG SWAP.**

Tiếp nối `BANGIAO_DICHUYEN_GIATLUI_2508.md` (25-26/08). Bộ log `[S6-*]` / `[S8-NAN]` / `[S12-*]`
đặt hồi đó **vẫn còn nguyên trong bản đang chạy**, nên lần này đo được thật chứ không phải suy đoán.

---

## 1. Mẫu đo — bản nào, tệp nào

| | |
|---|---|
| `bin\client\CoreClient.dll` | md5 **`7efb5720b0843b6e650b66dd4c912c12`** (03/09 10:25) |
| `bin\client\Game.exe` | md5 `0411771f0e13d6564df9f1c6a2bdc960` |
| `bin\client\config.ini` | `PaintFps=60  PaintInterp=1  PaintLog=1  AutoLog=1  Represent=2` |
| Nhân vật | **CaiBang** — client npc idx **1**, dwID **92480**, server npc idx **91477**, cấp 154 |
| Log client | `jx_auto.log` (19 MB, đang ghi) + `jx_auto.log.1` (67 MB) + `jx_paint.log` |
| Log máy chủ | `bin\server\jx_auto_server.log` (37 MB, đang ghi) |

Đã xác nhận **đủ 15 nhãn** `S6-ADD/DEL/CMD/SYNC/ME/ATK/ORPHAN/VANH/CAM/BAL/BANG/LOADMAP`, `S8-NAN`,
`SYNCME-DRIFT`, `SYNCMIN-DRIFT` **có mặt trong DLL đang chạy** (đếm chuỗi trực tiếp trong nhị phân).

**Chất lượng mẫu:** `grep -c "bo qua"` = **4** (jx_auto.log) và **134** (jx_auto.log.1) trên tổng
784.142 dòng ⇒ mất mẫu không đáng kể, số liệu dùng được.

**Mẫu hợp lệ:** 29.468 gói tự-đồng-bộ của chính nhân vật (`[S6-ME]`, đã loại các gói khác region),
trải **27,5 phút** chơi thật (auto Dã Tẩu bật).

> ⚠️ Đồng hồ `t=` của **cả client lẫn server đều là `timeGetTime()` trên CÙNG một máy** ⇒ ghép mốc
> giờ hai bên được. Đây là chìa khoá để bắt được ca ở mục 3.

---

## 2. TRIỆU CHỨNG 1 — "đang di chuyển hay bị giựt tới hoặc giựt lùi"

### 2.1 Kết luận một dòng

> Client **luôn chạy trước** máy chủ và độ lệch **chỉ tăng, không bao giờ tự xả**.
> Đường xả duy nhất là cú **ép vị trí ở ngưỡng 256 mps (8 ô)** — mà ngưỡng đó nay nằm **ngay trong
> đám mây nhiễu bình thường**, nên nó nổ **3,5 lần mỗi phút**, mỗi lần dịch nhân vật **8-13 ô**.

### 2.2 Số đo — độ lệch client ↔ server của CHÍNH nhân vật

29.468 mẫu, tính từ `[S6-ME]` (`cell*32768 + off`, chỉ lấy mẫu cùng region):

| | jx_auto.log.1 (25,4 ph) | jx_auto.log (10,8 ph) |
|---|---|---|
| p50 | 30 mps (0,9 ô) | 41 mps (1,3 ô) |
| p75 | 91 mps (2,9 ô) | 103 mps (3,2 ô) |
| **p90** | **165 mps (5,2 ô)** | **165 mps (5,2 ô)** |
| p95 | 211 mps (6,6 ô) | 197 mps (6,2 ô) |
| **p99** | **248 mps (7,8 ô)** | **233 mps (7,3 ô)** |
| max | 357 mps (11,2 ô) | 357 mps (11,2 ô) |
| ≥ 256 mps | 0,25 % | 0,27 % |

Theo trạng thái (`doing`), gộp cả hai tệp:

| doing | mẫu | p50 | p90 | p99 | max |
|---|---|---|---|---|---|
| 18 = **runattack** | 2.160 | 73 | **189-195** | 261 | **357** |
| 3 = run | 9.337 | 78 | 172-180 | 242 | 349 |
| 7 = attack | 7.111 | 30 | 161-182 | 236 | 282 |
| 1 = stand | 10.132 | 3-9 | 68-124 | 248 | 273 |
| 6 = magic | 713 | 21-24 | 67-91 | 147 | 194 |

*(bảng `NPCCMD` bắt đầu ở `do_none = 0`, `KNpc.h:49` ⇒ 1=stand, 3=run, 7=attack, 10=death,
17=blurmove, **18=runattack**, 21=revive)*

### 2.3 Cú nắn `[S8-NAN]` — chính là cái "giựt"

`KProtocolProcess.cpp:2521-2557`, nhánh thứ ba của `SyncNpcMinPlayer()`:

```c
const int nLech = g_GetDistance(nMeX, nMeY, pSync->m_dwMapX, pSync->m_dwMapY);
if (nLech >= 256)                       // 8 ô
{
    g_uS12CuaSoSelf = timeGetTime();    // mở cửa sổ 3000 ms nghe lệnh cho CHÍNH MÌNH
    ... DecRef / NpcChangeRegion / gán m_MapX,m_MapY,m_OffX,m_OffY = vị trí máy chủ ...
    memset(&m_sSyncPos, 0, ...);        // bỏ nội suy đang dở
    SubWorld[0].StopPath();             // HUỶ ĐƯỜNG ĐI ĐANG CHẠY
    return;
}
```

Đo được **96 cú / 1651 s = 3,49 cú/phút** (tách theo tệp: 87 cú / 25,4 ph = 3,43 và 42+ cú /
10,8 ph = 3,87):

- độ lệch lúc nắn: min **256**, p50 **274-278**, max **423 mps = 13,2 ô**;
- theo trạng thái: **run 62, runattack 43, stand 14, attack 10**;
- **24/86 khoảng cách giữa hai cú liên tiếp < 3 giây** ⇒ nổ thành **chùm**;
- 53/129 cú có kèm đổi region.

### 2.4 Hướng lệch — vì sao "giựt LÙI" nhiều hơn "giựt TỚI"

Chiếu véc-tơ lệch lên hướng nhân vật đang chạy, 2.684 mẫu (lệch ≥ 2 ô, đang di chuyển):

| | số mẫu | tỉ lệ | người chơi thấy |
|---|---|---|---|
| client **ở phía trước** máy chủ | 2.401 | **89,5 %** | nắn ⇒ **GIỰT LÙI** |
| client ở phía sau máy chủ | 174 | 6,5 % | nắn ⇒ **GIỰT TỚI** |
| lệch ngang | 109 | 4,1 % | giựt chéo |

Khớp đúng mô tả của chủ: chủ yếu là **giựt lùi**, thỉnh thoảng mới giựt tới.

### 2.5 Cơ chế thật — "bánh cóc" chỉ tiến không lùi

Dựng lại đường cong lệch trong 2,5 s trước mỗi cú nắn cho thấy độ lệch **nhảy từng bậc rồi nằm
phẳng**, chưa bao giờ giảm. Ví dụ cú nắn #9 (t=739613932):

```
t-1568  20,1 mps  doing=run
t-1337  83,6 mps  doing=attack      <- bậc +63 khi ĐỔI trạng thái
t-1008 207,6 mps  doing=attack      <- bậc +124
t- 666 207,6 mps  doing=attack      <- NẰM PHẲNG (hai bên chạy cùng tốc độ)
t- 346 207,6 mps  doing=stand
t-   0 292,4 mps  doing=run         <- bậc +85  => VƯỢT 256 => NẮN 9,1 ô
```

Đoạn nằm phẳng chứng minh **client và máy chủ chạy CÙNG tốc độ** (đã kiểm bằng tay trên một đoạn
sạch: cả hai đi 42 mps mỗi 113 ms, lệch giữ nguyên 57 mps). Nghĩa là:

- lệch **không** do sai tốc độ;
- lệch **tăng theo BẬC** mỗi lần đổi lệnh (đứng→chạy, chạy→đánh, đánh→chạy) — đúng bằng một nhịp
  trễ mạng + nhịp 18 fps của máy chủ, vì client tự chạy ngay còn máy chủ chỉ chạy sau khi nhận gói;
- **không có bất kỳ cơ chế nào kéo lệch về 0** khi lệch < 256. `NormalSync` (`:2322`) cố tình bỏ
  qua chính mình; `KNpc::ServeMove` chỉ đặt `m_nNeedFixPos` khi bị chặn (`KNpc.cpp:4600`).

⇒ Lệch cứ **cộng dồn** cho tới khi chạm 256 rồi bị xả một phát.

### 2.6 Vòng tự nuôi — vì sao nắn hay nổ thành chùm

Bằng chứng trực tiếp, cú #1 → cú #2 cách nhau **776 ms**, cả hai lúc `doing=stand`
(t=739481487 và t=739482263, cùng ở Tương Dương):

```
cú #1: nắn 274 mps  ->  StopPath()  ->  CLIENT ĐỨNG YÊN
t-666   lệch  21 mps   doing=stand      <- client đứng, server VẪN ĐI TIẾP
t-344   lệch 147 mps   doing=stand
t-121   lệch 231 mps   doing=stand
cú #2: nắn 272 mps
```

`StopPath()` chỉ xoá đường đi **phía client**; máy chủ không hề biết và vẫn đi nốt lệnh cũ ⇒ lệch
mọc lại với tốc độ chạy đầy đủ ⇒ chưa đầy 1 giây đã đủ 8 ô ⇒ nắn tiếp. Đây chính là "chùm 4 cú
liên tiếp" đã ghi trong chú thích `KProtocolProcess.cpp:45`.

### 2.7 Ngưỡng 256 đã mất hết biên an toàn

Chú thích tại `KProtocolProcess.cpp:2519` ghi rõ lý do chọn 256:

> *"Đo thật 25/08 (SYNCME-DRIFT): sai số dự đoán bình thường **p90 = 83 mps**, tức ngưỡng này cao
> **gấp 3 lần** đỉnh nhiễu bình thường -> không đụng tới đường chạy mượt."*

Đo lại 03/09: **p90 = 165 mps**, **p99 = 233-248 mps**.

| | 25/08 | 03/09 |
|---|---|---|
| p90 nhiễu | 83 mps | **165 mps** (gấp đôi) |
| ngưỡng / p90 | 3,08 × | **1,55 ×** |
| p99 so với ngưỡng | 181 (71 %) | **248 (97 %)** |

Ngưỡng bây giờ **nằm trong đuôi phân bố bình thường**, không còn là "chỉ có thể là dịch chuyển"
như thiết kế ban đầu. Vì thế nó nổ 3,5 lần/phút ngay cả khi **không có teleport nào**.

> Chưa xác định được vì sao p90 tăng gấp đôi so với 25/08 — hai phiên đo ở hai kiểu hoạt động khác
> nhau (25/08 nhiều lúc đứng, 03/09 auto Dã Tẩu chạy liên tục). **Cần một phép đo có kiểm soát**
> mới kết luận được, xem mục 6.

### 2.8 Tầng VẼ **không** phải thủ phạm trong phiên này

Cắt `jx_paint.log` đúng cửa sổ phiên chơi (t = 739.470.000 → 740.070.000, 60 khối 10 s):

| | |
|---|---|
| lượt bơm | **125,0 lượt/giây** (ổn định, min 123,1 max 125,1) |
| spike ≥ 25 ms | **14 cú / 10 phút = 1,4 cú/phút** |
| spike: p50 73 ms, p90 110 ms, max 164 ms | trong đó **logic p50 = 61 ms** (`UiHeartBeat`), paint p50 11 ms |
| **`shift` (chi phí nội suy POSSHIFT)** | **max = 0 ms** |
| cross biên region | p50 4 / 10 s, không gây spike |

⇒ Lớp nội suy **tốn 0 ms** và chỉ có **1,4 cú khựng/phút** so với **3,5 cú nắn mạng/phút**.
Nguyên nhân chính của "giựt" là **tầng đồng bộ vị trí**, không phải tầng vẽ.

> ⚠️ **Đừng dùng số tổng của `jx_paint.log`**: cả tệp 150 MB có 125.025 spike và max 162 giây,
> nhưng đó là **tích luỹ nhiều phiên** (kể cả lúc thu nhỏ cửa sổ). Phải cắt theo cửa sổ `t=`.

---

## 3. TRIỆU CHỨNG 2 — "dùng thổ địa phù về hay bị nhảy bậy toạ độ"

### 3.1 Nhận diện bản đồ (từ `settings\MapList.ini`)

| sw | tên | vai trò trong phiên |
|---|---|---|
| 10 | Bạch Thạch Động mê cung | bãi cày |
| 11 | **Thành Đô** | thành — phù về |
| 53 | **Tương Dương thành** | thành — phù về |
| 162 | Lạc Dương thành | thành |
| 227 | Sa mạc sơn động 3 | bãi cày |
| 395 | **Thiên Bảo Khố** (千宝库) | map nhiệm vụ tollgate |

32 lần đổi map trong 27,5 phút, trong đó 27 lần đi qua nhánh `nhanh=vaolandau`.

### 3.2 Đường ĐỔI MAP tự nó **KHÔNG SAI**

Kiểm cả 27 lần hạ cánh: **100 % đáp đúng vị trí máy chủ**.

- Đi vào thành: `sv=(10,10,0,0)` (Tương Dương, mps 52032,101696) và `sv=(13,2,0,0)` (Thành Đô,
  mps 96672,162880) — luôn cùng một điểm, `off=(0,0)`, lệch **d=(0,0)** ngay gói sync kế tiếp.
- Về lại bãi: đáp đúng vị trí cũ với offset lẻ (ví dụ `sv=(0,12,28672,24576)`) — đúng hành vi
  "phù về chỗ cũ".
- Chỉ **5/32** lần đổi map có cú nắn trong 8 giây sau đó.

### 3.3 Hai cú "nhảy" bắt được — **là script, đúng thiết kế**

Hai cú giống hệt nhau, cách lúc hạ cánh **đúng 3,73 giây**, ở map 395:

```
client t=739838646  [S6-LOADMAP] sw=395 loadnew=1
client t=739838660  [S6-ME] nhanh=vaolandau sv=(9,7,0,0)      -> đáp mps (45344,102624)  ĐÚNG
server t=739842340  [E4_POS_SETPOS] npc=91477 new=(45248,102112)
server t=739842340  [S12-TELE] CaiBang setpos cùng map -> báo chính chủ (45248,102112)
client t=739842375  [S8-NAN] lệch=390 mps (12,2 ô)            -> SNAP
```

Truy ngược toạ độ ra script: **`SetPos(1414,3191)` = mps (45248,102112)** và
**`SetPos(1414,3197)` = mps (45248,102304)** — đúng hai giá trị `[S12-TELE]` ghi được. Nằm ở:

- `script\task\tollgate\messenger\trap\trap_qianbaoku.lua:11-20` (**bẫy**)
- `script\task\tollgate\messenger\messenger_turerukou.lua:165`
- `script\task\tollgate\messenger\qianbaoku\messenger_turenpc.lua:114,175`

⇒ Đây là **bẫy/điểm vào của map nhiệm vụ Thiên Bảo Khố**, không phải lỗi engine. Cú snap 12 ô là
**đúng** (vá `[S12-TELE]` 27/08 tại `KNpc.cpp:10775-10800` đang chạy tốt: `SetPos` cùng map nay có
gửi `s2c_syncnpcminplayer` riêng cho chủ nhân vật).

### 3.4 🔴 BẮT ĐƯỢC MỘT CA THẬT — lệnh chạy của MAP CŨ lọt vào MAP MỚI

`t=740326002`, hạ cánh Thành Đô. **Cùng một mili-giây**:

```
[S6-ME]   nhanh=vaolandau cl=(12,5,...) reg=-1 sv=(13,2,0,0) reg=0 doing=3
[S12-CUA] mở cửa sổ theo-lệnh tại đặt-lại sv=(96672,162880)
[S6-CMD]  lệnh=run npc=92480 idx=1 ap=0 dich=(42336,102550)     <-- TOẠ ĐỘ MAP CŨ (sw=227)
[S12-THEO] ÁP LỆNH RUN CHO CHÍNH MÌNH dich=(42336,102550)       <-- ĐÃ ÁP
```

Nhân vật vừa đáp ở **(96672,162880)** thì bị ra lệnh chạy tới **(42336,102550)** — cách
**81.192 mps ≈ 2.537 ô**, là toạ độ của map sa mạc vừa rời.

Đường đi của lỗi:

1. `SyncNpcMinPlayer` nhánh `vaolandau` (`KProtocolProcess.cpp:2455`) **mở cửa sổ S12 ngay tại chỗ**
   (`g_uS12CuaSoSelf = timeGetTime()`) để nghe lệnh `run/walk` server phát cho chính mình;
2. gói `do_run` tồn đọng của **map cũ** đang trên đường về, tới ngay sau đó;
3. gác chống-echo `S12_ChoPhepSelf` (`:2408`) chỉ so đích với `g_nS12TuGuiX/Y` = **đích client tự
   gửi lần cuối** — sau teleport giá trị này là **của map cũ**, nên "lệch > 64 mps so với đích tự
   gửi" ⇒ nó **cho qua**;
4. `NetCommandRun` áp `SendCommand(do_run, 42336, 102550)` lên chính nhân vật.

**Lần này vô hại** vì (42336,102550) nằm ngoài khung Thành Đô (x 96256..96768) nên không đi được.
**Nhưng không phải lúc nào cũng vậy**: khung toạ độ của map 395 (x 45056-45568, y 102400-103424)
**nằm gọn bên trong** khung map 227 (x 41984-49152, y 96256-107520). Với cặp map như vậy, đúng cái
lệnh tồn đọng đó là **một điểm hợp lệ trong map mới** ⇒ nhân vật sẽ **thật sự chạy tới đó** —
đúng triệu chứng "nhảy bậy toạ độ".

Tần suất đo được: **1/32 lần hạ cánh** có lệnh xuyên map lọt qua (nhưng chỉ 27 lần đi qua nhánh
`vaolandau`; các lần khác server gửi gói `NPC_SYNC` đầy đủ trước nên không mở cửa sổ).

### 3.5 Cửa sổ S12 **tự làm mới**, có thể mở vô hạn

`KProtocolProcess.cpp:769-772` — mỗi lần áp được một lệnh cho chính mình lại
`g_uS12CuaSoSelf = timeGetTime()`. Nên chỉ cần server phát lệnh `run` liên tục (nó phát ~2 gói/100 ms)
là cửa sổ **không bao giờ đóng**. Đếm được **1.806 lượt `[S12-THEO]`** trong 27,5 phút, trong đó
riêng 3 giây sau các lần hạ cánh đã có **130 lượt** với đích cách chỗ đáp 115-872 mps.

Đây là **hành vi thiết kế** của bản vá 27/08 (để đoạn script "đặt-đi" hiện thành chạy mượt), nhưng
nó cũng có nghĩa là **nhân vật của người chơi đang bị máy chủ lái phần lớn thời gian** — cần chủ
game quyết xem có muốn thế không (mục 6).

### 3.6 Mìn chưa nổ — nhánh 2 của `SyncNpcMinPlayer`

`KProtocolProcess.cpp:2483-2510`:

```c
SubWorld[0].LoadMap(SubWorld[0].m_SubWorldID, dwRegionID);
nRegion = SubWorld[0].FindRegion(dwRegionID);
_ASSERT(nRegion >= 0);                    // Release build: KHÔNG LÀM GÌ
Npc[nNpcIdx].m_RegionIndex = nRegion;     // có thể = -1
SubWorld[0].NpcChangeRegion(-1, SubWorld[0].m_Region[nRegion].m_RegionID, nNpcIdx);
                                          //            ^^^ m_Region[-1] = ĐỌC NGOÀI MẢNG
```

Nếu `LoadMap` không nạp được region (đĩa chậm, region ngoài rìa map) thì `FindRegion` trả `-1`,
`_ASSERT` im lặng ở bản Release, rồi `m_Region[-1]` đọc ra ngoài mảng.

**Nhánh này chạy 0 lần trong 27,5 phút** (`nhanh=loadmap` = 0/29.468) — giống hệt kết quả 26/08.
Chưa nổ, nhưng vẫn là mìn.

### 3.7 Ghi chú trung thực về phạm vi mẫu

Trong 27,5 phút này **auto Dã Tẩu điều khiển toàn bộ**; **không có lần nào chủ tự tay dùng Thổ Địa
Phù, không có lần chết/hồi sinh nào** (0 dòng `doing=10/21` cho idx=1). Vì vậy:

- ✅ **Chứng minh được**: đường đổi map đặt vị trí đúng 27/27 lần; cơ chế lệnh-tồn-đọng-xuyên-map
  có thật (bắt được 1 ca); bẫy script Thiên Bảo Khố gây snap 12 ô.
- ❌ **Chưa chứng minh được**: ca dùng phù bằng tay và ca chết-hồi-sinh. Muốn chốt phải có mẫu
  riêng (mục 6.3).

---

## 4. MỔ NHỊ PHÂN CLIENT VLTK 2.0 — nội suy FPS

Đích: `C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky 2.0\gamecl.exe`
(PDB gốc: `D:\newBuilder\projects\jxvn20\code\product\win32\client\`).

### 4.1 Bung gói

`gamecl.exe` bị **UPX 3.03** đóng gói và **đã xoá PackHeader** ⇒ `upx -d` chịu.

| | |
|---|---|
| Thuật toán | **NRV2E_LE32** |
| Bắt đầu | offset **0x400** (đúng đầu section `UPX1`) |
| Kết quả | 12,5 MB (cắt ở 12 MB, đủ `.text` + `.rdata`) |
| Ánh xạ VA | **VA = 0x401000 + offset** (imagebase 0x400000, UPX0 va=0x1000) |
| Script | `scratchpad\unupx.py` (tự dò 3 thuật toán × 2 kiểu getbit × 7 điểm bắt đầu) |

Trùng đúng công thức đã tìm ra cho `game_y.exe` hồi 15/08 ⇒ dòng client này dùng chung cấu hình UPX.
Giải mã lệnh bằng **capstone** (có sẵn trong Python của máy), `scratchpad\xdis.py`.

### 4.2 Nhịp LOGIC của VLTK = **18 fps**, tính bằng 64-bit

`0x58FB40` (vòng `GameLoop`):

```asm
0x58FB5B  call [0x7847B8]        ; KTimer::GetElapse()  -> eax (ms)
0x58FB5D  mov  ecx, 0x12         ; 18  <-- GAME_FPS
0x58FB62  mul  ecx               ; edx:eax = elapsed * 18   (64 bit)
0x58FB64  push 0 / push 0x3E8 / push edx / push eax
0x58FB6D  call _aulldiv          ; esi = elapsed*18/1000 = SỐ TICK CẦN ĐẠT
0x58FB86  cmp  [0x9BB750], esi   ; m_GameCounter
0x58FB97  jae  <bỏ qua logic>
...
0x58FBEF  mov  eax, [0x9BB750]
0x58FBF4  lea  ecx, [eax+0x708]  ; +1800 tick (=100 s) : chốt chống tụt hậu
0x58FBFA  cmp  esi, ecx
0x58FBFE  mov  eax, esi          ; tụt quá 100 s thì NHẢY THẲNG, không đuổi
0x58FC02  add  eax, ebp          ; ngược lại ++m_GameCounter
0x58FC11  jb   0x58FBA0          ; LẶP tick cho tới khi bắt kịp
```

Khác biệt so với dự án:

| | VLTK 2.0 retail | Dự án (`S3Client.cpp:1396`) |
|---|---|---|
| GAME_FPS | 18 | 18 (`KNpc.h:26`, `S3Client.cpp:65`) |
| phép tính cổng | `mul` 64-bit + `_aulldiv` | `m_GameCounter * 1000 <= GetElapse() * GAME_FPS`, **DWORD 32-bit** |
| tràn số | không (giới hạn là 49,7 ngày của `GetTickCount`) | **tràn ở ~66 giờ chạy liên tục** (`m_GameCounter` là `DWORD`, `S3Client.h:15`) |
| bắt kịp tick | **lặp trong một lượt** cho tới khi kịp | tối đa **1 tick / lượt bơm** |
| chốt chống tụt hậu | có, 1800 tick | không có |

> Cổng **VẼ** của dự án đã dùng hiệu có dấu chống tràn (`S3Client.cpp:1508`
> `if ((int)(nPaintElapse + PAINT_LEAD_MS - s_dwNextPaint) >= 0)`) nhưng cổng **LOGIC** ở dòng 1396
> thì **chưa**. Với client auto chạy 24/7 thì mốc 66 giờ là có thật.

### 4.3 `PaintFps` của VLTK — **có, mặc định 60, KHÔNG có trần**

`0x591EE1` (trong `KMyApp::Initialize`, ngay sau khi đọc `FullScreen`):

```asm
0x591EE1  push 0x3C                ; MẶC ĐỊNH = 60
0x591EE3  push 0x7A411C            ; "PaintFps"
0x591EE8  push 0x78CC2C            ; "Client"
0x591EF5  call esi                 ; IniFile.GetInteger("Client","PaintFps",60,&v)
0x591EFB  cmp  ecx, 1
0x591F00  mov  ecx, 1              ; sàn = 1, KHÔNG kẹp trần
0x591F09  mov  eax, 0x3E8          ; 1000
0x591F0F  idiv ecx                 ; 1000 / PaintFps
0x591F15  mov  [0x8147F8], eax     ; g_nPaintStep (ms giữa hai khung vẽ)
```

Cổng vẽ, `0x58FC13`-`0x58FD09`:

```asm
0x58FC18  call edi                 ; GetElapse()
0x58FC1F  sub  eax, [0x9BB748]     ; now - lastPaintAt
0x58FC25  cmp  eax, [0x8147F8]     ; >= 1000/PaintFps ?
0x58FC2B  jb   <bỏ vẽ>
          ... vẽ ...
0x58FD09  mov  [0x9BB748], eax     ; neo vào MỐC VẼ THẬT
0x58FD0E  push 1 / call Sleep
```

**Dự án làm y hệt** (`S3Client.cpp:1496-1552`): `nPaintStep = 1000/g_nPaintFps`, neo vào mốc vẽ thật,
cộng thêm `PAINT_LEAD_MS = 4`. Khác duy nhất: dự án **kẹp trần 60** (`:509-510`) và mặc định 30.

VLTK còn **phơi PaintFps ra Tuỳ chọn trong game**: `0x48B6C6` ghi `[Options] PaintFPS` (cạnh
`QualityOpen`, `MissleOpen`, `NpcTheSame`, `VisibleNum`), kèm bộ khoá giao diện
`PaintFpsScroll` / `PaintFpsTextLeft` / `PaintFpsTextRight` ⇒ có **thanh trượt** cho người chơi.

### 4.4 🔴 VLTK **KHÔNG nội suy vị trí** — dự án đang VƯỢT bản gốc

- Chuỗi `"PaintInterp"` và cả `"Interp"`: **0 lần** trong toàn bộ ảnh đã bung (12,5 MB).
- Nhánh vẽ `0x58FC13 → 0x58FD0E` **không có một vòng duyệt NPC nào** — chỉ ~8 lệnh `call` là
  UI/Represent + bộ đếm FPS (`0x58FCC4-0x58FD03`, cửa sổ 1000 ms).

⇒ Ở `PaintFps=60`, client retail vẽ 60 khung/giây nhưng **nhân vật vẫn nhảy nấc 18 lần/giây**;
khung dôi ra chỉ làm mượt UI và hiệu ứng. **Đúng y kết luận đã ghi cho `game_y.exe` hồi 15/08**,
nay xác nhận lại trên bản retail mới nhất.

Phần nội suy của dự án (`CoreShell.cpp:2940-2955` + `19909-20030`,
`GOI_PROCFRAME_BREATHE`/`POSSHIFT`, `PAINT_INTERP_SNAP_DIST = 64`,
`MISSLE_INTERP_SNAP_DIST = 512`) là **vượt bản gốc**, và đo được **chi phí 0 ms** ⇒ **nên giữ**.

### 4.5 Bảng so sánh gọn

| | VLTK 2.0 retail (`gamecl.exe`) | Dự án |
|---|---|---|
| Nhịp logic | 18 fps, 64-bit, có lặp bắt kịp + chốt 1800 tick | 18 fps, 32-bit (tràn 66 h), 1 tick/lượt |
| Khoá ini | `[Client] PaintFps`, mặc định **60**, sàn 1, **không trần** | `[Client] PaintFps`, mặc định 30, kẹp **0..60** |
| Thanh trượt trong game | **có** (`[Options] PaintFPS`) | không |
| Công thức bước vẽ | `1000/PaintFps`, neo mốc vẽ thật | như VLTK + `PAINT_LEAD_MS=4` |
| Nội suy vị trí NPC | **KHÔNG** | **CÓ** (`POSSHIFT`, alpha 0..1000) |
| Nội suy đạn/kỹ năng | **KHÔNG** | **CÓ** (`KMissle m_nDraw*`) |
| Nghỉ giữa lượt | `Sleep(1)` mỗi lượt | `Sleep(1)` có điều kiện + `timeBeginPeriod` khi >30 fps |

### 4.6 Hạn chế còn lại của lớp nội suy dự án (đo từ mã, chưa đo hiện tượng)

`alpha = (nPaintElapse - s_dwLastTickAt) * 1000 / s_dwTickSpan` (`S3Client.cpp:1528`), trong đó
`s_dwTickSpan` là **khoảng của tick TRƯỚC**, còn `CoreShell.cpp:19967` kẹp `alpha ≤ 1000`.

⇒ Khi một tick logic **dài hơn tick trước nó**, alpha chạm 1000 sớm rồi nhân vật **đứng hình** cho
tới tick kế. Đo trong phiên này: **13 cú/10 phút ≥ 56 ms** (đủ trễ một tick) = **1,3 cú/phút** —
nhỏ hơn nhiều so với 3,5 cú nắn mạng/phút, nhưng vẫn là một nguồn "khựng" thật.

---

## 5. Tóm tắt nguyên nhân theo thứ tự sức nặng

| # | Nguyên nhân | Bằng chứng | Tần suất đo được |
|---|---|---|---|
| **1** | Lệch client↔server **chỉ tăng không xả**, ngưỡng nắn 256 mps nằm trong đuôi nhiễu (p99 = 233-248) | mục 2.2-2.5 | **3,5 cú/phút**, mỗi cú 8-13 ô, 89,5 % là **giựt lùi** |
| **2** | Nắn xong gọi `StopPath()` ⇒ client đứng, server đi tiếp ⇒ lệch mọc lại ngay ⇒ nắn tiếp | mục 2.6 | 24/86 cú cách nhau < 3 s |
| **3** | Cửa sổ S12 mở tại `vaolandau` cho **lệnh chạy tồn đọng của map cũ** lọt vào map mới | mục 3.4 | 1/32 lần hạ cánh (lần này vô hại) |
| **4** | Cửa sổ S12 **tự làm mới vô hạn** ⇒ nhân vật bị máy chủ lái phần lớn thời gian | mục 3.5 | 1.806 lệnh/27,5 phút |
| **5** | alpha nội suy kẹp trần khi tick logic dài hơn tick trước ⇒ đứng hình vài khung | mục 4.6 | 1,3 cú/phút |
| **6** | `_ASSERT` vô hiệu ở Release ⇒ `m_Region[-1]` trong nhánh `loadmap` | mục 3.6 | 0 lần (mìn chưa nổ) |
| **7** | Cổng logic `m_GameCounter*1000` tràn DWORD ở ~66 giờ | mục 4.2 | chưa quan sát |
| **—** | Bẫy script Thiên Bảo Khố `SetPos(1414,3191)` | mục 3.3 | **đúng thiết kế, không phải lỗi** |

---

## 6. CẦN CHỦ GAME QUYẾT / CẦN ĐO THÊM — chưa làm gì cả

### 6.1 Ba câu hỏi phải hỏi trước khi đụng vào

1. **Ngưỡng nắn 256 mps** — muốn đi hướng nào?
   (a) giữ 256 nhưng thêm **đường xả mềm** (mỗi gói sync kéo client về phía server một phần nhỏ,
   ví dụ 1/8 độ lệch) để lệch không bao giờ leo tới ngưỡng — nhân vật không bao giờ giựt nhưng
   luôn bị "kéo nhẹ";
   (b) nâng ngưỡng (ví dụ 512 = 16 ô) — ít giựt hơn nhưng lệch thật to hơn, máy đánh/máy Tống Kim
   bắn theo toạ độ client sẽ sai xa hơn;
   (c) giữ nguyên.
   **Tôi không tự chọn** vì (b) đụng thẳng vào chất lượng máy đánh đã ghi ở `[FIX-3 26/08]`.

2. **`StopPath()` sau khi nắn** — có nên bỏ không? Bỏ thì hết vòng tự nuôi ở mục 2.6, nhưng đường
   A* cũ tính từ điểm xuất phát sai sẽ được chạy tiếp. Đây là đánh đổi về **luật chơi của auto**,
   phải chủ quyết.

3. **Cửa sổ S12 lái nhân vật** (mục 3.5) — 1.806 lệnh/27,5 phút là **rất nhiều**. Có phải ý đồ
   ban đầu chỉ dành cho đoạn teleport Tống Kim không? Nếu đúng thì nên đóng lại thành "chỉ mở khi
   thật sự bị đặt-đi", chứ không mở mãi.

### 6.2 Việc có thể làm mà **không** đụng luật chơi (vẫn chờ chủ duyệt)

- Gác `S12_ChoPhepSelf`: **bỏ mọi lệnh có đích nằm ngoài khung toạ độ của map hiện tại**. Trị đúng
  ca ở mục 3.4, không đổi hành vi nào khác. (Cần bổ sung: xoá `g_nS12TuGuiX/Y` khi đổi map.)
- `KProtocolProcess.cpp:2489`: thay `_ASSERT(nRegion >= 0)` bằng kiểm thật + `return` (mục 3.6).
- `S3Client.cpp:1396`: đổi cổng logic sang hiệu có dấu như cổng vẽ đã làm (mục 4.2).

### 6.3 Mẫu còn thiếu — nhờ chủ làm để chốt nốt

Log đang bật sẵn (`AutoLog=1`, `PaintLog=1`). Cần trong **một phiên riêng**:

1. **Tự tay dùng Thổ Địa Phù 3-5 lần** (tắt auto), báo giờ — để có mẫu `vaolandau` không lẫn auto.
2. **Chết 1 lần và hồi sinh** — cả hai log hiện tại đều **không có** dòng `doing=10/21` nào cho
   idx=1, nên nhánh chết/hồi sinh vẫn chưa đo được lần nào.
3. **Đứng yên hoàn toàn 2 phút** rồi **chạy thẳng 2 phút** (không đánh) — để tách được phần lệch
   do trễ mạng khỏi phần lệch do đổi lệnh, và trả lời câu "vì sao p90 tăng gấp đôi so với 25/08".

### 6.4 Về nội suy FPS

Không có việc gì phải port từ VLTK — **bản retail còn thiếu hơn dự án**. Nếu muốn bám sát retail thì
chỉ có hai chi tiết nhỏ: bỏ trần 60 của `PaintFps` và thêm thanh trượt `[Options] PaintFPS` trong
Tuỳ chọn. Cả hai đều là **tuỳ chọn thẩm mỹ**, không liên quan tới lỗi giựt.

---

## 7. Công cụ để lại (scratchpad phiên `81d2e6d2`)

| tệp | dùng để |
|---|---|
| `an_drift.py <log>` | phân bố độ lệch của chính nhân vật + thống kê `[S8-NAN]` |
| `an_nan.py <log>` | dựng lại đường cong lệch 2,5 s trước mỗi cú nắn |
| `unupx.py <exe> <out>` | bung UPX biến thể (tự dò NRV2B/D/E × le32/8 × 7 điểm bắt đầu) |
| `xdis.py <VA>:<n>` | giải mã lệnh x86 tại VA trong ảnh đã bung (capstone) |
| `pe_info.py` / `pe_strings.py` | đọc header PE, rút chuỗi có lọc |
| `gamecl_unpacked.bin` | ảnh `gamecl.exe` đã bung, VA = 0x401000 + offset |

---

# 8. GỐC THẬT của "giựt tới/giựt lùi" — truy tới đáy (bổ sung 03/09, phần 2)

Mục 2 mô tả *hiện tượng* (bánh cóc). Phần này trả lời *tại sao máy chủ luôn thua client từng bậc* —
đúng câu hỏi cần để **sửa gốc, không chữa triệu chứng**.

## 8.1 Nhân vật của chính mình chạy bằng HAI BỘ MÔ PHỎNG ĐỘC LẬP

Truy từ mã (đã đọc đủ chuỗi gọi hai phía):

**Client** (`!_SERVER`), khi auto/WAuto ra lệnh đi:
```
ClientGotoPos / auto  → SendClientCmdRun(dest)   // gửi ĐÍCH cho máy chủ
                      → SendCommand(do_run,dest) // ĐỒNG THỜI tự chạy máy cục bộ
   mỗi tick: Activate → ProcCommand → RunTo → NewPath(dest){m_DesX/Y=dest}; DoRun
             → OnRun → ServeMove(runspeed) → m_PathFinder.GetDir(... m_DesX,m_DesY ...)
                → SubWorld.TestBarrier/TestBarrierMin   // ĐỌC LƯỚI _Region_C.dat
```

**Máy chủ** (`_SERVER`), nhận đúng cái đích đó qua mạng:
```
c2s_npcrun → NpcRunCommand → Npc[player].SendCommand(do_run, ParamX,ParamY) // CHỈ nhận ĐÍCH
   mỗi tick: Activate → ProcCommand → RunTo → NewPath(dest); DoRun
             → OnRun → ServeMove(runspeed) → m_PathFinder.GetDir(...)
                → SubWorld.TestBarrier/TestBarrierMin   // ĐỌC LƯỚI _Region_S.dat
```

⇒ Máy chủ **không bao giờ nhận vị trí thật của client**, chỉ nhận **đích**, rồi **tự mô phỏng lại
đường đi**. `NpcRunCommand` (`KProtocolProcess.cpp:5639`) chỉ có đúng một dòng
`SendCommand(do_run, ParamX, ParamY)` — không đọc toạ độ client. Đây là **mô phỏng có thẩm quyền**
(authoritative), song song với mô phỏng dự đoán của client.

## 8.2 Ba nguồn làm hai bộ mô phỏng lệch — không nguồn nào tự xả

| | nguồn | vì sao cộng dồn |
|---|---|---|
| **(a)** | **Trễ khởi động mỗi chặng.** Client chạy chặng mới **ngay**; máy chủ chỉ chạy sau khi gói `c2s_npcrun` tới (trễ mạng + tối đa 1 nhịp 18 fps ≈ 55 ms). Trong khoảng đó client đã đi, máy chủ chưa. | mỗi lần **đổi đích** thêm một bậc; đo được **43% số bậc** trùng retarget |
| **(b)** | **Đổi trạng thái không đồng thời.** Khi tung chiêu/đánh, client dừng đi ở nhịp này, máy chủ dừng ở nhịp khác (`do_skill`/`do_attack` chặn `ServeMove` khác thời điểm hai bên). | **31% số bậc** trùng đổi `doing`; nhiều nhất `stand→run`, `run→runattack` |
| **(c)** | **Hai lưới vật cản khác nhau.** `m_PathFinder.GetDir` → `TestBarrier` đọc `_Region_C.dat` (client) vs `_Region_S.dat` (máy chủ); đã đo lệch **25,7%–67%** ô ở vài map ([[jx1-tk-kim-2loi-goc-dongbo-2608]]). Hai bên né chướng ngại **khác hướng** trong cùng một chặng. | **40% số bậc** xảy ra lúc **chạy thẳng** (không đổi đích/trạng thái) = phần lệch cộng dồn trong lòng một chặng |

**Điểm mấu chốt — bộ điều khiển (auto) nuôi bánh cóc:** auto Dã Tẩu/đuổi mục tiêu **ra đích mới
trước khi chặng cũ hoàn tất**. Hai bộ mô phỏng chỉ hội tụ về 0 khi **cả hai đứng yên tại cùng điểm
đích đã tới**. Auto không bao giờ để điều đó xảy ra ⇒ bậc chỉ thêm, không bớt.

**Không có tầng hoà giải (reconciliation):** `SyncNpcMinPlayer` **vứt bỏ** vị trí có thẩm quyền của
máy chủ ở mọi mức lệch < 256 (`NormalSync` `:2322` loại trừ chính mình; `ServeMove` chỉ nắn khi bị
chặn). Trên 256 thì "hoà giải" = **dịch chuyển cứng + `StopPath`**.

## 8.3 Phân biệt "dẫn trước lành mạnh" với "lệch bệnh lý" — bằng chứng

Một phần offset là **lành**: client dự đoán chạy trước máy chủ ~một khoảng trễ, còn gói min-sync mang
vị trí máy chủ **đã cũ** ~1 nhịp. Cái này **nên giữ** (chính là thứ làm chuyển động mượt). Ghi chú
25/08 đã nêu: p50 ~1,45 ô khi chạy là bình thường.

Nhưng đo lại cho thấy phần **bệnh lý** có thật, không phải ảo giác lấy mẫu: trong cú nắn #9, offset
**giữ phẳng ở 207 mps (6,5 ô) suốt ~700 ms / 3 mẫu liên tiếp** rồi mới nhảy tiếp. Một khoảng dẫn
trước lành mạnh **không thể** đứng yên ở 6,5 ô — đó là **lệch tích luỹ thật** giữa hai đường đi, đúng
thứ cần khử.

## 8.4 Vì sao không sửa được bằng cách chỉnh ngưỡng

- **Nâng 256 → 512**: chỉ dời mốc nổ, bánh cóc vẫn leo, nổ thưa hơn nhưng **mỗi cú giật XA hơn**, và
  máy đánh/Tống Kim (bắn theo toạ độ client) sai xa hơn. = **chữa triệu chứng**.
- **Bỏ `StopPath` sau nắn**: bớt vòng tự nuôi (mục 2.6) nhưng đường A* cũ tính từ điểm sai vẫn chạy.
  = **nửa vời**.

Cả hai đều **không đụng tới nguồn (a)(b)(c)**.

## 8.5 BA TẦNG FIX GỐC — xếp theo độ triệt để, ghi rõ cái nào chạm Gate 2

> Đều **chưa làm**. Cần chủ game duyệt vì (Tầng A) đụng dò đường, (Tầng C) đụng giao thức.

**Tầng B — Hoà giải liên tục có vùng chết (fix gốc thực dụng, KHÔNG đụng giao thức).**
Thay nhánh nhị phân "dưới 256 không làm gì / tại 256 dịch cứng" trong `SyncNpcMinPlayer` bằng:
- giữ **vùng chết ~2 ô** (bảo toàn phần dẫn trước lành mạnh — không đụng dự đoán);
- với phần lệch **vượt** vùng chết, mỗi gói sync **kéo client về phía máy chủ một phần nhỏ**
  (ví dụ 1/4 phần dôi), để lệch không bao giờ leo tới ngưỡng nổ; tầng nội suy vẽ (`PAINT_INTERP_SNAP_DIST=64`)
  làm mượt từng cú kéo ⇒ mắt không thấy giật.

Đây **là fix gốc của sự CỘNG DỒN** (khử bánh cóc tận cơ chế, không phải dời ngưỡng). Gói min-sync
**đã mang sẵn** vị trí máy chủ nên **không cần đổi giao thức**. Rủi ro: **vừa** — phải chỉnh vùng chết
+ hệ số; bản "nắn nguyên phát" từng bị bác 25/08 chính vì **không** có hai thứ đó. Khử được cả (a)(b);
với (c) thì kéo lệch về nhưng vẫn còn dao động nền.

**Tầng A — Khử nguồn (c) tận gốc: hợp nhất lưới vật cản.**
Cho `TestBarrier`/`TestBarrierMin` khi tính **đường đi của chính người chơi** đọc **cùng dữ liệu**
máy chủ dùng (một lưới hoà giải, hoặc client áp cùng luật "thiếu dữ liệu = vật cản"). Nếu hai lưới
khớp, phần (c) biến mất, chỉ còn (a) trễ mạng có giới hạn và tự xả khi tới đích. **Không đụng giao
thức.** Rủi ro: **cao** (đổi hành vi dò đường, làm sai có thể tái sinh "đi xuyên tường"), khối lớn;
đã có công cụ đo `ReverseTools/tk_luoi_client_vs_server.py`. Không khử (a)(b) — nên đi kèm Tầng B.

**Tầng C — Dự đoán + hoà giải chuẩn (input-replay). ⚠️ ĐỤNG GIAO THỨC = Gate 2, phải hỏi chủ.**
Thêm **số thứ tự** vào gói `NPC_PLAYER_TYPE_NORMAL_SYNC` (hiện **không có** — chỉ ID + toạ độ +
doing, `KProtocol.h:531-543`); client lưu lịch sử lệnh; khi nhận sync thì **đặt về vị trí máy chủ rồi
diễn lại các lệnh chưa được xác nhận**. Kết quả: lệch **có giới hạn cứng**, đúng sách giáo khoa. Đúng
nhất nhưng **sửa gói mạng = đổi hợp đồng client↔server** ⇒ theo Gate 2 **không tự ý làm**.

**Yếu tố bồi (không phải fix chính):** auto retargeting quá dày nuôi nguồn (a). Nếu WAuto chỉ ra đích
mới khi mục tiêu dời > N ô thì số chặng giảm ⇒ ít bậc hơn. Là **tinh chỉnh phía WAuto**, giảm tải chứ
không trị gốc.

## 8.6 Khuyến nghị (chờ chủ quyết, chưa làm)

1. **Tầng B** là fix gốc thực dụng nhất, không đụng giao thức — nên làm trước. Cần chủ duyệt hệ số
   (vùng chết 2 ô, kéo 1/4/phát) rồi đo `[S8-NAN]`/phân bố offset trước-sau.
2. **Tầng A** làm kèm nếu muốn khử luôn (c); nhưng rủi ro dò đường cao, phải đo lưới trước.
3. **Tầng C** chỉ khi chủ chấp nhận sửa giao thức — cho kết quả đúng nhất.


---

# 9. PHÂN TÍCH LẠI (03/09, phần 3) — tự phản biện mục 8, đo lại, RA GỐC KHÁC

Chủ yêu cầu *"chưa fix, phân tích lại"*. Tôi kiểm lại từng khẳng định của mục 8 bằng phép đo mới trên
bản chụp log (`scratchpad\logsnap\cl.log` 82 MB + `sv.log` 85 MB, cùng đồng hồ `timeGetTime`).

## 9.1 🔴 NHỮNG GÌ MỤC 8 NÓI SAI — RÚT LẠI

| Khẳng định ở mục 8 | Phép đo | Kết luận |
|---|---|---|
| "(c) hai lưới vật cản khác nhau ⇒ **40 % số bậc** lúc chạy thẳng" | tốc độ tăng offset trong đoạn **chạy ổn định** (cùng doing, cùng đích, ≥400 ms): `run` p50 = **−2,6 mps/s**, 46 % đoạn *giảm*; `runattack` p50 = **−37 mps/s** | **SAI.** Lúc chạy thẳng offset **không tăng đều** — dao động hai chiều quanh 0. Lưới vật cản không phải nguồn cộng dồn. |
| "(a) trễ mạng mỗi chặng" là nguồn chính | client và máy chủ **cùng một máy**; trễ mạng ≈ 0; trễ tối đa 1–2 nhịp = 26–52 mps | **Chỉ là nền**, không giải thích được offset **đứng phẳng ở 207 mps (6,5 ô)**. |
| Giả thuyết trung gian "+50 tốc độ runattack chỉ client có" | máy chủ có **2.732 dòng `doing=18`** cho 91477, `E4_MOVE_PATH … speed=31 doing=18` ở CẢ hai bên; speed 26/26, 31/31, 21/21 | **SAI.** Tốc độ hai bên khớp. |
| Giả thuyết trung gian "gói hurt tới chính chủ bị ngân sách broadcast 100 nuốt" | `[S6-BANG]` phiên này: `dung=19/256`, **0 bot** trong vùng | **Không áp dụng phiên này** (`KRegion::BroadCast` quả có trừ ngân sách cho cả bot không kết nối — để riêng, mục 9.6). |

Các số **vẫn đứng vững**: p90 165 / p99 248 mps; 3,5 cú nắn/phút; 89,5 % client ở phía trước; vòng tự
nuôi `StopPath`; bẫy script map 395; ca lệnh map cũ lọt cửa sổ S12; VLTK không nội suy.

## 9.2 Bánh cóc nằm ĐÚNG ở khoảnh khắc chuyển trạng thái — và BẤT ĐỐI XỨNG

Δoffset = (offset ổn định ≥600 ms **sau** chuyển) − (offset ngay **trước** chuyển):

| chuyển | n | p25 | p50 | p75 |
|---|---|---|---|---|
| `stand→run` | 38 | −1 | **+20** | **+52** |
| `attack→run` | 13 | −40 | **+29** | **+103** |
| `run→stand` | 41 | −26 | 0 | 0 |
| `run→attack` | 25 | −21 | 0 | +20 |
| `stand→attack`, `attack→stand`, `stand→magic` | 92 | 0 | 0 | 0 |

**Tổng: BẮT ĐẦU chạy trung bình +31,0 mps (n=58); DỪNG lại trung bình −1,6 mps (n=86)
⇒ net +29 mps mỗi chu kỳ đánh–chạy.** ~8–9 chu kỳ là chạm 256. Khớp 3,5 cú/phút.

## 9.3 Máy chủ chấp nhận lệnh chạy TRỄ HÀNG TRĂM MS — trên cùng một máy

Từ "mẫu cuối client còn đứng" tới "echo `DoRun` đầu tiên của máy chủ" (`[S6-CMD] idx=1`):

| trạng thái trước | n | p25 | p50 | p75 | p90 |
|---|---|---|---|---|---|
| `stand` | 321 | 112 | **217 ms** | 560 | **1.240 ms** |
| `attack` | 178 | 136 | **329 ms** | 559 | 888 |

**141/508 lần (28 %) trễ > 500 ms, 56 lần > 1 s.** Trong lúc client chạy liên tục (45 đoạn, 59 s),
**27 % thời gian** nằm trong khoảng lặng ≥300 ms không có lệnh chạy nào được máy chủ nhận
(p99 khoảng lặng = 662 ms, max 1.543 ms) — dù client gửi ~9–13 lệnh/giây.

## 9.4 🔴🔴 GỐC THẬT — hai bên KHÔNG LÀM CÙNG MỘT VIỆC

Ma trận nhầm lẫn trạng thái **client × máy chủ** tại cùng mốc giờ (±60 ms), 5.736 cặp mẫu:

| client \ server | stand | run | attack | runatk | tổng | **khác nhau** |
|---|---|---|---|---|---|---|
| **stand** | 485 | 91 | 275 | 196 | 1.054 | **54 %** |
| **run** | 21 | 494 | 30 | 50 | 598 | 17 % |
| **attack** | 626 | 36 | 1.879 | 281 | 2.825 | **33 %** |
| **runatk** | 71 | 115 | 85 | 739 | 1.023 | 28 % |

**33,8 % thời gian, cùng một nhân vật đang làm hai việc khác nhau ở hai bên.** Client chạy mà máy chủ
không chạy: 14 %. Máy chủ dash (`runatk`) trong khi client đứng: 196 mẫu.

### Cơ chế — ba mắt xích trong ĐƯỜNG ỐNG LỆNH (đều là mã, đều đo được)

**(1) Hàng đợi lệnh MỘT KHE, ghi đè, gộp gói.**
`KNpc::SendCommand` (`KNpc.cpp:5693-5697`) ghi `m_Command` **vô điều kiện**, không kiểm khe đang có
lệnh. `ProcCommand` chỉ chạy **1 lần/nhịp** trong `Activate`. Phía máy chủ, `KSwordOnLineSever::Breathe`
(`KSOServer.cpp:1097+`) gọi **`MessageLoop()` xử lý HẾT gói đến, rồi mới `MainLoop()`** ⇒ mọi gói tới giữa
hai nhịp **gộp vào một khe — gói cuối thắng**. Client thì `SendAllCommand()` gom gói **cả khung** gửi một
chùm; còn chính client tiêu thụ khe **giữa khung** (`KSubWorld::Breathe`: `m_Region[i].Activate()`
`:1127/1141` chạy **trước** bộ bước đường `:1171+` đặt lệnh chạy) ⇒ **client thi hành CẢ chiêu (nhịp này)
và chạy (nhịp sau), máy chủ chỉ thi hành gói CUỐI của chùm.**
Đo: lệnh chiêu tới máy chủ mà **biến mất trước khi được xử lý** (PKT-IN − SWALLOW − CAST − DENY − ABORT):
1985: 5.775 → **438 mất (7,6 %)**; 1977: **9,9 %**; 1967: 5,5 %. Lệnh chạy không có nhãn máy chủ nhưng
chịu cùng khe.

**(2) Lệnh tới lúc AI tắt bị VỨT thay vì xếp chờ.**
`ProcCommand` nhánh `nAI == 0` (`KNpc.cpp:1057-1099`): `case do_run:` **không gọi `RunTo`**, chỉ bật lại
`m_ProcessAI` nếu đang `do_stand`, rồi dòng cuối `m_Command.CmdKind = do_none` **xoá lệnh**. Mỗi bên tắt
AI ở thời điểm **riêng** (hoạt ảnh chiêu bắt đầu lệch nhịp; `do_hurt` phía máy chủ) ⇒ mỗi bên vứt **những
lệnh khác nhau**. Bản vá 24/08 chỉ giữ lệnh ở **khung cuối** hoạt ảnh. Đo: `[S3-CMD-SWALLOW]` 91477
= 1.472 lệnh chiêu bị nuốt trong 27 phút (chỉ chiêu mới có nhãn).

**(3) Chiêu lướt (dash) thi hành phía client KHÔNG CHỜ máy chủ, và máy chủ KHÔNG BAO GIỜ xác nhận cho chính chủ.**
1977 **Hàm Sơn Kích** (Cái Bang, `SkillStyle=1`, cùng lớp 2118 Tạp Đạp Lưu Tinh / 995 Huyền Ảnh Mê Tung
Bộ) → `DoRunAttack` → `do_runattack` chạy tốc độ +50 (kẹp 31/nhịp) tới vị trí mục tiêu **snapshot lúc
cast**, ~440 ms ⇒ dịch tới **~250 mps** mỗi cú. Client cast **tại chỗ** (`SendCommand(do_skill)` + gửi gói);
gói xác nhận `s2c_skillcast` của máy chủ về tới `NetCommandSkill` (`KProtocolProcess.cpp`) nhưng chỉ áp khi
**`ConformIdx(nIdx)` = KHÔNG PHẢI CHÍNH MÌNH** ⇒ chính chủ **không bao giờ** được máy chủ điều chỉnh.

Đối chiếu **từng gói dash** client gửi với kết cục máy chủ (677 gói trong cửa sổ chung):

| máy chủ … | số gói | tỉ lệ |
|---|---|---|
| DASH thật (`S4-CAST 1977` ≤250 ms) | 329 | **49 %** |
| NUỐT (AI đang tắt) | 245 | 36 % |
| TỪ CHỐI (deny/abort) | 15 | 2 % |
| **BIẾN MẤT không dấu vết** (bị đè khe) | **88** | **13 %** — trong đó **24** lần ngay sau có echo `DoRun`: **máy chủ CHẠY thay vì dash** |

Cùng cửa sổ: **client dash 259 đợt, máy chủ dash 328 lần** — máy chủ dash **nhiều hơn 27 %** (gói gửi
lặp ~2,4×/quyết định tới lúc AI máy chủ rảnh nhưng client đã tự nuốt lệnh của mình). **Dash một phía**
= lệch ~250 mps tức thì. **71 % số cú nắn `[S8-NAN]` có client dash trong 2 giây trước đó.**

## 9.5 Chuỗi nhân–quả đã đo trọn

```
WAuto phát ~12 lệnh chiêu/s + bộ bước đường ~9 lệnh chạy/s
  → client gom chùm/khung, tiêu thụ khe GIỮA khung   → thi hành chiêu + chạy
  → máy chủ gộp chùm vào MỘT khe trước tick          → chỉ thi hành gói CUỐI       (1)
  → bên nào AI đang tắt thì VỨT lệnh                 → mỗi bên vứt lệnh KHÁC nhau  (2)
  → dash tự thi hành, máy chủ không xác nhận chủ     → dash MỘT PHÍA ~250 mps      (3)
  ⇒ 33,8 % thời gian hai bên khác trạng thái
  ⇒ +31 mps mỗi lần bắt đầu chạy, 0 khi dừng          → bánh cóc +29 mps/chu kỳ
  ⇒ không tầng hoà giải dưới 256                       → 8–9 chu kỳ = 1 cú giựt 8–13 ô
```

## 9.6 Hướng FIX GỐC (chưa làm — chờ chủ duyệt), xếp theo mắt xích

| # | Mắt xích | Sửa gốc | Đụng giao thức? | Ghi chú |
|---|---|---|---|---|
| **F1** | (1)+(2) đường ống lệnh | **Tách khe**: lệnh **di chuyển** và lệnh **chiêu** mỗi loại một khe (không đè nhau); lệnh tới lúc AI tắt **giữ chờ** rồi thi hành khi AI bật lại, thay vì xoá. Mã dùng chung ⇒ hai bên đối xứng. | **Không** | Sửa `KNpc::SendCommand` / `ProcCommand`. Thay đổi hành vi nhận lệnh — phải test kỹ bot/AI (cùng đường mã). |
| **F2** | (3) dash không xác nhận | Chính chủ **theo máy chủ**: bỏ loại trừ self trong `NetCommandSkill` cho chiêu dash **và** tắt tự thi hành dash cục bộ (chờ `s2c_skillcast`, ~1–2 nhịp trên LAN) — hoặc giữ dự đoán nhưng **nắn ngay** theo cú cast của máy chủ. | **Không** | Gói `s2c_skillcast` đã tồn tại và đã tới client. Cẩn thận không dash **hai lần**. |
| **F3** | không hoà giải | Tầng B (mục 8.5): hoà giải liên tục có vùng chết ~2 ô | **Không** | Lưới an toàn cho phần dư (hurt chỉ máy chủ biết, snapshot mục tiêu khác nhau). |
| **F4** | mìn riêng | `KRegion::BroadCast` trừ ngân sách 100 cho **cả bot không kết nối** (`nMaxCount--` ngoài `if (m_nNetConnectIdx >= 0)`) ⇒ vùng đông bot làm người chơi thật mất gói hurt/skill/run của **chính mình**. Phiên này vùng rỗng nên chưa lộ. | **Không** | Nên sửa cùng đợt: chỉ trừ ngân sách khi thật sự gửi. |

**Không phải fix gốc (nhắc lại):** nâng 256→512; bỏ `StopPath`; giảm tần suất WAuto (chỉ giảm tải).

## 9.7 Công cụ để lại thêm (scratchpad `logsnap\`)

`cls.py` (phân loại bậc theo retarget/doing) · `rate.py` (tốc độ tăng offset theo trạng thái) ·
`trans.py` (Δoffset theo chuyển trạng thái) · `delay.py` (trễ chấp nhận của máy chủ) · `stall.py` (mổ ca
trễ dài nhất, hai log) · `confus.py` (ma trận nhầm lẫn) · `dash2.py` (đợt dash + nhịp echo) ·
`burst.py` (kết cục từng gói dash) · bản chụp `cl.log`/`sv.log`.

> ⚠️ Log xoay vòng ở 67 MB (~25 phút) — dữ liệu 739.4M–741.0M của mục 1–3 **đã mất trên đĩa**, chỉ còn
> số liệu đã tính. **Luôn chụp log về scratchpad trước khi phân tích.**


---

# 10. THI CÔNG S13 (03/09 chiều) — vá gốc đường ống lệnh + hoà giải mềm, đã build, ĐÃ QUA PHẢN BIỆN, CHỜ SWAP

Chủ duyệt: *"hãy fix theo bạn và chạy phản biện nếu cần"*. Làm theo mục 9.6: **F1 + F3 + gác S13-XA**; **F2 (dash theo máy chủ) và F4 (ngân sách broadcast) CHƯA làm** — xem 10.6.

## 10.1 Nguyên tắc thi công

- **Không đổi layout `KNpc`, không đổi header, không đổi gói mạng** (Gate 2): khe di chuyển thứ hai là **mảng tĩnh
  `s_S13Move[MAX_NPC]` trong `KNpc.cpp`** đánh chỉ số theo `m_Index`. Server 98.000 × 16 B = 1,5 MB tĩnh.
- **Chỉ bật cho NGƯỜI CHƠI THẬT** (`S13_IsRealPlayer`): server = `kind_player` + `GetPlayerIdx()` hợp lệ +
  `m_nNetConnectIdx >= 0`; client = chính mình. Quái / bot (`m_nNetConnectIdx = -1`) / SimCity (`m_nPlayerIdx = 0`) /
  bạn đồng hành đi **nguyên đường mã cũ** (`pS13Move == NULL`).
- Sửa nguồn bằng `patch_l1.py` (latin-1, giữ EOL từng tệp, byte cao không đổi, FFFD = 0). Build ở worktree
  `D:\GAMEDEVNEW_wt_s13` (tách khỏi phiên khác), `-p:PostBuildEventUseInBuild=false` để post-build **không** chép đè DLL đang chạy.

## 10.2 Ba tệp, những gì đổi

**`KNpc.cpp`**
| chỗ | đổi |
|---|---|
| trước `KNpc::KNpc()` | `s_S13Move[]`, `S13_IsRealPlayer()`, `S13_ClearCmd(nIdx)` (hàm tự do, client gọi từ tệp khác; xoá khe hành động bằng `SendCommand(do_none)` vì `m_Command` private) |
| `Init()` | xoá `s_S13Move[m_Index]` theo chỉ số CŨ (Init được `Remove` gọi khi thu hồi khe) |
| `SendCommand()` | người chơi thật: `do_walk/do_run` → khe di chuyển, `return`; còn lại như cũ |
| `ProcCommand()` | khe hành động xử lý trước (như cũ); nhánh `nAI==0` + `do_skill`: **GIỮ** lệnh chiêu mới nhất (log `[S13-GIU-CHIEU]`), trừ khi đang `do_death/do_revive` thì xoá; cuối hàm nhãn `S13_Move:` xử lý khe di chuyển: **`nAI && m_ProcessAI`** (AI hiện tại — chiêu vừa thi hành ở trên đã tắt AI thì phải giữ, không `RunTo` cắt hoạt ảnh) → `Goto/RunTo`; AI tắt → giữ (RandMove+walk vẫn `Goto` như cũ; đứng mà AI tắt → bật AI như cũ); chết/hồi sinh → xoá |
| `DoDeath()` | người chơi thật: xoá cả hai khe |
| `SetPos()` / `ChangeWorld()` (server) | người chơi thật: xoá cả hai khe (lệnh đang giữ là của chỗ cũ / map cũ) |
| `DoSkill()` `case -1` (server) | `SendCommand(do_run, target)` "quá xa thì chạy tới" **trước S13 là mã chết** (bị dòng cuối `ProcCommand` xoá ngay) — nay chỉ giữ cho NPC không phải người thật, để không tạo "máy chủ tự đuổi bám mà client không biết" |

**`KProtocolProcess.cpp`** (toàn bộ trong `#ifndef _SERVER`)
| chỗ | đổi |
|---|---|
| trước `S12_ChoPhepSelf` | `extern S13_ClearCmd`; `S13_VUNGCHET 64`; `g_nS13KeoCount` |
| `S12_ChoPhepSelf` | **S13-XA**: đích cách mình `|dx|>4096 || |dy|>4096` (128 ô) → vứt, log `[S13-XA]` (Chebyshev — `g_GetDistance` chưa khai báo ở đầu tệp và nhân `int` tràn ở 81.192 mps) |
| `SyncNpcMinPlayer` đầu hàm | `s_uS13LastSync` (static cục bộ), `nS13Dt` = ms giữa hai gói tự-sync |
| nhánh `vaolandau` / `loadmap` / S8 | `S13_ClearCmd(nNpcIdx)` trước khi đặt lại vị trí |
| sau khối `if (nLech >= 256)` | **S13-KEO**: `else if (nLech > 64 && không lướt/nhảy/bị đánh/chết)`: bước = `(lech−64)·dt/450` kẹp [2, 40] mps (55 ms ≈ 1/8 phần dôi; trần 40 chừa 24 cho bước chạy cùng nhịp để tổng dịch chuyển ≤ `PAINT_INTERP_SNAP_DIST` 64); điểm kéo có vật cản (`SubWorld[0].TestBarrier(mps)` ≠ 0 và ≠ `Obstacle_JumpFly`) thì bỏ cú kéo; DecRef → `NpcChangeRegion` (theo REGION ID) → gán cell/off → AddRef (đúng thứ tự nhánh S8); **không** `StopPath`, **không** đụng `m_DesX/Y`; log `[S13-KEO] lech buoc dt doing dem` (throttle 1 s) |

**`KSubWorld.cpp`** — `LoadMap` khi đổi map thật: `S13_ClearCmd(self)` ngay sau `StopPath()`.

## 10.3 Phản biện (tác tử độc lập, chỉ đọc, 2 vòng) — 5 lỗi thật, tất cả đã sửa trước khi build bản cuối

| # | lỗi | sửa |
|---|---|---|
| 1 | `S13_Move` dùng `nAI` chụp lúc vào hàm → chiêu vừa tắt AI mà lệnh chạy vẫn `RunTo` → **cắt hoạt ảnh chiêu** (mana đã trừ, không Cast) | `nAI && m_ProcessAI` |
| 2 | `DoSkill case -1` `SendCommand(do_run)` từ mã chết thành sống **chỉ ở máy chủ** → lệch mới | gác `!S13_IsRealPlayer` |
| 3 | `g_GetDistance` nhân `int` tràn ở 81.192 mps → gác XA vô hiệu đúng ca mục tiêu | Chebyshev (đã sửa song song) |
| 4 | KEO giả định 18 gói/s; vùng đông chỉ ~1 gói/2 s (KRegion::Activate sync 5 NPC/nhịp/region) | bước theo thời gian thật |
| 5–6 | (nghi) điểm kéo trong tường; lệnh tới lúc nằm được giữ rồi nổ sau hồi sinh tại chỗ | kiểm vật cản điểm kéo; chết/hồi sinh → xoá thay vì giữ |
| **8 (vòng 2)** | kiểm vật cản dùng `KRegion::GetBarrier` — bản **client** hàm này **không đọc địa hình** (chỉ báo ô có NPC khi `g_nPbNpcChan`), địa hình client nằm ở `g_ScenePlace` | dùng `SubWorld[0].TestBarrier(mps)` (đúng hàm `KNpcFindPath::GetDir` dùng) |
| 9 (vòng 2, nghi) | trần bước 64 + bước chạy ≤31 trong cùng nhịp vượt ngưỡng snap nội suy 64 → giựt 2–3 ô ở vùng đông | trần 40 |
| 7 | (nghi) hai khe mất thứ tự đến trong cùng tick (`[chạy, chiêu]` → chiêu trước) | **chấp nhận**; WAuto gửi lại ~9 lệnh/s che bớt; ghi nhận |

Đổi ngữ nghĩa công cụ đo: `[S3-CMD-SWALLOW]` **không còn ghi cho người chơi thật** (đã đổi thành giữ, xem `[S13-GIU-CHIEU]`).

## 10.4 Binary — CHƯA SWAP

| tệp | md5 | kích thước | mốc build |
|---|---|---|---|
| `bin\client\CoreClient.dll.moi` | **`9976e63f4097b445abc663642c33a244`** | 2.488.832 | 13:49:23 |
| `bin\server\CoreServer.dll.moi` | **`cb4cf7f0159bb1ed9b28ed058c03efed`** | 18.277.888 | 13:41:50 |

Build từ commit `f2fa3c2b` (= bản `CoreClient.dll.moi 96c3085d` của phiên wauto-c0 đặt 12:44) + 3 tệp S13 ⇒ **tập cha**
của bản họ; đã kiểm chuỗi `[Công Thành]` TCVN3 còn trong DLL (điều kiện của họ). **Không đổi `autoData`/`ExtAuto`** ⇒ tương thích
`Game.exe.moi d3d626ba` + `WAuto.exe.moi 46fdc93f` đang chờ. Bản đang chạy: client `7efb5720`, server `489d587d`.

## 10.5 Nghiệm thu sau swap — chạy lại đúng các script mục 9.7 trên log mới

| chỉ số | trước | kỳ vọng |
|---|---|---|
| `[S8-NAN]` / phút | 3,5 | **< 0,5** (chỉ còn teleport thật / bẫy script) |
| ma trận trạng thái client×server khác nhau (`confus.py`) | 33,8 % | **giảm mạnh** (≤ 15 %) |
| Δoffset khi bắt đầu chạy (`trans.py`) | +31 mps | ~0 |
| `[S13-KEO]` `dem=` | — | có, tăng đều; `buoc` ≤ 40 |
| `[S13-GIU-CHIEU]` phía server (`jx_auto_server.log`) | — | xuất hiện thay cho `S3-CMD-SWALLOW` của 91477 |
| `[S13-XA]` | — | hiếm; xuất hiện đúng lúc đổi map |
| **Hồi quy phải soát**: chiêu bị cắt (mana trừ mà không đánh), nhân vật tự chạy sau hồi sinh/teleport, bot/quái đổi hành vi | | không có |

## 10.6 Còn lại, chưa làm (chờ chủ)

- **F2** dash chính chủ theo `s2c_skillcast` của máy chủ (`NetCommandSkill` hiện loại trừ self) — nếu sau swap `[S8-NAN]`
  vẫn còn gắn với `runatk` thì đây là bước kế.
- **F4** `KRegion::BroadCast` trừ ngân sách 100 cho cả bot không kết nối — cần thiết kế lại (CPU khi vùng 500 bot).
- Mất thứ tự trong tick (phản biện #7) — nếu đo thấy client "đứng sau chiêu" nhiều thì thêm dấu thứ tự cho hai khe.

## 10.7 SAU SWAP LẦN 1 (14:01) — chủ báo "chạy nhanh rồi chậm, đánh không trúng quái" → đo → S13e

Bản đang chạy khi chủ báo: client `9976e63f`, server `cb4cf7f0` (đã kiểm md5 + StartTime tiến trình 14:01). Log client
chứa **13 phút trước swap + 6,4 phút sau swap cùng một phiên** ⇒ so trực tiếp (`logsnap2\post.py`, `confus2.py`).

### Số đo trước → sau S13 (cùng phiên, cùng bãi)

| chỉ số | trước | sau |
|---|---|---|
| offset client↔server p90 / p99 | 149 / 241 mps | **77 / 155 mps** |
| mẫu ≥ 128 mps (4 ô) | 12,8 % | **2,9 %** |
| `[S8-NAN]` / phút | 3,85 | **1,88** (chưa đạt < 0,5) |
| khác trạng thái client×server (chỉ nhãn `S2-NETSKILL-IN`, cùng ngữ nghĩa hai giai đoạn) | 39,0 % | **33,5 %** |
| client đứng mà server chạy | 5,5 % mẫu | **2,8 %** |
| đứng ì khi có mục tiêu (`FIGHT-EMIT` ≤1,5 s trước) | 42 % | **14 %** |
| đánh trúng / phút (server `S4-MSL-HIT`) | 110,8 | 89,8 |
| trúng / chiêu | 2,05 | 2,01 |
| `FIGHT-EMIT` / phút · mục tiêu khác nhau / phút | 82 · 28,9 | 60 · 20,5 |

⇒ Đồng bộ **tốt hơn**, không hồi quy về trạng thái; "trúng/chiêu" không đổi. Số chiêu và số mục tiêu/phút giảm cùng
tỉ lệ ⇒ **đánh ít hơn vì ít mục tiêu hơn trong 6 phút đó**, không phải "trượt nhiều hơn". Cảm giác của chủ đến từ hai
thứ **nhìn thấy được**, đo được rõ:

**(1) KEO kéo lúc đang chạy / đang đánh.** 680 cú kéo / 6,4 phút (≈1,8/s); trong 102 dòng log: **67 lúc `run`, 20 `stand`,
14 `attack`**; bước p90 = 29 mps mỗi gói (~9 gói/s) ≈ kéo lùi 260 mps/s so tốc chạy ~400 mps/s ⇒ **hụt 30–40 % tốc độ theo
từng đợt** = "chạy nhanh rồi chậm"; kéo giữa lúc đánh làm nhân vật trượt, máy đánh (ngưỡng 75 mps, `[FIX-3]` lấy khoảng
cách xấu hơn) rơi ngoài tầm rồi chạy lại. Lúc chạy, phần lệch chủ yếu là **dẫn trước lành + trễ khởi động chặng** — kéo là
sai chỗ.

**(2) 12/12 cú `[S8-NAN]` còn lại đều trong/ngay sau DASH 1977** (`doing=runatk` 11/12, lệch 256–303 = vừa chạm ngưỡng):
client dash ngay, máy chủ **giữ** rồi dash trễ 1–2 nhịp ⇒ vài trăm ms client đi trước > 256 ⇒ nắn cứng + `StopPath` = giựt.
Chính lớp "chiêu lướt giật lùi" đã ghi buổi sáng (ngưỡng 256 < tầm lướt).

**(3) Ghi nhận, chưa đụng:** máy chủ **giữ** chiêu **1967** (WAuto bắn xen kẽ 1967/1985/1977; 1967 **luôn bị từ chối** —
188 deny / 0 cast) **63/149 lần** ⇒ mỗi chu kỳ tốn 1 nhịp deny trước khi 1985 thật chạy. Cấu hình chiêu 1967 của WAuto
nên xem lại; engine có thể xét "không giữ chiêu vừa bị từ chối".

### S13e (client-only, `KProtocolProcess.cpp`, commit `S13e 03/09`)

- **KEO chỉ khi `m_Doing == do_stand`** — lúc đứng, hai bên cùng đứng ⇒ lệch là lệch thật, kéo về đúng và không đụng
  tốc độ chạy / máy đánh.
- **Cửa ân hạn dash cho S8**: đang `runattack/blurmove/jump/jumpattack` hoặc vừa rời < 600 ms ⇒ **không nắn** khi
  256 ≤ lệch < 512 (log `[S13-DASH-GRACE]`); lệch ≥ 512 (16 ô) = dịch chuyển thật vẫn nắn.

| tệp | md5 | kích thước |
|---|---|---|
| `bin\client\CoreClient.dll.moi` | **`5b56367c`** | 2.489.344 |
| `bin\server\CoreServer.dll` (đang chạy, **không đổi**) | `cb4cf7f0` | 18.277.888 |

**Swap:** thoát hẳn `Game.exe` (+ `WAuto.exe`) → chạy `ChoiGame.bat` → mở lại. Không cần đụng server.

**Nghiệm thu S13e:** `[S8-NAN]`/phút phải rơi về **< 0,5** (dash không còn kích nắn; `[S13-DASH-GRACE]` xuất hiện thay);
`[S13-KEO]` chỉ còn `doing=1`; tốc độ chạy đều (p10 ≥ 330 mps/s như sau S13, không còn đợt hụt); trúng/chiêu ~2,0 giữ nguyên.
Nếu còn "đánh không trúng": bước kế là (a) không giữ chiêu vừa bị máy chủ từ chối / rà cấu hình 1967 của WAuto, (b) **F2**
dash theo `s2c_skillcast` của máy chủ.

## 10.8 SAU SWAP S13e (14:21) — chủ báo "hết chạy nhanh–chậm, còn di chuyển tới giựt lùi lại" → đo → S13f (BÁC) → S13g/h

Bản đang chạy khi báo: client `5b56367c` (S13e), server `cb4cf7f0`. Phiên S13e 10,8 phút (`logsnap3\`, pid client 704).

### Số đo phiên S13e

| chỉ số | giá trị | nhận xét |
|---|---|---|
| cú đảo chiều > 120° khi đang chạy (`giutlui.py`) | **30 = 2,8/phút** | = cái chủ thấy; **20/30 có `[S8-NAN]` ngay trước** |
| `[S8-NAN]` | 33 = 3,1/phút, **32/33 lúc `run`**, lệch **257–310** (vừa nhích qua 256) | lệch cộng dồn, không phải teleport |
| `[S13-DASH-GRACE]` | 20 | ân hạn có chạy, nhưng nắn xảy ra **sau** ân hạn, lúc `run` |
| `[S13-KEO]` | 86, **86/86 `doing=stand`** | S13e đúng — hết kéo lúc chạy/đánh (chủ xác nhận hết "nhanh–chậm") |
| `[S12-THEO]` | 969 = 90/phút; 478 tiến / 31 lùi / 26 ngang | cửa sổ mở gần liên tục (33 cú nắn × 3 s + tự làm mới) nhưng đích phần lớn cùng hướng ⇒ không phải thủ phạm chính |
| dash: client 162 đợt vs server `S4-CAST 1977` 158 | máy chủ dash đủ | **bác** giả thuyết "máy chủ mất dash" |
| trễ máy chủ chấp nhận lệnh chạy | **p50 328 ms, p75 887, p90 1443, 44 % > 0,5 s** | trước S13: p50 217, 28 % ⇒ **XẤU HƠN** |
| trước mỗi cú nắn, máy chủ không chạy đã | **p50 1,56 s, p90 2,8 s** | máy chủ đang `CAST 1985` liên tiếp / đứng |

### 🔴 Gốc: MÁY CHỦ ĐÓI DI CHUYỂN — do chính S13c #2

WAuto **chỉ gửi lệnh chạy khi client NGOÀI tầm** (`KPlayerAuto.cpp:1333-1341`, ≥5 khung/lần) và gửi chiêu khi client đã tới tầm.
Máy chủ tụt sau vài ô ⇒ khi client tới tầm và đánh, máy chủ **chỉ còn nhận chiêu**, chiêu cận chiến ở máy chủ **quá tầm**
(`CanCastSkill` = −1, `KSkills.cpp:366-372`) ⇒ từ chối ⇒ **đứng**, và **không có lệnh nào để đuổi kịp**. Sau ~1,5 s lệch > 256
⇒ nắn cứng kéo client **lùi về chỗ máy chủ đứng** = "di chuyển tới rồi giựt lùi lại".

Engine có sẵn dòng xử lý ca này: `DoSkill` case −1 (`#ifdef _SERVER`) → `SendCommand(do_run, target)` = NPC tự chạy tới mục
tiêu. Nhưng (phản biện đính chính) nó là **mã chết cho MỌI loại NPC**: `DoSkill` chỉ được gọi từ `ProcCommand`, và dòng cuối
`ProcCommand` xoá `m_Command` ngay sau đó. Hai khe của S13 làm nó sống lại cho người chơi thật; phản biện S13c #2 lo "máy chủ
tự đuổi mà client không biết" nên tôi **gác** lại — **chính cái gác đó làm máy chủ đói**.

### S13f — ĐÃ BUILD NHƯNG BỊ PHẢN BIỆN BÁC, ĐÃ GỠ (không swap)

Thử "ý định mới nhất thắng" giữa hai khe (lệnh tới sau > 60 ms huỷ lệnh cũ ở khe kia) + bảo vệ chiêu dịch chuyển đang giữ +
S12 chỉ làm mới khi thụ động. Phản biện (vòng 3) tìm **3 lỗi thật**: (1) huỷ theo thời điểm **đến** ≠ thi hành theo **thứ tự
thời gian** của client; chiêu tới sau huỷ lệnh chạy nhưng chiêu đó ở máy chủ **bị từ chối quá tầm** ⇒ máy chủ vẫn đứng, còn
hồi quy ca "client chạy tới tầm rồi bắn"; (2) bỏ chiêu thường khi dash đang giữ ⇒ client thi hành chiêu mà máy chủ bỏ (WAuto
**không** gửi lại) = đánh hụt; (3) `NetCommandWalk` vẫn làm mới cửa sổ S12 vô điều kiện. ⇒ **gỡ toàn bộ S13f**, hai cây về
đúng S13e (`811f93ac`, so byte = 0).

### S13g + S13h — MỘT chỗ sửa (`KNpc.cpp`, `DoSkill` case −1, `#ifdef _SERVER`)

Bỏ gác `if (!S13_IsRealPlayer(this))`: chiêu cận chiến quá tầm ⇒ máy chủ `SendCommand(do_run, target)` cho người chơi thật.
Nhờ hai khe, `do_run` vào khe di chuyển (không bị xoá) và thi hành **ngay cùng lượt** tại `S13_Move` (`nAI && m_ProcessAI`
vẫn 1 vì case −1 `return` trước `DoOrdinSkill`) ⇒ máy chủ chạy tới mục tiêu ⇒ đuổi kịp client (đang đứng cạnh mục tiêu) ⇒ hết
lệch ⇒ hết nắn ⇒ hết giựt lùi. Client **không đổi mã** (khối nằm trong `#ifdef _SERVER`).

Phản biện S13g (vòng 4, `KNpc.cpp:2749-2762`): (a) luồng đúng — `break` → `m_Command = do_none` → nhãn `S13_Move` → `RunTo`
cùng lượt; (b) không vòng lặp xấu — `DoRun` có `if (m_Doing == do_run) return;` trước khi đặt lại khung ⇒ đang chạy chỉ đổi
đích, không khựng; chi phí 1 gói `s2c_npcrun`/chiêu (~12/s lúc đuổi); (c) vọng về client bị `ConformIdx` loại, chỉ lọt cửa sổ
S12 3 s sau nắn — khi đó đích ≈ vị trí quái = cùng hướng máy chủ (hội tụ, không giằng co); (d) bot/quái không đổi (khe rỗng,
`m_Command` vẫn bị xoá như cũ); **(e) 1 lỗi thật → S13h**: khe di chuyển đang giữ lệnh chạy riêng của client (lùi/kite) mà
chiêu thi hành ⇒ lệnh tự đuổi **đè** mất ý định client ⇒ lệch mới. Sửa: `if (s_S13Move[m_Index].CmdKind == do_none)
SendCommand(do_run, …)` — chỉ tự đuổi khi khe rỗng (WAuto: đích run = vị trí quái nên tương đương; người chơi tay: client thắng).

| tệp | md5 | kích thước | swap |
|---|---|---|---|
| `bin\server\CoreServer.dll.moi` | **`7b3423c2`** | 18.277.888 | **CẦN** — tắt GameServer → `ChayGameServer.bat` |
| `bin\client\CoreClient.dll` | `5b56367c` (S13e, đang chạy) | | **không đổi** (bản build cùng đợt `ff6128da` chỉ khác dấu thời gian) |

**Nghiệm thu S13h** (script `logsnap3\`): trễ máy chủ chấp nhận lệnh chạy p50 về ≤ 200 ms, > 0,5 s ≤ 15 %; `[S8-NAN]` < 0,5/phút;
đảo chiều < 0,5/phút; `[S2-MELEE-TOOFAR-RUN]` phía server xuất hiện rồi **tắt** trong cùng chu kỳ (máy chủ tới tầm); dash 1:1;
trúng/chiêu ~2,0; `[S12-THEO]` (nếu có) đích phải là phía quái. **Hồi quy phải soát**: nhân vật "tự nhích sát quái" khi đang
đứng đánh (đích đuổi là vị trí quái, khoảng cách 0); máy chủ đi thẳng (`ServeMove`, không A*) — gặp tường thì đứng, chiêu kế
thử lại (trước S13h máy chủ cũng đứng chỗ đó, không tệ hơn).

## 10.9 SAU SWAP S13h (15:26) — chủ báo "vẫn còn giựt lùi" + "phù về đi tới rồi kéo lùi" → đo → S13i

Bản chạy khi báo: server `7b3423c2` (S13h, GameServer 15:26:55), client `5b56367c`. Log `logsnap4\` (cl5/sv5, 17,6 phút client).

### Số đo phiên S13h (so với S13e)

| chỉ số | S13e | **S13h** |
|---|---|---|
| `[S8-NAN]`/phút | 3,1 | **1,08** (19 cú, lệch 257–348, 15/19 lúc `run`) |
| đảo chiều > 120°/phút | 2,8 | **0,3** |
| `[S13-KEO]` | 86 (stand) | 43 (stand) |
| máy chủ tự đuổi `[S2-MELEE-TOOFAR-RUN]` | — | **3 lần/24 phút** ⇒ S13h ít khi kích hoạt; cải thiện phần lớn do bớt "giữ chiêu" |

### Ba gốc mới (đều đo trên log, mỗi gốc có ca cụ thể)

**G1 — máy chủ bắn vào XÁC (6/19 cú).** 4 cú đầu cùng một kịch bản: client `S2-CANCAST-DENY` + `E4_SKILL_ABORT people=0` (mục
tiêu đã chết) rồi chạy sang quái khác ngay; máy chủ thi hành chiêu 1985 đang giữ vào `tgt … doing=10 life=0` (`S4-CAST`), hoạt ảnh
~560 ms, rồi mới nhận lệnh chạy (trễ 560–830 ms) ⇒ tụt 270–330 mps ⇒ nắn lùi. Toàn phiên: **121/911 chiêu người chơi (13 %,
5 lần/phút) vào mục tiêu chết**. Mã: `KNpcSet::GetRelation` bản client (`:1531`) trả `relation_none` khi `!IsAlive()`; nhánh máy chủ
(`:1698+`) **không kiểm** ⇒ `CanCastSkill` cho qua.

**G2 — máy chủ MẤT MỘT KHUNG mỗi lần đổi vùng (lệch trôi ~14 mps/s).** Đo đúng cách trên 25 đoạn chạy liên tục ≥ 2,5 s: lệch tăng
p50 **14 mps/s** (p75 19) dù bước máy chủ khi cưỡi ngựa vẫn đúng 26 = client (bác giả thuyết tốc độ). Xét 1160 khoảng 2 khung (cả hai
khung đều là cặp gói sync): máy chủ **thiếu bước 44 lần, 39/44 nằm trong ±400 ms của `[S6-LOADMAP]`** (client đổi cửa sổ vùng =
người chơi vừa qua ranh vùng). Mã: `KSubWorld::NpcChangeRegion` máy chủ (`:2351`) `SetActiveFlag(TRUE)`; `KNpc::Activate` (`:645`)
gặp cờ thì xoá + return; client xoá cờ đầu **mỗi** khung (`KSubWorld::Activate :1103`, `#ifndef _SERVER`), máy chủ **không** ⇒ NPC
sang vùng chỉ số **nhỏ hơn** (đã duyệt) giữ cờ sang khung sau ⇒ mất một khung. Lỗi engine chung cho **mọi NPC máy chủ**.
⚠️ Bẫy đo đã gặp: gói sync chính chủ tới client theo **cặp** (2 gói cùng khung client, vị trí máy chủ cách 1 bước) ⇒ đo "2 khung
client = 1 bước máy chủ" cho ảo giác máy chủ nửa tốc độ; phải đo trên đoạn dài hoặc chỉ so khung-cặp với khung-cặp.

**G3 — phù về: client vẫn chạy tới đích của MAP CŨ.** Phiên S13e (`logsnap3`) ca `t=751604180`: map 227 → 11, `vaolandau` +0 ms,
**+404 ms `E4_MOVE_PATH npc=1 des=(43493,104034)`** (đích map cũ, `doing=3`), +692 ms `S8-NAN lech=257` kéo về = đúng "đi tới rồi kéo
lùi". `LoadMap` mới chỉ `StopPath()` + xoá khe lệnh, **không đổi `m_Doing`/`m_DesX/Y`** ⇒ `OnRun/ServeMove` đi tiếp. Phiên S13h chỉ
có 1 lần phù về (đang đứng) nên không tái hiện; 10 lần trong phiên S13e thì nhiều lần bị.

Bổ sung từ wauto-c0: WAuto lên/xuống ngựa ~2,5 s/lần giữa lúc đánh (DT_WalkTo→DT_Ride vs máy PK/đánh) — không phải gốc giựt lùi
(bước máy chủ khi cưỡi ngựa vẫn 26) nhưng cắt chiêu/đổi tốc; wauto-c0 sẽ vá bên WAuto sau khi chủ duyệt.

### S13i/S13j — ba chỗ vá (build từ main `a2fa66ca` = có MAIL đợt 2 của wauto-d9)

| | tệp | vá |
|---|---|---|
| A (server) | `KNpc.cpp` DoSkill, trước `switch(CanCastSkill)` | người thật, `nX==-1`, `!Npc[nY].IsAlive()` ⇒ log `[S13-XAC]`, `return` (không `DoStand`, giữ đang chạy) |
| B (server) | `KSubWorld.cpp` `Activate` + `NpcChangeRegion` | **O(1)**: biến tĩnh `s_nS13iVungDangDuyet` = chỉ số vùng đang duyệt (−1 ngoài vòng, 0x7FFFFFFF khi duyệt danh sách VOID); `NpcChangeRegion` chỉ `SetActiveFlag(TRUE)` khi đích còn được duyệt trong cùng khung (đích ≥ vùng đang duyệt, hoặc đích = VOID). Bản đầu (S13i) dùng `ClearActivateFlagOfAllNpc()` mỗi khung ở `MainLoop`, phản biện chỉ ra chi phí O(kho NPC toàn 909 map) mỗi khung → đổi sang O(1) |
| C (client) | `KProtocolProcess.cpp` SyncNpcMinPlayer nhánh `vaolandau` + nhánh region chưa nạp | đang `run/walk/runattack` ⇒ `SendCommand(do_stand)` (`DoStand` private, thi hành ở `ProcCommand` khung kế); lệnh đặt-đi của script tới sau vẫn vào khe di chuyển và thắng. Phản biện tìm 2 lỗi thật → **S13j**: (i) đang **lướt** (`do_runattack` tắt AI) thì nhánh AI-tắt vứt `do_stand` → `SetProcessAI(TRUE)` trước khi gửi; (ii) nhánh **cùng map** `LoadMap` không `StopPath` nên bộ theo-đường gửi lại `do_run` đè lệnh dừng → thêm `SubWorld[0].StopPath()` |

| tệp | md5 | kích thước | swap |
|---|---|---|---|
| `bin\server\CoreServer.dll.moi` | **`f29e592e`** | 18.281.472 | tắt GameServer → `ChayGameServer.bat` |
| `bin\client\CoreClient.dll.moi` | **`a4bcb831`** | 2.507.264 | thoát Game + WAuto → `ChoiGame.bat` (đổi luôn `Game.exe.moi c3daea6d` MAIL của wauto-d9) |

**Nghiệm thu S13i**: `[S13-XAC]` xuất hiện ≈ 5 lần/phút và `S4-CAST` vào `life=0` = 0; lệch trôi trên đoạn chạy dài p50 ≈ 0 mps/s
(±5); `[S8-NAN]` < 0,3/phút; phù về lúc đang chạy: không `E4_MOVE_PATH` đích map cũ, không `S8-NAN` trong 3 s sau `vaolandau`.
Hồi quy soát: bot/quái đổi vùng mượt hơn (không xấu đi); người thật cast lên xác (hồi sinh/bốc xác?) — nếu có tính năng đó thì `[S13-XAC]`
sẽ chặn, cần chủ báo.

## 10.10 SAU SWAP S13i/j (16:47) — đo 12,5 phút → xấu hơn (4,47 nắn/phút) → tìm ra gốc "1967 cắt đường chạy" → S13k

Bản chạy: server `bad8e293`, client `f2ad5ca3` (rồi client `6bbcda8f`/`e151cbfc` của wauto-d9, không đổi mã S13). Log `logsnap5\` (cl6/sv6).
Bối cảnh khác hẳn phiên S13h: map 340, đi bộ (82 % thời gian), quái dày: **đổi mục tiêu 22 lần/phút** (S13h: 1), 54 chiêu/phút (30).

| chỉ số | S13h | S13i/j |
|---|---|---|
| `[S8-NAN]`/phút | 1,08 | **4,47** (56 cú; 19 lúc client đứng, 14 máy chủ ở TRƯỚC, 12 ở SAU) |
| nắn / 100 chiêu | 3,6 | 8,3 |
| `[S13-XAC]` | — | 248 (10,5/phút), `S4-CAST` vào `life=0` = **0** (G1 vá đúng) |
| trễ nhận lệnh chạy client→server | — | **p50 0 ms**, p90 441 (S13e: p50 328) |
| **trễ pha chiêu** client thi hành → server thi hành cùng chiêu | p50 67 ms, p90 980 | **p50 180, p75 1.075, p90 1.788 ms; 45 % > 0,5 s** |

### Loại trừ (đo trên số, không đoán)
- **Không phải tốc độ**: cặp bước (client, máy chủ)/2 khung = (42,42) 452 lần đi bộ, (52,52) 81 lần ngựa.
- **Không phải XAC `return` không `DoStand`** (giả thuyết máy chủ chạy tiếp vượt client): 20 lần XAC lúc máy chủ chạy, lệch 1 s sau p50 −2 mps, chỉ 1/20 có nắn trong 3 s (nền 7 %).
- **G2 (mất khung đổi vùng) bằng chứng số yếu**: cửa sổ "mất bước" nằm gần hết **cạnh `[S6-LOADMAP]`** ở cả hai phiên (118/776 trước, 49/317 sau vá) ⇒ phần lớn là **ảo ảnh đo** do đổi khe vùng của cửa sổ 3×3 client; cơ chế mã vẫn đúng (phản biện xác nhận), giữ vá O(1), nhưng không còn coi là gốc chính.

### 🔴 Gốc mới: chiêu bị TỪ CHỐI cắt đường chạy của máy chủ
Máy chủ từ chối chiêu (case 0) **282 lần/12,5 phút, 100 % là chiêu 1967** — WAuto gửi 1967 (chiêu tự thân) nhắm quái xen kẽ với 1985/1977, cả hai phía đều từ chối (quan hệ mục tiêu). Mỗi từ chối đi qua `Exit → DoStand`. Client nhận 1967 khi **đang đứng** trong tầm (vô hại; lúc đang chạy thì WAuto cấp lại lệnh ngay khung sau). Máy chủ nhận 1967 khi **đang chạy phía sau** (xuất phát sau client) ⇒ `DoStand` **cắt đường chạy**, đứng ngoài tầm chờ lệnh chạy kế **p50 3,0 s** (client đứng trong tầm nên không gửi gì) ⇒ lệch đóng băng ở 200–300 ⇒ cú chạy kế vượt 256 ⇒ nắn. Đo: **76 lần/12,5 phút** máy chủ hủy 1967 lúc `doing=3`; 196 cú nhảy trễ pha > 300 ms, bối cảnh phổ biến nhất `cmd3 → CANCAST-DENY → SKILL_ABORT → CAST muộn`.
Trễ pha vì thế "dính": client đánh liên tục nên máy chủ luôn bận, không bao giờ tự rút; phút 0–5 chỉ 15 ms, từ phút 6 nhảy 500–1.100 ms.

### S13k (`KNpc.cpp` DoSkill, tại nhãn `Exit:`, CẢ HAI PHÍA)
Người chơi thật đang `do_run/do_walk` mà chiêu bị huỷ vì bất kỳ lý do nào đi tới `Exit` (từ chối `CanCastSkill`, hồi chiêu chưa xong, thiếu
mana/thể lực, cấm đánh, thiếu tầng Nộ, cast toạ độ quá tầm) ⇒ bỏ chiêu, **giữ nguyên di chuyển** (`return`, log `[S13-DENY-GIUCHAY]`), không
`DoStand`. Phản biện S13k: bản đầu chỉ gác `case 0` (hụt 5 đường khác) và chỉ máy chủ (máy chủ sẽ dẫn trước client 15–125 mps mỗi lần vì client
vẫn bị cắt rồi WAuto cấp lại mất 1–5 khung) ⇒ chuyển gác lên `Exit:` và bỏ `#ifdef _SERVER`. Client sẽ nhận phần này ở đợt build CoreClient kế
của wauto-c0 (từ main ≥ commit S13k); tới lúc đó lệch dấu ngược nhỏ, S13-KEO khép khi đứng.
Gốc xa thuộc WAuto (đã báo wauto-c0): 1967 phải cast lên chính mình hoặc bỏ khỏi vòng bắn mục tiêu — hết 23 lệnh vô ích/phút.

| tệp | md5 | kích thước | swap |
|---|---|---|---|
| `bin\server\CoreServer.dll.moi` | **`b68899b2`** | 18.298.368 | tắt GameServer → `ChayGameServer.bat` (build từ main `87757f14`, giữ mail đợt 3) |
| `bin\client\CoreClient.dll.moi` | **`e10abd7a`** | 2.515.968 | superset của `e151cbfc` (D5 wauto-d9) + client giữ chạy; đi cùng `Game.exe.moi bd5cb88e` (giữ nguyên); thoát Game + WAuto → `ChoiGame.bat` |

**Nghiệm thu S13k**: `[S13-DENY-GIUCHAY]` xuất hiện; trễ pha chiêu p75 < 400 ms; `[S8-NAN]` < 1/phút trong bối cảnh quái dày; khoảng "máy chủ đứng ngoài tầm" sau từ chối biến mất.
