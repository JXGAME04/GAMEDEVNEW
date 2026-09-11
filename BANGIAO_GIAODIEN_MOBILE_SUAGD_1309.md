# BÀN GIAO 13/09 — GIAO DIỆN MOBILE CĂN ĐỀU MỌI MÁY + TRÌNH CHỈNH CHO NGƯỜI CHƠI

> Nhánh `claude/mobile-ui-customization-analysis-202ae4` (tách từ đầu `mobile-0809` tại `47f01c6c`), gộp vào
> `mobile-0809` bằng `gan_ff.sh` như thường lệ. Đọc kèm `PHANTICH_GIAODIEN_MOBILE_KEHOACH1209.md` (phân tích, vì sao
> chọn cách này) và `KEHOACH_GIAODIEN_MOBILE_1209.md` (kế hoạch gốc). **Bản PC không đổi**: mọi dòng mới nằm trong
> `#ifdef JX_ANDROID` hoặc trong tệp chỉ Android; đã dựng lại dự án S3Client hai cấu hình Windows (ReleaseSDL, Release).

Tiêu chí chủ đặt: *phát hành lên loại điện thoại nào cũng không lỗi giao diện*, và *người chơi tự chỉnh vị trí,
to nhỏ từng icon dễ dàng*.

---

## 1. Đã làm gì (bốn khối)

### 1.1 Khung vẽ hai họ — `KSdlApp.cpp`, `JxSdl_ChotDoPhanGiaiTheoManHinh`, thẻ `[KHUNG 13/09 HAIHO]`

Hệ số phóng lấy **số lẻ**, chọn theo quy tắc `hệ số = min(cao thật / 616, rộng thật / 1040)`:

| Họ | Điều kiện | Khung vẽ |
|---|---|---|
| Điện thoại (1) | tỉ lệ màn ≥ 1040/616 = 1,69 | **cao đúng 616**, rộng = 616 × tỉ lệ máy |
| Máy tính bảng, màn gần vuông (2) | tỉ lệ < 1,69 | **rộng đúng 1040**, cao = 1040 / tỉ lệ máy |

Trong mỗi họ chỉ **một chiều** thay đổi. Điện thoại 720p và 1440p trước rơi vào khung cao 576, nay cùng 616 với máy
1080p. Bẫy khung rộng đúng 1024 vẫn giữ (đổi thành 1026). Tắt bằng `[Resolution] KhungHaiHo=0` (quay về nấc cũ).
`JxSdl_HoKhung()` trả 1/2; `UiToaDo` dùng nó để chọn tệp bố cục mặc định: họ 1 → `Ui\UiToaDo_MacDinh_Rong.ini`,
họ 2 → `Ui\UiToaDo_MacDinh.ini`.

### 1.2 Vùng an toàn làm không gian bố cục — thẻ `[ANTOAN 13/09]`

- `JxSdl_LayVungAnToan(l,t,w,h)` (`KSdlApp.cpp`): `SDL_GetWindowSafeArea` (Android: `SDLSurface.onApplyWindowInsets`,
  API ≥ 30, gộp tai thỏ + vùng vuốt hệ thống; iOS: `safeAreaInsets`) đổi sang toạ độ khung vẽ bằng đúng công thức
  letterbox của `SdlToLogical`. Đối xứng ngang mặc định (xoay 180° bố cục không đổi). Inset vô lý (ăn > 40 % khung) bị
  bỏ. Sự kiện `SDL_EVENT_WINDOW_SAFE_AREA_CHANGED` tăng bộ đếm → `UiToaDo_NhipMobile` nạp lại và áp lại bố cục.
- `JxActivity.java`: `requestApplyInsets()` sau khi vào toàn màn hình (SDL đặt inset = 0 trên Android 11–14).
- `UiToaDo.cpp` `DoiCaTepTheoNeo`: phần dư `DX/DY` tính theo **cỡ vùng an toàn**, cửa sổ gốc và ô vẽ tay cộng thêm
  góc vùng an toàn; `GhiTepVao` ghi `ManHinh=<cỡ vùng an toàn>` và trừ góc đó ra. Tệp bố cục cũ (ManHinh = cỡ khung)
  vẫn đọc được: lần đầu nạp trên máy có tai thỏ, các ô tự dịch vào trong, lưu lại là chuẩn mới.
- `UiPlayerBar::NeoNhomTren`: nhóm trên (đồng hồ, sóng, giấu icon, buff) bám **mép trên vùng an toàn**, gọi lại được
  (`NeoNhomTrenLai`, chỉ dịch phần chưa dịch), bỏ qua ô người chơi đã tự đặt (`UiToaDo_CoKhoa`).
- **Lượt kẹp** (`SuaGd_Kep`, trong `UiToaDoMobile.inc`): ô trong danh sách trắng lòi ra ngoài vùng an toàn thì đẩy vào,
  chạy 400 ms sau khi một ô HUD vừa dựng, sau khi thoát trình chỉnh, sau khi vùng an toàn / nhân vật đổi. Ghi vào
  bảng để lần sau không phải kẹp lại.

### 1.3 Trình chỉnh giao diện cho người chơi — `Sources/S3Client/Ui/Elem/UiToaDoMobile.inc` (include ở cuối `UiToaDo.cpp`)

Thay toàn bộ đường chuột / thanh nút cũ trên mobile (`UiToaDoM_NhanChuot`, `UiToaDoM_Ve`); PC giữ nguyên Ctrl+U.

| Việc | Cách làm |
|---|---|
| Vào | nút chữ **Chỉnh giao diện** trong cửa sổ **Cài đặt** (cạnh nút Đóng; `UiOptions.cpp/.h` rào `JX_ANDROID`, mục `[ChinhGiaoDien]` do `android/sinh_uioptions_chinhgd.py` thêm vào `ui/ui3/uioptions.ini`, chạy **sau** `sinh_uioptions_fps.py` của phiên 144 Hz), hoặc nút nổi cũ dưới bản đồ nhỏ (`[Ui] SuaToaDoNutX/Y`); chỉ khi ở trong thế giới; mobile mặc định `SuaToaDo=1` |
| Chọn | **danh sách trắng** `Ui\UiToaDo_DanhSach.ini` (63 mục HUD, tên hiển thị TCVN3, cờ 1 = không phóng, 2 = không giấu, 4 = không kẹp), dung sai 8 điểm, lấy ô **nhỏ nhất** chứa điểm chạm. Khung cha `KUiPlayerBar\|Main` không nằm trong danh sách → chạm nền không còn kéo cả cụm (gốc lỗi 12/09: `DummyWnd=1 + Trans=1 + ảnh nền`) |
| Kéo | ô đã chọn đi theo ngón, kéo ở **bất kỳ đâu** cũng được; bắt dính mép / tâm ô khác và mép vùng an toàn (6 điểm) kèm đường gióng tím |
| Chụm hai ngón | `KSdlApp::ChamSuKien` (`[SUAGD 13/09 CHUM]`): ngón 2 khi đang sửa = chụm; tỉ lệ làm tròn 5 %, 50–200 % |
| Bảng chỉnh | **một bảng nổi 225 px** (chủ 13/09: "làm lại cho gọn để không che hết các icon"), mặc định nằm giữa vùng thế giới, kéo bằng tiêu đề, nút `gọn`/`đủ` thu lại còn hai hàng nút. Hàng A: `Dời ô / Dời cụm`, `Giấu`, `Gốc ô`, `Hoàn tác` (64 bước, gộp theo thao tác); bảng mũi tên 4x3: dịch **1 điểm** (giữ 400 ms thì chạy đều 60 ms/bước), `+`/`−` 5 %, số %; dòng số `khung - họ - vùng an toàn`; hàng B: `Xoá hết`, `Lưu MĐ` (chỉ khi `[Ui] SuaToaDoChu=1`), `Xong - lưu`. Không còn thanh nút kín bề ngang, không dải chữ che hàng icon trên |
| Phóng | quanh **tâm** (`KWndWindow::UiDatTiLeQuanhTam`); icon ghép (đấu giá, thư, CL) phóng cả con; nút kỹ năng vẽ tay có **cỡ riêng từng nút** (`JxCanDieuKhien.cpp` `s_nKNCoRieng`, lưu vào trường `TiLe` của khoá `KyNang*`) |
| Hồ sơ | `UserData\UiToaDo_<mã nhân vật>.ini`, thiếu thì `UserData\UiToaDo.ini`; lưu ghi cả hai; đổi nhân vật là nạp lại |
| Số liệu máy thật | `[Ui] NhatKyBoCuc=1` → 3 s sau khi vào thế giới ghi `bocuc_thuc.txt` (khung, họ, vùng an toàn, hình chữ nhật thật của từng ô, ô nào ngoài vùng); dòng đầu trình chỉnh cũng hiện `Khung … an toàn …` |

### 1.4 Tệp dữ liệu (lớp ghi đè Android `android/du_lieu_ghi_de`)

- `config.ini`: `[Resolution] KhungHaiHo=1 CaoDienThoai=616 RongMayTinhBang=1040`; `[Ui] BatDinh=1 SuaToaDoChu=1
  NhatKyBoCuc=1 VungAnToan=1 VungAnToanDoiXung=1 LeAnToan=0`. **Phát hành: đặt `SuaToaDoChu=0`, `NhatKyBoCuc=0`.**
- `ui/uitoado_danhsach.ini`: danh sách trắng (sinh bởi `scratchpad/sinh_suagd.py` cùng với `UiToaDoMobile.inc`;
  muốn thêm ô thì thêm dòng `<khóa>=<tên>[,cờ]`, không cần dựng lại).

---

## 2. Cách thao tác (cho chủ và người chơi)

1. Vào game, mở **Cài đặt** và bấm **Chỉnh giao diện** (hoặc nút nổi dưới bản đồ nhỏ).
2. Chạm một icon: viền vàng, dòng xanh ghi tên, toạ độ, cỡ, %.
3. Kéo để dời (có bắt dính); chụm hai ngón hoặc bấm `+`/`−` để to nhỏ; mũi tên dịch từng điểm.
4. `Giấu/hiện`, `Mặc định ô`, `Hoàn tác` khi cần. `Dời cả cụm` chỉ dùng khi muốn dời nguyên khung cha / cả cụm kỹ năng.
5. `Xong - lưu`: ghi tệp theo nhân vật, kẹp lại vào vùng an toàn.
6. Chủ chỉnh bố cục chuẩn cho một họ khung xong bấm `Lưu mặc định` → ghi `Ui\UiToaDo_MacDinh(_Rong).ini` trong thư mục
   dữ liệu máy; chép về `android/du_lieu_ghi_de/ui/` để đóng gói.

---

## 3. Đã kiểm

- Biên dịch NDK clang (x86_64) cho 5 tệp đổi; APK debug hai ABI dựng bằng `gradlew assembleDebug`.
- Dự án S3Client Windows hai cấu hình `ReleaseSDL|x64` và `Release|x64` (chỉ dự án này chứa tệp đổi; Engine/Core không
  include các header đã sửa).
- Máy ảo LDPlayer 1040x604 (API 28, không có inset): ba lượt, xem mục 5.
- Mã hoá: mọi tệp TCVN3 vá bằng kịch bản latin-1, số byte ≥ 0x80 trước/sau bằng nhau (`check_encoding.py`).

## 4. Chưa làm / giới hạn

- **Chưa đo trên Fold 7**: giả thiết lỗ camera ở mép ngắn khi xoay ngang chỉ chứng minh được bằng `bocuc_thuc.txt` từ
  máy chủ (mục `vung an toan …`). Nếu inset về 0 trên Android 11–14 dù đã `requestApplyInsets`, phải đọc inset ở Java
  rồi gọi xuống C.
- Chữ (đồng hồ, nhãn) không đổi cỡ theo %; ảnh vật phẩm trong ô phím giữ cỡ gốc (chỉ nền ô giãn, bật `KeoAnhVatPham=1`
  nếu muốn kéo). Cần điều khiển ảo không có vị trí cố định nên không nằm trong trình chỉnh (vùng đặt theo `[Cham]`).
- `sinh_bocuc_rong.py --chuan` vẫn đọc `userdata/UiToaDo.ini` (tệp chung); tệp theo nhân vật chưa được kịch bản đó biết.
- Góc bo tròn không nằm trong inset nào; muốn chừa thì đặt `LeAnToan` (6–10).

## 5. Kết quả chạy thử máy ảo LDPlayer (1040x604, API 28)

**Lượt 1 (APK `jx1mobile-1309-suagd-a`)**: khung vẽ 1060x616, họ điện thoại, hệ số 0,980 (cách cũ là 1040x604 hệ số
1,00 — máy ảo nay bị thu nhỏ 2 %, chỉ ảnh hưởng môi trường thử). Tự đăng nhập, vào thế giới, `bocuc_thuc.txt` ghi
sau 3 s (581 dòng, vùng an toàn = cả khung vì API 28 không báo inset). Trình chỉnh bật/tắt được bằng nút, thanh nút,
bảng mũi tên, khung viền các ô đều vẽ đúng.

Lỗi tìm ra và đã sửa: hình chữ nhật của cửa sổ gốc không có kích thước (`KUiMsgCentrePad|Main`) lấy bằng
`GetAllChildLayoutRect` gộp cả **6 phòng chat đang ẩn** ở rel −300 → rộng 685, lòi ra ngoài → lượt kẹp đẩy cả khung
chat sang phải 305 điểm, và ô này thành "ô nhỏ nhất" khi chạm giữa màn. Nay gộp **chỉ con đang hiện**
(`SuaGd_GopConHien`); không con nào hiện thì ô không chọn, không kẹp.

Lưu ý: game ghi tệp bố cục với tên **chữ thường** (`userdata/uitoado.ini`, `uitoado_<mã nhân vật>.ini`) vì lớp đường
dẫn POSIX hạ chữ thường; trên Windows không phân biệt nên `UiToaDo.ini` cũ và `uitoado.ini` là một tệp. Bản sao lưu
bố cục chủ đặt: `userdata/UiToaDo.ini.bak-suagd-1309` (đã dùng để khôi phục sau lượt 1 và cuối phiên).

Lượt kẹp hợp lệ đã thấy: `KUiPlayerBar|Market` (y = −1 → 0), `KUiMiniMap|MiniMap` (mép phải 1064 > 1060 → −4).

**Lượt 2**: sửa "chỉ gộp con đang hiện" chưa đủ — 6 phòng chat ẩn **không tắt cờ hiện** mà bị đẩy ra x = −300 (kiểu
cũ). Sửa tiếp: bỏ qua con nằm hoàn toàn ngoài khung khi gộp, và gắn cờ 4 (không kẹp) cho `KUiMsgCentrePad|Main`,
`KSysMsgCentrePad|SysRoom`, `KUiSysMsgCentre|Main`.

**Lượt 3 (APK `jx1mobile-1309-suagd-c`, kịch bản chạm bằng adb)**: không còn dòng kẹp sai. Chạm nút → trình chỉnh
bật; chạm icon PK → viền vàng, dòng "Đang chọn: PK 945,164 37x37 85%"; kéo → 897,203; bấm `+` hai lần → 95 %; bấm
`Xong - lưu` → ghi `uitoado.ini` và `uitoado_3758889385.ini` (`ManHinh=1060,616`,
`KUiToolsControlBar|PK=888,195,950,0,2,0`), trình chỉnh đóng, icon PK nằm đúng chỗ mới, HUD còn lại không đổi. Không
crash. Trong lúc thử, chủ (hoặc ai đó) cũng đang bấm trực tiếp trên máy ảo (nhật ký có chuỗi chạm không phải của kịch
bản): các lần bấm "Lưu mặc định" lúc lượt 1–2 đã ghi bảng còn lỗi +305 vào `ui/uitoado_macdinh_rong.ini` của máy ảo
— đã chép lại tệp sạch từ `android/du_lieu_ghi_de/ui/`.

**Máy ảo dùng chung với phiên 144 Hz**: phiên đó cài APK mã phiên bản cao hơn (109102125) nên `adb install -r` của
phiên này bị từ chối im lặng (lượt "4" thực ra chạy APK của phiên kia, đã loại khỏi kết luận). Cài thử phải dùng
`adb install -r -d`. Hai phiên đã nhắn nhau qua kênh phiên; phiên 144 Hz không chạm máy ảo nữa.

**Lượt 5–7 (sau khi gộp `origin/mobile-0809` 6edcb896 của phiên 144 Hz)**, cài bằng `adb install -r -d`, chủ bấm thử
trực tiếp trên máy ảo song song với kịch bản:
- Chủ: "chữ hiển thị rất mờ" — hai nguyên nhân, đã sửa cả hai: (1) khung hai họ cho máy ảo 1040x604 ra 1060x616 rồi thu
  0,98 → **hệ số 0,85..1,15 nay vẽ đúng 1:1** (`[KHUNG 13/09 HAIHO b]`, máy 1080p trở lên không đổi); (2) chữ bảng chỉnh
  font 12 viền **trắng** (thừa hưởng `VeChu` cũ) → nút và tiêu đề font **16**, mọi chữ viền **đen**, ô nút 64 px.
- Chủ: "cho cái chỉnh giao diện vào phần cài đặt, làm lại giao diện chỉnh cho gọn" → bảng một khối (§1.3) + nút trong
  Tùy chọn. Chủ: "thêm nút như các nút khác" → nút ảnh 97x29 (ảnh nút trống VNKU `btn_f00.png` thu về, 3 khung, nhãn
  vẽ đè), nút Đóng dời sang trái. Nhật ký 11:53:55 ghi nhận chủ đã mở Tùy chọn và bấm nút này thành công.
- Chủ: "cái hiển thị FPS GPU không điều chỉnh được" → dòng `ThongTinGoc` của phiên 144 Hz neo mép phải-trên, không theo
  quy ước "tâm + cỡ": thêm `UiToaDo_DangKyORiengHinh` (hình chữ nhật thật) + dùng `pfnTrung` của ô khi dò chạm + đưa
  vào danh sách trắng (`ThongTinGoc=Dòng FPS - CPU - GPU,1`); neo kéo tính từ hình thật (`SuaGd_BatDauKeo`).
- Chú ý khi đọc nhật ký: các cú chạm không phải của kịch bản là của chủ; hai nguồn chạm xen nhau nên ảnh chụp kịch bản
  lượt 7 không dùng làm bằng chứng, chỉ dùng nhật ký `[UITOADO] BAT/ghi` và tệp lưu.

## 6. Giao cho chủ

- APK: `android/apk/jx1mobile-1309-suagd.apk` (debug, hai ABI). Đưa lên điện thoại theo quy trình cũ: chép thành
  `D:\jx1_android_data_dt_v4\jx1mobile.apk`, chép `android/du_lieu_ghi_de/config.ini` và `ui/uitoado_danhsach.ini`
  vào cây `dt_v4`, xoá `manifest.txt`, khởi động lại máy chủ 8765. **Tôi chưa làm bước này** vì nó đổi thứ điện thoại
  của chủ tự tải.
- Trên Fold 7 sau khi vào game 3 s, lấy `bocuc_thuc.txt` trong thư mục dữ liệu (hoặc đọc dòng "Khung … an toàn …" khi
  mở Chỉnh giao diện) và báo lại: đó là số đo đầu tiên của vùng an toàn trên máy thật.
- Muốn tắt nhanh từng phần: `KhungHaiHo=0` (khung vẽ), `VungAnToan=0` (neo theo mép màn), `SuaToaDo=0` (giấu nút
  Chỉnh giao diện), `NeoTheoMep=0` (bỏ cả neo).
