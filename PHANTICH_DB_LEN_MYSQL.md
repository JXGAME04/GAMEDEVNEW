# PHÂN TÍCH: ĐƯA TOÀN BỘ DATABASE JX1 LÊN MySQL

**Nguồn mã:** `D:\GAMEDEVNEW\Sources`
**Server đang chạy:** `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\{server, multiserver}`
**Ngày lập:** 18/08/2026
**Tính chất:** CHỈ PHÂN TÍCH — không sửa một dòng mã nào. Báo cáo này không chứa patch.

---

## 1. TÓM TẮT CHO CHỦ GAME

**Hiện trạng.** Hệ thống đang dùng **ba loại kho khác nhau**, không phải một:

1. **Tài khoản** (đăng nhập, mật khẩu, giờ chơi, đồng xu) — **ĐÃ Ở MySQL RỒI**, chạy thật, không cần làm gì thêm ngoài dọn dẹp. Ba bảng: `Account_info`, `Account_Habitus`, `ServerList`, cộng một **VIEW** tên `View_AccountMoney`.
2. **Nhân vật, bang hội, bạn bè** — đang nằm trong **Berkeley DB**, một loại kho nhúng chạy thẳng trong tiến trình, không phải máy chủ CSDL. Đây là phần chính phải chuyển. Gồm 6 kho: `roledb` (nhân vật, do Goddess.exe giữ) và 5 kho của S3Relay.exe (bạn bè + 4 kho bang hội).
3. **Một mớ dữ liệu người chơi nằm trong file thường** — đáng chú ý nhất là **tiền Xu gửi trong sòng bạc** (`baucua\deposits.json`), giftcode đã dùng, quyền sở hữu thành trong Công Thành Chiến, và một số bảng Lua mà server tự ghi đè.

**Khối lượng thật.** Kho nhân vật production có **1.802 nhân vật / 1.355 tài khoản**, tổng dữ liệu **19,4 MB**, bản ghi lớn nhất **69.257 byte**. (Con số "1.003 nhân vật / 2,9 MB" mà mọi người hay trích là của **máy test**, trong đó 1.000 là bot cấp 10 — không dùng để tính toán được.)

**Rủi ro lớn nhất — theo đúng thứ tự nguy hiểm:**

| # | Rủi ro | Hậu quả nếu bỏ qua |
|---|---|---|
| 1 | **Tên nhân vật là byte tiếng Việt TCVN3, và phân biệt HOA/thường** | Nếu tạo cột MySQL kiểu chữ thông thường: **mất 27 nhân vật** vì trùng khóa, và **205 tên có dấu bị hỏng**. Đã đo thật, không phải phỏng đoán. |
| 2 | **Đường ghi không có phanh (backpressure)** | Khi DB chậm hơn hiện tại, game **tự sinh thêm lệnh ghi** → càng chậm → sập. MySQL chắc chắn chậm hơn Berkeley DB hiện tại vì hiện tại đang chạy chế độ "không ép ghi xuống đĩa". |
| 3 | **Kết quả ghi bị vứt bỏ** | Game nhận trả lời "lưu xong" mà **không hề đọc mã thành/bại**. Với Berkeley DB thì hầu như không bao giờ lỗi nên bug này ngủ yên 20 năm. Với MySQL, mọi lỗi tạm thời sẽ biến thành **"mất phiên chơi trong im lặng"**. |
| 4 | **Khóa nhân vật không phải khóa loại trừ** | Đây là đường **rollback / nhân đôi vật phẩm** có thật khi chuyển GameServer. Không liên quan gì đến MySQL, nhưng phải vá cùng đợt. |
| 5 | **83,7% bản ghi hiện có CRC32 SAI** | Nếu công cụ nhập liệu kiểm CRC trước khi nhận, sẽ **loại oan 1.508/1.802 nhân vật**. |

**Khối lượng việc.** Nếu làm theo phương án an toàn (giữ nguyên định dạng bản ghi, chỉ thay tầng lưu trữ): khoảng **6–8 file phải sửa, ~1.200–1.800 dòng**, cộng một công cụ di trú viết mới. Nếu bung bản ghi ra thành cột quan hệ thật: gấp 5–7 lần và **rủi ro cao hơn hẳn** — xem mục 4.

**Tin tốt:** vòng lặp game (18 khung/giây) **KHÔNG bị chặn** bởi DB — GameServer bắn gói đi rồi trả về ngay, không chờ. Nên đặt MySQL sau Goddess **không làm giật game**. Và tải thực tế chỉ ~18–50 lệnh ghi/giây — MySQL thừa sức.

---

## 2. BẢN ĐỒ DỮ LIỆU HIỆN TẠI

### 2.1 Bảng liệt kê toàn bộ kho

| # | Kho | Công nghệ | Module sở hữu | File trên đĩa | Kích thước (production) | Chứa gì |
|---|-----|-----------|---------------|---------------|--------------------------|---------|
| 1 | `roledb` | Berkeley DB 18.1.40 BTREE | **Goddess.exe** (32-bit) | `bin\multiserver\database\roledb` + `roledb.0` + `log.*` | 1.802 nhân vật / 19,4 MB dữ liệu | Toàn bộ nhân vật: chỉ số, trang bị, rương, kỹ năng, buff, nhiệm vụ, kinh mạch |
| 2 | `dbfriend\friend` | Berkeley DB 18.1.40 | **S3Relay.exe** (32-bit) | `bin\multiserver\dbfriend\friend` | 36.864 B | Danh sách bạn bè + nhóm bạn |
| 3 | `TongDB\TongTable` | Berkeley DB 18.1.40 | S3Relay.exe | `bin\multiserver\TongDB\` | 32.768 B | Thông tin bang hội (6.860 B/bang) |
| 4 | `TongMemberDB\MemberTable` (+`.0`) | Berkeley DB 18.1.40 | S3Relay.exe | `bin\multiserver\TongMemberDB\` | 36.864 + 8.192 B | Thành viên bang (404 B/người) |
| 5 | `TongMoneyDB\MoneyTable` | Berkeley DB 18.1.40 | S3Relay.exe | `bin\multiserver\TongMoneyDB\` | 8.192 B | Quỹ bang (36 B/bang) |
| 6 | `TongZhaoMuDB\ZhaoMuTable` (+`.0`) | Berkeley DB 18.1.40 | S3Relay.exe | `bin\multiserver\TongZhaoMuDB\` | 16.384 B | Đơn xin vào bang (76 B/đơn) |
| 7 | DB `account` | **MySQL (ĐÃ CHẠY THẬT)** | Sword3PaySys.exe **+ S3RelayServer.exe** | máy chủ MySQL, cổng 3306 | không đo được | `Account_info`, `Account_Habitus`, `ServerList`, **VIEW `View_AccountMoney`** |
| 8 | `Account_Habitus.nExtPoint` | MySQL (đường thứ 2) | **S3Relay.exe** | cùng DB `account` | — | Đồng xu tài khoản, S3Relay đọc/ghi trực tiếp |
| 9 | `baucua\deposits.json` | JSON | **CoreServer.dll** | `bin\server\baucua\deposits.json` | 330 B, 7 người | **TIỀN XU THẬT** gửi trong sòng bạc |
| 10 | `StatData.dat` | Nhị phân đổ struct | Goddess.exe | `bin\multiserver\StatData.dat` | 202.216 B | Bảng xếp hạng; **ghi ngược nWorldStat/nSectStat vào roledb** |
| 11 | `dulieu\giftcode\*_used.txt` | Văn bản, 1 mã/dòng | CoreServer.dll | `bin\server\dulieu\giftcode\` | 1.028 + 5.000 mã | Chống nhận trùng giftcode |
| 12 | `settings\jx2citywar.txt` / `jx2ladder.txt` / `jx2league.txt` | Văn bản có cấu trúc | CoreServer.dll | `bin\server\settings\` | 98 / 56 / 645 B | **Quyền sở hữu 7 thành, bảng xếp hạng JX2, liên minh bang** — là STATE, không phải config |
| 13 | ~10 file `.lua` server tự ghi đè | Mã nguồn Lua làm CSDL | Lua trong CoreServer | `bin\server\script\...` | 399 KB + 123 KB + … | Giftcode, số dư ngân lượng/tiền vàng sòng bạc Lua, boss hoàng kim, xếp hạng sự kiện, chặn 1 acc/máy Tống Kim |
| 14 | `*.bak` dump ngày | Định dạng tự chế | Goddess.exe | `bin\multiserver\database\<mốc>\` | 19,7 MB/bản, **không bao giờ xóa** | Bản sao lưu nóng |

**Kho đã xác minh là CHẾT — bỏ được hoàn toàn:** `account.db` + `role.db` (Berkeley DB 4.1 ngày 27/05/2003, md5 giống hệt nhau ở cả 2 thư mục ⇒ chưa hề bị ghi; không binary nào chứa chuỗi tên file), `GMPrivate.mdb` (Access của công cụ GM ngoài), `PlayerRankLog.ini`, `bin\server\deposits.json` (0 byte), `data\giftcode\` + `data\bbcode\` (2022), `tongset.ini` + `chatset.ini` ở gốc, `bin\server\StatData.dat` (layout cũ 44 B/dòng), `testgop\`.

### 2.2 Sơ đồ luồng dữ liệu

```
                          ┌──────────────────────────────────────────┐
   Client (S3Client)      │            MySQL  (cổng 3306)            │
        │                 │  DB `account`                            │
        │ 5622            │   • Account_info    (mật khẩu, iClientID)│
        ▼                 │   • Account_Habitus (giờ chơi, nExtPoint)│
  ┌───────────┐           │   • ServerList      (danh tính máy chủ)  │
  │  Bishop   │──5002────▶│   • VIEW View_AccountMoney  ◀── ÍT AI BIẾT│
  │ (gateway) │           └────────▲──────────────▲──────────▲───────┘
  │  32-bit   │                    │              │          │
  └─────┬─────┘            ┌───────┴──────┐ ┌─────┴──────┐ ┌─┴────────────┐
        │ 5011             │Sword3PaySys  │ │S3RelayServer│ │  S3Relay     │
        │ (danh sách nv,   │ .exe (32bit) │ │.exe (ĐANG   │ │ .exe (32bit) │
        │  tạo/xóa nv)     │ 5 kết nối    │ │ CHẠY, 5 kn) │ │ chỉ nExtPoint│
        ▼                  └──────────────┘ └─────────────┘ └──────┬───────┘
  ┌────────────────┐                                               │
  │   Goddess.exe  │◀────5011──── GameServer.exe (x64) ────────────┘
  │    32-bit      │              CoreServer.dll (x64)      │
  │                │                    │                   │ 5 kho Berkeley DB
  │  Berkeley DB   │                    │ 30 giây/nhân vật  │ ┌──────────────┐
  │  ┌──────────┐  │                    │ (ghi ĐÈ cả bản ghi)│ │ dbfriend     │
  │  │ roledb   │  │◀───────────────────┘                   │ │ TongDB       │
  │  │ roledb.0 │  │   c2s_roleserver_saveroleinfo (=10)    ├▶│ TongMemberDB │
  │  └──────────┘  │   + 4 byte CRC32                       │ │ TongMoneyDB  │
  └───────┬────────┘                                        │ │ TongZhaoMuDB │
          │ 03:00 mỗi ngày                                  │ └──────────────┘
          ▼                                                 │
  database\<mốc>\*.bak  +  playerlist.txt  +  StatData.dat   │
          │                                     │            │
          └── StatData.dat GHI NGƯỢC ───────────┘            │
              nWorldStat/nSectStat vào roledb                │
              (mỗi lần Goddess khởi động)                    │

  CoreServer.dll cũng ghi thẳng ra file, KHÔNG qua kho nào:
     baucua\deposits.json      (TIỀN XU)
     dulieu\giftcode\*_used.txt
     settings\jx2citywar.txt / jx2ladder.txt / jx2league.txt
     script\**.lua  (server tự sinh lại mã nguồn Lua làm CSDL)
```

**Điểm cần nhớ về sơ đồ:** đường ghi nhân vật **không đi qua Bishop**. Bishop chỉ làm liệt kê / tạo / xóa / đọc lần đầu. Đường ghi định kỳ là **GameServer → Goddess** thẳng (`KSOServer.cpp:3301`, `:3333`).

---

## 3. PHÂN TÍCH TỪNG KHO

### 3.1 Kho `roledb` — nhân vật (kho quan trọng nhất)

**Cách mở.** `ZDBTable::ZDBTable`, `D:\GAMEDEVNEW\Sources\MultiServer\Goddess\DBTable.cpp:13-41`:
- thư mục môi trường = `getcwd()` + `\database`
- cờ: `DB_CREATE|DB_INIT_LOG|DB_INIT_LOCK|DB_INIT_MPOOL|DB_INIT_TXN|DB_RECOVER|DB_THREAD|DB_PRIVATE` (`DBTable.cpp:32`)
- `set_flags(DB_AUTO_COMMIT | DB_TXN_NOSYNC, 1)` (`DBTable.cpp:33`) ⇒ **commit KHÔNG ép ghi xuống đĩa**
- cache 64 MB, log 16 MB/file, log buffer 2 MB (`DBTable.cpp:26-30`)

**Khóa và giá trị.**
- Khóa chính = **tên nhân vật**, `key_size = strlen(szName)+1` (**kể cả byte NUL**), tối đa 32 — `IDBRoleServer.cpp:576`
- Khóa phụ `roledb.0` = **tên tài khoản** `caccname`, hàm sinh khóa `get_account` đọc thẳng trong blob — `IDBRoleServer.cpp:26-35`; `DB_DUP|DB_DUPSORT`
- Giá trị = **blob nhị phân nguyên khối** `TRoleData`

**Cấu trúc bản ghi — ĐÃ KIỂM CHỨNG TỪNG BYTE.**

> ⚠️ **CẢNH BÁO TRỌNG YẾU.** Trong cây nguồn có **BA** file `S3DBInterface.h` dùng chung `#ifndef S3DBINTERFACE_H`. Bản **THẬT SỰ ĐƯỢC BIÊN DỊCH** là `D:\GAMEDEVNEW\Lib\S3DBInterface.h` (581 dòng): `#pragma pack(push,1)` ở **dòng 15**, `pop` ở **dòng 579**, `char szName[32]` ở **dòng 46**, `char caccname[32]` ở **dòng 49**.
> Bản `D:\GAMEDEVNEW\Headers\S3DBInterface.h` (473 dòng, `szName[100]`, **không có pragma pack**) và `Sources\Core\lib\s3dbinterface.h` (470 dòng) là **HEADER CHẾT**.
> Bằng chứng đường include: `Core\Src\KPlayerDBFuns.cpp:9` include **tường minh** `"../../../lib/S3DBInterface.h"`; và `Goddess.vcxproj:75`/`:123`, `Bishop.vcxproj`, `GameServer.vcxproj` đều đặt `..\..\..\lib` **TRƯỚC** `..\..\..\Headers`.
> **Nếu ai viết công cụ chuyển đổi theo bản `Headers\` thì mọi offset lệch từ trường thứ hai trở đi (szName lệch 68 byte) và 1.802 bản ghi ra rác — nhưng vẫn biên dịch sạch, không báo lỗi.**

Bố cục blob (pack(1), tính tay từ khai báo, đã đối chiếu với dữ liệu thật):

```
offset 0   .. 700 : TRoleBaseInfo                         = 701 byte
     dwId@0(4)  szName@4(32)  bSex@36(1)  szAlias@37(32)  caccname@69(32)
     nFirstSect@101 nSect@102 cFightMode@103 cUseRevive@104 cIsExchange@105 cPkStatus@106
     ijoincount@107 isectrole@111 igroupcode@115 igrouprole@119
     irevivalid@123 irevivalx@127 irevivaly@131
     ientergameid@135 ientergamex@139 ientergamey@143
     cpartnercode@147(32) isavemoney@179 imoney@183 ifiveprop@187 iteam@191
     ifightlevel@195  fightexp@199(double 8)  ileadlevel@207 ileadexp@211 iliveexp@215
     ipower@219 iagility@223 iouter@227 iinside@231 iluck@235
     imaxlife@239 imaxstamina@243 imaxinner@247
     icurlife@251 icurstamina@255 icurinner@259 ipkvalue@263
     ileftprop@267 ileftfight@271 ileftlife@275 ifinishgame@279
     iarmorres@283(2) iweaponres@285(2) ihelmres@287(2)
     nSectStat@289 nWorldStat@293 nKillPeopleNumber@297 nForbiddenFlag@301 dwTongID@305
     ireputevalue@309 ifuyuanvalue@313 irebornvalue@317 ithuhangvalue@321
     ipassrole@325 irankrole@329 iexitemrole@333 iexboxrole@337
     skilltrain1..9@341(36)   ipduphong1..9@377(36)   szStringduphong1..9@413(288)
offset 701 : dwVersion(4)  bBaseNeedUpdate(1)
offset 706 : nFightSkillCount(2) nLiveSkillCount(2) nStateSkillCount(2)
offset 712 : nTaskCount(1 BYTE!)  nItemCount(2) nFriendCount(2)
offset 717 : dwTaskOffset dwLSkillOffset dwFSkillOffset dwSSkillOffset
             dwItemOffset dwFriendOffset dwDataLen  (7 × 4 byte)
offset 745 : pBuffer[] — vùng biến thiên, nối tiếp theo các offset ở trên:
             dwFSkillOffset → nFightSkillCount × TDBSkillData (8 B)
             dwSSkillOffset → nStateSkillCount × TDBSkillData (8 B)   ← buff còn hiệu lực
             dwTaskOffset   → nTaskCount       × TDBTaskData  (8 B)
             dwItemOffset   → nItemCount       × TDBItemData  (233 B) ← trang bị + túi + rương CHUNG
cuối cùng  : 4 byte CRC32 tại [dwDataLen-4]
```

`sizeof(TRoleBaseInfo)=701`, header `TRoleData` trước `pBuffer` = **745**, `sizeof(TRoleData)=746`, `sizeof(TDBItemData)=233` (vì `MAX_ITEM_MAGICATTRIB=8` tại `Core\Src\GameDataDef.h:37` ⇒ `iparam[16]`), `sizeof(TDBSkillData)=sizeof(TDBTaskData)=8`.

**Những chỗ dễ hiểu nhầm — phải biết trước khi thiết kế cột:**
- **Trang bị mặc trên người, túi đồ, rương, rương mở rộng, ô giao dịch NẰM CHUNG một khối**, phân biệt bằng `TDBItemData.ilocal` (enum `INVENTORY_ROOM`, `Core\Src\GameDataDef.h:297-314`).
- **Kinh mạch KHÔNG có khối riêng**: 8 byte nhị phân nhét trong `szStringduphong2` (offset 445) — `KPlayerDBFuns.cpp:962`. **Đây là bẫy chết người: byte nhị phân trong một trường mang tên "chuỗi", có thể chứa byte 0 ở giữa.** Ai ánh xạ "9 trường chuỗi dự phòng" thành 9 cột TEXT sẽ **cắt cụt kinh mạch**.
- **Chiến mã** chỉ là một vật phẩm thường ở ô `itempart_horse`.
- **Thuộc tính/option của trang bị KHÔNG được lưu** — engine tái sinh chúng từ `iequipversion + irandseed + iparam[16] + ilucky` mỗi lần nạp (`KPlayerDBFuns.cpp:500-727`). Nghĩa là **không thể truy vấn "tìm trang bị có option X" bằng SQL**, và đổi bảng sinh item sẽ làm lệch chỉ số toàn bộ trang bị cũ.
- **Các ô "dự phòng" ĐÃ BỊ CHIẾM**: `ipduphong1`=avatar, `ipduphong2`=khóa PK, `ipduphong3`=số bộ trang bị, `ipduphong4`=honor id, `szStringduphong1`=tên bạn đời, `szStringduphong2`=kinh mạch. **Cấm đặt tên cột là `reserve1..9`.**
- **Trường chết:** `nLiveSkillCount`, `dwLSkillOffset`, `nFriendCount`, `dwFriendOffset` — có trong struct, không nơi nào đổ dữ liệu (đã grep toàn `Core\Src\*.cpp`). Bạn bè đã chuyển sang S3Relay.
- **`dwVersion` là bẫy tâm lý — nó CHẾT.** Chỉ Bishop ghi `= 0` (`PlayerCreator.cpp:187`), không ai đọc. **Không có cơ chế phiên bản nào.** Phải tự thêm cột `fmt_ver` bên ngoài blob.

**Số đo thật trên kho production** (tự phân tích `bin\multiserver\New folder\database\202649_824790\202649_824790.bak`, 19.752.591 B, ngày 09/04/2026):

| Chỉ số | Giá trị |
|---|---|
| Số nhân vật hợp lệ | **1.802** |
| Số tài khoản | **1.355** |
| Phân bố nhân vật/tài khoản | 1 nv: 1.028 · 2 nv: 207 · 3 nv: 120 · **không có tài khoản nào 4 nhân vật** |
| Kích thước blob | min **1.002** · trung vị **9.393** · p95 **22.336** · **max 69.257** · trung bình 10.790 |
| Tổng dữ liệu | **19.444.875 byte** |
| Tên chứa byte ≥ 0x80 (TCVN3) | **205 / 1.802** |
| **Nhóm tên va chạm khi bỏ phân biệt HOA/thường** | **25 nhóm → mất 27 bản ghi** |
| **Bản ghi có CRC32 SAI** | **1.508 / 1.802 = 83,7 %** |
| Số biến nhiệm vụ tối đa | **71** (KHÔNG bản ghi nào vượt 255) |
| Số vật phẩm tối đa | **292** (giới hạn `MAX_PLAYER_ITEM` = 523) |

Ví dụ cặp tên va chạm thật: `BomMau`/`Bommau`, `CayTien`/`Caytien`/`caytien`, `DocLong`/`Doclong`, `KiemTien`/`kiemtien`, `ONG*KE`/`ong*ke`, `Ronaldo`/`ronaldo`.

**Về 83,7% CRC sai — đây là phát hiện mới, không có trong khảo sát nào.** Thuật toán CRC32 của game là **zlib nguyên bản** (`Sources\MultiServer\GameServer\CRC32.c` dòng 6: *"Get source code from Zlib Project"*), nên phép kiểm là chính xác. Nguyên nhân: `CDBBackup::SaveStatInfo` (`DBBackup.cpp:855-925`) ghi đè `nWorldStat`/`nSectStat` vào blob rồi `RunTable->add(...)` **mà đoạn tính lại CRC đã bị comment ở CẢ HAI chỗ** (`DBBackup.cpp:888-893` và `:915-920`). Hàm này chạy **mỗi lần Goddess khởi động** (`IDBRoleServer.cpp:673`). Với 1.802 nhân vật thì gần như ai cũng lọt vào top-1000 của ít nhất một bảng xếp hạng ⇒ gần như ai cũng bị ghi đè. Chỉ 294 người được GameServer lưu lại sau đó nên CRC mới đúng.
**⇒ Công cụ nhập liệu TUYỆT ĐỐI không được lọc theo CRC. Phải nhập hết, rồi tính lại CRC.**

**Bẫy về file `.bak`** (mới phát hiện, chưa khảo sát nào nêu): file dump production có **4 bản ghi rác ở ĐẦU file** (khóa là byte nhị phân như `\x00`, `\x03\x00`, `dwDataLen` = 0 hoặc lệch hẳn) và **205.766 byte rác ở CUỐI file** với mẫu `0xCD` / `0xFD` / `0xDD` — đây là mẫu tô vùng nhớ chưa khởi tạo / vùng đã giải phóng của bộ chạy Debug MSVC. Nguyên nhân: `CDBDump::AddData` khi ghi hỏng thì `fseek` lùi về (`DBDumpLoad.cpp:44-73`) nhưng **`fseek` lùi KHÔNG cắt ngắn file** — độ dài vẫn giữ ở mốc cao nhất. Công cụ đọc `.bak` **bắt buộc phải kiểm chứng chéo**: `keysize <= 32` **VÀ** `dwDataLen == datasize` **VÀ** `key == BaseInfo.szName`. Bỏ ba phép kiểm này là nhập rác vào MySQL.

**Điểm đọc/ghi trong mã (file:dòng):**

| Vai trò | Vị trí |
|---|---|
| Bộ phát nhịp ghi | `Core\Src\KPlayerSet.cpp:50` (`30*18` tick = 30 giây), `:1093-1115` (`AutoSave`, có `break` ⇒ 1 lượt/tick) |
| Đóng gói blob | `Core\Src\KPlayerDBFuns.cpp:205-221` (`UpdateDBPlayerInfo`) — gọi 5 hàm ĐÚNG THỨ TỰ BaseInfo → FightSkill → StateSkill → Task → Item |
| Ranh giới DLL | `Core\Src\CoreServerShell.cpp:1208-1222` (`SavePlayerDataAtOnce`) |
| Nối CRC + gửi gói | `MultiServer\GameServer\KSOServer.cpp:3323-3410` (`dwDataLen += 4` rồi CRC trên `dwDataLen-4`) |
| Nhận phía Goddess | `MultiServer\Goddess\ClientNode.cpp:413-456` (`_SaveRoleInfo`) |
| Ghi nghiệp vụ | `MultiServer\Goddess\IDBRoleServer.cpp:463-582` (`SaveRoleInfo`) — chặn `dwDataLen >= 327680` ở **:475**, kiểm CRC ở **:486-503**, chặn trùng tên ở **:549**, đếm số nhân vật ở **:561-570**, `add` ở **:576** |
| Chạm đĩa | `MultiServer\Goddess\DBTable.cpp:95-114` (`primary_db->put(..., DB_AUTO_COMMIT)`) |
| Đọc | `IDBRoleServer.cpp:437-461` (`GetRoleInfo`) — **KHÔNG kiểm CRC** |
| Liệt kê theo tài khoản | `IDBRoleServer.cpp:584-638` (`GetRoleListOfAccount`, dùng khóa phụ) |
| Xóa | `IDBRoleServer.cpp:640-647` — **XÓA THẬT**, không tombstone, không bản sao |
| Checkpoint + xóa log | `DBTable.cpp:286-301` + `IDBRoleServer.cpp:46-52` (60 phút/lần) |

**Tần suất ghi.** Chu kỳ danh nghĩa 30 giây/nhân vật. Nhưng `AutoSave()` có `break` ⇒ **trần 1 lượt/tick = 18 lượt/giây**. Ở 1.500 người, chu kỳ thực giãn ra ~83 giây. Đo thật trên `goddess_log.txt` (358.102 dòng, 318.737 lượt `SaveRoleInfo`, 21/08/2025 → 18/08/2026): đỉnh ghi đo được **0,156 lượt/giây** (máy test ~6 nick), đỉnh đọc **60 thao tác/giây** lúc bot đăng nhập hàng loạt. Tổng byte đã ghi: **7,35 GB cho một DB 2,9 MB** ⇒ **khuếch đại ghi ~2.500 lần**, vì mỗi lượt ghi đè cả bản ghi dù chỉ đổi một con số.

### 3.2 Năm kho Berkeley DB của S3Relay

Cùng lớp `ZDBTable` nhưng **hành vi giao dịch khác hẳn Goddess**: `S3Relay\DBTable.cpp:111-160` có `txn_begin` → `put(txn)` → `commit` rồi gọi tiếp `commit()` (`:94-109` = `primary_db->sync` + `txn_checkpoint`). **Mỗi lần thêm/sửa một bản ghi = một lần đồng bộ file + một lần checkpoint toàn DB.** Rất chậm nhưng bền hơn Goddess.

| Kho | Khóa chính | Khóa phụ | Struct | Kích thước |
|---|---|---|---|---|
| `friend` | tên nhân vật, **KHÔNG có byte NUL** (`FriendMgr.cpp:823-826`, dùng `dbkey.size()`) | không | Bản ghi biến thiên tự đóng gói: `[BYTE count]` rồi lặp `[bool cheating][bool cheated][BYTE len][tên][BYTE len][nhóm]` | ~14–98 B/bạn |
| `TongTable` | `szName` (**CÓ NUL**, `strlen+1`) | không | `TTongStruct` | **6.860 B** (bản cũ thiếu `szJX2Recruit` = 6.732 B, vẫn được chấp nhận) |
| `MemberTable` | `szName` người chơi | `szTong` (`MemberTable.0`) | `TMemberStruct` | **404 B** |
| `MoneyTable` | `szTong` | không | `TMoneyStruct` (`szTong[32]` + `DWORD m_dwMoney`) | **36 B** |
| `ZhaoMuTable` | `szName` người xin | `szTong` (`ZhaoMuTable.0`) | `TZhaoMuStruct` | **76 B** |

**⚠️ Khác biệt khóa NUL giữa `friend` (không NUL) và các bảng Tong (có NUL)** là bẫy mất dữ liệu âm thầm nhất khi di trú — cắt/không cắt byte cuối một cách đồng loạt sẽ làm khóa lệch 1 ký tự.

**⚠️ `TMoneyStruct.m_dwMoney` là `DWORD` KHÔNG DẤU** (`TONGDB.H:45`) — đây chính là gốc sự cố "quỹ bang tràn số âm → 4,29 tỷ". Khi lên MySQL **phải chọn `BIGINT` CÓ DẤU**, tuyệt đối không `UNSIGNED`, nếu không sẽ tái lập đúng lỗi cũ.

**Điểm vào/ra:** `TONGDB.CPP:45-72` (`Open`, tên thư mục **hard-code**, không đọc ini), `:131-169` (`ChangeTong`), `:431-436` (`ChangeMember`, bắt buộc đọc-trước-ghi-sau qua `JX2_PreserveMemberTail`), `:444-447` (`ChangeMoney`), `:527-544` (chiêu mộ). Bạn bè: `FriendMgr.cpp:889-940` (ghi), `:845-887` (đọc), `:942+` (bộ xả nền — **5 phút/lần, mỗi lần tối đa 8 người**, cấu hình ở `relay_friendcfg.ini`).

**Lỗi có sẵn phải biết:** `FriendMgr.cpp:851-861` lấy `pValue = cursor->data` rồi `closeCursor(cursor)` (đã `free`) **NHƯNG VẪN ĐỌC `pValue` sau đó**, cuối cùng `delete[]` trên vùng cấp phát bằng `malloc`. Đây là **dùng-sau-khi-giải-phóng thật**. ⇒ **Công cụ di trú tuyệt đối không tái dùng `DB_LoadSomeone` nguyên bản**, phải tự đọc qua `ZDBTable` rồi tự giải mã.

**Kênh chat: KHÔNG lưu gì xuống đĩa.** `CChannelMgr` thuần RAM. Riêng chat riêng có ghi log văn bản ra `s3relay_log\ChatSomeOne*.log` (`ChannelMgr.cpp:862-884`) nhưng **thư mục `s3relay_log` KHÔNG tồn tại** trong `bin` ⇒ `fopen` trả NULL ⇒ **log chat riêng đang mất trắng**.

### 3.3 Kho tài khoản MySQL — ĐÃ XONG, chỉ cần dọn

Bằng chứng đã ở MySQL: `S3PDBConnectionPool.cpp:171` comment dòng MSSQL, `:172` `new S3PDB_MySQL_Connection()`. Toàn bộ SQL sống dùng cú pháp MySQL (`NOW()`, `TIME_TO_SEC(ABS(TIMEDIFF()))`, `IF()`).

**Bảng / đối tượng thật sự được dùng:**
- `Account_info` — `cAccName`, `cPassword` (**lưu dạng chữ rõ**), `cSecPassword` (mật khẩu cấp 2, dùng để **xóa nhân vật**), `iClientID` (= `ServerList.iid` của gateway đang giữ; 0 = ngoại tuyến — đây là cờ chống đăng nhập hai nơi), `dLoginDate`, `dLogoutDate`, `nLockTm`
- `Account_Habitus` — `cAccName`, `iClientID`, `iLeftSecond`, `nExtPoint` (**đồng xu**), `dEndDate`, `dLoginDate`
- `ServerList` — `cServerName`, `cPassword` (MD5), `cIP`, `iPort`, `iid`, `cMemo` (chuỗi MAC `AABB-CCDD-EEFF`)
- **`View_AccountMoney`** — **một VIEW, không khảo sát ban đầu nào nêu tên.** Dùng bởi module `S3RelayServer`:
  `Sword3PaySys\S3RELAYSERVER\S3PAccount.cpp:253`
  ```sql
  select TIME_TO_SEC(ABS(TIMEDIFF(NOW(), dEndDate))), iLeftSecond, cAccName,
         TIME_TO_SEC(ABS(TIMEDIFF(NOW(), dLoginDate)))
  from View_AccountMoney where (iClientID = %d) and (dLoginDate is not null)
  ```

**⚠️ Có HAI tiến trình cùng ghi vào DB `account`**, không phải một: `Sword3PaySys.exe` và **`S3RelayServer.exe`** (dự án sống trong `JXAll.sln:36`, binary ở cả hai thư mục bin, log chạy `relayserver_log\2026_08_18\`). `S3RelayServer` mở pool riêng 5 kết nối (`S3RELAYSERVER\main.cpp:180`) và có thêm 5 câu SQL không ai liệt kê: `select count(*) from Account_info` (:150), `update Account_info set iClientID = 0 where cAccName` (:205), `update ... set iClientID = GetGMID()` (:221 — đóng băng tài khoản), `update ... where iClientID = %d` (:237 — mở khóa cả máy chủ), `GetServerID` (:307).
Hiện `S3RelayServer` **đang xác thực thất bại** — log lặp `"Relay (127.0.0.1)…: Name, Password !"` vì bản ghi `ServerList` tên `free` (mật khẩu MD5 của `1`) không tồn tại. Tức tính năng này **đang chết lặng lẽ từ trước**.

**Ba việc phải kiểm chứng trên MySQL đang chạy trước khi làm gì** (tôi không được phép chạy lệnh ghi/kết nối):
1. `SHOW CREATE VIEW View_AccountMoney;`
2. `SHOW CREATE TABLE Account_info; / Account_Habitus; / ServerList;` — file `XSHANJU\需求文档\DataBase\account.SQL` là bản **2002 đã lệch xa**, thiếu hẳn `cSecPassword`, `nLockTm`, `nExtPoint`, `Account_Habitus.iClientID`, `Account_Habitus.dLoginDate`. **Chạy file đó để dựng lại DB là hỏng ngay.**
3. Kiểu thật của `Account_Habitus.dLoginDate`: DDL 2002 ghi `varchar(10)` nhưng mã sống làm `TIMEDIFF(dLoginDate, NOW())`. **Nếu cột thật vẫn là varchar thì bộ tính giờ đang SAI ÂM THẦM từ trước tới nay.**

**Cấu hình gây hiểu nhầm:** `DataBase.ini` ghi `Port=1433` — **bị bỏ qua hoàn toàn**. `def_PORTKEYNAME` khai báo ở `GlobalDTD.h:17` nhưng **không nơi nào tham chiếu**; `_DATABASEINFO` (`S3PDBConVBC.h:17-23`) chỉ có 4 trường, không có port; cổng **3306 hard-code** tại `S3PDB_MySQL_Connection.cpp:51`. Đã kiểm chứng trên binary: `Sword3PaySys.exe` chứa chuỗi `DBConnects` nhưng **không chứa `1433`**.

**Kiến trúc bit:** `Goddess.exe`, `S3Relay.exe`, `Sword3PaySys.exe` là **x86 32-bit**; `CoreServer.dll` và `GameServer.exe` là **x64**. `libmySQL.dll` trong `bin` là **x86 (32-bit)** — dùng được cho Goddess. **Connector MySQL cho Goddess bắt buộc 32-bit.**

### 3.4 Các kho file phẳng (đáng quan tâm nhất là tiền)

**Bắt buộc chuyển hoặc ít nhất phải xử lý:**
1. **`baucua\deposits.json`** — **TIỀN XU THẬT**. `KProtocolProcess.cpp:6417-6440` **trừ Xu khỏi nhân vật trước** rồi mới `g_BauCua.deposit()` ghi JSON. Ghi/đọc: `BauCua.cpp:586-620`. Khóa là **base64(tên nhân vật)**. Số dư hiện tại: CaiBang 120, GameJX1 6.380, DDDDDDDD3 29.107, ThienVuongNe 14.028, NgaMy1 6.250, ThienNhan 1.401, DDDDDDDD 323.
   ⚠️ **File thật hiện CHỈ CÓ khóa `"balances"`, KHÔNG có khóa `"locked"`** (vòng ghi rỗng nên `nlohmann/json` không tạo khóa). Công cụ nhập phải chịu được trường hợp thiếu khóa, nếu không sẽ ném ngoại lệ và **mất sạch số dư**.
2. **Giftcode** — `dulieu\giftcode\giftcode_{new,tuan}_used.txt` (`GiftCodeManager.cpp:51-64`, append) + `script\codenew.lua` (399 KB) + `script\giftcode_fancung.lua` (123 KB). **`giftcode_tuan_used.txt` hiện CHƯA TỒN TẠI** — mất/không phục hồi file này thì **5.000 mã tuần quay lại trạng thái chưa dùng**.
3. **`settings\jx2citywar.txt` / `jx2ladder.txt` / `jx2league.txt`** — nằm trong thư mục `settings` nhưng là **STATE**, chính CoreServer ghi đè (`KJx2CityWar.cpp:215-243`, `KJx2SharedStore.cpp:333-360`, `KJx2League.cpp:200-232`). Ba file này **có dùng `.tmp` + `MoveFileEx REPLACE`** nên an toàn hơn đường Lua.
4. **`StatData.dat`** (202.216 B = đúng `sizeof(TStatData)` với `TRoleList` 48 byte) — bảng xếp hạng, và **ghi ngược vào roledb** như đã nói ở 3.1.
5. **~10 file `.lua` server tự ghi đè** — số dư ngân lượng/tiền vàng sòng bạc Lua (`script\songbac\datanganluong.lua`, `datatienvang.lua`), boss hoàng kim, xếp hạng sự kiện, danh sách bang Công Thành Chiến, chặn 1 tài khoản/máy Tống Kim.
   ⚠️ Cơ chế ghi là `openfile(file,"w+")` (`script\lib\lib_ham.lua:164-218`) = **cắt trắng file rồi ghi lại, không nguyên tử, không `.tmp`**. Mất điện giữa chừng = **mất cả bảng**.

**Kênh tích hợp bên ngoài** (không phải kho, nhưng phải tính): `dulieu\username_kick.txt` là **hàng đợi lệnh kick** do web admin ghi, `timerserver.lua:144-215` đọc rồi xóa trắng. `dulieu\topTG.txt` do một công cụ ngoài sinh ra — nhưng đường dẫn trong script là `C:\server\dulieu\topTG.txt`, **không khớp thư mục chạy thật** ⇒ chức năng này nhiều khả năng **đang hỏng**.

---

## 4. THIẾT KẾ MySQL ĐỀ XUẤT

### 4.1 Ba nguyên tắc bắt buộc, không thương lượng

**(1) Mọi cột chứa tên hoặc dữ liệu nhị phân phải là `VARBINARY` / `BLOB`, charset `binary`.**
Không phải "nên", mà là **đã đo**: 205/1.802 tên chứa byte ≥ 0x80 (TCVN3), và **25 nhóm tên sẽ va chạm ⇒ mất 27 nhân vật** nếu dùng collation `_ci`. Berkeley DB so sánh khóa theo byte (`memcmp`); MySQL mặc định `_ci` không phân biệt HOA/thường **và còn bỏ khoảng trắng cuối**.
⚠️ **Nhưng có một ngoại lệ ngược:** cột `cAccName` bên `account` phải giữ collation `_ci` như hiện tại, vì DDL gốc dùng `*_CI_AS` và người chơi đang đăng nhập không phân biệt hoa/thường. **Tên nhân vật = `_bin`; tên tài khoản = `_ci`.** Hai quy tắc trái ngược nhau trong cùng một hệ.

**(2) Blob nhân vật phải là `MEDIUMBLOB`, kèm `sql_mode = STRICT_ALL_TABLES`.**
Bản ghi thật đã đo **69.257 byte** > 65.535 ⇒ `BLOB` sẽ **cắt cụt âm thầm** nếu không ở chế độ strict. Trần trong mã là 327.680 byte (`IDBRoleServer.cpp:475`).

**(3) Bắt buộc dùng prepared statement có bind, cấm `sprintf` nối chuỗi SQL.**
Blob `TRoleData` chứa byte `0x00`, `0x27` (nháy đơn), `0x5C` (gạch chéo ngược) ở vị trí bất kỳ. Tên TCVN3 có thể có byte thứ hai = `0x5C`. `mysql_real_escape_string` **chỉ an toàn khi charset kết nối khớp dữ liệu** — với dữ liệu GBK/TCVN3 mà charset kết nối là latin1 sẽ sinh SQL injection kinh điển. Tầng tài khoản hiện tại **đã từng bị tấn công**: `S3PAccount.cpp:32-33` có một danh sách đen đúng **một phần tử** — `if (strcmp(strAccName, "')DELETE Account_Info --") == 0) return;`. Đó là bằng chứng.
⚠️ **Tham số cuối của `mysql_real_connect` phải GIỮ = 0.** Chỉ cần đổi thành `CLIENT_MULTI_STATEMENTS` là mở toang cửa cho `'; DROP TABLE ...; --`.

### 4.2 Phương án A — GIỮ BLOB (khuyến nghị cho đợt 1)

```sql
CREATE TABLE role (
  role_name    VARBINARY(32)   NOT NULL,          -- = BaseInfo.szName, byte TCVN3, PHẢI nhị phân
  acc_name     VARBINARY(32)   NOT NULL,          -- = BaseInfo.caccname, thay index roledb.0
  data_len     INT UNSIGNED    NOT NULL,          -- = dwDataLen, để đối chiếu
  role_blob    MEDIUMBLOB      NOT NULL,          -- nguyên TRoleData, KỂ CẢ 4 byte CRC32 cuối
  crc32        INT UNSIGNED    NOT NULL,          -- rút ra để soi nhanh, KHÔNG bỏ khỏi blob
  fmt_ver      SMALLINT        NOT NULL DEFAULT 1,-- BỔ SUNG: dwVersion trong blob là trường chết
  -- các cột dẫn xuất, chỉ để truy vấn/xếp hạng, KHÔNG phải nguồn sự thật:
  sex          TINYINT UNSIGNED NOT NULL DEFAULT 0,   -- BaseInfo.bSex
  series       INT             NOT NULL DEFAULT 0,    -- BaseInfo.ifiveprop
  level        INT             NOT NULL DEFAULT 0,    -- BaseInfo.ifightlevel
  sect         TINYINT UNSIGNED NOT NULL DEFAULT 255, -- BaseInfo.nSect
  money        BIGINT          NOT NULL DEFAULT 0,    -- imoney + isavemoney (BIGINT chống tràn)
  fight_exp    DOUBLE          NOT NULL DEFAULT 0,    -- BaseInfo.fightexp (GỐC LÀ double, không phải int)
  kill_num     INT             NOT NULL DEFAULT 0,
  tong_id      INT UNSIGNED    NOT NULL DEFAULT 0,    -- BaseInfo.dwTongID
  use_revive   TINYINT UNSIGNED NOT NULL DEFAULT 0,   -- Bishop chọn GameServer theo cột này
  revival_id   INT             NOT NULL DEFAULT 0,
  enter_game_id INT            NOT NULL DEFAULT 0,
  -- điều phối:
  ver          BIGINT UNSIGNED NOT NULL DEFAULT 0,    -- khóa lạc quan, mỗi lần lưu +1
  locked_by    INT                      NULL,         -- THAY cho m_csRoleLock trong RAM
  locked_at    DATETIME                 NULL,
  created_at   DATETIME        NOT NULL,
  updated_at   TIMESTAMP(3)    NOT NULL DEFAULT CURRENT_TIMESTAMP(3)
                                        ON UPDATE CURRENT_TIMESTAMP(3),
  PRIMARY KEY (role_name),
  KEY idx_acc   (acc_name),
  KEY idx_level (level),
  KEY idx_money (money)
) ENGINE=InnoDB DEFAULT CHARSET=binary ROW_FORMAT=DYNAMIC;

CREATE TABLE role_delete_log (           -- hiện tại XÓA LÀ MẤT VĨNH VIỄN
  id          BIGINT AUTO_INCREMENT PRIMARY KEY,
  role_name   VARBINARY(32) NOT NULL,
  acc_name    VARBINARY(32) NOT NULL,
  data_len    INT UNSIGNED  NOT NULL,
  role_blob   MEDIUMBLOB    NOT NULL,
  deleted_at  DATETIME      NOT NULL,
  deleted_by_ip VARBINARY(45) NULL,
  KEY idx_name (role_name), KEY idx_time (deleted_at)
) ENGINE=InnoDB DEFAULT CHARSET=binary;
```

**Ánh xạ thao tác** (chỉ thay ruột 5 hàm trong `IDBRoleServer.cpp`, **`ClientNode.cpp`, `Bishop`, `GameServer` không phải sửa một dòng**):

| Hàm hiện tại | Câu MySQL thay thế |
|---|---|
| `ZDBTable::add` (lưu định kỳ) | `UPDATE role SET role_blob=?, data_len=?, crc32=?, level=?, …, ver=ver+1 WHERE role_name=? AND ver=?` → kiểm `affected_rows == 1` |
| `ZDBTable::add` (tạo mới) | `START TRANSACTION; SELECT COUNT(*) FROM role WHERE acc_name=? FOR UPDATE; INSERT INTO role …; COMMIT;` — **trùng tên bắt bằng lỗi 1062, KHÔNG mô phỏng lại SELECT-rồi-INSERT** |
| `ZDBTable::remove` | `START TRANSACTION; INSERT INTO role_delete_log SELECT … FROM role WHERE role_name=?; DELETE FROM role WHERE role_name=?; COMMIT;` |
| `_search(key)` | `SELECT role_blob FROM role WHERE role_name=?` |
| `_search(acc, index 0)` + `_next` | `SELECT role_name, sex, series, level FROM role WHERE acc_name=? ORDER BY created_at LIMIT ?` — **không kéo blob**, đây là điểm lợi ngay so với Berkeley DB (hiện phải đọc cả bản ghi) |
| Đếm số nhân vật (`IDBRoleServer.cpp:561-570` đang quét cursor) | `SELECT COUNT(*) FROM role WHERE acc_name=? FOR UPDATE` |
| `c2s_roleserver_lock` | `UPDATE role SET locked_by=?, locked_at=NOW() WHERE role_name=? AND (locked_by IS NULL OR locked_by=? OR locked_at < NOW()-INTERVAL 5 MINUTE)` → kiểm `affected_rows` |
| `removeLog()`, `deadlock()` | thành hàm rỗng (MySQL tự lo) |
| Sao lưu (`CDBBackup::Backup`) | `mysqldump --single-transaction` — **ảnh chụp nhất quán THẬT**, khác hẳn cursor nóng hiện tại |
| Bảng xếp hạng (`GetMin`/`ListSort` O(n²) trong `DBBackup.cpp:47-120`) | `SELECT name, level FROM role ORDER BY level DESC, fight_exp DESC LIMIT 1000` … + `GROUP BY sect` |

**Ưu:** rủi ro thấp nhất, không đụng định dạng bản ghi, không đụng `Core`, giữ nguyên CRC32, chuyển đổi 1-1 kiểm chứng được bằng đếm bản ghi.
**Nhược:** vẫn không truy vấn được nội dung nhân vật bằng SQL; **vẫn giữ nguyên khuếch đại ghi 2.500 lần** (~10,8 KB ghi đè mỗi 30 giây/người); blob > 8 KB nằm ngoài trang InnoDB nên mỗi `UPDATE` là một chuỗi cấp phát/giải phóng trang tràn ⇒ **phân mảnh dần**.

**Giảm nhẹ khuếch đại ghi (rẻ, nên làm ngay trong phương án A):** thêm cột `blob_hash BINARY(8)`; nếu hash không đổi thì **bỏ qua `UPDATE`**, chỉ chạm `updated_at`. Log cho thấy có những chuỗi hàng trăm lượt `dwDataLen` lặp y hệt liên tiếp — tức phần lớn lượt ghi là vô ích.

### 4.3 Phương án B — BUNG THÀNH CỘT QUAN HỆ

```sql
role_base   (role_name PK, ~90 cột vô hướng từ TRoleBaseInfo)
role_item   (role_name, seq, ilocal, ix, iy, iequipcode, idetailtype, iparticulartype,
             ilevel, iseries, iequipversion, irandseed, iparam BINARY(64) hoặc 16 cột INT,
             ilucky, idurability, istacknum, …  PK(role_name, seq), KEY(role_name, ilocal))
role_skill  (role_name, kind ENUM('fight','state'), skill_id, skill_level, skill_val,
             PK(role_name, kind, skill_id))
role_task   (role_name, task_id SMALLINT UNSIGNED, task_value INT, PK(role_name, task_id))
role_meridian (role_name PK, m BINARY(8))     -- tách khỏi szStringduphong2
```

**Ưu:** hết khuếch đại ghi (chỉ `UPDATE` dòng thật sự đổi); truy vấn được bằng SQL (thống kê, công cụ GM, chống hack đồ); sửa được giới hạn `nTaskCount` kiểu BYTE.

**Nhược — và đây là phần chưa khảo sát nào nói thẳng:**
- Phải viết lại toàn bộ 5 hàm `Save*` + 5 hàm `Load*` trong `KPlayerDBFuns.cpp` (~1.100 dòng).
- **`iparam[16]` không được đổi thành JSON** — engine `memcpy` thẳng mảng này.
- **Nếu ghi rời rạc mà không bọc trong MỘT transaction, sập giữa chừng sẽ cho ra "túi đồ của bản cũ + tiền của bản mới" ⇒ NHÂN ĐÔI VẬT PHẨM THẬT.** Mô hình blob-một-mảnh hiện tại **đang bảo vệ ta khỏi loại lỗi này**. Tách bảng là **đánh đổi**, không phải nâng cấp thuần túy.
- Hàm nạp (`LoadDBPlayerInfo`, `KPlayerDBFuns.cpp:34-203`) là **máy trạng thái nạp chia nhỏ qua nhiều vòng game** (tối đa 10 phần tử/vòng, `KPlayerDef.h:4-7`), con trỏ chạy **tuần tự**; chỉ `dwFSkillOffset` được dùng khi `nBegin==0`. Tách bảng phải viết lại cả máy trạng thái này.
- Bỏ được CRC32, nhưng cũng **mất luôn lớp phát hiện hỏng dữ liệu duy nhất**.

**Khuyến nghị:** **Phương án A cho đợt 1. Phương án B cho đợt 3, và chỉ với `role_item` + `role_task`** (hai khối thay đổi thường xuyên nhất, chiếm phần lớn dung lượng), giữ `TRoleBaseInfo` dưới dạng blob. Đây là điểm cân bằng tốt nhất.

### 4.4 Schema cho 5 kho của S3Relay

```sql
CREATE TABLE friend_edge (             -- chuẩn hóa, KHÔNG giữ blob biến thiên
  owner_name  VARBINARY(32) NOT NULL,
  friend_name VARBINARY(32) NOT NULL,
  group_name  VARBINARY(64) NOT NULL DEFAULT '',
  cheating    TINYINT(1) NOT NULL DEFAULT 1,
  cheated     TINYINT(1) NOT NULL DEFAULT 1,
  updated_at  TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (owner_name, friend_name),
  KEY idx_friend (friend_name)
) ENGINE=InnoDB DEFAULT CHARSET=binary;
-- Giữ mô hình 2 dòng (A→B và B→A) như hiện tại để KHÔNG phải viết lại logic FriendMgr.
-- Khi xả 1 người: DELETE WHERE owner_name=? rồi INSERT nhiều dòng, trong 1 transaction
-- (khớp đúng ngữ nghĩa add() hiện tại là "ghi đè cả bản ghi").

CREATE TABLE tong (
  tong_name    VARBINARY(32) NOT NULL PRIMARY KEY,
  camp TINYINT, credit INT, level INT, recruit INT,
  master_name  VARBINARY(32), master_title VARBINARY(32),
  normal_title VARBINARY(32), boy_title VARBINARY(32), girl_title VARBINARY(32),
  way_edit VARBINARY(32), next_target VARBINARY(32), league_name VARBINARY(32),
  exp_guide INT, city_guide INT,
  jx2_announce VARBINARY(128), jx2_recruit VARBINARY(128),
  jx2_event_head INT, jx2_history_head INT,
  director_titles VARBINARY(224),   -- 7 × 32, giữ nguyên thứ tự byte của struct C
  manager_titles  VARBINARY(1792),  -- 56 × 32
  jx2_field_key   VARBINARY(384),   -- 192 × WORD
  jx2_field_val   VARBINARY(768),   -- 192 × DWORD
  jx2_event       VARBINARY(1536),  -- 16 × 96
  jx2_history     VARBINARY(1536),
  money BIGINT NOT NULL DEFAULT 0,  -- ❗ CÓ DẤU. Gộp TongMoneyDB vào đây.
  updated_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=binary;

CREATE TABLE tong_member (
  member_name VARBINARY(32) NOT NULL PRIMARY KEY,
  tong_name   VARBINARY(32) NOT NULL,
  member_class TINYINT NOT NULL, title_index INT NOT NULL, sex TINYINT NOT NULL,
  jx2_field_count INT, jx2_field_key VARBINARY(64), jx2_field_val VARBINARY(128),
  jx2_right_count INT, jx2_right VARBINARY(128),
  KEY idx_tong (tong_name)            -- thay MemberTable.0
) ENGINE=InnoDB DEFAULT CHARSET=binary;

CREATE TABLE tong_apply (
  applicant_name VARBINARY(32) NOT NULL PRIMARY KEY,
  tong_name VARBINARY(32) NOT NULL,
  name_id INT UNSIGNED, level SMALLINT UNSIGNED, sex TINYINT UNSIGNED,
  apply_time INT UNSIGNED,
  KEY idx_tong (tong_name)            -- thay ZhaoMuTable.0
) ENGINE=InnoDB DEFAULT CHARSET=binary;
```

**Chiến lược cho `TTongStruct`:** giữ nguyên thứ tự byte của từng **mảng** thành một cột `VARBINARY` riêng, chỉ tách các trường vô hướng ra cột thật. Như vậy `JX2_SaveToStruct`/`JX2_LoadFromStruct` sửa rất ít (`memcpy` vào/ra từng cột thay vì một struct lớn), và **tránh được bẫy "bản ghi lệch cỡ"** hiện có (`TONGDB.CPP:205-206` đang so kích thước 6.860 vs 6.732 và **âm thầm bỏ qua** bản ghi lệch).

**Gộp `TongMoneyDB` vào bảng `tong`:** lý do tách ban đầu chỉ là "tiền ghi thường xuyên"; với MySQL thì `UPDATE` một cột không đắt. **Nhưng phải sửa `TONGDB.CPP:315-334`** — chỗ `SearchTong` đang lấy tiền từ `MoneyTable` **đè lên** `TTongStruct.dwMoney`.

### 4.5 Schema cho các kho file phẳng

```sql
CREATE TABLE baucua_balance (          -- TIỀN XU — ưu tiên cao nhất
  role_name VARBINARY(32) NOT NULL PRIMARY KEY,
  balance BIGINT NOT NULL DEFAULT 0,
  locked  BIGINT NOT NULL DEFAULT 0,
  updated_at DATETIME NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=binary;
-- Nhập: base64_decode(khóa JSON) → tên. PHẢI chịu được trường hợp thiếu khóa "locked".
-- BẮT BUỘC gộp deposit/withdraw với thao tác trừ/cộng TASKVALUE_STATTASK_XU vào MỘT transaction
-- (hiện là 2 bước rời rạc — sập giữa chừng là nhân đôi hoặc bốc hơi Xu).

CREATE TABLE giftcode (                -- gộp 4 nguồn hiện tại làm một
  code VARCHAR(32) NOT NULL PRIMARY KEY,
  batch ENUM('new','tuan','codenew','fancung') NOT NULL,
  serial BIGINT NULL,
  used_by_role VARBINARY(32) NULL,     -- HIỆN KHÔNG CÓ — nên bổ sung, để truy vết được
  used_at DATETIME NULL,
  KEY idx_batch_used (batch, used_at)
) ENGINE=InnoDB;
-- Dùng mã: UPDATE giftcode SET used_by_role=?, used_at=NOW()
--          WHERE code=? AND used_at IS NULL;  kiểm affected_rows==1  ⇒ nguyên tử, hết đua

CREATE TABLE jx2_city (city_id TINYINT PRIMARY KEY, state TINYINT, tax INT,
  price_param INT, occupy_date INT, tax_set_date INT,
  owner_tong VARBINARY(64), master VARBINARY(64), challenger VARBINARY(64));

CREATE TABLE jx2_ladder (ladder_id INT UNSIGNED, rank_no SMALLINT,
  role_name VARBINARY(32), value BIGINT, sect INT, gender TINYINT, type TINYINT,
  PRIMARY KEY (ladder_id, rank_no));

CREATE TABLE jx2_league (lid INT AUTO_INCREMENT PRIMARY KEY, type INT, create_time INT,
  name VARBINARY(64), UNIQUE KEY uk (type, name));
CREATE TABLE jx2_league_task   (lid INT, task_id INT, val INT, PRIMARY KEY(lid, task_id));
CREATE TABLE jx2_league_member (lid INT, role_name VARBINARY(32), job INT,
  PRIMARY KEY(lid, role_name));
CREATE TABLE jx2_league_member_task (lid INT, role_name VARBINARY(32), task_id INT, val INT,
  PRIMARY KEY(lid, role_name, task_id));

-- Thay ~10 file .lua bằng MỘT bảng key-value tổng quát, để không phải thiết kế lại từng tính năng:
CREATE TABLE lua_kv (
  ns VARCHAR(64) NOT NULL,             -- vd 'congthanhchien.danhsach_bang'
  k  VARBINARY(128) NOT NULL,
  v  JSON NOT NULL,
  updated_at DATETIME NOT NULL,
  PRIMARY KEY (ns, k)
) ENGINE=InnoDB;
-- Rồi mở 4 hàm C++ cho Lua: KV_Get / KV_Set / KV_LoadAll / KV_Del
-- (đặt cạnh LuaJX2_WriteStringToFile trong KTongJX2.cpp:4003-4021, đã có khuôn sẵn).
-- ❗ RIÊNG datanganluong.lua / datatienvang.lua là TIỀN → tách bảng riêng như baucua_balance,
--    KHÔNG nhét vào lua_kv.

CREATE TABLE admin_command (           -- thay dulieu\username_kick.txt
  id BIGINT AUTO_INCREMENT PRIMARY KEY, cmd VARCHAR(32), arg VARBINARY(128),
  created_at DATETIME, done_at DATETIME NULL, KEY idx_pending (done_at, id));
```

### 4.6 Cấu hình MySQL

| Tham số | Giá trị | Lý do |
|---|---|---|
| `innodb_flush_log_at_trx_commit` | **2** (không phải 1) | Berkeley DB đang chạy `DB_TXN_NOSYNC` (`DBTable.cpp:33`) tức **không ép ghi xuống đĩa**. Đặt `=1` là **nâng độ bền lên**, nhưng sẽ **chậm hơn hiện tại** và MySQL sẽ bị quy oan. `=2` giữ nguyên mức mất dữ liệu như bây giờ mà vẫn tốt hơn (OS còn giữ). **Đây là quyết định của chủ game, không phải của kỹ thuật.** |
| `innodb_buffer_pool_size` | ≥ 2 GB | Berkeley DB hiện chỉ có 64 MB cache — đây là nâng cấp rõ |
| `innodb_log_file_size` | 1 GB | blob lớn |
| `binlog_row_image` | **MINIMAL** | **BẮT BUỘC.** `ROW` đầy đủ ghi cả ảnh trước + sau của blob ⇒ ~22 KB/lượt × 50 TPS = ~95 GB/ngày binlog |
| `expire_logs_days` | đủ dài (7–14) | binlog cho phép **khôi phục đến từng giây** — hiện tại hoàn toàn không có (log Berkeley DB bị xóa mỗi 60 phút) |
| `sql_mode` | `STRICT_ALL_TABLES` | biến cắt cụt thành lỗi |
| `max_allowed_packet` | ≥ 4 MB | bản ghi max 69 KB thì thừa, nhưng gói theo lô cần rộng |
| charset kết nối | `binary` hoặc `latin1` | **TUYỆT ĐỐI không `utf8mb4`.** Hiện `grep mysql_options` toàn `S3AccServer` **trả về rỗng** — không hề đặt charset, đang dùng mặc định máy chủ. |

**Tải dự kiến để nghiệm thu:** 18 `UPDATE`/giây (trần thực theo mã) → 50 `UPDATE`/giây (nếu sau này bỏ `break`), ~60 `SELECT`/giây lúc đăng nhập; blob trung bình 10,8 KB ⇒ ~195 KB/giây → ~540 KB/giây. **Tổng ~110 TPS — với InnoDB trên SSD đây là tải nhẹ. TPS KHÔNG PHẢI VẤN ĐỀ.**

---

## 5. KẾ HOẠCH CHUYỂN ĐỔI THEO GIAI ĐOẠN

### GIAI ĐOẠN 0 — Đo và vá, CHƯA đụng vào kho (an toàn tuyệt đối)

**Phạm vi:** không đổi bất kỳ kho dữ liệu nào. Chỉ chuẩn bị.

| Việc | File | Vì sao bắt buộc trước |
|---|---|---|
| Thêm bộ đo độ trễ lưu | `KSOServer.cpp:3403` (`SAVE_DOING`) ↔ `KSOServer.cpp:2151` (`SAVE_IDLE`) | Hiện **không có bất kỳ điểm đo nào** (`grep GetTickCount\|QueryPerformance\|timeGetTime` trên cả Goddess lẫn GameServer = rỗng). Không có đường cơ sở thì sau khi đổi **không chứng minh được nhanh hay chậm hơn** |
| **Đọc mã kết quả lưu** | `KSOServer.cpp:2146-2151` — hiện đặt `SAVE_IDLE` mà **không đọc `pPD->pDataBuffer[0]`** | Xem rủi ro R3 |
| Vá đường re-save vô hạn | `KPlayer.cpp:53`, `:1093` | Xem rủi ro R2 |
| Vá khóa nhân vật thành loại trừ | `ClientNode.cpp:715` | Xem rủi ro R4 |
| Bỏ `Sleep(1)` + tách worker | `ClientNode.cpp:186-207` | Xem rủi ro R1 |
| Bỏ `fflush` mỗi dòng + xoay vòng log | `IDBRoleServer.cpp:25`, `:748-749` | `goddess_log.txt` đã 22 MB, không bao giờ đóng |
| Lấy schema MySQL thật | trên máy chủ MySQL | `SHOW CREATE VIEW View_AccountMoney` + 3 `SHOW CREATE TABLE` |
| Quyết định số phận `S3RelayServer` | — | Đang chết lặng vì thiếu bản ghi `ServerList` tên `free` |

**Kiểm chứng:** server chạy bình thường 3–7 ngày, log độ trễ có số liệu, không lỗi mới.
**Quay lui:** đổi lại binary cũ (đã có bản sao cạnh bin). Không đụng dữ liệu ⇒ quay lui tức thời.

### GIAI ĐOẠN 1 — Tiền và giftcode lên MySQL (giá trị cao, rủi ro thấp)

**Phạm vi:** `baucua\deposits.json`, giftcode (4 nguồn), `datanganluong.lua`, `datatienvang.lua`.
**Module đụng đến:** `Core\Src\BauCua.cpp`, `Core\Src\GiftCodeManager.cpp`, vài file Lua.
**Vì sao làm trước:** đây là **tiền thật**, dữ liệu bé (7 người + 6.028 mã), độc lập hoàn toàn với `roledb`, và giải quyết luôn hai lỗi nguyên tử (giftcode đua nhau, Xu nhân đôi).
**Kiểm chứng:** đối chiếu tổng số Xu trước/sau; tổng số mã đã dùng trước/sau; thử nhận cùng một mã bằng 2 nhân vật đồng thời.
**Quay lui:** giữ song song đường ghi file cũ trong 1 tuần; tắt cờ là quay về file.

### GIAI ĐOẠN 2 — `roledb` sang MySQL theo phương án A (giai đoạn chính)

**Phạm vi:** thay ruột lớp `ZDBTable` trong Goddess.
**Module đụng đến:** `Goddess\DBTable.cpp/.h` (viết lại), `Goddess\IDBRoleServer.cpp` (5 hàm), `Goddess\DBBackup.cpp` (thay backup + bảng xếp hạng). **`ClientNode.cpp`, `Bishop`, `GameServer`, `Core` KHÔNG sửa.**

**Giữ nguyên hình dạng API công khai của `ZDBTable`** — `open/close/commit/add/remove/first/search/next/search_key/next_key/closeCursor/deadlock/removeLog` — và **giữ `ZCursor` với các trường `key/key_size/data/size`**, vì bên gọi phụ thuộc hoàn toàn vào quy ước "`next()` trả false thì cursor tự hủy" (`IDBRoleServer.cpp:567`, `:635`; `DBBackup.cpp:486`).
⚠️ **`closeCursor` giải phóng bằng `free()`** ⇒ lớp mới **bắt buộc cấp phát bằng `malloc()`**, cấm `new[]`.
⚠️ `DBBackup.cpp:885` **sửa trực tiếp vùng nhớ `cursor->data` rồi ghi lại**. Buffer của lớp MySQL phải có vòng đời tương đương (không được là buffer tái sử dụng của `mysql_stmt_fetch`).

**Công cụ di trú:** viết chương trình **đọc file `.bak`** (không mở Berkeley DB đang chạy), theo khuôn `[4B keysize][key][4B datasize][data]`, **kèm ba phép kiểm chứng chéo** đã nêu ở 3.1, rồi `INSERT` sang MySQL **không lọc CRC** (83,7% sai) và **tính lại CRC** khi nhập.

**Quy trình cắt chuyển (cutover):**
1. Dừng **toàn cụm** (GameServer trước, đợi xả hết lượt lưu treo → Bishop → S3Relay → Goddess).
2. Bấm "Manual backup" trên giao diện Goddess → lấy bản `.bak` **lạnh** (bản tự động 03:00 là **backup nóng, ảnh chụp lem**).
3. Chạy công cụ nhập. Đối chiếu: số bản ghi (`playerlist.txt`), tổng byte, số tài khoản, danh sách 25 nhóm tên va chạm phải nhập đủ **cả 52 bản ghi**.
4. Khởi động lại theo thứ tự: **mysqld → Goddess → GameServer → Bishop**.

**Kiểm chứng:** đăng nhập 5 tài khoản mẫu (gồm 1 tài khoản có tên nhân vật dấu tiếng Việt và 1 cặp tên va chạm HOA/thường); kiểm tra danh sách nhân vật, trang bị, rương, kinh mạch, task, bang hội; chạy 30 phút rồi đối chiếu số lượt ghi + độ trễ với đường cơ sở ở Giai đoạn 0.

**Quay lui:** giữ nguyên thư mục `database\` **KHÔNG ĐỘNG VÀO** (chỉ đọc `.bak`). Muốn lùi thì đổi lại binary Goddess cũ và khởi động — dữ liệu Berkeley DB vẫn ở đúng trạng thái lúc dừng, chỉ mất phần chơi sau thời điểm cắt.

**Chạy song song?** `ZDBTable::add` là **một hàm duy nhất** nên "ghi cả hai" rất dễ làm. **Nhưng không khuyến nghị**: hai môi trường Berkeley DB `DB_RECOVER|DB_PRIVATE` không khóa được nhau — đó chính là kịch bản đã làm hỏng `roledb` ngày 17/08 (dấu vết còn nguyên: `database_hong_2320\`, `database_chi49_2304\`, `_cuuho\database_nguon\`). Nếu chủ game vẫn muốn đối chiếu, chỉ nên **ghi MySQL + đọc Berkeley DB** rồi so sánh ngoại tuyến, tuyệt đối không hai tiến trình cùng mở `database\`.

### GIAI ĐOẠN 3 — S3Relay (bạn bè + bang hội) sang MySQL

**Phạm vi:** `S3Relay\DBTable.cpp`, `TONGDB.CPP`, `FriendMgr.cpp`.
**Việc phải vá trước:** dùng-sau-khi-giải-phóng ở `FriendMgr.cpp:851-861`; tràn `_alloca` ở `FriendMgr.cpp:912` (tính 33 B/bạn trong khi thực tế cần tới 98 B do `_GROUP_NAME_LEN=64`); ba con trỏ `TongTable`/`MemberTable`/`MoneyTable` được `delete` mà **không gán NULL** (`TONGDB.CPP:105-129` — riêng `m_ZhaoMuTable` thì có gán NULL, ngược với một khảo sát đã viết).
**Lợi ích ngay:** bỏ được kiểu "mỗi lần ghi = 1 sync + 1 checkpoint toàn DB", và bỏ được 5 file log 16 MB.
**Cảnh báo hiệu năng:** `JX2_TimerTick` (`KTongJX2Relay.cpp:2401-2455`) có thể ghi lại **TOÀN BỘ** `TongTable` mỗi 750 giây. **Phải gom lô hoặc chỉ ghi bang thật sự đổi**, đừng bắt chước 1-1.
**Quay lui:** như Giai đoạn 2 — giữ nguyên 5 thư mục kho cũ.

### GIAI ĐOẠN 4 — File phẳng còn lại + dọn dẹp

`jx2citywar/jx2ladder/jx2league`, `StatData.dat`, ~10 file `.lua` (qua bảng `lua_kv`), log người chơi (`dulieu\player_log\**` → bảng `player_log` phân vùng theo tháng), `username_kick.txt` → `admin_command`.
Xóa hẳn: `account.db`, `role.db`, `GMPrivate.mdb`, `PlayerRankLog.ini`, `data\giftcode\`, `data\bbcode\`, `tongset.ini`+`chatset.ini` ở gốc, `testgop\`, `bin\server\StatData.dat`.
Dọn mã chết: 18 file `*DAO.cpp`, `S3PDB_MSSQLServer_Connection.cpp` (đang bị biên dịch vô ích, kéo theo `msado15.tlh` 90 KB), `#include <jdbc/...>` ở `S3P_MySQL_Result.cpp:4-5`, 2 mục `[card]`/`[role]` trong `DataBase.ini`.

### GIAI ĐOẠN 5 (tùy chọn, sau cùng) — Tách `role_item` + `role_task` ra cột thật

Chỉ làm nếu chủ game thực sự cần truy vấn SQL trên vật phẩm. **Bọc trong một transaction duy nhất**, nếu không sẽ mở đường nhân đôi vật phẩm.

---

## 6. RỦI RO & BẪY (xếp theo mức nguy hiểm)

### R1 — 🔴 Nút cổ chai KHÔNG phải MySQL mà là Goddess, và nó nặng hơn mô tả

`CClientNode::m_hThread` là biến **static** (`ClientNode.h:48`, `.cpp:19`) — **một luồng duy nhất** xử lý toàn bộ yêu cầu DB của mọi GameServer. Tệ hơn: **toàn bộ vòng duyệt `m_theClientMap` nằm TRONG `CCriticalSection::Owner lock(m_csCL)`** (`ClientNode.cpp:186-200`). Một câu MySQL chặn ở giữa sẽ **giữ luôn khóa bảng kết nối**, chặn cả việc nhận/ngắt kết nối chứ không chỉ làm chậm DB. Cộng thêm `Sleep(1)` mỗi 2 vòng (`:204-207`), và Windows **không gọi `timeBeginPeriod`** nên `Sleep(1)` thực tế 1–15,6 ms.
**Phòng:** tách I/O DB ra worker pool + connection pool; **KHÔNG gọi MySQL trong phạm vi `m_csCL`**; bỏ `Sleep`. Làm ở Giai đoạn 0, đo lại trên Berkeley DB trước.

### R2 — 🔴 Đường ghi không có phanh: DB chậm ⇒ hệ tự sinh thêm lệnh ghi

`KPlayer.cpp:53` đặt `defPLAYER_SAVE_TIMEOUT = 30*20 = 600` tick (~33 giây), và `KPlayer.cpp:1093`:
```
if (m_uMustSave == SAVE_DOING && GetGameTime() - m_ulLastSaveTime > defPLAYER_SAVE_TIMEOUT)
    return TRUE;    // cho phép AutoSave PHÁT LẠI
```
Ở 1.500 người, chu kỳ thực là ~83 giây **> 33 giây** ⇒ **MỌI nhân vật luôn ở trạng thái quá hạn** ⇒ `CanSave()` luôn TRUE ⇒ phát lại liên tục. Trong log đã có bằng chứng: 10 lượt `SaveRoleInfo` trong 1 giây **của cùng một tên**. Đây không phải "bệnh lý ngẫu nhiên" mà là **đường re-save được code cố ý**. Berkeley DB nhanh nên chưa nổ; MySQL chậm hơn (round-trip mạng + fsync) sẽ kích hoạt vòng khuếch đại.
**Phòng:** vá `CanSave()` — không phát lại vô hạn, đặt trần số lần thử, và đưa timeout lên trên chu kỳ thực (hoặc thêm hàng đợi có giới hạn). **Bắt buộc làm ở Giai đoạn 0.**

### R3 — 🔴 Mất dữ liệu im lặng: kết quả ghi bị vứt bỏ

`ClientNode.cpp:445-450` trả `pDataBuffer[0] = (nResult == 1) ? 1 : -1`. Nhưng `KSOServer.cpp:2146-2151`:
```cpp
case s2c_roleserver_saverole_result:
    { TProcessData* pPD = (TProcessData *)pData;
      int nIndex = pPD->ulIdentity;
      m_pCoreServerShell->SetSaveStatus(nIndex, SAVE_IDLE); }
```
**Không hề đọc mã kết quả.** Với MySQL, mọi lỗi tạm thời (deadlock 1213, lock wait timeout 1205, mất kết nối, gói quá lớn) đều thành "lưu thất bại mà không ai biết".
**Nguy hiểm nhất:** lượt lưu **CUỐI** lúc đăng xuất / đổi server (`bUnLock = true`, `KSOServer.cpp:3456`, `:3552`, `:3872`) — sau đó không còn lượt nào để tự chữa ⇒ **mất trọn phiên chơi**.
Còn một đường thất bại im lặng thứ hai: `_SaveRoleInfo` trả `nResult = 0` khi nhân vật không bị khóa bởi chính kết nối đó (`ClientNode.cpp:430-437`) — không log, không cảnh báo.
**Phòng:** đọc mã kết quả; thất bại thì **giữ `SAVE_REQUEST`** và **chặn đăng xuất/chuyển server** cho tới khi lưu được; ghi log riêng cho mọi lần thất bại.

### R4 — 🔴 Khóa nhân vật KHÔNG phải khóa loại trừ ⇒ đường nhân đôi/rollback vật phẩm

`ClientNode.cpp:715` gán `m_csRoleLock[szRole] = m_nIndentity` **VÔ ĐIỀU KIỆN** — không kiểm tra entry đã tồn tại và thuộc về kết nối khác. GameServer thứ hai chỉ cần gửi `c2s_roleserver_lock` là **cướp được khóa**. Kết hợp với `_SaveRoleInfo` chỉ ghi khi `IsRoleLockBySelf` (`:430`): khi đổi GameServer, nếu gói lock của GS-B tới Goddess **trước** gói lưu cuối của GS-A, thì **lưu của A bị từ chối im lặng** và GS-B nạp blob **CŨ**. Goddess xử lý round-robin 1 gói/kết nối/vòng (`:186-200`) nên **thứ tự giữa hai kết nối không được bảo đảm**.
**Phòng:** khóa phải kiểm tra chủ sở hữu; sang MySQL thì đặt ở **tầng DB** (`UPDATE … WHERE locked_by IS NULL OR locked_by = ?` rồi kiểm `affected_rows`) kèm **TTL/heartbeat**, vì khóa của kết nối rớt hiện chỉ bị đặt `-1` chứ không hết hạn (`UnlockAllRole`, `ClientNode.cpp:770-780`).
*Đính chính một khảo sát:* đường đăng xuất bình thường **CÓ** `erase(it)` (`:720-724` và `UnlockRoleSelf` `:756-767`); chỉ `UnlockAllRole` (gọi từ destructor khi **rớt kết nối**) mới để lại entry `-1`. Câu "map chỉ tăng, không ai dọn" là sai.

### R5 — 🔴 Mã hóa ký tự: đã đo, sẽ mất 27 nhân vật và hỏng 205 tên

Xem 4.1. Số liệu đo trên kho production, không phải phỏng đoán.
**Phòng:** `VARBINARY` + charset `binary` cho tên nhân vật; **giữ `_ci` cho tên tài khoản**; chạy lại phép đếm va chạm trên kho production **ngay trước khi cắt chuyển** (danh sách có thể đổi).
**Bẫy phụ:** Goddess **chỉ hạ chữ thường `caccname` khi TẠO** nhân vật (`IDBRoleServer.cpp:477-484`, trong nhánh `bAutoInsertWhenNoExistUser`), **không hạ khi lưu thường**. Nhưng `GetRoleListOfAccount` **lại hạ chữ thường khóa tra cứu** (`:590-594`). Trong Berkeley DB, khóa phụ được **sinh lại từ blob mỗi lần `put`** nên khóa index bám theo blob. Nghĩa là: nếu blob có `caccname` chữ hoa, nhân vật đó **tồn tại nhưng không hiện trong danh sách chọn nhân vật**. Khi di trú, chọn "lấy nguyên từ blob" hay "hạ chữ thường" sẽ **thay đổi hành vi** — phải chạy thống kê đối chiếu trước.

### R6 — 🟠 CRC32: 83,7% bản ghi hiện SAI

Xem 3.1. Nếu công cụ nhập kiểm CRC thì **loại oan 1.508/1.802 nhân vật, trong đó có đúng những người top bảng xếp hạng**.
**Bẫy phụ chưa ai nêu:** phía GameServer, `sProcessData.nDataLen` được tính **TRƯỚC** dòng `pData->dwDataLen += 4` (`KSOServer.cpp:3336-3342`), tức `nDataLen` trong header gói **thiếu 4 byte** so với payload thật. Hiện vô hại vì Goddess không dùng `nDataLen` trên đường lưu. **Nhưng lớp MySQL mới nếu "làm đúng" bằng cách tin `nDataLen` sẽ cắt cụt 4 byte CRC của MỌI bản ghi ⇒ 100% lượt lưu bị từ chối.**
**Phòng:** nhập không lọc CRC, tính lại khi nhập; giữ nguyên quy ước "4 byte cuối là CRC, `dwDataLen` ĐÃ bao gồm 4 byte đó"; vá `SaveStatInfo` để tính lại CRC (hoặc bỏ hẳn đường ghi ngược này khi lên MySQL — dùng `UPDATE role SET … WHERE role_name=?` cho hai cột thứ hạng).

### R7 — 🟠 Ba loại chỉ số kích thước, không cái nào dùng để tính dung lượng được

Khảo sát đưa ra ba con số lệch nhau 15 lần: 1.652 B (dump máy test, 1.000/1.003 là bot), 3 KB (chia thẳng kích thước file BTree — **sai phương pháp**, có overhead trang 8.192 B), 24.765 B (log của ~6 nick test).
**Con số đúng để tính:** kho production **1.802 nhân vật, trung bình 10,8 KB, p95 22,3 KB, max 69,3 KB, tổng 19,4 MB**. Quy chiếu: 20.000 nhân vật ≈ 216 MB; 100.000 ≈ 1,08 GB.

### R8 — 🟠 Không có bất kỳ đường cơ sở đo lường nào

`grep GetTickCount|QueryPerformance|timeGetTime` trên cả `Goddess` lẫn `GameServer` = **rỗng**. `GameServer.log` **không có dấu thời gian**. `goddess_log.txt` chỉ tới giây. `Backup.log` bị **ghi đè mỗi lần** nên không có lịch sử.
**Phòng:** thêm bộ đo ở Giai đoạn 0. Nếu không, sau khi đổi sẽ không ai chứng minh được điều gì.

### R9 — 🟠 File `.bak` không sạch, và backup hiện tại là ảnh chụp lem

Bốn bản ghi rác ở đầu file, 205.766 byte rác (`0xCD`/`0xFD`/`0xDD`) ở cuối. Ngoài ra `CDBBackup::Backup()` duyệt cursor **trên chính DB đang chạy, không transaction, không khóa** (`DBBackup.cpp:410`) trong khi GameServer vẫn ghi.
Thêm nữa: `CDBDump::Open` **từ chối ghi đè** file đã tồn tại (`DBDumpLoad.cpp:20-22`) và `Backup()` **không kiểm kết quả `Open`** ⇒ trùng tên thì backup **thất bại hoàn toàn trong im lặng**, mà `Backup.log` vẫn báo "DB Dump Finished". **Đừng tin log này — phải kiểm kích thước file `.bak`.**
**Phòng:** cắt chuyển bằng bản backup **thủ công sau khi dừng server**; công cụ đọc `.bak` phải có 3 phép kiểm chứng chéo.

### R10 — 🟠 Điểm chết đơn lẻ `gm-kingsoft` và `free` trong `ServerList`

`S3PDBConnectionPool.cpp:100-103` đọc `ServerList` với `cServerName='gm-kingsoft'` để lấy `g_nGMID` **lúc khởi động**. Mất bản ghi này thì `g_nGMID = 0`, và `S3PAccount.cpp:166` (`else if (NewClientID == GetGMID())`) sẽ coi **mọi tài khoản `iClientID=0` là "đang đăng nhập bởi GM"** và trả thành công sai.
Tương tự, `S3RelayServer` cần bản ghi `cServerName='free'`, hiện **không có**.
**Phòng:** khi di trú `account`, giữ nguyên các bản ghi này **kể cả giá trị `iid`**.

### R11 — 🟡 Không có transaction xuyên hai bảng ở tầng tài khoản

`grep BEGIN|START TRANSACTION|COMMIT|ROLLBACK|mysql_autocommit` toàn `Sword3PaySys` = **không có dòng mã nào**. `Login` (`S3PAccount.cpp:64` rồi `:71`), `LoginGame` (`:109`+`:116`), `Logout` (`:153`+`:162`) đều là 2 lệnh `UPDATE` rời rạc trên `Account_info` và `Account_habitus`. Mất kết nối giữa chừng ⇒ `iClientID` lệch nhau ⇒ tài khoản **kẹt online** hoặc trừ giờ sai.
**Phòng:** bọc từng cặp vào một transaction. Đây là nâng cấp, không phải di trú.

### R12 — 🟡 Tên bảng viết hoa/thường không nhất quán — bom hẹn giờ nếu đổi sang Linux

Cùng một bảng gọi bằng 4 cách trong cùng một file: `Account_info` (:36,64,109,153,349,377,512), `Account_Info` (:408), `Account_habitus` (:71,116,162), `Account_Habitus` (:134,188,363,462). Trên Windows `lower_case_table_names=1` nên không sao; trên **Linux mặc định `=0`** thì `Account_Info` và `Account_info` là **hai bảng khác nhau** ⇒ `GetLockAccount` và `GetLeftSecondsOfDeposit` lỗi ⇒ `Login` trả `E_ACCOUNT_NODEPOSIT` ⇒ **không ai đăng nhập được**.

### R13 — 🟡 Ba bản `S3DBInterface.h`, sửa nhầm là hỏng toàn bộ

Xem cảnh báo ở 3.1. **Phép thử 1 giây trước khi gõ phím: `wc -l D:\GAMEDEVNEW\Lib\S3DBInterface.h` phải ra 581.**

### R14 — 🟡 Kiến trúc 32/64-bit

`Goddess.exe`, `S3Relay.exe`, `Sword3PaySys.exe` = **x86**; `CoreServer.dll`, `GameServer.exe` = **x64**. Connector MySQL cho Goddess **bắt buộc 32-bit** (`libmySQL.dll` trong bin đã là x86 — dùng được). Không gian địa chỉ 2 GB giới hạn kích thước pool/buffer.
Blob **không** phụ thuộc bitness (không con trỏ, không `long`, không `size_t`). Nhưng **`TProcessData` thì có**: đã có sẵn khối chú thích cảnh báo tại `Core\Src\KProtocol.h:1640-1650`. **Đã kiểm chứng: `TProcessData` = 11 byte** (`nProtoId` 1 + `nDataLen` 4 + `ulIdentity` 4 + `bLeave` 1 + `pDataBuffer[1]` 1), dưới `#pragma pack(1)` tại `Headers\KProtocol.h:18-19`. *(Một khảo sát ghi "14 byte" — sai, dùng số đó sẽ lệch 3 byte khi tính offset gói.)* **Cấm "hiện đại hóa" `nDataLen` thành `size_t`.** Cũng đừng hồi sinh `TDBFriendsData` — nó có `void* pBuffer`.
Định dạng `.bak` **thì có** phụ thuộc: `CDBDump` ghi `size_t`, Goddess chỉ build Win32 ⇒ 4 byte. Công cụ phải tự nhận diện 4 vs 8.

### R15 — 🟡 Rủi ro do chính đề xuất sinh ra

- **Tách bảng ở Phương án B mà không bọc transaction ⇒ nhân đôi vật phẩm.** Mô hình blob hiện tại đang bảo vệ ta khỏi lỗi này.
- `nTaskCount` kiểu BYTE với `MAX_TASK = 3000` là lỗi **thật** (`Lib\S3DBInterface.h:158` vs `KPlayerTask.h:13`), nhưng **đã đo trên kho production: tối đa 71 biến, 0 bản ghi vượt 255**. ⇒ **Không phải rủi ro cấp bách**, nhưng công cụ di trú vẫn nên tính số task bằng `(dwItemOffset - dwTaskOffset) / 8` thay vì tin `nTaskCount`.
- `MaxRoleCount=4` (`Goddess.cfg`) vs `s_nRoleListCount=3` (`Bishop\GamePlayer.cpp:37`) vs `MAX_PLAYER_IN_ACCOUNT=3` (`Core\CoreUseNameDef.h:5`) là mâu thuẫn **thật**, và mảng tạm `S3DBI_RoleBaseInfo DBI[4]` (`ClientNode.cpp:316`) sẽ **tràn ngăn xếp** nếu ai nâng `MaxRoleCount` lên >4 rồi sửa Bishop xin nhiều hơn. **Nhưng đã đo: kho production không có tài khoản nào quá 3 nhân vật** ⇒ không có "nhân vật ma" cần xử lý.
- `nRoleListCount` lấy **thẳng từ gói tin** (`ClientNode.cpp:298`) rồi truyền làm `nMaxCount` — **bất kỳ ai chạm được cổng 5011 và gửi số lớn đều gây tràn ngăn xếp**. Cổng 5011 **không có xác thực** (`ClientNode.cpp:82-89` phân phối lệnh ngay cho mọi kết nối).

### R16 — 🟡 Lỗi biên có sẵn sẽ lộ ra khi refactor

`ClientNode.cpp:471-475` và `:573-576`: kẹp `nDataLen <= 64` rồi `szRoleName[nDataLen] = '\0'` trên `char[64]` ⇒ **ghi ngoài mảng 1 byte** khi `nDataLen == 64`.
`GetRoleInfo` `memcpy` tới 327.679 byte vào buffer 327.680 byte đã trừ header ⇒ **tràn heap ~15 byte** với nhân vật lớn nhất (trần ở `:475` chỉ chặn `>= 327680`, không trừ header).
`DBTable.cpp:83`: `while(--index)` **bỏ sót index 0** khi dọn lỗi.
`IDBRoleServer.cpp:367`: `if(!hRemoveLogThread) TerminateThread(hRemoveLogThread, 0)` — **điều kiện đảo ngược**, không bao giờ giết đúng.
`PlayerCreator.cpp:26-32`: `if (!m_pRoleData[i]) delete[] …` — **đảo ngược**, rò rỉ cả 10 buffer.
Bộ phát hiện deadlock **đã bị comment** (`IDBRoleServer.cpp:64-77`) ⇒ nếu hệ hiện tại "thỉnh thoảng mất 1 lần lưu", nguyên nhân có thể là deadlock hết 16 lần thử. **Đừng lấy hiện trạng làm chuẩn vàng khi so sánh trước/sau.**

### R17 — 🟡 Thứ tự khởi động và phụ thuộc mới

Hiện Goddess **không phụ thuộc gì** khi khởi động. Thêm MySQL tạo chuỗi mới: `mysqld → Goddess → GameServer → Bishop`.
Nếu `mysqld` chưa sẵn sàng, `db_table->open()` thất bại ⇒ `InitDBInterface` trả FALSE (`IDBRoleServer.cpp:91`) ⇒ **Goddess VẪN CHẠY nhưng không có DB**, người chơi đăng nhập sẽ thấy danh sách nhân vật **trống trơn**.
**Bishop KHÔNG tự kết nối lại Goddess** (`Intercessor.cpp:200-217` chỉ báo lên giao diện, không có logic reconnect — khác `CSmartClient` nối S3AccServer có luồng reconnect ở `SmartClient.cpp:146-181`) ⇒ **restart Goddess bắt buộc restart cả Bishop**.
**Phòng:** thêm vòng thử lại có giới hạn + **từ chối mở cổng 5011 khi chưa có DB**; viết lại kịch bản khởi động cụm (chú ý `StartGameSV.exe`/`StartGameSV.cfg` vẫn còn trong `bin`, trỏ ra IP công cộng `103.77.167.8` — module đã chết về mã nguồn nhưng file cấu hình còn là bẫy).

### R18 — 🟡 Lua vẫn sẽ ghi file sau khi lên MySQL

`LuaJX2_WriteStringToFile` (`Core\Src\KTongJX2.cpp:4003-4021`) cho phép Lua `fopen(bất kỳ đường dẫn, "ab")`; `openfile/write` của Lua 4.0 không bị chặn. Nếu chỉ chuyển dữ liệu mà **không sửa ~40 điểm gọi `SaveData`/`LuuBang`/`openfile`** trong `script\`, server sẽ tiếp tục ghi song song ra file và **MySQL lệch ngay ngày đầu**.
⚠️ Cây `scriptjx2\` **chưa ai quét** — phải rà thêm.

### R19 — 🟢 Thư mục log thiếu làm mất dữ liệu âm thầm

`logs\` (cho `tong_jx2.log`, `script_jx2.log`) và `s3relay_log\` (cho log chat riêng) **KHÔNG tồn tại** trong `bin`. `fopen` chế độ `"a"` **không tự tạo thư mục** ⇒ đang mất trắng. Nếu định dùng log này làm bằng chứng tranh chấp thì **phải tạo thư mục TRƯỚC**.

### R20 — 🟢 Mật khẩu lưu dạng chữ rõ + binlog

`S3PAccount.cpp:36` so sánh `cPassword = '%s'` trực tiếp. Khi lên MySQL, bản sao/binlog đều chứa mật khẩu thô. Không phải lỗi do MySQL sinh ra, nhưng **rủi ro lộ lọt TĂNG** vì binlog ghi lại câu lệnh.
**Phòng:** `binlog_format=ROW` + hạn chế quyền đọc binlog, hoặc tắt binlog cho DB `account`.

---

## 7. CÂU HỎI CẦN CHỦ GAME QUYẾT ĐỊNH

| # | Câu hỏi | Vì sao quan trọng | Gợi ý |
|---|---|---|---|
| **Q1** | **Mục tiêu thật sự là gì?** (a) hết nỗi sợ hỏng DB; (b) truy vấn được dữ liệu bằng SQL cho công cụ GM/web; (c) chạy nhiều máy chủ; (d) nhanh hơn | Quyết định chọn Phương án A hay B, và có cần Giai đoạn 5 không | Nếu là (a) hoặc (c) → **Phương án A là đủ**. Nếu là (b) → cần thêm Giai đoạn 5. Nếu là (d) → **MySQL sẽ KHÔNG nhanh hơn**, nút cổ chai là R1 |
| **Q2** | **Chấp nhận dừng server bao lâu để cắt chuyển?** | Cách an toàn duy nhất là dừng cụm, backup lạnh, nhập, khởi động lại | Ước: 30–60 phút cho 1.802 nhân vật |
| **Q3** | **Độ bền dữ liệu: giữ như hiện tại hay nâng lên?** | Hiện chạy `DB_TXN_NOSYNC` = **không ép ghi xuống đĩa**, mất điện là mất giao dịch gần nhất. `innodb_flush_log_at_trx_commit=2` giữ nguyên mức đó; `=1` bền hơn **nhưng chậm hơn hiện tại** | Đề xuất bắt đầu `=2`, đo, rồi cân nhắc `=1` |
| **Q4** | **Có giữ tương thích ngược không?** Tức có cần đọc lại file `.bak` cũ, có giữ chức năng **MergeDB** (gộp 2 server) không? | Thư mục `testgop\` cho thấy đã từng gộp server. Nếu bỏ MergeDB thì **mất chức năng gộp server** | — |
| **Q5** | **Module `S3RelayServer` — giữ hay bỏ?** | Đang chạy nhưng **xác thực thất bại** (thiếu bản ghi `ServerList` tên `free`). Nó dùng **VIEW `View_AccountMoney`** mà schema phải có | Nếu bỏ được thì bớt một tiến trình ghi vào DB `account` |
| **Q6** | **`MaxRoleCount`: chốt 3 hay 4?** | Hiện Goddess cho 4, Bishop chỉ liệt kê 3, mảng `DBI[4]` là trần cứng | Kho production chưa ai vượt 3 ⇒ **chốt 3 là an toàn nhất**; muốn tăng phải nâng mảng `DBI[4]` TRƯỚC |
| **Q7** | **Đường ghi từ Goddess có được phép chậm hơn không?** | Quyết định có phải làm R1 (worker pool) trước hay chỉ vá tối thiểu | Đề xuất: **bắt buộc làm R1** |
| **Q8** | **Có sẵn bản `DBTranslate` hoàn chỉnh của "Phong Kieu (c) 2021" không?** | `Sources\MultiServer\DBTranslate` có khung công cụ chuyển đổi với struct `TRoleDataNew` (một định dạng bản ghi MỚI), nhưng hàm `DBTranslate()` **rỗng hoàn toàn**. Thư mục `database\` của nó có dấu vết chạy ngày 12/08/2026 | Nếu có bản đầy đủ thì tiết kiệm nhiều công |
| **Q9** | **Có bật lưu log chat không?** | Hiện log chat riêng **mất trắng** (thiếu thư mục), chat kênh/bang/đội **không có đường ghi nào**. Đây là **tính năng mới**, không phải di trú | — |
| **Q10** | **Mật khẩu: có nhân dịp này băm không?** | `cPassword` đang lưu chữ rõ. Đổi sẽ phải sửa cả client | Nên tách thành đợt riêng |

---

## 8. ƯỚC LƯỢNG CÔNG SỨC (chỉ ước lượng, chưa làm gì)

### 8.1 Theo giai đoạn

| Giai đoạn | File phải sửa | Dòng ước tính | Mức rủi ro | Ghi chú |
|---|---|---|---|---|
| **GĐ0** — Đo & vá | `KSOServer.cpp`, `KPlayer.cpp`, `ClientNode.cpp/.h`, `IDBRoleServer.cpp` | ~250–400 | **Trung bình** | Đụng vòng lặp chính và cơ chế khóa. **Phải phản biện kỹ và test nhiều** |
| **GĐ1** — Tiền & giftcode | `BauCua.cpp`, `GiftCodeManager.cpp/.h`, `KCore.cpp`, 3–4 file `.lua` + 1 lớp MySQL nhỏ | ~400–600 | **Thấp** | Dữ liệu bé, độc lập, dễ đối chiếu |
| **GĐ2** — `roledb` (Phương án A) | `Goddess\DBTable.cpp/.h` (**viết lại**), `IDBRoleServer.cpp` (5 hàm), `DBBackup.cpp`, `Goddess.vcxproj` | ~900–1.300 | **Cao** | Đây là trái tim. Có công cụ di trú riêng |
| **GĐ2b** — Công cụ di trú | 1 công cụ mới (C++ hoặc Python) | ~300–500 | Trung bình | Đọc `.bak`, 3 phép kiểm chứng chéo, nhập, đối chiếu |
| **GĐ3** — S3Relay | `S3Relay\DBTable.cpp/.h`, `TONGDB.CPP/.H`, `FriendMgr.cpp/.h`, `KTongJX2Relay.cpp` | ~900–1.400 | **Cao** | 5 kho, 4 struct, 2 khóa phụ, + vá 3 lỗi có sẵn |
| **GĐ4** — File phẳng + dọn | `KJx2CityWar.cpp`, `KJx2League.cpp`, `KJx2SharedStore.cpp`, `KTongJX2.cpp` (thêm 4 hàm Lua KV), `DBBackup.cpp`, ~15 file `.lua`, `save_log.lua`, `timerserver.lua` | ~800–1.200 | Trung bình | Nhiều file nhưng mỗi chỗ nhỏ |
| **GĐ5** (tùy chọn) — Tách cột | `KPlayerDBFuns.cpp` (**viết lại 10 hàm**), `IDBRoleServer.cpp` | ~1.100–1.600 | **Rất cao** | Chỉ làm nếu Q1 = (b) |

**Tổng cho GĐ0 → GĐ4 (không kể GĐ5): khoảng 3.500–5.400 dòng, ~20–28 file.**

### 8.2 Bảng rủi ro theo module

| Module | Độ rủi ro | Lý do |
|---|---|---|
| `Goddess\DBTable.cpp` | 🔴 Rất cao | Viết lại hoàn toàn; phải giữ đúng ngữ nghĩa `ZCursor`, `malloc/free`, vòng đời buffer |
| `Core\Src\KPlayer.cpp` (`CanSave`) | 🔴 Cao | Đụng vòng lặp game, ảnh hưởng mọi người chơi |
| `Goddess\ClientNode.cpp` (khóa + worker) | 🔴 Cao | Đụng đồng bộ đa luồng và khóa `m_csCL` |
| `S3Relay\FriendMgr.cpp` | 🟠 Cao | Có dùng-sau-khi-giải-phóng và tràn `_alloca` **đang tồn tại** |
| `S3Relay\TONGDB.CPP` | 🟠 Trung bình–cao | 4 kho, 2 khóa phụ, bẫy "bản ghi lệch cỡ" |
| `Core\Src\BauCua.cpp` | 🟢 Thấp | Nhỏ, độc lập, dễ kiểm |
| `Core\Src\GiftCodeManager.cpp` | 🟢 Thấp | ~65 dòng, hai hàm |
| `settings\jx2*.txt` (3 hàm save/load) | 🟢 Thấp | Đã có khuôn `.tmp` + `MoveFileEx`, chỉ đổi nơi lưu |
| Công cụ di trú | 🟠 Trung bình | Chạy một lần, nhưng sai là hỏng dữ liệu vĩnh viễn ⇒ phải chạy trên **bản sao**, đối chiếu 2 chiều |

### 8.3 Ba việc phải làm ngay, trước cả GĐ0

1. **Chốt bộ nào đang chạy thật.** `bin\server` và `bin\multiserver` dùng **CHUNG** `DataBase.ini` (cùng `127.0.0.1`, cùng DB `account`, cùng `root/123456`), và `Goddess.exe` giống hệt nhau ở **3 vị trí** (dấu vân tay của `PostBuildEvent` cấu hình **Debug** — bản đang chạy là **bản Debug**, link `libdb181sd.lib` biên dịch với macro `DIAGNOSTIC`). Không chốt được thì lặp lại đúng kịch bản đã làm hỏng `roledb` ngày 17/08.
2. **Lấy schema MySQL thật** (`SHOW CREATE VIEW View_AccountMoney` + 3 `SHOW CREATE TABLE`). Không có thứ này thì mọi thiết kế bảng đều là phỏng đoán.
3. **Sao lưu toàn bộ `bin\multiserver` và `bin\server` sang ổ khác** trước khi động vào bất cứ thứ gì.

---

## PHỤ LỤC — Các đính chính đối với 8 bản khảo sát

| Khẳng định trong khảo sát | Sự thật đã kiểm chứng |
|---|---|
| KHO 5 & KHO 8 mô tả `TRoleData` theo `Headers\S3DBInterface.h` (`szName[100]`, không pack) | **SAI.** Bản được biên dịch là `Lib\S3DBInterface.h` (581 dòng, pack(1) dòng 15, `szName[32]` dòng 46). Dùng phần "bản ghi" của KHO 5/KHO 8 để viết công cụ = hỏng toàn bộ |
| KHO 1: `TProcessData` = 14 byte | **SAI**, là **11 byte** (`Headers\KProtocol.h:18-19` pack(1), `:1107-1114`) |
| KHO 2: "roledb 2.998.272 / 1003 ⇒ ~3 KB/bản ghi" | **SAI phương pháp.** Kích thước file BTree có overhead trang. Số thật: **10,8 KB/bản ghi** trên kho production |
| KHO 5: "`CTongDB::Close()` chỉ delete 3, `m_ZhaoMuTable` bị rò rỉ" | **SAI ngược.** `m_ZhaoMuTable` **CÓ** `delete` + gán NULL (`TONGDB.CPP:105-129`). Lỗi thật là 3 bảng kia delete mà **không** gán NULL |
| KHO 1: "`UnlockAllRole` chỉ đặt -1 ⇒ map chỉ tăng, không ai dọn" | **SAI một nửa.** Đường đăng xuất bình thường **CÓ** `erase` (`:720-724`, `:756-767`). Chỉ khi **rớt kết nối** mới để lại entry |
| KHO 6: `deposits.json` = `{"balances":…, "locked":…}` | **Thiếu chính xác.** File thật **chỉ có `"balances"`**. Công cụ nhập phải chịu được thiếu khóa |
| KHO 4: "toàn bộ SQL sống nằm trong `S3AccServer\S3PAccount.cpp`", "chỉ còn 3 bảng" | **THIẾU.** Còn `S3RELAYSERVER\S3PAccount.cpp` (dự án sống, đang chạy) dùng thêm **VIEW `View_AccountMoney`** (`:253`) và 5 câu SQL khác |
| KHO 8: "10 lượt save/giây là vòng lặp bệnh lý, bỏ qua khi tính tải" | **SAI bản chất.** Đó là **đường re-save hết hạn được code cố ý** (`KPlayer.cpp:53`, `:1093`) — chính là chế độ hỏng mà MySQL sẽ kích hoạt |
| Mọi khảo sát: "`m_csRoleLock` chống hai GameServer cùng ghi" | **KHÔNG ĐÚNG.** `ClientNode.cpp:715` gán vô điều kiện — không phải khóa loại trừ |
| KHO 8: "không có `__db.00N` ở bất cứ môi trường nào" | **Chỉ đúng cho `bin`.** `Sources\MultiServer\DBTranslate\database\` có đủ `__db.001..005` (12/08/2026) — công cụ đó **đã từng được chạy** |
| Số dòng lệch (không sai kết luận, nhưng sai khi dùng để sửa mã) | `IDBRoleServer.cpp`: chặn `dwDataLen` ở **:475** (không phải 472), `db_table->add` ở **:576** (không phải 571/575), chặn trùng tên bắt đầu **:549** (không phải 539), đếm nhân vật **:561-570**. `S3PDB_MySQL_Connection.cpp`: `mysql_real_connect` ở **:51** (không phải 52). `KPlayerDBFuns.cpp`: `memcpy` kinh mạch ở **:962** (không phải 961) |

---

*Hết báo cáo. Mọi số liệu đo được đều lấy từ việc đọc trực tiếp file trên đĩa và mã nguồn; không có con số nào là phỏng đoán. Không một file mã nguồn game nào bị sửa, tạo hay xóa trong quá trình lập báo cáo này.*
