# BÀN GIAO CHO PHIÊN SAU — PHÁT HÀNH iOS (chốt 15/09/2026)

> **16/09:** đã có bản mới hơn — đọc `BANGIAO_PHIENSAU_IOS_1609.md` trước (A3, B2, B3 xong; §5.3 và §4 của tệp thiết kế bị bác).

> Đọc tệp này trước. Hai tệp kia đọc khi cần chi tiết:
> - **`THIETKE_CAPNHAT_IOS_MAC_1509.md`** — thiết kế hệ cập nhật + phần mổ nhị phân VNKU đối chứng
> - **`BANGIAO_TAIDULIEU_1509.md`** — gửi riêng phiên Android (2 tệp `android/` dùng chung)

---

## 1. Tình trạng git

Nhánh `mobile-0809`, sạch, lệch **0/0** với `origin`. Ba commit của ngày 15/09:

| Commit | Nội dung |
|---|---|
| `c1855780` | Màn cập nhật theo VNKU: hai thanh tiến độ + đồng hồ + nền ảnh |
| `5a461978` | Vá lỗi chặn phát hành `manifest.sig` + bộ tải chắc chắn hơn + bỏ màn hình đen |
| `054bb660` | *(của phiên Android)* — đã gộp tiến thẳng, không đụng độ |

---

## 2. Đã làm xong

**Lỗi chặn phát hành nặng nhất tìm được trong ngày:** `manifest.sig` lọt vào chính `manifest.txt` với md5
của chữ ký **cũ** rồi bị ghi đè bằng chữ ký **mới** (ECDSA dùng số ngẫu nhiên → không bao giờ trùng byte).
Bản iOS kiểm md5 nên **luôn hỏng** → **chưa từng đồng bộ nổi một lần nào**. Bản Android không lộ vì không
kiểm md5, đổi lại `da_tai.txt` bên đó bị ghi md5 sai mà vẫn coi như đã kiểm. → Đã vá.

Ngoài ra: kiểm mã HTTP **trước** khi ghi (trước đây nối thẳng thân phản hồi 404/500 vào `.part`);
xử lý máy chủ bỏ qua `Range`; trần số lần thử + giãn cách (Wi-Fi công cộng từng làm vòng lặp quay hàng
trăm nghìn lần); giữ `.part` khi chỉ đứt mạng, chỉ xoá khi byte thật sự sai; chặn manifest thiếu md5;
lọc `..`; chống khoá màn hình; vòng Thử lại; bốn lối chết không còn để lại **màn hình đen**;
và màn cập nhật hai thanh theo VNKU.

---

## 3. Còn thiếu

### A. Làm app không chạy được

| | Việc | Ghi chú |
|---|---|---|
| A1 | **Cài mới xong app không khởi động nổi** | Chặn nặng nhất. Cần A2 trước |
| A2 | Máy chủ HTTPS có tên miền | **Chủ thuê** |
| A3 | Phân biệt "dữ liệu cũ" / "không nối được máy chủ" + dấu hoàn tất | Thiết kế xong ở §3-§4 tệp thiết kế |
| A4 | Danh sách máy chủ còn trỏ `10.0.0.140` | |
| A5 | IPv6 (`getaddrinfo` + `AF_UNSPEC` + `AI_DEFAULT`) | ~40 dòng |

### B. Chặn từ vòng gửi bản

| | Việc | Ghi chú |
|---|---|---|
| B1 | **Tài khoản Apple Developer trả phí** | **Chưa có** — `dung_ban_phat_hanh.sh` ghi rõ |
| B2 | API nội bộ `-[UIApplication suspend]` | `ios/JxIosAnGame.mm` tự ghi "sẽ bị từ chối"; chuỗi có thật trong nhị phân |
| B3 | Bỏ `UIFileSharingEnabled` + `LSSupportsOpeningDocumentsInPlace` | Cũng là chỗ bịt lỗ `da_tai.txt` |
| B4 | `ITSAppUsesNonExemptEncryption` | **Chủ quyết** — khai báo pháp lý |
| B5 | Chứng thư Distribution + `-exportArchive` | Chưa viết |
| B6 | Bảng xếp hạng tuổi | Hạn 31/01/2026 đã qua |

### C. Rủi ro khi duyệt

C1 **bản quyền JX1 (5.2.1)** — mục duy nhất mất được cả tài khoản · C2 tải 8,5 GB, người duyệt không đợi ·
C3 hỏi ý trước khi tải + mặc định không dùng 4G · C4 **thiếu nút tố cáo** (1.2) · C5 tài khoản thử + máy chủ
phải chạy · C6 ảnh chụp/mô tả/URL · C7 Bầu Cua phải khai hạng tuổi.

### D. Chuyên nghiệp

D2 **ảnh nền splash** (chờ chủ) · D3 kiểm dung lượng trống *(⚠️ phải khai `DiskSpace` cùng lúc, không thì
ITMS-91053)* · D4 dọn pak thừa (Android có, iOS không) · D5 tải nhiều luồng (Android 4, iOS 1) ·
D6 thêm iPad để có bản MacBook.

---

## 4. Ba việc kế tiếp, theo thứ tự phụ thuộc

1. **Chủ thuê máy chủ HTTPS** (A2) → mở khoá A1.
2. **A3** — dấu hoàn tất + tách "cũ" khỏi "mất mạng". Làm được ngay, không chờ máy chủ.
3. **B2 + B3** — bỏ API nội bộ và hai khoá thợ khỏi bản phát hành. Làm được ngay.

---

## 5. Quyết định chủ đã chốt — ĐỪNG HỎI LẠI

| Việc | Chốt |
|---|---|
| Ép cập nhật | **Có** — không nút bỏ qua, không đường vòng vào game |
| Nút "Chơi tiếp" khi tải hỏng | **Không làm** (chủ chặn lại khi tôi định thêm) |
| Xu | **Chỉ kiếm trong game**, không bán → Bầu Cua là cờ bạc giả lập, được giữ |
| Bản macOS (`macos/`, `jx1mac`) | **Bỏ qua**, không dựng, không sửa |
| Máy ảo `jx1m-test` | **Không đụng** — của dự án USVOLAM |
| git | Tiến thẳng, không ép đẩy, **không `git stash`**; commit/đẩy phải xin phép |
| Cách làm | **Phản biện đối kháng trước mỗi đợt sửa** |

---

## 6. Những chỗ TÔI đã sai trong ngày — đừng lặp lại

1. **"Phải có HTTPS, ATS là bức tường"** → sai. VNKU khai `NSAllowsArbitraryLoads = true` (tắt sạch,
   không ngoại lệ) mà vẫn lên App Store. Vẫn nên dùng HTTPS, nhưng đó không phải tường chắn tuyệt đối.
2. **"Dời thư mục dữ liệu sang Application Support cho đúng chuẩn"** → **sai và nguy hiểm**.
   `KPosixWin32.cpp:1019` băm **đường dẫn thư mục dữ liệu** vào mã máy, mà mã đó **gửi lên máy chủ**
   (`Login.cpp:1085`) và máy chủ dùng để giới hạn tài khoản trên mỗi thiết bị. Dời = mọi người chơi
   hiện ra như thiết bị mới. VNKU cũng để cả 3,7 GB trong `Documents`. **Giữ nguyên Documents.**
3. **"Gác bộ tải bằng `pszDir == NULL`"** → sai chí mạng. Làm thế bộ tải **chỉ chạy lần mở app đầu tiên
   trong đời máy**; `phienban.txt` đọc từ tệp trong máy (`JxTaiDuLieu.mm:606`) nên cổng ép update
   **vĩnh viễn không kích hoạt được** — phá đúng thứ chủ yêu cầu.
4. **"Xoá `.part` ở mọi nhánh lỗi"** → sai. Đứt mạng chỉ làm **cụt** tệp chứ không làm **hỏng**.
   Nguyên tắc: byte trên đĩa **sai** thì xoá, chỉ **thiếu** thì giữ.
5. **"`phienban.txt` nằm trong manifest đã ký nên không ai hạ ngưỡng được"** → sai. Chữ ký **không có**
   dấu thời gian/số thứ tự/hạn dùng → một cặp (manifest, chữ ký) cũ bất kỳ **hợp lệ vĩnh viễn**.
6. **"`statvfs` đã nằm trong nhị phân"** *(một agent khẳng định)* → sai. Đo bằng `nm -u` trên **ba** bản
   dựng: không có. Chỗ gọi duy nhất nằm trong `JxReplay.cpp`, mà tệp đó bị loại khỏi bản dựng.
   **Nhưng** ngày thêm phép kiểm dung lượng thì **bắt buộc** khai `DiskSpace` cùng lúc.

---

## 7. Chờ chủ cung cấp

- **Ảnh nền màn tải** → đặt `ios/nen_tai.jpg` (hoặc `.png`), CMake tự đóng gói. Chưa có thì nền tối trơn.
- **Tên miền + máy chủ HTTPS**.
- **Quyết định** B4 (khai báo mã hoá), C1 (bản quyền), C2 (giảm dung lượng lần tải đầu).
- Hai câu hỏi tôi hỏi mà chưa có trả lời: khi **tắt Wi-Fi giữa chừng** và khi **bấm Home ra nền**,
  VNKU xử lý thế nào.

---

## 8. Bẫy công cụ — tôi vấp đủ bốn lần trong một buổi

- **Shell là zsh**: `$BIEN` **không tự tách từ** như bash; glob không khớp thì **hỏng cả lệnh**.
- **Nhiều tệp `Sources/` chứa byte GBK** → `grep` macOS coi là nhị phân, **im lặng trả rỗng**. Phải `grep -a`.
- **`grep -E` của BSD không hiểu `\s`**.
- **Chuỗi `@"..."` có dấu** được ObjC cất dạng **UTF-16**, không phải UTF-8.
- **Tên tệp macOS lưu dạng Unicode tách dấu** — gõ tay tên tiếng Việt dễ không khớp; để lệnh tự tìm đường dẫn.

---

## 9. Cách kiểm chứng — ĐỪNG tin kiểm rào cho phần này

`ios/kiem_rao.py` **chỉ so thư mục `Sources/`**. Toàn bộ việc iOS nằm ở `ios/` và `android/`, nên nó báo
**ĐẠT** mà **không hề nhìn** vào những gì ta sửa. Kiểm thật bằng:

```
cmake --build build/ios-dev --config Debug     # và --config Release
nm -u build/ios-dev/Debug-iphoneos/jx1ios.app/jx1ios | grep <ky hieu moi>
```

Và kiểm chuỗi giao diện bằng UTF-16, **luôn kèm một chuỗi CŨ để tự kiểm chứng phép đo** — nếu chuỗi cũ
cũng không thấy thì phép đo sai, không phải mã thiếu.
