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

## 1. TL;DR — trạng thái lúc bàn giao (21/08 ~11:40)

| Việc | Trạng thái |
|---|---|
| (1)(2) Phù về không bán mà tới Xa Phu → lên map → phù về lại (cả lúc treo xong lẫn đang làm) | ✅ sửa theo luồng Hậu cần có sẵn (mục 3) — **chưa test thật** |
| (3) Khoe đồ xong: gửi món vào rương thay vì bán | ✅ xong — **chưa test thật** |
| Phần cấu trúc tự thêm ở `f5a38c40`/`adc57b19`/`8bcd1772` | ✅ **đã gỡ** trong `d8d27119` |
| Hai vòng lọc bán rác (Hậu cần bước 1 + `DTP_SELLJUNK`) | ✅ **trả nguyên trạng** trong `78102641` — chủ game đã test, không đụng nữa |
| Phân tích phần LƯU cấu hình của Auto | ✅ xong (mục 8) — tự lưu đủ, còn 2 khe hở nhỏ, **chưa sửa (chờ ý chủ game)** |

### Việc phải làm đầu phiên sau
1. **Thoát game vào lại** — `Game.exe` chạy từ 09:20 (bản 09:20). Bản mới **11:25** (`78102641`).
2. GameServer đã restart 09:33 (phiên khác) ⇒ `EndGiveBox` (hoàn món khoe) đang sống.
3. Test theo mục 6.

## 2. Binary / commit

| Tệp | Mốc | Ghi chú |
|---|---|---|
| `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\CoreClient.dll` | **21/08 11:25** (2.215.424 B, md5 `b6f04852…`) | `78102641`; CRT-TINH ĐÚNG |
| bản lùi | `CoreClient_cu_2108_0920.dll` (sạch, trước phiên) | |
| `Game.exe` / `WAuto.exe` / `CoreServer.dll` | không đổi (`autoData` vẫn 6888 B) | |

```
f5a38c40 / adc57b19 / 8bcd1772  (vòng 1-3: có phần cấu trúc bị chủ game bác — KHÔNG lùi về đây)
d8d27119  r4: gỡ cấu trúc tự thêm; phù về có rác → chạy Hậu cần rồi Dã Tẩu
78102641  r5: trả nguyên trạng 2 vòng lọc bán rác; DT_LaRac hạ xuống CHỈ-ĐỂ-ĐỌC
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

## 4. 🟠 BÁO CÁO (KHÔNG SỬA): nghi vấn trong bước "bán rác" của Hậu cần

Chủ game chốt: *"phần lọc đồ bán và lọc đồ khi nhặt đã test rồi, đừng đụng và fix lại"* ⇒ **đã trả
nguyên trạng**, chỉ ghi lại để chủ game tự quyết:

`ATYPE_RETURN` bước 1 quét túi theo hàng, `nSelIdx` = món **cuối** hàng đạt điều kiện. Gặp món **giữ lại**
theo bộ lọc (nhẫn/dây/bội cấp cao, dòng trong danh sách Lọc, +all skill) thì làm `nSelIdx = 0; continue;`
→ **xóa luôn món rác đứng trước trong cùng hàng** ⇒ hàng đó không bán ở lượt này. Lượt sau (mỗi 300 ms)
quét lại nên **vẫn bán được** khi món giữ đã đứng ở vị trí khác; chỉ kẹt hẳn nếu món giữ luôn nằm sau món
rác trong mọi hàng. `DTP_SELLJUNK` của Dã Tẩu không có nét này (nó `break` ngay khi chọn được món).
Nếu chủ game muốn sửa: đổi thành "chọn món rác **đầu tiên**" (1 dòng) — nói một tiếng tôi làm.

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

## 7. Bản đồ mã (`78102641`)

| Thành phần | Vị trí CoreShell.cpp |
|---|---|
| Statics (`g_dwDTKhoe*`, `g_nDTSell*`, `g_uDTYieldT`, `DT_YIELD_GAP`, `g_nDTLastMap/g_uDTMapT`) | ~3240-3252 |
| `DT_LaRac` / `DT_CoRac` / `DT_TuiDayTP` | ~3300-3380 (sau `DT_IsQuestItem`) |
| `DT_BagToBox` / `DT_ChestRoomFor` / `DT_KhoeXong` / `DT_CatKhoe` | sau `DT_EnsureUnlock` |
| `DT_SellStart` / `DT_Yield` | trước `DT_BagRelease` |
| FARM "bị ra khỏi map" | ~5200-5245 |
| `case DTP_YIELD` | trước `case DTP_MUASAP` |
| Hậu cần bước 1 (bán rác) | ~9753 — **nguyên trạng, không đụng** |

Phản biện: vòng 1-2 (agent Opus) chạy trên bản cũ; **r4/r5 chưa được agent soi lại** — test thật trước.

## 8. Phân tích phần LƯU cấu hình của Auto (theo yêu cầu chủ game)

**Kết luận: WAuto đã tự lưu gần như MỌI thay đổi, ngay khi chỉnh.** Cách hoạt động:

| Thành phần | Chi tiết |
|---|---|
| Nơi lưu | `<thư mục WAuto>\APdata\<dwPID>.dat` — ghi **nguyên struct `autoData`** (hiện 6888 B). Thư mục đang dùng: `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\APdata` (216 tệp). |
| `dwPID` là gì | **ID nhân vật trên server** (`KProtocolProcess.cpp:1764: s.dwPID = Player[CLIENT_PLAYER_INDEX].m_dwID`), KHÔNG phải id tiến trình ⇒ mỗi nhân vật một tệp, giữ nguyên qua các lần thoát/vào game. |
| 2 hàm ghi | `SaveRoleData(hDlg, gnode)` = **đọc toàn bộ 154 control** trên hộp thoại → `apdata` → ghi tệp. `SaveRoleDataFast(gnode)` = ghi thẳng `apdata` (dùng sau khi sửa danh sách trong bộ nhớ). |
| Kích hoạt lưu | ① mọi **checkbox**: `BN_CLICKED` (khối `case` ~4220-4307) → `SaveRoleData`; ② mọi **combo**: `CBN_SELCHANGE` (~4986-5015); ③ mọi **ô nhập**: `EN_KILLFOCUS` (~4091-4216); ④ **nút danh sách** (lọc dòng ma, tọa độ, tổ đội, không-nhặt-theo-tên, sắp xếp acc) → `SaveRoleDataFast`; ⑤ phím tắt gán PK (subclass, dòng 3287); ⑥ bật/tắt PK bằng phím nóng (2352). |
| Đối chiếu | So bộ control **đọc khi lưu** với bộ control **kích hoạt lưu**: chỉ 1 mục lệch — `IDC_EDITOR_7_K` (ô phím PK) — nhưng nó lưu qua đường ⑤, nên **không sót**. Tab 8 (Dã Tẩu): 11 checkbox qua ①, 4 combo qua ②, 2 ô nhập qua ③ — đủ. |
| Nạp | `LoadRoleData`: tệp cũ nhỏ hơn struct thì đọc phần có + **di trú 2 bậc theo `offsetof`** (khối Dã Tẩu / 3 ô 20/08) rồi đặt mặc định — không mất lựa chọn cũ. Chưa có tệp → tạo mặc định và ghi ngay. |

**2 khe hở nhỏ (chưa sửa, chờ ý chủ game):**
1. **Ô nhập chỉ lưu khi rời ô** (`EN_KILLFOCUS`). Gõ số rồi **đóng WAuto ngay** (dấu ×/Alt+F4) mà con trỏ
   còn trong ô đó → giá trị vừa gõ **mất** (`WM_CLOSE` chỉ gọi `UnInit()` rồi `EndDialog`, không lưu).
   Sửa nếu muốn: gọi `SaveRoleData` một lần trong `WM_CLOSE`.
2. **Ghi đè trực tiếp** (`fopen(...,"wb+")` → `fwrite` → `fclose`): mất điện/kill giữa chừng có thể làm
   tệp `.dat` cụt. Sửa nếu muốn: ghi tệp tạm rồi `MoveFileEx` đè.

**Ghi chú (không phải lỗi):** ô tick **bật/tắt auto** ở danh sách nhân vật và ô "ẩn game" là trạng thái
phiên (gửi IPC sang game), **cố ý không lưu** — mở lại WAuto thì auto không tự bật.

*Ghi 21/08/2026 ~11:40.*
