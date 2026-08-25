# BÀN GIAO — AUTO HOẠT ĐỘNG (BÁCH NHÂN + BANG CHIẾN + TÍN SỨ) · WAuto tab thứ 12 · 24/08/2026 đêm

> Thi công theo `AUTO_HOATDONG_SPEC.md` (*"thiết kế Auto tự hoạt động các tính năng trên bản
> note - tham khảo mã nguồn auto có sẵn"*).
> **100% phía client** (WAuto.exe + Game.exe + CoreClient.dll), không đụng server.
> **CHƯA TEST THẬT** — binary `.moi_2408_liendau` đã build đè (mốc 20:17), chưa swap.

---

## 0. 🔴 Đính chính đặc tả + phạm vi đợt này

**Đặc tả mục 1.1 CHỈ NHẦM CÂY MÃ NGUỒN.** Nó bảo cây đúng là `_WAuto` (11.077 dòng) — đó là
cây của **dự án USVOLAM**, không phải dự án này (ký ức `wauto-canonical-tree`). Auto Tống
Kim / Liên đấu / Dã Tẩu của dự án này đều nằm ở **`E:\Src_Auto_Ngoai\WAuto\WAuto`** (UI) +
**`D:\GAMEDEVNEW\Sources`** (engine trong `CoreClient.dll`/`Game.exe`) — đợt này làm đúng cây
đó, "tham khảo mã nguồn auto có sẵn" = tái dùng hạ tầng Tống Kim/Liên đấu (đi thành, bấm
thoại, chọn địch, chống kẹt). Trường cuối struct cũng không phải `bAutoLD` như đặc tả ghi.

**Phạm vi đã làm — 2/4 hoạt động chạy được ngay:**

| Hoạt động | Trạng thái | Vì sao |
|---|---|---|
| **Bách Nhân Lôi Đài** | ✅ THI CÔNG XONG | ưu tiên #1 của đặc tả — cày exp mỗi ngày |
| **Bang Chiến** | ✅ THI CÔNG XONG | tới mùa (29/10) bật là chạy |
| **Tỷ Võ Đài** | ✋ CHƯA — thiết kế ở mục 5 | cần 2 phe người thật phối hợp + cơ chế trao SỐ THỨ TỰ giữa 2 client chưa kiểm chứng được khi chưa test tay |
| **Thành Bảo** | ⛔ CHẶN CỨNG | map 984 **thiếu dữ liệu client** — auto mà đưa người chơi vào là đen màn. Chờ chủ game cấp pak rồi làm |

## 1. Tab thứ 12 "H.động" (lưới nút tab 4×3 giờ dùng đủ 12 ô)

| Ô | Ý nghĩa | Mặc định |
|---|---|---|
| Bật auto Bách Nhân Lôi Đài | cày exp map 960 | **TẮT** |
| Vào lúc (giờ:phút) | theo giờ MÁY CHỦ + ô Lệch giờ | 12:05 |
| Cày (phút) | 0 = ở tới khi ăn đủ **50 lượt exp/ngày** | 0 |
| Chế độ | **Đứng ăn exp (an toàn)** / Tranh làm Lôi Chủ (x3 exp, đánh theo tab PK) | Đứng ăn |
| Đài | Tự chọn (đài 2 — né đài chủ) / Đài 1..5 | Tự chọn |
| Tự ăn buff Cổ Thủ | nghe loa → đi 15 điểm quen tìm NPC → x2 exp 30 phút | bật |
| Bật auto Bang Chiến | mùa 29/10-05/11 | **TẮT** |
| Đi lúc | cửa vào thật 20:30-21:30 | 20:25 |
| Lệch giờ | chung cho cả hai | 0 |

Cấu hình 11 trường mới **ở CUỐI `autoData`** + nhánh di trú `uOldSize <= offsetof(autoData,
bHDBachNhan)` — file `.dat` cũ không hỏng. Dòng trạng thái chân cửa sổ hiện đủ pha mới
(`Bách Nhân: đang cày kinh nghiệm`, `Bang Chiến: đang đánh trận`…).

## 2. Máy trạng thái (`HD_Process`, `ATYPE_HOATDONG`, sau TK/LD trong chuỗi nhường quyền `nBS`)

```
BÁCH NHÂN:
HDP_BN_GO  → chưa ở Lâm An thì LD_DiThanh (Thần Hành Phù / Xa Phu) → NPC "Quan nhắc nhở
             Hoàng Thành Tư" (1464,3223) → "Lôi Đài Hoàng Thành Tư" → "Ta muốn vào..."
             (bị "chưa tới giờ" → đợi 5' thử lại; "thiếu cấp" → bỏ)
             🔴 KHÔNG bao giờ chọn "Kết thúc đối thoại" — nó trỏ hàm lỗi (npc_enter.lua)
HDP_BN_IN  → map 960: đếm lượt exp cục bộ (5 phút/tick; buff +1, Lôi Chủ +2 — khớp
             "Lôi Chủ có buff tiêu 4 lượt/5 phút" của đặc tả); nếu task 2709 có sync và
             phần ngày khớp hôm nay thì tin số server
             · mode Đứng ăn: đứng gần điểm hồi sinh (1749,3135); 80 phút không có gì thì
               KHINH CÔNG (SendClientCmdJump) chạm đài rồi nhảy xuống — né bị đá 90 phút
             · mode Lôi Chủ: khinh công lên tâm đài đã chọn; có địch (relation enemy,
               người LẪN NPC cao thủ) → giao máy PK (trả 2); KHÔNG có địch → ĐỨNG YÊN
               trên đài (đạp trap lại = bỏ đài, mất chuỗi)
HDP_BN_BUFF→ loa "Cổ Thủ đã xuất hiện" → đi lần lượt 15 điểm drummer.txt, tìm NPC "Cổ Thủ"
             gần nhất → thoại → chọn dòng đầu → buff 30'
HDP_BN_OUT → đủ lượt / hết phút cày / loa "hôm nay kết thúc" → Xa phu (1750,3159) → về Lâm An

BANG CHIẾN:
HDP_BC_GO  → LD_DiThanh tới Ba Lăng (53) → NPC "Võ Lâm Truyền Nhân" (1628,3173) →
             tìm dòng "Ta muốn tham gia thi đấu" (khớp VĂN BẢN, không khớp chỉ số — pha
             khác thì huỷ, 60 giây bấm lại, tối đa hết cửa sổ 85 phút)
HDP_BC_SIGN→ map 608-613: đi tới Ô TRAP GẦN NHẤT trong 36 ô signup_trap.txt
             · "chưa bắt đầu"/"đang tập hợp" → 30 giây đạp lại (cửa thật 20:30)
             · bị đẩy về (1619,3175) im lặng 8 lần → "bang chưa có liên minh?" → bỏ
             · "chết đủ trần mạng"/"sai khu" → bỏ có thông báo
HDP_BC_FIGHT→ map 605-607: trả 2 — máy PK tab PK đánh; chọn địch bằng LD_ChonDich
             (mission group — server AddMSPlayer camp 1/2 nên phân biệt phe chuẩn)
             chết → tự hồi sinh (nBS bấm hộ) → đánh tiếp
             bị đẩy về map báo danh → đọc task 2370/2376: đủ trần mạng thì báo, KHÔNG
             🔴 bao giờ đạp lại trap (đạp lại khi đang trong trận = MẤT 1 MẠNG)
```

**Ưu tiên khi nhiều máy cùng bật**: Tống Kim > Liên đấu > Hoạt động (chuỗi `nBS` trong
`ExtAutoLoop`). Bang Chiến ưu tiên hơn Bách Nhân trong nội bộ tab (cửa sổ hẹp hơn).

## 3. Bám đúng 4 luật của đặc tả

1. **Không nhét cứng giờ/cấp**: giờ do người chơi nhập (server đổi `cauhinh_hoatdong.lua`
   thì sửa ô giờ); marker "chưa tới giờ"/"thiếu cấp" bắt bằng văn bản do server nói.
2. **Từ chối im lặng** xác nhận bằng **đổi map / vị trí** (trap Bang Chiến đếm số lần bị
   đẩy về ô 1619,3175; báo danh Bách Nhân xác nhận bằng sang map 960).
3. Mọi toạ độ/marker trích **byte-for-byte từ script sống** — `ReverseTools/
   gen_hoatdong_tables.py` → `KHoatDongTables.h` (5 đài + 4×5 điểm đẩy ra, 15 điểm Cổ Thủ,
   36 ô trap, 17 marker). Sửa thì chạy lại script.
4. Task client: 2709 (lượt exp) **được phòng hờ** — đếm cục bộ là chính vì đặc tả cảnh báo
   server đặt task không kèm sync; 2370/2376 Bang Chiến đặc tả xác nhận có sync nên đọc thẳng
   (`m_cTask.GetSaveVal`).

## 4. Nghiệm thu

**Bách Nhân (làm được NGAY hôm nay, 12:00-24:00):**
0. Swap 3 binary `.moi_2408_liendau` (thoát hẳn Game.exe + WAuto trước).
1. Tab "H.động" → bật Bách Nhân, giờ vào đặt phút hiện tại +2, Chế độ = Đứng ăn.
2. Auto tự đi Lâm An → NPC → vào map 960 → `[Hoạt động] Đã vào Lôi Đài Hoàng Thành Tư.` →
   đứng gần điểm hồi sinh; chân cửa sổ WAuto hiện `Bách Nhân: đang cày kinh nghiệm`.
3. Đợi loa Cổ Thủ (mốc phút 00/30) → auto chạy đi tìm → `Đã nhận buff Cổ Thủ x2 kinh nghiệm.`
4. Đặt "Cày (phút)" = 5 → hết 5 phút auto ra Xa phu → về Lâm An → trả máy.
5. Thử Chế độ = Tranh làm Lôi Chủ (đài 2): khinh công lên đài, có người khiêu chiến thì đánh.
6. **HỎNG cần báo**: chọn nhầm "Kết thúc đối thoại" (lỗi script server); đứng đấm tường
   không lên được đài; không tìm thấy Cổ Thủ dù loa nổ; quá nửa đêm không tự về.

**Bang Chiến (chỉ test được khi admin ép pha hoặc tới mùa 29/10):** bật ô, chỉnh giờ đi
khớp giờ admin mở; cần nhân vật **có bang thuộc liên minh**; xem nó qua Ba Lăng → thoại →
map báo danh → đạp trap → vào trận PK. Nhớ: **chạm trap là mất tổ đội** (luật server).

## 5. Thiết kế để lại cho 2 hoạt động chưa làm

**Tỷ Võ Đài** (khi nào muốn làm): khuôn giống tổ đội Liên đấu — thêm ô "Tự Tỷ Võ với bạn
diễn", 2 máy lập tổ đúng 2 người (tái dùng `LD_MoiParty`/`LD_NhanParty`), captain bấm NPC
"Công Bình Tử" (12 vị trí, 3 thành) chỉ số 0 → chọn `1 vs 1` (chỉ số = số người). Điểm phải
kiểm chứng bằng test tay trước khi code: sau khi mở trận, 2 đội trưởng vào đài bằng đường
nào (tự dịch chuyển hay phải nhập số) — thoại báo số nằm trong `cap.szDlg` bắt được, nhưng
hộp **nhập số** là `S2C_INPUT_BOX nType=2` (KUiGetNumber) — đã có sẵn kênh trả lời
(`SendClientCmdInputBox(2, ...)`, khuôn hộp nhập tên Liên đấu). Bẫy phải né: task 2340-2342
bị ghi đè mỗi lần bấm NPC; tổ 3 người là trượt; thông báo "không đem đủ ngân lượng" nghĩa
thật là "chờ trận cũ dọn xong".

**Thành Bảo**: mọi số liệu đã nằm trong đặc tả (NPC 1663,3262 Lâm An; điểm/lệnh bài; task
3399/4056-4061). Chặn duy nhất = **thiếu pak client map 984**. Khi có pak: khuôn = Bách Nhân
(canh Chủ nhật 17:00, vào map, đi theo tuyến cây, **đổi lệnh bài NGAY trong ngày** vì 0h
xoá điểm, kiểm ô túi trống vì lệnh bài không xếp chồng).

## 6. Tệp đã sửa (commit kèm)

- `Sources/Core/Src/KHoatDongTables.h` (MỚI - sinh tự động) + `ReverseTools/gen_hoatdong_tables.py` (MỚI)
- `Sources/Core/Src/CoreShell.cpp` — khối `AUTO HOAT DONG` (~720 dòng) + case `ATYPE_HOATDONG`
  + `WA_HoatDong` dời xuống sau khối HD (vì cần enum HDP_*)
- `Sources/Core/Src/CoreShell.h` — `ATYPE_HOATDONG`
- `Sources/Core/Src/KPlayer.h` — 20 trường `nHD*/uHD*` trong `ExtAuto`
- `Sources/Core/Src/ipc_shared.h` (+2 bản sao) — 11 trường cấu hình ở CUỐI struct
- `Sources/S3Client/S3Client.cpp` — chuỗi nhường quyền `nBS = nTK ? nTK : (nLD ? nLD : nHD)`
- `WAutoUI/{Resource.h,WAuto.rc,WAuto.cpp}` — tab 12 (23 điều khiển) + tooltip + lưu/nạp/di trú

Binary staged: `CoreClient.dll.moi_2408_liendau` 2.326.016 B (CRT-TĨNH ĐÚNG) ·
`Game.exe.moi_2408_liendau` 1.260.544 B (UCRT-RELEASE ĐÚNG) · `WAuto.exe.moi_2408_liendau`
387.072 B — mốc 20:17, **một lần swap ăn cả 3 đợt hôm nay** (Liên đấu r1+r2 + Hoạt động).

## 7. Đợt 2 cùng đêm — TÍN SỨ (r4, ~20:55)

### 7.0 🔴🔴 Sửa lỗi nghiêm trọng đợt r3 phát hiện khi làm r4

Miếng vá `S3Client.cpp` của r3 (nối `ATYPE_HOATDONG` vào chuỗi `nBS`) **chưa từng được áp**
— heredoc chết im lặng, `git log` xác nhận tệp không đổi. Nghĩa là **Game.exe staged mốc
20:17 KHÔNG có dây nối Hoạt Động nào cả** (bật Bách Nhân/Bang Chiến sẽ không chạy). Đã áp
lại bằng script file (`r4_s3.py`), build lại 20:53. **Chỉ dùng bộ binary mốc 20:53.**

### 7.1 Luồng Tín Sứ (đọc từ script server, task client tự sync)

- Task đọc được ở client: `1201` mã 5 rương (mở theo THỨ TỰ) · `1202` đã mở (prefix của
  1201) · `1203` trạng thái (0 không/hỏng; 10 đã nhận; 20 trong ải; 21 tạm ngưng; 25/30
  xong) · `1204` tuyến (1 = Thành Đô→Đại Lý, 2 = ngược) · `4128` lượt hôm nay
  (`YYMMDD*256+n`). Trần 2 lượt/ngày, lượt 3 cần Thiên Khố Bảo Lệnh (6,1,3431). Cần cấp 90.
- Máy 3 pha nối sau BC trong `HD_Process`: `HDP_TS_GO` (tới Dịch Quan thành — nhận khi
  1204==0, giao khi 1203∈{25,30}; giao xong còn lượt thì **nhận tiếp ngay tại thành đang
  đứng** vì 2 tuyến nối đuôi nhau) → `HDP_TS_XAPHU` (Xa Phu → "Đi nơi đặc biệt…" → "Muốn"
  → NewWorld 395) → `HDP_TS_AI` (trong ải: bấm Dịch quan "Bắt đầu/Tiếp tục nhiệm vụ", vòng
  5 rương: giết `Bảo Khố Thủ Hộ Giả N` cạnh rương bằng máy PK (`return 2`) rồi thoại `Bảo
  rương N`; xong/hỏng/quá 25 phút → "Rời khỏi khu vực" tự dịch về thành đích).
- Nhiệm vụ **dở dang** (`1203 != 0`): bật auto là làm tiếp NGAY bất kể khung giờ; đang kẹt
  trong map 395 lúc auto bật cũng tự nhận ra.
- Từ chối bắt theo marker: hết lượt/mệt mỏi/quá mệt → DONE; chưa đủ cấp 90 → DONE; **không
  đủ 5 ô trống** → nhắn người chơi dọn túi rồi DONE (auto KHÔNG tự vứt đồ).
- Bảng số mới trong `KHoatDongTables.h` (generator đã nới): toạ độ Dịch Quan 11/162/ải,
  2 tuyến `g_HDTSVe`, 9 rương `g_HDTSRuong`, 15 marker + 3 tên NPC.
- Quái Thủ Hộ Giả không phải `kind_dialoger` nên thêm `HD_TimQuai` (tìm quái sống gần nhất
  theo tên hạ-ascii) thay vì `LD_FindNpcGan`.

### 7.2 UI (chèn vào chỗ trống cuối tab "H.động", ID 539-546)

Kẻ ngăn `IDC_SEP_11C` y288 → ô "Bật auto Tín Sứ (đưa thư, cần cấp 90)" y292 → "Đi lúc"
giờ:phút y305 → "Lượt/ngày (3 cần Bảo Lệnh)" y318. 4 trường `bHDTinSu/nHDTSGio(9)/
nHDTSPhut(30)/nHDTSLuot(2)` nối CUỐI `autoData` + di trú `offsetof(bHDTinSu)`; đủ 5 dây
(save/update/default/migrate/autosave) + 3 tooltip; vòng ShowTab nới tới `IDC_EDITOR_12_TSL`.

### 7.3 Nghiệm thu Tín Sứ (nhân vật cấp ≥90, túi ≥5 ô trống)

1. Swap bộ binary **mốc 20:53** (thoát hẳn Game.exe + WAuto trước).
2. Tab "H.động" → bật ô Tín Sứ, giờ đặt phút hiện tại +2, Lượt = 1.
3. Xem auto: về Thành Đô/Đại Lý → Dịch Quan → `Tới giờ Tín Sứ...` → Xa Phu → vào ải 395 →
   giết Thủ Hộ Giả → mở 5 rương đúng thứ tự → rời ải (tự dịch sang thành kia) → Dịch Quan
   giao → `Đã giao nhiệm vụ Tín Sứ - nhận thưởng.` Chân WAuto đổi trạng thái theo pha.
4. Thoát game giữa chừng trong ải → vào lại, bật auto → phải tự làm tiếp.
5. Đặt Lượt = 2 → giao xong lượt 1 phải nhận tiếp ngay tại thành đích.
6. **HỎNG cần báo**: mở rương sai thứ tự (1203 về 0), đứng đơ cạnh rương không đánh Thủ Hộ
   Giả, không thoát ải sau 25 phút, giao xong không nhận tiếp.

### 7.4 Tệp sửa thêm đợt r4 + binary

`CoreShell.cpp` (3 pha + 4 hàm phụ TS + vào cuộc + `WA_HoatDong`) · `KHoatDongTables.h`
(sinh lại) + `gen_hoatdong_tables.py` (nới TS) · `ipc_shared.h` ×3 (+4 trường) ·
`KPlayer.h` (+`nHDKeyTS`) · `S3Client.cpp` (áp lại toàn bộ dây HD, mục 7.0) ·
`WAutoUI/{Resource.h,WAuto.rc,WAuto.cpp}` (khối TS).

Binary staged đè lên bản 20:17: `CoreClient.dll.moi_2408_liendau` 2.333.184 B md5 `70626f4d`
(CRT-TĨNH) · `Game.exe.moi_2408_liendau` 1.263.104 B md5 `8816d10e` (UCRT-RELEASE) ·
`WAuto.exe.moi_2408_liendau` 389.120 B md5 `3d3a8812` — **mốc 20:53, một lần swap ăn cả
Liên đấu r1+r2 + Hoạt động r3 + Tín sứ r4**.

### 7.5 Sáng 25/08 — vá theo báo lỗi của chủ game + 2 vòng phản biện (binary 09:43)

Chủ game test 08:50-09:00 (đúng lúc server ải 395 đang hỏng script — xem
`GHICHU_TINSU_TRAP_2508.md` của phiên song song) và báo 2 hiện tượng: (1) đặt 9:00 mà
chưa tới giờ auto đã chạy; (2) tắt kích hoạt nick rồi mà vẫn tự kích NPC. Gốc + vá:

- **Chạy trước giờ**: nhân vật còn nhiệm vụ dở dang (1203≠0) và thiết kế cũ cho "dở dang
  chạy bất kể giờ". Nay: **mọi đường vào đều phải trong khung [giờ đặt .. +12h)**; câu
  thông báo bắt đầu in kèm `(giờ máy chủ hh:mm)` để soi ô Lệch giờ.
- **Tắt không ăn**: trạng thái pha nằm trong CoreClient; tick lại là chạy tiếp giữa pha,
  và tắt 1 ô trong khi ô khác còn bật thì pha cũ vẫn chạy. Nay: **mỗi nhịp HD_Process
  kiểm lại đúng ô của pha đang chạy** — bỏ tick là buông máy về OFF ngay; cửa vào theo
  map (960/Bang Chiến) cũng phải có ô bật tương ứng.
- **Chống lặp vô hạn** (phản biện A, 5 lỗi CAO): `nHDKeyTS` đổi nghĩa thành **khoá "đã
  chốt hôm nay"** (xong đủ lượt / bỏ cuộc / bị từ chối hẳn — hết lượt, thiếu cấp 90, hỏng
  nhiệm vụ, quá 25' trong ải) — nhánh dở dang cũng qua khoá này nên mọi DONE là dừng thật;
  lỗi **tạm** (thiếu 5 ô trống, đi thành thất bại) nghỉ 10 phút bằng đồng hồ riêng
  `uHDTSNghi` rồi tự thử lại; thêm **watchdog 35 phút** trong ải (thoại liệt / không phải
  đội trưởng tổ đội → nghỉ hết ngày, cần xử lý tay — auto KHÔNG tự rời tổ đội).
- Vá kỹ thuật kèm: rò `uLDHopT` ở nhánh về-thành mới (làm mọi chuyến `LD_DiThanh` sau đó
  fail tức thì); HD gọi `LD_DiThanh` theo đúng nhịp nội bộ của nó (hết cảnh dùng phù
  ~2,5 lần/giây ở map hoang); ngày so khoá/lượt đổi sang **ngày máy chủ** (`HD_NgayServer`
  = giờ máy + Lệch giờ) khớp cách server ghi task 4128; bắt thêm câu "không thể tiếp nhận
  nhiệm vụ giống nhau" (task chưa sync) — chờ 2,5s thay vì loạn thoại; mở rương đánh dấu
  `uHDDlgSeen`; làm mới hạn 25' khi bấm "Bắt đầu/Tiếp tục"; HANPHA nghỉ 5' trước khi thử lại.
- **Dây nối** (phản biện B): `bUseFKey` không còn `RESETNPCID` khi `nBS==2` (trước đó ai bật
  "dùng phím F" là mục tiêu máy TK/LĐ/HĐ giao bị xoá mỗi nhịp — Tín Sứ không giết nổi Thủ Hộ
  Giả); máy nhặt-đuổi không chạy khi `nBS==1` (đang cầm lái đi đường) — `nBS==2` giữ nếp cũ.
- WAuto: kẹp giờ 0-23/phút 0-59 cho BN/BC/TS; tooltip Tín Sứ viết lại đúng luật khung giờ
  + ghi chú đánh Thủ Hộ Giả dùng thiết lập tab PK (bật "Đuổi theo mục tiêu", tầm nhìn > 0).

**Lưu ý phối hợp 25/08**: cây D: sáng nay có thêm code client của phiên "3 Hoạt Động"
(commit `f4fb0bf1`, F11 Săn Boss) — bộ binary dưới đây build SAU commit đó nên **đã gồm cả
phần client 3HD**; bộ `.moi_2508_3hoatdong` cũ hơn, không cần dùng nữa. Server ải 395 phải
được restart GS (vá trap của phiên Tín Sứ) thì Tín Sứ mới chơi được — auto phía client đã
tự vệ đủ nếu server còn hỏng (nghỉ hết ngày thay vì lặp).

Binary staged (đè bộ 20:53, hậu tố `.moi_2408_liendau`, **mốc 09:43 25/08**):
`CoreClient.dll` 2.335.744 B md5 `d485372c` (CRT-TĨNH) · `Game.exe` 1.264.128 B md5
`9eec1d50` (UCRT-RELEASE) · `WAuto.exe` 389.632 B md5 `1b3de743`.

*Ghi 24/08/2026 ~20:20, bổ sung Tín Sứ ~21:00, bổ sung 7.5 sáng 25/08.*
