# VIỆC CHO PHIÊN ANDROID — rút từ đợt làm iOS 16/09/2026 (để hai bản đồng bộ)

> Gửi phiên Android. Mọi số liệu dưới đây đo trên manifest thật của PC `10.0.0.140:8765` (688 tệp, 7,81 GB) và trên iPhone
> của chủ ngày 16/09. Chi tiết dài hơn ở `BANGIAO_PHIENSAU_IOS_1609.md` §2b. Bên iOS **đã** làm xong phần của mình; phần
> dưới là phần cần làm ở Android / máy chủ PC để cùng một kho dữ liệu chạy đúng trên cả hai nền.

## Ưu tiên 1 — ảnh hưởng người chơi, sửa sớm

### 1. Tên tệp chữ HOA Latin-1 trong kho: game Android cũng không thấy (LDPlayer 9)
- **Hiện trạng:** manifest PC có **142/688** đường dẫn chứa chữ hoa: 27 `spr/ui3/FortuneRank/*.spr` (ASCII) và 115 tên Latin-1
  kiểu `maps/ÌØÊâÓÃµØ/Ñ×µÛ±¦²Ø24.jpg`, `maps2/…`, `spr/npcres/man/ÌØÐ§¹Ò¼þ/…`, `spr/skill/…`, `spr/ui3/µÇÈë½çÃæ/…`
  (245 MB; bản đồ Tống Kim, hiệu ứng trang sức, màn đăng nhập).
- **Vì sao hỏng:** game mở tệp qua `JxPathPosix` (`Sources/Engine/Src/Platform/KPosixWin32.cpp:134-157`): hạ chữ thường
  ASCII **và** Latin-1 (`U+00C0–U+00DE` trừ `×` cộng 0x20; `Š/Œ/Ž` +1; `Ÿ→ÿ`) rồi UTF-8 NFC. Bộ tải Android ghi tệp đúng tên
  manifest (chữ hoa, `TaiDuLieuActivity.java:458`). `/storage/emulated/0` trên Android ≤ 10 (sdcardfs, LDPlayer 9) chỉ gập
  hoa/thường **ASCII** (kernel `strcasecmp` theo byte), byte UTF-8 thứ hai của `Ì` (C3 **8C**) và `ì` (C3 **AC**) không gập
  → 115 tệp Latin-1 hoa **không tìm thấy** trên Android; 27 tệp FortuneRank thì thấy (ASCII). Windows NTFS không phân
  biệt nên PC không lộ; iOS (APFS) phân biệt cả hai nên lộ hết — iOS đã tự chuẩn hoá tên trên máy.
- **Sửa (gốc, khuyến nghị):** hạ chữ thường Latin-1 ngay trên cây dữ liệu PC cho các thư mục còn sót (`maps/`, `maps2/`,
  `spr/npcres/`, `spr/skill/`, `spr/ui3/`…) — `android/chuan_bi_du_lieu.ps1` đã có bước `ToLower` Latin-1 nhưng các thư mục này
  không đi qua nó. Sau đó chạy lại `may_chu_tai_du_lieu.py --chi-manifest`. iOS không cần tải lại (bộ tải iOS đổi tên biến
  thể hoa → chuẩn, đã thử 13/13).
- **Sửa (phòng thủ, thêm):** trong `TaiDuLieuActivity.taiMot`, tên trên đĩa = tên chuẩn hoá như trên (URL vẫn tên gốc);
  khoá `da_tai.txt` cũng chuẩn hoá. Mẫu ở `ios/JxTaiDuLieu.mm` hàm `JxChuanHoaDuongDan` (20 dòng).
- **Kiểm:** trên LDPlayer vào bản đồ Tống Kim (`maps2/中原北区/宋金战场24.jpg`) xem ảnh nền có hiện không; hoặc
  `adb shell ls /storage/emulated/0/Android/data/vn.jx1.mobile/files/maps` xem tên hoa/thường.

### 2. Ký manifest ngay trên PC (hiện `manifest.sig` trả 404)
- `may_chu_tai_du_lieu.py` `_ky_lai()` **xoá** `manifest.sig` khi không thấy khoá riêng; khoá đang nằm ở Mac
  `~/.jx1_khoa/jx1_manifest_ec.key`. Bản iOS từ chối kho không chữ ký → 16/09 phải dựng trạm ký trung gian trên Mac
  (`ios/tram_ky.py`) để thử.
- **Sửa:** chủ chép khoá sang PC `%USERPROFILE%\.jx1_khoa\jx1_manifest_ec.key` (hoặc đặt `JX_KHOA_KY=<đường dẫn>`), khởi
  động lại máy chủ; kiểm `curl http://10.0.0.140:8765/manifest.sig` phải ra 97 byte base64. Khoá riêng **không** đưa vào git.

### 3. Đệm md5 theo giây làm manifest mang md5 cũ
- `may_chu_tai_du_lieu.py:98-99` đệm md5 theo `(cỡ, int(st_mtime))`. Ghi cùng một tệp hai lần trong **một giây** với cùng cỡ
  → manifest mang md5 **cũ** → iOS (kiểm md5) báo "md5 sai" mãi cho tới khi xoá `manifest_cache.txt`. Gặp thật 16/09 với
  `phienban.txt`. Android không lộ vì không kiểm md5 (xem mục 5).
- **Sửa:** dùng `st.st_mtime_ns` làm khoá đệm (đổi khoá = băm lại 9 GB **một lần**).

## Ưu tiên 2 — đúng như iOS đã sửa, nên làm cho khớp

### 4. Tệp 0 byte không bao giờ tải được
- `TaiDuLieuActivity.taiMot` (~dòng 496): tệp cỡ 0 không tạo `.part` nên `renameTo` thất bại → kẹt. iOS đã gặp đúng lỗi này
  (`system/spr/regiontiledefault.spr` 1 byte suýt trúng; manifest hiện chưa có tệp 0 byte nhưng chỉ cần một tệp là kẹt).
- **Sửa:** `co == 0` → tạo `.part` rỗng trước bước đổi tên.

### 5. Android không kiểm md5 sau khi tải
- `taiMot` chỉ so **cỡ**, rồi `ghiDaTai` ghi md5 **của manifest** như thể đã kiểm. Manifest đã ký mà tệp tải về không kiểm
  thì chữ ký vô nghĩa với tệp. iOS băm md5 từng tệp sau khi tải.
- **Sửa:** băm md5 tệp `.part` trước khi đổi tên, sai thì xoá và báo lỗi (như iOS `JxTaiMotTep`).

### 6. Bố cục mặc định đổi: xoá cả tệp theo nhân vật
- `donPakCu`/khối `[UITOADO 12/09 NEO c]` chỉ xoá `userdata/UiToaDo.ini`; `UiToaDoMobile.inc:293-303` **ưu tiên**
  `userdata/UiToaDo_<id>.ini` theo nhân vật → nhân vật đã có tệp riêng vẫn bị đè bố cục cũ. iOS xoá cả `uitoado.ini` lẫn
  `uitoado_*.ini` khi `ui/uitoado_macdinh*` vừa được tải.

### 7. Dọn tệp mồ côi rộng hơn `data/*.pak` (tuỳ chọn)
- iOS sau khi đồng bộ online trọn vẹn xoá mọi tệp trong các thư mục cấp 1 của manifest mà không có trong manifest (giữ
  `userdata/`, `apdata/`, tệp ở gốc, `.part` đang tải). Android chỉ dọn `data/*.pak`. Không bắt buộc, nhưng nếu làm mục 1 theo
  cách phòng thủ (chuẩn hoá tên trên máy) thì máy Android cũng sẽ có bản hoa thừa cần dọn.

## Ưu tiên 3 — thông tin, tự quyết

- **`manifest_dakiem.txt`** đã được thêm vào `BO_TEP` của `may_chu_tai_du_lieu.py` (tệp chỉ sinh trên iOS; loại cho đối xứng
  với `da_tai.txt`). Không cần làm gì.
- **`config.ini` không "chỉ gieo lần đầu":** game trên mobile không ghi `config.ini` (mọi thiết đặt vào `userdata/`, trừ hai
  hàm Lua thời PC `Set3D`/`SetFullWindow`), nên bộ tải iOS đồng bộ nó như mọi tệp. Nếu Android đang giữ `config.ini` riêng thì
  biết để không lệch nhau.
- **Địa chỉ máy chủ dải riêng:** bản iOS App Store tự bỏ `10.0.0.140` và region chỉ có địa chỉ riêng ngay trong mã
  (`Login.cpp`, rào `defined(JX_IOS) && !JX_IOS_NOI_BO`, có bảng ánh xạ chỉ số danh sách → `Region_n` vì `GetServerList` dùng
  chỉ số làm số section). Tệp `serverlist` dùng chung **không đổi**. Bản Play Store nếu cần cùng luật thì rào theo cờ phát
  hành của Android tương tự — tự quyết.
- **Nút "Tố cáo"** (App Store 1.2) hiện chỉ iOS (`UiGame.h/.cpp` rào `JX_IOS`): thực đơn lên người chơi khác → xác nhận → tin
  riêng `[TO CAO] <tên> - hh:mm dd/mm` tới `GM` (quy ước sẵn của máy chủ) + `userdata/tocao.log`. Relay ghi mọi tin riêng vào
  `s3relay_log\ChatSomeOne*.log`. Google Play cũng đòi cơ chế báo cáo UGC → nếu muốn dùng chung thì đổi rào sang `JX_MOBILE`
  (chuỗi TCVN3 đã viết bằng `\x`, thuần ASCII).
- **Nút Thoát** giấu chỉ trên iOS (`UiInit.cpp` rào `JX_IOS`); Android có `moveTaskToBack`, giữ nguyên.
- **IPv6** (`SocketClient.cpp` rào `JX_APPLE`): Android có 464XLAT cho địa chỉ IPv4 số nên không cần.
- **Kiểm rào:** mọi khối iOS thêm vào `Sources/` đều `#ifdef JX_IOS`/`JX_APPLE` với `#else` giữ mã cũ; `ios/kiem_rao.py`
  ĐẠT 9 tệp ở cả hai chế độ. Nếu phiên Android sửa các tệp đó (`SocketClient.cpp`, `Login.cpp`, `NetConnectAgent.cpp`,
  `UiInit.cpp`, `ShortcutKey.cpp`, `UiGame.h/.cpp`, `GameSpaceChangedNotify.cpp`) thì giữ nguyên các khối rào đó.

## Thứ tự đề nghị
1 (đổi tên trên PC, rồi `--chi-manifest`) → 2 (chép khoá, ký trên PC) → 3 (`st_mtime_ns`) → 4, 5, 6 → 7 tuỳ chọn.
