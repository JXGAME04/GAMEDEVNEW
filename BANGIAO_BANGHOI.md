# BÀN GIAO — DỰNG LẠI 100% HỆ BANG HỘI JX2 SANG NGUỒN JX1

> Lập ngày **12/08/2026**. Mở phiên mới thì **đọc hết file này trước khi làm gì**.
> Toàn bộ số liệu dưới đây là **đo thật**, không suy đoán; chỗ nào chưa chắc đều ghi rõ.

---

## 1. MỤC TIÊU

Chủ game yêu cầu: **làm lại toàn bộ hệ bang hội giống hệt bản Linux, cả máy chủ lẫn client.**

## 2. ĐƯỜNG DẪN (không được nhầm)

| Vai trò | Đường dẫn |
|---|---|
| **Nguồn C++ cần sửa** | `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\SOURCESUPDATE_KINHMACH_ONLTEST0608` |
| **Server + client chạy thử** | `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin` (client ở `bin\client`, 8,9GB, 37 pak) |
| **Bản mẫu JX2 — máy chủ** | `D:\ServerLinux\server1` (+ `gateway`) |
| **Bản mẫu JX2 — client** | `D:\ServerLinux\Patch` (lớp cập nhật chồng, 124MB) |
| **Kho git đích** | `https://github.com/JXGAME04/GAMEDEVNEW.git` (bản sao rỗng ở `D:\GAMEDEVNEW`) |

---

## 3. TÌNH TRẠNG GIT — VIỆC ĐẦU TIÊN PHẢI LÀM XONG

### Đã xong
- `core.autocrlf` đã đặt **`false`** cho kho nguồn. **Bắt buộc giữ nguyên** — để `true` là đổi byte file GBK/TCVN, hỏng hết chữ Trung và chữ Việt trong mã.
- Đã thêm remote `gamedevnew` trỏ đúng kho đích.
- Đã cập nhật `.gitignore` (thêm `vcpkg_installed/`, `Template/`, `x64/`, `EngineServerDebug/`, `EngineServerRelease/`, `Lib/debug64/`, `Lib/release64/`, `Lib/x64/`, `*.vcxproj.user`).
- **357 file đã được `git add` sẵn**, không lẫn rác — kiểm rồi.

### Chưa xong
**Commit và push chưa chạy.** Nhánh `master` vẫn ở commit cũ `5d158ca`.

⚠️ **Kho đích dùng nhánh `main`, không phải `master`.** Phải đẩy chéo nhánh:

```bash
cd "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/SOURCESUPDATE_KINHMACH_ONLTEST0608"
git commit -m "Snapshot nguon JX1 VS2022 truoc khi dung lai he bang hoi JX2"
git push -u gamedevnew master:main
```

### Luật git của chủ game (bắt buộc tuân thủ)
- **Sửa file nào là commit + push ngay**, không để dồn.
- 🔴 **`git add` ĐÍCH DANH từng file. TUYỆT ĐỐI KHÔNG `git add -A`** — trong cây có `vcpkg_installed/` (cả mã nguồn OpenSSL) và các thư mục biên dịch, `-A` sẽ nuốt hàng trăm MB rác. Dùng `git add -u` khi muốn đóng gói các file **đã theo dõi** bị sửa.
- ⚠️ Trong phiên lập file này, **mọi lệnh git ghi đều bị bộ lọc quyền của Claude Code chặn**. Nếu phiên mới cũng bị chặn, phải nhờ chủ game dán khối `permissions.allow` (danh sách lệnh git an toàn) vào `C:\Users\nguye\.claude\settings.json` rồi khởi động lại.

---

## 4. KẾT QUẢ KHẢO SÁT — PHÍA JX2 (bản mẫu)

- `D:\ServerLinux` **là JX2 / Kiếm Thế**, KHÔNG phải JX1. Bằng chứng: đường dẫn build `code.kingsoft.com/jx2` trong binary.
- `server1/jx_linux_y` (8,9MB) là GameServer thật: ELF 32-bit i386, tĩnh, **đã xóa section header**, chỉ còn 311 dynsym libc.
- **Nhưng bảng đăng ký hàm Lua còn nguyên văn** trong `.rodata` → đã trích được **1.661 hàm engine kèm địa chỉ mã lệnh**. Engine script là **Lua 4.0**. Mã **không bị làm rối**, disasm ra GCC `-O2` sạch.
- Bộ script: **5.232 file `.lua`, 20,32 MB**.

### Hệ bang hội phía JX2
- **150 hàm** (`TONG_` 123 + `TONGM_` 27).
- ⚠️ **`LG_`/`LGM_` (28 hàm) KHÔNG phải liên minh bang hội** — là hệ nhóm/xếp hạng dùng chung; nơi dùng nhiều nhất là **vinh dự Tống Kim** (`LG_SONGJINHONOUR`, xem `script/battles/battlehead.lua:1077`). Đừng gộp vào bang hội.
- **Đã có bảng đặc tả chữ ký cho 107/150 hàm** (số tham số lấy từ mọi chỗ gọi thật trong 5.232 file Lua). 43 hàm còn lại **không script nào gọi** nên phải dịch ngược từng cái.
- 3 hàm có **tham số tùy chọn**, phải cài đúng kiểu đó: `TONG_GetMemberCount` (1–2), `TONG_GetPauseState` (1–2), `TONG_GetTaskValue` (2–3).
- 🔑 **44/178 hàm không script nào gọi**, gồm `TONG_ApplyUpgrade`, `TONG_ApplyInit`, `TONG_ApplyMaintain`, `TONG_ApplySetMoney` ⇒ **lên cấp bang và bảo trì hàng tuần do engine/relay tự chạy, sửa script không đổi được**.
- ❌ **Phép suy số tham số từ mã máy đã bị BÁC** — chỉ đúng 33,6% khi đối chiếu 107 hàm đã biết. Đừng dùng lại; phải dịch ngược thủ công.

---

## 5. KẾT QUẢ KHẢO SÁT — PHÍA JX1 (nguồn đích)

### Kiến trúc 3 tầng (giống JX2 về triết lý)
```
S3Client  --enumTONG_COMMAND_ID_*-->  GameServer
                                          |  SGDI_TONG_*
                                          v
                              CoreServer.dll (KPlayerTong) = KIỂM QUYỀN
                                          |  enumC2S_TONG_*
                                          v
                        S3Relay (CTongSet/CTongControl) = NGUỒN SỰ THẬT
                                          |
                                    g_cTongDB (Berkeley DB)
```
- **GameServer KHÔNG có bản sao `CTongSet`** — mỗi người chơi chỉ giữ ảnh chụp `KPlayer::m_cTong`. Mọi thao tác **ghi** phải gửi lên Relay.
- 🔑 **API cho Lua ghi dữ liệu bang = `g_NewProtocolProcess.PushMsgInTong()`** (`Sources/Core/Src/KNewProtocolProcess.cpp:133`).
- 🔴 **Lưu bằng Berkeley DB, KHÔNG phải MySQL** — 3 bảng `TongDB` / `TongMemberDB` / `TongMoneyDB` (`TONGDB.CPP:37,41,48`). Ghi tức thì, không timer.

### Độ chênh
- JX1 chỉ mở **11 hàm Lua** về bang (`GetTongName`, `GetTongLevel`, `GetTongExp`, `GetTongInfo`, `GetTongFlag`, `OpenTong`, `SetTongExp`, `SetTongLevel`, `SetThueTongOwnCity`, `SetViewTongOwnCity`, **`Msg2Tong`**) — JX2 mở 150.
- **`STONG_MEMBER` chỉ có 3 trường** (mã tên, tên, giới tính). Không cống hiến, không ngày gia nhập.
- Bang chỉ có **một túi tiền** `m_dwMoney`; JX2 có 7 loại quỹ.
- **KHÔNG có hệ quyền linh hoạt** — quyền gắn cứng theo chức vụ bằng ~11 câu `if (m_nFigure != MASTER && ...)` trong `KPlayerTong.cpp` (dòng 987, 1081, 1148, 1205, 1710, 1959, 1982, 2005, 2028…). JX2 có **30 quyền** (xem `帮会权限分配页面.ini`) ⇒ phải dựng mới hoàn toàn rồi thay cả 11 điểm kiểm tra.
- **KHÔNG có chức năng giải tán bang** — `CTongDB::DelTong` (`TONGDB.CPP:139`) là code chết, không ai gọi.
- ❌ **Cả 11 khái niệm JX2 đều KHÔNG tồn tại**: quỹ kiến thiết, quỹ duy trì, mục tiêu tuần, cống hiến thành viên, công phường, tuyệt kỹ bang, lãnh địa riêng, thông báo bang, ngày chiếm thành, trạng thái tạm dừng, điểm nhiệm vụ bang.

⇒ **Kết luận: đây là xây mới ~90%**, chỉ tái dùng khung kết nối GS↔Relay, chỗ chứa `CTongSet`/`CTongControl`, và cơ chế lưu Berkeley DB.

### Điểm chèn hàm Lua mới — chỉ 2 chỗ, cùng 1 file
`Sources/Core/Src/ScriptFuns.cpp`:
1. **Thân hàm** chèn tại **dòng 6922** (đã nằm trong `#ifdef _SERVER`, mở :1990 đóng :8449).
2. **Đăng ký** chèn giữa **dòng 12137–12138**, trong bảng `GameScriptFuns[]` (khai báo :11691).
Không có hằng đếm nào phải sửa — `g_GetGameScriptFunNum()` dùng `sizeof`. Bảng được nạp tại `KSortScript.cpp:92`.

### Thêm 1 trường mới vào bang = 9 file / 20 điểm sửa
Hai chỗ **bắt buộc**, thiếu là mất dữ liệu:
- `S3Relay/TONGDB.CPP:115` (ghi xuống DB)
- `S3Relay/TONGDB.CPP:186` (đọc lại từ DB)

Bảng phân phối lệnh (**KHÔNG nằm ở `TongServer.cpp`**):
- Client→GS: `KSOServer.cpp:4028`
- Relay→GS: `KSOServer.cpp:1327`
- GS→Relay (phía Relay nhận): `TongConnect.cpp:241`
- GS→CoreServer: `CoreServerShell.cpp` (33 case `SGDI_TONG_*`)

⚠️ Mảng `g_nTongPSSize[]` ở `TongConnect.cpp:13-41` phải thêm **đúng thứ tự enum** — lệch một chỗ là **mọi gói sau đó bị từ chối im lặng**.

---

## 6. 🔴 NĂM BẪY CÓ SẴN — PHẢI VÁ TRƯỚC KHI GHÉP

1. **`CTongSet::InitFromDB` TRÀN HEAP khi server có hơn 16 bang.** `KTongSet.cpp:481` reset mảng về 16 ô rồi `:496-504` ghi `m_pcTong[i]` với `i < nGetNum` mà không kiểm trần. Server thật chắc chắn >16 bang ⇒ **đây là lỗi đang sống**.
2. **`TTongStruct` ghi/đọc NGUYÊN KHỐI vào Berkeley DB** (`TONGDB.CPP:134` dùng `sizeof`, `:312` `memcpy`), và `SearchTong:174` **không kiểm cỡ bản ghi** trước khi ép kiểu ⇒ thêm trường là dữ liệu bang cũ ra rác hoặc sập. **Phải thêm trường ở CUỐI struct** + xóa 3 thư mục DB để tạo lại, hoặc viết công cụ chuyển đổi.
3. **Relay chạy ĐƠN LUỒNG** (`_WORKMODE_SINGLETHREAD`); các `DUMMY_AUTOLOCK*` là **khóa giả** (`Global.h:25-29` định nghĩa thành 0). **Thêm luồng nào chạm `g_cTongSet` là hỏng ngay.**
4. **15 lệnh `Sleep(5)`** trong các vòng gửi tin tới từng thành viên (`KTongControl.cpp`) ⇒ bang 200 người, một thao tác đổi cấp bang **chặn cả Relay ~1 giây**, kéo chat và bạn bè của cả cụm đứng theo.
5. **`Headers/KTongProtocol.h` dùng chung GS + Relay** ⇒ phải build và **triển khai đồng thời**, lệch một bên là mọi lệnh bang bị hiểu sai.

Ngoài ra: `Sources/MultiServer/GameServer2010/` là **project mồi nhử** — bản sống là `Sources/MultiServer/GameServer/`.

---

## 7. PHÍA CLIENT — TIN TỐT

- Giao diện JX là **dữ liệu** (`.ini` bố cục + `.spr` ảnh + `.lua` logic), **không nằm cứng trong `game_y.exe`** ⇒ chép lại được.
- Có sẵn **17 cửa sổ bang hội** dạng `.ini` ở `D:\ServerLinux\Patch\ui\ui3_1024` (và bản `ui3_800`), tổng 34 file. Tên file mã GBK, giải ra: cửa sổ chính bang hội · trang thông tin cơ bản (17KB) · trang chức năng (27KB) · **trang phân bổ quyền hạn (11KB)** · hệ cải tạo (21KB) · trang công phường · đơn tạo bang · trang chiêu mộ · bổ nhiệm chức vụ · quản lý thành thị…
- ✅ **Đã tra: 64/67 = 95,5% ảnh giao diện bang hội CÓ SẴN trong pak client** ở `bin\client\data`. Chỉ **thiếu 3 file**, vẽ lại là xong:
  - `\Spr\Ui3\帮会界面\帮会信息页\帮会信息-帮会帮助.spr`
  - `\Spr\Ui3\帮会界面\帮会信息页\帮会信息-招人按钮.spr`
  - `\spr\Ui3\帮会界面\发放界面标题_银两.spr`
- Client JX1 hiện chỉ có **5 cửa sổ bang** (`UiTongManager`, `UiTongCreateSheet`, `UiTongAssignBox0`, `UiTongChangeTitle`, `UiTongGetString`) khớp 5 lớp C++ trong `Sources/S3Client/Ui/UiCase/`.

### 🔑 Cách đọc pak (đã kiểm chứng 76,8%)
Định dạng là **XPack**, KHÔNG phải `KPakData` cũ:
- Header **32 byte**: `{'PACK'(4), uCount, uIndexTableOffset, uDataOffset, uCrc32, cReserved[12]}`
- Mỗi mục chỉ mục **16 byte**: `{uId, uOffset, lSize, lCompressSizeFlag}` — **không phải 12 byte**.
- Chỉ mục **sắp xếp tăng dần theo `uId`** (engine dùng tìm kiếm nhị phân) → dùng điều này để tự kiểm tra đã đọc đúng chưa.
- Băm tên = **`KPakList::FileNameToId`** (`Sources/Engine/Src/KPakList.cpp`), **KHÔNG phải `g_FileName2Id`**. `KPakList::FindElemFile` đặt `szPackName[0]='\\'` rồi mới ghi đường dẫn vào sau ⇒ **GIỮ LẠI dấu `\` đầu khi băm**. Bỏ dấu đó là trượt 100%.
- Công thức: hạ chữ thường (byte 0x41–0x5A), rồi `id = (id + (++index)*c) % 0x8000000b * 0xffffffef`, kết `^ 0x12345678`. **`c` là `char` CÓ DẤU** (byte ≥0x80 thành số âm) — dùng không dấu chỉ trúng 54,1% thay vì 76,8%.

---

## 8. THỨ TỰ THI CÔNG ĐÃ CHỐT

1. **Vá 5 bẫy ở mục 6** (nhất là tràn heap >16 bang — lỗi đang sống).
2. **Mở rộng cấu trúc dữ liệu + Berkeley DB** (thêm trường ở **CUỐI** `TTongStruct`; nhớ cả 2 chỗ `TONGDB.CPP:115` và `:186`).
3. **Giao thức đồng bộ** (thêm enum + struct + `g_nTongPSSize[]` đúng thứ tự + case ở 4 bảng phân phối).
4. **150 hàm Lua** (chèn `ScriptFuns.cpp:6922` + đăng ký `:12137`; ghi qua `PushMsgInTong`).
5. **17 cửa sổ client** (chép `.ini` từ `Patch`, trích `.spr` từ pak, viết lớp `UiCase` tương ứng).

---

## 9. ⚠️ BẪY MÃ HÓA — ĐỌC TRƯỚC KHI SỬA FILE

Nguồn là **ANSI/GBK trộn TCVN3 (tiếng Việt)**. Công cụ `Edit`/`Write` thông thường **làm hỏng byte**.
- Khi sửa file `.cpp`/`.h` trong cây nguồn: **dùng skill `swordonline-dev`** (nó có đường ghi an toàn giữ nguyên byte).
- Khi đọc bằng python: `open(p,'rb').read().decode('gbk', errors='replace')`.
- Chữ Trung hiện thành ký tự lạ trong terminal **không có nghĩa là file hỏng**.

---

## 10. VIỆC CÒN DỞ

- ⏳ **Workflow phân tích 12 tầng phía JX2 chưa trả kết quả** (chạy suốt phiên 12/08 không xong). Nội dung: mô hình dữ liệu, vòng đời, chức vụ/quyền hạn, cấp bang & quỹ, cống hiến, nhiệm vụ & công phường, tuyệt kỹ, lãnh địa, chiến tranh bang/công thành, liên minh, lưu trữ & đồng bộ, tích hợp 97 file. **Phiên mới nên chạy lại** nếu cần ngữ nghĩa chi tiết từng hàm.
- ⏳ Chưa dịch ngược chữ ký của **43 hàm** không có chỗ gọi.
- ⏳ Chưa commit/push (xem mục 3).

## 11. CÔNG CỤ KÈM THEO

Nằm cùng thư mục này (`D:\GAMEDEVNEW\tools\`):

| File | Dùng để |
|---|---|
| `engine_api_full.txt` / `.json` | 1.661 hàm engine JX2 kèm địa chỉ mã lệnh |
| `jxdis.py` | Dịch ngược 1 hàm theo tên: `python jxdis.py TONG_GetMoney 60` |
| `sigspec_tong.md` | Đặc tả chữ ký 150 hàm bang hội |
| `xref_tong.md` | Hàm nào được gọi ở đâu, bao nhiêu lần |
| `missing_funcs.txt` | 819 hàm script cần mà JX1 chưa có |
| `pak_lookup3.py` | Đọc chỉ mục pak + tra file theo đường dẫn |
| `anh_banghoi_thieu.txt` | 3 ảnh giao diện bang hội còn thiếu |
