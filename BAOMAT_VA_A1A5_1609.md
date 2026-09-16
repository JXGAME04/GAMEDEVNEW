# VÁ 5 LỖ BẢO MẬT MÁY CHỦ (nhóm A, hồ sơ 14/09) — 16/09/2026

> Hồ sơ gốc: `BAOMAT_TRUOC_PHAT_HANH_1409.md` (nhóm A, "chặn phát hành, phải xong trước ngày mở").
> Nhánh: `claude/baomat-a1a5-1609` (từ `main` c01e1f34, tức đã gồm giới hạn mã máy [MAYID]).
> Kịch bản vá chạy lại được: `tools/va_baomat_1609.py`. Mọi chỗ sửa mang nhãn `[BAOMAT A? 16/09]`.
> **Chưa gộp vào `main`** — gộp sau khi chủ thử sống OK (đúng quy ước của đợt [MAYID]).

## 1. Đã đặt gì vào cây live (chủ tự áp, không ghi đè tệp đang chạy)

| Tệp | Đặt ở | md5 | Ghi chú |
|---|---|---|---|
| `CoreServer.dll.moi` | `bin\server\` | `dfffe145711ce3b3765b0589340d1180` | 18 506 752 byte; `ChayGameServer.bat` tự đổi tên (bản cũ → `.truoc`) |
| `Sword3PaySys.exe.moi` | `bin\multiserver\` | `4663529f08e43a313b547e501f3b6e83` | 856 576 byte; máy chủ tài khoản (đang chạy pid 208788) |
| `ChayTaiKhoan.bat` | `bin\multiserver\` | mới | áp `.moi` cho Sword3PaySys như `ChayS3Relay.bat` |
| `GmTaiKhoan.ini` | `bin\server\` | mới, **danh sách rỗng** | xem §3 — phải điền tài khoản GM |

Không có tệp nào của máy khách (Game.exe / CoreClient.dll) cần đổi: mọi sửa trong Core nằm trong vùng `#ifdef _SERVER`
(hoặc macro theo `_SERVER`), đã dựng thử `Client Release|Win32` = biên dịch + liên kết OK, mã máy khách không đổi.

**Thứ tự áp:** tắt GameServer → chạy `bin\server\ChayGameServer.bat` (ăn `CoreServer.dll.moi`) ; tắt Sword3PaySys.exe →
chạy `bin\multiserver\ChayTaiKhoan.bat`. Vì Bishop/GameServer nối vào máy chủ tài khoản lúc mở, an toàn nhất là **mở lại
cả cụm theo thứ tự thường ngày** (Sword3PaySys → S3RelayServer → Goddess → Bishop → S3Relay → GameServer).

## 2. Từng lỗ: gốc, sửa ở đâu, cách thử

### A1 — Người chơi thường chạy được Lua tuỳ ý qua chat (`?gm ds …`)
- **Gốc:** `KPlayerChat::ServerSendChat` kênh màn hình đưa mọi câu bắt đầu `?gm ` vào `TextGMFilter` → `ProcessGMCommand`
  chạy `DoScript`/`WorldScript`/`ExecuteScript`/`ReLoadScript` mà **không kiểm người gửi là ai**. Kèm tràn ngăn xếp:
  tên lệnh chép vào `char szCmd[20]` không cắt (câu chat tới 255 byte), `strstr` chạy quá độ dài câu, bộ đệm tham số
  `RSF` 200/100/100 byte trong khi mỗi token có thể dài tới 255.
- **Sửa (`Sources/Core/Src/KGMCommand.cpp`):**
  - `JxGmDuocPhep()` — đọc `\GmTaiKhoan.ini` `[GM] TaiKhoan=a,b,c`, so **tên tài khoản** (`GetPlayerAccount()`, không phân
    biệt hoa/thường). Không có tệp / danh sách rỗng = **không ai** chạy được. Mỗi lần thử (cho hay chặn) ghi log máy chủ
    `[BAOMAT-GM] CHO CHAY|CHAN: tai khoan '..' nhan vat '..' lenh '..'`. Người không phải GM: câu chat bị nuốt (như cũ),
    không chạy gì.
  - Tên lệnh: chỉ nhận dấu cách nằm trong `nLen`, độ dài 1..19, khác thì bỏ.
  - `RSF`/`RLS`: tham số phải < 300 byte (như `ds`/`dw`), bộ đệm 300/300/300.
  - Tệp đọc lại mỗi lần có `?gm` nên sửa danh sách **không cần mở lại máy chủ**.
- **Thử:** (1) tài khoản thường gõ `?gm ds Say("x")` → không có gì xảy ra, log máy chủ có dòng `[BAOMAT-GM] CHAN`.
  (2) Điền tài khoản GM vào `GmTaiKhoan.ini`, gõ lại → chạy, log `CHO CHAY`. (3) Gõ `?gm ` + 40 chữ liền → không sập.
- **Đổi hành vi cần biết:** GatewayRobot (bot thử tải) gửi `?gm ds SetPos(...)` — nếu còn dùng, thêm tài khoản bot vào
  danh sách. Đường GM qua S3Relay (`DoScript.cpp`, tài khoản "GM" của relay) không đi qua chat nên **không đổi**.

### A2 — Chèn SQL ở kiểm tài khoản (`Sword3PaySys.exe`)
- **Gốc:** `S3PAccount::Login/ServerLogin/VerifyUserModifyPassword` (và 8 hàm anh em) `sprintf` thẳng tên tài khoản /
  mật khẩu vào SQL; chỉ có một dòng chặn đúng một chuỗi tấn công cụ thể. API DB chỉ có `QuerySql(chuỗi)`, không có tham
  số hoá (MySQL, `S3PDB_MySQL_Connection`).
- **Sửa (`Sources/Sword3PaySys/S3AccServer/S3PAccount.cpp`):** `S3P_ChuoiSqlAnToan()` từ chối chuỗi **không kết thúc
  trong bộ đệm gói tin** (32/64 byte), hoặc chứa `'`, `\`, ký tự < 0x20. Không có `'` và `\` thì không cách nào đóng chuỗi
  SQL để chèn lệnh (kể cả mẹo byte đôi GBK, vì không escape mà **từ chối**). Áp ở đầu 11 hàm nhận tên tài khoản/mật
  khẩu: `Login`, `ServerLogin`, `VerifyUserModifyPassword` (trả `E_ACCOUNT_OR_PASSWORD`), `LoginGame`, `Logout`,
  `ElapseTime`, `QueryTime`, `GetServerID`, `GetAccountGameID`, `GetLockAccount`, `GetLeftSecondsOfDeposit` (trả
  `ACTION_FAILED`).
- **Thử:** tài khoản `' or '1'='1` / mật khẩu bất kỳ → "sai tài khoản hoặc mật khẩu". Tài khoản thật vẫn vào bình thường.
- **Đổi hành vi cần biết:** tài khoản/mật khẩu **có sẵn** chứa `'` hoặc `\` sẽ không đăng nhập được nữa (kiểm nhanh
  trong bảng `Account_info`: `select cAccName from Account_info where cAccName like '%''%' or cAccName like '%\\%'`).
  Đã kiểm: tên/mật khẩu Bishop nối máy chủ tài khoản (`Bishop.cfg`, `StartupCfg.ini`) không có ký tự bị cấm.
- **Chưa đụng:** `S3RELAYSERVER/S3PAccount.cpp` (S3RelayServer.exe) cũng ghép chuỗi kiểu này nhưng nhận tên từ máy chủ
  game (đã qua đăng nhập), không phải từ người chơi — để đợt sau nếu muốn cùng chuẩn.

### A3 — Mua sạp: nhân đồ / mua món chưa bày / nhét thẳng vào ô trang bị
- **Gốc (`KProtocolProcess::c2sTradeBuy`):** `m_Idx` do máy người chơi gửi dùng thẳng làm chỉ số `Item[]` (không kiểm
  biên); không chặn tự mua sạp mình; `GetPrice` không kiểm món có ở phòng bày bán; ô đích `m_Place/m_X/m_Y` vào thẳng
  `AddKIL` (kể cả `pos_equip`); `AddKIL` thất bại vẫn `Pay`.
- **Sửa:** kiểm `0 < m_Idx < MAX_ITEM`; cấm `m_PlayerId` = chính mình và `nPlayerIdx == nIndex`; món phải có trong danh
  sách người bán (`FindSame`) **ở hành trang** (`pos_equiproom`, đúng phòng `SendSellItemInfo` bày) với `nPrice > 0`;
  `m_Place` chỉ được `pos_equiproom` (máy khách luôn gửi thế); bản sao chỉ tạo khi hợp lệ; `AddKIL` thất bại (ô bị chiếm /
  ngoài lưới) → huỷ bản sao (`ItemSet.Remove`), **không trừ tiền**, sạp giữ nguyên.
- **Thử:** mua bình thường từ sạp người khác → vẫn được, thuế thành vẫn tính. Gói tin sửa `m_Idx` lạ / tự mua / `m_Place=2`
  → bị bỏ, không mất tiền.

### A4 — Giá âm
- **Gốc:** `KProtocolProcess::SetPrice` + `KItemList::SetPrice` lưu mọi giá ≠ 0, kể cả âm; `c2sTradeBuy` chỉ chặn `== 0`.
  `SetPrice` với món không có trong túi ghi vào `m_Items[0]`/`Item[0]`.
- **Sửa:** gói tin `m_Price < 0` → bỏ; `KItemList::SetPrice` (chỉ máy chủ) bỏ giá âm và bỏ khi `nGameId`/`nId` = 0;
  đường mua đòi `nPrice > 0` (A3).
- **Thử:** đặt giá bằng gói tin sửa `-1` → sạp không hiện món, mua không được.

### A5 — Gửi tiền ngân hàng tràn dấu
- **Gốc (`KProtocolProcess::StoreMoneyCommand`):** `m_dwMoney` (DWORD) đưa thẳng vào `ExchangeMoney(int)`: ≥ 2^31 hoá âm
  → `AddMoney(-n)` **cộng** cho phòng nguồn rồi trừ phòng đích, tức đảo chiều chuyển — rút tiền rương khi **chưa mở khoá**
  (kiểm `m_CUnlocked` chỉ áp khi nguồn là rương).
- **Sửa:** chỉ nhận `1 .. 0x7FFFFFFF`; còn lại bỏ. Với n > 0, `AddMoney(-n)` ở nguồn đã kiểm số dư nên bước hoàn tác không
  thể thất bại — `ExchangeMoney`/`AddMoney` (mã dùng chung với máy khách) **không đổi**.
- **Thử:** gửi/rút bình thường vẫn được; gói tin `m_dwMoney = 0xFFFFFFFF` → không đổi số dư.

## 3. Việc chủ phải làm
1. Điền tài khoản GM (tên **tài khoản đăng nhập**, cách nhau dấu phẩy) vào `bin\server\GmTaiKhoan.ini`:
   `TaiKhoan=gm,tenkhac`. Để rỗng = không ai dùng được `?gm`.
2. Áp `.moi` + mở lại cụm (§1). Kiểm log GameServer có dòng `[BAOMAT-GM]` khi gõ `?gm`.
3. Thử sống 5 mục ở §2 (ít nhất: đăng nhập thường, mua sạp thường, gửi/rút tiền rương thường, `?gm` với tài khoản GM).
4. Báo OK → tôi gộp nhánh vào `main` và `mobile-0809`.

## 4. Kiểm chứng đã làm
- 4 tệp nguồn: số byte cao TCVN3 bằng HEAD (`check_encoding.py`, FFFD = 0), chỉ thêm ASCII.
- Dựng `Core` **Server Release|x64** (CoreServer.dll, có chuỗi `[BAOMAT-GM]`, `GmTaiKhoan.ini`) và **Client Release|Win32**
  (biên dịch + liên kết OK, xác nhận nhánh `#else` cho máy khách vẫn dựng) ; `Sword3PaySys` **Release|Win32** (phụ thuộc
  DLL y hệt bản đang chạy: libmysql, ws2_32, kernel32, user32, ole32, oleaut32).
- Không đổi giao thức / cỡ gói / cấu trúc dữ liệu / DB (Gate 2), không đổi cân bằng (Gate 4).

## 5. Còn lại ngoài nhóm A (không thuộc đợt này)
Nhóm B/C của hồ sơ 14/09 (ký gói cài, tự cập nhật, mã hoá đường truyền, chống gian lận mobile) — đã liệt kê trong
`KEHOACH_PHAT_HANH_1609.md` mục 1C/1D.
