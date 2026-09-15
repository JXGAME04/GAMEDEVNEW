# BÀN GIAO 15/09 — SỬA BỘ TẢI DỮ LIỆU (có 2 tệp dùng chung với Android)

> **Gửi phiên Android.** Đợt này tôi sửa 4 tệp + thêm 2 tài liệu.
> **Hai tệp trong `android/` là dùng chung — đọc §1 trước khi làm gì tiếp.**
> **Không đụng một tệp nào trong `Sources/`** → xem §4 về kiểm rào.

---

## 1. ⚠️ LỖI CHẶN PHÁT HÀNH đã vá — ảnh hưởng CẢ Android

### `android/may_chu_tai_du_lieu.py` — `manifest.sig` lọt vào chính `manifest.txt`

`BO_TEP` loại `manifest.txt` nhưng **không loại `manifest.sig`**. Trình tự trong `lam_manifest()`:

1. dòng 72-78: duyệt cả cây, bỏ qua tệp có tên trong `BO_TEP`
2. dòng 93: ghi `manifest.txt`
3. dòng 98: gọi `_ky_lai()` → ghi **đè** `manifest.sig`

Nên **từ lần chạy thứ hai trở đi**: vòng duyệt thấy `manifest.sig` còn lại của lần trước → ghi nó vào manifest kèm
**md5 của chữ ký CŨ** → ngay sau đó chữ ký bị ghi đè bằng chữ ký MỚI. Chữ ký ECDSA dùng số ngẫu nhiên nên
**hai lần ký không bao giờ trùng byte**.

**Hậu quả theo từng nền:**

| | Biểu hiện |
|---|---|
| **iOS** | Kiểm md5 sau khi tải → **luôn sai** → kẹt màn "Thử lại" vĩnh viễn, mọi máy, mọi lần mở. Bản iOS **chưa từng đồng bộ nổi một lần nào**. |
| **Android** | **Không lộ**, vì `taiMot()` chỉ kiểm **kích thước**, không kiểm md5. Đổi lại `ghiDaTai()` ghi **md5 của manifest** chứ không phải md5 của tệp thật → `da_tai.txt` trên máy người chơi **đang chứa md5 sai** cho `manifest.sig`, mà vẫn được coi như đã kiểm. |

**Đã vá:** thêm `manifest.sig` vào `BO_TEP`. Thêm luôn `may_chu_tai.txt` và `may_chu_nhatky.txt` — hai tệp đó chứa
**địa chỉ máy chủ**; nếu lọt vào manifest thì bộ tải sẽ ghi chúng xuống máy **mọi người chơi**, tự bật bộ gửi nhật ký
trỏ về địa chỉ ghi trong tệp, mà đường gửi nhật ký **không hề kiểm chữ ký**.

**Việc phía bạn:** chạy lại `may_chu_tai_du_lieu.py --chi-manifest` để sinh manifest sạch. Dòng `manifest.sig` cũ
trong `da_tai.txt` của máy đang chơi sẽ tự hết tác dụng vì tệp đó không còn trong manifest nữa.

### `android/dong_goi_du_lieu_dien_thoai.py` — xoá cả chữ ký cũ

Dòng 391 trước đây chỉ xoá `("manifest.txt",)`. Chữ ký cũ nằm lại sau khi đóng gói lại thì vừa vô nghĩa vừa dễ làm
người ta tưởng kho đã được ký. Nay xoá cả `manifest.sig`.

---

## 2. Hai tệp `ios/` — Android KHÔNG bị ảnh hưởng

| Tệp | Nội dung |
|---|---|
| `ios/JxTaiDuLieu.mm` | Vòng **Thử lại** trên màn tải; **kiểm mã HTTP trước khi ghi** (trước đây nối thẳng thân phản hồi vào `.part`, kể cả trang lỗi 404/500); xử lý máy chủ bỏ qua `Range` (trả 200 khi đang xin khúc); **trần số lần thử + giãn cách**; giữ `.part` khi chỉ đứt mạng, chỉ xoá khi byte thật sự sai; chặn manifest thiếu md5; lọc `..` trong đường dẫn; chống khoá màn hình khi đang tải. |
| `ios/JxIosMain.cpp` | Bốn lối chết không còn để lại **màn hình đen**. |

**Vì sao "màn hình đen" là chuyện lớn:** SDL3 đã vô hiệu `exit()`
(`SDL_uikitappdelegate.m`, dòng `// exit(exit_status);`), nên `return` từ `main()` **không kết thúc tiến trình** —
nó để lại app sống mà không có cửa sổ. Người duyệt của Apple ghi nhận đó là **treo**.

Ba lỗi trong `ios/JxTaiDuLieu.mm` mà **bản Android đã làm đúng từ lâu**, iOS bỏ sót — nếu Android có sửa vùng này
thì giữ nguyên cách hiện tại, đừng "đồng bộ" theo iOS cũ:

- Android kiểm `getResponseCode()` **trước** khi đọc thân (`taiMot:476-478`)
- Android xử lý 200-khi-đang-xin-khúc (`:477`)
- Android lọc `..` và `/` đầu trong đường dẫn manifest (`:340-341`)

---

## 3. Tài liệu kèm theo

- **`THIETKE_CAPNHAT_IOS_MAC_1509.md`** — thiết kế hệ cập nhật cho iOS + MacBook, kèm phần mổ nhị phân
  VNKU (`vnku.bachgia.vn`) làm đối chứng. Có một kết luận đáng để bên Android biết: **bản MacBook không phải
  bản dựng riêng** — cùng một nhị phân iOS chạy trên Mac Apple Silicon, bật sẵn theo mặc định.
- Trong đó §5 liệt kê **những việc KHÔNG được làm**, đều là thứ tôi đã suýt làm rồi phải rút lại.

---

## 4. ⚠️ Kiểm rào ĐẠT nhưng KHÔNG chứng minh gì cho đợt này

`ios/kiem_rao.py` **chỉ so thư mục `Sources/`**. Đợt này tôi **không sửa một tệp nào trong `Sources/`** —
toàn bộ nằm ở `ios/` và `android/`. Nên nó sẽ báo:

```
KIEM RAO (ANDROID): DAT - ban ANDROID khong doi
KIEM RAO (WINDOWS): DAT - ban WINDOWS khong doi
```

**Kết quả đó đúng nhưng rỗng nghĩa**: nó chứng minh bản Android/Windows không bị đổi *qua đường `Sources/`*,
chứ **không** nói gì về hai tệp `android/*.py` tôi vừa sửa. Phần đó phải đọc §1 mà xét.

---

## 5. Một lỗi của bên Android, chủ đã chọn BỎ QUA (ghi lại kẻo thất lạc)

Bản macOS (`build/macos`) hỏng từ commit `98ec561a`. Năm lỗi "use of undeclared identifier", tất cả trong
`Sources/Represent/Represent3/D3D9onGPUDev.cpp` dòng 653, 657, 662, 665.

**Nguyên nhân:** ba biến `g_nJxPalBuffer`, `g_nJxPsBuffer`, `g_nJxAtlasKhoi` chỉ được khai trong rào
`#ifdef JX_MOBILE` (`BaseInclude.h`, mở dòng 63, đóng dòng 103), nhưng chỗ dùng lại nằm trong rào
`#ifdef JX_APPLE` (`D3D9onGPUDev.cpp`, mở 651, đóng 669).
`ios/CMakeLists.txt:105` có **cả hai** định nghĩa nên iOS dựng được; `macos/CMakeLists.txt:57` có `JX_APPLE`
mà **thiếu** `JX_MOBILE` nên hỏng.

Chủ đã quyết **không dùng bản macOS**, nên không ai phải sửa. Ghi ở đây để sau này khỏi mất công tìm lại.

---

## 6. Bẫy công cụ (tôi vấp đủ 4 lần trong một buổi)

- **Shell là zsh**: `$BIEN` **không tự tách từ** như bash; mẫu glob không khớp thì **hỏng cả lệnh**.
- **Nhiều tệp `Sources/` chứa byte GBK** → `grep` của macOS coi là nhị phân và **im lặng trả rỗng**. Phải `grep -a`.
- **`grep -E` của BSD không hiểu `\s`** — nó coi đó là chữ `s`.
- **Chuỗi `@"..."` có dấu tiếng Việt** được Objective-C cất dạng **UTF-16**, không phải UTF-8 — tìm trong nhị phân
  bằng UTF-8 sẽ không thấy và dễ tưởng mã chưa vào.
