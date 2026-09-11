# PHÂN TÍCH KẾ HOẠCH GIAO DIỆN MOBILE 12/09 ĐỐI CHIẾU VỚI MÃ NGUỒN — CHƯA SỬA MÃ

> Đọc: `KEHOACH_GIAODIEN_MOBILE_1209.md`, `GHICHU_PHIEN_1209.md`, `KEHOACH_GIAODIEN_MOBILE_0909.md`,
> `LOTRINH_MOBILE_0909.md`. Mã đối chiếu ở đầu nhánh `mobile-0809` (commit `47f01c6c`).
> Ràng buộc của chủ: **bản PC không đổi** (phát hành cả PC lẫn mobile, PC đã chuẩn); mọi việc mới rào
> `JX_ANDROID` / `JX_MOBILE`. `D:\USVOLAM` chỉ tham khảo, tính năng chỉnh toạ độ bên đó chưa hoàn thiện.

---

## 1. Kết luận

**Chọn phương án A của kế hoạch, nhưng làm theo hướng "mở rộng hệ `UiToaDo` đang có", không "đổi nghĩa neo" và không viết thêm lớp HUD mới.** Lý do: hệ hiện có đã đủ dữ liệu (khoá từng ô, `Left,Top,TiLe,Co,NeoX,NeoY`, tệp mặc định + tệp người chơi), đã chạy trên PC lẫn mobile, và **các điểm kế hoạch cho là "phải làm mới" thì phần lớn đã có sẵn**; cái thiếu là bốn chỗ cụ thể:

1. **Chọn ô để sửa phải qua danh sách trắng (~40 mục HUD)**, không dò cây cửa sổ. Đây là gốc thật của "kéo là đi cả cụm" (xem §3), không phải `Wnd_GetActive`.
2. **Gốc toạ độ bố cục = vùng an toàn** (`SDL_GetWindowSafeArea`, đã xác minh có đường ống Android lẫn iOS, xem §5) + **một lượt kẹp vào vùng an toàn** sau khi dựng xong cây.
3. **Phóng to phải hoàn thiện cho ba loại ô còn thiếu**: nút kỹ năng vẽ tay (chưa có kích thước riêng từng nút), ảnh vật phẩm trong ô phím (mặc định không kéo giãn), chữ (không đổi cỡ). Với ô ảnh `KWndImage` thì đã phóng đúng cả vùng bấm.
4. **Trình chỉnh cho người chơi**: chạm chọn → kéo → bảng nổi (mũi tên 1 điểm, −/+ 5 %, Ẩn, Mặc định, Hoàn tác, Xong), chụm hai ngón, hồ sơ theo nhân vật. Phần này viết mới nhưng dựa toàn bộ trên hàm đã có.

Phương án B (khung ảo cố định + letterbox) **không nên giữ làm lưới an toàn** như kế hoạch: khung 1040x604 trên máy 21:9 (Fold 7 màn ngoài 2520x1080) sẽ để **hai dải đen ~330 px mỗi bên**. Cái đáng cân nhắc thay vào là **cố định chiều cao khung vẽ (616) và chỉ để bề ngang đổi** (§6.4), rẻ hơn và loại bỏ hẳn biến số theo chiều dọc.

---

## 2. Đối chiếu từng điểm của kế hoạch với mã

| Kế hoạch 12/09 nói | Mã thực tế | Hệ quả |
|---|---|---|
| §4.1: chế độ sửa dò cửa sổ bằng `Wnd_GetActive`, chỉ trả cửa sổ cấp cao nhất → tóm khung to | Chế độ sửa **không** dùng `Wnd_GetActive`. `TimODuoiChuot` (`UiToaDo.cpp:1036`) gọi `TimCuaSoGoc` rồi `TopChildFromPoint` (`WndWindow.cpp:730`), hàm này **đi xuống con sâu nhất**. Tệp mặc định của chủ có ~40 ô con đã dời/phóng riêng (`KUiToolsControlBar\|PK=…,850`, `KUiPlayerBar\|Status=330,47`…) chứng minh chọn từng icon vốn chạy | Chẩn đoán sai gốc. Gốc thật ở §3 |
| §3.4: phải viết "dò trúng cửa sổ con sâu nhất" | Đã có `TopChildFromPoint`; đệ quy `JxUi_HoiCoVatPham` (`Wnds.cpp:528`) chỉ là bản riêng cho ô vật phẩm | Không cần viết; cần **danh sách trắng** |
| §3.2: neo hiện tại "chỉ dịch một khoảng nên tỉ lệ lệch xa là hở", phải đổi sang "dính mép" | Công thức neo (`UiToaDo.cpp:234-237`): `Left += (SW − W0)·neo/2`. Với `neo=2`: khoảng cách tới mép phải = `W0 − Left` = **giữ nguyên** → **đã là dính mép** khi `ManHinh=W0,H0` đúng. Con trừ neo cha (`nNeoXCha`) nên con của khung toàn màn cũng đúng (đo được: `HideChat` ra đúng x=0 ở 1440x616) | Không cần đổi mô hình. Cái thiếu là **gốc toạ độ chưa phải vùng an toàn** và **cửa sổ do mã tự đặt** (`NeoNhomTren`, nhánh `SCREEN_WIDTH == 1024`) |
| §3.3: phải "một lượt duy nhất, cha trước con" | Bảng lưu **toạ độ tương đối cha**, áp bằng `SetPosition` ngay trong `KWndWindow::Init` (`WndWindow.cpp:434`) → thứ tự không ảnh hưởng. Bản vá `[LE]` chết vì đọc **toạ độ tuyệt đối** giữa lúc duyệt | Chỉ **lượt kẹp vào vùng an toàn** (cần toạ độ tuyệt đối) mới phải chạy sau. Chỗ chạy sau **đã có sẵn**: quét `FitToScreen` trong `Wnd_RenderWindows` (`Wnds.cpp:113-129`) — hiện là **mã chết** vì không cửa sổ nào gọi `SetFitFlags` |
| §4.2: `UiDatTiLe` phóng cả vùng bấm, "chỉnh to nhỏ từng icon vốn đã chạy được" | Đúng cho lớp kế thừa `KWndImage` (nút, ảnh, hoạt hình): vẽ `RU_T_IMAGE_STRETCH` (`WndImage.cpp:185-203`), vùng bấm chia lại theo tỉ lệ (`WndImage.cpp:107-110`). **Sai** cho: chữ (`KWndText`, nhãn của `KWndLabeledButton` vẽ cỡ font cố định), `KWndImagePart`, ảnh vật phẩm trong ô (`CoreDrawGameObj.cpp:115` "mặc định KHÔNG kéo ảnh"), **nút kỹ năng vẽ tay** (cỡ lấy từ `[Cham] KyNangCo/KyNangCoPhu` toàn cục, `JxCanDieuKhien.cpp:778`), **cần điều khiển** (không đăng ký với trình sửa) | Đúng khoảng 2/3 HUD. Phần còn lại phải làm thêm (§4) |
| §1: SDL 3.2.14 đã có `SDL_GetWindowSafeArea` | Đúng. Android: `SDLSurface.java:196` → `onNativeInsetsChanged` → `SDL_android.c:1063` → `SDL_androidvideo.c:304` `SDL_SetWindowSafeAreaInsets`. iOS: `SDL_uikitview.m:466` | Dùng được. Có hai bẫy, xem §5 |
| §5: tầng vẽ chỉ có SPIR-V | Đúng: `D3D9onGPUDev.cpp:205` `SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, …)` | iOS cần MSL, đúng như kế hoạch |
| §4.4: chụm hai ngón chưa có | Đúng. Nhưng `KSdlApp::ChamSuKien` (`KSdlApp.cpp:653-729`) **đã đọc sự kiện từng ngón** và đếm ngón (`m_nNgonDangDat`); ngón thứ hai khi đang sửa hiện bị bỏ qua | Thêm được ngay trong tệp đã rào `JX_ANDROID` |
| §4.3.7: hồ sơ theo nhân vật | Chưa có (một tệp `UserData\UiToaDo.ini` mỗi máy). **Có tiền lệ** để chép: ô kỹ năng theo nhân vật `KyNangMobile_<id>.ini` (`JxCanDieuKhien.cpp:655-676`, lấy id qua `JxCore_WAutoNhanVat`) | Làm được, phải nạp lại lúc vào thế giới rồi `UiToaDo_ApChoTatCa()` |

---

## 3. Gốc thật của "kéo một cái là cả cụm đi"

`android/du_lieu_ghi_de/ui/ui3/uiplayerbar.ini` mục `[Main]`:

```ini
Width=800
Height=600
Trans=1
Image=\Spr\UiNew\UiPlayerBar\khung_chat_mobile.spr
DummyWnd=1
```

Ba cờ này gộp lại tạo đường đi sau:

1. `DummyWnd=1` → `WND_S_SIZE_WITH_ALL_CHILD`; `KWndWindow::PtInWindow` chỉ trúng khi **một ô con** trúng.
2. `Trans=1` + có `Image` → `KWndImage::PtInWindow` (`WndImage.cpp:95-113`): khi **không con nào trúng** (`bIn == false`) mà cửa sổ là dummy, nó **đo alpha của chính ảnh nền** `khung_chat_mobile.spr` tại điểm chạm. Điểm ảnh đục → **khung gốc được coi là trúng**.
3. `TopChildFromPoint` (`WndWindow.cpp:730`): gốc trúng → `pLastMatch = this`; không con nào trúng → **trả về gốc**.
4. `TimODuoiChuot` tạo khoá `KUiPlayerBar|Main` → `SetPosition` lên gốc → `AbsoluteMove` đẩy **toàn bộ con** (`WndWindow.cpp:336-346`) = cả HUD đi theo.

Trên điện thoại, icon 28x28 nhỏ hơn đầu ngón; chạm trượt vài điểm là rơi vào nền chat đục → kéo cả cụm. Tệp mặc định có `KUiPlayerBar|Main=1,1` (không phải 0,0) cho thấy gốc đã từng bị kéo.

Ngoài ra còn hai đường "cả cụm" có chủ ý: công cụ **"Dời khối"** trên thanh nút (dính trạng thái, `s_nCongCu`), và cụm kỹ năng vẽ tay chỉ đi cả cụm khi đang "Dời khối" (`KyNang_ORiengTrungCum`, `JxCanDieuKhien.cpp:1017`).

`KUiToolsControlBar` cũng `DummyWnd=1` nhưng `Image` bị chú thích → không dính bẫy này. `KUiMiniMap|MiniMap` có ảnh và là gốc thật → chạm vào nền bản đồ là kéo cả bản đồ lẫn 4 nút (đúng ý hay không tuỳ chủ).

**Cách sửa đúng:** trình chỉnh không dò cây nữa; duyệt **danh sách trắng** các khoá HUD, lấy ô **nhỏ nhất** chứa điểm chạm (cộng lề dung sai ~8 điểm), bỏ qua alpha và cờ `Disable`. Khung gốc `KUiPlayerBar|Main`, `KUiToolsControlBar|Main` **không nằm trong danh sách** → chạm nền không làm gì. Đây cũng đúng cách bản tham khảo chọn ô (`KUiCustomPos.cpp` `onTouchBegan`: ô có diện tích nhỏ nhất chứa điểm chạm).

---

## 4. Hiện trạng "dời" và "phóng" theo từng loại ô HUD

| Loại ô | Ví dụ khoá | Dời riêng | Phóng riêng | Ghi chú |
|---|---|---|---|---|
| Nút / ảnh `KWndImage` | `KUiPlayerBar\|Status`, `KUiToolsControlBar\|PK`, `KUiMiniMap\|WorldMapBtn` | Có | Có, cả vùng bấm | Phóng neo **góc trên-trái** (`UiDatTiLe` chỉ `SetSize`): icon bám mép phải sẽ trồi ra ngoài khi phóng. Trình chỉnh phải phóng **quanh tâm** (bù `Left/Top` nửa hiệu) |
| Icon ghép gốc + con | `KUiAuctionIcon\|Main` + `AuctionBtn`, `KUiMailIcon`, `KUiCLIcon` | Có | Phải phóng **cả hai** (tệp mặc định đang lưu Main 1850 và AuctionBtn 1700) vì `SetSize` gốc không phóng con | Trình chỉnh nên phóng gốc kèm con và dời con theo tỉ lệ, hoặc chỉ mở con |
| Ô phím 1–4 | `KUiPlayerBar\|Item_0..3` | Có | Nền ô (`o_phim_nen.spr`) giãn theo (`UiPlayerBar.cpp:287-312`), **ảnh vật phẩm không** (`CoreDrawGameObj.cpp:115-144`, cờ `0x40000000` mới kéo), số đếm không | Bật cờ kéo khi ô có tỉ lệ ≠ 100 % |
| Chữ / nhãn | `DateTime`, số thứ tự ô, nhãn nút | Có | **Không** (font 12/16 cố định) | Có thể chọn font 16 khi tỉ lệ ≥ 140 %; không đẹp tuyệt đối |
| Nút kỹ năng vẽ tay | `KyNang0..8`, `KyNangGan`, `CumKyNang` | Có (từng nút, `s_nKNDoiX/Y`) | **Không** — cỡ toàn cục `KyNangCo`, `KyNangCoPhu`, `KyNangCoIcon`; giao diện ô vẽ tay chỉ có `pfnLay/pfnDat` vị trí, trường `TiLe` trong tệp bị ghi cứng 1000 (`UiToaDo.cpp:1458`) | Thêm cặp hàm lấy/đặt cỡ vào `UiToaDo_DangKyORieng` + đọc trường `TiLe` |
| Cần điều khiển | — | **Không** (không đăng ký) | Không | Chỉ chỉnh qua `[Cham] CanVungRong/CanBanKinh…` |
| Icon NPC | `IconNpc` | Có | Không | Cỡ `IconNpcRong` toàn cục |
| Danh sách / thanh cuộn | `KSysMsgCentrePad\|SysRoom`… | Có | Cấm (`CamCoGian`, `UiToaDo.cpp:412`) | Đúng, giữ cấm |

---

## 5. Vùng an toàn: đã xác minh gì

- **Android**: `SDLSurface.onApplyWindowInsets` (API ≥ 30) gộp `systemBars | systemGestures | mandatorySystemGestures | tappableElement | displayCutout` rồi gọi `onNativeInsetsChanged` → `SDL_SetWindowSafeAreaInsets`. Vì gộp cả **vùng vuốt hệ thống**, vùng an toàn sẽ rộng hơn tai thỏ: nó **tự giải quyết luôn** mục §7.2 của kế hoạch (icon `Top = 0` bị dải vuốt thanh trạng thái nuốt chạm) và vùng vuốt Back ở hai mép.
- **Bẫy 1 (Android 11–14)**: `SDLActivity.java:936-941` — sau mỗi lần đặt kiểu cửa sổ (`SDL_SetWindowFullscreen` trong `KSdlApp::Init`) SDL **đặt lại inset = 0** cho API 30–34; giá trị đúng chỉ quay lại nếu Android phát lại `onApplyWindowInsets`. Fold 7 (Android 16) không dính; máy cũ hơn phải đo, nếu 0 thì đọc lại inset ở Java rồi gọi xuống.
- **Bẫy 2**: góc bo tròn **không** nằm trong inset nào → vẫn cần một lề cố định (đề nghị `[Ui] LeAnToan=8`).
- **Chưa có số đo thật**: giả thiết "camera đục lỗ nằm ở mép ngắn khi xoay ngang" khớp với ba mục đang ở `x = 0` (`HideChat` ở y ≈ 301, đúng giữa chiều cao 616) nhưng **chỉ chứng minh được bằng bước 1 của kế hoạch** (bộ thu số liệu, ghi thêm `SDL_GetWindowSafeArea`). `sensorLandscape` cho phép xoay 180°, lỗ camera có thể sang mép phải → vùng an toàn xử lý được cả hai, bảng toạ độ tay thì không.
- Đổi từ toạ độ cửa sổ sang khung vẽ dùng đúng công thức `SdlToLogical` (`KSdlApp.cpp:552`): trừ lề letterbox rồi chia hệ số.
- **iOS**: `SDL_uikitview.m:466` đặt safe area từ `safeAreaInsets` của UIKit → cùng một đoạn mã C++ dùng lại nguyên.

---

## 6. Phương án đề nghị (A', chi tiết theo mã)

### 6.1 Không gian bố cục = vùng an toàn, giữ nguyên mô hình neo
Trong `DoiCaTepTheoNeo` (`UiToaDo.cpp:184`): thay `SCREEN_WIDTH/HEIGHT` bằng `safeW/safeH` và cộng `(safeL, safeT)` cho **cửa sổ gốc** (con vẫn tương đối cha, không đổi). Rào `JX_MOBILE`, công tắc `[Ui] VungAnToan=1`, ghi log giá trị inset.

### 6.2 Một lượt kẹp sau khi dựng cây
Tái dụng quét `FitToScreen` trong `Wnd_RenderWindows` (đang chết): với các khoá trong danh sách trắng, nếu hình chữ nhật tuyệt đối ra ngoài vùng an toàn thì dời vào (con: sửa toạ độ tương đối). Chạy **một lần** khi cửa sổ vừa `Init` (cờ `m_bNeedFit` đã có) và lại một lần khi thoát trình chỉnh. Không kẹp cửa sổ hộp thoại (đang tự căn giữa).

### 6.3 Trình chỉnh cho người chơi (viết mới, dựa hàm sẵn có)
- **Chọn**: danh sách trắng (§3). Chạm một cái = chọn (viền sáng, tên + `x, y, %`), không kéo ngay.
- **Kéo**: sau khi chọn, kéo ở bất kỳ đâu trên màn (không cần đặt đúng lên icon). Bắt dính lưới 4 điểm và mép/tâm ô khác trong danh sách, vẽ đường gióng bằng `KRULine` (`WndWindow.cpp:848` đang dùng).
- **Bảng nổi**: 4 mũi tên (1 điểm, giữ chạy đều qua `NhipCham`), `−`/`+` 5 % (nấc `UITOADO_TILE_NAC` sẵn), `Ẩn`, `Mặc định` (xoá khoá khỏi bảng người chơi → rơi về lớp mặc định), `Hoàn tác` (ngăn xếp `(khoá, bản ghi cũ)` — chưa có, thêm ~30 dòng), `Xong` (= `UiToaDo_BatTat()` ghi tệp). Vẽ bằng `OKhoi`/`VeChu` sẵn có, hoặc ảnh VNKU `khung.png`/`nut_keo.png`/`btn_xong` đổi sang `.spr` bằng `ghi_spr_nhieu_khung`.
- **Chụm hai ngón**: trong `ChamSuKien`, khi `UiToaDo_DangSua()` và ngón thứ hai chạm xuống: huỷ kéo đang dở, vào trạng thái chụm, tỉ lệ = khoảng cách hiện tại / ban đầu × tỉ lệ lúc bắt đầu, làm tròn 5 %, gọi `UiDatTiLe` **quanh tâm**.
- **"Một icon / cả cụm"**: giữ công cụ "Dời khối" nhưng đổi thành công tắc có nhãn rõ và **tự trả về "một icon" khi thoát**.
- **Hồ sơ**: `UserData\UiToaDo_<idNhânVật>.ini`, thiếu thì dùng `UiToaDo.ini`; nạp lại khi vào thế giới (nơi gọi: mẫu `KyNang_DocGan`), rồi `UiToaDo_ApChoTatCa()`. "Chép bố cục bằng mã ngắn": tệp ~150 dòng, mã sẽ dài; đề nghị **lùi**, làm xuất/nhập qua bảng tạm trước.
- **Xem thử cỡ màn khác**: chỉ cần vẽ khung chữ nhật của vùng an toàn máy hẹp hơn lên trên (không cần đổi khung vẽ).

### 6.4 Lưới an toàn thay cho phương án B
Thay vì khung ảo cố định + letterbox (viền đen lớn), **cố định chiều cao khung vẽ = 616** trong `JxSdl_ChotDoPhanGiaiTheoManHinh` (bỏ nấc 1,00/1,25/… , cho hệ số lẻ) → mọi máy chỉ khác **bề ngang**, bài toán neo còn một chiều, 27 khung vẽ gom về một họ. Giá phải trả: hệ số lẻ mờ hơn nấc tròn không đáng kể (Fold 7: 1,753 so với 1,75). Máy ảo 604 px sẽ bị phóng nhẹ (>1) — chỉ là môi trường dựng thử. **Chủ quyết.**

### 6.5 Phóng to hoàn thiện (§4)
- `UiDatTiLeQuanhTam()` mới, dùng chung cho nút, chụm, bảng.
- Ô vẽ tay: thêm `pfnLayCo/pfnDatCo` vào `KORieng`, lưu `TiLe` thật thay 1000; cụm kỹ năng đọc cỡ riêng từng nút thay `KyNang_CoNut`.
- Ô phím: bật `0x40000000` khi ô có tỉ lệ ≠ 1000.
- Đăng ký cần điều khiển và icon NPC với cỡ.

---

## 7. Rào bản PC và chuẩn bị iOS

- `UiToaDo` (Ctrl+U, tệp `Left,Top,TiLe,Co`) **đã nằm trên `main` từ 07/09** (`2c52a6b2`, `f9da397a`) → là chuẩn PC.
- Nhánh mobile thêm **1.075 dòng** vào 6 tệp giao diện dùng chung (`UiToaDo.cpp` +807, `WndWindow.cpp` +133, `Wnds.cpp` +84…). Trong `UiToaDo.cpp` khoảng **467 dòng thêm nằm ngoài rào** `JX_ANDROID` (thanh nút chạm 09/09 B, ô vẽ tay 09/09 E, lớp mặc định 10/09 G, phần đọc/ghi neo). Chúng **trơ trên PC** vì chỉ chạy khi `[Ui] SuaToaDo=1` hoặc khi có tệp `Ui\UiToaDo_MacDinh.ini` (chỉ có trong lớp ghi đè Android), nhưng bản PC dựng từ `mobile-0809` **khác nhị phân** so với `main`. Nếu PC phát hành từ `main` thì không sao; nếu không, nên có một đợt dọn đưa các khối này vào rào — việc riêng, chủ quyết.
- **Mọi mã mới của đợt này rào `JX_MOBILE`** (thêm vào `JX_COMMON_DEFS` ở `android/CMakeLists.txt:36`; iOS sau này định nghĩa `JX_MOBILE` + `JX_IOS`). Chưa có chỗ nào định nghĩa `JX_MOBILE`.
- iOS: bố cục, chạm, vùng an toàn dùng lại nguyên; chặn thật là shader MSL (§5 kế hoạch đúng).

---

## 8. Thứ tự làm đề nghị và nghiệm thu

| Bước | Việc | Nghiệm thu |
|---|---|---|
| 1 | Bộ thu số liệu máy thật: khung vẽ, hệ số + lề letterbox, **vùng an toàn**, hình chữ nhật tuyệt đối của mọi khoá danh sách trắng; gửi lên máy chủ 8765 | Có tệp từ Fold 7 (cả hai chiều xoay) |
| 2 | Danh sách trắng + chọn bằng chạm + bảng nổi + hoàn tác (§6.3, chưa cần chụm) | Chủ tự dời/phóng từng icon, chạm nền không đi cả cụm |
| 3 | Vùng an toàn làm gốc + lượt kẹp (§6.1, §6.2), công tắc config | Ba mục `x = 0` vào trong trên Fold 7, PC và máy ảo không đổi |
| 4 | Phóng hoàn thiện cho ô vẽ tay, ô phím (§6.5) | Nút kỹ năng và ảnh vật phẩm to theo nấc 5 % |
| 5 | Chụm hai ngón, bắt dính + đường gióng, hồ sơ theo nhân vật | Đổi nhân vật là đổi bố cục |
| 6 | Sinh lại tệp mặc định theo vùng an toàn; cập nhật `sinh_bocuc_rong.py` cho tệp theo nhân vật | Máy mới không thừa hưởng bố cục máy khác |
| 7 | (tuỳ chủ) cố định chiều cao 616 (§6.4) | 27 khung → một họ |

Bước 1 và 2 độc lập, làm song song được; 2 đi trước 3 vì chủ muốn tự chỉnh ngay và 3 cần số đo của 1.

## 9. Câu hỏi cần chủ quyết

1. Bản PC phát hành dựng từ `main` hay từ `mobile-0809`? (quyết định có cần dọn 467 dòng ngoài rào hay không)
2. Có chấp nhận cố định chiều cao khung vẽ 616 (hệ số lẻ, máy ảo 604 bị phóng nhẹ) để bài toán neo còn một chiều không?
3. Danh sách trắng: mở bao nhiêu mục — chỉ HUD (~40 khoá liệt kê trong `uitoado_macdinh.ini`) hay cả bản đồ nhỏ, khung chat, cần điều khiển?
4. Bản đồ nhỏ: kéo nền bản đồ có đi cả 4 nút không?
5. "Chép bố cục bằng mã ngắn" có cần trong đợt này không, hay lùi sang xuất/nhập tệp?
