# PHÂN TÍCH QUY LUẬT THƯỞNG DÃ TẨU (tasklink)

Ngày chốt: **19/08/2026** · Đọc trực tiếp từ script + bảng dữ liệu, đối chiếu bản gốc Linux (`.goc`) và hàm C++ `Sources\Core\Src\ScriptFuns.cpp`.

**Nguồn (kho mã):** `D:\GAMEDEVNEW\serverscript_jx2\datau_tasklink\`
**Bản đang chạy:** `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\` — đã kiểm: **khớp 100%** với kho mã.

Mọi con số xác suất / khoảng giá trị dưới đây được **tính lại bằng mô phỏng đúng công thức trong script**, không phải ước lượng.

---

## 0. TÓM TẮT

| Hạng mục | Con số |
|---|---|
| Số loại nhiệm vụ | **6** |
| Số map nhiệm vụ (loại 4) | **14** (bản gốc Linux có 104 — port đã cắt 90) |
| Trần nhiệm vụ mỗi ngày | **40** |
| Mốc tích lũy vĩnh viễn | **80 mốc** (100 → 8000), + mốc mỗi-10 |
| Chuỗi đầy đủ | 20 link × 20 lần = **400 nhiệm vụ/vòng**, tối đa 20 vòng |

---

## 1. VÒNG LẶP & MÁY TRẠNG THÁI (task 1028 = course)

| course | Trạng thái | Ý nghĩa |
|---|---|---|
| 0 | Chưa nhập môn | NPC mời tham gia → `Task_Confirm` |
| 1 | Đang làm | Có nút trả / hủy |
| 2 | Đã nộp, chưa lãnh | Gặp lại NPC nhảy thẳng vào cửa sổ 3 rương — **không mất thưởng** |
| 3 | Đã lãnh | Hỏi làm tiếp → `Task_TaskProcess` |

**Bộ đếm chuỗi** — task `1020` đóng gói 4 byte: b1 = lần trong link (0–19), b2 = link (1–20), b3 = vòng, b4 = lượt hủy còn dư.

```
Tổng nhiệm vụ = (số link đã xong × 20) + lần trong link + (số vòng × 400)
                + 1 nếu course đang là 2 hoặc 3
```
Cache vào task `1044`.

### 🔴 LUẬT QUAN TRỌNG NHẤT
Hủy nhiệm vụ khi **không còn lượt hủy** → script đặt lại lần = 0, link = link khởi đầu, vòng = 0, đếm link = 0.
**Toàn bộ tiến trình tích lũy về 0** — mọi mốc 100…8000 phải làm lại từ đầu. Không có cơ chế bảo tồn.

### Trần ngày & hình phạt
- **40 nhiệm vụ/ngày** — task `2420` đếm, task `2419` giữ ngày (yymmdd). Sang ngày mới thì `2420` và `2797` (số lần hủy) tự về 0.
- **Hủy ở đầu chuỗi > 2 lần** — task `1036` = 10, mốc thời gian ghi task `1029`. Chờ **10.890 tick ≈ 10 phút** (JX1 18 tick/giây).

**Chỉnh ở đâu:** `script\global\seasonnpc.lua` → `checkTask_Limit()` (số 40), `tasklink_entence()` (số 10890) · `tasklink_head.lua` → `TL_MAXTIMES/TL_MAXLINKS/TL_MAXLOOPS`.

---

## 2. SÁU LOẠI NHIỆM VỤ

| # | Loại | Bảng dữ liệu | Dòng | Server kiểm bằng | Mất đồ? |
|---|---|---|---|---|---|
| 1 | Mua vật phẩm | `tasklink_buygoods.txt` | 35 | Khớp tuyệt đối 5 trường (genre, detail, particular, hệ, level) | **Có** |
| 2 | Tìm vật phẩm | `tasklink_findgoods.txt` | 526 | 5 trường cơ bản, hoặc mã thuộc tính 85–110 trong [Min,Max] | **Có** |
| 3 | Khoe vật phẩm | `tasklink_showgoods.txt` | 45 | Chỉ mã thuộc tính + khoảng, quét cả 6 ô ma pháp | Không |
| 4 | Địa Đồ / Mật Chỉ | `tasklink_findmaps.txt` | 28 | So task `1025` với cột `Num` | Không |
| 5 | Nâng chỉ số | `tasklink_upground.txt` | 45 | Hiệu số so mốc lúc nhận (task `1026`; exp dùng 1033/2574/1034) | Không |
| 6 | Sơn Hà Xã Tắc | `tasklink_worldmaps.txt` | 11 | task `1027` ≥ `Num`, khi trả trừ đúng `Num` | Trừ mảnh |

### Loại 5 — 6 kiểu chỉ số (cột `NumericType`)

| Type | Chỉ số | Ngưỡng trong bảng | Số dòng | Ghi chú vận hành |
|---|---|---|---|---|
| 2 | Kinh nghiệm | 5.000 → 50.000.000 | 12 | Dễ nhất |
| 3 | Danh vọng | 1 → 100 | 8 | 🔴 Là tiền tệ shop — tiêu vào là hiệu số âm, kẹt |
| 4 | Phúc duyên | 1 → 100 | 8 | 🔴 Cùng rủi ro |
| 5 | Điểm PK | 1 → 10 | 10 | 🔴 JX1 kẹp PK 0–10 → nền cao là bất khả thi |
| 6 | Tống Kim | 100 → 10.000 | 7 | Chỉ cộng cuối trận |

Dòng `5030`–`5038` (PK cần 2–10 điểm) có **toàn bộ TaskRate = 0 ở cả 20 link** → không bao giờ được chọn. Chỉ `5029` (PK 1 điểm) còn sống.

**Chỉnh ở đâu:** bỏ hẳn 1 loại → đặt cả hàng trong `tasklink_mainlink.txt` = 0 (script có chốt an toàn: link tổng trọng số 0 sẽ rơi về loại 2). Bỏ 1 dòng → đặt `TaskRate1..20` của dòng đó = 0.

---

## 3. TRỌNG SỐ LOẠI THEO LINK (`tasklink_mainlink.txt`)

6 hàng (loại) × 20 cột (link). Script tự chuẩn hoá nên tổng cột không cần bằng nhau.

| Link | Mua | Tìm | Khoe | Địa Đồ | Nâng | SHXT |
|---|---|---|---|---|---|---|
| 1 | 54,8% | 0,7% | 3,4% | 20,5% | 13,7% | 6,8% |
| 2 | 42,4% | 3,0% | 6,1% | 24,2% | 18,2% | 6,1% |
| 3 | 38,7% | 6,5% | 9,7% | 25,8% | 12,9% | 6,5% |
| 4 | 29,4% | 8,8% | 11,8% | 29,4% | 14,7% | 5,9% |
| 5 | 21,1% | 15,8% | 10,5% | 31,6% | 15,8% | 5,3% |
| 6 | 18,8% | 12,5% | 12,5% | 31,2% | 18,8% | 6,2% |
| 7 | 13,3% | 13,3% | 13,3% | 33,3% | 20,0% | 6,7% |
| 8 | 6,1% | 15,2% | 18,2% | 36,4% | 18,2% | 6,1% |
| 9 | 6,1% | 15,2% | 18,2% | 36,4% | 18,2% | 6,1% |
| 10 | 5,9% | 17,6% | 17,6% | 35,3% | 17,6% | 5,9% |
| 11 | 6,7% | 13,3% | 13,3% | 40,0% | 20,0% | 6,7% |
| 12 | 6,7% | 13,3% | 13,3% | 40,0% | 20,0% | 6,7% |
| 13 | 2,9% | 11,8% | 17,6% | 41,2% | 20,6% | 5,9% |
| 14 | 0,0% | 17,1% | 17,1% | 40,0% | 20,0% | 5,7% |
| 15 | 0,0% | 15,8% | 15,8% | 42,1% | 21,1% | 5,3% |
| 16 | 0,0% | 17,6% | 11,8% | 35,3% | 29,4% | 5,9% |
| 17 | 0,0% | 16,2% | 13,5% | 37,8% | 27,0% | 5,4% |
| 18 | 0,0% | 17,9% | 15,4% | 35,9% | 25,6% | 5,1% |
| 19 | 0,0% | 16,7% | 16,7% | 38,1% | 23,8% | 4,8% |
| 20 | 0,0% | 19,0% | 19,0% | 33,3% | 23,8% | 4,8% |
| **TB** | **12,6%** | **13,4%** | **13,7%** | **34,4%** | **20,0%** | **5,9%** |

Ba quy luật: **Mua vật phẩm** là nhiệm vụ tập sự (54,8% ở link 1, tắt hẳn từ link 14) · **Địa Đồ/Mật Chỉ** là xương sống (20–42%, hơn ⅓ toàn chuỗi) · **SHXT** giữ đều 5–7% vì trọng số gốc là hằng số 1000.

**Cấp nào bắt đầu link nào** (`levellink.txt`): cấp 1→link 1 · 20→3 · 50→6 · 80→11 · 100→16.

---

## 4. BẢN ĐỒ NHIỆM VỤ — 14 MAP (`tasklink_findmaps.txt`)

28 dòng = 14 map × 2 loại cuộn.

| Tầng | Map | Địa Đồ (cuộn 205) | Mật Chỉ (cuộn 212) | Link | TaskValue2 |
|---|---|---|---|---|---|
| Thành/thôn | 1 Phượng Tường · 11 Thành Đô · 37 Biện Kinh · 53 Ba Lăng Huyện · 78 Tương Dương · 80 Dương Châu · 162 Đại Lý · 176 Lâm An | 5 tấm | 1 tấm | 1–2 | 120.000 / 500.000 |
| Trung cấp | 21 Thanh Thành Sơn · 122 Hoàng Hà Nguyên Đầu | 8 tấm | 2 tấm | 3–10 | 450.000 / 1.500.000 |
| Cao cấp | 75 Khoái Lang Động · 225/226/227 Sa Mạc Sơn Động 1–3 | 15 tấm | 3 tấm | 6–20 | 800.000–1.200.000 / 2.400.000 |

- **Cuộn 205 (Địa Đồ Chỉ)** rơi từ quái thường · **Cuộn 212 (Mật Chỉ)** CHỈ rơi từ **boss xanh**.
- Nhặt cuộn khi không làm nhiệm vụ / sai map → **+1 mảnh SHXT** (task `1027`), không phí.
- Nhặt trong tổ đội: mỗi tấm cộng cho **cả đội**.

### 🔴 Bản port đã cắt 90 map
Bảng gốc Linux: **104 map / 208 dòng**. Map bị bỏ gồm toàn bộ hệ hang động: Tương Vân Động 1–5, Dược Vương Động 1–4, Thiên Tâm Tháp 1–3, Điểm Thương Động 1–3, Long Môn Trấn, Chu Tiên Trấn, Đào Hương Thôn, Thạch Cổ Trấn, Giang Tân Thôn, Cái Bang, Long Cung Động, Trường Bạch Sơn…

Bản gốc còn nguyên ở `tasklink_findmaps.txt.goc` — muốn mở lại map nào thì chép dòng tương ứng sang, **nhớ kiểm map id có tồn tại trên JX1**.

---

## 5. CỬA SỔ 3 RƯƠNG

Server bốc **3 loại khác nhau** từ 5 pool, bốc **không lặp** (loại trúng bị đặt trọng số = 0 cho lượt kế). Người chơi chọn **1 trong 3**.

| Pool | Trọng số | Xác suất CÓ MẶT trong bộ 3 | Nội dung |
|---|---|---|---|
| Vật phẩm | 34 | **86,5%** | Bốc từ `award_basic.txt` (103 dòng) |
| Kinh nghiệm | 33 | **85,8%** | Theo công thức, cộng thẳng |
| Tiền | 20 | **71,1%** | Lượng bạc, theo công thức |
| Đổi lần nữa | 8 | **34,4%** | Hộp bí mật — không hiện tên món |
| Lượt hủy | 5 | **22,3%** | +1 cơ hội hủy (trần 254) |

### "Đổi lần nữa" thực chất là gì
Không phải quay lại lần hai. Server **đã bốc sẵn**, chỉ giấu tên và hiện "Có muốn thử xem đây là gì không?". Bảng bốc = **103 dòng vật phẩm + 2 dòng ảo** (1 exp, 1 tiền), giá trị tính trọng số được **nhân 1,2**.
⇒ Ô này **có lợi hơn** ô Vật phẩm với đồ đắt. Nó cũng chịu may mắn mạnh gấp 30 lần (`lucky×0,03+1` thay vì `lucky×0,001+1`).

### Khoá hạt giống
Bộ 3 ô khoá bằng task `1037` (ghi lúc nộp = giờ game + giờ thực). Đóng/mở lại vẫn **đúng 3 món cũ**.

### 🔴 Điều kiện túi — dễ mất thưởng nhất
- **Trả nhiệm vụ** cần **≥5 ô trống**; **nhận vật phẩm** cũng **≥5 ô**. Thiếu → server từ chối, course vẫn = 2, nói chuyện lại là nhận được.
- **Mốc 10** cần **≥3 ô**; thiếu thì không phát và **không** đánh dấu → dọn túi gặp lại NPC vẫn nhận.
- **Mốc 40** chỉ *cảnh báo* khi <6 ô rồi **vẫn phát 5 rương** → đây là chỗ **duy nhất** có thể rơi mất đồ thật.

**Chỉnh ở đâu:** trọng số 5 pool ở `tasklink_award.lua` dòng 31 — `local myMainAwardRate = {20,33,34,8,5}` (thứ tự: Tiền, Exp, Vật phẩm, Đổi-lần-nữa, Lượt-hủy).

---

## 6. CÔNG THỨC TIỀN & KINH NGHIỆM

```
HỆ SỐ TIẾN ĐỘ  K = 1 + (SốLinkĐãXong + LầnTrongLink) × 0,1
               SốLinkĐãXong: 0–20 · LầnTrongLink: 0–19  →  K = 1,0 … 4,9

TIỀN = floor( (TaskValue2 × K + Vòng × 0,2) × 0,05 × 1,15 × rand(0,60…1,00) ) + TaskValue1
EXP  = floor( (TaskValue1 + TaskValue2 × K + Vòng × 0,2) × 0,36 × rand(0,80…1,20) )
```

Ba hệ số cần nhớ: **0,05 × 1,15 = 0,0575** cho tiền (1,15 là lần tăng 15% riêng bản VN năm 2006) và **0,36** cho exp → **exp ≈ 6,3× tiền** ở cùng nhiệm vụ. Biên độ ngẫu nhiên: tiền 60–100% (thiên thấp), exp 80–120% (cân đối).

### Giá trị thực tế

| Dòng nhiệm vụ | TaskValue2 | Tiền đầu chuỗi | Tiền cuối chuỗi | Exp đầu chuỗi | Exp cuối chuỗi |
|---|---|---|---|---|---|
| Mua vật phẩm (TV1=2.700) | 10.000 | 3.045–3.275 | 4.390–5.517 | 3.657–5.486 | 14.889–22.334 |
| Địa Đồ — thành/thôn | 120.000 | 4.139–6.899 | 20.286–33.810 | 34.560–51.840 | 169.344–254.016 |
| Địa Đồ — Thanh Thành/Hoàng Hà | 450.000 | 15.524–25.874 | 76.072–126.787 | 129.600–194.400 | 635.040–952.560 |
| Mật Chỉ — thành/thôn | 500.000 | 17.249–28.749 | 84.525–140.875 | 144.000–216.000 | 705.600–1.058.400 |
| Địa Đồ — Sa Mạc | 800.000 | 27.600–46.000 | 135.240–225.400 | 230.400–345.600 | 1.128.960–1.693.440 |
| Địa Đồ — Khoái Lang | 1.200.000 | 41.400–69.000 | 202.860–338.100 | 345.600–518.400 | 1.693.440–2.540.160 |
| Mật Chỉ — Sa Mạc/Khoái Lang | 2.400.000 | 82.800–138.000 | 405.720–676.200 | 691.200–1.036.800 | 3.386.880–5.080.320 |
| Nâng exp 50tr (dòng 5012) | 30.000.000 | 1.034.999–1.724.999 | 5.071.500–8.452.500 | 8.640.000–12.960.000 | **42.336.000–63.504.000** |
| SHXT 5.000 mảnh (dòng 6011) | 100.000.000 | 3.450.000–5.750.000 | **16.905.000–28.175.000** | 28.800.000–43.200.000 | **141.120.000–211.680.000** |

⚠️ **SHXT 5.000 mảnh ở cuối chuỗi trả tới 211 triệu exp trong MỘT lần** — nhiều hơn cả mốc 40 nhiệm vụ/ngày (100 triệu). Trọng số rất thấp (1–5 điểm ở link 16–20) nhưng vẫn tồn tại.

**Chỉnh ở đâu (`tasklink_award.lua`):**
- Toàn bộ exp → hệ số `0.36` (nhánh `myAwardType==2`)
- Toàn bộ tiền → `0.05 * 1.15` (nhánh `myAwardType==1`)
- Chuỗi dài đỡ lời → hệ số `0.1` trong `(1+(myCountLinks+myTimes)*0.1)` — chính nó làm cuối chuỗi lời gấp 4,9 lần
- Riêng 1 nhiệm vụ → cột `TaskValue2` của dòng đó trong bảng loại

---

## 7. BẢNG VẬT PHẨM NGẪU NHIÊN (`award_basic.txt`, 103 dòng)

Từ Huyền Thiên Chuỳ (TaskValue 500) tới An Bang Băng Tinh Thạch Hồng Liên (9,9 tỷ).

### Quy luật trọng số
Gọi **V** = giá trị nhiệm vụ (có nhân `lucky×0,001+1`), **N** = 103:

```
Trọng số dòng i = V ÷ ( N × max(V, TaskValue_i) )

→ Mọi món có TaskValue ≤ V đều có trọng số BẰNG NHAU (= 1/N)
→ Món có TaskValue > V bị phạt tỉ lệ nghịch: đắt gấp đôi V thì trọng số còn một nửa
```

**Hệ quả:** nhiệm vụ giá trị càng cao thì càng nhiều món "rẻ" rơi vào vùng bằng nhau, **làm loãng cơ hội trúng đồ tốt**.

| V | Dòng "rẻ" (trọng số bằng nhau) | Dòng CÓ THỂ trúng | P(Huyền Thiên Chuỳ) | Món đắt nhất còn với tới |
|---|---|---|---|---|
| 10.000 | 6/103 | 25/103 | **61,0%** | Bằng Phong hoàn (800K) |
| 100.000 | 11/103 | 35/103 | 22,0% | Huyền tinh cấp 3 (2,25tr) |
| 500.000 | 30/103 | 43/103 | 14,0% | Đại Lực hoàn (3,5tr) |
| 1.000.000 | 37/103 | 45/103 | 12,0% | Huyền Tinh cấp 4 (9tr) |
| 3.000.000 | 44/103 | 48/103 | 11,0% | Nhu Tình Thục Nữ Hồng Liên (40tr) |
| 10.000.000 | 47/103 | 52/103 | 10,0% | Tinh Hồng Bảo Thạch (200tr) |
| 50.000.000 | 54/103 | 60/103 | 10,0% | **Định Quốc Tử Đồng Hộ Uyển — Hoàng Kim (1,2 tỷ)** |

⚠️ **Trang bị Hoàng Kim nằm ngay trong bảng vật phẩm thường.** Ở nhiệm vụ giá trị cao (SHXT 5.000 mảnh, Nâng exp 50tr, Mật Chỉ Sa Mạc cuối chuỗi), V đủ lớn để với tới các dòng `Quality = 1` → Hoàng Kim có thể rơi từ ô Vật phẩm hằng ngày, **không cần đợi mốc 6000/7000/8000**. Không muốn vậy thì nâng `TaskValue` các dòng Hoàng Kim lên hoặc gỡ khỏi bảng.

### 🔴 BẪY LƯỢNG TỬ HOÁ 1% — ĐỌC KỸ TRƯỚC KHI SỬA BẢNG
Hàm bốc dùng `j = C_Random(1,100)/100 × tổng_trọng_số` → **chỉ 100 giá trị rời rạc**. Dòng nào chiếm dưới 1% dải tích lũy đều **không bao giờ trúng được** — cột "dòng có thể trúng" cho thấy **hơn một nửa bảng là trang trí**.

**Hệ quả khi chỉnh:** thêm một dòng mới có thể **vô hiệu hoá một dòng khác** (mọi trọng số đều chia cho N). Sau mỗi lần sửa bảng phải tính lại, đừng giả định.
Bảng xếp **giảm dần theo giá** nên đồ đắt nằm ở đầu dải tích lũy — chính vùng bị lượng tử hoá cắt mất nhiều nhất.

### Huyền Thiên Chuỳ — chìa mở rương
Bản port **đã thêm 5 dòng** Huyền Thiên Chuỳ (`6/1/2357`, TaskValue 500) vào cuối bảng; bản Linux gốc không có. TaskValue cực thấp nên luôn nằm trong vùng "bằng nhau" và chiếm phần đuôi dải → xác suất rất cao ở nhiệm vụ giá trị thấp (61% khi V=10.000), ổn định **10–12%** ở nhiệm vụ bình thường.

**Chỉnh ở đâu:** món hiếm hơn → **tăng** `TaskValue`. Phổ biến hơn → giảm xuống dưới V. Nhiều chìa hơn → thêm/bớt dòng Huyền Thiên Chuỳ (mỗi dòng ≈ +2 điểm phần trăm).
⚠️ Cột `Quality`: `1` = Hoàng Kim, phát bằng `AddGoldItem` + **bắn thông báo toàn server**; `0` = vật phẩm thường qua `AddItem`.

---

## 8. MỐC THƯỞNG & TÍCH LŨY

**Hai hệ mốc chạy song song, đếm bằng hai bộ đếm khác nhau** — đây là chỗ hay nhầm nhất.

### Hệ A — mốc theo NGÀY (reset 0h, KHÔNG tích lũy)

| Mốc | Điều kiện chính xác | Thưởng | Biến đếm |
|---|---|---|---|
| **30/ngày** | `(số làm − số hủy) == 30` | **+30.000.000 exp** | `2420 − 2797` |
| **40/ngày** | `số làm == 40 VÀ số hủy == 0` | **+100.000.000 exp + 5× Bảo rương thần bí (6/1/2383)** | `2420`, `2797` |

Điều kiện mốc 40 dùng **so sánh bằng chính xác**, không phải "≥". Hủy đúng 1 lần trong ngày là **mất trắng** cả 100 triệu exp lẫn 5 rương, không gỡ được (2797 chỉ reset khi sang ngày). Mốc 30 rộng tay hơn vì có trừ số hủy → hủy 1 lần vẫn đạt ở nhiệm vụ thứ 31.

### Hệ B — mốc TÍCH LŨY VĨNH VIỄN (đếm tổng nhiệm vụ, xuyên ngày)

| Nhịp | Thưởng | Nguồn | Chống trùng |
|---|---|---|---|
| **Mỗi 10 nhiệm vụ** | **3× Boss Triệu Hoán Phù (6/1/1023)** | `seasonnpc.lua` · `Task_GiveAward` | task `2690` |
| **Mỗi 100 nhiệm vụ** (80 mốc: 100→8000) | Theo bảng — xem dưới | `award_link.txt` · `tl_getlinkaward` | so bằng chính xác |

Trong 80 mốc thì **71 mốc chỉ cho 1 Tiên Thảo Lộ**. Chín mốc còn lại:

| Mốc | Phần thưởng | Số món | Cách bốc |
|---|---|---|---|
| 1.000 | Tử / Lam / Lục Thủy Tinh | 3 | ngẫu nhiên đều |
| 1.500 | Huyền Tinh Khoáng Thạch cấp 4 | 1 | cố định |
| 2.500 | Huyền Tinh Khoáng Thạch cấp 5 | 1 | cố định |
| 3.000 | Tẩy Tủy Kinh | 1 | cố định |
| 4.000 | Võ Lâm Mật Tịch | 1 | cố định |
| 5.000 | Huyền Tinh Khoáng Thạch cấp 6 | 1 | cố định |
| 6.000 | **Bộ Định Quốc (Hoàng Kim)** | 5 | ngẫu nhiên đều |
| 7.000 | **Bộ An Bang (Hoàng Kim)** | 4 | ngẫu nhiên đều |
| 8.000 | **Vũ khí Hoàng Kim đỉnh** | 23 | ngẫu nhiên đều |

Ba mốc cuối phát bằng `AddGoldItem` + **thông báo toàn server** + ghi log. Với trần 40/ngày, mốc 8000 cần tối thiểu **200 ngày** liên tục không hủy.

### 🔴 TÍCH LŨY KHÔNG AN TOÀN
Bộ đếm tích lũy được **suy ra từ vị trí trong chuỗi**, không phải biến cộng dồn độc lập. Chuỗi bị đặt lại (hủy khi hết lượt hủy) → tổng số nhiệm vụ về 0, toàn bộ mốc làm lại.

### Cống hiến bang
```
Cống hiến = floor( giá_trị_phần_thưởng_đã_chọn ÷ 8 )   rồi qua extong_double_award
```
Phát mỗi lần nhận thưởng, chỉ khi có bang. Nếu x2 đang bật thì giá trị được **chia lại** cho hệ số nhân trước khi tính → **x2 không làm tăng cống hiến bang**.

**Chỉnh ở đâu:** `seasonnpc.lua` → `Task_NewVersionAward()` (30000000 / 100000000 / nCount=5), `Task_GiveAward()` (`mod(nTotalTaskNum,10)==0`, `{6,1,1023}` nCount=3) · `award_link.txt` cột `Num` · `script\tong\tong_award_head.lua` → `tongaward_tasklink` (số 8).

---

## 9. BẢO RƯƠNG THẦN BÍ 2383

Chỉ đến từ mốc 40/ngày (5 cái/lần). Mở cần **6 Huyền Thiên Chuỳ** (`6/1/2357`) + **≥4 ô trống**.

| Tỉ lệ | Vật phẩm | ID | Ghi chú |
|---|---|---|---|
| **30%** | Khiêu chiến Lễ bao | 6/1/2015 | đã gộp 3 lệnh bị bỏ (Vân Lộc, Thương Lãng, Huyền Viên) |
| 25% | Đại Lực hoàn | 6/1/3 | |
| 25% | Phi Tốc hoàn | 6/1/6 | |
| 10% | Quẻ Huy Hoàng cao | 6/1/907 | hạn 7 ngày (604.800 giây) |
| 5% | Tiên Thảo Lộ | 6/1/71 | |
| 3% | Cẩm nang đổi trời đất | 6/1/1790 | param 60 |
| 2% | Tiên Thảo Lộ đặc biệt ×3 | 6/1/1182 | phát 3 cái — cần 3 ô trống |

### ⚠️ Cân đối kinh tế chìa — ĐANG THIẾU NẶNG
Một ngày hoàn hảo cho 5 rương → cần **30 chìa**. Nguồn chìa duy nhất là ô Vật phẩm (~10–12%/lần trúng ô đó), mà ô Vật phẩm chỉ có mặt 86,5% và người chơi chỉ chọn được 1/3 ô:

```
Nếu LUÔN ưu tiên chọn ô Vật phẩm khi có:
40 × 0,865 × 0,11 ≈ 3,8 chìa/ngày  →  mở được 0,63 rương/ngày
Tồn kho 5 rương/ngày cần 30 chìa   →  THIẾU ~87%
```

Rương sẽ dồn lại chứ không mở kịp. Muốn mở hết 5 rương/ngày phải **hạ số chìa từ 6 xuống 1**, hoặc thêm nhiều dòng Huyền Thiên Chuỳ vào `award_basic.txt`, hoặc mở nguồn chìa khác.

**Chỉnh ở đâu:** `script\item\ruong_datau_tasklink.lua` — `GetItemCount(0,6,1,2357) < 6` **và** `ConsumeItem(6,0,6,1,2357)`, phải sửa **cả hai**. Bảng roll: ngưỡng `25000/50000/80000/90000/95000/98000` trên thang 100.000.

---

## 10. HỆ SỐ NHÂN TOÀN CỤC

| Hệ số | Trạng thái | Tác động | Nguồn |
|---|---|---|---|
| **Sự kiện x2** (Hoàng chi chưởng) | **Có thể bật** | Nhân đôi *toàn bộ* tiền + exp Dã Tẩu. Kích hoạt khi `GN_HUANG_EVENTID` = **4**. Cống hiến bang bị chia ngược nên không hưởng x2. | `event\great_night\huangzhizhang\event.lua` |
| **May mắn** `GetLucky(0)` | Đang chạy | Ô Vật phẩm `×(lucky×0,001+1)` — không đáng kể.<br>Ô Đổi-lần-nữa `×(lucky×0,03+1)` — mạnh gấp 30 lần. | `tasklink_award.lua` · `tl_getplayerlucky` |
| **Độ mệt mỏi** `TireReduce` | **Vô hiệu** | JX1 không có hệ độ mệt. `LuaGetTiredDegree` luôn trả `0`, script cũng gán đè `TireDegree = 0`. Nhánh "đổi exp thành Điểm tích lũy Dã Tẩu" (item `6/1/1475`) là **code chết**. | `ScriptFuns.cpp:2015` · `tasklink_award.lua:628` |

**Bật x2 vĩnh viễn:** sửa `greatnight_huang_event` trả thẳng `2`, hoặc đặt `gb_GetTask(GN_EVENTNAME, GN_HUANG_EVENTID)` = 4. Cuối tuần x3 đã comment sẵn trong `gn_getaward_times()`, gỡ comment là dùng được.

---

## 11. DỮ LIỆU CHẾT & BẪY

| Hạng mục | Tình trạng | Chi tiết |
|---|---|---|
| `award_loop.txt` | **Chết hoàn toàn** | 10 dòng (Lệnh bài Phong Lăng Độ + 9 Đồ phổ Hoàng Kim). Chỉ `tl_giveplayeraward(3)` đọc bảng này và **không nơi nào gọi hàm đó** — kể cả bản Linux gốc. Sửa file này vô tác dụng. |
| `tl_giveplayeraward(2)` | Chết | Nhánh đọc `award_link` qua đường này không ai gọi. Bảng `award_link.txt` vẫn sống nhưng đi qua `tl_getlinkaward` trong `PayPlayerLinkAward`. |
| `SelectAward_Change` | Chết | Ô "Đổi lần nữa" được ánh xạ sang `mySG`/`SelectAward_Exp`/`SelectAward_Money` ngay lúc dựng cửa sổ. Hàm này chỉ còn phòng hờ. |
| Item `6/1/1475` | Chết | "Điểm tích lũy Dã Tẩu" chỉ phát khi độ mệt = 2, mà độ mệt luôn = 0. `LuaSetItemMagicLevel` đã là stub có chủ đích. |
| Mốc 8000 bonus | Đã tắt | Khối phát thêm 100 triệu lượng + 1 Hoàng Kim ngẫu nhiên ở `PayPlayerLinkAward` đang bị comment. Mốc 8000 trong `award_link.txt` (23 vũ khí Hoàng Kim) **vẫn chạy bình thường**. |
| Loại 5 kiểu PK | Gần chết | 9/10 dòng có toàn bộ TaskRate = 0. Chỉ dòng cần 1 điểm PK còn xuất hiện. |
| Điều kiện mốc 40 | **Đúng nhờ may** | Viết `if (nNum == 40 and nCancelNum) == 0` — thiếu ngoặc. Trong Lua 4 phép này *tình cờ* cho kết quả đúng (sai thì trả `nil`, mà `nil == 0` là sai). **Đừng đụng vào nếu không viết lại cho đúng:** `if nNum == 40 and nCancelNum == 0 then`. |

### Khác biệt bản port so với bản Linux gốc

| File | Thay đổi | Lý do |
|---|---|---|
| `tasklink_findmaps.txt` | **208 dòng → 28 dòng** | Cắt còn 14 map chắc chắn tồn tại trên JX1 |
| `award_basic.txt` | +5 dòng Huyền Thiên Chuỳ; Huyền Tinh particular `147→146`; Đại Lực/Phi Tốc hoàn detail `0→1` | Remap id sang bảng item JX1 + mở kinh tế chìa |
| `award_loop.txt` | Đồ phổ Hoàng Kim dịch id xuống 1 (`383→382`…) | Remap — nhưng bảng chết nên vô nghĩa |
| `award_link.txt` | Huyền Tinh `147→146` (3 dòng) | Remap id |
| `tasklink_findgoods.txt` | Huyền Tinh `147→146`; tắt TaskRate 2 dòng Hương Nang/Ngọc Bội hồi thể lực + 5 dòng Sát Thủ Giản cấp 90 | Remap + bỏ nhiệm vụ không lấy được đồ |
| `tasklink_buygoods.txt` | Đổi tên hiển thị 3 dòng (Toan Ngoa, Thanh Thông) | Khớp tên vật phẩm JX1 |

Mọi bản gốc còn nguyên ở file `.goc` cùng thư mục.

---

## 12. BẢNG TRA: CHỈNH Ở ĐÂU

Sửa ở **hai nơi**: kho mã `D:\GAMEDEVNEW\serverscript_jx2\datau_tasklink\` (nguồn sự thật, để commit) và cây triển khai `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\` (bản server đọc). Hiện hai bên khớp 100%.

| Muốn chỉnh | File | Vị trí |
|---|---|---|
| Trần nhiệm vụ mỗi ngày | `script\global\seasonnpc.lua` | `checkTask_Limit()` · `nNum>=40` |
| Exp mốc 30 & 40, số rương | `script\global\seasonnpc.lua` | `Task_NewVersionAward()` |
| Thưởng mốc mỗi 10 | `script\global\seasonnpc.lua` | `Task_GiveAward()` · `mod(...,10)` |
| Thời gian phạt hủy lậu | `script\global\seasonnpc.lua` | `tasklink_entence()` · `10890` |
| Yêu cầu ô trống | `script\global\seasonnpc.lua` | `CalcFreeItemCellCount()` |
| Trọng số 5 pool thưởng | `…\tasklink\tasklink_award.lua` | dòng 31 · `{20,33,34,8,5}` |
| Hệ số tiền (0,0575) | `…\tasklink\tasklink_award.lua` | `myAwardType==1` · `0.05*1.15` |
| Hệ số exp (0,36) | `…\tasklink\tasklink_award.lua` | `myAwardType==2` · `0.36` |
| Hệ số tiến độ chuỗi (0,1) | `…\tasklink\tasklink_award.lua` | `(1+(myCountLinks+myTimes)*0.1)` |
| Công thức trọng số vật phẩm | `…\tasklink\tasklink_award.lua` | `AssignValue_AwardRate()` |
| Độ dài chuỗi 20/20/20 | `…\tasklink\tasklink_head.lua` | `TL_MAXTIMES/LINKS/LOOPS` |
| Cấp nào bắt đầu link nào | `settings\task\levellink.txt` | 1→1, 20→3, 50→6, 80→11, 100→16 |
| Tỉ lệ 6 loại nhiệm vụ | `settings\task\tasklink_mainlink.txt` | 6 hàng × 20 cột |
| Bảng vật phẩm ô ngẫu nhiên | `settings\task\award_basic.txt` | cột `TaskValue` = độ hiếm |
| 80 mốc tích lũy 100→8000 | `settings\task\award_link.txt` | cột `Num` = mốc |
| Map & số cuộn loại 4 | `settings\task\tasklink_findmaps.txt` | `MapID` / `Num` / `MapType` |
| Ngưỡng chỉ số loại 5 | `settings\task\tasklink_upground.txt` | `NumericType` / `NumericValue` |
| Số mảnh SHXT loại 6 | `settings\task\tasklink_worldmaps.txt` | cột `Num` · 1 → 5000 |
| Nội dung rương 2383 | `script\item\ruong_datau_tasklink.lua` | bảng roll 1..100000 |
| Cống hiến bang | `script\tong\tong_award_head.lua` | `tongaward_tasklink` · ÷8 |
| Công tắc x2 | `…\huangzhizhang\event.lua` | `greatnight_huang_event` · eventid 4 |

### 🔴 TRƯỚC KHI SỬA
- Các file `.lua` và `.txt` này là **TCVN3 / ANSI**, không phải UTF-8. Mở bằng editor thường rồi lưu lại sẽ **phá hết tiếng Việt trong file**, không chỉ dòng bạn sửa. Dùng công cụ sửa an toàn theo latin-1, đối chiếu số byte >127 trước/sau khi lưu.
- Bảng `.txt` phân cách bằng **Tab**, dòng đầu là tên cột và script tra cột **theo tên**. Thêm cột sai chỗ là hỏng cả bảng.
