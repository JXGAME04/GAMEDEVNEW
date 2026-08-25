# GHI CHÚ PHỐI HỢP 25/08 ~09:20 — TÍN SỨ đang hỏng trên server test + 2 vá mới của phiên Tín Sứ

Phiên viết: phiên Tín Sứ/PORT5 (667a822b). Chủ game vừa test Tín Sứ lúc 08:50-09:00 và báo hỏng.

## Chủ game gặp gì (log đối chiếu `bin\server\ScriptError.log`)
1. Vào ải 3 (map 395): quái giữ rương **không đánh được, quái cũng không đánh người**, **không mở được rương**.
2. Bấm menu Dịch Quan / Tiêu Trấn **không phản ứng** — ScriptError 08:52 + 09:00:
   `(trap_qianbaoku.lua) cFuncName:(ture_try_starttask)` / `(ture_movecity)` = nil.
3. 36 lỗi `(turebug90.lua) OnRevive` + **160 lỗi `(kill_level.lua) OnRevive`** mỗi lần boot.

## Việc PHIÊN TÍN SỨ đã làm (KHÔNG đụng file của phiên 3HD)
- Vá **3 file trap** (`trap_qianbaoku/shanshenmiao/fengzhiqi.lua`): Include 2 script NPC của ải
  vào trap. Gốc: JX1 mỗi file một lua_State — trap chạy chiếm "current script" của player,
  menu NPC đang mở bấm vào gọi hàm trong state trap → nil. Linux single-state nên không dính.
- `turebug90.lua`: thêm `OnRevive()` **rỗng** (engine JX1 gọi vô điều kiện KNpc.cpp:8676; bản
  Linux không có hàm này). Backup: `*.truoc_trapfix_2508`.
- Các vá này CHỜ RESTART GS mới ăn (state cũ còn trong RAM).

## ⚠️ GỬI PHIÊN 3 HOẠT ĐỘNG (săn boss / PLĐ / vượt ải)
1. **ĐỪNG để ai restart GS trước khi swap DLL mới**: `killbosshead.lua` bản 08:55 đã trỏ
   `HD3_AddNpc`, nhưng `CoreServer.dll` đang chạy (build 08:38) **chưa có hàm này** (strings = 0).
   Restart bây giờ ⇒ `tinsu_addnpc()` lúc boot gọi `add_alltollgatenpc()` → nil ngay dòng đầu
   ⇒ **toàn bộ NPC Tín Sứ (kể cả Tiêu Trấn/Dịch quan 3 ải) không sinh** — Tín Sứ chết hẳn,
   nặng hơn hiện tại.
2. Triệu chứng "quái không đánh được" của chủ game hôm nay khớp đúng phân tích A7-C1 của các
   bạn (camp bị ghi đè → ally). Khi các bạn swap DLL có `HD3_AddNpc` + restart là hết —
   **báo lại phiên Tín Sứ hoặc chủ game để test lại sau restart đó.**
3. `kill_level.lua` (các bạn đang giữ, sửa 08:57) còn **160 lỗi OnRevive nil mỗi boot** — gợi ý
   thêm `function OnRevive(npcindex) end` rỗng như turebug90, hoặc xử trong đường sinh NPC.
4. Phiên Tín Sứ **không** đụng: killbosshead.lua, kill_level.lua, hd3_driver.lua, DLL, và
   không restart GS. Cả 3 file trap + turebug90 là file port Tín Sứ 21/08 (mtime cũ, không ai giữ).

## Mã chết ghi nhận (KHÔNG vá — giống hệt Linux)
- `posthouse.lua`: menu `/messenger_getlevel` bị comment từ gốc ở CẢ Linux; 9 nút
  "Quay lại/messenger_getlevel" nằm trong 4 hàm `messenger_level*` không còn đường vào.
