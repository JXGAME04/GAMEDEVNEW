# BÀN GIAO — WAuto MOBILE, BƯỚC B1: KHUNG AUTO + ICON AUTO (11/09)

> Lộ trình: `LOTRINH_WAUTO_MOBILE_1109.md`. B0 (đường ống): `BANGIAO_WAUTO_MOBILE_B0_1109.md`.
> B1 = phần chủ nhìn thấy: icon Auto trên thanh công cụ, khung "TỰ ĐỘNG" từ kho VNKU, dải nút nhóm / tab con, nút BẬT/TẮT,
> dòng trạng thái. **Chưa có nội dung tab** (B2 sinh trang theo bảng, B3+ từng tab).

---

## 1. Kết quả đo — LDPlayer 1040×604, `hinod1`, APK `android/apk/jx1mobile-1109-wauto-b1.apk`

Kịch bản chạm tự động (icon → BẬT → nhóm Hoạt động → TẮT → Đóng → icon), ảnh chụp từng bước trong scratchpad phiên:

| Bước | Thấy gì | Nhật ký |
|---|---|---|
| Chạm icon Auto (kiếm chéo xám, bên trái giữa màn) | Khung 720×432 hiện **giữa màn** (lần đầu 980×588, chủ xem xong nói "quá to, phải nhỏ gọn lại" → thu về 720×432, khoảng 70 % bề ngang, để lộ nhân vật xung quanh), tiêu đề "TỰ ĐỘNG", 4 nhóm *Điều khiển · Hậu cần · Cài đặt · Hoạt động*, hàng tab con của nhóm đang chọn (vàng = đang chọn), dòng "Chiến đấu - nội dung tab này có ở bước B2", nút **Bật Auto**, dòng trạng thái "Auto đang tắt. Chạm nút Bật Auto để tự đánh, nhặt đồ, uống thuốc." | |
| Chạm **Bật Auto** | Nút đổi thành **Tắt Auto**, trạng thái "Đang bật: đánh quái trong tầm, nhặt đồ, uống thuốc" | `[WAUTO-UI] nut BAT/TAT -> 1` → `[WAUTO] BAT (PRT_TICKSTART -> ATYPE_CLEAR)` → `[WAUTO] nhip ...` (máy auto B0 chạy) |
| Chạm nhóm **Hoạt động** | Hàng tab con đổi thành *Dã Tẩu · Tống Kim · CTC · Liên đấu · Sát thủ · H.động*, tab đầu vàng | |
| Chạm **Tắt Auto** | Về "Bật Auto", trạng thái "Auto đang tắt..." | `[WAUTO-UI] nut BAT/TAT -> 0` → `[WAUTO] TAT` |
| Chạm **Đóng** (góc phải dải tiêu đề) | Khung ẩn, màn chơi bình thường | |
| Chạm icon lần nữa | Khung mở lại đúng chỗ, giữ nhóm đang chọn | |
| Sập | `jx_crash.log` rỗng | |

Hai chuỗi Windows dựng lại trên nguồn đã gộp `mobile-0809` tới `b019f7db` (11 commit "gộp main" của phiên kia): x64 4/4 và SDL 6/6 **0 lỗi**.

---

## 2. Đã làm gì, ở đâu

| Tệp | Gì | Rào |
|---|---|---|
| `Sources/S3Client/Ui/UiCase/UiWAuto.h` / `.cpp` (**mới**, 253 dòng) | `KUiWAuto : KWndShowAnimate`. Khung ảnh nền; 4 `KWndPureTextBtn` nhóm + 8 `KWndPureTextBtn` tab con (tên lấy đúng `s_aNhomTab` của WAuto.exe, bỏ Ác chính / Đ.nhập); `KWndButton` BẬT/TẮT kiểu CheckBox (ảnh 4 khung: Up = "Bật Auto", Down = "Tắt Auto") gọi `JxWAuto_Bat()` của B0; `KWndText80` trạng thái lấy từ `JxCore_WAutoHoatDong()` mỗi 500 ms; `Đóng` chỉ **ẩn** (giữ đối tượng). Cửa sổ tự căn giữa theo `SCREEN_WIDTH/HEIGHT` lúc mở nên 1040×604 hay 1188×616 đều vừa | Chỉ Android; **không nằm trong vcxproj**, thêm thẳng vào target `main` của `android/CMakeLists.txt` |
| `Sources/S3Client/Ui/UiShell.h` / `.cpp` | lớp nút `Player_WAuto` (ClassType cho thanh công cụ): chạm = mở/đóng khung; `UpdateData` = `CheckButton(JxWAuto_DangBat())` nên icon **vàng khi auto đang bật, xám khi tắt**. Đăng ký ngay cạnh dòng `// Player_AutoPlay::RegisterSelfClass();` (giữ nguyên ghi chú, fkauto không dùng) | `#ifdef JX_ANDROID` |
| `Sources/Core/Src/CoreShell.cpp` | `JxCore_WAutoHoatDong(szOut, nMax)` bọc `WA_HoatDong()` — cùng dòng chữ WAuto.exe hiện ở chân cửa sổ | trong khối `#ifdef JX_ANDROID` sẵn có |
| `android/du_lieu_ghi_de/ui/ui3/uiwauto.ini` (**mới**) | bố cục 720×432 (tính từ hằng `KW, KH` trong kịch bản vá): nhóm y=36 (Font 14), tab y=70 (Font 12), tên tab giữa vùng nội dung, BẬT/TẮT (24,384) 102×40, trạng thái (134,394), Đóng (635,5). Đổi cỡ: sửa `KW/KH` + `KHUNG_W/KHUNG_H` rồi xoá ini và chạy lại hai kịch bản | lớp ghi đè |
| `android/du_lieu_ghi_de/ui/ui3/uitoolscontrolbar.ini` | `Button6=WAuto` + mục `[WAuto]` (52,330) 47×47, `ClassType=Player_WAuto`, `CheckBox=1` | lớp ghi đè |
| `android/du_lieu_ghi_de/spr/uinew/uiautonew/khung_wauto.spr` (980×588), `bat_tat_auto.spr` (122×48 ×4), `.../uitoolscontrolbar/auto_m.spr` (47×47 ×2) | ảnh sinh từ kho VNKU (chỉ đọc) bằng `android/anh_wauto_vnku.py` | dữ liệu Android |
| `android/va_nguon_android_wauto4.py`, `android/anh_wauto_vnku.py` | kịch bản vá (tạo 2 tệp nguồn + ini, vá 4 tệp) và kịch bản ảnh. Chạy lại vô hại | |

**Nguồn ảnh VNKU đã dùng:** `Spr\UiNew\UiAutoNew\khung.spr` (1313×788 → 720×432), `Spr\UiNew\MinMapSmall\bat_auto.spr` +
`tat_auto.spr` (285×112 → 102×40, 4 khung) — và **icon Auto cắt từ hình kiếm chéo bên trái nút "Bật Auto"** (xám / vàng).
`nut_de_auto.spr` của VNKU (151×151, 30 khung) là chữ "Giữ" nhấp nháy (nút đè auto), không hợp làm icon nên không dùng.
Nút tab của VNKU (`btn_chiendau.spr`, `btn_caidat.spr`...) đều **nung sẵn chữ**, không dùng lại cho 4 nhóm + 15 tab được. Chủ yêu cầu
"nút cho đẹp, ở kho có sẵn hình" → rà 230 nút trong kho, lấy nút **trơn** `UiTong_Sheet0tn_noname.spr` (dải xanh ngọc, 2 khung sáng/tối,
cùng tông thanh viền khung) làm nền: `nut_nhom.spr` 166×30 và `nut_tab.spr` 84×22 (khung 0 tối = chưa chọn, khung 1 sáng = đang chọn),
chữ vẽ bằng `KWndLabeledButton::SetLabel` (trắng, vàng khi chọn). Vá `va_nguon_android_wauto5.py`. Nút Đóng vẫn là chữ thuần.

---

## 3. Ba bẫy đã gặp (đọc trước khi làm B2)

| # | Bẫy | Cách tránh |
|---|---|---|
| 1 | **`KIniFile` không cắt chú thích sau giá trị.** `Button6=WAuto  ; ...` bị đọc thành tên mục `WAuto<tab>; ...` → không thấy `[WAuto]`, icon không hiện, mất một vòng thử | Chú thích trong `.ini` luôn ở **dòng riêng** |
| 2 | **Cột phải dưới nút Chạy (868,298) bị bảng "Theo dõi nhiệm vụ" đè lên** khi có nhiệm vụ đang theo dõi: chạm vào là mở Chỉ nam nhiệm vụ | Icon đặt bên **trái giữa màn** (52,330). Vẫn nằm trên vùng chữ chat trôi bên trái, nhìn hơi rối nhưng chạm được; chủ muốn dời chỗ thì sửa `Left/Top` mục `[WAuto]` trong `uitoolscontrolbar.ini` lớp ghi đè |
| 3 | **Lớp vẽ riêng của Android nằm TRÊN khung:** nút "Sửa giao diện" (UITOADO, chỉ khi `[Ui] SuaToaDo=1`), icon NPC đối thoại, cụm nút kỹ năng góc phải dưới đều vẽ sau `UiPaint` nên đè lên khung (thấy trong ảnh: "Điều khiển" bị nút Sửa giao diện che một phần, cụm kỹ năng che góc phải dưới) | B2 sẽ **tắt cụm kỹ năng + icon NPC khi khung Auto đang mở** (`KUiWAuto::GetIfVisible()` trong `JxKyNang_Ve` / `JxIconNpc_Ve`), hoặc thu khung hẹp hơn. Bản phát hành `SuaToaDo=0` thì nút Sửa giao diện không còn |

---

## 4. Cách chủ thử

1. Cài `android/apk/jx1mobile-1109-wauto-b1.apk` (đã cài sẵn trên LDPlayer, `[WAuto] Bat=0`).
2. Vào game, chạm **icon kiếm chéo** bên trái giữa màn hình → khung TỰ ĐỘNG.
3. Chạm **Bật Auto** → nhân vật tự đánh quái trong tầm 1000 (đứng trong thành thì không có gì để đánh), nhặt đồ, uống thuốc; icon
   chuyển vàng; dòng trạng thái đổi theo máy auto đang làm gì (Tống Kim / Dã Tẩu... khi bật các máy đó bằng `wauto_dat.py`, xem B0).
4. **Đóng** ở góc phải dải tiêu đề, hoặc chạm icon lần nữa. Trạng thái bật/tắt **không** lưu qua lần mở app (đó là công tắc
   `config.ini [WAuto] Bat`; B2 sẽ nhớ vào tệp `.dat`).

---

## 5. Tiếp theo: B2

Bộ sinh bố cục + lớp trang chạy theo bảng (xem lộ trình §B2). Mọi API B2 cần đã có: `JxWAuto_CauHinh()` (trỏ thẳng vào
`autoData` đang dùng), `JxWAuto_LuuCauHinh()`, `KUiWAuto` đã có vùng nội dung trống ≈680×280 (y 96..376 của khung 720×432) và hàm `ChonTab`.
