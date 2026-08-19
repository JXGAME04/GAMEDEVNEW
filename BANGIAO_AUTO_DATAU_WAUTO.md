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
| **Trạng thái** | Viết xong · build sạch · phản biện 2 vòng · **đã deploy** · **NGƯỜI DÙNG CHƯA TEST THẬT** |
| **Commit** (`D:\GAMEDEVNEW`, nhánh main) | `f606e540` (chính) → `1d24b9fc` → `8a9ae8f5` |
| **Nơi bật** | WAuto.exe → tab **"Dã Tẩu"** → tick "Bật auto Dã Tẩu" |
| **Mặc định** | **TẮT** (`bDaTau=0`) — không tick thì hành vi auto cũ nguyên vẹn |
| **Việc kế tiếp** | Hỏi người dùng kết quả test + dòng `[DaTau]` cuối trong khung chat |

---

## 2 · Trạng thái binary đã triển khai (19/08)

| Tệp | Đường dẫn | Dấu thời gian | Ghi chú |
|---|---|---|---|
| `CoreClient.dll` | `E:\SourceTuanLe\...\TESTLOFFF_ONLINE\bin\client\` | 19/08 06:49 | chứa engine Dã Tẩu |
| `Game.exe` | như trên | 19/08 06:49 | chứa cổng điều phối |
| `WAuto.exe` | **`E:\Src_Auto_Ngoai\`** (gốc, KHÔNG phải `Release\`) | 19/08 09:21 | tab 9 + vá `.dat` cũ |

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
