# THIẾT KẾ LẠI TAB + GIAO DIỆN WAuto — theo bài học từ auto Thái Lan

> Viết 24/08/2026 theo yêu cầu chủ game: *"đọc phân tích PHANTICH_SOURCE_AUTO_THAILAN để
> thiết kế lại các tab chức năng và giao diện cho WAuto"*.
> Mọi số liệu về auto Thái Lan trong file này là **đo thật** trên
> `D:\Source_ANTITHAILAN\Source_ANTITHAILAN\Keoxe365\Keoxe365\Form1.cs`, không chép lại từ
> bản phân tích cũ.

---

## 1. Giao diện auto Thái Lan thật sự là gì (đo được)

**36 tab, nhưng người dùng chỉ nhìn thấy 4 tab ở tầng ngoài.** Nó dùng **10 `TabControl`
lồng nhau, sâu 3 tầng**:

```
[Điều khiển]  264×383 px
   └─ Bảng 1 · Bảng 2 · TK · CTC · Train
         CTC   └─ Thất thành · Tam trụ · Tam môn đài
         Train └─ Thiết lập 1 · Thiết lập 2
[Hậu cần]
   └─ Mua thuốc · Phục hồi · Chiêu thức · Vũ khí
         Mua thuốc  └─ Nâng cao · Chuyển thuốc · Gán ô tắt
         Chiêu thức └─ Chuyển chiêu trái · Chiêu phải (ms) · Gặp Boss
[Cài game]
   └─ Thông tin · Lic HDD · Khác
[Phụ trợ]
   └─ Chung1 · Chung2 (Tín sứ) · Event · Boss · Test
         Event └─ Lọc trang bị · Mua dùng · Bán · VSThông
```

Ngoài ra **45 cửa sổ rời** (`FormMayphu`, `FormClickNPC`, `FormChayBoss`, `FormLocdo`…) cho
những thứ cần bảng dài / danh sách — không nhét vào tab.

**Ba nguyên tắc rút ra (đây mới là thứ đáng chép, không phải danh sách tab):**

| # | Nguyên tắc | Bằng chứng |
|---|---|---|
| N1 | **Tầng ngoài ít, tầng trong nhiều.** 36 tab gói trong 4 nhóm — người mới chỉ phải chọn 1 trong 4 | `tabControl1` có đúng 4 `TabPage` |
| N2 | **Việc nào có danh sách/bảng thì tách ra cửa sổ rời**, không nhồi vào tab | 45 `FormXxx`, ví dụ `FormLocdo` 0 lệnh đọc bộ nhớ nhưng 75 lệnh gọi engine |
| N3 | **Một tính năng = một tab riêng, đặt tên đúng tên hoạt động trong game** (TK, CTC, Thất thành, Tam trụ, Tín sứ) — không gộp vào "Nâng cao" | tên tab trùng tên hoạt động |

---

## 2. WAuto hôm nay

Hộp thoại **160 × 354 đơn vị dialog**, dải nút tab kiểu radio-pushlike, **1 tầng, 11 tab**:

| # | Tab | Tiêu đề nhóm |
|---|---|---|
| 0 | Cơ bản | phiên chơi, đăng nhập |
| 1 | Phục hồi | máu, phù, tự dùng |
| 2 | Chiến đấu | mục tiêu, kỹ năng |
| 3 | Di chuyển | theo sau, tọa độ |
| 4 | Nhặt đồ | lọc, giữ đồ quý |
| 5 | Hậu cần | bán, mua, rương, đi lại |
| 6 | Tổ đội | mời nhóm, quản lý |
| 7 | PK | mục tiêu, bùa, ưu tiên |
| 8 | Dã Tẩu | 40 nhiệm vụ/ngày |
| 9 | Tống Kim | tự tham gia đúng giờ |
| 10 | **Liên đấu** | tự thi đấu theo khung giờ ← **thêm 24/08** |

Cửa sổ rời đã có: **Lọc series** (`IDD_SETSERIES_DIALOG`), **Không nhặt theo tên**
(`IDD_NOPICK_DIALOG`) — đúng nguyên tắc N2 sẵn rồi.

**Chỗ đau trước đợt này:** dải nút tab lệch — hàng 1-2 mỗi hàng 3 nút rộng 49/50, hàng 3 bị
bóp thành 4 nút rộng 37. Thêm tab thứ 11 là hết chỗ.

---

## 3. ĐÃ LÀM 24/08 (đợt Liên đấu) — bước 1 của thiết kế

**Xếp lại dải nút tab thành LƯỚI ĐỀU 4 cột × 3 hàng**, mỗi nút rộng **39**, x = 2 / 41 / 80 / 119,
y = 72 / 85 / 98. Bề rộng 39 là **đo thật bằng GDI**, không ước lượng: Segoe UI 9pt cho 1 đơn vị dialog = 6,91/4 px, nên 37 đơn vị chỉ còn ~56 px cho chữ trong khi *Chiến đấu* 59 px / *Di chuyển* 58 px / *Tống Kim* 56 px → **bị cắt chữ**; 39 đơn vị cho ~61 px, đủ cả ba.

* 12 chỗ, đang dùng 11 → còn **1 chỗ trống** cho tính năng kế tiếp.
* **Không một điều khiển nào khác phải dời** — groupbox mọi tab vẫn bắt đầu ở y = 112,
  nên rủi ro gần bằng 0 và người chơi cũ không phải học lại chỗ nào.
* Ba nút cuối (`IDC_TABBTN_9/10/11` = ID 609/610/611) nằm **ngoài** dải ẩn/hiện
  `[IDC_STRING_0_L, IDC_INDEX_END)` nên phải ánh xạ tay trong `WM_COMMAND` — đã làm.

**Đồng thời nới hạ tầng ID** (bẫy đã dính một lần ở đợt Tống Kim):
`IDC_INDEX_END` 480 → **560**, khối ID ngoài dải đẩy 500-510 → **600-611**.
⇒ còn chỗ cho **~2 tab nữa** trước khi phải nới tiếp.

**CHƯA làm** (chờ chủ game duyệt vì đụng tới thói quen người dùng): dựng tầng tab thứ hai.

---

## 4. ĐỀ XUẤT — cấu trúc 2 tầng khi vượt 12 tab

Giữ đúng tinh thần N1 nhưng **không dùng `TabControl` của Windows** (WAuto là dialog thuần,
mọi điều khiển ẩn/hiện bằng tay — đổi sang TabControl là viết lại toàn bộ `ShowTab`).
Cách rẻ nhất, đúng thói quen sẵn có:

```
hàng 1 (y=59)  [ Nhân vật ] [ Săn quái ] [ Hoạt động ] [ Hệ thống ]   ← 4 nút NHÓM
hàng 2 (y=72)  ... tối đa 8 nút tab của nhóm đang chọn, 2 hàng 4 cột ...
hàng 3 (y=85)
(groupbox lùi từ y=112 xuống y=99 — mỗi tab dôi ra 13 đơn vị chiều cao)
```

Phân nhóm đề xuất (11 tab hiện có + chỗ cho tương lai):

| Nhóm | Tab hiện có | Chỗ trống dự phòng |
|---|---|---|
| **Nhân vật** | Cơ bản · Phục hồi · Tổ đội | Gán ô tắt · Thông tin |
| **Săn quái** | Chiến đấu · Di chuyển · Nhặt đồ · Hậu cần | Lọc trang bị · Quây quái |
| **Hoạt động** | Dã Tẩu · Tống Kim · **Liên đấu** | Tín Sứ · Lôi Đài · Công Thành · Bí cảnh |
| **Hệ thống** | PK | Cài game · Nhật ký |

**Giá phải trả:** mọi tab mất 13 đơn vị chiều cao (groupbox 219 → 206) ⇒ phải dời **toàn bộ**
toạ độ y của ~300 điều khiển trong `WAuto.rc`. Việc này **cơ học nhưng rộng** — nên làm bằng
script (đọc `.rc` UTF-16, cộng offset cho mọi dòng trong khối tab), không sửa tay.

**Khi nào nên làm:** khi cần tab thứ **13**. Trước đó lưới 4×3 vẫn đủ và an toàn hơn.

---

## 5. Việc auto Thái Lan làm được mà WAuto CHƯA có (lộ trình tính năng)

Xếp theo giá trị / công sức, dựa trên danh sách 36 tab + 45 form của nó:

| Ưu tiên | Tính năng bên Thái Lan | WAuto | Ghi chú thi công |
|---|---|---|---|
| 1 | **CTC / Công Thành Chiến** (tab CTC) | ✗ | dự án đã port Công Thành — chỉ thiếu lớp auto |
| 2 | **Thất Thành · Tam Trụ · Tam Môn Đài** | ✗ | 3 hoạt động theo giờ, khuôn giống hệt Tống Kim/Liên đấu |
| 3 | **Tín Sứ** (tab Chung2) | ✗ | server đã port 21/08 — làm được ngay |
| 4 | **Chạy Boss theo danh sách** (`FormChayBoss`, `Class103` + tự ghép Sát Thủ Giản) | 一 phần (tab PK có "Bỏ qua boss vàng") | cần bảng boss + cửa sổ rời |
| 5 | **Click NPC theo kịch bản** (`FormClickNPC`) | ✗ | rất mạnh: người chơi tự khai NPC + dòng thoại; giảm hẳn việc phải code từng hoạt động |
| 6 | **Mây Phù / dịch chuyển hàng loạt** (`FormMayphu`, 425 lệnh engine) | 一 phần | WAuto có `ATYPE_RETURN` + Xa phu |
| 7 | **Lượm rác / Lọc trang bị nâng cao** (`FormLocdo`, `FormLuomrac`) | ✓ có (tab Nhặt đồ + Lọc) | đủ dùng |
| 8 | **Chuyển chiêu trái / Chiêu phải theo ms** | 一 phần (`nSkillIdC`, `nSkillCSec`) | thiếu bảng nhiều chiêu |
| 9 | **Vượt ải Nhiếp Thí Trần** (`FormNhiepTT`) | ✗ | |
| 10 | **Huyền Tinh / chế đồ** (`FormHuyenTinh`) | ✗ | |

**KHÔNG nên chép:** cơ chế nền của nó — dò 277 chữ ký AOB rồi bắn shellcode
`60 … E8 … 61` + `CreateRemoteThread` vào client gốc. WAuto **là mã trong chính client**
(`CoreClient.dll` + `Game.exe`), gọi hàm trực tiếp: nhanh hơn, không gãy khi đổi build, và
không phải nuôi bảng địa chỉ. Cái đáng chép chỉ là **bố cục UI + độ phủ tính năng**.

---

## 6. Luật giao diện chốt cho WAuto (áp từ đợt này trở đi)

1. **Một hoạt động theo giờ = một tab riêng**, tên đúng tên trong game (Tống Kim, Liên đấu,
   Tín Sứ…). Không nhồi vào "Nâng cao".
2. **Mọi tab dùng chung khung**: `GROUPBOX "Tên · mô tả ngắn"` ở (2, 112, 156, 219), các ô bắt
   đầu y = 124, mỗi dòng cao 10, cách nhau 12; kẻ phân mục bằng `SS_ETCHEDHORZ` rộng 148.
3. **ID điều khiển của tab PHẢI liền mạch và nằm trong `[IDC_STRING_0_L, IDC_INDEX_END)`** —
   `ShowTab` ẩn cả dải này. Groupbox + kẻ phân mục cũng phải trong dải.
   ID nào ra ngoài dải (nút tab, hộp thoại rời) phải **ánh xạ tay** trong `WM_COMMAND`.
4. **Mọi ô mới phải nối đủ 5 chỗ** trong `WAuto.cpp`, thiếu một chỗ là mất cấu hình:
   `SaveRoleData` → `UpdateUI` → mặc định (nhánh chưa có `.dat`) → **nhánh di trú
   `uOldSize <= offsetof(autoData, <trường đầu khối>)`** → một trong ba đường tự lưu
   (`BN_CLICKED` / `CBN_SELCHANGE` / `EN_KILLFOCUS`).
5. **Trường cấu hình mới PHẢI thêm ở CUỐI `struct autoData`** — `.dat` ghi nguyên struct,
   chèn giữa là nát cấu hình của mọi người chơi cũ.
6. **Ô nào cũng phải có ghi chú (tooltip)** trong bảng `s_aTips` — người chơi không đọc tài liệu.
7. **Danh sách / bảng dài → cửa sổ rời**, đúng nguyên tắc N2.
8. **Chỉ tin số liệu trích tự động từ script sống** (`gen_*_tables.py`), không gõ tay toạ độ
   / marker thoại vào `.h`.

---

## 7. Việc kế tiếp đề xuất (theo thứ tự)

1. **Test thật auto Liên đấu** (`BANGIAO_AUTO_LIENDAU_2408.md` mục 7) — trước khi thêm gì nữa.
2. **Tab Tín Sứ** (chỗ trống thứ 12 của lưới) — server đã có, khuôn giống Liên đấu.
3. Khi cần tab thứ 13 → dựng tầng nhóm ở mục 4 **bằng script dời toạ độ**, không sửa tay.
4. Cân nhắc **FormClickNPC kiểu Thái Lan** (người chơi tự khai NPC + dòng thoại) — một lần
   làm, dùng được cho mọi hoạt động sau này, đỡ phải code từng cái.

*Ghi 24/08/2026.*
