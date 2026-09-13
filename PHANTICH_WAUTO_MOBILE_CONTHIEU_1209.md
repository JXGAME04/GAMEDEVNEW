# WAuto PC ↔ WAuto MOBILE — SOI TOÀN BỘ, DANH SÁCH CÒN THIẾU (12/09)

> Nguồn đối chiếu: `WAutoUI/WAuto.rc` (501 điều khiển, UTF-16), `WAutoUI/WAuto.cpp` (7.674 dòng),
> `WAutoUI/Resource.h`, `Sources/Core/Src/ipc_shared.h` — so với `Sources/S3Client/Ui/UiCase/UiWAutoBang.h`
> (bảng sinh), `UiWAutoTrang.cpp` (lớp chạy), `UiWAuto.cpp` (khung), `Platform/JxWAutoNoiBo.cpp` (đường ống),
> 15 tệp `android/du_lieu_ghi_de/ui/ui3/uiwauto_tab*.ini`.
> Trạng thái mã: nhánh `mobile-0809` tới `bc6158e4`. Bản WAuto mobile đang ở **B2** (+ B2 f/i, PHONGBANG) — chưa có B3.
> **Đây là bản phân tích, chưa sửa một dòng mã nào.**

---

## 0. SỐ ĐẾM GỐC (đối chiếu bằng máy, không ước lượng)

| Thứ | Số |
|---|---|
| Điều khiển trong `WAuto.rc` | 501 (479 có đủ toạ độ + mã `IDC_`) |
| 15 thẻ bản mobile dựng — điều khiển **tương tác** (tick / ô nhập / hộp chọn / nút / danh sách) | **283** |
| Trong đó bản mobile **có dựng** | **279** |
| → **thiếu hẳn, không vẽ ra** | **4** (§1) |
| Trong 279 cái đã vẽ: **nối được vào `autoData`, bấm là ăn** | **240** |
| → **vẽ ra nhưng CHẾT** (`Enable(0)` hoặc `nOff = -1`) | **39** (§2) |
| Thẻ **Ác chính** (6 điều khiển) + **Đ.nhập** (8) — cố ý bỏ | 14 |
| Vùng dùng chung trên/dưới cửa sổ PC (bảng nhân vật, [A], [Bật hết]…) | 12 — bản mobile có 0 (§5) |
| Hộp thoại con PC chưa port | 2 (`IDD_NOPICK_DIALOG`, `IDD_SETSERIES_DIALOG`) |
| Ghi chú rê chuột (`s_aTips`) | PC **283** — mobile **0** |
| Trang hướng dẫn theo thẻ (`s_aNote`) | PC **17 thẻ + 2 chung** — mobile **0** |

> 283 − 279 = 4 thiếu hẳn; 279 − 240 = 39 chết. Hai con số này là xương sống của bản này.

---

## 1. LỖI THẬT — 4 Ô TÍCH BIẾN MẤT VÌ MỘT DẤU PHẨY (sửa trước hết)

`android/sinh_bang_wauto.py:61` tách tham số của dòng `.rc` bằng một biểu thức có nhánh `[^,]+`. Nhánh đó
**ăn trước** cả dấu nháy mở khi phía trước có khoảng trắng, nên **nhãn nào có dấu phẩy thì cả dòng điều khiển
bị vứt**. Hậu quả: 4 ô tích và 1 nhãn **không hề có trong bản mobile** — không phải mờ, mà là không tồn tại.
Cả 4 đều là **công tắc chính**, còn ô số đi kèm thì vẫn được vẽ → người chơi thấy ô nhập mồ côi.

| Thẻ | Mất | Trường `autoData` | Hậu quả đang thấy trên máy |
|---|---|---|---|
| **Cơ bản** | `IDC_CHECKBOX_0_MIS` "Mọi chiêu chung 1 hiệu ứng, dòng:" | `bWAMissle` | ô nhập `nWAMissleIndex` đứng trơ, **không nhãn, không công tắc**. Đây đúng là tính năng *nhẹ máy* cần nhất cho điện thoại |
| **Nhặt đồ** | `IDC_CHECKBOX_4_RG` "Giữ nhẫn, dây chuyền, ngọc bội cấp >" | `bSaveRing` | ô `nSRLevel` (cấp giữ lại) vẫn còn nhưng không bật được → bán rác ăn luôn nhẫn/dây |
| **Tổ đội** | `IDC_CHECKBOX_6_TL` "Mời nhóm, vào nhóm theo danh sách" | `bJoinPtByList` | **công tắc chính của cả thẻ** |
| **H.động** | `IDC_CHECKBOX_12_TS` "Bật auto Tín Sứ (đưa thư, cần cấp 90)" | `bHDTinSu` | nhóm "Tín Sứ" có đủ *Đi lúc / phút / Lượt-ngày* nhưng **không có nút bật** |
| **H.động** | `IDC_STRING_12_CAY` "Cày (phút, 0 = hết lượt exp):" | (nhãn) | ô `nHDBNCay` trong nhóm Bách Nhân **không có nhãn** |

**Cách sửa:** thay bộ tách bằng một vòng quét ký tự tôn trọng chuỗi trong nháy kép (gặp `"` thì nhảy tới `"`
đóng rồi mới cắt theo dấu phẩy), chạy lại `sinh_bang_wauto.py` → `sinh_bocuc_wauto.py` → chép ini.
Không đụng C++. Kiểm chứng: sau khi sửa, `parse_rc()` phải ra **479** dòng thay vì **464**.

---

## 2. VẼ RA NHƯNG BẤM KHÔNG ĐƯỢC — 39 ĐIỀU KHIỂN

Ba nguyên nhân trong mã:

* `UiWAutoTrang.cpp:276` — **mọi nút đều `Enable(0)`**; `WndProc` `:514` trả về ngay, "(B2) chua co hanh dong".
* `UiWAutoTrang.cpp:249/258/267` — tick / ô nhập / hộp chọn bị tắt khi `nOff < 0` (bộ sinh không dò ra trường)
  hoặc `nNguon == WA_NGUON_KHAC` (danh sách phải lấy từ trong game lúc chạy).
* `WA_MUC_DSACH` có khai trong `UiWAutoBang.h` nhưng **không xuất hiện một lần nào** trong `UiWAutoTrang.cpp`
  → 4 danh sách **không được tạo ra**, chỉ có chữ xám "(danh sách - bước B18)".

### 2.1. Theo thẻ

| Thẻ | Chết | Chi tiết |
|---|---|---|
| **Cơ bản** | 1 | `0_HG` *Ẩn cửa sổ game* — **nên bỏ**, xem §5 |
| **Di chuyển** | **11** | hộp chọn *Theo sau* (tên người chơi quanh mình); **9 nút**: *Lấy* (bản đồ), *Lấy* (điểm cố định), *Đọc* (thêm toạ độ đang đứng), *Lên*, *Xuống*, *Xóa*, *Xóa hết*, *Đọc tệp*, *Ghi tệp*; danh sách **toạ độ đi tuần** (`sMoveCoord[24]`) |
| **Nhặt đồ** | **8** | hộp chọn *Dòng thuộc tính* + ô *Giá trị tối thiểu*; **5 nút**: *Cài đặt tên không nhặt*, *Xóa*, *Xóa hết*, *Đọc tệp*, *Ghi tệp*; danh sách **luật lọc** (`nFtMagic[40][2]`) |
| **Tổ đội** | **9** | hộp chọn *tên nhân vật quanh mình*; **7 nút**: *Thêm*, *Lên*, *Xuống*, *Xóa*, *Xóa hết*, *Đọc tệp*, *Ghi tệp*; danh sách **mời / nhận nhóm** (`szIJPtName[24][32]`) |
| **PK** | 3 | tick *Khoảng cách* (`7_PYD`), ô *Phím kích hoạt PK* (`7_K`), nút *Thứ tự* (mở bảng ngũ hành) |
| **Tống Kim** | 1 | hộp chọn **Rương cửa** (`nTKRuongHuong`) |
| **Liên đấu** | **5** | ô *Tên chiến đội* (`szLDTen`), hộp chọn *người quanh mình*, nút *Thêm*, nút *Xóa*, danh sách **bạn diễn** (`szLDPtName[8][32]`) |
| **Sát thủ** | 1 | hộp chọn **Boss cố định** (`nSTBoss`) |
| | **39** | 24 nút · 6 hộp chọn · 4 danh sách · 3 ô nhập · 2 tick |

### 2.2. Cái nào dễ, cái nào khó (đã tra tận nơi lấy dữ liệu)

**Nhóm A — dữ liệu là bảng TĨNH nằm sẵn trong `WAuto.cpp`, chỉ việc chép sang bảng sinh. Rẻ nhất:**

| Điều khiển | Nguồn thật | Ghi chú |
|---|---|---|
| **Rương cửa** (Tống Kim) | `s_aTKRHuong[5]` + `s_aTKRCo[7][5]`, `WAuto.cpp:131-183` | danh sách **đổi theo thành chọn ở "Hết trận về"**; giá trị lưu là *mã hướng* 0..4, mục cuối luôn là *"Gần nhất (tự chọn)"* = **5**. Không phải chỉ số dòng |
| **Boss cố định** (Sát thủ) | 20 tên cứng trong `WM_INITDIALOG`, `WAuto.cpp:5238+` | `nSTBoss = 141 + chỉ_số_dòng` (`WAuto.cpp:1328`). Bộ sinh trượt vì công thức có `+141` |
| **Dòng thuộc tính** (Nhặt đồ) | `g_MagicTable`, **40 dòng** | chọn dòng + gõ *Giá trị tối thiểu* rồi ghi vào `nFtMagic[i][0..1]` — đi kèm danh sách ở nhóm D |

**Nhóm B — phải xin dữ liệu từ trong game lúc chạy. Đường ống ĐÃ CÓ SẴN, bản mobile chưa dùng:**

3 hộp chọn *"tên người chơi quanh mình"* (Di chuyển *Theo sau*, Tổ đội *Thêm tên*, Liên đấu *bạn diễn*).
Trên PC: bấm vào hộp → gửi `PRT_GETTEAMAROUND` (`bHide` = 1 / 0 / 2) → game trả `PRG_TEAMNAMELIST` → đổ vào hộp.
Bên game **đã xử lý sẵn** `PRT_GETTEAMAROUND` (`S3Client.cpp:1450`) và `PRT_GETITEMNAME` (`:1434`).
Bên mobile `JxWAutoNoiBo.cpp` **chỉ gửi** `PRT_GAMELOOP` + `PRT_HIENTHI` + `PRT_TICKSTART`, và **không ai đọc**
gói `PRG_*` game trả về. Tức là phải thêm một **chiều về** trong `JxWAutoNoiBo` (đọc `g_pRepState`), rồi 3 hộp
chọn này chạy được ngay — cùng một đường đó cũng mở khoá nút *Cài đặt tên không nhặt* (§4).

**Nhóm C — chỉ cần đọc trạng thái nhân vật, không cần IPC:**

| Nút | Việc |
|---|---|
| Di chuyển · *Lấy* (bản đồ) | `nMoveMapId` + `szMoveMap` = bản đồ đang đứng |
| Di chuyển · *Lấy* (điểm) | `nPointX` / `nPointY` = toạ độ đang đứng |
| Di chuyển · *Đọc* | thêm toạ độ đang đứng vào `sMoveCoord[nCoordCount++]` (tối đa 24) |
| Tổ đội / Liên đấu · *Thêm* | thêm tên đang chọn ở hộp vào mảng tên (24 / 8), có chống trùng |
| *Lên / Xuống / Xóa / Xóa hết* (3 thẻ) | thao tác mảng thuần, y hệt PC |

**Nhóm D — 4 danh sách (`WA_MUC_DSACH`) — cần một widget mới.** Đây là việc nặng nhất còn lại:
mỗi danh sách cần cuộn được bằng ngón tay + chọn dòng. Dùng lại `KWndMessageListBox` (đã quen tay ở
KUiVatPham 14/09) hoặc viết `KWndDsachWA` nhỏ.

---

## 3. VẼ RA, BẤM ĐƯỢC, NHƯNG CHẠY KHÔNG ĐÚNG NHƯ PC

### 3.1. Hộp chọn chiêu KHÔNG lọc theo loại — 19 hộp đều ra cùng một danh sách

`JxCore_WAutoDanhSachChieu` (`CoreShell.cpp:25129`) gọi thẳng `KSkillList::GetAllSkillByType`
(`KSkillList.cpp:993`) → **một danh sách phẳng**: dòng 0 = đòn đánh thường theo vũ khí, còn lại là mọi chiêu
CHỦ ĐỘNG đã học (`Missles` / `Melee` / `InitiativeNpcState`, bỏ chiêu gốc).

WAuto.exe **không** dùng thẳng danh sách đó. Nó chẻ ra **7 danh sách con** ngay khi nhận `PRG_MAINSYNC`
(`WAuto.cpp:3057-3097`):

| Danh sách con | Điều kiện lọc | Dùng cho |
|---|---|---|
| `SkillAAr` | `bAura` | **Vòng sáng #1 / #2** |
| `SkillSAr` | không phải vòng sáng, và (`bState` hoặc `nStyle == 2`) và `bAlly` | **Bật hỗ trợ #1 / #2 / #3** (buff phe ta) |
| `SkillSEAr` | `bState` và **không** `bAlly` | **Chiêu Sát thủ #1 / #2 / #3** (thẻ PK) |
| `SkillBAr`, `SkillPAr` | `nStyle <= 1` và **không** `bAlly` | **Đánh boss**, **hồi Sinh lực %**, **hồi Nội lực %**, **Đổi tay trái** |
| `SkillKAr` | mọi chiêu **trừ** vòng sáng | **6 khe Chiêu kết hợp**, **Tiền chiêu** |
| `SkillLAr` / `SkillRAr` | theo `bLR` | trái / phải |

Trên mobile cả **19** hộp chọn chiêu đều mở **cùng một danh sách đầy đủ** → chọn được chiêu sai loại
(ví dụ nhét chiêu đánh vào ô *Vòng sáng*, nhét chiêu địch vào ô *Bật hỗ trợ*). Máy auto sẽ nhận một
`nSkillId` không hợp lệ cho khe đó. **Đây là lỗi âm thầm, chủ sẽ tưởng "bật rồi mà không chạy".**

Mọi trường cần để lọc (`bAura`, `bState`, `bAlly`, `nStyle`, `bLR`) **đã có sẵn** trong `IPCSkillInfo` mà
`GetAllSkillByType` trả về — chỉ cần lọc lại ở `UiWAutoTrang::MoMenuChon` và thêm một cột "loại danh sách"
vào bảng sinh. Việc nhẹ, giá trị cao.

### 3.2. Menu chọn KHÔNG cuộn — danh sách dài rơi ra ngoài màn

`KPopupMenu` không có cuộn: `m_nMenuHeight = nItemHeight * nNumItem` (`PopupMenu.cpp:171`), chiều cao dòng
bị nống lên cho vừa ngón (`:155-160`) ≈ **26 px**. Màn 604 px ⇒ **quá ~23 dòng là mất phần dưới**.
Đang có 2 chỗ vượt:

* **Hậu cần · Đi Xa Phu → bản đồ**: **31 dòng** → khoảng 806 px.
* **Mọi hộp chọn chiêu**: `defSKILLNUMGET` = **72**; `UiWAutoTrang.cpp:400` mới chỉ chặn cứng ở **40** dòng
  (`WA_MENU_TOI_DA`) — vẫn khoảng 1.040 px. Nhân vật từ ~20 chiêu trở lên là chạm không tới dòng cuối.

Cần: menu cuộn được, hoặc chia trang, hoặc menu 2 cột.

### 3.3. Số liệu nhân vật không bao giờ cập nhật

| Thẻ | Ô | PC lấy ở đâu |
|---|---|---|
| **Cơ bản** | **7 ô** — *Sinh lực*, *Nội lực*, *Thể lực*, *Bản đồ (tên)*, *Bản đồ (toạ độ)*, *Đẳng cấp*, *Kinh nghiệm* | `ProcIpcCommand` khi nhận `PRG_MAINSYNC` |
| **Di chuyển** | **3 ô** — *Bản đồ áp dụng*, *Điểm X*, *Điểm Y* | `UpdateUI` + các nút *Lấy* |

Trên mobile cả 10 ô đang là chữ `-` cứng trong ini, **không có mã nào ghi vào**. Trong game thì mọi số này
lấy thẳng từ `Player[]` được, không cần qua IPC.

### 3.4. Trạng thái Bật/Tắt không nhớ qua lần mở app

`JxWAutoNoiBo.cpp:275` — `JxWAuto_Bat` chỉ gán biến tĩnh `s_nBat`; giá trị đầu đọc từ
`config.ini [WAuto] Bat` (`:39`). `autoData` **không có** trường nào cho việc này (và **không được thêm vào
giữa struct** — luật §0.2 của lộ trình). Cách sạch: ghi riêng một khoá trong `config.ini` hoặc một tệp nhỏ
cạnh `APdata`, không đụng `autoData`.

### 3.5. Hai ô "Ưu tiên mục tiêu" là một cặp radio, mobile dựng thành 2 tick rời

PC: `nPriority` = 0 → *Khoảng cách* (gần nhất); = 1 → *Thứ tự ngũ hành* (`WAuto.cpp:893`, `:1963-1972`).
Bấm ô này thì ô kia tự tắt. Mobile: `7_PYS` nối vào `nPriority`, `7_PYD` **chết xám** → vẫn đặt được giá trị
nhưng nhìn như hỏng. Nên gộp thành **một hộp chọn 2 dòng**.

### 3.6. Chưa có ghi chú / hướng dẫn

PC có **283 dòng ghi chú rê chuột** và **19 trang hướng dẫn** ([H] chung, [Trợ giúp] theo thẻ, [?] vùng trên).
Bản mobile **không có gì**. Với 279 ô trên 15 thẻ, đây chính là lý do "nhiều tính năng không biết để làm gì".
Chữ đã viết sẵn trong `WAuto.cpp:3767-4077` — chuyển TCVN3 rồi hiện bằng một nút [?] mỗi nhóm là đủ.

### 3.7. Còn treo từ B1/B2 (chưa sửa)

* Cụm nút kỹ năng góc phải dưới + icon NPC vẫn **vẽ đè lên khung Auto** (bẫy 3 của B1).
* Chạm vào **chữ** cạnh ô tick không bật/tắt — chỉ trúng đúng ô 24 px.
* Tiêu đề nhóm là do bộ sinh đặt tay (`TIEU_DE_TAY`, 30 mục) — chủ chưa rà.

---

## 4. CHƯA PORT — 2 HỘP THOẠI CON

| Hộp thoại | Trường | Việc |
|---|---|---|
| `IDD_NOPICK_DIALOG` — **Không nhặt theo tên** | `szNOPName[60][80]`, `nNOPCount` | mở bằng nút *Cài đặt tên không nhặt* (Nhặt đồ). PC xin tên vật phẩm trong túi bằng `PRT_GETITEMNAME` → `PRG_OPENNOPICK` (game **đã xử lý sẵn**), chọn từ hộp rồi thêm; có nút *Xóa*. Tối đa 60 tên |
| `IDD_SETSERIES_DIALOG` — **Thứ tự ngũ hành** | `nSerPy[6]` | mở bằng nút *Thứ tự* (PK). Danh sách 5 dòng Kim / Mộc / Thủy / Hỏa / Thổ + 2 nút *Lên* / *Xuống* |

---

## 5. NÊN BỎ HẲN Ở BẢN MOBILE

> Theo luật §0.2 của lộ trình: **không xoá một dòng mã nào**, không xoá trường nào trong `autoData`.
> "Bỏ" ở đây = **không dựng ô đó trong giao diện mobile** (thêm vào danh sách loại trừ của bộ sinh).

### 5.1. Bỏ vì một điện thoại chỉ một nhân vật

| Bỏ | Số ô | Vì sao — đã tra tận nơi |
|---|---|---|
| **Thẻ Ác chính** | 6 | Máy auto lấy vị trí ác chính từ `nACX` / `nACY` / `nACMap` / `uACTuoi` / `nACSong` / `uACMucTieu` (`CoreShell.cpp:10441-10470`). Những trường đó do **WAuto.exe bơm sang** sau khi nhận `PRG_VITRI` của **cửa sổ khác**. Một máy một nhân vật thì không có nguồn bơm ⇒ tính năng **không thể chạy**. `JxWAutoNoiBo.cpp:355-358` đã chủ động xoá trắng mấy trường này mỗi nhịp — đúng |
| **Thẻ Đ.nhập** | 8 | `CreateProcessA("game.exe")` + quản lý nhiều tài khoản. Android đã có tự đăng nhập riêng |
| **Cài đặt theo + [Đồng bộ]** (`0_STF`) | 2 | chép cấu hình **từ nhân vật khác đang mở** sang — không có nhân vật khác |
| **Bảng 5 cột danh sách nhân vật** (`IDC_PLAYERLIST`) | 1 | mỗi dòng là một cửa sổ game đang mở |
| **[A] quét lại cửa sổ** | 1 | `EnumWindows` tìm cửa sổ game trên Windows |
| **[Bật hết] / [Tắt hết]** | 2 | tick / bỏ tick auto cho **mọi cửa sổ** |
| **[Đồng bộ]** (hàng dưới) | 1 | chép cấu hình sang **mọi cửa sổ khác** |
| **[Ác chính] / [X] / hộp chọn ác chính / 3 ô hàng 3** | 5 | như thẻ Ác chính |
| **Ẩn cửa sổ game hiện tại** (`0_HG`) | 1 | gửi `PRT_HIDEGAME` để **ẩn cửa sổ Windows** cho nhẹ máy. Điện thoại không có cửa sổ ⇒ vô nghĩa. **Hiện đang vẽ ra và xám** — nên gỡ hẳn |

**Cộng: 27 ô nên gỡ khỏi giao diện mobile.**

### 5.2. Bỏ vì điện thoại không có thứ đó

| Bỏ | Số ô | Vì sao |
|---|---|---|
| **Đọc tệp / Ghi tệp** — Di chuyển (2), Nhặt đồ (2), Tổ đội (2) | 6 | PC mở hộp thoại chọn tệp Windows để lưu / nạp danh sách ra `.txt`. Điện thoại không có trình quản lý tệp cho người chơi. Muốn chép giữa hai máy thì chép thẳng `APdata\<id>.dat` |
| **Phím kích hoạt PK** (`7_K` → `uFKey`) | 1 | Ô này **bắt phím bàn phím** (`EditSubclassProc`, bảng `g_KeyTable` 49 phím). Điện thoại không có bàn phím vật lý. Nên đặt cứng một giá trị mặc định và không vẽ ô |

**Cộng: 7 ô nữa.**

> Gỡ 27 + 7 = **34 ô**. Trong đó 2 ô (`0_HG`, `7_K`) đang nằm trong danh sách 39 ô chết ⇒ còn
> **37 điều khiển chết thật sự phải làm**, và giao diện gọn hơn hẳn.

### 5.3. KHÔNG bỏ, tuy nhìn giống "nhiều nhân vật"

Ba thứ dưới đây **vẫn cần** trên mobile, vì "người khác" là nhân vật của **người chơi khác trên máy khác**,
game vẫn thấy họ đứng quanh:

* **Di chuyển · Theo sau** — đi theo bạn cùng đoàn.
* **Tổ đội · Mời nhóm / vào nhóm theo danh sách** (kèm công tắc `bJoinPtByList` đang mất, §1).
* **Liên đấu · danh sách bạn diễn** (tối đa 8).

Cả ba chỉ cần **chiều về** của `PRT_GETTEAMAROUND` (§2.2 nhóm B).

---

## 6. THỨ TỰ LÀM ĐỀ NGHỊ

| Đợt | Việc | Được gì | Nặng |
|---|---|---|---|
| **1** | Sửa bộ tách `.rc` (§1), sinh lại bảng + 15 ini | Lấy lại 4 công tắc: *nhẹ máy 1 hiệu ứng*, *giữ nhẫn/dây*, *mời nhóm theo danh sách*, *Tín Sứ* | rất nhẹ, chỉ Python |
| **2** | Gỡ 34 ô §5 khỏi bộ sinh | Giao diện sạch, hết ô xám vô nghĩa | nhẹ, chỉ Python |
| **3** | Lọc danh sách chiêu theo 7 loại (§3.1) | Hết chọn nhầm chiêu — **đang là lỗi âm thầm** | nhẹ (dữ liệu đã có) |
| **4** | Menu cuộn / chia trang (§3.2) | Chọn được chiêu thứ 24+, bản đồ Xa Phu thứ 24+ | vừa |
| **5** | 3 hộp chọn tĩnh: Rương cửa, Boss cố định, Dòng thuộc tính (§2.2 A) | 3 tính năng sống lại | nhẹ |
| **6** | 10 ô số liệu nhân vật (§3.3) + nhớ trạng thái Bật/Tắt (§3.4) | Nhìn là biết máy đang ở đâu | nhẹ |
| **7** | Chiều về IPC + 3 hộp chọn "người quanh mình" (§2.2 B) | Theo sau / Tổ đội / Liên đấu | vừa |
| **8** | Widget danh sách + 4 danh sách + 20 nút thao tác mảng (§2.2 C, D) | Đi tuần toạ độ, lọc nhặt, danh sách nhóm, bạn diễn | **nặng nhất** |
| **9** | 2 hộp thoại con (§4) | Không nhặt theo tên, Thứ tự ngũ hành | vừa |
| **10** | Ghi chú + hướng dẫn theo thẻ (§3.6) | Người chơi hiểu 279 ô để làm gì | vừa, chữ có sẵn |
| **11** | Dọn §3.7: lớp vẽ đè, chạm vào chữ, rà tiêu đề nhóm | Cảm giác dùng | nhẹ |

---

## 7. NHỮNG CHỖ ĐÃ SOI VÀ **ĐÚNG** (không phải sửa)

Ghi lại để lần sau khỏi soi lại:

* **7 / 15 thẻ nối đúng 100 %**: Phục hồi (31/31), Chiến đấu (22/22), Hậu cần (28/28), Dã Tẩu (17/17),
  H.động (14/14 — trừ ô Tín Sứ ở §1), Chiêu KH (21/21), CTC (15/15). Tám thẻ còn lại chỉ hỏng đúng những ô
  đã liệt kê ở §1 và §2.
* **Mảng có chỉ số** đặt đúng: `bTKGio[0..3]` (4 khung giờ Tống Kim), `nComboSkill[0..5]` + `nComboDelay[0..5]`
  (6 khe chiêu kết hợp) — đã tra từng offset, khớp `ipc_shared.h`.
* **Tên tệp cấu hình trùng bản PC**: `APdata\<mã nhân vật>.dat`, 7.644 byte — chép qua lại được.
* **Bộ não auto** (180 hàm `DT_/TK_/LD_/CT_/ST_/HD_/AC_` trong `CoreShell.cpp`, `ExtAutoLoop`) **không thiếu gì** —
  toàn bộ đã dịch vào `libCoreClient.so` / `libmain.so`. Cái thiếu chỉ là **phần vỏ**.
* **Thứ tự nhóm / thẻ** giữ đúng `s_aNhomTab` của WAuto.exe → người chơi PC sang mobile không phải học lại.
* Bản mobile có **thêm** so với PC: nút bàn tay *Nhặt ngay 6 s* (`JxWAuto_NhatNgay`) và nhường quyền điều khiển
  khi đang kéo cần (`WA_TAY_CAM`) — hai thứ PC không có.
