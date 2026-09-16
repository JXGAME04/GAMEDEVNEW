# Kế hoạch chuẩn bị phát hành game (Android / iOS / máy chủ) — 16/09/2026

> Trả lời câu hỏi của chủ: "định hướng chuẩn bị phát hành game cần những gì". Tài liệu này là **phân tích + kế hoạch**, chưa đụng mã.
> Mọi mục "hiện trạng" đều đọc từ kho hôm nay (nhánh `mobile-0809` 060240dc, gói dt_v4 109161354) và các hồ sơ
> `BAOMAT_TRUOC_PHAT_HANH_1409.md`, `BANGIAO_PHIENSAU_IOS_1609.md`, `BANGIAO_GIOIHAN_MAMAY_THICONG_1609.md`.

---

## 0. Bốn quyết định của chủ (mọi việc bên dưới phụ thuộc)

| # | Câu hỏi | Vì sao phải chốt trước | Tôi đề nghị |
|---|---|---|---|
| Q1 | **Kênh phát hành**: (a) Google Play + App Store, hay (b) APK tự phát (web/QR) + iOS qua TestFlight/doanh nghiệp, hay (c) cả hai theo giai đoạn | Bản Android hiện **tự tải và tự cài APK mới** (`REQUEST_INSTALL_PACKAGES`, `TaiDuLieuActivity.kiemApk`) — Google Play **cấm** app tự cập nhật ngoài Play. Lên Play thì phải tắt cơ chế này, cập nhật qua Play; phát ngoài thì giữ. Hai kênh = hai cấu hình gói. | (c): mở thử ngoài chợ trước (APK + TestFlight) để chạy thật vài tuần, sửa lỗi, rồi mới nộp chợ. |
| Q2 | **Pháp lý**: giấy phép phát hành trò chơi điện tử trên mạng (G1) và bản quyền dữ liệu (đồ hoạ Kingsoft/VNG, ảnh VNKU) | Chợ ứng dụng ở Việt Nam đang yêu cầu chứng minh giấy phép với game online; bên giữ bản quyền có thể yêu cầu gỡ app ngay cả khi đã lên. Đây là rủi ro lớn nhất, kỹ thuật không giải quyết được. | Chủ hỏi luật sư / đơn vị phát hành trước khi bỏ công làm store listing. |
| Q3 | **Mô hình thu tiền**: nạp thẻ qua web / GM hay bán trong app | Trên Play/App Store, bán tiền ảo/vật phẩm dùng trong app **phải** qua Google Play Billing / Apple IAP (phí 15–30 %); đưa link nạp thẻ ngoài vào app là vi phạm. Phát ngoài chợ thì tự do. | Chốt cùng Q1. Client hiện không có màn nạp tiền nào (đã rà `Ui/UiCase`), tức đang nạp ngoài — hợp với (b). |
| Q4 | **Hạ tầng**: máy chủ game + kho dữ liệu HTTPS/CDN | Tất cả đang ở PC nhà (`10.0.0.140`: GameServer, S3Relay, Bishop, máy chủ tải 8765). Kho 7,8 GB cần tên miền + HTTPS + hỗ trợ `Range` + 2 gương (iOS đã đòi từ 15/09, `JX_IOS_KHO_DU_LIEU`). | Thuê VPS cho máy chủ game + object storage/CDN cho kho (S3/R2/CloudFront hoặc tương đương trong nước). |

---

## 1. Việc CHẶN phát hành (chưa xong thì không được mở)

### 1A. Bảo mật máy chủ — 5 lỗ ngày 14/09 **chưa vá dòng nào** (đã kiểm lại mã hôm nay)
| Lỗ | Chỗ | Hậu quả | Trạng thái |
|---|---|---|---|
| A1 lệnh GM qua chat chạy Lua tuỳ ý + tràn bộ đệm | `GameDataDef.h:27` `_CHAT_SCRIPT_OPEN` bật vô điều kiện; `KPlayerChat.cpp` không kiểm quyền | người chơi bất kỳ chạy script trên máy chủ | **chưa** |
| A2 chèn SQL kiểm tài khoản | `S3PAccount.cpp:36/234/512` | đăng nhập tài khoản người khác, phá DB | **chưa** |
| A3 mua sạp không kiểm chỉ số | `KProtocolProcess.cpp:7576` | nhân đôi vật phẩm | **chưa** |
| A4 đặt giá âm | `KItemList.cpp:6077` | lấy đồ gần miễn phí | **chưa** |
| A5 gửi tiền tràn dấu | `KProtocolProcess.cpp:6800` | tạo tiền | **chưa** |
Kèm nhóm C của hồ sơ 14/09 (Bầu Cua không kiểm số dư, băm mật khẩu = mật khẩu, kênh game không mã hoá `rand()`) cần rà tiếp.
**Ai làm:** phiên máy chủ (PC). Ước lượng: A1–A5 2–3 ngày làm + 1 ngày test; nhóm C 3–5 ngày.

### 1B. Giới hạn đăng nhập theo mã máy trên mobile
Máy chủ test đang cưỡng chế `MaxLogin=2` theo mã máy; mã máy bản mobile = hostname + thư mục dữ liệu (gần như **giống nhau giữa các điện thoại**, hồ sơ PC ghi rõ "giới hạn của mã mobile"). Ra thật: 2 người chơi mobile đầu tiên vào được, người thứ 3 bị đá.
**Việc:** mã thiết bị riêng cho mobile (Android: `ANDROID_ID` theo app + ký bằng khoá app; iOS: UUID trong Keychain; sau này Key Attestation / App Attest như `PHANTICH_GIOIHAN_DANGNHAP_MOBILE_1409.md` đề xuất), hoặc máy chủ phân biệt nền và không đếm mobile. **Ai:** tôi (client mobile) + phiên PC (máy chủ). 1–2 ngày.

### 1C. Gói Android
| Việc | Hiện trạng | Cần |
|---|---|---|
| Ký gói phát hành | mọi bản dt_v4 là `app-debug.apk`: `debuggable`, ký khoá Debug công khai; `release` trong `build.gradle` **không có signingConfig** | tạo keystore phát hành (giữ ở nơi an toàn, sao lưu — mất là không cập nhật được app trên Play), ký release, tắt debuggable |
| API mục tiêu | `targetSdkVersion 34` | Google Play từ 31/08/2026 đòi target **API 36** cho app mới và bản cập nhật → nâng compileSdk/targetSdk 36, rà thay đổi hành vi (quyền, edge-to-edge, foreground) |
| Trang bộ nhớ 16 KB | **9/9 tệp .so căn 4 KB** (đo hôm nay); NDK 25 | Play bắt buộc hỗ trợ 16 KB khi target ≥ 35; máy Android 15/16 dùng trang 16 KB không nạp được → thêm `-Wl,-z,max-page-size=16384` cho mọi thư viện + nâng NDK ≥ r27 (để `libc++_shared` cũng 16 KB) + dựng lại SDL3; kiểm bằng script đo alignment |
| Tự cập nhật APK | `REQUEST_INSTALL_PACKAGES` + tải APK từ máy chủ tải | Play: bỏ hoàn toàn trong bản chợ (rào theo cờ phát hành). Ngoài chợ: giữ, nhưng APK phải qua HTTPS và kiểm chữ ký gói |
| Kiến trúc | arm64-v8a + x86_64 | giữ x86_64 cho máy ảo/ChromeOS được; Play chấp nhận |
| minify | tắt | có thể giữ tắt (mã Java ít) |
**Ai:** tôi. 2–3 ngày (16 KB + NDK là phần tốn nhất vì phải dựng lại toàn bộ và đo lại nhịp trên Fold 7).

### 1D. Kho dữ liệu và cập nhật
| Việc | Hiện trạng | Cần |
|---|---|---|
| HTTPS + tên miền + CDN | HTTP thường trên PC nhà, `usesCleartextTraffic=true` | thuê theo Q4; tắt cleartext trong bản chợ |
| Chữ ký manifest | iOS kiểm ECDSA; **Android chỉ kiểm md5 từ chính phản hồi** (bị chèn giữa đường là đổi được cả manifest lẫn tệp) | Android kiểm `manifest.sig` bằng khoá công khai nhúng trong app (như iOS `JxTaiDuLieu.mm`); khoá riêng hiện chỉ có trên Mac → chép sang PC (`%USERPROFILE%\.jx1_khoa\`) |
| Dung lượng lần đầu | 7,8 GB (688 tệp) | mục tiêu 3–4 GB: chạy `rut_gon_du_lieu.py` (bỏ ~1,2 GB đã biết), ghi nhật ký tệp mở trong vài phiên chơi thật để tách "cần ngay" / "tải sau theo map"; người duyệt Apple/Google **không đợi 8 GB** |
| Quy trình rải bản | `phienban.txt` + `apk.txt` + manifest, sinh bằng `--chi-manifest` | viết thành script một lệnh (đã có bên iOS `dung_ban_phat_hanh.sh`), luật "không nâng `phienban.txt` khi bản đang chờ duyệt" |
**Ai:** tôi (mã), chủ (thuê hạ tầng, chép khoá). 2 ngày mã + thời gian thuê.

### 1E. iOS (từ hồ sơ iOS, còn mở)
Tài khoản Apple Developer trả phí (B1); Distribution profile / exportArchive (B5); khai mã hoá xuất khẩu (B4); xếp hạng tuổi (B6); địa chỉ kho thật (A2); IPv6 chưa đo trên NAT64 thật; tải nhiều luồng (D5); iPad (D6). **Ai:** phiên iOS + chủ (tài khoản, tiền).

### 1F. Lỗi đã biết còn mở
- 44 ảnh `LoadImage FAIL` trên Fold 7 (critter/passerby/enemy, nút kênh chat Tống Kim, `MiniMap.spr`, icon 通用) — **cây dữ liệu PC cũng thiếu** → phiên PC bổ sung dữ liệu gốc.
- Lag Tống Kim đông người (memory `mobile-tongkim-lag-goc`): gói Goto mỗi vòng lặp; GPU 82–99 %.
- Nút Tố cáo (bắt buộc UGC cả hai chợ) hiện chỉ iOS; Android đổi rào `JX_IOS` → `JX_MOBILE` (30 phút) — chờ chủ chốt.
- Nền đen ô sàn: **đã sửa tận gốc 16/09** ([NENGHEP]), chủ xác nhận.

---

## 2. Bắt buộc theo quy trình chợ (không phải mã, nhưng thiếu là bị từ chối)

| Mục | Google Play | App Store |
|---|---|---|
| Chính sách bảo mật (URL công khai) | bắt buộc | bắt buộc |
| Khai an toàn dữ liệu / App Privacy | Data safety form (thu gì: tài khoản, thiết bị, log) | App Privacy nutrition label |
| Xếp hạng nội dung | IARC — lưu ý **Bầu Cua** (`UiGamble.cpp`) = mô phỏng cờ bạc → xếp hạng cao hơn, có nước cấm | Age rating tương tự |
| Báo cáo nội dung người dùng (chat) | nút Tố cáo + cách chặn người chơi | đã có iOS (C4), còn thiếu thông tin liên hệ trong app |
| Xoá tài khoản trong app | không bắt buộc vì **app không tạo tài khoản** (đã rà `Login.cpp`/`UiLogin.cpp`), nhưng nên có trang web xoá | như bên |
| Thanh toán | theo Q3 | theo Q3 |
| Danh mục, mô tả, ảnh chụp (điện thoại + máy tính bảng), icon, video | cần bộ ảnh từ bản thật (không dùng ảnh VNKU nếu vướng bản quyền) | + ảnh iPad nếu hỗ trợ |
| Tài khoản demo cho người duyệt | không | **bắt buộc** (App Review đòi tài khoản chơi được ngay) |

---

## 3. Chất lượng và vận hành trước ngày mở

1. **Kiểm thử thiết bị**: Android 9/10 (sdcardfs — vừa sửa tên tệp), 11+, máy trang 16 KB (Pixel 8+), máy yếu 4 GB RAM (kho 7,8 GB + RAM 400 MB), máy tính bảng; iOS theo hồ sơ iOS.
2. **Mặc định bản phát hành**: tắt mọi chẩn đoán (`Rep3NenKiem=0` ✓, `Rep3ONenLog=0` ✓, `PaintLog=0`, `[DoNhip] Bat=0` ✓, `Rep3Log` cân nhắc tắt), HUD FPS ẩn, nhịp mặc định theo nhiệt (đã có nấc 60/120).
3. **Báo sập**: Android bị loại `CrashLog`; chưa có Crashlytics/ACRA → ít nhất bắt `SIGSEGV` ghi `jx_crash.log` + gửi về máy chủ ở lần mở sau (cơ chế gửi log đã có trong `JxDoNhip.java`).
4. **Máy chủ**: cấu hình `MaxLogin` thật, sao lưu DB tự động, giám sát (CPU/RAM/kết nối/lỗi), giới hạn kết nối chống dồn (hồ sơ 14/09: "máy chủ không giới hạn kết nối"), quy trình khởi động lại (GameServer không tự nối lại S3Relay — phải khởi động đúng thứ tự), GM tool.
5. **Vận hành**: kênh hỗ trợ người chơi, FAQ (tải dữ liệu, 4G, thiết bị), chính sách hoàn tiền/khoá nick, kế hoạch mở dần (100 → 1 000 người) và rollback (giữ APK/manifest bản trước; luật `phienban.txt`).

---

## 4. Thứ tự đề nghị và người làm

| Tuần | Việc | Ai |
|---|---|---|
| 0 (ngay) | Chốt Q1–Q4; thuê tên miền + HTTPS/CDN + VPS; chép khoá ký manifest sang PC; đăng ký tài khoản Play/Apple nếu đi Q1(a) | chủ |
| 1 | 1A bảo mật A1–A5 + rà nhóm C; 1B mã thiết bị mobile (hai bên) | phiên PC + tôi |
| 1 | 1C: keystore + ký release + tắt debuggable + cờ phát hành (tắt tự cập nhật/cleartext trong bản chợ); 1D: Android kiểm chữ ký manifest | tôi |
| 2 | 1C: targetSdk 36 + 16 KB + NDK mới + dựng lại SDL3, đo lại nhịp Fold 7; rút gọn dữ liệu lần đầu; script rải bản một lệnh | tôi |
| 2 | 1E iOS: tài khoản, profile, kho thật, IPv6 NAT64 | phiên iOS + chủ |
| 3 | Mở thử ngoài chợ (APK + TestFlight) với nhóm nhỏ; sửa lỗi; báo sập; dữ liệu PC thiếu (1F) | tất cả |
| 4 | Store listing (mục 2), xếp hạng, tài khoản demo, nộp duyệt; đóng băng kho khi chờ duyệt | chủ + tôi/iOS |

Việc tôi có thể bắt đầu ngay không cần chờ quyết định: keystore + ký release + tắt debuggable (giữ song song bản debug cho dt_v4), 16 KB + targetSdk 36, Android kiểm chữ ký manifest, cờ phát hành để tắt tự cập nhật/cleartext, nút Tố cáo cho Android, báo sập tối thiểu.
