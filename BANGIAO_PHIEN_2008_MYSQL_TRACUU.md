# BÀN GIAO PHIÊN 20/08/2026 (chiều → tối) — MySQL: sự cố gateway, cứu bang hội, 3 bảng tra cứu

> **ĐỌC TỆP NÀY TRƯỚC KHI GÕ BẤT CỨ THỨ GÌ.**
> Tài liệu nền của đợt sáng: `BANGIAO_DB_LEN_MYSQL_2008.md`.
> Phiên này tiếp nối và **sửa 2 sự cố do chính đợt MySQL gây ra**.

---

## 0. TÓM TẮT 30 GIÂY

| Việc | Commit | Trạng thái |
|---|---|---|
| Sửa đường dẫn `settings\jx2*.txt` trong CoreServer | `1ee170d2` | ✅ đã deploy |
| **Gộp giao dịch `SaveStatInfo`** → hết lỗi gateway 5632 | `b119f973` | ✅ đã deploy + nghiệm thu đầu-cuối |
| **Bishop: `Reset()` `m_hQuitEvent`** → hết "xác sống" | `746b0dac` | ✅ đã deploy, ⚠️ **CHƯA RESTART Bishop** |
| Cứu `dwTongID` của `CaiBang` | (thao tác dữ liệu) | ✅ đã kiểm 1003/1003 |
| **3 bảng tra cứu**: vật phẩm / giao dịch / mua shop | `ab65d0e3` | ✅ đã sinh + kiểm chứng ngược |

**THỨ TỰ KHỞI ĐỘNG BẮT BUỘC** (sai thứ tự là mất gateway):

```
mysqld → Sword3PaySys → Goddess → ⏳ CHỜ cổng 5011 nghe → Bishop → S3Relay → GameServer
```

Lệnh chờ:
```powershell
while (-not (Get-NetTCPConnection -State Listen -LocalPort 5011 -EA SilentlyContinue)) { Start-Sleep -Milliseconds 200 }
```
Nghiệm thu Bishop: tiêu đề cửa sổ phải đổi thành **`Bishop - [Enable]`** (`Application.cpp:530`).
Còn là `Bishop` trơn ⇒ đã hỏng ⇒ **TẮT HẲN rồi chạy lại, TUYỆT ĐỐI KHÔNG bấm nút OK** (xem §3).

---

## 1. SỰ CỐ "Connect to gateway is failed!" — GỐC LÀ BẢN MySQL CỦA TÔI

### Triệu chứng
GameServer in `Gateway IP : 127.0.0.1 - Port : 5632` rồi `Connect to gateway is failed!`.
S3Relay lặp `FAIL: reclient gateway - 127.0.0.1`.
Bishop **có chạy** nhưng **không nghe cổng nào**, kể cả 5622.

### Chuỗi nhân quả (đo thật, không suy đoán)

1. `Goddess.cpp:698-759` chạy theo thứ tự:
   `InitDBInterface()` → **`StartBackupTimer()` (`:707`)** → … → **`OpenService(INADDR_ANY, 5011)` (`:737`)**
   ⇒ **cổng 5011 chỉ mở SAU khi `StartBackupTimer` xong.**
2. `StartBackupTimer` → `IDBRoleServer.cpp:695` → `CDBBackup::SaveStatInfo()` **đồng bộ**.
3. `SaveStatInfo` (`DBBackup.cpp:855-925`) duyệt `LevelStat[1000]` + `LevelStatBySect[11][100]`.
   Đo trên `StatData.dat` thật: **1952 cặp `search()`+`add()`**, chạm **1003 bản ghi**.
4. Tầng MySQL đặt `mysql_autocommit(conn,1)` (`DBTable_MySQL.cpp:436`) ⇒ **mỗi `add()` là một giao dịch = một fsync**.
5. **Đo trong sản xuất** bằng cột `updated_at timestamp(3)`:
   `16:47:44.474 → 16:47:48.365` = **3,891 giây** cho 1003 bản ghi.
   *(Bản Berkeley DB ghi trong bộ nhớ nên gần như tức thì — bẫy chỉ lộ ra sau khi lên MySQL.)*
6. Bishop bật sau Goddess **1-3 giây**, gọi `ConnectTo(RoleSvr 5011)` **ĐÚNG MỘT LẦN**
   (`Intercessor.cpp:389`), **không thử lại** ⇒ hụt.
7. ⇒ `bDBRoleServerSucceed=false` ⇒ `StartupNetwork()` trả **false** (`Intercessor.cpp:505`, AND của 3 cờ)
8. ⇒ `Application.cpp:521-526` gọi `Destroy()` → `CleanNetwork()` → `CloseService()`
   **đóng luôn cổng 5632 vừa mở ở `:438`.**

### Bằng chứng thời gian
| Phiên Bishop | Goddess `open()` | Bishop bật | Cách | Gateway |
|---|---|---|---|---|
| 09:15:34 → 11:57:30 | *(Berkeley DB)* | 09:15:34 | — | **CÓ** (2h42) |
| 16:04:06 / 16:25:56 / 16:33:19 / 16:45:20 / 16:47:45 | 16:04:03 / 16:25:55 / 16:33:17 / 16:45:19 / 16:47:44 | — | **1-3 giây** | **KHÔNG** (cả 5) |

`Goddess.exe` bản MySQL deploy lúc **13:43**. Gateway chưa chạy lần nào sau mốc đó cho tới khi vá.

### Cách sửa — `b119f973`
Thêm **GÓI GHI** cho `ZDBTable`: `BatDauGoi()` / `KetThucGoi()`.
Trong gói, `add()` và `remove()` **không tự mở/chốt giao dịch** nữa; cả gói chốt **một lần**.
`SaveStatInfo` bọc trong một gói ⇒ **1003 lần fsync còn 1**.

Tệp sửa (đều byte-safe, byte cao giữ nguyên):
- `Sources/MultiServer/Goddess/DBTable.h` (502 byte cao) — khai báo
- `Sources/MultiServer/Goddess/DBTable.cpp` (116) — thân rỗng cho bản Berkeley DB (giữ lối quay lui)
- `Sources/MultiServer/Goddess/DBTable_MySQL.cpp` (0, ASCII, **LF** không phải CRLF) — cài đặt
- `Sources/MultiServer/Goddess/DBBackup.cpp` (1954) — bọc `SaveStatInfo`

### Nghiệm thu
```
SaveStatInfo                3,891 giây → 0,966 giây
Goddess sẵn sàng (5011 mở)  1,75 giây
Thử đầu-cuối: Goddess → chờ 5011 → Bishop  ⇒ cổng 5632 mở sau 0,42 giây ✓
1003/1003 bản ghi ghi đúng, role_save_fail = 0, không báo động mới
```
Deploy: `bin\multiserver\Goddess.exe`, bản cũ giữ ở `Goddess.exe.bak_truoc_goi_ghi`.

### 🔴 Nợ còn lại
1,75 giây **vẫn nằm trong** khoảng 1-3 giây Bishop hay bật. **Luật vận hành ở §0 là bắt buộc.**
Muốn diệt hẳn thì phải đảo `StartBackupTimer` xuống sau `OpenService` — tôi **chưa làm** vì
cần khảo sát xem `ConnectTo` của Bishop có cần bắt tay tầng ứng dụng hay chỉ cần TCP connect.

---

## 2. CỨU `dwTongID` — STUB TRẢ `false` ĐÃ LÀM MÁY CHỦ TỰ XOÁ BANG CỦA NGƯỜI CHƠI

### Đây là bài học đắt nhất của cả hai phiên
Đợt trước tôi để `ZDBTable::GetRecordEx` (bản MySQL của S3Relay) trả `false` kèm ghi chú
*"không nơi nào gọi"*. Kết luận đó **SAI** vì tôi grep `*.cpp` mà tệp thật tên **`TONGDB.CPP` VIẾT HOA**
nên glob không khớp.

**Hậu quả không dừng ở "đọc ra rỗng".** `CTongDB::GetTongCount/GetTongList`
(`TONGDB.CPP:349,357,371,379`) duyệt ra rỗng ⇒ máy chủ kết luận bang không tồn tại ⇒
**đặt `dwTongID=0` trong blob nhân vật rồi LƯU ĐÈ.**

### Bằng chứng (từ chính bảng `role_history` tôi tạo ra khi lên MySQL)
```
id=9     16:04:03  ver=0   tong_id=441122550   ← sau di trú, còn nguyên
id=1013  16:45:19  ver=20  tong_id=0           ← đã bị xoá
hiện tại           ver=53  tong_id=0
```
Đúng cửa sổ S3Relay chưa vá (bản sửa lên **16:14:35**).
Bản gốc Berkeley DB `database/roledb` (đóng băng 11:57) cũng cho 441122550.

### Cách cứu
Vá **đúng 4 byte** `dwTongID` tại `O_dwTongID = 305` + tính lại CRC ⇒ **8 byte đổi**.
**KHÔNG** khôi phục blob cũ (làm vậy nuốt 53 lượt lưu: cấp 151, tiền, 45 vật phẩm).

Công cụ: `scratchpad\cuu_tongid.py` — có chế độ chỉ-thử, chụp ảnh lùi vào `role_history`,
khoá `ver` chống tranh chấp, **chỉ chạy khi nhân vật OFFLINE**.

Kiểm lại 1003/1003: **2/2 nhân vật có bang, 0 mất, 0 thừa.**

### 🔑 LUẬT RÚT RA
> Một stub trả `false` trong tầng DB **không dừng ở "đọc ra rỗng"** — mã game phía trên
> coi rỗng là SỰ THẬT và **ghi đè dữ liệu thật**.
> Cấm để stub im lặng. Nếu buộc phải để, phải `DbLog` và phải rà mọi nơi gọi
> bằng tìm kiếm **ĐỆ QUY**, không dùng glob theo phần mở rộng.

---

## 3. LỖI "BISHOP XÁC SỐNG" — có sẵn từ lâu, không liên quan MySQL

`m_hQuitEvent` tạo kiểu **MANUAL-RESET** (`Intercessor.cpp:34`, tham số thứ 2 = `true`),
chỉ được `Set()` trong `Destroy()` (`:98`), và **KHÔNG NƠI NÀO `Reset()`**.
Hai vòng làm việc đều là `while ( !m_hQuitEvent.Wait( 0 ) )` (`:625` và `:734`).

**Hậu quả:** sau MỘT lần `Create()` hỏng, người vận hành **bấm nút OK để thử lại**
(`Application.cpp:700` gọi lại `Create()`): cổng 5622/5632 mở lại được **NHƯNG hai luồng mới
thoát ngay lập tức** vì cờ thoát vẫn bật. Bishop thành "xác sống" — cổng mở mà không xử lý gì,
không phát `c2s_ping`, nên Sword3PaySys cắt kết nối **mỗi 20 giây**
(`S3PDBSocketPool.cpp:21 PING_TIME 20000`). Nhìn ngoài tưởng bình thường nhưng không ai vào được.

### Sửa — `746b0dac`
- `Reset()` cả 3 sự kiện ở đầu `Create()`
- `== INVALID_HANDLE_VALUE` → `== NULL` tại 2 chỗ: `CreateThread` thất bại trả **NULL**
  (`INVALID_HANDLE_VALUE` là quy ước của `CreateFile`) ⇒ hai nhánh bắt lỗi cũ là **MÃ CHẾT**

### 🔴 Gỡ bẫy "hai cây lệch nhau" trước khi build (BẮT BUỘC làm lại nếu build Bishop lần sau)
`Bishop.exe` có từ **19/04**, cây nguồn D nhận commit snapshot 12/08 (`9bc7936a`, +319/−86).
Tôi đối chiếu **bảng chuỗi văn bản** giữa binary đang chạy và binary build lại:

```
1039/1041 chuỗi trùng = 99,81%
khác biệt duy nhất: đường dẫn .pdb của máy build
kích thước giống hệt: 2.579.456 byte
```
⇒ cây nguồn đúng là nguồn của binary đang chạy, build lại an toàn.
*(1,6 triệu byte lệch chỉ là bố cục tái định vị của bản Debug build lại — đừng hoảng.)*

Cấu hình build multiserver: **`Debug|Win32`** (không phải Release).
Deploy: `bin\multiserver\Bishop.exe`, bản cũ ở `Bishop.exe.bak_truoc_quitevent`.

> ⚠️ **CHƯA RESTART Bishop** ⇒ bản vá chưa có hiệu lực.

---

## 4. KIỂM TOÁN BANG HỘI — ĐẦY ĐỦ, KHÔNG MẤT BYTE NÀO

### Đối chiếu từng kho (đĩa ↔ MySQL)
| Kho | Tệp BDB | Trên đĩa | MySQL | |
|---|---|---|---|---|
| `TongTable` | `TongDB/TongTable` | 1 (`TESTGAME`, 6860 B) | 1 | KHỚP từng byte |
| `MemberTable` | `TongMemberDB/MemberTable` | 2 (404 B) | 2 | KHỚP từng byte |
| `MoneyTable` | `TongMoneyDB/MoneyTable` | 0 | 0 | rỗng từ đầu |
| `ZhaoMuTable` | `TongZhaoMuDB/ZhaoMuTable` | 0 | 0 | rỗng từ đầu |
| `friend` | `dbfriend/friend` | 0 | 0 | rỗng từ đầu |

**Ba kho rỗng là RỖNG TỪ ĐẦU, có chứng minh** (không phải lập luận vòng tròn):
grep `TESTGAME`/`CaiBang`/`ACE`/`BANK` trong **nhật ký giao dịch Berkeley DB** (16 MB mỗi kho —
chỗ dữ liệu có thể "kẹt" nếu commit mà chưa checkpoint) → **0/0/0 ở cả 15 phép thử**;
trong khi cùng phép thử trên `TongDB/log` cho hàng chục nghìn kết quả ⇒ phép thử **có độ nhạy**.

Vì sao rỗng (đúng thiết kế): `MoneyTable` — quỹ bang kiểu JX2 nằm **ngay trong `TTongStruct`**
(`KTongJX2Relay.cpp:124-134`), đường JX1 cũ không dùng ⇒ **vĩnh viễn rỗng**.
`ZhaoMuTable` — hàng đợi tạm đơn xin vào bang, xoá ngay khi duyệt.
`friend` — chưa ai trên cây test lưu danh sách bạn.

### Máy chủ này thật sự có **1 bang / 2 thành viên** — 5 nguồn độc lập
1. BDB đang chạy: 1/2 · 2. Sao lưu lạnh 12:00: 1/2 (SHA-256 trùng 3/3) · 3. MySQL: 1/2
4. `jx1_role.role`: trong 1003 nhân vật chỉ **2** người có `tong_id ≠ 0`
5. `relay_log` 8 ngày: `dump N tong → GS` xuất hiện **119 lần "1 tong"**, **chưa bao giờ ≥ 2**

"Rất nhiều dữ liệu" là ĐÚNG — nhưng nó **gói trong 1 bản ghi 6860 byte**: quỹ 1.066.539.990,
Exp 73.440, BuildFund 983.190, BuildLevel 5, StoredOffer 5.200, 6 cụm tác phường,
16/16 dòng sổ sự kiện, liên minh, chiêu mộ. **Nhiều dữ liệu ≠ nhiều dòng.**

### 🔑 MỐC ĐỨT GÃY: 13/08/2026 07:43 — KHÔNG LIÊN QUAN MySQL
Bản vá port JX2→JX1 đổi `TTongStruct` **2368 → 6860 byte**.
`TONGDB.CPP:205-210, 268-274, 354-356` **từ chối thẳng** mọi bản ghi lệch cỡ.
⇒ Dữ liệu bang hội cũ đã không đọc được **từ 13/08, MỘT TUẦN TRƯỚC khi lên MySQL.**

### 🟢 Cứu được — khối JX2 thêm ở CUỐI struct
`2368` là **tiền tố đúng từng byte** của `6860`; `76` là tiền tố của `404` (76 = 32+32+4+4+4).
Chỉ cần **đệm 0 vào đuôi** là engine hiện tại đọc được. `friend` giữ nguyên định dạng (`rec_size=0`).

Các bộ cũ tìm thấy (chưa nhập, **chờ quyết định của chủ dự án**):
| Cây | Bang | Thành viên | Thời điểm |
|---|---|---|---|
| `E:\...\TESTLOFFF_ONLINE\bin\multiserver\New folder\` | **6** (ACE, BANK, Bee Bank, Tam Hoàng, Thiên, Thích) | 144 (+404 bạn bè) | 04/2026 |
| `D:\SourceVs22\TESTLOFFF_ONLINE\bin\multiserver\` ⚠️ trùng tên cây đang chạy | 5 | 77 | 02/2026 |
| `E:\SourceTuanLe\dulieugame\multiserver\` | 3 | 56 | 10/2025 |

### Không cần đưa lên MySQL
Cả `settings\tong\` (tongset.ini, tong_setting.ini, tong_level_data.txt, tongstunt_setting.txt):
mtime **2021-10-30**, mã nguồn chỉ mở chế độ `"rt"` (`KTongJX2Relay.cpp:998,1061`) ⇒ **cấu hình chỉ đọc.**
`jx2citywar/league/ladder.txt` **đã** ở `jx1_game.game_kv`, SHA-256 trùng khít cả 3.

---

## 5. BA BẢNG TRA CỨU MỚI — `ab65d0e3`

> Cả 3 là bảng **DẪN XUẤT**. Game **KHÔNG đọc**. Xoá rồi sinh lại lúc nào cũng được.
> Ngược lại: **sửa dữ liệu trong chúng KHÔNG làm đổi gì trong game.**

### 5.1 `jx1_role.role_item` — 33.746 dòng / 1003 nhân vật
Bung từng `TDBItemData` **233 byte** ra khỏi `role.role_blob`.

Bố cục byte (đã kiểm cộng lại đúng 233, `Lib/S3DBInterface.h:177-226`, `#pragma pack(push,1)`):
```
  0 iid              48  iparam[16] (64B)   164 iiduphong1..9 (36B)
  4 iequipclasscode  112 ilucky             200 iBaiTanPrice   ← GIÁ BÀY BÁN
  8 ilocal           116 iidentify          204 ilocksell
 12 ix               120 idurability        208 ilocktrade
 16 iy               124 igoldid            212 ilockdrop
 20 iequipcode       128 istacknum          216 imantle (1B)
 24 idetailtype      132 ienchance          217 ifortune
 28 iparticulartype  136 ipoint             221 iowner
 32 ilevel           140 iyear              225 irow
 36 iseries          144 imonth             229 iequipnaturecode
 40 iequipversion    148 iday               233 = hết
 44 irandseed        152 ihour
                     156 ilockbh / 160 igiomokhoa
```

🔴 **BẪY ĐÃ MẮC:** `ilocal` **KHÔNG** phải `INVENTORY_ROOM` mà là `nPlace`
(`ITEM_POSITION`, `Core/Src/GameDataDef.h:316`) — xem `KPlayerDBFuns.cpp:1002`.
Đọc nhầm bảng sẽ ra "0 món trang bị trên người", vô lý — đó là dấu hiệu để phát hiện.

| `vi_tri` | Nghĩa | Số món |
|---|---|---|
| 2 | mang trên người | 10.449 |
| 3 | hành trang | 23.291 |
| 4 | rương | 4 |
| 12 | ô phím tắt | 2 |

Kiểm chứng ngược: **33.746 = `SUM(n_item)`** của bảng `role`, khớp chính xác.
Thống kê hữu ích: **3.885 món đang bày bán trên sạp**, **14.907 món khoá bảo hiểm**.

Sinh lại: `python ToolsMySQL\sinh_role_item.py` (có `--chi-thu`).

### 5.2 `jx1_game.giao_dich` + `giao_dich_item` — 251 giao dịch / 402 lượt vật phẩm
Đọc thẳng từ `bin\server\dulieu\player_log\log_trade\*.txt`.
Mỗi giao dịch là bản ghi **NHIỀU DÒNG** (chủ / khách / vị trí / tiền / các món) → gom thành **1 hàng**.
Tên tệp: **`DD_MM_YYYY.txt`**.

### 5.3 `jx1_game.mua_shop` — 1.099 lượt mua
Đọc từ `log_ktc\*.txt`. Tên tệp: **`YYYY_MM_DD.txt`** ← **khác định dạng của log_trade, để ý!**
Tự kiểm: 1099 × 2 dòng = **2198** = đúng số dòng `log_ktc` trong `game_log` ⇒ không sót bản ghi.

Chạy lại cả hai: `python ToolsMySQL\nhap_giaodich_shop.py` (có `--chi-thu`).

### 5.4 Công cụ kèm theo
- `ToolsMySQL\tcvn3.py` — đổi TCVN3 → Unicode. Bảng lấy **verbatim** từ
  `<skill>\scripts\vn_to_octal.py` (`_TCVN3_CHARS` / `_UNICODE_CHARS`).
  Độ phủ trên nhật ký thật: **94,62%**; 5 byte còn lại (`0xD9` 1521 lần, `0x95`, `0xA0`,
  `0xFF`, `0xC4`) là **ký tự trang trí trong tên nhân vật**, không phải chữ Việt.
- `ToolsMySQL\chay_sql.py` — chạy tệp `.sql` qua pymysql (máy này **không có `mysql.exe` trong PATH**).
- `ToolsMySQL\schema_tracuu.sql` — schema của cả 3 bảng.

Mọi cột chữ giữ **HAI bản**: `VARBINARY` byte gốc (đối chiếu với game) + `_vn` `VARCHAR utf8mb4` (đọc được).

### 5.5 Hạn chế còn lại
`role_item` mới có **MÃ** (`detail_type` / `particular_type` / `cap`) chứ **chưa có TÊN** vật phẩm.
Tên nằm rải trong `bin\server\settings\item\*.txt` theo taxonomy riêng ⇒ là **một việc riêng**.
`giao_dich_item` và `mua_shop` **đã có tên** (nhật ký gốc ghi sẵn).

---

## 6. CẠM BẪY MỚI GẶP TRONG PHIÊN NÀY (đọc kỹ, đều đã mất thời gian vì chúng)

1. 🔴🔴 **KHÔNG gõ tay byte `\xNN` để thử bộ giải mã TCVN3.** Tôi mắc **2 lần** trong một phiên:
   cả hai lần chuỗi thử tự gõ đều sai byte, làm tưởng bảng chuyển hỏng trong khi nó đúng.
   Phải lấy byte **THẬT** từ tệp ra mà thử. *(Luật này đã có trong skill — vẫn mắc.)*
2. 🔴 **`pymysql` với `charset='latin1'` không ghi được cột Unicode** (`_vn`) — lỗi
   `UnicodeEncodeError: 'charmap' codec`. Phải dùng `charset='utf8mb4'`;
   cột `VARBINARY` vẫn an toàn nhờ `binary_prefix=True` (pymysql viết `_binary'...'`).
3. 🔴 **Xuống dòng khác nhau theo tệp** trong cùng thư mục Goddess:
   `DBTable.h`/`DBTable.cpp`/`DBBackup.cpp` = **CRLF**, `DBTable_MySQL.cpp` = **LF**.
   Neo vá phải đúng kiểu của từng tệp, nếu không `count()` ra 0.
   `DBBackup.cpp` kết thúc bằng `}` **không có dòng trống cuối**.
4. 🔴 **Thụt dòng nối trong `DBTable.h` là 5 TAB**, không phải 4 — đếm nhầm là neo hụt.
5. 🔴 **Tệp `.py` khai `# -*- coding: ascii -*-` mà lọt một chữ có dấu** ⇒ `SyntaxError: encoding problem`.
6. 🔴 **Bash tool: đường dẫn Windows có `\` bị rút mất trong `python -c`.** Dùng gạch XUÔI
   (`D:/GAMEDEVNEW/...`) hoặc ghi ra **tệp script** rồi chạy. *(Bẫy cũ, lại gặp.)*
7. 🔴 **`git commit --no-verify:$false` là cú pháp PowerShell, không phải git** ⇒ `unknown option`.
8. ⚠️ Trong phiên này người vận hành **cũng đang tự bật/tắt máy chủ** — hai lần phép thử của tôi
   bị vô hiệu vì Goddess bị tắt giữa chừng. **Kiểm tiến trình ngay trước khi kết luận.**

---

## 7. VIỆC CÒN TREO

### Bắt buộc
- [ ] **Restart Bishop** để bản vá `m_hQuitEvent` (`746b0dac`) có hiệu lực
- [ ] Chạy quyền **Administrator** để bật binlog (cấu hình đã ghi vào `my.ini`, chưa hiệu lực):
      `Restart-Service MySQL57 -Force`

### Nên làm
- [ ] Cắt tỉa định kỳ `game_log` và `role_history` — ước tính **~480 MB/ngày** ở `LichSuPhut=30`
- [ ] Chuyển ~10 script `.lua` sang dùng `KV_Set` / `KV_Get`
- [ ] Diệt hẳn cuộc đua khởi động: đảo `StartBackupTimer` xuống sau `OpenService`
      (cần khảo sát `ConnectTo` của Bishop trước — xem §1 "Nợ còn lại")

### Tuỳ chọn — **chờ chủ dự án quyết**
- ~~Thêm tên vật phẩm vào `role_item`~~ — **chủ dự án đã quyết định KHÔNG làm** (20/08). Khảo sát đang chạy đã bị dừng, không ghi gì. Nếu sau này muốn làm lại: bộ đối chiếu vàng là `jx1_game.mua_shop` + `giao_dich_item` (đã có SẴN cả tên lẫn mã); phải kiểm bộ khoá `(genre, detail, particular[, level, series])` có cho ra **duy nhất một tên** không — gán nhầm tên nguy hiểm hơn không có tên.
- [ ] Khôi phục **6 bang hội tháng 4** — viết bộ chuyển `2368 → 6860` (đệm 0 vào đuôi) và `76 → 404`
*(mục FilterText đã bị gỡ — xem §9, tôi đã ghi sai)*

---

## 8. THAM CHIẾU NHANH

**Cơ sở dữ liệu**
```
mysqld 127.0.0.1:3306  root / 123456   (bản 32-bit)
jx1_role : role(1003) role_item(33746) role_history(3011) role_anomaly(1)
           role_delete_log(0) role_save_fail(0) relay_kv(3)
           migrate_audit(2) migrate_reject(9) relay_delete_log(0)
jx1_game : game_log(1472954) giftcode(6018) game_kv(8) game_kv_history(113)
           baucua_balance(10) baucua_ledger(9) admin_command(0)
           giao_dich(251) giao_dich_item(402) mua_shop(1099)
account  : hệ tài khoản (vốn đã MySQL)
```

**Cổng**
```
5002 Sword3PaySys · 5003/5004/5005 S3Relay · 5011 Goddess · 5622+5632 Bishop · 3306 mysqld
```

**Đường dẫn**
```
Cây nguồn (repo)     D:\GAMEDEVNEW
Cây vận hành         E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\{server,multiserver}
Công cụ              D:\GAMEDEVNEW\ToolsMySQL\
Sao lưu lạnh 12:00   D:\_BACKUP_TRUOC_MYSQL_20260820_1200\
Nhật ký giao dịch    ...\bin\server\dulieu\player_log\{log_trade,log_ktc,...}
```

**Commit của phiên này**
```
1ee170d2  CoreServer: sửa đường dẫn settings\jx2*.txt
b119f973  Goddess/MySQL: gộp SaveStatInfo vào một giao dịch — sửa "báo failed" gateway 5632
746b0dac  Bishop: đặt lại m_hQuitEvent đầu Create() — hết "xác sống" khi bấm OK thử lại
ab65d0e3  ToolsMySQL: 3 bảng tra cứu — vật phẩm từng nhân vật, giao dịch, mua shop
```

---

## 9. ĐÍNH CHÍNH — `FilterText.dll` KHÔNG hề bị tắt

> Trong phiên này tôi có nói *"FilterText.dll là x86 nên GameServer x64 không nạp được
> ⇒ bộ lọc từ cấm đang TẮT"*. **CÂU ĐÓ SAI.** Đừng tin nó nếu gặp lại ở đâu.

Sự thật, tra từ mã nguồn:

| Tiến trình | Dùng thế nào | Kiến trúc | Thực tế |
|---|---|---|---|
| **Goddess** | `Goddess.cpp:306-307` gọi `g_libFilterText.Initialize()` **và** `g_fltRoleName.Initialize()`; `ClientNode.cpp:362+` lọc **tên nhân vật lúc tạo** | x86 | ✅ **ĐANG CHẠY BÌNH THƯỜNG** |
| GameServer | `GameServer.cpp:17` **chỉ khai báo biến toàn cục**, KHÔNG gọi `Initialize()` | x64 | không dùng ⇒ `bin\server` **không cần** DLL |
| Bishop | `Application.cpp:145` gọi `Initialize()`, hàm lọc thật ở `:151` **bị comment** | x86 | chỉ nạp DLL |
| S3Relay | `S3Relay.cpp:219` tương tự, `:225` bị comment | x86 | chỉ nạp DLL |
| Client | `S3Client/Ui/ChatFilter.cpp` | x86 | lọc chat phía người chơi |

**Tác dụng thật ở server: chặn tên nhân vật tục tĩu lúc tạo nhân vật.**
Danh sách từ cấm: `bin\multiserver\goddess_rolename.flt` — **363 dòng**, nạp từng dòng
qua `AddExpression()` (`RoleNameFilter.cpp:40-65`).

### 🔴🔴 CẢNH BÁO — hai tệp này là ĐIỀU KIỆN SỐNG của Goddess và Bishop

```cpp
// Goddess.cpp:306-311
if (!g_libFilterText.Initialize() || !g_fltRoleName.Initialize()) {
    MessageBox(NULL, "text filter's initing has failed", ...);
    return -1;                    // Goddess THOÁT
}

// Application.cpp:145-149  (Bishop)
if (!g_libFilterText.Initialize())
    return 0;                     // Bishop THOÁT IM LẶNG (MessageBox bị comment ở :147)
```

Xoá `FilterText.dll` hoặc `goddess_rolename.flt` khỏi `bin\multiserver` ⇒ **Bishop chết ngay
không một lời báo**, triệu chứng **y hệt** sự cố gateway ở §1. **ĐỪNG dọn hai tệp đó.**
(`RoleNameFilter.cpp:40-42`: không mở được tệp `.flt` là `return FALSE` ⇒ kéo theo Goddess `return -1`.)

⇒ **Không có việc gì phải làm.** Không cần build FilterText.dll bản x64.
