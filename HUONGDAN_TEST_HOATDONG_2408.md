# HƯỚNG DẪN TEST 4 HOẠT ĐỘNG + FILE CONFIG (24/08/2026)

Bộ này gồm 3 phần đúng theo yêu cầu:
1. **Menu test trên Lệnh Bài Admin** — gọi mọi tính năng bất cứ lúc nào, không cần chờ lịch.
2. **Hướng dẫn test từng bước** (tài liệu này).
3. **1 file config trung tâm** `bin\server\script\header\cauhinh_hoatdong.lua` — chỉnh MỌI THỨ ở một chỗ.

---

## 0. CHUẨN BỊ (làm 1 lần)

1. **Tài khoản admin**: account phải nằm trong `GMLIST` tại `bin\server\script\lib\lib_ham.lua:17`. Thêm account rồi restart (hoặc dùng nút reload script có sẵn).
2. **Lệnh Bài Admin** = item dòng 4816 `settings\item\magicscript.txt` (genre 6, detail 1, id **4814**). Nhân vật GM nào chưa có thì phát bằng lệnh bài admin của nhân vật khác (mục Nhận vật phẩm) hoặc bằng công cụ phát đồ sẵn có của anh.
3. **Restart GameServer một lần sau đợt sửa này** để mọi state nạp bản script mới (menu thì dùng được ngay không cần restart, nhưng 18 file tính năng đã nối config chỉ "sống" sau restart hoặc sau khi bấm *Nạp lại CONFIG*).
4. Client: cần **2-3 nhân vật cấp ≥90** để test Tỷ Võ/Bách Nhân; Bang Chiến cần nhân vật có bang.

**Mở menu**: chuột phải Lệnh Bài Admin → dòng **"Hoat dong 23-24/08 (BC-BN-TV-TB): bo test"**.

Menu gồm 8 mục:
| # | Mục | Dùng để |
|---|-----|---------|
| 1 | Bang Chiến | trạng thái, ép khởi tạo mùa, ép tick, ép mở báo danh, dịch chuyển |
| 2 | Bách Nhân Lôi Đài | xem cấu hình đang chạy, dịch chuyển, gọi lại Init |
| 3 | Tỷ Võ Đài | dịch chuyển 3 thành, vào xem đấu trường, gỡ kẹt trận |
| 4 | Bang Hội Thành Bảo | hồi sinh/xoá cây ngay, xem/cộng điểm Thần Mộc, dịch chuyển |
| 5 | Nhận item thưởng | phát 4864/2273/3204/3205-3207/4857 để test |
| 6-7 | BOT Tống Kim BẬT/TẮT | `PB_SetTongKim(1/0)` |
| 8 | **NẠP LẠI CONFIG** | áp giá trị mới trong config **không cần restart** (xem mục 6) |

---

## 1. TEST TỶ VÕ ĐÀI (nhanh nhất — làm trước)

Cần 3 nhân vật ≥90 (2 đội trưởng + 1 người nhập số). Không có lịch — đăng ký lúc nào cũng được.

1. Menu → *3. Ty Vo Dai* → **"Dich chuyen: Cong Binh Tu DUONG CHAU"** (map 80, 1659/3020).
2. Nhân vật A lập đội đủ **2 người**, đội trưởng nói chuyện Công Bình Tử → "Được thôi!" → chọn **"2 vs 2"** → nhận **số thứ tự** (đội trưởng xem lại số bằng chính NPC).
3. Chọn "Ta là tuyển thủ" → cả đội vào **map 209** (đội Giáp 1620/3202, đội Ất 1612/3187).
4. Nhân vật B (đội 2) làm giống hệt → nhận số thứ tự 2.
5. Nhân vật C nói chuyện NPC → nhập số qua **HỘP SỐ client** (AskClientForNumber — tính năng engine mới) để vào đúng phe.
6. Sau **2 phút** (config `BW_PHUT_CHO`) trận bắt đầu: đánh nhau CÓ sát thương, chết **không mất exp/đồ**, bị loại khỏi đài; một phe hết người → phe kia thắng, tất cả về chỗ cũ, giao dịch/bày bán dùng lại được.
7. Trận kéo dài tối đa `BW_PHUT_TRAN` (12) phút → hết giờ so số người sống.
8. **Nếu có nhân vật kẹt trạng thái** (rớt mạng giữa trận…): menu → *"Go ket tran treo cua TOI"* (xoá TaskTemp 200 + task 2340-2342).

Khán giả: menu → *"Vao dau truong 209 (cho khan gia)"* (không bị chặn cấp).

---

## 2. TEST BÁCH NHÂN LÔI ĐÀI

Mở **12:00–24:00 hằng ngày** (`BR_GIO_MO`/`BR_GIO_DONG_H`), map 960, 5 đài.

1. Menu → *2. Bach Nhan* → *"Xem cau hinh dang chay"* (kiểm giờ/exp/trần đang áp dụng).
2. *"Dich chuyen: NPC loi vao (Lam An)"* → nói chuyện **Quan nhắc nhở Hoàng Thành Tứ** → "Ta muốn vào…" (cần cấp ≥`BR_CAP_TOITHIEU`; trước 12:00 sẽ bị chặn kèm giờ mở hiển thị đúng theo config).
3. Trong map 960 thấy Xa phu / Rương chứa đồ / Chợ dược điếm. Khinh công nhảy lên **đài 1** (~1784/3099) → thành **Lôi Chủ**.
4. Chờ `BR_GIAY_CHO_KHIEUCHIEN` (30 s) không ai lên → **NPC cao thủ** xuất hiện (cấp 90 + số-lượt-thắng/10) → đánh được.
5. Người thứ 2 nhảy lên đài → đếm ngược `BR_GIAY_DEM_NGUOC` (3 s) → đấu; hết `BR_PHUT_MOI_LUOT` (3 phút) → **so sát thương** (Lôi Chủ chịu ÍT sát thương hơn thì thắng).
6. Kiểm exp: mỗi `BR_PHUT_CHUKY_EXP` (5) phút, người trong map +`BR_EXP_TICK` (1tr), Lôi Chủ +thêm `BR_EXP_LOICHU` (2tr); có buff Cô Thư (NPC xuất hiện mỗi 30 phút, 20% người được chọn) thì exp thường **×2**; trần `BR_TRAN_LUOT_NGAY` (50) tick/ngày.
7. Thắng liên tiếp 10 trận ở **đài 1** → loa toàn server (không cắt chữ). 0h → tất cả bị trả về chỗ cũ.

---

## 3. TEST BANG CHIẾN (ép mùa bằng menu)

Điều kiện nền: **≥2 bang đang chiếm thành** (Tương Dương, Biện Kinh… — xem bảng thành). Mùa 9 thật: báo danh 29/10, đấu 29/10→05/11/2026 (nghỉ 04/11), trận 20:00–21:30 (`TW_GIO_KHAICHIEN/KETTHUC`).

Test ép ngay không chờ lịch:
1. Menu → *1. Bang Chien* → *"Xem trang thai"* (chưa có `jx2league.txt` = mùa chưa khởi tạo).
2. **B1. Khoi tao mua 9 NGAY** → kiểm trạng thái lại: `jx2league.txt` phải có **7 dòng G 10**.
3. **B3. Ep mo pha 2 – bao danh 3 cap test** (3 cặp bang + 3 sân lấy từ `TW_TEST_REDO` trong config — sửa id bang theo `jx2league.txt` trước khi bấm) → mission 33 mở trên map 605-607.
4. Nhân vật ≥90 **có bang thuộc liên minh** → *"Dich chuyen: NPC bao danh Ba Lang Huyen"* → NPC **Võ Lâm Truyền Nhân** (1628/3173) → "Ta muốn tham gia thi đấu" → vào map báo danh 608/610/612 → **đạp trap** → về hậu doanh (nằm tối đa `TW_GIAY_HAUDOANH` 120 s).
5. Trong trận kiểm: **+75 điểm/kill** (`TW_DIEM_KILL` × hệ số quân hàm), 3 mạng liên trảm +150, chết đủ `TW_SO_MANG` (10) bị đẩy ra không vào lại được, đủ 5 kill được +1 mạng.
6. Hết `TW_PHUT_TRAN` (90 phút) trận tự kết thúc, ghi league/ladder (BXH 10225-10234).
7. **B2. Ep tick lich NGAY** dùng khi muốn lịch tự nhiên chạy bước kế tiếp ngay (thay vì chờ tick 15 phút).
8. Pha 3 (sau `TW_MUA9[4]`): nhận danh hiệu + thưởng — bảng thưởng ở `TAB_AWORD_GOOD` (đổi qua `TW_THUONG` trong config + restart). Item nhận test nhanh bằng menu *5. Nhan item thuong*.

---

## 4. TEST BANG HỘI THÀNH BẢO

⚠ **Map 984 phía CLIENT chưa có dữ liệu** — vào map bằng client thường sẽ đen màn. Server-side test được: cây/điểm/đổi thưởng/dùng lệnh bài.

Lịch thật: **Chủ nhật 17:00–19:00** (`TC_THU`, `TC_GIO_MO/CAY2/CAY3/DONG`), nhận điểm **19:00–24:00** (`TC_GIO_NHANDIEM_TU/DEN`), điều kiện ≥45 phút trong map + vào bang >1 ngày + **Lâm An có chủ**.

Test không chờ lịch:
1. Menu → *4. Thanh Bao* → **"Hoi sinh cay 1/2/3 NGAY"** → cây spawn trong 984 theo `bronzetree/silvertree/goldtree.txt` (12 Thanh Đồng / 4 Bạch Ngân / 1 Hoàng Kim, tên tiếng Việt, HP 96tr/288tr/960tr).
2. Bang KHÁC chủ thành đánh cây → điểm cộng qua relay — kiểm file `settings\jx2sharedata\MISSION_tongcastle_*.bin` đổi mtime; điểm cây: `TC_DIEM_CAY` (At 15/30/150 – Def 10/20/100 – Cá nhân 20/40/200).
3. **"Cong 500 diem Than Moc (test)"** → *"Dich chuyen: NPC Thanh Bao (Lam An)"* → NPC đổi **Thần Mộc Lệnh** (giá `TC_GIA_LENH_1/2/3` = 10/20/120 điểm) và **bùa Thủ Vệ** (giá `TC_GIA_BUA` 200).
4. **Dùng** Thần Mộc Lệnh (lấy nhanh ở menu *5*): 3205 = +10tr exp, 3206 = +20 chân nguyên, 3207 = +60tr exp +120 chân nguyên (`TC_THUONG_LENH`), trần `TC_LENH_TUAN` (5) lần/loại/tuần — dùng xong **item phải bị trừ** (engine trừ stack-aware, fix F13).
5. Bùa 3204: đứng **gần cây** (bán kính `TC_BANKINH_BUA` 15) dùng bùa → triệu 1 Thủ Vệ (2031-2034, HP 9,6tr), trừ 1 bùa; trần `TC_TRAN_THUVE` (100) con/map.
6. **"Xoa het cay + record"** để dọn sạch sau test (tránh ×2 điểm khi revive lại).
7. Nhận điểm chính thức: trong khung 19:00–24:00, đứng đủ `TC_PHUT_TOITHIEU` (45) phút trong map → NPC "Ta muốn nhận điểm Thần Mộc".

---

## 5. TEST ITEM THƯỞNG + BOT TỐNG KIM

- Menu *5* phát: **Quả Đại Hoàng Kim 4864** (ăn +`QDHK_EXP` 200tr exp, hạn 7 ngày), **Hoàng Chân Đơn 2273** (+`HCD_EXP` 2 tỷ exp CÓ trần cấp 200), bùa 3204, lệnh 3205-3207, Hàn nguyên chân đan 4857. Ăn thử từng cái, kiểm exp cộng đúng số trong config.
- Menu *6/7*: bật/tắt bot tự tham gia Tống Kim (`PB_SetTongKim`) — bật rồi chờ trận Tống Kim gần nhất xem bot vào trận.

---

## 5b. TEST DANH HIỆU + VÒNG SÁNG (Bang Chiến) — menu *9*

**Trước khi test phải thoát hẳn game rồi vào lại một lần.** Bảng danh hiệu chỉ được
client nạp **một lần duy nhất** lúc khởi động (`KNpc.cpp:6049-6056`), nên 9 danh hiệu
mới thêm sẽ không hiện nếu client vẫn đang mở từ trước. Server thì **không cần restart**
(engine đọc lại bảng mỗi lần gán danh hiệu).

| Bấm menu *9* | Phải thấy gì |
|---|---|
| Quân hàm 1 — Binh Sĩ | chữ **Binh sĩ** trên đầu + **vòng sáng dưới chân** (skill 830) |
| Quân hàm 2 — Hiệu úy | chữ **Hiệu úy** + vòng sáng khác (831) |
| Quân hàm 3/4/5 | Thống Lĩnh / Phó tướng / Đại tướng + vòng sáng 832/833/834 |
| Danh hiệu 105/106 | **Võ Lâm Đệ Nhất Bang** + vòng tròn Võ Lâm Đại Hội (1169) |
| Danh hiệu 199 | **Cao Cấp Đệ Nhất Bang** + vòng tròn 1169 |
| Danh hiệu 3000 | **Võ Lâm Minh Chủ** + vòng tròn 1169 |
| Gỡ hết | mất sạch chữ lẫn vòng sáng |

**Vì sao trước đây báo mà không hiện** — có **hai hệ danh hiệu song song**: script Bang
Chiến port từ Linux gọi hệ JX2 (`Title_AddTitle`), hệ này chỉ ghi vào bảng riêng trong
DLL và **không đặt biến mà client dùng để vẽ**. Đã bắc cầu sang hệ JX1 (`SetPlayerTitle`)
và thêm 9 danh hiệu Bang Chiến vào `settings\PlayerTitle.txt` (id 287-295, **cả server
lẫn client**). Hai bảng đánh số **khác nghĩa nhau** (105 = Đệ Nhất Bang ở bảng JX2 nhưng
= *Hiệu úy* ở bảng JX1) nên phải tra bảng `TT_JX1ID`, không gọi thẳng.

**Vòng sáng lấy từ đâu** — cột `ExtSkill1`/`ExtSkillLevel1` của chính bảng đó; engine tự
thi triển khi gán danh hiệu và **tự cấp lại mỗi lần đăng nhập**. Dùng đúng bộ vòng sáng
bản này đã có sẵn: quân hàm 5 cấp = **830-834** (giống quân hàm Tống Kim id 198-202),
danh hiệu bang = **1169** (giống id 286 *Quán Quân THĐNB*). **Không thiếu ảnh** — bảng
ảnh `settings\npcres\state_magic_table_name.txt` đã có sẵn Status136-140 và Status81.

**Skill 661 không phải vòng sáng.** Nó là hiệu ứng *may mắn* vẽ **trên đầu**
(`lucky.spr`). Bản Linux vẫn gọi nên giữ nguyên cho đúng 100%, chỉ sửa lại lời thông báo
cho khỏi hiểu nhầm.

Muốn **đổi hoặc bỏ** vòng sáng: sửa thẳng `settings\PlayerTitle.txt` (cả 2 bản) —
đặt `ExtSkill1 = 0` là bỏ. Bảng tra đầy đủ nằm trong `cauhinh_hoatdong.lua`, mục
*DANH HIEU + VONG SANG*. Cố ý **không** tạo khoá Lua cho nó để tránh hai nguồn cùng
điều khiển một thứ.

---

### Lưu ý mới 25/08 — quái Tín Sứ giờ là BOSS VÀNG
Sau vá `b8d60392`, quái/boss trong ải Tín Sứ mang loại *boss hoàng kim* (tên **màu vàng** như bản Linux).
⚠ WAuto có ô **"Bỏ qua boss vàng"** (tab PK): người dùng auto Tín Sứ phải **ĐỪNG tick** ô này,
tick vào là auto sẽ không đánh quái giữ rương.

---

## 6. FILE CONFIG TRUNG TÂM — `script\header\cauhinh_hoatdong.lua`

- **Một file duy nhất** chứa TẤT CẢ khoá chỉnh của 4 hoạt động + 2 item exp, chú thích tiếng Việt từng dòng.
- Mỗi khoá đánh dấu:
  - **[LIVE]** — sửa xong bấm menu *8. NAP LAI CONFIG* là ăn ngay (đừng bấm khi đang có trận chạy — bảng trạng thái trong file bị nạp lại).
  - **[RESTART]** — sửa xong phải restart GameServer (các hằng nhịp trận/timer đóng băng lúc nạp).
  - **[ENGINE]** — không chỉnh được bằng lua; ghi rõ file `settings\...` + dòng phải sửa (sửa xong restart).
- Nhóm khoá chính:
  | Nhóm | Khoá tiêu biểu |
  |------|----------------|
  | Bang Chiến `TW_` | CAP_TOITHIEU, GIO_KHAICHIEN/KETTHUC, MUA9 (ngày mùa), PHUT_TRAN/CHUANBI, NGUOI_TOIDA/TOITHIEU, SO_MANG, DIEM_KILL/LIENTRAM, GIAY_HAUDOANH, THUONG (bảng thưởng), TEST_REDO |
  | Bách Nhân `BR_` | CAP_TOITHIEU, GIO_MO, GIO_DONG_H, PHUT_CHUKY_EXP, EXP_TICK, EXP_LOICHU, TRAN_LUOT_NGAY, PHUT_COTHU, TILE_BUFFX2, GIAY_CHO/DEM_NGUOC, PHUT_MOI_LUOT/DUNG_YEN/BUFF_X2, TRAN_CHUOI, CAP_NPC_GOC, NPC_MAP/ID/LEVEL |
  | Tỷ Võ `BW_` | CAP_TOITHIEU, DOI_TOIDA, PHUT_CHO, PHUT_TRAN |
  | Thành Bảo `TC_` | CAP_TOITHIEU, VAO_BANG_PHUT, THU (thứ mở), GIO_MO/CAY2/CAY3/DONG, GIO_NHANDIEM_TU/DEN, PHUT_TOITHIEU, DIEM_CAY, GIA_LENH_1/2/3, DOI_LENH_TOIDA, LENH_TUAN, THUONG_LENH, GIA_BUA, MUA_BUA_TOIDA, TRAN_THUVE, BANKINH_BUA |
  | Item | HCD_EXP (Hoàng Chân Đơn), QDHK_EXP (Quả Đại HK) |
- Ví dụ hay dùng khi test: hạ `TW_GIO_KHAICHIEN = 1400` để trận mở 14:00; `BR_GIO_MO = 800` mở Bách Nhân từ 8h sáng; `TC_THU = 6` đổi Thành Bảo sang thứ 7; `BW_PHUT_CHO = 1` đấu Tỷ Võ chờ 1 phút.
- **CẤM Include thêm gì vào file config** (nó phải là "lá" — bài học vòng Include đêm 23/08 làm GameServer boot sập stack).

---

## 7. SỰ CỐ THƯỜNG GẶP

| Hiện tượng | Xử lý |
|---|---|
| Menu không hiện dòng "Hoạt động 23-24.08" | Dùng lại lệnh bài (item tự dofile); nếu vẫn không có → xem `script\item\ScriptError.log` |
| Bấm nút không có phản ứng, GameServer báo lỗi script | 🔴 **Nhãn lựa chọn CẤM chứa dấu `/`** — engine tách nhãn/hàm bằng dấu `/` ĐẦU TIÊN (`ScriptFuns.cpp:716` `strstr(pAnswer,"/")`), nên nhãn kiểu "23-24/08" làm engine hiểu sai tên hàm. Quét bằng `ReverseTools\lua_syncheck\scan_slash.py` |
| Nghi file lua lỗi cú pháp | Build `ReverseTools\lua_syncheck\build_syncheck.bat` (dựng Lua 4.0.1 từ chính LuaLib của engine) rồi `syncheck.exe <file.lua>` — phân biệt lỗi cú pháp thật với lỗi chạy do thiếu hàm engine |
| Sửa config không ăn | Khoá [RESTART]? → restart GS. Khoá [LIVE]? → đã bấm menu 8 chưa |
| Tỷ Võ kẹt "đang trong trận" | Menu 3 → Gỡ kẹt trận treo |
| Bang Chiến B1 không ra jx2league.txt | Chưa đủ 2 bang chiếm thành — chiếm thành trước bằng bộ test cong thanh (TX_Root) |
| Thành Bảo cây không spawn | GS chưa restart sau đợt sửa 24/08? Map 984 load? Xem `script\mission\tongcastle\ScriptError.log` |
| Nạp lại config xong tính năng lạ | Đã bấm khi đang có trận → restart GS cho sạch |

File liên quan: `BANGIAO_PORT5_2308.md` (kiến trúc + quyết định), `cauhinh_hoatdong.lua` (config), `hoatdong_admin.lua` (menu).
