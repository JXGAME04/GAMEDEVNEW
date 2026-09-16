# BÀN GIAO CHO PHIÊN SAU — PHÁT HÀNH iOS (chốt 16/09/2026)

> Đọc tệp này trước. `BANGIAO_PHIENSAU_IOS_1509.md` vẫn đúng ở mọi phần không nhắc lại ở đây (quyết định chủ đã chốt §5,
> lỗi đã mắc §6, bẫy công cụ §8). `THIETKE_CAPNHAT_IOS_MAC_1509.md` là thiết kế gốc, nhưng **§4 (HMAC) và §5.3 (chỉ gieo
> `config.ini`) đã bị phản biện bác** — xem §5 dưới đây.

---

## 1. Tình trạng git

Nhánh `mobile-0809`. Đã gộp tiến thẳng 4 commit của phiên khác trong ngày (bộ công cụ mổ nhị phân; nền đen khi di
chuyển — không chạm `ios/`). Chủ cho phép lúc 11:15 → **đã commit `a0d2d66c` và đẩy lên `origin/mobile-0809`** (tiến thẳng),
gồm các tệp:

```
android/may_chu_tai_du_lieu.py   ios/CMakeLists.txt   ios/Info.plist   ios/JxIosAnGame.mm   ios/JxIosDuongDan.mm
ios/JxIosMain.cpp                ios/JxIosNhatKy.mm   ios/JxTaiDuLieu.mm   ios/dung_ban_phat_hanh.sh
BANGIAO_PHIENSAU_IOS_1509.md (2 dòng con trỏ)   BANGIAO_PHIENSAU_IOS_1609.md (mới)
```
Số liệu diff: 860 dòng thêm, 208 dòng bỏ; `ios/JxTaiDuLieu.mm` chiếm 797.

iPhone "An Nguyen" đang chạy đúng bản Release của commit này (cài 11:17, không còn nút ẩn game).

---

## 2. Đã làm hôm nay — A3 xong, B2 + B3 xong, cơ chế của A1 xong (chỉ còn thiếu địa chỉ), thêm C3 nửa đầu và "A trước B"

Cách làm theo luật của chủ: **4 phản biện đối kháng song song chạy xong rồi mới sửa mã**; kết luận bị bác ghi ở §5.

| Tệp | Nội dung |
|---|---|
| `ios/JxTaiDuLieu.mm` (viết lại phần lõi, 1374 dòng) | **Dấu hoàn tất** = manifest **đã kiểm chữ ký** giữ ở `<Documents>/manifest_dakiem.txt` (dòng đầu = chữ ký base64, sau đó là `manifest.txt` nguyên byte; MỘT tệp, ghi tạm + fsync + rename). **Máy trạng thái D/E/F/A** (§3). Thời hạn manifest **8 s** khi dữ liệu đã đủ theo manifest đã lưu, **30 s** khi chưa; nhiều gương cách nhau bằng dấu cách. `phienban.txt` tải + kiểm **trước** khi tải hàng GB. **Hỏi ý 4.2.3(ii)**: từ 50 MB hiện dung lượng + nút "Tải xuống" (một nút, không bỏ qua). Lời báo lỗi **cho người chơi** (có dấu, kèm một dòng tiếng Anh), chi tiết kỹ thuật cỡ nhỏ bên dưới; phân biệt không mạng / hết giờ / máy chủ lỗi / chữ ký sai / cổng Wi-Fi công cộng (200 + HTML). Sửa 2 lỗi chặn phản biện tìm ra: `JxTaiDongBo` giữ `NSData` không retain (dùng-sau-giải-phóng), tệp **0 byte** không bao giờ tải được. Sửa thêm: URL mã hoá NFD (tên có dấu → 404 trên máy chủ Windows/Linux) → dùng `UTF8String`; 200-khi-xin-Range ghi thẳng thay vì tải gấp đôi; thanh tiến độ âm khi tải lại; `da_tai.txt` ghi gọn lại sau mỗi lần xong (chống phình + chống một byte hỏng làm băm lại 8,5 GB mãi); nhớ md5 đã băm trong tiến trình để "Thử lại" không băm lại; `finishTasksAndInvalidate`; dealloc cho lớp trạng thái (MRC); bỏ nhánh Latin-1 chết. |
| `ios/JxIosMain.cpp` | Địa chỉ kho: `may_chu_tai.txt` **chỉ khi `JX_IOS_NOI_BO`**, không có thì `JxKhoDuLieu` trong Info.plist. Gọi `JxTaiDuLieu_DatPhienBanApp(JX_PHIEN_BAN_APP)`. `JX_DATA_DIR`, `jx_data_dir.txt` cũng chỉ ở cây thử. Lời chặn có dấu + tiếng Anh. |
| `ios/JxIosDuongDan.mm` | `JxIos_KhoDuLieu()` đọc khoá `JxKhoDuLieu`. |
| `ios/JxIosAnGame.mm` + `Sources/S3Client/Ui/UiCase/UiPlayerBar.cpp` | **Chủ chốt 16/09: bỏ hẳn nút "Ẩn game" trên Apple.** `UiPlayerBar.cpp` không `AddChild(&m_HideWindow)` trong rào `JX_APPLE` (mẫu `#ifdef JX_APPLE` chú thích `#else` mã cũ `#endif`, kiểm rào ĐẠT); trình "Sửa giao diện" duyệt cây con thật nên không thấy nút nữa. `JxIosAnGame.mm` **không còn được dịch** cho iOS (giữ tệp vì `macos/CMakeLists.txt:162` còn liệt kê); dòng log `[IOS-AN]` bỏ. Nhị phân cả hai cấu hình: 0 ký hiệu `JxIosAnGame`, 0 chuỗi `suspend`. |
| `ios/JxIosNhatKy.mm` | Bản phát hành không đọc `may_chu_nhatky.txt`/`may_chu_tai.txt`, không gửi nhật ký (bị loại khỏi link luôn). |
| `ios/CMakeLists.txt` | `option(JX_IOS_KHOA_NOI_BO ON)`; `JX_IOS_KHO_DU_LIEU` (CACHE STRING, escape XML `& < >`); `message(STATUS)` ghi hai giá trị vào `cauhinh.log`. Macro `JX_IOS_NOI_BO=0/1` vào `JX_COMMON_DEFS` (đổi là dịch lại ~456 đơn vị; Debug 37 s, Release ~35 s). |
| `ios/Info.plist` | Hai khoá chia sẻ tệp **và** hai khoá LAN (`NSAllowsLocalNetworking`, `NSLocalNetworkUsageDescription`) chỉ còn khi ON, qua `@JX_IOS_PLIST_NOI_BO@`; thêm `JxKhoDuLieu`. |
| `ios/dung_ban_phat_hanh.sh` | Luôn `-DJX_IOS_KHOA_NOI_BO=OFF`. **Chốt 3**: kho rỗng → HỎNG (trừ `JX_IOS_CHO_PHEP_KHO_RONG=1`); có kho → tải `manifest.txt`+`.sig` từ **từng gương**, kiểm chữ ký bằng đúng khoá trong `JxTaiDuLieu.mm`, `phienban.txt` không được đòi cao hơn `JX_PHIEN_BAN_APP`; NGHIÊM đòi `https://`. **Chốt 4** trên gói: 4 khoá thợ phải thiếu, `JxKhoDuLieu` phải có và khớp, `strings` đối chứng dương rồi `suspend` = 0, `may_chu_tai.txt` = 0, `__objc_methname` không có `suspend`. |
| `android/may_chu_tai_du_lieu.py` | Thêm `manifest_dakiem.txt` vào `BO_TEP` (tệp chỉ sinh trên máy iOS; loại cho đối xứng với `da_tai.txt`). Phiên Android: không cần làm gì. |

---

## 3. Máy trạng thái lúc mở app (đã cài, đã thử)

| | Điều kiện | Hành vi |
|---|---|---|
| **C** | manifest online hợp lệ, khớp hết | vào game; lưu đè `manifest_dakiem.txt` |
| **B** | manifest online hợp lệ, có tệp khác | ≥ 50 MB: màn "Cần tải X — [Tải xuống]"; rồi tải, không bỏ qua |
| **A** | `phienban.txt` (trong manifest đã ký) đòi bản cao hơn | **chặn hẳn** ngay sau manifest, **trước** khi tải GB; không nút (chưa có link App Store) |
| **D** | không lấy được manifest hợp lệ (mất mạng, hết giờ, 4xx/5xx, chữ ký sai, cổng Wi-Fi) **và** dữ liệu đủ theo manifest đã lưu | hiện lý do + "Không kiểm tra được cập nhật, dùng dữ liệu đã có" 1,5 s rồi vào game; `main()` ghi log câu đó |
| **E1** | như trên nhưng **chưa có** manifest đã lưu | "…Dữ liệu trong máy chưa đủ để chơi. Hãy kiểm tra mạng rồi bấm Thử lại." |
| **E2** | như trên, có manifest đã lưu nhưng dữ liệu **chưa đủ** theo nó | "Có bản cập nhật chưa tải xong (N tệp, X MB). <lý do>. …Thử lại" |
| **F** | tải được nhưng chữ ký sai | xử như D/E, **không** ghi đè manifest đã lưu |

Mọi thất bại **sau** khi đã có manifest hợp lệ (404 tệp, md5 sai, hết chỗ) là E — không lối vào game. Ghi `manifest_dakiem.txt`
**trước** khi tải: máy đã biết có bản mới thì mất mạng cũng không được chơi với bản cũ (đúng "ép cập nhật"); mặt trái:
rải bản dở (manifest lên trước, tệp 404) là mọi máy mở app lúc đó bị giữ lại tới khi kho đủ.

**Không HMAC/Keychain** (thiết kế §4): ECDSA trên manifest đã lưu đã trả lời "manifest thật". Nó **không** trả lời "mới nhất":
chữ ký không có dấu thời gian, một cặp cũ hợp lệ vĩnh viễn. Cổng thật vẫn phải ở máy chủ game lúc đăng nhập.

---

## 4. Kiểm chứng đã làm

1. **Bộ thử trên macOS** (bộ nhớ `bo-thu-bo-tai-tren-macos`): dịch riêng `JxTaiDuLieu.mm` + trình chạy 20 dòng + máy chủ
   `may_chu_tai_du_lieu.py` cục bộ + khoá ký thật `~/.jx1_khoa`. **31/31 ĐẠT**: C, B, D, E1, E2, F→D, cụt/mất
   `manifest_dakiem.txt`, mất `da_tai.txt` (băm lại → D, sinh lại 6 dòng), cổng Wi-Fi 200+HTML, cổng phiên bản, ép cập nhật
   với kept v2 + tệp 404 → E rồi khôi phục → D, không sót `.part`. Thời hạn đo được: **8,05 s** (có kept) / **30,08 s** (không).
2. **Dựng**: `build/ios-dev` Debug + Release (ON) sạch; cây `build/ios-kiem` (OFF, kho giả có `&`, không ký) sạch — plist
   thiếu đủ 4 khoá thợ, `JxKhoDuLieu` giữ nguyên `&`, `strings`: `suspend` 0 (chỉ còn `suspended` của Lua), `may_chu_tai.txt` 0,
   `__objc_methname` 0, `JxNhatKy_Bat` không còn được link. Chuỗi giao diện mới nằm trong nhị phân (UTF-16, đối chứng
   bằng chuỗi cũ "Chuẩn bị dữ liệu").
3. `ios/kiem_rao.py` cả hai chế độ: ĐẠT — **rỗng nghĩa** (không đụng `Sources/`).
4. Máy ảo `jx1-ios-test`: xem §4b (điền sau khi chạy).

### 4b. Máy ảo `jx1-ios-test` (cây `build/ios-sim`, `JxKhoDuLieu=http://127.0.0.1:8766/`, kho thử 63 MB)

| Kịch bản | Kết quả |
|---|---|
| Cài mới, máy chủ sống | manifest → `phienban.txt` tải trước → màn **"Cần tải 62.9 MB dữ liệu game để chơi. Nên dùng Wi-Fi." + nút Tải xuống** → bấm → tải xong 63 MB → `manifest_dakiem.txt` = chữ ký + manifest kho (nguyên byte) → game khởi động (hỏng "Represent2.dll" vì dữ liệu giả) → màn **"Không khởi động được"**, không màn đen |
| Máy chủ tắt, dữ liệu đủ | `[1587] [IOS-TAI] ket qua 0: Không nối được máy chủ dữ liệu. Không kiểm tra được cập nhật, dùng dữ liệu đã có.` → vào game sau 1,6 s (**D**) |
| Máy chủ tắt, xoá `main.lua` | màn **E2** "Có bản cập nhật chưa tải xong (1 tệp, 1 KB). Không nối được máy chủ dữ liệu. Dữ liệu trong máy chưa đủ để chơi…" + dòng kỹ thuật `offline, con 1 tep (15 B) theo manifest da kiem (HTTP 0, loi -1004)` + nút Thử lại |
| Kho đòi `phienban.txt` 20270101 | màn **A** "Bản game đã cũ — Bạn đang dùng bản 20260912, máy chủ yêu cầu từ bản 20270101 trở lên…" **không nút**, chặn trước khi tải tệp nào |
| Kho trả về 20260901, mở lại online | `[43] [IOS-TAI] ket qua 0` — tải bù `phienban.txt` + `main.lua` (**B → C**) |

Hai lỗi thấy trên máy ảo, đã sửa: nút Thử lại **đè lên tiêu đề** (cả hai neo vào `chiTiet.top` từ 15/09 → nút nay neo trên `nhan.top`);
các dòng `[IOS-TAI]` **mất khỏi `jx_ios.log`** vì `jx_fopen("jx_ios.log")` giải tương đối với CWD "/" trước khi `chdir`
(→ `JxIosLog` ghi thẳng đường tuyệt đối trong Documents).

**Lỗi máy chủ (báo phiên Android):** `may_chu_tai_du_lieu.py` đệm md5 theo `(cỡ, int(mtime))`; ghi cùng một tệp hai lần
trong **một giây** với cùng cỡ là manifest mang md5 **cũ** → mọi máy iOS báo "md5 sai" tới khi xoá `manifest_cache.txt`.
Gặp thật khi thử 16/09 (`phienban.txt`). Sửa: dùng `st.st_mtime_ns` (đổi khoá đệm = băm lại 9 GB một lần).

### 4c. iPhone của chủ ("An Nguyen", 11:06 16/09)
Cài `build/ios-dev/Release-iphoneos/jx1ios.app` bằng `devicectl` (lần gọi đầu "Connection reset by peer", gọi lại là được),
khởi chạy từ xa: `jx_ios.log` ghi `[IOS-TAI] khong co dia chi kho du lieu -> khong dong bo` → tìm thấy dữ liệu → vào game
bình thường, gửi nhật ký về PC vẫn BẬT (`may_chu_nhatky.txt` có sẵn). Máy này **không** đi qua bộ tải mới vì không có
`may_chu_tai.txt` và `JxKhoDuLieu` rỗng — muốn thử màn tải trên iPhone phải có kho thật (và chấp nhận `config.ini` bị ghi đè).

---

## 5. Phản biện 16/09 — kết luận nào của tài liệu 15/09 bị BÁC

1. **§5.3 "config.ini chỉ gieo lần đầu" — BÁC.** Game trên mobile **không ghi** `config.ini`: mọi thiết đặt vào `userdata\`
   (`UiBase.cpp:65,691-695`, `UiShell.cpp:282-287`, `Login.cpp:815-818`, `JxLiaCanh.cpp:197`), máy chủ đã loại thư mục đó.
   Chỉ hai hàm Lua thời PC `Set3D`/`SetFullWindow` (`ShortcutKey.cpp:1588,1655`) mới ghi. Gieo-một-lần = chủ **mất** khả năng
   đẩy `config.ini` xuống máy đã cài. **Giữ đồng bộ như mọi tệp.** Hệ quả cho máy phát triển của chủ: khi A1 có địa chỉ
   thật, iPhone của chủ sẽ bị ghi đè `config.ini` chỉnh tay (`PaintFps=60`…) — chủ quyết (cờ giữ cục bộ nếu cần).
   `camera_mobile.ini` chỉ được đọc (`JxLiaCanh.cpp:595`), càng không được gieo-một-lần.
2. **Thời hạn 3 s — BÁC.** 3000 ms của Android là số chỉnh cho LAN; trên mạng di động DNS + TLS + đánh thức sóng vượt 3 s
   là thường → D giả = đường vòng qua ép cập nhật. Chốt 8 s (= `THOI_HAN_MS` Android). **Chưa đo trên 4G thật.**
3. **§4 HMAC + Keychain — bỏ**, lý do §3.
4. **Thiết kế §3 dòng 112 "D cứu ta khỏi rớt 2.1 khi họ duyệt lúc kho hỏng" — SAI.** Người duyệt **cài mới**, không có byte
   nào → không bao giờ vào D, chỉ có E. Thứ cứu vòng duyệt là **kho phải sống** (CDN, 2 gương, đóng băng kho khi duyệt).
5. Hai lỗi chặn có sẵn từ trước (đã sửa): `JxTaiDongBo` gán `NSData` của block không retain (NSZombie bắt được, cùng lớp
   lỗi sập 14/09); tệp **0 byte** trong manifest → "co -1, manifest ghi 0" → kẹt Thử lại vĩnh viễn (Android cũng hỏng
   chỗ này: `renameTo` `.part` không tồn tại, `TaiDuLieuActivity.java:496` — **báo phiên Android**).
6. Bẫy CMake: `configure_file` thay biến **không tồn tại** bằng rỗng, không báo → script kiểm "khoá thiếu" tách khỏi "rỗng".

---

## 6. Chủ cần quyết (mới, ngoài B4 / C1 / C2 của tệp 1509)

1. ~~Nút "Ẩn game" trên Apple~~ — **chủ đã chốt 16/09: bỏ hẳn**, đã làm (xem §2). Dữ liệu chung
   `android/du_lieu_ghi_de/ui/uitoado_danhsach.ini:75` vẫn ghi "Nút ẩn game" cho Android; trên iOS mục đó trỏ tới nút
   không tồn tại nên vô hại.
2. **Nút Thoát vô hiệu trên iOS** (chốt 15/09 "giữ nút nhưng bấm không thoát") — phản biện xét duyệt xếp là mẫu 2.1 "nút không
   phản ứng". Cách vẫn đúng lời chủ: bấm về màn chọn máy chủ, hoặc giấu trên iOS.
3. **Kho dữ liệu**: HTTPS trên hạ tầng có SLA (S3/CloudFront/R2, chỉ cần tệp tĩnh + Range), **2 gương** (`JX_IOS_KHO_DU_LIEU`
   nhận nhiều địa chỉ), đóng băng kho suốt thời gian duyệt; **luật `phienban.txt`**: không bao giờ nâng vượt bản đang
   "Waiting for Review / In Review". Script phát hành đã biến luật này thành máy.
4. **Giảm dung lượng lần tải đầu** (8,5 GB): hỏi ý + dung lượng đã có, nhưng người duyệt vẫn không đợi 8,5 GB. Chỉ chủ quyết được.
5. **Đăng ký tài khoản trong app?** Nếu có, điều 5.1.1(v) đòi **xoá tài khoản trong app** — chưa kiểm `Login.cpp`, chưa nằm trong kế hoạch nào.

---

## 7. Còn lại (cập nhật từ tệp 1509 §3)

| | Việc | Trạng thái |
|---|---|---|
| A1 | Cài mới xong app chạy được | **Cơ chế xong** (`JxKhoDuLieu`), chỉ chờ địa chỉ thật (A2). Script từ chối đóng gói khi rỗng. |
| A2 | Máy chủ HTTPS có tên miền | **Chủ thuê** |
| A3 | Tách "cũ" / "mất mạng" + dấu hoàn tất | **Xong** |
| A4 | `serverlist` còn trỏ `10.0.0.140` | chưa (dữ liệu, không phải mã); trên máy người duyệt còn kích hộp "Mạng cục bộ" |
| A5 | IPv6 | chưa (~40 dòng) |
| B2, B3 | API nội bộ, khoá thợ | **Xong**, kèm chốt máy |
| B1, B4, B5, B6 | tài khoản trả phí, mã hoá, Distribution/exportArchive, xếp hạng tuổi | như cũ |
| C3 | hỏi ý trước khi tải | **nửa đầu xong** (dung lượng + nút); "không tải qua 4G mặc định" **không phải** yêu cầu xét duyệt (phản biện rà 4.2.3, 2.4.4), để sau |
| C4 | nút tố cáo | chưa |
| D2 | ảnh nền `ios/nen_tai.jpg` | chờ chủ |
| D3 | kiểm dung lượng trống (+ khai `DiskSpace`) | chưa |
| D5, D6 | tải nhiều luồng; iPad | chưa |
| Mới | nút "Mở App Store" ở màn A (cần ID app) | chờ tài khoản |
| Mới | `NSURLSession` nền (tải tiếp khi ra nền, không cần `UIBackgroundModes`) | ý phản biện, chưa làm |
| Mới | `#stt` trong manifest ký để chặn phát lại bản cũ | chủ quyết (đổi quy trình rải bản + Android) |

---

## 8. Bẫy mới trong ngày

- **`build/ios-sim` cũ (11/09) là generator Makefiles**, cấu hình lại bằng `-G Xcode` là hỏng; đã xoá và tạo lại bằng Xcode.
- **ATS áp cả cho công cụ dòng lệnh macOS** khi thử `NSURLSession`; `NSAllowsLocalNetworking` chỉ mở 127.0.0.1/LAN.
- `base64` macOS: `-D -i tệp -o tệp`. Trong shell tương tác `grep` là hàm bọc (ugrep) → đếm bằng `/usr/bin/grep`.
- `clang++ -x objective-c++` đặt trước tệp `.o` → clang đọc `.o` như mã nguồn.
- `configure_file` (Info.plist): biến gõ sai tên → thay rỗng, **không báo**.
- `timeoutInterval` của **yêu cầu** thắng `timeoutIntervalForRequest` của phiên (đo: 3 s vs 30 s → -1001 sau 3 s).
- `fileSystemRepresentation` trả UTF-8 **tách dấu (NFD)**; đưa vào URL là sai byte so với manifest (NFC).
- `.mm` không ARC: `NSData` trong block của `NSURLSession` là `dispatch_data`, chết ngay sau handler → phải `retain`.

---

## 9. Dùng script phát hành

```
JX_IOS_KHO_DU_LIEU='https://kho1.example/jx1/ https://kho2.example/jx1/' JX_IOS_SO_BAN_DUNG=3 \
JX_IOS_KY='Apple Distribution' JX_IOS_NGHIEM=1 sh ios/dung_ban_phat_hanh.sh
```
Chỉ thử quy trình mà chưa có kho: thêm `JX_IOS_CHO_PHEP_KHO_RONG=1` (gói đó **không** gửi duyệt được). Cây thử `build/ios-dev`
vẫn ON (Finder, LAN, `may_chu_tai.txt`) — không đổi gì cho việc cài thử lên iPhone của chủ.
