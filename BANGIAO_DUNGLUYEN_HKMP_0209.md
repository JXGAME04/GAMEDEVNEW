# BÀN GIAO — MỞ DUNG LUYỆN CHO HOÀNG KIM MÔN PHÁI / AN BANG / ĐỊNH QUỐC — 02/09/2026

Chủ game: *"dung luyện đang bị giới hạn item dung luyện … tôi muốn có thể dung luyện toàn bộ trang bị ở
hoàng kim môn phái An bang - định quốc trong goldequip.txt … và xem nguyên liệu dung luyện ở bản linux
có từ đâu để áp dụng cho bản dự án"*

Tiếp nối `BANGIAO_DUNGLUYEN_0109.md`. **Không sửa mã C++, không build, không cần swap `.moi`** — chỉ đổi dữ liệu.

---

## 1. Vì sao trước đây không dung luyện được

`KItem::GetFusionCap()` (KItem.cpp:539) → `FUS_TraRowInfo` → `PF_GetRowInfo` (KItemGenerator.CPP:60)
đọc **cột 58** (`可熔炼属性数量` = số ô Văn Cương) và **cột 59** (`可熔炼纹钢品质` = phẩm chất Văn Cương tối đa)
của `settings\item\goldequip.txt`, theo quy ước `dòng tệp = nRow + 2`.

Đo trên tệp đang chạy (7.398 dòng dữ liệu, 62 cột):

| | Số dòng |
|---|---|
| Đã có giá trị cột 58/59 | 3.940 (khối Bạch Hổ / Xích Lân / Minh Phượng / Đằng Long… từ dòng 2286 trở đi) |
| **Để TRỐNG** → `cap = 0` → `SmeltEquip` trả mã 4 | **3.388** (gồm TOÀN BỘ hoàng kim môn phái, An Bang, Định Quốc ở dòng 2–2285) |

Bản Linux (`D:\ServerLinux\server1\settings\item\004\goldequip.txt`, cùng 62 cột, 5.939 dòng) **cũng để trống
y hệt khối này** — 3.311 dòng trống. Nghĩa là: đây không phải lỗi port, mà là **thiết kế gốc của Kiếm Thế**:
hoàng kim môn phái đời cũ vốn KHÔNG dung luyện được. Mở ra là mở rộng có chủ ý theo yêu cầu chủ game.

Cách bản Linux đặt giá trị (để đối chiếu): gần như tất cả là **2 ô**, phẩm chất chia bậc theo đời
đồ — Bạch Hổ 7 · Xích Lân 8 · Minh Phượng 9 · Đằng Long/Tinh Sương/Nguyệt Khuyết 10; chỉ 2 dòng cá biệt
được 4 ô ("Hồng Hoang Chi Giới") và 1 thang test 1→6 ô.

---

## 2. Đã làm — điền cột 58/59 cho 423 dòng

Chủ chốt 02/09: **HKMP + An Bang + Định Quốc, BỎ đồ thuê theo giờ**, mức như sau:

| Nhóm | Cột 58 (số ô) | Cột 59 (phẩm chất) | Số dòng |
|---|---|---|---|
| Hoàng kim môn phái (31 bộ: Mộng Long, Phục Ma, Tứ Không … Định Nghiệp) | **4** | **10** | 367 |
| An Bang | **3** | **8** | 12 |
| Định Quốc | **2** | **7** | 23 |
| `[Cực phẩm]` / `[Hoàn Mỹ]` (ưu tiên cao nhất, đè 3 nhóm trên) | **3** | **9** | 21 |
| **Tổng** | | | **423** |

Loại trừ 213 dòng `[Định thời]` / `[Hạn chế thời gian]` (đồ thuê theo giờ — dung luyện vào rồi hết hạn là
mất luôn Văn Cương).

**Tệp đã sửa** (server và client giống hệt nhau, md5 `5e1e6570570d9a96a03a0d5f49f2781e`):
- `bin\server\settings\item\goldequip.txt`
- `bin\client\settings\item\goldequip.txt`

Sao lưu: `goldequip.txt.truoc_dungluyen_hkmp_0209` ở cả 2 cây.

**Công cụ** (chạy lại nhiều lần được, chỉ điền ô đang trống, đọc/ghi latin-1 nên không phá byte TCVN3):
- `ReverseTools\dungluyen\phanloai_hkmp.py` — phân loại dòng theo 4 mức
- `ReverseTools\dungluyen\va_goldequip_dungluyen.py` — chạy thử; thêm `--that` để ghi

Tự kiểm sau khi vá: đúng **423 dòng khác**, **chỉ cột 58 và 59 đổi**, số byte cao giữ nguyên 115.753,
FFFD = 0, CRLF ×7399, số cột vẫn 62.

### 2.1 Cần làm để có hiệu lực (chủ game tự chạy)
`PF_GetRowInfo` **nạp bảng một lần rồi cache tĩnh** trong tiến trình, nên:
1. Tắt → bật lại **GameServer** (tiến trình hiện tại khởi động 02/09 14:10 đã cache bảng cũ).
2. **Thoát HẲN Game.exe** rồi mở lại (relog nhân vật KHÔNG đủ) — client cũng đọc chính bảng này để
   vẽ tooltip và cho phép bỏ đồ vào box.

Không cần đổi `.moi`: `CoreServer.dll` (13:47, md5 `6c6ad9b5…`) và `CoreClient.dll` (13:47, `768b9ad6…`)
đang chạy đều đã có mã dung luyện (`fusion.txt` xuất hiện trong cả 2 DLL).
Bộ `.moi` 15:31 đang chờ swap là của Vũ Hồn/Tiêu Dao — không liên quan đợt này.

### 2.2 Kiểm chứng trên dữ liệu thật
Truy vấn `jx1_role.role_item`: toàn server có 10 món trang bị hoàng kim, **cả 10 đều `gold_id = 0`**
(sinh theo dòng goldequip) — 9 món thành dung luyện được ngay sau khi restart:

| `hang` | Tên | Sau khi vá |
|---|---|---|
| 402–406 | `[Cực phẩm] Định Quốc` (5 món) | 3 ô / phẩm chất 9 |
| 423–426 | `[Hoàn Mỹ] Cực Phẩm An Bang` (4 món) | 3 ô / phẩm chất 9 |
| 188 | `Hiệp Cốt Tình ý Kết` | vẫn 0 — xem mục 4 |

---

## 3. Nguyên liệu dung luyện ở bản Linux đến từ đâu

### 3.1 Thiết kế gốc (bản tiếng Trung, VNG đã chú thích tắt)
`server1\script\global\global_tiejiang.lua` dòng **190** và **198** còn nguyên 2 câu hướng dẫn gốc bị
chú thích (GBK). Dịch:

> **Văn Cương**: *Mặc Thạch Lão Nhân* — cửa hàng **Thần Cơ Trị** đổi được Văn Cương **phẩm chất 1** đủ loại.
> Qua **thuyền Phong Lăng Độ, 10 Hoàng Kim BOSS, Viêm Đế Bảo Tàng, Sát thủ cao cấp, Võ lâm liên đấu**
> nhận **Tinh Thiết Khoáng**, dùng ra Văn Cương **phẩm chất 2→4**.
>
> **Văn Tinh**: qua đúng những hoạt động trên nhận **Tinh Tinh Khoáng**, dùng (tiêu hao **1 Huyền Hỏa Than**)
> ra Văn Tinh **phẩm chất 1→6**.

### 3.2 Thực tế đang nối dây trong bản Linux
Mã vật phẩm Linux: Tinh Thiết Khoáng `{6,1,3810}` · Tinh Tinh Khoáng `{6,1,3811}` · Huyền Hỏa Than `{6,1,3507}`.

- **Tinh Thiết / Tinh Tinh Khoáng** nằm trong ~45 danh sách thưởng: `activitysys\config\` (1000, 1006, 1021,
  1023, 1024, 1029, 1034, 1035, 1039–1073, 17 = rương thủy tặc, 129 = theo dõi), Tống Kim mật bảo
  (`item\songjinmibao.lua`), rương tân thủ, boss Lý Bạch, Challenge of Time, mê cung, Thất Thành,
  Thiên Trì mật cảnh (+ boss tầng 4), Viêm Đế Bảo Tàng, sát thủ (`task\tollgate\killer\kill_level.lua`).
- **Long Môn Tiêu Cục** (`script\event\longmenbiaoju\award.lua`) cho **mảnh** khoáng `4535/4536`
  (10 mảnh → khoáng "không nguyên chất" `4537/4538` → khoáng cấp 1).
- **Huyền Hỏa Than `3507`: KHÔNG có nguồn nào trong toàn bộ bản dump** — chỉ bị tiêu hao ở
  `item_jingjingkuang.lua` (`ConsumeEquiproomItem(1,6,1,3507,-1)`). Bản gốc lấy từ shop không có trong dump.

### 3.3 Bản dự án đang có gì (mã JX1: `4428` / `4429` / `4125`)
**Đính chính `BANGIAO_DUNGLUYEN_0109.md` mục 6.3 ý 6** — 2 loại khoáng ĐÃ có nguồn từ trước (các đợt port
hoạt động 21–29/08), không phải "chưa có":

| Nguồn | Tệp | Tỷ lệ (Tinh Tinh / Tinh Thiết) |
|---|---|---|
| Viêm Đế Bảo Tàng | `missions\yandibaozang\head.lua:158` | 6,430 % / 3,210 % |
| Viêm Đế — Mật Bảo | `missions\yandibaozang\item\yandimibao.lua:68` | 1 % / 0,5 % |
| Vượt ải (mốc 30) | `missions\challengeoftime\chuangguang30.lua:40` | 20 % / 10 % |
| Vượt ải — rương | `missions\challengeoftime\item\chuangguanbaoxiang.lua:68` | 1 % / 0,5 % |
| Phong Lăng Độ — rương thủy tặc | `missions\fengling_ferry\hd3_baoruongthuytac.lua:64` | 1 % / 0,5 % |
| Sát thủ | `task\tollgate\killer\kill_level.lua:98` | 1,04 % / 0,52 % |
| Rương tân thủ | `item\xinshirenwu\xinshibaoxiang.lua:59` | 1 % / 0,5 % |

Trùng 4/5 nguồn chính thức của Linux; còn thiếu **10 Hoàng Kim BOSS** và **Võ Lâm Liên Đấu**.

**Huyền Hỏa Than `4125` vẫn KHÔNG có nguồn nào** (không có trong `goods.txt`/`buysell.txt`, không có trong
danh sách thưởng nào). Hệ quả: **không Tinh chế / thí luyện (gỡ Văn Cương) được** và **Tinh Tinh Khoáng
thành vô dụng** (bấm ra thông báo "Đại hiệp không có Huyền Hỏa Than").

**Chủ chốt: cho rớt ở các hoạt động boss — làm sau.** Chỗ đặt gọn nhất khi làm:
`missions\boss\bigboss.lua` (đã sẵn danh sách `tbProp`, thêm dòng `{tbProp = {6,1,4125,1,0,0}, nCount=1}`),
và/hoặc chèn cùng chỗ 2 loại khoáng trong 7 tệp bảng trên.

---

## 4. Việc còn hở — chờ chủ quyết

1. **Hoàng kim KHÔNG thuộc môn phái vẫn cap 0**: Hiệp Cốt, Nhu Tình, Thiên Hoàng Long, Kim Phong, Kim Quang,
   Hồng ảnh, Tuệ Thanh, Duyên Quân, Vinh Diệu, Tinh Chuẩn (dòng 169–216 + các biến thể).
   NPC hỗ trợ test (`npcthon\balanghuyen\hotrotest.lua` → `global\trangbihoangkim.lua`) đang phát
   Thiên Hoàng và Nhu Tình từ menu sống, nên **bộ đồ sẽ có món dung luyện được món không** — trong roledb
   đúng là món `Hiệp Cốt Tình ý Kết` bị lẻ ra. Muốn mở thêm: sửa hàm `classify` trong
   `ReverseTools\dungluyen\phanloai_hkmp.py` rồi chạy lại `va_goldequip_dungluyen.py --that`.
2. **Đồ sinh qua `AddGoldItem` (bảng `GoldItem.txt`) KHÔNG bao giờ dung luyện được**, dù goldequip đã có
   cột 58/59 — vì `sFUS_LaTrangBiVang` (KItem.cpp:535) đòi `nGoldId == 0`, mà `GoldItem.txt` là **bảng
   riêng 58 cột, thứ tự khác hẳn** (đối chiếu 899 mã: chỉ 148 khớp tên) nên không có dòng goldequip để tra.
   Các chỗ đang dùng đường này: `global\trangbihoangkim.lua` (43 lệnh — trong đó `hoangkimmonphai345()`
   phát đúng 31 bộ HKMP, **may là đang bị chú thích tắt khỏi menu**), `event\eventtet\npcsukien.lua` (13),
   `event\20thang10\EventLib2010.lua` (7), `event\eventnoel\` (4), `task\tollgate\messenger\posthouse.lua` (4)…
   Đường ĐÚNG (đã dùng ở nơi khác) là `AddItem2(2, 0, <nRow goldequip>, 0, 0, 0)`:
   `thoren.lua:464` ghép HKMP (`nhkmpID - 238` → nRow 0–139 = 31 bộ HKMP), `trangbihoangkim.lua`
   `chonanbangld`/`chondinhquocld`, `event\30thang4\`.
   → Nếu sau này bật lại các menu kia thì phải nắn `AddGoldItem(id)` sang `AddItem2(2,0,nRow)`, **và phải
   dò lại từng dòng bằng tên** (không có công thức lệch cố định), đồng thời chấp nhận chỉ số món có thể
   đổi vì hai bảng khác nhau.
3. **Lưu ý cân bằng**: theo mức chủ chốt thì HKMP thường (4 ô / phẩm 10) **mạnh hơn** bản `[Cực phẩm]` và
   `[Hoàn Mỹ]` của chính nó (3 ô / phẩm 9). Nếu là nhầm thì đổi 2 con số trong `phanloai_hkmp.py` rồi
   chạy lại là xong (nhớ xoá giá trị cũ hoặc phục hồi từ bản sao lưu trước).
4. `SyncItem` nay gửi thêm gói `s2c_syncfusion` (41 byte) cho **mọi** món hoàng kim có `cap > 0` — trước
   đây khối 2–2285 không tốn byte nào. Với 423 dòng mới thì lượng tăng không đáng kể, chỉ ghi lại để biết.

---

## 5. Kiểm thử sau khi restart

1. Vào NPC hỗ trợ test → An Bang / Định Quốc → xem tooltip có dòng *"Số lượng Văn Cương đã dung luyện 0 / 3"*.
2. Thợ Rèn → Trang bị dung luyện → Dung luyện Văn Cương → bỏ 1 món An Bang + 1 Văn Cương phẩm chất ≤ 8
   (loại khớp: nhẫn/dây chuyền/ngọc bội = cột 16–21 có 2/3/9 hoặc 1/4/8) → bấm Dung luyện (100 vạn).
3. Bỏ Văn Cương phẩm chất 9–10 vào món Định Quốc (trần 7) → phải báo mã 6 "phẩm chất vượt trần".
4. Món HKMP ghép ở Thợ Rèn (`ghephkmp`) → dung luyện được tới 4 ô, nhận Văn Cương tới phẩm chất 10.
5. Mặc/tháo → chỉ số đổi; thoát/vào lại → Văn Cương còn nguyên.
6. Món `Hiệp Cốt Tình ý Kết` → vẫn báo mã 4 "trang bị không dung luyện được" (đúng phạm vi đã chốt).
