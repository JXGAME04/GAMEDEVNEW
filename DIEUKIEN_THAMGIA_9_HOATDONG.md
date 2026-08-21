# ĐIỀU KIỆN THAM GIA + CÁCH HOẠT ĐỘNG — 9 HOẠT ĐỘNG LÔI ĐÀI / TÍN SỨ / BANG HỘI

> Phiên **21/08/2026**, tiếp `PHANTICH_LOIDAI_TINSU_BANGHOI.md`. **CHỈ PHÂN TÍCH.**
> Nguồn: `D:\ServerLinux\server1\script` (bản Linux JX2/Kiếm Thế VNG đang chạy thật).
> Cách lấy số: workflow 13 tác tử (9 đọc song song → 3 phản biện → 1 rà soát), 2,6 triệu token,
> mọi điều kiện đều được **kiểm lại lần 2** xem có bị comment `--` tắt hay không.
> Kết quả thô: `ReverseTools/dieukien_9_hoatdong_raw.json`.

---

## 0. QUYẾT ĐỊNH CỦA CHỦ GAME (21/08)

> **"những giới hạn đó để lại chỉ cần trên cấp 90 là có thể tham gia"** + **"không cần trùng sinh"**

Chính sách chốt cho bản port: **mọi cổng chặn về `GetLevel() >= 90`, bỏ sạch điều kiện trùng sinh.**
Danh sách đích danh từng dòng phải sửa: **mục 3**.

⚠️ Một lưu ý trước khi làm (nêu một lần, không nhắc lại): **4/9 hoạt động vốn KHÔNG có cổng cấp độ nào**
(Lôi đài tỷ võ, Lôi Đài Bang Hội, Cổ Tháp, Bang Chiến — mục 2). Với chúng, "cấp 90" là **THÊM** một
cổng chứ không phải nới. Và bỏ điều kiện *thời gian đã vào bang* (5 ngày / 7 ngày / 1 ngày) sẽ mở
đường cho **nhảy bang đánh thuê**: vào bang trước giờ mở 1 phút là đánh được. Chủ game quyết thì
làm theo, nhưng nên biết mình đang đổi cái gì.

### 0.1 Chốt phạm vi thi công (chủ game trả lời 21/08)

| Câu hỏi | Trả lời |
|---|---|
| "Giống 100% bản Linux" với Tín Sứ nghĩa là gì? | **Chép đúng hiện trạng Linux** — 2 tuyến Thành Đô ↔ Đại Lý, **chỉ ải Thiên Bảo Khố (map 395)**. Các khối bị VNG comment tắt (40 tuyến, 2 ải Phong Chi Kỵ / Sơn Thần Miếu, 3 mức cấp, đồng hồ 7200 giây, nâng danh hiệu Tín Sứ) **GIỮ NGUYÊN TRẠNG TẮT**. Chỉ đổi cấp 120 → 90. |
| Ai restart GameServer sau mỗi đợt? | **Chủ game tự restart.** Claude sửa + build + đặt sẵn binary/script rồi báo. |

⚠️ Hệ quả đã báo trước và chủ game chấp nhận: bản tự viết `tinhnang/thienbaokho` đang có Dịch Quan ở
**7 thành**; thay bằng bản Linux thì còn **2 thành**. Đây là lựa chọn có ý thức, không phải sót.

### 0.2 Yêu cầu bổ sung (21/08)

- Tính năng nào bản dự án **trùng** thì **gỡ bản tự viết, thay bản Linux vào** (Lôi Đài Bang Hội ↔
  `tinhnang/loidai`; Tín Sứ ↔ `tinhnang/thienbaokho`).
- Phải giống 100% bản Linux **từ item đến hình ảnh**; **thiếu item thì làm thêm item**.
- **Ưu tiên cao nhất: các hoạt động bang hội + BOSS BANG HỘI.**
- Làm luôn **bang hội chiếm lĩnh trên bản đồ + thông tin thuế mỗi thành**.
- **Mọi nhiệm vụ lúc nhận phải hiện thông tin ở Chỉ Nam Nhiệm Vụ (F11).**

---

## 1. BẢNG TỔNG — 30 GIÂY

| # | Hoạt động | Cách hoạt động (1 câu) | Cổng hiện tại | Sau khi hạ |
|---|---|---|---|---|
| L1 | **Lôi đài tỷ võ** | 2 người lập tổ đội 2 người → đội trưởng đăng ký cỡ trận 1v1…8v8 → mỗi phe được 1 **số thứ tự**, đồng đội gõ số đó để vào map 209 → 2 phút chuẩn bị, đánh 10 phút | tổ đội đúng 2 người + là đội trưởng. **KHÔNG cấp, KHÔNG tiền, KHÔNG bang** | + cấp ≥ 90 |
| L2 | **Bách Nhân Lôi Đài** ("Lôi Đài Hoàng Thành Tư") | Vào map 960 có **5 đài**, dùng khinh công nhảy lên đài trống thành **Lôi Chủ**, ai nhảy lên sau là khiêu chiến, đánh 3 phút/lượt, thắng thì giữ đài lên bậc | **cấp ≥ 120** + khung giờ 12:00–24:00 | **cấp ≥ 90** |
| L3 | **Cảnh Kỹ Trường** | Bấm bản đồ nhỏ góc phải để báo danh → hệ thống ghép cặp theo **30 bậc điểm** → ném vào bản sao map 975 → 60 giây chuẩn bị, đánh 5 phút, ai chịu ít sát thương hơn thì thắng | **KHÔNG có cổng cấp nào** | + cấp ≥ 90 |
| L4 | **Lôi Đài Bang Hội** | Relay ghép cặp 2 bang → mở 1 trong 8 sân (map 213-220) → thành viên 2 bang tự vào chọn phe, 16 người/phe → chết là bị loại khỏi sân, hết giờ bên nào còn nhiều người hơn thì thắng | thuộc 1 trong 2 bang được ghép + **đã vào bang ≥ 7200 phút = 5 ngày**. **KHÔNG cấp** | + cấp ≥ 90, **bỏ 5 ngày** |
| T1 | **Tín Sứ** | Nhận thư ở **Dịch Quan** (Thành Đô / Đại Lý) → Xa Phu đưa vào ải Thiên Bảo Khố (map 395) → hạ Thủ Hộ Giả rồi **mở đúng 5 trong 9 Bảo Rương theo đúng thứ tự** hệ thống bốc → gặp Tiêu Trấn ra ải → về Dịch Quan trả nhiệm vụ | **cấp ≥ 120** + 2 lượt/ngày (+1 lượt nếu có Thiên Bảo Khố Lệnh) + 5 ô trống hành trang | **cấp ≥ 90** |
| B1 | **3 Hoạt động Phường** (Chiêu mộ đệ tử · Niên Thú · Thu thập vật tư) | NPC **Tổng quản Hoạt động phường** trong lãnh địa bang → trả **điểm cống hiến** báo danh → vào sân chờ, gom đủ ≥ 5 người thì chia nhóm 10 → chơi mini-game tính điểm xếp hạng | có bang + phường đã xây + cống hiến 800/500/300 + 5 lần/ngày. Riêng **Niên Thú cấp ≥ 50** | **cấp ≥ 90** (Niên Thú), 2 cái kia + cấp 90 |
| B2 | **Cổ Tháp Bang Hội** | Phó bản **riêng từng bang** (bản sao map 996), gom 25-60 người + tối đa 15 người hỗ trợ ngoài bang → Bang chủ/Trưởng lão bấm mở từng ải → đánh 6 ải boss, qua ải được Kim Bảo rương, reset mỗi Chủ Nhật | **vào bang ≥ 7 ngày** + **trùng sinh ≥ 3** (đúng 3 thì cấp ≥ 150) | **cấp ≥ 90**, bỏ 7 ngày + trùng sinh |
| B3 | **Bang Hội Thành Bảo** | **Chủ Nhật 17:00–19:00 tại Lâm An**: bang chủ nhà thủ trong thành bảo (map 984) và thuê Thủ Vệ giữ cây; các bang khác vào chặt **Thần Mộc** lấy điểm, đổi ra **Thần Mộc Lệnh** | **trùng sinh ≥ 5** (hoặc =4 và cấp ≥ 150) + **vào bang ≥ 1 ngày** + thành phải có chủ | **cấp ≥ 90**, bỏ trùng sinh + 1 ngày |
| B4 | **Bang Chiến** (Võ Lâm Đệ Nhất Bang) | 7 bang chiếm thành mời các bang khác **lập liên minh** → mỗi ngày 3 cặp thành đánh 1 trận: 30 phút báo danh + 90 phút giao tranh, 150 người/phe, có hệ **quân hàm 6 bậc** | có bang + bang thuộc liên minh + **trùng sinh ≥ 4** + **đã vào bang trước 0h 29/03/2014** + cấm 6 loại mặt nạ. **KHÔNG cấp** | + cấp ≥ 90, bỏ trùng sinh, **bỏ mốc 2014** |

---

## 2. GIẢI THÍCH TỪNG HOẠT ĐỘNG

### L1 — LÔI ĐÀI TỶ VÕ (`missions/bw`, map 209/210/211)

**Cách hoạt động.** Hai người muốn tổ chức trận **lập chung một tổ đội đúng 2 người** rồi tới NPC
**Công Bình Tử** (Dương Châu 80 / Tương Dương 78 / Thành Đô 11). Đội trưởng chọn cỡ trận **1v1 → 8v8**.
Hệ thống lập tức biến 2 người đó thành **đội trưởng của 2 phe**, mỗi người nhận một **số thứ tự ngẫu
nhiên 1–9999** (`bw_getkey`, bwmanager.lua:108-122). Đồng đội tới gặp Công Bình Tử **gõ đúng số đó**
mới vào được phe tương ứng. Trận 1v1 thì không phát số — chỉ 2 đội trưởng đánh nhau.
Báo danh 2 phút, đánh **10 phút**, 20 giây báo tình hình một lần. Khán giả vào phe 3 và **bị tàng hình**
(`ChangeOwnFeature`, bwhead.lua:158). Hết giờ hoặc một bên sạch người thì kết trận.

**Điều kiện.** Tổ đội đúng 2 người · là đội trưởng · sân đang rảnh (cả server 1 trận tại một thời điểm).
**Miễn phí hoàn toàn** — grep `Money|Cost|Cash|Pay|Item` trên cả thư mục = 0 kết quả. **Không cấp độ,
không bang hội, không giới hạn lượt/ngày.**

> 🔴 **NHƯNG: tính năng ĐÃ BỊ KHOÁ CỨNG.** Dòng đầu tiên của `main()`:
> `bwmanager.lua:7` → `do Talk(1, "", "Chức năng đã đóng.") return end` — **không có `--`, đang chạy**.
> Toàn bộ `missions/bw` là mã chết trên bản VNG. Port thì phải xoá dòng này.

---

### L2 — BÁCH NHÂN LÔI ĐÀI / "LÔI ĐÀI HOÀNG THÀNH TƯ" (`missions/bairenleitai`, map 960)

**Cách hoạt động.** Mở **12:00 → 24:00 hằng ngày**. NPC **"Quan nhắc nhở Hoàng Thành Tư"** đưa vào map
960 có **5 lôi đài**, trong đó **Đài 1 là Đài Chủ** (thắng ở đài này mới được vinh danh).
Không xếp lượt, không báo danh: **dùng khinh công nhảy lên ô trap của đài**. Đài trống → bạn thành
**Lôi Chủ**, chờ 30 giây. Ai nhảy lên sau là **người khiêu chiến**, đếm ngược 3 giây rồi đánh
**tối đa 3 phút**; hoà thì **ai chịu ít sát thương hơn** giữ đài. Không ai khiêu chiến trong 30 giây
thì hệ thống thả **cao thủ NPC của 1 trong 10 phái** (npcId 1786-1795, cấp 90) lên đánh — **Lôi Chủ
phải hạ trong 3 phút, không thì tính là bại**. Thắng thì `nGrade + 1` và giữ đài tiếp.
NPC **Cổ Thủ** đánh trống cho hiệu ứng **nhân đôi kinh nghiệm** (mỗi lượt nhận 1 lần, có hạn suất).

**Điều kiện.** **Cấp ≥ 120** (`hundred_arena.lua:411`) · trong khung 12:00–24:00 · không báo danh,
không mất phí, không tổ đội/môn phái/bang hội/trùng sinh. Trong map: cấm phù hồi thành, cấm mở sạp,
**ở lì tối đa 90 phút**, trần cộng exp 50 lượt/ngày (daily task 2709).

> 🟡 Lệnh `SetAForbitSkill(210, 1)` (cấm khinh công) **đã bị comment** — may, vì bật thì không ai
> lên được đài. Đừng "sửa lại" thành bật.
> 🔴 Tác tử rà soát báo: NPC cửa vào **bị `ClearNPCNewVersion()` xoá khỏi 28 map lúc khởi động**
> (`autoexec.lua:214, :275-286` — map 176 nằm trong danh sách). Cần kiểm chứng lại khi port.

---

### L3 — CẢNH KỸ TRƯỜNG (`missions/arena`, map mẫu 975)

**Cách hoạt động.** Người chơi **bấm chuột phải vào biểu tượng bản đồ Cảnh Kỹ Trường ở góc phải giao
diện** để báo danh. Relay xếp mọi người vào **30 bậc điểm** (`tbPlayerManage:Init(30)`, player.lua:357),
ghép cặp **trong cùng bậc trước**; chờ quá **30 giây** thì nới dần sang bậc liền kề (i−1, j+1…).
Ghép xong → cấp một **bản sao map 975** riêng cho cặp đó → 60 giây chuẩn bị, đếm ngược 5 + 5 →
đánh **5 phút**. Ai chết thì thua; hết giờ thì **ai chịu ít sát thương hơn thắng** (`ST_GetDamageCounter`).
Kết quả cộng/trừ điểm rank (task 3172), đủ **20 trận/tuần** thì nhận thưởng ở NPC Quan Viên.

**9 danh hiệu theo mốc rank:** 2600+ Độc Cô Cầu Bại · 2400 Võ Lâm Chí Tôn · 2200 Nhất Đại Tông sư ·
2000 Tuyệt Thế Cao Thủ · 1800 Vang Danh Giang Hồ · 1600 Hiệp Danh Viễn Bá · 1400 Võ Lâm Tân Tú ·
1200 Mới nhập giang hồ · dưới nữa Nhập Môn Đệ Tử.

**Điều kiện.** **KHÔNG có cổng cấp độ nào** — 9 tệp không có một dòng `GetLevel` nào. Trong map: cấm
tổ đội, cấm mở sạp, cấm 2 loại đồ (CALLNPC/TRANSFER), **cấm 76 loại thuốc buff riêng cho map 975**,
gỡ sạch 90 trạng thái buff khi vào.

> 🔴 **Đây là phế tích, không phải tính năng đang chạy.** Phản biện xác nhận **5 mảnh CHẾT**:
> giao thức báo danh `emSCRIPT_PROTOCOL_SIGNUP_AREAN` **đã comment ở cả 2 nơi**; hàm `apply_signup`
> phía GameServer **không tồn tại**; 3 hàm `on_player_enter_map` / `on_player_leave_map` /
> `on_begin_battle` mà `rule.lua` gọi **không tồn tại ở bất kỳ đâu**; **lớp `tbMember`
> (giữ điểm rank) KHÔNG TỒN TẠI** dù `rule.lua:101` gọi `tbMember:new()`; `common.lua` **không
> tệp nào Include**. ⇒ Port Cảnh Kỹ Trường = **phải tự viết lại** phần thiếu, không chỉ chép.

---

### L4 — LÔI ĐÀI BANG HỘI (`missions/citywar_arena`, map 213-220)

**Cách hoạt động (bản gốc Linux).** **Không có bước báo danh thủ công.** Relay/Công Thành Chiến tự
ghép cặp 2 bang rồi mở 1 trong **8 sân**. Mission toàn cục `citywar_global` poll mỗi 5 phút; sân nào
`IsArenaBegin` bật thì thành viên **2 bang được ghép** tới NPC chọn phe, **16 người/phe**.
Chết là **bị loại khỏi sân**. Hết giờ **bên nào còn nhiều người hơn thì thắng**; bang thắng
**+1200 điểm kinh nghiệm bang**, bang thua **−1400**.

**Điều kiện (bản gốc).** Thuộc đúng 1 trong 2 bang được ghép (`GetTongName() == GetMissionS(1|2)`) ·
**`GetJoinTongTime() >= 7200`**. Vào là **bị đá khỏi tổ đội**, bị ép bật PK, cấm đổi phe bang.
**Không cấp độ, không tiền, không vật phẩm, không ô trống hành trang.**

> 🔴 **Đính chính một sai số của tác tử:** tác tử L4 ghi "7200 giây = 2 giờ". **SAI.**
> `GetJoinTongTime()` trả về **PHÚT** — chứng minh 3 chỗ: `tong_guta` dùng `7*24*60 = 10080` kèm
> chú thích "7 ngày"; `tongcastle` dùng `60*24 = 1440` kèm câu "nhiều hơn 1 ngày";
> `tongwar_signup.lua:15` viết `GetJoinTongTime()*60` rồi so với `GetCurServerTime()` (giây).
> ⇒ **7200 phút = 120 giờ = 5 NGÀY.**

> 🟡 Các câu thoại "phải là bang chủ", "đặt cược 1000 vạn lượng" trong `OnHelp` (camper.lua:44,
> manager.lua:37) là **chữ hiển thị của một hệ khác, KHÔNG có mã thực thi** trong `citywar_arena`
> — `head.lua:81-129` không có một lệnh `Pay` nào. Đừng chép câu thoại rồi tưởng có thu tiền.
> Điều kiện "bang chủ + đội trưởng + 1.000.000 lượng + thứ Năm 18:00-20:29" là của **bản tự viết
> `tinhnang/loidai` trong dự án**, không phải bản gốc.

---

### T1 — TÍN SỨ (`task/tollgate/messenger`, map 395)

**Cách hoạt động.**
1. **Dịch Quan** (chỉ ở **Thành Đô 11** hoặc **Đại Lý 162**) giao thư → task 1204 = tuyến, task 1203 = 10.
2. **Xa Phu** đứng cạnh đưa vào ải **Thiên Bảo Khố** (map 395, điểm vào 1417,3207).
3. NPC cửa ải: **"Bắt đầu nhiệm vụ"**. Nếu đang tổ đội thì **chỉ đội trưởng bấm được**, cả đội cùng
   nhận **một mã**. Hệ thống bốc ngẫu nhiên **5 số trong 9 rương** → đó là **thứ tự phải mở**.
   Vào ải được **bất tử 3 giây** chống đồ sát.
4. Trong ải: hạ **Bảo Khố Thủ Hộ Giả** đúng số rồi mới mở được rương số đó; mở **sai thứ tự là hỏng lượt**.
5. Đủ 5 rương → gặp **Tiêu Trấn** ra ải (task 1203 = 30) → về Dịch Quan trả nhiệm vụ.
6. Thưởng: **2 Tín Sứ Bảo Rương** mỗi lượt, lần đầu trong ngày thêm **3 Hành Hiệp Lệnh**, tối đa
   2 lần/ngày nhận nguyên liệu kinh mạch.

**Điều kiện.** **Cấp ≥ 120** (`posthouse.lua:132`) · đứng ở map 11 hoặc 162 · không đang dở nhiệm vụ
khác (task 1204 == 0) · **2 lượt thường/ngày**, lượt thứ 3 phải **có sẵn Thiên Bảo Khố Lệnh (6,1,2813)**
trong người · trả nhiệm vụ cần **5 ô trống hành trang**.

> 🟡 Rà soát phát hiện: **KHÔNG hề có điều kiện "phải về đúng thành đích mới trả được"**.
> `messenger_finishtask` chỉ xét task 1203 == 30 hoặc 25, không đối chiếu vị trí. Người nhận ở
> Thành Đô vẫn trả ngay tại Thành Đô được.
> 🔴 **5 Yêu bài Tín Sứ (Mộc/Đồng/Ngân/Kim/Ngự Tứ) là đồ chết**: chúng đòi task 1206 ≥ 1..5, mà hàm
> duy nhất tăng task 1206 (`messenger_getlevel`) **đã bị comment**.

---

### B1 — BA HOẠT ĐỘNG PHƯỜNG BANG HỘI (`missions/tong`)

**Cách hoạt động.** Cả ba đi chung một cổng: NPC **"Tổng quản Hoạt động phường"** đặt **trong lãnh
địa bang mình**. Người chơi trả **điểm cống hiến** để báo danh → vào **sân chuẩn bị** → gom đủ
**≥ 5 người** thì xáo trộn và **chia nhóm 10 người** (`chaos`) → đẩy vào bản đồ chơi (`gogamemap`,
**trừ cống hiến lần 2**) → chơi mini-game tính điểm → xếp hạng, nhận thưởng ở chính NPC đó.
Dưới 5 người thì **huỷ đợt** và trả về vị trí cũ.

| | Chiêu mộ đệ tử | Đánh Niên Thú | Thu thập vật tư |
|---|---|---|---|
| Lịch (theo lời NPC) | 0h, 3h, 6h… 21h | 1h, 4h, 7h… 22h | 2h, 5h, 8h… 23h |
| Mission / map | 37 / 38 → 821, 822 | 39 / 40 → 823, 824 | 44 / 45 → 827, 828 |
| Cống hiến (cổng NPC) | **800** | **500** | **300** |
| Sức chứa toàn server | 120 | 100 | 120 |
| Lượt/người/ngày | 5 | 5 | 5 |
| Cấp độ | không | **≥ 50** | không |

**Điều kiện chung.** Có bang · **đứng trong lãnh địa bang** · bang đã **xây Hoạt động phường**, cấp
dùng ≥ 1, phường **đang mở cửa**, bang **không ở trạng thái tạm ngừng** · đúng **pha báo danh** ·
**trần 20 lượt người/bang/ngày cho mỗi hoạt động**. Không cần chức vụ, không cần tổ đội, không mất tiền.

> 🔴 **Lệch có thật giữa cổng NPC và lúc vào sân**: NPC kiểm 300 cống hiến cho Thu thập vật tư
> (`TONGGXD[3][1]`) nhưng `collectgoods/schedule/preparetimer.lua:63` lại trừ **`TONGGXD[1][1]` = 800**.
> Chiêu mộ đệ tử cũng trừ 800, Niên Thú trừ 500. Phải chốt lại khi port.
> 🔴 **Bug reset trần 20 lượt**: `tong_head.lua:40-45` dùng `GetLocalDate("%d")` = **ngày trong tháng**.
> Bang dùng hết 20 lượt ngày 15, nếu không chơi lại thì phải chờ **đúng ngày 15 tháng sau** mới reset.
> (Bộ đếm cá nhân dùng `%y%m%d` nên đúng.)
> 🟡 Mục menu "Tham gia hoạt động thu thập vật phẩm" **đã bị comment khỏi NPC** — nhánh `use_g_5_ok`.

---

### B2 — CỔ THÁP BANG HỘI (`missions/tong_guta`, map mẫu 996)

**Cách hoạt động.** Mỗi bang có **một bản sao map riêng**. Thành viên vào tháp qua NPC **"Kinh Thanh"**
(Thành Đô, 3100/5050). Khi trong tháp đủ **25–60 người**, **Bang chủ hoặc Trưởng lão** bấm **mở từng ải**.
Đánh lần lượt **6 ải boss**, mỗi ải qua được cho bang **1–7 Kim Bảo rương**. Đồ rơi chia bằng
**roll xúc xắc 15 giây**. Điểm: vào trận 200 · qua ải còn sống 300 · kết liễu boss ải cuối 1000 ·
người hỗ trợ 150. **Toàn bộ tiến độ reset mỗi Chủ Nhật** (bảo trì 23:45 → 00:15).

**6 ải:** Dung Hạ (Ngũ Thánh — để 3 loại cùng lúc là thua) · Chiêm Bình (tứ đại thần thú) ·
Vô Tình + Lãnh Huyết (2 boss không được đứng gần nhau) · Hoàn Nhan Quân Sĩ (Hắc Y Nhân theo ngũ hành
tương khắc) · Quách Nham (3 giai đoạn Phong/Hoả/Thần Thí + thu thập Thanh Thủy Mẫu Hoa) · Trương Phiền (3 pha).

**Điều kiện.** Có bang · **vào bang ≥ 7 ngày** · **trùng sinh ≥ 3** (đúng 3 thì cấp ≥ 150) ·
tháp không đang chiến đấu / không trong giờ bảo trì / bang chưa thông hết 6 ải trong tuần ·
toàn server tối đa 50 bản đồ Cổ Tháp cùng lúc.
**Người hỗ trợ (khác bang):** cùng điều kiện trùng sinh, **tối đa 3 lần/tuần**; phải được **Bang chủ**
dẫn theo tổ đội, và **mọi thành viên tổ đội đều phải đạt điều kiện**.
**Mở ải:** phải là người của chính bang chủ nhà, chức vụ Bang chủ (0) hoặc Trưởng lão (1).
**Lãnh Kim Bảo rương:** chỉ Bang chủ, cần ≥ 1 ô trống.

---

### B3 — BANG HỘI THÀNH BẢO (`missions/tongcastle`, map 984)

**Cách hoạt động.** **Chủ Nhật hằng tuần, chỉ tại Lâm An**, 17:00–19:00. Bang **đang chiếm thành**
là chủ nhà, thủ trong thành bảo và **thuê Thủ Vệ** (tối đa 100/bản đồ, mỗi con tốn 1 phù triệu hồi,
chỉ đặt được gần Thần Mộc). **Mọi bang khác đều vào được**, đi 4 cổng ngoài để **chặt 17 cây
"Thượng Cổ Thần Thụ"** lấy **điểm Thần Mộc**.
Ba phòng mở dần: **Nhân Tự Phòng 17:00** · **Địa Tự Phòng 18:00** (phải chặt hết cây Thanh Đồng của
hướng đó mới xuống được đường hầm) · **Thiên Tự Phòng 18:30** (phải chặt hết cả 4 cây Bạch Ngân).
19:00–24:00 là cửa sổ **đổi điểm Thần Mộc** lấy **Thần Mộc Lệnh** (Thanh Đồng 3205 giá 10 · Bạch Ngân
3206 giá 20 · Hoàng Kim 3207 giá 120), tối đa **5 cái/tuần/mỗi loại**.

**Điều kiện.** Có bang · thành Lâm An **đang có chủ** · **trùng sinh ≥ 5** (hoặc = 4 và cấp ≥ 150) ·
**vào bang ≥ 1 ngày** · đúng Chủ Nhật, đúng giờ.
**Nhận điểm thưởng** còn cần: ở lì **≥ 45 phút**, **1 lần/ngày**, trong khung 19:00–24:00.

> 🔴 **Phản biện bắt được một chỗ báo ngược (mức NẶNG):** `CheckTrapCondition` khi `GetTmpCamp()==1`
> in câu *"hạn chế không truyền tống"* rồi **`return 1`** — mà **1 = CHO PHÉP**
> (`tongcastle.lua:368-373`, đối chứng: 2 nhánh chặn thật `return` rỗng ở `:388` và `:416`;
> nhánh cho qua `return 1` ở `:419`). Nghĩa là bang chủ nhà **không những dùng được trap, mà còn bỏ
> qua toàn bộ kiểm tra giờ mở phòng và kiểm tra đã chặt hết Thần Mộc**. Câu tiếng Việt là thông báo
> gây hiểu nhầm, không phải logic. Port thì giữ nguyên hay sửa — phải quyết có ý thức.

---

### B4 — BANG CHIẾN / VÕ LÂM ĐỆ NHẤT BANG (`missions/tongwar` + `event/tongwar`)

**Cách hoạt động.** Mùa giải chia **3 pha** (1 = báo danh/lập liên minh · 2 = thi đấu · 3 = nhận thưởng).
Pha 1: **bang chủ của bang đang chiếm 1 trong 7 đại thành** chọn thành đại diện, rồi **lập liên minh**
với các bang khác (phải tổ đội, người lập là đội trưởng, **mọi thành viên tổ đội đều phải là bang chủ**).
Pha 2: mỗi ngày 3 cặp thành đánh — **30 phút báo danh + 90 phút giao tranh**, **150 người/phe**,
map riêng theo cặp (605-613). Giết người **+75 điểm**, chuỗi liên trảm tối đa **+150**;
**chết quá 10 lần là hết lượt**; sau mỗi lần chết chỉ được ở hậu doanh **120 giây**.
**Quân hàm 6 bậc** theo điểm tích luỹ (0 / 10.000 / 20.000 / 40.000 / 60.000 / 80.000):
Binh Sĩ → Hiệu Úy (+20% HP) → Thống Lĩnh (+30% HP, +5% thủ) → Phó Tướng (+40%, +10%) →
Đại Tướng (+50%, +15%) → Nguyên Soái.

**Điều kiện vào đấu trường.** Có bang · **bang phải thuộc một liên minh chiếm thành** ·
**trùng sinh ≥ 4** · **đã vào bang trước 0h 29/03/2014** · **không đeo 6 loại mặt nạ tăng điểm
Tống Kim** (particular 482, 447, 450, 446, 647, 806) · trận đã mở. **Không có cổng cấp độ nào.**

> 🔴 **Rà soát: Bang Chiến hiện KHÔNG CÓ LỐI VÀO.** `tongwar_want2signup` chỉ được gọi từ menu
> `tongWar_Start` (`event/tongwar/head.lua:191`), mà `tongWar_Start` **không NPC nào gọi** — chỉ
> xuất hiện ở định nghĩa (`:169`) và 3 nút "Trở lại" của chính nó. NPC Sứ giả (lối vào duy nhất
> được đặt) chỉ có 3 mục: xem danh sách / báo danh 54 người / nộp vật phẩm hỗ trợ Tống Kim.
> ⇒ Muốn port phải **tự nối lại lối vào**.
> 🔴 Mốc **29/03/2014** hardcode: port nguyên xi = **không ai vào được**.
> 🟡 Cửa sổ ngày đăng ký danh sách 54 người cũng hardcode: `20140407`.

---

## 3. DANH SÁCH ĐÍCH DANH PHẢI SỬA (chính sách "cấp ≥ 90, không trùng sinh")

| # | Tệp : dòng | Hiện tại | Đổi thành |
|---|---|---|---|
| 1 | `missions/bw/bwmanager.lua:7` | `do Talk(1,"","Chức năng đã đóng.") return end` | **XOÁ dòng** (nếu không thì mọi thứ khác vô nghĩa) |
| 2 | `missions/bw/bwmanager.lua` (OnRegister) | không có cổng cấp | **THÊM** `if GetLevel() < 90 then ... return end` |
| 3 | `missions/bairenleitai/hundred_arena.lua:411` | `GetLevel() >= 120` | `GetLevel() >= 90` (sửa cả câu thoại `:414`) |
| 4 | `missions/arena/*` | không có cổng cấp | **THÊM** cổng cấp 90 tại điểm báo danh (phải tự viết, xem L3) |
| 5 | `missions/citywar_arena/camper.lua:81` và `:87` | `GetJoinTongTime() >= 7200` (5 ngày) | **BỎ**, thay bằng `GetLevel() >= 90` |
| 6 | `task/tollgate/messenger/posthouse.lua:132` | `GetLevel() < 120` | `GetLevel() < 90` (sửa cả câu thoại) |
| 7 | `task/tollgate/messenger/messenger_turerukou.lua:38` | `GetLevel() < 120` (đang comment) | nếu bật lại thì để 90 |
| 8 | `missions/tong/tong_springfestival/head.lua:19` | `SF_LEVELLIMIT = 50` | `SF_LEVELLIMIT = 90` |
| 9 | `missions/tong/tong_disciple/head.lua` + `collectgoods/head.lua` | không có cổng cấp | **THÊM** cổng cấp 90 |
| 10 | `missions/tong_guta/guta_config.lua:85` | `JOIN_TONG_TIME = 7*24*60` | `0` (bỏ) |
| 11 | `missions/tong_guta/guta_config.lua:86-87` | `MIN_LEVEL = 150`, `MIN_TRANSLIFE = 3` | `MIN_LEVEL = 90`, `MIN_TRANSLIFE = 0` |
| 12 | `missions/tong_guta/signup_npc.lua:24` và `:36` | `nTransLife < MIN_TRANSLIFE or (== và nLevel < MIN_LEVEL)` | rút gọn còn `GetLevel() < 90` |
| 13 | `missions/tongcastle/guideperson.lua:16-18` | `JOIN_TONG_TIME=60*24`, `TRANSLIFE_COUNT=4`, `MIN_LEVEL=150` | `0`, `0`, `90` |
| 14 | `missions/tongcastle/guideperson.lua:194` | `nTransLifeCount < 4 or (== 4 và GetLevel() < 150)` | `GetLevel() < 90` |
| 15 | `missions/tongcastle/guideperson.lua:199` | `GetJoinTongTime() < JOIN_TONG_TIME` | **BỎ** |
| 16 | `missions/tongcastle/guideperson.lua:24-25` | chuỗi `MSG_PLAYER_LIMIT` | viết lại: *"Phải đạt cấp 90 trở lên…"* |
| 17 | `missions/tongcastle/treedeath.lua:49-51` | `GetJoinTongTime() > JOIN_TONG_TIME` mới được cộng điểm chặt cây | **BỎ** |
| 18 | `event/tongwar/tongwar_signup.lua:15-21` | mốc `Tm2Time(2014,3,29,0,0)` | **XOÁ CẢ KHỐI** |
| 19 | `event/tongwar/tongwar_signup.lua:22-26` | `ST_GetTransLifeCount() < 4` | `GetLevel() < 90` |
| 20 | `missions/tongwar/trap/tongwar_trap.lua:65-73` | bản sao của 2 điều kiện trên (kiểm lại lần 2 khi đạp trap) | **sửa đồng bộ với #18/#19** |
| 21 | `event/tongwar/npc_shizhe.lua:341` | `GetJoinTongTime() < 10080` (7 ngày, cho danh sách 54 người) | **BỎ** |

**Giữ nguyên, KHÔNG đụng** (đây là luật chơi, không phải cổng chặn người mới):
điều kiện *phải có bang* · *phải là bang chủ / trưởng lão / đội trưởng* · cống hiến 800/500/300 ·
số lượt mỗi ngày · ô trống hành trang · trần người mỗi phe · khung giờ mở · cấm 6 mặt nạ Bang Chiến.

---

## 4. NHỮNG THỨ *KHÔNG* TỒN TẠI (đã soát, để khỏi đi tìm)

Rà soát toàn bộ 9 cây, **không hoạt động nào** kiểm: **giới tính** · **môn phái** (trừ Bang Chiến
đòi *có* môn phái khi đăng ký danh sách 54 người) · **trạng thái PK / đồ sát** · **trang bị đang đeo**
(trừ 6 mặt nạ Bang Chiến) · **tiền mang theo** (bản gốc **không** thu tiền ở đâu cả) ·
**hành trang đầy ở cổng vào** (chỉ kiểm lúc *nhận thưởng*).

**Cảnh báo suy diễn sai:** `tbMapSet[3]` (map Bang Chiến 605-613) có khai báo trong
`vng_feature/forbiditem/vngforbidspecialitem.lua:34-37` nhưng **không vật phẩm nào trỏ tới** —
chỉ map 975 (Cảnh Kỹ Trường) và 380 mới thật sự có danh sách cấm. Đừng tưởng Bang Chiến cũng cấm
76 loại thuốc như Cảnh Kỹ Trường.

---

## 5. BỐN THỨ ĐANG CHẾT — phải sửa dù có hạ cấp hay không

| Hoạt động | Chết ở đâu | Hệ quả |
|---|---|---|
| **Lôi đài tỷ võ** | `bwmanager.lua:7` khoá cứng | bấm NPC ra "Chức năng đã đóng" |
| **Cảnh Kỹ Trường** | giao thức báo danh comment + `apply_signup`, 3 hàm GS, lớp `tbMember` **không tồn tại** | không có đường vào, và có vào cũng không tính được điểm |
| **Bang Chiến** | `tongWar_Start` không NPC nào gọi + mốc 2014 | không có đường vào |
| **Yêu bài Tín Sứ** | `messenger_getlevel` comment ⇒ task 1206 không tăng được | 5 lệnh bài vĩnh viễn không dùng được |
