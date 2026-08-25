# BÀN GIAO — AUTO LIÊN ĐẤU (WLLS) CHO NGƯỜI CHƠI THẬT · WAuto tab thứ 11 · 24/08/2026

> Thi công theo đơn của chủ game: *"đọc bàn giao và làm thêm tính năng Auto tự động liên đấu
> cho người chơi"*.
> **100% phía client** (WAuto.exe + Game.exe + CoreClient.dll). **KHÔNG đụng một dòng nào phía
> server** — hệ leaguematch đã port 20-21/08 giữ nguyên.
> **CHƯA TEST THẬT** — binary mới đang nằm cạnh binary đang chạy, xem mục 2.

> 🔴 **Commit: `faeee99d`** — KHÔNG phải commit riêng. Lúc tôi sắp commit thì **phiên song song**
> (đang vá hệ va chạm đạn trong cùng cây `D:\GAMEDEVNEW`) commit trước và **cuốn luôn 15 tệp
> đã stage của đợt Liên đấu vào commit của họ** — đúng cạm bẫy đã ghi trong ký ức
> *"hai phiên chung cây D: — commit phiên kia nuốt tệp mình sửa"*. **Không mất gì** (đã đối chiếu
> lại từng tệp trên đĩa), chỉ là lời commit của `faeee99d` **chỉ nói về vá đạn**, không nhắc
> Liên đấu. Ai tra cứu sau này cứ nhớ: **toàn bộ mã Liên đấu nằm trong `faeee99d`.**

---

## 0. Trả lời 3 câu hỏi chủ game đặt ra

**(a) Source auto Thái Lan có tính năng Liên đấu để làm theo không? → KHÔNG CÓ.**
Đã quét toàn bộ 614 tệp `.cs/.resx/.txt` của `D:\Source_ANTITHAILAN` bằng cả byte TCVN3 thô
lẫn TCVN3-bọc-UTF8 (bộ quét chạy đúng: *Phượng Tường* 44 hit, *Tống Kim* 44, *Dã Tẩu* 34,
*Thất Thành* 2, *Tín Sứ* 2). Các từ khoá liên đấu đều **0 hit**:

| Từ khoá | Hit |
|---|---|
| liên đấu / hội trường / chiến đội / sứ giả / thi đấu / báo danh / Uy Danh / vinh dự | **0** |
| `LienDau`, `LeagueMatch`, `wlls`, `HoiTruong`, `ThiDau`, `ChienDoi`, `SuGia`, `LoiDai` (ASCII) | **0** |

Bảng 246 địa chỉ `Class56.cs` cũng chỉ có nhóm `TONG_*` (Tống Kim), không có gì cho liên đấu.
Bản auto đó là auto cho private-server VNG, xoay quanh Tống Kim / CTC / Thất Thành / Tín Sứ.
⇒ **Không có gì để "đồng bộ" — toàn bộ auto Liên đấu phải dựng từ script sống của máy chủ**
(đúng những gì đã làm, mục 3). Cái vay được từ auto Thái Lan là **kiến trúc và cách bố trí
tab**, viết riêng ở `THIETKE_UI_WAUTO_THEO_THAILAN.md`.

**(b) Đặc tả liên đấu cũ có chính xác không? → Đúng phần lớn, 6 điểm phải sửa** (mục 6).

**(c) Hệ liên đấu có đang sống không? → CÓ, đo thật hôm nay.**
`logs\script_jx2.log` lúc **18:00:11 ngày 24/08/2026** ghi
`dw wlls_setphase(131,1,4,26082401,1,{1,1})` = mùa 131, thể loại 1 (Song đấu), **pha 4 (mở báo
danh)**, matchid 26082401, mở cả 2 hạng; kèm dòng tin toàn máy chủ *"Võ lâm Kiệt xuất 1 đang
trong giai đoạn báo danh…"*. Đồng hồ máy chủ **trùng đồng hồ máy này** (18:04 = 18:04) ⇒ ô
"Lệch giờ" để 0.

---

## 1. Đã làm gì

| Việc | Trạng thái | Nằm ở đâu |
|---|---|---|
| Tới khung giờ tự bỏ việc đang làm để đi liên đấu | ✅ | `LD_KhungGio` + tin toàn máy chủ |
| Tự ra **Sứ giả liên đấu** đúng hạng trong thành | ✅ | pha `LDP_GO` |
| **Tự lập chiến đội** (kể cả hộp nhập tên đội) | ✅ | pha `LDP_TEAM` / `LDP_NAME` |
| Vào **hội trường** bằng dòng "Ta muốn đến khu thi đấu hạng…" | ✅ | pha `LDP_TEAM` |
| **Tự cất đồ CẤM vào rương** trước khi báo danh (77 món) | ✅ | pha `LDP_STASH` |
| Báo danh 3 màn thoại ở **Quan viên hội trường** | ✅ | pha `LDP_SIGNUP` |
| **Đứng yên** trong khu chuẩn bị (ra khỏi = bỏ cuộc) | ✅ | pha `LDP_PREP` |
| Vào đấu trường **đánh theo cấu hình tab PK** | ✅ | pha `LDP_FIGHT` → trả 2 → `ATYPE_PKFIGHT` |
| Chết → tự bấm hồi sinh → chờ lượt sau → báo danh lại | ✅ | `LDP_WAIT` + S3Client `PushReviveButton` |
| Hết khung giờ → **Xa phu** đưa về thành → trả máy | ✅ | pha `LDP_LEAVE` |
| Cuối mùa (29→07) **tự nhận thưởng xếp hạng + danh hiệu** | ✅ | pha `LDP_AWARD` |

## 2. Binary mới (ĐÃ ĐẶT CẠNH bản đang chạy — chưa swap)

Lúc thi công **Game.exe và WAuto.exe đang chạy** nên không ghi đè được. Ba tệp mới nằm ở
`E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\`:

| Tệp mới | Cỡ | Kiểm |
|---|---|---|
| `CoreClient.dll.moi_2408_liendau` | 2.322.432 B | CRT-TĨNH ĐÚNG |
| `Game.exe.moi_2408_liendau` | 1.264.128 B | UCRT-RELEASE ĐÚNG |
| `WAuto.exe.moi_2408_liendau` | 377.344 B | — |

**Người vận hành thoát hẳn Game.exe + WAuto.exe rồi chạy:**

```bat
cd /d E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client
move /y CoreClient.dll CoreClient.dll.cu_2408_truoc_liendau
move /y Game.exe       Game.exe.cu_2408_truoc_liendau
move /y WAuto.exe      WAuto.exe.cu_2408_truoc_liendau
move /y CoreClient.dll.moi_2408_liendau CoreClient.dll
move /y Game.exe.moi_2408_liendau       Game.exe
move /y WAuto.exe.moi_2408_liendau      WAuto.exe
```

🔴 Đợt này **Game.exe CÓ đổi** (`KMyApp::ExtAutoLoop` trong `Sources/S3Client/S3Client.cpp`) —
chỉ swap DLL là tính năng không bao giờ chạy.
Build lại: `Core.vcxproj "Client Release" Win32` → `S3Client.vcxproj Release Win32
-p:VcpkgEnableManifest=false` → `WAuto.vcxproj Release Win32` (cây `E:\Src_Auto_Ngoai\WAuto\WAuto`).
(`MSB3073` cuối build S3Client là bước chép `Game.map` không có tệp — exe vẫn hợp lệ.)

## 3. Bản đồ mã

### 3.1 Tệp mới
- `Sources/Core/Src/KLienDauTables.h` — **SINH TỰ ĐỘNG** bởi
  `ReverseTools/gen_liendau_tables.py`, trích **byte-for-byte từ script Lua đang chạy**:
  21 map hội trường / 21 khu chuẩn bị / 21 đấu trường, 7 thành có Sứ giả kèm hạng + toạ độ,
  toạ độ 3 Quan viên + Xa phu + Rương + Thị vệ, 7 dòng Xa phu → map, **19 marker thoại raw
  TCVN3**, 5 tên NPC (đã hạ thường ASCII đúng như `g_StrLower`), **bảng 77 món đồ CẤM**.
  **Sửa lại thì chạy lại script, đừng gõ tay.**
- `ReverseTools/gen_liendau_tables.py` — bộ sinh bảng trên.

### 3.2 Tệp sửa
| Tệp | Sửa gì |
|---|---|
| `Sources/Core/Src/CoreShell.cpp` | +1000 dòng: khối `AUTO LIEN DAU / WLLS` (sau `HET AUTO TONG KIM`), `case ATYPE_LIENDAU` |
| `Sources/Core/Src/CoreShell.h` | `ATYPE_LIENDAU` (ngay sau `ATYPE_TONGKIM`) |
| `Sources/Core/Src/KPlayer.h` | 18 trường `nLD*/uLD*` trong `ExtAuto` + khởi tạo |
| `Sources/Core/Src/KDaTauCap.h` | `uInpSeq` + `szInpFunc` + `szInpHoi` — chụp **hộp nhập chuỗi** |
| `Sources/Core/Src/KProtocolProcess.cpp` | `s2cInPutBox` case 1: chụp tên hàm callback trước khi bung UI |
| `Sources/Core/Src/ipc_shared.h` (+ `WAutoUI/`, cây E) | 17 trường cấu hình Liên đấu **ở CUỐI struct** |
| `Sources/S3Client/S3Client.cpp` | gọi máy Liên đấu sau Tống Kim; gộp `nTK`/`nLD` thành **một biến `nBS`** cho 17 công tắc nhường quyền |
| `WAutoUI/{Resource.h,WAuto.rc,WAuto.cpp}` | tab thứ 11 "Liên đấu" (26 điều khiển) + **xếp lại dải nút tab thành lưới 4×3** |

### 3.3 Máy trạng thái (`LD_Process`, trả 0 = thả máy / 1 = cầm lái / 2 = đang trong đấu trường)

```
LDP_OFF   → tới khung giờ (đồng hồ máy + ô "Lệch giờ") → kiểm cấp ≥ 80 → LDP_GO
            (nếu đang đứng sẵn ở khu chuẩn bị / đấu trường thì cầm lái NGAY,
             còn ở hội trường thì chỉ cầm lái khi ĐANG trong khung giờ)
LDP_GO    → đang ở thành có Sứ giả ĐÚNG HẠNG? → tới Sứ giả, mở thoại → LDP_TEAM
            không đúng thành → (tuỳ chọn) dùng Thần Hành Phù 2 lần rồi bỏ khung giờ
LDP_TEAM  → bấm "Ta muốn đến khu thi đấu hạng …"; bị từ chối vì chưa có đội thì
            vào "Chiến đội hạng … của ta" → "Ta muốn lập…nhóm" → "Ta muốn lập nhóm!"
            → hộp nhập tên → gửi tên → quay lại bấm vào hội trường
            (thể loại Đơn đấu: máy chủ mời "Ta muốn lập chiến đội!" — bấm thẳng)
LDP_STASH → cất đồ CẤM (tay → hành trang → rương; ô dùng ngay nhấc lên tay trước)
LDP_SIGNUP→ Quan viên hội trường GẦN NHẤT: "Ta muốn tham chiến!" → "Ta đã sẵn sàng!"
            → "Xác nhận"; bị mắng đồ cấm → quay lại LDP_STASH (tối đa 3 lần);
            chưa tới pha 4 → huỷ thoại, 12 giây bấm lại
LDP_PREP  → khu chuẩn bị: ĐỨNG YÊN (ra khỏi = bỏ cuộc), huỷ mọi thoại
LDP_FIGHT → chọn địch = NGƯỜI CHƠI khác MISSION GROUP, có đường nhìn → giao cho máy PK
LDP_WAIT  → về hội trường sau trận: còn khung giờ → báo danh lượt sau; hết → LDP_LEAVE
LDP_AWARD → cuối mùa: "Ta muốn lãnh phần thưởng liên đấu" → xếp hạng → danh hiệu
LDP_LEAVE → Xa phu hội trường → chọn thành → trả máy
```
Mỗi pha (trừ FIGHT / PREP / WAIT) **kẹt quá 3 phút** → bỏ khung giờ, trả máy cho auto cũ.

**Ba mốc nhận diện, không cần đọc biến GLB của máy chủ:**
1. **Map** — 3 bảng map (hội trường / khu chuẩn bị / đấu trường) trích từ 7 tệp `macthtype`.
2. **Tin toàn máy chủ** `AddGlobalNews` khi vào pha 4 (`wlls_gmscript.lua:118`) — đã có sẵn
   đường chụp `UI_NEWSINFO` từ đợt Tống Kim.
3. **Lời thoại NPC** — 19 marker byte-chuẩn.

## 4. Tab "Liên đấu" trong WAuto

| Ô | Ý nghĩa | Mặc định |
|---|---|---|
| Bật auto Liên đấu | công tắc tổng | **TẮT** |
| Khung 1 / Khung 2 (giờ : phút) | khớp 1-1 với `GIO_MO` trong `wlls_config.lua` | 18:00 và 20:00, bật cả 2 |
| Số lượt / Phút/lượt | khớp `nLuot` và `PHUT_MOI_LUOT` | 4 và 15 |
| Đi sớm (phút) | tới hội trường trước giờ mở | 5 |
| Lệch giờ | giờ máy chủ − giờ máy này, tính bằng **phút** (âm được) | 0 |
| Tự lập chiến đội nếu chưa có | | bật |
| Tên đội | ≤ 16 ký tự, để trống = tên nhân vật | trống |
| Tự cất đồ cấm vào rương | dùng mật khẩu rương ở tab Hậu cần | bật |
| Cuối mùa tự nhận thưởng + danh hiệu | ngày 29 → 07, mỗi ngày 1 lần | bật |
| Hết giờ về | 7 thành + "Trở lại thành lúc đi" | Trở lại thành lúc đi |
| Dùng phù về thành nếu chưa đúng thành | | tắt |

Cấu hình lưu **nguyên struct** vào `APdata\<ID nhân vật>.dat` như cũ; 17 trường mới nằm **ở
cuối struct** + nhánh di trú `uOldSize <= offsetof(autoData, bLienDau)` ⇒ **file .dat cũ đọc
lên không hỏng**, tính năng mặc định TẮT.

**Dải nút tab đã xếp lại thành lưới đều 4 cột × 3 hàng** (mỗi nút rộng 39, x = 2/41/80/119),
tức **12 chỗ, đang dùng 11** — trước đây hàng 1-2 mỗi hàng 3 nút rộng 49/50 còn hàng 3 bị bóp.
Không điều khiển nào khác phải dời (groupbox vẫn bắt đầu ở y = 112).

## 5. 🔴 Năm điều chủ game cần biết

**5.1 Mùa này là Song đấu (2 người).** Mùa 131 = tháng 8/2026 = thể loại **1 Song đấu**,
`max_member = 2`. Auto lập được đội **nhưng không tự rủ được đồng đội** — muốn có đồng đội
thì hai người phải lập tổ đội thường rồi đội trưởng vào Sứ giả chọn *"Để đội hữu của Ta gia
nhập vào đội thi đấu này"*. **Đội 1 người vẫn báo danh và thi đấu được** (server chỉ đòi
`count <= max_member`), chỉ là ra sân ít người hơn. Muốn auto chạy trọn vẹn một mình thì đặt
`LOAI_CO_DINH = 5` (Đơn đấu) trong `script\leaguematch\wlls_config.lua` rồi restart — lúc đó
Sứ giả tự mời lập đội và auto bấm luôn.

**5.2 Cửa báo danh chỉ mở 4 PHÚT ĐẦU mỗi lượt.** Trong 15 phút của một lượt: 4 phút pha 4
(báo danh + khu chuẩn bị) → 10 phút pha 5 (thi đấu) → nghỉ. Ngoài 4 phút đó Quan viên trả lời
*"Vẫn đang tiến hành thi đấu!"* và auto tự chờ lượt sau. Vì vậy ô **"Đi sớm" nên để ≥ 3 phút**.

**5.3 Đồ CẤM là 77 món, nhiều món rất hay mang theo** — toàn bộ hoàn PK, Bảo rương thần bí
của Dã Tẩu, Hoàng Kim Bảo Hạp, Túi Dược Phẩm, Tín Sứ yêu bài, Bánh chưng, Hoàn Hồn Đơn…
Mang theo là bị Quan viên đuổi *"Ngươi thật to gan!…"*. Auto tự cất vào rương, **nhưng cần
mật khẩu rương ở tab Hậu cần nếu rương đang khoá**; rương đầy thì auto báo một dòng rồi thôi.

**5.4 Hạng gắn với THÀNH, không phải với nhân vật.** Biện Kinh (37) / Tương Dương (78) /
Lâm An (176) chỉ có Sứ giả **Kiệt xuất** (cấp 80-119); Phượng Tường (1) / Thành Đô (11) /
Dương Châu (80) / Đại Lý (162) chỉ có Sứ giả **Võ lâm** (cấp ≥ 120). Đứng nhầm thành là Sứ
giả từ chối. Auto **không tự đi liên thành** — nó báo một dòng rồi bỏ khung giờ đó.

**5.5 Đối thủ là NGƯỜI, lọc bằng mission group.** Trong đấu trường máy chủ đặt hai đội vào
mission camp `i` và `i+1` rồi `SetCurCamp(i%2+2)` — client nhận được `m_nMissionGroup` của
từng người nên auto phân biệt được đồng đội / đối thủ chính xác 100%, không dựa vào cờ PK.
**Hai đội xuất phát ở ĐÚNG CÙNG MỘT TOẠ ĐỘ** (`champion_gmpos.txt`) nên vào sân là chạm mặt
ngay, không cần tìm đường.

## 6. 🔴 Sáu chỗ đặc tả cũ (`AUTO_LIENDAU_SPEC.md`) SAI hoặc thiếu — đã sửa theo script sống

| # | Đặc tả cũ | Sự thật đo được |
|---|---|---|
| 1 | *"Danh sách WLLS_FORBID_ITEM (~30 mục)"* | **77 mục** (bản này đã thêm Tín Sứ yêu bài, 12 lễ bao, Túi Dược Phẩm 30557…) |
| 2 | *"Kiệt xuất mặc định config đang MỞ (gốc Linux ĐÓNG)"* | Đúng, và **cả 2 hạng đang mở** — log 18:00 hôm nay `{1,1}` |
| 3 | *"NPC 87 'Sứ giả liên đấu' (đăng ký tìm đồng đội)"* | Đúng, **và đây là bẫy**: ở 4 thành 78/162/80/11 nó đứng cách Sứ giả báo danh **dưới 20 ô**, trùng tên ⇒ hàm dò NPC "khớp đầu tiên" bấm nhầm. Đã phải viết `LD_FindNpcGan` chọn **NPC gần nhất** |
| 4 | *"Thoại NPC bắt bằng marker"* | Thoại liên đấu dùng `Describe`, không phải `Say` — may là engine ánh xạ `Describe → LuaSelectUI` nên vẫn chụp được. Máy chủ **cắt bỏ phần `/tên_hàm`** trước khi gửi ⇒ marker chỉ được dùng phần chữ hiển thị |
| 5 | *"auto suy pha từ đồng hồ hệ thống là chính"* | Có mốc chắc hơn: pha 4 phát `AddGlobalNews` ⇒ bắt bằng đường `UI_NEWSINFO` đã có sẵn |
| 6 | *"Bot server-side: có lệnh GM `wlls_player_join`"* | Đúng nhưng **không dùng** — bản này 100% client, không đụng server |

Ngoài ra xác nhận đúng: mùa theo tháng ngày 08→28; 15 phút/lượt; 4 phút chuẩn bị + 10 phút
đánh; cấp 80/120; toạ độ hội trường (1523,3024) và khu chuẩn bị (1596,2977); chết là bị kéo
về hội trường ngay (`playerdeath.lua:13`); hết trận cả sân bị kéo về hội trường
(`wlls_remove_camp`); 7 thành + 4 sân mỗi hạng.

## 6b. ĐỢT R2 (24/08 tối) — tổ đội Song đấu + thành báo danh + giao diện có màu

Theo 3 yêu cầu bổ sung của chủ game (*tự tổ đội ở NPC liên đấu khi Song đấu, tối đa 1 người;
Đơn đấu thì không tổ đội; thêm tùy chọn thành để quay về báo danh; thêm màu thêm thông tin
như auto tham khảo*). Binary `.moi_2408_liendau` đã được **build đè bằng bản r2** (mốc 19:36).

**1. Tổ đội Song đấu (pha mới `LDP_PARTY`):**
- Ô **"Song đấu: tự tổ đội với bạn diễn (1 người)"** + combo lấy **tên người chơi đang đứng
  quanh nhân vật** (bấm vào combo là game gửi danh sách lên — cùng cơ chế tab Tổ đội) +
  danh sách tối đa 8 tên (`szLDPtName`), nút Thêm/Xóa, lưu `.dat` như mọi danh sách khác.
- Tới giờ, khi đã đứng ở thành báo danh: **máy đội trưởng** (bật *Tự lập chiến đội*) đứng
  cạnh Sứ giả **mời** người trong danh sách vào tổ đội (`TeamInviteAdd`, chống spam 15 s);
  **máy bạn diễn** (tắt *Tự lập*) **nhận lời mời** (`ReplyInvite`) — nhận cả trong lúc đứng
  chờ ở Sứ giả. Đủ 2 người → đội trưởng vào mục *"Chiến đội hạng … của ta"* → bấm
  **"Để đội hữu của Ta gia nhập vào đội thi đấu này"** → xác nhận *"Đúng rồi! Đăng ký…"*
  (2 marker mới trích byte-chuẩn từ `double.lua`/`officer.lua`) → rồi mới bấm vào hội trường.
- Bạn diễn không còn bỏ cuộc khi chưa có chiến đội: cứ **8 giây bấm lại Sứ giả** chờ đội
  trưởng đăng ký mình, tối đa 3 phút. Quá **100 giây** không gặp nhau → đi báo danh một mình
  (đội 1 người vẫn thi đấu được).
- **Đơn đấu tự bỏ qua tổ đội**: bảng `g_LDLoaiThang[12]` (trích từ vòng xoay `timetable.lua`,
  đã kiểm chứng lặp đúng 12 tháng suốt sid 124-243) — tháng thể loại 5 thì `LD_CanParty` = 0.
  Đồng thời dòng *"Ta muốn lập chiến đội!"* (chỉ Đơn đấu mới có) giờ **bấm luôn** không cần
  bật *Tự lập* — không lập thì khỏi thi đấu.

**2. Ô "Báo danh ở" (8 lựa chọn):** 7 thành + *Thành đang đứng (không tự đi)* (mặc định).
Chọn thành cụ thể thì tới giờ auto **tự đi tới đó**: ưu tiên **Thần Hành Phù** (6/1/1271)
trong túi, không có thì **Xa Phu** (mục *"Những thành thị đã đi qua"* → tên thành — cùng bảng
`g_aDTSapTown` đường Dã Tẩu đang dùng), lạc map hoang thì dùng phù về thành rồi nhảy tiếp;
quá 150 giây thì bỏ khung giờ. Chọn thành **sai hạng** → auto báo và coi như *Thành đang đứng*.
Ô cũ *"Dùng phù về thành nếu chưa đúng thành"* **bỏ** (tính năng này thay thế).

**3. Giao diện thêm màu + thông tin (kiểu auto Thái Lan):**
- **Dòng trạng thái ở chân cửa sổ** (luôn hiện, chữ xanh lá đậm): auto của nhân vật đang chọn
  **đang làm gì** — `Liên đấu: đang thi đấu`, `Tống Kim: mua thuốc hậu doanh`, `Dã Tẩu: đánh
  quái nhiệm vụ`… Nguồn: trường mới `szHoatDong[48]` **ở CUỐI `IPCMainSync`** do
  `WA_HoatDong()` (CoreShell.cpp) điền mỗi nhịp đồng bộ; WAuto **kiểm `Size` trước khi đọc**
  nên chạy lẫn với Game.exe cũ không đọc rác; hàm gọi trong `KProtocolProcess.cpp` guard
  `#ifndef _SERVER` (CoreShell.cpp không biên dịch bản server).
- **Danh sách nhân vật tô màu** (NM_CUSTOMDRAW): **xanh lá** = đang tick auto, **đen** = online
  chưa bật, **xám** = mất kết nối.
- Cấp + Kinh nghiệm tab Cơ bản chuyển **màu tím**; giữ nguyên bộ màu máu đỏ / nội lực xanh /
  thể lực lục có sẵn. Cửa sổ cao thêm 6 đơn vị (354 → 360) để chứa dòng trạng thái.

**Cách dùng cho cặp Song đấu (2 máy):** máy A (đội trưởng): bật *Tự lập chiến đội* + bật ô
tổ đội + thêm tên B; máy B: **tắt** *Tự lập* + bật ô tổ đội + thêm tên A; cả hai chọn **cùng
một thành** ở *"Báo danh ở"*. Tới giờ hai máy tự gặp nhau ở Sứ giả, tự nhóm, tự đăng ký,
tự vào hội trường.

## 7. Nghiệm thu (làm theo thứ tự)

0. **Thoát hẳn Game.exe + WAuto.exe**, swap 3 binary (mục 2), mở lại.
1. Mở WAuto → hàng nút tab giờ là **lưới 4×3, nút thứ 11 "Liên đấu"**. Bấm thử **cả 11 tab**
   xem có tab nào mất chữ / chồng ô không (đây là chỗ dễ hỏng nhất của đợt này).
2. Vào tab Liên đấu, chỉnh vài ô, **đóng WAuto mở lại → giá trị còn nguyên**.
3. Nhân vật ≥ cấp 80, đứng ở **thành đúng hạng** (cấp 80-119: Biện Kinh / Tương Dương /
   Lâm An; cấp ≥120: Phượng Tường / Thành Đô / Dương Châu / Đại Lý). Bật "Bật auto Liên đấu",
   để Khung 1 = 18:00, Đi sớm 5. Tick ô auto ở dòng nhân vật.
4. **17:55** → khung chat ra `[Liên đấu] Tới giờ Liên đấu - tạm dừng việc đang làm…` →
   chạy tới Sứ giả → (nếu chưa có đội) `Đang lập chiến đội liên đấu.` → `Đã vào hội trường`.
5. Trong hội trường: cất đồ cấm (nhìn rương), rồi **18:00** báo danh →
   `Đã vào khu chuẩn bị - chờ ghép cặp.` → đứng yên 4 phút.
6. **18:04** → `Đã vào đấu trường - đánh theo cấu hình tab PK.` → đánh đối thủ, **không được
   đánh đồng đội**.
7. Chết hoặc hết trận → về hội trường → `Hết trận - về hội trường chờ lượt sau.` → **18:15**
   báo danh lượt 2.
8. **19:00** hết khung → `Xong Liên đấu - trả máy lại cho auto cũ.` → Dã Tẩu / auto cũ chạy tiếp.
9. **HỎNG cần báo:** bấm nhầm NPC "Sứ giả liên đấu" tìm đồng đội (đứng gõ hoài); lập trùng
   nhiều chiến đội; đánh nhầm đồng đội trong sân; bị mắng đồ cấm mà không tự cất; đi lung tung
   trong khu chuẩn bị (ra khỏi khu = bỏ cuộc).

## 8. Nợ kỹ thuật / chưa làm

- **Không tự rủ đồng đội** (thể loại ≥ 2 người vẫn cần người thật lập tổ đội) — xem 5.1.
- **Không tự đi liên thành** khi đứng sai hạng (chỉ dùng được Thần Hành Phù về thành nhà).
- Chưa tự đổi điểm vinh dự lấy đồ ở shop 93 ("Ta muốn đổi phần thưởng danh dự").
- Chưa dùng "Bật tắt nhận kinh nghiệm khi thi đấu" (giữ nguyên lựa chọn của người chơi).
- Chưa tự mua thuốc ở "Chủ dược điếm" trong hội trường (NPC 389, shop 53).
- Chưa test thật một trận nào — mới build sạch, chưa swap binary.

## PHỤ LỤC — nguồn dữ liệu (script SỐNG của máy chủ, đã đọc trực tiếp 24/08/2026)

`script/missions/leaguematch/{head,wlls_autoexec,wlls_gmscript,award}.lua` ·
`.../npc/{officer,signup,head,chefu,housecarl,chuwuxiang,yaoshang}.lua` ·
`.../macthtype/*.lua` (7 tệp) · `.../combat/{mission,newworld,playerdeath}.lua` ·
`.../glbmission/schedule.lua` · `.../schedule/newworld.lua` ·
`script/leaguematch/{head,timetable,wlls_config,joinmatch,gsdriver}.lua` ·
`settings/maps/championship/champion_gmpos.txt` · `Maps/WorldSet_GameServer.ini` ·
`logs/script_jx2.log` (mốc 18:00:11 ngày 24/08/2026).

*Ghi 24/08/2026 ~18:45.*
