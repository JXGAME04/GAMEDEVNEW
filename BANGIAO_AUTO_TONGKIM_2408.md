# BÀN GIAO — AUTO TỐNG KIM CHO NGƯỜI CHƠI THẬT (WAuto) · 24/08/2026

> Thi công theo đơn đặt của chủ game + đặc tả `AUTO_TONGKIM_SPEC.md`.
> **100% phía client** (WAuto.exe + Game.exe + CoreClient.dll). KHÔNG đụng một dòng nào
> phía server, cũng không đụng hệ BOT Tống Kim (`pb_Tk*`, `PB_SetTongKim`).
> **CHƯA TEST THẬT** — phải đóng game + WAuto rồi mở lại, xem mục 6.

---

## 1. Đã làm gì (theo đúng 8 gạch đầu dòng chủ game yêu cầu)

| Yêu cầu | Trạng thái | Nằm ở đâu |
|---|---|---|
| Tự tham gia Tống Kim **khi đúng giờ** | ✅ | `TK_KhungGio` + tin toàn máy chủ (mục 3.3) |
| Về map báo danh, **bấm tham gia ở NPC báo danh**, có **chọn phe** | ✅ | pha `TKP_GO/BOOK/SIGNUP`, combo *Phe* |
| Phe quá đông → **tự tới Xa Phu ở map báo danh** qua phe kia | ✅ | pha `TKP_SWAP` |
| **Tự ăn thuốc hoạt động** (thuốc bán ở *Cửa hàng điểm Tống Kim*) | ✅ | `TK_AnThuoc` — 18 loại `6/1/177..194` |
| **Tự mua máu ở NPC hậu cần** (Quân Y) bằng dòng **mua nhanh** | ✅ | pha `TKP_CAMP`, combo *Mua máu* |
| **Ưu tiên đánh Hiệu Úy / Phó Tướng / Đại Tướng** | ✅ (xem 🔴 mục 5.1) | `TK_ChonDich`, combo *Ưu tiên đánh* |
| Tới giờ mà **đang chạy Dã Tẩu → dừng Dã Tẩu**; trong map Tống Kim **không dùng Hậu cần / Di chuyển** | ✅ | `S3Client.cpp` (mục 3.2) |
| Máy Tống Kim **riêng**: mua máu, tự đi tới NPC báo danh, **ra trap** rồi ra ngoài PK | ✅ | pha `TKP_TRAP` |
| Đánh theo **cấu hình tab PK**, di chuyển theo **toạ độ như bot** | ✅ | trả về 2 → `ATYPE_PKFIGHT`; bảng `g_TKBinhA/B` |

## 2. Binary đã đổi (đã chép vào `bin\client`, **chờ khởi động lại**)

| Tệp | Mốc | Bản lùi |
|---|---|---|
| `bin\client\CoreClient.dll` | 24/08 17:16 · 2.307.584 B · CRT-TĨNH ĐÚNG | `CoreClient.dll.cu_2408_truoc_tongkim` |
| `bin\client\Game.exe` | 24/08 17:20 · 1.263.616 B · UCRT-RELEASE ĐÚNG | `Game.exe.cu_2408_truoc_tongkim` |
| `bin\client\WAuto.exe` | 24/08 17:16 · 368.128 B | `WAuto.exe.cu_2408_truoc_tongkim` |

🔴 **Bắt buộc đóng cả `Game.exe` lẫn `WAuto.exe` rồi mở lại** — lần này Game.exe CÓ đổi
(vòng lặp auto nằm trong `S3Client.cpp`), khác các đợt Dã Tẩu trước chỉ đổi mỗi DLL.

Build lại: `Core.vcxproj "Client Release" Win32` → `S3Client.vcxproj Release Win32
-p:VcpkgEnableManifest=false` → `WAuto.vcxproj Release Win32` (cây `E:\Src_Auto_Ngoai\WAuto\WAuto`).

## 3. Bản đồ mã

### 3.1 Tệp mới
- `Sources/Core/Src/KTongKimTables.h` — **SINH TỰ ĐỘNG** bởi
  `ReverseTools/gen_tongkim_tables.py`: 12 marker thoại (raw TCVN3, trích byte-for-byte từ
  chính script Lua đang chạy), 4 khung giờ, toạ độ NPC/hậu doanh/trap/8+8 điểm xuất quân,
  bảng binh đoàn 78 + 139 điểm, id vật phẩm. **Sửa lại thì chạy lại script, đừng gõ tay.**
- `ReverseTools/ins_block.py` — công cụ chèn khối mã có tiếng Việt vào tệp TCVN3.

### 3.2 Tệp sửa
| Tệp | Sửa gì |
|---|---|
| `Sources/Core/Src/CoreShell.cpp` | +900 dòng: khối `AUTO TONG KIM` (sau `HET AUTO DA TAU`), `case ATYPE_TONGKIM` |
| `Sources/Core/Src/CoreShell.h` | `ATYPE_TONGKIM` (cuối enum) |
| `Sources/Core/Src/KPlayer.h` | 22 trường `nTK*/uTK*` trong `ExtAuto` + khởi tạo |
| `Sources/Core/Src/KDaTauCap.h` | thêm `uNewsSeq` + `szNews` (chụp tin toàn máy chủ) |
| `Sources/Core/Src/KPlayer.cpp` | 6 chỗ `UI_NEWSINFO` chụp tin trước khi `TEncodeText` đổi byte |
| `Sources/Core/Src/ipc_shared.h` (+ `WAutoUI/`, cây E) | 11 trường cấu hình Tống Kim **ở CUỐI struct** |
| `Sources/S3Client/S3Client.cpp` | gọi máy TK **trước** Dã Tẩu + 15 điểm nhường quyền |
| `WAutoUI/{Resource.h,WAuto.rc,WAuto.cpp}` | tab thứ 10 “Tống Kim” (26 điều khiển, tooltip, lưu/nạp, mặc định) |

**S3Client — máy TK cầm lái thì ai nhường:** `nTK != 0` ⇒ bỏ `ATYPE_DATAU` (Dã Tẩu),
7 điều kiện phù về thành + `bOutWhenTP`, `ATYPE_MOVE`, `ATYPE_FIGHT`, `ATYPE_RETURN`
(Hậu cần), mời/vào tổ đội. `nTK == 2` ⇒ **luôn** gọi `ATYPE_PKFIGHT` (không cần bật công
tắc PK, không cần giữ phím tắt) và tự bấm nút hồi sinh dù chưa bật “Tự hồi sinh”.

### 3.3 Máy trạng thái (`TK_Process`, trả 0 = thả máy / 1 = cầm lái / 2 = trong trận)
```
TKP_OFF  → tới khung giờ (giờ máy + ô "Lệch giờ") HOẶC nghe tin máy chủ
           "Báo danh Tống Kim đã bắt đầu" → kiểm cấp 80 + còn Chiêu thư → TKP_GO
TKP_GO   → dùng Tống Kim Chiêu thư (6/1/154)              [đang ở 324/379 thì vào thẳng]
TKP_BOOK → thoại 3 dòng: chọn dòng theo phe               → đợi đổi map 324
TKP_SIGNUP → đi tới NPC báo danh phe mình → bấm "Ta muốn đầu quân..." → đợi 2,2s
           đổi map 379 = ĐƯỢC; còn ở 324 = bị từ chối IM LẶNG → thử lại,
           3 lần (nếu chọn Tự cân bằng) → TKP_SWAP; 10 lần → bỏ khung giờ
           tự cân bằng: đọc "Tống: X ... Kim: Y" ngay trong câu thoại NPC
TKP_SWAP → Xa Phu phe kia → dòng "Đến điểm báo danh phe …" → quay lại TKP_SIGNUP
TKP_CAMP → (hậu doanh) ăn thuốc → tới Quân Y → "Mua nhanh…" (hoặc mua N bình) → TKP_TRAP
TKP_TRAP → đi men theo vết trap ra trại (10 ô chéo) — server tự ném ra trận
           (10 giây đầu nó từ chối; không thì đồng hồ 90 giây cũng tự ném)
TKP_FIGHT→ ăn thuốc → chọn địch (ưu tiên quan quân, LỌC TƯỜNG) → giao `uNpcID` cho máy PK
           không có địch hợp lệ → chạy tới điểm ngẫu nhiên trong bảng binh đoàn BÊN ĐỊCH
           chết → hồi sinh về hậu doanh → TKP_CAMP; bị đá về 324 → báo danh lại
TKP_END  → (ở 324) vứt bớt bình thuốc mua nhanh cho còn ≥ 8 ô → Xa Phu
           "Trở lại chỗ lúc nãy" (hoặc "Những thành thị đã đi qua") → TKP_DONE
```
Mỗi pha (trừ FIGHT) **kẹt quá 3 phút** → bỏ khung giờ, trả máy lại cho auto cũ.
Nhận diện thế trận **không cần biến mission**: đo khoảng cách tới 2 toạ độ hậu doanh
(< 45 ô = đang trong trại); nửa bản đồ nào của mình thì bảng binh đoàn bên kia là đích.

## 4. Tab “Tống Kim” trong WAuto (hàng nút thứ 3 đã bóp còn 4 nút)

| Ô | Ý nghĩa | Mặc định |
|---|---|---|
| Bật auto Tống Kim | công tắc tổng | **TẮT** |
| 13h23 / 17h50 / 20h50 / 22h50 | 4 khung giờ | bật cả 4 |
| Đi sớm (phút) | tới điểm báo danh trước giờ | 2 |
| Lệch giờ | giờ máy chủ − giờ máy này, tính bằng **phút** (âm được) | 0 |
| Phe | Tống / Kim / **Tự cân bằng** | Tự cân bằng |
| Mua máu | **Mua nhanh (đầy túi)** / Mua theo số lượng / Không mua | Mua nhanh |
| Số bình | dùng khi chọn “Mua theo số lượng” | 20 |
| Tự ăn thuốc Tống Kim | 18 loại hoàn 3 phút | bật |
| Loại thuốc | Tất cả / Chỉ công / Chỉ thủ / Công + máu-tốc | Tất cả |
| Ưu tiên đánh | Địch gần nhất / **Hiệu Úy · Phó Tướng · Đại Tướng** | quan quân |
| Hết trận về chỗ cũ | Xa Phu “Trở lại chỗ lúc nãy” | bật |

Cấu hình lưu **nguyên struct** vào `APdata\<ID nhân vật>.dat` như cũ; 11 trường mới nằm
**ở cuối struct** + có nhánh di trú `uOldSize <= offsetof(autoData, bTongKim)` nên **file
.dat cũ đọc lên không hỏng**, tính năng mặc định TẮT.

## 5. 🔴 Bốn điều chủ game cần biết

**5.1 Hiệu Úy / Phó Tướng / Đại Tướng hiện KHÔNG spawn.** `lib_tktc.lua:
RandPThucTongKim()` đang **hardcode trả Cửu Sát cho cả hai nhánh** (kể cả giờ 22), mà
`timertask/task01.lua:83` ghi rõ *“cuu sat không add npc hay object gì hết”* ⇒ mọi trận
hiện tại **thuần PvP, không có NPC quan quân nào**. Ô “Ưu tiên đánh” đã làm sẵn theo res id
(632/638 Hiệu Úy, 634/640 Phó Tướng, 635/641 Đại Tướng) và **tự lùi về “địch gần nhất”**
khi không có; chỉ cần chủ game mở lại thể thức Bảo Vệ Nguyên Soái là nó chạy đúng ngay,
không phải sửa client. **Việc mở lại thể thức là sửa SERVER — tôi không tự đụng.**

**5.2 “Mua nhanh” mua đúng bằng SỐ Ô TRỐNG** (`quany.lua`: `CalcFreeItemCellCount()`, 1
lượng/bình) ⇒ **đầy 100% hành trang**. Theo chốt của chủ game: vẫn dùng dòng mua nhanh,
và **hết trận auto tự vứt bớt đúng loại bình nó đã mua** (Ngũ Hoa Ngọc Lộ Hoàn 1/2/0 cấp 5)
cho tới khi còn ≥ 8 ô trống, để Dã Tẩu chạy tiếp được (Dã Tẩu cần ≥ 5 ô).
Không đụng tới bất kỳ món nào khác của người chơi.

**5.3 Thuốc hoạt động chỉ dùng được TRÊN MAP TRẬN** (`forbidmap.lua: checkSJMaps` — map
375-395), nên auto không phí thuốc ở map báo danh. 18 loại + giá 50 điểm tích luỹ/viên bán
ở *Cửa hàng điểm Tống Kim* (NPC Quân Nhu Quan, shop 92 — chính là NPC chủ game nhắc tới).
**Auto chỉ ĂN, chưa tự MUA bằng điểm** — muốn tự mua nữa thì nói một tiếng, thêm được.

**5.4 Chưa đo được 2 thứ (C1/C2 trong đặc tả):** nguồn mua Chiêu Thư, và “Trở lại chỗ lúc
nãy” neo về đâu sau chuỗi Chiêu thư → trap → chết nhiều lần. Nếu neo sai ý, tắt ô “Hết trận
về chỗ cũ” là auto dùng “Những thành thị đã đi qua”.

## 6. Nghiệm thu (làm theo thứ tự)

0. **Thoát hẳn Game.exe + WAuto.exe rồi mở lại** (đợt này Game.exe có đổi).
1. Mở WAuto → thấy **nút tab thứ 10 “Tống Kim”** ở hàng nút thứ 3. Mở tab, chỉnh vài ô,
   đóng WAuto mở lại → **giá trị còn nguyên** (kiểm tra lưu `.dat`).
2. Kiểm tra tab cũ (Dã Tẩu, PK, Hậu cần…) vẫn hiện đúng — hàng nút thứ 3 đã bóp lại.
3. Bật “Bật auto Tống Kim”, tick khung giờ gần nhất, để **Phe = Tự cân bằng**, cầm sẵn
   ≥ 1 Tống Kim Chiêu thư, nhân vật ≥ cấp 80 và đã vào môn phái. Tick ô auto ở dòng nhân vật.
4. Tới giờ: khung chat phải ra `[Tống Kim] Tới giờ Tống Kim - tạm dừng việc đang làm…`
   → dùng Chiêu thư → đáp 324 → chạy tới NPC báo danh → `Báo danh xong - đang ở hậu doanh.`
5. Hậu doanh: chạy tới Quân Y → `Đã mua nhanh thuốc ở Quân Y.` → ra vết trap →
   `Đã vào chiến trường - đánh theo cấu hình tab PK.`
6. Trong trận: có địch thì đánh (theo tầm nhìn/tiếp cận tab PK), không có thì chạy sang
   nửa bản đồ bên địch. **Không được đứng đấm tường**, không được đứng im quá 5 phút
   (bị đá về 324 — nếu bị thì auto phải tự báo danh lại).
7. Chết → tự bấm hồi sinh → về hậu doanh → mua thuốc → ra trap lại.
8. Hết trận (server kéo về 324) → `Hết trận Tống Kim - đang rời điểm báo danh.` → vứt bớt
   thuốc thừa → Xa Phu → rời map → `Xong Tống Kim - trả máy lại cho auto cũ.` →
   **Dã Tẩu / auto cũ chạy tiếp bình thường**.
9. **HỎNG cần báo:** kẹt vòng lặp bấm thoại; dùng quá 1 Chiêu thư cho một trận; sau trận
   túi vẫn đầy cứng; Dã Tẩu không chạy lại; đứng im giữa chiến trường.

## 7. Nợ kỹ thuật / chưa làm

- Chưa tự **mua** thuốc Tống Kim bằng điểm tích luỹ (mới chỉ tự ăn).
- Chưa tự đổi điểm tích luỹ lấy Huy Chương / Chân Nguyên / EXP ở Quân Nhu Quan.
- **Nhặt đồ** vẫn chạy bình thường trong trận (không tắt) — nếu thấy nó kéo nhân vật đi
  lung tung khi đang đánh thì nói, chặn 1 dòng là xong.
- Chưa dùng Trinh Sát (dịch chuyển ra tiền tuyến, 500 lượng).
- Cửa sổ vào trận tính 40 phút sau mốc giờ (trận 45 phút): 5 phút cuối nếu bị đá về 324 thì
  auto đi ra luôn thay vì báo danh lại.

## PHỤ LỤC — nguồn dữ liệu (đều là script SỐNG của máy chủ, đã đọc trực tiếp)

`tinhnang/tong_kim_tcap/{lib_tktc,mobinhtk,xaphu,quany,quanquan}.lua` ·
`maps/tongkim/trap/{tongratrai,kimratrai}.lua` · `item/battles/rescript.lua` +
`sj_*wan.lua` · `startgame.lua:84-96` · `timerserver.lua:698-716` · `timertask/task01.lua` ·
`settings/{buysell.txt:93, goods.txt:633-653, item/magicscript.txt}` ·
`script/header/forbidmap.lua`.

*Ghi 24/08/2026 ~17:25.*
