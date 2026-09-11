# KẾ HOẠCH: GIAO DIỆN MOBILE TỰ CĂN ĐỀU TRÊN MỌI MÁY + TRÌNH CHỈNH CHO NGƯỜI CHƠI (12/09)

> **Trạng thái 13/09:** đã phân tích lại (`PHANTICH_GIAODIEN_MOBILE_KEHOACH1209.md`) và làm xong đợt đầu theo
> phương án A' (khung vẽ hai họ + vùng an toàn + trình chỉnh danh sách trắng), xem
> `BANGIAO_GIAODIEN_MOBILE_SUAGD_1309.md`. Ba chẩn đoán trong tệp này đã được đính chính ở đó (§4.1 gốc "cả cụm",
> §3.2 neo, §3.3 "cha trước con").

> Nhánh `mobile-0809`. Đọc kèm `KEHOACH_GIAODIEN_MOBILE_0909.md` (nguồn tham khảo VNKU/USVOLAM),
> `BANGIAO_ANDROID_DULIEU_1109.md` (lớp ghi đè dữ liệu).
>
> **Chủ game chốt 12/09 tối:**
> 1. Mọi cỡ màn điện thoại đều phải căn đều, không lệch, không lọt ra ngoài.
> 2. Có tính năng cho **người chơi tự chỉnh vị trí và phóng to nhỏ TỪNG icon**, chính xác tuyệt đối,
>    không phải kéo là đi cả cụm như hiện tại.
> 3. Phương án phải **dùng lại được cho bản iOS**.
> 4. **Chỉ bản mobile**, bản PC không đụng tới.
> 5. Không bị giới hạn bởi engine hiện tại — nếu cách khác tốt hơn thì đổi.

---

## 1. Vì sao phải lập lại kế hoạch

Ngày 12/09 tôi vá bốn lần liên tiếp cho lỗi "icon bên trái lọt ra ngoài" và **cả bốn đều sai**. Ghi lại
để không lặp:

| Lần | Giả thiết | Thực tế |
|---|---|---|
| 1 | Khung vẽ rơi đúng 1024 nên game nhảy sang bộ giao diện 1024x768 | Đúng, nhưng chỉ là **một** lỗi riêng, không phải lỗi chủ đang gặp |
| 2 | Màn hẹp hơn 1040 nên thanh dưới bị đẩy trái | Đúng với máy tính bảng, không đúng với máy chủ |
| 3 | Màn Fold cong nên nuốt dải sát mép | **Sai** — chủ nói màn phẳng |
| 4 | Chưa cập nhật bản cài | **Sai** — log máy chủ tải ghi rõ 18:45:21 máy chủ đã tải APK, 18:45:33 tải bố cục |

Bản vá lề còn **hỏng về kỹ thuật**: nó đọc toạ độ tuyệt đối rồi đẩy ô, nhưng ô cha chưa chắc đã được đặt
xong trước ô con, nên với bố cục riêng của chủ nó đẩy **sai chiều**, icon ra xa hơn. Đã gỡ hẳn bằng
`[UITOADO 12/09 LE x]`.

**Bài học vào quy tắc làm việc:** không vá bố cục khi chưa có số đo từ **máy thật của chủ**. Máy ảo
LDPlayer chặn bề ngang 2080 px nên không dựng được màn 2520 px của Fold 7, và máy ảo dùng tệp bố cục
riêng khác máy chủ — nên "đo trên máy ảo thấy đúng" **không chứng minh được gì**.

### Số liệu đã đo được (giữ lại làm mốc)

- Khung vẽ máy chủ (Galaxy Z Fold 7, màn ngoài): **1440x616**, chủ tự đọc trên máy.
- Quét 27 khung vẽ sinh ra từ 30 dòng máy đang bán: **0 cửa sổ ra ngoài khung** ở tất cả.
- Ở đúng khung 1440x616, các mục nằm **đúng x = 0**:
  `KSysMsgCentrePad|SysRoom` và 3 nút cuộn của nó, `KUiPlayerBar|HideChat`,
  `KUiMsgCentrePad|Main` (x = 2).
- Engine **đã có** phóng đều: `CDevGpu::Letterbox` lấy `min` hai chiều rồi căn giữa → hình không méo.
- SDL trong dự án là **3.2.14**, đã có `SDL_GetWindowSafeArea`.
- Mã mobile đang rào bằng `JX_ANDROID`: **172 chỗ / 59 tệp**.
- Tầng vẽ chỉ có shader **SPIR-V** (`Rep3ShadersGPU_spv.h`), `SDL_CreateGPUDevice` xin đúng
  `SDL_GPU_SHADERFORMAT_SPIRV` → **iOS sẽ cần bản MSL**, xem §5.

---

## 2. Năm phương án và lý do chọn

| Phương án | Mọi cỡ màn | Chỉnh từng icon | Dùng lại cho iOS | Công | Rủi ro bản PC |
|---|---|---|---|---|---|
| **A. Giữ engine, thêm lớp HUD: neo mép + vùng an toàn + trình chỉnh** | Đủ | Đủ | Nguyên vẹn (C++ thuần) | Vừa | Không |
| B. Khung ảo cố định, để letterbox lo | Đủ, có viền đen ở tỉ lệ lệch xa | Vẫn phải làm thêm | Nguyên vẹn | Nhỏ | Không |
| C. Viết lại giao diện bằng bộ mới | Đủ | Đủ | Được | Rất lớn | Cao |
| D. HUD bằng giao diện gốc hệ điều hành | Đủ | Đủ | **Viết hai lần** (Java + Swift) | Lớn | Vừa |
| E. Dò tay bảng toạ độ cho từng nhóm máy | Không | Không | Không | Nhỏ nhưng lặp mãi | Không |

**Chọn A, giữ B làm lưới an toàn.**

- **D bị loại vì yêu cầu iOS**: làm HUD bằng `View` của Android rồi làm lại bằng UIKit là nhân đôi công,
  nhân đôi lỗi, và rối phần định tuyến chạm với bàn phím.
- **C bị loại vì rủi ro**: viết lại toolkit đời 2005 kéo theo toàn bộ hộp thoại, danh sách, ô đồ, giao
  dịch, cửa hàng.
- **E chính là cách đang làm** và là nguyên nhân của ngày hôm nay.
- Bằng chứng thực tế cho A: **VNKU và USVOLAM cũng không viết lại engine**, họ giữ hệ cửa sổ JX1 rồi
  chồng lớp HUD lên (xem `KEHOACH_GIAODIEN_MOBILE_0909.md` §"Phát hiện quan trọng nhất").

---

## 3. Bốn thay đổi trong engine (phần A)

Cả bốn nằm trong rào biên dịch mobile, **bản PC không đổi một dòng hành vi**.

1. **Không gian bố cục = VÙNG AN TOÀN**, lấy từ `SDL_GetWindowSafeArea` rồi đổi sang toạ độ khung vẽ.
   Hiện mọi thứ tính từ `0,0` đến hết khung. Trên iOS chính hàm này trả về phần tránh tai thỏ và thanh
   Home → làm bây giờ là dùng được luôn cho iOS.
2. **Neo đổi nghĩa: từ "dịch" sang "DÍNH MÉP"**. Mỗi mục khai *dính mép nào, cách bao nhiêu* thay cho
   *toạ độ tuyệt đối rồi cộng trừ theo neo*. Neo hiện tại chỉ dịch một khoảng nên tỉ lệ lệch xa là hở;
   dính mép thì **tính lại** nên khung nào cũng đúng.
3. **Một lượt tính duy nhất**, chạy sau khi cây cửa sổ dựng xong, **cha trước con**. Bản vá 12/09 chết
   đúng vì thiếu quy tắc này.
4. **Dò trúng cửa sổ con sâu nhất** khi chạm, để chọn được từng icon thay vì tóm khung cha. Đoạn đệ quy
   này đã viết và chạy được hôm nay trong `JxUi_CoVatPhamTaiDiem` (`Wnds.cpp`), dùng lại.

**Công tắc dự phòng (phần B):** `[Ui] KhungCoDinh=1` → dùng một khung ảo cố định, để `CDevGpu::Letterbox`
lo phần còn lại. Mọi máy thấy giao diện y hệt nhau, đổi lại có viền đen. Chi phí gần bằng không vì
letterbox đã chạy sẵn.

---

## 4. Trình chỉnh giao diện cho người chơi — thiết kế chi tiết

### 4.1 Vì sao hiện tại khó chỉnh

Không phải lỗi thiết lập mà là **cấu trúc**: `KUiPlayerBar` là **một** cửa sổ 800x600 chứa hàng chục
icon con. Chế độ sửa dò cửa sổ dưới ngón bằng `Wnd_GetActive`, hàm này chỉ trả cửa sổ **cấp cao nhất**
→ luôn tóm trúng khung to, kéo một cái là cả cụm đi.

### 4.2 Phần dữ liệu đã có sẵn

Mỗi mục đã lưu `Left, Top, TiLe, Co` = vị trí, **tỉ lệ phóng**, cờ ẩn. `UiDatTiLe` phóng **cả vùng bấm**
chứ không chỉ phóng ảnh (chụp kích thước gốc một lần, luôn tính từ gốc nên áp lại bao nhiêu lần cũng ra
một kết quả). `UiDatAn` đã có. Nghĩa là **chỉnh to nhỏ từng icon vốn đã chạy được**, chỉ thiếu chỗ cho
người chơi chạm tới.

### 4.3 Cách thao tác — chọn cho DỄ NHẤT với người chơi

Thứ tự ưu tiên theo đúng thói quen người chơi mobile:

1. **Chạm một cái để CHỌN.** Mục được chọn sáng viền, mọi mục khác mờ đi, hiện tên + `x, y, tỉ lệ %`.
2. **Kéo để đặt thô.** Có **bắt dính**: dính lưới, và dính mép/tâm của icon bên cạnh kèm đường gióng.
   Đây mới là thứ làm việc kéo "ăn tay", chứ không phải độ nhạy.
3. **Chụm hai ngón để phóng to nhỏ** ngay trên icon đang chọn. Tự nhiên và dễ khám phá nhất.
4. **Bảng nút nổi để chỉnh CHÍNH XÁC TUYỆT ĐỐI** — vì ngón tay không bao giờ đặt nổi 1 điểm ảnh:
   - 4 mũi tên: mỗi lần bấm dịch **1 điểm**, giữ thì chạy đều.
   - `−` / `+` tỉ lệ theo nấc **5 %**, kèm số phần trăm.
   - `Ẩn` / `Hiện`, `Mặc định` (trả riêng mục này), `Hoàn tác`, `Xong`.
5. **Công tắc `một icon` / `cả cụm`**, vì đôi khi cần dời nguyên hàng.
6. **Chỉ mở khoảng 30 mục HUD** cho người chơi chỉnh, phần còn lại khoá. Mở hết vài trăm cửa sổ là rối
   và dễ làm hỏng hộp thoại.
7. **Hồ sơ bố cục**: lưu theo từng nhân vật và từng máy; nút `chép bố cục` sinh một mã ngắn để mang sang
   máy khác.
8. **Nút xem thử ở cỡ màn khác** ngay trong trình chỉnh, để người chơi thấy bố cục của mình không vỡ
   trên máy hẹp hơn.

> **Tóm lại, cách dễ nhất = kéo có bắt dính + chụm hai ngón để phóng, và mũi tên 1 điểm cho nét cuối.**
> Kéo lo phần nhanh, bắt dính lo phần gọn, mũi tên lo phần chính xác.

### 4.4 Việc phải viết thêm

| Việc | Đã có gì | Phải làm gì |
|---|---|---|
| Chọn đúng icon | đệ quy dò con sâu nhất trong `Wnds.cpp` | dùng lại, thêm lọc theo danh sách 30 mục |
| Dời | `SetPosition` + bảng khoá theo tên | ghi lại vào bảng, thêm hoàn tác |
| Phóng | `UiDatTiLe` (phóng cả vùng bấm) | nút `−`/`+` và chụm hai ngón |
| Ẩn | `UiDatAn` | nút |
| Lưu | tệp bố cục đã lưu `Left,Top,TiLe,Co` theo khoá | tách hồ sơ theo nhân vật/máy |
| Chụm hai ngón | **chưa có** | thêm xử lý đa chạm trong `KSdlApp` |
| Bắt dính + đường gióng | **chưa có** | vẽ + tính, không đụng engine |

---

## 5. Chuẩn bị cho iOS

- **Phần bố cục dùng lại nguyên**: toàn bộ là C++ trong `S3Client` và `Represent`, không dính JNI.
- **Làm ngay, gần như miễn phí:** mã **mới** của phần bố cục rào bằng `JX_MOBILE` (đặt cho cả Android và
  iOS) thay vì `JX_ANDROID`. 172 chỗ cũ cứ để yên, chuyển dần. Không làm bây giờ thì sau phải sửa tay
  từng chỗ.
- **Việc chặn thật sự, nằm NGOÀI phần giao diện:** tầng vẽ chỉ có shader SPIR-V. `SDL_CreateGPUDevice`
  đang xin `SDL_GPU_SHADERFORMAT_SPIRV`, trong nguồn chỉ có `Rep3ShadersGPU_spv.h` và bản HLSL cho
  D3D11, **không có bản Metal**. iOS chạy SDL_GPU trên Metal nên cần dịch chéo sang MSL (SPIRV-Cross)
  và xin `SDL_GPU_SHADERFORMAT_MSL`. Không có bước này thì bản iOS không chạy dù giao diện đã sẵn sàng.

---

## 6. Thứ tự làm và cách nghiệm thu

| Bước | Nội dung | Nghiệm thu |
|---|---|---|
| 1 | **Bộ thu số liệu từ máy thật**: ghi cỡ cửa sổ, khung vẽ, hệ số + lề letterbox, vùng an toàn, toạ độ cuối của từng cửa sổ; gửi lên máy chủ tải (máy chủ vốn đã nói chuyện với nó) | Có tệp của máy Fold 7 để đối chiếu |
| 2 | **Vùng an toàn** vào không gian bố cục | Ba mục đang ở `x = 0` vào trong, đo trên máy chủ |
| 3 | **Neo dính mép**, sinh tự động từ bản PC | Ở khung 1040x604 nhìn y hệt hiện tại, sai lệch 0 |
| 4 | **Bộ xem trước ngoại tuyến**: vẽ 27 khung ra ảnh | Soát một lượt, không cần máy ảo |
| 5 | **Trình chỉnh cho người chơi** (§4) | Chủ tự chỉnh được từng icon, không đi cả cụm |
| 6 | **Dọn tệp bố cục riêng**: theo dính mép, tách theo máy, bỏ `userdata` khỏi bộ tải | Máy này không thừa hưởng bố cục máy kia |
| 7 | *(khi làm iOS)* shader MSL | Bản iOS lên hình |

Mỗi bước có công tắc tắt trong `config.ini` để hỏng là quay về ngay.

---

## 7. Việc còn treo, cần chủ quyết

1. Thứ tự: làm **bước 1** trước, hay làm **bước 5 (trình chỉnh)** trước để chủ tự kéo cho vừa mắt?
2. Icon đấu giá và thư đang đặt `Top = 0`, sát mép trên — vùng vuốt thanh trạng thái của Android nuốt
   chạm nên bấm không ăn trên máy thật (máy ảo không có vùng này nên vẫn bấm được). Hạ xuống hay giữ?
3. Động vật trang trí trên mobile: bảng `npc_res_kind_file_name.txt` trong dữ liệu điện thoại **chỉ có
   `critter008`**, và thư mục `spr\npcres\critter` **rỗng hoàn toàn** — bộ rút gọn đã cắt vì tính năng
   trước nay vẫn tắt. Muốn có thì phải đóng gói lại kèm ảnh, gói tải nặng thêm (chưa đo).
4. `KPakFile::Open` với `m_nPakFileMode = 0` mở tệp **trên đĩa trước** rồi mới tìm trong pak — bộ tải
   điện thoại cố tình giữ một số tệp rời nên tệp rời sẽ che bản trong pak. Cần rà lại danh sách giữ rời.
