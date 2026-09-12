# BÀN GIAO: BẢN ĐO NHỊP VẼ TRÊN ĐIỆN THOẠI THẬT — `[DONHIP 12/09]`

> Worktree `D:\GAMEDEVNEW\.claude\worktrees\mobile-144hz-optimization-429e2b`, nhánh `claude/mobile-144hz-optimization-429e2b`.
> Đọc kèm `BANGIAO_NHIP_VE_144HZ_0809.md` (tối ưu nhịp vẽ của bản PC) — bản đo này kiểm các khác biệt mobile ↔ PC trên máy thật.

## 0. Trạng thái (cập nhật 22:05)

> **17:35 11/09 — BƯỚC 1 CỦA ĐỢT PORT ĐÃ LÊN BỘ TẢI: `[CHUATLAS 11/09]` chữ vào atlas.** Chủ 17:2x: "oke hãy làm và đo từng bước",
> kèm lưu ý có phiên làm iOS trên MacBook khác có thể đẩy git. dt_v4 = **109111729** (md5 `14442104…`, máy chủ 8765 PID 383480),
> commit `0e74d95a` trên `origin/mobile-0809`, APK lưu `android/apk/jx1mobile-1109-chuatlas.apk`.
> - **Việc:** `CAtlasMgrGpu::Eligible` cho `D3DPOOL_MANAGED` vào atlas. Đúng một điều kiện, y như bản PC làm ở `[MANG 09/09 d]`, vì texture ảo
>   trên mobile đã hỗ trợ bản CPU và tải lại vùng bẩn. Xác nhận trong mã: `REP3_POOL_MANAGED` = `D3DPOOL_MANAGED` khi `Rep3Ex=0`
>   (`D3D_Device.h:83`), nên chữ và ảnh dựng sẵn của nhân vật trước nay bị loại khỏi atlas **chỉ vì loại bộ nhớ**.
> - **Bộ đếm để đo và để bắt rủi ro:** `[VE-GOP]` thêm `o atlas: DEFAULT n, MANAGED m (managed=1), xin o moi k`. Rủi ro của bước này là
>   texture MANAGED bị ghi lại thường xuyên sẽ phải **xin ô mới** mỗi khung; nếu `xin o moi` tăng vọt thì tắt.
> - **Thử máy ảo:** không lỗi, chữ hiển thị đúng hoàn toàn (nút, tiêu đề, chữ đỏ "(New)", chữ nhỏ góc trái); log `o atlas: DEFAULT 0,
>   MANAGED 1 (managed=1), xin o moi 0`.
> - **Chủ test:** mở lại app, nhìn chữ trước hết (nhãn tên trên đầu nhân vật, dòng chat, số sát thương, chữ trong bảng), rồi Tống Kim 10 đến
>   15 phút. Mốc so sánh: lý do quad không gộp hiện là **texture0 82 đến 88 %**, pipeline 12 đến 17 %; kỳ vọng texture0 tụt. Tắt nhanh nếu chữ
>   sai: `Rep3AtlasManaged=0` trong `[Client]` của config dt_v4 rồi khởi động lại 8765.
> - **Bước 2 và 3 của đợt port** (loại mặt khuất trên CPU, rồi atlas khối cố định gắn sẵn sampler) làm sau khi đọc log bước 1.
> - **Lưu ý phối hợp:** có phiên iOS trên máy khác có thể đẩy git, nên mọi lần đẩy đều phải `git fetch` rồi gộp, tuyệt đối không ép đẩy.

> **17:00 11/09 — TÌM RA HƯỚNG ĐÚNG: NHÁNH CHÍNH ĐÃ GIẢI XONG BÀI TOÁN NÀY CHO BẢN PC, MOBILE CHƯA PORT.** Chủ chỉ: bản sửa "nằm ở main
> chính của dự án, không phải main mobile"; bản PC chạy 144 fps, đông vẫn trên 135.
> Loạt commit `[MANG 09/09]` a đến f trên `origin/main` sửa cho **đường vẽ D3D11** (`D3D9on11*.cpp`), còn mobile chạy **đường SDL_GPU / Vulkan**
> (`D3D9onGPU*.cpp`) nên không được hưởng:
>
> | Bước | Commit | Việc | Số đo của bản PC |
> |---|---|---|---|
> | a | `0b855da7` | atlas thành Texture2DArray | bỏ 95,9 % lý do vỡ lô quad |
> | b | `9190fab1` | alpha test theo đỉnh, hai mảng atlas gắn cố định khe t3/t4, mảng lớn lên không kèm dữ liệu | bỏ khung 20 đến 57 ms |
> | c | `c6e98e0f` | bộ đệm đỉnh 4 lên 16 MB | |
> | d | `4ee8e6ad` | **chữ (texture MANAGED) vào atlas** + tham số tầng 0 theo đỉnh | hai lý do vỡ lô cuối |
> | e | `ac7d255b` | **cull trên CPU cho lệnh 2D** | 92 % lý do vỡ lô |
> | f | `5311778b` | atlas theo **khối cố định** (R8G8 32 lớp/64 MB, tối đa 16 khối), trường lớp 9 bit = khối<<5 hoặc lớp, shader chọn khối bằng switch trên **16 sampler gắn cố định** | hết đổi binding texture |
>
> **Vì sao C1 của tôi hỏng còn bản PC thành công.** Tôi làm cụm tăng dần 2/4/8 lớp với một sampler mảng duy nhất, nên vẫn phải đổi binding
> giữa các cụm. Bản PC làm khối lớn cố định 32 lớp và **gắn cố định 16 mảng vào các khe sampler**, chọn bằng chỉ số theo đỉnh, nên không bao giờ
> đổi binding. Đó là khác biệt quyết định.
> **Hai nguyên nhân của bản PC đều đúng với mobile, đã kiểm trong mã và log:** đường SDL_GPU có gộp `D3DRS_CULLMODE` vào khoá pipeline
> (`D3D9onGPUDev.cpp:437`), và chữ trên mobile là MANAGED nên `CAtlasMgrGpu::Eligible` loại khỏi atlas. Log mobile: lý do quad không gộp là
> texture0 82 đến 88 %, pipeline 12 đến 17 %.
> **Việc kế, thay cho mọi hướng tôi tự nghĩ trước đó:** port sang đường SDL_GPU theo thứ tự **(d) chữ vào atlas → (e) cull trên CPU → (f) khối
> cố định gắn sampler**, mỗi bước một công tắc và đo bằng đúng bộ log này.

> **16:30 11/09 — C1 HỎNG TRÊN MÁY THẬT, ĐÃ TẮT NGAY BẰNG CONFIG.** Chủ 16:25: "đợt này di chuyển màn hình hay bị giật và FPS tụt xuống 25".
> Đã đặt `Rep3AtlasMangGpu=0` (tên khoá đổi lúc 16:30 vì `Rep3AtlasMang` đã là khoá của đường vẽ D3D11; khoá cũ trong dt_v4 nay vô hại vì mặc định là 0) trong `D:\jx1_android_data_dt_v4\config.ini` và khởi động lại 8765 (PID 369628) lúc 16:27; APK giữ nguyên 109111608 nên
> chủ chỉ cần **mở lại app** là về đúng hành vi bản 109111545. Mặc định trong mã cũng đã đổi sang 0 (commit sau), mã giữ lại sau công tắc.
>
> **Số đo phiên `SM-F966U1_20260911_162152`** (trung vị cửa sổ 30 s, bỏ màn đăng nhập):
>
> | | D1 109111459 | C bước 1 109111545 | C1 109111608 |
> |---|---|---|---|
> | đổi texture / khung | 844 | 1 043 | **508** |
> | lệnh vẽ / khung | 1 677 | 2 001 | **1 222** |
> | ghi lệnh | 2,59 ms | 2,26 ms | 2,28 ms |
> | **nộp** | 0,64 ms | 1,42 ms | **3,28 ms** |
> | **vẽ CPU** | 2,60 ms | 2,96 ms | **4,51 ms** |
>
> **Đọc số:** C1 làm đúng phần cơ học của nó (gộp được lệnh: đổi texture giảm một nửa, lệnh vẽ giảm 39 %), **nhưng GPU trả giá đắt hơn nhiều
> phần tiết kiệm được**: nộp tăng 2,3 lần và vẽ CPU tăng 1,5 lần → fps tụt, giật khi cuộn màn hình. 143 dòng `[VE-GIAT]`, 135 trong số đó phần nặng
> nhất là "vẽ khác" (mã vẽ của client), nạp và chép đều 0,0 ms → không phải nạp sprite, không phải tải texture.
> **Giải thích khả dĩ:** lấy mẫu qua texture mảng 2048×2048×8 lớp với chỉ số lớp đổi theo từng quad làm mất tính cục bộ của bộ đệm texture trên
> Adreno, và 5 cụm đã cấp 240 MB trong khi chỉ dùng 208 MB nên phần bộ nhớ thừa càng ép bộ đệm. Bài học: **gộp lệnh không tự động thắng** khi cách
> gộp làm GPU lấy mẫu xấu đi; phải đo cả hai phía chứ không chỉ đếm lệnh.
> **Việc kế:** bỏ hướng C1 (giữ mã sau công tắc để thử lại với trang nhỏ hơn hoặc ít lớp hơn nếu muốn); quay lại **A2** (lưới an toàn nhiệt) và **E**
> (nạp sprite ở luồng nền khi NPC xuất hiện), và xem lại nghi vấn nộp tăng của bước 1 (`Rep3PsBuffer=0` để đối chứng).

> **16:15 11/09 — C1 ĐÃ LÊN BỘ TẢI: dt_v4 = 109111608** (md5 `bdcf2baa…`, 20 355 055 B, máy chủ 8765 PID 374324, giữ nguyên
> `data/sprvuhontieudao3.pak` + `package.ini`), commit `[MANG 11/09]`, `origin/mobile-0809 = ee3c3cdd`. Chủ 16:13: "nếu bạn lấy đủ log rồi thì đẩy lên".
>
> **Số chốt của bước 1** (hai phiên giờ CÙNG mức đông nên so được trực tiếp; trung vị cửa sổ 30 s có npc ≥ 100):
>
> | | D1 109111459 (33 cửa sổ, npc 116 · đạn 114) | C bước 1 109111545 (24 cửa sổ, npc 123 · đạn 103) |
> |---|---|---|
> | fps / thấp nhất | 114 / 110 | 115 / 110 |
> | ghi lệnh | 2,93 ms | **2,44 ms** |
> | **µs mỗi lệnh vẽ** | 1,37 | **1,09 (−20 %)** |
> | đổi trạng thái pixel / khung | 1 105 | **0** |
> | đổi texture / khung | 1 146 | 1 154 (phần của C1) |
> | nộp | 0,72 ms | 1,30 ms |
> | CPU tiến trình / luồng chính | 80 / 75 % | 79 / 74 % |
> | điện · GPU bận | 3,57 W · 75 % | 3,99 W · 80 % |
>
> **Điểm cần theo dõi:** nộp +0,58 ms, điện +0,42 W và GPU 75 → 80 % ở cảnh tương đương. Nghi do shader đọc bảng trạng thái từ storage buffer cho
> **từng điểm ảnh** (4,3 triệu điểm mỗi khung) thay vì từ uniform như trước; CPU thì giảm đúng như thiết kế. C1 không đụng phần này, nên nếu log C1
> vẫn thấy nộp và GPU cao thì bước sau nên thử đưa bảng trạng thái về uniform theo lô, hoặc rút gọn tổ hợp (chỉ 5 tổ hợp mỗi khung) thành vài hằng số
> trong shader. Nếu muốn đối chứng ngay: `Rep3PsBuffer=0` trong config dt_v4 rồi khởi động lại 8765.
>
> **Chủ test bản 109111608:** mở lại app. Kiểm **hình** trước hết vì C1 đổi cách sprite nằm trong bộ nhớ GPU: nhân vật, quái, hiệu ứng kỹ năng, chữ,
> giao diện, vật phẩm trong hành trang, ảnh nền đăng nhập; sai sẽ lộ thành ảnh lẫn sang sprite khác hoặc ô trống. Rồi Tống Kim 10–15 phút.
> Tôi đọc `[VE-GOP]` (đổi texture mỗi khung, kỳ vọng tụt mạnh từ 1 154), `[VE]` (ghi lệnh, lệnh mỗi khung), `[MANG]` (số cụm atlas), `[MAU]` (W, GPU).
> Hỏng hình → `Rep3AtlasMangGpu=0` (tên khoá đổi lúc 16:30 vì `Rep3AtlasMang` đã là khoá của đường vẽ D3D11; khoá cũ trong dt_v4 nay vô hại vì mặc định là 0) trong `[Client]` của config dt_v4 rồi khởi động lại 8765, không cần APK mới.

> **16:10 11/09 — KẾT QUẢ BƯỚC 1 TRÊN FOLD 7 + C1 ĐÃ DỰNG XONG (CHƯA ĐẨY BỘ TẢI).** Chủ 15:52: "tôi mới up bản mới rồi tí nữa bạn lấy log —
> phải dựa vào log và lịch trình định sẵn". Phiên `SM-F966U1_20260911_155303` (bản 109111545, màn trong, Tống Kim liên tục, 99 cửa sổ 10 s).
>
> | Cửa sổ **rất đông** (npc ≥ 100), trung vị | D1 109111459 (33 cửa sổ) | C bước 1 109111545 (16 cửa sổ) |
> |---|---|---|
> | npc/tick · đạn/tick của phiên | 82 · 61 | **112 · 112** (cảnh nặng gần gấp đôi) |
> | fps / thấp nhất | 114 / 110 | 115 / 110 |
> | ghi lệnh ms | 2,93 | 2,38 |
> | **µs mỗi lệnh vẽ** | 1,37 | **1,07 (−22 %)** |
> | đổi trạng thái pixel / khung | 1 105 | **0** |
> | đổi texture / khung | 1 146 | 1 139 (chưa đụng, đó là C1) |
> | nộp ms | 0,72 | 1,34 |
> | CPU tiến trình / luồng chính | 80 / 75 % | 77 / 73 % |
>
> Đọc số: **C2 đạt đúng mục tiêu** (không còn lần đẩy uniform nào, bảng chỉ 5 tổ hợp, tràn 0) và **C3 rút giá mỗi lệnh 22 %**; CPU vẫn giảm nhẹ dù
> tải nặng gần gấp đôi. Điện 3,57 → 4,01 W và GPU 75 → 80 % là do cảnh nặng hơn, không so trực tiếp được. **Nộp tăng 0,72 → 1,34 ms** là điểm duy nhất
> cần theo dõi (nghi do GPU bận hơn ở cảnh nặng; bảng trạng thái chỉ 400 byte/khung nên không phải do tải bảng).
> **C1 đã viết + dựng + thử máy ảo, CHƯA thay vào dt_v4** vì chủ còn đang đo bản 109111545 (pin 6 %) — đẩy lúc này sẽ phá phép đo. Commit
> `[MANG 11/09]`, APK 109111608 (md5 `bdcf2baa…`, `android/apk/jx1mobile-1109-c1.apk`): nhiều trang atlas nằm trong một texture mảng 2D (mỗi trang một
> lớp, chỉ số lớp đi theo đỉnh ở bit 25..30 của ô PALROW), cụm cấp tăng dần 2 → 4 → 8 lớp trong ngân sách 64 MB, trang rỗng **trả lớp** về cụm ở cả hai
> đường trả trang (texture là của cụm, huỷ nhầm là mất hết sprite), texture riêng và texture trắng cũng tạo dạng mảng một lớp, texture tầng 1 bị ép ra
> khỏi atlas. Bốn mảng shader cũ giữ nguyên từng byte. Máy ảo: màn menu và bảng chọn máy chủ y hệt, log `[VE] atlas mang 2D=1` và
> `[MANG] cum atlas moi: 2048x2048 x 2 lop fmt 3 (16 MB), tong 1 cum`, không lỗi. Công tắc tắt: `Rep3AtlasMangGpu=0` (tên khoá đổi lúc 16:30 vì `Rep3AtlasMang` đã là khoá của đường vẽ D3D11; khoá cũ trong dt_v4 nay vô hại vì mặc định là 0).

> **15:50 11/09 — ĐỢT C BƯỚC 1 ĐÃ LÊN BỘ TẢI: `[GOP 11/09]` trạng thái tầng texture theo ĐỈNH + bind ring một lần** — commit `00a09114`
> = `origin/mobile-0809` (FF cả `wt_mobile`), **dt_v4 = 109111545** (md5 `fb90eceb…`, 20 322 287 B, máy chủ 8765 PID 362344, giữ nguyên
> `data/sprvuhontieudao3.pak` + `package.ini` của phiên giao diện), APK lưu `android/apk/jx1mobile-1109-gop1.apk`. Chủ 15:35: "làm C và
> hãy nhớ kiểm tra bên phiên client PC có gì mới cập nhật vào cho đồng bộ".
> - **Đồng bộ phía PC:** gộp `origin/main = 714cbed0` (`0bc49e49` [TK-RUONG+TK-CUA] WAuto Tống Kim, chỉ `Core/Src/CoreShell.cpp`, không đụng
>   Represent3 / S3Client / android). Đã hỏi cả ba phiên PC (wauto-bb, wauto-d2, wauto-80): không phiên nào còn thay đổi chưa đẩy; wauto-80
>   xác nhận `0bc49e49` chỉ dùng hàm sẵn có của Core nên bố cục lớp không đổi (an toàn cho bản mobile dùng chung Core).
> - **C2 (ps theo đỉnh):** `RgPsCb` 80 byte (colorop / alphaop / alpha test / lọc) không còn đẩy uniform mỗi lệnh vẽ. Mỗi khung gom các tổ hợp
>   **duy nhất** vào storage buffer 4 096 mục, chỉ số 12 bit đi theo đỉnh trong ô PALROW (bit 0..12 hàng bảng màu, 13..24 chỉ số ps, 25..30 để
>   dành cho lớp atlas của bước sau). Nhờ vậy hai quad **chỉ khác trạng thái tầng texture vẫn gộp chung một lệnh vẽ** (so trạng thái bỏ phần ps).
>   Tra bảng: nhớ ô cuối rồi mới tra bảng băm FNV-1a; va chạm băm vẫn kiểm lại bằng memcmp nên không thể trả nhầm tổ hợp.
> - **C3 (bind ring một lần):** bộ đệm đỉnh bind một lần mỗi render pass, mỗi lệnh vẽ dùng `first_vertex = ringOff / (stride+4)` (ringOff được
>   căn lên bội `stride+4` khi mở lệnh mới) → bớt một lệnh Vulkan cho **mỗi** draw (Tống Kim 1 818 draw/khung).
> - **Giữ PC nguyên byte:** biến thể shader thứ ba `g_Rep3GpuFSPalPs` nằm trong `#ifdef JX_ANDROID`; nhánh không-`JX_PS_BUFFER` dùng **macro**
>   `JX_PALROW`/`JX_PALKHONG` nên sinh đúng chuỗi token cũ — đã kiểm: `g_Rep3GpuVS`, `g_Rep3GpuFS` (PC) và `g_Rep3GpuFSPalBuf` (bản 109111459)
>   **giữ nguyên từng byte**. (Lần đầu viết bằng biến cục bộ làm hai mảng cũ đổi byte → đã hoàn nguyên và làm lại bằng macro.)
> - **Thử máy ảo:** không sập, màn menu + bảng chọn máy chủ màu y hệt bản D1 (nút vàng, chữ đỏ "(New)", khung đúng); log
>   `ps theo dinh=1, bind ring mot lan=1`, `bang trang thai tang texture: storage buffer 4096 muc x 80 byte (320 KB)`, `[VE-GOP] … ps bang 4 muc`,
>   0 dòng "that bai". **Chưa thử được trong thế giới** (chủ đang chơi trên Fold 7, máy ảo dùng chung tài khoản sẽ đá phiên).
> - **Công tắc tắt nhanh** trong `[Client]` của config dt_v4: `Rep3PsBuffer=0` (về đúng bản 109111459), `Rep3BindRing=0`, `Rep3PalBuffer=0`
>   (về hẳn shader PC, tắt cả hai). Kỳ vọng: ghi lệnh 2,6 → ~1,3–1,8 ms lúc đông, CPU luồng chính 85 → ~70 % ở cửa sổ 800 đạn/tick.
>   Bài test: §10 mục 8. Việc kế: **C1** (atlas thành texture mảng 2D — texture0 chiếm 73 % lý do không gộp), rồi **A2**, **E**.

> **15:25 11/09 — KẾT QUẢ FOLD 7 BẢN D1 109111459 (phiên `SM-F966U1_20260911_151020`, màn trong, 12 phút, gần như toàn bộ là Tống Kim; máy BẮT ĐẦU
> ĐÃ NÓNG: nhiệt 3, headroom 0,98, 37,1 °C — ngay sau 55 phút Tống Kim của phiên 14:16).** Chủ 15:20: "lấy log đi bạn". `[D1] swapchain 1040x936 | backbuffer
> 1040x936 | cua so 2184x1968 | SDL: tao 1040x936; extent min 1x1 max 4096x4096` → đúng thiết kế, không lỗi, không "tắt hint".
>
> | Tống Kim (npc 60–200, trung vị cửa sổ 30 s) | 14:16 trước D1 (84 cửa sổ) | 15:10 D1 (20 cửa sổ) |
> |---|---|---|
> | fps / fps nhỏ nhất trong cửa sổ | 109 / 96 | **115 / 110** |
> | paint ms | 8,0 | 7,2 |
> | nộp TB (max) ms — chờ GPU | 1,6 (10,7) | **0,8 (5,9)** |
> | GPU bận @ xung | **83 % @ 648 MHz** | **73 % @ 336 MHz** |
> | điện W | **4,61** | **3,35 (−27 %)** |
> | CPU tiến trình / luồng chính | 73 / 68 % | 76 / 70 % |
> | nhiệt / headroom | 3 / 0,98 | 2 / 0,90 (giảm dù đang đánh) |
>
> Cửa sổ đông nhất (npc ≥ 100): đạn 85 → **118/tick (nặng hơn)** mà fps 107 (min 92) → **115 (min 112)**, GPU 85 % @ 655 → 74 % @ 332 MHz, W 4,75 → 3,36;
> cửa sổ 801 đạn/tick + 145 NPC + 2 797 lệnh: **111 fps** (trước D1 các cửa sổ 700–900 đạn/tick chỉ 70–89 fps) — nhưng CPU 96 / 85 % → **CPU giờ là
> nút thắt lúc đông nhất** (ghi 3,2 ms, vẽ CPU 3,3 ms). Nhiệt độ pin 37,1 → 36,1–36,4 °C **giảm trong lúc đánh** (phiên trước tăng 33 → 37,3); pin 26 → 22 %
> trong 11 phút (22 %/giờ, trước 26 %/giờ). `[VE-GIAT]` 171 dòng: "vẽ khác" 155 (CPU vẽ lúc đông), **nộp 3 (trước 62)**, chép 1. Đăng nhập 30 fps, 20 lần `[DOI]`
> đầu phiên (Samsung đổi 60/120 lúc mở app), không "that bai". Cảnh yên chỉ 1 cửa sổ (chủ vào Tống Kim ngay): 1,16 W lúc máy còn nóng.
> **Kết luận:** D1 làm đúng việc — GPU không còn là nút thắt (nộp/chờ GPU giảm 2–3 lần, xung GPU giảm một nửa), điện Tống Kim −27 %, fps lúc đông nhất
> 70–89 → 111–115 và máy nguội dần thay vì nóng lên. Hình ảnh: chờ chủ xác nhận bằng mắt (log không thấy được). **Việc kế: C** (gộp lệnh: CPU 96 / 85 % ở
> 800 đạn/tick), rồi **A2** (lưới an toàn nhiệt) và **BKG b** (xin 60 Hz khi đứng yên).

> **15:15 11/09 — D1 ĐÃ LÊN BỘ TẢI: `[D1 11/09]` swapchain theo KHUNG LOGIC** — commit `9851000b` = `origin/mobile-0809` (FF cả `wt_mobile`), **dt_v4 = 109111459**
> (md5 `cf951865…`, 20 256 751 B, máy chủ 8765 PID 366076, tệp rời giữ nguyên), APK lưu `android/apk/jx1mobile-1109-d1.apk`. Chủ 14:55: "làm d1 ngay"
> (log 14:16: Tống Kim màn trong GPU bận 82–99 % ở 650–950 MHz trong khi game chỉ vẽ 1040×936).
> - **SDL** (`android/va_sdl3_d1.py`, CMake tự chạy SAU `va_sdl3_donhip.py`, chỉ cây Android): `VULKAN_INTERNAL_CreateSwapchain` lấy hint `JX_SWAPCHAIN_W/H`
>   (> 0) thay cho kích thước cửa sổ, kẹp trong `[minImageExtent, maxImageExtent]`, ghi lại `JX_SWAPCHAIN_THAT` / `JX_SWAPCHAIN_EXTENT`; `vkCreateSwapchainKHR`
>   từ chối → tắt hint, tạo lại bằng cửa sổ (một lần); acquire: hint đổi so với lần tạo gần nhất → dựng lại; dựng lại > 8 lần/2 s khi đang dùng hint → tắt hint
>   (chống lặp kiểu M1). Không hint = hành vi 3.2.30 nguyên bản.
> - **Represent3** (`android/va_nguon_android_d1.py`): `[Client] Rep3SwapchainLogic` (100 = khung logic — mặc định; 150 = 1,5× nếu muốn nét hơn; 0 = cửa sổ như cũ);
>   `JxDatHintSwapchain` đặt hint trước `SDL_ClaimWindowForGPUDevice` và trong `Reset` (gập/mở → `SetGPUSwapchainParameters` dựng lại với hint mới; bằng cửa sổ
>   thì hint 0); `[D1]` trong `jx_rep3.log` mỗi khi kích thước swapchain nhận từ acquire đổi. `Letterbox()` đọc kích thước thật → tỷ lệ tự về 1, không sửa thêm.
> - **Thử máy ảo** (màn giả `wm size 2080x1208`, đã reset về 1040×604 và mở game bấm nút kiểm): loader Vulkan Android báo `extent min 1x1 max 4096x4096`,
>   tạo swapchain **1060×616 trong cửa sổ 2080×1208**, ảnh phủ đúng toàn màn, không sập; cỡ thường hint 0 → 1040×604 như cũ.
> - **Kỳ vọng Fold 7:** màn trong swapchain 1040×936 (÷4,4 điểm GPU), màn ngoài 1436×616 (÷3,1) → Tống Kim GPU bận và xung GPU giảm, điện giảm, fps lúc đông
>   bớt tụt; hình có thể mịn hơn một chút nếu trước đây lọc điểm (so bằng `Rep3SwapchainLogic=0`). Bài test + cách đọc: §10 mục 7.

> **14:50 11/09 — KẾT QUẢ FOLD 7 BẢN 109111313 (phiên `SM-F966U1_20260911_141642`, màn trong, 28 phút: 8 phút cảnh yên trong thành rồi 20 phút Tống Kim;
> bộ đọc `scratchpad/phan_tich_bkg.py` + `phan_tich_tong.py` nhóm G; mốc so = phiên 11:48 cùng màn trong, bản f).** Chủ 14:40: "lấy log về phân tích".
> 1. **Cảnh yên (npc ≤ 12, 14 cửa sổ 30 s so với 35 cửa sổ của 11:48):** fps 120 (118); **CPU tiến trình/luồng chính 36 / 31 % (53 / 44 %)**; **điện trung vị
>    1,99 W (2,39 W), có cửa sổ 1,5–1,8 W**; 6 nhân hiệu năng đứng ở **556 MHz**, 2 nhân lớn 1 017 MHz (xung nghỉ); GPU 37–39 % bận ở 160 MHz (xung thấp nhất).
>    `[VE-BKG]`: chỉ **29 % khung được trình chiếu** (bỏ 71 %; cả phiên bỏ 34 480/185 889 = 19 % vì 20 phút đông không bỏ được); chuỗi bỏ dài nhất 6 khung
>    = đúng một tick 55 ms (mỗi tick logic đổi hoạt ảnh NPC → khung khác → trình chiếu ~18–36 lần/giây khi đứng yên); "giống nhưng có tải" 3, "ép" 34
>    (trần 250 ms hầu như không cần). Không dòng "that bai", chủ chơi liên tục 28 phút không thoát.
> 2. **Khựng do bảng màu HẾT:** `[VE-GIAT]` chép ≥ 10 ms **1 lần/28 phút** (11:48: 229 lần/21 phút = 10,8/phút), lần đó là tải thật 21 texture 20 MB lúc
>    vào map; `lenh tai bang mau` 0,000–0,001 ms/khung; phần lớn nhất của `[VE-GIAT]` chuyển từ "chép" (229) sang "vẽ khác" (445, gần hết ở cảnh đông, vẽ CPU
>    24–37 ms khi sinh NPC hàng loạt lúc vào đám đông t=494 s) và "nộp" (62, chờ GPU lúc đông).
> 3. **[FPSNGOAI] chạy đúng:** 20 s đầu 299–300 khung/10 s (PaintFps 30), màn xuống 60 Hz lúc đăng nhập (`[DOI]`), vào thế giới về 120 Hz / 120 fps.
> 4. **Tống Kim (npc 60–200, 40 cửa sổ) — như trước, đúng dự kiến (BKG/PALBUF không nhắm chỗ này):** fps trung vị 109, tụt 70–79 ở 700–900 đạn/tick; paint
>    7,8 ms; CPU 72 / 68 %; **điện trung vị 4,7 W (p90 6,2, đỉnh 12 W)**; **GPU 82 % bận ở 652 MHz, đỉnh 99 % ở 866–956 MHz → lúc đông màn trong nghẽn GPU thật**
>    (lần đầu có số GPU: `[GPU-SYS]` Fold 7 đọc được `kgsl/gpu_busy_percentage` + `devfreq/cur_freq`); CPU 6 nhân 1,4–2,7 GHz. Nhiệt 0 → 1 (4 phút vào đông)
>    → 2 → **3 SEVERE sau ~8 phút**, pin 33 → 37 °C, 49 → 37 % trong 28 phút. `[VE-GOP]` cả phiên đổi/khung TB pipeline 175, texture 656, ps 708; lệnh 1 700–2 800/khung,
>    ghi 2,3–4,7 ms, nộp 1,3–4,4 ms.
> 5. **Kết luận:** đợt 1 đạt mục tiêu ở cảnh chơi bình thường (−17 điểm CPU, −0,4 W, SoC về xung nghỉ, hết khựng bảng màu, hình đúng theo log — chờ chủ xác nhận
>    mắt). Nóng/pin giờ chỉ còn ở cảnh đông, và ở đó **GPU là nút thắt trước CPU** → việc kế: **D1** (swapchain = khung logic 1040×936, GPU tô ÷4,4, hint SDL nhỏ)
>    rồi **C** (atlas mảng + ps theo đỉnh + first_vertex: lệnh 1 700–2 800 → 300–500), và **A2** (hạ nấc khi nhiệt ≥ 2 — lưới an toàn vì 8 phút Tống Kim đã SEVERE).
>    Có thể thêm **BKG b**: khi > 70 % khung bị bỏ trong 2 s thì xin màn 60 Hz (đứng yên panel vẫn 120 Hz suốt 8 phút, ~0,3 W).

> **13:20 11/09 — SỬA TẬN GỐC ĐỢT 1 ĐÃ LÊN BỘ TẢI: `[BKG 11/09]` + `[PALBUF 11/09]` + `[FPSNGOAI 11/09]` + `[MAU 11/09]`** — commit `ccb66888`,
> `origin/mobile-0809 = 9177d84a` (đã FF, cây `D:\GAMEDEVNEW_wt_mobile` = 9177d84a), **dt_v4 = 109111313** (md5 `250bd14a…`, 20 256 747 B, máy chủ 8765
> PID 304848 chạy từ worktree này, tệp rời của phiên giao diện giữ nguyên), APK lưu `android/apk/jx1mobile-1109-bkg1.apk`. Chủ 12:40: "thực hiện các bước
> và hướng dẫn test lấy log; fix ở bản mobile không ảnh hưởng bản PC; up git lên bản mobile; phiên khác đang làm thì yêu cầu đồng bộ trước khi build".
> Đã hỏi 5 phiên trước khi dựng: giao diện đồng bộ 21e969b0 → 1270a115 → 164e5889 (đã gộp hết); DONHIP cũ, wauto-bb/-d2/-80 không liên quan; gộp cả
> `origin/main d8105a1a` (TKMS/TKFIX — xung đột `UiRankData.cpp` giữ ép kiểu `KUPARAM` của mobile). Phương án + tư vấn: `PHUONGAN_FPS_NHIET_PIN_MOBILE_1109.md`
> §6; bộ vá tái tạo `android/va_nguon_android_bkg1.py` (latin-1, CRLF, byte cao không đổi) + `ReverseTools/mobile_x64/dich_shader_gpu.py`. Mọi dòng C++
> mới trong `#ifdef JX_ANDROID` hoặc tệp chỉ-Android; mảng shader PC trong `Rep3ShadersGPU_spv.h` giữ nguyên byte (0 dòng xoá).
> 1. **[BKG] Khung giống hệt khung vừa trình chiếu thì không trình chiếu** (`CDevGpu::JxBoKhungGiong`, gọi từ `Present`): so `m_cmds` + `m_ring` với bản
>    khung vừa trình chiếu bằng `memcmp` (RgCmd được memset 0). Chỉ bỏ khi: không texture / bảng màu / vùng 0 chờ tải, không flush giữa khung
>    (`SubmitFrame(false)` đặt `m_bJxKhungCoFlush`), chưa quá `Rep3BoKhungGiongMs` (250) kể từ lần trình chiếu trước, không bị ép. Ép = `Rep3_JxEpTrinhChieu`
>    do `KSdlApp::TranslateEvent` gọi trước `switch` khi SHOWN / EXPOSED / RESIZED / PIXEL_SIZE / RESTORED / DISPLAY / SAFE_AREA / FOCUS_GAINED /
>    DID_ENTER_FOREGROUND; `Reset` cũng ép. Bỏ = `FrameReset()` không submit (texture chạm, ô atlas, bảng màu dọn như thường). Đếm `[VE-BKG]` mỗi kỳ:
>    trình chiếu / bỏ / giống-có-tải / ép / chỉ đếm / chuỗi bỏ dài nhất. `[Client] Rep3BoKhungGiong` 1 (mặc định) / 0 chỉ đếm / -1 tắt hẳn.
>    Máy ảo màn menu 30 s: 1 830 khung → trình chiếu 117, bỏ 1 713, ép 115 (đúng trần 250 ms), chuỗi 15; ảnh chụp lúc đang bỏ hiện đúng, không đen.
> 2. **[PALBUF] Bảng màu = storage buffer** 8192 hàng × 1 KB (`m_pJxPalBuf`, tạo ngay trong `Init`), hàng mới đi cùng staging `m_texStage` → `SDL_UploadToGPUBuffer`
>    (`m_jxPalUploads`); shader biến thể `-DJX_PAL_BUFFER` = `g_Rep3GpuFSPalBuf` (2 sampler + `readonly buffer PalBuf` set 2 binding 2, `unpackUnorm4x8(...).zyx`);
>    `[Client] Rep3PalBuffer=0` = texture 256×8192 cũ. Đo riêng lệnh tải bảng màu ở cả hai kiểu: `[VE] … bang mau kieu …: lenh tai TB`, `[VE-GIAT] … pal lenh`.
>    Gốc: phiên 11:48 260/260 khung "chép" ≥ 10 ms là khung tải hàng bảng màu vào texture (≈ 20 ms cố định, không theo Hz/KB). Máy ảo: 4 hàng/30 s, 0,0 ms.
> 3. **[FPSNGOAI] Ngoài thế giới vẽ 30 fps + xin màn 60 Hz** (`FpsNgoai_Nhip` trong `PerfHud_Draw`; `LoadSetting` áp đè 2 lần lúc mở app nên hễ thấy nhịp ≠ 30 là
>    đặt lại; vào thế giới (`KUiToolsControlBar::GetSelf()`) áp lại `JxNhip_DatMuc(s_nFpsMuc)`; không chạy khi `[DoNhip] Bat=1`). `[Client] FpsNgoaiTheGioi=30` (0 tắt).
>    Máy ảo: 300 khung/10 s ở menu (trước 604).
> 4. **[MAU] Java** `[MAU]` thêm ` | gpu=NN% gpu_mhz=… cpu_mhz=a/b/…` (kgsl `gpu_busy_percentage` / `gpuclk`, `scaling_cur_freq` từng nhân) + dòng `[GPU-SYS]`
>    báo đường sysfs đọc được (Fold 7 có cho đọc không → xem log phiên tới).
> **Chưa thử được TRONG thế giới trên máy ảo** (chủ đang chơi trên Fold 7 — phiên 13:06 còn cập nhật — máy ảo đăng nhập cùng tài khoản sẽ đá phiên);
> chỉ thử tới màn menu 2 lượt: không sập, màu đúng, bỏ khung + 30 fps chạy. Bài test trên Fold 7 + cách đọc log: **§10**. Việc kế theo §6 phương án:
> C (atlas mảng 2D + ps theo đỉnh + first_vertex) → D1 (swapchain = khung logic, hint SDL) → E (nạp spr nền khi NPC sinh) → A2 (hạ nấc theo nhiệt, lưới an toàn).

> **11:30 11/09 — PHÂN TÍCH TOÀN BỘ LOG FOLD 7 MỘT LƯỢT** (chủ: "đọc hết log lại rồi phân tích một lần rồi báo tôi"). 39 phiên, 22:05 10/09 → 11:23 11/09;
> bộ đọc `scratchpad/phan_tich_tong.py` (bảng theo phiên, cửa sổ 30 s, fps theo độ đông, nguồn giật, nhiệt) + tra chỉ mục pak bằng `name2id` của
> `ReverseTools/pakcheck.py`. Chưa sửa gì thêm — chờ chủ chọn hướng.
> 1. **FPS:** từ bản [DAN] (01:40) trung vị 115–119 fps ở 120 Hz, p10 105–113, giây < 55 khung 0,2–1,1/phút (đêm trước DAN 12–41/phút, có pha đo
>    cố ý xấu). Tụt theo đợt: vào bản đồ (30–60 s đầu 86–100 fps), đám đông mới tới (màn trong 10:22: 69–71 fps suốt 60 s; 10:47: 100, min 64),
>    cảnh rất đông (09:31 t=2667: 2 909 lệnh/khung → 114 fps màn ngoài; 10:22 t=1411: 2 524 lệnh → 97 fps màn trong).
> 2. **Màn trong nặng hơn màn ngoài:** mở gập 2184x1968 (khung vẽ 1040x936) vẽ 6,5–8 ms/khung, màn ngoài (1436x616 → 2520x1080) 4–7 ms: thấy thêm
>    ~50 % chiều cao bản đồ (nhiều sprite hơn) và GPU tô ở độ phân giải gốc 4,3 triệu điểm (ngoài 2,7 triệu). Ngân sách 120 Hz = 8,3 ms → màn trong
>    gần hết dư. Đám đông 80–150 NPC/tick: màn ngoài 117 fps (p10 106), màn trong 109 (p10 78).
> 3. **Chi phí mỗi khung trên luồng chính** (hồi quy cửa sổ 30 s, r 0,77–0,96): ghi lệnh Vulkan ≈ 1,0 ms + 1,0 ms/1 000 lệnh; vẽ CPU Begin→End
>    ≈ 1,6 ms + 0,7–1,0 ms/1 000 quad; nộp 0,3–2,5 ms; chép lên GPU 0,1–0,5 ms; tick game 0,2–1,8 ms. Quá 8,3 ms thì vsync 120 Hz bắt khung chờ
>    vblank kế → khung đó thành 60 fps → cả cửa sổ 60–100 fps.
> 4. **Nóng / CPU:** CPU tiến trình 60–80 % của MỘT nhân (luồng chính 55–78 %); thanh thông tin chia 8 nhân nên chỉ hiện ~8–10 %. Điện năng tăng theo
>    khối lượng vẽ (r 0,85–0,91): dưới 500 lệnh/khung 2,1–2,6 W, từ 1 200 lệnh 3,6–4,8 W, cảnh đông nhất 5–6 W; 60 fps (phiên 11:11) 1,6–2,0 W, CPU
>    37–47 %. Nhiệt: sáng nay tối đa MODERATE (2) ở màn trong sau ~20 phút đông (pin 35,8 °C); đêm qua SEVERE (3), pin 38,6 °C sau 20 phút liên tục.
> 5. **Tick game ≥ 100 ms** đều là lúc đổi bản đồ (phiên 10:47: 14/14 lần trùng đổi map, 150–300 ms/lần) — có từ trước, không do bản VE.
> 6. **Lỗi hiển thị — ba loại khác nhau:** (a) bản d+e (10:47–11:11): 54 lần tạo trang atlas = 54 lần tô 0 đè ảnh vừa tải → ảnh ẩn hiện (đăng nhập,
>    map, ngựa); bản f (11:11, 109111058) đã đổi thứ tự; log không ghi được "texture trống" nên cần chủ nhìn lại. (b) CŨ, không do VE: mỗi phiên
>    40–141 sprite nạp hỏng và không bao giờ nạp lại được (0 lần "OK sau khi thất bại"), gồm đầu ngựa `MA_HR_015_HD.spr`, `FM_HR_015_HD.spr`,
>    `FM_HR_002_HD.spr`, quái `enemy141/157/162/165/167/169`, `boss130/202`, `passerby*`, `critter*`, `sizenpc\cuongthi1/phapsu`, `\Spr\Ui3\小地图\MiniMap.spr`,
>    nút kênh chat 攻方/守方/闲聊; tra chỉ mục: KHÔNG có trong pak nào của client nguồn mobile (TESTLOFFF), client `D:\SourceVs22\SOURCEDANGONLINE`
>    (kể cả `sprvlngaothe2.pak`), máy ảo hay điện thoại → thiếu tài nguyên client, máy ảo hỏng y hệt. (c) MỚI, chỉ điện thoại: 3 cây
>    `\游戏资源\美术图素\城市\西南\室外\树木\雪松\1.spr`, `雪松\2.spr`, `\游戏资源\美术图素\城市\两湖\室外\树木\菩提树3.spr` không bao giờ vẽ (菩提树3 bị bỏ
>    0,1–0,77 triệu lượt mỗi phiên): `XPackFile::GetSprHeader` cất số thứ tự mục pak vào 16 bit (`*((WORD*)&pSpr->Reserved[2]) = (WORD)nElemIndex`),
>    `GetSprFrame` đọc lại 16 bit; `mobile_13.pak` có 70 602 mục, 3 SPR nén-theo-khung ở mục 66 328 / 67 606 / 69 604 → bị cắt thành 792 / 2 070 /
>    4 068 (tệp `.dat` bản đồ, không có cờ TYPE_FRAME) → mọi khung NULL. Byte trong pak giống hệt PC; máy ảo dùng pak PC (`resource.pak` 2 810 mục)
>    nên không lỗi. Sửa phía dữ liệu (đóng pak để SPR-khung không vượt mục 65 535, hoặc < 65 536 mục/pak) giữ nguyên PC.
> 7. **"Ảnh null" 100–200 nghìn lượt/30 s** phần lớn là tên rỗng (4,77 triệu/phiên 10:47) → loại ngay dòng đầu `GetImage`, không tốn thời gian.
> **Đề xuất (chưa làm):** (i) chơi lâu / màn trong: nấc 60 hoặc 90, tự hạ 60 khi nhiệt ≥ MODERATE; (ii) màn trong: vẽ cảnh vào khung 1040x936 rồi
> phóng một lần (bớt ~4 lần điểm GPU phải tô) — đo GPU trước; (iii) gộp lệnh / giảm đổi pipeline (58–197 lần/khung) để bớt 1–2 ms ghi lệnh lúc đông;
> (iv) đóng lại pak điện thoại cho 3 cây; (v) xin tài nguyên client đủ (đầu ngựa 015/002, quái thiếu, MiniMap, nút chat).

> **11:00 11/09 — LỖI HIỂN THỊ CỦA BẢN d+e (109111021) ĐÃ SỬA: `[VE 11/09 f]` = `5a72a37a`, bộ tải = 109111058 (md5 `74e8ea6e…`), máy chủ 8765
> PID 345452.** Chủ báo 10:55 trên điện thoại: "lúc hiện lúc không spr lúc đăng nhập, vào game lỗi hiển thị map, cưỡi ngựa lúc ẩn lúc hiện đầu
> đuôi ngựa". Nguyên nhân (lỗi của tôi trong `[VE 11/09 d]`): khối tô 0 trang atlas mới đặt SAU khối tải nội dung texture trong cùng copy pass;
> trang mới và những ảnh đầu tiên trên nó nằm trong cùng một khung, lệnh copy chạy theo thứ tự → tô 0 đè lên ảnh vừa tải; bản CPU đã bỏ (BOCPU)
> nên texture trống cho tới khi cache bỏ và nạp lại → UI đăng nhập ẩn hiện, ô map trống, khung ngựa trống theo chu kỳ hoạt ảnh. Trước d, vùng 0
> nằm chung hàng `m_texUploads` nên luôn đi trước. Sửa = chuyển khối vùng 0 lên đầu copy pass (`android/va_nguon_android_ve4.py`, chỉ đổi chỗ).
> Máy ảo 10:34 không lộ vì cảnh tĩnh, không cưỡi ngựa, không nhìn kỹ màn đăng nhập → bài học: mọi thay đổi đường tải texture phải thử cảnh có
> trang atlas mới sinh + hoạt ảnh nhiều khung (ngựa) + màn đăng nhập. Số phiên 10:47 (d+e, 8 phút, có lỗi): `[VE-GOP]` texture0 77 % (b: 86–89 %),
> đổi texture TB 273/khung (b: 416–498), ghi lệnh lúc đông 2,25 ms (b: 2,1–3,35) → e chỉ giảm một phần; fps đám đông vẫn 64–95; chủ: "fps tụt,
> máy nóng, cpu cao chưa thay đổi gì nhiều" → BƯỚC KẾ = đo kỹ phần "vẽ khác" (mã vẽ client ngoài Represent3) + tick logic lúc đông trước khi sửa tiếp.

> **10:30 11/09 — ĐÃ ĐỌC LOG FOLD 7 PHIÊN 09:31 (bản 109110317 có `[VE b+c]`, 33 phút) → `[VE 11/09 d]` đã viết + dịch (109111016), CHỜ THỬ MÁY ẢO.**
> Mục 1 chạy đúng trên máy thật: `[VE-NAP]` giao 70 297 → xong 69 534, hỏng 625; giật vẽ ≥ 40 ms 13,1 → 4,6 lần/phút so với y2; khung nạp > 16 ms
> 2,9 → 1,5 lần/phút; cửa sổ đông nhất 83–95 → 99–108 fps; `nhiet=0` suốt phiên ở 120 Hz (1,9–3,4 W). Hai điểm nghẽn mới (chi tiết §9 cuối,
> "Kết quả Fold 7 09:31"): (a) **chép lên GPU 40–200 ms/khung** ở 203/326 khung chậm, 134/179 khung ấy tải < 500 KB → không phải lượng dữ liệu;
> nghi SDL cấp/giải phóng khối bộ nhớ (bảng màu tạo/huỷ transfer buffer mỗi khung; trang atlas mới memset 2–4 MB vào staging → transfer
> buffer phình 32 MB rồi cycle); (b) `[VE-GOP]`: quad không gộp do **texture0 86 %**, pipeline 11 %, ps 2 % → mục 3 = gom trang atlas.
> `[VE 11/09 d]` (commit `d272bd99`, bộ vá `android/va_nguon_android_ve2.py` áp sau ve1): bảng màu + vùng 0 atlas qua bộ đệm cố định, đồng hồ con từng
> bước chép trong `[VE-GIAT]`/`[VE]`, hỏi kích thước NPC đang nạp nền → "chưa có" (`[Client] NapHoiKhongDe=1`), khung rỗng không giao lại.
> `[VE 11/09 e]` (commit `a501b532`, bộ vá `android/va_nguon_android_ve3.py`): mục 3 — atlas xếp KỆ theo định dạng (một trang nhiều hàng bin,
> `CAtlasMgrGpu::JxAllocKe/JxFreeKe`), trang 2048², công tắc `[Client] Rep3AtlasKe=1 Rep3AtlasTrang=2048`; vùng 0 tải theo dải từ bộ đệm 0 cố định
> 2 MiB. **10:40: d+e (109111021) ĐÃ THỬ MÁY ẢO TRONG GAME 150 s (chủ cho dừng app máy ảo): không crash; chép lên GPU TB 0,02–0,03 ms/khung,
> khung nặng nhất 11,8 ms là khung vào map tải 93 texture 22 MB thật (UI); "zero 2 vùng 0,0 ms", "bảng màu 116 hàng 0,1 ms"; `[VE-GIAT]` chỉ 4 dòng/180 s
> (bản b cùng chỗ: hàng chục); `[VE-GOP]` với kệ 2048²: **4 trang** thay vì 30+ trang 1024², đổi texture 71–73/khung (bản b: 161–184), texture0
> 52–54 k/30 s (bản b: 189–220 k), lệnh/khung 122–127 (bản b: 230–270); "hỏi NPC đang nạp → chưa có 94" lúc vào map. Máy ảo đã cài lại 109110954.
> `origin/mobile-0809` = `7a15c0bc`. **10:47: đã đẩy thẳng `dt_v4\jx1mobile.apk` = 109111021 (md5 `bdc338a8…`, 20 116 463 B, = fab69766 + d + e, có cả
> [BANPHIM]/[KINHMACH] của phiên giao diện), máy chủ 8765 PID 317268, config/tệp rời dt_v4 giữ nguyên → chủ mở lại app là nhận d+e ngay lúc đang chơi
> đám đông; phiên giao diện sẽ thay đè bằng bản gộp [PHONGBANG] sau.** Việc kế: đọc `[VE]`/`[VE-GIAT] chep:`/`[VE-GOP] atlas` của phiên Fold 7 kế tiếp.

> **03:25 11/09 — `[VE 11/09 b+c]` ĐÃ THỬ MÁY ẢO TRONG GAME, ĐÃ LÊN BỘ TẢI trong bản gộp của phiên giao diện: `dt_v4\jx1mobile.apk` =
> versionCode **109110317**, md5 `139f7284…`, 20 099 023 B, dựng từ `origin/mobile-0809 = 32c469d0` (= icon Tống Kim/Kinh Mạch + ô dùng nhanh
> của họ + `[VE 11/09 + b + c]`), máy chủ 8765 PID 301072, dt_v4\config.ini giữ nguyên (5 khoá `NapKhung*`/`VeGiatMs` có mặc định trong mã).**
> Máy ảo 03:18–03:20 (bản `b`, Khoả Lang Động, 26–40 đơn vị NPC/khung): không crash; `[VE-NAP]` giao 514 → xong 475, hỏng 0; nạp trước 2 khung
> làm "bỏ vẽ" tụt 407 → 4–26 lượt/30 s; luồng nền bận 18–55 ms/30 s; áp texture trên luồng vẽ ≤ 0,42 ms/khung; `[REP3-NAP]` "khung có nạp
> > 16 ms" còn 1/30 s và đều là **nạp đồng bộ NGOÀI lúc vẽ** (logic hỏi `GetImageParam` → nạp cả sprite, 253 lần/200 ms ở cửa sổ đầu) →
> việc kế (đợt 2b). `[VE-GOP]` (máy ảo): quad không gộp do **texture0 ≈ 80 %**, pipeline ≈ 20 %, ps ≈ 6 % → mục 3 = gom trang atlas (trang
> đang chia theo chiều cao bin), không phải cache giao diện. `[VE 11/09 c]`: ngưỡng `[VE-GIAT]` và đếm khung > 8/16 ms trừ thời gian chờ
> swapchain (máy ảo chờ vblank 20–30 ms/khung kích oan). Chi tiết số: §9 cuối. Chủ mở lại app là nhận bản 109110317; log về `D:\jx1_android_log`.

> **03:05 11/09 — `[VE 11/09]` ĐỢT 2 (a) ĐÃ VIẾT + DỰNG, CHƯA THỬ MÁY ẢO / FOLD 7.** Chủ: "làm 1 2 3 luôn cho tôi gắn log để đọc phân tích".
> Commit `6ac47732` (gộp `f30439b5` của phiên giao diện → `96fcd8b2` = `origin/mobile-0809` đã FF), chỉ `JX_ANDROID`, chỉ Represent3 +
> `android/du_lieu_ghi_de/config.ini`, bộ vá `android/va_nguon_android_ve1.py` (tái tạo y hệt diff). **(1)** nạp KHUNG sprite ở luồng nền theo
> ngân sách (`[Client] NapKhungNen=1 NapKhungMs=3 NapKhungTruoc=2 NapKhungApMs=3`) — chữa giật 56–157 ms khi đám đông tới (phiên y2);
> **(A)** đo từng bước trình chiếu `SubmitFrame` → `jx_rep3.log` `[VE]` / `[VE-GOP]` / `[VE-NAP]` mỗi 30 s + `[VE-GIAT]` cho khung > `VeGiatMs=20`.
> Mục **2** (cache giao diện) và **3** (gộp lệnh) chưa đổi mã: `[VE-GOP]` sẽ cho biết vì sao 1 993 quad thành 1 672 lệnh (y2) — quyết sau khi đọc số.
> APK dựng từ `96fcd8b2`: `app-debug.apk` versionCode 109110258, md5 `7412d350…`, 20 095 279 B; máy ảo đang bận bản 109110259 của phiên giao diện
> → thử máy ảo xong mới đẩy dt_v4 (đã nhắn phiên giao diện). Chi tiết: §9 cuối ("Đợt 2 (a)").

> **01:30 11/09 — SỬA "ĐÔNG LÀ TỤT FPS" ĐỢT 1 ĐÃ LÊN BỘ TẢI: bản `v` = `android/apk/jx1mobile-1209-donhip-v.apk`** (versionCode 109110129,
> md5 `f789946a…`, dựng từ `origin/mobile-0809 = 3267b755` = đăng nhập mới của phiên giao diện + `[DAN 11/09 + b + c+d]`). Gốc lỗi tìm được
> qua bộ đo `[DAN]`: với `AutoLog=1` (bộ tải đang bật) mỗi site `AUTOLOG_EVERY` là một `SDL_GetTicks()`, mỗi viên đạn mỗi tick ~60 lần
> (49 ô của `GetOffsetAxis` + ~12 site) ≈ 3 ms/tick ở 700 viên; sửa: macro so với mốc mỗi tick, và bộ lọc ô cuốn sang vùng kề bỏ qua ô trống
> trước `GetOffsetAxis`/`FindNpc` (máy ảo: 20,9 → 4,5 µs/viên, `FindNpc` 23 760 → 803/10 s, va chạm giữ nguyên). Tất cả chỉ `JX_ANDROID`,
> hành vi game y hệt, khoá `[Client] DanToiUu=0` để đối chứng. **01:38: bản `w` (109110137, md5 `dd55c22f…`) thay `v`** vì bộ gửi log Java
> chỉ chạy khi `[DoNhip] Bat=1` — `Bat=0` từ 00:46 nên các phiên r/u/v **không có log nào về** (điện thoại tải `v` lúc 01:33 mà không có thư
> mục phiên). `[DAN 11/09 e]`: khoá `[DoNhip] GuiLog=1` = chỉ gửi log, không chạy bài đo; dt_v4 đã đặt `GuiLog=1`, `Bat=0`. Chủ mở lại app →
> nhận `w`, chơi đông 3–5 phút → đọc `[DAN]` + fps (§9 bước 3). Chi tiết: §9.

> **00:55 11/09 — ĐO XONG SDL 3.2.30 (§9), ĐÃ ÁP KẾT QUẢ.** Phiên Fold 7 00:31–00:43 (bản `-d`) đủ 12 bước: pha 0 mặc định 97–116 khung/s
> không cần hint; nhịp PC không thua ở đâu → `[Client] PaintVsync=1 PaintSmooth=2` mặc định mobile; Android không chép swapchain mỗi khung;
> nấc 60 Hz = 59 khung/s đều ở 2,4 W; 2 khung bay bắt buộc (commit `80b3369d` [DONHIP 12/09 d], dịch thử Android 0 lỗi = `android/apk/
> jx1mobile-1209-donhip-e.apk`, không đẩy bộ tải). Bộ tải hiện là bản `r` của phiên giao diện (882059d7 + giao diện, `[DoNhip] Bat=0`);
> bản kế tiếp của họ dựng từ `origin/mobile-0809 = f4107553`. "Đông là tụt fps" = tick logic "đạn" 4–5 ms/tick trên luồng chính, không phải
> vẽ — xem §9, việc tiếp là đo sâu `[DAN]` (chờ chủ chốt). Chuỗi Windows không dựng lại: thay đổi C++ chỉ trong `#ifdef JX_ANDROID` /
> tệp chỉ-Android.

> **00:30 11/09 — BẢN `-c` HỎNG TRÊN FOLD 7, ĐÃ THAY BẰNG `android/apk/jx1mobile-1209-donhip-d.apk`** (versionCode 109110027, md5
> `9510ae82…`, máy chủ 8765 PID 277384). Lỗi `-c`: `SDLActivity.onCreate` so `nativeGetVersion()` (3.2.30) với hằng `SDL_*_VERSION` trong
> `SDLActivity.java` của gói (bản gốc 3.2.14) → `mBrokenLibraries`, hộp thoại lỗi, app đóng sau 1–4 s không chạy native (5 phiên 00:23 chỉ có
> `jx_thietbi.log`). Sửa (commit `f278ad79` [DONHIP 12/09 c]): chép `SDLActivity.java` + `SDLSurface.java` của 3.2.30 vào gói, CMake chốt
> `FATAL_ERROR` khi Java ≠ `JX_SDL3_VER`. Đã thử máy ảo: native chạy `(SDL 3.2.30)`, Vulkan + Represent3 lên, vào game. Trong lúc chủ kẹt,
> phiên giao diện đẩy tạm `-p` (3.2.14 vá, 109110015) lúc 00:27; `-d` cao hơn nên điện thoại tự lên. Bẫy thứ hai cùng đêm: chép tệp vào
> dt_v4 mà không khởi động lại máy chủ → manifest cũ → "2 tệp lỗi" (00:21, xem §8 cuối).

> **00:15 11/09 — APK `android/apk/jx1mobile-1209-donhip-c.apk` = SDL 3.2.30 + bảng pha mới ĐÃ LÊN dt_v4** (versionCode 109110010,
> md5 `206638db…`, 20,0 MB; máy chủ 8765 khởi động lại PID 284056 với đúng dòng lệnh cũ của phiên giao diện; config dt_v4 giữ nguyên:
> `[DoNhip] Bat=1 GiayMoiPha=60 Pha=0,1,2,3,4,5 LanLap=2` → 12 phút, pha 5 = "SDL cũ" đối chứng). Nhánh: `6838634e` [DONHIP 12/09 b]
> + gộp `origin/mobile-0809 = 838fdf36` → `b9d85851`. Chưa chạy thử trên máy ảo (đang có người chạy bản `-o`). Lần mở app kế tiếp trên
> Fold 7 tự cập nhật rồi đo; kiểm nhanh: `jx_android.log` ghi `(SDL 3.2.30)`, `[NHIP-BAT]` không còn "chua co hint", pha 0 phải ≈ 118–120
> khung/s **mà không cần hint**, pha 5 ≈ 45. Cách dựng + bẫy: §8 cuối.

> **Mới nhất 10/09 23:45 — LOG FOLD 7 ĐÃ VỀ ĐỦ 12 BƯỚC, M1 XÁC NHẬN, ĐÃ CHỌN CÁCH SỬA (xem §8).** Phiên 23:23 (APK `-b`): pha 0 = 35–61 khung/s
> vì SDL dựng lại swapchain **mỗi khung**; chỉ bỏ qua SUBOPTIMAL (pha 1) → **118–120 khung/s khoá cứng, cùng mức điện ~2,4 W**; nhịp PC (pha 2)
> đều thêm chút; 1 khung bay (pha 3–5) không ổn định → giữ 2; 60 Hz phải đo lại. Hai phiên 22:05/22:53 và đoạn 23:24–23:30 là chủ **ra nền**,
> không sập. **Cảnh báo:** APK `-m` (247beeea, phiên giao diện dựng ở `D:\GAMEDEVNEW_wt_mobile` 23:29, đã lên dt_v4 23:33) **không có bản vá SDL**
> — phiên Fold 7 23:41 ghi `chua co hint`, `suboptimal +0` → pha 1 vô hiệu, máy lại 28–45 khung/s. Quyết định: nâng SDL Android lên **3.2.30**
> (chỉ Android; gói VC Windows giữ 3.2.14), bước vá đếm chạy tự động lúc CMake để mọi cây giống nhau. **23:50: phiên giao diện đã thay
> APK bộ tải bằng `jx1mobile-1309-suagd-n`** (= 7702bab0 + SDL 3.2.14 đã vá DONHIP, md5 `f49c9386…`, versionCode 109102349, đã kiểm khớp
> apk.txt; `origin/mobile-0809 = bc18c53d`) — lần thử Fold 7 kế tiếp có cả vá SDL lẫn vùng an toàn theo camera. Worktree này = `b42f8110`
> + tệp này, đang sau `mobile-0809` 6 commit — **trước khi dựng phải gộp mobile-0809** (luật chủ 10/09).

> **Mới nhất (22:05):** phiên giao diện đã gộp bản này và FF `mobile-0809 = d9a72b72` ([SUAGD 13/09] a/b/c: khung "hai họ", HUD theo vùng an toàn,
> trình chỉnh giao diện cho người chơi, nút "Chỉnh giao diện" trong Cài đặt cạnh thanh FPS). Worktree này đã FF lên `d9a72b72`;
> **APK điện thoại = `android/apk/jx1mobile-1209-donhip-b.apk`** dựng từ đúng `d9a72b72` + vá SDL (md5 `be0f3dae…`), đã thay vào bộ tải
> và khởi động lại máy chủ 8765. Bộ tải cũng có `ui/uitoado_danhsach.ini`, `spr/ui3/uisuagd/nut_chinh_gd.spr`, `uioptions.ini` mới và các khoá
> config [KHUNG/SUAGD/ANTOAN 13/09] (giữ `[DoNhip] Bat=1`, `PaintLog=1`). APK `-a` (6edcb896) chỉ còn để đối chiếu.
> Khung vẽ Fold 7 nay là **1437x616** (hai họ) — đọc từ `[NHIP-BAT]`.

- Đã vá, đã dựng APK `android/apk/jx1mobile-1209-donhip-a.apk` (arm64 + x86_64, đã soi chuỗi mã mới trong `libmain.so`,
  `libRepresent3.so`, `libSDL3.so`), commit `e56a6760` (DONHIP) + commit FPS/THONGTIN (xem git log).
- Gộp `origin/main` (BHXS / QUAICHAN / TRANGTRI f) = `bd7bd40e`, không xung đột.
- **Thêm theo yêu cầu chủ 12/09 tối** ("có thanh chỉnh FPS ở cài đặt và hiện FPS – CPU – GPU – pin ở góc phải như game mobile khác,
  không phải PerfHud"): `android/va_nguon_android_donhip2.py` + `android/sinh_uioptions_fps.py` —
  - **Thanh FPS trong Cài đặt** (`UiOptions.h/.cpp`, chỉ `JX_ANDROID`): 6 nấc *Tự động (theo màn hình) / 30 / 45 / 60 / 90 / 120*,
    đặt vào hàng "Bộ phím tắt" (ẩn trên điện thoại). Áp ngay: `PaintFps` + xin tần số màn (`ANativeWindow_setFrameRate`);
    lưu `UserData\UiCommon.ini [Options] FpsMuc`, áp lại lúc mở game (`LoadSetting`). Ini: lớp ghi đè `ui/ui3/uioptions.ini` mục `[Fps]`.
  - **Thông tin góc phải-trên**: `60 FPS | CPU 23% | GPU 41% | Pin 87% 34C | 25 ms` (`JxPerfHudAndroid.cpp`, không phụ thuộc PerfHud),
    hiện khi đã vào thế giới, `[Client] ThongTinGoc=0` để tắt, kéo được trong Sửa giao diện (khoá `ThongTinGoc`). GPU % đọc sysfs
    (Adreno `kgsl`, Samsung `/sys/kernel/gpu`, Mali) — máy không cho đọc thì hiện `-`. **Mặc định đè lên bản đồ nhỏ** (góc phải-trên
    của JX1) — chủ kéo chỗ khác hoặc phiên sau đổi mặc định sang trái bản đồ nhỏ (`KUiMiniMap::GetSelf()->GetAbsolutePos`).
- Đã cài máy ảo (kiểm md5 bản đã cài), đã đưa lên bộ tải `D:\jx1_android_data_dt_v4` + bật máy chủ 8765 `--nhat-ky D:\jx1_android_log`.
- Chuỗi Windows: **0 lỗi cả hai** (x64: Engine/Core/Represent3/S3Client; SDL: đủ 6 bước, ra `GameSDL.exe`) sau khi chép thêm các thư mục
  git bỏ qua từ worktree trước: `Lib\release64`, `Sources\packages\Microsoft.DXSDK.D3DX.9.29.952.8\build\native\{debug,release}` (d3dx9.lib),
  `Sources\Core|Engine\vcpkg_installed` (nlohmann/json.hpp), `ThirdParty\SDL3-src`, `android\gradle-project\local.properties`.
- **Đã FF + push 21:38: `origin/mobile-0809 = 6edcb896`** (gan_ff, cây `D:\GAMEDEVNEW_wt_mobile` sạch). Phiên giao diện
  (`claude/mobile-ui-customization-analysis-202ae4`, đợt [SUAGD 13/09]/[KHUNG 13/09 HAIHO]/[ANTOAN 13/09], chưa commit lúc đó) sẽ merge
  `mobile-0809` rồi thêm nút "Chỉnh giao diện" vào cửa sổ Cài đặt trên bản `UiOptions.cpp` này — **đừng đổi UiOptions/uioptions.ini song song**.
  Lưu ý từ phiên đó: khung vẽ sắp theo "hai họ" (Fold 7 = 1437x616, máy ảo 1060x616; tắt `[Resolution] KhungHaiHo=0`) → đọc log đo nhịp
  theo `SCREEN_WIDTH/HEIGHT` trong `[NHIP-BAT]`, không giả định 1440x616.
- Máy ảo: phiên giao diện đang dùng (chạy kịch bản adb, bật Sửa giao diện) — **không chạm máy ảo khi họ đang chạy**; ảnh chụp 21:33 cho thấy dòng
  góc phải chạy: `57 FPS | CPU 16% | GPU - | Pin 100% | 26 ms` (GPU `-` vì LDPlayer không có sysfs), chưa chụp được cửa sổ Cài đặt.
- Thay đổi trong worktree (đã commit):

| Tệp | Việc |
|---|---|
| `android/va_sdl3_donhip.py` (mới) | vá bản SDL 3.2.14: đếm `VK_SUBOPTIMAL_KHR` + số lần dựng lại swapchain qua hint, hint `JX_BO_QUA_SUBOPTIMAL=1` = bản sửa `bca30aa` của SDL, bật/tắt lúc chạy. Mặc định giữ nguyên hành vi 3.2.14 |
| `android/va_nguon_android_donhip1.py` (mới) | vá 3 tệp C++ (TCVN3, latin-1, kiểm số byte cao, mỗi mốc khớp đúng 1 chỗ — đã soát bằng Grep): `S3Client.cpp`, `D3D9onGPUDev.cpp`, `JxPerfHudAndroid.cpp`. Tất cả trong `#ifdef JX_ANDROID` |
| `android/gradle-project/.../JxDoNhip.java` (mới) | log thiết bị + gửi log về máy chủ tải |
| `android/gradle-project/.../JxActivity.java` | móc `onCreate` / `onPause` / `onDestroy` (SDLActivity khai `protected`, khớp) |
| `android/may_chu_tai_du_lieu.py` | thêm `POST /nhatky` + tham số `--nhat-ky` (mặc định `D:\jx1_android_log`) |
| `android/du_lieu_ghi_de/config.ini` | mục `[DoNhip]`, **`Bat=0`** (tắt = không đổi gì) |

- Đã chép vào worktree (git bỏ qua, lấy từ worktree `full-wauto-mobile-analysis-f781df`): `android/gradle-project/local.properties`,
  `ThirdParty/SDL3-src/SDL3-3.2.14`, `Lib/release64sdl`.

## 1. Chủ quyết 12/09

1. **FPS không có mặc định cố định** — tự chọn theo cấu hình từng máy.
2. **Có số đo rồi mới chọn cách sửa**, ưu tiên cách tiện cho phát triển về sau (ví dụ nâng SDL thay vì vá tay).
3. **Log lấy từ Fold 7 qua mạng LAN**: dựng APK, đưa lên máy chủ tải 8765, chủ tải về test, log tự gửi về máy tính.

Bốn nghi vấn cần số đo trên máy thật (LDPlayer không lộ được):
- **M1** swapchain dựng lại mỗi khung: SDL 3.2.14 đặt `preTransform = IDENTITY` trên Android (`SDL_gpu_vulkan.c:4669`), màn ngoài Fold 7
  hướng gốc dọc + game khoá ngang → `VK_SUBOPTIMAL_KHR` mỗi lần trình khung → dựng lại + chờ GPU rỗi (`:10589`, `:9890`, `:9819`).
  SDL issue #12950, sửa ở commit `bca30aa` (09/10/2025, nhánh release-3.2.x).
- **M2** chép nguyên swapchain sang `m_pLastFrame` mỗi khung chỉ để chụp màn hình (`D3D9onGPUDev.cpp:1052-1069`); swapchain SDL không có `TRANSFER_SRC`.
- **Nhịp vẽ**: config Android không đặt `PaintVsync` / `PaintSmooth` → chạy `PaintSmooth=1` và (ở 60 Hz) đường lập lịch cũ, không phải `[NHIP a–e]` của PC.
- **Tần số màn** là ảnh chụp lúc mở app (`SDLSurface.java:142`), game không xin nhịp màn (`setFrameRate` không có).

## 2. Các pha đo (tự chạy khi `[DoNhip] Bat=1`)

Vào thế giới 10 s thì bắt đầu; mỗi pha `GiayMoiPha` giây (60), `LanLap` vòng (2) → ~12 phút; dòng chữ vàng giữa màn hình ghi pha đang chạy.
Mỗi pha **thêm** một thay đổi so với pha trước; hết vòng tự trả về cấu hình lúc mở app.

| Pha | Thay đổi | Trả lời câu hỏi |
|---|---|---|
| 0 | hiện tại | mốc so sánh; `dung lai` ≈ số khung ⇒ M1 có thật |
| 1 | + sửa SDL (bỏ qua SUBOPTIMAL) | M1 ăn bao nhiêu FPS / độ đều |
| 2 | + nhịp PC: `PaintVsync=1`, `PaintSmooth=2` | nhịp `[NHIP]` của PC có làm đều khung trên mobile không |
| 3 | + bỏ chép khung mỗi khung, 1 khung bay | M2 + độ trễ; có tụt FPS không |
| 4 | + xin màn 120 Hz (`ANativeWindow_setFrameRate`), `PaintFps=120` | máy có chạy thật 120 không, tốn bao nhiêu W / nhiệt |
| 5 | + xin màn 60 Hz, `PaintFps=60` | mốc 60 khoá nhịp để so với 4 |

## 3. Log: ra đâu, đọc thế nào

- Điện thoại ghi trong thư mục dữ liệu app: `jx_nhip.log` (C++), `jx_thietbi.log` (Java), cùng `jx_paint.log`, `jx_rep3.log`, `jx_android.log`.
- `JxDoNhip.java` gửi **phần mới** mỗi 10 s (và khi app ra nền) → máy chủ ghi `D:\jx1_android_log\<model>_<yyyyMMdd_HHmmss>\<tệp>`.
- `jx_nhip.log`: `[NHIP-BAT]` (cửa sổ, khung vẽ, tần số SDL báo, hướng gốc/hiện tại, cấu hình lúc mở app), `[NHIP]` mỗi 10 s,
  `[NHIP-PHA]` cả pha, `[NHIP-XIN]` kết quả xin tần số, `[NHIP-XONG]`. Mỗi dòng: cách khung vẽ p50/p95/p99/max + số khung trễ > 1,5 chu kỳ,
  cắt ngang nội suy, chờ swapchain TB/max, cách giữa hai lần trả swapchain (≈ nhịp màn), SUBOPTIMAL / dựng lại swapchain, cấu hình.
- `jx_thietbi.log`: `[THIETBI]` máy/SoC/Android, `[MANHINH]` mọi chế độ màn, `[MAU]` mỗi 5 s (tần số màn đang chạy, nhiệt, headroom,
  pin %, nhiệt độ pin, dòng, áp, công suất W), `[DOI]` khi màn đổi chế độ.
- Quyết định: M1 xác nhận → sửa bằng **nâng SDL trong nhánh 3.2.x** (gồm cả `7875654` dựng lại swapchain khi quay lại app và `e0050c3`
  sập khi trở về từ nền) — đúng ý chủ "tiện phát triển về sau"; pha 2 đều hơn pha 1 → bật nhịp PC cho mobile; pha 4/5 → chính sách FPS theo máy.

## 4. Việc đã làm (21:00–21:40) và còn lại

Đã: vá SDL + 2 bản vá C++ (kiểm mã hoá: byte cao giữ nguyên), dựng APK 2 ABI, cài máy ảo (md5 khớp, versionCode 109102125), bộ tải
`D:\jx1_android_data_dt_v4` có APK + `config.ini` (`ThongTinGoc=1`, `[DoNhip] Bat=1 GiayMoiPha=60 Pha=0,1,2,3,4,5 LanLap=2`) +
`ui/ui3/uioptions.ini`; máy chủ 8765 chạy (pid xem `Get-NetTCPConnection -LocalPort 8765`) với `--nhat-ky D:\jx1_android_log`
(`POST /nhatky` đã thử: 200, ghi đúng chỗ — thư mục thử `THU_thu` để lại); hai chuỗi Windows 0 lỗi; FF + push `mobile-0809`.

Còn lại:
1. Chủ mở app trên Fold 7 → tự cập nhật (apk.txt 109102125) → chơi ~12 phút chỗ đông (§5). Theo dõi `D:\jx1_android_log\<model>_<phiên>\`.
2. Đọc `jx_nhip.log` `[NHIP-PHA]` theo §3, chọn cách sửa (M1 → nâng SDL nhánh 3.2.x; nhịp PC; chính sách FPS theo máy). Ghi kết luận vào tệp này.
3. Sau test: `D:\jx1_android_data_dt_v4\config.ini` đặt `[DoNhip] Bat=0` (máy chủ tự sinh lại manifest khi khởi động lại).
4. Mặc định dòng góc phải đang đè bản đồ nhỏ → cân nhắc đổi `ThongTin_MacDinh` sang bên trái bản đồ nhỏ (`KUiMiniMap::GetSelf()->GetAbsolutePos`).
5. Phiên giao diện sẽ merge `mobile-0809` và thêm nút "Chỉnh giao diện" vào `UiOptions.cpp` — chờ họ FF, không sửa UiOptions song song.

## 5. Chủ test trên Fold 7

- Điện thoại cùng Wi-Fi với máy `10.0.0.140`.
- Mở app → cài bản mới → mở lại → vào game → ở chỗ đông (thành / Tống Kim), đi bằng cần và đánh bình thường khoảng 12 phút.
- Dòng chữ vàng giữa màn hình ghi pha đang chạy; muốn nhận xét cảm giác thì ghi lại "pha nào mượt / rung".
- Đừng thoát app giữa chừng. Xong khi dòng vàng báo `DO NHIP: xong`.

## 7. Phân tích log MÁY ẢO 12/09 tối (chủ test thanh FPS trên LDPlayer) — đọc từ `D:\jx1_android_data\*.log`

Nguồn: `jx_paint.log` (`[SUM]` mỗi 10 s: khung vẽ, cách khung min/TB/max, `spikes` = lượt vòng bơm ≥ 25 ms, `ve` = thời gian UiPaint
**gồm cả chờ vsync trong Present**, `passes` = số vòng bơm), `jx_rep3.log`, `jx_nhip.log` (`[NHIP-XIN]`), `UserData\UiCommon.ini`.
Không lấy được logcat (phiên bị chặn lệnh shell) → không có dòng `[FPS]`/`[THONGTIN]`. Máy ảo: LDPlayer 1040x604, **màn 60 Hz, present mode vsync**,
Android 9 → `ANativeWindow_setFrameRate` không có (mọi `[NHIP-XIN]` trả −9999) — mức FPS chỉ đổi `PaintFps`. Chủ kéo thanh FPS lúc 21:36, 21:42,
22:27 và **lưu mức 5 = 120** (`FpsMuc=5`). App khởi động lại ~10 lần trong buổi (phần lớn do kịch bản adb của phiên giao diện cài lại APK).

| Đoạn (10 s/mẫu) | Khung/10 s | Cách khung min/TB/max (ms) | spikes ≥25 ms/10 s | `ve` TB (ms) | Vòng bơm/10 s | Kẹp alpha |
|---|---|---|---|---|---|---|
| **Mức 60**, chủ chơi, trước khi chuyển (21:32→21:36, giây 11–212) | 573–600 | 2–7 / 16–17 / 30–56 (cú nạp 117) | 6–15 | **7–13** | **770–1170** (có ngủ) | 0–4 |
| **Mức 120**, cảnh đông (giây 92–402 một lần chạy 21:4x) | 578–597 | 2–6 / 16–17 / 24–52 | 0–33 | **16** | 597–656 (≈ số khung) | 0–5 |
| **Mức 120**, chủ chơi cảnh yên (22:10, giây 202–642) | 595–597 | 7–12 / 16 / 22–35 | 0–6 | 16 | 597–613 | 0–3 |
| Mức 120, lần chạy tự động 22:34 (giây 92–682) | 588–601 | 5–11 / 16 / 24–37; hai cú 528 và 268 ms (nạp map, tick 479 ms) | 0–18 | 16 | 590–630 | 0–5 |

Đọc số:
1. **Trên màn 60 Hz, mức 120 không cho thêm khung nào** (59,5–60 khung/s ở mọi mức; `jx_rep3.log` `fps TB 60–61`). Vsync trong `SDL_WaitAndAcquireGPUSwapchainTexture` chặn.
2. **Cái giá của mức 120 trên màn 60 Hz**: luồng chính bị khoá trong Present ≈ 16 ms mỗi khung (`ve` 16 vs 7–13; CPU vẽ thật chỉ 2–4 ms theo `[PDET] render`),
   vòng bơm không còn khoảng ngủ (passes ≈ số khung, lưới 1 ms). Trên điện thoại đây là **M4** (alpha nội suy tính trước khi chờ 16 ms) + tốn pin.
   → Máy 60 Hz nên để **Tự động** (= 60); mức 120 chỉ có nghĩa trên màn 120 Hz thật (Fold 7) — cần log điện thoại để kết luận.
3. **Cách khung không đều ở cả hai mức** (min 2–7 ms, max 25–50 ms quanh trung bình 16,7): cùng hiện tượng "rung" 11/09. Mức 60 do lưới 8 ms + lead 4 ms
   thỉnh thoảng vẽ hai khung sát nhau; mức 120 do vẽ ngay khi swapchain thả rồi chờ. Đó chính là thứ **pha 2** (PaintVsync=1 + PaintSmooth=2) đo trên
   điện thoại; máy ảo không thử được vì LDPlayer chỉ có vsync.
4. Tick logic TB 0–3 ms, `cross` 0–10 → không nghẽn logic. Cú giật lớn chỉ khi nạp map (528–697 ms) và thi thoảng 100–270 ms (nạp sprite mới) — việc
   "30 s đầu vào map" đã biết, không liên quan mức FPS.
5. `spikes` tăng ở mức 120 (0–33 so với 6–15) một phần là **giả tạo**: mỗi vòng bơm đã gồm 16 ms chờ vsync nên chỉ cần thêm 9 ms là vượt ngưỡng 25.
6. `jx_rep3.log`: RAM riêng 560 MB, cache texture 282–288/393 MB, GPU tex 303–310 MB, giải mã sprite 5–12 ms/30 s trên luồng vẽ, `anh_null` 72–120 nghìn/30 s
   (bảng NpcRes thiếu ảnh — đã biết). GPU % trên máy ảo = `-` (không có sysfs) — đúng như thiết kế.
7. Việc nhỏ nên làm sau: kéo thanh FPS sinh **hàng chục lần áp mức** trong 2 s (mỗi bước = một lần `JxNhip_DatMuc`, `g_SetLoopInterval`, ghi log);
   nên áp khi **nhả ngón** (WND_N_SCORLLBAR_POS_CHANGED cuối) — vô hại nhưng thừa. `LoadSetting` áp mức 2 lần lúc mở game (UiInit + UiShell:447) — vô hại.

## 8. KẾT QUẢ FOLD 7 (10/09 23:23–23:41, APK `-b` = d9a72b72 + vá SDL) — M1 XÁC NHẬN, ĐÃ CHỌN CÁCH SỬA

Nguồn: `D:\jx1_android_log\SM-F966U1_20260910_232309\jx_nhip.log` (12 bước × 60 s, 2 vòng; `[NHIP-XONG]` không về vì chủ mở lại app
lúc 23:41:30, 6 s trước khi xong) và `jx_thietbi.log` (điện, tần số màn theo bước). Máy: SM-F966U1 (SM8750, Android 16), màn ngoài
2520×1080 120 Hz, hướng gốc **dọc** (`huong goc 3 hien tai 1`), khung vẽ 1436×616, `luc mo app: PaintFps 120 vsync 0 smooth 1`.

Tám phiên 22:05–23:21 trước đó đều ngắn, **không phiên nào sập**: hai phiên 22:05/22:53 chủ ra nền sau ~60 s (Java vẫn ghi `[MAU]` 5 s một,
C++ ngừng vẽ vì SDL chặn vòng lặp khi app ẩn, POST trong nền bị chặn nhưng bộ gửi giữ vị trí và gửi bù khi quay lại — không mất dòng nào);
các phiên còn lại là mở/đóng ở màn đăng nhập (`JxPosixMain tra ve 0` ghi vào phiên kế vì tiến trình cũ thoát sau khi phiên mới đã mở).
Phiên 23:23 cũng ra nền 23:24:34–23:30:35 (`xoay=0` rồi `xoay=1`) nên bước 1 kéo dài 412 s; 11 bước sau đủ 60 s.

| Bước | Pha (cộng dồn) | khung/s | cách khung p50 / p95 / p99 (ms) | trễ >1,5T | cắt ngang | chờ swapchain TB (ms) | SDL dựng lại | màn | W |
|---|---|---|---|---|---|---|---|---|---|
| 1 | 0 hiện tại (50 s đầu, trước khi ra nền) | 46–61 | 18–22 / 22–25 / 23–27 | ≈ mọi khung | 0–2 | 13–17 | **mỗi khung** (2683/2683) | 120 | — |
| 2 | 1 +sửa SDL (bỏ qua SUBOPTIMAL) | **118,5** | 8,12 / 9,62 / 10,1 | 33 | 30 | 0,04 | +1 | 120 | 2,38 |
| 3 | 2 +nhịp PC (`PaintVsync=1 PaintSmooth=2`) | **119,4** | 8,12 / 9,38 / 9,88 | **9** | **0** | 0,04 | 0 | 120 | 2,35 |
| 4 | 3 +bỏ chép khung, 1 khung bay | 82,3 | 11,6 / 15,4 / 20,1 | 1413 | 2 | 6,0 | 0 | 120 | 1,93 |
| 5 | 4 +xin 120 Hz | 86,1 | 11,1 / 15,4 / 16,9 | 936 | 0 | 5,7 | 0 | 120 | 1,75 |
| 6 | 5 +xin 60 Hz, `PaintFps=60` | 36,1 | 23,9 / 50,1 / 73,9 | 987 | 16 | 9,2 | 0 | **60** | 1,69 |
| 7 | 0 (vòng 2) | 35,3 | 27,1 / 47,6 / 55,6 | 2090 | 8 | 13,1 | mỗi khung (2117/2118) | 120 | 2,37 |
| 8 | 1 | 107,1 | 8,38 / 16,9 / 22,1 | 684 | 21 | 0,50 | +1 | 120 | 4,94 (?) |
| 9 | 2 | 110,7 | 8,38 / 14,1 / 20,1 | 406 | 11 | 0,47 | 0 | 120 | 2,83 |
| 10 | 3 | **119,9** | 8,38 / 9,62 / 9,88 | **3** | 0 | 0,12 | 0 | 120 | 2,02 |
| 11 | 4 | 55,3 | 15,6 / 37,1 / 53,4 | 1917 | 12 | 6,7 | 0 | 120 | 1,76 |
| 12 | 5 (50 s) | 33–46 | 21–28 / 34–50 / 43–55 | ~190 /10 s | 1 | 8,7–9,7 | 0 | 60 | 1,90 |

(T = 8,33 ms ở `PaintFps=120`, 16,7 ms ở 60; "trễ >1,5T" = số khung cách nhau quá 1,5 chu kỳ trong 60 s; W = trung bình `[MAU]` p= của bước, không sạc.)

Đọc số:
1. **M1 là nút thắt duy nhất đáng kể.** Pha 0: SDL 3.2.14 dựng lại swapchain **mỗi khung** (2117 lần / 2118 khung) → luồng chính chờ 13–17 ms
   mỗi khung dù CPU vẽ 2–3 ms (`[PDET] render`) → 35–61 khung/s. Chỉ bỏ qua `VK_SUBOPTIMAL_KHR` sau `vkQueuePresentKHR` (đúng nội dung
   commit SDL `bca30aa`: bọc chỗ đặt `needsSwapchainRecreate` trong `#ifndef SDL_PLATFORM_ANDROID`) → **118–120 khung/s, chờ swapchain 0,04 ms,
   cùng mức điện (2,35–2,38 W so với 2,37 W)**. SUBOPTIMAL vẫn trả về mỗi khung (+7112) vì preTransform vẫn lệch hướng màn, nhưng không dựng
   lại nên vô hại (compositor Android tự xoay).
2. **Nhịp PC có ích thêm một chút**: vòng 1 trễ 9 so với 33, cắt ngang 0 so với 30; vòng 2: 406 so với 684. → bật `PaintVsync=1`,
   `PaintSmooth=2` trong lớp ghi đè Android sau khi sửa SDL (pha 2 = đúng đường `[NHIP a–e]` của PC).
3. **1 khung bay không ổn định**: cảnh nhẹ (bước 10) cho kết quả tốt nhất cả bài (119,9; trễ 3; 2,0 W) nhưng khi CPU nặng (bước 4, 11:
   `[SEC] world` 100–130 ms/s) tụt còn 82 → 55 khung/s, chờ swapchain 6–7 ms **mỗi khung** = CPU đứng chờ GPU. Giữ **2 khung bay** như PC.
   "Bỏ chép khung" (M2) chưa tách được khỏi "1 khung bay"; điện thấp hơn ~0,4 W ở các bước 4/5/10/11 gợi ý M2 đáng làm — cần pha riêng
   (chép 0, bay 2).
4. Xin 120 Hz: không thêm gì (màn đã 120 khi game vẽ 120; Samsung tự hạ 60 Hz khi game chậm — thấy `[DOI]` ở pha 0). Xin 60 Hz: màn
   **xuống 60 Hz thật** (`che_do id=10`, `setFrameRate` trả 0), điện 1,7–1,9 W, nhưng chỉ 33–46 khung/s vì dính "1 khung bay" + vsync
   (lỡ 16,7 ms là rơi 33 ms). Phải đo lại 60 Hz với 2 khung bay trước khi làm nấc "tiết kiệm pin" của thanh FPS.
5. Nhiệt: `nhiet=0` suốt 18 phút, headroom ≤ 0,85, pin 75 → 72 %. Bước 8 đo 4,94 W là ngoại lệ chưa giải thích (các bước cùng cấu hình ~2,4 W).
6. Bài đo tự chạy ổn: chuyển pha, đổi khung bay, xin tần số đều không sập. Thiếu sót: đồng hồ pha tính cả lúc ra nền (bước 1 = 412 s)
   → phiên sau dừng đồng hồ khi không có khung vẽ (hoặc bỏ qua khoảng > 2 s giữa hai khung).
7. **APK `-m` (247beeea, phiên giao diện dựng 23:29 ở `D:\GAMEDEVNEW_wt_mobile`, lên dt_v4 23:33) không có bản vá SDL** (`grep "DONHIP 12/09"`
   trong `SDL_gpu_vulkan.c` của cây đó = 0): phiên Fold 7 23:41 ghi `SDL dem: suboptimal 0 dung lai 0 (chua co hint)`, `SDL suboptimal +0`
   → pha 1 vô hiệu, máy lại 28–45 khung/s (rủi ro §6 đã thành thật). Đã nhắn phiên giao diện; họ trả lời 23:50: đã chạy
   `va_sdl3_donhip.py` trong cây của họ (grep = 3), dựng `jx1mobile-1309-suagd-n` (23:49) và **đã thay vào dt_v4 23:50** (md5 `f49c9386…`,
   versionCode 109102349, `libSDL3.so` arm64 có chuỗi `JX_BO_QUA_SUBOPTIMAL`; máy chủ 8765 PID 279172; cũng cài lên máy ảo); commit
   `bc18c53d` [SUAGD 13/09 i] và `BANGIAO_GIAODIEN_MOBILE_SUAGD_1309.md` §7.9 ghi quy tắc "cây mới phải vá trước khi dựng".
   `[DoNhip] Bat=1` vẫn giữ → lần mở app kế tiếp trên Fold 7 sẽ đo lại đủ 12 bước với bản `-n` (chủ đừng ra nền giữa chừng).

Đối chứng thêm (chủ chạy bản `-n`/`-o` = SDL 3.2.14 đã vá, 23:50–00:08, ba phiên `SM-F966U1_20260910_235058`, `_235554`,
`SM-F966U1_20260911_000602`): pha 0 = 38–61 khung/s (dựng lại mỗi khung), **pha 1 = 109–115 khung/s (p50 8,38)**, pha 2 = 115–118
(trễ 107–262 so với 208–485 của pha 1), pha 3 "1 khung bay" = 43–72 khung/s, pha 5 = 43 khung/s. Phiên 23:41 (bản `-m` chưa vá) mọi pha
đều 34–53 khung/s, `suboptimal +0` — đúng như cảnh báo mục 7. Kết luận §8 giữ nguyên.

Quyết định (theo chủ quyết #2 "tiện phát triển về sau"):
- **Sửa M1 bằng nâng SDL Android lên 3.2.30** — bản 3.2.x cuối (01/01/2026; nhánh `release-3.2.x` đang 3.2.31-dev), có `bca30aa` (vào từ
  3.2.26, 30/10/2025), "Fixed Android applications losing vsync when being resumed" (3.2.24) và "Fixed a crash on Android upon returning
  from the background when using the GPU API" (3.2.30). **Chỉ Android**: `android/CMakeLists.txt` `JX_SDL3_SRC` → `SDL3-3.2.30`,
  `android/tai_sdl3_src.ps1` nhận phiên bản riêng cho Android; gói VC `ThirdParty\SDL3` 3.2.14 của Windows (GameSDL.exe) giữ nguyên → PC không đổi.
  Giữ bộ đếm suboptimal / dựng lại để bài đo còn dùng được: `va_sdl3_donhip.py` sửa neo cho 3.2.30 và **chạy tự động lúc CMake configure**
  (idempotent) để mọi worktree dựng ra cùng một SDL; hint đảo chiều `JX_DUNG_LAI_SUBOPTIMAL=1` = hành vi 3.2.14 (chỉ để pha 0 đối chứng).
- Sau khi nâng: lớp ghi đè Android `PaintVsync=1`, `PaintSmooth=2`, 2 khung bay; bảng pha mới cho `[DoNhip]`: 0 = SDL mới, 1 = +nhịp PC,
  2 = +bỏ chép (bay 2), 3 = xin 60 Hz + `PaintFps=60` (bay 2), 4 = 1 khung bay (đối chứng); đồng hồ pha dừng khi ra nền.
- Trước mắt: APK `-m` trên dt_v4 phải dựng lại với SDL đã vá (`python android\va_sdl3_donhip.py <wt_mobile>\ThirdParty\SDL3-src\SDL3-3.2.14`
  rồi gradle) hoặc đặt `[DoNhip] Bat=0` để chủ khỏi đo vô ích; mọi lần dựng phải gộp `origin/mobile-0809` trước (luật chủ 10/09).

**Đã làm 00:15 11/09 (chủ chốt "nâng SDL Android lên 3.2.30 theo §8, dựng lại và đo với bảng pha mới")** — commit `6838634e`
[DONHIP 12/09 b] (chỉ Android, không tệp nào của chuỗi Windows đổi: `JxPerfHudAndroid.cpp` không nằm trong vcxproj nào):
- `android/CMakeLists.txt`: `JX_SDL3_VER 3.2.30` (biến thường, không CACHE để cây cũ không giữ đường 3.2.14), báo lỗi rõ khi chưa tải nguồn,
  **tự chạy `va_sdl3_donhip.py` lúc configure** (`find_package(Python3)` + `execute_process`, FATAL_ERROR nếu lỗi) → mọi cây dựng ra
  cùng một `libSDL3.so`. `android/tai_sdl3_src.ps1`: `-Ver` mặc định 3.2.30, không còn lấy phiên bản từ gói VC Windows.
- `android/va_sdl3_donhip.py` viết lại cho 3.2.30: đếm SUBOPTIMAL / dựng lại như cũ, hint **đảo chiều** `JX_DUNG_LAI_SUBOPTIMAL=1` = hành vi
  3.2.14 (chỉ để pha đối chứng), từ chối SDL < 3.2.26, ghi tệp nguyên tử (CMake hai ABI có thể gọi cùng lúc). Đường acquire của 3.2.30
  đã coi SUBOPTIMAL là thành công, không cần vá.
- `JxPerfHudAndroid.cpp` (qua `android/va_nguon_android_donhip3.py`, latin-1 + CRLF, byte cao không đổi): bảng pha 0 "SDL mới, nhịp cũ"
  (vsync 0, smooth 1) | 1 +nhịp PC | 2 +bỏ chép khung, 2 bay | 3 xin 60 Hz + PaintFps 60 (2 bay) | 4 "1 khung bay" đối chứng | 5 "SDL cũ"
  đối chứng (hint = 1; mặc định `Pha=0,1,2,3,4`, dt_v4 ghi rõ `0,1,2,3,4,5`); **đồng hồ pha dừng khi app ra nền** (hai vòng bơm cách
  > 2 s → dời mốc, ghi `[NHIP-NGHI]`). Lớp ghi đè `config.ini`: chú thích bảng mới, `Pha=0,1,2,3,4`, `Bat=0` giữ nguyên.
- Dựng: worktree này thiếu mọi thứ git bỏ qua → chép `Sources\Core|Engine\vcpkg_installed` từ wt_mobile (robocopy, mã 1 = OK), viết
  `local.properties`, junction `D:\GAMEDEVNEW_wt_sdl30`, tải `SDL3-3.2.30.tar.gz` (15,9 MB, GitHub release) vào `ThirdParty\SDL3-src`.
  **Bẫy:** gọi `gradlew.bat` tương đối từ công cụ shell của phiên báo "not recognized" dù cwd đúng — phải gọi đường tuyệt đối
  `D:\GAMEDEVNEW_wt_sdl30\android\gradle-project\gradlew.bat assembleDebug -p <thư mục đó>`; dựng hết 2 phút 37 giây (38 task).
  Kiểm APK bằng zipfile + regex: `libSDL3.so` hai ABI có `release-3.2.30-0-gf5e5f6588`, ba hint mới, không còn `JX_BO_QUA_SUBOPTIMAL`;
  `libmain.so` có "SDL moi, nhip cu", "SDL cu (doi chung)", `[NHIP-NGHI]`.
- Đọc log lần đo tới: pha 0 (3.2.30, không hint) phải ≈ pha 1 cũ (118–120 khung/s, chờ swapchain ~0,04 ms, `dung lai +0`); pha 5 phải ≈ 45
  (chứng minh hint đối chứng hoạt động); pha 3 (60 Hz, 2 bay) kỳ vọng ~60 khung/s đều ở ~1,7 W → nếu đúng, đó là nấc "tiết kiệm pin";
  pha 4 (1 bay) kỳ vọng tụt khi CPU nặng → chốt 2 bay; pha 1 so pha 0 → có bật `PaintVsync=1 PaintSmooth=2` mặc định cho mobile không;
  pha 2 so pha 1 → có bỏ chép khung (M2) không. Sau đó đặt `[DoNhip] Bat=0` trên dt_v4.

**Sự cố 00:21–00:30 và cách xử lý (ghi để khỏi lặp):**
1. *"Báo lỗi 2 tệp không cho update"* (00:21): phiên giao diện chép `ui/uitoado_macdinh*.ini` mới vào dt_v4 sau khi manifest sinh (00:15) →
   md5 lệch → launcher lặp tải hai tệp mỗi giây, không vào bước cài APK. Xử lý: khởi động lại `may_chu_tai_du_lieu.py` (manifest chỉ sinh lúc
   khởi động). **Quy tắc:** chép bất cứ gì vào dt_v4 xong phải khởi động lại máy chủ ngay. Gợi ý sau này: máy chủ tự băm lại khi mtime đổi.
2. *Bản `-c` (SDL 3.2.30) mở rồi tự đóng sau 1–4 s* (00:23): thứ tự launcher là kiểm APK **trước** đồng bộ dữ liệu, nên điện thoại đã cài
   109110010 rồi mới gặp lỗi 2 tệp; sau khi manifest sửa, JxActivity chạy nhưng `SDLActivity.onCreate` thấy `nativeGetVersion()` = 3.2.30 ≠
   hằng Java 3.2.14 → `mBrokenLibraries` → hộp thoại "An error occurred… reinstall", bấm OK là `onDestroy`; native main không chạy nên không có
   `jx_android.log`. Bảng JNI `RegisterNatives` 3.2.14 ↔ 3.2.30 giống nhau, chỉ khác Java (`getPreferredLocales`, clipboard, `onResolvePointerIcon`).
   Sửa: gói dùng nguyên `org/libsdl/app/*.java` của SDL3-3.2.30 (`diff -rq` = 0), CMake đọc hằng trong `SDLActivity.java` và chặn khi khác
   `JX_SDL3_VER`. **Quy tắc nâng SDL:** nguồn C + Java `android-project` phải cùng phiên bản; `JxActivity` chỉ dùng `createSDLSurface(Context)`
   (còn trong 3.2.30). Launcher **không tự hạ cấp** (`maMoi <= maDangCai` thì bỏ qua) → bản hỏng đã cài chỉ gỡ được bằng bản có versionCode cao hơn.
3. Thử máy ảo bằng adb (00:28): `am force-stop` → `adb install -r D:/…apk` (MSYS_NO_PATHCONV=1) → `am start -n vn.jx1.mobile/.TaiDuLieuActivity`
   → logcat có `Running main function SDL_main`, `[ANDROID] … (SDL 3.2.30)`, `goldfish_vulkan`, `[LOGIN] da nho du dang nhap -> vao thang game`.
   Máy ảo **tự đăng nhập tài khoản của chủ** → thử xong phải `am force-stop` ngay kẻo đá phiên trên điện thoại. `screencap`/`pull` với đường
   `/sdcard/...` cần `MSYS_NO_PATHCONV=1`, đích ghi bằng đường Windows `C:/...`.

## 9. KẾT QUẢ SDL 3.2.30 TRÊN FOLD 7 (00:31–00:43 11/09, APK `-d`) + VÌ SAO "ĐÔNG LÀ TỤT FPS"

Nguồn: `D:\jx1_android_log\SM-F966U1_20260911_003034` (12 bước × 60 s, không ra nền, `[NHIP-XONG]` 00:43:12; pin 62 → 58 %, headroom ≤ 0,92,
`nhiet=0`). `[NHIP-BAT]`: `suboptimal 3434, dung lai 2` trong 37 s đầu — bản 3.2.30 tự hết dựng lại swapchain, **không cần hint**.

| Bước | Pha | khung/s | p50 / p95 / p99 (ms) | trễ >1,5T | chờ sc (ms) | dựng lại | W | màn |
|---|---|---|---|---|---|---|---|---|
| 1 / 7 | 0 SDL mới, nhịp cũ (vsync 0, smooth 1) | 111,0 / 96,9 | 8,4 / 10,9 / 26,4 — 8,6 / 20,9 / 27,6 | 238 / 992 | 0,02 / 0,04 | 0 / 1 | 3,18 / 3,49 | 120 |
| 2 / 8 | 1 +nhịp PC (vsync 1, smooth 2) | 115,5 / 115,7 | 8,4 / 11,4 / 16,1 — 8,4 / 10,4 / 19,1 | 218 / 185 | 0,03 | 0 | 3,56 / 4,41 | 120 |
| 3 / 9 | 2 +bỏ chép khung, 2 bay | 114,7 / 110,9 | 8,4 / 11,4 / 17,1 — 8,4 / 13,4 / 20,9 | 243 / 463 | 0,03 | 0 | 2,73 / 4,33 | 120 |
| 4 / 10 | 3 +xin 60 Hz, PaintFps 60 | **59,0 / 59,3** | 16,6 / 20,1 / 26,9 — 16,6 / 20,9 / 27,6 | 57 / 61 | 0,05 | 0 | **2,35 / 2,46** | **60** |
| 5 / 11 | 4 "1 khung bay" (đối chứng) | 47,7 / 80,4 | 18,1 / 41,1 / 53,6 — 11,9 / 18,1 / 24,4 | 2332 / 1886 | 8,5 / 6,0 | 0 | 2,34 / 2,39 | 120 |
| 6 / 12 | 5 "SDL cũ" (hint, đối chứng) | 34,0 / 32,6 | 29,1 / 49,4 / 62,6 — 30,6 / 44,1 / 57,6 | 2034 / 1954 | 16,0 / 16,8 | 2041 / 1955 | 2,43 / 2,25 | 120 |

Kết luận (thay cho §8 "Đọc log lần tới"):
1. **Nâng SDL 3.2.30 giải quyết M1 ngay ở cấu hình mặc định**: 97–116 khung/s, chờ swapchain 0,02–0,04 ms; pha 5 (đối chứng bật hành vi cũ
   bằng hint) rơi về 33–34 khung/s, dựng lại mỗi khung — chứng minh khác biệt đúng là chỗ đó.
2. **Nhịp PC (PaintVsync=1, PaintSmooth=2)**: không thua ở bất cứ bước nào, p99 16–19 so với 26–28 ms, trễ 185–218 so với 238–992 →
   **đặt mặc định cho mobile** (lớp ghi đè). Điện đo cao hơn 0,4–0,9 W nhưng cảnh vòng 2 nặng hơn hẳn (tất cả các pha vòng 2 đều cao) nên chưa
   tách được; đo lại khi cần bằng cảnh tĩnh.
3. **Bỏ chép khung mỗi khung (M2)**: khung/s như nhau, điện thấp hơn 0,1–0,8 W → chỉ chép khi có yêu cầu chụp màn hình (chỉ Android).
4. **Nấc 60 Hz của thanh FPS** = chế độ tiết kiệm pin thật: 59 khung/s đều (p95 20 ms, 57–61 khung trễ/phút), 2,35–2,46 W so với 3,2–4,4 W ở 120,
   màn ở 60 Hz suốt. Không cần sửa mã.
5. **2 khung bay là bắt buộc**: 1 khung bay = CPU đứng chờ GPU 6–8,5 ms mỗi khung → 48–80 khung/s.
6. Đồng hồ pha mới đúng: không có `[NHIP-NGHI]`, 12 bước đúng 60 s.

**Vì sao "FPS giảm nhiều khi đông → lag"** (chủ hỏi 00:47; dòng thời gian 10 s một, ghép `jx_nhip.log` + `[SEC]/[SPIKE]/[WORLD b]` + `[REP3-NAP]`):
- **4 trong 12 phút là pha đối chứng cố ý xấu** (bước 5–6 lúc 00:35–00:37, bước 11–12 lúc 00:41–00:43: 28–80 khung/s, 140–312 cú giật/10 s).
  Ai chơi đúng lúc đó sẽ thấy lag dù đông hay không. Từ 00:46 `Bat=0`, không còn.
- Trong các pha thật, các cửa sổ tụt (t=67 s 95 khung/s; t=447–457 s 76 → 53; t=577 s 91; t=417/547 s 104–107) **trùng với tick logic
  thế giới nặng trên luồng chính**: `[SEC] world=110–151 ms mỗi giây, tickmax 10–15 ms` đúng lúc NPC/tick nhảy 70 → 134 (đám đông tới).
  Mỗi tick 10–15 ms chặn vòng vẽ 1–2 khung (chu kỳ 8,3 ms) × 18 tick/s → còn 55–75 khung/s. Chờ swapchain vẫn 0,03 ms → **không phải vẽ, không phải GPU**.
- Trong tick, NPC chỉ tốn 0,7–0,8 ms/tick (134 NPC, 5–6 µs/NPC). Phần nặng là **"đạn"** (`g_uKhacSo[1]/g_dKhacMs[1]` = kỹ năng/đạn bay):
  **687–787 viên/tick tốn 4,3–5,3 ms/tick** (≈ 80–95 ms mỗi giây), kéo theo truy vấn không gian: `cay: duyet2 2,4–3,7 triệu / 10 s`
  (≈ 13–20 nghìn lần duyệt cây mỗi tick), `vung: hoi 425–441 nghìn / 10 s`, `quet_vung max 9–15 ms`. Cảnh yên (t=589 s): đạn 22/tick 0,5 ms,
  duyệt 155 nghìn/10 s → 119 khung/s. Tức **lag lúc đông = số đạn/kỹ năng đang bay × cách mỗi viên tìm mục tiêu trên luồng chính**.
- Phút đầu vào map còn thêm **nạp sprite trên luồng vẽ**: `[REP3-NAP]` 30 s đầu 415 tệp spr 254 ms, 24.500 lần giải mã, 7 khung nạp > 16 ms,
  nặng nhất 106 ms/khung; `NAPNPC` "trễ 437" (ảnh NPC chưa kịp) — các cú giật `paint=55–71 ms` ở t=60–62 s là chỗ này (việc "30 s đầu" đã biết).
- So với ghi chú 11/09 (LDPlayer, Tống Kim, mục tiêu 60 khung/s, "tick < 1,1 ms"): trên Fold 7 mục tiêu 120 khung/s (8,3 ms) nên tick 10–15 ms
  giờ lộ thành tụt khung; trước đây ở 16,7 ms/khung nó "vừa lọt".

Chủ chốt 01:00 11/09: *"cứ đối chiếu log để fix làm sao không tụt FPS khi đông người chơi mà không giảm trải nghiệm"* → làm theo thứ tự đo → sửa có
công tắc → chủ thử (mọi thứ chỉ `JX_ANDROID`):
- **Bước 1 (đã làm 01:15, commit [DAN 11/09])**: bộ đo `[DAN]` — `android/va_nguon_android_dan1.py` vá `KMissle.cpp` + `KSubWorldSet.cpp`
  (neo lấy nguyên văn từ tệp, khớp trên bản LF vì tệp CRLF). Bucket "đạn" thật ra là vòng `KRegion::Activate` gọi `Missle[i].Activate()`
  cho `m_MissleList` của 9 vùng (`KRegion.cpp:965`). `[DAN]` in mỗi 10 s cạnh `[WORLD b]` khi `PaintLog>0`: số viên/tick, µs/viên, thời gian
  `OnFly` (trong đó `TestBarrier` × n, `CheckBeyondRegion`, `CheckCollision` × n với số `FindNpc` và số va chạm), `MoveObject` (cây Ipot),
  phần còn lại (log/IsMatch/Map2Mps). Chi phí đo ~0,1 ms/tick ở 700 viên. Đã loại trừ trước khi đo: `AUTOLOG_EVERY` (chỉ `timeGetTime()`),
  `g_AutoLog` (tệp mở sẵn, ≤ 1200 dòng/s, ~500 dòng/10 s), `S5LogScan` (chỉ server), `GetRelation` (tra bảng), đạn không làm rơi chỉ mục
  NPC (`m_pMslRef`), `MAX_MISSLE` 3000 ô quét/tick. Lưu ý: bộ tải điện thoại đang `AutoLog=1 PaintLog=1` (lớp ghi đè mặc định 0).
- **Bước 2 (đã làm 01:25, commit [DAN 11/09 b], `android/va_nguon_android_dan2.py`)** — `[DAN]` trên máy ảo (cảnh yên, 3,5 viên/tick) đã lộ
  cấu trúc chi phí: mỗi `CheckCollision` gọi **~55 lần `FindNpc`** (quét ô 7×7 quanh đạn, `CollideRange` 3), mỗi ô đi qua `GetOffsetAxis`
  mà hàm này mở đầu bằng `AUTOLOG_EVERY`; với `[Client] AutoLog=1` (bộ tải điện thoại **đang bật**, lớp ghi đè mặc định 0) mỗi site
  `AUTOLOG_EVERY` = một `timeGetTime()` = `SDL_GetTicks()` (PLT + `clock_gettime`, ~50–80 ns). Mỗi viên mỗi tick ≈ 49 ô + ~12 site khác
  ≈ 60 lần lấy giờ → 700 viên × 60 × ~70 ns ≈ **3 ms/tick** — đúng cỡ 4–5 ms đo được. Hai sửa, chỉ `JX_ANDROID`, **kết quả game y hệt**:
  (B) `KCore.h`: `AUTOLOG_EVERY`/`AUTOLOG_IDX_EVERY` so với `g_uAutoLogNow` (cập nhật mỗi tick ở `KSubWorldSet::MainLoop` và mỗi lần
  `g_AutoLog` ghi) thay vì gọi giờ ở mọi site — nhịp ghi log lệch tối đa 1 tick so với chu kỳ ≥ 500 ms. (C) `KMissle.cpp`: ba vòng quét ô
  (`CheckCollision`, `CheckNearestCollision`, `ProcessCollision`) bỏ qua ô **trong vùng** không có NPC (`KRegion::JxSoNpcO` đọc bộ đếm
  `m_pNpcRef`, cùng dữ liệu `FindNpc` dùng để thoát sớm) trước khi gọi `GetOffsetAxis`/`FindNpc`; ô ở biên vùng đi đường cũ. Khoá
  `[Client] DanToiUu=0` tắt (C) để đối chứng. `[DAN]` thêm `boqua` (số ô bỏ qua) và `vacham` (ms/tick trong ProcessCollision + DoCollision).
  Ngoài ra máy ảo cho thấy `hit` = `col` (432/432): đạn không phải kiểu bay thẳng thì `DoCollision` (hiệu ứng) lặp mỗi tick khi mục tiêu
  trong tầm — hành vi gốc của kỹ năng liên tục, **không sửa** (giữ hiệu ứng).
- **Bước 2b (01:30, [DAN 11/09 c + d])**: bộ đếm tách theo vòng (`o C1 a/b/c C2 … C3 …` = bỏ qua ô trống / bỏ qua không có vùng đích /
  có NPC) cho thấy ở vòng C1 (7×7 quanh đạn) **80% ô nằm ngoài vùng hiện tại** (vùng chỉ 16 ô, cửa sổ chờm sang vùng kề) nên bộ lọc
  "chỉ trong vùng" bỏ sót; C3 thì lọc tốt. Bộ lọc mới `JxDanBoQuaO(vòng, subworld, region, x, y)` **tự cuốn sang vùng kề y như
  `GetOffsetAxis`** (x trước, y sau, `m_nConnectRegion` 2/6/4/0; không có vùng đích → bỏ qua = `GetOffsetAxis` trả FALSE) rồi đọc bộ đếm NPC
  của vùng đích; chỉ ô có NPC mới đi đường gốc. Máy ảo: 20,9 → 4,0–4,7 µs/viên đã đo với bộ lọc cũ; bộ lọc mới đo lại ở bản `-i`.
- **Bước 3 — KẾT QUẢ TRÊN FOLD 7 (01:44–01:55 11/09, bản `x`, phiên `SM-F966U1_20260911_014409`, 11 phút, 65 cửa sổ 10 s)**: sau 40 s nạp
  map, **fps 101–120 ở mọi cửa sổ** (trung bình 116); các cửa sổ đông nhất: **677 viên đạn/tick + 154 NPC/tick → 101 fps, đạn 1,60 ms/tick
  (2,4 µs/viên)**; 570/tick → 105 fps; 539/tick → 102; 428/tick + 135 NPC → 119. Trước sửa (00:31): 690–790 viên/tick + 134 NPC → 53–76 fps,
  đạn 4,3–5,3 ms/tick (6,5 µs/viên), `world` 110–151 ms mỗi giây. Nay **không còn giây nào `world ≥ 50 ms`** (tổng lớn nhất 52 ms lúc nạp map).
  `[DAN]` ở 677 viên/tick (2,3 µs/viên, tổng 1,54 ms/tick): `move` (cây Ipot) 0,76 ≈ 50 %, `col` 0,40 (trong đó `vacham` 0,17 = kỹ năng
  liên tục đánh mỗi tick), `khac` 0,21; ở 539–570 viên/tick cũng 1,0–1,3 ms/tick, `move` 0,49–0,64; vòng C1 bỏ qua 85–90 % ô. Phần còn lại là hành vi gốc → **đợt 1 đủ**, đợt 2 (nếu muốn thêm dư địa) = cách cây
  cảnh cập nhật vị trí đạn (`KScenePlaceC::MoveObject`: khoá tới hạn + `DoiViTri` mỗi viên mỗi tick).
  **Nhiệt/pin (đáng lưu ý):** cả 130 mẫu `nhiet=2` (Android THERMAL_STATUS_MODERATE, đã nóng sẵn từ phiên trước), headroom 0,86–0,99 (1,0 = giảm xung), 3,57 W trung bình
  (đỉnh 11,5 W), pin 39 → 35 % trong 11 phút, màn giữ 120 Hz 129/130 mẫu. Chơi lâu ở 120 khung/s sẽ tới ngưỡng giảm xung → nấc **60** của
  thanh FPS (59 khung/s đều, 2,4 W, §9 mục 4) là chế độ chơi dài; có thể làm "tự hạ 60 khi headroom > 0,9" nếu chủ muốn.
  Chú ý: bộ gửi log chỉ chạy khi `Bat=1` hoặc (từ bản `w`) `GuiLog=1`; bản `x` thêm `ScriptError.log`. Chuỗi Windows không dựng lại: mọi
  dòng C++ mới nằm trong `#ifdef JX_ANDROID`, nhánh `#else` giữ nguyên văn.
- **Phiên `y2` 02:07 (bản của phiên giao diện = 3a602219, cùng mã đạn)**: sau nạp map 116–119 fps với 155 NPC + 204–350 viên/tick. Hai cửa
  sổ tụt (94 và 83 fps, t=61 s và 81 s) trùng lúc đám đông xuất hiện (NPC 34 → 154 trong 30 s): `[SPIKE]` là **phía vẽ** (logic 0–10 ms,
  paint 56–157 ms; `[PDET] render` 18–47 ms + `end` 40–71 ms) do **nạp ảnh NPC trên luồng vẽ**: `[REP3-NAP]` rút khung 30 136 lần/196 ms,
  giải mã 18 420 lần/128 ms mỗi 30 s, 5–7 khung nạp > 16 ms, nặng nhất 75 ms/khung; `NAPNPC` "trễ 239–279" (ảnh cần mà chưa kịp). Chỉ 1–2 giây
  có `world` 92 ms (sinh NPC hàng loạt). → Đây là việc "30 s đầu vào map / đám đông mới tới" đã ghi ở [[mobile-tongkim-lag-goc]], **không phải
  đạn**. Đợt 2 (nếu chủ muốn), theo thứ tự lợi ích: (a) nạp/giải mã ảnh NPC theo ngân sách mỗi khung hoặc nạp trước khi NPC vào tầm nhìn
  (luồng vẽ hết giật 60–150 ms khi đám đông tới); (b) `KScenePlaceC::MoveObject` cho đạn (≈ 50 % chi phí đạn còn lại). Đấu giá/Chiến Lệnh:
  jx_mail.log phiên này ghi `protocol_def_c.lua ... ok`, `dispatch (75, 1) loi=0`, không có `ScriptError.log` → vá Include-từ-pak của phiên
  giao diện chạy đúng.
- **Đợt 2 (a) — `[VE 11/09]` (03:05, commit `6ac47732`, bộ vá `android/va_nguon_android_ve1.py`, chỉ `JX_ANDROID`, Windows giữ nguyên từng dòng):**
  - **Nạp khung nền.** `TextureResSpr::PrepareFrameData` khi đang vẽ (`m_bVeDangDien`) mà tổng nạp đồng bộ của khung (`g_dRep3NapKhung`) đã
    ≥ `NapKhungMs` → giao khung cho luồng nền sẵn có của `[NAP 08/09 b]` (ưu tiên sau hàng "ảnh đang vẽ cần", trước hàng "nạp trước"): luồng
    nền `SprGetFrame` (pak có khoá riêng) + giải mã RLE vào bộ đệm (`JxGiaiMaNen`, chỉ đọc header / offset / bảng màu bất biến; spr không nén
    theo khung thì đọc `pRawData` cố định từ lúc nạp); luồng vẽ ở `RepresentBegin` tạo texture từ kết quả (`JxNapKhungNhan`, tối đa `NapKhungApMs`
    ms/khung, phần dư để khung sau; ghi thẳng vào texture `POOL_DEFAULT` vì lớp SDL_GPU khoá được → bớt 1 cấp phát + 1 chép so với SYSTEMMEM +
    `UpdateTexture`). Hết ngân sách mà khung đang chờ luồng nền (hoặc vừa giao được) thì bỏ vẽ khung đó một khung (như bỏ vẽ cả sprite của
    NAP 08/09 b), `Rep3AnhNullGhi` không tính là ảnh thiếu; còn ngân sách thì nạp đồng bộ luôn kể cả khi luồng nền đang làm khung đó (kết quả
    về sau bị bỏ, đếm vào "bỏ" của `[VE-NAP]`). Cả hai nhánh (đồng bộ còn ngân sách / giao nền) đều nạp trước `NapKhungTruoc` khung kế tiếp cùng hướng (khung = hướng × số khung mỗi
    hướng + chỉ số, xoay vòng trong hướng) để NPC đã hiện không nháy khi đổi khung. Vòng đời: `TextureResSpr::Release` → `JxNapKhungHuy` (bỏ
    việc chưa chạy, CHỜ việc đang chạy vài ms, bỏ kết quả); `NapNenDung` dọn hàng; luồng nền không bao giờ giữ `m_ImageProcessLock` nên không
    kẹt. `NapKhungNen=0` = như cũ. Hỏi kích thước / alpha từ logic (`m_bVeDangDien=false`) vẫn nạp đồng bộ như cũ — `[VE-NAP]` đếm riêng
    "nạp đồng bộ NGOÀI lúc vẽ" để biết phần này có đáng làm tiếp không.
  - **Đo trình chiếu** (`CDevGpu::SubmitFrame`, `D3D9onGPUDev.cpp`): chờ lệnh + swapchain / chép lên GPU (số texture tải, KB, ring KB) / ghi lệnh
    render pass (lệnh, quad, đỉnh, pass, đổi pipeline / texture-sampler / uniform vs / ps / cắt-viewport) / nộp; `JxGopVo` đếm lý do quad không
    gộp vào lệnh trước theo thứ tự kiểm: stride, không liên tiếp trong ring, pipeline, texture0, texture1/sampler, vs, ps, cắt.
  - **Đọc log** (`jx_rep3.log`, bộ gửi đã có tệp này): mỗi 30 s `[VE]` (TB/max từng bước, khung SubmitFrame > 8 / > 16 ms, vẽ CPU Begin→End),
    `[VE-GOP]` (đổi trạng thái mỗi khung + lý do không gộp cả kỳ), `[VE-NAP]` (giao / xong / hỏng / bỏ, bỏ vẽ, đồng bộ trong ngân sách, hàng chờ
    max, trễ giao→áp, luồng nền bận ms, áp trên luồng vẽ, nạp đồng bộ NGOÀI lúc vẽ); mỗi khung chậm `[VE-GIAT]` (tối đa 12 dòng / 10 s):
    tổng = vẽ CPU (nạp tách tệp spr / rút khung / giải mã / tạo GPU / ngoài vẽ) + trình chiếu (chờ / chép / ghi / nộp). **Kiểm trên Fold 7:** vào
    chỗ đông; kỳ vọng `[REP3-NAP]` "khung có nạp > 16 ms" ≈ 0 và `[SPIKE] paint` không còn 56–157 ms; `[VE-NAP]` bỏ vẽ vài trăm lượt / 30 s
    là bình thường (mỗi lượt = một khung ảnh trễ 8–16 ms); nếu thấy NPC nháy → tăng `NapKhungMs` (4–5) hoặc `NapKhungTruoc` (3).
  - **Mục 2 (cache giao diện) và 3 (gộp lệnh): chưa đổi mã.** Số y2: 1 993 quad → 1 672 lệnh / khung (gộp 1,2 quad / lệnh); `[VE-GOP]` sẽ nói vì
    sao (dự đoán: đổi trang atlas ở texture0 — 203 trang 1024², mỗi sprite một chỗ). Nếu đúng thì việc tiếp là trang atlas lớn hơn / gom các
    khung cùng sprite vào một trang, lợi hơn cache giao diện (UI chỉ 341 / ~2 500 đơn vị vẽ mỗi khung; cache ở GPU không bớt được phần CPU sinh
    lệnh, mà phần GPU của 341 quad trên Adreno là không đáng kể). Quyết sau khi có `[VE]` / `[VE-GOP]` của Fold 7.
  - **Kết quả máy ảo 03:18–03:20 (bản `b` 109110305, LDPlayer 60 Hz, `PaintFps=60 smooth=1 vsync=0` của config máy ảo, Khoả Lang Động):**
    `[VE-NAP]` 4 cửa sổ 30 s: giao 514 / 1 207 / 1 457 / 522 (trong đó nạp trước 347 / 1 181 / 1 452 / 518), xong 475 / 1 243 / 1 457 / 522,
    hỏng 0, bỏ 3 / 0 / 0 / 0; bỏ vẽ 407 → 26 → 5 → 4 lượt; đồng bộ trong ngân sách 107 / 618 / 814 / 286; hàng chờ max 92 → 12; trễ giao→áp
    TB 48,8 ms (max 128) lúc dồn, sau 15–17 ms (= 1 khung 60 Hz); luồng nền bận 37 / 39 / 55 / 18 ms mỗi 30 s; áp trên luồng vẽ 478–1 457
    khung mất 1,8–3,3 ms mỗi 30 s (max 0,42 ms/khung). `[REP3-NAP]`: giải mã đồng bộ chỉ 2,6–2,8 ms/30 s, "khung có nạp > 16 ms" còn 1 ở hai
    cửa sổ đầu (max 19,2 / 29,0 ms) — do **nạp đồng bộ NGOÀI lúc vẽ** (`[VE-NAP]` "253 lần 199,7 ms", tệp spr 26 lần 46 ms): logic gọi
    `GetImageParam` khi NPC xuất hiện → `LoadSprFile` ngay trên luồng chính. → **Đợt 2b:** trả tham số từ header đã nạp sẵn / nạp trước khi
    `SetSprFile` (đã có `NAPNPC`), không nạp cả sprite đồng bộ.
    `[VE]` (máy ảo, CPU x86 nhanh): chờ lệnh + swapchain TB 7,7–9,3 ms (vblank vì `vsync=0` phía client), chép lên GPU TB 0,02–0,04 ms (max 8,1
    lúc tải 26 MB texture một khung), ghi lệnh TB 0,03–0,11 ms cho 230–270 lệnh / 313–365 quad, nộp TB 0,3–0,5 ms (max 11,5). `[VE-GOP]`
    mỗi khung: đổi pipeline 58–70, đổi texture/sampler 161–184, đổi ps 121–142; quad không gộp cả kỳ: **texture0 188 690–219 642, pipeline
    49 989–60 005, ps 12 903–16 495**, không liên tiếp 1 746–1 770 (= 1/khung, lệnh đầu sau clear), stride/vs/cắt 0. **Kết luận mục 3:**
    (1) trang atlas chia theo (chiều cao bin, định dạng) nên khung cùng sprite/cùng NPC rơi vào trang khác nhau → đổi texture0 liên tục; sửa =
    một trang nhiều hàng bin (shelf) hoặc trang 2048–4096² để hầu hết sprite NPC nằm cùng trang; (2) 58–70 lần đổi pipeline/khung = quad xen
    kẽ chế độ blend khác nhau (không gộp được trừ khi sắp xếp); (3) ps 121–142 = đổi trạng thái tầng texture (COLOROP/ALPHAOP/alphatest) giữa
    các quad. Trên Fold 7 (đám đông 1 300 đơn vị NPC/khung) tỷ lệ texture0 sẽ còn cao hơn — đọc `[VE-GOP]` phiên kế rồi làm (1).
  - **Kết quả Fold 7 09:31–10:05 (bản 109110317 = `[VE b+c]`, SM-F966U1, 120 Hz, 33 phút, 172 cửa sổ 10 s; đọc bằng
    `scratchpad/bang_ve.py`):**
    - *Mục 1:* `[VE-NAP]` cả phiên giao 70 297 (nạp trước ≈ 95 %) → xong 69 534, hỏng 625 (0,9 %, xem "rỗng" dưới), bỏ vẽ 6 679 lượt; cửa sổ
      đông nhất t=1197 s giao 9 383/30 s, t=1317 s 10 296/30 s: luồng nền bận 360–452 ms/30 s, áp texture 19–20 ms/30 s (max 2 ms/khung), trễ
      giao→áp TB 12–17 ms. `[REP3-NAP]` "khung có nạp > 16 ms" 49 lần/33 phút (y2: 23 lần/8 phút); `[SPIKE] paint ≥ 40 ms` 131 lần = 4,6/phút
      (y2 85 lần = 13,1/phút); khung/10 s TB 1 157 (y2 1 100); các cửa sổ đông nhất 989–1 085 khung/10 s (y2 831–946). Nhiệt `nhiet=0`
      suốt, headroom 0,61–0,78, 1,9–3,4 W, pin 100 → 94 %.
    - *Nghẽn (a) — chép lên GPU:* trong 326 `[VE-GIAT]` phần lớn nhất là "chép" ở 203 dòng (vẽ khác 104, nạp 27, nộp 4); chép ≥ 15 ms ở 179
      khung, 134 khung ấy tải < 500 KB, có khung "tải 0 tex 0 KB" mà chép 45–60 ms. Hai khung 149 và 199 ms trùng lúc tạo ~15 trang atlas
      (`QueueZeroUpload` memset 2 MB/trang vào staging → 30 881 KB một khung, `m_texXferSize` phình tới 32 MB). Đọc SDL 3.2.30
      (`SDL_gpu_vulkan.c`): transfer buffer map cố định (không flush), nhưng `VULKAN_INTERNAL_RemoveMemoryUsedRegion` **vkFreeMemory khi khối
      hết vùng dùng** và cấp lại khối 16 MB (`SMALL_ALLOCATION_SIZE`) khi tạo mới → bảng màu tạo/huỷ transfer buffer mỗi khung có sprite mới =
      cấp/giải phóng 16 MB mỗi khung; buffer > 2 MiB dùng khối 64 MiB (`LARGE_ALLOCATION_INCREMENT`) → transfer 32 MB cycle = nhiều khối 64 MB.
      → `[VE 11/09 d]`: bảng màu đi chung staging + transfer buffer cố định; vùng 0 (trang mới, ô chưa có bản CPU) tải từ **một bộ đệm 0 cố
      định** (memset một lần), staging không phình; đồng hồ con `chep: bang mau / tex map+chep / lenh tai / zero / ring / xfer KB (phinh)`.
    - *Nghẽn (b) — `[VE-GOP]`:* cả phiên quad không gộp: texture0 86,1 %, pipeline 11,4 %, ps 2,3 %, không liên tiếp 0,2 %; lúc đông TB 1 765
      lệnh / 1 607 quad mỗi khung, đổi texture/sampler TB 1 153/khung (max 3 835), đổi pipeline 190–197, ps 726–854 → ghi lệnh 2,3–2,8 ms +
      nộp ~1 ms mỗi khung. Trang atlas hiện chia theo (bin cao, định dạng) → khung cùng NPC nằm rải rác. Việc kế `[VE 11/09 e]`: trang xếp kệ
      theo định dạng (một trang nhiều hàng bin, `CAtlasMgrGpu::Alloc/Free/NewPage`), tuỳ chọn trang 2048², công tắc `[Client] Rep3AtlasKe`.
    - *Còn lại "nạp ngoài lúc vẽ":* `GetImageParam` từ `KNpcRes` khi NPC đổi ảnh: 448 lần/282 ms (t=181 s), 294 lần/251 ms (t=1197 s), một lần
      tới 33 ms; `[NAPNPC]` "lần dùng đầu: kịp 1 118, trễ 1 213" = nửa số lần hỏi rơi vào lúc luồng nền đang nạp mà vẫn nạp đồng bộ đè lên
      (`GetImage` nhánh `m_bDangNap` + `m_bVeDangDien=false`). → `[VE 11/09 d]`: sprite `spr\npcres` đang nạp nền → trả "chưa có", KNpcRes giữ
      `m_bChange` hỏi lại khung sau (`[Client] NapHoiKhongDe=1`); đếm `hoi NPC dang nap -> chua co` trong `[VE-NAP]`.
    - *"hỏng" 625:* xảy ra theo cụm lúc đông (119/86/30/64/102/99 mỗi 30 s), `tao_hong` = 0 → là khung rỗng (w/h = 0) hoặc rút khung hỏng; trước
      đây giao lại mỗi lần vẽ → `[VE 11/09 d]` đánh dấu `nJxNen = 2`, không giao lại, nhánh đồng bộ giữ raw nên lần sau rẻ; đếm `khung rong`.
    - *"vẽ khác" 104 dòng* (vẽ CPU 40–90 ms không do nạp, `[PDET] render 51–86`): mã vẽ của client (KScenePlaceC/UI) — ngoài phạm vi đợt này.
  - **Phiên 10:22 (bản 109110954 = `[VE b+c]`, đọc lúc 10:30 khi chủ đang chơi, 7,5 phút, cảnh đông hơn: 300–500 đơn vị NPC/khung, đạn tới
    1 091 viên/tick):** fps 117–120 khi thường, **53–96 fps trong 50 s đầu vào đám đông** (t=61–111 s), 43–69 fps ở t=211–221 s; `nhiet` 0 → 1
    sau 6 phút, 2,3–3,2 W. Mục 1: giao 39 381 → xong 39 355, hỏng 14; đông nhất 10 430 khung/30 s, luồng nền bận 368–662 ms/30 s, bỏ vẽ
    831–859 lượt/30 s (≈ 0,3/khung). Chép: hai khung 130 / 141 ms lúc tạo trang atlas (96 tex 27 MB, 31 tex 17 MB); 61 khung chép ≥ 15 ms,
    38 khung tải < 500 KB. `tep spr` đồng bộ 196–233 lần/208–360 ms mỗi 30 s, max **37,8 ms**; "ngoài lúc vẽ" 344–399 lần/215–370 ms.
    `[VE-GOP]` texture0 **88,9 %**, lúc đông 1 145–1 358 lệnh/khung, đổi texture TB 498/khung (max 4 477) → ghi lệnh TB 2,1–3,35 ms (max
    20), nộp TB 1,3–1,6 (max 33), vẽ CPU TB 2,5–4,0 → vượt 8,3 ms của 120 Hz; nộp/chờ swapchain 25–33 ms cho thấy lúc đông **nghẽn cả GPU**.
    "vẽ khác" 100 khung, chép 68, nộp 11, nạp 7. → Cả ba việc của d+e đều đúng chỗ; đo lại sau khi lên bản d+e.
- Nạp trước sprite NPC khi vào map (`NAPNPC` "trễ 437") — việc đã ghi trong [[mobile-tongkim-lag-goc]].
- Sau khi bật nhịp PC mặc định: khi tick 10–15 ms xảy ra, `PaintSmooth=2` nội suy giúp mượt hơn (`cat ngang` thấp), nhưng không bù được khung mất.

## 6. Rủi ro

- Bản vá SDL chỉ nằm trong bản SDL của worktree này (git bỏ qua) — dựng APK ở worktree khác sẽ thiếu bộ đếm, pha 1 mất tác dụng.
  **Đã xảy ra 10/09 23:29 với APK `-m` của phiên giao diện (§8 mục 7).**
- Pha 1–5 đổi hành vi thật khi đang chơi (đó là mục đích); hết vòng tự trả về như cũ.
- Đổi số khung bay (pha 3, lúc kết thúc) làm SDL chờ hết hàng lệnh + dựng lại swapchain một lần.
- `POST /nhatky` không xác thực (chỉ trong LAN); chỉ ghi vào thư mục nhật ký, tên tệp đã làm sạch (không thể thành `..`).

## 10. Hướng dẫn test bản 109111313 (BKG / PALBUF / FPSNGOAI / MAU) trên Fold 7 và lấy log

1. **Nhận bản:** mở app → bộ tải thấy `apk.txt` 109111313 → cài → mở lại → vào game. Điện thoại cùng Wi-Fi với `10.0.0.140`; log tự về
   `D:\jx1_android_log\SM-F966U1_<phiên>\` mỗi 10 s (dt_v4 đang `[DoNhip] GuiLog=1 Bat=0`, `PaintLog=1 AutoLog=1`). Không cần đổi `config.ini`.
2. **Màn đăng nhập / chọn nhân vật:** dòng góc phải sẽ hiện ~30 FPS — đúng ý (ngoài thế giới vẽ 30, màn 60 Hz). Vào thế giới phải trở lại nấc đã chọn
   (Tự động = 120); nếu vào thế giới mà vẫn 30 → báo tôi.
3. **Bài 1 (15 phút, cảnh yên, màn trong):** đứng yên 2 phút, đi lại, mở/đóng hành trang, chat vài dòng, **gập/mở máy 1 lần, thoát ra màn hình chính rồi
   quay lại app 1 lần**. Để ý ba thứ: (a) màu sắc nhân vật / NPC / giao diện có y như trước không (bảng màu); (b) có lúc nào **hình đứng** — thao tác mà
   hình không đổi, hoặc sau gập/mở / quay lại app hình cũ không cập nhật quá nửa giây; (c) khựng lặt vặt còn không.
4. **Bài 2 (15 phút, Tống Kim / chỗ đông, màn ngoài):** đánh bình thường; để ý khựng khi đám đông tới, màu sắc, máy nóng hơn hay mát hơn bản trước.
5. **Thấy lỗi:** nói rõ *lỗi gì, lúc mấy giờ, màn trong hay ngoài*. Tắt nhanh từng mục không cần APK: sửa `D:\jx1_android_data_dt_v4\config.ini` mục
   `[Client]` thêm `Rep3BoKhungGiong=0` (hình đứng) / `Rep3PalBuffer=0` (sai màu) / `FpsNgoaiTheGioi=0` (nhịp ngoài thế giới), khởi động lại máy chủ 8765
   (manifest sinh lúc khởi động), mở lại app.
6. **Tôi đọc log** (`scratchpad/phan_tich_tong.py`, `bang_ve.py` + grep): `jx_rep3.log` `[VE-BKG]` (trình chiếu / bỏ mỗi 30 s — cảnh yên kỳ vọng bỏ ≥ 50 %),
   `[VE] … bang mau kieu storage buffer`, số dòng `[VE-GIAT]` có "chép" ≥ 10 ms (kỳ vọng ≈ 0, trước 12/phút), `[VE]` cho/chép/ghi/nộp; `jx_thietbi.log`
   `[MAU]` W, nhiệt, `gpu=`, `cpu_mhz=`, `[GPU-SYS]`; `jx_paint.log` `[SUM]`. Mốc so sánh: bảng 1.1/1.2 của phương án (yên màn trong 120 Hz: 2,1–2,8 W,
   CPU 48–68 %, pin 13,6 %/giờ; 12 khung chép chậm/phút).
7. **Bản 109111459 (D1):** mở lại app để nhận. Kiểm: (a) vào thế giới hình phủ đúng toàn màn, không méo, chữ đọc được; nếu thấy "mịn/mờ hơn" nói rõ; gập/mở
   máy một lần xem hình có cập nhật đúng cỡ không; (b) chơi Tống Kim 10–15 phút như bài 2. Tôi đọc `jx_rep3.log` dòng `[D1] swapchain WxH | backbuffer …
   | cua so … | SDL: tao …; extent min … max …` (Fold 7 kỳ vọng `1040x936` / `1436x616`), rồi so `[MAU]` `gpu=` / `gpu_mhz` / W và fps Tống Kim với phiên 14:16
   (GPU 82 % @ 652 MHz, 4,7 W, fps 109). Tắt nhanh nếu có vấn đề: `Rep3SwapchainLogic=0` trong `[Client]` của config dt_v4 + khởi động lại 8765; muốn nét hơn
   thử `Rep3SwapchainLogic=150`.
8. **Bản 109111545 (đợt C bước 1):** mở lại app để nhận. Kiểm **màu sắc** trước hết — trạng thái tầng texture giờ đi theo đỉnh nên nếu sai sẽ lộ ngay:
   màu nhân vật, hiệu ứng kỹ năng, chữ, thanh máu, vật phẩm trong hành trang, ảnh nền đăng nhập. Rồi chơi Tống Kim 10–15 phút như bài 2.
   Tôi đọc `[VE]` (ghi lệnh TB), `[VE-GOP]` ("ps bang N muc", đổi trạng thái/khung, quad không gộp), `[MAU]` (W, gpu=, cpu_mhz), fps so với phiên 15:10
   (ghi lệnh 2,6 ms, đổi ps 837/khung, lệnh 1 818/khung, CPU 76/70 %). Sai màu → `Rep3PsBuffer=0`; nghi lệnh vẽ sai chỗ (hình méo/nhoè khối) →
   `Rep3BindRing=0`; cả hai → `Rep3PalBuffer=0`; sửa trong `[Client]` của config dt_v4 rồi khởi động lại 8765.


---

## 18:00 11/09 — Đọc log bước 1 `[CHUATLAS]`, và bước 2 `[CULLCPU]` (APK 109111759)

### Bước 1 (chữ vào atlas) **có chạy**, nhưng gần như vô ích trong thế giới

Phiên `SM-F966U1_20260911_173416` (APK 109111729), 24 cửa sổ `[VE-GOP]`:

| cửa sổ | ô atlas DEFAULT | ô atlas MANAGED | xin ô mới |
|---|---|---|---|
| 1 (menu) | 0 | **1** | 0 |
| 2 (đăng nhập) | 1 148 | **3** | 0 |
| 3 → 24 (trong thế giới) | 777 … 12 418 | **0** | 0 … 1 |

Đọc đúng là: **bốn texture MANAGED đã vào atlas thật** — đó là bốn bảng chữ (`KFontRes.cpp:84`, 512×512,
`D3DFMT_A4R4G4B4`, `D3DPOOL_MANAGED` khi `Rep3Ex=0`). Bộ đếm đặt lại mỗi kỳ 30 s, mà bảng chữ chỉ tạo **một lần** lúc
khởi động, nên các cửa sổ sau đọc 0 là đúng — không phải bị loại. `xin ô mới` gần như bằng 0 → không có chuyện nội dung
đổi liên tục phải cấp ô mới (rủi ro lớn nhất của bước này **không xảy ra**).

Nhưng lợi thì rất ít: 4 ô trên tổng **47 551 texture GPU**. Tỷ lệ vỡ lô không nhúc nhích —
`pipeline 11,6 % → 11,5 %`, `texture0 88,3 % → 88,4 %`. Bên PC bước (d) ăn đậm vì ở đó chữ bị vỡ lô 100–450 lần/khung;
bên mobile chữ **đã** được gộp bằng đường khác nên phần thắng còn lại nằm chỗ khác.

### Chỗ thật sự mất: 42 trang atlas + chế độ cull

Cửa sổ cuối (rất đông, Tống Kim):

```
texture/sampler 1 134 lần đổi/khung (đỉnh 2 221)  |  pipeline 273 lần/khung
quad không gộp: texture0 3 687 840 (88,4 %), pipeline 470 686 (11,3 %), không liên tiếp 3 631, còn lại 0
atlas kệ=1 trang 2048: 42 trang  |  gpu tex 47 551 (290 MB)
```

- **texture0 88,4 %** = mỗi trang atlas là **một texture riêng**; 42 trang → cứ nhảy trang là cắt lô. Đây là bài toán
  mà loạt `[MANG 09/09 f]` (commit `5311778b`) đã giải bên PC bằng atlas khối cố định + nhiều mảng gắn chết khe sampler.
- **pipeline 11,3 %** = khoá pipeline của đường SDL_GPU có `CULLMODE` ở bit 34 (`D3D9onGPUDev.cpp` `GetPipeline`).
  `KFont3` đặt `CULLMODE=CCW` cho chữ, sprite dùng `NONE` → hai pipeline khác nhau → cắt lô. **Đúng y nguyên nhân (e)
  bên PC** (commit `ac7d255b`).

### Bước 2 đã làm: `[CULLCPU 11/09]` — port (e) sang mobile

`android/va_nguon_android_cullcpu.py`, chỉ trong `#ifdef JX_ANDROID`, công tắc `[Client] Rep3CullCpu` (mặc định 1):

1. `GetPipeline` tách thành vỏ bọc quanh `GetPipelineCull(..., dwCull)` để ép được `CULL_NONE`.
2. `ComputeState`: lệnh 2D (đỉnh `XYZRHW`) + cull `CW/CCW` → nhớ chế độ cull rồi lấy pipeline `CULL_NONE`.
3. `DrawInternal`: cả ba đường ghi đỉnh (quad 4 đỉnh, fan, danh sách tam giác) tự bỏ tam giác sai chiều **trên CPU**
   bằng tích chéo màn hình, y hệt bản PC. Kết quả trên màn hình không đổi, chỉ khác là chữ nay nằm chung lô với sprite.
4. Thêm số đo: `cull cpu=N: giu A bo B` và **tách lý do "pipeline"** thành `fvf / topo / blend / cull / fill / rt / stride`
   trong `[VE-GOP]` — kỳ sau sẽ biết chính xác phần 11,3 % còn lại là gì.

**Thử máy ảo trước khi phát:** cài lên LDPlayer, vào tới Tống Kim đông (ảnh chụp) — tên nhân vật, bảng xếp hạng, chữ
tiếng Việt, số sát thương, chat đều hiện đủ; nếu chiều cull ngược thì **toàn bộ chữ sẽ biến mất**, nên đây là phép thử
dứt điểm. 59 FPS / CPU 24 % trên máy ảo.

### Sửa kèm: phiên iOS làm đứt liên kết bản Android

`git fetch` trước khi dựng thấy `origin/mobile-0809 = 513d62f8 [IOS 11/09]`. Gộp vào thì **APK không dựng được**:

```
FAILED: libRainbow.so
ld: error: undefined symbol: IID_IClientFactory
ld: error: undefined symbol: IID_IESClient
```

`[DONTRUNG 11/09]` gỡ hai định nghĩa GUID khỏi `Sources/MultiServer/Rainbow/IClient.cpp` vì trên iOS cả game là **một**
nhị phân nên chúng trùng với bản do `NetConnectAgent.cpp` sinh ra. Trên Android thì Rainbow là **`.so` riêng**
(`libRainbow.so`, nạp bằng `dlopen`) và **không** chứa `NetConnectAgent.cpp`, nên mất luôn định nghĩa. Tôi định nghĩa lại
đúng hai GUID đó, **rào `#ifdef __ANDROID__`**, giá trị y hệt bản gốc → iOS vẫn không trùng ký hiệu, Windows không đổi.

> **Nhắn phiên iOS:** trước khi gỡ một ký hiệu dùng chung, kiểm cả cấu trúc nhiều `.so` của Android
> (`android/CMakeLists.txt`: `Rainbow` và `Represent3` là `SHARED`, nạp lúc chạy).

### Bản 109111759 — cần thử gì

APK `109111759`, md5 `1b6419045abac2d66a4de33d0b7b0182`, 20 371 455 B, đã lên `dt_v4` 17:59, máy chủ 8765 PID 385824,
`config.ini` của dt_v4 thêm `Rep3CullCpu=1`.

1. Mở lại app để nhận bản. **Kiểm chữ trước hết**: tên nhân vật trên đầu, chat, số sát thương, chữ trong hành trang,
   bảng xếp hạng Tống Kim. Mất chữ hoặc chữ nhấp nháy → `Rep3CullCpu=0` trong `[Client]` của config dt_v4 + khởi động lại
   8765 là về như cũ ngay, không cần APK.
2. Chơi Tống Kim 10–15 phút như bài 2 (chỗ đông, màn ngoài).
3. Tôi đọc `[VE-GOP]`: `cull cpu=1: giu A bo B` (B nhỏ là bình thường), `pipeline vo: … cull K` (**K phải tụt về gần 0**),
   `quad không gộp` phần `pipeline` (kỳ vọng 470 686 → còn vài %), `lệnh vẽ/khung`, rồi `[MAU]` W / `gpu=` / `cpu_mhz` và fps.

### Bước 3 kế tiếp (chưa làm)

Port `[MANG 09/09 f]` (`5311778b`): atlas theo **khối cố định** + nhiều mảng gắn chết khe sampler → **texture0 88,4 %**.
Đây mới là phần lớn. Không lặp lại sai lầm của `[MANG 11/09]` (C1, đã tắt): C1 dựng cụm tăng dần 2/4/8 lớp với **một**
sampler mảng nên vẫn đổi binding; bản PC dùng khối lớn cố định + nhiều mảng gắn chết nên **không bao giờ đổi binding**.
Trước khi viết sẽ đo giới hạn thiết bị (số khe sampler, số lớp tối đa của texture mảng) để không đánh cược.

### Ràng buộc đã đo cho bước 3 (đo trước, chưa viết mã)

| điều cần biết | số đo | nguồn |
|---|---|---|
| khe texture+sampler cho tầng điểm ảnh | **16** | `MAX_TEXTURE_SAMPLERS_PER_STAGE`, `SDL3-3.2.30/src/gpu/SDL_sysgpu.h:29` |
| đang dùng | 2 (`g_t0`, `g_t1`) khi `Rep3PalBuffer=1` | `D3D9onGPUDev.cpp` `num_samplers = 2` |
| còn trống cho mảng atlas | **14** | |
| bit trống trong ô PALROW cho lớp atlas | 6 (bit 25..30), bit 31 còn trống | `DrawInternal` |
| trang atlas cuối phiên đông | **56 trang, 464 MB**, tất cả 2 byte/điểm ảnh | `[GPU] atlas 56 trang (464 MB)` |

Nghĩa là bản mobile **không** chép được nguyên hình bản PC: PC xài 32 khe sampler (16 cho `R8G8` + 16 cho `BGRA8`)
vì D3D11 cho tới 128 khe, còn SDL_GPU chốt cứng 16 khe mỗi tầng. Bù lại mobile chỉ có **một** họ định dạng
(mọi trang đều 2 byte/điểm ảnh) nên 14 khe là thừa. Hình dạng dự kiến: khối cố định **8 lớp × 2048² × 2 B = 64 MB**,
7 khối phủ được 56 trang hiện tại, PALROW đủ 6 bit cho 8 khối × 8 lớp = 64 trang (cần hơn thì mở bit 31 thành 7 bit).

Điểm chết người của C1 (`[MANG 11/09]`, đã tắt) là **cụm lớn dần thì phải chép mảng cũ sang mảng mới** — đó là lý do
`nộp` vọt 0,64 → 3,28 ms và `vẽ CPU` 2,60 → 4,51 ms, chứ không phải do lấy mẫu theo lớp. Bản PC `(f)` ghi rõ
"khong chep mang khi lon, khong cap du". Khối cố định bỏ hẳn cú chép đó, nên đây là chỗ khác biệt phải giữ đúng.


---

## 18:30 11/09 — Đo bước 2 `[CULLCPU]`: phiên `SM-F966U1_20260911_180949`

APK 109111759, 12 phút chơi, 32 cửa sổ. So với phiên 17:34 (bản `[CHUATLAS]`), lấy các cửa sổ **cùng độ đông**
(2 400–3 500 quad/khung) cho công bằng:

| | 17:34 `[CHUATLAS]` | 18:09 `[CULLCPU]` | đổi |
|---|---|---|---|
| đổi pipeline / khung | 345 | **3** | −99,1 % |
| vỡ lô vì pipeline | 10,5 % | **0,05 %** | hết hẳn |
| vỡ lô vì `cull` (số tách mới) | — | **0** | mục tiêu đạt |
| lệnh / quad | 0,987 | **0,871** | −11,8 % |
| ghi lệnh | 2,49 ms | **2,19 ms** | −12 % |
| ghi lệnh / quad | 0,981 µs | **0,784 µs** | −20 % |
| khung việc > 16 ms (cả kỳ) | 6 | **1** | |
| fps | 109,9 | 110,5 | ngang |
| GPU (mẫu ≥ 60 %) | 77,8 % @ 486 MHz | 76,6 % @ 468 MHz | ngang |
| điện | 3,67 W | 3,63 W | ngang |
| nhiệt (đỉnh) | 3 | 3 | ngang |

Ghi lệnh giảm đều ở **mọi** mức tải, không riêng cảnh đông: cửa sổ "vừa" (≈1 450 quad) 2,01 → 1,57 ms (−22 %).

### Ba điều log nói thẳng

**1. Cái định sửa đã sửa xong.** `pipeline vo: … cull 0` ở cả 25 cửa sổ. Phần "pipeline" còn sót chỉ là
`topo` (≈3 500 lần/kỳ, khác kiểu hình học nên không gộp được, đúng bản chất) và `blend` (0–54 lần). Không còn gì để
vắt ở nhánh này.

**2. `nộp` tăng 0,39 → 1,41 ms, nhưng đó là CHỜ chứ không phải việc.** Hai gáo "chờ" đều phình
(`chờ lệnh+swapchain` 0,07 → 0,58 ms), trong khi gáo "việc" (`ghi lệnh`) teo lại; fps, tải GPU, điện, nhiệt y nguyên.
CPU nay tới chỗ trình chiếu sớm hơn nên nằm chờ vsync lâu hơn. Đối chứng: bản C1 hỏng (16:21) có `nộp` 3,9 ms **kèm**
`ghi lệnh` 3,0 ms và fps tụt — dạng khác hẳn.

**3. Bộ cull trên CPU chưa từng bỏ một tam giác nào.** `bo 0` trên **~1,5 tỷ** tam giác đã thử suốt 12 phút
(menu, đăng nhập, Tống Kim). Nghĩa là trạng thái `CULLMODE=CCW` mà `KFont3` đặt cho chữ **chưa bao giờ thật sự cắt gì**
trên bản mobile — chỉ ép `CULL_NONE` là đủ, phép thử tích chéo là phần thừa. Vẫn giữ vì nó rẻ hơn phần thắng
(`ghi lệnh` đã giảm ròng dù có nó) và nó là cái bảo hiểm nếu về sau có sprite lật. Ghi lại đây để sau muốn vắt thêm
0,1–0,2 ms thì biết chỗ.

### Còn lại đúng một mục tiêu

`vỡ lô vì texture0` giờ chiếm **99,9 %** (trước 89 %), đổi texture 1 653–1 941 lần/khung (đỉnh 3 492),
atlas cuối phiên **56 trang 464 MB**. Toàn bộ phần thắng còn lại nằm ở bước 3 (atlas khối cố định + nhiều mảng gắn chết
khe sampler). Ràng buộc đã đo ở mục trên: 14 khe sampler còn trống, một họ định dạng, khối 8 lớp × 64 MB, 7 khối phủ đủ.


---

## 18:50 11/09 — Bước 3 `[KHOI]`: atlas theo khối cố định (APK 109111847)

Chủ dặn "hãy dựa vào bản client pc để tham khảo", nên bước này chép thẳng cách làm của commit `5311778b`
(`[MANG 09/09 f]`) bên đường D3D11, chỉ đổi những chỗ **bắt buộc phải khác** vì SDL_GPU khác D3D11.

### Cách làm

Mỗi **khối** là một texture mảng 2D 8 lớp (mỗi lớp là một trang atlas 2048×2048). Khối được **gắn chết** vào một khe
sampler và **không bao giờ đổi trong cả khung**. Đỉnh mang chỉ số khối và lớp trong ô PALROW, nên hai quad nằm ở hai
trang atlas khác nhau vẫn gộp chung một lệnh vẽ.

Ô PALROW (32 bit mỗi đỉnh): `0..12` hàng bảng màu, `13..24` chỉ số tổ hợp trạng thái, `25..27` lớp, `28..30` khối,
`31` = ảnh lấy từ khối atlas.

Khác bản PC, và vì sao:

| | bản PC (D3D11) | bản mobile (SDL_GPU) | lý do |
|---|---|---|---|
| khe sampler cho khối | 32 (t3..t34) | 8 (khe 2..9) | SDL_GPU chốt cứng 16 khe mỗi tầng |
| khối mỗi định dạng | 16 | 8 dùng chung mọi định dạng | hệ quả của dòng trên |
| trang atlas | 1024² | 2048² | mobile vốn đã dùng 2048 |
| khối | 32 lớp R8G8 / 4 lớp BGRA8 | 8 lớp 2 byte / 4 lớp 4 byte | giữ đúng ngân sách ~64 MB một khối như PC |
| khe 0 | mảng | vẫn `sampler2D` | không ép texture thường thành mảng (C1 ép, đây là một khác biệt nữa) |
| khối rỗng trả VRAM | có | **chưa làm** | trang atlas mobile không bị huỷ lúc chạy; để sau nếu đo thấy tốn |

Hết 8 khối thì trang mới lùi về texture riêng như cũ, không lỗi, và đếm ở `het khoi`.

### Đã gỡ oan cho hướng này trước khi viết

C1 (`[MANG 11/09]`, đã tắt) cũng dùng texture mảng và làm fps tụt còn 75, nên trước khi viết tôi đo lại xem lỗi ở
**mảng** hay ở **cách dùng mảng**:

| | lệnh/quad | fps | GPU | nhiệt | thời lượng |
|---|---|---|---|---|---|
| C bước 1 (15:53) | 0,906 | 112,4 | 79,5 % @ 510 MHz | 2,74 | 28 phút |
| C1 atlas mảng (16:22) | 0,842 | **75,1** | 87,5 % @ **241 MHz** | **3,00** | **3,5 phút** |

C1 chạy **ngay sau** một phiên 28 phút, máy đã 37,6 °C, và suốt 3,5 phút đó nhiệt ở **mức 3 liên tục** với GPU bị hạ
xuống 241 MHz. Nói cách khác số của C1 là số của một máy đang bị bóp xung, không đủ để kết tội texture mảng. Và điểm
khác biệt thật sự: C1 cho **cụm lớn dần** (2, 4, 8 lớp) nên mỗi lần lớn là một cụm MỚI, các trang cũ nằm rải ở nhiều
texture → **vẫn phải đổi binding**. Khối cố định thì không bao giờ có chuyện đó. Bản PC ghi đúng câu này trong commit:
"khong chep mang khi lon, khong cap du".

### Thử máy ảo trước khi phát

| | trước (`[CULLCPU]`, điện thoại) | sau (`[KHOI]`, máy ảo) |
|---|---|---|
| đổi texture / khung | 1 653 – 1 941 (đỉnh 3 492) | **20 (đỉnh 27–33)** |
| đổi pipeline / khung | 3 | 3 |

Vào tới Tống Kim đông: nhân vật, hiệu ứng, tên, chữ, bảng xếp hạng, giao diện đều đúng; cảnh vắng cũng đúng.
Khối cấp ra như dự kiến: `#0` BGRA8 4 lớp 64 MB, `#1..` 2 byte 8 lớp 64 MB.

### Bản 109111847 — cần thử gì

APK `109111847`, md5 `2b70f9064bf3e79eb984d0c04fa7fd2c`, 20 436 991 B, lên `dt_v4` 18:50, máy chủ 8765 PID 393652,
config dt_v4 đặt `Rep3AtlasKhoi=1`.

1. Mở lại app để nhận bản. **Nhìn kỹ hình trước hết**: nhân vật và trang bị có đúng màu không, hiệu ứng kỹ năng, ảnh
   nền bản đồ, chữ, thanh máu, vật phẩm trong hành trang, ảnh màn đăng nhập. Sai hình → sửa `Rep3AtlasKhoi=0` trong
   `[Client]` của config dt_v4 rồi khởi động lại 8765, **không cần APK**.
2. Chơi Tống Kim 10–15 phút chỗ đông. Để ý **giật khi di chuyển màn hình** (đúng triệu chứng mà C1 gây ra) và máy có
   nóng hơn không.
3. Tôi đọc `[VE-GOP]`: `texture/sampler N` (kỳ vọng tụt từ ~1 700 về vài chục), `quad không gộp` phần `texture0`,
   `atlas khoi=1: N khoi (... MB), het khoi K` (K > 0 nghĩa là đã hết 8 khối, phải chia lại bit), rồi `[VE]` ghi lệnh /
   nộp và `[MAU]` W / `gpu=` / `cpu_mhz` / nhiệt, và fps.

Điểm cần canh nhất lần này là **bộ nhớ**: khối cấp trọn 8 lớp ngay cả khi mới dùng một trang, nên đỉnh bộ nhớ có thể
cao hơn bản cũ. Log in `atlas khoi=... MB` để theo dõi.


### 19:05 — Đo thật trên điện thoại: phiên `SM-F966U1_20260911_185508`

**Đính chính trước.** Khối 18:50 ở trên tôi so "1 653 → 20" là **so sai**: 1 653 là của điện thoại, 20 là của máy ảo,
hai máy khác nhau và hai cảnh khác nhau. Chủ nhắc đúng. Dưới đây là số so được: **cùng một điện thoại, cùng người
chơi, ghép theo số quad mỗi khung**.

| cửa sổ khớp mật độ | trước `[CULLCPU]` | sau `[KHOI]` |
|---|---|---|
| quad / khung | 2 344 | 2 207 |
| **đổi texture / khung** | **1 183** | **23** |
| đổi texture / quad | 0,505 | 0,010 |
| lệnh / quad | 0,892 | 0,380 |
| ghi lệnh | 1,91 ms | 0,48 ms |
| tổng (khoảng vẽ) | 4,37 ms | 3,26 ms |
| fps | 117,2 | 117,2 (chạm trần vsync) |

Cặp thứ hai, cảnh nhẹ hơn: 1 252 quad → đổi texture 601, ghi lệnh 1,56 ms; sau: 1 310 quad → đổi texture 23,
ghi lệnh 0,47 ms.

**Điều đáng nói nhất:** ở bản mới, số lần đổi texture **đứng yên ở 19–24 mỗi khung bất kể cảnh đông hay vắng**
(812 quad cũng 19, 2 207 quad cũng 23). Bản cũ thì nó bám theo số quad, cứ khoảng 0,5 lần đổi cho mỗi quad. Đó đúng
là điều mà "gắn chết khe sampler" hứa hẹn, và là bằng chứng cơ chế chạy đúng chứ không phải may.

Thiết bị (mẫu lúc GPU ≥ 60 %):

| | trước (512 mẫu) | sau (89 mẫu) |
|---|---|---|
| điện | 3,36 W | **2,55 W** |
| GPU | 77,4 % @ 420 MHz | 72,5 % @ **237 MHz** |
| nhiệt trung bình | 2,67 | 2,42 |

GPU chạy ở 237 MHz thay vì 420 MHz cho cùng mức tải: bộ điều tốc hạ xung vì không còn nhiều việc. Đây mới là con số
trả lời đúng câu hỏi gốc của chủ (nóng máy, hao pin), chứ không phải fps.

**Cảnh báo cần theo dõi:** hiện 7 khối, 448 MB, `het khoi 0`, 49 trang atlas đang dùng. Trần của 8 khối là khoảng
60 trang, mà phiên trước đã từng lên 56 trang. Nếu `het khoi` bắt đầu tăng thì trang mới sẽ lùi về texture riêng và
mất dần phần thắng. Lúc đó phải chia lại ô PALROW: trường chỉ số tổ hợp trạng thái đang cấp 12 bit mà thực tế chỉ
dùng 5 mức, cắt bớt là dư bit cho 16 khối.

Số mẫu của bản mới còn ít (8 cửa sổ, ~4 phút) nên cần chủ chơi thêm, nhất là Tống Kim lúc đông nhất.


### 19:17 — Tống Kim thật: 12 cửa sổ đông, 227 mẫu thiết bị

Chủ vào Tống Kim lúc 19:0x. Số đã đủ chắc (trước: 43 cửa sổ / 512 mẫu; sau: 12 cửa sổ / 227 mẫu), lấy cửa sổ
≥ 1 800 quad mỗi khung ở cả hai bên nên mật độ khớp nhau (2 396 so với 2 428 quad).

| | trước `[CULLCPU]` | sau `[KHOI]` | đổi |
|---|---|---|---|
| **đổi texture / khung** | 1 258 (đỉnh 2 677) | **49 (đỉnh 121)** | −96 % |
| lệnh / quad | 0,888 | **0,381** | −57 % |
| **ghi lệnh** | 2,11 ms | **0,59 ms** | −72 % |
| nộp | 1,47 ms | 2,03 ms | +0,56 (là chờ) |
| chờ lệnh + swapchain | 0,51 ms | 0,30 ms | |
| **tổng khoảng vẽ** | 4,35 ms | **3,20 ms** | −26 % |
| fps | 109,6 | 108,5 | ngang |
| **điện** | 3,36 W | **3,03 W** | −10 % |
| **GPU** | 77,4 % @ 420 MHz | 75,9 % @ **313 MHz** | xung −25 % |
| nhiệt trung bình | 2,67 (đỉnh 3) | **2,25** (đỉnh 3) | |
| máy | 38,6 °C | 38,0 °C | |

Đọc số này: **việc của CPU ở khâu vẽ giảm 72 %**, GPU chạy ở xung thấp hơn hẳn cho cùng mức tải, điện giảm 10 % và
nhiệt trung bình từ 2,67 xuống 2,25. fps không đổi vì đã chạm trần vsync và nút thắt còn lại nằm ngoài khâu vẽ.
Với câu hỏi gốc của chủ (nóng máy, hao pin, máy yếu chơi được không) thì đây mới là con số đáng kể, chứ không phải fps.

### Đã chạm trần 8 khối — việc cần làm tiếp

`atlas khoi=1: 8 khoi (8 lop/khoi, 512 MB), het khoi 8`: tám khối đã đầy, 8 trang atlas phải lùi về texture riêng.
Đó là lý do đổi texture nhích từ 23 lên 49 (đỉnh 121). Vẫn nhỏ so với 1 258, nhưng con số này sẽ tăng dần nếu chơi lâu.

Cách chữa đã rõ và không phải đoán: ô PALROW hiện cấp **12 bit** cho chỉ số tổ hợp trạng thái tầng texture
(bit 13..24) trong khi log nói suốt cả phiên chỉ dùng **5 mức** (`ps bang 5 muc, tran 0`). Cắt trường đó xuống 6 bit
là dư 6 bit, đủ cho 16 khối × 16 lớp = 256 trang, gấp bốn trần hiện tại. Chưa làm vì chủ đang đo bản này; làm xong
sẽ phải sinh lại SPIR-V và dựng lại cả hai bản.
