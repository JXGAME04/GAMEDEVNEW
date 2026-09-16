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

iPhone "An Nguyen" đang chạy bản Release của cây thử (khoá thợ ON).

### 1b. THỬ TRÊN LAN (11:30–11:40, theo yêu cầu chủ) — trạng thái iPhone đã ĐỔI, phiên sau phải biết
- PC `10.0.0.140:8765` phục vụ 688 tệp / 7,81 GB, manifest sạch, **nhưng `manifest.sig` 404** (PC không có khoá riêng, khoá ở
  Mac `~/.jx1_khoa`). Giải pháp thử: **`ios/tram_ky.py`** chạy trên Mac (cổng 8770) — lấy manifest từ PC, ký bằng khoa Mac,
  giữ nguyên `config.ini` riêng của iPhone (đổi dòng `config.ini` trong manifest thành cỡ+md5 bản trên máy), chuyển tiếp
  mọi tệp khác sang PC (Range/404 nguyên). Bản sao `config.ini` của iPhone: `~/jx1_thu/iphone_config_1609.ini`.
- **iPhone nay có `Documents/may_chu_tai.txt` = `http://10.0.0.34:8770/`** → mỗi lần mở game sẽ đồng bộ qua trạm; trạm tắt thì
  D (vào game sau ~0–8 s). Muốn tắt: đẩy một `may_chu_tai.txt` **rỗng** lên (`devicectl device copy to`), không xoá được tệp bằng devicectl.
- Kết quả lần đầu: **66 s** cho băm 7,5 GB + tải 117 tệp (116 ảnh bản đồ tên GBK 245 MB — trên máy tên bị mã hoá hai lần nên
  bộ tải coi là thiếu — + `magicscript.txt` 0,9 MB + 1 tệp 1 byte); `da_tai.txt` 688 dòng; `manifest_dakiem.txt` khớp trạm;
  `config.ini` giữ nguyên md5. `[65973] [IOS-TAI] ket qua 0` rồi vào game. Thử D/E2 trên iPhone chưa làm được từ xa vì máy khoá
  màn hình — chủ tự thử: tắt Wi-Fi rồi mở game (D); muốn E2 thì tôi đẩy một tệp cụt lên rồi chủ mở game khi tắt Wi-Fi.
- Vấn đề lộ ra: **tên tệp GBK trên iPhone không khớp manifest** (các bản chép tay trước đây bị mã hoá hai lần); sau đồng bộ máy có
  cả hai bản (bản cũ 245 MB thành rác, D4 "dọn pak thừa" sẽ xử). Game đọc bản nào — chưa kiểm.
- Ảnh nền màn tải: chủ chọn lấy nền màn cập nhật của VNKU = `KHTD_Ui/UpdateScene.png` (1136×640) → `ios/nen_tai.png`, CMake tự đóng gói (ảnh nằm trong gói vì màn này chạy lúc chưa có dữ liệu). Các ảnh khác trong gói VNKU: `Bg_Login.jpg`, `Bg_SelPlayer.jpg`, `hinhnen1.png`, `LaunchScreenBackground.png` (logo).

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

### 2b. Đợt 2 trong ngày (chủ: "tiếp tục làm 1–4, giấu nút Thoát") — 3 phản biện trước khi sửa, kiểm rào ĐẠT 9 tệp

| Việc | Tệp | Nội dung |
|---|---|---|
| 1. Tên tệp GBK/hoa | `ios/JxTaiDuLieu.mm` | **Tên trên đĩa = dạng `JxPathPosix` tìm** (`KPosixWin32.cpp:134-157`): hạ thường ASCII + Latin-1 (U+00C0–U+00DE trừ ×, Š/Œ/Ž, Ÿ), NFC; URL vẫn tên gốc; khoá `da_tai.txt` chuẩn hoá khi đọc. Manifest PC có **142/688** tên hoa (27 `spr/ui3/FortuneRank/*` + 115 Latin-1) mà iOS (APFS phân biệt hoa/thường) không tìm thấy → trước đây bản đồ Tống Kim, hiệu ứng trang sức, màn đăng nhập… bị **ảnh trống** trên iPhone (chưa ai vào đúng chỗ). Trước khi so: **đổi tên** biến thể hoa → chuẩn (không tải lại 292 MB). Sau đồng bộ **online trọn vẹn**: xoá tệp **mồ côi** trong các thư mục cấp 1 của manifest (rác mã hoá hai lần 245 MB, pak cũ; giữ `userdata/`, `apdata/`, tệp gốc, `.part` đang tải); `ui/uitoado_macdinh*` đổi → xoá `userdata/uitoado*.ini` (như Android). Không cắt NBSP khi đọc manifest. Thử 13/13 trên ảnh đĩa APFS **phân biệt hoa/thường** (`hdiutil` — ổ Mac không phân biệt nên thử trên ổ Mac là vô nghĩa) + hồi quy 31/31. |
| 2. IPv6 | `Sources/MultiServer/Common/SocketClient.cpp` (`JX_APPLE`) | `CreateConnectionSocket`: `getaddrinfo(AF_UNSPEC, AI_DEFAULT)` → socket theo họ địa chỉ trả về → connect (tối đa 3 địa chỉ). **Không** `AI_NUMERICHOST` (tắt NAT64). Hỏng → `INVALID_SOCKET` (cùng đường UI như cũ). `nm -u` có `_getaddrinfo`. Chưa đo trên mạng NAT64 thật. |
| 2. Địa chỉ riêng | `Sources/S3Client/Login/Login.cpp` (`defined(JX_IOS) && !JX_IOS_NOI_BO`) | **Bản App Store** bỏ địa chỉ dải riêng (10/8, 172.16/12, 192.168/16, 127/8, 169.254/16) và region chỉ có địa chỉ riêng, **có bảng ánh xạ** chỉ số danh sách → số `Region_n` (phản biện: `GetServerList` dùng chỉ số danh sách làm số section, bỏ Region_0 mà không ánh xạ = mất sạch máy chủ). Đính chính tiền đề cũ: NAT64 của iOS **vẫn tổng hợp** 10.0.0.140 (Libinfo chỉ loại 0/8, 127/8, 169.254/16, 192.0.0.0/29, 192.88.99/24, 224/4) → connect treo 75 s trên luồng chính; đó là lý do lọc. Cây thử giữ nguyên để vào 10.0.0.140. Không đổi tệp `serverlist` dùng chung. |
| 2. IP game server | `Sources/S3Client/NetConnect/NetConnectAgent.cpp` (`JX_IOS`) | Máy chủ tài khoản **công cộng** trả IP game server dải riêng (GameServer_cfg.ini ghi IP LAN) → dùng IP máy chủ tài khoản (mở rộng phép thay 127.x sẵn có); không áp khi máy chủ tài khoản cũng là LAN. **Chủ kiểm:** 70.36.108.27 trả IP gì cho game server. |
| 3. Nút Thoát | `UiInit.cpp:220` (`JX_IOS`), `ShortcutKey.cpp` `LuaExit` | Giấu hẳn nút Thoát màn đầu (không `AddChild`); `Exit()` từ Lua → về màn đầu như `ExitGame()` thay vì nút chết. |
| 4a. Dung lượng trống | `ios/JxTaiDuLieu.mm`, `ios/PrivacyInfo.xcprivacy`, `ios/dung_ban_phat_hanh.sh` | `NSURLVolumeAvailableCapacityForImportantUsageKey`; cần = còn phải tải + **tệp lớn nhất** (downloadTask giữ tệp tạm riêng, pak 512 MB) + 150 MB; thiếu → E "Máy còn trống X, cần Y"; màn hỏi ý in "máy còn trống". Khai `DiskSpace` lý do **E174.1 + 85F4.1** (không gửi số lên máy chủ). Script: chốt 5 — `nm -u` có API dung lượng thì plist phải có DiskSpace. |
| 4c. Nút Tố cáo (1.2) | `UiGame.h/.cpp`, `GameSpaceChangedNotify.cpp` (`JX_IOS`) | Thực đơn lên người chơi khác thêm **"Tố cáo"** (cuối enum, TCVN3 viết `\x` để khối iOS thuần ASCII) → hộp `UIMessageBox` "Tố cáo người chơi X?" (Xác nhận/Huỷ bỏ, nút 1 = 0) → gửi **tin riêng** `[TO CAO] <tên> - hh:mm dd/mm` tới nhân vật quản trị (`config.ini [Client] TenGM`, mặc định `GM`) bằng `OnSendSomeoneMessage` (không qua bộ lọc tục), ghi `userdata/tocao.log`, báo "Đã gửi tố cáo tới quản trị viên"; chặn dòng "GM không có trên mạng" cho tin `[TO CAO]`. Relay **luôn** ghi `s3relay_log/ChatSomeOne*.log` kể cả GM offline → có dấu vết máy chủ, không phải sửa máy chủ. **Tên GM tự xác định (chủ hỏi 16/09):** "GM" là quy ước sẵn của chính mã máy chủ — `ScriptFuns.cpp:13493` `SendInfoToGM("GM", …)`, S3Relay `DoScript.cpp:128/167/226` và `RelayRpc.cpp:391/486` đặt `AccountName = "GM"`, `KPlayerBot.cpp:7949` chừa tên "GM", client `UiMsgCentrePad.cpp:1450` coi "GM" là tên đặc biệt. Vì vậy mặc định `GM` là đúng, **không cần đặt `TenGM`** (khoá đó chỉ để đổi nếu muốn). Relay ghi **mọi** tin riêng vào `s3relay_log\ChatSomeOne<dd>_<mm>_<yyyy>.log` (`ChannelMgr.cpp` SomeoneChat, sau bước gửi, không phụ thuộc người nhận có tồn tại) → tố cáo luôn được lưu ở máy chủ relay; nhân vật tên `GM` đang online thì nhận thêm tin tức thì. Chủ chỉ cần: xem log đó (hoặc tạo nhân vật `GM` để nhận trực tiếp), ghi quy trình xử lý 24 giờ vào Review Notes, thêm thông tin liên hệ trong app (1.2 đòi). |
| 4b. Dọn tệp thừa | `ios/JxTaiDuLieu.mm` | xem việc 1. |

Bằng chứng: kiểm rào ĐẠT (9 tệp `Sources/`), Release/Debug cây thử sạch, cây OFF dịch được nhánh lọc region, hồi quy 31/31 + chuẩn hoá 13/13, bản đã cài lên iPhone lúc 13:1x (máy khoá, chủ tự mở: lần mở đầu sẽ đổi tên 142 tệp và xoá 245 MB rác).

**Báo phiên Android (mới)** — bản đầy đủ, có thứ tự và cách sửa: `BANGIAO_ANDROID_TU_IOS_1609.md`. Tóm tắt: (a) LDPlayer 9 (sdcardfs) **cũng phân biệt hoa/thường Latin-1** → 115 tệp tên Latin-1 hoa cũng không tìm thấy trên Android, nên chuẩn hoá tên trong `TaiDuLieuActivity` như iOS (hoặc đổi tên trên PC); (b) `donPakCu` chỉ xoá `userdata/UiToaDo.ini`, còn `UiToaDo_<id>.ini` theo nhân vật thì không; (c) `st_mtime_ns` (đã ghi trên).

### 2c. Bản MacBook: đã dựng và ĐỐI CHIẾU ĐỒNG BỘ với iPhone (16/09, chủ yêu cầu)

Bản MacBook = **chính bản iOS** (xem `ban-macbook-mot-ban-dung`), cây `build/ios-mac` (`JX_IOS_KHO_DU_LIEU=http://10.0.0.34:8770/`).

**Dựng:** `xcodebuild -destination 'id=<UDID Mac>,arch=arm64'` — ĐẠT, chữ ký hợp lệ (`valid on disk`, `satisfies its Designated Requirement`).
Nhị phân Mac so với nhị phân iPhone: cùng `arm64`, cùng nền tảng iOS, chỉ khác **10/13909** ký hiệu (do bản Mac khai thêm iPad và
số bản dựng khác). Mọi ký hiệu và chuỗi của đợt sửa có đủ ở cả hai: `JxTaiDuLieu_DatPhienBanApp`, `JxIos_KhoDuLieu`,
`_getaddrinfo`, `manifest_dakiem`, `IOS-TENTEP`, `IOS-DON`, `tocao.log`, `[TO CAO]`, `"Tố cáo"` (TCVN3), và các chuỗi giao diện
UTF-16 ("Tải xuống", "Không kiểm tra được cập nhật", "Máy còn trống"…).

**Đồng bộ dữ liệu — chạy thật trên Mac** (chính `ios/JxTaiDuLieu.mm`, cùng kho và cùng khoá ký mà iPhone dùng): tải trọn
**7,81 GB / 688 tệp trong 262 giây**, kết quả 0, cổng `phienban.txt` qua. Đối chiếu:

| Phép kiểm | Mac | iPhone |
|---|---|---|
| Mục manifest có mặt dưới tên CHUẨN, đúng cỡ | 688/688 | 687/688 (`config.ini` riêng của máy) |
| Biến thể tên HOA còn lại | 0 | 0 |
| Tệp mồ côi trong thư mục manifest | 0 | 0 |
| `da_tai.txt` | 688 dòng, 0 khoá hoa | 688 dòng, 0 khoá hoa |
| `manifest_dakiem.txt` = chữ ký + manifest kho (nguyên byte) | khớp | khớp |
| md5 trên 5 tệp lấy ngẫu nhiên | 5/5 | — |
| Tên chỉ có ở một bên / cùng tên khác cỡ | **không có** | **không có** |

→ **Hai bản đồng bộ y hệt nhau.**

**Chỗ CHƯA tự động hoá được:** bật app iOS lên *màn hình* Mac. `open`/Finder trả "incorrect executable format"; bọc `Wrapper`
+ `WrappedBundle` như gói App Store cũng không lên; `xcrun devicectl` không nhận "My Mac" là thiết bị. Apple chỉ khởi chạy kiểu
app này qua khung CoreDevice nội bộ mà **nút Run của Xcode** gọi; điều khiển nút đó bằng kịch bản thì macOS chặn
(`osascript is not allowed assistive access`, cần cấp quyền Accessibility). Muốn xem giao diện: mở
`build/ios-mac/JX1Ios.xcodeproj`, chọn scheme `jx1ios` + đích **My Mac (Designed for iPad)**, bấm ⌘R.
**Bẫy đã trả giá:** đừng sửa nhị phân bằng `vtool` rồi ký tay — gói sẽ bị `SIGKILL (Code Signature Invalid)` lúc mở, mất công
truy ngược; dựng sạch bằng `xcodebuild` là xong.

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
2. ~~Nút Thoát vô hiệu trên iOS~~ — **chủ chốt 16/09: giấu**, đã làm (UiInit.cpp); `Exit()` từ Lua về màn đầu.
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
| A4 | `serverlist` còn trỏ `10.0.0.140` | **Xong bằng mã**: bản App Store tự bỏ địa chỉ riêng (Login.cpp); dữ liệu chung không đổi |
| A5 | IPv6 | **Xong** (SocketClient.cpp, chưa đo trên NAT64 thật) |
| B2, B3 | API nội bộ, khoá thợ | **Xong**, kèm chốt máy |
| B1, B4, B5, B6 | tài khoản trả phí, mã hoá, Distribution/exportArchive, xếp hạng tuổi | như cũ |
| C3 | hỏi ý trước khi tải | **nửa đầu xong** (dung lượng + nút); "không tải qua 4G mặc định" **không phải** yêu cầu xét duyệt (phản biện rà 4.2.3, 2.4.4), để sau |
| C4 | nút tố cáo | **Xong** (tin riêng tới GM); còn thiếu thông tin liên hệ trong app |
| D2 | ảnh nền màn tải | **Xong** — `ios/nen_tai.png` = `KHTD_Ui/UpdateScene.png` của VNKU (1136×640, chủ chọn 16/09; KHÔNG phải LaunchScreenBackground logo) |
| D3 | kiểm dung lượng trống (+ khai `DiskSpace`) | **Xong** |
| D4 | dọn tệp thừa | **Xong** (mồ côi trong thư mục manifest, chỉ sau đồng bộ online trọn vẹn) |
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
