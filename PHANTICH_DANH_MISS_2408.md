# "ĐÁNH HAY MISS" — TÌM RA GỐC & ĐÃ VÁ (24/08/2026)

> Chủ game: *"hay đánh miss với các phái tiếp cận, không phải các phái đánh đạn skill bay ra"*.
> Kết luận: **đúng 1 lỗi 2 dòng** trong `KMissle::CheckNearestCollision()` — trừ toạ độ ô **cục bộ của
> HAI region khác nhau**. Đã vá ở commit `a0dd86c2`. DLL máy chủ `26f43102` đã đặt vào `bin\server`, **chờ RESTART**.

---

## 1. Số liệu gốc (log 24/08, lọc tên `CaiBang`, ~3 giờ)

Nguồn: `bin\client\jx_auto.log` (679.992 dòng) + `bin\server\jx_auto_server.log` (340.183 dòng).
Bản chụp đã dùng để phân tích nằm trong scratchpad của phiên; hai tệp gốc vẫn đang được ghi tiếp.

| Chặng | Số |
|---|---|
| `[S4-CAST]` — máy chủ **thực sự** thi hành chiêu | 2.840 (2.832 chiêu 361 vào quái `kind=0`) |
| Nhịp khi đang giao tranh | **1,80 đòn/giây** (bình thường) |
| `[S4-MSL-END]` — viên đạn kết thúc | 2.783 |
| **Hụt hoàn toàn** (`status=1 lasthit=0 barrier=0`, sống hết 4/4 khung) | **831 = 29,9%** |
| `[S1-WHO]` — đòn đã chạm | 3.429, **tất cả** `melee=1 usear=1 missrate=0 dohurt=100`, `S1-ARDATA autohit=1` |

⇒ **Khâu xúc xắc trúng/trượt hoàn toàn không liên quan.** Đòn nào chạm là chắc chắn trừ máu.
Vấn đề là **viên đạn không chạm**.

### Miss bám theo MỤC TIÊU, không rải đều

| Mục tiêu | Số đạn | % hụt |
|---|---|---|
| id 54158 | 614 | **88,6%** |
| id 54392 | 59 | **88,1%** |
| id 54226 | 271 | 62,4% |
| 78 mục tiêu khác (≥5 đạn) | 1.564 | **0–5%** |

Phân loại theo **hình học lúc bắn** (ghép 1-1 `[S4-CAST]` ↔ `[S4-MSL-END]`, 2.783 cặp, 0 mồ côi):

| Nhóm | Số đạn | Hụt |
|---|---|---|
| ô người bắn **nằm trên biên region** + mục tiêu cùng ô | 919 | **799 = 86,9%** |
| ô người bắn **KHÔNG** trên biên + mục tiêu cùng ô | 496 | 14 = **2,8%** |
| trên biên + khác ô | 232 | 15 = 6,5% |
| không trên biên + khác ô | 1.136 | 3 = 0,3% |

**799/831 = 96,1%** số đạn hụt nằm trong ô "trên biên". Trừ nền 2,8% → **~93%** là do lỗi dưới đây.

---

## 2. Gốc — `Sources/Core/Src/KMissle.cpp:1825-1826`

```cpp
for (int i = -1; i <= 1; i ++)
  for (int j = -1; j <= 1; j ++)
  {
      GetOffsetAxis(m_nSubWorldId, m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, i, j,
                    nSearchRegion, nRMx, nRMy);            // KMissle.cpp:1418
      nNpcIdx = SubWorld[..].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, ...);  // KRegion.h:180
      if (nNpcIdx > 0)
      {
          nDX = m_nCurrentMapX - Npc[nNpcIdx].m_MapX;      // ← SAI
          nDY = m_nCurrentMapY - Npc[nNpcIdx].m_MapY;      // ← SAI
```

- `GetOffsetAxis` đặt `nDesMapX = nSrcMapX + i` **rồi mới cuộn vòng** sang region kế
  (`nDesMapX += m_nRegionWidth` và đổi `m_nConnectRegion[2]/[6]`, tương tự trục Y với `[4]/[0]`).
- `FindNpc` chỉ trả NPC có `m_MapX == nRMx && m_MapY == nRMy` — tức toạ độ **cục bộ trong region đó**.
- Nên khi ô kế nằm ở **region khác**, phép trừ ra **±15** (region rộng 16 ô) hoặc **±31** (cao 32 ô)
  thay vì ±1. Hai nhánh so offset bên dưới đối xứng tuyệt đối:
  - `nDX < 0` loại khi `nCellWidth - m_nXOffset + nNpcOffsetX > nCellWidth`
  - `nDX > 0` loại khi `nCellWidth - nNpcOffsetX + m_nXOffset > nCellWidth`

  ⇒ **dấu bị lật ⇒ điều kiện loại bị đảo ⇒ `bCollision = FALSE`** ⇒ đạn bay xuyên qua quái.

### Kiểm lại bằng số trên một vết thật

`[MSL-NEARMISS] msl=7 sk=361 npc=53159(id=54158) d_cell(15,0) msloff(11264,16128) npcoff(1024,13312) cell(32768,32768) bCollision=0`

| | phép tính | kết quả |
|---|---|---|
| `nDX` sai = **+15** | `32768 - 1024 + 11264 = 43008 > 32768` | **loại → HỤT** |
| `nDX` đúng = **−1** | `32768 - 11264 + 1024 = 22528 ≤ 32768` | **TRÚNG** |

Cả **544 viên** hụt vào con này đáng lẽ đều trúng. `id 54158` đứng ở **cột 0** của region 129 (mép trái);
`id 54226` ở **cột 15** (mép phải); `id 54392` ở **hàng 31** (mép dưới, phủ trục Y).
Đối chứng `id 54137` (0% hụt): **không một dòng nào** qua biên, toàn `|d| ≤ 1`.

---

## 3. Vì sao **chỉ phái cận chiến** dính

`KMissle::CheckCollision()` (KMissle.cpp:628) rẽ hai nhánh theo `m_nCollideRange`:

| Nhánh | Đường đi | Dính lỗi? |
|---|---|---|
| `m_nCollideRange == 1` | `CheckNearestCollision()` — **có** phép trừ ô ở trên | **CÓ** |
| `m_nCollideRange > 1` | vòng `GetOffsetAxis` + `FindNpc` + `ProcessCollision()` — **không hề** tính hiệu ô | Không |

Đối chiếu `settings\missles.txt` (441 viên đạn, 238 = 54,0% có `CollidRange == 1`):

| Loại | Viên đạn | Tham số | Đường đi |
|---|---|---|---|
| Đòn thường vũ khí cận chiến | 64 | `MoveKind=1 ColRange=1 Life=6 Speed=20` | **có lỗi** |
| Đòn thường cung | 65 | `MoveKind=1 ColRange=1 Life=20 Speed=16` | có lỗi (nhưng ít lộ, xem dưới) |
| Chiêu cận chiến (361, 327, 408, 418-428…) | — | `MoveKind=1 ColRange=1 Life=3-4 Speed=24-30` | **có lỗi** |
| Chiêu diện rộng / bắn (1177, 202, 211, 281, 203, 373, 382, 383…) | — | `ColRange=2..14`, phần lớn `MoveKind=0` | **an toàn** |

Trong toàn bộ log, **không một chiêu diện rộng nào từng xuất hiện ở `[MSL-NEARMISS]`** — đúng như chủ game mô tả.

Vì sao cung ít lộ hơn: đạn cung `Speed=16` (0,5 ô/khung) và `Life=20` nên **luôn có khung được lấy mẫu ngay
trong ô của mục tiêu** (`i=j=0`, `nDX=0` → không dính lỗi). Đạn cận chiến `Speed=30` (0,94 ô/khung),
`Life=4`, và `OnFly` **di chuyển trước rồi mới kiểm va chạm** (KMissle.cpp:978) — nên mục tiêu đứng sát
thường **không bao giờ** được lấy mẫu ở `i=j=0`, toàn bộ trông cậy vào ô kế ⇒ đúng chỗ lỗi.
Đo được: `dist ≤ 15 mps` → **56,0%** phát hụt; `dist 16-45` → 1,8%; `dist ≥ 46` → 0,4%.
**98,6% số viên hụt là bắn sát mặt.**

---

## 4. Bản vá đã áp (commit `a0dd86c2`)

```cpp
nDX = -i;	// GetOffsetAxis dat o dich = o dan + (i,j) ROI MOI cuon vong sang region ke,
nDY = -j;	// nen hieu 2 toa do CUC BO ra +-15 / +-31 khi qua bien -> lat nguoc phep so offset.
```

**Vì sao an toàn:** trong cùng region `nRMx = m_nCurrentMapX + i`, nên biểu thức cũ **vốn đã bằng `-i`**
→ bản vá là **no-op** cho mọi trường hợp không qua biên. Chỉ đổi hành vi khi `nSearchRegion != m_nRegionId`.
Không thêm biến, không đổi chữ ký, **không đụng giao thức / gói tin / cấu trúc dữ liệu / bảng số**.

Đã áp cùng 2 dòng sang cây vận hành
`E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\SOURCESUPDATE_KINHMACH_ONLTEST0608\Sources\Core\Src\KMissle.cpp:1760-1761`
(áp riêng từng dòng bằng `safe_edit.py`, **không** chép đè cả hàm — hai cây đã lệch).

### Trạng thái binary

| | |
|---|---|
| `CoreServer.dll` x64 md5 **`26f43102`** | đã đặt vào `E:\...\TESTLOFFF_ONLINE\bin\server\`, backup `CoreServer.dll.cu_2408_truoc_fix_missle_0edd1e40` (= bản `0edd1e40` đang chạy) — **CHỜ RESTART GameServer** |
| `CoreClient.dll` Win32 md5 `16e2a3a7` | đã build ở `Sources\Core\ClientRelease\` nhưng **CHƯA thay** — bản đang chạy là 22/08, thay sẽ kéo theo cả đợt PORT5 chưa test phía client. **Không cần** để hết miss: sát thương do máy chủ quyết định 100%; client chỉ vẽ hiệu ứng. |

### ⚠️ Phải báo trước cho người test

Lỗi này **lật ngược** nhánh chứ không **tắt** nhánh, nên nó vừa tạo hụt oan vừa tạo **trúng oan**.
Đo trên 818 dòng `[MSL-NEARMISS]`: **147 lần đang bị từ chối oan** (sẽ thành TRÚNG) và
**9 lần đang được chấp nhận oan** (sẽ thành TRƯỢT).
→ Sau khi vá, một số đòn hiện đang "ăn" ở sát biên region sẽ **ngừng ăn**. Đó là **hành vi ĐÚNG**, không phải hồi quy.
→ **Quái bắn người chơi cũng hết trượt** ⇒ sát thương **nhận vào** cũng tăng. Phải đo lại độ khó bãi luyện + Tống Kim.

---

## 5. Cách đo lại sau khi restart (công thức DUY NHẤT được chấp nhận)

```
ty le hut = so dong [S4-MSL-END] co lasthit=0 va barrier=0  /  tong so dong [S4-MSL-END]
```

1. `bin\server\config.ini` → `[AutoLog] On=1`, **`Name=<tên nhân vật>`** (máy chủ chạy ~1000 bot, bot cũng `IsPlayer()`).
2. **Xoá `jx_auto_server.log` + `jx_auto.log` trước khi thử.**
3. `grep -c "bo qua .* dong"` phải = 0 (trần cứng 1200 dòng/giây, `KCore.cpp:791`).
4. Bài A/B cụ thể: đánh **id 54158 từ ô (0,22)** (trước vá: 543 phát trúng ~0) và **id 54392 từ ô (8,31)** (phủ trục Y).
   Sau vá phải ~100% trúng. Kỳ vọng tỷ lệ hụt chung **29,9% → 2-3%**.

---

## 6. Các lỗi THẬT khác đã tìm ra nhưng **KHÔNG** gây hụt đạn

> ✅ **ĐÃ VÁ HẾT ở đợt 2 — commit `faeee99d`. Xem mục 10 để biết bằng chứng đo lại, 5 lỗi phụ
> phát sinh, và 1 thay đổi ĐỔI CÂN BẰNG PvP cần chủ game duyệt.**

Xếp theo giá trị. Không được cộng số này vào 831.

| # | Lỗi | Tệp:dòng | Hậu quả |
|---|---|---|---|
| A | `CheckNearestCollision` trả NPC **đầu tiên theo thứ tự quét 3×3**, không ưu tiên `m_nFollowNpcIdx`, **không loại xác chết** | `KMissle.cpp:1820` | **405/2783 (14,6%)** viên chỉ trúng con khác, mục tiêu không ăn gì; **42,1%** các cú "cướp" là trúng **XÁC** (`doing=10 life=0`) — xác nằm im một ô, cướp mọi phát bắn tới khi despawn. Gộp với hụt: mục tiêu không ăn sát thương = **44,4%**. |
| B | `ProcessCollision` gặp NPC **bất tử** thì `return 0` (thoát **cả** vòng quét) thay vì `continue` | `KMissle.cpp:1530` | 0% trong log này (quái luôn `protect=0`) nhưng **sẽ nổ ở Tống Kim / Lôi Đài / PK**: một mục tiêu bất tử chặn hết sát thương diện rộng của cả viên đạn. |
| C | `KRegion::AddRef/DecRef` nhánh `obj_missle` trỏ **nhầm** vào `m_pObjRef` (bảng VẬT PHẨM), và **thiếu `DecRef`** khi đạn chết | `KRegion.cpp:1110`, `:1156` | Mỗi viên đạn để lại **+1 vĩnh viễn** trên ô nó chết trong bảng vật phẩm. Đo thật: ô (r128, 12,22) ăn **592 lần** trong 3 giờ của **một** nhân vật → chạm trần byte 255 → `CanPutObj` trả FALSE **vĩnh viễn** tại ô đó. Đây là gốc của "**đồ rơi lệch ô / bỏ sót đồ**". **Phải làm đủ cả hai phần**, làm nửa vời là đổi lỗi nhẹ lấy lỗi nặng. Rác hiện có **tự sạch khi restart**. |
| D | `KRegion::FindNpc` đọc `m_pNpcRef` **không chặn cận trên** | `KRegion.h:181` | đọc tràn tối đa 208 byte ngoài mảng 512 byte; chỉ kích hoạt với NPC `vision ≥ 544` (15/1262 con). Đường va chạm của đạn không bao giờ tràn. |
| E | `ProcCommand` chạy **trước** `ProcStatus` và xoá lệnh vô điều kiện | `KNpc.cpp:971` | mất đúng **1 khung (~55 ms)** ở ~43% số đòn ⇒ **−4,2% thông lượng** (104,4 → 109,0 đòn/phút). Không mất DPS theo nghĩa "hụt". |
| F | `KSubWorld::SetObstacle` gọi nhầm `SetTrap` ở ô tâm | `KSubWorld.cpp:3242` | **mã chết** — không script `.lua` nào gọi `AddObstacle`. Vá khi rảnh. |

**Thứ tự đề nghị:** vá lõi (xong) → đo lại → nếu ổn 1-2 ngày thì làm **A** + **B** (test kỹ Tống Kim) →
rồi **C** (a+b+c cùng lúc) + **D** → cuối cùng **E**, **F**.

---

## 7. ⛔ Các bản vá đã bị PHẢN BIỆN BÁC BỎ — cấm làm lại

| Đề xuất | Vì sao bác |
|---|---|
| Sửa 4 công thức toạ độ trong `KSubWorld::SetTrap`/`SetObstacle` | Mọi lời gọi (`ScriptFuns.cpp:3740/3779/4041`) đều hardcode `nRange = 0 hoặc 1`; tại đúng các giá trị đó cả 4 công thức **trùng khít** công thức đúng. Lệch thật = **0 ô**. |
| Thêm `abs()` vào `KRegion::BroadCast` | Vét cạn 8 hướng: **0/2.097.152** cặp bị cắt oan. Region rộng 16 ô ⇒ `\|nDX\| ≤ 31 < 32`, nhánh đó là **mã chết**. Thêm `abs()` mà giữ ngưỡng 32 sẽ làm **NPC cách 33-40 ô phía trên biến mất**. |
| Đổi `>` thành `>=` khi chuẩn hoá offset (`KMissle.cpp:1083`, `KNpc.cpp:4652/4663/4766/4777`) | `Map2Mps` tuyến tính nên `(ô N, off 32768)` ≡ `(ô N+1, off 0)`. Kết quả trúng/hụt **y hệt**. Áp vào còn lật 3 lần từ chối **đúng** thành trúng ảo. |
| Nới `KSkills.cpp:350` từ `radius+20` lên `radius+120` (chữa 486 lần `[S2-MELEE-TOOFAR-RUN]`) | 486 dòng chỉ là **82-100 đợt** đuổi, không phải 486 đòn mất. Khoảng cách server đo là **THẬT** (trung vị 140, radius 90). Nới sẽ xoá luôn bước đuổi và cho vung chiêu vào không khí. Giá trị thật chỉ **0,9-4,2%** thông lượng. |
| Nắn vị trí nhân vật trong `SyncNpcMinPlayer` | Việc không nắn là **CÓ Ý** (client-side prediction). Độ lệch là trễ mạng một chiều, bão hoà ~128 mps, về 0 khi đứng yên. Snap vào = **giật rubber-band 4 ô mỗi lần sync**. |
| Bỏ cổng `m_nNeedFixPos > 0` để nắn vị trí quái | Đọc nhầm ngữ nghĩa: `m_nNeedFixPos` là **bộ đếm bị vật cản chặn** (`KNpc.cpp:4603`). `fix=0` ở 99,6% = quái đang đi **bình thường**. |
| "Client câm lặng, không vẽ hiệu ứng chạm" | Bị log client bác thẳng: 579 dòng `[MSL-HIT-CELL]`, 332 dòng `[MSL-COLLIDE]` cho chiêu 361 của chính người chơi. |
| `OnFly` kiểm va chạm ngay tại vị trí bắn (biến thể "1b") | Trên tập log này **thừa** với bản vá lõi (mô phỏng: vá riêng mỗi bên đều cứu 830/831). Rủi ro cao hơn hẳn: `return` sớm làm đạn **mất một bước bay** ⇒ tầm thực tế ngắn đi 30 mps cho **mọi** chiêu, mọi môn phái. Chỉ làm nếu đo xong vẫn còn hụt. |

---

## 8. Bẫy đọc log (ghi lại để phiên sau khỏi mắc)

1. **`AUTOLOG_EVERY(ms, …)`** (`KCore.h:215`) — tiết chế theo thời gian, **KHÔNG lọc tên nhân vật**.
   `[MSL-NEARMISS]` = 1 dòng/2000 ms cho đạn của **toàn bộ ~1000 bot**.
   **Tuyệt đối không dùng số đếm nhãn này làm tỷ lệ** — chỉ dùng được theo kiểu **số học từng dòng**
   (mỗi dòng tự chứa đủ `msloff`/`npcoff`/`bCollision` để kiểm lại công thức; khớp 818/818).
2. **Nhãn không tiết chế + có lọc tên** (dùng làm mẫu số được): `[S4-CAST]`, `[S4-MSL-HIT]`, `[S4-MSL-END]`,
   `[S3-PKT-IN]`, `[S3-CMD-SWALLOW]`, `[S3-PROC-FINDSAME]`, `[S2-MELEE-TOOFAR-RUN]`, `NET-SKILL-PKT`, `[FIGHT-NODMG]`.
   **`[S2-NETSKILL-IN]` KHÔNG phải số gói** — nó là `AUTOLOG_IDX_EVERY(…, 300, …)` (`KProtocolProcess.cpp:5059`).
3. **`S3-PKT-IN 21069` vs `NET-SKILL-PKT 9042` KHÔNG phải mất gói.** Đó là **lệch cửa sổ thời gian**:
   server pid 11856 kết thúc trước khi `client.log` bắt đầu. Lọc đúng cửa sổ: 9028 vs 9042 = **99,85%**, ghép 1-1, 0 gói mồ côi.
   **Luôn lọc theo cửa sổ giao nhau của hai tệp trước khi lấy bất kỳ tỷ lệ nào** (trong đợt này tỷ lệ hụt là
   24,3% trong cửa sổ so với 34,0% ngoài cửa sổ).
4. **`pos(r=,cx,cy)` trong `[S4-MSL-END]` là vị trí lúc đạn HẾT TUỔI THỌ**, không phải lúc chạy phép thử va chạm.
   Đạn sinh ra trên mép rồi bay ra xa 3-4 ô mới tắt — **đừng dùng nó để kiểm định giả thuyết biên region**.
5. **`msl` id BỊ TÁI SỬ DỤNG** (pool 20000). Ghép `S4-MSL-HIT` vào `S4-MSL-END` phải theo **thứ tự thời gian**,
   không được gom theo id — gom theo id cho ra "785 viên trúng/2783" (sai hoàn toàn).
6. **Enum `NPCCMD` (`KNpc.h:48+`)**: `do_none=0, do_stand=1, do_walk=2, do_run=3, do_magic=6, do_attack=7, do_hurt=9`.
   **`do_stand=1` là ĐỨNG YÊN**, không phải "đang đánh".

---

## 9. Phương pháp

Điều tra bằng 56 tác nhân: 5 góc nhìn độc lập (phản bác giả thuyết gốc / tìm nguyên nhân thứ hai /
quét mã tìm cùng kiểu lỗi / soi đường lệnh auto→client→server / góc nhìn client), mỗi phát hiện qua
**2 lớp phản biện độc lập** (một lớp kiểm cơ chế trong mã, một lớp tự đếm lại trên log bằng Python).
7 bản vá bị bác bỏ ở mục 7 đều là phát hiện **đã chết trong vòng phản biện** — giữ lại để khỏi làm lại.

---

## 10. ĐỢT 2 — ĐÃ VÁ 6 LỖI CÒN LẠI + 5 LỖI PHỤ (commit `faeee99d`, 24/08 tối)

Chủ game: *"Fix tiếp 6 lỗi"* + ***"phải xác định chính xác lỗi mới fix"***.
Mọi con số dưới đây **đo lại từ đầu** trên log 24/08 (2.783 viên đạn của CaiBang), không lấy số của ai khác.

### 10.1 Bằng chứng từng lỗi

| Mã | Lỗi | Bằng chứng ĐO ĐƯỢC |
|---|---|---|
| **A1** | `CheckNearestCollision` trả về XÁC; `ReceiveDamage` (`KNpc.cpp:3964`) `return TRUE` không trừ máu, còn `CheckCollision` đã `return 1` nên đạn tắt luôn | **537/3419 = 15,7%** lần chạm rơi vào NPC `doing=10` hoặc `life≤0`; **167 viên (6,0%)** chỉ chạm xác |
| **A2** | Trả về con **đầu tiên theo thứ tự quét 3×3 cố định**, không ưu tiên `m_nFollowNpcIdx` | **405/1952 = 20,7%** viên có chạm nhưng **không hề chạm `wantid`** (40,2% số đó là chạm xác). Cộng 831 viên hụt ⇒ **mục tiêu không ăn gì = 1236/2783 = 44,4%** |
| **B** | `ProcessCollision`: `return 0` thoát **cả hai vòng** khi gặp NPC bất tử, trong khi cửa chặn là **theo từng NPC** | Không có ca thật trong log (790/790 `COLL-NPC-FOUND` đều `protect=0`). **Chứng minh là LỖI chứ không phải cơ chế:** những con quét TRƯỚC đó **vẫn ăn** sát thương (`ProcessDamage` đã chạy) ⇒ kết quả phụ thuộc **thứ tự quét** |
| **C** | Bộ đếm đạn ghi `m_pObjRef` (bảng VẬT PHẨM) trong khi `GetRef(obj_missle)` (`KRegion.cpp:1082`) đọc `m_pMslRef`; 5 `AddRef` / 4 `DecRef`; `AddObj` `>=0` vs `RemoveObj` `>0` | Ô **(region 128, cell 12,22) ăn 592 lần** trong 3 giờ của **một** nhân vật ⇒ **vượt trần byte 255** ⇒ `CanPutObj` FALSE vĩnh viễn |
| **D** | `FindNpc` thiếu chặn cận trên | Người gọi không chỉ `KMissle`: `KNpcAI.cpp:959/1059`, `KPlayer.cpp:11604/11649/11694/11739` chỉ cuộn **một** bước region ⇒ NPC tầm nhìn > 512 mps cho `nRMx ≥ 16` ⇒ đọc quá mảng 512 byte. **Kết quả trả về không đổi** (vòng quét không thể khớp toạ độ ngoài dải) ⇒ thuần an toàn bộ nhớ |
| **E** | `ProcCommand` chạy **trước** `ProcStatus` (`KNpc.cpp:598-599`) và xoá lệnh vô điều kiện | Histogram khoảng cách 2 lần cast chiêu 361 tách **đúng hai cụm**: `430-459 ms` (**1176** mẫu = 8 khung) và `480-519 ms` (**1204** mẫu = 9 khung) — cách nhau **55,5 ms = 1 khung @ GAME_FPS 18**. **50,6%** số đòn ở cụm chậm ⇒ **≈5% thông lượng** |
| **F** | `SetObstacle` ghi ô **tâm** vào bảng **BẪY** thay vì bảng vật cản | **KHÔNG phải mã chết**: `LuaAddObstacle` (`ScriptFuns.cpp:4034`, đăng ký `"AddObstacle"` ở `:14407`) gọi với `nRange=1`; **5 script đang chạy** dùng: `lib_map.lua`, `hundred_arena.lua`, `citywar_city/head.lua`, `tongcastle.lua`, `lib_ctc.lua` |

### 10.2 Năm lỗi PHỤ do vòng phản biện 2 lớp (11 tác nhân) tìm ra — đều đã tự kiểm lại

| Mã | Tệp | Vì sao bắt buộc |
|---|---|---|
| **G1** | `KMissle.cpp` `PrePareFly` | `Mps2Map` đặt `*nR = -1` (`KSubWorld.cpp:1430/1445`), dòng ngay sau gọi `CurRegion.AddRef` ⇒ `m_Region[-1]`. `KRegion::AddRef` **ĐỌC** `m_nWidth/m_nHeight` ngoài mảng rồi **GHI** `pBuffer[index]++` — phép GHI này nằm **NGOÀI** khối `__try` ⇒ hỏng heap. Thêm `if (m_nRegionId >= 0)` |
| **G2** | `KNpc::Init()` | `Init()` (được `KNpc::Remove` gọi) **không xoá** `m_Command`. Sau bản vá **E**, khe NPC có thể được thu hồi khi còn lệnh `do_skill` treo ⇒ nhân vật **mới** nạp vào khe thi hành lệnh của người trước (`DoSkill` → `SendCommand(do_run)` tới toạ độ cũ) |
| **G3** | `KMissleSet::Remove` | `KMissle::Release()` phía **SERVER** không đặt `m_nMissleId = -1` (dòng đó nằm trong `#ifndef _SERVER`) ⇒ cửa chặn đầu hàm **không bao giờ ăn** ⇒ `Remove()` chạy 2 lần, trừ bộ đếm 2 lần. Đặt `m_nRegionId = -1` sau `DecRef` (đúng khuôn `KRegion::Close`) |
| **G4** | `KRegion::RemoveObj` | `KObj::Release()` đặt `m_nMapX/m_nMapY/m_nRegionIdx = 0` ⇒ lần `RemoveObj` **thứ hai** sẽ `DecRef` nhầm ô (0,0) region 0 sau khi đổi `> 0` → `>= 0`. Thêm cờ `bFound` |
| **G5** | `KSubWorld::CanPutObjBarrier` | **BẮT BUỘC đi cùng C1.** Trước C1 vế `\|\| GetRef(obj_missle)` **chưa bao giờ chạy** (bộ đếm luôn 0); sau C1 nó sống. Đạn **bay qua được tường** nên nó KHÔNG phải bằng chứng ô đó đi được — để nguyên thì tia quét của `GetFreeObjPos` **xuyên tường** và đồ rơi ra sau tường, đúng triệu chứng hàm này sinh ra để chữa. Kèm chặn `m_nRegionId >= 0` trước `RemoveMissle` (`:2248`) |

### 10.3 ⚠️ MỘT thay đổi ĐỔI CÂN BẰNG PvP — chủ game cần duyệt

Bản vá **B** là thay đổi duy nhất người chơi **cảm nhận được ngay**:

- `kimratrai.lua:47` và `tongratrai.lua:46` gọi `SetProtectTime(18*3)` cho **mọi** người ra cửa trại Tống Kim ⇒ **cửa trại lúc nào cũng có người bất tử**.
- **Hôm nay**: chiêu diện rộng dội vào cửa trại gần như vô hại (một con bất tử huỷ cả vùng quét).
- **Sau khi vá**: nó giết sạch tất cả những ai **không** còn bất tử.
- `pubgutils.lua:41` còn cho bất tử **180 giây** — ở chế độ đó thì cửa chặn cũ gần như vĩnh viễn.

**Muốn gỡ RIÊNG bản vá B** (giữ nguyên 5 cái còn lại):

```bash
cd /d/GAMEDEVNEW && python "$HOME/.claude/skills/swordonline-dev/scripts/safe_edit.py" Sources/Core/Src/KMissle.cpp --old "continue; //FIX 24/08: bo qua RIENG muc tieu bat tu" --new "return 0; //vong tron bat tu (giu nguyen theo yeu cau chu game) //"
```

### 10.4 Trạng thái binary

| | |
|---|---|
| `CoreServer.dll` x64 md5 **`36316816`** | đã đặt `E:\...\TESTLOFFF_ONLINE\bin\server\`, backup `CoreServer.dll.cu_2408_dot1_26f43102` — **CHỜ RESTART** |
| Cách build | **cây worktree riêng** (`HEAD` + đúng 6 tệp này) để **không cuốn theo** phần việc chưa commit của phiên song song (auto Tống Kim của WAuto: `CoreShell.cpp`, `ipc_shared.h`, `KPlayer.h` `ExtAuto`…) |
| `CoreClient.dll` | **KHÔNG thay.** Bản đang chạy là 22/08; thay sẽ kéo theo cả PORT5 + auto Tống Kim chưa test. Không cần: sát thương do máy chủ quyết định 100% |
| Cây vận hành `E:` | đã áp **từng dòng** cả 6 + 5 bản vá (không chép đè cả hàm) |

### 10.5 Cách nghiệm thu

| Vá | Kiểm thế nào |
|---|---|
| A1+A2 | Đo lại `[S4-MSL-HIT]`: tỷ lệ chạm `doing=10` phải về ~0; tỷ lệ viên "có chạm nhưng không chạm `wantid`" phải giảm mạnh từ 20,7% |
| C | Đứng một chỗ bắn ~600 phát rồi thả đồ tại đúng ô đó — trước đây ô bão hoà thì đồ nhảy ra ô khác; và **đồ không được rơi xuyên tường** (nếu có là G5 sai) |
| E | Histogram cast phải gộp về **một cụm ~444 ms** thay vì hai cụm |
| F | Vào Bách Nhân / Thành Bảo / Công Thành Chiến: ô **tâm** vật cản do script đặt phải chặn được |
| B | Tống Kim: chiêu diện rộng dội vào cửa trại **có** sát thương lên người hết bất tử |
| G2 | Thoát/vào lại nhiều lần ở bãi đông: nhân vật **không** được tự chạy/tự đánh ngay khi vừa nạp |

### 10.6 🔴 Bẫy vận hành phát hiện trong đợt này

- **Hai phiên Claude cùng làm trên `D:\GAMEDEVNEW`.** Phiên kia đang viết auto Tống Kim cho WAuto
  (`CoreShell.cpp` +909, `KPlayer.h` `struct ExtAuto`, `ipc_shared.h`, `KTongKimTables.h`, `S3Client.cpp`).
  ⇒ Build ở cây chính sẽ **cuốn theo** việc chưa xong của họ. `struct ExtAuto` nằm trong `#ifndef _SERVER`
  nên không ảnh hưởng `CoreServer.dll`, nhưng **đừng dựa vào may mắn** — build DLL đem thả phải qua worktree riêng.
- **`git add` phải chỉ đích danh tệp mình sửa.** Trong cây còn 4 tệp `.lib` nhị phân trạng thái `M`
  (`Lib/debug/engine.lib`, `Lib/release/engine.lib`, `Lib/release/CoreClient.lib`, `Lib/Represent.lib`) —
  **tuyệt đối không commit**, và chính vì `engine.lib` đã lệch nên **build client trong worktree sẽ FAIL**
  (`LNK2019 g_SetCanvasLockProbe`). Client phải build ở cây chính.
- **`Core.vcxproj` `Server Release|x64` có PostBuildEvent chép thẳng DLL vào `..\..\..\bin\server\`** —
  build = deploy trong phạm vi cây đó. Ở worktree thì nó rơi vào `_wt_*/bin/server`, vô hại.
