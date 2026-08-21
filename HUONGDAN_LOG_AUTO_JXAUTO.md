# LOG CHẨN ĐOÁN AUTO — `jx_auto.log` (21/08/2026)

> Đặt log để tìm nguyên nhân **"đánh hay miss"** và **"bỏ sót đồ rớt từ NPC khi luyện công"**.
> **Chỉ đặt log — không sửa logic** (bản vá 341 dòng, 0 dòng bị xoá). Commit `61e83b52`.

## 1. Bật / tắt

`bin\client\Config.ini`, mục `[Client]`:

```ini
AutoLog=1      ; 1 = ghi log, 0 (mặc định) = TẮT hoàn toàn, không tốn chi phí
```
(Đã bật sẵn ở máy hiện tại. Đổi giá trị xong phải **thoát game vào lại**.)

Tệp ghi ra: **`bin\client\jx_auto.log`** (thư mục làm việc của `Game.exe`).
- Mỗi dòng tự có tiền tố `t=<mili giây> pid=<id tiến trình>` → mở nhiều client vẫn tách được.
- Trần **600 dòng/giây**; vượt thì bỏ và ghi lại `[AUTOLOG] bo qua N dong`.
- Tệp > 64 MB tự đổi tên thành `jx_auto.log.1` rồi ghi lại từ đầu.
- **Muốn log sạch cho một lần thử: xoá `jx_auto.log` trước khi vào game.**

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
- Binary đang chạy: `CoreClient.dll` **21/08 12:50** (2.286.592 B, md5 `f13d525d…`); bản lùi
  `CoreClient_cu_2108_1210.dll` (không có log).
