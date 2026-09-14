# PHÂN TÍCH NHẬT KÝ FOLD 7 — 13/09/2026 (bản 109131615, Tống Kim đông)

> Chủ: *"bạn kéo log của điện thoại về phân tích"*. Nhật ký do máy tự gửi về `D:\jx1_android_log` (`[DoNhip] GuiLog=1`),
> không cần cắm cáp. Kịch bản đọc: `scratchpad/phantich_log_fold7.py` (đọc lại được khi log lớn thêm).
> Máy: Samsung SM-F966U1 (Fold 7), SoC SM8750, Android 16, màn 1968×2184 có 120/80/60/48/30/24/10 Hz, khung logic **1040×936**
> (`[D1]` swapchain = khung logic). Bản chạy: **109131615** (TG + KHỐI bật, `Rep3AtlasKhoi=1`, `TheGioiRT=1`).

| Gói | Giờ | Dài | Nội dung |
|---|---|---|---|
| `SM-F966U1_20260913_161829` | 16:18:29 → 16:41:39 | 23 phút | vào game, Tống Kim (NPC 101/tick TB, max 181; đạn 33 500/10 s TB, đỉnh 260 000) |
| `SM-F966U1_20260913_164146` | 16:41:46 → (đang chạy, 17:00+) | 16+ phút | Tống Kim tiếp (NPC 103/tick, max 180) |

---

## 1. Kết luận nhanh

1. **Không sập, không `SIGSEGV`** trong cả hai gói (bản 109131615 có lỗi KHỐI tiềm ẩn — xem §7 — nhưng điều kiện gây sập chưa xảy ra trên máy).
2. **60 fps giữ tốt trong trận đông**: TB 58,4 / 59,0 fps; trung vị 60; p10 55 / 57. Giây dưới 55 fps chỉ 8 % / 5 %, dưới 30 fps chỉ 7 giây (gói 1, lúc vào map) / 0 (gói 2).
3. **Máy ấm chứ không nóng**: nhiệt mức 0 chiếm 86 % gói 1; gói 2 mức 2 chiếm 56 % (36 °C, ổn định). Công suất TB 2,34 / 2,13 W (đỉnh 7,8 W lúc nạp). Pin 100 → 91 % trong ~40 phút (≈ 13–15 %/giờ, khoảng 6,5 giờ chơi).
4. **[TG] thế giới RT đã chạy thật**: K=2 vào 3 lần trong 30–81 s đầu (màn 120 Hz + PaintFps 120, việc/khung 6,2–7,8 ms > 70 % của 8,33 ms), tự thoát sau 4 s dưới 45 %; tổng **2 164 khung vẽ vào RT + 2 161 khung chỉ blit, không lỗi**. Từ giây 81 thang nhịp xin 60 Hz → chu kỳ 16,67 ms → K=1 suốt phần còn lại (gói 2: 0 lần đổi K).
5. **Nút thắt còn lại là CPU của game, không phải GPU**: khung giật (>25 ms) có *vẽ CPU ngoài lớp vẽ* TB 21 ms (max 196–209 ms); GPU chỉ 61–63 % TB. Chữ vẽ mới 556 000 dòng / 16 phút (≈ 600 dòng/giây) so với 81 000 dòng dùng lại → **cache chữ** (P2 của bản mổ VNKU) là việc đáng làm nhất.

---

## 2. Nhịp khung (jx_paint.log `[SEC]`, `[SPIKE]`, `[SUM]`)

| | Gói 1 (861 giây đo) | Gói 2 (694 giây) |
|---|---|---|
| fps TB / trung vị / p10 / min | 58,4 / 60 / 55 / 1 | 59,0 / 60 / 57 / 30 |
| giây < 55 fps | 71 (8 %) | 35 (5 %) |
| giây < 45 fps | 22 | 7 |
| giây < 30 fps | 7 | 0 |
| lệnh vẽ / giây TB | 380 | 386 |
| việc thế giới (tick) ms / giây | 45 | 45 |
| spike ≥ 100 ms / ≥ 250 / ≥ 500 | 11 / 5 / 1 (1 170 ms ở giây 27 = vào map) | 4 / 1 / 0 (368 ms ở giây 18 = vào map) |
| spike do logic vs do paint | 17 vs 875 | 6 vs 635 |
| `[SUM]` cuối: vẽ TB/max, tick TB/max | 7 / 15 ms, 0 / 7 ms | 9 / 27 ms, 9 / 15 ms |

Giây tụt dưới 45 fps của gói 1 tập trung ở **27–95 s** (vào map + nạp: 27 s = 1 giây 1 fps, 70–80 s = 31–44 fps) và vài điểm lẻ
(515, 852, 887, 1025, 1168–1191, 1370–1375 s). Gói 2: 18 s (vào map), 405, 430, 782, 858, 931–932 s. Tức **ngoài lúc vào map,
trận đông vẫn giữ 55–60 fps**; các cú tụt lẻ là khung giật đơn (xem §4), không phải tụt kéo dài.

---

## 3. Màn hình, nhịp xin, nhiệt, pin (jx_thietbi.log `[MAU]`, `[DOI]`; jx_nhip.log `[NHIP-XIN]`)

| | Gói 1 (278 mẫu, 5 s/mẫu) | Gói 2 (190 mẫu) |
|---|---|---|
| màn 120 Hz / 60 Hz (số mẫu) | 15 / 262 | 1 / 189 |
| GPU bận TB / p90 / max | 61 % / 78 % / 94 % | 63 % / 78 % / 83 % |
| xung GPU TB | 288 MHz | 245 MHz |
| xung CPU lõi 0 TB | 1 085 MHz | 1 012 MHz |
| công suất TB / max | 2,34 W / 7,84 W | 2,13 W / 5,49 W |
| nhiệt mức 0 / 1 / 2 | 238 / 34 / 6 | 0 / 83 / 107 |
| pin, nhiệt độ | 100 → 95 %, 29,8 → 36,5 °C | 95 → 91 %, 36,5 → 36,0 °C |
| `[DOI]` màn đổi chế độ | **75 lần** | 9 lần |

* App xin `ANativeWindow_setFrameRate(60)` ngay giây 1,4 (thang nhịp), sau đó Samsung vẫn **tự bật 120 Hz khi chạm rồi về 60**
  → 75 lần đổi chế độ trong 23 phút (gói 1). Không hại: `[TG]` lấy chu kỳ = max(1000/PaintFps, 1000/Hz màn) nên màn 120 Hz mà
  PaintFps 60 vẫn là 16,67 ms, không rơi vào K=2 nhầm (bẫy 16:00 hôm nay đã chặn).
* Ở giây 79–81 thang nhịp xin **30 Hz rồi 45 rồi 60** (đúng lúc fps tụt 31–44 vì nạp) — nấc 30 chỉ tồn tại 1,7 s.
* Nhiệt gói 2 ở mức 2 (MODERATE) quá nửa thời gian nhưng nhiệt độ pin đứng yên 36 °C và fps không đổi → thang nhiệt chưa cần hạ thêm.

---

## 4. Khung giật và phần việc của từng tầng (jx_rep3.log `[VE-GIAT]`, `[VE]`, `[REP3-NAP]`, `[PGND]`)

| | Gói 1 | Gói 2 |
|---|---|---|
| khung giật (> ~25 ms) | 822 | 683 |
| khung giật TB / max | 30,2 / 918 ms | 30,2 / 952 ms |
| **vẽ CPU ngoài lớp vẽ** TB / max | **20,9 / 196 ms** | **21,0 / 209 ms** |
| nạp ảnh trong khung giật TB / max; khung có nạp > 5 ms | 0,6 / 134 ms; 18 | 0,6 / 139 ms; 8 |
| trình chiếu (chờ GPU/swapchain) TB / max; khung > 8 ms | 9,3 / 754 ms; 94 | 9,2 / 750 ms; 69 |
| `[PGND]` nạp nền: lần, TB, max, tổng | 398, 18 ms, 66 ms, 7,0 s | 314, 19 ms, 208 ms, 5,9 s |
| `[VE]` 30 s cuối: tải texture | 577 ảnh, 134 MB (max 2,3 MB/khung) | 706 ảnh, 137 MB (max 1,3 MB/khung) |
| `[VE]` ring đỉnh / ghi lệnh TB | 1,5 MB / 0,38 ms (609 lệnh, 1 203 quad) | 2,6 MB / 0,87 ms (960 lệnh, 2 255 quad) |
| `[VE-NAP]` nạp khung nền | giao 366, xong 366, hỏng 0, trễ giao→áp 14 ms | giao 405, xong 405, hỏng 0, trễ 13,6 ms |
| `[NAPNPC]` nạp trước NPC: gọi / giao nền / lần dùng đầu trễ | 19 389 / 41 / 2 | 45 417 / 94 / 13 |
| `[REP3-NAP]` trên luồng vẽ 30 s: rút khung, giải mã, tạo GPU | 66 lần 4,7 ms, 212 lần 2,0 ms, 212 lần 2,0 ms | 99 lần 9,9 ms, 240 lần 4,2 ms, 240 lần 3,3 ms |

Đọc: trong một khung giật điển hình 30 ms, **21 ms là CPU của game** (quét vùng, sắp sprite, chuẩn bị chữ, giao diện) — phần
`[VE-GIAT]` ghi "trong lớp vẽ 0.0 / 0 lệnh, ngoài 21" — Represent3 và nạp ảnh gần như không góp (nạp 0,6 ms). Trình chiếu 9 ms
là chờ GPU ở những khung GPU đầy (p90 78 %). Nạp nền `[PGND]` mỗi lần 18–19 ms xảy ra ~15 lần/phút → mỗi lần là một khung rơi.

---

## 5. Gộp lệnh, chữ, cache (jx_rep3.log `[VE-GOP]`, `[CHUGIU]`, `[VE-BKG]`)

* `[VE-GOP]` đổi trạng thái/khung TB: pipeline 3, texture/sampler 23–24 (max 29), viewport/cắt 2. Quad không gộp cả kỳ:
  **texture0 30 286 → 37 683** là lý do chính, không liên tiếp 1 620–1 901. Số khe sampler còn 12 khối là trần (§7).
* `[CHUGIU]` giữ 12 ms: dòng chữ **giữ 81 288, vẽ mới 556 149** (gói 2, 16 phút) → 87 % chữ vẽ lại từ đầu mỗi lần (tên, sát thương,
  chat đổi liên tục). Đây là phần lớn của "vẽ CPU ngoài lớp vẽ" trong trận đông.
* `[VE-BKG]` bỏ khung giống: 0 (trận đông khung nào cũng khác — đúng như thiết kế, không tốn gì thêm).
* `[GOP]` vỡ lô: 0 ở mọi mục — bind ring / ps theo đỉnh hoạt động đúng.

---

## 6. Ảnh null (jx_rep3.log `[REP3] anh_null top`) — việc riêng, chưa thuộc đợt này

"anh_null" = `GetImage` trả NULL hoặc khung ngoài tầm (đếm top 8 của `[REP3 08/09 h]`, in mỗi 30 s). Tên thư mục trong log là GBK,
giải mã ra (kỳ 30 s lúc 17:03, gói 2):

| Ảnh | Khung thiếu | Lần / 30 s |
|---|---|---|
| `\spr\skill\天忍\mag_bz_huo3_爆炸效果.spr` (hiệu ứng nổ chiêu Thiên Nhẫn) | k6, k7, k10, k11, k12 | 4 600–4 800 **mỗi khung số** (≈ 160 lần/giây) |
| `\spr\npcres\man\MA_HR_015_HD.spr` (tóc nam HD) | k30, k70 | 56, 214 |
| (không tên) | k0, k1, k3 | k0 ≈ 55 000–81 000 (≈ 31–45 lần/khung), k1 1/khung, k3 ≈ 4 000 |
| các kỳ khác (16:5x): `\Spr\Ui3\<thư mục Trung>\MiniMap.spr` k0 và 3 ảnh giao diện cùng thư mục | k0 | 1–2 lần/khung |

Tức trong trận đông **một hiệu ứng nổ chiêu bị thiếu 5 khung** (người chơi thấy hiệu ứng nháy/hụt), tóc HD thiếu khung, và một
ảnh không tên bị gọi ~35 lần mỗi khung. Nghi vấn: pak điện thoại đóng từ cây khác cây PC (khung ít hơn) hoặc bẫy chỉ mục 16 bit
(`[PAK 12/09 CHISOMUC]` đã sửa mã, nhưng pak trên dt_v4 vẫn ngày 10/09). Việc của phiên pak/dữ liệu, không thuộc đợt này;
không ảnh hưởng fps.

---

## 7. KHỐI atlas — lỗi tiềm ẩn trong bản 109131615 (đã sửa trong bản 109131648)

* Gói 1 cấp tới **8 khối = 512 MB** (`[KHOI] khoi atlas moi #7 ... tong 8 khoi, 512 MB` ở giây 1 305); gói 2: 7 khối = 448 MB ở giây 597.
  Trần `JX_KHOI_MAX` = 12 khối = 768 MB. Khối tăng vì **lớp trong khối chưa bao giờ được trả lại**: đường "trang rỗng thứ hai" của
  `CAtlasMgrGpu::Free/JxFreeKe` không có nhánh cho khối.
* Nguy hiểm hơn: nhánh đó rơi vào `DeferRelease(pPage->m_pTex)` = **huỷ cả texture khối dùng chung** (12 khe sampler + mọi trang
  khác) → `SIGSEGV` ở `SDL_BindGPUFragmentSamplers`. Phiên WAuto đã gặp đúng lỗi này 12/09 (thẻ Cơ bản đổi chữ 500 ms) và phải tắt
  `Rep3AtlasKhoi` trên máy ảo. Trên Fold 7 chưa xảy ra vì trang atlas trong trận đông hiếm khi rỗng hoàn toàn.
* Đã sửa (commit `0ac371b2`, kịch bản `android/va_nguon_mobile_1309_c.py`): trang trong khối → `JxTraLopKhoi` trả LỚP về khối, giữ
  texture; `ReleaseAll` không huỷ hai lần. Bản **109131648** mang bản vá này cùng phần WAuto hoàn thiện.

---

## 8. Việc nên làm tiếp (theo thứ tự lợi/công)

1. **Cache chữ** (P2 của `PHANTICH_NHIPHAN_VNKU_CPU_GPU_1309.md`): 600 dòng chữ mới/giây trong trận đông; giữ texture chữ theo
   (chuỗi, font, màu) thay vì vẽ lại → giảm phần "vẽ CPU ngoài lớp vẽ" 21 ms của khung giật.
2. **Nạp trước nền** (P3): 314–398 lần `[PGND]` 18–19 ms mỗi gói → mỗi lần một khung rơi; nạp ô nền kề theo hướng đi ở luồng nền.
3. **Ngân sách khối**: 8 khối × 64 MB; sau bản vá lớp được tái dùng nên sẽ không leo thang nữa — đo lại số khối ở gói log kế tiếp;
   nếu vẫn > 6 khối thì hạ `Rep3AtlasKhoiLop` 8 → 4 (khối 32 MB) hoặc thêm dọn trang rỗng theo tuổi.
4. **Ảnh null** (§6): soi tên thật, đóng lại pak nếu thiếu.
5. Không cần đụng thang nhiệt/nhịp: 60 Hz + nhiệt mức 1–2 + 2,1 W là điểm cân bằng tốt cho Fold 7 trong Tống Kim.

## 14/09 — log sau P4 `[TAI 14/09]` (4 gói 07:30 / 08:47 / 09:04 / 09:48, bản ≥ 109140039)

**Kết quả tốt (gói 09:48, 16 phút, đông: npc/tick TB 96 max 250, đạn/10 s TB 19 735):** fps TB **59,8**, trung vị 60, p10 60, chỉ 4 s/564 s
dưới 55 fps; GPU 60 % (p90 78), 2,00 W TB, nhiệt mức 0 suốt; không sập; 9 khối atlas (576 MB), RAM riêng 664 MB, cache 425 MB.
`[TG]` K=1 suốt (màn 60 Hz). Ba gói sáng ngắn 1–2 phút (vào map) không đại diện.

**P4a tô 0 trang atlas bằng chép GPU: ĐÚNG.** `[VE-TAI] … trang atlas moi to 0 bang chep GPU: 16 trang 0.5 ms` (0,03 ms/trang, trước 117–172 ms),
mục `zero` trong `[VE-GIAT]` = 0 khung giật ở cả 4 gói. Dải nguồn tạo 2 lần (fmt 12 BGRA8 2 MB, fmt 3 16-bit 1 MB) đúng như thiết kế.
**P4b tải dần:** chạy (7 708 khung/30 s lúc vào map, 600–3 000/30 s sau đó, 9–11 ms/30 s); không đo được phần nó giảm vì khung cần ngay vẫn tải lúc vẽ.
**P4c:** `[PGND] xoa 0.0` như máy ảo.

**Còn lại = tải texture thường (`lenh tai`)**: gói 09:48 có 80 `[VE-GIAT]` (> 20 ms) / 16 phút = 5/phút, **67 là `lenh tai` > 10 ms**;
tổng 643 lệnh, 159 MB, 3 784 ms → **23,8 ms/MB**. Theo cỡ lệnh:

| Cỡ mỗi lệnh | Số lệnh | ms/lệnh | Nhận xét |
|---|---|---|---|
| ≥ 1 MB (ô 512×512 BGRA8 = 1 024 KB đúng bằng) | 47 | **31** (đơn lẻ 45–50) | ô atlas vào **khối** (2D array 64 MB) |
| 256 KB–1 MB | 187 | 7,3 | |
| 64–256 KB | 223 | 3,7 | |
| < 64 KB | 186 | 0,7 | |
| dải nguồn 0 (2 MB, ảnh riêng, tải **cả ảnh**) | 2 | **1,3** (0,6 ms/MB) | nhanh gấp 30–70 lần |

Vào map: 103–124 texture 26–31 MB một khung = 540–550 ms (khung 784 ms, ẩn sau màn nạp); 8 ô × 1 MB = 158 ms.
Cùng driver, cùng transfer buffer, cùng lệnh `SDL_UploadToGPUTexture` nhưng **tải cả ảnh riêng nhỏ = 0,6 ms/MB, tải vùng con vào khối = 20–45 ms/MB**
→ chi phí nằm ở *đích* (vùng con của ảnh lớn / định dạng), không phải ở byte nguồn. Chưa biết chắc cơ chế (xếp ô trên CPU khi ghi vùng con
ảnh UBWC? theo định dạng BGRA8?). Bằng chứng gián tiếp cùng chiều: [PALBUF 11/09] tải hàng bảng màu vào texture 256×8192 (vùng con) từng mất
17–100 ms, đổi sang buffer thì hết.

**Phương án (chờ chủ chọn, chưa làm):**
1. **Đo trước 5 phút** (`[TAI-DO]` lúc khởi động, chỉ log): tải 1 MB theo 8 kiểu — vùng con vào khối / vào ảnh riêng 2048² / cả ảnh 512² riêng,
   ba định dạng BGRA8 · 16-bit · R8, từ bộ đệm khung (cycle) và bộ đệm cố định, buffer→buffer, ảnh→ảnh. Một bản, một lần mở app, đọc log là biết
   đường nào nhanh. Rẻ nhất, không đổi gì người chơi thấy.
2. Nếu "cả ảnh nhỏ" nhanh như dải nguồn: **tải qua ảnh đệm**: mỗi khung gom các ô cần tải vào MỘT ảnh đệm 2048×H (tải cả ảnh, nhanh) rồi
   `SDL_CopyGPUTextureToTexture` từng ô sang khối (GPU→GPU, đã chứng minh 0,03 ms/trang). Sửa trong SubmitFrame + QueueUpload, chỉ JX_MOBILE.
3. Thử nhanh không cần dựng: `Rep3AtlasKhoi=0` trong config dt_v4 (atlas = ảnh riêng 2048², mất gộp lệnh vẽ qua khối) — chỉ để so `lenh tai`,
   không phải cách sửa.

Ngoài ra vẫn còn: ảnh null `MA_HR_015_HD.spr`/`FM_HR_015_HD.spr` k20/k29/k34 (x282–422, việc pak), vào map 100 tệp spr đồng bộ ~110 ms.

## 14/09 10:49 — log sau bước 2 `[DEM 14/09]` (bản 109141044, 10 phút, có `[DEM] anh dem` trong log)

**Tải lên GPU: xong.** `[VE-GIAT]` có `lenh tai` > 10 ms: **0** (trước 67/80). Trong 24 khung giật còn lại tổng lệnh tải = 26 ms cho 56 MB
= **0,47 ms/MB** (trước 23,8). Vào map: 75 texture / 20 MB trong một khung = 4,1 ms lệnh tải (trước 518–550 ms). `[VE]` mỗi kỳ: chép lên GPU
max 17 ms (kỳ vào map, gồm 10,4 ms memcpy 20 MB vào transfer buffer), các kỳ khác ≤ 12 ms, TB 0,1–0,4 ms/khung. `[VE-TAI]`: 49 + 21 ô BGRA8
qua ảnh đệm lúc vào map, sau đó 0 (ô mới hiếm), tô 0 chép GPU 7 lần 0,3 ms. fps TB 59,3 (p10 60), 1,68 W, nhiệt mức 0 (103/118 mẫu), GPU 45 %,
không sập, 7 khối (448 MB), RAM riêng ~400 MB.

**Khung giật còn lại (24 khung > 20 ms / 10 phút = 2,4/phút; trước P3 là 20–33/phút):**

| Nhóm | Số | Ví dụ | Bản chất |
|---|---|---|---|
| Nạp đồng bộ trên luồng vẽ | 10 | rút khung 1 khung 24 / 25,7 / 72,4 ms (t=19, 103, 229 s); mở tệp spr 1 tệp 37,4 ms *ngoài lúc vẽ* (t=229,6 s); 19 tệp 14,9 ms (t=21 s) | một khung sprite to hoặc tệp lạnh trên bộ nhớ flash; ngân sách đồng bộ 3 ms (`NapKhungMs`) chỉ kiểm TRƯỚC khi rút nên không chặn được một lần rút 72 ms; "ngoài lúc vẽ" = logic hỏi kích thước/ảnh → phải mở tệp ngay |
| Logic game (ngoài lớp vẽ) | 13 | `[SPIKE] logic=199/89/183 ms` (t=16, 128, 145 s), 8 khung 60–72 ms vẽ CPU không nạp gì | không phải vẽ, không phải NPC (`[WORLD b]` 0,7 ms/tick); chưa có số đo pha logic (mạng / script / UI) |
| Vào map | 1 | 210 ms = 80 tệp spr 54,5 ms + logic | ẩn sau màn nạp |

Ghi chú: `anh_null` tên rỗng `(k0) x58 588 / 30 s` (≈ 2 000 lần/s) có từ trước (09:48: x65 749) — ai đó xin ảnh với tên rỗng mỗi khung; rẻ nhưng nên tìm.

**Đề xuất tiếp (chờ chủ chọn):**
1. Rút khung to → luồng nền theo *cỡ*: biết cỡ nén của khung từ chỉ mục spr trước khi rút; > ngưỡng (vd 256 KB) thì giao luồng nền và bỏ vẽ
   khung này 1–3 khung (như `bo ve` đang làm khi hết ngân sách), thay vì rút đồng bộ 24–72 ms. Kèm ghi tên tệp/khung vào `[VE-GIAT]` khi
   một lần rút/mở tệp > 10 ms để biết đó là gì (đang thiếu).
2. Đo pha logic: một bản đo `[TICK-DO]` chia tick logic thành mạng nhận/gửi, script Lua, UI KWnd, âm thanh, còn lại; ghi khi tick > 30 ms.
   Nhóm này giờ là lớn nhất (13/24) và chưa có số.
3. Tìm nguồn `anh_null` tên rỗng (một lần grep + log tên cửa sổ gọi).

## 14/09 11:12 — log sau `[NAPTO]` + `[LOGIC-PHA]` (bản 109141108, 4 phút gồm vào map)

fps TB **60,0** (p10 60, min 58, 0 giây dưới 55), 1,80 W, nhiệt mức 0, không sập; 20 `[VE-GIAT]` (9 nạp đồng bộ, 10 vẽ CPU, 1 tải), `[SPIKE]` 11 (≥ 100 ms: 1, lúc vào map).

**Logic = MẠNG (đã chốt):** `[LOGIC-PHA] t=6.6s logic=206: mang 165.0, uihb 40.3` · `t=11.1s logic=237: mang 236.1` · `t=11.3s logic=41: mang 40.1`;
WAuto / IPC / Breathe / PROCFRAME / gửi đều ≈ 0. Cả ba lần đều lúc vào thế giới (đợt đồng bộ đầu). `KNetConnectAgent::Breathe` xử lý **hết** gói
đang chờ trong `while (true)` (2 vòng: client + game server) → một đợt 236 ms trong một vòng lặp. Phiên 10:49 có cú 89/183 ms giữa trận (t=128/145 s)
chưa có số pha nhưng cùng cơ chế là khả năng cao (đông người → gói đồng bộ dồn).

**Nạp đồng bộ = CHỜ KHOÁ PAK, không phải khung to:** `[NAP-CHAM] rut khung FM_BD_015_ST01.spr k9 (1 KB nén, 19×61): 17.9 ms` · `MA_YY_999_AT01.spr k75 (1 KB): 37.4 ms` ·
`xx_…(chiêu cy).spr k8 (23 KB, 245×203): 97.4 ms` · `k5 (19 KB): 12.2` · `tr…k22 (60 KB): 17.1` — khung 1 KB mất 17–37 ms thì không phải giải nén hay đọc flash
(một trang UFS ~0,2 ms). `SprGetFrame` khoá mutex **theo tệp pak** (`ZSPRPackFile`), luồng nền (`NapNenChay`: nạp cả tệp spr trước + rút khung) giữ khoá
khi đọc tệp to → luồng vẽ rút một khung nhỏ phải đợi. NAPTO (ngưỡng cỡ nén) vì thế chỉ bắt 21 khung to, không bắt được các cú này.
Mở tệp ngoài lúc vẽ: 1 lần 12,6 ms (ảnh UI `…\通用\…\字.spr`).

**Vẽ CPU 60–72 ms (10 khung) = hai loại theo `[PDET]`:** (1) `render=78: lop duoi 44.9` / `render=39: lop duoi 36.1` / `lop tren 31.5` — **lớp cửa sổ UI**
vẽ 31–45 ms (thế giới 0,0 trong pass đó), phiên 10:49 y hệt (43,3 / 36,0 / 30,9): một cửa sổ nào đó vẽ rất nặng (nghi bảng nhiều chữ: Tống Kim,
chat, WAuto — `[CHUGIU] ve moi 507 775 dòng/16 phút`); chưa có tên cửa sổ. (2) vào map: `the gioi 119.9 (dau ham 79.3 = ghép nền, nen dat 36.9), lop giua 39.4`
+ `[PGND-V] #~4~#: RIO 66.4 ms` (ghi lệnh vẽ 69 ô mất 66 ms — chỉ lúc vào map, có thể là nộp khung giữa chừng khi GPU đang ngập tải; XA/kề bên bình thường 5–9 ms).
(3) `VAT THE 18.8–24.5` khi đông NPC (98/tick) — bình thường.

**Đề xuất tiếp (chờ chủ chọn):**
1. **`[MANG]`** ngân sách xử lý gói mỗi vòng lặp: `[Client] MangMs` (mặc định 8 ms), quá thì để phần còn lại sang vòng sau (thứ tự giữ nguyên, trễ thêm
   ≤ 1 vòng ≈ 8–16 ms; bộ đệm nhận giữ gói). Ghi `[MANG-CAT]` khi cắt. Hết cú 165–236 ms lúc vào map, và cú giữa trận nếu cùng cơ chế.
2. **`[PAKBAN]`** cờ "luồng nền đang đọc pak": luồng nền bật cờ quanh nạp tệp / rút khung; luồng vẽ thấy cờ thì giao khung cho luồng nền (bỏ vẽ 1–3 khung,
   như NAPTO) thay vì đợi khoá 17–97 ms. Không đụng Engine (khoá pak giữ nguyên).
3. **Đo cửa sổ UI nặng**: trong pass UI ≥ 25 ms ghi tên lớp/cửa sổ tốn nhất (`[PDET-UI]`), rồi mới quyết cache chữ hay sửa cửa sổ đó.
4. RIO 66 ms lúc vào map và nền vùng 15–20 ms/lần đổi vùng: để sau (ẩn sau màn nạp / thưa).

## 14/09 12:41 — log bản 109141129 (5 phút, chủ thử zoom/lắc/phóng to) + gói 12:38 (3 phút)

fps TB 58,7 (p10 54, min 33; 5 giây < 55 fps), 1,96 W, nhiệt mức 0, không sập; gói 12:38: fps 58,2, p10 48. `[VE-GIAT]` 23 (7 nạp, 9 vẽ CPU, 7 trình chiếu).
Ghép mốc sự kiện với khung giật cho thấy **hai nguồn mới, đo được rõ**:

| Sự kiện | Khung giật đi kèm | Bản chất |
|---|---|---|
| `[KHOI] khoi atlas moi #2` t=31,1 s | 73,4 ms (vẽ CPU 67,5) + 22,0 + 29,4 ms (trình chiếu, `nop` 21–27) | tạo texture 64 MB (`SDL_CreateGPUTexture`, [TAI-DO] đo 54 ms) + 2–3 lần nộp đầu tiên driver cam kết bộ nhớ (`nop` 21–43 ms) |
| khối #3 t=46,4 s | 72,5 + 45,7 + 44,0 ms (`nop` 42,9 / 34,1) | như trên |
| khối #4 t=120,7 s · #5 t=208,9 s | 113,1 + 33,4 · 95,2 + 34,7 ms | như trên → **mỗi khối mới = 2–3 khung 33–113 ms**, 4 khối/5 phút |
| chụm ngón zoom 1200→800 t=24,1–24,2 s (5 bước zoom + RT2 tạo 3 lần 2212×1992 / 2080×1872 / 2096×1888) | 97,4 ms (vẽ CPU 93,7, không nạp) | mỗi bước zoom > 1000 cấp lại RT, mỗi bước < 1000 cấp lại RT2 8–9 MB → ~10 lần cấp RT trong 1 giây |
| `[LAC] le RT 1000 -> 1120` (t=23,9 / 37,0 / 58,9 / 64,1 / 95,8 s) và trả về | các khung 20–30 ms quanh đó | mỗi lần bắt đầu/kết thúc lia cấp lại RT |
| `[LOGIC-PHA]` t=32,8 s mạng 159 ms; t=78,9 s mạng 45 ms (ngoài lúc vào map) | logic 166 / 45 ms | **gói mạng dồn GIỮA trận** — không chỉ lúc vào map |
| `[NAP-CHAM]` rút khung 0–1 KB 14,7 / 59,0 ms; mở tệp UI/skill 10–29 ms ngoài lúc vẽ (4 lần) | 60,5 / 4,8+29,9 / 20,4 ms | chờ khoá pak; tệp lạnh |
| `[PDET]` lớp dưới 43,9 / lớp trên 34,9 / lớp dưới 21,9 | 3 khung | cửa sổ UI nặng (chưa biết tên) |
| `[PDET] VAT THE 67,0 / 59,3` (npc tới 100/tick, zoom 120–150 %) | 2 khung | vẽ vật thể đông khi nhìn rộng (diện tích ×1,4–2,3); `Rep3DoVeChiTiet=0` nên chưa tách được |

**Việc còn lại, xếp theo mức đau (log này):** (1) khối atlas mới 2–3 khung/khối; (2) cấp lại RT/RT2 theo từng bước zoom/lề (phần LIA/ZOOM3D của phiên
camera — đã báo họ: cấp RT một lần ở cỡ tối đa (khung × 1,5) và RT2 = 2× khung, đổi viewport/uv thay vì cấp lại); (3) gói mạng dồn 45–159 ms;
(4) chờ khoá pak 15–59 ms; (5) cửa sổ UI 22–44 ms; (6) vật thể đông khi nhìn rộng.

**Đề xuất (chờ chủ chọn):**
A. `[KHOITRUOC]` cấp sẵn khối trong màn nạp lúc vào thế giới: `[Client] Rep3KhoiTruoc` = 3 khối R8G8 + 1 BGRA8 (≈ 256 MB, phiên này dùng 6 khối = 384 MB)
   và tô 0 cả các lớp ngay lúc đó (chép GPU) để driver cam kết bộ nhớ trong màn nạp; khối thứ 5+ mới tốn giữa trận (hiếm sau 10 phút).
   (SDL không cam kết `SDL_CreateGPUTexture` an toàn từ luồng khác nên không tạo ở luồng nền.)
B. `[MANG]` ngân sách xử lý gói mỗi vòng lặp (`[Client] MangMs`, 8 ms), phần dư sang vòng sau.
C. `[PAKBAN]` cờ luồng nền đang đọc pak → luồng vẽ giao khung thay vì đợi khoá.
D. `[PDET-UI]` ghi tên cửa sổ UI nặng nhất khi pass UI ≥ 25 ms; bật `Rep3DoVeChiTiet=1` trên dt_v4 để tách vẽ vật thể (chỉ config).
