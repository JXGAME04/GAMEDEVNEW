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

Đã swap 16:47 03/09 (bad8e293 / f2ad5ca3 / 24762253). Lần thử đầu của chủ (16:49, 16:50) báo `ScriptError 4 ... lenhbaiadmin.lua
cFuncName:(xu/exp/mailtest1)`: nhãn menu "Gửi thư thử: tiền/xu/exp/mailtest1" có dấu `/` thừa — Say tách tên hàm ở dấu `/` ĐẦU TIÊN
(luật cũ: cấm `/` `|` trong nhãn). Đã sửa thành "Gửi thư thử: tiền, xu, exp" (commit bfa7cd24). Nạp lại script mà không khởi động lại
GameServer: gõ vào ô chat `?gm RLS \script\item\lenhbaiadmin.lua` (`KGMCommand.cpp`, mở bằng `_CHAT_SCRIPT_OPEN` trong `GameDataDef.h`;
🔴 lệnh này KHÔNG kiểm tra quyền GM — mọi người chơi đều gõ được `?gm ds <lua>`, cần chặn trước khi mở server thật).

0. Nạp lại script lệnh bài (chat `?gm RLS \script\item\lenhbaiadmin.lua`) hoặc khởi động lại GameServer.
1. Lệnh bài admin → "Thu kenh ScriptProtocol (ECHO)": khung thoại `[ECHO] xin chao tu may chu` + chat `May chu da nhan ECHO: ...`.
2. Lệnh bài admin → "Gửi thư thử: tiền, xu, exp" → chat "Đã gửi thư thử (id N)"; bồ câu góc phải nhấp nháy.
3. Đến **Tín Sứ** (Quan Dịch Trạm) bất kỳ thành nào → "Nhận thư" → cửa sổ Hộp thư: hàng thư "Nhà phát hành / Thư thử hệ thống thư / 30 ngày" (kẹp đính kèm).
4. Bấm hàng → chi tiết (người gửi, tiêu đề, nội dung 3 dòng, 3 ô thưởng) → "Nhận" → chat "Đã nhận đính kèm trong thư." + Ngân lượng/xu/exp tăng; bấm Nhận lần 2 → "Đính kèm đã được nhận rồi!".
5. "Gửi thư thử: có vật phẩm" → ô vật phẩm (Lệnh bài Bắc Đẩu 6,1,4139) có chú giải khi rê chuột → Nhận → vật phẩm vào túi.
6. Xoá thư (có xác nhận khi còn đính kèm) · Chọn hết + Xóa toàn bộ · Lọc (Toàn bộ / chưa đọc / có đính kèm / hệ thống) · Tự động xóa thư trống (đọc thư không đính kèm → tự xoá).
7. Web: INSERT một dòng như mục 2 cho nhân vật đang online → trong ≤ 30 s bồ câu nhấp nháy.
8. Thoát game, vào lại: thư còn nguyên, bồ câu hiện nếu còn thư chưa đọc.

## 5b. ĐỢT 4 (03/09 chiều) — chủ báo "bấm Nhận thư không hiện gì" + icon dưới Bầu Cua + tự mở hộp thư + bỏ Tín Sứ

**Bộ `.moi` đợt 4 (chỉ CLIENT; CoreServer giữ bad8e293; build từ origin/main b020e6ea + D4b, KHÔNG gộp rep3-0309):**

| Tệp | md5 | Kích thước |
|---|---|---|
| `bin\client\CoreClient.dll.moi` | 6bbcda8f | 2.508.800 |
| `bin\client\Game.exe.moi` | bd5cb88e | 1.401.856 |

Swap: thoát game → `ChoiGame.bat`. Máy chủ không đổi binary nhưng SCRIPT đã đổi (`mailmanager.lua`, `dichquan.lua`, `lenhbaiadmin.lua`):
gõ chat `?gm RLS \script\player\playerlogin.lua` + `?gm RLS \script\global\npcchucnang\dichquan.lua` + `?gm RLS \script\item\lenhbaiadmin.lua`,
hoặc chạy lại `ChayGameServer.bat`.

Chẩn đoán lần 1 (16:50, chủ bấm "Nhận thư" ở Tín Sứ 5 lần): không có lỗi script hai phía, `g_DebugLog` chỉ bắn ra cửa sổ debug (không ra tệp)
→ mù. Đọc lại toàn chuỗi (enum `s2c_scriptdata` trước `s2c_end`, bảng kích thước khớp 155/109 hàng, tách gói `-1`, dispatcher Lua) không thấy lỗi
⇒ thêm nhật ký để lần test sau nhìn được:
- Client `bin\client\jx_mail.log`: `[SP] nhan s2c_scriptdata id= len=` → `[SP] dispatch ...` → `[MAILUI] Lua->UI cmd=` → `[UI] cmd=` →
  `[UI] OpenWindow: cua so tai (x,y)`; `[UI] GAME_START`, `[UI] bieu tuong thu tai (x,y)`; `[UI] KHONG nap duoc <ini>` nếu thiếu ini.
- Máy chủ `logs\hethong.log` `[MAIL]`: `X dang nhap: N thu`, `gui emSCRIPT_PROTOCOL_MAIL_HEADERLIST -> player N: ok|THAT BAI`, `X mo hop thu`.

Thiết kế mới (theo 3 yêu cầu của chủ):
- Icon thư LUÔN hiện sau khi vào game tại (765,296) ở 800x600 = ngay dưới icon Bầu Cua (`UiPlayerBar.ini [SpringGame]` 765,243 50x50);
  1024x768: x = 1024−30 như cột icon phải (`UiPlayerBar.cpp`). Nhấp nháy khi còn thư chưa đọc. Bấm icon → mở hộp thư ngay (không Tín Sứ).
- Thư mới (web/hoạt động khi đang online, hoặc thư state 0 giao lúc đăng nhập) → hộp thư TỰ MỞ (`UIMail:NewMailEventArrival` → `OpenMailWindow`).
- Gói thư lúc đăng nhập đến TRƯỚC `s2c_syncend` (`playerlogin.lua` chạy đầu `KPlayer::LaunchPlayer`) → C++ hoãn icon/mở tới
  `GDCNI_GAME_START`; `GDCNI_EXIT_GAME` huỷ cửa sổ + icon + `MAILUI_OP_RESET` → `UIMail:Reset()` (state Lua client sống suốt tiến trình).
- Tín Sứ: gỡ mục "Nhận thư" khỏi `dichquan.lua` (`MailManager_OpenWindow` giữ cho script khác dùng).

Kiểm tra sau swap: (1) vào game → icon thư dưới Bầu Cua; (2) lệnh bài "Gửi thư thử: tiền, xu, exp" → hộp thư TỰ MỞ, có hàng thư;
(3) bấm hàng → chi tiết → Nhận; (4) đóng hộp thư, bấm icon → mở lại; (5) web INSERT → ≤ 30 s tự mở; (6) thoát ra chọn nhân vật, vào lại → icon
lại hiện, danh sách đúng nhân vật. Nếu vẫn không hiện: gửi `bin\client\jx_mail.log` + `bin\server\logs\hethong.log`.

**ĐỢT 5 (17:40–17:50) — GỐC LỖI TÌM RA NHỜ jx_mail.log:** chủ swap 6bbcda8f/bd5cb88e → icon hiện, bấm không mở. Log: `[SP] nhan s2c_scriptdata id=37`
(kênh tốt) rồi `[SP] khong nap duoc bo dieu phoi \script\script_protocol\protocol_def_c.lua`, `RunClientLua: khong nap duoc \script\ui\uimail.lua`.
Nguyên nhân: `KSortScript.h:15` phía client `MAX_SCRIPT_IN_SET = 5` (máy chủ 5000) → `LoadAllScript("\script")` nạp 5 tệp `lib\` đầu rồi đầy,
`g_GetScript`/`ReLoadScript` thất bại với mọi tệp khác. Sửa: `KScriptProtocol.cpp` giữ bảng `KLuaScript` riêng phía client (nạp theo yêu cầu,
`RegisterFunctions(GameScriptFuns)` + `MODEL_GAMECLIENT` như `LoadScriptToSortListA`), `SP_FindScript()` xuất cho `LuaDynamicExecute` (client).
→ `bin\client\CoreClient.dll.moi` **e151cbfc** (2.515.456); `Game.exe` bd5cb88e giữ nguyên. Commit D5 trên origin/main.

Phiên wauto-c1 (Represent3, nhánh rep3-0309) bị chặn ghi `bin\client` và nhờ gộp vào bộ này — KHÔNG làm hộ (chủ tự chép/cho phép); họ tự gộp
origin/main (đã có D4/D4b) vào rep3-0309 và đặt sau. `Represent3.dll` 74ac07ad đã nằm ở bin\client nhưng `config.ini [Client] Represent=2` nên chưa dùng.

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
