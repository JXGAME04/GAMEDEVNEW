# BÀN GIAO PHIÊN 20/08 — AUTO DÃ TẨU (WAuto): loại 3 đi chợ → Thần Hành Phù → DANH BẠ SẠP

> **ĐỌC FILE NÀY TRƯỚC KHI GÕ.** Chi tiết cơ chế từng tính năng nằm ở
> `BANGIAO_AUTO_DATAU_WAUTO.md` **mục 13.14, phần A → L**. File này là bản tổng hợp phiên:
> trạng thái, việc đã làm, cạm bẫy, việc còn nợ, checklist test.

---

## 0. TL;DR — trạng thái lúc bàn giao (20/08 ~19:20)

| Việc | Trạng thái |
|---|---|
| Loại 3 "Tìm trang bị (khoe)" đi chợ mua ở sạp | ✅ xong, chưa test thật |
| Thần Hành Phù thay Xa Phu khi nhảy thành | ✅ xong, chưa test thật |
| Thăm dò sạp (lọc dân SimCity giả sạp) | ✅ xong, chưa test thật |
| **DANH BẠ SẠP toàn map** (hỏi server vị trí mọi sạp) | ✅ xong, **CHƯA CHẠY ĐƯỢC** — chờ restart GameServer |
| 4 vòng phản biện, 18 lỗi CONFIRMED | ✅ đã vá hết |
| Phản biện vòng 5 | ❌ **CHƯA CHẠY** (hết hạn mức phiên) |

### 🔴 HAI VIỆC PHẢI LÀM ĐẦU PHIÊN SAU

1. **Thoát game vào lại** — tiến trình `Game.exe` đang chạy từ **18:40**, tức vẫn nạp
   `CoreClient.dll` bản **18:43** trong RAM. Bản mới là **19:14**.
2. **Restart GameServer** — tiến trình đang chạy từ **18:33**, trước khi `CoreServer.dll`
   **18:41** được chép vào `bin\server`. **Chưa restart thì danh bạ sạp KHÔNG chạy** (client
   hỏi, không ai trả lời → tự rơi về kiểu đi tuần cũ, vẫn dùng được nhưng chậm).
   Restart cũng kích hoạt luôn `lbhtdatau.lua` (lệnh bài hoàn thành) đã sửa từ 19/08.

---

## 1. Binary & deploy hiện tại

| Tệp | Vị trí | Mốc | Ghi chú |
|---|---|---|---|
| `CoreClient.dll` | `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\` | **20/08 19:14** (2.206.208 B) | tới commit `f3a65a84` (r5i) |
| `CoreServer.dll` | `...\bin\server\` | **20/08 18:41** (17.893.888 B) | r5h — phần server của danh bạ sạp |
| `Game.exe` | `...\bin\client\` | 20/08 03:49 | không đổi phiên này |
| `WAuto.exe` | `E:\Src_Auto_Ngoai\` **và** `...\bin\client\` | 19/08 22:23 (361.472 B) | không đổi phiên này, 2 nơi md5 trùng |
| `lbhtdatau.lua` | `...\bin\server\script\item\` | 20/08 10:04 | chờ restart mới có hiệu lực |

**Bản lùi còn giữ**: `CoreClient_locked16.dll` (18:43 = r5h), `CoreServer_cu_r5h.dll` (18:41),
`CoreServer_cu_1644.dll` (16:44 = bản phiên bot), `lbhtdatau_cu_1908.lua`.

**Cặp ipc**: `autoData` = **6888 byte**. `WAuto.exe` 22:23 + `Game.exe` 03:49 là cặp đúng —
**đừng thay một cái mà giữ cái kia bản cũ hơn 19/08 22:1x**.

---

## 2. Commit của phiên (theo thứ tự)

```
e04b8121  vòng 4+5: loại 3 đi chợ + 4 fix + Thần Hành Phù + phản biện r5b
00c6bb53  báo rõ khi không có Thần Hành Phù trong túi
4c399948  r5c: thăm dò sạp trước khi đi (hết chạy tới dân SimCity giả sạp)
4bc64d4f  r5d: cửa sổ thưởng trễ không cướp tour đi chợ + báo "đi tuần điểm tụ tập"
68e11346  r5e: DANH BẠ SẠP toàn map
836797e3  r5f: phản biện vòng 1 — 5 lỗi CONFIRMED
6fb81ef6  r5g: phản biện vòng 2 — cơ chế hỏi lại xung khắc với cooldown
edf1e8b1  r5h: phản biện vòng 3 — hạn giờ mỗi sạp + giữ độ phủ cả thành
f3a65a84  r5i: phản biện vòng 4 — 3 hồi quy do chính r5h sinh ra
```
Tất cả đã push. (Cùng ngày còn nhiều commit của **phiên song song** về bot/MySQL/S3Relay —
không liên quan auto Dã Tẩu, đừng lẫn.)

---

## 3. Tính năng đã thêm — cách hoạt động (tóm tắt; chi tiết ở 13.14)

### 3.1 Loại 3 "Tìm trang bị (khoe)" cũng đi chợ *(A)*
Túi/rương không có đồ **và** bật ô "Mua ở sạp" → vào `DTP_MUASAP` như loại 2 (trần đi chợ 25
phút). Bộ lọc hàng sạp phân nhánh theo loại: loại 3 dùng `g_DTShow` — khớp **chỉ theo dòng ma
trong khoảng** (đúng luật `Task_Accept_03` của server). Chỉ treo/hủy sau khi **hết 10 thành/thôn**.
Đồ khoe trả xong được server **hoàn lại** nên mua càng lợi. Trần mua mặc định loại 3 = **30 vạn**
(loại khác 200 vạn); đặt tay ô "trần mua" thì theo người dùng.

### 3.2 Thần Hành Phù thay Xa Phu *(C)*
Item **(6,1,1271)**, script `script\item\ib\shenxingfu.lua`, **không tiêu hao, miễn phí**.
Vào lượt nhảy thành: `AutoUseItem(6,1,1271)` → lái chuỗi thoại (needle "thuật thần hành" →
"Thành thị"/"Thôn trang" theo đích → tên thành → khu). 12 giây không đổi map → rơi xuống Xa Phu.
Ba thông báo phân biệt: dùng phù (cyan) / không có phù trong **túi** (xám) / phù không đưa đi
được (vàng).

### 3.3 Thăm dò sạp — lọc dân SimCity *(r5c)*
Dân SimCity là `kind_player` + `m_BaiTan=1` (sạp trang trí), biển hiệu bốc từ **cùng file** với
bot thật ⇒ lọc theo loại NPC/biển hiệu **vô dụng**. Phân biệt duy nhất: gói hỏi số món
`c2s_playerneedcount` — server tra `FindAroundPlayer`, **chỉ sạp có PLAYER thật mới trả lời**.
Bot **đứng tại chỗ thăm dò trước** (tầm 3×3 region), 2 lần im lặng 1,3 s = sạp giả → báo xám
"Bỏ qua sạp trang trí" và **không tốn bước chân nào**.

### 3.4 DANH BẠ SẠP toàn map *(G — tính năng chính của phiên)*
- Client hỏi bằng **chính gói needcount cũ** với `dwId = DATAU_SAPMAP_ID` (`0x0DA75AB1`) —
  **không thêm packet mới** (an toàn Gate 2).
- Server `c2sNeedCount` duyệt Player cùng subworld có `m_BaiTan` → trả
  `"[SapMap] id:x:y ..."` (tọa độ CELL, tối đa 12 mục, kẹp `DATAU_SAPMAP_MAXLEN = 200` byte)
  qua `mgs2player_from_c.lua` → `Msg2Player`.
- Client hook chat bắt `[SapMap]` vào kênh riêng `szSapMap/uSapMapSeq` (KHÔNG vào vòng 4 khe,
  **chặn không hiện khung chat** — chỉ chặn đúng gói của "Hệ Thống").
- `DTP_MUASAP`: mỗi map hỏi 1 lần (làm mới 90 s, hỏi lại tối đa 3 lần khi mất gói) → **chạy
  thẳng tới từng sạp theo tọa độ**, kể cả chỗ lạ. **Hạn 45 giây/mục**; hết giờ → ghi vào danh
  sách "không tới được" riêng + báo xám rồi đi sạp kế.
- **Hết danh bạ vẫn đi tuần** các điểm tụ tập để quét nốt (server chỉ trả 12 sạp/lô; **con trỏ
  quét xoay vòng** phía server để lô sau trả 12 sạp **khác**), hết cả hai mới sang thành kế.
- Server cũ (chưa restart) im lặng 1,8 s → **tự rơi về đi tuần như cũ** (tương thích ngược).

### 3.5 Hai fix phụ đáng nhớ
- *(r5d)* **Cửa sổ thưởng thứ 2 tới trễ** cướp pha giữa lúc đi chợ → bot chạy về NPC Dã Tẩu và
  **làm lại tour từ đầu mỗi nhiệm vụ**. Nay `ExtAuto.nDTPhaseBack` nhớ pha, bấm thưởng xong
  **quay lại đúng chỗ đang dở**.
- *(B4)* **Trả trượt server KHÔNG đóng hộp giao** → item kẹt ở `pos_affairitem`, mọi bộ quét
  không thấy → auto tưởng mất đồ và **đi mua thêm**. Nay `DT_ThuHoiBox()` gửi gói RECOVERY_BOX
  (đúng gói nút Hủy), **mỗi lần 1 món + tự kiểm chỗ trống trước**.

---

## 4. 🔴 CẠM BẪY ENGINE MỚI PHÁT HIỆN (kiến thức lâu dài — đọc kỹ)

| # | Cạm bẫy | Hệ quả nếu quên |
|---|---|---|
| 1 | **Server tasklink chỉ kiểm 6 Ô DÒNG MA đầu** (`for i=1,6` trong `seasonnpc.lua`), ô 7/8 là dòng ẩn hoàng kim/khảm | Client khớp 8 ô → mua món chỉ khớp ô 7/8 → **trả trượt vĩnh viễn + mất tiền** |
| 2 | **Trả nhiệm vụ TRƯỢT: server chỉ `Say` lỗi, KHÔNG đóng hộp giao** | Item kẹt `pos_affairitem`, tàng hình với mọi bộ quét |
| 3 | **`KPlayer::RecoveryBox` khi túi thiếu chỗ: nhét món vào `pos_hand` rồi lệnh kế NÉM món đang cầm XUỐNG ĐẤT** | Mất đồ vĩnh viễn |
| 4 | **Dân SimCity = `kind_player` + `m_BaiTan=1`** (sạp trang trí), biển hiệu trùng bot thật | Lọc kind/tên vô dụng — phải thăm dò bằng gói needcount |
| 5 | **`sentlen` của gói chat là BYTE**, còn `SendSystemInfo` kẹp đúng `MAX_SENTENCE_LENGTH = 256` ⇒ **256 tràn về 0** | Chuỗi ≥256 byte **mất trắng, im lặng** |
| 6 | Payload chat **KHÔNG kết thúc NUL** | `strncmp` đọc quá vùng — dùng `memcmp` sau khi chắc độ dài |
| 7 | **`m_dwIDCreator` (KNpcSet) đếm đơn từ 1000, không wrap, không reset ngoài constructor** | Sentinel in-band sẽ đụng id thật ⇒ phải **đặt chỗ** trong `KNpcSet::SetID` |
| 8 | **`Src\CoreShell.cpp` bị `ExcludedFromBuild` ở MỌI cấu hình Server** | Sửa riêng file này **không cần** dựng lại CoreServer (trước tưởng compile chung) |
| 9 | **`FindPath` trả −1 ở map không có lưới đường** (map 20/121) và `DT_WalkTo` **không phát hiện kẹt** | Bot đứng im vĩnh viễn — **mọi vòng đi tới toạ độ PHẢI có hạn giờ** |
| 10 | **Mốc thời gian TUYỆT ĐỐI** (`g_uDTSapWptT`, `g_uDTSapDwell`, `g_uDTSapDsItemT`) treo qua hạn khi chuyển chế độ | Bước kế bị **đốt** ngay nhịp đầu, hoặc kẹt vòng lặp |
| 11 | **Khe người chơi (`nIndex`) được TÁI SỬ DỤNG** khi có người thoát/vào | Mảng static theo khe phải theo dõi `m_dwID` chủ khe |
| 12 | `SubWorld[0].m_dwCurrentTime` là **số FRAME** (GAME_FPS=18), không phải ms | Tính cooldown sai 18 lần |
| 13 | Needle thoại phải **NGẮN**: bảng Thần Hành Phù viết "Ba Lăng **H**uyện" (H hoa), menu khu không có chữ " Phủ" | `strstr` trượt → kẹt 12 giây mỗi chuyến |
| 14 | Biến `static` trong DLL **sống qua cả chuyến đi chợ / đổi nhân vật** | Chuyến mới thừa hưởng trạng thái cũ → bỏ qua nguyên một thành |

---

## 5. Bốn vòng phản biện — 18 lỗi CONFIRMED (bài học)

| Vòng | Soi cái gì | Kết quả |
|---|---|---|
| 1 (r5f) | mã r5e vừa viết | 5 CONFIRMED (2 nặng: nhánh "0 sạp" là mã chết; 256 tràn BYTE) |
| 2 (r5g) | **chính bản vá r5f** | 5 CONFIRMED, 0 bị bác — **hai fix trong cùng một đợt xung khắc nhau** (hỏi lại 1,8 s vs cooldown 5 s của chính nó) |
| 3 (r5h) | bản vá r5g + toàn tính năng | 5 CONFIRMED, 1 bị bác (kẹt cả thành 25 phút; danh bạ làm **giảm** độ phủ) |
| 4 (r5i) | bản vá r5h | 3 CONFIRMED — **tất cả là hồi quy do chính r5h sinh ra** (nặng nhất: kẹt vĩnh viễn ở điểm tuần 1 + spam chat 3 dòng/giây) |

> **Bài học quan trọng nhất của phiên**: vòng nào cũng đẻ lỗi mới **ngay tại chỗ vừa sửa**.
> Phải soi **bản vá**, không chỉ soi mã cũ. Vòng 5 chưa chạy ⇒ **chưa có cơ sở nói bản r5i đã sạch**.

Khuôn workflow đã dùng (chạy lại được): 2 agent soi theo chiều (client / server) → mỗi phát hiện
1 agent **phản bác** đọc mã thật → chỉ vá cái `refuted=false`. Effort `high`, model Opus.

---

## 6. VIỆC CÒN NỢ / CHƯA KIỂM CHỨNG

1. **Phản biện vòng 5** trên khối `DTP_MUASAP` danh bạ (r5i) — chưa chạy, hết hạn mức phiên.
2. **Chưa test thật** bất cứ tính năng nào của phiên: loại 3 đi chợ, Thần Hành Phù, thăm dò sạp,
   danh bạ sạp. Toàn bộ mới chỉ qua đọc mã + phản biện.
3. **Danh bạ sạp chưa từng chạy live** (server chưa restart) — lần đầu chạy cần theo dõi kỹ:
   có ra dòng "Server báo N sạp trong thành" không, bot có đi thẳng tới sạp không.
4. Con trỏ quét xoay vòng phía server (lô 12 sạp kế tiếp) **chưa được phản biện** (agent soi
   server của vòng 4 chết vì hết hạn mức).
5. `lbhtdatau.lua` (lệnh bài hoàn thành Dã Tẩu) — sửa từ 19/08, **chưa bao giờ có hiệu lực** vì
   server chưa restart lần nào từ lúc đó.

---

## 7. Checklist test sau khi restart (làm theo thứ tự)

1. Thoát game → vào lại. Restart GameServer.
2. Bật auto Dã Tẩu, bật ô **"Mua ở sạp"**, để **Thần Hành Phù (6,1,1271) trong TÚI** (rương
   không tính).
3. Nhận nhiệm vụ **loại 3 (Tìm trang bị)** khi túi/rương không có đồ → phải thấy:
   `Không có trang bị cần khoe trong túi/rương - đi xem sạp người bán để mua...`
4. Trong thành → phải thấy **`Server báo N sạp trong thành - chạy thẳng tới từng sạp.`**
   *(Nếu thấy `Đi tuần điểm tụ tập 1/n...` thay vào đó ⇒ server chưa restart hoặc danh bạ hỏng.)*
5. Bot chạy tới sạp → `Xem sạp "tên"...`. Gặp sạp giả → `Bỏ qua sạp trang trí "tên"`.
   Sạp không tới được → sau 45 giây: `Không đến được chỗ sạp này - bỏ qua, đi sạp kế.`
6. Hết sạp trong thành → phải thấy `Đi tuần điểm tụ tập N/M...` (quét nốt), rồi mới nhảy thành:
   `Dùng Thần Hành Phù dịch chuyển tới thành kế tiếp...`
7. **Dấu hiệu HỎNG cần báo ngay**: cùng một dòng thông báo lặp liên tục (>2 dòng/giây); bot đứng
   im quá 1 phút ở một chỗ; chạy tới NPC chức năng **mà không có dòng nào ở trên**.

---

## 8. Lệnh build & deploy chuẩn (đã dùng cả phiên)

```bash
# CLIENT (CoreClient.dll) — build tại D, KHÔNG build cây E
cd /d/GAMEDEVNEW && MSBuild Sources/Core/Core.vcxproj "-p:Configuration=Client Release" -p:Platform=Win32 -m

# SERVER (CoreServer.dll) — chỉ cần khi sửa file KHÔNG bị ExcludedFromBuild (vd KProtocolProcess.cpp)
cd /d/GAMEDEVNEW && MSBuild Sources/Core/Core.vcxproj "-p:Configuration=Server Release" -p:Platform=x64 -m
```
Rồi **chép tay** sang `E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\{client,server}\`
(đổi tên file cũ nếu bị game giữ), chạy `ReverseTools\re_pe_crt.py <bin\client>` — phải ra
`CoreClient.dll ... CRT-TINH  DUNG` và `KET QUA: tat ca khop bang dung`.

**Luật sửa mã (TCVN3)**: `.cpp/.h` chỉ vá bằng **script python latin-1** (Write tool → chạy file,
**không** dùng heredoc vì bash nuốt backslash), kiểm `check_encoding.py` (FFFD = 0, số high-byte
không đổi). Chuỗi tiếng Việt mới: sinh literal bằng `vn_to_octal.py`, **đối chiếu byte** trước khi
tin — phiên này đã gõ nhầm 1 escape (`chỗ` = `\347` không phải `\352`).

---

## 9. Bản đồ mã (nơi cần sửa tiếp)

| Thành phần | Vị trí |
|---|---|
| Máy trạng thái auto | `Sources/Core/Src/CoreShell.cpp` → `DT_Process`, case `ATYPE_DATAU` |
| Pha đi chợ | `DTP_MUASAP` (~5300-5630), nhảy thành `DTP_CITYHOP` |
| Danh bạ sạp — client | cùng file, khối `g_nDTSapDs*` (~5455-5610) |
| Danh bạ sạp — server | `Sources/Core/Src/KProtocolProcess.cpp` → `c2sNeedCount` (~6070-6140) |
| Hook chat bắt `[SapMap]` | cùng file → `s2cExtendChat` (~4145-4185) |
| Hằng số dùng chung | `Sources/Core/Src/KDaTauCap.h` → `DATAU_SAPMAP_ID`, `DATAU_SAPMAP_MAXLEN` |
| Đặt chỗ id sentinel | `Sources/Core/Src/KNpcSet.cpp` → `KNpcSet::SetID` |
| Trạng thái auto (client-local) | `Sources/Core/Src/KPlayer.h` → `struct ExtAuto` |

---

*Ghi 20/08/2026 ~19:20. Tài liệu chi tiết cơ chế: `BANGIAO_AUTO_DATAU_WAUTO.md` mục 13.14 (A→L).*
