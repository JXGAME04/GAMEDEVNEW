# BÀN GIAO PHIÊN 21/08 — AUTO DÃ TẨU (WAuto): hết vòng "phù về không bán – Xa Phu – lên map – phù về lại" + món khoe cất rương

> **ĐỌC FILE NÀY TRƯỚC KHI GÕ.** Cơ chế chi tiết: `BANGIAO_AUTO_DATAU_WAUTO.md` **mục 13.15**.
> Phiên trước (20/08, danh bạ sạp / Thần Hành Phù): `BANGIAO_PHIEN_2008_AUTODATAU.md`.

---

## 0. TL;DR — trạng thái lúc bàn giao (21/08 ~11:10)

| Việc | Trạng thái |
|---|---|
| (1) Treo xong → về thành full túi không bán, tới Xa Phu, lên map, phù về lại | ✅ sửa gốc (cùng cơ chế với (2)), **chưa test thật** |
| (2) Đang làm nhiệm vụ, full túi phù về không bán, lặp như trên | ✅ sửa gốc, **chưa test thật** |
| (3) Khoe đồ xong: gửi món vào rương thay vì bán | ✅ xong, **chưa test thật** |
| Phản biện bản vá (agent Opus đọc mã thật) | ✅ vòng 1: 7 CONFIRMED đã vá (`adc57b19`); ✅ vòng 2 (soi chính `adc57b19`): 2 nặng + 2 vừa đã vá (`8bcd1772`); ❌ vòng 3 (soi `8bcd1772`) **chưa chạy** |
| Lỗi tiềm ẩn phát hiện thêm: mốc set "cần ít nhất N ô" → SELLJUNK lặp vô tận | ✅ vá luôn |

### 🔴 VIỆC PHẢI LÀM ĐẦU PHIÊN SAU

1. **Thoát game vào lại** — `Game.exe` đang chạy từ **09:20** (nạp `CoreClient.dll` 09:20 của phiên
   bot). Bản mới là **10:56** (`8bcd1772`). Chưa vào lại thì mọi thứ ở đây **chưa có trong RAM**.
2. GameServer **đã** được restart 09:33 hôm nay (phiên khác) ⇒ danh bạ sạp, `lbhtdatau.lua`,
   `seasonnpc.lua` có `EndGiveBox` (hoàn món khoe về túi) **đang sống**. Không cần restart cho phiên này
   (sửa chỉ ở client).
3. Test theo checklist mục 6, đặc biệt nhìn dòng `Túi gần đầy khi đang đánh quái - bán bớt rác tại chỗ`
   và `Khoe xong - cất món khoe vào rương`.

---

## 1. Binary & deploy

| Tệp | Vị trí | Mốc | Ghi chú |
|---|---|---|---|
| `CoreClient.dll` | `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\` | **21/08 10:56** (2.215.936 B, md5 `022bd192…`) | `8bcd1772`; `re_pe_crt.py` = CRT-TINH ĐÚNG, cả bảng khớp |
| `Game.exe` | cùng chỗ | 21/08 05:45 (phiên khác) | không đổi |
| `WAuto.exe` | cùng chỗ | 19/08 22:23 | không đổi; `autoData` vẫn 6888 B — **không đụng ipc** |
| `CoreServer.dll` | `...\bin\server\` | 21/08 10:10 (phiên bot) | phiên này **không** sửa server |

Bản lùi: `CoreClient_cu_2108_0920.dll` (bản 09:20 trước phiên — lùi về đây nếu cần),
`CoreClient_cu_2108_1043.dll` (`adc57b19` — **đừng dùng**, còn R2-1 lặp vô hạn).

Build: `MSBuild Sources/Core/Core.vcxproj "-p:Configuration=Client Release" -p:Platform=Win32 -m` tại
`D:\GAMEDEVNEW` → artifact `Sources\Core\ClientRelease\CoreClient.dll` (post-build chỉ chép vào
`D:\GAMEDEVNEW\bin\client`, **không** đụng cây E) → chép tay sang `bin\client` (đổi tên file cũ vì game
đang giữ) → `ReverseTools\re_pe_crt.py <bin\client>`.

## 2. Commit

```
f5a38c40  auto Da Tau 21/08: het lap "phu ve khong ban - toi xa phu - len map - phu ve lai" + mon khoe cat ruong
adc57b19  r2: phan bien ban va - 1 nang (mon khoe khong bao gio duoc ghi ID) + 6 vua/nhe
8bcd1772  r3: phan bien vong 2 (soi adc57b19) - 2 nang + 2 vua
```
Chỉ `Sources/Core/Src/CoreShell.cpp`. Đã push. (Cây D có **3 phiên song song** sáng nay: bot
`98dbe69c`, Goddess/MySQL `a93882aa`, va chạm `350506ca` — đừng lẫn; `Lib/debug/engine.lib` M là của
phiên khác.)

## 3. Gốc bệnh (tóm tắt — chi tiết 13.15 A)

TP *"Về thành khi túi đầy"* (`ATYPE_TP_FULLITEM`) **không bị khóa theo máy Dã Tẩu** → bắn giữa
`DTP_FARM` → về thành, FARM thấy `nMap != nDTMapId` → "bị đá khỏi map" → `EXEC → GOXAFU` → lên lại
map → TP bắn tiếp. Không ai bán: Hậu cần bị chặn (nDT≠0), `DTP_RETURN` (nơi có "về thành thì bán
trước") không nằm trên đường này. Treo xong cũng rơi vào đường này (hết hạn treo trên map nhiệm vụ →
`IDLE → FARM`). FARM còn **không hề kiểm túi**.

## 4. Đã sửa (chi tiết 13.15 B)

1. FARM < 5 ô → bán tại chỗ (`DTP_SELLJUNK`, vẫn đánh); tắt "Bán vật phẩm" → phù về nhờ Hậu cần /
   treo có lời.
2. Cổng TP túi đầy: chặn khi máy DT đang cầm lái **và** vừa chạy trong nhịp (`g_uDTTickT`).
3. Pha mới **`DTP_YIELD`**: bị đưa về **thành** (thành Dã Tẩu hoặc bất kỳ map không fight-mode —
   phù về có thể rơi vào thôn không có Dã Tẩu; chờ 1,5 s sau đổi map cho fight-mode đồng bộ) giữa
   FARM (TP nào cũng vậy / chết / bấm tay) + bật "Về thành" → nhường Hậu cần bước 0-8, lấy lại máy
   trước bước 9 (Xa Phu của nó). Khoảng cách 2 lần nhường `DT_YIELD_GAP` = 3 phút. Ở thành mà túi
   vẫn < 5 và không còn cách dọn → treo 15' có lời (không lên lại map cho lặp).
4. SELLJUNK hết rác vẫn chật → YIELD (ở thành) / phù về rồi YIELD (ngoài thành) / treo như cũ.
5. Mốc set "cần ít nhất N ô" → `g_nDTSellMin/Need` theo N (kẹp ≤ 40).
6. Món khoe loại 3: ghi ID **lúc đặt vào hộp** → rương thưởng **và món đã về túi** = xong →
   `DT_CatKhoe` cất rương ở đầu `DTP_GOTONPC` (cần rương mở / mật khẩu tab Hậu cần; theo
   `nSelStore`; không rút hộp giao, không cất khi đang đi trả / món là món đã chốt). `DT_IsQuestItem`
   cấm bán món đang chờ cất.
7. Hậu cần bước 5 "cất đồ" giữ lại: món đã chốt nộp, món khoe DT tự cất, và (chỉ khi tắt ô "lấy từ
   rương") ứng viên theo luật (`DT_GiuTrongTui`).

## 5. Phản biện

**Vòng 1 (soi `f5a38c40`) — 7 CONFIRMED, đã vá trong `adc57b19`:**

| # | Mức | Lỗi | Vá |
|---|---|---|---|
| F1 | NẶNG | `g_dwDTKhoePend` ghi SAU nút OK, nhưng nhịp OK thực tế đi nhánh "bấm OK rỗng" (item đã ở `pos_affairitem` nên guard đầu GIVEBOX rẽ) → **không bao giờ ghi** → tính năng cất rương = no-op | ghi ngay nhịp đặt item vào hộp |
| F2 | vừa | `DT_CatKhoe` chạy cả khi đang đi trả nhiệm vụ kế; cùng món có thể được chốt cho nhiệm vụ khoe kế | bỏ qua khi `nDTStep==TURNIN` / món == `nDTItemIdx` |
| F3 | vừa | Hậu cần bước 5 cất CẢ item nhiệm vụ đang làm vào rương | thêm `DT_IsQuestItem` |
| F4 | vừa | FARM < 5 ô mà tắt "Bán vật phẩm" → rơi vào SELLJUNK = treo 15' mỗi lần túi đầy | phù về nhờ Hậu cần / treo có lời |
| F5 | vừa | các TP khác (tiền/đồ hỏng/thuốc) + khoảng nhường 10' → vòng "Xa Phu – lên map – TP" suốt 10' | 3' |
| F7 | nhẹ | N ≤ 60 → ngưỡng 62 > túi 60 ô | kẹp 40 |
| F8 | nhẹ | dead store `nDTBackXaFu` (phù về rồi YIELD xóa) | để nguyên, vô hại |

**Vòng 2 (soi chính `adc57b19`) — 2 NẶNG + 2 vừa, đã vá trong `8bcd1772`:**

| # | Mức | Lỗi | Vá |
|---|---|---|---|
| R2-1 | NẶNG | FARM < 5 ô + tắt "Bán vật phẩm" → phù về rơi vào **thôn không có Dã Tẩu** → nhánh "bị ra khỏi map" không coi là thành → `EXEC→GOXAFU` → lên map → phù về lại, **vô hạn** đốt phù (yield-timer không đổi, watchdog stall bị re-stamp) | "thành" = DT-town ∨ !fight-mode (chờ 1,5 s sau đổi map); ở thành mà < 5 ô và hết cách → treo 15' có lời |
| R2-2 | NẶNG | guard bước 5 theo **lớp luật** (`DT_IsQuestItem`): khoe dòng phổ biến → cả túi khớp → Hậu cần không cất gì → nhường máy vô ích | `DT_GiuTrongTui` (món đã chốt + món khoe + ứng viên chỉ khi tắt lấy-từ-rương) |
| R2-3 | vừa | cửa sổ thưởng TRỄ của nhiệm vụ trước tới lúc món khoe mới còn trong hộp giao → chốt nhầm → `DT_CatKhoe` rút món khỏi hộp đang nộp | chốt chỉ khi món đã ở `pos_equiproom`; `DT_CatKhoe` không rút hộp giao; guard món==`nDTItemIdx` trước guard TURNIN |
| R2-4 | vừa | `return 2` khi chờ phù giữ cổng TP đóng suốt vòng R2-1 | đóng cùng R2-1 (treo = nhả máy) |
| R2-5 | nhẹ | F4 không gọi `DT_PortalPull` | bỏ qua — V07: ngoài thành không kéo rương được |
| R2-6 | nhẹ | FAILREQ xóa `g_dwDTKhoeId` của nhiệm vụ trước | bỏ xóa |

**Vòng 3 (soi `8bcd1772`): chưa chạy** — ưu tiên test thật trước.

## 6. Checklist test (sau khi thoát game vào lại)

1. Bật auto Dã Tẩu + tab Hậu cần bật **"Về thành"** và **"Bán vật phẩm"** (có mật khẩu rương nếu
   muốn cất rương); để phù về (6,1,437 hoặc Thổ Địa Phù) trong túi.
2. Nhận nhiệm vụ loại 4 (Địa đồ/Mật chỉ), để túi đầy dần khi đánh → phải thấy
   `Túi gần đầy khi đang đánh quái - bán bớt rác tại chỗ rồi đánh tiếp.` rồi
   `Túi đã có chỗ trống - quay lại làm Dã Tẩu.` — **không** phù về.
3. Ép kịch bản cũ: đang FARM, tự bấm phù về → tới thành phải thấy `Bị đưa về thành giữa lúc đánh
   quái - để Hậu cần bán/cất đồ...` → Hậu cần bán/cất/mua → `Hậu cần dọn xong - Dã Tẩu làm tiếp.` →
   ra Xa Phu → lên map. **Dấu hiệu hỏng:** về thành rồi đi thẳng Xa Phu không có dòng nào ở trên.
4. Nhận nhiệm vụ loại 3 (Tìm trang bị/khoe) có đồ → trả xong phải thấy `Khoe xong - cất món khoe vào
   rương...` rồi `Đã cất món khoe vào rương - ...`; mở rương kiểm tra món. Rương khóa không mật khẩu
   → dòng vàng nhắc nhập mật khẩu tab Hậu cần.
5. Dấu hiệu HỎNG chung: dòng nào lặp > 2 lần/giây; đứng im > 1' ở thành mà không có dòng
   `Hậu cần...`; loop Xa Phu ↔ map quá 2 vòng.

## 7. Bản đồ mã mới

| Thành phần | Vị trí (CoreShell.cpp, `adc57b19`) |
|---|---|
| Statics mới (`g_dwDTKhoe*`, `g_uDTTickT`, `g_nDTSell*`, `g_uDTYieldT`, `DT_YIELD_GAP`, `g_uDTSellPortalT`, `g_nDTLastMap/g_uDTMapT`) | ~3240-3254 |
| `DT_GiuTrongTui` (guard bước 5 Hậu cần) | ngay trước `DT_EnsureUnlock` (~3300) |
| `DT_BagToBox` / `DT_ChestRoomFor` / `DT_KhoeXong` / `DT_CatKhoe` | ~3340-3470 (sau `DT_EnsureUnlock`) |
| `DT_SellStart` / `DT_Yield` | ngay trước `DT_BagRelease` (~3930) |
| SELLJUNK ngưỡng + hết rác | ~4060-4200 |
| FARM: về thành giữa chừng / kiểm túi | ~5170-5250 |
| GIVEBOX ghi pend | ~5410 |
| `case DTP_YIELD` | ngay trước `case DTP_MUASAP` |
| Cổng TP | `case ATYPE_TP_FULLITEM` block (~7560) |
| Hậu cần bước 5 guard (`DT_GiuTrongTui`) | ~9984 |

*Ghi 21/08/2026 ~11:10.*
