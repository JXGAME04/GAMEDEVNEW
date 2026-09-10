# BÀN GIAO — WAuto MOBILE, BƯỚC B2: BỘ SINH BỐ CỤC + LỚP TRANG CHẠY THEO BẢNG (11/09)

> Lộ trình: `LOTRINH_WAUTO_MOBILE_1109.md`. B0 (đường ống): `BANGIAO_WAUTO_MOBILE_B0_1109.md`. B1 (khung + icon): `BANGIAO_WAUTO_MOBILE_B1_1109.md`.
> B2 = **cả 15 tab đã có nội dung**, sinh bằng máy từ `WAutoUI/WAuto.rc` + `SaveRoleData` + `ipc_shared.h`: ô tick, ô nhập, hộp chọn
> đọc ghi **thẳng vào `autoData`** đang chạy và lưu `APdata\<id>.dat` ngay khi đổi. B3+ chỉ còn: rà từng tab với chủ, làm 11 điều khiển
> đặc biệt chưa nối, 6 bảng danh sách (B18).

---

## 1. Kết quả đo — LDPlayer 1040×604, `hinod1`, APK `android/apk/jx1mobile-1109-wauto-b2c.apk`

Kịch bản chạm tự động (scratchpad phiên: `thu_b2b.sh`, ảnh `ld/b2b_*.png`, `ld/b2d_*.png`); `.dat` được chép ra trước và **trả lại nguyên**
sau khi thử, app khởi động lại để nạp lại.

| Bước | Thấy gì | Bằng chứng |
|---|---|---|
| Mở khung → tab **Chiến đấu** | 4 ô nhóm có màu riêng (*Mục tiêu* vàng nâu, *Thiết lập kỹ năng* xanh ngọc, *Vòng sáng* xanh lam, *Đổi vũ khí* xanh lá), mỗi hàng nhãn + điều khiển thẳng cột, ô nhập có nền tối viền vàng, hộp chọn nền đỏ đen có mũi tên | `[WAUTO-UI] tab 2: 22 muc, 13 hang, 2 cot` |
| Chạm hộp chọn *Tự đánh phạm vi* | Menu 3 dòng (Tự động / Lên ngựa / Xuống ngựa) mở **xuống**; các ô của trang nằm dưới menu tự ẩn; chọn dòng đầu | `[WAUTO-UI] IDC_COMBO_2_F = 0`, `.dat` offset 248 = 0 |
| Chạm hộp chọn *Bật hỗ trợ #1* | Menu **danh sách chiêu thật** của nhân vật (Không thiết lập, Công kích vật lý, Phiêu Tuyết Xuyên Vân, Từ Hàng Phổ Độ, ... 17 dòng); nút nhóm / tab nằm dưới menu tự ẩn; chạm ra ngoài thì đóng và hiện lại | `JxCore_WAutoDanhSachChieu` = `GetAllSkillByType` (cùng hàm nạp `IPCMainSync.skill[]` cho WAuto.exe) |
| Hoạt động → **Tống Kim**, chạm ô *13h23* | Ô bỏ tick, `bTKGio[0]` 1 → 0 (chạm lại thì về 1) | `[WAUTO-UI] IDC_CHECKBOX_9_G1 = 0`, `wauto_dat.py xem` |
| Chạm ô *Đi sớm (phút)*, xoá, gõ `2` | `nTKSom` = 2 | `.dat` |
| Tab **Hậu cần**, **Di chuyển**, **Chiêu KH** | 11 / 14 / 16 hàng đều lọt 2 cột; danh sách hiện ô tạm "(danh sách - bước B18)" với dải nút mờ | ảnh `b2b_9`, `b2b_10` |
| Sập | `jx_crash.log` rỗng suốt 4 vòng thử | |

Hai chuỗi Windows dựng lại sau mỗi đợt (x64 4/4, SDL 6/6) **0 lỗi**; `CoreShell.cpp` chỉ đổi trong khối `#ifdef JX_ANDROID`.

**Năm góp ý của chủ trong lúc làm, đã đưa vào:** (1) "chữ phải hiển thị đầy đủ, thẳng hàng theo từng mục, kẻ ô để phân biệt từng nhóm"
→ bỏ cách nhân toạ độ hộp thoại PC, viết bộ xếp theo NHÓM / HÀNG / CỘT (mục 3); (2) "nút nền đỏ" rồi "đỏ đen, màu hiện tại quá đậm",
"tab chính phụ phải có nút" → nút vẽ có khối (chuyển sắc, cạnh trên sáng, viền đen, chọn = viền vàng); (3) "mục nào đi kèm mục nào"
→ ưu tiên **không tách nhóm sang cột khác** (chỉ tách khi nhóm cao hơn cả cột), hàng con lùi thẳng ô thứ hai của hàng trên; (4) "có màu
phân biệt các nhóm chức năng" → 6 bộ màu viền + nền tiêu đề, luân phiên theo nhóm (`Mau=` trong ini); (5) "các nút tab chính - phụ phải
màu xanh, căn chỉnh chữ đúng ngay giữa", rồi "các nút có sẵn ở kho VNKU bạn quên rồi à" → **bỏ nút vẽ tay, lấy nguyên ảnh kho**:
`nut_nhom.spr` / `nut_tab.spr` = `UiTong_Sheet0\btn_noname.spr` (nút trơn xanh ngọc, khung tối = thường, khung sáng = đang chọn),
`nut_do_W.spr` + `hop_chon_W.spr` = `UiTong_Sheet0\btn_1.spr` (đỏ đen viền đen) + ô mũi tên vàng cắt từ `UiAutoNew\nut_pop.spr`
(cột 414..476), ô tick = `UiAutoNew\tick_chon.spr`; chữ nút căn giữa dọc bằng `LabelYOffset` đo trên ảnh 20:47 (offset 0: chữ nhóm
font 14 chiếm hàng 121..133 trong nút 120..150 → **nhóm 8**; chữ tab 157..165 trong 156..178 → **tab 5**; hộp chọn 5, nút 4 —
`KWndLabeledButton` vẽ chữ từ mép trên + offset, ngang đã tự căn theo `(Width − n·font/2)/2`, đo đúng giữa ±1 px); chủ nhắc "căn chỉnh
các chữ trong nút cho thẳng hàng" → trong một hàng, mọi ô chữ cùng mép trên: nhãn / chữ tick / chữ hộp chọn / chữ nút / **ô nhập**
(`KWndEdit` vẽ chữ từ mép trên cửa sổ nên ô nhập đặt `Top` = Top nhãn, cao 16 — trước đó "1000" cao hơn nhãn 3 px). Việc (5) làm lúc
21:00 khi chủ đang Tống Kim nên **chưa có ảnh nghiệm thu** — kịch bản `thu_b2d.sh` chụp lại sau 21:36; soi ở đầu B3.

---

## 2. Đã làm gì, ở đâu

| Tệp | Gì | Rào |
|---|---|---|
| `android/sinh_bang_wauto.py` (**mới**, 287 dòng) | đọc `WAutoUI/WAuto.rc` (UTF-16), `Resource.h`, hàm `SaveRoleData` → `android/wauto_bang.json`: 395 điều khiển, **240 nối** vào `autoData` (offset, kiểu, cỡ, chỉ số mảng), 37 hộp chọn tĩnh có danh sách, 19 hộp chọn chiêu, 24 hộp nguồn động, 11 chưa nối, 19 bỏ (Ác chính, Đ.nhập) | Python, chỉ đọc nguồn WAuto |
| `android/sinh_bocuc_wauto.py` (**mới**, 713 dòng) | từ json sinh 15 tệp `ui\ui3\uiwauto_tabN.ini` + `Sources/S3Client/Ui/UiCase/UiWAutoBang.h` (bảng C `s_WAUiTab[15]`); `--xem` vẽ 15 ảnh xem trước (PIL) vào `%TEMP%\wauto_xem\` để soát không cần máy ảo | lớp ghi đè + tệp Android-only |
| `Sources/S3Client/Ui/UiCase/UiWAutoBang.h` (**sinh**, 419 dòng) | mỗi ô: loại, khe trong kho widget, khe nhãn, chỉ số mảng, offset, kiểu, cỡ, danh sách lựa chọn (TCVN3), nguồn, tên IDC gốc; `WA_SIZEOF_AUTODATA 7644` để `UiWAutoTrang` từ chối ghi khi struct lệch | `#ifdef JX_ANDROID` |
| `Sources/S3Client/Ui/UiCase/UiWAutoTrang.h` / `.cpp` (**mới**, 87 + 550 dòng, sinh bởi `va_nguon_android_wauto6.py`) | `KUiWAutoTrang : KWndWindow` — kho widget dùng chung 15 tab (22 tick `KWndButton`, 12 `KWndNhapWA`, 12 hộp chọn `KWndLabeledButton`, 32 nhãn `KWndText80`, 12 nút, 8 ô nhóm `KWndHopNhomWA`). `NapTab(n)` đọc ini, đặt lại widget, `DienGiaTri()` đọc `autoData` qua `JxWAuto_CauHinh()` + offset. Tick / nhập / chọn → `DatInt` + `JxWAuto_LuuCauHinh()`. Hộp chọn: `KPopupMenu` (uParam = khe; `WND_M_MENUITEM_SELECTED` LOWORD = chỉ số, HIWORD = khe); chiêu: `JxCore_WAutoDanhSachChieu`. `KWndNhapWA::PaintWindow` vẽ nền tối + viền; `KWndHopNhomWA` vẽ ô nhóm (bóng mờ, viền màu, dải màu mép trên, tiêu đề trên nền màu) | không nằm trong vcxproj, thêm vào target `main` của `android/CMakeLists.txt` |
| `Sources/S3Client/Ui/UiCase/UiWAuto.h` / `.cpp` | thêm `m_Trang`; `s_aNhomTabId[4][8]` (nhóm, tab con) → số tab toàn cục; `ChonTab` → `NapTab`; `OpenWindow` → `DienGiaTri`; `Breathe` → `m_Trang.Breathe()` + `AnDuoiMenu()` (nút nhóm / tab / BẬT-TẮT nằm dưới menu chọn đang mở thì tạm ẩn) | Android-only |
| `Sources/Core/Src/CoreShell.cpp` | `JxCore_WAutoDanhSachChieu(IPCSkillInfo*, nMax)` | trong khối `#ifdef JX_ANDROID` sẵn có |
| `android/anh_wauto_vnku.py` | thêm nền đỏ đen: `nut_nhom.spr` 166×30, `nut_tab.spr` 84×22, `nut_do_60/84/120/160.spr` (×22), `hop_chon_120/160/200.spr` (×24, mũi tên vàng), `tick_chon.spr` 24×24 — vân lấy từ `UiTong_Sheet0\btn_1.spr`, ô tick từ `UiAutoNew\tick_chon.spr` | dữ liệu Android (lớp ghi đè + `D:\jx1_android_data`) |
| `android/va_nguon_android_wauto6.py` (**mới**, 911 dòng) | kịch bản vá: tạo `UiWAutoTrang.h/.cpp`, vá `UiWAuto.h/.cpp` (3 dấu B2 / B2 b / B2 c), `CoreShell.cpp`, `CMakeLists.txt`. Chạy lại vô hại; muốn sinh lại `UiWAutoTrang.*` thì xoá 2 tệp rồi chạy | |

**Sinh lại từ đầu (theo thứ tự):** `python android/sinh_bang_wauto.py` → `python android/sinh_bocuc_wauto.py --xem` → `python android/anh_wauto_vnku.py`
→ (xoá `UiWAutoTrang.h/.cpp` nếu muốn) `python android/va_nguon_android_wauto6.py` → chép `android/du_lieu_ghi_de/ui/ui3/uiwauto_tab*.ini`
sang `D:\jx1_android_data\ui\ui3\` (ảnh `.spr` tự chép) → `gradlew assembleDebug`. **Ini và ảnh không nằm trong APK**: đổi bố cục chỉ cần chép
tệp rồi khởi động lại app.

---

## 3. Bộ xếp bố cục — luật đang dùng (để B3+ chỉnh từng tab)

* Trang 720×432; vùng nội dung y 94..382; **2 cột × 336** (gutter 10); trong ô nhóm lề 6/6, tiêu đề 18, đáy 3; cách nhóm 4.
* **Hàng** = điều khiển cùng y (±3 đơn vị hộp thoại PC). **Nhóm** = hàng cách nhau ≥ 18 đơn vị, hoặc hàng chỉ có một dòng chữ
  (thành tiêu đề), hoặc hàng chữ mở đầu bảng ("Hồi phục | Mức 1 | Mức 2 | 1/1000 giây": chữ đầu = tiêu đề, còn lại = tiêu đề cột).
  Tiêu đề tay cho nhóm không có dòng chữ: `TIEU_DE_TAY` trong `sinh_bocuc_wauto.py` (30 mục) — **B3+ sửa ở đây**.
* Trong nhóm: hàng "nhãn + điều khiển" đặt điều khiển từ **cột chung col1** = nhãn dài nhất của nhóm (trần 42 % bề rộng); nhãn dài hơn
  col1 thì nối tiếp nếu vừa, không thì xuống dòng (lùi vào col1). Hàng toàn tick chia ô đều (2 hoặc 3 / dòng); hàng con (RC x ≥ 40) lùi
  thẳng ô thứ hai của hàng trên. Hàng trộn xếp nối tiếp; hàng đúng 2 tick thì tick 2 ở giữa cột. Hộp chọn tự co 200 → 160 → 120 để cả
  hàng vừa một dòng; chữ trong hộp dài quá phần trái mũi tên thì cắt "..". Chữ đơn vị ("vạn", "giờ") sát ô nhập hơn (4 px).
* **Rộng nhãn = 6 × số byte TCVN3 + 14** — `KWndText` → `OutputRichText` chỉ vẽ `floor((Width − font) / (font/2))` ký tự, thiếu 12 px là
  mất chữ (đã đo: "Ph", "Đi sớm (phút"). Font 12 = 6 px / ký tự (kể cả chữ có dấu).
* Bước hàng thử 28 → 26 → 24 (tick 24, ô nhập 44×20, hộp chọn ×24, nút ×22); thứ tự thử: 2 cột không tách nhóm → 2 cột cho tách →
  3 cột. Kết quả 15 tab: đều **2 cột**, bước 28 (7 tab), 26 (2), 24 (6).
* Danh sách (6 tab: Di chuyển, Nhặt đồ, Tổ đội, Liên đấu + 2 nhóm) = ô tạm 3 hàng + dải nút mờ gom ngay dưới (B18).
* Kho widget mỗi loại (`POOL`) = C++ `WA_TR_*`: tick 22, nhập 12, chọn 12, nhãn 32, nút 12, ô nhóm 8; vượt thì bộ sinh cảnh báo `CANH BAO`.

---

## 4. Bẫy đã gặp (đọc trước B3)

| # | Bẫy | Cách tránh |
|---|---|---|
| 1 | `KWndText::Init` chỉ đọc `Text=` khi chuỗi **đang rỗng** → đổi tab thấy nhãn cũ cắt ngắn | `SetText("")` trước `Init` (đã làm trong `NapTab`) |
| 2 | `SetIntText` / `SetText` của `KWndEdit` bắn `WND_N_EDIT_CHANGE` → mỗi lần dựng tab ghi `.dat` 5-10 lần | cờ `m_bDangDien` trong `DienGiaTri` |
| 3 | Trình chiếu Android vẽ **chữ có viền và ảnh SAU bóng** (`RU_T_SHADOW` = `DrawRect` tức thời) → menu chọn bị chữ / nút của trang đè lên | `AnDuoiMenu`: tạm ẩn widget giao với hình chữ nhật menu (`KPopupMenu::GetMenuData()`), hiện lại khi menu đóng (`Breathe` soát) — cả ở trang lẫn khung |
| 4 | `KRUShadow.Color_b.a` là **độ trong 0..31** (`alpha = 255 − a<<3`), không phải 0..255 | `WA_Bong(..., a)`: a nhỏ = đặc |
| 5 | Nhãn thiếu 12 px là mất chữ (mục 3) | `NHAN_DEM = font + 2` |
| 6 | `ALW_GetWndPosition`: điểm neo ở nửa dưới màn thì menu mở **lên**; danh sách chiêu 17+ dòng cao hơn khung, kẹp về y = 0 và đè lên dải nút | đã ẩn dải nút dưới menu; menu không cuộn → chiêu > ~20 dòng sẽ mất phần dưới (ghi ở §5) |
| 7 | `g_pRepresentShell` không có trong header nào của Ui — phải `extern iRepresentShell* g_pRepresentShell;` như `WndImage.cpp` | |
| 8 | Heredoc của công cụ Bash bóp `\\` thành `\` → vá Python có `"\t"` / `"\spr"` hỏng ngầm | sửa kịch bản bằng Edit/Write, chạy bằng Bash |
| 9 | `adb install` với `$PWD` dạng `/d/...` khi `MSYS_NO_PATHCONV=1` → "failed to stat", thử chạy trên APK cũ mà không biết | luôn đưa đường dẫn `D:/...` |
| 10 | 20:50 là **Tống Kim** — chủ / phiên kia vào trận ngay lúc tôi chụp màn (ảnh `b2d_*` là trận Tống Kim, không phải khung Auto) | không đụng máy ảo 20:50-21:30; ảnh nghiệm thu là `b2b_*` (20:47) |

---

## 5. Chưa làm / giới hạn còn lại (B3+)

* **11 điều khiển chưa nối** (`khong_noi` trong json, ô mờ trong game): `IDC_CHECKBOX_0_HG`, `IDC_COMBO_3_FL`, `IDC_COMBO_4_FT`, `IDC_EDITOR_4_FT`,
  `IDC_COMBO_6_NE`, `IDC_EDITOR_7_K`, `IDC_CHECKBOX_7_PYD`, `IDC_COMBO_9_RH`, `IDC_EDITOR_10_TEN`, `IDC_COMBO_10_PT`, `IDC_COMBO_13_BOSS` —
  mỗi cái có logic riêng trong `WAuto.cpp` (nguồn động: danh sách người chơi, tên đội, boss...). Làm ở đợt của tab đó.
* **24 hộp chọn nguồn động** (ví dụ bản đồ đi xa phu 31 dòng) hiện đã có danh sách tĩnh nếu WAuto `CB_ADDSTRING` mảng tĩnh; cái nào
  `dong:` thì mờ.
* **Nút hành động** (Lấy, Đọc, Thứ tự, Cài đặt tên...) hiện mờ (`Enable(0)`); **6 danh sách** = ô tạm (B18).
* Tab **Cơ bản**: 6 ô số liệu nhân vật (Sinh lực, Nội lực, Đẳng cấp, Bản đồ...) hiện "-"; B3 đọc từ `Player[]` như `IPCMainSync`.
* Menu chọn không cuộn: nhân vật > ~20 chiêu thì dòng cuối vượt màn (Chiêu KH / Bật hỗ trợ). Cần menu 2 cột hoặc trang riêng ở B18.
* Lớp vẽ riêng của Android (cụm nút kỹ năng góc phải dưới, icon NPC) vẫn đè lên khung — chưa tắt khi khung mở (bẫy 3 của B1).
* Chạm vào **chữ** cạnh ô tick chưa bật/tắt (chỉ ô 24 px) — nên mở rộng cho ngón tay ở B3.
* Nhãn tiêu đề nhóm là do tôi đặt (`TIEU_DE_TAY`), chủ rà lại từng tab.

---

## 6. Cách chủ thử

1. Cài `android/apk/jx1mobile-1109-wauto-b2c.apk` (đã cài trên LDPlayer). Dữ liệu (`ui\ui3\uiwauto_tab*.ini`, `spr\uinew\uiautonew\*.spr`) đã
   chép vào `D:\jx1_android_data`.
2. Chạm icon kiếm chéo → khung TỰ ĐỘNG → chọn nhóm / tab: **mọi ô đều sống**: tick, gõ số (bàn phím ảo), hộp chọn mở menu.
   Đổi gì lưu ngay `APdata\<mã nhân vật>.dat` (cùng tệp WAuto.exe đọc).
3. Bật Auto rồi đổi thiết lập trong lúc chạy: máy auto đọc `autoData` mỗi nhịp 54 ms nên có tác dụng tức thì.
