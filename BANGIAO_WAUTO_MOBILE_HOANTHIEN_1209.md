# BÀN GIAO — HOÀN THIỆN WAuto MOBILE (12/09)

> Việc này làm theo bản soi `PHANTICH_WAUTO_MOBILE_CONTHIEU_1209.md` (cùng phiên).
> Nhánh làm việc: `claude/wauto-mobile-analysis-b7df3f`, worktree `D:\GAMEDEVNEW\.claude\worktrees\kind-bassi-b0f319`
> (đường ngắn `D:\wt9`), gộp `mobile-0809` tới `bc6158e4`.
> **Mọi thay đổi chỉ ở nhánh mobile:** tệp mới không nằm trong vcxproj nào; tệp dùng chung chỉ sửa trong `#ifdef JX_MOBILE`.

---

## 1. KẾT QUẢ — TRANG WAuto KHÔNG CÒN Ô CHẾT

| | Trước (B2, 11/09) | Sau |
|---|---|---|
| Điều khiển bản mobile dựng | 279 | **266** (gỡ 28 ô không dùng được, thêm 4 ô bị mất, gộp các nút danh sách vào bảng phụ) |
| Nối được vào `autoData` | 240 | **251** |
| **Vẽ ra nhưng bấm không được** | **39** | **0** |
| Ô tích bị mất hẳn (lỗi dấu phẩy) | 4 | **0** |
| Hộp chọn chiêu lọc theo loại | không | **19/19** |
| Ghi chú / hướng dẫn | 0 | **242 ghi chú + 15 trang** |

Trang hiện có: 112 ô tích · 78 ô nhập · 58 hộp chọn · 4 nút · 4 ô danh sách · 10 ô số liệu chạy thật.

---

## 2. TỪNG VIỆC ĐÃ LÀM

### 2.1. Lấy lại 4 công tắc bị mất vì một dấu phẩy (§1 của bản soi)

`android/sinh_bang_wauto.py:61` tách tham số dòng `.rc` bằng `re.findall(r'"..."|[^,]+')`; nhánh `[^,]+` ăn trước cả
dấu nháy mở khi phía trước còn khoảng trắng nên **nhãn nào có dấu phẩy thì cả dòng điều khiển bị vứt**. Nay quét từng
ký tự, tôn trọng chuỗi trong nháy kép. Kiểm chứng: `parse_rc()` ra **479** dòng thay vì **464**.

Lấy lại: `bWAMissle` (Mọi chiêu chung 1 hiệu ứng — *nhẹ máy*), `bSaveRing` (Giữ nhẫn/dây/ngọc bội cấp >),
`bJoinPtByList` (Mời nhóm, vào nhóm theo danh sách), `bHDTinSu` (Bật auto Tín Sứ) và nhãn `IDC_STRING_12_CAY`
("Cày (phút, 0 = hết lượt exp)").

### 2.2. Gỡ 28 ô không dùng được trên điện thoại (§5)

Bảng `BO_O_MOBILE` trong `sinh_bang_wauto.py` — **mã WAuto và mọi trường `autoData` giữ nguyên**, chỉ không dựng ô:

* **Ẩn cửa sổ game** (`0_HG`) — ẩn cửa sổ Windows, điện thoại không có cửa sổ.
* **Dùng phím / Phím kích hoạt PK** (`7_K`) — `bUseFKey = 1` nghĩa là "chỉ PK khi đang GIỮ phím"; điện thoại không có
  bàn phím nên `Wnd_IsPKKeyDown()` luôn 0 ⇒ **máy PK không bao giờ chạy**. `JxWAutoNoiBo` nay **ép `bUseFKey = 0` mỗi
  nhịp** để tệp `.dat` chép từ bản PC (có thể đang bật) không âm thầm làm hỏng PK.
* **6 nút Đọc tệp / Ghi tệp** — hộp thoại chọn tệp Windows.
* **19 ô thêm/bớt/xếp thứ tự của 4 danh sách** — chuyển vào bảng phụ (§2.3).

Hai thẻ **Ác chính** và **Đ.nhập** vẫn không dựng như trước (`TAB_BO`).

### 2.3. Bảng phụ — 6 danh sách + bộ chọn dài (§2, §3.2, §4)

Tệp mới `Sources/S3Client/Ui/UiCase/UiWAutoDsach.h/.cpp` + bố cục `ui\ui3\uiwauto_dsach.ini`.

* `KWndDsachWA` — ô danh sách chạm được, dòng cao 22 px cho ngón tay, cuộn bằng hai nút, chọn một dòng.
* `KUiWAutoDsach` — phủ lên vùng nội dung khung Auto (trang bị ẩn nên không nhận chạm xuyên qua), **bốn chế độ**:

| Chế độ | Dùng cho |
|---|---|
| 0 · sửa danh sách | **Toạ độ đi tuần** (24), **Lọc thuộc tính** (40), **Không nhặt theo tên** (60), **Mời/vào nhóm** (24), **Bạn diễn Liên đấu** (8), **Thứ tự ngũ hành** (5) |
| 1 · chọn nguồn | 40 dòng thuộc tính · tên món trong hành trang · tên người chơi quanh đây |
| 2 · chọn một mục | thay `KPopupMenu` khi danh sách **≥ 10 dòng** (menu không cuộn được) |
| 3 · hướng dẫn | §2.7 |

Bốn ô danh sách trong thẻ nay là **một dòng tóm tắt** ("3 mục: 1. 1234/5678, ...") + nút **Sửa**.
Hai hộp thoại con của bản PC (`IDD_NOPICK_DIALOG`, `IDD_SETSERIES_DIALOG`) dùng chung bảng này, không port riêng.

### 2.4. Lọc danh sách chiêu theo 7 bảng con của WAuto (§3.1)

`GetAllSkillByType` chỉ trả **một** danh sách phẳng; WAuto.exe chẻ thành 7 bảng con (`WAuto.cpp:3057-3097`).
Nay bộ sinh ghi cột `nLocChieu` (`WA_LC_K/S/SE/BP/A`) cho **19** hộp chọn chiêu và `WA_ChieuHop()` lọc lại đúng
điều kiện của WAuto: vòng sáng `bAura`; buff phe ta `(bState || nStyle==2) && bAlly && !bAura`; trạng thái lên địch
`bState && !bAlly`; chiêu đánh `nStyle <= 1 && !bAlly`; chiêu kết hợp = mọi chiêu chủ động trừ vòng sáng.
Trước đây cả 19 hộp mở cùng một danh sách ⇒ chọn được chiêu sai khe, máy auto nhận mã chiêu vô nghĩa mà không báo gì.

### 2.5. Ba hộp chọn có luật riêng + hộp "Theo sau" (§2.2)

| Hộp | Luật |
|---|---|
| **Rương cửa** (Tống Kim) | danh sách **đổi theo thành ở "Hết trận về"** (`s_WATKRuongCo[7][5]` lấy từ `WAuto.cpp`); giá trị lưu là **mã hướng** 0..4, dòng cuối "Gần nhất (tự chọn)" = 5. Thành không có hướng đang lưu thì **tự lùi về "Gần nhất"** như `WA_NapRuongHuong` bên PC |
| **Boss cố định** (Sát thủ) | 20 tên, giá trị = **141 + dòng** |
| **Dòng thuộc tính** (Nhặt đồ) | 40 dòng `g_MagicTable`, dùng trong bảng phụ |
| **Theo sau** (Di chuyển) | giá trị là **tên** (`szFollName`), danh sách lấy người chơi đang đứng quanh |

### 2.6. Bốn hàm mới trong CoreShell — không cần vòng IPC (§2.2 nhóm B)

Bản PC phải gửi `PRT_GETTEAMAROUND` / `PRT_GETITEMNAME` rồi chờ `PRG_*` quay lại. Trong game gọi thẳng:

| Hàm (`CoreShell.cpp`, trong `#ifdef JX_MOBILE`) | Dùng cho |
|---|---|
| `JxCore_WAutoSoLieu` | 7 ô số liệu thẻ Cơ bản (cùng nguồn `KProtocolProcess:2073` điền `IPCMainSync`) |
| `JxCore_WAutoViTri` | hai nút **Lấy** (bản đồ / toạ độ) và nút **Lấy ở đây** của danh sách toạ độ |
| `JxCore_WAutoTenQuanhDay` | `ATYPE_GETAROUNDNAME` — Theo sau, Tổ đội, Liên đấu |
| `JxCore_WAutoTenVatPham` | `ATYPE_GETITEMNAME` — bảng "Không nhặt theo tên" |

### 2.7. Ghi chú và hướng dẫn (§3.6)

`android/sinh_ghichu_wauto.py` rút **242 ghi chú** (`s_aTips`, bỏ 41 ô không có trên mobile) + **15 trang hướng dẫn
theo thẻ** (`s_aNote`) ra `ui\ui3\uiwauto_ghichu.ini` (TCVN3, 35 KB). Nút **[?]** cạnh nút Đóng mở bảng phụ chế độ 3:
trang hướng dẫn của thẻ đang xem rồi từng ô một ("nhãn: ghi chú"), tự xuống dòng theo bề ngang ô, cuộn được.

### 2.8. Những chỗ còn lại

* **10 ô số liệu** (§3.3) điền lại mỗi 500 ms trong `Breathe`: sinh lực / nội lực / thể lực / bản đồ / toạ độ /
  đẳng cấp / kinh nghiệm (thẻ Cơ bản) và bản đồ + điểm đã đặt (thẻ Di chuyển). Kinh nghiệm trả bằng **chữ** vì
  `m_nExp` là `double`, cấp cao vượt `int`.
* **Nhớ Bật/Tắt theo nhân vật** (§3.4): tệp `APdata\<id>.bat` 1 byte. **Không** thêm trường nào vào `autoData`
  (thêm vào giữa struct là lệch offset mọi tệp `.dat` của người chơi PC).
* **Cặp radio "Ưu tiên"** (§3.5): Khoảng cách = `nPriority` 0, Ngũ hành = 1; bấm cái này thì cái kia tự tắt
  (`DienTick()` đọc lại cả thẻ sau mỗi lần bấm).
* **Lớp vẽ riêng đè lên khung** (§3.7, bẫy 3 của B1): khung Auto đang mở thì không vẽ **cụm nút kỹ năng** và
  **icon NPC**. Cần điều khiển vẫn vẽ để còn đi được (máy auto đã nhường khi đang cầm cần — `WA_TAY_CAM`, B2 g).
* **Chạm vào chữ** (§3.7): nhãn cạnh ô tick chạm được (bật/tắt), nhãn cạnh hộp chọn chạm được (mở hộp).

---

## 3. TỆP ĐÃ ĐỤNG

| Tệp | Việc | Rào |
|---|---|---|
| `android/sinh_bang_wauto.py` | sửa bộ tách `.rc`; 5 bảng tay: `NOI_TAY` (7 ô), `BO_O_MOBILE` (28), `VIEC` (14), `LOC_CHIEU`, `SO_LIEU`; rút `g_MagicTable` + `s_aTKRCo` | Python |
| `android/sinh_bocuc_wauto.py` | 2 cột mới trong bảng C (`nLocChieu`, `nViec`), loại `WA_MUC_SOLIEU`, ô danh sách → tóm tắt + nút Sửa, nối nhãn cho hộp chọn, `#ifdef JX_MOBILE`, không còn ô nhóm rỗng | Python |
| `android/sinh_ghichu_wauto.py` (**mới**) | 242 ghi chú + 15 trang hướng dẫn → `uiwauto_ghichu.ini` | Python |
| `Sources/S3Client/Ui/UiCase/UiWAutoDsach.h/.cpp` (**mới**, 240 + 830 dòng) | bảng phụ 4 chế độ | `#ifdef JX_MOBILE`, không trong vcxproj |
| `Sources/S3Client/Ui/UiCase/UiWAutoTrang.h/.cpp` | lọc chiêu, việc của nút / hộp chọn, ô số liệu, tóm tắt danh sách, nhãn chạm được | như trên |
| `Sources/S3Client/Ui/UiCase/UiWAuto.h/.cpp` | ghép bảng phụ (vẽ sau cùng = nằm trên), nút `[?]`, đóng bảng phụ khi đổi thẻ / đóng khung | như trên |
| `Sources/S3Client/Ui/UiCase/UiWAutoBang.h` | **sinh bằng máy** | `#ifdef JX_MOBILE` |
| `Sources/S3Client/Platform/JxWAutoNoiBo.h/.cpp` | nhớ Bật/Tắt; ép `bUseFKey = 0` | chỉ mobile |
| `Sources/Core/Src/CoreShell.cpp` | 4 hàm `JxCore_WAuto*` | **trong khối `#ifdef JX_MOBILE` sẵn có** (dòng 25046) |
| `Sources/S3Client/Ui/UiShell.cpp` | không vẽ cụm kỹ năng / icon NPC khi khung Auto mở | `#ifdef JX_MOBILE` |
| `android/CMakeLists.txt` | thêm `UiWAutoDsach.cpp` | Android |
| `android/du_lieu_ghi_de/ui/ui3/` | 15 `uiwauto_tab*.ini` sinh lại + `uiwauto.ini` (nút `[?]`) + `uiwauto_dsach.ini` + `uiwauto_ghichu.ini` | lớp ghi đè |
| `android/va_nguon_android_wauto20..29.py` (**mới**, 10 kịch bản) | các bản vá, chạy lại vô hại | |
| `Sources/S3Client/S3Client.cpp`, `Sources/Engine/Src/KDebug.h` + `.cpp` | vá hai lỗi dựng Windows của phiên khác (§6.1) | không rào: sửa cho đúng ở CẢ HAI bản |

**Sinh lại từ đầu (đúng thứ tự):**

```bash
python android/sinh_bang_wauto.py && python android/sinh_bocuc_wauto.py && python android/sinh_ghichu_wauto.py
```

rồi `va_nguon_android_wauto20.py` → `21` → ... → `29`, chép
`android/du_lieu_ghi_de/ui/ui3/uiwauto*.ini` sang `D:\jx1_android_data\ui\ui3\`, dựng APK.

---

## 4. ĐÃ ĐO

### 4.1. Dựng

| Việc | Kết quả |
|---|---|
| Dịch thử từng tệp bằng clang của NDK (`-fsyntax-only`, arm64) | **0 lỗi** cho cả 7 tệp đụng tới |
| `libmain.so` arm64 + x86_64 (CMake + Ninja, 727 đích) | **exit 0** |
| APK hai ABI (`gradlew assembleDebug`) | **BUILD SUCCESSFUL**, 20,6 MB; đã soi `libmain.so` trong APK có chuỗi mới ở **cả hai** ABI |
| **Chuỗi Windows x64** (Release, x64) | **4/4, 0 lỗi** |
| **Chuỗi Windows SDL** (ReleaseSDL, x64) | **6/6, 0 lỗi** |
| Ký hiệu **dữ liệu** toàn cục trùng giữa các `.so` (luật §0.4) | **2** (`IID_IESClient`, `IID_IClientFactory` — có sẵn từ trước; mã mới không thêm cái nào) |
| Bố cục 15 thẻ | đều **2 cột**, không cảnh báo hết khe, không còn ô nhóm rỗng, không còn ô đè lên nhãn |

### 4.2. Chạy thật trên LDPlayer (nhân vật `TestBot` Lv120, Tương Dương)

| Thử | Kết quả |
|---|---|
| Mở khung Auto | đủ 4 nhóm + hàng thẻ; **cụm nút kỹ năng và icon NPC không còn đè lên khung** |
| **Cơ bản** | Sinh lực `9497/13910` · Nội lực `834/834` · Thể lực `1705/1705` · Bản đồ **Tương Dương** · Toạ độ `47168 / 101664` · Đẳng cấp `120` · Kinh nghiệm — **số sống, 500 ms/lần, thẳng cột**; ô **"Mọi chiêu chung 1 hiệu ứng, dòng:"** đã có |
| **Phục hồi** | 19 tick + 11 ô nhập + bảng "Mức 1 / Mức 2 / 1-1000" thẳng hàng |
| **Di chuyển** | "Theo sau (không theo ai)"; "Bản đồ **Tương Dương** [Lấy]"; ô danh sách thành **"1 mục: 1. 47168 / 101664" + [Sửa]** |
| **Bảng phụ · Toạ độ đi tuần** | [Lấy ở đây] thêm 2 dòng → chọn dòng → [Xóa] còn 1 → [Đóng] → dòng tóm tắt trong thẻ đổi theo |
| Ghi vào tệp | `APdata\3758889385.dat`: `nCoordCount = 1`, `sMoveCoord[0] = (47168, 101664)`, `nMoveMapId = 78`, `szMoveMap = Tương Dương` |
| **Lọc chiêu** | "Vòng sáng #1" mở ra **đúng 6 vòng sáng** của Nga My (Mộng Điệp, Lưu Thủy, Phật Tâm Từ Hữu, Thanh Âm Phạn Xướng, Phổ Độ Chúng Sinh, Bế Nguyệt Phất Trần) + "Không thiết lập" — trước đây là cả danh sách chiêu |
| **Bảng chọn dài** | "Đi bản đồ" (31 dòng) mở **bảng phụ cuộn được**, đếm `1-10/31`; chọn "Trường Bạch Sơn Bắc" → `.dat` ghi `nSelMap = 1` |
| **Nút [?]** | "Hướng dẫn: Hậu cần" — trang note 7 mục rồi từng ô một ("Sửa đồ trên bãi: ...", "Rút tiền: ..."), đếm `1-10/43`, cuộn được |
| **Nhớ Bật/Tắt** | bật → `APdata\<id>.bat` = 1; khởi động lại: `config.ini [WAuto] Bat=0` nhưng log `nho trang thai lan truoc: Bat=1` → `[WAUTO] BAT` + `[WAUTO] nhip: ...` |
| Sập | `jx_crash.log` **không có**; `logcat` sạch sau khi sửa (§5) |

---

## 5. HAI LỖI TÌM RA KHI THỬ — ĐÃ SỬA

### 5.1. Hộp chọn tĩnh luôn hiện dòng đầu (lỗi của đợt này)

`DienChon` bản 11/09 đọc `v = LayInt(p)` ngay đầu hàm. Bản 12/09 tách `LayInt` vào từng nhánh cho ba hộp có luật
riêng (Boss `141+i`, Rương cửa theo thành, Theo sau là chuỗi) nhưng **quên nhánh chung** ⇒ mọi hộp chọn tĩnh hiện
`pLuaChon[0]` bất kể giá trị đang lưu.

Đo 12:29: chọn "Trường Bạch Sơn Bắc" → `.dat` ghi `nSelMap = 1` **đúng**, nhưng ô vẫn hiện "Hắc Sa động". Riêng thẻ
Hậu cần có **4 ô** sai cùng lúc (Đi bản đồ, Mua sinh lực, Mua nội lực, Mua giải độc). Sửa ở
`android/va_nguon_android_wauto29.py`; đo lại 12:33: cả 4 ô hiện đúng giá trị đang lưu.

### 5.2. Ô số liệu đè lên nhãn của chính nó

Hàng kiểu "bảng" đặt ô từ cột chung `col1` với bề rộng cố định `EDIT_W` (44). Nhóm "Nhân vật" không có hàng kiểu
"nhãn + điều khiển" nên `col1 = 6` ⇒ ô số liệu vẽ chồng lên nhãn, đọc ra `S1s910/1 Bản đồ`. Sửa: hàng có ô số liệu
đặt theo **bề rộng thật** của từng ô và bắt đầu sau nhãn, dùng **một cột chung cho cả nhóm** để các hàng thẳng nhau.

---

## 6. BA CHỖ CỦA PHIÊN KHÁC CHẶN ĐƯỜNG — PHẢI BÁO

> Cả ba chỗ dưới đây **không phải** của đợt WAuto. Ghi lại theo luật "soi chéo mã dùng chung" (12/09).

### 6.1. `[BAOMAT 12/09]` làm gãy **cả hai** chuỗi dựng Windows

Commit `1a400b7e` thêm khối đọc `[Client] NhatKyChanDoan` vào `Sources/S3Client/S3Client.cpp`. Bản Android vẫn dựng
được (mọi thứ là `.so` dùng chung, có sẵn SDL), nhưng Windows thì:

| Cấu hình | Lỗi |
|---|---|
| Release x64 (client PC phát hành, **không có SDL**) | `error C3861: 'SDL_Log': identifier not found` |
| ReleaseSDL x64 (GameSDL.exe) | `error LNK2001: unresolved external symbol "int g_nJxNhatKyChanDoan"` — biến nằm trong `Engine.dll` mà **không có `ENGINE_API`** |

Đã vá tối thiểu (`android/va_nguon_android_wauto28.py`): rào `SDL_Log` bằng `#ifdef JX_PLATFORM_SDL`, và thêm hàm
**xuất** `g_SetNhatKyChanDoan(int)` trong `KDebug` thay cho việc chạm thẳng vào biến. Sau khi vá: x64 **4/4**,
SDL **6/6**, 0 lỗi. Nhờ phiên `[BAOMAT]` soi lại cách vá này.

### 6.2. `Rep3AtlasKhoi=1` làm **sập game** khi chữ đổi liên tục

Đo 12:15-12:26: mở thẻ Cơ bản là sập sau khoảng 145 ms, `signal 11 (SIGSEGV)` địa chỉ rác kiểu `0x?00000029`.
Dò bằng `llvm-addr2line` ra đúng dòng:

```
Sources/Represent/Represent3/D3D9onGPUDev.cpp:1553
SDL_BindGPUFragmentSamplers(pass, 2, tk, 12);   // [KHOI2 11/09]
```

tức khối **atlas theo khối** của đợt tối ưu vẽ. Thẻ Cơ bản là chỗ đầu tiên trong game có **chữ đổi mỗi 500 ms**
(sinh lực / nội lực / toạ độ) nên lòi ra; một ô đổi thì chưa sao, hai ô trở lên là sập.

* `Rep3AtlasKhoi` **mặc định là 0** trong mã nguồn (`KRepresentShell3.cpp:752`), nhưng `D:\jx1_android_data\config.ini`
  của máy ảo đang để **1**.
* Đặt `Rep3AtlasKhoi=0` → thẻ Cơ bản chạy mượt, không sập (đo 12:26 và mọi lần thử sau).
* **Tôi đã đặt 0 trong `D:\jx1_android_data\config.ini`** và giữ bản cũ ở `config.ini.wauto_bak`. Không dám để 1 vì
  đó là thiết lập làm sập game.

### 6.3. Không giữ lại được APK cũ trên máy ảo

Máy ảo dùng chung với phiên khác. Lệnh `adb pull` bị Git Bash đổi đường dẫn `/data/...` thành
`C:/Program Files/Git/data/...` nên bước sao lưu im lặng thất bại; lúc phát hiện thì bản cũ đã bị thay.
Dựng lại từ nhánh của phiên đó là ra. Từ nay đặt `MSYS2_ARG_CONV_EXCL='/data'` trước lệnh.

---

## 7. CÒN LẠI

* Chủ rà từng thẻ: tiêu đề nhóm vẫn là `TIEU_DE_TAY` do bộ sinh đặt.
* Chưa thử trên **điện thoại thật**.
* Chưa commit: mọi thay đổi đang nằm trong worktree `kind-bassi-b0f319` (nhánh `claude/wauto-mobile-analysis-b7df3f`).

---

## 8. BẪY GẶP TRONG ĐỢT NÀY (ghi cho người sau)

| # | Bẫy | Cách tránh |
|---|---|---|
| 1 | Tách dòng `.rc` bằng `re.findall` có nhánh `[^,]+`: nhánh đó ăn cả dấu nháy mở khi trước nó còn khoảng trắng ⇒ **mất im lặng** mọi điều khiển có dấu phẩy trong nhãn | quét từng ký tự, gặp `"` thì nhảy tới `"` đóng |
| 2 | `parse_options` giữ nguyên hộp chọn đang theo khi gặp `GetDlgItem(hDlg, IDC_COMBO_14_S1 + cki)` (không khớp mẫu) ⇒ 20 tên boss bị thay bằng danh sách chiêu của vòng lặp kế | thấy `GetDlgItem(` mà không khớp thì **quên** hộp đang theo |
| 3 | `io.open(p, "w", newline=...)` sai tham số **báo lỗi SAU khi tệp đã bị cắt trắng**; bản vá của tôi đã làm rỗng `sinh_ghichu_wauto.py` | kiểm tham số trước khi mở để ghi, hoặc ghi ra tệp tạm rồi đổi tên |
| 4 | Thêm một widget vào khung làm tràn hai mảng `[WA_UI_SO_NHOM + WA_UI_TAB_MOI_NHOM + 3]` của `AnDuoiMenu` | đếm lại mảng mỗi khi thêm widget |
| 5 | `#include "UiCase/UiWAuto.h"` của `UiShell.cpp` nằm ở **dòng 1074**, không dùng được cho khối vẽ ở dòng 375 | thêm một dòng include ở đầu tệp (có rào) |
| 6 | Bỏ một điều khiển khỏi bộ sinh làm khoảng cách hàng vượt `GAP_NHOM` ⇒ sinh ra **ô nhóm rỗng chỉ có tiêu đề** | nhóm vừa mở bằng dòng tiêu đề mà chưa có hàng nào thì không mở nhóm mới |
| 7 | Heredoc của công cụ Bash bóp hai dấu chéo ngược thành một — biểu thức chính quy viết qua heredoc hỏng ngầm (bẫy cũ của B2, gặp lại 3 lần) | sửa kịch bản bằng Write/Edit, chỉ *chạy* bằng Bash |
| 8 | `adb pull /data/...` trong Git Bash bị đổi thành `C:/Program Files/Git/data/...` | đặt `MSYS2_ARG_CONV_EXCL='/data'` trước lệnh |
| 9 | Kịch bản chuỗi dựng bản worktree tắt `PostBuildEventUseInBuild` ⇒ `Engine.lib` mới **không vào `Lib\`**, S3Client link bản cũ, báo lỗi link khó hiểu | chỉ tắt cho cấu hình MÁY CHỦ (chép ra ngoài kho); Release x64 và ReleaseSDL x64 chép trong kho, phải giữ |
| 10 | Đặt tên kịch bản vá theo số đếm chung: tôi **ghi đè nhầm** `va_nguon_android_wauto8.py` và `9.py` của bước B2 i (đúng lỗi đã ghi ở §2 bản giao B0) | trước khi tạo, `ls` xem số nào đã có; của tôi đổi sang dải `20..29` |

---

## 9. LÀM LẠI Ô DANH SÁCH (13/09) — thanh cuộn thật, vuốt để cuộn, có chiều sâu

Chủ yêu cầu: *"làm các ô tuỳ chọn vật phẩm lại, đẹp hơn, có hiển thị nút cuộn thay vì nút chữ,
vuốt lên xuống để cuộn, thẩm mỹ hơn có chiều sâu hơn"*. Bản vá `android/va_nguon_android_wauto30.py`
và `android/va_nguon_android_wauto31.py`.

### 9.1. Vuốt để cuộn — không phải viết mới

`KSdlApp.cpp` (`[ANDROID 09/09 CUON]`) đã đổi **vuốt dọc trên giao diện** thành `WM_MOUSEWHEEL`
(`MAKEWPARAM(0, ±WHEEL_DELTA)`, mỗi **28 px** một nấc) sau khi kéo con trỏ về đúng điểm chạm đầu;
`Wnds.cpp:448` gửi thẳng `WM_MOUSEWHEEL` cho `pMouseOverWnd->WndProc`. Nên `KWndDsachWA` **chỉ cần
bắt `WM_MOUSEWHEEL`** là đi chung đường với khung chat / thoại NPC / danh sách máy chủ:

```cpp
if (uMsg == WM_MOUSEWHEEL)
{
    int zDelta = (int)(short)HIWORD(uParam);
    if (zDelta) Cuon(-zDelta / WHEEL_DELTA);
    return 1;
}
```

Hai nút chữ `[Cuon0]` / `[Cuon1]` bị **bỏ hẳn** (khỏi lớp, khỏi `AddChild`, khỏi
`uiwauto_dsach.ini`); ô `[Dsach]` rộng ra **560 → 660** vì không còn phải chừa chỗ cho nút.

### 9.2. Vẽ có chiều sâu (`KWndDsachWA::PaintWindow`)

| Lớp | Cách vẽ |
|---|---|
| bóng đổ | hai hình chữ nhật đen lệch +2 và +4 px về phải‑dưới |
| lòng ô lõm | nền nâu rất tối `(30,26,20)`; mép **trên‑trái tối** (bóng hắt vào), **dưới‑phải sáng** (`WAD_Khoi(..., 0)`) |
| viền | hai lớp: viền tối ôm ngoài + viền vàng ấm bên trong |
| dòng chẵn/lẻ | dòng lẻ phủ một lớp xám xanh rất nhạt (alpha 28) để đếm dòng đỡ mỏi mắt |
| dòng đang chọn | dải vàng ấm **ba lớp** (đậm giữa, nhạt dần ra mép = giả độ cong) + viền sáng + **vạch đánh dấu** bên trái |
| gợi ý còn nội dung | dải mờ dần 5 px ở mép trên / mép dưới khi còn dòng bị che |
| thanh cuộn | **rãnh lõm + con trượt nổi** ở mép phải, cao theo tỉ lệ phần đang thấy, chỉ vẽ khi `m_nSo > SoDongHien()` |

### 9.3. Ba lỗi tìm ra khi **đo thật** trên LDPlayer (11:10 → 11:20)

1. **Dòng hướng dẫn bị cắt ngang** — không phải do bề ngang ô (660 px chứa được 110 byte) mà do
   `m_Huong` là `KWndText80`: **đệm chữ chỉ 80 byte**, các câu hướng dẫn dài 90–102 byte.
   → đổi sang `KWndText256`.
2. **Chữ trang hướng dẫn xuống dòng sớm** — `XuongDong` lấy `nMax = (rộng − 12) / 6 = 108` byte nhưng
   bị chặn bởi `WA_DS_CHU − 1 = 95` ⇒ thừa ~90 px bên phải mỗi dòng.
   → nâng `WA_DS_CHU` **96 → 112** (mọi chỗ ghi vào `autoData` đều `strncpy` có chặn nên không tràn).
3. **Viền lõm vẽ rồi mà không nhìn ra** — lòng ô đang là `(14,13,11)` gần như đen nên bóng đen hắt vào
   trùng màu nền. → nâng nền lên `(30,26,20)`.

### 9.4. Nghiệm thu trên LDPlayer (13/09, bản `155833a5dd436e3ec3b27da800f3fa1e`)

| Việc | Kết quả |
|---|---|
| Thẻ Nhặt đồ → *Cài đặt tên không nhặt* | mở bảng phụ, ô có viền vàng + lòng lõm, dòng đang chọn là dải vàng có vạch trái |
| *(chọn)* → **Chọn món trong hành trang** (13 món) | hiện 10 dòng + **thanh cuộn** ở mép phải, con trượt cao 10/13 rãnh |
| vuốt lên trong ô | cuộn xuống, con trượt trượt theo, **chặn đúng** ở dòng 3 (13 − 10) |
| vuốt xuống | về đầu danh sách, con trượt về đỉnh |
| bấm một dòng **sau khi đã cuộn** | trả đúng món đã bấm (*Đại Bách Luyện Thành Cương*), không lệch chỉ số |
| nút **?** → trang hướng dẫn (~200 dòng) | cuộn mượt, con trượt nhỏ đúng tỉ lệ, chữ dùng hết bề ngang ô |
| dòng hướng dẫn chân bảng | hiện **đủ cả câu** |

Vẫn phải giữ `Rep3AtlasKhoi=0` trong `D:\jx1_android_data\config.ini` (để 1 là sập ở
`D3D9onGPUDev.cpp:1553`, xem §6.2).

---

## 10. TÊN Ô TRONG BẢNG CHỌN + CHỮ TRONG HỘP CHỌN BỊ CẮT (13/09)

### 10.1. Bảng chọn dài đề "Chọn một mục" thay vì tên ô

Đo 11:25: thẻ Hậu cần, bấm hộp **Đi bản đồ** → bảng phụ đề *"Chọn một mục"*.

Gốc: hàng đó là `[v] Đi bản đồ   [hộp chọn]` — chữ "Đi bản đồ" là **nhãn của ô tick**, mà bộ sinh chỉ cho hộp
chọn mượn nhãn **riêng** cùng hàng (`nhan_gan` bỏ qua nhãn của tick) ⇒ `nKheNhan = 255` ⇒ rơi về câu chung.

* `sinh_bocuc_wauto.py`: thêm `nhan_tick`; hộp chọn không có nhãn riêng thì dùng chung nhãn của tick cùng hàng.
  Sau khi sinh lại: **58 hộp chọn, chỉ còn 4 cái không có tên** (hàng không có chữ nào).
* Một nhãn nay có thể ứng với **cả** tick lẫn hộp chọn ⇒ vòng tìm trong `WndProc` phải **quét hai lượt, ô tick
  được ưu tiên** (`va_nguon_android_wauto32.py`). Trước đó ăn theo thứ tự bảng — đúng nhưng là đúng nhờ may mắn.
* Nghiệm thu: tiêu đề đổi thành **"Đi bản đồ"**; bấm vào chữ "Đi bản đồ" vẫn **bật/tắt ô tick** (không mở hộp).

### 10.2. Chữ trong hộp chọn bị cắt — 20 hộp

Soát bằng máy (`scratchpad/soat_chon.py`: so dòng dài nhất của từng hộp với bề ngang ô):
**20/58 hộp** có dòng dài hơn ô ⇒ chữ chạy xuống dưới mũi tên (`Tự cân bằng (bên ít người)`,
`Hiệu úy / Phó Tướng / Đại Tướng`, `Thành đang đứng (Xa Phu gần nhất)`…).

Gốc: hộp chọn chỉ có **ba cỡ** 120 / 160 / 200 và `rong_chon()` lấy đúng một cỡ rồi thôi, không nới thêm dù
hàng còn chỗ. Ba việc:

| Việc | Chỗ sửa |
|---|---|
| Nấc bề ngang **20 px**: 120…240 | `CHON_RONG` ở `sinh_bocuc_wauto.py` **và** `anh_wauto_vnku.py` (sinh thêm `hop_chon_140/180/220/240.spr`) |
| Hàng còn chỗ thì **nới** hộp chọn ra cho đủ chữ | vòng nới trong `co_lai_chon()` (trước chỉ có vòng **co**) |
| Hàng vẫn thiếu chỗ thì **bỏ cột chung riêng hàng đó** (hộp chọn bắt đầu ngay sau nhãn của nó) | nhánh `lh == "nhan"` trong `xep_nhom()`; `x_thu_hai` vẫn giữ `col1` nên các hàng khác không lệch theo |

Kết quả soát: **20 → 7** hộp còn thiếu chỗ theo chuẩn chặt (`chữ + 34`), và **20 → 3** theo chuẩn thật
(chữ chạm mũi tên, `chữ + 26`). Ba cái còn lại thiếu ~24 px, đều là tên vật phẩm dài
(`Cửu Chuyển Hoàn Hồn Đơn`, `Hoắc Hương Chính khí hoàn`, `Thành đang đứng (Xa Phu gần nhất)`) — hàng đó còn
ô nhập số bên phải nên không nới thêm được.

Nghiệm thu trên máy: Tống Kim (*Tự cân bằng (bên ít người)*, *Hiệu úy / Phó Tướng / Đại Tướng*), CTC
(*Điểm lưu (Rời khỏi đấu trường)*), Liên đấu (*Thành đang đứng (không tự đi)*), H.động
(*Tự chọn (đài 2 - né đài chủ)*), Dã Tẩu (*Ưu tiên 100 mảnh SHXT*), Chiến đấu, Nhặt đồ, Chiêu KH — **hiện đủ chữ**.

### 10.3. Bảng chọn dài tô sáng lệch — không còn

Thử lại bằng danh sách chiêu (12 dòng, ô hiện 10): chọn dòng cuối *Băng Vũ Lạc Tinh*, đóng, mở lại —
danh sách **tự cuộn tới** và tô đúng dòng đó. Lỗi cũ là của bản hai nút cuộn, bản viết lại §9 đã hết.

### 10.4. BẪY: `anh_wauto_vnku.py` không sinh lại đúng `auto_m.spr`

Chạy `python android/anh_wauto_vnku.py` (để sinh thêm ảnh hộp chọn) **ghi đè luôn**
`android/du_lieu_ghi_de/spr/uinew/uitoolscontrolbar/auto_m.spr`: bản trong kho 3 272 byte, bản sinh ra
5 814 byte. Bản trong kho là bản đã chỉnh tay của đợt khác. **Đã trả lại bằng `git checkout`** (cả hai cây).
Lần sau chạy bộ sinh ảnh thì soát `git status` ngay sau đó.

---

## 11. NÚT "ĐÓNG" VÀ "?" TRÊN THANH TIÊU ĐỀ KHUNG AUTO (13/09)

Chủ: *"sửa nút đóng lại ở Auto cho đẹp"*.

**Trước:** `m_Dong` / `m_TroGiup` là `KWndPureTextBtn` = **chữ trần** màu trắng nằm trơ trọi trên thanh đá,
nhìn như chưa làm xong.

**Sau:** `KWndLabeledButton` có ảnh, cùng chất liệu ngọc + hoa văn vàng của bộ ảnh UiAutoNew.

| Ô | Ảnh | Nguồn |
|---|---|---|
| Đóng 64×24 | `nut_dong.spr` | **nút Đóng có sẵn** của bộ UiAutoNew: `UiAutoNew\CloseSeriesBtn.spr` 236×86, thu nguyên ảnh (tỉ lệ 2,74 ≈ 2,67 → không méo), chữ "Đóng" vẽ sẵn trong ảnh nên **không đặt Label** |
| ? 28×24 | `nut_hoi.spr` | hai đầu hoa văn của cùng ảnh + khúc giữa **sạch chữ** (cột 44–71; chữ nằm ở cột 75–161) kéo giãn; chữ "?" vẽ bằng Label |

Ba khung của ảnh gốc: 0 ngọc sáng chữ trắng (thường) · 1 chữ vàng (đang bấm) · 2 xám (tắt) →
ini `Up=0 Down=1 DisableFrame=2`.

Đã thử bốn kiểu dán lên **đúng thanh tiêu đề chụp từ máy** trước khi làm: chip cắt từ `btn_noname.spr`
(chất liệu nút tab) ra **hộp đen** vì khung thường của ảnh đó gần như đen → bỏ.

Tệp đụng: `android/anh_wauto_vnku.py` (hàm mới `nut_dong_hoi()`), `android/va_nguon_android_wauto33.py`
(đổi lớp hai nút trong `UiWAuto.h/.cpp` + mục `[Dong]`/`[TroGiup]` của `uiwauto.ini`).
`UiWAuto.cpp/.h` chỉ nằm trong `android/CMakeLists.txt`, không vcxproj nào → bản PC không đổi.

⚠️ Sinh lại ảnh: **chỉ gọi riêng** `python -c "import sys; sys.path.insert(0,'android'); import anh_wauto_vnku as a; a.nut_dong_hoi()"`.
Chạy cả tệp là dính bẫy §10.4 (ghi đè `auto_m.spr`).

### 11.1. Đã kiểm tới đâu

* Dựng + cài APK `668d193601e8065b11c6fca944c7c308` (MD5 trên máy khớp).
* **Hình:** ảnh chụp máy ảo 14:04 thấy đúng hai nút mới trên thanh tiêu đề, nét rõ, không lệch.
* **Bấm:** kiểm bằng **mã**, CHƯA chạm thử trên máy — `KWndButton` gửi `WND_N_BUTTON_CLICK` cho khung cha
  (`WndButton.cpp:363, 387`), `KUiWAuto::WndProc` so `uParam == &m_Dong / &m_TroGiup` như cũ.
  Lý do chưa chạm: lúc 14:03:32 game trên máy ảo đã được **người khác mở lại** (phiên `wauto-41` vừa mở
  6 phút trước) và khung Auto đang mở ở thẻ Cài đặt — không chen vào. Đã nhắn phiên đó rằng APK 14:02
  là của đợt này (có thể đã đè bản của họ).
* **Luật §0.7:** tôi dựng lúc 14:02 **mà chưa soát lịch Tống Kim**. Trận 13:23 = báo danh 1 phút + đánh
  45 phút theo **giờ HĐH máy chủ** (`AUTO_TONGKIM_SPEC.md` §1.1), máy chủ từng lệch múi giờ (§6.3) nên không
  chắc lúc đó có trận hay không. Sau khi phát hiện thì không dựng, không đo gì thêm.
