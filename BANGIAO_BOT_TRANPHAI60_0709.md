# BÀN GIAO 07/09 11:00 — Bot nâng MAX chiêu trấn phái cấp 60

Chủ: *"kiểm tra thử bot đã công full skill 1x đến 6x chưa?"* → *"cho bot nâng max skill trấn phái cấp 60"*.

Commit **`962a8255`** trên `origin/main`. Bộ vá idempotent: `ReverseTools/goi_va_bot_tranphai60_0709.py`. Công cụ đo: `ReverseTools/kiem_skill_bot_0709.py <kiem_skill_bot_skills_map.json>`.

## 1. Nhị phân chờ swap

| tệp | md5 | cỡ | ghi chú |
|---|---|---|---|
| `bin\server\CoreServer.dll.moi` | `e8130f460cd5219e51a55ddebe8e5a29` | 18 482 176 | đặt 11:02, build main `962a8255` = bản đang chạy 8008e14d (`34b58589`) + TRANPHAI60; chỉ máy chủ, client không đổi |

Swap: tắt GameServer → `ChayGameServer.bat`. Khe trống lúc đặt.

## 2. Đo trước khi sửa (1.000 bot, bản lưu mới nhất sau 07:28)

- Chiêu chủ động 1x–6x: full 20 ở 9/10 phái. Đường Môn 4 chiêu ám khí khác hệ (347/343/345/349) = 0 trên 99/99 bot vì vòng nâng bỏ chiêu khác ngũ hành — **chưa sửa, chờ chủ**.
- Chiêu bị động (style 3) = 0 ở mọi phái vì vòng nâng bỏ qua có chủ đích (chú thích "bot không dùng bùa" nhầm style 3 bị động với style 2 bùa cast).
- Bộ **trấn phái 60** = chiêu mốc 60 có MaxLevel 30, mỗi phái đúng một chiêu, trùng chú thích "tran phai" trong `SKILLNORMAL` (`script\header\factionhead.lua`):

| phái | id | chiêu | kiểu | trước | sau swap |
|---|---|---|---|---|---|
| Thiếu Lâm | 273 | Như Lai Thiên Diệp | 2 | 20 | 30 |
| Thiên Vương | 36 | Thiên Vương Chiến Ý | 3 | 0 | 30 |
| Đường Môn | 48 | Tâm Nhãn | 3 | 0 | 30 |
| Ngũ Độc | 75 | Ngũ Độc Kỳ Kinh | 3 | 0 | 30 |
| Nga Mi | 252 | Phật Pháp Vô Biên | 3 | 0 | 30 |
| Thúy Yên | 114 | Băng Cốt Tuyết Tâm | 3 | 0 | 30 |
| Cái Bang | 130 | Tẩy Điệp Cuồng Vũ | 2 | 20 | 30 |
| Thiên Nhẫn | 150 | Thiên Ma Giải Thể | 2 | 20 | 30 |
| Võ Đang | 166 | Thái Cực Thần Công | 3 | 0 | 30 |
| Côn Lôn | 275 | Sương Ngạo Côn Lôn | 3 | 0 | 30 |

## 3. Sửa gì ([KPlayerBot.cpp](Sources/Core/Src/KPlayerBot.cpp) vòng "nâng full theo cấp" trong `pb_Fight`)

`bTranPhai = (ReqLevel == 60 && GetSkillMaxLevel > 20)`. Chiêu trấn phái không bị bỏ qua dù là bị động và được nâng tới MaxLevel (30) thay vì kẹp 20; `KSkillList::Add`/`IncreaseLevel` tự `Cast` chiêu bị động nên hiệu ứng áp ngay. Mọi chiêu khác giữ luật cũ. Bot cũ được nâng ở nhịp đánh đầu tiên sau restart (`nAtkPickLevel` về 0), có log `[BotTranPhai] <tên> cap N: <chiêu> (id) cũ -> mới (max)` trong `bot.log`.

## 4. Nghiệm thu sau swap

```bash
grep -c "\[BotTranPhai\]" /e/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/server/bot.log
```

Mong đợi ~1.000 dòng trong vài phút đầu (mỗi bot một chiêu), sau đó thưa dần (chỉ bot mới lên 60). Kiểm lại dữ liệu: `python ReverseTools/kiem_skill_bot_0709.py ReverseTools/kiem_skill_bot_skills_map.json` → mục "CHIEU BI DONG chua full" không còn 10 chiêu trên; các "X pháp" cấp 10 và tâm pháp khác (21, 360, 630, 66, 124, 129, 274, 160) vẫn 0 là đúng thiết kế hiện tại.

## 6. Nghiệm thu (bản e8130f46 ĐÃ LIVE 11:09:22, chủ swap ngay sau khi đặt)

- `bot.log` 11:13–11:17: **799 dòng `[BotTranPhai]`**, mỗi bot đang đánh nâng đúng một chiêu: 166 ×95, 150 ×90, 36 ×87, 75 ×86, 252 ×83, 114 ×83, 48 ×76, 273 ×73, 275 ×66, 130 ×60; bị động 0 → 30, bùa 20 → 30, đúng bộ 10 chiêu.
- ~200 bot bán sạp không vào `pb_Fight` nên chưa nâng (đúng thiết kế hiện tại: nâng chiêu nằm trong nhịp đánh). Muốn nâng cả bot sạp thì chuyển khối nâng sang `pb_TrangBiTheoCap` (mỗi lần lên cấp) — chưa làm.
- `role_history` cập nhật dần theo nhịp lưu (11:2x mới 468 bot lưu sau khi nâng) nên `kiem_skill_bot_0709.py` còn thấy một phần bot chưa 30 — đọc `bot.log` là chuẩn.
- Khe `.moi` sau đó thuộc phiên LNCK (a71d3305 / 90eeaa8d, main bc1a8224 chứa 962a8255).

## 5. Còn mở (chờ chủ)

- Đường Môn 4 chiêu ám khí khác hệ (347/343/345/349) ở cấp 0: cho nâng không?
- Các chiêu bị động khác (X pháp cấp 10, Dịch Cân Kinh 21, Tiêu Diêu Công 360, Huyền Thiên Vô Cực 630, ...): giữ 0 hay nâng?
- Bot `LeDuy125` chưa vào phái (chỉ có chiêu 365/368).
