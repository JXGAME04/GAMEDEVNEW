# BÀN GIAO 30/08 — GỠ CÁC TÍNH NĂNG CŨ ĐÃ CÓ BẢN PORT TỪ LINUX/JX2

**Yêu cầu của chủ game:** *"tôi đã port một số tính năng ở LINUX qua nên sẽ trùng
tính năng — bạn thấy cái nào 2 tính năng như nhau thì bỏ phần tính năng ở dự án
cũ để lại phần tính năng đã port từ linux qua"*, và *"đọc lại phần .md của tôi
or git để biết tính năng nào đã port qua"*.

**Nguyên tắc đã theo:**
- **Không xoá gì.** Mọi thứ được **DỜI** sang `bin\server\_dara\script\<đường dẫn cũ>`,
  giữ nguyên cấu trúc — muốn lấy lại chỉ cần chép ngược. Nhật ký ở
  `_dara\NHATKY_DOI.txt`.
- Sửa tại chỗ thì **comment kèm dấu mốc `[GOHECU 30/08]`** chứ không xoá dòng —
  đúng lối các đợt port trước đã dùng (`timerserver.lua:18 "[WLLS port
  20/08/2026] da go he lien_dau cu"`).
- Mỗi tệp sửa xong đều **đọc lại kiểm byte**, **so cân bằng từ khoá Lua**, và
  **chạy qua trình phân tích cú pháp Lua 4.0.1 thật** (`syncheck.exe`, dựng từ
  chính `Sources\Library\LuaLib` của dự án).
- Tệp gốc trước khi sửa để cạnh, đuôi `.truoc_gohecu`.

---

## 1. CĂN CỨ — vì sao chắc bản nào là cũ, bản nào là bản port

Không đoán. Ba nguồn độc lập cùng chỉ một hướng:

| Nguồn | Bằng chứng |
|---|---|
| **Ghi chú trong chính cây script** | `startgame.lua:104` *"[3HD 25/08] TAT ban VN Phong Lang Do (thay bang ban Linux)"* · `balanghuyen.lua:79` *"[DOT-E1 bo he CTC cu]"* · `timerserver.lua:18` *"[WLLS port 20/08/2026] da go he lien_dau cu"* · `item\lbhtdatau.lua:3-4` *"he do da MO COI sau dot port 15-16/08"* |
| **Tài liệu bàn giao + git** | `THICONG_LIENDAU_PORT.md` D8 *"Go he cu"* · `THICONG_DOT1_LOIDAI_TINSU.md:72` *"Phải gỡ (bản tự viết trùng chức năng): script/tinhnang/loidai/"* · commit `21e570bf` chép lời chủ *"bo di tinh nang lien dau co san o du an"* |
| **Trạng thái chạy thật (tôi tự đo)** | `ch_lich.lua`: `BAT_HD3=1`, `BAT_VIEMDE=1`, `BAT_CTC_JX2=1` — bản port **BẬT**; `BAT_VUOTAI_VIET=0`, `BAT_PHONGLANGDO_VIET=0`, `BAT_CONGTHANH_VIET=0`, `BAT_LOIDAI_BANGHOI=0`, `BAT_LIENDAU=0` — bản cũ **TẮT** |

Thêm một bằng chứng mạnh: `hd3_driver.lua:52-67` (bản Linux đang chạy) **chủ động
xoá NPC bản Việt mỗi phút** — tức đợt port trước đã coi bản cũ là thứ phải dọn.

---

## 2. ĐÃ LÀM — sáu đợt

### Đợt 1 — dời 4 tệp mồ côi hoàn toàn (1.396 dòng)

| Tệp | Vì sao chắc mồ côi |
|---|---|
| `header\liendau.lua` | 0 chỗ gọi; thư mục `tinhnang\lien_dau` đã dời từ 20/08 |
| `global\npcchucnang\datau.lua` (828 dòng) | `grep "AddNpc.*datau"` = 0 — không NPC nào mang script này |
| `item\lbhtdatau_cu_1908.lua` | bản cũ của `lbhtdatau.lua` |
| `global\npcchucnang\nhieptran.lua` (405 dòng) | NPC bị `hd3_driver` xoá mỗi phút; bản thay = `task\tollgate\killer\nieshichen.lua` |

### Đợt 2 — gỡ 8 dòng Include/hằng chết
`startgame.lua` ×2, `item\lenhbaiadmin.lua` ×3, `timertask\task07.lua` ×1,
`lib\lib_map.lua` ×2 (`OTHER_SGSTHU`, `OTHER_DATAU` — không nơi nào đọc; 7 chỗ
dùng `OTHER_SGSTHU` đều đã comment sẵn).

> Cách xác minh: liệt kê **41 hàm chỉ tồn tại trong 3 thư viện cũ**, rồi kiểm từng
> tệp gọi hàm nào — cả 4 tệp trên gọi **0** hàm riêng.

### Đợt 3 — gỡ lịch 4 hoạt động cũ khỏi `timerserver.lua` (208 dòng)

| Hàm gỡ | Dòng | Bản thay đang chạy |
|---|---|---|
| `sukien_congthanh` | 57 | Công Thành JX2 (`CTC_JX2_Tick`) |
| `sukien_loidaibanghoi` | 31 | Lôi Đài JX2 (`citywar_arena`, TimerTask 16/17) |
| `sukien_vuotai` | 28 | `challengeoftime` (Linux, `HD3_Tick`) |
| `sukien_phonglangdo` | 40 | `fengling_ferry` (Linux, `HD3_Tick`) |

Kèm 4 nhánh gọi, 4 dòng `Include`, và **bỏ 4 khoá cấu hình đã thành vô nghĩa**
(`BAT_CONGTHANH_VIET`, `BAT_LOIDAI_BANGHOI`, `BAT_VUOTAI_VIET`,
`BAT_PHONGLANGDO_VIET`) — công tắc không điều khiển gì thì tệ hơn là không có.

### Đợt 4 — 12 tệp còn móc vào hệ cũ

**Rút về khung rỗng** (8 tệp, 496 dòng) — `mission02/03/04/06.lua`,
`task04/05/06/07.lua`. **Không xoá tệp** vì engine nạp theo *số hiệu*; tệp rỗng
thì số hiệu vẫn hợp lệ. Khuôn khung rỗng chép y `missions\mission07.lua` và
`mission09.lua` — hai tệp mà đợt port JX2 **đã làm đúng kiểu này từ trước**.
Bản gốc chép sang `_dara\...\*.goc`.

**Gỡ riêng nhánh chết** (3 tệp còn sống vì Tống Kim dùng chung) —
`task01/02/10.lua`: giữ nguyên nhánh Tống Kim, gỡ nhánh Công Thành cũ + Vượt Ải
cũ + Phong Lăng Độ cũ.

**Nội tuyến hằng** — `item\longxuewan.lua` (Long Huyết Hoàn) chỉ cần
`MAX_NUM_LONGHH_DAY` từ thư viện cũ; chép thẳng giá trị (`= 1`) rồi bỏ `Include`.

### Đợt 5 — ba tàn dư cuối, trong đó **một lỗi đang xảy ra**

**🔴 Ba bẫy Công Thành cũ trên bản đồ 221.** `startgame.lua:117` gọi
`addtrapcongthanh()` mỗi lần khởi động. Trong hàm đó 9/12 dòng đã comment từ
trước, **còn 3 dòng sống** đặt bẫy bán kính 30 tại (1472,3085), (1535,3029),
(1409,3149) trên `ID_MAP_CTC = 221`.
Bản đồ 221 = *"Công thành chiến trường"* — **đúng bản đồ Công Thành JX2 đang
chạy** (`citywar_city\head.lua:83-84  CS_CampPos1/2 = {221,...}`).
Ba bẫy chạy `chancong_1/2/3.lua`, đọc `GetMissionV(M_CTHANHVO_1..3)` của
mission **2** (không còn mở nữa → luôn = 0) rồi `SetPos` búng người chơi lùi.
→ **Giữa trận Công Thành JX2, ai đi qua 3 điểm đó bị bắn lùi.** Đã comment 3 dòng.

**Mìn trong đồng hồ máy chủ.** `sukien_liendau` (`timerserver.lua:808`) Include
`\script\tinhnang\lien_dau\data\...` — thư mục **đã dời đi từ 20/08**; hàm còn
dùng `TIME_LIEN_DAU`/`TAB_DANHSACHTRUNG`/`TAB_DANHSACHCAO` mà **không nơi nào
khai**. Đang tắt nhờ `BAT_LIENDAU=0`, nhưng cái tên công tắc dễ khiến người ta
bật lên tưởng là bật Liên Đấu — trong khi Liên Đấu **thật** (WLLS) chạy độc lập,
không đọc khoá này. Đã gỡ hàm + nhánh gọi + khoá.

**Mục menu chết.** `lenhbaiadmin.lua:512 "Test Phong Lăng Độ/admintestPLD"` —
hàm chỉ có trong thư viện Phong Lăng Độ cũ; bấm vào là gọi hàm nil. Đã comment.
(Quét toàn cây: đây là **mục menu chết duy nhất**.)

### Đợt 6 — dời 4 thư mục hệ cũ (23 tệp, 3.029 dòng)

Làm theo bậc vì chúng phụ thuộc nhau: `tinhnang\vuot_ai` → `startgame\khac\satthu.lua`
→ `tinhnang\boss_satthu` → `tinhnang\phonglangdo` → `tinhnang\loidai`.

---

## 3. **KHÔNG DỜI** — và lý do cứng

| Thứ | Lý do |
|---|---|
| `tinhnang\datau\` | `KNpc.cpp:1698` gọi cứng `danhquai.lua` **mỗi lần người chơi giết quái**. Hơn nữa tệp đó nay còn mang **móc đếm giết quái của Bạn Đồng Hành** (`danhquai.lua:5,20` → `BDH_OnKillNpc`, port 27-28/08). |
| `tinhnang\congthanhchien\` | `KProtocolProcess.cpp:6874` gọi cứng `ghilog_tax.lua`. Thư mục còn chứa **dữ liệu dùng chung**: `danhsach_bang.lua` (playerlogin đọc), `mapbanghoi.lua`, `log_tax.lua`, `quany.lua` (bản JX2 đọc), `ruongchua.lua`. |
| 2 rương ở bản đồ 222/223 (`lib_ctc.lua:229-230`) | Bản JX2 có xa phu / quân nhu quan / vệ binh / dược thương cho hậu phương **nhưng không có rương**. Bỏ đi là mất chỗ gửi đồ. **Chờ chủ quyết.** |
| `tinhnang\loidaihonchien\` | **Không có bản Linux thay thế** — bỏ là mất hẳn một hoạt động. Xem mục 5. |

---

## 4. NGHIỆM THU

| Phép kiểm | Kết quả |
|---|---|
| Tham chiếu treo (trỏ tới tệp đã dời) | **2** — cả hai ở `hd3_driver.lua:49-50`, là **tên script để XOÁ NPC cũ**; thư mục đi rồi thì lời gọi thành vô hại. Cố ý giữ (mã của hệ đang sống). |
| Lời gọi hàm nil | **0** |
| Cú pháp Lua 4.0.1 (`syncheck.exe`) | **1.680/1.680 tệp tên ASCII đạt** |
| 1.398 tệp tên tiếng Trung | cân bằng từ khoá lệch = **0** (không truyền qua argv được nên kiểm bằng cân bằng) |

**Tổng đưa ra khỏi cây: 35 tệp / 4.921 dòng.** Cộng ~950 dòng gỡ tại chỗ.

---

## 5. CẦN CHỦ QUYẾT — chưa động vào

1. **Lôi Đài Hỗn Chiến** (`tinhnang\loidaihonchien`, bản đồ 210) — **nửa sống**:
   NPC báo danh vẫn còn (`balanghuyen.lua:84`) nhưng đồng hồ trao thưởng tắt
   (`BAT_LOIDAI_HONCHIEN=0`). Vào được, ăn exp mỗi mạng, **trận không bao giờ kết
   thúc**. Không có bản Linux thay. → **Bật hẳn hay tắt hẳn?**
2. **Hai rương hậu phương** bản đồ 222/223 (mục 3) — giữ hay bỏ?
3. **Bốn hoạt động cũ đang tắt, không có bản thay**: Boss Hoàng Kim, Hạt Huy
   Hoàng, Trạng Nguyên/Hoa Đăng, Trống Bang Hội. Đang tắt bằng `BAT_*=0`. Gỡ hẳn
   hay để đó?
4. **Long Huyết Hoàn** (vật phẩm 2128) đặt lại lượt Vượt Ải bằng task 43, nhưng
   hệ `challengeoftime` mới **không đọc task 43** → vật phẩm hiện **vô dụng**.
   Sửa cho trỏ sang bộ đếm mới hay bỏ vật phẩm?
5. **Rương Dã Tẩu** (vật phẩm 1341) trỏ `\script\item\ruongdatau.lua` — **tệp
   không tồn tại**; bấm vào không có gì xảy ra.
6. **Dọn triệt để hệ Dã Tẩu cũ**: cách sạch là tách móc Bạn Đồng Hành khỏi
   `danhquai.lua` ra tệp riêng rồi sửa `KNpc.cpp:1698` trỏ sang đó — nhưng phải
   dựng lại `CoreServer.dll` và tắt cụm để swap. **Chờ lệnh.**

---

## 6. HOÀN TÁC

Từng phần:
```bash
python D:/GAMEDEVNEW/ReverseTools/cauhinh/doi_tep.py
```
hoặc trong Python: `import doi_tep; doi_tep.hoan_tac("tinhnang/vuot_ai", ghi=True)`.

Tệp sửa tại chỗ: chép đè bản `.truoc_gohecu` nằm cạnh.

## 7. CÔNG CỤ (đều để trong `ReverseTools\cauhinh\`)

`t35_quet_mocoi_he_cu.py` · `t36_do_phu_thuoc_he_cu.py` · `t37..t43` (sáu đợt) ·
`t41_quet_menu_chet.py` · `t44_nghiem_thu.py` · `lua_ham.py` (dò ranh giới hàm
Lua theo cân bằng từ khoá) · `doi_tep.py` (dời-không-xoá, có chốt "còn ai gọi").
