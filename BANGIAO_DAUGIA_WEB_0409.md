# GIAO KÈO — ĐẤU GIÁ THẾ GIỚI cấu hình từ WEB ADMIN (04/09/2026)

Hai bên: **web admin** (phiên `webver5-eb`, model `loi/DauGia.php`) và **máy chủ game** (phiên `wauto-e7`, C++ `KAuctionServer.cpp` + Lua `auction_manager.lua`). Tài liệu này là nguồn duy nhất về tên bảng / tên cột / ý nghĩa. Đổi gì phải sửa ở đây trước.

## 1. Cách chạy (luật chủ đã chốt 04/09)

1. Admin cấu hình một **NHÓM vật phẩm** kèm giá (bảng `auction_web_pool`) và **LỊCH** (bảng `auction_web_cfg`).
2. Máy chủ quét mỗi 30 giây. Tới giờ `next_round` thì **mở đợt mới**: bốc ngẫu nhiên `items_per_round` món trong nhóm (theo trọng số, không trùng trong một đợt), **dựng vật phẩm thật** (trang bị được bốc thuộc tính ngẫu nhiên y như phần thưởng thư, mỗi đợt một bộ khác nhau), rồi chèn vào `auction_item` với `atype = 2` (Thế giới), tên phiên `Đợt N HH:MM dd/mm` (N = số thứ tự đợt `round_no`), người bán `@WEB`.
3. Đợt kéo dài đúng một chu kỳ (`period_min`, mặc định 180 phút). Trong đợt: khởi điểm = `start_price`, mỗi lượt trả thêm 10 % khởi điểm, người đang giữ giá cao nhất không tự nâng được, ai trả bằng/vượt `buy_price` (nếu có) là chốt ngay.
4. Hết đợt: có người trả → **món gửi về thư người thắng** (đường `aucitem:<id>` có sẵn), tiền vào hệ thống; không ai trả → món thu hồi (không mất gì, vì món sinh từ cấu hình). Đợt kế mở ngay sau đó.
5. Web **không bao giờ ghi** `auction_item`. Web chỉ đọc nó để hiện "đang lên sàn / lịch sử".
6. Máy chủ **tự tạo hai bảng mới** (`CREATE TABLE IF NOT EXISTS`) đúng khuôn bảng `mail`. Web không CREATE / ALTER. Chưa thấy bảng = máy chủ chưa cập nhật bản mới → web hiện "máy chủ chưa hỗ trợ".

## 2. Bảng `auction_web_pool` — NHÓM VẬT PHẨM (web sở hữu)

```sql
CREATE TABLE IF NOT EXISTS auction_web_pool (
  id           INT AUTO_INCREMENT PRIMARY KEY,
  award        VARCHAR(255)   NOT NULL,             -- WEB: MỘT mục item:/gold: đúng cú pháp thư (mục 4)
  label        VARBINARY(128) NOT NULL DEFAULT '',  -- WEB tự dùng để hiển thị (mã gì tuỳ web). MÁY CHỦ KHÔNG ĐỌC.
  currency     TINYINT        NOT NULL DEFAULT 1,   -- WEB: 1 = Ngân lượng, 2 = Xu
  start_price  BIGINT         NOT NULL DEFAULT 0,   -- WEB: giá KHỞI ĐIỂM (giá cơ bản), 1 .. 2000000000
  buy_price    BIGINT         NOT NULL DEFAULT 0,   -- WEB: giá MUA NGAY; 0 = không có nút Mua ngay; nếu > 0 phải > start_price
  weight       INT            NOT NULL DEFAULT 1,   -- WEB: trọng số bốc, 1 .. 1000 (2 = gấp đôi cơ hội); máy chủ kẹp: ≤ 0 coi như 1, > 1000 coi như 1000
  enabled      TINYINT        NOT NULL DEFAULT 1,   -- WEB: 1 tham gia bốc, 0 tạm tắt
  drawn_count  INT            NOT NULL DEFAULT 0,   -- MÁY CHỦ: số lần đã lên sàn
  drawn_time   INT            NOT NULL DEFAULT 0,   -- MÁY CHỦ: lần cuối lên sàn (unix)
  drawn_auc    INT            NOT NULL DEFAULT 0,   -- MÁY CHỦ: id dòng auction_item của lần cuối
  item_name    VARBINARY(64)  NOT NULL DEFAULT '',  -- MÁY CHỦ: tên thật của món sau lần dựng đầu (byte TCVN3)
  err          VARBINARY(191) NOT NULL DEFAULT '',  -- MÁY CHỦ: lỗi dựng gần nhất, ASCII không dấu; '' = ổn
  ctime        INT            NOT NULL DEFAULT 0,   -- WEB: unix lúc tạo
  mtime        INT            NOT NULL DEFAULT 0,   -- WEB: unix lúc sửa
  KEY idx_enabled (enabled)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
```

Luật:
- Web INSERT / UPDATE / DELETE tự do các cột **WEB**. Xoá dòng đang có món trên sàn không sao (dòng `auction_item` độc lập, đã chứa `item_rec`).
- Web **không ghi** 5 cột MÁY CHỦ (`drawn_*`, `item_name`, `err`). Máy chủ ghi chúng bằng `UPDATE ... WHERE id = ?`, không đụng cột khác.
- Máy chủ đọc nhóm **lúc mở đợt** (không cache). Sửa xong là đợt kế có hiệu lực. Mỗi đợt đọc **tối đa 2.000 nhóm** đang bật (theo `id` tăng dần); chạm trần thì `last_msg` có chữ CANH BAO — web nên chặn khi số nhóm bật vượt 2.000.
- Một dòng lỗi (`err <> ''`) **vẫn được bốc lại** ở đợt sau (admin sửa `award` thì tự hết lỗi). Muốn loại hẳn thì đặt `enabled = 0`.

## 3. Bảng `auction_web_cfg` — LỊCH (một dòng, `id = 1`)

```sql
CREATE TABLE IF NOT EXISTS auction_web_cfg (
  id               INT            PRIMARY KEY,            -- luôn = 1 (máy chủ tự INSERT dòng này khi tạo bảng)
  enabled          TINYINT        NOT NULL DEFAULT 0,     -- WEB: 1 = chạy tự động, 0 = dừng (đợt đang chạy vẫn kết thúc bình thường)
  period_min       INT            NOT NULL DEFAULT 180,   -- WEB: chu kỳ mỗi đợt, phút, 10 .. 1440
  items_per_round  INT            NOT NULL DEFAULT 3,     -- WEB: số món mỗi đợt, 1 .. 30
  next_round       INT            NOT NULL DEFAULT 0,     -- MÁY CHỦ đặt = giờ mở đợt kế. WEB chỉ được đặt = 0 ("mở đợt ngay")
  last_round       INT            NOT NULL DEFAULT 0,     -- MÁY CHỦ: giờ mở đợt gần nhất (unix)
  round_no         INT            NOT NULL DEFAULT 0,     -- MÁY CHỦ: số đợt đã mở kể từ đầu
  last_msg         VARBINARY(191) NOT NULL DEFAULT '',    -- MÁY CHỦ: kết quả đợt gần nhất, ASCII không dấu
  mtime            INT            NOT NULL DEFAULT 0      -- ai ghi cũng cập nhật
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
```

Luật:
- "**Mở đợt ngay**" = web chạy `UPDATE auction_web_cfg SET next_round = 0, mtime = ? WHERE id = 1 AND next_round <> 0`. Máy chủ thấy `next_round <= now` ở nhịp kế (≤ 30 s) là mở. **Đợt cũ không bị cắt**: món cũ chạy tới hết giờ của nó, nên trong lúc đó tab Thế giới có hai đợt. Lịch tính lại từ lúc mở: `next_round = now + period_min × 60`.
- `enabled = 0` chỉ ngăn mở đợt mới; không đụng món đang đấu.
- Máy chủ giành đợt bằng một câu nguyên tử `UPDATE ... SET next_round = ?, last_round = ?, round_no = round_no + 1 WHERE id = 1 AND enabled = 1 AND next_round <= ?` và chỉ mở khi `affected_rows = 1` (an toàn nếu sau này chạy hai GameServer).
- `next_round` mặc định 0 nên **lần đầu bật `enabled = 1` là mở đợt ngay** — nhưng **nhóm rỗng thì không mở** (không đốt chu kỳ; `last_msg` = `nhom rong: chua mo dot...`), nhịp nào có món đang bật là mở. Nếu mở đợt mà **không món nào dựng được** (toàn lỗi), máy chủ tự hẹn lại `next_round = now + 60` và `last_msg` nói rõ; sửa `err` xong là ≤ 60 giây sau mở lại.

## 4. Cột `award` — cú pháp (dùng lại đúng ngữ pháp thư, thêm luật đấu giá)

Chỉ **MỘT** mục, không dấu `;`. Hai tiền tố được nhận:

| Tiền tố | Cú pháp | Nghĩa cho đấu giá |
|---|---|---|
| `item:` | `item:genre,detail,particular,level,series,luck,n[,lock][,expSec][,magic][,stack]` | 6 số đầu bắt buộc (như thư). `n` = số món **trong một chồng**, chỉ với món có **cờ xếp chồng** trong bảng vật phẩm (thuốc, tinh thạch, vật phẩm nhiệm vụ / bí kíp có MaxStack > 0); trần chồng theo bảng, không có thì 500. Trang bị (0) và **văn cương (8)** luôn 1 — ghi `n > 1` cho món không chồng được thì máy chủ **từ chối** (`err` = `mon khong xep chong, n phai = 1` / `van cuong khong xep chong, n phai = 1`). `magic` giữ nghĩa như thư (tham số 7 của AddItem). `stack` bị bỏ qua. Trang bị detail 13-16 (ấn, trang sức, mũ trùm, áo choàng) dựng được. |
| `gold:` | `gold:record,n[,lock][,expSec]` | Hoàng kim theo dòng `goldequip.txt` (như thư). `n` luôn 1. |

Luật riêng đấu giá (web kiểm trước khi lưu, máy chủ kiểm lại và ghi `err` nếu lọt):
- `level`: trang bị thường (detail ≤ ngựa) phải **1..10**, thuốc phải **1..5** — level 0 hay quá trần sẽ dựng ra **món khác** nên máy chủ từ chối (`err` = `level trang bi phai 1..10` / `level thuoc phai 1..5`).
- `genre` chỉ trong **0, 1, 4, 5, 6, 8, 9** (đúng danh sách `Thu::locChuoiQua` đã chặn 2/3; máy chủ còn chặn 7 = trang bị hỏng).
- `lock` và `expSec` **phải bằng 0** — món đấu giá không được khoá, không được có hạn (cùng luật ký gửi trong game: "có hạn sử dụng không thể ký gửi").
- Không nhận `money:` `xu:` `exp:` `repute:` `task:` `aucitem:`.
- Dài ≤ 255 byte, ASCII thuần.
- Web nên dùng lại bộ tra `data/tim_vatpham.php` để chọn món; máy chủ sẽ tự lấy tên thật ghi vào `item_name` sau lần dựng đầu.

Ví dụ: `item:1,1,5,1,0,0,50` (50 Đại hoàn đan trong một chồng) · `item:0,1,182,3,2,10,1,0,0,20` (trang bị luck 10, magic 20, thuộc tính bốc ngẫu nhiên mỗi đợt) · `gold:1234,1`.

## 5. Máy chủ kiểm gì trước khi đưa lên sàn (ghi vào `err`, bỏ qua món, bốc món khác)

**Web hiện `err` NGUYÊN VĂN** (ASCII không dấu) và tô đỏ khi khác rỗng — đừng tra bảng, vì danh sách còn dài thêm khi máy chủ sửa. Các chuỗi hiện có:

| `err` | Nguyên nhân |
|---|---|
| `award: thieu/sai cu phap` · `award: chi mot muc, khong dau ;` · `award: chi nhan item: hoac gold:` · `award: so khong hop le` · `award: ky tu la` · `award: so am hoac qua lon` · `award: item can 6 so` · `award: gold can so dong goldequip` | chuỗi award sai |
| `genre X khong ho tro` | 2, 3, 7 hoặc số lạ |
| `lock/expSec phai = 0` | |
| `so luong qua lon` · `so luong toi da N mot chong` · `mon khong xep chong, n phai = 1` · `van cuong khong xep chong, n phai = 1` | `n` > 9999, vượt trần chồng, hoặc `n > 1` cho món không chồng được |
| `dung vat pham that bai (detail/particular/dong sai)` | bộ sinh trả rỗng |
| `het khe vat pham` · `khong doi duoc sang rec` · `rec khong doc lai duoc` · `dung lai lan hai that bai` · `dung lai lan hai ra mon khac` | lỗi nội bộ khi dựng/kiểm lại — báo máy chủ |
| `currency 1 hoac 2` · `gia khoi diem 1..2000000000` · `gia mua ngay phai > khoi diem (va <= 2000000000)` | giá/tiền sai |
| `khong ghi duoc auction_item` | MySQL lỗi lúc chèn — đợt sau thử lại |

Trần số: giá ≤ 2 000 000 000 (client đấu bằng số nguyên 32 bit); `period_min` kẹp 10..1440; `items_per_round` kẹp 1..30; `weight` kẹp 1..1000. Nếu nhóm hợp lệ ít hơn `items_per_round` thì mở bấy nhiêu món, ghi `last_msg`.

## 6. Web đọc "đang lên sàn / lịch sử" từ `auction_item` (chỉ đọc)

```sql
SELECT id, activity, item_name, currency, guaranteed_price AS start_price, base_price, cur_price,
       buyer, buy_price, state, start_time, end_time
FROM auction_item
WHERE atype = 2 AND seller = '@WEB'
ORDER BY id DESC LIMIT 100;
```

| Cột | Nghĩa |
|---|---|
| `activity` | tên đợt, **byte TCVN3** (`Đợt 15:00 04/09`) |
| `item_name` | tên món, **byte TCVN3** |
| `guaranteed_price` | giá khởi điểm |
| `base_price` | giá mua ngay; = khởi điểm nghĩa là **không có** Mua ngay |
| `cur_price` | giá cao nhất hiện tại, 0 = chưa ai trả (khi `state = 0`, `buy_price` chỉ là bản sao giá đang trả — **đừng** dùng nó làm dấu "đã chốt") |
| `buyer` | người đang giữ giá / người thắng |
| `buy_price` | giá chốt khi đã bán (`state` 1/3, `buyer <> ''`) — luôn bằng giá người thắng trả, kể cả khi hết giờ |
| `state` | 0 đang đấu · 1 đã bán, đang giao thư · 3 kết thúc: `buyer <> ''` = đã bán và giao xong, `buyer = ''` = hết giờ không ai mua, đã thu hồi. (Dòng `@WEB` không bao giờ ở `state = 2` — giá trị đó chỉ dùng cho ký gửi cá nhân trả hàng về người bán; và không rút được nên không có trường hợp "đã rút".) |
| `end_time` | giờ kết thúc (unix); có người trả trong 60 giây cuối thì tự gia hạn thêm 60 giây |

Dòng `auction_item` **không bao giờ bị xoá** (thư `aucitem:<id>` trỏ vào nó), nên lịch sử là vĩnh viễn.

## 7. Gợi ý bố cục trang admin "Đấu giá thế giới" (một trang, ba khối từ trên xuống)

**Khối 1 — Lịch tự động** (đọc/ghi `auction_web_cfg`)
- Công tắc **Bật/Tắt**; ô "Mỗi ___ phút thay món" (mặc định 180, gợi ý nhanh: 60 / 180 / 360 / 720); ô "Số món mỗi đợt" (mặc định 3).
- Dòng trạng thái: "Đợt hiện tại mở lúc … · kết thúc lúc … · đã mở N đợt · máy chủ: <last_msg>". Nếu `next_round` đã qua > 2 phút mà không đổi → cảnh báo "máy chủ chưa quét (chưa cập nhật bản mới hoặc đang tắt)".
- Nút **"Mở đợt mới ngay"** kèm chú thích: "Đợt đang chạy vẫn kết thúc đúng giờ; đợt mới chạy song song."

**Khối 2 — Nhóm vật phẩm** (bảng `auction_web_pool`)
- Cột: Hình · Tên (`label`) · Tiền · Khởi điểm · Mua ngay · Trọng số · Bật · Đã lên sàn (lần) · Lần cuối · Lỗi (`err`, tô đỏ nếu khác rỗng) · Sửa · Xoá.
- Form thêm/sửa: ô chọn món **dùng lại ô "gõ tên → hiện hình" của thư**; số lượng trong chồng (chỉ hiện với loại chồng được); chọn tiền (Ngân lượng / Xu); Khởi điểm; Mua ngay (để trống = không có); Trọng số (mặc định 1, ẩn dưới "Nâng cao"); Bật.
- Hiện giá theo cách game: Ngân lượng ≥ 1 vạn thì "120 vạn", Xu thì "100 xu" (không dùng "ức").
- Kiểm ngay trên form: khởi điểm ≥ 1; mua ngay trống hoặc > khởi điểm; ≤ 2 tỷ; genre hợp lệ; không khoá/không hạn.

**Khối 3 — Đang lên sàn & lịch sử** (đọc `auction_item`, mục 6)
- Hai thẻ: "Đang đấu" (`state = 0`) và "Lịch sử" (còn lại). Cột: Đợt · Món · Khởi điểm · Giá hiện tại/chốt · Người giữ giá/thắng · Trạng thái · Kết thúc.
- Tên đợt và tên món phải giải mã TCVN3 → UTF-8 trước khi hiện (như tiêu đề thư).

## 8. Phía máy chủ — TRẠNG THÁI 04/09 14:39: XONG, CHỜ SWAP

- `bin\server\CoreServer.dll.moi` = **8181a940** (gốc: origin/main affbc060 + origin/chienlenh-0409 2ef04da2 + mail-0309 cf84c24e; đủ dấu của Chiến Lệnh, đấu giá web và các việc chung). Script `auction_manager.lua` trên cây chạy thật đã là bản mới (28e49e48). **Chủ chạy `ChayGameServer.bat`** là đủ: DLL + script có hiệu lực cùng lúc, hai bảng tự tạo trong ≤ 30 giây sau khi máy chủ lên.
- 5 commit (`7002737e` → `cf84c24e`) đã cherry-pick lên `origin/main` (`ab9cefaf`). Bộ giả lập Lua (`lua4.exe` + `test_aucweb.lua`, 9 kịch bản) và phản biện 4 lát cắt (43 tác tử) đã chạy; mọi phát hiện được giữ đã sửa (W3 → W8).
- Web (`webver5-eb`) đã xong `loi/DauGia.php` + `giaodien/admin_daugia.php`, chờ swap để bật thử.

### 8a. Danh sách việc đã làm (giữ để đối chiếu)

1. C++ `KAuctionServer.cpp`: tạo 2 bảng + dòng cfg; `AUCWEB_Cfg / AUCWEB_ClaimRound / AUCWEB_Pool / AUCWEB_Drawn / AUCWEB_Err / AUCWEB_Msg`; `AUC_MakeRec(szAward)` dựng vật phẩm thật từ chuỗi award (không cần người chơi), tự kiểm dựng lại được rồi mới trả `item_rec`.
2. Lua `auction_manager.lua` (bộ sinh `p12_daugia.py`): vòng quét riêng `AucWeb_Tick` 30 giây, bọc `call(..., "x")` để một dòng lỗi không giết vòng quét; bốc theo trọng số; chèn qua `AUC_PutOn` có sẵn; báo người đang mở cửa sổ một lần cuối lô; chặn cứng rút món / hoàn tiền với dòng `seller = '@WEB'`.
3. Cần dựng lại `CoreServer.dll` (không đụng client, không đổi gói tin, không đổi `auction_item`).

Điểm chờ chủ chốt (có thể đổi sau, không ảnh hưởng tên cột): (a) "Mở đợt ngay" có cắt sớm đợt cũ không — mặc định KHÔNG; (b) chu kỳ mặc định 180 phút; (c) có cho đặt giá Mua ngay không — mặc định TUỲ TỪNG MÓN (`buy_price = 0` là không).

## 9. Thông báo vào KÊNH CHAT (04/09 chiều, `[DAUGIA-CHAT]`, phiên `wauto-4a`)

Chủ: *"viết thêm thông báo mỗi khi thay đổi đấu giá trên thế giới vào kênh chat - đấu giá bang vào kênh chat bang; gôm thông tin để người chơi biết"*. **Web không đổi gì** (không thêm bảng/cột; giao kèo mục 2-6 giữ nguyên). Máy chủ tự phát:

| Sự kiện | Thế giới (dòng "Hệ Thống", tới mọi người) | Bang (dòng "Tin bang", chỉ thành viên bang, vào thẻ **bang** của khung chat) |
|---|---|---|
| Đợt web mở | `[Đấu giá thế giới] Đợt N HH:MM dd/mm mở: 3 món mới, còn 3 giờ (kết thúc 18:00). Ai trả cao nhất khi hết giờ được nhận qua thư. Mở cửa sổ Đấu giá, thẻ Thế giới để trả giá.` rồi mỗi món một dòng `1. <tên>: khởi điểm X, mua ngay Y` | — |
| GM / bang chủ đưa món lên sàn | `<tên người> đưa <món> lên sàn (<phiên>): khởi điểm X, mua ngay Y, còn 30 phút (kết thúc HH:MM). Mở cửa sổ Đấu giá, thẻ ... để trả giá.` (Hà Lan: `giá mở X, hạ dần tới Y`) | cùng mẫu, tiền tố `[Đấu giá bang]` |
| Trả giá | `<ai> trả X cho <món> - muốn vượt phải trả từ X+bước, mua ngay Y; còn N phút.` | cùng mẫu |
| Mua ngay | `<ai> mua ngay <món> với giá X, vật phẩm gửi qua thư.` | + ` Quỹ bang nhận <sau thuế>.` (Xu: ` Người bán nhận ...`) |
| Hết giờ có người thắng | `Hết giờ: <ai> thắng <món> với giá X, vật phẩm gửi qua thư.` | + quỹ bang như trên |
| Hết giờ ế | `Hết giờ: <món> không ai trả giá, đã thu hồi.` | `... trả về người bán qua thư.` |
| Rút món / hạ giá Hà Lan | `<món> đã được rút khỏi sàn (chưa ai trả giá).` / `<món> hạ giá còn X (còn N lần hạ, thấp nhất Y), còn N phút.` | cùng mẫu |

Ký gửi cá nhân **không** phát. Mỗi dòng ≤ 200 byte (gói chat mang độ dài 1 byte, máy chủ kẹp 256 → quá là mất cả dòng), dài hơn bị cắt `...`.

Điều kiện: script nạp lúc boot → **khởi động lại máy chủ** là có phần Thế giới + phần Bang dạng "Tin bang" (DLL cũ: dòng vào khung chính qua `Msg2Tong`, vẫn chỉ tới thành viên). Muốn dòng bang vào **đúng thẻ bang** cần `CoreServer.dll` có `AUC_MsgTong` (**f576ac9e**, đặt cạnh khe `bin\server\CoreServer.dll.moi.chat_f576ac9e` vì khe đang có bản `b67f002f` của phiên khác). Mã: `mail-0309` c8264cf5 → `origin/main` 4f7f2e2e. Chi tiết kỹ thuật: `BANGIAO_DAUGIA_WEB_THICONG_0409.md` mục 15.
