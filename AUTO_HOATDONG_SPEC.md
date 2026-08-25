# ĐẶC TẢ: AUTO TỰ ĐỘNG THAM GIA 4 HOẠT ĐỘNG MỚI (WAuto)

> Viết 24/08/2026 theo yêu cầu chủ game: *"note toàn bộ tính năng vừa làm để phiên sau có thể
> làm Auto tự động tham gia các hoạt động"*.
>
> 4 hoạt động vừa port từ bản Linux sang server JX1 này (23-24/08/2026):
> **Bang Chiến (Võ Lâm Đệ Nhất Bang)** · **Bách Nhân Lôi Đài** · **Tỷ Võ Đài** · **Bang Hội Thành Bảo**.
>
> Mọi số liệu trong tài liệu **đã kiểm chứng trực tiếp trên script sống**
> `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script` ngày 24/08 (có ghi `file:dòng`).
>
> **PHẠM VI: 100% phía CLIENT (WAuto). KHÔNG sửa server.** Đường đi của người chơi thật đã
> chạy được trên server (đã kiểm chứng bằng log sống). Nếu trong lúc thi công thấy "hình như
> cần server hỗ trợ" thì **DỪNG LẠI HỎI CHỦ GAME**, không tự sửa server.

---

## 0. ĐỌC GÌ TRƯỚC

| Đọc | Để làm gì |
|---|---|
| `D:\GAMEDEVNEW\AUTO_TONGKIM_SPEC.md` | **Auto anh em gần nhất** — cùng kiểu "canh lịch → tới NPC → báo danh → vào map → đánh → thoát". Chép khuôn xử lý lịch, phát hiện-vào-map, chống kẹt từ đó. |
| `D:\GAMEDEVNEW\AUTO_LIENDAU_SPEC.md` | Khuôn auto theo lịch + pha. |
| `D:\GAMEDEVNEW\BANGIAO_AUTO_DATAU_WAUTO.md` mục 13 | Cơ chế WAuto đã có sẵn: bấm thoại NPC, chọn dòng thoại, dùng item trong túi, dùng phù, di chuyển. **Đừng viết lại cái đã có.** |
| `D:\GAMEDEVNEW\BANGIAO_PORT5_2308.md` | Kiến trúc 4 hoạt động phía server + các quyết định lệch so với bản Linux. |
| `D:\GAMEDEVNEW\HUONGDAN_TEST_HOATDONG_2408.md` | Cách test tay từng hoạt động (dùng để đối chiếu khi auto chạy sai). |
| Ký ức `wauto-canonical-tree`, `wauto-luu-cau-hinh-apdata`, `gio-server-mui-gio-wauto`, `feedback-khong-tu-y-doi-cau-truc-auto` | 4 bẫy WAuto kinh điển. |

---

## 1. NỀN TẢNG KỸ THUẬT (kiểm chứng 24/08)

### 1.1 🔴 CÂY MÃ NGUỒN ĐÚNG CỦA WAuto

```
J:\CayChay\NangLen64bit\SwordOnline0_03_05\SwordOnline_AUTODEV\_WAuto
```
Build: `MSBuild _WAuto\WAuto.vcxproj -p:Configuration=Release -p:Platform=Win32`
→ ra thẳng `_WAuto\Release\WAuto.exe`.

🔴🔴 **`J:\CayChay\Src_Auto_Ngoai\WAuto\WAuto` là CÂY BẪY** — chính nó có file
`_DO_NOT_USE_WRONG_TREE.txt` cảnh báo: `WAuto.cpp` ở đó khác phiên bản (5.045 dòng) so với bản
thật (**11.077 dòng**); build ra không kết nối được với game.

### 1.2 Cấu trúc cấu hình auto

- Cấu hình nằm trong `struct autoData` tại `_WAuto\ipc_shared.h` (**411 dòng**), truyền qua
  **bộ nhớ chia sẻ** (`SHARED_SIZE = 512*128`), lưu xuống `APdata\<ID nhân vật>.dat`.
- 🔴 **Trường mới BẮT BUỘC thêm Ở CUỐI struct** (sau `bAutoLD` — trường cuối hiện tại, là auto
  Liên Đấu), nếu không sẽ hỏng file `.dat` cũ của người chơi.
- Đặt tên theo khuôn có sẵn: `bAutoLD` (Liên Đấu) → đề nghị `bAutoBangChien`, `bAutoBachNhan`,
  `bAutoTyVo`, `bAutoThanhBao` + các ô phụ (giờ, cấp, số lần…).
- 🔴 Luật cũ của chủ game: **sửa WAuto phải dựa vào ô cấu hình/luồng có sẵn**, không tự chế
  cấu trúc mới (ký ức `feedback-khong-tu-y-doi-cau-truc-auto`).

### 1.3 🔴 GIỜ — cạm bẫy lớn nhất

- Toàn bộ script dùng `GetLocalDate("%H%M")` = **giờ hệ điều hành MÁY CHỦ**.
- Máy chủ đang chạy **múi Pacific (PDT = UTC−7)**; giờ Việt Nam là UTC+7 ⇒ **lệch 14 giờ**.
- ⇒ Auto **KHÔNG được** so giờ máy người chơi. Phải quy đổi, hoặc tốt hơn: **canh theo trạng
  thái server** (map mở chưa, task đổi chưa) thay vì canh đồng hồ.

### 1.4 Một file chỉnh mọi thông số (chủ game có thể đổi bất cứ lúc nào)

`bin\server\script\header\cauhinh_hoatdong.lua` — **60 khoá** điều khiển giờ mở, cấp tối thiểu,
điểm, exp, giá đổi… của cả 4 hoạt động.

🔴 **Hệ quả cho auto**: **CẤM nhét cứng số giờ/cấp vào WAuto.** Chủ game đổi `BR_GIO_MO` từ 1200
sang 800 là auto sai ngay. Auto phải:
- hoặc đọc trạng thái thật (thử tương tác, xem có bị chặn không),
- hoặc để người chơi tự nhập giờ trong giao diện WAuto,
- hoặc (tốt nhất) bắt chuỗi thông báo từ chối của server để biết chưa tới giờ.

### 1.5 Cách kiểm chứng khi thi công

- Nhật ký hoạt động server: **`bin\server\logs\script_jx2.log`** — đây là nơi `OutputMsg`/
  `WriteLog` ghi (`KTongJX2.cpp:3988`), **không phải console**. Ví dụ dòng thật:
  `[2026-08-24 17:30:10] BaiRenLeiTai	CallNPC_GuShou:960,53376,99296`.
- Lỗi script: `ScriptError.log` **nằm cùng thư mục với file .lua bị lỗi**.
- Menu test tay: Lệnh Bài Admin (item 4814) → *"Hoạt động 23-24.08 (BC-BN-TV-TB): bộ test"*.

---

## 2. BẢNG TỔNG QUAN 4 HOẠT ĐỘNG

| | Bang Chiến | Bách Nhân Lôi Đài | Tỷ Võ Đài | Thành Bảo |
|---|---|---|---|---|
| **Lịch** | mùa 29/10→05/11/2026, 20:00–21:30 | **hằng ngày 12:00–24:00** | **không có lịch** — báo danh lúc nào cũng được | Chủ nhật 17:00–19:00 |
| **Cấp tối thiểu** | 90 | 90 | 90 | 90 |
| **Điều kiện khác** | có bang, bang trong liên minh | không | đội đúng 2 người, phải là đội trưởng | có bang, vào bang > 1 ngày, Lâm An phải có chủ |
| **Map đích** | 605/606/607 (qua 608–613) | 960 | 209 | 984 |
| **Phần thưởng** | điểm quân hàm + thưởng cuối mùa | **exp 1tr/5 phút** (Lôi Chủ +2tr) | nhiệm vụ nhánh | điểm Thần Mộc → đổi item |
| **Auto nên ưu tiên** | ⭐⭐ (chờ tới mùa) | ⭐⭐⭐⭐⭐ **cày exp mỗi ngày** | ⭐⭐ (cần 3 người) | ⭐⭐⭐ (1 buổi/tuần) |
| **Sẵn sàng chưa** | ✅ server chạy | ✅ **đã chạy thật** | ✅ server chạy | ⚠️ **client thiếu map 984** |

🔴 **Khuyến nghị thứ tự thi công**: **Bách Nhân trước** (dễ nhất, lợi nhất, đã chứng minh chạy
thật, không cần bang/đội) → Thành Bảo → Bang Chiến → Tỷ Võ (khó nhất vì cần 3 người phối hợp).

⚠️ **Thành Bảo hiện chưa vào được**: map 984 **chưa có dữ liệu phía client** — người chơi vào sẽ
đen màn. Phải chờ chủ game cấp pak client rồi mới test auto được.

---

## 3. CHI TIẾT TỪNG HOẠT ĐỘNG

*Mỗi hoạt động một mục: điểm vào, cây thoại chính xác, biến trạng thái, đường đi, cách thoát, bẫy.*

---

## 4. BÁCH NHÂN LÔI ĐÀI — *ưu tiên thi công #1*

### 4.1 Luồng vào (3 bước, không có map trung gian)

```
Map 176 Lâm An, ô (1464,3223) — NPC tpl 1747 "Quan nhạc nhỏ Hoàng Thành Từ"
  └► lựa chọn 1: "Lôi Đài Hoàng Thành Từ"                    [npc_quan_jx1.lua:11]
      └► lựa chọn 1: "Ta muốn vào Lôi Đài Hoàng Thành Từ"     [npc_enter.lua:16]
          └► NewWorld(960, …) — spawn ngẫu nhiên 1 trong 10 ô của inmap.txt
```
Toạ độ NPC đọc từ `settings\maps\chrismas\enternpc.txt` dòng 2 (46848,103136 → ô 1464,3223).

**Chỉ 2 điều kiện chặn** (`hundred_arena.lua:407-422`), cả hai đều CÓ thoại báo:
1. giờ máy chủ ≥ `BR_GIO_MO` (mặc định 12:00) — chưa tới giờ: *"Thời gian mở Lôi Đài mỗi ngày vào lúc 12:00 ~ 24:00…"*
2. cấp ≥ `BR_CAP_TOITHIEU` (90) — thiếu cấp: *"Cấp 90 và 90 trở lên mới có thể tham gia."*

Không cần bang, không cần tổ đội, không tốn vật phẩm.

### 4.2 Lên đài

5 đài, toạ độ **ô** (`missions\bairenleitai\head.lua:11-80`):

| Đài | Tâm đài (InPos) | 4 điểm bị đẩy ra (OutPos) | Ô trap |
|---|---|---|---|
| **1 (đài chủ)** | **1784, 3099** | (1776,3087) (1776,3115) (1796,3086) (1796,3116) | `arena1.txt` — 128 ô |
| 2 | 1744, 3061 | (1737,3051) (1737,3077) (1757,3050) (1758,3077) | `arena2.txt` — 129 ô |
| 3 | 1822, 3138 | (1816,3125) (1814,3153) (1836,3127) (1835,3153) | `arena3.txt` — 127 ô |
| 4 | 1705, 3101 | (1699,3087) (1698,3116) (1720,3088) (1719,3117) | `arena4.txt` — 126 ô |
| 5 | 1781, 3178 | (1774,3192) (1776,3163) (1795,3164) (1795,3190) | `arena5.txt` — 134 ô |

Điểm hồi sinh chung: **1749, 3135**. File trap ở `settings\maps\missions\bairenleitai\`, giá trị trong file là ô×32.

🔴 **Phải dùng KHINH CÔNG để lên đài** — map có 33 vật cản, và game nhắc thẳng: *"Xin hãy sử dụng khinh công để nhảy lên lôi đài tỷ võ."* Auto đi bộ thuần có thể không bao giờ chạm được ô trap.

### 4.3 Máy trạng thái đài — auto phải bám

| Trạng thái đài | Đạp trap lúc này sẽ ra gì |
|---|---|
| 0 = trống | Bạn thành **Lôi Chủ**, đài hẹn giờ 30 giây chờ người khiêu chiến |
| 1 = có Lôi Chủ, đang chờ | Bạn thành **người khiêu chiến**. 🔴 **Nếu bạn CHÍNH LÀ Lôi Chủ ⇒ BỎ ĐÀI, mất chuỗi thắng** |
| 2 = đếm ngược / đang đánh | Bị từ chối + đẩy ra OutPos: *"Lôi đài đã tiến hành tỷ võ, xin hãy đợi lượt báo danh sau."* |

Nhịp: chờ khiêu chiến **30s** → đếm ngược **3s** → đánh **3 phút**. Hết 3 phút không ai chết thì **ai CHỊU ít sát thương hơn thì thắng**; hoà thì Lôi Chủ giữ đài.

Không ai lên khiêu chiến trong 30s → server tự gọi **NPC cao thủ** (template 1786-1795), cấp = `90 + floor((số trận thắng−1)/10)`, trần 99. 🔴 Máu NPC nhân tới **×100** ở cấp 99 — từ khoảng trận 40-50 trở đi auto gần như chắc chắn thua vì không hạ nổi trong 3 phút.

### 4.4 Phần thưởng — CHỈ CÓ KINH NGHIỆM, không có vật phẩm

Mỗi **5 phút** (mốc phút chia hết cho 5 theo giờ máy chủ):
- Người có mặt trong map: **+1.000.000 exp** (×2 nếu đang có buff Cổ Thụ)
- **Lôi Chủ** được **+2.000.000 exp** nữa

🔴 **Trần 50 lượt/ngày** (task **2709**). Hết trần thì **vẫn vào map, vẫn đánh, chỉ không có exp — và KHÔNG có bất kỳ thông báo nào**. Auto phải tự đếm.
Lôi Chủ có buff x2 tiêu **4 lượt mỗi 5 phút** ⇒ cháy hết 50 lượt sau **~62 phút**.

**Buff x2**: mỗi 30 phút server thả NPC **"Cổ Thụ"** (tpl 1571) ở 1 trong 15 vị trí, loa *"Cổ Thụ đã xuất hiện mọi người nhanh chân đi tìm đi!"*, tồn tại 15 phút. Nói chuyện → **chọn lựa chọn 1** → buff 30 phút. Số suất = 20% số người đang nhận exp.

### 4.5 Biến trạng thái auto đọc được

| Task | Ý nghĩa |
|---|---|
| **2709** | Lượt exp trong ngày, mã hoá `YYMMDD*256 + số_lượt` → còn lại = `50 − mod(2709,256)` (chỉ tính khi phần ngày khớp hôm nay) |
| **2323 / 2324 / 2325** | map / x / y **nơi đứng trước khi vào** — server dùng để trả người chơi về |

🔴 **KHÔNG có task nào cho biết "đang đứng trên đài"** — trạng thái đó chỉ nằm trong bảng Lua phía server. Auto phải suy bằng **vị trí trùng InPos của đài** + `GetFightState()` + các dòng loa. Dấu hiệu rời đài: bị dịch ra 1 trong 4 OutPos kèm *"Ngươi đã rời khỏi Lôi Đài"*; lên đài: *"Ngươi đã vào Lôi Đài"*.

⚠ 4 task trên dùng `SetTask` **không kèm** `SyncTaskValue` ⇒ **chưa xác định** client có thấy giá trị mới ngay không. **Phải kiểm chứng trước khi dựa vào.**

### 4.6 Thoát ra

- Rời đài (khi đang là Lôi Chủ chờ khiêu chiến): đạp lại ô trap của chính đài đó.
- Rời map: NPC **"Xa phu"** ô **(1750, 3159)** → 7 thành. (Trong map còn Rương ô 1726,3141 và Chợ dược ô 1748,3117.)
- 🔴 Bị đá tự động sau **90 phút không hoạt động** → trả về task 2323/2324/2325. Đồng hồ reset khi vào/rời đài, khi bắt đầu đánh, khi nhận buff Cổ Thụ ⇒ **auto cắm exp phải định kỳ lên-xuống đài**.
- **0h** server trả tất cả về, loa *"Lôi Đài Hoàng Thành Từ hôm nay kết thúc !"*.

### 4.7 Bẫy riêng

1. 🔴 Lựa chọn cuối "Kết thúc đối thoại" ở NPC lối vào trỏ tới hàm **`oncancel` không tồn tại** (`npc_enter.lua:18` và `:31`) — có thể ném lỗi script. Auto nên đóng hộp thoại bằng cách huỷ, đừng chọn mục cuối.
2. 🔴 Chết trong map **không bị đưa ra ngoài** (script giữ nguyên hành vi gốc bản Linux) — hồi sinh tại điểm đăng nhập. Đừng giả định "chết là ra khỏi map 960".
3. 🔴 Điều kiện giờ chỉ chặn cận dưới 12:00, **không chặn cận trên** — vào lúc 23:5x sẽ bị đá ngay khi sang 0h.
4. 🟡 Chỉ **đài 1** mới được loa toàn server khi thắng chuỗi 10 trận; đủ 100 trận thì bị ép rời đài.

---

## 5. BANG CHIẾN (VÕ LÂM ĐỆ NHẤT BANG)

### 5.1 Lịch

- **Mùa 9: 29/10 → 05/11/2026**, chỉ đánh các ngày-trong-tháng **29, 30, 31, 1, 2, 3, 5**.
- **Giờ: 20:00 mở, 21:30 đóng** (`TW_GIO_KHAICHIEN` / `TW_GIO_KETTHUC`).
- 🔴 **Cửa sổ vào trận THẬT là 20:30 → 21:30**: từ 20:00 NPC đã cho sang map báo danh, nhưng trap chỉ mở sau 30 phút chuẩn bị. Đạp trap trước 20:30 chỉ nhận *"Phe ta hiện đang tập họp chuẩn bị vào đấu trường!"* rồi bị đẩy lùi.
- 🔴 **Text NPC ghi giờ SAI** ("08:00~08:29 / 08:30~09:30") — đừng đọc giờ từ thoại.

### 5.2 Luồng vào

```
Map 53 Ba Lăng Huyện, ô (1628,3173) — NPC tpl 53 "Võ Lâm Truyền Nhân"
  └► (pha 2) lựa chọn 1: "Ta muốn tham gia thi đấu"          [head.lua:191]
      └► NewWorld tới MAP BÁO DANH, spawn 1 trong 3 ô:
         (1582,3174) | (1588,3160) | (1604,3147)
  └► đi tới ô TRAP (36 ô, vùng 1614-1625 / 3173-3184)
      └► NewWorld tới CHIẾN TRƯỜNG 605 | 606 | 607
```

| Chiến trường | map báo danh phe 1 | phe 2 |
|---|---|---|
| 605 | 608 | 609 |
| 606 | 610 | 611 |
| 607 | 612 | 613 |

**Cách auto biết đang ở pha nào — đọc cây thoại NPC** (không có biến nào client đọc được):

| Pha | Lựa chọn số 1 của NPC |
|---|---|
| 0 (đóng) | "Hoạt động 'Võ Lâm Đệ Nhất Bang'" — menu chỉ còn **2 dòng** |
| 1 (báo danh liên minh) | "Chọn thành thị muốn đại diện…" *(chỉ hiện với bang chủ đủ điều kiện)* hoặc "Đồng minh gia nhập liên minh bang hội" |
| **2 (thi đấu)** | **"Ta muốn tham gia thi đấu"** ← auto nhắm pha này |
| 3 (lĩnh thưởng) | "Nhận danh hiệu Võ Lâm Đệ Nhất Bang" |

🔴 Ở pha 1 chỉ số lựa chọn **dịch 1 đơn vị** tuỳ điều kiện ⇒ **khớp theo văn bản, dùng chỉ số làm dự phòng**.

### 5.3 Điều kiện chặn

Khi bấm "Ta muốn tham gia thi đấu" (`tongwar_signup.lua`): có bang · bang thuộc **liên minh** · **cấp ≥ 90** · **không mang mặt nạ cấm** (genre 0 / detail 11, 6 loại) · đúng giờ.

Khi đạp trap (`tongwar_trap.lua`): trận phải đang ở trạng thái đánh · có liên minh · **đúng map báo danh của phe mình** · cấp ≥ 90 · **chưa chết đủ trần mạng** · phe chưa đủ **150 người**.

🔴 **Hai nhánh từ chối HOÀN TOÀN IM LẶNG**: không có liên minh, và lỗi camp/thiếu map. Auto chỉ nhận biết bằng **việc bị dịch về ô (1619, 3175)**.

### 5.4 Biến trạng thái — task **có** đồng bộ xuống client

| Task | Ý nghĩa |
|---|---|
| **2370** | số lần đã chết trận này |
| **2376** | trần mạng trận này (khởi tạo 10; **+1 mỗi 5 mạng giết được**) |
| **2369 / 2378** | điểm trận này / tổng điểm cả mùa |
| **2371 / 2374 / 2375** | số người đã giết / chuỗi giết / chuỗi cao nhất |
| **2373** | quân hàm hiện tại 1..5 |
| **2377** | khoá định danh trận — khác 0 nghĩa là đã báo danh trận nào đó |

- **Đang trong trận?** → map hiện tại ∈ {605, 606, 607}
- **Bị loại chưa?** → `task 2370 >= task 2376` (bị đẩy về map báo danh trong ≤20 giây)
- **Không có trần lượt/ngày.**

### 5.5 Trong trận

- Mục tiêu: **PK người phe địch**, không có mục tiêu NPC nào.
- Điểm: giết 1 địch = `75 × hệ số chênh quân hàm` (0.2→2.0); mỗi 3 mạng liên trảm +150; mỗi 5 mạng giết được thì **trần mạng +1**.
- Quân hàm lên theo mốc điểm `0 / 1000 / 3000 / 6000 / 10000`, kèm buff kỹ năng 661.
- Phe nào còn **dưới 5 người** (sau mốc chuẩn bị) → **xử thua ngay**.
- 🔴 Trong trận: **bị ép rời tổ đội**, **cấm phù hồi thành**, ép bật cờ PK, cấm đổi chế độ PK.
- Hậu doanh chỉ đứng được **120 giây** rồi bị đẩy ra tiền tuyến.
- **Không có NPC thoát.** Chỉ ra được khi: chết đủ trần mạng, hết giờ trận, hoặc thoát game.
- 🔴 Toạ độ hậu doanh **random mỗi trận** (2 khu vực, mỗi khu 4-5 ô) ⇒ **không hardcode**.

### 5.6 Bẫy riêng

1. 🔴🔴 **Đạp trap lần 2 khi đã ở trong trận = MẤT 1 MẠNG** (`tongwar_trap.lua:85-87`). Auto tuyệt đối không đạp lại trap.
2. 🔴 **Chỉ cần chạm ô trap là MẤT TỔ ĐỘI** — lệnh rời tổ đội chạy vô điều kiện ở dòng đầu, kể cả khi sau đó bị từ chối.
3. 🔴 Ô bị đẩy về khi từ chối là (1619,3175) — **không** nằm trong vùng trap nên không lặp vô hạn.
4. 🟡 Thưởng cuối mùa **chỉ bang chủ của bang chiếm thành** mới nhận được (Quả Đại Hoàng Kim 4864, Hoàng Chân Đan 2273, Cuồng Lan 4491, Hàn nguyên 4857, Phi Phong 3476).

---

## 6. TỶ VÕ ĐÀI

### 6.1 Không có lịch — mở cả ngày

Không cần giờ, không cần bang, **miễn phí**. Chỉ cần **2 người lập tổ đội** rồi báo danh; người thứ 3 trở đi vào bằng **số thứ tự**.

### 6.2 NPC "Công Bình Tử" — 12 vị trí (template 309)

| Map | Toạ độ ô |
|---|---|
| **80 Dương Châu** | (1659,3020) · (1852,3049) · (1627,3208) · (1709,3251) |
| **78 Tương Dương** | (1464,3183) · (1705,3235) · (1474,3272) · (1577,3376) |
| **11 Thành Đô** | (3071,5002) · (3225,5042) · (3040,5096) · (3166,5195) |

Cả 12 NPC **cùng một cây thoại** (2 file thành phụ chỉ Include file gốc).
🟡 Tên hiển thị "Công Bình Tử" do lệnh tạo đặt; tên gốc của template là "Lãng nhân 3" ⇒ **auto tìm NPC theo tên hiển thị**.

### 6.3 Cây thoại (chỉ số gửi lên server là **0-based**)

**Khi chưa ai báo danh:**

| Chỉ số | Dòng |
|---|---|
| **0** | `Được thôi!` → mở menu chọn số người |
| 1 | `Quy tắc thi đấu ra sao?` |
| 2 | `Để ta suy nghĩ lại!` |

**Menu chọn số người** — rất tiện: **chỉ số gửi lên = đúng số người mỗi đội**
(0 = "Thôi khỏi!", 1 = `1 vs 1`, 2 = `2 vs 2`, … 8 = `8 vs 8`).

**Khi đã có trận đang chờ:**

| Chỉ số | Dòng |
|---|---|
| **0** | `Ta là đội trưởng, muốn biết số thứ tự vào đấu trường.` |
| **1** | `Ta là tuyển thủ, muốn vào đấu trường.` |
| 2 | `Ta muốn xem trận đấu này.` (khán giả — **không bị chặn cấp**) |
| 3 | `Ta không hứng thú với trận đấu này.` |

**Người thứ 3 trở đi** chọn 1 → chọn **0** (`Được thôi! Để ta nhập vào`) → hiện **hộp nhập số**
(`Nhập số thứ tự vào đấu trường:`, khoảng **0…10000**) → gõ đúng số của đội mình.

### 6.4 Luồng báo danh (auto phải dựng đúng thứ tự)

1. Hai đội trưởng A và B **lập tổ đội đúng 2 người với nhau** (chỉ A + B, người khác **không được** ở trong tổ).
2. A nói chuyện NPC → chỉ số 0 → chọn `N vs N`.
3. Server mở trận, sinh **2 số thứ tự ngẫu nhiên 1…9999**, và **biến A và B thành đội trưởng của 2 phe**. Mỗi người nhận thoại báo số của mình.
4. Người thứ 3+ tới NPC → nhập số của phe muốn vào.
5. Trong lúc chờ, cứ 20 giây server nhắc lại số cho đội trưởng nào chưa vào.

### 6.5 Điều kiện chặn

| Điều kiện | Giá trị |
|---|---|
| Tổ đội | **đúng 2 người** (3 người là trượt) |
| Người bấm | phải là **đội trưởng** |
| Cấp | **cả 2 người trong tổ** đều ≥ 90 |
| Số người mỗi đội | 1…8 |
| Trạng thái | chưa có trận nào đang mở |

### 6.6 Trong trận

- Vào đài: đội Giáp **(209, 1620, 3202)**, đội Ất **(209, 1612, 3187)**, khán giả **(209, 1598, 3216)** *(khán giả được tàng hình)*.
- Chờ **2 phút** rồi đánh; tổng trận **12 phút**. Hết giờ: phe **đông người hơn** thắng, bằng nhau thì hoà.
- Mục tiêu: **giết sạch phe kia**. Không có điểm số.
- Bị áp: cấm bày bán/giao dịch, **cấm lập tổ đội**, ép bật cờ PK, **cấm phù hồi thành**.
- Thoát sớm: NPC **"Trương Tam"** ô **(1599,3202)** hoặc **"Lý Tư"** ô **(1608,3211)** → chỉ số **0** (`Phải, ta ra đây một lát!`).

### 6.7 Biến trạng thái

| Biến | Ý nghĩa |
|---|---|
| task **2340 / 2341 / 2342** | map / x / y nơi bấm NPC (điểm trả về) |
| **TaskTemp 200** | `1` = đang trong đấu trường, `0` = đã ra |

**Không có** hệ điểm, **không có** giới hạn lượt/ngày/tuần. Thưởng duy nhất là **cập nhật nhiệm vụ nhánh** cho phe thắng.

### 6.8 Bẫy riêng

1. 🔴 **Tổ đội phải ĐÚNG 2** — 3 người là trượt. Sau khi vào đấu trường thì **không lập/đổi tổ đội được nữa**, phải sắp xếp trước.
2. 🔴 **Chỉ bị loại khi bị NGƯỜI CHƠI phe địch giết.** Chết vì lý do khác không bị loại ⇒ auto đếm "còn mấy người" theo số lần chết sẽ sai.
3. 🔴 Khi trận vừa kết thúc, NPC trả về thông báo **sai nghĩa**: *"Song phương tham gia thi đấu không đem đủ ngân lượng."* — **không phải** thiếu tiền, mà là "chờ vài giây cho trận cũ dọn xong".
4. 🔴 **Task 2340-2342 bị ghi đè MỖI LẦN bấm NPC**, kể cả chỉ xem ⇒ nếu auto bấm NPC ở nơi khác rồi mới vào trận, người chơi sẽ bị trả về **nơi đó**.
5. 🔴 Kẹt trạng thái (server sập giữa trận): TaskTemp 200 còn 1 ⇒ mang cờ PK, cấm bày bán, cấm phù. Gỡ bằng nút **"Gỡ kẹt trận treo của TÔI"** trong Lệnh Bài Admin.
6. 🟡 Người thứ 3 vẫn nhập số vào được **sau khi trận đã bắt đầu**, miễn phe chưa đủ quân.

---

## 7. BANG HỘI THÀNH BẢO

> ⚠️ **Chưa thi công auto được** cho tới khi có dữ liệu map 984 phía client.

### 7.1 Lịch và điều kiện

- **Chủ nhật, 17:00 → 19:00** (map gắn với **Lâm An 176**).
- Ba phòng mở dần: **Nhân Tự 17:00** → **Địa Tự 18:00** → **Thiên Tự 18:30**, tất cả đóng **19:00**.
- Điều kiện vào: **có bang** · **Lâm An phải có bang chiếm** · **cấp ≥ 90** · **đã vào bang > 1 ngày (1440 phút)**.

### 7.2 Điểm vào

NPC **"Người chỉ dẫn bang hội thành bảo"** (template 108), map **176 Lâm An**, ô **(1663, 3262)**.

| Chỉ số | Dòng |
|---|---|
| **0** | `Ta muốn vào bang hội thành bảo thành này` |
| **1** | `Ta muốn nhận Thần Mộc Lệnh` (đổi điểm lấy item) |
| **2** | `Ta muốn nhận điểm Thần Mộc` |
| 3 | `Ta muốn xem số điểm thần mộc của mình` |
| 4 | `Kết thúc đối thoại` |

**Điểm rơi trong map 984:**
- Bang **đang chiếm thành**: **(1311, 3515)** — ngay cạnh cây Hoàng Kim.
- Bang **khác**: **ngẫu nhiên 1 trong 4 cổng** — Đông (1638,3851) · Bắc (1647,3174) · Tây (917,3106) · Nam (983,3884).
  🔴 ⇒ auto **phải đọc lại toạ độ sau khi vào map** rồi mới chọn tuyến, không hardcode một hướng.

### 7.3 Mục tiêu và tuyến đi

Chặt Thần Mộc theo thứ tự, mỗi lần mở một phòng sâu hơn:

```
Thanh Đồng (12 cây, 3 mỗi hướng)  → chặt hết 1 hướng thì mở kênh sang Địa Tự hướng đó
Bạch Ngân  (4 cây, 1 mỗi hướng)   → chặt hết CẢ 4 mới mở Thiên Tự
Hoàng Kim  (1 cây, ở giữa)
```

Máu cây: **96 triệu / 288 triệu / 960 triệu**.

Điểm nhận được:

| Cây | Cá nhân | Bang đi chặt | Bang giữ thành (mỗi cây còn sống lúc 19:00) |
|---|---|---|---|
| Thanh Đồng | 20 | 15 | 10 |
| Bạch Ngân | 40 | 30 | 20 |
| Hoàng Kim | 200 | 150 | 100 |

🔴 **Bang chủ thành KHÔNG dùng được bẫy dịch chuyển** — phải đi bộ. Bang khác thì đi bằng bẫy.

### 7.4 Nhận thưởng

**Bước 1 — nhận điểm** (chỉ số 2 ở NPC): khung **19:00 → 24:00**, phải đã ở trong map đủ **45 phút**, **1 lần/ngày**.
**Bước 2 — đổi lệnh bài** (chỉ số 1): hộp nhập số **1…100**.

| Item | Giá (điểm Thần Mộc) | Dùng được gì | Trần/tuần |
|---|---|---|---|
| Thanh Đồng Thần Mộc Lệnh (6,1,**3205**) | 10 | +10 triệu exp | 5 |
| Bạch Ngân Thần Mộc Lệnh (6,1,**3206**) | 20 | +20 điểm Chân Nguyên | 5 |
| Hoàng Kim Thần Mộc Lệnh (6,1,**3207**) | 120 | +60 triệu exp **và** +120 Chân Nguyên | 5 |

### 7.5 Biến trạng thái

| Task | Ý nghĩa |
|---|---|
| **3399** | điểm Thần Mộc hiện có |
| **4056** | số phút đã ở trong Thành Bảo hôm nay (cần ≥ 45) |
| **4057** | ngày lần cuối reset điểm |
| **4058 / 4059 / 4060** | số lần đã dùng từng loại lệnh bài trong tuần |
| **4061** | đã nhận điểm hôm nay chưa |

### 7.6 Bẫy riêng

1. 🔴🔴 **Điểm Thần Mộc (task 3399) bị XOÁ SẠCH khi sang ngày mới.** Auto **phải đổi lệnh bài ngay trong ngày**, để qua 0h là mất hết.
2. 🔴 **Điểm chỉ được cộng khi đã vào bang > 1440 phút** (dấu "lớn hơn"), trong khi cửa vào dùng "lớn hơn hoặc bằng" ⇒ người vừa tròn 1 ngày **vào được nhưng chặt cây không ra điểm**. Auto nên chờ dư vài phút.
3. 🔴 **Lệnh bài KHÔNG xếp chồng** — đổi 100 cái là chiếm **100 ô túi**. Kiểm tra chỗ trống trước.
4. 🔴 **Bẫy từ chối im lặng**: khi chưa đủ điều kiện đi tiếp, bẫy **không làm gì cả** — auto phải dò **vị trí có đổi không**, đừng chờ thoại.
5. 🔴 **Hai lỗi cấp và thời-gian-vào-bang dùng CHUNG một câu thông báo** ⇒ auto không phân biệt được nguyên nhân qua thoại, phải tự kiểm.
6. 🟡 **Thoại NPC ghi cứng giờ 17:00/18:00/18:30/19:00** trong khi giờ thật lấy từ file config ⇒ đừng đọc giờ từ thoại.
7. 🟡 **Không có mục mua Bùa triệu Thủ Vệ trong menu NPC** — hàm có sẵn nhưng không gắn vào menu. **Đây là nguyên trạng của bản Linux gốc**, không phải thiếu sót khi port (đã đối chiếu). Hiện chỉ Lệnh Bài Admin phát được item 3204.
8. 🟡 **Từ Hàng Thủ Vệ (loại hồi máu cho cây) bị tắt trong menu triệu hồi** — cũng **đã bị comment sẵn trong bản Linux gốc**. Vì vậy chỉ triệu được 3 loại, và chỉ số 2 là **Hoả Tường**, không phải Từ Hàng.
9. ✅ Lỗi `OB_LoadShareData` từng ghi trong nhật ký đêm 23/08 **đã hết** — đó là do lúc đó server còn chạy DLL cũ chưa có hàm này; sau khi thay DLL (23:32 ngày 23/08) không còn lỗi nào.

---

## 8. VIỆC CÒN LẠI / CHƯA XÁC ĐỊNH

| Việc | Ghi chú |
|---|---|
| **Map 984 thiếu dữ liệu client** | Chặn hoàn toàn phần auto Thành Bảo. Cần chủ game cấp pak. |
| **Task Bách Nhân có sync xuống client không** | 2323/2324/2325/2709 dùng lệnh đặt task **không kèm đồng bộ** ⇒ phải thử thực tế trước khi auto dựa vào. |
| Trạng thái "đang trên đài" (Bách Nhân) | Không có biến nào — phải suy từ vị trí + trạng thái chiến đấu + dòng loa. |
| Pha Bang Chiến | Không có biến client đọc được — phải suy từ **cây thoại NPC**. |

---

## 9. TÓM TẮT CHO NGƯỜI THI CÔNG

1. Mở cây **`_WAuto`** đúng (mục 1.1), thêm ô cấu hình **ở cuối** `struct autoData`.
2. Làm **Bách Nhân trước**: luồng ngắn nhất (2 lần chọn thoại), thưởng exp thẳng, đã chứng minh chạy thật.
3. Đừng nhét cứng giờ/cấp — chủ game đổi được qua `cauhinh_hoatdong.lua`.
4. Với mỗi hoạt động, **luôn có ít nhất một nhánh từ chối im lặng** — auto phải xác nhận thành công bằng **đổi map / đổi vị trí**, không chờ thoại.
5. Test tay bằng **Lệnh Bài Admin** (mục *Hoạt động 23-24.08*) trước khi để auto chạy.
