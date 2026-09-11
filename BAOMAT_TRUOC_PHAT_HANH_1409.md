# Bảo mật trước khi phát hành — tổng hợp phát hiện (14/09)

Chủ yêu cầu: *"cách chống bug - phá - để chuẩn bị phát hành game cần làm gì"* và
*"note toàn bộ phát hiện lại cho tôi để làm trước khi phát hành"*.

**Cách đọc mức tin cậy.** Mục ghi **đã kiểm tận nơi** là tôi tự mở đúng dòng mã đó đọc bằng mắt.
Mục ghi **từ đợt rà** là kết quả lượt quét diện rộng, đáng tin nhưng nên xác minh lại trước khi bỏ công sửa.

Tài liệu chứa chi tiết khai thác được. Không đưa ra ngoài nhóm phát triển.

---

## Nhóm A — Chặn phát hành, phải xong trước ngày mở

### A1. Người chơi thường chạy được mã Lua tùy ý trên máy chủ

**Đã kiểm tận nơi.** Nặng nhất: mất máy chủ chứ không chỉ mất đồ.

- Cờ mở đường được bật vô điều kiện trong header dùng chung: `Sources/Core/Src/GameDataDef.h:27`.
- Hàm nhận chat `Sources/Core/Src/KPlayerChat.cpp:399` chỉ kiểm kênh, độ dài, cỡ gói, tốc độ gõ.
  Không có một dòng nào kiểm người gửi có phải GM.
- `Sources/Core/Src/KPlayerChat.cpp:467` đưa mọi câu chat mở đầu bằng tiền tố lệnh GM vào bộ xử lý.
- `Sources/Core/Src/KGMCommand.cpp:9` là bảng lệnh, gồm chạy Lua trực tiếp, chạy Lua phạm vi thế giới,
  chạy tệp script bất kỳ, nạp lại toàn bộ script.
- `Sources/Core/Src/KGMCommand.cpp:86` là nơi thực thi. Mọi nhánh chỉ kiểm độ dài tham số và
  "người này có đang đăng nhập hợp lệ không", không kiểm quyền.
- Lua phía máy chủ có sẵn hàm đuổi người chơi và khoá tài khoản: `Sources/Core/Src/ScriptFuns.cpp:9949`.


Kèm theo là lỗi tràn bộ đệm ngăn xếp: bộ đệm tên lệnh 20 byte, câu chat cho phép 256 byte
(`Sources/Core/Src/GameDataDef.h:30`), mã chép thẳng không cắt.

**Cần làm:** tắt cờ mở đường, hoặc thêm kiểm quyền thật và cắt bộ đệm tên lệnh.

### A2. Chèn SQL ở khâu kiểm tài khoản

**Đã kiểm tận nơi.** Sau A1 là lỗ đáng lo nhất.

- `Sources/Sword3PaySys/S3AccServer/S3PAccount.cpp:36`: câu truy vấn kiểm tài khoản ghép thẳng
  tên tài khoản và mật khẩu vào chuỗi SQL bằng sprintf.
- Ngay phía trên có đúng một dòng chặn cứng so tên tài khoản với một chuỗi tấn công cụ thể.
  Nghĩa là hệ thống từng bị đánh kiểu này và chỉ vá đúng chuỗi đó. Mọi biến thể khác vẫn lọt.
- Cùng kiểu ghép chuỗi còn ở `:234` (đăng nhập máy chủ) và `:512` (đổi mật khẩu).

**Khai thác:** gõ `' or '1'='1` vào ô tài khoản là đăng nhập thành người khác; nặng hơn là đọc / xoá cả bảng.

**Cần làm:** chuyển toàn bộ các truy vấn này sang tham số hoá, bỏ hẳn ghép chuỗi. Việc của vài hàm.

### A3. Mua từ sạp người chơi không kiểm chỉ số vật phẩm

**Đã kiểm tận nơi.**

- `Sources/Core/Src/KProtocolProcess.cpp:7576`: chỉ số vật phẩm do máy người chơi gửi lên được dùng
  thẳng để truy cập mảng vật phẩm toàn cục rồi nhân bản, không kiểm giới hạn.
- Cùng hàm không chặn tự mua sạp của chính mình (các hàm anh em đều có chặn: `:6903`, `:7350`, `:7548`).
- Người mua tự chọn ô đích (`m_Place/m_X/m_Y` vào thẳng `AddKIL`), vượt kiểm khi trang bị.
- Hàm tra giá `Sources/Core/Src/KItemList.cpp:6094` không kiểm món có đang ở phòng bày bán.

**Khai thác:** tự mua sạp của mình để nhân đôi đồ (chỉ mất thuế thành), hoặc mua món chưa từng bày,
hoặc nhét thẳng đồ vào ô trang bị.

**Cần làm:** kiểm giới hạn chỉ số, chặn tự mua, buộc món phải đang ở phòng bày bán, máy chủ tự chọn ô đích.
Mẫu đúng để nhân ra: đường rương đồ `Sources/Core/Src/KProtocolProcess.cpp:6215` kiểm cả sở hữu lẫn ô.

### A4. Đặt giá âm được chấp nhận

**Đã kiểm tận nơi.**

- `Sources/Core/Src/KItemList.cpp:6077` (`SetPrice`): trường giá là số có dấu, chỉ cần khác 0 là lưu.
  Không chặn số âm.
- Ở đường mua `KProtocolProcess.cpp:7574`, hai điều kiện `tiền < giá` và `giá == 0` đều lọt khi giá âm.

**Khai thác:** đặt giá -1, người mua lấy món gần như miễn phí (còn được cộng tiền do bước trả tiền âm).

**Cần làm:** chặn giá không dương ở cả đặt giá lẫn mọi đường mua bán.

### A5. Gửi tiền ngân hàng tràn dấu

**Đã kiểm tận nơi.**

- `Sources/Core/Src/KProtocolProcess.cpp:6800` (`StoreMoneyCommand`): số tiền là số không dấu bên máy
  người chơi, đưa thẳng vào `ExchangeMoney` (nhận số có dấu), không kẹp, không kiểm số dư.
- `Sources/Core/Src/KItemList.cpp:2524` (`ExchangeMoney`) chỉ dựa vào chặn duy nhất ở
  `KInventory::AddMoney` (`:230`): `if (m_nMoney + nMoney < 0) return FALSE`.

**Khai thác:** gửi một con số đủ lớn để hoá âm, phòng nguồn được cộng tiền thay vì trừ; bước trừ ở phòng
đích thất bại và bước hoàn tác cũng thất bại nhưng không ai kiểm giá trị trả về, nên tiền ở lại trong túi.

**Cần làm:** kẹp số tiền về khoảng hợp lệ và kiểm số dư nguồn trước khi chuyển; kiểm kết quả bước hoàn tác.

---

## Nhóm B — Gói cài và nền tảng, làm ngay sau nhóm A

### B1. Gói cài đang phát hành là bản gỡ rối, ký bằng chứng chỉ công khai

**Đã kiểm tận nơi** (trên bản 109111459 và cấu hình dựng).

- Bản đang phát hành bật cờ cho phép gắn trình gỡ rối. Ai cũng đọc / ghi bộ nhớ game trên máy mình
  mà không cần quyền quản trị — cách gian lận dễ nhất.
- Ký bằng chứng chỉ gỡ rối mặc định (CN=Android Debug), khoá này ai cũng có.
- `android/gradle-project/app/build.gradle`: nhánh release không có mục ký riêng, `minifyEnabled false`.

**Cần làm:** tạo khoá ký riêng giữ kín, tắt cờ gỡ rối cho bản phát hành, bật rút gọn mã.

### B2. Đường tự cập nhật có thể bị cướp

**Đã kiểm tận nơi.**

- `android/.../TaiDuLieuActivity.java`: tải qua HTTP thường; đối chiếu mã băm MD5 lấy từ chính phản
  hồi đó; rồi gọi trình cài đặt.
- `android/may_chu_tai_du_lieu.py`: máy chủ HTTP thường, không HTTPS, không xác thực.

**Khai thác:** kẻ cùng mạng thay cả tệp mô tả lẫn gói cài; vì gói giả ký được bằng khoá công khai (B1)
nên máy chấp nhận cài đè.

**Cần làm:** bọc HTTPS cho máy chủ tải; kiểm chữ ký gói cài, không chỉ kiểm MD5.

### B3. Đường truyền game không mã hoá

**Đã kiểm tận nơi.** Tìm trong cả tầng mạng client lẫn tầng chung máy chủ, không thấy mã hoá hay
xáo trộn gói tin. Khoá tầng truyền chỉ là `rand()`. Sửa gói tin bằng công cụ proxy là khả thi.

**Cần làm:** cân nhắc mã hoá kênh; ít nhất phải hoàn tất kiểm hợp lệ phía máy chủ (nhóm A và C) vì
kênh hở nghĩa là mọi gói tin đều có thể bị chế.

### B4. Bản điện thoại không có lớp chống gian lận, không giới hạn kết nối

**Đã kiểm tận nơi.**

- 16 tệp trong thư mục chống gian lận bị loại khỏi bản Android (`android/gen_lists.py`). Phần lớn dò
  cửa sổ Windows nên sang điện thoại vô dụng, chỉ mô-đun kiểm toàn vẹn bằng CRC là đáng port.
- Máy chủ không có cơ chế chặn lũ kết nối hay giới hạn theo địa chỉ, dễ nghẽn ngày mở.
- Dữ liệu game nằm ở bộ nhớ ngoài, tệp rời đè được lên gói nén, không kiểm toàn vẹn.

**Cần làm:** port mô-đun kiểm CRC và cho máy chủ đối chiếu kết quả; thêm giới hạn kết nối theo địa chỉ.

---

## Nhóm C — Cần xác minh thêm rồi mới sửa (từ đợt rà)

Các điểm này đáng tin nhưng tôi chưa tự đọc từng dòng. Xác minh trước khi bỏ công.

- **Bán cho cửa hàng NPC trả tiền trước khi xoá món** — `Sources/Core/Src/KBuySell.cpp:343`.
  Chỉ số món lấy từ tìm-theo-id, tra trượt trả về 0 tức trỏ vào ô 0. Có thể bán món không sở hữu.
- **Cược Bầu Cua không kiểm số dư** — `Sources/Core/Src/KProtocolProcess.cpp:7990`. Đặt cược số tuỳ ý
  không cần nạp tiền (nhánh nạp thì có kẹp, nhánh cược thì không).
- **Điểm kỹ năng nhận số âm** — `Sources/Core/Src/KPlayer.cpp:4649`. Gửi số điểm âm để cộng điểm.
- **Máy chủ trả về `Item[0]` khi tra trượt** ở nhiều chỗ: sửa đồ `KPlayer.cpp:10381`,
  xoá theo hạn `KItemList.cpp:2187`, đặt giá / khoá món `KItemList.cpp:6077/6110`.
- **Ném chiêu diện rộng xuống toạ độ bất kỳ** — `Sources/Core/Src/KProtocolProcess.cpp:6158`.
  Máy chủ không kiểm khoảng cách từ người ném tới điểm đánh.
- **Cỡ gói tin cho dư 4 byte** — `Sources/Core/Src/CoreServerShell.cpp:1263`. Gửi thiếu 4 byte thì
  trường cuối đọc từ bộ nhớ cũ. Vài handler cast thẳng không kiểm cỡ (danh sách trong đợt rà).
- **Chuỗi băm mật khẩu chính là mật khẩu** — client tự băm rồi gửi thẳng, không muối, không thách đố.
  Bắt được một lần là dùng lại mãi; lộ cơ sở dữ liệu là dùng được ngay.
  `Sources/S3Client/Ui/UiCase/UiLogin.cpp:411`, `Sources/S3Client/Login/Login.cpp`.
- **Cửa hàng đấu giá / thư qua kênh script** — `Sources/Core/Src/KAuctionServer.cpp:493` dựng lại
  vật phẩm từ chuỗi hex; script nào chuyển tiếp chuỗi của người chơi vào đây là máy sinh đồ.

---

## Nhóm D — Đã làm đúng, giữ nguyên (đối chiếu)

- **Rương đồ** kiểm cả sở hữu lẫn đúng ô: `Sources/Core/Src/KProtocolProcess.cpp:6215`. Mẫu nên nhân ra.
- **Giao dịch người với người** kẹp tiền và kiểm đối tác: `Sources/Core/Src/KPlayer.cpp:5796`, `:6455`.
- **Chiêu thức phần lớn do máy chủ quyết**: hồi chiêu, mục tiêu theo khoảng cách máy chủ, id chiêu có
  chặn giới hạn — `Sources/Core/Src/KProtocolProcess.cpp:6102`.
- **Tốc độ di chuyển do máy chủ giữ**, không có hack tốc độ do client (nhưng thiếu chặn dịch chuyển tức thời).
- **Điểm nhận nhật ký** lọc tên tệp đúng, không leo thư mục; nhật ký gửi về không chứa tài khoản / mật mã.
- **Bộ phân tích thẻ màu trong chat** đã được chặn tràn từ trước.

---

## Thứ tự làm

1. A1 (lệnh GM) và A2 (chèn SQL) — hai lỗ mất máy chủ, làm trước hết.
2. A3, A4, A5 — nhóm mất đồ và tiền.
3. Xác minh và sửa nhóm C.
4. B1 (bản phát hành đúng nghĩa) — bắt buộc trước khi đưa lên cửa hàng.
5. B2, B3, B4 — cứng hoá đường cập nhật, kênh truyền, chống gian lận.

Ước lượng: nhóm A khoảng một tới hai ngày làm tập trung, vì hầu hết là thêm kiểm tra, không đổi cấu trúc.

---

## Đã sửa trong phiên này (14/09)

- **Thiếu biểu tượng Nộ / Âm Luật ở mini skill.** Gốc: gói `sprvuhontieudao3.pak` (11 mục, 8 icon
  trạng thái Vũ Hồn / Tiêu Dao) tạo 02/09 lúc 11:23, sau khi cây dữ liệu điện thoại đã sao chép xong
  lúc 06:58, nên không vào `mobile_NN.pak` và không có trong `package.ini`. Đã chép gói 18 KB vào
  `data/` của cả hai cây, nối một dòng vào `package.ini`, sinh lại manifest bằng `--chi-manifest`
  (không restart máy chủ 8765). Máy ảo kiểm chứng: log `PakList Open ... sprvuhontieudao3.pak ... Ok`,
  số gói mở 33 -> 34. Chỉ sửa dữ liệu, không đụng mã, không đụng bản PC. Chi tiết ở
  `BANGIAO_GIAODIEN_MOBILE_SUAGD_1309.md` nếu cần dò lại.
