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
| **Trạng thái** | test lần 1 đứng yên → fix `DT_WalkTo` (8.6) → **test lần 2 ĐÃ CHẠY** ✅ → nâng cấp trưa 19/08 → 5 lỗi test chiều (8.7) + bảng cụm quái (7.2) → tối: bấm rương thật (8.9) + 54 thông báo màu (mục 6) → **ĐỢT 20/08: 11 mục người dùng (mục 13 — hộp giao, bán rác tại chỗ, phù 437, lệnh bài, MUA Ở SẠP + nhảy 10 thành, ưu tiên tab Di chuyển, UI 2 cột…)** — chờ test |
| **Commit** (`D:\GAMEDEVNEW`, nhánh main) | … → `ae1129f8` → `e40f40ea` (bấm rương thật + thông báo màu) → **`c9de7a7b` (đợt 20/08, 11 mục)** |
| **Nơi bật** | WAuto.exe → tab **"Dã Tẩu"** → tick "Bật auto Dã Tẩu" |
| **Mặc định** | **TẮT** (`bDaTau=0`) — không tick thì hành vi auto cũ nguyên vẹn |
| **Việc kế tiếp** | Hỏi người dùng kết quả test + dòng `[Dã Tẩu]` màu cuối cùng trong khung chat |

---

## 2 · Trạng thái binary đã triển khai (19/08)

| Tệp | Đường dẫn | Dấu thời gian | Ghi chú |
|---|---|---|---|
| `CoreClient.dll` | `E:\SourceTuanLe\...\TESTLOFFF_ONLINE\bin\client\` | **20/08 12:08** (2.203.648 B) | engine vòng 4+5 (mục 13.14): loại 3 đi chợ + 4 fix + Thần Hành Phù; bản lùi: `_locked7` (10:11, thiếu vá r5b) |
| `settings\datau_toado.txt` | `...\bin\client\settings\` | **19/08 17:10** (80 KB) | **MỚI** — bảng tọa độ cụm quái 204 map, engine nạp lúc chạy; sửa tay được, không cần dựng lại DLL |
| `Game.exe` | như trên | **19/08 21:32** (1.251.840 B) | `GDCNI_UI_ACT uParam=7` (AutoPick) + đọc `autoData` **6888 B**; bản lùi = `Game_cu_1908tt.exe` |
| `WAuto.exe` | **`E:\Src_Auto_Ngoai\`** (gốc) **và** `bin\client\WAuto.exe` | **19/08 21:35** (361.472 B) | tab Dã Tẩu bố cục 2 cột + 3 điều khiển mới (LB 435, MS 436, MM 437/438, INDEX_END→441); gửi `autoData` 6888 B — **phải dùng đúng cặp WAuto+Game 21:3x**. ⚠️ post-build gọi `pwsh.exe` không có trên máy ⇒ **luôn chép tay**. ⚠️ WAuto tự thoát nếu không có Game.exe đang chạy |
| `script\item\lbhtdatau.lua` | `...\bin\server\script\item\` | **19/08 21:37** (1.583 B) | **VIẾT LẠI cho tasklink** (mục 13.9); bản cũ = `lbhtdatau_cu_1908.lua`. ⚠️ **cần restart GameServer** mới ăn (script item bị cache; đằng nào cũng đang chờ restart cho DLL bot của phiên song song) |

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

### 4.3 HAI bản `ipc_shared.h` phải giống hệt nhau (từ 20/08)

`D:\GAMEDEVNEW\Sources\Core\Src\` · `E:\Src_Auto_Ngoai\WAuto\WAuto\` — **sizeof(autoData) = 6888**
(20/08 thêm `bDTLenhBai`/`bDTMuaSap`/`nDTMaxMua` ở CUỐI). Lệch một byte là Core đọc sai offset
(ép kiểu thẳng `(autoData*)nParam`) — hỏng âm thầm, không có version handshake.
Bản thứ ba ở cây `E:\SourceTuanLe\...\SOURCESUPDATE_KINHMACH_ONLTEST0608` **KHÔNG cập nhật nữa**
— cây nguồn E đã bỏ (build ở D, chép tay artifact; xem đầu mục 13).

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
| Dùng Lệnh bài hoàn thành khi loại tắt | ✔ (mặc định; chỉ tác dụng khi có item 6,1,4818 + server đã restart) |
| Thiếu đồ: mua ở sạp | ✔ (mặc định) — cần có chút tiền đi Xa Phu (10-50 lượng/chuyến) |
| Giá sạp tối đa (vạn) | 200 (mặc định) — món rẻ nhất đạt yêu cầu sẽ được mua |
| Xong lên map luyện công | tuỳ ý (tắt = đứng yên khi xong) |
| Tab Chiến đấu | **bật "Đánh quái"** — loại 4 bắt buộc cần |

### 5.3 Chạy
- Cho nhân vật đứng ở **1 trong 10 thành có NPC Dã Tẩu** (Phượng Tường, Thành Đô, Giang Tân
  Thôn, Biện Kinh, Ba Lăng, Tương Dương, Dương Châu, Long Môn Trấn, Đại Lý, Lâm An).
  Đứng nơi khác thì auto tự dùng Thổ Địa Phù về thành.
- Tick ô auto ở dòng nhân vật như bình thường.
- **Theo dõi khung chat**: mọi bước đều báo dòng `[Dã Tẩu]: ...` **có màu** (bảng tra ở mục 6).

### 5.4 Nếu không thấy nhúc nhích
Chụp lại **dòng `[Dã Tẩu]` cuối cùng** — đó chính là chỗ máy đang dừng. Nếu **không có dòng
`[Dã Tẩu]` nào**, nghĩa là engine chưa được gọi ⇒ kiểm theo thứ tự:
1. WAuto.exe có đúng là bản ở **gốc** `E:\Src_Auto_Ngoai\` không (không phải `Release\`)?
2. Game.exe trong `bin\client` có dấu thời gian 19/08 06:49 không?
3. Đã khởi động lại game sau khi thay tệp chưa?

---

## 6 · Bảng tra thông báo (từ 19/08 tối `e40f40ea`: **tiếng Việt có dấu + màu**)

Người gửi trong chat là **`[Dã Tẩu]:`** (TCVN3). Màu nhúng bằng thẻ `<color=...>` —
chat CHỈ lọc mã màu thô 0x02 (`FilterTextColor`) rồi **mới** dịch thẻ (`TEncodeText`,
Text.cpp:487) nên màu vẫn ăn. Hai luật khi thêm câu mới: trước mỗi thẻ `<` phải là
ký tự ASCII (dấu cách); ký tự **cuối chuỗi** phải ASCII (byte TCVN3 đơn cuối chuỗi bị drop).

| Màu | Ý nghĩa |
|---|---|
| **Cyan** (xanh ngọc) | đang chạy bình thường (đi NPC, về thành, ra Xa Phu, cày tiếp) |
| **Green** (xanh lá) | xong một bước tốt (đủ exp, đủ cuộn, tới map nhiệm vụ) |
| **Yellow** (vàng) | người chơi cần xử lý: dọn túi, mật khẩu rương, bật ô cấu hình, thiếu tiền |
| **Orange** (cam) | không đáp ứng được nhiệm vụ / thiếu đồ / bị phạt → bỏ qua hoặc treo |
| **Red** (đỏ) | lỗi bất thường — cần báo lại cho người viết mã |
| **AYellow** (vàng kim) | mốc vui: rương thưởng mở, nhận thưởng xong, đủ 40 nhiệm vụ |
| **Gray** (xám) | ghi chú máy tự thêm: `(tạm nghỉ N phút)` (mọi DT_Hold có hạn) · `(hủy nhiệm vụ - loại này đang tắt)` (DT_Skip nhánh hủy) |

**Cyan — đang chạy:** `Đang đến chỗ NPC Dã Tẩu để nhận / trả nhiệm vụ.` ·
`Không ở thành có Dã Tẩu - dùng Thổ Địa Phù về thành.` · `Đã về thành - ra Xa Phu đi tiếp tới map nhiệm vụ.` ·
`Không phải map nhiệm vụ - dùng Thổ Địa Phù về thành đi lại.` · `Nhiệm vụ kinh nghiệm: thả cho auto thường cày, đủ sẽ tự về trả.` ·
`Chưa đủ kinh nghiệm - cày tiếp...` · `Túi đã có chỗ trống - quay lại làm Dã Tẩu.`

**Green — xong bước:** `Đã đủ kinh nghiệm - quay về trả nhiệm vụ.` · `Đã tới map nhiệm vụ - bắt đầu đánh quái nhặt cuộn.` ·
`Đã nhặt đủ số cuộn - quay về trả nhiệm vụ.`

**AYellow — mốc thưởng:** `Rương thưởng đã mở - auto bấm chọn phần thưởng...` ·
`Đã nhận thưởng xong - đi nhận nhiệm vụ kế tiếp!` · `Tuyệt! Đã đủ 40 nhiệm vụ Dã Tẩu hôm nay - nghỉ, mai làm tiếp.`

**Yellow — người chơi xử lý:** `Túi đầy (dưới 5 ô trống) - hãy dọn túi rồi auto chạy tiếp.` ·
`Cần ít nhất 5 ô trống để trả nhiệm vụ - hãy dọn túi.` · `Túi đầy - nhờ auto Hậu cần bán rác, xong sẽ tự làm tiếp.` ·
`Bán rác xong vẫn chưa đủ ô trống - hãy dọn bớt túi giúp auto.` · `Túi đầy, không mua được đồ.` ·
`Túi không đủ chỗ để lấy đồ từ rương - hãy dọn túi.` · `Không đủ tiền mua đồ nhiệm vụ.` ·
`Không mở khóa được rương - kiểm tra mật khẩu rương ở tab Hậu cần.` ·
`Đồ cần trả đang nằm trong rương - hãy bật ô «Lấy đồ/tiền từ rương» ở tab Dã Tẩu.` (tên ô màu trắng) ·
`Nhiệm vụ đánh quái: hãy bật ô «Đánh quái» ở tab Chiến đấu.` (tên ô màu trắng) ·
`Loại nhiệm vụ này đang tắt trong tab Dã Tẩu - treo chờ.` (chữ "tắt" màu đỏ)

**Orange — bỏ qua/thiếu đồ:** `Không đáp ứng được nhiệm vụ này.` · `Không có đồ cần tìm trong túi/rương - nên tích trữ sẵn trang sức.` ·
`Không có đồ 'khoe' phù hợp trong túi/rương.` · `Nhiệm vụ cần vũ khí/ngựa - hãy bỏ sẵn món đó vào rương (xem tên trong nhiệm vụ).` ·
`Tiệm không bán món cần mua.` · `Tiệm không có mục giao dịch phù hợp.` · `Không mở được cửa sổ tiệm tạp hóa.` ·
`Không thấy chủ tiệm tạp hóa.` · `Không thấy Xa Phu.` · `Hết lượt hủy nhiệm vụ.` ·
`Bị Dã Tẩu phạt vì hủy nhiều - chờ hết phạt sẽ làm tiếp.` · `Đánh quái quá lâu không tiến triển.` ·
`Dùng quá nhiều Phúc Duyên Lộ mà vẫn chưa đủ điểm.` · `Hết Phúc Duyên Lộ mà vẫn chưa đủ điểm.`

**Red — bất thường, báo dev:** `Hội thoại không nhận dạng được (server đổi lời thoại?).` ·
`Không hiểu nội dung nhiệm vụ (lạ) - hãy báo lại lỗi này.` · `Không khớp được dòng bảng dữ liệu nhiệm vụ.` ·
`Không thấy NPC Dã Tẩu ở tọa độ đã định.` · `Không thấy nút hủy trong hội thoại.` · `NPC không trả lời hội thoại.` ·
`Kẹt: không ở thành mà không dùng được Thổ Địa Phù.` · `Kẹt: không về được thành (hết Thổ Địa Phù?).` ·
`Thành này chưa có tọa độ tiệm tạp hóa.` · `Mất tọa độ tiệm tạp hóa.` · `Thành này chưa có tọa độ Xa Phu.` ·
`Xa Phu không chở đi map nhiệm vụ (kiểm tra lại nhiệm vụ).` · `Cày kinh nghiệm quá lâu không tiến triển.` ·
`Không đọc được tên map / số lượng của nhiệm vụ loại 4 (đánh quái).` · `Loại nhiệm vụ lạ - auto chưa hỗ trợ.` ·
`Không chọn được rương thưởng (đã thử đủ 6 nút) - bỏ qua.` · `Phúc Duyên Lộ không có tác dụng (server thiếu script?).`

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

### 8.9 Cửa sổ thưởng nay được BẤM THẬT — hết cảnh "nhận rồi mà bảng vẫn hiện" (`e40f40ea`, 19/08 tối)

Người dùng báo: *"nhận thưởng người chơi chưa tự kích vào phần thưởng để chọn mà chạy
thẳng script nên cho dù nhận thưởng rồi nhưng vẫn hiện bảng nhận thưởng"*.

**Nguyên nhân thật:** nút của `KUiDaTau`/`KUiDaTau1` khi người chơi bấm làm HAI việc —
`Hide()` **rồi mới** `OperationRequest(GOI_ADD_UI_CMD_SCRIPT, ...)` (UiQuestDT.cpp:87-117).
Engine cũ chỉ gọi `SendUiCmdScript(...)` = nửa sau, **thiếu `Hide()`** ⇒ server phát thưởng
nhưng cửa sổ (client tự quản, server KHÔNG gửi lệnh đóng) trơ trên màn hình mãi.

**Cách sửa:** đi đúng đường click của người chơi:
```
DTP_REWARD ──CoreDataChanged(GDCNI_UI_ACT, 7, nhóm*10+nút)──► GameSpaceChangedNotify.cpp
              └─► KUiDaTau::AutoPick(nút) / KUiDaTau1::AutoPick(nút)   [hàm mới]
                    └─ WndProc(WND_N_BUTTON_CLICK, &nút) → Hide() + GOI_ADD_UI_CMD_SCRIPT
```
- nParam mã hóa `30..32` = KUiDaTau (exp/tiền/ngẫu nhiên — đúng thứ tự `DT_FIN3`),
  `40..42` = KUiDaTau1 (điểm/may mắn/vật phẩm — đúng `DT_FIN4`).
- AutoPick trả **1** = đã bấm. Trả **0** = cửa sổ nhóm đó không mở (xảy ra ở 3 lần thử
  chéo nhóm) ⇒ UI **ẩn nốt cửa sổ thừa** (AutoHide cả hai) rồi engine gửi thẳng script
  dự phòng như bản cũ — không bao giờ mất thưởng, màn hình vẫn sạch.
- Vòng xoay 6 nút + bắt `uFinSeq` giữ nguyên; chỉ thay cách "bấm".
- Vào pha lần đầu (`nThu==0`) có thêm câu vàng kim `Rương thưởng đã mở - auto bấm chọn phần thưởng...`
- ⚠️ chú thích trong mã ghi nhầm "(20/08)" — thật ra là tối **19/08**.

Vì sao give-box (hộp nộp đồ) không dính lỗi này: server có gói đóng riêng
(`S2C_GIVE_BOX nType==2` → `GDCNI_END_AFFAIR_BOX`), còn cửa sổ thưởng thì không.


## 13 · ĐỢT 20/08 (tối 19/08, `c9de7a7b`) — 11 mục theo yêu cầu: CÁCH HOẠT ĐỘNG

> Toàn bộ nằm trong `CoreShell.cpp` (engine), trừ chỗ ghi rõ. Đã build + deploy 21:32-21:35.
> ⚠️ **Cây nguồn E không dùng nữa** (chốt của phiên F11): build ở `D:\GAMEDEVNEW` rồi CHÉP TAY
> artifact (`Sources\Core\ClientRelease\CoreClient.dll`, `Sources\S3Client\Release\Game.exe`)
> vào `bin\client`. **Đừng build cây E nữa — sẽ mất tính năng F11 + đợt này.**

### 13.1 Hộp giao vật phẩm nhận được item (bug "có đồ đúng mà không bỏ vào box")
- **Gốc bệnh:** server `KItemList::ExchangeItem` **từ chối** mọi gói move-item có `Down != Up`
  (KItemList.cpp:2227) — mỗi gói phải là MỘT CÚ CLICK trong đúng một ô đồ (nhặt lên tay /
  đặt xuống theo `m_Hand`). Engine cũ gửi 1 gói Down=túi/Up=hộp ⇒ server im lặng bỏ qua.
- **Cách chạy mới:** `DT_ClickItem` phát 3 gói click như người kéo thật (khuôn KPlayer.cpp:3281):
  ① click ô item trong túi (nhấc lên tay) → ② click ô (0,0) hộp giao (đặt xuống) → ③ click lại
  ô cũ (nếu đặt trượt thì item tự về túi, không kẹt trên tay; đặt trúng thì click 3 vô hại).
  Sau 900ms mới bấm OK (`SendUiCmdScript(1, szBoxFunc)` — đúng gói của nút OK thật).

### 13.2 Phúc Duyên lấy từ rương không cần bật ô "Lấy đồ/tiền từ rương"
- `DTP_USEPD` giờ chỉ cần **có mật khẩu rương** (tab Hậu cần) là tự kéo Phúc Duyên Lộ
  (6,1,121/122/123 = Tiểu +10 / Trung +20 / Đại +50) từ rương về túi rồi dùng; đủ điểm
  (server so DELTA với mốc lúc nhận, task 1026) thì về NPC trả; hết item → hủy/treo theo ô
  "Khi bỏ qua" như cũ.

### 13.3 Túi đầy: TỰ BÁN RÁC TẠI CHỖ (hết vòng "phù về − không bán − đi xa phu − phù về")
- **Gốc bệnh cũ:** túi đầy → thả máy cho auto Hậu cần; nhưng Hậu cần chạy theo cấu hình riêng
  (không bật "Bán vật phẩm" thì không bán; xong việc lại tự quay về bãi/đi trạm xe) ⇒ lặp vô tận.
- **Cách chạy mới:** `DTP_SELLJUNK` tự bán **tại chỗ** (server cho bán không cần đứng cạnh tiệm —
  chính máy Hậu cần gốc vẫn bán ngay bước 1): mỗi 700ms bán 1 **trang bị trắng/xanh chưa khóa**,
  tôn trọng bộ lọc giữ đồ của tab Hậu cần (không bán ngựa nếu tắt, giữ nhẫn/dây/bội cấp cao theo
  `bSaveRing`, giữ đồ có dòng ma theo bảng lọc, +tất kỹ năng luôn giữ) **và không bao giờ bán item
  đạt yêu cầu nhiệm vụ (13.8)**. Đủ 8 ô (hoặc hết rác mà ≥5 ô) → quay lại làm tiếp ngay.
  Gửi ≥4 lệnh bán mà số ô trống không tăng + đang ngoài thành → phù về thành bán tiếp; hạn 4 phút.

### 13.4 Phù về thành: KHÔNG tự mua nữa + dùng phù vô hạn (6,1,437)
- Bỏ cả hai chỗ tự mua từ xa (`SendClientCmdOpenShop(item_townportal,...)` ở DTP_RETURN và
  chuẩn bị loại 4). Thứ tự dùng mới: **phù VÔ HẠN (6,1,437)** (không tiêu hao) → Thổ Địa Phù
  (5,0,0) → Hồi thành phù (6,1,1083/1084) → **kéo từ rương** (cần mật khẩu) → treo + nhắc
  vàng "bỏ phù vào túi giúp auto". Loại 4 trước khi đi chỉ NHẮC nếu không có phù (vẫn đi làm).

### 13.5 Đang treo Tìm/Khoe mà nhặt được đồ đạt yêu cầu → tự về trả
- Trong `DTP_HOLD` (trừ hold "đủ 40"), nếu đang giữ nhiệm vụ loại 2/3: mỗi 5 giây quét
  túi+rương theo đúng luật ứng viên; thấy đồ đạt → nhả treo, vào `DTP_EXEC` (tự lo kéo từ
  rương nếu cần) → về NPC trả. Có báo xanh lá trong chat.

### 13.6 Đủ 40 nhiệm vụ/ngày: nghỉ rồi TỰ chạy lại
- Như cũ: sang ngày mới (giờ máy) là mở lại ngay. **Mới:** hold "đủ 40" giờ có hạn 60 phút —
  mỗi giờ tự ra hỏi NPC một lần, phòng ngày của SERVER lệch ngày máy (múi giờ); server reset
  là chạy tiếp trong vòng ≤1 giờ. Vẫn cần đang tick "Bật auto Dã Tẩu".

### 13.7 FARM loại 4 (Địa đồ chỉ / Mật chỉ): ƯU TIÊN tab Di chuyển
- Nếu tab Di chuyển cấu hình **đúng map nhiệm vụ** (`nMoveMapId == map`): "Di chuyển theo tọa
  độ" → đi tuần tự các tọa độ đã đặt (dùng làm chuỗi điểm farm); "Quanh điểm" → đảo 8 hướng
  quanh điểm đã đặt (bán kính = Phạm vi nhìn, kẹp 600-1800). Bảng cụm quái `datau_toado.txt`
  chỉ dùng khi tab Di chuyển KHÔNG cấu hình cho map đó. Đuổi quái đã lọt tầm nhìn vẫn ưu tiên
  trước như cũ.

### 13.8 CẤM bán nhầm item nhiệm vụ (cẩn thận theo yêu cầu)
- `DT_IsQuestItem`: khi đang giữ nhiệm vụ loại 1/2/3 (auto Dã Tẩu bật), mọi item khớp **bất kỳ
  ứng viên nào** của nhiệm vụ (kể cả khớp dòng ma của loại khoe) + item đã chốt nộp đều bị
  chặn ở: ① máy bán rác Hậu cần (bước bán, CoreShell), ② máy bán tại chỗ 13.3. Thiên vị an
  toàn: thà giữ nhầm còn hơn bán nhầm.

### 13.9 Lệnh bài hoàn thành Dã Tẩu (6,1,4818) — Ô MỚI "Dùng Lệnh bài hoàn thành khi loại tắt"
- **Client:** gặp nhiệm vụ thuộc loại đã TẮT + ô bật: có lệnh bài trong túi → đóng thoại, dùng
  luôn; nằm trong rương → kéo về (cần mật khẩu). Server hoàn thành + bung rương thưởng ⇒
  `uFinSeq` bắt được ở mọi pha → bấm rương như thường → nhận nhiệm vụ kế. Dùng 2 lần mà không
  thấy thưởng (hội thoại loại-tắt lại hiện) → báo đỏ "server chưa cập nhật script?" rồi làm
  theo ô "Khi bỏ qua". Bộ đếm reset khi sang nhiệm vụ khác.
- **Server (`lbhtdatau.lua` viết lại):** bản cũ nói chuyện với hệ Dã Tẩu CHẾT (task 87-98) nên
  luôn từ chối. Bản mới: guard trần 100 lượt (task 318, giữ nguyên ý gốc) → đang có nhiệm vụ
  tasklink (`1021≠0, 1030≠0, course==1`) → cần ≥5 ô trống → `Task_AwardRecord()` (course=2,
  cộng chuỗi/điểm y hệt trả ở NPC) → trừ 1 lệnh bài → `Task_GiveAward()` (mở rương thưởng;
  callback finish_* quay về đúng script item vì genre 6 chạy bGlobal=true). Lỡ mất cửa sổ
  thưởng thì gặp NPC — course 2 có nhánh phát lại. **Cần restart GameServer mới ăn script mới.**

### 13.10 Thiếu đồ loại 2 (tìm vật phẩm): ĐI CHỢ MUA Ở SẠP — Ô MỚI "Thiếu đồ: mua ở sạp"
- Thứ tự đúng yêu cầu: túi → rương (như cũ, trong EXEC) → **sạp**. `DTP_MUASAP`:
  ① quét `Npc[].m_BaiTan` (người thật + bot dùng chung hệ sạp) quanh mình, đi tới ≤320mps
  (server chỉ cần chung/cạnh region), mở xem bằng `g_cSellItem.ApplyViewItem` — **timeout
  2,5s** vì sạp trang trí SimCity có cờ mà không có người, server im lặng;
  ② hàng sạp được client dựng lại vào `Item[]` **kèm dòng ma thật** ⇒ lọc thẳng bằng
  `DT_MatchRule` theo mọi ứng viên; chọn món **RẺ NHẤT** ≤ trần "Giá sạp tối đa (vạn)" (ô mới,
  mặc định 200); thiếu tiền thì rút từ rương theo cấu hình cũ;
  ③ mua: **tự tính ô trống hợp lệ** trước khi gửi (`SearchPosition`, bắt buộc vì server bỏ qua
  lỗi đặt — sai ô là MẤT TIỀN + MẤT MÓN, KProtocolProcess.cpp:6073), gửi
  `SendClientCmdPlayerBuy(server-idx, dwID người bán, ...)`; không có gói ACK — xác nhận bằng
  item xuất hiện trong túi rồi về NPC trả;
  ④ hết sạp gần → đi tuần các điểm tụ tập trong thành (trung tâm/tạp hóa/Xa Phu/Dã Tẩu, dừng
  2,5s mỗi điểm cho sạp sync); hết thành → `DTP_CITYHOP`: ra Xa Phu, menu 1 chọn "Những thành
  thị đã đi qua", menu 2 chọn dòng mang tên thành đích (bảng 10 tên TCVN3 khớp từng byte
  `Station.txt`: Phượng Tường, Thành Đô, Đại Lý, Biện Kinh, Tương Dương, Dương Châu, Lâm An,
  Giang Tân, Long Môn, **Ba Lăng huyện**), server thu 10-50 lượng rồi chở; 150 giây/lượt nhảy,
  25 phút cho cả cuộc đi chợ; đi đủ 10 thành/thôn không có → hủy/treo theo ô "Khi bỏ qua".

### 13.11 UI tab Dã Tẩu (WAuto.exe 21:35)
- 6 ô loại nhiệm vụ chuyển **2 cột × 74-76du** — hết cụt chữ ("Khoe vật phẩm" → **"Tìm trang
  bị"** theo yêu cầu; "Địa đồ chỉ" → "Địa đồ/Mật chỉ"); nhãn rương rút gọn cho vừa 152du.
- 3 điều khiển mới (đều có tooltip): "Dùng Lệnh bài hoàn thành khi loại tắt" (bật sẵn),
  "Thiếu đồ: mua ở sạp (tự qua 10 thành)" (bật sẵn), "Giá sạp tối đa (vạn)" (200).
- `autoData` +3 trường CUỐI (6876 → **6888 B**) — 2 bản `ipc_shared.h` D + WAuto **khớp**;
  bản cây E bỏ (cây chết). `.dat` cũ di trú **HAI BẬC** theo `offsetof`: file trước 18/08 mới
  đặt mặc định cả khối Dã Tẩu; file 18-19/08 chỉ đặt 3 trường mới — **không đè lựa chọn cũ**.

### 13.12 Phản biện đa-agent của đợt này (`838960a9`) — 8 lỗi thật đã sửa

Chạy 6 mũi soi độc lập + 14 vòng bác bỏ đối kháng trên commit `c9de7a7b` (20 phát hiện thô
→ **8 CONFIRMED**, 6 bị bác có dẫn chứng). Đã sửa hết trong `838960a9`, build + deploy lại
22:23:

| Mã | Lỗi thật | Cách sửa |
|---|---|---|
| V07 | Kéo phù/đồ từ rương khi **ngoài thành** bị server DROP im lặng (mọi click vào rương bị chặn lúc `m_FightMode=1` — KItemList.cpp:2250) ⇒ treo lặp vô hạn | `DT_PortalPull` trả 0 ngay khi fight-mode + thông báo nói rõ "phù ở rương không lấy được khi ngoài thành" (kéo từ rương chỉ chạy TRONG thành — vẫn phủ ca chuẩn bị loại 4) |
| V09 | Bán rác tại chỗ bỏ qua công tắc "Bán vật phẩm" ⇒ người tắt vẫn bị bán đồ | tắt `bSellItem` ⇒ không bán, treo với hướng dẫn bật ô hoặc tự dọn |
| V10 | Quét "nhặt được đồ" khi treo nhận cả đồ trong RƯƠNG ⇒ vòng HOLD↔EXEC vô hạn nếu mật khẩu rương sai | chỉ nhận đồ trong **túi** (`pos_equiproom`) |
| V11 | Giá trần sạp chỉ kiểm snapshot cũ — server tính giá **hiện tại**, người bán đổi giá là mua hớ | snapshot quá 3,5 giây phải `UpdateItem` làm mới rồi mới mua |
| V12 | Món bị người khác mua trước: không có ACK ⇒ spam lệnh mua y hệt tới 25 phút | trần 3 lần gửi mua/sạp rồi bỏ qua sạp |
| V15+V20 | Rút tiền rương: tick đầu `EnsureUnlock` false bị coi là "không đủ tiền" (bỏ oan sạp); rút quá số dư thì `ExchangeMoney` fail im lặng ⇒ livelock | chờ mở khóa xong; kẹp theo số dư rương; tối đa 3 lần/sạp |
| V19 | Lệnh bài dùng khi túi <5 ô: server từ chối bằng Talk client không đọc được ⇒ tưởng "script chưa cập nhật" | guard ≥5 ô trống trước khi dùng, thiếu thì đi bán rác trước |
| V13 | Cửa sổ thưởng đến đúng khe 900ms "đặt đồ → OK" của hộp giao ⇒ máy bị cướp pha, quên item trong hộp | catch-all `uFinSeq` nhường 1 nhịp khi đang ở bước đó |

Kèm 3 lỗi nhẹ: mốc ô-trống của SELLJUNK đổi sang `nDTShopTry` (trả `nDTItemIdx` về đúng nghĩa
cho lá chắn `DT_IsQuestItem`); hộp giao đóng giữa chừng thì xóa cờ "đã đặt item"; **di trú
`.dat` đổi `<` thành `<=`** — không sửa thì MỌI người dùng cũ (file 6876 B — đúng bằng
`offsetof(bDTLenhBai)`) bị 2 ô mới mặc định TẮT.

6 phát hiện bị BÁC (đã có chốt chặn sẵn — khỏi sửa): mua lặp sau 3s lock (server `GetPrice==0`
chặn); click swap ở ô (0,0) hộp giao (có đường thu hồi `GOI_RECOVERY_BOX_COMMAND`); catch-all
thưởng nổ giữa MUASAP (GOTONPC/WAITDLG đã tiêu thụ fin trước đó — TCP có thứ tự); bán đồ khi
chết ×2 (server chặn bán lúc fight-mode; `PushReviveButton` chạy TRƯỚC mọi pha, không bị Dã Tẩu
chặn); ExtAuto thiếu init `nDTLBTry` (mọi trường DT đều dựa zero-init + ATYPE_CLEAR, nhất quán).

### 13.13 VÒNG 3 (20/08 rạng sáng, `a3720c97` + `643a6cfd`) — 5 lỗi test thật + 8 lỗi phản biện

**5 lỗi người dùng báo sau khi test, nguyên nhân thật → cách sửa:**

| Lỗi báo | Nguyên nhân thật | Sửa |
|---|---|---|
| Đủ 40 đứng yên, không lên lại map treo | Máy giữ `engaged=1` khi ô "lên map luyện công" tắt (mặc định tắt) **và** máy Hậu cần còn kẹt ở trạng thái "đã xong" từ chuyến trước (`nHomeStep` chỉ reset khi fight-mode lật 1→0) nên chẳng ai đưa nhân vật đi | Đủ 40 → **nhả máy hoàn toàn** + đánh thức Hậu cần từ đầu (bán rác/mua đồ/đi trạm/lên map theo tab Hậu cần); mọi hold khác cũng nhả máy |
| Phúc duyên không lấy bình Đại từ rương | Cổng rương đòi **mật khẩu trong WAuto** — rương không đặt mật khẩu thì ô này trống ⇒ auto bỏ qua rương dù rương đang MỞ | Mọi cổng kéo rương chấp nhận `m_CUnlocked` (rương đang mở); rương khóa + không mật khẩu + CÓ bình → nhắc thẳng "nhập mật khẩu rương ở tab Hậu cần" |
| Chưa tới từng sạp | (chẩn đoán tại trận) | Bỏ lọc kind (cứ treo biển `m_BaiTan` là tới xem, timeout 2,5s lo sạp câm), báo `Xem sạp "tên"...` khi mở, và thống kê `Thành này: xem X sạp (Y không phản hồi)` khi rời thành — chạy lần tới sẽ biết ngay kẹt ở đâu |
| Phù về không bán rác liền | `DTP_RETURN` về thành là đi thẳng NPC/Xa Phu | Vừa đặt chân về thành + bật "Bán vật phẩm" + túi <10 ô trống → **bán rác trước**; `DT_SellResume` đưa về đúng mạch sau khi bán (đang trả đồ → NPC, cần đi lại map → Xa Phu) |
| Đôi khi đủ mảnh đồ chí mà không phù về | Bộ chụp tin "Hệ Thống" chỉ có **1 khe** — 2 tin đến trong 1 nhịp 250ms là mất tin đầu; tin CHỐT "tổng cộng N tấm" bị đè là hết (server **ngưng thả mảnh** khi đủ). Nguồn tin: `script\item\tasklink_goods(.secret).lua` | **Kênh riêng cho tin tiến độ** (`szTien`, hook lọc "tổng cộng" — spam thông báo toàn server không đè được) + vòng 4 khe chung + đọc hết tin trước khi rời FARM + lưới: đứng ở (cần−1) quá 4 phút → về thử trả MỘT lần; trả sớm trượt → loại 4 quay lại map đánh tiếp (không skip) |

**Phản biện đợt này (6+4 mũi soi, 8+8 vòng bác bỏ) xác nhận và đã sửa thêm 8 lỗi** (chi tiết
commit `643a6cfd`): probe đủ-40 kiểm vị trí mỗi 60 GIÂY thay vì 60 phút (không thì cơ chế hỏi-lại
gần như không bao giờ trúng lúc ở thành — lệch múi giờ server là mất nhiều giờ); clamp vòng khe
lệch-1 (4 khe chỉ giữ được 3); FARM đọc tin trước khi thoát vì đổi map + EXEC-4 không xóa trắng
tiến độ khi quay lại cùng nhiệm vụ; LIMIT không đè trạng thái giữa-chuyến của Hậu cần (rút tiền
lặp); Hậu cần bước 2 không còn spam mở khóa 300ms/lần khi không có mật khẩu (đứng im vĩnh viễn);
`DT_Hold` xóa cờ `nDTBackXaFu` (rò cờ làm chuyến trả sau bị chở ngược ra map); nắp trần vòng mở
khóa ở USEPD/chuẩn-bị-phù (mật khẩu sai không đứng rực). Lưu ý đã xác minh: mật khẩu rương phải
là **6 chữ số không bắt đầu bằng 0** (server `CheckChestPW` bác mọi giá trị < 100000).

### 13.14 — Vòng 4+5 (20/08 sáng): loại 3 đi chợ, 4 fix, Thần Hành Phù

**A. Loại 3 "Tìm trang bị (khoe)" cũng đi chợ mua ở sạp** (người dùng: *"khi tới nhiệm vụ tìm
trang bị thì nó báo không có trang bị cần tìm (khoe) phù hợp trong túi / rương nó chạy lên bãi
treo luôn"*). Trước đây DTP_MUASAP chỉ gắn cho loại 2; loại 3 rơi thẳng xuống DT_Skip/treo.
Cách hoạt động sau sửa:
- `DTP_EXEC` loại 3 không thấy đồ trong túi/rương **và** ô "Mua ở sạp" bật → reset bộ đếm
  `g_nDTSap*` → vào `DTP_MUASAP` (trần thời gian đi chợ 25 phút giữ nguyên). Tắt ô thì giữ
  hành vi cũ (báo cam + treo/hủy theo cấu hình).
- Trong `DTP_MUASAP`, bộ lọc món hàng sạp phân nhánh theo `ea.nDTQType`: loại 3 dùng
  `g_DTShow[cand]` — khớp **chỉ theo dòng ma trong khoảng** (`DT_MatchRule(nIt,-1,-1,-1,-1,-1,
  nMagic,nMin,nMax)`), đúng luật server `Task_Accept_03`. Loại 2 giữ `g_DTFind` như cũ.
- Toàn bộ máy đi chợ (tour waypoint, xem từng sạp, thống kê "xem X sạp", nhảy 10 thành/thôn,
  chỉ treo sau khi HẾT thành) dùng chung — "tìm hết các thành thị thôn mà không có mới đi treo".
- Đồ khoe trả xong được server HOÀN LẠI (khác loại 2 bị thu) → mua càng lợi.

**B. 4 fix (2 phản biện + 2 người dùng báo khi test thật):**
1. *(phản biện, nặng)* `DT_MatchRule` quét đủ 8 ô dòng ma nhưng server `seasonnpc.lua` chỉ kiểm
   `for i=1,6` (cả loại 2 lẫn 3) — ô 7/8 là dòng ẩn hoàng kim/khảm. Món chỉ khớp ở ô 7/8 sẽ:
   client mua (mất tiền) → trả trượt → HOLD-scan lại thấy → kẹt vĩnh viễn. **Sửa: kẹp vòng quét
   6 ô** ngay trong DT_MatchRule → đồng bộ mọi nơi (mua sạp, quét túi/rương, bảo vệ bán, HOLD-scan).
2. *(phản biện)* Trần mua mặc định 200 vạn quá cao cho đồ khoe (đồ trắng/xanh có dòng phổ biến —
   dễ bị sạp "mồi" hút máu). **Sửa: mặc định theo loại — loại 3 = 30 vạn, loại khác 200 vạn**;
   người dùng đặt tay ô "trần mua" thì luôn theo người dùng. Khuyên: đặt trần thấp nếu server
   có sạp giá đểu.
3. *(người dùng: "lúc tìm người bày bán thì tìm nhầm vào npc chức năng ở thành")* — vòng 3 đã bỏ
   lọc kind khi quét sạp; hoá ra slot NPC tái dùng có thể SÓT cờ `m_BaiTan` cũ → NPC thoại/chức
   năng bị coi là sạp. **Sửa: lọc lại `m_Kind == kind_player`** (người thật + bot sạp đều là
   player; test thật đã xác nhận mua được ở sạp nên lọc này không bỏ sót sạp thật).
4. *(người dùng: "lúc mua được item nhiệm vụ rồi lúc giao item vào box giao nhiệm vụ thì sẽ tìm
   mua thêm vì WAuto xác định không có item nhiệm vụ trong rương nên tìm mua tiếp")* — cơ chế:
   khi trả TRƯỢT, server chỉ `Say` lỗi mà **không đóng hộp giao** → item nằm kẹt ở
   `pos_affairitem`, mọi bộ quét túi/rương không thấy → auto tưởng mất đồ, đi mua nữa (tiền chảy
   máu, hộp phồng). **Sửa: helper `DT_ThuHoiBox()`** — duyệt `m_ItemList`, mỗi item ở
   `pos_affairitem` gửi `SendClientRecoveryBox(id,w,h)` (đúng gói nút **Hủy** thật gửi — nhái
   case `GOI_RECOVERY_BOX_COMMAND`); gọi ở ĐẦU nhánh FAILREQ nên phủ mọi lối ra (loại 4 về map,
   phúc duyên dùng thêm, xoay ứng viên, skip). Item về túi → vòng sau EXEC/MUASAP thấy ngay,
   tự thử trả tiếp thay vì mua trùng.

**C. Thần Hành Phù thay Xa Phu khi nhảy thành** (người dùng: *"lúc di chuyển thành thị hoặc thôn
thì mở thần hành phù lên chọn những thành thị thôn có sẵn ở trong thần hành phù không mất công
chạy tới xa phu cho xa"*). Cách hoạt động:
- Item = **(6,1,1271)**, script server `script\item\ib\shenxingfu.lua`. KHÔNG bị tiêu hao khi
  dùng, dịch chuyển **miễn phí** (Xa Phu thu [N lượng]).
- Vào lượt nhảy thành mới (`g_uDTSapHopT` khởi tạo), engine thử `AutoUseItem(6,1,1271)`:
  có phù → chờ chuỗi thoại; không có → `g_uDTThpT=1` đi Xa Phu như cũ.
- Chuỗi thoại THP đi qua **cùng bộ bắt thoại** của CITYHOP, thêm 2 needle:
  1. menu chính: dòng chứa `"thuật thần hành"` (TCVN3 `thu\313t th\307n h\265nh`) → chọn;
  2. menu "chọn địa điểm": `"Thành thị"` hoặc `"Thôn trang"` theo đích
     (thôn = map 20 Giang Tân / 121 Long Môn / 53 Ba Lăng);
  3. danh sách tên thành **và** menu khu (Trung Tâm/Đông/Tây...) đều được needle `szTen`
     (bảng `g_aDTSapTown`) khớp sẵn — mọi bảng thành trong shenxingfu liệt kê "Trung Tâm"
     **hàng đầu** nên tự vào khu trung tâm (Lâm An không có Trung Tâm → vào hàng đầu "Lâm An
     Nam"); thôn dịch chuyển thẳng không có menu khu.
- 12 giây chưa đổi map (server chặn/quá cấp/thiếu item lúc chạy script) → nhả xuống đường Xa Phu.
  Tới nơi hoặc đổi thành đích thì `g_uDTThpT` reset để lượt sau thử phù lại.
- Needle Ba Lăng rút còn `"Ba L\250ng"` vì bảng THP viết "Ba Lăng **H**uyện" (H hoa) còn menu
  Xa Phu viết "huyện" thường — needle ngắn khớp cả hai.
- Phủ sóng: đủ 10/10 thành-thôn của tour nằm trong THANH_ARRAY/THON_ARRAY của shenxingfu.

**D. Phản biện r5b (sau deploy 10:11) — 4 CONFIRMED, vá tiếp trong 10:44:**
1. *(nặng)* `DT_ThuHoiBox` bản đầu bắn **loạt** lệnh thu hồi không kiểm chỗ trống: server
   `KPlayer::RecoveryBox` khi túi thiếu chỗ nhét món vào **pos_hand**, và lệnh thu hồi kế tiếp
   làm món đang cầm bị **ném xuống đất = mất vĩnh viễn** (KPlayer.cpp:6466-6512). Sửa: mỗi lần
   gọi chỉ thu **1 món**, client tự `CheckCanPlaceInEquipment` trước khi gửi; thiếu chỗ → trả −1,
   **giữ món trong hộp** (an toàn, hộp là kho tạm) + báo vàng "túi thiếu chỗ - sẽ tự lấy sau
   khi dọn túi"; món còn lại thu ở lần FAILREQ sau.
2. Món ở pos_hand tàng hình với `DT_FindItemRule` → fix cũ tự vô hiệu đúng lúc túi chật —
   giải quyết cùng gốc bằng (1): không bao giờ đẩy món vào pos_hand nữa.
3. Needle map 11 "Thành Đô **Phủ**": menu khu của THP chỉ ghi "Thành Đô Trung Tâm/Đông/…"
   (không có " Phủ") → strstr trượt → kẹt 12s mỗi chuyến tới Thành Đô rồi mới rơi xuống Xa Phu.
   Sửa: rút needle còn `"Th\265nh \247\253"` (vẫn khớp duy nhất ở city-list, menu khu và
   Station.txt của Xa Phu).
4. Chú thích sai về Lâm An: bảng THP của Lâm An **không có** "Trung Tâm" (chỉ Nam/Đông/Bắc) —
   hành vi hiện tại đúng nhờ szTen khớp hàng đầu "Lâm An Nam". Đã sửa chú thích kèm cảnh báo
   **cấm** đổi cơ chế sang khớp needle "Trung Tâm" (Lâm An sẽ trượt hết và kẹt 12s).

Ghi chú môi trường (từ phản biện): server đang chạy CHƯA restart nên bản `seasonnpc.lua` mới
(có EndGiveBox — phiên bot thêm 20/08) chưa hiệu lực → loại 3 trả THÀNH CÔNG trên server cũ
vẫn để món khoe kẹt trong hộp giao; engine sẽ nhặt lại dần qua các lần FAILREQ (mỗi lần 1 món).
Restart GameServer là hết cảnh này.

Binary chốt vòng 4+5: **CoreClient.dll 20/08 10:44 (2.201.600 B)**, bản lùi `_locked7` (10:11).

**E. r5c (12:08) — "vẫn tới NPC chức năng" trên bản MỚI → gốc thật là dân SimCity:**
`KSimCity.cpp:545` dân giả lập là **kind_player mang ngoại hình người chơi** và được bật
`m_BaiTan=1` + biển hiệu (sạp trang trí) — đồng bộ xuống client qua PLAYER_SYNC y như người
thật ⇒ **không bộ lọc kind nào phân biệt nổi**; biển hiệu cũng vô dụng (bot PB bốc biển từ
CÙNG `settings/simcity/stall_adv.txt`). Cách phân biệt DUY NHẤT: gói hỏi số món
`c2s_playerneedcount` — server tra `FindAroundPlayer` (chỉ thấy PLAYER thật) nên **sạp giả im
lặng, sạp thật (người + bot PB) trả lời**. Fix: máy **thăm dò trước khi đi** trong khối chọn
sạp — đứng tại chỗ gửi hỏi (tầm 3×3 region, xa hơn thì đi lại gần rồi hỏi), 2 lần im lặng
1,3s = sạp giả → đánh dấu bỏ + báo xám `Bỏ qua sạp trang trí "tên"`, trả lời 0 món = sạp thật
hết hàng → bỏ không mở, trả lời >0 → đi tới mở xem như cũ. Bắt phản hồi qua 3 trường mới
`uCntSeq/dwCntId/nCnt` trong `KDaTauCap.h` + hook `s2cGetCouunt` (KProtocolProcess.cpp).
Binary chốt: **CoreClient.dll 20/08 12:08 (2.203.648 B)**. Đã audit 12/12 marker mọi vòng +
font 8/8 chuỗi mới chuẩn TCVN3 trong binary này.

**F. r5d (12:4x) — "vẫn chạy tới NPC chức năng" lần 2 → 2 nguồn còn lại (đã soi cổng điều phối):**
Cổng S3Client.cpp:993/1069: MOVE + Hậu cần **chỉ chạy khi DT trả 0** — trong lúc đi chợ DT trả 1
nên chúng bị khóa hoàn toàn (chỉ khi treo/hold thì Hậu cần đi tạp hóa/dược/rương là ĐÚNG thiết kế).
Hai nguồn thật: (1) **cửa sổ thưởng THỨ 2 tới trễ** (thiết kế 2-cửa-sổ Dã Tẩu) cướp pha giữa lúc
đi chợ → REWARD → thoát ra GOTONPC = chạy về NPC Dã Tẩu + làm lại tour từ đầu MỖI nhiệm vụ. Fix:
trường mới `ExtAuto.nDTPhaseBack` — catch-all nhớ pha MUASAP/CITYHOP, REWARD bấm xong **quay lại
đúng pha đang dở, GIỮ nhiệm vụ + tour** (xóa cờ ở DT_Hold/DT_Skip/ParseQuest chống rò). (2) tour
`DT_SapWaypoint` vốn đi tuần **trung tâm + 2 tiệm + Xa Phu + chỗ Dã Tẩu** (sạp tụ quanh chợ) nhưng
đi LẶNG LẼ nhìn như chạy nhầm → nay báo xám `Đi tuần điểm tụ tập N/M tìm sạp quanh đó...` mỗi chặng.
Binary: **CoreClient.dll 20/08 12:4x (xem bảng)**.

**G. r5e (17:29) — DANH BẠ SẠP toàn map** (người dùng: *"chỉ cần ở cùng map là biết ngay vị trí
sạp ở đâu, tránh trường hợp người chơi bày sạp ở vị trí khác thì không tìm ra"*):
- Client hỏi bằng **gói needcount CŨ với `dwId = 0x0DA75AB1`** (không thêm packet — Gate 2 an
  toàn; giá trị không thể trùng dwID thật). Server (`c2sNeedCount`) duyệt Player cùng subworld có
  `m_BaiTan` → trả `"[SapMap] id:x:y ..."` (tọa độ CELL, tối đa 12 sạp) qua
  `mgs2player_from_c.lua` → tin Hệ Thống riêng người hỏi. **Chỉ liệt kê sạp có PLAYER thật đứng
  sau** (người chơi + bot PB) — dân SimCity (KNpc) tự bị loại ngay từ nguồn.
- Client: hook chat bắt `[SapMap]` vào kênh riêng (`szSapMap/uSapMapSeq` trong KDaTauCap — không
  vào vòng 4 khe, không đè tin tiến độ) và **chặn không hiện lên khung chat**.
- MUASAP: mỗi map hỏi 1 lần (90s hỏi lại) → có danh bạ thì **chạy thẳng tới từng sạp theo tọa
  độ** (kể cả chỗ lạ), tới gần 300 MPS mà sạp đã dọn thì đánh dấu bỏ; **hết danh bạ / server xác
  nhận 0 sạp = qua thành kế luôn, khỏi đi tuần mù**. Server CŨ chưa restart: im lặng 1,8s → rơi
  về đi tuần waypoint như trước (tương thích ngược).
- Thông báo: cyan `Server báo N sạp trong thành - chạy thẳng tới từng sạp.` / xám `Thành này
  không có sạp thật nào (server xác nhận) - qua thành kế.`
- **CẦN RESTART GameServer** (CoreServer.dll 17:29 đã nằm bin\server, bản lùi `_cu_1644` của
  phiên bot) — trước restart client vẫn chạy kiểu đi tuần cũ.

**H. r5f (18:01) — phản biện r5e: 5 lỗi CONFIRMED đã vá** (2 agent soi độc lập + 5 agent bác bỏ,
1 phát hiện bị bác nhưng lòi ra lỗi nặng hơn):
1. *(nặng — tính năng chính chết ở thành trống)* Thành **không có sạp nào** thì server trả đúng
   chuỗi `"[SapMap]"` = **8 byte**, nhưng client bắt bằng `nDTLen > 8` → gói bị vứt, cờ danh bạ
   không nhảy → nhánh "0 sạp → qua thành kế" và câu báo xám là **mã chết**, bot vẫn đi tuần mù
   mọi thành trống; tin còn chiếm 1 khe vòng tin tiến độ. Sửa: `>= 8`, và đổi `strncmp` →
   `memcmp` (payload **không kết thúc NUL**).
2. *(nặng — cả danh bạ bị vứt im lặng)* `sentlen` trên đường dây là **BYTE**, còn
   `KPlayerChat::SendSystemInfo` kẹp đúng `MAX_SENTENCE_LENGTH = 256` → **256 tràn về 0** ⇒ mọi
   danh bạ ≥256 byte (12 sạp + dwID dài) mất trắng. Sửa: server kẹp chuỗi ở
   `DATAU_SAPMAP_MAXLEN = 200`.
3. *(nguyên tắc)* Sentinel `0x0DA75AB1` nằm **trong** không gian `m_dwIDCreator` (KNpcSet.cpp:26
   đếm đơn từ 1000, không wrap, không reset ngoài constructor) → NPC/sạp thật thứ ~229 triệu sẽ
   mang đúng id đó và bị gói xin danh bạ cướp mất (khi đó bot coi sạp thật là "sạp trang trí" và
   bỏ qua). Sửa: hằng số chung `DATAU_SAPMAP_ID` (KDaTauCap.h) + `KNpcSet::SetID` **bỏ qua đúng
   một giá trị** khi cấp id ⇒ đóng gói hoàn toàn khả năng va chạm.
4. *(chống spam)* Nhánh sentinel chạy cho **mọi** client, mỗi gói quét `MAX_PLAYER` + gọi Lua —
   nặng hơn hẳn đường cũ. Sửa: cooldown **5 giây/người** (`static DWORD s_uSapDsNext[MAX_PLAYER]`,
   mốc `SubWorld[0].m_dwCurrentTime`, `GAME_FPS*5`); bot tự hỏi 90 giây/lần nên không ảnh hưởng.
5. *(mất gói = mù vĩnh viễn)* `g_nDTSapDs == -1` là trạng thái chết: mất trả lời (server vừa
   restart, gói rớt) thì **cả hai vế điều kiện hỏi lại đều sai** → không bao giờ hỏi lại trên map
   đó. Sửa: `g_nDTSapDsTry` — hỏi lại tối đa **3 lần**, làm mới bộ đếm sau 30 giây.
6. *(ảnh hưởng người chơi thật)* Dòng chặn hiển thị `[SapMap]` nằm **ngoài** khối "Hệ Thống" nên
   áp cho **mọi** kênh chat: người chơi gõ tin bắt đầu bằng `[SapMap]` bị **nuốt với tất cả**
   client chạy bản r5e. Sửa: cờ `bDTSapMap` chỉ bật bên trong khối Hệ Thống khi đúng là gói danh
   bạ; chặn theo cờ.

Binary: **CoreClient.dll 20/08 18:01 (2.205.696 B)** + **CoreServer.dll x64 18:01** (bản lùi
`CoreClient_locked12` / `CoreServer_cu_r5e`). **VẪN CẦN RESTART GameServer** để danh bạ sạp sống.

**I. r5g (18:18) — phản biện vòng 2 (soi chính bản vá r5f): 5 lỗi CONFIRMED, 0 bị bác.**
Toàn bộ nằm ở cơ chế "hỏi lại" vừa thêm — nó **xung khắc với chính cooldown 5 giây trong cùng
bản vá**:
1. *(hồi quy do r5f gây ra)* Nhánh hỏi-lại chạy **trước** khối phân tích và ghi đè
   `g_uDTSapDsSeen` vô điều kiện ⇒ trả lời về **muộn hơn 1,8 giây bị vứt vĩnh viễn** (bộ đệm đơn,
   seq đã bị đánh dấu đã đọc). Sửa: chỉ hỏi lại khi `uSapMapSeq == g_uDTSapDsSeen` (không còn
   trả lời chưa đọc).
2. Nhịp hỏi lại 1,8 s **nằm gọn trong** cooldown 5 giây của server ⇒ mọi lần hỏi lại bị `return`
   im lặng, cơ chế hỏi lại vô dụng đúng trong tình huống nó sinh ra để cứu. Sửa: lần đầu 1,8 s,
   các lần sau **6 s**.
3. `g_nDTSapDsTry` tăng cả ở nhánh **làm mới 90 giây** và không reset khi nhận hồi âm ⇒ ở một
   thành >3 phút là **cạn sạch ngân sách** hỏi lại. Sửa: reset khi đổi map / khi đã có danh bạ /
   khi nhận hồi âm — bộ đếm chỉ đếm lần hỏi **liên tiếp không hồi âm**.
4. Bot **đứng im 5,4 giây mỗi chu kỳ** (3×1,8 s) thay vì 1,8 s như r5e. Sửa: chỉ đứng chờ ở lần
   hỏi đầu; các lần sau **vừa đi tuần vừa chờ**.
5. *(server)* `s_uSapDsNext` không xóa khi **khe người chơi bị tái sử dụng**, và cooldown theo
   **người** chứ không theo **map** ⇒ người mới thừa kế cooldown của chủ cũ; bot vừa đổi thành
   (hỏi ngay) có thể bị vứt gói đầu tiên. Sửa: theo dõi chủ khe (`m_dwID`) + subworld, đổi là xóa.

Binary: **CoreClient.dll 20/08 18:18 (2.205.696 B)** + **CoreServer.dll x64 18:18** (bản lùi
`CoreClient_locked13` / `CoreServer_cu_r5f`). Vẫn **CẦN RESTART GameServer**.

**K. r5h (18:43) — phản biện vòng 3: 5 CONFIRMED nữa, 1 bị bác.** Hai lỗi nặng đúng vào mục
tiêu "khỏi lòng vòng":
1. *(nặng — kẹt cả thành)* Vòng đi tới từng mục danh bạ **không có hạn giờ**: `DT_WalkTo` không
   phát hiện kẹt (chỉ phát lại đường mỗi 2,5 s rồi trả 0 mãi), mà `FindPath` trả -1 ngay ở **map
   không có lưới đường** ⇒ bot đứng im, ngốn trọn 25 phút của cả thành. (Nhánh đi tuần cũ vốn
   có hạn 45 s — nhánh danh bạ bị bỏ mất.) Sửa: **hạn 45 giây/mục**, hết giờ thì ghi "đã xem"
   (không chỉ tăng con trỏ — vì làm mới 90 s sẽ đặt lại con trỏ) + báo xám rồi đi sạp kế.
2. *(giảm độ phủ)* Danh bạ **tắt hẳn** đường đi tuần, mà server chỉ trả **tối đa 12 sạp** ⇒ thành
   đông bị tuyên bố "xong" sau 12 sạp — kém hơn cả bản trước khi có danh bạ. Sửa: danh bạ đi
   trước (địa chỉ chính xác), **hết danh bạ thì tour điểm tụ tập quét nốt**, hết cả hai mới qua
   thành kế; kèm **con trỏ quét xoay vòng phía server** để lượt làm mới sau trả lô 12 sạp **khác**.
3. `6000 ms` giãn nhịp đặt **nhầm chỗ**: nó chỉ chi phối khoảng cách lần 2→3, còn lần 1→2 vẫn là
   1,8 s ⇒ vẫn nằm trong cooldown 5 giây, đốt oan một lượt. Sửa: điều kiện hỏi lại thêm
   `uCurTime > g_uDTSapDsFresh + 5200`, hạn chờ trả về 1,8 s cho mọi lần.
4. Đi tuần chạy trong lúc chờ làm `g_uDTSapWptT`/`g_uDTSapDwell` (**mốc tuyệt đối**) treo quá
   hạn ⇒ lần tuần sau **đốt thẳng** điểm tụ tập đầu tiên mà chưa hề đi tới. Sửa: xóa hai mốc khi
   vào chế độ danh bạ.
5. Biến static danh bạ **không nằm trong khối reset** khi vào `DTP_MUASAP` (static sống qua cả
   chuyến/đổi nhân vật) ⇒ chuyến đi chợ mới vào lại đúng thành cũ sẽ **bỏ qua nguyên thành**.
   Sửa: hàm `DT_SapDsReset()` gọi ở cả hai lối vào pha (`g_nDTSapDsMap` khởi tạo −1; **nuốt seq
   hiện tại** để không phân tích lại gói sót của chuyến trước).
*(Bị bác: "bỏ đứng-im ở lần hỏi lại làm bot bỏ thành vĩnh viễn" — city-hop chỉ chạm được ở 10 map
có NPC Dã Tẩu nên kịch bản không xảy ra.)*

Binary: **CoreClient.dll 20/08 18:43 (2.205.696 B)** + **CoreServer.dll x64 18:41**.

**L. r5i (19:14) — phản biện vòng 4: 3 CONFIRMED, đều là hồi quy do chính r5h sinh ra**
*(vòng bác bỏ bị cắt vì hết hạn mức phiên; cả 3 đã được xác minh thủ công trên mã thật)*:
1. *(nặng nhất)* Hai lệnh xóa mốc tuần `g_uDTSapWptT = 0; g_uDTSapDwell = 0;` nằm **ngoài**
   nhánh "đang đi theo danh bạ" ⇒ khi danh bạ **đã cạn**, mỗi nhịp đều xóa rồi gọi
   `DT_SapWaypoint`: hạn 45 s nạp lại liên tục và `dwell` luôn 0 ⇒ **bot kẹt vĩnh viễn ở điểm
   tuần đầu tiên**, không bao giờ sang thành kế, **và spam chat ~3 dòng/giây**. Sửa: chỉ xóa khi
   thực sự đang đi theo danh bạ.
2. Hạn 45 s/mục **không nạp lại** khi mục trước hoàn tất qua đường "sạp thật" (vòng quét gần xử
   lý xong mất 15-40 s) ⇒ mục kế thừa hạn đã cạn, **bị bỏ oan khi chưa đi một bước** — hỏng xen
   kẽ, mất tới nửa danh bạ. Sửa: theo dõi mục đang tính giờ (`g_nDTSapDsItemIdx`), đổi mục là
   nạp lại hạn.
3. Hết giờ ghi vào **đúng danh sách "đã xem"** mà vòng quét gần dùng để lọc ⇒ sạp không tới được
   bằng đường dài **mù luôn với quét cận cảnh** (bot đứng sát bên sạp còn hàng vẫn không mở xem).
   Sửa: danh sách riêng `g_aDTSapXa[16]` chỉ chặn đường danh bạ, quét gần vẫn thấy bình thường.

**Kiến thức build (đã kiểm)**: `Src\CoreShell.cpp` bị `ExcludedFromBuild` ở **mọi cấu hình
Server** ⇒ sửa riêng CoreShell **không cần** dựng lại CoreServer. Bản server hiện tại (18:41,
r5h) đã đủ.

Binary: **CoreClient.dll 20/08 19:14 (2.206.208 B)** + CoreServer.dll x64 18:41. Vẫn **CẦN
RESTART GameServer** cho tính năng danh bạ.

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
| Đặt lại số nhiệm vụ hôm nay | bỏ trần 40/ngày | chỉ `2420=0` (⚠️ ăn lại mốc 30; **cố ý không đụng `2797`** để người đã hủy vẫn không ăn lại mốc 40) |
| Chọn nhân vật khác | sửa cho người chơi đang online, không phải chỉ cho GM | `FindPlayer` + `SetTaskTemp(TMP_INDEX_PLAYER)` rồi đổi `PlayerIndex`, xong **trả lại** |

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

🔴 **Đo đệm menu bằng CHUỖI LÚC CHẠY, không phải độ dài mã**: bản đầu của menu này tính ra
~648 byte (tiêu đề + 7 nhãn) — **quá 512 nên sẽ bị cắt mất mục cuối mà không báo lỗi**; đã rút
gọn còn 448/512. `ReverseTools`-style bộ kiểm trước khi deploy nằm ở scratchpad, kiểm 5 điều:
cân bằng `function`/`end`, mọi hàm đổi `PlayerIndex` đều trả lại, nhãn không có `|`, tên hàm gọi
lại ≤31 ký tự, độ dài menu <490.

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
# Mọi thông báo trạng thái (từ e40f40ea là octal TCVN3 + thẻ màu)
grep -o '"<color=[^"]*"' D:/GAMEDEVNEW/Sources/Core/Src/CoreShell.cpp | sort -u
# Kiểm binary đã triển khai có mã mới chưa (bản mới: '[DaTau]' ASCII chỉ còn 1 - ở g_DebugLog;
# đếm '<color=' phải ra hàng trăm)
grep -c "<color=" E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/client/CoreClient.dll
```
