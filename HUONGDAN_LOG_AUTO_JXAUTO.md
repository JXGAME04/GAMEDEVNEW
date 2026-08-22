# LOG CHẨN ĐOÁN AUTO — `jx_auto.log` (21/08/2026)

> Đặt log để tìm nguyên nhân **"đánh hay miss"** và **"bỏ sót đồ rớt từ NPC khi luyện công"**.
> **Chỉ đặt log — không sửa logic.** Bản chốt: **310 dòng log**, commit `61e83b52` → `f601e862`.
> Binary đang chạy: `CoreClient.dll` **21/08 13:15** (md5 `ff775c6f…`).

## 1. Bật / tắt

`bin\client\Config.ini`, mục `[Client]`:

```ini
AutoLog=1      ; 1 = ghi log, 0 (mặc định) = TẮT hoàn toàn, không tốn chi phí
```
(Đã bật sẵn ở máy hiện tại. Đổi giá trị xong phải **thoát game vào lại**.)

Tệp ghi ra: **`bin\client\jx_auto.log`** (thư mục làm việc của `Game.exe`).
- Mỗi dòng tự có tiền tố `t=<mili giây> pid=<id tiến trình>` → mở nhiều client vẫn tách được.
- Trần **1200 dòng/giây**; vượt thì bỏ và ghi lại `[AUTOLOG] bo qua N dong`.
- Ghi qua tệp **mở sẵn** (flush mỗi 50 dòng / 500 ms) nên gần như không ảnh hưởng nhịp game.
- Bản **Server tắt cứng** — GameServer không bao giờ ghi/in gì.
- Tệp > 64 MB tự đổi tên thành `jx_auto.log.1` rồi ghi lại từ đầu.
- **Muốn log sạch cho một lần thử: xoá `jx_auto.log` trước khi vào game** (tệp cũ trộn nhiều bản DLL sẽ gây hiểu nhầm).

### Mức chi tiết theo vùng

| Vùng | Ghi thế nào |
|---|---|
| Auto (`CoreShell`, `KPlayer`, `S3Client`): chọn mục tiêu, phát lệnh đánh, từng món đồ bị bỏ qua | ghi **mọi lần** |
| Engine (`KMissle`, `KSkills`, `KNpc`, `KProtocolProcess`…): chạy mỗi khung hình cho từng viên đạn/NPC | tiết chế **1 dòng/giây** mỗi điểm (vài nhãn rất nóng: 2–5 giây) |

> 🔴 **Đo thật 21/08:** bản chưa tiết chế ngập **604 dòng/giây** (28 MB trong vài phút) và **nuốt mất**
> chính các dòng `FIGHT-*` / `PICK-*` cần xem. Bản 13:15 đã tiết chế; nếu vẫn thấy dòng
> `[AUTOLOG] bo qua N dong` xuất hiện liên tục thì báo tôi để giãn tiếp.

## 2. Đọc log thế nào

Lọc theo nhãn, ví dụ (Git Bash / PowerShell):

```bash
grep -E "\[(FIGHT|FT|CAST|MSL|MIS)-" bin/client/jx_auto.log | tail -200
```

| Muốn biết | Lọc nhãn |
|---|---|
| Vì sao **không đánh / đánh hụt** | `FIGHT-`, `FT-`, `CAST-`, `AUTO-`, `E4_SKILL`, `E4_DMG`, `E4_HIT` |
| Đường đi viên chiêu (**missile**) | `MIS-`, `MSL-`, `E2-`, `E3_`, `COLL-`, `DMG-`, `HIT-` |
| Vì sao **bỏ sót đồ** | `PICK-`, `PICK2-`, `PICKOBJ-`, `AUTOPICK-`, `SPICK-`, `OBJ`, `CHECKOBJ` |
| **Di chuyển** (không tới nơi, kẹt) | `MC-`, `MOVE-`, `FOLL-`, `ENC-`, `AP-` |
| **Đồng bộ toạ độ** server ↔ client | `SYNCNPC`, `SYNCMIN`, `SYNCME`, `SYNCPLAYER`, `NET-`, `NETSKILL-`, `ME-` |
| Cổng điều phối auto (ai "nuốt" lượt) | `AUTO-PASS`, `AUTO-STAGE`, `PICK-RET`, `FIGHT-RET`, `MOVE-RET`, `PK-RET`, `FIGHT-SKIP` |

## 3. Các nhãn quan trọng nhất (đọc trước)

### "Đánh hay miss"
| Nhãn | Ý nghĩa |
|---|---|
| `FIGHT-IN` | ảnh chụp trạng thái mỗi lượt auto đánh (fight-mode, ô lưới, máu, mục tiêu) |
| `FIGHT-NOTARGET` | auto **không tìm được mục tiêu** → đứng im (kèm tầm nhìn, gốc quét) |
| `FT-SKIP-EXCL` | con quái bị **sổ đen 30 giây** loại ra (xem `FIGHT-NODMG` bên dưới) |
| `FT-SKIP-REL` / `FT-SKIP-VIS` | bị loại vì **không phải kẻ địch** / **ngoài tầm nhìn** (chỉ log con sát rìa) |
| `FIGHT-DIST` | khoảng cách thật tới mục tiêu tại đúng thời điểm quyết định |
| **`FIGHT-NODMG`** | **auto kết luận "đánh mà máu mục tiêu không đổi" → cho vào sổ đen.** Đây là dấu hiệu trực tiếp của "đánh hay miss" |
| `FIGHT-CLAMP` | tầm đánh bị **kẹp** xuống `max(nNearDist,75)` khi bật "tiến tới đánh" |
| `FIGHT-EMIT` | auto **thật sự phát lệnh đánh** (so `dist` với `radius` để biết đánh hay chạy tới) |
| `CAST-GATE` | bị chặn tung chiêu: **cooldown** hay **thiếu nội lực** |
| `CAST-RANGE` | ngoài tầm chiêu → **chỉ nhớ mục tiêu, không gửi và không đi lại gần** |
| `NET-SKILL-PKT` | gói lệnh chiêu thật sự rời client (đếm được số lần đánh) |
| `MIS-BORN` | viên chiêu sinh ra (chủ, chiêu, mục tiêu, toạ độ, tầm, tốc độ) |
| `MIS-ACT-FOLLOWLOST` | **mất mục tiêu bám đuổi** (chết / tàng hình / bất tử / khác map) → bay thẳng, trượt |
| `MIS-LIFE-END` | hết tuổi thọ mà **không tự nổ** → biến mất, không sát thương |
| `E2-*`, `DMG-*`, `HIT-*` | tính sát thương / trúng - trượt |

### "Bỏ sót đồ rớt"
| Nhãn | Ý nghĩa |
|---|---|
| `PICK-OFF` | tắt ô "Tự nhặt đồ" |
| **`PICK-NOFIGHT`** | **chưa rút vũ khí (`m_FightMode=0`) → TOÀN BỘ hệ nhặt bị tắt** |
| `PICK-SCAN` / `PICK2-SCAN` | mốc gốc: vị trí thật, tầm nhặt đang dùng |
| **`PICK-FAR`** | món đồ **ngoài tầm nhặt trực tiếp (cứng 200)** |
| **`PICK-SKIP-COOLDOWN` / `PICK2-SKIP3`** | bị bảng nhớ bỏ qua — **khoá theo *loại* vật phẩm**, nên 3 lần lỗi là cả loại đó bị bỏ 3 phút |
| `PICK-BAGFULL` / `PICK2-BAGFULL` | **túi đầy** (im lặng ở bản gốc) |
| `PICK-SKIP-NAME` / `PICK-TYPE-IN` | bị loại vì danh sách cấm nhặt / bộ lọc "loại vật phẩm ưu tiên" |
| **`PICK2-OUTANCHOR`** | đang đi theo điểm/toạ độ → đồ phải gần **điểm neo**, không phải gần nhân vật |
| `PICK2-GO` | quyết định chạy tới nhặt (toạ độ đích) |
| `PICKOBJ-NOROOM` | không tìm được ô trống → **không gửi lệnh nhặt** |
| `PICKOBJ-SEND` | gói lệnh nhặt thật sự rời client |
| `OBJADD-*` / `OBJREM-*` | đồ rơi được server đồng bộ xuống / bị xoá |

### Đồng bộ toạ độ
`SYNCNPC-*`, `SYNCMIN-*`, `SYNCME-DRIFT` in **cả toạ độ client đang giữ lẫn toạ độ server gửi + độ
lệch** → biết "miss" có phải do lệch vị trí không. `NET-WALK/RUN/JUMP/HURT/DEATH` là lệnh nhận về.

## 4. Cách dùng để tìm nguyên nhân

1. Xoá `jx_auto.log`, vào game, bật auto **đúng cấu hình đang lỗi**, chạy ~5-10 phút.
2. Lỗi **bỏ sót đồ**: tìm thời điểm có đồ rơi mà bot không nhặt → xem quanh mốc `t=` đó có nhãn
   `PICK-*` nào, dòng đầu tiên giải thích lý do bỏ qua.
3. Lỗi **đánh miss**: tìm `FIGHT-NODMG`; lùi lên trên xem `FIGHT-EMIT` / `NET-SKILL-PKT` (có gửi lệnh
   không), `MIS-BORN` → `MIS-*` (viên chiêu kết thúc thế nào), và `SYNCNPC-*` quanh đó (mục tiêu có bị
   server kéo đi chỗ khác không).
4. Gửi tôi đoạn log quanh mốc đó — tôi sẽ chỉ ra đúng nhánh mã gây ra.

## 5. Ghi chú kỹ thuật

- Hạ tầng ở `Sources/Core/Src/KCore.h` (khai báo + macro) và `KCore.cpp` (cài đặt) — build cho **cả
  client lẫn server**; hiện chỉ client bật (`Config.ini` của server không có khoá `AutoLog`).
- Log dùng `AUTOLOG_EVERY(ms, ...)` ở chỗ chạy mỗi khung hình để khỏi ngập; các nhánh hiếm dùng
  `AUTOLOG(...)`.
- Bản vá **không xoá dòng nào** của mã gốc (`git show 61e83b52 --stat`: 432 insertions, 0 deletions).
- Binary đang chạy: `CoreClient.dll` **21/08 13:15** (2.278.912 B, md5 `ff775c6f…`); bản lùi
  `CoreClient_cu_2108_1210.dll` (không có log).
- 🔴 Ba lỗi do **chính đợt đặt log** gây ra đã vá ở `b7e69aee` — nặng nhất: dòng log lọt vào giữa `if`
  không ngoặc trong `FindTargetNpc` khiến **auto không chọn được mục tiêu** (chỉ chạy tới NPC rồi đứng).
  Lần sau thêm log phải chạy lại bộ quét `scan_danger.py` trong scratchpad.


---

## 6. LOG PHÍA MÁY CHỦ — `jx_auto_server.log` (thêm 21/08 chiều)

Client chỉ thấy *"đánh mà máu không giảm"*; **đường tính TRÚNG/TRƯỢT nằm hoàn toàn ở máy chủ**.
25 điểm log server dưới đây bù đúng khoảng mù đó.

### 6.1 Bật / tắt (KHÔNG cần build lại)

`E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINEin\server\config.ini`:

```ini
[AutoLog]
On=1
Name=CaiBang
```

- `On=1` bật, `0` tắt. Ghi ra `bin\server\jx_auto_server.log`, **không** in ra cửa sổ server.
- `Name=` là **tên nhân vật cần theo dõi**. 🔴 **Bắt buộc điền** — máy chủ đang chạy ~1000 bot,
  bot nào cũng tính là "người chơi", để trống là log ngập và che mất đòn đánh của nhân vật thật.
- Đổi tên nhân vật khác thì sửa dòng `Name=` rồi **khởi động lại GameServer** (đọc lúc nạp).

### 6.2 Đọc theo thứ tự — một đòn đánh đi qua 3 chặng

| Chặng | Nhãn | Cho biết |
|---|---|---|
| **S3 — gói tin vào** | `S3-PKT-IN` → `S2-NETSKILL-IN` → `S3-PKT-REJ` | Máy chủ **có nhận** lệnh chiêu không, có bị từ chối im lặng không (id chiêu, toạ độ) |
| | `S2-NETSKILL-TARGET`, `S3-TGT-FIND` | Tìm mục tiêu theo id: `found_idx=0` = **không thấy** (mục tiêu ngoài 9 vùng quét) |
| | `S3-CAST-XY`, `S3-CMD-SWALLOW` | Lệnh bị **nuốt** vì ô lệnh `m_Command` chỉ chứa 1 lệnh/nhịp |
| **S2 — quyết định đánh** | `S2-MELEE-TOOFAR-RUN` | 🔴 Nghi can số 1: máy chủ đo xa hơn tầm chiêu → **tự đổi đòn đánh thành lệnh CHẠY** |
| | `S2-CANCAST-DENY` | `CanCastSkill` từ chối (nội lực, im lặng, đang cưỡi ngựa, tầm) |
| | `S2-SKILL-NOTLEARNED`, `S3-SETACTIVE` | Chiêu chưa học / chọn nhầm ô chiêu |
| **S1 — tính sát thương** | `S1-WHO`, `S1-ARDATA` | Ai đánh ai, AR / phòng thủ / `usear` |
| | `S1-MELEE-NOROLL` | Đòn **cận chiến của người chơi vào quái BỎ QUA xúc xắc trúng/trượt** ⇒ loại trừ khâu này |
| | `S1-CRIT-ROLL`, `S1-PHYS-PRE/POST` | Máu trước/sau — đối chứng trực tiếp với `FIGHT-NODMG` bên client |
| | `S2-DODGE-1/2/3` | Chữ "né" client vẽ ra **thực chất là sát thương bị về 0**, không phải xúc xắc né |
| | `S2-ARMOR`, `S2-ARMOR-EAT` | Giáp vật lý **nuốt trọn** đòn |

### 6.3 Ghép hai bên log

Cả hai tệp cùng tiền tố `t=<ms>`; lấy mốc `t=` của một dòng `FIGHT-NODMG` (client) rồi tìm
`S1-PHYS-PRE`/`S1-PHYS-POST` gần nhất trong log server để biết máu **thực sự** có đổi không.

### 6.4 Quy tắc khi thêm điểm log server mới

Dùng `AUTOLOG_IDX(idx, ...)` / `AUTOLOG_IDX_EVERY(idx, ms, ...)` (`KCore.h`) — chúng lọc theo tên
**và** tự kiểm chỉ số, nên `Npc[idx]` trong danh sách đối số không bao giờ bị đọc với chỉ số xấu.
Đây chính là loại lỗi đã làm **sập game lúc 13:52** (`Npc[chỉ-số-viên-đạn]`).

### 6.5 Nhóm S4 (thêm 21/08 22:00) — đo "đánh hụt" THẬT theo từng viên đạn

`S1-WHO` chỉ đếm đạn **đã chạm** mục tiêu; chiêu cận chiến 361 là đạn bay thẳng (`SKILL_MF_Line`), bay hụt thì
không bao giờ tới `S1-WHO`. Nhóm S4 (lọc tên, **không tiết chế**, chỉ server) bịt lỗ đó:

| Nhãn | Ở đâu | Nghĩa |
|---|---|---|
| `S4-CAST` | `KNpc::DoSkill` ngay trước `DoOrdinSkill` | máy chủ **thực sự thi hành** chiêu; có khoảng cách server đo, vị trí 2 bên, mục tiêu còn sống không |
| `S4-MSL-HIT` | `KMissle::ProcessCollision` trước `ProcessDamage` | viên đạn `msl=` chạm NPC nào (kind/doing/life) |
| `S4-MSL-END` | `KMissle::DoVanish` | đạn kết thúc: `status=2 lasthit=0` = **bay hết tuổi thọ không chạm ai** = hụt thật; `follow=0` = mất mục tiêu bám; `barrier=1` = vướng địa hình |

**Tỷ lệ hụt thật** = số `S4-CAST` không có `S4-MSL-HIT` tương ứng (ghép theo `msl=` của `S4-MSL-END` ngay sau,
hoặc theo mốc `t=`) / tổng `S4-CAST`. Bốn nhãn `S3-PKT-IN`, `S3-PROC-FINDSAME`, `S3-CMD-SWALLOW`,
`S2-MELEE-TOOFAR-RUN` nay cũng **không tiết chế** ⇒ `% lệnh bị nuốt = SWALLOW / (SWALLOW + FINDSAME)` là số thật.
