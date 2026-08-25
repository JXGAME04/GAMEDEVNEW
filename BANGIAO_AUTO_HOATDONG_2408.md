# BÀN GIAO — AUTO HOẠT ĐỘNG (BÁCH NHÂN + BANG CHIẾN) · WAuto tab thứ 12 · 24/08/2026 đêm

> Thi công theo `AUTO_HOATDONG_SPEC.md` (*"thiết kế Auto tự hoạt động các tính năng trên bản
> note - tham khảo mã nguồn auto có sẵn"*).
> **100% phía client** (WAuto.exe + Game.exe + CoreClient.dll), không đụng server.
> **CHƯA TEST THẬT** — binary `.moi_2408_liendau` đã build đè (mốc 20:17), chưa swap.

---

## 0. 🔴 Đính chính đặc tả + phạm vi đợt này

**Đặc tả mục 1.1 CHỈ NHẦM CÂY MÃ NGUỒN.** Nó bảo cây đúng là `_WAuto` (11.077 dòng) — đó là
cây của **dự án USVOLAM**, không phải dự án này (ký ức `wauto-canonical-tree`). Auto Tống
Kim / Liên đấu / Dã Tẩu của dự án này đều nằm ở **`E:\Src_Auto_Ngoai\WAuto\WAuto`** (UI) +
**`D:\GAMEDEVNEW\Sources`** (engine trong `CoreClient.dll`/`Game.exe`) — đợt này làm đúng cây
đó, "tham khảo mã nguồn auto có sẵn" = tái dùng hạ tầng Tống Kim/Liên đấu (đi thành, bấm
thoại, chọn địch, chống kẹt). Trường cuối struct cũng không phải `bAutoLD` như đặc tả ghi.

**Phạm vi đã làm — 2/4 hoạt động chạy được ngay:**

| Hoạt động | Trạng thái | Vì sao |
|---|---|---|
| **Bách Nhân Lôi Đài** | ✅ THI CÔNG XONG | ưu tiên #1 của đặc tả — cày exp mỗi ngày |
| **Bang Chiến** | ✅ THI CÔNG XONG | tới mùa (29/10) bật là chạy |
| **Tỷ Võ Đài** | ✋ CHƯA — thiết kế ở mục 5 | cần 2 phe người thật phối hợp + cơ chế trao SỐ THỨ TỰ giữa 2 client chưa kiểm chứng được khi chưa test tay |
| **Thành Bảo** | ⛔ CHẶN CỨNG | map 984 **thiếu dữ liệu client** — auto mà đưa người chơi vào là đen màn. Chờ chủ game cấp pak rồi làm |

## 1. Tab thứ 12 "H.động" (lưới nút tab 4×3 giờ dùng đủ 12 ô)

| Ô | Ý nghĩa | Mặc định |
|---|---|---|
| Bật auto Bách Nhân Lôi Đài | cày exp map 960 | **TẮT** |
| Vào lúc (giờ:phút) | theo giờ MÁY CHỦ + ô Lệch giờ | 12:05 |
| Cày (phút) | 0 = ở tới khi ăn đủ **50 lượt exp/ngày** | 0 |
| Chế độ | **Đứng ăn exp (an toàn)** / Tranh làm Lôi Chủ (x3 exp, đánh theo tab PK) | Đứng ăn |
| Đài | Tự chọn (đài 2 — né đài chủ) / Đài 1..5 | Tự chọn |
| Tự ăn buff Cổ Thủ | nghe loa → đi 15 điểm quen tìm NPC → x2 exp 30 phút | bật |
| Bật auto Bang Chiến | mùa 29/10-05/11 | **TẮT** |
| Đi lúc | cửa vào thật 20:30-21:30 | 20:25 |
| Lệch giờ | chung cho cả hai | 0 |

Cấu hình 11 trường mới **ở CUỐI `autoData`** + nhánh di trú `uOldSize <= offsetof(autoData,
bHDBachNhan)` — file `.dat` cũ không hỏng. Dòng trạng thái chân cửa sổ hiện đủ pha mới
(`Bách Nhân: đang cày kinh nghiệm`, `Bang Chiến: đang đánh trận`…).

## 2. Máy trạng thái (`HD_Process`, `ATYPE_HOATDONG`, sau TK/LD trong chuỗi nhường quyền `nBS`)

```
BÁCH NHÂN:
HDP_BN_GO  → chưa ở Lâm An thì LD_DiThanh (Thần Hành Phù / Xa Phu) → NPC "Quan nhắc nhở
             Hoàng Thành Tư" (1464,3223) → "Lôi Đài Hoàng Thành Tư" → "Ta muốn vào..."
             (bị "chưa tới giờ" → đợi 5' thử lại; "thiếu cấp" → bỏ)
             🔴 KHÔNG bao giờ chọn "Kết thúc đối thoại" — nó trỏ hàm lỗi (npc_enter.lua)
HDP_BN_IN  → map 960: đếm lượt exp cục bộ (5 phút/tick; buff +1, Lôi Chủ +2 — khớp
             "Lôi Chủ có buff tiêu 4 lượt/5 phút" của đặc tả); nếu task 2709 có sync và
             phần ngày khớp hôm nay thì tin số server
             · mode Đứng ăn: đứng gần điểm hồi sinh (1749,3135); 80 phút không có gì thì
               KHINH CÔNG (SendClientCmdJump) chạm đài rồi nhảy xuống — né bị đá 90 phút
             · mode Lôi Chủ: khinh công lên tâm đài đã chọn; có địch (relation enemy,
               người LẪN NPC cao thủ) → giao máy PK (trả 2); KHÔNG có địch → ĐỨNG YÊN
               trên đài (đạp trap lại = bỏ đài, mất chuỗi)
HDP_BN_BUFF→ loa "Cổ Thủ đã xuất hiện" → đi lần lượt 15 điểm drummer.txt, tìm NPC "Cổ Thủ"
             gần nhất → thoại → chọn dòng đầu → buff 30'
HDP_BN_OUT → đủ lượt / hết phút cày / loa "hôm nay kết thúc" → Xa phu (1750,3159) → về Lâm An

BANG CHIẾN:
HDP_BC_GO  → LD_DiThanh tới Ba Lăng (53) → NPC "Võ Lâm Truyền Nhân" (1628,3173) →
             tìm dòng "Ta muốn tham gia thi đấu" (khớp VĂN BẢN, không khớp chỉ số — pha
             khác thì huỷ, 60 giây bấm lại, tối đa hết cửa sổ 85 phút)
HDP_BC_SIGN→ map 608-613: đi tới Ô TRAP GẦN NHẤT trong 36 ô signup_trap.txt
             · "chưa bắt đầu"/"đang tập hợp" → 30 giây đạp lại (cửa thật 20:30)
             · bị đẩy về (1619,3175) im lặng 8 lần → "bang chưa có liên minh?" → bỏ
             · "chết đủ trần mạng"/"sai khu" → bỏ có thông báo
HDP_BC_FIGHT→ map 605-607: trả 2 — máy PK tab PK đánh; chọn địch bằng LD_ChonDich
             (mission group — server AddMSPlayer camp 1/2 nên phân biệt phe chuẩn)
             chết → tự hồi sinh (nBS bấm hộ) → đánh tiếp
             bị đẩy về map báo danh → đọc task 2370/2376: đủ trần mạng thì báo, KHÔNG
             🔴 bao giờ đạp lại trap (đạp lại khi đang trong trận = MẤT 1 MẠNG)
```

**Ưu tiên khi nhiều máy cùng bật**: Tống Kim > Liên đấu > Hoạt động (chuỗi `nBS` trong
`ExtAutoLoop`). Bang Chiến ưu tiên hơn Bách Nhân trong nội bộ tab (cửa sổ hẹp hơn).

## 3. Bám đúng 4 luật của đặc tả

1. **Không nhét cứng giờ/cấp**: giờ do người chơi nhập (server đổi `cauhinh_hoatdong.lua`
   thì sửa ô giờ); marker "chưa tới giờ"/"thiếu cấp" bắt bằng văn bản do server nói.
2. **Từ chối im lặng** xác nhận bằng **đổi map / vị trí** (trap Bang Chiến đếm số lần bị
   đẩy về ô 1619,3175; báo danh Bách Nhân xác nhận bằng sang map 960).
3. Mọi toạ độ/marker trích **byte-for-byte từ script sống** — `ReverseTools/
   gen_hoatdong_tables.py` → `KHoatDongTables.h` (5 đài + 4×5 điểm đẩy ra, 15 điểm Cổ Thủ,
   36 ô trap, 17 marker). Sửa thì chạy lại script.
4. Task client: 2709 (lượt exp) **được phòng hờ** — đếm cục bộ là chính vì đặc tả cảnh báo
   server đặt task không kèm sync; 2370/2376 Bang Chiến đặc tả xác nhận có sync nên đọc thẳng
   (`m_cTask.GetSaveVal`).

## 4. Nghiệm thu

**Bách Nhân (làm được NGAY hôm nay, 12:00-24:00):**
0. Swap 3 binary `.moi_2408_liendau` (thoát hẳn Game.exe + WAuto trước).
1. Tab "H.động" → bật Bách Nhân, giờ vào đặt phút hiện tại +2, Chế độ = Đứng ăn.
2. Auto tự đi Lâm An → NPC → vào map 960 → `[Hoạt động] Đã vào Lôi Đài Hoàng Thành Tư.` →
   đứng gần điểm hồi sinh; chân cửa sổ WAuto hiện `Bách Nhân: đang cày kinh nghiệm`.
3. Đợi loa Cổ Thủ (mốc phút 00/30) → auto chạy đi tìm → `Đã nhận buff Cổ Thủ x2 kinh nghiệm.`
4. Đặt "Cày (phút)" = 5 → hết 5 phút auto ra Xa phu → về Lâm An → trả máy.
5. Thử Chế độ = Tranh làm Lôi Chủ (đài 2): khinh công lên đài, có người khiêu chiến thì đánh.
6. **HỎNG cần báo**: chọn nhầm "Kết thúc đối thoại" (lỗi script server); đứng đấm tường
   không lên được đài; không tìm thấy Cổ Thủ dù loa nổ; quá nửa đêm không tự về.

**Bang Chiến (chỉ test được khi admin ép pha hoặc tới mùa 29/10):** bật ô, chỉnh giờ đi
khớp giờ admin mở; cần nhân vật **có bang thuộc liên minh**; xem nó qua Ba Lăng → thoại →
map báo danh → đạp trap → vào trận PK. Nhớ: **chạm trap là mất tổ đội** (luật server).

## 5. Thiết kế để lại cho 2 hoạt động chưa làm

**Tỷ Võ Đài** (khi nào muốn làm): khuôn giống tổ đội Liên đấu — thêm ô "Tự Tỷ Võ với bạn
diễn", 2 máy lập tổ đúng 2 người (tái dùng `LD_MoiParty`/`LD_NhanParty`), captain bấm NPC
"Công Bình Tử" (12 vị trí, 3 thành) chỉ số 0 → chọn `1 vs 1` (chỉ số = số người). Điểm phải
kiểm chứng bằng test tay trước khi code: sau khi mở trận, 2 đội trưởng vào đài bằng đường
nào (tự dịch chuyển hay phải nhập số) — thoại báo số nằm trong `cap.szDlg` bắt được, nhưng
hộp **nhập số** là `S2C_INPUT_BOX nType=2` (KUiGetNumber) — đã có sẵn kênh trả lời
(`SendClientCmdInputBox(2, ...)`, khuôn hộp nhập tên Liên đấu). Bẫy phải né: task 2340-2342
bị ghi đè mỗi lần bấm NPC; tổ 3 người là trượt; thông báo "không đem đủ ngân lượng" nghĩa
thật là "chờ trận cũ dọn xong".

**Thành Bảo**: mọi số liệu đã nằm trong đặc tả (NPC 1663,3262 Lâm An; điểm/lệnh bài; task
3399/4056-4061). Chặn duy nhất = **thiếu pak client map 984**. Khi có pak: khuôn = Bách Nhân
(canh Chủ nhật 17:00, vào map, đi theo tuyến cây, **đổi lệnh bài NGAY trong ngày** vì 0h
xoá điểm, kiểm ô túi trống vì lệnh bài không xếp chồng).

## 6. Tệp đã sửa (commit kèm)

- `Sources/Core/Src/KHoatDongTables.h` (MỚI - sinh tự động) + `ReverseTools/gen_hoatdong_tables.py` (MỚI)
- `Sources/Core/Src/CoreShell.cpp` — khối `AUTO HOAT DONG` (~720 dòng) + case `ATYPE_HOATDONG`
  + `WA_HoatDong` dời xuống sau khối HD (vì cần enum HDP_*)
- `Sources/Core/Src/CoreShell.h` — `ATYPE_HOATDONG`
- `Sources/Core/Src/KPlayer.h` — 20 trường `nHD*/uHD*` trong `ExtAuto`
- `Sources/Core/Src/ipc_shared.h` (+2 bản sao) — 11 trường cấu hình ở CUỐI struct
- `Sources/S3Client/S3Client.cpp` — chuỗi nhường quyền `nBS = nTK ? nTK : (nLD ? nLD : nHD)`
- `WAutoUI/{Resource.h,WAuto.rc,WAuto.cpp}` — tab 12 (23 điều khiển) + tooltip + lưu/nạp/di trú

Binary staged: `CoreClient.dll.moi_2408_liendau` 2.326.016 B (CRT-TĨNH ĐÚNG) ·
`Game.exe.moi_2408_liendau` 1.260.544 B (UCRT-RELEASE ĐÚNG) · `WAuto.exe.moi_2408_liendau`
387.072 B — mốc 20:17, **một lần swap ăn cả 3 đợt hôm nay** (Liên đấu r1+r2 + Hoạt động).

*Ghi 24/08/2026 ~20:20.*
