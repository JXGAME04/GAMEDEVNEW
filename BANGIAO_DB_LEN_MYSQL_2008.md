# BÀN GIAO — NÂNG DATABASE JX1 LÊN MySQL (đợt 1, 20/08/2026)

**Đọc file này TRƯỚC KHI GÕ bất cứ thứ gì liên quan tới kho nhân vật.**
Tài liệu nền: `PHANTICH_DB_LEN_MYSQL.md` (18/08) — **nhưng file này đính chính 7 điểm sai của nó**, xem mục 3.

| | |
|---|---|
| **Trạng thái** | GĐ0 (đo & sao lưu) + GĐ2b (công cụ di trú) + GĐ2 (tầng lưu trữ MySQL cho Goddess) **XONG và đã nghiệm thu**. Chưa cắt chuyển server thật. |
| **Kho đích đã chốt** | Server test tại `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE` (chủ game chọn làm trước) |
| **Chưa chạm tới** | GĐ1 (tiền Xu + giftcode), GĐ3 (S3Relay/bang hội), GĐ4 (file phẳng), các bản vá R2/R3/R4 phía Core |
| **Bản sao lạnh** | `D:\_BACKUP_TRUOC_MYSQL_20260820_1200\` — 14.559 tệp / 12,3 GB, đã đối chiếu số tệp + tổng byte + SHA256 của `roledb` |

---

## 1. VIỆC CÒN LẠI CỦA CHỦ GAME (1 lệnh)

`my.ini` đã được sửa để bật binlog nhưng **chưa có hiệu lực** vì restart dịch vụ cần quyền Administrator.
Mở PowerShell **Run as Administrator** và chạy:

```
Restart-Service MySQL57 -Force
```

Kiểm chứng sau khi chạy: `SELECT @@log_bin, @@binlog_row_image;` phải ra `1` và `MINIMAL`.
Từ lúc đó mới có khả năng **khôi phục dữ liệu tới từng giây** — trước nay hoàn toàn không có.

---

## 2. ĐÃ LÀM GÌ, VÀ BẰNG CHỨNG NÀO

### 2.1 Sao lưu lạnh (bắt buộc trước mọi thứ)
Server game vừa dừng lúc **11:57:30** (`GamePlayer.log`: "End of application!"), `roledb` **không bị khóa** — kiểm bằng `File.Open(..., FileShare.None)`, không phải bằng một lần `Get-Process`
*(bài học 17/08: đừng kết luận "server tắt rồi" từ một lần liếc tiến trình).*

Đã chép `bin\multiserver` + `bin\server` sang `D:\_BACKUP_TRUOC_MYSQL_20260820_1200\`.
Nghiệm thu: **7.056 + 7.503 tệp khớp**, **10.557.876.817 + 2.690.294.715 byte khớp**, SHA256 của `roledb` khớp tuyệt đối.
Cũng đã `mysqldump` DB `account` (283 KB) và lưu bản gốc `my.ini`.

### 2.2 Bộ đọc Berkeley DB độc lập — và tự phản biện chính nó
`ToolsMySQL\jx_bdb.py` đọc thẳng trang BTREE (không cần `libdb`, không mở môi trường, **không ghi một byte nào**).

Vì "đọc đúng" là mệnh đề phải chứng minh chứ không phải tin, `ToolsMySQL\kiem_cay.py` cài **đường đọc thứ hai**: duyệt cây từ trang gốc thay vì quét trang tuần tự, rồi so hai tập kết quả:

| | Kho test | Kho production |
|---|---|---|
| Cặp (khóa, giá trị) khi **quét trang** | 1.003 | 1.828 |
| Cặp khi **duyệt cây từ gốc** | 1.003 | 1.828 |
| Trang lá mồ côi / khóa trùng / giá trị lệch | 0 / 0 / 0 | 0 / 0 / 0 |

### 2.3 Bố cục bản ghi — chứng minh bằng dữ liệu, không theo tài liệu
Trên **2.822 bản ghi thật**:
- `dwDataLen == len(blob)` → đúng 100%
- `key == szName + NUL` (khớp **chính xác**, không phải `rstrip`) → đúng 100%
- `(dwItemOffset − dwTaskOffset) / 8 == nTaskCount` → đúng 100% ⇒ `TDBTaskData = 8`
- Chuỗi offset FightSkill → StateSkill → Task → Item liền mạch → đúng 100%
- `nItemCount` khớp offset → đúng 100% ⇒ `TDBItemData = 233`

⇒ `Lib\S3DBInterface.h` (581 dòng, `#pragma pack(push,1)` dòng 15) là bản đúng. Xác nhận lại cảnh báo của tài liệu nền.

### 2.4 🔴 LỖI MẤT DỮ LIỆU ĐÃ CHẶN ĐƯỢC: **hai dạng bản ghi, không phải một**

Bộ giải mã bản đầu của tôi **loại 3 người chơi thật**: `CayTien1`, `THIEULAMCUIBAP`, `TinheBanDam`.
Nguyên nhân: vùng vật phẩm của họ ra 229 byte, không chia hết 233.

Truy tới gốc thì đây **không phải dữ liệu hỏng** mà là **dạng bản ghi thứ hai**:

| Dạng | Sinh ra bởi | `dwDataLen` | CRC | Số lượng (production) |
|---|---|---|---|---|
| **A** | GameServer lưu định kỳ — `KSOServer.cpp` làm `dwDataLen += 4` rồi nối CRC32 | **đã gồm** 4 byte CRC | có | 1.816 |
| **B** | Bishop tạo nhân vật — `PlayerCreator.cpp:354` `dwDataLen = (BYTE*)pItemData - pData` | **không gồm** CRC | **không có** | **3** |

Ba nhân vật đó đều cấp 1, blob đúng 1002 byte = `745 header + 3 skill×8 + 1 vật phẩm×233`.
Không thể nhầm lẫn hai dạng: nếu cả `(X−4)` và `X` cùng chia hết 233 thì 4 phải chia hết 233 — vô lý.

**Cả `jx_role.py` lẫn `DBTable_MySQL.cpp` đều tự nhận diện hai dạng.** Ép cứng "luôn có CRC" = mất 3 người chơi, im lặng.

### 2.5 Công cụ di trú + nghiệm thu vòng tròn
`ToolsMySQL\nhap_role.py`. Nguyên tắc: chỉ đọc nguồn; **từ chối chạy nếu file nguồn đang bị khóa**; blob nhập **nguyên vẹn từng byte, KHÔNG tính lại CRC** (nhờ vậy mới chứng minh được "không mất dữ liệu" bằng hash); mọi bản ghi bị loại đều vào bảng `migrate_reject` kèm lý do + 64 byte đầu — **cấm loại im lặng**; lỗi giữa chừng thì `ROLLBACK`.

| Kho | Đọc được | Bị loại (rác thật) | Nhập | **Khớp từng byte khi đọc ngược** |
|---|---|---|---|---|
| production (ảnh 15/04) | 1.819 | 9 | 1.819 | **1.819 / 1.819** |
| test (đang chạy) | 1.003 | 0 | 1.003 | **1.003 / 1.003** |

9 bản ghi bị loại là rác thật: khóa nhị phân (`\x00`, `\xff\xff\x00`…), tên rỗng, cấp 0, `dwDataLen` lệch hẳn.

### 2.6 Ba rủi ro chết người của tài liệu — kiểm chứng ngay trên MySQL

1. **Va chạm HOA/thường** — `SELECT COUNT(*) - COUNT(DISTINCT LOWER(CONVERT(role_name USING latin1)))` = **27**.
   Đúng 27 bản ghi sẽ mất nếu dùng collation `_ci`; cả 27 còn nguyên nhờ `VARBINARY` + charset `binary`.
   Ví dụ thật: `CayTien` / `Caytien` / `caytien` — **ba người khác nhau**, khác tài khoản, cấp 120 / 120 / 141.
   ⚠️ `LOWER()` trên `VARBINARY` **không làm gì cả** — câu truy vấn kiểm tra phải `CONVERT(... USING latin1)` trước, nếu không sẽ báo 0 va chạm và ru ngủ người kiểm.
2. **Tên TCVN3** — 222 tên chứa byte ≥ 0x80, còn nguyên byte.
3. **Blob > 64 KB** — bản lớn nhất **70.646 byte** lưu và đọc lại nguyên vẹn (`MEDIUMBLOB`).

### 2.7 Tầng lưu trữ MySQL cho Goddess
`Sources\MultiServer\Goddess\DBTable_MySQL.cpp` (mới) cài lại lớp `ZDBTable` trên MySQL.
**`IDBRoleServer.cpp`, `DBBackup.cpp`, `ClientNode.cpp`, Bishop, GameServer, Core: KHÔNG sửa một dòng nào.**

Bốn ràng buộc bắt buộc đã giữ đúng:
1. `closeCursor` giải phóng bằng `free()` ⇒ mọi vùng key/data cấp phát bằng `malloc()`, cấm `new[]`
2. `next()` trả `false` thì con trỏ **tự hủy** (`IDBRoleServer.cpp:156/224/285/638`, `DBBackup.cpp:410` đều dựa vào quy ước này)
3. `DBBackup.cpp:885` **sửa thẳng** `cursor->data` rồi ghi lại ⇒ vùng đó phải là bộ nhớ riêng của cursor
4. `first()` trong bản Berkeley DB gọi `_search(false,NULL,0,-1)` và **vô tình đọc `index_db[-1]`** — do bố cục lớp, ô nhớ đó chính là `primary_db`. Nghĩa là `first()` duyệt **bảng chính**. Bản MySQL làm đúng hành vi thực tế đó.

Kèm 4 lỗ hổng của bản cũ được vá ngay trong tầng này:
- **V1** — `remove()` không còn xóa trắng: `INSERT INTO role_delete_log` **rồi mới** `DELETE`, trong một transaction
- **V2** — `add()` chặn **cả hai đầu** kích thước. Bản cũ (`IDBRoleServer.cpp:478`) chỉ chặn trên, nên `dwDataLen` = 0..3 hoặc **âm** lọt qua rồi `dwDataLen - 4` tràn ngược ⇒ đọc ngoài biên. Cổng 5011 **không có xác thực** nên đây là lỗ tấn công thật.
- **V3** — ghi thất bại không còn im lặng: vào bảng `role_save_fail`, giữ nguyên gói bị từ chối
- **V4** — mất kết nối: tự nối lại + chuẩn bị lại câu lệnh + thử lại đúng một lần

Và ba nguyên tắc an toàn: prepared statement có bind nhị phân (blob chứa `0x00`/`0x27`/`0x5C` ở vị trí bất kỳ), charset kết nối `latin1` (**tuyệt đối không `utf8mb4`**), tham số cuối `mysql_real_connect` **giữ = 0** (thêm `CLIENT_MULTI_STATEMENTS` là mở cửa cho `'; DROP TABLE ...`).

### 2.8 Nghiệm thu tầng C++ — 26/26 ĐẠT
`ToolsMySQL\test_dbtable\` gọi thẳng API mà Goddess dùng, đối chiếu với tệp tham chiếu xuất từ Berkeley DB:

| Phép kiểm | Kết quả |
|---|---|
| `search()` từng bản ghi, so **từng byte** | 1.003/1.003 |
| `first()`/`next()` duyệt toàn bảng | 1.003/1.003, khóa đều kết thúc NUL, 0 khóa trùng |
| `search(acc, index 0)` đếm nhân vật/tài khoản | 60/60 tài khoản |
| Tra tên không tồn tại | trả NULL |
| Tra bản chữ thường **không** khớp bản chữ hoa | 8/8 |
| `add()` ghi đè → đọc lại khớp từng byte | ĐẠT |
| `add()` **từ chối** `data_size` = 0 / 3 / **âm** / ngắn hơn header | 4/4, không bản ghi rác nào lọt |
| `remove()` → bản gốc nằm trong `role_delete_log`, byte y hệt | ĐẠT |
| Dựng lại kịch bản **mất 10 vật phẩm** → chuông báo + bản chụp cứu được | ĐẠT (xem mục 2bis) |

Xác nhận chéo đáng giá: **CRC32 của game (assembly viết tay, `Common\CRC32.c`) và `zlib.crc32` của Python cho kết quả trùng khớp trên toàn bộ 1.003 bản ghi.**

Chính bộ kiểm thử này lộ ra một khe hở do tôi tạo: gói `data_size = -8` không ghi được vào `role_save_fail` vì cột `data_len` là `INT UNSIGNED`. Đã đổi sang `INT` có dấu và chạy lại.

---

## 2bis. CHỐNG LỖI DỮ LIỆU — bốn lớp, không phải một

Câu hỏi của chủ game: *"có cách nào lưu dữ liệu lên MySQL chống được lỗi dữ liệu không"*.
Trả lời ngắn: **có, nhưng phải chia đúng hai loại lỗi** — bản thân MySQL chỉ chống được loại thứ nhất.

### Loại 1 — hỏng do đĩa / mất điện. InnoDB đã lo, và lo tốt hơn Berkeley DB hiện tại.

| Cơ chế | MySQL đang bật | Berkeley DB đang chạy |
|---|---|---|
| `innodb_doublewrite=1` — chống **trang ghi dở** (mất điện giữa lúc ghi 16 KB) | ✅ | ❌ không có |
| `innodb_flush_log_at_trx_commit=1` — commit là **đã xuống đĩa thật** | ✅ | ❌ `DB_TXN_NOSYNC` = *không* ép ghi (`DBTable.cpp:33`) |
| `innodb_checksum_algorithm=crc32` — phát hiện hỏng từng trang | ✅ | ❌ |
| Transaction nguyên tử | ✅ | ⚠️ chỉ `DB_AUTO_COMMIT` |
| Khôi phục tới từng giây (binlog) | ⏳ chờ restart | ❌ log bị xóa mỗi 60 phút |

⇒ **Chỉ riêng việc chuyển sang MySQL đã nâng độ bền lên**, không phải đánh đổi.
Khuyến nghị `flush_log_at_trx_commit=2` trong tài liệu 18/08 **KHÔNG áp dụng** — hiện đang là `1`,
tức bền hơn, mà tải chỉ ~18 lượt ghi/giây nên không cần hạ xuống.

### Loại 2 — game ghi đè một blob SAI. Đây mới là thứ làm mất đồ, và InnoDB không giúp được gì.

Nếu GameServer gửi lên một blob thiếu 10 vật phẩm, MySQL sẽ lưu **đúng** cái sai đó.
Ba cơ chế sau nhắm vào đúng loại này, đều nằm trong `DBTable_MySQL.cpp`:

1. **`role_history` — chụp ảnh TRƯỚC khi ghi đè.**
   Sao chép **ngay trong máy chủ** bằng `INSERT ... SELECT` nên blob không đi qua đường mạng lần nào.
   Chụp khi có báo động, hoặc định kỳ mỗi `LichSuPhut` phút.
2. **`role_anomaly` — chuông báo ngay lúc xảy ra.** So cột dẫn xuất cũ ↔ mới:
   `mat_vat_pham` (tụt ≥ `NguongMatVatPham` vật phẩm) · `tien_nhay` (lệch ≥ `NguongTien`) · `cap_tut`.
   Mỗi báo động có `history_id` trỏ thẳng tới bản chụp để quay lui.
3. **Kiểm CRC lúc đọc** — nhưng **chỉ báo động khi cột `crc_ok=1` mà byte đọc ra lại sai**,
   tức hỏng *sau* khi ghi. Nhờ vậy chuông không kêu oan với 82% bản ghi cũ vốn đã sai CRC sẵn.

**Cả ba nằm trong MỘT transaction cùng lượt ghi** — không thể có cảnh "đã chụp mà chưa ghi"
hoặc "đã ghi mà mất bản chụp".

**Đã dựng lại đúng kịch bản mất đồ để nghiệm thu** (phép kiểm số 9): ép ghi đè `BuiAnh731` từ 29 xuống 19 vật phẩm.

```
role_anomaly : mat_vat_pham | "so vat pham 29 -> 19 (tut 10)" | history_id=2
role_history : ver=0, data_len=7930, n_item=29, reason='bat thuong'
so blob trong ban chup voi ban goc  ->  GIONG HET TUNG BYTE
```

⇒ Nếu game làm mất đồ, hệ thống **biết ngay** và **vẫn còn bản gốc để trả lại**.

### Ba tham số cấu hình (`DataBase.ini`, section `[roledb]`)
`LichSuPhut=30` · `NguongMatVatPham=5` · `NguongTien=100000000` · `KiemCrcKhiDoc=1`.
Đặt `0` là tắt từng cơ chế. Mẫu đầy đủ: `ToolsMySQL\DataBase_roledb_mau.ini`.

### Còn thiếu gì để trọn vẹn
- **Khóa loại trừ ở tầng DB** (cột `locked_by`/`locked_at` đã có sẵn nhưng **chưa nối vào `ClientNode.cpp`**).
  Đây mới là thứ chặn đường **nhân đôi vật phẩm** khi đổi GameServer (rủi ro R4).
- **Khóa lạc quan `ver`**: cột đã tăng đúng mỗi lượt ghi, nhưng `ZDBTable::add()` không nhận tham số version
  nên chưa kiểm được `affected_rows`. Muốn dùng phải sửa `IDBRoleServer.cpp` — **để đợt sau**.
- **Dọn `role_history` định kỳ**: chưa có. Ước lượng 1.000 người × 10 KB × 48 bản/ngày ≈ 480 MB/ngày
  nếu để `LichSuPhut=30`. **Phải thêm việc xóa bản cũ trước khi chạy thật lâu dài.**

---

## 3. ĐÍNH CHÍNH TÀI LIỆU `PHANTICH_DB_LEN_MYSQL.md` (18/08)

| Tài liệu 18/08 nói | Sự thật đã kiểm chứng |
|---|---|
| "1.802 nhân vật / 1.355 tài khoản / 19,4 MB" | Đọc thẳng Berkeley DB: **1.828 khóa, 1.819 nhân vật hợp lệ, 1.367 tài khoản, 18,7 MB**. Con số cũ đọc từ `.bak` nên thiếu. |
| "Blob max 69.257 byte" | **70.646 byte** |
| "`GetRoleInfo` KHÔNG kiểm CRC" — hàm ý Goddess không kiểm CRC | `GetRoleInfo` đúng là không kiểm, nhưng **`SaveRoleInfo` CÓ kiểm CRC32 và TỪ CHỐI lưu khi sai** (`IDBRoleServer.cpp:489-507`), ghi vào tệp `crc_error`. |
| "Chặn `dwDataLen` ở `:475`, `add` ở `:576`, chặn trùng tên `:549`" | Số dòng thật: **478 / 579 / 552**. Tệp đã sửa 18/08 21:23, **sau** khi tài liệu lập lúc 15:55. |
| "`nTaskCount` kiểu BYTE là lỗi thật" | Đúng về kiểu, nhưng trên **2.822 bản ghi** `nTaskCount` **luôn khớp** giá trị suy từ offset, tối đa 71. Không phải rủi ro thực tế. |
| Ngầm hiểu mọi bản ghi đều có 4 byte CRC cuối | **Sai** — xem mục 2.4. Có 3 bản ghi dạng B. |
| "`libmySQL.dll` trong bin là x86 — dùng được cho Goddess" | Đúng. Bổ sung: **`mysqld` đang chạy cũng là bản 32-bit** ⇒ trần bộ nhớ ~2 GB, **khuyến nghị `innodb_buffer_pool_size = 2 GB` của tài liệu là KHÔNG khả thi**. Đang để 128 MB, thừa cho 19 MB dữ liệu. |

**Một điểm tôi từng nói sai trong phiên và đã tự đính chính:** Goddess **CÓ** hạ chữ thường tên tài khoản, nhưng bằng vòng lặp viết tay `*ptr += 'a'-'A'` (dòng 482-487 lúc tạo, 592-596 lúc liệt kê) nên `grep tolower|strlwr` **không thấy gì**. Số đo vẫn đứng: **0/1.816 blob có `caccname` chữ hoa** ⇒ bẫy R5 đang ngủ, di trú không phải chọn giữa "giữ nguyên" và "hạ chữ".

---

## 4. LỖ HỔNG MỚI PHÁT HIỆN (tổ phản biện mã nguồn, 10 tác tử) — CHƯA VÁ

Ngoài V1–V4 đã vá trong tầng MySQL, còn 5 lỗ nằm ngoài phạm vi `DBTable`:

1. 🔴 **Rò con trỏ Berkeley DB trong `GetRoleListOfAccount`** (`IDBRoleServer.cpp:609-639`): thoát vòng vì `count < nMaxCount` sai thì **không ai gọi `closeCursor`** ⇒ `dbcp` không `c_close`, cursor giữ khóa đọc. Xảy ra khi tài khoản có nhiều nhân vật hơn `nMaxRoleCount`.
2. 🔴 **Luồng phát hiện deadlock BỊ COMMENT** (`IDBRoleServer.cpp:67-80`) ⇒ `deadlock()` chưa bao giờ chạy. Cộng với mục 1 là combo nguy hiểm. *(Với MySQL thì InnoDB tự lo, nên mục này hết ý nghĩa sau khi cắt chuyển.)*
3. 🔴 **Trần thông lượng thật của Goddess KHÔNG nằm ở mutex `m_csCL`** như tài liệu nói, mà ở `ClientNode.cpp:265-283`: `Process()` chỉ lấy **MỘT** `CBuffer` mỗi lần gọi, trong khi bên sản xuất (`Goddess.cpp:191-196`) vét sạch bằng `while`. Cộng `Sleep(1)` cách vòng ⇒ ~1 gói / 0,5–8 ms mỗi node, hàng đợi phình không giới hạn khi tải cao.
   *(Cũng bác luôn khẳng định "busy-spin đốt 100% CPU" — `if (g_nDBEngineLoop & 0x1) Sleep(1)` ngủ ở **mọi giá trị lẻ**, tức cứ 1 vòng lại ngủ 1 vòng.)*
4. ⚠️ **`GetRoleInfo` `memcpy` không kiểm sức chứa** (`IDBRoleServer.cpp:454`) ⇒ bản ghi cực đại tràn heap ~10 byte.
5. ⚠️ **Hai quy ước độ dài trong CÙNG `KSOServer.cpp`**: đường lưu thường chốt `nDataLen` **trước** `+= 4` (3381/3382), còn đường chuyển server làm **ngược lại** (2647/2648). Hiện vô hại vì `_SaveRoleInfo` không đọc `nDataLen`, nhưng ai "làm cho đúng" mà tin `nDataLen` sẽ cắt cụt 4 byte CRC của **mọi** bản ghi.

Đính chính thêm với R3 của tài liệu: mã kết quả lưu **CÓ** được đọc ở `KSOServer.cpp:2182` (để bot khóa lại), và `SAVE_IDLE` đặt vô điều kiện nên nhịp 30 giây kế tiếp **vẫn gửi lại blob**. Phát biểu đúng là *"không thử lại tức thì, không log, không cảnh báo"* — chứ không phải "mất trắng phiên chơi".

---

## 5. CÁCH QUAY LUI (kiểm 30 giây, không mất dữ liệu)

Kho Berkeley DB **chưa bị đụng một byte nào** — công cụ chỉ đọc, và đọc trên bản sao.

1. `Goddess.vcxproj`: xóa 3 dòng `<ExcludedFromBuild>` của `DBTable.cpp`, xóa dòng `<ClCompile Include="DBTable_MySQL.cpp" />`
2. Bỏ `ROLEDB_MYSQL` khỏi `<PreprocessorDefinitions>` (2 chỗ) — `DBTable.h` tự quay về gọi `dbenv->lock_detect`
3. Build lại, chép `Goddess.exe` về

Bản gốc còn nguyên tại `DBTable_BDB_goc.h.bak` / `DBTable_BDB_goc.cpp.bak` và `Goddess.vcxproj.bak_truoc_mysql`.

---

## 6. BƯỚC TIẾP THEO (theo thứ tự)

1. **Chủ game restart MySQL57** (mục 1) → có binlog
2. **Vá phía Core trước khi cắt chuyển** — R2 (`CanSave` phát lại vô hạn), R3 (đọc mã kết quả lưu), R4 (`ClientNode.cpp:717` gán khóa **vô điều kiện** ⇒ đường nhân đôi/rollback vật phẩm có thật). Đây mới đúng là ba thứ gây "đúp đồ / mất đồ", và **không liên quan gì tới MySQL**.
3. **Chạy thử Goddess bản MySQL trên server test** — cần bấm nút khởi động trong giao diện Goddess (không tự động hóa được), rồi đăng nhập thử: một tài khoản có tên tiếng Việt có dấu, một cặp tên chỉ khác HOA/thường, một nhân vật rương đầy.
4. **GĐ1 — tiền Xu + giftcode** (`BauCua.cpp`, `GiftCodeManager.cpp`). Dữ liệu bé, độc lập, giá trị cao.
5. **GĐ3 — S3Relay/bang hội**. Nhớ: khóa bảng `friend` **không** có byte NUL còn các bảng Tong **có**; `TMoneyStruct.m_dwMoney` là `DWORD` **không dấu** — lên MySQL phải `BIGINT` **CÓ DẤU**, nếu không tái lập đúng lỗi "quỹ bang tràn số âm → 4,29 tỷ".
6. **DB `account`** — chưa quyết. Hiện trạng đã xác minh: **không có primary key/index nào**, 6 cột `varchar(0)` (gồm `Account_Habitus.iClientID` và `iLeftSecond` — chỉ chứa được chuỗi rỗng), `dLoginDate` là `varchar(19)` trong khi mã sống làm `TIMEDIFF()` trên nó, và **VIEW `View_AccountMoney` mà `S3RelayServer` cần thì KHÔNG tồn tại**. Đã xác minh hồ sơ Navicat tên "Game" trỏ đúng vào `localhost:3306` này, chỉ có một mysqld duy nhất.

---

## 7. DANH MỤC TỆP

| Tệp | Vai trò |
|---|---|
| `ToolsMySQL\jx_bdb.py` | Đọc Berkeley DB BTREE thuần Python, chỉ đọc |
| `ToolsMySQL\kiem_cay.py` | **Phản biện** bộ đọc trên bằng đường duyệt cây từ gốc |
| `ToolsMySQL\jx_role.py` | Giải mã blob `TRoleData` + bộ đọc `.bak` có 3 phép kiểm chéo |
| `ToolsMySQL\do_kho.py` | Đo một kho: kích thước, va chạm tên, CRC, TCVN3 |
| `ToolsMySQL\schema_role.sql` | Schema `jx1_role` (5 bảng) |
| `ToolsMySQL\nhap_role.py` | Công cụ di trú + nghiệm thu vòng tròn |
| `ToolsMySQL\xuat_ref.py` | Xuất tệp tham chiếu cho bộ kiểm thử C++ |
| `ToolsMySQL\bat_binlog.py` | Thêm khối binlog vào `my.ini` (chạy lại được nhiều lần) |
| `ToolsMySQL\test_dbtable\` | Bộ kiểm thử C++ 23 phép cho `ZDBTable` bản MySQL |
| `Sources\MultiServer\Goddess\DBTable_MySQL.cpp` | **Tầng lưu trữ MySQL** |
| `Sources\MultiServer\Goddess\DBTable_BDB_goc.*.bak` | Bản Berkeley DB gốc, để quay lui |
| `D:\_TEST_GODDESS_MYSQL\` | Hộp cát chạy thử Goddess, tách biệt khỏi server thật |

**Cấu hình kết nối** nằm ở section `[roledb]` trong `DataBase.ini` (Server / Port / DataBase / User / PassWord).
Section `[role]` cũ ghi `Port=1433` là **rác chết** — đừng dùng.

---

# ĐỢT 2 (chiều 20/08) — vá R2/R3/R4, DB `account`, tiền Xu, và ĐÃ ĐỒNG BỘ SANG MÁY TEST

## Đ2.1 Đã deploy sang `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE`

| Tệp | Cũ | Mới | Bản lưu để quay lui |
|---|---|---|---|
| `multiserver\Goddess.exe` | 5.331.968 | **2.379.776** (nền MySQL) | `Goddess.exe.bak_truoc_mysql_2008` |
| `server\CoreServer.dll` | 17.866.752 | **17.868.800** | `CoreServer.dll.bak_truoc_mysql_2008` |
| `multiserver\libmysql.dll` | (chưa có) | 6.500.424 (x86) | — |
| `multiserver\DataBase.ini` | — | thêm section `[roledb]` | `DataBase.ini.bak_truoc_mysql_2008` |

Kiểm trước khi chép: không tiến trình server nào chạy · `roledb` không bị khoá · hash `roledb` **vẫn khớp** bản đã nhập.
Kiểm sau khi chép: chạy bộ kiểm thử **ngay trong thư mục server thật** (để nó đọc đúng `DataBase.ini` vừa sửa) → **26/26 ĐẠT**.
Nghiệm thu cuối đối chiếu với `roledb` sống: **1003/1003 khớp từng byte**.

**Kho Berkeley DB `database\` vẫn nguyên vẹn, chưa bị chạm một byte.** Quay lui = đổi lại 2 tệp `.bak_truoc_mysql_2008`.

## Đ2.2 Ba bản vá chống đúp đồ / mất đồ

**R4 — khoá nhân vật (`ClientNode.cpp`)**, ba lỗi thật:
- Đặt khoá **vô điều kiện** (dòng cũ 717): GameServer thứ hai chỉ cần gửi `c2s_roleserver_lock` là **cướp được khoá**; lượt lưu cuối của máy cũ bị từ chối im lặng, máy mới nạp blob **cũ** ⇒ rollback/nhân đôi vật phẩm. Nay: vẫn cho chuyển chủ (đổi GameServer cần thế) nhưng **ghi nhật ký `LOCK_TAKEOVER`** và nhớ chủ cũ.
- Mở khoá **không kiểm chủ**: bất kỳ kết nối nào cũng mở được khoá của người khác. Nay chỉ chủ sở hữu (hoặc khoá đã chết `-1`) mới mở được.
- `UnlockAllRole` đặt `-1` thay vì xoá ⇒ nhân vật đó **không ai lưu được nữa** cho tới khi có người khoá lại, và map chỉ phình thêm sau mỗi lần rớt kết nối. Nay xoá hẳn.

**R3 — gói lưu bị từ chối không còn im lặng**: `_SaveRoleInfo` trước đây chỉ `nResult = 0`. Nguy hiểm nhất là lượt lưu **cuối** lúc đăng xuất/đổi máy chủ (`bLeave=true`) — sau đó không còn lượt nào để tự chữa, **mất trọn phiên chơi**. Nay ghi nhật ký `SAVE_REJECT` và **cách ly nguyên gói** vào bảng `role_save_fail` qua `QuarantineRoleInfo()`.

**R2 — tài liệu 18/08 nói SAI, đã bác bằng mã nguồn.** Không có "bão ghi": `KPlayerSet.cpp:1102` AND thêm cổng **30 giây/người**, và `Save()` cập nhật `m_ulLastSaveTime` (`KPlayer.cpp:1079`) ⇒ tối đa 1 lượt/người/30 giây. **Nên tôi KHÔNG đụng vào `CanSave()`.**
Nhưng ngay cạnh đó có **lỗi thật**: `break` nằm **ngoài** nhánh `if (Save())`. Một người chơi có `Save()` luôn thất bại sẽ được chọn lại mỗi tick rồi `break` ⇒ **cả máy chủ không còn ai được lưu**. Đã sửa: thất bại thì đi tiếp người kế, trần 8 lần thử/tick, vẫn giữ đúng 1 lượt lưu **thành công**/tick.

## Đ2.3 DB `account` — làm phần an toàn, cố ý KHÔNG làm phần nguy hiểm

Đo được: 1003 dòng `account_info` nhưng **chỉ 13 tài khoản thật** — 990 dòng là **rác thuần túy** (mọi cột NULL, kể cả `iid` và `cPassword`). `account_habitus` tương tự (880/893).

**Đã làm:** sao lưu `mysqldump` trước khi động · chép 990+880 dòng rác sang bảng `*_rac_20260820` để tham chiếu · thêm chỉ mục `cAccName` cho cả hai bảng (trước nay **mỗi lần đăng nhập quét toàn bảng**), `iClientID`, `cServerName` · tạo **VIEW `View_AccountMoney`** mà `S3RelayServer` cần (trước nay **không tồn tại**) — đã chạy thử đúng câu SQL thật của nó, giờ trả về dòng thay vì lỗi.

**Cố ý KHÔNG làm — và đây là quyết định kỹ thuật, không phải bỏ sót:**
- **Không xoá 990 dòng rác.** `cAccName IS NULL` không bao giờ khớp tra cứu nên chúng vô hại; xoá thì có rủi ro mà lợi ích chỉ là thẩm mỹ.
- **Không hồi sinh cột tính giờ chơi.** `account_habitus` **thiếu hẳn cột `dLoginDate`** mà code ghi vào, còn `iClientID`/`iLeftSecond` là `varchar(0)` — nghĩa là toàn bộ SQL tính giờ **đang thất bại từ lâu**. Nhưng `dEndDate` của mọi tài khoản là **2070**, server chạy theo mô hình chơi không giới hạn. Bật lại đường trừ giờ là **đổi luật chơi** và có thể khoá người chơi ra ngoài. Muốn làm phải là quyết định của chủ game, có kiểm thử riêng.

## Đ2.4 Tiền Xu sòng bạc — chống mất, nhưng KHÔNG lên được MySQL

🔴 **Chặn cứng:** `CoreServer.dll` là **x64**, mà máy này **chỉ có thư viện MySQL bản x86** (cả MySQL Server 5.7 lẫn Connector C++ 8.0 đều 32-bit). Không link được. Muốn đưa tiền Xu/giftcode lên MySQL phải **cài MySQL Connector/C bản x64** trước.

Nên đợt này tôi làm đúng việc mà MySQL sẽ làm, nhưng trên tệp (`BauCua.cpp`):
- `saveDeposits()` trước đây dùng `std::ofstream` ghi thẳng — **mở tệp là cắt trắng ngay**, sập điện giữa chừng = mất sạch số dư. **Bằng chứng đã xảy ra thật: `bin\server\deposits.json` hiện đang 0 byte.** Nay: ghi ra `.tmp` → `fflush` + `_commit` ép xuống đĩa → giữ bản cũ thành `.bak` → `MoveFileEx(REPLACE_EXISTING|WRITE_THROUGH)` đổi chỗ nguyên tử.
- `loadDeposits()` **không có try/catch**, mà nó chạy trong hàm dựng của biến toàn cục `g_BauCua` ⇒ JSON hỏng là **ném ngoại lệ lúc nạp DLL, chết tiến trình**. Nay bắt ngoại lệ và tự động thử bản `.bak`.
- Thêm **sổ cái chỉ-ghi-thêm** `baucua/deposits.log`: mất tệp chính vẫn dựng lại được số dư.
- Tự tạo thư mục `baucua\` (trước không tạo ⇒ ghi hỏng **im lặng**).

## Đ2.5 S3Relay (bang hội/bạn bè) — DỪNG LẠI CÓ CHỦ Ý, chưa chuyển

Đã viết xong `S3Relay\DBTable_MySQL.cpp` + schema `relay_kv` (đã nạp vào MySQL, đang rỗng, chờ đợt sau). Nhưng **không deploy**, vì khi build lộ ra một ràng buộc nguy hiểm:

`S3Relay.vcxproj` đặt `RuntimeLibrary = MultiThreaded` (/MT) **nhưng lại** `IgnoreSpecificDefaultLibraries = libcmt.lib;msvcrt.lib;msvcrtd.lib`. Kết quả: **thư viện Berkeley DB tĩnh là thứ DUY NHẤT kéo C runtime vào cả tiến trình**. Bỏ `DBTable.cpp` ra khỏi build là libdb không còn được liên kết ⇒ **1.688 lỗi `unresolved external symbol "operator new"`**. Số liệu đối chiếu: bản gốc `libdb181` xuất hiện 504 lần trong nhật ký liên kết, bản MySQL chỉ 2 lần.

"Sửa" việc này nghĩa là **đổi CRT cho toàn bộ S3Relay** — đúng loại thay đổi đã từng gây hỏng heap trong dự án này. Tôi đã **trả S3Relay về Berkeley DB và build lại xác nhận 0 lỗi**, đường quay lui nguyên vẹn.

Hai lỗi thật phát hiện được trong lúc khảo sát, ghi lại để đợt sau xử:
- 🔴 **`CTongDB::DelTong` TREO VÔ TẬN**: `TONGDB.CPP:181` viết `while(!m_MemberTable->remove(aTongName, aKeySize, 0)) {}` nhưng `remove()` **bỏ qua tham số index** (`DBTable.cpp:251`, luôn xoá theo khoá chính). Xoá bang hội = lặp vô tận chiếm 100% CPU một lõi. (Bản MySQL đã viết sẽ gỡ luôn lỗi này vì hiện thực đúng ngữ nghĩa khoá phụ.)
- Kích thước struct đã đối chiếu với **dữ liệu thật**: `TTongStruct`=6860, `TMemberStruct`=404 — khớp chính xác. `TZhaoMuStruct`=76 có **1 byte đệm ở offset 71** không bao giờ được xoá trắng ⇒ bản ghi không tất định.

## Đ2.6 Việc còn lại

1. **Chủ game chạy (Run as Administrator):** `Restart-Service MySQL57 -Force` → bật binlog.
2. **Bật Goddess trên máy test và đăng nhập thử.** Goddess là ứng dụng có giao diện, phải bấm nút khởi động — tôi không tự động hoá được. Nên thử: một tài khoản có tên nhân vật **tiếng Việt có dấu**, một cặp tên chỉ khác **HOA/thường**, một nhân vật **rương đầy**.
3. Cài **MySQL Connector/C bản x64** nếu muốn đưa tiền Xu + giftcode lên MySQL.
4. Dọn `role_history` định kỳ trước khi chạy dài ngày (ước 480 MB/ngày với `LichSuPhut=30`).
5. S3Relay: quyết định về CRT rồi mới làm tiếp.

---

# ĐỢT 3 — HAI VẬT CẢN ĐÃ GỠ, S3RELAY ĐÃ LÊN MySQL

Chủ game hỏi: *"mục tiêu đưa toàn bộ lên MySQL, có cách nào làm mà phải an toàn không?"*
**Có.** Cả hai vật cản nêu ở Đợt 2 đều đã gỡ được, và **đã kiểm chứng bằng build thật**, không phải suy đoán.

## Đ3.1 Vật cản 1 — thư viện MySQL x64: ĐÃ CÓ SẴN TRÊN MÁY

Không cần tải gì. MySQL Installer để lại **`mysql-5.7.44-winx64.msi` (117 MB)** trong
`C:\ProgramData\MySQL\MySQL Installer for Windows\Product Cache\`.

Đã bung bằng `msiexec /a` (**administrative install = chỉ giải nén, không cài dịch vụ, không đụng
mysqld đang chạy**) ra `D:\_MYSQL_X64_EXTRACT\`. Kiểm bằng `dumpbin /headers`:

| Tệp | Kiến trúc |
|---|---|
| `libmysql.dll` | **x64** (7.781.448 byte) |
| `libmysql.lib` | **x64** (`8664 machine`) |
| `mysqlclient.lib` | **x64** |
| `mysql.h` | (header, dùng chung) |

⇒ `CoreServer.dll` (x64) **nối được MySQL**. Đây là **chìa khoá chính**: nó mở đường cho tiền Xu,
giftcode, và **toàn bộ nhóm file phẳng** (`jx2citywar/jx2ladder/jx2league`, `StatData.dat`,
~10 tệp `.lua`, `player_log`, `username_kick`) — vì tất cả đều nằm trong CoreServer.

*Lưu ý: client x64 nói chuyện với `mysqld` 32-bit hoàn toàn bình thường — giao thức MySQL là giao
thức mạng, kiến trúc hai đầu độc lập nhau.*

## Đ3.2 Vật cản 2 — bẫy CRT của S3Relay: gỡ bằng "tệp neo", KHÔNG đụng cấu hình CRT

Nhắc lại vấn đề: `S3Relay.vcxproj` đặt `/MT` nhưng lại `IgnoreSpecificDefaultLibraries = libcmt.lib`,
nên **thư viện Berkeley DB tĩnh là thứ duy nhất kéo C runtime vào cả tiến trình**. Bỏ `DBTable.cpp`
là mất luôn CRT ⇒ 1.688 lỗi `unresolved operator new`.

Cách gỡ **không cần đổi CRT**: thêm `_neo_libdb.cpp` — một tệp 25 dòng gọi `db_version()`, một hàm
vô hại của Berkeley DB, **không bao giờ chạy lúc thi hành**. Nhờ nó trình liên kết vẫn phải rút
`libdb` vào, nên môi trường liên kết giữ **y hệt** bản đang chạy.

Kết quả build, đối chiếu số liệu:

| | Bản gốc (Berkeley DB) | Bản MySQL + tệp neo |
|---|---|---|
| Lỗi biên dịch/liên kết | 0 | **0** |
| `libdb181` trong nhật ký liên kết | 504 lần | **504 lần** |
| Sinh ra `S3Relay.exe` | có | **có** (4.405.760 byte) |

Con số 504 giống hệt nhau là bằng chứng môi trường CRT **không đổi một chút nào**.

## Đ3.3 S3Relay đã lên MySQL và ĐÃ DEPLOY

- Bảng `relay_kv` gộp cả 5 kho, blob giữ **nguyên vẹn từng byte**, kèm cột `idx0` thay khoá phụ và
  `money` (**BIGINT CÓ DẤU** — chống tái lập lỗi "quỹ bang tràn số âm → 4,29 tỷ").
- Di trú: **3/3 bản ghi khớp từng byte** (1 bang `TESTGAME` 6860 byte, 2 thành viên 404 byte;
  friend/money/zhaomu rỗng). Khoá phụ kiểm đúng: cả hai thành viên trỏ về `TESTGAME`.
- Đã chép `S3Relay.exe` sang máy test (bản cũ giữ `.bak_truoc_mysql_2008`) và thêm section `[relaydb]`.
- **Gỡ luôn lỗi treo `CTongDB::DelTong`**: bản MySQL hiện thực đúng ngữ nghĩa khoá phụ nên
  `while(!remove(tong, size, 0)){}` thoát đúng cách thay vì lặp vô tận.

⚠️ **Một chi tiết bắt được nhờ kiểm chứng ngược:** bảng đặt `DEFAULT CHARSET=binary` nên
`VARCHAR(24)` **thành `VARBINARY(24)`**, MySQL trả về `bytes` chứ không phải chuỗi. Lần đối chiếu
đầu báo 0/3 khớp chỉ vì so sai kiểu. Nếu không có bước kiểm chứng ngược thì đã tưởng dữ liệu hỏng.

## Đ3.4 Còn lại gì để "toàn bộ lên MySQL"

Sau đợt 3, phần chưa lên MySQL **chỉ còn nhóm nằm trong CoreServer (x64)** — và nay đã có thư viện x64:

| Nhóm | Nội dung | Ghi chú |
|---|---|---|
| Tiền Xu sòng bạc | `BauCua.cpp` | đã chống mất bằng ghi nguyên tử; lên MySQL được rồi |
| Giftcode | `GiftCodeManager.cpp` + 2 tệp `.lua` | có 3 đường song song, chỉ 1 đi qua C++ |
| Công Thành Chiến / xếp hạng / liên minh | `jx2citywar.txt`, `jx2ladder.txt`, `jx2league.txt` | đã có khuôn `.tmp`+`MoveFileEx` |
| Bảng xếp hạng | `StatData.dat` | kèm việc bỏ đường ghi ngược vào roledb |
| ~10 tệp `.lua` server tự ghi đè | ngân lượng/tiền vàng sòng bạc, boss, sự kiện | cần 4 hàm KV cho Lua |
| Nhật ký người chơi | `dulieu\player_log\**` | bảng phân vùng theo tháng |

**Thứ tự đề xuất (rủi ro tăng dần):** giftcode → Công Thành Chiến/xếp hạng/liên minh → tiền Xu →
`StatData.dat` → tệp `.lua` qua bảng KV → nhật ký người chơi.
