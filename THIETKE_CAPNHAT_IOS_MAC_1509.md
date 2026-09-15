# THIẾT KẾ HỆ CẬP NHẬT DỮ LIỆU — iOS (điện thoại) và MacBook

> **Ngày:** 15/09/2026 · **Trạng thái:** THIẾT KẾ + các bản vá đã làm trong ngày.
> **Thứ tự chủ chốt:** làm xong **iPhone** trước, **Mac** làm sau — nhưng Mac **không phải bản dựng riêng** (xem §2).
> Mọi số liệu về VNKU dưới đây do tôi tự đọc từ gói đã cài trên máy, không phải nghe lại.

---

## 0. Vì sao có tài liệu này

Chủ yêu cầu ba việc, theo đúng lời:

1. *"phải ép người chơi update mới mới chơi được"*
2. *"phải viết cho phân biệt dữ liệu cũ và mới để update"*
3. *"làm cho đầy đủ và chuyên nghiệp như các game khác đang làm"*

Việc 1 và 2 **mâu thuẫn nhau nếu làm ẩu**: mã hiện tại không phân biệt được *"dữ liệu của anh đã cũ"* với
*"không nối được máy chủ"* — cả hai đều rơi vào cùng một nhánh lỗi. Ép update kiểu đó nghĩa là **ngày máy
chủ trục trặc thì toàn bộ người chơi không vào được game**, dù dữ liệu trong máy họ còn nguyên vẹn.

Tài liệu này tách hai tình huống đó ra, và lấy một game **cùng dòng đang bán trên App Store** làm đối chứng.

---

## 1. Đối chứng: VNKU (Vẹn Nguyên Ký Ức) làm thế nào

Gói đã cài: `/Applications/Vẹn Nguyên Ký Ức.app` → `Wrapper/KHTDMobile.app`, mã gói `vnku.bachgia.vn`, bản 1.0.4 (70).

| Hạng mục | VNKU | Bản GAMEDEVNEW hiện tại |
|---|---|---|
| Kích thước gói | **84 MB** | 7,6 MB |
| Dữ liệu tải về | **3,7 GB** | ~8,5 GB |
| Nơi chứa dữ liệu | **`Data/Documents/`** (`data/`, `music/`, `package.ini`, `UserData`) | `Documents/` |
| `UIFileSharingEnabled` | **không có** | **đang bật** ← phải bỏ |
| Địa chỉ kho tải | mã hoá trong `config.ini` (`downloadurl=vE0_0bYt0nL3KLL…`) | đọc từ `may_chu_tai.txt` chép tay |
| `NSAppTransportSecurity` | **`NSAllowsArbitraryLoads = true`** (tắt hết, không ngoại lệ) | chỉ `NSAllowsLocalNetworking` |
| `UIBackgroundModes` | **không có** → ra nền là đứt tải | không có → giống hệt |
| Background Assets / ODR | **không dùng**, tự viết bộ tải | tự viết |
| `StoreKit` | **có liên kết** → bán xu bằng IAP chính thức | không |
| Bản kê quyền riêng tư của app | **không có** (chỉ 3 tệp của Facebook SDK) | **có, đã đối chiếu `nm`** |
| `ITSAppUsesNonExemptEncryption` | **không khai** | không khai |
| Thiết bị | `UIDeviceFamily = 2` (**chỉ iPad**), iOS ≥ 18.0 | `[1]` (chỉ iPhone), iOS ≥ 15.0 |
| Thân nhị phân | `cryptid 1` — Apple mã hoá FairPlay, **không mổ được, không bẻ** | — |

### Màn hình cập nhật của họ (chụp lúc đang chạy)

- **Hai thanh tiến độ**: thanh xanh dương cho *tệp/tài nguyên đang tải* kèm số MB thật
  (`Đang tải tài nguyên... - 37.5% (366.8 / 978.5 MB)`), thanh xanh lá cho *tổng* (`Total: 35.0%`).
- Đồng hồ **thời gian đã trôi** (`00:00:00`), có chỗ hiện FPS.
- Nền là **tranh splash của game**, không phải nền đen.
- **Không có nút Huỷ, không có nút Bỏ qua.** Đúng tinh thần ép update.
- Đợt cập nhật 978 MB chứ không phải 3,7 GB → họ tải **phần chênh lệch**, giống cơ chế manifest của ta.

### Ba điều rút ra khiến tôi phải sửa lại chính mình

1. **ATS không phải bức tường.** Tôi từng nói bắt buộc HTTPS. VNKU tắt sạch ATS mà vẫn lên được App Store.
   *Nhưng*: khoá đó kéo thêm câu hỏi lúc duyệt, và VNKU bán ở **kho Việt Nam**, không phải kho Mỹ.
   → **Vẫn nên dùng HTTPS**, nhưng nếu kẹt thì đây là đường lui có tiền lệ.
2. **Không dời thư mục dữ liệu.** Tôi từng đề nghị chuyển sang `Library/Application Support` cho "đúng chuẩn".
   VNKU để **toàn bộ 3,7 GB trong Documents**. Cộng thêm phát hiện ở §5.2 (dời thư mục = đổi mã máy gửi lên
   máy chủ), chuyện này khép lại: **giữ nguyên Documents**.
3. **Tải không chạy nền không phải thiếu sót của ta.** VNKU cũng không có `UIBackgroundModes`.

---

## 2. Bản MacBook: MỘT bản dựng, không phải hai

Đây là phát hiện quan trọng nhất cho câu hỏi của chủ.

VNKU chạy trên Mac **không phải vì họ dựng bản Mac**. Gói của họ là:

```
Vẹn Nguyên Ký Ức.app/
  Wrapper/KHTDMobile.app/     ← đúng một bản iOS/iPadOS
    Info.plist: LSRequiresIPhoneOS = true, DTPlatformName = iphoneos
```

Đó là cơ chế **"app iPhone/iPad chạy trên Mac Apple Silicon"**. Theo tài liệu App Store Connect:

- Đặt **theo app**, áp cho **mọi bản**, ở *Pricing and Availability → iPhone and iPad Apps on Apple Silicon Mac*.
- **MẶC ĐỊNH LÀ BẬT.** Không phải xin; muốn không có mặt trên Mac thì mới phải vào **bỏ tích**.
- Điều kiện loại trừ duy nhất Apple nêu: app đã có mặt trên Mac App Store qua Universal Purchase.

### Hệ quả cho kho mã

- Thư mục **`macos/`** (bản `jx1mac`, hiện đang hỏng, chủ đã bảo bỏ qua) **KHÔNG phải** đường ra bản MacBook.
  Nó chỉ là công cụ chạy thử trên máy bàn. Đừng đổ công vào đó để có bản Mac.
- Đường ra bản MacBook là **chính bản iOS đang làm**. Làm xong điện thoại là gần như xong Mac.
- **Khuyến nghị: khai thêm iPad** — đổi `TARGETED_DEVICE_FAMILY` thành `1,2` và `UIDeviceFamily` thành `[1, 2]`.
  Lý do: (a) Apple không nói rõ app chỉ-iPhone có đủ điều kiện không, thêm iPad thì đúng trong **cả hai** trường hợp;
  (b) VNKU — bản đã chứng minh chạy được trên Mac — chính là iPad-only;
  (c) cửa sổ trên Mac sẽ theo tỉ lệ iPad, dễ nhìn hơn nhiều so với khung iPhone kéo giãn.
- **Bẫy kèm theo:** bật iPad là `UIRequiresFullScreen` bắt đầu có tác dụng thật (hiện đang vô nghĩa vì chỉ-iPhone),
  và bố cục khoá ngang ở `ios/Info.plist` cần xem lại cho màn hình đổi kích thước được.

---

## 3. Máy trạng thái lúc mở app

Đây là phần trả lời trực tiếp *"phân biệt dữ liệu cũ và mới"*.

| Tình huống | Điều kiện | Xử lý |
|---|---|---|
| **A. Bản app quá cũ** | lấy được manifest, `phienban.txt` đòi bản cao hơn | **Chặn** + nút **Mở App Store** |
| **B. Dữ liệu đã cũ** | lấy được manifest, có tệp khác | **Bắt buộc cập nhật**, hiện dung lượng, **không nút bỏ qua** |
| **C. Dữ liệu đủ và mới** | lấy được manifest, khớp hết | Vào game |
| **D. Không nối được kho, dữ liệu **đủ** | không lấy được manifest, dấu hoàn tất hợp lệ | **Vào game**, hiện thông báo nhẹ "không kiểm tra được cập nhật" |
| **E. Không nối được kho, dữ liệu **thiếu** | không lấy được manifest, không có dấu hoàn tất | Màn **Thử lại** (ngõ cụt duy nhất trung thực) |
| **F. Chữ ký manifest sai** | tải được nhưng chữ ký hỏng | Xử như **D** — chưa áp byte nào nên chưa mất gì |

**Điểm mấu chốt:** phân biệt **D** với **E**. Hiện tại mã gộp cả hai vào một nhánh lỗi, nên ép update kiểu bây giờ
sẽ chặn cả người có dữ liệu đầy đủ chỉ vì máy chủ hắt hơi. **D là thứ cứu ta khỏi bị Apple đánh rớt điều 2.1**
khi họ duyệt đúng lúc kho dữ liệu trục trặc.

**F xử như D chứ không chặn:** kẻ sửa được manifest thì cũng giả được "mất mạng", nên chặn chẳng thêm an toàn,
mà lại phạt người chơi vì lỗi ký hỏng của chính mình — chuyện rất dễ xảy ra (`_ky_lai` tự xoá chữ ký khi thiếu khoá).

---

## 4. Dấu hoàn tất — thứ hiện CHƯA CÓ

Không có nó thì không thể phân biệt D với E khi mất mạng.

**Hiện tại:** bằng chứng "máy có đủ dữ liệu" chỉ là *có tệp `config.ini`*. Mà `config.ini` là một trong những
tệp được tải **sớm nhất**. Một lần tải chết ở giây thứ hai cũng qua được phép thử này → game vào tới giữa chừng
mới chết vì thiếu tài nguyên.

**Thiết kế:** sau khi áp xong **toàn bộ** manifest, ghi một bản ghi nhỏ gồm:

- `manifest_id` = SHA-256 của đúng chuỗi byte manifest đã kiểm chữ ký
- số tệp + tổng dung lượng
- thời điểm hoàn tất, số phiên bản
- một mã xác thực (HMAC) bằng khoá ngẫu nhiên riêng máy, cất trong **Keychain** (đã có sẵn `ios/JxIosKhoaMat.mm`)

Kèm theo: **giữ lại bản `manifest.txt` + `manifest.sig` đã kiểm** (~60 KB). Nhờ đó khi mất mạng vẫn còn một
danh sách **có chữ ký** để đối chiếu — câu hỏi "dữ liệu có đủ không" thành một lượt `stat` từng tệp trong manifest,
không còn là phỏng đoán.

**Chi phí mỗi lần mở app:** băm lại bản manifest đã giữ (~60 KB) + `stat` từng tệp. **Không bao giờ băm lại 8,5 GB.**
(Số tệp trong manifest chưa đo; đo bằng `wc -l manifest.txt` trên kho dữ liệu thật khi làm.)

**Chuyển tiếp cho máy đang có dữ liệu (máy của chủ):** lần đầu chạy bản mới, nếu chưa có dấu hoàn tất nhưng
`da_tai.txt` đã có, và đối chiếu manifest thấy không thiếu tệp nào → **sinh thẳng dấu hoàn tất**, không tải lại,
không băm lại. Cách này không nới lỏng gì: bản hiện tại vốn đã tin `da_tai.txt` y như vậy ở mỗi lần mở app.

**Nói thẳng về giới hạn:** khoá HMAC nằm trong Keychain của chính app, máy đã bẻ khoá thì lấy được. Đây **không phải**
hàng rào an ninh, chỉ chống hỏng hóc vô tình và sửa vặt qua Files. Hàng rào thật vẫn là chữ ký ECDSA + kiểm phiên bản
**ở phía máy chủ lúc đăng nhập**.

---

## 5. Những điều KHÔNG được làm (tôi đã suýt làm, đều sai)

### 5.1. Đừng gác bộ tải bằng "không tìm thấy dữ liệu"

Tôi định cho bộ tải chỉ chạy khi `pszDir == NULL`. Như thế bộ tải **chỉ chạy đúng lần mở app đầu tiên trong đời máy**.
Từ lần hai trở đi `config.ini` đã có → không đồng bộ → **không bao giờ tải manifest mới**. Và vì `phienban.txt`
được đọc từ **tệp trong máy** ([`ios/JxTaiDuLieu.mm:606`](ios/JxTaiDuLieu.mm)), cổng ép update **vĩnh viễn không kích hoạt được**.
→ Phá hỏng đúng thứ chủ yêu cầu.

**Đúng:** cứ đồng bộ mỗi lần mở app khi biết địa chỉ kho (như hôm nay), và bảo vệ `config.ini` bằng cách khác (§5.3).

### 5.2. Đừng dời thư mục dữ liệu

[`KPosixWin32.cpp:1019`](Sources/Engine/Src/Platform/KPosixWin32.cpp) bịa mã máy bằng cách băm **tên máy + đường dẫn thư mục dữ liệu**:

```c
for (const char* c = s_szDataDir; *c; c++) a = a * 33 + (unsigned char)*c;
```

Mã đó được **gửi lên máy chủ** trong gói đăng nhập ([`Login.cpp:1085`](Sources/S3Client/Login/Login.cpp)), và chú thích ở
[`NetConnectAgent.cpp:250`](Sources/S3Client/NetConnect/NetConnectAgent.cpp) ghi rõ mục đích *"send HWID to server"* kèm hai
dòng đã tắt để *"mở giới hạn log vào game"* → máy chủ dùng nó để **giới hạn số tài khoản trên mỗi thiết bị**.

**Dời thư mục = mọi người chơi hiện tại hiện ra như thiết bị mới.** Không đáng đổi.

### 5.3. `config.ini` phải là "chỉ gieo lần đầu"

Bộ tải hiện ghi đè `config.ini` mỗi lần đồng bộ, vì bản trên máy khác md5 trong manifest — mà nó khác **theo định nghĩa**
sau khi người chơi đổi thiết đặt. Chủ đã ghi cảnh báo này ở [`ios/JxIosNhatKy.mm:15`](ios/JxIosNhatKy.mm).

**Đúng:** giữ một danh sách ngắn (`config.ini`, `camera_mobile.ini`) — **đã có trên máy thì bỏ qua**, bất kể md5.
**Không** loại chúng khỏi manifest phía máy chủ: làm vậy thì máy cài mới sẽ không bao giờ có `config.ini` để khởi động.

### 5.4. Đừng bỏ `UIFileSharingEnabled` theo Debug/Release

Chủ **có** dựng bản Release để chạy trên iPhone thật khi đo hiệu năng. Gác theo `CONFIGURATION` là lần đo tới
mất luôn đường chép dữ liệu bằng Finder và đường gửi nhật ký LAN, mà **không báo lỗi gì**.
**Đúng:** một cờ CMake riêng (ví dụ `JX_IOS_KHOA_NOI_BO`), mặc định bật, script phát hành truyền `OFF`.

---

## 6. Đã sửa xong trong ngày 15/09 (4 tệp, đã dựng sạch)

| Tệp | Nội dung |
|---|---|
| `android/may_chu_tai_du_lieu.py` | **Lỗi chặn phát hành**: `manifest.sig` lọt vào chính `manifest.txt` với md5 của chữ ký cũ rồi bị ghi đè → bản iOS kiểm md5 luôn hỏng → kẹt vòng thử lại vĩnh viễn. Bản Android không lộ vì **không kiểm md5**. Đã thêm `manifest.sig`, `may_chu_tai.txt`, `may_chu_nhatky.txt` vào danh sách loại. |
| `android/dong_goi_du_lieu_dien_thoai.py` | Xoá cả chữ ký cũ khi đóng gói lại, không chỉ `manifest.txt`. |
| `ios/JxTaiDuLieu.mm` | Vòng **Thử lại**; **kiểm mã trả về TRƯỚC khi ghi**; xử lý máy chủ bỏ qua `Range`; trần số lần thử + giãn cách; giữ `.part` khi chỉ đứt mạng, chỉ xoá khi byte thật sự sai; chặn manifest thiếu md5; lọc `..` trong đường dẫn; chống khoá màn hình khi tải. |
| `ios/JxIosMain.cpp` | Bốn lối chết không còn để lại **màn hình đen**. |

**Vì sao "màn hình đen" là chuyện lớn:** SDL3 đã vô hiệu `exit()`
(`SDL_uikitappdelegate.m`, dòng `// exit(exit_status);`), nên `return` từ `main()` **không kết thúc tiến trình** —
nó để lại app sống mà không cửa sổ. Người duyệt của Apple thấy đó là **treo**, nặng hơn "thoát".

---

## 7. Thứ tự làm tiếp

**Giai đoạn 1 — điện thoại (làm trước):**

1. **Ghi sẵn địa chỉ kho dữ liệu vào bản dựng.** Không có bước này thì máy cài mới từ App Store **không khởi động nổi**
   — đây là chỗ chặn nặng nhất còn lại. Nên để trong `Info.plist` (script phát hành đã sẵn thói quen tự kiểm bằng
   `PlistBuddy`), giá trị để trống cho tới khi có máy chủ.
2. **Dấu hoàn tất + giữ manifest đã kiểm** (§4).
3. **Tách D khỏi E** trong `JxTaiDuLieu_Chay` (§3) — phần trả lời *"phân biệt dữ liệu cũ và mới"*.
4. **Hỏi ý trước khi tải 8,5 GB** (điều 4.2.3(ii)) + mặc định **không** tải qua mạng di động.
5. **Kiểm dung lượng trống** trước khi ghi — ⚠️ **phải khai `DiskSpace` trong `PrivacyInfo.xcprivacy` cùng lúc**,
   không thì bị chặn ngay từ vòng gửi bản (ITMS-91053). Hiện `statvfs` **chưa** có trong nhị phân (đã đo bằng `nm -u`
   trên cả ba bản dựng), nên thêm phép kiểm là phải khai thêm.
6. **Bỏ khỏi bản phát hành**: `UIFileSharingEnabled`, `LSSupportsOpeningDocumentsInPlace`, và **API nội bộ
   `-[UIApplication suspend]`** trong `ios/JxIosAnGame.mm` — chính tệp đó đã tự ghi *"đưa lên App Store sẽ bị từ chối"*,
   và chuỗi `suspend` **có thật** trong nhị phân đã dựng.
7. **Hai thanh tiến độ + đồng hồ + nền tranh splash**, theo VNKU.

**Giai đoạn 2 — MacBook (sau khi điện thoại xong):**

8. Thêm iPad: `TARGETED_DEVICE_FAMILY = "1,2"`, `UIDeviceFamily = [1, 2]`.
9. Xem lại bố cục cho màn đổi kích thước được (Mac cho kéo cửa sổ).
10. Trong App Store Connect: để **nguyên mặc định** ở *Pricing and Availability* — đã bật sẵn.
11. **Không** đụng vào `macos/`. Nó không liên quan tới bản MacBook phát hành.

---

## 8. Chủ cần quyết

1. **Kho dữ liệu đặt ở đâu** — phải có tên miền + HTTPS trước khi làm được việc số 1. Chi phí chính là **băng thông ra**:
   mỗi lượt cài mới kéo 8,5 GB; 1.000 người là 8,5 TB.
2. **Giảm dung lượng lần tải đầu?** Người duyệt của Apple sẽ không ngồi đợi 8,5 GB. Cách chuẩn của ngành là gói một
   phần lõi chơi được rồi tải tiếp phần còn lại trong lúc chơi. VNKU cũng chỉ tải 978 MB cho một đợt.
3. **Nạp xu**: VNKU dùng **StoreKit** (IAP chính thức của Apple). Xem [[phat-hanh-ios-app-store-my]].
4. **Bản quyền (điều 5.2.1)** — vẫn là mục duy nhất có thể mất cả tài khoản Apple Developer, và bị xử lý **sau** khi đã duyệt.

---

## 9. Bẫy cho người làm tiếp

- **`ios/kiem_rao.py` chỉ so thư mục `Sources/`.** Toàn bộ việc trong tài liệu này nằm ở `ios/` và `android/`, nên nó
  sẽ báo **ĐẠT** mà **không hề nhìn** vào những gì ta sửa. Đừng lấy nó làm bằng chứng đúng/sai cho phần này.
- **Shell ở đây là zsh**: `$BIEN` không tự tách từ như bash; mẫu glob không khớp thì **hỏng cả lệnh**.
- **Nhiều tệp trong `Sources/` chứa byte GBK** → `grep` của macOS coi là nhị phân và **im lặng trả rỗng**.
  Phải `LC_ALL=C grep -a`. `grep -E` của BSD **không hiểu `\s`**.
- **Có 5 bản gần giống nhau của `ESClient/Socket.cpp`.** Bản sống cho iOS là `Sources/MultiServer/Common/Socket.cpp`.
  Muốn biết tệp nào thật sự được dựng thì tra `android/lists/*.cmake`.
- **Chuỗi `@"..."` có dấu tiếng Việt được Objective-C cất dạng UTF-16**, không phải UTF-8 — tìm trong nhị phân
  bằng UTF-8 sẽ không thấy và dễ tưởng mã không vào.
- **Bản macOS (`build/macos`) đang hỏng sẵn** từ commit `98ec561a` của phiên Android: khối `#ifdef JX_APPLE` ở
  `D3D9onGPUDev.cpp:651-669` dùng ba biến chỉ khai trong `#ifdef JX_MOBILE` (`BaseInclude.h:63-103`). Chủ đã chọn bỏ qua.
