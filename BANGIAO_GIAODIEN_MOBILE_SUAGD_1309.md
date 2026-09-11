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

- Nhánh: `claude/mobile-ui-customization-analysis-202ae4` = `origin/mobile-0809` (6edcb896, phiên 144 Hz) + 3 commit
  SUAGD; đã fast-forward `mobile-0809` lên đầu nhánh này và push (xem `git log`). Máy ảo đang chạy APK cuối
  (`android/apk/jx1mobile-1309-suagd.apk`).

- APK: `android/apk/jx1mobile-1309-suagd.apk` (debug, hai ABI). Đưa lên điện thoại theo quy trình cũ: chép thành
  `D:\jx1_android_data_dt_v4\jx1mobile.apk`, chép `android/du_lieu_ghi_de/config.ini` và `ui/uitoado_danhsach.ini`
  vào cây `dt_v4`, xoá `manifest.txt`, khởi động lại máy chủ 8765. **Tôi chưa làm bước này** vì nó đổi thứ điện thoại
  của chủ tự tải.
- Trên Fold 7 sau khi vào game 3 s, lấy `bocuc_thuc.txt` trong thư mục dữ liệu (hoặc đọc dòng "Khung … an toàn …" khi
  mở Chỉnh giao diện) và báo lại: đó là số đo đầu tiên của vùng an toàn trên máy thật.
- Muốn tắt nhanh từng phần: `KhungHaiHo=0` (khung vẽ), `VungAnToan=0` (neo theo mép màn), `SuaToaDo=0` (giấu nút
  Chỉnh giao diện), `NeoTheoMep=0` (bỏ cả neo).

## 7. Bổ sung tối 13/09 — lượt d và e (ba yêu cầu mới của chủ + câu hỏi "mini skill góc phải")

Chủ (sau lượt 7): *"tạm thời lưu tọa độ hiện tại làm mặc định và tôi muốn bạn xóa đi các icon tôi đã ẩn; các giao
diện khi bấm vào icon chưa chỉnh sửa tọa độ được; các phần lúc đăng nhập vào game chưa tự căn chỉnh được"*, rồi:
*"bạn có xóa nhầm mini skill góc phải màn hình không, tôi đang nói xóa các icon đã ẩn thôi"*.

### 7.1 Bố cục mặc định = bố cục chủ đang dùng (`[SUAGD 13/09 d]`)

- `android/sinh_bocuc_macdinh_tu_chu.py` (mới, trong repo): đọc tệp người chơi mới nhất trên máy ảo
  (`D:\jx1_android_data\userdata\uitoado_<id>.ini`), giữ các dòng `Goc.<lớp>=<mục>` cũ, ghi ra cả hai họ
  `ui/uitoado_macdinh_rong.ini` (điện thoại) và `ui/uitoado_macdinh.ini` (máy tính bảng) + bản sao vào cây máy ảo.
  Chạy lại bất cứ lúc nào chủ muốn "lấy bố cục hiện tại làm mặc định".
- 7 khung màn đăng nhập (`KUiInit|Main`, `KUiLogin|Main`, `KUiConnectInfo|Main`, `KUiSelNativePlace|Main`,
  `KUiSelServer|Main`, `KUiSelPlayer|SelRole`, `KUiNewPlayer|NewPlayer`) đặt `NeoY=1` (neo giữa dọc) trong mặc định và
  trong tệp người chơi trên máy ảo → khung 800x600 nằm giữa màn mọi tỉ lệ, không còn dính mép trên.
- Cửa sổ mở từ icon (hành trang, nhân vật, võ công, bang hội, tổ đội, tuỳ chọn, hộp hệ thống, rương, cửa hàng, giao
  dịch, thư, đấu giá, Auto, bản đồ) và khung đăng nhập vào danh sách trắng với cờ 7 (chỉ dời chỗ, không phóng, không
  giấu, không kẹp). Cửa sổ gốc **bất kỳ** đang mở mà không có trong danh sách cũng tự được thêm lúc làm mới (tên = tên
  lớp) → chạm thân cửa sổ là chọn được. Trình chỉnh bật được ở mọi màn (cả màn đăng nhập) qua nút nổi.
- Đã kiểm trên máy ảo: chạm thân cửa sổ Liên lạc (`KUiChatCentre|Main`, tự thêm) → chọn, kéo → dời (710,73 → 636,97).

### 7.2 "Xoá icon đã ẩn" và câu hỏi "mini skill góc phải" (`[SUAGD 13/09 e]`)

Điều tra: 6 ô có cờ giấu trong tệp của chủ (`KUiPlayerBar|AutoPlay`, `HideWindow`, `PartnerIcon`,
`ImediaLeftSkill`, `ImediaRightSkill`, `KUiToolsControlBar|Rec`) **đã bị giấu từ bố cục mặc định 12/09** (mọi bản từ
883d6ae1), chủ không tự giấu ô nào thêm. Trước lượt d, trình chỉnh **vẽ cả ô đã giấu** (khung đỏ, vì
`KWndWindow::Paint` bỏ qua cờ giấu khi đang sửa) nên chủ thấy nút **Auto** (ảnh hai kiếm chéo `autobt3.spr`, trông như
một ô kỹ năng nhỏ) và đã kéo nó lên góc phải trên (924,238 → 1006,168); trong game bình thường nó không hiện. Lượt d
gỡ 6 khoá khỏi danh sách trắng → ô vẫn được vẽ khi sửa nhưng không chọn được nữa → chủ hỏi "xoá nhầm mini skill".

Sửa (lượt e):
- **Nút Auto hiện lại** ở đúng chỗ chủ đã kéo (`KUiPlayerBar|AutoPlay=1006,168,1000,0,2,0`) trong mặc định hai họ và
  trong hai tệp người chơi trên máy ảo (`HIEN_LAI` trong `sinh_bocuc_macdinh_tu_chu.py`). Nếu chủ không cần: chạm ô →
  Giấu.
- Ô đã giấu nay **bị cắt hẳn khỏi trình chỉnh** (không vẽ, không chọn, không bắt dính) — đúng nghĩa "xoá" — nhưng
  **không mất**: bấm nút **Giấu khi không chọn ô** (nhãn đổi thành "Ô đã giấu") → hiện các ô đã giấu với khung đỏ,
  chạm ô rồi bấm "Hiện lại"; bấm lại ("Cất ô giấu") hoặc đóng trình chỉnh → cất. `KWndWindow::Paint` / `PtInWindow`
  (Android) hỏi `UiToaDo_HienOAn()` thay vì `UiToaDo_DangSua()`; bản PC giữ nguyên dòng cũ trong `#else`.
- 6 khoá trở lại danh sách trắng (Nút Auto ở hàng icon; 5 khoá còn lại ở mục "giấu sẵn" cuối tệp). Nhãn nút Giấu
  theo ngữ cảnh: "Giấu" / "Hiện lại" (ô đang chọn đã giấu) / "Ô đã giấu" / "Cất ô giấu".
- Kiểm: `ninja` x86_64 cho `UiToaDo.cpp` + `WndWindow.cpp` 0 lỗi; APK `jx1mobile-1309-suagd-i`; Windows
  ReleaseSDL|x64 + Release|x64 (`WndWindow.cpp` là mã dùng chung) đều 0 lỗi. Máy ảo: ô đã giấu không còn vẽ khi mở
  trình chỉnh; bấm "Ô đã giấu" → khung đỏ + dòng nhắc; nút Auto hiện ở góc phải trên (ảnh `ld/shot9_1.png`, `shot9_2.png`).

### 7.4 Lượt f — chủ trả lời ngay sau lượt e (ảnh nút hai kiếm chéo): *"icon này tôi không giấu được? nó dư thừa; mini skill tôi không chỉnh tọa độ được; hiển thị thông tin nhân vật cũng vậy"*

Kết luận: "mini skill" **không phải** nút Auto (đoán sai ở lượt e). Nút Auto là ô dư thừa → **giấu lại** trong mặc định hai
họ và trong tệp người chơi trên máy ảo (`HIEN_LAI = []`). Lúc chủ thử giấu, máy ảo còn chạy danh sách trắng lượt d (chưa
có `KUiPlayerBar|AutoPlay`) nên không chọn được; nay có trong danh sách, muốn giấu/hiện tự làm được.

Hai ô chủ không chỉnh được, tìm theo `bocuc_thuc.txt`:

- **"Mini skill" = dãy icon buff** (`KUiPlayerBar|BuffImage` × 20 + `txtBuffTime` × 20, `[BuffPos] XY=120,28
  End=26,27 MaxIHeight=10,20` trong `uiplayerbar.ini`; cùng tên gọi với `UiMiniSkill` = bảng buff của nhánh PC
  USVOLAM). 40 ô con **cùng tên mục ini** nên hệ UiToaDo không có khoá riêng cho từng ô → gộp thành **một ô vẽ tay
  `ThanhBuff`** (`UiPlayerBar.cpp`, `KUiPlayerBar::BuffTrung/BuffLay/BuffDat/BuffHinh`, đăng ký bằng
  `UiToaDo_DangKyORieng` + `UiToaDo_DangKyORiengHinh` trước `NeoNhomTren()`): vị trí = góc trái-trên tuyệt đối của ô
  buff 0, dời = dịch cả dãy; hình = gộp các ô đang hiện, không có buff nào thì lấy 5 ô đầu để vẫn chạm chọn được.
  `NeoNhomTren` không dồn dãy buff nữa khi người chơi đã tự đặt (`UiToaDo_CoKhoa("ThanhBuff")`). Danh sách trắng:
  `ThanhBuff=Mini skill - buff,1`.
- **"Hiển thị thông tin nhân vật" = khung mục tiêu `KUiTargetInfo`** (tên + máu của NPC/người đang chọn, hiện giữa
  trên: "Tiêu Dao Mật Sứ 100%"). Cửa sổ gốc chỉ 27x23 tại (370,0) còn hộp hiện nằm ở ô con (0,33)…(170,66) → thêm
  **cờ 8 = gộp cả ô con đang hiện** khi tính hình (`SUAGD_CO_GOPCON`, `SuaGd_HinhKWnd(…, nCo)`); áp cho
  `KUiTargetInfo|Main=Khung mục tiêu,9` và `KUiHeaderControlBar|Main=Thanh trên,9` (thanh 17 px + dòng số bên dưới).
  Không áp đại trà cho mọi cửa sổ gốc vì `KUiMiniMap` có ô con `NameShadow` ở y = −2 → lượt kẹp sẽ đẩy bản đồ xuống.
- Thanh trên (`KUiHeaderControlBar|Main`) vốn đã chọn/kéo được (thử: 172,0 → 172,58 rồi hoàn tác) — chủ có thể đã
  chạm vào dòng số nằm ngoài khung 17 px; cờ 9 gộp luôn dòng số.

Kiểm: `ninja` x86_64 `UiToaDo.cpp` + `UiPlayerBar.cpp` 0 lỗi; APK `jx1mobile-1309-suagd-j` (= `android/apk/jx1mobile-1309-suagd.apk`);
Windows ReleaseSDL|x64 + Release|x64 (`UiPlayerBar.cpp/.h` là mã dùng chung, phần thêm nằm trong `JX_ANDROID`) 0 lỗi.
Máy ảo: chạm (150,45) → "Mini skill - buff 268x86", kéo +50 → cả dãy icon lẫn dòng giây đi theo; "Thanh trên" 552x27.

### 7.6 Lượt g — chủ thử ngay lượt f: *"di chuyển được nhưng thanh máu màu đỏ không di chuyển theo"* (ảnh khung mục tiêu "Sự Kiện Tết Đoàn 100%") và *"tống kim cũng bị lệch mà không chỉnh tọa độ được"*

- **Thanh máu đỏ của khung mục tiêu**: `KUiTargetInfo::PaintWindow` không tự vẽ mà gọi Core
  (`GOI_DRAW_TARGET_INFO` → `KNpc::PaintTargetInfo`, `KNpc.cpp`), và Core vẽ thanh máu ở toạ độ **cố định**
  `x = 420, y = 55..65` (khung 800x600, `[Main]` ở 370,0; `nWeightOffset` trong ini không dùng). Các ô con KWnd (tên,
  nền, đầu) đi theo cửa sổ, riêng thanh máu thì không. Sửa (chỉ `JX_ANDROID`): `UiTargetInfo.cpp` đọc `[Main] Left/Top`
  của ini, mỗi lần vẽ tính `g_nJxMucTieuDichX/Y = vị trí tuyệt đối − ini`; `KNpc.cpp` cộng hai số này vào toạ độ thanh
  máu. Bản PC: dòng cũ nguyên vẹn (rào `#ifdef`).
- **Tống Kim lệch**: bảng điểm `KUiTongKimScore|Main` (469x122) nằm ở 119,45 với `NeoX=1` trong bố cục nhập từ bản
  PC 12/09 → trên 1040 vẫn 119 (tệp `ManHinh=1040` nên neo không cộng gì) = lệch trái. Đưa vào danh sách trắng
  (`KUiTongKimScore|Main=Bảng điểm Tống Kim,1`, `KUiTongKimInfo|Main=Bảng Tống Kim,1`) và đặt mặc định **285,45** (căn
  giữa 1040, `NeoX=1` nên màn rộng hơn vẫn giữa) trong hai tệp mặc định + tệp người chơi trên máy ảo. Chủ muốn chỗ
  khác thì kéo.
- **Chạm suông làm ô nhích**: khi thử dãy buff thấy chạm (không kéo) vẫn qua `SuaGd_Keo` → bắt dính tới 6 điểm. Thêm
  vùng chết `SUAGD_KEO_CHET = 5`: ngón chưa đi quá 5 điểm kể từ lúc chạm thì không đổi, không bắt dính.
- Chưa giải thích được: dãy buff ở 121,28 lúc vào game nhưng lúc mở trình chỉnh đã là 96,29 (−25,+1) trước khi
  chạm. Đã thêm nhật ký `[SUAGD] ThanhBuff: dat …` trong `KUiPlayerBar::BuffDat` để bắt kẻ dời; xem logcat lượt sau.
- Khung mục tiêu có mục mặc định `KUiTargetInfo|Main=370,0,1000,0,1,0` (neo giữa ngang) để màn rộng hơn vẫn ở giữa.

- Chủ (ảnh bảng Tống Kim): *"nó bị lồi chữ tống kim ra ngoài, đúng là nó nằm giữa"* → mục `KUiTongKimInfo|Title=120,4,…,NeoX=1`
  trong bố cục là vết cộng +120 của lần đổi 800→1040 (12/09) áp nhầm cho ô CON (ini `[Title] Left=0`, bảng rộng 221) →
  tiêu đề thò ra ngoài bảng. Xoá mục này khỏi hai tệp mặc định + tệp người chơi; cùng lúc rà các mục con khác có neo ≠ 0:
  còn lại đều là mục chủ tự đặt (icon, ô phím…), không phải vết.
- Bẫy khi "dọn" tệp bố cục lúc game đang chạy: bảng trong bộ nhớ vẫn giữ mục cũ (`ThanhBuff=96,29`, `Title`) và **ghi lại
  cả vào tệp mặc định** mỗi lần chủ bấm Xong / Lưu MĐ → phải dừng game, dọn tệp, rồi mới cài APK (script `thu_h.sh`).
- `[Ui] NhatKyBoCuc=2` (mới): `ApMotO` ghi `[UITOADO] ap <khoá> -> x,y` cho từng ô lúc áp bố cục — dùng để soi ô "nhảy về
  chỗ cũ" (khung mục tiêu từng đọc ra 369,−1 dù tệp người chơi ghi 664,83). Khung mục tiêu là `KWndShowAnimate`: mỗi lần
  ẩn/hiện nó `SetPosition(m_oFixPos)`; `m_oFixPos` chỉ được cập nhật qua `UiNhoViTri()` (ApMotO và kéo trong trình chỉnh
  đều gọi) — không thấy lối nào khác, nên theo dõi bằng nhật ký mức 2 ở lượt sau.

- Kiểm lượt g (APK `jx1mobile-1309-suagd-l`, tệp dọn sạch trước khi cài): nhật ký mức 2 ghi `ap KUiTargetInfo|Main -> 664,83`
  và `ap KUiTongKimScore|Main -> 285,38` ở cả ba lần áp (vùng an toàn, nhân vật), `bocuc_thuc.txt` xác nhận khung mục tiêu
  nằm đúng 664,83, không còn dòng `ThanhBuff: dat` (dãy buff giữ chỗ ini). Windows S3Client hai cấu hình + Core
  `Client ReleaseSDL|x64` + `Client Release|x64` đều 0 lỗi.

### 7.8 Lượt h — ảnh điện thoại (2000x868): *"khi chơi trên điện thoại nó chưa tự căng chỉnh"*

Đọc ảnh: cụm trái bắt đầu ở ~78 điểm lô-gic thay vì mép, cụm phải dừng ở ~71 điểm trước mép, thanh máu bị đẩy xuống ~46
điểm, đáy ~6. Đó không phải bố cục sai mà là **vùng an toàn quá rộng**: `SDLSurface.onApplyWindowInsets` của SDL3 gộp
`systemBars + systemGestures + mandatorySystemGestures + tappableElement + displayCutout` → vùng cử chỉ lùi (~30 dp mỗi
bên) và thanh trạng thái (dù đã ẩn) đều thành lề, còn `VungAnToanDoiXung=1` áp lề lớn hơn cho cả hai bên.

Sửa (`[ANTOAN 13/09 b]`):
- `JxActivity.datVungAnToanTheoCamera()` (gọi ngay sau `super.onCreate`): thay listener inset của `SDLSurface` (mỗi View chỉ
  một listener) bằng listener chỉ lấy **`displayCutout`** (API 30: `getInsets(Type.displayCutout())`; API 28–29:
  `getDisplayCutout()`), rồi gọi `SDLActivity.onNativeInsetsChanged` như SDL. Máy không có chỗ khoét → 0,0,0,0 = neo sát
  mép như máy ảo.
- `VungAnToanDoiXung` mặc định 0 (config + `KSdlApp.cpp`): chỉ mép có camera bị lùi, mép kia sát màn.
- Cách kiểm trên điện thoại: mở "Chỉnh giao diện", dòng dưới bảng ghi `<khung> ĐT | an toàn <x,y> <w>x<h>`; mong đợi
  `an toàn 0,0` (hoặc chỉ một mép lùi bằng đúng lỗ camera).

### 7.9 Lượt i — phiên DONHIP báo APK m thiếu bản vá SDL (23:45)

`ThirdParty/SDL3-src/` nằm trong `.gitignore`, nên bản vá `[DONHIP 12/09]` trong `src/gpu/vulkan/SDL_gpu_vulkan.c` (đếm
`VK_SUBOPTIMAL_KHR`, hint `JX_BO_QUA_SUBOPTIMAL` = bản sửa bca30aa của SDL) không đi theo git. Cây này chép SDL3-src từ
`wt_mobile` (chưa vá) → mọi APK dựng ở đây (a…m) đều thiếu vá; trên Fold 7 pha 1 của DONHIP vô hiệu, chỉ 28–45 khung/s
(nhật ký "SDL dem: … (chua co hint)"). Máy ảo không lộ vì LDPlayer hướng gốc ngang.

Sửa: chạy `python androida_sdl3_donhip.py` trong cây (idempotent, kiểm `grep -c "DONHIP 12/09"` = 3), dựng lại APK
(`jx1mobile-1309-suagd-n`), thay vào dt_v4 + khởi động lại 8765. **Quy tắc cho cây mới:** vá SDL3-src trước khi dựng APK.

### 7.10 Lượt k — ảnh Fold 7 với trình chỉnh mở: "an toàn 58,46 1306x570"; chủ: *"một số icon không căn chỉnh chính xác, còn hở rất nhiều"* rồi *"không cần vùng an toàn đâu, tôi chơi nhiều game đâu có phần này"*

- Số đo thật đầu tiên trên Fold 7 (SM-F966U1, 1080x2520 xoay 1): khung vẽ **1436x616**, vùng an toàn 58,46 1306x570 → lề
  trái 58, trên 46, phải 72, dưới 0. Bản n (có `datVungAnToanTheoCamera`) vẫn ra số này vì `SDLSurface.handleResume()`
  đăng ký lại listener của SDL mỗi lần resume → listener thay ngoài mất tác dụng. Sửa cho đúng: lớp con `JxSurface extends
  SDLSurface` qua `createSDLSurface()`, ghi đè thẳng `onApplyWindowInsets` (chỉ lấy `displayCutout`).
- Chủ chốt bỏ hẳn vùng an toàn → **`VungAnToan=0` mặc định** (config lớp ghi đè, dt_v4, máy ảo, và mặc định trong
  `KSdlApp.cpp`). Neo theo mép màn, bỏ qua mọi inset; lỗ camera có thể đè lên icon, người chơi tự kéo icon nếu thấy vướng.
- "Icon không căn chỉnh": nhóm góc trên-trái (đồng hồ, sóng, nút ẩn icon, dãy buff) không có mục riêng nên theo khung cha
  `KUiPlayerBar|Main` — khung này neo GIỮA ngang (`1,1,…,1,2`) để thanh chat/ô nhập ở đáy nằm giữa, nên trên màn rộng hơn
  1040 cả nhóm trôi vào giữa (ảnh: đồng hồ ở x ≈ 200). `NeoNhomTren` nay bù cả X (`ms_nLeftIni`, `ms_nDaDichX`,
  `JxSdl_AnToanTrai`): nhóm bám mép trái khung (hoặc mép trái vùng an toàn nếu bật lại), như trên máy ảo.
- Phần còn lại trong ảnh đã đúng neo: cụm phải sát mép phải vùng an toàn, hàng icon và thanh đáy giữa, bảng Tống Kim trái.

### 7.11 Lượt p/q (00:06–00:30 14/09) — chủ sau bản o: *"nút ẩn các icon phía trên bị lỗi chưa căn chỉnh; ổ khoá khoá rương chưa căn theo; lấy config tôi vừa lưu làm mặc định"*, rồi *"phóng to bản đồ mini rồi thu nhỏ lại thì sai vị trí, 4 icon ngay bản đồ không đi theo"*, và *"icon đấu giá bấm được trên giả lập, điện thoại không"*

- **Nút ẩn hàng icon** (`HideIcons`, ini 782,54) nằm CUỐI hàng icon (hàng neo giữa theo khung cha) nhưng lượt trước tôi xếp nó
  vào nhóm góc trên-trái và bù X → trên Fold 7 nó rơi vào giữa hàng, đè icon Tổ đội. Nay chỉ bù Y (`[NHOMTREN 13/09 X b]`).
- **Ổ khoá khoá rương** = `KUiMiniMap|LockBtn`: chủ kéo trong trình chỉnh → mục mới ghi với neo suy từ toạ độ tương đối
  (19,101 → "trái") trong khi cha (bản đồ) neo phải → trên màn rộng lệch 396 điểm. Sửa gốc (`[NEO 13/09 CON]`, `NeoDeGhi`):
  ô con chưa có neo thì ghi neo của cha như lúc nạp. Dữ liệu: LockBtn → neo 2,0 trong mặc định + tệp máy ảo + dt_v4.
- **Mặc định = bố cục chủ lưu 00:09** (máy ảo, Lưu MĐ + Xong): sinh lại hai tệp mặc định, chép lên `dt_v4/ui`.
- **Bản đồ nhỏ to/nhỏ** (`[UITOADO 14/09 LOPPHU]`): bản đồ có 4 ini theo chế độ (nhỏ 188x147 ở 619,14; to 420x317 ở
  385,14; to mở rộng 512x415; không ảnh 130x50) nhưng cùng khoá `KUiMiniMap|…` → phóng to áp toạ độ bản nhỏ (852) → lòi
  phải → lượt kẹp đẩy vào rồi ghi đè → thu nhỏ sai chỗ; 4 nút nhận toạ độ tương đối của bản nhỏ. Nay `KWndWindow::UiTenLopPhu()`
  (ảo, Android) ghép hậu tố vào tên lớp khi tạo khoá: `KUiMiniMapTo|…`, `KUiMiniMapToEx|…`, `KUiMiniMapKhongAnh|…` — mỗi chế độ
  một bộ khoá; khoá chưa có mục thì dùng ini (nút đi theo bản đồ). Mặc định: bản to 620,12 neo phải (mép phải = 1040 như bản
  nhỏ), to mở rộng 528,54, không ảnh 910,0; dòng `Goc.KUiMiniMapTo=MiniMap`… trong tệp mặc định; danh sách trắng thêm
  "Bản đồ to / to mở rộng / không ảnh".
- **Đấu giá trên điện thoại**: 16/16 tệp (`ui/Ui3/auction/*.ini`, `script/ui/uiauction_house.lua`, `auction_def.lua`,
  `settings/auction.ini`) đều có trong `mobile_01.pak` (kiểm bằng name2id, `kiem_pak_daugia.py`) → không phải thiếu dữ liệu.
  Icon 23x23 phóng 1,85 (`UiDatTiLe` phóng cả Width/Height nên vùng chạm = hình vẽ). Chưa rõ; cần chủ cho biết icon thư bên
  cạnh có bấm được không và icon có lún xuống khi chạm không.
- Phiên DONHIP (theo lệnh chủ) đã thay dt_v4 bằng bản SDL 3.2.30 (`jx1mobile-1209-donhip-c`, 109110010); `origin/mobile-0809`
  = 097e984a có CMake tự vá SDL và trỏ SDL3-3.2.30. Bản p (SDL 3.2.14) **không** đưa lên dt_v4; bản q dựng sau khi gộp 097e984a
  (nguồn SDL3-3.2.30 chép từ worktree DONHIP, không tải lại).

- 00:23–00:27: bản -c (SDL 3.2.30) của phiên DONHIP tự đóng trên Fold 7 sau 1–4 s (Java org/libsdl/app vẫn 3.2.14, JNI lệch) và
  bản q (cùng cây 097e984a) cũng không vào được thế giới trên máy ảo. Chủ kẹt (điện thoại đã cài 109110010, launcher không hạ cấp) →
  tôi đẩy tạm **bản p** (838fdf36 + nút ẩn icon + neo ô con, SDL 3.2.14 đã vá, 109110015) lên dt_v4 + restart 8765 (taskkill,
  PID 279232). Bản q (bản đồ to/nhỏ) đợi phiên DONHIP commit Java SDL 3.2.30 rồi gộp, dựng lại. Windows q: 2 cấu hình 0 lỗi.
- Quy tắc dt_v4 (phiên DONHIP nhắc, tôi đã dính): chép tệp vào dt_v4 xong PHẢI restart may_chu_tai_du_lieu.py ngay, manifest chỉ
  sinh lúc khởi động; tệp lệch md5 làm điện thoại lặp tải và không cập nhật APK. Restart = taskkill PID đang nghe rồi Start-Process
  đúng dòng lệnh, kiểm PID mới + giờ manifest.

- 00:34: **bản r** = 882059d7 (gộp Java SDL 3.2.30 của phiên DONHIP) + không đổi mã; dex có JxSurface + getPreferredLocales, libSDL3
  3.2.30. Máy ảo: vào thế giới bình thường; bấm nút đổi bản đồ → nhật ký `ap KUiMiniMapTo|MiniMap -> 620,12`, bản đồ to nằm sát mép
  phải với 4 nút ở đáy bản đồ to (ảnh `ld/shot16_1.png`); bước đổi lại bản nhỏ chưa kiểm được vì lúc đó trình chỉnh đang mở (nuốt
  chạm). Bản r để sẵn ở `android/apk/jx1mobile-1309-suagd.apk`, chỉ thay lên dt_v4 sau khi phiên DONHIP báo đo xong.

### 7.12 Lượt s/t (00:45–01:05 14/09) — chủ: *"fix phần giao diện đăng nhập; các thông báo đăng nhập - vào game chưa căn chính giữa; viết lại phần lưu tên tài khoản - mật khẩu, không tự động đăng nhập; khung đăng nhập trên điện thoại bị bó, nhập tên tài khoản bị mờ chữ"*

- **Không tự đăng nhập**: `[Login] TuDongDangNhap=0` mặc định (lớp ghi đè, dt_v4, máy ảo). Cơ chế tự đăng nhập Android
  (`JxUi_TuDangNhapAndroid`, 09/09) vẫn còn, chỉ tắt bằng config.
- **Nhớ tài khoản + mật khẩu** (`[DANGNHAP 14/09]`): "Ghi nhớ" trên Android vốn đã lưu cả bản băm MD5 của mật khẩu
  (`Setting.ini [Login] LastPassword`, mã hoá OneTimePad). Nay `KUiLogin::Show()` điền tài khoản như cũ và điền 8 dấu `*` vào ô mật
  khẩu khi có bản băm đã nhớ (`KLogin::GetLoginPasswordSaved`, Android); `OnLogin()` thấy ô vẫn là 8 dấu `*` thì dùng bản băm đã
  nhớ, không băm lại chuỗi `********`; người chơi gõ đè thì đi đường cũ. Bản PC giữ nguyên (nhánh `#else`).
- Sập tìm ra khi thử lần đầu (bản s): khối `KSG_PASSWORD` 64 byte đọc bằng `GetStruct` từ `Setting.ini` không chắc kết thúc chuỗi
  (sau 32 ký tự MD5 là rác của lần ghi); đường auto-login cũ gửi cả khối nên không sao, còn `OnLogin` gọi
  `OperationRequest(ATYPE_SETPASS)` → `strcpy` → `FORTIFY: prevented 73-byte write into 64-byte buffer` (SIGABRT). Sửa:
  `GetLoginPasswordSaved` chép sạch tối đa 32 ký tự, kết thúc chuỗi (bản s2).
- **Ô tài khoản/mật khẩu**: lớp ghi đè `ui/ui3/uilogin.ini` (mới): cao 22, chữ 16 (gốc 18/14); phần còn lại bằng bản gốc.
  Chữ mờ trên Fold 7 phần lớn do cả khung lô-gic 1436x616 bị phóng 1,75 lần lên 2520x1080 (font bitmap) — chữ 16 đỡ hơn chữ
  14, muốn nét hẳn phải vẽ chữ theo độ phân giải thật (chưa làm).
- **Thông báo đăng nhập căn giữa — tìm ra**: hộp "THÔNG BÁO" lúc đăng nhập ("Đang đăng nhập vào trò chơi", "Tài khoản hoặc Mật khẩu không đúng") là `KUiConnectInfo`, gốc khởi tạo từ mục ini `RuningImgBg` (không phải `Main`) nên khoá `KUiConnectInfo|Main` trong mặc định không khớp → khung 800 đứng ở mép trái (tâm 400 trên 1040, 400 trên 1436). Đổi khoá thành `KUiConnectInfo|RuningImgBg` + dòng `Goc.KUiConnectInfo=RuningImgBg` (mặc định, máy ảo, dt_v4, danh sách trắng) → nhật ký `ap KUiConnectInfo|RuningImgBg -> 120,0`, hộp về giữa. Các khung màn đăng nhập khác (`KUiInit`, `KUiLogin`, `KUiConnectInfo`, `KUiSelServer`,
  `KUiSelPlayer`, `KUiNewPlayer`) và hộp `KUiInformation*`/`KUiMsgSel*` đều có mục neo giữa (1,1) trong mặc định; thông báo
  của `KUiConnectInfo` vẽ giữa khung 800 (`[Message] Pos=261,259 Size=280`). Thử khung rộng 1186x616 trên máy ảo: màn hình chính
  và hộp chọn máy chủ nằm giữa (`ld/shot17_0.png`, `shot17_2.png`). Chưa tái hiện được thông báo lệch — cần ảnh cụ thể của chủ.
- Đấu giá trên điện thoại: chưa tìm ra; gói đủ ini/script/ảnh, cơ chế nạp Lua giống thư (thư chạy). Nhật ký mô-đun đấu giá ghi
  `jx_mail.log` trong thư mục app của điện thoại (chưa được gửi về máy chủ) — nhờ phiên DONHIP thêm vào bộ gửi nhật ký.

### 7.7 Trạng thái cuối

- Máy ảo + dt_v4: APK `android/apk/jx1mobile-1309-suagd.apk` (= lượt k, `jx1mobile-1309-suagd-o`, dựng 00:03 14/09: vùng an toàn tắt, nhóm trên-trái bám mép, `JxSurface`, SDL3 đã vá DONHIP).
- Điện thoại (23:33): đã tự đưa APK m thành `D:\jx1_android_data_dt_v4\jx1mobile.apk` (apk.txt versionCode 109102329), đổi
  `VungAnToanDoiXung=0` trong config của dt_v4 (các khoá khác của phiên 144 Hz giữ nguyên), khởi động lại
  `may_chu_tai_du_lieu.py` đúng dòng lệnh cũ; phiên nối tiếp `bangiao-donhip-mobile-context-deaeff-ab` đã được báo. Tệp mặc định trên máy ảo và trong repo giống nhau.
- Việc chủ/phiên 144 Hz cần làm cho điện thoại: chép `ui/uitoado_macdinh_rong.ini`, `ui/uitoado_macdinh.ini`,
  `ui/uitoado_danhsach.ini`, `config.ini` và APK mới vào `D:\jx1_android_data_dt_v4`, xoá `manifest.txt`, khởi động
  lại máy chủ 8765 (phiên 144 Hz quản lý cây này — đã nhắn).
- Vẫn chưa đo Fold 7 (`bocuc_thuc.txt` hoặc dòng "Khung … an toàn" trong trình chỉnh).

### 7.13 Lượt u (01:20–01:45 14/09) — đấu giá / Chiến Lệnh trên điện thoại: đã loại trừ gói pak, ký hiệu trùng, dữ liệu; chờ jx_mail.log của điện thoại

- **Phát hiện "788 tệp trong pak là bản cũ" là BÁO ĐỘNG GIẢ** (rút lại 01:20): `ReverseTools/pak_vltk/pakdump.py::read_entry` chỉ thử
  zlib, còn bộ đóng gói điện thoại nén mục bằng **UCL NRV2B** (`android/ucl_nen.exe`, cờ byte cao = 1) → hàm trả blob nén thô
  → md5 lệch, "kích thước pak" nhỏ hơn thật ra là kích thước nén. Giải nén đúng bằng `ucl.nrv2b_decompress_8(blob, size)`
  (`scratchpad/quet_pak_lech2.py`): 815/820 tệp `script/ ui/ settings/` trùng md5 với `D:\jx1_android_data`, **0 thiếu**
  (tên Hán như `ui\Ui3\Meridian\任脉.ini` phải `name2id` trên byte GBK; tệp `.cu_*`/`.goc_datau` là rác). 5 tệp khác
  (`uiitem.ini` pak = bản Android 12/09 còn tệp rời là bản PC!, `uioptions.ini` pak thiếu [Fps]+[ChinhGiaoDien],
  `uitoolscontrolbar.ini` [WAuto] Height 56/48, `uilogin.ini` mới, `settings\faction\门派设定.ini`) đều có bản rời trong
  dt_v4 (rời nạp trước) nên điện thoại vẫn đọc bản đúng. dt_v4 tệp rời (642) so với `D:\jx1_android_data`: chỉ khác
  `config.ini`, `package.ini`, `userdata\*` (userdata KHÔNG nằm trong manifest → không tải xuống).
- **Ký hiệu trùng (bẫy 09/09) còn nguyên vá**: `llvm-nm -D --defined-only` trên 9 `.so` của bản u: `GameScriptFuns` và
  `g_GetGameScriptFunNum` chỉ có trong `libCoreClient.so` ở cả arm64-v8a lẫn x86_64; 40 ký hiệu trùng khác giống hệt nhau
  giữa hai ABI (`scratchpad/nm_dup_*.txt`: IR_UpdateTime, KNode typeinfo, g_pMusic, l_Time…, không liên quan script).
  Máy ảo ghi `[SP] dang ky 133 ham C (Say...)` = bảng Core đúng.
- **Cơ chế** (đọc mã): icon Chiến Lệnh do MÁY CHỦ đẩy: `serverscript_jx2/chienlenh/script/chienlenh/cl_ui.lua CLUI_OnLogin()`
  gửi `emSCRIPT_PROTOCOL_CL_SHOWICON`(id 75) khi `CL_Ready()==1` → client nạp `\script\script_protocol\protocol_def_c.lua`
  rồi `\script\ui\uichienlenh.lua` → `CLUI_OnShowIcon` → `CLUi_SetIconVisible` → `KUiCLIcon` (UiChienLenh.cpp; chỉ tạo khi
  nhận lệnh này, thoát game thì Release). Icon đấu giá do client tự tạo lúc GAME_START (`KUiAuction_OnGameStart`, giống thư);
  chạm → `AUCUI_OP_ICON_CLICK` → `AuctionUi_OnRequest` → Lua `UIAuctionHouse:OnAuctionIconClick(1)` → `OpenAuctionWindow()`
  (mở ngay phía client, không chờ máy chủ) → `Lua->UI cmd=18, cmd=1` → `[AUC] [UI] OpenWindow visible=1`. Thư cũng đi
  đường Lua (`UIMail:OnMailIconClick()`) — thư chạy trên điện thoại nên hệ Lua client KHÔNG chết toàn bộ như 09/09.
- Đường chạm (KSdlApp.cpp ChamSuKien): chạm nhanh = WM_MOUSEMOVE + WM_LBUTTONDOWN + WM_LBUTTONUP tại CHỖ ĐẶT NGÓN
  (không phải chỗ nhả) → không phải lỗi run tay; hai lần chạm < 400 ms & < 24 px = WM_LBUTTONDBLCLK (chạm lại nhanh sau
  lần "không ăn" sẽ thành nháy đúp).
- **Chưa kết luận được** vì điện thoại chưa gửi log nào từ 00:47: bộ gửi log Java chỉ chạy khi `[DoNhip] Bat=1` (phiên DONHIP
  phát hiện 01:40) → bản w (109110137, `[DoNhip] GuiLog=1`) đã lên dt_v4, chủ mở app một lần là có `jx_mail.log`. Đọc
  theo thứ tự: sau `GAME_START` phải có `nhan s2c_scriptdata id=75 len=9` → `nap \script\ui\uichienlenh.lua vao bang
  rieng: ok` (thiếu = máy chủ không gửi / lỗi chunk → cần ScriptError.log, đã nhờ DONHIP gửi thêm); khi chạm đấu giá phải
  có `[AUCUI] UI->Lua op=1 param=1` (không có = chạm không tới icon: hình học/che khuất) → `Lua->UI cmd=18` (không có =
  Lua lỗi) → `OpenWindow visible=1` (có mà không thấy = cửa sổ nằm ngoài khung/bị che).
- Bản u = 510d3be7 (gộp origin/mobile-0809 705e8a5b [DAN] + JxDoNhip gửi jx_mail.log) versionCode 109110123 đã lên dt_v4
  01:27 rồi bị DONHIP thay bằng v (109110129, 3267b755 = 510d3be7 + [DAN c+d]) 01:30 và w (109110137) 01:40; mobile-0809
  FF = 510d3be7 rồi DONHIP FF tiếp. Thử máy ảo: bản u vào thế giới bình thường; **không đổi `wm size` máy ảo khi chủ đang
  dùng** (01:33 đổi 2080x1080 để thử khung rộng → chuột LDPlayer lệch, chủ "không kích mở game được"; đã reset).

### 7.14 Lượt y (01:44–02:10 14/09) — GỐC đấu giá / Chiến Lệnh trên điện thoại: `Include()` của Lua client không đọc pak. Chủ: *"bản đưa lên điện thoại nó chưa kết nối được lua5.4 nên không mở lên được đấu giá và mất icon chiến lệnh; lỗi này từng bị ở bản giả lập rồi đã fix 1 lần"*

- **Bằng chứng** (phiên DONHIP thêm `ScriptError.log` + `jx_mail.log` vào bộ gửi, bản w/x, phiên Fold 7 01:44):
  ```
  \script\script_protocol\protocol_def_c.lua:146: attempt to index a nil value (global 'ScriptProtocol')
  \script\ui\uiauction_house.lua:10:  attempt to index a nil value (global 'AUCTION_DEF')
  \script\ui\uimail.lua:387: ObjBuffer nil   uimail.lua:540: tblen nil   sj_guanxian.lua:22: Param2String nil
  [SP] nap \script\script_protocol\protocol_def_c.lua vao bang rieng: LOI than chunk
  [AUCUI] UI->Lua op=1 param=1  ->  [SP] RunClientLua loi ...: UIAuctionHouse:OnAuctionIconClick(1)   (mỗi lần chạm)
  ```
  Mọi biến nil đều là thứ một tệp `Include("\script\...")` phải định nghĩa → `Include` im lặng không làm gì.
- **Gốc**: `Core/Src/ScriptFuns.cpp LuaIncludeFile` ghép đường tuyệt đối rồi gọi `lua_dofile` = `lua4_dofile` → `luaL_loadfilex`
  → `fopen` thật (`Library/Lua54/jx_lua_fopen.c` → `JxPathPosix`), **không biết pak**. Máy ảo đọc `/mnt/shared/Misc`
  = `D:\jx1_android_data` có `script\` rời nên luôn mở được; điện thoại chỉ có `mobile_NN.pak` (dt_v4 chưa bao giờ phát
  `script/` rời) → `LUA_ERRFILE` → `Include` = không. Hệ quả: bộ điều phối `protocol_def_c.lua` đứt dòng 146 → mọi
  `s2c_scriptdata` (id 75 `CL_SHOWICON` → icon Chiến Lệnh, id 37 danh sách thư) `loi=1`; thân `uiauction_house.lua` đứt
  dòng 10 (`AUCTION_DEF` nil) nên `OnAuctionIconClick` không tồn tại → chạm có tới nhưng không mở (dời icon đi đâu cũng
  vậy); `uimail.lua` không đụng biến thiếu ở đầu tệp nên hộp thư mở được nhưng trống. Khác bẫy 09/09 (bảng `GameScriptFuns`
  trùng tên → `Include` = nil mọi nơi): `nm -D` bản u hai ABI đều chỉ có bảng ở `libCoreClient` (vá còn nguyên).
- **Sửa** `c7de4f04` + `[PAK b]` (chỉ `#ifdef JX_ANDROID`, nhánh `#else` giữ nguyên hai dòng PC): `lua_dofile` trả
  `LUA_ERRFILE` (2, `l4_loi_nap`) → `sIncludeTuPak` mở lại qua `KPakFile` (đĩa trước, pak sau — đúng đường
  `KLuaScript::Load` nạp `uimail.lua` từ pak), tên hạ chữ thường dạng `\script\...`, chạy thân tệp trong **đúng state L**
  bằng `lua_dobuffer` (= `luaL_loadbufferx` + `l4_gan_env` + `lua4_call`, cùng đuôi với `lua4_dofile`), tên chunk
  `@\script\x.lua` để lỗi ghi số dòng. Bản y (chưa gate) trên máy ảo cho thấy tệp rời CÓ nhưng thân lỗi
  (`task_addplayerexp.lua`, mã 1) bị chạy hai lần → `[PAK b]` chỉ đọc lại khi mã = 2; mã 1/3 giữ nguyên "Include HONG".
- Không thử được đường pak trên máy ảo: 33 pak trong `D:\jx1_android_data\data` không có `script\` (máy ảo luôn đọc tệp
  rời). Kiểm chứng = điện thoại: sau bản y2 lên dt_v4, chủ mở app, `jx_mail.log` phải có
  `nap \script\script_protocol\protocol_def_c.lua vao bang rieng: ok`, `dispatch ... (75, 1) ... (loi=0)`, khi chạm đấu giá
  `Lua->UI cmd=18` → `[AUC] [UI] OpenWindow visible=1`; `ScriptError.log` không còn dòng nil.
- Bài học ghi vào bộ nhớ: **mọi đường mở tệp bằng `fopen`/`lua_dofile`/`io.open` đều KHÔNG thấy pak** → trên điện thoại
  chỉ dữ liệu qua `KPakFile`/`g_pPakList` mới có; muốn tái hiện lỗi "chỉ điện thoại" phải thử với dữ liệu đóng gói, máy ảo
  đọc tệp rời không bao giờ lộ.
- **Đã xác nhận trên Fold 7 02:08 14/09 (bản y2)**: `protocol_def_c.lua`/`uichienlenh.lua`/`uimail.lua`/`uiauction_house.lua` đều `nap ... ok`, 101 dispatch `loi=0`, 0 `loi=1`, không có ScriptError.log; chạm đấu giá 02:08:21 → `Lua->UI cmd=18` → `[AUC] [UI] OpenWindow visible=1`. Chủ: *"đã oke rồi hiển thị đầy đủ"*.

### 7.15 Lượt c (02:10–02:45 14/09) — nhớ tài khoản/mật mã một lần là xong; vào game tự bật tên + thanh máu NPC + tên vật phẩm rơi. Chủ: *"bạn fix lỗi lưu chưa? tôi muốn đăng nhập 1 lần là lưu lại luôn không mất nữa"* và *"viết thêm tính năng lúc vào game tự mở hiển thị tên - thanh máu của npc - tên item rơi xuống đất"*

- **Gốc "lúc lưu lúc không"** (đọc mã, không đoán): bản lưu chỉ chạy ở hai chỗ, `KLogin::SelectRole` (lúc chọn nhân vật, thêm 09/09) và `UiExit` (thoát sạch). Điện thoại bị vuốt tắt hoặc hệ thống giết ngay ở màn chọn nhân vật thì cả hai không chạy. Thêm: ô "Ghi nhớ" chỉ tự tick khi đã nhớ tài khoản từ trước, lần đầu (máy mới cài, vừa xoá dữ liệu) đang tắt, gõ xong bấm Đăng nhập là không lưu gì. Khi có lưu thì lưu đúng: Setting.ini giả lập có tài khoản 6 byte và bản băm 32 byte, giải mã khớp.
- **Sửa** `e684559d` (chỉ `#ifdef JX_ANDROID`): (1) `Login.cpp` gọi `SaveLoginChoice()` ngay khi máy chủ trả danh sách nhân vật, tức đã nhận tài khoản + mật mã; cờ nhớ đã được đặt ở `CloseWindow(false)` cuối `OnLogin` nên có sẵn. (2) `KUiLogin::Show()` tick sẵn ô "Ghi nhớ" trên điện thoại, ai không muốn thì bỏ tick trước khi bấm.
- **Gốc tên / thanh máu / tên đồ tắt**: nhánh Android `[HIENTEN 12/09]` trong `KUiPlayerBar::LoadPrivateSetting` chỉ mặc định bật khi UiConfig chưa có khoá; nhưng `SavePrivateSetting` ghi lại trạng thái lúc thoát, nên 8 trên 17 hồ sơ trên máy chủ tải đã có `ShowObjName=0`, một hồ sơ cả ba bằng 0, vào game lại tắt. Điện thoại không có F7 / F8 / Ctrl+Space để bật lại. Giá trị 2 và 4 trong tệp là bit `PATE_NAME` / `PATE_LIFE` đọc ra qua `CheckShowName/Life`, hàm Set nhận BOOL nên truyền 1 là đúng. `m_nShowPateFlag` khởi tạo chỉ có `PATE_CHAT`, nên nếu `LoadPrivateSetting` bị bỏ qua (hồ sơ chưa có khoá "Scheme") thì tên và thanh máu tắt sẵn.
- **Sửa** (cùng commit): `LoadPrivateSetting` nhánh Android luôn đặt 1 cho cả ba, bỏ qua giá trị đã lưu; và chốt lại ở cuối khối `GDCNI_GAME_START` trong `GameSpaceChangedNotify.cpp`. Lý do đặt cuối: `NotifyToStartGame()` ở đầu khối gọi đồng bộ xuống `UiOnGameServerStartSyncEnd` → `LoadPrivateConfig` → `LoadPrivateSetting`, nên dòng cuối khối là quyết định cuối, và lo luôn trường hợp bị bỏ qua. Bản PC / máy chủ không đổi, nhánh `#else` giữ nguyên từng dòng.
- **Kiểm chứng giả lập** (bản b): đặt `ShowObjName=0` cho hồ sơ `1474bb336l71ff3e656` rồi mở game, sau khi vào thế giới tự về 1. Ảnh chụp: tên kèm cấp độ và thanh xanh ngay dưới tên trên mọi nhân vật; quái "Hắc Cẩu" thanh đỏ, "Lang bổng" thanh xanh. Ô "Ghi nhớ" chưa thử được vì giả lập tự vào thẳng thế giới: `JxUi_TuDangNhapAndroid` đọc `TuDongDangNhap` từ `GetCurrentDirectory + Config.ini`, trên giả lập không trúng tệp nên lấy mặc định 1, dù `config.ini` mục `[Login]` đã đặt 0. Đây là bẫy riêng của giả lập, điện thoại vẫn dừng ở màn đăng nhập. Nhờ chủ xác nhận ô tick trên Fold 7.
- Bản c = 33bd80c8 (e684559d + gộp ccee33b3 của DONHIP) versionCode 109110243, md5 c0f316da…, 20062511 byte, lên dt_v4 02:44:59, máy chủ 8765 PID 293944, config giữ nguyên của DONHIP.
- Chủ giao tiếp lúc 02:40: icon Tống Kim mở bảng xếp hạng (PC là phím dấu huyền), icon Kinh Mạch (PC là F2), ô dùng nhanh 1-4 bấm liên tục bị chậm. Hai dòng cuối chủ nêu (F7-F8, Ctrl+Space) là phím PC của đúng tính năng vừa làm, không phải việc mới.
