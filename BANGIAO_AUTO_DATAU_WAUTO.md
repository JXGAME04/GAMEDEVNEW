# BÀN GIAO: AUTO DÃ TẨU trong WAuto (18-19/08/2026)

> **ĐỌC TỆP NÀY TRƯỚC KHI GÕ bất cứ dòng nào liên quan Dã Tẩu.**
>
> | Tệp | Dùng khi nào |
> |---|---|
> | **`BANGIAO_AUTO_DATAU_WAUTO.md`** (tệp này) | đọc đầu tiên — tổng quan, test, cạm bẫy, việc còn nợ |
> | **`THAMCHIEU_AUTO_DATAU.md`** | tra cứu chi tiết: bảng 14 pha, watchdog, 19 điều khiển UI, 6 hook — **kèm `file:line`** |
> | `KEHOACH_AUTO_DATAU_WAUTO.md` | thiết kế gốc + kết quả phản biện vòng 1 |
> | `AUTO_DATAU_SPEC.md` | đặc tả nghiệp vụ Dã Tẩu (**có 1 điểm SAI — xem mục 3.1**) |
> | `BANGIAO_DATAU_TASKLINK.md` | tài liệu port hệ nhiệm vụ sang JX1 |

---

## 1 · Tóm tắt một phút

Người chơi bật **tab 9 "Dã Tẩu"** trong WAuto.exe → nhân vật tự đi gặp NPC Dã Tẩu, nhận
nhiệm vụ, làm đủ **6 loại**, trả nhiệm vụ, chọn thưởng, rồi lặp lại đến khi đủ 40 lần/ngày.

| | |
|---|---|
| **Trạng thái** | test lần 1 đứng yên → fix `DT_WalkTo` (8.6) → **test lần 2 ĐÃ CHẠY LÀM DÃ TẨU** ✅ → đợt nâng cấp 19/08 trưa: đóng khung thoại sau nhận nhiệm vụ · T4 tự chạy tìm quái (roam 8 hướng + quét NpcSet) · túi đầy tự thả máy về bán rác Hậu cần (`DTP_SELLJUNK`) · **UI WAuto thiết kế lại** (khung nhóm/kẻ mục/~190 tooltip) — chờ test đợt 2 |
| **Commit** (`D:\GAMEDEVNEW`, nhánh main) | `f606e540` → `1d24b9fc` → `8a9ae8f5` → `32e60788` (fix đứng yên) → **`0c97e240` (3 nâng cấp + UI, kèm backup UI trong `WAutoUI/`)** |
| **Nơi bật** | WAuto.exe → tab **"Dã Tẩu"** → tick "Bật auto Dã Tẩu" |
| **Mặc định** | **TẮT** (`bDaTau=0`) — không tick thì hành vi auto cũ nguyên vẹn |
| **Việc kế tiếp** | Hỏi người dùng kết quả test + dòng `[DaTau]` cuối trong khung chat |

---

## 2 · Trạng thái binary đã triển khai (19/08)

| Tệp | Đường dẫn | Dấu thời gian | Ghi chú |
|---|---|---|---|
| `CoreClient.dll` | `E:\SourceTuanLe\...\TESTLOFFF_ONLINE\bin\client\` | **19/08 17:50** | engine Dã Tẩu, toàn bộ fix tới `ae1129f8`; bản trước = `CoreClient_cu_1908i.dll` |
| `settings\datau_toado.txt` | `...\bin\client\settings\` | **19/08 17:10** (80 KB) | **MỚI** — bảng tọa độ cụm quái 204 map, engine nạp lúc chạy; sửa tay được, không cần dựng lại DLL |
| `Game.exe` | như trên | 19/08 06:49 | chứa cổng điều phối |
| `WAuto.exe` | **`E:\Src_Auto_Ngoai\`** (gốc, KHÔNG phải `Release\`) | **19/08 11:39** (360.448 B) | UI mới: khung nhóm + kẻ mục + ~190 tooltip; ID dời (GRP 412-420, SEP 421-434, INDEX_END=436, popup/TABBTN_9→440-449). ⚠️ post-build gọi `pwsh.exe` không có trên máy ⇒ **luôn phải chép tay ra gốc**. ⚠️ WAuto tự thoát ngay nếu không có Game.exe đang chạy (hành vi vốn có, đừng tưởng exe hỏng) |

- `re_pe_crt.py` **PASS**: CoreClient=CRT-tĩnh · Game.exe=UCRT-RELEASE · engine/Represent2=UCRT-DEBUG.
- `bin\client\debug\` chỉ có tệp tháng 4 ⇒ **không** dựng nhầm Debug.
- Bản lưu: `CoreClient_cu2.dll`, `Represent2_cu.dll`, `WAuto_cu_2204.exe`.
- **Phải khởi động lại game** (tiến trình đang chạy vẫn giữ DLL cũ trong bộ nhớ).

---

## 3 · Vì sao thiết kế như vậy

### 3.1 🔴 Client KHÔNG đọc được task value ≥ 256 — điểm SAI của đặc tả

`TASK_VALUE_SYNC.nTaskId` là **BYTE** (`Sources/Core/Src/KProtocol.h:1868`), và
`KPlayerTask::SetSaveVal` gán thẳng `sValue.nTaskId = nNo` (`KPlayerTask.cpp:85`) ⇒ mọi id
≥256 bị **cắt mod 256**. Toàn bộ id Dã Tẩu (1020-1046, 2419, 2420, 2797) **không bao giờ**
tới client đúng; tệ hơn, ghi id 1028 sẽ rơi vào ô 4 của client.

`AUTO_DATAU_SPEC.md` §3 nói "client giữ bản sao task" — **đúng với id <256, SAI với Dã Tẩu**.

**Hệ quả thiết kế:** không đọc bộ nhớ task được, và **cấm sửa protocol** (server chạy ở máy
khác `206.82.7.181`, client cũ của người chơi sẽ lệch) ⇒ máy trạng thái phải đọc
**NỘI DUNG HỘI THOẠI** mà server gửi xuống. Toàn bộ tính năng **thuần client**.

### 3.2 Engine auto ngoại nằm ở đâu

Chuỗi thật (đã truy bằng mã, không đoán):

```
WAuto.exe ──shared memory (autoData, pack(1), 54ms/lần)──► Game.exe
   └─ ProcIpcCommand → PRT_GAMELOOP → KMyApp::ExtAutoLoop        (S3Client.cpp)
        └─ OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_DATAU, …) (vtable → CoreClient.dll)
             └─ DT_Process(...)                                   (CoreShell.cpp)
                  trạng thái: KPlayer::m_sExtAuto (struct ExtAuto, KPlayer.h)
```

⚠️ **`KPlayerAuto.cpp` KHÔNG phải engine này** — đó là "fkauto" (auto trong game), tick của
nó đã bị chú thích chết tại `KPlayer.cpp:432`. Đừng sửa nhầm nơi.

### 3.3 Ba phát hiện giúp rút gọn rất nhiều việc

1. **Rương thao tác được TỪ XA** — server chỉ kiểm cờ `m_CUnlocked`, không kiểm khoảng cách
   (`KItemList.cpp:1998` rút tiền · `KPlayer.cpp:4914` chuyển đồ). Client lại **luôn biết**
   nội dung rương (đồng bộ từ lúc đăng nhập). ⇒ "kiểm rương 1/2/3" = quét bộ nhớ, **không
   cần chạy tới rương**.
2. **Mua đồ ở thành nào cũng được** — server so khớp 5 trường tuyệt đối khi trả, không kiểm
   nguồn gốc (`tasklink_head.lua:285-292`). Tên tiệm trong lời thoại chỉ là lời văn.
3. **Xa Phu "godatau"** = mục **5** trong menu 9 lựa chọn (`xaphu.lua:19`), **miễn phí**,
   server tự chở đúng map nhiệm vụ.

---

## 4 · Bản đồ tệp — sửa gì, ở đâu, encoding nào

### 4.1 Phía game (có trong git `D:\GAMEDEVNEW`)

| Tệp | Vai trò | Encoding |
|---|---|---|
| `Sources/Core/Src/KDaTauCap.h` | **MỚI** — bộ đệm chụp gói tin `g_sDTCap` | ASCII |
| `Sources/Core/Src/KDaTauTables.h` | **MỚI, SINH TỰ ĐỘNG** — bảng dữ liệu + 36 marker TCVN3 | ASCII (octal) |
| `Sources/Core/Src/CoreShell.cpp` | engine `DT_Process` + `case ATYPE_DATAU` | **TCVN3** |
| `Sources/Core/Src/CoreShell.h` | thêm `ATYPE_DATAU` cuối enum | TCVN3 |
| `Sources/Core/Src/KPlayer.h` | thêm ~35 trường Dã Tẩu vào `struct ExtAuto` | TCVN3 |
| `Sources/Core/Src/KPlayer.cpp` | 2 hook chụp hội thoại + định nghĩa `g_sDTCap` | **TCVN3** |
| `Sources/Core/Src/KProtocolProcess.cpp` | 4 hook (thưởng / hộp nộp / chat hệ thống) | **TCVN3** |
| `Sources/Core/Src/ipc_shared.h` | +14 trường **CUỐI** `autoData` (sizeof 6820→**6876**) | ASCII |
| `Sources/S3Client/S3Client.cpp` | gọi `ATYPE_DATAU` + cổng chặn MOVE/FIGHT/RETURN | TCVN3 |
| `ReverseTools/gen_datau_tables.py` | **MỚI** — bộ sinh `KDaTauTables.h` | ASCII |

### 4.2 Phía UI (KHÔNG có trong git — `E:\Src_Auto_Ngoai\WAuto\WAuto\`)

| Tệp | Sửa gì | Encoding |
|---|---|---|
| `Resource.h` | 19 ID tab-8 (393-411), `IDC_INDEX_END`→412, popup dời +19, `IDC_TABBTN_9`=422 | ASCII |
| `WAuto.rc` | nút tab "Dã Tẩu" + 19 điều khiển | **UTF-16 LE** |
| `WAuto.cpp` | ShowTab/handler/Save/Update/Load/InitDialog + vá `.dat` cũ | **UTF-16 LE** |
| `ipc_shared.h` | bản sao — **phải khớp byte** với 2 bản kia | ASCII |

> ⚠️ **Mã nguồn UI không được quản lý phiên bản.** Nên sao lưu vào git (đề xuất, chưa làm).

### 4.3 Ba bản `ipc_shared.h` phải giống hệt nhau

`D:\GAMEDEVNEW\Sources\Core\Src\` · `E:\SourceTuanLe\...\SOURCESUPDATE_...\Sources\Core\Src\` ·
`E:\Src_Auto_Ngoai\WAuto\WAuto\`. Đã kiểm: **md5 giống nhau (`73f453c6…`)**. Lệch một byte là
Core đọc sai offset (Core ép kiểu thẳng `(autoData*)nParam`) — hỏng âm thầm, không có version handshake.

---

## 5 · Hướng dẫn TEST

### 5.1 Chuẩn bị
1. **Đóng game và đóng WAuto** đang mở (bản cũ nằm trong bộ nhớ).
2. Mở lại `bin\client\Game.exe`, rồi mở **`E:\Src_Auto_Ngoai\WAuto.exe`**.
3. Kiểm nhanh: WAuto phải có **nút tab "Dã Tẩu"** nằm dưới hàng nút PK.

### 5.2 Cấu hình
| Ô | Khuyến nghị lần test đầu |
|---|---|
| Bật auto Dã Tẩu | ✔ |
| 6 loại nhiệm vụ | bật hết (mặc định) |
| Khi bỏ qua | **Treo (tạm dừng)** — an toàn, không mất mốc 40 |
| Cách hủy | Chỉ dùng lượt hủy |
| Thưởng | Exp / Vật phẩm |
| Lấy đồ/tiền từ rương | ✔ + nhập **mật khẩu rương ở tab Hậu cần** |
| Xong lên map luyện công | tuỳ ý (tắt = đứng yên khi xong) |
| Tab Chiến đấu | **bật "Đánh quái"** — loại 4 bắt buộc cần |

### 5.3 Chạy
- Cho nhân vật đứng ở **1 trong 10 thành có NPC Dã Tẩu** (Phượng Tường, Thành Đô, Giang Tân
  Thôn, Biện Kinh, Ba Lăng, Tương Dương, Dương Châu, Long Môn Trấn, Đại Lý, Lâm An).
  Đứng nơi khác thì auto tự dùng Thổ Địa Phù về thành.
- Tick ô auto ở dòng nhân vật như bình thường.
- **Theo dõi khung chat**: mọi bước đều báo `[DaTau] ...` (bảng tra ở mục 6).

### 5.4 Nếu không thấy nhúc nhích
Chụp lại **dòng `[DaTau]` cuối cùng** — đó chính là chỗ máy đang dừng. Nếu **không có dòng
`[DaTau]` nào**, nghĩa là engine chưa được gọi ⇒ kiểm theo thứ tự:
1. WAuto.exe có đúng là bản ở **gốc** `E:\Src_Auto_Ngoai\` không (không phải `Release\`)?
2. Game.exe trong `bin\client` có dấu thời gian 19/08 06:49 không?
3. Đã khởi động lại game sau khi thay tệp chưa?

---

## 6 · Bảng tra thông báo `[DaTau]` (46 câu)

**Đang chạy bình thường**
- `uu tien Da Tau: di den NPC nhan/tra nhiem vu` — bắt đầu lái, đang tới NPC
- `khong o thanh Da Tau - dung Tho Dia Phu ve thanh`
- `nhiem vu exp: tha cho auto thuong cay, du se tu ve tra` · `chua du exp, cay tiep...`
- `du kinh nghiem/du so cuon, quay ve tra nhiem vu`

**Thiếu điều kiện — người chơi xử lý được**
- `can >=5 o trong de tra nhiem vu` · `tui day (<5 o trong) - don tui roi auto chay lai`
- `tui day, khong mua duoc` · `tui khong du cho de lay do tu ruong`
- `khong du tien mua do nhiem vu` · `khong mo khoa duoc ruong (kiem tra mat khau)`
- `do can nam trong ruong nhung chua cho phep lay` — bật ô "Lấy đồ/tiền từ rương"
- `loai 4 can bat 'Danh quai' o tab Chien dau`
- `loai nhiem vu nay dang TAT - treo` — bật lại loại đó hoặc đổi "Khi bỏ qua"=Hủy
- `du 40 nhiem vu hom nay - nghi den mai` ✅ (đã xong chỉ tiêu)

**Không làm nổi nhiệm vụ → bỏ qua/treo theo cấu hình**
- `khong co do can tim trong tui/ruong - nen tich luy trang suc`
- `khong co do 'khoe' phu hop trong tui/ruong`
- `can mua vu khi/ngua - hay bo san mon nay vao ruong`
- `tiem khong ban mon can mua` · `khong dap ung duoc nhiem vu nay`
- `het Phuc Duyen Lo ma chua du diem` · `dung qua nhieu Phuc Duyen Lo ma chua du`
- `het luot huy, treo nhiem vu`

**Bất thường — cần báo lại cho người viết mã**
- `hoi thoai khong nhan dang duoc` — server đổi lời thoại? (xem mục 8.3)
- `khong hieu noi dung nhiem vu (?)` · `khong khop duoc dong bang du lieu`
- `khong thay NPC Da Tau o toa do` · `khong thay xa phu` · `khong thay chu tiem tap hoa`
- `xa phu khong chuyen map (kiem tra nhiem vu)`
- `farm qua lau khong tien trien - bo qua` · `cay exp qua lau khong tien - treo 15 phut`
- `Phuc Duyen Lo khong tac dung (server chua co script?)`
- `bi NPC phat, cho ~10 phut` (hủy lậu >2 lần đầu chuỗi)

---

## 7 · Quy trình dựng & triển khai (BẮT BUỘC theo thứ tự)

```bash
# 1. Sửa ở D, GREP LẠI chính tệp đó (luật bàn giao)
# 2. Kiểm encoding — FFFD phải = 0
python C:/Users/nguye/.claude/skills/swordonline-dev/scripts/check_encoding.py <tệp>

# 3. Dựng ở D để bắt lỗi biên dịch sớm (dựng THẲNG .vcxproj, KHÔNG qua .sln)
MSBuild Sources/Core/Core.vcxproj "-p:Configuration=Client Release" -p:Platform=Win32
MSBuild Sources/S3Client/S3Client.vcxproj -p:Configuration=Release -p:Platform=Win32 -p:VcpkgEnableManifest=false

# 4. Áp sang cây E:  tệp GIỐNG NHAU thì cp thẳng;
#    KPlayer.cpp / KProtocolProcess.cpp / KProtocol.h KHÁC NHAU -> áp riêng từng hunk
# 5. Dựng ở E (cùng 2 lệnh trên) — build = deploy, tự chép vào bin\client
# 6. Nếu tệp bị khoá (game đang chạy): thay bằng ĐỔI TÊN
mv bin/client/X.dll bin/client/X_cu.dll && cp <bản mới> bin/client/X.dll
# 7. Kiểm CRT (BẮT BUỘC)
python D:/GAMEDEVNEW/ReverseTools/re_pe_crt.py E:/SourceTuanLe/.../bin/client
# 8. WAuto: MSBuild WAuto.vcxproj -p:Configuration=Release -p:Platform=Win32
#    rồi CHÉP RA GỐC:  cp WAuto/WAuto/Release/WAuto.exe  E:/Src_Auto_Ngoai/WAuto.exe
# 9. commit + push ở D
```

### 7.2 Sinh lại bảng TỌA ĐỘ QUÁI (`settings\datau_toado.txt`)
```bash
python D:/GAMEDEVNEW/ReverseTools/gen_datau_spots.py
```
Ghi **2 nơi**: `bin\client\settings\datau_toado.txt` (engine nạp lúc chạy) và
`ReverseTools\datau_toado.txt` (bản trong git), kèm `KDaTauSpots.h` làm **bản dự phòng**
khi thiếu tệp txt. Kết quả 19/08: **204 map · 63.301 quái · 3.365 dòng cụm** (153 map có
quái; 51 map thành thị/trong nhà không có).

**Nguồn:** file add NPC của server trong pak — `\maps\<đường dẫn map>\v_NNN\NNN_Region_S.dat`,
mục `REGION_NPC_FILE_INDEX`. Bộ sinh tự cài lại: định dạng pak `PACK` (`XPackFile.cpp`),
hàm băm tên tệp `KPakList::FileNameToId` (**`char` CÓ DẤU** với byte GBK), giải nén
`ucl_nrv2b_decompress_8` (`ucl/n2b_d.c` + `getbit_8`; kiểu nén `0x01` UCL và `0x20` VNG
dùng **cùng** thuật toán).

**5 cạm bẫy đã trả giá để biết** (đừng bỏ khi sửa bộ sinh):
1. **Chỉ số region cộng offset** `m_nRegionBeginX/Y` → phải quét dải rộng (dùng 0-383;
   map lớn nhất chạm 273). Quét 0-30 ra 0 kết quả, quét 0-255 mất 34% quái của map 25.
2. **Hàm băm chỉ 31 bit hiệu dụng** → quét ~147k tên/map thì đụng vài chục id của tệp
   khác. Lọc bằng **phép kiểm tuyệt đối**: NPC của region `(nx,ny)` phải nằm trong ô của
   chính nó — X ∈ [nx·512, +512), Y ∈ [ny·1024, +1024) (`REGION_GRID_WIDTH` 16 ·
   `REGION_GRID_HEIGHT` 32). Trước khi lọc: **33 map toàn tọa độ bịa**.
3. **Chỉ đọc pak trong `bin\server\package.ini`** (maps.pak + 3 tệp `.mps` + namcung.pak).
   `maps_error.pak`/`maps_tieu_bang_chien.pak` nằm cùng thư mục nhưng server **không nạp** —
   đọc chúng sinh tọa độ ma.
4. **Đừng lọc region theo kích thước** ("≤2100 byte = chỉ có vật cản"): map 1 có 8 region
   CÓ NPC mà vẫn ≤2100 → mất 16 quái.
5. **Neo cụm vào vị trí con quái THẬT** gần tâm nhất, không lấy trung bình cộng: tâm hình
   học rơi vào ô vật cản 10,1% số điểm (map 53 tới 21%), mà engine chỉ né vật cản **một
   lần** mỗi lệnh đi — lần hai là đứng im vĩnh viễn. Đối chứng: 0/5.205 vị trí quái thật
   nằm trên ô vật cản.

**Bao nhiêu map nhiệm vụ là đúng?** Bảng quyết định map loại 4 là
`settings\task\tasklink_findmaps.txt` (cột `MapID`, đọc ở `tasklink_head.lua:97`, ghi vào
task 1031 ở `:141`; Xa Phu `godatau` đọc lại task 1031). Bản **đang chạy** chỉ còn 28 dòng =
**14 map** (1, 11, 21, 37, 53, 75, 78, 80, 122, 162, 176, 225, 226, 227). Bản gốc
`tasklink_findmaps.txt.goc` có 208 dòng = **104 map** — muốn "nhiều map nhiệm vụ" thì phục
hồi tệp đó chứ không sửa mã. `map_index.lua` (`TL_MAPTRAPINDEX`, 204 map) **chỉ là bảng tra
tọa độ**, không phải danh sách nhiệm vụ. Bảng txt hiện phủ cả 204 map nên phục hồi `.goc`
cũng không phải sinh lại.

**Quái sinh từ pak, không phải từ Lua** — đã kiểm chứng: `gamesetting.ini:259`
`NotAddNpcNormal=1`, và `KRegion.cpp:468` là `if (shKind != kind_dialoger)` ⇒ server **giữ
quái**, **bỏ** NPC đối thoại (Lua lo NPC đối thoại). ⚠️ Chú thích trong mã và tên biến
`NotAddNpcNormal` **mô tả ngược** hành vi thật — đừng sửa mã theo chú thích đó.

### 7.1 Sinh lại bảng dữ liệu (khi server đổi bảng/lời thoại)
```bash
python D:/GAMEDEVNEW/ReverseTools/gen_datau_tables.py    # -> Sources/Core/Src/KDaTauTables.h
```
Bộ sinh **assert từng marker**: nếu server đổi chữ trong Lua, nó sẽ **báo lỗi ngay** thay vì
sinh bảng sai. Sau đó dựng lại CoreClient.dll.

---

## 8 · Cạm bẫy đã mắc trong phiên này (đừng lặp lại)

### 8.1 🔴🔴 Edit tool phá 533 byte TCVN3 của `CoreShell.cpp`
Sửa **2 dòng thuần ASCII** bằng Edit tool → **toàn bộ** ký tự tiếng Việt trong tệp thành
U+FFFD. **Build vẫn xanh, không lộ.** Chỉ bắt được nhờ `check_encoding.py`.
→ **LUẬT: mọi `.cpp/.h/.lua` TCVN3 chỉ sửa bằng python latin-1 hoặc `safe_edit.py`.**
Khôi phục: `git checkout <tệp>` rồi áp lại bằng python.

### 8.2 🔴 WAuto.exe deploy sai chỗ = "auto không chạy"
Người dùng bấm `E:\Src_Auto_Ngoai\WAuto.exe`, còn bản dựng nằm ở
`E:\Src_Auto_Ngoai\WAuto\WAuto\Release\WAuto.exe`. Bản cũ gửi struct **6820 B** trong khi
Game.exe mới đọc **6876 B** ⇒ 56 byte đuôi là rác trong bộ nhớ chia sẻ ⇒ có thể kích Dã Tẩu
"rác" làm đứng toàn bộ auto. Đã thêm cổng `bDaTau == 1` chống rác.

### 8.3 🔴 Tệp `.dat` cũ làm tắt hết 6 loại nhiệm vụ
`LoadRoleData` đọc tệp ngắn vào struct dài ⇒ phần Dã Tẩu = 0 ⇒ **mọi loại đều "TẮT"** ⇒ bật
auto là treo ngay. Đã vá: tệp cũ (`uSize < sizeof`) thì áp mặc định Dã Tẩu.

### 8.4 🔴 Máy Dã Tẩu "treo" mà vẫn giữ quyền lái
`DT_Hold` cũ giữ `nDTEngaged=1` ⇒ cổng ở `S3Client.cpp` chặn luôn MOVE/FIGHT/RETURN của auto
thường ⇒ **nhìn y như auto chết** (người dùng nghi xung đột tab Hậu cần). Nay mọi lần treo do
lỗi đều **nhả máy**; chỉ "đủ 40/ngày + tắt ô lên map luyện công" mới đứng yên (`nDTHoldFreeze`).

### 8.5 🔴 Bash tool rút `\\` thành `\`
Chuỗi octal TCVN3 viết trong heredoc bị biến thành byte thật. → Script có backslash **phải đi
qua tệp** (Write tool rồi chạy), đúng như [[bash-inline-backslash-halved]].

### 8.6 🔴🔴 `DT_WalkTo` dùng sai `HaveTarget` → auto đứng yên vĩnh viễn (test thật 19/08, sửa ở `32e60788`)
`KSubWorld::HaveTarget(int& x, int& y)` (KSubWorld.h:181) **GHI target hiện tại RA tham số**
(hàm "xuất", không phải hàm "kiểm tra"). `DT_WalkTo` truyền thẳng đích đến vào ⇒ đích bị ghi đè
thành `(0,0)` ⇒ `FindPath(0,0)` bị chặn (`nX<=0||nY<=0 → -1`, KSubWorld.cpp:911) ⇒ **mọi pha
di chuyển tê liệt**: sau đúng 1 dòng `[DaTau] uu tien Da Tau...` nhân vật đứng im, không thêm
thông báo nào (watchdog GOTONPC chỉ đếm khi ĐÃ tới tọa độ). Mã đúng xem `ATYPE_MOVE` nhánh
`bAroundPoint` (CoreShell.cpp:8671): hứng target ra **biến nháp**, truyền đích **riêng** vào
`FindPath`, chỉ re-path khi target ≠ đích. Đường tiêu thụ path (`KSubWorld::Activate`:1082)
chạy vô điều kiện mỗi tick nên chỉ cần FindPath nhận đích đúng là nhân vật đi.
Kèm theo: build Client của cây D hỏng từ `561e2163` (2 `PB_LogNgoai` ngoài `#ifdef _SERVER`,
lỗi C3861) — đã bọc lại ở `0d3c6629`. Số dòng CoreShell.cpp sau 2704 lệch **+4** so với THAMCHIEU.

---

### 8.7 🔴 Năm lỗi bắt được khi test thật 19/08 chiều (`f6f550c5` + `6bde16a1`)

| Triệu chứng người dùng | Nguyên nhân thật | Sửa |
|---|---|---|
| "ra bảng chọn phần thưởng mà không chọn" | `seasonnpc.lua` hàm `Prise`: **một** cửa sổ, 3 thưởng gán vào 3 nút của cửa sổ đó; bấm nút **không được ánh xạ** thì `Prise_Chon` **mở lại cửa sổ** (`:1327-1333`), bấm nhầm nhóm thì C++ dùng reward-id nhóm kia (=0) | `DTP_REWARD` xoay nút: nút người chơi chọn → mở lại thì nút kế → hết nhóm này sang nhóm kia → quá 6 lần thì bỏ qua. Bắt `uFinSeq` ở **mọi pha** |
| "chọn Hủy nhiệm vụ thì nó hủy luôn loại đang bật" | `DT_Skip` hủy vô điều kiện khi `nDTSkipMode==1` | chỉ hủy khi loại hiện tại **bị TẮT**; loại đang bật mà kẹt thì treo |
| "chưa xong nhiệm vụ đã tự phù về" | (a) id map đọc từ **văn bản** nhiệm vụ lệch với map Xa Phu thả xuống (map nhiều tầng) ⇒ nhánh "lạc map" bắt phù về; (b) `DTP_IDLE` sau mỗi lần hết treo coi map nhiệm vụ là "không ở thành" | (a) **tin máy chủ**: Xa Phu thả ra khỏi thành = đúng map, gán `nDTMapId = nMap`; (b) IDLE đang giữ loại 4 đúng map thì vào thẳng `DTP_FARM` |
| "đi về thành trả nhiệm vụ phải lên ngựa" | `DT_WalkTo` không hề lên ngựa | thêm `DT_Ride` (khuôn `case PA_RIDE` `CoreShell.cpp:9614`) gọi trong `DT_WalkTo` mọi pha đi đường, **trừ** `DTP_FARM` |
| "quét không được NPC trong map" | client chỉ thấy NPC đã đồng bộ; neo nhiệm vụ (chỗ Xa Phu thả) ở map 53/80/226 **nằm ngoài vùng có quái** | bảng cụm quái sinh từ pak — xem mục 7.2 |

---

### 8.8 🔴 Hai nguồn tên map — "sa mạc 3" báo không đọc được map rồi treo (`ae1129f8`)

Câu nhiệm vụ loại 4 ghép tên map từ **cột `TaskInfo1` của `settings\task\tasklink_findmaps.txt`**
(`tasklink_head.lua:924` lấy `myTaskOrder`, `:942` ghép `"Ngươi hãy đến <color=yellow>"..myTaskOrder..`),
**không** phải từ `map_index.lua`. Bảng `g_DTQuestMap` lại lấy tên từ `map_index.lua`. 11 map
trùng tên nên chạy tốt suốt, riêng 3 map sa mạc lệch hẳn thứ tự chữ:

| Map | Câu nhiệm vụ (TaskInfo1) | map_index.lua |
|---|---|---|
| 225/226/227 | `Sa Mạc sơn␣␣động N` (hai dấu cách) | `Sơn Động Sa mạc tầng N` |

⇒ `DT_HasName` trượt ⇒ `nDTMapId = 0` ⇒ `DT_Skip` ⇒ **treo** (vì loại 4 đang bật, theo luật
"chỉ hủy loại đã tắt" chốt cùng ngày). Sửa **tận gốc ở bộ sinh**: danh sách map và tên đều đọc
từ `tasklink_findmaps.txt` (bỏ luôn danh sách `QUEST_MAPS` chép tay), tọa độ vẫn từ
`map_index.lua`; tên nào lệch thì phát **thêm một dòng** với tên còn lại — vòng khớp
`CoreShell.cpp:3049` break ở dòng đầu trùng nên dòng dư vô hại. 14 map → 17 dòng, **không phải
sửa C++**.

---

## 9 · Phản biện — đã làm gì

| Vòng | Kết quả |
|---|---|
| **Vòng 1** (5 lens × verify độc lập) | 30 finding → **4 CONFIRMED**, đều là **thiếu watchdog gây kẹt mềm**; KHÔNG có lỗi bộ nhớ/dữ liệu/protocol |
| **Verify bản vá** | bắt thêm 2 lỗi *trong chính bản vá* (chốt bị vô hiệu; watchdog bị tắt) → đã sửa |
| **Vòng 2** (toàn diện) | ❌ **HẾT HẠN MỨC PHIÊN** (5/5 mũi lỗi) — **CÒN NỢ, nên chạy lại** |
| **Tự kiểm thay thế** | ✅ 36/36 marker khớp byte với Lua · ✅ `DT_HasName` không trượt · ✅ encoding · ✅ D≡E · ✅ 3 bản ipc_shared khớp md5 · tự tìm thêm 2 lỗi thật đã sửa |

**Bốn lỗi đã sửa từ phản biện:** `nDTUnknown` (kẹt khi NPC trả hội thoại lạ) ·
`nDTXaFuTry` (kẹt vòng xa phu) · `uDTFarmStall` (farm loại 4 & exp không tiến 20 phút) ·
`DT_HasName` (tên ngắn khớp nhầm vào tên dài, vd "Giới Chỉ (Kim)" ⊂ "Hoàng Ngọc Giới Chỉ (Kim)").

**Các FALSE_POSITIVE đáng ghi nhận** (xác nhận thiết kế đúng): hộp nộp đồ dùng 1 gói giống UI
thật · không làm mất đồ (server không tiêu huỷ khi trả sai) · không spam gói tin · không hồi
quy auto cũ khi `bDaTau=0` · ID tab-8 liền mạch < `IDC_INDEX_END` · nút tab 9 cùng nhóm radio.

---

## 10 · Việc còn nợ / giới hạn phiên bản 1

| # | Việc | Ghi chú |
|---|---|---|
| 1 | **Người dùng test thật** | chưa chạy lần nào — việc đầu tiên phiên sau |
| 2 | **Chạy lại phản biện vòng 2** | 5 mũi, bị hết hạn mức; script còn trong thư mục workflows |
| 3 | Mua **ngựa** (loại 1, dòng 30-36) | mỗi thành bán ngựa khác nhau → hiện bỏ qua |
| 4 | Loại 5: PK / Tống Kim / Danh vọng | chỉ thử trả một lần rồi bỏ qua, không tự cày |
| 5 | Phúc Duyên Lộ (6/1/121-123) | `fuyuanlu_*.lua` **thiếu ở cây D** (có thể có trên server sống); auto tự phát hiện item vô tác dụng sau 2 lần rồi bỏ qua |
| 6 | Mã nguồn UI chưa có git | nên sao lưu `E:\Src_Auto_Ngoai\WAuto` vào repo |
| 7 | 2 dòng dữ liệu gốc sai hệ | `tasklink_findgoods.txt` dòng 27 & 47 — lỗi **dữ liệu server**, không phải mã; server cũng đọc cùng bảng nên trả vẫn theo `nFive` |

---

## 12 · Công cụ Dã Tẩu trong LỆNH BÀI ADMIN (19/08, `d72ace99`)

Vào lệnh bài admin → **"Da Tau: xoa phat huy + them luot huy"**. Mã ở
`script\item\datau_admin.lua` (ASCII thuần, có cả bản trong git `serverscript_jx2\jx1_edits\`),
được `lenhbaiadmin.lua` `Include` (2 dòng chèn byte-safe, TCVN3 giữ nguyên 172 byte cao).

| Mục | Làm gì | Ghi vào |
|---|---|---|
| Xóa phạt | bỏ phạt "hủy nhiều nhiệm vụ", gặp NPC là làm tiếp ngay | `1036=0`, `1029=0`, và **ghi lại `1046` cho khớp** để chữa luôn ca bị cấm hủy do lệch bản sao |
| Thêm 10 lượt hủy | cộng lượt hủy (kẹp trần **100**) | byte 4 của `1020` **và** `1046` |
| Đặt lại lượt hủy = 0 | về 0 | như trên |
| Đặt lại số nhiệm vụ hôm nay | bỏ trần 40/ngày | `2420=0`, `2797=0` (⚠️ ăn lại được thưởng mốc 30/40) |

**Bản đồ biến đếm (truy từ mã thật, đừng đoán lại):** lượt hủy = **BYTE 4 của task 1020**
(`tl_settaskstate`, tasklink_head.lua:488-500) · **1046** = bản sao chống gian lận, lệch với
byte 4 là **cấm hủy vĩnh viễn + ghi log gian lận** (`_CancelTaskDebug`, seasonnpc.lua:1121) ⇒
**đổi lượt hủy phải ghi CẢ HAI** · phạt = **1036 == 10** (đặt ở `Task_Cancel` khi hủy quá 3
lần đầu chuỗi, seasonnpc.lua:676-683) + mốc thời gian **1029**, hệ thống chỉ tự tha sau
**10890 tick ≈ 10 phút** · **2419** ngày · **2420** số nhiệm vụ/ngày (trần 40) · **2797** số
lần hủy trong ngày, chỉ dùng tính thưởng mốc 30/40 chứ không chặn gì.

🔴 **Trần lượt hủy THẬT là 127, không phải 254** như `tl_settaskstate` ghi: `LuaGetByte`
(ScriptFuns.cpp:121-134) đọc byte 4 bằng `(x & (0xff<<24)) >> 24` trên **int có dấu** nên byte
≥128 đọc ra **số âm** → `Task_Cancel` tưởng "hết lượt" và rơi vào nhánh **xóa sạch chuỗi
nhiệm vụ** (seasonnpc.lua:705-725). Đây là **bug có sẵn của hệ gốc** (người chơi tự tích lượt
hủy bằng nút thưởng "finish_lucky" cũng dính) — công cụ admin kẹp trần 100 và tự chữa khi
gặp byte âm.

🔑 **Sửa Lua KHÔNG cần restart server**: `LuaIncludeFile` = `lua_dofile`, **không cache**
(ScriptFuns.cpp:1964), và `main()` của lệnh bài tự `dofile` lại chính nó mỗi lần dùng vật phẩm.
Các tệp `.lua` **khác** (không nằm trong cây Include của lệnh bài) thì vẫn phải restart.

🔑 Giới hạn khi thêm mục menu: tiêu đề + các mục **dùng chung đệm 512 byte**
(`MAX_SCIRPTACTION_BUFFERNUM`, cắt âm thầm không báo lỗi), số mục tối đa 50
(`MAX_ANSWERNUM`), tên hàm gọi lại tối đa 31 ký tự, và **cấm ký tự `|`** trong nhãn. Menu hiện
dùng ~415/512 byte với 8 mục.

---

## 11 · Tra cứu nhanh

```bash
# Engine
grep -n "AUTO DA TAU" D:/GAMEDEVNEW/Sources/Core/Src/CoreShell.cpp
# Hook chụp gói
grep -n "g_sDTCap" D:/GAMEDEVNEW/Sources/Core/Src/KPlayer.cpp D:/GAMEDEVNEW/Sources/Core/Src/KProtocolProcess.cpp
# Cổng điều phối
grep -n "nDT" D:/GAMEDEVNEW/Sources/S3Client/S3Client.cpp
# Mọi thông báo trạng thái
grep -o '"\[DaTau\][^"]*"' D:/GAMEDEVNEW/Sources/Core/Src/CoreShell.cpp | sort -u
# Kiểm binary đã triển khai có mã mới chưa
grep -c "\[DaTau\]" E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/client/CoreClient.dll
```
