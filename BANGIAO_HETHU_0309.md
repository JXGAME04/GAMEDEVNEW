# BÀN GIAO HỆ THƯ (viết lại theo client VLTK 2.0) — 03/09/2026

Chủ chốt: lưu thư bằng **MySQL**; kênh **ScriptProtocol thật** (dùng nguyên script 2.0); đính kèm **item / Ngân lượng / xu / EXP**;
thư gửi từ **web admin, hoạt động, thưởng top tuần-tháng, sự kiện đua top**; nội dung như ảnh mẫu 2.0; **bỏ** mở URL.

Tài liệu mổ client 2.0: `ReverseTools\pak_vltk\vltk2\README_MAIL.md`. Công cụ sinh/vá: `ReverseTools\mail\p1_*.py p2_*.py p3_*.py`.
Gương script cây chạy thật: `serverscript_jx2\mail\{server,client}\`.

## 1. Ba đợt — binary chờ swap (ba tệp client + server phải lên CÙNG LÚC: 2 gói mới nối cuối enum, GOI/GDCNI cuối enum)

| Đợt | Nội dung | Commit |
|---|---|---|
| 1 | Kênh ScriptProtocol (ObjBuffer) thật: `Core\Src\KScriptProtocol.{h,cpp}`, gói `s2c_scriptdata`/`c2s_scriptdata`, ObjBuffer cho client, `protocol.lua` + 12 tên MAIL, `protocol_def_c.lua`, ECHO thử | 258f0948 |
| 2 | Cửa sổ thư client: `S3Client\Ui\UiCase\UiMail.{h,cpp}`, `Core\Src\KMailClient.{h,cpp}`, `KMailUiDef.h`, `script\ui\uimail.lua` (bản 2.0), `script\mail\maildef.lua`, 6 ini `ui\Ui3\mail\` | 2b1aff9e |
| 3 | Server: `Core\Src\KMailServer.{h,cpp}` (bảng MySQL `mail`), `script\mail\mailmanager.lua`, `mailpoll.lua`, 6 handler trong `protocol_def_gs.lua`, móc `playerlogin.lua`, NPC Tín Sứ `dichquan.lua` mục "Nhận thư", lệnh bài admin 2 mục gửi thư thử | c72aaa8c (main 5c8b8736) |

**Bộ `.moi` cuối (16:50, sau khi wauto-6a gộp S13i/j — cả ba build từ ≥ 5c8b8736, chứa đủ hệ thư):**

| Tệp | md5 | Kích thước |
|---|---|---|
| `bin\server\CoreServer.dll.moi` | bad8e293 | 18.298.368 |
| `bin\client\CoreClient.dll.moi` | f2ad5ca3 | 2.507.776 |
| `bin\client\Game.exe.moi` | 24762253 | 1.399.808 |

(Bản build thuần đợt 3 của nhánh mail-0309: cca51fdf / e4ac910a / 24762253 — đã bị hai tệp trên thay bằng superset.)

Swap: thoát game, tắt GameServer, chạy `ChayGameServer.bat` + `ChoiGame.bat` (đổi `CoreServer.dll.moi`, `CoreClient.dll.moi`, `Game.exe.moi`).
Script đã nằm sẵn ở cây chạy thật, nạp khi khởi động lại.

## 2. Bảng MySQL `mail` (tự tạo khi máy chủ dùng lần đầu; DB = mục `[gamedb]` của `DataBase.ini`)

```sql
CREATE TABLE IF NOT EXISTS mail (
  id INT AUTO_INCREMENT PRIMARY KEY,
  role_name VARBINARY(32) NOT NULL,          -- tên nhân vật, byte TCVN3 thô (đúng như trong game)
  sender VARBINARY(64) NOT NULL DEFAULT '',  -- "Nhà phát hành", "Thư hệ thống", tên hoạt động... (TCVN3)
  title VARBINARY(128) NOT NULL DEFAULT '',
  content BLOB,                              -- tối đa 2000 byte hiển thị; xuống dòng = <enter>
  award VARCHAR(512) NOT NULL DEFAULT '',    -- xem mục 3
  award_count INT NOT NULL DEFAULT 0,        -- số mục đính kèm (0 = không có)
  state TINYINT NOT NULL DEFAULT 0,          -- 0 chưa giao, 1 chưa đọc, 2 đã đọc, 3 đã nhận đính kèm, 4 đã xoá
  send_time INT NOT NULL DEFAULT 0,          -- unix
  expire_time INT NOT NULL DEFAULT 0,        -- unix; 0 = không hết hạn. Mặc định 30 ngày
  source VARBINARY(32) NOT NULL DEFAULT '',  -- 'web', 'event', 'top', 'gm', 'script'
  KEY idx_role_state (role_name, state),
  KEY idx_state_id (state, id)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
```

**Web admin gửi thư = INSERT một dòng `state = 0`**, ví dụ (chuỗi Việt phải là byte TCVN3 như game; nếu web lưu UTF-8 thì
chuyển sang TCVN3 trước khi INSERT — dùng bảng mã của `vn_to_octal.py`):

```sql
INSERT INTO mail (role_name, sender, title, content, award, award_count, state, send_time, expire_time, source)
VALUES (?, ?, ?, ?, 'item:6,1,4139,0,0,0,1;money:10000', 2, 0, UNIX_TIMESTAMP(), UNIX_TIMESTAMP()+30*86400, 'web');
```

Máy chủ dò bảng **mỗi 30 giây** (`mailpoll.lua`): người nhận đang online → báo thư mới ngay (bồ câu nhấp nháy + header);
đang offline → giao lúc đăng nhập (`playerlogin.lua` → `MailManager_OnLogin`). Thư hết hạn được quét mỗi 10 phút và khi mở hộp thư.
Quá 100 thư/người: thư cũ nhất bị đánh dấu xoá (như 2.0: OVERFLOW).

## 3. Định dạng đính kèm (cột `award`)

Nhiều mục cách nhau bằng `;`:

| Mục | Ý nghĩa | Khi nhận |
|---|---|---|
| `item:genre,detail,particular,level,series,luck,count` | vật phẩm theo thuộc tính JX1 (như `AddItem`) | `AddItem` × count (kiểm chỗ trống túi trước: cần ≥ 6 ô/1 vật phẩm) |
| `money:N` | Ngân lượng | `Earn(N)` |
| `xu:N` | xu (ô nhiệm vụ 251 như `petsys\jx1_compat.lua`) | `SetTask(251, +N)` |
| `exp:N` | kinh nghiệm | `AddOwnExp(N)` |

`award_count` = số mục. Client hiện vật phẩm bằng vật phẩm tạm (đường `GDI_ITEM_CHAT` như hộp xúc xắc): đúng biểu tượng + chú giải;
tiền/xu/EXP hiện biểu tượng `银两图标.spr` / `jinding.spr` / `exp.spr` kèm số.
Nhận đính kèm là **nguyên tử** (`UPDATE ... WHERE state < 3`): không nhận hai lần dù bấm nhiều lần / nhiều GameServer.

## 4. API cho script (hoạt động, top tuần-tháng, đua top)

```lua
Include("\\script\\mail\\mailmanager.lua")
-- MailManager_SendMail(tên nhân vật, người gửi, tiêu đề, nội dung, đính kèm, số ngày hết hạn, nguồn) -> id thư (0 = lỗi)
MailManager_SendMail("TenNhanVat", MAILMGR_SENDER_NPH, "Trao Thuong Dua Top", "Dai hiep than men,<enter>...<enter>Tran trong",
    "item:6,1,4139,0,0,0,1;money:10000;xu:10;exp:50000", 30, "top")
```
Người nhận đang online được báo ngay; offline nhận lúc đăng nhập. Chuỗi Việt trong .lua = TCVN3 thô (RULE 0 của skill).

## 5. Thử nghiệm

1. Lệnh bài admin → "Thu kenh ScriptProtocol (ECHO)": khung thoại `[ECHO] xin chao tu may chu` + chat `May chu da nhan ECHO: ...`.
2. Lệnh bài admin → "Gửi thư thử: tiền/xu/exp" → chat "Đã gửi thư thử (id N)"; bồ câu góc phải nhấp nháy.
3. Đến **Tín Sứ** (Quan Dịch Trạm) bất kỳ thành nào → "Nhận thư" → cửa sổ Hộp thư: hàng thư "Nhà phát hành / Thư thử hệ thống thư / 30 ngày" (kẹp đính kèm).
4. Bấm hàng → chi tiết (người gửi, tiêu đề, nội dung 3 dòng, 3 ô thưởng) → "Nhận" → chat "Đã nhận đính kèm trong thư." + Ngân lượng/xu/exp tăng; bấm Nhận lần 2 → "Đính kèm đã được nhận rồi!".
5. "Gửi thư thử: có vật phẩm" → ô vật phẩm (Lệnh bài Bắc Đẩu 6,1,4139) có chú giải khi rê chuột → Nhận → vật phẩm vào túi.
6. Xoá thư (có xác nhận khi còn đính kèm) · Chọn hết + Xóa toàn bộ · Lọc (Toàn bộ / chưa đọc / có đính kèm / hệ thống) · Tự động xóa thư trống (đọc thư không đính kèm → tự xoá).
7. Web: INSERT một dòng như mục 2 cho nhân vật đang online → trong ≤ 30 s bồ câu nhấp nháy.
8. Thoát game, vào lại: thư còn nguyên, bồ câu hiện nếu còn thư chưa đọc.

## 6. Khác 2.0 (đã chốt hoặc bắt buộc)

- Không có đấu giá → không có người gửi "Chưởng quầy khu đấu giá" (bộ lọc chỉ 4 mục).
- Bỏ mở URL trong thư (`REQUEST_OPENURL` được nhận nhưng bỏ qua).
- Bấm bồ câu chỉ nhắc "đến Tín Sứ"; không tự chạy đường (client JX1 không có `AutoCrossMapFindPath`).
- Vật phẩm đính kèm là **thuộc tính** JX1 (không có seed ngẫu nhiên của 2.0): thuộc tính ngẫu nhiên sinh lúc nhận.
- Hộp xác nhận xoá dùng `UIMessageBox` của JX1 thay `ClientSay`.

## 7. Rủi ro chưa kiểm bằng mắt (cần chủ test)

- Nút `ForClickImg` phủ cả hàng thư: nếu sprite `信件选择框11.spr` không trong suốt sẽ che chữ → đổi thứ tự AddChild hoặc bỏ nút này (sửa `UiMail.cpp` KUiMailRow::Build).
- Ô vật phẩm 26x26 (`mail_award_item.ini`) nhỏ hơn ô chuẩn 38 px: ảnh có thể tràn viền → đổi Width/Height trong ini.
- Nội dung dài: `KWndText` 2048 byte, chưa có thanh cuộn nội dung.
