# BÀN GIAO — XẾP HẠNG THẾ GIỚI: bảng bên phải trống (07/09/2026, 18:0x)

> Chủ báo: *"tìm bản xếp hạng thế giới fix lỗi hiển thị không hiện danh sách xếp hạng"* — kèm ảnh
> chụp: cột trái đủ mục (Thập Đại Cao Thủ, Thập Đại Phú Hào, Hoang Dã Cao Thủ, … Phú Hào),
> **khung bên phải chỉ có hoa văn nền**.

Điều tra bằng 56 agent soi 6 hướng song song + phản biện đối kháng 2 góc nhìn/phát hiện
(24 phát hiện, 11 sống sót). Mọi khẳng định chịu lực bên dưới **tôi đã tự kiểm lại**, gồm cả
đo trên nhị phân đang chạy.

---

## 1. Trạng thái — CHỜ SWAP, **ba** tệp máy chủ

| Tệp | md5 | Cấu hình build |
|---|---|---|
| `bin\multiserver\Goddess.exe.moi` | `2ffeb6e4` | `Debug\|Win32` |
| `bin\server\GameServer.exe.moi` | `d07555ad` | `Release\|x64` |
| `bin\server\CoreServer.dll.moi` | `7361e2dd` | `Server Release\|x64` |

Bản đang chạy: `Goddess.exe` `73f10c62` (01/09 22:03) · `GameServer.exe` `9c49a16f` ·
`CoreServer.dll` `6693429f`. Khe `.moi` lúc đặt **trống cả hai thư mục**, không đè của ai.
Nguồn: `origin/main` `02eb802d` + commit `c95ac794`.

Client **không** cần build lại. Đã sửa sẵn `bin\client\Ui\IndexAndRankMapping.ini` (mục 5).

### Cách đổi

1. **Tắt cả cụm máy chủ**, gồm **cả cửa sổ Goddess** (đây là điểm khác mọi lần trước).
2. `bin\server\ChayGameServer.bat` — bat tự đổi `CoreServer.dll` và `GameServer.exe`.
3. **Goddess.exe phải đổi tay** (bat không quản thư mục `multiserver`):
   ```
   cd /d E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\multiserver
   ren Goddess.exe Goddess.exe.truoc_xephang
   ren Goddess.exe.moi Goddess.exe
   ```
4. Mở lại cụm như thường lệ.

> Tôi có định thêm bước đổi Goddess vào `ChayGameServer.bat` nhưng bộ lọc quyền chặn việc sửa
> tệp bat khởi động — và ngẫm lại thì chặn đúng: sửa bat gác cổng khởi động là rủi ro thừa khi
> chủ đằng nào cũng restart cả cụm. Nên để đổi tay.

**Đổi thiếu thì sao?** Không sập. Nếu chỉ đổi `bin\server` mà quên Goddess, bảng vẫn rỗng —
nhưng từ đợt này máy chủ **in rõ lý do** ra console/`GameServer.log`:
`--KLadder::Init: LECH CO, nhan 6382 byte, doi 7918 byte--`.

---

## 2. Gốc — hồi quy từ đợt Hoa Sơn 01/09, ba bản sao của cùng một struct

Đợt `fa4235ef` [HOASON 01/09] nới `TGAME_STAT_DATA` từ `[11]` lên `[MAX_FACTION_NUM+1]` = `[14]`
(13 phái + ô chưa nhập môn). Chú thích trong mã còn ghi *"Goddess DBBackup.h TStatData PHẢI cùng
cỡ (sizeof kiểm ở KLadder::Init)"* — sửa đúng 2 chỗ, **sót chỗ thứ ba**:

| Tệp | Trước đợt này | Ai biên dịch |
|---|---|---|
| `Sources/Core/Src/KProtocol.h:2308` | `[MAX_FACTION_NUM+1]` → **7918** byte | CoreServer.dll |
| `Sources/MultiServer/Goddess/DBBackup.h:39` | `[14]` | Goddess (bảng thu thập nội bộ) |
| **`Headers/KProtocol.h:1483`** | **`[11]` → 6382 byte** | **Goddess (gói gửi đi)** |

Goddess biên dịch bản `Headers\` vì `Goddess.vcxproj:75` chỉ có include
`..\..\..\lib;..\..\..\Headers` — **không có `Core\Src`**. Hai tệp dùng **chung** include guard
`KPROTOCOL_H` nên trong một đơn vị dịch chỉ một bản thắng — đó chính là cái bẫy.

### Đo trên nhị phân đang chạy (đếm hằng số 4 byte little-endian)

| Tệp | chứa 6382 | chứa 7918 |
|---|---|---|
| `Goddess.exe` (01/09 22:03) | **6** | 0 |
| `CoreServer.dll` | 1 | **5** |

Dữ liệu gốc **vẫn tốt**: `bin\multiserver\StatData.dat` = 231.048 byte = đúng cỡ bản `[14]`,
cập nhật 07/09 03:24. Không phải thiếu dữ liệu.

---

## 3. Chuỗi triệu chứng → nguyên nhân

1. GameServer định kỳ xin thống kê (`KSOServer.cpp:3265`, lần đầu ngay lúc boot).
2. Goddess trả gói `nDataLen = 6382` (`ClientNode.cpp:672`).
3. `KLadder::Init` ([KLadder.cpp:20](Sources/Core/Src/KLadder.cpp:20)) đòi 7918 → `return FALSE`
   **im lặng, không log**. `GameStatData` giữ nguyên `ZeroMemory` từ hàm dựng.
   `_ASSERT` ở `KSOServer.cpp:2267` vô hiệu vì bản `NDEBUG`.
4. Người chơi bấm một mục → máy chủ **vẫn trả đủ 10 bản ghi**, nhưng tên rỗng
   ([KProtocolProcess.cpp:5885](Sources/Core/Src/KProtocolProcess.cpp:5885) `strlen("")` = 0).
5. Client vứt sạch bản ghi tên rỗng
   ([UiStrengthRank.cpp:672](Sources/S3Client/Ui/UiCase/UiStrengthRank.cpp:672)
   `if(pIncome->szMsg[0])`).
6. `ConstructRankView` → `nCount = 0` → vòng vẽ không chạy lần nào → **khung phải trống, không sập**.
7. Cột trái vẫn đầy vì đọc **hoàn toàn cục bộ** từ `Ui\IndexAndRankMapping.ini`
   (`UiStrengthRank.cpp:113-128`), không dính máy chủ.

---

## 4. Đã sửa (commit `c95ac794`, bộ vá `ReverseTools\goi_va_xephang_0709.py`)

| Tệp | Nội dung |
|---|---|
| `Headers/KProtocol.h` | 5 mảng `[11]` → `[MAX_FACTION_NUM + 1]` — **hết lệch cỡ** |
| `Goddess/DBBackup.h` | 5 mảng `[14]` → `[MAX_FACTION_NUM + 1]` (bỏ hằng số rời, hết nguy cơ trôi lần sau) |
| `Goddess/DBBackup.cpp` | **9 vòng** duyệt chiều phái `<11` → `< MAX_FACTION_NUM + 1`: sắp xếp (735/739), tính mũi tên Sort (809), **chép sang `SendStatData`** (934), ghi `nSectStat` ngược (1006), 4 vòng ghi log (878/896/901/906) |
| `GameServer/KSOServer.cpp` | **Chốt kiểm độ dài** cho `s2c_gamestatistic` + log lệch cỡ |
| `Core/KLadder.cpp` | Log khi từ chối vì lệch cỡ |

**Thứ tự bắt buộc** (bộ vá tự làm đúng): nới **mảng trước**, nới **vòng lặp sau**. Nới vòng
trước là ghi tràn `SendStatData`.

**Vì sao phải thêm chốt độ dài:** gói phồng 6382 → 7918 byte = 13 → 16 khung 512
(`CPackager` mặc định `packLength = 512`). Mất một khung giữa đường (đúng lớp sự cố Rainbow
04/09) thì `KLadder::Init` vẫn `memcpy` theo `nDataLen` **khai báo** → **đọc tràn**. Trước đây
đường này không kiểm gì; chốt chép đúng mẫu đã có ở `s2c_roleserver_getroleinfo_result`.

### Cố ý KHÔNG đụng

`DBBackup.cpp:709` và `:721` — `if (…Sect <= 10 && …Sect >= 1) ++SectMoneyMost[Sect + 1]`.
Quy ước chỉ số ở đây **khác hẳn** mọi chỗ còn lại (chỗ khác dùng `nSect + 1` với `nSect` từ −1;
ở đây lại chặn `>= 1` nên bỏ qua cả phái 0). Đây là thống kê **phần trăm**, không phải bảng xếp
hạng, và tôi chưa hiểu rõ quy ước → không đoán. **Ghi nhận để đợt sau xử lý.**

---

## 5. Bảng ánh xạ `.ini` (client, không cần build lại)

`MAX_FAC` 11 → 14 làm `KLadder.h` đổi `enumFacTopTenRicher` 15 → 18 và `enumLadderEnd` 26 → 32,
trong khi `Ui\IndexAndRankMapping.ini` vẫn là sơ đồ cũ 25 mục. Client ép mã bảng = số thứ tự
dòng + 1 (`UiStrengthRank.cpp:128`), nên **11 mục "Phú Hào" trỏ nhầm bảng**:

| ID | Nhãn .ini cũ | Máy chủ thực sự trả |
|---|---|---|
| 15,16,17 | Hoang Dã / Thiếu Lâm / Thiên Vương **Phú Hào** | **CẤP** của Hoa Sơn / Vũ Hồn / Tiêu Dao |
| 18..25 | Đường Môn..Côn Lôn **Phú Hào** | **TIỀN** phái −1..6 — lệch 3 bậc |
| 26..31 | (không có dòng) | 6 bảng Phú Hào không bao giờ hỏi được |

Đã sinh lại bằng `ReverseTools\gen_indexrank_ini_0709.py`: **`ListCount` 25 → 31**, 4..17 =
14 mục Cao Thủ (Hoang Dã + 13 phái), 18..31 = 14 mục Phú Hào. Bản cũ cất ở
`IndexAndRankMapping.ini.truoc_xephang_0709`. **Tệp này không nằm trong git** — chỉ sống trong
cây chạy thật; bộ sinh thì có trong git.

---

## 6. 🔴 SAU KHI SỬA VẪN CÒN DÒNG TRỐNG — VÀ ĐÓ LÀ ĐÚNG

Giải mã `StatData.dat` (đếm ô có tên):

```
KillerStat  : 0/10      <- RỖNG HOÀN TOÀN
LevelStat   : 1000/1000 ; MoneyStat : 1000/1000
phái -1 (chưa nhập môn) : 0/100   <- RỖNG
phái 0..9               : 84..100 <- ĐẦY ĐỦ
Hoa Sơn 1 ; Vũ Hồn 1 ; Tiêu Dao 0
```

Nên **kỳ vọng đúng** sau khi swap:

| Mục | Kỳ vọng |
|---|---|
| Thập Đại Cao Thủ / Phú Hào | **có dữ liệu** ← đây là phép thử chính |
| 10 phái gốc, Cao Thủ và Phú Hào | **có dữ liệu** |
| Hoang Dã (chưa nhập môn) | trống — thiếu dữ liệu, **không phải lỗi** |
| Hoa Sơn / Vũ Hồn | 1 dòng mỗi phái |
| Tiêu Dao | trống — chưa có ai |
| Dòng kẻ (ID 3 = Sát Thủ) | trống — `KillerStat` rỗng |

---

## 7. Mắt xích duy nhất chưa đo được

Chưa ai chứng minh gói `s2c_gamestatistic` **thật sự tới** GameServer rồi mới bị từ chối, thay vì
không bao giờ tới. Toàn bộ chẩn đoán suy từ mã + nhị phân. Bản Goddess Debug này không biên dịch
`CONSOLE_DEBUG` (quét: 0 lần) nên không có đường đo miễn phí.

Vì đằng nào cũng phải build lại, tôi đã nhét sẵn log. **Ngay lần chạy đầu sau swap** sẽ biết chắc:

```
findstr /C:"XepHang" /C:"KLadder::Init" GameServer.log
```

- Không có dòng nào **và** bảng có dữ liệu → xong, gói vẫn tới như thường.
- `--XepHang: LECH CO! Goddess gui … byte, GameServer doi …--` → đổi thiếu Goddess.exe.
- `--XepHang: goi thong ke ghep duoc … nhung khai bao …--` → mất khung, chốt mới đã cứu (trước
  đây chỗ này sẽ đọc tràn).

---

## 8. Bẫy gặp trong đợt này

1. **Build tăng dần để lại obj cũ → exe trộn hai cỡ struct.** Lần build đầu còn 4 obj từ
   **21/08** (`DBDumpLoad`, `FilterTextLib`, `RoleNameFilter`, `StdAfx`) không được biên dịch lại
   → `Goddess.exe` ra **6382 ×3 + 7918 ×3**, tức chính cái lỗi đang sửa. Phải `-t:Rebuild` sau khi
   xoá obj. Đúng bẫy `jx1-build-song-song-obj-lech`. **Luôn kiểm nhị phân bằng cách đếm hằng số
   `sizeof` trước khi đặt `.moi`.**
2. **Hai tệp cùng include guard `KPROTOCOL_H`.** Không có cảnh báo nào từ trình biên dịch; mỗi
   project lấy một bản tuỳ include path. Đây là lớp lỗi sẽ tái diễn — chỗ nào sửa `TGAME_STAT_DATA`
   hoặc `MAX_FACTION_NUM` phải sửa **cả ba** bản.
3. `MSBuild -p:SolutionDir="D:\GAMEDEVNEW\"` làm Goddess ghi thêm một bản ra `D:\bin\multiserver\`.
   Vô hại, nhưng **đừng lấy nhầm** bản đó — bản thật ở `Sources\MultiServer\Goddess\Debug\`.
4. `git pull --rebase` bị chặn vì một tệp untracked của phiên khác trùng tên với tệp vừa được
   commit lên. Đã so md5 (giống hệt) rồi mới dời — **không xoá thứ chưa nhìn**.
