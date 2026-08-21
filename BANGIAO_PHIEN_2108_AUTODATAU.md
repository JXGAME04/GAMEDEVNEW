# BÀN GIAO PHIÊN 21/08 — AUTO DÃ TẨU (WAuto): "phù về không bán rác" + món khoe cất rương

> **ĐỌC FILE NÀY TRƯỚC KHI GÕ.** Chi tiết cơ chế: `BANGIAO_AUTO_DATAU_WAUTO.md` **mục 13.15**.
> Phiên trước (20/08, danh bạ sạp / Thần Hành Phù): `BANGIAO_PHIEN_2008_AUTODATAU.md`.

---

## 0. 🔴 LUẬT CHỦ GAME CHỐT TRONG PHIÊN NÀY (đọc trước khi sửa bất cứ gì về túi/bán rác)

1. **Không tự ý sửa cấu trúc auto.** Sửa lỗi = dựa vào luồng WAuto có sẵn. Chủ game đã bác
   toàn bộ: bán-tại-chỗ trên map luyện, khóa TP "Đầy hành trang", phù tự chế, guard cất đồ… (đã gỡ).
2. **"Full rương" = hành trang đầy theo mức người chơi chọn ở tab Cơ bản** ("Đầy hành trang" +
   combo cỡ đồ `bCheckTPIBox/nTPiboxSel`). Auto KHÔNG được đặt ngưỡng riêng.
3. **Phần bán rác đã có bộ lọc sẵn** (tab Hậu cần "Bán rác" + combo *Bán giữ lọc đồ / Bán hết
   đồ* `nSelSell`; luật giữ theo danh sách **Lọc** tab Nhặt đồ `nFtMagic`; "Giữ nhẫn/dây/bội cấp >").
   Chỉ dùng bộ lọc này.
4. Luồng mong muốn: **phù về (túi đầy) → kiểm tra có rác → chạy Hậu cần (nó tự bán rác) → rồi
   mới chạy Dã Tẩu.** Đang treo thì: phù về có rác → bán → chạy Hậu cần như cũ.

## 1. TL;DR — trạng thái lúc bàn giao (21/08 ~11:25)

| Việc | Trạng thái |
|---|---|
| (1)(2) Phù về không bán mà tới Xa Phu → lên map → phù về lại (cả lúc treo xong lẫn đang làm) | ✅ sửa theo luồng Hậu cần (mục 3) + **sửa lỗi thật trong bước bán rác Hậu cần** (mục 4) — **chưa test thật** |
| (3) Khoe đồ xong: gửi món vào rương thay vì bán | ✅ xong — **chưa test thật** |
| Phần cấu trúc tự thêm ở `f5a38c40`/`adc57b19`/`8bcd1772` | ✅ **đã gỡ** trong `d8d27119` |

### Việc phải làm đầu phiên sau
1. **Thoát game vào lại** — `Game.exe` chạy từ 09:20 (bản 09:20). Bản mới **11:19** (`d8d27119`).
2. GameServer đã restart 09:33 (phiên khác) ⇒ `EndGiveBox` (hoàn món khoe) đang sống.
3. Test theo mục 6.

## 2. Binary / commit

| Tệp | Mốc | Ghi chú |
|---|---|---|
| `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\CoreClient.dll` | **21/08 11:19** (2.213.888 B, md5 `d043c045…`) | `d8d27119`; CRT-TINH ĐÚNG |
| bản lùi | `CoreClient_cu_2108_0920.dll` (sạch, trước phiên) · `CoreClient_cu_2108_1056.dll` (`8bcd1772`, **đừng dùng** — còn cấu trúc bị bác) | |
| `Game.exe` / `WAuto.exe` / `CoreServer.dll` | không đổi (`autoData` vẫn 6888 B) | |

```
f5a38c40 / adc57b19 / 8bcd1772  (vòng 1-3: có phần cấu trúc bị chủ game bác — KHÔNG lùi về đây)
d8d27119  r4: gỡ cấu trúc tự thêm; phù về có rác → chạy Hậu cần rồi Dã Tẩu; sửa vòng bán rác Hậu cần
```
Chỉ `Sources/Core/Src/CoreShell.cpp`. Build: `MSBuild Sources/Core/Core.vcxproj "-p:Configuration=Client
Release" -p:Platform=Win32 -m` tại D → chép tay `ClientRelease\CoreClient.dll` sang `bin\client` →
`ReverseTools\re_pe_crt.py`.

## 3. Gốc bệnh (1)(2) và cách sửa còn lại

**Gốc:** phù "Đầy hành trang" (tab Cơ bản, `ATYPE_TP_FULLITEM`) bắn giữa `DTP_FARM`; về thành, FARM
thấy `nMap != nDTMapId` → coi là "bị đá khỏi map" → `EXEC → GOXAFU` → lên lại map. Không ai bán: Hậu
cần bị chặn khi máy DT cầm lái (`nDT != 0`), còn `DTP_RETURN` (nơi r3 đặt "về thành bán trước") không
nằm trên đường này. Treo xong cũng rơi vào đây (hết hạn treo trên map nhiệm vụ → `IDLE → FARM`).

**Sửa (FARM, nhánh "bị ra khỏi map"):** chờ 1,5 s (fight-mode của mình đồng bộ sau id map) → đang ở
**thành** (thành Dã Tẩu hoặc map không fight-mode — phù về có thể rơi vào thôn không có Dã Tẩu) và
(**có rác** theo `DT_LaRac` **hoặc** hành trang **vẫn đầy theo mức tab Cơ bản** `DT_TuiDayTP`):
- bật "Về thành" (`bReturn`) → **`DTP_YIELD`**: nhả máy cho Hậu cần chạy **bước 0-8** (bán rác / mở
  rương / rút / sửa / cất / mua bình / giữ tiền) theo cấu hình của nó; tới **bước 9** (sắp ra Xa Phu lên
  map luyện công) hoặc hết 5' / đổi map / fight-mode → lấy lại máy → `EXEC → GOXAFU`. Giãn 2 lần nhường
  ≥ 3' (`DT_YIELD_GAP`, chống ping-pong).
- chưa bật "Về thành" nhưng bật "Bán rác" → `DTP_SELLJUNK` bán **hết** rác bằng chính bộ lọc
  (`g_nDTSellNeed = 999`) rồi `nDTBackXaFu` → Xa Phu.
- không dọn được (tắt cả hai, hoặc vừa nhường xong vẫn đầy) → treo 15' có lời.

## 4. Lỗi thật trong bước "bán rác" của Hậu cần (gốc "đôi lúc có rác mà không bán")

`ATYPE_RETURN` bước 1 quét túi theo hàng; `nSelIdx` = món **cuối** hàng đạt điều kiện; gặp món **giữ
lại** theo bộ lọc (nhẫn/dây/bội cấp cao, dòng trong danh sách Lọc, +all skill) thì `nSelIdx = 0; continue;`
→ **xóa luôn món rác đứng trước trong cùng hàng** → hàng đó không bao giờ bán. Một món giữ cỡ 2×4 ở cột
cuối chặn 4 hàng. Sửa: **một bộ lọc `DT_LaRac`** (y nguyên luật cũ) dùng chung cho Hậu cần bước 1 và
`DTP_SELLJUNK`, chọn món rác **đầu tiên**.

## 5. Món khoe (loại 3) → rương (yêu cầu 3)

Ghi ID **lúc đặt vào hộp giao** (`g_dwDTKhoePend`; ghi sau nút OK là vô dụng — nhịp OK đi nhánh "bấm OK
rỗng" vì item đã ở `pos_affairitem`) → thấy rương thưởng **và món đã về túi** (`DT_KhoeXong`, server
`EndGiveBox` hoàn trước khi bung thưởng) → đầu `DTP_GOTONPC`: `DT_CatKhoe` cất vào rương (`DT_BagToBox`,
rương chính → mở rộng theo `nSelStore`; cần rương mở / mật khẩu tab Hậu cần; 12 nhịp; không cất khi đang
đi trả / món là món đã chốt). `DT_IsQuestItem` cấm bán món đang chờ cất.

## 6. Checklist test (sau khi thoát game vào lại)

1. Tab Cơ bản bật "Đầy hành trang" + chọn cỡ; tab Hậu cần bật "Về thành" + "Bán rác" (chọn *Bán giữ lọc
   đồ* nếu có danh sách Lọc); phù về trong túi.
2. Loại 4: đánh tới đầy túi → phù về → phải thấy `Phù về thành có rác - để Hậu cần bán rác/dọn túi xong sẽ
   chạy Dã Tẩu tiếp.` → Hậu cần bán/mua → `Hậu cần dọn xong - Dã Tẩu làm tiếp.` → ra Xa Phu → lên map.
   **HỎNG:** về thành rồi đi thẳng Xa Phu không có dòng nào.
3. Túi có rác + món giữ theo Lọc nằm cuối hàng → Hậu cần (cả khi treo) phải bán hết rác, giữ đúng món lọc.
4. Loại 3 trả xong → `Khoe xong - cất món khoe vào rương...` → `Đã cất món khoe vào rương`.
5. HỎNG chung: dòng lặp > 2 lần/giây; loop Xa Phu ↔ map quá 2 vòng.

## 7. Bản đồ mã (`d8d27119`)

| Thành phần | Vị trí CoreShell.cpp |
|---|---|
| Statics (`g_dwDTKhoe*`, `g_nDTSell*`, `g_uDTYieldT`, `DT_YIELD_GAP`, `g_nDTLastMap/g_uDTMapT`) | ~3240-3252 |
| `DT_LaRac` / `DT_CoRac` / `DT_TuiDayTP` | ~3300-3380 (sau `DT_IsQuestItem`) |
| `DT_BagToBox` / `DT_ChestRoomFor` / `DT_KhoeXong` / `DT_CatKhoe` | sau `DT_EnsureUnlock` |
| `DT_SellStart` / `DT_Yield` | trước `DT_BagRelease` |
| FARM "bị ra khỏi map" | ~5200-5245 |
| `case DTP_YIELD` | trước `case DTP_MUASAP` |
| Hậu cần bước 1 (bán rác) | ~9753 |

Phản biện: vòng 1-2 (agent Opus) đã chạy trên bản cũ; **r4 chưa được agent soi lại** — test thật trước.

*Ghi 21/08/2026 ~11:25.*
