# BÀN GIAO 04/09/2026 — WAuto: dựng lại giao diện theo auto Thái Lan (vlhkmp), đợt 1–5

Phiên `wauto` (Claude Opus 5). Chủ giao: *"thiết kế lại toàn bộ giao diện Auto như Auto thailan… Các nút, các note hướng dẫn, vị trí tab tôi cần bạn làm giống vậy cho auto WAuto"*.

Cách làm: mổ giao diện auto Thái từ **mã C# WinForms** (`D:\Source_ANTITHAILAN\...\Keoxe365\Keoxe365\Form1.cs` — toạ độ Designer là số pixel chính xác), 17 tác tử song song đọc từng phần + 3 phương án thiết kế + 1 vòng phản biện đối kháng. Không cần chạy AutoThaiLan.exe: ảnh chụp chủ gửi đã là tham chiếu hiển thị thật.
**Hướng chốt: "DÁNG THÁI — RUỘT WAUTO"** — bắt chước bố cục, tên nút, note hướng dẫn; giữ nguyên toàn bộ ID, cơ chế lưu `.dat`, 277 tooltip và 467 control sẵn có.

## 1. Tệp swap — CHỈ WAuto.exe

| Tệp | md5 | Cỡ | Ghi chú |
|---|---|---|---|
| `bin\client\WAuto.exe.moi` | `77ccc22d` | `455.168` | đợt 1–5. **`ChoiGame.bat` KHÔNG đổi tệp này → đổi tay.** |
| `bin\client\WAuto.exe` | `72862beb` | đang chạy | bản Ác chính 03/09 (nền của đợt này). |
| CoreClient.dll / Game.exe / CoreServer.dll | không đổi | | đợt này **chỉ sửa giao diện WAuto.exe**, không đụng game, không đụng `autoData`, không đụng IPC. |

**Checklist swap:** tắt WAuto → đổi `WAuto.exe` thành `WAuto.exe.truoc` → đổi `WAuto.exe.moi` thành `WAuto.exe` → mở lại.
**Lùi:** đổi ngược, hoặc build lại từ `WAuto.cpp.cu_0409_dot0` / `WAuto.rc.cu_0409_dot0` / `Resource.h.cu_0409_dot0` (bản trước đợt này, nằm cùng thư mục nguồn).

## 2. Cửa sổ mới trông thế nào (so với ảnh auto Thái)

```
 ┌──────────────────────────────────────────────┐
 │ WAuto — 6 cửa sổ, 4 đang bật auto   │A│H│Trợ giúp│   ← hàng 1 (Thái: chữ chạy + [A][H][Tài trợ])
 │ ┌────────┬─────────┬──────────────────┬───┐  │
 │ │Thường ▾│Ác chính │ <tên ác chính>  ▾│ X │  │   ← hàng 2 (Thái: combo chế độ + [Ac chính] + [X])
 │ └────────┴─────────┴──────────────────┴───┘  │
 │ [v] Tìm ác chính [200] mps [v] Trong thành │?│ ← hàng 3 (giống hệt Thái)
 │ ──────────────────────────────────────────── │
 │ ☑ Nhân vật │ SL │ NL │ Việc      │ Bản đồ    │   ← danh sách 5 cột (Thái: Tên nhân vật|Doing|Bản đồ)
 │ ☑ ThienLong│4210│ 980│ Tống Kim  │ Chiến trường│
 │ ☐ BachHo   │   0│   0│ —         │ —         │
 │ ┌────────┐┌────────┐┌────────┐┌────────┐     │   ← đài tab CẤP 1 (4 nhóm)
 │ │Điều    ││Hậu cần ││Cài đặt ││Hoạt    │     │
 │ │khiển ▮ ││        ││        ││động    │     │
 │ └────────┘└────────┘└────────┘└────────┘     │
 │   ┌────┐┌────┐┌────┐┌────┐                   │   ← đài tab CẤP 2 (tab con của nhóm)
 │   │Chiến││Chiêu││ PK ││Ác  │                  │
 │   │đấu ▮││KH  ││    ││chính│                 │
 │   └────┘└────┘└────┘└────┘                   │
 │   ┌────┐┌────┐                               │
 │   │Di  ││Nhặt│                               │
 │   │chuyển││đồ │                              │
 │   └────┘└────┘                               │
 │ ┌── Chiến đấu ─────────────────────────────┐ │   ← nội dung tab (467 control cũ, dịch xuống 34)
 │ │ …                                        │ │
 │ └──────────────────────────────────────────┘ │
 │ HĐ: Đang đánh Tống Kim, phe Tống             │
 │           Võ Lâm Ngạo Thế                    │
 └──────────────────────────────────────────────┘
```

## 3. Đã làm gì (5 đợt, đều nằm trong bản `77ccc22d`)

### Đợt 1 — đài tab xếp theo kiểu Thái (chỉ `WAuto.cpp`)
4 nhóm cấp 1 đổi thành **Điều khiển | Hậu cần | Cài đặt | Hoạt động** (Thái: Điều khiển | Hậu cần | Cài game | Phụ trợ), tab con xếp theo tần suất dùng:

| Nhóm | Tab con |
|---|---|
| **Điều khiển** | Chiến đấu · Chiêu KH · PK · Ác chính · Di chuyển · Nhặt đồ |
| **Hậu cần** | Hậu cần · Phục hồi · Tổ đội |
| **Cài đặt** | Cơ bản |
| **Hoạt động** | Dã Tẩu · TK · CTC · Liên đấu · Sát thủ · H.động |

Đủ 16 tab, không sót tính năng nào. Rút tên cho vừa nút: "Tống Kim" → **TK**, "Công Thành" → **CTC**, "Ac chính" → **Ác chính**. Mở máy vào thẳng tab **Chiến đấu** (Thái mở vào "Bảng 1").

### Đợt 2 — danh sách nhân vật 5 cột (chỉ `WAuto.cpp`)
`Nhân vật | SL | NL | Việc | Bản đồ` (Thái: `Tên nhân vật | Doing | Bản đồ`). Cột "Việc" lấy từ dòng trạng thái auto đã có sẵn trong gói đồng bộ, cột "Bản đồ" lấy từ tên map — **không cần đổi IPC, không cần đổi `autoData`, không cần gói tin mới**. Hai cột này là TCVN3 nên đi qua `g_Convert2UCEChar` rồi `delete[]`. Cột "Việc" chỉ vẽ lại khi đổi chữ (khỏi nháy). Chưa kết nối / vừa thêm dòng thì ghi "—".

### Đợt 3 — NOTE HƯỚNG DẪN (bắt chước `FormTip` của Thái)
- Hộp thoại mới `IDD_HELP_DIALOG` (210×180): không modal, luôn nổi trên cùng, **chỉ một hộp sống một lúc**, đặt cạnh cửa sổ WAuto, dấu `|` trong chuỗi = xuống dòng, **đếm ngược trên tiêu đề rồi tự đóng sau 600 giây** — đúng 7 nết của FormTip.
- Nội dung: **`s_aNote[16]` — viết mới toàn bộ 16 bài hướng dẫn tiếng Việt**, mỗi bài đánh số từng ô, có câu CẢNH BÁO và dòng LIÊN QUAN, cộng `s_szNoteChung` và `s_szNoteVungTren`.
- Ba đường mở: nút **[Trợ giúp]** (note của tab đang mở) · **[H]** (hướng dẫn chung) · **[?]** (vùng trên) · và bấm dòng chữ "Võ Lâm Ngạo Thế" ở đáy cũng mở note của tab đang mở.
- **Tooltip rê chuột 277 mục GIỮ NGUYÊN** (auto Thái không có tầng này — không bỏ).

### Đợt 4 — vùng trên 3 hàng + dịch cả khối nội dung
- 9 control mới, ID **720–728**, nằm **ngoài dải ẩn/hiện `[161,700)`** nên **luôn hiện ở mọi tab** đúng như Thái.
- Nút [A] quét lại danh sách cửa sổ game; [H] và [Trợ giúp] và [?] mở note; combo **chế độ nhanh** 6 mục (Thường / PK / TK / CTC / Liên đấu / Dã Tẩu) — chọn một mục là **vừa nhảy sang tab đó vừa bật công tắc chính** của nó; nút [Ác chính] đặt nhân vật đang chọn làm ác chính cho mọi cửa sổ khác; nút [X] xoá ác chính.
- Chữ hàng 1 đổi câu mỗi 6 giây, câu đầu đếm "N cửa sổ, M đang bật auto".
- 3 nút chrome vẽ tay (owner-draw) đúng màu Thái: [Trợ giúp] nền DarkSeaGreen, [A]/[H] nền LightSteelBlue, chữ [A] màu đỏ.
- **Không nắn lại 467 toạ độ trong `WAuto.rc`**: một hàm `WA_DichKhoiNoiDung` chạy đúng một lần lúc mở cửa sổ, dịch cả khối nội dung xuống `WA_DICH_Y = 34` đơn vị bằng `SetWindowPos(... SWP_NOSIZE ...)` — `SWP_NOSIZE` là điểm mấu chốt, nó **không đụng chiều cao thả xuống của 63 combo**. Danh sách nhân vật, 12 nút đài tab và 5 ô Ác chính nằm ngoài dải nên đặt tay. 16 lệnh đặt chỗ chép cứng trong `ShowTab` đã cộng thêm 34, đếm đủ không sót.

### Đợt 5 — 5 ô "Ác chính" lên vùng trên (giữ nguyên số ID)
Combo ác chính, [v] Tìm ác chính, ô mps, nhãn mps, [v] Trong thành **dời lên hàng 2 và hàng 3**, chỉnh được ở **mọi tab** thay vì phải mở tab Ác chính. Vì **giữ nguyên ID (634–638)** nên không phải sửa một dòng nào ở lưu/nạp cấu hình, nạp combo, tooltip hay bảng màu. Chữ rút gọn ("Tìm ác chính", "Trong thành"), câu đầy đủ nằm trong tooltip và note. Tab Ác chính còn lại ô "Đánh cùng mục tiêu ác chính" + dòng trạng thái, chiều cao 244 → 208.

## 4. Cách kiểm khi swap
1. Mở WAuto: hàng 1 có chữ chạy + [A] [H] [Trợ giúp]; hàng 2 có combo chế độ + [Ác chính] + combo tên + [X]; hàng 3 có [v] Tìm ác chính + ô số + [v] Trong thành + [?].
2. Bấm 4 nút nhóm: tên đúng **Điều khiển / Hậu cần / Cài đặt / Hoạt động**, tab con đúng bảng ở mục 3.
3. Bấm **[Trợ giúp]**: hộp "HƯỚNG DẪN — <tên tab> (600s)" hiện cạnh cửa sổ, số giây đếm lùi, bấm X đóng được, mở tab khác rồi bấm lại thì nội dung đổi theo tab.
4. Danh sách nhân vật đủ 5 cột, không có thanh cuộn ngang, cột "Việc" đổi theo việc auto đang làm.
5. Đổi tab qua lại 16 tab: **không control nào nhảy sai chỗ**, nhất là tab **Chiến đấu** và **PK** (hai tab có lệnh đặt chỗ chép cứng).
6. Combo chế độ nhanh: chọn "TK" → nhảy sang tab TK và ô "Bật auto Tống Kim" tự tick.
7. Ô "Tìm ác chính" ở hàng 3 chỉnh được khi đang mở bất kỳ tab nào; giá trị vẫn lưu đúng theo từng nhân vật.

## 5. Bẫy và việc còn lại
1. 🔴 **CHIỀU CAO CỬA SỔ**: dịch khối làm cửa sổ cao thêm ~64 px. Tab cao nhất là **Cơ bản** (≈837 px cả khung). Màn 1080p ở 100 % thì vừa; ở **125 %** có thể bị cắt đáy vì WAuto không khai báo DPI-aware. Cách chữa gốc là **đợt 6**: tách tab "Cơ bản" thành "Cơ bản" + "Đăng nhập" (hạ trần từ 362 xuống 335 đơn vị), chưa làm. Nếu chủ thấy cắt đáy thì báo, tôi làm đợt 6 ngay.
2. Hàng nút đáy kiểu Thái ([Bật hết] [Tắt hết] [Đồng bộ] [Toạ độ]) thuộc **đợt 6**, chưa có.
3. Chưa thêm tooltip cho 9 control mới (đã có note qua nút [?] và [H]).
4. Đính chính: nút màu ngoài cùng bên phải của auto Thái chữ thật là **"Tài trợ"** (xoay banner quảng cáo), không phải "Trợ giúp". WAuto không có quảng cáo nên khe đó làm nút **Trợ giúp** thật, giữ đúng vị trí và màu.
5. Ba thứ **cố ý không bắt chước**: giữ bề ngang 160 đơn vị (Thái hẹp hơn 16 px — hạ xuống phải nắn 472 control); giữ cơ chế một hộp thoại + ẩn/hiện theo dải ID (không chuyển sang TabControl lồng như Thái); giữ 277 tooltip.

## 6. Bộ vá (idempotent, chạy lại được)
- `ReverseTools\goi_va_wauto_ui_thai_0409.py` — đợt 1 + 2
- `ReverseTools\goi_va_wauto_ui_thai_0409_note.py` — đợt 3
- `ReverseTools\goi_va_wauto_ui_thai_0409_tren.py` — đợt 4 + 5

Build: `msbuild WAuto.vcxproj -p:Configuration=Release -p:Platform=Win32` trong `E:\Src_Auto_Ngoai\WAuto\WAuto` → `Release\WAuto.exe`.
**BẪY**: `WAuto.cpp` và `WAuto.rc` là UTF-16LE (đọc/ghi `io.open(..., encoding='utf-16', newline='')`), `Resource.h` là ASCII. Nhiều dòng "trống" trong `WAuto.cpp` thật ra là hai ký tự tab — neo vá phải theo DÒNG, không dùng khối cứng.
