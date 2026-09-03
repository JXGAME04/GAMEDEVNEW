# Mổ client "Vo Lam Truyen Ky 2.0" (Level Up Games) — hệ THƯ (03/09/2026)

Cây mổ: `C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky 2.0` (client JX2 bản VNG 2021+, UI "Ui4").

## Kết luận

| Câu hỏi | Trả lời |
|---|---|
| Có hệ thư không? | **Có** — hộp thư đến (inbox) do server gửi: "Thư hệ thống", "Nhà phát hành", "Chưởng quầy khu đấu giá". |
| Người chơi **gửi thư** cho nhau được không? | **Không.** 12 giao thức MAIL không có REQUEST_SEND; exe không có lớp/nút soạn thư, chỉ `KUiMailInBox`. |
| Người chơi làm được gì? | Đọc, nhận đính kèm (item/Ngân lượng/Kim đĩnh/EXP/Danh vọng/Vinh dự…), xoá 1 / xoá tất cả đã chọn, lọc (tất cả/chưa đọc/chưa nhận/hệ thống/đấu giá), tự xoá thư rỗng, mở URL trong thư. |
| Mở hộp thư ở đâu? | NPC **Tín Sứ** ở thành (server bắn `emSCRIPT_PROTOCOL_MAIL_OPENWINDOW`). Biểu tượng "bồ câu đưa thư" nhấp vào chỉ đường tới Tín Sứ (map 176/80/37/78/11/162/1). |
| Giới hạn | 100 thư/người (`MAILDEF.PLAYER_MAX_MAIL`), cache/hết hạn 30 ngày (biến thể 7 ngày). Trạng thái 0 chưa nhận→1 chưa đọc→2 đã đọc→3 đã nhận→4 đã xoá. |

## Bằng chứng

### Script client (`data\slistcl.pak`, 666 mục, nén UCL NRV2B, chữ Việt TCVN3 + chú thích GBK)
| Tệp | uid | Ghi chú |
|---|---|---|
| `\script\ui\uimail.lua` | 9565EFB1 | Toàn bộ UI hộp thư (`mail_vltk2\uimail.lua`) |
| `\script\mail\MailDef.lua` | 1F41E8E9 | Hằng số; server script = `\script\mail\MailManager.lua` (không có trong client) |
| `\script\protocol.lua` | 26039A29 | 12 tên `emSCRIPT_PROTOCOL_MAIL_*` |
| bộ điều phối objbuffer | D0E42D05 | 6 handler S→C → `UIMail:*Arrival` / `OpenMailWindow` |
| `\ui\ui3_1024\mail\mail_icon.ini` | E772B550 | `[MailBtn] Image=spr\Ui4\email\邮箱图标.spr Tip=Nhắc nhở thư mới` (bản ui3_800 = CDBBBE0F) |

Giao thức: S→C `HEADERLIST, NEWMAIL, STATECHANGE, DELETE, WHOLEMAIL, OPENWINDOW`;
C→S `REQUEST_HEADERLIST, REQUEST_DELETE, REQUEST_WHOLEMAIL, REQUEST_STATECHANGE, REQUEST_AUTODELETE, REQUEST_OPENURL`.

### `gamecl.exe` (nén UPX 3.x NRV2E_LE32, filter 0x24 — giải bằng `upx_unpack.py`)
- 14 hàm C++ xuất cho Lua: `SetMailIconVisible OpenMailWindow UpdateMailDetail UpdateMailCount SetMailBntStatus CleanMailDetail CleanMailList CleanMailAll DeleteOneMail SetMailHeader AddMailHeader NewMailUIEventArrival SelectMail SwitchMailManager`.
- 7 lớp: `KUiMailManager KUiMailInBox KUiMailList KUiMailHeader KUiMailDetail KUiMailIcon UiMailAwardIcon`.
- 6 ini nạp bằng `sprintf("%s\%s", <gốc theme>, "mail\mail_{manager,list,header,detail,icon,award_item}.ini")`;
  gốc theme từ `\ui\setting.ini` `[Theme] 0_Path=ui3_800 / 1_Path=ui3_1024`.
- Thành phần UI: `ItemAccept_Btn DeleteMail_Btn DeleteAllMail_Btn SelectAllMail_Btn AutoDelete_Btn MailFilterBtn MailFilterMenu MailList MailCountValue MailSender* MailTitle* MailContent* MailURLLable MailSendTime MailAwardPos` + tab `InBox`. **Không** có Send/Compose/Recipient/OutBox.
- `enginefree.dll` không nén, không có logic game (chỉ chuỗi OpenSSL "email").

### Đối chiếu
- **Client JX1 cũ** `C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky\game_y.exe` (không nén): cùng 14 hàm + 7 lớp; `slistcache.pak` có `uimail.lua` (19.499 B — bản 2.0 thêm `REQUEST_OPENURL`, `nSendTime`, thưởng Danh Vọng/Vinh Dự/Tu Vi); **đủ 6 ini** `\ui\ui3\mail\*.ini` (`mail_jx1cu\ui3_*.ini`): tiêu đề "Hộp thư", tab "Nhận thư", nút "Nhận vật phẩm / Xóa thư / Xóa toàn bộ / Tự động xóa trống hộp thư".
- **Server Linux `D:\ServerLinux`**: KHÔNG có `script\mail`, `protocol.lua` (38 tên) không có MAIL, binary `jx_linux_y` 0 hit. Có nền `ScriptProtocol/ObjBuffer/SendScriptData` (139 script dùng).
- **Mã nguồn JX1 dự án**: 0 tệp có Mail; có `ObjBuffer/ScriptProtocol` ở `KJx2SharedStore.cpp`, `ScriptFuns.cpp`.

### Chưa rõ
- Trong 2.0 chưa định vị được ini **cửa sổ** thư (`mail_manager/list/header/detail/award_item`) trong mọi mục
  dạng chữ của 13 pak — nhưng ini cửa sổ của liên đấu/luyện dược (`uiarena.ini`, `composemedicine.ini`)
  cũng không thấy ⇒ chưa kết luận được là "2.0 bỏ cửa sổ thư" hay "ini cửa sổ 2.0 nằm dạng khác".
- `resource.pak`/`res2.pak`/`spr.pak` cờ 0x11: raw bắt đầu `SPR` dù csize≠size — sprite nén nội bộ, NRV2B thất bại.

## Công cụ (thư mục này)
| Tệp | Việc |
|---|---|
| `nrv.py` | NRV2B/2D/2E (8-bit & LE32) thuần Python, có `limit` để "nhìn đầu tệp" |
| `upx_unpack.py <exe> <out.bin>` | Giải nén ảnh UPX (đọc packheader `UPX!`, tự tìm `pushad; mov esi` để lấy offset) |
| `pakpeek.py <pak> <outdir>` | Phân loại mọi mục bằng 96 byte đầu, rút mục dạng chữ |
| `pakscan2.py <pak> <outdir> [--dump]` | Giải nén trọn pak + quét từ khoá thư nhiều bảng mã |
| `strscan.py <img> <baseRVA> <imagebase> <regex>` | Quét chuỗi ASCII trong ảnh đã giải nén |
| `uiroot2.py / uiroot3.py` | Dò gốc `%s` của đường ini (xref, brute-force) |

Bẫy: heredoc Bash biến `\\u` thành `\u` (SyntaxError) — chuỗi đường dẫn dùng `chr(92)` hoặc viết tệp bằng Write.
Bẫy: brute-force băm 32-bit trên ~160k uid cho hàng chục **trùng băm giả** — chỉ tin hit khi giải nén ra đúng nội dung.

## Tiến độ thi công (chủ chốt 03/09: MySQL · kênh ScriptProtocol thật · đính kèm item/Ngân lượng/xu/EXP · thư từ web admin + hoạt động + top tuần/tháng + đua top · bỏ URL)

| Đợt | Việc | Trạng thái |
|---|---|---|
| 1 | Kênh ScriptProtocol thật: `KScriptProtocol.{h,cpp}`, gói `s2c_scriptdata`/`c2s_scriptdata`, ObjBuffer cho client, `protocol.lua` + 12 tên MAIL, `protocol_def_c.lua`, ECHO thử | XONG (258f0948) |
| 2 | Client: `uimail.lua` + `maildef.lua`, 6 ini `\ui\ui3\mail\`, `UiMail.{h,cpp}` + `KMailClient.{h,cpp}` (15 hàm 2.0) | XONG (2b1aff9e) |
| 3 | Server: `KMailServer.{h,cpp}` bảng MySQL `mail`, `mailmanager.lua` + `mailpoll.lua`, `MailManager_SendMail` API, Tín Sứ, lệnh bài admin, web admin ghi thẳng bảng | XONG (c72aaa8c, main 5c8b8736) — **chờ swap bộ 3**: `CoreServer.dll.moi` cca51fdf · `CoreClient.dll.moi` e4ac910a · `Game.exe.moi` 24762253 |
| 4 | Nghiệm thu 4 loại đính kèm, xoá, lọc, hết hạn, túi đầy | chờ chủ test — 8 bước trong `D:\GAMEDEVNEW\BANGIAO_HETHU_0309.md` |

Thử đợt 1: lệnh bài admin → "Thu kenh ScriptProtocol (ECHO)/specho" → khung thoại `[ECHO] xin chao tu may chu`,
chat `May chu da nhan ECHO: client da nhan: xin chao tu may chu`, `logs\hethong.log` dòng `[SCRIPTPROTO]`.

## Khảo sát dự án JX1 cho việc port (03/09, chưa làm)

| Hạng mục | Tình trạng ở JX1 | Việc phải làm |
|---|---|---|
| Kênh `ScriptProtocol`/`ObjBuffer` | `SendScriptData` = STUB (`KJx2WarInfra.cpp:2182`); ObjBuffer chỉ trong bộ nhớ; client có `protocol.lua` + `objbuffer_head.lua` + dispatcher 234 B | 2 gói s2c/c2s chở ObjBuffer, `ScriptProtocol:SendData` phía client, dispatch 2 đầu |
| Script client | Không có `uimail.lua`/`MailDef.lua` | Chép nguyên từ 2.0 (đã TCVN3), thêm 12 tên vào `protocol.lua` 2 đầu, 6 handler vào dispatcher |
| Ini + sprite | Gốc ini `\ui\ui3\`; 17/17 sprite đã có | Thả 6 ini `\ui\ui3\mail\*.ini` của client cũ |
| C++ UI client | 0 lớp Mail; khung `Sources\S3Client\Ui\UiCase\` | Viết `UiMailManager/InBox/List/Header/Detail/Icon/AwardIcon` + 15 hàm xuất Lua |
| Server | Không có `script\mail`; C++ có MySQL | `MailManager.lua` + lưu trữ (MySQL) + API `SendMail` cho GM/sự kiện + hết hạn/tràn |
| NPC Tín Sứ | `dichquan.lua` ở đúng 7 thành 1/11/162/37/78/80/176 | Thêm mục "Nhận thư" → `MAIL_OPENWINDOW` |
| Đấu giá | Không có | Bỏ người gửi "Chưởng quầy khu đấu giá" |
| Đính kèm | `NewItemEx` là item JX2 | Đổi sang item JX1 hoặc server gửi {icon, tên, mô tả, số lượng} |
