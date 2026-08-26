# "ĐÁNH HAY MISS" — TOÀN TẬP 4 ĐỢT (24–25/08/2026)

> Đây là **cửa vào**. Chi tiết đợt 1–2 nằm ở [`PHANTICH_DANH_MISS_2408.md`](PHANTICH_DANH_MISS_2408.md).
> Tài liệu này ghi **toàn bộ quá trình**: tìm ra gì, vá gì, **và những chỗ tôi làm sai** — phần cuối
> quan trọng nhất, vì nó là thứ làm mất nhiều thời gian nhất.

Triệu chứng chủ game báo, không đổi suốt 4 đợt:
> *"hay đánh miss với các phái tiếp cận, không phải các phái đánh đạn skill bay ra"*
> *"phải kích chuột di chuyển trong game mới đánh trúng"*

---

## 0. BẢNG TỔNG — 4 gốc KHÁC NHAU, cùng một triệu chứng

| Đợt | Gốc | Tệp | Commit | Đo trước → sau |
|---|---|---|---|---|
| **1** | Trừ toạ độ ô **cục bộ của HAI region khác nhau** ⇒ ±15/±31 thay vì ±1 ⇒ lật ngược phép so offset | `KMissle.cpp` `CheckNearestCollision` | `a0dd86c2` | 29,9% đạn hụt; 96% số hụt ở ô sát biên |
| **2** | 6 lỗi phụ của hệ va chạm (xác cướp va chạm, không ưu tiên mục tiêu, vòng bất tử chặn cả vùng, bộ đếm đạn ghi nhầm bảng vật phẩm, `FindNpc` tràn mảng, mất 1 khung/đòn) + 5 lỗi do chính bản vá lôi ra | `KMissle/KRegion/KNpc/KSubWorld/KMissleSet` | `faeee99d` | chạm XÁC 23,8% → **0%**; chạm nhầm 16,3% → **4,8%** |
| **3** | **`KRegion::FindNpc` chỉ trả NPC ĐẦU TIÊN trong ô** ⇒ nhiều con chung ô thì mục tiêu thật không bao giờ được xét | `KRegion.h` + `KMissle.cpp` | `550b7d0b` + `d8b29194` | "mục tiêu trong tầm 1 ô mà vẫn bị bỏ": **96,6% → 0,2%**; sát thương/cast 1,44 → **1,82** |
| **4** | **Quái đè lên đúng toạ độ người chơi (`dist=0`) ⇒ KHÔNG SINH RA VIÊN ĐẠN NÀO** | `KSkills.cpp` (2 chỗ) | `a1783e51` | 445 cast → chỉ 150 viên đạn; 1 mục tiêu ăn 290 cast / **0** sát thương |

**Đợt 4 mới là thứ trả lời câu "phải di chuyển mới đánh trúng"** — nó nằm ở khâu **SINH ĐẠN**, trước cả khâu va chạm mà đợt 1–3 vá. Ba đợt đầu **không thừa**: chúng vá ba nhánh thật khác nhau, đo được trước/sau.

**Bài học lớn nhất:** một triệu chứng duy nhất che **bốn** lỗi độc lập. Mỗi lần vá xong, tỷ lệ hụt giảm nhưng **không về 0** — và mỗi lần như thế phải **đo lại rồi mới kết luận**, không được cho là "đã xong".

---

## 1. Hạ tầng đo — thứ quyết định thắng thua

Không có log thì cả 4 đợt đều là đoán. Ba nhóm nhãn được thêm dần:

| Nhóm | Thêm khi | Trả lời câu hỏi |
|---|---|---|
| `S1`–`S3` | 21/08 | Máy chủ có nhận lệnh chiêu không, có tính sát thương không |
| **`S4`** (`S4-CAST`, `S4-MSL-HIT`, `S4-MSL-END`) | 22/08 | Máy chủ **thực sự thi hành** chiêu chưa; viên đạn có chạm ai không |
| **`S5-SCAN`** | 24/08 đêm (`1138a42a`) | **Bên trong vòng quét 3×3**: ô nào có NPC, ô nào bị phép so offset loại |

`S5-SCAN` in **bản đồ 9 ô** mỗi lần `CheckNearestCollision` chạy:
`.` ô trống · `x` có NPC nhưng bị loại · `o` NPC khác va chạm · `F` đúng mục tiêu bám.
**Chính nhãn này phá vỡ bế tắc ở đợt 3** — trước đó tôi chỉ thấy "đạn hụt" mà không biết vì sao.

> 🔑 Đặt log **lọc theo tên và không tiết chế** cho đúng khoảng mù, thay vì suy diễn từ dữ liệu tiết chế.
> Nếu bí, hãy thêm một điểm log rồi restart — rẻ hơn nhiều so với đoán sai ba lần.

### Công thức đo DUY NHẤT được chấp nhận

```
ty le hut     = so [S4-MSL-END] co lasthit=0  /  tong [S4-MSL-END]
sat thuong/cast = so [S1-WHO]                 /  so [S4-CAST]
```

`S4-CAST` vs `S4-MSL-END` lệch nhau ⇒ **có cast mà không sinh đạn** (chính là đợt 4).

---

## 2. Đợt 1 — lệch hệ toạ độ qua biên region

`KMissle.cpp` `CheckNearestCollision`:
```cpp
nDX = m_nCurrentMapX - Npc[nNpcIdx].m_MapX;   // hai hệ toạ độ CỤC BỘ khác nhau
```
`GetOffsetAxis` đặt ô đích = ô đạn + (i,j) **rồi mới cuộn vòng** sang region kế; `FindNpc` chỉ trả NPC
có `m_MapX == nRMx` (cục bộ region đó). Qua biên ⇒ hiệu ra **±15** (region rộng 16 ô) / **±31** (cao 32 ô).

Vá: **`nDX = -i; nDY = -j;`** — trong cùng region biểu thức cũ vốn đã bằng `-i`, nên là **no-op**;
chỉ đổi hành vi khi qua biên.

**Vì sao chỉ phái cận chiến**: `CheckCollision` rẽ hai nhánh theo `CollidRange` —
`==1` đi qua hàm có lỗi, `>1` (chiêu diện rộng) đi nhánh `else` **không hề tính hiệu ô**.

---

## 3. Đợt 2 — 6 lỗi phụ + 5 lỗi do chính bản vá lôi ra

Chi tiết ở mục 10 của `PHANTICH_DANH_MISS_2408.md`. Đáng nhớ nhất:

- **`AddRef(obj_missle)` ghi vào bảng VẬT PHẨM** trong khi `GetRef(obj_missle)` đọc `m_pMslRef`, cộng
  thiếu một `DecRef` ⇒ mỗi ô đạn chết **+1 vĩnh viễn**. Đo thật: một ô ăn **592 lần/3h của MỘT nhân vật**
  ⇒ vượt trần byte 255 ⇒ `CanPutObj` FALSE vĩnh viễn = **gốc "bỏ sót đồ rơi"**.
- **`ProcCommand` mất đúng 1 khung mỗi đòn** — bằng chứng đẹp nhất cả đợt: histogram khoảng cách hai lần
  cast tách **đúng hai cụm** `430-459 ms` và `480-519 ms`, cách nhau **55,5 ms = 1 khung @ 18 fps**.
- **Bản vá B (`return 0` → `continue` khi gặp NPC bất tử) ĐỔI CÂN BẰNG PvP**: `kimratrai.lua:47` /
  `tongratrai.lua:46` cho **mọi** người ra cửa trại Tống Kim 3 giây bất tử ⇒ cửa trại luôn có người bất tử.
  Lệnh gỡ riêng hunk này ở mục 10.3 bàn giao.

---

## 4. Đợt 3 — `FindNpc` chỉ trả NPC đầu tiên trong ô

Nhờ `S5-SCAN`, đo được trên 1102 viên hụt tại khung kiểm va chạm **đầu tiên**:

| | |
|---|---|
| Mục tiêu **vẫn trong tầm 1 ô** cả hai trục | **96,6%** (trung vị \|dx\|=11, \|dy\|=15 mps; ô = 32 mps) |
| Vòng quét **CÓ** thấy NPC trong ô đó nhưng bị loại (`map9='x'`) | **99,4%** |
| Tính lại phép so cho **chính mục tiêu**: đáng lẽ va chạm được | **1064/1102** |

Mảnh khép kín: **81 ca mục tiêu ở CÙNG Ô với viên đạn** (i=j=0) — cùng ô thì `nDX=nDY=0`, cả hai phép so
đều bị bỏ qua, **không có đường nào loại được**. Vậy mà ô đó vẫn `'x'` ⇒ con mà `FindNpc` trả về **không
phải mục tiêu**.

`KRegion::FindNpc` duyệt `m_NpcList` và trả về **con đầu tiên** ở ô được hỏi. Nhiều con chung một ô là
chuyện rất thường (xác nằm im + con mới hồi sinh + con đang đánh) ⇒ `CheckNearestCollision` xét con đó
thất bại thì **bỏ luôn CẢ Ô**.

Vá: thêm tham số **tuỳ chọn** `nPreferIdx = 0` — mặc định giữ hành vi cũ **y nguyên** cho 7 lời gọi khác;
khi có thì trả đúng mục tiêu, và **xác không được che mất con còn sống cùng ô**.

**r2 (`d8b29194`)**: đo lại thấy vẫn 6,07% lần chạm rơi vào xác — `ProcessCollision` gọi **lại** `FindNpc`
trên chính ô đó mà **không nói mình muốn con nào**. Cùng gốc, con đường thứ hai.

---

## 5. Đợt 4 — `dist=0` thì không sinh ra viên đạn nào

```cpp
// KSkills.cpp, nhanh SKILL_MF_Line / SKILL_SLT_Npc  (HAI cho: ~828 va ~918)
if (nSrcPX == nDesPX && nSrcPY == nDesPY)   return FALSE;   // <-- thoat, khong sinh dan
nDistance = g_GetDistance(...);  if (nDistance == 0) return FALSE;
```

Quái đi **đè đúng lên toạ độ người chơi** ⇒ `g_GetDirIndex` trả **−1** (`KMath.h:113`) ⇒ mã gốc bỏ cuộc.
**Nội lực vẫn bị trừ**, hoạt ảnh vẫn chạy, không có gì bay ra. Nhích một bước ⇒ đánh được.

Đo (pid 25308, 14 phút): **445 `S4-CAST` nhưng chỉ 150 `S4-MSL-END`**; id 27062 ăn **290 cast / 0 sát thương**,
`dist=0` cả 290 lần, trong khi hai con khác cùng lúc (dist=7, dist=9) vẫn ~1,0 sát thương/cast.
Bằng chứng trực tiếp: 104 dòng `[CAST-LINE-VEC]` đều `src==des dir=-1 childnum=1 movekind=1`.

Tỷ lệ `dist=0` phụ thuộc **loại quái / chỗ cắm**: đêm 24/08 **0,30%** · sáng 25/08 **0,25%** · tối 25/08 **28,33%**.
Vì thế mãi mới lộ.

Vá: khi `src == des` thì đặt hướng **cố định 0** và đẩy đích ra **16 mps** ⇒ `nDistance=16, nCos=1024, nSin=0`,
lọt cả hai cửa `abs() > 1024`. Đạn sinh ngay trên người mục tiêu, vẫn bám `m_nFollowNpcIdx`.

---

## 6. 🔴 NHỮNG CHỖ TÔI LÀM SAI — đọc kỹ phần này

Đây là phần đắt nhất. Mỗi mục dưới đây đã **thật sự** dẫn đến kết luận sai hoặc mất một vòng đo.

### 6.1 Sai khi ĐỌC LOG

| Lỗi | Hậu quả | Luật rút ra |
|---|---|---|
| Ghép `S4-MSL-HIT` ↔ `S4-MSL-END` **theo `msl` id** | `msl` id **bị tái sử dụng** ⇒ ra "785 viên trúng / 2783" — sai hoàn toàn | Ghép theo **thứ tự thời gian**, không gom theo id |
| Suy luận từ **sự VẮNG MẶT** của `[CAST-LINE-VEC]`, `[MISSLE-PARAM]` | Vô giá trị: nhãn `AUTOLOG_EVERY` **tiết chế + không lọc tên** (dùng chung ~1000 bot). Bằng chứng thật lại là 104 dòng **CÓ MẶT** | Chỉ suy luận từ vắng mặt với nhãn `AUTOLOG_IDX` (lọc tên, không tiết chế): `S4-*`, `S5-SCAN`, `S1-WHO`. `AUTOLOG_EVERY` **chỉ dùng khi nó CÓ MẶT** |
| Dùng số đếm `[MSL-NEARMISS]` làm tỷ lệ | Nhãn tiết chế 1 dòng/2 s cho toàn máy chủ | Chỉ dùng **số học từng dòng** (mỗi dòng tự chứa đủ tham số để kiểm lại công thức) |
| **Đếm nhầm cột** trong regex — 2 lần | Lần 1: đọc `mapY` tưởng là `barrier`. Lần 2: lệch 1 nhóm ⇒ ra "0% trong tầm" cho **cả** viên trúng lẫn hụt (vô lý) | Khi một con số vô lý cho **nhóm đối chứng**, nghi regex trước khi nghi game |
| Gộp cả **đạn con của chiêu khác** (285-290) vào thống kê chiêu 361 | Ra "62% hụt" — sai; lọc đúng `sk=361` thì là 8,6% | Luôn tách theo `sk=` trước khi tính tỷ lệ |
| So `S3-PKT-IN 21069` với `NET-SKILL-PKT 9042` rồi kết luận "mất gói" | Thực ra là **lệch cửa sổ thời gian** giữa hai tệp log | Lọc theo **cửa sổ giao nhau** trước khi lấy bất kỳ tỷ lệ nào |
| Đọc `pos()` trong `S4-MSL-END` như vị trí lúc thử va chạm | Đó là vị trí **lúc hết tuổi thọ** — đạn sinh ở mép rồi bay xa 3-4 ô | |
| Kết luận `SetObstacle` là "mã chết" | **Sai** — `LuaAddObstacle` có **5 script đang chạy** dùng (Bách Nhân, Thành Bảo, Công Thành Chiến…) | Kiểm người gọi trong **script `.lua` thật**, không chỉ trong C++ |

### 6.2 Sai khi ĐO — bẫy môi trường

- **`[AutoLog] On=0`**: có một lần kéo log về mà **không có một dòng nào của DLL mới**, vì phiên đo hiệu
  năng đã tắt log (`config.ini:19`). Mất trọn một vòng.
  ⇒ **Luôn kiểm `On=1` + giờ khởi động GameServer TRƯỚC khi phân tích.**
- **Dấu vân tay để biết DLL nào đang chạy**: đếm `S4-MSL-HIT` có `doing=10` (xác) — bản trước đợt 2 là
  **23,8%**, sau đợt 2 là **0,00%**. Rẻ và không thể cãi.
- **So sánh hai cửa sổ khác bãi cắm là vô nghĩa**: cùng DLL cũ, bãi này 0,9% hụt, bãi kia 29,9%. Luôn
  tách theo **mục tiêu** và theo **hình học** (dist, ô có nằm trên biên region không).
- **`[MSL-SET-FULL]` báo động giả**: dòng log đặt **TRƯỚC** cửa chặn `if (nFreeIndex <= 0)` nên in ra ở
  **mọi lần Add THÀNH CÔNG** (đo thật: 97 dòng với `dang dung=9` trên `MAX_MISSLE=20000`). Suýt đi vào
  đường cụt "hết khe đạn".

### 6.3 Sai trong CHÍNH BẢN VÁ — do vòng phản biện bắt

| Bản vá | Lỗi | Sửa |
|---|---|---|
| Đợt 4 (`dist=0`) | Lấy hướng từ `Npc[].m_Dir` — **biến chết phía server**: mọi chỗ gán nằm trong `#ifndef _SERVER` (`KNpc.cpp` 2636/7664/7780), chỉ còn `KNpc::TurnTo` (:10310) chạy cả hai bên, mà `TurnTo` cũng trả −1 khi trùng điểm | Đặt **cố định 0** — client/server luôn khớp; hướng vốn không quan trọng vì mục tiêu nằm đúng điểm bắn |
| Đợt 4 | Chỗ vá thứ nhất còn nhận cả **`MMK_Parabola`**; đẩy đích 16 mps cho `nTime = 16/speed = 0` ⇒ `m_nHeightSpeed` **âm** ⇒ đạn rơi ngay dưới chân, sinh **vụ nổ điểm-không chưa từng có** | Chỉ mở cho `MMK_Line` |
| Đợt 2 (C1) | Đổi `obj_missle` sang `m_pMslRef` **đánh thức** vế `\|\| GetRef(obj_missle)` trong `CanPutObjBarrier` — vế này **chưa bao giờ chạy**. Đạn **bay qua được tường** nên nó không phải bằng chứng ô đó đi được ⇒ đồ rơi **xuyên tường** | Bỏ vế đó, giữ vế `obj_npc` |
| Đợt 2 (C3) | `KObj::Release()` zero toạ độ ⇒ `RemoveObj` lần 2 `DecRef` nhầm ô (0,0) region 0 sau khi đổi `>0`→`>=0` | Thêm cờ `bFound` |
| Đợt 2 (E) | `KNpc::Init()` **không xoá `m_Command`** ⇒ khe NPC tái dùng sẽ thi hành lệnh của người trước ("vừa vào game tự chạy/tự đánh") | Thêm `m_Command.CmdKind = do_none;` |
| Đợt 2 (C2) | `KMissle::Release()` phía **server không đặt `m_nMissleId = -1`** ⇒ `Remove()` chạy được 2 lần, trừ bộ đếm 2 lần | Đặt `m_nRegionId = -1` sau `DecRef` |
| Đợt 2 (G1) | `PrePareFly`: `Mps2Map` đặt `*nR = -1` rồi dòng ngay sau gọi `CurRegion.AddRef` ⇒ `m_Region[-1]`; `AddRef` **GHI** `pBuffer[index]++` **ngoài khối `__try`** ⇒ hỏng heap | Thêm `if (m_nRegionId >= 0)` |

> **Luật:** mỗi bản vá đều phải đi qua ít nhất một vòng phản biện đọc lại **mã thật**, đặc biệt câu hỏi
> *"vế này trước nay có bao giờ chạy chưa?"* — bật một nhánh chết là cách tạo lỗi mới nhanh nhất.

### 6.4 Sai trong VẬN HÀNH

- **Hai phiên Claude cùng làm trên `D:\GAMEDEVNEW`** và cùng thả DLL vào một `bin\`. Đã có lúc DLL đang
  chạy **không phải** bản tôi thả.
  ⇒ Build DLL đem thả phải qua **`git worktree` riêng** (HEAD + đúng tệp của mình); **`md5` lại trước và sau**
  khi thay; `git add` **chỉ đích danh tệp mình sửa** (trong cây còn 4 tệp `.lib` nhị phân trạng thái `M` — cấm commit).
- **Suýt thả bản client dịch từ trạng thái nguồn trung gian**: `.obj` cũ hơn `.cpp`.
  ⇒ **So mốc thời gian `.obj`/`.dll` với mốc sửa nguồn** trước khi thả. Đây là cửa chặn bắt buộc.
- **Build client trong worktree sẽ FAIL** (`LNK2019 g_SetCanvasLockProbe`) vì `engine.lib` trong git đã lạc hậu
  ⇒ client phải build ở cây chính.
- `Core.vcxproj` `Server Release|x64` có **PostBuildEvent chép thẳng DLL** vào `..\..\..\bin\server\` — **build = deploy**
  trong phạm vi cây đó.
- Mã nguồn là **ANSI/TCVN3 + GBK**: mọi sửa đổi đi qua `safe_edit.py`/`vn_edit.py`, kiểm `high bytes` không đổi
  và `FFFD = 0` trước/sau. Tệp CRLF ⇒ neo nhiều dòng phải ghi bằng CRLF.

---

## 7. ⛔ Bản vá đã bị PHẢN BIỆN BÁC BỎ — cấm làm lại

Danh sách đầy đủ ở mục 7 của `PHANTICH_DANH_MISS_2408.md`. Tóm tắt:

| Đề xuất | Vì sao bác |
|---|---|
| Sửa 4 công thức toạ độ trong `SetTrap`/`SetObstacle` | Mọi lời gọi hardcode `nRange = 0/1`; tại đúng các giá trị đó công thức **trùng khít** công thức đúng |
| Thêm `abs()` vào `KRegion::BroadCast` | Vét cạn 8 hướng: **0/2.097.152** cặp bị cắt oan; thêm vào còn làm **NPC cách 33-40 ô biến mất** |
| Đổi `>` thành `>=` khi chuẩn hoá offset | `Map2Mps` tuyến tính nên `(ô N, off 32768)` ≡ `(ô N+1, off 0)` — kết quả **y hệt**; áp vào còn lật 3 lần từ chối đúng thành trúng ảo |
| Nới `radius+20` → `radius+120` cho `S2-MELEE-TOOFAR-RUN` | 486 dòng chỉ là 82-100 **đợt** đuổi, không phải 486 đòn mất; khoảng cách server đo là **THẬT** |
| Nắn vị trí nhân vật trong `SyncNpcMinPlayer` | Việc không nắn là **CÓ Ý** (client-side prediction); snap vào = giật rubber-band 4 ô mỗi lần sync |
| Bỏ cổng `m_nNeedFixPos > 0` | Đọc nhầm ngữ nghĩa: nó là **bộ đếm bị vật cản chặn**, `fix=0` = quái đi **bình thường** |
| `OnFly` kiểm va chạm ngay tại vị trí bắn ("1b") | Trên tập dữ liệu đó **thừa** với bản vá đợt 1; rủi ro cao hơn hẳn (đạn **mất một bước bay** ⇒ tầm ngắn đi 30 mps cho **mọi** chiêu) |

---

## 8. Trạng thái binary & cách nghiệm thu

| | md5 | Bản lùi |
|---|---|---|
| `bin\server\CoreServer.dll` | **`d44b1233`** | `.cu_2508_truoc_dist0_d5d0f6e1` |
| `bin\client\CoreClient.dll` | **`edbb0516`** | `.cu_2508_truoc_dist0_a0535f2a` |

**Chờ restart GameServer.** Điểm log `S5-SCAN` vẫn còn để đo tiếp.

Nghiệm thu đợt 4: **`S4-CAST` và `S4-MSL-END` phải bằng nhau** (mỗi lần cast sinh đúng một viên đạn),
thay vì 445 / 150 như hiện nay. Kèm `sát thương/cast` và `% chạm doing=10`.

⚠️ Nhớ báo người test: các bản vá này **lật ngược** nhánh sai chứ không tắt nhánh, nên vừa hết hụt oan
vừa mất vài đòn "trúng oan"; và **quái đánh người chơi cũng hết hụt** ⇒ sát thương nhận vào tăng,
phải đo lại độ khó bãi luyện + Tống Kim.

---

## 9. Việc còn lại (chưa làm, đã đo)

1. **Auto quạt vào tường**: một cửa sổ có **46,5%** số cast dồn vào **một** con ở `dist=73` mà **1.271 viên
   đạn chết ở khung đầu với `barrier=1`** (đâm địa hình). Thuộc phần **auto/chọn mục tiêu**: cần cho auto
   **bỏ mục tiêu khi đạn liên tục chết vì `barrier`** thay vì đứng quạt.
2. **Bản vá B đổi cân bằng PvP** ở cửa trại Tống Kim — cần chủ game duyệt hoặc gỡ riêng (mục 10.3).
3. **`[MSL-SET-FULL]` đặt sai chỗ** (`KMissleSet.cpp:65`) — nên chuyển vào trong thân `if` hoặc đổi tên nhãn.
4. Cùng kiểu lỗi `nDirIndex = -1` còn ở các nhánh anh em (`MF_Wall`, `CastSpread`, `MF_Circle`) — **không gây
   mất máu**, chỉ bắn lệch hướng khi quái đè lên người. Chưa sửa vì ngoài phạm vi tối thiểu.

---

## 10. Chuỗi commit

```
a0dd86c2  24/08 16:54  [FIX] dot 1 - lech he toa do qua bien region
49c72d5e  24/08 16:56  PHANTICH_DANH_MISS_2408 (ban giao dot 1)
faeee99d  24/08 18:49  [FIX] dot 2 - 6 loi con lai + 5 loi phu
25049436  24/08 18:51  PHANTICH_DANH_MISS_2408 muc 10 (ban giao dot 2)
1138a42a  24/08 23:29  [CHAN DOAN] diem log S5-SCAN
550b7d0b  25/08 01:04  [FIX] dot 3 - KRegion::FindNpc chi tra NPC dau tien
d8b29194  25/08 08:39  [FIX] dot 3 r2 - ProcessCollision cung phai noi muon ai
a1783e51  25/08 22:31  [FIX] dot 4 - dist=0 khong sinh ra vien dan nao
```
