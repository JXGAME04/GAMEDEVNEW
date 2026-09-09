# LỘ TRÌNH: TỪ "CHẠY ĐƯỢC" ĐẾN "CẦM ĐIỆN THOẠI LÊN CHƠI ĐƯỢC" (09/09)

> Nhánh `mobile-0809` (worktree `D:\GAMEDEVNEW_wt_mobile`). Đọc `BANGIAO_ANDROID_PHA4_0809.md` trước.
> Đây là **danh sách việc chưa làm, xếp theo thứ tự nên làm**. Việc nào chặn đường thì lên trước.

## Đang ở đâu (09/09, đã đo thật)

| | Trạng thái |
|---|---|
| Chạy hết đường: khởi động → đăng nhập → vào bản đồ → đánh nhau | ✅ LDPlayer x86_64 |
| **Bản arm64-v8a** | ✅ chạy được (LDPlayer dịch ARM: nạp `lib/arm64/libmain.so`, vào thẳng game) — **nhưng chưa chạy trên ARM thật + GPU thật** |
| Độ phân giải / hệ số giao diện | ✅ 1040×604 và 2080×1080 |
| Nhớ mật mã + tự đăng nhập | ✅ mở app là vào thẳng bản đồ |
| Vẽ Vulkan, chữ, mạng, âm thanh (hiệu ứng) | ✅ |

**Kết luận:** máy đã *chạy*, nhưng **chưa ai chơi được bằng ngón tay** và **chưa có cách đưa 10 GB dữ liệu vào điện thoại**. Đó là hai việc số 1 và số 2.

---

## 1. ĐIỀU KHIỂN BẰNG NGÓN TAY — chặn đường, làm trước hết

Hiện game chỉ nhận chạm qua kiểu "giả lập chuột" của SDL: **một ngón = chuột trái, hết**. Đây là việc lớn nhất còn lại.

| # | Việc | Vì sao chặn | Ghi chú kỹ thuật |
|---|---|---|---|
| 1.1 | **Bàn phím ảo bật suốt** → che nửa màn hình | `KSdlApp::Init` gọi `SDL_StartTextInput` một lần rồi giữ đến lúc thoát. Trên LDPlayer có bàn phím cứng nên không lộ; **trên điện thoại thật bàn phím sẽ bật ngay từ màn hình chính và không tắt** | Chỉ bật khi ô nhập (`KWndEdit`) có tiêu điểm, tắt khi mất tiêu điểm. `Wnd_SetFocusWnd` là điểm móc |
| 1.2 | **Chuột phải** | JX1 dùng chuột phải để *đánh ép* (`Mouse_Force1`) và mở *menu chuột phải* (`Mouse_Menu`). Không có chuột phải thì không đánh được quái đã bị người khác đánh, không mở được menu người chơi | Chạm giữ ~400 ms mà ngón không xê dịch → sinh `WM_RBUTTONDOWN/UP` tại chỗ đó |
| 1.3 | **Rê chuột (hover)** | Thông tin vật phẩm, tên NPC, gợi ý nút — đều hiện khi *rê* chuột. Ngón tay không rê được | Chạm giữ trên ô đồ = hiện thông tin; hoặc gửi `WM_MOUSEMOVE` theo ngón trước khi nhả |
| 1.4 | **Nút Back của Android** | Hiện không làm gì (`SDLK_AC_BACK` không có trong `SdlKeyToVk`) | Ánh xạ thành `VK_ESCAPE` (mở bảng hệ thống / đóng cửa sổ) |
| 1.5 | Kéo–thả và bấm đúp | Chuyển đồ giữa túi, bán đồ, dùng vật phẩm | SDL đã sinh down/move/up nên **có thể đã chạy** — phải kiểm tay |
| 1.6 | Hai ngón: phóng to / kéo bản đồ | Không bắt buộc nhưng rất tiện trên màn nhỏ | `SetZoom` đã có sẵn trong `autoexec.lua` |

## 2. ĐƯA 10 GB DỮ LIỆU VÀO ĐIỆN THOẠI — chặn đường phát hành

Đo thật thư mục dữ liệu hiện tại: **10,22 GB / 102.633 tệp**.

| Thư mục | Cỡ | Số tệp |
|---|---|---|
| `data` (các tệp .pak) | 7.878 MB | **42** |
| `spr` (tệp lẻ) | 1.348 MB | 3.256 |
| `maps2` (tệp lẻ) | 771 MB | **95.365** |
| `maps` | 366 MB | 2.423 |
| `settings` | 79 MB | 701 |
| còn lại (`ui`, `script`, `userdata`) | ~6 MB | 836 |

| # | Việc | Ghi chú |
|---|---|---|
| 2.1 | **Kiểm xem thư mục lẻ có trùng với pak không** | `KPakList` tra tệp lẻ TRƯỚC, nên nếu pak đã chứa đủ thì bỏ hẳn `spr/` `maps2/` `maps/` → **10,2 GB còn ~8 GB và 102.633 tệp còn ~50 tệp**. Đây là việc rẻ nhất, lợi nhất — làm trước |
| 2.2 | Gom nốt tệp lẻ còn thiếu vào pak | 95.365 tệp lẻ khiến chép vào máy mất hàng chục phút và mở tệp cũng chậm |
| 2.3 | **Chọn cách phát hành dữ liệu** — *cần chủ quyết* | (a) tải trong app lần đầu từ máy chủ của chủ; (b) người chơi tự chép qua cáp/thẻ nhớ; (c) APK + tệp mở rộng. Với 8 GB thì (a) là cách duy nhất tử tế, nhưng phải làm bộ tải + kiểm tra hỏng + tải tiếp khi đứt |
| 2.4 | Chỗ trống & vị trí | `Android/data/vn.jx1.mobile/files` bị xoá khi gỡ app; nhiều máy 128 GB sẽ chật. Cần kiểm chỗ trống trước khi tải |

## 3. BỘ NHỚ VÀ HIỆU NĂNG CHO MÁY THẬT

| # | Việc | Vì sao |
|---|---|---|
| 3.1 | **`Rep3CacheMB=1500` quá lớn** | Đang đặt cứng 1500 MB ảnh trên máy ảo 4 GB. Điện thoại 4–6 GB RAM sẽ **bị hệ thống giết**. Phải tự tính theo RAM máy (ví dụ 1/6 RAM, kẹp 256–512 MB trên Android) |
| 3.2 | `PaintFps=120` | Tốn pin, nóng máy. Điện thoại nên 60 (hoặc theo tần số màn hình), có thể hạ khi pin yếu |
| 3.3 | Đo FPS / nhiệt / pin trên máy thật | Chưa có số nào từ GPU di động thật (Adreno/Mali) |
| 3.4 | Vulkan trên GPU di động | LDPlayer chạy Vulkan của máy tính. GPU di động khác hẳn: định dạng texture, bộ nhớ, driver |

## 4. CHẠY TRÊN ĐIỆN THOẠI THẬT — *cần chủ có máy*

Bản arm64 đã chạy qua **bộ dịch ARM** của LDPlayer, tức là mã máy arm64 đúng (kiểu dữ liệu LP64, bố cục struct, hợp ngữ). Nhưng chưa qua: CPU ARM thật, GPU di động thật, driver Vulkan thật, quyền tệp thật.
Việc này chỉ cần một cái điện thoại Android 7+ và khoảng 15 GB trống.

## 5. GIAO DIỆN CHO NGÓN TAY

| # | Việc |
|---|---|
| 5.1 | Nút/ô nhỏ so với ngón tay (thanh kỹ năng, ô túi). Cần vùng chạm rộng hơn phần vẽ |
| 5.2 | Tranh nền màn hình chính / đăng nhập là ảnh **4:3** → màn 20:9 chừa dải đen bên phải |
| 5.3 | Máy có **tai thỏ**: game đang vẽ tràn cả phần tai (`SHORT_EDGES`) nên góc giao diện có thể bị che |
| 5.4 | Chỉnh `ChieuCaoMucTieu` (đang 640) sau khi chủ soi trên máy thật |
| 5.5 | Nút ảo cho các phím tắt hay dùng (ngồi/nhặt/tự đánh) |

## 6. GÕ TIẾNG VIỆT TRONG CHAT

Bộ gõ của game (`KIme`, Telex) là mã Windows, đã **bị loại khỏi bản Android** (nằm trong 33 tệp Windows-only). Bàn phím Android gửi UTF-8, còn chữ trong game là **TCVN3**. Nhiều khả năng gõ tiếng Việt có dấu sẽ ra sai — **chưa ai thử**.

## 7. ÂM THANH

Hiệu ứng chạy (miniaudio). **Chưa có thư mục `Music`** nên không có nhạc nền — chỉ là chưa chép dữ liệu.

## 8. PHÁT HÀNH

| # | Việc |
|---|---|
| 8.1 | **Chưa ký APK release** — `build.gradle` mới chỉ có bản debug, chưa có `signingConfigs` |
| 8.2 | **Địa chỉ máy chủ đang trỏ `10.0.0.140`** (IP mạng LAN của máy này) → phải đổi thành IP/tên miền thật |
| 8.3 | Tên ứng dụng, biểu tượng, số phiên bản, ảnh giới thiệu |
| 8.4 | Gỡ nhật ký chẩn đoán (`[DPG]`, `[LOGIN]`, `[GO]`, `[FONT]`) |
| 8.5 | Cập nhật: người chơi lấy bản mới và dữ liệu mới bằng cách nào |
| 8.6 | Chống gian lận: `AntiHack` là mã Windows, đã bị loại khỏi bản Android — bản mobile hiện **không có lớp chống nào** |

## 9. VIỆC NHỎ CÒN TREO

- `LoadImage FAIL` còn ~26 lần (giống hệt bản PC, không phải lỗi riêng Android).
- WAuto ngoài không dùng được trên Android (không có bộ nhớ chia sẻ liên tiến trình); auto trong game vẫn chạy.
- Video mở đầu bị loại khỏi bản Android.
- Xoay màn hình: đang khoá `sensorLandscape` — chưa thử xoay ngược khi đang chơi.

---

## Thứ tự đề nghị

1. **§1 Điều khiển bằng ngón tay** (1.1 → 1.4 trước) — không có thì không gọi là chơi được.
2. **§2.1 Kiểm dữ liệu trùng** — rẻ, có thể cắt 102.633 tệp còn ~50.
3. **§3.1 Bộ nhớ ảnh theo RAM máy** — không sửa thì điện thoại sẽ bị giết giữa chừng.
4. **§4 Chủ chạy thử trên điện thoại thật** — chốt xem còn gì vỡ.
5. **§2.3 Chốt cách phát hành dữ liệu** — cần chủ quyết trước khi làm bộ tải.
6. §5, §6, §7 theo phản hồi khi chơi thật.
7. §8 khi chuẩn bị phát hành.
